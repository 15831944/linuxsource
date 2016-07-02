// Recorder.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Recorder.h"
#include "RecorderDlg.h"
//#include "ConfAgent.h"

#include "evrec.h"
#include "usbverify.h"
#include "usbkey.h"
//CUsbVerify	g_cUsbVerify;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecorderApp

BEGIN_MESSAGE_MAP(CRecorderApp, CWinApp)
	//{{AFX_MSG_MAP(CRecorderApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecorderApp construction

CRecorderApp::CRecorderApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRecorderApp object

CRecorderApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRecorderApp initialization

/*=============================================================================
  �� �� ���� USBKeyOutCallBack
  ��    �ܣ� USB Key �γ��ص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
u32 __stdcall USBKeyOutCallBack(IN void *pCBData)
{
	HWND hMainWnd = theApp.GetMainWnd()->GetSafeHwnd();
    if ( LANGUAGE_CHINESE == GetSystemDefaultLangID() )
	{
		MessageBox(hMainWnd, "�������ȷ�� USB Key !\n", "����", MB_ICONSTOP);
	}
	else
	{
		MessageBox(hMainWnd, "Please insert USB Key !\n", "Error", MB_ICONSTOP);
	}
    SendMessage(hMainWnd, WM_QUITREC, 0, 0);

	return 0;
}

/*====================================================================
    ������	     ��InitInstance
	����		 ��Ӧ�ó���ʵ����ʼ������
	����ȫ�ֱ��� ����
    �������˵�� ��
	����ֵ˵��   ��TRUE - �ɹ�   FALSE - ʧ��
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    05/03/31    3.6         �          ��ֹ�������ж��
====================================================================*/
BOOL CRecorderApp::InitInstance()
{
	AfxEnableControlContainer();

    BOOL32 m_bDefaultLang = GetSystemDefaultLangID();

    // ���� [6/8/2006] ������ʵ��
	s8 achProfileName[KDV_MAX_PATH] = _T("");
	CString csPath = _T("");

	GetModuleFileName(AfxGetInstanceHandle(), achProfileName, sizeof (achProfileName));

	csPath = achProfileName;
	csPath = csPath.Left( csPath.ReverseFind('\\') + 1 );
	csPath += RECCFG_FILE;

    g_tCfg.SetCfgFilename( csPath );
    g_tCfg.ReadConfigFromFile();

    if (!g_tCfg.IsInited())
    {
        // ��ʼ��ʧ�ܣ��˳�
        return FALSE;
    }

    // ��������������򴴽�������
    HANDLE hMutex = NULL;
    // guzh [9/8/2006] ��������ͬ·��¼���������ͬʱ��ֹ��ͬ·����¼�������
    // ����������Գ���ȫ������������
    //if (!g_tCfg.bMultiInst)
    {
        // ���������ֲ��ܴ�\���ܳ���Ϊ KDV_MAX_PATH
        CString cstrMutexName = achProfileName;
        cstrMutexName.Replace("\\", "/");   
        hMutex = CreateMutex(NULL, FALSE, cstrMutexName);
        if (hMutex == NULL)
        {
            if ( LANGUAGE_CHINESE == m_bDefaultLang ) 
            {
                AfxMessageBox("KDV ¼�����������ʼ��ʧ��!");
            }
            else
            {
                AfxMessageBox("KDV Record Server initialize failed !");
            }
            return FALSE;
        }
        else if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            CWnd* pCBWnd;
            if ( LANGUAGE_CHINESE == m_bDefaultLang )
            {
                AfxMessageBox("KDV ¼����������Ѿ�����!");
                pCBWnd = CWnd::FindWindow(NULL, "KDV ¼���������");
            }
            else
            {
                AfxMessageBox("KDV Record Server has been running already!");
                pCBWnd = CWnd::FindWindow(NULL, "KDV Recorder Server");                
            }
		    if (pCBWnd)
		    {
			    pCBWnd->ShowWindow(SW_SHOWNORMAL);
			    pCBWnd->SetForegroundWindow();
		    }

            return FALSE;
        }
    }

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif


    // [11/23/2006-zbq] ����USB Key��֧����ע�͵�
//	//��� USB Key
//	if ( g_cUsbVerify.Verify(enumUsbRightVOS) )
//	{
//		g_cUsbVerify.SetDeviceCallBack( NULL, USBKeyOutCallBack, NULL );
//	}
//	else
//	{
//		if ( LANGUAGE_CHINESE == m_bDefaultLang )
//		{
//			AfxMessageBox( "�������ȷ�� USB Key !\n" );
//		}
//		else
//		{
//			AfxMessageBox( "Please insert USB Key first !\n" );
//		}
//		return FALSE;
//	}

	
    // ��ʼ��OspӦ��
    UserInit();  

    // �����Ի���
	CRecorderDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.

    if (hMutex != NULL)
    {
         ReleaseMutex(hMutex);
    }
	return FALSE;
}
