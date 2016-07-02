/*****************************************************************************
   ģ����      : ������
   �ļ���      : errmixinst.h
   ����ʱ��    : 2003�� 12�� 9��
   ʵ�ֹ���    : 
   ����        : ������
   �汾        : 
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   03/12/09    3.0         zmy         ����
******************************************************************************/
#ifndef _KDV_MIXER_ERROR_CODE_H_
#define _KDV_MIXER_ERROR_CODE_H_
#include "errorid.h"

#define ERR_MIXER_NONE               ( ERR_MIXER_BGN  )     //!�޴��� 
#define ERR_MIXER_GRPNOTEXIST        ( ERR_MIXER_BGN + 1 )  //!ָ���Ļ����鲻����
#define ERR_MIXER_MMBNOTEXIST        ( ERR_MIXER_BGN + 2 )  //!ָ���Ļ�����Ա������
#define ERR_MIXER_NOFREEGRP          ( ERR_MIXER_BGN + 3 )  //!û�п��еĻ�����
#define ERR_MIXER_NOFREEMMB          ( ERR_MIXER_BGN + 4 )  //!û�п��еĻ���ͨ��
#define ERR_MIXER_ILLEGALBODY        ( ERR_MIXER_BGN + 5 )  //!��Ϣ�е���Ϣ��Ƿ�
#define ERR_MIXER_CALLDEVICE         ( ERR_MIXER_BGN + 6 )  //!���õײ㺯��ʧ��
#define ERR_MIXER_MIXING             ( ERR_MIXER_BGN + 7 )  //!���ڻ���
#define ERR_MIXER_NOTMIXING          ( ERR_MIXER_BGN + 8 )  //!δ����
#define ERR_MIXER_ERRCONFID          ( ERR_MIXER_BGN + 9 )  //!��Ч�Ļ���ID
#define ERR_MIXER_GRPINVALID         ( ERR_MIXER_BGN + 10 ) //��Ч�Ļ�����ID

#endif/*_KDV_MIXER_ERROR_CODE_H_*/
