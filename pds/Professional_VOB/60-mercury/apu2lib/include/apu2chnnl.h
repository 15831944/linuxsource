/*****************************************************************************
  ģ����      : apu2lib
  �ļ���      : apu2chnnl.h
  ����ļ�    : 
  �ļ�ʵ�ֹ���: apu2ͨ��������չ
  ����        : �ܼ���
  �汾        : V1.0  Copyright(C) 2009-2010 KDC, All rights reserved.
******************************************************************************/
#ifndef _APU2_CHNNL_H_
#define _APU2_CHNNL_H_

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
typedef struct tAudDecEncInfo
{
	//������Ϣ
	u8 byDecAudType;
	u8 byDecSoundChnlNum;
	u8 bySampleRate;
	TDoublePayload tDecPayload;
	//������Ϣ
	u8 byEncAudType;
	u8 byEncSoundChnlNum;
	TDoublePayload tEncPayload;
}TAudDecEncInfo;

	/************************************************************************/
	/*							CApu2Chnnl�ඨ��							*/
	/************************************************************************/
class CRcvCallBack;

class CApu2Chnnl:public CEqpChnnlBase
{
public:
	CApu2Chnnl(const CEqpBase* const pEqp, CEqpCfg* const pcCfg, const u16 wChnIdx);
	~CApu2Chnnl();
public:
	//ͨ����ʼ��
	BOOL32 Init();
	//ͨ������
	BOOL32 Destroy();

	/************************��Ϣ���*************************/
	BOOL32 OnMessage(CMessage* const pcMsg);

	/************************��Ϣ�ӿ�*************************/
	//������������
	void ProcStartMix(CMessage* const pcMsg);
	//ֹͣ��������
	void ProcStopMix(CMessage* const pcMsg);
	//���ӻ�����Ա����
	void ProcAddMixMember(CMessage* const pcMsg);
	//ɾ��������Ա����
	void ProcRemoveMixMember(CMessage* const pcMsg);
	//����������������
	void ProcStartVac(CMessage* const pcMsg);
	//ֹͣ������������
	void ProcStopVac(CMessage* const pcMsg);
	//����ǿ�ƻ�������
	void ProcForceActive(CMessage* const pcMsg);
	//ֹͣǿ�ƻ�������
	void ProcCancelForceActive(CMessage* const pcMsg);
	//��ʼ���뷢��
	void ProcMixSendNtf(CMessage* const pcMsg);
	//���û������
	void ProcSetMixDepth(CMessage* const pcMsg);
	//����������������ʱ��
	void ProcSetVacKeepTime(CMessage* const pcMsg);	
	//������Ա�ı�
	void ProcActiveMemChange(const u8 byActiveChnId);
	//�������Կ�������
	void ProcTestStartMix(CMessage* const pcMsg);
	//��������ֹͣ����
	void ProcTestStopMix();
	//�������Ը�λ
	void ProcTestRestore();
#ifndef _8KI_//8KI��֧�ֵ��
	//�������Ե��
	void ProcTestLed();
	//�������Ե�ƶ�ʱ����
	void ProcTimerLed(CMessage* const pcMsg);
#endif

	/************************���ܽӿ�*************************/
	//��������
	BOOL32  StartMixing();
	//ֹͣ����
	BOOL32  StopMixing();
	//ֹͣ��������
	BOOL32	StopVac();
	//���ӵ���������Ա
	BOOL32  AddMixMem(TMixMember tMixMember, u8 byChIdx);
	//�Ƴ�����������Ա
	BOOL32  RemoveMixMem(const u8 byMtId);
	//�Ƴ����л�����Ա
	BOOL32  RemoveAllMixMem();
	//���÷��Ͷ������
	BOOL32  SetSndObjectPara(u32 dwDstIp, u16 wDstPort, u8 byChnIdx);
	//���ý��ն������
	BOOL32  SetRcvObjectPara(const u8 byChnIdx);
	//�ն��Ƿ��ڻ���
	u8      IsMtInMix(const u8 byMtId);
	//����ͨ��������ȡ��ԱID
	u8      GetMtIdFromChnId(const u8 byChnId);
	//��ȡ��Ƶģʽ
	u8      GetAudioMode(const u8 byAudioType, u8 byAudioTrackNum = 1);	
	//��ȡ���緢�Ͷ���
	CKdvMediaSnd* GetMediaSnd(const u16 wIndex, BOOL32 bDMode);
	//��ȡý����ƶ���
    CKdvMixer* GetKdvMixer();
	//ת����Ƶ����
	u8		TransSimRate(const u8 bySimpleRate);
	//ֹͣͨ������
	BOOL32  Stop(const u8 byMode = MODE_BOTH);
	//��������Ϣ��ӡ
	void    Print();
	//ͨ����Ϣ��ӡ
	void	PrintChnlInfo();
	//�����������Ϣ��ӡ
	void	PrintMixState();
public:
	void InitAudChnlInfo();
	TAudDecEncInfo*	GetChnlInfo(const u8 byIdx);
	BOOL32 SetMixEncInfo(TAudioCapInfo *ptCapInfo, u8 bySndIdx);
	BOOL32 SetMixDecInfo(TAudioCapInfo *ptCapInfo, u8 byRcvIdx);
public:
	void SetMediaEncParam(u8 byChIdx, BOOL32 bNMode = FALSE);	/*���õײ�������*/
	void SetMediaDecParam(u8 byChIdx);							/*���õײ�������*/
	void SetLocNetParam(TMixMember *ptMixMember, u8 byChIdx);	/*���ý��յ�ַ����*/
	void SetAudResend(u8 bySndChlIdx);							/*���õײ����෢�Ͳ���*/
public:
	BOOL32 CheckConfId(CMessage* pcMsg);
	s8 *ReadMsgBody(CServMsg &cServMsg,  u8 &wMsgLen, u8 &byChIdx);
	void SendStartError(CServMsg &cMsg, u16 wRet = 0);
	void ProcMsgPrint(void);

private:
	//�����շ���ý�����	
	CKdvMixer	   m_cMixer;							//ý����ƶ���
	CKdvMediaRcv * m_pcAudioRcv;						//��Ƶ���ն���
	CKdvMediaSnd * m_pcAudioSnd;						//��Ƶ���Ͷ���
	CRcvCallBack * m_pcRcvCallBack;						//���ջص�����
	u16			   m_wAudRcvNum;						//��Ƶ���ն�����
	u16			   m_wAudSndNum;						//��Ƶ���Ͷ�����
	/*����*/
	TMediaEncrypt  m_tMediaEncrypt;
	TCapSupportEx  m_tCapSupportEx;
	u8			   m_byIsNeedPrs;
	u8			   m_byDepth;
	u8			   m_byQualityLvl;						// �������ʵȼ�
	TAudDecEncInfo m_atAudChnlInfo[MAXNUM_MIXER_CHNNL + MAXNUM_NMODECHN];	//��ͨ���������Ϣ
	//�������
	CConfId        m_cConfId;
	u8			   m_abyMtId[MAXNUM_MIXER_CHNNL];
	//����ָ��
	CApu2MixerCfg*const m_pcMixerCfg;
	//�������Ե��
	u8             m_abyLed[APU2_TEST_LEDNUM];
};

	/************************************************************************/
	/*							CApu2Chnnl�ඨ��							*/
	/************************************************************************/
class CApu2ChnStatus:public CChnnlStatusBase
{
public:
	enum EnMixState
	{
		IDLE = 0,
			READY,	
			MIXING,			//���ƻ��������ܻ���
			VAC,            //��������
			MIXING_VAC,	    //���ܻ��� + ��������
	};
public:
	CApu2ChnStatus();
	virtual~CApu2ChnStatus();
};

	/************************************************************************/
	/*							CRcvCallBack�ඨ��						    */
	/************************************************************************/
class CRcvCallBack
{
public:
    CApu2Chnnl*   m_pcMixerChn;    //�ص��Ļ�����
    u16           m_wChnId;        //�ص���ͨ���� 
	CRcvCallBack()
	{
		m_pcMixerChn = NULL;
		m_wChnId     = 0;
	}
	virtual~CRcvCallBack(){};
};

#endif