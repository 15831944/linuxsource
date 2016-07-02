/******************************************************************************
   ģ����      : �ն�����Ự
   �ļ���      : TMtssn.cpp
   ����ļ�    : TMtssn.h
   �ļ�ʵ�ֹ���: �ն�����Ựʵ��
   ����        : ������
   �汾        : V3.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/11/08  3.0         ������      ����
   2005/02/19  3.6         ����      �����޸ġ���3.5�汾�ϲ�
******************************************************************************/

#include "evmcumt.h"
#include "evmcu.h"
#include "evmcuvcs.h"
#include "mcuvc.h"
#include "mtadpssn.h"
#include "mcuver.h"
#include "evmcueqp.h"
//#include "mcuerrcode.h"

CMtAdpSsnApp     g_cMtAdpSsnApp;

//����
CMtAdpSsnInst::CMtAdpSsnInst()
{
	m_byMtAdpId		 = 0;
	m_dwMtAdpAppIId	 = 0;
	m_byProtocolType = 0;
	m_byMaxMtNum     = 0;
	m_dwMtAdpNode	 = INVALID_NODE;
}

//����
CMtAdpSsnInst::~CMtAdpSsnInst()
{

}

/*====================================================================
    ������      : InstanceEntry
    ����        ����ͨʵ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����pcMsg ��Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
#define MT_EV_LOG(level)	MCU_EV_LOG(level, MID_MCU_MT, "Mt")

void CMtAdpSsnInst::InstanceEntry( CMessage * const pcMsg )
{
	
	if( NULL == pcMsg )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MT, "MtAdpSsn[InstanceEntry]: Null pointer message Received in InstanceEntry.\n" );
		return;
	}
	
	switch( pcMsg->event ) 
	{
	case MTADP_MCU_REGISTER_REQ:
		MT_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMtAdpRegisterReq( pcMsg );
		break;
	case OSP_DISCONNECT:
		MT_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMtAdpDisconnect( pcMsg );
		break;
    
    case MTADP_MCU_GETMSSTATUS_REQ:
    case MCU_MSSTATE_EXCHANGE_NTF:
		MT_EV_LOG(LOG_LVL_KEYSTATUS);
        ProcMtAdpGetMsStatusReq( pcMsg );
        break;

	case MT_MCU_ROUNDTRIPDELAY_ACK:			//�ն˷���MCU�ĳɹ�Ӧ��
	case MT_MCU_ROUNDTRIPDELAY_REQ:			//MT����MCU��RoundTripDelay����
		break;

	case MT_MCU_REGISTERGK_ACK:				//��GK��ע��ɹ�
	case MT_MCU_REGISTERGK_NACK:			//��GK��ע��ʧ��
	case MT_MCU_UNREGISTERGK_ACK:			//��GK��ע���ɹ�
	case MT_MCU_UNREGISTERGK_NACK:			//��GK��ע��ʧ��
	case MT_MCU_UPDATE_GKANDEPID_NTF:       //��������������GK��ע���GatekeeperID/EndpointID��Ϣ��֪ͨ
		// [9/15/2011 liuxu] MT_MCU_UPDATE_GKANDEPID_NTF��Ϣ̫��, �����������ȼ�
		if(pcMsg->event != MT_MCU_UPDATE_GKANDEPID_NTF)
		{
			MT_EV_LOG(LOG_LVL_KEYSTATUS);
		}else
		{
			MT_EV_LOG(LOG_LVL_DETAIL);
		}
		ProcMtAdpMcuGKMsg( pcMsg );
		break;

	case MCU_MCU_REGISTER_NOTIF:	//�ϲ�������ע����Ϣ
	case MCU_MCU_ROSTER_NOTIF:		//mcu�ϲ�����ʽ�Ļ�����֪ͨ
	case MCU_MCU_MTLIST_REQ:		//����Է�MCU���ն��б� 
	case MCU_MCU_MTLIST_ACK:		//����Է�MCU���ն��б�ĳɹ�Ӧ��
	case MCU_MCU_MTLIST_NACK:		//����Է�MCU���ն��б��ʧ��Ӧ��
	case MCU_MCU_VIDEOINFO_REQ:		//����Է�MCU����Ƶ��Ϣ
	case MCU_MCU_VIDEOINFO_ACK:		//����Է�MCU����Ƶ��Ϣ�ĳɹ�Ӧ��
	case MCU_MCU_VIDEOINFO_NACK:	//����Է�MCU����Ƶ��Ϣ��ʧ��Ӧ��
	case MCU_MCU_CONFVIEWCHG_NOTIF:	//Ķmcu-mcu������ͼ�����仯֪ͨ   
	case MCU_MCU_AUDIOINFO_REQ:		//����Է�MCU����Ƶ��Ϣ
	case MCU_MCU_AUDIOINFO_ACK:		//����Է�MCU����Ƶ��Ϣ�ĳɹ�Ӧ��
	case MCU_MCU_AUDIOINFO_NACK:	//����Է�MCU����Ƶ��Ϣ��ʧ��Ӧ��
		
	case MCU_MCU_INVITEMT_REQ:		//�Է�MCUָ�����������ն�����			
	case MCU_MCU_INVITEMT_ACK:		//�Է�MCUָ�����������ն˳ɹ�Ӧ��
	case MCU_MCU_INVITEMT_NACK:		//�Է�MCUָ�����������ն�ʧ��Ӧ��
	case MCU_MCU_NEWMT_NOTIF:		//�Է�MCU֪ͨ�����������ն˼���
	case MCU_MCU_REINVITEMT_REQ:	//�Է�MCUָ�����������ն�����
	case MCU_MCU_REINVITEMT_ACK:    //�����ն�����ĳɹ�Ӧ��
	case MCU_MCU_REINVITEMT_NACK:   //�����ն������ʧ��Ӧ��
	case MCU_MCU_DROPMT_REQ:        //mcu-mcu�Ҷ��ն�����
	case MCU_MCU_DROPMT_ACK:        //mcu-mcu�Ҷ��ն�����ɹ�Ӧ��
	case MCU_MCU_DROPMT_NACK:       //mcu-mcu�Ҷ��ն�����ʧ��Ӧ��
	case MCU_MCU_DROPMT_NOTIF:		//mcu-mcu�Ҷ��ն�֪ͨ

	case MCU_MCU_DELMT_REQ:         //mcu-mcuɾ���ն�����
	case MCU_MCU_DELMT_ACK:			//mcu-mcuɾ���ն˳ɹ�Ӧ��
	case MCU_MCU_DELMT_NACK:		//mcu-mcuɾ���ն�ʧ��Ӧ��
	case MCU_MCU_DELMT_NOTIF:       //ɾ���ն�֪ͨ
		
	case MCU_MCU_SETMTCHAN_REQ:    //mcu-mcu��������ý��ͨ������
	case MCU_MCU_SETMTCHAN_ACK:    //mcu-mcu��������ý��ͨ�����سɹ�Ӧ��
	case MCU_MCU_SETMTCHAN_NACK:   //mcu-mcu��������ý��ͨ������ʧ��Ӧ��
	case MCU_MCU_SETMTCHAN_NOTIF:  //mcu-mcuý��ͨ������״̬֪ͨ

	case MCU_MCU_CALLALERTING_NOTIF:	//���ں���ͨ��
	
	case MCU_MCU_SETIN_REQ:         //mcu-mcuָ���ն�ͼ�����view
	case MCU_MCU_SETIN_ACK:         //mcu-mcuָ���ն�ͼ�����view �ɹ�Ӧ��
	case MCU_MCU_SETIN_NACK:        //mcu-mcuָ���ն�ͼ�����view ʧ��Ӧ��

	case MCU_MCU_SETOUT_REQ:         //mcu-mcuָ��view���뵽�ն�
	case MCU_MCU_SETOUT_ACK:         //mcu-mcuָ��view���뵽�ն˳ɹ�Ӧ��
	case MCU_MCU_SETOUT_NACK:        //mcu-mcuָ��view���뵽�ն�ʧ��Ӧ��
	case MCU_MCU_SETOUT_NOTIF:       //mcu-mcuָ��view���뵽�ն�֪ͨ

// [pengjie 2010/8/12]
	case MCU_MCU_FECC_CMD:			//��MCU��������ͷԶң

	case MCU_MCU_SENDMSG_NOTIF:     //mcu-mcu����Ϣ
	case MCU_MCU_STARTMIXER_CMD:    //mcu-mcu
	case MCU_MCU_STARTMIXER_NOTIF:  //mcu-mcu
	case MCU_MCU_STOPMIXER_CMD:     //mcu-mcu
	case MCU_MCU_STOPMIXER_NOTIF:   //mcu-mcu
	case MCU_MCU_GETMIXERPARAM_REQ: //mcu-mcu
	case MCU_MCU_GETMIXERPARAM_ACK: //mcu-mcu
	case MCU_MCU_GETMIXERPARAM_NACK: //mcu-mcu
	case MCU_MCU_MIXERPARAM_NOTIF:  //mcu-mcu
	case MCU_MCU_ADDMIXMEMBER_CMD:  //mcu-mcu
	case MCU_MCU_REMOVEMIXMEMBER_CMD:  //mcu-mcu

	case MCU_MCU_LOCK_REQ:            //mcu-mcu ���������Է�MCU
	case MCU_MCU_LOCK_ACK:            //mcu-mcu ���������Է�MCU�ɹ�Ӧ��
	case MCU_MCU_LOCK_NACK:           //mcu-mcu ���������Է�MCUʧ��Ӧ��
		
	case MCU_MCU_MTSTATUS_CMD:        //mcu-mcu ����Է�MCUĳһ�ն�״̬
	case MCU_MCU_MTSTATUS_NOTIF:      //mcu-mcu ֪ͨ�Է�MCU�Լ�ĳһ����ֱ���ն�״̬
        
    case MCU_MCU_AUTOSWITCH_REQ:      //RadMcu->KedaMcu ������ѯ����
    case MCU_MCU_AUTOSWITCH_ACK:      //KedaMcu->RadMcu ������ѯ�ɹ�Ӧ��
    case MCU_MCU_AUTOSWITCH_NACK:     //KedaMcu->RadMcu ������ѯʧ��Ӧ��

	case MCU_MCU_ADJMTRES_REQ:		  //������MT��Ƶ�ֱ��� (�Ǳ�)
	case MCU_MCU_ADJMTRES_ACK:
	case MCU_MCU_ADJMTRES_NACK:

// [pengjie 2010/6/19] ������֡�� (�Ǳ�)
	case MCU_MCU_ADJMTFPS_REQ:        
	case MCU_MCU_ADJMTFPS_ACK:
	case MCU_MCU_ADJMTFPS_NACK:

	//zjj20100201
	//  pengjie[9/29/2009] ������ش���Ϣ
	case MCU_MCU_MULTSPYCAP_NOTIF:
	case MCU_MCU_PRESETIN_REQ:
	case MCU_MCU_PRESETIN_ACK:
	case MCU_MCU_PRESETIN_NACK:
	case MCU_MCU_SPYCHNNL_NOTIF:
	case MCU_MCU_SWITCHTOMCUFAIL_NOTIF:
	case MCU_MCU_REJECTSPY_NOTIF:
	case MCU_MCU_BANISHSPY_CMD:
	case MCU_MCU_MTEXTINFO_NOTIF:
	//lukunpeng 2010/06/10 �������������ϼ������¼�����ռ��������ʲ���Ҫ��ͨ���¼�֪ͨ�ϼ�
	//case MCU_MCU_BANISHSPY_NOTIF:
	case MCU_MCU_SPYFASTUPDATEPIC_CMD:

    case MCU_MCU_APPLYSPEAKER_REQ:      //�缶���뷢����֧��
    case MCU_MCU_APPLYSPEAKER_ACK:
    case MCU_MCU_APPLYSPEAKER_NACK:
    case MCU_MCU_APPLYSPEAKER_NOTIF:
	case MCU_MCU_SPEAKSTATUS_NTF:
	case MCU_MCU_SPEAKERMODE_NOTIFY:
    case MCU_MCU_CANCELMESPEAKER_REQ:
    case MCU_MCU_CANCELMESPEAKER_ACK:
    case MCU_MCU_CANCELMESPEAKER_NACK:
	
// [8/6/2010 xliang] mt mute and dumb operation
	case MCU_MCU_MTAUDMUTE_REQ:
	case MCU_MCU_MTAUDMUTE_ACK:
	case MCU_MCU_MTAUDMUTE_NACK:

	case VCS_MCU_MUTE_REQ:					// [9/6/2010 xliang] vcs Զ�˾�������͸��
    case MCU_MCU_CHANGEMTSECVIDSEND_CMD:    //[5/4/2011 zhushengze]VCS���Ʒ����˷�˫��
    case MCU_MCU_TRANSPARENTMSG_NOTIFY:     //[2/23/2012 zhushengze]���桢�ն���Ϣ͸��
	case MCU_MCU_ADJMTBITRATE_CMD:
		MT_EV_LOG(LOG_LVL_DETAIL);
		ProcMcuCasCadeMsg( pcMsg );
		break;	
	
	case MT_MCU_CREATECONF_REQ:				//�ն˷���MCU�Ĵ�����������
    case MT_MCU_LOWERCALLEDIN_NTF:          // guzh [6/19/2007] �¼�MCU��������
	case MT_MCU_DROPCONF_CMD:				//��ϯ�ն˷���MCU����ֹ��������
	case MT_MCU_INVITEMT_ACK:				//MT����Ӧ��MT->MCU
	case MT_MCU_INVITEMT_NACK:				//MT�ܾ�Ӧ��MT->MCU
	case MT_MCU_DELMT_REQ:					//��ϯ�ն˷���MCU��ǿ���ն��˳�����
	case MT_MCU_APPLYCHAIRMAN_REQ:			//�ն˷���MCU��������ϯ����
	case MT_MCU_CANCELCHAIRMAN_REQ:			//��ϯ�ն˷��͸�MCU��ȡ����ϯ������
	case MT_MCU_GETCHAIRMAN_REQ:			//��ѯ������ϯȨ�޵��ն�ID
	case MT_MCU_SPECSPEAKER_REQ:			//��ϯ�ն˷���MCU��ָ������������
	case MT_MCU_SPECSPEAKER_CMD:			//��ϯ�ն˷���MCU��ȡ������������
	case MT_MCU_CANCELSPEAKER_CMD:
	case MT_MCU_APPLYSPEAKER_NOTIF:			//�ն˷���MCU�����뷢������
	case MT_MCU_STARTSELMT_CMD:				//��ϯ�ն˷���MCU��ѡ���ն�����
	case MT_MCU_STOPSELMT_CMD:				//��ϯ�ն˷���MCU��ֹͣѡ���ն�����
	case MT_MCU_STARTSELMT_REQ:
	case MT_MCU_OPENLOGICCHNNL_ACK:			//�ն˷���MCU�ĳɹ�Ӧ��
	case MT_MCU_OPENLOGICCHNNL_NACK:		//�ն˷���MCU��ʧ��Ӧ��
	case MT_MCU_OPENLOGICCHNNL_REQ:			//MT����MCU�Ĵ��߼�ͨ������
	case MT_MCU_CLOSELOGICCHNNL_NOTIF:		//MT����MCU�Ĺر��߼�ͨ������
	case MT_MCU_LOGICCHNNLOPENED_NTF:       //�նˣ�Э��ջ������MCU��ͨ���ɹ�������֪ͨ

	case MT_MCU_MEDIALOOPON_REQ:
	case MT_MCU_MEDIALOOPOFF_CMD:
	case MT_MCU_FREEZEPIC_CMD:
	case MT_MCU_FASTUPDATEPIC_CMD:
	case MT_MCU_FLOWCONTROL_CMD:			//������������
	case MT_MCU_FLOWCONTROLINDICATION_NOTIF://��������ָʾ
	case MT_MCU_CAPBILITYSET_NOTIF:			//�ն˷���MCU��������֪ͨ
	case MT_MCU_MSD_NOTIF:					//�ն����䷢��ҵ�������ȷ�������֪ͨ
	case MT_MCU_JOINEDMTLIST_REQ:			//�ն˷���MCU�Ļ�������ն��б��ѯ����
	case MT_MCU_JOINEDMTLISTID_REQ:			//�ն˷���MCU�Ļ�������ն��б���ID��ѯ����
	case MT_MCU_GETMTALIAS_REQ:				//��ѯ�ض��ն˱���
	case MT_MCU_MTALIAS_NOTIF:				//�ն˻㱨����
	case MT_MCU_DELAYCONF_REQ:				//��Ϣ��Ϊu16 ����
	case MT_MCU_GETMTSTATUS_REQ:			//�ն˷���MCU�Ĳ�ѯ������ĳ���ն�״̬����
	case MT_MCU_GETMTSTATUS_ACK:			//MT����Ӧ��MT->MCU
	case MT_MCU_GETMTSTATUS_NACK:			//MT�ܾ�Ӧ��MT->MCU
	case MT_MCU_MTSTATUS_NOTIF:				//MT״̬�ı�ʱ��������MCU״̬��Ϣ
	case MT_MCU_GETCONFINFO_REQ:			//�ն˷���MCU�Ĳ�ѯ����������Ϣ����
	case MT_MCU_MTCONNECTED_NOTIF:			//���ն˽������ӳɹ�֪ͨ
	case MT_MCU_MTJOINCONF_NOTIF:			//�ն˳ɹ����֪ͨ
	case MT_MCU_MTTYPE_NOTIF:               //�ն�����֪ͨ��320����ʱ�Ķ���֪ͨ��
	case MT_MCU_MTDISCONNECTED_NOTIF:		//���ն����ӶϿ�֪ͨ
	case MT_MCU_SPECCHAIRMAN_REQ:			//��ϯ�ն˷���MCU��ָ����ϯ����
	case MT_MCU_STARTMTSELME_REQ:			//�ն˷���MCU�������ն�ѡ���Լ�������
	case MT_MCU_STOPMTSELME_CMD:			//�ն˷���MCU��ֹͣ�����ն�ѡ���Լ���֪ͨ
	case MT_MCU_STARTBROADCASTMT_REQ:		//�ն˷���MCU������MCU�鲥�����ն˵�����
	case MT_MCU_STOPBROADCASTMT_CMD:		//�ն˷���MCU��ֹͣMCU�鲥�����ն˵�����
	case MT_MCU_GETMTSELSTUTS_REQ:			//�ն˷���MCU�Ĳ�ѯ�����ն�ѡ�����������
	case MT_MCU_MTMUTE_CMD:					//��ϯ�ն�����MCU�����ն˾�������
	case MT_MCU_MTDUMB_CMD:					//��ϯ�ն�����MCU�����ն���������
	case MT_MCU_MTCAMERA_CTRL_CMD:			//�����ն��������ͷ�˶�����
	case MT_MCU_MTCAMERA_CTRL_STOP:			//��ϯ�����ն�����ͷֹͣ�ƶ�
	case MT_MCU_MTCAMERA_RCENABLE_CMD:		//�����ն������ң����ʹ������
	case MT_MCU_MTCAMERA_SAVETOPOS_CMD:		//�����ն������Ҫ�󽫵�ǰλ����Ϣ����ָ��λ��
	case MT_MCU_MTCAMERA_MOVETOPOS_CMD:		//�����ն������Ҫ�������ָ��λ��
	// libo [4/4/2005]VideoSource
	case MT_MCU_SELECTVIDEOSOURCE_CMD:           //MTҪ��MCU�����ն���ƵԴ
	case MT_MCU_VIDEOSOURCESWITCHED_CMD:
	// libo [4/4/2005]end
   	case MT_MCU_STARTVAC_REQ:				//��ϯ����MCU��ʼ�����������Ʒ���		
   	case MT_MCU_STOPVAC_REQ:				//��ϯ����MCUֹͣ�����������Ʒ���
	case MT_MCU_STARTDISCUSS_REQ:			//��ʼ������������
	case MT_MCU_STOPDISCUSS_REQ:			//��ϯ����MCU������������
	case MT_MCU_STARTVMP_REQ:				//��ʼ��Ƶ��������
	case MT_MCU_STOPVMP_REQ:				//������Ƶ��������
	case MT_MCU_CHANGEVMPPARAM_REQ:			//��ϯ����MCU�ı���Ƶ���ϲ���
	case MT_MCU_GETVMPPARAM_REQ:			//��ѯ��Ƶ���ϳ�Ա����	
	case MT_MCU_STARTVMPBRDST_REQ:			//��ϯ����MCU��ʼ�ѻ���ϳ�ͼ��㲥���ն�
	case MT_MCU_STOPVMPBRDST_REQ:			//��ϯ����MCUֹͣ�ѻ���ϳ�ͼ��㲥���ն�
	case MT_MCU_STARTSWITCHVMPMT_REQ:		//��ϯѡ��VMP
	case MT_MCU_GETMTPOLLPARAM_ACK:			//MtӦ��MCU��ѯ�ն���ѯ����  
	case MT_MCU_GETMTPOLLPARAM_NACK:		//MT�ܾ�MCU��ѯ�ն���ѯ����   
	case MT_MCU_POLLMTSTATE_NOTIF:			//MT֪ͨMCU��ǰ���ն���ѯ״̬
	case MT_MCU_ADDMT_REQ:					//��ϯ�ն˷���MCU�������ն�����
	case MT_MCU_SENDMSG_CMD:				//�ն����ն˶���Ϣ����
	case MT_MCU_SENDMCMSG_CMD:				//�ն����������̨����Ϣ����	
    case MT_MCU_GETBITRATEINFO_ACK:         //mcu��ȡ�ն�����Ӧ��
    case MT_MCU_GETBITRATEINFO_NACK:
    case MT_MCU_GETBITRATEINFO_NOTIF:       
    case MT_MCU_MATRIX_ALLSCHEMES_NOTIF:    //���þ���
    case MT_MCU_MATRIX_ONESCHEME_NOTIF:
    case MT_MCU_MATRIX_SAVESCHEME_NOTIF:
    case MT_MCU_MATRIX_SETCURSCHEME_NOTIF:
    case MT_MCU_MATRIX_CURSCHEME_NOTIF:
    case MT_MCU_EXMATRIXINFO_NOTIFY:        //���þ���      
    case MT_MCU_EXMATRIX_GETPORT_ACK:        
    case MT_MCU_EXMATRIX_GETPORT_NACK:       
    case MT_MCU_EXMATRIX_GETPORT_NOTIF:
    case MT_MCU_EXMATRIX_PORTNAME_NOTIF:        
    case MT_MCU_EXMATRIX_ALLPORTNAME_NOTIF:   
    case MT_MCU_ALLVIDEOSOURCEINFO_NOTIF:   //��չ��ƵԴ
    case MT_MCU_VIDEOSOURCEINFO_NOTIF:
	case MT_MCU_ADDMIXMEMBER_CMD:           //��ϯ���ӻ����ն�
	case MT_MCU_REMOVEMIXMEMBER_CMD:		//��ϯ�Ƴ������ն� 
    case MT_MCU_VIEWBRAODCASTINGSRC_CMD:    //��ϯ�ն�ǿ�ƹ㲥
	case MT_MCU_APPLYMIX_NOTIF:             //�ն˷���MCU������μӻ�������
	case MT_MCU_CALLMTFAILURE_NOTIFY:       //�ն˸�MCU�ĺ���ʧ��֪ͨ
	case MT_MCU_GETH239TOKEN_REQ:           //�ն˸�MCU�� ��ȡ H239���� Ȩ������
	case MT_MCU_OWNH239TOKEN_NOTIF:         //�ն˸�MCU�� ӵ�� H239���� Ȩ��֪ͨ
	case MT_MCU_RELEASEH239TOKEN_NOTIF:     //�ն˸�MCU�� �ͷ� H239���� Ȩ��֪ͨ
    case MT_MCU_BANDWIDTH_NOTIF:            //�ն˴���ָʾ֪ͨ

    case MT_MCU_CONF_STOPCHARGE_ACK:        //��GK��ֹͣ�Ʒѳɹ�
    case MT_MCU_CONF_STOPCHARGE_NACK:       //��GK��ֹͣ�Ʒ�ʧ��
    case MT_MCU_CONF_STARTCHARGE_ACK:       //��GK�Ͽ�ʼ�Ʒѳɹ�
    case MT_MCU_CONF_STARTCHARGE_NACK:      //��GK�Ͽ�ʼ�Ʒ�ʧ��
	case MT_MCU_CONF_CHARGEEXP_NOTIF:		//��GK�ϼƷѵĻ��鷢���쳣֪ͨ
    case MT_MCU_CHARGE_REGGK_NOTIF:         //��GKע��״̬֪ͨ

//    case MT_MCU_GETMTVERID_ACK:             //��ȡ�ն˰汾�ųɹ�Ӧ��
    case MT_MCU_GETMTVERID_NACK:            //��ȡ�ն˰汾��ʧ��Ӧ��

    case MT_MCU_CALLFAIL_HDIFULL_NOTIF:

	case POLY_MCU_GETH239TOKEN_ACK:			//��ȡPolyMCU��H239TOKEN ͬ��Ӧ��
	case POLY_MCU_GETH239TOKEN_NACK:		//��ȡPolyMCU��H239TOKEN �ܾ�Ӧ��
	case POLY_MCU_OWNH239TOKEN_NOTIF:		//PolyMCU֪ͨ��ǰ��TOKEN��ӵ����
	case POLY_MCU_RELEASEH239TOKEN_CMD:		//PolyMCU�ͷ�H329TOKEN ����
		
	case MT_MCU_RELEASEMT_REQ:
    case MT_MCU_APPLYCANCELSPEAKER_REQ:
	case MT_MCU_MTVIDEOALIAS_NOTIF:         //�ն���ƵԴ����֪ͨ
    case MT_MCU_TRANSPARENTMSG_NOTIFY:
	case MTADP_MCU_GETMTLIST_NOTIF:
	case MTADP_MCU_GETSMCUMTALIAS_NOTIF:	//����¼�mcu�ն˱���֪ͨ

	case REC_MCU_STARTREC_ACK:				//��ʼ¼��Ack
	case REC_MCU_STARTREC_NACK:				//��ʼ¼��Nack
	case REC_MCU_RECORDCHNSTATUS_NOTIF:		// ¼��״̬�ϱ�
	case REC_MCU_RECORDPROG_NOTIF:			//¼������ϱ�
	case REC_MCU_LISTALLRECORD_ACK:			//��ȡ�ļ��б�Ack
	case REC_MCU_LISTALLRECORD_NACK:		//��ȡ�ļ��б�Nack
	case REC_MCU_FILEGROUPLIST_NOTIF:		// ��Ŀ���б��ϱ�
	case REC_MCU_LISTALLRECORD_NOTIF:		// �ļ����б��ϱ�
	case REC_MCU_STARTPLAY_ACK:				//��ʼ¼��Ack
	case REC_MCU_STARTPLAY_NACK:			//��ʼ¼��Nack
	case REC_MCU_PLAYCHNSTATUS_NOTIF:		// ����״̬�ϱ�
	case REC_MCU_PLAYPROG_NOTIF:			//��������ϱ�
	case REC_MCU_SEEK_ACK:					// ��������϶�Ack
	case REC_MCU_SEEK_NACK:				// ��������϶�Nack

		ProcMtAdpMcuMsg( pcMsg );
		MT_EV_LOG(LOG_LVL_DETAIL);
		break;

	case MCU_MT_CREATECONF_ACK:				//MCU����MT�Ĵ�������Ӧ��
	case MCU_MT_CREATECONF_NACK:			//MCU�ܾ�MT�Ĵ�����������
	case MCU_MT_INVITEMT_REQ:				//MCU�����ն˼���ָ����������
	case MCU_MT_DELMT_CMD:					//MCUǿ���ն��˳���������
	case MCU_MT_DELMT_ACK:					//MCU�صĳɹ�Ӧ��
	case MCU_MT_DELMT_NACK:					//MCU�ص�ʧ��Ӧ��
	case MCU_MT_MTJOINED_NOTIF:				//�ն˼������֪ͨ
	case MCU_MT_MTLEFT_NOTIF:				//�ն��뿪����֪ͨ
	case MCU_MT_APPLYCHAIRMAN_ACK:			//�ն˷���MCU��������ϯ����ɹ�Ӧ��
	case MCU_MT_APPLYCHAIRMAN_NACK:			//�ն˷���MCU��������ϯ����ʧ��Ӧ��
	case MCU_MT_CANCELCHAIRMAN_NOTIF:		//MCU�����ն˵�ȡ����ϯ����
	case MCU_MT_CANCELCHAIRMAN_ACK:			//MCU->MT
	case MCU_MT_CANCELCHAIRMAN_NACK:
	case MCU_MT_GETCHAIRMAN_ACK:			//��ѯ��ϯIDӦ��
	case MCU_MT_GETCHAIRMAN_NACK:
	case MCU_MT_SPECSPEAKER_ACK:			//MCU�صĳɹ�Ӧ��
	case MCU_MT_SPECSPEAKER_NACK:			//MCU�ص�ʧ��Ӧ��
	case MCU_MT_SPECSPEAKER_NOTIF:			//MCU�����ն˵�ָ������֪ͨ
	case MCU_MT_CANCELSPEAKER_NOTIF:		//MCU�����ն˵�ȡ����������
	case MCU_MT_APPLYSPEAKER_NOTIF:			//֪ͨ��ϯ�ն����뷢������
	case MCU_MT_STARTSELMT_ACK:
	case MCU_MT_STARTSELMT_NACK:
	case MCU_MT_YOUARESEEING_NOTIF:			//MCU��֪�ն˵�ǰ��ƵԴ
	case MCU_MT_SEENBYOTHER_NOTIF:			//MCU֪ͨ�ն˱�ѡ��
	case MCU_MT_CANCELSEENBYOTHER_NOTIF:	//MCU֪ͨ�ն˱�ȡ��ѡ��
	case MCU_MT_OPENLOGICCHNNL_REQ:			//MCU����MT�Ĵ��߼�ͨ������
	case MCU_MT_OPENLOGICCHNNL_ACK:			//MCU�����ն˵ĳɹ�Ӧ��
	case MCU_MT_OPENLOGICCHNNL_NACK:		//MCU�����ն˵�ʧ��Ӧ��
	case MCU_MT_CLOSELOGICCHNNL_CMD:		//MCU����MT�Ĺر��߼�ͨ������
	case MCU_MT_CHANNELON_CMD:				//MCU�����ն˵�ChannelOn��Ϣ
	case MCU_MT_CHANNELOFF_CMD:				//MCU�����ն˵�ChannelOff��Ϣ
	case MCU_MT_MEDIALOOPON_ACK:			//�ն�Զ�˻���ack
	case MCU_MT_MEDIALOOPON_NACK:			//�ն�Զ�˻���nack
	case MCU_MT_FREEZEPIC_CMD:				//MCU�����ն˶���ͼ������
	case MCU_MT_FASTUPDATEPIC_CMD:			//MCU�����ն˿��ٸ���ͼ������
	case MCU_MT_FLOWCONTROL_CMD:
	case MCU_MT_FLOWCONTROLINDICATION_NOTIF:
    case MCU_MT_GETBITRATEINFO_REQ:         //mcu�����ն�������Ϣ
	case MCU_MT_ROUNDTRIPDELAY_REQ:			//MCU����MT��RoundTripDelay����
	case MCU_MT_ROUNDTRIPDELAY_ACK:			//MCU�����ն˵ĳɹ�Ӧ��
	case MCU_MT_CAPBILITYSET_NOTIF:			//MCU�����ն˵�������֪ͨ
	case MCU_MT_JOINEDMTLIST_ACK:			//MCU���سɹ����
	case MCU_MT_JOINEDMTLIST_NACK:			//MCU����ʧ�ܽ��
	case MCU_MT_JOINEDMTLISTID_ACK:			//MCU���سɹ����
	case MCU_MT_JOINEDMTLISTID_NACK:
	case MCU_MT_GETMTALIAS_ACK:				//��ѯ�ض��ն˱���Ӧ����Ϣ��TMt+TMtAlias
	case MCU_MT_GETMTALIAS_NACK:
	case MCU_MT_DELAYCONF_ACK:
	case MCU_MT_DELAYCONF_NACK:
	case MCU_MT_DELAYCONF_NOTIF:			//��Ϣ��Ϊu16 ����
	case MCU_MT_CONFWILLEND_NOTIF:
	case MCU_MT_GETMTSTATUS_ACK:			//MCU�صĳɹ�Ӧ��
	case MCU_MT_GETMTSTATUS_NACK:			//MCU�ص�ʧ��Ӧ��
	case MCU_MT_GETMTSTATUS_REQ:			//MCU�����ն˲�ѯ״̬����
	case MCU_MT_GETCONFINFO_ACK:			//MCU�صĳɹ�Ӧ��
	case MCU_MT_GETCONFINFO_NACK:			//MCU�ص�ʧ��Ӧ��
	case MCU_MT_CONF_NOTIF:					//������Ϣ֪ͨ
	case MCU_MT_SIMPLECONF_NOTIF:           //�򻯻�����Ϣ֪ͨ 2005-10-11
	case MCU_MT_SPECCHAIRMAN_ACK:			//MCU�صĳɹ�Ӧ��
	case MCU_MT_SPECCHAIRMAN_NACK:			//MCU�ص�ʧ��Ӧ��
	case MCU_MT_APPLYCHAIRMAN_NOTIF:		//֪ͨ��ϯ�ն�������ϯ����
    case MCU_MT_APPLYCHAIRMAN_CMD:			//MCU��������MCU������ϯ��֪ͨ(��ӦԶң�������⴦��)
	case MCU_MT_CANCELCHAIRMAN_CMD:			//mcu��������mcuȡ����������
	case MCU_MT_SPECCHAIRMAN_NOTIF:			//MCU�����ն˵�ָ����ϯ֪ͨ
	case MCU_MT_STARTMTSELME_ACK:			//MCU�����ն˵������ն�ѡ���Լ�������Ӧ��
	case MCU_MT_STARTMTSELME_NACK:			//MCU�����ն˵������ն�ѡ���Լ��ľܾ�
	case MCU_MT_GETMTSELSTUTS_ACK:			//MCU�����ն˵Ĳ�ѯ�����ն�ѡ�������Ӧ��
	case MCU_MT_GETMTSELSTUTS_NACK:			//MCU�����ն˵Ĳ�ѯ�����ն�ѡ������ĵľܾ�
	case MCU_MT_MTMUTE_CMD:					//MCU�����ն�ĳһ����ʵ�����ն˾�������
	case MCU_MT_MTDUMB_CMD:					//MCU�����ն�ĳһ����ʵ�����ն˽�����������
	case MCU_MT_MTCAMERA_CTRL_CMD:			//�����ն��������ͷ�˶�����
	case MCU_MT_MTCAMERA_CTRL_STOP:			//�����ն��������ͷֹͣ�˶�����
	case MCU_MT_MTCAMERA_RCENABLE_CMD:		//�����ն������ң����ʹ������
	case MCU_MT_MTCAMERA_SAVETOPOS_CMD:		//�����ն������Ҫ�󽫵�ǰλ����Ϣ����ָ��λ��
	case MCU_MT_MTCAMERA_MOVETOPOS_CMD:		//�����ն������Ҫ�������ָ��λ��
	case MCU_MT_SETMTVIDSRC_CMD:            //�����ն˵�ѡ����ƵԴ����
    
	case MCU_MT_VIDEOSOURCESWITCHED_CMD:

  	case MCU_MT_STARTVAC_ACK:				//MCUͬ����ϯ��ʼ�����������Ʒ��Ե�����   
   	case MCU_MT_STARTVAC_NACK:				//MCU�ܾ���ϯ��ʼ�����������Ʒ��Ե�����   
   	case MCU_MT_STARTVAC_NOTIF:				//MCU����ϯ��ʼ�����������Ʒ��Ե�֪ͨ
   	case MCU_MT_STOPVAC_ACK:				//MCUͬ����ϯֹͣ�����������Ʒ��Ե�����  
   	case MCU_MT_STOPVAC_NACK:				//MCU�ܾ���ϯֹͣ�����������Ʒ��Ե�����  
   	case MCU_MT_STOPVAC_NOTIF:				//MCU����ϯֹͣ�����������Ʒ��Ե�֪ͨ
	case MCU_MT_STARTDISCUSS_ACK:			//MCUͬ�⿪ʼ��������	
	case MCU_MT_STARTDISCUSS_NACK:			//MCU�ܾ���ʼ��������	
	case MCU_MT_STARTDISCUSS_NOTIF:			//MCU��ʼ��������֪ͨ	
	case MCU_MT_STOPDISCUSS_ACK:			//MCUͬ����ϯ�����������۵�����	
	case MCU_MT_STOPDISCUSS_NACK:			//MCU�ܾ���ϯ�����������۵�����	
	case MCU_MT_STOPDISCUSS_NOTIF:			//MCU����ϯ�����������۵�֪ͨ	
	case MCU_MT_STARTVMP_ACK:				//MCUͬ����Ƶ��������	
	case MCU_MT_STARTVMP_NACK:				//MCU��ͬ����Ƶ��������	
	case MCU_MT_STARTVMP_NOTIF:				//MCU��ʼ��Ƶ���ϵ�֪ͨ	
	case MCU_MT_STOPVMP_ACK:				//MCUͬ����Ƶ������������	
	case MCU_MT_STOPVMP_NACK:				//MCU��ͬ�������Ƶ��������	
	case MCU_MT_STOPVMP_NOTIF:				//MCU������Ƶ���ϵ�֪ͨ	
	case MCU_MT_CHANGEVMPPARAM_ACK:			//MCUͬ����ϯ�ĸı���Ƶ���ϲ�������	
	case MCU_MT_CHANGEVMPPARAM_NACK:		//MCU�ܾ���ϯ�ĸı���Ƶ���ϲ�������	
	case MCU_MT_GETVMPPARAM_ACK:			//MCUͬ���ѯ��Ƶ���ϳ�Ա	
	case MCU_MT_GETVMPPARAM_NACK:			//MCU��ͬ���ѯ��Ƶ���ϳ�Ա	
	case MCU_MT_VMPPARAM_NOTIF:				//MCU����ϯ����Ƶ���ϲ���֪ͨ	
	case MCU_MT_STARTVMPBRDST_ACK:			//MCUͬ��㲥��Ƶ��������, ��Ϣ����	
	case MCU_MT_STARTVMPBRDST_NACK:			//MCU�ܾ��㲥��Ƶ��������, ��Ϣ����	
	case MCU_MT_STOPVMPBRDST_ACK:			//MCUͬ��ֹͣ�㲥��Ƶ��������, ��Ϣ����	
	case MCU_MT_STOPVMPBRDST_NACK:			//MCU�ܾ�ֹͣ�㲥��Ƶ��������, ��Ϣ����
	case MCU_MT_STARTMTPOLL_CMD:			//MCU������ն˿�ʼ��ѯѡ��  
	case MCU_MT_STOPMTPOLL_CMD:				//MCU������ն�ֹͣ��ѯѡ��  
	case MCU_MT_PAUSEMTPOLL_CMD:			//MCU������ն���ͣ��ѯѡ��   
	case MCU_MT_RESUMEMTPOLL_CMD:			//MCU������ն˼�����ѯѡ��
	case MCU_MT_GETMTPOLLPARAM_REQ:			//MCU���ն˲�ѯ��ѯ����
	case MCU_MT_ADDMT_ACK:					//MCU�صĳɹ�Ӧ��
	case MCU_MT_ADDMT_NACK:					//MCU�ص�ʧ��Ӧ��
	case MCU_MT_SENDMSG_NOTIF:				//���ն˶���Ϣ֪ͨ
	case MCU_MT_CHANGESPEAKER_NOTIF:        //MCU֪ͨ�ն˷����˸�
	case MCU_MT_CHANGECHAIRMAN_NOTIF:		//MCU֪ͨ�ն���ϯ�ı�
	case MCU_MT_REGISTERGK_REQ:             //��GK��ע��
	case MCU_MT_UNREGISTERGK_REQ:           //��GK��ע��	
	case MCU_MT_MATRIX_SAVESCHEME_CMD:		//������󷽰�����
    case MCU_MT_MATRIX_GETALLSCHEMES_CMD:
    case MCU_MT_MATRIX_GETONESCHEME_CMD:
    case MCU_MT_MATRIX_SETCURSCHEME_CMD:
    case MCU_MT_MATRIX_GETCURSCHEME_CMD:
    case MCU_MT_EXMATRIX_GETINFO_CMD:
    case MCU_MT_EXMATRIX_SETPORT_CMD:
    case MCU_MT_EXMATRIX_GETPORT_REQ:
    case MCU_MT_EXMATRIX_SETPORTNAME_CMD:
    case MCU_MT_EXMATRIX_GETALLPORTNAME_CMD:        
    case MCU_MT_GETVIDEOSOURCEINFO_CMD:    //�ն���չ��ƵԴ   
    case MCU_MT_SETVIDEOSOURCEINFO_CMD:       
    case MCU_MT_SELECTEXVIDEOSRC_CMD:
	case MCU_MT_STARTBROADCASTMT_ACK:       //MCU�����ն˵��鲥�����ն˵���Ӧ
	case MCU_MT_STARTBROADCASTMT_NACK:      //MCU�����ն˵��鲥�����ն˵���Ӧ
	case MCU_MT_MTSOURCENAME_NOTIF:         //�ն���ƵԴ����ָʾ
	case MCU_MT_MTAPPLYMIX_NOTIF:           //֪ͨ��ϯ�ն������ն�����μӻ�������
	case MCU_MT_MTADDMIX_NOTIF:             //֪ͨ�ն��ѳɹ��μӻ���
	case MCU_MT_DISCUSSPARAM_NOTIF:			//֪ͨ�ն˶��ƻ���״̬ // [11/25/2010 xliang] for MT of T3, R3 type
	case MCU_MT_GETH239TOKEN_ACK:           //MCU���ն˵� ��ȡH239����Ȩ������� ��Ӧ֪ͨ
	case MCU_MT_GETH239TOKEN_NACK:          //
	case MCU_MT_RELEASEH239TOKEN_CMD:       //MCU���ն˵� �ͷ�H239���� Ȩ������
	case MCU_MT_OWNH239TOKEN_NOTIF:         //MCU���ն˵� ӵ�� H239���� Ȩ��֪ͨ
	case MCU_MT_CONFPWD_NTF:                //MCUͬ���ն������û���Ļ������룬���ںϲ������������֤У��
        
//    case MCU_MT_SATDCONFADDR_NOTIF:       //���ǻ����鲥��ַ֪ͨ
    case MCU_MT_SATDCONFCHGADDR_CMD:        //���ǻ����ն˽��յ�ַ�ı�����
    case MCU_MT_VIDEOPARAMCHANGE_CMD:       //�ն���Ƶ�������� xliang [080731]
	case MCU_MT_VIDEOFPSCHANGE_CMD:         //�ն�֡�ʵ��� pengjie [20100422]

    case MCU_MT_CONF_STARTCHARGE_REQ:       //��ؿ�ʼ�Ʒ�����
    case MCU_MT_CONF_STOPCHARGE_REQ:        //��ؽ����Ʒ�����

    case MCU_MT_UNREGGK_NPLUS_CMD:          //N+1�����£�����MCUע����MCUʵ��
    case MCU_MT_CONF_STOPCHARGE_CMD:        //N+1�����£�����MCUֹͣ��MCU����Ʒ�
    case MCU_MT_SETMTVOLUME_CMD:            //�����ն��������� add by wangliang 2006/12/26
    case MCU_MT_RESTORE_MCUE164_NTF:        //N+1�����£�����MCUע�� ��MCU.E164
        
    case MCU_MT_SOMEOFFERRING_NOTIF:        //MCU�����ն˵� ͨ��֪ͨ
    case MCU_MT_GETMTVERID_REQ:             //MCU�����ն˵Ļ�ȡ�ն˰汾������
	
	case MCU_POLY_GETH239TOKEN_REQ:			//KEDAMCU��PolyMCU��������ȡPolyMCU��H239TOKEN����
	case MCU_POLY_OWNH239TOKEN_NOTIF:		//KEDAMCU��PolyMCU������֪ͨPolyMCU H239TOKENΪ��ǰMCU����
	case MCU_POLY_RELEASEH239TOKEN_CMD:		//KEDAMCU��PolyMCU������KEDAMCUֹͣ˫���� ����

	case MCU_MT_STARTSWITCHVMPMT_ACK:		//��ϯѡ��VMPӦ��
	case MCU_MT_STARTSWITCHVMPMT_NACK:

	case MCU_MT_RELEASEMT_ACK:
	case MCU_MT_RELEASEMT_NACK:

	case MCU_MT_MMCUCONFNAMESHOWTYPE_CMD:
	case MCU_MT_CHANGEADMINLEVEL_CMD:

	case MCU_MT_MTSPEAKSTATUS_NTF:          //MCU�����ն� ������״̬
    case MCU_MT_CONFSPEAKMODE_NTF:          //MCU�����ն� ���鷢��ģʽ ֪ͨ

    case MCU_MT_APPLYCANCELSPEAKER_ACK:
    case MCU_MT_APPLYCANCELSPEAKER_NACK:

	case MCU_MT_GETMTVIDEOALIAS_CMD:        //��ȡ�ն���ƵԴ������Ϣ
	case MCU_MTADP_GETMTLIST_CMD:
	case MCU_MTADP_GETSMCUMTALIAS_CMD:		//׷�ӻ���¼�ָ���ն˱�������

    case MCU_MT_CHANGEMTSECVIDSEND_CMD:     //[5/4/2011 zhushengze]VCS���Ʒ����˷�˫��
    case MCU_MT_TRANSPARENTMSG_NOTIFY:      //[2/23/2012 zhushengze]���桢�ն���Ϣ͸��
	// vrs��¼�������Ϣ֧��
	case MCU_REC_STARTREC_REQ:				//��ʼ¼��
	case MCU_REC_LISTALLRECORD_REQ:			//��ȡ�ļ��б�
	case MCU_REC_STARTPLAY_REQ:				//��������
	case MCU_REC_SEEK_REQ:					//��������϶�
	case MCU_REC_GETRECORDCHNSTATUS_REQ:	//MCU������¼���¼��״̬����
	case MCU_REC_GETPLAYCHNSTATUS_REQ:		//MCU������¼�������״̬����
	case MCU_REC_GETRECPROG_CMD:			//��ȡ¼�����
	case MCU_REC_GETPLAYPROG_CMD:			//��ȡ�������

		MT_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcuMtAdpMsg( pcMsg );
		break;

	case MCU_MT_ENTERPASSWORD_REQ:			//MCU�����ն���������		
	case MT_MCU_ENTERPASSWORD_ACK:			//�ն˻�Ӧ����
	case MT_MCU_ENTERPASSWORD_NACK:
		{
			MT_EV_LOG(LOG_LVL_DETAIL);
			if ( AID_MCU_MTADP == GETAPP( pcMsg->srcid ) )	
			{
				//��mcu�����˻�������ʱҪ��ֱ��mcu���������Ӧ
				ProcMtAdpMcuMsg( pcMsg );
			}
			else
			{
				//���¼�mcu�����˻�������ʱҪ���ϼ�mcu���������Ӧ
				ProcMcuMtAdpMsg( pcMsg );
			}
			break;	
		}

//	case MCU_MT_REREGISTERGK_REQ:           //ԭ��GKע���323�������ߺ����²���һ��323�����ע��
	case MCU_MT_UPDATE_REGGKSTATUS_NTF:     //��������������MCU������ʵ���GKע����Ϣ��֪ͨ
	case MCU_MT_UPDATE_GKANDEPID_NTF:       //��������������GK��ע���GatekeeperID/EndpointID��Ϣ��֪ͨ
	case MCU_MTADP_GKADDR_UPDATE_CMD:		//GK��ַ���ø���
		MT_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcuMtAdpGKMsg( pcMsg );
		break;
        
    case MCU_MT_MTADPCFG_CMD:        
    case MCU_MT_SETQOS_CMD:
        {
			MT_EV_LOG(LOG_LVL_KEYSTATUS);
            CServMsg cMsg(pcMsg->content, pcMsg->length);
            SendMsgToH323MtAdp(cMsg);
        }        
        break;    
		
	default:
		LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "MtAdpSsn[InstanceEntry]: Wrong message %u(%s) received in InstanceEntry()!\n", pcMsg->event, 
			::OspEventDesc( pcMsg->event ) );
		break;
	}

}

/*====================================================================
    ������      ��DaemonInstanceEntry
    ����        ��ʵ����Ϣ������ں���������override
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
				  CApp* pcApp
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/05/29    1.0         
====================================================================*/
void CMtAdpSsnInst::DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp )
{
	if (NULL == pcMsg)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "MtAdpSsn[DaemonInstanceEntry]: The received msg's pointer in the msg entry is NULL!");
		return;
	}

	switch (pcMsg->event)
	{
	case MCU_APPTASKTEST_REQ:			//GUARD Probe Message
		DaemonProcAppTaskRequest( pcMsg );
		break;
	default:
		LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "MtAdpSsn[DaemonInstanceEntry]: Wrong message %u(%s) received! AppId = %u\n", pcMsg->event, 
			::OspEventDesc( pcMsg->event ),pcApp!=NULL?pcApp->appId:0 );
		break;
	}
	
	return;
}

/*====================================================================
    ������      ��DaemonProcAppTaskRequest
    ����        ��GUARDģ��̽����Ϣ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/05/29    1.0         
====================================================================*/
void CMtAdpSsnInst::DaemonProcAppTaskRequest( const CMessage * pcMsg )
{
	post( pcMsg->srcid, MCU_APPTASKTEST_ACK, pcMsg->content, pcMsg->length );

	return;
}

/*====================================================================
    ������      : SendMsgToMtAdp
    ����        ������Ϣ��MtAdp
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����wEvent ��ϢID
				  pbyMsg ��Ϣ��
                  wLen   ��Ϣ����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
void CMtAdpSsnInst::SendMsgToMtAdp(u16 wEvent, u8 * const pbyMsg, u16 wLen)
{
    LogPrint(LOG_LVL_DETAIL, MID_MCU_MT, "[SendMsgToMtAdp]: Send Msg %u(%s) to MTAdpId%u Protocol%u MaxMtNum%u m_dwMtAdpAppIId.%d m_dwMtAdpNode.%d!\n", 
              wEvent, ::OspEventDesc(wEvent), m_byMtAdpId, m_byProtocolType, m_byMaxMtNum, m_dwMtAdpAppIId, m_dwMtAdpNode);

    post(m_dwMtAdpAppIId, wEvent, pbyMsg, wLen, m_dwMtAdpNode);
}

/*====================================================================
    ������      : SendMsgToMcu
    ����        ������Ϣ��Mcu
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����wEvent ��ϢID
				  pbyMsg ��Ϣ��
                  wLen   ��Ϣ����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
void CMtAdpSsnInst::SendMsgToMcu( u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
    CServMsg cServMsg( pbyMsg, wLen );

	if( wEvent == MT_MCU_CREATECONF_REQ ||
        wEvent == MT_MCU_LOWERCALLEDIN_NTF ||
		wEvent == MTADP_MCU_REGISTER_REQ ||
		wEvent == MCU_MTADP_DISCONNECTED_NOTIFY ||
        wEvent == MT_MCU_REGISTERGK_NACK ||
        wEvent == MT_MCU_REGISTERGK_ACK ||
        wEvent == MT_MCU_UNREGISTERGK_ACK ||
        wEvent == MT_MCU_UNREGISTERGK_NACK ||
        wEvent == MT_MCU_CONF_STOPCHARGE_ACK ||
        wEvent == MT_MCU_CONF_STOPCHARGE_NACK ||
        wEvent == MT_MCU_CONF_STARTCHARGE_ACK ||
        wEvent == MT_MCU_CONF_STARTCHARGE_NACK ||
		wEvent == MT_MCU_CONF_CHARGEEXP_NOTIF ||
        wEvent == MT_MCU_CHARGE_REGGK_NOTIF ||
		wEvent == MT_MCU_CALLFAIL_HDIFULL_NOTIF ) // xliang [10/31/2008] 
	{
		g_cMcuVcApp.SendMsgToDaemonConf( wEvent, pbyMsg, wLen );
	}		
	else
	{
		g_cMcuVcApp.SendMsgToConf( cServMsg.GetConfId(), wEvent, pbyMsg, wLen );
	}
}

/*=============================================================================
  �� �� ���� SendMsgToH323MtAdp
  ��    �ܣ� ����Ϣ��H323MtAdp
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMtAdpSsnInst::SendMsgToH323MtAdp( CServMsg &cMsg )
{
    if(PROTOCOL_TYPE_H323 == m_byProtocolType)
    {
        SendMsgToMtAdp(cMsg.GetEventId(), cMsg.GetServMsg(), cMsg.GetServMsgLen());
    }

	return;
}

/*====================================================================
    ������      : ProcMcuCasCadeMsg
    ����        ������mcu������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����pcMsg ��Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
void CMtAdpSsnInst::ProcMcuCasCadeMsg( CMessage * const pcMsg )
{
	u16 wAppId = GETAPP( pcMsg->srcid );

	switch(CurState()) 
	{
	case STATE_NORMAL:
		
		if( AID_MCU_MTADP == wAppId )
		{
			SendMsgToMcu( pcMsg->event, pcMsg->content, pcMsg->length );
		}
		else
		{
			SendMsgToMtAdp( pcMsg->event, pcMsg->content, pcMsg->length );
		}

		break;
	default:
		break;
	}
}


/*====================================================================
    ������      : ProcMtAdpRegisterReq
    ����        ������MtAdp��ע����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����pcMsg ��Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
void CMtAdpSsnInst::ProcMtAdpRegisterReq( CMessage * const pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMtAdpReg tMtAdpReg =  *( TMtAdpReg* )cServMsg.GetMsgBody();
	TMtAdpRegRsp tMtAdpRegRsp;
    s8 achMcuAlias[MAXLEN_ALIAS] = {0};
	memset( &tMtAdpRegRsp, 0, sizeof(tMtAdpRegRsp) );

	u32 dwSysSSrc = g_cMSSsnApp.GetMSSsrc();
	
	switch( CurState() ) 
	{
	case STATE_IDLE:
    //zbq[12/11/2007]״̬���ϸ�ת
	//case STATE_NORMAL: 
        {	
        if (cServMsg.GetMsgBodyLen() != sizeof(TMtAdpReg)
			&& cServMsg.GetMsgBodyLen() != (sizeof(TMtAdpReg) + IPV6_STR_LEN) )
        {
            LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "CMtAdpSsnInst: invalid msg body len:%d, (should %d) nack.\n", 
                cServMsg.GetMsgBodyLen(), sizeof(TMtAdpReg));
            post( pcMsg->srcid, pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(),  pcMsg->srcnode );
            ::OspDisconnectTcpNode( pcMsg->srcnode );
			// ����mtadp��linux�಻����������ʱ�� [pengguofeng 5/18/2012]
            return;
        }
        
		if( tMtAdpReg.GetDriId() != GetInsID() )
		{
			LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "CMtAdpSsnInst: MtAdp's DriId: %d not equal to InsId: %d, nack.\n",tMtAdpReg.GetDriId(), GetInsID());
			post( pcMsg->srcid, pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(),  pcMsg->srcnode );
			::OspDisconnectTcpNode( pcMsg->srcnode );
			return;
		}
		//Ŀǰ��ʱֻ֧��H323/H320,�Ժ����SIP֧��
		if( tMtAdpReg.GetProtocolType() > PROTOCOL_TYPE_H320 )
		{
			LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "CMtAdpSsnInst: MtAdp's ProtocolType: %d not support, nack.\n",tMtAdpReg.GetDriId() );
			post( pcMsg->srcid, pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(),  pcMsg->srcnode );
			::OspDisconnectTcpNode( pcMsg->srcnode );
			return;
		}
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)// [3/1/2010 xliang] remove check below for 8000E
		//��֤,�Ƿ��Ǳ�MCU�ĵ���
// 		if( tMtAdpReg.GetIpAddr() != ntohl(g_cMcuAgent.GetBrdIpAddr(tMtAdpReg.GetDriId())) )
// 		{
// 			//NACK
// 			LogPrint( LOG_LVL_ERROR, MID_MCU_MT, "CMtAdpSsnInst: other mcu board mtadp[Ip= %0x, Id= %d] register while agent info[Ip = %0x], nack!\n", 
//                                     tMtAdpReg.GetIpAddr(), tMtAdpReg.GetDriId(), 
//                                     ntohl(g_cMcuAgent.GetBrdIpAddr(tMtAdpReg.GetDriId())) );
// 			SendMsgToMtAdp( pcMsg->event+2, pcMsg->content, pcMsg->length );
// 			::OspDisconnectTcpNode( pcMsg->srcnode );
// 			return;
// 		}
#endif
		u8 byIpType = IP_NONE;
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcMtAdpRegisterReq]MsgLen:%d TMtAdp_Size:%d IPV6_len:%d\n",
			cServMsg.GetMsgBodyLen(), sizeof(tMtAdpReg), IPV6_STR_LEN);
		if ( tMtAdpReg.GetVersion() == DEVVER_MTADPV6 )
		{
			//[pengguofeng 5/11/2012]��֤��Ϣ���ȣ�����ȷ���NACK 
			if ( cServMsg.GetMsgBodyLen() != sizeof(tMtAdpReg) + sizeof(u8)*IPV6_STR_LEN 
				&& cServMsg.GetMsgBodyLen() != sizeof(tMtAdpReg) )
			{
				cServMsg.SetErrorCode(ERR_MCU_VER_UNMATCH);
				post(pcMsg->srcid, pcMsg->event + 2,
					cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);
				
				LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "MtadpSsn: Mtadp.%d(0x%x) version %d differs from MCU %d, NACK!\n",                       
					GetInsID(),
					tMtAdpReg.GetIpAddr(),
					tMtAdpReg.GetVersion(),
					DEVVER_MTADPV6 );
				
				
				// �����ã�����
				if (tMtAdpReg.GetIpAddr() != ntohl(g_cMcuAgent.GetMpcIp()) )
				{
					::OspDisconnectTcpNode(pcMsg->srcnode);
				}
				return;
			}

			//get IpV6
			u8	abyIpV6[IPV6_STR_LEN] = {0};
			u8  abyIpV6Null[IPV6_STR_LEN];
			memset(abyIpV6Null, 0, sizeof(abyIpV6Null));
			memcpy(abyIpV6, cServMsg.GetMsgBody() + sizeof(TMtAdpReg), sizeof(abyIpV6));
			
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT,"[ProcMtAdpRegisterReq]get Ipv6 from Msg:%s\n", abyIpV6);
			if ( memcmp(abyIpV6, abyIpV6Null, sizeof(abyIpV6)) != 0 )
			{
				//is IPV6
				if ( tMtAdpReg.GetIpAddr() != 0 )
				{
					//BOTH
					byIpType = IP_V4V6;
				}
				else
					byIpType = IP_V6;
			}
			else if ( tMtAdpReg.GetIpAddr() != 0)
			{
				byIpType = IP_V4;
			}
		} else
		// �汾��֤ʧ�ܣ��ܾ�ע��, zgc, 2007-09-28
        if ( tMtAdpReg.GetVersion() != 0 &&
             tMtAdpReg.GetVersion() != DEVVER_MTADP )
        {
            cServMsg.SetErrorCode(ERR_MCU_VER_UNMATCH);
            post(pcMsg->srcid, pcMsg->event + 2,
                 cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);

            LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "MtadpSsn: Mtadp.%d(0x%x) version %d differs from MCU %d, NACK!\n",                       
                      GetInsID(),
                      tMtAdpReg.GetIpAddr(),
                      tMtAdpReg.GetVersion(),
                      DEVVER_MTADP );


            // �����ã�����
            if (tMtAdpReg.GetIpAddr() != ntohl(g_cMcuAgent.GetMpcIp()) )
            {
                ::OspDisconnectTcpNode(pcMsg->srcnode);
            }
            return;
        }

		//[pengguofeng 5/12/2012]�ϰ汾���V4
		if ( tMtAdpReg.GetVersion() == DEVVER_MTADP )
		{
			byIpType = IP_V4;
		}

		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[MtAdpSsn]: MtAdp %d {Version:%d IPtype:%d} register to mcu success!\n",
			tMtAdpReg.GetDriId(), tMtAdpReg.GetVersion(), byIpType);
		
		//����ʵ������
		m_byMtAdpId         = tMtAdpReg.GetDriId();
		m_byMtAdpAttachMode = tMtAdpReg.GetAttachMode();
		m_byProtocolType    = tMtAdpReg.GetProtocolType() ;
		m_byMaxMtNum        = tMtAdpReg.GetMaxMtNum();
		m_dwMtAdpNode       = pcMsg->srcnode;
		m_dwMtAdpAppIId     = pcMsg->srcid;
	   
		//ע�����֪ͨ
		::OspNodeDiscCBRegQ( pcMsg->srcnode, GetAppID(), GetInsID() );       

        TLocalInfo tLocalInfo;
        if(SUCCESS_AGENT != g_cMcuAgent.GetLocalInfo(&tLocalInfo))
        {
            LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[MtAdpSsn]: get local cfg info failed!\n" );
            tMtAdpRegRsp.SetHeartBeatInterval( DEF_DISCCHECKTIME );
            tMtAdpRegRsp.SetHeartBeatFailTimes( DEF_DISCCHECKTIMES );
        }
        else
        {
            tMtAdpRegRsp.SetHeartBeatInterval( tLocalInfo.GetCheckTime() );
            tMtAdpRegRsp.SetHeartBeatFailTimes( (u8)tLocalInfo.GetCheckTimes() );

            TMtAdpCfg tCfg;
            tCfg.SetAudInfoRefreshTime(tLocalInfo.GetAudioRefreshTime());
            tCfg.SetVidInfoRefreshTime(tLocalInfo.GetVideoRefreshTime());
            tCfg.SetPartListRefreshTime(tLocalInfo.GetListRefreshTime());
            
            CServMsg cCfgMsg;
            cCfgMsg.SetEventId(MCU_MT_MTADPCFG_CMD);
            cCfgMsg.SetMsgBody((u8*)&tCfg, sizeof(tCfg));
            SendMsgToH323MtAdp(cCfgMsg);
        }

		//Ӧ��ע������
		tMtAdpRegRsp.SetVcIp( ntohl( g_cMcuAgent.GetMpcIp() ) );
		tMtAdpRegRsp.SetMcuNetId( LOCAL_MCUID);
		tMtAdpRegRsp.SetGkIp( ntohl( g_cMcuAgent.GetGkIpAddr() ) );
		tMtAdpRegRsp.SetH225H245Port( g_cMcuAgent.Get225245StartPort() );
		tMtAdpRegRsp.SetH225H245MtNum( g_cMcuAgent.Get225245MtNum() );

        tMtAdpRegRsp.SetMcuVersion(GetMcuVersion());
		
		//�Ƿ�Ϊ��Э�������
		if ( tMtAdpReg.GetIpAddr() == g_cMcuAgent.GetRRQMtadpIp() )
		{
			tMtAdpRegRsp.SetMtAdpMaster(TRUE);
		}
		g_cMcuAgent.GetMcuAlias( achMcuAlias, MAXLEN_ALIAS );
		tMtAdpRegRsp.SetAlias( achMcuAlias );
        
        //�Ƿ�֧��GK�Ʒ�
        tMtAdpRegRsp.SetIsGKCharge( g_cMcuAgent.GetIsGKCharge() );
		
		//����MCU����, zgc, 2007/04/03
		tMtAdpRegRsp.SetMcuType( GetMcuPdtType() );

        // guzh [4/30/2007] ���ñ�����ʾ��ʽ
        tMtAdpRegRsp.SetCasAliasType( g_cMcuVcApp.GetCascadeAliasType() ); 
        tMtAdpRegRsp.SetAdminLevel( g_cMcuVcApp.GetCascadeAdminLevel() );

		// guzh [6/12/2007] ���ӻỰֵ֪ͨ
        LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[MtAdpSsn: Reg Ack with System SSRC.%u.\n", dwSysSSrc);
        tMtAdpRegRsp.SetMSSsrc(dwSysSSrc); 
        
        // zbq [05/14/2007] �Ƿ����ú���ƥ��
        tMtAdpRegRsp.SetUseCallingMatch( g_cMcuVcApp.IsLimitAccessByMtModal() );

		s8 achPassword[GK_RRQ_PWD_LEN]={0};
		strncpy(achPassword,g_cMcuAgent.GetGkRRQPassword(),sizeof(achPassword));
		
		tMtAdpRegRsp.SetGkRRQUsePwdFlag(g_cMcuAgent.GetGkRRQUsePwdFlag());
		tMtAdpRegRsp.SetGkRRQPassword(achPassword);

		cServMsg.SetMsgBody( (u8*)&tMtAdpRegRsp, sizeof( tMtAdpRegRsp ) );
		
		// [11/23/2011 liuxu] ��ӻ��巢���¼��ն��б��������Ϣ
		u8 byAdpSMcuListBufFlag = g_cMcuVcApp.GetMtAdpSMcuListBufFlag();
		cServMsg.CatMsgBody( (u8*)&byAdpSMcuListBufFlag, sizeof(byAdpSMcuListBufFlag));
		u32 dwMtAdpSMcuListInterval = htonl(g_cMcuVcApp.GetRefreshMtAdpSMcuListInterval());
		cServMsg.CatMsgBody( (u8*)&dwMtAdpSMcuListInterval, sizeof(dwMtAdpSMcuListInterval));

#ifdef _UTF8
        //[4/8/2013 liaokang] ���뷽ʽ
        u8 byEncoding = emenCoding_Utf8;
        cServMsg.CatMsgBody( &byEncoding, sizeof(u8));
#endif
		
		SendMsgToMtAdp( pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

        g_cMcuVcApp.AddMtAdp( tMtAdpReg );

		g_cMcuVcApp.SetMtAdpIpType(tMtAdpReg.GetDriId(), byIpType);

		if( 0 != g_cMcuAgent.GetGkIpAddr() && PROTOCOL_TYPE_H323 == m_byProtocolType)
		{
            // zbq [04/25/2007] �ն�����Ucf��urj����δ֪ͨMCU������ע��״̬��һ��.
            // ����������ˢ��ע��һ�Σ���֤��������� �� ������ͬʱ�������������
            // ����ע��GK״̬�ĳɹ������￼�����������ϱ�URQ�Ľ�������Խ��ǰһ��
            // ״̬��һ�µ����⣬�����ܲ��ܽ��������ͬʱ�����������⡣��һ���Ż��� ...
            if ( /*!g_cMcuVcApp.GetConfRegState(0) && */tMtAdpRegRsp.IsMtAdpMaster() )
            {
                //��GK��ע��MCU
                g_cMcuVcApp.SetRegGKDriId( m_byMtAdpId );
                g_cMcuVcApp.RegisterConfToGK( 0, m_byMtAdpId );
                
                //��GK��ע�����ʵ��(���������ģ��)
                for( u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++ )
                {
                    //ע��ģ��
                    TConfMapData tConfMapData = g_cMcuVcApp.GetConfMapData( byConfIdx );
                    if ( tConfMapData.IsTemUsed() )
                    {
                        g_cMcuVcApp.RegisterConfToGK( byConfIdx, m_byMtAdpId, TRUE, FALSE );
                    }
                    //ע�����
                    else if ( tConfMapData.IsValidConf() )
                    {
                        g_cMcuVcApp.RegisterConfToGK( byConfIdx, m_byMtAdpId, FALSE, FALSE );
                    }
                }
            }
            else if (g_cMcuVcApp.GetRegGKDriId() != 0)
            {
                g_cMcuVcApp.RegisterConfToGK( 0, g_cMcuVcApp.GetRegGKDriId() );
            }

			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "CMtAdpSsnInst: MtAdp's DriId:%d Register To GK.\n", m_byMtAdpId);
		}        

        //set qos        
        TQosInfo tAgentQos;
        if(SUCCESS_AGENT == g_cMcuAgent.GetQosInfo(&tAgentQos))
        {
            TMcuQosCfgInfo tMcuQosCfg;
            tMcuQosCfg.SetQosType(tAgentQos.GetQosType());
            tMcuQosCfg.SetAudLevel(tAgentQos.GetAudioLevel());
            tMcuQosCfg.SetVidLevel(tAgentQos.GetVideoLevel());
            tMcuQosCfg.SetDataLevel(tAgentQos.GetDataLevel());
            tMcuQosCfg.SetSignalLevel(tAgentQos.GetSignalLevel());
            tMcuQosCfg.SetIpServiceType(tAgentQos.GetIpServiceType());                
                 
            CServMsg cQosMsg;
            cQosMsg.SetEventId(MCU_MT_SETQOS_CMD);
            cQosMsg.SetMsgBody((u8*)&tMcuQosCfg, sizeof(tMcuQosCfg));  
            SendMsgToH323MtAdp(cQosMsg);
        }
        else
        {
            LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[ProcEqpMcuRegMsg] get qos info failed!\n");
        }    
		
	    //֪ͨMcu
	    SendMsgToMcu(pcMsg->event,pcMsg->content,pcMsg->length);

		NEXTSTATE( STATE_NORMAL );
		break;
        }
    default:
        LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "MtAdpSsn: Wrong message %u(%s) received in current state %u!\n",
                  pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());         
        break;
	}

    return; 
}

/*====================================================================
    ������      : ProcMtAdpDisconnect
    ����        ��MtAdp��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����pcMsg ��Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
void CMtAdpSsnInst::ProcMtAdpDisconnect(CMessage * const pcMsg)
{
	LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[MtAdpSsn]: Mtadp disconnect messege %u(%s) received!,Dri id is:%d,ProtocolType is:%d,MaxMtNum is:%d.\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), m_byMtAdpId, m_byProtocolType, m_byMaxMtNum );

	u8 byRRQReset = FALSE;
	if( *( u32 * )pcMsg->content == m_dwMtAdpNode )	//��ʵ����Ӧ���Ӷ�
	{
        if (INVALID_NODE != m_dwMtAdpNode)
        {
            OspDisconnectTcpNode(m_dwMtAdpNode);
        }
        
		if( g_cMcuVcApp.GetRegGKDriId() == m_byMtAdpId )
		{
			g_cMcuVcApp.SetConfRegState( 0, 0 );
			g_cMcuVcApp.SetRegGKDriId(0);
			byRRQReset = TRUE;
		}

		//���ʵ��״̬
		m_byMtAdpId		 = 0;
		m_dwMtAdpAppIId	 = 0;
		m_byProtocolType = 0;
		m_byMaxMtNum     = 0;
		m_dwMtAdpNode    = INVALID_NODE;

		TMtAdpReg tMtAdpReg;
		tMtAdpReg.SetDriId( (u8)GetInsID() );

		CServMsg cServMsg;
		cServMsg.SetMsgBody((u8 *)&tMtAdpReg,sizeof(tMtAdpReg));
		cServMsg.CatMsgBody(&byRRQReset,sizeof(byRRQReset));

		//��CMcuVcInst���Ƶ��˴�����RemoveMtadp��AddMtadp��Ӧ����mtadpssn�д����ֱ����vcinst��mtadpssninst�д���
		//����ʱ������[12/22/2012 chendaiwei]
		g_cMcuVcApp.RemoveMtAdp( tMtAdpReg.GetDriId() );	
	    SendMsgToMcu( MCU_MTADP_DISCONNECTED_NOTIFY,cServMsg.GetServMsg(),cServMsg.GetServMsgLen());

		NEXTSTATE( STATE_IDLE );
	}	
}

/*=============================================================================
�� �� ���� ProcMtAdpGetMsStatusReq
��    �ܣ� �����ȡmcu����״̬
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage * const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/2/21  4.0			������                  ����
=============================================================================*/
void CMtAdpSsnInst::ProcMtAdpGetMsStatusReq( CMessage * const pcMsg )
{
    // guzh [4/11/2007] ���û��ͨ��ע��ͷ��ͻ�ȡ���󣬺��п�������������ǰ�����ӣ������
    if ( CurState() == STATE_IDLE )
    {
        LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[ProcMtAdpGetMsStatusReq]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);

        return;
    }

    if (MCU_MSSTATE_EXCHANGE_NTF == pcMsg->event)
    {
        // ���ͬ��ʧ��,�Ͽ���Ӧ������
        CServMsg cServMsg(pcMsg->content, pcMsg->length);
        u8 byIsSwitchOk = *cServMsg.GetMsgBody();
        if (0 == byIsSwitchOk)
        {
            OspDisconnectTcpNode( m_dwMtAdpNode );
            return;
        }        
    }
    
    if( MCU_MSSTATE_ACTIVE == g_cMSSsnApp.GetCurMSState() )
    {        
        TMcuMsStatus tMsStatus;
        tMsStatus.SetMsSwitchOK(g_cMSSsnApp.IsMsSwitchOK());
        
        CServMsg cServMsg;
        cServMsg.SetMsgBody((u8*)&tMsStatus, sizeof(tMsStatus));
        SendMsgToMtAdp(MCU_MTADP_GETMSSTATUS_ACK, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

        LogPrint(LOG_LVL_DETAIL, MID_MCU_MT, "[ProcMtAdpGetMsStatusReq]: IsMsSwitchOK :%d.\n", tMsStatus.IsMsSwitchOK());
    }

    return;
}

/*=============================================================================
    �� �� ���� ProcMcuMtAdpGKMsg
    ��    �ܣ� ����MCU���ն˵�GK��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CMessage * const pcMsg
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/11   3.6			����                  ����
=============================================================================*/
void CMtAdpSsnInst::ProcMcuMtAdpGKMsg( CMessage * const pcMsg )
{
	//CServMsg cServMsg( pcMsg->content, pcMsg->length );

	switch(CurState()) 
	{
	case STATE_NORMAL:
		
		//����ע�������ģ�������������ע��Ǩ�ƣ�����ѡ������ģ��� ע��MCU������ʵ��

/*		if( MCU_MT_REREGISTERGK_REQ == pcMsg->event )
		{
			if( !g_cMcuVcApp.GetConfRegState(0) && g_cMcuAgent.GetGkIpAddr()  && 
				0 == g_cMcuVcApp.GetRegGKDriId() && PROTOCOL_TYPE_H323 == m_byProtocolType )
			{
				//��GK��ע��MCU
				g_cMcuVcApp.SetRegGKDriId(m_byMtAdpId);
				//g_cMcuVcApp.RegisterConfToGK( 0, m_byMtAdpId );

				//��GK��ע�����ʵ��
				CMcuVcInst *pConf;
				for( u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++ )
				{
					pConf = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
					if( pConf != NULL ) 			
					{
						pConf->m_byRegGKDriId = m_byMtAdpId;
						//g_cMcuVcApp.RegisterConfToGK( (byLoop+1), pConf->m_byRegGKDriId );
					}
				}

				SendMsgToMtAdp( pcMsg->event, pcMsg->content, pcMsg->length );

				LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "CMtAdpSsnInst: MtAdp's DriId:%d Register To GK Again.\n", m_byMtAdpId);
			}
		}
		else*/
        if ( MCU_MT_UPDATE_REGGKSTATUS_NTF == pcMsg->event || 
			 MCU_MT_UPDATE_GKANDEPID_NTF == pcMsg->event ||
			 MCU_MTADP_GKADDR_UPDATE_CMD == pcMsg->event )
		{
			SendMsgToMtAdp( pcMsg->event, pcMsg->content, pcMsg->length );
		}

		break;

	default:
		break;
	}
}
	
/*====================================================================
    ������      : ProcMtAdpMcuGKMsg
    ����        �������ն˵�MCU��GK��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����pcMsg ��Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/02/28    3.0         ������        ����
====================================================================*/
void CMtAdpSsnInst::ProcMtAdpMcuGKMsg( CMessage * const pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byConfIdx  = cServMsg.GetConfIdx();
    
	switch(CurState()) 
	{
	case STATE_NORMAL:
		
		//��������������GK��ע���GatekeeperID/EndpointID��Ϣ��֪ͨ
		if( MT_MCU_UPDATE_GKANDEPID_NTF == pcMsg->event )
		{
			TH323EPGKIDAlias *ptGKIDAlias = (TH323EPGKIDAlias*)(cServMsg.GetMsgBody());
			TH323EPGKIDAlias *ptEPIDAlias = ptGKIDAlias + 1;

			if( memcmp( (void*)ptGKIDAlias, g_cMcuVcApp.GetH323GKIDAlias(), sizeof(TH323EPGKIDAlias) ) || 
				memcmp( (void*)ptGKIDAlias, g_cMcuVcApp.GetH323EPIDAlias(), sizeof(TH323EPGKIDAlias) ) )
			{
				g_cMcuVcApp.SetH323GKIDAlias( ptGKIDAlias );
				g_cMcuVcApp.SetH323EPIDAlias( ptEPIDAlias );

				cServMsg.SetMsgBody( (u8*)ptGKIDAlias, sizeof(TH323EPGKIDAlias) );
				cServMsg.CatMsgBody( (u8*)ptEPIDAlias, sizeof(TH323EPGKIDAlias) );

				for( u8 byOtherDri = 1; byOtherDri <= MAXNUM_DRI; byOtherDri++)
				{
					if( g_cMcuVcApp.IsMtAdpConnected(byOtherDri) && 
						PROTOCOL_TYPE_H323 == g_cMcuVcApp.GetMtAdpProtocalType(byOtherDri) && 
						g_cMcuAgent.GetGkIpAddr()  && 0 != g_cMcuVcApp.GetRegGKDriId() && 
						byOtherDri != g_cMcuVcApp.GetRegGKDriId() )
					{
						cServMsg.SetDstDriId( byOtherDri );
						g_cMtAdpSsnApp.SendMsgToMtAdpSsn( byOtherDri, MCU_MT_UPDATE_GKANDEPID_NTF, cServMsg );
					}
				}
			}
            
            // N+1���ݣ������������ɸ���MCU��GKID��EPID����ʹ�õ������ٴ�ע��ǰ��ע�������ɹ�[12/18/2006-zbq]
            if ( GetInsID() == g_cMcuVcApp.GetRegGKDriId() &&
                 MCU_NPLUS_IDLE != g_cNPlusApp.GetLocalNPlusState() )
            {
                if ( 0 != g_cMcuAgent.GetGkIpAddr() &&
                     MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState() ) 
                {
                    TRASInfo tRASInfo;
                    tRASInfo.SetEPID( ptEPIDAlias );
                    tRASInfo.SetGKID( ptGKIDAlias );
                    tRASInfo.SetGKIp( ntohl(g_cMcuAgent.GetGkIpAddr()) );
                    tRASInfo.SetRRQIp( g_cMcuAgent.GetRRQMtadpIp() );

                    // zbq [03/15/2007] ͬ��RAS��Ϣ������MCU��E164һ��
                    TLocalInfo tLocalInfo;
                    g_cMcuAgent.GetLocalInfo( &tLocalInfo );
                    tRASInfo.SetMcuE164( tLocalInfo.GetE164Num() );

                    CServMsg cSendServMsg;
                    cSendServMsg.SetEventId( MCU_NPLUS_RASINFOUPDATE_REQ );
                    cSendServMsg.SetMsgBody( (u8*)&tRASInfo, sizeof(TRASInfo) );
                    g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cSendServMsg.GetServMsg(), cSendServMsg.GetServMsgLen());
                }
            }
			return;
		}

        if( cServMsg.GetSrcDriId() != g_cMcuVcApp.GetRegGKDriId() )
        {
            LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[Mtadpssn]: SrcDriId.(%d) != g_cMcuVcApp.GetRegGKDriId().(%d) %u(%s) return!\n", 
                                    cServMsg.GetSrcDriId(), g_cMcuVcApp.GetRegGKDriId(), 
                                    pcMsg->event, ::OspEventDesc( pcMsg->event ) );
            return;
        }

        // zbq [03/30/2007] ֻ��֪ͨ����������ҵ��
        if ( MT_MCU_UNREGISTERGK_NACK == pcMsg->event )
        {
            LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[Mtadpssn]: ConfIdx<%d> unreg result.%d<%s>!\n", 
                    byConfIdx, pcMsg->event, OspEventDesc(pcMsg->event) );
            return;
        }
	
		//ע��Ϊ���鼰mcu��Ϣͳһע�ᣬ��ֻҪ��ack��mcu���Ѿ�����ע��
		if( pcMsg->event ==  MT_MCU_REGISTERGK_ACK )// MCU��ע���Ӧ
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[Mtadpssn]: ConfIdx<%d> register to GK success!\n", byConfIdx );
		}
		else if( 0 == byConfIdx && MT_MCU_REGISTERGK_NACK == pcMsg->event )
		{
			LogPrint(LOG_LVL_WARNING, MID_MCU_MT2, "[Mtadpssn]: ConfIdx<%d> register to GK failure!\n", byConfIdx );
		}

        //[12/2/2011 zhushengze]ͨ��������������ע��״̬�ŵ�VC�н��д���
        //��CMtAdpSsnInst���˶�����ͬ��������ע��ʱ�����л��鵱ǰע��״̬ͨ���������ʱ������
// 		if( byConfIdx <= MAX_CONFIDX )
// 		{
// 			//u8 byConfIdx  = 0; //0��mcu��1��MAXNUM_MCU_CONF ����
// 			u8 byRegState = 0; //0��δע�ᣬ1���ɹ�ע��
// 			u8 byOtherDri = 1;
// 			TMtAlias tMtAlias;
// 			
// 			if( MT_MCU_REGISTERGK_ACK == pcMsg->event )
// 			{
// 				byRegState = 1;
// 			}
// 			g_cMcuVcApp.SetConfRegState( byConfIdx, byRegState );
// 
//     		//ͬ����ʱ��ע��MCU������ʵ�� ע����Ϣ �����������
// 			if( 0 == byConfIdx )
// 			{ 	
// 				char achMcuAlias[MAXLEN_ALIAS];
// 				g_cMcuAgent.GetE164Number( achMcuAlias, MAXLEN_ALIAS );
// 				tMtAlias.SetE164Alias( achMcuAlias );
// 				cServMsg.SetMcuId( LOCAL_MCUID );
// 				cServMsg.SetMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
// 				cServMsg.CatMsgBody( (u8*)&byConfIdx, sizeof( byConfIdx ) );
// 				cServMsg.CatMsgBody( (u8*)&byRegState, sizeof( byRegState ) );
// 			}
// 			else
// 			{		
//                 TConfInfo* ptConfInfo = NULL;
//                 TTemplateInfo tTemInfo;
//                 
//                 TConfMapData tMapData = g_cMcuVcApp.GetConfMapData(byConfIdx);
//                 if(tMapData.IsValidConf())
//                 {
// 					CMcuVcInst* pcVcinst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
//                     if( NULL ==  pcVcinst)
//                     {
//                         //zbq [10/11/2007] �������� ���������������ģ��ע��״̬�������⣺
//                         //    ����Ǵ������ģ�嵫ȴû�л��飬����ͬ����ģ��ĵ�ǰע��״̬
//                         LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[ProcMtAdpMcuGKMsg] RegAck for Conf.%d received while conf is not create yet which should be there before this moment, just take a try of its template once more\n", byConfIdx);
// 
//                         if(tMapData.IsTemUsed())
//                         {
//                             if(!g_cMcuVcApp.GetTemplate(byConfIdx, tTemInfo))
//                             {
//                                 LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[ProcMtAdpMcuGKMsg] RegAck for Conf.%d whose template unexist yet, ignore it\n", byConfIdx);
//                                 return;
//                             }
//                             else
//                             {
//                                 ptConfInfo = &tTemInfo.m_tConfInfo;
//                                 LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[ProcMtAdpMcuGKMsg] RegAck for Conf.%d whose template exist, continue\n", byConfIdx);
//                             }
//                         }
//                         return;
//                     }
//                     else
//                     {
// 						ptConfInfo = &pcVcinst->m_tConf;
//                     }
//                 }
//                 else if(tMapData.IsTemUsed())
//                 {
//                     if(!g_cMcuVcApp.GetTemplate(byConfIdx, tTemInfo))
//                     {
//                         LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[ProcMtAdpMcuGKMsg] RegAck for Template.%d while template unexist, ignore it\n", byConfIdx);
//                         return;
//                     }
//                     else
//                     {
//                         ptConfInfo = &tTemInfo.m_tConfInfo;
//                     }
//                 }
//                 else
//                 {
//                     LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[ProcMtAdpMcuGKMsg] RegAck for Conf/Temp.%d while both of them unexist, ignore it\n", byConfIdx);
//                     return;
//                 }
// 				
// 				//MT_MCU_UNREGISTERGK_ACK MT_MCU_UNREGISTERGK_NACK
// 				if( MT_MCU_REGISTERGK_ACK  == pcMsg->event || 
// 					MT_MCU_REGISTERGK_NACK == pcMsg->event )
// 				{
// 					tMtAlias.SetE164Alias( ptConfInfo->GetConfE164() );
// 					cServMsg.SetConfId( ptConfInfo->GetConfId() );
// 				}
// 				cServMsg.SetMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
// 				cServMsg.CatMsgBody( (u8*)&byConfIdx, sizeof( byConfIdx ) );
// 				cServMsg.CatMsgBody( (u8*)&byRegState, sizeof( byRegState ) );
// 
//                 if (byRegState == 0)
//                 {
//                     LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[ProcMtAdpMcuGKMsg]conf.%d reg state is NULL, Alias is:\n ", byConfIdx);
//                     tMtAlias.Print();
//                 }
// 			}
// 
// 			for( byOtherDri = 1; byOtherDri <= MAXNUM_DRI; byOtherDri++)
// 			{
// 				if( g_cMcuVcApp.IsMtAdpConnected(byOtherDri) && 
// 					PROTOCOL_TYPE_H323 == g_cMcuVcApp.GetMtAdpProtocalType(byOtherDri) && 
// 					g_cMcuAgent.GetGkIpAddr()  && 0 != g_cMcuVcApp.GetRegGKDriId() && 
// 					byOtherDri != g_cMcuVcApp.GetRegGKDriId() )
// 				{
// 					cServMsg.SetDstDriId( byOtherDri );
// 					g_cMtAdpSsnApp.SendMsgToMtAdpSsn( byOtherDri, MCU_MT_UPDATE_REGGKSTATUS_NTF, cServMsg );
// 				}
// 			}
// 		}
		
        // ������GK��ע��Ļ�Ӧ
        //if( 0 != cServMsg.GetConfIdx() ||
        //    (0 == cServMsg.GetConfIdx() && MT_MCU_REGISTERGK_NACK == cServMsg.GetEventId()) )
		{
			SendMsgToMcu( pcMsg->event, pcMsg->content, pcMsg->length );
		}
		
		break;

	default:
		break;
	}
}

/*====================================================================
    ������      : ProcMtAdpMcuMsg
    ����        ����ת������Ϣ��MCUҵ��ģ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����pcMsg ��Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
void CMtAdpSsnInst::ProcMtAdpMcuMsg( CMessage * const pcMsg )
{
	switch(CurState()) 
	{
	case STATE_NORMAL:
		
		SendMsgToMcu( pcMsg->event, pcMsg->content, pcMsg->length );

		break;
	default:
		break;
	}

}

/*====================================================================
    ������      : ProcMcuMtAdpMsg
    ����        ����תMCUҵ��ģ����Ϣ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����pcMsg ��Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
void CMtAdpSsnInst::ProcMcuMtAdpMsg( CMessage * const pcMsg )
{
	switch(CurState()) 
	{
	case STATE_NORMAL:

		SendMsgToMtAdp( pcMsg->event, pcMsg->content, pcMsg->length );		
		
		break;

	default:
		break;
	}
}



//CMpConfig
/*--------------------------------------------------------------------------------*/

//����
CMtAdpConfig::CMtAdpConfig()
{

}

//����
CMtAdpConfig::~CMtAdpConfig()
{

}

/*====================================================================
    ������      : SendMsgToMtAdpSsn
    ����        ������Ϣ��MtAdp�Ự
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMt & tDstMt: ��Ϣ��Ŀ��
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
BOOL32 CMtAdpConfig::SendMsgToMtAdpSsn( const TMt & tDstMt, u16 wEvent, CServMsg & cServMsg )
{
    if ( 0 == tDstMt.GetDriId() || tDstMt.GetDriId() > MAXNUM_DRI)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[MtAdpCfg]: MtId.%d Invalid MtAdpId.%d for message %s.\n", 
                   tDstMt.GetMtId(),tDstMt.GetDriId(), OspEventDesc(wEvent) );
		return FALSE;
	}
	else if (!g_cMcuVcApp.IsMtAdpConnected(tDstMt.GetDriId()))
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[MtAdpCfg]: MtId.%d MtAdp.%d is not connected.\n", tDstMt.GetMtId(),tDstMt.GetDriId() );
		return FALSE;
	}
	else
	{
		//�����������������ⲿ��AppͶ����Ϣ��������
		if (FALSE == g_cMSSsnApp.JudgeSndMsgPass())
		{
			return TRUE;
		}
		
		//cServMsg.SetEventId(wEvent);
		cServMsg.SetDstDriId( tDstMt.GetDriId() );
		cServMsg.SetDstMtId( tDstMt.GetMtId() );
		cServMsg.SetMcuId( LOCAL_MCUID );

		return ( ::OspPost( MAKEIID( AID_MCU_MTADPSSN, tDstMt.GetDriId() ), wEvent, 
						cServMsg.GetServMsg(), cServMsg.GetServMsgLen() ) == OSP_OK );
	}
}

/*====================================================================
    ������      : SendMsgToMtAdpSsn
    ����        ������Ϣ��MtAdp�Ự
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMpId: ��Ϣ��Ŀ��
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/02/28    3.0         ������         ����
====================================================================*/
BOOL32 CMtAdpConfig::SendMsgToMtAdpSsn( u8 byMpId, u16 wEvent, CServMsg & cServMsg )
{
    if (0 == byMpId || byMpId > MAXNUM_DRI)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[MtAdpCfg]: SendMsgToMtAdpSsn Invalid MpId: %d.\n", byMpId);
		return FALSE;
	}
	else
	{
		//�����������������ⲿ��AppͶ����Ϣ��������
		if (FALSE == g_cMSSsnApp.JudgeSndMsgPass())
		{
			return TRUE;
		}

		cServMsg.SetMcuId( LOCAL_MCUID );			
		return ( ::OspPost( MAKEIID( AID_MCU_MTADPSSN, byMpId ), wEvent, 
						cServMsg.GetServMsg(), cServMsg.GetServMsgLen() ) == OSP_OK );
	}
}

/*====================================================================
    ������      : BroadcastToAllMtAdpSsn
    ����        ������Ϣ�����������ӵ�MtAdp�Ự
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����wEvent ��ϢID
				  pbyMsg ��Ϣ��
                  wLen   ��Ϣ����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
BOOL32 CMtAdpConfig::BroadcastToAllMtAdpSsn(u16 wEvent, const CServMsg & cServMsg )
{
	//�����������������ⲿ��AppͶ����Ϣ��������
	if (FALSE == g_cMSSsnApp.JudgeSndMsgPass())
	{
		return TRUE;
	}

	return ( OSP_OK == ::OspPost( MAKEIID( AID_MCU_MTADPSSN, CInstance::EACH ), wEvent, 
		                          cServMsg.GetServMsg(), cServMsg.GetServMsgLen() ) );
}


//END OF FILE
