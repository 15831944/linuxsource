/*****************************************************************************
   ģ����      : ������뿨
   �ļ���      : errhdu.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: HDU����Ŷ���
   ����        : ���ֱ�
   �汾        : V1.0  Copyright(C) 2008-2011 KDC, All rights reserved.
 -----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2008/12/18     		   ���ֱ�		����
******************************************************************************/
#ifndef _HDU_ERROR_CODE_H_
#define _HDU_ERROR_CODE_H_

#include "errorid.h"

#define ERR_HDU_INVALID_CHANNL	    ( ERR_HDU_BGN + 1 )  //��Ч��ͨ����
#define ERR_HDU_INVALID_MEDIATYPE   ( ERR_HDU_BGN + 2 )  //��Ч��ý������(���뺬��Ƶ)
#define ERR_HDU_STARTPLAY_FAIL      ( ERR_HDU_BGN + 3 )  //��ʼ����ʧ��
#define ERR_HDU_STOPPLAY_FAIL       ( ERR_HDU_BGN + 4 )  //ֹͣ����ʧ��
#endif
