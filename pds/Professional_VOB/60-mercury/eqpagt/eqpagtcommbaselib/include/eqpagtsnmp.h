/*****************************************************************************
    ģ����      : EqpAgt
    �ļ���      : eqpagtsnmp.h
    ����ļ�    : 
    �ļ�ʵ�ֹ���: Snmp���ܵ���
    ����        : liaokang
    �汾        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    �޸ļ�¼:
    ��  ��      �汾        �޸���        �޸�����
    2012/06/18  4.7         liaokang      ����
******************************************************************************/
#ifndef _EQPAGT_SNMP_H_
#define _EQPAGT_SNMP_H_

#include "eqpagtutility.h"

class CEqpAgtSnmp : public CAgentAdapter
{

public:
    CEqpAgtSnmp();
    ~CEqpAgtSnmp();

public :
    u16  EqpAgtSendTrap(CNodes& cNodes);
};

extern CEqpAgtSnmp g_cEqpAgtSnmp;

#endif // _EQPAGT_SNMP_H_

