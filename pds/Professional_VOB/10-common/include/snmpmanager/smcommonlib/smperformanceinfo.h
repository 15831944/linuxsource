/*****************************************************************************
   ģ����      : KDVSNMPMANAGER
   �ļ���      : smperformanceinfo.h
   ����ļ�    : smperformanceinfo.cpp
   �ļ�ʵ�ֹ���: �����������Ϣ�����
   ����		   : ½����
   �汾        : V5.0  Copyright(C) 2009-2012 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾     �޸���      �޸�����
   2009/12/31  5.0      ½����      ����
******************************************************************************/
#ifndef KDV_SMPERFORMANCEINFO_H
#define KDV_SMPERFORMANCEINFO_H

#include "smmacro.h"

#ifdef WIN32
#pragma pack( push, 1 )
#ifndef PACKED
#define PACKED 
#endif
#endif //WIN32

#ifdef _LINUX_
#ifndef PACKED
#define PACKED  __attribute__((packed))
#endif // PACKED 
#endif //_LINUX_

//���¶�������ͳ������

const u8 KDV_STATISTIC_NOTHING = 0; //�澯
const u8 KDV_STATISTIC_CONFNUM = KDV_STATISTIC_NOTHING + 1; //mcu������
const u8 KDV_STATISTIC_MTNUM = KDV_STATISTIC_NOTHING + 2; //mcu�����ն���
const u8 KDV_STATISTIC_EQPUSERATE = KDV_STATISTIC_NOTHING + 3; //mcu��������Դʹ����
const u8 KDV_STATISTIC_CPURATE = KDV_STATISTIC_NOTHING + 4; //CPU����ͳ��
const u16 KDV_PFMINFO_BUFFER_MAX = 1000;

//���½ṹ���ڴ��䣬������
//����������������
typedef struct tagEqpStat
{
	u16 m_wStatEqpType;
	u16 m_wReserved;
	u16 m_wEqpUsed;
	u16 m_wEqpTotal;
}PACKED TEqpStat;

//���½ṹ���ڴ��䣬������
//����������������
typedef struct tagPerformanceInfo
{
	u32	m_wAudioLoseRate;//��16λΪ�����ʣ���16λΪ�ܰ���
	u32	m_wVideoLoseRate;//��16λΪ�����ʣ���16λΪ�ܰ���
	u32	m_dwMemory;//��16λΪʹ��������16λΪ����
	u16	m_wMtNum;
	u16	m_wConfNum;
	u32 m_dwAuthMtNum; //��Ϊ���֣���16λ������ʹ��MT��Ȩ������16λ��MT��Ȩ��
	u16	m_wMachTemperature;
	u16	m_wCpuRate;
	u16	m_wEqpTypeNum;
	u16	m_wReserved;
	TEqpStat m_atEqpStat[SM_STAT_EQP_TYPE_MAX];
}PACKED TPerformanceInfo;

//���½ṹ���ڴ��䣬������
//�豸��������Ϣ
typedef struct tagDevPfmInfo
{
	u16 m_wDeviceType;
	s8  m_achAlias[SM_MAX_ALIAS_LEN + 4];//�豸����
	TPerformanceInfo m_tPfmInfo;
}PACKED TDevPfmInfo;

/*====================================================================
����  : CDevPfmInfoBase
����  : ��ȡ��ǰ�豸������Ϣ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
1/11/2010   5.0			½����			����
====================================================================*/
class CDevPfmInfoBase
{
public:
	//���캯��
	CDevPfmInfoBase();

	//�˺�����������������ܵ�buffer��Ľ���
	CDevPfmInfoBase(const s8* pszBuffer, u32 dwSize);
	virtual ~CDevPfmInfoBase();
	
public:

	//�豸���ͻ�ȡ������
	//ע����8λΪMainType, ��8λΪSubType
	u16 GetDeviceType();
	u8 GetDeviceMainType();
	u8 GetDeviceSubType();
	void SetDeviceType(u16 wDeviceType);

	//�����Ļ�ȡ������
	const s8* GetAlias();
	void SetAlias(const s8* pszAlias, u32 dwLen);

	//��ȡ��������
	//ע�����tPfmInfoΪ������
	const TDevPfmInfo& GetDevPfmInfo();	
	void SetDevPfmInfo(const TDevPfmInfo& tPfmInfo);
	void SetPfmInfobase(const TPerformanceInfo& tPfmInfobase);

	//�˺��������������紫��ķ�װbuffer
	virtual u32 GetBuffer(s8 *pszBuffer, u32 dwBufferSize);

protected:
	//���캯��ֱ�ӳ�ʼ��������
	CDevPfmInfoBase(const CDevPfmInfoBase&);
	//����Ϊ���������ڣ��ڲ�ʵ�֣���ֹ�ⲿ���á�
	CDevPfmInfoBase& operator=(const CDevPfmInfoBase& cPfmInfo);
	void Copy(const CDevPfmInfoBase* pcPfmInfo);

protected:
	TDevPfmInfo m_tDevPfmInfo; //�����������
}PACKED;


//���½ṹ���ڴ��䣬������
//��ʷ���ܻ����洢��Ԫ
typedef struct tagPfmInfoBase
{
	u32 m_dwTime; //ʱ��
	u32 m_dwValue; //��Ӧ������ֵ, ��16λΪ��ʹ�ã���16λΪ��
}PACKED TPfmInfoBase;


//���½ṹ���ڴ��䣬������
typedef struct tagPfmInfoReq
{
	u8 m_byStatisticKind;//ͳ������
	u8  m_byEqpType;	//���������ʹ����ͳ�ƣ���Ҫ��д��������
	u32 m_dwStartTime;	//��ʼͳ��ʱ��
	u32 m_dwEndTime;	//����ͳ��ʱ��
	u16 m_wHitsRate;	//�����ʣ�ȷָͳ��ʱ������Ҫ��������
}PACKED TPfmInfoReq;

/*====================================================================
����  : CDevPfmInfoBase
����  : ��ȡ�豸��ʷʱ���������������,����NP��SM��������ʱ
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
1/11/2010   5.0			½����			����
====================================================================*/
class CDevPfmInfoReq
{
public:
	CDevPfmInfoReq();
	
	//�˺�����������������ܵ�buffer��Ľ���
	CDevPfmInfoReq(const s8* pszBuffer, u32 dwSize);
	//���麯�������������ռ��ұ��̳У���������������
	~CDevPfmInfoReq();

	//ͳ������Ļ�ȡ������
	void SetStatisticKind(u8 byKind);
	u8 GetStatisticKind();

	void SetStatEqpType(u8 byStatEqpType);
	u8 GetStatEqpType();

	//ʱ������Ļ�ȡ������
	void SetTimeSpan(u32 dwStartTime, u32 dwEndTime);
	u32 GetStartTime();
	u32 GetEndTime();

	//�����ʵ�ͳ�Ƽ�����
	void SetHitsRate(u16 wHitsRate);
	u16 GetHitsRate();

	//�˺��������������紫��ķ�װbuffer
	virtual u32 GetBuffer(s8 *pszBuffer, u32 dwBufferSize);

	const TPfmInfoReq* GetPfmInfoReq();

protected:
	//���캯��ֱ�ӳ�ʼ��������
	CDevPfmInfoReq(const CDevPfmInfoReq&);
	//����Ϊ���������ڣ��ڲ�ʵ�֣���ֹ�ⲿ���á�
	CDevPfmInfoReq& operator=(const CDevPfmInfoReq& cPfmInfoReq);
	void Copy(const CDevPfmInfoReq* pcPfmInfoReq);
private:
	//�ڲ���װ���洢Ϊ������
	TPfmInfoReq m_tPfmInfoReq;//��ȡ�豸���ܵ�Req��Ϣ
};

/*====================================================================
����  : CDevPfmInfoNotify
����  : �����豸��ʷʱ�����������,����SM��NP��������ʱ
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
1/11/2010   5.0			½����			����
====================================================================*/
class CDevPfmInfoNotify
{
public:
	//���캯��
	CDevPfmInfoNotify();
	
	//�˺�����������������ܵ�buffer��Ľ���
	CDevPfmInfoNotify(const s8* pszBuffer, u32 dwSize);
	//���麯�������������ռ��ұ��̳У���������������
	~CDevPfmInfoNotify();

public:

	u16 GetPfmInfoNum();

	//���ӻ�������������Ϣ
	BOOL32 Add(const TPfmInfoBase& tPfmInfo);

	//ͨ��������ȡ�豸��Ϣ
	//�������� 0 - KDV_PFMINFO_BUFFER_MAX(1000) - 1
	const TPfmInfoBase* GetPfmInfo(u32 dwIndex);
	
	//���buffer��Ϣ��m_wNum = 0
	void ClearBuffer();

	//�˺��������������紫��ķ�װbuffer
	virtual u32 GetBuffer(s8 *pszBuffer, u32 dwBufferSize);

protected:
	//���캯��ֱ�ӳ�ʼ��������
	CDevPfmInfoNotify(const CDevPfmInfoNotify&);
	//����Ϊ���������ڣ��ڲ�ʵ�֣���ֹ�ⲿ���á�
	CDevPfmInfoNotify& operator=(const CDevPfmInfoNotify& cPfmInfoNotify);
	void Copy(const CDevPfmInfoNotify* pcPfmInfoNotify);


private:
	u16 m_wNum; //���ܸ�����ȡbufferʱת��Ϊ������
	TPfmInfoBase m_atPfmInfo[KDV_PFMINFO_BUFFER_MAX]; //�豸����������Ϣbuffer
};

#ifdef WIN32
#ifdef PACKED
#pragma pack(pop)
#endif
#undef PACKED
#endif //WIN32

#endif
