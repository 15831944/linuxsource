// Sink.cpp: implementation of the CSink class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sample.h"
#include "Sink.h"
#include "recinterface.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSink::CSink()
{

}

CSink::~CSink()
{

}

STDMETHODIMP CSink::QueryInterface(const struct _GUID &iid,void ** ppv)
{
    *ppv=this;
    return S_OK;
}

ULONG __stdcall CSink::AddRef(void)
{	return 1;	}	// �����ٵľͿ��ԣ���Ϊ������������ڳ������ǰ�ǲ����˳���

ULONG __stdcall CSink::Release(void)
{	return 0;	}	// �����ٵľͿ��ԣ���Ϊ������������ڳ������ǰ�ǲ����˳���

STDMETHODIMP CSink::GetTypeInfoCount(unsigned int *)
{	return E_NOTIMPL;	}	// ����ʵ�֣�����Ҳ����

STDMETHODIMP CSink::GetTypeInfo(unsigned int,unsigned long,struct ITypeInfo ** )
{	return E_NOTIMPL;	}	// ����ʵ�֣�����Ҳ����

STDMETHODIMP CSink::GetIDsOfNames(const struct _GUID &,unsigned short ** ,unsigned int,unsigned long,long *)
{	return E_NOTIMPL;	}	// ����ʵ�֣�����Ҳ����

STDMETHODIMP CSink::Invoke(
                           long dispID,
                           const struct _GUID &,
                           unsigned long,
                           unsigned short,
                           struct tagDISPPARAMS * pParams,
                           struct tagVARIANT *,
                           struct tagEXCEPINFO *,
                           unsigned int *)
{		
    // ֻ��Ҫʵ��������㹻��
    switch(dispID)	// ���ݲ�ͬ��dispID,��ɲ�ͬ�Ļص�����
    {
    case 1:
        {
            TRecConfStatus *ptStatus = (TRecConfStatus *)pParams->rgvarg[0].lVal;
            
            DWORD dwConfIdx = ptStatus->GetIndex();
            u32   dwState  = ptStatus->GetRecState();
            ::PostMessage(g_hMain, WM_CONFUPDATE, dwState, dwConfIdx);

            break;
        }
    case 2:
        
            break;
    default:
        break;
    }
    return S_OK;
}