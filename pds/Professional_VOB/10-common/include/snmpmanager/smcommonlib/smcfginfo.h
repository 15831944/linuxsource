/*****************************************************************************
   ģ����      : KDVSNMPMANAGER
   �ļ���      : smcfginfo.h
   ����ļ�    : smcfginfo.cpp
   �ļ�ʵ�ֹ���: �����������Ϣ�����
   ����		   : ½����
   �汾        : V5.0  Copyright(C) 2009-2012 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾     �޸���      �޸�����
   2009/12/31  5.0      ½����      ����
******************************************************************************/
#ifndef KDV_SMCFGINFO_H
#define KDV_SMCFGINFO_H

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

/*====================================================================
����  : CCfgInfoBase
����  : ���ù��������Ϣ����
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
1/11/2010   5.0			½����			����
====================================================================*/
class CCfgInfoBase
{
public:

	//���캯��
	CCfgInfoBase();

	//�˺�����������������ܵ�buffer��Ľ���
	CCfgInfoBase(const s8* pszBuffer, u32 dwSize);

	//����
	virtual ~CCfgInfoBase();
	
public:

	//�豸���ͻ�ȡ������
	//ע����8λΪMainType, ��8λΪSubType
	u16 GetDeviceType();
	u8 GetDeviceMainType();
	u8 GetDeviceSubType();
	void SetDeviceType(u16 wDeviceType);

	//������ȡ������
	const s8* GetAlias();
	void SetAlias(const s8* pszAlias, u32 dwLen);

	//164�ŵĻ�ȡ������
	const s8* Get164Num();
	void Set164Num(const s8* psz164Num, u32 dwLen);

	//�豸IP�Ļ�ȡ������
	u32 GetDeviceIP();
	void SetDeviceIP(u32 dwIpAddr/*������*/);

	//GK�Ļ�ȡ������
	u32 GetGkIP();	
	void SetGkIP(u32 dwIpAddr/*������*/);

	//��ʼ���ܶ˿ڵĻ�ȡ������
	u16 GetStartRevPort();
	void SetStartRevPort(u16 wPort/*������*/);

	//225245��ʼ�˿ڵĻ�ȡ������
	u16 Get225245StartPort();
	void Set225245StartPort(u16 wPort/*������*/);

	//��������Ļ�ȡ������
	u32 GetIpMask();
	void SetIpMask(u32 dwIpMask/*������*/);

	//���صĻ�ȡ������
	u32 GetGateWay();
	void SetGateWay(u32 dwGateWay/*������*/);

	//��ۺŵĻ�ȡ������
	u8 GetLayerID();
	void SetLayerID(u8 byLayerID/*������*/);
	u8 GetSlotID();
	void SetSlotID(u8 bySlotID/*������*/);

	//�������͵Ļ�ȡ������
	u8 GetNetPortKind();
	void SetNetPortKind(u8 byNetPortKind/*������*/);

	//�˺��������������紫��ķ�װbuffer
	virtual u32 GetBuffer(s8 *pszBuffer, u32 dwBufferSize);

	CCfgInfoBase& operator=(const CCfgInfoBase& cCfgInfo);

protected:
	//���캯��ֱ�ӳ�ʼ��������
	CCfgInfoBase(const CCfgInfoBase&);
	void Copy(const CCfgInfoBase* pcCfgInfoBase);

protected:
	u16 m_wDeviceType;//�������豸���͡� ������ĵ�8λΪMainType, ��8λΪSubType
	s8  m_achAlias[SM_MAX_ALIAS_LEN + 4];//�豸����
	s8  m_ach164Num[SM_MAX_E164_LEN + 1];//�豸164��
	u32 m_dwDeviceIP; //�������豸IP
	u32 m_dwGkIP; //�������豸GKIP
	u16 m_wStartRevPort; //������MCUʱΪ�豸��ʼ���ܶ˿ڣ�MTʱΪ�豸UDP�˿�
	u16 m_w225245StartPort; //������MCUʱΪ�豸225245��ʼ�˿ڣ�MTʱΪ�豸TCP�˿�
	u32 m_dwIpMask;//����������
	u32 m_dwGateWay;//����������
	u8	m_byLayerID;//���
	u8  m_bySlotID;//�ۺ�
	u8	m_byNetPortKind; //��������
	u8  m_byReserved;
	u16 m_wReserved;
}PACKED;


/*====================================================================
����  : CBoardCfgInfo
����  : �������õ������(����mcu)���̳���CCfgInfoBase
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
1/11/2010   5.0			½����			����
====================================================================*/
class CBoardCfgInfo : public CCfgInfoBase
{
public:
	//���캯��
	CBoardCfgInfo();
	//�˺�����������������ܵ�buffer��Ľ���
	CBoardCfgInfo(const s8* pszBuffer, u32 dwSize);
	virtual ~CBoardCfgInfo();

	//�˺��������������紫��ķ�װbuffer
	//����ֵΪ��ʵbuffer�Ĵ�С
	virtual u32 GetBuffer(s8 *pszBuffer, u32 dwBufferSize);

	//����Ϊ���������ڣ��ڲ�ʵ�֣���ֹ�ⲿ���á�
	CBoardCfgInfo& operator=(const CBoardCfgInfo& cCfgInfo);
protected:
	//���캯��ֱ�ӳ�ʼ��������
	CBoardCfgInfo(const CBoardCfgInfo&);
	void Copy(const CBoardCfgInfo* pcCfgInfo);

private:
}PACKED;

/*====================================================================
����  : CMtCfgInfo
����  : �ն����õ�������̳���CCfgInfoBase
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
1/11/2010   5.0			½����			����
====================================================================*/
class CMtCfgInfo : public CCfgInfoBase
{
public:
	//���캯��
	CMtCfgInfo();
	//�˺�����������������ܵ�buffer��Ľ���
	CMtCfgInfo(const s8* pszBuffer, u32 dwSize);
	virtual ~CMtCfgInfo();

public:

	//����IP�Ļ�ȡ������
	u32 GetAgentIp();
	void SetAgentIp(u32 dwIpAddr);

	//����˿ڵĻ�ȡ������
	u16 GetAgentPort();	
	void SetAgentPort(u16 wIpPort);

	//������������Ļ�ȡ������
	//0 : ��̬IP��ַ
	//1 : PPPoE
	u16 GetNetKind();	
	void SetNetKind(u16 wNetKind);

	//��̬NAT�Ļ�ȡ������
	u32 GetNatIp();	
	void SetNatIp(u32 dwIpAddr);

	//�˺��������������紫��ķ�װbuffer
	virtual u32 GetBuffer(s8 *pszBuffer, u32 dwBufferSize);

	CMtCfgInfo& operator=(const CMtCfgInfo& cCfgInfo);
protected:
	//���캯��ֱ�ӳ�ʼ��������
	CMtCfgInfo(const CMtCfgInfo&);
	void Copy(const CMtCfgInfo* pcCfgInfo);

private:
	u32 m_dwAgentIp;//�����򣬴���IP
	u16 m_wAgentPort;//�����򣬴���˿�
	u16 m_wNetKind;//��������������
	u32 m_dwNatIp;//�����򣬾�̬IP
	
}PACKED;


/*====================================================================
����  : CBatchCfgInfoBase
����  : �������û��࣬������
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
1/11/2010   5.0			½����			����
====================================================================*/
class CBatchCfgInfoBase
{
public:
	//���캯��
	CBatchCfgInfoBase();
	//�˺�����������������ܵ�buffer��Ľ���
	CBatchCfgInfoBase(const s8* pszBuffer, u32 dwSize);
	virtual ~CBatchCfgInfoBase();

public:

	//�豸���ͻ�ȡ������
	//ע����8λΪMainType, ��8λΪSubType
	u16 GetDeviceType();
	u8 GetDeviceMainType();
	u8 GetDeviceSubType();
	void SetDeviceType(u16 wDeviceType);

	//�豸GK��IP
	//�õ�������
	u32 GetGkIP();	
	void SetGkIP(u32 dwIpAddr/*������*/);

	//�˺��������������紫��ķ�װbuffer
	virtual u32 GetBuffer(s8 *pszBuffer, u32 dwBufferSize);

protected:
	//���캯��ֱ�ӳ�ʼ��������
	CBatchCfgInfoBase(const CBatchCfgInfoBase&);
	//�˺�����������������ܵ�buffer��Ľ���
	CBatchCfgInfoBase& operator=(const CBatchCfgInfoBase& cCfgInfo);
	void Copy(const CBatchCfgInfoBase* pcCfgInfo);

private:
	u16 m_wDeviceType; //�������豸����
	u32 m_dwGkIP; //�������豸GKIP
}PACKED;

/*====================================================================
����  : CBoardBatchCfgInfo���̳���CBatchCfgInfoBase
����  : �����������ù����������紫��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
1/11/2010   5.0			½����			����
====================================================================*/
class CBoardBatchCfgInfo : public CBatchCfgInfoBase
{
public:
	//���캯��
	CBoardBatchCfgInfo();
	//�˺�����������������ܵ�buffer��Ľ���
	CBoardBatchCfgInfo(const s8* pszBuffer, u32 dwSize);
	virtual ~CBoardBatchCfgInfo();

	//��ۺŵĻ�ȡ������
	u8 GetLayerID();	
	void SetLayerID(u8 byLayerID);
	u8 GetSlotID();	
	void SetSlotID(u8 bySlotID);

	//�˺��������������紫��ķ�װbuffer
	virtual u32 GetBuffer(s8 *pszBuffer, u32 dwBufferSize);

protected:
	CBoardBatchCfgInfo(const CBoardBatchCfgInfo&);
	CBoardBatchCfgInfo& operator=(const CBoardBatchCfgInfo& cCfgInfo);

	void Copy(const CBoardBatchCfgInfo* pcCfgInfo);
private:
	u8	m_byLayerID;//���
	u8  m_bySlotID;//�ۺ�
}PACKED;

/*====================================================================
����  : CMtBatchCfgInfo���̳���CBatchCfgInfoBase
����  : �����������ù����������紫��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
1/11/2010   5.0			½����			����
====================================================================*/
class CMtBatchCfgInfo : public CBatchCfgInfoBase
{
public:
	//���캯��
	CMtBatchCfgInfo();
	//�˺�����������������ܵ�buffer��Ľ���
	CMtBatchCfgInfo(const s8* pszBuffer, u32 dwSize);
	virtual ~CMtBatchCfgInfo();

	//�˺��������������紫��ķ�װbuffer	
	//����ʵ�����buffer�ĳ���
	virtual u32 GetBuffer(s8 *pszBuffer, u32 dwBufferSize);


protected:
	CMtBatchCfgInfo(const CMtBatchCfgInfo&);
	CMtBatchCfgInfo& operator=(const CMtBatchCfgInfo& cCfgInfo);

	void Copy(const CMtBatchCfgInfo* pcCfgInfo);
private:
}PACKED;

#ifdef WIN32
#ifdef PACKED
#pragma pack(pop)
#endif
#undef PACKED
#endif //WIN32

#endif
