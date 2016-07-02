/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : bapagent.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: �������
   ����        : guzh
   �汾        : V4.5  Copyright(C) 2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
******************************************************************************/
#ifndef BAPAGENT_H
#define BAPAGENT_H

#include "boardagentbasic.h"

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

    u8     GetEqpId() const
    {
        return m_byEqpId;
    }
    void   SetEqpId( u8 byEqpId ) 
    {
        m_byEqpId = byEqpId;
    }
    u8     GetEqpIdB() const
    {
        return m_byEqpIdB;
    }
    void   SetEqpIdB( u8 byEqpId ) 
    {
        m_byEqpIdB = byEqpId;
    }
    u8     GetWorkMode() const
    {
        return m_byWorkMode;
    }
    void   SetWorkMode( u8 byWorkMode ) 
    {
        m_byWorkMode = byWorkMode;
    }
    void   SetEqpRcvStartPort( u16 wEqpRcvStartPort) 
    { 
        m_wEqpRcvStartPort = htons(wEqpRcvStartPort); 
    }
	u16    GetEqpRcvStartPort( void ) const 
    { 
        return ntohs(m_wEqpRcvStartPort); 
    }
    void   SetEqpRcvStartPort2( u16 wEqpRcvStartPort) 
    { 
        m_wEqpRcvStartPort2 = htons(wEqpRcvStartPort); 
    }
	u16    GetEqpRcvStartPort2( void ) const 
    { 
        return ntohs(m_wEqpRcvStartPort2); 
    }

	BOOL  GetIsTest(void){ return m_bTest; }
    void  SetIsTest( BOOL bTest ){ m_bTest = bTest; }
private:
    u8     m_byEqpId; 
    u8     m_byEqpIdB;
    u8     m_byWorkMode;
    u16    m_wEqpRcvStartPort;
    u16    m_wEqpRcvStartPort2;
	BOOL32 m_bTest;
};

typedef zTemplate< CBoardAgent, BOARD_AGENT_MAX_INSTANCE, CBoardConfig > CBrdAgentApp;

extern CBrdAgentApp	g_cBrdAgentApp;	

API BOOL InitBrdAgent();
void BrdMpuAPIEnableInLinux(void);

#endif  /* BAPAGENT_H */
