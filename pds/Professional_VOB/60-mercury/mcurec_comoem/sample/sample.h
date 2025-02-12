// sample.h : main header file for the SAMPLE application
//

#if !defined(AFX_SAMPLE_H__CFF26DDF_1F49_42F3_A725_4315BC351512__INCLUDED_)
#define AFX_SAMPLE_H__CFF26DDF_1F49_42F3_A725_4315BC351512__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "Sample_i.h"

#define WM_CONFUPDATE       (WM_USER+0x1000)
#define WM_STREAMUPDATE     (WM_USER+0x1001)
/////////////////////////////////////////////////////////////////////////////
// CSampleApp:
// See sample.cpp for the implementation of this class
//

class CSampleApp : public CWinApp
{
public:
	CSampleApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSampleApp)
	public:
	virtual BOOL InitInstance();
		virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSampleApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bATLInited;
private:
	BOOL InitATL();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAMPLE_H__CFF26DDF_1F49_42F3_A725_4315BC351512__INCLUDED_)
