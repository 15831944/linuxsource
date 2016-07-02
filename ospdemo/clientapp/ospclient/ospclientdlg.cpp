#include "stdafx.h"
#include "clientcommon.h"
#include "ospclient.h"
#include "ospclientdlg.h"
#include "clientospapp.h"

COspClientDlg *pCwindow = NULL;						//���ļ���ʹ�ã������ര������
static void CB_UpdateProgress(s8 chProgress);		//����ͻ������׵���Ϣ���ṩ�û�������Ϣ

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
// COspClientDlg dialog

COspClientDlg::COspClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COspClientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COspClientDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COspClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COspClientDlg)
	DDX_Control(pDX, IDC_IPADDRESS2, m_pAdress);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COspClientDlg, CDialog)
	//{{AFX_MSG_MAP(COspClientDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_OUTOFMEMORY, IDC_PROGRESS1, OnCSendFileProgress)
	ON_BN_CLICKED(IDC_BUTTON1, OnConnect)
	ON_BN_CLICKED(IDC_BUTTON2, OnDisconnect)
	ON_BN_CLICKED(IDC_BUTTON3, OnChooseFile)
	ON_BN_CLICKED(IDC_BUTTON4, OnDeleteFile)
	ON_BN_CLICKED(IDC_BUTTON5, OnSendFile)
	ON_BN_CLICKED(IDC_BUTTON6, OnPauseSendFile)
	ON_BN_CLICKED(IDC_BUTTON7, OnRestartSendFile)
	ON_BN_CLICKED(IDC_BUTTON8, OnCancel)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS2, OnFieldchangedIpaddress)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COspClientDlg message handlers

BOOL COspClientDlg::OnInitDialog()
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

	SetDlgItemText(IDC_EDIT2, SERVERNODE);				//��ʼ���ڶ������ÿ�Ĭ��6682 
	SetDlgItemText(IDC_EDIT3, "��ǰ��δ���ӷ�����!");	//��ʼ��״̬��

	CString strIP	= SERVERIP;							//����IP�ؼ��ĵ�ַ
	DWORD dwIP	=   inet_addr(strIP);
	BYTE *pIP   =   (BYTE*)&dwIP;
	m_pAdress.SetAddress(*pIP, *(pIP+1), *(pIP+2), *(pIP+3));

	m_progress.SetRange(0,100);							//��ʼ����������Χ
	m_progress.SetPos(0);								//��������ʼλ��
	
	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);		//�������Ӱ���
	GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);		//�ر���ͣ����
	GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);		//�ر������ϴ�����
	GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);		//�رշ��Ͱ���
	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);		//�ر�ɾ������
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);		//�رնϿ��ҵ�
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);		//�ر�ѡ���ļ�����

	nFileOwnFlag = FALSE;								//�б����Ѿ����ļ���־
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void COspClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void COspClientDlg::OnPaint() 
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
HCURSOR COspClientDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void COspClientDlg::OnCSendFileProgress(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void COspClientDlg::OnFieldchangedIpaddress(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void COspClientDlg::OnOK() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_BUTTON1)->SetFocus();									//���ÿռ佹�㣬��Ӧ�س�
	OnConnect();
}

void COspClientDlg::OnCancel() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}

void COspClientDlg::OnConnect()												//����
{	
	BYTE *pIP = NULL;
	CString strServerIp;
	DWORD dwIP = 0;
	m_pAdress.GetAddress(dwIP);
	pIP = (unsigned char*)&dwIP;
	strServerIp.Format("%u.%u.%u.%u",*(pIP+3), *(pIP+2), *(pIP+1), *pIP);	//ת��ΪCString��

	CString strServerPort;
	GetDlgItemText(IDC_EDIT2, strServerPort);								//��ȡ�˿ں�
	CSetServerIpandPort((LPCSTR)strServerIp, _ttoi(strServerPort));			//������õ�ip���˿ں�

	pCwindow = (COspClientDlg*)this;										//ע�ᵱǰ��������ָ��
	CB_CCallBack(CB_UpdateProgress);										//���ûص�����ʵʱ��ʾ���Ͱٷֱ��Լ�״̬

	//�ж��Ƿ��ʼ��
	if ( !CInitClient() )
	{
		SetDlgItemText(IDC_EDIT3, "���ӵ�������ʧ��!");	
		MessageBox("��ʼ��ʧ��!");	
		return;
	}
	
	//�ж�APP�Ƿ񴴽��ɹ�
	if ( !CCreatApp() )
	{
		SetDlgItemText(IDC_EDIT3, "���ӵ�������ʧ��!");		
		MessageBox("����ʧ�ܣ�����IP������!");
		return;
	}
	CConnectServer();
}

void COspClientDlg::OnDisconnect()											//�Ͽ�����
{
	CDisconnectServer();
}

void COspClientDlg::OnChooseFile()											//ѡ���ļ�
{
	CFileDialog dlg( TRUE );
 	dlg.m_ofn.Flags |= OFN_ALLOWMULTISELECT|OFN_HIDEREADONLY;  
	dlg.m_ofn.lpstrFilter  =  _T("File(*.*)\0*.*\0\0");;
 	dlg.m_ofn.nMaxFile = 262;  
	
	//��ǰ�б�ǿ�
	if( nFileOwnFlag )
	{
		MessageBox("����ɾ������ӣ�һ��ֻ����ʾһ���ļ�!");
		return;
	}

	if( IDOK == dlg.DoModal() )
	{
		CString filename = dlg.GetFileName();
		m_List.AddString(filename);
		nFileOwnFlag = TRUE;
	}	
}

void COspClientDlg::OnDeleteFile()											//ɾ���ļ�	
{
	m_List.DeleteString(0);
	CRemoveFile();
	CB_CToSProgress(C_SEND_FILE_TO_S_DELETFILE);
	nFileOwnFlag = FALSE;
}

void COspClientDlg::OnSendFile()											//�����ļ�
{
	s8 achFileName[MAX_PATH] = {0};
	m_List.GetText(0,achFileName);											//ֻ��ȡ�б��һ���ļ���
	
	u32 dwerrcode = access(achFileName, 0);
	if ( dwerrcode == -1 )													//ȷ��Ŀ¼��Ϊ��
	{		
		MessageBox("��ȡ�ļ�ʧ��!");
		return;
	}

	//�ж��ϴ��ļ��Ƿ����1G
	HANDLE hFile = CreateFile(achFileName, GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwFileSizeHigh = 0;
	DWORD dwFileSize = GetFileSize(hFile, &dwFileSizeHigh);

	DWORD dwFileFullSize = 0;
	if ( hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox("������ѡ���ļ��ϴ�!");
		return;
	}
	//�ļ�����4G
	if ( dwFileSizeHigh != 0)
	{
		dwFileFullSize = (dwFileSize + dwFileSizeHigh) / 1024 / 1024 + 4096;
	}
	else
	{	//�ļ�С��4G
		dwFileFullSize = dwFileSize / 1024 / 1024;
	}

	if ( dwFileFullSize > 1024 )
	{
		MessageBox("�ϴ��ļ����󣬵��ļ��벻Ҫ����1G!");
		CloseHandle(hFile);
		return;
	}

	CloseHandle(hFile);
	OspPrintf(TRUE, FALSE, "�ϴ����ļ���СΪ��%ld M\n", dwFileFullSize);
	CUploadFile(achFileName, dwFileSize);
}

void COspClientDlg::OnPauseSendFile()										//������ͣ
{
	CPauseUploadFile();
}

void COspClientDlg::OnRestartSendFile()										//���·���
{
	CRestartUploadFile();
}

/////////////////�ص������û�ʵ�ֲ���:�Բ������д���////////////
//����CB_UpdateProgress���û�ʵ�ֶԲ����Ĵ���
//�ò�����ȡֵΪ1-100�Լ������enum�ͻ���״̬
static void CB_UpdateProgress(s8 chProgress)								//����ͻ������׵���Ϣ���ṩ�û�������Ϣ
{
	static s8 chTmp = 0;	
	if ( chProgress < 100 )
	{
		if ( chTmp != chProgress)
		{
			pCwindow->m_progress.SetStep(chProgress - chTmp);				//�����߶��ľ���
			chTmp = chProgress;												//��¼�ϴα䶯ֵ
			pCwindow->m_progress.StepIt();									//�������߶�
		}		
	}
	else
	{
		switch(chProgress)
		{
		case C_SEND_FILE_TO_S_SETERR:										//C->S����ip����
			{
				pCwindow->MessageBox("����IP��������������!");
			}
			break;
		case C_SEND_FILE_TO_S_UPLOADING:									//C->S�����ļ���
			{	
				pCwindow->SetDlgItemText(IDC_EDIT3, "�ϴ��ļ��У����Ժ�...!");
				pCwindow->GetDlgItem(IDC_BUTTON6)->EnableWindow(TRUE);		//������ͣ����
				pCwindow->GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);		//���öϿ�����
				pCwindow->GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);		//�ر�ɾ������
				pCwindow->GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);		//�رշ��Ͱ���
				pCwindow->GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);		//�ر����·��Ͱ���
				pCwindow->GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);		//�ر�ѡ���ļ�����
				pCwindow->GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);		//�ر����Ӱ���
			}
			break;
		case C_SEND_FILE_TO_S_OVER:											//C->S�����ļ�����
			{
				pCwindow->MessageBox("�������!");
				pCwindow->SetDlgItemText(IDC_EDIT3, "��ǰ�Ѿ����ӵ�������!");
				pCwindow->GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);		//���÷��Ͱ���
				pCwindow->GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);		//����ɾ������
				pCwindow->GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);		//����ѡ���ļ�����
				pCwindow->GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);		//���öϿ�����
				pCwindow->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);		//�ر���ͣ����
				pCwindow->GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);		//�ر����Ӱ���
				pCwindow->GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);		//�ر����·��Ͱ���
				pCwindow->m_progress.SetPos(0);								//����������
				chTmp = 0;
			}
			break;
		case C_SEND_FILE_TO_S_FAIL:											//C->S�����ļ�ʧ��
			{
				pCwindow->MessageBox("����ʧ��!");
				pCwindow->GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);		//���÷��Ͱ���
				pCwindow->GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);		//����ɾ������
				pCwindow->GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);		//����ѡ���ļ�����
				pCwindow->GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);		//���öϿ�����
				pCwindow->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);		//�ر���ͣ����
				pCwindow->GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);		//�ر����Ӱ���
				pCwindow->GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);		//�ر����·��Ͱ���
				pCwindow->m_progress.SetPos(0);								//����������
				chTmp = 0;
			}
			break;
		case C_SEND_FILE_TO_S_BUSYNODEL:									//C->S�����ļ�ʱ����ɾ���ļ�
			{
				pCwindow->SetDlgItemText(IDC_EDIT3, "�����ϴ�������ͣ���ϴ�������ɾ��!");
			}
			break;
		case C_SEND_FILE_TO_S_PAUSE:										//C->S�����ļ���ͣ��
			{
				pCwindow->SetDlgItemText(IDC_EDIT3, "�ϴ���ͣ�У���ѡ��ָ��ϴ���ɾ���ļ�!");
				pCwindow->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);		//�ر���ͣ����
				pCwindow->GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);		//�ر����Ӱ���
				pCwindow->GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);		//�رշ��Ͱ���
				pCwindow->GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);		//�ر�ѡ���ļ�����
				pCwindow->GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);		//����ɾ������
				pCwindow->GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);		//���������ϴ�����
				pCwindow->GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);		//���öϿ�����
			}
			break;
		case C_SEND_FILE_TO_S_NOPAUSE:										//C->S�����ļ�С��50M����������ͣ
			{
				pCwindow->SetDlgItemText(IDC_EDIT3, "�����ļ�С��50M����������ͣ!");
			}
			break;
		case C_SEND_FILE_TO_S_COEENCTING:									//��ʾ�������ӣ���ֹ���粻���������û��ȴ���ʾ
			{
				pCwindow->SetDlgItemText(IDC_EDIT3, "�������ӵ�������, ���Ժ�...");
			}
			break;
		case C_SEND_FILE_TO_S_COEENCT:										//C->S�Ѿ���������״̬
			{
				pCwindow->SetDlgItemText(IDC_EDIT3, "��ǰ�Ѿ����ӵ�������!");
				pCwindow->MessageBox("��������!");
				pCwindow->GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);		//�ر����Ӱ���
				pCwindow->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);		//�ر���ͣ����
				pCwindow->GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);		//�ر������ϴ�����
				pCwindow->GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);		//���öϿ�����
				pCwindow->GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);		//���÷��Ͱ���
				pCwindow->GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);		//����ɾ������
				pCwindow->GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);		//����ѡ���ļ�����
			}
			break;
		case C_SEND_FILE_TO_S_NACKCOEENCT:									//C->S����ʧ��״̬
			{
				pCwindow->SetDlgItemText(IDC_EDIT3, "��ǰ���ӷ�����ʧ��!");
				pCwindow->MessageBox("��������ʧ��!");
				pCwindow->GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);		//�������Ӱ���
				pCwindow->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);		//�ر���ͣ����
				pCwindow->GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);		//�ر������ϴ�����
				pCwindow->GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);		//�رշ��Ͱ���
				pCwindow->GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);		//�ر�ɾ������
				pCwindow->GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);		//�رնϿ��ҵ�
				pCwindow->GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);		//�ر�ѡ���ļ�����
			}
			break;
		case C_SEND_FILE_TO_S_DICCOEENCT:									//C->S�Ͽ����ӵ�״̬��ʾ
			{
				pCwindow->SetDlgItemText(IDC_EDIT3, "��ǰ�Ѿ��Ͽ�����!");
				pCwindow->GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);		//�������Ӱ���
				pCwindow->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);		//�ر���ͣ����
				pCwindow->GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);		//�ر������ϴ�����
				pCwindow->GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);		//�رշ��Ͱ���
				pCwindow->GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);		//�ر�ɾ������
				pCwindow->GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);		//�رնϿ��ҵ�
				pCwindow->GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);		//�ر�ѡ���ļ�����
				pCwindow->m_progress.SetPos(0);								//���������
				chTmp = 0;
			}
			break;
		case C_SEND_FILE_TO_S_DELETFILE:									//C->Sɾ���ļ���״̬��ʾ
			{
				pCwindow->SetDlgItemText(IDC_EDIT3, "��ǰ�Ѿ����ӵ�������!");
				pCwindow->GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);		//����ѡ���ļ�����
				pCwindow->GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);		//���öϿ�����
				pCwindow->GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);		//����ɾ������
				pCwindow->GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);		//���÷��Ͱ���
				pCwindow->GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);		//�ر����Ӱ���
				pCwindow->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);		//�ر���ͣ����
				pCwindow->GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);		//�ر����·��Ͱ���
				pCwindow->m_progress.SetPos(0);								//���������
				chTmp = 0;
			}
			break;
		default:
			{
				//��������
			}
			break;
		}
	}
}

//end




