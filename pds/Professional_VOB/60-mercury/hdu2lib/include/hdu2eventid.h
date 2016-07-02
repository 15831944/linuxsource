/*****************************************************************************
ģ����      : ������뵥Ԫ
�ļ���      : hdu2_eventid.h
����ļ�    : 
�ļ�ʵ�ֹ���: hdu2ʵ��ͷ�ļ�
����        : ��־��
�汾        : 4.7  Copyright(C) 2011-2013 KDV, All rights reserved.
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
11/10/31    4.7         ��־��      ����
******************************************************************************/
#ifndef _EVENET_HDU2_H_
#define _EVENET_HDU2_H_

#include "osp.h"
#include "eventid.h"
/**********************************************************
*            HDU2�ڲ�ҵ����Ϣ��30501-30600��              *
***********************************************************/
//ͨ�������鴴��
OSPEVENT( EV_HDUCHNMGRGRP_CREATE,		   EV_HDU_BGN + 1 );
//����MCU��Ϣ
OSPEVENT( EV_HDU_CONNECT_TIMER,			   EV_HDU_BGN + 2 );
//����MCUB��Ϣ
OSPEVENT( EV_HDU_CONNECTB_TIMER,		   EV_HDU_BGN + 3 );
//��MCUע����Ϣ
OSPEVENT( EV_HDU_REGISTER_TIMER,		   EV_HDU_BGN + 4 );
//��MCUBע����Ϣ
OSPEVENT( EV_HDU_REGISTERB_TIMER,		   EV_HDU_BGN + 5 );
//��ʼ��ͨ������
OSPEVENT( EV_HDU_INITCHNCFG,	           EV_HDU_BGN + 6);
//HDU��MCU����ؼ�֡
OSPEVENT( EV_HDU_NEEDIFRAME_TIMER,	       EV_HDU_BGN + 7 );
//��Mcuȡ��������״̬��Mcuȡ��������״̬
OSPEVENT( EV_HDU_GETMSSTATUS_TIMER,	       EV_HDU_BGN + 8 );
//֪ͨͨ������ͨ������뼰��������(������������)
OSPEVENT( EV_C_STARTSWITCH_CMD,		       EV_HDU_BGN + 9 );
//֪ͨͨ��ֹͣͨ������뼰��������(������������)
OSPEVENT( EV_C_STOPSWITCH_CMD,		       EV_HDU_BGN + 10 );
//��ʾHDU���������Ϣ
OSPEVENT( EV_HDU_STATUSSHOW,		       EV_HDU_BGN + 11 );
//��ʾHDU��ǰ�����ģʽ
OSPEVENT( EVENT_HDU_SHOWMODWE,		       EV_HDU_BGN + 12 );
//���HDU��ǰģʽ
OSPEVENT( EVENT_HDU_CLEARMODWE,		       EV_HDU_BGN + 13 );

#endif

