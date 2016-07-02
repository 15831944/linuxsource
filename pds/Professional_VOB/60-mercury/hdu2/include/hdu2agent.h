/*****************************************************************************
ģ����      : Hdu2Board Agent
�ļ���      : hdu2agent.h
����ļ�    : 
�ļ�ʵ�ֹ���: HDU2�������
����        : ��־��
�汾        : V4.7  Copyright(C) 2011 KDV, All rights reserved.
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
11/11/28    4.7         ��־��		  ����
******************************************************************************/
#ifndef _HDU2AGENT_H
#define _HDU2AGENT_H
#include "boardagentbasic.h"
#include "boardguardbasic.h"
/*=====================================================================
����  : hdu2agtLog
����  : HDU2�����ӡ�ӿ�
����  : const u8 byLevel ��ӡ����
		const u16 wModule ��ӡģ���
		const s8* pszFmt, ...�ɱ������
���  : ��
����  : ��
ע    :
-----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2011/11/28  4.7         ��־��        ����
=====================================================================*/
inline void hdu2agtLog( const u8 byLevel, const u16 wModule, const s8* pszFmt, ...)
{
	s8 achBuf[1024] = { 0 };
    va_list argptr;		      
    va_start( argptr, pszFmt );
	vsnprintf(achBuf , 1024, pszFmt, argptr);

    va_end(argptr); 
	LogPrint( byLevel, wModule, achBuf );
    return;
}
class CHdu2BoardAgent : public CBBoardAgent
{
private:
   void  InstanceEntry(CMessage* const pMsg);
   void  ProcBoardGetConfigAck( CMessage* const pcMsg );     //ȡ������ϢӦ����Ϣ
   void  ProcBoardGetConfigNAck( CMessage* const pcMsg );    //ȡ������Ϣ��Ӧ��
};
class CHdu2BoardConfig : public CBBoardConfig
{
private:
	BOOL32 m_bIsTest;
    u8     m_byEqpId;    
    THduModePort m_atHdu2ChnCfg[MAXNUM_HDU_CHANNEL];
	u8     m_byStartMode;  //����ģʽ(��mpc��֪)
	//���ڼ�¼�������ļ��ж�ȡ���ĸ���ͨ��������ӿ�����
	u8     m_byOutPortTypeInCfg[MAXNUM_HDU_CHANNEL];
public:
	CHdu2BoardConfig()
	{
		m_bIsTest = FALSE;
		m_byEqpId = 0;
		m_byStartMode = 0;
		memset(m_atHdu2ChnCfg, 0x0, sizeof(m_atHdu2ChnCfg));
		memset(m_byOutPortTypeInCfg, 0x0, sizeof(m_byOutPortTypeInCfg));
	}
    BOOL32 ReadConfig(void);
    void  GetChnCfg(u8 byNum, THduModePort &tModePort) 
	{
		tModePort.SetOutModeType( m_atHdu2ChnCfg[byNum].GetOutModeType() );
		tModePort.SetOutPortType( m_atHdu2ChnCfg[byNum].GetOutPortType() );
		tModePort.SetZoomRate( m_atHdu2ChnCfg[byNum].GetZoomRate() );
		tModePort.SetScalingMode( m_atHdu2ChnCfg[byNum].GetScalingMode() );
	}
	void SetChnCfg( u8 byNum, THduModePort &tModePort )
	{
        m_atHdu2ChnCfg[byNum].SetOutModeType( tModePort.GetOutModeType() );
		m_atHdu2ChnCfg[byNum].SetOutPortType( tModePort.GetOutPortType() );
		m_atHdu2ChnCfg[byNum].SetZoomRate( tModePort.GetZoomRate() );
		m_atHdu2ChnCfg[byNum].SetScalingMode( tModePort.GetScalingMode() );
	}
	
    u8 GetEqpId(void) const
    {
        return m_byEqpId;
    }
	
	void  SetEqpId( u8 byEqpId )
	{
		m_byEqpId = byEqpId;
	}
    
	BOOL  GetIsTest(void){ return m_bIsTest; }
    void  SetIsTest( BOOL bIsTest ){ m_bIsTest = bIsTest; }

	void SetStartMode(u8 byStartMode){ m_byStartMode = byStartMode;}
	u8   GetStartMode(){ return m_byStartMode;}
	void SetOutPortTypeInCfg(u8 byChnIdx,u8 byOutPortTypeInCfg)
	{
		if (byChnIdx >= MAXNUM_HDU_CHANNEL)
		{
			return;
		}
		m_byOutPortTypeInCfg[byChnIdx] = byOutPortTypeInCfg;
	}
	u8   GetOutPortTypeInCfg(u8 byChnIdx)
	{
		if (byChnIdx >= MAXNUM_HDU_CHANNEL)
		{
			return 0;
		}
		return m_byOutPortTypeInCfg[byChnIdx];
	}
};
typedef zTemplate< CHdu2BoardAgent, BOARD_AGENT_MAX_INSTANCE, CHdu2BoardConfig, 0 > CHdu2BrdAgentApp;
extern CHdu2BrdAgentApp	g_cBrdAgentApp;	//�������Ӧ��APP
API BOOL32 InitBrdAgent(void);
u32 BrdGetDstMcuNode(void);
u32 BrdGetDstMcuNodeB(void);
#endif  /* _HDU2AGENT_H */

