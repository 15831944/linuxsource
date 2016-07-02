/*==============================================================================
ģ����  ��������ʵ��
�ļ���  ��serverinstance.h  
����ļ���osp.h, ospcomm.h, kdvtype.h
ʵ�ֹ��ܣ��̳���CInstance,ʵ��ʵ����ع���
����    ��������
��Ȩ    ��<Copyright(c) 2013-2013 Suzhou Keda Technology Co.,Ltd.All right reserved.>
--------------------------------------------------------------------------------
�޸ļ�¼��
��  ��        �汾        �޸���        �߶���        �޸ļ�¼
2013/08/08	  V1.0        ������						����
==============================================================================*/

#ifndef SERVERINSTANCE_H_
#define SERVERINSTANCE_H_

#include "osp.h"
#include "ospcomm.h"
#include "kdvtype.h"
/*==============================================================================
����    ��CServerInstance
����    ���̳�OSPģ���� CInstance ���࣬ʵ��ʵ����ͨ�Ŵ�����

--------------------------------------------------------------------------------
�޸ļ�¼��
��  ��        �汾        �޸���            �߶���            �޸ļ�¼
2013/08/08    V1.0        ������						     ��¼�ؼ�����
==============================================================================*/
class CServerInstance : public CInstance
{
public:
	CServerInstance();
	~CServerInstance();

	void DaemonInstanceEntry( CMessage *const pMsg,CApp* pcApp);	//�ػ�ʵ�����
	void InstanceEntry( CMessage *const pMsg);						//ʵ�����

private:
	void CConnectSReqManage(CMessage *const pMsg);					//�ͻ����������ӷ������Ĵ�����
	void DisConnectedCtoSManage(u32 SrcNodeID);						//�Ͽ�C->S���Ӵ�����
	void CUploadtoSReqManage(CMessage *const pMsg);					//C->S�����ϴ�������
	void CUploadingtoSReqManage(CMessage *const pMsg);				//C->S��������ϴ�������
	void CUploadtoSErrManage();										//C->S�ϴ�ʧ�ܴ�����
	void FileRenameManage(s8 *pFileName);							//�����������ļ�ʱ��������⺯��
	BOOL IsServerSpaceEnough(DWORD dwFileSize);						//�������жϵ�ǰ�ռ��Ƿ����ĺ���
	void ServerInstanceRelease();									//�������ͷ���Դ
	void FileNameOpt(s8 *pchName);									//������ͨ��·���ļ�����ȡ�ļ����ĺ���

	s8	m_achPathFileName[MAX_NAME_LEN];			//��¼�ļ�·����
	s32 m_nSaveClientNode;                          //����ͻ���NODE��
	s32 m_nSaveClientID;                            //����ͻ���ID�� 
	s32 m_dwServerSpace;							//��¼���̿ռ��С
	FILE		   *m_phGetFile;					//�������ļ����
	UploadFileInfo m_tGetFileInfo;					//���յ��ļ���Ϣ�ṹ��
	CFileInfo m_tCGetFileInfo;					    //���յ��ļ���Ϣ�ṹ��

	enum InsState
	{
		STATE_IDLE,									//����״̬
		STATE_READY,								//����״̬
		STATE_BUSY									//�����ļ�״̬
	};
};

#endif

//end
