/*****************************************************************************
   ģ����      : ¼���
   �ļ���      : RecError.h
   ����ļ�    : 
   �ļ�����    : ¼��������붨��
   ����        : ������
   �汾        : V0.9  Copyright(C) 2001-2003 KDC, All rights reserved.

-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
******************************************************************************/

#ifndef _KDV_RECORDER_ERROR_CODE_H_
#define _KDV_RECORDER_ERROR_CODE_H_
#include "ErrorId.h"

#define ERR_REC_NOERROR         ERR_REC_BGN + 0        //û�д���
#define ERR_REC_NORECORD        ERR_REC_BGN + 1        //û�м�¼
#define ERR_REC_OPENRECORD      ERR_REC_BGN + 2        //�򿪼�¼ʧ��
#define ERR_REC_UMMATCHCMD      ERR_REC_BGN + 3        //��ƥ������������һ�������豸��ʼ¼��
#define ERR_REC_VALIDCHN        ERR_REC_BGN + 4        //��Ч��ͨ��
#define ERR_REC_RECORDING       ERR_REC_BGN + 5        //����¼��
#define ERR_REC_PLAYING         ERR_REC_BGN + 6        //���ڲ���
#define ERR_REC_NOTPLAY         ERR_REC_BGN + 7        //����ͨ��û�ڲ��� 
#define ERR_REC_NOTRECORD       ERR_REC_BGN + 8        //¼��ͨ��û��¼��
#define ERR_REC_ACTFAIL         ERR_REC_BGN + 9        //ִ��ָ����������ʧ��
#define ERR_REC_CURDOING        ERR_REC_BGN + 10       //����Ķ�����ǰ����ִ��
#define ERR_REC_RCMD_TO_PCHN    ERR_REC_BGN + 11       //һ������ͨ�������¼��ͨ��
#define ERR_REC_PCMD_TO_RCHN    ERR_REC_BGN + 12       //һ��¼��ͨ�����������ͨ��
#define ERR_REC_DISKFULL        ERR_REC_BGN + 13       //������
#define ERR_REC_FILEDAMAGE      ERR_REC_BGN + 14       //�ļ���
#define ERR_REC_FILEEMPTY       ERR_REC_BGN + 15       //���ļ�
#define ERR_REC_FILEUSING       ERR_REC_BGN + 16       //ָ���ļ����ڱ�����
#define ERR_REC_FILENOTEXIST    ERR_REC_BGN + 17       //ָ���ļ�������
#define ERR_REC_PUBLICFAILED    ERR_REC_BGN + 18       //����ʧ��
#define ERR_REC_RENAMEFILE      ERR_REC_BGN + 19       //�ļ�����ͻ,�����ļ���ʧ��
#define ERR_REC_FILEEXISTED     ERR_REC_BGN + 20       //�ļ�����ͻ,�޷�¼��
#define ERR_REC_PLAYERRPAYLOAD  ERR_REC_BGN + 21       //�����ļ�ý���غ���������鲻ƥ��
#define ERR_REC_CALLLIBFAILED   ERR_REC_BGN + 22       //����¼���ʧ��,�������°�װ
#define ERR_REC_SETMOTHODFAILED ERR_REC_BGN + 23       //¼��ʽ����ʧ��
#define ERR_REC_PALYFILERES		ERR_REC_BGN + 24	   //¼���ļ��ֱ���ƫ��
#define ERR_REC_FILENAMETOOLONG ERR_REC_BGN + 25       //�����ļ�������    // add by jlb  081026
#define ERR_REC_HAS_UNDERLINE	ERR_REC_BGN + 26       //�û�����¼���ļ��ļ����а����»����ַ� // [8/1/2011 liuxu]
#define ERR_REC_WRITE_FILE_ERR	ERR_REC_BGN + 27       //д�ļ�����, ¼����ֹ // [8/1/2011 liuxu]
#define ERR_REC_CREATE_SNDSOCK  ERR_REC_BGN + 28    //����ģ�鴴��socketʧ�� // [pengjie 2011/8/25]
#define ERR_REC_PLAYFILE_HPDISMATCH  ERR_REC_BGN + 29    //���鲻֧�ַ����ļ���high profile����[zhouyiliang 20120309]

#ifndef RPC_S_SERVER_UNAVAILABLE
#define RPC_S_SERVER_UNAVAILABLE 0x800706ba		//RPC_S_SERVER_UNAVAILABLE = 0x800706ba ��������Ͽ�����
#endif


#endif /*!_KDV_RECORDER_ERROR_CODE_H_*/
