/*****************************************************************************
ģ����      : hduhardwate auto test client
�ļ���      : hduhwautotest_evid.h
����ļ�    : hduhwautotest.cpp
�ļ�ʵ�ֹ���: HDUӲ���Զ�������
����        : 
�汾        : V4.6.2  Copyright(C) 2009-2015 KDC, All rights reserved.
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
2009/04/21  1.0         ���ֱ�        ���� 
******************************************************************************/
#ifndef HDUHWAUTOTEST_EV_H
#define HDUHWAUTOTEST_EV_H

/*hdu�Զ������Ϣ��57641��57700��*/ 
#define EV_C_BASE             57641

#define EV_C_STARTSWITCH_CMD               EV_C_BASE + 1    // ��ʼhdu����
#define EV_C_STOPSWITCH_CMD                EV_C_BASE + 2    // ֹͣhdu����

#define EV_C_CONNECT_TIMER                 EV_C_BASE + 3    // ����             
#define EV_C_INIT                          EV_C_BASE + 4    // ��ʼ��

#define EV_C_INITCHL                       EV_C_BASE + 5    // ��ʼ��ͨ��
#define EV_C_CHANGEAUTOTEST_CMD            EV_C_BASE + 6    // �ı��Զ����Ա�־

#endif
