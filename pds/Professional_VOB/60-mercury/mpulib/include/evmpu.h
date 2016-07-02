/*****************************************************************************
   ģ����      : mpulib
   �ļ���      : evmpu.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: mpulib�ڲ���Ϣ����
   ����        : ����
   �汾        : V4.6  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��       �汾        �޸���      �޸�����
   2009/3/14    4.6         �ű���      ע��
******************************************************************************/

#ifndef _EVENT_MPUBAS_H_
#define _EVENT_MPUBAS_H_
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
//�޸�BAS���͵�ַ
OSPEVENT( EV_BAS_MODSENDADDR,       EV_BAS_BGN + 17 );

OSPEVENT( EV_BAS_SHOWDEBUG,         EV_BAS_BGN + 18 );				//ȡBASdebug�ļ���Ϣ

OSPEVENT( EV_VMP_TIMER,                EV_VMP_BGN );                //�ڲ���ʱ��
OSPEVENT( EV_VMP_NEEDIFRAME_TIMER,     EV_VMP_BGN + 1 );            //�ؼ�֡��ʱ��
OSPEVENT( EV_VMP_CONNECT_TIMER,        EV_VMP_BGN + 2 );            //���Ӷ�ʱ��
OSPEVENT( EV_VMP_CONNECT_TIMERB,       EV_VMP_BGN + 3 );            //���Ӷ�ʱ��
OSPEVENT( EV_VMP_REGISTER_TIMER,       EV_VMP_BGN + 4 );            //ע�ᶨʱ��
OSPEVENT( EV_VMP_REGISTER_TIMERB,      EV_VMP_BGN + 5 );            //ע�ᶨʱ��
OSPEVENT( TIME_GET_MSSTATUS,           EV_VMP_BGN + 6 );            //ȡ��������״̬
OSPEVENT( EV_VMP_SHOWDEBUG,            EV_BAS_BGN + 19 );			//ȡVMPdebug�ļ���Ϣ,19���˺�BAS����Ϣ�����������Ժ�BAS��Ϣ���ӣ����ֵ�͵ó���

#endif// _EVENT_MPUBAS_H_

//END OF FILE

