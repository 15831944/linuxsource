/*****************************************************************************
   ģ����      : Mcu Agent
   �ļ���      : AgetnMsgCenter.cpp
   ����ļ�    : AgeentMsgCenter.h
   �ļ�ʵ�ֹ���: ��Ϣ����ת
   ����        : liuhuiyun
   �汾        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���          �޸�����
   2005/08/17  1.0         liuhuiyun       ����
   2006/04/30  4.0		   liuhuiyun       �Ż�
   2006/11/02  4.0         �ű���          ���ݶ�ȡ�㷨�Ż�
******************************************************************************/

// Last value assigned to variable not used)
/*lint -save -e438*/
/*lint -save -e529*/		// Symbol not subsequently referenced
/*lint -save -e550*/		// Symbol not accessed)
/*lint -save -e578*/		// Declaration of symbol hides symbol 

#include <string.h>
#include <stdlib.h>
//#include <time.h>

#include "osp.h"
#include "agentcommon.h"
#include "agentmsgcenter.h"
#include "configureagent.h"

#ifdef _VXWORKS_
    #include <semLib.h>
    #include <taskLib.h>
    #include <inetlib.h>
    #define IS_DSL8000_BRD_MPC
#endif

CMsgCenterApp		    g_cMsgProcApp;
extern  SEMHANDLE       g_hSync;	            // ȫ��ͬ���ź�����������֮��ͬ������
BOOL32                  g_bNmsOnline = FALSE;   // ȫ�ֱ���������MPC��NMS��״̬���� mqs add 20101118
BOOL32                  g_bNmsLedState = FALSE; // ȫ�ֱ��������ڱ��MPC��NMS��״̬ mqs add 20101118

CMsgCenterInst::CMsgCenterInst()
{
}

CMsgCenterInst::~CMsgCenterInst()
{
    Quit();
}

/*=============================================================================
  �� �� ���� AgentCallBack
  ��    �ܣ� Snmp�ص�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwNodeName
             u8 byRWFlag
             void * pBuf
             u16* pwBufLen
  �� �� ֵ�� u16 
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2006/07/28  4.0         �ű���      �Ż�����
=============================================================================*/
u16 AgentCallBack(u32 dwNodeName, u8 byRWFlag, void * pBuf, u16* pwBufLen)
{
    u32    dwNodeValue = GET_NODENAME(dwNodeName);
    BOOL32 bRead = ( READ_FLAG == byRWFlag ) ? TRUE : FALSE;
    u16    wResult = SNMP_SUCCESS;
   
    switch( dwNodeValue )
    {
    //Mcu Sys - system
    case NODE_MCUSYSSTATE:
    case NODE_MCUSYSTIME:
    case NODE_MCUSYSFTPFILENAME:    
    case NODE_MCUSYSCONFIGVERSION:
    case NODE_MCUSYSSOFTWAREVERSION:
    case NODE_MCUSYSFTPUSER:
    case NODE_MCUSYSFTPPASSWORD:
    case NODE_MCUSYSCONFIGERRORSTRING:
    case NODE_MCUSYSLAYER:
    case NODE_MCUSYSSLOT:
    case NODE_MCUSYSMGTSTATE:
    case NODE_MCUSYSMGTUPDATESOFTWARE:
    case NODE_MCUSYSMGTBITERRORTEST:
/*  case NODE_MCUSYSMGTTIMESYNC:*/  // [20120806 liaokang] ����ͨ��NODE_MCUSYSTIMEֱ��ͬ�����е���ʱ��
    case NODE_MCUSYSMGTSELFTEST:
    case NODE_MCUSYSOSTYPE:
    case NODE_MCUSYSSUBTYPE:
	case NODE_MCUSYSSELFUPDATESOFTWARE:
	case NODE_MCUCOMPILETIME:
        wResult = ProcCallBackMcuSystem(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    //Mcu Sys - pfm Alarm
    case NODE_MCUPFMALARMSTAMP:
    case NODE_MCUPFMALARMSERIALNO:
    case NODE_MCUPFMALARMALARMCODE:
    case NODE_MCUPFMALARMOBJTYPE:
    case NODE_MCUPFMALARMOBJECT:
    case NODE_MCUPFMALARMTIME:
        wResult = ProcCallBackMcuPfmAlarm(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    //Mcu Sys - notifications
    case NODE_MCUCONFSTATENOTIFY:
    case NODE_MCUCONFIDNOTIFY:
    case NODE_MCUCONFNAMENOTIFY:
    case NODE_MCUCONFSTARTTIMENOTIFY:
    case NODE_MTSTATENOTIFY:
    case NODE_MTALIASNOTIFY:
    case NODE_MTJOINTIMENOTIFY:
        wResult = ProcCallBackMcuNotifications(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    //Mcu Sys - Local info
    case NODE_MCUID:
    case NODE_MCUALIAS:
    case NODE_MCUE164NUMBER:
	case NODE_MCULOCALINFOS:
	//����Case���SetLocalInfo������Ӧ�޸�[2/21/2013 chendaiwei]
        wResult = ProcCallBackMcuLocal(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    //Mcu Net - Net
	case NODE_MCUNETPORTKIND:
	case NODE_MCUNETTEMPPORTKIND:
	case NODE_MCUNETIPADDR:
	case NODE_MCUNETIPMASK:
	case NODE_MCUNETGATEWAY:		
    case NODE_MCUNETGKIPADDR:
    case NODE_MCUNETMULTICASTIPADDR:
    case NODE_MCUNETMULTICASTPORT:         
    case NODE_MCUNETSTARTRECVPORT: 
    case NODE_MCUNET225245STARTPORT:   
    case NODE_MCUNET225245MAXMTNUM:
    case NODE_MCUNETUSEMPCTRANSDATA:
    case NODE_MCUNETUSEMPCSTACK:
	case NODE_MCUNETMAC:
	case NODE_MCUNETGKUSED:
	//����Case���SetNetWorkInfo������Ӧ�޸�[2/21/2013 chendaiwei]
        wResult = ProcCallBackMcuNet(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    //Mcu Net - QOS
    case NODE_MCUNETQOSTYPE:
    case NODE_MCUNETQOSIPSERVICETYPE: 
    case NODE_MCUNETQOSAUDIOLEVEL: 
    case NODE_MCUNETQOSVIDEOLEVEL:
    case NODE_MCUNETQOSDATALEVEL: 
    case NODE_MCUNETQOSSIGNALLEVEL:
        wResult = ProcCallBackMcuNetQos(dwNodeName, bRead, pBuf, pwBufLen);
        break;
    
    //Mcu Net - brd snmp table
    case NODE_MCUNETSNMPCFGIPADDR:
    case NODE_MCUNETSNMPCFGREADCOMMUNITY: 
    case NODE_MCUNETSNMPCFGWRITECOMMUNITY:
    case NODE_MCUNETSNMPCFGGETSETPORT:
    case NODE_MCUNETSNMPCFGTRAPPORT:
    case NODE_MCUNETSNMPCFGEXIST:
        wResult = ProcCallBackMcuNetSnmpCfg(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    //Mcu Net - brd cfg table
    case NODE_MCUNETBRDCFGID:
    case NODE_MCUNETBRDCFGLAYER: 
    case NODE_MCUNETBRDCFGSLOT: 
    case NODE_MCUNETBRDCFGTYPE:
    case NODE_MCUNETBRDCFGSTATUS:
    case NODE_MCUNETBRDCFGVERSION:
    case NODE_MCUNETBRDCFGIPADDR:
    case NODE_MCUNETBRDCFGMODULE:
    case NODE_MCUNETBRDCFGPANELLED:
    case NODE_MCUNETBRDCFGOSTYPE:
        wResult = ProcCallBackMcuNetBoardCfg(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    // Mcu Eqp - mixer table
    case NODE_MCUEQPMIXERID: 
    case NODE_MCUEQPMIXERPORT:
    case NODE_MCUEQPMIXERSWITCHBRDID:
    case NODE_MCUEQPMIXERALIAS:
    case NODE_MCUEQPMIXERRUNNINGBRDID:
    case NODE_MCUEQPMIXERIPADDR:
    case NODE_MCUEQPMIXERMIXSTARTPORT:
    case NODE_MCUEQPMIXERMAXMIXGROUPNUM:  
    case NODE_MCUEQPMIXERCONNSTATE:
    case NODE_MCUEQPMIXEREXIST: 
        wResult = ProcCallBackMcuEqpMixer(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    //Mcu Eqp - recorder table
    case NODE_MCUEQPRECORDERID: 
    case NODE_MCUEQPRECORDERSWITCHBRDID:
    case NODE_MCUEQPRECORDERPORT:
    case NODE_MCUEQPRECORDERALIAS:
    case NODE_MCUEQPRECORDERIPADDR:
    case NODE_MCUEQPRECORDERRECVSTARTPORT: 
    case NODE_MCUEQPRECORDERCONNSTATE:
    case NODE_MCUEQPRECORDEREXIST:
        wResult = ProcCallBackMcuEqpRecorder(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    //Mcu Eqp - TVWall table
    case NODE_MCUEQPTVWALLID:
    case NODE_MCUEQPTVWALLALIAS: 
    case NODE_MCUEQPTVWALLRUNNINGBRDID: 
    case NODE_MCUEQPTVWALLIPADDR:  
    case NODE_MCUEQPTVWALLVIDEOSTARTRECVPORT:  
    case NODE_MCUEQPTVWALLCONNSTATE: 
    case NODE_MCUEQPTVWALLEXIST:
        wResult = ProcCallBackMcuEqpTVWall(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    //Mcu Eqp - Bas table
    case NODE_MCUEQPBASID:
    case NODE_MCUEQPBASPORT:
    case NODE_MCUEQPBASSWITCHBRDID:
    case NODE_MCUEQPBASALIAS:
    case NODE_MCUEQPBASRUNNINGBRDID:
    case NODE_MCUEQPBASIPADDR:
    case NODE_MCUEQPBASSTARTRECVPORT :
    case NODE_MCUEQPBASCONNSTATE:
    case NODE_MCUEQPBASEXIST:
        wResult = ProcCallBackMcuEqpBas(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    //Mcu Eqp - Vmp table
    case NODE_MCUEQPVMPID :
    case NODE_MCUEQPVMPPORT :
    case NODE_MCUEQPVMPSWITCHBRDID :
    case NODE_MCUEQPVMPALIAS:
    case NODE_MCUEQPVMPRUNNINGBRDID :
    case NODE_MCUEQPVMPIPADDR :
    case NODE_MCUEQPVMPSTARTRECVPORT : 
    case NODE_MCUEQPVMPENCODERNUM: 
        wResult = ProcCallBackMcuEqpVMP(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    //Mcu Eqp - DCS Stub
    case NODE_MCUEQPDCSIPADDR :  
    case NODE_MCUEQPDCSCONNSTATE:
        wResult = ProcCallBackMcuEqpDcs(dwNodeName, bRead, pBuf, pwBufLen);
       break;

    //Mcu Eqp - PRS Stub
    case NODE_MCUEQPPRSID:   
    case NODE_MCUEQPPRSMCUSTARTPORT: 
    case NODE_MCUEQPPRSSWITCHBRDID:
    case NODE_MCUEQPPRSALIAS:
    case NODE_MCUEQPPRSRUNNINGBRDID:
    case NODE_MCUEQPPRSIPADDR:       
    case NODE_MCUEQPPRSSTARTPORT: 
    case NODE_MCUEQPPRSFIRSTTIMESPAN: 
    case NODE_MCUEQPPRSSECONDTIMESPAN: 
    case NODE_MCUEQPPRSTHIRDTIMESPAN:
    case NODE_MCUEQPPRSREJECTTIMESPAN:
        wResult = ProcCallBackMcuEqpPrs(dwNodeName, bRead, pBuf, pwBufLen);
        break;
      
    //Mcu Eqp - Net Sync Stub
    case NODE_MCUEQPNETSYNCMODE : 
    case NODE_MCUEQPNETSYNCDTSLOT:  
    case NODE_MCUEQPNETSYNCE1INDEX :  
    case NODE_MCUEQPNETSYNCSTATE:
        wResult = ProcCallBackMcuEqpNetSync(dwNodeName, bRead, pBuf, pwBufLen);
       break;

	//Mcu Performance Info
	case NODE_MCUPFMCPURATE:
	case NODE_MCUPFMMEMORY:
	case NODE_MCUPFMMTNUM:
	case NODE_MCUPFMCONFNUM:
	case NODE_MCUPFMEQPUSEDRATE:
	case NODE_MCUPFMAUTHRATE:
	case NODE_MCUPFMMACHTEMPER:
    case NODE_MCUPFMMCSCONNECTIONS:           // ���� ����MCS��IP��Ϣ  [04/24/2012 liaokang] 
        wResult = ProcCallBackMcuPfmInfo(dwNodeName, bRead, pBuf, pwBufLen);
       break;

    default:
        Agtlog(LOG_ERROR, "[AgentCallBack] unexpected node(0x%x) received for: %d !\n", dwNodeValue, bRead);
        wResult = SNMP_GENERAL_ERROR;
        break;
    }
    return wResult;
}

/*=============================================================================
  �� �� ���� ProcCallBackMcuSystem
  ��    �ܣ� ����MCUϵͳ״̬�ص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwNodeName:    ����ڵ���
             BOOL32  bRead:     ��дģʽ 0-write��1-read
             void * pBuf:       �����������
             u16 *pwBufLen:     ���ݳ���
  �� �� ֵ�� u16 �ɹ� SNMP_SUCCESS
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2006/07/28  4.0         �ű���       ����
=============================================================================*/
u16 ProcCallBackMcuSystem(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    //u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
    u16 wResult = SNMP_SUCCESS;
    BOOL32 bSupportOpr = TRUE;
    
    TMcuSystem tMcuSysInfo;
    g_cCfgParse.GetSystemInfo( &tMcuSysInfo );
    
    switch(dwNodeValue)
    {
    case NODE_MCUSYSSTATE:

        if( bRead )
        {
            *pwBufLen = (u16)sizeof(s32);
            *(s32*)pBuf = (s32)tMcuSysInfo.GetState();
        }
        else
        {
            u32 dwOpt = *(u32*)pBuf;
            tMcuSysInfo.SetState( (u8)dwOpt );
        }
        break;

    case NODE_MCUSYSTIME:

        if( bRead )
        {
			u16 wTimeLen = strlen(tMcuSysInfo.GetTime());
            *pwBufLen = min( *pwBufLen, wTimeLen );
            strncpy( (s8*)pBuf, tMcuSysInfo.GetTime(), *pwBufLen );
        }
        else
        {
            //ͬ������ʱ�����ʵʱ�������: ������ͨ���ṹ����ᵼ��ϵͳʱ�������[12/12/2006-zbq]
            g_cCfgParse.SyncSystemTime( (const s8*)pBuf );
        }
        break;

    case NODE_MCUSYSFTPFILENAME:
        
        bSupportOpr = FALSE;
        break;
        
    case NODE_MCUSYSCONFIGVERSION:
        
        if ( bRead )
        {
			u16 wCfgVerLen = strlen(tMcuSysInfo.GetConfigVer()) ;
            *pwBufLen = min( *pwBufLen, wCfgVerLen );
            strncpy( (s8*)pBuf, tMcuSysInfo.GetConfigVer(), *pwBufLen ); 

			g_bNmsOnline = TRUE;                                 // miaoqingsong  20101118 add ��MPC��NMS�� 
        }
        else
        {
            tMcuSysInfo.SetConfigVer( (s8*)pBuf );
        }
        break;

    case NODE_MCUSYSSOFTWAREVERSION:

        if ( bRead )
        {
			// [miaoqingsong 20111122] ������ܻ�ȡ����汾�Žӿ�GetMcuSoftVersion()��ȡ����汾��������MCS����ͳһ
			u16 wSoftVerLen = strlen(GetMcuSoftVersion());
			*pwBufLen = min( *pwBufLen, wSoftVerLen );
			strncpy( (s8*)pBuf, GetMcuSoftVersion(), *pwBufLen);

//             memcpy((s8*)pBuf, VER_MCU, strlen(VER_MCU));
//             *pwBufLen = strlen(VER_MCU); 
			g_bNmsOnline = TRUE;                                 // miaoqingsong  20101118 add ��MPC��NMS��
			Agtlog( LOG_INFORM, "[ProcCallBackMcuSystem] NODE_MCUSYSSOFTWAREVERSION <%s>!\n", GetMcuSoftVersion() );  
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;
        
    case NODE_MCUSYSFTPUSER:

        if( bRead )
        {
            memcpy((s8*)pBuf, "admin", sizeof("admin"));
            *pwBufLen = sizeof("admin");
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUSYSFTPPASSWORD:

        if( bRead )
        {
            memcpy((s8*)pBuf, "admin", sizeof("admin"));
            *pwBufLen = sizeof("admin");
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;
        
    case NODE_MCUSYSCONFIGERRORSTRING:

        if ( bRead )
        {
            g_cAlarmProc.GetErrorString((s8*)pBuf, *pwBufLen);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUSYSLAYER:

        if( bRead )
        {
            *(s32*)pBuf = (s32)g_cCfgParse.GetMcuLayerCount();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUSYSSLOT:

        if( bRead )
        {
            *(s32*)pBuf = (s32)MCU_SLOT_NUM;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUSYSMGTSTATE:

        if( bRead )
        {
            *(s32*)pBuf = tMcuSysInfo.GetState();
            *pwBufLen = sizeof(s32);
        }
        else
        {
			TBrdPosition  tBrdPos;
            tBrdPos.byBrdLayer = *(u8*)pBuf;
            tBrdPos.byBrdSlot  = *((u8*)pBuf+1);
			// [20110316 miaoqingsong modify] ������ܲൽMCU�൥������ת��
            tBrdPos.byBrdID    = g_cCfgParse.GetMcuBrdTypeFromSnmp(*((u8*)pBuf+2));
            u8 byOpt           = *((u8*)pBuf+3);
            if( RESET_BRD == byOpt )
            {
                g_cCfgParse.SnmpRebootBoard(tBrdPos);
                Agtlog(LOG_INFORM, "[ProcCallBackMcuSystem] <%d,%d:%s> reboot \n", 
                                    tBrdPos.byBrdLayer, tBrdPos.byBrdSlot, 
                                    g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID) );       
            }
        }
        break;

    case NODE_MCUSYSMGTUPDATESOFTWARE:
        
        if ( bRead )
        {
            wResult = McuMgtGetUpdateFiles( pBuf, pwBufLen );
        }
        else
        {
            wResult = McuMgtSetUpdateFiles( pBuf, pwBufLen );
        }
        break;

		//File����Ϊϵͳ�����ļ��������������ļ�
	case NODE_MCUSYSSELFUPDATESOFTWARE:
		{
			if ( bRead )
			{
				wResult = McuSmGetUpdateFiles( pBuf, pwBufLen );
			}
			else
			{
				wResult = McuSmSetUpdateFiles( pBuf, pwBufLen );
			}
		}
        break;

    case NODE_MCUSYSMGTBITERRORTEST:
        
        //���ڵ��ݲ����ֶ�д
        {
            TBrdPosition  tBrdPos;
            tBrdPos.byBrdLayer = *(u8*)pBuf;
            tBrdPos.byBrdSlot  = *((u8*)pBuf+1);
            tBrdPos.byBrdID    = *((u8*)pBuf+2);
            u8 byOpt = *((u8*)pBuf+3);
            if( NMS_TEST_START == byOpt)
            {
                g_cCfgParse.BitErrorTest( tBrdPos );
            }
        }
        break;

        // [20120806 liaokang] ����ͨ��NODE_MCUSYSTIMEֱ��ͬ�����е���ʱ��
//     case NODE_MCUSYSMGTTIMESYNC:
// 
//         if ( bRead )
//         {
//             bSupportOpr = FALSE;
//         }
//         else
//         {
//             g_cCfgParse.SyncBoardTime((u8*)pBuf);
//         }
//         break;

    case NODE_MCUSYSMGTSELFTEST:
        
        //���ڵ��ݲ����ֶ�д
        {
            TBrdPosition  tBrdPos;
            tBrdPos.byBrdLayer = *(u8*)pBuf;
            tBrdPos.byBrdSlot = *((u8*)pBuf+1);
            tBrdPos.byBrdID = *((u8*)pBuf+2);
            g_cCfgParse.BoardSelfTest( tBrdPos );
        }
        break;

    case NODE_MCUSYSOSTYPE:
        
        if ( bRead )
        {
        #ifdef WIN32
            *(s32*)pBuf = SNMP_OS_TYPE_WIN32;
        #elif defined _VXWORKS_
            *(s32*)pBuf = SNMP_OS_TYPE_VXWORKS;
		#elif defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
            *(s32*)pBuf = SNMP_OS_TYPE_LINUX_RH;
        #else   /*_LINUX_*/
            *(s32*)pBuf = SNMP_OS_TYPE_LINUX;
        #endif
            *pwBufLen = sizeof(s32);
			g_bNmsOnline = TRUE;                                       // miaoqingsong  20101118 add ��MPC��NMS��
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUSYSSUBTYPE:

        if ( bRead )
        {
            u8 byBrdId = 0;
            u8 byMcuSubType = 0;
        
        #ifndef WIN32
			#if defined(_8KE_) 
				byBrdId = SNMP_MCUSUBTYPE_8000G;
			#elif defined(_8KH_)
				if(g_cCfgParse.Is800LMcu())
				{
					byBrdId = SNMP_MCUSUBTYPE_800L;
				}
				else if(g_cCfgParse.Is8000HmMcu())
				{
					byBrdId = SNMP_MCUSUBTYPE_8000H_M;
				}
				else
				{
					byBrdId = SNMP_MCUSUBTYPE_8000H;
				}
			#elif defined(_8KI_)
				byBrdId = SNMP_MCUSUBTYPE_8000I;
			#else
				byBrdId = BrdGetBoardID();
			#endif			
        #else
            byBrdId = BRD_TYPE_MPC/*DSL8000_BRD_MPC*/;
        #endif

            byMcuSubType = g_cCfgParse.McuSubTypeAgt2Snmp(byBrdId);

            *(s32*)pBuf = byMcuSubType;
            *pwBufLen = sizeof(s32); 
			g_bNmsOnline = TRUE;                                   // miaoqingsong  20101118 add ��MPC��NMS��
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

	case NODE_MCUCOMPILETIME:
		{
			if ( bRead )
			{
				s8* pCompileTime = g_cCfgParse.GetMcuCompileTime(*pwBufLen);
				memcpy(pBuf, pCompileTime, *pwBufLen);
			}
			else
			{
				bSupportOpr = FALSE;
			}
		}
		break;
    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_VERBOSE, "[ProcCallBackMcuSystem] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }

    // NOTE: ͨ�� NODE_MCUSYSMGTTIMESYNC �� NODE_MCUSYSTIME �� MPC�����������
    //       ����ʱ�����ÿ��Ի�����ܴ��������ʱ�䡣���˴���tMcuSysInfo������
    //       ��ʱ���ǻ�ȡ�ýṹʱ�� MPC ʱ�䣬�˴����������ã�����ͬ��������
    //       ����ϵͳʱ�䣬���ң������ͬ��MPC�����MCU������� 
    //       NODE_MCUSYSMGTTIMESYNC �� NODE_MCUSYSTIME �Ĵ�����������ʱ�䡣[11/27/2006-zbq]
    if ( !bRead && NODE_MCUSYSMGTTIMESYNC != dwNodeValue && NODE_MCUSYSTIME != dwNodeValue )
    {
        wResult = g_cCfgParse.SetSystemInfo( &tMcuSysInfo );
    }
    if ( !bSupportOpr )
    {
        Agtlog(LOG_WARN, "[CallBackMcuSys] node(0x%x) not support operate for: %d<1.read-0.write> \n", dwNodeValue, bRead);
    }
    return wResult;
}

/*=============================================================================
  �� �� ���� McuMgtSetUpdateFiles
  ��    �ܣ� ����ĳ�����MPC���ĳһ����ĳ�����ļ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void * pBuf:       �����������
             u16 *pwBufLen:     ���ݳ���
  �� �� ֵ�� u16 �ɹ� SNMP_SUCCESS
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2006/09/20  4.0         �ű���       ����
=============================================================================*/
u16 McuMgtSetUpdateFiles( void *pBuf, u16 *pwBufLen )
{
    u16 wResult = SNMP_SUCCESS;
    
    u8 byNum = 0;
    u16 wLen = 0;
    TBrdPosition tBrdPos;
    u8 abyMsgBody[256];
    memset( abyMsgBody, 0, sizeof(abyMsgBody) );

    wLen = *pwBufLen;

    //u8 byUpdateType    = *(u8*)pBuf - '0';      // 0-MCU,1-����(��MPC), Ŀǰ���ֶκ�Ϊ1
    tBrdPos.byBrdLayer = *((u8*)pBuf+1) - '0';  // ��������
    tBrdPos.byBrdSlot  = *((u8*)pBuf+2) - '0';  // ������ۺ�
    tBrdPos.byBrdID    = g_cCfgParse.GetMcuBrdTypeFromSnmp(*((u8*)pBuf+3) - '0');  // ����������
    byNum              = *((u8*)pBuf+4);        // �����ļ���
    wLen = wLen - 5;

    Agtlog(LOG_INFORM, "[McuMgtSetUpdateFiles] Brd<%d,%d:%s> update file len: %d.\n",
                        tBrdPos.byBrdLayer, tBrdPos.byBrdSlot, 
                        g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID), wLen );    
    
    u8 *pbyMsgBody = abyMsgBody;
    memcpy( pbyMsgBody, &tBrdPos, sizeof(tBrdPos));
    pbyMsgBody += sizeof(tBrdPos);
    memcpy( pbyMsgBody, &byNum, sizeof(u8));
    pbyMsgBody += sizeof(u8);
    memcpy( pbyMsgBody, ((u8*)pBuf+5), wLen);
    
    OspPost( MAKEIID(AID_MCU_AGENT, 1), EV_MSGCENTER_NMS_UPDATEDMPC_CMD,
                         abyMsgBody, wLen + sizeof(tBrdPos)+sizeof(u8));

    return wResult;
}

/*=============================================================================
  �� �� ���� McuMgtSetUpdateFiles
  ��    �ܣ� ����ĳ�����MPC���ĳһ����ĳ�����ļ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void * pBuf:       �����������
             u16 *pwBufLen:     ���ݳ���
  �� �� ֵ�� u16 �ɹ� SNMP_SUCCESS
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2006/09/20  4.0         �ű���       ����
=============================================================================*/
u16 McuSmSetUpdateFiles( void *pBuf, u16 *pwBufLen )
{
    u16 wResult = SNMP_SUCCESS;

	TMcuUpdateInfo* ptUpdateInfo = g_cCfgParse.GetMcuUpdateInfo();

	Agtlog(LOG_VERBOSE, "McuSmSetUpdateFiles: pwBufLen.%d\n",pwBufLen!=NULL?*pwBufLen:0);

	if (ptUpdateInfo == NULL)
	{
		return SNMP_GENERAL_ERROR;
	}

	if (ptUpdateInfo->GetUpdateState() != SNMP_FILE_UPDATE_INIT)
	{
		return SNMP_GENERAL_ERROR;
	}

	ptUpdateInfo->Clear();

	ptUpdateInfo->SetUpdateState(SNMP_FILE_UPDATE_INPROCESS);

	u8 byUpdateNum = 0;
	u8 byLayerID = 0;
	u8 bySlotID = 0;
	u8 byType = 0;

	u8 *pszBuff = (u8 *)pBuf;

	byUpdateNum = *(u8 *)(pszBuff + 0) ;//�ϴ��ļ���
	byLayerID = *(u8 *)(pszBuff + 1); //��
	bySlotID = *(u8 *)(pszBuff + 2);//��
	byType = *(u8 *)(pszBuff + 3);//��������

	pszBuff += sizeof(u32);

	//  [1/26/2011 chendaiwei]֧��MPC2
	if ((byUpdateNum == 0) || (byType != BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ && byType !=BRD_TYPE_MPC2 ))
	{
		ptUpdateInfo->SetUpdateState(SNMP_FILE_UPDATE_INIT);
		return SNMP_GENERAL_ERROR;
	}

	ptUpdateInfo->SetBrdPosition(byLayerID, bySlotID, byType);

	u32 dwSvrIp = 0; //ftp server Ip
	u16 wSrvPort = 0;//ftp server port
	s8  achVerServUsername[MAXLEN_PWD + 4] = {0};    //�汾��������½�û���
	s8  achVerServPassword[MAXLEN_PWD + 4] = {0};     //�汾��������½����
	s8	aszFilePath[KDV_MAX_PATH] = {0};

	u8 byLen = 0;

	TUpdateFileInfo tUpdateFileInfo;

	u8 byIndex = 0;
	for (byIndex = 0; byIndex < byUpdateNum; ++byIndex)
	{
		memset(&tUpdateFileInfo, 0, sizeof(tUpdateFileInfo));

		dwSvrIp = *(u32 *)pszBuff;
		pszBuff += sizeof(u32);

		wSrvPort = *(u16 *)pszBuff;
		pszBuff += sizeof(u16);

		byLen = *(u8 *)pszBuff;
		pszBuff += sizeof(u8);
		memcpy(achVerServPassword, pszBuff, byLen);
		achVerServPassword[byLen] = '\0';
		pszBuff += byLen;

		byLen = *(u8 *)pszBuff;
		pszBuff += sizeof(u8);
		memcpy(achVerServUsername, pszBuff, byLen);
		achVerServUsername[byLen] = '\0';
		pszBuff += byLen;

		byLen = *(u8 *)pszBuff;
		pszBuff += sizeof(u8);
		memcpy(aszFilePath, pszBuff, byLen);
		aszFilePath[byLen] = '\0';
		pszBuff += byLen;

		Agtlog(LOG_VERBOSE, "dwSvrIp: 0x%x, wSrvPort: %d\n", dwSvrIp, wSrvPort);

		Agtlog(LOG_VERBOSE, "achVerServUsername %s, achVerServPassword: %s\n", achVerServUsername, achVerServPassword);

		Agtlog(LOG_VERBOSE, "aszFilePath %s, Len: %d\n", aszFilePath, byLen);

		tUpdateFileInfo.SetServ(dwSvrIp, wSrvPort);
		tUpdateFileInfo.setSvrUser(achVerServUsername, achVerServPassword);
		tUpdateFileInfo.SetSvrFilePath(aszFilePath);
		tUpdateFileInfo.ChgUpdateState(FTPC_TRANSPERCENT);
		ptUpdateInfo->AddUpdateInfo(tUpdateFileInfo);
	}

	Agtlog(LOG_VERBOSE, "ptUpdateInfo->FtpGet:\n");

	if (!ptUpdateInfo->FtpGet())
	{
		Agtlog(LOG_VERBOSE, "ptUpdateInfo->FtpGet: Fail\n");
		ptUpdateInfo->SetUpdateState(SNMP_FILE_UPDATE_FAILED);
		return SNMP_GENERAL_ERROR;
	}

    return wResult;
}

/*=============================================================================
  �� �� ���� McuMgtGetUpdateFiles
  ��    �ܣ� ��ȡ��ǰ�������ߵ��� �� MPC��� �����ļ�(Ŀǰ������)������״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void * pBuf:       �����������
             u16 *pwBufLen:     ���ݳ���
  �� �� ֵ�� u16 �ɹ� SNMP_SUCCESS
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2006/09/21  4.0         �ű���       ����
=============================================================================*/
u16 McuMgtGetUpdateFiles( void *pBuf, u16 *pwBufLen )
{
    u16 wResult = SNMP_SUCCESS;
    
    u8  abyBuf[4096];
    memset(abyBuf, 0, sizeof(abyBuf));
    u8 *pbyBuf = &abyBuf[0] ;
    u8  byBrdNum = (u8)g_cCfgParse.GetBoardNum();
    u8  byBrdNumOnlineOSLinux = 0;
    
    //1. ��һ���ֶοճ����������ߵ� linux ������(��MPC, ����Զ��MPC)
    pbyBuf += 1;
    Agtlog(LOG_INFORM, "[McuMgtGetUpdateFiles] total brd num: %d !\n", byBrdNum);

    //2. ���������尴 u8(��)+u8(��)+u8(����)+u8(�ļ���)+u8(�ļ�����)+s8[](�ļ���)+u8(����״̬) ... ˳��д��
    s8 achOSFileName[32] = {0};
    s8 achImageFileName[32] = {0};
	s8 achBinFileName[32] = {0};
    memset(achOSFileName, 0, sizeof(achOSFileName));
    memcpy(achOSFileName, "/ramdisk/update.linux", sizeof("/ramdisk/update.linux"));
    
    u8 byImageFileStatus = SNMP_FILE_UPDATE_INIT;
    u8 byOSFileStatus    = SNMP_FILE_UPDATE_INIT;
	u8 byBinFileStatus	 = SNMP_FILE_UPDATE_INIT;
    BOOL32 bOSLinux = FALSE;
#ifdef _LINUX_
    bOSLinux = TRUE;
#endif
    u8 byRow = 1;
    for( ; byRow <= byBrdNum; byRow ++ )
    {
        TEqpBrdCfgEntry tBrdInfo;
        TBrdPosition    tBrdPos;
        memset(achImageFileName, 0, sizeof(achImageFileName));
        
        if ( SUCCESS_AGENT != g_cCfgParse.GetBrdInfoByRow(byRow, &tBrdInfo) )
        {
            Agtlog(LOG_WARN, "[McuMgtGetUpdateFiles] Index.%d GetBrdCfg failed !\n", byRow);
            continue;
        }
        else
        {
            tBrdPos.byBrdLayer = tBrdInfo.GetLayer();
            tBrdPos.byBrdSlot  = tBrdInfo.GetSlot();
            tBrdPos.byBrdID    = tBrdInfo.GetType();            
        }

		//  [1/21/2011 chendaiwei]֧��MPC2
        TMPCInfo tMPCInfo;
        g_cCfgParse.GetMPCInfo( &tMPCInfo );
        if ( !( ( g_cBrdManagerApp.IsRegedBoard(tBrdPos) && OS_TYPE_LINUX == tBrdInfo.GetOSType() ) ||
                ( ( BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ == tBrdInfo.GetType() || BRD_TYPE_MPC2 == tBrdInfo.GetType()) && 
                  tMPCInfo.GetLocalLayer() == tBrdInfo.GetLayer() && 
                  tMPCInfo.GetLocalSlot()  == tBrdInfo.GetSlot()  && bOSLinux ) ) )
        {
            Agtlog(LOG_WARN, "[McuMgtGetUpdateFiles] BrdId.%d <%d,%d:%s>, os<%d> reg<%d> is not reged linux brd or local linux mpc!\n", 
                              tBrdInfo.GetBrdId(), tBrdInfo.GetLayer(), tBrdInfo.GetSlot(),
                              g_cCfgParse.GetBrdTypeStr(tBrdInfo.GetType()),
                              tBrdInfo.GetOSType(), g_cBrdManagerApp.IsRegedBoard(tBrdPos) );
            continue;
        }

		BOOL32 bRet = g_cCfgParse.GetImageFileName(tBrdInfo.GetType(), achImageFileName);

		bRet |= g_cCfgParse.GetBinFileName(tBrdInfo.GetType(), achBinFileName);

		if (!bRet)
		{
            Agtlog(LOG_WARN, "[McuMgtGetUpdateFiles] BrdId.%d <%d,%d:%s> get image name failed!\n", 
				tBrdInfo.GetBrdId(), tBrdInfo.GetLayer(), tBrdInfo.GetSlot(),
				g_cCfgParse.GetBrdTypeStr(tBrdInfo.GetType()) );
            continue;
		}

        byImageFileStatus = g_cBrdManagerApp.GetImageFileStatus(tBrdPos) + '0';
        byOSFileStatus    = g_cBrdManagerApp.GetOSFileStatus(tBrdPos) + '0';
		byBinFileStatus	  = g_cBrdManagerApp.GetBinFileStatus(tBrdPos) + '0';
        if ( 255 == byImageFileStatus || 255 == byOSFileStatus || 255 == byBinFileStatus)
        {
            Agtlog(LOG_WARN, "[McuMgtGetUpdateFiles] BrdId.%d <%d,%d:%s> get status <I.%d,O.%d>failed!\n", 
                              tBrdInfo.GetBrdId(), tBrdInfo.GetLayer(), tBrdInfo.GetSlot(),
                              g_cCfgParse.GetBrdTypeStr(tBrdInfo.GetType()),
                              byImageFileStatus - '0', byOSFileStatus - '0' );
            continue;
        }

        // ʵ�ʷ��͵ĵ�����
        byBrdNumOnlineOSLinux ++;

        //[2.1] Layer
        *pbyBuf = tBrdInfo.GetLayer() + '0';
        pbyBuf += 1;

        //[2.2] Slot
        *pbyBuf = tBrdInfo.GetSlot() + '0';
        pbyBuf += 1;

        //[2.3] Type
        *pbyBuf = g_cCfgParse.GetSnmpBrdTypeFromMcu(tBrdInfo.GetType()) + '0';
        pbyBuf += 1;

        //[2.4] �ļ���
        *pbyBuf = 3;    // Ŀǰֻ�������ļ�Image, Linux, Bin
        pbyBuf += 1;

        //[2.5] �ļ�image������+�ļ���+����״̬
        *pbyBuf = strlen(achImageFileName);
        pbyBuf += 1;
        memcpy(pbyBuf, achImageFileName, sizeof(achImageFileName));
        pbyBuf += strlen(achImageFileName);
        *pbyBuf = byImageFileStatus;
        pbyBuf += 1;
        
        //[2.6] �ļ�os������+�ļ���+����״̬
        *pbyBuf = strlen(achOSFileName);
        pbyBuf += 1;
        memcpy(pbyBuf, achOSFileName, sizeof(achOSFileName));
        pbyBuf += strlen(achOSFileName);
        *pbyBuf = byOSFileStatus;
        pbyBuf += 1;
		
        //[2.6] �ļ�os������+�ļ���+����״̬
        *pbyBuf = strlen(achBinFileName);
        pbyBuf += 1;
        memcpy(pbyBuf, achBinFileName, sizeof(achBinFileName));
        pbyBuf += strlen(achBinFileName);
        *pbyBuf = byBinFileStatus;
        pbyBuf += 1;
        Agtlog(LOG_INFORM, "[McuMgtGetUpdateFiles] brd<%d,%d:%s> set %s<%d> %s<%d> and %s<%d> to buf !\n",
                            tBrdInfo.GetLayer(), tBrdInfo.GetSlot(), 
                            g_cCfgParse.GetBrdTypeStr(tBrdInfo.GetType()),
                            achImageFileName, byImageFileStatus-'0', 
							achOSFileName, byOSFileStatus-'0',
							achBinFileName, byBinFileStatus-'0'
							);
    }

    //3.���е����ѯ��ϣ�ʵ�ʷ��͵���ȷ��
    abyBuf[0] = byBrdNumOnlineOSLinux;
    
    //4.���ظ� snmp����
    *pwBufLen  = strlen((s8*)abyBuf);
    memcpy(pBuf, abyBuf, *pwBufLen);
    Agtlog(LOG_INFORM, "[McuMgtGetUpdateFiles] Buf<len:%d>, Brd Num: %d !\n", 
                                          *pwBufLen, byBrdNumOnlineOSLinux );
    return wResult;
}

/*=============================================================================
  �� �� ���� McuSmGetUpdateFiles
  ��    �ܣ� ��ȡ��ǰ�������ߵ��� �� MPC��� �����ļ�(Ŀǰ������)������״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void * pBuf:       �����������
             u16 *pwBufLen:     ���ݳ���
  �� �� ֵ�� u16 �ɹ� SNMP_SUCCESS
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
=============================================================================*/
u16 McuSmGetUpdateFiles( void *pBuf, u16 *pwBufLen )
{
    u16 wResult = SNMP_SUCCESS;

	u8  abyBuf[4096];
    memset(abyBuf, 0, sizeof(abyBuf));
    u8 *pbyBuf = &abyBuf[0] ;
    
	TMcuUpdateInfo tUpdateInfo;
	g_cCfgParse.GetMcuUpdateInfo(&tUpdateInfo);
	
	u8 byFileNum = tUpdateInfo.GetFileNum();
	u16 wBuffSize = 0;
	TUpdateFileInfo *ptUpdateFile = NULL;
	u8 byFileLen = 0;
	u8 byState = 0;

	u8 byUpdate = tUpdateInfo.GetUpdateState();

	Agtlog(LOG_INFORM, "McuSmGetUpdateFiles FileNum:%d totalState:%d\n", byFileNum, byUpdate);

	//u8 file num
	*(u8 *)pbyBuf = byFileNum;
	wBuffSize += sizeof(u8);
	pbyBuf += sizeof(u8);
	for (u8 byIndex = 0; byIndex < byFileNum; ++byIndex)
	{
		ptUpdateFile = tUpdateInfo.GetUpdateFile(byIndex);
		
		if (ptUpdateFile == NULL)
		{
			break;
		}
		
		*(u32 *)pbyBuf = ptUpdateFile->GetSvrIp();
		pbyBuf += sizeof(u32);
		wBuffSize += sizeof(u32);
		
		*(u16 *)pbyBuf = ptUpdateFile->GetSvrPort();
		pbyBuf += sizeof(u16);
		wBuffSize += sizeof(u16);
		
		byFileLen = strlen(ptUpdateFile->GetSvrFilePath());
		*(u8 *)pbyBuf = byFileLen;
		pbyBuf += sizeof(u8);
		wBuffSize += sizeof(u8);
		
		memcpy(pbyBuf, ptUpdateFile->GetSvrFilePath(), byFileLen);
		pbyBuf += byFileLen;
		wBuffSize += byFileLen;
		
		*(u8 *)pbyBuf = (u8)ptUpdateFile->GetTransPercent();
		pbyBuf += sizeof(u8);
		wBuffSize += sizeof(u8);
		
		byState = SNMP_FILE_UPDATE_INPROCESS;
		if ((byUpdate == SNMP_FILE_UPDATE_FAILED) || (byUpdate == SNMP_FILE_UPDATE_INIT))
		{
			byState = SNMP_FILE_UPDATE_FAILED;
		}
		else if (byUpdate == SNMP_FILE_UPDATE_SUCCEED)
		{
			byState = SNMP_FILE_UPDATE_SUCCEED;
		}
		else
		{
		}

		Agtlog(LOG_INFORM, "McuSmGetUpdateFiles FileIndex:%d FilePath:%s State:%d\n", byIndex, ptUpdateFile->GetSvrFilePath(), byState);
		
		*(u8 *)pbyBuf = byState;
		pbyBuf += sizeof(u8);
		wBuffSize += sizeof(u8);
		
		*(u8 *)pbyBuf = tUpdateInfo.GetLayerID();
		pbyBuf += sizeof(u8);
		wBuffSize += sizeof(u8);
		
		*(u8 *)pbyBuf = tUpdateInfo.GetSlotID();
		pbyBuf += sizeof(u8);
		wBuffSize += sizeof(u8);
		
		*(u8 *)pbyBuf = tUpdateInfo.GetType();
		pbyBuf += sizeof(u8);
		wBuffSize += sizeof(u8);
	}

	//4.���ظ� snmp����
    *pwBufLen  = wBuffSize;
    memcpy(pBuf, abyBuf, *pwBufLen);
    Agtlog(LOG_INFORM, "[McuSmGetUpdateFiles] Buf<len:%d>, File Num: %d !\n", 
                                          *pwBufLen, byFileNum );
    return wResult;
}

/*=============================================================================
  �� �� ���� ProcCallBackMcuPfmAlarm
  ��    �ܣ� ����MCUϵͳ�澯�ص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwNodeName:    ����ڵ���
             BOOL32  bRead:     ��дģʽ 1-read��0-write
             void * pBuf:       �����������
             u16 *pwBufLen:     ���ݳ���
  �� �� ֵ�� u16 �ɹ� SNMP_SUCCESS
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2006/07/28  4.0         �ű���       ����
=============================================================================*/
u16 ProcCallBackMcuPfmAlarm(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);

    u16 wResult = SNMP_SUCCESS;
    //BOOL32 bSupportOpr = FALSE;

    TMcupfmAlarmEntry tAlarmEntry;
    if ( NODE_MCUPFMALARMSTAMP != dwNodeValue ) 
    {
        BOOL32 bResult = g_cAlarmProc.GetAlarmTableIndex( byRow-1, tAlarmEntry );
        if ( !bResult )
        {
            Agtlog(LOG_VERBOSE, "[CallBackPfmAlarm] GetAlarmTableIndex.%d failed !\n", byRow );
            return SNMP_GENERAL_ERROR;
        }
    }
    Agtlog(LOG_VERBOSE, "[CallBackPfmAlarm] Node(0x%x) received for: %d!\n", dwNodeValue, bRead );

    switch(dwNodeValue)
    {
    case NODE_MCUPFMALARMSTAMP:
        
        if ( bRead )
        {
            u16 wTmp16Data = 0;
            g_cAlarmProc.GetAlarmStamp(wTmp16Data);
            *pwBufLen = sizeof(s32);
            *(s32*)pBuf = wTmp16Data;
        }
        break;

    case NODE_MCUPFMALARMSERIALNO:
        
        if ( bRead )
        {
            *pwBufLen = sizeof(s32);
            *(s32*)pBuf = tAlarmEntry.m_dwSerialNo;
        }
        break;

    case NODE_MCUPFMALARMALARMCODE:
        
        if ( bRead )
        {
            *pwBufLen = sizeof(s32);
            *(s32*)pBuf = tAlarmEntry.m_dwAlarmCode;
        }
        break;

    case NODE_MCUPFMALARMOBJTYPE:
        
        if ( bRead )
        {
            *pwBufLen = sizeof(s32);
            *(s32*)pBuf = tAlarmEntry.m_byObjType;
        }       
        break;

    case NODE_MCUPFMALARMOBJECT:
        
        if ( bRead )
        {
            *pwBufLen = sizeof(tAlarmEntry.m_achObject);
            memcpy((s8*)pBuf, tAlarmEntry.m_achObject, sizeof(tAlarmEntry.m_achObject));
        }
        break;

    case NODE_MCUPFMALARMTIME:
        
        if ( bRead )
        {
            *pwBufLen = sizeof(tAlarmEntry.m_achTime);
            memcpy((s8*)pBuf, tAlarmEntry.m_achTime, sizeof(tAlarmEntry.m_achTime));
        }
        break;
        
    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_VERBOSE, "[ProcCallBackMcuPfmAlarm] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }
    if ( !bRead )
    {
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_WARN, "[CallBackPfmAlarm] Node(0x%x) support no write!\n", dwNodeValue );        
    }
    return wResult;
}

/*=============================================================================
  �� �� ���� ProcCallBackMcuNotifications
  ��    �ܣ� ����MCUϵͳ֪ͨ��Ϣ�ص���Ŀǰû�ж�Ӧ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwNodeName:    ����ڵ���
             BOOL32  bRead:     ��дģʽ 0-write��1-read
             void * pBuf:       �����������
             u16 *pwBufLen:     ���ݳ���
  �� �� ֵ�� u16 �ɹ� SNMP_SUCCESS
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2006/07/28  4.0         �ű���       ����
=============================================================================*/
u16 ProcCallBackMcuNotifications(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
    u16 wResult = SNMP_SUCCESS;
    switch(dwNodeValue)
    {
    case NODE_MCUCONFSTATENOTIFY:
    case NODE_MCUCONFIDNOTIFY:
    case NODE_MCUCONFNAMENOTIFY:
    case NODE_MCUCONFSTARTTIMENOTIFY:
    case NODE_MTSTATENOTIFY:
    case NODE_MTALIASNOTIFY:
    case NODE_MTJOINTIMENOTIFY:
        Agtlog(LOG_WARN, "[ProcCallBackMcuNotifications] node(0x%x) didn't support operate!\n", dwNodeValue);
        break;

    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_VERBOSE, "[ProcCallBackMcuNotifications] unexpected node(0x%x) received ! bRead.%d pBuf.%p *pwBufLen.%d\n", dwNodeValue,bRead,pBuf,pwBufLen!=NULL?(*pwBufLen):0 );
        break;
    }
    return wResult;
}

/*=============================================================================
  �� �� ���� ProcCallBackMcuLocal
  ��    �ܣ� ����MCUϵͳ������Ϣ�ص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwNodeName:    ����ڵ���
             BOOL32  bRead:     ��дģʽ 0-write��1-read
             void * pBuf:       �����������
             u16 *pwBufLen:     ���ݳ���
  �� �� ֵ�� u16 �ɹ� SNMP_SUCCESS
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2006/07/28  4.0         �ű���       ����
=============================================================================*/
u16 ProcCallBackMcuLocal(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);

    TLocalInfo tLoaclInfo;
    g_cCfgParse.GetLocalInfo(&tLoaclInfo);
    Agtlog(LOG_VERBOSE, "[CallBackLocal] Node(0x%x) received for: %d!\n", dwNodeValue, bRead );

    u16 wResult = SNMP_SUCCESS;
    
    switch(dwNodeValue) 
    {
    case NODE_MCUID:
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tLoaclInfo.GetMcuId();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tLoaclInfo.SetMcuId( *(u8*)pBuf );
        }
        break;
        
    case NODE_MCUALIAS:
        {    
            //[5/16/2013 liaokang] ת��
            s8 achEncodingBuf[MAXLEN_ALIAS+1] = {0};
            u16  wLen = min(sizeof(achEncodingBuf)-1, *pwBufLen);
            if( bRead )
            {
                if( FALSE == TransEncodingForNmsData(tLoaclInfo.GetAlias(), achEncodingBuf, sizeof(achEncodingBuf)) )
                {
                    strncpy( achEncodingBuf, tLoaclInfo.GetAlias(), wLen);
                }
                *pwBufLen = strlen(achEncodingBuf)+1;
                *pwBufLen = min(*pwBufLen, wLen );
                memcpy( (s8*)pBuf, achEncodingBuf, *pwBufLen ); 
                g_bNmsOnline = TRUE;                                          // miaoqingsong 20101118 add ��MPC��NMS��
            }
            else
            {
                if( FALSE == TransEncodingForNmsData((s8*)pBuf, achEncodingBuf, sizeof(achEncodingBuf), TRUE) )
                {
                    strncpy(achEncodingBuf, (s8*)pBuf, wLen);
                }
                tLoaclInfo.SetAlias( achEncodingBuf ); 
            }
            
        }
        break;
        
    case NODE_MCUE164NUMBER:  
        
        if( bRead )
        {
			u16 wLocalInfolen =  strlen(tLoaclInfo.GetE164Num());
            *pwBufLen = min(*pwBufLen, wLocalInfolen );
            memcpy((s8*)pBuf, tLoaclInfo.GetE164Num(), *pwBufLen);
        }
        else
        {
            tLoaclInfo.SetE164Num( (s8*)pBuf );
        }
        break;

	case NODE_MCULOCALINFOS: //�ֽ�1    ������ݰ�����������
							 //�ֽ�2    ��1����ı�ʶ��(���磺���뷽ʽ��ʶ��)
							 //�ֽ�3    ����ֵ�����ݳ��ȣ����255��
							 //�ֽ�4    ������ֵ        ����ֵ�����ֵ��Ҫ������������ַ�����/0����������ֵ�����ݳ��ȣ�
		
        //[5/16/2013 liaokang] ֻҪ�յ��˽ڵ����Ϊ��֧��Utf8�����ܣ�������Ȼ��Ϊ��GBK�����ܣ��ϵ�������Ҫ����Խ���ת�룩 
        SetSnmpIsUtf8(TRUE);

        if( bRead )
		{
			u8 *pTempBuf = (u8*)pBuf;
			u16 wBufLen = 0;

			*pTempBuf = 1; 
			wBufLen ++;
			pTempBuf++;

			*pTempBuf = (u8)emCharset;
			wBufLen ++;
			pTempBuf++;

			*pTempBuf = 1; 
			wBufLen ++;
			pTempBuf++;

#ifdef _UTF8
            //[4/8/2013 liaokang] �޸ı��뷽ʽΪutf8
            *pTempBuf = (u8)emenCoding_Utf8;
#else
			*pTempBuf = (u8)emenCoding_GBK; //�ϰ汾��GBK,ֵΪ2
#endif
			wBufLen ++;
			pTempBuf++;

            *pwBufLen = min(*pwBufLen, wBufLen );
		}
		else
		{
			//Ŀǰ��Ϣ��ʱ��֧��д
		}
		break;
        
    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_VERBOSE, "[ProcCallBackMcuLocal] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }
    if ( !bRead )
    {
        wResult = g_cCfgParse.SetLocalInfo(&tLoaclInfo,dwNodeValue);
        if(SUCCESS_AGENT != wResult)
        {
            Agtlog(LOG_ERROR, "[CallBackLocal] SetLocalInfo failed!\n" );
            wResult = SNMP_GENERAL_ERROR;
        }
    }
    return wResult;
}

/*=============================================================================
  �� �� ���� ProcCallBackMcuNet
  ��    �ܣ� ����MCUϵͳ����������Ϣ�ص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwNodeName:    ����ڵ���
             BOOL32  bRead:     ��дģʽ 0-write��1-read
             void * pBuf:       �����������
             u16 *pwBufLen:     ���ݳ���
  �� �� ֵ�� u16 �ɹ� SNMP_SUCCESS
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2006/07/28  4.0         �ű���       ����
=============================================================================*/
u16 ProcCallBackMcuNet(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
    
    u16 wResult = SNMP_SUCCESS;

	TNetWorkInfo  tNetWorkInfo;
	TMPCInfo tMPCInfo;
	g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
	g_cCfgParse.GetMPCInfo(&tMPCInfo);

	//u8 byTempPortKind = g_cCfgParse.GetTempPortKind();
	u8 byLocalPortKind = tMPCInfo.GetLocalPortChoice();
	Agtlog(LOG_VERBOSE, "[NetCallBack] Node(0x%x) received for: %d!\n", dwNodeValue, bRead );

    switch(dwNodeValue) 
    {
	case NODE_MCUNETPORTKIND:
		{
			if( bRead )
			{
				*(s32*)pBuf = (s32)tMPCInfo.GetLocalPortChoice();
				*pwBufLen = sizeof(s32);
			}
			else
			{
				tMPCInfo.SetLocalPortChoice((u8)*(u32*)pBuf);
				g_cCfgParse.SetMPCInfo(tMPCInfo);
			}
		}
		break;
	case NODE_MCUNETTEMPPORTKIND:
		{
			if( bRead )
			{
				*(s32*)pBuf = (s32)g_cCfgParse.GetTempPortKind();
				*pwBufLen = sizeof(s32);
			}
			else
			{
				g_cCfgParse.SetTempPortKind((u8)*(u32*)pBuf);
			}
		}
		break;
	case NODE_MCUNETIPADDR:
		{
			if( bRead )
			{
				*(u32*)pBuf = htonl(g_cCfgParse.GetLocalIp());     
				*pwBufLen = sizeof(s32);
			}
			else
			{
				g_cCfgParse.SetLocalIp(*(u32*)pBuf, byLocalPortKind);
				//g_cCfgParse.SetMPCInfo(tMPCInfo);
			}
		}
		break;
	case NODE_MCUNETIPMASK:
		{
			if( bRead )
			{
				*(u32*)pBuf = htonl(g_cCfgParse.GetMpcMaskIp());
				*pwBufLen = sizeof(s32);
			}
			else
			{
				g_cCfgParse.SetMpcMaskIp(*(u32*)pBuf, byLocalPortKind);
				//g_cCfgParse.SetMPCInfo(tMPCInfo);
			}			
		}
		break;
	case NODE_MCUNETGATEWAY:
		{
			if( bRead )
			{
				*(u32*)pBuf = htonl(g_cCfgParse.GetMpcGateway());
				*pwBufLen = sizeof(s32);
			}
			else
			{
				g_cCfgParse.SetMpcGateway(*(u32*)pBuf);
			}			
		}
		break;
    case NODE_MCUNETGKIPADDR:
        
        if( bRead )
        {
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
			
			*(u32*)pBuf = tNetWorkInfo.GetGkIp();
			
#else
			
			*(u32*)pBuf = htonl(tNetWorkInfo.GetGkIp());
			
#endif

            *pwBufLen = sizeof(s32);
        }
        else
        {
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
			
			tNetWorkInfo.SetGKIp(ntohl(*(u32*)pBuf));
			
#else
			
			tNetWorkInfo.SetGKIp(*(u32*)pBuf);
			
#endif
        }
        break;

    case NODE_MCUNETMULTICASTIPADDR:

        if( bRead )
        {
            *(u32*)pBuf = htonl(tNetWorkInfo.GetCastIp());
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tNetWorkInfo.SetCastIp(*(u32*)pBuf);
        }
        break;

    case NODE_MCUNETMULTICASTPORT: 
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tNetWorkInfo.GetCastPort();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tNetWorkInfo.SetCastPort((u16)*(u32*)pBuf);
        }
        break;
        
    case NODE_MCUNETSTARTRECVPORT : 
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tNetWorkInfo.GetRecvStartPort();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tNetWorkInfo.SetRecvStartPort((u16)*(u32*)pBuf);
        }
        break;

    case NODE_MCUNET225245STARTPORT :   
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tNetWorkInfo.Get225245StartPort();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tNetWorkInfo.Set225245StartPort((u16)*(u32*)pBuf);
        }
        break;

    case NODE_MCUNET225245MAXMTNUM:

        if( bRead )
        {
            *(s32*)pBuf = (s32)tNetWorkInfo.Get225245MtNum();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tNetWorkInfo.Set225245MtNum((u16)*(u32*)pBuf);
        }
        break;

    case NODE_MCUNETUSEMPCTRANSDATA: 
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tNetWorkInfo.GetMpcTransData();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tNetWorkInfo.SetMpcTransData((u8)*(u32*)pBuf);
        }
        break;

    case NODE_MCUNETUSEMPCSTACK:    
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tNetWorkInfo.GetMpcStack();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tNetWorkInfo.SetMpcStack((u8)*(u32*)pBuf);
        }
        break;

	case NODE_MCUNETMAC:
		if (bRead)
		{
			*pwBufLen = g_cCfgParse.GetMac((s8 *)pBuf);
		}
		else
		{
			Agtlog(LOG_ERROR, "[ProcCallBackMcuNet] Can not write mac !\n");
		}
		break;

	case NODE_MCUNETGKUSED:
		if (bRead)
		{
			*(s32*)pBuf = (s32)tNetWorkInfo.GetIsGKCharge();
			*pwBufLen = sizeof(s32);
		}
		else
		{
			tNetWorkInfo.SetIsGKCharge((u8)*(u32*)pBuf);
		}
        break;

    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuNet] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }
	if ( !bRead )
	{
		u16 wRet = g_cCfgParse.SetNetWorkInfo(&tNetWorkInfo,dwNodeValue);
		if(SUCCESS_AGENT != wRet)
		{
			Agtlog(LOG_ERROR, "[ProcCallBackMcuNet] SetNetWorkInfo mpctask failed !\n");
			wResult = SNMP_GENERAL_ERROR;
		}
	}
    return wResult;
}

/*=============================================================================
  �� �� ���� ProcCallBackMcuNetQos
  ��    �ܣ� ����MCUϵͳ����QOS��Ϣ�ص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwNodeName:    ����ڵ���
             BOOL32  bRead:     ��дģʽ 0-write��1-read
             void * pBuf:       �����������
             u16 *pwBufLen:     ���ݳ���
  �� �� ֵ�� u16 �ɹ� SNMP_SUCCESS
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2006/07/28  4.0         �ű���       ����
=============================================================================*/
u16 ProcCallBackMcuNetQos(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
    u8  byTmp8Data  = 0;
    BOOL32 bResult = TRUE;
    u16 wResult = SNMP_SUCCESS;
    
    *pwBufLen = sizeof(s32);

	TQosInfo tQosInfo;
	g_cCfgParse.GetQosInfo( &tQosInfo );
	Agtlog(LOG_VERBOSE, "[NetQosCallBack] Node(0x%x) received for: %d!\n", dwNodeValue, bRead );

    switch(dwNodeValue) 
    {
    case NODE_MCUNETQOSTYPE:
        
        if( bRead )
        {
            byTmp8Data = tQosInfo.GetQosType();
            *(s32*)pBuf = (s32)byTmp8Data;
        }
        else
        {
            bResult = tQosInfo.SetQosType(*(u8*)pBuf);
        }
        break;

    case NODE_MCUNETQOSIPSERVICETYPE: 

        if( bRead )
        {
            byTmp8Data = tQosInfo.GetIpServiceType();
            *(s32*)pBuf = (s32)byTmp8Data;
        }
        else
        {
            bResult = tQosInfo.SetIpServiceType(*(u8*)pBuf);
        }
        break;

    case NODE_MCUNETQOSAUDIOLEVEL: 
        
        if( bRead )
        {
            byTmp8Data = tQosInfo.GetAudioLevel();
            *(s32*)pBuf = (s32)byTmp8Data;
        }
        else
        {
            bResult = tQosInfo.SetAudioLevel(*(u8*)pBuf);
        }
        break;

    case NODE_MCUNETQOSVIDEOLEVEL:
        
        if( bRead )
        {
            byTmp8Data = tQosInfo.GetVideoLevel();
            *(s32*)pBuf = (s32)byTmp8Data;
        }
        else
        {
            bResult = tQosInfo.SetVideoLevel(*(u8*)pBuf);
        }
        break;

    case NODE_MCUNETQOSDATALEVEL: 
        
        if( bRead )
        {
            byTmp8Data = tQosInfo.GetDataLevel();
            *(s32*)pBuf = (s32)byTmp8Data;
        }
        else
        {
            bResult = tQosInfo.SetDataLevel(*(u8*)pBuf);
        }
        break;

    case NODE_MCUNETQOSSIGNALLEVEL: 
        
        if( bRead )
        {
            byTmp8Data = tQosInfo.GetSignalLevel();
            *(s32*)pBuf = (s32)byTmp8Data;
        }
        else
        {
            bResult = tQosInfo.SetSignalLevel(*(u8*)pBuf);
        }
        break;
        
    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuNetQos] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }
	if ( !bResult )
	{
		wResult = SNMP_GENERAL_ERROR;
	}
	if ( !bRead )
	{
		if ( SUCCESS_AGENT != g_cCfgParse.SetQosInfo( &tQosInfo ) )
        {
            wResult = SNMP_GENERAL_ERROR;
		}
	}
    return wResult;
}

/*=============================================================================
  �� �� ���� ProcCallBackMcuNetSnmpCfg
  ��    �ܣ� ����MCUϵͳ����snmp������Ϣ�ص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwNodeName:    ����ڵ���
             BOOL32  bRead:     ��дģʽ 0-write��1-read
             void * pBuf:       �����������
             u16 *pwBufLen:     ���ݳ���
  �� �� ֵ�� u16 �ɹ� SNMP_SUCCESS
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2006/07/28  4.0         �ű���       ����
=============================================================================*/
u16 ProcCallBackMcuNetSnmpCfg(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);

    u16 wTmp16Data  = 0;
    //u32 dwTmp32Data = 0;
    s8  achTmpStr[32];
    memset( achTmpStr, '\0', sizeof(achTmpStr));
    u16 wLen = (*pwBufLen > sizeof(achTmpStr)) ? sizeof(achTmpStr) : *pwBufLen;

	u16 wResult = SNMP_SUCCESS;

	TTrapInfo tTrapInfo;
	wResult = g_cCfgParse.GetTrapInfo(byRow, &tTrapInfo);
	Agtlog(LOG_VERBOSE, "[SnmpCallBack] Node(0x%x) received for: %d!\n", dwNodeValue, bRead );
    
    switch(dwNodeValue) 
    {    
    case NODE_MCUNETSNMPCFGIPADDR:
		        
		if( bRead )
		{
			*(u32*)pBuf = tTrapInfo.GetTrapIp();
			*pwBufLen = sizeof(u32);
		}
		else
		{
			tTrapInfo.SetTrapIp( *(u32*)pBuf );
		}
        break;

    case NODE_MCUNETSNMPCFGREADCOMMUNITY: 

		if( bRead )
		{
			u16 wTrapLen = strlen(tTrapInfo.GetReadCom());
			wLen = min( sizeof(achTmpStr), wTrapLen );
			strncpy( (s8*)pBuf, tTrapInfo.GetReadCom(), wLen);
			*pwBufLen = wLen;
		 }
		 else
		 {
			 tTrapInfo.SetReadCom( (s8*)pBuf );
		 }
         break;

    case NODE_MCUNETSNMPCFGWRITECOMMUNITY:

		if( bRead )
		{
			u16 wTrapLen = strlen(tTrapInfo.GetWriteCom());
			wLen = min( *pwBufLen, wTrapLen );
			strncpy( (s8*)pBuf, tTrapInfo.GetWriteCom(), wLen);
			*pwBufLen = wLen;
		}
		else
		{
			tTrapInfo.SetWriteCom( (s8*)pBuf );
		}
        break;

    case NODE_MCUNETSNMPCFGGETSETPORT:
        
		if( bRead )
		{
			*(s32*)pBuf = (s32)tTrapInfo.GetGSPort();
			*pwBufLen = sizeof(s32);
		}
		else
		{
			tTrapInfo.SetGSPort( *(u16*)pBuf );
		}
        break;

    case NODE_MCUNETSNMPCFGTRAPPORT:

		if( bRead )
		{
			*(s32*)pBuf = (s32)tTrapInfo.GetTrapPort();
			*pwBufLen = sizeof(s32);
		}
		else
		{            
			/*tTrapInfo.SetGSPort( *(u16*)pBuf );*/     // [2012/05/04 liaokang] 
            tTrapInfo.SetTrapPort( *(u16*)pBuf );
		}
        break;

    case NODE_MCUNETSNMPCFGEXIST:

        Agtlog(LOG_WARN, "[SnmpCallBack] Node(0x%x) didn't support operate!\n", dwNodeValue);
        break;

    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuNetSnmpCfg] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }
	if ( !bRead )
	{
		wResult = g_cCfgParse.SetTrapInfo( byRow, &tTrapInfo );
	}
    return wResult;
}

/*=============================================================================
  �� �� ���� ProcCallBackMcuNetBoardCfg
  ��    �ܣ� ����MCUϵͳ���絥��������Ϣ�ص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwNodeName:    ����ڵ���
             BOOL32  bRead:     ��дģʽ 0-write��1-read
             void * pBuf:       �����������
             u16 *pwBufLen:     ���ݳ���
  �� �� ֵ�� u16 �ɹ� SNMP_SUCCESS
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2006/07/28  4.0         �ű���       ����
=============================================================================*/
u16 ProcCallBackMcuNetBoardCfg(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);

    u16 wResult = SNMP_SUCCESS;
    BOOL32 bSupportOpr = TRUE;
    TEqpBrdCfgEntry tBrdInfo;
    memset( &tBrdInfo, 0, sizeof(tBrdInfo) );
    u16 wRet = g_cCfgParse.GetBrdInfoByRow( byRow, &tBrdInfo, TRUE );
    if ( SUCCESS_AGENT != wRet ) 
    {
		Agtlog(LOG_WARN, "[CallBackBrdCfg] Node(0x%x) Row(%d) Error\n", dwNodeValue, byRow );
        return SNMP_GENERAL_ERROR;
    }
    
    Agtlog(LOG_VERBOSE, "[CallBackBrdCfg] Node(0x%x) received for: %d!\n", dwNodeValue, bRead );
    
    switch(dwNodeValue) 
    {
    case NODE_MCUNETBRDCFGID:

        if( bRead )
        {
            *(s32*)pBuf = (s32)tBrdInfo.GetBrdId();
            *pwBufLen = sizeof(s32);
		
			Agtlog(LOG_VERBOSE, "[CallBackBrdCfg] NODE_MCUNETBRDCFGID : %d!\n", tBrdInfo.GetBrdId());
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUNETBRDCFGLAYER: 

        if( bRead )
        {
            *(s32*)pBuf = (s32)tBrdInfo.GetLayer();
            *pwBufLen = sizeof(s32);
			
			Agtlog(LOG_VERBOSE, "[CallBackBrdCfg] NODE_MCUNETBRDCFGLAYER : %d!\n", tBrdInfo.GetLayer());
        }
        else
        {
            tBrdInfo.SetLayer( *(u8*)pBuf );
        }
        break;

    case NODE_MCUNETBRDCFGSLOT: 
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tBrdInfo.GetSlot();
            *pwBufLen = sizeof(s32);
			
			Agtlog(LOG_VERBOSE, "[CallBackBrdCfg] NODE_MCUNETBRDCFGSLOT : %d!\n", tBrdInfo.GetSlot());
        }
        else
        {
            tBrdInfo.SetSlot(  *(u8*)pBuf );
        }
        break;

    case NODE_MCUNETBRDCFGTYPE:
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)(g_cCfgParse.GetSnmpBrdTypeFromMcu(tBrdInfo.GetType()));
            *pwBufLen = sizeof(s32);
			
			Agtlog(LOG_VERBOSE, "[CallBackBrdCfg] NODE_MCUNETBRDCFGTYPE : %d!\n", *(s32*)pBuf);
        }
        else
        {
			// [20110316 miaoqingsong modify] ������ܲൽMCU�൥������ת��
			u8 byBrdType = g_cCfgParse.GetMcuBrdTypeFromSnmp(*(u8*)pBuf);
            tBrdInfo.SetType(byBrdType);
        }
        break;

    case NODE_MCUNETBRDCFGSTATUS:
        
        if( bRead )
        {
			// [20110316 miaoqingsong commentary] ��ȡ����״̬��1 - ������2 - ����
            *(s32*)pBuf = (s32)(g_cCfgParse.BrdStatusAgt2NMS(tBrdInfo.GetState()));
            *pwBufLen = sizeof(s32);
			
			Agtlog(LOG_VERBOSE, "[CallBackBrdCfg] NODE_MCUNETBRDCFGSTATUS : %d!\n", *(s32*)pBuf);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUNETBRDCFGVERSION:
        
        if( bRead )
        {
            u16 wLen = MAX_SOFT_VER_LEN > *pwBufLen ? *pwBufLen : MAX_SOFT_VER_LEN;
            strncpy( (s8*)pBuf, tBrdInfo.GetVersion(), wLen );
            *pwBufLen = wLen;
        }
        else
        {
            tBrdInfo.SetVersion( (s8*)pBuf );
        }
        break;

    case NODE_MCUNETBRDCFGIPADDR:
        
        if( bRead )
        {
            *(u32*)pBuf = htonl(tBrdInfo.GetBrdIp());
            *pwBufLen = sizeof(u32);
        }
        else
        {
            tBrdInfo.SetBrdIp( ntohl(*(u32*)pBuf) );
        }
        break;

    case NODE_MCUNETBRDCFGMODULE:

        bSupportOpr = FALSE;
        break;

    case NODE_MCUNETBRDCFGPANELLED:

        if( bRead )
        {
            // liaokang [2012/04/05] �޸� ����״̬��ȡ��LED�����ڵȣ�
/*			u16 wBrdInfoLen = strlen(tBrdInfo.GetPanelLed());
            u16 wLen = min( wBrdInfoLen, *pwBufLen );
            *pwBufLen = (u16)wLen;
            strncpy( (s8*)pBuf, tBrdInfo.GetPanelLed(), wLen );
            
			// [20110813 miaoqingosng add] ����ͼ��״̬��ӡ��ӣ�1 �� 2 �� 3 ���� 4 ���� 5 ÿ��2������
			Agtlog( LOG_VERBOSE, "[CallBackBrdCfg] NODE_MCUNETBRDCFGPANELLED <%s>!\n", tBrdInfo.GetPanelLed() );*/
            
            u16 wCopyLen = 0;
            s8 achLedStatus[MAX_BOARD_LED_NUM + 1]={0};
            s8 achEthPortStatus[MAX_BOARD_ETHPORT_NUM + 1]={0};

            u16 wBrdLedLen = (u16)g_cCfgParse.PanelLedStatusAgt2NMS( tBrdInfo, achLedStatus);
            Agtlog( LOG_VERBOSE, "[CallBackBrdCfg] NODE_MCUNETBRDCFGPANELLED  BrdType: %d! Recombined Led: <%s>! Led Num %d!\n",
                tBrdInfo.GetType(), achLedStatus, wBrdLedLen );

            u16 wBrdEthPortLen = (u16)g_cCfgParse.EthPortStatusAgt2NMS( tBrdInfo, achEthPortStatus );
            Agtlog( LOG_VERBOSE, "[CallBackBrdCfg] NODE_MCUNETBRDCFGPANELLED  BrdType: %d! Recombined EthPort: <%s>! EthPortBufSize %d!\n",
                tBrdInfo.GetType(), achEthPortStatus, wBrdEthPortLen );
            
            wCopyLen = min( wBrdLedLen, *pwBufLen );
            if ( wCopyLen )  // ����Led״̬����0��
            {
                strncpy( (s8*)pBuf, achLedStatus, wCopyLen );
            }
            else
            {
                switch ( tBrdInfo.GetType() )
                {
                case BRD_TYPE_IS22:
                    wBrdLedLen = 32; // IS2.2 32��led
                    wCopyLen = wBrdLedLen;
                    memset( (s8*)pBuf, 2, wCopyLen );   // IS2.2��״̬ ��
                    break;
                default:
                    break; 
                } 
            }

            if ( wBrdEthPortLen )      // ��������״̬����0��
            {
                wCopyLen = min( wBrdEthPortLen, (*pwBufLen) - wCopyLen );
                if ( wCopyLen )
                {
                    strncpy( (s8*)pBuf + wBrdLedLen, achEthPortStatus, wCopyLen );
                    wCopyLen = wCopyLen + wBrdLedLen;
                }
            }
            else
            {
                switch ( tBrdInfo.GetType() )
                {
                case BRD_TYPE_IS22:
                    {
                        // �ָ���'~'(s8) + '~'(s8) + ״̬����(u8) +ǰ������Ŀ(u8) + ǰ����״̬(3��) + ��������Ŀ(u8) + ������״̬(8��)
                        s8 achEthPort[16]; 
                        memset( achEthPort, 3, sizeof(achEthPort) ); // IS2.2����״̬ δ֪
                        achEthPort[0] = '~';        // �ָ���
                        achEthPort[1] = '~';        // �ָ���
                        achEthPort[2] = 1;          // ״̬����
                        achEthPort[3] = 3;          // ǰ������Ŀ
                        achEthPort[7] = 8;          // ��������Ŀ
                        wBrdEthPortLen = sizeof( achEthPort );
                        wCopyLen = min( wBrdEthPortLen, (*pwBufLen) - wCopyLen );
                        if ( wCopyLen )
                        {
                            // �ָ���
                            strncpy( (s8*)pBuf + wBrdLedLen, achEthPort, wCopyLen );
                            wCopyLen = wCopyLen + wBrdLedLen;  
                        }
                    }
                    break;
                default:
                    break; 
                } 
            }

            *pwBufLen = (u16)wCopyLen; 
            Agtlog( LOG_VERBOSE, "[CallBackBrdCfg] NODE_MCUNETBRDCFGPANELLED BrdType: %d! Buf:%s! BufSize: %d!\n",
                tBrdInfo.GetType(), pBuf, *pwBufLen );
        }   
        else
        {
            bSupportOpr = FALSE;
        }
        break;
        
    case NODE_MCUNETBRDCFGOSTYPE:

        if ( bRead )
        {
            *(s32*)pBuf = (s32)tBrdInfo.GetOSType();
            *pwBufLen = sizeof(s32);
			Agtlog(LOG_VERBOSE, "[CallBackBrdCfg] NODE_MCUNETBRDCFGOSTYPE : %d!\n", *(s32*)pBuf);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuNetBoardCfg] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }
    if ( !bRead )
    {
        wResult = g_cCfgParse.SetBrdInfoByRow( byRow, &tBrdInfo );
    }
    if ( !bSupportOpr )
    {
        Agtlog( LOG_VERBOSE, "[CallBackBrdCfg] node(0x%x) not support operate for: %d<1.read-0.write> \n", dwNodeValue, bRead );
    }
    return wResult;
}

/*=============================================================================
  �� �� ���� ProcCallBackMcuEqpMixer
  ��    �ܣ� ����MCU����mixer��Ϣ�ص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwNodeName:    ����ڵ���
             BOOL32  bRead:     ��дģʽ 0-write��1-read
             void * pBuf:       �����������
             u16 *pwBufLen:     ���ݳ���
  �� �� ֵ�� u16 �ɹ� SNMP_SUCCESS
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2006/07/28  4.0         �ű���       ����
=============================================================================*/
u16 ProcCallBackMcuEqpMixer(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);

    u8   byTmp8Data = 0;
    u16  wTmp16Data = 0;
    u32  dwTmp32Data = 0;

    TEqpMixerInfo tMixerInfo;
    u16 wRet = g_cCfgParse.GetEqpMixerCfgByRow( byRow, &tMixerInfo );
    if ( SUCCESS_AGENT != wRet )
    {
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpMixer] get mixer info failed, ret:%d !\n", wRet);
        return SNMP_GENERAL_ERROR;
    }
    Agtlog(LOG_VERBOSE, "[CallBackMixer]Node<0x%x> received for: %d!\n", dwNodeValue, bRead );

    u16 wResult = SNMP_SUCCESS;
    BOOL32 bSupportOpr = TRUE;

    switch(dwNodeValue) 
    {
    case NODE_MCUEQPMIXERID: 

        if( bRead )
        {
           byTmp8Data = tMixerInfo.GetEqpId();
           *(s32*)pBuf = (s32)byTmp8Data; 
           *pwBufLen = sizeof(byTmp8Data);
        }
        else
        {
            bSupportOpr = FALSE;
        }        
        break;

    case NODE_MCUEQPMIXERPORT:

        if( bRead )
        {
            wTmp16Data = tMixerInfo.GetMcuRecvPort();
            *(s32*)pBuf = (s32)wTmp16Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tMixerInfo.SetMcuRecvPort( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPMIXERSWITCHBRDID:

        if( bRead )
        {
            byTmp8Data = tMixerInfo.GetSwitchBrdId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tMixerInfo.SetSwitchBrdId( *(u8*)pBuf );
        }
        break;

    case NODE_MCUEQPMIXERALIAS:
        {            
            //[5/16/2013 liaokang] ת��
            s8 achEncodingBuf[MAXLEN_EQP_ALIAS+1] = {0};
            u16  wLen = min(sizeof(achEncodingBuf)-1, *pwBufLen);
            if( bRead )
            {                
                if( FALSE == TransEncodingForNmsData(tMixerInfo.GetAlias(), achEncodingBuf, sizeof(achEncodingBuf)) )
                {
                    strncpy( achEncodingBuf, tMixerInfo.GetAlias(), wLen);
                }
                *pwBufLen = strlen(achEncodingBuf)+1;
                *pwBufLen = min(*pwBufLen, wLen);
                strncpy( (s8*)pBuf, achEncodingBuf, *pwBufLen);
            }     
            else
            {
                if( FALSE == TransEncodingForNmsData((s8*)pBuf, achEncodingBuf, wLen, TRUE) )
                {
                    strncpy(achEncodingBuf, (s8*)pBuf, wLen);
                }
                tMixerInfo.SetAlias(achEncodingBuf);
            }
        }
        break;

    case NODE_MCUEQPMIXERRUNNINGBRDID:

        if( bRead )
        {
            byTmp8Data = tMixerInfo.GetRunBrdId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tMixerInfo.SetRunBrdId( *(u8*)pBuf );
        }
        break;

    case NODE_MCUEQPMIXERIPADDR:

        if( bRead )
        {
            dwTmp32Data = htonl(tMixerInfo.GetIpAddr());
            *(u32*)pBuf = dwTmp32Data;
            *pwBufLen = sizeof(dwTmp32Data);
        }
        else
        {
            bSupportOpr = FALSE;
        } 
        break;

    case NODE_MCUEQPMIXERMIXSTARTPORT:

        if( bRead )
        {
            wTmp16Data = tMixerInfo.GetEqpRecvPort();
            *(s32*)pBuf = (s32)wTmp16Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tMixerInfo.SetEqpRecvPort( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPMIXERMAXMIXGROUPNUM:  

        if( bRead )
        {
             byTmp8Data = tMixerInfo.GetMaxChnInGrp();
             *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tMixerInfo.SetMaxChnInGrp( *(u8*)pBuf );
        }
        break;

    case NODE_MCUEQPMIXERCONNSTATE:

        bSupportOpr = FALSE;
        break;

    case NODE_MCUEQPMIXEREXIST:
        
        bSupportOpr = FALSE;
        break;

    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpMixer] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }
    
    if ( !bRead )
    {
        u16 wResult = g_cCfgParse.SetEqpMixerCfgByRow(byRow, tMixerInfo);       
    }
    if ( !bSupportOpr )
    {
        Agtlog(LOG_WARN, "[ProcCallBackMcuEqpMixer] node(0x%x) not support operate for: %d<1.read-0.write> \n", dwNodeValue, bRead);
    }
    return wResult;
}

/*=============================================================================
  �� �� ���� ProcCallBackMcuEqpRecorder
  ��    �ܣ� ����MCU����recorder��Ϣ�ص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwNodeName:    ����ڵ���
             BOOL32  bRead:     ��дģʽ 0-write��1-read
             void * pBuf:       �����������
             u16 *pwBufLen:     ���ݳ���
  �� �� ֵ�� u16 �ɹ� SNMP_SUCCESS
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2006/07/28  4.0         �ű���       ����
=============================================================================*/
u16 ProcCallBackMcuEqpRecorder(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
    
    u8   byTmp8Data = 0;
    u16  wTmp16Data = 0;
    u32  dwTmp32Data = 0;
    
    TEqpRecInfo tRecInfo;
    u16 wRet = g_cCfgParse.GetEqpRecCfgByRow( byRow, &tRecInfo );
    if ( SUCCESS_AGENT != wRet )
    {
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpRecorder] get rec info failed, ret:%d !\n", wRet);
        return SNMP_GENERAL_ERROR;
    }
    Agtlog(LOG_VERBOSE, "[CallBackRec] Node<0x%x> received for: %d!\n", dwNodeValue, bRead );

    u16 wResult = SNMP_SUCCESS;
    BOOL32 bSupportOpr = TRUE;
    
    switch(dwNodeValue) 
    {
    case NODE_MCUEQPRECORDERID: 

        if( bRead )
        {
            byTmp8Data = tRecInfo.GetEqpId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            bSupportOpr = FALSE;
        } 
        break;

    case NODE_MCUEQPRECORDERSWITCHBRDID:
        
        if( bRead )
        {
            byTmp8Data = tRecInfo.GetSwitchBrdId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tRecInfo.SetSwitchBrdId( *(u8*)pBuf );
        }
        break;

    case NODE_MCUEQPRECORDERPORT:
        
		if( bRead )
		{
			wTmp16Data = tRecInfo.GetMcuRecvPort();
			*(s32*)pBuf = (s32)wTmp16Data;
			*pwBufLen = sizeof(s32);
		}
		else
		{
			tRecInfo.SetEqpRecvPort( *(u16*)pBuf );
		}
        break;

    case NODE_MCUEQPRECORDERALIAS:
        {            
            //[5/16/2013 liaokang] ת��
            s8 achEncodingBuf[MAXLEN_EQP_ALIAS+1] = {0};
            u16  wLen = min(sizeof(achEncodingBuf)-1, *pwBufLen);
            if( bRead )
            {                
                if( FALSE == TransEncodingForNmsData(tRecInfo.GetAlias(), achEncodingBuf, sizeof(achEncodingBuf)) )
                {
                    strncpy(achEncodingBuf, tRecInfo.GetAlias(), wLen);
                }
                *pwBufLen = strlen(achEncodingBuf)+1;
                *pwBufLen = min(*pwBufLen, wLen);
                strncpy( (s8*)pBuf, achEncodingBuf, *pwBufLen);
            }     
            else
            {
                if( FALSE == TransEncodingForNmsData((s8*)pBuf, achEncodingBuf, wLen, TRUE) )
                {
                    strncpy(achEncodingBuf, (s8*)pBuf, wLen);
                }
                tRecInfo.SetAlias( achEncodingBuf );
            }
        }
        break;

    case NODE_MCUEQPRECORDERIPADDR:

		if( bRead )
		{
		   dwTmp32Data = htonl(tRecInfo.GetIpAddr());
		   *(u32*)pBuf = dwTmp32Data;
		   *pwBufLen = sizeof(dwTmp32Data);
		}
		else
		{
			tRecInfo.SetIpAddr( *(u32*)pBuf );
		}
        break;

    case NODE_MCUEQPRECORDERRECVSTARTPORT: 

		if( bRead )
		{
		   wTmp16Data = tRecInfo.GetEqpRecvPort();
		   *(s32*)pBuf = (s32)wTmp16Data;
		   *pwBufLen = sizeof(s32);
		}
		else
		{
			tRecInfo.SetEqpRecvPort( *(u16*)pBuf );
		}
        break;

    case NODE_MCUEQPRECORDERCONNSTATE:
    case NODE_MCUEQPRECORDEREXIST:
        
        bSupportOpr = FALSE;
        break;

    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpRecorder] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }
    if ( !bRead )
    {
        u16 wResult = g_cCfgParse.SetEqpRecCfgByRow( byRow, &tRecInfo);
        if( SUCCESS_AGENT != wResult )
        {
            wResult = SNMP_GENERAL_ERROR;
        }        
    }
    if ( !bSupportOpr )
    {
        Agtlog(LOG_WARN, "[ProcCallBackMcuEqpRecorder] node(0x%x) not support operate for: %d<1.read-0.write> \n", dwNodeValue, bRead);
    }
    return wResult;
}

/*=============================================================================
  �� �� ���� ProcCallBackMcuEqpTVWall
  ��    �ܣ� ����MCU����tvwall��Ϣ�ص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwNodeName:    ����ڵ���
             BOOL32  bRead:     ��дģʽ 0-write��1-read
             void * pBuf:       �����������
             u16 *pwBufLen:     ���ݳ���
  �� �� ֵ�� u16 �ɹ� SNMP_SUCCESS
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2006/07/28  4.0         �ű���       ����
=============================================================================*/
u16 ProcCallBackMcuEqpTVWall(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
    
    u8   byTmp8Data = 0;
    u16  wTmp16Data = 0;
    u32  dwTmp32Data = 0;

    TEqpTVWallInfo tTVWallInfo;
    u16 wRet = g_cCfgParse.GetEqpTVWallCfgByRow( byRow, &tTVWallInfo );
    if ( SUCCESS_AGENT != wRet )
    {
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpTVWall] get rec info failed, ret:%d !\n", wRet);
        return SNMP_GENERAL_ERROR;
    }
    Agtlog(LOG_VERBOSE, "[CallBackTVWall] Node<0x%x> received for: %d!\n", dwNodeValue, bRead );

    u16 wResult = SNMP_SUCCESS;
    BOOL32 bSupportOpr = TRUE;
    
    switch(dwNodeValue) 
    {
    case NODE_MCUEQPTVWALLID:

        if( bRead )
        {
            byTmp8Data = tTVWallInfo.GetEqpId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            bSupportOpr = FALSE;
        } 
        break;

    case NODE_MCUEQPTVWALLALIAS: 
        {            
            //[5/16/2013 liaokang] ת��
            s8 achEncodingBuf[MAXLEN_EQP_ALIAS+1] = {0};
            u16  wLen = min(sizeof(achEncodingBuf)-1, *pwBufLen);
            if( bRead )
            {                
                if( FALSE == TransEncodingForNmsData(tTVWallInfo.GetAlias(), achEncodingBuf, sizeof(achEncodingBuf)) )
                {
                    strncpy( achEncodingBuf, tTVWallInfo.GetAlias(), wLen);
                }
                *pwBufLen = strlen(achEncodingBuf)+1;
                *pwBufLen = min(*pwBufLen, wLen);
                strncpy( (s8*)pBuf, achEncodingBuf, *pwBufLen);
            }     
            else
            {
                if( FALSE == TransEncodingForNmsData((s8*)pBuf, achEncodingBuf, wLen, TRUE) )
                {
                    strncpy(achEncodingBuf, (s8*)pBuf, wLen);
                }
                tTVWallInfo.SetAlias(achEncodingBuf);
            }
        }
        break;

    case NODE_MCUEQPTVWALLRUNNINGBRDID: 
        
        if( bRead )
        {
            byTmp8Data = tTVWallInfo.GetRunBrdId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(byTmp8Data);
        }
        else
        {
            tTVWallInfo.SetRunBrdId( *(u8*)pBuf );
        }
        break;

    case NODE_MCUEQPTVWALLIPADDR:  

        if( bRead )
        {
            dwTmp32Data = htonl(tTVWallInfo.GetIpAddr());
            *(u32*)pBuf = dwTmp32Data;
            *pwBufLen = sizeof(dwTmp32Data);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUEQPTVWALLVIDEOSTARTRECVPORT:  

        if( bRead )
        {
            wTmp16Data = tTVWallInfo.GetEqpRecvPort();
            *(s32*)pBuf = (s32)wTmp16Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tTVWallInfo.SetEqpRecvPort( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPTVWALLCONNSTATE:        
    case NODE_MCUEQPTVWALLEXIST:
        
        bSupportOpr = FALSE;
        break;

    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpTVWall] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }

    if ( !bRead )
    {
        u16 wResult = g_cCfgParse.SetEqpTVWallCfgByRow( byRow, &tTVWallInfo );
        if( SUCCESS_AGENT != wResult )
        {
            wResult = SNMP_GENERAL_ERROR;
        }        
    }
    if ( !bSupportOpr )
    {
        Agtlog(LOG_WARN, "[ProcCallBackMcuEqpTVWall] node(0x%x) not support operate for: %d<1.read-0.write> \n", dwNodeValue, bRead);
    }
    return wResult;
}

/*=============================================================================
  �� �� ���� ProcCallBackMcuEqpBas
  ��    �ܣ� ����MCU����bas��Ϣ�ص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwNodeName:    ����ڵ���
             BOOL32  bRead:     ��дģʽ 0-write��1-read
             void * pBuf:       �����������
             u16 *pwBufLen:     ���ݳ���
  �� �� ֵ�� u16 �ɹ� SNMP_SUCCESS
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2006/07/28  4.0         �ű���       ����
=============================================================================*/
u16 ProcCallBackMcuEqpBas(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
    
    u8   byTmp8Data = 0;
    u16  wTmp16Data = 0;
    u32  dwTmp32Data = 0;    
   
    TEqpBasInfo tBasInfo;
    u16 wRet = g_cCfgParse.GetEqpBasCfgByRow( byRow, &tBasInfo );
    if ( SUCCESS_AGENT != wRet )
    {
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpBas] get bas info failed, ret:%d !\n", wRet);
        return SNMP_GENERAL_ERROR;
    }
    Agtlog(LOG_VERBOSE, "[CallBackBas] Node<0x%x> received for: %d!\n", dwNodeValue, bRead );
    
    u16 wResult = SNMP_SUCCESS;
    BOOL32 bSupportOpr = TRUE;
    
    switch(dwNodeValue) 
    {
    case NODE_MCUEQPBASID:
        
        if( bRead )
        {
            byTmp8Data = tBasInfo.GetEqpId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUEQPBASPORT:

        if( bRead )
        {
            wTmp16Data = tBasInfo.GetMcuRecvPort();
            *(s32*)pBuf = (s32)wTmp16Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tBasInfo.SetMcuRecvPort( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPBASSWITCHBRDID:   

        if( bRead )
        {
            byTmp8Data = tBasInfo.GetSwitchBrdId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tBasInfo.SetSwitchBrdId( *(u8*)pBuf );
        }
        break;

    case NODE_MCUEQPBASALIAS:
        {            
            //[5/16/2013 liaokang] ת��
            s8 achEncodingBuf[MAXLEN_EQP_ALIAS+1] = {0};
            u16  wLen = min(sizeof(achEncodingBuf)-1, *pwBufLen);
            if( bRead )
            {                
                if( FALSE == TransEncodingForNmsData(tBasInfo.GetAlias(), achEncodingBuf, sizeof(achEncodingBuf)) )
                {
                    strncpy( achEncodingBuf, tBasInfo.GetAlias(), wLen);
                }
                *pwBufLen = strlen(achEncodingBuf)+1;
                *pwBufLen = min(*pwBufLen, wLen);
                strncpy( (s8*)pBuf, achEncodingBuf, *pwBufLen);
            }     
            else
            {
                if( FALSE == TransEncodingForNmsData((s8*)pBuf, achEncodingBuf, wLen, TRUE) )
                {
                    strncpy(achEncodingBuf, (s8*)pBuf, wLen);
                }
                tBasInfo.SetAlias(achEncodingBuf);
            }
        }
        break; 

    case NODE_MCUEQPBASRUNNINGBRDID:
        
        if( bRead )
        {
            byTmp8Data = tBasInfo.GetRunBrdId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tBasInfo.SetRunBrdId( *(u8*)pBuf );
        }
        break;

    case NODE_MCUEQPBASIPADDR:

        if( bRead )
        {
            dwTmp32Data = htonl(tBasInfo.GetIpAddr());
            *(u32*)pBuf = dwTmp32Data;
            *pwBufLen = sizeof(dwTmp32Data);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUEQPBASSTARTRECVPORT :

        if( bRead )
        {
            wTmp16Data = tBasInfo.GetEqpRecvPort();
            *(s32*)pBuf = (s32)wTmp16Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tBasInfo.SetEqpRecvPort( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPBASCONNSTATE:
    case NODE_MCUEQPBASEXIST:
        
        bSupportOpr = FALSE;
        break;

    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpBas] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }

    if ( !bRead )
    {
        u16 wResult = g_cCfgParse.SetEqpBasCfgByRow( byRow, &tBasInfo );
        if( SUCCESS_AGENT != wResult )
        {
            wResult = SNMP_GENERAL_ERROR;
        }        
    }
    if ( !bSupportOpr )
    {
        Agtlog(LOG_WARN, "[ProcCallBackMcuEqpBas] node(0x%x) not support operate for: %d<1.read-0.write> \n", dwNodeValue, bRead);
    }
    return wResult;
}

/*=============================================================================
  �� �� ���� ProcCallBackMcuEqpVMP
  ��    �ܣ� ����MCU����VMP��Ϣ�ص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwNodeName:    ����ڵ���
             BOOL32  bRead:     ��дģʽ 0-write��1-read
             void * pBuf:       �����������
             u16 *pwBufLen:     ���ݳ���
  �� �� ֵ�� u16 �ɹ� SNMP_SUCCESS
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2006/07/28  4.0         �ű���       ����
=============================================================================*/
u16 ProcCallBackMcuEqpVMP(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
 
    u8   byTmp8Data = 0;
    u16  wTmp16Data = 0;
    u32  dwTmp32Data = 0;
   
    TEqpVMPInfo tVMPInfo;
    u16 wRet = g_cCfgParse.GetEqpVMPCfgByRow( byRow, &tVMPInfo );
    if ( SUCCESS_AGENT != wRet )
    {
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpVMP] get vmp info failed, ret:%d !\n", wRet);
        return SNMP_GENERAL_ERROR;
    }
    Agtlog(LOG_VERBOSE, "[CallBackVMP] Node<0x%x> received for: %d!\n", dwNodeValue, bRead );
    
    u16 wResult = SNMP_SUCCESS;
    BOOL32 bSupportOpr = TRUE;

    switch(dwNodeValue) 
    {
    case NODE_MCUEQPVMPID :

        if( bRead )
        {
            byTmp8Data = tVMPInfo.GetEqpId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUEQPVMPPORT :
        
        if( bRead )
        {
            wTmp16Data = tVMPInfo.GetMcuRecvPort();
            *(s32*)pBuf = (s32)wTmp16Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tVMPInfo.SetMcuRecvPort( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPVMPSWITCHBRDID :

        if( bRead )
        {
            byTmp8Data = tVMPInfo.GetSwitchBrdId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tVMPInfo.SetSwitchBrdId( *(u8*)pBuf );
        }
        break;

    case NODE_MCUEQPVMPALIAS:
        {            
            //[5/16/2013 liaokang] ת��
            s8 achEncodingBuf[MAXLEN_EQP_ALIAS+1] = {0};
            u16  wLen = min(sizeof(achEncodingBuf)-1, *pwBufLen);
            if( bRead )
            {                
                if( FALSE == TransEncodingForNmsData(tVMPInfo.GetAlias(), achEncodingBuf, sizeof(achEncodingBuf)) )
                {
                    strncpy( achEncodingBuf, tVMPInfo.GetAlias(), wLen);
                }
                *pwBufLen = strlen(achEncodingBuf)+1;
                *pwBufLen = min(*pwBufLen, wLen);
                strncpy( (s8*)pBuf, achEncodingBuf, *pwBufLen);
            }     
            else
            {
                if( FALSE == TransEncodingForNmsData((s8*)pBuf, achEncodingBuf, wLen, TRUE) )
                {
                    strncpy(achEncodingBuf, (s8*)pBuf, wLen);
                }
                tVMPInfo.SetAlias(achEncodingBuf);
            }
        }
        break;

    case NODE_MCUEQPVMPRUNNINGBRDID :
        
        if( bRead )
        {
            byTmp8Data = tVMPInfo.GetRunBrdId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tVMPInfo.SetRunBrdId( *(u8*)pBuf );
        }
        break;

    case NODE_MCUEQPVMPIPADDR :

        if( bRead )
        {
            dwTmp32Data = htonl(tVMPInfo.GetIpAddr());
            *(u32*)pBuf = dwTmp32Data;
            *pwBufLen = sizeof(dwTmp32Data);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUEQPVMPSTARTRECVPORT : 
        
        if( bRead )
        {
            wTmp16Data = tVMPInfo.GetEqpRecvPort();
            *(s32*)pBuf = (s32)wTmp16Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tVMPInfo.SetEqpRecvPort( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPVMPENCODERNUM: 
        
        if( bRead )
        {
            byTmp8Data = tVMPInfo.GetEncodeNum();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tVMPInfo.SetEncodeNum( *(u8*)pBuf );
        }
        break;

    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpVMP] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }
    if ( !bRead )
    {
        u16 wResult = g_cCfgParse.SetEqpVMPCfgByRow( byRow, &tVMPInfo );
        if( SUCCESS_AGENT != wResult )
        {
            wResult = SNMP_GENERAL_ERROR;
        }        
    }
    if ( !bSupportOpr )
    {
        Agtlog(LOG_WARN, "[ProcCallBackMcuEqpVMP] node(0x%x) not support operate for: %d<1.read-0.write> \n", dwNodeValue, bRead);
    }
    return wResult;
}

/*=============================================================================
  �� �� ���� ProcCallBackMcuEqpDcs
  ��    �ܣ� ����MCU����DCS��Ϣ�ص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwNodeName:    ����ڵ���
             BOOL32  bRead:     ��дģʽ 0-write��1-read
             void * pBuf:       �����������
             u16 *pwBufLen:     ���ݳ���
  �� �� ֵ�� u16 �ɹ� SNMP_SUCCESS
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2006/07/28  4.0         �ű���       ����
=============================================================================*/
u16 ProcCallBackMcuEqpDcs(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
    
    u32 dwTmp32Data = 0;
    u16 wResult = SNMP_SUCCESS;
    
    switch(dwNodeValue) 
    {
    case NODE_MCUEQPDCSIPADDR :  
        
        Agtlog(LOG_VERBOSE, "NODE_MCUEQPDCSIPADDR \treceived For: %d!\n", bRead );
        if( bRead )
        {
            dwTmp32Data = g_cCfgParse.GetDcsIp();
            *(u32*)pBuf = dwTmp32Data;           
            *pwBufLen = sizeof(dwTmp32Data);
        }
        else
        {
            dwTmp32Data = *(u32*)pBuf;
            u16 wResult = g_cCfgParse.SetDcsIp( dwTmp32Data );
            if( SUCCESS_AGENT != wResult )
            {
                wResult = SNMP_GENERAL_ERROR;
            }
        }
        break;
        
    case NODE_MCUEQPDCSCONNSTATE:
        
        Agtlog(LOG_WARN, "NODE_MCUEQPDCSCONNSTATE didn't support operate!\n");
        break;
        
    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpDcs] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }
    return wResult;
}


/*=============================================================================
  �� �� ���� ProcCallBackMcuEqpPrs
  ��    �ܣ� ����MCU����prs��Ϣ�ص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwNodeName:    ����ڵ���
             BOOL32  bRead:     ��дģʽ 0-write��1-read
             void * pBuf:       �����������
             u16 *pwBufLen:     ���ݳ���
  �� �� ֵ�� u16 �ɹ� SNMP_SUCCESS
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2006/07/28  4.0         �ű���       ����
=============================================================================*/
u16 ProcCallBackMcuEqpPrs(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
    
    u8   byTmp8Data = 0;
    u16  wTmp16Data = 0;
    u32  dwTmp32Data = 0;
    BOOL32 bResult = FALSE;
   
    TEqpPrsInfo tPrsInfo;
    u16 wRet = g_cCfgParse.GetEqpPrsCfgByRow( byRow, &tPrsInfo );
    if ( SUCCESS_AGENT != wRet )
    {
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpPrs] get prs info failed, ret:%d !\n", wRet);
        return SNMP_GENERAL_ERROR;
    }
    Agtlog(LOG_VERBOSE, "[CallBackPrs] Node<0x%x> received for: %d!\n", dwNodeValue, bRead );
    
    u16 wResult = SNMP_SUCCESS;
    BOOL32 bSupportOpr = TRUE;
    
    switch(dwNodeValue) 
    {
    case NODE_MCUEQPPRSID:   
        
        if( bRead )
        {
            byTmp8Data = tPrsInfo.GetEqpId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUEQPPRSMCUSTARTPORT: 
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tPrsInfo.GetMcuRecvPort();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tPrsInfo.SetMcuRecvPort( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPPRSSWITCHBRDID :  
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tPrsInfo.GetSwitchBrdId();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tPrsInfo.SetSwitchBrdId( *(u8*)pBuf );
        }
        break;

    case NODE_MCUEQPPRSALIAS:
        {            
            //[5/16/2013 liaokang] ת��
            s8 achEncodingBuf[MAXLEN_EQP_ALIAS+1] = {0};
            u16  wLen = min(sizeof(achEncodingBuf)-1, *pwBufLen);
            if( bRead )
            {                
                if( FALSE == TransEncodingForNmsData(tPrsInfo.GetAlias(), achEncodingBuf, sizeof(achEncodingBuf)) )
                {
                    strncpy( achEncodingBuf, tPrsInfo.GetAlias(), wLen);
                }
                *pwBufLen = strlen(achEncodingBuf)+1;
                *pwBufLen = min(*pwBufLen, wLen);
                strncpy( (s8*)pBuf, achEncodingBuf, *pwBufLen);
            }     
            else
            {
                if( FALSE == TransEncodingForNmsData((s8*)pBuf, achEncodingBuf, wLen, TRUE) )
                {
                    strncpy(achEncodingBuf, (s8*)pBuf, wLen);
                }
                tPrsInfo.SetAlias(achEncodingBuf);
            }
        }
        break;

    case NODE_MCUEQPPRSRUNNINGBRDID :  
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tPrsInfo.GetRunBrdId();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tPrsInfo.SetRunBrdId( *(u8*)pBuf );
        }
        break;

    case NODE_MCUEQPPRSIPADDR :       
        
        if( bRead )
        {
            *(u32*)pBuf = htonl(tPrsInfo.GetIpAddr());
            *pwBufLen = sizeof(dwTmp32Data);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUEQPPRSSTARTPORT : 
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tPrsInfo.GetEqpRecvPort();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tPrsInfo.SetEqpRecvPort( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPPRSFIRSTTIMESPAN : 
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tPrsInfo.GetFirstTimeSpan();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tPrsInfo.SetFirstTimeSpan( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPPRSSECONDTIMESPAN : 
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tPrsInfo.GetSecondTimeSpan();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tPrsInfo.SetSecondTimeSpan( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPPRSTHIRDTIMESPAN :
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tPrsInfo.GetThirdTimeSpan();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tPrsInfo.SetThirdTimeSpan( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPPRSREJECTTIMESPAN: 
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tPrsInfo.GetRejectTimeSpan();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tPrsInfo.SetRejectTimeSpan( *(u16*)pBuf );
        }
        break;

    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpPrs] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }

    if ( !bRead )
    {
        u16 wResult = g_cCfgParse.SetEqpPrsCfgByRow( byRow, &tPrsInfo );
        if( SUCCESS_AGENT != wResult )
        {
            wResult = SNMP_GENERAL_ERROR;
        }        
    }
    if ( !bSupportOpr )
    {
        Agtlog(LOG_WARN, "[ProcCallBackMcuEqpPrs] node(0x%x) not support operate for: %d<1.read-0.write> \n", dwNodeValue, bRead);
    }
    return wResult;
}

/*=============================================================================
  �� �� ���� ProcCallBackMcuEqpNetSync
  ��    �ܣ� ����MCU������ͬ����Ϣ�ص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwNodeName:    ����ڵ���
             BOOL32  bRead:     ��дģʽ 0-write��1-read
             void * pBuf:       �����������
             u16 *pwBufLen:     ���ݳ���
  �� �� ֵ�� u16 �ɹ� SNMP_SUCCESS
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2006/07/28  4.0         �ű���       ����
=============================================================================*/
u16 ProcCallBackMcuEqpNetSync(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
    
    u8   byTmp8Data = 0;
    BOOL32 bResult = FALSE;
    u16 wResult = SNMP_SUCCESS;
	*pwBufLen = sizeof(s32);

	TNetSyncInfo tNetSyncInfo;
	g_cCfgParse.GetNetSyncInfo( &tNetSyncInfo );
    Agtlog(LOG_VERBOSE, "[NetSyncCallBack] Node(0x%x) received for: %d!\n", dwNodeValue, bRead );

    switch(dwNodeValue) 
    {
    case NODE_MCUEQPNETSYNCMODE : 
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tNetSyncInfo.GetMode();
        }
        else
        {
            tNetSyncInfo.SetMode( *(u8*)pBuf );
        }
        break;
        
    case NODE_MCUEQPNETSYNCDTSLOT:  
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tNetSyncInfo.GetDTSlot();
        }
        else
        {
            tNetSyncInfo.SetDTSlot( byTmp8Data );
        }
        break;
        
    case NODE_MCUEQPNETSYNCE1INDEX :  
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tNetSyncInfo.GetE1Index();
        }
        else
        {
            tNetSyncInfo.SetE1Index( *(u8*)pBuf );
        }
        break;
        
    case NODE_MCUEQPNETSYNCSTATE:
		break;
		
    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpNetSync] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }
	if ( !bRead ) 
	{
		wResult = g_cCfgParse.SetNetSyncInfo( &tNetSyncInfo );
	}
    return wResult;
}


/*=============================================================================
  �� �� ���� ProcCallBackMcuPfmInfo
  ��    �ܣ� ����MCU������Ϣ�ص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwNodeName:    ����ڵ���
             BOOL32  bRead:     ��дģʽ 0-write��1-read
             void * pBuf:       �����������
             u16 *pwBufLen:     ���ݳ���
  �� �� ֵ�� u16 �ɹ� SNMP_SUCCESS
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2010/03/18  5.0         ½����       ����
  2010/11/23  5.0         ������       �޸�
=============================================================================*/
u16 ProcCallBackMcuPfmInfo(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
    
    u16 wResult = SNMP_SUCCESS;
	BOOL32 bSupportOpr = TRUE;

	*pwBufLen = sizeof(s32);

	TMcuPfmInfo tMcuPfmInfo;
	memset(&tMcuPfmInfo, 0, sizeof(tMcuPfmInfo));
	g_cCfgParse.GetMcuPfmInfo( &tMcuPfmInfo );
    Agtlog(LOG_VERBOSE, "[ProcCallBackMcuPfmInfo] Node(0x%x) received for: %d!\n", dwNodeValue, bRead );

    switch(dwNodeValue) 
    {
    case NODE_MCUPFMCPURATE : 
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)g_cCfgParse.GetCpuRate();
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;
        
    case NODE_MCUPFMMEMORY:  
        
        if( bRead )
        {
            *(s32*)pBuf = g_cCfgParse.GetMemoryRate();
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;
        
    case NODE_MCUPFMMTNUM :  
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tMcuPfmInfo.GetTotalMtNum();
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;
        
    case NODE_MCUPFMCONFNUM:
        if( bRead )
        {
            *(s32*)pBuf = (s32)tMcuPfmInfo.GetConfNum();
        }
        else
        {
            bSupportOpr = FALSE;
        }
		break;

	case NODE_MCUPFMEQPUSEDRATE:  
        
        if( bRead )
        {
			*(u32*)pBuf = SM_STAT_EQP_TYPE_MAX;
			Agtlog(LOG_VERBOSE, "GetEqpBuffer %ld\n", *(u32*)pBuf);

			*pwBufLen = tMcuPfmInfo.GetEqpBuffer((s8 *)pBuf);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

	case NODE_MCUPFMAUTHRATE:
        
        if( bRead )
        {
			u32 dwValue = MAKEDWORD(tMcuPfmInfo.GetAuthMTNum(), tMcuPfmInfo.GetAuthMTTotal());
            *(s32*)pBuf = dwValue;

			Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpNetSync] NODE_MCUPFMAUTHRATE total:(%d); used:(%d)!\n", 
				tMcuPfmInfo.GetAuthMTTotal(), tMcuPfmInfo.GetAuthMTNum());
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;	
		
	case NODE_MCUPFMMACHTEMPER:  
        
        if( bRead )
        {
			//��ʱ�ײ㻹û����صĽӿڣ��ȵײ���ɺ����
			//����ʹ�ã�Ĭ��Ϊ20
            *(s32*)pBuf = 20;
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;	

    // ���� ����MCS��IP��Ϣ  [04/24/2012 liaokang] 
    case NODE_MCUPFMMCSCONNECTIONS:
        
        if( bRead )
        {
            s8 abyBuf[256] = {0};
            abyBuf[255] = '\0';
            BOOL32 nRet;
            u16 wBufLen;
            nRet = g_cCfgParse.GetRecombinedRegedMcsIpInfo( abyBuf, wBufLen );
            if ( !nRet )
            {
                *pwBufLen = 2;
            }
            else
            {
                *pwBufLen = wBufLen;
            }
            memcpy( pBuf, abyBuf, *pwBufLen );
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;	

    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpNetSync] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }

	if ( !bSupportOpr )
    {
        Agtlog(LOG_WARN, "[ProcCallBackMcuPfmInfo] node(0x%x) not support operate for: %d<1.read-0.write> \n", dwNodeValue, bRead);
    }

    return wResult;
}

/*=============================================================================
  �� �� ���� InitalData
  ��    �ܣ� ��ʼ��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
// ע��û�п���ȡ�ź�����ʧ��
=============================================================================*/
BOOL32 CMsgCenterInst::Initialize(void)
{
	printf("CMsgCenterInst::Initialize 1\n");
	OspSemTake( g_hSync );

	BOOL32 bRet = TRUE;
    m_pcSnmpNodes = new(CNodes);
    if( NULL == m_pcSnmpNodes )
    {
        printf("[AgentMsgCenter] Fail to new a CNodes.\n");
        bRet = FALSE;
    }
	else
	{
		m_pcAgentSnmp = new(CAgentSnmp);
		if( NULL == m_pcAgentSnmp )
		{
			printf("[AgentMsgCenter] Fail to new CAgentSnmp.\n");
			if( NULL != m_pcSnmpNodes )
			{
				delete m_pcSnmpNodes;
				m_pcSnmpNodes = NULL;
			}
			bRet = FALSE;
		}
	}

#ifdef _LINUX_

	if(FTPC_OK != FTPCInit()) 
	{         
		printf("FTPCInit ��ʼ��ʧ��\n"); 
    } 

	if (FTPC_OK != FTPCRegCallback(ProcCbFtpcCallbackFun))
	{
		printf("FTPCRegCallback ��ʼ��ʧ��\n"); 
	}
#endif

	if( !bRet )
	{
		printf("\n\n fail to alloc memory for snmp nodes \n\n");
	}
	else
	{
		OspSemGive( g_hSync );	// �ɹ����ͷ��ź���
	}

	printf("CMsgCenterInst::Initialize 2\n");

    return bRet;
}

/*=============================================================================
  �� �� ���� Quit
  ��    �ܣ� �ͷ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMsgCenterInst::Quit(void)
{   
    if( NULL != m_pcSnmpNodes )
	{
        m_pcSnmpNodes->Clear();
		delete m_pcSnmpNodes;
		m_pcSnmpNodes = NULL;
	}

	if( NULL != m_pcAgentSnmp )
    {
		delete m_pcAgentSnmp;
		m_pcAgentSnmp = NULL;
	}

    return TRUE;
}

/*=============================================================================
    ������      ��InstanceEntry
    ����        ��ʵ����Ϣ������ں���������override
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
-------------------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
=============================================================================*/
void CMsgCenterInst::InstanceEntry(CMessage *const pcMsg)
{
	Agtlog(LOG_VERBOSE, "Msg %u(%s) received!\n", pcMsg->event, ::OspEventDesc( pcMsg->event ) );

    if( NULL == pcMsg )
	{
		Agtlog(LOG_ERROR, "[InstanceEntry] Error input parameter!\n");
		return;
	}

    switch( pcMsg->event )
    {
	case SVC_AGT_CONFERENCE_START:
	case SVC_AGT_CONFERENCE_STOP:
	case SVC_AGT_ADD_MT:		              
	case SVC_AGT_DEL_MT:                    
		ProcConfStatusChange( pcMsg );
		break;

	case SVC_AGT_POWER_STATUS:				  // ��Դ״̬�ı�
		ProcPowerStatusChange( pcMsg );
		break;

	case SVC_AGT_POWER_FAN_STATUS:            // ��Դ����״̬�ı�
		ProcPowerFanStatusChange( pcMsg );
		break;

	case SVC_AGT_BOX_FAN_STATUS:              // �������״̬�ı�  [miaoqingsong add]
        ProcBoxFanStatusChange( pcMsg );
		break;

	case SVC_AGT_CPU_STATUS:                  // MPC��CPU״̬�ı�  [miaoqingsong add]
		ProcMPCCpuStatusChange( pcMsg );    
		break;

	case SVC_AGT_MPCMEMORY_STATUS:            // MPC��Memory״̬�ı� [miaoqingsong add]
		ProcMPCMemoryStatusChange( pcMsg );  
		break;

	case SVC_AGT_CPU_FAN_STATUS:
		ProcCpuFanStatusChange( pcMsg );	  // CPU����״̬�ı�
		break;

	case SVC_AGT_CPUTEMP_STATUS:
		ProcCpuTempStatusChange( pcMsg );	  // CPU�¶�״̬�ı�
		break;

	case SVC_AGT_POWERTEMP_STATUS:            // ��Դ���¶�״̬�ı� [miaoqingsong add]
		ProcPowerTempStatusChange( pcMsg );
		break;

	case SVC_AGT_MPC2TEMP_STATUS:             // MPC2���¶ȹ���״̬�ı� [miaoqingsong add]
		ProcMpc2TempStatusChange( pcMsg );
		break;

	case SVC_AGT_MODULE_STATUS:				  // ģ��״̬�ı�(DIC CRI�����)
		ProcModuleStatusChange( pcMsg );
		break;

    case SVC_AGT_LED_STATUS:				  // �ɵ�������������
        ProcLedStatusChange(pcMsg);
        break;
		
	case SVC_AGT_SDH_STATUS:				 // SDH״̬�ı䣬�ɵ�������������
		ProcSDHStatusChange( pcMsg );
		break;

    case MCU_AGT_BOARD_STATUSCHANGE:		 // ����״̬�ı�
    case SVC_AGT_STANDBYMPCSTATUS_NOTIFY:    // ��MPC״̬�ı�        
		ProcBoardStatusChange( pcMsg );
        break;

    case MCU_AGT_LINK_STATUSCHANGE:			 // ��·״̬�ı�
		ProcLinkStatusChange( pcMsg );
        break;

	case SVC_AGT_BRD_TEMPERATURE_STATUS_NOTIFY:         //�����¶ȱ仯��Ŀǰ����֧��DRI2
		ProcBoardTempStatusChange(pcMsg);
		break;

	case SVC_AGT_BRD_CPU_STATUS_NOTIF:		 //����CPU״̬�ı䣬Ŀǰ֧��DRI��DRI2
		ProcBoardCpuStatusChange(pcMsg);
		break;

    case MCU_AGT_SYNCSOURCE_STATUSCHANGE:    // ͬ��Դ״̬�ı�
		ProcNetSyncStatusChange( pcMsg );
        break;

	case MCU_BRD_FAN_STATUS:
		ProcBrdFanStatusChange( pcMsg );	 // Imt����״̬�ı�
		break;

    case SVC_AGT_MEMORY_STATUS:				 // �ڴ�״̬�ı�
    case SVC_AGT_FILESYSTEM_STATUS:			 // �ļ�ϵͳ״̬�ı�
	case SVC_AGT_TASK_STATUS:				 // ����״̬�ı�
		ProcSoftwareStatusChange( pcMsg );
		break;

	case AGT_SVC_POWEROFF:					 // ҵ�񷢹����Ĵ�����Ϣ
	case AGT_SVC_POWERON:					 // ҵ�񷢹����ĴӴ�����������Ϣ
    case SVC_AGT_COLD_RESTART:				 // ���������
    case SVC_AGT_POWEROFF:					 // �ػ�
		ProcPowerOnOffStatusChange( pcMsg );
        break;

	case SVC_AGT_CONFIG_ERROR:
        ProcGetConfigError();				 // �������ļ�������Ϣ
        break;
#ifdef _MINIMCU_
	case SVC_AGT_SETDSCINFO_REQ:			 // ����DSC info, zgc, 2007-07-17
		ProcSetDscInfoReq( pcMsg );
		break;

	case AGT_SVC_DSCREGSUCCEED_NOTIF:		 // ֪ͨMCU DSC���Ѿ�ע��ɹ�, zgc, 2007-07-31
		ProcDscRegSucceedNotif( pcMsg );
		break;

	case BOARD_MPC_SETDSCINFO_ACK:
	case BOARD_MPC_SETDSCINFO_NACK:
		ProcSetDscInfoRsp( pcMsg );
		break;
#endif

    case EV_MSGCENTER_POWER_ON:
		Initialize();						 // ��ʼ������
		break;

    case EV_TOMSGCENTER_INITAL_SNMPINFO:
        ProcSnmpInfo();						 // ��ʼ��Snmp����
        break;

    case EV_MSGCENTER_POWER_OFF:
        ProcPowerOffAlarm();				 // �ػ��澯(�����˳�ʱ����)
        break;

    case EV_BOARD_LED_ALARM:                 // �������������Led�澯
    case EV_BOARD_ETHPORT_ALARM:			 // ����������������ڸ澯
        /*ProcLedAlarm( pcMsg );*/
        ProcBoardStatusAlarm( pcMsg );       // �������赥��澯��Led/����״̬�� [05/04/2012 liaokang]
        break;

    case EV_AGENT_COLD_RESTART:
        ProcColdStartdAlarm();
        break;
        
    // xsl [8/17/2006] mcu֪ͨ������µ���汾�������Լ���
    case SVC_AGT_UPDATEBRDVERSION_CMD:
        ProcMcsUpdateBrdCmd(pcMsg);
        break;

#ifdef _MINIMCU_
	case SVC_AGT_STARTUPDATEDSCVERSION_REQ:
	case SVC_AGT_DSCUPDATEFILE_REQ:
		ProcMcsUpdateDscMsg(pcMsg);
		break;
#endif

	case SVC_AGT_DISCONNECTBRD_CMD:
	case SVC_AGT_BOARDCFGMODIFY_NOTIF:
#ifdef _MINIMCU_
	case SVC_AGT_DSCTELNETLOGININFO_UPDATE_CMD:
	case SVC_AGT_DSCGKINFO_UPDATE_CMD:
#endif	
		ProcMsgToBrdManager(pcMsg);
		break;

    case EV_MSGCENTER_NMS_UPDATEDMPC_CMD:
        ProcNMSUpdateBrdCmd( pcMsg );
        break;

    case EV_SM_UPDATEDMPC_CMD:
        ProcSmUpdateBrdCmd( );
        break;

	case MCUAGENT_UPDATE_MCUPFMINFO_TIMER:
		ProcUpdateMcuPfmInfoTimer();
		break;

	case MCUAGENT_OPENORCLOSE_MPCLED_TIMER:
		ProcOpenorcloseMpcLedTimer();
		break;

	/*case SVC_AGT_BRD_SOFTWARE_VERSION_NOTIFY:
		ProcBoardSoftwareVersionNotif(pcMsg);
		break;
	*/

    default:
        Agtlog(LOG_ERROR, "[InstanceEntry] unknown message %u(%d) received!\n", 
			                    pcMsg->event, ::OspEventDesc( pcMsg->event ) );
        break;
    }

    return;
}

/*=============================================================================
  �� �� ���� ProcUpdateMcuPfmInfoTimer
  ��    �ܣ� ����MCU������Ϣ��ʱTrap�ϱ�������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pcMsg
  �� �� ֵ�� void
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���           �޸�����
  2010/03/18  5.0         ½����             ����
  2010/11/23  5.0         ������             �޸�
=============================================================================*/
void CMsgCenterInst::ProcUpdateMcuPfmInfoTimer( void )
{
	if (!m_pcSnmpNodes) return;

	TMcuPfmInfo tMcuPfmInfo;
	memset(&tMcuPfmInfo, 0, sizeof(tMcuPfmInfo));
	g_cCfgParse.GetMcuPfmInfo( &tMcuPfmInfo );

	u32 dwCpuRate = (u32)g_cCfgParse.GetCpuRate();          
	u32 dwMemoryRate = g_cCfgParse.GetMemoryRate();
	u32 dwMtNum = (u32)tMcuPfmInfo.GetTotalMtNum();
	u32 dwConfNum = (u32)tMcuPfmInfo.GetConfNum();

	s8 aszEqpBuff[AGENT_MAX_EQPNODEBUFF_LEN] = {0};
	u16 wBuffSize = tMcuPfmInfo.GetEqpBuffer(aszEqpBuff);
    
	u32 dwAuthMTRate = MAKEDWORD(tMcuPfmInfo.GetAuthMTNum(),tMcuPfmInfo.GetAuthMTTotal());

	time_t tTime = time(NULL);

	m_pcSnmpNodes->Clear();
	m_pcSnmpNodes->SetTrapType( TRAP_MCU_PFMINFO );

	// [20101123 miaoqingsong] ���CPU �ڴ�ʹ���ʺͽ�����Ȩ��ռ����Trap��Ϣ�ϱ�
	m_pcSnmpNodes->AddNodeValue(NODE_MCUPFMCPURATE, &dwCpuRate, sizeof(u32));
	m_pcSnmpNodes->AddNodeValue(NODE_MCUPFMMEMORY, &dwMemoryRate, sizeof(u32));
	m_pcSnmpNodes->AddNodeValue(NODE_MCUPFMMTNUM, &dwMtNum, sizeof(u32));
	m_pcSnmpNodes->AddNodeValue(NODE_MCUPFMCONFNUM, &dwConfNum, sizeof(u32));
	m_pcSnmpNodes->AddNodeValue(NODE_MCUPFMEQPUSEDRATE, aszEqpBuff, wBuffSize);
	m_pcSnmpNodes->AddNodeValue(NODE_MCUPFMAUTHRATE, &dwAuthMTRate, sizeof(u32));
	m_pcSnmpNodes->AddNodeValue(NODE_MCUPFMTIME, &tTime, sizeof(u32));

	if(m_pcAgentSnmp)
		m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );

    // [20101123 miaoqingsong] ���ö�ʱ��ÿ60s��Trap��ʽ�����ܷ�һ���豸������Ϣ
	SetTimer( MCUAGENT_UPDATE_MCUPFMINFO_TIMER, UPDATE_PFMINFO_TIME_OUT ); 
}

/*=============================================================================
  �� �� ����ProcOpenorcloseMpcLedTimer
  ��    �ܣ�����MPC��NMS��״̬
  �㷨ʵ�֣�
  ȫ�ֱ�����BOOL32    g_bNmsLedState
            BOOL32    g_bNmsOnline
  ��    ����CMessage* const pcMsg
  �� �� ֵ��void
  ----------------------------------------------------------------------------
  �޸ļ�¼��
  ��   ��        �汾        �޸���        �޸�����
  2010/11/18     5.0         ������          ����
=============================================================================*/
void CMsgCenterInst::ProcOpenorcloseMpcLedTimer( void )
{
	if ( TRUE == g_bNmsOnline )
	{
		if( !g_bNmsLedState)
		{
			if ( OK == BrdLedStatusSet(LED_SYS_LINK, BRD_LED_ON) )
			{
				g_bNmsLedState = TRUE;
			}
		}
	}
	else 
	{
		if ( OK == BrdLedStatusSet(LED_SYS_LINK, BRD_LED_OFF) )
		{
			g_bNmsLedState = FALSE;
		}
	}

	g_bNmsOnline = FALSE;
	
	// ��ʱɨ��Get���ûص����������
	SetTimer( MCUAGENT_OPENORCLOSE_MPCLED_TIMER, OPENORCLOSE_MPCLED_TIME_OUT );  
}

/*=============================================================================
  �� �� ���� ProcSnmpInfo
  ��    �ܣ� ��ʼ��Snmp����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CMsgCenterInst::ProcSnmpInfo()
{
	printf("CMsgCenterInst::ProcSnmpInfo 1\n");
	
	TSnmpAdpParam tSnmpParam;
	g_cCfgParse.GetSnmpParam( tSnmpParam );
    
	// [2010/03/10 miaoqingsong] ����豸�����������⣬���µ����ص������ĵ���˳��
	if( m_pcAgentSnmp && SNMP_SUCCESS == m_pcAgentSnmp->Initialize( AgentCallBack, &tSnmpParam ) )
	{
			//m_pcAgentSnmp->SetAgentCallBack( AgentCallBack );
			printf("[ProcSnmpInfo] initialize snmp succeed!\n");
			Agtlog(LOG_ERROR, "[ProcSnmpInfo] initialize snmp succeed!\n");
	}
	else
	{
			printf("[ProcSnmpInfo] fail to initialize snmp .\n");
			Agtlog(LOG_ERROR, "[ProcSnmpInfo] fail to initialize snmp .\n");
	}	
    
	printf("CMsgCenterInst::ProcSnmpInfo 2\n");
    return;
}

/*=============================================================================
  �� �� ���� ProcConfStatusChange
  ��    �ܣ� MCU���״̬Trap��Ϣ�ϱ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��        �汾       �޸���            �޸�����
  2010/11/23    5.0        ������              ����(MT���״̬��Ϣ�����ն��Լ���)
================================================================================*/
void CMsgCenterInst::ProcConfStatusChange( CMessage* const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcConfStatusChange] The pointer can not be Null\n");
        return;
    }
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcConfStatusChange] There's no NMS configed !\n");
        return;
    }
    
	u8              byIndex = 0;
	u32             dwState = 0;
    s8              achConfId[32];
    time_t          tCurTime;
    struct tm       *ptTime = NULL;
	TMcuConfState   tConfState;
	TMcuMtConfState tMtConfState;
	TConfNotify     *ptConfNotify = NULL;
	TMtNotify       *ptMtNotify = NULL;
	CConfId         cConfId;
    
    memset(achConfId, 0, sizeof(achConfId));
    memset(&cConfId, 0, sizeof(CConfId));
    memset( &tConfState, 0, sizeof(tConfState) );
	memset( &tMtConfState, 0, sizeof(tMtConfState) );

	TMcuPfmInfo* ptMcuPfmInfo = g_cCfgParse.GetMcuPfmInfo();
		
	switch( pcMsg->event )
    {
	case SVC_AGT_CONFERENCE_START:
		{
			
			ptConfNotify = ( TConfNotify* )pcMsg->content;
			if (NULL == ptConfNotify) break;
			
			u8 byConfNtyconfIdLen = sizeof(ptConfNotify->m_abyConfId);
			u8 byConfStateConfIdLen = sizeof(tConfState.m_abyConfId);
			memcpy( tConfState.m_abyConfId, ptConfNotify->m_abyConfId, min(byConfNtyconfIdLen, byConfStateConfIdLen) );
			//[5/16/2013 liaokang] ת��
            //strncpy( (s8*)tConfState.m_abyConfName, (s8*)ptConfNotify->m_abyConfName, sizeof( tConfState.m_abyConfName ) );            
            //tConfState.m_abyConfName[ sizeof(tConfState.m_abyConfName) - 1 ] = '\0';
            s8 achEncodingBuf[MAXLEN_CONFNAME] = {0};
            if( FALSE == TransEncodingForNmsData((s8*)ptConfNotify->m_abyConfName, achEncodingBuf, sizeof(achEncodingBuf)) )
            {
                strncpy(achEncodingBuf, (s8*)ptConfNotify->m_abyConfName, sizeof(achEncodingBuf)- 1);
            }
            strncpy( (s8*)tConfState.m_abyConfName, achEncodingBuf, sizeof(tConfState.m_abyConfName) - 1 );            

			tCurTime = time(NULL);
			ptTime = localtime( &tCurTime );
			sprintf((s8*)tConfState.m_abyTime, "%4.4u-%2.2u-%2.2u %2.2u:%2.2u:%2.2u", ptTime->tm_year + 1900, 
				ptTime->tm_mon + 1, ptTime->tm_mday, ptTime->tm_hour, ptTime->tm_min, ptTime->tm_sec );
			tConfState.m_byState = CONF_STATE_START;
			if(m_pcSnmpNodes)
			{
				m_pcSnmpNodes->Clear();
				m_pcSnmpNodes->SetTrapType( TRAP_CONF_STATE );
			}
			dwState = tConfState.m_byState;
			cConfId.SetConfId(tConfState.m_abyConfId, sizeof(tConfState.m_abyConfId));
			cConfId.GetConfIdString(achConfId, sizeof(achConfId));
			
			if(m_pcSnmpNodes)
			{
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFSTATENOTIFY, &dwState, sizeof(u32) );
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFIDNOTIFY, achConfId, sizeof(achConfId) );
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFNAMENOTIFY, tConfState.m_abyConfName, sizeof(tConfState.m_abyConfName) );
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFSTARTTIMENOTIFY, tConfState.m_abyTime, sizeof(tConfState.m_abyTime) );
				
				if(m_pcAgentSnmp)
				{
					m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );
				}
			}
			
			if (ptMcuPfmInfo && !ptMcuPfmInfo->FindConf(cConfId, byIndex))
			{
				ptMcuPfmInfo->AddConf(cConfId);
			}
		}
		
		break;
		
	case SVC_AGT_CONFERENCE_STOP:
		{
			ptConfNotify = ( TConfNotify* )pcMsg->content;
			if (NULL == ptConfNotify) break;
			
			u8 byConfNtyconfIdLen = sizeof(ptConfNotify->m_abyConfId);
			u8 byConfStateConfIdLen = sizeof(tConfState.m_abyConfId);
			memcpy( tConfState.m_abyConfId, ptConfNotify->m_abyConfId, min(byConfNtyconfIdLen, byConfStateConfIdLen) );
            //[5/16/2013 liaokang] ת��
            //strncpy( (s8*)tConfState.m_abyConfName, (s8*)ptConfNotify->m_abyConfName, sizeof( tConfState.m_abyConfName ) );            
            //tConfState.m_abyConfName[ sizeof(tConfState.m_abyConfName) - 1 ] = '\0';
            s8 achEncodingBuf[MAXLEN_CONFNAME] = {0};
            if( FALSE == TransEncodingForNmsData((s8*)ptConfNotify->m_abyConfName, achEncodingBuf, sizeof(achEncodingBuf)) )
            {
                strncpy(achEncodingBuf, (s8*)ptConfNotify->m_abyConfName, sizeof(achEncodingBuf) - 1);
            }
            strncpy( (s8*)tConfState.m_abyConfName, achEncodingBuf, sizeof(tConfState.m_abyConfName) - 1 );            

			tCurTime = time(NULL);
			ptTime = localtime( &tCurTime );
			sprintf((s8*)tConfState.m_abyTime, "%4.4u-%2.2u-%2.2u %2.2u:%2.2u:%2.2u", ptTime->tm_year + 1900, 
				ptTime->tm_mon + 1, ptTime->tm_mday, ptTime->tm_hour, ptTime->tm_min, ptTime->tm_sec );
			tConfState.m_byState = CONF_STATE_STOP;

			if(m_pcSnmpNodes)
			{
				m_pcSnmpNodes->Clear();
				m_pcSnmpNodes->SetTrapType( TRAP_CONF_STATE );
			}
			dwState = tConfState.m_byState;
			cConfId.SetConfId(tConfState.m_abyConfId, sizeof(tConfState.m_abyConfId));
			cConfId.GetConfIdString(achConfId, sizeof(achConfId));
			
			if(m_pcSnmpNodes)
			{
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFSTATENOTIFY, &dwState, sizeof(u32) );
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFIDNOTIFY, achConfId, sizeof(achConfId) );
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFNAMENOTIFY, tConfState.m_abyConfName, sizeof(tConfState.m_abyConfName) );
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFSTARTTIMENOTIFY, tConfState.m_abyTime, sizeof(tConfState.m_abyTime) );
				
				if(m_pcAgentSnmp)
				{
					m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );
				}
			}
			ptMcuPfmInfo->DelConf(cConfId);
		}
		break;
    
	case SVC_AGT_ADD_MT:		
		{
			ptMtNotify = ( TMtNotify* )pcMsg->content;
			
			u8 byMtNtyconfIdLen = sizeof(ptMtNotify->m_abyConfId);
			u8 byMtConfStateConfIdLen = sizeof(tMtConfState.m_abyConfId);
			memcpy( tMtConfState.m_abyConfId, ptMtNotify->m_abyConfId, min(byMtNtyconfIdLen, byMtConfStateConfIdLen) );
			
            //[5/16/2013 liaokang] ת��
// 			strncpy( (s8*)tMtConfState.m_abyMtAlias, (s8*)ptMtNotify->m_abyMtAlias, sizeof(tMtConfState.m_abyMtAlias) );
// 			tMtConfState.m_abyMtAlias[ sizeof(tMtConfState.m_abyMtAlias) - 1 ] = '\0';
            s8 achEncodingBuf[MAXLEN_ALIAS+1] = {0};
            if( FALSE == TransEncodingForNmsData((s8*)ptMtNotify->m_abyMtAlias, achEncodingBuf, sizeof(achEncodingBuf)) )
            {
                strncpy(achEncodingBuf, (s8*)ptMtNotify->m_abyMtAlias, sizeof(achEncodingBuf) - 1);
            }
			strncpy( (s8*)tMtConfState.m_abyMtAlias, achEncodingBuf, sizeof(tMtConfState.m_abyMtAlias) - 1 );
			
			tCurTime = time( 0 );
			ptTime = localtime( &tCurTime );
			sprintf((s8*)tMtConfState.m_abyTime, "%4.4u-%2.2u-%2.2u %2.2u:%2.2u:%2.2u", ptTime->tm_year + 1900, 
				ptTime->tm_mon + 1, ptTime->tm_mday, ptTime->tm_hour, ptTime->tm_min, ptTime->tm_sec );
			
			tMtConfState.m_byState = CONF_ADD_TERMINAL;
			
			if (NULL != m_pcSnmpNodes)
			{
				m_pcSnmpNodes->Clear();
				m_pcSnmpNodes->SetTrapType(TRAP_MT_STATE);
			}
			
			
			dwState = tMtConfState.m_byState;
			cConfId.SetConfId(tMtConfState.m_abyConfId, sizeof(tMtConfState.m_abyConfId));
			cConfId.GetConfIdString(achConfId, sizeof(achConfId));
			
			if (NULL != m_pcSnmpNodes)
			{
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFIDNOTIFY, achConfId, sizeof(achConfId) );
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFSTARTTIMENOTIFY, tMtConfState.m_abyTime, sizeof(tMtConfState.m_abyTime) );
				m_pcSnmpNodes->AddNodeValue(NODE_MTSTATENOTIFY, &dwState, sizeof(u32) );
				m_pcSnmpNodes->AddNodeValue(NODE_MTALIASNOTIFY, tMtConfState.m_abyMtAlias, sizeof(tMtConfState.m_abyMtAlias) );
				
				if (NULL != m_pcAgentSnmp)
				{
					m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );
				}
			}
			
			if (ptMcuPfmInfo->FindConf(cConfId, byIndex))
			{
				ptMcuPfmInfo->IncMtNum(byIndex);
			}
		}

		break;

	case SVC_AGT_DEL_MT:
		{
			ptMtNotify = ( TMtNotify* )pcMsg->content;
			
			u8 byMtNtyconfIdLen = sizeof(ptMtNotify->m_abyConfId);
			u8 byMtConfStateConfIdLen = sizeof(tMtConfState.m_abyConfId);
			memcpy( tMtConfState.m_abyConfId, ptMtNotify->m_abyConfId, min(byMtNtyconfIdLen, byMtConfStateConfIdLen) );
			
            //[5/16/2013 liaokang] ת��
            // 			strncpy( (s8*)tMtConfState.m_abyMtAlias, (s8*)ptMtNotify->m_abyMtAlias, sizeof(tMtConfState.m_abyMtAlias) );
            // 			tMtConfState.m_abyMtAlias[ sizeof(tMtConfState.m_abyMtAlias) - 1 ] = '\0';
            s8 achEncodingBuf[MAXLEN_ALIAS+1] = {0};
            if( FALSE == TransEncodingForNmsData((s8*)ptMtNotify->m_abyMtAlias, achEncodingBuf, sizeof(achEncodingBuf)) )
            {
                strncpy(achEncodingBuf, (s8*)ptMtNotify->m_abyMtAlias, sizeof(achEncodingBuf) - 1 );
            }
            strncpy( (s8*)tMtConfState.m_abyMtAlias, achEncodingBuf, sizeof(tMtConfState.m_abyMtAlias) - 1 );
			
			tCurTime = time( 0 );
			ptTime = localtime( &tCurTime );
			sprintf((s8*)tMtConfState.m_abyTime, "%4.4u-%2.2u-%2.2u %2.2u:%2.2u:%2.2u", ptTime->tm_year + 1900, 
				ptTime->tm_mon + 1, ptTime->tm_mday, ptTime->tm_hour, ptTime->tm_min, ptTime->tm_sec );
			
			tMtConfState.m_byState = CONF_DEL_TERMINAL;

			if (NULL != m_pcSnmpNodes)
			{
				m_pcSnmpNodes->Clear();
				m_pcSnmpNodes->SetTrapType(TRAP_MT_STATE);
			}
			
			dwState = tMtConfState.m_byState;
			cConfId.SetConfId(tMtConfState.m_abyConfId, sizeof(tMtConfState.m_abyConfId));
			cConfId.GetConfIdString(achConfId, sizeof(achConfId));
			
			if (NULL != m_pcSnmpNodes)
			{
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFIDNOTIFY, achConfId, sizeof(achConfId) );
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFSTARTTIMENOTIFY, tMtConfState.m_abyTime, sizeof(tMtConfState.m_abyTime) );
				m_pcSnmpNodes->AddNodeValue(NODE_MTSTATENOTIFY, &dwState, sizeof(u32) );
				m_pcSnmpNodes->AddNodeValue(NODE_MTALIASNOTIFY, tMtConfState.m_abyMtAlias, sizeof(tMtConfState.m_abyMtAlias) );

				if (NULL != m_pcAgentSnmp)
				{
					m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );
				}
			}
			
			if (ptMcuPfmInfo->FindConf(cConfId, byIndex))
			{
				ptMcuPfmInfo->DecMtNum(byIndex);
			}
		}
		break;

    default:
        Agtlog(LOG_ERROR, "[ProcConfStatusChange] unexpected msg%d<%s> received!\n", 
                                          pcMsg->event, OspEventDesc(pcMsg->event));
        break;
	}

	return ;
}

/*=============================================================================
  �� �� ���� ProcPowerStatusChange
  ��    �ܣ� �����ϵ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMsgCenterInst::ProcPowerStatusChange( CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcPowerStatusChange] The pointer1 can not be Null\n");
        return;
    }
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcPowerStatusChange] There's no NMS configed !\n");
        return;
    }

    u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));
    TMcupfmAlarmEntry    tAlarmData;
	TPowerStatus* ptPowerStatus = NULL;

	switch(pcMsg->event)
	{
	case SVC_AGT_POWER_STATUS:
		ptPowerStatus = (TPowerStatus*)pcMsg->content;
        abyAlarmObj[0] = ( u8 )ptPowerStatus->bySlot;
        abyAlarmObj[1] = ( u8 )ptPowerStatus->byType;
        abyAlarmObj[2] = 0;
        abyAlarmObj[3] = 0;
        abyAlarmObj[4] = 0;

        if( g_cAlarmProc.FindAlarm( ALARM_MCU_POWER_ABNORMAL, ALARMOBJ_MCU_POWER, abyAlarmObj, &tAlarmData ) )
        {
            if( 0 == ptPowerStatus->byStatus)      // ����
            {
                if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                {
                    Agtlog(LOG_WARN, "[ProcPowerStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                                        tAlarmData.m_dwAlarmCode, 
                                        tAlarmData.m_achObject[0], 
                                        tAlarmData.m_achObject[1] );
                }
                else
                {
                    if (SNMP_SUCCESS != SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcPowerStatusChange] power status trap<%d> send failed !\n", TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcPowerStatusChange] power status trap<%d> send succeed !\n", TRAP_ALARM_RESTORE);
                    }
                }
            }
        }
        else        // �澯������
        {
            if( 0 != ptPowerStatus->byStatus )     // �쳣
            {
                if( !g_cAlarmProc.AddAlarm( ALARM_MCU_POWER_ABNORMAL, ALARMOBJ_MCU_POWER, abyAlarmObj, &tAlarmData ) )
                {
                    Agtlog(LOG_WARN, "[ProcPowerStatusChange] McuAgent: AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                                        ALARM_MCU_POWER_ABNORMAL, abyAlarmObj[0], abyAlarmObj[1] );
                }
                else
                {   
                    if (SNMP_SUCCESS != SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcPowerStatusChange] power status trap<%d> send failed !\n", TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcPowerStatusChange] power status trap<%d> send succeed !\n", TRAP_ALARM_GENERATE);
                    }
                }
            }
        }
        break;

	default:
        Agtlog(LOG_ERROR, "[ProcPowerOffStatusChange] unexpected msg%d<%s> received!\n", pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}

	return ;
}

/*=============================================================================
  �� �� ���� ProcModuleStatusChange
  ��    �ܣ� ����ģ��״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMsgCenterInst::ProcModuleStatusChange( CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcModuleStatusChange] The pointer can not be Null\n");
        return;
    }
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcModuleStatusChange] There's no NMS configed !\n");
        return;
    }

    TModuleStatus *ptModuleStatus = NULL;
    u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    TMcupfmAlarmEntry     tAlarmData;
	u16    wLoop = 0;

    u16   awModuleAlarmCode[] = { ALARM_MCU_MODULE_OFFLINE, 
                                   ALARM_MCU_MODULE_CONFLICT,
								   ALARM_MCU_MODULE_ERROR };
    u8   abyModuleAlarmBit[] = { 0x01, 0x02, 0x04 };
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));

	switch(pcMsg->event)
	{
	case SVC_AGT_MODULE_STATUS:
		ptModuleStatus = ( TModuleStatus * )pcMsg->content;
        abyAlarmObj[0] = ( u8 )ptModuleStatus->byLayer;
        abyAlarmObj[1] = ( u8 )ptModuleStatus->bySlot;
        abyAlarmObj[2] = ( u8 )ptModuleStatus->byType;
        abyAlarmObj[3] = ( u8 )ptModuleStatus->byModuleId;
        abyAlarmObj[4] = 0;

        for( wLoop = 0; wLoop < sizeof( awModuleAlarmCode ) / sizeof( awModuleAlarmCode[0] ); wLoop++ )
        {
            if( g_cAlarmProc.FindAlarm( awModuleAlarmCode[wLoop], ALARMOBJ_MCU_MODULE, abyAlarmObj, &tAlarmData ) )
            {
                if( 0 == ( ptModuleStatus->byStatus & abyModuleAlarmBit[wLoop] ))    //normal
                {
                    if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                    {
                            Agtlog(LOG_WARN, "[ProcModuleStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d, %d, %d ) failed!\n", 
                                                tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], 
                                                tAlarmData.m_achObject[1], tAlarmData.m_achObject[2],
							                    tAlarmData.m_achObject[3]);
                    }
                    else
                    {
                        if (SNMP_SUCCESS != SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                        {
                            Agtlog(LOG_ERROR, "[ProcModuleStatusChange] module status trap<%d> send failed !\n", TRAP_ALARM_RESTORE);
                        }
                        else
                        {
                            Agtlog(LOG_INFORM, "[ProcModuleStatusChange] module status trap<%d> send succeed !\n", TRAP_ALARM_RESTORE);
                        }
                    }
                }
            }
            else
            {
                if( 0 != ( ptModuleStatus->byStatus & abyModuleAlarmBit[wLoop] ) )    // abnormal
                {
                    if( !g_cAlarmProc.AddAlarm( awModuleAlarmCode[wLoop], ALARMOBJ_MCU_MODULE, abyAlarmObj, &tAlarmData ) )
                    {
                        Agtlog(LOG_ERROR, "[ProcModuleStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d, %d, %d) failed!\n", 
                               awModuleAlarmCode[wLoop], abyAlarmObj[0], abyAlarmObj[1], abyAlarmObj[2], abyAlarmObj[3] );
                    }
                    else
                    {
                        if (SNMP_SUCCESS != SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                        {
                            Agtlog(LOG_ERROR, "[ProcModuleStatusChange] module status trap<%d> send failed !\n", TRAP_ALARM_GENERATE);
                        }
                        else
                        {
                            Agtlog(LOG_INFORM, "[ProcModuleStatusChange] module status trap<%d> send succeed !\n", TRAP_ALARM_GENERATE);
                        }
                    }
                }
            }
        }
        break;
	default:
        Agtlog(LOG_ERROR, "[ProcModuleStatusChange] unexpected msg%d<%s> received!\n", pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	
	return ;
}

/*=============================================================================
  �� �� ���� SetBoardLedState
  ��    �ܣ� ����Led״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byLayer, 
             u8 bySlot, 
             u8 byType 
             s8 * pchLedState
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMsgCenterInst::SetBoardLedState( u8 byLayer, u8 bySlot, u8 byType , s8* pszLedState )
{
    if( NULL == pszLedState)
    {
        Agtlog(LOG_ERROR, "[SetBoardLedState] The pointer can not be Null\n");
        return FALSE;
    }
    TBrdPosition tPos;
    tPos.byBrdLayer = byLayer;
    tPos.byBrdSlot  = bySlot;
    tPos.byBrdID    = byType;

	u8 byRow = g_cCfgParse.GetBoardRow(byLayer, bySlot, byType);
    if( ERR_AGENT_BRDNOTEXIST == byRow )
    {
        Agtlog(LOG_WARN, "[SetBoardLedState] Cannot Get Board Row by Layer.%d, Slot.%d !\n", 
			byLayer, bySlot);
        return FALSE;
    }

	TEqpBrdCfgEntry tBrdCfg;
    memset( &tBrdCfg, 0, sizeof(tBrdCfg) );

    u16 wRet = g_cCfgParse.GetBrdInfoByRow( byRow, &tBrdCfg );

    tBrdCfg.SetPanelLed( pszLedState );

    wRet |= g_cCfgParse.SetBrdInfoByRow( byRow, &tBrdCfg );

	if ( SUCCESS_AGENT != wRet )
    {
        Agtlog(LOG_ERROR, "[SetBoardLedState] operate brd.%d cfg failed !\n", tBrdCfg.GetBrdId());
        return FALSE;
    }

    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[SetBoardLedState] There's no NMS configed !\n");
        return TRUE;    //û��NMS����ȻӦ�ö�����LED״̬����TRUE
    }

	u32 dwLayer = byLayer;
    u32 dwSlot = bySlot;
    u32 dwType = byType;
	if (!m_pcSnmpNodes || !m_pcAgentSnmp) 
	{
		Agtlog(LOG_WARN, "[SetBoardLedState] m_pcSnmpNodes or m_pcAgentSnmp is null!\n");
		return FALSE;
	}

    m_pcSnmpNodes->Clear();
    m_pcSnmpNodes->SetTrapType(TRAP_LED_STATE);
	
    u32 dwNodeName = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUNETBRDCFGLAYER);
    m_pcSnmpNodes->AddNodeValue(dwNodeName, &dwLayer, sizeof(dwLayer) );

    dwNodeName = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUNETBRDCFGSLOT);
    m_pcSnmpNodes->AddNodeValue(dwNodeName, &dwSlot, sizeof(dwSlot) );

    dwNodeName = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUNETBRDCFGTYPE);
    m_pcSnmpNodes->AddNodeValue(dwNodeName, &dwType, sizeof(dwType) );

    dwNodeName = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUNETBRDCFGPANELLED);
    m_pcSnmpNodes->AddNodeValue(dwNodeName, pszLedState, strlen(pszLedState) );

    u32 dwState = tBrdCfg.GetState();
    dwNodeName = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUNETBRDCFGSTATUS);
    m_pcSnmpNodes->AddNodeValue(dwNodeName, &dwState, sizeof(dwState));

    m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );
    Agtlog(LOG_INFORM,  "Trap send succeed, type: %d, bMasterMcu: %d !\n", 
                         TRAP_LED_STATE, g_cCfgParse.IsMpcActive() );
    return TRUE;	
}

/*=============================================================================
  �� �� ���� ProcLedStatusChange
  ��    �ܣ� ����Led״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMsgCenterInst::ProcLedStatusChange( CMessage * const pcMsg )
{
//#ifndef WIN32
    TBrdLedState tBrdLedState = {0};
    
    memcpy(&tBrdLedState, pcMsg->content, pcMsg->length );
    
    TBrdPosition tPos;
    memset( &tPos, 0, sizeof(tPos) );
    
    TMPCInfo tMPCInfo;
    g_cCfgParse.GetMPCInfo( &tMPCInfo );
    tPos.byBrdLayer = tMPCInfo.GetLocalLayer();
    tPos.byBrdSlot  = tMPCInfo.GetLocalSlot();

#ifdef _MINIMCU_
	tPos.byBrdLayer = 0;
	tPos.byBrdSlot  = 0;
#endif


#ifdef _LINUX12_
	//  [1/21/2011 chendaiwei]֧��MPC2
	tPos.byBrdID = BRD_TYPE_MPC2/*DSL8000_BRD_MPC2*/;
    SetBoardLedState( tPos.byBrdLayer, tPos.byBrdSlot, tPos.byBrdID,
                        (s8*)&tBrdLedState.nlunion.tBrdMPC2LedState);
#else
	tPos.byBrdID = BRD_TYPE_MPC/*DSL8000_BRD_MPC*/;
	SetBoardLedState( tPos.byBrdLayer, tPos.byBrdSlot, tPos.byBrdID,
                        (s8*)&tBrdLedState.nlunion.tBrdMPCLedState);
#endif
//#endif
    
    return;
}

/*=============================================================================
  �� �� ���� ProcSDHStatusChange
  ��    �ܣ� ����SDH״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMsgCenterInst::ProcSDHStatusChange( CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcSDHStatusChange] The pointer can not be Null\n");
        return;
    }
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcSDHStatusChange] There's no NMS configed !\n");
        return;
    }
    
    TSDHStatus *ptSDHStatus = NULL;
    u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    TMcupfmAlarmEntry     tAlarmData;
	u16    wLoop;

	// SDH�ĸ澯��
    u16   awSDHAlarmCode[] = { ALARM_MCU_SDH_LOS,      ALARM_MCU_SDH_LOF,
							    ALARM_MCU_SDH_OOF,      ALARM_MCU_SDH_LOM,
							    ALARM_MCU_SDH_LOP,      ALARM_MCU_SDH_MS_RDI,
							    ALARM_MCU_SDH_MS_AIS,   ALARM_MCU_SDH_HP_RDI,
							    ALARM_MCU_SDH_HP_AIS,   ALARM_MCU_SDH_RS_TIM,
							    ALARM_MCU_SDH_HP_TIM,   ALARM_MCU_SDH_HP_UNEQ,
							    ALARM_MCU_SDH_HP_PSLM,  ALARM_MCU_SDH_TU_LOP,
							    ALARM_MCU_SDH_LP_RDI,   ALARM_MCU_SDH_LP_AIS,
							    ALARM_MCU_SDH_LP_TIM,   ALARM_MCU_SDH_LP_PSLM };
	// ����SDH�澯���Ӧ��BITλ
    u32   adwSDHAlarmBit[] = { 0x00000001,   0x00000002,
							    0x00000004,   0x00000008,
							    0x00000010,   0x00000020,
							    0x00000040,   0x00000080,
							    0x00000100,   0x00000200,
							    0x00000400,   0x00000800,
							    0x00001000,   0x00002000,
							    0x00004000,   0x00008000,
							    0x00010000,   0x00020000 };

	memset( abyAlarmObj, 0, sizeof(abyAlarmObj) );
	switch(pcMsg->event)
	{
	case SVC_AGT_SDH_STATUS:
		ptSDHStatus = ( TSDHStatus * )pcMsg->content;
		*(u32*)abyAlarmObj = ptSDHStatus->dwStatus;  // ȡSDH״̬
        for( wLoop = 0; wLoop < sizeof( awSDHAlarmCode ) / sizeof( awSDHAlarmCode[0] ); wLoop++ )
        {
            if( g_cAlarmProc.FindAlarm( awSDHAlarmCode[wLoop], ALARMOBJ_MCU_SDH, abyAlarmObj, &tAlarmData ) )
            {
                if( 0 ==  ( ptSDHStatus->dwStatus & adwSDHAlarmBit[wLoop] ))    // normal
                {
                    if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                    {
                        Agtlog(LOG_ERROR, "[ProcSDHStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d, %d ) failed!\n", 
                                            tAlarmData.m_dwAlarmCode, 
                                            tAlarmData.m_achObject[0], 
                                            tAlarmData.m_achObject[1], 
                                            tAlarmData.m_achObject[2]);
                    }
                    else
                    {
                        if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                        {
                            Agtlog(LOG_ERROR, "[ProcSDHStatusChange] SDH status trap<%d> send failed !\n", TRAP_ALARM_RESTORE);
                        }
                        else
                        {
                            Agtlog(LOG_INFORM, "[ProcSDHStatusChange] SDH status trap<%d> send succeed !\n", TRAP_ALARM_RESTORE);
                        }
                    }
                }
            }
            else        // no such alarm
            {
                if( 0 != ( ptSDHStatus->dwStatus & adwSDHAlarmBit[wLoop] ) )    // abnormal
                {
                    if( !g_cAlarmProc.AddAlarm( awSDHAlarmCode[wLoop], ALARMOBJ_MCU_SDH, abyAlarmObj, &tAlarmData ) )
                    {
                        Agtlog(LOG_ERROR, "[ProcSDHStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                                            awSDHAlarmCode[wLoop], abyAlarmObj[0], abyAlarmObj[1], abyAlarmObj[2] );
                    }
                    else
                    {
                        if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                        {
                            Agtlog(LOG_ERROR, "[ProcSDHStatusChange] SDH status trap<%d> send failed !\n", TRAP_ALARM_GENERATE);
                        }
                        else
                        {
                            Agtlog(LOG_INFORM, "[ProcSDHStatusChange] SDH status trap<%d> send succeed !\n", TRAP_ALARM_GENERATE);
                        }
                    }
                }
            }
        }
        break;

	default:
        Agtlog(LOG_ERROR, "[ProcSDHStatusChange] unexpected msg%d<%s> received!\n", pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	
	return ;
}

/*=============================================================================
  �� �� ���� ProcBoardStatusChange
  ��    �ܣ� ������״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMsgCenterInst::ProcBoardStatusChange( CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcBoardStatusChange] The pointer can not be Null\n");
        return;
    }   

    TBrdStatus  tBrdStatus;
    TBrdStatus* ptBrdStatus = NULL;
	u8		abyAlarmObj[MAX_ALARMOBJECT_NUM];  
    TMcupfmAlarmEntry     tAlarmData;
	u32   dwLoop;
    u16   awBoardAlarmCode[] = { ALARM_MCU_BOARD_DISCONNECT, ALARM_MCU_BOARD_CFGCONFLICT};
    u8    abyBoardAlarmBit[] = { 0x01, 0x02 };

    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));

    if (pcMsg->event == SVC_AGT_STANDBYMPCSTATUS_NOTIFY)
    {
        TMPCInfo tMPCInfo;
        g_cCfgParse.GetMPCInfo( &tMPCInfo );
        
        tBrdStatus.byLayer = tMPCInfo.GetOtherMpcLayer();
        tBrdStatus.bySlot  = tMPCInfo.GetOtherMpcSlot();

		//  [1/21/2011 chendaiwei]֧��MPC2
#ifdef _LINUX12_
        tBrdStatus.byType = BRD_TYPE_MPC2/*DSL8000_BRD_MPC2*/;
#else
        tBrdStatus.byType = BRD_TYPE_MPC/*DSL8000_BRD_MPC*/;
#endif

        tBrdStatus.byStatus = g_cCfgParse.BrdStatusMcuVc2Agt( *pcMsg->content );
        tBrdStatus.byOsType = tMPCInfo.GetOSType();
        
        tMPCInfo.SetOtherMpcStatus( tBrdStatus.byStatus );
        g_cCfgParse.SetMPCInfo( tMPCInfo );
        Agtlog(LOG_INFORM, "[ProcBoardStatusChange] Other MPC Status Changed to %d!\n", 
                                                                 tBrdStatus.byStatus );

        ptBrdStatus = &tBrdStatus;
    }
    else
    {
        ptBrdStatus = ( TBrdStatus * )pcMsg->content;
    }
    
    TBrdPosition tBrdPos;
    tBrdPos.byBrdLayer = ptBrdStatus->byLayer;
    tBrdPos.byBrdSlot  = ptBrdStatus->bySlot;
    tBrdPos.byBrdID    = ptBrdStatus->byType;

    TBoardInfo tBrdInfo;
    g_cCfgParse.GetBrdCfgById( g_cCfgParse.GetBrdId(tBrdPos), &tBrdInfo );

	switch(pcMsg->event)
	{
    // guzh [9/15/2006] ���Ӵ�MSҵ��֪ͨMCU����
    case SVC_AGT_STANDBYMPCSTATUS_NOTIFY:
	case MCU_AGT_BOARD_STATUSCHANGE:        
        abyAlarmObj[0] = ( u8 )ptBrdStatus->byLayer;
        abyAlarmObj[1] = ( u8 )ptBrdStatus->bySlot;
        abyAlarmObj[2] = ( u8 )ptBrdStatus->byType;
        abyAlarmObj[3] = ( u8 )ptBrdStatus->byStatus;
        abyAlarmObj[4] = 0;
        
        if (ptBrdStatus->byOsType == OS_TYPE_WIN32 || 
            ptBrdStatus->byOsType == OS_TYPE_LINUX || 
            ptBrdStatus->byOsType == OS_TYPE_VXWORKS)
        {
            tBrdInfo.SetOSType( ptBrdStatus->byOsType );			
            g_cCfgParse.SetBrdCfgById( tBrdInfo.GetBrdId(), &tBrdInfo );
        }
        else
        {
            // guzh [9/18/2006] ���û������,���ȡԭ����
            ptBrdStatus->byOsType = tBrdInfo.GetOSType();			
        }

		//  [1/21/2011 chendaiwei]֧��MPC2
        if( g_cCfgParse.IsMpcActive() || 
			( ptBrdStatus->byType == BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ || ptBrdStatus->byType == BRD_TYPE_MPC2 ) )
        {
            SendMsgToMcuCfg(AGT_SVC_BOARDSTATUS_NOTIFY, (u8*)ptBrdStatus, sizeof(TBrdStatus));
        }        

        if ( !g_cCfgParse.HasSnmpNms() )
        {
            Agtlog(LOG_WARN, "[ProcBoardStatusChange] There's no NMS configed !\n");
            return;
        }

        // �������������
		// �Ƿ����ɸ澯��澯�ָ�		
        for( dwLoop = 0; dwLoop < sizeof( awBoardAlarmCode ) / sizeof( awBoardAlarmCode[0] ); dwLoop++ )
        {
            if( g_cAlarmProc.FindAlarm( awBoardAlarmCode[dwLoop], ALARMOBJ_MCU_BOARD, abyAlarmObj, &tAlarmData ) )
            {
                if( 0 == ( ptBrdStatus->byStatus & abyBoardAlarmBit[dwLoop] ))    // normal
                {
                    if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                    {
                        Agtlog(LOG_ERROR, "[ProcBoardStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d, %d ) failed!\n", 
                                            tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], 
                                            tAlarmData.m_achObject[1], tAlarmData.m_achObject[2]);
                    }
                    else
                    {
                        if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                        {
                            Agtlog(LOG_ERROR, "[ProcBoardStatusChange] Board<%d,%d:%s> status<%d> trap<%d> send failed !\n",
                                               ptBrdStatus->byLayer, ptBrdStatus->bySlot, 
                                               g_cCfgParse.GetBrdTypeStr(ptBrdStatus->byType), 
                                               ptBrdStatus->byStatus, TRAP_ALARM_RESTORE);
                        }
                        else
                        {
                            Agtlog(LOG_INFORM, "[ProcBoardStatusChange] Board<%d,%d:%s> status<%d> trap<%d> send succeed !\n",
                                                ptBrdStatus->byLayer, ptBrdStatus->bySlot, 
                                                g_cCfgParse.GetBrdTypeStr(ptBrdStatus->byType), 
                                                ptBrdStatus->byStatus, TRAP_ALARM_RESTORE);
                        }
                    }
                }
            }
            else        //no such alarm
            {
                if( 0 != ( ptBrdStatus->byStatus & abyBoardAlarmBit[dwLoop] ))    //abnormal
                {
                    if( !g_cAlarmProc.AddAlarm( awBoardAlarmCode[dwLoop], ALARMOBJ_MCU_BOARD, abyAlarmObj, &tAlarmData ) )
                    {
                        Agtlog(LOG_ERROR, "[ProcBoardStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                            awBoardAlarmCode[dwLoop], abyAlarmObj[0], abyAlarmObj[1], abyAlarmObj[2] );
                    }
                    else
                    {
                        if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                        {
                            Agtlog(LOG_ERROR, "[ProcBoardStatusChange] Board<%d,%d:%s> status<%d> trap<%d> send failed !\n",
                                                ptBrdStatus->byLayer, ptBrdStatus->bySlot, 
                                                g_cCfgParse.GetBrdTypeStr(ptBrdStatus->byType),
                                                ptBrdStatus->byStatus, TRAP_ALARM_GENERATE);
                        }
                        else
                        {
                            Agtlog(LOG_INFORM, "[ProcBoardStatusChange] Board<%d,%d:%s> status<%d> trap<%d> send succeed !\n",
                                                ptBrdStatus->byLayer, ptBrdStatus->bySlot, 
                                                g_cCfgParse.GetBrdTypeStr(ptBrdStatus->byType), 
                                                ptBrdStatus->byStatus, TRAP_ALARM_GENERATE);
                        }
                    }
                }
            }
        }
        break;

	default:
        Agtlog(LOG_ERROR, "[ProcBoardStatusChange] unexpected msg%d<%s> received!\n", pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}

	return ;
}

/*=============================================================================
  �� �� ���� ProcLinkStatusChange
  ��    �ܣ� ������·״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMsgCenterInst::ProcLinkStatusChange( CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcLinkStatusChange] The pointer can not be Null\n");
        return;
    }
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcLinkStatusChange] There's no NMS configed !\n");
        return;
    }

    TLinkStatus *ptLinkStatus = NULL;
    u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    TMcupfmAlarmEntry     tAlarmData;
	u16    wLoop;
    u16    awE1AlarmCode[] = { ALARM_MCU_BRD_E1_RCMF, ALARM_MCU_BRD_E1_RMF,
							   ALARM_MCU_BRD_E1_RUA1, ALARM_MCU_BRD_E1_RRA, 
							   ALARM_MCU_BRD_E1_LOC,  ALARM_MCU_BRD_E1_RLOS };
    u8   abyE1AlarmBit[] = { 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
    BOOL32 bAlarmResult = FALSE;
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));

	switch(pcMsg->event)
	{
	case MCU_AGT_LINK_STATUSCHANGE:
		ptLinkStatus = ( TLinkStatus * )pcMsg->content;
        abyAlarmObj[0] = ( u8 )ptLinkStatus->byLayer;
        abyAlarmObj[1] = ( u8 )ptLinkStatus->bySlot;
        abyAlarmObj[2] = ( u8 )ptLinkStatus->byType;
        abyAlarmObj[3] = ( u8 )ptLinkStatus->byE1Num;
        abyAlarmObj[4] = ( u8 )ptLinkStatus->byStatus;

        for( wLoop = 0; wLoop < sizeof( awE1AlarmCode ) / sizeof( awE1AlarmCode[0] ); wLoop++ )
        {
            bAlarmResult = g_cAlarmProc.FindAlarm( awE1AlarmCode[wLoop], ALARMOBJ_MCU_LINK, abyAlarmObj, &tAlarmData );
            if( bAlarmResult )
            {
                if( 0 == ( ptLinkStatus->byStatus & abyE1AlarmBit[wLoop] ) )    //normal
                {
                    bAlarmResult = g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo );
                    if(  !bAlarmResult )
                    {
                        Agtlog(LOG_ERROR, "[ProcLinkStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d, %d ) failed!\n", 
                                            tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], 
                                            tAlarmData.m_achObject[1], tAlarmData.m_achObject[2]);
                    }
                    else
                    {
                        if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                        {
                            Agtlog(LOG_ERROR, "[ProcLinkStatusChange] Link status<%d> trap<%d> send failed !\n",
                                               ptLinkStatus->byStatus, TRAP_ALARM_RESTORE);
                        }
                        else
                        {
                            Agtlog(LOG_INFORM, "[ProcLinkStatusChange] Link status<%d> trap<%d> send succeed !\n",
                                                ptLinkStatus->byStatus, TRAP_ALARM_RESTORE);
                        }
                    }
                }
            }
            else        // no such alarm
            {
                if( 0 != ( ptLinkStatus->byStatus & abyE1AlarmBit[wLoop] ))    // abnormal
                {
                    bAlarmResult = g_cAlarmProc.AddAlarm( awE1AlarmCode[wLoop], ALARMOBJ_MCU_LINK, abyAlarmObj, &tAlarmData );
                    if(  !bAlarmResult )
                    {
                        Agtlog(LOG_ERROR, "[ProcLinkStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                            awE1AlarmCode[wLoop], abyAlarmObj[0], abyAlarmObj[1], abyAlarmObj[2] );
                    }
                    else
                    {
                        if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                        {
                            Agtlog(LOG_ERROR, "[ProcLinkStatusChange] Link status<%d> trap<%d> send failed !\n",
                                               ptLinkStatus->byStatus, TRAP_ALARM_GENERATE);
                        }
                        else
                        {
                            Agtlog(LOG_INFORM, "[ProcLinkStatusChange] Link status<%d> trap<%d> send succeed !\n",
                                                ptLinkStatus->byStatus, TRAP_ALARM_GENERATE);
                        }
                    }
                }
            }
        }
        break;

	default:
        Agtlog(LOG_ERROR, "[ProcLinkStatusChange] unexpected msg%d<%s> received!\n", pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	
	return;
}

/*=============================================================================
  �� �� ���� ProcNetSyncStatusChange
  ��    �ܣ� ������ͬ��״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMsgCenterInst::ProcNetSyncStatusChange( CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcNetSyncStatusChange] The pointer can not be Null\n");
        return;
    }
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcNetSyncStatusChange] There's no NMS configed !\n");
        return;
    }
    
    u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    TMcupfmAlarmEntry     tAlarmData;
    u8   bySyncSrcAlarm = 0;
    BOOL32    bAlarmResult = FALSE;
	memset( abyAlarmObj, 0, sizeof(abyAlarmObj) );
    u8   bySyncStatus = 0; // Ĭ��Ϊ����

	switch(pcMsg->event)
	{
	case MCU_AGT_SYNCSOURCE_STATUSCHANGE:
        bySyncStatus = *pcMsg->content;
        abyAlarmObj[0] = bySyncStatus;
        
        // ����ͬ��Դ�쳣 ALARM_SYNCSRC_ABNORMAL
        bAlarmResult = g_cAlarmProc.FindAlarm( ALARM_MCU_SYNCSRC_ABNORMAL, ALARMOBJ_MCU, abyAlarmObj, &tAlarmData );
        if( bAlarmResult )
        {
            if( 0 == bySyncStatus )  // normal
            {
                bAlarmResult = g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo );
                if(  !bAlarmResult )
                {
                    Agtlog(LOG_ERROR, "[ProcNetSyncStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                                       tAlarmData.m_dwAlarmCode, 0, 0 );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcNetSyncStatusChange] net sync status<%d> trap<%d> send failed !\n",
                                           bySyncStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcNetSyncStatusChange] net sync status<%d> trap<%d> send succeed !\n",
                                            bySyncStatus, TRAP_ALARM_RESTORE);
                    }
                }
            }
            else
            {
                bySyncSrcAlarm++;
            }
        }
        else
        {
            if( 1 == bySyncStatus )  // abnormal
            {
                bAlarmResult = g_cAlarmProc.AddAlarm( ALARM_MCU_SYNCSRC_ABNORMAL, ALARMOBJ_MCU, abyAlarmObj, &tAlarmData );
                if(  !bAlarmResult )
                {
                    Agtlog(LOG_ERROR, "[ProcNetSyncStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                                        ALARM_MCU_SYNCSRC_ABNORMAL, 0, 0 );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcNetSyncStatusChange] net sync status<%d> trap<%d> send failed !\n",
                                           bySyncStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcNetSyncStatusChange] net sync status<%d> trap<%d> send failed !\n",
                                            bySyncStatus, TRAP_ALARM_GENERATE);
                    }
                }   
                
                bySyncSrcAlarm++;
            }
        }
/*
        // ������ͬ��Դ�쳣 ALARM_SYNCSRC_SPARE_ABNORMAL
        bAlarmResult = g_cAlarmProc.FindAlarm( ALARM_MCU_SYNCSRC_SPARE_ABNORMAL, ALARMOBJ_MCU, abyAlarmObj, &tAlarmData );
        if( bAlarmResult )
        {
            if( 0 == ( ( CSyncSourceAlarm * )pcMsg->content )->slavestatus )     //normal
            {
                bAlarmResult = g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo );
                if(  !bAlarmResult )
                {
                    OspPrintf(TRUE, FALSE,  "McuAgent: DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                        tAlarmData.m_dwAlarmCode, 0, 0 );
                }
                else
                {
                    SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE);
                }
            }
            else
            {
                bySyncSrcAlarm++;
            }
        }
        else        // no such alarm
        {
            if( 1 == ( ( CSyncSourceAlarm * )pcMsg->content )->slavestatus)     // abnormal
            {
                bAlarmResult = g_cAlarmProc.AddAlarm( ALARM_MCU_SYNCSRC_SPARE_ABNORMAL, ALARMOBJ_MCU, abyAlarmObj, &tAlarmData );
                if(  !bAlarmResult )
                {
                    OspPrintf(TRUE, FALSE,  "McuAgent: AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                        ALARM_MCU_SYNCSRC_SPARE_ABNORMAL, 0, 0 );
                }
                else
                {
                    SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE);
                }
                bySyncSrcAlarm++;
            }
        }

      
        // ������ͬ��Դͬʱ�쳣 ALARM_SYNCSRC_BOTH_ABNORMAL
        bAlarmResult = g_cAlarmProc.FindAlarm( ALARM_MCU_SYNCSRC_BOTH_ABNORMAL, ALARMOBJ_MCU, abyAlarmObj, &tAlarmData );
        if( bAlarmResult )
        {
            if( bySyncSrcAlarm < 2 )        // normal
            {
                bAlarmResult = g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo );
                if(  !bAlarmResult )
                {
                    OspPrintf(TRUE, FALSE,  "McuAgent: DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                        tAlarmData.m_dwAlarmCode, 0, 0 );
                }
                else
                {
                    SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE);
                }
            }
        }
        else        // no such alarm
        {
            if( 2 == bySyncSrcAlarm)     // abnormal(�������쳣)
            {
                bAlarmResult = g_cAlarmProc.AddAlarm( ALARM_MCU_SYNCSRC_BOTH_ABNORMAL, ALARMOBJ_MCU, abyAlarmObj, &tAlarmData );
                if(  !bAlarmResult )
                {
                    OspPrintf(TRUE, FALSE,  "McuAgent: AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                        ALARM_MCU_SYNCSRC_BOTH_ABNORMAL, 0, 0 );
                }
                else
                {
                    SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE);
                }
            }
        }
*/
        break;

	default:
        Agtlog(LOG_ERROR, "[ProcNetSyncStatusChange] unexpected msg%d<%s> received!\n", pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	
	return ;
}


/*=============================================================================
  �� �� ���� ProcBoardTempStatusChange
  ��    �ܣ� �������¶�״̬�ı�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2011/10/27    4.0			�´�ΰ                  ����
=============================================================================*/
void CMsgCenterInst::ProcBoardTempStatusChange( CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcBoardTempStatusChange] The pointer can not be Null\n");
        return;
    }
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcBoardTempStatusChange] There's no NMS configed !\n");
        return;
    }

    TBrdStatus *ptBrdStatus = NULL;
    u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    TMcupfmAlarmEntry     tAlarmData;
	u16    wLoop = 0;
    BOOL32 bAlarmResult = FALSE;
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));

	switch(pcMsg->event)
	{
	case SVC_AGT_BRD_TEMPERATURE_STATUS_NOTIFY:
		ptBrdStatus = ( TBrdStatus * )pcMsg->content;
        abyAlarmObj[0] = ( u8 )ptBrdStatus->byLayer;
        abyAlarmObj[1] = ( u8 )ptBrdStatus->bySlot;
        abyAlarmObj[2] = ( u8 )ptBrdStatus->byType;
        abyAlarmObj[3] = ( u8 )ptBrdStatus->byStatus;
        abyAlarmObj[4] = 0;

        bAlarmResult = g_cAlarmProc.FindAlarm( ALARM_MCU_BRD_TEMP_ABNORMAL, ALARMOBJ_MCU_BOARD, abyAlarmObj, &tAlarmData );
        if( bAlarmResult )
        {
            if( BRD_STATUS_NORMAL == ptBrdStatus->byStatus)    //normal
            {
                bAlarmResult = g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo );
                if(  !bAlarmResult )
                {
                    Agtlog(LOG_ERROR, "[ProcBoardTempStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d, %d ) failed!\n", 
                                        tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], 
                                        tAlarmData.m_achObject[1], tAlarmData.m_achObject[2]);
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcBoardTempStatusChange] temperature status<%d> trap<%d> send failed !\n",
                                           ptBrdStatus->byStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcBoardTempStatusChange] temperature status<%d> trap<%d> send succeed !\n",
                                            ptBrdStatus->byStatus, TRAP_ALARM_RESTORE);
                    }
                }
            }
        }
        else        // no such alarm
        {
            if( BRD_STATUS_ABNORMAL == ptBrdStatus->byStatus)    // abnormal
            {
                bAlarmResult = g_cAlarmProc.AddAlarm( ALARM_MCU_BRD_TEMP_ABNORMAL, ALARMOBJ_MCU_BOARD, abyAlarmObj, &tAlarmData );
                if(  !bAlarmResult )
                {
                    Agtlog(LOG_ERROR, "[ProcBoardTempStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                        ALARM_MCU_BRD_TEMP_ABNORMAL, abyAlarmObj[0], abyAlarmObj[1], abyAlarmObj[2] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcBoardTempStatusChange] temperature status<%d> trap<%d> send failed !\n",
                                           ptBrdStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcBoardTempStatusChange] temperature status<%d> trap<%d> send succeed !\n",
                                            ptBrdStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                }
            }
        }

        break;

	default:
        Agtlog(LOG_ERROR, "[ProcBoardTempStatusChange] unexpected msg%d<%s> received!\n", pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	
	return;
}

/*=============================================================================
  �� �� ���� ProcBoardCpuStatusChange
  ��    �ܣ� ������CPU״̬�ı�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2011/10/27    4.0			�´�ΰ                  ����
=============================================================================*/
void CMsgCenterInst::ProcBoardCpuStatusChange(CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcBoardCpuStatusChange] The pointer can not be Null\n");
        return;
    }
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcBoardCpuStatusChange] There's no NMS configed !\n");
        return;
    }
	
    TBrdStatus *ptBrdStatus = NULL;
    u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    TMcupfmAlarmEntry     tAlarmData;
	u16    wLoop = 0;
    BOOL32 bAlarmResult = FALSE;
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));
	
	switch(pcMsg->event)
	{
	case SVC_AGT_BRD_CPU_STATUS_NOTIF:
		ptBrdStatus = ( TBrdStatus * )pcMsg->content;
        abyAlarmObj[0] = ( u8 )ptBrdStatus->byLayer;
        abyAlarmObj[1] = ( u8 )ptBrdStatus->bySlot;
        abyAlarmObj[2] = ( u8 )ptBrdStatus->byType;
        abyAlarmObj[3] = ( u8 )ptBrdStatus->byStatus;
        abyAlarmObj[4] = 0;
		
        bAlarmResult = g_cAlarmProc.FindAlarm( ALARM_MCU_BRD_CPU_ABNORMAL, ALARMOBJ_MCU_BOARD, abyAlarmObj, &tAlarmData );
        if( bAlarmResult )
        {
            if( BRD_STATUS_NORMAL == ptBrdStatus->byStatus)    //normal
            {
                bAlarmResult = g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo );
                if(  !bAlarmResult )
                {
                    Agtlog(LOG_ERROR, "[ProcBoardCpuStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d, %d ) failed!\n", 
						tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], 
						tAlarmData.m_achObject[1], tAlarmData.m_achObject[2]);
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcBoardCpuStatusChange] cpu status<%d> trap<%d> send failed !\n",
							ptBrdStatus->byStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcBoardCpuStatusChange] cpu status<%d> trap<%d> send succeed !\n",
							ptBrdStatus->byStatus, TRAP_ALARM_RESTORE);
                    }
                }
            }
        }
        else        // no such alarm
        {
            if( BRD_STATUS_ABNORMAL == ptBrdStatus->byStatus)    // abnormal
            {
                bAlarmResult = g_cAlarmProc.AddAlarm( ALARM_MCU_BRD_CPU_ABNORMAL, ALARMOBJ_MCU_BOARD, abyAlarmObj, &tAlarmData );
                if(  !bAlarmResult )
                {
                    Agtlog(LOG_ERROR, "[ProcBoardCpuStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                        ALARM_MCU_BRD_CPU_ABNORMAL, abyAlarmObj[0], abyAlarmObj[1], abyAlarmObj[2] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcBoardCpuStatusChange] cpu status<%d> trap<%d> send failed !\n",
							ptBrdStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcBoardCpuStatusChange] cpu status<%d> trap<%d> send succeed !\n",
							ptBrdStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                }
            }
        }
		
        break;
		
	default:
        Agtlog(LOG_ERROR, "[ProcBoardCpuStatusChange] unexpected msg%d<%s> received!\n", pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	
	return;
}

/*=============================================================================
  �� �� ���� ProcBoardSoftwareVersionNotif
  ��    �ܣ� ������CPU״̬�ı�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2011/12/12    4.0			�´�ΰ                  ����
=============================================================================*/
/*void CMsgCenterInst::ProcBoardSoftwareVersionNotif(CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcBoardSoftwareVersionNotif] The pointer can not be Null\n");
        return;
    }

    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcBoardSoftwareVersionNotif] There's no NMS configed !\n");
        return;
    }

	//TODO ������ͨ�����ʵ��[12/13/2011 chendaiwei]
	
	return;
}
*/
/*=============================================================================
  �� �� ���� ProcBrdFanStatusChange
  ��    �ܣ� ���������״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMsgCenterInst::ProcBrdFanStatusChange( CMessage * const pcMsg )
{
    
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcBrdFanStatusChange] The pointer can not be Null\n");
        return;
    }
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcBrdFanStatusChange] There's no NMS configed !\n");
        return;
    }

    u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    BOOL32 bAlarmResult;
    TMcupfmAlarmEntry     tAlarmData;
	TBrdFanStatus* ptBrdFanStatus = NULL;	
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));

	switch(pcMsg->event)
	{
	case MCU_BRD_FAN_STATUS:
		ptBrdFanStatus = (TBrdFanStatus*)pcMsg->content;
        abyAlarmObj[0] = ptBrdFanStatus->byLayer;
        abyAlarmObj[1] = ptBrdFanStatus->bySlot;
        abyAlarmObj[2] = ptBrdFanStatus->byType;
        abyAlarmObj[3] = ptBrdFanStatus->byFanId;
        abyAlarmObj[4] = ptBrdFanStatus->byStatus;

        bAlarmResult = g_cAlarmProc.FindAlarm( ALARM_MCU_BOARD_FAN_STOP, ALARMOBJ_MCU_BRD_FAN, abyAlarmObj, &tAlarmData );
        if( bAlarmResult )
        {
            if( ptBrdFanStatus->byStatus  == 0 )      //normal
            {
                bAlarmResult = g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo );
                if( !bAlarmResult )
                {
                    Agtlog(LOG_ERROR, "[ProcBrdFanStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                                       tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], 
                                       tAlarmData.m_achObject[1] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcBrdFanStatusChange] FAN status<%d> trap<%d> send failed !\n",
                                           ptBrdFanStatus->byStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcBrdFanStatusChange] FAN status<%d> trap<%d> send succeed !\n",
                                            ptBrdFanStatus->byStatus, TRAP_ALARM_RESTORE);
                    }
                }
            }
        }
        else        // no such alarm
        {
            if( ptBrdFanStatus->byStatus != 0 )     // abnormal
            {
                bAlarmResult = g_cAlarmProc.AddAlarm( ALARM_MCU_BOARD_FAN_STOP, ALARMOBJ_MCU_BRD_FAN, abyAlarmObj, &tAlarmData );
                if( !bAlarmResult )
                {
                    Agtlog(LOG_ERROR, "[ProcBrdFanStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                        ALARM_MCU_BOARD_FAN_STOP, abyAlarmObj[0], abyAlarmObj[1] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcBrdFanStatusChange] FAN status<%d> trap<%d> send failed !\n",
                                           ptBrdFanStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcBrdFanStatusChange] FAN status<%d> trap<%d> send failed !\n",
                                            ptBrdFanStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                }
            }
        }
        break;

	default:
        Agtlog(LOG_ERROR, "[ProcBrdFanStatusChange] unexpected msg%d<%s> received!\n", pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}

	return ;
}

/*=============================================================================
  �� �� ���� ProcSoftwareStatusChange
  ��    �ܣ� ����������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMsgCenterInst::ProcSoftwareStatusChange( CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcSoftwareStatusChange] The pointer can not be Null\n");
        return;
    }
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcSoftwareStatusChange] There's no NMS configed !\n");
        return;
    }

    u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    TMcupfmAlarmEntry     tAlarmData;
	TSoftwareStatus* ptSoftStatus = NULL;
    BOOL32  bAlarmResult = FALSE;
    CTaskStatus* pcTaskStatus = NULL;

    u32 dwMemAllocRate = 0;
	memset( abyAlarmObj, 0, sizeof(abyAlarmObj) );

	switch(pcMsg->event)
	{
    case SVC_AGT_MEMORY_STATUS:     //�ڴ�״̬�ı�

		ptSoftStatus = (TSoftwareStatus*)pcMsg->content;
        abyAlarmObj[0] = ( u8 )ptSoftStatus->byLayer;
        abyAlarmObj[1] = ( u8 )ptSoftStatus->bySlot;
        abyAlarmObj[2] = ( u8 )ptSoftStatus->byType;

// 		dwMemAllocRate = *(u32*)pcMsg->content;
//         *(u32*)&abyAlarmObj[0] = dwMemAllocRate;
        
        bAlarmResult = g_cAlarmProc.FindAlarm( ALARM_MCU_MEMORYERROR, ALARMOBJ_MCU_SOFTWARE, abyAlarmObj, &tAlarmData );
        if( bAlarmResult )
        {
            if( 0 == ptSoftStatus->byStatus )      // normal
            {
                bAlarmResult = g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo );
                if(  !bAlarmResult )
                {
					Agtlog( LOG_VERBOSE, "McuAgent: DeleteAlarm( AlarmCode = %lu, Object = %d, %d, %d ) failed!\n", 
                        tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], 
                        tAlarmData.m_achObject[1], tAlarmData.m_achObject[2] );
                }
                else
                {
                    SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE);
                }
            }
        }
        else        // no such alarm
        {
            if( 1 == ptSoftStatus->byStatus)     // abnormal
            {
                bAlarmResult = g_cAlarmProc.AddAlarm( ALARM_MCU_MEMORYERROR, ALARMOBJ_MCU_SOFTWARE, abyAlarmObj, &tAlarmData );
                if(  !bAlarmResult )
                {
					Agtlog(LOG_VERBOSE, "McuAgent: AddAlarm( AlarmCode = %lu, Object = %d, %d, %d ) failed!\n", 
                        ALARM_MCU_MEMORYERROR, abyAlarmObj[0], abyAlarmObj[1], abyAlarmObj[2] );
                }
                else
                {
                    SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE);
                }
            }
        }
        break;

    case SVC_AGT_FILESYSTEM_STATUS:     //�ļ�ϵͳ״̬�ı�
		ptSoftStatus = (TSoftwareStatus*)pcMsg->content;
        abyAlarmObj[0] = ( u8 )ptSoftStatus->byLayer;
        abyAlarmObj[1] = ( u8 )ptSoftStatus->bySlot;
        abyAlarmObj[2] = ( u8 )ptSoftStatus->byType;
        abyAlarmObj[3] = 0;
        abyAlarmObj[4] = 0;

        bAlarmResult = g_cAlarmProc.FindAlarm( ALARM_MCU_FILESYSTEMERROR, ALARMOBJ_MCU_SOFTWARE, abyAlarmObj, &tAlarmData );
        if( bAlarmResult )
        {
            if( 0 == ptSoftStatus->byStatus)      // normal
            {
                bAlarmResult = g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo );
                if(  !bAlarmResult )
                {
                    Agtlog(LOG_ERROR, "[ProcSoftwareStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d, %d ) failed!\n", 
                                        tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], 
                                        tAlarmData.m_achObject[1], tAlarmData.m_achObject[2] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcSoftwareStatusChange] File system status<%d> trap<%d> send failed !\n",
                                           ptSoftStatus->byStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcSoftwareStatusChange] File system status<%d> trap<%d> send succeed !\n",
                                            ptSoftStatus->byStatus, TRAP_ALARM_RESTORE);
                    }
                }
            }
        }
        else        // no such alarm
        {
            if( 1 == ptSoftStatus->byStatus)     // abnormal
            {
                bAlarmResult = g_cAlarmProc.AddAlarm( ALARM_MCU_FILESYSTEMERROR, ALARMOBJ_MCU_SOFTWARE, abyAlarmObj, &tAlarmData );
                if(  !bAlarmResult )
                {
                    Agtlog(LOG_ERROR, "[ProcSoftwareStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d, %d ) failed!\n", 
                           ALARM_MCU_FILESYSTEMERROR, abyAlarmObj[0], abyAlarmObj[1], abyAlarmObj[1] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcSoftwareStatusChange] File system status<%d> trap<%d> send failed !\n",
                                           ptSoftStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcSoftwareStatusChange] File system status<%d> trap<%d> send succeed !\n",
                                            ptSoftStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                }
            }
        }
        break;

	case SVC_AGT_TASK_STATUS:       // ����״̬�ı�(mcu guard ���͹���)
        pcTaskStatus = ( CTaskStatus * )pcMsg->content;

        //����ı����appID һ������ʶ�Ϳ�����
        //abyAlarmObj[0] = ( u8 )pcTaskStatus->status;
        *(u16*)&abyAlarmObj[0] = pcTaskStatus->appid;

        bAlarmResult = g_cAlarmProc.FindAlarm( ALARM_MCU_TASK_BUSY, ALARMOBJ_MCU_TASK, abyAlarmObj, &tAlarmData );
		if( bAlarmResult )
		{
			if( 0 == pcTaskStatus->status)    //normal
			{
				bAlarmResult = g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo );
                if(  !bAlarmResult )
                {
					Agtlog(LOG_ERROR, "[ProcSoftwareStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d, %d, %d, %d ) failed!\n", 
					                    tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], 
					                    tAlarmData.m_achObject[1], tAlarmData.m_achObject[2], 
					                    tAlarmData.m_achObject[3], tAlarmData.m_achObject[4] );
                }
				else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcSoftwareStatusChange] Task status<%d> trap<%d> send failed !\n",
                                           pcTaskStatus->status, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcSoftwareStatusChange] Task status<%d> trap<%d> send succeed !\n",
                                            pcTaskStatus->status, TRAP_ALARM_RESTORE);
                    }
                }
			}
		}
		else        // no such alarm
		{
			if( 0 != pcTaskStatus->status)    // abnormal
			{
				bAlarmResult = g_cAlarmProc.AddAlarm( ALARM_MCU_TASK_BUSY, ALARMOBJ_MCU_TASK, abyAlarmObj, &tAlarmData );
                if(  !bAlarmResult )
                {
					Agtlog(LOG_ERROR, "[ProcSoftwareStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d, %d, %d, %d ) failed!\n", 
					ALARM_MCU_TASK_BUSY, abyAlarmObj[0], abyAlarmObj[1], abyAlarmObj[2], abyAlarmObj[3], abyAlarmObj[4]);
                }
				else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcSoftwareStatusChange] Task status<%d> trap<%d> send failed !\n",
                                           pcTaskStatus->status, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcSoftwareStatusChange] Task status<%d> trap<%d> send succeed !\n",
                                            pcTaskStatus->status, TRAP_ALARM_GENERATE);
                    }
                }
            }
		}
		break;
		
	default:
        Agtlog(LOG_INFORM, "[ProcSoftwareStatusChange] unexpected msg%d<%s> received!\n", pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	return ;
}

/*=============================================================================
  �� �� ���� ProcPowerFanStatusChange
  ��    �ܣ� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMsgCenterInst::ProcPowerFanStatusChange( CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcPowerFanStatusChange] The pointer can not be Null\n");
        return;
    }

    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcPowerFanStatusChange] There's no NMS configed !\n");
        return;
    }

    u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));
    TMcupfmAlarmEntry    tAlarmData;
	TPowerFanStatus* ptPowerFanStatus = NULL;

	switch(pcMsg->event)
	{
	case SVC_AGT_POWER_FAN_STATUS:
		ptPowerFanStatus = (TPowerFanStatus*)pcMsg->content;
        abyAlarmObj[0] = (u8)ptPowerFanStatus->byFanPos;
        abyAlarmObj[1] = (u8)ptPowerFanStatus->bySlot;
        abyAlarmObj[2] = 0;
        abyAlarmObj[3] = 0;
        abyAlarmObj[4] = 0;

        if( g_cAlarmProc.FindAlarm( ALARM_MCU_POWER_FAN_ABNORMAL, ALARMOBJ_MCU_POWER, abyAlarmObj, &tAlarmData ) )
        {
            if( 0 == ptPowerFanStatus->byStatus)      // ����
            {
                if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                {
                    Agtlog(LOG_ERROR, "[ProcPowerFanStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                                       tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], 
                                       tAlarmData.m_achObject[1] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcPowerFanStatusChange] Task status<%d> trap<%d> send failed !\n",
                                           ptPowerFanStatus->byStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcPowerFanStatusChange] Task status<%d> trap<%d> send succeed !\n",
                                            ptPowerFanStatus->byStatus, TRAP_ALARM_RESTORE);
                    }
                }
            }
        }
        else
        {
            if( 0 != ptPowerFanStatus->byStatus )
            {
                if( !g_cAlarmProc.AddAlarm( ALARM_MCU_POWER_FAN_ABNORMAL, ALARMOBJ_MCU_POWER, abyAlarmObj, &tAlarmData ) )
                {
                    Agtlog(LOG_ERROR, "[ProcPowerFanStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                                       ALARM_MCU_POWER_FAN_ABNORMAL, abyAlarmObj[0], abyAlarmObj[1] );
                }
                else
                {   
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcPowerFanStatusChange] Task status<%d> trap<%d> send failed !\n",
                                           ptPowerFanStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcPowerFanStatusChange] Task status<%d> trap<%d> send succeed !\n",
                                            ptPowerFanStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                }
            }
        }
        break;

	default:
        Agtlog(LOG_ERROR, "[ProcPowerFanStatusChange] unexpected msg%d<%s> received!\n", 
                                              pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}

	return ;
}

/*=============================================================================
  �� �� ���� ProcBoxFanStatusChange
  ��    �ܣ� ��������쳣�ϱ��澯����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
  ----------------------------------------------------------------------------
  �޸ļ�¼��
  ��   ��        �汾        �޸���        �޸�����
  2010/12/14     5.0         ������          ����
=============================================================================*/
void CMsgCenterInst::ProcBoxFanStatusChange( CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcBoxFanStatusChange] The pointer can not be Null\n");
        return;
    }
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcBoxFanStatusChange] There's no NMS configed !\n");
        return;
    }
	
    u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));
    TMcupfmAlarmEntry    tAlarmData;
	TBoxFanStatus* ptBoxFanStatus = NULL;
	
	switch(pcMsg->event)
	{
	case SVC_AGT_BOX_FAN_STATUS:
		ptBoxFanStatus = (TBoxFanStatus*)pcMsg->content;
        abyAlarmObj[0] = ( u8 )ptBoxFanStatus->byFanPos;
        abyAlarmObj[1] = ( u8 )ptBoxFanStatus->bySlot;
        abyAlarmObj[2] = 0;
        abyAlarmObj[3] = 0;
        abyAlarmObj[4] = 0;
		
        if( g_cAlarmProc.FindAlarm( ALARM_MCU_BOX_FAN_ABNORMAL, ALARMOBJ_MCU_BOX_FAN, abyAlarmObj, &tAlarmData ) )
        {
            if( 0 == ptBoxFanStatus->byStatus)      // ����
            {
                if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                {
                    Agtlog(LOG_ERROR, "[ProcBoxFanStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
						tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], 
						tAlarmData.m_achObject[1] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcBoxFanStatusChange] Task status<%d> trap<%d> send failed !\n",
							ptBoxFanStatus->byStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcBoxFanStatusChange] Task status<%d> trap<%d> send succeed !\n",
							ptBoxFanStatus->byStatus, TRAP_ALARM_RESTORE);       
                    }
                }
            }
        }
        else
        {
            if( 0 != ptBoxFanStatus->byStatus )
            {
                if( !g_cAlarmProc.AddAlarm( ALARM_MCU_BOX_FAN_ABNORMAL, ALARMOBJ_MCU_BOX_FAN, abyAlarmObj, &tAlarmData ) )
                {
                    Agtlog(LOG_ERROR, "[ProcBoxFanStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
						ALARM_MCU_BOX_FAN_ABNORMAL, abyAlarmObj[0], abyAlarmObj[1] );
                }
                else
                {   
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcBoxFanStatusChange] Task status<%d> trap<%d> send failed !\n",
							ptBoxFanStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcBoxFanStatusChange] Task status<%d> trap<%d> send succeed !\n",
							ptBoxFanStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                }
            }
        }
        break;
		
	default:
        Agtlog(LOG_ERROR, "[ProcBoxFanStatusChange] unexpected msg%d<%s> received!\n", 
			pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	
	return ;
}

/*=============================================================================
  �� �� ���� ProcMPCMemoryStatusChange
  ��    �ܣ� MPC��Mermory״̬�쳣�ϱ��澯����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
  ----------------------------------------------------------------------------
  �޸ļ�¼��
  ��   ��        �汾        �޸���        �޸�����
  2010/12/15     5.0         ������          ����
=============================================================================*/
void CMsgCenterInst::ProcMPCMemoryStatusChange( CMessage * const pcMsg)
{
	if( NULL == pcMsg)
	{
		Agtlog(LOG_ERROR, "[ProcMPCMemoryStatusChange] The pointer can not be Null!\n");
        return;
	}
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcMPCMemoryStatusChange] There's no NMS configed !\n");
        return;
    }
    
	u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));
    TMcupfmAlarmEntry    tAlarmData;
    TMPCMemoryStatus* ptMPCMemoryStatus = NULL;

	switch(pcMsg->event)
	{
	case SVC_AGT_MPCMEMORY_STATUS:
		ptMPCMemoryStatus = (TMPCMemoryStatus*)pcMsg->content;
        abyAlarmObj[0] = MEMORY_MCU_OBJECT;
        abyAlarmObj[1] = 0;
        abyAlarmObj[2] = 0;
        abyAlarmObj[3] = 0;
        abyAlarmObj[4] = 0;
		
        if( g_cAlarmProc.FindAlarm( ALARM_MCU_MPC_MEMORY_ABNORMAL, ALARMOBJ_MCU_MPC_MEMORY, abyAlarmObj, &tAlarmData ) )
        {
            if( 0 == ptMPCMemoryStatus->byMemoryStatus)      // ����
            {
                if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                {
                    Agtlog(LOG_ERROR, "[ProcMPCMemoryStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
						tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], tAlarmData.m_achObject[1] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcMPCMemoryStatusChange] Task status<%d> trap<%d> send failed !\n",
							ptMPCMemoryStatus->byMemoryStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcMPCMemoryStatusChange] Task status<%d> trap<%d> send succeed !\n",
							ptMPCMemoryStatus->byMemoryStatus, TRAP_ALARM_RESTORE);

						OspPrintf( TRUE, FALSE, "[ProcMPCMemoryStatusChange] Alarm restore!\n");          // [miaoqingsong �Բ��ӡ]
                    }
                }
            }
        }
        else
        {
            if( 0 != ptMPCMemoryStatus->byMemoryStatus )    // �쳣
            {
                if( !g_cAlarmProc.AddAlarm( ALARM_MCU_MPC_MEMORY_ABNORMAL, ALARMOBJ_MCU_MPC_MEMORY, abyAlarmObj, &tAlarmData ) )
                {
                    Agtlog(LOG_ERROR, "[ProcMPCMemoryStatusChange] AddAlarm( AlarmCode = %lu, Object = %d ) failed!\n", 
						ALARM_MCU_MPC_MEMORY_ABNORMAL, abyAlarmObj[0] );
                }
                else
                {   
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcMPCMemoryStatusChange] Task status<%d> trap<%d> send failed !\n",
							ptMPCMemoryStatus->byMemoryStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcMPCMemoryStatusChange] Task status<%d> trap<%d> send succeed !\n",
							ptMPCMemoryStatus->byMemoryStatus, TRAP_ALARM_GENERATE);

						OspPrintf( TRUE, FALSE, "[ProcMPCMemoryStatusChange] Alarm generate!\n");         // [miaoqingsong �Բ��ӡ]

                    }
                }
            }
        }
        break;
		
	default:
        Agtlog(LOG_ERROR, "[ProcMPCCpuStatusChange] unexpected msg%d<%s> received!\n", 
			pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	return ;
}

/*=============================================================================
  �� �� ���� ProcMpc2TempStatusChange
  ��    �ܣ� MPC2���¶��쳣�ϱ��澯����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
  ----------------------------------------------------------------------------
  �޸ļ�¼��
  ��   ��        �汾        �޸���        �޸�����
  2011/01/13     5.0         ������          ����
=============================================================================*/
void CMsgCenterInst::ProcMpc2TempStatusChange( CMessage * const pcMsg )
{
	if ( NULL == pcMsg )
	{
		Agtlog(LOG_ERROR, "[ProcMpc2TempStatusChange] The pointer can not be Null!\n");
        return;
	}
    
	if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcMpc2TempStatusChange] There's no NMS configed !\n");
        return;
    }
    
	u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));
    TMcupfmAlarmEntry    tAlarmData;
    TMPC2TempStatus* ptMPC2TemStatus = NULL;

	switch(pcMsg->event)
	{
	case SVC_AGT_MPC2TEMP_STATUS:
		ptMPC2TemStatus = (TMPC2TempStatus*)pcMsg->content;
        abyAlarmObj[0] = TEMP_MCU_OBJECT;
        abyAlarmObj[1] = 0;
        abyAlarmObj[2] = 0;
        abyAlarmObj[3] = 0;
        abyAlarmObj[4] = 0;
		
        if( g_cAlarmProc.FindAlarm( ALARM_MCU_MPC_TEMP_ABNORMAL, ALARMOBJ_MCU_MPC_TEMP, abyAlarmObj, &tAlarmData ) )
        {
            if( 0 == ptMPC2TemStatus->byMpc2TempStatus)      // ����
            {
                if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                {
                    Agtlog(LOG_ERROR, "[ProcMpc2TempStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d ) failed!\n", 
						tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcMpc2TempStatusChange] Task status<%d> trap<%d> send failed !\n",
							ptMPC2TemStatus->byMpc2TempStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcMpc2TempStatusChange] Task status<%d> trap<%d> send succeed !\n",
							ptMPC2TemStatus->byMpc2TempStatus, TRAP_ALARM_RESTORE);

						OspPrintf( TRUE, FALSE, "[ProcMpc2TempStatusChange] Alarm restore!\n");           // [miaoqingsong �Բ��ӡ]
                    }
                }
            }
        }
        else
        {
            if( 0 != ptMPC2TemStatus->byMpc2TempStatus )
            {
                if( !g_cAlarmProc.AddAlarm( ALARM_MCU_MPC_TEMP_ABNORMAL, ALARMOBJ_MCU_MPC_TEMP, abyAlarmObj, &tAlarmData ) )
                {
                    Agtlog(LOG_ERROR, "[ProcMpc2TempStatusChange] AddAlarm( AlarmCode = %lu, Object = %d ) failed!\n", 
						tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0] );
                }
                else
                {   
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcMpc2TempStatusChange] Task status<%d> trap<%d> send failed !\n",
							ptMPC2TemStatus->byMpc2TempStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcMpc2TempStatusChange] Task status<%d> trap<%d> send succeed !\n",
							ptMPC2TemStatus->byMpc2TempStatus, TRAP_ALARM_GENERATE);

						OspPrintf( TRUE, FALSE, "[ProcMpc2TempStatusChange] Alarm generate!\n");        // [miaoqingsong �Բ��ӡ]
                    }
                }
            }
        }
        break;
		
	default:
        Agtlog(LOG_ERROR, "[ProcMpc2TempStatusChange] unexpected msg%d<%s> received!\n", 
			pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}

	return ;	
}

/*=============================================================================
  �� �� ���� ProcMPCCpuStatusChange
  ��    �ܣ� MPC��Cpu״̬�쳣�ϱ��澯����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
  ----------------------------------------------------------------------------
  �޸ļ�¼��
  ��   ��        �汾        �޸���        �޸�����
  2010/12/15     5.0         ������          ����
=============================================================================*/
void CMsgCenterInst::ProcMPCCpuStatusChange( CMessage * const pcMsg )
{
	if ( NULL == pcMsg )
	{
		Agtlog(LOG_ERROR, "[ProcMPCCpuStatusChange] The pointer can not be Null!\n");
        return;
	}
    
	if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcMPCCpuStatusChange] There's no NMS configed !\n");
        return;
    }
    
	u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));
    TMcupfmAlarmEntry    tAlarmData;
    TMPCCpuStatus* ptMPCCpuStatus = NULL;

	switch(pcMsg->event)
	{
	case SVC_AGT_CPU_STATUS:
		ptMPCCpuStatus = (TMPCCpuStatus*)pcMsg->content;
        abyAlarmObj[0] = CPU_MCU_OBJECT;
        abyAlarmObj[1] = 0;
        abyAlarmObj[2] = 0;
        abyAlarmObj[3] = 0;
        abyAlarmObj[4] = 0;
		
        if( g_cAlarmProc.FindAlarm( ALARM_MCU_MPC_CPU_ABNORMAL, ALARMOBJ_MCU_MPC_CPU, abyAlarmObj, &tAlarmData ) )
        {
            if( 0 == ptMPCCpuStatus->byCpuStatus)      // ����
            {
                if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                {
                    Agtlog(LOG_ERROR, "[ProcMPCCpuStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
						tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], tAlarmData.m_achObject[1] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcMPCCpuStatusChange] Task status<%d> trap<%d> send failed !\n",
							ptMPCCpuStatus->byCpuStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcMPCCpuStatusChange] Task status<%d> trap<%d> send succeed !\n",
							ptMPCCpuStatus->byCpuStatus, TRAP_ALARM_RESTORE);

						OspPrintf( TRUE, FALSE, "[ProcMPCCpuStatusChange] Alarm restore!\n");           // [miaoqingsong �Բ��ӡ]
                    }
                }
            }
        }
        else
        {
            if( 0 != ptMPCCpuStatus->byCpuStatus )
            {
                if( !g_cAlarmProc.AddAlarm( ALARM_MCU_MPC_CPU_ABNORMAL, ALARMOBJ_MCU_MPC_CPU, abyAlarmObj, &tAlarmData ) )
                {
                    Agtlog(LOG_ERROR, "[ProcMPCCpuStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
						ALARM_MCU_MPC_CPU_ABNORMAL, abyAlarmObj[0], abyAlarmObj[1] );
                }
                else
                {   
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcMPCCpuStatusChange] Task status<%d> trap<%d> send failed !\n",
							ptMPCCpuStatus->byCpuStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcMPCCpuStatusChange] Task status<%d> trap<%d> send succeed !\n",
							ptMPCCpuStatus->byCpuStatus, TRAP_ALARM_GENERATE);

						OspPrintf( TRUE, FALSE, "[ProcMPCCpuStatusChange] Alarm generate!\n");        // [miaoqingsong �Բ��ӡ]
                    }
                }
            }
        }
        break;
		
	default:
        Agtlog(LOG_ERROR, "[ProcMPCCpuStatusChange] unexpected msg%d<%s> received!\n", 
			pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	return ;	
}

/*=============================================================================
  �� �� ���� ProcPowerTempStatusChange
  ��    �ܣ� ��Դ���¶�״̬�쳣�ϱ��澯����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
  ----------------------------------------------------------------------------
  �޸ļ�¼��
  ��   ��        �汾        �޸���        �޸�����
  2010/12/16     5.0         ������          ����
=============================================================================*/
void CMsgCenterInst::ProcPowerTempStatusChange( CMessage * const pcMsg )
{
	if ( NULL == pcMsg)
	{
		Agtlog(LOG_ERROR, "[ProcPowerTempStatusChange] The pointer can not be Null\n");
        return;
	}

	if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcPowerTempStatusChange] There's no NMS configed !\n");
        return;
    }

	u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));
    TMcupfmAlarmEntry    tAlarmData;
    TPowerBrdTempStatus* ptPowerBrdTempStatus = NULL;
	
	switch(pcMsg->event)
	{
	case SVC_AGT_POWERTEMP_STATUS:
		ptPowerBrdTempStatus = (TPowerBrdTempStatus*)pcMsg->content;
        abyAlarmObj[0] = ( u8 )ptPowerBrdTempStatus->bySlot;
        abyAlarmObj[1] = 0;
        abyAlarmObj[2] = 0;
        abyAlarmObj[3] = 0;
        abyAlarmObj[4] = 0;
		
        if( g_cAlarmProc.FindAlarm( ALARM_MCU_POWER_TEMP_ABNORMAL, ALARMOBJ_MCU_POWER_TEMP, abyAlarmObj, &tAlarmData ) )
        {
            if( 0 == ptPowerBrdTempStatus->byPowerBrdTempStatus )      // ����
            {
                if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                {
                    Agtlog(LOG_ERROR, "[ProcPowerTempStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
						tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], tAlarmData.m_achObject[1] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcPowerTempStatusChange] Task status<%d> trap<%d> send failed !\n",
							ptPowerBrdTempStatus->byPowerBrdTempStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcPowerTempStatusChange] Task status<%d> trap<%d> send succeed !\n",
							ptPowerBrdTempStatus->byPowerBrdTempStatus, TRAP_ALARM_RESTORE);

						OspPrintf( TRUE, FALSE, "[ProcPowerTempStatusChange] Alarm restore!\n");           // [miaoqingsong �Բ��ӡ]
                    }
                }
            }
        }
        else
        {
            if( 0 != ptPowerBrdTempStatus->byPowerBrdTempStatus )
            {
                if( !g_cAlarmProc.AddAlarm( ALARM_MCU_POWER_TEMP_ABNORMAL, ALARMOBJ_MCU_POWER_TEMP, abyAlarmObj, &tAlarmData ) )
                {
                    Agtlog(LOG_ERROR, "[ProcPowerTempStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
						ALARM_MCU_POWER_TEMP_ABNORMAL, abyAlarmObj[0], abyAlarmObj[1] );
                }
                else
                {   
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcPowerTempStatusChange] Task status<%d> trap<%d> send failed !\n",
							ptPowerBrdTempStatus->byPowerBrdTempStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcPowerTempStatusChange] Task status<%d> trap<%d> send succeed !\n",
							ptPowerBrdTempStatus->byPowerBrdTempStatus, TRAP_ALARM_GENERATE);

						OspPrintf( TRUE, FALSE, "[ProcPowerTempStatusChange] Alarm generate!\n");           // [miaoqingsong �Բ��ӡ]

                    }
                }
            }
        }
        break;
		
	default:
        Agtlog(LOG_ERROR, "[ProcPowerTempStatusChange] unexpected msg%d<%s> received!\n", 
			pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	
	return ;	
}

/*=============================================================================
  �� �� ���� ProcCpuFanStatusChange
  ��    �ܣ� ����cpu���ȸ澯�쳣
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMsgCenterInst::ProcCpuFanStatusChange( CMessage * const pcMsg )
{
	if ( NULL == pcMsg)
	{
		Agtlog(LOG_ERROR, "[ProcCpuFanStatusChange] The pointer can not be Null\n");
        return;
	}

	if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcCpuFanStatusChange] There's no NMS configed !\n");
        return;
    }

	u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));
    TMcupfmAlarmEntry    tAlarmData;
    u8* pbyCpuFanStatus = NULL;
	
	switch(pcMsg->event)
	{
	case SVC_AGT_CPU_FAN_STATUS:
		pbyCpuFanStatus = (u8*)pcMsg->content;        
		abyAlarmObj[0] = CPU_MCU_OBJECT;

        if( g_cAlarmProc.FindAlarm( ALARM_MCU_CPU_FAN_ABNORMAL, ALARMOBJ_MCU_CPU_FAN, abyAlarmObj, &tAlarmData ) )
        {
            if( 0 == *pbyCpuFanStatus )      // ����
            {
                if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                {
                    Agtlog(LOG_ERROR, "[ProcCpuFanStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
						tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], tAlarmData.m_achObject[1] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcCpuFanStatusChange] Task status<%d> trap<%d> send failed !\n",
							*pbyCpuFanStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcCpuFanStatusChange] Task status<%d> trap<%d> send succeed !\n",
							*pbyCpuFanStatus, TRAP_ALARM_RESTORE);						
                    }
                }
            }
        }
        else
        {
            if( 0 != *pbyCpuFanStatus )
            {
                if( !g_cAlarmProc.AddAlarm( ALARM_MCU_CPU_FAN_ABNORMAL, ALARMOBJ_MCU_CPU_FAN, abyAlarmObj, &tAlarmData ) )
                {
                    Agtlog(LOG_ERROR, "[ProcCpuFanStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
						ALARM_MCU_CPU_FAN_ABNORMAL, abyAlarmObj[0], abyAlarmObj[1] );
                }
                else
                {   
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcCpuFanStatusChange] Task status<%d> trap<%d> send failed !\n",
							*pbyCpuFanStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcCpuFanStatusChange] Task status<%d> trap<%d> send succeed !\n",
							*pbyCpuFanStatus, TRAP_ALARM_GENERATE);					

                    }
                }
            }
        }
        break;
		
	default:
        Agtlog(LOG_ERROR, "[ProcCpuFanStatusChange] unexpected msg%d<%s> received!\n", 
			pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	
	return ;
}

/*=============================================================================
  �� �� ���� ProcCpuTempStatusChange
  ��    �ܣ� ����cpu�¶ȸ澯�쳣
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMsgCenterInst::ProcCpuTempStatusChange( CMessage * const pcMsg )
{
	if ( NULL == pcMsg)
	{
		Agtlog(LOG_ERROR, "[ProcCpuTempStatusChange] The pointer can not be Null\n");
        return;
	}

	if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcCpuTempStatusChange] There's no NMS configed !\n");
        return;
    }

	u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));
    TMcupfmAlarmEntry    tAlarmData;
    u8* pbyCpuTempStatus = NULL;
	
	switch(pcMsg->event)
	{
	case SVC_AGT_CPUTEMP_STATUS:
		pbyCpuTempStatus = (u8*)pcMsg->content;        
		abyAlarmObj[0] = CPU_MCU_OBJECT;

        if( g_cAlarmProc.FindAlarm( ALARM_MCU_CPU_TEMP_ABNORMAL, ALARMOBJ_MCU_CPU_TEMP, abyAlarmObj, &tAlarmData ) )
        {
            if( 0 == *pbyCpuTempStatus )      // ����
            {
                if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                {
                    Agtlog(LOG_ERROR, "[ProcCpuTempStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
						tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], tAlarmData.m_achObject[1] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcCpuTempStatusChange] Task status<%d> trap<%d> send failed !\n",
							*pbyCpuTempStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcCpuTempStatusChange] Task status<%d> trap<%d> send succeed !\n",
							*pbyCpuTempStatus, TRAP_ALARM_RESTORE);						
                    }
                }
            }
        }
        else
        {
            if( 0 != *pbyCpuTempStatus )
            {
                if( !g_cAlarmProc.AddAlarm( ALARM_MCU_CPU_TEMP_ABNORMAL, ALARMOBJ_MCU_CPU_TEMP, abyAlarmObj, &tAlarmData ) )
                {
                    Agtlog(LOG_ERROR, "[ProcCpuTempStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
						ALARM_MCU_CPU_TEMP_ABNORMAL, abyAlarmObj[0], abyAlarmObj[1] );
                }
                else
                {   
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcCpuTempStatusChange] Task status<%d> trap<%d> send failed !\n",
							*pbyCpuTempStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcCpuTempStatusChange] Task status<%d> trap<%d> send succeed !\n",
							*pbyCpuTempStatus, TRAP_ALARM_GENERATE);					

                    }
                }
            }
        }
        break;
		
	default:
        Agtlog(LOG_ERROR, "[ProcCpuTempStatusChange] unexpected msg%d<%s> received!\n", 
			pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	
	return ;
}

/*=============================================================================
  �� �� ���� ProcPowerOnOffStatusChange
  ��    �ܣ� ���������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMsgCenterInst::ProcPowerOnOffStatusChange( CMessage * const pcMsg )
{
    if( NULL == pcMsg )
    {
        Agtlog(LOG_ERROR, "[ProcPowerOnOffStatusChange] The pointer can not be Null\n");
        return;
    }
    //״̬����
    TMcuSystem tMcuSysInfo;
    g_cCfgParse.GetSystemInfo( &tMcuSysInfo );

    if ( AGT_SVC_POWEROFF == pcMsg->event )
    {
        tMcuSysInfo.SetState( MCU_STATE_STANDY );
    }
    else if ( AGT_SVC_POWERON == pcMsg->event )
    {
        tMcuSysInfo.SetState( MCU_STATE_RUNNING );
    }
    g_cCfgParse.SetSystemInfo( &tMcuSysInfo );

    //trap�ķ���
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcPowerOnOffStatusChange] There's no NMS configed !\n");
        return;
    }

	if(!m_pcSnmpNodes || !m_pcAgentSnmp)
	{
		Agtlog(LOG_WARN, "[ProcPowerOnOffStatusChange] m_pcSnmpNodes or m_pcAgentSnmp is null !\n");
        return;
	}

    switch(pcMsg->event)
	{
	case AGT_SVC_POWEROFF:     // ҵ�񷢹����Ĵ�����Ϣ
        {
            u32 dwState = MCU_STATE_STANDY;
            m_pcSnmpNodes->Clear();
            m_pcSnmpNodes->SetTrapType( TRAP_POWEROFF );
            m_pcSnmpNodes->AddNodeValue( NODE_MCUSYSSTATE, &dwState, sizeof(dwState) );
            m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );
        }

		break;

	case AGT_SVC_POWERON:    // ҵ�񷢹����ĴӴ�����������Ϣ
        {
            u32 dwState = MCU_STATE_RUNNING;
            s8 achErrorStr[MAX_ERROR_NUM][MAX_ERROR_STR_LEN+1];
            memset(achErrorStr, '\0', sizeof(achErrorStr) );
			
			g_cAlarmProc.GetErrorString( (s8*)achErrorStr, sizeof(achErrorStr) );
            
            TMPCInfo tMPCInfo;
            g_cCfgParse.GetMPCInfo( &tMPCInfo );
            u32 dwLayer = (u32)tMPCInfo.GetLocalLayer();
            u32 dwSlot  = (u32)tMPCInfo.GetLocalSlot();
            m_pcSnmpNodes->Clear();
            m_pcSnmpNodes->SetTrapType( TRAP_COLD_RESTART );
            m_pcSnmpNodes->AddNodeValue( NODE_MCUSYSSTATE, &dwState, sizeof(dwState) );
            m_pcSnmpNodes->AddNodeValue( NODE_MCUSYSCONFIGERRORSTRING, achErrorStr, sizeof(achErrorStr) );
            m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );
        }
		break;

    case SVC_AGT_COLD_RESTART:     // ���������
        {
            u32 dwState = MPC_COLD_START;
            s8 achErrorStr[MAX_ERROR_NUM][MAX_ERROR_STR_LEN+1];
            memset(achErrorStr, '\0', sizeof(achErrorStr) );

			g_cAlarmProc.GetErrorString( (s8*)achErrorStr, sizeof(achErrorStr) );
            m_pcSnmpNodes->Clear();
            m_pcSnmpNodes->SetTrapType( TRAP_COLD_RESTART );
            m_pcSnmpNodes->AddNodeValue( NODE_MCUSYSSTATE, &dwState, sizeof(dwState) );
            m_pcSnmpNodes->AddNodeValue( NODE_MCUSYSCONFIGERRORSTRING, achErrorStr, sizeof(achErrorStr) );
            m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );
        }
        break;

    case SVC_AGT_POWEROFF:        // �ػ�
        {
            u32 dwState = MPC_POWER_OFF;
            m_pcSnmpNodes->Clear();
            m_pcSnmpNodes->SetTrapType(TRAP_POWEROFF);
            m_pcSnmpNodes->AddNodeValue( NODE_MCUSYSSTATE, &dwState, sizeof(dwState) );
            m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );
        }
        break;
		
	default:
        Agtlog(LOG_ERROR, "[ProcPowerOnOffStatusChange] unexpected msg%d<%s> received!\n", 
                                                pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}	
	return ;
}

/*=============================================================================
  �� �� ���� ProcGetConfigError
  ��    �ܣ� ȡ������Ϣ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CMsgCenterInst::ProcGetConfigError()
{
    Agtlog(LOG_ERROR, "[ProcGetConfigError] Error to read Mcu config\n");

    SetTimer( SVC_AGT_CONFIG_ERROR, 3000 ); // ������ʱ��
    return;
}

#ifdef _MINIMCU_
/*=============================================================================
�� �� ���� ProcSetDscInfoReq
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage * const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/17   4.0			�ܹ��                  ����
=============================================================================*/
void CMsgCenterInst::ProcSetDscInfoReq( CMessage * const pcMsg )
{
#ifdef _MINIMCU_
	CServMsg cMsg(pcMsg->content, pcMsg->length);

	u8 byLayer = * cMsg.GetMsgBody();
	u8 bySlot =  *( cMsg.GetMsgBody() + sizeof(u8) );
	TDSCModuleInfo tDscInfo = *(TDSCModuleInfo *)( cMsg.GetMsgBody() + sizeof(u8) + sizeof(u8) );
	TBrdPosition tBrdPos;
	tBrdPos.byBrdLayer = byLayer;
	tBrdPos.byBrdSlot = bySlot;
	tBrdPos.byBrdID = g_cCfgParse.GetConfigedDscType();

	u8 byDscType = 0xff;
	if ( g_cCfgParse.IsConfigedBrd(tBrdPos) && g_cCfgParse.IsDscReged(byDscType) )
	{
		u8 byDstInstId = g_cCfgParse.GetBrdInstIdByPos( tBrdPos );
		cMsg.SetMsgBody((u8*)&tDscInfo, sizeof(tDscInfo));
		post( MAKEIID(AID_MCU_BRDMGR, byDstInstId), MPC_BOARD_SETDSCINFO_REQ, cMsg.GetServMsg(), cMsg.GetServMsgLen() );
	}
	else
	{
		if ( SUCCESS_AGENT != g_cCfgParse.SetDSCInfo( &tDscInfo, TRUE ) )
		{
			Agtlog(LOG_ERROR, "[ProcSetDscInfo] Set dscinfo failed!\n");
			post( MAKEIID(AID_MCU_CONFIG, 1), AGT_SVC_SETDSCINFO_NACK, pcMsg->content, pcMsg->length );
		}
		post( MAKEIID(AID_MCU_CONFIG, 1), AGT_SVC_SETDSCINFO_ACK, pcMsg->content, pcMsg->length );
	}
#endif
	return;
}

/*=============================================================================
�� �� ���� ProcDscRegSucceedNotif
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage * const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/31   4.0			�ܹ��                  ����
=============================================================================*/
void CMsgCenterInst::ProcDscRegSucceedNotif( CMessage * const pcMsg )
{
	post( MAKEIID(AID_MCU_CONFIG, 1), pcMsg->event, pcMsg->content, pcMsg->length );
}

/*=============================================================================
�� �� ���� ProcSetDscInfoRsp
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage * const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/17   4.0			�ܹ��                  ����
=============================================================================*/
void CMsgCenterInst::ProcSetDscInfoRsp( CMessage * const pcMsg )
{
	Agtlog( LOG_INFORM, "[ProcSetDscInfoRsp]: %u(%s) passed!\n", pcMsg->event, ::OspEventDesc(pcMsg->event));
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	switch(pcMsg->event) 
	{
	case BOARD_MPC_SETDSCINFO_ACK:
		{
			TDSCModuleInfo tNewInfo = *(TDSCModuleInfo *)cServMsg.GetMsgBody();
			tNewInfo.Print();
			TDSCModuleInfo tOldInfo;
			g_cCfgParse.GetLastDscInfo( &tOldInfo );

			if ( SUCCESS_AGENT != g_cCfgParse.SetDSCInfo( &tNewInfo, TRUE ) )
			{
				Agtlog(LOG_ERROR, "[ProcSetDscInfo] Set dscinfo failed!\n");
				g_cCfgParse.SetDSCInfo( &tOldInfo, TRUE );
				post( MAKEIID(AID_MCU_CONFIG, 1), AGT_SVC_SETDSCINFO_NACK, pcMsg->content, pcMsg->length );
			}
			else
			{
				post( MAKEIID(AID_MCU_CONFIG, 1), AGT_SVC_SETDSCINFO_ACK );
			}
		}
		break;
	case BOARD_MPC_SETDSCINFO_NACK:
		post( MAKEIID(AID_MCU_CONFIG, 1), AGT_SVC_SETDSCINFO_NACK );
		break;
	default:
		break;
	}
}
#endif

/*=============================================================================
  �� �� ���� ProcBoardStatusAlarm
  ��    �ܣ� �������赥��澯��Led/����״̬��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void
  -----------------------------------------------------------------------------
  �޸ļ�¼    ��
  ��    ��	  �汾		�޸���		�߶���    �޸�����
  2012/04/06  4.7		liaokang             �޸� ������״̬���ӵ�led״̬֮��
=============================================================================*/
void CMsgCenterInst::ProcBoardStatusAlarm( CMessage * const pcMsg )
{
    if( NULL == pcMsg )
    {
        Agtlog(LOG_ERROR, "[ProcBoardStatusAlarm] The pointer can not be Null\n");
        return;
    }

    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcBoardStatusAlarm] There's no NMS configed !\n");
        return;
    }

    TEqpBrdCfgEntry* ptBrdCfg;
    ptBrdCfg = (TEqpBrdCfgEntry*)pcMsg->content;
    s8 achLedStatus[MAX_BOARD_LED_NUM + 1]={0};         // ��󵥰����ĿΪ32��
    s8 achEthPortStatus[MAX_BOARD_ETHPORT_NUM + 1]={0}; // ֧��IS2.2����״̬���ӵ�led״̬ [05/04/2012 liaokang]
    s8 achBuf[MAX_BOARD_LED_NUM + MAX_BOARD_ETHPORT_NUM + 1]={0};

    u16 wRow = g_cCfgParse.GetBoardRow(ptBrdCfg->GetLayer(),
                                       ptBrdCfg->GetSlot(),
                                       ptBrdCfg->GetType());
    if( ERR_AGENT_BRDNOTEXIST == wRow )
    {
        Agtlog(LOG_WARN, "[ProcBoardStatusAlarm] The board is not exist\n");
        return;
    }

	if (!m_pcSnmpNodes || !m_pcAgentSnmp)
	{
        Agtlog(LOG_WARN, "[ProcBoardStatusAlarm] m_pcSnmpNodes or m_pcAgentSnmp is null\n");
		return;
	}

    m_pcSnmpNodes->Clear();
    m_pcSnmpNodes->SetTrapType(TRAP_LED_STATE);

    u32 dwTmpData = ptBrdCfg->GetLayer();
    u32 dwNodeName = MAKE_MCUTABLE_NODENAME(wRow, NODE_MCUNETBRDCFGLAYER);
    m_pcSnmpNodes->AddNodeValue(dwNodeName, &dwTmpData, sizeof(dwTmpData) );

    dwTmpData = ptBrdCfg->GetSlot();
    dwNodeName = MAKE_MCUTABLE_NODENAME(wRow, NODE_MCUNETBRDCFGSLOT);
    m_pcSnmpNodes->AddNodeValue(dwNodeName, &dwTmpData, sizeof(dwTmpData) );
    
    dwTmpData = ptBrdCfg->GetType();
    dwNodeName = MAKE_MCUTABLE_NODENAME(wRow, NODE_MCUNETBRDCFGTYPE);
    m_pcSnmpNodes->AddNodeValue(dwNodeName, &dwTmpData, sizeof(dwTmpData) );
    
    // ֧��IS2.2����״̬���ӵ�led״̬ [05/04/2012 liaokang]
    u8 byBufSize;
    u8 byLedBufSize = g_cCfgParse.PanelLedStatusAgt2NMS( *ptBrdCfg, achLedStatus);
    u8 byEthPortBufSize = g_cCfgParse.EthPortStatusAgt2NMS( *ptBrdCfg, achEthPortStatus ); 
    
    if ( byLedBufSize )
    {  
        memcpy( achBuf, achLedStatus, byLedBufSize );
    }
    else
    {
        switch ( ptBrdCfg->GetType() )
        {
        case BRD_TYPE_IS22:  
            byLedBufSize = 32;
            memset( achBuf, 2, byLedBufSize ); // IS2.2��״̬ ��
            break;
        default:
            break; 
        }
    }
        
    byBufSize = byLedBufSize;

    if ( byEthPortBufSize )
    {  
        memcpy( achBuf + byBufSize, achEthPortStatus, byEthPortBufSize );
        byBufSize += byEthPortBufSize;
    }
    else // IS2.2����״̬��δ��ȡ��
    {
        switch ( ptBrdCfg->GetType() )
        {
        case BRD_TYPE_IS22:
            s8 achEthPort[16]; 
            memset( achEthPort, 3, sizeof(achEthPort) ); // IS2.2����״̬ δ֪
            achEthPort[0] = '~';        // �ָ���
            achEthPort[1] = '~';        // �ָ���
            achEthPort[2] = 1;          // ״̬����
            achEthPort[3] = 3;          // ǰ������Ŀ
            achEthPort[7] = 8;          // ��������Ŀ
            // �ָ���
            memcpy( achBuf + byBufSize, achEthPort, sizeof(achEthPort) );
            byBufSize += sizeof(achEthPort); 
            break;
        default:
            break; 
        } 
    }

    dwNodeName = MAKE_MCUTABLE_NODENAME(wRow, NODE_MCUNETBRDCFGPANELLED);
    m_pcSnmpNodes->AddNodeValue(dwNodeName, achBuf, byBufSize );


    dwTmpData = ptBrdCfg->GetState();
    dwNodeName = MAKE_MCUTABLE_NODENAME(wRow, NODE_MCUNETBRDCFGSTATUS);
    m_pcSnmpNodes->AddNodeValue(dwNodeName, &dwTmpData, sizeof(dwTmpData) );
    
    m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );

    Agtlog(LOG_VERBOSE, "[ProcBoardStatusAlarm] Layer = %d slot = %d Type = %d Recombined led and netport = %s\n",
        ptBrdCfg->GetLayer(), ptBrdCfg->GetSlot(), ptBrdCfg->GetType(), achBuf);

    return;
}

/*=============================================================================
  �� �� ���� ProcPowerOffAlarm
  ��    �ܣ� �ػ��澯
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CMsgCenterInst::ProcPowerOffAlarm()
{
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcPowerOffAlarm] There's no NMS configed !\n");
        return;
    }

	if (!m_pcSnmpNodes || !m_pcAgentSnmp)
	{
        printf("[ProcPowerOffAlarm] m_pcSnmpNodes or m_pcAgentSnmp is null\n");
		return;
	}

    u32 dwState = TRAP_POWEROFF; // �ػ�
    m_pcSnmpNodes->Clear();
    m_pcSnmpNodes->SetTrapType( TRAP_POWEROFF );
    m_pcSnmpNodes->AddNodeValue( NODE_MCUSYSSTATE, &dwState, sizeof(dwState) );

    Agtlog(LOG_INFORM, "[ProcPowerOffAlarm] Send alarm about power off\n");
    m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );
    
    return;
}

/*=============================================================================
  �� �� ���� ProcColStartdAlarm
  ��    �ܣ� ����������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CMsgCenterInst::ProcColdStartdAlarm()
{
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcColdStartdAlarm] There's no NMS configed !\n");
		printf("[ProcColdStartdAlarm] There's no NMS configed !\n");
        return;
    }

	if (!m_pcSnmpNodes || !m_pcAgentSnmp)
	{
        printf("[ProcColdStartdAlarm] m_pcSnmpNodes or m_pcAgentSnmp is null\n");
		return;
	}
	
    s8 achErrorStr[MAX_ERROR_NUM][MAX_ERROR_STR_LEN+1];
    memset(achErrorStr, '\0', sizeof(achErrorStr) );
	
	u16 wErrorLen = sizeof(achErrorStr);

	BOOL32 bRet = g_cAlarmProc.GetErrorString( (s8*)achErrorStr, sizeof(achErrorStr) );
	
	if( !bRet )
	{
		wErrorLen = 0;
	}

    u32 dwState = MPC_COLD_START;

    m_pcSnmpNodes->Clear();
    m_pcSnmpNodes->SetTrapType( TRAP_COLD_RESTART );
    m_pcSnmpNodes->AddNodeValue( NODE_MCUSYSSTATE, &dwState, sizeof(u32) );
    m_pcSnmpNodes->AddNodeValue( NODE_MCUSYSCONFIGERRORSTRING, achErrorStr, wErrorLen );    
    m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );

	SetTimer( MCUAGENT_UPDATE_MCUPFMINFO_TIMER, UPDATE_PFMINFO_TIME_OUT ); // ����Mcu����
    return;
}


/*=============================================================================
�� �� ���� ProcMcsUpdateBrdCmd
��    �ܣ� mcu֪ͨ������µ���汾
�㷨ʵ�֣� u8(��������) + u8(Դ�ļ�������) + u8(�ļ�������) + s8[](�ļ���)  //ע��(�ļ�����������·��)
ȫ�ֱ����� 
��    ���� CMessage * const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/8/17  4.0			������                  ����
=============================================================================*/
void CMsgCenterInst::ProcMcsUpdateBrdCmd(CMessage * const pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u8 byBrdIdx = *(u8*)cServMsg.GetMsgBody();
    u8 byRet = 1;
    
    TBoardInfo tBrdInfo;
    if ( SUCCESS_AGENT == g_cCfgParse.GetBrdCfgById(byBrdIdx, &tBrdInfo) )
    {
        Agtlog(LOG_INFORM, "[ProcMcsUpdateBrdCmd] byBrdIdx.%d, pos<%d,%d:%s>\n", 
                            byBrdIdx, tBrdInfo.GetLayer(), tBrdInfo.GetSlot(), 
                            g_cCfgParse.GetBrdTypeStr(tBrdInfo.GetType()) );
        TMPCInfo tMPCInfo;
        g_cCfgParse.GetMPCInfo( &tMPCInfo );
        Agtlog(LOG_INFORM, "[ProcMcsUpdateBrdCmd] mcu: local<%d,%d>, other<%d,%d>\n", 
                            tMPCInfo.GetLocalLayer(), tMPCInfo.GetLocalSlot(), 
                            tMPCInfo.GetOtherMpcLayer(), tMPCInfo.GetOtherMpcSlot() );

        // MPC������
		//  [1/21/2011 chendaiwei]֧��MPC2
        if ( ( BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ == tBrdInfo.GetType() || BRD_TYPE_MPC2 == tBrdInfo.GetType() )
		#ifndef _MINIMCU_
			 && 
             tMPCInfo.GetLocalLayer() == tBrdInfo.GetLayer() &&
             tMPCInfo.GetLocalSlot()  == tBrdInfo.GetSlot()
		#endif
		   )
        {
        #ifdef _LINUX_
            //linux�µ��õײ�ӿ�appupdate
            u8  byNum = *(u8*)(cServMsg.GetMsgBody()+sizeof(u8));
            s8 *lpMsgBody = (s8*)(cServMsg.GetMsgBody()+sizeof(u8)+sizeof(u8));
            u16 wMsgLen = cServMsg.GetMsgBodyLen() - 2;
            u8  abyResult[MAXNUM_FILE_UPDATE];
            memset( abyResult, 1, sizeof(abyResult) );
            Agtlog(LOG_INFORM, "[ProcMcsUpdateBrdCmd] update num: %d, len: %d !\n", byNum, wMsgLen);

            
            if ( !g_cCfgParse.UpdateMpcSoftware(lpMsgBody, wMsgLen, byNum, abyResult) )
                Agtlog( LOG_ERROR, "[ProcMcsUpdateBrdCmd] UpdateMpcSoftWare failed !\n");
            
            s32 nIndex = 0;
            for( ; nIndex < byNum; nIndex ++ )
            {
                Agtlog( LOG_INFORM, "[ProcMcsUpdateBrdCmd] Recv mpc result: %d\n", abyResult[nIndex] );
            }
            
            cServMsg.SetMsgBody(&byBrdIdx, sizeof(u8));
            cServMsg.CatMsgBody(&byNum, sizeof(u8));
            cServMsg.CatMsgBody(abyResult, byNum);
            
            u8 abyStrBuf[256];
            memset(abyStrBuf, 0, sizeof(abyStrBuf));
            ChangeNumBufToStrBuf(cServMsg.GetMsgBody(), byNum + 2, abyStrBuf);
            cServMsg.SetMsgBody(abyStrBuf, byNum + 2);
        #endif
            SendMsgToMcuCfg(AGT_SVC_UPDATEBRDVERSION_NOTIF, cServMsg.GetServMsg(), 
                                                        cServMsg.GetServMsgLen());
        }
        // ������������
        else
        {
            //[zw][09022008]����mcu����Ӧ�����Է������� 
			//  [1/21/2011 chendaiwei]֧��MPC2
            if ( BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ == tBrdInfo.GetType() || BRD_TYPE_MPC2 == tBrdInfo.GetType())
            {
                Agtlog(LOG_INFORM, "byBrdIdx.%d, pos<%d,%d:%s> update, ignore it due to MPC\n", 
                                   byBrdIdx, tBrdInfo.GetLayer(), tBrdInfo.GetSlot(), 
                                   g_cCfgParse.GetBrdTypeStr(tBrdInfo.GetType()) );
                return;
            }

            TBrdPosition tBrdPos;
            tBrdPos.byBrdID = tBrdInfo.GetType();
            tBrdPos.byBrdLayer = tBrdInfo.GetLayer();
            tBrdPos.byBrdSlot = tBrdInfo.GetSlot();
            
            // ׷��һ���ֶΣ���ʶ����������
            cServMsg.SetChnIndex(UPDATE_MCS);
            g_cCfgParse.UpdateBrdSoftware(tBrdPos, cServMsg);
        }
    }
    else
    {
        Agtlog(LOG_ERROR, "[ProcMcsUpdateBrdCmd] get TBoardInfo for byBrdIdx.%d failed.\n", byBrdIdx);
        
    #ifndef WIN32
        u8 byNum = *(cServMsg.GetMsgBody() + sizeof(u8));
        u8 abyRet[MAXNUM_FILE_UPDATE];
        memset(abyRet, 0, sizeof(abyRet));
        cServMsg.SetMsgBody(&byBrdIdx, sizeof(u8));
        cServMsg.CatMsgBody(&byNum,    sizeof(u8));
        cServMsg.CatMsgBody(abyRet, byNum);
        u8 abyStrBuf[256];
        memset(abyStrBuf, 0, sizeof(abyStrBuf));
        ChangeNumBufToStrBuf( cServMsg.GetMsgBody(), byNum + 2, abyStrBuf);
        cServMsg.SetMsgBody( abyStrBuf, byNum + 2 );
    #endif
        SendMsgToMcuCfg(AGT_SVC_UPDATEBRDVERSION_NOTIF, cServMsg.GetServMsg(), 
                                                    cServMsg.GetServMsgLen());
    }

    return;
}

#ifdef _MINIMCU_
/*=============================================================================
�� �� ���� ProcMcsUpdateDscMsg
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage * const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/8/20   4.0			�ܹ��                  ����
=============================================================================*/
void CMsgCenterInst::ProcMcsUpdateDscMsg( CMessage * const pcMsg )
{
	if ( NULL == pcMsg )
	{
		return;
	}

	CServMsg cMsg(pcMsg->content, pcMsg->length);

	u8 byBrdIdx = *cMsg.GetMsgBody();
	TBoardInfo tBrdInfo;
	TBrdPosition tBrdPos;
	memset(&tBrdPos,0,sizeof(tBrdPos));

    if ( SUCCESS_AGENT == g_cCfgParse.GetBrdCfgById(byBrdIdx, &tBrdInfo) )
	{	
        tBrdPos.byBrdID = tBrdInfo.GetType();
        tBrdPos.byBrdLayer = tBrdInfo.GetLayer();
        tBrdPos.byBrdSlot = tBrdInfo.GetSlot();
	}
	switch(pcMsg->event)
	{
	case SVC_AGT_STARTUPDATEDSCVERSION_REQ:
		g_cCfgParse.PostMsgToBrdMgr( tBrdPos, MPC_DSC_STARTUPDATE_SOFTWARE_REQ, cMsg.GetServMsg(), cMsg.GetServMsgLen() );
		break;
	case SVC_AGT_DSCUPDATEFILE_REQ:
		g_cCfgParse.PostMsgToBrdMgr( tBrdPos, MPC_DSC_UPDATEFILE_REQ, cMsg.GetServMsg(), cMsg.GetServMsgLen() );
		break;
	default:
		break;
	}
	return;
}
#endif

/*=============================================================================
�� �� ���� ProcNMSUpdateBrdCmd
��    �ܣ� ���� NMS ���� MPC 
�㷨ʵ�֣� u8(��������) + u8(Դ�ļ�������) + u8(�ļ�������) + s8[](�ļ���)  //ע��(�ļ�����������·��)
ȫ�ֱ����� 
��    ���� CMessage * const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/9/22   4.0			�ű���                  ����
=============================================================================*/
void CMsgCenterInst::ProcNMSUpdateBrdCmd( CMessage * const pcMsg )
{
    TBrdPosition tBrdPos;
    memcpy(&tBrdPos, pcMsg->content, sizeof(tBrdPos));
    u8  byNum       = *(pcMsg->content + sizeof(tBrdPos));
    u8 *pbyMsgBody  = pcMsg->content + sizeof(tBrdPos) + sizeof(u8);
    u16 wMsgBodyLen = pcMsg->length - sizeof(tBrdPos) - sizeof(u8);
    u8  byBrdIdx    = g_cCfgParse.GetBrdId(tBrdPos);

    Agtlog(LOG_INFORM, "[ProcNMSUpdateBrdCmd]brd<%d,%d:%s> to be update !\n",
                        tBrdPos.byBrdLayer, tBrdPos.byBrdSlot,
                        g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID) );
    
    // ��MPC ����, �ɵ��������
	//  [1/21/2011 chendaiwei]֧��MPC2
    if ( BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ != tBrdPos.byBrdID && BRD_TYPE_MPC2 != tBrdPos.byBrdID ) 
    {
        CServMsg cServMsg;
        cServMsg.SetChnIndex( UPDATE_NMS );         //���ֶ��������ʶ�������ķ�����Ϊ����
        cServMsg.SetMsgBody(&byBrdIdx, sizeof(u8));
        cServMsg.CatMsgBody(&byNum, sizeof(byNum));
        cServMsg.CatMsgBody(pbyMsgBody, wMsgBodyLen); 
        g_cCfgParse.UpdateBrdSoftware(tBrdPos, cServMsg);
        
        Agtlog(LOG_INFORM, "[ProcNMSUpdateBrdCmd] not mpc update!\n");

        return;
    }

    // �ڱ�������MPC��,������������mcu 
    TBoardInfo tBrdInfo;
    if ( SUCCESS_AGENT == g_cCfgParse.GetBrdCfgById(byBrdIdx, &tBrdInfo) )
    {
    #ifndef _MINIMCU_
        TMPCInfo tMPCInfo;
        g_cCfgParse.GetMPCInfo( &tMPCInfo );

        Agtlog(LOG_INFORM, "[ProcNMSUpdateBrdCmd]other mcu<%d,%d>, local mcu<%d,%d>\n", tMPCInfo.GetOtherMpcLayer(), tMPCInfo.GetOtherMpcSlot(),
               tMPCInfo.GetLocalLayer(), tMPCInfo.GetLocalSlot() );
		//  [1/21/2011 chendaiwei]֧��MPC2
        if ( (BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ == tBrdInfo.GetType() || BRD_TYPE_MPC2 == tBrdInfo.GetType())
          && tMPCInfo.GetOtherMpcLayer() == tBrdInfo.GetLayer() 
          && tMPCInfo.GetOtherMpcSlot()  == tBrdInfo.GetSlot()
          && tMPCInfo.GetOtherMpcLayer() != 0
          && tMPCInfo.GetOtherMpcSlot()  != 0 )
        {
            OspPost( MAKEIID(AID_MCU_MSMANAGERSSN, 1), EV_AGENT_MCU_MCU_UPDATE, pcMsg->content, pcMsg->length );
            Agtlog(LOG_INFORM, "[ProcNMSUpdateBrdCmd] EV_AGENT_MCU_MCU_UPDATE post here!\n");

            return;
        }
    #endif 
    #ifndef WIN32
        // linux�µ��õײ�ӿ�appupdate, VX�¸���
        u8  abyResult[MAXNUM_FILE_UPDATE];
        memset( abyResult, 1, sizeof(abyResult) );
        Agtlog(LOG_INFORM, "[ProcNMSUpdateBrdCmd] update num: %d, len: %d !\n", byNum, wMsgBodyLen);

        if ( !g_cCfgParse.UpdateMpcSoftware((s8*)pbyMsgBody, wMsgBodyLen, byNum, abyResult, TRUE) )
            Agtlog( LOG_ERROR, "[ProcNMSUpdateBrdCmd] UpdateMpcSoftWare failed !\n");

        s32 nIndex = 0;
        for( ; nIndex < byNum; nIndex ++ )
        {
            Agtlog( LOG_INFORM, "[ProcNMSUpdateBrdCmd] Recv mpc result: %d\n", abyResult[nIndex] );
        }           
    #endif
    }
    else
    {
        Agtlog(LOG_ERROR, "[ProcNMSUpdateBrdCmd] get TBoardInfo for byBrdIdx.%d failed.\n", byBrdIdx);
    }
    return;
}

/*=============================================================================
�� �� ���� ProcSmUpdateBrdCmd
��    �ܣ� ����SnmpManager���� MPC 
�㷨ʵ�֣�
ȫ�ֱ����� 
��    ���� CMessage * const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2010/3/18   5.0			½����                  ����
=============================================================================*/
void CMsgCenterInst::ProcSmUpdateBrdCmd( void )
{
    TBrdPosition tBrdPos;

	TMcuUpdateInfo* ptUpdateInfo = g_cCfgParse.GetMcuUpdateInfo();

	tBrdPos.byBrdID = ptUpdateInfo->GetType();
	tBrdPos.byBrdLayer = ptUpdateInfo->GetLayerID();
	tBrdPos.byBrdSlot = ptUpdateInfo->GetSlotID();

    Agtlog(LOG_INFORM, "[ProcSmUpdateBrdCmd]brd<%d,%d:%s> to be update !\n",
                        tBrdPos.byBrdLayer, tBrdPos.byBrdSlot,
                        g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID) );
    
    // ��MPC ����, �ɵ��������
	//  [1/21/2011 chendaiwei]֧��MPC2
    if ( BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ != tBrdPos.byBrdID && BRD_TYPE_MPC2 != tBrdPos.byBrdID ) 
    {
        Agtlog(LOG_INFORM, "[ProcSmUpdateBrdCmd] not mpc update!\n");
		ptUpdateInfo->SetUpdateState(SNMP_FILE_UPDATE_FAILED);
        return;
    }

    // �ڱ�������MPC��,������������mcu
	u8  byBrdIdx    = g_cCfgParse.GetBrdId(tBrdPos);

    TBoardInfo tBrdInfo;
    if ( SUCCESS_AGENT == g_cCfgParse.GetBrdCfgById(byBrdIdx, &tBrdInfo) )
    {
    #ifndef _MINIMCU_
        TMPCInfo tMPCInfo;
        g_cCfgParse.GetMPCInfo( &tMPCInfo );

        Agtlog(LOG_INFORM, "[ProcSmUpdateBrdCmd]other mcu<%d,%d>, local mcu<%d,%d>\n", tMPCInfo.GetOtherMpcLayer(), tMPCInfo.GetOtherMpcSlot(),
               tMPCInfo.GetLocalLayer(), tMPCInfo.GetLocalSlot() );

		//  [1/21/2011 chendaiwei]֧��MPC2
        if ( (BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ == tBrdInfo.GetType() || BRD_TYPE_MPC2 == tBrdInfo.GetType())
          && tMPCInfo.GetOtherMpcLayer() == tBrdInfo.GetLayer() 
          && tMPCInfo.GetOtherMpcSlot()  == tBrdInfo.GetSlot()
          && tMPCInfo.GetOtherMpcLayer() != 0
          && tMPCInfo.GetOtherMpcSlot()  != 0 )
        {
			//��ʱ��֧�ֱ���mcu������
			Agtlog(LOG_WARN, "[ProcSmUpdateBrdCmd] ��ʱ��֧�ֱ���mcu������!\n");
            //OspPost( MAKEIID(AID_MCU_MSMANAGERSSN, 1), EV_AGENT_MCU_MCU_UPDATE, pcMsg->content, pcMsg->length );
            //Agtlog(LOG_INFORM, "[ProcSmUpdateBrdCmd] EV_AGENT_MCU_MCU_UPDATE post here!\n");

            return;
        }
    #endif 
    #ifndef WIN32
        // linux�µ��õײ�ӿ�appupdate, VX�¸���
        if ( !g_cCfgParse.SmUpdateMpcSoftware() )
            Agtlog( LOG_ERROR, "[ProcSmUpdateBrdCmd] SmUpdateMpcSoftware failed !\n");
   
    #endif
    }
    else
    {
        Agtlog(LOG_ERROR, "[ProcSmUpdateBrdCmd] get TBoardInfo for byBrdIdx.%d failed.\n", byBrdIdx);
    }
    return;
}

/*=============================================================================
�� �� ���� ProcMsgToBrdManager
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage * const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/8/28   4.0			�ܹ��                  ����
=============================================================================*/
void CMsgCenterInst::ProcMsgToBrdManager( CMessage * const pcMsg )
{
	if ( NULL == pcMsg )
	{
		Agtlog(LOG_ERROR, "[ProcMsgToBrdManager] Msg is NULL!\n");
		return;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	u8 byBrdLayer = 0;
	u8 byBrdSlot = 0;
	memcpy(&byBrdLayer, cServMsg.GetMsgBody(), sizeof(u8));
	memcpy(&byBrdSlot, cServMsg.GetMsgBody()+sizeof(u8), sizeof(u8));
	TBrdPosition tBrdPos;
	tBrdPos.byBrdLayer = byBrdLayer;
	tBrdPos.byBrdSlot = byBrdSlot;
	u16 wMsgEvent = 0;
	BOOL32 IsSendMsg2BrdMgr = TRUE;
	switch(pcMsg->event)
	{
	case SVC_AGT_DISCONNECTBRD_CMD:
		{
			memcpy(&tBrdPos.byBrdID, cServMsg.GetMsgBody()+sizeof(u8)*2, sizeof(u8));
			if ( !g_cBrdManagerApp.IsRegedBoard(tBrdPos) )
			{
				IsSendMsg2BrdMgr = FALSE;
			}
			wMsgEvent = MPC_BOARD_DISCONNECT_CMD;
		}
		break;

#ifndef _MINIMCU_
	case SVC_AGT_BOARDCFGMODIFY_NOTIF:
		wMsgEvent = MCU_BOARD_CONFIGMODIFY_NOTIF;
		break;
#endif

#ifdef _MINIMCU_
	case SVC_AGT_DSCTELNETLOGININFO_UPDATE_CMD:
		wMsgEvent = MCU_BOARD_DSCTELNETLOGININFO_UPDATE_CMD;
		break;
	case SVC_AGT_DSCGKINFO_UPDATE_CMD:
		wMsgEvent = MPC_BOARD_DSCGKINFO_UPDATE_CMD;
		break;
#endif

	default:
		break;
	}
	if ( IsSendMsg2BrdMgr )
	{
		g_cCfgParse.PostMsgToBrdMgr( tBrdPos, wMsgEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	}
}

/*=============================================================================
  �� �� ���� GetBoardLedStatus
  ��    �ܣ� ȡ��Ӧ����Ƶ�״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byBrdType
             TEqpBrdCfgEntry& tBrdCfg
             u8* pszLedStr
  �� �� ֵ�� u8 
=============================================================================*/
u8 CMsgCenterInst::GetBoardLedStatus(u8 byBrdType, TEqpBrdCfgEntry& tBrdCfg, u8* pszLedStr)
{
    if(NULL == pszLedStr)
    {
        return 0;
    }
    u8 byLedCount = 0;

    switch( byBrdType )
	{
	case BRD_TYPE_MPC/*DSL8000_BRD_MPC*/:
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 9 );     //mpc��9������
        byLedCount = 9;
	    break;
	//[2011/01/25 zhushz]IS2.x����֧��
	case BRD_TYPE_IS21:
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 29 );     //IS2.1��29������
        byLedCount = 29;
	    break;
	case BRD_TYPE_IS22:
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 32 );     //IS2.2��32������
        byLedCount = 32;
	    break;				
	//  [1/21/2011 chendaiwei]֧��MPC2
	case BRD_TYPE_MPC2/*DSL8000_BRD_MP2C*/:
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 8 );     //mpc2��8������
        byLedCount = 8;
	    break;
					
	case BRD_TYPE_CRI/*DSL8000_BRD_CRI*/:
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 10 );    //cir��10������	
        byLedCount = 10;
	    break;
		
	//  [1/21/2011 chendaiwei]CRI2֧��
	case BRD_TYPE_CRI2:
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 14 );    //cri2��14������	
        byLedCount = 14;
	    break;

	//  [1/21/2011 chendaiwei]DRI2֧��
	case BRD_TYPE_DRI2:
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 12 );    //dri2��12������	
        byLedCount = 12;
	    break;
					
	case BRD_TYPE_MMP/*DSL8000_BRD_MMP*/:
	case BRD_TYPE_VPU/*DSL8000_BRD_VPU*/:
	case BRD_TYPE_DEC5/*DSL8000_BRD_DEC5*/:
	    memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 10 );    //mpc��10������
        byLedCount = 10;
	    break;
					
	case BRD_TYPE_DRI/*DSL8000_BRD_DRI*/:
	case BRD_TYPE_DSI/*DSL8000_BRD_DSI*/:
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 12 );    //mpc��12������
        byLedCount = 12;
	    break;

	case BRD_TYPE_16E1/*DSL8000_BRD_16E1*/:
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 20 );    //DRI16��20������
        byLedCount = 20;
	    break;
		
	case BRD_TYPE_VAS/*DSL8000_BRD_VAS*/:
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 4 );     //mpc��4������
        byLedCount = 4;
		break;
					
	case BRD_TYPE_IMT/*DSL8000_BRD_IMT*/:
	case BRD_TYPE_APU/*DSL8000_BRD_APU*/:
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 8 );     //mpc��8������	
        byLedCount = 8;
	    break;
								
	case BRD_TYPE_DTI/*DSL8000_BRD_DTI*/:   //not use now
	case BRD_TYPE_DIC/*DSL8000_BRD_DIC*/:	//not use now
        
	default:
        byLedCount = strlen(tBrdCfg.GetPanelLed());
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), byLedCount);
	    break;
    }
    return byLedCount;
}

//������Ϣ��mcu��������
void SendMsgToMcuCfg( u16 wEvent, const u8 * pcMsg, u16 wLen )
{
    ::OspPost( MAKEIID( AID_MCU_CONFIG, 1), wEvent, pcMsg, wLen);
    return;
}

/*=============================================================================
  �� �� ���� SendPrmAlarmTrap
  ��    �ܣ� ����pfm�澯trap
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CNodes * pcNodes:              Ҫ���͵Ľڵ����
             TMcupfmAlarmEntry tAlarmEntry: �澯���ݰ��ṹ
             u8 byTrapType:                 trap����
  �� �� ֵ�� u16 
 -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2006/07/28  4.0         �ű���       ����
=============================================================================*/
/*lint -save -esym(429, pcNodes)*/
u16 CMsgCenterInst::SendPrmAlarmTrap( CNodes          * pcNodes, 
                                      TMcupfmAlarmEntry tAlarmEntry, 
                                      u8                byTrapType )
{
    u16 wRetVal = SNMP_GENERAL_ERROR;

    wRetVal = pcNodes->Clear();
    if (SNMP_SUCCESS != wRetVal) 
    {
        Agtlog(LOG_ERROR, "[SendPrmAlarmTrap] m_pcNodes clear failed !\n");
        return wRetVal;
    }

    wRetVal = pcNodes->SetTrapType(byTrapType);
    if (SNMP_SUCCESS != wRetVal) 
    {
        Agtlog(LOG_ERROR, "[SendPrmAlarmTrap] m_pcNodes set trap type failed !\n");
        return wRetVal;
    }

    wRetVal = pcNodes->AddNodeValue(NODE_MCUPFMALARMSERIALNO, 
                                    &tAlarmEntry.m_dwSerialNo, 
                                    sizeof(tAlarmEntry.m_dwSerialNo));
    if (SNMP_SUCCESS != wRetVal) 
    {
        Agtlog(LOG_ERROR, "[SendPrmAlarmTrap] m_pcNodes add node(0x%x) failed !\n", 
                                                         NODE_MCUPFMALARMSERIALNO);
        return wRetVal;
    }
    
    wRetVal = pcNodes->AddNodeValue(NODE_MCUPFMALARMALARMCODE, 
                                    &tAlarmEntry.m_dwAlarmCode,
                                    sizeof(tAlarmEntry.m_dwAlarmCode));
    if (SNMP_SUCCESS != wRetVal) 
    {
        Agtlog(LOG_ERROR, "[SendPrmAlarmTrap] m_pcNodes add node(0x%x) failed !\n", 
                                                        NODE_MCUPFMALARMALARMCODE);
        return wRetVal;
    }
	u32 dwObjType = tAlarmEntry.m_byObjType;
	
    wRetVal = pcNodes->AddNodeValue(NODE_MCUPFMALARMOBJTYPE, 
									&dwObjType, 
                                    sizeof(dwObjType));

    if (SNMP_SUCCESS != wRetVal) 
    {
        Agtlog(LOG_ERROR, "[SendPrmAlarmTrap] m_pcNodes add node(0x%x) failed !\n", 
                                                          NODE_MCUPFMALARMOBJTYPE);
        return wRetVal;
    }

    wRetVal = pcNodes->AddNodeValue(NODE_MCUPFMALARMOBJECT, 
                                    tAlarmEntry.m_achObject, 
                                    sizeof(tAlarmEntry.m_achObject));
    if (SNMP_SUCCESS != wRetVal) 
    {
        Agtlog(LOG_ERROR, "[SendPrmAlarmTrap] m_pcNodes add node(0x%x) failed !\n", 
                                                           NODE_MCUPFMALARMOBJECT);
        return wRetVal;
    }

    wRetVal = pcNodes->AddNodeValue(NODE_MCUPFMALARMTIME, 
                                    tAlarmEntry.m_achTime, 
                                    sizeof(tAlarmEntry.m_achTime));
    if (SNMP_SUCCESS != wRetVal) 
    {
        Agtlog(LOG_ERROR, "[SendPrmAlarmTrap] m_pcNodes add node(0x%x) failed !\n", 
                                                             NODE_MCUPFMALARMTIME);
        return wRetVal;
    }

	if (!m_pcAgentSnmp)
	{
		return SNMP_GENERAL_ERROR;
	}

    wRetVal = m_pcAgentSnmp->AgentSendTrap( *pcNodes );
    if (SNMP_SUCCESS != wRetVal) 
    {
        Agtlog(LOG_ERROR, "[SendPrmAlarmTrap] AgentSendTrap failed !\n");
        return wRetVal;
    }

    return wRetVal;
}

/*=============================================================================
  �� �� ���� ChangeNumBufToStrBuf
  ��    �ܣ� ����pfm�澯trap
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8* pNumBuf:  ��������� buf
             u8 byBufLen:  ����
             u8* pStrBuf:  �������ַ��� buf
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2006/09/14  4.0         �ű���       ����
=============================================================================*/
BOOL32 ChangeNumBufToStrBuf( u8* pNumBuf, u8 byBufLen, u8* pStrBuf )
{
    if ( NULL == pNumBuf || NULL == pStrBuf || 0 == byBufLen )
    {
        Agtlog( LOG_ERROR, "[ChangeNumBufToStrBuf] param error: pIn<0x%x>pOut<0x%x>len<%d>\n",
                            pNumBuf, pStrBuf, byBufLen );
        return FALSE;
    }
    s32 nIndex = 0;
    for( ; nIndex < byBufLen; nIndex ++ )
    {
        Agtlog(LOG_INFORM, "[ChangeNumBufToStrBuf] %d.num: %d to be change\n",
                            nIndex, pNumBuf[nIndex]);
        pStrBuf[nIndex] = pNumBuf[nIndex] + '0';
    }
    pStrBuf[nIndex] = '\0';
    
    return TRUE;
}

void ProcCbFtpcCallbackFun(TFtpcCallback *PTParam, void *pCbd)
{
	TUpdateFileInfo* ptLastUpdateFile = (TUpdateFileInfo *)pCbd;

	if (ptLastUpdateFile == NULL)
	{
		Agtlog(LOG_ERROR, "ProcCbFtpcCallbackFun ��ȡ�ص�����ʧ��\n");
		return;
	}

	if (PTParam == NULL)
	{
		Agtlog(LOG_ERROR, "ProcCbFtpcCallbackFun ��ȡ��������ʧ��\n");
		return;
	}

	ptLastUpdateFile->ChgUpdateState(PTParam->dwMsgType);

	TMcuUpdateInfo* ptUpdateInfo = g_cCfgParse.GetMcuUpdateInfo();

	if (PTParam->dwMsgType == FTPC_FINISHED)
	{
		Agtlog(LOG_INFORM, "FtpcCallbackFun Msg: %ld, Percent:%ld, File:%s FTPC_FINISHED\n", 
		PTParam->dwMsgType, PTParam->UMsgInfo.dwTransPercent, ptLastUpdateFile->GetSvrFilePath());

		if (ptUpdateInfo->IsFtpEnd())
		{
			//֪ͨmcu����
			OspPost( MAKEIID(AID_MCU_AGENT, 1), EV_SM_UPDATEDMPC_CMD);
		}
		else
		{
			if (!ptUpdateInfo->FtpGet())
			{
				Agtlog(LOG_ERROR, "ProcCbFtpcCallbackFun FtpGet Fail\n");
				ptUpdateInfo->SetUpdateState(SNMP_FILE_UPDATE_FAILED);
			}
		}
	}
	else
	{
		ptLastUpdateFile->ChgTransPercent(PTParam->UMsgInfo.dwTransPercent);

		if (PTParam->dwMsgType != FTPC_TRANSPERCENT)
		{
			Agtlog(LOG_ERROR, "ProcCbFtpcCallbackFun Error\n");
			ptUpdateInfo->SetUpdateState(SNMP_FILE_UPDATE_FAILED);
		}

		Agtlog(LOG_VERBOSE, "FtpcCallbackFun Msg: %ld, Percent:%ld, File:%s\n", 
		PTParam->dwMsgType, PTParam->UMsgInfo.dwTransPercent, ptLastUpdateFile->GetSvrFilePath());
	}


}

//[5/16/2013 liaokang] ���ڴ�,�ȴ�����snmpadp֧����ȥ
// Snmp�Ƿ�ΪUTF8����
BOOL32 g_bSnmpIsUtf8 = FALSE; // Ĭ��ΪGBK
void SetSnmpIsUtf8(BOOL32 bIsUtf8)
{ 
    g_bSnmpIsUtf8 = bIsUtf8; 
}
BOOL32 GetSnmpIsUtf8() 
{ 
    return g_bSnmpIsUtf8;
}
//end

/*====================================================================
    ������      : TransEncodingForNmsData
    ����        : ���������ݽ��������ת��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    ����ֵ˵��  : False û�н���ת�����
                  TURE  ������ת�����
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2013/05/16              liaokang      ����
====================================================================*/
/*lint -save -e715*/
BOOL32 TransEncodingForNmsData( const s8 *pSrc, s8 *pDst, u32 dwLen, BOOL32 bSet)
{
#ifndef _UTF8
    return FALSE; // �ϰ汾����FALSE��������ת�����
#else
    if ( !pSrc || !pDst )
    {
		return FALSE;
	}
   
    BOOL32 bNmsIsUtf8 = GetSnmpIsUtf8();
    if( bNmsIsUtf8 )
    {
        return FALSE;  // ����Ҫ����ת��
    }

    if( bSet )
    {
        gb2312_to_utf8(pSrc, pDst, dwLen-1);
    }
    else
    {
        utf8_to_gb2312(pSrc, pDst, dwLen-1);
    }    
    
    return TRUE;
#endif
}
/*lint -restore*/

// END OF FILE
