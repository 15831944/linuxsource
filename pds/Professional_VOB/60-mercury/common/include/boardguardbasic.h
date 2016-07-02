/*****************************************************************************
   ģ����      : Board Agent Basic
   �ļ���      : boardguardbasic.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: �����������ඨ��
   ����        : �ܹ��
   �汾        : V4.0  Copyright(C) 2001-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2007/08/20  4.0         �ܹ��       ����
******************************************************************************/
#ifndef BOARDGUARDBASIC_H
#define BOARDGUARDBASIC_H

#include "osp.h"
#include "mcuagtstruct.h"
#include "boardagentbasic.h"

class CBBoardGuard : public CInstance  
{
public:
	CBBoardGuard();
	~CBBoardGuard();
	
protected:
	//��Ϣ��ں�������������
	void InstanceEntry( CMessage * const pcMsg );
	//GUARDģ��������Ϣ
	void ProcGuardPowerOn(CMessage* const pcMsg);
	//����״̬��ʱɨ��
	void ProcGuardStateScan(CMessage* const pcMsg);

private:
	TBrdAlarmState   m_tBrdAlarmState;
	TBrdLedState	 m_tBrdLedState;
#ifdef _LINUX12_  // Ŀǰ��brdwrapper.h����TBrdEthInfo
    /* �������赥����˵,IS2.2 8313��ȡ�ڲ�ͨ�����ڣ������ڣ���Ϣ�������ͨ��������ȡǰ������Ϣ */
    TBrdEthInfo      m_tBrdFrontEthPortState; // ǰ����״̬
#endif
    u8 m_byBrdTempStatus;			//�����¶��Ƿ���ߣ�����Ϊ1������Ϊ0 [10/25/2011 chendaiwei]
	u8 m_byBrdCpuStatus;			//����CPUռ�����������Ϊ1������Ϊ0 [10/25/2011 chendaiwei]
	u8 m_byBrdId;					//��������(BRD_TYPE_HDU��) [12/1/2011 chendaiwei]
	
};

typedef zTemplate< CBBoardGuard, 1 > CBoardGuardApp;
extern CBoardGuardApp	g_cBrdGuardApp;	//���������Ӧ��ʵ��

#endif    /* CRIGUARD_H */
