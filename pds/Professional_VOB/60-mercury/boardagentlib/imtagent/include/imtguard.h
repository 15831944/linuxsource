/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : imtguard.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: �����������ඨ��
   ����        : jianghy
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/08/25  1.0         jianghy       ����
   2004/12/05  3.5         �� ��        �½ӿڵ��޸�
******************************************************************************/
#ifndef IMTGUARD_H
#define IMTGUARD_H

#include "osp.h"
#include "kdvsys.h"
#include "mcuagtstruct.h"
#include "imtagent.h"




class CBoardGuard : public CInstance  
{
public:
	CBoardGuard();
	~CBoardGuard();

	
protected:
	//��Ϣ��ں�������������
	void InstanceEntry( CMessage * const pcMsg );

	//GUARDģ��������Ϣ
	void ProcGuardPowerOn(CMessage* const pcMsg);
	//����״̬��ʱɨ��
	void ProcGuardStateScan(CMessage* const pcMsg);

private:
#ifndef WIN32
#ifdef IMT
	TBrdIMTLedStateDesc m_tLedState;
	TBrdIMTAlarmAll		m_tImtBrdAlarm;
#endif
#endif
};

typedef zTemplate< CBoardGuard, 1 > CBoardGuardApp;
extern CBoardGuardApp	g_cBrdGuardApp;	//���������Ӧ��ʵ��

#endif /* IMTGUARD_H */