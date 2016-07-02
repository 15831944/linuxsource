/*****************************************************************************
   ģ����      : Recorder Loader
   �ļ���      : recloaderdlg.cpp
   ����ļ�    : recloader.cpp
   �ļ�ʵ�ֹ���: ¼�����װ��ж�غ�����
   ����        : 
   �汾        : V4.0  Copyright(C) 2004-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2006/08/23  4.0         ����      ����
******************************************************************************/

// recloaderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "recloader.h"
#include "recloaderDlg.h"
#include "winsock2.h"
#include "kdvsys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define LANGUAGE_CHINESE        MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED)

#define RECAPPNAME          (LPCTSTR)"Recorder.exe"
#define RECCFGPATH          (LPCTSTR)"conf"
#define RECCFGNAME          (LPCTSTR)"reccfg.ini"

#define SEC_SYS             (LPCTSTR)"RecorderSystem"


// ��ʾ
#define RES_TITLE           1
#define RES_LAUNCH          2
#define RES_INSTALLED       3
#define RES_EXIT            4
#define RES_EXIT_ALONE      5
#define RES_ALL             6
#define RES_NONE            7

#define RES_REINSTALL       100

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecloaderDlg dialog

CRecloaderDlg::CRecloaderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRecloaderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRecloaderDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRecloaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecloaderDlg)
	DDX_Control(pDX, IDC_CK_REC14, m_ckRec14);
	DDX_Control(pDX, IDC_CK_REC13, m_ckRec13);
	DDX_Control(pDX, IDC_CK_REC12, m_ckRec12);
	DDX_Control(pDX, IDC_CK_REC11, m_ckRec11);
	DDX_Control(pDX, IDC_CK_REC10, m_ckRec10);
	DDX_Control(pDX, IDC_CK_REC9, m_ckRec9);
	DDX_Control(pDX, IDC_CK_REC8, m_ckRec8);
	DDX_Control(pDX, IDC_CK_REC7, m_ckRec7);
	DDX_Control(pDX, IDC_CK_REC6, m_ckRec6);
	DDX_Control(pDX, IDC_CK_REC5, m_ckRec5);
	DDX_Control(pDX, IDC_CK_REC4, m_ckRec4);
	DDX_Control(pDX, IDC_CK_REC3, m_ckRec3);
	DDX_Control(pDX, IDC_CK_REC2, m_ckRec2);
	DDX_Control(pDX, IDC_CK_REC1, m_ckRec1);
	DDX_Control(pDX, IDC_CK_REC0, m_ckRec0);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRecloaderDlg, CDialog)
	//{{AFX_MSG_MAP(CRecloaderDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_START, OnBtnStart)
	ON_BN_CLICKED(IDC_BTN_ALL, OnBtnAll)
	ON_BN_CLICKED(IDC_BTN_NONE, OnBtnNone)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecloaderDlg message handlers

BOOL CRecloaderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
    m_tLangId = GetSystemDefaultLangID();
    
    InitRecChecks();

	// Os�Ǽ������ģ��л����Ե�����
    if( LANGUAGE_CHINESE == m_tLangId )
    {
        ChgUILang();
    }
    
    if ( m_nValidRecNum == 1 )
    {
        OnBtnStart();

        if (m_vtInst.size() == 1)
        {
            OnCancel();
        }
    }
    
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRecloaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRecloaderDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRecloaderDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/*====================================================================
    ������	     ��InitRecChecks
	����		 ����ʼ������
	����ȫ�ֱ��� ����
    �������˵�� ��
	����ֵ˵��   ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	06/08/23	4.0			����        ����
====================================================================*/
void CRecloaderDlg::InitRecChecks()
{
    m_apckRec[0] = &m_ckRec0;
    m_apckRec[1] = &m_ckRec1;
    m_apckRec[2] = &m_ckRec2;
    m_apckRec[3] = &m_ckRec3;
    m_apckRec[4] = &m_ckRec4;
    m_apckRec[5] = &m_ckRec5;
    m_apckRec[6] = &m_ckRec6;
    m_apckRec[7] = &m_ckRec7;
    m_apckRec[8] = &m_ckRec8;
    m_apckRec[9] = &m_ckRec9;
    m_apckRec[10] = &m_ckRec10;
    m_apckRec[11] = &m_ckRec11;
    m_apckRec[12] = &m_ckRec12;
    m_apckRec[13] = &m_ckRec13;
    m_apckRec[14] = &m_ckRec14;

    m_nValidRecNum = 0;
    
	TCHAR achExeName[KDV_MAX_PATH] = _T("");

	GetModuleFileName(AfxGetInstanceHandle(), achExeName, sizeof (achExeName));

	CString cstrExePath = achExeName;
	cstrExePath = cstrExePath.Left( cstrExePath.ReverseFind('\\') );    // ����·��

    CString cstrOldCfgName;
    cstrOldCfgName.Format("%s\\%s\\%s", cstrExePath, RECCFGPATH, RECCFGNAME);
    
    int nCopies = 0;
    nCopies = GetPrivateProfileInt( SEC_SYS,
                                    "InstalledCopies",
                                    0,
                                    cstrOldCfgName );    
    
    if (nCopies == 0)
    {
        AfxMessageBox(GetText(RES_REINSTALL));
        return;
    }

    CString cstrNewCfgName;
    int nID;
    char szAlias[KDV_MAX_PATH] = {0};
    CString cstrTmp;
    
    CFileFind cff;
    for (int i = 0; i < nCopies; i++)
    {
        // ��ȡ����¼���ID/����
        cstrNewCfgName.Format("%s\\%d\\%s\\%s", cstrExePath, i, RECCFGPATH, RECCFGNAME);

        if (!cff.FindFile(cstrNewCfgName))
        {
            // û�ҵ�������
            cff.Close();
            continue;
        }
        cff.Close();
        
        // ID
        cstrTmp.Format("%d", 17+i);        
        nID = GetPrivateProfileInt(SEC_SYS, 
                                   _T("ID"),
                                   17,
                                   cstrNewCfgName);

        // Alias
        GetPrivateProfileString(SEC_SYS, 
                                _T("Alias"),
                                "Recorder",
                                szAlias,
                                KDV_MAX_PATH,
                                cstrNewCfgName);

        cstrTmp.Format("ID%d: %s", nID, szAlias);

        m_apckRec[m_nValidRecNum]->SetWindowText( cstrTmp );
        m_apckRec[m_nValidRecNum]->ShowWindow(SW_SHOW);
        m_apckRec[m_nValidRecNum]->SetCheck(BST_CHECKED);

        // ������Ӧ����
        m_astrAppName[m_nValidRecNum].Format("%s\\%d\\%s", cstrExePath, i, RECAPPNAME);

        m_nValidRecNum ++;
    }
}

/*====================================================================
    ������	     ��GetText
	����		 ��˫��֧��
	����ȫ�ֱ��� ����
    �������˵�� ��nResID - ������Դ
	����ֵ˵��   ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	06/08/23	4.0			����        ����
====================================================================*/
CString CRecloaderDlg::GetText(int nResID)
{
    if (LANGUAGE_CHINESE == m_tLangId)
    {
        switch (nResID)
        {
            case RES_TITLE:
                return "����¼���";
            case RES_LAUNCH:
                return "����¼���";
            case RES_INSTALLED:
                return "��װ��¼���";
            case RES_EXIT:
                return "�˳�����������¼���";
            case RES_EXIT_ALONE:
                return "ֱ���˳�";
            case RES_REINSTALL:
                return "û�м�⵽��װ��¼��������޸���װ!";
            case RES_ALL:
                return "����";
            case RES_NONE:
                return "���";
            default:
                return "";
        }
    }
    else
    {
        switch (nResID)
        {
            case RES_TITLE:
                return "Kedacom Recorder Loader";
            case RES_LAUNCH:
                return "Launch";
            case RES_INSTALLED:
                return "Recorder Installed";
            case RES_EXIT:
                return "Exit with All Recorders";
            case RES_EXIT_ALONE:
                return "Exit Alone";
            case RES_REINSTALL:
                return "No Recorder Detected, please repair installation!";
            case RES_ALL:
                return "All";
            case RES_NONE:
                return "None";
            default:
                return "";
        }
    }
}

/*====================================================================
    ������	     ��ChgUILang
	����		 ���л�����
	����ȫ�ֱ��� ����
    �������˵�� ��
	����ֵ˵��   ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	06/08/23	4.0			����        ����
====================================================================*/
void CRecloaderDlg::ChgUILang()
{
    AfxGetMainWnd()->SetWindowText(GetText(RES_TITLE));
    
    GetDlgItem(IDC_GRP_REC)->SetWindowText(GetText(RES_INSTALLED));
    GetDlgItem(IDC_BTN_START)->SetWindowText(GetText(RES_LAUNCH));
    GetDlgItem(IDC_BTN_ALL)->SetWindowText(GetText(RES_ALL));
    GetDlgItem(IDC_BTN_NONE)->SetWindowText(GetText(RES_NONE));
    GetDlgItem(IDOK)->SetWindowText(GetText(RES_EXIT));
    GetDlgItem(IDCANCEL)->SetWindowText(GetText(RES_EXIT_ALONE));
}


/*====================================================================
    ������	     ��GetSpecPath
	����		 ����ȡ����������ļ��е�ַ
	����ȫ�ֱ��� ����
    �������˵�� ��
	����ֵ˵��   ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	06/08/24	4.0			����        ����
====================================================================*/
BOOL GetSpecPath(int csidlType, char *szPath)
{
    LPITEMIDLIST ppidl;
    HRESULT hRes = SHGetSpecialFolderLocation(NULL, csidlType , &ppidl);    // | CSIDL_FLAG_CREATE
    BOOL bRet = FALSE;
    if (hRes == S_OK)
    {
        bRet = SHGetPathFromIDList( ppidl, szPath );
    }    
    return bRet;
}

/*====================================================================
    ������	     ��InitCOM
	����		 ����ʼ��COM�ӿ�
	����ȫ�ֱ��� ����
    �������˵�� ��
	����ֵ˵��   ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	06/08/24	4.0			����        ����
====================================================================*/
BOOL InitCOM(IShellLink* &pisl, IPersistFile* &pipf)
{
    ::CoInitialize(NULL);            
    HRESULT hRes = CoCreateInstance (CLSID_ShellLink, 
                                     NULL, CLSCTX_INPROC_SERVER,
                                     IID_IShellLink, 
                                     (void **)&pisl); 
    if (!SUCCEEDED (hRes))
    {
        return FALSE;
    }
    else
    {
        // ��ISHELLLINK�����л��IpersistFile����Ľӿ�
        hRes = pisl->QueryInterface(IID_IPersistFile, (void **)&pipf);         
        if (!SUCCEEDED (hRes))
        { 
            pisl->Release();
            return FALSE;
        }
        else
        {
            return TRUE;
        }    
    } 
}

/*====================================================================
    ������	     ��Install
	����		 ����װ¼����Ķ������������װ��ݷ�ʽ
	����ȫ�ֱ��� ����
    �������˵�� ��nCopies - �������������-1��ʾ�޸���װ�������������ȡ
	����ֵ˵��   ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	06/08/24	4.0			����        ����
====================================================================*/
void CRecloaderDlg::Install(int nCopies)
{    
	TCHAR achExeName[KDV_MAX_PATH] = _T("");

	GetModuleFileName(AfxGetInstanceHandle(), achExeName, sizeof (achExeName));

    // ��ȡ��ݷ�ʽ��Ŀ��·��
    TCHAR szShortcutPath[KDV_MAX_PATH] = {0};
    BOOL bCreateShortcut = FALSE;
    bCreateShortcut = GetSpecPath(CSIDL_COMMON_PROGRAMS, szShortcutPath);  //CSIDL_DESKTOP, CSIDL_PROGRAMS, 
    strcat(szShortcutPath, "\\Kedacom\\Recorder4.0");

    IShellLink *pisl;    
    IPersistFile *pipf;
    if (bCreateShortcut)
    {
        bCreateShortcut = InitCOM(pisl, pipf);
    }

	CString cstrExePath = achExeName;
	cstrExePath = cstrExePath.Left( cstrExePath.ReverseFind('\\') );    // ����·��

    CString cstrOldRecName;
    cstrOldRecName.Format("%s\\%s", cstrExePath, RECAPPNAME);
    CString cstrOldCfgName;
    cstrOldCfgName.Format("%s\\%s\\%s", cstrExePath, RECCFGPATH, RECCFGNAME);

    CFileFind cff;
    if (!cff.FindFile(cstrOldRecName))
    {
        // ʧ��
        cff.Close();
        return;
    }
    cff.Close();
    if (!cff.FindFile(cstrOldCfgName))
    {
        // ʧ��
        cff.Close();
        return;
    }
    cff.Close();
    
	//�ı������ļ�������ΪNormal, �Ա����޸ĺ�Ĳ���. 
	CFile* pFile = NULL;
	CFileStatus cFileOrignalStatus;
	memset( &cFileOrignalStatus, 0, sizeof(CFileStatus) );
	CFileStatus cFileChangedStatus;
	memset( &cFileChangedStatus, 0, sizeof(CFileStatus) );
	pFile->GetStatus( cstrOldCfgName, cFileOrignalStatus );
	pFile->GetStatus( cstrOldCfgName, cFileChangedStatus );

	if ( 0x00 != cFileOrignalStatus.m_attribute ) 
	{
		cFileChangedStatus.m_attribute = 0x00;
		pFile->SetStatus(cstrOldCfgName, cFileChangedStatus);
	}


    CString cstrPath, cstrPath2;
    CString cstrAppName, cstrCfgName;
    CString cstrTmp;

    if (nCopies == -1)
    {
        // �޸�����ȡԭ���������汣���
        nCopies = GetPrivateProfileInt( SEC_SYS,
                                        "InstalledCopies",
                                        1,
                                        cstrOldCfgName );    

        if (nCopies == 0)
            nCopies = 1;
    }
    // д�뿽��������
    cstrTmp.Format("%d", nCopies);
    WritePrivateProfileString(SEC_SYS,
                              "InstalledCopies",
                              cstrTmp,
                              cstrOldCfgName );

	 
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

	char szHostName[32] = _T("");
    CString cstrHostIp;
            
    // ������
    wVersionRequested = MAKEWORD( 2, 2 );
    err = WSAStartup( wVersionRequested, &wsaData );

    if ( err != 0 ) 
    {
        // ȡĬ������
        strcpy( szHostName, "KEDACOM" );
        cstrHostIp = "127.0.0.1";
    }
    else
    {
        gethostname(szHostName, 31);
        if (strlen(szHostName) == 0)
        {
            strcpy( szHostName, "KEDACOM" );
        }
        else
        {
            hostent *tHost = gethostbyname(szHostName);
            if (tHost == NULL)
            {
                cstrHostIp = "127.0.0.1";
            }
            else
            {
	            sockaddr_in sa;
                if ( tHost->h_addr_list[0] )
                {
		            memcpy (&sa.sin_addr.s_addr, tHost->h_addr_list[0],tHost->h_length);
		            cstrHostIp = inet_ntoa(sa.sin_addr);
                }    
                else
                {
                    cstrHostIp = "127.0.0.1";
                }
            }    
        }
    } 

    for (int i = 0; i < nCopies; i++)
    {
        // ����Ŀ¼
        cstrPath.Format("%s\\%d", cstrExePath, i);
        if (!cff.FindFile(cstrPath))
        {
            CreateDirectory ( cstrPath, NULL );
        }
        cff.Close();
        
        cstrPath2.Format("%s\\%s", cstrPath, RECCFGPATH);
        if (!cff.FindFile(cstrPath2))
        {
            CreateDirectory ( cstrPath2, NULL );
        }
        cff.Close();        

        // �����ļ�
        cstrAppName.Format("%s\\%s", cstrPath, RECAPPNAME);
        CopyFile( cstrOldRecName,  cstrAppName, FALSE);

        cstrCfgName.Format("%s\\%s", cstrPath2, RECCFGNAME);
        CopyFile( cstrOldCfgName,  cstrCfgName, FALSE);
        
        // ���������ļ�    

        // ID
        cstrTmp.Format("%d", 17+i);
        WritePrivateProfileString(SEC_SYS, 
                                  _T("ID"),
                                  cstrTmp,
                                  cstrCfgName);

        // Alias
        cstrTmp.Format("Recorder%d", 17+i);        
        WritePrivateProfileString(SEC_SYS, 
                                  _T("Alias"),
                                  cstrTmp,
                                  cstrCfgName);

        // IPAddr��������
        WritePrivateProfileString(SEC_SYS, 
                                  _T("IpAddr"),
                                  cstrHostIp,
                                  cstrCfgName);

        WritePrivateProfileString(SEC_SYS, 
                                  _T("MachineName"),
                                  szHostName,
                                  cstrCfgName);

        // �˿�
        cstrTmp.Format("%d", 60000+i*200);
        WritePrivateProfileString(SEC_SYS, 
                                  _T("RecorderRcvStartPort"),
                                  cstrTmp,
                                  cstrCfgName);

        cstrTmp.Format("%d", 60100+i*200);
        WritePrivateProfileString(SEC_SYS, 
                                  _T("RecorderPlayStartPort"),
                                  cstrTmp,
                                  cstrCfgName);     
        
        // ¼����ͨ��
        cstrTmp.Format("%d", 3);
        WritePrivateProfileString(SEC_SYS, 
                                  _T("RecordChannels"),
                                  cstrTmp,
                                  cstrCfgName);

        cstrTmp.Format("%d", 3);
        WritePrivateProfileString(SEC_SYS, 
                                  _T("PlayChannels"),
                                  cstrTmp,
                                  cstrCfgName);   

/*  ���´���������ɿ�ݷ�ʽ��ָ��λ�� 
        // ������ݷ�ʽ������˵�
        if (bCreateShortcut)
        {
            pisl->SetPath (cstrAppName);

            cstrTmp.Format("%s\\Recorder %d.lnk", szShortcutPath, 17+i);

            WORD uniszLnk [KDV_MAX_PATH] = {(WORD)0}; 
            // ȷ����ݷ�ʽ·����ANSI�ַ����
            MultiByteToWideChar (CP_ACP, 0, cstrTmp, -1, uniszLnk, KDV_MAX_PATH);

            //�����ݷ�ʽ
            pipf->Save(uniszLnk, TRUE);  
        }                
*/
    }
    if (bCreateShortcut)
    {
        pipf->Release ();
        pisl->Release ();
    }

    ::CoUninitialize();
}

/*====================================================================
    ������	     ��UnInstall
	����		 ��ж��¼����Ķ��������
	����ȫ�ֱ��� ����
    �������˵�� ��
	����ֵ˵��   ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	06/08/24	4.0			����        ����
====================================================================*/
void CRecloaderDlg::UnInstall()
{
    // ɾ�������������ɵ�Ŀ¼���ļ��������Ǵ������ļ�����ȥȡ����Ϊ�����ļ����ܱ������޸ġ�

    // ��ȡ��ݷ�ʽ��Ŀ��·��
    TCHAR szShortcutPath[KDV_MAX_PATH] = {0};
    BOOL bCreateShortcut = FALSE;
    bCreateShortcut = GetSpecPath(CSIDL_COMMON_PROGRAMS, szShortcutPath);  //CSIDL_DESKTOP, CSIDL_PROGRAMS
    strcat(szShortcutPath, "\\Kedacom\\Recorder4.0");

    CFileFind cff;
    
	TCHAR achExeName[KDV_MAX_PATH] = _T("");

	GetModuleFileName(AfxGetInstanceHandle(), achExeName, sizeof (achExeName));

	CString cstrExePath = achExeName;
	cstrExePath = cstrExePath.Left( cstrExePath.ReverseFind('\\') );    // ����·��

    CString cstrTarget;
    cstrTarget.Format("%s\\*.*", cstrExePath);

    BOOL bFound = cff.FindFile( cstrTarget );
    CString cstrPath;
    CString cstrTmp;
    
    while (bFound)
    {
        // ������һ��Ŀ¼
        bFound = cff.FindNextFile();
        if (!cff.IsDirectory())
        {
            continue;
        }
        if (cff.IsDots())
        {
            continue;
        }
        if (cff.GetFileName() == RECCFGPATH)
        {
            continue;
        }
        cstrPath = cff.GetFilePath();
        cstrTmp.Format("%s\\%s", cstrPath, RECAPPNAME);
        ::DeleteFile( cstrTmp );

        cstrTmp.Format("%s\\%s\\%s", cstrPath, RECCFGPATH, RECCFGNAME);
        ::DeleteFile( cstrTmp );

        cstrTmp.Format("%s\\%s", cstrPath, RECCFGPATH);
        ::RemoveDirectory( cstrTmp );

        ::RemoveDirectory( cstrPath );
    }
    cff.Close();

/*  ���´������ɾ����ݷ�ʽ
    // ɾ����ݷ�ʽ��ע�⣺����ֻ��ɾ�� \\��ʼ�˵�\����\Kedacom\Recorder4.0\*.lnk
    cstrTarget.Format("%s\\*.lnk", szShortcutPath);

    bFound = cff.FindFile( cstrTarget );
    while (bFound)
    {
        // ������һ��Ŀ¼
        bFound = cff.FindNextFile();
        if (cff.IsDots())
        {
            continue;
        }

        cstrPath = cff.GetFilePath();
        ::DeleteFile( cstrPath );
    }
    cff.Close();
*/
    ::CoUninitialize();

}

/*====================================================================
    ������	     ��OnBtnStart
	����		 ������¼�������
	����ȫ�ֱ��� ����
    �������˵�� ��
	����ֵ˵��   ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	06/08/24	4.0			����        ����
====================================================================*/
void CRecloaderDlg::OnBtnStart() 
{
    BOOL bOk;
    PROCESS_INFORMATION hInfo;
    STARTUPINFO si = {0}; 
    si.cb = sizeof(si); 
    CString cstrPath;

    ZeroMemory( &hInfo, sizeof(hInfo) );
    
    for ( int i = 0; i < m_nValidRecNum; i ++ )
    {
        if (m_apckRec[i]->GetCheck() == BST_CHECKED)
        {
            // �û�ѡ��    
            cstrPath = m_astrAppName[i].Left( m_astrAppName[i].ReverseFind('\\') );    // ����·��
                
            bOk = ::CreateProcess(m_astrAppName[i], (LPSTR)(LPCTSTR)m_astrAppName[i], NULL, NULL, FALSE, 0, NULL, 
                                  cstrPath, &si, &hInfo);
            if ( bOk )
            {
                m_vtInst.push_back( hInfo.hProcess );
                Sleep(100);
            }            
        }
    }
   
    if ( m_vtInst.size() > 0 )
    {
        // �����ɹ���
	    GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);

        // �˳�����
        // OnCancel();
    }
}

void CRecloaderDlg::OnOK() 
{
    for (int i = 0; i < m_vtInst.size(); i++)
    {
        TerminateProcess( m_vtInst[i], 0 );

        Sleep(1000);
    }    
	
	OnCancel();
}

void CRecloaderDlg::OnCancel() 
{	
	CDialog::OnCancel();
}

void CRecloaderDlg::OnBtnAll() 
{
    for (int i = 0; i < m_nValidRecNum; i++)
	    m_apckRec[i]->SetCheck(BST_CHECKED);
	
}

void CRecloaderDlg::OnBtnNone() 
{
    for (int i = 0; i < m_nValidRecNum; i++)
	    m_apckRec[i]->SetCheck(BST_UNCHECKED);	
}
