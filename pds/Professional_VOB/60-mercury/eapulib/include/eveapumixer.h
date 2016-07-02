/*****************************************************************************
   ģ����      : ������
   �ļ���      : evmixer.h
   ����ʱ��    : 2010�� 01�� 09��
   ʵ�ֹ���    : 
   ����        : 
   �汾        : 
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   03/12/09    3.0                     ����
******************************************************************************/

#ifndef _EVENT_MCU_MIXER_H_
#define _EVENT_MCU_MIXER_H_
#include "evmcueqp.h"
#include "eventid.h"

//--------------------�������ڲ���Ϣ-------------------

//��������ʼ����Ϣ,��ϢΪTMixerCfg
OSPEVENT(EV_MIXER_INIT,                 EV_MIXER_BGN + 1 ); 
//�������ʼ����Ϣ,����Ϣ��
OSPEVENT(EV_MIXER_GRPINIT,              EV_MIXER_BGN + 2 );
//��ʱ������Ϣ,����Ϣ��
OSPEVENT(EV_MIXER_CONNECT,              EV_MIXER_BGN + 3 );
//��MCUע����Ϣ,����Ϣ��
OSPEVENT(EV_MIXER_REGISTER,             EV_MIXER_BGN + 4 );
//��MCUע����Ϣ�ȴ�Ӧ��ʱ��Ϣ,����Ϣ��
OSPEVENT(EV_MIXER_REGTIMEOUT,           EV_MIXER_BGN + 5 );
//�������Ա�仯֪ͨ��Ϣ
OSPEVENT(EV_MIXER_ACTIVECHANGE_NOTIF,   EV_MIXER_BGN + 6 );
//�������˳���Ϣ
OSPEVENT(EV_MIXER_EXIT,                 EV_MIXER_BGN + 7 );
//��ʾ������״̬�ײ㺯����Ϣ
OSPEVENT(EV_MIXER_DEVICESTATUSSHOW,     EV_MIXER_BGN + 8 );
//��ʾ������״̬
OSPEVENT(EV_MIXER_SHOWGROUP,            EV_MIXER_BGN + 9 );
//��ʾ������Ա
OSPEVENT(EV_MIXER_SHOWMEMBER,           EV_MIXER_BGN + 10 );
//����Mcu.B
OSPEVENT(EV_MIXER_CONNECTB,             EV_MIXER_BGN + 11 );
//��Mcu.Bע��
OSPEVENT(EV_MIXER_REGISTERB,            EV_MIXER_BGN + 12 );
//��Mcuȡ��������״̬
OSPEVENT(EV_MIXER_MCU_GETMSSTATUS,      EV_MIXER_BGN + 13 );
//��ʾ��������ǰ�ն˺ͻ���ͨ��ӳ���
OSPEVENT(EV_MIXER_SHOWMT2CHNNL,         EV_MIXER_BGN + 14 );


//--------------------������������Ϣ-------------------

OSPEVENT(EV_MIXER_TEST_GETMSG,          EV_TEST_MIXER_BGN + 1 );
//��ȡ����״̬
OSPEVENT(EV_MIXER_TEST_GETEQPSTATUS,    EV_TEST_MIXER_BGN + 2 );
//ǿ��MCU�����������
OSPEVENT(EV_MIXER_TEST_DISCONNECTMIXER, EV_TEST_MIXER_BGN + 3 );

#endif//!_EVENT_MCU_MIXER_H_
