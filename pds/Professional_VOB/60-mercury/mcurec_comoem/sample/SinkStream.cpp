// SinkStream.cpp: implementation of the CSinkStream class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sample.h"
#include "SinkStream.h"
#include "recinterface.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSinkStream::CSinkStream()
{

}

CSinkStream::~CSinkStream()
{

}

STDMETHODIMP CSinkStream::QueryInterface(const struct _GUID &iid,void ** ppv)
{
    *ppv=this;
    return S_OK;
}

ULONG __stdcall CSinkStream::AddRef(void)
{	return 1;	}	// �����ٵľͿ��ԣ���Ϊ������������ڳ������ǰ�ǲ����˳���

ULONG __stdcall CSinkStream::Release(void)
{	return 0;	}	// �����ٵľͿ��ԣ���Ϊ������������ڳ������ǰ�ǲ����˳���

STDMETHODIMP CSinkStream::GetTypeInfoCount(unsigned int *)
{	return E_NOTIMPL;	}	// ����ʵ�֣�����Ҳ����

STDMETHODIMP CSinkStream::GetTypeInfo(unsigned int,unsigned long,struct ITypeInfo ** )
{	return E_NOTIMPL;	}	// ����ʵ�֣�����Ҳ����

STDMETHODIMP CSinkStream::GetIDsOfNames(const struct _GUID &,unsigned short ** ,unsigned int,unsigned long,long *)
{	return E_NOTIMPL;	}	// ����ʵ�֣�����Ҳ����

STDMETHODIMP CSinkStream::Invoke(
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
            TStreamProperty *ptStream = (TStreamProperty*)pParams->rgvarg[0].lVal;
            /*
            CString cstr;
            cstr.Format("�յ�����: (%u, %u, %u)�ص���\n", 
                        ptStream->GetConfIndex(),
                        ptStream->GetStreamIndex(),
                        ptStream->GetFrameHead()->GetTStamp());

            printf(cstr);
            //AfxMessageBox(cstr);
            */
            ::SendMessage(g_hMain, WM_STREAMUPDATE, (WPARAM)ptStream, NULL);
            
            break;
        }
    case 2:
        
        break;
    default:
        break;
    }
    return S_OK;
}