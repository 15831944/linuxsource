 /*****************************************************************************
   ģ����      : mpuhardware (vmp)auto test client 
   �ļ���      : mpuvmphwtest_evid.h
   ����ļ�    : mpuvmphwtest.cpp
   �ļ�ʵ�ֹ���: MPU����ϳ��Զ�������
   ����        : 
   �汾        : V4.6.2  Copyright(C) 2009-2015 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2009/05/27  1.0         Ѧ��        ���� 
******************************************************************************/
#ifndef MPUHARDWARETEST_EV_H
#define MPUHARDWARETEST_EV_H

/*MPU�Զ������Ϣ��57741��57800��*/ 
#define EV_C_BASE             57741

#define EV_C_CONNECT_TIMER                 EV_C_BASE + 1    // ����
#define EV_C_REGISTER_TIMER				   EV_C_BASE + 2
#define EV_C_INIT                          EV_C_BASE + 3    // ��ʼ��

				   

#define EV_C_CHANGEAUTOTEST_CMD            EV_C_BASE + 6    // �ı��Զ����Ա�־

#define EV_C_BASCHL_INIT					EV_C_BASE + 10	// bas��ͨ����ʼ�� 

#endif
