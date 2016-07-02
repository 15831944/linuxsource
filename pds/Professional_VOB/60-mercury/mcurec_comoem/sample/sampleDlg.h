// sampleDlg.h : header file
//



#if !defined(AFX_SAMPLEDLG_H__3B697FDF_280D_42F2_A339_556F51308E12__INCLUDED_)
#define AFX_SAMPLEDLG_H__3B697FDF_280D_42F2_A339_556F51308E12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Sink.h"	
#include "SinkStream.h"
#include "..\prj_win32\mcu_i.c"
#include "RecInterface.h"

class CSampleDlgAutoProxy;

/////////////////////////////////////////////////////////////////////////////
// CSampleDlg dialog

class CSampleDlg : public CDialog
{
	DECLARE_DYNAMIC(CSampleDlg);
	friend class CSampleDlgAutoProxy;

// Construction
public:
	CSampleDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CSampleDlg();

// Dialog Data
	//{{AFX_DATA(CSampleDlg)
	enum { IDD = IDD_SAMPLE_DIALOG };
	CListCtrl	m_lsView;
	int		m_nDebug;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSampleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSampleDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

	// Generated message map functions
	//{{AFX_MSG(CSampleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBtnStart();
	afx_msg void OnSelchangeCbDebug();
    afx_msg LRESULT OnConfUpdate (WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnStreamUpdate (WPARAM wParam, LPARAM lParam);
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


    CComQIPtr< IMCCtrl >       m_spRecCtrl;  // ¼�������ָ��
    CComQIPtr< IConfProperty > m_spProperty; // �������Ի�ȡָ��

    IMCStatusCallbackPtr m_spMcStatus; 		 // ����״̬����ӿ�ָ��
    IMCStreamCallbackPtr m_spMcStream;       // ������������ӿ�ָ��

	CComQIPtr< IConnectionPoint > m_spCPStatus;	// ����״̬֪ͨ���ӵ�ָ��
    CComQIPtr< IConnectionPoint > m_spCPStream;	// ����֪ͨ���ӵ�ָ��
    
    CSink m_cRecorder;						// ����������
    CSinkStream m_cStream;
    DWORD m_dwCookieStatus;					// ���ӵ� cookie
    DWORD m_dwCookieStream;					// ���ӵ� cookie

    void UpdateView(DWORD dwIdx);
    void UpdateView(TConfProperty &tConf, TRecProperty &tRec, emRecState emState );
    void UpdateView(TStreamProperty &tStream);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAMPLEDLG_H__3B697FDF_280D_42F2_A339_556F51308E12__INCLUDED_)
