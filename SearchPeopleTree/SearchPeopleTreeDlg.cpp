// SearchPeopleTreeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SearchPeopleTree.h"
#include "SearchPeopleTreeDlg.h"
#include "Search.h"
#include "type.h"
#include "io.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

s8 g_SeartchWord[NUM_KEY][NAME_LEN]={0};
s8	m_achPathFileName[MAX_NAME_LEN] = "./seartchdatabase";

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
// CSearchPeopleTreeDlg dialog

CSearchPeopleTreeDlg::CSearchPeopleTreeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSearchPeopleTreeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSearchPeopleTreeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSearchPeopleTreeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchPeopleTreeDlg)
	DDX_Control(pDX, IDC_LIST2, m_ctList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSearchPeopleTreeDlg, CDialog)
	//{{AFX_MSG_MAP(CSearchPeopleTreeDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnAdd)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, OnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT3, OnChangeEdit3)
	ON_BN_CLICKED(IDC_BUTTON3, OnDel)
	ON_BN_CLICKED(IDC_BUTTON4, OnSearchByKeyWord)	
	ON_BN_CLICKED(IDC_BUTTON2, OnSearch)
	ON_EN_CHANGE(IDC_EDIT4, OnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT5, OnChangeEdit5)
	ON_EN_CHANGE(IDC_EDIT6, OnChangeEdit6)
	ON_EN_CHANGE(IDC_EDIT7, OnChangeEdit7)
	ON_EN_CHANGE(IDC_EDIT8, OnChangeEdit8)
	ON_EN_CHANGE(IDC_EDIT9, OnChangeEdit9)
	ON_EN_CHANGE(IDC_EDIT10, OnChangeEdit10)
	ON_EN_CHANGE(IDC_EDIT11, OnChangeEdit11)
	ON_EN_CHANGE(IDC_EDIT13, OnChangeEdit13)
	ON_EN_CHANGE(IDC_EDIT12, OnChangeEdit12)
	ON_NOTIFY(NM_CLICK, IDC_LIST2, OnClickList2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchPeopleTreeDlg message handlers

BOOL CSearchPeopleTreeDlg::OnInitDialog()
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
	m_ctList.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE);
	m_ctList.InsertColumn(0,"����",LVCFMT_LEFT,70);
	m_ctList.InsertColumn(1,"�Ա�",LVCFMT_LEFT,40);
	m_ctList.InsertColumn(2,"����",LVCFMT_LEFT,40);
	m_ctList.InsertColumn(3,"�ǳ�",LVCFMT_LEFT,80);
	UpdateList();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSearchPeopleTreeDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSearchPeopleTreeDlg::OnPaint() 
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
HCURSOR CSearchPeopleTreeDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//ˢ���б�
void CSearchPeopleTreeDlg::UpdateList()
{
	TPeopleTree pt;
	pt.Init();
	
	FILE *m_phFile;	
	
	u32 lPtLen = sizeof(pt);
	//����Ϣ��pt
	u8 byCount = 0;
	CString sName;
	m_phFile = fopen(m_achPathFileName, "rb+");
	if ( m_phFile != NULL )
	{
		fseek(m_phFile, 0L,SEEK_END);
		u32 lFileLen = ftell(m_phFile);
		fclose(m_phFile);	
		m_ctList.DeleteAllItems();
		for(u32 byIdx = 0; byIdx < lFileLen; byIdx += lPtLen)
		{
			m_phFile = fopen(m_achPathFileName, "rb+");
			fseek(m_phFile, byIdx, SEEK_SET);
			fread(&pt, lPtLen, 1, m_phFile);
			fclose(m_phFile);
			//��ʾ���б�
			if (strcmp(pt.GetName(), "") != 0 && strcmp(pt.GetName(), " ") != 0)
			{
				m_ctList.InsertItem(byCount, pt.GetName());
				m_ctList.SetItemText(byCount,1,pt.GetSex());
				m_ctList.SetItemText(byCount,2,pt.GetAge());
				m_ctList.SetItemText(byCount,3,pt.GetAliens());
				byCount++;
			}	
		}
		return;
	}
}

void CSearchPeopleTreeDlg::AddInfoToPt(TPeopleTree &pt)
{
	pt.Init();

	CString sName;					//�û�����
	CString sSheng;					//ʡ
	CString sShi;					//��
	CString sXian;					//��
	CString sJie;					//�ֵ�
	CString sSex;					//�Ա�
	CString sAge;					//����
	CString sAliens;				//�ǳ�
	CString sPhone;					//�绰
	CString sQQ;					//QQ
	CString sCompany;				//������λ
	CString sZhiwu;					//ְ��
	CString sPeoPleMiaoShu;			//��������

	//����
	GetDlgItemText(IDC_EDIT1, sName);
	pt.SetName(sName, NAME_LEN);

	//�Ա�
	GetDlgItemText(IDC_EDIT2, sSex);
	pt.SetSex(sSex, NAME_LEN);

	//����
	GetDlgItemText(IDC_EDIT3, sAge);
	pt.SetAge(sAge, NAME_LEN);

	//�ǳ�
	GetDlgItemText(IDC_EDIT4, sAliens);
	pt.SetAliens(sAliens, NAME_LEN);

	//�绰
	GetDlgItemText(IDC_EDIT5, sPhone);
	pt.SetPhone(sPhone, NAME_LEN);

	//QQ
	GetDlgItemText(IDC_EDIT6, sQQ);
	pt.SetQQ(sQQ, NUM_LEN);

	//ʡ
	GetDlgItemText(IDC_EDIT7, sSheng);
	pt.SetSheng(sSheng, PLACE_LEN);

	//��
	GetDlgItemText(IDC_EDIT8, sShi);
	pt.SetShi(sShi, PLACE_LEN);

	//��
	GetDlgItemText(IDC_EDIT9, sXian);
	pt.SetXian(sXian, PLACE_LEN);

	//�ֵ�
	GetDlgItemText(IDC_EDIT10, sJie);
	pt.SetJie(sJie, PLACE_LEN);

	//ְ��
	GetDlgItemText(IDC_EDIT11, sZhiwu);
	pt.SetZhiwu(sZhiwu, PLACE_LEN);

	//��˾
	GetDlgItemText(IDC_EDIT12, sCompany);
	pt.SetCompany(sCompany, PLACE_LEN);

	//��ǩ
	GetDlgItemText(IDC_EDIT13, sPeoPleMiaoShu);
	pt.SetPeoPleMiaoShu(sPeoPleMiaoShu, NAME_LEN);
}

BOOL32 CSearchPeopleTreeDlg::IsStrCmp(TPeopleTree &pt, const s8 *cKeyWord)
{
	if (cKeyWord == NULL)
	{
		return FALSE;
	}

	if (strcmp(pt.GetName(), "") != 0 && strstr(pt.GetName(), cKeyWord))
	{
		return TRUE;
	}
	else if (strcmp(pt.GetSex(), "") != 0 && strstr(pt.GetSex(), cKeyWord))
	{
		return TRUE;
	}	
	else if (strcmp(pt.GetAge(), "") != 0 && strstr(pt.GetAge(), cKeyWord))
	{
		return TRUE;
	}
	else if (strcmp(pt.GetAliens(), "") != 0 && strstr(pt.GetAliens(), cKeyWord))
	{
		return TRUE;
	}
	else if (strcmp(pt.GetPhone(), "") != 0 && strstr(pt.GetPhone(), cKeyWord))
	{
		return TRUE;
	}
	else if (strcmp(pt.GetQQ(), "") != 0 && strstr(pt.GetQQ(), cKeyWord))
	{
		return TRUE;
	}
	else if (strcmp(pt.GetSheng(), "") != 0 && strstr(pt.GetSheng(), cKeyWord))
	{
		return TRUE;
	}
	else if (strcmp(pt.GetShi(), "") != 0 && strstr(pt.GetShi(), cKeyWord))
	{
		return TRUE;
	}
	else if (strcmp(pt.GetXian(), "") != 0 && strstr(pt.GetXian(), cKeyWord))
	{
		return TRUE;
	}
	else if (strcmp(pt.GetJie(), "") != 0 && strstr(pt.GetJie(), cKeyWord))
	{
		return TRUE;
	}
	else if (strcmp(pt.GetZhiwu(), "") != 0 && strstr(pt.GetZhiwu(), cKeyWord))
	{
		return TRUE;
	}
	else if (strcmp(pt.GetCompany(), "") != 0 && strstr(pt.GetCompany(), cKeyWord))
	{
		return TRUE;
	}
	else if (strcmp(pt.GetPeoPleMiaoShu(), "") != 0 && strstr(pt.GetPeoPleMiaoShu(), cKeyWord))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL32 CSearchPeopleTreeDlg::IsCmpByKeyWord(TPeopleTree &pt, CString cKeyWord1, CString cKeyWord2, CString cKeyWord3)
{
	//��ʾ���б�
	if (!cKeyWord1.IsEmpty())
	{
		if (!cKeyWord2.IsEmpty())
		{
			if (!cKeyWord3.IsEmpty())
			{
				if (IsStrCmp(pt, cKeyWord1) && IsStrCmp(pt, cKeyWord2) && IsStrCmp(pt, cKeyWord3))
				{
					return TRUE;
				}
			}
			else
			{
				if (IsStrCmp(pt, cKeyWord1) && IsStrCmp(pt, cKeyWord2))
				{
					return TRUE;
				}
			}
		}
		else
		{
			if (!cKeyWord3.IsEmpty())
			{
				if (IsStrCmp(pt, cKeyWord1) && IsStrCmp(pt, cKeyWord3))
				{
					return TRUE;
				}
			}
			else
			{
				if (IsStrCmp(pt, cKeyWord1))
				{
					return TRUE;
				}
			}
		}
	}
	else if (!cKeyWord2.IsEmpty())
	{
		if (!cKeyWord3.IsEmpty())
		{
			if (IsStrCmp(pt, cKeyWord2) && IsStrCmp(pt, cKeyWord3))
			{
				return TRUE;
			}
		}
		else
		{
			if (IsStrCmp(pt, cKeyWord2))
			{
				return TRUE;
			}
		}
	}
	else if (!cKeyWord3.IsEmpty())
	{
		if (IsStrCmp(pt, cKeyWord3))
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CSearchPeopleTreeDlg::OnSearchByKeyWord()
{
	CString cKeyWord1;
	//��ȡ����
	GetDlgItemText(IDC_EDIT14, cKeyWord1);
	
	CString cKeyWord2;
	//��ȡ����
	GetDlgItemText(IDC_EDIT15, cKeyWord2);
	
	CString cKeyWord3;
	//��ȡ����
	GetDlgItemText(IDC_EDIT16, cKeyWord3);
	
	TPeopleTree pt;
	pt.Init();
	
	FILE *m_phFile;	
	
	u32 lPtLen = sizeof(pt);
	//����Ϣ��pt
	u8 byCount = 0;
	CString sName;
	m_phFile = fopen(m_achPathFileName, "rb+");
	if ( m_phFile != NULL )
	{
		fseek(m_phFile, 0L,SEEK_END);
		u32 lFileLen = ftell(m_phFile);
		fclose(m_phFile);	
		m_ctList.DeleteAllItems();
		for(u32 byIdx = 0; byIdx < lFileLen; byIdx += lPtLen)
		{
			m_phFile = fopen(m_achPathFileName, "rb+");
			fseek(m_phFile, byIdx, SEEK_SET);
			fread(&pt, lPtLen, 1, m_phFile);
			fclose(m_phFile);
			if (strcmp(pt.GetName(), "") == 0)
			{
				continue;
			}

			if (IsCmpByKeyWord(pt, cKeyWord1, cKeyWord2, cKeyWord3))
			{
				m_ctList.InsertItem(byCount, pt.GetName());
				m_ctList.SetItemText(byCount,1,pt.GetSex());
				m_ctList.SetItemText(byCount,2,pt.GetAge());
				m_ctList.SetItemText(byCount,3,pt.GetAliens());
				byCount++;
				continue;
			}

			//���ǿյĻ���ˢ���б�
			if (cKeyWord1.IsEmpty() && cKeyWord2.IsEmpty() && cKeyWord3.IsEmpty())
			{
				m_ctList.InsertItem(byCount, pt.GetName());
				m_ctList.SetItemText(byCount,1,pt.GetSex());
				m_ctList.SetItemText(byCount,2,pt.GetAge());
				m_ctList.SetItemText(byCount,3,pt.GetAliens());
				byCount++;
				continue;
			}
		}
		return;
	}
}

void CSearchPeopleTreeDlg::OnDel()
{
	CString cName;
	//��ȡ����
	GetDlgItemText(IDC_EDIT1, cName);

	//ɾ��
	TPeopleTree pt;
	ReadOrWriteInFile(pt, cName, DELET);

	//�����б�
	UpdateList();
}

void CSearchPeopleTreeDlg::ReadOrWriteInFile(TPeopleTree &pt, const s8 *sName,/* = NULL,*/ u8 byDo/* = NONE*/)
{
	//��¼��ȡ����Ϣ
	TPeopleTree tTmpPt;
	
	FILE *m_phFile;																			
	
	//�鿴�Ƿ��и��ļ�
	BOOL32 bIsExit = access(m_achPathFileName, 0) == 0;
	if ( !bIsExit )
	{
		//��д��ʽ�򿪣�û�оʹ���
		m_phFile = fopen(m_achPathFileName, "wb+");
		fclose(m_phFile);
	}
	
	if (strcmp(sName, "") == 0 || strcmp(sName, " ") == 0)
	{
		MessageBox("�����������������룡");
		return;
	}
	
	u32 lPtLen = sizeof(pt);
	//ֻ����ʽ�򿪣�Ϊ��ȡ�ļ�����
	m_phFile = fopen(m_achPathFileName, "rb+");
	if ( m_phFile != NULL )
	{
		fseek(m_phFile, 0L,SEEK_END);
		u32 lFileLen = ftell(m_phFile);
		fclose(m_phFile);
		if (lFileLen == 0)
		{	
			if (byDo == WRITE)
			{
				//׷�ӷ�ʽ�򿪣����ļ����ݺ�׷��
				m_phFile = fopen(m_achPathFileName, "ab+");	
				fwrite(&pt, lPtLen, 1, m_phFile);
				fclose(m_phFile);
				MessageBox("��ӳɹ���");
				UpdateList();
				return;
			}
			else if (byDo == READ)
			{
				MessageBox("��ȡʧ�ܣ�");
				return;
			}
			else if (byDo == DELET)
			{
				MessageBox("�����ڸü�¼��");
				return;
			}
		}
		
		for(u32 byIdx = 0; byIdx < lFileLen; byIdx += lPtLen)
		{
			tTmpPt.Init();
			m_phFile = fopen(m_achPathFileName, "rb+");
			fseek(m_phFile, byIdx, SEEK_SET);
			fread(&tTmpPt, lPtLen, 1, m_phFile);
			fclose(m_phFile);
			if (strcmp(tTmpPt.GetName(), sName) == 0)
			{
				if (byDo == WRITE || byDo == DELET)
				{
					//д��ʽ
					m_phFile = fopen(m_achPathFileName, "rb+");	
					fseek(m_phFile, byIdx, SEEK_SET);
					if (byDo == DELET)
					{
						pt.Init();
					}
					fwrite(&pt, lPtLen, 1, m_phFile);
					fclose(m_phFile);
					MessageBox("�޸ļ�¼�ɹ���");
					return;
				}
				else if (byDo == READ)
				{
					pt = tTmpPt;
					return;
				}
			}
		}
		
		if (byIdx >= lFileLen)
		{
			if (byDo == WRITE)
			{
				//׷�ӷ�ʽ�򿪣����ļ����ݺ�׷��
				m_phFile = fopen(m_achPathFileName, "ab+");	
				fwrite(&pt, lPtLen, 1, m_phFile);
				fclose(m_phFile);
				MessageBox("׷�ӳɹ���");
				return;
			}
			else if (byDo == READ)
			{
				MessageBox("�����ڸü�¼��");
				return;
			}
		}		
		return;
	}
	else
	{
		MessageBox("���ļ�ʧ�ܣ�");
		return;
	}
}

void CSearchPeopleTreeDlg::OnAdd() 
{
	// TODO: Add your control notification handler code here

	TPeopleTree pt;
	//ץȡ��������ݵ�pt
	AddInfoToPt(pt);
	
	CString sName;
	//��ȡ����
	GetDlgItemText(IDC_EDIT1, sName);

	//��ӵ��ļ�
	ReadOrWriteInFile(pt, sName, WRITE);

	//ˢ���б�
	UpdateList();
}

void CSearchPeopleTreeDlg::OnDisPlay(TPeopleTree &pt) 
{
	//��ʾ����
	SetDlgItemText(IDC_EDIT1, pt.GetName());
	//�Ա�
	SetDlgItemText(IDC_EDIT2, pt.GetSex());
	//����
	SetDlgItemText(IDC_EDIT3, pt.GetAge());
	//�ǳ�
	SetDlgItemText(IDC_EDIT4, pt.GetAliens());
	//�绰
	SetDlgItemText(IDC_EDIT5, pt.GetPhone());
	//QQ
	SetDlgItemText(IDC_EDIT6, pt.GetQQ());
	//ʡ
	SetDlgItemText(IDC_EDIT7, pt.GetSheng());
	//��
	SetDlgItemText(IDC_EDIT8, pt.GetShi());
	//��
	SetDlgItemText(IDC_EDIT9, pt.GetXian());
	//�ֵ�
	SetDlgItemText(IDC_EDIT10, pt.GetJie());
	//ְ��
	SetDlgItemText(IDC_EDIT11, pt.GetZhiwu());
	//��˾
	SetDlgItemText(IDC_EDIT12, pt.GetCompany());
	//��ǩ
	SetDlgItemText(IDC_EDIT13, pt.GetPeoPleMiaoShu());
}

void CSearchPeopleTreeDlg::OnSearch() 
{
	// TODO: Add your control notification handler code here
	
	//��ȡ����
	CString cName;
	GetDlgItemText(IDC_EDIT1, cName);
	
	//ͨ����������
	TPeopleTree pt;
	ReadOrWriteInFile(pt, cName, READ);

	//��ʾ
	OnDisPlay(pt);

	//ˢ���б�
	UpdateList();
}

void CSearchPeopleTreeDlg::OnClickList2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem != -1)
	{
		/*CString strtemp;
		strtemp.Format("�������ǵ�%d�е�%d��",
			pNMListView->iItem, pNMListView->iSubItem);
        AfxMessageBox(strtemp);*/

		//ץȡ�����������Ϣ���˴�Ϊ����
		CString sStr = m_ctList.GetItemText(pNMListView->iItem, 0);
		if(strcmp(sStr, "") != 0 && strcmp(sStr, " ") != 0)
		{ 
			TPeopleTree pt;
			ReadOrWriteInFile(pt, sStr, READ);
			OnDisPlay(pt);
		}
    }

	*pResult = 0;
}

void CSearchPeopleTreeDlg::OnChangeEdit1() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CSearchPeopleTreeDlg::OnChangeEdit2() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CSearchPeopleTreeDlg::OnChangeEdit3() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CSearchPeopleTreeDlg::OnChangeEdit4() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CSearchPeopleTreeDlg::OnChangeEdit5() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CSearchPeopleTreeDlg::OnChangeEdit6() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CSearchPeopleTreeDlg::OnChangeEdit7() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CSearchPeopleTreeDlg::OnChangeEdit8() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CSearchPeopleTreeDlg::OnChangeEdit9() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CSearchPeopleTreeDlg::OnChangeEdit10() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CSearchPeopleTreeDlg::OnChangeEdit11() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CSearchPeopleTreeDlg::OnChangeEdit13() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CSearchPeopleTreeDlg::OnChangeEdit12() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CSearchPeopleTreeDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CSearchPeopleTreeDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();
}

void CSearchPeopleTreeDlg::OnClickTree2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CSearchPeopleTreeDlg::OnSelchangeList1() 
{
	// TODO: Add your control notification handler code here
	
}

