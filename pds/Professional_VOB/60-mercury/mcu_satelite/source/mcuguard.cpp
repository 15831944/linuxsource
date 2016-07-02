/*****************************************************************************
   ģ����      : mcu
   �ļ���      : mcuguard.cpp
   ����ļ�    : mcuguard.h
   �ļ�ʵ�ֹ���: MCU����Ӧ���ඨ��
   ����        : ����
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2002/07/25  0.9         ����        ����
******************************************************************************/

#include "evmcu.h"
#include "mcuvc.h"
#include "mcuguard.h"
#include "kdvencrypt.h"
#include "readlicense.h"
#include "usbkeyconst.h"
#include "licensekeyconst.h"

#if defined(_VXWORKS_)
#include "memlib.h"
#include "tasklib.h"
#include "brddrvlib.h"
#elif defined(_LINUX_)
#include "boardwrapper.h"
#endif

CMcuGuardApp	g_cMcuGuardApp;	//MCU����Ӧ��ʵ��
BOOL32  g_bEnableMcuGuard = TRUE;

#ifdef _LINUX_
void writeexclog(const s8 *pachBuf, s32 nBufLen);
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMcuGuardInst::CMcuGuardInst()
{
	m_dwGuardTestSeriNo = 0;
	m_byCurSyncSourceState = 5; //����ʼ״̬��ΪSYNCSOURCE_NORMAL��SYNCSOURCE_ABNORMAL�����ֵ
	m_bySyncSourceMode  = 0;
	m_byTaskState      = 0;
    m_bRebootMcu = FALSE;
	memset( m_abyTaskBusyTimes, 0, sizeof( m_abyTaskBusyTimes ) );
}

CMcuGuardInst::~CMcuGuardInst()
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
    2003-4-16					���
====================================================================*/
void CMcuGuardInst::InstanceEntry( CMessage * const pcMsg )
{
	if( NULL == pcMsg )
	{
		OspPrintf( TRUE, FALSE, "CMcuVcInst: The received msg's pointer in the msg entry is NULL!");
		return;
	}
	
	switch( pcMsg->event )
	{
		case OSP_POWERON:				//�ϵ���Ϣ
			ProcReoPowerOn( );
			break;
		case MCUGD_SCHEDULED_CHECK_TIMER:
			ProcTaskCheck();
			ProcMemCheck();
			ProcFileSystemCheck();
			ProcCpuStatusCheck();
			ProcSyncSrcStateCheck();
			//Send Test packet 			
			PingEveryTask();
			//restart the check timer
			SetTimer( MCUGD_SCHEDULED_CHECK_TIMER, TIMESPACE_GUARD_ONGOING_CHECK/6 );
			break;
		case MCUGD_FEEDDOG_TIMER:
			ProcTimerFeedDog();
			break;
		case MCU_APPTASKTEST_ACK:
			ProcTaskTestAck( pcMsg );
			break;
		case AGT_SVC_REBOOT:
			ProcPowerOff();
			break;
		case MCU_STOPGUARDTIMER_CMD:
			KillTimer( MCUGD_SCHEDULED_CHECK_TIMER );
			break;
		case MCU_STARTGUARDTIMER_CMD:
			SetTimer( MCUGD_SCHEDULED_CHECK_TIMER, TIMESPACE_GUARD_ONGOING_CHECK/6 );
			break;		
		// get license data when mcu power on 
		case MCUGD_GETLICENSE_DATA:
			ProcGetLicenseDataFromFile( pcMsg );
			break;

		default:
			break;
	}
	return;
}

/*====================================================================
    ������      ��ProcReoPowerOn
    ����        ���ϵ紦��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2003-4-16					���
====================================================================*/
void CMcuGuardInst::ProcReoPowerOn( )
{
	//set reboot msg
    g_cMcuAgent.SetRunningMsgDst( AGT_SVC_POWERON, MAKEIID(AID_MCU_GUARD, 1) );
	g_cMcuAgent.SetRebootMsgDst( AGT_SVC_REBOOT, MAKEIID(AID_MCU_GUARD, 1) );
	g_cMcuAgent.SetPowerOffMsgDst( AGT_SVC_POWEROFF, MAKEIID(AID_MCU_GUARD, 1) );
	
	//Set the sync source mode
	TNetSyncInfo tNetSyncInfo;
	g_cMcuAgent.GetNetSyncInfo(&tNetSyncInfo);
	m_bySyncSourceMode = tNetSyncInfo.GetMode();
	ProcSyncSrcStateCheck();
	//start the check timer
	PingEveryTask();
	SetTimer( MCUGD_SCHEDULED_CHECK_TIMER, TIMESPACE_GUARD_ONGOING_CHECK/6 );

	// change feed dog mode

#ifndef WIN32
	if (g_cMcuVcApp.IsWatchDogEnable())
	{

#if defined(_VXWORKS_)
        // Ӳ��ι����ҵ�񲻹���
		BrdMpcWatchdogMode( WATCHDOG_USE_CLK );
#elif defined(_LINUX_)

        // ����ϵͳ ι�����, ʱ�䵥λΪ��
        SysOpenWdGuard( TIMESPACE_FEEDDOG * 3 / 1000);
        // ��ʼι��
        SetTimer( MCUGD_FEEDDOG_TIMER, TIMESPACE_FEEDDOG);		
#endif
	}
	else
	{
		SysRebootDisable();
	}
#endif

	return;
}

/*====================================================================
    ������      ��ProcMemCheck
    ����        ���ڴ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2003-4-16					���
====================================================================*/
void CMcuGuardInst::ProcMemCheck(void)
{
#ifdef _VXWORKS_
	MEM_PART_STATS  tMemPartInfo;

	memPartInfoGet(memSysPartId, &tMemPartInfo);
	u32 dwMemTop = (u32)sysPhysMemTop();
	//u32 dwMemAllocRate = tMemPartInfo.numBytesAlloc /(dwMemTop/100);

    u32 dwMemSpare = dwMemTop - tMemPartInfo.numBytesAlloc;

    if (dwMemSpare < MEMSPACE_MIN_SIZE)
    {
        OspPrintf( TRUE, FALSE, "CMcuGuardInst: The Free system memory is not enough(%d<3M)!\n", dwMemSpare);
    }
/*  else
    {
        OspPrintf( TRUE, FALSE, "CMcuGuardInst: dwMemTop is %d!\n", dwMemTop);
        OspPrintf( TRUE, FALSE, "CMcuGuardInst: tMemPartInfo.numBytesAlloc is %d!\n", tMemPartInfo.numBytesAlloc);
        OspPrintf( TRUE, FALSE, "CMcuGuardInst: The Free system memory is %d!\n", dwMemSpare);
    }*/
    
/*
	if( dwMemAllocRate > STATUS_ALLFULL )
	{
		OspPrintf( TRUE, FALSE, "CMcuGuardInst: The Free system memory is not enough(>%95)!\n");
	}
	else if( dwMemAllocRate > STATUS_CONGESTION )
	{
		OspPrintf( TRUE, FALSE, "CMcuGuardInst: The use of system memory is congestion(>%85)!\n");
	}
	else if( dwMemAllocRate > STATUS_BUSY )
	{
		OspPrintf( TRUE, FALSE, "CMcuGuardInst: The use of system memory is busy(>%75)!\n");
	}
	else
	{

	}
*/
#endif

	return;
}

/*====================================================================
    ������      ��ProcFileSystemCheck
    ����        ���ļ�ϵͳ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2003-4-16					���
====================================================================*/
void CMcuGuardInst::ProcFileSystemCheck(void)
{
	u8 byTimes = 5;

	for (u8 bycounter=0; bycounter<byTimes; bycounter++)
	{//creat a file and continue to open it five times
		
	}

	return;
}

/*====================================================================
    ������      ��ProcTaskCheck
    ����        ��MCU������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2003-4-16					���
====================================================================*/
void CMcuGuardInst::ProcTaskCheck(void)
{
	//Check whether every App's Ack came.
	for (u8 byLoop = 0; byLoop < SHIFT_END_CHECK; byLoop++)
	{
		ProcEachTaskCheck( byLoop );
	}

	return;
}

/*====================================================================
    ������      ��ProcTaskTestAck
    ����        ��MCU������ȷ�ϴ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * pcMsg �����������������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2003-4-16				���
====================================================================*/
void CMcuGuardInst::ProcTaskTestAck( const CMessage * pcMsg )
{
	u16  wAppId;
	char achName[32];
	u32  dwMsgSeriNo = *(u32 *)pcMsg->content;
	
	if (dwMsgSeriNo != m_dwGuardTestSeriNo)
	{
		return;
	}

	wAppId = GETAPP(pcMsg->srcid);
	switch ( wAppId )
	{
		case AID_MCU_MCSSN:
			m_byTaskState |= ( u8 )1 << SHIFT_MCSSN_CHECK;
			strncpy( achName, "McSsn", sizeof( achAppName ) );
			achName[ sizeof( achName ) - 1 ] = 0;
			break;
		case AID_MCU_PERIEQPSSN:
			m_byTaskState |= ( u8 )1 << SHIFT_PERIEQPSSN_CHECK;
			strncpy( achName, "EqpSsn", sizeof( achAppName ) );
			achName[ sizeof( achName ) - 1 ] = 0;
			break;
		case AID_MCU_VC:
			m_byTaskState |= ( u8 )1 << SHIFT_MCUVC_CHECK; 
			strncpy( achName, "McuVc", sizeof( achAppName ) );
			achName[ sizeof( achName ) - 1 ] = 0;
			break;
		case AID_MCU_CONFIG:
			m_byTaskState |= ( u8 )1 << SHIFT_MCUCONFIG_CHECK;
			strncpy( achName, "McuConfig", sizeof( achAppName ) );
			achName[ sizeof( achName ) - 1 ] = 0;
			break;
		case AID_MCU_MPSSN:
			m_byTaskState |= ( u8 )1 << SHIFT_MPSSN_CHECK;
			strncpy( achName, "MpSsn", sizeof( achAppName ) );
			achName[ sizeof( achName ) - 1 ] = 0;
			break;
		case AID_MCU_MTADPSSN:
			m_byTaskState |= ( u8 )1 << SHIFT_MTADPSSN_CHECK;
			strncpy( achName, "MtadpSsn", sizeof( achAppName ) );
			achName[ sizeof( achName ) - 1 ] = 0;
			break;
        case AID_MCU_DCSSSN:
            m_byTaskState |= ( u8 )1 << SHIFT_DCSSSN_CHECK;
            strncpy( achName, "DcsSsn", sizeof( achAppName ) );
            achName[ sizeof( achName ) - 1 ] = 0;
            break;
		default:
			OspPrintf(TRUE, FALSE, "CMcuGuardInst: receive the wrong task ack msg\n");
			break;
	}

	GuardLog("Ping app %s ack received!\n", achName);	
	return;
}

/*====================================================================
    ������      ��ProcCpuStatusCheck
    ����        ��CPU���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2003-4-16					���
====================================================================*/
void CMcuGuardInst::ProcCpuStatusCheck(void)
{
	return;
}

/*====================================================================
    ������      ��ProcTimerSyncSourceCheck(void)
    ����        ��ͬ��Դ״̬���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2003-7-2				JQL
====================================================================*/
void CMcuGuardInst::ProcSyncSrcStateCheck( void )
{
	//��ʱ���
	u8	bySyncCurState = GuardGetBrdNetSyncLock();
	CSyncSourceAlarm cSyncSourceAlarm;
	
	//Only Post Message to Agent when Source Sync changed!
	if( bySyncCurState != m_byCurSyncSourceState )
	{
		m_byCurSyncSourceState = bySyncCurState;
		memset( &cSyncSourceAlarm, 0, sizeof( CSyncSourceAlarm ) );		
		cSyncSourceAlarm.masterstatus = m_byCurSyncSourceState;
		post( MAKEIID( AID_MCU_AGENT, 1),  MCU_AGT_SYNCSOURCE_STATUSCHANGE, &cSyncSourceAlarm, sizeof( CSyncSourceAlarm ) );	
	}
}


/*====================================================================
    ������      ��ProcPowerOff
    ����        ���µ���Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2003-4-16					���
====================================================================*/
void CMcuGuardInst::ProcPowerOff(void)
{
    //��������MCU�����µ�ǰMCU����״����
#if ( defined _VXWORKS_ ) && ( !defined _MINIMCU_ )
    FILE  *hFilestream = NULL;
    char   achFullName[] = { "/log/mcudump.log" } ;
    time_t currenttime;
    
    hFilestream = fopen( achFullName, "a+" ); 
    if (NULL != hFilestream)
    {
        MEM_PART_STATS  tMemPartInfo;			
        memPartInfoGet(memSysPartId, &tMemPartInfo);
        u32 dwMemAlloc = tMemPartInfo.numBytesAlloc;
        time( &currenttime );			
        
        fprintf( hFilestream, "Mcu Reboot, Current Time %s.\n", ctime( &currenttime ) );
        fprintf( hFilestream, "System Memory Info: %u Memory Alloced.\n", dwMemAlloc );
        
        for (u8 byIndex = 0; byIndex <= SHIFT_MTADPSSN_CHECK; byIndex++)
        {
            if (m_abyTaskBusyTimes[byIndex] > 0)
            {
                fprintf( hFilestream, "App shift %d, No Ack for %d Times.\n", byIndex, m_abyTaskBusyTimes[byIndex] );
            }		
        }
        
        int taskId = taskNameToId( achAppName );
        if (ERROR != taskId)
        {
            TASK_DESC taskDesc;
            if (ERROR != taskInfoGet( taskId, &taskDesc ))
            {
                fprintf( hFilestream, "Task Name:%s,  StackSize:%d,	 StackMargin:%d,  ErrorCode:%d.\n", 
                    taskDesc.td_name, taskDesc.td_stackSize, taskDesc.td_stackMargin, taskDesc.td_errorStatus );
            }
        }
        fprintf( hFilestream, "\n\n" );
        fclose( hFilestream );								
    }
#endif    

	//send the system start msg to agent
	McuAgentQuit(FALSE);
	
	OspDelay( 1000 );
	
	OspQuit();
	
	OspDelay( 1000 );

    printf("Quiting MCU...\n");

	#ifndef WIN32
	BrdHwReset();
	#endif

	return;
}

void CMcuGuardInst::ProcGuardTest(void)
{
	return;
}

/*====================================================================
    ������      :GuardGetBrdNetSyncLock
    ����        ����ȡ���ݰ�ͬ��״��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2003-7-4				JQL
====================================================================*/
u8 CMcuGuardInst::GuardGetBrdNetSyncLock()
{
	return SYNCSOURCE_NORMAL;
}

/*====================================================================
    ������      : PingEveryTask
    ����        ����MCU��ÿһ��Ӧ�÷��Ͳ��԰�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2003-7-4				JQL
====================================================================*/
void CMcuGuardInst::PingEveryTask()
{
	//IncompletePro: ��������ͬ����ɸ�App����ʱ�ݲ������⴦��

	//����֮ǰ����ʼ��״̬����
	m_byTaskState = 0;
	m_dwGuardTestSeriNo++;

	//�������ݰ�
	post( MAKEIID( AID_MCU_MCSSN, CInstance::DAEMON ), MCU_APPTASKTEST_REQ, 
		  (u8 *)&m_dwGuardTestSeriNo, sizeof(m_dwGuardTestSeriNo) );
	post( MAKEIID( AID_MCU_PERIEQPSSN, CInstance::DAEMON ), MCU_APPTASKTEST_REQ, 
		  (u8 *)&m_dwGuardTestSeriNo, sizeof(m_dwGuardTestSeriNo) );
	post( MAKEIID( AID_MCU_VC, CInstance::DAEMON ), MCU_APPTASKTEST_REQ, 
		  (u8 *)&m_dwGuardTestSeriNo, sizeof(m_dwGuardTestSeriNo) );
	post( MAKEIID( AID_MCU_CONFIG, CInstance::DAEMON ), MCU_APPTASKTEST_REQ, 
		  (u8 *)&m_dwGuardTestSeriNo, sizeof(m_dwGuardTestSeriNo) );
	post( MAKEIID( AID_MCU_MPSSN, CInstance::DAEMON ), MCU_APPTASKTEST_REQ, 
		  (u8 *)&m_dwGuardTestSeriNo, sizeof(m_dwGuardTestSeriNo) );
	post( MAKEIID( AID_MCU_MTADPSSN, CInstance::DAEMON ), MCU_APPTASKTEST_REQ, 
		  (u8 *)&m_dwGuardTestSeriNo, sizeof(m_dwGuardTestSeriNo) );
    post( MAKEIID( AID_MCU_DCSSSN, CInstance::DAEMON ), MCU_APPTASKTEST_REQ, 
        (u8 *)&m_dwGuardTestSeriNo, sizeof(m_dwGuardTestSeriNo) );

	return;
}

/*====================================================================
    ������      : ProcEachTaskCheck
    ����        �����ÿһ������״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 AppId Ӧ�úţ�u8 Shift ��TaskBusyTimes�е�λ��
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2003-7-17				JQL
====================================================================*/
void CMcuGuardInst::ProcEachTaskCheck( u8 byShift )
{
	u8 byAppId;
	
	//����ƫ�����õ�Ӧ�úţ�Ӧ����
	switch ( byShift ) 
	{
	case SHIFT_MCSSN_CHECK:
		byAppId = AID_MCU_MCSSN;
		strncpy( achAppName, "McSsn", sizeof( achAppName ) );
		achAppName[ sizeof( achAppName ) - 1 ] = 0;
		break;
	case SHIFT_PERIEQPSSN_CHECK:
		byAppId = AID_MCU_PERIEQPSSN;
		strncpy( achAppName, "EqpSsn", sizeof( achAppName ) );
		achAppName[ sizeof( achAppName ) - 1 ] = 0;
		break;
	case SHIFT_MCUVC_CHECK:
		byAppId = AID_MCU_VC;
		strncpy( achAppName, "McuVc", sizeof( achAppName ) );
		achAppName[ sizeof( achAppName ) - 1 ] = 0;
		break;
	case SHIFT_MCUCONFIG_CHECK:
		byAppId = AID_MCU_CONFIG;
		strncpy( achAppName, "McuConfig", sizeof( achAppName ) );
		achAppName[ sizeof( achAppName ) - 1 ] = 0;
		break;
	case SHIFT_MPSSN_CHECK:
		byAppId = AID_MCU_MPSSN;
		strncpy( achAppName, "MpSsn", sizeof( achAppName ) );
		achAppName[ sizeof( achAppName ) - 1 ] = 0;
		break;
	case SHIFT_MTADPSSN_CHECK:
		byAppId = AID_MCU_MTADPSSN;
		strncpy( achAppName, "MtadpSsn", sizeof( achAppName ) );
		achAppName[ sizeof( achAppName ) - 1 ] = 0;
		break;
    case SHIFT_DCSSSN_CHECK:
        byAppId = AID_MCU_DCSSSN;
        strncpy( achAppName, "DcsSsn", sizeof( achAppName ) );
        achAppName[ sizeof( achAppName ) - 1 ] = 0;
        break;
	default:
		return;
	}
		
	if ( 0 == (m_byTaskState & ((u8)1 << byShift)) )
	{
		OspPrintf( TRUE, FALSE, "McuGuard: The %s Task maybe is busy or dead!\n", achAppName );
		
		//��������Ӧ��û�з���Ӧ��������澯
		if (TIMESPACE_MAX_FAILED_TIMES == m_abyTaskBusyTimes[byShift])
		{
			CTaskStatus taskStatus;
			taskStatus.appid = byAppId;
			taskStatus.status = TASK_BUSY;
			post( MAKEIID( AID_MCU_AGENT, 1 ), SVC_AGT_TASK_STATUS, &taskStatus, sizeof( CTaskStatus ) );
		
			//����MCU
#ifndef _DEBUG
			m_bRebootMcu = TRUE;
			OspPrintf( TRUE, FALSE, "CMcuGuardInst: Task %s has no response, The System will Reboot.\n", achAppName );			

            //  xsl [6/16/2006] �����ϲ�ι����ʽ����������У���Ϊ�ײ㳬ʱʱ��Ϊ1.6�룬����vx����ʱ����Ӳ��ι���ķ�ʽ
            if (g_bEnableMcuGuard)
            {
                ProcPowerOff();
            }            
#endif		
		}
		else
		{
			m_abyTaskBusyTimes[byShift]++;
		}
	}
	else
	{
		//�������״̬������������ͻָ��澯
		if (TIMESPACE_MAX_FAILED_TIMES == m_abyTaskBusyTimes[byShift])
		{
			CTaskStatus taskStatus;
			taskStatus.appid = byAppId;
			taskStatus.status = TASK_NORMAL;
			post( MAKEIID( AID_MCU_AGENT, 1 ), SVC_AGT_TASK_STATUS, &taskStatus, sizeof( CTaskStatus ) );
		}
		m_abyTaskBusyTimes[byShift] = 0;
	}

	return;
}

/*====================================================================
    ������      : ProcTimerFeedDog
    ����        ��ι����ʱ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2003-10-22				JQL
====================================================================*/
void CMcuGuardInst::ProcTimerFeedDog()
{
#ifdef _LINUX_
    GuardLog("BrdQueryNipState will be calling.\n");
    if (OK != BrdQueryNipState())
    {
        m_bRebootMcu = TRUE;
              
        // xsl [11/1/2006] д����־�ļ�
        s8 achInfo[255];
        time_t tiCurTime = ::time(NULL);             
        s32 nLen = sprintf(achInfo, "\nSytem time %s\n%s\n\n", 
                            ctime(&tiCurTime), "BrdQueryNipState is failed, system will stop feed dog.");
        writeexclog(achInfo, nLen);
        OspPrintf(TRUE, FALSE, achInfo);
    }
#endif

	//�����������MCU������ι��
	if (!m_bRebootMcu)
	{
#if defined(_LINUX_)
        // ����Linuxϵͳ feed
		SysNoticeWdGuard();
        SetTimer( MCUGD_FEEDDOG_TIMER, TIMESPACE_FEEDDOG );

        GuardLog("[ProcTimerFeedDog] BrdMpcFeedDog() and settimer, curTicks:%d\n", OspTickGet());
#elif defined(_VXWORKS_)
        // ��ʱ����������Ӳ��ι��
#endif	                
	}

    return;
}

/*=============================================================================
�� �� ���� ProcGetLicenseDataFromFile
��    �ܣ� ����license key
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage *pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/10/20  4.0			������                  ����
2007/01/23  4.0			�ܹ��				  ���Ӷ��°汾License�Ķ�д���ܣ�
											  ͬʱ���ֶԾɰ汾��֧��
=============================================================================*/
void CMcuGuardInst::ProcGetLicenseDataFromFile( const CMessage *pcMsg )
{
//    u8 achEncText[LEN_KEYFILE_CONT] = {0};
	
    // guzh [4/25/2007] ���ȼ���Ʒ����
    if ( !g_cMcuAgent.IsMcuPdtBrdMatch( GetMcuPdtType() ) )
    {
        printf("[Mcu License] Illegale Running Board!\n");
        g_cMcuVcApp.SetLicenseNum( 0 );
        g_cMcuGuardApp.SetLicenseErrorCode( 9 );
		return;        
    }

    s8 achKeyFileName[KDV_MAX_PATH] = {0};
    sprintf(achKeyFileName, "%s/%s", DIR_DATA, KEY_FILENAME);

	//��ɰ汾����, zgc, 2007/01/23
	FILE *pHandler = fopen( achKeyFileName, "rb" );
	if( NULL == pHandler )
	{
		printf("[Mcu License] fail to open key file.\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
        g_cMcuGuardApp.SetLicenseErrorCode( 1 );
		return;
	}    
	
	fseek(pHandler, 0, SEEK_SET);

	s8 achcopyright[128];
	memset( achcopyright, 0 ,sizeof(achcopyright) );
	
	fread( achcopyright, strlen(KEDA_COPYRIGHT), 1, pHandler );
	fclose( pHandler );
	if( 0 == memcmp( achcopyright, KEDA_COPYRIGHT, strlen(KEDA_COPYRIGHT) ) )
	{
		//���ɰ汾License, zgc, 2007/01/23
		GetOldLicenseDataFromFile( achKeyFileName );
	}
	else
	{
		//���°汾License, zgc, 2007/01/23
		GetCurrentLicenseDataFromFile( achKeyFileName );
	}
	
	return;
}

void CMcuGuardInst::GetBoardSequenceNum( u8 *pBuf, s8* achSep )
{
#ifndef WIN32
	TBrdEthParam tBrdEthParam;
	u8 byEthId = g_cMcuAgent.GetInterface(); // ǰ������ѡ��

    u8 byRet = BrdGetEthParam( byEthId, &tBrdEthParam );
	
	s8 achMacBuf[64];
	memset( achMacBuf, 0, sizeof(achMacBuf) );
    s8 szTmp[3] = {0};
	u8 byLp = 0;
	while(byLp < 6)
	{
        sprintf(szTmp, "%.2X", tBrdEthParam.byMacAdrs[byLp] );
        strcat( achMacBuf, szTmp);
        if (byLp < 5)
        {
            // guzh [7/3/2007] �Զ���ָ��
            strcat(achMacBuf, achSep);
        }
        byLp ++;
	}
	
	memcpy( pBuf, achMacBuf, strlen(achMacBuf) );
#endif

	return;
}

void CMcuGuardInst::GenEncodeData(u8 *pOutBuf, u16 wInBufLen, u8 *pInBuf, BOOL32 bDirect)
{
    s8 achIV[MAX_IV_SIZE] = {0};
	
    s8 achKey[32] = {0};
	//strncpy( achKey, KEDA_AES_KEY, sizeof(KEDA_AES_KEY) );// ����keyΪ16�ı���
	strncpy( achKey, KEDA_AES_KEY, strlen(KEDA_AES_KEY)+1 );// ����keyΪ16�ı���
    u16 wKeyLen = strlen(achKey);

    s32 nRet;
	if( bDirect ) // eccrypt 
	{
		nRet = KdvAES( achKey, wKeyLen, MODE_CBC, DIR_ENCRYPT, achIV,
					   pInBuf, wInBufLen, pOutBuf );
	}
	else  // decrypt
	{
		nRet = KdvAES( achKey, wKeyLen, MODE_CBC, DIR_DECRYPT, achIV,
					   pInBuf, wInBufLen, pOutBuf );
	}

	return;
}

/*=============================================================================
�� �� ���� GetOldLicenseDataFromFile
��    �ܣ� ���ؾɰ汾license key
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� s8* pchPath
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/01/23  4.0			�ܹ��                 ����
=============================================================================*/
BOOL32 CMcuGuardInst::GetOldLicenseDataFromFile( s8* pchPath )
{
	u8 achEncText[LEN_KEYFILE_CONT] = {0};

	FILE *pHandler = fopen( pchPath, "rb" );
	if( NULL == pHandler )
	{
		printf("[Mcu License] fail to open key file.\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
        g_cMcuGuardApp.SetLicenseErrorCode( 1 );
		return FALSE;
	}
	
	fseek(pHandler, 0, SEEK_SET);

	s8 achcopyright[128];
	memset( achcopyright, 0 ,sizeof(achcopyright) );
	
	fread( achcopyright, strlen(KEDA_COPYRIGHT), 1, pHandler );
	
	u16 wEncTextLen = 0;
	fread( &wEncTextLen, sizeof(wEncTextLen), 1, pHandler );    // ���������Ӧ���� LEN_KEYFILE_CONT
    wEncTextLen = ntohs(wEncTextLen);
    
	if (wEncTextLen > LEN_KEYFILE_CONT)
    {
        // �������LEN_KEYFILE_CONT�����Ժ��������
        wEncTextLen = LEN_KEYFILE_CONT;
    }

	fread( achEncText, wEncTextLen, 1, pHandler );

	fclose( pHandler );
	pHandler = NULL;
    
    u8 achDecText[LEN_KEYFILE_CONT] = {0};

	GenEncodeData( achDecText, wEncTextLen, achEncText, FALSE );

    u8 achSequenceStr[64] = {0};
	GetBoardSequenceNum( achSequenceStr, ":" );  //�����ID
	
    // 1:�û���,������
	s8  achSeps[] = "|"; 

	s8 *pchToken = strtok( (s8*)achDecText, achSeps );
	if( NULL == pchToken )
	{
		printf("Error key file, Get UserID Failed!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		return FALSE;
	}

    // 2: DeviceID
    pchToken = strtok( NULL, achSeps );
	if( NULL == pchToken )
	{
		printf("Device id null!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 5 );
		return FALSE;
	}
    
	u16 wSequeLen = strlen((s8*)achSequenceStr);
	StrUpper( (s8*)achSequenceStr );
	StrUpper( pchToken );
	if( 0 != strcmp( pchToken, (s8*)achSequenceStr ) )
	{
		printf("Error key file for this board, bad Device ID!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 5 );
		return FALSE;
	}

    // 3, ��Ȩ����
	pchToken = strtok( NULL, achSeps );
	if( NULL == pchToken )
	{
		printf("Error key file, Get License Failed!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 7 );
		return FALSE;
	}

	g_cMcuVcApp.SetLicenseNum( atoi(pchToken) );        
    // ���ܴ�������ֵ
    if (g_cMcuVcApp.GetLicenseNum() > MAXNUM_MCU_MT)
    {
        g_cMcuVcApp.SetLicenseNum(MAXNUM_MCU_MT);
    }

    // 4: Expire Date
    pchToken = strtok( NULL, achSeps );
    if ( NULL != pchToken )
    {
		TKdvTime tDate;
        tDate.SetYear(atoi(pchToken));        
        pchToken = strtok( NULL, achSeps );
        if ( NULL != pchToken )
        {
            tDate.SetMonth(atoi(pchToken));
            pchToken = strtok( NULL, achSeps );
            if (NULL != pchToken)
            {
                tDate.SetDay(atoi(pchToken));
            }            
        } 
        // guzh [5/8/2007] ������ó���2030�꣬��Ϊ������
        if ( tDate.GetYear() >= 2030 )
        {
            memset( &tDate, 0, sizeof(tDate) );
        }
        g_cMcuVcApp.SetMcuExpireDate(tDate);
    }

	return TRUE;
}

/*=============================================================================
�� �� ���� GetCurrentLicenseDataFromFile
��    �ܣ� ���ص�ǰ�汾license key
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� s8* pchPath
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/01/23  4.0			�ܹ��                 ����
=============================================================================*/
BOOL32 CMcuGuardInst::GetCurrentLicenseDataFromFile( s8* pchPath )
{
#ifndef WIN32    
	u16 wRet = 0; 
	wRet = ReadLicenseFile( pchPath );
	if(SUCCESS_LICENSE != wRet) 
	{
		printf("Read License file failed! wRet = %d!\n", wRet);
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 1 );
		return FALSE;
	}
	
	s8 achGetValue[50];
	s32 nValuelen = 50;

	//�����֤�汾��
	nValuelen = 50;
	memset( achGetValue, 0, sizeof(achGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_LICENSE_VERSION, achGetValue, nValuelen) ) )
	{
		printf("Read license version failed!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 2 ); 
		return FALSE;
	}
    // License �汾�����������ֲ�ͬ�汾��License��������У��
    /*
	if( 0 != memcmp( LICENSE_VERSION, achGetValue, strlen(LICENSE_VERSION) ) )
	{
		printf("License version is not correct!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 2 );
		return FALSE;
	}
	//printf("License version: %s\n",achGetValue);
    */

	//�����֤����
	nValuelen = 50;
	memset( achGetValue, 0, sizeof(achGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_LICENSE_TYPE, achGetValue, nValuelen) ) )
	{
		printf("Read license type failed!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 3 );
		return FALSE;
	}
	if( 0 != memcmp( LICENSE_TYPE, achGetValue, strlen(LICENSE_TYPE) ) )
	{
		printf("License type is not correct!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 3 );
		return FALSE;
	}
	//printf("License type: %s\n",achGetValue);
	
	//��License Key
	nValuelen = 50;
	memset( achGetValue, 0, sizeof(achGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue( (s8*)KEY_LICENSE_KEY, achGetValue, nValuelen ) ) )
	{
		printf("Read LicenseKey failed");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 4);
		return FALSE;
	}
    g_cMcuGuardApp.SetLicenseSN(achGetValue);
    
	//printf("License key: %s\n",achGetValue);

	//���豸ID
    nValuelen = 50;
	memset( achGetValue, 0, sizeof(achGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MCU_DEVICE_ID, achGetValue, nValuelen) ) )
	{
		printf("Read device id failed!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 5 );
		return FALSE;
	}
    StrUpper( achGetValue );

    // guzh [7/3/2007] �ֱ���2�ַָ���
    u8 achSequenceStr[64] = {0};    
	GetBoardSequenceNum( achSequenceStr, ":" );
	StrUpper( (s8*)achSequenceStr );	
	if( 0 != strcmp( (s8*)achSequenceStr, achGetValue ) )
	{
        memset(achSequenceStr, 0, sizeof(achSequenceStr));
        GetBoardSequenceNum( achSequenceStr, "-" ); 
        StrUpper( (s8*)achSequenceStr );	
        if( 0 != strcmp( (s8*)achSequenceStr, achGetValue ) )
	    {
		    printf("Device id is incorrect for this board!\n");
		    g_cMcuVcApp.SetLicenseNum( 0 );
		    g_cMcuGuardApp.SetLicenseErrorCode( 5 );
		    return FALSE;
        }
	}
	//printf("Device id: %s\n",achGetValue);

	//��MCU����
	nValuelen = 50;
	memset( achGetValue, 0, sizeof(achGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MCU_TYPE, achGetValue, nValuelen) ) )
	{
		printf("Read Mcu type failed!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 6 );
		return FALSE;
	}
	else
	{
		// ��MCU���ͽ���У��, zgc, 2007-03-09
		u8 byMcuType = GetMcuPdtType();
		switch( byMcuType ) {
		case MCU_TYPE_KDV8000B:
		case MCU_TYPE_KDV8000B_HD:
			if( 0 != strcmp( "8000B", achGetValue ) )
			{
				printf( "Mcu type(%s) is error!\n", achGetValue);
				g_cMcuVcApp.SetLicenseNum( 0 );
                g_cMcuGuardApp.SetLicenseErrorCode( 6 );
				return FALSE;
			}
			break;
		case MCU_TYPE_KDV8000C:
			if( 0 != strcmp( "8000C", achGetValue ) )
			{
				printf( "Mcu type(%s) is error!\n", achGetValue);
				g_cMcuVcApp.SetLicenseNum( 0 );
                g_cMcuGuardApp.SetLicenseErrorCode( 6 );
				return FALSE;
			}
			break;
        case MCU_TYPE_KDV8000:
            if( 0 != strcmp( "8000", achGetValue ) )
            {
                printf( "Mcu type(%s) is error!\n", achGetValue);
                g_cMcuVcApp.SetLicenseNum( 0 );
                g_cMcuGuardApp.SetLicenseErrorCode( 6 );
                return FALSE;
            }
            break;
		case MCU_TYPE_WIN32:
			break;
		default:
			printf( "Mcu type(%s) is error!\n", achGetValue);
            g_cMcuGuardApp.SetLicenseErrorCode( 6 );
			g_cMcuVcApp.SetLicenseNum( 0 );
			return FALSE;
			break;
		}
	} 
	
	//����Ȩ��������
	nValuelen = 50;
	memset( achGetValue, 0, sizeof(achGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MCU_ACCESS_NUM, achGetValue, nValuelen) ) )
	{
		printf("Read Mcu access num failed!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 7 );
		return FALSE;
	}
	g_cMcuVcApp.SetLicenseNum( atoi(achGetValue) );        
    // ���ܴ�������ֵ
    if (g_cMcuVcApp.GetLicenseNum() > MAXNUM_MCU_MT)
    {
        g_cMcuVcApp.SetLicenseNum(MAXNUM_MCU_MT);
    }
	//printf("Access num: %s\n",achGetValue);
	
	//��ʧЧ����
	nValuelen = 50;
	memset( achGetValue, 0, sizeof(achGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MCU_EXP_DATE, achGetValue, nValuelen) ) )
	{
		printf("Read Mcu expire time failed!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 8 );
		return FALSE;
	}
	TKdvTime tDate;
	s8 * pchToken = strtok( achGetValue, "-" );
	if( NULL != pchToken)
	{
		tDate.SetYear( atoi( pchToken ) );
		pchToken = strtok( NULL, "-" );
		if( NULL != pchToken )
		{
			tDate.SetMonth( atoi( pchToken ) );
			pchToken = strtok( NULL, "-" );
			if( NULL != pchToken )
			{
				tDate.SetDay( atoi( pchToken ) );
			}
		}
	}
	if( 0 == tDate.GetYear() || 0 == tDate.GetMonth() || 0 == tDate.GetDay() )
	{
		printf("Read Mcu expire time failed!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 8 );
		return FALSE;
	}
    // guzh [5/8/2007] ������ó���2030�꣬��Ϊ������
    if ( tDate.GetYear() >= 2030 )
    {
        memset( &tDate, 0, sizeof(tDate) );
    }
	g_cMcuVcApp.SetMcuExpireDate(tDate);

	//��VCS��Ȩ��������
	nValuelen = 50;
	memset( achGetValue, 0, sizeof(achGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MCU_VCS_ACCESS_NUM, achGetValue, nValuelen) ) )
	{
		printf("Read vcs access num failed!\n");
		g_cMcuVcApp.SetVCSAccessNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 10 );
		return FALSE;
	}
	g_cMcuVcApp.SetVCSAccessNum(atoi(achGetValue));        
    // ���ܴ������ֵ
    if (g_cMcuVcApp.GetVCSAccessNum() > MAXNUM_MCU_VC)
    {
        g_cMcuVcApp.SetVCSAccessNum(MAXNUM_MCU_VC);
    }
#endif
	return TRUE;
}

CMcuGuardData::CMcuGuardData()
{
	m_byLicenseErrorCode = 0;
    memset(m_szLicenseSN, 0, sizeof(m_szLicenseSN));
}
CMcuGuardData::~CMcuGuardData()
{
}
/*=============================================================================
�� �� ���� GetLicenseErrorCode
��    �ܣ� ��ô�����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� ��
�� �� ֵ�� u8 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/01/26  4.0			�ܹ��                 ����
=============================================================================*/
u8 CMcuGuardData::GetLicenseErrorCode()
{
	return m_byLicenseErrorCode;
}

/*=============================================================================
�� �� ���� SetLicenseErrorCode
��    �ܣ� ���ô�����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8
�� �� ֵ�� �� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/01/26  4.0			�ܹ��                 ����
=============================================================================*/
void CMcuGuardData::SetLicenseErrorCode( u8 byErrorCode )
{
	m_byLicenseErrorCode = byErrorCode;
}

/*=============================================================================
�� �� ���� SetLicenseSN
��    �ܣ� ����License���к�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8
�� �� ֵ�� �� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/06/08  4.0			����                 ����
=============================================================================*/
void CMcuGuardData::SetLicenseSN(const s8* szSN)
{
    strncpy(m_szLicenseSN, szSN, sizeof(m_szLicenseSN)-1);
}

/*=============================================================================
�� �� ���� GetLicenseSN
��    �ܣ� ��ȡLicense���к�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8
�� �� ֵ�� �� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/06/08  4.0			����                 ����
=============================================================================*/
const s8* CMcuGuardData::GetLicenseSN(void)
{
    return m_szLicenseSN;
}


API void mcuguarddisable(void)
{
    g_bEnableMcuGuard = FALSE;
}

API void mcuguardenable(void)
{
    g_bEnableMcuGuard = TRUE;
}

API void showlicense(void)
{
    switch( g_cMcuGuardApp.GetLicenseErrorCode() ) 
	{
	case 0:
        {
            OspPrintf( TRUE, FALSE, "License SN(Key)       : %s\n", g_cMcuGuardApp.GetLicenseSN() );
            OspPrintf( TRUE, FALSE, "Licensed Access Number: %d\n", g_cMcuVcApp.GetLicenseNum() );
            OspPrintf( TRUE, FALSE, "VCS Access Number: %d\n", g_cMcuVcApp.GetVCSAccessNum() );

            TKdvTime tExpireDate = g_cMcuVcApp.GetMcuExpireDate(); 
            OspPrintf( TRUE, FALSE, "License Expired Date  : %.4d-%.2d-%.2d\n", 
                       tExpireDate.GetYear(), tExpireDate.GetMonth(), tExpireDate.GetDay() );

            break;
        }
	case 1:
		OspPrintf( TRUE, FALSE, "\nLicense Error: %s\n", READ_LICENSE_FILE_ERROR );
		break;
	case 2:
		OspPrintf( TRUE, FALSE, "\nLicense Error: %s\n", READ_LICENSE_VERSION_ERROR );
		break;
	case 3:
		OspPrintf( TRUE, FALSE, "\nLicense Error: %s\n", READ_LICENSE_TYPE_ERROR );
		break;
	case 4:
		OspPrintf( TRUE, FALSE, "\nLicense Error: %s\n", READ_LICENSE_KEY_ERROR );
		break;
	case 5:
		OspPrintf( TRUE, FALSE, "\nLicense Error: %s\n", READ_LICENSE_MCUDEVICEID_ERROR );
		break;
	case 6:
		OspPrintf( TRUE, FALSE, "\nLicense Error: %s\n", READ_LICENSE_MCUTYPE_ERROR );
		break;
	case 7:
		OspPrintf( TRUE, FALSE, "\nLicense Error: %s\n", READ_LICENSE_MCUACCESSNUM_ERROR );
		break;
	case 8:
		OspPrintf( TRUE, FALSE, "\nLicense Error: %s\n", READ_LICENSE_MCUEXPIREDATA_ERROR );
		break;
    case 9:
        OspPrintf( TRUE, FALSE, "\nBoard Error: %s\n",   READ_LICENSE_ILLEGALEBOARD_ERROR );
        break;
	case 10:
        OspPrintf( TRUE, FALSE, "\nLicense Error: %s\n", READ_LICENSE_VCSACCESSNUM_ERROR );
		break;		
	default:
		OspPrintf( TRUE, FALSE, "\nLicense Error: %s\n", READ_LICENSE_UNDEFINED_ERROR );
		break;
	}
}

// END OF FILE
