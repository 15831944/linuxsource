/*****************************************************************************
   ģ����      : Mcu Agent
   �ļ���      : AgentSnmp.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: Snmp��
   ����        : 
   �汾        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2005/08/17  4.0         liuhuiyun     ����
******************************************************************************/
#ifndef _AGENT_SNMP_H
#define _AGENT_SNMP_H

#include "snmpadp.h"

class CAgentSnmp : public CAgentAdapter
{

public:
    CAgentSnmp();
    ~CAgentSnmp();

public :
    u16  AgentSendTrap(CNodes & cNodes);
};

#endif // _AGENT_SNMP_H

