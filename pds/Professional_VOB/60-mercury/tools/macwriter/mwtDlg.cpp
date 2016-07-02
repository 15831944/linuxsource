// mwtDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mwt.h"
#include "mwtDlg.h"
#include "MacSerializer.h"
#include "ShowMacDlg.h"
#include "MacSNDlg.h"
#include "snlog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMacSerializer g_cSerializer;

const s8* GetRunPath()
{
    static s8 szPath[KDV_MAX_PATH] = {0};

    ::GetModuleFileName(::AfxGetInstanceHandle() , szPath, sizeof(szPath));

    CString cstrPath = szPath;
    cstrPath = cstrPath.Left( cstrPath.ReverseFind('\\') + 1 );

    memset( szPath, 0, sizeof(szPath) );
    strncpy( szPath, cstrPath, sizeof(szPath) - 1 );
    return szPath;
}

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
// CMwtDlg dialog

CMwtDlg::CMwtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMwtDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMwtDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMwtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMwtDlg)
	DDX_Control(pDX, IDC_LS_MACSN, m_lsMacSn);
	DDX_Control(pDX, IDC_PROGRESS_WRITE, m_prgWrite);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMwtDlg, CDialog)
	//{{AFX_MSG_MAP(CMwtDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_BROWSE, OnBtnBrowse)
	ON_BN_CLICKED(IDC_BTN_MODIFYMAC, OnBtnModifymac)
	ON_BN_CLICKED(IDC_BTN_SNLIST, OnBtnSnlist)
	ON_BN_CLICKED(IDC_BTN_LOG, OnBtnLog)
	ON_BN_CLICKED(IDC_BTN_REFRESH, OnBtnRefresh)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_SHOWMAC, OnBtnShowmac)
	ON_BN_CLICKED(IDC_CK_AUTO, OnCkAuto)
	ON_BN_CLICKED(IDC_BTN_ADDSN, OnBtnAddsn)
	ON_BN_CLICKED(IDC_BTN_DELSN, OnBtnDelsn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMwtDlg message handlers

BOOL CMwtDlg::OnInitDialog()
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
    CheckDlgButton(IDC_RD_DIFFFOLDER, BST_CHECKED);
    CheckDlgButton(IDC_RD_DIFFNAME,   BST_UNCHECKED);

    GetDlgItem(IDC_CK_FILL)->ShowWindow(SW_HIDE);
    CheckDlgButton(IDC_CK_FILL,  BST_UNCHECKED);

    LoadDefaultMacAddr();

    g_SNLogMsg.LoadHistorySNFromFile();

    SetDlgItemText( IDC_ET_MACADDR, m_cMacCurr.ToString() );
    SetDlgItemText( IDC_ET_MAC4SN, m_cMacCurr.ToString() );

    SetDlgItemInt( IDC_ET_TARGETNUM, 1, FALSE );

    SetDlgItemText( IDC_ET_FILENAME, "u-boot.bin" );
	
    // ��ʼ���б�
    m_lsMacSn.InsertColumn( 0,  "���к�",  LVCFMT_CENTER, 160 );
    m_lsMacSn.InsertColumn( 1, "��ַ����", LVCFMT_CENTER, 70);
    m_lsMacSn.InsertColumn( 2, "MAC��ַ0", LVCFMT_CENTER, 140 );
    m_lsMacSn.InsertColumn( 3, "MAC��ַ1", LVCFMT_CENTER, 140);
    
	m_lsMacSn.SetExtendedStyle(m_lsMacSn.GetExtendedStyle()
					   		   | LVS_EX_FULLROWSELECT );
    
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMwtDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMwtDlg::OnPaint() 
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
HCURSOR CMwtDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMwtDlg::OnBtnBrowse() 
{
    // ����ļ�
    OPENFILENAME ofn;      
    TCHAR szFile[KDV_MAX_PATH] = _T("");   
    TCHAR szFilter[256];

    _stprintf( szFilter, _T("%s(*.bin)|*.bin||"), "u-boot ROM �ļ�" );
    LPTSTR lptrFilter = szFilter;
    while ( ( lptrFilter = _tcschr( lptrFilter, '|' ) ) != NULL )
    {
        lptrFilter[0] = '\0';
        lptrFilter ++;
    }

    // Initialize OPENFILENAME
    ZeroMemory( &ofn, sizeof (ofn) );
    ofn.lStructSize = sizeof (ofn);
    ofn.hwndOwner = GetSafeHwnd();
    ofn.lpstrFile = szFile;

    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof (szFile);
    ofn.lpstrFilter = szFilter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = GetRunPath();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    BOOL bRet = GetOpenFileName(&ofn);	

    if (bRet)
    {
		CString cstrName = szFile;
		SetDlgItemText(IDC_ET_FILENAME, cstrName);

        // ˢ��
        OnBtnRefresh();        
    }    	
}

void CMwtDlg::OnBtnModifymac() 
{
	// �����û��޸ĵ�ǰȱʡMac��ַ
    GetDlgItem(IDC_ET_MACADDR)->EnableWindow(TRUE);	
}


void CMwtDlg::OnBtnSnlist() 
{
    //������־�б�	
    CMacSNDlg dlg;
    DWORD dwCount = GetDlgItemInt( IDC_ET_MACNUM );
    if (dwCount == 0)
    {
        dwCount = 1;
    }
    dlg.SetMacPerDevice( dwCount );
    dlg.SetCurrMac( m_cMacCurr );

    dlg.DoModal();
}

void CMwtDlg::OnBtnLog() 
{
    CSNLog dlg;
    dlg.DoModal();
}


void CMwtDlg::OnBtnRefresh() 
{
    // ��ȡָ���ļ�����ȡMac��ַ	
    CString cstr;
    GetDlgItemText( IDC_ET_FILENAME, cstr );
    DWORD dwCount = 0;
    if (g_cSerializer.LoadUBootBin(cstr))
    {
        dwCount= g_cSerializer.GetMacCount();   
    }
    else
    {
        ShowErr( "ָ�����ļ�û���ҵ����ǺϷ���u-boot ROM �ļ�!" );
    }
    SetDlgItemInt( IDC_ET_MACNUM, dwCount, FALSE );	
}


void CMwtDlg::OnOK() 
{

    DWORD dwDeviceCount = GetDlgItemInt( IDC_ET_TARGETNUM, NULL, FALSE );

    ShowErr("���ڷ���ԭʼ�ļ�...", FALSE);
    // ˢ��
	OnBtnRefresh();

    if (g_cSerializer.GetMacCount() == 0)
    {
        // ���Ϸ����ļ�
        ShowErr("ԭʼ�ļ����Ϸ�...", FALSE);
        return ;
    }    
    ShowErr("ԭʼ�ļ��Ϸ�.", FALSE);
    
    if (dwDeviceCount == 0)
    {
        ShowErr("��ָ����Ҫд����豸����!");
        return;
    }

    // ��ȡ�û�ָ����MAC��ַ
    CString cstrMac;
    GetDlgItemText(IDC_ET_MACADDR, cstrMac);
    CMacAddr cTmpMac;
    if ( !cTmpMac.Str2u48( cstrMac ) )
    {
        ShowErr("�û�ָ����MAC��ַ�Ƿ�");
        return;
    }
    m_cMacCurr = cTmpMac;

    ShowErr("����ƥ�����к�...", FALSE);
    // �����־�����Ƿ���ȷ
    BOOL bIsAuto = IsDlgButtonChecked(IDC_CK_AUTO);
    if (bIsAuto)
    {
        CString cstrSNBegin;
        GetDlgItemText( IDC_ET_SN, cstrSNBegin );
        if (cstrSNBegin.IsEmpty())
        {
            ShowErr("��������ʼ���к�!", FALSE);
            return;
        }
        g_SNLogMsg.SetAutoInc( cstrSNBegin );
    }
    else
    {
        // ���ÿ��MAC�Ƿ����������к�
        g_SNLogMsg.SetNoAutoInc();
        CMacAddr cAddr = m_cMacCurr;

        DWORD dwTotalMacNum = g_cSerializer.GetMacCount() * dwDeviceCount;
        CString cstrTmp;
        SNItem item;
        for ( int i = 0; i < dwTotalMacNum; i ++)
        {
            if ( !g_SNLogMsg.GetMatchedSNbyMacInNew(cAddr, item) )
            {
                cstrTmp.Format("MAC��ַ %s ��û��ָ�����кţ�������Ϊ��������к�!", cAddr.ToString());
                ShowErr(cstrTmp);
                return;
            }

            cAddr ++;
        }
    }
    
    ShowErr("����׼��д��...", FALSE);
    m_prgWrite.SetRange32(0, dwDeviceCount);
    m_prgWrite.SetStep(1);

    // �Զ����
    g_cSerializer.SetFillFile( IsDlgButtonChecked(IDC_CK_FILL) );
    
    // �����û�����ѭ��д��
    CString cstrFilename = GetRunPath();
    CString cstrTmp;
    // �ļ�������
    BYTE byFilenameType = IsDlgButtonChecked(IDC_RD_DIFFFOLDER) ? 1 : 2;
    cstrFilename += "Output";
    CreateDirectory(cstrFilename, NULL);

    CStdioFile cFileReport;
    CString cstrFilenameReport = cstrFilename + "\\" + FILENAME_REPORT;
    cFileReport.Open(cstrFilenameReport, CFile::modeCreate | CFile::modeReadWrite);

    time_t tNow = time(NULL);
    tm when = *localtime( &tNow );
    cstrTmp.Format("����MAC��ַд������ %s\n", asctime( &when ));
    cFileReport.WriteString(cstrTmp);

    SNItem item;
    CMacAddr cAddrTmp;
    for (DWORD i = 0; i < dwDeviceCount; i++)
    {
        cstrTmp.Format("����д�� %s ...", m_cMacCurr.ToString());
        ShowErr(cstrTmp, FALSE);
        BOOL bSingle = (g_cSerializer.GetMacCount() == 1);
        if (!bSingle)
        {
            cAddrTmp = m_cMacCurr;
            cAddrTmp ++;
        }

        // �ж�2��MAC��ַ�Ƿ���ڷ�Χ��
        CString cstr, cstr1, cstr2, cstr3;
        if ( !m_cMacCurr.IsInRange( m_cMacMin, m_cMacMax ) )
        {            
            cstr1 = m_cMacCurr.ToString();
            cstr2 = m_cMacMin.ToString();
            cstr3 = m_cMacMax.ToString();
            cstr.Format("MAC ��ַ %s ��������ķ�Χ%s --- %s��!", 
                cstr1, 
                cstr2, 
                cstr3);
            ShowErr(cstr);
            break;
        }
        if ( !bSingle && !cAddrTmp.IsInRange( m_cMacMin, m_cMacMax ) )
        {
            cstr1 = cAddrTmp.ToString();
            cstr2 = m_cMacMin.ToString();
            cstr3 = m_cMacMax.ToString();
            cstr.Format("MAC ��ַ %s ��������ķ�Χ%s --- %s��!", 
                cstr1, 
                cstr2, 
                cstr3);
            ShowErr(cstr);
            break;

        }
        else             
        {
            if ( byFilenameType == 1 )
            {
                cstrFilename = GetRunPath();
                cstrTmp.Format("%.5d", i);
                cstrFilename += "Output\\";
                cstrFilename += cstrTmp;
                CreateDirectory(cstrFilename, NULL);

                cstrFilename += "\\u-boot.bin";
            }
            else
            {
                cstrFilename= GetRunPath();
                cstrFilename += "Output\\";
                cstrTmp.Format("u-boot_%.5d.bin", i);
                cstrFilename += cstrTmp;
            }

            // д��
            if (!g_cSerializer.WriteMac2Bin( cstrFilename, m_cMacCurr, 0xFF ))
            {
                CString cstr;    
                cstr.Format("д���ļ� %s ʱ��������!", cstrFilename);
                ShowErr(cstr);
                break;
            }

            // д��־����¼ʱ��            
            g_SNLogMsg.GetMatchedSNbyMacInNew(m_cMacCurr, item);

            // д˵���ļ�            
            cAddrTmp = item.atU48[0];
            cstrTmp.Format("%s<---->%s", item.cstrSN, cAddrTmp.ToString());
            if ( item.dwMACCount == 2)
            {
                cstrTmp += "\t";
                cAddrTmp = item.atU48[1];
                cstrTmp += cAddrTmp.ToString();
            }
            cstrTmp += "\r\n";
            cFileReport.WriteString(cstrTmp);

            // д��־
            item.tTime = time(NULL);
            g_SNLogMsg.AddHistoryItem(item);

            m_cMacCurr ++;
            if ( !bSingle )
            {
                m_cMacCurr ++;
            }

            m_prgWrite.StepIt();
            
        }
    }
   
    // ������־���ļ�
    g_SNLogMsg.SaveHistorySN();

    SaveDefaultMacAddr();
       
    //����Mac��ַ
    SetDlgItemText( IDC_ET_MACADDR, m_cMacCurr.ToString() );

    //��ȡ����ʾ����ļ�
    cFileReport.SeekToBegin();
    CString cstrFull;
    while (cFileReport.ReadString(cstrTmp))
    {
        cstrFull += cstrTmp;
        cstrFull += "\r\n";
    }
    cFileReport.Close();

    SetDlgItemText( IDC_ET_RESULT, cstrFull );

    ShowErr("�����ļ�д����ɡ�����Ѿ���¼����־��", FALSE);
    m_prgWrite.SetPos(0);

}

void CMwtDlg::OnCancel() 
{
	CDialog::OnCancel();
}


// �������ļ��������
void CMwtDlg::LoadDefaultMacAddr()
{
    CString cstrName = GetRunPath() ;
    cstrName += FILENAME_CFG;

    // ��ȡ��Χ
    s8 szValue[KDV_MAX_PATH];
	GetPrivateProfileString(_T("MAC"), _T("Min"), MAC_MIN,
                            szValue, KDV_MAX_PATH, cstrName);

    m_cMacMin = szValue;

	GetPrivateProfileString(_T("MAC"), _T("Max"), MAC_MAX,
                            szValue, KDV_MAX_PATH, cstrName);

    m_cMacMax = szValue;

	GetPrivateProfileString(_T("MAC"), _T("Current"), MAC_MIN,
                            szValue, KDV_MAX_PATH, cstrName);

    m_cMacCurr = szValue;    
}

// д�뵽�����ļ���Ŀǰֻд��ǰֵ
void CMwtDlg::SaveDefaultMacAddr()
{
    CString cstrName = GetRunPath() ;
    cstrName += FILENAME_CFG;

    ::WritePrivateProfileString(_T("MAC"), _T("Current"), m_cMacCurr.ToString(), cstrName); 
}


void CMwtDlg::OnDestroy() 
{
    // ���浱ǰMac��ַ
    SaveDefaultMacAddr();

    // ������־
    g_SNLogMsg.SaveHistorySN();

	CDialog::OnDestroy();
}

void CMwtDlg::OnBtnShowmac() 
{
	OnBtnRefresh();
    if ( g_cSerializer.GetMacCount() == 0)
    {
        return;
    }

    CString cstr;
    GetDlgItemText( IDC_ET_FILENAME, cstr );

    CShowMacDlg dlg;
    dlg.SetFilename( cstr );

    dlg.DoModal();	

    OnBtnRefresh();
}

void CMwtDlg::OnCkAuto() 
{
	BOOL bIsAuto = IsDlgButtonChecked(IDC_CK_AUTO);

    GetDlgItem(IDC_BTN_SNLIST)->EnableWindow(!bIsAuto);
	
}

void CMwtDlg::ShowErr(LPCTSTR lpszErr, BOOL bMsgBox)
{
    
    CWnd *pcStLable = GetDlgItem(IDC_ST_PROGRESS);
    if (lpszErr == NULL)
    {
        pcStLable->SetWindowText("");
        return;
    }

    pcStLable->SetWindowText(lpszErr);
    if (bMsgBox)
    {
        AfxMessageBox( lpszErr );
    }    
}

void CMwtDlg::OnBtnAddsn() 
{
    DWORD dwMacNum = GetDlgItemInt(IDC_ET_MACNUM, NULL, FALSE);
    if (dwMacNum == 0)
    {
        AfxMessageBox("����ָ���Ϸ���BIN�ļ�!");
        return;
    }

    SNItem item;

    if (!ValidateInput(item))
    {
        return;
    }

    if ( g_SNLogMsg.IsSNExistInHistory(item) )
    {
        AfxMessageBox("�����к�����ʷ��¼���Ѿ���ʹ��!");
        GetDlgItem(IDC_ET_SN)->SetFocus();
        return ;
    }

    if ( g_SNLogMsg.IsSNExistInNew(item) )
    {
        AfxMessageBox("�����кű��������!");
        GetDlgItem(IDC_ET_SN)->SetFocus();
        return ;
    }

    g_SNLogMsg.AddNewItem(item);

    LoadMacSN();    

    // �Զ�����
    CMacAddr cAddr;
    if ( dwMacNum == 1 )
    {
        cAddr = item.atU48[0];
    }
    else
    {
        cAddr = item.atU48[1];
    }
    cAddr++;    

    SetDlgItemText( IDC_ET_MAC4SN, cAddr.ToString() );

    GetDlgItem(IDC_ET_SN)->SetFocus();	
}

void CMwtDlg::OnBtnDelsn() 
{
    POSITION pos = m_lsMacSn.GetFirstSelectedItemPosition();

    if (pos == NULL)
    {
        AfxMessageBox("����ѡ����Ҫɾ������Ŀ!");
        return;
    }

    int nItem = m_lsMacSn.GetNextSelectedItem(pos);
    DWORD dwIndex = m_lsMacSn.GetItemData( nItem );

    if (dwIndex >= g_SNLogMsg.m_vtAddedSNItem.size())
    {
        return;
    }	

    g_SNLogMsg.m_vtAddedSNItem.erase( &g_SNLogMsg.m_vtAddedSNItem[dwIndex] );

    LoadMacSN();

    GetDlgItem(IDC_ET_SN)->SetFocus();
}

// �����ݽṹ���뵽����
void CMwtDlg::LoadMacSN()
{
    vector<SNItem>::const_iterator citr = g_SNLogMsg.m_vtAddedSNItem.begin();
    vector<SNItem>::const_iterator citrEnd = g_SNLogMsg.m_vtAddedSNItem.end();

    m_lsMacSn.DeleteAllItems();

    int i = 0;
    CString cstr;
    CMacAddr cAddr;
    while ( citr != citrEnd )
    {
        int nItem = m_lsMacSn.InsertItem(m_lsMacSn.GetItemCount(), citr->cstrSN );
        m_lsMacSn.SetItemData( nItem, i );
        
        cstr.Format("%u", citr->dwMACCount);            
        m_lsMacSn.SetItemText( nItem, 1, cstr );
        cAddr = citr->atU48[0];
        m_lsMacSn.SetItemText( nItem, 2, cAddr.ToString() );
        if ( citr->dwMACCount == 2)
        {
            cAddr = citr->atU48[1];
            m_lsMacSn.SetItemText( nItem, 3, cAddr.ToString() );
        }

        i++;
        citr++;
    }
}

// ��֤�����û�����
BOOL CMwtDlg::ValidateInput(SNItem &item)
{
    CString cstrSN;
    GetDlgItemText( IDC_ET_SN, cstrSN);

    CString cstr;
    CMacAddr cAddr1, cAddr2;
    GetDlgItemText(IDC_ET_MAC4SN, cstr);
    BOOL bValid1 = cAddr1.Str2u48(cstr);

    DWORD dwMacNum = GetDlgItemInt(IDC_ET_MACNUM, NULL, FALSE); 
    // ����Mac
    BOOL bValid2 = TRUE;
    if ( dwMacNum == 2 )
    {
        cAddr2 = cAddr1;
        cAddr2 ++;
        bValid2 = TRUE;        
    }

    if ( cstrSN.IsEmpty() )
    {
        AfxMessageBox("���кŲ���Ϊ��");
        GetDlgItem(IDC_ET_SN)->SetFocus();
        return FALSE;
    }
	
    if (!bValid1)
    {
        CString cstr2;
        GetDlgItemText(IDC_ET_MAC1, cstr);
        cstr2.Format("MAC ��ַ %s �Ƿ�!", cstr);
        AfxMessageBox(cstr2);
        GetDlgItem(IDC_ET_MAC1)->SetFocus();
        return FALSE;
    }
/*
    if (!bValid2)
    {
        CString cstr2;
        GetDlgItemText(IDC_ET_MAC2, cstr);
        cstr2.Format("MAC ��ַ %s �Ƿ�!", cstr);
        AfxMessageBox(cstr2);
        GetDlgItem(IDC_ET_MAC2)->SetFocus();
        return FALSE;
    }
*/
    
    item.cstrSN = cstrSN;
    item.dwMACCount = dwMacNum;
    item.atU48[0] = cAddr1.GetU48();
    if ( dwMacNum == 2 )
    {
        item.atU48[1] = cAddr2.GetU48();
    }
    return TRUE;
}
