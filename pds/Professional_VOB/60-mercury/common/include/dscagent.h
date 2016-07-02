/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : dscagent.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ���������������ͨ��������
   ����        : �� ��
   �汾        : V3.6  Copyright(C) 2005-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2005/04/25  3.6         �� ��         ����
   2005/11/23  40          john          ����ģ�顢�Ƹ澯�澯������
******************************************************************************/
#ifndef _DSCAGENT_H_
#define _DSCAGENT_H_

#include "osp.h"
#include "dscconfig.h"

class CBoardAgent:public CInstance
{
	enum 
	{ 
		STATE_IDLE,			//����
		STATE_INIT,			//��ʼ��
		STATE_NORMAL,		//����
	};

public:
	CBoardAgent();
	~CBoardAgent();

protected:
	//ʵ����Ϣ������ں���������override
	void InstanceEntry(CMessage* const pMsg);
	void InstanceExit();

	//OSP����
	void ProcOspDisconnect( CMessage* const pcMsg );

	//�����Ǻ�MANAGER����Ϣ����
	//ע��Ӧ����Ϣ
	void ProcBoardRegAck( CMessage* const pcMsg );	
	//ע���Ӧ����Ϣ
	void ProcBoardRegNAck( CMessage* const pcMsg );
	//ȡ������Ϣ��Ϣ��ʱ�����ڴ���
	void ProcGetConfigTimeOut( CMessage* const pcMsg );
	//����Ӧ����Ϣ
	void ProcBoardGetConfigAck( CMessage* const pcMsg );
	//���÷�Ӧ����Ϣ
	void ProcBoardGetConfigNAck( CMessage* const pcMsg );
    //��������Ϣ֪ͨ
    void ProcSetDscInfoReq( CMessage* const pcMsg );
	// DSC��GK info��������
	void ProcBoardGkInfoUpdateCmd( CMessage* const pcMsg );
	// DSC��LoginInfo����
	void ProcBoardLoginInfoUpdateCmd( CMessage* const pcMsg );
    
	//�澯ͬ������
	void ProcAlarmSyncReq( CMessage* const pcMsg );
	//������Ϣ
	void ProcBoardPowerOn( CMessage* const pcMsg );
	//���ӳ�ʱ
	void ProcBoardConnectManagerTimeOut( BOOL32 bIsConnectA );
	//�ȴ�ע�ᳬʱ
	void ProcBoardRegisterTimeOut( BOOL32 bIsConnectA );
	
    void ProcBoardModuleStatus( CMessage* const pcMsg );
    void ProcBoardLedStatus( CMessage* const pcMsg );
    void ProcGetModuleInfoReq( CMessage* const pcMsg );
	// reboot the board
	void ProcRebootCmd(CMessage* const pcMsg);
	void ProcBoardSelfTestCmd(CMessage* const pcMsg);
	void ProcBitErrorTestCmd(CMessage* const pcMsg );
	void ProcTimeSyncCmd(CMessage* const pcMsg );
	void ProcUpdateSoftwareCmd(CMessage* const pcMsg );
	void ProcStartDscUpdateSoftwareReq( CMessage* const pcMsg );
	void ProcUpdateFileReq( CMessage* const pcMsg );
	void ProcUpdateSoftwareWaittimer( CMessage* const pcMsg );

protected:
	//ȡ�����Ҫ���ӵ�MPC��������Ϣ
	BOOL GetBoardCfgInfo();
	//���ӹ������
	BOOL ConnectManager(u32& dwMcuNode, u32 dwMpcIp, u16 wMpcPort);
	//������Ϣ���������
	BOOL PostMsgToManager( u16  wEvent, u8 * const pbyContent, u16  wLen );

	void FreeDataA(void);
    void FreeDataB(void);
    void RegisterToMcuAgent(u32 dwDstNode);
    void MsgRegAck(CMessage* const pcMsg );
    void MsgGetConfAck(CMessage* const pcMsg);
    void MsgDisconnectInfo(CMessage* const pcMsg);

	void   NackUpdateFileReq(void);

private:
	TDscUpdateRsp m_tCurUpdateReq;
	s8 m_achCurUpdateFileName[MAXNUM_FILE_UPDATE][MAXLEN_MCU_FILEPATH];
	u8 m_abyUpdateResult[MAXNUM_FILE_UPDATE];
	u8 m_byUpdateFileNum;
	u8 m_byWaitTimes;
	u8 m_byErrTimes;
	u8 m_byBrdIdx;
	u8 m_byMcsSsnIdx;
	FILE *m_hUpdateFile;
	u8 *m_pbyFileBuf;
	u32 m_dwBufUsedSize;

private:
	TBrdPosition	m_tBoardPosition;    //����λ�ã������㡢�ۡ�����
	u32	m_dwDstNode;    //�ڵ��
	u32	m_dwDstIId;		//Ŀ��ʵ����

    u32 m_dwDstNodeB;
    u32 m_dwDstIIdB;

private:
    u32 m_dwBoardIpAddr;

#ifndef WIN32
	TBrdCRILedStateDesc m_tLedState;
#endif
	// ������
	BOOL    m_bIsTest;
	u16 	m_wLastEvent;
	u8	m_abyLastEventPara[20];
};

typedef zTemplate< CBoardAgent, BOARD_AGENT_MAX_INSTANCE, CDscConfig, 0 > CBrdAgentApp;

extern CBrdAgentApp	g_cBrdAgentApp;	//�������Ӧ��ʵ��

API BOOL InitDscAgent();

API u8 GetBoardSlot();

void DscAgentAPIEnableInLinux();

#endif  /* DSCAGENT_H */
