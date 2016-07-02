#ifndef _MESSAGESTRUCT_H_
#define _MESSAGESTRUCT_H_
#include "umconst.h"
#include "osp.h"
#include "md5.h"
#ifdef WIN32
    #pragma pack( push )
    #pragma pack( 1 )
#endif
API void umver();
class CLoginRequest
{
private:
	char		name[MAX_CHARLENGTH]; //�û���
	char		password[MAX_CHARLENGTH];//����
	CMD5Encrypt m_cMd5;
public:
	CLoginRequest()
	{
		memset(name,0,MAX_CHARLENGTH);
		memset(password,0,MAX_CHARLENGTH);
	}
	//���ú͵õ��û���
	char* GetName()
	{
		return name;
	}
	void  SetName(char* buf)
	{
		if(buf == NULL)return;
		memset(name,0,MAX_CHARLENGTH);
		int length = (strlen(buf) >= MAX_CHARLENGTH -1 ? MAX_CHARLENGTH - 1: strlen(buf));
		memcpy(name,buf,length);
		name[length] = '\0';
	};
	//���ú͵õ�����
	char* GetPassword()
	{
		return password;
	}
	//��������Ϊ����
	void  SetPassword(char* buf)
	{
		
		m_cMd5.GetEncrypted(buf,password);
		password[MAX_CHARLENGTH-1]='\0';
	}
	//����������Ѿ�������
	void SetEncryptedPassword(char* buf)
	{
		memcpy(password,buf,MAX_CHARLENGTH);
		password[MAX_CHARLENGTH-1]='\0';
	}
	void Empty()
	{
		memset(name,0,MAX_CHARLENGTH);
		memset(password,0,MAX_CHARLENGTH);
	}
}
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif // _VXWORKS_
;

class CUserFullInfo
{
// private:
public: // ��������Ա��С [pengguofeng 5/9/2013]
	CMD5Encrypt m_cMd5;
public:
	char		name[MAX_CHARLENGTH]; //�û���
	char		password[MAX_CHARLENGTH];//����
	unsigned char	m_Actor;//Ȩ��
	char		fullname[MAX_CHARLENGTH];//�����û���
	char		discription[2*MAX_CHARLENGTH];//�û�����
public:
	CUserFullInfo()
	{
		Empty();
	}
	~CUserFullInfo(){};
	//�õ��û���
	char* GetName()
	{
		name[MAX_CHARLENGTH-1] = '\0';
		return name;
	}
	//�����û���
	void  SetName(char* buf)
	{
		if(buf == NULL)return;
		memset(name,0,MAX_CHARLENGTH);
		int length = (strlen(buf) >= MAX_CHARLENGTH -1 ? MAX_CHARLENGTH - 1: strlen(buf));
		memcpy(name,buf,length);
		name[length] = '\0';
	}
	//�õ�����
	char* GetPassword()
	{
		return password;
	}
	//��������
	void  SetPassword(char* buf)
	{
		m_cMd5.GetEncrypted(buf,password);
		password[MAX_CHARLENGTH-1]='\0';
	}
	//�Ѿ����ܵ�����
	void  SetEncryptedPassword(char* buf)
	{
		memcpy(password,buf,MAX_CHARLENGTH);
		password[MAX_CHARLENGTH-1]='\0';
	}
	//�õ��û�������
	char* GetFullName()
	{
		return fullname;
	}
	//�����û�������
	void SetFullName(char* buf)
	{
		if(buf == NULL)return;
		memset(fullname,0,MAX_CHARLENGTH);
		int length = (strlen(buf) >= MAX_CHARLENGTH -1 ? MAX_CHARLENGTH - 1: strlen(buf));
		memcpy(fullname,buf,length);
		fullname[length] = '\0';
	}
	//�õ��û�����
	char* GetDiscription()
	{
		return discription;
	}
	
	//�����û�����
	void SetDiscription(char* buf)
	{
		if(buf == NULL)return;
		memset(discription,0, 2*MAX_CHARLENGTH);
		int length = (strlen(buf) >= 2*MAX_CHARLENGTH -1 ?  2*MAX_CHARLENGTH - 1: strlen(buf));
		memcpy(discription,buf,length);
		discription[length] = '\0';
	}
	//�õ�Ȩ��
	unsigned char   GetActor(){return m_Actor;}
	//����Ȩ��
	void  SetActor(unsigned char actor){ m_Actor = actor;}
	//����ֵ
	const CUserFullInfo& operator=(const CUserFullInfo& newInfo)
	{
		if (this != &newInfo)
		{
			memcpy(this, &newInfo, sizeof(CUserFullInfo));
		}
		return *this;
	}

	// ȡ���г�Ա�ĳ��� [pengguofeng 5/14/2013]
	unsigned char GetMemLen( const unsigned char &byMemId )
	{
		u8 byRet = 0;
		switch ( byMemId)
		{
		case 0:
			byRet = sizeof(m_cMd5.context.state);
			break;
		case 1:
			byRet = sizeof(m_cMd5.context.count);
			break;
		case 2:
			byRet = sizeof(m_cMd5.context.buffer);
			break;
		case 3:
			byRet = sizeof(name);
			break;
		case 4:
			byRet = sizeof(password);
			break;
		case 5:
			byRet = sizeof(m_Actor);
			break;
		case 6:
			byRet = sizeof(fullname);
			break;
		case 7:
			byRet = sizeof(discription);
			break;
		case 8:
//			byRet = sizeof(add1);
			break;
		case 9:
//			byRet = sizeof(add2);
			break;
		default:
			break;
		}

		return byRet;
	}
	//�ж�ĳһ����Ա�Ƿ�Ϊ�ַ���,0 1 2����MD5����Ϣ�������޸�
	bool IsMemString(const unsigned char &byMemId)
	{
		if ( byMemId == 3
			|| byMemId == 4
			|| byMemId == 6
			|| byMemId == 7
		/*	|| byMemId == 8
			|| byMemId == 9
		*/
			)
		{
			return TRUE;
		}
		return FALSE;
	}
	//�ж��û����Ƿ���ͬ
	bool IsEqualName(char* newName)
	{
		if (strlen(name) != strlen(newName) || strlen(newName) == 0 )
			return FALSE;
		for (unsigned int i=0 ; i< strlen(name) ; i++)
		{
			if (name[i] != newName[i])
				return FALSE;
		}
		return TRUE;
	}	
	//�жϿ����Ƿ���ͬ
	bool IsPassWordEqual(char* newPass)
	{
		for (int i=0 ; i< MAX_CHARLENGTH-1; i++)
		{
			if (password[i] != newPass[i])
				return FALSE;
		}
		return TRUE;
	}
	//�ҵ��յ�
	bool IsEmpty()
	{
		if ( strlen(password) > 16 )
		      return FALSE;
		return TRUE;
	}
	//���
	void Empty()
	{
		memset(this, 0, sizeof(CUserFullInfo));
		m_Actor = UM_OPERATOR;
	}	

}
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif // _VXWORKS_
;
class CLongMessage
{
public:
	u8 m_uTotal; //��Ϣ��֡��
	u8 m_uCurr;  //��ǰָ֡ʾ
	u8 m_uNumUsr;//��ǰ֡�е��û���
public:
	CLongMessage()
	{
		Reset();
	}
	~CLongMessage(){};
public:
	void Reset()
	{
		m_uTotal = 0;
		m_uCurr =  0;
		m_uNumUsr = 0;
	}
}
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif // _VXWORKS_
;
#ifdef WIN32
#pragma pack( pop )
#endif
#endif
