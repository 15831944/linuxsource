/*****************************************************************************
   ģ����      : ����������
   �ļ���      : basinst.h
   ����ʱ��    : 2003�� 6�� 25��
   ʵ�ֹ���    : ������ͷ�ļ�
   ����        : ������
   �汾        : 
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
******************************************************************************/
#ifndef _BAS_ERR_CODE_H_
#define _BAS_ERR_CODE_H_

#include "errorid.h"

#define ERR_BAS_CHNOTREAD     ( ERR_BAS_BGN + 1 ) //��ǰͨ�����ھ���״̬
#define ERR_BAS_CHRUNING      ( ERR_BAS_BGN + 2 ) //��ǰͨ���������� 
#define ERR_BAS_CHREADY       ( ERR_BAS_BGN + 3 ) //��ǰͨ���������� 
#define ERR_BAS_CHIDLE        ( ERR_BAS_BGN + 4 ) //��ǰͨ��Ϊ��     
#define ERR_BAS_OPMAP         ( ERR_BAS_BGN + 5 ) //���õײ㺯��ʧ��
#define ERR_BAS_ERRCONFID     ( ERR_BAS_BGN + 6 ) //����Ļ���
#define ERR_BAS_NOENOUGHCHLS  ( ERR_BAS_BGN + 7 ) //ͨ��������

#endif
