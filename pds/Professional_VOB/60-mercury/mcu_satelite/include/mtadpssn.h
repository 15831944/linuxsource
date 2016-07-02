/*****************************************************************************
   ģ����      : �ն�����Ự
   �ļ���      : mtadpssn.h
   ����ļ�    : mtadpssn.cpp
   �ļ�ʵ�ֹ���: �ն�����Ự����
   ����        : ������
   �汾        : V3.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/11/08  3.0         ������        ����
******************************************************************************/
#ifndef __MTADPSSN_H_
#define __MTADPSSN_H_

#include "osp.h"

class CMtAdpSsnInst : public CInstance  
{
	enum 
	{ 
		STATE_IDLE = 0,  //����
		STATE_NORMAL,    //����
	};
	
protected:
	//����Ϣ��MtAdp
	void SendMsgToMtAdp( u16 wEvent, u8 * const pbyMsg, u16 wLen );
    //����Ϣ��323MtAdp
    void SendMsgToH323MtAdp( CServMsg &cMsg );
	//����Ϣ��Mcu
	void SendMsgToMcu( u16 wEvent, u8 * const pbyMsg, u16 wLen );

public:
	//��ͨʵ�����
	void InstanceEntry( CMessage * const pcMsg );
	//����MtAdpע����Ϣ
	void ProcMtAdpRegisterReq( CMessage * const pcMsg );
	//����Mp����
	void ProcMtAdpDisconnect( CMessage * const pcMsg );	
	//�����ն˵�MCU����Ϣ
	void ProcMtAdpMcuMsg( CMessage * const pcMsg );
	//����MCU���ն˵���Ϣ
	void ProcMcuMtAdpMsg( CMessage * const pcMsg );    
    
	//����MCU������Ϣ
	void ProcMcuCasCadeMsg( CMessage * const pcMsg );
    
    void ProcMtAdpGetMsStatusReq( CMessage * const pcMsg );
	
	//�����ն˵�MCU��GK��Ϣ
	void ProcMtAdpMcuGKMsg( CMessage * const pcMsg );
	//����MCU���ն˵�GK��Ϣ
	void ProcMcuMtAdpGKMsg( CMessage * const pcMsg );

	void DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp );
	void DaemonProcAppTaskRequest( const CMessage * pcMsg );

public:
	CMtAdpSsnInst();
	virtual ~CMtAdpSsnInst();

private:
	u32 m_dwMtAdpNode;       //��ӦMtAdp�ڵ��
	u32 m_dwMtAdpAppIId;     //ע���MtAdp���MtAdpʵ��APP��
	u8  m_byMtAdpId;         //MtAdp��
	u8  m_byMtAdpAttachMode; //������ʽ
	u8  m_byProtocolType;    //�����Э������//H323��H320��SIP
	u8  m_byMaxMtNum;        //�����֧������ն���
};


class CMtAdpConfig
{
public:
	BOOL32 SendMsgToMtAdpSsn( u8 byMpId, u16 wEvent, CServMsg & cServMsg );
	BOOL32 SendMsgToMtAdpSsn( const TMt & tDstMt, u16 wEvent, CServMsg & cServMsg );
	BOOL32 BroadcastToAllMtAdpSsn( u16 wEvent, const CServMsg & cServMsg );	

public:
	CMtAdpConfig();
	virtual ~CMtAdpConfig();
};

typedef zTemplate< CMtAdpSsnInst, MAXNUM_DRI, CMtAdpConfig, sizeof( u8 ) > CMtAdpSsnApp;

extern  CMtAdpSsnApp g_cMtAdpSsnApp;

#endif
