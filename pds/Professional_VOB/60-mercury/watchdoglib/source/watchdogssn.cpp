/*****************************************************************************
   ģ����      : WatchDog ʵ��ʵ��
   �ļ���      : watchdog.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: 

   ����        : ����
   �汾        : V4.0  Copyright(C) 2003-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2005/05/26  4.0         ����      ����
******************************************************************************/
#include "watchdogssn.h"
#include "watchdogdef.h"

#ifdef _DEBUG
#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWatchDogSsnInst::CWatchDogSsnInst()
{
    g_bWDLog = FALSE;
}

CWatchDogSsnInst::~CWatchDogSsnInst()
{

}

/*====================================================================
    ������      ��WDLog
    ����        ����ӡ��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2005/05/26  4.0         ����      ����
====================================================================*/
/*lint -save -e438 -e550*/
void WDLog( s8 * pszFmt, ... )
{
    if (g_bWDLog)
    {
	    s8 achPrintBuf[255];
        s32 nBufLen;
        va_list argptr;
	    s32 nLen = sprintf( achPrintBuf, "[WDClnt]:" );
        va_start( argptr, pszFmt );    
        nBufLen = vsprintf( achPrintBuf + nLen, pszFmt, argptr );   
        OspPrintf( nLen, FALSE, achPrintBuf );
        va_end(argptr);
    }
}
/*lint -restore*/

/*====================================================================
    ������      ��ProcSetModuleState
    ����        ��������ģ��״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2005/05/26  4.0         ����      ����
====================================================================*/
void CWatchDogSsnInst::ProcSetModuleState( )
{    
    if (m_dwServerNode != INVALID_NODE && 
        m_dwServerIID != INVALID_INS)
    {
        for (u8 byLoop = 0; byLoop < g_cWatchDogSsn.m_byModuleNum; byLoop ++)
        {
            ::OspPost( m_dwServerIID, EV_MODULESETSTATE, 
                (u8*)&g_cWatchDogSsn.m_atModuleParam[byLoop], 
                sizeof( IModuleParameter ), 
                m_dwServerNode, 
                MAKEIID(WD_CLT_APP_ID, 1));

            WDLog( "Sending Module State: %u, %u.\n", 
                g_cWatchDogSsn.m_atModuleParam[byLoop].emType, 
                g_cWatchDogSsn.m_atModuleParam[byLoop].emState );
        }
    }    
}

/*====================================================================
    ������      ��ProcConnectWDServer
    ����        ���������ӷ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2005/05/26  4.0         ����      ����
====================================================================*/
void CWatchDogSsnInst::ProcConnectWDServer()
{    
    if ( INVALID_NODE == m_dwServerNode )
    {
        u32 dwServerNode = ::OspConnectTcpNode(WD_SER_IP, WD_SER_TCP_PORT);
        
        if(INVALID_NODE != dwServerNode)
        {             
            m_dwServerNode = dwServerNode;
        }
        else
        {
            SetTimer( EV_CONNECTWDSERV_TIMER, TIMER_REG_DISC );
            return;
        }
    }

    ::OspNodeDiscCBReg( m_dwServerNode, GetAppID(), GetInsID() );	//ע���������ʵ��

    // ��������������Ϣÿ��������ע��һ��
    WDLog( "WD Server TCP Connected, Registing...\n");

    for (u8 byLoop = 0; byLoop < g_cWatchDogSsn.m_byModuleNum; byLoop ++)
    {
        ::OspPost( MAKEIID(WD_SER_APP_ID, 1 ), EV_MODULECONCMD, 
            (u8*)&g_cWatchDogSsn.m_atModuleParam[byLoop], 
            sizeof( IModuleParameter ), 
            m_dwServerNode, 
            MAKEIID(WD_CLT_APP_ID, 1));
    }
    
    // ע�ᳬʱ��ʱ��
    SetTimer( EV_REGWDSERV_TIMER, TIMER_REG_DISC );
}

/*------------------------------------------------------------------------
	 ������:   InstanceEntry
	 ��  ��:   ��Ϣ��ں��� 
	 ��  ��:   CMessage  * const pMsg           [in]���յ���Ϣ     
	 ����ֵ:   void
	  */
void CWatchDogSsnInst::InstanceEntry(CMessage *const pcMsg)
{
	u16 wCurEvent = pcMsg->event;
	switch(wCurEvent)//�жϴ�����¼�����
	{

    case OSP_POWERON:
    // ����
        {
            SetTimer( EV_CONNECTWDSERV_TIMER, TIMER_REG_POWERON );
        }
        
        break;

    case OSP_DISCONNECT:
    // ����
        {
            u32 dwNodeDisconnect = *(u32*)pcMsg->content;
            if ( INVALID_NODE != dwNodeDisconnect )
            {
                OspDisconnectTcpNode( dwNodeDisconnect );
            }
            SetTimer( EV_CONNECTWDSERV_TIMER, TIMER_REG_DISC );
        }
        break;

    // ���ӳ�ʱ        
    case EV_CONNECTWDSERV_TIMER:
        KillTimer( EV_CONNECTWDSERV_TIMER );
        ProcConnectWDServer();
        break; 

    // ע�ᳬʱ
    case EV_REGWDSERV_TIMER:
        KillTimer( EV_REGWDSERV_TIMER );
        if(INVALID_NODE != m_dwServerNode)
        {
            // �Ͽ�����
            OspDisconnectTcpNode ( m_dwServerNode );
        }
        break;

	// ����ȷ��
	case EV_MODULECONIND:  
        KillTimer( EV_REGWDSERV_TIMER );
        m_dwServerIID = pcMsg->srcid;
                
        WDLog( "Regist to WatchDog Server Ack!\n");
        
        // ���ñ�ģ��״̬
        ProcSetModuleState();
        break; 
	default:
        WDLog( "Unknown Event: %s(%u)!\n", OspEventDesc( wCurEvent ),  wCurEvent);
		break;
	}
}
