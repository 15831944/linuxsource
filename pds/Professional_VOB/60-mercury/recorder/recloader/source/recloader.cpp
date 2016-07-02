/*****************************************************************************
   ģ����      : Recorder Loader
   �ļ���      : recloader.cpp
   ����ļ�    : recloaderdlg.cpp
   �ļ�ʵ�ֹ���: ¼�����װ��ж�غ�����
   ����        : 
   �汾        : V4.0  Copyright(C) 2004-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2006/08/23  4.0         ����      ����
******************************************************************************/

// recloader.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "recloader.h"
#include "recloaderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecloaderApp

BEGIN_MESSAGE_MAP(CRecloaderApp, CWinApp)
	//{{AFX_MSG_MAP(CRecloaderApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecloaderApp construction

CRecloaderApp::CRecloaderApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRecloaderApp object

CRecloaderApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRecloaderApp initialization


/*====================================================================
    ������	     ��InitInstance
	����		 ��������ڣ��������������
	����ȫ�ֱ��� ����
    �������˵�� ��
	����ֵ˵��   ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	06/08/23	4.0			����        ����
====================================================================*/
BOOL CRecloaderApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CRecloaderDlg dlg;    

    // ����������
    LPCTSTR lpszCmdLine = GetCommandLine();
    
    CString cstrCmdLine = lpszCmdLine;   
    cstrCmdLine.MakeLower();
    
    int nStart = cstrCmdLine.Find(".exe");
    if (nStart == -1)
    {
        // ����
        printf("Command line: recloader.exe [install <n> | remove]\n");
        return FALSE;
    }

    nStart += strlen(".exe")+1;
    cstrCmdLine = (lpszCmdLine+nStart);
    cstrCmdLine.TrimLeft(" ");
    cstrCmdLine.TrimRight(" ");

    if (cstrCmdLine.Find("install") >= 0)
    {
        // ��װ
        nStart = cstrCmdLine.Find("install");
        int nCopies = atoi( (LPCTSTR)cstrCmdLine + nStart + strlen("install") );
        dlg.Install(nCopies);
    }
    else if (cstrCmdLine.Find("remove") >= 0)
    {
        // ɾ��
        dlg.UnInstall();
    }
    else
    {
        // ��������
        m_pMainWnd = &dlg;
        dlg.DoModal();
    }
         
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
