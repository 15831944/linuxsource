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
   2004/12/05  3.5         �� ��        �½ӿڵ��޸�
******************************************************************************/
#ifndef IMTAGENT_H
#define IMTAGENT_H

#include "osp.h"
#include "agtcomm.h"
#include "mcuagtstruct.h"
#include "boardagentbasic.h"
#include "imt_config.h"


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

	//�����Ǻ�MANAGER����Ϣ����
	//����Ӧ����Ϣ
	void ProcBoardGetConfigAck( CMessage* const pcMsg );
	//���÷�Ӧ����Ϣ
	void ProcBoardGetConfigNAck( CMessage* const pcMsg );
	
	//ȡ����״̬��Ϣ����
	void ProcLedStatusReq( CMessage* const pcMsg );

	//������״̬��Ϣ����
	//�ڴ�״̬
	//ģ����Ϣ����
	void ProcBoardModuleStatus( CMessage* const pcMsg );
	//��״̬�ı�
	void ProcBoardLedStatus( CMessage* const pcMsg );
	//����״̬�ı�
	void ProcImtFanStatus( CMessage* const pcMsg );

	//MPC�޸ĵ�������, zgc, 2007-09-17
	void ProcBoardConfigModify( CMessage* const pcMsg );

protected:
    
    void MsgGetConfAck(CMessage* const pcMsg);
    
//#ifndef WIN32
#ifdef IMT
	TBrdIMTLedStateDesc m_tLedState;
#endif
//#endif
};

typedef zTemplate< CBoardAgent, BOARD_AGENT_MAX_INSTANCE, CImtConfig, 0 > CBrdAgentApp;

extern CBrdAgentApp	g_cBrdAgentApp;	//�������Ӧ��ʵ��

void ImtLog(char * fmt, ...);
API BOOL InitImtAgent();
API void pimtmsg(void);
API void npimtmsg(void);

#endif  /* IMTAGENT_H */
