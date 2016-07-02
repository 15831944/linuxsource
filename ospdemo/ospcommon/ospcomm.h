/*****************************************************************************
   ģ����      : OSPCOMM 
   �ļ���      : ospcomm.h 
   �ļ�ʵ�ֹ���: ͨ����Ϣ�궨��
   �汾        : V3.0  Copyright(C) 2001-2002 KDC, All rights reserved.
******************************************************************************/
#ifndef _OSPCOMM_H_
#define _OSPCOMM_H_

#include "osp.h"
#include "io.h"

//����Ĭ��ֵ
#define	  PAUSESIZE				  50						//�ϴ���ͣ��Ҫ�ļ�����50M
#define	  BASEDOCUMENT			  "D:\\tmp"					//������Ĭ�ϸ�Ŀ¼
#define	  SERVERIP				  "127.0.0.1"				//������Ĭ��IP
#define	  SERVERNODE			  "6682"					//������Ĭ��NODE
#define   LOGPRINTLEVELH		  1							//���ÿ���log��ӡ�������
#define   LOGPRINTLEVELM		  2							//���ÿ���log��ӡ�������
#define   LOGPRINTLEVELL		  3							//���ÿ���log��ӡ����ȫ��

//���峣�õĲ���
const u16 CLIENT_APP_ID			= 100;						//�ͻ���APP��
const u16 SERVER_APP_ID			= 150;						//��������APP��
const u16 APP_PRIO				= 100;						//�������Ϳͻ���APP���ȼ�
const u32 UPLOAD_PAGESIZE		= 10240;					//�ļ�����ÿ���ֽ���
const u32 SERVER_MAX_INS		= 20;						//������������ 
const u16 SERVER_LISTEN_PORT	= 6682;					    //����˼����˿�
const long RETRY_TIME			= 3 * 1000;					//�������ӷ��������ʱ��
const u16 CHECK_PERIOD			= 10;						//��·�������
const u8 PING_NUM				= 3;						//ping����
const s8  CLIENT_PRI			= 100;						// �ͻ������ȼ�
const u16 MAX_NAME_LEN			= 260;						//�ļ�����󳤶�

/**********************************************************************************
							�ͻ��˸�������֮�����Ϣ����
**********************************************************************************/
const u16 USR_EV_BASE			= (OSP_USEREVENT_BASE + 20);//�û��¼�����ʼ��ַ����
//(�ͻ��˵��ͻ���)   (��Ϣ��Ϊ��)
const u16 CTOC_CONN_REQ			= (USR_EV_BASE + 1);		//����������Ϣ
//(�ͻ��˵������)   (��Ϣ��Ϊ��)					
const u16 CTOS_CONN_REQ			= (USR_EV_BASE + 2);		//����������Ϣ	
//(����˵��ͻ���)   (��Ϣ��Ϊ��)				
const u16 STOC_CONN_ACK			= (USR_EV_BASE + 3);		//������������	
//(����˵��ͻ���)	 ����Ϣδʹ��		
const u16 STOC_CONN_NACK		= (USR_EV_BASE + 4);		//�ܾ���������	
//(�ͻ��˵��ͻ���)   (��Ϣ��Ϊ��)				
const u16 CTOC_CONN_DISC		= (USR_EV_BASE + 5);		//�Ͽ���������					

//(�ͻ��˵��ͻ���)   (��Ϣ��ΪCFileInfo)
const u16 CTOC_UPLOAD_REQ		= (USR_EV_BASE + 6);		//�����ϴ��ļ�
//(�ͻ��˵������)   (��Ϣ��ΪCFileInfo)					
const u16 CTOS_UPLOAD_REQ		= (USR_EV_BASE + 7);		//�����ϴ��ļ�	
//(����˵��ͻ���)   (��Ϣ��Ϊ��)				
const u16 STOC_UPLOAD_ACK		= (USR_EV_BASE + 8);		//�����ϴ��ļ�����				
//(����˵��ͻ���)   (��Ϣ��ΪeErrCode)
const u16 STOC_UPLOAD_NACK		= (USR_EV_BASE + 9);		//�ܾ��ϴ��ļ�����	
//(�ͻ��˵������)   (��Ϣ��Ϊ��)			
const u16 CTOS_UPLOAD_FREADERR	= (USR_EV_BASE + 10);		//�ϴ����̶��ļ�����			
//(�ͻ��˵������)   (��Ϣ��ΪUploadFileInfo)
const u16 CTOS_UPLOADING_REQ	= (USR_EV_BASE + 11);		//�����ϴ�δ��ɵ��ļ�����	
//(����˵��ͻ���)   (��Ϣ��Ϊ��)	
const u16 STOC_UPLOADING_ACK	= (USR_EV_BASE + 12);		//���ܼ����ϴ�δ��ɵ��ļ�����	
//(�ͻ��˵������)   ����Ϣδʹ��
const u16 CTOS_UPLOADING_OVER	= (USR_EV_BASE + 13);		//�ļ��ϴ����					
//(�ͻ��˵��ͻ���)   (��Ϣ��Ϊ��)
const u16 CTOC_UPLOADING_PAUSE	= (USR_EV_BASE + 14);		//�ļ��ϴ���ͣ		
//(�ͻ��˵��ͻ���)   (��Ϣ��Ϊ��)			
const u16 CTOC_UPLOADING_RESTART= (USR_EV_BASE + 15);		//�ļ��ϴ���ͣ��ʼ			
//(����˵��ͻ���)   (��Ϣ��ΪeErrCode)
const u16 STOC_UPLOADING_NACK	= (USR_EV_BASE + 16);		//�������˴����ļ�������Ϣ		
//(�ͻ��˵��ͻ���)	 (��Ϣ��Ϊ��)
const u16 CTOC_FILE_REMOVE		= (USR_EV_BASE + 17);		//�ļ��ϴ���ͣʱɾ��			
//(�ͻ��˵��ͻ���)   ����Ϣδʹ��
const u16 CTOC_SERVER_IP_PROT	= (USR_EV_BASE + 18);		//�ͻ������õķ�����IP���˿���Ϣ
//(�ͻ��˵��ͻ���)	 (��Ϣ��Ϊ��)
const u16 CTOS_CONNENT_TIMER	= (USR_EV_BASE + 19);		//�ͻ�����������ʱ�ĳ�ʱ��ʱ��	
//(��������������)	 (��Ϣ��Ϊ��)
const u16 S_WAIT_UPLOAD_TIMER	= (USR_EV_BASE + 20);		//�������յ�����������յ��ϴ������ĳ�ʱ��ʱ��
//(��������������)	 (��Ϣ��Ϊ��)	
const u16 S_WAIT_UPLOAD_AGAIN_TIMER	= (USR_EV_BASE + 21);	//������������ɵ����յ��ϴ�����ĳ�ʱ��ʱ��		

typedef enum ErrCode
{
	STOC_FILE_CREATEERR = 1,						//�����������ļ�����
	STOC_FILE_WRITEERR,								//������д�ļ�����
	STOC_SERVER_NOSPACE								//�������ռ䲻��
}ErrCode;

//�ļ���Ϣ�ṹ��,��¼�ļ�������С
typedef struct CFileInfo
{
	s8			achFileName[MAX_PATH];				//�ļ���
	DWORD		dwFileSize;							//�ļ���С
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
CFileInfo,*pCFileInfo;

//�ļ���Ϣ�ṹ��,��¼�ϴ��ļ�ÿ�����ݸ�����
typedef struct UploadFileInfo
{
	s8			achData[UPLOAD_PAGESIZE];			//�ļ�����
	s32			nDataLen;							//��Ч���ݳ���
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
UploadFileInfo,*pUploadFileInfo;

#endif  /* _OSPCOMM_H_ */

/* end of file ospcomm.h */