/*****************************************************************************
   ģ����      : Mcu Agent
   �ļ���      : AgentSnmp.cpp
   ����ļ�    : AgentSnmp.h
   �ļ�ʵ�ֹ���: Snmp���ܵ���
   ����        : 
   �汾        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2005/08/17  4.0         liuhuiyun     ����
******************************************************************************/
#include "agentsnmp.h"
#include "configureagent.h"

CAgentSnmp::CAgentSnmp()
{
}
CAgentSnmp::~CAgentSnmp()
{
}

/*=============================================================================
  �� �� ���� AgentSendTrap
  ��    �ܣ� ����Trap
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TTarget tTrapRcvAddr
             CNodes & cNodes
  �� �� ֵ�� u16 
=============================================================================*/
u16 CAgentSnmp::AgentSendTrap(CNodes & cNodes)
{
    u8      byLoop = 0;
    TTarget tTrapRcvAddr;
    memset( &tTrapRcvAddr, 0, sizeof(tTrapRcvAddr) );
    u16     wRet = SNMP_GENERAL_ERROR;

    for(; byLoop < g_cCfgParse.GetTrapServerNum(); byLoop++)
    {
        g_cCfgParse.GetTrapTarget(byLoop, tTrapRcvAddr);

        if(0 != tTrapRcvAddr.dwIp && 0 != tTrapRcvAddr.wPort)
        {
            wRet = SendTrap(tTrapRcvAddr, cNodes);
        }
    }
#ifdef _LINUX_
    if ( SNMP_SUCCESS == wRet )
    {
        wRet = FreeNodes( cNodes );
    }
    else
    {
        Agtlog(LOG_ERROR, "[AgentSendTrap] send trap failed !\n");
    }
#endif

    return wRet;
}

// END OF FILE
