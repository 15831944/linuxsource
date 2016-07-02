/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : bapagent.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: �������
   ����        : 
   �汾        :
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
******************************************************************************/
#ifndef BAPAGENT_H
#define BAPAGENT_H

#include "boardagentbasic.h"
#include "brdwrapperdef.h"
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

	void ProcBoardConfigModify();
};

class CBoardConfig : public CBBoardConfig
{
public:
    BOOL32 ReadConfig();
    BOOL32 SetEqpEMixerEntry(u8 byMixIdx,TEqpMixerEntry tEqpMixerEntry) 
	{ 
		m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].byEqpId       = tEqpMixerEntry.GetEqpId();
		m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].byEqpType     = 12;//�ϵ�EMIXER���������ͣ�ͳһ�����ϵĶ���
        
		// miaoqingsong [20110610] ������������ʼ��ʱ�Ѿ���ReadConfig()�ӿڶ��������ļ�������û��Ҫ�ٶ�
		if ( TRUE == m_bIsSimuApu )
		{
			m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].byEqpType     = EQP_TYPE_MIXER;
			m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].m_byMixerMemberNum = MAXNUM_APU_MIXING_MEMBER;
		}
		else
		{
			m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].m_byMixerMemberNum = tEqpMixerEntry.GetMaxChnInGrp();
		}

// 		BOOL  bRet;
// 		s32 sdwDefault = 0;
// 		s32 sdwReturnValue = 0;
// 		s8    achProfileName[64] = {0};
// 		memset((void*)achProfileName, 0x0, sizeof(achProfileName));
//  		sprintf(achProfileName, "%s/%s", DIR_CONFIG, "mcueqp.ini");
// 		bRet = GetRegKeyInt( achProfileName, SECTION_EqpMixer, KEY_IsSimuApu, 
// 							sdwDefault, &sdwReturnValue);
// 
// 		if( bRet && sdwReturnValue != 0 )  
// 		{
// 			m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].byEqpType     = EQP_TYPE_MIXER;
// 			m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].m_byMixerMemberNum = MAXNUM_APU_MIXING_MEMBER;
// 		}
// 		else
// 		{
// 			m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].m_byMixerMemberNum = tEqpMixerEntry.GetMaxChnInGrp();
// 		}


		m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].wMcuId        = LOCAL_MCUID;
		m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].wRcvStartPort = tEqpMixerEntry.GetEqpRecvPort();
		strcpy(m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].achAlias, tEqpMixerEntry.GetAlias());
		m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].achAlias[MAXLEN_EQP_ALIAS-1] = '\0';
		//m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].m_byMixerMemberNum = tEqpMixerEntry.GetMaxChnInGrp();
		m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].m_bIsMultiFormat = FALSE;
		return TRUE;
	}
	void SetEMixerNum(u8 byMixNum)
	{
		m_tEapuCfg.m_byMixerNum = min(MAXNUM_EAPU_MIXER,byMixNum);
	}
	TEapuCfg GetEapuCfg() 
	{  
		return m_tEapuCfg;
	}
	void SetProductTest(BOOL32 bIsProduct){ m_tEapuCfg.SetProductTest(bIsProduct);	}
	BOOL32 IsProductTest(){ return m_tEapuCfg.IsProductTest();	}
	BOOL32 IsSimuApu(){ return m_bIsSimuApu;	}
private:
	BOOL32    m_bIsSimuApu;
    TEapuCfg  m_tEapuCfg;
};

typedef zTemplate< CBoardAgent, BOARD_AGENT_MAX_INSTANCE, CBoardConfig > CBrdAgentApp;

extern CBrdAgentApp	g_cBrdAgentApp;	

API BOOL InitBrdAgent();
void BrdEapuAPIEnableInLinux(void);

#endif  /* BAPAGENT_H */
