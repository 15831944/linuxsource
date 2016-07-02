/*****************************************************************************
ģ����      : prs
�ļ���      : mcuprsguard.h
����ļ�    : mcuprsguard.cpp
�ļ�ʵ�ֹ���: prs����Ӧ����ͷ�ļ�
����        : �ܹ��
�汾        : V4.0  Copyright(C) 2001-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
2008/04/07  4.0         �ܹ��        ����
******************************************************************************/

#ifndef __PRSGUARD_H
#define __PRSGUARD_H

#include "osp.h"
#include "evmcueqp.h"
#include "mcustruct.h"
#include "mcuconst.h"
#include "mcuprs.h"


#define PRSGUARD_MAINTASKSCAN_TIMEOUT			(u16)(10*1000)		// prs guard��ʱ̽��prs���̵߳�ʱ����
#define MAXNUM_MAINTASKSCAN_TIMEOUT				(u8)3				// δ�յ���ȷ��Ӧ��������

class CMcuPrsGuard : public CInstance
{
	enum //ʵ��״̬
	{
		STATE_IDLE   = 0,
		STATE_NORMAL = 1,
	};
public:
	CMcuPrsGuard(){};
	~CMcuPrsGuard(){};

private:
	//-------------��Ϣ��Ӧ----------------
	void InstanceEntry( CMessage *const pcMsg );    //��Ϣ���
	void ProcGuardPowerOn( CMessage *const pcMsg );	//������Ϣ��Ӧ����
	void ProcMainTaskScanTimeOut( CMessage *const pcMsg );	//̽�����̶߳�ʱ����ʱ������
	void ProcMainTaskScanAck( CMessage *const pcMsg );	//̽���Ӧ��Ϣ������
};


class CMcuPrsGuardData
{
public:
	CMcuPrsGuardData();
	virtual ~CMcuPrsGuardData(){};

public:
	u8 GetNoScanAckTimes(void);
	void AddNoScanAckTimes(void);
	void ResetNoScanAckTimes(void);

	BOOL32 IsRecvScanAck(void);
	void  SetIsRecvScanAck(BOOL32 bIsRecv);

private:
	u8 m_byNoScanAckTimes;
	u8 m_byRecvScanAck;
};

typedef zTemplate< CMcuPrsGuard, 1, CMcuPrsGuardData > CMcuPrsGuardApp;
extern CMcuPrsGuardApp g_cMcuPrsGuardApp;

#endif

