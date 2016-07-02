/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : mmpguard.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: �����������ඨ��
   ����        : jianghy
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/08/25  1.0         jianghy       ����
   2004/11/11  3.5         �� ��         �½ӿڵ��޸�
******************************************************************************/
#ifndef MMPGUARD_H
#define MMPGUARD_H

#include "osp.h"
#include "kdvsys.h"
#include "mcuagtstruct.h"
#include "mmpagent.h"

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
#ifdef MMP
	TBrdMMPLedStateDesc m_tLedState;
#endif
#endif
};

typedef zTemplate< CBoardGuard, 1 > CBoardGuardApp;
extern CBoardGuardApp	g_cBrdGuardApp;	//���������Ӧ��ʵ��

#endif /* MMPGUARD_H */
