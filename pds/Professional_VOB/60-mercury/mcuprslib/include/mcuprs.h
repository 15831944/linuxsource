/*****************************************************************************
ģ����      : prs
�ļ���      : mcuprs.h
����ļ�    : mcuprs.cpp
�ļ�ʵ�ֹ���: prsģ��ͳһͷ�ļ�
����        : �ܹ��
�汾        : V4.0  Copyright(C) 2001-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
2008/04/07  4.0         �ܹ��        ����
******************************************************************************/
#ifndef _MCUPRS_H_
#define _MCUPRS_H_

#include "eventid.h"

#define FILE_PRSLOG				( LPCSTR )"prs.log"

OSPEVENT( EV_MCUPRS_TIMER,               EV_PRS_BGN );                   // �ڲ���ʱ��
OSPEVENT( EV_CONNECT_TIMER,              EV_PRS_BGN + 1 );               // connect to MCU.A
OSPEVENT( EV_CONNECT_TIMERB,             EV_PRS_BGN + 2 );               // connect to MCU.B
OSPEVENT( EV_REGISTER_TIMER,             EV_PRS_BGN + 3 );               // register to MCU.A
OSPEVENT( EV_REGISTER_TIMERB,            EV_PRS_BGN + 4 );               // register to MCU.B
OSPEVENT( TIME_GET_MSSTATUS,             EV_PRS_BGN + 5 );               // ȡ��������״̬

OSPEVENT( EV_PRSGUARD_MAINTASKSCAN_TIMER,	 EV_PRS_BGN + 10 );			 // PRS GUARD̽�����̶߳�ʱ��
OSPEVENT( EV_MAINTASKSCAN_REQ,			 EV_PRS_BGN + 11 );				 // ̽������
OSPEVENT( EV_MAINTASKSCAN_ACK,			 EV_PRS_BGN + 12 );				 // ̽���Ӧ

#endif

