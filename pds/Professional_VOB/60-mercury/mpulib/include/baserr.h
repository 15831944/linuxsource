/*****************************************************************************
   ģ����      : mpulib
   �ļ���      : baserr.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: mpulib�����붨��
   ����        : ����
   �汾        : V4.6  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��       �汾        �޸���      �޸�����
   2009/3/14    4.6         �ű���      ע��
******************************************************************************/
#ifndef _BAS_ERR_CODE_H_
#define _BAS_ERR_CODE_H_

#include "errorid.h"

#define ERR_BAS_CHNOTREAD           ( ERR_BAS_BGN + 1 ) //��ǰͨ�����ھ���״̬
#define ERR_BAS_CHRUNING            ( ERR_BAS_BGN + 2 ) //��ǰͨ���������� 
#define ERR_BAS_CHREADY             ( ERR_BAS_BGN + 3 ) //��ǰͨ���������� 
#define ERR_BAS_CHIDLE              ( ERR_BAS_BGN + 4 ) //��ǰͨ��Ϊ��     
#define ERR_BAS_OPMAP               ( ERR_BAS_BGN + 5 ) //���õײ㺯��ʧ��
#define ERR_BAS_ERRCONFID           ( ERR_BAS_BGN + 6 ) //����Ļ���
#define ERR_BAS_NOENOUGHCHLS        ( ERR_BAS_BGN + 7 ) //ͨ��������

#endif


//END OF FILE

