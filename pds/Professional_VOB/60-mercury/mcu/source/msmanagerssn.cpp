/*****************************************************************************
   ģ����      : MCU�����ȱ��ݵ�ͨѶ��ʵ�ֻỰģ��
   �ļ���      : msmanagerssn.cpp
   ����ļ�    : msmanagerssn.h
   �ļ�ʵ�ֹ���: MCU�����ȱ��ݵ�ͨѶ��ʵ�ֻỰģ��ʵ��
   ����        : ����
   �汾        : V4.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2005/10/18  4.0         ����        ����
-----------------------------------------------------------------------------
�������⣺
0�����������л�ʱ,�µ����ð��轫�����Ϣ����֪ͨ���,�Ա㱣���л������п��ܵı仯,�Ա���һ��
1�����ð��������ݵ���ʱ��ȱ�� [MSMag <->McuVc]ģ�黥����
   ���ֶ�ʱ��δ�ָ�
2����̬����ع��ñ�����д���ʲ���ȱ�ٻ��Ᵽ��
3����ǰ���ͳ�ȥ��ͬ�����ݵ�״̬ SendReq ��ȴ���ӦAck: 
   �賬ʱ��ʧ��ʱ���ش����ƣ������ش�2�Σ���ʧ�ܵȴ��´�ͬ����ע����� 
4����ͬ�������ݵļ򻯴���
5��ͬ��ǰ����̬��δ����Osp��Ϣ���յ�״̬�����ڶѻ�ʱҲ��ͬ��
6������̬�ı�����Ϊ���õĴ����ʩ: �޷��ع�������
7�����ݽṹ���գ�����ͬ��������MCUӦ�û���һ�£��ݲ����ǽ������⣺Pack Ingore
8��IncompletePro ��ǵĴ��벿�����һ��
******************************************************************************/

#include "evmcu.h"
#include "agentinterface.h"
#include "mcuvc.h"
//#include "msmanagerssn.h"
#include "vcsssn.h"

#ifdef _VXWORKS_
#include <inetLib.h>
#endif

// MPC2 ֧��
#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
        #include "nipwrapper.h"
        #include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
#else
    #include "brddrvlib.h"
#endif

#include "brdwrapperdef.h"

#include "mcuver.h"

CMSManagerSsnApp  g_cMSSsnApp;

u32 g_dwMsCheckTime = MS_CEHCK_MSSTATE_TIMEOUT;

//����
CMSManagerSsnInst::CMSManagerSsnInst()
{
	m_dwMcuAppIId   = 0;
	m_dwMcuNode     = INVALID_NODE;
	m_dwRcvFrmLen   = 0;
	m_bRemoteInited = FALSE;
	m_byCheckTimes  = 0;
	memset((void*)&m_tSndFrmHead, 0, sizeof(m_tSndFrmHead));
	memset((void*)&m_tRcvFrmHead, 0, sizeof(m_tRcvFrmHead));
	memset((void*)&m_tMSCurSynState, 0, sizeof(m_tMSCurSynState));	
}

//����
CMSManagerSsnInst::~CMSManagerSsnInst()
{
    // destructor ���ܵ��� 
	// ClearInst();
	if (NULL != m_tSndFrmHead.m_pbyFrmBuf)
	{
/*lint -save -e424*/	
		u8* pbyFrmBuf = m_tSndFrmHead.m_pbyFrmBuf-sizeof(TMSSynDataReqHead);
		MCU_SAFE_DELETE(pbyFrmBuf)
/*lint -restore*/
	}
	if (NULL != m_tRcvFrmHead.m_pbyFrmBuf)
	{
		MCU_SAFE_DELETE(m_tRcvFrmHead.m_pbyFrmBuf)
	}
	memset((void*)&m_tSndFrmHead, 0, sizeof(m_tSndFrmHead));
	memset((void*)&m_tRcvFrmHead, 0, sizeof(m_tRcvFrmHead));
}

/*=============================================================================
    �� �� ���� InstanceEntry
    ��    �ܣ� ��ͨʵ�����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CMessage * const pcMsg ��Ϣ
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
void CMSManagerSsnInst::InstanceEntry( CMessage * const pcMsg )
{
	if( NULL == pcMsg )
	{
		PrintMSWarning("InstanceEntry The received msg's pointer in the msg entry is NULL!\n" );
		return;
	}

	switch( pcMsg->event ) 
	{
	case OSP_OVERFLOW:   //ʵ������
		break;

	case OSP_DISCONNECT: //�ϵ�֪ͨ
		ProcMcuMcuDisconnectNtf();
		break;

	case MCU_MSEXCHANGER_POWERON_CMD:
		ProcPowerOnCmd();
		break;

	//��λ��MCU����λ��MCU �����Ӷ�ʱ��
	case MCU_MCU_CONNECT_TIMER:
		ProcMcuMcuConnectTimeOut();
		break;
	//��λ��MCU����λ��MCU ��ע�ᶨʱ��
	case MCU_MCU_REGISTER_TIMER:
		ProcMcuMcuRegisterTimeOut();
		break;

	//vxworks�¼��������״̬�Ķ�ʱ��
	case MCU_MCU_CEHCK_MSSTATE_TIMER:
		ProcMcuMcuCheckMSStateTimeOut();
		break;

	//���ð�MCU���ð�MCU �����ݵ�����ʱ��
	case MCU_MCU_SYN_TIMER:
		ProcMcuMcuSynTimeOut();
		break;
		
	//MCU֮�� �ȴ��Զ˵�����Ӧ�� �ĳ�ʱ��ʱ��
	case MCU_MCU_WAITFORRSP_TIMER:
		ProcMcuMcuWaitforRspTimeOut( pcMsg );
		break;

	//����λ��MCU��������ע������
	case MCU_MCU_REGISTER_REQ:
		ProcMcuMcuRegisterReq( pcMsg );
        break;
	//������λ��MCU��������ע������� Ӧ��
	case MCU_MCU_REGISTER_ACK:
	case MCU_MCU_REGISTER_NACK:
		ProcMcuMcuRegisterRsp( pcMsg );		
		break;

	//����Զ�MCU������λ���ͼ���ǰ������ʹ��״̬֪ͨ������������Э��
	case MCU_MCU_MS_DETERMINE:
		ProcMcuMcuMSDetermine( pcMsg );
		break;
	//����������Э�̽��֪ͨ
	case MCU_MCU_MS_RESULT:
		ProcMcuMcuMSResult( pcMsg );
		break;
		
	//���������ð�MCU֮�� ��ʼ���ݵ��� ����
	case MCU_MCU_START_SYN_REQ:
		ProcMcuMcuStartSynReq( pcMsg );
		break;
	case MCU_MCU_START_SYN_ACK:
	case MCU_MCU_START_SYN_NACK:
		ProcMcuMcuStartSynRsp( pcMsg );
		break;	
	//�������ð�MCU���ð�MCU �������ݵ��� ֪ͨ
	case MCU_MCU_END_SYN_NTF:
		ProcMcuMcuEndSynNtf();
		break;

	//���������ð�MCU֮�� ʵ���а���������ݵ��� ����
	case MCU_MCU_SYNNING_DATA_REQ:
		ProcMcuMcuSynningDataReq( pcMsg );
		break;
	case MCU_MCU_SYNNING_DATA_RSP:
		ProcMcuMcuSynningDataRsp( pcMsg );
		break;

    case MCU_MCU_PROBE_REQ:
        ProcMcuMcuProbeReq();
        break;
        
    case MCU_MCU_PROBE_ACK:
        ProcMcuMcuProbeAck();
        break;
 
    // [zw] [09/11/2008] ��mcu�ñ�mcu������������
    case EV_AGENT_MCU_MCU_RESTART:
        PostMsgToOtherMcuDaemon( MCU_MCU_REBOOT_CMD, NULL, 0 );
        break;
    case MCU_MCU_REBOOT_CMD:
        OspPost( MAKEIID(AID_MCU_BRDGUARD, 1), AGT_SVC_REBOOT );
		printf("[CMSManagerSsnInst::InstanceEntry]MCU_MCU_REBOOT_CMD req reboot\n");
        break;
    case EV_AGENT_MCU_MCU_UPDATE:        
        PostMsgToOtherMcuDaemon( MCU_MCU_UPDATE_CMD, (u8*)pcMsg->content, pcMsg->length );
        break;
    case MCU_MCU_UPDATE_CMD:
        {
            CServMsg cMsg(pcMsg->content, pcMsg->length);
            OspPost( MAKEIID(AID_MCU_AGENT, 1), EV_MSGCENTER_NMS_UPDATEDMPC_CMD, cMsg.GetMsgBody(), cMsg.GetMsgBodyLen() );
            break;
        }

	default:
		PrintMSCritical("[MSManager] Wrong message %u(%s) received in InstanceEntry()!\n", 
			             pcMsg->event, ::OspEventDesc(pcMsg->event));
		break;
	}

	return;
}

/*=============================================================================
    �� �� ���� ProcPowerOnCmd
    ��    �ܣ� �ϵ�֪ͨ��ͨ������ͨѶ�ڵ��������Э�̼���Ӧ��������Ϣͬ������
	           �������������ϵͳʱ�估�����ļ�ͬ�������ͷ�ͬ���ź�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CMessage * const pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
void CMSManagerSsnInst::ProcPowerOnCmd( void )
{
/*lint -save -esym(429, pbyDataBuf)*/
	if (MS_STATE_IDLE != CurState())
	{
		printf("[ProcPowerOnCmd] MS_STATE_IDLE != CurState().%d!!!\n", CurState());
		return;
	}

	u8 *pbyDataBuf = new u8[MS_MAX_FRAME_LEN + sizeof(TMSSynDataReqHead)];
	if (NULL == pbyDataBuf)
	{
		ClearInst();
		printf("[ProcPowerOnCmd] allocate memory failed for SndFrmBuf!!!\n");
		return;
	}
	m_tSndFrmHead.m_pbyFrmBuf = pbyDataBuf + sizeof(TMSSynDataReqHead);
	m_tRcvFrmHead.m_pbyFrmBuf = new u8[MS_MAX_FRAME_LEN];
	if (NULL == m_tRcvFrmHead.m_pbyFrmBuf)
	{
		ClearInst();
		printf("[ProcPowerOnCmd] allocate memory failed for RcvFrmBuf!!!\n");
		return;
	}

	//������״̬��ȡ
	if (g_cMSSsnApp.GetMSDetermineType())
	{
		emMCUMSState emLocalMSState = MCU_MSSTATE_STANDBY;
		if (g_cMSSsnApp.IsActiveBoard())
		{
			emLocalMSState = MCU_MSSTATE_ACTIVE;
		}
		LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[ProcPowerOnCmd] SetCurMSState.%d!\n", emLocalMSState);
		g_cMSSsnApp.SetCurMSState( emLocalMSState );
		SetTimer(MCU_MCU_CEHCK_MSSTATE_TIMER, g_dwMsCheckTime);
	}

	PrintMSInfo("[ProcPowerOnCmd] ready to com: LocalMSInfo Type.%d State.%d\n", 
		         g_cMSSsnApp.GetLocalMSType(), g_cMSSsnApp.GetCurMSState());

	//��λ�� ���� ��λ�壬��������Ӧע��
	if (MCU_MSTYPE_SLAVE == g_cMSSsnApp.GetLocalMSType())
	{
        SetTimer( MCU_MCU_CONNECT_TIMER, 10 );//������ʼ����	
        LogPrint( LOG_LVL_ERROR, MID_MCU_MSMGR, "[ProcPowerOnCmd] start timer connect to another mcu.\n");
	}

	return;

/*lint -restore*/
}

/*=============================================================================
    �� �� ���� ClearInst
    ��    �ܣ� ���ʵ�� ���ݣ�m_pbySndDataBuf��m_pbyRcvDataBuf���⣩
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
void CMSManagerSsnInst::ClearInst( void )
{
	m_dwMcuAppIId   = 0;
	m_dwMcuNode     = INVALID_NODE;
	m_dwRcvFrmLen   = 0;
	m_bRemoteInited = FALSE;
	memset((void*)&m_tMSCurSynState, 0, sizeof(m_tMSCurSynState));
    g_cMSSsnApp.SetRemoteMpcConnected(FALSE);

	KillTimer(MCU_MCU_CONNECT_TIMER);
	KillTimer(MCU_MCU_REGISTER_TIMER);
	KillTimer(MCU_MCU_SYN_TIMER);
	KillTimer(MCU_MCU_WAITFORRSP_TIMER);
	//KillTimer(MCU_MCU_CEHCK_MSSTATE_TIMER);
	//2010102122_tzy �������ͬ��δ��ɻ�ʧ�ܣ���ʱother mcu����������
	g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);

	NEXTSTATE(MS_STATE_IDLE);

	LogPrint( LOG_LVL_ERROR, MID_MCU_MSMGR, "[ClearInst] LocalMSInfo Type.%d State.%d\n", 
		         g_cMSSsnApp.GetLocalMSType(), g_cMSSsnApp.GetCurMSState());

	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuDisconnectNtf
    ��    �ܣ� MCU֮�� ����֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CMessage * const pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuDisconnectNtf( void )
{
	PrintMSException("[ProcMcuMcuDisconnectNtf] LocalMSInfo Type.%d State.%d\n", 
		             g_cMSSsnApp.GetLocalMSType(), g_cMSSsnApp.GetCurMSState());

    //  xsl [2/25/2006]������Ϣ��ʱ������ע�����
    if (INVALID_NODE != m_dwMcuNode)
    {
        OspDisconnectTcpNode( m_dwMcuNode );
    }    
    
	ClearInst();
	
	//���ʵ�������õ�����
	if (FALSE == g_cMSSsnApp.GetMSDetermineType())
	{
		BOOL32 bNtfMSExchange = FALSE;
		if (MCU_MSSTATE_STANDBY == g_cMSSsnApp.GetCurMSState())
		{
			//���ð�����Ϊ���ð�
			g_cMSSsnApp.SetCurMSState(MCU_MSSTATE_ACTIVE);
			bNtfMSExchange = TRUE;

            g_cMSSsnApp.SetMsSwitchOK(FALSE);
            // guzh [9/12/2006] ���Ŀǰ��ͬ���ɹ�״̬(����ӵ����������)������Ϊ�л��ɹ�
            if(g_cMSSsnApp.IsMSSynOK())
            {
                g_cMSSsnApp.SetMsSwitchOK(TRUE);
            }
            
			LogPrint( LOG_LVL_ERROR, MID_MCU_MSMGR, "[McuMcuDisconnectNtf] STANDBY->ACTIVE, IsMsSwitchOK :%d\n",
                      g_cMSSsnApp.IsMsSwitchOK());            
		}
        
		if (MCU_MSSTATE_ACTIVE == g_cMSSsnApp.GetCurMSState())
		{
            u8 byIsSwitchOk = g_cMSSsnApp.IsMsSwitchOK() ? 1 : 0;

			g_cMSSsnApp.SetMSSynOKFlag(FALSE);
			PrintMSInfo("[ProcMcuMcuDisconnectNtf] Unlock, byIsSwitchOk.%d\n", byIsSwitchOk);

			//���ð�����Ϊ���ð壬[MSMag <->McuVc]ģ�黥�������� 2005-12-15
			g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);

            if (FALSE == g_cMSSsnApp.IsMSConfigInited())
            {
                // guzh [4/17/2007] �л���ͬʱ������ʼ����
                g_cMSSsnApp.LeaveMSInitLock();	
            }
            
            // guzh [4/17/2007] ��out��
            g_cMSSsnApp.SetMpcOusLed(FALSE);

			//֪ͨ[McuVc]ģ��������״̬���
			if (bNtfMSExchange)
			{
				::OspPost(MAKEIID(AID_MCU_VC, CInstance::DAEMON), MCU_MSSTATE_EXCHANGE_NTF, &byIsSwitchOk, sizeof(u8));
			}
		}
	}
	
    u32 dwTimeOut = 10;
    //�����󱾶�Ӧ���� Active ״̬
	if (MCU_MSSTATE_ACTIVE != g_cMSSsnApp.GetCurMSState())
	{
		PrintMSException("[ProcMcuMcuDisconnectNtf] CurMSState.%d != MCU_MSSTATE_ACTIVE\n", 
			              g_cMSSsnApp.GetCurMSState());

        // guzh [9/12/2006] �����ʱӲ����û�з�ת����ȴ�һ���
        dwTimeOut = CONNECT_MASTERMCU_TIMEOUT*3;
	}

	//��λ�� ���³������� ��λ��
	if (MCU_MSTYPE_SLAVE == g_cMSSsnApp.GetLocalMSType())
	{
        SetTimer( MCU_MCU_CONNECT_TIMER, dwTimeOut );//��ʼ����
        LogPrint( LOG_LVL_ERROR, MID_MCU_MSMGR, "[ProcMcuMcuDisconnectNtf] start timer connect to another mcu.\n");
    }

    // guzh [9/15/2006] ֪ͨMCU����,MPC����
    u8 byState = BOARD_OUTLINE;
	post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_STANDBYMPCSTATUS_NOTIFY, 
	      &byState, sizeof(byState) );

	PrintMSInfo("[ProcMcuMcuDisconnectNtf] Notify Other MPC Status.%d\n", 
		         byState);
    
	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuConnectTimeOut
    ��    �ܣ� ��λ��MCU������λ��mcu���Ӷ�ʱ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuConnectTimeOut( void )
{
    PrintMSWarning("[ProcMcuMcuConnectTimeOut] Trying Connect to Master(MC1) MPC...\n");
	//printf("[ProcMcuMcuConnectTimeOut] Trying Connect to Master(MC1) MPC...\n");

	if (MS_STATE_IDLE != CurState())
	{
		return;
	}
	
	//��λ�� ���� ��λ�壬��������Ӧע��
	if (TRUE == ConnectMasterMcu())
	{
		NEXTSTATE(MS_STATE_INIT);
	}
	
	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuRegisterTimeOut
    ��    �ܣ� ��λ��MCU������λ��mcuע�ᶨʱ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuRegisterTimeOut( void )
{
	if (MS_STATE_INIT != CurState())
	{
		return;
	}
	
	//��λ�� ���� ��λ�壬��������Ӧע��
	if (FALSE == ConnectMasterMcu())
	{
		NEXTSTATE(MS_STATE_IDLE);
	}

	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuCheckMSStateTimeOut
    ��    �ܣ� vxworks�¼��������״̬�Ķ�ʱ��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
    2007/04/18  4.0         ����                ֧������ɳ�ʼ��֮ǰ�ͽ��м��
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuCheckMSStateTimeOut( void )
{
	//�����õ�����
	if (FALSE == g_cMSSsnApp.GetMSDetermineType())
	{
		SetTimer(MCU_MCU_CEHCK_MSSTATE_TIMER, g_dwMsCheckTime);
		return;
	}

	BOOL32 bMpcEnable = g_cMSSsnApp.IsActiveBoard();
	emMCUMSState emLocalMSState = MCU_MSSTATE_STANDBY;
	if (bMpcEnable)
	{
		emLocalMSState = MCU_MSSTATE_ACTIVE;
	}

	PrintMSVerbose("[ProcMcuMcuCheckMSStateTimeOut] LocalMSInfo Type.%d OldState.%d NewState.%d CheckTime.%u\n", 
				   g_cMSSsnApp.GetLocalMSType(), g_cMSSsnApp.GetCurMSState(), emLocalMSState, g_dwMsCheckTime);

	//������״̬�����
	if (emLocalMSState != g_cMSSsnApp.GetCurMSState())
	{
		//����������״̬���ʱ�Ķ�������:����������״̬����б������Ž����л�,��ֹ״̬���ٱ���Ķ���
		m_byCheckTimes++;
        g_cMSSsnApp.IncDitheringTimes();
        
		if (m_byCheckTimes < MS_CHECK_MSSTATE_TIMES)
		{			
			PrintMSException("[ProcMcuMcuCheckMSStateTimeOut] CheckTimes.%d LocalMSInfo Type.%d OldState.%d NewState.%d\n", 
					  m_byCheckTimes, g_cMSSsnApp.GetLocalMSType(), g_cMSSsnApp.GetCurMSState(), emLocalMSState);                       
		}
		else
		{
			m_byCheckTimes = 0;
			g_cMSSsnApp.SetCurMSState(emLocalMSState);

            g_cMSSsnApp.SetMsSwitchOK(FALSE);
             // guzh [9/12/2006] ���Ŀǰ�Ѿ���ͬ����ȫ״̬������Ϊ�л��ɹ�
            if(MCU_MSSTATE_ACTIVE == emLocalMSState && g_cMSSsnApp.IsMSSynOK() )
            {
                g_cMSSsnApp.SetMsSwitchOK(TRUE);
                printf("[ProcMcuMcuCheckMSStateTimeOut] MCU MS Switched to ACTIVE success!\n");
            }
            // guzh [2/11/2007] �������ous�ƣ���ֹ����ʾ
            g_cMSSsnApp.SetMpcOusLed(FALSE);

			PrintMSException("[ProcMcuMcuCheckMSStateTimeOut] New State.%d IsSynOK.%d IsSwitchOK.%d\n", 
					         emLocalMSState, 
                             g_cMSSsnApp.IsMSSynOK(),
                             g_cMSSsnApp.IsMsSwitchOK());

			KillTimer(MCU_MCU_SYN_TIMER);
			KillTimer(MCU_MCU_WAITFORRSP_TIMER);	

            // guzh [9/14/2006] ���Ŀǰ��ͬ���ɹ�״̬(����ӵ����������)������Ϊ�л��ɹ�
            u8 bySwitchOk = g_cMSSsnApp.IsMsSwitchOK() ? 1 : 0;

			g_cMSSsnApp.SetMSSynOKFlag(FALSE);

			//��յ�ǰ������������Ϣ��¼
			memset((void*)&m_tMSCurSynState, 0, sizeof(m_tMSCurSynState));

			//[MSMag <->McuVc]ģ�黥�������� 2005-12-15
			g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);
            
            // guzh [4/17/2007] �л��ɹ���ͬʱ������ʼ����
            if (FALSE == g_cMSSsnApp.IsMSConfigInited())
            {
                PrintMSException("[CheckMSStateTimeOut] STANDBY->ACTIVE Unlock & LeaveMSInitLock: LocalMSInfo Type.%d State.%d\n", 
						  g_cMSSsnApp.GetLocalMSType(), emLocalMSState);

                g_cMSSsnApp.LeaveMSInitLock();	
            }
            else
            {
                if (bMpcEnable)
                {
                    //���ð�����Ϊ���ð�
                    //֪ͨ[McuVc]ģ��������״̬���
                    u8 byIsSwitchOk = g_cMSSsnApp.IsMsSwitchOK() ? 1 : 0;
                    ::OspPost(MAKEIID(AID_MCU_VC, CInstance::DAEMON), MCU_MSSTATE_EXCHANGE_NTF, &byIsSwitchOk, sizeof(u8));

                    //��ʱ�Զ��������ȫͬ�����Իָ��Լ������ͬ��
                    SetTimer(MCU_MCU_SYN_TIMER, MS_SYN_MIN_SPAN_TIMEOUT);
                    
                    PrintMSException("[CheckMSStateTimeOut] STANDBY->ACTIVE Unlock: LocalMSInfo Type.%d State.%d\n", 
						  g_cMSSsnApp.GetLocalMSType(), emLocalMSState);
                }
                else
                {
                    PrintMSException("[CheckMSStateTimeOut] ACTIVE->STANDBY Lock: LocalMSInfo Type.%d State.%d\n", 
                        g_cMSSsnApp.GetLocalMSType(), emLocalMSState);
			    }
            }
		}
	}
	else
	{
		m_byCheckTimes = 0;
	}

	SetTimer(MCU_MCU_CEHCK_MSSTATE_TIMER, g_dwMsCheckTime);
	return;
}


/*=============================================================================
    �� �� ���� PostMsgToOtherMcuDaemon
    ��    �ܣ� ����һ��MCU-Daemon������Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u16 wEvent, 
	           u8 *const pbyMsg, 
			   u16 wLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::PostMsgToOtherMcuDaemon(u16 wEvent, u8 *const pbyMsg, u16 wLen)
{	
	BOOL32 bRet = FALSE;
	if (INVALID_NODE == m_dwMcuNode)
	{
		PrintMSWarning("PostMsgToOtherMcuDaemon post wEvent.%d Failed, Invalid Node\n", wEvent);
		return bRet;
	}
	
	CServMsg cMsg;
	cMsg.SetEventId(wEvent);
	if (NULL != pbyMsg)
	{
		cMsg.SetMsgBody(pbyMsg, wLen);
	}

	if (post(MAKEIID(AID_MCU_MSMANAGERSSN, 1), wEvent, 
		cMsg.GetServMsg(), cMsg.GetServMsgLen(), m_dwMcuNode) < 0)
	{
		PrintMSWarning("PostMsgToOtherMcuDaemon post wEvent.%d Failed\n", wEvent);
	}
	else
	{
		bRet = TRUE;
	}
	
	return bRet;
}

/*=============================================================================
    �� �� ���� PostMsgToOtherMcuInst
    ��    �ܣ� ����һ��MCU-Inst������Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u16 wEvent, 
	           u8 *const pbyMsg, 
			   u16 wLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::PostMsgToOtherMcuInst(u16 wEvent, u8 *const pbyMsg, u16 wLen)
{
	BOOL32 bRet = FALSE;
	if (0 == m_dwMcuAppIId || INVALID_NODE == m_dwMcuNode)
	{
		PrintMSWarning("PostMsgToOtherMcuInst post wEvent.%d Failed, Invalid Node\n", wEvent);
		//printf("PostMsgToOtherMcuInst post wEvent.%d Failed, Invalid Node\n", wEvent);
		return bRet;
	}

	CServMsg cMsg;
	cMsg.SetEventId(wEvent);
	if (NULL != pbyMsg)
	{
		cMsg.SetMsgBody(pbyMsg, wLen);
	}

	if (post(m_dwMcuAppIId, wEvent, cMsg.GetServMsg(), cMsg.GetServMsgLen(), m_dwMcuNode) < 0)
	{
		PrintMSWarning("PostMsgToOtherMcuInst post wEvent.%d Failed\n", wEvent);
		//printf("PostMsgToOtherMcuInst post wEvent.%d Failed\n", wEvent);
	}
	else
	{
		bRet = TRUE;
	}
	
	return bRet;
}

/*=============================================================================
    �� �� ���� ConnectMasterMcu
    ��    �ܣ� ��λ��MCU������λ��mcu����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::ConnectMasterMcu( void )
{
	if (FALSE == OspIsValidTcpNode(m_dwMcuNode))
	{
		m_dwMcuNode = OspConnectTcpNode( htonl(g_cMSSsnApp.GetAnotherMcuIP()), 
										 g_cMSSsnApp.GetMcuConnectPort(),
                                         5,
                                         3,
                                         1000   // guzh [4/27/2007] ��������1�룬�����סҵ��
                                        );
		if (FALSE == OspIsValidTcpNode(m_dwMcuNode))
		{
			m_dwMcuNode = INVALID_NODE;
			PrintMSException("OspConnectTcpNode Another_MCU Failed!\n");
			SetTimer( MCU_MCU_CONNECT_TIMER, CONNECT_MASTERMCU_TIMEOUT );
			return FALSE;
		}	
		OspNodeDiscCBRegQ(m_dwMcuNode, GetAppID(), GetInsID());

        // guzh [4/27/2007] ������������⣬���̼��ʱ��(3*3)
        if (FALSE == g_cMSSsnApp.GetMSDetermineType())
        {
            OspSetHBParam(m_dwMcuNode, 3, 3);
        }
	}

	//�� Master_MCU ������� ע��
	u32 dwLocalIp = htonl(g_cMSSsnApp.GetLocalMcuIP());

    //[5/7/2013 liaokang] ׷����Ϣ u16 (�汾�ų���) + �汾���ַ���
	//PostMsgToOtherMcuDaemon(MCU_MCU_REGISTER_REQ, (u8*)&dwLocalIp, sizeof(dwLocalIp));
    CServMsg cSrvMsg;
    cSrvMsg.SetMsgBody((u8*)&dwLocalIp, sizeof(u32));
    u16 wVerLen = strlen(VER_MCU);
    cSrvMsg.CatMsgBody((u8*)&wVerLen,sizeof(u16));
    cSrvMsg.CatMsgBody((u8*)VER_MCU,wVerLen);

    PostMsgToOtherMcuDaemon(MCU_MCU_REGISTER_REQ, cSrvMsg.GetMsgBody(), cSrvMsg.GetMsgBodyLen());

	PrintMSCritical("Send Regiester Request(LocalIp0x%0x) To Another_MCU!\n", dwLocalIp);
	//printf("Send Regiester Request(LocalIp0x%x) To Another_MCU!\n", dwLocalIp);

	SetTimer( MCU_MCU_REGISTER_TIMER, REGISTER_MASTERMCU_TIMEOUT );

	return TRUE;
}


/*=============================================================================
    �� �� ���� ProcMcuMcuRegisterReq
    ��    �ܣ� ����λ��MCU��������ע������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CMessage * const pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuRegisterReq( CMessage * const pcMsg )
{
	if (MS_STATE_IDLE != CurState())
	{
		post( pcMsg->srcid, MCU_MCU_REGISTER_NACK, NULL, 0, pcMsg->srcnode );
		OspDisconnectTcpNode( pcMsg->srcnode );
		ClearInst();
        PrintMSException("ProcMcuMcuRegisterReq invalid ins state :%d.\n", CurState());
		return;
	}

	if (MCU_MSTYPE_SLAVE == g_cMSSsnApp.GetLocalMSType())
	{	
		post( pcMsg->srcid, MCU_MCU_REGISTER_NACK, NULL, 0, pcMsg->srcnode );
		OspDisconnectTcpNode( pcMsg->srcnode );
		ClearInst();
        PrintMSException("ProcMcuMcuRegisterReq invalid local MSType :%d.\n", g_cMSSsnApp.GetLocalMSType());
		return;
	}
	
	CServMsg cMsg( pcMsg->content, pcMsg->length );
	u32 dwRemoteIp = *(u32*)cMsg.GetMsgBody();	
	PrintMSInfo("ProcMcuMcuRegisterReq from ip.0x%0x Slave_MCU!\n", dwRemoteIp);

    //[5/7/2013 liaokang] �汾У��
    s8 achRemoteVerInfo[MAX_SOFT_VER_LEN + 1] = {0};
    u16 wVerLen = 0;
    if(cMsg.GetMsgBodyLen() > sizeof(u32) )
    {
        wVerLen = *(u16*)(cMsg.GetMsgBody() + sizeof(u32));
        memcpy(achRemoteVerInfo, (cMsg.GetMsgBody() + sizeof(u32) + sizeof(u16)), min(wVerLen, MAX_SOFT_VER_LEN));
    }

    if( FALSE == CompareStringElements(achRemoteVerInfo, VER_MCU) )
    {
        post( pcMsg->srcid, MCU_MCU_REGISTER_NACK, NULL, 0, pcMsg->srcnode );
        OspDisconnectTcpNode( pcMsg->srcnode );
        ClearInst();
		PrintMSException("ProcMcuMcuRegisterReq local mcu ver: %s, remote mcu ver %s, disconnect!\n", VER_MCU, achRemoteVerInfo);
        return;
    }

	//��֤
	if (dwRemoteIp != ntohl(g_cMSSsnApp.GetAnotherMcuIP()))
	{
		post( pcMsg->srcid, MCU_MCU_REGISTER_NACK, NULL, 0, pcMsg->srcnode );
		OspDisconnectTcpNode( pcMsg->srcnode );
		ClearInst();
		PrintMSException("ProcMcuMcuRegisterReq RemoteIp.0x%0x Failed\n", dwRemoteIp);
	}
	else
	{
		m_dwMcuNode   = pcMsg->srcnode;
		m_dwMcuAppIId = pcMsg->srcid;
		::OspNodeDiscCBRegQ( pcMsg->srcnode, GetAppID(), GetInsID() );
        g_cMSSsnApp.SetRemoteMpcConnected(TRUE);
		NEXTSTATE(MS_STATE_NORMAL);

        //[5/7/2013 liaokang] ׷�Ӱ汾��
		//PostMsgToOtherMcuInst(MCU_MCU_REGISTER_ACK, NULL, 0);        
        wVerLen = strlen(VER_MCU);
        cMsg.SetMsgBody((u8*)&wVerLen,sizeof(u16));
        cMsg.CatMsgBody((u8*)VER_MCU,wVerLen);
		PostMsgToOtherMcuInst(MCU_MCU_REGISTER_ACK, cMsg.GetMsgBody(), cMsg.GetMsgBodyLen());
		
		//֪ͨ �Է� ����MCU������λ���ͼ���ǰ������ʹ��״̬������������Э��
		u8 abyMSDNtf[2];
		abyMSDNtf[0] = (u8)g_cMSSsnApp.GetLocalMSType();
		abyMSDNtf[1] = (u8)g_cMSSsnApp.GetCurMSState();
		PostMsgToOtherMcuInst(MCU_MCU_MS_DETERMINE, abyMSDNtf, sizeof(abyMSDNtf));
	}

	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuRegisterRsp
    ��    �ܣ� ������λ��MCU��������ע������� Ӧ��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CMessage * const pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuRegisterRsp( CMessage * const pcMsg )
{
	if (MS_STATE_INIT != CurState())
	{
		return;
	}
	if (MCU_MSTYPE_SLAVE != g_cMSSsnApp.GetLocalMSType())
	{
		return;
	}

	CServMsg cMsg( pcMsg->content, pcMsg->length );

    //[5/7/2013 liaokang] �汾У��
    s8 achRemoteVerInfo[MAX_SOFT_VER_LEN + 1] = {0};
    if(cMsg.GetMsgBodyLen() > 0)
    {
        u16 wVerLen = *(u16*)(cMsg.GetMsgBody());
        memcpy(achRemoteVerInfo, (cMsg.GetMsgBody() + sizeof(u16)), min(wVerLen, MAX_SOFT_VER_LEN));
    }
    
    if( FALSE == CompareStringElements(achRemoteVerInfo, VER_MCU) )
    {
        OspDisconnectTcpNode( pcMsg->srcnode );
        ClearInst();
        PrintMSException("ProcMcuMcuRegisterRsp local mcu ver: %s, remote mcu ver %s, disconnect!\n", VER_MCU, achRemoteVerInfo);
        return;
    }

	if (MCU_MCU_REGISTER_ACK == pcMsg->event)
	{
		KillTimer(MCU_MCU_REGISTER_TIMER);

		m_dwMcuAppIId = pcMsg->srcid;
        g_cMSSsnApp.SetRemoteMpcConnected(TRUE);
		NEXTSTATE(MS_STATE_NORMAL);

		//֪ͨ �Է� ����MCU������λ���ͼ���ǰ������ʹ��״̬������������Э��
		u8 abyMSDNtf[2];
		abyMSDNtf[0] = g_cMSSsnApp.GetLocalMSType();
		abyMSDNtf[1] = g_cMSSsnApp.GetCurMSState();
		PostMsgToOtherMcuInst(MCU_MCU_MS_DETERMINE, abyMSDNtf, sizeof(abyMSDNtf));

		PrintMSInfo("ProcMcuMcuRegisterRsp Ack From Another_MCU!\n");
	}
	else
	{
		PrintMSWarning("ProcMcuMcuRegisterRsp Nack From Another_MCU!\n");
	}
	//�����������ȴ�Զ�˶Ͽ������ӣ��յ�����֪ͨ�����³���

	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuMSDetermine
    ��    �ܣ� �Զ�MCU������λ���ͼ���ǰ������ʹ��״̬֪ͨ������������Э��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CMessage * const pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuMSDetermine( CMessage * const pcMsg )
{
	if (MS_STATE_NORMAL != CurState())
	{
		return;
	}

	CServMsg cMsg( pcMsg->content, pcMsg->length );
	emMCUMSType  emRemoteMSType  = (emMCUMSType)(*cMsg.GetMsgBody());
	emMCUMSState emRemoteMSState = (emMCUMSState)(*(cMsg.GetMsgBody()+1));
	emMCUMSType  emLocalMSType   = g_cMSSsnApp.GetLocalMSType();
	emMCUMSState emLocalMSState  = g_cMSSsnApp.GetCurMSState();
	BOOL32 bDetermineok = TRUE;
	
	LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[ProcMcuMcuMSDetermine] Local<Type.%d, State.%d>, Remote<Type.%d, State.%d>!\n",
								emLocalMSType, emLocalMSState, emRemoteMSType, emRemoteMSState);
	//������Э��
	bDetermineok = g_cMSSsnApp.DetermineMSlave(emLocalMSType, emLocalMSState, 
		                                       emRemoteMSType, emRemoteMSState);

	if (bDetermineok)
	{
		u8 byRemoteMSState = (u8)emRemoteMSState;
		PostMsgToOtherMcuInst(MCU_MCU_MS_RESULT, &byRemoteMSState, sizeof(byRemoteMSState));
		g_cMSSsnApp.SetCurMSState(emLocalMSState);

		//������Э�̳ɹ�
		KillTimer(MCU_MCU_SYN_TIMER);
		g_cMSSsnApp.SetMSSynOKFlag(FALSE);

		PrintMSInfo("[ProcMcuMcuMSDetermine] Determineok Unlock: LocalMSInfo Type.%d State.%d RemoteMSState.%d\n", 
				     g_cMSSsnApp.GetLocalMSType(), emLocalMSState, emRemoteMSState);

		//printf("[ProcMcuMcuMSDetermine] Determineok Unlock: LocalMSInfo Type.%d State.%d RemoteMSState.%d\n", 
		//		     g_cMSSsnApp.GetLocalMSType(), emLocalMSState, emRemoteMSState);
	}
	else
	{
		PrintMSException("[ProcMcuMcuMSDetermine] Determine Err: LocalMSType.%d LocalMSState.%d RemoteMSType.%d RemoteMSState.%d\n", 
			             g_cMSSsnApp.GetLocalMSType(), emLocalMSState, emRemoteMSType, emRemoteMSState);

		//printf("[ProcMcuMcuMSDetermine] Determine Err: LocalMSType.%d LocalMSState.%d RemoteMSType.%d RemoteMSState.%d\n", 
		//	             g_cMSSsnApp.GetLocalMSType(), emLocalMSState, emRemoteMSType, emRemoteMSState);

		//������Э���쳣���������䣬�ѳ�ʼ��������ǣ�������ΪStandby......
		if (FALSE == g_cMSSsnApp.IsMSConfigInited())
		{
			g_cMSSsnApp.SetCurMSState( MCU_MSSTATE_STANDBY );
		}
		OspDisconnectTcpNode(m_dwMcuNode);
		ClearInst();
	}

	if (MCU_MSSTATE_ACTIVE == emLocalMSState)
	{
		//��ʱ�Զ��������ȫͬ������������ͬ��
		SetTimer(MCU_MCU_SYN_TIMER, MS_SYN_NOW_SPAN_TIMEOUT);
	}
	
	PrintMSInfo("ProcMcuMcuMSDetermine bDetermineok.%d LocalMSState.%d RemoteMSState.%d\n", 
		         bDetermineok, emLocalMSState, emRemoteMSState);

    //printf("ProcMcuMcuMSDetermine bDetermineok.%d LocalMSState.%d RemoteMSState.%d\n", 
	//	         bDetermineok, emLocalMSState, emRemoteMSState);

    // guzh [9/15/2006] ����Э�̳ɹ���֪ͨMCU����,����MPC����
    if (bDetermineok)
    {
        u8 byState = BOARD_INLINE;
		post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_STANDBYMPCSTATUS_NOTIFY, 
			&byState, sizeof(byState) );

	    PrintMSInfo("[ProcMcuMcuMSDetermine] Notify Other MPC Status.%d\n", 
		             byState);
    }

	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuMSResult
    ��    �ܣ� MCU֮�� ������Э�̽��֪ͨ����Э�̺�ı���������״̬��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CMessage * const pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuMSResult( CMessage * const pcMsg )
{
	if (MS_STATE_NORMAL != CurState())
	{
		return;
	}
	
	CServMsg cMsg( pcMsg->content, pcMsg->length );
	emMCUMSState emLocalMSState = (emMCUMSState)(*cMsg.GetMsgBody());
	
	if (emLocalMSState != g_cMSSsnApp.GetCurMSState())
	{
		OspDisconnectTcpNode(m_dwMcuNode);
		ClearInst();
		PrintMSException("[ProcMcuMcuMSResult] LocalMSType.%d ResultMSType.%d\n", 
		                 g_cMSSsnApp.GetCurMSState(), emLocalMSState);
	}
	
	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuSynTimeOut
    ��    �ܣ� ���� ���ð�MCU���ð�MCU ���ݵ�����ʱ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuSynTimeOut( void )
{
	if (MS_STATE_NORMAL != CurState() || 
		MCU_MSSTATE_ACTIVE != g_cMSSsnApp.GetCurMSState())
	{
        PrintMSException("[ProcMcuMcuSynTimeOut] invalid ins state :%d, CurMsState :%d.\n",
                         CurState(), g_cMSSsnApp.GetCurMSState());

		//printf("[ProcMcuMcuSynTimeOut] invalid ins state :%d, CurMsState :%d.\n",
         //               CurState(), g_cMSSsnApp.GetCurMSState());
		return;
	}
	
	//�ر�����ʱ���
	KillTimer(MCU_MCU_WAITFORRSP_TIMER);

    //���ð�ͬ������ʱ��[MSMag <->McuVc]ģ�黥�������� 2005-12-15
    g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);
    
    PrintMSWarning("[ProcMcuMcuSynTimeOut] Waitfor_SynOpr_Rsp_Timeout: UnLock\n");
    //printf("[ProcMcuMcuSynTimeOut] Waitfor_SynOpr_Rsp_Timeout: UnLock\n");

	if (m_tMSCurSynState.m_emSynType > emSynDataStart)
	{
		g_cMSSsnApp.SetMSSynOKFlag(FALSE);
		//��յ�ǰ������������Ϣ��¼
		memset((void*)&m_tMSCurSynState, 0, sizeof(m_tMSCurSynState));
	}
	
    //̽��Զ��Ƿ���ڣ���������ʼ����ͬ������
	if (!PostMsgToOtherMcuInst(MCU_MCU_PROBE_REQ, NULL, 0))
    {
        PrintMSCritical("[ProcMcuMcuSynTimeOut] Post_Probe_Req failed m_dwMcuNode.%u\n", m_dwMcuNode);
		//printf("[ProcMcuMcuSynTimeOut] Post_Probe_Req failed m_dwMcuNode.%u\n", m_dwMcuNode);
    }

	//���Զ�δ�����ȫͬ�����Իָ��Լ������ͬ���������������Լ������ͬ��
	if (g_cMSSsnApp.IsMSSynOK())
	{
		SetTimer(MCU_MCU_SYN_TIMER, g_cMcuVcApp.GetMsSynTime()*1000 );
	}
	else
	{
		SetTimer(MCU_MCU_SYN_TIMER, MS_SYN_MIN_SPAN_TIMEOUT);
	}

	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuWaitforRspTimeOut
    ��    �ܣ� ���� MCU֮�� �ȴ��Զ˵�����Ӧ�� �ĳ�ʱ��ʱ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CMessage * const pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuWaitforRspTimeOut( CMessage * const pcMsg )
{	
	switch( CurState() )
	{
	case MS_STATE_NORMAL:
		
		KillTimer(pcMsg->event);

		if (MCU_MCU_WAITFORRSP_TIMER == pcMsg->event && 
			MCU_MSSTATE_ACTIVE == g_cMSSsnApp.GetCurMSState())
		{
			//MCU_MCU_START_SYN_REQ
			//MCU_MCU_SYNNING_DATA_REQ
			//��ʱ�Զ��������ȫͬ�����Իָ��Լ������ͬ��
			g_cMSSsnApp.SetMSSynOKFlag(FALSE);
			KillTimer(MCU_MCU_SYN_TIMER);
			SetTimer(MCU_MCU_SYN_TIMER, MS_SYN_MIN_SPAN_TIMEOUT);

			//��յ�ǰ������������Ϣ��¼
			memset((void*)&m_tMSCurSynState, 0, sizeof(m_tMSCurSynState));

			//���ð�ͬ������ʱ��[MSMag <->McuVc]ģ�黥�������� 2005-12-15
			g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);
			
			PrintMSWarning("[ProcMcuMcuWaitforRspTimeOut] Waitfor_SynOpr_Rsp_Timeout: UnLock\n");
			//printf("[ProcMcuMcuWaitforRspTimeOut] Waitfor_SynOpr_Rsp_Timeout: UnLock\n");
		}
        else if (MCU_MCU_WAITFORRSP_TIMER == pcMsg->event && 
			MCU_MSSTATE_STANDBY == g_cMSSsnApp.GetCurMSState())
        {
			//�����timeout������
			g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);
			PrintMSWarning("[ProcMcuMcuWaitforRspTimeOut] StandBy board Waitfor_SynOpr_Rsp_Timeout: UnLock\n");

		}
		else
		{
     
            PrintMSException("[ProcMcuMcuWaitforRspTimeOut] Waitfor_SynOpr_Rsp_Timeout, CurMSState:%d\n", 
                             g_cMSSsnApp.GetCurMSState());

			//printf("[ProcMcuMcuWaitforRspTimeOut] Waitfor_SynOpr_Rsp_Timeout, CurMSState:%d\n", 
            //                 g_cMSSsnApp.GetCurMSState());
        }
		return;
	default:
		break;
	}

	return;
}


/*=============================================================================
�� �� ���� ProcMcuMcuProbeReq
��    �ܣ� �����򱸰�̽����Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage * const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/4/4  4.0			������                  ����
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuProbeReq( void )
{
    if (g_cMSSsnApp.GetCurMSState() == MCU_MSSTATE_STANDBY)
    {
        PostMsgToOtherMcuInst(MCU_MCU_PROBE_ACK, NULL, 0);
		//���������������ݵ�ʱ��ҲӦ����ס��������ܳ����������ݲ�һ�µ����������vc��������vc������
		//10s timeout,10s������ûͬ����� vc��ms����
		SetTimer(MCU_MCU_WAITFORRSP_TIMER,10000);
		//���ǵ��������⣬���ܳ���ǰһ��timerû������һ��probe�ֵ�����������Ƚ�������
		g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);
		g_cMSSsnApp.EnterMSSynLock(AID_MCU_MSMANAGERSSN);
		PrintMSInfo("[ProcMcuMcuProbeReq] Rcv_SynOpr_Req: Lock\n");
    }    
    return;
}

/*=============================================================================
�� �� ���� ProcMcuMcuProbeAck
��    �ܣ� ���幤����������ͬ����������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage * const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/4/4  4.0			������                  ����
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuProbeAck( void )
{
    //����ͬ������֪ͨ�Զˡ����˵�ǰ����״̬�����Ա�֤ͬ��ǰ����̬��
    TMSSynInfoReq tMSSynInfoReq;
    g_cMcuVcApp.GetVcDeamonEnvState(tMSSynInfoReq.m_tEnvState);
    tMSSynInfoReq.m_byInited = g_cMSSsnApp.IsMSConfigInited() ? 1 : 0;
    if (PostMsgToOtherMcuInst(MCU_MCU_START_SYN_REQ, (u8*)&tMSSynInfoReq, sizeof(tMSSynInfoReq)))
    {
        //[MSMag <->McuVc]ģ�黥�������� 2005-12-15
		g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);//�����������������ack�����ᵼ����ס
        g_cMSSsnApp.EnterMSSynLock(AID_MCU_MSMANAGERSSN);
        
        PrintMSInfo("[ProcMcuMcuProbeAck] Post_SynOpr_Req: Lock\n");
        
        //��������ʱ���       
        //���̶�ʱ������ֹ��������ʱ���޷���⵽�Զ˶���
        SetTimer(MCU_MCU_WAITFORRSP_TIMER, MS_WAITFOTRSP_TIMEOUT, (u32)MCU_MCU_START_SYN_REQ);
    }
    else
    {
        PrintMSCritical("[ProcMcuMcuProbeAck] Post_SynOpr_Req failed m_dwMcuNode.%u\n", m_dwMcuNode);
    }

    return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuStartSynReq
    ��    �ܣ� ���� ���ð�MCU���ð�MCU ��ʼ���ݵ��� ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CMessage * const pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/12  4.0		    ����                  ����
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuStartSynReq( CMessage * const pcMsg )
{
	if (MS_STATE_NORMAL != CurState() || 
		MCU_MSSTATE_STANDBY != g_cMSSsnApp.GetCurMSState())
	{
		PrintMSWarning("[ProcMcuMcuStartSynReq] Post_SynOpr_Nack: CurInstState.%d CurMSState.%d\n", 
			            CurState(), g_cMSSsnApp.GetCurMSState());

		//printf("[ProcMcuMcuStartSynReq] Post_SynOpr_Nack: CurInstState.%d CurMSState.%d\n", 
		//	            CurState(), g_cMSSsnApp.GetCurMSState());

		PostMsgToOtherMcuInst(MCU_MCU_START_SYN_NACK, NULL, 0);
		return;
	}

	CServMsg cMsg( pcMsg->content, pcMsg->length );
	if (cMsg.GetMsgBodyLen() != sizeof(TMSSynInfoReq)) 
	{
		PrintMSException("[ProcMcuMcuStartSynReq] Invalid msgLen != sizeof(TMSSynInfoReq).%d\n", sizeof(TMSSynInfoReq));
	
		//printf("[ProcMcuMcuStartSynReq] Invalid msgLen != sizeof(TMSSynInfoReq).%d\n", sizeof(TMSSynInfoReq));
		return;
	}

	TMSSynInfoReq *ptMSSynInfoReq = (TMSSynInfoReq*)cMsg.GetMsgBody();
	m_bRemoteInited = (1 == ptMSSynInfoReq->m_byInited) ? TRUE : FALSE;

	//�յ�ͬ������������δ��ɳ�ʼ����������ͬ����
	//              ����������ɳ�ʼ�������ա����˵�ǰ����״̬����һ��������ͬ�����Ա�֤ͬ��ǰ����̬
	u8 byInited    = g_cMSSsnApp.IsMSConfigInited() ? 1 : 0;
	BOOL32 bPrintErr = (MS_DEBUG_INFO <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;
    TMSSynState tMSSynState;
	if (1 == byInited && 
		FALSE == g_cMcuVcApp.IsEqualToVcDeamonEnvState(&ptMSSynInfoReq->m_tEnvState, bPrintErr, &tMSSynState))
	{
        //����������Ϣ��һ��
        if (!tMSSynState.IsSynSucceed())
        {
			//NACK���������
			g_cMSSsnApp.SetMSSynOKFlag(FALSE);

			PostMsgToOtherMcuInst(MCU_MCU_START_SYN_NACK, (u8*)&tMSSynState, sizeof(TMSSynState) );
           
			PrintMSInfo( "[ProcMcuMcuStartSynReq] EntityType:%d, EntityId:%d state conflict!\n",
			          tMSSynState.GetEntityType(), tMSSynState.GetEntityId() );

			//printf("[ProcMcuMcuStartSynReq] EntityType:%d, EntityId:%d state conflict!\n",
			 //         tMSSynState.GetEntityType(), tMSSynState.GetEntityId());

		}

        //������̬��ʧ�����ous��
        if (g_cMSSsnApp.GetCurMSState() == MCU_MSSTATE_STANDBY)
        {
            g_cMSSsnApp.SetMpcOusLed(TRUE);
        }
		//g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);
		PrintMSInfo("[ProcMcuMcuStartSynReq] IsEqualToVcDeamonEnvState Failed, byInited.%d, Post_SynOpr_Nack\n", byInited);
		//printf("[ProcMcuMcuStartSynReq] IsEqualToVcDeamonEnvState Failed, byInited.%d, Post_SynOpr_Nack\n", byInited);
	}
	else
	{
        TMSSynInfoRsp tRsp;
        tRsp.m_byInited = byInited;
		PostMsgToOtherMcuInst(MCU_MCU_START_SYN_ACK, (u8*)&tRsp, sizeof(tRsp));
		
		// ��¼�������MCS����״̬, zgc, 2008-03-25
		g_cMSSsnApp.SetMSVcMCState( ptMSSynInfoReq->m_tEnvState.m_tMCState );
		
		//[MSMag <->McuVc]ģ�黥�������� 2005-12-15

		//IncompletePro: ���ð����������
		//g_cMSSsnApp.EnterMSSynLock(AID_MCU_MSMANAGERSSN);
		
		//��յ�ǰ������������Ϣ��¼
		memset((void*)&m_tMSCurSynState, 0, sizeof(m_tMSCurSynState));

		PrintMSInfo("[ProcMcuMcuStartSynReq] IsEqualToVcDeamonEnvState OK, byInited.%d, Post_SynOpr_Ack, CurTick :%u\n", 
            byInited, OspTickGet());

		//printf("[ProcMcuMcuStartSynReq] IsEqualToVcDeamonEnvState OK, byInited.%d, Post_SynOpr_Ack, CurTick :%u\n", 
         //   byInited, OspTickGet());
	}
	
	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuStartSynRsp
    ��    �ܣ� ���� ���ð�MCU�����ð�MCU ��ʼ���ݵ��� �����Ӧ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CMessage * const pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuStartSynRsp( CMessage * const pcMsg )
{
	if (MS_STATE_NORMAL != CurState() || 
		MCU_MSSTATE_ACTIVE != g_cMSSsnApp.GetCurMSState())
	{
		PrintMSWarning("[ProcMcuMcuStartSynRsp] Ingore: CurInstState.%d CurMSState.%d\n", 
			            CurState(), g_cMSSsnApp.GetCurMSState());

		//printf("[ProcMcuMcuStartSynRsp] Ingore: CurInstState.%d CurMSState.%d\n", 
		//	            CurState(), g_cMSSsnApp.GetCurMSState());
		return;
	}
	
	//�ر�����ʱ���
	KillTimer(MCU_MCU_WAITFORRSP_TIMER);
    PrintMSInfo("[ProcMcuMcuStartSynRsp] kill timer MCU_MCU_WAITFORRSP_TIMER.\n");
	//printf("[ProcMcuMcuStartSynRsp] kill timer MCU_MCU_WAITFORRSP_TIMER.\n");

	CServMsg cMsg( pcMsg->content, pcMsg->length );

	//���ð�ͬ�����󱻾ܣ�
	if (MCU_MCU_START_SYN_NACK == cMsg.GetEventId())
	{	
		//��ʱ�Զ������½�����ȫͬ�����Իָ��Լ������ͬ��
		g_cMSSsnApp.SetMSSynOKFlag(FALSE);
		KillTimer(MCU_MCU_SYN_TIMER);
		SetTimer(MCU_MCU_SYN_TIMER, MS_SYN_MIN_SPAN_TIMEOUT);

        //ȡͬ��ʧ�ܵ�״̬��Ϣ
        g_cMSSsnApp.SetCurMSSynState( *(TMSSynState*)cMsg.GetMsgBody() );
        
		//[MSMag <->McuVc]ģ�黥�������� 2005-12-15
		g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);

		PrintMSException("[ProcMcuMcuStartSynRsp] Rcv_SynOpr_Nack: Unlock\n");
		//printf("[ProcMcuMcuStartSynRsp] Rcv_SynOpr_Nack: Unlock\n");
		return;
	}
	else
	{
		if (cMsg.GetMsgBodyLen() != sizeof(TMSSynInfoRsp)) 
		{
			PrintMSException("[ProcMcuMcuStartSynRsp] Invalid msgLen != sizeof(TMSSynInfoRsp).%d\n", sizeof(TMSSynInfoRsp));
			//printf("[ProcMcuMcuStartSynRsp] Invalid msgLen != sizeof(TMSSynInfoRsp).%d\n", sizeof(TMSSynInfoRsp));
			return;
		}
		TMSSynInfoRsp *ptSynInfoRsp = (TMSSynInfoRsp*)cMsg.GetMsgBody();
		m_bRemoteInited = (1 == ptSynInfoRsp->m_byInited) ? TRUE : FALSE;
	}

	PrintMSInfo("[ProcMcuMcuStartSynRsp] Rcv_SynOpr_Ack StartSyn, RemoteInited :%d, CurTicks :%u\n", 
                m_bRemoteInited, OspTickGet());

	//printf("[ProcMcuMcuStartSynRsp] Rcv_SynOpr_Ack StartSyn, RemoteInited :%d, CurTicks :%u\n", 
     //           m_bRemoteInited, OspTickGet());

	//��ʼ������ͬ������
	SendSynData(TRUE);
	
	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuEndSynNtf
    ��    �ܣ� ���� ���ð�MCU���ð�MCU �������ݵ��� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CMessage * const pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuEndSynNtf( void )
{
	if (MS_STATE_NORMAL != CurState() || 
		MCU_MSSTATE_STANDBY != g_cMSSsnApp.GetCurMSState())
	{
		PrintMSWarning("[ProcMcuMcuEndSynNtf] Ingore: CurInstState.%d CurMSState.%d\n", 
			            CurState(), g_cMSSsnApp.GetCurMSState());

		//printf("[ProcMcuMcuEndSynNtf] Ingore: CurInstState.%d CurMSState.%d\n", 
		//	            CurState(), g_cMSSsnApp.GetCurMSState());
		return;
	}
	
	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		//֪ͨ���App ��������ģ���ʼ������
		g_cMSSsnApp.LeaveMSInitLock();
	}
	else
	{
		g_cMSSsnApp.SetMSSynOKFlag(TRUE);

        //����ͬ���ɹ�����ous��
        // zw [2008/10/13] �Լ���һЩ��Ϣͬ��������
        if (g_cMSSsnApp.GetCurMSState() == MCU_MSSTATE_STANDBY)
        {
            g_cMSSsnApp.SetMpcOusLed(FALSE);
        }     
	}

	//[MSMag <->McuVc]ģ�黥�������� 2005-12-15
	//IncompletePro: ���ð�Ľ�������
	//g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);

	//��յ�ǰ������������Ϣ��¼
	memset((void*)&m_tMSCurSynState, 0, sizeof(m_tMSCurSynState));
	//�����timeout������
	g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);
	PrintMSInfo("[ProcMcuMcuEndSynNtf] EndSyn,Standby unlock, CurTicks :%u\n", OspTickGet());
	//printf("[ProcMcuMcuEndSynNtf] EndSyn, CurTicks :%u\n", OspTickGet());

	return;
}


/*=============================================================================
    �� �� ���� ProcMcuMcuSynningDataReq
    ��    �ܣ� ���� ��λ��MCU��λ��MCU ʵ���а���������ݵ��� ͬ������
	           ����1.ϵͳʱ�䡢mcu����������Ϣ��������Ϣ���û���Ϣ����ַ����debug�ļ� �Լ�
			       2.ҵ���ڴ�����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CMessage * const pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuSynningDataReq( CMessage * const pcMsg )
{
	if (MS_STATE_NORMAL != CurState() || 
		MCU_MSSTATE_STANDBY != g_cMSSsnApp.GetCurMSState())
	{
		PrintMSWarning("[ProcMcuMcuSynningDataReq] Ingore: CurInstState.%d CurMSState.%d\n", 
			            CurState(), g_cMSSsnApp.GetCurMSState());
		return;
	}
	
	if( NULL == pcMsg )
	{
		PrintMSWarning("ProcMcuMcuSynningDataReq The received msg's pointer is NULL!\n");
		return;
	}

	//��֡
	if (FALSE == RecvCustomMsgFromOtherMcu(pcMsg))
	{
		return;
	}

	PrintMSInfo("[ProcMcuMcuSynningDataReq] RecvCustomMsgFromOtherMcu OK: MsgID.%d FrmLen.%u FrmSN.%d\n", 
		            m_tRcvFrmHead.m_byFrmType, m_tRcvFrmHead.m_dwFrmLen, m_tRcvFrmHead.m_wFrmSN);

	
	//ͬ������Ӧͬ�����
	TMSSynDataRsp tMSDataRsp;
	tMSDataRsp.m_byMsgID  = m_tRcvFrmHead.m_byFrmType;
	tMSDataRsp.m_dwFrmLen = htonl(m_tRcvFrmHead.m_dwFrmLen);
	tMSDataRsp.m_wFrmSN   = htons(m_tRcvFrmHead.m_wFrmSN);
	if (DealOneCustomSynMsg(&m_tRcvFrmHead) )
	{
		tMSDataRsp.m_byRetVal = (u8)emMSReturnValue_Ok;

		PrintMSInfo("[ProcMcuMcuSynningDataReq] DealOneCustomSynMsg OK: MsgID.%d FrmLen.%u FrmSN.%d\n", 
			         m_tRcvFrmHead.m_byFrmType, m_tRcvFrmHead.m_dwFrmLen, m_tRcvFrmHead.m_wFrmSN);
	}
	else
	{
		tMSDataRsp.m_byRetVal = (u8)emMSReturnValue_Error;
		
		//IncompletePro: ���ð�Ľ�������
		//g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);

		//��յ�ǰ������������Ϣ��¼
		//memset((void*)&m_tMSCurSynState, 0, sizeof(m_tMSCurSynState));

        //ͬ��ʧ�ܺ����ous��
        if (g_cMSSsnApp.GetCurMSState() == MCU_MSSTATE_STANDBY)
        {
            g_cMSSsnApp.SetMpcOusLed(TRUE);
        }
		
		PrintMSWarning("[ProcMcuMcuSynningDataReq] DealOneCustomSynMsg Failed!\n");
	}
	PostMsgToOtherMcuInst(MCU_MCU_SYNNING_DATA_RSP, (u8*)&tMSDataRsp, sizeof(tMSDataRsp));

	//��մ�������������֡��Ϣ
	m_dwRcvFrmLen = 0;
	m_tRcvFrmHead.m_byFrmType = emSynDataStart;
	m_tRcvFrmHead.m_dwFrmLen  = 0;
	
	return;
}

/*=============================================================================
    �� �� ���� ProcMcuMcuSynningDataRsp
    ��    �ܣ� ��λ��MCU����λ��MCU ʵ���а���������ݵ��� ͬ����� ��Ӧ
	           ����1.ϵͳʱ�䡢mcu����������Ϣ��������Ϣ���û���Ϣ����ַ����debug�ļ� �Լ�
			       2.ҵ���ڴ�����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CMessage * const pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuSynningDataRsp( CMessage * const pcMsg )
{
	if (MS_STATE_NORMAL != CurState() || 
		MCU_MSSTATE_ACTIVE != g_cMSSsnApp.GetCurMSState())
	{
		return;
	}

	CServMsg cMsg( pcMsg->content, pcMsg->length );
	if (cMsg.GetMsgBodyLen() < sizeof(TMSSynDataRsp)) 
	{
		PrintMSException("[ProcMcuMcuSynningDataRsp] Invalid msgLen < sizeof(TMSSynDataRsp).%d\n", sizeof(TMSSynDataRsp));
		//printf("[ProcMcuMcuSynningDataRsp] Invalid msgLen < sizeof(TMSSynDataRsp).%d\n", sizeof(TMSSynDataRsp));
		return;
	}

	TMSSynDataRsp *ptMSDataRsp = (TMSSynDataRsp *)cMsg.GetMsgBody();
	ptMSDataRsp->m_dwFrmLen    = ntohl(ptMSDataRsp->m_dwFrmLen);
	ptMSDataRsp->m_wFrmSN      = ntohs(ptMSDataRsp->m_wFrmSN);
	
	PrintMSVerbose("SynnData: RetVal.%d MsgID.%d FrmLen.%u FrmSN.%d\n", 
		         ptMSDataRsp->m_byRetVal, ptMSDataRsp->m_byMsgID, ptMSDataRsp->m_dwFrmLen, ptMSDataRsp->m_wFrmSN);
	
	//printf("[ProcMcuMcuSynningDataRsp] RetVal.%d MsgID.%d FrmLen.%u FrmSN.%d\n", 
	//	         ptMSDataRsp->m_byRetVal, ptMSDataRsp->m_byMsgID, ptMSDataRsp->m_dwFrmLen, ptMSDataRsp->m_wFrmSN);
	
	//���й���У�飬��������
	if (m_tSndFrmHead.m_byFrmType != ptMSDataRsp->m_byMsgID || 
		m_tSndFrmHead.m_wFrmSN != ptMSDataRsp->m_wFrmSN)
	{
		PrintMSException("[ProcMcuMcuSynningDataRsp] DataRsp Overdue! ReqMsgID.%d ReqFrmSN.%d RspMsgID.%d RspFrmSN.%d!\n", 
			              m_tSndFrmHead.m_byFrmType, m_tSndFrmHead.m_wFrmSN, 
						  ptMSDataRsp->m_byMsgID, ptMSDataRsp->m_wFrmSN);

		//printf("[ProcMcuMcuSynningDataRsp] DataRsp Overdue! ReqMsgID.%d ReqFrmSN.%d RspMsgID.%d RspFrmSN.%d!\n", 
		//			   	  m_tSndFrmHead.m_byFrmType, m_tSndFrmHead.m_wFrmSN, 
		//				  ptMSDataRsp->m_byMsgID, ptMSDataRsp->m_wFrmSN);
		return;
	}

	//�ر�����ʱ���
	KillTimer(MCU_MCU_WAITFORRSP_TIMER);

	//���ð�����ͬ������ʧ�ܣ��ݲ�֧���ش����ȴ��´�ͬ��
	if ((u8)emMSReturnValue_Ok != ptMSDataRsp->m_byRetVal)
	{
		//��ʱ�Զ������½�����ȫͬ�����Իָ��Լ������ͬ��
		g_cMSSsnApp.SetMSSynOKFlag(FALSE);
		KillTimer(MCU_MCU_SYN_TIMER);
		SetTimer(MCU_MCU_SYN_TIMER, MS_SYN_MIN_SPAN_TIMEOUT);
		
		//[MSMag <->McuVc]ģ�黥�������� 2005-12-15
		g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);

		PrintMSException("SynnData:RetVal.%d!\n", ptMSDataRsp->m_byRetVal);
		//printf("[ProcMcuMcuSynningDataRsp] RetVal.%d!\n", ptMSDataRsp->m_byRetVal);
		return;
	}

	//��������������ͬ������, �б��Ƿ��Ѿ����ȫ������ͬ��
	if (SendSynData(FALSE))
	{
		PostMsgToOtherMcuInst(MCU_MCU_END_SYN_NTF, NULL, 0);
		
		if (FALSE == g_cMSSsnApp.IsMSConfigInited())
		{
			//֪ͨ���App ��������ģ���ʼ������
			g_cMSSsnApp.LeaveMSInitLock();
		}
		else
		{
			//��ʱ�Զ�����ȫͬ�����������Լ������ͬ��
			if (m_bRemoteInited)
			{
				g_cMSSsnApp.SetMSSynOKFlag(TRUE); 
                
                if ( !g_cMSSsnApp.IsMsSwitchOK() )
                {
                    // guzh [9/14/2006] ����ʱ����Ϊ������Ҳ�Ѿ��ɹ��л�
                    g_cMSSsnApp.SetMsSwitchOK(TRUE);
                }

                //�������ͬ��ʧ��״̬��Ϣ
                TMSSynState tMSSynState;                
                if ( !g_cMSSsnApp.GetCurMSSynState().IsSynSucceed() )
                {
                    g_cMSSsnApp.SetCurMSSynState(tMSSynState);
                }
			}
		}

		//���ð����ͬ����[MSMag <->McuVc]ģ�黥�������� 2005-12-15
		g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);

		PrintMSInfo("[ProcMcuMcuSynningDataRsp] Finish_SynOpr: Unlock, CurTick :%u\n", OspTickGet());
		//printf("[ProcMcuMcuSynningDataRsp] Finish_SynOpr: Unlock, CurTick :%u\n", OspTickGet());
	}
	
	return;
}


/*=============================================================================
    �� �� ���� DealOneCustomSynMsg
    ��    �ܣ� �������ݵ���ʱ�� һ������������Զ�������ͬ����Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TMSFrmHead *ptMSFrmHead �Զ�����Ϣ�����Ϣͷ
    �� �� ֵ�� BOOL32
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::DealOneCustomSynMsg( TMSFrmHead *ptMSFrmHead )
{
	BOOL32 bRet = FALSE;
	if( NULL == ptMSFrmHead )
	{
		PrintMSWarning("[DealOneCustomSynMsg] The received msg's pointer is NULL!\n");
		return bRet;
	}

	switch( ptMSFrmHead->m_byFrmType ) 
	{
	case emSysTime:
		bRet = SaveSynDataOfSysTime(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emSysCritData:
        bRet = SaveSynDataOfSysCritData(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
        break;
	case emCfgFile:
		bRet = SaveSynDataOfCfgFile(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emDebugFile:
		bRet = SaveSynDataOfDebugFile(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
    case emAddrbookFile_Utf8:
        bRet = SaveSynDataOfAddrbookFileUtf8(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emAddrbookFile:
		bRet = SaveSynDataOfAddrbookFile(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emConfinfoFile:
		bRet = SaveSynDataOfConfinfoFile(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emUnProcConfinfoFile:
		bRet = SaveSynDataOfUnProcConfinfoFile(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emLoguserFile:
		bRet = SaveSynDataOfLoguserFile(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emVCSLoguserFile:
		bRet = SaveSynDataOfVCSLoguserFile(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emVCSUserTaskFile:
		bRet = SaveSynDataOfVCSUserTaskFile(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
    case emUserExFile:  // �û�����չ��Ϣ
        bRet = SaveSynDataOfUserExFile(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
        break;

        
	case emVcDeamonPeriEqpData:
		bRet = SaveSynDataOfVcDeamonPeriEqpData(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emVcDeamonMCData:
		bRet = SaveSynDataOfVcDeamonMCData(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emVcDeamonMpData:
		bRet = SaveSynDataOfVcDeamonMpData(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emVcDeamonMtadpData:
		bRet = SaveSynDataOfVcDeamonMtadpData(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emVcDeamonTemplateData:
		bRet = SaveSynDataOfVcDeamonTemplateData(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emVcDeamonOtherData:
		bRet = SaveSynDataOfVcDeamonOtherData(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emAllVcInstState:
		bRet = SaveSynDataOfAllVcInstState(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emOneVcInstConfMtTableData:
		bRet = SaveSynDataOfOneVcInstConfMtTableData(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emOneVcInstConfSwitchTableData:
		bRet = SaveSynDataOfOneVcInstConfSwitchTableData(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emOneVcInstConfOtherMcuTableData:
		bRet = SaveSynDataOfOneVcInstConfOtherMcTableData(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emOneVcInstOtherData:
		bRet = SaveSynDataOfOneVcInstOtherData(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
		
	default:
		PrintMSCritical("[MSManager] Wrong Custom MsgId%u(%s) received in DealOneCustomSynMsg()!\n", 
			             ptMSFrmHead->m_byFrmType, ::OspEventDesc(ptMSFrmHead->m_byFrmType));
		break;
	}

	return bRet;
}

/*=============================================================================
    �� �� ���� SendCustomMsgToOtherMcu
    ��    �ܣ� ����һ��MCU-Inst���������ݵ�������Ϣ��
	           ����Ϣ�峤�ȳ��� MS_MAX_PACKET_LEN ��������а�����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TMSFrmHead *ptSndFrmHead
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendCustomMsgToOtherMcu(TMSFrmHead *ptSndFrmHead)
{	
	BOOL32 bRet = TRUE;
	if (NULL == ptSndFrmHead)
	{
		PrintMSWarning("[SendCustomMsgToOtherMcu] invalid msg, Failed\n");
		//printf("[SendCustomMsgToOtherMcu] invalid msg, Failed\n");
		return bRet;
	}

	u8  byPackNum  = (u8)((ptSndFrmHead->m_dwFrmLen+MS_MAX_PACKET_LEN-1)/MS_MAX_PACKET_LEN);
	u8 *pbyPackBuf = ptSndFrmHead->m_pbyFrmBuf;
	u16 wPackLen   = MS_MAX_PACKET_LEN;
	TMSSynDataReqHead *ptMSDataHead = NULL;
	if (0 == byPackNum)
	{
		byPackNum = 1;
	}
	
	//n-1��
	for (u8 byLoop = 1; byLoop < byPackNum; byLoop++)
	{
		ptMSDataHead = (TMSSynDataReqHead *)(pbyPackBuf-sizeof(TMSSynDataReqHead));
		ptMSDataHead->m_byMsgID   = ptSndFrmHead->m_byFrmType;
		ptMSDataHead->m_dwFrmLen  = htonl(ptSndFrmHead->m_dwFrmLen);
		ptMSDataHead->m_wFrmSN    = htons(ptSndFrmHead->m_wFrmSN);
		ptMSDataHead->m_byPackIdx = byLoop-1;
		ptMSDataHead->m_byMark    = 0;
		if (FALSE == PostMsgToOtherMcuInst(MCU_MCU_SYNNING_DATA_REQ, 
			                               (u8*)ptMSDataHead, wPackLen+sizeof(TMSSynDataReqHead)))
		{
			PrintMSException("[SendCustomMsgToOtherMcu] post err: MsgID.%d TotalLen.%u CurPackLen.%d CurPackIdx.%d Failed\n", 
				              ptMSDataHead->m_byMsgID, ptMSDataHead->m_dwFrmLen, wPackLen, ptMSDataHead->m_byPackIdx);

			//printf("[SendCustomMsgToOtherMcu] post err: MsgID.%d TotalLen.%u CurPackLen.%d CurPackIdx.%d Failed\n", 
			//	              ptMSDataHead->m_byMsgID, ptMSDataHead->m_dwFrmLen, wPackLen, ptMSDataHead->m_byPackIdx);
			return bRet;
		}
		pbyPackBuf += MS_MAX_PACKET_LEN;
	}
	
	//���һ��
	//������֡����Ϊ0
	ptMSDataHead = (TMSSynDataReqHead *)(pbyPackBuf-sizeof(TMSSynDataReqHead));
	wPackLen    = (u16)(ptSndFrmHead->m_dwFrmLen + ptSndFrmHead->m_pbyFrmBuf - pbyPackBuf);
	ptMSDataHead->m_byMsgID   = ptSndFrmHead->m_byFrmType;
	ptMSDataHead->m_dwFrmLen  = htonl(ptSndFrmHead->m_dwFrmLen);
	ptMSDataHead->m_wFrmSN    = htons(ptSndFrmHead->m_wFrmSN);
	ptMSDataHead->m_byPackIdx = byPackNum-1;
	ptMSDataHead->m_byMark    = 1;
	if (FALSE == PostMsgToOtherMcuInst(MCU_MCU_SYNNING_DATA_REQ, 
		                               (u8*)ptMSDataHead, wPackLen+sizeof(TMSSynDataReqHead)))
	{
		PrintMSException("[SendCustomMsgToOtherMcu] post err: MsgID.%d TotalLen.%u CurPackLen.%d CurPackIdx.%d Failed\n", 
			              ptMSDataHead->m_byMsgID, ptMSDataHead->m_dwFrmLen, wPackLen, ptMSDataHead->m_byPackIdx);

		//printf("[SendCustomMsgToOtherMcu] post err: MsgID.%d TotalLen.%u CurPackLen.%d CurPackIdx.%d Failed\n", 
		//	              ptMSDataHead->m_byMsgID, ptMSDataHead->m_dwFrmLen, wPackLen, ptMSDataHead->m_byPackIdx);
		return bRet;
	}
	
	bRet = TRUE;
	return bRet;
}

/*=============================================================================
    �� �� ���� RecvCustomMsgFromOtherMcu
    ��    �ܣ� ������һ��MCU-Inst����Ϣ����Ϣ���������飩
	           ���������򡢶����������һ��������ʧ�ܴ���
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CMessage * const pcMsg
    �� �� ֵ�� TMSFrmHead *
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
TMSFrmHead *CMSManagerSsnInst::RecvCustomMsgFromOtherMcu( CMessage * const pcMsg )
{
	CServMsg cMsg( pcMsg->content ,pcMsg->length );
	TMSFrmHead *ptMSFrmHead = NULL;

	//������֡����Ϊ0
	if (cMsg.GetMsgBodyLen() < sizeof(TMSSynDataReqHead)) 
	{
		PrintMSException("[RecvCustomMsgFromOtherMcu] Invalid msgLen < sizeof(TMSSynDataReqHead).%d\n", sizeof(TMSSynDataReqHead));
		return ptMSFrmHead;
	}

	u16 wPackLen   = cMsg.GetMsgBodyLen() - sizeof(TMSSynDataReqHead);
	u8 *pbyPackBuf = cMsg.GetMsgBody() + sizeof(TMSSynDataReqHead);
	TMSSynDataReqHead *ptMSDataHead = (TMSSynDataReqHead *)cMsg.GetMsgBody();
	ptMSDataHead->m_dwFrmLen        = ntohl(ptMSDataHead->m_dwFrmLen);
	ptMSDataHead->m_wFrmSN          = ntohs(ptMSDataHead->m_wFrmSN);

	if (ptMSDataHead->m_dwFrmLen > MS_MAX_FRAME_LEN || 
		ptMSDataHead->m_byPackIdx >= MS_MAX_PACKET_NUM || 
		ptMSDataHead->m_byMsgID == (u8)emSynDataStart || 
		ptMSDataHead->m_byMsgID >= (u8)emSynDataEnd)
	{
		PrintMSException("[RecvCustomMsgFromOtherMcu] Invalid msgInfo FrmLen.%u PackIdx.%d MsgID.%d\n", 
			              ptMSDataHead->m_dwFrmLen, ptMSDataHead->m_byPackIdx, ptMSDataHead->m_byMsgID);
		return ptMSFrmHead;
	}
	if ((0 == ptMSDataHead->m_byMark && wPackLen != MS_MAX_PACKET_LEN) || 
		(1 == ptMSDataHead->m_byMark && wPackLen >  MS_MAX_PACKET_LEN))
	{
		PrintMSException("[RecvCustomMsgFromOtherMcu] Invalid msgLen FrmLen.%u PackLen.%d PackIdx.%d Mark.%d MsgID.%d\n", 
			              ptMSDataHead->m_dwFrmLen, wPackLen, ptMSDataHead->m_byPackIdx, 
					      ptMSDataHead->m_byMark, ptMSDataHead->m_byMsgID);
		return ptMSFrmHead;
	}

	if (m_tRcvFrmHead.m_byFrmType != ptMSDataHead->m_byMsgID || 
		m_tRcvFrmHead.m_wFrmSN != ptMSDataHead->m_wFrmSN || 
		0 == ptMSDataHead->m_byPackIdx)
	{
		if (emSynDataStart != m_tRcvFrmHead.m_byFrmType)
		{
			PrintMSException("[RecvCustomMsgFromOtherMcu] Lost One Incomplete Msg : FrmLen.%u RcvFrmLen.%u MsgID.%d\n", 
				              m_tRcvFrmHead.m_dwFrmLen, m_dwRcvFrmLen, m_tRcvFrmHead.m_byFrmType);
		}
		m_dwRcvFrmLen = 0;
		m_tRcvFrmHead.m_byFrmType = ptMSDataHead->m_byMsgID;
		m_tRcvFrmHead.m_dwFrmLen  = ptMSDataHead->m_dwFrmLen;
		m_tRcvFrmHead.m_wFrmSN    = ptMSDataHead->m_wFrmSN;
	}
	u32 dwOffset = MS_MAX_PACKET_LEN*ptMSDataHead->m_byPackIdx;
	if ((dwOffset+wPackLen) > m_tRcvFrmHead.m_dwFrmLen || 
		(m_dwRcvFrmLen+wPackLen) > m_tRcvFrmHead.m_dwFrmLen)
	{
		PrintMSException("[RecvCustomMsgFromOtherMcu] Lost One Msg FrmLen.%u RcvFrmLen.%u PackLen.%d PackIdx.%d MsgID.%d\n", 
			              m_tRcvFrmHead.m_dwFrmLen, m_dwRcvFrmLen, 
					      wPackLen, ptMSDataHead->m_byPackIdx, m_tRcvFrmHead.m_byFrmType);
		return ptMSFrmHead;
	}

	memcpy((m_tRcvFrmHead.m_pbyFrmBuf+dwOffset), pbyPackBuf, wPackLen);
	m_dwRcvFrmLen += wPackLen;
	if (m_dwRcvFrmLen == m_tRcvFrmHead.m_dwFrmLen)
	{
		ptMSFrmHead = &m_tRcvFrmHead;
	}
	
	return ptMSFrmHead;
}

/*=============================================================================
    �� �� ���� SendSynData
    ��    �ܣ� ���� ͬ��mcu�������� ֪ͨ��ȫ������ͬ��������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� BOOL32 bStart TRUE-��ʼ���ͣ���ʼ��״̬ FALSE-��������
    �� �� ֵ�� BOOL32 TRUE-����ͬ������ɣ�FALSE-δȫ�����
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynData( BOOL32 bStart /*= FALSE*/ )
{
	BOOL32 bEndFlag = FALSE;

	//ͬ����ʼ�㣺���Զ��������ȫͬ����ͬ��������Ϣ�����鹫����ҵ�����ݣ�
	//                            ����  ֱ��ͬ�����鹫����ҵ������
	//2005-12-27 �Ծɸ�Ϊȫ��ͬ��
	if (bStart)
	{
		memset((void*)&m_tMSCurSynState, 0, sizeof(m_tMSCurSynState));

		/*
		if (g_cMSSsnApp.IsMSConfigInited() && 
			g_cMSSsnApp.IsMSSynOK())
		{
			m_tMSCurSynState.m_emSynType = emVcDeamonPeriEqpData;
		}
		else
		*/
		{
			m_tMSCurSynState.m_emSynType = emSysTime;
		}
	}
	else
	{
        // guzh [4/18/2007] ������ڵ���ԤԼ���飬������������MC��
        if ( emOneVcInstConfMtTableData == m_tMSCurSynState.m_emSynType && 
            CONF_TAKEMODE_ONGOING != g_cMcuVcApp.GetTakeModeOfInstId(m_tMSCurSynState.m_byInstID) ) 
        {
            m_tMSCurSynState.m_emSynType = emOneVcInstOtherData;
        }
        else
        {
            m_tMSCurSynState.m_emSynType = (emgMSSynDataType)(m_tMSCurSynState.m_emSynType+1);
        }
	}

	if (m_tMSCurSynState.m_emSynType <= emAllVcInstState)
	{
		//����������Ϣ�����鹫����ҵ������
		m_tMSCurSynState.m_byInstID  = 0;

		if (FALSE == g_cMSSsnApp.IsMSConfigInited() && 
			emVcDeamonPeriEqpData == m_tMSCurSynState.m_emSynType)
		{
			//ͬ�������㣺������δ��ɳ�ʼ����ҵ��App��δ����������ͬ��
			memset((void*)&m_tMSCurSynState, 0, sizeof(m_tMSCurSynState));
			bEndFlag = TRUE;
		}
	}
	else
	{
		//���ͻ���ʵ����ҵ�����ݣ����ڶ����ͬ���Ļ���ʵ����
		u8 byInstID = m_tMSCurSynState.m_byInstID;
		if (emSynDataEnd == m_tMSCurSynState.m_emSynType || 
			emOneVcInstConfMtTableData == m_tMSCurSynState.m_emSynType)
		{
			m_tMSCurSynState.m_emSynType = emOneVcInstConfMtTableData;
			//Find Next No-Idle-Inst ID
			byInstID = g_cMcuVcApp.FindNextConfInstIDOfNotIdle( byInstID );
		}

		if (0 != byInstID)
		{
			m_tMSCurSynState.m_byInstID = byInstID;
		}
		else
		{
			//ͬ�������㣺����������ɳ�ʼ�������������ͬ���Ļ���ʵ��������ͬ��
			memset((void*)&m_tMSCurSynState, 0, sizeof(m_tMSCurSynState));
			bEndFlag = TRUE;
		}
	}

	if (FALSE == bEndFlag)
	{
		if (SendSynData(m_tMSCurSynState))
		{
			PrintMSInfo("[SendSynData] OK: EndFlag.%d Start.%d SynTypeID.%u InstID.%d!\n", 
				         bEndFlag, bStart, m_tMSCurSynState.m_emSynType, m_tMSCurSynState.m_byInstID);
		}
		else
		{
			PrintMSWarning("[SendSynData] Failed: bStart.%d SynTypeID.%u InstID.%d\n", 
				           bStart, m_tMSCurSynState.m_emSynType, m_tMSCurSynState.m_byInstID);
		}
	}
	
	return bEndFlag;
}

/*=============================================================================
    �� �� ���� SendSynData
    ��    �ܣ� ���� ָ�����͵�ͬ������ ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TMSCurSynState tMSCurSynState
    �� �� ֵ�� BOOL32 TRUE-���ͳɹ�
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynData( TMSCurSynState tMSCurSynState )
{
	BOOL32 bRet = FALSE;

	//check valid
	if (tMSCurSynState.m_emSynType <= emSynDataStart || tMSCurSynState.m_emSynType >= emSynDataEnd)
	{
		PrintMSWarning("[SendSynData] Invalid argument SynTypeID.%u InstID.%d failed!\n", 
			            tMSCurSynState.m_emSynType, tMSCurSynState.m_byInstID);
		return bRet;
	}
	
	switch(tMSCurSynState.m_emSynType)
	{
	//����������Ϣ����
	case emSysTime:
		bRet = SendSynDataOfSysTime();
		break;
	case emSysCritData:
        bRet = SendSynDataOfSysCritData();
        break;
	case emCfgFile:
		bRet = SendSynDataOfCfgFile();
		break;
	case emDebugFile:
		bRet = SendSynDataOfDebugFile();
		break;
    case emAddrbookFile_Utf8:
        bRet = SendSynDataOfAddrbookFileUtf8();
		break;
	case emAddrbookFile:
		bRet = SendSynDataOfAddrbookFile();
		break;
	case emConfinfoFile:
		bRet = SendSynDataOfConfinfoFile();
		break;
	case emUnProcConfinfoFile:
		bRet = SendSynDataOfUnProcConfinfoFile();
		break;
	case emLoguserFile:
		bRet = SendSynDataOfLoguserFile();
		break;
	case emVCSLoguserFile:
		bRet = SendSynDataOfVCSLoguserFile();
		break;
	case emVCSUserTaskFile:
		bRet = SendSynDataOfVCSUserTaskFile();
		break;
    case emUserExFile:
        bRet = SendSynDataOfUserExFile();
        break;

	//���ͻ��鹫����ҵ������
	case emVcDeamonPeriEqpData:
		bRet = SendSynDataOfVcDeamonPeriEqpData();
		break;
	case emVcDeamonMCData:
		bRet = SendSynDataOfVcDeamonMCData();
		break;
	case emVcDeamonMpData:
		bRet = SendSynDataOfVcDeamonMpData();
		break;
	case emVcDeamonMtadpData:
		bRet = SendSynDataOfVcDeamonMtadpData();
		break;
	case emVcDeamonTemplateData:
		bRet = SendSynDataOfVcDeamonTemplateData();
		break;		
	case emVcDeamonOtherData:
		bRet = SendSynDataOfVcDeamonOtherData();
		break;
	case emAllVcInstState:
		bRet = SendSynDataOfAllVcInstState();
		break;

	//����ָ������ʵ����ҵ������
	case emOneVcInstConfMtTableData:
		bRet = SendSynDataOfOneVcInstConfMtTableData(tMSCurSynState.m_byInstID);
		break;
	case emOneVcInstConfSwitchTableData:
		bRet = SendSynDataOfOneVcInstConfSwitchTableData(tMSCurSynState.m_byInstID);
		break;
	case emOneVcInstConfOtherMcuTableData:
		bRet = SendSynDataOfOneVcInstConfOtherMcTableData(tMSCurSynState.m_byInstID);
		break;
	case emOneVcInstOtherData:
		bRet = SendSynDataOfOneVcInstOtherData(tMSCurSynState.m_byInstID);
		break;

	default:
		PrintMSCritical("[SendSynData] Wrong Custom SynTypeID.%u InstID.%d failed!\n", 
			             tMSCurSynState.m_emSynType, tMSCurSynState.m_byInstID);
		break;
	}
	
	if (bRet)
	{
        // guzh [9/25/2006] ͬ�����������̵ȴ���Ӧ��ʱ��
        // ��ֹ��������ʱ���޷���⵽�Զ˶���
		SetTimer(MCU_MCU_WAITFORRSP_TIMER, MS_WAITFOTRSP_TIMEOUT, (u32)MCU_MCU_SYNNING_DATA_REQ);
	}
	
	return bRet;
}

/*=============================================================================
    �� �� ���� SendSynDataOfSysTime
    ��    �ܣ� ���� ͬ��mcuϵͳʱ�� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfSysTime( void )
{
	TKdvTime tCurMcuTime;
	time_t tMcuTime;
	
	::time( &tMcuTime );
	tCurMcuTime.SetTime( &tMcuTime );
	
	m_tSndFrmHead.m_byFrmType = emSysTime;
	m_tSndFrmHead.m_dwFrmLen  = sizeof(tCurMcuTime);
	m_tSndFrmHead.m_wFrmSN   += 1;
	memcpy(m_tSndFrmHead.m_pbyFrmBuf, (u8*)&tCurMcuTime, sizeof(tCurMcuTime) );
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    �� �� ���� SaveSynDataOfSysTime
    ��    �ܣ� ���� ͬ��mcuϵͳʱ�� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 *pbyBuf, 
	           u32 dwBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfSysTime( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	//check valid
	if (NULL == pbyBuf || sizeof(TKdvTime) != dwBufLen)
	{
		PrintMSWarning("[SaveSynDataOfSysTime] invalid argument BufLen.%d\n", dwBufLen);
		return bRet;
	}
	
    TKdvTime *ptTime = (TKdvTime *)pbyBuf;
    time_t tTime;
    ptTime->GetTime(tTime);
	bRet = g_cMSSsnApp.SetLocalMcuTime(tTime);
    if (FALSE == bRet)
    {
        PrintMSWarning("[SaveSynDataOfSysTime] change system time failed\n");
    }
	
	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emSysTime;
		m_tMSCurSynState.m_byInstID  = 0;
	}
		
    return bRet;
}

/*=============================================================================
    �� �� ���� SendSynDataOfSysTime
    ��    �ܣ� ���� mcuϵͳ�ؼ����� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2007/6/11  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfSysCritData( void )
{
    u32 dwSysSSrc = htonl(g_cMSSsnApp.GetMSSsrc());
    
    m_tSndFrmHead.m_byFrmType = emSysCritData;
    m_tSndFrmHead.m_dwFrmLen  = sizeof(dwSysSSrc);
    m_tSndFrmHead.m_wFrmSN   += 1;
    memcpy(m_tSndFrmHead.m_pbyFrmBuf, (u8*)&dwSysSSrc, sizeof(dwSysSSrc) );
    
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    �� �� ���� SaveSynDataOfSysCritData
    ��    �ܣ� ���� ͬ��mcuϵͳ�ؼ�����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 *pbyBuf, 
	           u32 dwBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfSysCritData( u8 *pbyBuf, u32 dwBufLen )
{
    BOOL32 bRet = TRUE;
    //check valid
    if (NULL == pbyBuf || sizeof(u32) != dwBufLen)
    {
        PrintMSWarning("[SaveSynDataOfSysCritData] invalid argument BufLen.%d\n", dwBufLen);
        return bRet;
    }
    
    u32 dwSysSSrc = ntohl(*(u32*)pbyBuf);
    g_cMSSsnApp.SetMSSsrc(dwSysSSrc);
    
    if (bRet)
    {
        m_tMSCurSynState.m_emSynType = emSysCritData;
        m_tMSCurSynState.m_byInstID  = 0;
    }
    
    return bRet;
}

/*=============================================================================
    �� �� ���� SendSynDataOfCfgFile
    ��    �ܣ� ���� ͬ��mcu/conf�µ� mcucfg.ini ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfCfgFile( void )
{
	u32 dwOutBufLen = 0;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;

	if (FALSE == g_cMcuVcApp.GetCfgFileData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                    MS_MAX_FRAME_LEN, dwOutBufLen, bPrintErr))
	{
		PrintMSWarning("[SendSynDataOfCfgFile] GetCfgFileData failed\n");
		return FALSE;
	}
	
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_byFrmType = emCfgFile;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    �� �� ���� SaveSynDataOfCfgFile
    ��    �ܣ� ���� ͬ��mcu/conf�µ� mcucfg.ini ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 *pbyBuf, 
	           u32 dwBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfCfgFile( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;
	
	bRet = g_cMcuVcApp.SetCfgFileData(pbyBuf, dwBufLen, bPrintErr);
	if (FALSE == bRet)
	{
		PrintMSWarning("[SaveSynDataOfCfgFile] SetCfgFileData BufLen.%d failed\n", dwBufLen);
	}
	else
	{
		m_tMSCurSynState.m_emSynType = emCfgFile;
		m_tMSCurSynState.m_byInstID  = 0;
	}
	
    return bRet;
}

/*=============================================================================
    �� �� ���� SendSynDataOfDebugFile
    ��    �ܣ� ���� ͬ��mcu/conf�µ� mcudebug.ini ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfDebugFile( void )
{
	u32 dwOutBufLen = 0;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;

	if (FALSE == g_cMcuVcApp.GetDebugFileData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                      MS_MAX_FRAME_LEN, dwOutBufLen, bPrintErr))
	{
		PrintMSWarning("[SendSynDataOfDebugFile] GetDebugFileData failed\n");
		return FALSE;
	}
	
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_byFrmType = emDebugFile;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    �� �� ���� SaveSynDataOfDebugFile
    ��    �ܣ� ���� ͬ��mcu/conf�µ� mcudebug.ini ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 *pbyBuf, 
	           u32 dwBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfDebugFile( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;
	
	bRet = g_cMcuVcApp.SetDebugFileData(pbyBuf, dwBufLen, bPrintErr);
	if (FALSE == bRet)
	{
		PrintMSWarning("[SaveSynDataOfDebugFile] SetDebugFileData BufLen.%d failed\n", dwBufLen);
	}
	else
	{
		m_tMSCurSynState.m_emSynType = emDebugFile;
		m_tMSCurSynState.m_byInstID  = 0;
	}
	
    return bRet;
}

/*=============================================================================
    �� �� ���� SendSynDataOfAddrbookFileUtf8
    ��    �ܣ� ���� ͬ��mcu/conf�µ� kdvaddrbook.kdv ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2013/07/31  4.7			liaokang                ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfAddrbookFileUtf8( void )
{
    u32 dwOutBufLen = 0;
    BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;
    
    if (FALSE == g_cMcuVcApp.GetAddrbookFileDataUtf8(m_tSndFrmHead.m_pbyFrmBuf, 
        MS_MAX_FRAME_LEN, dwOutBufLen, bPrintErr))
    {
        PrintMSWarning("[SendSynDataOfAddrbookFile] GetAddrbookFileData failed\n");
        return FALSE;
    }
    
    m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
    m_tSndFrmHead.m_byFrmType = emAddrbookFile_Utf8;
    m_tSndFrmHead.m_wFrmSN   += 1;
    
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    �� �� ���� SendSynDataOfAddrbookFile
    ��    �ܣ� ���� ͬ��mcu/conf�µ� addrbook.kdv ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfAddrbookFile( void )
{
	u32 dwOutBufLen = 0;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;
	
	if (FALSE == g_cMcuVcApp.GetAddrbookFileData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                         MS_MAX_FRAME_LEN, dwOutBufLen, bPrintErr))
	{
		PrintMSWarning("[SendSynDataOfAddrbookFile] GetAddrbookFileData failed\n");
		return FALSE;
	}
	
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_byFrmType = emAddrbookFile;
	m_tSndFrmHead.m_wFrmSN   += 1;

	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    �� �� ���� SaveSynDataOfAddrbookFileUtf8
    ��    �ܣ� ���� ͬ��mcu/conf�µ� kdvaddrbook.kdv ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 *pbyBuf, 
               u32 dwBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2013/07/31  4.7			liaokang                ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfAddrbookFileUtf8( u8 *pbyBuf, u32 dwBufLen )
{
    BOOL32 bRet = FALSE;
    BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;
    
    bRet = g_cMcuVcApp.SetAddrbookFileDataUtf8(pbyBuf, dwBufLen, bPrintErr);
    if (FALSE == bRet)
    {
        PrintMSWarning("[SaveSynDataOfAddrbookFile] SetAddrbookFileData BufLen.%d failed\n", dwBufLen);
    }
    else
    {
        m_tMSCurSynState.m_emSynType = emAddrbookFile_Utf8;
        m_tMSCurSynState.m_byInstID  = 0;
    }
    
    return bRet;
}

/*=============================================================================
    �� �� ���� SaveSynDataOfAddrbookFile
    ��    �ܣ� ���� ͬ��mcu/conf�µ� addrbook.kdv ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 *pbyBuf, 
	           u32 dwBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfAddrbookFile( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;
	
	bRet = g_cMcuVcApp.SetAddrbookFileData(pbyBuf, dwBufLen, bPrintErr);
	if (FALSE == bRet)
	{
		PrintMSWarning("[SaveSynDataOfAddrbookFile] SetAddrbookFileData BufLen.%d failed\n", dwBufLen);
	}
	else
	{
		m_tMSCurSynState.m_emSynType = emAddrbookFile;
		m_tMSCurSynState.m_byInstID  = 0;
	}

    return bRet;
}

/*=============================================================================
    �� �� ���� SendSynDataOfConfinfoFile
    ��    �ܣ� ���� ͬ��mcu/data�µ� confinfo.dat ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfConfinfoFile( void )
{
	u32 dwOutBufLen = 0;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;
	
	if (FALSE == g_cMcuVcApp.GetConfinfoFileData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                         MS_MAX_FRAME_LEN, dwOutBufLen, bPrintErr))
	{
		PrintMSWarning("[SendSynDataOfConfinfoFile] GetConfinfoFileData failed\n");
		return FALSE;
	}
	
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_byFrmType = emConfinfoFile;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    �� �� ���� SaveSynDataOfConfinfoFile
    ��    �ܣ� ���� ͬ��mcu/data�µ� confinfo.dat ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 *pbyBuf, 
	           u32 dwBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfConfinfoFile( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;
	   
	bRet = g_cMcuVcApp.SetConfinfoFileData(pbyBuf, dwBufLen, bPrintErr);    
	if (FALSE == bRet)
	{
		PrintMSWarning("[SaveSynDataOfConfinfoFile] SetConfinfoFileData BufLen.%d failed\n", dwBufLen);
	}
	else
	{
		m_tMSCurSynState.m_emSynType = emConfinfoFile;
		m_tMSCurSynState.m_byInstID  = 0;
	}
	
    return bRet;
}
/*=============================================================================
    �� �� ���� SendSynDataOfUnProcConfinfoFile
    ��    �ܣ� ���� ͬ��mcu/data�µ� unprocconfinfo.dat��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/04/03  4.0			���㻪                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfUnProcConfinfoFile( void )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetUnProcConfinfoFileData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                               MS_MAX_FRAME_LEN, dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfUnProcConfinfoFile] GetUnProcConfinfoFileData failed\n");
		return FALSE;
	}

	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_byFrmType = emUnProcConfinfoFile;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}
/*=============================================================================
    �� �� ���� SaveSynDataOfUnProcConfinfoFile
    ��    �ܣ� ���� ͬ��mcu/data�µ� unprocconfinfo.dat��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 *pbyBuf, 
	           u32 dwBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/04/03  4.0			���㻪                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfUnProcConfinfoFile( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;	   
	bRet = g_cMcuVcApp.SetUnProcConfinfoFileData(pbyBuf, dwBufLen);    
	if (FALSE == bRet)
	{
		PrintMSWarning("[SaveSynDataOfConfinfoFile] SetUnProcConfinfoFileData BufLen.%d failed\n", dwBufLen);
	}
	else
	{
		m_tMSCurSynState.m_emSynType = emUnProcConfinfoFile;
		m_tMSCurSynState.m_byInstID  = 0;
	}
	
    return bRet;
}
/*=============================================================================
    �� �� ���� SendSynDataOfVCSLoguserFile
    ��    �ܣ� ���� ͬ��mcu/data�µ� vcslogin.usr��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/04/03  4.0			���㻪                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfVCSLoguserFile( void )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetVCSLoguserFileData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                           MS_MAX_FRAME_LEN, dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfVCSLoguserFile] GetVCSLoguserFileData failed\n");
		return FALSE;
	}

	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_byFrmType = emVCSLoguserFile;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}
/*=============================================================================
    �� �� ���� SaveSynDataOfVCSLoguserFile
    ��    �ܣ� ���� ͬ��mcu/data�µ� vcslogin.usr��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 *pbyBuf, 
	           u32 dwBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/04/03  4.0			���㻪                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfVCSLoguserFile( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;	   
	bRet = g_cMcuVcApp.SetVCSLoguserFileData(pbyBuf, dwBufLen);    
	if (FALSE == bRet)
	{
		PrintMSWarning("[SaveSynDataOfVCSLoguserFile] SetVCSLoguserFileData BufLen.%d failed\n", dwBufLen);
	}
	else
	{
		m_tMSCurSynState.m_emSynType = emVCSLoguserFile;
		m_tMSCurSynState.m_byInstID  = 0;
	}
	
    return bRet;
}
/*=============================================================================
    �� �� ���� SendSynDataOfVCSUserTaskFile
    ��    �ܣ� ���� ͬ��mcu/data�µ� usrtask.dat ��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/04/03  4.0			���㻪                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfVCSUserTaskFile( void )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetVCSUserTaskFileData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                            MS_MAX_FRAME_LEN, dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfVCSUserTaskFile] GetVCSUserTaskFileData failed\n");
		return FALSE;
	}

	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_byFrmType = emVCSUserTaskFile;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}
/*=============================================================================
    �� �� ���� SaveSynDataOfVCSUserTaskFile
    ��    �ܣ� ���� ͬ��mcu/data�µ� usrtask.dat ��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 *pbyBuf, 
	           u32 dwBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2009/04/03  4.0			���㻪                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfVCSUserTaskFile( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;	   
	bRet = g_cMcuVcApp.SetVCSUserTaskFileData(pbyBuf, dwBufLen);    
	if (FALSE == bRet)
	{
		PrintMSWarning("[SaveSynDataOfVCSUserTaskFile] SetVCSUserTaskFileData BufLen.%d failed\n", dwBufLen);
	}
	else
	{
		m_tMSCurSynState.m_emSynType = emVCSUserTaskFile;
		m_tMSCurSynState.m_byInstID  = 0;
	}
	
    return bRet;
}
/*=============================================================================
    �� �� ���� SendSynDataOfLoguserFile
    ��    �ܣ� ���� ͬ��mcu/data�µ� login.usr ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfLoguserFile( void )
{
	u32 dwOutBufLen = 0;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;

	if (FALSE == g_cMcuVcApp.GetLoguserFileData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                        MS_MAX_FRAME_LEN, dwOutBufLen, bPrintErr))
	{
		PrintMSWarning("[SendSynDataOfLoguserFile] GetLoguserFileData failed\n");
		return FALSE;
	}

	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_byFrmType = emLoguserFile;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    �� �� ���� SaveSynDataOfLoguserFile
    ��    �ܣ� ���� ͬ��mcu/data�µ� login.usr ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 *pSetVcDeamonPeriEqpDatabyBuf, 
	           u32 dwBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfLoguserFile( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;

	bRet = g_cMcuVcApp.SetLoguserFileData(pbyBuf, dwBufLen, bPrintErr);
	if (FALSE == bRet)
	{
		PrintMSWarning("[SaveSynDataOfLoguserFile] SetLoguserFileData BufLen.%d failed\n", dwBufLen);
	}
	else
	{
		m_tMSCurSynState.m_emSynType = emLoguserFile;
		m_tMSCurSynState.m_byInstID  = 0;
	}
	
    return bRet;
}


/*=============================================================================
    �� �� ���� SendSynDataOfUserExFile
    ��    �ܣ� ���� ͬ��mcu/data�µ� usrgrp.usr ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/06/22  4.0			����                ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfUserExFile( void )
{
	u32 dwOutBufLen = 0;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;

	if (FALSE == g_cMcuVcApp.GetUserExFileData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                        MS_MAX_FRAME_LEN, dwOutBufLen, bPrintErr))
	{
		PrintMSWarning("[SendSynDataOfLoguserFile] GetLoguserFileData failed\n");
		return FALSE;
	}

	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_byFrmType = emUserExFile;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    �� �� ���� SaveSynDataOfUserExFile
    ��    �ܣ� ���� ͬ��mcu/data�µ� usrgrp.usr ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 *pSetVcDeamonPeriEqpDatabyBuf, 
	           u32 dwBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/06/22  4.0			����                ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfUserExFile( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;

	bRet = g_cMcuVcApp.SetUserExFileData(pbyBuf, dwBufLen, bPrintErr);
	if (FALSE == bRet)
	{
		PrintMSWarning("[SaveSynDataOfLoguserFile] SetLoguserFileData BufLen.%d failed\n", dwBufLen);
	}
	else
	{
		m_tMSCurSynState.m_emSynType = emUserExFile;
		m_tMSCurSynState.m_byInstID  = 0;
	}
	
    return bRet;
}


/*=============================================================================
    �� �� ���� SendSynDataOfVcDeamonPeriEqpData
    ��    �ܣ� ���� ͬ��mcuҵ��Deamon_TPeriEqpData�ڴ����� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfVcDeamonPeriEqpData( void )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetVcDeamonPeriEqpData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                            MS_MAX_FRAME_LEN, dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfVcDeamonPeriEqpData] GetVcDeamonPeriEqpData failed\n");
		return FALSE;
	}
	m_tSndFrmHead.m_byFrmType = emVcDeamonPeriEqpData;
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    �� �� ���� SaveSynDataOfVcDeamonPeriEqpData
    ��    �ܣ� ���� ͬ��mcuҵ��Deamon_TPeriEqpData�ڴ����� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 *pbyBuf, 
	           u32 dwBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfVcDeamonPeriEqpData( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	
	//���Զ����ã����˱��ã�ͬ��ʱ��δ��ɳ�ʼ����ֱ�ӷ��أ�����ͬ��
	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		PrintMSInfo("[SaveSynDataOfVcDeamonPeriEqpData] McuVcApp Not Inited, return\n");
		bRet = TRUE;
	}
	else
	{
		bRet = g_cMcuVcApp.SetVcDeamonPeriEqpData( pbyBuf, dwBufLen );
		if (FALSE == bRet)
		{
			PrintMSWarning("[SaveSynDataOfVcDeamonPeriEqpData] SetVcDeamonPeriEqpData BufLen.%d failed\n", dwBufLen);
		}
	}

	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emVcDeamonPeriEqpData;
		m_tMSCurSynState.m_byInstID  = 0;
	}

    return bRet;
}

/*=============================================================================
    �� �� ���� SendSynDataOfVcDeamonMCData
    ��    �ܣ� ���� ͬ��mcuҵ��Deamon_TMCData�ڴ����� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfVcDeamonMCData( void )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetVcDeamonMCData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                       MS_MAX_FRAME_LEN, dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfVcDeamonMCData] GetVcDeamonMCData failed\n");
		return FALSE;
	}
	m_tSndFrmHead.m_byFrmType = emVcDeamonMCData;
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    �� �� ���� SaveSynDataOfVcDeamonMCData
    ��    �ܣ� ���� ͬ��mcuҵ��Deamon_TMCData�ڴ����� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 *pbyBuf, 
	           u32 dwBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfVcDeamonMCData( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	
	//���Զ����ã����˱��ã�ͬ��ʱ��δ��ɳ�ʼ����ֱ�ӷ��أ�����ͬ��
	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		PrintMSInfo("[SaveSynDataOfVcDeamonMCData] McuVcApp Not Inited, return\n");
		bRet = TRUE;
	}
	else
	{
		bRet = g_cMcuVcApp.SetVcDeamonMCData( pbyBuf, dwBufLen );
		if (FALSE == bRet)
		{
			PrintMSWarning("[SaveSynDataOfVcDeamonMCData] SetVcDeamonMCData BufLen.%d failed\n", dwBufLen);
		}
	}

	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emVcDeamonMCData;
		m_tMSCurSynState.m_byInstID  = 0;
	}

    return bRet;
}

/*=============================================================================
    �� �� ���� SendSynDataOfVcDeamonMpData
    ��    �ܣ� ���� ͬ��mcuҵ��Deamon_TMpData�ڴ����� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfVcDeamonMpData( void )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetVcDeamonMpData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                       MS_MAX_FRAME_LEN, dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfVcDeamonMpData] GetVcDeamonMpData failed\n");
		return FALSE;
	}
	m_tSndFrmHead.m_byFrmType = emVcDeamonMpData;
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    �� �� ���� SaveSynDataOfVcDeamonMpData
    ��    �ܣ� ���� ͬ��mcuҵ��Deamon_TMpData�ڴ����� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 *pbyBuf, 
	           u32 dwBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfVcDeamonMpData( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	
	//���Զ����ã����˱��ã�ͬ��ʱ��δ��ɳ�ʼ����ֱ�ӷ��أ�����ͬ��
	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		PrintMSInfo("[SaveSynDataOfVcDeamonMpData] McuVcApp Not Inited, return\n");
		bRet = TRUE;
	}
	else
	{
		bRet = g_cMcuVcApp.SetVcDeamonMpData( pbyBuf, dwBufLen );
		if (FALSE == bRet)
		{
			PrintMSWarning("[SaveSynDataOfVcDeamonMpData] SetVcDeamonMpData BufLen.%d failed\n", dwBufLen);
		}
	}

	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emVcDeamonMpData;
		m_tMSCurSynState.m_byInstID  = 0;
	}

    return bRet;
}

/*=============================================================================
    �� �� ���� SendSynDataOfVcDeamonMtadpData
    ��    �ܣ� ���� ͬ��mcuҵ��Deamon_TMtadpData�ڴ����� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfVcDeamonMtadpData( void )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetVcDeamonMtadpData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                          MS_MAX_FRAME_LEN, dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfVcDeamonMtadpData] GetVcDeamonMtadpData failed\n");
		return FALSE;
	}
	m_tSndFrmHead.m_byFrmType = emVcDeamonMtadpData;
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    �� �� ���� SaveSynDataOfVcDeamonMtadpData
    ��    �ܣ� ���� ͬ��mcuҵ��Deamon_TMtadpData�ڴ����� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 *pbyBuf, 
	           u32 dwBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfVcDeamonMtadpData( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	
	//���Զ����ã����˱��ã�ͬ��ʱ��δ��ɳ�ʼ����ֱ�ӷ��أ�����ͬ��
	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		PrintMSInfo("[SaveSynDataOfVcDeamonMtadpData] McuVcApp Not Inited, return\n");
		bRet = TRUE;
	}
	else
	{
		bRet = g_cMcuVcApp.SetVcDeamonMtadpData( pbyBuf, dwBufLen );
		if (FALSE == bRet)
		{
			PrintMSWarning("[SaveSynDataOfVcDeamonMtadpData] SetVcDeamonMtadpData BufLen.%d failed\n", dwBufLen);
		}
	}

	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emVcDeamonMtadpData;
		m_tMSCurSynState.m_byInstID  = 0;
	}

    return bRet;
}

/*=============================================================================
    �� �� ���� SendSynDataOfVcDeamonTemplateData
    ��    �ܣ� ���� ͬ��mcuҵ��Deamon_TTemplateData�ڴ����� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfVcDeamonTemplateData( void )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetVcDeamonTemplateData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                             MS_MAX_FRAME_LEN, dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfVcDeamonTemplateData] GetVcDeamonTemplateData failed\n");
		return FALSE;
	}
	m_tSndFrmHead.m_byFrmType = emVcDeamonTemplateData;
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    �� �� ���� SaveSynDataOfVcDeamonTemplateData
    ��    �ܣ� ���� ͬ��mcuҵ��Deamon_TTemplateData�ڴ����� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 *pbyBuf, 
	           u32 dwBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfVcDeamonTemplateData( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	
	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		PrintMSInfo("[SaveSynDataOfVcDeamonTemplateData] McuVcApp Not Inited, return\n");
		bRet = TRUE;
	}
	else
	{
		bRet = g_cMcuVcApp.SetVcDeamonTemplateData( pbyBuf, dwBufLen );
		if (FALSE == bRet)
		{
			PrintMSWarning("[SaveSynDataOfVcDeamonTemplateData] SetVcDeamonTemplateData BufLen.%d failed\n", dwBufLen);
		}
	}
	
	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emVcDeamonTemplateData;
		m_tMSCurSynState.m_byInstID  = 0;
	}
	
    return bRet;
}

/*=============================================================================
    �� �� ���� SendSynDataOfVcDeamonOtherData
    ��    �ܣ� ���� ͬ��mcuҵ��VcDeamon_OtherData�ڴ����� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfVcDeamonOtherData( void )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetVcDeamonOtherData(m_tSndFrmHead.m_pbyFrmBuf, dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfVcDeamonOtherData] GetVcDeamonOtherData failed\n");
		return FALSE;
	}
	m_tSndFrmHead.m_byFrmType = emVcDeamonOtherData;
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    �� �� ���� SaveSynDataOfVcDeamonOtherData
    ��    �ܣ� ���� ͬ��mcuҵ��VcDeamon_OtherData�ڴ����� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 *pbyBuf, 
	           u32 dwBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfVcDeamonOtherData( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	
	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		PrintMSInfo("[SaveSynDataOfVcDeamonOtherData] McuVcApp Not Inited, return\n");
		bRet = TRUE;
	}
	else
	{
		bRet = g_cMcuVcApp.SetVcDeamonOtherData( pbyBuf );
		if (FALSE == bRet)
		{
			PrintMSException("[SaveSynDataOfVcDeamonOtherData] SetVcDeamonOtherData BufLen.%d failed\n", dwBufLen);
		}
	}
	
	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emVcDeamonOtherData;
		m_tMSCurSynState.m_byInstID  = 0;
	}

    return bRet;
}

/*=============================================================================
    �� �� ���� SendSynDataOfAllVcInstState
    ��    �ܣ� ���� ͬ��mcuҵ��All_VcInst_State�ڴ����� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfAllVcInstState( void )
{
	TMSVcInstState tMSVcInstState;
	if (FALSE == g_cMcuVcApp.GetAllVcInstState(tMSVcInstState))
	{
		PrintMSWarning("[SendSynDataOfAllVcInstState] GetAllVcInstState failed\n");
		return FALSE;
	}
	m_tSndFrmHead.m_byFrmType = emAllVcInstState;
	m_tSndFrmHead.m_dwFrmLen  = sizeof(TMSVcInstState);
	m_tSndFrmHead.m_wFrmSN   += 1;
	memcpy(m_tSndFrmHead.m_pbyFrmBuf, (s8*)&tMSVcInstState, sizeof(tMSVcInstState));
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    �� �� ���� SaveSynDataOfAllVcInstState
    ��    �ܣ� ���� ͬ��mcuҵ��All_VcInst_State�ڴ����� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 *pbyBuf, 
	           u32 dwBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfAllVcInstState( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	TMSVcInstState tMSVcInstState;
	
	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		PrintMSInfo("[SaveSynDataOfAllVcInstState] McuVcApp Not Inited, return\n");
		bRet = TRUE;
	}
	else
	{
		//check valid
		if (NULL == pbyBuf || sizeof(TMSVcInstState) != dwBufLen)
		{
			PrintMSWarning("[SaveSynDataOfAllVcInstState] invalid argument BufLen.%d\n", dwBufLen);
			return bRet;
		}
		memcpy((s8*)&tMSVcInstState, pbyBuf, sizeof(tMSVcInstState));

		bRet = g_cMcuVcApp.SetAllVcInstState( tMSVcInstState );
		if (FALSE == bRet)
		{
			PrintMSWarning("[SaveSynDataOfAllVcInstState] SetAllVcInstState BufLen.%d failed\n", dwBufLen);
		}
	}
	
	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emAllVcInstState;
		m_tMSCurSynState.m_byInstID  = 0;
	}

    return bRet;
}

/*=============================================================================
    �� �� ���� SendSynDataOfOneVcInstConfMtTableData
    ��    �ܣ� ���� ͬ��mcuҵ��One_VcInst_TConfMtTableData�ڴ����� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byInstID
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfOneVcInstConfMtTableData( u8 byInstID )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetOneVcInstConfMtTableData(byInstID, 
		                                                 m_tSndFrmHead.m_pbyFrmBuf+sizeof(TMSVcInstDataHead), 
		                                                 MS_MAX_FRAME_LEN-sizeof(TMSVcInstDataHead), dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfOneVcInstConfMtTableData] GetOneVcInstConfMtTableData failed\n");
		return FALSE;
	}
	TMSVcInstDataHead tMSVcInstHead;
	tMSVcInstHead.m_byInstID  = byInstID;
	m_tSndFrmHead.m_byFrmType = emOneVcInstConfMtTableData;
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen+sizeof(tMSVcInstHead);
	m_tSndFrmHead.m_wFrmSN   += 1;
	memcpy(m_tSndFrmHead.m_pbyFrmBuf, (u8*)&tMSVcInstHead, sizeof(tMSVcInstHead));
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    �� �� ���� SaveSynDataOfOneVcInstConfMtTableData
    ��    �ܣ� ���� ͬ��mcuҵ��One_VcInst_TConfMtTableData�ڴ����� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 *pbyBuf, 
	           u32 dwBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfOneVcInstConfMtTableData( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	TMSVcInstDataHead tMSVcInstHead = {0};
	
	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		PrintMSInfo("[SaveSynDataOfOneVcInstConfMtTableData] McuVcApp Not Inited, return\n");
		bRet = TRUE;
	}
	else
	{
		//check valid
		if (NULL == pbyBuf || dwBufLen <= sizeof(TMSVcInstDataHead))
		{
			PrintMSWarning("[SaveSynDataOfOneVcInstConfMtTableData] invalid argument BufLen.%d\n", dwBufLen);
			return bRet;
		}
		memcpy((s8*)&tMSVcInstHead, pbyBuf, sizeof(tMSVcInstHead));

		bRet = g_cMcuVcApp.SetOneVcInstConfMtTableData(tMSVcInstHead.m_byInstID, 
			                                           pbyBuf+sizeof(tMSVcInstHead), 
													   dwBufLen-sizeof(tMSVcInstHead));
		if (FALSE == bRet)
		{
			PrintMSWarning("[SaveSynDataOfOneVcInstConfMtTableData] SetOneVcInstConfMtTableData InstID.%d BufLen.%d failed\n", 
				            tMSVcInstHead.m_byInstID, dwBufLen);
		}
	}

	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emOneVcInstConfMtTableData;
		m_tMSCurSynState.m_byInstID  = tMSVcInstHead.m_byInstID;
	}

    return bRet;
}

/*=============================================================================
    �� �� ���� SendSynDataOfOneVcInstConfSwitchTableData
    ��    �ܣ� ���� ͬ��mcuҵ��One_VcInst_TConfSwitchTable�ڴ����� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byInstID
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfOneVcInstConfSwitchTableData( u8 byInstID )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetOneVcInstConfSwitchTableData(byInstID, 
		                                                     m_tSndFrmHead.m_pbyFrmBuf+sizeof(TMSVcInstDataHead), 
		                                                     MS_MAX_FRAME_LEN-sizeof(TMSVcInstDataHead), dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfOneVcInstConfSwitchTableData] GetOneVcInstConfSwitchTableData failed\n");
		return FALSE;
	}
	TMSVcInstDataHead tMSVcInstHead;
	tMSVcInstHead.m_byInstID  = byInstID;
	m_tSndFrmHead.m_byFrmType = emOneVcInstConfSwitchTableData;
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen+sizeof(tMSVcInstHead);
	m_tSndFrmHead.m_wFrmSN   += 1;
	memcpy(m_tSndFrmHead.m_pbyFrmBuf, (u8*)&tMSVcInstHead, sizeof(tMSVcInstHead));
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    �� �� ���� SaveSynDataOfOneVcInstConfSwitchTableData
    ��    �ܣ� ���� ͬ��mcuҵ��One_VcInst_TConfSwitchTable�ڴ����� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 *pbyBuf, 
	           u32 dwBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfOneVcInstConfSwitchTableData( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	TMSVcInstDataHead tMSVcInstHead;
	
	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		PrintMSInfo("[SaveSynDataOfOneVcInstConfSwitchTableData] McuVcApp Not Inited, return\n");
		bRet = TRUE;
		return bRet;
	}
	else
	{
		//check valid
		if (NULL == pbyBuf || dwBufLen <= sizeof(TMSVcInstDataHead))
		{
			PrintMSWarning("[SaveSynDataOfOneVcInstConfSwitchTableData] invalid argument BufLen.%d\n", dwBufLen);
			return bRet;
		}
		memcpy((s8*)&tMSVcInstHead, pbyBuf, sizeof(tMSVcInstHead));

		bRet = g_cMcuVcApp.SetOneVcInstConfSwitchTableData(tMSVcInstHead.m_byInstID, 
			                                               pbyBuf+sizeof(tMSVcInstHead), 
														   dwBufLen-sizeof(tMSVcInstHead));
		if (FALSE == bRet)
		{
			PrintMSWarning("[SaveSynDataOfOneVcInstConfSwitchTableData] SetOneVcInstConfSwitchTableData InstID.%d BufLen.%d failed\n", 
				            tMSVcInstHead.m_byInstID, dwBufLen);
		}
	}
	
	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emOneVcInstConfSwitchTableData;
		m_tMSCurSynState.m_byInstID  = tMSVcInstHead.m_byInstID;
	}
	
    return bRet;
}

/*=============================================================================
    �� �� ���� SendSynDataOfOneVcInstConfOtherMcTableData
    ��    �ܣ� ���� ͬ��mcuҵ��One_VcInst_TConfOtherMcTable�ڴ����� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byInstID
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfOneVcInstConfOtherMcTableData( u8 byInstID )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetOneVcInstConfOtherMcTableData(byInstID, 
		                                                      m_tSndFrmHead.m_pbyFrmBuf+sizeof(TMSVcInstDataHead), 
		                                                      MS_MAX_FRAME_LEN-sizeof(TMSVcInstDataHead), dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfOneVcInstConfOtherMcTableData] GetOneVcInstConfOtherMcTableData failed\n");
		return FALSE;
	}
	TMSVcInstDataHead tMSVcInstHead;
	tMSVcInstHead.m_byInstID  = byInstID;
	m_tSndFrmHead.m_byFrmType = emOneVcInstConfOtherMcuTableData;
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen+sizeof(tMSVcInstHead);
	m_tSndFrmHead.m_wFrmSN   += 1;
	memcpy(m_tSndFrmHead.m_pbyFrmBuf, (s8*)&tMSVcInstHead, sizeof(tMSVcInstHead));
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    �� �� ���� SaveSynDataOfOneVcInstConfOtherMcTableData
    ��    �ܣ� ���� ͬ��mcuҵ��One_VcInst_TConfOtherMcTable�ڴ����� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 *pbyBuf, 
	           u32 dwBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfOneVcInstConfOtherMcTableData( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	TMSVcInstDataHead tMSVcInstHead;
	
	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		PrintMSInfo("[SaveSynDataOfOneVcInstConfOtherMcTableData] McuVcApp Not Inited, return\n");
		bRet = TRUE;
		return bRet;
	}
	else
	{
		//check valid
		if (NULL == pbyBuf || dwBufLen <= sizeof(TMSVcInstDataHead))
		{
			PrintMSWarning("[SaveSynDataOfOneVcInstConfOtherMcTableData] invalid argument BufLen.%d\n", dwBufLen);
			return bRet;
		}
		memcpy((s8*)&tMSVcInstHead, pbyBuf, sizeof(tMSVcInstHead));

		bRet = g_cMcuVcApp.SetOneVcInstConfOtherMcTableData(tMSVcInstHead.m_byInstID, 
			                                                pbyBuf+sizeof(tMSVcInstHead), 
															dwBufLen-sizeof(tMSVcInstHead));
		if (FALSE == bRet)
		{
			PrintMSWarning("[SaveSynDataOfOneVcInstConfOtherMcTableData] SetOneVcInstConfOtherMcTableData InstID.%d BufLen.%d failed\n", 
				            tMSVcInstHead.m_byInstID, dwBufLen);
		}
	}
	
	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emOneVcInstConfOtherMcuTableData;
		m_tMSCurSynState.m_byInstID  = tMSVcInstHead.m_byInstID;
	}
	
    return bRet;
}

/*=============================================================================
    �� �� ���� SendSynDataOfOneVcInstOtherData
    ��    �ܣ� ���� ͬ��mcuҵ��One_VcInst_OtherData�ڴ����� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byInstID
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfOneVcInstOtherData( u8 byInstID )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetOneVcInstOtherData(byInstID, 
		                                           m_tSndFrmHead.m_pbyFrmBuf+sizeof(TMSVcInstDataHead), 
		                                           MS_MAX_FRAME_LEN-sizeof(TMSVcInstDataHead), dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfOneVcInstOtherData] GetOneVcInstOtherData failed\n");
		return FALSE;
	}
	TMSVcInstDataHead tMSVcInstHead;
	tMSVcInstHead.m_byInstID  = byInstID;
	m_tSndFrmHead.m_byFrmType = emOneVcInstOtherData;
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen+sizeof(tMSVcInstHead);
	m_tSndFrmHead.m_wFrmSN   += 1;
	memcpy(m_tSndFrmHead.m_pbyFrmBuf, (u8*)&tMSVcInstHead, sizeof(tMSVcInstHead));
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    �� �� ���� SaveSynDataOfOneVcInstOtherData
    ��    �ܣ� ���� ͬ��mcuҵ��One_VcInst_OtherData�ڴ����� ֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 *pbyBuf, 
	           u32 dwBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfOneVcInstOtherData( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	TMSVcInstDataHead tMSVcInstHead;
	BOOL32 bResumeTimer = FALSE;

	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		PrintMSInfo("[SaveSynDataOfOneVcInstOtherData] McuVcApp Not Inited, return\n");
		bRet = TRUE;
		return bRet;
	}
	else
	{
		//check valid
		if (NULL == pbyBuf || dwBufLen <= sizeof(TMSVcInstDataHead))
		{
			PrintMSWarning("[SaveSynDataOfOneVcInstOtherData] invalid argument BufLen.%d\n", dwBufLen);
			return bRet;
		}
		
		memcpy((s8*)&tMSVcInstHead, pbyBuf, sizeof(tMSVcInstHead));
		if (FALSE == g_cMSSsnApp.IsMSSynOK())
		{
			bResumeTimer = TRUE;
		}

		bRet = g_cMcuVcApp.SetOneVcInstOtherData(tMSVcInstHead.m_byInstID, 
			                                     pbyBuf+sizeof(tMSVcInstHead), 
												 dwBufLen-sizeof(tMSVcInstHead), bResumeTimer);
		if (FALSE == bRet)
		{
			PrintMSWarning("[SaveSynDataOfOneVcInstOtherData] SetOneVcInstOtherData InstID.%d BufLen.%d ResumeTimer.%d failed\n", 
				            tMSVcInstHead.m_byInstID, dwBufLen, bResumeTimer);
		}
	}

	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emOneVcInstOtherData;
		m_tMSCurSynState.m_byInstID  = tMSVcInstHead.m_byInstID;
	}

    return bRet;
}

/*=============================================================================
    �� �� ���� PrintMSVerbose
    ��    �ܣ� ����������ӡ������ϸ��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� s8 * pszFmt
               ...
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
/*lint -save -e628*/
void CMSManagerSsnInst::PrintMSVerbose(s8*  pszFmt, ...)
{
	s8  achPrintBuf[1024];
	s32 nBufLen = sprintf( achPrintBuf, "[MS_VERBOSE]: " );

	va_list tArgptr;
	va_start(tArgptr, pszFmt);    
	vsnprintf(achPrintBuf + nBufLen, 1024 - nBufLen - 1, pszFmt, tArgptr );
	//vsprintf( achPrintBuf+nBufLen, pszFmt, tArgptr );   
	va_end(tArgptr);

	MSManagerPrint(MS_DEBUG_VERBOSE, achPrintBuf);
	return;
}
/*lint -restore*/

/*=============================================================================
    �� �� ���� PrintMSInfo
    ��    �ܣ� ����������ӡ������ͨ��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� s8 * pszFmt
               ...
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
void CMSManagerSsnInst::PrintMSInfo(s8*  pszFmt, ...)
{
	s8  achPrintBuf[1024];
	s32 nBufLen = sprintf( achPrintBuf, "[MS_INFO]: " );
	
	va_list tArgptr;
	va_start(tArgptr, pszFmt);    
	vsnprintf(achPrintBuf + nBufLen, 1024 - nBufLen - 1, pszFmt, tArgptr );
	//vsprintf( achPrintBuf+nBufLen, pszFmt, tArgptr );   
	va_end(tArgptr);
	
	MSManagerPrint(MS_DEBUG_INFO, achPrintBuf);
	
	return;
}

/*=============================================================================
    �� �� ���� PrintMSWarning
    ��    �ܣ� ����������ӡ�澯��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� s8 * pszFmt
               ...
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
void CMSManagerSsnInst::PrintMSWarning(s8*  pszFmt, ...)
{
	s8  achPrintBuf[1024];
	s32 nBufLen = sprintf( achPrintBuf, "[MS_WARNING]: " );
	
	va_list tArgptr;
	va_start(tArgptr, pszFmt);    
	vsnprintf(achPrintBuf + nBufLen, 1024 - nBufLen - 1, pszFmt, tArgptr );
	//vsprintf( achPrintBuf+nBufLen, pszFmt, tArgptr );   
	va_end(tArgptr);
		
	MSManagerPrint(MS_DEBUG_WARNING, achPrintBuf);
	
	return;
}

/*=============================================================================
    �� �� ���� PrintMSException
    ��    �ܣ� ����������ӡ������Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� s8 * pszFmt
               ...
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
void CMSManagerSsnInst::PrintMSException( s8*  pszFmt, ... )
{
	s8  achPrintBuf[1024];
	s32 nBufLen = sprintf( achPrintBuf, "[MS_ERROR]: " );
	
    va_list tArgptr;
    va_start(tArgptr, pszFmt);    
	vsnprintf(achPrintBuf + nBufLen, 1024 - nBufLen - 1, pszFmt, tArgptr );
    //vsprintf( achPrintBuf+nBufLen, pszFmt, tArgptr );   
    va_end(tArgptr);
	
	MSManagerPrint(MS_DEBUG_ERROR, achPrintBuf);
	
	return;
}

/*=============================================================================
    �� �� ���� PrintMSCritical
    ��    �ܣ� ����������ӡ�쳣��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� s8 * pszFmt
               ...
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
void CMSManagerSsnInst::PrintMSCritical( s8*  pszFmt, ... )
{
	s8  achPrintBuf[1024];
	s32 nBufLen = sprintf( achPrintBuf, "[MS_CRITICAL]: " );

	va_list tArgptr;
	va_start(tArgptr, pszFmt);    
	vsnprintf(achPrintBuf + nBufLen, 1024 - nBufLen - 1, pszFmt, tArgptr );
	//vsprintf( achPrintBuf+nBufLen, pszFmt, tArgptr );   
	va_end(tArgptr);

	MSManagerPrint(MS_DEBUG_ERROR, achPrintBuf);

	return;
}

/*=============================================================================
    �� �� ���� MSManagerPrint
    ��    �ܣ� ����������ӡ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byLevel
               s8 * pszPrintStr
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0			����                  ����
=============================================================================*/
void CMSManagerSsnInst::MSManagerPrint( u8 byLevel, s8 *pszPrintStr )
{
	s8  achPrintBuf[256];
	s32 nLen = 0;
		
	if (CInstance::DAEMON != GetInsID())
	{
		nLen = sprintf( achPrintBuf, "[MS]" );
	}
	else
	{
		nLen = sprintf( achPrintBuf, "[MSDaemon]" );
	}
	strncat( achPrintBuf+nLen, pszPrintStr, sizeof(achPrintBuf)-(u32)nLen );
	
	LogPrint( byLevel, MID_MCU_MSMGR, achPrintBuf);
	
	return;
}


//����
CMSManagerConfig::CMSManagerConfig()
{
	m_dwLocalMcuIP  = 0;
	m_dwRemoteMcuIP = 0;
	m_wMcuListenPort= 0;
	m_byLocalMcuType= 0;
	m_bDoubleLink   = FALSE;
	m_bInited       = FALSE;
    m_bRemoteConnected = FALSE;
	m_emLocalMSType = MCU_MSTYPE_INVALID;
	m_emCurMSState  = MCU_MSSTATE_OFFLINE;
	m_bSynOK        = FALSE;
    m_bMsSwitchOK   = FALSE;
	m_hSemInitLock  = NULL;
	m_hSemSynLock   = NULL;
	m_wLockedAppId  = 0;
    
	//vxworks��Ĭ�ϲ��ö�ʱ����ȡӲ��������״̬��
	//vxworks�²�֧����Ӳ��ƽֱ̨�ӻ�ȡ�����ý��������WIN32��ʽ����
	//WIN32�¹̶�����Osp��ϢЭ��������״̬����������������Ϊ����
#ifdef WIN32
	m_bMSDetermineType = FALSE;
	m_byDebugLevel  = MS_DEBUG_INFO;
#else
	m_bMSDetermineType = TRUE;
	m_byDebugLevel  = MS_DEBUG_CRITICAL;
#endif

	m_dwDitheringTimes = 0;

	// guzh [6/12/2007] ��ʼ��ϵͳ�Ựֵ
    srand(time(NULL));
    SetMSSsrc(rand()+1);
}

//����
CMSManagerConfig::~CMSManagerConfig()
{
	FreeMSConfig();
}

/*=============================================================================
    �� �� ���� InitMSConfig
    ��    �ܣ� ��ȡ�����嵹��ҵ�����ã���������Ҫ����MSManagerӦ�ã�����Э�̺󷵻�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TMSStartParam tMSStartParam
    �� �� ֵ�� BOOL32 ���ص�ǰMCU������״̬ TRUE-���ã�FALSE-����
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
BOOL32 CMSManagerConfig::InitMSConfig( TMSStartParam tMSStartParam )
{
	BOOL32 bMpcEnable = TRUE;
	
	FreeMSConfig();

#if defined(_MINIMCU_) || defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	SetCurMSState(MCU_MSSTATE_ACTIVE);
	SetMSSynOKFlag(TRUE);
	bMpcEnable = TRUE;
	m_bDoubleLink = FALSE;
	return bMpcEnable;
#endif

	if (FALSE == InitLocalCfgInfo(tMSStartParam.m_byMcuType))
	{		
        //printf("[InitMSConfig] InitLocalCfgInfo Failed!\n");
		LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitMSConfig] InitLocalCfgInfo Failed!\n");
		FreeMSConfig();
		//RebootMCU();
		return bMpcEnable;
	}
	
	m_wMcuListenPort = tMSStartParam.m_wMcuListenPort;
	m_bMSDetermineType = tMSStartParam.m_bMSDetermineType;

	//����ҵ�� - 
	if (m_bDoubleLink)
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitMSConfig] m_bDoubleLink is true!\n");
		//˫�㽨��֧�֣���ʼ��MSManagerӦ�ã��ȴ�����������Э�̽��

		//������Դʧ��������
		if (FALSE == OspSemBCreate(&m_hSemInitLock))
		{
			//printf("[InitMSConfig] OspSemBCreate m_hSemInitLock Failed, DoubleLink->SingleLink!\n");
			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitMSConfig] OspSemBCreate m_hSemInitLock Failed, DoubleLink->SingleLink!\n");
			FreeMSConfig();
			RebootMCU();
			return bMpcEnable;
		}
		if (FALSE == OspSemBCreate(&m_hSemSynLock))
		{
			//printf("[InitMSConfig] OspSemBCreate m_hSemSynLock Failed, DoubleLink->SingleLink!\n");
			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitMSConfig] OspSemBCreate m_hSemSynLock Failed, DoubleLink->SingleLink!\n");
			FreeMSConfig();
			RebootMCU();
			return bMpcEnable;
		}

		//��ʼ���źţ��ͷ�
		OspSemTake(m_hSemInitLock);

		//����ȼ��� McuVc��Task ����һ�£�
		s32 nRet = g_cMSSsnApp.CreateApp( "McuMSMag", AID_MCU_MSMANAGERSSN, APPPRI_MCUMSMGR, 1000, 4<<20 );
		if ( OSP_OK != nRet )
		{
			StaticLog("[InitMSConfig]g_cMSSsnApp.CreateApp McuMSMag failed!\n ");
		}
		OspPost(MAKEIID(AID_MCU_MSMANAGERSSN, 1), MCU_MSEXCHANGER_POWERON_CMD);

		//��Ϊ���壬ͬ���ȴ�����Э�̲���������Э�̽��������һ������
        if(!g_cMSSsnApp.IsActiveBoard())
        {
            if (!EnterMSInitLock())
            {
                //�ȴ���ʱ��
                //��ΪWIN32��ʽ����������Ϊ���ð壬����VxWorks����Ӳ������������״̬
                if (FALSE == GetMSDetermineType())
                {
                    SetCurMSState(MCU_MSSTATE_ACTIVE);
                    SetMSSynOKFlag(TRUE);
                    //printf("[InitMSConfig] EnterMSInitLock TimeOut CurMSState.%d Unlock\n", m_emCurMSState);
					LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitMSConfig] EnterMSInitLock TimeOut CurMSState.%d Unlock\n", m_emCurMSState);
                }
                else
                {
                    if (MCU_MSSTATE_ACTIVE == m_emCurMSState)
                    {
                        //printf("[InitMSConfig] EnterMSInitLock TimeOut CurMSState.%d Unlock\n", m_emCurMSState);
						LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitMSConfig] EnterMSInitLock TimeOut CurMSState.%d Unlock\n", m_emCurMSState);
                    }
                    else
                    {
                        //printf("[InitMSConfig] EnterMSInitLock TimeOut CurMSState.%d Reboot MCU\n", m_emCurMSState);
						LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitMSConfig] EnterMSInitLock TimeOut CurMSState.%d Reboot MCU\n", m_emCurMSState);
						RebootMCU();
                        return bMpcEnable;
                    }
                }
            }
        }		
		else
		{
			SetCurMSState(MCU_MSSTATE_ACTIVE);
		}
		bMpcEnable = (MCU_MSSTATE_ACTIVE == m_emCurMSState) ? TRUE : FALSE;
	}
	else
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitMSConfig] m_bDoubleLink is false!\n");
		//���㽨����ֱ����Ϊ���ð�
		SetCurMSState(MCU_MSSTATE_ACTIVE);
		SetMSSynOKFlag(TRUE);
		bMpcEnable = TRUE;
	}

	//printf("[Mcu] MS Init OK. MpcEnable.%d DoubleLink.%d CurMSState.%d MSDetermineType.%d\n", 
	//	    bMpcEnable, m_bDoubleLink, m_emCurMSState, m_bMSDetermineType);

	LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[Mcu] MS Init OK. MpcEnable.%d DoubleLink.%d CurMSState.%d MSDetermineType.%d\n", 
		    bMpcEnable, m_bDoubleLink, m_emCurMSState, m_bMSDetermineType);

    //��Ϊ������ous�Ƶ���
    if (GetCurMSState() == MCU_MSSTATE_ACTIVE)
    {
        SetMpcOusLed(FALSE);
    }
    
	m_bInited = TRUE;
	return bMpcEnable;
}

/*=============================================================================
    �� �� ���� FreeMSConfig
    ��    �ܣ� �ͷ�����ͨѶ�ڵ� �� ���������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� void
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
void CMSManagerConfig::FreeMSConfig( void )
{
	m_dwLocalMcuIP  = 0;
	m_dwRemoteMcuIP = 0;
	m_wMcuListenPort= 0;
	m_byLocalMcuType= 0;
	m_bDoubleLink   = FALSE;
	m_bInited       = FALSE;
	m_emLocalMSType = MCU_MSTYPE_INVALID;
	m_emCurMSState  = MCU_MSSTATE_OFFLINE;
	m_bSynOK        = FALSE;
	if (NULL != m_hSemInitLock)
	{
		OspSemDelete(m_hSemInitLock);
		m_hSemInitLock = NULL;
	}
	if (NULL != m_hSemSynLock)
	{
		OspSemDelete(m_hSemSynLock);
		m_hSemSynLock = NULL;
	}
	m_wLockedAppId = 0;

	return;
}

/*=============================================================================
    �� �� ���� InitLocalCfgInfo
    ��    �ܣ� ��ʼ�� ����������Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byLocalMcuType
    �� �� ֵ�� BOOL32
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/18  4.0		    ����                  ����
=============================================================================*/
BOOL32 CMSManagerConfig::InitLocalCfgInfo( u8 byLocalMcuType )
{
	/*lint -save -esym(550, byInterface, byEntLayer)*/
	/*lint -save -esym(429, ppszTable)*/
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_CONFIG, MCUCFGFILENAME);
	
	s8* *ppszTable = NULL;
	s32  nEntryNum = 0;
	s32  nLoop = 0;
	
	// ���ж������ļ��Ƿ����, �粻������ֱ�Ӱ���������, zgc, 2007-05-23
	FILE* hOpen = NULL;
    if(NULL != (hOpen = fopen(achFullName, "r"))) // exist
    {
        fclose(hOpen); 
    }
	else
	{
		m_bDoubleLink = FALSE;
		m_dwRemoteMcuIP = 0;
		return TRUE;
	}
	
	//get the number of entry 
	if (FALSE == GetRegKeyInt(achFullName, SECTION_mcueqpBoardConfig, STR_ENTRY_NUM, 0, &nEntryNum)) 
	{
		//printf("[InitLocalCfgInfo] Wrong profile while reading %s %s, SingleLink Start!\n", 
		//	SECTION_mcueqpBoardConfig, STR_ENTRY_NUM );  

		LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Wrong profile while reading %s %s, SingleLink Start!\n", 
			SECTION_mcueqpBoardConfig, STR_ENTRY_NUM );  

		//��ȡ���������ֶ�ʧ�ܣ�����Ĭ�ϵĵ����������ɴ����Զ�����ȱʡ�������ļ�
		m_bDoubleLink   = FALSE;
		m_dwRemoteMcuIP = 0;
		return TRUE;
	}
	if (0 == nEntryNum)
	{
		//printf("[InitLocalCfgInfo] The Board Num is 0, SingleLink Start\n");

		LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] The Board Num is 0, SingleLink Start\n");
		//��ȡ���������ֶ�ʧ�ܣ�����Ĭ�ϵĵ����������ɴ����Զ�����ȱʡ�������ļ�
		m_bDoubleLink   = FALSE;
		m_dwRemoteMcuIP = 0;
		return TRUE;
	}
	
	//alloc memory
	ppszTable = new s8*[(u32)nEntryNum];
	if (NULL == ppszTable)
	{
		//printf("[InitLocalCfgInfo] Fail to malloc memory for board table\n");

		LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Fail to malloc memory for board table\n");
		return FALSE;
	}
	memset(ppszTable, 0, (u32)nEntryNum*(u32)sizeof(s8*));
	for (nLoop = 0; nLoop < nEntryNum; nLoop++)
	{
		ppszTable[nLoop] = new s8[MS_MAX_STRING_LEN+1];
		if (NULL == ppszTable[nLoop])
		{
			
			TableMemoryFree(ppszTable, nEntryNum);
			//printf("[InitLocalCfgInfo] Fail to malloc memory for board table\n");
			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Fail to malloc memory for board table\n");
			return FALSE;
		}
	}
	
	//get the board config table
	u32 dwEntryNum = (u32)nEntryNum;
	// ��ΪGetRegKeyStringTableֻ���ڶ���STR_ENTRY_NUMʱ�Ż᷵��FALSE�������������
	// ������TRUE����������޸��жϱ�׼, zgc, 2007-03-21
	BOOL32 bRet = GetRegKeyStringTable(achFullName, SECTION_mcueqpBoardConfig, "fail", 
		ppszTable, &dwEntryNum, (MS_MAX_STRING_LEN+1));
	bRet &= ( (u32)nEntryNum == dwEntryNum ) ? TRUE : FALSE;
	if (FALSE == bRet )
	{
		TableMemoryFree(ppszTable, nEntryNum);

		//printf("[InitLocalCfgInfo] Wrong profile while reading %s table!\n", 
		//	SECTION_mcueqpBoardConfig);

		LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Wrong profile while reading %s table!\n", 
			SECTION_mcueqpBoardConfig);

		return FALSE;
	}
	
	s8  achSeps[] = " \t";  //�ָ���
	s8 *pchToken  = NULL;
	u8  byEntLayer = 0;
	u8  byEntSlot  = 0;
	u8  byEntType  = 0;
	u32 dwEntIpAddr = 0;    //net order
	u8  byInterface = 1;    // ǰ������ѡ��
	
#ifndef WIN32
	TBrdPosition tCurrentPos;
	BrdQueryPosition( &tCurrentPos ); // ֻ���Ĳۺ�/ *MpcΪ����ʱ,ȡ���ʧ�ܣ����� */
	//printf( "[InitLocalCfgInfo] BrdQueryPosition[BrdId.%d, Layer.%d, Slot.%d]!\n", 
	//											tCurrentPos.byBrdID,
	//											tCurrentPos.byBrdLayer,
	//											tCurrentPos.byBrdSlot);

	LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] BrdQueryPosition[BrdId.%d, Layer.%d, Slot.%d]!\n", 
												tCurrentPos.byBrdID,
												tCurrentPos.byBrdLayer,
												tCurrentPos.byBrdSlot);
#endif	
	
	// analyze entry strings 
	for (nLoop = 0; nLoop < nEntryNum; nLoop++ )
	{
		// BoardId 
		pchToken = strtok( ppszTable[nLoop], achSeps );
		if (NULL == pchToken)
		{
			//printf("[InitLocalCfgInfo] Wrong profile while reading %s!\n", FIELD_mcueqpBoardId );
			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Wrong profile while reading %s!\n", FIELD_mcueqpBoardId );
			continue;
		}
		
		//Layer
		pchToken = strtok( NULL, achSeps );
		if (NULL == pchToken)
		{
			//printf("[InitLocalCfgInfo] Wrong profile while reading %s!\n", FIELD_mcueqpBoardLayer );

			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Wrong profile while reading %s!\n", FIELD_mcueqpBoardLayer );
			continue;
		}
		else
		{
			byEntLayer = atoi(pchToken);
		}
		
		//Slot 
		pchToken = strtok( NULL, achSeps );
		if (NULL == pchToken)
		{
			//printf("[InitLocalCfgInfo] Wrong profile while reading %s!\n", FIELD_mcueqpBoardSlot );
			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Wrong profile while reading %s!\n", FIELD_mcueqpBoardSlot );
			continue;
		}
		else
		{
			StrUpper( pchToken );
			if (FALSE == GetSlotThroughName(pchToken, &byEntSlot))
			{
				//printf("[InitLocalCfgInfo] Wrong Value %s = %s !\n", FIELD_mcueqpBoardSlot, pchToken );
				LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Wrong Value %s = %s !\n", FIELD_mcueqpBoardSlot, pchToken );
				continue;
			}
		}
		
		//Type 
		pchToken = strtok( NULL, achSeps );
		if (NULL == pchToken)
		{
			//printf("[InitLocalCfgInfo] Wrong profile while reading %s!\n", FIELD_mcueqpBoardType );
			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Wrong profile while reading %s!\n", FIELD_mcueqpBoardType );
			continue;
		}
		else
		{
			StrUpper( pchToken );
			if (FALSE == GetTypeThroughName(pchToken, &byEntType))
			{
				//printf("[InitLocalCfgInfo] Wrong Value %s = %s !\n", FIELD_mcueqpBoardType, pchToken );
				LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Wrong Value %s = %s !\n", FIELD_mcueqpBoardType, pchToken );
				continue;
			}
		}
		
		//IpAddr
		pchToken = strtok( NULL, achSeps );
		if (NULL == pchToken)
		{
			//printf("[InitLocalCfgInfo] Wrong profile while reading %s!\n", FIELD_mcueqpBoardIpAddr );
			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Wrong profile while reading %s!\n", FIELD_mcueqpBoardIpAddr );
			continue;
		}
		else
		{
			dwEntIpAddr = INET_ADDR(pchToken);
		}
		
		pchToken = strtok(NULL, achSeps);
		if( BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ == byEntType || BRD_TYPE_MPC2 == byEntType )
		{
#ifdef _MINIMCU_               
			byInterface = 0;
#else
#ifdef _LINUX_
			// �Ȳ�eth1
			STATUS nRet = 0;
			TBrdEthParamAll tBrdEthParamAll;
			u8 byLop = 0;
			nRet = BrdGetEthParamAll( 1, &tBrdEthParamAll ); 
			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Get eth1 params -> IpNum.%d!\n", tBrdEthParamAll.dwIpNum);
			if (tBrdEthParamAll.dwIpNum > 0)
			{
				for (u8 byIdx = 0; byIdx < tBrdEthParamAll.dwIpNum; byIdx++)
				{
					LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] eth1 Ip.%d -> 0x%x!\n", 
									byIdx, tBrdEthParamAll.atBrdEthParam[byIdx].dwIpAdrs);
				}
			}

			if ( nRet == ERROR )
			{
				//printf( "[InitLocalCfgInfo] Get eth1 params failed!\n" );
				LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Get eth1 params failed!\n" );
			}
			else if( tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs != 0 )
			{
				for ( byLop = 0; byLop < tBrdEthParamAll.dwIpNum; byLop++ )
				{
					if ( dwEntIpAddr == tBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs )
					{
						LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Get eth1 params success!\n" );
						byInterface = 1;
						break;
					}
				}
			}
			
			//�ٲ�eth0
			if ( nRet == ERROR 
				|| tBrdEthParamAll.dwIpNum == 0 
				|| (tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs == 0)
				|| byLop == tBrdEthParamAll.dwIpNum )
			{
				memset( &tBrdEthParamAll, 0, sizeof(tBrdEthParamAll) );
				nRet = BrdGetEthParamAll( 0, &tBrdEthParamAll ); 

				LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Get eth0 params -> IpNum.%d!\n", tBrdEthParamAll.dwIpNum);
				
				if (tBrdEthParamAll.dwIpNum > 0)
				{
					for (u8 byIdx = 0; byIdx < tBrdEthParamAll.dwIpNum; byIdx++)
					{
						LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] eth0 Ip.%d -> 0x%x!\n", 
													byIdx, tBrdEthParamAll.atBrdEthParam[byIdx].dwIpAdrs);
					}
				}

				if ( nRet == ERROR )
				{
					//printf( "[InitLocalCfgInfo] Get eth0 params failed!\n" );
					LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Get eth0 params failed!\n" );
				}
				else if ( tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs != 0 )
				{
					for ( byLop = 0; byLop < tBrdEthParamAll.dwIpNum; byLop++ )
					{
						if ( dwEntIpAddr == tBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs )
						{
							byInterface = 0;
							break;
						}
					}
				}
				if ( nRet == ERROR 
					|| tBrdEthParamAll.dwIpNum == 0 
					|| (tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs == 0)
					|| byLop == tBrdEthParamAll.dwIpNum )
				{
					//printf( "[InitLocalCfgInfo] MPC ip is not correct!\n" );
					LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] MPC ip is not correct!\n" );
					byInterface = 1;
				}
			}
			
#endif
#ifdef _VXWORKS_
			// �Ȳ�eth1
			s32 nRet;
			TBrdEthParam tOneParam;
			memset(&tOneParam, 0, sizeof(tOneParam));
			nRet = BrdGetEthParam( 1, &tOneParam );
			if ( nRet == ERROR )
			{
				printf( "[InitLocalCfgInfo] Get eth1 param failed!\n" );
			}
			else if ( dwEntIpAddr == tOneParam.dwIpAdrs )
			{
				byInterface = 1;
			}
			
			if ( nRet == ERROR || dwEntIpAddr != tOneParam.dwIpAdrs ) 
			{
				//�ٲ�eth0
				memset(&tOneParam, 0, sizeof(tOneParam));
				nRet = BrdGetEthParam( 0, &tOneParam );
				if ( nRet == ERROR )
				{
					printf( "[InitLocalCfgInfo] Get eth0 param failed!\n" );
				}
				else if ( dwEntIpAddr == tOneParam.dwIpAdrs )
				{
					byInterface = 0;
				}
				
				if ( nRet == ERROR || dwEntIpAddr != tOneParam.dwIpAdrs ) 
				{
					printf( "[InitLocalCfgInfo] MPC ip is not correct!\n" );
					byInterface = 1;
				}
			}
#endif
#ifdef WIN32
			byInterface = 1;
#endif
#endif		
		}
		else
		{
			if (NULL == pchToken)
			{
				//            if( BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ == byEntType )
				//            {
				// #ifdef _MINIMCU_               
				// 				byInterface = 0;
				// #else
				// 			#ifdef _LINUX_
				// 				// �Ȳ�eth1
				// 				STATUS nRet = 0;
				// 				TBrdEthParamAll tBrdEthParamAll;
				// 				u8 byLop = 0;
				// 				nRet = BrdGetEthParamAll( 1, &tBrdEthParamAll ); 
				// 				if ( nRet == ERROR )
				// 				{
				// 					printf( "[InitLocalCfgInfo] Get eth1 params failed!\n" );
				// 				}
				// 				else if( tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs != 0 )
				// 				{
				// 					for ( byLop = 0; byLop < tBrdEthParamAll.dwIpNum; byLop++ )
				// 					{
				// 						if ( dwEntIpAddr == tBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs )
				// 						{
				// 							byInterface = 1;
				// 							break;
				// 						}
				// 					}
				// 				}
				// 				
				// 				//�ٲ�eth0
				// 				if ( nRet == ERROR 
				// 					|| tBrdEthParamAll.dwIpNum <= 0 
				// 					|| (tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs == 0)
				// 					|| byLop == tBrdEthParamAll.dwIpNum )
				// 				{
				// 					memset( &tBrdEthParamAll, 0, sizeof(tBrdEthParamAll) );
				// 					nRet = BrdGetEthParamAll( 0, &tBrdEthParamAll ); 
				// 					if ( nRet == ERROR )
				// 					{
				// 						printf( "[InitLocalCfgInfo] Get eth0 params failed!\n" );
				// 					}
				// 					else if ( tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs != 0 )
				// 					{
				// 						for ( byLop = 0; byLop < tBrdEthParamAll.dwIpNum; byLop++ )
				// 						{
				// 							if ( dwEntIpAddr == tBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs )
				// 							{
				// 								byInterface = 0;
				// 								break;
				// 							}
				// 						}
				// 					}
				// 					if ( nRet == ERROR 
				// 					|| tBrdEthParamAll.dwIpNum <= 0 
				// 					|| (tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs == 0)
				// 					|| byLop == tBrdEthParamAll.dwIpNum )
				// 					{
				// 						printf( "[InitLocalCfgInfo] MPC ip is not correct!\n" );
				// 						byInterface = 1;
				// 					}
				// 				}
				// 				
				// 			#endif
				// 			#ifdef _VXWORKS_
				// 				// �Ȳ�eth1
				// 				s32 nRet;
				// 				TBrdEthParam tOneParam;
				// 				memset(&tOneParam, 0, sizeof(tOneParam));
				// 				nRet = BrdGetEthParam( 1, &tOneParam );
				// 				if ( nRet == ERROR )
				// 				{
				// 					printf( "[InitLocalCfgInfo] Get eth1 param failed!\n" );
				// 				}
				// 				else if ( dwEntIpAddr == tOneParam.dwIpAdrs )
				// 				{
				// 					byInterface = 1;
				// 				}
				// 				
				// 				if ( nRet == ERROR || dwEntIpAddr != tOneParam.dwIpAdrs ) 
				// 				{
				// 					//�ٲ�eth0
				// 					memset(&tOneParam, 0, sizeof(tOneParam));
				// 					nRet = BrdGetEthParam( 0, &tOneParam );
				// 					if ( nRet == ERROR )
				// 					{
				// 						printf( "[InitLocalCfgInfo] Get eth0 param failed!\n" );
				// 					}
				// 					else if ( dwEntIpAddr == tOneParam.dwIpAdrs )
				// 					{
				// 						byInterface = 0;
				// 					}
				// 
				// 					if ( nRet == ERROR || dwEntIpAddr != tOneParam.dwIpAdrs ) 
				// 					{
				// 						printf( "[InitLocalCfgInfo] MPC ip is not correct!\n" );
				// 						byInterface = 1;
				// 					}
				// 				}
				// 			#endif
				// 			#ifdef WIN32
				// 				byInterface = 1;
				// 			#endif
// #endif		
//            }
// 		   else
			   {
				   byInterface = 1;
			   }
			}
			else
			{
				byInterface = atoi( pchToken );
			}
		}
		
		//�ж����ú�ʵ���Ƿ�һ��, ����ֻ����һ��MPC��
		//  [1/21/2011 chendaiwei]֧��MPC2
		if (BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ == byEntType || BRD_TYPE_MPC2 == byEntType)
		{
#ifndef WIN32			
			TBrdEthParam tMpcEthParam;
			BrdGetEthParam( byInterface, &tMpcEthParam);
			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] BrdGetEthParam <Interface.%d, Ip.0x%x)\n", 
										byInterface, tMpcEthParam.dwIpAdrs);

			if ( ntohl(dwEntIpAddr) == tMpcEthParam.dwIpAdrs )
#else
				if( OspIsLocalHost( dwEntIpAddr) )
#endif
				{
					m_dwLocalMcuIP = dwEntIpAddr;
					if (1 == byEntSlot) //mc1
					{
						m_emLocalMSType = MCU_MSTYPE_MASTER;
					}
					else                //mc0
					{
						m_emLocalMSType = MCU_MSTYPE_SLAVE;
					}
					LogPrint(LOG_LVL_KEYSTATUS,  MID_PUB_ALWAYS, "[InitLocalCfgInfo] m_emLocalMSType.%d!\n", m_emLocalMSType);
				}
#ifndef WIN32
				else if( tCurrentPos.byBrdSlot == byEntSlot ) // �����ļ���MpcIp����
				{
					m_dwLocalMcuIP = htonl( tMpcEthParam.dwIpAdrs );
					LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[!!Debug!!] the mpc's ip %0x.\n", m_dwLocalMcuIP);
				}
#endif
				else
				{
					m_dwRemoteMcuIP = dwEntIpAddr;
				}
		}
	}
	
	//drawback memory
	TableMemoryFree(ppszTable, nEntryNum);
    
	m_byLocalMcuType = byLocalMcuType;
	
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
	if (0 == m_dwLocalMcuIP)
	{
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Wrong profile while LocalMcuIP!\n");
		m_bDoubleLink   = FALSE;
		m_dwRemoteMcuIP = 0;
		return FALSE;
	}
	
	if (MCU_TYPE_KDV8000B != m_byLocalMcuType && MCU_TYPE_KDV8000C != m_byLocalMcuType && 
		0 != m_dwRemoteMcuIP && 0xFFFFFFFF != m_dwRemoteMcuIP && m_dwLocalMcuIP != m_dwRemoteMcuIP)
	{
		m_bDoubleLink = TRUE;
	}
	else
	{
		m_bDoubleLink   = FALSE;
		m_dwRemoteMcuIP = 0;
	}
#endif	
	return TRUE;
	
	/*lint -restore*/
}

/*=============================================================================
    �� �� ���� GetLocalMcuIP
    ��    �ܣ� ��ȡ��������ʱ����mcu��IP
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� u32 ������
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/24  4.0			����                  ����
=============================================================================*/
u32 CMSManagerConfig::GetLocalMcuIP( void )
{
	return ntohl(m_dwLocalMcuIP);
}

/*=============================================================================
    �� �� ���� SetLocalMcuIP
    ��    �ܣ� ������������ʱ����mcu��IP
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� dwLocalMcuIP ������ 
    �� �� ֵ�� void
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/24  4.0			����                  ����
=============================================================================*/
void CMSManagerConfig::SetLocalMcuIP( u32 dwLocalMcuIP )
{
	m_dwLocalMcuIP = htonl(dwLocalMcuIP);

	return;
}

/*=============================================================================
    �� �� ���� GetAnotherMcuIP
    ��    �ܣ� ��ȡ��������ʱ��һ��mcu��IP
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� u32 ������
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/24  4.0			����                  ����
=============================================================================*/
u32 CMSManagerConfig::GetAnotherMcuIP( void )
{
	return ntohl(m_dwRemoteMcuIP);
}

/*=============================================================================
    �� �� ���� SetAnotherMcuIP
    ��    �ܣ� ������������ʱ��һ��mcu��IP
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u32 dwAnotherMcuIP ������
    �� �� ֵ�� void
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/24  4.0			����                  ����
=============================================================================*/
void CMSManagerConfig::SetAnotherMcuIP( u32 dwAnotherMcuIP )
{
	m_dwRemoteMcuIP = htonl(dwAnotherMcuIP);
	
	return;
}

/*=============================================================================
    �� �� ���� GetLocalMcuType
    ��    �ܣ� ��ȡMCU���Ӷ˿�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� u16 ������
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/24  4.0			����                  ����
=============================================================================*/
u16 CMSManagerConfig::GetMcuConnectPort( void )
{
	return m_wMcuListenPort;
}

/*=============================================================================
    �� �� ���� SetMcuConnectPort
    ��    �ܣ� ����MCU���Ӷ˿�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u16 wConnectPort ������
    �� �� ֵ�� void
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/24  4.0			����                  ����
=============================================================================*/
void CMSManagerConfig::SetMcuConnectPort( u16 wConnectPort )
{
	m_wMcuListenPort = wConnectPort;
	
	return;
}

/*=============================================================================
    �� �� ���� GetLocalMcuType
    ��    �ܣ� ��ȡ����MCU���� -- 8000A��8000B��WIN32
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� u8
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/24  4.0			����                  ����
=============================================================================*/
u8 CMSManagerConfig::GetLocalMcuType( void )
{
	return m_byLocalMcuType;
}

/*=============================================================================
    �� �� ���� SetLocalMcuType
    ��    �ܣ� ���ñ���MCU���� -- 8000A��8000B��WIN32
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byLocalMcuType
    �� �� ֵ�� void
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/24  4.0			����                  ����
=============================================================================*/
void CMSManagerConfig::SetLocalMcuType( u8 byLocalMcuType )
{
	m_byLocalMcuType = byLocalMcuType;

	return;
}

/*=============================================================================
    �� �� ���� IsDoubleLink
    ��    �ܣ� ��ȡ�Ƿ����˫�����������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� BOOL32
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/24  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerConfig::IsDoubleLink( void )
{
	return m_bDoubleLink;
}

/*=============================================================================
    �� �� ���� SetDoubleLink
    ��    �ܣ� �����Ƿ����˫�����������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� BOOL32 bDoubleLink
    �� �� ֵ�� void
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/24  4.0			����                  ����
=============================================================================*/
void CMSManagerConfig::SetDoubleLink( BOOL32 bDoubleLink )
{
	m_bDoubleLink = bDoubleLink;

	return;
}

/*=============================================================================
    �� �� ���� GetLocalMSType
    ��    �ܣ� ��ȡ��������ʱ����MCU��������������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� emMCUMSType ����MCU��������������
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/24  4.0			����                  ����
=============================================================================*/
emMCUMSType CMSManagerConfig::GetLocalMSType( void )
{
	return m_emLocalMSType;
}

/*=============================================================================
    �� �� ���� SetLocalMSType
    ��    �ܣ� ������������ʱ����MCU��������������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� emMCUMSType emLocalMSType
    �� �� ֵ�� void
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/24  4.0			����                  ����
=============================================================================*/
void CMSManagerConfig::SetLocalMSType( emMCUMSType emLocalMSType )
{
	m_emLocalMSType = emLocalMSType;

	return;
}

/*=============================================================================
    �� �� ���� SetRemoteMpcConnected
    ��    �ܣ� ���öԶ��Ƿ��Ѿ�����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� BOOL32
    �� �� ֵ�� void
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/09/25  4.0			����                  ����
=============================================================================*/
void CMSManagerConfig::SetRemoteMpcConnected( BOOL32 bConnect )
{
	m_bRemoteConnected = bConnect;
}

/*=============================================================================
    �� �� ���� IsRemoteMpcConnected
    ��    �ܣ� ���ضԶ��Ƿ��Ѿ�����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� BOOL32
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/09/25  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerConfig::IsRemoteMpcConnected() const
{
	return m_bRemoteConnected;
}

/*=============================================================================
    �� �� ���� GetCurMSState
    ��    �ܣ� ��ȡ��������ʱ����MCU��ǰ��������״̬
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� emMCUMSState ����MCU��ǰ��������״̬
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/24  4.0			����                  ����
=============================================================================*/
emMCUMSState CMSManagerConfig::GetCurMSState( void )
{
	return m_emCurMSState;
}

/*=============================================================================
    �� �� ���� SetCurMSState
    ��    �ܣ� ������������ʱ����MCU��ǰ��������״̬����ͬ��������Ӧ״̬
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� emMCUMSState emCurMSState MCU_MSSTATE_ACTIVE/MCU_MSSTATE_STANDBY
    �� �� ֵ�� void
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/24  4.0			����                  ����
=============================================================================*/
void CMSManagerConfig::SetCurMSState(emMCUMSState emCurMSState)
{
	if (emCurMSState <= MCU_MSSTATE_OFFLINE || emCurMSState > MCU_MSSTATE_ACTIVE)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MSMGR, "[SetCurMSState] invalid argument emCurMSState(%d), Failed!\n", emCurMSState);
		return;
	}
	if (emCurMSState != m_emCurMSState)
	{
		//vxworks�²�֧����Ӳ��ƽֱ̨�ӻ�ȡ�����ý��ʱ��os�ӿ�ͬ��Ӳ��������״̬
#ifndef WIN32
		if (FALSE == m_bMSDetermineType && TRUE == m_bDoubleLink)
		{
			u8 byLocalMSState = BRD_MPC_RUN_SLAVE;
			if (MCU_MSSTATE_ACTIVE == emCurMSState)
			{
				byLocalMSState = BRD_MPC_RUN_MASTER;
			}
			if (OK != BrdMPCSetLocalMSState(byLocalMSState))			
			{
				LogPrint( LOG_LVL_ERROR, MID_MCU_MSMGR, "[SetCurMSState] BrdMPCSetLocalMSState Failed Old_CurMSState.%d New_CurMSState.%d!\n", 
						  m_emCurMSState, emCurMSState);
			}
		}
#endif
		m_emCurMSState = emCurMSState;
	}

	//ͬ�� ����ģ�� ������״̬
	if (m_bInited)
	{
		BOOL32 bMpcEnabled = FALSE;

		if( MCU_MSSTATE_ACTIVE == m_emCurMSState )
		{
			bMpcEnabled = TRUE;
		}

		g_cMcuAgent.SetMpcEnabled(bMpcEnabled);
	}

	return;
}

/*=============================================================================
    �� �� ���� DetermineMSlave
    ��    �ܣ� ����Э��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]     emMCUMSType   emLocalMSType, 
	           [IN/OUT] emMCUMSState  &emLocalMSState, 
	           [IN]     emMCUMSType   emRemoteMSType,
	           [IN/OUT] emMCUMSState  &emRemoteMSState
    �� �� ֵ�� BOOL32 TRUE-����Э�̳ɹ�
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/22  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerConfig::DetermineMSlave( emMCUMSType emLocalMSType, emMCUMSState &emLocalMSState, 
										  emMCUMSType emRemoteMSType, emMCUMSState &emRemoteMSState )
{
	BOOL32 bDetermineok = TRUE;

	//������Э�̣�vxworks����Ӳ��ƽֱ̨�ӻ�ȡ�����ý����������Э��
	//            vxworks�²�֧����Ӳ��ƽֱ̨�ӻ�ȡ�����ý��������WIN32��ʽ����
	//            WIN32�½��в���Osp��ϢЭ��������״̬
	
	if (m_bMSDetermineType)
	{
		if (IsActiveBoard())
		{
			emLocalMSState  = MCU_MSSTATE_ACTIVE;
			emRemoteMSState = MCU_MSSTATE_STANDBY;
		}
		else
		{
			emLocalMSState  = MCU_MSSTATE_STANDBY;
			emRemoteMSState = MCU_MSSTATE_ACTIVE;
		}
	}
	else
	{
		if (MCU_MSSTATE_ACTIVE == emRemoteMSState)
		{
			if (MCU_MSSTATE_ACTIVE == emLocalMSState)
			{
				bDetermineok = FALSE;
			}
			else
			{
				emLocalMSState  = MCU_MSSTATE_STANDBY;
			}
		}
		else if (MCU_MSSTATE_ACTIVE == emLocalMSState)
		{
			if (MCU_MSSTATE_ACTIVE == emRemoteMSState)
			{
				bDetermineok = FALSE;
			}
			else
			{
				emRemoteMSState  = MCU_MSSTATE_STANDBY;
			}
		}
		else
		{
			if (emRemoteMSType == emLocalMSType)
			{			
				bDetermineok = FALSE;
			}
			else
			{
				emRemoteMSState = (emRemoteMSType>emLocalMSType) ? MCU_MSSTATE_ACTIVE : MCU_MSSTATE_STANDBY;
				emLocalMSState  = (emRemoteMSType>emLocalMSType) ? MCU_MSSTATE_STANDBY : MCU_MSSTATE_ACTIVE;
			}
		}
	}

	return bDetermineok;
}

/*=============================================================================
    �� �� ���� IsMSConfigInited
    ��    �ܣ� �Ƿ��Ѿ���ʼ��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 TRUE-�ѳ�ʼ��
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/24  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerConfig::IsMSConfigInited( void )
{
	return m_bInited;
}

/*=============================================================================
�� �� ���� SetMpcOusLed
��    �ܣ� ����8000 mpc��ous���Ƿ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� BOOL32 bOpen
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/3/27  4.0			������                  ����
=============================================================================*/
void CMSManagerConfig::SetMpcOusLed(BOOL32 bOpen)
{
#ifndef WIN32
#ifndef _MINIMCU_    
    if (bOpen)
    {
        BrdLedStatusSet(LED_MPC_OUS, BRD_LED_ON);            
    }
    else
    {
        BrdLedStatusSet(LED_MPC_OUS, BRD_LED_OFF);  
    }    
#endif
#endif

    return;
}

/*=============================================================================
    �� �� ���� SetMSSynOKFlag
    ��    �ܣ� ���� �Ƿ�������������ݵ�����ͬ������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� BOOL32 bSynOK
    �� �� ֵ�� void
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/12  4.0			����                  ����
=============================================================================*/
void CMSManagerConfig::SetMSSynOKFlag(BOOL32 bSynOK)
{
	//FALSE-���ð�δ���ͬ���������ܵ�����Ϣ����ֹ����
	m_bSynOK = bSynOK;

	return;
}

/*=============================================================================
    �� �� ���� IsMSSynOK
    ��    �ܣ� ��ȡ �Ƿ�������������ݵ�����ͬ������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 TRUE-���ð������״̬�¿ɽ�����Ϣ
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/12  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerConfig::IsMSSynOK(void)
{
	return m_bSynOK;
}

/*=============================================================================
�� �� ���� SetMsSwitchOK
��    �ܣ� ���������л��Ƿ�ɹ�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� BOOL32 bSwitchOK
�� �� ֵ�� void  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/2/21  4.0			������                  ����
=============================================================================*/
void   CMSManagerConfig::SetMsSwitchOK(BOOL32 bSwitchOK)
{
    m_bMsSwitchOK = bSwitchOK;
    return;
}

/*=============================================================================
�� �� ���� IsMsSwitchOK
��    �ܣ� �����л��Ƿ�ɹ�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/2/21  4.0			������                ����
=============================================================================*/
BOOL32 CMSManagerConfig::IsMsSwitchOK(void)
{
    return m_bMsSwitchOK;
}


/*=============================================================================
    �� �� ���� JudgeSndMsgPass
    ��    �ܣ� ��ȡ �Ƿ�����ҵ��ģ�����ⲿ��AppͶ����Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 FALSE- ���� TRUE-����
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/12  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerConfig::JudgeSndMsgPass(void)
{
	BOOL32 bRet = FALSE;
	
	//�������߻��߱����������������������ⲿ��AppͶ����Ϣ
	if (MCU_MSSTATE_ACTIVE == m_emCurMSState )
	{
		bRet = TRUE;
	}
	
	return bRet;
}

/*=============================================================================
    �� �� ���� JudgeRcvMsgPass
    ��    �ܣ� ��ȡ �Ƿ�����ҵ��ģ����ո��ⲿApp��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 FALSE- ���� TRUE-����
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/12  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerConfig::JudgeRcvMsgPass(void)
{	
	//�������߻��߱�����δ��ȫͬ������������������ҵ��ģ��Ͷ����Ϣ
	if (MCU_MSSTATE_OFFLINE == m_emCurMSState || 
		(MCU_MSSTATE_STANDBY == m_emCurMSState && FALSE == m_bSynOK))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

/*=============================================================================
    �� �� ���� EnterMSInitLock
    ��    �ܣ� ���� ����ģ���ʼ������ ���� ��ֻ��һ�Σ�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 ������������ź���ʱ����ʧ�ܣ�
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/12  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerConfig::EnterMSInitLock( void )
{
	BOOL32 bRet = FALSE;

	if (NULL != m_hSemInitLock)
	{
		bRet = OspSemTakeByTime(m_hSemInitLock, MS_INIT_LOCK_SPAN_TIMEOUT);
	}

	return bRet;
}

/*=============================================================================
    �� �� ���� LeaveMSInitLock
    ��    �ܣ� �˳� ����ģ���ʼ������ ���� ��ֻ��һ�Σ�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� void�����ź���ʱֱ�ӷ��أ�
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/12  4.0			����                  ����
=============================================================================*/
void CMSManagerConfig::LeaveMSInitLock( void )
{	
	if (NULL != m_hSemInitLock)
	{
		OspSemGive(m_hSemInitLock);
	}

	return;
}

/*=============================================================================
    �� �� ���� EnterMSSynLock
    ��    �ܣ� ���� ����ģ��App���ⲿҵ��ģ��App��������ͬ������ ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u16 wAppId AppӦ��ID
    �� �� ֵ�� void�����ź���ʱֱ�ӷ��أ�
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/15  4.0			����                  ����
=============================================================================*/
void CMSManagerConfig::EnterMSSynLock( u16 wAppId )
{
	if (NULL == m_hSemSynLock)
	{
		return;
	}

    OspSemTakeByTime(m_hSemSynLock, MS_SYN_LOCK_SPAN_TIMEOUT);
//	OspSemTake(m_hSemSynLock);
	m_wLockedAppId = wAppId;

	return;
}

/*=============================================================================
    �� �� ���� LeaveMSSynLock
    ��    �ܣ� �˳� ����ģ��App���ⲿҵ��ģ��App��������ͬ������ ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u16 wAppId AppӦ��ID
    �� �� ֵ�� void�����ź���ʱֱ�ӷ��أ�
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/15  4.0			����                  ����
=============================================================================*/
void CMSManagerConfig::LeaveMSSynLock( u16 wAppId )
{
	if (NULL == m_hSemSynLock)
	{
		return;
	}
	
	//���˿��ܵĶ��LeaveMSSynLock
	if (m_wLockedAppId == wAppId)
	{
		m_wLockedAppId = 0;
		OspSemGive(m_hSemSynLock);
	}

	return;
}


/*=============================================================================
�� �� ���� IncDitheringTimes
��    �ܣ� Ӳ������������1
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/2/18  4.0			������                  ����
=============================================================================*/
void CMSManagerConfig::IncDitheringTimes( void )
{
    m_dwDitheringTimes++;
    return;
}


/*=============================================================================
�� �� ���� GetDitheringTimes
��    �ܣ� ��ȡϵͳ��������Ӳ���ܶ�������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� u32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/2/18  4.0			������                  ����
=============================================================================*/
u32 CMSManagerConfig::GetDitheringTimes( void )
{
    return m_dwDitheringTimes;
}

/*=============================================================================
�� �� ���� SetMSSsrc
��    �ܣ� ����ϵͳ�ỰУ��ֵ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/6/11   4.0			����                  ����
=============================================================================*/
void CMSManagerConfig::SetMSSsrc( u32 dwSSrc )
{
    m_dwSysSSrc = htonl(dwSSrc);
}

/*=============================================================================
�� �� ���� GetMSSsrc
��    �ܣ� ��ȡϵͳ�ỰУ��ֵ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� u32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/6/11   4.0			����                  ����
=============================================================================*/
u32 CMSManagerConfig::GetMSSsrc( void ) const
{
    return ntohl(m_dwSysSSrc);
}

/*=============================================================================
�� �� ���� SetCurMSSynState
��    �ܣ� ��������ͬ��ʧ��״̬
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TMSSynState *ptMSSynState��ʧ��״̬��¼
�� �� ֵ�� void
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/06/02  4.0			�ű���                  ����
=============================================================================*/
void CMSManagerConfig::SetCurMSSynState( TMSSynState &tMSSynState )
{
    m_tMSSynState = tMSSynState;
    return;
}

/*=============================================================================
�� �� ���� GetCurMSSynState
��    �ܣ� ��ȡ����ͬ��ʧ��״̬
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� TMSSynState
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/06/02  4.0			�ű���                  ����
=============================================================================*/
TMSSynState CMSManagerConfig::GetCurMSSynState()
{
    return m_tMSSynState;
}

/*=============================================================================
    �� �� ���� GetDebugLevel
    ��    �ܣ� ��ȡ���Դ�ӡ�ȼ�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� u8
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/24  4.0			����                  ����
=============================================================================*/
u8 CMSManagerConfig::GetDebugLevel( void )
{
	return m_byDebugLevel;
}

/*=============================================================================
    �� �� ���� SetDebugLevel
    ��    �ܣ� ���õ��Դ�ӡ�ȼ�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byDebugLevel
    �� �� ֵ�� void
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/24  4.0			����                  ����
=============================================================================*/
void CMSManagerConfig::SetDebugLevel( u8 byDebugLevel )
{
	if (byDebugLevel > MS_DEBUG_VERBOSE)
	{
		byDebugLevel = MS_DEBUG_VERBOSE;
	}
	m_byDebugLevel = byDebugLevel;

	return;
}

/*=============================================================================
    �� �� ���� GetMSDetermineType
    ��    �ܣ� ��ȡ �Ƿ���ö�ʱ����ȡӲ��������״̬
	           vxworks��Ĭ�ϲ��ö�ʱ����ȡӲ��������״̬��
	           vxworks�²�֧����Ӳ��ƽֱ̨�ӻ�ȡ�����ý��������WIN32��ʽ����
	           WIN32�¹̶�����Osp��ϢЭ��������״̬����������������Ϊ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 TRUE�����ö�ʱ����ȡӲ��������״̬��FALSE������Osp��ϢЭ��������״̬
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/24  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerConfig::GetMSDetermineType( void )
{
	return m_bMSDetermineType;
}

/*=============================================================================
    �� �� ���� SetMSDetermineType
    ��    �ܣ� ���� �Ƿ���ö�ʱ����ȡӲ��������״̬
	           vxworks��Ĭ�ϲ��ö�ʱ����ȡӲ��������״̬��
	           vxworks�²�֧����Ӳ��ƽֱ̨�ӻ�ȡ�����ý��������WIN32��ʽ����
	           WIN32�¹̶�����Osp��ϢЭ��������״̬����������������Ϊ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� BOOL32 bMSDetermineType TRUE�����ö�ʱ����ȡӲ��������״̬��FALSE������Osp��ϢЭ��������״̬
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/24  4.0			����                  ����
=============================================================================*/
void CMSManagerConfig::SetMSDetermineType( BOOL32 bMSDetermineType )
{
#ifdef WIN32
	m_bMSDetermineType = FALSE;
#else
	m_bMSDetermineType = bMSDetermineType;
#endif

	return;
}

/*=============================================================================
    �� �� ���� RebootMCU
    ��    �ܣ� ����MCU
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� void
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/22  4.0			����                  ����
=============================================================================*/
void CMSManagerConfig::RebootMCU( void )
{
#ifndef _8KH_
	OspQuit();
	OspDelay(1000);
#endif

#ifndef WIN32
	BrdHwReset();
	printf("[CMSManagerConfig]ms reboot\n");	
#endif

	return;
}

/*=============================================================================
    �� �� ���� SetLocalMcuTime
    ��    �ܣ� ���ñ���ϵͳʱ��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� time_t tTime
    �� �� ֵ�� BOOL32
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/08  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerConfig::SetLocalMcuTime(time_t tTime)
{
	tTime += 0;  //��������
    u16 wRet = g_cMcuAgent.SetSystemTime(tTime);
	return ( SUCCESS_AGENT == wRet);
}

/*=============================================================================
    �� �� ���� IsActiveBoard
    ��    �ܣ� ��ȡ ��ǰ�Ƿ�Ϊ���ð�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� BOOL32 TRUE - ���� FALSE - ����
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/08  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerConfig::IsActiveBoard(void)
{
#ifndef WIN32  
#ifndef _MINIMCU_
    u8 byLocalMSState = BRD_MPC_RUN_SLAVE;
    byLocalMSState = BrdMPCQueryLocalMSState();
    LogPrint(LOG_LVL_DETAIL, MID_MCU_MSMGR, "[IsActiveBoard] BrdMPCQueryLocalMSState->%d!\n", byLocalMSState);
    return (BRD_MPC_RUN_MASTER == byLocalMSState);
#else
    return TRUE;
#endif
#else   // WIN32
    return ( MCU_MSSTATE_ACTIVE == m_emCurMSState );
#endif    
}

/*=============================================================================
    �� �� ���� GetSlotThroughName
    ��    �ܣ� �ɵ���ۺŵõ���������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const s8* pszName
	           u8* pbySlot
    �� �� ֵ�� BOOL32
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/08  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerConfig::GetSlotThroughName(const s8* pszName, u8* pbySlot)
{
	if (0 == memcmp(pszName, "EX", 2) || 0 == memcmp(pszName, "MC", 2))
	{
		*pbySlot = atoi(pszName+2);
		return TRUE;
	}
	else if ( 0 == memcmp(pszName, "TVSEX", 5) )
	{
		*pbySlot = atoi(pszName+5);
		return TRUE;
	}
	else
	{
		*pbySlot = 0;
		return FALSE;
	}
}

/*=============================================================================
    �� �� ���� GetTypeThroughName
    ��    �ܣ� �ɵ������͵õ���������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const s8* pszName
	           u8* pbyType
    �� �� ֵ�� BOOL32
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/08  4.0			����                  ����
=============================================================================*/
BOOL32 CMSManagerConfig::GetTypeThroughName( const s8* pszName, u8* pbyType )
{
	if (0 == strcmp(pszName, "MPC"))
	{
		*pbyType = BRD_TYPE_MPC/*DSL8000_BRD_MPC*/;
		return TRUE;
	}
	//  [1/21/2011 chendaiwei]֧��MPC2
	if (0 == strcmp(pszName, "MPC2"))
	{
		*pbyType = BRD_TYPE_MPC2/*DSL8000_BRD_MPC2*/;
		return TRUE;
	}
	else if (0 == strcmp(pszName, "CRI"))
	{
		*pbyType = BRD_TYPE_CRI/*DSL8000_BRD_CRI*/;
		return TRUE;
	}
	//  [1/21/2011 chendaiwei] CRI2֧��
	else if (0 == strcmp(pszName, "CRI2"))
	{
		*pbyType = BRD_TYPE_CRI2/*DSL8000_BRD_CRI*/;
		return TRUE;
	}
	else if (0 == strcmp(pszName, "DTI"))
	{
		*pbyType = BRD_TYPE_DTI/*DSL8000_BRD_DTI*/;
		return TRUE;
	}
	else if (0 == strcmp(pszName, "DIC"))
	{
		*pbyType = BRD_TYPE_DIC/*DSL8000_BRD_DIC*/;
		return TRUE;
	}
	else if (0 == strcmp(pszName, "DRI"))
	{
		*pbyType = BRD_TYPE_DRI/*DSL8000_BRD_DRI*/;
		return TRUE;
	}
	//  [1/21/2011 chendaiwei]DRI2֧��
	else if (0 == strcmp(pszName, "DRI2"))
	{
		*pbyType = BRD_TYPE_DRI2/*DSL8000_BRD_DRI2*/;
		return TRUE;
	}
	else if (0 == strcmp(pszName, "MMP"))
	{
		*pbyType = BRD_TYPE_MMP/*DSL8000_BRD_MMP*/;
		return TRUE;
	}
	else if (0 == strcmp(pszName, "VPU"))
	{
		*pbyType = BRD_TYPE_VPU/*DSL8000_BRD_VPU*/;
		return TRUE;
	}
	else if (0 == strcmp(pszName, "DEC5"))
	{
		*pbyType = BRD_TYPE_DEC5/*DSL8000_BRD_DEC5*/;
		return TRUE;
	}
	else if (0 == strcmp(pszName, "VAS"))
	{
		*pbyType = BRD_TYPE_VAS/*DSL8000_BRD_VAS*/;
		return TRUE;
	}
	else if (0 == strcmp(pszName, "IMT"))
	{
		*pbyType = BRD_TYPE_IMT/*DSL8000_BRD_IMT*/;
		return TRUE;
	}
	else if (0 == strcmp(pszName, "APU"))
	{
		*pbyType = BRD_TYPE_APU/*DSL8000_BRD_APU*/;
		return TRUE;
	}
	else if (0 == strcmp(pszName, "DSI"))
	{
		*pbyType = BRD_TYPE_DSI/*DSL8000_BRD_DSI*/;
		return TRUE;
	}
	else if( 0 == strcmp(pszName, "DSC") )
	{
		*pbyType = BRD_TYPE_DSC/*KDV8000B_MODULE*/;
		return TRUE;
	}
	else if( 0 == strcmp( pszName, "MDSC") )
	{
		*pbyType = BRD_TYPE_MDSC/*DSL8000_BRD_MDSC*/;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, "HDSC") )
	{
		*pbyType = BRD_TYPE_HDSC;
		return TRUE;
	}
	//4.6 �¼Ӱ汾 jlb  20090105
	else if ( 0 == strcmp( pszName, "HDU") )
	{
		*pbyType = BRD_TYPE_HDU/*BRD_HWID_KDM200_HDU*/;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, "HDU_L") )
	{
		*pbyType = BRD_TYPE_HDU_L;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, "MPU") )
	{
		*pbyType = BRD_TYPE_MPU/*BRD_HWID_DSL8000_MPU*/;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, "EBAP") )
	{
		*pbyType = BRD_HWID_EBAP;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, "EVPU") )
	{
		*pbyType = BRD_HWID_EVPU;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, "EAPU") )
	{
		*pbyType = BRD_TYPE_EAPU;
		return TRUE;
	}
	//[2011/01/25 zhushz]IS2.x����֧��
	else if ( 0 == strcmp( pszName, "IS21") )
	{
		*pbyType = BRD_TYPE_IS21;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, "IS22") )
	{
		*pbyType = BRD_TYPE_IS22;
		return TRUE;
	}
	// 4.7�汾�°忨 [1/31/2012 chendaiwei]
	else if ( 0 == strcmp( pszName, BOARD_TYPE_HDU2) )
	{
		*pbyType = BRD_TYPE_HDU2;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, BOARD_TYPE_HDU2_S) )
	{
		*pbyType = BRD_TYPE_HDU2_S;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, BOARD_TYPE_HDU2_L) )
	{
		*pbyType = BRD_TYPE_HDU2_L;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, BOARD_TYPE_MPU2) )
	{
		*pbyType = BRD_TYPE_MPU2;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, BOARD_TYPE_MPU2ECARD) )
	{
		*pbyType = BRD_TYPE_MPU2ECARD;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, BOARD_TYPE_APU2) )
	{
		*pbyType = BRD_TYPE_MPU2;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*=============================================================================
    �� �� ���� msdebug
    ��    �ܣ� ���������ȱ���ģ����Դ�ӡ�ȼ�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� s32 nDbgLvl
    �� �� ֵ�� void
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/24  4.0			����                  ����
=============================================================================*/
API void msdebug( s32 nDbgLvl )
{
	if( nDbgLvl > 0 ) 
	{
		logenablemod(MID_MCU_MSMGR);
	}
	else
	{
		logdisablemod(MID_MCU_MSMGR);
	}

	if( nDbgLvl < MS_DEBUG_CRITICAL ) 
		nDbgLvl = MS_DEBUG_CRITICAL;
	if( nDbgLvl > MS_DEBUG_VERBOSE )
		nDbgLvl = MS_DEBUG_VERBOSE;
	
	g_cMSSsnApp.SetDebugLevel((u8)nDbgLvl);	

	return;
}

/*=============================================================================
�� �� ���� setmschecktime
��    �ܣ� ��������״̬���ʱ����(���Խӿ�)
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u32 dwTime (��λms)
�� �� ֵ�� API void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/2/18  4.0			������                  ����
=============================================================================*/
API void setmschecktime(u32 dwTime) 
{
    if(dwTime >= 50) //��С50ms
    {
        g_dwMsCheckTime = dwTime;
    }    
    
    return;
}

/*=============================================================================
    �� �� ���� showmsstate
    ��    �ܣ� ��ӡ����״̬
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� s32 nDbgLvl
    �� �� ֵ�� void
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/10/24  4.0			����                  ����
=============================================================================*/
API void showmsstate()
{
	s8  achPrt[MS_MAX_STRING_LEN];
	s8  achPrt2[MS_MAX_STRING_LEN];
	u32 dwLocalMcuIP   = g_cMSSsnApp.GetLocalMcuIP();
	u32 dwRemoteMcuIP  = g_cMSSsnApp.GetAnotherMcuIP();
	u8  byLocalMcuType = g_cMSSsnApp.GetLocalMcuType();
	BOOL32 bDoubleLink = g_cMSSsnApp.IsDoubleLink();
	
	BOOL32 bInited   = g_cMSSsnApp.IsMSConfigInited();
    BOOL32 bConnected= g_cMSSsnApp.IsRemoteMpcConnected();
	BOOL32 bSynOK    = g_cMSSsnApp.IsMSSynOK();	
    BOOL32 bSwitchOk = g_cMSSsnApp.IsMsSwitchOK();	
	BOOL32 bMSDetermineType    = g_cMSSsnApp.GetMSDetermineType();
	emMCUMSType  emLocalMSType = g_cMSSsnApp.GetLocalMSType();
	emMCUMSState emCurMSState  = g_cMSSsnApp.GetCurMSState();
    u32 dwDitheringTimes = g_cMSSsnApp.GetDitheringTimes();

	u32 dwSysSSrc = g_cMSSsnApp.GetMSSsrc();
	
	memset(achPrt, 0, sizeof(achPrt));
	if (MCU_TYPE_KDV8000 == byLocalMcuType)
	{
		const s8* pszName = "KDV8000A";
		const s32 nDstBufLen = sizeof(achPrt);
		const s32 nSrcNameLen = strlen(pszName) + 1;
		const s32 nMaxCpyLen = min(nSrcNameLen, nDstBufLen);
		memcpy(achPrt, pszName, nMaxCpyLen);
		achPrt[MS_MAX_STRING_LEN-1] = '\0';
	}
	else if (MCU_TYPE_KDV8000B == byLocalMcuType)
	{
		const s8* pszName = "KDV8000B";
		memcpy(achPrt, "KDV8000B", strlen(pszName) + 1);
		achPrt[MS_MAX_STRING_LEN-1] = '\0';
	}
	else if (MCU_TYPE_WIN32 == byLocalMcuType)
	{
		const s8* pszName = "KDVWIN32";
		const s32 nDstBufLen = sizeof(achPrt);
		const s32 nSrcNameLen = strlen(pszName) + 1;
		const s32 nMaxCpyLen = min(nSrcNameLen, nDstBufLen);
		memcpy(achPrt, pszName, nMaxCpyLen);
		achPrt[MS_MAX_STRING_LEN-1] = '\0';
	}
    else if (MCU_TYPE_KDV8000C == byLocalMcuType)
    {
		const s8* pszName = "8000C";
		const s32 nDstBufLen = sizeof(achPrt);
		const s32 nSrcNameLen = strlen(pszName) + 1;
		const s32 nMaxCpyLen = min(nSrcNameLen, nDstBufLen);
		memcpy(achPrt, pszName, nMaxCpyLen);
        achPrt[MS_MAX_STRING_LEN-1] = '\0';
    }    
	else
	{
		const s8* pszName = "UNKNOWN";
		const s32 nDstBufLen = sizeof(achPrt);
		const s32 nSrcNameLen = strlen(pszName) + 1;
		const s32 nMaxCpyLen = min(nSrcNameLen, nDstBufLen);
		memcpy(achPrt, pszName, nMaxCpyLen);
		achPrt[MS_MAX_STRING_LEN-1] = '\0';
	}
	
	StaticLog("MS Base State: LocalMcuIP:0x%0x RemoteMcuIP:0x%0x McuType:%s DoubleLink:%d Connected: %d SysSSRC:%u\n", 
		      dwLocalMcuIP, dwRemoteMcuIP, achPrt, bDoubleLink, bConnected, dwSysSSrc);

	memset(achPrt, 0, sizeof(achPrt));
	if (MCU_MSTYPE_MASTER == emLocalMSType)
	{
		const s8* pszName = "MC1(MST)";
		const s32 nDstBufLen = sizeof(achPrt);
		const s32 nSrcNameLen = strlen(pszName) + 1;
		const s32 nMaxCpyLen = min(nSrcNameLen, nDstBufLen);
		memcpy(achPrt, pszName, nMaxCpyLen);
		achPrt[MS_MAX_STRING_LEN-1] = '\0';
	}
	else if (MCU_MSTYPE_SLAVE == emLocalMSType)
	{
		const s8* pszName = "MC0(SLV)";
		const s32 nDstBufLen = sizeof(achPrt);
		const s32 nSrcNameLen = strlen(pszName) + 1;
		const s32 nMaxCpyLen = min(nSrcNameLen, nDstBufLen);
		memcpy(achPrt, pszName, nMaxCpyLen);
		achPrt[MS_MAX_STRING_LEN-1] = '\0';
	}
	else
	{
		const s8* pszName = "INVALID";
		const s32 nDstBufLen = sizeof(achPrt);
		const s32 nSrcNameLen = strlen(pszName) + 1;
		const s32 nMaxCpyLen = min(nSrcNameLen, nDstBufLen);
		memcpy(achPrt, pszName, nMaxCpyLen);
		achPrt[MS_MAX_STRING_LEN-1] = '\0';
	}
	
	memset(achPrt2, 0, sizeof(achPrt2));
	if (MCU_MSSTATE_ACTIVE == emCurMSState)
	{
		const s8* pszName = "ACTIVE";
		const s32 nDstBufLen = sizeof(achPrt);
		const s32 nSrcNameLen = strlen(pszName) + 1;
		const s32 nMaxCpyLen = min(nSrcNameLen, nDstBufLen);
		memcpy(achPrt, pszName, nMaxCpyLen);
		achPrt2[MS_MAX_STRING_LEN-1] = '\0';
	}
	else if (MCU_MSSTATE_STANDBY == emCurMSState)
	{
		const s8* pszName = "STANDBY";
		const s32 nDstBufLen = sizeof(achPrt);
		const s32 nSrcNameLen = strlen(pszName) + 1;
		const s32 nMaxCpyLen = min(nSrcNameLen, nDstBufLen);
		memcpy(achPrt, pszName, nMaxCpyLen);
		achPrt2[MS_MAX_STRING_LEN-1] = '\0';
	}
	else
	{
		const s8* pszName = "OFFLINE";
		const s32 nDstBufLen = sizeof(achPrt);
		const s32 nSrcNameLen = strlen(pszName) + 1;
		const s32 nMaxCpyLen = min(nSrcNameLen, nDstBufLen);
		memcpy(achPrt, pszName, nMaxCpyLen);
		achPrt2[MS_MAX_STRING_LEN-1] = '\0';
	}
	
	StaticLog("MS Comm State: MSType:%s MSState:%s Inited:%d SynOK:%d SwitchOK:%d MSDetermineType:%d HWDitheringTimes:%u\n", 
		      achPrt, achPrt2, bInited, bSynOK, bSwitchOk, bMSDetermineType, dwDitheringTimes);

	return;
}

//END OF FILE
