// ospserverdlg.cpp : implementation file
//

#include "stdafx.h"
#include "ospserver.h"
#include "ospserverdlg.h"
#include "direct.h"
#include "serverospapp.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

s8	g_achServerBase[MAX_NAME_LEN];						//Ĭ�����ø�Ŀ¼D:\\tmp
s32	g_nServerPort = SERVER_LISTEN_PORT;					//Ĭ�����÷������˿ں�6682

#ifdef __cplusplus
}
#endif  /* __cplusplus */

static void CB_SRecProgress(s8 chProgress);				//����ͻ������׵���Ϣ���ṩ�û�������Ϣ

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
// COspserverDlg dialog

COspserverDlg::COspserverDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COspserverDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COspserverDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COspserverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COspserverDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COspserverDlg, CDialog)
	//{{AFX_MSG_MAP(COspserverDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_OK, OnOk)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedDecgen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COspserverDlg message handlers

BOOL COspserverDlg::OnInitDialog()
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

	CB_ServerCallBack(CB_SRecProgress);						//ע��ص�����

	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);		

	SetDlgItemText(IDC_EDIT3, BASEDOCUMENT);	//��ʼ���ڶ������ÿ�Ĭ�ϸ�Ŀ¼D:\\tmp 
	SetDlgItemText(IDC_EDIT4, SERVERNODE);		//��ʼ�����������ÿ�Ĭ�϶˿ں�6682
	SetDlgItemText(IDC_EDIT2, "������δ��ʼ��!");

	mkdir(BASEDOCUMENT);						//����Ĭ��Ŀ¼

	ZeroMemory(g_achServerBase, MAX_NAME_LEN);
	memcpy(g_achServerBase, BASEDOCUMENT, MAX_NAME_LEN);	//��ʼ��ΪD:\\tmp

	g_zServerOspApp.CInitServer();							//��ʼ��server
	g_zServerOspApp.CCreateServerTcpNode();					//���ӽ�㲢����APP

	return TRUE;  
}

void COspserverDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void COspserverDlg::OnPaint() 
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
HCURSOR COspserverDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void COspserverDlg::OnServerSpace() 
{

}

void COspserverDlg::OnSetBaseDocument() 
{
	
}

void COspserverDlg::OnBaseDocument() 
{
	
}

void COspserverDlg::OnCancel() 
{
	CDialog::OnCancel();
}

/*==========================================================
������ OnBnClickedDecgen
���� ������������ѡ���ļ���ŵ�Ŀ¼
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵������
============================================================*/
void COspserverDlg::OnBnClickedDecgen() 
{
	s8 szPath[MAX_PATH];     //���ѡ���Ŀ¼·�� 
    CString str;	
    ZeroMemory(szPath, sizeof(szPath));   
	
    BROWSEINFO bi;   
    bi.hwndOwner = m_hWnd;   
    bi.pidlRoot = NULL;   
    bi.pszDisplayName = szPath;   
    bi.lpszTitle = "��ѡ���ļ���ŵ�Ŀ¼��";   
    bi.ulFlags = 0;   
    bi.lpfn = NULL;   
    bi.lParam = 0;   
    bi.iImage = 0;   
    //����ѡ��Ŀ¼�Ի���
    LPITEMIDLIST lp = SHBrowseForFolder(&bi);   
	
    if(lp && SHGetPathFromIDList(lp, szPath))   
    {
        str.Format("%s", szPath);
        SetDlgItemText(IDC_EDIT3, str);	//���õڶ������ÿ� 		       
    }
    else 
	{
        MessageBox("��Ч��Ŀ¼��������ѡ��"); 
	}	
}

/*==========================================================
������ OnOK
���� ���������������ú���ȷ��
�㷨ʵ�֣�<��ѡ��>
����˵������
����ֵ˵������
============================================================*/
void COspserverDlg::OnOk() 
{
	CString strNum3;
	CString strNum4;
	
	GetDlgItemText(IDC_EDIT3, strNum3);											//�Ի���3���ø�Ŀ¼
	GetDlgItemText(IDC_EDIT4, strNum4);											//�Ի���4���ö˿ں�
	
	strncpy(g_achServerBase, (LPCSTR)strNum3, MAX_NAME_LEN);					//ת��CString to char/arrary
	g_nServerPort = _ttoi(strNum4);												//ת��CString to unsigned long
	

	MessageBox("���óɹ�!");
	mkdir(g_achServerBase);														//����Ŀ¼							
	OspLog(LOGPRINTLEVELH, "\n��Ŀ¼:%s, �˿ں�:%d\n", g_achServerBase, g_nServerPort);
	GetDlgItem(IDC_EDIT3)->EnableWindow(FALSE);		//�ر����ø�Ŀ¼��
	GetDlgItem(IDC_EDIT4)->EnableWindow(FALSE);		//�ر����ö˿ںſ�
	GetDlgItem(ID_OK)->EnableWindow(FALSE);			//�ر�ȷ����

}

/////////////////�ص������û�ʵ�ֲ���:�Բ������д���////////////
static void CB_SRecProgress(s8 chProgress)					//����ͻ������׵���Ϣ���ṩ�û�������Ϣ
{
	switch(chProgress)
	{
	case SERVER_INIT_OK:									//��������ʼ���ɹ�
		{
			SetDlgItemText(AfxGetApp()->m_pMainWnd->m_hWnd, IDC_EDIT2, "��������ʼ���ɹ�!");
		}
		break;
	case SERVER_INIT_FAIL:									//��������ʼ��ʧ��
		{
			SetDlgItemText(AfxGetApp()->m_pMainWnd->m_hWnd, IDC_EDIT2, "��������ʼ��ʧ��!");
		}
		break;
	case SERVER_NOSPACE:									//�������ռ䲻��
		{
			MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "�������ռ䲻��!", NULL, MB_OK);
		}
		break;
	default:
		{
			//��������
		}
		break;
	}
}

//end


