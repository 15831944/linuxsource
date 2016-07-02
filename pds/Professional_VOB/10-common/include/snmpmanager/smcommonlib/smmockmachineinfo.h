/*****************************************************************************
   ģ����      : KDVSNMPMANAGER
   �ļ���      : smmockmachineinfo.h
   ����ļ�    : smmockmachineinfo.cpp
   �ļ�ʵ�ֹ���: ����ļܹ�ͼ��Ϣ�����
   ����		   : ½����
   �汾        : V5.0  Copyright(C) 2009-2012 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾     �޸���      �޸�����
   2009/12/31  5.0      ½����      ����
******************************************************************************/
#ifndef KDV_SMMOCKMACHINEINFO_H
#define KDV_SMMOCKMACHINEINFO_H
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


//���½ṹ���ڴ��䣬������
//��Ӧ�Ĳ����Ϣ��Ҳ����Ӧ�İ忨��Ϣ
typedef struct tagLayerSlotInfo
{
	u32	m_dwBoardIp;		// �忨��ip��ַ
	u8	m_byLayerIndex;		// ����Ĳ�����
	u8	m_bySlotIndex;		// ����Ĳ�λ����
	u8	m_byBoardType;		// �忨����
	u8	m_byLinkStatus;		// �������·״��
	u8  m_byOsType;			// OS����
	s8	m_szPanelLed[MAX_LED_STATUS_LEN];	//������źŵ����
	s8  m_szSoftwareVersion[MAX_SOFTWARE_VER_LEN];//��������汾
}PACKED TLayerSlotInfo;

//���½ṹ���ڴ��䣬������
//�忨��Ϣ�����ͷ
typedef struct tagMockMachineInfoHead
{
	u8	m_byLayerAmount;		// ����Ĳ�������������Դ
	u8	m_bySlotAmountOfLayer;	// ����Ĳ�λ����
	u16	m_wBoardNum;			// �����е��������(ʵ�ʲ����)
}PACKED TMockMachineInfoHead;

/*====================================================================
����  : CMockMachineInfo
����  : ����ͼ�����Ϣ������
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
1/11/2010   5.0			½����			����
====================================================================*/
class CMockMachineInfo
{
public:
	//���캯��
	CMockMachineInfo();

	//�˺�����������������ܵ�buffer��Ľ���
	CMockMachineInfo(const s8* pszBuffer, u32 dwSize);
	virtual ~CMockMachineInfo();
	
public:
	
	//�����Ļ�ȡ������
	void SetLayerAmount(u8 byLayerNum);
	u8 GetLayerAmount();

	//ÿ��Ĳ����Ļ�ȡ������
	void SetSlotNumOfLayer(u8 bySlotNum);
	u8 GetSlotAmountOfLayer();

	//��õ���ĸ���,����������
	u16 GetBoardNum();

	//�˺����Ǽ��������ȣ��ȽϺ�ʱ�����֪��dwIndex,��ֱ����������ȡ��
	//byLayerID������0 - 3
	//bySlotID ������0 - 15
	//���ش洢Ϊ������
	const TLayerSlotInfo* GetLayerSlotInfo(u8 byLayerID, u8 bySlotID);

	//dwIndex ��������0 - 63
	//���ش洢Ϊ������
	const TLayerSlotInfo* GetLayerSlotInfo(u32 dwIndex);

	//tLayerSlotInfoΪ������
	BOOL32 AddBoard(const TLayerSlotInfo& tLayerSlotInfo);

	//���������Ϣm_tMachineInfoHead.m_wBoardNum = 0
	void ClearBoardInfo();

	//�˺��������������紫��ķ�װbuffer
	virtual u32 GetBuffer(s8 *pszBuffer, u32 dwBufferSize);

protected:
	//���캯��ֱ�ӳ�ʼ��������
	CMockMachineInfo(CMockMachineInfo&);
	//����Ϊ���������ڣ��ڲ�ʵ�֣���ֹ�ⲿ���á�
	CMockMachineInfo& operator=(const CMockMachineInfo& cPfmInfo);
	void Copy(const CMockMachineInfo* pcPfmInfo);

protected:
	TMockMachineInfoHead m_tMachineInfoHead; //mcu
	TLayerSlotInfo m_atLayerSlotInfo[SM_MCU_LAYER_SLOT_MAX];
}PACKED;

#ifdef WIN32
#ifdef PACKED
#pragma pack(pop)
#endif
#undef PACKED
#endif //WIN32

#endif
