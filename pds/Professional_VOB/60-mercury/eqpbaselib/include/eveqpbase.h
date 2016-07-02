/*****************************************************************************
   ģ����      : eqpbase
   �ļ���      : eveqpbase.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ���������Ϣ����
   ����        : �ܼ���
   �汾        :  1.0
   ����		   :  2012/02/14
-----------------------------------------------------------------------------
******************************************************************************/

#ifndef _EQP_BASE_EVENT_H
#define _EQP_BASE_EVENT_H

#include "osp.h"
#include "eventid.h"

/********************************˵��************************************/
/* 1. Osp������Ϣ0x400
   2. ҵ����Ϣ��ʼ 10000
   3. eqpbase��9900��ʼ*/
#define EV_EQPBASE_BGN        (u16)9900
/************************************************************************/
/*				    	������mcu֮����Ϣ����                           */
/************************************************************************/
//������mcu����ע��
OSPEVENT( EQP_MCU_REG_REQ,					EV_EQPBASE_BGN + 1 );
//����ע��ɹ�ʱmcu�Ļ�Ӧ
OSPEVENT( MCU_EQP_REG_ACK,					EV_EQPBASE_BGN + 2 );
//����ע��ʧ��ʱ, mcu�Ļ�Ӧ
OSPEVENT( MCU_EQP_REG_NACK,					EV_EQPBASE_BGN + 3 );

//��mcu��������״̬
OSPEVENT( EQP_MCU_GETMSSTAT_REQ,			EV_EQPBASE_BGN + 4 );
//����״̬��ȡ�ɹ�
OSPEVENT( MCU_EQP_GETMSSTAT_ACK,			EV_EQPBASE_BGN + 5 );
//��������״̬ʧ��
OSPEVENT( MCU_EQP_GETMSSTAT_NACK,			EV_EQPBASE_BGN + 6 );
// ����qos����Ϣ�壺TMcuQosCfgInfo
OSPEVENT( MCU_EQP_QOS_CMD,					EV_EQPBASE_BGN + 7 );
//�޸ķ��͵�ַ
OSPEVENT(MCU_EQP_MODIFYADDR_CMD,			EV_EQPBASE_BGN + 8 );

/************************************************************************/
/*                      �����ڲ���ͨ����Ϣ����				            */
/************************************************************************/
//����MCU��Ϣ
OSPEVENT( EV_EQP_CONNECT_TIMER,				EV_EQPBASE_BGN + 51 );
//����MCUB��Ϣ
OSPEVENT( EV_EQP_CONNECTB_TIMER,			EV_EQPBASE_BGN + 52 );
//���趨ʱע��Mcu
OSPEVENT( EV_EQP_REGISTER_TIMER,			EV_EQPBASE_BGN + 53 );
//���趨ʱע��McuB
OSPEVENT( EV_EQP_REGISTERB_TIMER,			EV_EQPBASE_BGN + 54 );
//���趨ʱ��ȡ����״̬
OSPEVENT( EV_EQP_GETMSSTATUS_TIMER,			EV_EQPBASE_BGN + 55 );


/************************************************************************/
/*				     	�������������Է�������Ϣ����                    */
/************************************************************************/
//���趨ʱע���������Է�����
OSPEVENT( EQP_SERVER_REGISTER_REQ,		    EV_EQPBASE_BGN + 70 );
OSPEVENT( SERVER_EQP_REGISTER_ACK,		    EV_EQPBASE_BGN + 71 );
OSPEVENT( SERVER_EQP_REGISTER_NACK,		    EV_EQPBASE_BGN + 72 );

/************************************************************************/
/*					       �������������ڲ���Ϣ                         */
/************************************************************************/
//�����������趨ʱ����
OSPEVENT( EV_EQPSERVER_CONNECT_TIMER,		EV_EQPBASE_BGN + 80 );
//�����������趨ʱע��
OSPEVENT( EV_EQPSERVER_REGISTER_TIMER,		EV_EQPBASE_BGN + 81 );

#endif