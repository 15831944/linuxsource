/*****************************************************************************
   ģ����      : mpulib
   �ļ���      : mpuutility.h
   ����ļ�    : mpuutility.cpp
   �ļ�ʵ�ֹ���: ����/ͨ�� ��/���� ����
   ����        : zhangbq
   �汾        : V4.5  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��       �汾        �޸���      �޸�����
   2009/3/14    4.6         �ű���      ����
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

u16  AdjustPrsMaxSpan(u8 byVmpStyle);
void GetRSParamByPos(u8 byPos, TRSParam &tRSParam);
u8   ConvertVmpStyle2ChnnlNum(u8 byVmpStyle);
void ConvertToRealFR(THDAdaptParam& tAdptParm);
void ConverToRealParam(u8 byChnId, u8 byOutIdx, THDAdaptParam& tBasParam);



#endif //!_MPU_UTILITY_H_

//END OF FILE

