/*****************************************************************************
    ģ����      : EqpAgt
    �ļ���      : eqpagtsnmp.cpp
    ����ļ�    : 
    �ļ�ʵ�ֹ���: Snmp���ܵ���
    ����        : liaokang
    �汾        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    �޸ļ�¼:
    ��  ��      �汾        �޸���        �޸�����
    2012/06/18  4.7         liaokang      ����
******************************************************************************/
#include "eqpagtsnmp.h"
#include "eqpagtcfg.h"

CEqpAgtSnmp g_cEqpAgtSnmp;

// ����
CEqpAgtSnmp::CEqpAgtSnmp()
{
}

// ����
CEqpAgtSnmp::~CEqpAgtSnmp()
{
}

/*====================================================================
    ������      : EqpAgtSendTrap
    ����        : ����Trap
    �㷨ʵ��    :
    ����ȫ�ֱ���: g_cEqpAgtCfg
    �������˵��: CNodes& cNodes Trap�ڵ���Ϣ
    ����ֵ˵��  : u16
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
u16 CEqpAgtSnmp::EqpAgtSendTrap(CNodes& cNodes)
{  
    u16 wRet = SNMP_GENERAL_ERROR;
    TTarget tTrapRcvAddr;
    memset( &tTrapRcvAddr, 0, sizeof(tTrapRcvAddr) );

    if ( !g_cEqpAgtCfg.HasSnmpNms() )
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtSendTrap] no NMS!\n");
        return wRet;
    }

    // 1������Trap
    for( u8 byLoop = 0; byLoop < g_cEqpAgtCfg.GetTrapServerNum(); byLoop++ )
    {
        g_cEqpAgtCfg.GetTrapTarget( byLoop, tTrapRcvAddr );

        if( ( 0 != tTrapRcvAddr.dwIp ) && ( 0 != tTrapRcvAddr.wPort ) )
        {
            wRet = SendTrap(tTrapRcvAddr, cNodes);
        }
    }

    // 2���ͷ�
#ifdef _LINUX_

    if ( SNMP_SUCCESS != wRet )
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtSendTrap] send trap failed !\n");
    }
    else
    {
        EqpAgtLog( LOG_KEYSTATUS, "[EqpAgtSendTrap] send trap successed !\n");
        wRet = FreeNodes( cNodes );
        if ( SNMP_SUCCESS != wRet )
        {
            EqpAgtLog( LOG_ERROR, "[EqpAgtSendTrap] Free Nodes failed !\n");
        }
        else
        {
            EqpAgtLog( LOG_KEYSTATUS, "[EqpAgtSendTrap] Free Nodes successed !\n");
        }
    }

#endif
    
    return wRet;
}

// END OF FILE
