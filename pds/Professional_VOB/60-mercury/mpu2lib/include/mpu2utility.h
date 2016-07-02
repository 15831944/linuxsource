/*****************************************************************************
   ģ����      : mpu2lib
   �ļ���      : mpuu2tility.h
   ����ļ�    : mpuu2tility.cpp
   �ļ�ʵ�ֹ���: ����/ͨ�� ��/���� ����
   ����        : ��־��
   �汾        : V4.7  Copyright(C) 2012-2015 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��       �汾        �޸���      �޸�����
   2012/07/26  4.7         ��־��       ����
******************************************************************************/
#ifndef _MPU_UTILITY_H_
#define _MPU_UTILITY_H_

#include "osp.h"
#include "kdvtype.h"
#include "kdvmedianet.h"
#include "mcustruct.h"

//��ӡ������
#define MPU_CRIT                (u8)0       //�ؼ���/����/�쳣
#define MPU_WARN                (u8)1       //����
#define MPU_INFO                (u8)2       //����
#define MPU_DETAIL              (u8)3       //����ϸ��/��ϸ���Ե�




void mpulog( u8 byLevel, s8* pszFmt, ... );
void mpulogall( s8* pszFmt, ... );
s8 * ipStr( u32 dwIP );	
s8 * StrOfIP( u32 dwIP );

//��װkdvlog��4���ȼ�
void ErrorLog(s8* pszFmt,...);
void KeyLog(s8* pszFmt, ...);
void WarningLog(s8* pszFmt,...);
void DetailLog(s8* pszFmt,...);

u16  AdjustPrsMaxSpan(u8 byVmpStyle);
void GetRSParamByPos(u8 byPos, TRSParam &tRSParam);
u8   ConvertVmpStyle2ChnnlNum(u8 byVmpStyle);
void ConvertToRealFR(THDAdaptParam& tAdptParm);
void ConverToRealParam(u8 byBasIdx, u8 byChnId,u8 byOutIdx,THDAdaptParam& tBasParam);
u8 ConvertVcStyle2HardStyle( u8 byOldstyle );


#endif //!_MPU_UTILITY_H_

//END OF FILE

