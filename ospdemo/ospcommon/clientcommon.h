/*==============================================================================
ģ����  �������ӿ�
�ļ���  ��Clientcommon.h   
����ļ���osp.h,ospcomm.h,kdvtype.h
ʵ�ֹ��ܣ��ṩ���ⲿ���õĹ����ӿ�
����    ��������
��Ȩ    ��<Copyright(c) 2013-2013 Suzhou Keda Technology Co.,Ltd.All right reserved.>
--------------------------------------------------------------------------------
�޸ļ�¼��
��  ��        �汾        �޸���        �߶���        �޸ļ�¼
2013/08/08	  V1.0        ������						����
==============================================================================*/
#ifndef CLIENTCOMMON_H_
#define CLIENTCOMMON_H_

#include "osp.h"
#include "ospcomm.h"
#include "kdvtype.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern s32 g_nServerNode;							//�ͻ��˻�ȡ��node

//�ͻ���״̬
typedef enum CSendFileState
{
		C_SEND_FILE_TO_S_SETERR	= 100,				//C->S����ip����
		C_SEND_FILE_TO_S_UPLOADING,					//C->S�����ļ���
		C_SEND_FILE_TO_S_OVER,						//C->S�������
		C_SEND_FILE_TO_S_FAIL,						//C->S����ʧ��
		C_SEND_FILE_TO_S_PAUSE,						//C->S��ͣ��
		C_SEND_FILE_TO_S_NOPAUSE,					//C->S�����ļ�С��50M����������ͣ
		C_SEND_FILE_TO_S_BUSYNODEL,					//C->S�ϴ�ʱ����ɾ���ļ�
		C_SEND_FILE_TO_S_COEENCTING,				//C->S����������
		C_SEND_FILE_TO_S_COEENCT,					//C->S��������
		C_SEND_FILE_TO_S_NACKCOEENCT,				//C->S��������ʧ��
		C_SEND_FILE_TO_S_DICCOEENCT,				//C->S�Ͽ�����
		C_SEND_FILE_TO_S_DELETFILE					//C->Sɾ���ļ�
}CSendFileState;

//////////////////////////////�������ӿ�/////////////////////////////////
//��CClientOpsApp����з�װ���ṩ�������ӿڣ�������ͬ
void CB_CToSProgress(s8 chProgress);								//�ú������û���䣬�Բ������д���
void CB_CCallBack(void (*pCB_CToCProgress)(s8 chProgress));			//�ص�������
BOOL CCreatApp();													//����APP
BOOL CInitClient();													//��ʼ���ͻ���
void CConnectServer();												//�ͻ������ӷ�����
void CDisconnectServer();											//�Ͽ�����	
void CUploadFile(s8 *chUploadFile, DWORD dwGetFileSize);			//�ͻ����ϴ��ļ���������
void CPauseUploadFile();											//�ͻ����ϴ��ļ�ʱ��ͣ
void CRestartUploadFile();											//�ͻ�����ͣ�����¿�ʼ�ϴ�
void CRemoveFile();													//�ͻ���ɾ���ļ�
void CSetServerIpandPort(const s8 *pchServerIp, s32 nServerPort);	//�ͻ�������Ҫ���ӵķ�������IP���˿ں�

//////////////////////////////Telnet�ӿ�/////////////////////////////////
API void clientsetloglevel(u8 byScrnLevel);
API void clienthelp();
API void clientversion();

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif