/********************************************************************
ģ����:      ���ݿ����������
�ļ���:      DBOperate.h
����ļ�:    
�ļ�ʵ�ֹ��� �����ӿ�ʵ��ͷ�ļ�
���ߣ�       
�汾��       4.0
------------------------------------------------------------------------
�޸ļ�¼:
��		��	�汾	�޸���	�߶���	�޸�����
2005/08/22	4.0		�κ�ƽ

*********************************************************************/

#if !defined(AFX_DBOPERATE_H__C01764B0_66E9_471B_9931_62802DDD3E83__INCLUDED_)
#define AFX_DBOPERATE_H__C01764B0_66E9_471B_9931_62802DDD3E83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4146)

#import "C:\Program Files\Common Files\System\ADO\msado15.dll"\
	no_namespace \
rename("EOF","ISEOF")


#include <winsock2.h>
#pragma comment(lib, "ws2_32")
#include "kdvtype.h"

//һЩ�궨��

#define  VOD_OK                 (u16)0    //VOD������ɹ� 
#define  VODERR_BASE_CODE		(u16)20000//�������׼ֵ


//��¼ʱ�������֤ʧ��
#define VODERR_DB_INDENTIFY_METHOD			(u16)(VODERR_BASE_CODE + 1) 
//����������Ϣʧ��
#define VODERR_DB_LOADCLASSINFO_METHOD		(u16)(VODERR_BASE_CODE + 2) 
//���������е��ļ���Ϣʧ��
#define VODERR_DB_LOADFILEINFO_METHOD		(u16)(VODERR_BASE_CODE + 3) 
//����������Ȩ����Ϣ
#define VODERR_DB_LOADPOPINFO_METHOD		(u16)(VODERR_BASE_CODE + 4) 
//�����û���½����
#define VODERR_DB_CHANGEPWD_METHOD			(u16)(VODERR_BASE_CODE + 5) 
//�������Ȩ�޼�
#define VODERR_DB_UPDATEPOPS_METHOD			(u16)(VODERR_BASE_CODE + 6) 
//������־��Ϣʧ��
#define VODERR_DB_LOADLOGINFO_METHOD		(u16)(VODERR_BASE_CODE + 7) 
//����һ���㲥��¼
#define VODERR_DB_ADDONELOG_METHOD			(u16)(VODERR_BASE_CODE + 8) 
//ɾ��һ���㲥��¼
#define VODERR_DB_DELONELOG_METHOD			(u16)(VODERR_BASE_CODE + 9) 
//����һ���ļ�������¼
#define VODERR_DB_ADDONEPUB_METHOD			(u16)(VODERR_BASE_CODE + 10)
//����һ���ļ�������¼
#define VODERR_DB_UPDATEONEPUB_METHOD		(u16)(VODERR_BASE_CODE + 11) 
//ɾ��һ���ļ�������¼
#define VODERR_DB_DELONEPUB_METHOD			(u16)(VODERR_BASE_CODE + 12) 
//����һ���ļ����¼
#define VODERR_DB_ADDONECLASS_METHOD		(u16)(VODERR_BASE_CODE + 13)
//����һ���ļ����¼
#define VODERR_DB_UPDATEONECLASS_METHOD		(u16)(VODERR_BASE_CODE + 14) 
//ɾ��һ���ļ����¼
#define VODERR_DB_DELONECLASS_METHOD		(u16)(VODERR_BASE_CODE + 15) 
//�����û���Ϣʧ��
#define VODERR_DB_LOADUSERINFO_METHOD		(u16)(VODERR_BASE_CODE + 16) 
//����һ���û���¼
#define VODERR_DB_ADDONEUSER_METHOD			(u16)(VODERR_BASE_CODE + 17)
//����һ���û���¼
#define VODERR_DB_UPDATEONEUSER_METHOD		(u16)(VODERR_BASE_CODE + 18) 
//ɾ��һ���û���¼
#define VODERR_DB_DELONEUSER_METHOD			(u16)(VODERR_BASE_CODE + 19) 
//����һ���û���ͣ�á����ü�¼���
#define VODERR_DB_SETONEUSERFLAG_METHOD		(u16)(VODERR_BASE_CODE + 20)
//����һ���㲥��¼
#define VODERR_DB_UPDATEONEPLAYLOG_METHOD   (u16)(VODERR_BASE_CODE + 21)
//��ѯ��ǰ�㲥����
#define VODERR_DB_QUERYNUMOFPLAYING_METHOD	(u16)(VODERR_BASE_CODE + 22);


//�����ļ����������ݿ����������Ч
#define VODERR_OPEN_DB_PARAM				(u16)(VODERR_BASE_CODE + 47) 
//�����ݿ����ӵĲ���ʧ��
#define VODERR_OPEN_DB						(u16)(VODERR_BASE_CODE + 48) 
//���ݿ�û�н������Ӳ������Դ��ڹر�״̬
#define VODERR_DB_CLOSE_STATUS				(u16)(VODERR_BASE_CODE + 49) 


//�����ļ�����ʧ��
#define VODERR_DB_INSERT_METHOD				(u16)(VODERR_BASE_CODE + 50) 
//ȡ�������ļ�����ʧ��
#define VODERR_DB_CANCEL_METHOD				(u16)(VODERR_BASE_CODE + 51) 
//��ѯ�ļ�����״̬����ʧ��
#define VODERR_DB_QUERY_METHOD				(u16)(VODERR_BASE_CODE + 52) 
//�����ļ�����״̬����ʧ��
#define VODERR_DB_UPDATE_METHOD				(u16)(VODERR_BASE_CODE + 53)
//�������ݿ����ʧ��
#define VODERR_DB_UPDATEDB_METHOD			(u16)(VODERR_BASE_CODE + 54)

//���һ��ֱ��Դʧ��
#define VODERR_DB_ADDONELIVE_METHOD			(u16)(VODERR_BASE_CODE + 55)
#define VODERR_DB_DELONELIVE_METHOD			(u16)(VODERR_BASE_CODE + 56)
#define VODERR_DB_UPDATEONELIVE_METHOD		(u16)(VODERR_BASE_CODE + 57)



#define MAX_LIST_PAGE_SIZE		10


enum emDbType
{
	emDbTypeMySQL,
	emDbTypeMSSQL,
	emDBTypeNone = 255
};

#define PORT_MYSQL	(u16)3306
#define PORT_MSSQL	(u16)1433

//DBOperate�ඨ��

class CDBOperate  
{
public:
	CDBOperate();
	virtual ~CDBOperate();

public:
	emDbType JudgeDBType(char *IP);
	
	/*=============================================================================
	�� �� ����OpenDB
	��    �ܣ������ݿ�
	��    ����szHostIP
			  szUserName
			  szPwd
			  wPort = 1433          
	�� �� ֵ��WORD      : ����VOD_OK ��ʾ�����ɹ���
	=============================================================================*/
	virtual u16 OpenDB(CString &szHostIP, CString &szUserName, CString &szPwd, u16 wPort = 3066, emDbType dbType = emDbTypeMySQL );
	/*=============================================================================
	�� �� ����CloseDB
	��    �ܣ��ر����ݿ�
	��    ������	           
	�� �� ֵ��WORD
	=============================================================================*/
	virtual u16 CloseDB();
	/*=============================================================================
	�� �� ����GetErrorDescript
	��    �ܣ� 
	��    ������	           
	�� �� ֵ��CString
	=============================================================================*/
	 virtual CString GetErrorDescript();
	/*=============================================================================
	�� �� ����PublishOneFile
	��    �ܣ�
	��    ����szFilePath
	          szFileName
			  bSecret
	�� �� ֵ��WORD
	=============================================================================*/
	
	virtual u16 PublishOneFile(char *szFilePath, char *szFileName, u32 dwFileSize,
							   char *szPublishTime, BOOL bSecret, u32 dwProgramLen);
	/*=============================================================================
	�� �� ����CancelOnePublishedFile
	��    �ܣ�
	��    ����szFilePath
			  szFileName
	�� �� ֵ��WORD
	=============================================================================*/
	
	virtual u16 UpdatePublishOneFile(char *szFilePath, char *szFileName, u32 dwFileSize, u32 dwProgramLen);
	/*=============================================================================
	�� �� ����UpdateOnePublishedFile
	��    �ܣ�
	��    ����szFilePath
			  szFileName
	�� �� ֵ��WORD
	=============================================================================*/

	virtual u16 CancelOnePublishedFile(char *szFilePath, char *szFileName);
	/*=============================================================================
	�� �� ����QueryOneFileStauts
	��    �ܣ�
	��    ����szFilePath
			  szFileName
			  bPublished
	�� �� ֵ��WORD
	=============================================================================*/
	virtual u16 QueryOneFileStauts(char *szFilePath, char *szFileName, BOOL &bPublished);	 

	/*=============================================================================
	�� �� ����UpdateDataBase
	��    �ܣ������������ݿ� �ж������ı��ֶ��Ƿ���ڣ������ڵĻ����Զ�����
	��    ����
	�� �� ֵ��WORD
	�޸ļ�¼��
	����			�汾		�޸���		�޸ļ�¼
	2012/07/26		4.7			�޿���		����
	=============================================================================*/
	virtual u16 UpdateDataBase();

	/*=============================================================================
	�� �� ����UpdateTBLvod_stream
	��    �ܣ�����vod_stream��
	��    ����
	�� �� ֵ��WORD
	�޸ļ�¼��
	����			�汾		�޸���		�޸ļ�¼
	2013/01/08		4.7			����		����
	=============================================================================*/
	virtual u16 UpdateTBLvod_stream();
	/*=============================================================================
	�� �� ����AddOneLiveSource
	��    �ܣ�����һ��ֱ������Դ
	��    ����bMulticast true���鲥ֱ��  false windowsmedia server�ϵ�ֱ��
	�� �� ֵ��WORD
	�޸ļ�¼��
	����			�汾		�޸���		�޸ļ�¼
	2013/01/08		4.7			����		����
	=============================================================================*/
	virtual WORD AddOneLiveSource( CString &szStreamSource,CString &szIPAddress, bool bMulticast=true);
	
	/*=============================================================================
	�� �� ����DeleteOneLiveSource
	��    �ܣ�ɾ��һ��ֱ��Դ
	��    ����
	�� �� ֵ��WORD
	�޸ļ�¼��
	����			�汾		�޸���		�޸ļ�¼
	2013/01/08		4.7			����		����
	=============================================================================*/
	virtual WORD DeleteOneLiveSource(CString strSourceName,CString strIPAddress);
	/*=============================================================================
	�� �� ����UpdateOneLiveSource
	��    �ܣ�����ֱ��Դ
	��    ����
	�� �� ֵ��WORD
	�޸ļ�¼��
	����			�汾		�޸���		�޸ļ�¼
	2013/01/08		4.7			����		����
	=============================================================================*/
	virtual WORD UpdateOneLiveSource(CString strOldStreamName,CString strOldIPAddress,CString strNewSourceName,CString strNewIPAddress);

private:
	bool	InitSocket();

protected:
	_ConnectionPtr m_pCon;		//���ݿ����ӱ���

	CString m_szErrorInfo;

	CRITICAL_SECTION  m_csdbLock;

	BOOL m_bWSAInited;

	emDbType m_emDbType;
};

#endif // !defined(AFX_DBOPERATE_H__C01764B0_66E9_471B_9931_62802DDD3E83__INCLUDED_)
