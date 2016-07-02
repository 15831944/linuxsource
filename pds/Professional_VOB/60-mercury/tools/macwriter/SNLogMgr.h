// SNLogMgr.h: interface for the CSNLogMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SNLOGMGR_H__45734DF0_40DB_4CB6_81B6_BA320D08245D__INCLUDED_)
#define AFX_SNLOGMGR_H__45734DF0_40DB_4CB6_81B6_BA320D08245D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MacAddr.h"

#define FILENAME_LOG        "mtwlog.txt"

// ÿ��Log��¼������
struct SNItem
{
    time_t  tTime;
    CString cstrSN;
    DWORD   dwMACCount;
    TMacU48 atU48[2];
};

class CSNLogMgr  
{
public:
	CSNLogMgr();
	virtual ~CSNLogMgr();

public:
    void SetAutoInc(CString cstrBegin)
    {
        bAutoInc = TRUE;
        cstrAutoSNBase = cstrBegin;
    }

    void SetNoAutoInc()
    {
        bAutoInc = FALSE;
        cstrAutoSNBase.Empty();
    }

    void AddNewItem(SNItem &item)
    {
        m_vtAddedSNItem.push_back(item);
    }

    void AddHistoryItem(SNItem &item)
    {
        m_vtHistorySNItem.push_back(item);
    }

    BOOL IsSNExistInHistory(SNItem &item);
    BOOL IsSNExistInNew(SNItem &item);

    // ��������������ָ��MAC��SN
    BOOL GetMatchedSNbyMacInNew(const CMacAddr &cAddr, SNItem &item);

    // ��ʷSN��ȡ
    void LoadHistorySNFromFile();
    void SaveHistorySN();


private:
    // �Զ�������ʽ
    BOOL bAutoInc;
    CString cstrAutoSNBase;
    
public:
    // �������õ���Ŀ
    vector<SNItem> m_vtAddedSNItem;

    // ������ʷ��Ŀ
    vector<SNItem> m_vtHistorySNItem;

};

extern CSNLogMgr g_SNLogMsg;
#endif // !defined(AFX_SNLOGMGR_H__45734DF0_40DB_4CB6_81B6_BA320D08245D__INCLUDED_)
