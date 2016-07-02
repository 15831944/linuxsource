// QuitRecDlg.cpp : implementation file
//

#include "stdafx.h"
#include "recorder.h"
#include "QuitRecDlg.h"
//#include "ConfAgent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CQuitRecDlg dialog


CQuitRecDlg::CQuitRecDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQuitRecDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CQuitRecDlg)
    m_achNotfyStr = _T("");
	//}}AFX_DATA_INIT

    m_bEnglishLang = FALSE;
}

void CQuitRecDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuitRecDlg)
	DDX_Text(pDX, IDC_STATIC_NOTIFY, m_achNotfyStr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQuitRecDlg, CDialog)
	//{{AFX_MSG_MAP(CQuitRecDlg)
	//}}AFX_MSG_MAP
    ON_MESSAGE( WM_LANGTOCHANGE, OnChangeLanguage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuitRecDlg message handlers


/*=============================================================================
  �� �� ���� OnChangeLanguage
  ��    �ܣ� ��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� WPARAM wParam
             LPARAM lParam
  �� �� ֵ�� void 
=============================================================================*/
void CQuitRecDlg::OnChangeLanguage(WPARAM wParam, LPARAM lParam)
{
    if( TRUE == wParam )
    {
        m_achNotfyStr = _T(" Quit Recorder Server?");
    }
    else
    {
        m_achNotfyStr = _T(" �˳�¼���?");
    }
    AfxGetMainWnd()->UpdateData(FALSE);
    return;
}


/*=============================================================================
�� �� ���� OnInitDialog
��    �ܣ�  
��    ���� 
�� �� ֵ�� BOOL 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/1/23  4.0			������                  ����
=============================================================================*/
BOOL CQuitRecDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    if( m_bEnglishLang ) 
    {
        m_achNotfyStr = _T("Quit Recorder Server?");
        GetDlgItem(IDOK)->SetWindowText("Ok");
        GetDlgItem(IDCANCEL)->SetWindowText("Cancel");
        SetWindowText("Quit");
    }
    else
    {
        m_achNotfyStr = _T("�˳�¼���?");
        GetDlgItem(IDOK)->SetWindowText("ȷ��");
        GetDlgItem(IDCANCEL)->SetWindowText("ȡ��");
        SetWindowText("�˳�");
    }
    
    UpdateData(FALSE);

    return TRUE;
}
