/*****************************************************************************
   ģ����      : apu2boardagent
   �ļ���      : apu2agent.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: �������
   ����        : �ܼ���
   �汾        :  1.0
   ����		   :  2012/02/14
-----------------------------------------------------------------------------
******************************************************************************/
#ifndef BAPAGENT_H
#define BAPAGENT_H

#include "boardagentbasic.h"
#include "brdwrapperdef.h"
#include "apu2cfg.h"

	/************************************************************************/
	/*						CBoardAgent��������ඨ��		                */
	/************************************************************************/
class CBoardAgent : public CBBoardAgent
{
public:
	CBoardAgent(){}
	~CBoardAgent(){}

public:
    void InstanceEntry( CMessage* const pMsg );

	void ProcBoardGetConfigAck( CMessage* const pcMsg );

	void ProcBoardGetConfigNAck( CMessage* const pcMsg );

    void ProcLedStatusReq(CMessage* const pcMsg);

	void ProcBoardLedStatus( CMessage* const pcMsg );

	void ProcBoardConfigModify( CMessage* const pcMsg );

public:
    TBrdLedState m_tLedState;
};


	/************************************************************************/
	/*						CBoardConfig���������ඨ��		                */
	/************************************************************************/
class CBoardConfig : public CBBoardConfig
{
public:
	CBoardConfig(){}
	~CBoardConfig(){}
public:

    BOOL32		ReadConfig();

    BOOL32		SetEqpEMixerEntry(u8 byMixIdx, TEqpMixerEntry tEqpMixerEntry);
	
	BOOL32		SetEqpBasEntry(u8 byBasIdx, TEqpMpuBasEntry tEqpMpuBasEntry);

	void		SetApu2MixerNum(u8 byMixerNum);

	void		SetApu2BasNum(u8 byBasNum);

	TApu2EqpCfg GetApu2Cfg();

	void		SetProductTest(BOOL32 bIsProduct);

	BOOL32		IsProductTest();
 
	BOOL32		IsSimuApu();
	
private:
	BOOL32		 m_bIsSimuApu;   //�Ƿ�ģ��apu
	u16          m_wSimApuVer;   //ģ��apu�汾��
    TApu2EqpCfg  m_tApu2Cfg;     //Apu2����
	u8			 m_byQualityLvl; //���ʵȼ�
};

typedef zTemplate< CBoardAgent, BOARD_AGENT_MAX_INSTANCE, CBoardConfig > CBrdAgentApp;

extern CBrdAgentApp	g_cBrdAgentApp;	

API BOOL InitBrdAgent();
#endif