/*****************************************************************************
  ģ����      : apu2lib
  �ļ���      : apu2baschnnl.h
  ����ļ�    : 
  �ļ�ʵ�ֹ���: apu2ͨ��������չ
  ����        : ������
  �汾        : V1.0  Copyright(C) 2009-2010 KDC, All rights reserved.
******************************************************************************/
#ifndef _APU2BASCHNNL_H_
#define _APU2BASCHNNL_H_

#include "eqpbase.h"
#include "apu2cfg.h"
#include "kdvmixer_apu2.h"
#include "evapu2.h"
#include "kdvmedianet.h"
#include "mcuinnerstruct.h"

#ifdef WIN32
	#include "winbrdwrapper.h"
#endif

//�������Ϣ
typedef struct tAudBasInfo
{
	//������Ϣ
	u8 byDecAudType;
	u8 byDecSoundChnlNum;
	u8 bySampleRate;
	TDoublePayload tDecPayload;
	//������Ϣ
	u8 abyEncAudType[MAXNUM_APU2BAS_OUTCHN];
	u8 abyEncSoundChnlNum[MAXNUM_APU2BAS_OUTCHN];
	TDoublePayload atEncPayload[MAXNUM_APU2BAS_OUTCHN];
}TAudBasInfo;

	/************************************************************************/
	/*							CApu2BasChnnl�ඨ��							*/
	/************************************************************************/
class CBasRcvCallBack;

class CApu2BasChnnl:public CEqpChnnlBase
{
public:
	CApu2BasChnnl(const CEqpBase* const pEqp, CEqpCfg* const pcCfg, const u16 wChnIdx);
	~CApu2BasChnnl();
public:
	//����
	BOOL32 Init();
	BOOL32 Destroy();
	BOOL32 OnMessage(CMessage* const pcMsg);

	//��Ϣ����
	void ProcStartAdaptReq(CMessage* const pcMsg);		/*��������������*/
	void ProcChgAdaptReq(CMessage* const pcMsg);		/*�����޸Ĳ�������*/
	void ProcStopAdaptReq(CMessage* const pcMsg);		/*����ֹͣ��������*/

	//���ܽӿ�
	u16		OnStartAdaptReq(void);						/*������*/
	u16		OnChgAdaptReq(const u8 byChIdx, u8 *abyEncChg);				/*�Ĳ���*/
	BOOL32 	OnStopAdaptReq(void);						/*������*/
	u16		AddBasMember(const u8 byChIdx);				/*��ͨ��*/
	u16		RemoveBasMember(const u8 byChIdx);			/*ɾͨ��*/
	u8		GetAudioMode(const u8 byAudioType, u8 byAudioTrackNum = 1);		/*��ȡ��Ƶģʽ*/
	CKdvAudAdaptor* GetKdvBas();						/*��ȡý�����*/
	u8		TransSimRate(const u8 bySimpleRate);		/*ת��������*/
	BOOL32  Stop(const u8 byMode = MODE_BOTH);			/*ͨ��ֹͣ*/
	void    Print();
	void	PrintChnlInfo();

public:
	/*m_atAudBasInfo*/
	void	InitChnlInfo(TAudBasInfo &tAudChnlInfo);	/*��ʼ��ͨ����Ϣ*/
	tAudBasInfo* GetChnlInfo(const u8 byIdx);			/*��ȡͨ����Ϣ*/
	BOOL32	SetBasEncInfo(TAudAdaptParam *pAptParam, u8 byRcvIdx, u8 bySndIdx);	/*����ͨ��������Ϣ*/
	BOOL32	SetBasDecInfo(TAudioCapInfo *pAudCapInfo, u8 byRcvIdx);	/*����ͨ��������Ϣ*/

	/*m_acConfId*/
	CConfId GetConfId(const u16 wIndex);				/*��ȡͨ����������*/

	/*m_abChnlOccupy*/
	BOOL32	GetChnlOccupy(u8 byChIdx);					/*��ȡͨ��ռ�����*/

	/*m_pcAudioRcv*/
	void CreateAudioRcv(u16 wNum);		/*ʵ�������ն���*/

	/*m_pcAudioSnd*/
	void CreateAydioSnd(u16 wNum);		/*ʵ�������Ͷ���*/
	CKdvMediaSnd* GetMediaSnd(const u32 wIndex);		/*��ȡ���Ͷ���*/

public:
	void SetMediaEncParam(u8 byChIdx);	/*���õײ�������*/
	void SetMediaDecParam(u8 byChIdx);	/*���õײ�������*/
	void SetAudResend(u8 bySndChlIdx);	/*���õײ����෢�Ͳ���*/
	BOOL32  SetSndObjectPara(u32 dwDstIp, u16 wDstPort, u8 bySndChnIdx);	/*���÷��Ͷ���*/
	BOOL32  SetRcvObjectPara(const u8 byChnIdx);							/*���ý��ն���*/
	BOOL32	SetRcvNetParam(const u8 byChnIdx);								/*���õ�ַ����*/

public:
	void SendMsgToMcu(CServMsg &cServMsg, u16 wRet = ERR_BAS_NOERROR, u8 byChIdx = 0);
	s8	*ReadMsgBody(CServMsg &cServMsg,  u8 &wMsgLen, u8 &byChIdx);
	void VariablesClear(u8 byClearLevel = 0);
	void ProcMsgPrint(u8 byChIdx);

private:
	CKdvAudAdaptor m_cBas;								/*ý����ƶ���*/
	CKdvMediaRcv * m_pcAudioRcv;						/*��Ƶ���ն���*/
	CKdvMediaSnd * m_pcAudioSnd;						/*��Ƶ���Ͷ���*/
	CBasRcvCallBack *m_pcRcvCallBack;					/*���ջص�����*/
	/*�������*/
	TMediaEncrypt  m_tMediaEncrypt;						/*���ܲ���*/
	BOOL32		   m_bIsNeedPrs;						/*�Ƿ��ش�*/

	u8			   m_byQualityLvl;						/*���ʵȼ�*/
	TAudBasInfo	   m_atAudBasInfo[MAXNUM_APU2_BASCHN];	/*��ͨ���������Ϣ*/
	CConfId        m_acConfId[MAXNUM_APU2_BASCHN];		/*ͨ����������*/
	BOOL32		   m_abChnlOccupy[MAXNUM_APU2_BASCHN];	/*ռ��ͨ�����ն�����*/

	/*����ָ��*/
	CApu2BasCfg*const m_pcBasCfg;
};

	/************************************************************************/
	/*							CApu2BasChnStatus�ඨ��						*/
	/************************************************************************/
class CApu2BasChnStatus : public CChnnlStatusBase
{
public:
	enum EnMixState
	{
		IDLE = 0,
		READY,	
		RUNNING,	
	};
public:
	CApu2BasChnStatus();
	virtual~CApu2BasChnStatus();
};

	/************************************************************************/
	/*							CBasRcvCallBack�ඨ��						*/
	/************************************************************************/
class CBasRcvCallBack
{
public:
    CApu2BasChnnl*		m_pcBasChn;    //�ص���������
    u16					m_wChnId;      //�ص���ͨ���� 
	CBasRcvCallBack()
	{
		m_pcBasChn = NULL;
		m_wChnId     = 0;
	}
	virtual~CBasRcvCallBack(){};
};

#endif