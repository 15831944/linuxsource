/*****************************************************************************
  ģ����      : apu2lib
  �ļ���      : apu2eqp.h
  ����ļ�    : 
  �ļ�ʵ�ֹ���: apu2���������չ
  ����        : �ܼ���
  �汾        : V1.0  Copyright(C) 2009-2010 KDC, All rights reserved.
******************************************************************************/
#ifndef _APU2EQP_H_
#define _APU2EQP_H_

#include "eqpbase.h"
#include "apu2chnnl.h"
#include "evapu2.h"
#include "eqpautotest.h"

#define MIX_CONNETC_TIMEOUT             (u16)(3 * 1000)     // ���ӳ�ʱֵ3s

	/************************************************************************/
	/*							CApu2Eqp�ඨ��								*/
	/************************************************************************/
class CApu2Eqp:public CEqpBase
{
public:
	CApu2Eqp(CEqpMsgSender* const pcMsgSender, CEqpCfg* const pcCfg, const u16 wChnNum);
	~CApu2Eqp();

public:
	/***********************��Ϣ���*************************/
	virtual BOOL32 OnMessage(CMessage* const pcMsg);

    /***********************��Ϣ�ӿ�*************************/
	//ע��Mcu�ɹ�����
	BOOL32  OnRegisterMcuAck(CMessage *const pcMsg);
	//ע��Mcuʧ�ܴ���
	BOOL32  OnRegisterMcuNack(CMessage *const pcMsg);
	//�޸ķ��͵�ַ
	BOOL32	OnModifyMcuRcvIp(CMessage* const pcMsg);
	//��ӡ
	BOOL32  OnMixPrint(u16 wMsgID);

	/************************ҵ��ӿ�***********************/
	//�����ʼ��
	BOOL32  Init();
	//��������
	BOOL32  Destroy();
	//��ȡʵ����ͨ��
	CApu2Chnnl* GetChnnl(const u16 wChnIdx);

private:
	BOOL32 m_bInited;
};

/************************************************************************/
/*							CTestEqp�ඨ��								*/
/************************************************************************/
class CTestEqp:public CEqpBase
{
public:
	CTestEqp(CEqpMsgSender* const pcMsgSender, CEqpCfg* const pcCfg, const u16 wChnNum);
	~CTestEqp();
	
public:
	/***********************��Ϣ���*************************/
	virtual BOOL32 OnMessage(CMessage* const pcMsg);
	
    /***********************��Ϣ�ӿ�*************************/
	//ע��Mcu�ɹ�����
	BOOL32  OnRegisterServerAck(CMessage *const pcMsg);
	//ע��Mcuʧ�ܴ���
	BOOL32  OnRegisterServerNack(CMessage *const pcMsg);
	//��ӡ
	BOOL32  OnMixPrint(u16 wMsgID);
	//��������ע���������ʱ����
	virtual BOOL32 OnRegisterServerTimer();
	/************************ҵ��ӿ�***********************/
	//�����ʼ��
	BOOL32  Init();
	//��������
	BOOL32  Destroy();
	//��ȡʵ����ͨ��
	CApu2Chnnl* GetChnnl(const u16 wChnIdx);
	
private:
	BOOL32 m_bInited;
};
#endif