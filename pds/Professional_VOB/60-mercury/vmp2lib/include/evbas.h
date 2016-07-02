/*****************************************************************************
   ģ����      : ����������
   �ļ���      : evbas.h
   ����ʱ��    : 2003�� 6�� 25��
   ʵ�ֹ���    : �ڲ���Ϣ����
   ����        : ������
   �汾        : 
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
 
******************************************************************************/
#ifndef _EVENT_BAS_H_
#define _EVENT_BAS_H_
#include "eventid.h"

//������������ʼ����Ϣ

OSPEVENT( EV_BAS_INI,               EV_BAS_BGN + 1 );
//�����������˳���Ϣ��
OSPEVENT( EV_BAS_EXIT,              EV_BAS_BGN + 2 );
//��ʱ������Ϣ
OSPEVENT( EV_BAS_CONNECT,           EV_BAS_BGN + 3 );
//��ʱע����Ϣ
OSPEVENT( EV_BAS_REGISTER,          EV_BAS_BGN + 4 );
//�ȴ�ע��Ӧ��
OSPEVENT( EV_BAS_REGTIMEOUT,        EV_BAS_BGN + 5 );
//�˳�����
OSPEVENT( EV_BAS_QUIT,              EV_BAS_BGN + 6 );
//��ʾ��������Ϣ
OSPEVENT( EV_BAS_SHOW,              EV_BAS_BGN + 7 );
//ǿ�ƹرո�ͨ��
OSPEVENT( EV_BAS_STOPCHNADP,        EV_BAS_BGN + 8 );
//��ʾBas״̬
OSPEVENT( EV_BAS_SHOWINFO,          EV_BAS_BGN + 10 );
//����MCU.B
OSPEVENT( EV_BAS_CONNECTB,          EV_BAS_BGN + 11 );
//��MCU.Bע��
OSPEVENT( EV_BAS_REGISTERB,         EV_BAS_BGN + 12 );
//��MCUȡ��������״̬
OSPEVENT( EV_BAS_GETMSSTATUS,       EV_BAS_BGN + 13 );
//����ؼ�֡
OSPEVENT( EV_BAS_NEEDIFRAME_TIMER,  EV_BAS_BGN + 14 );
//������ͨ��״̬
OSPEVENT( EV_BAS_SENDCHNSTATUS,     EV_BAS_BGN + 15 );

//����BAS״̬�л���ʱ��
OSPEVENT( EV_BAS_STATUSCHK_TIMER,	EV_BAS_BGN + 16 );

#endif// _EVENT_BAS_H_
