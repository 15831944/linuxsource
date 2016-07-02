// SNLog.cpp : implementation file
//

#include "stdafx.h"
#include "mwt.h"
#include "SNLog.h"
#include "SNLogMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSNLog dialog


CSNLog::CSNLog(CWnd* pParent /*=NULL*/)
	: CDialog(CSNLog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSNLog)
	m_tStart = 0;
	m_tEnd = 0;
	//}}AFX_DATA_INIT
}


void CSNLog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSNLog)
	DDX_Control(pDX, IDC_CB_SEARCHTYPE, m_cbSearchType);
	DDX_Control(pDX, IDC_LS_MACSN, m_lsMacSn);
	DDX_DateTimeCtrl(pDX, IDC_DP1, m_tStart);
	DDX_DateTimeCtrl(pDX, IDC_DP2, m_tEnd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSNLog, CDialog)
	//{{AFX_MSG_MAP(CSNLog)
	ON_CBN_SELCHANGE(IDC_CB_SEARCHTYPE, OnSelchangeCbSearchtype)
	ON_BN_CLICKED(IDC_BTN_SEARCH, OnBtnSearch)
	ON_BN_CLICKED(IDC_BTN_SEARCH2, OnBtnSearch2)
	ON_BN_CLICKED(IDC_BTN_EXPORT, OnBtnExport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSNLog message handlers

BOOL CSNLog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    // ��ʼ���б�
    m_lsMacSn.InsertColumn( 0,  "ʱ��",    LVCFMT_CENTER, 160 );
    m_lsMacSn.InsertColumn( 1,  "���к�",  LVCFMT_CENTER, 160 );
    m_lsMacSn.InsertColumn( 2, "��ַ����", LVCFMT_CENTER, 70);
    m_lsMacSn.InsertColumn( 3, "MAC��ַ0", LVCFMT_CENTER, 140 );
    m_lsMacSn.InsertColumn( 4, "MAC��ַ1", LVCFMT_CENTER, 140);
    
	m_lsMacSn.SetExtendedStyle(m_lsMacSn.GetExtendedStyle()
					   		   | LVS_EX_FULLROWSELECT );

    LoadHistorySN();

    m_cbSearchType.AddString( "ȫ��" );
    m_cbSearchType.AddString( "ָ�����к�" );
    m_cbSearchType.AddString( "ָ��MAC��ַ" );
    m_cbSearchType.AddString( "ָ������" );
    
    m_cbSearchType.SetCurSel(0);
    OnSelchangeCbSearchtype();

    CTime t = CTime::GetCurrentTime();

    m_tStart = t;
    m_tEnd = t;
    UpdateData(FALSE);
    
    
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSNLog::LoadHistorySN() 
{
    vector<SNItem>::const_iterator citr = g_SNLogMsg.m_vtHistorySNItem.begin();
    vector<SNItem>::const_iterator citrEnd = g_SNLogMsg.m_vtHistorySNItem.end();

    m_lsMacSn.DeleteAllItems();

    int i = 0;
    CString cstr;
    CMacAddr cAddr;
    struct tm when;
    while ( citr != citrEnd )
    {
        when = *localtime( &citr->tTime );                
        
        int nItem = m_lsMacSn.InsertItem(m_lsMacSn.GetItemCount(), asctime( &when ) );
        m_lsMacSn.SetItemData( nItem, i );
        
        m_lsMacSn.SetItemText( nItem, 1, citr->cstrSN );

        cstr.Format("%u", citr->dwMACCount);            
        m_lsMacSn.SetItemText( nItem, 2, cstr );
        cAddr = citr->atU48[0];
        m_lsMacSn.SetItemText( nItem, 3, cAddr.ToString() );
        if ( citr->dwMACCount == 2)
        {
            cAddr = citr->atU48[1];
            m_lsMacSn.SetItemText( nItem, 4, cAddr.ToString() );
        }

        i++;
        citr++;
    }
}

void CSNLog::OnSelchangeCbSearchtype() 
{
	switch (m_cbSearchType.GetCurSel())
    {
    case 0:     //ȫ��
        GetDlgItem(IDC_ET_INPUT)->EnableWindow(FALSE);
        GetDlgItem(IDC_BTN_SEARCH2)->EnableWindow(FALSE);
        GetDlgItem(IDC_BTN_SEARCH)->EnableWindow(FALSE);
        GetDlgItem(IDC_DP1)->EnableWindow(FALSE);
        GetDlgItem(IDC_DP2)->EnableWindow(FALSE);
        LoadHistorySN();
        break;
    case 1:     //ָ��SN
    case 2:     //ָ��MAC    
        GetDlgItem(IDC_ET_INPUT)->EnableWindow(TRUE);
        GetDlgItem(IDC_BTN_SEARCH2)->EnableWindow(TRUE);
        GetDlgItem(IDC_BTN_SEARCH)->EnableWindow(FALSE);
        GetDlgItem(IDC_DP1)->EnableWindow(FALSE);
        GetDlgItem(IDC_DP2)->EnableWindow(FALSE);

        GetDlgItem(IDC_ET_INPUT)->SetFocus();

        break;
    case 3:     //ָ��Date
        GetDlgItem(IDC_ET_INPUT)->EnableWindow(FALSE);
        GetDlgItem(IDC_BTN_SEARCH2)->EnableWindow(FALSE);
        GetDlgItem(IDC_BTN_SEARCH)->EnableWindow(TRUE);
        GetDlgItem(IDC_DP1)->EnableWindow(TRUE);
        GetDlgItem(IDC_DP2)->EnableWindow(TRUE);
        break;
    default:
        break;  

    }	
}

void CSNLog::OnBtnSearch() 
{
	// ����ָ��ֵ����
    vector<SNItem>::const_iterator citr = g_SNLogMsg.m_vtHistorySNItem.begin();
    vector<SNItem>::const_iterator citrEnd = g_SNLogMsg.m_vtHistorySNItem.end();

    m_lsMacSn.DeleteAllItems();

        
    DWORD dwType = m_cbSearchType.GetCurSel();
    CString cstrSearch;  
    if ( dwType == 3)
    {
        // ����, ��ȡ
        UpdateData(TRUE);
        CTime tNewStartTime(m_tStart.GetYear(), m_tStart.GetMonth(), m_tStart.GetDay(), 0,0,0);
        m_tStart = tNewStartTime;
        CTime tNewEndTime(m_tEnd.GetYear(), m_tEnd.GetMonth(), m_tEnd.GetDay(), 23,59,59);
        m_tEnd = tNewEndTime;
    }
    else
    {
        // ����
        GetDlgItemText( IDC_ET_INPUT, cstrSearch );
    }

    int i = 0;
    CString cstr;
    CMacAddr cAddr = cstrSearch;
    BOOL bInsert;
    struct tm when;
    
    while ( citr != citrEnd )
    {   
        bInsert = FALSE;
        switch (dwType)
        {
        case 1:
            // ���к�
            if ( citr->cstrSN == cstrSearch )
                bInsert = TRUE;
            break;
        case 2:
            // MAC
            if ( cAddr == citr->atU48[0]  || 
                 (citr->dwMACCount == 2 && cAddr == citr->atU48[1] ) )
                bInsert = TRUE;
            break;
        case 3:
            // ����
            if ( citr->tTime >= m_tStart.GetTime() && citr->tTime <= m_tEnd.GetTime() )
                bInsert = TRUE;
            break;
        }

        if (bInsert)
        {
            when = *localtime( &citr->tTime );    
            
            int nItem = m_lsMacSn.InsertItem(m_lsMacSn.GetItemCount(), asctime( &when ) );
            m_lsMacSn.SetItemData( nItem, i );
        
            m_lsMacSn.SetItemText( nItem, 1, citr->cstrSN );

            cstr.Format("%u", citr->dwMACCount);            
            m_lsMacSn.SetItemText( nItem, 2, cstr );
            cAddr = citr->atU48[0];
            m_lsMacSn.SetItemText( nItem, 3, cAddr.ToString() );
            if ( citr->dwMACCount == 2)
            {
                cAddr = citr->atU48[1];
                m_lsMacSn.SetItemText( nItem, 4, cAddr.ToString() );
            }
        }

        i++;
        citr++;
    }
	
}

void CSNLog::OnBtnSearch2() 
{
	OnBtnSearch();	
}

const s8* GetRunPath();
void CSNLog::OnBtnExport() 
{
	// �������ҽ��
	int nCount = m_lsMacSn.GetItemCount();
    if ( nCount <=0 )
    {
        AfxMessageBox("û���κε�����¼!");
        return;
    }    

    // ����ļ�
    OPENFILENAME ofn; 
    //�趨�ļ������ƴ���ռ� 
    TCHAR lpstrFilename[KDV_MAX_PATH] = _T(""); 

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);  //lStructSize�ķ���ֵ����76 
    ofn.hwndOwner = this->m_hWnd;
    ofn.lpstrFile = lpstrFilename;
    ofn.nMaxFile = KDV_MAX_PATH;
    ofn.lpstrFilter = _T("��־�ļ�(*.Log)\0*.Log\0All Files(*.*)\0*.*\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = GetRunPath();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_NOREADONLYRETURN | OFN_OVERWRITEPROMPT;

    BOOL bRet = GetSaveFileName(&ofn);
    if (!bRet)
    {
        return ;
    }
    
    CString cstrNewLogName = lpstrFilename;
    if (cstrNewLogName.ReverseFind('.') < cstrNewLogName.GetLength() - 5)
    {
        cstrNewLogName += _T(".log");
    }
    

    vector<SNItem> vtItems;
    DWORD dwIndex;
    for (int i = 0; i < nCount ; i ++)
    {
        dwIndex = m_lsMacSn.GetItemData( i );
 
        vtItems.push_back( g_SNLogMsg.m_vtHistorySNItem[dwIndex] );
    }


    // д�ļ�
    CStdioFile file;
    file.Open( cstrNewLogName, CFile::modeCreate | CFile::modeWrite );
    CString cstr, cstr2;
    CMacAddr addr;
    CTime t;
    for ( i = 0; i < nCount ; i ++)
    {
        addr = vtItems[i].atU48[0];
        cstr.Format("%s<---->%s\t", vtItems[i].cstrSN, addr.ToString());
        if ( vtItems[i].dwMACCount == 2 )
        {
            addr = vtItems[i].atU48[1];
            cstr += addr.ToString();
        }
        cstr += "\r\n";
            
        file.WriteString( cstr );
    }

    file.Close();
    AfxMessageBox( "�������!" );

    return;
}
