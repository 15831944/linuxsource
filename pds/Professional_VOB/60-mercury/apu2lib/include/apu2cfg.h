/*****************************************************************************
  ģ����      : libapu2.a
  �ļ���      : apu2cfg.h
  ����ļ�    : 
  �ļ�ʵ�ֹ���: apu2������չ
  ����        : �ܼ���
  �汾        : V1.0  Copyright(C) 2009-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------*/
#ifndef _APU2_CFG_H_
#define _APU2_CFG_H_

#include "eqpbase.h"

#define  MAXNUM_APU2_BAS		(u8)1				//APU2�������������				
#define  MIXER_APU2_CHN_MINNUM  (u8)2				//APU2��С����ͨ����
#define  BRD_APU2_SNDPORT       (u16)10000          //APU2������ʼ���Ͷ˿�

	/************************************************************************/
	/*							CApu2Cfg�����ඨ��							*/
	/*																		*/
	/*        ˵����һ��Apu2���2����������ÿ�����������ö�ӦCApu2Cfg		*/
	/************************************************************************/
class CApu2MixerCfg:public CEqpCfg
{
public:
	CApu2MixerCfg():m_byMemberNum(0),
					m_bySndOff(0),
					m_byRcvOff(0),
					m_byIsSimuApu(0),
					m_wSimApuVer(0),
					m_byQualityLvl(0)
	{
	
	};
	~CApu2MixerCfg()
	{	
		Clear();
	};
public:

	void Clear()
	{
		m_byMemberNum     = 0;
		m_bySndOff        = 0;
		m_byRcvOff        = 0;
		m_byIsSimuApu     = 0; 
		m_wSimApuVer      = 0;
		m_byQualityLvl	  = 0;
	}

	void Copy (const CApu2MixerCfg* const pcCfg)
	{
		if (NULL == pcCfg)
		{
			return;
		}
		
		m_byMemberNum      = pcCfg->m_byMemberNum;
		m_bySndOff         = pcCfg->m_bySndOff;
		m_byRcvOff		   = pcCfg->m_byRcvOff;
		m_byIsSimuApu	   = pcCfg->m_byIsSimuApu;
		m_wSimApuVer       = pcCfg->m_wSimApuVer;
		m_byQualityLvl	   = pcCfg->m_byQualityLvl;

		m_dwMcuNodeA       = pcCfg->m_dwMcuNodeA;
		m_dwMcuIIdA        = pcCfg->m_dwMcuIIdA;
		m_dwMcuNodeB       = pcCfg->m_dwMcuNodeB;
		m_dwMcuIIdB		   = pcCfg->m_dwMcuIIdB;
		m_dwServerNode     = pcCfg->m_dwServerNode;
		m_dwServerIId      = pcCfg->m_dwServerIId;
		m_dwMpcSSrc		   = pcCfg->m_dwMpcSSrc;
		m_dwMcuRcvIp       = pcCfg->m_dwMcuRcvIp;
		m_wMcuRcvStartPort = pcCfg->m_wMcuRcvStartPort;
		m_wEqpVer		   = pcCfg->m_wEqpVer;
		m_byRegAckNum      = pcCfg->m_byRegAckNum;
		memcpy(&m_tPrsTimeSpan, &pcCfg->m_tPrsTimeSpan, sizeof(m_tPrsTimeSpan));
		memcpy(&m_tQosInfo, &pcCfg->m_tQosInfo, sizeof(m_tQosInfo));

		
		byEqpId			   = pcCfg->byEqpId;
		byEqpType          = pcCfg->byEqpType;
		dwLocalIP		   = pcCfg->dwLocalIP;
		dwConnectIP		   = pcCfg->dwConnectIP;
		wConnectPort	   = pcCfg->wConnectPort;
		wMcuId			   = pcCfg->wMcuId;
		wRcvStartPort	   = pcCfg->wRcvStartPort;
		wMAPCount		   = pcCfg->wMAPCount;
		dwConnectIpB	   = pcCfg->dwConnectIpB;
		wConnectPortB	   = pcCfg->wConnectPortB;
		wRcvStartPortB     = pcCfg->wRcvStartPortB;
		memcpy(achAlias, pcCfg->achAlias, MAXLEN_ALIAS);
		memcpy(m_atMap, pcCfg->m_atMap, sizeof(m_atMap));
		return;
	}

	void Print()
	{
		CEqpCfg::Print();
		StaticLog("\nMix: \n");
		StaticLog("\t MemberNum		: %d\n", m_byMemberNum);
		StaticLog("\t m_bySndOff		: %d\n", m_bySndOff);
		StaticLog("\t m_byRcvOff		: %d\n", m_byRcvOff);
		StaticLog("\t m_byIsSimuApu		: %d\n", m_byIsSimuApu);
		StaticLog("\t m_wSimApuVer		: %d\n", m_wSimApuVer);
		StaticLog("\t m_byQualityLvl		: %d\n", m_byQualityLvl);
	}
public:	
	u8     m_byMemberNum;		//���ó�Ա����
	u8     m_bySndOff;			//���Ͷ���ƫ��
	u8	   m_byRcvOff;			//���ն���ƫ��
	u8     m_byIsSimuApu;       //�Ƿ�ģ��Apu
	u16    m_wSimApuVer;		//ģ��Apu�汾
	u8	   m_byQualityLvl;		//���ʵȼ�
	
};

class CApu2BasCfg : public CEqpCfg
{
public:
	CApu2BasCfg():m_byQualityLvl(0)
	{
		
	};
	~CApu2BasCfg()
	{	
		Clear();
	};
public:
	
	void Clear()
	{
		m_byQualityLvl	  = 0;
	}
	
	void Copy (const CApu2BasCfg* const pcCfg)
	{
		if (NULL == pcCfg)
		{
			return;
		}
		
		m_byQualityLvl	   = pcCfg->m_byQualityLvl;
		
		m_dwMcuNodeA       = pcCfg->m_dwMcuNodeA;
		m_dwMcuIIdA        = pcCfg->m_dwMcuIIdA;
		m_dwMcuNodeB       = pcCfg->m_dwMcuNodeB;
		m_dwMcuIIdB		   = pcCfg->m_dwMcuIIdB;
		m_dwServerNode     = pcCfg->m_dwServerNode;
		m_dwServerIId      = pcCfg->m_dwServerIId;
		m_dwMpcSSrc		   = pcCfg->m_dwMpcSSrc;
		m_dwMcuRcvIp       = pcCfg->m_dwMcuRcvIp;
		m_wMcuRcvStartPort = pcCfg->m_wMcuRcvStartPort;
		m_wEqpVer		   = pcCfg->m_wEqpVer;
		m_byRegAckNum      = pcCfg->m_byRegAckNum;
		memcpy(&m_tPrsTimeSpan, &pcCfg->m_tPrsTimeSpan, sizeof(m_tPrsTimeSpan));
		memcpy(&m_tQosInfo, &pcCfg->m_tQosInfo, sizeof(m_tQosInfo));
		
		
		byEqpId			   = pcCfg->byEqpId;
		byEqpType          = pcCfg->byEqpType;
		dwLocalIP		   = pcCfg->dwLocalIP;
		dwConnectIP		   = pcCfg->dwConnectIP;
		wConnectPort	   = pcCfg->wConnectPort;
		wMcuId			   = pcCfg->wMcuId;
		wRcvStartPort	   = pcCfg->wRcvStartPort;
		wMAPCount		   = pcCfg->wMAPCount;
		dwConnectIpB	   = pcCfg->dwConnectIpB;
		wConnectPortB	   = pcCfg->wConnectPortB;
		wRcvStartPortB     = pcCfg->wRcvStartPortB;
		memcpy(achAlias, pcCfg->achAlias, MAXLEN_ALIAS);
		memcpy(m_atMap, pcCfg->m_atMap, sizeof(m_atMap));
		return;
	}
	
	void Print()
	{
		CEqpCfg::Print();
		StaticLog("\nBas: \n");
		StaticLog("\t m_byQualityLvl		: %d", m_byQualityLvl);
	}
public:	
	u8	   m_byQualityLvl;		//���ʵȼ�
};

	/************************************************************************/
	/*							CApu2Cfg�����ඨ��							*/
	/*																		*/
	/*					˵����һ��Apu2�豸��ӦTApu2EqpCfg					*/
	/************************************************************************/
struct TApu2EqpCfg
{
public:
	BOOL32        m_bIsProductTest;					   //�Ƿ���������
	u8		      m_byMixerNum;						   //Apu2�豸���˼���������
	CApu2MixerCfg m_acMixerCfg[MAXNUM_APU2_MIXER];	   //ÿ������������
	u8			  m_byBasNum;						   //APU2�豸���˼���������
	CApu2BasCfg	  m_acBasCfg[MAXNUM_APU2_BAS];		   //ÿ������������
public:
	TApu2EqpCfg()
	{
		m_bIsProductTest = FALSE;
		m_byMixerNum     = 0;
	}
};

#endif