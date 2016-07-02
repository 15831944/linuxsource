/*****************************************************************************
   ģ����      : Recorder
   �ļ���      : RecServInst.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: Recorder��Server��Instance����
   ����        : 
   �汾        : V1.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2010/08/26  1.0         �� ��         ����
******************************************************************************/


#ifndef RECSERVINST_H
#define RECSERVINST_H

#include "osp.h"
#include "mcustruct.h"
#include "RecServ_Def.h"
#include "StateMgr.h"


class CRecServInst : public CInstance
{
	// Ϊ����������ʵ����ʹ��CInstance�ı�����ԱNextState���ؽ�������Ϊ��Ԫ
    friend class CRecServInst;
	// CStateMgr���Բ���CRecServInst��˽�г�Ա
	friend class CStateMgr;
public:
	/** 
	/*	����: ���캯��
	*/
    CRecServInst();

	/** 
	/*	����: ��������
	*/
	~CRecServInst();
    
public:
	/** 
	/*	����: �ػ�ʵ����Ϣ���
	*/
    void DaemonInstanceEntry( CMessage * const pMsg, CApp* pcApp);

    /** 
	/*	����: ��ͨʵ����Ϣ���
	*/
	void InstanceEntry( CMessage * const pcMsg );

public:
	/** 
	/*	���ܣ���ȡ��ǰ��״̬
	/*  ��������
	/*  ����ֵ����ǰ״ֵ̬
	*/
	const u32 GetState() const {return m_pcStateMgr->GetState();}


	/** 
	/*	���ܣ���ȡ��ǰ��״̬
	/*  ˵��: Ϊ��ʹ����Ƶ�״̬ģʽ����������,�ش�������CInstance��NextState
	/*  ������dwState = ��һ��״̬��enumֵ
	/*  ����ֵ���������ֵ,�����RET_TYPE
	*/
	const u32 NextState(u32 dwState) const {return m_pcStateMgr->ChangeStateTo(dwState);}

	/** 
	/*	���ܣ�����Ϣ�����ն�
	/*  ������
	/*          wEvent   = ��Ϣ��
	/*          pContent = ��Ϣ����
	/*          dwLength = ��Ϣ���ݳ���
	/*  ����ֵ���������ֵ,�����RET_TYPE
	*/
	RET_TYPE PostMsgToMt(const u16 wEvent, const u8* pContent = NULL, const u16 dwLength = 0);
	
	/** 
	/*	���ܣ�����Ϣ����¼����ػ�ʵ��
	/*  ������
	/*          wEvent   = ��Ϣ��
	/*          pContent = ��Ϣ����
	/*          dwLength = ��Ϣ���ݳ���
	/*  ����ֵ���������ֵ,�����RET_TYPE
	*/
    RET_TYPE PostMsgToRecDaemInst(const u16 wEvent, const u8* pContent = NULL, const u16 dwLength = 0);

	/** 
	/*	��    �ܣ�����Ϣ����¼�����¼��ͨ��ʵ��
	/*  ǰ������: �Ѿ����ͨ����������,���Ѿ�����¼��
	/*  ��    ����
	/*          wEvent   = ��Ϣ��
	/*          pContent = ��Ϣ����
	/*          dwLength = ��Ϣ���ݳ���
	/*  ����ֵ���������ֵ,�����RET_TYPE
	*/
	RET_TYPE PostMsgToRecChnInst(const u16 wEvent, const u8* pContent = NULL, const u16 dwLength = 0);

	/** 
	/*	���ܣ��洢¼����ڵ���Ϣ
	/*  ������
	/*       dwNodeId : �ڵ��
	/*       dwInstId : mt��Instance�ţ���MAKEID(appid��instanceid)����
	/*  ����ֵ����
	*/
	void SetRecNodeInfo(const TNodeInfo& tNode)
	{
		m_tRecNodeInfo.m_dwAppId = tNode.m_dwAppId;
		m_tRecNodeInfo.m_dwNodeId = tNode.m_dwNodeId;
	}

	/** 
	/*	���ܣ��洢�ն˽ڵ���Ϣ
	/*  ������
	/*       dwNodeId : �ڵ��
	/*       dwInstId : mt��Instance�ţ���MAKEID(appid��instanceid)����
	/*  ����ֵ����
	*/
    void SetMTNodeInfo(const TNodeInfo& tNode)
	{
		m_tMTNodeInfo.m_dwAppId = tNode.m_dwAppId;
		m_tMTNodeInfo.m_dwNodeId = tNode.m_dwNodeId;
	}

private:        
    /** 
	/*	���ܣ��ػ�ʵ�����ն�ע����Ӧ����
	*/
    void OnMTRegisterReq(CMessage * const, CApp*);

    /** 
	/*	���ܣ��ػ�ʵ���д�ӡ¼���������״̬����
	*/
    void OnDaemShowStatus(CMessage * const, CApp*);

    /** 
	/*	���ܣ��ػ�ʵ����ת����¼�����Nodity��Ϣ
	*/
    void OnDaemNodifyEvent(CMessage * const, CApp*);

    /** 
	/*	���ܣ���ͨʵ���ն����ӵ���ʱ����Ӧ����
	*/
    void OnDisconnectMsg(CMessage * const);

private:
    /** 
	/*	���ܣ��ػ�ʵ����CApp�л�ȡһ�����е���ͨʵ��
	/*  ������CApp ����appʵ��
	/*  ����ֵ��CInstance* �� ָ�򱾵ص�ĳ������Instance��ָ��
	*/
    CRecServInst* GetIdleInst(CApp*);

	/** 
	/*	���ܣ�����CStateMgr����ָ��
	*/
	CStateMgr* GetStateMgr() const {return m_pcStateMgr;}

	/** 
	/*	���ܣ���֤�����ն˵������Ƿ�Ϸ�
	/*  ������pcMsg ��CMessage
	/*  ����ֵ��������ն��Ѿ�ע��,��True,����False
	*/
	BOOL32 IsMtReqValid(CMessage * const pcMsg);

	/** 
	/*	���ܣ���֤����¼����������Ƿ�Ϸ�
	/*  ������pcMsg ��CMessage
	/*  ����ֵ���Ϸ�����True,����False
	*/
	BOOL32 IsRecMsgValid(CMessage * const pcMsg);
    
private:
    CServMsg m_cServMsg;				// �����Recorder������CServMsg��Ϣͷ��Ϣ
    TNodeInfo m_tMTNodeInfo;			// �ն˽ڵ���Ϣ
	TNodeInfo m_tRecNodeInfo;			// ¼����Ľڵ���Ϣ
	CStateMgr* m_pcStateMgr;			// ״̬����ڶ���

private:
// GETLOCALAPPID�궨��, ��ñ�ʵ����ȫ�ֺ�
#define GETLOCALAPPID MAKEIID(GetAppID(), GetInsID())
};

// CRecServApp����
typedef zTemplate< CRecServInst, MAXNUM_RECORDER_CHNNL > CRecServApp;

#endif