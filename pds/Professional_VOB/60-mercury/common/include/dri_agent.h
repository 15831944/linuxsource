/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : driagent.h
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
#ifndef DRIAGENT_H
#define DRIAGENT_H

#include "dri_config.h"
#include "boardagentbasic.h"


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

	//ȡ����״̬��Ϣ����
	void ProcLedStatusReq( CMessage* const pcMsg );
	//e1���������
	void ProcE1LoopBackCmd( CMessage* const pcMsg );

	//��״̬�ı�
	void ProcBoardLedStatus( CMessage* const pcMsg );
	//��·״̬�ı�
	void ProcBoardLinkStatus( CMessage* const pcMsg );
	//MPC�޸ĵ�������, zgc, 2007-09-19
	void ProcBoardConfigModify( CMessage* const pcMsg );
	
	//����E1�澯״̬֪ͨ
	void ProcBoardE1AlarmNotif(CMessage* const pcMsg);
    //����E1��·��ⳬʱ
    void ProcBoardCheckE1TimerOut( void );

private:
//#ifndef WIN32
	//TBrdDRILedStateDesc m_tLedState;
	TBrdLedState m_tBrdLedState;

    u16 m_wCheckE1Time;            //Dri����E1��·��ʱ����(����)
    u8  m_wPortE1Num[8];               //���˿�����E1�˿���
    u16 m_wLastE1BandWidth[8];     //�����ϴμ��E1������

//#endif

	u8 m_byE1Alarm[8];	// ��¼8·E1��Alarm��Ϣ [10/26/2011 chendaiwei]
};

typedef zTemplate< CBoardAgent, BOARD_AGENT_MAX_INSTANCE, CDriConfig, 0> CBrdAgentApp;

extern CBrdAgentApp	g_cBrdAgentApp;	//�������Ӧ��ʵ��

API BOOL InitDriAgent();
API u32 BrdGetDstMcuNode(void);
API u32 BrdGetDstMcuNodeB(void);
API void noupdatesoft(void);
API void updatesoft(void);
API void pdrimsg(void);
API void npdrimsg(void);
void DriLog(char * fmt, ...);
API u8  GetBoardSlot();

#endif  /* DRIAGENT_H */
