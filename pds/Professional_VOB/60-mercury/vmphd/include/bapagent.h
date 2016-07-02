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
    void ProcBoardGetConfigAck( CMessage* const )
    {
    }
    void ProcBoardGetConfigNAck( CMessage* const )
    {
    }
};

class CBoardConfig : public CBBoardConfig
{
public:
    BOOL32 ReadConfig();

    u8     GetEqpId() const
    {
        return m_byEqpId;
    }
private:
    u8     m_byEqpId;
};

typedef zTemplate< CBoardAgent, BOARD_AGENT_MAX_INSTANCE, CBoardConfig, 0 > CBrdAgentApp;

extern CBrdAgentApp	g_cBrdAgentApp;	//�������Ӧ��ʵ��

API BOOL InitBrdAgent();

#endif  /* BAPAGENT_H */
