/*****************************************************************************
    ģ����      : EqpAgt
    �ļ���      : eqpagtscan.cpp
    ����ļ�    : 
    �ļ�ʵ�ֹ���: ɨ���豸trap��Ϣ
    ����        : liaokang
    �汾        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    �޸ļ�¼:
    ��  ��      �汾        �޸���          �޸�����
    2012/06/18  4.7         liaokang      ����
******************************************************************************/
#include "eqpagtscan.h"
#include "eqpagtsnmp.h"
#include "eqpagtcommbase.h"

CEqpAgtScanApp	g_cEqpAgtScanApp;

// ���캯��
CEqpAgtScan::CEqpAgtScan()
{
    m_dwMaxTimerIdx = 0;
}

// ��������
CEqpAgtScan::~CEqpAgtScan()
{
}

/*====================================================================
    ������      ��InstanceEntry
    ����        ��ʵ����Ϣ������ں���������override
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
void CEqpAgtScan::InstanceEntry(CMessage* const pcMsg)
{
	if( NULL == pcMsg )
	{
        EqpAgtLog( LOG_ERROR, "[CEqpAgtGuard::InstanceEntry] Null point!\n" );
		return;
	}
    
	switch( pcMsg->event )
	{
	case EQPAGT_SCAN_POWERON:       // ������Ϣ
		ProcScanPowerOn();
		break;

    case EQPAGT_SCAN_TIMER:         // ���涨ʱ��
        ProcScanTimer();
		break;

	default:
        ProcTrapInfoScan( pcMsg );  // ��ҵ��ģ��ɨ�趨ʱ��
		break;
	}	
	return;
}

/*====================================================================
    ������      : ProcScanPowerOn
    ����        : ģ��������Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtScan::ProcScanPowerOn( void )
{
	SetTimer( EQPAGT_SCAN_TIMER, SCAN_STATE_TIME_OUT ); // �ȴ�ϵͳ��̬
	return;
}

/*====================================================================
    ������      : ScanAndSendTrap
    ����        : ɨ���ȡTrap��Ϣ������
    �㷨ʵ��    :
    ����ȫ�ֱ���: g_cEqpAgtSnmp
    �������˵��: CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtScan::ScanAndSendTrap( TEqpAgtTrapFunc pfTrapFunc )
{
    if ( NULL == pfTrapFunc  )
    {
        EqpAgtLog( LOG_DETAIL, "[ScanAndSendTrap] The input param is null!\n" );
        return;
    }

    u16 wRet = NO_TRAP;
    CNodes cNodes;
    cNodes.Clear();
    wRet = (*pfTrapFunc)( cNodes );
    if ( NO_TRAP == wRet  )
    {
        EqpAgtLog( LOG_DETAIL, "[ScanAndSendTrap] No Trap!\n" );
    }
    else// ��ȡ��Trap��Ϣ
    {
        wRet = g_cEqpAgtSnmp.EqpAgtSendTrap( cNodes );
        if ( SNMP_GENERAL_ERROR == wRet ) // ���ʹ���
        {                    
            EqpAgtLog( LOG_ERROR, "[ScanAndSendTrap] Send Trap failed!\n" );
        }
        else // ������ȷ
        {
            EqpAgtLog( LOG_DETAIL, "[ScanAndSendTrap] Send Trap Success!\n" );
        }
    }
    return;
}

/*====================================================================
    ������      : ProcScanTimer
    ����        : ���涨ʱ��
    �㷨ʵ��    :
    ����ȫ�ֱ���: g_cEqpAgtTrapList
    �������˵��: 
    ����ֵ˵��  : void
    ----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtScan::ProcScanTimer( void )
{
    u32 dwTimerId = EQPAGT_SCAN_TIMER;
    u16 wLoop = 0;
    u16 wTrapFuncNum = g_cEqpAgtTrapList.Size();
    u32 dwTrapScanTime = 0;
    TEqpAgtTrapEntry tEqpAgtTrapEntry;
    memset( &tEqpAgtTrapEntry, 0, sizeof(tEqpAgtTrapEntry));
    TEqpAgtTrapFunc pfTrapFunc = NULL;
    BOOL32 bRet = TRUE;
    
    // ��ҵ��ģ��Trap��Ϣ��ʱɨ��������
    if ( 0 == g_cEqpAgtTrapList.Size() )
    {
        SetTimer( EQPAGT_SCAN_TIMER, SCAN_STATE_TIME_OUT );
        return;
    }

    // ����ҵ��ģ��Trap��Ϣ��ʱɨ����û������
    if ( 0 == m_dwMaxTimerIdx )
    {
        // ��ѯ����Trap������Ϣ��������
        for( wLoop = 0; wLoop < wTrapFuncNum; wLoop++ )
        {
            bRet = g_cEqpAgtTrapList.GetEqpAgtTrapEntry( wLoop, &tEqpAgtTrapEntry );
            if ( bRet )
            {
                pfTrapFunc = tEqpAgtTrapEntry.pfFunc;
                // ɨ���ȡTrap��Ϣ������
                ScanAndSendTrap( pfTrapFunc );
                // ������ʱ��
                dwTimerId++;
                dwTrapScanTime = tEqpAgtTrapEntry.dwScanTimeSpan * 1000;
                SetTimer( dwTimerId, dwTrapScanTime );
            }
        }
        // ��¼
        m_dwMaxTimerIdx = dwTimerId;
        SetTimer( EQPAGT_SCAN_TIMER, SCAN_STATE_TIME_OUT );
        return;
    }

    if( ( m_dwMaxTimerIdx - EQPAGT_SCAN_TIMER ) < wTrapFuncNum )
    {      
        // ��ѯ����Trap������Ϣ
        dwTimerId = m_dwMaxTimerIdx;
        for( wLoop = (u16)(m_dwMaxTimerIdx - EQPAGT_SCAN_TIMER - 1); wLoop < wTrapFuncNum; wLoop++ )
        {
            bRet = g_cEqpAgtTrapList.GetEqpAgtTrapEntry( wLoop, &tEqpAgtTrapEntry );
            if ( bRet )
            {
                pfTrapFunc = tEqpAgtTrapEntry.pfFunc;
                // ɨ���ȡTrap��Ϣ������
                ScanAndSendTrap( pfTrapFunc );
                // ������ʱ��
                dwTimerId++;
                dwTrapScanTime = tEqpAgtTrapEntry.dwScanTimeSpan * 1000;
                SetTimer( dwTimerId, dwTrapScanTime );
            }
        }
        // ��¼
        m_dwMaxTimerIdx = dwTimerId;
        SetTimer( EQPAGT_SCAN_TIMER, SCAN_STATE_TIME_OUT );
        return;        
    }
}

/*====================================================================
    ������      : ProcTrapInfoScan
    ����        : ��ҵ��ģ��Trap��Ϣ��ʱɨ��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  : void
    ----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtScan::ProcTrapInfoScan( CMessage* const pcMsg )
{
    u32 dwTimerId = pcMsg->event;
    u16 wLoop = 0;
    u32 dwTrapScanTime = 0;
    TEqpAgtTrapEntry tEqpAgtTrapEntry;
    memset( &tEqpAgtTrapEntry, 0, sizeof(tEqpAgtTrapEntry));
    TEqpAgtTrapFunc pfTrapFunc = NULL;
    BOOL32 bRet = TRUE;

    // ĳ��Trap�����Ķ�ʱ��
    if ( ( EQPAGT_SCAN_TIMER < dwTimerId ) && ( dwTimerId <= m_dwMaxTimerIdx ) )
    {
        wLoop = (u16)( dwTimerId - EQPAGT_SCAN_TIMER ) - 1;
        bRet = g_cEqpAgtTrapList.GetEqpAgtTrapEntry( wLoop, &tEqpAgtTrapEntry );
        if ( bRet )
        {
            if ( tEqpAgtTrapEntry.bEnable )
            {
                EqpAgtLog( LOG_DETAIL, "[ProcTrapInfoScan] scaning node(0x%x) !\n", tEqpAgtTrapEntry.dwNodeValue );
                pfTrapFunc = tEqpAgtTrapEntry.pfFunc;
                // ɨ���ȡTrap��Ϣ������
                ScanAndSendTrap( pfTrapFunc );
            }
        }   
        // ������ʱ��
        dwTrapScanTime = tEqpAgtTrapEntry.dwScanTimeSpan * 1000;
        SetTimer( dwTimerId, dwTrapScanTime );
    }
    else
    {
        EqpAgtLog( LOG_ERROR, "[ProcTrapInfoScan] wrong message received !\n" );
    }
    return;
}

// END OF FILE
