#include "kdvtype.h"
#include "osp.h"
#include "netbuf.h"

//ģ��汾��Ϣ�ͱ���ʱ�� �� �����Ŀ�İ汾�ͱ���ʱ��
void netbufver()
{
}


//ģ�������Ϣ �� �����Ŀ�İ�����Ϣ
void netbufhelp()
{
}


/*=============================================================================
������		nbInit
����		��ʼ�� �����ط����� ��,����֮һ�� �ⲿʹ��
�㷨ʵ��	������ѡ�
�������˵����
����ֵ˵���� �ɹ�����FE_OK,  ʧ�ܷ��ش�����
=============================================================================*/
u16 nbInit()
{
	return NB_NO_ERROR;
}


/*=============================================================================
������		nbRelease
����		���� �����ط����� ��,�������ò�������,ֱ���ͷſ⣬����֮һ�� �ⲿʹ��
�㷨ʵ��	������ѡ�
�������˵������
����ֵ˵���� �ɹ�����FE_OK,  ʧ�ܷ��ش�����
=============================================================================*/
u16 nbRelease()
{
	return NB_NO_ERROR;
}

class CLinkerObj
{

};

CNetBuf::CNetBuf()
{
}
CNetBuf::~CNetBuf()
{
	if (m_pcLinkerObj)
	{
		delete m_pcLinkerObj;
		m_pcLinkerObj = NULL;
	}

	if (m_hSynSem)
	{
		delete m_hSynSem;
		m_hSynSem = NULL;
	}
}

	//��
u16 CNetBuf::CreateNetBuf( TRSParam tRSParam, u16 wSndPort )
{
	return NB_NO_ERROR;
}
	//�ر�
u16 CNetBuf::FreeNetBuf()
{
	return NB_NO_ERROR;
}
	//�����뷢�Ͷ��໥�����Ľ��յ�ַ�����ظ����ã����ú��µ�ַȡ���ɵ�ַ, ����NULL������ײ��׽���
	//���ڽ��ա����淢�Ͷ˵�rtp������Ⲣ����rtcp�ش������
u16 CNetBuf::SetLocalAddr( TNetLocalAddr *ptLocalAddr )
{
	return NB_NO_ERROR;
}
	//��������ն��໥�����Ľ��յ�ַ�����ظ����ã����ú��µ�ַȡ���ɵ�ַ, ����NULL������ײ��׽���
	//���ڽ��ս��ն˵�rtcp�ش�����������������󷢳���Ӧrtp��
u16 CNetBuf::SetChannelLocalAddr( TNetAddr *ptChannelLocalAddr )
{
	return NB_NO_ERROR;
}

