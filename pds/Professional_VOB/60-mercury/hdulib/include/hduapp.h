/*****************************************************************************
ģ����      : ������뿨
�ļ���      : hduapp.h
����ļ�    : 
�ļ�ʵ�ֹ���: �໭����ʾʵ��ͷ�ļ�
����        : ���ֱ�
�汾        : V1.0  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
08/11/26    4.6         ���ֱ�        ����
******************************************************************************/
#ifndef _HDU_APP_H_
#define _HDU_APP_H_

#ifndef __BITSTREAM_ADAPT_SERVER_APP_H__
#define __BITSTREAM_ADAPT_SERVER_APP_H__

#ifdef _LINUX_
#include "nipwrapper.h"
#endif

API void sethdulog(int nlvl);    //���ô�ӡ����
API void hduver(void);           //��ʾ�汾��
API void hduhelp(void);          //��ʾ������Ϣ
API void hduscreen(void);        //��ӡ��Ļ��Ϣ
API void hdushow(void);          //HDU״̬��ʾ
API	void hduinfo(void);          // ��ʾ������Ϣ
API void sethdumode(u8 byNum, u8 byPort, u8 byMode, u8 byPlayScale);  
API void showmode(void); 
API void hdutau( LPCSTR lpszUsrName = NULL, LPCSTR lpszPwd = NULL );    // telnet��Ȩ
API void setautotest( s32 nIsAutoTest );  //�����Ƿ�Ӳ���Զ����
void HduAPIEnableInLinux(void);  //ע��linux���Խӿ�
BOOL hduInit(THduCfg* ptCfg);    //HDU��ʼ��

#endif//!__BITSTREAM_ADAPT_SERVER_APP_H__

#endif//_HDU_APP_H
