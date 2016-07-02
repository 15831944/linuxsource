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
   2004/11/11  3.5         �� ��         �½ӿڵ��޸�
******************************************************************************/
#ifndef MMPAGENT_H
#define MMPAGENT_H

#include "osp.h"
#include "kdvsys.h"
#include "agtcomm.h"
#include "mcuagtstruct.h"
#include "mmpconfig.h"

#ifdef _VXWORKS_
#include "brddrvLib.h"
#endif

// MPC2 ֧��
#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
        #include "brdwrapperdef.h"
        #include "nipwrapper.h"
        #include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
#endif

#define BOARD_AGENT_MAX_INSTANCE	1		// ���嵥������������ʵ����Ŀ
#define POWEN_ON_CONNECT            200
#define CONNECT_MANAGER_TIMEOUT		2000	// ��������MPC�Ϲ������Ķ�ʱ��ʱ��(2��)
#define REGISTER_TIMEOUT			4000	// ע��ȴ���ʱ��ʱ��(4��)
#define GET_CONFIG_TIMEOUT			3000	// ȡ������Ϣ�ȴ���ʱ��ʱ��(3��)
#define SCAN_STATE_TIME_OUT			5000	// ��ʱɨ�赥��״̬��ʱ��ʱ��(5s)
#define  MAX_TIME_SPAN_REBOOT	    3		// �������ʱ����(����)
#define	MAXNUM_BOARD_ALARM			80		// ����澯�����󳤶�

//�澯�ṹ
typedef struct
{
	u32	dwBoardAlarmSerialNo;			//�澯���к�
    u32 dwBoardAlarmCode;				//�澯��
    u8  byBoardAlarmObjType;			//�澯��������
    u8	byBoardAlarmObject[5];		    //�澯����
	TBoardAlarmMsgInfo tBoardAlarm;         //״̬��Ϣ�ͽṹ
    BOOL  	bExist;						    //�ø澯�Ƿ���Ч
}TBoardAlarm;

//��WINDOWS��ģ��ʹ��
#ifdef WIN32
typedef struct
{
    u8 byBrdID;                      // ����ID�� 
    u8 byBrdLayer;                   // �������ڲ��
    u8 byBrdSlot;                    // �������ڲ�λ��
}TBrdPosition;
#else
/*
#ifndef MMP
typedef struct
{
    u8 byBrdID;                      // ����ID�� 
    u8 byBrdLayer;                   // �������ڲ��
    u8 byBrdSlot;                    // �������ڲ�λ��
}TBrdPosition;
#endif
*/
#endif 


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

	//�˳�ʱ����
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
	//�澯ͬ������
	void ProcAlarmSyncReq( CMessage* const pcMsg );
	//������Ϣ
	void ProcBoardPowerOn( CMessage* const pcMsg );
	//���ӳ�ʱ
	void ProcBoardConnectManagerTimeOut( BOOL32 bIsConnectA );
	//�ȴ�ע�ᳬʱ
	void ProcBoardRegisterTimeOut( BOOL32 bIsConnectA );
	
	//ȡ��������汾��Ϣ����
	void ProcGetVersionReq( CMessage* const pcMsg );
	//ȡ�����ģ����Ϣ��Ϣ����
	void ProcGetModuleInfoReq( CMessage* const pcMsg );
	//���帴λ��Ϣ����
	void ProcBoardResetCmd( CMessage* const pcMsg );
	//����ʱ��ͬ����Ϣ����
	void ProcTimeSyncCmd( CMessage* const pcMsg );
	//�����Բ���Ϣ����
	void ProcBoardSelfTestCmd( CMessage* const pcMsg );
	//�������������Ϣ����
	void ProcBitErrorTestCmd( CMessage* const pcMsg );
	//����汾������Ϣ
	void ProcUpdateSoftwareCmd( CMessage* const pcMsg );
	//ȡ�����ͳ����Ϣ��Ϣ����
	void ProcGetStatisticsReq( CMessage* const pcMsg );
	//ȡ����״̬��Ϣ����
	void ProcLedStatusReq( CMessage* const pcMsg );

	//������״̬��Ϣ����
	//�ڴ�״̬
	void ProcBoardMemeryStatus( CMessage* const pcMsg );
	//�ļ�ϵͳ״̬
	void ProcBoardFileSystemStatus( CMessage* const pcMsg );
	//ģ����Ϣ����
	void ProcBoardModuleStatus( CMessage* const pcMsg );
	//��״̬�ı�
	void ProcBoardLedStatus( CMessage* const pcMsg );

protected:
	//��������Ϣ
	void ProcBoardGetCfgTest( CMessage* const pcMsg );
	void ProcBoardGetAlarm( CMessage* const pcMsg );
	void ProcBoardGetLastManagerCmd( CMessage* const pcMsg );
	void TestingProcess( CMessage* const pcMsg );

	
protected:
	//ȡ�����Ҫ���ӵ�MPC��������Ϣ
	BOOL GetBoardCfgInfo();
	//���ӹ������
	BOOL ConnectManager(u32& dwMcuNode, u32 dwMpcIp, u16 wMpcPort);
	//������Ϣ���������
	BOOL PostMsgToManager( u16  wEvent, u8 * const pbyContent, u16  wLen );
	//�澯�б���������Ӧ�ĸ澯��Ϣ
    BOOL AddAlarm( u32 dwAlarmCode, u8 byObjType, u8 byObject[], TBoardAlarm *ptData );
	//�澯�б���ɾ����Ӧ�ĸ澯��Ϣ
    BOOL DeleteAlarm( u32 dwSerialNo );	
	//�澯�б��л�ȡ��Ӧ�ĸ澯��Ϣ
    BOOL FindAlarm( u32 dwAlarmCode, u8 byObjType, u8 byObject[], TBoardAlarm *ptData );
	//��������ø澯����Ϣ��Ϣ
	BOOL SetAlarmMsgInfo( u32 dwSerialNo, TBoardAlarmMsgInfo* const ptMsg );
    
    void RegisterToMcuAgent(u32 dwDstNode);
    void PostGetCfgMsg( u16 wEvent, u8 * const pbyContent, u16 wLen );

    void FreeDataA(void);
    void FreeDataB(void);
	// �Ƚ�����ʱ���Ƿ񳬹�ָ�����
	BOOL32 Compare2TimeIsValid(struct tm* ptTimeA, struct tm* ptTimeB); 
private:
	TBrdPosition	m_tBoardPosition;    // ����λ�ã������㡢�ۡ�����
	u32	m_dwDstNode;    // �ڵ��
	u32	m_dwDstIId;		// Ŀ��ʵ����
private:
    u32 m_dwDstNodeB;   // �ڵ��
    u32 m_dwDstIIdB;    // Ŀ��ʵ����

private:
	time_t  m_tLastDisTimeA; // Mpc A �ϴζ�����ʱ��
	time_t  m_tLastDisTimeB; // Mpc B �ϴζ�����ʱ��
    u32 m_dwBoardIpAddr; // look 
    
    u8  m_byConnectNum;
#ifndef WIN32
#ifdef MMP
	TBrdMMPLedStateDesc m_tLedState;
#endif
#endif
	TBoardAlarm  m_atBoardAlarmTable[ MAXNUM_BOARD_ALARM ];  /*�澯��*/
	u32			 m_dwBoardAlarmTableMaxNo;  /* �澯���и澯�������*/
	//������
	BOOL    m_bIsTest;
	u16 	m_wLastEvent;
	u8	m_abyLastEventPara[20];
};

typedef zTemplate< CBoardAgent, BOARD_AGENT_MAX_INSTANCE, CMmpConfig, 0 > CBrdAgentApp;

extern CBrdAgentApp	g_cBrdAgentApp;	//�������Ӧ��ʵ��

API BOOL InitMmpAgent();

#endif  /* MMPAGENT_H */
