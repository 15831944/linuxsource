#ifndef TEMPLATECLASS_H
#define TEMPLATECLASS_H
/*---------------------------------------------------------------------
* ��	����Singleton
* ��    �ܣ�������ͳһ�ӿ�
* ����˵����
* �޸ļ�¼��
* ����			�汾		�޸���		�޸ļ�¼
* 2011/05/09	v1.0		Ĳ��ï		����
----------------------------------------------------------------------*/
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#pragma warning( disable : 4800 )
#include <assert.h>
#include <string>
#include <map>
namespace AutoUIFactory
{

template<class T> class Singleton
{
protected:
	static T* ms_pSingleton;
public:
	Singleton( void )
	{
		assert( !ms_pSingleton );
		ms_pSingleton = static_cast<T*>(this);
	}
	~Singleton( void )
	{
		assert( ms_pSingleton );
		ms_pSingleton = 0;
	}
	static T& GetSingleton( void )
	{
		assert( ms_pSingleton );
		return ( *ms_pSingleton );
	}
	static T* GetSingletonPtr( void )
	{
		return ms_pSingleton;
	}
};

class IArgs
{
public:
	IArgs( std::string strType ) : m_strType( strType ){}
	virtual ~IArgs(){}
	std::string GetType() const { return m_strType; } 
	virtual void Print(){}
protected:
	std::string m_strType;
};

class IObserver
{
public:
	virtual ~IObserver(){}
	virtual void Update( IArgs* pArgs ) = 0;
};

/*---------------------------------------------------------------------
* ��	����ICallBackFuncBase
* ��    �ܣ��ص������ӿ�
* ����˵����
* �޸ļ�¼��
* ����			�汾		�޸���		�޸ļ�¼
* 2010/08/20	v1.0		Ĳ��ï		����
----------------------------------------------------------------------*/
class ICallBackFuncBase_Args
{
public:
	virtual ~ICallBackFuncBase_Args(){}
	virtual bool operator()( const IArgs* args ) = 0;
};

/*---------------------------------------------------------------------
* ��	����CMemberCallBackFunc
* ��    �ܣ�Ϊ��Ա��������Ļص�����
* ����˵����
* �޸ļ�¼��
* ����			�汾		�޸���		�޸ļ�¼
* 2010/08/20	v1.0		Ĳ��ï		����
----------------------------------------------------------------------*/
template<class T> class CMemberCallBackFunc_Args : public ICallBackFuncBase_Args
{
public:
    //����ص������ĸ�ʽ
    typedef bool (T::*MemberFunctionType)( const IArgs* args );
	
	// �����ʱ����ҪΪ������ֵ
    CMemberCallBackFunc_Args( MemberFunctionType pFunc, T* pObject ) : m_pObject(pObject), m_pFuc( pFunc ) {}
	
    virtual bool operator()( const IArgs* args )
    {
        return (m_pObject->*m_pFuc)( args );
    }
	
private:
	T* m_pObject;
    MemberFunctionType m_pFuc;	
};

/*---------------------------------------------------------------------
* ��	����CMemberCallBackFunc
* ��    �ܣ�Ϊ��Ա��������Ļص�����
* ����˵����
* �޸ļ�¼��
* ����			�汾		�޸���		�޸ļ�¼
* 2010/08/20	v1.0		Ĳ��ï		����
----------------------------------------------------------------------*/
class CCallBackFunc_Args : public ICallBackFuncBase_Args
{
public:
	virtual bool operator()( const IArgs* args)
	{
		return m_pFuc(args);
	}
	
    //����ص������ĸ�ʽ
    typedef bool (CallBackFunc)( const IArgs* args);
	
	// �����ʱ����ҪΪ������ֵ
    CCallBackFunc_Args(CallBackFunc* pFunc) : m_pFuc( pFunc ) {}
	
private:
    CallBackFunc* m_pFuc;	
};

/*---------------------------------------------------------------------
* ��	����INoncopyable
* ��    �ܣ����ڲ��ɿ�����Ļ��࣬�ȷ�˵һЩȫ�ֵ���Դ
* ����˵�������Ҫʹ�ã�����private�̳�
* �޸ļ�¼��
* ����			�汾		�޸���		�޸ļ�¼
* 2011/07/19	v1.0		Ĳ��ï		����
----------------------------------------------------------------------*/
class INoncopyable
{
protected:
	INoncopyable() {}
	~INoncopyable() {}
private:  // emphasize the following members are private
	   INoncopyable( const INoncopyable& );
	   const INoncopyable& operator=( const INoncopyable& );
};

} // namespace AutoUIFactory end
#endif