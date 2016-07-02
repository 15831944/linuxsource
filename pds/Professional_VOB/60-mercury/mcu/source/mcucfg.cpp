/*****************************************************************************
   ģ����      : mcu���ý��滯
   �ļ���      : mcucfg.cpp
   ����ļ�    : mcucfg.h
   �ļ�ʵ�ֹ���: MCU���ý��滯
   ����        : ������
   �汾        : V4.0  Copyright(C) 2005-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2005/08/23  4.0         ������        ����
******************************************************************************/

#include "evmcumcs.h"
#include "evmcuvcs.h"
#include "evmcumt.h"
#include "evmcueqp.h"
#include "evmcu.h"
#include "mcuvc.h"
#include "mcucfg.h"
#include "mtadpssn.h"
#include "eqpssn.h"
#include "mcsssn.h"
//#include "msmanagerssn.h"
//#include "mcuerrcode.h"
//#include "mcuver.h"
//#include "mcuagtstruct.h"

#include "commonlib.h"// [1/29/2010 xliang] 

//extern   u8 g_byCurEthUsed;
// MPC2 ֧��
#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
        #include "nipwrapper.h"
        //#include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
#include <unistd.h>		// [3/6/2010 xliang]
#else
    #include "brddrvlib.h"
#endif

#include "brdwrapperdef.h"

// BSP�ж���,WINDOWS��ģ��ʱʹ��
#ifdef WIN32

#pragma pack( push )
#pragma pack(1)

struct TBrdPosition
{
public:
    u8 byBrdID;         // ����ID�� 
    u8 byBrdLayer;      // �������ڲ��
    u8 byBrdSlot;       // �������ڲ�λ��
    
public:
	// pclint���δ���, �����޸�����ע��
	/*lint -save -sem(TBrdPosition::Clear,initializer)*/
    TBrdPosition(){ Clear();}
    void Clear(void) { memset( this, 0, sizeof(TBrdPosition) );  }
    void SetBrdId(u8 byID ) { byBrdID = byID;  }
    u8   GetBrdId(void) const { return byBrdID; }
    void SetBrdLayer(u8 byLayer ) { byBrdLayer = byLayer; }
    u8   GetBrdLayer(void) const { return byBrdLayer; }
    void SetBrdSlot(u8 bySlot) { byBrdSlot = bySlot; }
    u8   GetBrdSlot(void) const { return byBrdSlot; }
    BOOL32 IsSameBrd( TBrdPosition &tBrdPosition)
    {
        if ( tBrdPosition.GetBrdId() == byBrdID &&
            tBrdPosition.GetBrdLayer() == byBrdLayer &&
            tBrdPosition.GetBrdSlot() == byBrdSlot ) 
        {
            return TRUE;
        }
        return FALSE;
    }
    
};

#pragma pack( pop )
#endif


CMcuCfgApp	g_cMcuCfgApp;	

//nack
#define MCUCFG_INVALIDPARAM_NACK(cMsg, event, code)     \
    cMsg.SetMsgBody(NULL, 0);                           \
    cMsg.SetErrorCode(code);                            \
    SendMsg2Mcsssn(cMsg, event);                        \
	NEXTSTATE(STATE_IDLE);								\

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMcuCfgInst::CMcuCfgInst()
{
	memset( m_abyServMsgHead, 0, SERV_MSGHEAD_LEN );
}

CMcuCfgInst::~CMcuCfgInst()
{
}

void CMcuCfgInst::InstanceEntry( CMessage * const pcMsg )
{
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "CMcuCfgInst: The received msg's pointer in the msg entry is NULL!\n");
		return;
	}	
    
    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CFG, "receive msg %u(%s).\n", pcMsg->event, ::OspEventDesc( pcMsg->event ));

    switch(pcMsg->event)
    {
    //MCS����mcu ��cpuռ����
    case MCS_MCU_CPUPERCENTAGE_REQ:       
    case MCS_MCU_CPUPERCENTAGE_CMD:
        ProcCpuPercentage(pcMsg);
        break;

    //mcu�豸������Ϣ
    case MCS_MCU_CHANGEMCUEQPCFG_REQ:
    case MCS_MCU_GETMCUEQPCFG_REQ:
        ProcMcuEqpCfg(pcMsg);
        break;

    //����������Ϣ
    case MCS_MCU_CHANGEBRDCFG_REQ:
    case MCS_MCU_GETBRDCFG_REQ:
        ProcBrdCfg(pcMsg);
        break;
        
    //mcu������Ϣ��������
    case MCS_MCU_CHANGEMCUGENERALCFG_REQ:     
    case MCS_MCU_GETMCUGENERALCFG_REQ:
        ProcMcuGeneralCfg(pcMsg);
        break;

	//����telnet��½��Ϣ
	case MCS_MCU_UPDATETELNETLOGININFO_REQ:
		ProcUpdateTelnetLoginInfo(pcMsg);
		break;

    // ���ȡVmp�ϳɷ�����Ϣ
    case MCS_MCU_GETVMPSCHEMEINFO_REQ:
        ProcMcsGetVmpScheme(pcMsg);
        break;

	//4.6�汾 �¼� jlb
	// ȡHDUԤ��
    case MCS_MCU_GETHDUSCHEMEINFO_REQ:
		ProcMcsGetHduScheme( pcMsg );
        break;
	
	case MCS_MCU_CHANGEHDUSCHEMEINFO_REQ:
		ProcMcsChangeHduScheme( pcMsg );
        break;

	// ��������
    case MCS_MCU_REBOOTBRD_REQ:
        ProcRebootBrd(pcMsg);
        break;
        
    case MCS_MCU_UPDATEBRDVERSION_CMD:
	case MCS_MCU_STARTUPDATEDSCVERSION_REQ:
	case MCS_MCU_DSCUPDATEFILE_REQ:
        ProcUpdateBrdVersion( pcMsg );
        break;

	case AGT_SVC_STARTUPDATEDSCVERSION_ACK:
	case AGT_SVC_STARTUPDATEDSCVERSION_NACK:
		ProcStartUpdateDscVersionRsp( pcMsg );
		break;

	case AGT_SVC_DSCUPDATEFILE_ACK:
	case AGT_SVC_DSCUPDATEFILE_NACK:
		ProcDscUpdateFileRsp(pcMsg);
		break;

    // ����������ĵ���״̬    
    case AGT_SVC_BOARDSTATUS_NOTIFY:
    //�汾����״̬
    case AGT_SVC_UPDATEBRDVERSION_NOTIF:
	// ���µ�DSC��ע������
//	case AGT_SVC_NEWDSCREGREQ_NOTIFY:
        NotifyMcsBoardStatus(pcMsg);
        break;

	case AGT_SVC_SETDSCINFO_ACK:	// ����/�޸�DSC info�Ļ�Ӧ��Ϣ, zgc, 2007-07-16
	case AGT_SVC_SETDSCINFO_NACK:
		ProcSetDscInfoRsp(pcMsg);
		break;

	case AGT_SVC_DSCREGSUCCEED_NOTIF:	// ��DSC��ע��ɹ�, zgc, 2007-08-24
		ProcDscRegSucceedNotif(pcMsg);
		break;

    //���������л�ʱ,�µ����ð彫���滯������Ϣ����֪ͨ��ǰ���л��
    case MCU_MSSTATE_EXCHANGE_NTF:
        ProcMcuSynMcsCfgNtf();
        break;

	case MCUCFG_DSCCFG_WAITINGCHANGE_OVER_TIMER:
		ProcDscCfgWaitingChangeOverTimerOut();
		break;

	case MCSSSN_MCUCFG_MCSDISSCONNNECT_NOTIFY:
		ProcMcsDisconnectNotif( pcMsg );
		break;
	// vcs	
	case VCS_MCU_GETSOFTNAME_REQ:
    case VCS_MCU_CHGSOFTNAME_REQ:
		ProcVCSGeneralCfg( pcMsg );
		break;

	//agt->mcu: eth shift 
	case AGT_MCU_CFG_ETHCHANGE_NOTIF:
		NotifyMcsEqpCfg();
		break;

    default:
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[CfgSsn]: Wrong message %u(%s) received in InstanceEntry!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ) );
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
void CMcuCfgInst::DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp )
{
	if (NULL == pcMsg)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "McuCfg[DaemonInstanceEntry]: The received msg's pointer is NULL!");
		return;
	}

	switch (pcMsg->event)
	{
	case MCU_APPTASKTEST_REQ:			//GUARD Probe Message
		DaemonProcAppTaskRequest( pcMsg );
		break;
	default:
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "McuCfg[DaemonInstanceEntry]: Wrong message %u(%s) received! appId %u\n", 
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
void CMcuCfgInst::DaemonProcAppTaskRequest( const CMessage * pcMsg )
{
	post( pcMsg->srcid, MCU_APPTASKTEST_ACK, pcMsg->content, pcMsg->length );

	return;
}

/*=============================================================================
  �� �� ���� ProcRebootBrd
  ��    �ܣ� �������ĳ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void  
=============================================================================*/
void  CMcuCfgInst::ProcRebootBrd(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u8 byBrdIndex = *cServMsg.GetMsgBody();

    u8 byBrdNum = 0;
    TBoardInfo atBrdInfo[MAX_BOARD_NUM];
    u16 wRet = g_cMcuAgent.ReadBrdTable(&byBrdNum, atBrdInfo);
    if (SUCCESS_AGENT != wRet || 0 == byBrdNum || byBrdNum > MAX_BOARD_NUM)
    {
        LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcRebootBrd] read brd info failed! brd num :%d, wRet :%d\n", byBrdNum, wRet);
        SendMsg2Mcsssn(cServMsg, pcMsg->event+2);
        return;
    }

    u8 byIndex;
    for (byIndex = 0; byIndex < byBrdNum; byIndex++)
    {
        if (atBrdInfo[byIndex].GetBrdId() == byBrdIndex)
        {
            break;
        }
    }
    if (byIndex == byBrdNum)
    {
        LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcRebootBrd] invalid brd index :%d\n", byBrdIndex);
        SendMsg2Mcsssn(cServMsg, pcMsg->event+2);
        return;
    }

    wRet = g_cMcuAgent.RebootBoard(atBrdInfo[byIndex].GetLayer(), atBrdInfo[byIndex].GetSlot(), atBrdInfo[byIndex].GetType());
    if (SUCCESS_AGENT != wRet)
    {
        LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcRebootBrd] RebootBoard failed, ret :%d\n", wRet);
        SendMsg2Mcsssn(cServMsg, pcMsg->event+2);
        return;
    }

    SendMsg2Mcsssn(cServMsg, pcMsg->event+1);
    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CFG, "[ProcRebootBrd] RebootBoard success, Index:%d, layer:%d, slot:%d, type:%d\n", 
            atBrdInfo[byIndex].GetBrdId(), atBrdInfo[byIndex].GetLayer(), 
            atBrdInfo[byIndex].GetSlot(), atBrdInfo[byIndex].GetType());
    return;
}

/*=============================================================================
  �� �� ���� ProcUpdateBrdVersion
  ��    �ܣ� �������������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMcuCfgInst::ProcUpdateBrdVersion(const CMessage * const pcMsg )
{
	if ( pcMsg == NULL )
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG,  "[ProcUpdateBrdVersion] Msg is NULL, Error!\n" );
		return;
	}

	switch(pcMsg->event)
	{
	case MCS_MCU_UPDATEBRDVERSION_CMD:
		{
// #if defined(_LINUX_) && defined(_8KE_)
// 			CServMsg cServMsg(pcMsg->content, pcMsg->length);
// 			
// 			s8 *pchFileFullName = (s8*)(cServMsg.GetMsgBody()+sizeof(u8)+sizeof(u8));
// 			
// 			s8    achFileName[MAXLEN_MCU_FILEPATH] = {0};
// 			sprintf(achFileName, "%s/%s", DIR_FTP, KDV8KE_UPDATE_FILENAME);
// 			
// 			//У���ϴ���fullname�Ƿ���������Ҫ��fullnameһ��
// 			if( 0 != strcmp(pchFileFullName, achFileName) ||
// 				0 != access (achFileName, F_OK)	//�ļ������� 
// 				)
// 			{
// 				LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "the fullname \" %s \" passed by mcs is wrong when update!\n", pchFileFullName);
// 				cServMsg.SetMsgBody();
// 				u8 byRet = 0;		//0��ʧ��
// 				cServMsg.CatMsgBody(&byRet, sizeof(byRet));
// 				SendMsg2Mcsssn(cServMsg, MCU_MCS_UPDATEBRDVERSION_NOTIF);
// 				//note: ��8000E, ��Ϣ��ֻ��һ��u8
// 			}
// 			else
// 			{
// 				//���Կ�ʼ����
// 				// ------current dir: opt/mcu/    // [3/8/2010 xliang] need to confirm
// 				//1, ��ԭ��ҵ�����(������)ͳͳ�����ѹ��(���в����ļ���), ������ִ��Ȩ�޵�mcuorg.tar.gz
// 				s8   chCmdline[256] =  {0};   
// 				sprintf(chCmdline,  "cd /opt/mcu; tar czvf %s ./*; chmod a+x %s", 
// 					"mcuorg.tar.gz", "mcuorg.tar.gz");   //��opt/mcu������mcuorg.tar.gz
// 				system(chCmdline);
// 				
// 				//------current dir:
// 				//2, ���µ���������ļ��ƶ���opt/mcu��,û��Ҫ����ʡ
// // 				s8   chCmdline_mv[256] =  {0};
// // 				sprintf(chCmdline_mv, "cd /opt/mcu; mv ./%s/mcuupdate.bin ./; chmod a+x mcuupdate.bin", DIR_FTP);
// // 				system(chCmdline_mv);
// 				
// 				// �ڴ�ʱѯ��mcs����?
// 				
// 				//3, ����ļ���λ
// 				// 			s8    achProfileName[MAXLEN_MCU_FILEPATH] = {0};
// 				// 			sprintf(achProfileName, "%s/%s", DIR_DATA, RUNSTATUS_8KE_CHKFILENAME);
// 				
// 				// 			s32  dwRunSt = 0;
// 				// 			BOOL32 bRet = SetRegKeyInt( achProfileName, SECTION_RUNSTATUS, KEY_MCU8KE, dwRunSt );
// 				
// 				//4, ������������
// 				chdir((const char*)DIR_FTP);
// 				s8    achFile[MAXLEN_MCU_FILEPATH] = {0};
// 				sprintf(achFile, "./%s", KDV8KE_UPDATE_FILENAME);
// 				chmod(achFile, S_IREAD|S_IWRITE|S_IEXEC);
// 				system(achFile); 
// 
// 				//ѯ��mcs����
// 				cServMsg.SetMsgBody();
// 				u8 byRet = 1;		
// 				cServMsg.CatMsgBody(&byRet, sizeof(byRet));
// 				SendMsg2Mcsssn(cServMsg, MCU_MCS_UPDATEBRDVERSION_NOTIF);
// 				
// 			}
// 			
// 			break;
// #endif

			post( MAKEIID( AID_MCU_AGENT, 1 ), SVC_AGT_UPDATEBRDVERSION_CMD, pcMsg->content, pcMsg->length );
			break;
		}
	case MCS_MCU_STARTUPDATEDSCVERSION_REQ:
		{
			CServMsg cRevMsg(pcMsg->content, pcMsg->length);
			CServMsg cSendMsg;
			u8 byMcsSsnId = cRevMsg.GetSrcSsnId();
			u8 byBrdIdx = *(cRevMsg.GetMsgBody());
			u8 byFileNum = *(cRevMsg.GetMsgBody()+sizeof(u8));
			cSendMsg.SetMsgBody((u8*)&byBrdIdx, sizeof(byBrdIdx));
			cSendMsg.CatMsgBody((u8*)&byMcsSsnId, sizeof(byMcsSsnId));
			cSendMsg.CatMsgBody((u8*)&byFileNum, sizeof(byFileNum));
			cSendMsg.CatMsgBody((u8*)(cRevMsg.GetMsgBody()+sizeof(u8)*2), cRevMsg.GetMsgBodyLen()-2*sizeof(u8));
			post( MAKEIID( AID_MCU_AGENT, 1 ), SVC_AGT_STARTUPDATEDSCVERSION_REQ, cSendMsg.GetServMsg(), cSendMsg.GetServMsgLen() );
		}
		break;
	case MCS_MCU_DSCUPDATEFILE_REQ:
		{
			CServMsg cRevMsg(pcMsg->content, pcMsg->length);
			CServMsg cSendMsg;
			u8 byMcsSsnId = cRevMsg.GetSrcSsnId();
			u8 byBrdIdx = *(cRevMsg.GetMsgBody());
			cSendMsg.SetMsgBody((u8*)&byBrdIdx, sizeof(byBrdIdx));
			cSendMsg.CatMsgBody((u8*)&byMcsSsnId, sizeof(byMcsSsnId));
			cSendMsg.CatMsgBody((u8*)(cRevMsg.GetMsgBody()+sizeof(u8)), cRevMsg.GetMsgBodyLen()-sizeof(u8));
			post( MAKEIID( AID_MCU_AGENT, 1 ), SVC_AGT_DSCUPDATEFILE_REQ, cSendMsg.GetServMsg(), cSendMsg.GetServMsgLen() );
		}
		break;
	default:
		break;
	}
    
	return;
}

/*=============================================================================
�� �� ���� ProcStartUpdateDscVersionRsp
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage* pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/8/25   4.0			�ܹ��                  ����
=============================================================================*/
void CMcuCfgInst::ProcStartUpdateDscVersionRsp( const CMessage* pcMsg )
{
	CServMsg cRevMsg(pcMsg->content, pcMsg->length);
	u8 byBrdIdx = *(cRevMsg.GetMsgBody());
	u8 byMcsSsnId = *(cRevMsg.GetMsgBody()+sizeof(u8));
	CServMsg cSendMsg;
	cSendMsg.SetSrcSsnId(byMcsSsnId);
	cSendMsg.SetMsgBody( (u8*)&byBrdIdx, sizeof(byBrdIdx) );
	cSendMsg.CatMsgBody( (u8*)(cRevMsg.GetMsgBody()+sizeof(u8)*2), cRevMsg.GetMsgBodyLen()-sizeof(u8)*2 );
	if ( AGT_SVC_STARTUPDATEDSCVERSION_ACK == pcMsg->event )
	{
		SendMsg2Mcsssn(cSendMsg, MCU_MCS_STARTUPDATEDSCVERSION_ACK);
	}
	else
	{
		SendMsg2Mcsssn(cSendMsg, MCU_MCS_STARTUPDATEDSCVERSION_NACK);
	}
}

/*=============================================================================
�� �� ���� ProcDscUpdateFileRsp
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage* pcMsg
�� �� ֵ�� void  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/8/25   4.0			�ܹ��                  ����
=============================================================================*/
void  CMcuCfgInst::ProcDscUpdateFileRsp( const CMessage* pcMsg )
{
	CServMsg cRevMsg(pcMsg->content, pcMsg->length);
	u8 byBrdIdx = *(cRevMsg.GetMsgBody());
	u8 byMcsSsnId = *(cRevMsg.GetMsgBody()+sizeof(u8));
	CServMsg cSendMsg;
	cSendMsg.SetSrcSsnId(byMcsSsnId);
	cSendMsg.SetMsgBody( (u8*)&byBrdIdx, sizeof(byBrdIdx) );
	cSendMsg.CatMsgBody( (u8*)(cRevMsg.GetMsgBody()+sizeof(u8)*2), cRevMsg.GetMsgBodyLen()-sizeof(u8)*2 );
	if ( AGT_SVC_DSCUPDATEFILE_ACK == pcMsg->event )
	{
		SendMsg2Mcsssn(cSendMsg, MCU_MCS_DSCUPDATEFILE_ACK);
	}
	else
	{
		SendMsg2Mcsssn(cSendMsg, MCU_MCS_DSCUPDATEFILE_NACK);
	}
}

/*=============================================================================
  �� �� ���� ProcCpuPercentage
  ��    �ܣ� mcu ��cpuռ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void   
=============================================================================*/
void  CMcuCfgInst::ProcCpuPercentage(const CMessage * pcMsg)
{
	u8 byPercentage = g_cMcuAgent.GetCpuRate();
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	if(MCS_MCU_CPUPERCENTAGE_REQ == pcMsg->event)
	{
		SendMsg2Mcsssn(cServMsg, pcMsg->event+1);//ack
	}
	
	LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "cpu percentage :%d\n", byPercentage);
	
	//notify this mcs
	cServMsg.SetMsgBody(&byPercentage, sizeof(byPercentage));
	SendMsg2Mcsssn(cServMsg, MCU_MCS_CPUPERCENTAGE_NOTIF);
	
	return;
}

/*=============================================================================
  �� �� ���� ProcMcuEqpCfg
  ��    �ܣ� mcu�豸������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void    
=============================================================================*/
void  CMcuCfgInst::ProcMcuEqpCfg(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	u16 wRet = SUCCESS_AGENT;

	//get eqp cfg
	if (MCS_MCU_GETMCUEQPCFG_REQ == pcMsg->event)
	{
		SendMsg2Mcsssn(cServMsg, pcMsg->event+1);  //ack
	}
	//change eqp cfg
	else
	{
#if	defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
		
#ifdef _XLTest_ // [3/4/2010 xliang] remove this line later
		
#else			// [3/4/2010 xliang] remove this line later

		//����GK IP��MCU IP��ͬ������£�δ����IP��ͻ����mcu���ý����RASport
		//��Q931 port��Ϊ1919��1920[6/19/2012 chendaiwei]
		TGKProxyCfgInfo tGkPxyInfo;
		g_cMcuAgent.GetGkProxyCfgInfo(tGkPxyInfo);

		////////////////////////////////////////////////////////////
#ifdef _8KI_
		if (sizeof(TMcu8KiNetCfg) != cServMsg.GetMsgBodyLen())
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MCUEQP)
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuEqpCfg] mcueqp body is invalid!\n");
			return;
		}
		
		TMcu8KiNetCfg *ptMcuNetCfg = (TMcu8KiNetCfg *)cServMsg.GetMsgBody();
		
		if (g_bPrintCfgMsg)
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CFG, "[ProcMcuEqpCfg] receive changing config info as follow:\n");
			ptMcuNetCfg->Print();
		}
		//����GK IP��Sip Ip��MCU IP������ͬ������£�Ϊ����IP��ͻ����mcu���ý����RASport��Q931 port
		//��Sip port��Ϊ1919��1920��1720
		//g_cMcuAgent.AdjustNewMcuRasPort(ptMcuNetCfg->m_atMcuEqpCfg[0].GetMcuIpAddr(), tGkPxyInfo.GetGkIpAddr(), ptMcuNetCfg->GetSipIpAddr());
#else
		if (sizeof(TMcuNetCfg) != cServMsg.GetMsgBodyLen())
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MCUEQP)
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuEqpCfg] mcueqp body is invalid!\n");
			return;
		}
		
		TMcuNetCfg *ptMcuTmpNetCfg = (TMcuNetCfg *)cServMsg.GetMsgBody();
		
		if (g_bPrintCfgMsg)
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CFG, "[ProcMcuEqpCfg] receive changing config info as follow:\n");
			ptMcuTmpNetCfg->Print();
		}

		// Ϊ����8000H-M,IP��ַͳһ��eth1�� [pengguofeng 10/31/2012]
		// �����²�ԭ��: 8000H-M��ִ�аѵ�ǰ���ڵ�IPͬ������һ��������,�������������ͬ������һ������
		g_cMcuAgent.AdjustMcuRasPort(ptMcuTmpNetCfg->m_atMcuEqpCfg[1].GetMcuIpAddr(),tGkPxyInfo.GetGkIpAddr());

		TMcu8KiNetCfg *ptMcuNetCfg = NULL;
		ptMcuNetCfg->SetMcuEqpCfg(0,ptMcuTmpNetCfg->GetMcuEqpCfg(0));
		ptMcuNetCfg->SetInterface(ptMcuTmpNetCfg->GetInterface());
#endif

		TMcu8KECfg tMcu8KECfg;
		g_cMcuAgent.GetMcuEqpCfg(&tMcu8KECfg);

		tMcu8KECfg.SetUseModeType(ptMcuNetCfg->GetUseModeType());
		tMcu8KECfg.SetNetworkType(ptMcuNetCfg->GetNetworkType());
		tMcu8KECfg.SetIsUseSip(ptMcuNetCfg->IsUseSip());
		tMcu8KECfg.SetSipInEthIdx(ptMcuNetCfg->GetSipInEthIdx());
		tMcu8KECfg.SetSipIpAddr(ptMcuNetCfg->GetSipIpAddr());
		tMcu8KECfg.SetSipMaskAddr(ptMcuNetCfg->GetSipMaskAddr());
		tMcu8KECfg.SetSipGwAddr(ptMcuNetCfg->GetSipGwAddr());

		//for(u8 byLp = 0; byLp < MAXNUM_ETH_INTERFACE; byLp ++)
		//{

			TNetAdaptInfo *ptNetAdatpInfo = tMcu8KECfg.m_tLinkNetAdap.GetValue(0);
			if( ptNetAdatpInfo == NULL)
			{
				return;
			}
			u8 byRealIdx = ptNetAdatpInfo->GetAdaptIdx();

			u32 dwIpAddr	= ptMcuNetCfg->m_atMcuEqpCfg[byRealIdx].GetMcuIpAddr();
			u32 dwMaskAddr	= ptMcuNetCfg->m_atMcuEqpCfg[byRealIdx].GetMcuSubNetMask();
			u32 dwGwIpAddr	= ptMcuNetCfg->m_atMcuEqpCfg[byRealIdx].GetGWIpAddr();

			TNetParam tNetParam;
			tNetParam.SetNetParam(dwIpAddr, dwMaskAddr);
			
			TNetParam tGwParam;
			tGwParam.SetNetParam(dwGwIpAddr, dwMaskAddr);

			ptNetAdatpInfo->m_tLinkIpAddr.Delete(0);		//��ĳ���ڵĵ�0��ip ɾ��
			ptNetAdatpInfo->m_tLinkIpAddr.Insert(0, tNetParam);

			ptNetAdatpInfo->SetDefGWIpAddr(dwGwIpAddr);		
		//}

		g_cMcuAgent.Save8KENetCfgToSys(&tMcu8KECfg);

#endif  // _XLTest_  // [3/4/2010 xliang] remove this line later

#else	//8000A �� 8000B�����߼�

#ifdef _MINIMCU_
		if ( STATE_IDLE == CurState() )
		{
			memcpy( m_abyServMsgHead, &cServMsg, SERV_MSGHEAD_LEN );
			NEXTSTATE(STATE_CFGING);
		}
		else
		{
			CServMsg cTempMsg;
			memcpy( &cTempMsg, m_abyServMsgHead, SERV_MSGHEAD_LEN );
			if ( cServMsg.GetSrcSsnId() != cTempMsg.GetSrcSsnId() )
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuEqpCfg] Mcu is cfging, waite a moment!\n");
				MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_CONFLICT_WITHOTHERMCS);
				return;
			}
		}
#endif

		if (sizeof(TMcuEqpCfg) != cServMsg.GetMsgBodyLen())
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MCUEQP)
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuEqpCfg] mcueqp body is invalid!\n");
			return;
		}

		TMcuEqpCfg *ptEqpCfg = (TMcuEqpCfg *)cServMsg.GetMsgBody();
		u8 byLocalLayer = 0;
		u8 byLocalSlot  = 0;
#ifndef _MINIMCU_
		if (0 == ptEqpCfg->GetMcuIpAddr())
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MCUEQP)
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuEqpCfg] mcu ip can't be 0!\n");
			return;
		} 
#endif		

		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CFG, "\n\nThe MpcIp: 0x%xd, McuIp: 0x%xd\n\n", 
			   ntohl(g_cMcuAgent.GetMpcIp()), ptEqpCfg->GetMcuIpAddr());

		g_cMcuAgent.GetLocalLayer(byLocalLayer);
		g_cMcuAgent.GetLocalSlot(byLocalSlot);
		if (g_cMSSsnApp.IsActiveBoard()  && 
			ptEqpCfg->GetLayer() == byLocalLayer && 
			ptEqpCfg->GetSlot() == byLocalSlot)
		{
			//[2011/02/11 zhushz] mcs�޸�mcu ipҪ����������Ч������mcs��mcu����
			if (ntohl(g_cMcuAgent.GetMpcIp()) != ptEqpCfg->GetMcuIpAddr()
				||ntohl(g_cMcuAgent.GetMaskIp()) != ptEqpCfg->GetMcuSubNetMask()
				||ntohl(g_cMcuAgent.GetGateway()) != ptEqpCfg->GetGWIpAddr()
				|| g_cMcuAgent.GetInterface() != ptEqpCfg->GetInterface()
 				|| !g_cMcuAgent.IsMcuConfiged())
			{
				if(g_cMSSsnApp.IsDoubleLink() && g_cMcuAgent.GetMpcDIpAddr() == ptEqpCfg->GetMcuIpAddr())
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_MCUEQP);
					LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuEqpCfg] New Active MPC IP<0x%x> conflict with slave MPC,return!\n",ptEqpCfg->GetMcuIpAddr());
					return;
				}

				//�����޸ı�־
				g_cMcuAgent.SetIsNetCfgBeModifed(TRUE);
				//�����µ���������
				TMcuNewNetCfg tMcuNewNetInfo;
				tMcuNewNetInfo.SetNewMpcIP(htonl(ptEqpCfg->GetMcuIpAddr()));
				tMcuNewNetInfo.SetNewMpcInterface(ptEqpCfg->GetInterface());
				tMcuNewNetInfo.SetNewMpcMaskIp(htonl(ptEqpCfg->GetMcuSubNetMask()));
				tMcuNewNetInfo.SetNewMpcDefGateWay(htonl(ptEqpCfg->GetGWIpAddr()));
				g_cMcuAgent.SetNewNetCfg(tMcuNewNetInfo);
				
			}
// 			if (ntohl(g_cMcuAgent.GetMpcIp()) != ptEqpCfg->GetMcuIpAddr()
// 				|| g_cMcuAgent.GetInterface() != ptEqpCfg->GetInterface()
// 				|| !g_cMcuAgent.IsMcuConfiged())
// 			{
// #ifndef _MINIMCU_
// //    			wRet = g_cMcuAgent.SetMpcIp(htonl(ptEqpCfg->GetMcuIpAddr()), ptEqpCfg->GetInterface());
// // 				if (SUCCESS_AGENT != wRet)
// // 				{
// // 					LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcMcuEqpCfg] set mcu ip addr failed, ret:%d, McuIp:0x%x!\n", 
// // 							wRet, ptEqpCfg->GetMcuIpAddr());
// // 				}
// 				g_cMcuAgent.SetMpcNewIp(htonl(ptEqpCfg->GetMcuIpAddr()), ptEqpCfg->GetInterface());
// 				
// #endif
// 			}
// 
// 			if (ntohl(g_cMcuAgent.GetMaskIp()) != ptEqpCfg->GetMcuSubNetMask()
// 				|| g_cMcuAgent.GetInterface() != ptEqpCfg->GetInterface()
// 				|| !g_cMcuAgent.IsMcuConfiged())
// 			{
// #ifndef _MINIMCU_
// 				wRet = g_cMcuAgent.SetMaskIp(htonl(ptEqpCfg->GetMcuSubNetMask()), ptEqpCfg->GetInterface());
// 				if (SUCCESS_AGENT != wRet)
// 				{
// 					LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcMcuEqpCfg] set mcu mask addr failed, ret:%d, McuMask:0x%x!\n", 
// 							wRet, ptEqpCfg->GetMcuSubNetMask());
// 				}
// #endif
// 			}
// 
// 			if (ntohl(g_cMcuAgent.GetGateway()) != ptEqpCfg->GetGWIpAddr()
// 				|| !g_cMcuAgent.IsMcuConfiged())
// 			{
// 				wRet = g_cMcuAgent.SetGateway(htonl(ptEqpCfg->GetGWIpAddr()));
// 				if (SUCCESS_AGENT != wRet)
// 				{
// 					LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcMcuEqpCfg] set mcu gateway addr failed, ret:%d, McuGW:0x%x!\n", 
// 							wRet, ptEqpCfg->GetGWIpAddr());
// 				}
// 			}
// 
// 			if( g_cMcuAgent.GetInterface() != ptEqpCfg->GetInterface() )
// 			{
// 				wRet = g_cMcuAgent.SetInterface( ptEqpCfg->GetInterface() );
// 				if (SUCCESS_AGENT != wRet)
// 				{
// 					LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcMcuEqpCfg] set mcu interface failed, ret:%d, Interface: %d!\n", 
// 							wRet, ptEqpCfg->GetInterface());
// 				}
// 			}
		}
		if (g_bPrintCfgMsg)
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CFG, "[ProcMcuEqpCfg] receive changing config info as follow:\n");
			ptEqpCfg->Print();
		}
#endif	// !8KE

		if (!g_cMcuAgent.IsMcuConfiged())
		{
			wRet = g_cMcuAgent.SetIsMcuConfiged( TRUE );
			if (SUCCESS_AGENT != wRet)
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_ISCONFIGED);
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuEqpCfg] mcu eqp write configflag failed. ret:%d!\n", wRet);
				return;
			}
		}

#ifdef _UTF8
		// ������� [pengguofeng 4/16/2013]
		if ( pcMsg->event == MCS_MCU_CHANGEMCUEQPCFG_REQ )
		{
			u8 byEncoding = 0;
			g_cMcuAgent.ReadMcuEncodingType(byEncoding);
			if ( byEncoding != emenCoding_Utf8 )
			{
				g_cMcuAgent.WriteMcuEncodingType(emenCoding_Utf8);
			}
		}
#endif
		SendMsg2Mcsssn(cServMsg, pcMsg->event+1);
		NEXTSTATE(STATE_IDLE);
	}
     
	//notify all mcs
	NotifyMcsEqpCfg();
	
	return;
}

/*=============================================================================
  �� �� ���� ProcBrdCfg
  ��    �ܣ� ����������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void    
=============================================================================*/
void  CMcuCfgInst::ProcBrdCfg(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	if (MCS_MCU_GETBRDCFG_REQ == pcMsg->event)
	{
		SendMsg2Mcsssn(cServMsg, pcMsg->event+1);    //ack   
	}
	//change brd cfg info
	else
	{
// #ifdef _8KE_
// 		// [1/27/2010 xliang] mcs�������Ϣ�������������ע���˴�ֱ�ӻ�ack
// 		SendMsg2Mcsssn(cServMsg, pcMsg->event+1);
// 		NotifyMcsBrdCfg();
// 		return;
// #endif

#ifdef _MINIMCU_
		if ( STATE_IDLE == CurState() )
		{
			memcpy( m_abyServMsgHead, &cServMsg, SERV_MSGHEAD_LEN );
			NEXTSTATE(STATE_CFGING);
		}
		else
		{
			CServMsg cTempMsg;
			memcpy( &cTempMsg, m_abyServMsgHead, SERV_MSGHEAD_LEN );
			if ( cServMsg.GetSrcSsnId() != cTempMsg.GetSrcSsnId() )
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CFG, "[ProcBrdCfg] Mcu is cfging, waite a moment!\n");
				MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_CONFLICT_WITHOTHERMCS);
				return;
			}
		}
#endif

		if ( !BrdCfgProcIntegration( cServMsg ) )
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[ProcBrdCfg] Brd cfg proc error!\n");
			return;
		}
		else
		{
#ifndef _MINIMCU_
		//ack
			SendMsg2Mcsssn(cServMsg, pcMsg->event+1);
#endif
		}

		// �������:��ϢΪchange��ʱ��Ҫ��mcuEncoding�������� [pengguofeng 4/16/2013]
		// ��������Ҳ�����Ͳ�����
#ifdef _UTF8
		if ( pcMsg->event == MCS_MCU_CHANGEBRDCFG_REQ )
		{
			u8 byEncodingType = 0;
			g_cMcuAgent.ReadMcuEncodingType(byEncodingType);
			if (  byEncodingType != emenCoding_Utf8 )
			{
				g_cMcuAgent.WriteMcuEncodingType(emenCoding_Utf8);
			}
		}
#endif
    }
	//notify all mcs
	NotifyMcsBrdCfg();

	return;
}
/*
void  CMcuCfgInst::ProcBrdCfg(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	u8 *pbyMsg = cServMsg.GetMsgBody();
	u16 wMsgLen = cServMsg.GetMsgBodyLen();
	u16 wRet = 0;

	u16 wBrdLen = 0;
	u16 wMixLen = 0;
	u16 wRecLen = 0;
	u16 wBasLen = 0;
	u16 wTWLen = 0;
	u16 wVmpLen = 0;
	u16 wPrsLen = 0;
	u16 wMtwLen = 0;

	u8 byBrdNum = 0;
	u8 byMixNum = 0;
	u8 byRecNum = 0;
	u8 byBasNum = 0;
	u8 byTWNum = 0;
	u8 byVmpNum = 0;
	u8 byPrsNum = 0;
	u8 byMtwNum = 0;

	BOOL32 bIsExistDsc = FALSE;
	u32 dwExpDscIp = 0; // ��¼DSC���ⲿIP, zgc, 2007-07-04

	s32 nIndex = 0;

	//get brd cfg info
	if (MCS_MCU_GETBRDCFG_REQ == pcMsg->event)
	{
		SendMsg2Mcsssn(cServMsg, pcMsg->event+1);    //ack   
	}
	//change brd cfg info
	else
	{
#ifdef _MINIMCU_
		// debug, zgc, 2007-07-03
		if ( STATE_IDLE == CurState() )
		{
			memcpy( m_abyServMsgHead, &cServMsg, SERV_MSGHEAD_LEN );
			NEXTSTATE(STATE_CFGING);
		}
		else
		{
			CServMsg cTempMsg;
			memcpy( &cTempMsg, m_abyServMsgHead, SERV_MSGHEAD_LEN );
			if ( cServMsg.GetSrcSsnId() != cTempMsg.GetSrcSsnId() )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] Mcu is cfging, waite a moment!\n");
				MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_CONFLICT_WITHOTHERMCS);
				return;
			}
		}
#endif

		TBrdCfgInfo *ptBrdCfg = NULL;
		TEqpMixerCfgInfo *ptMixerCfg = NULL;
		TEqpRecCfgInfo *ptRecCfg = NULL;
		TEqpTvWallCfgInfo *ptTWCfg = NULL;
		TEqpBasCfgInfo *ptBasCfg = NULL;
		TEqpVmpCfgInfo * ptVmpCfg = NULL;
		TPrsCfgInfo *ptPrsCfg = NULL;
		TEqpMTvwallCfgInfo *ptMtwCfg = NULL;

        
        //mpc������
        u8 byMpcIndex = 0;

		if (g_bPrintCfgMsg)
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] receive changing config info as follow:\n");
		}

		//brd info
		byBrdNum = *pbyMsg;
		wBrdLen = sizeof(u8) +byBrdNum*sizeof(TBrdCfgInfo);
		if (0 == byBrdNum || byBrdNum > MAXNUM_BOARD || wMsgLen < wBrdLen)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
			return;
		}
		else
		{
			ptBrdCfg = (TBrdCfgInfo *)(pbyMsg + sizeof(u8));
			for (nIndex = 0; nIndex < byBrdNum; nIndex++)            
			{
				if (g_bPrintCfgMsg)
				{
					ptBrdCfg[nIndex].Print();
				}  
                
                if(BRD_TYPE_MPC == ptBrdCfg[nIndex].GetType())
                {
                    byMpcIndex = ptBrdCfg[nIndex].GetIndex();
                }

				u8 bySlot, bySlotType;
				ptBrdCfg[nIndex].GetSlot(bySlot, bySlotType);
				if (ptBrdCfg[nIndex].GetLayer() > MAXNUM_LAYER || 
					bySlot > MAXNUM_SLOT || 
					0 == ptBrdCfg[nIndex].GetIpAddr())
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] brd info is invalid!\n");
					return;
				}

#ifdef _MINIMCU_
				// ֪ͨMCU���� DSC��IP, zgc, 2007-03-09
				u8 byBrdType = ptBrdCfg[nIndex].GetType();
				if( BRD_TYPE_DSC == byBrdType ||
					BRD_TYPE_MDSC == byBrdType ||
					BRD_TYPE_HDSC == byBrdType )
				{
					bIsExistDsc = TRUE;
					dwExpDscIp = ptBrdCfg[nIndex].GetIpAddr();
				}
#endif
			}
		}

		//mixer info
		byMixNum = *(pbyMsg + wBrdLen);
		wMixLen = sizeof(u8) + byMixNum*sizeof(TEqpMixerCfgInfo);
		if (byMixNum > MAXNUM_PERIEQP || wMsgLen-wBrdLen < wMixLen)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MIXER)
			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] mixer body is invalid!\n");
			return;
		}
		else if (byMixNum > 0)
		{
			ptMixerCfg = (TEqpMixerCfgInfo *)(pbyMsg + wBrdLen + sizeof(u8));
			for (nIndex = 0; nIndex < byMixNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptMixerCfg[nIndex].Print();
				}                

				if (ptMixerCfg[nIndex].GetEqpId() < MIXERID_MIN || 
					ptMixerCfg[nIndex].GetEqpId() > MIXERID_MAX || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptMixerCfg[nIndex].GetSwitchBrdIndex()) || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptMixerCfg[nIndex].GetRunningBrdIndex()))
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MIXER)
					LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] mixer info is invalid!\n");
					return;
				}

                //ֻ��8000B��mcu������mpc��������mixer
#ifndef _MINIMCU_
                if(ptMixerCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
                {
                    MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MIXER)
                    LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] only 8000B mcu can run mixer on mpc, mpc Index :%d!\n", byMpcIndex);
                    return;
                }
#endif
				//ֻ��8000B/C������MP���������MPC��Ϊת���壬zgc, 2007-09-06
#ifdef _MINIMCU_
				TMcuPerfLimit tPerfLimit = g_cMcuVcApp.GetPerfLimit();
				if ( !tPerfLimit.IsMpcRunMp() && ptMixerCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MIXER)
                    LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] mix%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptMixerCfg[nIndex].GetEqpId(), byMpcIndex);
                    return;
				}
#endif
			}
		}

		//rec info
		byRecNum = *(pbyMsg + wBrdLen + wMixLen);
		wRecLen = sizeof(u8) + byRecNum*sizeof(TEqpRecCfgInfo);
		if (byRecNum  > MAXNUM_PERIEQP || wMsgLen-wBrdLen-wMixLen < wRecLen)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_REC)
			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] rec body is invalid!\n");
			return;
		}
		else if (byRecNum > 0)
		{
			ptRecCfg = (TEqpRecCfgInfo *)(pbyMsg + wBrdLen + wMixLen + sizeof(u8));
			for (nIndex = 0; nIndex < byRecNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptRecCfg[nIndex].Print();
				}                

				if (ptRecCfg[nIndex].GetEqpId() < RECORDERID_MIN || 
					ptRecCfg[nIndex].GetEqpId() > RECORDERID_MAX || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptRecCfg[nIndex].GetSwitchBrdIndex()))
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_REC)
					LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] rec info is invalid!\n");
					return;
				}
				//ֻ��8000B/C������MP���������MPC��Ϊת���壬zgc, 2007-09-06
#ifdef _MINIMCU_
				TMcuPerfLimit tPerfLimit = g_cMcuVcApp.GetPerfLimit();
				if ( !tPerfLimit.IsMpcRunMp() && ptRecCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_REC)
                    LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] rec%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptRecCfg[nIndex].GetEqpId(), byMpcIndex);
                    return;
				}
#endif
			}
		}

		//tvwall info
		byTWNum = *(pbyMsg + wBrdLen + wMixLen + wRecLen);
		wTWLen = sizeof(u8) + byTWNum*sizeof(TEqpTvWallCfgInfo);
		if (byTWNum  > MAXNUM_PERIEQP || wMsgLen-wBrdLen-wMixLen-wRecLen < wTWLen)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_TVWALL)
			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] tvwall body is invalid!\n");
			return;
		}
		else if (byTWNum > 0)
		{
			ptTWCfg = (TEqpTvWallCfgInfo *)(pbyMsg + wBrdLen + wMixLen + wRecLen + sizeof(u8));
			for (nIndex = 0; nIndex < byTWNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptTWCfg[nIndex].Print();
				}                

				if (ptTWCfg[nIndex].GetEqpId() < TVWALLID_MIN || 
					ptTWCfg[nIndex].GetEqpId() > TVWALLID_MAX || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptTWCfg[nIndex].GetRunningBrdIndex()))
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_TVWALL)
					LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] tvwall info is invalid!\n");
					return;
				}
				//ֻ��8000B/C������MP���������MPC��Ϊת���壬zgc, 2007-09-06
#ifdef _MINIMCU_
				TMcuPerfLimit tPerfLimit = g_cMcuVcApp.GetPerfLimit();
				if ( !tPerfLimit.IsMpcRunMp() && ptTWCfg[nIndex].GetRunningBrdIndex() == byMpcIndex )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_TVWALL)
                    LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] tvwall%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptTWCfg[nIndex].GetEqpId(), byMpcIndex);
                    return;
				}
#endif
			}
		}

		//bas info
		byBasNum = *(pbyMsg + wBrdLen + wMixLen + wRecLen + wTWLen);
		wBasLen = sizeof(u8) + byBasNum*sizeof(TEqpBasCfgInfo);
		if (byBasNum  > MAXNUM_PERIEQP || wMsgLen-wBrdLen-wMixLen-wRecLen-wTWLen < wBasLen)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BAS)
			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] bas body is invalid!\n");
			return;
		}
		else if (byBasNum > 0)
		{
			ptBasCfg = (TEqpBasCfgInfo *)(pbyMsg + wBrdLen + wMixLen + wRecLen + wTWLen + sizeof(u8));
			for (nIndex = 0; nIndex < byBasNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptBasCfg[nIndex].Print();
				}

				if (ptBasCfg[nIndex].GetEqpId() < BASID_MIN || 
					ptBasCfg[nIndex].GetEqpId() > BASID_MAX || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptBasCfg[nIndex].GetRunningBrdIndex()) || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptBasCfg[nIndex].GetSwitchBrdIndex()))
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BAS)
					LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] bas info is invalid!\n");
					return;
				}

                //ֻ��8000B��mcu������mpc��������bas
#ifndef _MINIMCU_
                if(ptBasCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
                {
                    MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BAS)
                    LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] only 8000B mcu can run bas on mpc, mpc Index :%d!\n", byMpcIndex);
                    return;
                }
#endif
				//ֻ��8000B/C������MP���������MPC��Ϊת���壬zgc, 2007-09-06
#ifdef _MINIMCU_
				TMcuPerfLimit tPerfLimit = g_cMcuVcApp.GetPerfLimit();
				if ( !tPerfLimit.IsMpcRunMp() && ptBasCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BAS)
                    LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] bas%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptBasCfg[nIndex].GetEqpId(), byMpcIndex);
                    return;
				}
#endif
			}
		}

		//vmp info
		byVmpNum = *(pbyMsg + wBrdLen + wMixLen + wRecLen + wTWLen + wBasLen);
		wVmpLen = sizeof(u8) + byVmpNum*sizeof(TEqpVmpCfgInfo);
		if (byVmpNum  > MAXNUM_PERIEQP || wMsgLen-wBrdLen-wMixLen-wRecLen-wTWLen-wBasLen < wVmpLen)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_VMP)
			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] vmp body is invalid!\n");
			return;
		}
		else if (byVmpNum > 0)
		{
			ptVmpCfg = (TEqpVmpCfgInfo *)(pbyMsg + wBrdLen + wMixLen + wRecLen + wTWLen + wBasLen + sizeof(u8));
			for (nIndex = 0; nIndex < byVmpNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptVmpCfg[nIndex].Print();
				}

				if (ptVmpCfg[nIndex].GetEqpId() < VMPID_MIN || 
					ptVmpCfg[nIndex].GetEqpId() > VMPID_MAX || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptVmpCfg[nIndex].GetRunningBrdIndex()) || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptVmpCfg[nIndex].GetSwitchBrdIndex()))
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_VMP)
					LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] vmp info is invalid!\n");
					return;
				}

                //ֻ��8000B��mcu������mpc��������vmp
#ifndef _MINIMCU_
                if(ptVmpCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
                {
                    MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_VMP)
                    LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] only 8000B mcu can run vmp on mpc, mpc Index :%d!\n", byMpcIndex);
                    return;
                }
#endif
				//ֻ��8000B/C������MP���������MPC��Ϊת���壬zgc, 2007-09-06
#ifdef _MINIMCU_
				TMcuPerfLimit tPerfLimit = g_cMcuVcApp.GetPerfLimit();
				if ( !tPerfLimit.IsMpcRunMp() && ptVmpCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_VMP)
                    LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] vmp%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptVmpCfg[nIndex].GetEqpId(), byMpcIndex);
                    return;
				}
#endif
			}
		}

		//prs info
		byPrsNum = *(pbyMsg + wBrdLen + wMixLen + wRecLen + wTWLen + wBasLen + wVmpLen);
		wPrsLen = sizeof(u8) + byPrsNum*sizeof(TPrsCfgInfo);
		if (byPrsNum  > MAXNUM_PERIEQP || wMsgLen-wBrdLen-wMixLen-wRecLen-wTWLen-wBasLen-wVmpLen < wPrsLen)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_PRS)
			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] prs body is invalid!\n");
			return;
		}
		else if (byPrsNum > 0)
		{
			ptPrsCfg = (TPrsCfgInfo *)(pbyMsg + wBrdLen + wMixLen + wRecLen + wTWLen + wBasLen + wVmpLen + sizeof(u8));
			for (nIndex = 0; nIndex < byPrsNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptPrsCfg[nIndex].Print();
				}

				if (ptPrsCfg[nIndex].GetEqpId() < PRSID_MIN || 
					ptPrsCfg[nIndex].GetEqpId() > PRSID_MAX || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptPrsCfg[nIndex].GetRunningBrdIndex()) || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptPrsCfg[nIndex].GetSwitchBrdIndex()))
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_PRS)
					LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] prs info is invalid!\n");
					return;
				}

                //ֻ��8000B��mcu������mpc��������prs
#ifndef _MINIMCU_
                if(ptPrsCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
                {
                    MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_PRS)
                    LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] only 8000B mcu can run prs on mpc, mpc Index :%d!\n", byMpcIndex);
                    return;
                }
#endif
				//ֻ��8000B/C������MP���������MPC��Ϊת���壬zgc, 2007-09-06
#ifdef _MINIMCU_
				TMcuPerfLimit tPerfLimit = g_cMcuVcApp.GetPerfLimit();
				if ( !tPerfLimit.IsMpcRunMp() && ptPrsCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_PRS)
                    LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] prs%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptPrsCfg[nIndex].GetEqpId(), byMpcIndex);
                    return;
				}
#endif
			}
		}

		byMtwNum = *(pbyMsg + wBrdLen + wMixLen + wRecLen + wTWLen + wBasLen + wVmpLen + wPrsLen);
		wMtwLen = sizeof(u8) + byMtwNum*sizeof(TEqpMTvwallCfgInfo);
		if (byMtwNum > MAXNUM_PERIEQP || wMsgLen-wBrdLen-wMixLen-wRecLen-wTWLen-wBasLen-wVmpLen-wPrsLen < wMtwLen)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MTVWALL)
			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] mtw body is invalid!\n");
			return;
		}   
		else if (byMtwNum > 0)
		{
			ptMtwCfg = (TEqpMTvwallCfgInfo *)(pbyMsg + wBrdLen + wMixLen + wRecLen + wTWLen + wBasLen + wVmpLen + wPrsLen + sizeof(u8));
			for (nIndex = 0; nIndex < byMtwNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptMtwCfg[nIndex].Print();
				}

				if (ptMtwCfg[nIndex].GetEqpId() < VMPTWID_MIN || 
					ptMtwCfg[nIndex].GetEqpId() > VMPTWID_MAX || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptMtwCfg[nIndex].GetRunningBrdIndex()))
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MTVWALL)
					LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] mtw info is invalid!\n");
					return;
				}
				//ֻ��8000B/C������MP���������MPC��Ϊת���壬zgc, 2007-09-06
#ifdef _MINIMCU_
				TMcuPerfLimit tPerfLimit = g_cMcuVcApp.GetPerfLimit();
				if ( !tPerfLimit.IsMpcRunMp() && ptMtwCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MTVWALL)
                    LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] mtw%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptMtwCfg[nIndex].GetEqpId(), byMpcIndex);
                    return;
				}
#endif
			}
		}

// �����Ϣ�壬TDSCModuleInfo�ṹ��MCS_MCU_GETMCUGENERALCFG_REQ��Ϣ��ת�Ƶ�MCS_MCU_GETBRDCFG_REQ, zgc, 2007-03-15
#ifdef _MINIMCU_	
		T8000BDscMod tDscInfo = *(T8000BDscMod*)( pbyMsg + wBrdLen + wMixLen + wRecLen + wTWLen + wBasLen + wVmpLen + wPrsLen + wMtwLen );	
		TDSCModuleInfo tAgentDscInfo;
		DscInfoIn2Out( tDscInfo, tAgentDscInfo );
		if( g_bPrintCfgMsg )
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "T8000BDscMod:\n");
			tDscInfo.Print();
			LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "TDSCModuleInfo:\n");
			tAgentDscInfo.Print();
		}
		BOOL32 bIsRegDsc = FALSE;
		if ( g_cMcuAgent.IsRegedBoard( g_cMcuAgent.GetConfigedDscType(), 0, 0 ) )
		{
			bIsRegDsc = TRUE;
		}
#endif

		//change brd info        
		TBoardInfo atBrdInfo[MAXNUM_BOARD]; 
		BrdCfgIn2Out(ptBrdCfg, atBrdInfo, byBrdNum);
		wRet = g_cMcuAgent.WriteBrdTable(byBrdNum, atBrdInfo);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_BRD)
			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] write brd info failed, ret:%d!\n", wRet);
			return;
		}

		//change mixer info   
		TEqpMixerInfo atMixerInfo[MAXNUM_PERIEQP];
		if (NULL != ptMixerCfg)
		{
			MixerCfgIn2Out(ptMixerCfg, atMixerInfo, byMixNum);            
		}
		else
		{
			byMixNum = 0;
		}
		wRet = g_cMcuAgent.WriteMixerTable(byMixNum, atMixerInfo);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_MIXER)
			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] write mixer info failed, ret:%d!\n", wRet);
			return;
		}

		//change rec info
		TEqpRecInfo atRecInfo[MAXNUM_PERIEQP];
		if (NULL != ptRecCfg)
		{
			RecCfgIn2Out(ptRecCfg, atRecInfo, byRecNum);            
		}     
		else
		{
			byRecNum = 0;
		}
		wRet = g_cMcuAgent.WriteRecTable(byRecNum, atRecInfo);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_REC)
			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] write rec info failed, ret:%d!\n", wRet);
			return;
		}

		//change tvwall info
		TEqpTVWallInfo atTWInfo[MAXNUM_PERIEQP];
		if (NULL != ptTWCfg)
		{
			TWCfgIn2Out(ptTWCfg, atTWInfo, byTWNum);            
		}
		else
		{
			byTWNum = 0;
		}
		wRet = g_cMcuAgent.WriteTvTable(byTWNum, atTWInfo);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_TVWALL)
			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] write tvwall info failed, ret:%d!\n", wRet);
			return;
		}

		//change bas info
		TEqpBasInfo atBasInfo[MAXNUM_PERIEQP];
		if (NULL != ptBasCfg)
		{
			BasCfgIn2Out(ptBasCfg, atBasInfo, byBasNum);            
		}
		else
		{
			byBasNum = 0;
		}
		wRet = g_cMcuAgent.WriteBasTable(byBasNum, atBasInfo);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_BAS)
			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] write bas info failed, ret:%d!\n", wRet);
			return;
		}

		//change vmp info
		TEqpVMPInfo atVmpInfo[MAXNUM_PERIEQP];
		if (NULL != ptVmpCfg)
		{
			VmpCfgIn2Out(ptVmpCfg, atVmpInfo, byVmpNum);            
		}
		else
		{
			byVmpNum = 0;
		}
		wRet = g_cMcuAgent.WriteVmpTable(byVmpNum, atVmpInfo);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_VMP)

			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] write vmp info failed, ret:%d!\n", wRet);
			return;
		}

		//change prs info
		TEqpPrsInfo atPrsInfo[MAXNUM_PERIEQP];
		if (NULL != ptPrsCfg)
		{
			PrsCfgIn2Out(ptPrsCfg, atPrsInfo, byPrsNum);            
		}    
		else
		{
			byPrsNum = 0;
		}

        wRet = g_cMcuAgent.WritePrsTable(byPrsNum, atPrsInfo);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_PRS)
			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] write prs info failed, ret:%d!\n", wRet);
			return;
		}

		//change mtw info
		TEqpMpwInfo atMtwInfo[MAXNUM_PERIEQP];
		if (NULL != ptMtwCfg)
		{              
			MtwCfgIn2Out(ptMtwCfg, atMtwInfo, byMtwNum);            
		}
		else
		{
			byMtwNum = 0;
		}
		wRet = g_cMcuAgent.WriteMpwTable(byMtwNum, atMtwInfo);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_MTVWALL)

			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] write mtw info failed, ret:%d!\n", wRet);
			return;
		}

// �����Ϣ�壬TDSCModuleInfo�ṹ��MCS_MCU_GETMCUGENERALCFG_REQ��Ϣ��ת�Ƶ�MCS_MCU_GETBRDCFG_REQ, zgc, 2007-03-15
#ifdef _MINIMCU_
		if ( bIsExistDsc )
		{
			TDSCModuleInfo tLocalDscInfo;
			g_cMcuAgent.GetDscInfo( &tLocalDscInfo );
			u32 dwDscInnerIp = tLocalDscInfo.GetDscInnerIp();
			u32 dwMcuInnerIp = tLocalDscInfo.GetMcuInnerIp();
			u32 dwInnerIpMask = tLocalDscInfo.GetInnerIpMask();
			u32 dwNewCfgIp1 = 0;
			u32 dwNewCfgIp2 = 0;
			u32 dwNewCfgIpMask1 = 0;
			u32 dwNewCfgIpMask2 = 0;
			u32 dwNewCfgGWIp1 = 0;
			u8 byIpMaskBitNum = 0;
			u8 byCallAddrNum = tAgentDscInfo.GetCallAddrNum();
			u8 byMcsAccessAddrNum = tAgentDscInfo.GetMcsAccessAddrNum();
			u8 byLop = 0;
			u8 byLop1 = 0;
			u8 byLop2 = 0;

			TMINIMCUNetParamAll tNetParamAll;
			TMINIMCUNetParam tNetParam;
			
			if ( tAgentDscInfo.GetNetType() == NETTYPE_INVALID )
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
				LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] NetType INVALID!\n");
				return;
			}
			
			tAgentDscInfo.GetCallAddrAll( tNetParamAll );
			if ( !tNetParamAll.IsValid() )
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
				LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] DSC module ip num error! McsAccessAddrNum.%d, CallAddrNum.%d\n", byMcsAccessAddrNum, byCallAddrNum);
				return;
			}
			memset( &tNetParamAll, 0, sizeof(tNetParamAll) );
			tAgentDscInfo.GetMcsAccessAddrAll( tNetParamAll );
			if ( !tNetParamAll.IsValid() )
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
				LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] DSC module ip num error! McsAccessAddrNum.%d, CallAddrNum.%d\n", byMcsAccessAddrNum, byCallAddrNum);
				return;
			}
			// IP������Ϊ0
			if ( 0 == byMcsAccessAddrNum || MAXNUM_MCSACCESSADDR < byMcsAccessAddrNum 
				|| 0 == byCallAddrNum || ETH_IP_MAXNUM-1 < byCallAddrNum )
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
				LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] DSC module ip num error! McsAccessAddrNum.%d, CallAddrNum.%d\n", byMcsAccessAddrNum, byCallAddrNum);
				return;
			}

			tAgentDscInfo.GetMcsAccessAddrAll( tNetParamAll );
			// ���������ӵ�ַ�Ƿ�˴˳�ͻ, zgc, 2007-07-17
			// ���������ӵ�ַ�����ص�ַ�Ƿ���ͬһ������, zgc, 2007-08-23
			for ( byLop1 = 0; byLop1 < byMcsAccessAddrNum; byLop1++ )
			{
				if ( !tNetParamAll.GetNetParambyIdx( byLop1, tNetParam ) )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] Get McsAccessAddr.%d from new DSC module cfg error!\n", byLop1);
					return;
				}
				if ( !tNetParam.IsValid() )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] McsAccessAddr.%d is invalid!\n", byLop1);
					return;
				}
				
				dwNewCfgIp1 = tNetParam.GetIpAddr();
				dwNewCfgIpMask1 = tNetParam.GetIpMask();
				dwNewCfgGWIp1 = tNetParam.GetGatewayIp();
				// IPMASK���ܴ���30λ,����ȫ0,�Ҳ���0,1���
				if ( !IsIpMaskValid(dwNewCfgIpMask1) )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] Ip0x%x: IpMask is invalid!\n", dwNewCfgIp1);
					return;
				}
				// ����IP����ȫ0��ȫ1
				dwNewCfgIpMask1 = ~dwNewCfgIpMask1;
				if ( (dwNewCfgIp1&dwNewCfgIpMask1) == 0 
					|| (dwNewCfgIp1&dwNewCfgIpMask1) == dwNewCfgIpMask1 )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] subnet ip.0x%x can not be network or broadcast!\n", dwNewCfgIp1);
					return;
				}
				dwNewCfgIpMask1 = ~dwNewCfgIpMask1;
				//����IP������Ӧ����ͬһ����
				if ( tNetParam.IsWan()
					&& (dwNewCfgIp1&dwNewCfgIpMask1) != (dwNewCfgGWIp1&dwNewCfgIpMask1)
				   )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg]WAN ip.0x%x should be in same subnet with GWIp!\n", dwNewCfgIp1);
					return;
				}

				for ( byLop2 = byLop1+1; byLop2 < byMcsAccessAddrNum; byLop2++ )
				{
					if ( !tNetParamAll.GetNetParambyIdx( byLop2, tNetParam ) )
					{
						MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
						LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] Get McsAccessAddr.%d from new DSC module cfg error!\n", byLop2);
						return;
					}
					dwNewCfgIp2 = tNetParam.GetIpAddr();
					dwNewCfgIpMask2 = tNetParam.GetIpMask();
					if ( 
						(dwNewCfgIp1&dwNewCfgIpMask1) == (dwNewCfgIp2&dwNewCfgIpMask1) 
						|| (dwNewCfgIp1&dwNewCfgIpMask2) == (dwNewCfgIp2&dwNewCfgIpMask2) 
					   )
					{
						MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
						LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] Mcs access address ip.0x%x conflict to ip.0x%x!\n ",
								dwNewCfgIp1, dwNewCfgIp2);
						return;
					}
				}
			}

			// ������е�ַ�Ƿ�˴˳�ͻ(�����ڿ�IP), zgc, 2007-07-17
			memset( &tNetParamAll, 0, sizeof(tNetParamAll) );
			tAgentDscInfo.GetCallAddrAll(tNetParamAll);
			for ( byLop1 = 0; byLop1 < byCallAddrNum; byLop1++ )
			{
				if ( !tNetParamAll.GetNetParambyIdx(byLop1, tNetParam) )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] Get CallAddr.%d from new DSC module cfg error!\n", byLop1);
					return;
				}
				if ( !tNetParam.IsValid() )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] Ip.%d can not be 0!\n",byLop1);
					return;
				}

				dwNewCfgIp1 = tNetParam.GetIpAddr();
				dwNewCfgIpMask1 = tNetParam.GetIpMask();
				dwNewCfgGWIp1 = tNetParam.GetGatewayIp();			
				// IPMASK���ܴ���30λ,����ȫ0,�Ҳ���0,1���
				if ( !IsIpMaskValid(dwNewCfgIpMask1) )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] Ip0x%x: IpMask is invalid!\n", dwNewCfgIp1);
					return;
				}
				// ����IP����ȫ0��ȫ1
				dwNewCfgIpMask1 = ~dwNewCfgIpMask1;
				if ( (dwNewCfgIp1&dwNewCfgIpMask1) == 0 
					|| (dwNewCfgIp1&dwNewCfgIpMask1) == dwNewCfgIpMask1 )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] subnet ip.0x%x can not be network or broadcast!\n", dwNewCfgIp1);
					return;
				}
				dwNewCfgIpMask1 = ~dwNewCfgIpMask1;
				//����IP������Ӧ����ͬһ����
				if ( tNetParam.IsWan()
					&& (dwNewCfgIp1&dwNewCfgIpMask1) != (dwNewCfgGWIp1&dwNewCfgIpMask1)
				   )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg]WAN ip.0x%x should be in same subnet with GWIp!\n", dwNewCfgIp1);
					return;
				}

				for ( byLop2 = byLop1+1; byLop2 < byCallAddrNum; byLop2++ )
				{
					if ( !tNetParamAll.GetNetParambyIdx(byLop2, tNetParam) )
					{
						MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
						LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg]Get CallAddr.%d from new DSC module cfg error!\n",byLop2);
						return;
					}
					dwNewCfgIp2 = tNetParam.GetIpAddr();
					dwNewCfgIpMask2 = tNetParam.GetIpMask();
					if ( 
						(dwNewCfgIp1&dwNewCfgIpMask1) == (dwNewCfgIp2&dwNewCfgIpMask1) 
						|| (dwNewCfgIp1&dwNewCfgIpMask2) == (dwNewCfgIp2&dwNewCfgIpMask2) 
					   )
					{
						MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
						LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] Call address ip.0x%x conflict to ip.0x%x!\n ",
								dwNewCfgIp1, dwNewCfgIp2);
						return;
					}
				}
				if ( 
					(dwNewCfgIp1&dwNewCfgIpMask1) == (dwDscInnerIp&dwNewCfgIpMask1) 
					|| (dwNewCfgIp1&dwInnerIpMask) == (dwDscInnerIp&dwInnerIpMask) 
				   )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] Call address ip.0x%x conflict to ip.0x%x!\n ",
							dwNewCfgIp1, dwDscInnerIp);
					return;
				}
			}
		}
		else
		{
			if ( bIsRegDsc )
			{
				CServMsg cDisMsg;
				u8 byBrdLayer = 0;
				u8 byBrdSlot = 0;
				cDisMsg.SetMsgBody((u8*)&byBrdLayer, sizeof(byBrdLayer));
				cDisMsg.CatMsgBody((u8*)&byBrdSlot, sizeof(byBrdSlot));
				post( MAKEIID(AID_MCU_AGENT,1), SVC_AGT_DISCONNECTBRD_CMD, cDisMsg.GetServMsg(), cDisMsg.GetServMsgLen()  );
			}
		}
		
		CServMsg cDscInfoMsg;
		u8 byBrdLayer = 0;
		u8 byBrdSlot = 0;
		cDscInfoMsg.SetMsgBody((u8*)&byBrdLayer, sizeof(byBrdLayer));
		cDscInfoMsg.CatMsgBody((u8*)&byBrdSlot, sizeof(byBrdSlot));
		cDscInfoMsg.CatMsgBody((u8*)&tAgentDscInfo, sizeof(tAgentDscInfo));
		post( MAKEIID(AID_MCU_AGENT,1), SVC_AGT_SETDSCINFO_REQ, cDscInfoMsg.GetServMsg(), cDscInfoMsg.GetServMsgLen()  );

		SetTimer(MCUCFG_DSCCFG_WAITINGCHANGE_OVER_TIMER, 10000 );
		g_cMcuAgent.SetDscInfo( &tAgentDscInfo, FALSE );  //���ڷ�����MCS�����޸��ļ�

#endif
#ifndef _MINIMCU_
		//ack
		SendMsg2Mcsssn(cServMsg, pcMsg->event+1);
#endif
    }

	//notify all mcs
	NotifyMcsBrdCfg();

	return;
}
*/
/*=============================================================================
  �� �� ���� CheckBrdIndex
  ��    �ܣ� ����������а�򽻻��������Ϸ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TBrdCfgInfo * ptBrdCfg
             u8 byBrdNum
             u8 byBrdIndex  ��������
  �� �� ֵ�� BOOL32  
=============================================================================*/
BOOL32 CMcuCfgInst::CheckBrdIndex(TBrdCfgInfo * ptBrdCfg, u8 byBrdNum, u8 byBrdIndex)
{
    if (NULL == ptBrdCfg)
    {
        return FALSE;
    }

    for (u8 byIndex = 0; byIndex < byBrdNum; byIndex++)
    {
        if (ptBrdCfg[byIndex].GetIndex() == byBrdIndex)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*=============================================================================
�� �� ���� IsValidRRQMtadpIp
��    �ܣ� �Ƿ�Ϸ�����Э�������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TMcuNetCfgInfo &tNetCfgInfo
�� �� ֵ�� BOOL32  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/3/14  4.0			������                  ����
=============================================================================*/
BOOL32  CMcuCfgInst::IsValidRRQMtadpIp(TMcuNetCfgInfo &tNetCfgInfo)
{  
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	return TRUE;
#endif

    if ( (0 != tNetCfgInfo.GetGkIpAddr() && 0 == tNetCfgInfo.GetRRQMtadpIp()) || 
         (0 == tNetCfgInfo.GetGkIpAddr() && 0 != tNetCfgInfo.GetRRQMtadpIp()) )
    {
        LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[IsValidRRQMtadpIp] GKIpAddr :0x%x, RRQMtadpIpAddr :0x%x\n", 
            tNetCfgInfo.GetGkIpAddr(), tNetCfgInfo.GetRRQMtadpIp());
        return FALSE;
    }

    if (0 == tNetCfgInfo.GetRRQMtadpIp())
    {
        return TRUE;
    }

    u8 byBrdNum = 0;
    TBoardInfo atBrdInfo[MAX_BOARD_NUM];
    u16 wRet = g_cMcuAgent.ReadBrdTable(&byBrdNum, atBrdInfo);
    if (SUCCESS_AGENT != wRet || 0 == byBrdNum || byBrdNum > MAX_BOARD_NUM)
    {
        LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[IsValidRRQMtadpIp] read brd info failed! brd num :%d, wRet :%d\n", byBrdNum, wRet);
        return FALSE;
    }   

    for (u8 byIdx = 0; byIdx < byBrdNum; byIdx++)
    {
        switch(atBrdInfo[byIdx].GetType())
        {
        case BRD_TYPE_MPC/*DSL8000_BRD_MPC*/:
        case BRD_TYPE_CRI/*DSL8000_BRD_CRI*/:
        case BRD_TYPE_DRI/*DSL8000_BRD_DRI*/:
		case BRD_TYPE_16E1/*DSL8000_BRD_16E1*/:
        case BRD_TYPE_MDSC/*DSL8000_BRD_MDSC*/:
        case BRD_TYPE_HDSC/*DSL8000_BRD_HDSC*/:
// [pengjie 2010/12/7] IS22 ֧��
		case BRD_TYPE_IS22:
			
		//  [1/21/2011 chendaiwei] CRI2,DRI2,MPC2֧��
		case BRD_TYPE_CRI2:
		case BRD_TYPE_DRI2:
		case BRD_TYPE_MPC2:

            if ( atBrdInfo[byIdx].GetBrdIp() == tNetCfgInfo.GetRRQMtadpIp() )
            {
                if (BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ == atBrdInfo[byIdx].GetType() || BRD_TYPE_MPC2 == atBrdInfo[byIdx].GetType())
                {
                    if (0 != tNetCfgInfo.GetIsUseMpcStack())
                    {
                        return TRUE;
                    }                    
                }
                else
                {
                    return TRUE;
                }                
            }
            break;

        default:
            break;
        }        
    }

    return FALSE;
}

/*=============================================================================
  �� �� ���� ProcMcuGeneralCfg
  ��    �ܣ� mcu������Ϣ��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void   
=============================================================================*/
void  CMcuCfgInst::ProcMcuGeneralCfg(const CMessage * pcMsg)
{

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	u8 *pbyMsg  = cServMsg.GetMsgBody();
	u16 wMsgLen = cServMsg.GetMsgBodyLen();
	u16 wRet = 0;

	//get mcu general cfg
	if (MCS_MCU_GETMCUGENERALCFG_REQ == pcMsg->event)
	{        
		SendMsg2Mcsssn(cServMsg, pcMsg->event+1);   //ack    
	}
	//change mcu general cfg
	else
	{
#ifdef _MINIMCU_
		if ( STATE_IDLE == CurState() )
		{
			memcpy( m_abyServMsgHead, &cServMsg, SERV_MSGHEAD_LEN );
			NEXTSTATE(STATE_CFGING);
		}
		else
		{
			CServMsg cTempMsg;
			memcpy( &cTempMsg, m_abyServMsgHead, SERV_MSGHEAD_LEN );
			if ( cServMsg.GetSrcSsnId() != cTempMsg.GetSrcSsnId() )
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CFG, "[ProcMcuGeneralCfg] Mcu is cfging, waite a moment!\n");
				MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_CONFLICT_WITHOTHERMCS);
				return;
			}
		}
#endif

		// [6/3/2011 liuxu] TMultiManuNetAccess��sp1�¼ӵ�, Ϊ�˼��ݷ��, ������Ҫ��ȥ
		// �������ᱨ��������
		if (wMsgLen < (sizeof(TMcuGeneralCfg) - sizeof( TMultiManuNetAccess )))
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MCUGEN)
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuGeneralCfg] mcu general body is invalid!\n");
			return;
		}
        

#if defined(_8KH_) || defined(_8KI_) //8000H������Ϣ��ķ�ʽ��8000G��ͬ[2/18/2012 chendaiwei]

		//TODO:�ж�REboot������
		TMcuGeneralCfg tGenCfg;
		memcpy(&tGenCfg, pbyMsg, sizeof(TMcuGeneralCfg));
				
		//whether the changed cfg be in effect immediately
		u8 byInEffectNow = 0;
		byInEffectNow = *(u8*)( cServMsg.GetMsgBody() + sizeof(TMcuGeneralCfg) );
		
#else
		//vmp style,mcu local cfg,net cfg,qos cfg,net syn cfg,dcs cfg
		// [6/3/2011 liuxu] ��Ϣ����˳����:(TMcuGeneralCfg - TMultiManuNetAccess) + u8 + TMultiManuNetAccess
		TMcuGeneralCfg tGenCfg;
		memset(&tGenCfg, 0, sizeof(TMcuGeneralCfg));
		const u16 wOrigLen = sizeof(TMcuGeneralCfg) - sizeof( TMultiManuNetAccess );

		// ������TMcuGeneralCfg - TMultiManuNetAccess
		memcpy(&tGenCfg, pbyMsg, wOrigLen);
		
		//whether the changed cfg be in effect immediately
		u8 byInEffectNow = 0;
		byInEffectNow = *(u8*)( cServMsg.GetMsgBody() + sizeof(TMcuGeneralCfg) );

		// [6/3/2011 liuxu] Ϊ�˼��ݷ��, TMultiManuNetAccess�����Ƶ�����Ϣ���
		if ( wMsgLen >= sizeof(TMcuGeneralCfg) )
		{
			// ������
			const u16 wStartPos = wOrigLen + sizeof( u8 );
			memcpy( (u8*)&tGenCfg + wOrigLen, cServMsg.GetMsgBody() + wStartPos, sizeof(TMultiManuNetAccess));
		}

#endif
		TMultiEthManuNetAccess tMultiEthInfo[MCU_MAXNUM_ADAPTER];
		memset(tMultiEthInfo, 0, sizeof(tMultiEthInfo));
		u8 byEthNum = 0;

		TMcuNewGeneralCfg tNewGenCfg;
		tNewGenCfg.SetMcuGeneralCfg(&tGenCfg);
		if (wMsgLen > sizeof(TMcuGeneralCfg) + sizeof(u8))
		{
			u8 *pbyBuffer = cServMsg.GetMsgBody()+sizeof(TMcuGeneralCfg)+sizeof(u8);
			tNewGenCfg.SetGkRRQUsePwdFlag(*pbyBuffer);
			pbyBuffer += sizeof(u8);
			
			tNewGenCfg.SetGkRRQPassword( (LPCSTR)pbyBuffer);
			pbyBuffer += sizeof(tNewGenCfg.m_achRRQPassword);	
			
#if defined(_8KH_) || defined(_8KE_) || defined(_8KI_)
			u16 wOrgMsgLen = sizeof(TMcuGeneralCfg) + sizeof(u8)+sizeof(tNewGenCfg.m_achRRQPassword)	\
							+sizeof(tNewGenCfg.m_byIsGkUseRRQPwd);
			if (wMsgLen > wOrgMsgLen)
			{
				tNewGenCfg.SetDMZInfo((TProxyDMZInfo*)pbyBuffer);
				pbyBuffer += sizeof(TProxyDMZInfo);
				wOrgMsgLen += sizeof(TProxyDMZInfo);

				//ȡ����1������2��Ϣ
				if(wMsgLen > wOrgMsgLen)
				{
					byEthNum = (u8)*pbyBuffer;
					pbyBuffer += sizeof(u8);
					wOrgMsgLen += sizeof(u8);

					if( byEthNum < MCU_MAXNUM_ADAPTER )
					{
						memcpy(tMultiEthInfo, pbyBuffer, sizeof(TMultiEthManuNetAccess) * byEthNum);
						pbyBuffer += sizeof(TMultiEthManuNetAccess);
						wOrgMsgLen += sizeof(TMultiEthManuNetAccess);
					}
					else
					{
						MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MCUGEN)
						LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[ProcMcuGeneralCfg] receive ethnum is invalid!\n");
						return;
					}
				}
			}
#endif
		}

		if (g_bPrintCfgMsg)
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CFG, "[ProcMcuGeneralCfg] receive changing config info as follow:\n");
			tNewGenCfg.Print();
		}

		TAgentMcuGenCfg tAgentGenCfg;
		if (!GeneralCfgIn2Out(tNewGenCfg, tAgentGenCfg))
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MCUGEN)
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuGeneralCfg] mcu general param is invalid!\n");
			return;
		}
		
		//MCU��E164�����Ѵ��ڣ��ܾ� 
		if( g_cMcuVcApp.IsE164Repeated( tAgentGenCfg.m_tLocal.GetE164Num(),TRUE , FALSE)
			|| 0 != g_cMcuVcApp.GetOngoingConfIdxByE164(tAgentGenCfg.m_tLocal.GetE164Num()))
		{
            MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CONFE164_REPEAT)
            LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuGeneralCfg] MCU E164 %s repeated,refuse!\n",tAgentGenCfg.m_tLocal.GetE164Num());

			return;       
		}
		
        //��Ҫ�ڴ���Ϣǰ�ȴ�����������Ϣ!
        if (!IsValidRRQMtadpIp(tGenCfg.m_tNet))
        {
            MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_RRQMTADPIP)
            LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuGeneralCfg] mcu rrq mtadp addr is invalid!\n");
            return;
        }

		wRet = g_cMcuAgent.WriteVmpAttachTable(tAgentGenCfg.m_byVmpStyleNum, tAgentGenCfg.m_atVmpStyle);
		if (SUCCESS_AGENT != wRet)            
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_VMPSTYLE)
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuGeneralCfg] mcu general write vmp style failed. ret:%d!\n", wRet);
			return;
		}

		//notify vmp
		//CServMsg cVmpMsg;
		//cVmpMsg.SetEventId(MCU_VMP_SETSTYLE_CMD);
		//cVmpMsg.SetMsgBody((u8*)&tGenCfg.m_tVmpStyle, sizeof(tGenCfg.m_tVmpStyle));
		//g_cEqpSsnApp.BroadcastToAllPeriEqpSsn(MCU_VMP_SETSTYLE_CMD, cVmpMsg.GetServMsg(), cVmpMsg.GetServMsgLen());         

		//�Ƿ���gkע��
		BOOL32 bRegGk = FALSE;
		BOOL32 bNotifyMtadp = FALSE;
		TLocalInfo tOldLocal;
		wRet = g_cMcuAgent.GetLocalInfo(&tOldLocal);
		if (SUCCESS_AGENT != wRet)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[ProcMcuGeneralCfg] get old mcu local info failed, ret:%d!\n", wRet);
		}
		else
		{
			if (0 != strcmp(tOldLocal.GetAlias(), tAgentGenCfg.m_tLocal.GetAlias()) || 
				0 != strcmp(tOldLocal.GetE164Num(), tAgentGenCfg.m_tLocal.GetE164Num()))
			{
				bRegGk = TRUE;
			}

			if (tOldLocal.GetAudioRefreshTime() != tAgentGenCfg.m_tLocal.GetAudioRefreshTime() || 
				tOldLocal.GetVideoRefreshTime() != tAgentGenCfg.m_tLocal.GetVideoRefreshTime() || 
				tOldLocal.GetListRefreshTime()  != tAgentGenCfg.m_tLocal.GetListRefreshTime() )
			{
				bNotifyMtadp = TRUE;
			}
		}

		wRet = g_cMcuAgent.SetLocalInfo(tAgentGenCfg.m_tLocal);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_LOCALINFO)
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuGeneralCfg] mcu general write mcu local failed. ret:%d!\n", wRet);
			return;
		}

		//////////////////////////////////////////////////////////////////////////		
		if( tOldLocal.GetConfNameShowType() != tAgentGenCfg.m_tLocal.GetConfNameShowType() )
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcMcuGeneralCfg] old ConfNameShowType:%d newConfNameShowType:%d\n",
				tOldLocal.GetConfNameShowType(),tAgentGenCfg.m_tLocal.GetConfNameShowType()
				);
			u8 byShowType = tAgentGenCfg.m_tLocal.GetConfNameShowType();
			//�����1ʱ��ΪҪ������ǰ�Ĵ��룬��ǰ����2�ǻ������ƣ�3��mcu���ƣ���������
			byShowType += 1;
			CServMsg cCfgMsg;
			cCfgMsg.SetEventId( MCU_MT_MMCUCONFNAMESHOWTYPE_CMD );
			cCfgMsg.SetMsgBody( (u8*)&byShowType, sizeof(u8) );
			g_cMtAdpSsnApp.BroadcastToAllMtAdpSsn( MCU_MT_MMCUCONFNAMESHOWTYPE_CMD, cCfgMsg );
		}
		if( tOldLocal.GetAdminLevel() != tAgentGenCfg.m_tLocal.GetAdminLevel() )
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcMcuGeneralCfg] old AdminLevel:%d new AdminLevel:%d\n",
				tOldLocal.GetAdminLevel(),tAgentGenCfg.m_tLocal.GetAdminLevel()
				);
			u8 byNewAdminiLevel = tAgentGenCfg.m_tLocal.GetAdminLevel();

			CServMsg cCfgMsg;
			cCfgMsg.SetEventId( MCU_MT_CHANGEADMINLEVEL_CMD );
			cCfgMsg.SetMsgBody( (u8*)&byNewAdminiLevel, sizeof(u8) );
			g_cMtAdpSsnApp.BroadcastToAllMtAdpSsn( MCU_MT_CHANGEADMINLEVEL_CMD, cCfgMsg );		
		}
		if (g_bPrintCfgMsg)
		{
			wRet = g_cMcuAgent.GetLocalInfo(&tOldLocal);
			if( SUCCESS_AGENT == wRet )
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CFG, "[ProcMcuGeneralCfg] now print new local Info\n" );
				tOldLocal.Print();
			}
			else
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuGeneralCfg] g_cMcuAgent.GetLocalInfo error retVal:%d\n",wRet);
			}
		}
		//////////////////////////////////////////////////////////////////////////

		//����������ն���Ϣ����ʱ����
		if (bNotifyMtadp)
		{
			TMtAdpCfg tCfg;
			tCfg.SetAudInfoRefreshTime(tAgentGenCfg.m_tLocal.GetAudioRefreshTime());
			tCfg.SetVidInfoRefreshTime(tAgentGenCfg.m_tLocal.GetVideoRefreshTime());
			tCfg.SetPartListRefreshTime(tAgentGenCfg.m_tLocal.GetListRefreshTime());
			CServMsg cCfgMsg;
			cCfgMsg.SetEventId(MCU_MT_MTADPCFG_CMD);
			cCfgMsg.SetMsgBody((u8*)&tCfg, sizeof(tCfg));
			g_cMtAdpSsnApp.BroadcastToAllMtAdpSsn(MCU_MT_MTADPCFG_CMD, cCfgMsg);  
		}
        
		//���±�����Ϣ(����ע��gk)
		if (bRegGk)
		{
			g_cMcuVcApp.RegisterConfToGK( 0, g_cMcuVcApp.GetRegGKDriId() );
            OspPost( MAKEIID(AID_MCU_VC, CInstance::DAEMON), MCU_MCUREREGISTERGK_NOITF );
		}            
		
		//GK��ַ���߸��²��ԣ�
		//	  (1) �¾�GK��ַ����ͬ������Ҫ������Ч������GK��ַ��������
		//	  (2) ��mtadp���յ��µĵ�ַ��д�뱾����Ϣ�У�����ձ���״̬
		//	  (3) ��mtadp���յ��µĵ�ַ��ע�����ص�������Ϣ������MCU��������ע������
		u32 dwOldGkIp = g_cMcuAgent.GetGkIpAddr();
        u32 dwRRQMtadpIp = g_cMcuAgent.GetRRQMtadpIp();

        // guzh [9/8/2006] 
		// ֪ͨMTADP֮ǰһ��д����º������
		// �����˿ڲ�С���ն�MCU����ʼ�˿�, zgc, 2007-06-05
		if ( MT_MCU_STARTPORT > tAgentGenCfg.m_tNet.GetRecvStartPort() )
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_NET)
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuGeneralCfg] RecvStartPort value(%d) is error!\n", tAgentGenCfg.m_tNet.GetRecvStartPort() );
			return;
		}
		wRet = g_cMcuAgent.SetNetWorkInfo(tAgentGenCfg.m_tNet);
        
		if ( dwOldGkIp != htonl(tAgentGenCfg.m_tNet.GetGkIp()) ||
             dwRRQMtadpIp != tAgentGenCfg.m_tNet.GetRRQMtadpIp() )
		{
			//�Ƿ�������Ч	
			u32 dwGKIp = 0;
			if ( 1 == byInEffectNow )
			{
				//�������е�����壬����GK��ַ
				CServMsg cSendServMsg;
				cSendServMsg.SetEventId( MCU_MTADP_GKADDR_UPDATE_CMD );
				dwGKIp = tAgentGenCfg.m_tNet.GetGkIp();
				cSendServMsg.SetMsgBody( (u8*)&dwGKIp, sizeof(u32) );
                
                //zbq[07/17/2009] N+1����Ϊ��֤�ն˵���������ʵʱˢ�µ�ʧЧ�ԣ��˶���������
                if (MCU_NPLUS_MASTER_CONNECTED != g_cNPlusApp.GetLocalNPlusState())
                {
                    g_cMtAdpSsnApp.BroadcastToAllMtAdpSsn( MCU_MTADP_GKADDR_UPDATE_CMD, cSendServMsg );
                }
				
				//���MCU��ע��״̬
				g_cMcuVcApp.SetConfRegState( 0, 0 );
				g_cMcuVcApp.SetRegGKDriId( 0 );
            }

            // guzh [7/28/2007] �����Ƿ�������Ч��ҪдDSC����
			// �ô���ȥ�޸�DSC��GK info, zgc, 2007-07-21
#ifdef _MINIMCU_
			cServMsg.SetEventId( SVC_AGT_DSCGKINFO_UPDATE_CMD );
			dwGKIp = htonl(tAgentGenCfg.m_tNet.GetGkIp());
			u8 byLayer = 0;
			u8 bySlot = 0;
			cServMsg.SetMsgBody((u8*)&byLayer, sizeof(byLayer));
			cServMsg.CatMsgBody((u8*)&bySlot, sizeof(bySlot));
			cServMsg.CatMsgBody((u8*)&dwGKIp, sizeof(u32));
			post( MAKEIID(AID_MCU_AGENT,1), SVC_AGT_DSCGKINFO_UPDATE_CMD, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()  );
#endif
		}

		wRet |= g_cMcuAgent.WriteTrapTable(tAgentGenCfg.m_byTrapListNum, tAgentGenCfg.m_atTrapList);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_NET) 
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuGeneralCfg] mcu general write net failed. ret:%d!\n", wRet);
			return;
		}

		//�Ƿ����Qos��Ϣ
		BOOL32 bUpdateQos = FALSE;
		TQosInfo tOldQos;
		wRet = g_cMcuAgent.GetQosInfo(&tOldQos);
		if (SUCCESS_AGENT != wRet)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[ProcMcuGeneralCfg] get old qos info failed, ret:%d!\n", wRet);
		}
		else
		{
			if (0 != memcmp(&tOldQos, &tAgentGenCfg.m_tQos, sizeof(TQosInfo)))
			{
				bUpdateQos = TRUE;
			}
		}

		wRet = g_cMcuAgent.SetQosInfo(tAgentGenCfg.m_tQos);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_QOS)
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuGeneralCfg] mcu general write qos failed. ret:%d!\n", wRet);
			return;
		}

		//notify mtadp and eqp 
		if (bUpdateQos)
		{
			CServMsg cQosMsg;
			cQosMsg.SetEventId(MCU_MT_SETQOS_CMD);
			cQosMsg.SetMsgBody((u8*)&tGenCfg.m_tQos, sizeof(tGenCfg.m_tQos));
			g_cMtAdpSsnApp.BroadcastToAllMtAdpSsn(MCU_MT_SETQOS_CMD, cQosMsg);    

			cQosMsg.SetEventId(MCU_EQP_SETQOS_CMD);
			g_cEqpSsnApp.BroadcastToAllPeriEqpSsn(MCU_EQP_SETQOS_CMD, cQosMsg.GetServMsg(), cQosMsg.GetServMsgLen()); 
		}
        

		wRet = g_cMcuAgent.SetNetSyncInfo(tAgentGenCfg.m_tNetSyn);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_NETSYN)
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuGeneralCfg] mcu general write netsyn failed. ret:%d!\n", wRet);
			return;
		}

		wRet = g_cMcuAgent.SetDcsIp(tAgentGenCfg.m_dwDcsIp);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_DCS)
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuGeneralCfg] mcu general write dcs failed. ret:%d!\n", wRet);
			return;
		}

		//  [1/8/2010 pengjie] Modify
		wRet = g_cMcuAgent.SetEqpExCfgInfo( tAgentGenCfg.m_tEqpExCfgInfo );
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_EQPEXCFGINFO)
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuGeneralCfg] mcu general write EqpExCfgInfo failed. ret:%d!\n", wRet);
			return;
		}
		TPeriEqpStatus tHduStatus;
		u8 byVidPlayPolicy = tAgentGenCfg.m_tEqpExCfgInfo.m_tHDUExCfgInfo.m_byIdleChlShowMode;
		for (u8 byHduId=HDUID_MIN;byHduId<=HDUID_MAX;byHduId++)
		{
			if (g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus) &&
				tHduStatus.m_byOnline)
			{
				OspPost(MAKEIID(AID_MCU_PERIEQPSSN, byHduId), MCU_HDU_CHANGEPLAYPOLICY_NOTIF, (void*)&byVidPlayPolicy, sizeof(u8));
			}
		}
		// End Modify

#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)

		//дע��GK���ļ�
		u32 dwGKIp = tAgentGenCfg.m_tNet.GetGkIp();
		g_cMcuAgent.SetSipRegGkInfo(dwGKIp);
	
		wRet = g_cMcuAgent.SetProxyDMZInfo(tNewGenCfg.m_tProxyDMZInfo);
		if (SUCCESS_AGENT != wRet)
		{
			//TODO:������Ӧerrorcode
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_GKPROXYCFGINFO)
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuGeneralCfg] mcu general write DMZInfo failed. ret:%d!\n", wRet);
			return;
		}
		
		// Save Gk and proxy configuration to file
		wRet = g_cMcuAgent.SetGkProxyCfgInfo(tGenCfg.m_tGkProxyCfg);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_GKPROXYCFGINFO)
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuGeneralCfg] mcu general write GkProxyCfgInfo failed. ret:%d!\n", wRet);
			return;
		}
		
		LogPrint(LOG_LVL_DETAIL,MID_MCU_CFG,"[ProcMcuGeneralCfg] Mpc IP:0x%x, Gk IP:0x%x\n",ntohl(g_cMcuAgent.GetMpcIp()),tGenCfg.m_tGkProxyCfg.GetGkIpAddr());
		g_cMcuAgent.AdjustMcuRasPort(ntohl(g_cMcuAgent.GetMpcIp()),tGenCfg.m_tGkProxyCfg.GetGkIpAddr());

		// execute corresponding action according to the GkProxy configuration
		// No action at present, wait for reboot.
// 		if (tGenCfg.m_tGkProxyCfg.IsGkUsed())
// 		{
// 			
// 		}
// 		if (tGenCfg.m_tGkProxyCfg.IsProxyUsed())
// 		{
// 			
// 		}

		// Save Prs time span to file
		wRet = g_cMcuAgent.SetPrsTimeSpanCfgInfo(tGenCfg.m_tPrsTimeSpanCfg);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_EQPEXCFGINFO)
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuGeneralCfg] mcu general write PrsTimeSpanCfgInfo failed. ret:%d!\n", wRet);
			return;
		}


		//����Ӫ����������
		wRet = g_cMcuAgent.SetMultiManuNetCfg(tAgentGenCfg.m_tMultiManuNetCfg, tMultiEthInfo, byEthNum);

		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_MULTIMANUNET)
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuGeneralCfg] mcu general write PrsTimeSpanCfgInfo failed. ret:%d!\n", wRet);
			return;
		}

		// save Multi-net cfg
		g_cMcuAgent.SetRouteCfg(tGenCfg.m_tMultiNetCfg);

#endif

/*
#ifdef _MINIMCU_
        // ����DSC������Ϣ
        u32 dwDscInnerIp = tAgentGenCfg.m_tDscInfo.GetDscIp();
        u32 dwMcuInnerIp = tAgentGenCfg.m_tDscInfo.GetMcuIp();
        u32 dwIpMask = tAgentGenCfg.m_tDscInfo.GetIpMask();
		// Bug00007792, �ж��Ƿ�������DSC, ��ֹ������NACK, zgc, 2007-02-28
		u8 byCfgDscType = g_cMcuAgent.GetConfigedDscType();
		if( byCfgDscType != 0 &&  // Bug00007792, �ж��Ƿ�������DSC, ��ֹ������NACK, zgc, 2007-02-28
			(
			 0 == dwDscInnerIp ||
			 0 == dwMcuInnerIp ||
             0 == dwIpMask ||
             dwDscInnerIp == dwMcuInnerIp || 
             (dwDscInnerIp&dwIpMask) != (dwMcuInnerIp&dwIpMask)
			)
          )
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MCUGEN)
                LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcMcuGeneralCfg] DCC module cfg error!\n");
            return;
		}
        wRet = g_cMcuAgent.SetDscInfo( &tAgentGenCfg.m_tDscInfo );
        if (SUCCESS_AGENT != wRet)
        {
            MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_DCS)
                LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcMcuGeneralCfg] mcu general write dsc module failed. ret:%d!\n", wRet);
            return;
		}
#endif
*/
		//[20101025fxh]�Ƴ�һ����MCS_MCU_CHANGEMCUEQPCFG_REQ��Ϊʵ���û�������ʹ��Ĭ������ʱ�ܹ�����ip��Ϣ��ϵͳ
// 		wRet = g_cMcuAgent.SetIsMcuConfiged( TRUE );
// 		if (SUCCESS_AGENT != wRet)
// 		{
// 			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_ISCONFIGED);
// 			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcMcuGeneralCfg] mcu general write configflag failed. ret:%d!\n", wRet);
// 			return;
// 		}



/*
		// ����LoginInfo, zgc, 2007-10-12
		// ���ñ���LoginInfo
#ifndef WIN32
		wRet = g_cMcuAgent.SetLoginInfo( &tAgentGenCfg.m_tLoginInfo );
		if ( SUCCESS_AGENT != wRet )
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_LOGININFO);
			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcMcuGeneralCfg] mcu general write logininfo failed. ret:%d!\n", wRet);
			return;
		}
#endif
		// ���͸�8000B/C
#ifdef _MINIMCU_
		cServMsg.SetEventId( SVC_AGT_DSCTELNETLOGININFO_UPDATE_CMD );
		u8 bySlot = 0;
		u8 byLayer = 0;
		cServMsg.SetMsgBody( (u8*)&byLayer, sizeof(byLayer) );
		cServMsg.CatMsgBody( (u8*)&bySlot, sizeof(bySlot) );
		cServMsg.CatMsgBody( (u8*)&tAgentGenCfg.m_tLoginInfo, sizeof(tAgentGenCfg.m_tLoginInfo) );
		post( MAKEIID(AID_MCU_AGENT,1), SVC_AGT_DSCTELNETLOGININFO_UPDATE_CMD, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()  );
#endif
*/
		// ������� [pengguofeng 4/16/2013]
#ifdef _UTF8
		if ( pcMsg->event == MCS_MCU_CHANGEMCUGENERALCFG_REQ )
		{
			u8 byEncodingType = 0;
			g_cMcuAgent.ReadMcuEncodingType(byEncodingType);
			if ( byEncodingType != emenCoding_Utf8 )
			{
				g_cMcuAgent.WriteMcuEncodingType(emenCoding_Utf8);
			}
		}
#endif
		//ack
		SendMsg2Mcsssn(cServMsg, pcMsg->event+1);
	}

	//notify all mcs
	NotifyMcsGeneralCfg();

	return;
}

/*=============================================================================
�� �� ���� ProcUpdateTelnetLoginInfo
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/10/19   4.0		�ܹ��                  ����
=============================================================================*/
void CMcuCfgInst::ProcUpdateTelnetLoginInfo(const CMessage * const pcMsg)
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TLoginInfo tLoginInfo;
	memcpy( (u8*)&tLoginInfo, cServMsg.GetMsgBody(), sizeof(TLoginInfo) );

	// Ŀǰд���û���Ϊadmin
	tLoginInfo.SetUser("admin");

	if ( g_bPrintCfgMsg )
	{
		tLoginInfo.Print();
	}

	// ����LoginInfo, zgc, 2007-10-19
	// ���ñ���LoginInfo
#ifndef WIN32
	STATUS wRet = g_cMcuAgent.SetLoginInfo( &tLoginInfo );
	if ( SUCCESS_AGENT != wRet )
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcuGeneralCfg] mcu write logininfo failed. ret:%d!\n", wRet);
		SendMsg2Mcsssn( cServMsg, MCU_MCS_UPDATETELNETLOGININFO_NACK );
		return;
	}
#endif

	SendMsg2Mcsssn( cServMsg, MCU_MCS_UPDATETELNETLOGININFO_ACK );

#ifdef _MINIMCU_
	// ���͸�8000B/C
	cServMsg.SetEventId( SVC_AGT_DSCTELNETLOGININFO_UPDATE_CMD );
	u8 bySlot = 0;
	u8 byLayer = 0;
	cServMsg.SetMsgBody( (u8*)&byLayer, sizeof(byLayer) );
	cServMsg.CatMsgBody( (u8*)&bySlot, sizeof(bySlot) );
	cServMsg.CatMsgBody( (u8*)&tLoginInfo, sizeof(tLoginInfo) );
	post( MAKEIID(AID_MCU_AGENT,1), SVC_AGT_DSCTELNETLOGININFO_UPDATE_CMD, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()  );
#endif

	return;
}

/*=============================================================================
  �� �� ���� BrdCfgOut2In
  ��    �ܣ� ������Ϣ��ʽת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TBoardInfo *ptIn
             TBrdCfgInfo *ptOut
             u8 byNum
  �� �� ֵ�� BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::BrdCfgOut2In(TBoardInfo *ptIn, TBrdCfgInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut || byNum > MAX_BOARD_NUM)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[BrdCfgOut2In] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetIndex(ptIn[byIndex].GetBrdId());
		ptOut[byIndex].SetLayer(ptIn[byIndex].GetLayer());
		ptOut[byIndex].SetSlot(ptIn[byIndex].GetSlot(), GetSlotType(ptIn[byIndex].GetType()));
		ptOut[byIndex].SetType(/*BrdTypeOut2In*/(ptIn[byIndex].GetType()));
		ptOut[byIndex].SetIpAddr(ptIn[byIndex].GetBrdIp()); 
		ptOut[byIndex].SetSelNetPort(ptIn[byIndex].GetPortChoice());
		ptOut[byIndex].SetSelMulticast(ptIn[byIndex].GetCastChoice());
        ptOut[byIndex].SetBrdState( BrdStatusOut2In(ptIn[byIndex].GetState()) );
        ptOut[byIndex].SetOsType(ptIn[byIndex].GetOSType());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� BrdCfgIn2Out
  ��    �ܣ� ������Ϣ��ʽת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TBrdCfgInfo *ptIn
             TBoardInfo *ptOut
             u8 byNum
			 u8 byRealNum
  �� �� ֵ�� BOOL32   
=============================================================================*/
BOOL32  CMcuCfgInst::BrdCfgIn2Out(TBrdCfgInfo *ptIn, TBoardInfo *ptOut, u8 byNum, u8 *pbyRealNum /*= NULL*/)
{
	if (NULL == ptIn || NULL == ptOut || byNum > MAX_BOARD_NUM)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[BrdCfgIn2Out] invalid input param!\n");
		return FALSE;
	}
	u8 byOutSlot = 0;	

	u8 bySlotType = SLOTTYPE_EX;
	u8 byIndex = 0;
	u8 byIdx = 0;
	u8 bySlot = 0;
	u8 byIsRepeat = 0;
	if( NULL != pbyRealNum )
	{
		*pbyRealNum = 0;
	}
	for (u8 byOutIndex = 0; byOutIndex < byNum; byOutIndex++)
	{
		byIsRepeat = 0;
		for( byIdx = 0;byIdx < byIndex;++byIdx )
		{
			byOutSlot = ptOut[byIdx].GetSlot();
			ptIn[byOutIndex].GetSlot(bySlot, bySlotType);
			if( ptOut[byIdx].GetLayer() == ptIn[byOutIndex].GetLayer() &&
				ptOut[byIdx].GetType() == ptIn[byOutIndex].GetType() &&
				byOutSlot == bySlot /*&&
				ptOut[byIdx].GetPortChoice() == ptIn[byOutIndex].GetSelNetPort() &&
				ptOut[byIdx].GetCastChoice() == ptIn[byOutIndex].GetSelMulticast() &&
				ptOut[byIdx].GetBrdIp() == ptIn[byOutIndex].GetIpAddr() &&
				0 == strcmp( ptOut[byIdx].GetAlias(),ptIn[byOutIndex].GetAlias() )*/ )
			{
				byIsRepeat = 1;
				LogPrint( LOG_LVL_ERROR, MID_MCU_CFG, "[BrdCfgIn2Out] repeat brdid.%d Index: %d, Layer: %d, Slot: %d, Type: %d, Alias: %s, Ip: %0x\n", 
							ptOut[byIdx].GetBrdId(), ptIn[byOutIndex].GetIndex(), ptIn[byOutIndex].GetLayer(), ptOut[byIdx].GetSlot(), ptIn[byOutIndex].GetType(), ptIn[byOutIndex].GetAlias(), ptIn[byOutIndex].GetIpAddr());
				break;
			}
		}
		if( 1 == byIsRepeat )
		{
			continue;
		}

		if ( BRD_TYPE_HDU == ptIn[byOutIndex].GetType() ||
			 BRD_TYPE_HDU_L == ptIn[byOutIndex].GetType())     //4.6  jlb
		{
		    ptOut[byIndex].SetBrdId(ptIn[byOutIndex].GetIndex());
		}
		else
		{
			ptOut[byIndex].SetBrdId(ptIn[byOutIndex].GetIndex());
		}
		ptOut[byIndex].SetLayer(ptIn[byOutIndex].GetLayer());

        
		ptIn[byOutIndex].GetSlot(bySlot, bySlotType);
        ptOut[byIndex].SetSlot(bySlot);
        
		ptOut[byIndex].SetType(/*BrdTypeIn2Out*/(ptIn[byOutIndex].GetType()));
		ptOut[byIndex].SetBrdIp(ptIn[byOutIndex].GetIpAddr()); 
		ptOut[byIndex].SetPortChoice(ptIn[byOutIndex].GetSelNetPort());
		ptOut[byIndex].SetCastChoice(ptIn[byOutIndex].GetSelMulticast());
        
        ptOut[byIndex].SetAlias(ptIn[byOutIndex].GetAlias());
		

		
		LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[BrdCfgIn2Out] Index: %d, Layer: %d, Slot: %d, Type: %d, Alias: %s, Ip: %0x\n", 
			    ptOut[byIndex].GetBrdId(), ptIn[byOutIndex].GetLayer(), ptOut[byIndex].GetSlot(), ptIn[byOutIndex].GetType(), ptIn[byOutIndex].GetAlias(), ptIn[byOutIndex].GetIpAddr());

		++byIndex;
		if( NULL != pbyRealNum )
		{
			*pbyRealNum = *pbyRealNum + 1;
		}			
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� MixerCfgOut2In
  ��    �ܣ� ��������Ϣת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpMixerInfo *ptIn
             TEqpMixerCfgInfo *ptOut
             u8 byNum
  �� �� ֵ�� BOOL32    
=============================================================================*/
BOOL32  CMcuCfgInst::MixerCfgOut2In(TEqpMixerInfo *ptIn, TEqpMixerCfgInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[MixerCfgOut2In] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuStartPort(ptIn[byIndex].GetMcuRecvPort());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetSwitchBrdIndex(ptIn[byIndex].GetSwitchBrdId());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());
		//tianzhiyong 2010/03/23  EAPU�¸�������Ա
		ptOut[byIndex].SetMaxChnnlInGrp(ptIn[byIndex].GetMaxChnInGrp());
        
        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        ptIn[byIndex].GetUsedMap( abyMapId, byMapNum );
        ptOut[byIndex].SetMapId( abyMapId, byMapNum );
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� MixerCfgIn2Out
  ��    �ܣ� ��������Ϣת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpMixerCfgInfo *ptIn
             TEqpMixerInfo *ptOut
             u8 byNum
  �� �� ֵ�� BOOL32    
=============================================================================*/
BOOL32  CMcuCfgInst::MixerCfgIn2Out(TEqpMixerCfgInfo *ptIn, TEqpMixerInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[MixerCfgIn2Out] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId( ptIn[byIndex].GetEqpId() );
        ptOut[byIndex].SetAlias( ptIn[byIndex].GetAlias() );
        ptOut[byIndex].SetMcuRecvPort( ptIn[byIndex].GetMcuStartPort() );
    	ptOut[byIndex].SetEqpRecvPort( ptIn[byIndex].GetEqpStartPort() );
		ptOut[byIndex].SetSwitchBrdId( ptIn[byIndex].GetSwitchBrdIndex() );
		ptOut[byIndex].SetRunBrdId( ptIn[byIndex].GetRunningBrdIndex() );
		//tianzhiyong 2010/03/23  EAPU�¸�������Ա
		ptOut[byIndex].SetMaxChnInGrp( ptIn[byIndex].GetMaxChnnlInGrp());

        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        ptIn[byIndex].GetMapId( abyMapId, byMapNum );
        ptOut[byIndex].SetUsedMap( abyMapId, byMapNum );
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� RecCfgOut2In
  ��    �ܣ� ¼�����Ϣת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpRecInfo *ptIn
             TEqpRecCfgInfo *ptOut
             u8 byNum
  �� �� ֵ�� BOOL32    
=============================================================================*/
BOOL32  CMcuCfgInst::RecCfgOut2In(TEqpRecInfo *ptIn, TEqpRecCfgInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[RecCfgOut2In] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());        
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuStartPort(ptIn[byIndex].GetMcuRecvPort());
        // guzh [7/11/2007]  ����Ҫ������
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetIpAddr(ptIn[byIndex].GetIpAddr());
		ptOut[byIndex].SetSwitchBrdIndex(ptIn[byIndex].GetSwitchBrdId());
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� RecCfgIn2Out
  ��    �ܣ� ¼�����Ϣת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpRecCfgInfo *ptIn
             TEqpRecInfo *ptOut
             u8 byNum
  �� �� ֵ�� BOOL32    
=============================================================================*/
BOOL32  CMcuCfgInst::RecCfgIn2Out(TEqpRecCfgInfo *ptIn, TEqpRecInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[RecCfgIn2Out] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{        
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
        // guzh [7/11/2007]  ����Ҫ������
		ptOut[byIndex].SetMcuRecvPort(ptIn[byIndex].GetMcuStartPort());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetIpAddr(ptIn[byIndex].GetIpAddr());
		ptOut[byIndex].SetSwitchBrdId(ptIn[byIndex].GetSwitchBrdIndex());
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� TWCfgOut2In
  ��    �ܣ� ����ǽ��Ϣת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpTVWallInfo *ptIn
             TEqpTvWallCfgInfo *ptOut
             u8 byNum
  �� �� ֵ�� BOOL32    
=============================================================================*/
BOOL32  CMcuCfgInst::TWCfgOut2In(TEqpTVWallInfo *ptIn, TEqpTvWallCfgInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[TWCfgOut2In] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex <byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());

        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        ptIn[byIndex].GetUsedMap( abyMapId, byMapNum );
        ptOut[byIndex].SetMapId( abyMapId, byMapNum );
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� TWCfgIn2Out
  ��    �ܣ� ����ǽ��Ϣת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpTvWallCfgInfo *ptIn
             TEqpTVWallInfo *ptOut
             u8 byNum
  �� �� ֵ�� BOOL32    
=============================================================================*/
BOOL32  CMcuCfgInst::TWCfgIn2Out(TEqpTvWallCfgInfo *ptIn, TEqpTVWallInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[TWCfgIn2Out] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex <byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetRunBrdId(ptIn[byIndex].GetRunningBrdIndex());

        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        ptIn[byIndex].GetMapId( abyMapId, byMapNum );
        ptOut[byIndex].SetUsedMap( abyMapId, byMapNum );
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� BasCfgOut2In
  ��    �ܣ� bas��Ϣת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpBasInfo *ptIn
             TEqpBasCfgInfo *ptOut
             u8 byNum
  �� �� ֵ�� BOOL32   
=============================================================================*/
BOOL32  CMcuCfgInst::BasCfgOut2In(TEqpBasInfo *ptIn, TEqpBasCfgInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[BasCfgOut2In] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuStartPort(ptIn[byIndex].GetMcuRecvPort());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetSwitchBrdIndex(ptIn[byIndex].GetSwitchBrdId());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());

        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        ptIn[byIndex].GetUsedMap( abyMapId, byMapNum );
        ptOut[byIndex].SetMapId( abyMapId, byMapNum );
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� BasHDCfgOut2In
  ��    �ܣ� basHD��Ϣת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpBasHDInfo *ptIn
             TEqpBasHDCfgInfo *ptOut
             u8 byNum
  �� �� ֵ�� BOOL32   
=============================================================================*/
BOOL32  CMcuCfgInst::BasHDCfgOut2In(TEqpBasHDInfo *ptIn, TEqpBasHDCfgInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[BasHDCfgOut2In] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());        
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuStartPort(ptIn[byIndex].GetMcuRecvPort());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetIpAddr(ptIn[byIndex].GetIpAddr());
		ptOut[byIndex].SetSwitchBrdIndex(ptIn[byIndex].GetSwitchBrdId());
        ptOut[byIndex].SetType(ptIn[byIndex].GetHDBasType());
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� BasCfgIn2Out
  ��    �ܣ� bas��Ϣת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpBasCfgInfo *ptIn
             TEqpBasInfo *ptOut
             u8 byNum
  �� �� ֵ�� BOOL32    
=============================================================================*/
BOOL32  CMcuCfgInst::BasCfgIn2Out(TEqpBasCfgInfo *ptIn, TEqpBasInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[BasCfgIn2Out] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId( ptIn[byIndex].GetEqpId() );
        ptOut[byIndex].SetAlias( ptIn[byIndex].GetAlias() );

		ptOut[byIndex].SetMcuRecvPort( ptIn[byIndex].GetMcuStartPort() );
		ptOut[byIndex].SetEqpRecvPort( ptIn[byIndex].GetEqpStartPort() );
		ptOut[byIndex].SetSwitchBrdId( ptIn[byIndex].GetSwitchBrdIndex() );
		ptOut[byIndex].SetRunBrdId( ptIn[byIndex].GetRunningBrdIndex() );
        
        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        ptIn[byIndex].GetMapId( abyMapId, byMapNum );
        ptOut[byIndex].SetUsedMap( abyMapId, byMapNum );
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� BasHDCfgIn2Out
  ��    �ܣ� basHD��Ϣת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpBasHDCfgInfo *ptIn
             TEqpBasHDInfo *ptOut
             u8 byNum
  �� �� ֵ�� BOOL32    
=============================================================================*/
BOOL32  CMcuCfgInst::BasHDCfgIn2Out(TEqpBasHDCfgInfo *ptIn, TEqpBasHDInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[BasHDCfgIn2Out] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuRecvPort(ptIn[byIndex].GetMcuStartPort());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetIpAddr(ptIn[byIndex].GetIpAddr());
		ptOut[byIndex].SetSwitchBrdId(ptIn[byIndex].GetSwitchBrdIndex());
        ptOut[byIndex].SetHDBasType(ptIn[byIndex].GetType());
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� VmpCfgOut2In
  ��    �ܣ� vmp��Ϣת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpVMPInfo *ptIn
             TEqpVmpCfgInfo *ptOut
             u8 byNum
  �� �� ֵ�� BOOL32   
=============================================================================*/
BOOL32  CMcuCfgInst::VmpCfgOut2In(TEqpVMPInfo *ptIn, TEqpVmpCfgInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[VmpCfgOut2In] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuStartPort(ptIn[byIndex].GetMcuRecvPort());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetSwitchBrdIndex(ptIn[byIndex].GetSwitchBrdId());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());
		ptOut[byIndex].SetEncodeNum(ptIn[byIndex].GetEncodeNum());

        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        ptIn[byIndex].GetUsedMap( abyMapId, byMapNum );
        ptOut[byIndex].SetMapId( abyMapId, byMapNum );
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� VmpCfgIn2Out
  ��    �ܣ� vmp��Ϣת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpVmpCfgInfo *ptIn
             TEqpVMPInfo *ptOut
             u8 byNum
  �� �� ֵ�� BOOL32   
=============================================================================*/
BOOL32  CMcuCfgInst::VmpCfgIn2Out(TEqpVmpCfgInfo *ptIn, TEqpVMPInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[VmpCfgIn2Out] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuRecvPort(ptIn[byIndex].GetMcuStartPort());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetSwitchBrdId(ptIn[byIndex].GetSwitchBrdIndex());
		ptOut[byIndex].SetRunBrdId(ptIn[byIndex].GetRunningBrdIndex());
		ptOut[byIndex].SetEncodeNum(ptIn[byIndex].GetEncodeNum());

        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        ptIn[byIndex].GetMapId( abyMapId, byMapNum );
        ptOut[byIndex].SetUsedMap( abyMapId, byMapNum );
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� PrsCfgOut2In
  ��    �ܣ� prs��Ϣת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpPrsInfo *ptIn
             TPrsCfgInfo *ptOut
             u8 byNum
  �� �� ֵ�� BOOL32   
=============================================================================*/
BOOL32  CMcuCfgInst::PrsCfgOut2In(TEqpPrsInfo *ptIn, TPrsCfgInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[PrsCfgOut2In] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuStartPort(ptIn[byIndex].GetMcuRecvPort());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetSwitchBrdIndex(ptIn[byIndex].GetSwitchBrdId());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());
		ptOut[byIndex].SetFirstTimeSpan(ptIn[byIndex].GetFirstTimeSpan());
		ptOut[byIndex].SetSecondTimeSpan(ptIn[byIndex].GetSecondTimeSpan());
		ptOut[byIndex].SetThirdTimeSpan(ptIn[byIndex].GetThirdTimeSpan());
		ptOut[byIndex].SetLoseTimeSpan(ptIn[byIndex].GetRejectTimeSpan());
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� PrsCfgIn2Out
  ��    �ܣ� prs��Ϣת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TPrsCfgInfo *ptIn
             TEqpPrsInfo *ptOut
             u8 byNum
  �� �� ֵ�� BOOL32   
=============================================================================*/
BOOL32  CMcuCfgInst::PrsCfgIn2Out(TPrsCfgInfo *ptIn, TEqpPrsInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[PrsCfgIn2Out] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuRecvPort(ptIn[byIndex].GetMcuStartPort());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetSwitchBrdId(ptIn[byIndex].GetSwitchBrdIndex());
		ptOut[byIndex].SetRunBrdId(ptIn[byIndex].GetRunningBrdIndex());
		ptOut[byIndex].SetFirstTimeSpan(ptIn[byIndex].GetFirstTimeSpan());
		ptOut[byIndex].SetSecondTimeSpan(ptIn[byIndex].GetSecondTimeSpan());
		ptOut[byIndex].SetThirdTimeSpan(ptIn[byIndex].GetThirdTimeSpan());
		ptOut[byIndex].SetRejectTimeSpan(ptIn[byIndex].GetLoseTimeSpan());
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� MtwCfgOut2In
  ��    �ܣ� ���ϵ���ǽ�ṹת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpMpwInfo *ptIn
             TMTvwallInfo *ptOut
             u8 byNum = 1
  �� �� ֵ�� BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::MtwCfgOut2In(TEqpMpwInfo *ptIn, TEqpMTvwallCfgInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[MtwCfgOut2In] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());

        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        ptIn[byIndex].GetUsedMap( abyMapId, byMapNum );
        ptOut[byIndex].SetMapId( abyMapId, byMapNum );
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� MtwCfgIn2Out
  ��    �ܣ� ���ϵ���ǽ�ṹת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TMTvwallInfo *ptIn
             TEqpMpwInfo *ptOut
             u8 byNum = 1
  �� �� ֵ�� BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::MtwCfgIn2Out(TEqpMTvwallCfgInfo *ptIn, TEqpMpwInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[MtwCfgOut2In] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetRunBrdId(ptIn[byIndex].GetRunningBrdIndex());
        
        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        ptIn[byIndex].GetMapId( abyMapId, byMapNum );
        ptOut[byIndex].SetUsedMap( abyMapId, byMapNum );
	}

	return TRUE;
}

//4.6�¼����� jlb
/*=============================================================================
  �� �� ���� HduCfgOut2In
  ��    �ܣ� ������뿨�ṹת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpHduInfo *ptIn
             TEqpHduCfgInfo *ptOut
             u8 byNum = 1
  �� �� ֵ�� BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::HduCfgOut2In(TEqpHduInfo *ptIn, TEqpHduCfgInfo *ptOut, u8 byNum/* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[TWCfgOut2In] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex <byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());
		ptOut[byIndex].SetStartMode(ptIn[byIndex].GetStartMode());

        THduChnlModePortAgt tHduChnModePortAgt;
        THduChnlModePort tHduChnModePort;
        memset(&tHduChnModePortAgt, 0x0, sizeof(tHduChnModePortAgt));
		memset(&tHduChnModePort, 0x0, sizeof(tHduChnModePort));
		ptIn[byIndex].GetHduChnlModePort(0, tHduChnModePortAgt);
        tHduChnModePort.SetOutModeType(tHduChnModePortAgt.GetOutModeType());
		tHduChnModePort.SetOutPortType(tHduChnModePortAgt.GetOutPortType());
        tHduChnModePort.SetZoomRate( tHduChnModePortAgt.GetZoomRate() );
		tHduChnModePort.SetScalingMode(tHduChnModePortAgt.GetScalingMode());
		ptOut[byIndex].SetHduChnlModePort(0, tHduChnModePort);
		ptIn[byIndex].GetHduChnlModePort(1, tHduChnModePortAgt);
		tHduChnModePort.SetOutModeType(tHduChnModePortAgt.GetOutModeType());
		tHduChnModePort.SetOutPortType(tHduChnModePortAgt.GetOutPortType());
        tHduChnModePort.SetZoomRate( tHduChnModePortAgt.GetZoomRate() );
		tHduChnModePort.SetScalingMode(tHduChnModePortAgt.GetScalingMode());
		ptOut[byIndex].SetHduChnlModePort(1, tHduChnModePort);

//		ptOut[byIndex].SetOutModeType(ptIn[byIndex].GetOutModeType());
//		ptOut[byIndex].SetOutPortType(ptIn[byIndex].GetOutPortType());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());    //???
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� HduCfgIn2Out
  ��    �ܣ� ������뿨�ṹת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpHduCfgInfo *ptIn
             TEqpHduInfo *ptOut
             u8 byNum = 1
  �� �� ֵ�� BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::HduCfgIn2Out(TEqpHduCfgInfo *ptIn, TEqpHduInfo *ptOut, u8 byNum /* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[HduCfgIn2Out] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex <byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetRunBrdId(ptIn[byIndex].GetRunningBrdIndex());
		ptOut[byIndex].SetStartMode(ptIn[byIndex].GetStartMode());

		THduChnlModePort tHduChnModePort;
		THduChnlModePortAgt tHduChnModePortAgt;
		memset(&tHduChnModePortAgt, 0x0, sizeof(tHduChnModePortAgt));
		memset(&tHduChnModePort, 0x0, sizeof(tHduChnModePort));
		ptIn[byIndex].GetHduChnlModePort(0, tHduChnModePort);
		tHduChnModePortAgt.SetZoomRate(tHduChnModePort.GetZoomRate());
		tHduChnModePortAgt.SetOutModeType(tHduChnModePort.GetOutModeType());
		tHduChnModePortAgt.SetOutPortType(tHduChnModePort.GetOutPortType());
		tHduChnModePortAgt.SetScalingMode(tHduChnModePort.GetScalingMode());
		ptOut[byIndex].SetHduChnlModePort(0, tHduChnModePortAgt);
		ptIn[byIndex].GetHduChnlModePort(1, tHduChnModePort);
        tHduChnModePortAgt.SetZoomRate( tHduChnModePort.GetZoomRate() );
		tHduChnModePortAgt.SetOutModeType(tHduChnModePort.GetOutModeType());
		tHduChnModePortAgt.SetOutPortType(tHduChnModePort.GetOutPortType());
		tHduChnModePortAgt.SetScalingMode(tHduChnModePort.GetScalingMode());
		ptOut[byIndex].SetHduChnlModePort(1, tHduChnModePortAgt);


// 		
// 		ptOut[byIndex].SetOutModeType(ptIn[byIndex].GetOutModeType());
// 		ptOut[byIndex].SetOutPortType(ptIn[byIndex].GetOutPortType());

	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� SvmpCfgOut2In
  ��    �ܣ� ��Vmp�ṹת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpSvmpInfo *ptIn
             TEqpSvmpCfgInfo *ptOut
             u8 byNum = 1
  �� �� ֵ�� BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::SvmpCfgOut2In(TEqpSvmpInfo *ptIn, TEqpSvmpCfgInfo *ptOut, u8 byNum /* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[SvmpCfgOut2In] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuStartPort(ptIn[byIndex].GetMcuRecvPort());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetSwitchBrdIndex(ptIn[byIndex].GetSwitchBrdId());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());
		ptOut[byIndex].SetVmpType(ptIn[byIndex].GetVmpType());
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� SvmpCfgOut2In
  ��    �ܣ� ��Vmp�ṹת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpSvmpCfgInfo *ptIn
             TEqpSvmpInfo *ptOut
             u8 byNum = 1
  �� �� ֵ�� BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::SvmpCfgIn2Out(TEqpSvmpCfgInfo *ptIn, TEqpSvmpInfo *ptOut, u8 byNum /* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[SvmpCfgIn2Out] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuRecvPort(ptIn[byIndex].GetMcuStartPort());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetSwitchBrdId(ptIn[byIndex].GetSwitchBrdIndex());
		ptOut[byIndex].SetRunBrdId(ptIn[byIndex].GetRunningBrdIndex());
		ptOut[byIndex].SetVmpType(ptIn[byIndex].GetVmpType());
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� DvmpCfgOut2In
  ��    �ܣ� ˫Vmp�ṹת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpDvmpBasicInfo *ptIn
             TEqpDvmpCfgBasicInfo *ptOut
             u8 byNum = 1
  �� �� ֵ�� BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::DvmpCfgOut2In(TEqpDvmpBasicInfo *ptIn, TEqpDvmpCfgBasicInfo *ptOut, u8 byNum /* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[DvmpCfgOut2In] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuStartPort(ptIn[byIndex].GetMcuRecvPort());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetSwitchBrdIndex(ptIn[byIndex].GetSwitchBrdId());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());
		
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� DvmpCfgIn2Out
  ��    �ܣ� ˫Vmp�ṹת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpDvmpCfgBasicInfo *ptIn
             TEqpDvmpBasicInfo *ptOut
             u8 byNum = 1
  �� �� ֵ�� BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::DvmpCfgIn2Out(TEqpDvmpCfgBasicInfo *ptIn, TEqpDvmpBasicInfo *ptOut, u8 byNum /* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[DvmpCfgIn2Out] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuRecvPort(ptIn[byIndex].GetMcuStartPort());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetSwitchBrdId(ptIn[byIndex].GetSwitchBrdIndex());
		ptOut[byIndex].SetRunBrdId(ptIn[byIndex].GetRunningBrdIndex());
		
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� MpuBasCfgOut2In
  ��    �ܣ� ˫Vmp�ṹ�е���vmpת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpMpuBasInfo *ptIn
             TEqpMpuBasCfgInfo *ptOut
             u8 byNum = 1
  �� �� ֵ�� BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::MpuBasCfgOut2In(TEqpMpuBasInfo *ptIn, TEqpMpuBasCfgInfo *ptOut, u8 byNum /* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[MpuBasCfgOut2In] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuStartPort(ptIn[byIndex].GetMcuRecvPort());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetSwitchBrdIndex(ptIn[byIndex].GetSwitchBrdId());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());
		ptOut[byIndex].SetStartMode(ptIn[byIndex].GetStartMode());
		
	}
	return TRUE;
}

/*=============================================================================
  �� �� ���� MpuBasCfgOut2In
  ��    �ܣ� ˫Vmp�ṹ�е���vmpת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpMpuBasInfo *ptIn
             TEqpMpuBasCfgInfo *ptOut
             u8 byNum = 1
  �� �� ֵ�� BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::MpuBasCfgIn2Out(TEqpMpuBasCfgInfo *ptIn, TEqpMpuBasInfo *ptOut, u8 byNum /* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[MpuBasCfgIn2Out] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuRecvPort(ptIn[byIndex].GetMcuStartPort());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetSwitchBrdId(ptIn[byIndex].GetSwitchBrdIndex());
		ptOut[byIndex].SetRunBrdId(ptIn[byIndex].GetRunningBrdIndex());
		ptOut[byIndex].SetStartMode(ptIn[byIndex].GetStartMode());
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� EbapCfgOut2In
  ��    �ܣ� Ebap�ṹת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpEbapInfo *ptIn
             TEqpEbapCfgInfo *ptOut
             u8 byNum = 1
  �� �� ֵ�� BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::EbapCfgOut2In(TEqpEbapInfo *ptIn, TEqpEbapCfgInfo *ptOut, u8 byNum /* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[EbapCfgOut2In] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuStartPort(ptIn[byIndex].GetMcuRecvPort());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetSwitchBrdIndex(ptIn[byIndex].GetSwitchBrdId());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());
	}

	return TRUE;
}
/*=============================================================================
  �� �� ���� EbapCfgIn2Out
  ��    �ܣ� Ebap�ṹת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpEbapCfgInfo *ptIn
             TEqpEbapInfo *ptOut
             u8 byNum = 1
  �� �� ֵ�� BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::EbapCfgIn2Out(TEqpEbapCfgInfo *ptIn, TEqpEbapInfo *ptOut, u8 byNum /* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[EbapCfgIn2Out] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuRecvPort(ptIn[byIndex].GetMcuStartPort());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetSwitchBrdId(ptIn[byIndex].GetSwitchBrdIndex());
		ptOut[byIndex].SetRunBrdId(ptIn[byIndex].GetRunningBrdIndex());
		
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� EvpuCfgOut2In
  ��    �ܣ� Ebap�ṹת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpEvpuInfo *ptIn
             TEqpEvpuCfgInfo *ptOut
             u8 byNum = 1
  �� �� ֵ�� BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::EvpuCfgOut2In(TEqpEvpuInfo *ptIn, TEqpEvpuCfgInfo *ptOut, u8 byNum /* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[EvpuCfgOut2In] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuStartPort(ptIn[byIndex].GetMcuRecvPort());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetSwitchBrdIndex(ptIn[byIndex].GetSwitchBrdId());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());
		
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� EvpuCfgIn2Out
  ��    �ܣ� Ebap�ṹת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TEqpEvpuCfgInfo *ptIn
             TEqpEvpuInfo *ptOut
             u8 byNum = 1
  �� �� ֵ�� BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::EvpuCfgIn2Out(TEqpEvpuCfgInfo *ptIn, TEqpEvpuInfo *ptOut, u8 byNum /* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[EvpuCfgIn2Out] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuRecvPort(ptIn[byIndex].GetMcuStartPort());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetSwitchBrdId(ptIn[byIndex].GetSwitchBrdIndex());
		ptOut[byIndex].SetRunBrdId(ptIn[byIndex].GetRunningBrdIndex());
		
	}

	return TRUE;
}

/*=============================================================================
�� �� ���� DscInfoOut2In
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  TDSCModuleInfo &tIn
           T8000BDscMod &tOut
�� �� ֵ�� BOOL32  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/3/15  4.0			�ܹ��                  ����
=============================================================================*/
BOOL32  CMcuCfgInst::DscInfoOut2In( TDSCModuleInfo &tIn, T8000BDscMod &tOut )
{
	tOut.SetDscInnerIp( tIn.GetDscInnerIp() );
	tOut.SetMcuInnerIp( tIn.GetMcuInnerIp() );
	tOut.SetInnerIpMask( tIn.GetInnerIpMask() );
	tOut.SetMcuPort( tIn.GetMcuPort() );
	tOut.SetStartDcs( tIn.IsStartDcs() );
	tOut.SetStartGk( tIn.IsStartGk() );
	tOut.SetStartMp( tIn.IsStartMp() );
	tOut.SetStartMtAdp( tIn.IsStartMtAdp() );
	tOut.SetStartProxy( tIn.IsStartProxy() );
	tOut.SetNetType( tIn.GetNetType() );
	tOut.SetLanMtProxyIpPos( tIn.GetLanMtProxyIpPos() );

	TMINIMCUNetParamAll tNetParamAll;
	tIn.GetCallAddrAll( tNetParamAll );
	tOut.SetCallAddrAll( tNetParamAll );

	memset( &tNetParamAll, 0, sizeof(tNetParamAll) );
	tIn.GetMcsAccessAddrAll( tNetParamAll );
	tOut.SetMcsAccessAddrAll( tNetParamAll );

	return TRUE;
}

/*=============================================================================
�� �� ���� DscInfoIn2Out
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  T8000BDscMod &tIn
           TDSCModuleInfo &tOut
�� �� ֵ�� BOOL32  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/3/15  4.0			�ܹ��                  ����
=============================================================================*/
BOOL32  CMcuCfgInst::DscInfoIn2Out( T8000BDscMod &tIn, TDSCModuleInfo &tOut )
{
	tOut.SetDscInnerIp( tIn.GetDscInnerIp() );
	tOut.SetMcuInnerIp( tIn.GetMcuInnerIp() );
	tOut.SetInnerIpMask( tIn.GetInnerIpMask() );
	tOut.SetMcuPort( tIn.GetMcuPort() );
	tOut.SetStartDcs( tIn.IsStartDcs() );
	tOut.SetStartGk( tIn.IsStartGk() );
	tOut.SetStartMp( tIn.IsStartMp() );
	tOut.SetStartMtAdp( tIn.IsStartMtAdp() );
	tOut.SetStartProxy( tIn.IsStartProxy() );
	tOut.SetNetType( tIn.GetNetType() );
	tOut.SetLanMtProxyIpPos( tIn.GetLanMtProxyIpPos() );

	TMINIMCUNetParamAll tNetParamAll;
	tIn.GetCallAddrAll( tNetParamAll );
	tOut.SetCallAddrAll( tNetParamAll );

	memset( &tNetParamAll, 0, sizeof(tNetParamAll) );
	tIn.GetMcsAccessAddrAll( tNetParamAll );
	tOut.SetMcsAccessAddrAll( tNetParamAll );

	return TRUE;
}

/*=============================================================================
  �� �� ���� GeneralCfgOut2In
  ��    �ܣ� GeneralCfgת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TAgentMcuGenCfg &tIn
             TMcuGeneralCfg &tOut
  �� �� ֵ�� BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::GeneralCfgOut2In(TAgentMcuGenCfg &tIn, TMcuNewGeneralCfg &tOut)
{
    TTrapCfgInfo atTrapInfoList[MAXNUM_TRAP_LIST];
    
	//vmp style
	if (tIn.m_byVmpStyleNum > MAX_VMPSTYLE_NUM)
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[GeneralCfgOut2In] vmp style num %d overflow!\n", tIn.m_byVmpStyleNum);
		return FALSE;
	}
	for (u8 byIndex = 0; byIndex < tIn.m_byVmpStyleNum; byIndex++)
	{
		tOut.m_atVmpStyle[byIndex].SetBackgroundColor(tIn.m_atVmpStyle[byIndex].GetBGDColor());
		tOut.m_atVmpStyle[byIndex].SetFrameColor(tIn.m_atVmpStyle[byIndex].GetFrameColor());
		tOut.m_atVmpStyle[byIndex].SetChairFrameColor(tIn.m_atVmpStyle[byIndex].GetChairFrameColor());
		tOut.m_atVmpStyle[byIndex].SetSpeakerFrameColor(tIn.m_atVmpStyle[byIndex].GetSpeakerFrameColor());  
		tOut.m_atVmpStyle[byIndex].SetSchemeId(tIn.m_atVmpStyle[byIndex].GetIndex());
		tOut.m_atVmpStyle[byIndex].SetFontType(tIn.m_atVmpStyle[byIndex].GetFontType());
		tOut.m_atVmpStyle[byIndex].SetFontSize(tIn.m_atVmpStyle[byIndex].GetFontSize());
		tOut.m_atVmpStyle[byIndex].SetAlignment(tIn.m_atVmpStyle[byIndex].GetAlignType());
		tOut.m_atVmpStyle[byIndex].SetTextColor(tIn.m_atVmpStyle[byIndex].GetTextColor());
		tOut.m_atVmpStyle[byIndex].SetTopicBkColor(tIn.m_atVmpStyle[byIndex].GetTopicBGDColor());
		tOut.m_atVmpStyle[byIndex].SetDiaphaneity(tIn.m_atVmpStyle[byIndex].GetDiaphaneity());
		tOut.m_atVmpStyle[byIndex].SetSchemeAlias((s8*)tIn.m_atVmpStyle[byIndex].GetAlias());
	}    
	tOut.m_byVmpStyleNum = tIn.m_byVmpStyleNum;

	//mcu local cfg
	tOut.m_tLocal.SetMcuAlias(tIn.m_tLocal.GetAlias());
	tOut.m_tLocal.SetMcuE164(tIn.m_tLocal.GetE164Num());
	tOut.m_tLocal.SetAudInfoRefreshTime(tIn.m_tLocal.GetAudioRefreshTime());
	tOut.m_tLocal.SetVidInfoRefreshTime(tIn.m_tLocal.GetVideoRefreshTime());
	tOut.m_tLocal.SetPartListRefreshTime(tIn.m_tLocal.GetListRefreshTime());
	tOut.m_tLocal.SetDiscCheckTime(tIn.m_tLocal.GetCheckTime());
	tOut.m_tLocal.SetDiscCheckTimes(tIn.m_tLocal.GetCheckTimes());
	tOut.m_tLocal.SetIsSaveBandWidth(tIn.m_tLocal.GetIsSaveBand());
	tOut.m_tLocal.SetMcuSoftVer(GetMcuVersion());
    tOut.m_tLocal.SetIsNPlusMode(tIn.m_tLocal.IsNPlusMode());
    tOut.m_tLocal.SetIsNPlusBackupMode(tIn.m_tLocal.IsNPlusBackupMode());
    tOut.m_tLocal.SetNPlusMcuIp(tIn.m_tLocal.GetNPlusMcuIp());
    tOut.m_tLocal.SetNPlusRtdTime(tIn.m_tLocal.GetNPlusRtdTime());
    tOut.m_tLocal.SetNPlusRtdNum(tIn.m_tLocal.GetNPlusRtdNum());
    tOut.m_tLocal.SetIsNPlusRollBack(tIn.m_tLocal.GetIsNPlusRollBack());

	tOut.m_tLocal.SetIsHoldDefaultConf( tIn.m_tLocal.GetIsHoldDefaultConf() );
	tOut.m_tLocal.SetIsShowMMcuMtList( tIn.m_tLocal.GetIsShowMMcuMtList() );
	tOut.m_tLocal.SetMaxMcsOnGoingConfNum( tIn.m_tLocal.GetMaxMcsOnGoingConfNum() );
	tOut.m_tLocal.SetAdminLevel( tIn.m_tLocal.GetAdminLevel() );
	tOut.m_tLocal.SetConfNameShowType( tIn.m_tLocal.GetConfNameShowType() );
	tOut.m_tLocal.SetIsMMcuSpeaker( tIn.m_tLocal.GetIsMMcuSpeaker() );

	s8  szVer[MAXLEN_ALIAS];
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	sprintf(szVer, "V%s", "6.0");
#else
	sprintf(szVer, "V%d", g_cMcuAgent.GetHWVersion());
#endif	
	tOut.m_tLocal.SetMcuHardVer(szVer);

	//net cfg
	tOut.m_tNet.SetGkIpAddr(tIn.m_tNet.GetGkIp());
    tOut.m_tNet.SetIsGKCharge(tIn.m_tNet.GetIsGKCharge());
    tOut.m_tNet.SetRRQMtadpIp(tIn.m_tNet.GetRRQMtadpIp());
	tOut.m_tNet.SetMultiIpAddr(tIn.m_tNet.GetCastIp());
	tOut.m_tNet.SetMultiPort(tIn.m_tNet.GetCastPort());
	tOut.m_tNet.SetRcvStartPort(tIn.m_tNet.GetRecvStartPort());
	tOut.m_tNet.Set225245StartPort(tIn.m_tNet.Get225245StartPort());
	tOut.m_tNet.SetMaxMtNum(tIn.m_tNet.Get225245MtNum());
	tOut.m_tNet.SetIsUseMpcTransData(tIn.m_tNet.GetMpcTransData());
	tOut.m_tNet.SetIsUseMpcStack(tIn.m_tNet.GetMpcStack());
	tOut.m_tNet.SetMTUSize( tIn.m_tNet.GetMTUSize() ); //����MTU�Ĵ�С, zgc, 2007-04-02
	tOut.m_tNet.SetMTUSetupMode( tIn.m_tNet.GetMTUSetupMode() ); // MTU����ģʽ, zgc, 2007-04-05
	tOut.SetGkRRQUsePwdFlag(tIn.m_tNet.GetGkRRQUsePwdFlag());
	tOut.SetGkRRQPassword(tIn.m_tNet.GetGkRRQPassword());
    	
	if (tIn.m_byTrapListNum > MAXNUM_TRAP_LIST)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[GeneralCfgOut2In] too large traplist num!\n");
		return FALSE;
	}
	for (s32 nIndex = 0; nIndex < tIn.m_byTrapListNum; nIndex++)
	{
		atTrapInfoList[ nIndex].SetTrapIp(tIn.m_atTrapList[nIndex].GetTrapIp());
		atTrapInfoList[nIndex].SetReadWritePort(tIn.m_atTrapList[nIndex].GetGSPort());
		atTrapInfoList[nIndex].SetSendTrapPort(tIn.m_atTrapList[nIndex].GetTrapPort());
		atTrapInfoList[nIndex].SetReadCom(tIn.m_atTrapList[nIndex].GetReadCom());
		atTrapInfoList[nIndex].SetWriteCom(tIn.m_atTrapList[nIndex].GetWriteCom());
	}
	tOut.m_tNet.SetTrapList(atTrapInfoList, tIn.m_byTrapListNum);

	//qos
	tOut.m_tQos.SetQosType(tIn.m_tQos.GetQosType());
	tOut.m_tQos.SetAudLevel(tIn.m_tQos.GetAudioLevel());
	tOut.m_tQos.SetVidLevel(tIn.m_tQos.GetVideoLevel());
	tOut.m_tQos.SetDataLevel(tIn.m_tQos.GetDataLevel());
	tOut.m_tQos.SetSignalLevel(tIn.m_tQos.GetSignalLevel());
	tOut.m_tQos.SetIpServiceType(tIn.m_tQos.GetIpServiceType());

	//net syn
	tOut.m_tNetSyn.SetNetSynMode(tIn.m_tNetSyn.GetMode());
	tOut.m_tNetSyn.SetNetSynDTSlot(tIn.m_tNetSyn.GetDTSlot());
	tOut.m_tNetSyn.SetNetSynE1Index(tIn.m_tNetSyn.GetE1Index());

	//dcs 
	tOut.m_tDcs.SetDcsEqpIp(ntohl(tIn.m_dwDcsIp));

    // guzh [1/12/2007] 8000B ��DSC������ģ�����Ϣ
    tOut.m_tDscInfo.SetMcuInnerIp( tIn.m_tDscInfo.GetMcuInnerIp() );
    tOut.m_tDscInfo.SetDscInnerIp( tIn.m_tDscInfo.GetDscInnerIp() );
    tOut.m_tDscInfo.SetMcuPort( tIn.m_tDscInfo.GetMcuPort() );
    tOut.m_tDscInfo.SetInnerIpMask( tIn.m_tDscInfo.GetInnerIpMask() );
	tOut.m_tDscInfo.SetNetType( tIn.m_tDscInfo.GetNetType() );
	tOut.m_tDscInfo.SetLanMtProxyIpPos( tIn.m_tDscInfo.GetLanMtProxyIpPos() );

    tOut.m_tDscInfo.SetStartMp(tIn.m_tDscInfo.IsStartMp());
    tOut.m_tDscInfo.SetStartMtAdp(tIn.m_tDscInfo.IsStartMtAdp());
    tOut.m_tDscInfo.SetStartGk(tIn.m_tDscInfo.IsStartGk());
    tOut.m_tDscInfo.SetStartProxy(tIn.m_tDscInfo.IsStartProxy());
    tOut.m_tDscInfo.SetStartDcs(tIn.m_tDscInfo.IsStartDcs());

	TMINIMCUNetParamAll tNetParamAll;
	tIn.m_tDscInfo.GetCallAddrAll( tNetParamAll );
	tOut.m_tDscInfo.SetCallAddrAll( tNetParamAll );

	memset( &tNetParamAll, 0, sizeof(tNetParamAll) );
	tIn.m_tDscInfo.GetMcsAccessAddrAll( tNetParamAll );
	tOut.m_tDscInfo.SetMcsAccessAddrAll( tNetParamAll );

	tOut.m_tLoginInfo.SetUser( tIn.m_tLoginInfo.GetUser() );
	s8 achPwd[MAXLEN_PWD+1];
	memset(achPwd, 0, sizeof(achPwd));
	tIn.m_tLoginInfo.GetPwd(achPwd, sizeof(achPwd));
	tOut.m_tLoginInfo.SetPwd( (LPCSTR)achPwd );

	//  [1/8/2010 pengjie] Modify
	tOut.m_tEqpExCfgInfo = tIn.m_tEqpExCfgInfo;
	// End Modify

	return TRUE;
}

/*=============================================================================
  �� �� ���� GeneralCfgIn2Out
  ��    �ܣ� GeneralCfgת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TMcuGeneralCfg &tIn
             TAgentMcuGenCfg &tOut
  �� �� ֵ�� BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::GeneralCfgIn2Out(TMcuNewGeneralCfg &tIn, TAgentMcuGenCfg &tOut)
{   
    TTrapCfgInfo atTrapInfoList[MAXNUM_TRAP_LIST];
    
	//vmp style  
	if (tIn.m_byVmpStyleNum > MAX_VMPSTYLE_NUM)
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[GeneralCfgIn2Out] vmp style num %d overflow!\n", tIn.m_byVmpStyleNum);
		return FALSE;
	}
	for (u8 byIndex = 0; byIndex < tIn.m_byVmpStyleNum; byIndex++)
	{
		tOut.m_atVmpStyle[byIndex].SetBGDColor(tIn.m_atVmpStyle[byIndex].GetBackgroundColor());
		tOut.m_atVmpStyle[byIndex].SetFrameColor(tIn.m_atVmpStyle[byIndex].GetFrameColor());
		tOut.m_atVmpStyle[byIndex].SetChairFrameColor(tIn.m_atVmpStyle[byIndex].GetChairFrameColor());
		tOut.m_atVmpStyle[byIndex].SetSpeakerFrameColor(tIn.m_atVmpStyle[byIndex].GetSpeakerFrameColor());
		tOut.m_atVmpStyle[byIndex].SetIndex(tIn.m_atVmpStyle[byIndex].GetSchemeId());
		tOut.m_atVmpStyle[byIndex].SetFontType(tIn.m_atVmpStyle[byIndex].GetFontType());
		tOut.m_atVmpStyle[byIndex].SetFontSize(tIn.m_atVmpStyle[byIndex].GetFontSize());
		tOut.m_atVmpStyle[byIndex].SetAlignType(tIn.m_atVmpStyle[byIndex].GetAlignment());
		tOut.m_atVmpStyle[byIndex].SetTextColor(tIn.m_atVmpStyle[byIndex].GetTextColor());
		tOut.m_atVmpStyle[byIndex].SetTopicBGDColor(tIn.m_atVmpStyle[byIndex].GetTopicBkColor());
		tOut.m_atVmpStyle[byIndex].SetDiaphaneity(tIn.m_atVmpStyle[byIndex].GetDiaphaneity());
        
        s8 achAlias[MAX_VMPSTYLE_ALIASLEN];
        memset( achAlias, 0, sizeof(achAlias) );
        tIn.m_atVmpStyle[byIndex].GetSchemeAlias( achAlias, sizeof(achAlias) );
        tOut.m_atVmpStyle[byIndex].SetAlias(achAlias);
	}
	tOut.m_byVmpStyleNum = tIn.m_byVmpStyleNum;

	//mcu local cfg
	if (0 == strlen(tIn.m_tLocal.GetMcuAlias()) || 0 == strlen(tIn.m_tLocal.GetMcuE164()))
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[GeneralCfgIn2Out] mcu alias or E164 is empty!\n");
		return FALSE;
	}

    // N+1ģʽ������Ч�Լ��
    if (tIn.m_tLocal.IsNPlusMode())
    {
        if (!tIn.m_tLocal.IsNPlusBackupMode() && tIn.m_tLocal.GetNPlusMcuIp() == 0)
        {
            LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[GeneralCfgIn2Out] NPlusMcuIp can't be empty in not NPlusBackupMode!\n");
		    return FALSE;
        }

        //rtd ��Ч�Լ��
        if ( tIn.m_tLocal.IsNPlusBackupMode() && 
             ( tIn.m_tLocal.GetNPlusRtdTime() == 0 || tIn.m_tLocal.GetNPlusRtdNum() == 0 ) )
        {
            LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[GeneralCfgIn2Out] RtdTime or RtdTimes can't be 0 in NPlusBackupMode!\n");
		    return FALSE;
        }
    }

    tOut.m_tLocal.SetAlias(tIn.m_tLocal.GetMcuAlias());
    tOut.m_tLocal.SetE164Num(tIn.m_tLocal.GetMcuE164());
	tOut.m_tLocal.SetAudioRefreshTime(tIn.m_tLocal.GetAudInfoRefreshTime());
	tOut.m_tLocal.SetVideoRefreshTime(tIn.m_tLocal.GetVidInfoRefreshTime());
	tOut.m_tLocal.SetListRefreshTime(tIn.m_tLocal.GetPartListRefreshTime());
	tOut.m_tLocal.SetCheckTime(tIn.m_tLocal.GetDiscCheckTime());
	tOut.m_tLocal.SetCheckTimes(tIn.m_tLocal.GetDiscCheckTimes());
	tOut.m_tLocal.SetIsSaveBand(tIn.m_tLocal.GetIsSaveBandWidth());
	tOut.m_tLocal.SetMcuId(LOCAL_MCUID);
    tOut.m_tLocal.SetIsNPlusMode(tIn.m_tLocal.IsNPlusMode());
    tOut.m_tLocal.SetIsNPlusBackupMode(tIn.m_tLocal.IsNPlusBackupMode());
    tOut.m_tLocal.SetNPlusMcuIp(tIn.m_tLocal.GetNPlusMcuIp());
    tOut.m_tLocal.SetNPlusRtdTime(tIn.m_tLocal.GetNPlusRtdTime());
    tOut.m_tLocal.SetNPlusRtdNum(tIn.m_tLocal.GetNPlusRtdNum());
    tOut.m_tLocal.SetIsNPlusRollBack(tIn.m_tLocal.GetIsNPlusRollBack());

	//////////////////////////////////////////////////////////////////////////
	//zjj20100107
	tOut.m_tLocal.SetIsHoldDefaultConf( tIn.m_tLocal.GetIsHoldDefaultConf() );
	tOut.m_tLocal.SetIsShowMMcuMtList( tIn.m_tLocal.GetIsShowMMcuMtList() );
	tOut.m_tLocal.SetMaxMcsOnGoingConfNum( tIn.m_tLocal.GetMaxMcsOnGoingConfNum() );
	tOut.m_tLocal.SetAdminLevel( tIn.m_tLocal.GetAdminLevel() );
	tOut.m_tLocal.SetConfNameShowType( tIn.m_tLocal.GetConfNameShowType() );
	//zjj20100205
	tOut.m_tLocal.SetIsMMcuSpeaker( tIn.m_tLocal.GetIsMMcuSpeaker() );
	//////////////////////////////////////////////////////////////////////////

	//net cfg
	tOut.m_tNet.SetGKIp(tIn.m_tNet.GetGkIpAddr());
    tOut.m_tNet.SetIsGKCharge(tIn.m_tNet.GetIsGKCharge());
    tOut.m_tNet.SetRRQMtadpIp(tIn.m_tNet.GetRRQMtadpIp());
	tOut.m_tNet.SetCastIp(tIn.m_tNet.GetMultiIpAddr());
	tOut.m_tNet.SetCastPort(tIn.m_tNet.GetMultiPort());
	tOut.m_tNet.SetRecvStartPort(tIn.m_tNet.GetRcvStartPort());
	tOut.m_tNet.Set225245StartPort(tIn.m_tNet.Get225245StartPort());
	tOut.m_tNet.SetMTUSize( tIn.m_tNet.GetMTUSize() ); //����MTU�Ĵ�С, zgc, 2007-04-02
	tOut.m_tNet.SetMTUSetupMode( tIn.m_tNet.GetMTUSetupMode() ); // MTU����ģʽ, zgc, 2007-04-05
	tOut.m_tNet.SetGkRRQUsePwdFlag(tIn.GetGkRRQUsePwdFlag());
	tOut.m_tNet.SetGkRRQPassword(tIn.GetGkRRQPassword());
	
	if (tOut.m_tNet.GetRecvStartPort() % PORTSPAN > 0)
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[GeneralCfgIn2Out] mcu receive start port Ӧ��Ϊ%d�ı���.\n", PORTSPAN);
		u16 wRecvStartPort = tOut.m_tNet.GetRecvStartPort();
		tOut.m_tNet.SetRecvStartPort( wRecvStartPort - wRecvStartPort % PORTSPAN);
	}
	
    tOut.m_tNet.Set225245MtNum( g_cMcuAgent.Get225245MtNum() );
    if(0 == tOut.m_tNet.Get225245MtNum())
    {
        tOut.m_tNet.Set225245MtNum( MAXNUM_H225H245_MT );
    }
    
	tOut.m_tNet.SetMpcTransData( tIn.m_tNet.GetIsUseMpcTransData() );
	tOut.m_tNet.SetMpcStack( tIn.m_tNet.GetIsUseMpcStack() );
	// [12/30/2009 xliang] add print
	LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[GeneralCfgIn2Out] tOut.m_tNet.SetMpcStack: %d\n", 
		tIn.m_tNet.GetIsUseMpcStack());

	tIn.m_tNet.GetTrapList(atTrapInfoList, tOut.m_byTrapListNum);    
	for (s32 nIndex = 0; nIndex < tOut.m_byTrapListNum; nIndex++)
	{
		tOut.m_atTrapList[nIndex].SetTrapIp(atTrapInfoList[nIndex].GetTrapIp());
		tOut.m_atTrapList[nIndex].SetGSPort(atTrapInfoList[nIndex].GetReadWritePort());
		tOut.m_atTrapList[nIndex].SetTrapPort(atTrapInfoList[nIndex].GetSendTrapPort());
		tOut.m_atTrapList[nIndex].SetReadCom(atTrapInfoList[nIndex].GetReadCom());
		tOut.m_atTrapList[nIndex].SetWriteCom(atTrapInfoList[nIndex].GetWriteCom());
	}

	//qos
	if (QOSTYPE_DIFFERENCE_SERVICE == tIn.m_tQos.GetQosType())
	{
		if (tIn.m_tQos.GetAudLevel() > QOS_DIFF_MAXLEVEL || 
			tIn.m_tQos.GetVidLevel() > QOS_DIFF_MAXLEVEL || 
			tIn.m_tQos.GetDataLevel() > QOS_DIFF_MAXLEVEL || 
			tIn.m_tQos.GetSignalLevel() > QOS_DIFF_MAXLEVEL)
		{
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[GeneralCfgIn2Out] qos level aud<%d> vid<%d> data<%d> sign<%d> is invalid!\n", 
				   tIn.m_tQos.GetAudLevel(), tIn.m_tQos.GetVidLevel(), tIn.m_tQos.GetDataLevel(), 
				   tIn.m_tQos.GetSignalLevel());

			return FALSE;
		}
	}
	else if (QOSTYPE_IP_PRIORITY == tIn.m_tQos.GetQosType())
	{
		if (tIn.m_tQos.GetAudLevel() > QOS_IP_MAXLEVEL || 
			tIn.m_tQos.GetVidLevel() > QOS_IP_MAXLEVEL || 
			tIn.m_tQos.GetDataLevel() > QOS_IP_MAXLEVEL || 
			tIn.m_tQos.GetSignalLevel() > QOS_IP_MAXLEVEL)
		{
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[GeneralCfgIn2Out] qos level aud<%d> vid<%d> data<%d> sign<%d>, or service type<%d> is invalid!\n", 
				   tIn.m_tQos.GetAudLevel(), tIn.m_tQos.GetVidLevel(), tIn.m_tQos.GetDataLevel(), 
				   tIn.m_tQos.GetSignalLevel(), tIn.m_tQos.GetIpServiceType());
			return FALSE;
		}
	}
	else
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[GeneralCfgIn2Out] qos type<%d> is invalid!\n", tIn.m_tQos.GetQosType());
		return FALSE;
	}

	tOut.m_tQos.SetQosType( tIn.m_tQos.GetQosType() );
	tOut.m_tQos.SetAudioLevel( tIn.m_tQos.GetAudLevel() );
	tOut.m_tQos.SetVideoLevel( tIn.m_tQos.GetVidLevel() );
	tOut.m_tQos.SetDataLevel( tIn.m_tQos.GetDataLevel() );
	tOut.m_tQos.SetSignalLevel( tIn.m_tQos.GetSignalLevel() );
	tOut.m_tQos.SetIpServiceType( tIn.m_tQos.GetIpServiceType() );

	//net syn
	if (NETSYNTYPE_VIBRATION != tIn.m_tNetSyn.GetNetSynMode() && 
		NETSYNTYPE_TRACE != tIn.m_tNetSyn.GetNetSynMode())
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[GeneralCfgIn2Out] net syn mode<%d> is invalid!\n", tIn.m_tNetSyn.GetNetSynMode());
		return FALSE;
	}
	if (tIn.m_tNetSyn.GetNetSynDTSlot() > NETSYN_MAX_DTNUM || 
		tIn.m_tNetSyn.GetNetSynE1Index() > NETSYN_MAX_E1NUM)
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[GeneralCfgIn2Out] net syn DT num<%d> or E1 num<%d> is invalid!\n", 
			   tIn.m_tNetSyn.GetNetSynDTSlot(), tIn.m_tNetSyn.GetNetSynE1Index());
		return FALSE;
	}

	tOut.m_tNetSyn.SetMode( tIn.m_tNetSyn.GetNetSynMode());
	tOut.m_tNetSyn.SetDTSlot( tIn.m_tNetSyn.GetNetSynDTSlot());
	tOut.m_tNetSyn.SetE1Index( tIn.m_tNetSyn.GetNetSynE1Index());

	//dcs 
	tOut.m_dwDcsIp = htonl(tIn.m_tDcs.GetDcsEqpIp());

    // guzh [1/12/2007] 8000B ��DSC������ģ���������Ϣ
    tOut.m_tDscInfo.SetMcuInnerIp( tIn.m_tDscInfo.GetMcuInnerIp() );
    tOut.m_tDscInfo.SetDscInnerIp( tIn.m_tDscInfo.GetDscInnerIp() );
    tOut.m_tDscInfo.SetMcuPort( tIn.m_tDscInfo.GetMcuPort() );
    tOut.m_tDscInfo.SetInnerIpMask( tIn.m_tDscInfo.GetInnerIpMask() );
	tOut.m_tDscInfo.SetNetType( tIn.m_tDscInfo.GetNetType() );
	tOut.m_tDscInfo.SetLanMtProxyIpPos( tIn.m_tDscInfo.GetLanMtProxyIpPos() );

    tOut.m_tDscInfo.SetStartMp(tIn.m_tDscInfo.IsStartMp());
    tOut.m_tDscInfo.SetStartMtAdp(tIn.m_tDscInfo.IsStartMtAdp());
    tOut.m_tDscInfo.SetStartGk(tIn.m_tDscInfo.IsStartGk());
    tOut.m_tDscInfo.SetStartProxy(tIn.m_tDscInfo.IsStartProxy());
    tOut.m_tDscInfo.SetStartDcs(tIn.m_tDscInfo.IsStartDcs());

	TMINIMCUNetParamAll tNetParamAll;
	tIn.m_tDscInfo.GetCallAddrAll( tNetParamAll );
	tOut.m_tDscInfo.SetCallAddrAll( tNetParamAll );

	memset( &tNetParamAll, 0, sizeof(tNetParamAll) );
	tIn.m_tDscInfo.GetMcsAccessAddrAll( tNetParamAll );
	tOut.m_tDscInfo.SetMcsAccessAddrAll( tNetParamAll );

	//Login info
	// д��Ϊ admin, zgc, 2007-10-16
	//tOut.m_tLoginInfo.SetUser( tIn.m_tLoginInfo.GetUser() );
	tOut.m_tLoginInfo.SetUser( (LPCSTR)"admin" );
	s8 achPwd[MAXLEN_PWD+1];
	memset(achPwd, 0, sizeof(achPwd));
	tIn.m_tLoginInfo.GetPwd(achPwd, sizeof(achPwd));
	tOut.m_tLoginInfo.SetPwd( (LPCSTR)achPwd );

	//  [1/8/2010 pengjie] Modify ������չ��Ϣת��
	tOut.m_tEqpExCfgInfo = tIn.m_tEqpExCfgInfo;
	// End Modify

	// [4/1/2011 xliang] ����Ӫ����������
	tOut.m_tMultiManuNetCfg.m_byEnableMultiManuNet = tIn.m_tGkProxyCfg.IsProxyUsed();
	tOut.m_tMultiManuNetCfg.SetGkIp( tIn.m_tGkProxyCfg.GetGkIpAddr() );
	tOut.m_tMultiManuNetCfg.m_tMultiManuNetAccess = tIn.m_tMultiManuNetAccess;

	return TRUE;
}

/*=============================================================================
  �� �� ���� BrdTypeOut2In
  ��    �ܣ� ������ת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byType
  �� �� ֵ�� u8   
=============================================================================*/
// u8  CMcuCfgInst::BrdTypeOut2In(u8 byType)
// {
//     u8 byInType = 0;
//     switch(byType)
//     {
//     case DSL8000_BRD_MPC:
//         byInType = BRD_TYPE_MPC;
//         break;
//     case DSL8000_BRD_CRI:
//         byInType = BRD_TYPE_CRI;
//         break;
//     case DSL8000_BRD_DRI:
//         byInType = BRD_TYPE_DRI;
//         break;
//     case DSL8000_BRD_APU:
//         byInType = BRD_TYPE_APU;
//         break;
// 	//tianzhiyong 2010/03/21 ����EAPU���ͻ�����
// 	case BRD_TYPE_EAPU:
//         byInType = BRD_TYPE_EAPU;
//         break;
//     case DSL8000_BRD_VPU:
//         byInType = BRD_TYPE_VPU;
//         break;
//     case DSL8000_BRD_DEC5:
//         byInType = BRD_TYPE_DEC5;
//         break;
// 	case DSL8000_BRD_16E1:
// 		byInType = BRD_TYPE_DRI;
//         break;	
//     case DSL8000_BRD_MMP:
//         byInType = BRD_TYPE_MMP;
//         break;
//     case DSL8000_BRD_DTI:
//         byInType = BRD_TYPE_DTI;
//         break;
//     case DSL8000_BRD_DIC:
//         byInType = BRD_TYPE_DIC;
//         break;
//     case DSL8000_BRD_VAS:
//         byInType = BRD_TYPE_VAS;
//         break;
//     case DSL8000_BRD_IMT:
//         byInType = BRD_TYPE_IMT;
//         break;
//     case DSL8000_BRD_DSI:
//         byInType = BRD_TYPE_DSI;
//         break;
// 	case KDV8000B_MODULE:
// 		byInType = BRD_TYPE_DSC;
// 		break;
// 	case DSL8000_BRD_MDSC:
// 		byInType = BRD_TYPE_MDSC;
// 		break;
//     case DSL8000_BRD_HDSC:
//         byInType = BRD_TYPE_HDSC;
//         break;
// 	//4.6 �汾 �¼�  jlb
// 	case BRD_HWID_DSL8000_MPU:
// 		byInType = BRD_TYPE_MPU;
// 		break;
// 	case BRD_HWID_EBAP:
// 		byInType = BRD_TYPE_EBAP;
// 		break;
// 	case BRD_HWID_EVPU:
// 		byInType = BRD_TYPE_EVPU;
// 		break;
// 	case BRD_HWID_KDM200_HDU:
// 		byInType = BRD_TYPE_HDU;
// 		break;
//     default:
//         LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[BrdTypeOut2In] invalid brd type:%d\n", byType);
//         break;
//     }
//     
//     return byInType;
// }

/*=============================================================================
  �� �� ���� BrdTypeIn2Out
  ��    �ܣ� ������ת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byType
  �� �� ֵ�� u8  
=============================================================================*/
// u8  CMcuCfgInst::BrdTypeIn2Out(u8 byType)
// {
//     u8 byOutType = 0;
//     switch(byType)
//     {
//     case BRD_TYPE_MPC:
//         byOutType = DSL8000_BRD_MPC;
//         break;
//     case BRD_TYPE_CRI:
//         byOutType = DSL8000_BRD_CRI;
//         break;
//     case BRD_TYPE_DRI:
//         byOutType = DSL8000_BRD_DRI;
//         break;
//     case BRD_TYPE_16E1:
//         byOutType = DSL8000_BRD_16E1;
//         break;
//     case BRD_TYPE_APU:
//         byOutType = DSL8000_BRD_APU;
//         break;
// 	//tianzhiyong 2010/03/21 ����EAPU���Ͱ�
// 	case BRD_TYPE_EAPU:
//         byOutType = BRD_TYPE_EAPU;
//         break;
//     case BRD_TYPE_VPU:
//         byOutType = DSL8000_BRD_VPU;
//         break;
//     case BRD_TYPE_DEC5:
//         byOutType = DSL8000_BRD_DEC5;
//         break;
//     case BRD_TYPE_MMP:
//         byOutType = DSL8000_BRD_MMP;
//         break;
//     case BRD_TYPE_DTI:
//         byOutType = DSL8000_BRD_DTI;
//         break;
//     case BRD_TYPE_DIC:
//         byOutType = DSL8000_BRD_DIC;
//         break;
//     case BRD_TYPE_VAS:
//         byOutType = DSL8000_BRD_VAS;
//         break;
//     case BRD_TYPE_IMT:
//         byOutType = DSL8000_BRD_IMT;
//         break;
//     case BRD_TYPE_DSI:
//         byOutType = DSL8000_BRD_DSI;
//         break;
// 	case BRD_TYPE_DSC:
// 		byOutType = KDV8000B_MODULE;
// 		break;
// 	case BRD_TYPE_MDSC:
// 		byOutType = DSL8000_BRD_MDSC;
// 		break;
//     case BRD_TYPE_HDSC:
//         byOutType = DSL8000_BRD_HDSC;
//         break;
// 	//4.6�¼Ӱ汾���� jlb
// 	case BRD_TYPE_HDU:
// 		byOutType = BRD_HWID_KDM200_HDU;
// 		break;
// 	case BRD_TYPE_MPU:
// 		byOutType = BRD_HWID_DSL8000_MPU;
//         break;
// 	case BRD_TYPE_EBAP: 
// 		byOutType = BRD_HWID_EBAP;
// 		break;
// 	case BRD_TYPE_EVPU:
// 		byOutType = BRD_HWID_EVPU;
//         break;
//     default:
//         LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[BrdTypeIn2Out] invalid brd type:%d\n", byType);
//         break;
//     }
// 
//     return byOutType;
// }

/*=============================================================================
  �� �� ���� BrdStatusIn2Out
  ��    �ܣ� ��������״̬��VCת�������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byStatus
  �� �� ֵ�� u8  
=============================================================================*/
u8  CMcuCfgInst::BrdStatusIn2Out( u8 byStatus )
{
    u8 byStatusOut = 0;
    switch( byStatus )
    {
    case BOARD_INLINE:      byStatusOut = BOARD_STATUS_INLINE;      break;
    case BOARD_OUTLINE:     byStatusOut = BOARD_STATUS_DISCONNECT;  break;
    case BOARD_UNKNOW:      byStatusOut = BOARD_STATUS_UNKNOW;      break;

    // FIXME: ��ת��Ŀǰ�����õ�. �������ã��ȴ�������չ.
    case BOARD_CFGCONFLICT: byStatusOut = BOARD_STATUS_CFGCONFLICT; break;
    default:                byStatusOut = BOARD_STATUS_UNKNOW;      break;
    }
    return byStatusOut;
}

/*=============================================================================
  �� �� ���� BrdStatusOut2In
  ��    �ܣ� ��������״̬�Ӵ����ת��VC
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byStatus
  �� �� ֵ�� u8  
=============================================================================*/
u8  CMcuCfgInst::BrdStatusOut2In( u8 byStatus )
{
    u8 byStatusOut = 0;
    switch( byStatus )
    {
    case BOARD_STATUS_INLINE:       byStatusOut = BOARD_INLINE;    break;
    case BOARD_STATUS_DISCONNECT:   byStatusOut = BOARD_OUTLINE;   break;
        
    // FIXME: ��ת��Ŀǰ�Ѿ��ܵ�. ����תΪ BOARD_CFGCONFLICT �ģ�
    //        ������ѭ��ǰ���߼�תΪ BOARD_UNKNOW���ȴ�������չ��
    case BOARD_STATUS_CFGCONFLICT:  byStatusOut = BOARD_UNKNOW;    break;
    case BOARD_STATUS_UNKNOW:       byStatusOut = BOARD_UNKNOW;    break;
    default:                        byStatusOut = BOARD_UNKNOW;    break;
    }
    return byStatusOut;
}

/*=============================================================================
  �� �� ���� GetSlotType
  ��    �ܣ� �Ӱ����͵õ�������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byBrdType
  �� �� ֵ�� u8  
=============================================================================*/
u8  CMcuCfgInst::GetSlotType(u8 byBrdType)
{
    u8 bySlotType = 0;

    switch(byBrdType)
    {
	//  [1/21/2011 chendaiwei]֧��MPC2
    case BRD_TYPE_MPC/*DSL8000_BRD_MPC*/:
	case BRD_TYPE_MPC2:
        bySlotType = SLOTTYPE_MC;
    	break;

	case BRD_TYPE_HDU/*BRD_HWID_KDM200_HDU*/:
    case BRD_TYPE_HDU_L:
	case BRD_TYPE_HDU2:
	case BRD_TYPE_HDU2_L:
	case BRD_TYPE_HDU2_S:
	bySlotType = SLOTTYPE_TVSEX;
		break;
	//[2011/01/25 zhushz]IS2.x����֧��
	case BRD_TYPE_IS21:
	case BRD_TYPE_IS22:
		bySlotType = SLOTTYPE_IS;
		break;
    
    default:
        bySlotType = SLOTTYPE_EX;
        break;
    }

    return bySlotType;
}

/*=============================================================================
  �� �� ���� ProcMcsGetVmpScheme
  ��    �ܣ� ȡ����ϳɷ�����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage* pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMcuCfgInst::ProcMcsGetVmpScheme(const CMessage* pcMsg)
{
	CServMsg cVmpSchemeMsg(pcMsg->content, pcMsg->length);
	u8 byVmpSchemeNum = 0;
	TVmpAttachCfg atVmpSchemeTable[MAX_VMPSTYLE_NUM];
	u16 wRet = g_cMcuAgent.ReadVmpAttachTable(&byVmpSchemeNum, atVmpSchemeTable);
	if (SUCCESS_AGENT == wRet)
	{
		TVmpStyleCfgInfo atVmpSchemeInfo[MAX_VMPSTYLE_NUM];
		for (u8 byLp = 0; byLp < byVmpSchemeNum; byLp++)
		{
			atVmpSchemeInfo[byLp].SetBackgroundColor(atVmpSchemeTable[byLp].GetBGDColor());
			atVmpSchemeInfo[byLp].SetFrameColor(atVmpSchemeTable[byLp].GetFrameColor());
			atVmpSchemeInfo[byLp].SetChairFrameColor(atVmpSchemeTable[byLp].GetChairFrameColor());
			atVmpSchemeInfo[byLp].SetSpeakerFrameColor(atVmpSchemeTable[byLp].GetSpeakerFrameColor());  
			atVmpSchemeInfo[byLp].SetSchemeId(atVmpSchemeTable[byLp].GetIndex());
			atVmpSchemeInfo[byLp].SetFontType(atVmpSchemeTable[byLp].GetFontType());
			atVmpSchemeInfo[byLp].SetFontSize(atVmpSchemeTable[byLp].GetFontSize());
			atVmpSchemeInfo[byLp].SetTextColor(atVmpSchemeTable[byLp].GetTextColor());
			atVmpSchemeInfo[byLp].SetTopicBkColor(atVmpSchemeTable[byLp].GetTopicBGDColor());
			atVmpSchemeInfo[byLp].SetDiaphaneity(atVmpSchemeTable[byLp].GetDiaphaneity());
			atVmpSchemeInfo[byLp].SetSchemeAlias((s8*)atVmpSchemeTable[byLp].GetAlias());
		}
		cVmpSchemeMsg.SetMsgBody((u8*)atVmpSchemeInfo, sizeof(atVmpSchemeInfo));
		SendMsg2Mcsssn(cVmpSchemeMsg, pcMsg->event+1);
	}
	else
	{
		cVmpSchemeMsg.SetErrorCode(wRet);
		SendMsg2Mcsssn(cVmpSchemeMsg, pcMsg->event+2);
	}

	return;
}

//4.6�汾 �¼� jlb
/*=============================================================================
  �� �� ���� ProcMcsGetHduScheme
  ��    �ܣ� ȡHDUԤ��������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage* pcMsg
  �� �� ֵ�� void
------------------------------------------------------------------------------
  �� ��      �� ��      �޸���      �߶���       �޸�����
 2009/01/10   4.6       ���ֱ�                    ����
=============================================================================*/
void  CMcuCfgInst::ProcMcsGetHduScheme(const CMessage* pcMsg)
{
	CServMsg cHduSchemeMsg(pcMsg->content,pcMsg->length);
    u8 byHduSchemeNum =0;
    
	THduStyleInfo atHduStyleInfoTable[MAX_HDUSTYLE_NUM];
	u16 wRet = g_cMcuAgent.ReadHduSchemeTable(&byHduSchemeNum, atHduStyleInfoTable);
	if (SUCCESS_AGENT == wRet)
	{
		THduStyleCfgInfo atHduStyleCfgInfo[MAX_HDUSTYLE_NUM];
		for ( u8 byLp = 0; byLp < byHduSchemeNum; byLp++ )
		{
			THduChnlInfo  atHduChnlInfo[MAXNUM_HDUCFG_CHNLNUM];

			atHduStyleInfoTable[byLp].GetHduChnlTable(atHduChnlInfo);

			THduChnlCfgInfo atHduChnlCfgInfo[MAXNUM_HDUCFG_CHNLNUM];
			memset(atHduChnlCfgInfo, 0x0, sizeof(atHduChnlCfgInfo));
			memcpy( (void*)atHduChnlCfgInfo, (void*)atHduChnlInfo, MAXNUM_HDUCFG_CHNLNUM*sizeof(THduChnlInfo));
			atHduStyleCfgInfo[byLp].SetHduChnlCfgTable( atHduChnlCfgInfo);
		    atHduStyleCfgInfo[byLp].SetTotalChnlNum( atHduStyleInfoTable[byLp].GetTotalChnlNum() );
		    atHduStyleCfgInfo[byLp].SetIsBeQuiet( atHduStyleInfoTable[byLp].GetIsBeQuiet() );
			atHduStyleCfgInfo[byLp].SetSchemeAlias( atHduStyleInfoTable[byLp].GetSchemeAlias() );
		    atHduStyleCfgInfo[byLp].SetStyleIdx( atHduStyleInfoTable[byLp].GetStyleIdx() );
		    atHduStyleCfgInfo[byLp].SetVolumn( atHduStyleInfoTable[byLp].GetVolumn() );
			atHduStyleCfgInfo[byLp].SetHeight( atHduStyleInfoTable[byLp].GetHeight() );
            atHduStyleCfgInfo[byLp].SetWidth( atHduStyleInfoTable[byLp].GetWidth() );
		}
		cHduSchemeMsg.SetMsgBody((u8*)atHduStyleCfgInfo, sizeof(atHduStyleCfgInfo));
		SendMsg2Mcsssn(cHduSchemeMsg, pcMsg->event + 1);
	}
	else
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[ProcMcsGetHduScheme]:ReadHduSchemeTable fail! wRet = %d\n", wRet);
		cHduSchemeMsg.SetErrorCode(wRet);
		SendMsg2Mcsssn(cHduSchemeMsg, pcMsg->event + 2);
	}

    NotifyMcsHduSchemeCfg(cHduSchemeMsg);

	return;
}

/*=============================================================================
  �� �� ���� ProcMcsChangeHduScheme
  ��    �ܣ� �޸�HDUԤ��������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage* pcMsg
  �� �� ֵ�� void 
------------------------------------------------------------------------------
  �� ��      �� ��      �޸���      �߶���       �޸�����
  2009/01/10   4.6       ���ֱ�                    ����
  2011/11/03   4.6       liuxu                    ����
=============================================================================*/
void  CMcuCfgInst::ProcMcsChangeHduScheme(const CMessage* pcMsg)
{
	CServMsg cHduSchemeMsg(pcMsg->content,pcMsg->length);

	// ��ȡ��Ԥ����Ϣ
	THduStyleCfgInfo *ptNewHduStyleInfoTable = (THduStyleCfgInfo*)cHduSchemeMsg.GetMsgBody();
    u8 byNewHduSchemeNum = cHduSchemeMsg.GetMsgBodyLen() / sizeof(THduStyleCfgInfo);
    if (byNewHduSchemeNum > MAX_HDUSTYLE_NUM)
    {
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[CMcuCfgInst]:HduScheme Num is too big!\n");
        MCUCFG_INVALIDPARAM_NACK(cHduSchemeMsg, pcMsg->event + 2, ERR_MCU_CFG_HDUSCHEME_NUM_OVER)
        return;
    }

	// ��ȡ��Ԥ����Ϣ
	u8 byOldHduSchemeNum = 0;
	THduStyleInfo atOldHduSchemeTab[MAX_HDUSTYLE_NUM];
	u16 wReadRet = g_cMcuAgent.ReadHduSchemeTable( &byOldHduSchemeNum, atOldHduSchemeTab );

	if ( SUCCESS_AGENT != wReadRet || byOldHduSchemeNum > MAX_HDUSTYLE_NUM)
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcsChangeHduScheme]: Read hdu scheme fail!\n");
		cHduSchemeMsg.SetErrorCode(wReadRet);
		SendMsg2Mcsssn(cHduSchemeMsg, pcMsg->event + 2);
	}

	// ����Ԥ����Ԥ��������η�������abyOldHduSchemeIdx��
	u8 abyOldHduSchemeIdx[MAX_HDUSTYLE_NUM] = { 0 };
	for ( u8 bySchemeIdxLoop = 0; bySchemeIdxLoop < byOldHduSchemeNum; bySchemeIdxLoop++ )
	{
		abyOldHduSchemeIdx[bySchemeIdxLoop] = atOldHduSchemeTab[bySchemeIdxLoop].GetStyleIdx();
	}

	// �ж�ÿ����Ԥ���Ƿ��ڻ���ģ����
	u8 abyOldSchmeInConf[MAX_HDUSTYLE_NUM] = { 0 };
	JudgeHduSchemeInConf( abyOldHduSchemeIdx, abyOldSchmeInConf,  byOldHduSchemeNum);
    
	// �����Ԥ����ͨ�����ڱ�ʹ�ã����ܱ��޸�
	THduChnlInfo tOldHduChnlInfo[MAXNUM_HDUCFG_CHNLNUM];
	THduChnlCfgInfo  atNewHduChnlCfg[MAXNUM_HDUCFG_CHNLNUM];

	THduStyleInfo atFinalHduStyleInfo[MAX_HDUSTYLE_NUM];					// ���ձ����Ԥ����Ϣ

	// ����״̬
	TPeriEqpStatus tHduStatus;						

	// ѭ��������Ԥ��, ���ݾ�Ԥ���жϿɲ������޸�
    for ( u8 byOldSchemeLoop = 0; byOldSchemeLoop < min(byOldHduSchemeNum,MAX_HDUSTYLE_NUM); byOldSchemeLoop ++ )
    {
        const u8 byOldSchmeIdx = atOldHduSchemeTab[byOldSchemeLoop].GetStyleIdx();
		BOOL32 bModified = FALSE;						// ��Ԥ���Ƿ��޸�
		BOOL32 bOldSchmeInUsing = FALSE;				// ��Ԥ���Ƿ����ڱ�����ʹ��

		// �Ƿ������ڻ���ģ����
		const BOOL32 bOldSchmeInConf = 0 < abyOldSchmeInConf[byOldSchemeLoop] ? TRUE : FALSE;
		// �����Ԥ��������ʹ��, ���ǻ����
		u8 byConfidx = abyOldSchmeInConf[byOldSchemeLoop];		

		// ���жϾ�Ԥ���Ƿ����ڱ�����ʹ��
		atOldHduSchemeTab[byOldSchemeLoop].GetHduChnlTable( tOldHduChnlInfo );
		for ( u8 byOldSchmeChnnLoop = 0; byOldSchmeChnnLoop < MAXNUM_HDUCFG_CHNLNUM; byOldSchmeChnnLoop ++ )
		{
			u8 byOldHduEqpId = tOldHduChnlInfo[byOldSchmeChnnLoop].GetEqpId();
			u8 byOldHduChnlIdx = tOldHduChnlInfo[byOldSchmeChnnLoop].GetChnlIdx();
			
			if ( IsValidHduChn(byOldHduEqpId, byOldHduChnlIdx) )
			{
				// ��ȡ��Ӧ����״̬
				g_cMcuVcApp.GetPeriEqpStatus( byOldHduEqpId, &tHduStatus );

				// [2013/05/21 chenbing]
				u8 byChnNum = 0;
				if (HDUCHN_MODE_FOUR == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byOldHduChnlIdx))
				{
					byChnNum = HDU_MODEFOUR_MAX_SUBCHNNUM;
				}
				else
				{
					byChnNum = 1;
				}

				for (u8 bySubChnId = 0; bySubChnId < byChnNum; bySubChnId++)
				{
					// ��ȡͨ��ʹ�õ�Ԥ��
					const u8 byHduChnnlStatus = tHduStatus.m_tStatus.tHdu.GetChnStatus(byOldHduChnlIdx, bySubChnId);
					if ( 0 != tHduStatus.m_tStatus.tHdu.GetConfIdx(byOldHduChnlIdx, bySubChnId) 
						&& (THduChnStatus::eRUNNING == byHduChnnlStatus || 
						THduChnStatus::eWAITSTART == byHduChnnlStatus ||
						THduChnStatus::eWAITSTOP == byHduChnnlStatus ) &&
						tHduStatus.m_tStatus.tHdu.GetSchemeIdx(byOldHduChnlIdx, bySubChnId) == byOldSchmeIdx
						)
					{
						bOldSchmeInUsing = TRUE;
						byConfidx = tHduStatus.m_tStatus.tHdu.GetConfIdx(byOldHduChnlIdx, bySubChnId);
						break;
					}
				}
			}
		}

		// ѭ��������Ԥ���б�, Ѱ����Ԥ���еľ�Ԥ��
		u8 byNewSchmeLoop = 0;
		for ( byNewSchmeLoop = 0; byNewSchmeLoop < min(byNewHduSchemeNum,MAX_HDUSTYLE_NUM); byNewSchmeLoop++)
		{
			const u8 byNewSchmeIdx = ptNewHduStyleInfoTable[byNewSchmeLoop].GetStyleIdx();
			if ( byNewSchmeIdx == byOldSchmeIdx)
			{
				const u8 byOldSchmeChnnlNum = atOldHduSchemeTab[byOldSchemeLoop].GetTotalChnlNum();
				if ( byOldSchmeChnnlNum != ptNewHduStyleInfoTable[byNewSchmeLoop].GetTotalChnlNum() ||
					 atOldHduSchemeTab[byOldSchemeLoop].GetWidth() != ptNewHduStyleInfoTable[byNewSchmeLoop].GetWidth() ||
					 atOldHduSchemeTab[byOldSchemeLoop].GetHeight() != ptNewHduStyleInfoTable[byNewSchmeLoop].GetHeight() ||
					 atOldHduSchemeTab[byOldSchemeLoop].GetVolumn() != ptNewHduStyleInfoTable[byNewSchmeLoop].GetVolumn() ||
					 (atOldHduSchemeTab[byOldSchemeLoop].GetIsBeQuiet() && !ptNewHduStyleInfoTable[byNewSchmeLoop].GetIsBeQuiet()) ||
					 0 != strcmp(atOldHduSchemeTab[byOldSchemeLoop].GetSchemeAlias(),ptNewHduStyleInfoTable[byNewSchmeLoop].GetSchemeAlias())
					)
				{
					bModified = TRUE;
					break;
				}

				ptNewHduStyleInfoTable[byNewSchmeLoop].GetHduChnlCfgTable(atNewHduChnlCfg);
				for ( u8 byNewSchmeChnnlIdx = 0; byNewSchmeChnnlIdx < MAXNUM_HDUCFG_CHNLNUM && !bModified; byNewSchmeChnnlIdx ++ )
				{
					const u8 byOldHduEqpId = tOldHduChnlInfo[byNewSchmeChnnlIdx].GetEqpId();
					const u8 byOldHduChnlIdx = tOldHduChnlInfo[byNewSchmeChnnlIdx].GetChnlIdx();

					const u8 byNewHduEqpId = atNewHduChnlCfg[byNewSchmeChnnlIdx].GetEqpId();
					const u8 byNewHduChnlIdx = atNewHduChnlCfg[byNewSchmeChnnlIdx].GetChnlIdx();
					
					if ( (byOldHduEqpId !=  byNewHduEqpId || byOldHduChnlIdx != byNewHduChnlIdx)
						&& IsValidHduChn(byOldHduEqpId, byOldHduChnlIdx))
					{
						bModified = TRUE;
						break;
					}
				}

				break;
			}
		}

		// �ж���û������Ԥ���б����ҵ���Ԥ����Ӧ��Ԥ��
		if (byNewSchmeLoop >= byNewHduSchemeNum)
		{
			// û�ҵ�, ˵���þ�Ԥ����ɾ����
			bModified = TRUE;
		}

		if ( bModified && ( bOldSchmeInConf || bOldSchmeInUsing))
		{
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcsChangeHduScheme] current hdu scheme:%s is in using, can not change it!\n", 
				atOldHduSchemeTab[byOldSchemeLoop].GetSchemeAlias());
			
			TTemplateInfo tConfTpt;										// ����ģ��
			g_cMcuVcApp.GetTemplate(byConfidx, tConfTpt);				// ��ȡ����ģ��
			cHduSchemeMsg.SetConfId(tConfTpt.m_tConfInfo.GetConfId());	// ����confid
			cHduSchemeMsg.SetConfIdx( byConfidx );						// ����confidx

			MCUCFG_INVALIDPARAM_NACK( cHduSchemeMsg, pcMsg->event + 2, ERR_MCU_CFG_CHANGEHDUSCHEME )
				
			NotifyMcsHduSchemeCfg(cHduSchemeMsg);
			return;
		}
    }
	THduChnlCfgInfo  atHduChnlCfgInfo[MAXNUM_HDUCFG_CHNLNUM];
	THduChnlInfo atHduChnlInfo[MAXNUM_HDUCFG_CHNLNUM];
    for (u8 byIdx = 0; byIdx < min(byNewHduSchemeNum,MAXNUM_HDUCFG_CHNLNUM); byIdx ++)
    {
		memset(atHduChnlCfgInfo, 0x0, sizeof(atHduChnlCfgInfo));
        ptNewHduStyleInfoTable[byIdx].GetHduChnlCfgTable(atHduChnlCfgInfo);
        memset(atHduChnlInfo, 0x0, sizeof(atHduChnlInfo));
        memcpy( (void*)atHduChnlInfo, (void*)atHduChnlCfgInfo, MAXNUM_HDUCFG_CHNLNUM*sizeof(THduChnlCfgInfo));
        BOOL32 bVoidScheme = TRUE;  // �Ƿ�Ϊ��Ԥ��
		for ( u8 byIdx2 = 0; byIdx2 < MAXNUM_HDUCFG_CHNLNUM; byIdx2 ++ )
		{
			if ( atHduChnlCfgInfo[byIdx2].GetEqpId() >= HDUID_MIN &&  atHduChnlCfgInfo[byIdx2].GetEqpId() <= HDUID_MAX)
			{
				bVoidScheme = FALSE;
			}			
		}
        //�������Ԥ���п�Ԥ����NACK
        if ( bVoidScheme )
        {
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcsChangeHduScheme] There is void scheme , can not change!\n");
			cHduSchemeMsg.SetConfIdx( g_cMcuVcApp.GetConfIdx( cHduSchemeMsg.GetConfId() ) );
			MCUCFG_INVALIDPARAM_NACK( cHduSchemeMsg, pcMsg->event + 2, ERR_MCU_CFG_HDUSCHEMEVOID )
			NotifyMcsHduSchemeCfg(cHduSchemeMsg);
			return;
        }

        atFinalHduStyleInfo[byIdx].SetHduChnlTable(atHduChnlInfo);
		atFinalHduStyleInfo[byIdx].SetStyleIdx(ptNewHduStyleInfoTable[byIdx].GetStyleIdx());
        atFinalHduStyleInfo[byIdx].SetTotalChnlNum(ptNewHduStyleInfoTable[byIdx].GetTotalChnlNum());
        atFinalHduStyleInfo[byIdx].SetHeight(ptNewHduStyleInfoTable[byIdx].GetHeight());
        atFinalHduStyleInfo[byIdx].SetWidth(ptNewHduStyleInfoTable[byIdx].GetWidth());
        atFinalHduStyleInfo[byIdx].SetVolumn(ptNewHduStyleInfoTable[byIdx].GetVolumn());
        atFinalHduStyleInfo[byIdx].SetSchemeAlias(ptNewHduStyleInfoTable[byIdx].GetSchemeAlias());
        atFinalHduStyleInfo[byIdx].SetIsBeQuiet(ptNewHduStyleInfoTable[byIdx].GetIsBeQuiet());

    }

	u16 wRet = g_cMcuAgent.WriteHduSchemeTable(byNewHduSchemeNum, atFinalHduStyleInfo);
	if (SUCCESS_AGENT != wRet)
	{
	    LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcMcsChangeHduScheme]: write hdu scheme fail!\n");
		cHduSchemeMsg.SetErrorCode(wRet);
	    SendMsg2Mcsssn(cHduSchemeMsg, pcMsg->event + 2);
		return;
	}
	// �������: [pengguofeng 4/16/2013]
	// ��Ȼ��������Ŀ�ط�������ʵ����ֻ��һ���������õ����ö��ѡ�����
#ifdef _UTF8
	u8 byEncoding = 0;
	g_cMcuAgent.ReadMcuEncodingType(byEncoding);
	if ( byEncoding != emenCoding_Utf8 )
	{
		g_cMcuAgent.WriteMcuEncodingType(emenCoding_Utf8);
	}
#endif
    SendMsg2Mcsssn(cHduSchemeMsg, pcMsg->event + 1);

    NotifyMcsHduSchemeCfg(cHduSchemeMsg);
	
	return;
}

/*=============================================================================
    �� �� ���� ProcMcuSynMcsCfgNtf
    ��    �ܣ� ���������л�ʱ,�µ����ð彫���滯������Ϣ����֪ͨ��ǰ���л��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/31  4.0			����                  ����
=============================================================================*/
void CMcuCfgInst::ProcMcuSynMcsCfgNtf( void )
{
	NotifyMcsCpuPercentage();
	NotifyMcsEqpCfg();
	NotifyMcsBrdCfg();
	NotifyMcsGeneralCfg();

	return;
}

/*=============================================================================
�� �� ���� ProcDscCfgWaitingChangeOverTimerOut
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/2   4.0			�ܹ��                  ����
=============================================================================*/
void CMcuCfgInst::ProcDscCfgWaitingChangeOverTimerOut( void )
{
	KillTimer(MCUCFG_DSCCFG_WAITINGCHANGE_OVER_TIMER);
	CServMsg cServMsg;
	memcpy( &cServMsg, m_abyServMsgHead, SERV_MSGHEAD_LEN );
	MCUCFG_INVALIDPARAM_NACK(cServMsg, MCU_MCS_CHANGEBRDCFG_NACK, ERR_MCU_CFG_WRITE_DSC);
/*
#ifdef _MINIMCU_
	LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG,  "[ProcDscCfgWaitingChangeOverTimerOut] Timer over!\n");

	TDSCModuleInfo tLastDscInfo;
	g_cMcuAgent.GetLastDscInfo( &tLastDscInfo );
	g_cMcuAgent.SetDscInfo( &tLastDscInfo, TRUE, FALSE );

	//brd cfg
	u32 dwMpcBrdIp = g_cMcuAgent.GetMpcIp(); // ������
	u32 dwDscExtIp = tLastDscInfo.GetDscExtIp(); // ������
	TBoardInfo atBrdInfo[MAXNUM_BOARD];
	u8 byBrdNum = 0;
	u16 wRet = g_cMcuAgent.ReadBrdTable(&byBrdNum, atBrdInfo);
	if (SUCCESS_AGENT != wRet || 0 == byBrdNum || byBrdNum > MAXNUM_BOARD)
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcDscCfgWaitingChangeOverTimerOut] read brd info failed! brd num :%d, wRet :%d\n", byBrdNum, wRet);
	}
	for( u8 byLop = 0; byLop < byBrdNum; byLop++ )
	{
		if ( atBrdInfo[byLop].GetType() ==  DSL8000_BRD_MPC )
		{
			atBrdInfo[byLop].SetBrdIp( ntohl(dwMpcBrdIp) );
			continue;
		}
		if ( atBrdInfo[byLop].GetType() == KDV8000B_MODULE 
			|| atBrdInfo[byLop].GetType() == DSL8000_BRD_MDSC 
			|| atBrdInfo[byLop].GetType() == DSL8000_BRD_HDSC
		   )
		{
			atBrdInfo[byLop].SetBrdIp( dwDscExtIp );
			continue;
		}
	}
	wRet = g_cMcuAgent.WriteBrdTable( byBrdNum, atBrdInfo );
	if ( SUCCESS_AGENT != wRet )
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcDscCfgWaitingChangeOverTimerOut] Write brd info failed! \n" );
	}
	
	CServMsg cMsg;
	memcpy( &cMsg, m_abyServMsgHead, SERV_MSGHEAD_LEN );
	cMsg.SetMsgBody(NULL, 0);
	cMsg.SetErrorCode(ERR_MCU_CFG_WRITE_DSC); 
	SendMsg2Mcsssn(cMsg, MCU_MCS_CHANGEBRDCFG_NACK); 
	memset( m_abyServMsgHead, 0, SERV_MSGHEAD_LEN );
	NEXTSTATE(STATE_IDLE);

	return;
#endif
*/
}

/*=============================================================================
�� �� ���� ProcMcsDisconnectNotif
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * const pcMsg
�� �� ֵ�� void  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/11/9   4.0		�ܹ��                  ����
=============================================================================*/
void  CMcuCfgInst::ProcMcsDisconnectNotif(const CMessage * const pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

#ifdef _MINIMCU_
	
	if ( STATE_IDLE == CurState() )
	{
		return;
	}
	else
	{
		CServMsg cTempMsg;
		memcpy( &cTempMsg, m_abyServMsgHead, SERV_MSGHEAD_LEN );
		if ( cServMsg.GetSrcSsnId() != cTempMsg.GetSrcSsnId() )
		{
			return;
		}
		else
		{
			memset( m_abyServMsgHead, 0, sizeof(SERV_MSGHEAD_LEN) );
			NEXTSTATE(STATE_IDLE);
		}
	}
#endif
	printf("[ProcMcsDisconnectNotif]Src SSn Id %u\n",cServMsg.GetSrcSsnId());
	return;
}

/*=============================================================================
�� �� ���� ProcSetDscInfoRsp
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * const pcMsg
�� �� ֵ�� void  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/16   4.0			�ܹ��                  ����
=============================================================================*/
void  CMcuCfgInst::ProcSetDscInfoRsp( const CMessage * const pcMsg )
{
	KillTimer(MCUCFG_DSCCFG_WAITINGCHANGE_OVER_TIMER);

	CServMsg cServMsg;
	memcpy( &cServMsg, m_abyServMsgHead, SERV_MSGHEAD_LEN );
	switch(pcMsg->event) 
	{
	case AGT_SVC_SETDSCINFO_ACK:
		SendMsg2Mcsssn( cServMsg, MCU_MCS_CHANGEBRDCFG_ACK);
		break;
	case AGT_SVC_SETDSCINFO_NACK:
		MCUCFG_INVALIDPARAM_NACK(cServMsg, MCU_MCS_CHANGEBRDCFG_NACK, ERR_MCU_CFG_WRITE_DSC);
		break;
	default:
		break;
	}
	return;
}

/*=============================================================================
�� �� ���� SendExtMcuStatus2Mcsssn
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage* pcMsg
�� �� ֵ�� void  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2011/09/09   4.0			��־��                  ����
=============================================================================*/
void   CMcuCfgInst::SendExtMcuStatus2Mcsssn(u8 byMcsId)
{		
	TMcuStatus tMcuStatus;
	if (!g_cMcuVcApp.GetMcuCurStatus(tMcuStatus))
	{
		return;
	}
	
	u8		byExtPeriEqpNum = 0;	                       
	TEqp	atPeriEqp[MAXNUM_MCU_PERIEQP - MAXNUM_OLDMCU_PERIEQP];	    
	u8      byEqpOnline[MAXNUM_MCU_PERIEQP -MAXNUM_OLDMCU_PERIEQP];     
	u32     dwPeriEqpIpAddr[MAXNUM_MCU_PERIEQP - MAXNUM_OLDMCU_PERIEQP]; 
	if ( !g_cMcuVcApp.GetMcuExtCurStatus(MAXNUM_OLDMCU_PERIEQP+1, byExtPeriEqpNum, atPeriEqp, byEqpOnline, dwPeriEqpIpAddr) )
	{
		OspPrintf(TRUE,FALSE, "[SendExtMcuStatusNotify]GetMcuExtCurStatus failed\n");
		return;
	}
	
	CServMsg cServMsg;
	cServMsg.SetSrcSsnId(byMcsId);
	if ( byExtPeriEqpNum >0 )
	{
		cServMsg.SetMsgBody(&byExtPeriEqpNum,sizeof(u8));
		u8 nLoop = 0;
		for ( nLoop = 0; nLoop < byExtPeriEqpNum; nLoop++ )
		{
			cServMsg.CatMsgBody( (u8*)atPeriEqp + nLoop*(sizeof(TEqp)), sizeof(TEqp) );
		}
		
		for ( nLoop = 0; nLoop < byExtPeriEqpNum; nLoop++ )
		{
			cServMsg.CatMsgBody( (u8*)byEqpOnline + nLoop*(sizeof(u8)), sizeof(u8) );
		}
		for ( nLoop = 0; nLoop < byExtPeriEqpNum; nLoop++ )
		{
			cServMsg.CatMsgBody( (u8*)atPeriEqp + nLoop*(sizeof(u32)), sizeof(u32) );	
		}
		
		SendMsg2Mcsssn(cServMsg,MCU_MCS_MCUEXTSTATUS_NOTIFY,TRUE);
	}
	
	return;
}

/*=============================================================================
�� �� ���� JudgeHduSchemeInConf
��    �ܣ� �ж�bySchemeNum��hduԤ���Ƿ��ڴ��ڻ���ģ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� [in] pbySchemeIdx, ��ʾԤ����ŵ�����
[out] pbySchemeInConf, ��ʾpbySchemeIdx����ͬλ�õ�Ԥ�����ڵĻ����
[in] bySchemeNum, ��ʾ�ж��ٸ�Ԥ��Ҫ�ж�
�� �� ֵ�� true, ����; false, ������  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2011/11/02  4.6			����                  ����
=============================================================================*/
void CMcuCfgInst::JudgeHduSchemeInConf( const u8* pbySchemeIdx, u8* const pbySchemeInConf, const u8 bySchemeNum )
{
	// ������֤
	if ( !pbySchemeIdx || !pbySchemeInConf || 0 == bySchemeNum)
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CFG, "JudgeHduSchemeInConf fail, invalid param\n");		
		return;
	}
	
	// ����pbySchemeInConf
	memset(pbySchemeInConf, 0, bySchemeNum);
	
	TTemplateInfo tConfTpt;					// ����Ĥ��
	
	// ѭ��������������ģ��
	for ( u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx ++ )
	{
		// ��ȡ����ģ��
		if (!g_cMcuVcApp.GetTemplate(byConfIdx, tConfTpt))
		{
			continue;
		}
		
		for ( u8 bySchemeLoop = 0; bySchemeLoop < bySchemeNum; bySchemeLoop++ )
		{
			if ( 0 == pbySchemeInConf[bySchemeLoop]								// ����Ѿ��ڻ�������, �Ͳ�Ҫ������
				&& IsHduSchmeInConf(pbySchemeIdx[bySchemeLoop], tConfTpt))
			{
				pbySchemeInConf[bySchemeLoop] = tConfTpt.m_byConfIdx;
				//break;//Bug00159163���޸�����ʹ��HDU2-S����ǽԤ�������ƣ������޸ĳɹ�
			}
		}	
    }
}


/*=============================================================================
�� �� ���� IsHduSchmeInConf
��    �ܣ� �ж�ĳ��hduԤ���Ƿ��ڴ���ĳ������ģ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� true, ����; false, ������  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2011/11/02  4.6			����                  ����
=============================================================================*/
BOOL32	CMcuCfgInst::IsHduSchmeInConf( const u8 bySchemeIdx, const TTemplateInfo& tTemplateInfo ) const 
{
	// �ж��Ƿ�Ϊ��
	if(tTemplateInfo.IsEmpty())
	{
		return FALSE;
	}
	
	// ����vcs����ģ��
	if (tTemplateInfo.m_tConfInfo.GetConfSource() == VCS_CONF)
	{
		return FALSE;
	}
	
	// ��ȡ�������ǽ��Ϣ
	const TMultiTvWallModule& tConfTvWall = tTemplateInfo.m_tMultiTvWallModule;
	TTvWallModule tTvWallModule;
	
	for( u8 byTvLp = 0; byTvLp < tConfTvWall.GetTvModuleNum(); byTvLp++ )
	{
		tConfTvWall.GetTvModuleByIdx(byTvLp, tTvWallModule);
		
		const u8 byEqpType  = tTvWallModule.GetTvEqp().GetEqpType();			
		const u8 byTvWallId = tTvWallModule.GetTvEqp().GetEqpId();
		
		if (EQP_TYPE_HDU_SCHEME == byEqpType && bySchemeIdx == byTvWallId )
			return TRUE;
	}
	
	return FALSE;
}

/*=============================================================================
�� �� ���� ProcDscRegSucceedNotif
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage* pcMsg
�� �� ֵ�� void  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/8/24   4.0			�ܹ��                  ����
=============================================================================*/
void  CMcuCfgInst::ProcDscRegSucceedNotif( const CMessage* pcMsg )
{
	if ( NULL == pcMsg )
	{
		return;
	}
#ifdef _MINIMCU_
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	BOOL32 bIsConfiged;
	memcpy( &bIsConfiged, cServMsg.GetMsgBody(), sizeof(BOOL32) );

	TMcuStatus tMcuStatus;
	g_cMcuVcApp.GetMcuCurStatus( tMcuStatus );
	
	cServMsg.SetMsgBody((u8 *)&tMcuStatus, sizeof(tMcuStatus));
	SendMsg2Mcsssn(cServMsg, MCU_MCS_MCUSTATUS_NOTIF, TRUE);

	//mcu������Ϣ״̬֪ͨ
	SendExtMcuStatus2Mcsssn (cServMsg.GetSrcSsnId());

	NotifyMcsBrdCfg();

	if ( !bIsConfiged )
	{
		cServMsg.SetMsgBody();
		SendMsg2Mcsssn(cServMsg, MCU_MCS_UNCFGDSCREG_NOTIF, TRUE);
	}
#endif
}

/*=============================================================================
    �� �� ���� NotifyMcsCpuPercentage
    ��    �ܣ� ֪ͨ��ǰ���л�� ��ǰmcu��cpuռ������Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/31  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuCfgInst::NotifyMcsCpuPercentage( void )
{
	CServMsg cMsg;
	
	u8 byPercentage = g_cMcuAgent.GetCpuRate();
	LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[NotifyMcsCpuPercentage] cpu percentage :%d%\n", byPercentage);

	//notify all mcs
	cMsg.SetMsgBody(&byPercentage, sizeof(byPercentage));
	SendMsg2Mcsssn(cMsg, MCU_MCS_CPUPERCENTAGE_NOTIF, TRUE);

	return TRUE;
}

/*=============================================================================
    �� �� ���� NotifyMcsEqpCfg
    ��    �ܣ� ֪ͨ��ǰ���л�� ��ǰmcu���豸������Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/31  4.0			����                  ����
	2010/03/01	4.6			Ѧ��					����8000E
=============================================================================*/
BOOL32 CMcuCfgInst::NotifyMcsEqpCfg( void )
{
	CServMsg cMsg;
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	TMcu8KECfg tMcu8KECfg;
	g_cMcuAgent.GetMcuEqpCfg(&tMcu8KECfg);
	
	// [1/18/2010 xliang]  ������������Ϣ
// 	TMcuEqpCfg tEqpCfg;
//     ReConstructTMcuEqpCfgForMcs( tMcu8KECfg, tEqpCfg);
#ifdef _8KI_
	TMcuNetCfg tTmpEqpCfg;
	ReConstructTMcuNetCfgForMcs( tMcu8KECfg, tTmpEqpCfg);

	TMcu8KiNetCfg tEqpCfg;
	TMcuEqpCfg *ptMcuEqpCfg = tTmpEqpCfg.GetMcuEqpCfg(0);
	tEqpCfg.SetMcuEqpCfg(0,*ptMcuEqpCfg);
	tEqpCfg.SetInterface(tTmpEqpCfg.GetInterface());
	tEqpCfg.SetUseModeType(tMcu8KECfg.GetUseModeType());
	tEqpCfg.SetNetworkType(tMcu8KECfg.GetNetworkType());
	tEqpCfg.SetIsUseSip(tMcu8KECfg.IsUseSip());
	tEqpCfg.SetSipInEthIdx(tMcu8KECfg.GetSipInEthIdx());
	tEqpCfg.SetSipIpAddr(tMcu8KECfg.GetSipIpAddr());
	tEqpCfg.SetSipMaskAddr(tMcu8KECfg.GetSipMaskAddr());
	tEqpCfg.SetSipGwAddr(tMcu8KECfg.GetSipGwAddr());
#else
	TMcuNetCfg tEqpCfg;
	ReConstructTMcuNetCfgForMcs( tMcu8KECfg, tEqpCfg);
#endif

#else
	TMcuEqpCfg tEqpCfg;
	u8 byLocalLayer = 0;
	u8 byLocalSlot  = 0;
	g_cMcuAgent.GetLocalLayer(byLocalLayer);
	g_cMcuAgent.GetLocalSlot(byLocalSlot);
	//[2011/02/11 zhushz] ���mcs�޸�����������,�ϱ��µ�����
	if (g_cMcuAgent.IsMpcNetCfgBeModifedByMcs())
	{
		TMcuNewNetCfg tMcuNewNetInfo;
		g_cMcuAgent.GetNewNetCfg(tMcuNewNetInfo);

		tEqpCfg.SetMcuIpAddr(ntohl(tMcuNewNetInfo.GetNewMpcIP()));
		tEqpCfg.SetMcuSubNetMask(ntohl(tMcuNewNetInfo.GetNewMpcMaskIp()));
		tEqpCfg.SetGWIpAddr(ntohl(tMcuNewNetInfo.GetNewMpcDefGateWay()));
		tEqpCfg.SetInterface( tMcuNewNetInfo.GetNewMpcInterface() );
	}
	else
	{
		tEqpCfg.SetMcuIpAddr(ntohl(g_cMcuAgent.GetMpcIp()));
		tEqpCfg.SetMcuSubNetMask(ntohl(g_cMcuAgent.GetMaskIp()));
		// g_cMcuAgent.GetGateway()���ؾ��������򣬹ʲ���Ҫntohlת��
		tEqpCfg.SetGWIpAddr( g_cMcuAgent.GetGateway() );
		tEqpCfg.SetInterface( g_cMcuAgent.GetInterface() );
	}
	tEqpCfg.SetLayer(byLocalLayer);
	tEqpCfg.SetSlot(byLocalSlot);
#endif
	
	if (g_bPrintCfgMsg)
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[NotifyMcsEqpCfg] notifying config info as follow:\n");
		tEqpCfg.Print();
	}
	
	//notify all mcs
	cMsg.SetMsgBody((u8*)&tEqpCfg, sizeof(tEqpCfg));
	SendMsg2Mcsssn(cMsg, MCU_MCS_MCUEQPCFG_NOTIF, TRUE);
	
	return TRUE;
}

/*=============================================================================
    �� �� ���� NotifyMcsBrdCfg
    ��    �ܣ� ֪ͨ��ǰ���л�� ��ǰmcu�ĵ���������Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/31  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuCfgInst::NotifyMcsBrdCfg( void )
{
	CServMsg cMsg;
	u16 wRet = 0;

	u8 byBrdNum = 0;
	u8 byMixNum = 0;
	u8 byRecNum = 0;
	u8 byBasNum = 0;
    u8 byBasHDNum = 0;
	u8 byTWNum = 0;
	u8 byVmpNum = 0;
	u8 byPrsNum = 0;
	u8 byMtwNum = 0;
	//4.6�汾 �¼�ģ�� jlb
	u8 byHduNum = 0;
	u8 bySvmpNum = 0;
	u8 byMpuBasNum = 0;
	u8 byVrsRecNum = 0;
	
	s32 nIndex = 0;

	//brd cfg        
	TBoardInfo atBrdInfo[MAXNUM_BOARD];
	wRet = g_cMcuAgent.ReadBrdTable(&byBrdNum, atBrdInfo);

	//[2011/01/25 zhushz]IS2.x����֧��
	//  [12/5/2010 chendaiwei]�����IS2.2�ĵ���������Ϣ�����ϱ�IS2.2�ĵ�����Ϣ
// 	if( atBrdInfo[byBrdNum-1].GetType() == BRD_TYPE_IS22 )
// 	{
// 		memset(&atBrdInfo[byBrdNum-1],0,sizeof(TBoardInfo));
// 
// 		byBrdNum--;
// 	}

	if (SUCCESS_AGENT != wRet || 0 == byBrdNum || byBrdNum > MAX_BOARD_NUM)
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[NotifyMcsBrdCfg] read brd info failed! brd num :%d, wRet :%d\n", byBrdNum, wRet);
		return FALSE;
	}

	TBrdCfgInfo atBrdCfg[MAX_BOARD_NUM];
	BrdCfgOut2In(atBrdInfo, atBrdCfg, byBrdNum); 
	cMsg.SetMsgBody(&byBrdNum, sizeof(u8));
	cMsg.CatMsgBody((u8*)atBrdCfg, byBrdNum*sizeof(TBrdCfgInfo));

	if (g_bPrintCfgMsg)
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[NotifyMcsBrdCfg] notifying config info as follow:\n");
		for (nIndex = 0; nIndex < byBrdNum; nIndex++)
		{
			atBrdCfg[nIndex].Print();
		}
	}

	//mixer cfg
	TEqpMixerInfo atMixerInfo[MAXNUM_PERIEQP];
	wRet = g_cMcuAgent.ReadMixerTable(&byMixNum, atMixerInfo);
	if (SUCCESS_AGENT == wRet)
	{
		if (byMixNum <= MAXNUM_PERIEQP)
		{
			TEqpMixerCfgInfo atMixerCfg[MAXNUM_PERIEQP];
			MixerCfgOut2In(atMixerInfo, atMixerCfg, byMixNum);
			cMsg.CatMsgBody(&byMixNum, sizeof(u8));
			cMsg.CatMsgBody((u8*)atMixerCfg, byMixNum*sizeof(TEqpMixerCfgInfo));

			if (g_bPrintCfgMsg)
			{
				for (nIndex = 0; nIndex < byMixNum; nIndex++)
				{
					atMixerCfg[nIndex].Print();
				}
			}
		}
	}      
	else
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[NotifyMcsBrdCfg] read mixer info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

	//rec cfg
	TEqpRecInfo atRecInfo[MAXNUM_PERIEQP];
	wRet = g_cMcuAgent.ReadRecTable(&byRecNum, atRecInfo);
	if (SUCCESS_AGENT == wRet)
	{
		if (byRecNum <= MAXNUM_PERIEQP)
		{
			TEqpRecCfgInfo atRecCfg[MAXNUM_PERIEQP];
			RecCfgOut2In(atRecInfo, atRecCfg, byRecNum);
			cMsg.CatMsgBody(&byRecNum, sizeof(u8));
			cMsg.CatMsgBody((u8*)atRecCfg, byRecNum*sizeof(TEqpRecCfgInfo));

			if (g_bPrintCfgMsg)
			{
				for (nIndex = 0; nIndex < byRecNum; nIndex++)
				{
					atRecCfg[nIndex].Print();
				}
			}
		}
	}   
	else
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[NotifyMcsBrdCfg] read rec info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

	//tvwall cfg
	TEqpTVWallInfo atTWInfo[MAXNUM_PERIEQP];
	wRet = g_cMcuAgent.ReadTvTable(&byTWNum, atTWInfo);
	if (SUCCESS_AGENT == wRet)
	{
		if (byTWNum <= MAXNUM_PERIEQP)
		{
			TEqpTvWallCfgInfo atTWCfg[MAXNUM_PERIEQP];
			TWCfgOut2In(atTWInfo, atTWCfg, byTWNum);
			cMsg.CatMsgBody(&byTWNum, sizeof(u8));
			cMsg.CatMsgBody((u8*)atTWCfg, byTWNum*sizeof(TEqpTvWallCfgInfo));

			if (g_bPrintCfgMsg)
			{
				for (nIndex = 0; nIndex < byTWNum; nIndex++)
				{
					atTWCfg[nIndex].Print();
				}
			}
		}
	}  
	else
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[NotifyMcsBrdCfg] read tvwall info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

	//bas cfg
	TEqpBasInfo atBasInfo[MAXNUM_PERIEQP];
	wRet = g_cMcuAgent.ReadBasTable(&byBasNum, atBasInfo);
	if (SUCCESS_AGENT == wRet)
	{
		if (byBasNum <= MAXNUM_PERIEQP)
		{
			TEqpBasCfgInfo atBasCfg[MAXNUM_PERIEQP];
			BasCfgOut2In(atBasInfo, atBasCfg, byBasNum);
			cMsg.CatMsgBody(&byBasNum, sizeof(u8));
			cMsg.CatMsgBody((u8*)atBasCfg, byBasNum*sizeof(TEqpBasCfgInfo));

			if (g_bPrintCfgMsg)
			{
				for (nIndex = 0; nIndex < byBasNum; nIndex++)
				{
					atBasCfg[nIndex].Print();
				}
			}
		}
	}        
	else
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[NotifyMcsBrdCfg] read bas info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

	//vmp cfg
	TEqpVMPInfo atVmpInfo[MAXNUM_PERIEQP];
	wRet = g_cMcuAgent.ReadVmpTable(&byVmpNum, atVmpInfo);
	if (SUCCESS_AGENT == wRet)
	{
		if (byVmpNum <= MAXNUM_PERIEQP)
		{
			TEqpVmpCfgInfo atVmpCfg[MAXNUM_PERIEQP];
			VmpCfgOut2In(atVmpInfo, atVmpCfg, byVmpNum);
			cMsg.CatMsgBody(&byVmpNum, sizeof(u8));
			cMsg.CatMsgBody((u8*)atVmpCfg, byVmpNum*sizeof(TEqpVmpCfgInfo));

			if (g_bPrintCfgMsg)
			{
				for (nIndex = 0; nIndex < byVmpNum; nIndex++)
				{
					atVmpCfg[nIndex].Print();
				}
			}
		}
	}        
	else
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[NotifyMcsBrdCfg] read vmp info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

	//prs cfg
	TEqpPrsInfo atPrsInfo[MAXNUM_PERIEQP];
    wRet = g_cMcuAgent.ReadPrsTable(&byPrsNum, atPrsInfo);

	//  [12/5/2010 chendaiwei]�����IS2.2��PRS��Ϣ�����ϱ�IS2.2��PRS��Ϣ
	if( atPrsInfo[byPrsNum-1].GetEqpId() == PRSID_MAX-1)
	{
		memset(&atPrsInfo[byPrsNum-1],0,sizeof(TEqpPrsInfo));
		byPrsNum--;
	}

	if (SUCCESS_AGENT == wRet)
	{
        if(byPrsNum <= MAXNUM_PERIEQP)
        {
            TPrsCfgInfo atPrsCfg[MAXNUM_PERIEQP];
            PrsCfgOut2In(atPrsInfo, atPrsCfg, byPrsNum);
            cMsg.CatMsgBody(&byPrsNum, sizeof(u8));
            cMsg.CatMsgBody((u8*)atPrsCfg, byPrsNum*sizeof(TPrsCfgInfo));
            
            if (g_bPrintCfgMsg)
            {
                for(u8 byLp = 0; byLp < byPrsNum; byLp++)
                {
                    atPrsCfg[byLp].Print();
                }
            }
        }		
	}
	else
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[NotifyMcsBrdCfg] read prs info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

	//mtw cfg
	TEqpMpwInfo atMtwInfo[MAXNUM_PERIEQP];
	wRet = g_cMcuAgent.ReadMpwTable(&byMtwNum, atMtwInfo);
	if (SUCCESS_AGENT == wRet)
	{
		if (byMtwNum <= MAXNUM_PERIEQP)
		{
			TEqpMTvwallCfgInfo atMtwCfg[MAXNUM_PERIEQP];
			MtwCfgOut2In(atMtwInfo, atMtwCfg, byMtwNum);
			cMsg.CatMsgBody(&byMtwNum, sizeof(u8));
			cMsg.CatMsgBody((u8*)atMtwCfg, byMtwNum*sizeof(TEqpMTvwallCfgInfo));

			if (g_bPrintCfgMsg)
			{
				for (nIndex = 0; nIndex < byMtwNum; nIndex++)
				{
					atMtwCfg[nIndex].Print();
				}
			}
		}
	}
	else
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[NotifyMcsBrdCfg] read mtw info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

    //basHD cfg
	TEqpBasHDInfo atBasHDInfo[MAXNUM_PERIEQP];
	wRet = g_cMcuAgent.ReadBasHDTable(&byBasHDNum, atBasHDInfo);
	if (SUCCESS_AGENT == wRet)
	{
		if (byBasHDNum <= MAXNUM_PERIEQP)
		{
			TEqpBasHDCfgInfo atBasHDCfg[MAXNUM_PERIEQP];
			BasHDCfgOut2In(atBasHDInfo, atBasHDCfg, byBasHDNum);
			cMsg.CatMsgBody(&byBasHDNum, sizeof(u8));
			cMsg.CatMsgBody((u8*)atBasHDCfg, byBasHDNum*sizeof(TEqpBasHDCfgInfo));

			if (g_bPrintCfgMsg)
			{
				for (nIndex = 0; nIndex < byRecNum; nIndex++)
				{
					atBasHDCfg[nIndex].Print();
				}
			}
		}
	}   
	else
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[NotifyMcsBrdCfg] read basHD info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

	//4.6�汾 �¼�ģ�� jlb
    //svmp  cfg
	TEqpSvmpInfo atSvmpInfo[MAXNUM_PERIEQP];
	wRet = g_cMcuAgent.ReadSvmpTable(&bySvmpNum, atSvmpInfo);
	if (SUCCESS_AGENT == wRet)
	{
		if ( bySvmpNum <= MAXNUM_PERIEQP )
		{
			TEqpSvmpCfgInfo atSvmpCfg[MAXNUM_PERIEQP];
			SvmpCfgOut2In(atSvmpInfo, atSvmpCfg, bySvmpNum);
			
			cMsg.CatMsgBody(&bySvmpNum, sizeof(u8));
			cMsg.CatMsgBody((u8*)atSvmpCfg, bySvmpNum*sizeof(TEqpSvmpCfgInfo));
			
			if (g_bPrintCfgMsg)
			{
				for (nIndex = 0; nIndex < bySvmpNum; nIndex++)
				{
					atSvmpCfg[nIndex].Print();
				}
			}
		}
	}
	else
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[NotifyMcsBrdCfg] read Svmp info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

    // MpuBas  cfg
	TEqpMpuBasInfo atMpuBasInfo[MAXNUM_PERIEQP];     
	wRet = g_cMcuAgent.ReadMpuBasTable(&byMpuBasNum, atMpuBasInfo);
	if (SUCCESS_AGENT == wRet)
	{
		if ( byMpuBasNum <= MAXNUM_PERIEQP )
		{
			TEqpMpuBasCfgInfo atMpuBasCfg[MAXNUM_PERIEQP];
			MpuBasCfgOut2In(atMpuBasInfo, atMpuBasCfg, byMpuBasNum);
			cMsg.CatMsgBody(&byMpuBasNum, sizeof(u8));
			cMsg.CatMsgBody((u8*)atMpuBasCfg, byMpuBasNum*sizeof(TEqpMpuBasCfgInfo));
			
			if (g_bPrintCfgMsg)
			{
				for (nIndex = 0; nIndex < byMpuBasNum; nIndex++)
				{
					atMpuBasCfg[nIndex].Print();
				}
			}
		}
	}
	else
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[NotifyMcsBrdCfg] read MpuBas info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

	// [6/3/2011 liuxu] Ϊ�����ռ���, ���ｫEbap��Evpu����Ϊ0
	{
		u8 byTemp =0 ;
		cMsg.CatMsgBody(&byTemp, sizeof(u8));			// Ebap����Ϊ0
		cMsg.CatMsgBody(&byTemp, sizeof(u8));			// Evpu����Ϊ0
	}

    // Ebap  cfg
// 	TEqpEbapInfo atEbapInfo[MAXNUM_PERIEQP];     
// 	wRet = g_cMcuAgent.ReadEbapTable(&byEbapNum, atEbapInfo);
// 	if (SUCCESS_AGENT == wRet)
// 	{
// 		if ( byEbapNum <= MAXNUM_PERIEQP )
// 		{
// 			TEqpEbapCfgInfo atEbapCfg[MAXNUM_PERIEQP];
// 			EbapCfgOut2In(atEbapInfo, atEbapCfg, byEbapNum);
// 			cMsg.CatMsgBody(&byEbapNum, sizeof(u8));
// 			cMsg.CatMsgBody((u8*)atEbapCfg, byEbapNum*sizeof(TEqpEbapCfgInfo));
// 			
// 			if (g_bPrintCfgMsg)
// 			{
// 				for (nIndex = 0; nIndex < byEbapNum; nIndex++)
// 				{
// 					atEbapCfg[nIndex].Print();
// 				}
// 			}
// 		}
// 	}
// 	else
// 	{
// 		LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[NotifyMcsBrdCfg] read Ebap info failed, wRet:%d!\n", wRet);
// 		return FALSE;
// 	}

    // Evpu  cfg
// 	TEqpEvpuInfo atEvpuInfo[MAXNUM_PERIEQP];     
// 	wRet = g_cMcuAgent.ReadEvpuTable(&byEvpuNum, atEvpuInfo);
// 	if (SUCCESS_AGENT == wRet)
// 	{
// 		if ( byEvpuNum <= MAXNUM_PERIEQP )
// 		{
// 			TEqpEvpuCfgInfo atEvpuCfg[MAXNUM_PERIEQP];
// 			EvpuCfgOut2In(atEvpuInfo, atEvpuCfg, byEvpuNum);
// 			cMsg.CatMsgBody(&byEvpuNum, sizeof(u8));
// 			cMsg.CatMsgBody((u8*)atEvpuCfg, byEvpuNum*sizeof(TEqpEvpuCfgInfo));
// 			
// 			if (g_bPrintCfgMsg)
// 			{
// 				for (nIndex = 0; nIndex < byEvpuNum; nIndex++)
// 				{
// 					atEvpuCfg[nIndex].Print();
// 				}
// 			}
// 		}
// 	}
// 	else
// 	{
// 		LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[NotifyMcsBrdCfg] read Evpu info failed, wRet:%d!\n", wRet);
// 		return FALSE;
// 	}

    //hdu  cfg
	TEqpHduInfo atHduInfo[MAXNUM_PERIHDU];
	wRet = g_cMcuAgent.ReadHduTable(&byHduNum, atHduInfo);
	if (SUCCESS_AGENT == wRet)
	{
		if ( byHduNum <= MAXNUM_PERIHDU )
		{
			TEqpHduCfgInfo atHduCfg[MAXNUM_PERIHDU];
			HduCfgOut2In(atHduInfo, atHduCfg, byHduNum);
			cMsg.CatMsgBody(&byHduNum, sizeof(u8));
			cMsg.CatMsgBody((u8*)atHduCfg, byHduNum*sizeof(TEqpHduCfgInfo));
			
			if (g_bPrintCfgMsg)
			{
				for (nIndex = 0; nIndex < byHduNum; nIndex++)
				{
					atHduCfg[nIndex].Print();
				}
			}
		}
	}
	else
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[NotifyMcsBrdCfg] read Hdu info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

//     // dvmp  cfg
// 	TEqpDvmpBasicInfo atDvmpInfo[MAXNUM_PERIEQP];      // ��ȡ˫VMPʱ����һ�ζ�16��
// 	wRet = g_cMcuAgent.ReadDvmpTable(&byDvmpNum, atDvmpInfo);
// 	if (SUCCESS_AGENT == wRet)
// 	{
// 		if ( byDvmpNum <= MAXNUM_PERIEQP )
// 		{
// 			TEqpDvmpCfgInfo atDvmpCfgInfo[MAXNUM_PERIDVMP];
// 			TEqpDvmpCfgBasicInfo atDvmpCfg[MAXNUM_PERIEQP];
// 			DvmpCfgOut2In(atDvmpInfo, atDvmpCfg, byDvmpNum);
// 			for ( nIndex=0; nIndex<byDvmpNum; nIndex++ )
// 			{
// 				memcpy( &atDvmpCfgInfo[nIndex/2].tEqpDvmpCfgBasicInfo[0], &atDvmpCfg[nIndex], sizeof(TEqpDvmpCfgBasicInfo) );
//                 memcpy( &atDvmpCfgInfo[nIndex/2].tEqpDvmpCfgBasicInfo[1], &atDvmpCfg[nIndex+1], sizeof(TEqpDvmpCfgBasicInfo) );
// 				nIndex += 2;
// 			}
//             if ( byDvmpNum%2 == 0 )
//             {
//                 byDvmpNum /= 2;
//             }
// 			else                          //��ֻ��������vmpʱ
// 			{
// 				byDvmpNum = byDvmpNum/2 + 1;
// 			}			
// 			cMsg.CatMsgBody(&byDvmpNum, sizeof(u8));
// 			cMsg.CatMsgBody((u8*)atDvmpCfgInfo, byDvmpNum*sizeof(TEqpDvmpCfgInfo));   
// 			
// 			if (g_bPrintCfgMsg)
// 			{
// 				for ( nIndex=0; nIndex<byDvmpNum; nIndex++ )
// 				{
// 					atDvmpCfgInfo[nIndex].tEqpDvmpCfgBasicInfo[0].Print();
// 					atDvmpCfgInfo[nIndex].tEqpDvmpCfgBasicInfo[1].Print();
// 				}
// 			}
// 		}
// 	}
// 	else
// 	{
// 		LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[NotifyMcsBrdCfg] read Dvmp info failed, wRet:%d!\n", wRet);
// 		return FALSE;
// 	}
	
	//vrsrec cfg׷����¼������
	TEqpVrsRecCfgInfo atVrsRecInfo[MAXNUM_PERIEQP];
	wRet = g_cMcuAgent.ReadVrsRecTable(&byVrsRecNum, atVrsRecInfo);
	if (SUCCESS_AGENT == wRet)
	{
		if (byVrsRecNum <= MAXNUM_PERIEQP)
		{
			cMsg.CatMsgBody(&byVrsRecNum, sizeof(u8));
			cMsg.CatMsgBody((u8*)atVrsRecInfo, byVrsRecNum*sizeof(TEqpVrsRecCfgInfo));
			
			if (g_bPrintCfgMsg)
			{
				for (nIndex = 0; nIndex < byVrsRecNum; nIndex++)
				{
					atVrsRecInfo[nIndex].Print();
				}
			}
		}
	}   
	else
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[NotifyMcsBrdCfg] read vrsrec info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

	//notify all mcs
	SendMsg2Mcsssn(cMsg, MCU_MCS_BRDCFG_NOTIF, TRUE);

	return TRUE;
}

/*=============================================================================
    �� �� ���� NotifyMcsHduSchemeCfg
    ��    �ܣ� ֪ͨ��ǰ���л�� ��ǰhduԤ����������Ϣ��������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
   2009/02/10   4.6			���ֱ�                  ����
=============================================================================*/
BOOL32	CMcuCfgInst::NotifyMcsHduSchemeCfg( CServMsg &cMsg )
{
	u16 wRet = 0;
	u8  byHduSchemeNum = 0;
    THduStyleInfo atHduStyleInfo[MAX_HDUSTYLE_NUM];
	
	wRet = g_cMcuAgent.ReadHduSchemeTable(&byHduSchemeNum,atHduStyleInfo);
	if (SUCCESS_AGENT != wRet)
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[NotifyMcsHduSchemeCfg]read hdu Scheme failed!\n");
		return FALSE;
	}

	THduStyleCfgInfo atHduStyleCfgInfo[MAX_HDUSTYLE_NUM];
	memset( atHduStyleCfgInfo, 0x0, sizeof(atHduStyleCfgInfo) );
	for ( s32 nIndex = 0; nIndex < byHduSchemeNum; nIndex++ )
	{
		THduChnlInfo  atHduChnlInfo[MAXNUM_HDUCFG_CHNLNUM];

		atHduStyleInfo[nIndex].GetHduChnlTable(atHduChnlInfo);
			
		THduChnlCfgInfo atHduChnlCfgInfo[MAXNUM_HDUCFG_CHNLNUM];
		memset(atHduChnlCfgInfo, 0x0, sizeof(atHduChnlCfgInfo));
		memcpy( (void*)atHduChnlCfgInfo, (void*)atHduChnlInfo, MAXNUM_HDUCFG_CHNLNUM*sizeof(THduChnlInfo));

		atHduStyleCfgInfo[nIndex].SetHduChnlCfgTable( atHduChnlCfgInfo);
		atHduStyleCfgInfo[nIndex].SetTotalChnlNum( atHduStyleInfo[nIndex].GetTotalChnlNum() );
		atHduStyleCfgInfo[nIndex].SetIsBeQuiet( atHduStyleInfo[nIndex].GetIsBeQuiet() );
		atHduStyleCfgInfo[nIndex].SetSchemeAlias( atHduStyleInfo[nIndex].GetSchemeAlias() );
		atHduStyleCfgInfo[nIndex].SetStyleIdx( atHduStyleInfo[nIndex].GetStyleIdx() );
		atHduStyleCfgInfo[nIndex].SetVolumn( atHduStyleInfo[nIndex].GetVolumn() );
		atHduStyleCfgInfo[nIndex].SetHeight( atHduStyleInfo[nIndex].GetHeight() );
        atHduStyleCfgInfo[nIndex].SetWidth( atHduStyleInfo[nIndex].GetWidth() );				
	}

	cMsg.SetMsgBody((u8*)atHduStyleCfgInfo, byHduSchemeNum*sizeof(THduStyleCfgInfo));

	SendMsg2Mcsssn(cMsg, MCU_MCS_HDUSCHEMEINFO_NOTIF);
		
	return TRUE;

}

/*=============================================================================
    �� �� ���� NotifyMcsGeneralCfg
    ��    �ܣ� ֪ͨ��ǰ���л�� ��ǰmcu��������Ϣ��������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/31  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuCfgInst::NotifyMcsGeneralCfg( void )
{
	CServMsg cMsg;
	u16 wRet = 0;
	u8 byEthNum = MCU_MAXNUM_ADAPTER-1;
	TMultiEthManuNetAccess  tMultiEthManuNetAccess[MAXNUM_ETH_INTERFACE];
	memset(tMultiEthManuNetAccess, 0, sizeof(tMultiEthManuNetAccess));

	TAgentMcuGenCfg tAgentGenCfg;
	wRet = g_cMcuAgent.ReadVmpAttachTable(&tAgentGenCfg.m_byVmpStyleNum, tAgentGenCfg.m_atVmpStyle);
	if (SUCCESS_AGENT != wRet)
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[NotifyMcsGeneralCfg] read vmp style failed, ret:%d!\n", wRet);
	}

    wRet = g_cMcuAgent.GetLocalInfo(&tAgentGenCfg.m_tLocal);
	if (SUCCESS_AGENT != wRet)
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[NotifyMcsGeneralCfg] read local info failed, ret:%d!\n", wRet);
	}

	wRet = g_cMcuAgent.GetNetWorkInfo(&tAgentGenCfg.m_tNet);
	if (SUCCESS_AGENT != wRet)
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[NotifyMcsGeneralCfg] read net info failed, ret:%d!\n", wRet);
	}

	wRet = g_cMcuAgent.ReadTrapTable(&tAgentGenCfg.m_byTrapListNum ,tAgentGenCfg.m_atTrapList);
	if (SUCCESS_AGENT != wRet)
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[NotifyMcsGeneralCfg] read trap info failed, ret:%d!\n", wRet);
	}

	wRet = g_cMcuAgent.GetQosInfo(&tAgentGenCfg.m_tQos);
	if (SUCCESS_AGENT != wRet)
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[NotifyMcsGeneralCfg] read qos info failed, ret:%d!\n", wRet);
	}

	wRet = g_cMcuAgent.GetNetSyncInfo(&tAgentGenCfg.m_tNetSyn);
	if (SUCCESS_AGENT != wRet)
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[NotifyMcsGeneralCfg] read net syn info failed, ret:%d!\n", wRet);
	}

	tAgentGenCfg.m_dwDcsIp = g_cMcuAgent.GetDcsIp();

#ifdef _MINIMCU_
	g_cMcuAgent.GetDscInfo(&tAgentGenCfg.m_tDscInfo);
#endif

	wRet = g_cMcuAgent.GetLoginInfo(&tAgentGenCfg.m_tLoginInfo);
	if (SUCCESS_AGENT != wRet)
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[NotifyMcsGeneralCfg] read telnet login info failed, ret:%d!\n", wRet);
	}
 
	// [pengjie 2010/1/16] Modify
	wRet = g_cMcuAgent.GetEqpExCfgInfo( tAgentGenCfg.m_tEqpExCfgInfo );
	if (SUCCESS_AGENT != wRet)
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[NotifyMcsGeneralCfg] read EqpExCfgInfo failed, ret:%d!\n", wRet);
	}
	// End Modify

	TMcuNewGeneralCfg tGenCfg;
	if (!GeneralCfgOut2In(tAgentGenCfg, tGenCfg))
	{
		return FALSE;
	}
	else
	{
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
		
		//��ȡDMZ������Ϣ
		g_cMcuAgent.GetProxyDMZInfo(tGenCfg.m_tProxyDMZInfo);

		//gk,proxy config
		TGKProxyCfgInfo tGKProxyCfgInfo;
		g_cMcuAgent.GetGkProxyCfgInfo(tGKProxyCfgInfo);
		tGenCfg.m_tGkProxyCfg = tGKProxyCfgInfo;
		
		// prs time span config
		TPrsTimeSpan  tPrsTimeSpan;
		g_cMcuAgent.GetPrsTimeSpanCfgInfo(tPrsTimeSpan);
		tGenCfg.m_tPrsTimeSpanCfg = tPrsTimeSpan;

		// [3/16/2010 xliang] multiNet config
		TMultiNetCfgInfo tMultiNetCfgInfo;
		g_cMcuAgent.GetMultiNetCfgInfo(tMultiNetCfgInfo);
		//GetMultiNetCfgFromSys(tMultiNetCfgInfo);
		tGenCfg.m_tMultiNetCfg = tMultiNetCfgInfo;

		// [4/1/2011 xliang] multi-manufacory net config
		TMultiManuNetAccess  tMultiManuNetAccess;
		g_cMcuAgent.GetMultiManuNetAccess(tMultiManuNetAccess, FALSE, tMultiEthManuNetAccess, &byEthNum);
		tGenCfg.m_tMultiManuNetAccess = tMultiManuNetAccess;	
#endif
		if (g_bPrintCfgMsg)
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[NotifyMcsGeneralCfg] notifying config info as follow:\n");
			tGenCfg.Print();
		}
	}

	//notify to all mcs
	cMsg.SetMsgBody((u8*)&tGenCfg, sizeof(tGenCfg));

#if defined(_8KI_)
	cMsg.CatMsgBody((u8*)&byEthNum, sizeof(byEthNum));

	cMsg.CatMsgBody((u8*)tMultiEthManuNetAccess, sizeof(TMultiEthManuNetAccess) * byEthNum);
#endif

	SendMsg2Mcsssn(cMsg, MCU_MCS_MCUGENERALCFG_NOTIF, TRUE);
	
	return TRUE;
}

/*=============================================================================
  �� �� ���� NotifyMcsBoardStatus
  ��    �ܣ� ֪ͨMcs����״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMcuCfgInst::NotifyMcsBoardStatus(const CMessage * pcMsg)
{
    CServMsg cServMsg;
    if (AGT_SVC_BOARDSTATUS_NOTIFY == pcMsg->event)
    {
        TBrdStatus *ptBrdStatus = (TBrdStatus*)pcMsg->content;
        TBoardStatusNotify tBrdStatusNotify;
        tBrdStatusNotify.SetBrdState( BrdStatusOut2In(ptBrdStatus->byStatus) );
        tBrdStatusNotify.SetBrdLayer(ptBrdStatus->byLayer);
        tBrdStatusNotify.SetBrdSlot( ptBrdStatus->bySlot );
        tBrdStatusNotify.SetBrdType( /*BrdTypeOut2In*/(ptBrdStatus->byType) );
        tBrdStatusNotify.SetBrdOsType( ptBrdStatus->byOsType );

        LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CFG, "Board Layer= %d, Slot= %d, Type= %d, state= %d OsType= %d status notify\n", 
                              tBrdStatusNotify.GetBrdLayer(), tBrdStatusNotify.GetBrdSlot(),
                              tBrdStatusNotify.GetBrdType(), tBrdStatusNotify.GetBrdState(),
                              tBrdStatusNotify.GetBrdOsType());
    
        cServMsg.SetMsgBody((u8*)&tBrdStatusNotify, sizeof(tBrdStatusNotify));
        SendMsg2Mcsssn(cServMsg, MCU_MCS_BOARDSTATUS_NOTIFY, TRUE);
    }
    else if (AGT_SVC_UPDATEBRDVERSION_NOTIF == pcMsg->event)
    {
        //[12/12/2011 zhushengze]
        //������������        
		cServMsg.SetServMsg(pcMsg->content, pcMsg->length);
        u8* achStrBuf  = (u8*)cServMsg.GetMsgBody();  
        u8 byBrdIdx = achStrBuf[0]-'0';//��ֵ���ַ���ӦתΪ����[10/23/2012 chendaiwei]

		u8 byNum = achStrBuf[1] - '0';		
		for( u8 byIdx = 0;byIdx < byNum; ++byIdx )
		{
			if( 2 == (achStrBuf[2+byIdx] - '0') )
			{
				achStrBuf[2+byIdx] = '0';
				cServMsg.SetErrorCode( ERR_MCU_UPDATEBOARD_PIDCHECKERR );
				LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[NotifyMcsBoardStatus]Update pid chk err!!! byNum.%d\n",byNum );
				break;
			}
			if( 3 == (achStrBuf[2+byIdx] - '0') )
			{
				achStrBuf[2+byIdx] = '0';
				cServMsg.SetErrorCode( ERR_MCU_UPDATEBOARD_FILECHECKERR );
				LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[NotifyMcsBoardStatus]Update FILE chk err!!! byNum.%d\n",byNum );
				break;
			}
		}

		TBoardInfo tBrdInfo;
		if ( SUCCESS_AGENT == g_cMcuAgent.GetBrdCfgById(byBrdIdx, &tBrdInfo) )
		{
			//����������ϱ����е�������֪ͨ��MCS
			//����ֻ�ϱ�MPC��MPC2����֪ͨ����Ϊ���岻���ϱ���������ͨ��
			if ( g_cMSSsnApp.JudgeSndMsgPass() )
			{
				//����				
				LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[NotifyMcsBoardStatus]receive AGT_SVC_UPDATEBRDVERSION_NOTIF and begin to send to mcs MCU_MCS_UPDATEBRDVERSION_NOTIF!\n");
				SendMsg2Mcsssn(cServMsg, MCU_MCS_UPDATEBRDVERSION_NOTIF);
			}
			else if (tBrdInfo.GetType() == BRD_TYPE_MPC || tBrdInfo.GetType() == BRD_TYPE_MPC2)
			{
				//����ֻ�ϱ�MPC/MPC2����ͨ��				
				LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[NotifyMcsBoardStatus]receive AGT_SVC_UPDATEBRDVERSION_NOTIF and standby begin to send to mcs MCU_MCS_UPDATEBRDVERSION_NOTIF for mpc!\n");
				SendMsg2Mcsssn(cServMsg, MCU_MCS_UPDATEBRDVERSION_NOTIF);
			}
			else
			{
				//���岻�ϱ������忨����ͨ��
				LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[NotifyMcsBoardStatus]receive AGT_SVC_UPDATEBRDVERSION_NOTIF and standby not send to mcs MCU_MCS_UPDATEBRDVERSION_NOTIF!,Brd Type is %d\n", tBrdInfo.GetType());        
			}
		}

//      cServMsg.SetServMsg(pcMsg->content, pcMsg->length);
// 		LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "receive AGT_SVC_UPDATEBRDVERSION_NOTIF and begin to send to mcs MCU_MCS_UPDATEBRDVERSION_NOTIF!\n");
//      SendMsg2Mcsssn(cServMsg, MCU_MCS_UPDATEBRDVERSION_NOTIF);
    }
	// ���µ�DSCע��, zgc, 2007-03-05
//	else if (AGT_SVC_NEWDSCREGREQ_NOTIFY == pcMsg->event)
//	{
//		cServMsg.SetServMsg(pcMsg->content, pcMsg->length);
//		SendMsg2Mcsssn(cServMsg, MCU_MCS_NEWDSCREGREQ_NOTIFY, TRUE);
//	}

    return;
}

/*=============================================================================
  �� �� ���� SendMsg2Mcsssn
  ��    �ܣ� ������Ϣ��mcs�Ự
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CServMsg &cServMsg
  �� �� ֵ�� void  
=============================================================================*/
void CMcuCfgInst::SendMsg2Mcsssn(CServMsg &cServMsg, u16 wEvent, BOOL32 bAllMcsSsn)
{
	cServMsg.SetEventId(wEvent);

	if (bAllMcsSsn)
	{
		CMcsSsn::BroadcastToAllMcsSsn(wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
	}
	else
	{
		CMcsSsn::SendMsgToMcsSsn(cServMsg.GetSrcSsnId(), wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
	}

	LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "send msg: %d<%s> to mcsssn, bAllMcs.%d \n", wEvent, OspEventDesc(wEvent), bAllMcsSsn);

	return;
}

/*=============================================================================
�� �� ���� IsIpMaskValid
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u32 dwIpMask(������)
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/8/24   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CMcuCfgInst::IsIpMaskValid(u32 dwIpMask)
{
	if ( dwIpMask == 0 )
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG,  "[IsIpMaskValid] IpMask = 0\n" );
		return FALSE;
	}
	s32 nLop1 = 0, nLop2 = 0;
	u8 byMaskBitNum = 0;
	for ( nLop1 = 31; nLop1 >= 0; nLop1--)
	{
		if ( ((dwIpMask>>nLop1)&0x00000001) == 1 )
		{
			byMaskBitNum++;
			continue;
		}
		else
		{
			if ( nLop1 == 31 ) //��λ��Ϊ0
			{
				return FALSE;
			}
			else if( nLop1 == 0 ) //��һ��0���ֵ�λ�������һλ
			{
				return TRUE;
			}
			else
			{
				for ( nLop2 = nLop1-1; nLop2 >= 0; nLop2-- )
				{
					if ( ((dwIpMask>>nLop2)&0x00000001) == 1 )
					{
						return FALSE;
					}
				}
			}
			break;
		}
	}
	if ( byMaskBitNum > 30 )
	{
		return FALSE;
	}
	return TRUE;
}

/*=============================================================================
�� �� ���� BrdCfgProcIntegration
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CServMsg &cServMsg
�� �� ֵ�� BOOL32  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/9/7   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32  CMcuCfgInst::BrdCfgProcIntegration(CServMsg &cServMsg)
{	
	u8 *pbyMsg = cServMsg.GetMsgBody();
	u16 wMsgLen = cServMsg.GetMsgBodyLen();
	u16 wRet = 0;

	u16 wBrdLen =   0;
	u16 wMixLen =   0;
	u16 wRecLen =   0;
	u16 wBasLen =   0;
    u16 wBasHDLen = 0;
	u16 wTWLen =    0;
	u16 wVmpLen =   0;
	u16 wPrsLen =   0;
	u16 wMtwLen =   0;
    //4.6���¼�����������Ϣ jlb
    u16 wHdulen =   0;
	u16 wSvmplen =  0;
	u16 wMpuBaslen = 0;
	u16 wEbaplen =  0;
	u16 wEvpulen =  0;
	u16 wVrsRecLen = 0;
	u16 wTotalLen = 0;   //�ѽ����İ��ĳ���

	u8 byBrdNum =   0;
	u8 byMixNum =   0;
	u8 byRecNum =   0;
	u8 byBasNum =   0;
    u8 byBasHDNum = 0;
	u8 byTWNum =    0;
	u8 byVmpNum =   0;
	u8 byPrsNum =   0;
	u8 byMtwNum =   0;
    //4.6���¼�����������Ϣ jlb
    u8 byHduNum =   0;
	u8 bySvmpNum =  0;
	u8 byMpuBasNum = 0;
	u8 byEbapNum = 0;
	u8 byEvpuNum = 0;
	u8 byDvmpNum = 0;
	u8 byVrsRecNum = 0;

#ifdef _MINIMCU_
	BOOL32 bIsExistDsc = FALSE;
#endif

	//u32 dwExpDscIp = 0; // ��¼DSC���ⲿIP, zgc, 2007-07-04

	u32 nIndex = 0;
	u32 dwBrdLoop = 0;

	TBrdCfgInfo *ptBrdCfg =        NULL;
	TEqpMixerCfgInfo *ptMixerCfg = NULL;
	TEqpRecCfgInfo *ptRecCfg =     NULL;
	TEqpTvWallCfgInfo *ptTWCfg =   NULL;
	TEqpBasCfgInfo *ptBasCfg =     NULL;
    TEqpBasHDCfgInfo *ptBasHDCfg = NULL;
	TEqpVmpCfgInfo *ptVmpCfg =    NULL;
	TPrsCfgInfo *ptPrsCfg =        NULL;
	TEqpMTvwallCfgInfo *ptMtwCfg = NULL;
	//4.6���¼�����������Ϣ jlb
	TEqpHduCfgInfo     *ptHduCfg =    NULL;
	TEqpSvmpCfgInfo    *ptSvmpCfg =   NULL;
	TEqpDvmpCfgInfo    *ptDvmpCfg =   NULL;
	TEqpMpuBasCfgInfo  *ptMpuBasCfg = NULL;
    TEqpEbapCfgInfo    *ptEbapCfg =   NULL;
	TEqpEvpuCfgInfo    *ptEvpuCfg =   NULL;
	TEqpVrsRecCfgInfo  *ptVrsRecCfg = NULL;

    //mpc������
    u8 byMpcIndex = 0;

	//brd info
	byBrdNum = *(pbyMsg);
	wBrdLen = sizeof(u8) +byBrdNum*sizeof(TBrdCfgInfo);
	wTotalLen += wBrdLen;
 	
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	if ( wMsgLen < wTotalLen )
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_BRD)
			return FALSE;
	}
#else
	if (0 == byBrdNum || byBrdNum > MAX_BOARD_NUM || wMsgLen < wTotalLen)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_BRD)
			return FALSE;
	}
#endif

	else
	{
		ptBrdCfg = (TBrdCfgInfo *)(pbyMsg + (wTotalLen - wBrdLen) + sizeof(u8));
		for (nIndex = 0; nIndex < byBrdNum; nIndex++)            
		{
			if (g_bPrintCfgMsg)
			{
				ptBrdCfg[nIndex].Print();
			}  
            
			//  [1/21/2011 chendaiwei]֧��MPC2
            if(BRD_TYPE_MPC == ptBrdCfg[nIndex].GetType() || BRD_TYPE_MPC2 == ptBrdCfg[nIndex].GetType())
            {
                byMpcIndex = ptBrdCfg[nIndex].GetIndex();
            }
//4.6 �¼�  jlb 
			if (BRD_TYPE_HDU == ptBrdCfg[nIndex].GetType()
				|| BRD_TYPE_HDU_L == ptBrdCfg[nIndex].GetType()
				|| BRD_TYPE_HDU2 == ptBrdCfg[nIndex].GetType()
				|| BRD_TYPE_HDU2_L == ptBrdCfg[nIndex].GetType()
				|| BRD_TYPE_HDU2_S == ptBrdCfg[nIndex].GetType())
			{   
				
				u8 bySlot, bySlotType;
				ptBrdCfg[nIndex].GetSlot(bySlot, bySlotType);
				if (ptBrdCfg[nIndex].GetLayer() > MAXNUM_HDU_LAYER || 
					bySlot > MAXNUM_TVSSLOT || 
					0 == ptBrdCfg[nIndex].GetIpAddr())
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_BRD)
					LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] hdu brd info is invalid!\n");
					return FALSE;
				}
			}
			else
			{
				u8 bySlot, bySlotType;
				ptBrdCfg[nIndex].GetSlot(bySlot, bySlotType);
				if (ptBrdCfg[nIndex].GetLayer() > MAXNUM_LAYER || 
					bySlot > MAXNUM_SLOT || 
					0 == ptBrdCfg[nIndex].GetIpAddr())
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_BRD)
					LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] brd info is invalid!\n");
					return FALSE;
				}
			}
		
#ifdef _MINIMCU_
			u8 byBrdType = ptBrdCfg[nIndex].GetType();
			if( BRD_TYPE_DSC == byBrdType ||
				BRD_TYPE_MDSC == byBrdType ||
				BRD_TYPE_HDSC == byBrdType )
			{
				bIsExistDsc = TRUE;
				//dwExpDscIp = ptBrdCfg[nIndex].GetIpAddr();
			}
#endif
		} //for (nIndex = 0; nIndex < byBrdNum...
	} //if (0 == byBrdNum || byBrdNum > MAXNUM_BOARD...else...

	//mixer info
	byMixNum = *(pbyMsg + wTotalLen);
	wMixLen = sizeof(u8) + byMixNum*sizeof(TEqpMixerCfgInfo);
	wTotalLen += wMixLen;
	if (byMixNum > MAXNUM_PERIEQP || wMsgLen < wTotalLen)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_MIXER)
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] mixer body is invalid!\n");
		return FALSE;
	}
	else if (byMixNum > 0)
	{
		ptMixerCfg = (TEqpMixerCfgInfo *)(pbyMsg + (wTotalLen - wMixLen) + sizeof(u8));
		for (nIndex = 0; nIndex < byMixNum; nIndex++)
		{
			if (g_bPrintCfgMsg)
			{
				ptMixerCfg[nIndex].Print();
			}                

			if (ptMixerCfg[nIndex].GetEqpId() < MIXERID_MIN || 
				ptMixerCfg[nIndex].GetEqpId() > MIXERID_MAX || 
				!CheckBrdIndex(ptBrdCfg, byBrdNum, ptMixerCfg[nIndex].GetSwitchBrdIndex()) || 
				!CheckBrdIndex(ptBrdCfg, byBrdNum, ptMixerCfg[nIndex].GetRunningBrdIndex()))
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_MIXER)
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] mixer info is invalid!\n");
				return FALSE;
			}

            //8000B����8000H�Լ�8000I��8000G��mcu������mpc��������mixer
#if (!defined(_MINIMCU_) && !defined(_8KH_) && !defined(_8KI_) && !defined(_8KE_))
            if(ptMixerCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
            {
                MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_MIXER)
                LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] only 8000B mcu can run mixer on mpc, mpc Index :%d!\n", byMpcIndex);
                return FALSE;
            }
#endif

			//ֻ��8000B/C������MP���������MPC��Ϊת���壬zgc, 2007-09-06
#ifdef _MINIMCU_
			if ( !g_cMcuAgent.GetIsUseMpcTranData()
				&& ptMixerCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_MIXER)
                LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] mix%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptMixerCfg[nIndex].GetEqpId(), byMpcIndex);
                return FALSE;
			}
#endif
		} // for (nIndex = 0; nIndex < byMixNum ...
	} // if (byMixNum > MAXNUM_PERIEQP || wMsgLen-wB... else...

	//rec info
	byRecNum = *(pbyMsg + wTotalLen);
	wRecLen = sizeof(u8) + byRecNum*sizeof(TEqpRecCfgInfo);
	wTotalLen += wRecLen;
	if (byRecNum  > MAXNUM_PERIEQP || wMsgLen < wTotalLen)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_REC)
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] rec body is invalid!\n");
		return FALSE;
	}
	else if (byRecNum > 0)
	{
		ptRecCfg = (TEqpRecCfgInfo *)(pbyMsg + (wTotalLen - wRecLen) + sizeof(u8));
		for (nIndex = 0; nIndex < byRecNum; nIndex++)
		{
			if (g_bPrintCfgMsg)
			{
				ptRecCfg[nIndex].Print();
			}                

#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
			if (ptRecCfg[nIndex].GetEqpId() < RECORDERID_MIN || 
				ptRecCfg[nIndex].GetEqpId() > RECORDERID_MAX )
#else
			if (ptRecCfg[nIndex].GetEqpId() < RECORDERID_MIN || 
				ptRecCfg[nIndex].GetEqpId() > RECORDERID_MAX || 
				!CheckBrdIndex(ptBrdCfg, byBrdNum, ptRecCfg[nIndex].GetSwitchBrdIndex()))
#endif

			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_REC)
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] rec info is invalid!\n");
				return FALSE;
			}
			//ֻ��8000B/C������MP���������MPC��Ϊת���壬zgc, 2007-09-06
#ifdef _MINIMCU_
			if ( !g_cMcuAgent.GetIsUseMpcTranData() && ptRecCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_REC)
                LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] rec%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptRecCfg[nIndex].GetEqpId(), byMpcIndex);
                return FALSE;
			}
#endif
		} // for (nIndex = 0; nIndex < byRe ...
	} //if (byRecNum  > MAXNUM_PERIEQP || wMsgLen-wBrdLen-wMixLen ... else ...

	//tvwall info
	byTWNum = *(pbyMsg + wTotalLen);
	wTWLen = sizeof(u8) + byTWNum*sizeof(TEqpTvWallCfgInfo);
    wTotalLen += wTWLen;
	if (byTWNum  > MAXNUM_PERIEQP || wMsgLen < wTotalLen)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_TVWALL)
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] tvwall body is invalid!\n");
		return FALSE;
	}
	else if (byTWNum > 0)
	{
		ptTWCfg = (TEqpTvWallCfgInfo *)(pbyMsg + (wTotalLen - wTWLen) + sizeof(u8));
		for (nIndex = 0; nIndex < byTWNum; nIndex++)
		{
			if (g_bPrintCfgMsg)
			{
				ptTWCfg[nIndex].Print();
			}                

			if (ptTWCfg[nIndex].GetEqpId() < TVWALLID_MIN || 
				ptTWCfg[nIndex].GetEqpId() > TVWALLID_MAX || 
				!CheckBrdIndex(ptBrdCfg, byBrdNum, ptTWCfg[nIndex].GetRunningBrdIndex()))
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_TVWALL)
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] tvwall info is invalid!\n");
				return FALSE;
			}
			//ֻ��8000B/C������MP���������MPC��Ϊת���壬zgc, 2007-09-06
#ifdef _MINIMCU_
			if ( !g_cMcuAgent.GetIsUseMpcTranData()&& ptTWCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_TVWALL)
                LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] tvwall%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptTWCfg[nIndex].GetEqpId(), byMpcIndex);
                return FALSE;
			}
#endif
		} // for (nIndex = 0; nIndex < byTW ...
	} //if (byTWNum  > MAXNUM_PERIEQP || wMsgLen-wBrdLe ... else ...

	//bas info
	byBasNum = *(pbyMsg + wTotalLen);
	wBasLen = sizeof(u8) + byBasNum*sizeof(TEqpBasCfgInfo);
	wTotalLen += wBasLen;
	if (byBasNum  > MAXNUM_PERIEQP || wMsgLen < wTotalLen)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_BAS)
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] bas body is invalid!\n");
		return FALSE;
	}
	else if (byBasNum > 0)
	{
		ptBasCfg = (TEqpBasCfgInfo *)(pbyMsg + (wTotalLen - wBasLen) + sizeof(u8));
		for (nIndex = 0; nIndex < byBasNum; nIndex++)
		{
			if (g_bPrintCfgMsg)
			{
				ptBasCfg[nIndex].Print();
			}

			if (ptBasCfg[nIndex].GetEqpId() < BASID_MIN || 
				ptBasCfg[nIndex].GetEqpId() > BASID_MAX || 
				!CheckBrdIndex(ptBrdCfg, byBrdNum, ptBasCfg[nIndex].GetRunningBrdIndex()) || 
				!CheckBrdIndex(ptBrdCfg, byBrdNum, ptBasCfg[nIndex].GetSwitchBrdIndex()))
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_BAS)
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] bas info is invalid!\n");
				return FALSE;
			}

            //ֻ��8000B��mcu������mpc��������bas
#ifndef _MINIMCU_
            if(ptBasCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
            {
                MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_BAS)
                LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] only 8000B mcu can run bas on mpc, mpc Index :%d!\n", byMpcIndex);
                return FALSE;
            }
#endif
			//ֻ��8000B/C������MP���������MPC��Ϊת���壬zgc, 2007-09-06
#ifdef _MINIMCU_
			if ( !g_cMcuAgent.GetIsUseMpcTranData() && ptBasCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_BAS)
                LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] bas%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptBasCfg[nIndex].GetEqpId(), byMpcIndex);
                return FALSE;
			}
#endif
			
		} // for (nIndex = 0; nIndex < byBasN ...
	} // if (byBasNum  > MAXNUM_PERIEQP || wMsgLen-wBrdLe ... else ...

	//vmp info
	byVmpNum = *(pbyMsg + wTotalLen);
	wVmpLen = sizeof(u8) + byVmpNum*sizeof(TEqpVmpCfgInfo);
	wTotalLen += wVmpLen; 
	if (byVmpNum  > MAXNUM_PERIEQP || wMsgLen < wTotalLen)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_VMP)
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] vmp body is invalid!\n");
		return FALSE;
	}
	else if (byVmpNum > 0)
	{
		ptVmpCfg = (TEqpVmpCfgInfo *)(pbyMsg + (wTotalLen - wVmpLen) + sizeof(u8));
		for (nIndex = 0; nIndex < byVmpNum; nIndex++)
		{
			if (g_bPrintCfgMsg)
			{
				ptVmpCfg[nIndex].Print();
			}

			if (ptVmpCfg[nIndex].GetEqpId() < VMPID_MIN || 
				ptVmpCfg[nIndex].GetEqpId() > VMPID_MAX || 
				!CheckBrdIndex(ptBrdCfg, byBrdNum, ptVmpCfg[nIndex].GetRunningBrdIndex()) || 
				!CheckBrdIndex(ptBrdCfg, byBrdNum, ptVmpCfg[nIndex].GetSwitchBrdIndex()))
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_VMP)
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] vmp info is invalid!\n");
				return FALSE;
			}

            //ֻ��8000B��mcu������mpc��������vmp
#ifndef _MINIMCU_
            if(ptVmpCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
            {
                MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_VMP)
                LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] only 8000B mcu can run vmp on mpc, mpc Index :%d!\n", byMpcIndex);
                return FALSE;
            }
#endif
			//ֻ��8000B/C������MP���������MPC��Ϊת���壬zgc, 2007-09-06
#ifdef _MINIMCU_
			if ( !g_cMcuAgent.GetIsUseMpcTranData() && ptVmpCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_VMP)
                LogPrint(LOG_LVL_WARNING, MID_MCU_CFG,  "[ProcBrdCfg] vmp%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", 
				        ptVmpCfg[nIndex].GetEqpId(), byMpcIndex );
                return FALSE;
			}
#endif
		} // for (nIndex = 0; nIndex < byVmpNum; nInde ...
	} // if (byVmpNum  > MAXNUM_PERIEQP || wMsgLen-wBr ... else ...

	//prs info
	byPrsNum = *(pbyMsg + wTotalLen);
	wPrsLen = sizeof(u8) + byPrsNum*sizeof(TPrsCfgInfo);
	wTotalLen += wPrsLen;
	if (byPrsNum  > MAXNUM_PERIEQP || wMsgLen < wTotalLen)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_PRS)
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] prs body is invalid!\n");
		return FALSE;
	}
	else if (byPrsNum > 0)
	{
		ptPrsCfg = (TPrsCfgInfo *)(pbyMsg + (wTotalLen - wPrsLen) + sizeof(u8));
		for (nIndex = 0; nIndex < byPrsNum; nIndex++)
		{
			if (g_bPrintCfgMsg)
			{
				ptPrsCfg[nIndex].Print();
			}

			if (ptPrsCfg[nIndex].GetEqpId() < PRSID_MIN || 
				ptPrsCfg[nIndex].GetEqpId() > PRSID_MAX || 
				!CheckBrdIndex(ptBrdCfg, byBrdNum, ptPrsCfg[nIndex].GetRunningBrdIndex()) || 
				!CheckBrdIndex(ptBrdCfg, byBrdNum, ptPrsCfg[nIndex].GetSwitchBrdIndex()))
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_PRS)
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] prs info is invalid!\n");
				return FALSE;
			}

            //ֻ��8000B 8000E��mcu������mpc��������prs
#if !defined(_MINIMCU_) && !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
            if(ptPrsCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
            {
                MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_PRS)
                LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] only 8000B mcu can run prs on mpc, mpc Index :%d!\n", byMpcIndex);
                return FALSE;
            }
#endif
			//ֻ��8000B/C������MP���������MPC��Ϊת���壬zgc, 2007-09-06
#ifdef _MINIMCU_
			if ( !g_cMcuAgent.GetIsUseMpcTranData() && ptPrsCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_PRS)
                LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] prs%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptPrsCfg[nIndex].GetEqpId(), byMpcIndex);
                return FALSE;
			}
#endif
		} // for (nIndex = 0; nIndex < b ...
	} // if (byPrsNum  > MAXNUM_PERIEQP || wMsgL ... else ...

	//mtw��Ϣ
	byMtwNum = *(pbyMsg + wTotalLen);
	wMtwLen = sizeof(u8) + byMtwNum*sizeof(TEqpMTvwallCfgInfo);
	wTotalLen += wMtwLen;
	if (byMtwNum > MAXNUM_PERIEQP || wMsgLen < wTotalLen)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_MTVWALL)
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] mtw body is invalid!\n");
		return FALSE;
	}   
	else if (byMtwNum > 0)
	{
		ptMtwCfg = (TEqpMTvwallCfgInfo *)(pbyMsg + (wTotalLen - wMtwLen) + sizeof(u8));
		for (nIndex = 0; nIndex < byMtwNum; nIndex++)
		{
			if (g_bPrintCfgMsg)
			{
				ptMtwCfg[nIndex].Print();
			}

			if (ptMtwCfg[nIndex].GetEqpId() < VMPTWID_MIN || 
				ptMtwCfg[nIndex].GetEqpId() > VMPTWID_MAX || 
				!CheckBrdIndex(ptBrdCfg, byBrdNum, ptMtwCfg[nIndex].GetRunningBrdIndex()))
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_MTVWALL)
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] mtw info is invalid!\n");
				return FALSE;
			}
			//ֻ��8000B/C������MP���������MPC��Ϊת���壬zgc, 2007-09-06
#ifdef _MINIMCU_
			if (!g_cMcuAgent.GetIsUseMpcTranData() && ptMtwCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_MTVWALL)
                LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] mtw%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", 
				        ptMtwCfg[nIndex].GetEqpId(), byMpcIndex);
                return FALSE;
			}
#endif
		} // for (nIndex = 0; nIndex < by ...
	} // if (byMtwNum > MAXNUM_PERIEQP || wMsgLen-wBrdLen-w ... else ...



//zw[08/02/2008]
    //basHD info
    if (wMsgLen > wTotalLen)
    {
        byBasHDNum = *(pbyMsg + wTotalLen);
        wBasHDLen = sizeof(u8) + byBasHDNum * sizeof(TEqpBasHDCfgInfo);
        wTotalLen += wBasHDLen;
		if (byBasHDNum  > MAXNUM_PERIEQP || wMsgLen  < wTotalLen)
	    {
		    MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_BASHD)
		    LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] basHD body is invalid!\n");
		    return FALSE;
	    }
        else if (byBasHDNum > 0)
	    {
		    ptBasHDCfg = (TEqpBasHDCfgInfo*)(pbyMsg + (wTotalLen - wBasHDLen) + sizeof(u8));
		    for (nIndex = 0; nIndex < byBasHDNum; nIndex++)
		    {
			    if (g_bPrintCfgMsg)
			    {
				    ptBasHDCfg[nIndex].Print();
			    }

			    if (ptBasHDCfg[nIndex].GetEqpId() < BASID_MIN || 
				    ptBasHDCfg[nIndex].GetEqpId() > BASID_MAX || 
				    !CheckBrdIndex(ptBrdCfg, byBrdNum, ptBasHDCfg[nIndex].GetSwitchBrdIndex()))
			    {
				    MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_BASHD)
				    LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] basHD info is invalid!\n");
				    return FALSE;
			    }			    
		    } // for (nIndex = 0; nIndex < byBasHDN ...
	    } // if (byBasHDNum  > MAXNUM_PERIEQP || wMsgLen-wBrdLe ... else ...
    }
    else
    {
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[ProcBrdCfg] have no BasHD info!\n");
    }

	//4.6�汾 �¼����� jlb

	//SVMP info jlb
	if (wMsgLen > wTotalLen)
	{
		bySvmpNum = *(pbyMsg + wTotalLen);
		wSvmplen = sizeof(u8) + bySvmpNum * sizeof(TEqpSvmpCfgInfo);
		wTotalLen += wSvmplen;
		if (bySvmpNum  > MAXNUM_PERIEQP || (wMsgLen  < wTotalLen))
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_SVMP)
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] SVMP body is invalid!\n");
			return FALSE;
		}
		else if (bySvmpNum > 0)
		{
			ptSvmpCfg = (TEqpSvmpCfgInfo*)(pbyMsg + (wTotalLen - wSvmplen) + sizeof(u8));
			for (nIndex = 0; nIndex < bySvmpNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptSvmpCfg[nIndex].Print();
				}				 
				if (ptSvmpCfg[nIndex].GetEqpId() < VMPID_MIN || 
					ptSvmpCfg[nIndex].GetEqpId() > VMPID_MAX ||
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptSvmpCfg[nIndex].GetRunningBrdIndex()) ||
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptSvmpCfg[nIndex].GetSwitchBrdIndex()))
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_SVMP)
						LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] SVMP info is invalid!\n");
					return FALSE;
				}
				
#ifndef _MINIMCU_
				if(ptSvmpCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_SVMP)
						LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] only 8000B mcu can run Svmp on mpc, mpc Index :%d!\n", byMpcIndex);
					return FALSE;
				}
#endif

			} 
		} 
		else
		{

		}
    }
	else
    {
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[ProcBrdCfg] have no SVMP info!\n");
    }

	//MpuBas info  jlb


	if (wMsgLen > wTotalLen)
	{
		byMpuBasNum = *(pbyMsg + wTotalLen);
		wMpuBaslen = sizeof(u8) + byMpuBasNum * sizeof(TEqpMpuBasCfgInfo);
		wTotalLen += wMpuBaslen;
		if (byMpuBasNum  > MAXNUM_PERIEQP || (wMsgLen  < wTotalLen))
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[ProcBrdCfg]MpuBas  1\n");
			MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_MPUBAS)
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] MpuBas body is invalid!\n");
			return FALSE;
		}
		else if (byMpuBasNum > 0)
		{
			ptMpuBasCfg = (TEqpMpuBasCfgInfo*)(pbyMsg + (wTotalLen- wMpuBaslen) + sizeof(u8));
			for (nIndex = 0; nIndex < byMpuBasNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptMpuBasCfg[nIndex].Print();
				}				 
				if (ptMpuBasCfg[nIndex].GetEqpId() < BASID_MIN ||
					ptMpuBasCfg[nIndex].GetEqpId() > BASID_MAX ||
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptMpuBasCfg[nIndex].GetRunningBrdIndex()) ||
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptMpuBasCfg[nIndex].GetSwitchBrdIndex()) )
				{
					LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[ProcBrdCfg]MpuBas  2\n");
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_MPUBAS)
						LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] MpuBas info is invalid!\n");
					return FALSE;
				}	
				
#ifndef _MINIMCU_
				if(ptMpuBasCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
				{
					LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[ProcBrdCfg]MpuBas  3\n");
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_MPUBAS)
						LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] only 8000B mcu can run MpuBas on mpc, mpc Index :%d!\n", byMpcIndex);
					return FALSE;
				}
#endif

			} 
		} 
		else
		{
			
		}
    }
	else
    {
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[ProcBrdCfg] have no MpuBas info!\n");
    }

	//Ebap info  jlb
	if (wMsgLen > wTotalLen)
	{
		byEbapNum = *(pbyMsg + wTotalLen);
		wEbaplen = sizeof(u8) + byEbapNum * sizeof(TEqpEbapCfgInfo);
		wTotalLen += wEbaplen;
		if ( byEbapNum  > MAXNUM_PERIEQP || 
			(wMsgLen < wTotalLen) )
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_EBAP)
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] Ebap body is invalid!\n");
			return FALSE;
		}
		else if (byEbapNum > 0)
		{
			ptEbapCfg = (TEqpEbapCfgInfo*)(pbyMsg + (wTotalLen - wEbaplen) + sizeof(u8));
			for (nIndex = 0; nIndex < byEbapNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptEbapCfg[nIndex].Print();
				}				 
				if (ptEbapCfg[nIndex].GetEqpId() < BASID_MIN ||
					ptEbapCfg[nIndex].GetEqpId() > BASID_MAX ||
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptEbapCfg[nIndex].GetRunningBrdIndex()) ||
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptEbapCfg[nIndex].GetSwitchBrdIndex()) )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_EBAP)
						LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] Ebap info is invalid!\n");
					return FALSE;
				}
				
#ifndef _MINIMCU_
				if(ptEbapCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_EBAP)
						LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] only 8000B mcu can run ebap on mpc, mpc Index :%d!\n", byMpcIndex);
					return FALSE;
				}
#endif

			} 
		} 
		else
		{
			
		}
    }
	else
    {
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[ProcBrdCfg] have no Ebap info!\n");
    }

	//Evpu info  jlb
	if (wMsgLen > wTotalLen)
	{
		byEvpuNum = *(pbyMsg + wTotalLen);
		wEvpulen = sizeof(u8) + byEvpuNum * sizeof(TEqpEvpuCfgInfo);
		wTotalLen += wEvpulen;
		if ( byEvpuNum  > MAXNUM_PERIEQP || 
			(wMsgLen  < wTotalLen) )
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_EVPU)
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] Evpu body is invalid!\n");
			return FALSE;
		}
		else if (byEvpuNum > 0)
		{
			ptEvpuCfg = (TEqpEvpuCfgInfo*)(pbyMsg + (wTotalLen - wEvpulen) + sizeof(u8));
			for (nIndex = 0; nIndex < byEvpuNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptEvpuCfg[nIndex].Print();
				}				 
				if (ptEvpuCfg[nIndex].GetEqpId() < VMPID_MIN ||
					ptEvpuCfg[nIndex].GetEqpId() > VMPID_MAX ||
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptEvpuCfg[nIndex].GetRunningBrdIndex()) ||
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptEvpuCfg[nIndex].GetSwitchBrdIndex()) )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_EVPU)
						LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] Evpu info is invalid!\n");
					return FALSE;
				}	
#ifndef _MINIMCU_
				if(ptEvpuCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_EVPU)
						LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] only 8000B mcu can run evpu on mpc, mpc Index :%d!\n", byMpcIndex);
					return FALSE;
				}
#endif
			} 
		} 
		else
		{
			
		}
    }
	else
    {
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[ProcBrdCfg] have no Evpu info!\n");
    }

	//HDU info jlb
	if (wMsgLen > wTotalLen)
	{
		byHduNum = *(pbyMsg + wTotalLen);
		wHdulen = sizeof(u8) + byHduNum * sizeof(TEqpHduCfgInfo);
        wTotalLen += wHdulen;
		if ( byHduNum  > MAXNUM_PERIHDU || (wMsgLen  < wTotalLen) )
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_HDU)
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] Hdu body is invalid!\n");
			return FALSE;
		}
		else if (byHduNum > 0)
		{
			ptHduCfg = (TEqpHduCfgInfo*)(pbyMsg + (wTotalLen - wHdulen) + sizeof(u8));
			for (nIndex = 0; nIndex < byHduNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptHduCfg[nIndex].Print();
				}				 
				if ( ptHduCfg[nIndex].GetEqpId() < HDUID_MIN || 
					ptHduCfg[nIndex].GetEqpId() > HDUID_MAX || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptHduCfg[nIndex].GetRunningBrdIndex()) )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_HDU)
						LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] HDU info is invalid!\n");
					return FALSE;
				}
        #ifdef _LINUX_                
            #ifndef _MINIMCU_
				if(ptHduCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_HDU)
						LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] only 8000B mcu can run Hdu on mpc, mpc Index :%d!\n", byMpcIndex);
					return FALSE;
				}
            #endif
        #endif
				//ֻ��8000B/C������MP���������MPC��Ϊת���壬zgc, 2007-09-06
#ifdef _MINIMCU_
				if ( !g_cMcuAgent.GetIsUseMpcTranData() && ptHduCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_TVWALL)
						LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] tvwall%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptTWCfg[nIndex].GetEqpId(), byMpcIndex);
					return FALSE;
				}
#endif				
			} 
		} 
    }
	else
    {
		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[ProcBrdCfg] have no HDU info!\n");
    }

// 		//DVMP info  jlb
//  	if (wMsgLen > wTotalLen)
//  	{
//  		byDvmpNum = *(pbyMsg + wTotalLen);
//  		wDvmplen = sizeof(u8) + byDvmpNum * sizeof(TEqpDvmpCfgInfo);
//  		wTotalLen += wDvmplen;
//  		if (byDvmpNum  > MAXNUM_PERIDVMP || (wMsgLen < wTotalLen))
//  		{
//  			MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_DVMP)
//  				LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] DVMP body is invalid!\n");
//  			return FALSE;
//  		}
//  		else if (byDvmpNum > 0)
//  		{
//  			ptDvmpCfg = (TEqpDvmpCfgInfo*)(pbyMsg + (wTotalLen - wDvmplen) + sizeof(u8));
//  			for (nIndex = 0; nIndex < byDvmpNum; nIndex++)
//  			{
//  				if (g_bPrintCfgMsg)
//  				{
//  					ptDvmpCfg[nIndex].tEqpDvmpCfgBasicInfo[0].Print();
//  					ptDvmpCfg[nIndex].tEqpDvmpCfgBasicInfo[1].Print();
//  				}				 
//  				if ( ptDvmpCfg[nIndex].tEqpDvmpCfgBasicInfo[0].GetEqpId() < VMPID_MIN ||
//  					 ptDvmpCfg[nIndex].tEqpDvmpCfgBasicInfo[0].GetEqpId() > VMPID_MAX ||
//  					 !CheckBrdIndex(ptBrdCfg, byBrdNum, ptDvmpCfg[nIndex].tEqpDvmpCfgBasicInfo[0].GetRunningBrdIndex()) ||
// 					 !CheckBrdIndex(ptBrdCfg, byBrdNum, ptDvmpCfg[nIndex].tEqpDvmpCfgBasicInfo[0].GetSwitchBrdIndex()) )
//  				{
//  					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_DVMP)
//  						LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] DVMP[0] info is invalid!\n");
//  					return FALSE;
//  				}
//  				if ( ptDvmpCfg[nIndex].tEqpDvmpCfgBasicInfo[1].GetEqpId() < VMPID_MIN ||
//  					 ptDvmpCfg[nIndex].tEqpDvmpCfgBasicInfo[1].GetEqpId() > VMPID_MAX ||
//  					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptDvmpCfg[nIndex].tEqpDvmpCfgBasicInfo[1].GetRunningBrdIndex()) ||
//  					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptDvmpCfg[nIndex].tEqpDvmpCfgBasicInfo[1].GetSwitchBrdIndex()) )
//  				{
//  					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_DVMP)
//  						LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] DVMP[1] info is invalid!\n");
//  					return FALSE;
//  				}
//  /*
// // #ifndef _MINIMCU_
// // 				if(ptPrsCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
// // 				{
// // 					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_PRS)
// // 						LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] only 8000B mcu can run Dvmp on mpc, mpc Index :%d!\n", byMpcIndex);
// // 					return FALSE;
// // 				}
// // #endif
// // */
//  			} 
//  		} 
//  		else
//  		{
//  			
//  		}
//      }
//  	else
//      {
//  		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[ProcBrdCfg] have no DVMP info!\n");
//      }


	// �����Ϣ�壬TDSCModuleInfo�ṹ��MCS_MCU_GETMCUGENERALCFG_REQ��Ϣ��ת�Ƶ�MCS_MCU_GETBRDCFG_REQ, zgc, 2007-03-15
 #ifdef _MINIMCU_	
	T8000BDscMod tDscInfo = *(T8000BDscMod*)( pbyMsg + wTotalLen);	
 	TDSCModuleInfo tAgentDscInfo;
 	DscInfoIn2Out( tDscInfo, tAgentDscInfo );
 	if( g_bPrintCfgMsg )
 	{
 		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "T8000BDscMod:\n");
 		tDscInfo.Print();
 		LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "TDSCModuleInfo:\n");
 		tAgentDscInfo.Print();
 	}
 	//	BOOL32 bIsRegDsc = FALSE;
 	//	if ( g_cMcuAgent.IsRegedBoard( g_cMcuAgent.GetConfigedDscType(), 0, 0 ) )
 	//	{
 	//		bIsRegDsc = TRUE;
 	//	}
#endif
	
	//vrsrec info ��¼��׷��
	byVrsRecNum = *(pbyMsg + wTotalLen);
	wVrsRecLen = sizeof(u8) + byVrsRecNum*sizeof(TEqpVrsRecCfgInfo);
	wTotalLen += wVrsRecLen;
	if (byVrsRecNum  > MAXNUM_PERIEQP || wMsgLen < wTotalLen)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_REC)
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] vrsrec body is invalid!\n");
		return FALSE;
	}
	else if (byVrsRecNum > 0)
	{
		ptVrsRecCfg = (TEqpVrsRecCfgInfo *)(pbyMsg + (wTotalLen - wVrsRecLen) + sizeof(u8));
		for (nIndex = 0; nIndex < byVrsRecNum; nIndex++)
		{
			if (g_bPrintCfgMsg)
			{
				ptVrsRecCfg[nIndex].Print();
			}
		}
	}

	//��ʼ������MCU����ʼ�˿�,Ϊ��һ��writeMixerTable�Ⱥ�������˿�����׼�� [12/13/2011 chendaiwei]
	g_cMcuAgent.ClearAllPeriEqpMcuRecvPort();

	//change brd info        
	TBoardInfo atBrdInfo[MAX_BOARD_NUM]; 
	u8 byBrdRealNum = 0;
	BrdCfgIn2Out( ptBrdCfg, atBrdInfo, byBrdNum,&byBrdRealNum );
	

	//change mixer info   
	TEqpMixerInfo atMixerInfo[MAXNUM_PERIEQP];
	if (NULL != ptMixerCfg)
	{
		MixerCfgIn2Out(ptMixerCfg, atMixerInfo, byMixNum);
		for ( nIndex = 0; nIndex < byMixNum; nIndex++ )
		{
			// ��Ϊ�Ϸ�EQP, zgc, 2007-10-29
			g_cMcuVcApp.SetPeriEqpIsValid( atMixerInfo[nIndex].GetEqpId() );
			for( dwBrdLoop = 0; dwBrdLoop < byBrdNum; dwBrdLoop++)
			{
				if( atBrdInfo[dwBrdLoop].GetBrdId() == atMixerInfo[nIndex].GetRunBrdId() )
				{
					atMixerInfo[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
					atBrdInfo[dwBrdLoop].SetPeriId( atMixerInfo[nIndex].GetEqpId() );
					break;
				}
			}
			if( dwBrdLoop >= byBrdNum )
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[BrdCfgProcIntegration] Mixer%d running brd(Id.%d) isn't exist!\n", 
						 atMixerInfo[nIndex].GetEqpId(), atMixerInfo[nIndex].GetRunBrdId());
			}
		}
	}
	else
	{
		byMixNum = 0;
	}		

	//change rec info
	TEqpRecInfo atRecInfo[MAXNUM_PERIEQP];
	if (NULL != ptRecCfg)
	{
		RecCfgIn2Out(ptRecCfg, atRecInfo, byRecNum);
		for ( nIndex = 0; nIndex < byRecNum; nIndex++ )
		{
			// ��Ϊ�Ϸ�EQP, zgc, 2007-10-29
			g_cMcuVcApp.SetPeriEqpIsValid( atRecInfo[nIndex].GetEqpId() );
		}
	}     
	else
	{
		byRecNum = 0;
	}

	//change tvwall info
	TEqpTVWallInfo atTWInfo[MAXNUM_PERIEQP];
	if (NULL != ptTWCfg)
	{
		TWCfgIn2Out(ptTWCfg, atTWInfo, byTWNum);
		for ( nIndex = 0; nIndex < byTWNum; nIndex++ )
		{
			// ��Ϊ�Ϸ�EQP, zgc, 2007-10-29
			g_cMcuVcApp.SetPeriEqpIsValid( atTWInfo[nIndex].GetEqpId() );
			for( dwBrdLoop = 0; dwBrdLoop < byBrdNum; dwBrdLoop++)
			{
				if( atBrdInfo[dwBrdLoop].GetBrdId() == atTWInfo[nIndex].GetRunBrdId() )
				{
					atTWInfo[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
					atBrdInfo[dwBrdLoop].SetPeriId( atTWInfo[nIndex].GetEqpId() );
					break;
				}
			}
			if( dwBrdLoop >= byBrdNum )
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[BrdCfgProcIntegration] TVWall%d running brd(Id.%d) isn't exist!\n", 
						 atTWInfo[nIndex].GetEqpId(), atTWInfo[nIndex].GetRunBrdId());
			}
		}
	}
	else
	{
		byTWNum = 0;
	}
	
	//change bas info
	TEqpBasInfo atBasInfo[MAXNUM_PERIEQP];
	if (NULL != ptBasCfg)
	{
#if defined(_MINIMCU_)
		ptBasCfg->SetSwitchBrdIndex(1);
#endif
		BasCfgIn2Out(ptBasCfg, atBasInfo, byBasNum);
		for ( nIndex = 0; nIndex < byBasNum; nIndex++ )
		{
			// ��Ϊ�Ϸ�EQP, zgc, 2007-10-29
			g_cMcuVcApp.SetPeriEqpIsValid( atBasInfo[nIndex].GetEqpId() );
			for( dwBrdLoop = 0; dwBrdLoop < byBrdNum; dwBrdLoop++)
			{
				if( atBrdInfo[dwBrdLoop].GetBrdId() == atBasInfo[nIndex].GetRunBrdId() )
				{
					atBasInfo[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
					atBrdInfo[dwBrdLoop].SetPeriId( atBasInfo[nIndex].GetEqpId() );
					break;
				}
			}
			if( dwBrdLoop >= byBrdNum )
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[BrdCfgProcIntegration] Bas%d running brd(Id.%d) isn't exist!\n", 
						 atBasInfo[nIndex].GetEqpId(), atBasInfo[nIndex].GetRunBrdId());
			}
		}
	}
	else
	{
		byBasNum = 0;
	}

    //zw[08/05/2008]
	//change basHD info
	TEqpBasHDInfo atBasHDInfo[MAXNUM_PERIEQP];
	if (NULL != ptBasHDCfg)
	{
		BasHDCfgIn2Out(ptBasHDCfg, atBasHDInfo, byBasHDNum);
		for ( nIndex = 0; nIndex < byBasHDNum; nIndex++ )
		{
			g_cMcuVcApp.SetPeriEqpIsValid( atBasHDInfo[nIndex].GetEqpId() );
		}
	}
	else
	{
		byBasHDNum = 0;
	}

	//change vmp info
	TEqpVMPInfo atVmpInfo[MAXNUM_PERIEQP];
	if (NULL != ptVmpCfg)
	{
#if defined(_MINIMCU_)
		ptVmpCfg->SetSwitchBrdIndex(1);
#endif
		VmpCfgIn2Out(ptVmpCfg, atVmpInfo, byVmpNum);
		for ( nIndex = 0; nIndex < byVmpNum; nIndex++ )
		{
			// ��Ϊ�Ϸ�EQP, zgc, 2007-10-29
			g_cMcuVcApp.SetPeriEqpIsValid( atVmpInfo[nIndex].GetEqpId() );
			for( dwBrdLoop = 0; dwBrdLoop < byBrdNum; dwBrdLoop++)
			{
				if( atBrdInfo[dwBrdLoop].GetBrdId() == atVmpInfo[nIndex].GetRunBrdId() )
				{
					atVmpInfo[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
					atBrdInfo[dwBrdLoop].SetPeriId( atVmpInfo[nIndex].GetEqpId() );
					break;
				}
			}
			if( dwBrdLoop >= byBrdNum )
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[BrdCfgProcIntegration] Vmp%d running brd(Id.%d) isn't exist!\n", 
						 atVmpInfo[nIndex].GetEqpId(), atVmpInfo[nIndex].GetRunBrdId());
			}
		}
	}
	else
	{
		byVmpNum = 0;
	}
	
	//change prs info
	TEqpPrsInfo atPrsInfo[MAXNUM_PERIEQP];
	if (NULL != ptPrsCfg)
	{
#if defined(_MINIMCU_)
		ptPrsCfg->SetSwitchBrdIndex(1);
#endif
		PrsCfgIn2Out(ptPrsCfg, atPrsInfo, byPrsNum); 
		for ( nIndex = 0; nIndex < byPrsNum; nIndex++ )
		{
			// ��Ϊ�Ϸ�EQP, zgc, 2007-10-29
			g_cMcuVcApp.SetPeriEqpIsValid( atPrsInfo[nIndex].GetEqpId() );
			for( dwBrdLoop = 0; dwBrdLoop < byBrdNum; dwBrdLoop++)
			{
				if( atBrdInfo[dwBrdLoop].GetBrdId() == atPrsInfo[nIndex].GetRunBrdId() )
				{
					atPrsInfo[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
					atBrdInfo[dwBrdLoop].SetPeriId( atPrsInfo[nIndex].GetEqpId() );
					break;
				}
			}
			if( dwBrdLoop >= byBrdNum )
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[BrdCfgProcIntegration] Prs%d running brd(Id.%d) isn't exist!\n", 
						 atPrsInfo[nIndex].GetEqpId(), atPrsInfo[nIndex].GetRunBrdId());
			}
		}
	}    
	else
	{
		byPrsNum = 0;
	}

	//change mtw info
	TEqpMpwInfo atMtwInfo[MAXNUM_PERIEQP];
	if (NULL != ptMtwCfg)
	{              
		MtwCfgIn2Out(ptMtwCfg, atMtwInfo, byMtwNum); 
		for ( nIndex = 0; nIndex < byMtwNum; nIndex++ )
		{
			// ��Ϊ�Ϸ�EQP, zgc, 2007-10-29
			g_cMcuVcApp.SetPeriEqpIsValid( atMtwInfo[nIndex].GetEqpId() );
			for( dwBrdLoop = 0; dwBrdLoop < byBrdNum; dwBrdLoop++)
			{
				if( atBrdInfo[dwBrdLoop].GetBrdId() == atMtwInfo[nIndex].GetRunBrdId() )
				{
					atMtwInfo[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
					atBrdInfo[dwBrdLoop].SetPeriId( atMtwInfo[nIndex].GetEqpId() );
					break;
				}
			}
			if( dwBrdLoop >= byBrdNum )
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[BrdCfgProcIntegration] Mtw%d running brd(Id.%d) isn't exist!\n", 
						 atMtwInfo[nIndex].GetEqpId(), atMtwInfo[nIndex].GetRunBrdId());
			}
		}
	}
	else
	{
		byMtwNum = 0;
	}

	//4.6�¼����� jlb
    //change hdu info  
    TEqpHduInfo atHduInfo[MAXNUM_PERIHDU];
	if (NULL != ptHduCfg)
	{
		HduCfgIn2Out(ptHduCfg, atHduInfo, byHduNum);
		for (nIndex=0; nIndex<byHduNum; nIndex++)
		{
			g_cMcuVcApp.SetPeriEqpIsValid( atHduInfo[nIndex].GetEqpId() );
			for (dwBrdLoop=0; dwBrdLoop<byBrdNum; dwBrdLoop++)
			{
				if ( atBrdInfo[dwBrdLoop].GetBrdId() == atHduInfo[nIndex].GetRunBrdId() )
				{
					atHduInfo[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
				    atBrdInfo[dwBrdLoop].SetPeriId( atHduInfo[nIndex].GetEqpId() );
					break;
				}
			}
			if ( dwBrdLoop >= byBrdNum )
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[BrdCfgProcIntegration] Hdu%d running brd(Id.%d) isn't exist!\n",
					    atHduInfo[nIndex].GetEqpId(), atHduInfo[nIndex].GetRunBrdId() );
			}
		}
	}
	else
	{
        byHduNum = 0;
	}

	//change svmp info  
    TEqpSvmpInfo atSvmpInfo[MAXNUM_PERIEQP];
	if (NULL != ptSvmpCfg)
	{
		SvmpCfgIn2Out(ptSvmpCfg, atSvmpInfo, bySvmpNum);
		for (nIndex=0; nIndex<bySvmpNum; nIndex++)
		{
			g_cMcuVcApp.SetPeriEqpIsValid( atSvmpInfo[nIndex].GetEqpId() );
			for (dwBrdLoop=0; dwBrdLoop<byBrdNum; dwBrdLoop++)
			{
				if ( atBrdInfo[dwBrdLoop].GetBrdId() == atSvmpInfo[nIndex].GetRunBrdId() )
				{
					atSvmpInfo[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
					atBrdInfo[dwBrdLoop].SetPeriId( atSvmpInfo[nIndex].GetEqpId() );
					break;
				}
			}
			if ( dwBrdLoop >= byBrdNum )
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[BrdCfgProcIntegration] Svmp%d running brd(Id.%d) isn't exist!\n",
					atSvmpInfo[nIndex].GetEqpId(), atSvmpInfo[nIndex].GetRunBrdId() );
			}
		}
	}
	else
	{
        bySvmpNum = 0;
	}

// 	//change dvmp info  
//     TEqpDvmpBasicInfo atDvmpInfo1[MAXNUM_PERIDVMP];
//     TEqpDvmpBasicInfo atDvmpInfo2[MAXNUM_PERIDVMP];
// 	if (NULL != ptDvmpCfg)
// 	{
// 		DvmpCfgIn2Out(&(ptDvmpCfg->tEqpDvmpCfgBasicInfo[0]), atDvmpInfo1, byDvmpNum);
// 		DvmpCfgIn2Out(&(ptDvmpCfg->tEqpDvmpCfgBasicInfo[1]), atDvmpInfo2, byHduNum);
// 		for (nIndex=0; nIndex<byDvmpNum; nIndex++)
// 		{
// 			g_cMcuVcApp.SetPeriEqpIsValid( atDvmpInfo1[nIndex].GetEqpId() );
// 			for (dwBrdLoop=0; dwBrdLoop<byBrdNum; dwBrdLoop++)
// 			{
// 				if ( atBrdInfo[dwBrdLoop].GetBrdId() == atDvmpInfo1[nIndex].GetRunBrdId() )
// 				{
// 					atDvmpInfo1[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
// 					atBrdInfo[dwBrdLoop].SetPeriId( atDvmpInfo1[nIndex].GetEqpId() );
// 					break;
// 				}
// 			}
// 			if ( dwBrdLoop >= byBrdNum )
// 			{
// 				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[BrdCfgProcIntegration] DvmpBasicInfo1%d running brd(Id.%d) isn't exist!\n",
// 					atDvmpInfo1[nIndex].GetEqpId(), atDvmpInfo1[nIndex].GetRunBrdId() );
// 			}
// 
// 			g_cMcuVcApp.SetPeriEqpIsValid( atDvmpInfo2[nIndex].GetEqpId() );
// 			for (dwBrdLoop=0; dwBrdLoop<byBrdNum; dwBrdLoop++)
// 			{
// 				if ( atBrdInfo[dwBrdLoop].GetBrdId() == atDvmpInfo2[nIndex].GetRunBrdId() )
// 				{
// 					atDvmpInfo2[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
// 					atBrdInfo[dwBrdLoop].SetPeriId( atDvmpInfo2[nIndex].GetEqpId() );
// 					break;
// 				}
// 			}
// 			if ( dwBrdLoop >= byBrdNum )
// 			{
// 				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[BrdCfgProcIntegration] DvmpBasicInfo2%d running brd(Id.%d) isn't exist!\n",
// 					atDvmpInfo2[nIndex].GetEqpId(), atDvmpInfo2[nIndex].GetRunBrdId() );
// 			}
// 		}
// 	}
// // 	else
// // 	{
// //         byDvmpNum = 0;
// // 	}

	//change MpuBas info  
    TEqpMpuBasInfo atMpuBasInfo[MAXNUM_PERIEQP];
	if (NULL != ptMpuBasCfg)
	{
		MpuBasCfgIn2Out(ptMpuBasCfg, atMpuBasInfo, byMpuBasNum);
		for (nIndex=0; nIndex<byMpuBasNum; nIndex++)
		{
			g_cMcuVcApp.SetPeriEqpIsValid( atMpuBasInfo[nIndex].GetEqpId() );
			for (dwBrdLoop=0; dwBrdLoop<byBrdNum; dwBrdLoop++)
			{
				if ( atBrdInfo[dwBrdLoop].GetBrdId() == atMpuBasInfo[nIndex].GetRunBrdId() )
				{
					atMpuBasInfo[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
					atBrdInfo[dwBrdLoop].SetPeriId( atMpuBasInfo[nIndex].GetEqpId() );
					break;
				}
			}
			if ( dwBrdLoop >= byBrdNum )
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[BrdCfgProcIntegration] MpuBas%d running brd(Id.%d) isn't exist!\n",
					atMpuBasInfo[nIndex].GetEqpId(), atMpuBasInfo[nIndex].GetRunBrdId() );
			}
		}
	}
	else
	{
        byMpuBasNum = 0;
	}

	//change Ebap info  
    TEqpEbapInfo atEbapInfo[MAXNUM_PERIEQP];
	if (NULL != ptEbapCfg)
	{
		EbapCfgIn2Out(ptEbapCfg, atEbapInfo, byEbapNum);
		for (nIndex=0; nIndex<byEbapNum; nIndex++)
		{
			g_cMcuVcApp.SetPeriEqpIsValid( atEbapInfo[nIndex].GetEqpId() );
			for (dwBrdLoop=0; dwBrdLoop<byBrdNum; dwBrdLoop++)
			{
				if ( atBrdInfo[dwBrdLoop].GetBrdId() == atEbapInfo[nIndex].GetRunBrdId() )
				{
					atEbapInfo[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
					atBrdInfo[dwBrdLoop].SetPeriId( atEbapInfo[nIndex].GetEqpId() );
					break;
				}
			}
			if ( dwBrdLoop >= byBrdNum )
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[BrdCfgProcIntegration] Ebap%d running brd(Id.%d) isn't exist!\n",
					atEbapInfo[nIndex].GetEqpId(), atEbapInfo[nIndex].GetRunBrdId() );
			}
		}
	}
// 	else
// 	{
//         byEbapNum = 0;
// 	}

	//change evpu info  
    TEqpEvpuInfo atEvpuInfo[MAXNUM_PERIEQP];
	if (NULL != ptEvpuCfg)
	{
		EvpuCfgIn2Out(ptEvpuCfg, atEvpuInfo, byEvpuNum);
		for (nIndex=0; nIndex<byEvpuNum; nIndex++)
		{
			g_cMcuVcApp.SetPeriEqpIsValid( atEvpuInfo[nIndex].GetEqpId() );
			for (dwBrdLoop=0; dwBrdLoop<byBrdNum; dwBrdLoop++)
			{
				if ( atBrdInfo[dwBrdLoop].GetBrdId() == atEvpuInfo[nIndex].GetRunBrdId() )
				{
					atEvpuInfo[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
					atBrdInfo[dwBrdLoop].SetPeriId( atEvpuInfo[nIndex].GetEqpId() );
					break;
				}
			}
			if ( dwBrdLoop >= byBrdNum )
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[BrdCfgProcIntegration] Evpu%d running brd(Id.%d) isn't exist!\n",
					atEvpuInfo[nIndex].GetEqpId(), atEvpuInfo[nIndex].GetRunBrdId() );
			}
		}
	}
// 	else
// 	{
//         byEvpuNum = 0;
// 	}

	// ��¼��ǰ�ĵ�������, zgc
	TBoardInfo atCurBrdInfo[MAX_BOARD_NUM];
	memset(atCurBrdInfo, 0, sizeof(atCurBrdInfo));
	u8 byCruBrdNum = 0;
	g_cMcuAgent.ReadBrdTable( &byCruBrdNum, atCurBrdInfo );

	//[2011/01/25 zhushz]IS2.x����֧��
	// �޸�������Ϣ
	//  [12/5/2010 chendaiwei]MCS���ô��������ݲ�����IS2.2�壬�����ֶ���IS2.2��������Ϣ׷���ڵ�������λ��
// 	for(u8 byIndex = 0; byIndex < byCruBrdNum; byIndex++ )
// 	{
// 		if(atCurBrdInfo[byIndex].GetType() == BRD_TYPE_IS22)
// 		{
// 			memcpy(&atBrdInfo[byBrdNum],&atCurBrdInfo[byIndex],sizeof(TBoardInfo));
// 			byBrdNum++;
// 
// 			break;
// 		}
// 	}

	wRet = g_cMcuAgent.WriteBrdTable( byBrdRealNum/*byBrdNum*/, atBrdInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg,cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_BRD)
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] write brd info failed, ret:%d!\n", wRet);
		return FALSE;
	}

	// ����Ҫ�����ڴ��еĶ˿���Ϣ[12/15/2011 chendaiwei]
	
	wRet = g_cMcuAgent.WriteMixerTable(byMixNum, atMixerInfo,FALSE);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_MIXER)
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] write mixer info failed, ret:%d!\n", wRet);
		return FALSE;
	}

	WriteMixerCfgToFile(byMixNum, atMixerInfo);

	// ����Ҫ�����ڴ��еĶ˿���Ϣ[12/15/2011 chendaiwei]
	wRet = g_cMcuAgent.WriteRecTable(byRecNum, atRecInfo,FALSE);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_REC)
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] write rec info failed, ret:%d!\n", wRet);
		return FALSE;
	}

	wRet = g_cMcuAgent.WriteTvTable(byTWNum, atTWInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_TVWALL)
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] write tvwall info failed, ret:%d!\n", wRet);
		return FALSE;
	}

	// ����Ҫ�����ڴ��еĶ˿���Ϣ[12/15/2011 chendaiwei]
	wRet = g_cMcuAgent.WriteBasTable(byBasNum, atBasInfo,FALSE);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_BAS)
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] write bas info failed, ret:%d!\n", wRet);
		return FALSE;
	}

    //zw[08/05/2008]
	// ����Ҫ�����ڴ��еĶ˿���Ϣ[12/15/2011 chendaiwei]
    wRet = g_cMcuAgent.WriteBasHDTable(byBasHDNum, atBasHDInfo,FALSE);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_BASHD)
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] write basHD info failed, ret:%d!\n", wRet);
		return FALSE;
	}

	// ����Ҫ�����ڴ��еĶ˿���Ϣ[12/15/2011 chendaiwei]
	wRet = g_cMcuAgent.WriteVmpTable(byVmpNum, atVmpInfo,FALSE);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_VMP)

		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] write vmp info failed, ret:%d!\n", wRet);
		return FALSE;
	}

	//  [12/5/2010 chendaiwei]MCS���ô��������ݲ�����IS2.2�壬�����ֶ���IS2.2��������Ϣ׷���ڵ�������λ��
	TEqpPrsInfo atCurPrsInfo[MAXNUM_PERIEQP];
	memset( atCurPrsInfo, 0, sizeof(atCurPrsInfo) );
	u8 byCurPrsNum = 0;
	wRet = g_cMcuAgent.ReadPrsTable(&byCurPrsNum, atCurPrsInfo);

	for(u8 byPrsIndex = 0; byPrsIndex<byCurPrsNum; byPrsIndex++)
	{
		//  [12/5/2010 chendaiwei]PRSID_MAX-1��IS2.2��
		if (atCurPrsInfo[byPrsIndex].GetEqpId() == PRSID_MAX-1)
		{
			memcpy(&atPrsInfo[byPrsNum],&atCurPrsInfo[byPrsIndex],sizeof(TEqpPrsInfo));
			byPrsNum ++;
			
			break;
		}
	}

	wRet = g_cMcuAgent.WritePrsTable(byPrsNum, atPrsInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_PRS)
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] write prs info failed, ret:%d!\n", wRet);
		return FALSE;
	}
	
	wRet = g_cMcuAgent.WriteMpwTable(byMtwNum, atMtwInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_MTVWALL)

		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] write mtw info failed, ret:%d!\n", wRet);
		return FALSE;
	}

	//4.6�汾 �¼����� jlb
    TEqpHduInfo  atOldEqpHduInfo[MAXNUM_PERIHDU]; 
	memset(atOldEqpHduInfo, 0x0, sizeof(atOldEqpHduInfo));
    u8 byOldHduNum = 0;
	wRet = g_cMcuAgent.ReadHduTable(&byOldHduNum, atOldEqpHduInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_READ_HDU)
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] write hdu info failed, ret:%d!\n", wRet);
		return FALSE;
	}
    
	// �޸����������ʽ
	u8 byHduId = 0;
	THduChnlModePort atChnModePort[MAXNUM_HDU_CHANNEL];
	memset(atChnModePort, 0x0, sizeof(atChnModePort));
	TPeriEqpStatus tHduStatus;
	for(u32 nLoop1=0; nLoop1<byHduNum; nLoop1++)
	{
		if (g_cMcuVcApp.GetPeriEqpStatus(atHduInfo[nLoop1].GetEqpId(), &tHduStatus) &&
			1 == tHduStatus.m_byOnline)
		{
			for(u32 nLoop2=0; nLoop2<byOldHduNum; nLoop2++)
			{
				byHduId = atHduInfo[nLoop1].GetEqpId();
				if ( ((byHduId>=HDUID_MIN)&&(byHduId<=HDUID_MAX))&& 
					(byHduId == atOldEqpHduInfo[nLoop2].GetEqpId()))
				{	
					THduChnlModePortAgt tHduModePortAgt1, tHduModePortAgt2;
					atHduInfo[nLoop1].GetHduChnlModePort(0, tHduModePortAgt1);
					atHduInfo[nLoop1].GetHduChnlModePort(1, tHduModePortAgt2);
					atChnModePort[0].SetZoomRate( tHduModePortAgt1.GetZoomRate() );
					atChnModePort[0].SetOutModeType(tHduModePortAgt1.GetOutModeType());
					atChnModePort[0].SetOutPortType(tHduModePortAgt1.GetOutPortType());
					atChnModePort[0].SetScalingMode(tHduModePortAgt1.GetScalingMode());
					atChnModePort[1].SetZoomRate( tHduModePortAgt2.GetZoomRate() );
					atChnModePort[1].SetOutModeType(tHduModePortAgt2.GetOutModeType());
					atChnModePort[1].SetOutPortType(tHduModePortAgt2.GetOutPortType());
					atChnModePort[1].SetScalingMode(tHduModePortAgt2.GetScalingMode());
					OspPost(MAKEIID(AID_MCU_PERIEQPSSN, atHduInfo[nLoop1].GetEqpId()), MCU_HDU_CHANGEMODEPORT_NOTIF, (void*)atChnModePort, sizeof(atChnModePort));
				}
				else
				{
					LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "The Hdu Id is wrong or first register!\n");
				}
			}
		}     
	}
	wRet = g_cMcuAgent.WriteHduTable(byHduNum,atHduInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_HDU)
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] write hdu info failed, ret:%d!\n", wRet);
		return FALSE;
	}

	// ����Ҫ�����ڴ��еĶ˿���Ϣ[12/15/2011 chendaiwei]
	wRet = g_cMcuAgent.WriteSvmpTable(bySvmpNum,atSvmpInfo,FALSE);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_SVMP)
		
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] write svmp info failed, ret:%d!\n", wRet);
		return FALSE;
	}

	//˫VMPд��
//     TEqpDvmpBasicInfo atDvmpInfo[MAXNUM_PERIEQP];
//     memcpy(atDvmpInfo, atDvmpInfo1, byDvmpNum);
// 	memcpy((atDvmpInfo + byDvmpNum), atDvmpInfo2, byDvmpNum);
// 	wRet = g_cMcuAgent.WriteDvmpTable(byDvmpNum*2,atDvmpInfo);
// 	if (SUCCESS_AGENT != wRet)
// 	{
// 		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_DVMP)
// 			
// 			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ProcBrdCfg] write Dvmp info failed, ret:%d!\n", wRet);
// 		return FALSE;
// 	}

	TEqpMpuBasInfo atMpuCurBasInfo[MAXNUM_PERIEQP];
	u8 byCurMpuBasNum = 0;
	wRet = g_cMcuAgent.ReadMpuBasTable(&byCurMpuBasNum,atMpuCurBasInfo);
	if (SUCCESS_AGENT != wRet)
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] read mpubas info failed, ret:%d!\n", wRet);
		return FALSE;
	}
	
	//��������IP��ַ��Ϣ[12/24/2012 chendaiwei]
	TBoardInfo atTmpBrdInfo[MAX_BOARD_NUM]; 
	u8 byTmpBrdNum = 0;
	memset(atTmpBrdInfo, 0, sizeof(atTmpBrdInfo));
	g_cMcuAgent.ReadBrdTable(&byTmpBrdNum, atTmpBrdInfo);
	for (u8 byIndex=0; byIndex<byCurMpuBasNum; byIndex++)
	{
		for (u8 byBrdLoop=0; byBrdLoop<byTmpBrdNum; byBrdLoop++)
		{
			if ( atTmpBrdInfo[byBrdLoop].GetBrdId() == atMpuCurBasInfo[byIndex].GetRunBrdId() )
			{
				atMpuCurBasInfo[byIndex].SetIpAddr( atTmpBrdInfo[byBrdLoop].GetBrdIp() );
				break;
			}
		}
	}

	//  Bug00085246 �Ƚ�MPU��Bas�����Ƿ��иı䣬����У���Ҫ������������MPU�ı�־λ[5/3/2012 chendaiwei]
	if( byCurMpuBasNum != byMpuBasNum ||
		( byCurMpuBasNum == byMpuBasNum && 0 != memcmp(atMpuCurBasInfo,atMpuBasInfo,sizeof(TEqpMpuBasInfo)*byCurMpuBasNum)))
	{
		g_cMcuAgent.SetRebootAllMpuBasFlag(TRUE);
	}

	// ����Ҫ�����ڴ��еĶ˿���Ϣ[12/15/2011 chendaiwei]
	wRet = g_cMcuAgent.WriteMpuBasTable(byMpuBasNum,atMpuBasInfo,FALSE);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_MPUBAS)
			
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] write mpubas info failed, ret:%d!\n", wRet);
		return FALSE;
	}

//	wRet = g_cMcuAgent.WriteEbapTable(byEbapNum,atEbapInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_EBAP)
			
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] write ebap info failed, ret:%d!\n", wRet);
		return FALSE;
	}

//	wRet = g_cMcuAgent.WriteEvpuTable(byEvpuNum,atEvpuInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_EVPU)
			
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] write evpu info failed, ret:%d!\n", wRet);
		return FALSE;
	}
	
	// д��Vrs��¼����Ϣ��
	wRet = g_cMcuAgent.WriteVrsRecTable(byVrsRecNum, ptVrsRecCfg);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_REC)
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg] write vrsrec info failed, ret:%d!\n", wRet);
		return FALSE;
	}

#ifndef _MINIMCU_
	byBrdNum = 0;
	memset(atBrdInfo, 0, sizeof(atBrdInfo));
	g_cMcuAgent.ReadBrdTable(&byBrdNum, atBrdInfo);
	u8 byLayer = 0;
	u8 bySlot = 0;
	u8 byType = 0;
	CServMsg cBrdCfgMsg;
	for( dwBrdLoop = 0; dwBrdLoop < byBrdNum; dwBrdLoop++ )
	{
		if(BOARD_STATUS_INLINE == atBrdInfo[dwBrdLoop].GetState())
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CFG,  "[BrdCfgProcIntegration] Board [%d, %d] is online!\n", 
																				   atBrdInfo[dwBrdLoop].GetLayer(), 
																				   atBrdInfo[dwBrdLoop].GetSlot());
			for ( nIndex = 0; nIndex < byCruBrdNum; nIndex++ )
			{
				//  [1/21/2011 chendaiwei]֧��MPC2
				if ( atCurBrdInfo[nIndex].GetType() == atBrdInfo[dwBrdLoop].GetType() 
					&& atCurBrdInfo[nIndex].GetLayer() == atBrdInfo[dwBrdLoop].GetLayer()
					&& atCurBrdInfo[nIndex].GetSlot() == atBrdInfo[dwBrdLoop].GetSlot()
					&& atCurBrdInfo[nIndex].GetBrdIp() == atBrdInfo[dwBrdLoop].GetBrdIp()
					&& atBrdInfo[dwBrdLoop].GetType() != BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ 
					&& atBrdInfo[dwBrdLoop].GetType() != BRD_TYPE_MPC2 )
				{
	//				memset(&cCfgBuf, 0, sizeof(cCfgBuf));
	//				g_cMcuAgent.PackEqpConfig( cCfgBuf, atBrdInfo[dwBrdLoop] );
	//
	//				if ( g_cMcuAgent.IsRegedBoard(atBrdInfo[dwBrdLoop].GetType(), atBrdInfo[dwBrdLoop].GetLayer(), atBrdInfo[dwBrdLoop].GetSlot()) )
	//				{
	//					byLayer = atBrdInfo[dwBrdLoop].GetLayer();
	//					bySlot = atBrdInfo[dwBrdLoop].GetSlot();
	//					cBrdCfgMsg.SetMsgBody((u8*)&byLayer, sizeof(byLayer));
	//					cBrdCfgMsg.CatMsgBody((u8*)&bySlot,sizeof(bySlot));
	//
	//					u16 wLen = 0;
	//					u8* pbyBuf = cCfgBuf.GetBuffer(wLen);
	//					cBrdCfgMsg.CatMsgBody(pbyBuf, wLen);
	//					post(MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_BOARDCFGMODIFY_NOTIF, cBrdCfgMsg.GetServMsg(), cBrdCfgMsg.GetServMsgLen());
	//				}
					byLayer = atBrdInfo[dwBrdLoop].GetLayer();
					bySlot = atBrdInfo[dwBrdLoop].GetSlot();
					byType = atBrdInfo[dwBrdLoop].GetType();
					cBrdCfgMsg.SetMsgBody((u8*)&byLayer, sizeof(byLayer));
					cBrdCfgMsg.CatMsgBody((u8*)&bySlot,sizeof(bySlot));
					cBrdCfgMsg.CatMsgBody((u8*)&byType,sizeof(byType));
					post(MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_BOARDCFGMODIFY_NOTIF, cBrdCfgMsg.GetServMsg(), cBrdCfgMsg.GetServMsgLen());
					if (!g_cMcuAgent.RefreshBrdOsType(atBrdInfo[dwBrdLoop].GetBrdId(),atCurBrdInfo[nIndex].GetOSType()))
					{
						LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[BrdCfgProcIntegration]RefreshBrdOsType()BrdId:%d,Layer:%d,Slot:%d,BrdType:%d,OsType:%d failed!\n",
							atBrdInfo[dwBrdLoop].GetBrdId(),byLayer,bySlot,byType,atCurBrdInfo[nIndex].GetOSType());
					}
					break;
				} //if ( atCurBrdInfo[nIndex].GetType ...
			} //for ( nIndex = 0; nIndex < byCruB ...
		}
	} //for( dwBrdLoop = 0; dwBrdL...
#endif

// �����Ϣ�壬TDSCModuleInfo�ṹ��MCS_MCU_GETMCUGENERALCFG_REQ��Ϣ��ת�Ƶ�MCS_MCU_GETBRDCFG_REQ, zgc, 2007-03-15
#ifdef _MINIMCU_
	if ( bIsExistDsc )
	{
		if ( !CheckNewDscInfo( tAgentDscInfo ) )
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_BRD)
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG,  "[ProcBrdCfg] New dscinfo is error! Nack!\n" );
			return FALSE;
		}
	}
	else
	{
//		if ( bIsRegDsc )
//		{
		u8 byCurDscType = 0;
		for ( nIndex = 0; nIndex < byCruBrdNum; nIndex++ )
		{
			if( BRD_TYPE_DSC == atCurBrdInfo[nIndex].GetType() ||
				BRD_TYPE_MDSC == atCurBrdInfo[nIndex].GetType() ||
				BRD_TYPE_HDSC == atCurBrdInfo[nIndex].GetType() )
			{
				byCurDscType = atCurBrdInfo[nIndex].GetType();
				break;
			}
		}
		if ( 0 != byCurDscType )
		{
			CServMsg cDisMsg;
			u8 byBrdLayer = 0;
			u8 byBrdSlot = 0;
			cDisMsg.SetMsgBody((u8*)&byBrdLayer, sizeof(byBrdLayer));
			cDisMsg.CatMsgBody((u8*)&byBrdSlot, sizeof(byBrdSlot));
			cDisMsg.CatMsgBody((u8*)&byCurDscType, sizeof(byCurDscType));
			post( MAKEIID(AID_MCU_AGENT,1), SVC_AGT_DISCONNECTBRD_CMD, cDisMsg.GetServMsg(), cDisMsg.GetServMsgLen()  );
		}
//		}
	}

	CServMsg cDscInfoMsg;
	u8 byBrdLayer = 0;
	u8 byBrdSlot = 0;
	cDscInfoMsg.SetMsgBody((u8*)&byBrdLayer, sizeof(byBrdLayer));
	cDscInfoMsg.CatMsgBody((u8*)&byBrdSlot, sizeof(byBrdSlot));
	cDscInfoMsg.CatMsgBody((u8*)&tAgentDscInfo, sizeof(tAgentDscInfo));
	post( MAKEIID(AID_MCU_AGENT,1), SVC_AGT_SETDSCINFO_REQ, cDscInfoMsg.GetServMsg(), cDscInfoMsg.GetServMsgLen()  );

	SetTimer(MCUCFG_DSCCFG_WAITINGCHANGE_OVER_TIMER, 10000 );
	g_cMcuAgent.SetDscInfo( &tAgentDscInfo, FALSE );  //���ڷ�����MCS�����޸��ļ�
#endif

	return TRUE;
}

/*=============================================================================
�� �� ���� CheckNewDscInfo
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TDSCModuleInfo tDscInfo
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/9/18   4.0			�ܹ��                  ����
=============================================================================*/
#ifdef _MINIMCU_
BOOL32 CMcuCfgInst::CheckNewDscInfo(TDSCModuleInfo tDscInfo)
{
	TDSCModuleInfo tLocalDscInfo;
	g_cMcuAgent.GetDscInfo( &tLocalDscInfo );
	u32 dwDscInnerIp = tLocalDscInfo.GetDscInnerIp();
	u32 dwMcuInnerIp = tLocalDscInfo.GetMcuInnerIp();
	u32 dwInnerIpMask = tLocalDscInfo.GetInnerIpMask();
	u32 dwNewCfgIp1 = 0;
	u32 dwNewCfgIp2 = 0;
	u32 dwNewCfgIpMask1 = 0;
	u32 dwNewCfgIpMask2 = 0;
	u32 dwNewCfgGWIp1 = 0;
	u8 byIpMaskBitNum = 0;
	u8 byCallAddrNum = tDscInfo.GetCallAddrNum();
	u8 byMcsAccessAddrNum = tDscInfo.GetMcsAccessAddrNum();
	u8 byLop = 0;
	u8 byLop1 = 0;
	u8 byLop2 = 0;

	TMINIMCUNetParamAll tNetParamAll;
	TMINIMCUNetParam tNetParam;
			
	if ( tDscInfo.GetNetType() == NETTYPE_INVALID )
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[CheckNewDscInfo] NetType INVALID!\n");
		return FALSE;
	}
	
	tDscInfo.GetCallAddrAll( tNetParamAll );
	if ( !tNetParamAll.IsValid() )
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[CheckNewDscInfo] DSC module ip num error! McsAccessAddrNum.%d, CallAddrNum.%d\n", byMcsAccessAddrNum, byCallAddrNum);
		return FALSE; 
	}

	memset( &tNetParamAll, 0, sizeof(tNetParamAll) );
	tDscInfo.GetMcsAccessAddrAll( tNetParamAll );
	if ( !tNetParamAll.IsValid() )
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[CheckNewDscInfo] DSC module ip num error! McsAccessAddrNum.%d, CallAddrNum.%d\n", byMcsAccessAddrNum, byCallAddrNum);
		return FALSE;
	}
	// IP������Ϊ0
	if ( 0 == byMcsAccessAddrNum || MAXNUM_MCSACCESSADDR < byMcsAccessAddrNum 
		|| 0 == byCallAddrNum || ETH_IP_MAXNUM-1 < byCallAddrNum )
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[CheckNewDscInfo] DSC module ip num error! McsAccessAddrNum.%d, CallAddrNum.%d\n", byMcsAccessAddrNum, byCallAddrNum);
		return FALSE;
	}

	tDscInfo.GetMcsAccessAddrAll( tNetParamAll );
	// ���������ӵ�ַ�Ƿ�˴˳�ͻ, zgc, 2007-07-17
	// ���������ӵ�ַ�����ص�ַ�Ƿ���ͬһ������, zgc, 2007-08-23
	LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG,  "[CheckNewDscInfo] Check McsAccessAddr!\n" );
	if ( !IsValidNetParamAll(tNetParamAll) )
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG,  "[CheckNewDscInfo] McsAccessAddr config error!\n");
		return FALSE;
	}
	
	// ������е�ַ�Ƿ�˴˳�ͻ(�����ڿ�IP), zgc, 2007-07-17
	memset( &tNetParamAll, 0, sizeof(tNetParamAll) );
	tDscInfo.GetCallAddrAll(tNetParamAll);
	LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG,  "[CheckNewDscInfo] Check CallAddr!\n" );
	if ( !IsValidNetParamAll(tNetParamAll, dwDscInnerIp, dwInnerIpMask) )
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG,  "[CheckNewDscInfo] CallAddr config error!\n");
		return FALSE;
	}

	return TRUE;
}
#endif
/*=============================================================================
�� �� ���� IsValidNetParamAll
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TMINIMCUNetParamAll tNetParamAll
           u32 dwIpAddr
           u32 dwIpMask
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/9/18   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CMcuCfgInst::IsValidNetParamAll(TMINIMCUNetParamAll tNetParamAll, u32 dwIpAddr, u32 dwIpMask)
{
	u8 byNetParamNum = tNetParamAll.GetNetParamNum();
	if ( 0 == byNetParamNum )
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[IsValidNetParamAll] The netparam num is zero!\n");
		return FALSE;
	}
	
	u32 dwNewCfgIp1 = 0;
	u32 dwNewCfgIp2 = 0;
	u32 dwNewCfgIpMask1 = 0;
	u32 dwNewCfgIpMask2 = 0;
	u32 dwNewCfgGWIp1 = 0;
	u8 byLop1, byLop2;
	TMINIMCUNetParam tNetParam;
	for ( byLop1 = 0; byLop1 < byNetParamNum; byLop1++ )
	{
		if ( !tNetParamAll.GetNetParambyIdx(byLop1, tNetParam) )
		{
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[IsValidNetParamAll] Get NetParam.%d error!\n", byLop1);
			return FALSE;
		}
		
		dwNewCfgIp1 = tNetParam.GetIpAddr();
		dwNewCfgIpMask1 = tNetParam.GetIpMask();
		dwNewCfgGWIp1 = tNetParam.GetGatewayIp();			
		// IPMASK���ܴ���30λ,����ȫ0,�Ҳ���0,1���
		if ( !IsIpMaskValid(dwNewCfgIpMask1) )
		{
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[IsValidNetParamAll] Ip0x%x: IpMask is invalid!\n", dwNewCfgIp1);
			return FALSE;
		}
		// ����IP����ȫ0��ȫ1
		dwNewCfgIpMask1 = ~dwNewCfgIpMask1;
		if ( (dwNewCfgIp1&dwNewCfgIpMask1) == 0 
			|| (dwNewCfgIp1&dwNewCfgIpMask1) == dwNewCfgIpMask1 )
		{
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[IsValidNetParamAll] subnet ip.0x%x can not be network or broadcast!\n", dwNewCfgIp1);
			return FALSE;
		}
		dwNewCfgIpMask1 = ~dwNewCfgIpMask1;

		//IP������Ӧ����ͬһ����
		if ( (dwNewCfgIp1&dwNewCfgIpMask1) != (dwNewCfgGWIp1&dwNewCfgIpMask1) )
		{
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcBrdCfg]WAN ip.0x%x should be in same subnet with GWIp!\n", dwNewCfgIp1);
			return FALSE;
		}

		for ( byLop2 = byLop1+1; byLop2 < byNetParamNum; byLop2++ )
		{
			if ( !tNetParamAll.GetNetParambyIdx(byLop2, tNetParam) )
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[IsValidNetParamAll] Get NetParam.%d error!\n", byLop2);
				return FALSE;
			}
			// ����IP�Ƿ���ͬһ������
			dwNewCfgIp2 = tNetParam.GetIpAddr();
			dwNewCfgIpMask2 = tNetParam.GetIpMask();
			if ( 
				(dwNewCfgIp1&dwNewCfgIpMask1) == (dwNewCfgIp2&dwNewCfgIpMask1) 
				|| (dwNewCfgIp1&dwNewCfgIpMask2) == (dwNewCfgIp2&dwNewCfgIpMask2) 
			   )
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[IsValidNetParamAll] ip.0x%x conflict to ip.0x%x!\n ",
						dwNewCfgIp1, dwNewCfgIp2);
				return FALSE;
			}
		}

		// �����������Ȥ��IP��ַ�ĳ�ͻ����
		if ( dwIpAddr != 0)
		{
			if ( 
				(dwNewCfgIp1&dwNewCfgIpMask1) == (dwIpAddr&dwNewCfgIpMask1) 
				|| (dwNewCfgIp1&dwIpMask) == (dwIpAddr&dwIpMask) 
				)
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[IsValidNetParamAll] ip.0x%x conflict to outer ip.0x%x!\n ",
						dwNewCfgIp1, dwIpAddr);
				return FALSE;
			}
		}	
	}	//for ( byLop1 = 0; byLop1 < byNetParamNum ...

	return TRUE;
}

/*=============================================================================
  �� �� ���� ProcVCSGeneralCfg
  ��    �ܣ� VCS������Ϣ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void   
=============================================================================*/
void  CMcuCfgInst::ProcVCSGeneralCfg(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	u8 *pbyMsg  = cServMsg.GetMsgBody();
	u16 wMsgLen = cServMsg.GetMsgBodyLen();

	s8 achSoftName[MAX_VCSSOFTNAME_LEN];
	memset(achSoftName, 0, sizeof(achSoftName));
	if (VCS_MCU_GETSOFTNAME_REQ == pcMsg->event)
	{
		g_cMcuAgent.GetVCSSoftName(achSoftName);
		cServMsg.SetMsgBody((u8*)achSoftName, sizeof(achSoftName));
		SendMsg2Mcsssn(cServMsg, pcMsg->event + 1);
	}
	else if (VCS_MCU_CHGSOFTNAME_REQ == pcMsg->event)
	{
		memcpy(achSoftName, pbyMsg, min(wMsgLen, MAX_VCSSOFTNAME_LEN));

		u16 wRet = g_cMcuAgent.SetVCSSoftName(achSoftName);
		if (SUCCESS_AGENT != wRet)            
		{
			cServMsg.SetMsgBody(NULL, 0);                           
			cServMsg.SetErrorCode(ERR_MCU_VCS_WRITESOTTNAME);                            
			SendMsg2Mcsssn(cServMsg,  pcMsg->event + 2); 
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ProcVCSGeneralCfg]write softname failed. ret:%d!\n", wRet);
			return;
		}
		else
		{
			SendMsg2Mcsssn(cServMsg, pcMsg->event + 1);
		}

		CMcsSsn::BroadcastToAllMcsSsn(pcMsg->event + 3, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

	}
}

void CMcuCfgInst::ReConstructTMcuNetCfgForMcs( TMcu8KECfg &tMcuEqpCfg, TMcuNetCfg &tMcuNetCfg)
{

#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)

    u32 dwAdaptNum = tMcuEqpCfg.m_tLinkNetAdap.Length();
	
    printf( "[ReConstructTMcuNetCfgForMcs] Adapte num = %d\n", dwAdaptNum );
	
	
    for ( u32 dwAdaptIdx = 0; dwAdaptIdx < dwAdaptNum; dwAdaptIdx++ )
    {
		
        TNetAdaptInfo * ptNetAdapt = tMcuEqpCfg.m_tLinkNetAdap.GetValue(dwAdaptIdx);
        if ( NULL == ptNetAdapt )
        {
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ReConstructTMcuNetCfgForMcs] Get ptNetAdapt for %d failed!\n", dwAdaptIdx);
            continue;
        }
		
		u32 dwRealIdx = ptNetAdapt->GetAdaptIdx();
#ifndef WIN32
		
//         if ( IsValidEthIfName( ptNetAdapt->GetAdapterName() ) )
//         {
//             dwRealIdx = GetEthIdxbyEthIfName( ptNetAdapt->GetAdapterName() );       
// 			//            dwHardIdx = g_cMcuAgent.EthIdxSys2Hard( dwHardIdx );		
// 			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG,  "[ReConstructTMcuNetCfgForMcs] EthIdx = %d\n", dwRealIdx );
//         }
//         else
//         {
//             LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[ReConstructTMcuEqpCfgForMcs] Adapter name %s is error!\n", ptNetAdapt->GetAdapterName() );
//         }
// 		
// 		ptNetAdapt->SetAdaptIdx( dwRealIdx );
#endif //LINUX

		TNetParam *ptNetParam = ptNetAdapt->m_tLinkIpAddr.GetValue(0);
		if( NULL == ptNetParam)
		{
			LogPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[ReConstructTMcuNetCfgForMcs] Get IpAddr failed!\n");
			continue;
		}
		
		u32 dwIp	= ptNetParam->GetIpAddr();
		u32 dwMask	= ptNetParam->GetIpMask();
		u32 dwGw	= ptNetAdapt->GetDefGWIpAddr();
		tMcuNetCfg.m_atMcuEqpCfg[dwRealIdx].SetMcuIpAddr(dwIp);
		tMcuNetCfg.m_atMcuEqpCfg[dwRealIdx].SetMcuSubNetMask(dwMask);
		tMcuNetCfg.m_atMcuEqpCfg[dwRealIdx].SetGWIpAddr(dwGw);
    }
	
	tMcuNetCfg.m_byInterface = g_cMcuAgent.GetInterface();
#endif	

	LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG, "[ReConstructTMcuNetCfgForMcs] now interface is:%d adpNum:%d\n", tMcuNetCfg.m_byInterface,tMcuEqpCfg.m_tLinkNetAdap.Length());

    return;
}

/*=============================================================================
�� �� ���� ReConstructTMcuEqpCfgForMcs
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����	TMcuEqpCfg & tMcuEqpCfg		[i]
			TMcuEqpCfg & tEqpCfg		[o]
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2010/01/18   8000E		  Ѧ��                  ����
=============================================================================*/
// void CMcuCfgInst::ReConstructTMcuEqpCfgForMcs( TMcu8KECfg & tMcuEqpCfg, TMcuEqpCfg &tEqpCfg )
// {
// #if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
//     u32 dwAdaptNum = tMcuEqpCfg.m_tLinkNetAdap.Length();
// 	
//     printf( "[ReConstructTMcuEqpCfgForMcs] Adapte num = %d\n", dwAdaptNum );
// 	
// 	
//     for ( u32 dwAdaptIdx = 0; dwAdaptIdx < dwAdaptNum; dwAdaptIdx++ )
//     {
// 		
//         TNetAdaptInfo * ptNetAdapt = tMcuEqpCfg.m_tLinkNetAdap.GetValue(dwAdaptIdx);
//         if ( NULL == ptNetAdapt )
//         {
//             continue;
//         }
// #ifndef WIN32
// 		u32 dwHardIdx = 0;
// 		
//         if ( IsValidEthIfName( ptNetAdapt->GetAdapterName() ) )
//         {
//             dwHardIdx = GetEthIdxbyEthIfName( ptNetAdapt->GetAdapterName() );       
// //            dwHardIdx = g_cMcuAgent.EthIdxSys2Hard( dwHardIdx );		
// 			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG,  "[ReConstructTMcuEqpCfgForMcs] EthIdx = %d\n", dwHardIdx );
//         }
//         else
//         {           
//             LogPrint(LOG_LVL_WARNING, MID_MCU_CFG,  "[ReConstructTMcuEqpCfgForMcs] Adapter name %s is error!\n", ptNetAdapt->GetAdapterName() );
//         }
// 		
// 		ptNetAdapt->SetAdaptIdx( dwHardIdx );
// #endif //LINUX
// 		
//     }
// 	
// 	// [3/26/2010 xliang] FIXME: ȡeth0 ��eth1 �������ŵ�������Ϣ, ѡ�����ں�ȡָ�����ڵĵ�0��������Ϊ����ַ
// 	TNetAdaptInfo * ptNetAdapt = tMcuEqpCfg.m_tLinkNetAdap.GetValue(0);		
// 	
// 	if( ptNetAdapt == NULL )
// 	{
// 		LogPrint(LOG_LVL_WARNING, MID_MCU_CFG,  "[ReConstructTMcuEqpCfgForMcs] ptNetAdapt is NULL!\n");
// 		return;
// 	}
// 
// 	TNetParam *ptNetParam = ptNetAdapt->m_tLinkIpAddr.GetValue(0);			//host ip, mask
// 
// 	u32 dwDefGWIpAddr = ptNetAdapt->GetDefGWIpAddr();
// //	TNetParam *ptGWParam  = ptNetAdapt->m_tLinkDefaultGateway.GetValue(0);	//����ip
// 	
// 	//TMcuEqpCfg tEqpCfg;
// 	u8 byLocalLayer = 0;
// 	u8 byLocalSlot  = 0;
// 	// 	g_cMcuAgent.GetLocalLayer(byLocalLayer);
// 	// 	g_cMcuAgent.GetLocalSlot(byLocalSlot);
// 	tEqpCfg.SetMcuIpAddr(ptNetParam->GetIpAddr());
// 	tEqpCfg.SetMcuSubNetMask(ptNetParam->GetIpMask());
// 	
// 	//	tEqpCfg.SetGWIpAddr(ntohl(g_cMcuAgent.GetGateway())); 
// 	tEqpCfg.SetGWIpAddr(dwDefGWIpAddr);
// 	tEqpCfg.SetLayer(byLocalLayer);
// 	tEqpCfg.SetSlot(byLocalSlot);
// 	tEqpCfg.SetInterface( g_cMcuAgent.GetInterface() );
// 	
// 	
// #endif	
//     return;
// 
// }

/*=============================================================================
�� �� ���� SetMultiNetCfgToSys
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����	TMultiNetCfgInfo & tMultiNetCfgInfo	[i]
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2010/03/16   4.6		Ѧ��                  ����
=============================================================================*/
//�ýӿڲ��ᱻ���� [5/7/2012 chendaiwei]
// void CMcuCfgInst::SetMultiNetCfgToSys(TMultiNetCfgInfo &tMultiNetCfgInfo)
// {
// #if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
// 
// 	TMcu8KECfg tMcu8KECfg;
// 	g_cMcuAgent.GetMcuEqpCfg(&tMcu8KECfg);
// 	
// 	//ͨ��TMultiNetCfgInfo����TMcu8KECfg
// 	for(u8 byEthIdx = 0; byEthIdx < MAXNUM_ETH_INTERFACE; byEthIdx ++)
// 	{
// 		TMcuEthCfg8KE *ptMcuEthCfg8KE = tMultiNetCfgInfo.GetMcuEthCfg(byEthIdx);
// 		if( ptMcuEthCfg8KE == NULL )
// 		{
// 			continue;
// 		}
// 		
// 		TNetAdaptInfo *ptNetAdapt = tMcu8KECfg.m_tLinkNetAdap.GetValue(byEthIdx);
// 		if(ptNetAdapt == NULL)
// 		{
// 			continue;
// 		}
// 		ptNetAdapt->m_tLinkIpAddr.Clear();
// 		ptNetAdapt->m_tLinkDefaultGateway.Clear();
// 		
// 		// ĳ���ڵ�ip��
// 		u8 byIpSecNum = ptMcuEthCfg8KE->GetIpSecNum();
// 		for( u8 byIpSecIdx = 0; byIpSecIdx < byIpSecNum; byIpSecIdx ++)
// 		{
// 			u32 dwHIp	= ptMcuEthCfg8KE->GetMcuIpAddr(byIpSecIdx);
// 			u32 dwHMask = ptMcuEthCfg8KE->GetMcuSubNetMask(byIpSecIdx);
// 			u32 dwHGWIp	= ptMcuEthCfg8KE->GetGWIpAddr(byIpSecIdx);
// 			
// 			if( dwHIp == 0 )
// 			{
// 				continue;
// 			}
// 
// 			
// 			TNetParam tNetParam;
// 			tNetParam.SetNetParam(dwHIp, dwHMask);		//����һ��Ҫ������//�����ݿ���̶�����255.255.255.255
// 			
// 			TNetParam tGwParam;
// 			tGwParam.SetNetParam(dwHGWIp, dwHMask);		//���ص����밴���������
// 			
// 			ptNetAdapt->m_tLinkIpAddr.Append(tNetParam);
// 			ptNetAdapt->m_tLinkDefaultGateway.Append(tGwParam);
// 			
// 		}
// 
// 		//Ĭ������
// // 		TNetParam tGwParam;
// // 		u32 dwDefGWIp = ptMcuEthCfg8KE->GetDefGWIpAddr();
// // 		tGwParam.SetNetParam(dwDefGWIp, 0);
// // 		ptNetAdapt->m_tLinkDefaultGateway.Append(tGwParam);
// //		ptNetAdapt->m_dwDefGWIpAddr = ptMcuEthCfg8KE->GetDefGWIpAddr();
// 		ptNetAdapt->SetDefGWIpAddr(ptMcuEthCfg8KE->GetDefGWIpAddr());
// 	}
// 	//ˢ��ϵͳ����
// 	g_cMcuAgent.Save8KENetCfgToSys(&tMcu8KECfg);
// 
// 
// 	
// 
// 	return;
// #endif
// 
// }

/*=============================================================================
�� �� ���� GetMultiNetCfgFromSys
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����	TMultiNetCfgInfo & tMultiNetCfgInfo	[o]
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2010/03/16   4.6		Ѧ��                  ����
=============================================================================*/
/*
void CMcuCfgInst::GetMultiNetCfgFromSys(TMultiNetCfgInfo &tMultiNetCfgInfo)
{
#ifdef _8KE_
	
// 	TNetAdaptInfoAll tNetAdapterInfoAll;
//     if ( GetNetAdapterInfo( &tNetAdapterInfoAll ) ) //��ȡ���������Ϣ
//     {
//         printf("[AgentGetNetworkInfo] New mcu agent!\n");
//         g_cMcuAgent.SetMcuEqpCfg( &tNetAdapterInfoAll );
//     }

	TMcu8KECfg tMultiEthCfg;
	g_cMcuAgent.GetMcuEqpCfg(&tMultiEthCfg);
	
	// reconstruct TMultiNetCfgInfo
	u32 dwAdaptNum = tMultiEthCfg.m_tLinkNetAdap.Length();
	
	for ( u32 dwAdaptIdx = 0; dwAdaptIdx < dwAdaptNum; dwAdaptIdx++ )
	{
		if( dwAdaptIdx >= MAXNUM_ETH_INTERFACE) //8000E �������3��
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "dwAdaptNum is %d\n", dwAdaptNum);
			break;
		}
		TNetAdaptInfo * ptNetAdapt = tMultiEthCfg.m_tLinkNetAdap.GetValue(dwAdaptIdx);
		if ( NULL == ptNetAdapt )
		{
			continue;
		}
#ifndef WIN32
		u32 dwHardIdx = 0;
		
		if ( IsValidEthIfName( ptNetAdapt->GetAdapterName() ) )
		{
			dwHardIdx = GetEthIdxbyEthIfName( ptNetAdapt->GetAdapterName() );       
			dwHardIdx = g_cMcuAgent.EthIdxSys2Hard( dwHardIdx );		
			LogPrint(LOG_LVL_DETAIL, MID_MCU_CFG,  "EthIdx = %d\n", dwHardIdx );
		}
		else
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[ReConstructTMcuEqpCfgForMcs] Adapter name %s is error!\n", ptNetAdapt->GetAdapterName() );
		}
		
		ptNetAdapt->SetAdaptIdx( dwHardIdx );
#endif //LINUX

		u8 byIpSecIdx = 0;
		TMcuEthCfg8KE tMcuEthCfg8KE; 
		for( ; byIpSecIdx < MCU_MAXNUM_ADAPTER_IP; byIpSecIdx ++)
		{
			//host ip, mask
			TNetParam *ptNetParam = ptNetAdapt->m_tLinkIpAddr.GetValue(byIpSecIdx);
			if( ptNetParam == NULL)
			{
				continue;
			}
			
			if( !ptNetParam->IsValid() )
			{
				continue;
			}
			
			// [3/26/2010 xliang] ip ��Ӧ�ľ�̬·�ɣ�����Ӧ����- ���Զ��������ļ���ȡ
			// ȱʡ����Ĭ�����ء�
			u32 dwGWIp = ptNetAdapt->GetDefGWIpAddr();
 			TNetParam *ptGWParam  = ptNetAdapt->m_tLinkDefaultGateway.GetValue(byIpSecIdx);	//����ip
			if( ptGWParam != NULL && ptGWParam->IsValid()) 
			{
				dwGWIp = ptGWParam->GetIpAddr();
			}
			
			tMcuEthCfg8KE.AddIpSection(ptNetParam->GetIpAddr(), ptNetParam->GetIpMask(), dwGWIp);
			
		}
		// default GW
		tMcuEthCfg8KE.SetDefGWIpAddr( ptNetAdapt->GetDefGWIpAddr() );

		// Set linkStatus
		//tMcuEthCfg8KE.SetLinkStatus();
		
		// FIXME: Set work mode
		//tMcuEthCfg8KE.SetNetWorkMode();
		
		tMultiNetCfgInfo.SetMcuEthCfg( dwAdaptIdx, tMcuEthCfg8KE);
	}
	tMultiNetCfgInfo.Print();
#endif
}
*/

/*=============================================================================
�� �� ���� WriteMixerCfgToFile
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����	u8 byMixNum, TEqpMixerInfo* ptMixerTable
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		 �汾		�޸���		�߶���    �޸�����
2011/08/15   4.6		��־��                 ����
=============================================================================*/
void CMcuCfgInst::WriteMixerCfgToFile( u8 byMixNum, TEqpMixerInfo* ptMixerTable )
{
	FILE *hFile = NULL;
	s8  achFileName[64] = {0};
	sprintf(achFileName, "%s/%s", DIR_CONFIG,"MixerCfg.ini");		
	hFile = fopen(achFileName, "a+b");
	if (NULL == hFile)
	{
		OspPrintf( TRUE, FALSE, "[WriteMixerCfgToFile]Waring:Open File(%s) Failed,So Return!\n",achFileName);
		return;
	}
	s8 achCfgInfo[255] = {0};
	time_t tiCurTime = ::time(NULL);             
    int nLen = sprintf(achCfgInfo, "\n%s\n", ctime(&tiCurTime));
	fwrite((u8*)achCfgInfo, nLen, 1, hFile);
	for (u8 byMixIdx = 0 ; byMixIdx < byMixNum ; byMixIdx++)
	{
		if (ptMixerTable == NULL)
		{
			OspPrintf( TRUE, FALSE, "[WriteMixerCfgToFile]byMixIdxe(%d),ptMixerTable == NULL,So Return!\n",byMixIdx);
			return;
		}
		nLen = sprintf(achCfgInfo, "EqpId:[%d]RunBrdId:[%d]RunBrdIp:[%x]RcvPort:[%d]SwitchBrdId:[%d]SwitchPort[%d]MixName:[%s]\t\n",
			ptMixerTable->GetEqpId(),ptMixerTable->GetRunBrdId(),ptMixerTable->GetIpAddr(),ptMixerTable->GetEqpRecvPort(),
			ptMixerTable->GetSwitchBrdId(),ptMixerTable->GetMcuRecvPort(),ptMixerTable->GetAlias());
		fwrite((u8*)achCfgInfo, nLen, 1, hFile);
		ptMixerTable++;
	}
	fclose(hFile);
	hFile = NULL;
}
/*====================================================================
������      : AdjustBrdCfgTablePos
����        : ���¾�������Ϣ���бȽϣ�����Ҫ����İ�������Ϣ����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u8 byBrdNum ���°����
			  TBoardInfo *ptNewBrdCfginfo ���°��б�,
			  TBoardInfo *ptAdjustedBrdCfgInfo ������������б�
����ֵ˵��  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
11/12/27                ��־��          ����
====================================================================*/
/*void CMcuCfgInst::AdjustBrdCfgTablePos(u8 byBrdNum ,TBoardInfo *ptNewBrdCfginfo,TBoardInfo *ptAdjustedBrdCfgInfo)
{
	//�������ԭ���ڱ�֤������Ϣ���յ�ǰ���¾�����ԭ�ȴ��ڵİ���λ�ñ��ֲ��䡣
	//�������ñ�������޸ģ����ǽ����е��嶼Ҫ����д�������ļ������ǽ����������д�����ļ������ٶ������ļ��Ĳ�����
	TBoardInfo atCurBrdInfo[MAXNUM_BOARD];
	memset(atCurBrdInfo, 0, sizeof(atCurBrdInfo));
	u8 byCruBrdNum = 0;
	g_cMcuAgent.ReadBrdTable( &byCruBrdNum, atCurBrdInfo );
	u8 abyIsWrite[ MAXNUM_BOARD / 8 + 1 ];
	memset( &abyIsWrite[0],0,sizeof(abyIsWrite) );

	u8 byOldIdx = 0,byNewIdx = 0;
	BOOL32 bIsNewTblExists = FALSE;
	for( byOldIdx = 0;byOldIdx < byCruBrdNum; ++byOldIdx )
	{
		bIsNewTblExists = FALSE;
		for( byNewIdx = 0;byNewIdx < byBrdNum; ++byNewIdx )
		{
			if( atCurBrdInfo[byOldIdx].IsEqual(ptNewBrdCfginfo[byNewIdx]) )
			{
				bIsNewTblExists = TRUE;
				break;
			}
		}
		
		if( bIsNewTblExists )
		{
			//�ϱ��У��±��У����ϱ�λ�÷�
			memcpy( &ptAdjustedBrdCfgInfo[byOldIdx],&atCurBrdInfo[byOldIdx],sizeof(ptAdjustedBrdCfgInfo[byOldIdx]) );
		}
		else
		{
			//�ϱ��У��±�û�����ϱ�λ�����
			memset( &ptAdjustedBrdCfgInfo[byOldIdx],0,sizeof(ptAdjustedBrdCfgInfo[byOldIdx]) );
		}
	}
	
	
	BOOL32 bIsOldTblExists = FALSE;
	for( byNewIdx = 0;byNewIdx < byBrdNum; ++byNewIdx )
	{
		bIsOldTblExists = FALSE;
		for( byOldIdx = 0;byOldIdx < byCruBrdNum; ++byOldIdx )
		{
			if( atCurBrdInfo[byOldIdx].IsEqual(ptNewBrdCfginfo[byNewIdx]) )
			{
				bIsOldTblExists = TRUE;
				break;
			}
		}
		
		if( !bIsOldTblExists )
		{
			//�±����ϱ�û���ҿ�λ�÷ţ���¼Index
			u8 byIdx = 0;
			while( byIdx<MAXNUM_BOARD )
			{
				if( ptAdjustedBrdCfgInfo[byIdx].GetBrdId() == 0 )
				{
					memcpy( &ptAdjustedBrdCfgInfo[byIdx],&ptNewBrdCfginfo[byNewIdx],sizeof(ptAdjustedBrdCfgInfo[byOldIdx]) );
					abyIsWrite[byIdx/8] |= 1 << ( byIdx % 8 );
					break;
				}
				++byIdx;
			}
		}
	}
	for( byNewIdx = 0;byNewIdx < byBrdNum; ++byNewIdx )
	{
		if( ptAdjustedBrdCfgInfo[byNewIdx].GetBrdId() == 0 && byBrdNum != byNewIdx )
		{
			u8 byIdx = MAXNUM_BOARD - 1;
			while( byIdx > 0 )
			{
				if( ptAdjustedBrdCfgInfo[byIdx].GetBrdId() != 0 )
				{
					memcpy( &ptAdjustedBrdCfgInfo[byNewIdx],&ptAdjustedBrdCfgInfo[byIdx],sizeof(ptAdjustedBrdCfgInfo[byNewIdx]) );
					memset( &ptAdjustedBrdCfgInfo[byIdx],0,sizeof(ptAdjustedBrdCfgInfo[byIdx]) );	
					abyIsWrite[byNewIdx/8] |= 1 << ( byNewIdx % 8 );
					break;
				}
				--byIdx;
			}
		}
	}
	
	//û�м�¼Indexλ�õ�,���
	for( byNewIdx = 0;byNewIdx < byBrdNum; ++byNewIdx )
	{
		if( (abyIsWrite[byNewIdx/8] & (1 << ( byNewIdx % 8 ))) == 0 )
		{
			memset( &ptAdjustedBrdCfgInfo[byNewIdx],0,sizeof(ptAdjustedBrdCfgInfo[byNewIdx]) );
		}
	}
}
*/
// END OF FILE
