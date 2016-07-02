/*****************************************************************************
   ģ����      : Board Agent Basic
   �ļ���      : boardagentbasic.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ���������������ͨ��������
   ����        : �ܹ��
   �汾        : V1.0  Copyright(C) 2001-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2007/08/20  4.0         �ܹ��       ����
******************************************************************************/
#ifndef BOARDAGENTBASIC_H
#define BOARDAGENTBASIC_H

#include "osp.h"
#include "agentcommon.h"
#include "boardconfigbasic.h"

#define ALARM_MCU_OBJECT_LENGTH 2
#define ALARM_TABLE_OBJECT_LENGTH 5

// �澯�ṹ
typedef struct
{
	u32	dwBoardAlarmSerialNo;			//�澯���к�
    u32   dwBoardAlarmCode;				//�澯��
    u8   byBoardAlarmObjType;			//�澯��������
    u8	byBoardAlarmObject[ALARM_TABLE_OBJECT_LENGTH];		    //�澯����
	TBoardAlarmMsgInfo tBoardAlarm;         //״̬��Ϣ�ͽṹ
    BOOL  	bExist;						    //�ø澯�Ƿ���Ч
}TBoardAlarm
;

class CBBoardAgent:public CInstance
{
protected:
	enum 
	{ 
		STATE_IDLE,			//����
		STATE_INIT,			//��ʼ��
		STATE_NORMAL,		//����
	};

public:
	CBBoardAgent();
	virtual ~CBBoardAgent();

//protected:
	//ʵ����Ϣ������ں���������override
	void InstanceEntry(CMessage* const pMsg);
	void InstanceExit();
	//������Ϣ
	void ProcBoardPowerOn( CMessage* const pcMsg );
	//OSP����
	void ProcOspDisconnect( CMessage* const pcMsg );
	//���帴λ��Ϣ����
	void ProcBoardResetCmd( CMessage* const pcMsg );
	//�����Ǻ�MANAGER����Ϣ����
	//���ӳ�ʱ
//	void ProcBoardConnectManagerTimeOut( BOOL32 IsConnectA );
	void ProcBoardConnectManagerTimeOut( void );			// [6/1/2010 xliang] 
	BOOL32 ProcConnect();									// [6/1/2010 xliang] 
	//�ȴ�ע�ᳬʱ
	void ProcBoardRegisterTimeOut( BOOL32 bIsConnectA );
	//ע��Ӧ����Ϣ
	void ProcBoardRegAck( CMessage* const pcMsg );	
	//ע���Ӧ����Ϣ
	void ProcBoardRegNAck( CMessage* const pcMsg );
	//ȡ������Ϣ��Ϣ��ʱ�����ڴ���
	void ProcGetConfigTimeOut( CMessage* const pcMsg );
	//����Ӧ����Ϣ
	virtual void ProcBoardGetConfigAck( CMessage* const pcMsg ) = 0;
	//���÷�Ӧ����Ϣ
	virtual void ProcBoardGetConfigNAck( CMessage* const pcMsg ) = 0;
	//�澯ͬ������
	void ProcAlarmSyncReq( CMessage* const pcMsg );
	//ȡ��������汾��Ϣ����
	void ProcGetVersionReq( CMessage* const pcMsg );
	//ȡ�����ģ����Ϣ��Ϣ����
	void ProcGetModuleInfoReq( CMessage* const pcMsg );
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
	//������״̬��Ϣ����
	//�ڴ�״̬
	void ProcBoardMemeryStatus( CMessage* const pcMsg );
	//�ļ�ϵͳ״̬
	void ProcBoardFileSystemStatus( CMessage* const pcMsg );
	//ģ����Ϣ����
	void ProcBoardModuleStatus( CMessage* const pcMsg );

	//��������Ϣ
	void ProcBoardGetCfgTest( CMessage* const pcMsg );
	void ProcBoardGetAlarm( CMessage* const pcMsg );
	void ProcBoardGetLastManagerCmd( CMessage* const pcMsg );
	void TestingProcess( CMessage* const pcMsg );

//protected:
	//���ӹ������
//	BOOL32 ConnectManager(u32& dwMcuNode, u32 dwMpcIp, u16 wMpcPort);
	BOOL32 ConnectManager( BOOL32 bIsNodeA );
	//�ж��Ƿ���Ҫ���ӵ�MPC A/B
	BOOL32 IsNeedConnectNode( BOOL32 bIsNodeA );

	//������Ϣ���������
	BOOL32 PostMsgToManager( u16  wEvent, u8 * const pbyContent = NULL, u16  wLen = 0 );
	//�澯�б���������Ӧ�ĸ澯��Ϣ
    BOOL32 AddAlarm( u32 dwAlarmCode, u8 byObjType, u8 byObject[], TBoardAlarm *ptData );
	//�澯�б���ɾ����Ӧ�ĸ澯��Ϣ
    BOOL32 DeleteAlarm( u32 dwSerialNo );	
	//�澯�б��л�ȡ��Ӧ�ĸ澯��Ϣ
    BOOL32 FindAlarm( u32 dwAlarmCode, u8 byObjType, u8 byObject[], TBoardAlarm *ptData );
	//��������ø澯����Ϣ��Ϣ
	BOOL32 SetAlarmMsgInfo( u32 dwSerialNo, TBoardAlarmMsgInfo* const ptMsg );
		
    void FreeDataA(void);
    void FreeDataB(void);
    void RegisterToMcuAgent(u32 dwDstNode);
    void MsgRegAck(CMessage* const pcMsg );
	void brdagtlog(char * fmt, ...);

	void UpdateIpPort(u32 dwIp, u16 wPort, BOOL32 bUpdateA);

	//�����¶��쳣��������״̬֪ͨ
	void ProcBoardTempStatusNotif(CMessage* const pcMsg);
	//����CPUռ����״̬֪ͨ
	void ProcBoardCpuStatusNotif(CMessage* const pcMsg);

private:
	CBBoardConfig *m_pBBoardConfig;

	TBoardAlarm  m_atBoardAlarmTable[ MAXNUM_BOARD_ALARM ];  /*�澯��*/
	u32		 m_dwBoardAlarmTableMaxNo;  /* �澯���и澯�������*/
public:
	u8       m_byIsMpu2SimuMpu;			//�Ƿ�MPU2ģ��MPU
	u8		 m_byIsAnyBrdRegSuccess;	//ֵΪ0��ʾδע��ɹ���1��ʾ˫��������һ��ע��ɹ�
	u8       m_byRegBrdTypeFlagA;		//ע�ᵽMPCA�壬ֵΪ0��BRDID Aע�ᣬֵΪ1��BRDID Bע�ᣬ��������MCU��BRD TYPE���岻ͬ
	//Ŀǰ֧��MPU2ģ��MPU
	u8       m_byRegBrdTypeFlagB;		//ע�ᵽMPCB�壬ֵΪ0��BRDID Aע�ᣬֵΪ1��BRDID Bע�ᣬ��������MCU��BRD TYPE���岻ͬ
										//Ŀǰ֧��MPU2ģ��MPU

};

void AddBrdAgtEventStr();            // ��ӵ��������Ϣ�ַ��� [liaokang 2012/05/05]

#endif  /* BOARDAGENTBASIC_H */
