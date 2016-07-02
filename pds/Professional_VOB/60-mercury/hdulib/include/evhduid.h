/*****************************************************************************
ģ����      : ������뿨
�ļ���      : evhduid.h
����ļ�    : 
�ļ�ʵ�ֹ���: HDU�ڲ���Ϣ
����        : ���ֱ�
�汾        : 
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
2008/11/21              ���ֱ�	     ����		
******************************************************************************/
#ifndef _EVENET_HDU_H_
#define _EVENET_HDU_H_

#include "osp.h"
#include "eventid.h"


/**********************************************************
*            HDU�ڲ�ҵ����Ϣ��30501-30600��              *
***********************************************************/
//ͨ������������
OSPEVENT( EV_HDU_INIT,		        	   EV_HDU_BGN + 1 );
//ֹͣHDU
OSPEVENT( EV_HDU_QUIT,		        	   EV_HDU_BGN + 2 );
//����MCU��Ϣ
OSPEVENT( EV_HDU_CONNECT,			       EV_HDU_BGN + 3 );
//����MCUB��Ϣ
OSPEVENT( EV_HDU_CONNECTB,			       EV_HDU_BGN + 4 );
//��MCUע����Ϣ
OSPEVENT( EV_HDU_REGISTER,			       EV_HDU_BGN + 5 );
//��MCUBע����Ϣ
OSPEVENT( EV_HDU_REGISTERB,			       EV_HDU_BGN + 6 );
//��ʾHDU����
OSPEVENT( EV_HDU_CONFIGSHOW,			   EV_HDU_BGN + 8 );
//��ʾHDU״̬
OSPEVENT( EV_HDU_STATUSSHOW,		       EV_HDU_BGN + 9 );
//��ʾͨ��״̬
OSPEVENT( EV_HDU_SENDCHNSTATUS,		       EV_HDU_BGN + 10 );
//��Mcuȡ��������״̬
OSPEVENT( EV_HDU_GETMSSTATUS_TIMER,	       EV_HDU_BGN + 11 );
//HDU��MCU����ؼ�֡
OSPEVENT( EV_HDU_NEEDIFRAME_TIMER,	       EV_HDU_BGN + 12 );
//����HDU���������ʽ
OSPEVENT( EV_HDU_SETMODE,	               EV_HDU_BGN + 13 );
//��ʼ��ͨ����Ϣ
OSPEVENT( EV_HDU_INITCHNLINFO,	           EV_HDU_BGN + 14 );




#endif

