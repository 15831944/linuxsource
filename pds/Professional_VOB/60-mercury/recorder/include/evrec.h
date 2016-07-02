/*****************************************************************************
   ģ����      : ¼���
   �ļ���      : EvRec.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ¼����ӿ���Ϣ����
				 *_REQ: ��Ӧ������
				 *_ACK, *_NACK: ����֮��
				 *_CMD: ����Ӧ������
				 *_NOTIF: ״̬֪ͨ
   ����        : ������
   �汾        : V1.0  Copyright(C) 2001-2003 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/07/04  3.0         ������      ������¼�����ļ��ı���
******************************************************************************/
#ifndef EVREC_H
#define EVREC_H

#include "eventid.h"

//-------------     ¼����ڲ���Ϣ     --------------

//¼�����ʼ����Ϣ��
OSPEVENT( EV_REC_INIT,				EV_REC_BGN  + 1 );
//¼�����ͨ����ʼ����Ϣ
OSPEVENT( EV_REC_CHNINIT,			EV_REC_BGN  + 2 );
//¼�����ʱ������Ϣ
OSPEVENT( EV_REC_CONNECTTIMER,		EV_REC_BGN  + 3 );
//��ʱע����Ϣ
OSPEVENT( EV_REC_REGTIMER,			EV_REC_BGN  + 4 );
//¼����豸֪ͨ
OSPEVENT( EV_REC_DEVNOTIF,			EV_REC_BGN  + 5 );
//�ȴ�ע��Ӧ����Ϣ
OSPEVENT( EV_REC_REGWAIT,			EV_REC_BGN  + 6 );
//ǿ��ֹͣ��ͨ���豸
OSPEVENT( EV_REC_STOPDEVICE,		EV_REC_BGN  + 7 );
//¼��ͨ������֪ͨ
OSPEVENT( EV_REC_RECPROGTIME,		EV_REC_BGN  + 8 );
//��ʾ¼��״̬��Ϣ
OSPEVENT( EV_REC_STATUSSHOW,		EV_REC_BGN  + 9 );
//¼����˳�
OSPEVENT( EV_REC_QUIT,				EV_REC_BGN + 10 );
//¼�����ʱ״̬�ϱ���Ϣ
OSPEVENT( EV_REC_STATUSNOTYTIME,	EV_REC_BGN + 11 );
//ά��¼��������б�
//OSPEVENT( EV_REC_PLAYLIST_MAINTAIN,	EV_REC_BGN + 12 );
//���̿ռ�澯
//OSPEVENT( EV_REC_DISKNOTIFY,		EV_REC_BGN + 13 );
//¼����ϵ�
OSPEVENT( EV_REC_POWERON,			EV_REC_BGN + 14 );
//�Ի���ȡ������Ϣ
OSPEVENT(EV_DLG_REC_GETCONF,		EV_REC_BGN + 15 );
//��Mcuȡ��������״̬
OSPEVENT(EV_REC_GETMSSTATUS_TIMER,	EV_REC_BGN + 16 );
//¼�������
OSPEVENT( EV_REC_RESTART,			EV_REC_BGN + 21 );
//¼�����ʱ������Ϣ
OSPEVENT( EV_REC_CONNECTTIMERB,		EV_REC_BGN + 22 );
//��ʱע����Ϣ
OSPEVENT( EV_REC_REGTIMERB,			EV_REC_BGN + 23 );
//֪ͨ��Ӧ��¼��ͨ�����ı䵱ǰ¼���ļ���
OSPEVENT( EV_REC_REFRESH_FILENAME,  EV_REC_BGN + 24 );
//��ʼ����ȴ���������֪ͨ��ʱ��
OSPEVENT( EV_REC_SWITCHSTART_WAITTIMER, EV_REC_BGN + 25 );
//��MCU����ؼ�֡
OSPEVENT( EV_REC_MCU_NEEDIFRAME_TIMER, EV_REC_BGN + 26 );
//���������㶨ʱ��
OSPEVENT( EV_REC_PUBLISHPOINT_TIMER, EV_REC_BGN + 27 );

//--------------     ����ʹ����Ϣ     -------------------

//���Ӧ����Ϣ
OSPEVENT(REC_TEST_GET_RECACKEVENT,	EV_TEST_REC_BGN + 1 );
//��ȡ¼�������״̬
OSPEVENT( REC_TEST_GETEQPSTATUS,	EV_TEST_REC_BGN + 3 );

// ������Ϣ
#define WM_REC_RESTART              (WM_USER+0x101)     // ��������
#define WM_SHOWTASK				    (WM_USER+0x102)     // ��������Ϣ
#define WM_OPENDATABASE			    (WM_USER+0x103)		// �����ݿ�
#define OPEN_DATABASE_TIMER         (WM_USER+0x104)	    // �����ݿⶨʱ��
#define REFRSH_CHNL_TIMER           (WM_USER+0x105)	    // ˢ�½���
#define WM_QUITREC					(WM_USER+0x106)		// �˳�REC
#endif /*!EVREC_H*/
