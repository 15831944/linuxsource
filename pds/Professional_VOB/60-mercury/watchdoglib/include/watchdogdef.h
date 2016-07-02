/*****************************************************************************
   ģ����      : WatchDog ��ꡢ��������
   �ļ���      : bindwatchdog.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ����WatchDog���������ע������״̬��

   ����        : ����
   �汾        : V4.0  Copyright(C) 2003-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2005/05/26  4.0         ����      ����
******************************************************************************/

#ifndef _WATCHDOGCONSTDEF_H_
#define _WATCHDOGCONSTDEF_H_

#include "dsccomm.h"

// ��ʱ������
#define TIMER_REG_POWERON   500
#define TIMER_REG_DISC      5000

// ��ʱ���¼�

// ���ӷ�����
OSPEVENT(EV_CONNECTWDSERV_TIMER,       (EV_DSCCONSOLE_BGN +90) );   

// �������ע��
OSPEVENT(EV_REGWDSERV_TIMER,           (EV_DSCCONSOLE_BGN +91) );   


#endif
