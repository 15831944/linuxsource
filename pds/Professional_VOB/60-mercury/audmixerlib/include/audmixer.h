/*****************************************************************************
   ģ����      : ������
   �ļ���      : audiomixer.h
   ����ʱ��    : 2003�� 12�� 9��
   ʵ�ֹ���    : 
   ����        : ������
   �汾        : 
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   03/12/09    3.0         zmy         ����
******************************************************************************/
#ifndef _KDV_MCU_MIXER_H_
#define _KDV_MCU_MIXER_H_
#include "osp.h"
#include "evmixer.h"

BOOL32 UserInit();

API void mixerver();
API void amixSetDebugScreen();
API void amixSetReleaseScreen();
API void amixSetDebugFile();
API void amixSetReleaseFile();

#endif //!_KDV_MCU_MIXER_H_
