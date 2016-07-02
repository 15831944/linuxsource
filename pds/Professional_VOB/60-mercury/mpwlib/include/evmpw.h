/*****************************************************************************
   ģ����      : MpwLib�໭�渴�ϵ���ǽ
   �ļ���      : evmpw.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ����mpw���ڲ��¼���
   ����        : john
   �汾        : V4.0  Copyright(C) 2001-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2005/09/25  1.0         john         ����
******************************************************************************/
#ifndef _VIDEO_MPWEVENT_H_
#define _VIDEO_MPWEVENT_H_

#include "osp.h"

#define DOUBLE_VIDEO                (u8)2       // ˫·����
#define FIRST_REGACK                (u8)1       // ��һ���յ�ע��Ack
#define FIRST_CHANNEL               (u8)1       // һ·��Ƶ
#define SECOND_CHANNEL              (u8)2       // ��·��Ƶ

#define MPW_CONNECT_TIMEOUT         (u16)3*1000
#define MPW_REGISTER_TIMEOUT        (u16)5*1000
#define CHECK_IFRAME_INTERVAL       (u16)1000    // ����ؼ�֡ʱ����

//ȡ��������״̬
OSPEVENT( EV_MPW_GETMSSTATUS_TIMER,  EV_VMPTW_BGN + 1 );
//����MCU.A
OSPEVENT( EV_MPW_CONNECT_TIMER,      EV_VMPTW_BGN + 2 );  
//����MCU.B
OSPEVENT( EV_MPW_CONNECT_TIMERB,     EV_VMPTW_BGN + 3 ); 
//ע��MCU.A
OSPEVENT( EV_MPW_REGISTER_TIMER,     EV_VMPTW_BGN + 4 ); 
//ע��MCU.B
OSPEVENT( EV_MPW_REGISTER_TIMERB,    EV_VMPTW_BGN + 5 ); 
//����ؼ�֡��ʱ��
OSPEVENT( EV_MPW_NEEDIFRAME_TIMER,   EV_VMPTW_BGN + 6 ); 
#endif
