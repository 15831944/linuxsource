// nmsinstance.h: interface for the CNmsInstance class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NMSINSTANCE_H__20545268_BF94_4A22_869A_6C419B8FFAC3__INCLUDED_)
#define AFX_NMSINSTANCE_H__20545268_BF94_4A22_869A_6C419B8FFAC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4244)  // conversion from 'unsigned long' to 'unsigned char', possible loss of data
#pragma warning(disable:4018)  // signed/unsigned mismatch
#pragma warning(disable:4786)  // identifier over 255

#include <map>
#include <vector>
using namespace std;
#include "kdvtype.h"
#include "kdvdef.h"
#include "osp.h"
#include "kdvsys.h"


#pragma comment(lib,"osplib.lib")

/*---------------------------------------------------------------------
* ��	����CObserver
* ��    �ܣ��۲��߳���ӿڣ������Ҫ����Instance��������Ϣ����Ҫ����ע��
* ����˵����
* �޸ļ�¼��
* ����			�汾		�޸���		�޸ļ�¼
* 2010/08/11	v1.0		Ĳ��ï		����
----------------------------------------------------------------------*/
class CInstanceAssist;
class CNmsInstance;
class CObserver
{
public:
	virtual ~CObserver(){}
	virtual void Report( CNmsInstance* pInstance, CInstanceAssist* pInstanceAssist, CMessage *const pMsg, void* pUserData = NULL ) = 0;
};

class CWndObserver : public CObserver
{
public:
	CWndObserver( HWND hWnd ) : m_hWnd( hWnd ){}
	virtual void Report( CNmsInstance* pInstance, CInstanceAssist* pInstanceAssist, CMessage *const pMsg, void* pUserData = NULL ){}
protected:
	HWND m_hWnd;
};

/*---------------------------------------------------------------------
* ��	����ICallBackFuncBase
* ��    �ܣ��ص������ӿ�
* ����˵����
* �޸ļ�¼��
* ����			�汾		�޸���		�޸ļ�¼
* 2010/08/20	v1.0		Ĳ��ï		����
----------------------------------------------------------------------*/
class ICallBackFuncBase
{
public:
	virtual ~ICallBackFuncBase(){}
	virtual BOOL32 operator()( CNmsInstance* pInstance, CInstanceAssist*, CMessage *const pMsg, void* pUserData = NULL ) = 0;
};

/*---------------------------------------------------------------------
* ��	����CStaticCallBackFunc
* ��    �ܣ���̬������һ�ֻص�����
* ����˵����
* �޸ļ�¼��
* ����			�汾		�޸���		�޸ļ�¼
* 2010/08/20	v1.0		Ĳ��ï		����
----------------------------------------------------------------------*/
class CStaticCallBackFunc : public ICallBackFuncBase
{
public:
	virtual BOOL32 operator()( CNmsInstance* pInstance, CInstanceAssist* pInstanceAssist, CMessage *const pMsg, void* pUserData = NULL )
	{
		return m_pFuc( pInstance, pInstanceAssist, pMsg, pUserData );
	}
	
    //����ص������ĸ�ʽ
    typedef BOOL32 (CallBackFunc)( CNmsInstance* pInstance, CInstanceAssist*, CMessage *const, void* );
	
	// �����ʱ����ҪΪ������ֵ
    CStaticCallBackFunc(CallBackFunc* pFunc) :	m_pFuc( pFunc ) {}
	
private:
    CallBackFunc* m_pFuc;	
};

/*---------------------------------------------------------------------
* ��	����CMemberCallBackFunc
* ��    �ܣ�Ϊ��Ա��������Ļص�����
* ����˵����
* �޸ļ�¼��
* ����			�汾		�޸���		�޸ļ�¼
* 2010/08/20	v1.0		Ĳ��ï		����
----------------------------------------------------------------------*/
template<class T> class CMemberCallBackFunc : public ICallBackFuncBase
{
public:
    //����ص������ĸ�ʽ
    typedef BOOL32 (T::*MemberFunctionType)( CNmsInstance* pInstance, CInstanceAssist*, CMessage *const, void* );
	
	// �����ʱ����ҪΪ������ֵ
    CMemberCallBackFunc( MemberFunctionType pFunc, T* pObject ) : m_pObject(pObject), m_pFuc( pFunc ) {}
	
    virtual BOOL32 operator()( CNmsInstance* pInstance, CInstanceAssist* pInstanceAssist, CMessage *const pMsg, void* pUserData = NULL )
    {
        return (m_pObject->*m_pFuc)( pInstance, pInstanceAssist, pMsg, pUserData );
    }
	
private:
	T* m_pObject;
    MemberFunctionType m_pFuc;	
};

/*---------------------------------------------------------------------
* ��	����CInstanceAssist
* ��    �ܣ������Ĵ���Ԫ/�����߼��Ĵ����Լ�֪ͨUI
* ����˵����
* �޸ļ�¼��
* ����			�汾		�޸���		�޸ļ�¼
* 2010/08/19	v1.0		Ĳ��ï		����
----------------------------------------------------------------------*/
class CInstanceAssist
{
public:
	// ��Ϣ����
	virtual void TransData( CNmsInstance* pInstance, CMessage *const pMsg ) = 0;
	// ������ΪNmsInstace��������Ԫ�ṩ�ӿ�
	virtual CInstanceAssist* Clone() = 0;

	// ������Ϣ���Զ�
	virtual BOOL32 PostMsg( u16 wEvent, void* pBody = NULL, u32 dwBodyLen = 0 );

	// ��msg����һЩ������
	BOOL32 CheckMsg( CMessage *const pMsg );

	virtual void SetInstanceParam(u32 dwDstAppId, u32 dwDstNode, u32 dwSrcAppId, u32 dwSrcNode );

	virtual ~CInstanceAssist(){}
	
public:
	BOOL32 m_bConnected;
	u32 m_dwDstAppId;
	u32 m_dwDstNode;
	u32 m_dwSrcAppId;
	u32 m_dwSrcNode;
	u32 m_dwTimerState;
};

/*---------------------------------------------------------------------
* ��	����CNmsInstance
* ��    �ܣ����ʵ����������Ϣ
* ����˵����
* �޸ļ�¼��
* ����			�汾		�޸���		�޸ļ�¼
* 2010/08/11	v1.0		Ĳ��ï		����
----------------------------------------------------------------------*/
class CNmsInstance : public CInstance
{
public:
	CNmsInstance(){ m_pAssist = NULL; }
	//�ػ�ʵ����ں���
	virtual void DaemonInstanceEntry( CMessage *const, CApp* );
	
	//ʵ����ں���
	virtual void InstanceEntry( CMessage *const pMsg );

	virtual BOOL32 CanConnect( CMessage *const pMsg );

	virtual BOOL32 StartTimer( u32 nTimer, long nMilliSeconds, u32 nPara = 0 );
	virtual BOOL32 StopTimer( u32 nTimer );
public:
	// ����Instance������
	static BOOL32 SetInstanceAssistType( CInstanceAssist* pType );
	// ���ڴ���Assist����
	static CInstanceAssist* m_pBaseIntanceAssist;	
	// ͨ��m_pBaseIntanceAssist����Assist
	BOOL32 CreateInstanceAssist();

	CInstanceAssist* m_pAssist;
};
#endif // !defined(AFX_NMSINSTANCE_H__20545268_BF94_4A22_869A_6C419B8FFAC3__INCLUDED_)
