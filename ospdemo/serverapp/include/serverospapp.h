/*==============================================================================
ģ����  ��������APP
�ļ���  ��cserverospapp.h 
����ļ���serverinstance.h
ʵ�ֹ��ܣ���װ������Instance,ʵ���ⲿ���ýӿ�
����    ��������
��Ȩ    ��<Copyright(c) 2013-2013 Suzhou Keda Technology Co.,Ltd.All right reserved.>
--------------------------------------------------------------------------------
�޸ļ�¼��
��  ��        �汾        �޸���        �߶���        �޸ļ�¼
2013/08/08	  V1.0        ������						����
==============================================================================*/

#ifndef CCLIENTOPSAPP_H_
#define CCLIENTOPSAPP_H_

#include "serverinstance.h"

/*==============================================================================
����    ��CServerOspApp
����    ����װOSPģ���� zTemplate ���࣬���ڸ��ⲿ���õĽ���

--------------------------------------------------------------------------------
�޸ļ�¼��
��  ��        �汾        �޸���            �߶���            �޸ļ�¼
2013/08/08    V1.0        ������						     ��¼�ؼ�����
==============================================================================*/
class CServerOspApp
{
public:
	CServerOspApp();
	~CServerOspApp();
	
	void CInitServer();							//��ʼ��������
	void CCreateServerTcpNode();				//��������������app

private:
	s32		m_nServerNode;						//��¼������NODE
};

typedef zTemplate<CServerInstance, SERVER_MAX_INS,CServerOspApp, 20> COspApp;
extern COspApp g_zServerOspApp;					//��װzTemplateģ����


//������״̬
typedef enum ServerState
{
		SERVER_INIT_OK,							//��������ʼ���ɹ�
		SERVER_INIT_FAIL,						//��������ʼ��ʧ��
		SERVER_NOSPACE							//�������ռ䲻��
}ServerState;


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern s32 g_nServerPort;						//��¼���õĶ˿ں�
extern s8 g_achServerBase[MAX_NAME_LEN];		//��¼�ļ���ŵ�Ŀ¼
			
//////////////////////////////�������ӿ�/////////////////////////////////
//��CClientOspApp����з�װ���ṩ�������ӿڣ�������ͬ
void CB_RevProgress(s8 chProgress);										//�ú������û���䣬�Բ������д���
void CB_ServerCallBack(void (*pCB_ServerRevProgress)(s8 chProgress));	//�ص�������

//////////////////////////////Telnet�ӿ�/////////////////////////////////

API void serversetloglevel(u8 byScrnLevel);							
API void serverhelp();
API void serverversion();
API void serverspace();
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif

//end
