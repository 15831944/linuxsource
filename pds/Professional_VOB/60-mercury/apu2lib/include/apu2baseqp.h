/*****************************************************************************
  ģ����      : apu2lib
  �ļ���      : apu2baseqp.h
  ����ļ�    : 
  �ļ�ʵ�ֹ���: apu2���������չ
  ����        : ������
  �汾        : V1.0  Copyright(C) 2009-2010 KDC, All rights reserved.
******************************************************************************/
#ifndef _APU2BASEQP_H_
#define _APU2BASEQP_H_

#include "eqpbase.h"
#include "apu2baschnnl.h"
#include "evapu2.h"
#include "eqpautotest.h"

#define BAS_CONNETC_TIMEOUT             (u16)(3 * 1000)     // ���ӳ�ʱֵ3s

	/************************************************************************/
	/*							CApu2BasEqp�ඨ��								*/
	/************************************************************************/
class CApu2BasEqp:public CEqpBase
{
public:
	CApu2BasEqp(CEqpMsgSender* const pcMsgSender, CEqpCfg* const pcCfg, const u16 wChnNum);
	~CApu2BasEqp();

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
	BOOL32	OnBasPrint(u16 wMsgID);

	/************************ҵ��ӿ�***********************/
	//�����ʼ��
	BOOL32  Init();
	//��������
	BOOL32  Destroy();
	//��ȡʵ����ͨ��
	CApu2BasChnnl* GetChnnl(const u16 wChnIdx);
	//����ͨ��״̬
	void	SendChnlNotify(const u8 byState, const u16 wChIdx, BOOL32 bIsMainBrd);
	//�����豸״̬
	void	SendEqpStusNotify(void);

private:
	BOOL32 m_bInited;
};
#endif