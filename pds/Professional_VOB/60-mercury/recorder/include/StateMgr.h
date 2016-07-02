/*****************************************************************************
   ģ����      : Recorder
   �ļ���      : StateMgr.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: Recorder��Server��StateMgr����
   ����        : 
   �汾        : V1.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2010/08/26  1.0         �� ��         ����
******************************************************************************/

#ifndef STATEMGR_H
#define STATEMGR_H

#include "osp.h"
#include "kdvtype.h"
#include "RecServ_Def.h"

// ����ǰ������
class CStateBase;
class CRecServInst;

class CStateMgr
{
public:
	/** 
	/* ����: ���캯��
	*/
	CStateMgr(CRecServInst * const pInst);

	/** 
	/* ����: ���캯��
	*/
	~CStateMgr();

public:
	/** 
	/* ����: ��ʼ������
	*/
	void Initialize(){}

	/** 
	/* ����: ��Ϣ��ں���
	*/
	RET_TYPE OnMessage(CMessage *const);

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
	/*	���ܣ���ȡ��ǰ��״̬
	/*  ˵��: Ϊ��ʹ����Ƶ�״̬ģʽ����������,�ش�������CInstance��NextState
	/*  ������dwState = ��һ��״̬��enumֵ
	/*  ����ֵ���������ֵ,�����RET_TYPE
	*/	
	RET_TYPE ChangeStateTo(const u32 &);

	/** 
	/*	���ܣ�ȡ��״̬����ǰ״̬
	/*  ��������
	/*  ����ֵ����ǰ״ֵ̬
	*/
    const u32 GetState();

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


	/** 
	/*	���ܣ��洢¼����ڵ���Ϣ
	/*  ������
	/*       dwNodeId : �ڵ��
	/*       dwInstId : mt��Instance�ţ���MAKEID(appid��instanceid)����
	/*  ����ֵ����
	*/
	void SetRecNodeInfo(const TNodeInfo& tNode);

	/** 
	/*	���ܣ��洢�ն˽ڵ���Ϣ
	/*  ������
	/*       dwNodeId : �ڵ��
	/*       dwInstId : mt��Instance�ţ���MAKEID(appid��instanceid)����
	/*  ����ֵ����
	*/
    void SetMTNodeInfo(const TNodeInfo& tNode);

public:
	/*	���ܣ���ȡCRecServInstʵ��
	/*  ��������
	/*  ����ֵ��CRecServInstʵ��ָ��
	*/	
	CRecServInst* GetOwnInst(){	return m_pServInst; }
	
private:
	CRecServInst*  m_pServInst;					// CRecServInstʵ��ָ��
	CStateBase* m_pcState;						// ״ָ̬��
};

#endif