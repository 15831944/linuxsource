/*==============================================================================
ģ����  ���ͻ���APP
�ļ���  ��cclientospapp.h 
����ļ���clientinstance.h
ʵ�ֹ��ܣ���װ�ͻ���Instance,ʵ���ⲿ���ýӿ�
����    ��������
��Ȩ    ��<Copyright(c) 2013-2013 Suzhou Keda Technology Co.,Ltd.All right reserved.>
--------------------------------------------------------------------------------
�޸ļ�¼��
��  ��        �汾        �޸���        �߶���        �޸ļ�¼
2013/08/08	  V1.0        ������						����
==============================================================================*/

#ifndef CCLIENTOSPAPP_H_
#define CCLIENTOSPAPP_H_

#include "clientinstance.h"

/*==============================================================================
����    ��CClientOspApp
����    ����װOSP�࣬���ڸ��ⲿ���õĽ���

--------------------------------------------------------------------------------
�޸ļ�¼��
��  ��        �汾        �޸���            �߶���            �޸ļ�¼
2013/08/08    V1.0        ������						     ��¼�ؼ�����
==============================================================================*/
class CClientOspApp
{
public:
	CClientOspApp();
	~CClientOspApp();
	
	BOOL InitClient();								//��ʼ���ͻ���
	BOOL CreatClientApp();							//����APP
	void ClientConnectServer();						//�ͻ������ӷ�����
	void ClientDisconnectServer();					//�Ͽ�����					
	void ClientUploadFile(s8 *chUploadFile, DWORD dwGetFileSize);//�ͻ����ϴ��ļ���������
	void ClientPauseUploadFile();					//�ͻ����ϴ��ļ�ʱ��ͣ
	void ClientRestartUploadFile();					//�ͻ�����ͣ�����¿�ʼ�ϴ�
	void ClientRemoveFile();						//�ͻ���ɾ���ļ�
	void ClientSetServerIpandPort(const s8 *pchServerIp, s32 nServerPort);//�ͻ�������Ҫ���ӵķ�������IP���˿ں�

private:
	s8 m_pchServerIp[MAX_NAME_LEN];					//���õķ�����IP
	s32 m_nServerPort;								//���õķ�����PORT
};

typedef zTemplate<CClientInstance, 1, CClientOspApp, 20> COspApp;
extern COspApp g_zClientOspApp;

#endif
