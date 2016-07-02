/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : mmpagent.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ���������������ͨ��������
   ����        : jianghy
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/08/25  1.0         jianghy       ����
   2004/11/11  3.5         �� ��         �½ӿڵ��޸�
******************************************************************************/
#ifndef MMPAGENT_H
#define MMPAGENT_H

#include "osp.h"
#include "mmp_config.h"
#include "boardagentbasic.h"


#define  MAX_TIME_SPAN_REBOOT	    3		// �������ʱ����(����)

class CBoardAgent:public CBBoardAgent
{

public:
	CBoardAgent();
	~CBoardAgent();

protected:
	//ʵ����Ϣ������ں���������override
	void InstanceEntry(CMessage* const pMsg);

	//�˳�ʱ����
	void InstanceExit();

	//����Ӧ����Ϣ
	void ProcBoardGetConfigAck( CMessage* const pcMsg );
	//���÷�Ӧ����Ϣ
	void ProcBoardGetConfigNAck( CMessage* const pcMsg );

	//MPC�޸ĵ�������, zgc, 2007-09-17
	void ProcBoardConfigModify( CMessage* const pcMsg );
	
	//ȡ����״̬��Ϣ����
	void ProcLedStatusReq( CMessage* const pcMsg );
	//ģ����Ϣ����
	void ProcBoardModuleStatus( CMessage* const pcMsg );
	//��״̬�ı�
	void ProcBoardLedStatus( CMessage* const pcMsg );

	// �Ƚ�����ʱ���Ƿ񳬹�ָ�����
	BOOL32 Compare2TimeIsValid(struct tm* ptTimeA, struct tm* ptTimeB); 

private:
	time_t  m_tLastDisTimeA; // Mpc A �ϴζ�����ʱ��
	time_t  m_tLastDisTimeB; // Mpc B �ϴζ�����ʱ��
    
    u8  m_byConnectNum;
#ifdef MMP
	TBrdMMPLedStateDesc m_tLedState;
#endif
};

typedef zTemplate< CBoardAgent, BOARD_AGENT_MAX_INSTANCE, CMmpConfig, 0 > CBrdAgentApp;

extern CBrdAgentApp	g_cBrdAgentApp;	//�������Ӧ��ʵ��

API BOOL InitMmpAgent();
void MmpLog(char * fmt, ...);
API void pmmpmsg(void);
API void npmmpmsg(void);

#endif  /* MMPAGENT_H */
