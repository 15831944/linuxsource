/*****************************************************************************
ģ����      : mcuprs
�ļ���      : mcuprsguard.cpp
����ļ�    : mcuprsguard.h
�ļ�ʵ�ֹ���: mcuprs����Ӧ���ඨ��
����        : �ܹ��
�汾        : V4.0  Copyright(C) 2001-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
2008/04/07  4.0         �ܹ��        ����
******************************************************************************/
#include "mcuprsguard.h"
//#include "boardagent.h"
//#include "mcuver.h"

CMcuPrsGuardApp g_cMcuPrsGuardApp;
s32  g_nprsguardlog = 0;

BOOL32 CreatPrsDir(const s8* lpszDirPath);
/*=============================================================================
  �� �� ���� prsgrdlog
  ��    �ܣ� ��ӡ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� char* fmt
             ...
  �� �� ֵ�� void 
=============================================================================*/
/*lint -save -e438 -e530 -e1055*/
void prsgrdlog( s8* pszFmt, ...)
{
    s8 achPrintBuf[255];
    s32  nBufLen = 0;
    va_list argptr;
    if( g_nprsguardlog == 1 )
    {		  
        nBufLen = sprintf( achPrintBuf, "[PrsGuard]: " ); 
        va_start( argptr, pszFmt );
        nBufLen += vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
        va_end(argptr); 
        OspPrintf( TRUE, FALSE, achPrintBuf ); 
    }
}
/*lint -restore*/

/*=============================================================================
�� �� ���� InstanceEntry
��    �ܣ� ��Ϣ���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage *const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/4/7   4.0		�ܹ��                  ����
=============================================================================*/
void CMcuPrsGuard::InstanceEntry( CMessage *const pcMsg )
{	
	if (pcMsg == NULL)
	{
		OspPrintf( TRUE, FALSE ,"[MCU Prs Guard Recv] NULL message received.\n");
		return ;
	}
	
    prsgrdlog( "Message %u(%s).\n", pcMsg->event ,::OspEventDesc( pcMsg->event ));
	
	switch (pcMsg->event)
    {
	case OSP_POWERON:
		ProcGuardPowerOn(pcMsg);
		break;
	case EV_PRSGUARD_MAINTASKSCAN_TIMER:
		ProcMainTaskScanTimeOut(pcMsg);
		break;
	case EV_MAINTASKSCAN_ACK:
		ProcMainTaskScanAck(pcMsg);
		break;
		// δ�������Ϣ
	default:
		OspPrintf( TRUE, FALSE, "[PrsGuard]Unexcpet Message %u(%s).\n", pcMsg->event, ::OspEventDesc( pcMsg->event ));
		break;
	}
}

/*=============================================================================
�� �� ���� ProcGuardPowerOn
��    �ܣ� ������Ϣ��Ӧ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage *const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/4/7   4.0		�ܹ��                  ����
=============================================================================*/
void CMcuPrsGuard::ProcGuardPowerOn( CMessage *const pcMsg )
{
	switch( CurState() )
	{
	case STATE_IDLE:
	{
		NEXTSTATE( STATE_NORMAL );

		post( MAKEIID(AID_PRS, 1), EV_MAINTASKSCAN_REQ );
		g_cMcuPrsGuardApp.SetIsRecvScanAck(FALSE);
		SetTimer( EV_PRSGUARD_MAINTASKSCAN_TIMER, PRSGUARD_MAINTASKSCAN_TIMEOUT );

		break;
	}
	default:
		OspPrintf( TRUE, FALSE, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

	return;
}

/*=============================================================================
�� �� ���� ProcMainTaskScanTimeOut
��    �ܣ� ̽�����̶߳�ʱ����ʱ������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage *const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/4/7   4.0		�ܹ��                  ����
=============================================================================*/
void CMcuPrsGuard::ProcMainTaskScanTimeOut( CMessage *const pcMsg )
{
	SetTimer( EV_PRSGUARD_MAINTASKSCAN_TIMER, PRSGUARD_MAINTASKSCAN_TIMEOUT );
	post( MAKEIID(AID_PRS, 1), EV_MAINTASKSCAN_REQ );

	switch( CurState() )
	{
	case STATE_NORMAL:
	{
		if ( g_cMcuPrsGuardApp.IsRecvScanAck() )
		{		
			g_cMcuPrsGuardApp.SetIsRecvScanAck(FALSE);			
			post( MAKEIID(AID_PRS, 1), EV_MAINTASKSCAN_REQ );		
		}
		else
		{
			u8 byNoScanAckTimes = g_cMcuPrsGuardApp.GetNoScanAckTimes();
			if ( byNoScanAckTimes <= MAXNUM_MAINTASKSCAN_TIMEOUT )
			{
				g_cMcuPrsGuardApp.AddNoScanAckTimes();
				return;
			}			
			
			OspPrintf( TRUE, FALSE, "[PrsGuard] Main task dead or blocked!\n" );
			
			// ��¼���ļ�
			s8 asPrsLogFile[256] = {0};
			sprintf( asPrsLogFile, "%s/%s", DIR_LOG, FILE_PRSLOG );
			FILE *hLogFile = fopen(asPrsLogFile, "r+b");
			
			if ( NULL == hLogFile )
			{
				// ����log�ļ�
				prsgrdlog("prs.log not exist and create it\n");
				
				BOOL32 bRet = CreatPrsDir( DIR_LOG );
				if ( !bRet )
				{
					prsgrdlog("create dir<%s> failed for %s\n", DIR_LOG);     
					return;
				}
				hLogFile = fopen(asPrsLogFile, "w+b");
				if (NULL == hLogFile)
				{
					prsgrdlog("create prs.log failed for %s\n", strerror(errno));     
					return;
				}
			}
			
			// дlog�ļ�
			s8 achInfo[255] = {0};
			time_t tiCurTime = ::time(NULL);             
			s32 nLen = sprintf(achInfo, "\nSytem time %s\n[PrsGuard] Main task dead or blocked!\n", ctime(&tiCurTime));
			
			fseek(hLogFile, 0, SEEK_END);
			nLen = fwrite(achInfo, nLen, 1, hLogFile);
			fclose(hLogFile);
			if (0 == nLen)
			{
				prsgrdlog("write to prs.log failed\n");
				return;
			}			
		}
		break;
	}
	default:
		OspPrintf( TRUE, FALSE, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	
	return;
}

/*=============================================================================
�� �� ���� ProcMainTaskScanAck
��    �ܣ� ��Ӧ������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage *const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/4/7   4.0			�ܹ��                  ����
=============================================================================*/
void CMcuPrsGuard::ProcMainTaskScanAck( CMessage *const pcMsg )
{
	switch( CurState() )
	{
	case STATE_NORMAL:
	{
		g_cMcuPrsGuardApp.ResetNoScanAckTimes();
		g_cMcuPrsGuardApp.SetIsRecvScanAck( TRUE );
		break;
	}
	default:
		OspPrintf( TRUE, FALSE, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

//---------------------CMcuPrsGuardData-------------------------//

/*=============================================================================
�� �� ���� CMcuPrsGuardData
��    �ܣ� ���캯��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/4/7   4.0		�ܹ��                  ����
=============================================================================*/
CMcuPrsGuardData::CMcuPrsGuardData()
{
	m_byNoScanAckTimes = 0;
	m_byRecvScanAck = 0;
}

/*=============================================================================
�� �� ���� GetNoScanAckTimes
��    �ܣ� ���δ�յ�̽���Ӧ��Ϣ�Ĵ���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� u8 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/4/7   4.0		�ܹ��                  ����
=============================================================================*/
u8 CMcuPrsGuardData::GetNoScanAckTimes(void)
{
	return m_byNoScanAckTimes;
}

/*=============================================================================
�� �� ���� AddNoScanAckTimes
��    �ܣ� δ�յ�̽���Ӧ��Ϣ�Ĵ�����1
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/4/7   4.0		�ܹ��                  ����
=============================================================================*/
void CMcuPrsGuardData::AddNoScanAckTimes(void)
{
	if ( MAXNUM_MAINTASKSCAN_TIMEOUT >= m_byNoScanAckTimes )
	{
		m_byNoScanAckTimes++;
	}
}

/*=============================================================================
�� �� ���� ResetNoScanAckTimes
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/4/7   4.0		�ܹ��                  ����
=============================================================================*/
void CMcuPrsGuardData::ResetNoScanAckTimes(void)
{
	m_byNoScanAckTimes = 0;
}

/*==========================================================================
�� �� ���� IsRecvScanAck
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/4/7   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CMcuPrsGuardData::IsRecvScanAck(void)
{
	return ( 1 == m_byRecvScanAck );
}

/*=============================================================================
�� �� ���� SetIsRecvScanAck
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� BOOL32 bIsRecv
�� �� ֵ�� void  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/4/7   4.0			�ܹ��                  ����
=============================================================================*/
void  CMcuPrsGuardData::SetIsRecvScanAck(BOOL32 bIsRecv)
{
	m_byRecvScanAck = bIsRecv ? 1 : 0;
}


