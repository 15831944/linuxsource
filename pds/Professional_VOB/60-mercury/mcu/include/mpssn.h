/*****************************************************************************
   ģ����      : MP�Ựģ��
   �ļ���      : mpssn.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: MP�Ựģ��ʵ������
   ����        : ������
   �汾        : V0.1  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/07/11  0.1         ������        ����
******************************************************************************/
#ifndef __MPSSN_H_
#define __MPSSN_H_

class CMpSsnInst : public CInstance  
{
	enum 
	{ 
		STATE_IDLE,      //����
		STATE_NORMAL,    //����
	};
	
protected:
	//����Ϣ��Mp
	void SendMsgToMp( u16 wEvent, u8 * const pbyMsg, u16 wLen );
	//����Ϣ��Mcu
	void SendMsgToMcu( u16 wEvent, u8 * const pbyMsg, u16 wLen );

public:
	//��ͨʵ�����
	void InstanceEntry( CMessage * const pcMsg );
	//����Mpע����Ϣ
	void ProcMpRegisterReq( CMessage * const pcMsg);
	//����Mp����
	void ProcMpDisconnect( CMessage * const pcMsg);

	// MP ȡ��������״̬
	void ProcMpGetMsStatusReq(CMessage* const pcMsg);
	void DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp );
	void DaemonProcAppTaskRequest( const CMessage * pcMsg );

	// [12/17/2009 xliang] add new functions
	BOOL32	IsMpReged( const u32 &dwMpIp );			//�ݲ���
    u8		GetIdleInst( void );					
	
    void	SetMpIp( u32 dwMpIp ) { m_dwMpIp = htonl(dwMpIp); }
    u32		GetMpIp( void ) { return ntohl(m_dwMpIp); }

public:
	CMpSsnInst();
	virtual ~CMpSsnInst();

private:
	u32 m_dwMpNode;       //��ӦMp�ڵ��
	u32 m_dwMpIId;        //Mp��App��IId
	u8  m_byMpId;         //Mp��
	u8  m_byMpAttachMode; //������ʽ
	u32 m_dwMpIp;         //��ӦMP��IP : ������	// [12/17/2009 xliang] 
};


class CMpConfig
{
public:
	void SendMsgToMpSsn( u8 byMpId, u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	void BroadcastToAllMpSsn(u16 wEvent, u8 * const pbyMsg, u16 wLen);

public:
	CMpConfig();
	virtual ~CMpConfig();
};

typedef zTemplate< CMpSsnInst, MAXNUM_DRI+1, CMpConfig, sizeof( u8 ) > CMpSsnApp;

extern  CMpSsnApp g_cMpSsnApp;

#endif
