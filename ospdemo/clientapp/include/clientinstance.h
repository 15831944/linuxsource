/*==============================================================================
ģ����  ���ͻ���ʵ��
�ļ���  ��ClientInstance.h   
����ļ���
ʵ�ֹ��ܣ��̳���CInstance,ʵ��ʵ����ع���
����    ��������
��Ȩ    ��<Copyright(c) 2013-2013 Suzhou Keda Technology Co.,Ltd.All right reserved.>
--------------------------------------------------------------------------------
�޸ļ�¼��
��  ��        �汾        �޸���        �߶���        �޸ļ�¼
2013/08/08	  V1.0        ������						����
==============================================================================*/
#ifndef CLIENTINSTANCE_H_
#define CLIENTINSTANCE_H_

#include "clientcommon.h"

/*==============================================================================
����    ��CClientInstance
����    ���̳�OSPģ���� CInstance ���࣬ʵ��ʵ����ͨ�Ŵ�����

--------------------------------------------------------------------------------
�޸ļ�¼��
��  ��        �汾        �޸���            �߶���            �޸ļ�¼
2013/08/08    V1.0        ������						     ��¼�ؼ�����
==============================================================================*/
class CClientInstance : public CInstance 
{

public:

	CClientInstance();
	~CClientInstance();

	void DaemonInstanceEntry( CMessage *const pMsg, CApp* pcApp);	//ʵ���ػ����
	void InstanceEntry( CMessage *const pMsg);						//ʵ�����

private:
	void ClientConnectToServer();									//C->S�ͻ������ӵ�������
	void CDisconnectManage();									//C->C�ͻ��˶Ͽ����Ӵ�����
	void CToSConnectAckManage(CMessage *const pMsg);				//S->C�ͻ����յ�����������Ӧ������
	void CUploadFileReqToServer(CMessage *const pMsg);				//C->S�ͻ��������ϴ��ļ���������
	void CUploadFileToS();											//C->S�ͻ����ϴ��ļ�����
	void CUploadToSAckManage();										//S->C�ͻ����յ��ϴ��ļ�Ӧ����
	void CUploadToSNackManage(CMessage *const pMsg);				//S->C�ͻ����յ��ϴ�����ܾ��Ĵ�����
	void CUploadingToSAckManage();									//S->C�ͻ����յ������ϴ��ļ�Ӧ��Ĵ�����
	void CPauseUploadFileToS();										//C->C�ͻ����ϴ��ļ���������ͣ
	void CRestartUploadFileToS();									//C->C�ͻ�����ͣ�ļ��ϴ������¿�ʼ�ϴ�����
	void CPauseRemoveFileManage();									//C->C�ͻ�����ͣʱɾ���б��ļ�ʱ�ĳ�����
	void CReadFileErrManage();										//�ͻ����ϴ��ļ�ʧ��ʱ�Ĵ���
	void ClientInstanceRelease();									//�ͻ����ͷ���Դ
private:
	FILE *m_pFile;								//������ļ�ָ��
	s32 m_nServerID;							//������ID
	s32 m_nServerNode;							//������node
	u32 m_uCount;								//��¼�ϴ��ļ�����
	u32 m_uPacketCount;							//��¼�ϴ��ļ��ָ��ܰ���
	DWORD m_uLastPacketSize;					//��¼���һ�����ݰ��Ĵ�С
	u32 m_dProgress;							//��¼�������

	CFileInfo m_tCFileInfo;						//��¼�ϴ��ļ�����Ϣ�ṹ��	
	UploadFileInfo m_tUploadFileInfo;			//��¼�ϴ��ļ�����Ϣ�ṹ��

	enum InsState
	{
		STATE_IDLE,								//����״̬
		STATE_CONNECT,							//���ӽ��ɹ�״̬
		STATE_READY,							//��������״̬
		STATE_BUSY,								//�ϴ�״̬
		STATE_PAUSE								//��ͣ״̬
	};
};

#endif
