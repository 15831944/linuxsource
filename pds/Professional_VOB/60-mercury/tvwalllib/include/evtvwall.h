/*****************************************************************************
   ģ����      : �໭����ʾ
   �ļ���      : evtvwall.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: �໭����ʾ�ڲ���Ϣ
   ����        : ����
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
  -----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/11/20     			   zhangsh		MCC
******************************************************************************/
#ifndef _EVENET_TVWALL_H_
#define _EVENET_TVWALL_H_

#include "osp.h"
#include "eventid.h"

/**********************************************************
 ����ǽ�ڲ�ҵ����Ϣ��30401-30500��
**********************************************************/

//�ϵ���Ϣ
OSPEVENT( EV_TVWALL_INIT,				EV_TVWALL_BGN + 1 );
//����MCU��Ϣ
OSPEVENT( EV_TVWALL_CONNECT,			EV_TVWALL_BGN + 2 );
//��MCUע����Ϣ
OSPEVENT( EV_TVWALL_REGISTER,			EV_TVWALL_BGN + 3 );
//ע�ᶨʱ��Ϣ
OSPEVENT( EV_TVWALL_REGTIMEOUT,			EV_TVWALL_BGN + 4 );
//��ʾ����ǽ����
OSPEVENT( EV_TVWALL_CONFIGSHOW,			EV_TVWALL_BGN +	5 );
//��ʾ����ǽ״̬
OSPEVENT( EV_TVWALL_STATUSSHOW,			EV_TVWALL_BGN +	6 );
//����MCU��Ϣ
OSPEVENT( EV_TVWALL_CONNECTB,			EV_TVWALL_BGN + 7 );
//��MCUע����Ϣ
OSPEVENT( EV_TVWALL_REGISTERB,			EV_TVWALL_BGN + 8 );
//��Mcuȡ��������״̬
OSPEVENT( EV_TVWALL_GETMSSTATUS_TIMER,	EV_TVWALL_BGN + 9 );

//...............................................................

//...............................................................

//���ɲ�����Ϣ
OSPEVENT( EV_TVWALL_INTEG_GETSTATE_CMD  ,	EV_TVWALL_BGN + 80 );

//��ȡָ��ͨ����Դ
OSPEVENT( EV_TVWALL_INTEG_GETCHNSRC_CMD ,	EV_TVWALL_BGN + 81 );

#endif // _EVENET_TVWALL_H_
