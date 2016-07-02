/*****************************************************************************
   ģ����      : KDVNMS
   �ļ���      : smevent.h
   ����ļ�    : smevent.cpp
   �ļ�ʵ�ֹ���: ��������Event����
   ����		   : Ѧ����
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾     �޸���      �޸�����
   2010/01/01  5.0      ½����      ����
******************************************************************************/
#ifndef SM_SMEVENT_H
#define SM_SMEVENT_H

#include "osp.h"
#include "eventid.h"

#define EV_SYS_BGN EV_SVR_BGN

#define EV_SM_BGN EV_SVR_BGN + 100

#define EV_CM_BGN EV_SVR_BGN + 200

#define EV_FM_BGN EV_SVR_BGN + 300

#define EV_CB_BGN EV_SVR_BGN + 400

#define EV_NP_BGN EV_SVR_BGN + 500

//System��Ϣ����
OSPEVENT( EV_SYS_SM_CMD,						EV_SYS_BGN + 0 );
OSPEVENT( EV_SYS_CM_CMD,						EV_SYS_BGN + 1 );
OSPEVENT( EV_SYS_FM_CMD,						EV_SYS_BGN + 2 );
OSPEVENT( EV_SYS_SM_START_CMD,					EV_SYS_BGN + 3 );

////////////////////////////////////////////////////////////////////
//SessionManager ��Ϣ����
//����������ȡ����Ϣ��ת����CM
//�������豸�����ϱ�����Ϣ����FM���������ݿ�������Ҳ��FM

//������Ϣ
OSPEVENT( EV_SM_CONNECT,						EV_SM_BGN + 0 );
OSPEVENT( EV_SM_REGISTER,						EV_SM_BGN + 1 );
//snmpmanaer��nms������ע��Timer��Ϣ
OSPEVENT( EV_SM_CONNECT_REGISTER,				EV_SM_BGN + 2 );

//SM������CMͨ����Ϣ
OSPEVENT( EV_SM_CM_CMD,							EV_SM_BGN + 10 );
OSPEVENT( EV_SM_CM_START_CMD,					EV_SM_CM_CMD + 1 );
OSPEVENT( EV_SM_CM_MERGEDEVICE_NTY,				EV_SM_CM_CMD + 2 );
//�豸��Ϣ���ɾ�����޸���Ϣ
OSPEVENT( EV_SM_CM_DEVICE_OPT_CMD,				EV_SM_CM_CMD + 3 );
//��Ҫ��֪REQ��ACK����MSG TYPE����Ϣ����
OSPEVENT( EV_SM_CM_DEVICE_CFG_CMD,				EV_SM_CM_CMD + 4 );
OSPEVENT( EV_SM_CM_DEVICE_PFM_CMD,				EV_SM_CM_CMD + 5 );
OSPEVENT( EV_SM_CM_DEVICE_RESET_CMD,			EV_SM_CM_CMD + 6 );
OSPEVENT( EV_SM_CM_DEVICE_SELFUPDATE_CMD,		EV_SM_CM_CMD + 7 );
OSPEVENT( EV_SM_CM_DEVICE_BATCH_CFG_CMD,		EV_SM_CM_CMD + 8 );

//SM������FMͨ����Ϣ
OSPEVENT( EV_SM_FM_CMD,							EV_SM_BGN + 50 );
OSPEVENT( EV_SM_FM_PFMSTATISTIC_CMD,			EV_SM_FM_CMD + 1 );
//SessionManager ��Ϣ���� End
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//CM��Ϣ����
//������Ϣ
OSPEVENT( EV_CM_START_POLL_CMD,					EV_CM_BGN + 0 );
OSPEVENT( EV_CM_TIMER_FOR_POLL_CMD,				EV_CM_BGN + 1 );
OSPEVENT( EV_CM_TIMER_FOR_DEVICEINFO_CMD,		EV_CM_BGN + 2 );
OSPEVENT( EV_CM_TIMER_FOR_QUERY_UPDATE_CMD,		EV_CM_BGN + 3 );

//CM������SMͨ����Ϣ
OSPEVENT( EV_CM_SM_CMD,							EV_CM_BGN + 10 );
OSPEVENT( EV_CM_SM_DEVICE_ONLINE_CMD,			EV_CM_SM_CMD + 1 );
OSPEVENT( EV_CM_SM_DEVICE_OFFLINE_CMD,			EV_CM_SM_CMD + 2 );
OSPEVENT( EV_CM_SM_CHANGEIP_NOTIFY,				EV_CM_SM_CMD + 3 );
OSPEVENT( EV_CM_SM_OPER_FAIL,					EV_CM_SM_CMD + 4 );

//CM������FMͨ����Ϣ
OSPEVENT( EV_CM_FM_CMD,							EV_CM_BGN + 1 );
//���Ը澯
OSPEVENT( EV_CM_FM_UNIT_ALARM_NOTIFY,			EV_CM_FM_CMD + 1 );
//CM��Ϣ���� End
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//FM��Ϣ����
//FM�����Լ�����Ϣ

//FM������SMͨ����Ϣ
OSPEVENT( EV_FM_SM_CMD,							EV_FM_BGN + 10 );
OSPEVENT( EV_FM_SM_ALARM_NOTIFY,				EV_FM_SM_CMD + 1 );

//FM������CMͨ����Ϣ
OSPEVENT( EV_FM_CM_CMD,							EV_FM_BGN + 40 );
//FM��Ϣ���� End
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//CB��Ϣ����

//CB������CMͨ����Ϣ
OSPEVENT( EV_CB_CM_CMD,							EV_CB_BGN + 0 );
OSPEVENT( EV_CB_CM_BOARDSTATUS_NTF,				EV_CB_CM_CMD + 1 );
OSPEVENT( EV_CB_CM_SNMPREVFAIL_NTF,				EV_CB_CM_CMD + 2 );
OSPEVENT( EV_CB_CM_GETPOLLINFO_NTF,				EV_CB_CM_CMD + 3 );
OSPEVENT( EV_CB_CM_GETDEVICEINFO_NTF,			EV_CB_CM_CMD + 4 );
OSPEVENT( EV_CB_CM_DEVICECFG_NTF,				EV_CB_CM_CMD + 5 );
OSPEVENT( EV_CB_CM_GETDEVICEPFM_NTF,			EV_CB_CM_CMD + 6 );
OSPEVENT( EV_CB_CM_ADDBOARD_NTF,				EV_CB_CM_CMD + 7 );
OSPEVENT( EV_CB_CM_GETBOARD_NTF,				EV_CB_CM_CMD + 8 );
OSPEVENT( EV_CB_CM_GETBOARDCOMPLETED_NTF,		EV_CB_CM_CMD + 9 );
OSPEVENT( EV_CB_CM_CHGIP_NTF,					EV_CB_CM_CMD + 10 );
OSPEVENT( EV_CB_CM_DEVICELINK_NTF,				EV_CB_CM_CMD + 11 );
OSPEVENT( EV_CB_CM_DEVOPER_NTF,				    EV_CB_CM_CMD + 12 );
OSPEVENT( EV_CB_CM_QUERYUPDATE_NTF,				EV_CB_CM_CMD + 13 );

//CB������FMͨ����Ϣ
OSPEVENT( EV_CB_FM_CMD,							EV_CB_BGN + 40 );
OSPEVENT( EV_CB_FM_ALARM_NOTIFY,				EV_CB_FM_CMD + 1 );
OSPEVENT( EV_CB_FM_PERFORMANCE_NOTIFY,			EV_CB_FM_CMD + 2 );

//CB������SMͨ����Ϣ (����Nack��Ϣ����cb��ȡʧ�ܺ�ֱ�ӷ���sm)
OSPEVENT( EV_CB_SM_CMD,							EV_CB_BGN + 80 );
OSPEVENT( EV_CB_SM_OPER_FAIL,					EV_CB_SM_CMD + 1 );

//CB��Ϣ���� End
////////////////////////////////////////////////////////////////////


//NP�����Ϣ����

//����Ϣ��ʱ������
//��ʾ����NP���͸�SM����Ϣ�ܳ�
//NP��������ΪSUBEV_TYPE_REQ
//SM�ظ�����ΪSUBEV_TYPE_ACK/SUBEV_TYPE_NACK/SUBEV_TYPE_NOTIFY/SUBEV_TYPE_FINISH
OSPEVENT( EV_NP_SM_CMD,							EV_NP_BGN + 0 );

//����ϢΪNP��SM���͵����������Ϣ
//NP��������ΪSUBEV_TYPE_REQ
//SM�ظ�����ΪSUBEV_TYPE_ACK/SUBEV_TYPE_NACK
//NP���͵�MsgTypeΪTNM_MSG_GET(��ȡ)/TNM_MSG_SET(����)
//SM�ظ���Ϣ��ΪCBoardCfgInfo/CMtCfgInfo��Ӧ��GetBuffer
//NP�����ʱ��ֱ�ӵ���CCfgInfoBase/CMtCfgInfo(const s8* pszBuffer, u32 dwSize)����
OSPEVENT( EV_NP_SM_DEVICE_CFG,					EV_NP_BGN + 1 );

//����ϢΪNP��SM���͵Ļ�ȡ��ǰ���������Ϣ
//NP��������ΪSUBEV_TYPE_REQ
//SM�ظ�����ΪSUBEV_TYPE_ACK/SUBEV_TYPE_NACK
//NP���͵�MsgTypeΪTNM_MSG_GET(��ȡ)
//SM�ظ���Ϣ��ΪCDevPfmInfoBase��Ӧ��GetBuffer
//NP�����ʱ��ֱ�ӵ���CDevPfmInfoBase(const s8* pszBuffer, u32 dwSize)����
OSPEVENT( EV_NP_SM_DEVICE_PFM,					EV_NP_BGN + 2 );


//����ϢΪNP��SM���͵�����ͳ�������Ϣ
//NP��������ΪSUBEV_TYPE_REQ
//SM�ظ�����ΪSUBEV_TYPE_ACK(�ظ�)/SUBEV_TYPE_NACK(�ܾ�)/SUBEV_TYPE_NOTIFY(֪ͨ)/SUBEV_TYPE_FINISH()
//NP���͵�MsgTypeΪTNM_MSG_GET(��ȡ)/TNM_MSG_NTF(֪ͨ)
//���ڿ��ǵ����Ƚϴ�SM���������
//SM�ظ���Ϣ������
//SUBEV_TYPE_ACK��Ϣ�壺
//SUBEV_TYPE_NOTIFY��Ϣ�壺CDevPfmInfoNotify��Ӧ��GetBuffer(���ܷ���Σ�ÿ�����1000)
//SUBEV_TYPE_FINISH��Ϣ�壺u32 num(���ܵ�����)
OSPEVENT( EV_NP_SM_DEVICE_PFMSTATISTIC,			EV_NP_BGN + 3 );

//����ϢΪNP��SM���͵Ļ���ͼ�����Ϣ
//NP��������ΪSUBEV_TYPE_REQ
//SM�ظ�����ΪSUBEV_TYPE_ACK/SUBEV_TYPE_NACK
//NP���͵�MsgTypeΪTNM_MSG_GET(��ȡ)
//SM�ظ���Ϣ��ΪCMockMachineInfo��Ӧ��GetBuffer
//NP�����ʱ��ֱ�ӵ���CMockMachineInfo(const s8* pszBuffer, u32 dwSize)����
OSPEVENT( EV_NP_SM_DEVICE_MOCKMACHIN,			EV_NP_BGN + 4 );

//����ϢΪNP��SM���͵��������������Ϣ
//NP��������ΪSUBEV_TYPE_REQ
//SM�ظ�����ΪSUBEV_TYPE_ACK/SUBEV_TYPE_NACK
//NP���͵�MsgTypeΪTNM_MSG_GET(��ȡ)/TNM_MSG_SET(����)
//SM�ظ���Ϣ��ΪCBoardCfgInfo/CMtCfgInfo��Ӧ��GetBuffer
//NP�����ʱ��ֱ�ӵ���CCfgInfoBase/CMtCfgInfo(const s8* pszBuffer, u32 dwSize)����
OSPEVENT( EV_NP_SM_DEVICE_BATCH_CFG,			EV_NP_BGN + 5 );


//ϵͳ�еĶ�ʱ������
//For Session mamage
#define SM_TIMER_FOR_OPER				EV_SVR_BGN + 120

//ϵͳ��ʱ����ʱ����������Ϣ����
//For Session mamage
#define EV_SM_TIMER_FOR_OPER			EV_SVR_BGN + 120


#endif
