/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : criagent.h
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
#ifndef CRIAGENT_H
#define CRIAGENT_H

#include "cri_config.h"
#include "boardagentbasic.h"

class CBoardAgent:public CBBoardAgent
{

public:
	CBoardAgent();
	~CBoardAgent();

protected:
	//ʵ����Ϣ������ں���������override
	void InstanceEntry(CMessage* const pMsg);
	void InstanceExit();

	//����Ӧ����Ϣ
	void ProcBoardGetConfigAck( CMessage* const pcMsg );
	//���÷�Ӧ����Ϣ
	void ProcBoardGetConfigNAck( CMessage* const pcMsg );
	//ȡ����״̬��Ϣ����
	void ProcLedStatusReq( CMessage* const pcMsg );     // change[2012/05/04 liaokang]
	//��״̬�ı� 
	void ProcBoardLedStatus( CMessage* const pcMsg );   // change[2012/05/04 liaokang]
    // ȡ����״̬��Ϣ����
	void ProcEthPortStatusReq( CMessage* const pcMsg );
    // ����״̬�ı䣨��ϵͳ��
	void ProcBoardEthPortStatus( CMessage* const pcMsg );   // [2012/05/04 liaokang]
	//ģ��״̬�ı�
	void ProcBoardModuleStatus( CMessage* const pcMsg );
	//MPC�޸ĵ�������, zgc, 2007-09-17
	void ProcBoardConfigModify( CMessage* const pcMsg );
    
    //  ֧��ȡ֧�ִ�ϵͳ��IS2.2 8313��Led������״̬[2012/05/04 liaokang]
    // ��ϵͳ��IS2.2 8313�� ע������
    void ProcBrdSlaveSysRegReq( CMessage* const pcMsg );
    // ��ϵͳ��IS2.2 8313�� led״̬
    void ProcBrdSlaveSysLedStatus( CMessage* const pcMsg );
    // ��ϵͳ��IS2.2 8313�� ����״̬
    void ProcBrdSlaveSysEthPortStatus( CMessage* const pcMsg );
    // ��������
    void ProcDisconnect( CMessage* const pcMsg );
    // ����Ϣ����ϵͳ��IS2.2 8313��
    void PostMsgToBrdSlaveSys( u16 wEvent, void * pMsg = NULL, u16 wLen = 0 );

	//����IS2.2����[6/8/2013 chendaiwei]
	void ProcBoardIS22ResetCmd( CMessage* const pcMsg );

//#ifndef WIN32
//	TBrdCRILedStateDesc m_tLedState;
//#endif

// #if (defined _LINUX_ && defined _LINUX12_)
// 	TBrdCRI2LedDesc m_tLedState;
// #else
// 	TBrdCRILedStateDesc m_tLedState;
// #endif

//  ֧��IS2.2 [2012/05/04 liaokang]
 private:    
    TBrdLedState	 m_tBrdMasterSysLedState;   // ��ϵͳ��CRI/CRI2/IS2.2 8548��led״̬
#ifdef _LINUX12_   // Ŀǰ��brdwrapper.h����TBrdEthInfo SWEthInfo
    /* �������赥����˵,IS2.2 8313��ȡ�ڲ�ͨ�����ڣ������ڣ���Ϣ�������ͨ��������ȡǰ������Ϣ */
    TBrdEthInfo      m_tBrdMasterSysFrontEthPortState; // ��ϵͳ��CRI/CRI2/IS2.2 8548��ǰ����״̬
    SWEthInfo        m_tBrdSlaveSysEthPortState;// ��ϵͳ��IS2.2 8313������״̬ 
#endif //end _LINUX12_
    TBrdLedState	 m_tBrdSlaveSysLedState;    // ��ϵͳ��IS2.2 8313��led״̬
    u32			     m_dwBrdSlaveSysNode;		// ��ϵͳ��IS2.2 8313���Ľ���
	u32			     m_dwBrdSlaveSysIId;		// ��ϵͳ��IS2.2 8313����IID
};

typedef zTemplate< CBoardAgent, BOARD_AGENT_MAX_INSTANCE, CCriConfig, 0 > CBrdAgentApp;

extern CBrdAgentApp	g_cBrdAgentApp;	//�������Ӧ��ʵ��

API BOOL InitCriAgent();
API void pcrimsg(void);
API void npcrimsg( void );
API void CriLog(char * fmt, ...);
API u32 BrdGetDstMcuNode( void );
API u32 BrdGetDstMcuNodeB( void );
API u8 GetBoardSlot();

#endif  /* CRIAGENT_H */
