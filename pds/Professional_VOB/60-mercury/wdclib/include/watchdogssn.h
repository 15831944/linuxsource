/*****************************************************************************
ģ����      : watchdogssn
�ļ���      : watchdogssn.h
����ʱ��    : 2009�� 02�� 23��
ʵ�ֹ���    : 
����        : �ź��
�汾        : 
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
2009/02/23  1.0         �ź��        ����
******************************************************************************/
#ifndef _WATCHDOGSSN_H_
#define _WATCHDOGSSN_H_

#include "watchdogcomm.h"
#include "watchdog.h"
#include "mcustruct.h"
#include "mcuconst.h"
#ifdef WIN32
#include <Tlhelp32.h>
#endif

void WatchDogPrint( s8 * pszFmt, ... );

typedef struct tagCheckAppData
{
	TMType m_eMType;
	u32 m_adwAPPNodeIID[8];
	u32 m_adwAPPNode[8];
	u8  m_abyTimeOUtCount[8];//��¼ÿ��ģ�鳬ʱ����

	u8 m_byRegistAPP;//Ӧ��ע���ģ��app��һ���߳�ռ��һλ
	u8 m_byCurrentState;//�����������л�Ӧ������λ

	tagCheckAppData()
	{
		Clear();
	}
	void Clear()
	{
		m_eMType = em_INVALIDMODULE;
		m_byCurrentState = 0;
		m_byRegistAPP    = 0;
		memset(m_adwAPPNodeIID, 0, sizeof(m_adwAPPNodeIID));
		memset(m_adwAPPNode, 0, sizeof(m_adwAPPNode));
		memset(m_abyTimeOUtCount, 0 ,sizeof(m_abyTimeOUtCount));
	}
	void SetMType(TMType eMType)
	{
		m_eMType = eMType;
	}
	BOOL32 IsRegOK()
	{
		switch(m_eMType)
		{
		case em_INVALIDMODULE:
			WatchDogPrint("Module Type is Invalid!\n");
			return FALSE;
		case em_TS:
			if (m_byRegistAPP == 7)
			{
				return TRUE;
			}
			break;
		case em_GK:
			if (m_byRegistAPP == 1)
			{
				return TRUE;
			}
			break;
		case em_PROXY:
			break;
		case em_MMP:
			break;
		case em_MCU:
			if (m_byRegistAPP == 1)
			{
				return TRUE;
			}
			break;
		default:
			break;
		}
		return FALSE;
	}
	void AddAPPNodeInfo(u32 byApptype,u32 dwNode, u32 dwNodeIID)
	{
		if (byApptype > 8 || byApptype ==0)
		{
			return;
		}
		m_adwAPPNodeIID[byApptype-1] = dwNodeIID;
		m_adwAPPNode[byApptype -1]  = dwNode;
		return;

	}
	u8 AddAPPReg(u32 byBite)
	{
		if (byBite > 8 || byBite ==0)
		{
			return 0;
		}
		m_byRegistAPP |= ( 0x01 << (byBite -1)) ;
		return m_byRegistAPP;
	}
	u8 DelAPPReg(u8 byBite)
	{
		return 0;
	}
	u8 ADDAPPState(u8 byBite)
	{
		if (byBite > 8 || byBite == 0)
		{
			return 0;
		}
		m_byCurrentState |= ( 0x01 << (byBite -1)) ;
		return m_byCurrentState;
	}
	void AddTimeOut()
	{
		u8 bytimeout = ~(m_byCurrentState) & m_byRegistAPP;
		for (u8 Index =0;Index <8; Index ++)
		{
			if ((bytimeout >> Index) & 0x01)
			{
				m_abyTimeOUtCount[Index]++;
			}
		}
	}
	BOOL32 IsAPPStateNormal()
	{
		return (m_byCurrentState == m_byRegistAPP);
	}
	void SetStateZero()
	{
		m_byCurrentState &= 0;
		memset(m_abyTimeOUtCount, 0 ,sizeof(m_abyTimeOUtCount));
	}
	BOOL32 IsTimeOut(u8* pbyBite)
	{
		for (u8 Index=0;Index<8;Index++)
		{
			if(m_abyTimeOUtCount[Index] >=3)
			{
				*pbyBite = Index;
				return TRUE;
			}
		}
		return FALSE;

	}
}TCheckAppData;

class CWatchDogClientSsnInst : public CInstance
{
public:
	CWatchDogClientSsnInst();
	virtual ~CWatchDogClientSsnInst();
	void DaemonInstanceEntry(CMessage *const pMsg, CApp *pcApp);
	void InstanceEntry(CMessage *const pcMsg);
	
private:
	void DaemProcAPPREG(CMessage *const pMsg, CApp *pcApp);
	void DaemPingEachAPP();
	void DaemProcHBACK(u8 byAPPBite);
	void DaemProcHBTOUT();

	void ProcConnectWDServ();
	void ProcConnectWDServReply(CMessage *const pcMsg);
	void ProcHBInfo();
	void PrcoMcuGetKeyAck(CMessage *const pcMsg);
	void ProcMcuGetOtherMDACK(CMessage *const pcMsg);
	void ProcWDSQuitMsg();
	void KillPByName(const u8* pbyPName);
	//TModuleParameter m_tModuleParameter;
	//����������Ľڵ��
//	u32 m_dwServNode;
	//�����������IID
//	u32 m_dwServIID;
};
class  CWatchDogClientModuleDate
{
public:
	CWatchDogClientModuleDate()
	{
		m_tModuleParameter.emState = em_STOP;
		m_tModuleParameter.emType  = em_INVALIDMODULE;
		m_hCheckAPPSem = NULL;
		m_dwServNode = INVALID_NODE;
    	m_dwServIID  = INVALID_INS;
		memset(&m_tCheckAppData,0,sizeof(TCheckAppData));
		memset(&m_tLicenseInfo,0,sizeof(TLicenseMsg));
		for (u8 loop =0;loop<WD_MODULE_NUM;loop++)
		{
			m_achOtherMdInfo[loop].emState = em_STOP;
			m_achOtherMdInfo[loop].emType = em_INVALIDMODULE;
		}
	};
	virtual ~CWatchDogClientModuleDate(){};
	BOOL32 InitModule(TMType emType);
	void WDPrint();
	
	u32 m_dwServNode;
	//�����������IID
	u32 m_dwServIID;

	TModuleParameter m_tModuleParameter;
	//����߳�ע�����
	TCheckAppData m_tCheckAppData;
	SEMHANDLE m_hCheckAPPSem;

	//For mcu
	TLicenseMsg      m_tLicenseInfo;
	TModuleParameter m_achOtherMdInfo[em_END];
private:
	//nothing
};

typedef zTemplate <CWatchDogClientSsnInst ,1,CWatchDogClientModuleDate> CWDClientSsnAPP;

extern CWDClientSsnAPP g_cWDClientSsn;
extern BOOL32 g_bDebug;



// ��ʱ������
#define TIMER_CONNECT_POWERON     400
#define TIMER_CONNECT_DISC        3000
#define TIMER_CHECK_OTHERAPP      2000
#define TIMER_CHECK_OTHERAPP_OUT  4000 

// ��ʱ���¼�

// ���ӷ�����
OSPEVENT(EV_CONNECTWDSERV_TIMER,             (EV_WD8000E_BGN + 70) );   

// �������ע��
OSPEVENT(EV_CONNECTWDSERV_TIMEOUT,           (EV_WD8000E_BGN + 71) );

//��ѯ�ϲ��̼߳�ʱ��
OSPEVENT(EV_CHECKAPP_HB_REQ,				 (EV_WD8000E_BGN + 72) );
OSPEVENT(EV_CHECKAPP_HB_OUT,                 (EV_WD8000E_BGN + 73) ); 
#endif //end _WATCHDOGSSN_H_