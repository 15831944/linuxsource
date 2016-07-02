/*****************************************************************************
   ģ����      : �·���ҵ��
   �ļ���      : evmcu.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: �·���ҵ��MCU�ڲ���Ϣ����
   ����        : ����
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2002/07/24  0.9         ����        ����
******************************************************************************/
#ifndef _EV_MCU_H_
#define _EV_MCU_H_

#include "osp.h"
#include "eventid.h"

/**********************************************************
 ����Ϊҵ��MCU�ڲ���Ϣ��28001-29000��
**********************************************************/

///////////////////////////////////////////////////////////
//ͬһMCU�ڲ���Ϣ

//MCU�����𴴽�����������Ϣ��Ϊ�ṹTConfFullInfo����ʾ��Ӧ������Ϣ
OSPEVENT( MCU_CREATECONF_FROMFILE,			EV_MCU_BGN + 1 );
//����MCU�ڲ�����ʵ��֪ͨĳ�¼�MT�Ǽǳɹ�����Ϣ��Ϊ�ṹTMt
//OSPEVENT( MCU_MTCONNECTED_NOTIF,			EV_MCU_BGN + 2 );
//����MCU�ڲ�����ʵ��֪ͨĳ�¼�MT��������Ϣ��Ϊ�ṹTMt
//OSPEVENT( MCU_MTDISCONNECTED_NOTIF,		EV_MCU_BGN + 3 );

//MCU ��N+1���ݴ�������
OSPEVENT( MCU_CREATECONF_NPLUS,             EV_MCU_BGN + 3 );

//MCU ֪ͨ�Ự ���� ��Ӧʵ��, zgc, 2007/04/29
OSPEVENT( MCU_DISCONNECT_CMD,		EV_MCU_BGN + 4 );
//MCU ֪ͨ�Ự ����MPCB ��Ӧʵ��, zgc, 2007/04/30
OSPEVENT( MCU_CONNECTMPCB_CMD,		EV_MCU_BGN + 5 );

//����MCU�ڲ�����ʵ��֪ͨ���轨���ɹ�����Ϣ��ΪTPeriEqpRegReq
OSPEVENT( MCU_EQPCONNECTED_NOTIF,			EV_MCU_BGN + 11 );
//����MCU�ڲ�����ʵ��֪ͨ�����������Ϣ��ΪTEqp
OSPEVENT( MCU_EQPDISCONNECTED_NOTIF,		EV_MCU_BGN + 12 );
//����MCU�ڲ�����ʵ��֪ͨDCS�����ɹ�, ��Ϣ��Ϊ CDcsMcuRegReqPdu
OSPEVENT( MCU_DCSCONNCETED_NOTIF,			EV_MCU_BGN + 13 );
//����MCU�ڲ�����ʵ��֪ͨDCS����, ��Ϣ��Ϊ CDcsMcuRegReqPdu
OSPEVENT( MCU_DCSDISCONNECTED_NOTIF,		EV_MCU_BGN + 14 );

//����MCU�ڲ�����ʵ��֪ͨ��ؽ����ɹ�����Ϣ��Ϊ���ֽڻ��ʵ����+CVcCtrlRegReq
OSPEVENT( MCU_MCSCONNECTED_NOTIF,			EV_MCU_BGN + 21 );
//����MCU�ڲ�����ʵ��֪ͨ��ض�������Ϣ��Ϊ���ֽڻ��ʵ����
OSPEVENT( MCU_MCSDISCONNECTED_NOTIF,		EV_MCU_BGN + 22 );

//����MCU�ڲ� MCU����ע��GK ֪ͨ����Ϣ�壺NULL
OSPEVENT( MCU_MCUREREGISTERGK_NOITF,        EV_MCU_BGN + 23 );

//����Ƶ��
OSPEVENT( MCU_NMS_SENDNMSMSG_CMD,           EV_MCU_BGN + 30 );

//����MCU�ڲ�����ʵ��֪ͨBAS�����ɹ�����Ϣ��ΪTPeriEqpRegReq
OSPEVENT( MCU_BASCONNECTED_NOTIF,			EV_MCU_BGN + 31 );
//����MCU�ڲ�����ʵ��֪ͨBAS��������Ϣ��ΪTEqp
OSPEVENT( MCU_BASDISCONNECTED_NOTIF,		EV_MCU_BGN + 32 );

//����MCU�ڲ�����ʵ��֪ͨPRS�����ɹ�����Ϣ��ΪTPeriEqpRegReq
OSPEVENT( MCU_PRSCONNECTED_NOTIF,			EV_MCU_BGN + 35 );
//����MCU�ڲ�����ʵ��֪ͨPRS��������Ϣ��ΪTEqp
OSPEVENT( MCU_PRSDISCONNECTED_NOTIF,		EV_MCU_BGN + 36 );

//����MCU�ڲ�����ʵ��֪ͨREC���������ɹ�����Ϣ��ΪTPeriEqpRegReq
OSPEVENT( MCU_RECCONNECTED_NOTIF,           EV_MCU_BGN + 40 );
//����MCU�ڲ�����ʵ��֪ͨREC��������Ϣ��ΪTEqp
OSPEVENT( MCU_RECDISCONNECTED_NOTIF,		EV_MCU_BGN + 41 );

//����MCU�ڲ�����ʵ��֪ͨMixer�����ɹ�����Ϣ��ΪTEqp
OSPEVENT( MCU_MIXERCONNECTED_NOTIF,		    EV_MCU_BGN + 42 );
//����MCU�ڲ�����ʵ��֪ͨMixer��������Ϣ��ΪTEqp
OSPEVENT( MCU_MIXERDISCONNECTED_NOTIF,		EV_MCU_BGN + 43 );

//����MCU�ڲ�����ʵ��֪ͨVmp�����ɹ�����Ϣ��ΪTPeriEqpRegReq
OSPEVENT( MCU_VMPCONNECTED_NOTIF,		    EV_MCU_BGN + 44 );
//����MCU�ڲ�����ʵ��֪ͨVmp��������Ϣ��ΪTEqp
OSPEVENT( MCU_VMPDISCONNECTED_NOTIF,		EV_MCU_BGN + 45 );

//����MCU�ڲ�����ʵ��֪ͨVmpTw�����ɹ�����Ϣ��ΪTPeriEqpRegReq
OSPEVENT( MCU_VMPTWCONNECTED_NOTIF,		    EV_MCU_BGN + 46 );
//����MCU�ڲ�����ʵ��֪ͨVmpTw��������Ϣ��ΪTEqp
OSPEVENT( MCU_VMPTWDISCONNECTED_NOTIF,		EV_MCU_BGN + 47 );

//tvwall 2
//����MCU�ڲ�����ʵ��֪ͨTvWall�����ɹ�����Ϣ��ΪTPeriEqpRegReq
OSPEVENT( MCU_TVWALLCONNECTED_NOTIF,		EV_MCU_BGN + 48 );
//����MCU�ڲ�����ʵ��֪ͨTvwall��������Ϣ��ΪTEqp
OSPEVENT( MCU_TVWALLDISCONNECTED_NOTIF,		EV_MCU_BGN + 49 );

// Mcu�����Լ��Ĵ��ļ��ָ�����,��Ϣ��ΪTConfInfo
OSPEVENT( MCU_WAITEQP_CREATE_CONF_NOTIFY,   EV_MCU_BGN + 50 );

//MCU��GUARDģ��������ģ�鷢�Ĳ���������Ϣ
OSPEVENT( MCU_APPTASKTEST_REQ,				EV_MCU_BGN + 51 );
//MCU������ģ����GUARDģ�鷢�Ĳ��Ի�Ӧ��Ϣ
OSPEVENT( MCU_APPTASKTEST_ACK,				EV_MCU_BGN + 52 );
//MCUֹͣGUARD�ڲ���ʱ��
OSPEVENT( MCU_STOPGUARDTIMER_CMD,           EV_MCU_BGN + 53 );
//MCU����GUARD�ڲ���ʱ��
OSPEVENT( MCU_STARTGUARDTIMER_CMD,          EV_MCU_BGN + 54 );

//MCU��ʼ����ַ�����ͨ��
OSPEVENT( MCU_ADDRINITIALIZED_NOTIFY,       EV_MCU_BGN + 55 );

//MP�Ự֪ͨMCU����
OSPEVENT( MCU_MTADP_DISCONNECTED_NOTIFY,    EV_MCU_BGN + 56 );

//VCS������ģ���Ϊ��ʱ����
OSPEVENT( MCU_SCHEDULE_VCSCONF_START,       EV_MCU_BGN + 59 );
//ԤԼ�����Ϊ��ʱ����
OSPEVENT( MCU_SCHEDULE_CONF_START,			EV_MCU_BGN + 60 );

OSPEVENT( MCU_ADDRBOOK_GETENTRYLIST_NOTIF,  EV_MCU_BGN + 61 );
OSPEVENT( MCU_ADDRBOOK_GETGROUPLIST_NOTIF,  EV_MCU_BGN + 62 );

//Mcu Guard timer id
OSPEVENT( MCUGD_GETLICENSE_DATA,            EV_MCU_BGN + 63 );	        // license�������� 
OSPEVENT( MCUGD_SCHEDULED_CHECK_TIMER,      EV_MCU_BGN + 65 );	        // ��ʱ���
//OSPEVENT( MCUGD_PASSIVE_CHECK_TIMER,      EV_MCU_BGN + 66 );			// �������
OSPEVENT( MCUGD_FEEDDOG_TIMER,              EV_MCU_BGN + 66 );	        // ��ʱι�� 

//McuVc timer id
OSPEVENT( MCUVC_APPLYFREQUENCE_CHECK_TIMER,     EV_MCU_BGN + 69 );          //���ǻ��飬mcu�����ܷ���������Ƶ�ʼ��
OSPEVENT( MCUVC_SENDFLOWCONTROL_TIMER,          EV_MCU_BGN + 70 );          //mcu����˫�������ն�flowcontrolʱ�Ķ�ʱ��
//OSPEVENT( MCUVC_VMP_WAITVMPRSP_TIMER,			EV_MCU_BGN + 71 );          //MCU�ȴ�VMPӦ��ʱ,����Ϊ16��
OSPEVENT( MCUVC_MIX_WAITMIXERRSP_TIMER,	        EV_MCU_BGN + 72 );          //MCU�ȴ�MIXERӦ��ʱ     
OSPEVENT( MCUVC_SCHEDULED_CHECK_TIMER,          EV_MCU_BGN + 73 );	        //ԤԼ���鶨ʱ���
OSPEVENT( MCUVC_ONGOING_CHECK_TIMER,            EV_MCU_BGN + 74 );			//�����л��鶨ʱ���
OSPEVENT( MCUVC_INVITE_UNJOINEDMT_TIMER,        EV_MCU_BGN + 75 );			//��ʱ����δ����ն�
OSPEVENT( MCUVC_POLLING_CHANGE_TIMER,           EV_MCU_BGN + 76 );			//��ѯ�ı�ʱ��
//OSPEVENT( MCUVC_TWPOLLING_CHANGE_TIMER,         EV_MCU_BGN + 77 );			//����ǽ��ѯ�ı�ʱ��  
OSPEVENT( MCUVC_VMPPOLLING_CHANGE_TIMER,        EV_MCU_BGN + 77 );			//��ѯ�ı�ʱ��
OSPEVENT( MCUVC_WAIT_MPREG_TIMER,               EV_MCU_BGN + 78 );			//�ȴ�Mpע��ʱ��     
OSPEVENT( MCUVC_REFRESH_MCS_TIMER,              EV_MCU_BGN + 79 );			//ˢ�»��ʱ��          
OSPEVENT( MCUVC_MCUSRC_CHECK_TIMER,             EV_MCU_BGN + 80 );			//mcuԴ֪ͨ           
OSPEVENT( MCUVC_CHANGE_VMPCHAN_SPEAKER_TIMER,   EV_MCU_BGN + 81 );            
OSPEVENT( MCUVC_CHANGE_VMPCHAN_CHAIRMAN_TIMER,  EV_MCU_BGN + 82 );
OSPEVENT( MCUVC_CHANGE_VMPPARAM_TIMER,          EV_MCU_BGN + 83 );
OSPEVENT( MCUVC_RECREATE_DATACONF_TIMER,        EV_MCU_BGN + 84 );			//���´������ݻ���
OSPEVENT( MCU_SMCUOPENDVIDEOCHNNL_TIMER,		EV_MCU_BGN + 85 );			//�¼�mcu�ȴ���˫��ͨ����ʱ��
OSPEVENT( MCUVC_WAIT_CASCADE_CHANNEL_TIMER,     EV_MCU_BGN + 86 );          //�ȴ����¼�MCU�ļ���ͨ��
OSPEVENT( MCUVC_WAIT_MPOVERLOAR_TIMER,          EV_MCU_BGN + 87 );          //�ȴ� ����ص�MP������Ϣ�� ��ʱ��
OSPEVENT( MCUVC_AUTOSWITCH_TIMER,               EV_MCU_BGN + 88 );          //Rad������ѯKedaMcu�����ն�SetOut��ʱ��
//OSPEVENT( MCUVC_HDUPOLLING_CHANGE_TIMER,        EV_MCU_BGN + 89 );          //hdu��ѯ�ı�ʱ��

// xliang [12/12/2008] vmp hdͨ����ռ�У�Ҫ��ռ���ն˽�����ռ��ʱ
OSPEVENT( MCUVC_MTSEIZEVMP_TIMER,				EV_MCU_BGN + 90 );
// xliang [12/22/2008] VMP ������ѯ��ʱ
OSPEVENT( MCUVC_VMPBATCHPOLL_TIMER,				EV_MCU_BGN + 91 );

// hdu������ѯ��ʱ
OSPEVENT( MCUVC_HDUBATCHPOLLI_CHANGE_TIMER,     EV_MCU_BGN + 92 );          //hdu������ѯ�ı�ʱ��

OSPEVENT( MCUVC_VCMTOVERTIMER_TIMER,            EV_MCU_BGN + 93 );          //�����ն˳�ʱ��ʱ��
OSPEVENT( MCUVC_VCS_CHAIRPOLL_TIMER,            EV_MCU_BGN + 94 );          //VCS��ϯ��ѯ��ʱ��

//MCU�ȴ�������Ӧ��ʱ, ��Ϊbas��5�����ͣ�ҪԤ��5���ռ�
OSPEVENT( MCUVC_WAITBASRRSP_TIMER,			    EV_MCU_BGN + 95 );
//MCU���ն˷�FASTUPDATE�Ķ�ʱ���� ��199�Ŀռ�
OSPEVENT( MCUVC_FASTUPDATE_TIMER_ARRAY,	        EV_MCU_BGN + 100 );


//MCU�ڲ�MP���������� ֪ͨ��Ϣ
OSPEVENT( MCUVC_MPOVERLOAD_NOTIF,               EV_MCU_BGN + 298 );
//MCU���ն˷�˫��FASTUPDATE�Ķ�ʱ��
OSPEVENT( MCUVC_SECVIDEO_FASTUPDATE_TIMER,  	EV_MCU_BGN + 299 );

//MCU���鿪ʼ�Զ�¼��ʱ����
OSPEVENT( MCUVC_CONFSTARTREC_TIMER,             EV_MCU_BGN + 300 ); 
//MCU��ʼ����ʱ��, zgc, 2008-03-27
OSPEVENT( MCUVC_RECPLAY_WAITMPACK_TIMER,		EV_MCU_BGN + 301 );
//MCUÿ���賿0�㣬���øö�ʱ����������ˢ�¸�����Զ�¼��ʱ��, pengjie, 20091225
OSPEVENT( MCUVC_UPDATA_AUTOREC_TIMER,		EV_MCU_BGN + 302 );

//˫������mcu��ʱ�����ƻ��Ķ�ʱ�� [pengjie 2010/2/26]
OSPEVENT( MCUVC_NOTIFYMCUH239TOKEN_TIMER, 	EV_MCU_BGN + 303 );

// [pengjie 2010/9/29] �������Ƿ��й㲥Դ
OSPEVENT( MCUVC_CHECK_CONFBRDSRC_TIMER,		EV_MCU_BGN + 304 );

//MCU�ȴ�HD����Ƶ������Ӧ��ʱ, ��Ϊ����hd-bas����Ƶ���������4·��ҪԤ��3���ռ䣨��0��ʼ��
OSPEVENT( MCUVC_WAITHDVIDBASRSP_TIMER,			EV_MCU_BGN + 305 );
//MCU�ȴ�HD˫��������Ӧ��ʱ, ��Ϊ����hd-bas˫�����������1·
OSPEVENT( MCUVC_WAITHDDVIDBASRSP_TIMER,			EV_MCU_BGN + 310 );

//���ǻ������صĲ��ֶ�ʱ
OSPEVENT( TIMER_SCHEDULED_CHECK,                EV_MCU_BGN + 311 );
OSPEVENT( TIMER_DELAYCONF,                      EV_MCU_BGN + 312 );
OSPEVENT( TIMER_RESCHED,                        EV_MCU_BGN + 313 );
OSPEVENT( TIMER_ADDMT,                          EV_MCU_BGN + 314 );
OSPEVENT( TIMER_DELMT,                          EV_MCU_BGN + 315 );
OSPEVENT( TIMER_INVITE_UNJOINEDMT,              EV_MCU_BGN + 316 );
OSPEVENT( TIMER_SCHEDCONF,                      EV_MCU_BGN + 317 );

//���ǻ��飬��ʱˢ��; ���ǻ����ն���·��KeepAlive���ڱ���Ϣ����
OSPEVENT( MCUVC_CONFINFO_MULTICAST_TIMER,		EV_MCU_BGN + 318 );

//����MCU�ڲ�����ʵ��֪ͨHdu�����ɹ�����Ϣ��ΪTPeriEqpRegReq
OSPEVENT( MCU_HDUCONNECTED_NOTIF,		        EV_MCU_BGN + 350 );
//����MCU�ڲ�����ʵ��֪ͨHdu��������Ϣ��ΪTEqp
OSPEVENT( MCU_HDUDISCONNECTED_NOTIF,		    EV_MCU_BGN + 351 );
//mcucfg�ȴ�DSC�޸����ý�����ʱ��ʱ��, zgc, 2007-06-23
OSPEVENT( MCUCFG_DSCCFG_WAITINGCHANGE_OVER_TIMER,	EV_MCU_BGN + 352 );
//��ػỰ֪ͨmcucfg������Ϣ, zgc, 2007-11-09
//��Ϣ�壺��
OSPEVENT( MCSSSN_MCUCFG_MCSDISSCONNNECT_NOTIFY,		EV_MCU_BGN + 353 );

// [11/14/2011 liuxu]���巢��AllMtInfoToAllMcs��timer
OSPEVENT( MCUVC_SENDALLMT_ALLMCS_TIMER,	        EV_MCU_BGN + 354 );

// [11/14/2011 liuxu]���巢��AllMtInfoToAllMcs��timer
OSPEVENT( MCUVC_SEND_SMCUMTSTATUS_TIMER,	    EV_MCU_BGN + 355 );


//MCU�����跢FASTUPDATE�Ķ�ʱ���� ��48���ռ� _(+360 ��+408)
//������HDU��Tvwall��Recorder��Mixer��Prs��ֻ����bas/hdbas��vmp/hdvmp��vmptw���ݲ�������ͨ��ID
OSPEVENT( MCUVC_FASTUPDATE_TIMER_ARRAY4EQP,     EV_MCU_BGN + 360 );


/*
*	Special For Satellate MCU
*/
OSPEVENT( MCU_MULTICAST_MSG,				    EV_MCU_BGN + 410 );

OSPEVENT( MCU_MULTICAST_CONFLIST,				EV_MCU_BGN + 411 );

/////////////////////////////////////////////////////////////////////////
//mcu�ڲ���Ϣ
//����ǽ��ͨ����ѯ��ʱ��(��������dec5, hdu)Ԥ��80���ռ�(412 ~ 491)
OSPEVENT(MCUVC_TWPOLLING_CHANGE_TIMER ,		    EV_MCU_BGN + 412 );

//////////////////////////////////////////////////////////////////////////
//  mcuhdu�ڲ���Ϣ
//
//  ֪ͨhdu��ʼ��������
//  ��Ϣ��  THduSwitchInfo
OSPEVENT( MCUVC_STARTSWITCHHDU_NOTIFY,		  EV_MCU_BGN + 492 );
 
///////////////////////////////////////////////////////////
// mcuvcs�ڲ���Ϣ
// �ϼ�MCU�ͷŶ��¼����Ȼ�����Ƶ�Ȩ��      ��Ϣ��:��(��Ϣͷ�к���ָ�������CONFID)
OSPEVENT( MCUVC_VCSSN_MMCURLSCTRL_CMD,	 EV_MCU_BGN + 493 );
// �ϼ�MCU��ʼ�ӹܶԻ���ĵ���Ȩ��          ��Ϣ��:��
OSPEVENT( MCUVC_MMCUGETCTRL_CMD,             EV_MCU_BGN + 494 );

///////////////////////////////////////////////////////////
//����λ��MCU֮�������������ͨѶ��Ϣ

//������������ģ��������Ϣ
OSPEVENT( MCU_MSEXCHANGER_POWERON_CMD,	EV_MCU_BGN + 500 );

//��λ��MCU����λ��MCU �����Ӷ�ʱ��
OSPEVENT( MCU_MCU_CONNECT_TIMER,		EV_MCU_BGN + 501 );
//��λ��MCU����λ��MCU ��ע�ᶨʱ��
OSPEVENT( MCU_MCU_REGISTER_TIMER,		EV_MCU_BGN + 502 );

//vxworks�¼��������״̬�Ķ�ʱ��
OSPEVENT( MCU_MCU_CEHCK_MSSTATE_TIMER,	EV_MCU_BGN + 503 );

//���ð�MCU���ð�MCU �����ݵ�����ʱ��
OSPEVENT( MCU_MCU_SYN_TIMER,		    EV_MCU_BGN + 504 );

//MCU֮�� �ȴ��Զ˵�����Ӧ�� �ĳ�ʱ��ʱ��
OSPEVENT( MCU_MCU_WAITFORRSP_TIMER,	    EV_MCU_BGN + 505 );

//��MCU��mcu����������
OSPEVENT( MCU_MCU_REBOOT_CMD,	        EV_MCU_BGN + 506 );
//��MCU��mcu��������������
OSPEVENT( MCU_MCU_UPDATE_CMD,	        EV_MCU_BGN + 509 );

//mcuagt��mcu������������mcu������
OSPEVENT( MCUAGT_MCU_REBOOT_CMD,	    EV_MCU_BGN + 507 );

//MCU֮�� ROUNDTRIP��·��� �ĳ�ʱ��ʱ��
OSPEVENT( MCU_MCU_ROUNDTRIP_TIMER,	    EV_MCU_BGN + 508 );

//��λ��MCU����λ��MCU ��ע������ ��Ϣ��: u32 dwLocalIp
OSPEVENT( MCU_MCU_REGISTER_REQ,			EV_MCU_BGN + 511 );
//��λ��MCU����λ��MCU ��ע���������
OSPEVENT( MCU_MCU_REGISTER_ACK,			EV_MCU_BGN + 512 );
//��λ��MCU����λ��MCU ��ע������ܾ�
OSPEVENT( MCU_MCU_REGISTER_NACK,		EV_MCU_BGN + 513 );

//MCU֮�� ֪ͨ �Է� ����MCU������λ���ͼ���ǰ������ʹ��״̬������������Э�� ��Ϣ��: u8 LocalMSType + u8 CurMSState
OSPEVENT( MCU_MCU_MS_DETERMINE,			EV_MCU_BGN + 516 );
//MCU֮�� ֪ͨ �Է� ����MCU��Э�̵�����ʹ�ý�� ��Ϣ��: u8 CurMSState
OSPEVENT( MCU_MCU_MS_RESULT,			EV_MCU_BGN + 517 );

//���ð�MCU���ð�MCU ���� ��ʼ���ݵ��� ��Ϣ��: TMSSynInfoReq
OSPEVENT( MCU_MCU_START_SYN_REQ,		EV_MCU_BGN + 521 );
//���ð�MCU�����ð�MCU ���� ��ʼ���ݵ��� ��Ϣ��: TMSSynInfoRsp
OSPEVENT( MCU_MCU_START_SYN_ACK,		EV_MCU_BGN + 522 );
//��λ��MCU����λ��MCU ������Ϣͬ������ܾ� ��Ϣ��: TMSSynState
OSPEVENT( MCU_MCU_START_SYN_NACK,		EV_MCU_BGN + 523 );
//���ð�MCU���ð�MCU �������ݵ��� ֪ͨ
OSPEVENT( MCU_MCU_END_SYN_NTF,			EV_MCU_BGN + 524 );

//��λ��MCU��λ��MCU ʵ���а���������ݵ��� ͬ�� ���� ��Ϣ��: TMSSynDataReqHead
OSPEVENT( MCU_MCU_SYNNING_DATA_REQ,		EV_MCU_BGN + 526 );
//��λ��MCU����λ��MCU ʵ���а���������ݵ��� ͬ�� ��Ӧ ��Ϣ��: TMSSynDataRsp
OSPEVENT( MCU_MCU_SYNNING_DATA_RSP,		EV_MCU_BGN + 527 );

//���ð�����Ϊ���ð�ʱ ֪ͨҵ��͸����Ựģ��������״̬�������Ϣ��: u8�Ƿ��л��ɹ�
OSPEVENT( MCU_MSSTATE_EXCHANGE_NTF,		EV_MCU_BGN + 531 );

//����̽����Ϣ
OSPEVENT( MCU_MCU_PROBE_REQ,            EV_MCU_BGN + 532 );
OSPEVENT( MCU_MCU_PROBE_ACK,            EV_MCU_BGN + 533 );  

//��mcu����mcu��LED���Ե�notify����Ϣ�壺TBrdLedState
OSPEVENT( MCU_MCU_LEDSTATUS_NOTIFY,	    EV_MCU_BGN + 534 );

OSPEVENT( EV_MCU_OPENSECVID_TIMER,		EV_MCU_BGN + 535 );//Ŀǰֻ���polycom

//����֧�ֶ�vmp��vmp���Timer����Ϊ16�ݣ�Ԥ��32��(608-639)
//����MCUVC_VMP_WAITVMPRSP_TIMER��MCUVC_WAIT_ALLVMPPRESETIN_ACK_TIMER
OSPEVENT( MCUVC_VMP_WAITVMPRSP_TIMER,        EV_MCU_BGN + 608 );
OSPEVENT( MCUVC_WAIT_ALLVMPPRESETIN_ACK_TIMER,  EV_MCU_BGN + 624 );

//N+1ģʽ MCU�ڲ���Ϣ
OSPEVENT( NPLUS_VC_DATAUPDATE_NOTIF,        EV_MCU_BGN + 640 );
//mcuҵ�񷢸���mcu���ݹ���ģ����Ϣ
OSPEVENT( VC_NPLUS_MSG_NOTIF,               EV_MCU_BGN + 641 );
//֪ͨN+1���ݷ��������ã�ֹͣ���ݷ��񣬵����Ͽ����ӵĿͻ���
OSPEVENT( VC_NPLUS_RESET_NOTIF,             EV_MCU_BGN + 642 );

//N+1ģʽ mcu����Ϣ
//���Ӷ�ʱ��
OSPEVENT( MCU_NPLUS_CONNECT_TIMER,          EV_MCU_BGN + 650 );

//ע�ᶨʱ��
OSPEVENT( MCU_NPLUS_REG_TIMER,              EV_MCU_BGN + 651 );

//��·��ⳬʱ��ʱ��
OSPEVENT( MCU_NPLUS_RTD_TIMER,              EV_MCU_BGN + 652 );

//ע����Ϣ����Ϣ�壺TCMcuRegInfo
OSPEVENT( MCU_NPLUS_REG_REQ,                EV_MCU_BGN + 700 );
OSPEVENT( MCU_NPLUS_REG_ACK,                EV_MCU_BGN + 701 );
OSPEVENT( MCU_NPLUS_REG_NACK,               EV_MCU_BGN + 702 );

//��·�����Ϣ����Ϣ�壺��
OSPEVENT( MCU_NPLUS_RTD_REQ,                EV_MCU_BGN + 703 );
OSPEVENT( MCU_NPLUS_RTD_RSP,                EV_MCU_BGN + 704 );

//mcu������Ϣ֪ͨ��Ϣ������/��������ʱ֪ͨ������Ϣ�壺u8(NPLUS_CONF_START, NPLUS_CONF_RELEASE)+TConfInfo
OSPEVENT( MCU_NPLUS_CONFINFOUPDATE_REQ,     EV_MCU_BGN + 705 );
OSPEVENT( MCU_NPLUS_CONFINFOUPDATE_ACK,     EV_MCU_BGN + 706 );
OSPEVENT( MCU_NPLUS_CONFINFOUPDATE_NACK,    EV_MCU_BGN + 707 );

//mcu�û�����Ϣ֪ͨ��Ϣ ��ע��ɹ����û�����Ϣ�仯ʱ�����ع�ʱ֪ͨ������Ϣ�壺CUsrGrpsInfo
OSPEVENT( MCU_NPLUS_USRGRPUPDATE_REQ,         EV_MCU_BGN + 708 );
OSPEVENT( MCU_NPLUS_USRGRPUPDATE_ACK,         EV_MCU_BGN + 709 );
OSPEVENT( MCU_NPLUS_USRGRPUPDATE_NACK,        EV_MCU_BGN + 710 );

//����ն���Ϣ֪ͨ������ն��б仯ʱ֪ͨ������Ϣ�壺TMtInfo[]
OSPEVENT( MCU_NPLUS_CONFMTUPDATE_REQ,       EV_MCU_BGN + 711 );
OSPEVENT( MCU_NPLUS_CONFMTUPDATE_ACK,       EV_MCU_BGN + 712 );
OSPEVENT( MCU_NPLUS_CONFMTUPDATE_NACK,      EV_MCU_BGN + 713 );

//��ϯ�ն�֪ͨ����Ϣ�壺TMtAlias
OSPEVENT( MCU_NPLUS_CHAIRUPDATE_REQ,        EV_MCU_BGN + 714 );
OSPEVENT( MCU_NPLUS_CHAIRUPDATE_ACK,        EV_MCU_BGN + 715 );
OSPEVENT( MCU_NPLUS_CHAIRUPDATE_NACK,       EV_MCU_BGN + 716 );

//�����ն�֪ͨ����Ϣ�壺TMtAlias
OSPEVENT( MCU_NPLUS_SPEAKERUPDATE_REQ,      EV_MCU_BGN + 717 );
OSPEVENT( MCU_NPLUS_SPEAKERUPDATE_ACK,      EV_MCU_BGN + 718 );
OSPEVENT( MCU_NPLUS_SPEAKERUPDATE_NACK,     EV_MCU_BGN + 719 );

//VMP����Ϣ�壺TNPlusVmpInfo+TVMPParam
OSPEVENT( MCU_NPLUS_VMPUPDATE_REQ,          EV_MCU_BGN + 720 );
OSPEVENT( MCU_NPLUS_VMPUPDATE_ACK,          EV_MCU_BGN + 721 );
OSPEVENT( MCU_NPLUS_VMPUPDATE_NACK,         EV_MCU_BGN + 722 );

//ConfData֪ͨ��Ϣ��ע��ɹ�ʱ�����ع�ʱ֪ͨ������Ϣ�壺TNPlusConfData + N�ֽ���չ����+MAXNUM_SUB_MCU*TSmcuCallInfo
OSPEVENT( MCU_NPLUS_CONFDATAUPDATE_REQ,       EV_MCU_BGN + 723 );
OSPEVENT( MCU_NPLUS_CONFDATAUPDATE_ACK,       EV_MCU_BGN + 724 );
OSPEVENT( MCU_NPLUS_CONFDATAUPDATE_NACK,      EV_MCU_BGN + 725 );

//�û���Ϣ֪ͨ��Ϣ��ע��ɹ�ʱ�����ع�ʱ֪ͨ������Ϣ�壺CExUsrInfo[]����Ҫ�����а�
OSPEVENT( MCU_NPLUS_USRINFOUPDATE_REQ,        EV_MCU_BGN + 726 );
OSPEVENT( MCU_NPLUS_USRINFOUPDATE_ACK,        EV_MCU_BGN + 727 );
OSPEVENT( MCU_NPLUS_USRINFOUPDATE_NACK,       EV_MCU_BGN + 728 );

//������Ϣ�ع�
OSPEVENT( MCU_NPLUS_CONFROLLBACK_REQ,        EV_MCU_BGN + 729 );
OSPEVENT( MCU_NPLUS_CONFROLLBACK_ACK,        EV_MCU_BGN + 730 );
OSPEVENT( MCU_NPLUS_CONFROLLBACK_NACK,       EV_MCU_BGN + 731 );

//�û���Ϣ�ع�
OSPEVENT( MCU_NPLUS_USRROLLBACK_REQ,        EV_MCU_BGN + 732 );
OSPEVENT( MCU_NPLUS_USRROLLBACK_ACK,        EV_MCU_BGN + 733 );
OSPEVENT( MCU_NPLUS_USRROLLBACK_NACK,       EV_MCU_BGN + 734 );

//�û�����Ϣ�ع�
OSPEVENT( MCU_NPLUS_GRPROLLBACK_REQ,        EV_MCU_BGN + 735 );
OSPEVENT( MCU_NPLUS_GRPROLLBACK_ACK,        EV_MCU_BGN + 736 );
OSPEVENT( MCU_NPLUS_GRPROLLBACK_NACK,       EV_MCU_BGN + 737 );

//GKʵ��ע��֪ͨ��Ϣ����Ϣ�壺TRASInfo
OSPEVENT( MCU_NPLUS_RASINFOUPDATE_REQ,      EV_MCU_BGN + 738 );
OSPEVENT( MCU_NPLUS_RASINFOUPDATE_ACK,      EV_MCU_BGN + 739 );
OSPEVENT( MCU_NPLUS_RASINFOUPDATE_NACK,     EV_MCU_BGN + 740 );

//AutoMix����Ϣ�壺BOOL32
OSPEVENT( MCU_NPLUS_AUTOMIXUPDATE_REQ,          EV_MCU_BGN + 741 );
OSPEVENT( MCU_NPLUS_AUTOMIXUPDATE_ACK,          EV_MCU_BGN + 742 );
OSPEVENT( MCU_NPLUS_AUTOMIXUPDATE_NACK,         EV_MCU_BGN + 743 );

//ͬ���¼�MCU������Ϣ ��Ϣ�壺TSmcuCallInfo[]����
OSPEVENT( MCU_NPLUS_SMCUINFOUPDATE_REQ,         EV_MCU_BGN + 744 );
OSPEVENT( MCU_NPLUS_SMCUINFOUPDATE_ACK,			EV_MCU_BGN + 745 );
OSPEVENT( MCU_NPLUS_SMCUINFOUPDATE_NACK,		EV_MCU_BGN + 746 );

//�����������֪ͨ����Ϣ�壺TNPlusEqpCap
OSPEVENT( MCU_NPLUS_EQPCAP_NOTIF,           EV_MCU_BGN + 750 );

//MCU����VCS���͵ķ�����Ϣ��ʱ�� ��MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE(128�� +800 -- +927)
OSPEVENT( MCUVC_PACKINFO_TIMER,             EV_MCU_BGN + 800);

// [1/7/2011 xliang] ����Ϣ�������Ͷ�ʱ
OSPEVENT( MCUVC_SMSPACK_TIMER,				EV_MCU_BGN + 950);

// [pengjie 2011/5/18]��Ϣ951~953�����������Ѿ���ռ�ã����ǵ����ܺϲ������⣬���ｫ��Ԥ��

// [pengjie 2011/5/19] ��vcsssn��ͨinstan����daemon�Ŀ�����ʱ�����Ϣ��Ϊ u8 byTimeIdx ��ʱ��Id
OSPEVENT( MCUVC_STARTRELEASECONFTIMER_CMD,			EV_MCU_BGN + 954);
// [pengjie 2011/5/18] vcs�����ӳٽ�ᶨʱ������Ҫռ��16��timer( 955 ~ 971 )
OSPEVENT( MCUVC_RELEASECONF_TIMER,			EV_MCU_BGN + 955);

// END (EV_MCU_BGN + 971)
//�ն�����H239token�ȴ���˫��ͨ����ʱ��
OSPEVENT( MCUVC_WAITOPENDSCHAN_TIMER,           EV_MCU_BGN + 972 );
//��������ϳɳ�Ա����ı�����еĳ�Ա��Ϊ�¼��նˣ�������ش���������16��
//OSPEVENT( MCUVC_WAIT_ALLVMPPRESETIN_ACK_TIMER,			    EV_MCU_BGN + 973 );
//����������������������Ҫ�ָ��������Ҫ������ͬ������
OSPEVENT( MCUVC_RECOVERCONF_SYN_TIMER,           EV_MCU_BGN + 974 );
//��ش��������뱻������ģʽ�£����¼��ն˵�ʱ����Ҫ�ȴ��¼��ն˻�ack����Ϊ���Ե��¸��ն�
OSPEVENT( MCUVC_WAIT_LASTVMPROLLCALLFINISH_TIMER,           EV_MCU_BGN + 975 );
//�������ݻ������ݵ�ʱ�򣬸�instance���ã�ongoing��schedule��״̬����ȴ���������timer
OSPEVENT( MCUVC_WAITMSCONFDATA_TIMER,           EV_MCU_BGN + 976 );
///////////////////////////////////////////////////////////

#endif /* _EV_MCU_H_ */
