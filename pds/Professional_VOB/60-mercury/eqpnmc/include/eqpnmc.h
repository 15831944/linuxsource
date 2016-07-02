/*****************************************************************************
    ģ����      : EqpNmc
    �ļ���      : eqpnmc.h
    ����ļ�    : 
    �ļ�ʵ�ֹ���: �������ܿͻ���
    ����        : liaokang
    �汾        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    �޸ļ�¼:
    ��  ��      �汾        �޸���          �޸�����
    2012/06/18  4.7         liaokang        ����
******************************************************************************/
#ifndef EQPNMC_H_
#define EQPNMC_H_
#include "eqpagtcommon.h"

// kdvlog config file
#if defined( WIN32 ) || defined(_WIN32)
#define EQPNMC_KDVLOG_FILE	    ( LPCSTR )"./conf/kdvlog_eqpnmc.ini"
#else
#define EQPNMC_KDVLOG_FILE	    ( LPCSTR )"/usr/etc/config/conf/kdvlog_eqpnmc.ini"
#endif

// EQP
#define EQP_TELNET_PORT         2500

// EQP����Ӧ��OSP��ʼ��Ȩ����
#define EQP_TEL_USRNAME         ( LPCSTR )"admin"
#define EQP_TEL_PWD             ( LPCSTR )"admin"

#define AUTORUN_FLAG            ( LPCSTR )" -autorun"

#define WM_TRAY_MESSAGE         (WM_USER + 1)

API BOOL32 IsAutoRun(void);
API BOOL32 AutoRun(void);
API void CancelAutoRun(void);
API void quit(void);

#endif  // _EQPNMC_H_