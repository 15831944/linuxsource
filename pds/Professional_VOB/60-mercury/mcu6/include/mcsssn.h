/*****************************************************************************
   ģ����      : mcu_new
   �ļ���      : McsSsn.h
   ����ļ�    : McsSsn.cpp
   �ļ�ʵ�ֹ���: MCU�������̨�ỰӦ����ͷ�ļ�
   ����        : ����
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2002/07/25  0.9         ����        ����
******************************************************************************/

#ifndef _MCSSSN_H_
#define _MCSSSN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "usermanage.h"
#include "addrbook.h"

class CMcsSsnInst : public CInstance  
{

public:
	u32   m_dwMcsNode;				  //���ӵĻ������̨�ڵ��
	u32   m_dwMcsIp;                  //mcs��IP��ַ
	u32	  m_dwMcsIId;                 //Mcs��IId
	s8    m_achUser[MAXLEN_PWD];      //������û���

protected:
	enum 
	{ 
		STATE_IDLE,
		STATE_NORMAL,
		STATE_INVALID
	};

	CConfId cCurConfId;
	
	BOOL32 SendMsgToMcs( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0, BOOL32 bMsFilter = TRUE );
    void AddrEntryOut2In(CAddrEntry *pcAddrIn, TMcuAddrEntry *ptAddrOut, u8 byNum=1);
    void AddrGroupOut2In(CAddrMultiSetEntry *pcGroupIn, TMcuAddrGroup *ptGroupOut, u8 byNum=1);
	void AddrEntryV1Out2In(CAddrEntry *pcAddrIn, TMcuAddrEntryV1 *ptAddrOut, u8 byNum=1);
	void AddrGroupV1Out2In(CAddrMultiSetEntry *pcGroupIn, TMcuAddrGroupV1 *ptGroupOut, u8 byNum=1);

    void AddrEntryIn2Out(TMcuAddrEntry *ptAddrIn, TADDRENTRY *ptAddrOut);
    void AddrGroupIn2Out(TMcuAddrGroup *ptGroupIn, TADDRMULTISETENTRY *ptGroupOut);

    //N+1�û�����û�������
    void ProcNPlusGrpInfo( const CMessage * pcMsg );
    void ProcNPlusUsrInfo( const CMessage * pcMsg );

public:
	//���ﴦ���ɲ��Ե��û�����
	void DaemonProcIntTestUserInfo( CMessage *pcMsg );
	//������ҵ����	
	void ProcReoOverFlow( const CMessage * pcMsg );
	void ProcMcsMcuDaemonConfMsg( const CMessage * pcMsg );
	void ProcMcsMcuMsg( const CMessage * pcMsg );

    // ����MCU��Ӧ,������Ϣ���������Ƿ�Ҫ�����û�����й���
	void ProcMcuMcsMsg( const CMessage * pcMsg, BOOL32 bCheckGrp );
	void ProcMcsMcuStopPlayReq( const CMessage * pcMsg );
	void ProcMcsMcuGetMcuStatusReq( const CMessage * pcMsg );
	void ProcMcsMcuGetAllMtStatusReq( const CMessage * pcMsg );
	void ProcMcsDisconnect( const CMessage * pcMsg );
	void ProcMcsMcuConnectReq( const CMessage * pcMsg );
	void ProcMcsInquiryMcuTime( const CMessage * pcMsg );
    void ProcMcsMcuChgSysTime( const CMessage * pcMsg );
	void ProcUserManagerMsg( const CMessage * pcMsg );
    void ProcAddrBookMsg( const CMessage * pcMsg );
    void ProcMcuCfgMsg( const CMessage *const pcMsg);    
    void ProcMcuCfgRsp( const CMessage * pcMsg);
    void ProcMcuCfgFileMsg( const CMessage * pcMsg);    // ���������ļ�������
    void ProcMcsMcuGetMsStatusReq( const CMessage * pcMsg );
	void ProcMcuUnitTestMsg( const CMessage * pcMsg );
    void DaemonProcNPlusUsrGrpNotif( const CMessage * pcMsg );
    
    void ProcMcsNPlusReq( const CMessage * pcMsg );
    void ProcNPlusMcsRsp( const CMessage * pcMsg );

    void DaemonProcPowerOn( void );
	void DaemonProcAddrInitialized( const CMessage * pcMsg );	
	void DaemonProcAppTaskRequest( const CMessage * pcMsg );
	void DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp );
	void InstanceEntry( CMessage * const pcMsg );

    //���Զ������
    void McsMcuRebootCmd( const CMessage * const pcMsg );

    // ɾ���û�
    void ProcMcuDelUser( CMessage * const pcMsg );
	void InstanceDump( u32 param = 0 );
	//ץ�����
#if defined(_8KH_) ||  defined(_8KE_) || defined(_8KI_)
	void ProcMcsMcuStartNetCapCMD(const CMessage* pcMsg);//mcs��ʼץ��
	
	void ProcMcsMcuStoptNetCapCmd(const CMessage* pcMsg);//mcsֹͣץ��֪ͨ
	
	void ProcMcsMcuGetNetCapStatusReq(const CMessage* pcMsg);//mcs��ȡ��ǰץ��״̬����

	void NotifyAllMcsCurNetCapStatus();

	void ProcMcsMcuOpenDebugModeCmd( const CMessage* pcMsg );

	void ProcMcsMcuStopDebugModeCmd( const CMessage* pcMsg );

	void NotifyAllMcsCurDebugMode();

	void ProcMcsMcuGetDebugModeReq(const CMessage* pcMsg);
#endif

	CMcsSsnInst();
	virtual ~CMcsSsnInst();
};

class CMcsSsn
{
public:
    // �������ʵ���û���������Id���±�=ʵ����-1 
    static u8 m_abyUsrGrp[MAXNUM_MCU_MC + MAXNUM_MCU_VC];

public:
    // ����McSsn���û���
    static void SetUserGroup( u16 byInsId, u8 byGrpId ) ;
    // ��ȡMcSsn���û���
    static u8   GetUserGroup( u16 byInsId );    
    // �����½
    static BOOL32 CheckLoginValid( CUsrManage& cUsrManageObj, CLoginRequest *pcUsr, u8 &byUserRole );


public:
	static BOOL32 SendMsgToMcsSsn( u16 wInstId, u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	static void   BroadcastToAllMcsSsn( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );

    // �㲥��Ϣ���ض����û��顣
    // ҵ���������ӿڷ��͵���Ϣ��һ�㶼���޷���McSsn�Ự�������û�������Ϣ����Ϣͷ��������Ϣ����
    // ������ÿ���û����������Ϣ
    // ��ͨ������Ϣֻ��Ҫ BroadcastToAllMcsSsn������McSsn�����û�����Ϣ���˼���
    void Broadcast2SpecGrpMcsSsn( u8 byGrdIp, u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );

	void SetCurrentDebugMode( emDebugMode emMode );
	emDebugMode GetCurrentDebugMode( void );

	void SetCurrentDebugModeInsId( u8 byInsId );
	u8 GetCurrentDebugModeInsId( void );

	CMcsSsn();
	virtual ~CMcsSsn();
private:
	u8 m_byCurrendDebugMode;
	u8 m_byCurrendDebugInsId;
};

typedef zTemplate< CMcsSsnInst, MAXNUM_MCU_MC + MAXNUM_MCU_VC, CMcsSsn > CMcsSsnApp;

extern CMcsSsnApp	g_cMcsSsnApp;	//�������̨�ỰӦ��ʵ��
extern CUsrManage   g_cUsrManage;
extern CAddrBook    *g_pcAddrBook;

#endif //_MCSSSN_H_
