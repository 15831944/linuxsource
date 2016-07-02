/*****************************************************************************
   ģ����      : mpu2Board Agent
   �ļ���      : mpu2agent.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: �������
   ����        : zhouyiliang
   �汾        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
******************************************************************************/
#ifndef MPU2AGENT_H
#define MPU2AGENT_H

#include "boardagentbasic.h"
#include "kdvlog.h"

class CBoardAgent : public CBBoardAgent
{
public:
    TBrdLedState m_tLedState;

public:
    void  InstanceEntry( CMessage* const pMsg );
	//����Ӧ����Ϣ
	void ProcBoardGetConfigAck( CMessage* const pcMsg );
	//���÷�Ӧ����Ϣ
	void ProcBoardGetConfigNAck( CMessage* const pcMsg );
    //LED�ƻ�ȡ
    void ProcLedStatusReq(CMessage* const pcMsg);

	void ProcBoardLedStatus( CMessage* const pcMsg );

	void ProcBoardConfigModify( CMessage* const pcMsg );
};

class CBoardConfig : public CBBoardConfig
{
public:
    BOOL32 ReadConfig();   
	void SetIsAutoTest(BOOL32 bAutoTest){ m_tMpu2Cfg.SetIsAutoTest(bAutoTest) ; }
	BOOL32 GetIsAutoTest()const{return m_tMpu2Cfg.GetIsAutoTest();}
	void SetMpu2EqpNum(u8 byEqpNum){m_tMpu2Cfg.SetMpu2EqpNum(byEqpNum);}
	u8   GetMpu2EqpNum()const{return m_tMpu2Cfg.GetMpu2EqpNum(); }
	void SetMpu2WorkMode(u8 byWorkMode){m_tMpu2Cfg.SetMpu2WorkMode(byWorkMode);}
	u8   GetMpu2WorkMode()const{return m_tMpu2Cfg.GetMpu2WorkMode(); }
	BOOL32 SetMpu2EqpCfg(u8 byEqpIdx, const TEqpBasicCfg& tEqpCfg)
	{
		return m_tMpu2Cfg.SetMpu2EqpCfg(byEqpIdx,tEqpCfg);
	}
	TEqpBasicCfg GetMpu2EqpCfg(u8 byEqpIdx)const
	{
	
		return m_tMpu2Cfg.GetMpu2EqpCfg(byEqpIdx);
	}

	TMpu2Cfg GetMpu2Cfg()const{return m_tMpu2Cfg;}
protected:
	TMpu2Cfg m_tMpu2Cfg;  //mpu2�ĵ���������Ϣ�����������˼������裬ÿ����������õȡ�
};

typedef zTemplate< CBoardAgent, BOARD_AGENT_MAX_INSTANCE, CBoardConfig > CBrdAgentApp;

extern CBrdAgentApp	g_cBrdAgentApp;	

API BOOL InitBrdAgent();
void BrdMpuAPIEnableInLinux(void);


#endif  /* MPU2AGENT_H */
