// singleton.h: interface for the CSingleTon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLETON_H__AEF4D8BA_C3E8_44C1_874D_854F10BE573A__INCLUDED_)
#define AFX_SINGLETON_H__AEF4D8BA_C3E8_44C1_874D_854F10BE573A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*---------------------------------------------------------------------
* ��	����Singleton
* ��    �ܣ��ṩͳһ������ӿ�
* ����˵����1. ������           class CData : public CSingleton<CData>
			2. ��������(��CData��cpp�ļ���) 
			template<> CData* CSingleton<CData>::m_psSingleton  = NULL;
			
			3. �ڳ����ʼ�ĳ�ʼ������г�ʼ��
			new CData();

			4. ����ʼ�����󣬾Ϳ����ڳ��������ط�ʹ��
			
			CData* pData = CSingleton<CData>::GetSingleTonPtr();

			����û�ж�CData�Ĺ��캯���������ƣ������м���3��ʱ��ȷ��CDataֻ��ʼ��һ��

* �޸ļ�¼��
* ����			�汾		�޸���		�޸ļ�¼
* 2010/07/16	v1.0		Ĳ��ï		����
----------------------------------------------------------------------*/
template<typename T> class CSingleton
{
protected:
	static T* m_psSingleton;
	
public:
	CSingleton()
	{
		m_psSingleton = static_cast<T*>(this);
	}
	
	virtual ~CSingleton()
	{
		m_psSingleton = NULL;
	}
	
	// ��ȡָ��Ͷ���
	static T& GetSingleton(){ return *m_psSingleton; }
	
	static T* GetSingletonPtr(){ return m_psSingleton; }
};

#endif // !defined(AFX_SINGLETON_H__AEF4D8BA_C3E8_44C1_874D_854F10BE573A__INCLUDED_)
