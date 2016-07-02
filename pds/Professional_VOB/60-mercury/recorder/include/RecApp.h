/*****************************************************************************
   ģ����      : Recorder
   �ļ���      : RecApp.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ¼���ģ����ڼ�һЩȫ�ֺ����Ķ���
   ����        : 
   �汾        : V3.5  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
******************************************************************************/
#ifndef	RECAPP_H
#define RECAPP_H

#include "osp.h"
#include "ConfAgent.h"
#include "RPCtrl.h"

API void UserInit();
API void RestartRecorder(HWND hMainDlg);

API void rechelp();
API void recSetDebugScreen();
API void recSetReleaseScreen();
//API void recSetDebugFile();
//API void recSetReleaseFile();
API void recQuit();

#endif  /* !RECAPP_H */
