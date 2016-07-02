// AddUserDlg.cpp : implementation file
//

#include "stdafx.h"
#include "kdvmediavodapp.h"
#include "kdvmediavod.h"
#include "AddUserDlg.h"
#include "kdvmediavodinstance.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
#endif

extern int g_nLanguageType;
/////////////////////////////////////////////////////////////////////////////
// CAddUserDlg dialog


CAddUserDlg::CAddUserDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddUserDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddUserDlg)
	m_csUserName = _T("");
	m_csPassWord = _T("");
	//}}AFX_DATA_INIT
}


void CAddUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddUserDlg)
	DDX_Text(pDX, IDC_USERNAME_EDIT, m_csUserName);
	DDX_Text(pDX, IDC_PASSWORD_EDIT, m_csPassWord);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddUserDlg, CDialog)
	//{{AFX_MSG_MAP(CAddUserDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddUserDlg message handlers

BOOL CAddUserDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
    if ( g_nLanguageType == LANGUAGE_CHINESE )
    {
        GetDlgItem(IDC_USERNAME_STATIC)->SetWindowText("�û���");
        GetDlgItem(IDC_PASSWORD_STATIC)->SetWindowText("����");
        GetDlgItem(IDOK)->SetWindowText("ȷ��");
        GetDlgItem(IDCANCEL)->SetWindowText("ȡ��");
    }
    else
    {
        GetDlgItem(IDC_USERNAME_STATIC)->SetWindowText("Username");
        GetDlgItem(IDC_PASSWORD_STATIC)->SetWindowText("Password");
        GetDlgItem(IDOK)->SetWindowText("Ok");
        GetDlgItem(IDCANCEL)->SetWindowText("Cancel");
    }

    GetDlgItem(IDC_USERNAME_EDIT)->SetFocus();

	/* 2011-06-07 add by peng guofeng : �ն˵�vod�㲥��mediavod��������½�������Ϊ0-9֮�������*/	
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_PASSWORD_EDIT);
	if( NULL != pEdit )
	{
		pEdit->ModifyStyle(0, ES_NUMBER);		
	}
	/* 2011-06-07 add end */
    
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddUserDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData( TRUE );

    if ( m_csUserName.GetLength() == 0 || 
         m_csPassWord.GetLength() == 0 )
    {
        if ( g_nLanguageType == LANGUAGE_CHINESE )
        {
            AfxMessageBox( "�û��������벻��Ϊ��!" );
        }
        else
        {
            AfxMessageBox( "Username or password should not be null!" );
        }

        return;
    }

	CDialog::OnOK();
}
