#if !defined(AFX_OSPCLIENTINSTANCE_H__2546E876_68F6_45B7_9E49_47017E9D3695__INCLUDED_)
#define AFX_OSPCLIENTINSTANCE_H__2546E876_68F6_45B7_9E49_47017E9D3695__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "osp.h"
#include "dsccomm.h"

void WDLog( s8 * pszFmt, ... );

#define WD_MAXMODULE    (u8)8
class CWatchDogSsnInst : public CInstance  
{
public:
	
	CWatchDogSsnInst();
	virtual ~CWatchDogSsnInst();

     /*------------------------------------------------------------------------
	 ������:   InstanceEntry
	 ��  ��:   ��Ϣ��ں��� 
	 ��  ��:   CMessage  * const pMsg           [in]���յ���Ϣ     
	 ����ֵ:   void
	  */
	void InstanceEntry(CMessage *const pcMsg);
    
private:
    void ProcConnectWDServer();
    void ProcSetModuleState();

    // �������Ľڵ��
    u32 m_dwServerNode;
    // �����������Ӧ��IID�������˷�����Ϣ��Ŀ��
    u32 m_dwServerIID;

};

class CWatchDogCfg
{
public:    
    CWatchDogCfg()
    {
        m_byModuleNum = 0;
    }
    u8 AddModule(emModuleType emNewType)
    {
        if (m_byModuleNum < WD_MAXMODULE)
        {
            m_atModuleParam[m_byModuleNum].emType = emNewType;
            m_atModuleParam[m_byModuleNum].emState = emRUNNING;
            m_byModuleNum ++;
            return m_byModuleNum;
        }
        return 0;
    }
    void PrintModuleState() const
    {
        for (u8 byLoop = 0; byLoop < m_byModuleNum; byLoop ++)
        {
            OspPrintf(TRUE,FALSE, "Module: %d, State: %d\n", m_atModuleParam[byLoop].emType, m_atModuleParam[byLoop].emState);
        }
    }
    IModuleParameter m_atModuleParam[WD_MAXMODULE];
    u8               m_byModuleNum;
};

typedef zTemplate <CWatchDogSsnInst,1, CWatchDogCfg> CWatchDogSsnApp;

extern CWatchDogSsnApp g_cWatchDogSsn;
extern BOOL32          g_bWDLog;

#endif // !defined(AFX_OSPCLIENTINSTANCE_H__2546E876_68F6_45B7_9E49_47017E9D3695__INCLUDED_)
