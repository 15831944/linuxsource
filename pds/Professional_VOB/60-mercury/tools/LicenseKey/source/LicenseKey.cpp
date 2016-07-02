// LicenseKey.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "LicenseKey.h"
#include "LicenseKeySheet.h"
#include "usbkey.h"
#include "usbkey_i.c"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


HRESULT     hr;
IOprKey     *pISL = NULL;
/////////////////////////////////////////////////////////////////////////////
// CLicenseKeyApp

BEGIN_MESSAGE_MAP(CLicenseKeyApp, CWinApp)
	//{{AFX_MSG_MAP(CLicenseKeyApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLicenseKeyApp construction

CLicenseKeyApp::CLicenseKeyApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CLicenseKeyApp object

CLicenseKeyApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CLicenseKeyApp initialization

BOOL CLicenseKeyApp::InitInstance()
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
	system("regsvr32 /s usbkey.dll");

	hr = CoInitialize ( NULL );
    if ( FAILED ( hr ) )
    {
        AfxMessageBox("��ʼ��ʧ�ܣ������XPϵͳ���벻Ҫ�ѳ�����ں���������·���С�");
        return FALSE;
    }

	hr = CoCreateInstance ( CLSID_OprKey,				// coclass ��CLSID 
							NULL,						// �����þۺ�
							CLSCTX_INPROC_SERVER,		// ����������
							IID_IOprKey,				// �ӿڵ�IID 
							(void**) &pISL );			// ָ��ӿڵ�ָ��
	
    if ( FAILED ( hr ) )
	{
        CString cstr;
        cstr.Format("��ȡusbkey.dll����ӿ�ʧ�ܣ������°�װ!�����XPϵͳ���벻Ҫ�ѳ�����ں���������·���С������룺%d", hr);
		AfxMessageBox(cstr);
        return FALSE;
	}

	CLicenseKeySheet dlg;
	dlg.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	dlg.m_psh.pszCaption = "KEDACOM ���֤���ߡ������ڲ�ʹ��";
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

    CoUninitialize();
	return FALSE;
}
