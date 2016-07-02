/*****************************************************************************
   ģ����      : mcu
   �ļ���      : eqpssn.cpp
   ����ļ�    : eqpssn.h
   �ļ�ʵ�ֹ���: ����ỰӦ����ʵ��
   ����        : Ѧ����
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2002/08/19  0.9         Ѧ����      ����
   2002/08/20  0.9         LI Yi       ��Ӵ���
   2005/02/19  3.6         ����      �����޸ġ���3.5�汾�ϲ�
******************************************************************************/

#include "evmcueqp.h"
#include "evmcu.h"
#include "evmcutest.h"
#include "mcuvc.h"
#include "eqpssn.h"
#include "mcuver.h"

/*lint -save -esym(641, emMcuErrorCode)*/

CEqpSsnApp	g_cEqpSsnApp;	//����ỰӦ��ʵ��

CEqpSsnInst::CEqpSsnInst()
{
	m_dwEqpNode = INVALID_NODE;
	m_byEqpId = 0;
	memset( m_achAlias, 0, MAXLEN_EQP_ALIAS );
}

CEqpSsnInst::~CEqpSsnInst()
{
}

/*====================================================================
    ������      ��InstanceDump
    ����        �����ش�ӡ��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u32 param, ��ӡ״̬����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/21    1.0         LI Yi        ����
====================================================================*/
void CEqpSsnInst::InstanceDump( u32 param )
{
	char	achType[32];
	memset( achType, 0, sizeof(achType) );

	switch( m_byEqpType )
	{
	case EQP_TYPE_TVWALL:
		strncpy( achType, "TVWALL", sizeof(achType) );
		break;
	case EQP_TYPE_MIXER:
		{
			u8 byMixerSubtype = UNKONW_MIXER;
			g_cMcuAgent.GetMixerSubTypeByEqpId(m_byEqpId,byMixerSubtype);
			switch (byMixerSubtype)
			{
			case APU_MIXER:
				strncpy( achType, "MIXER_APU", sizeof(achType)  );
				break;
			case EAPU_MIXER:
				strncpy( achType, "MIXER_EAPU", sizeof(achType)  );
				break;
			case APU2_MIXER:
				strncpy( achType, "MIXER_APU2", sizeof(achType)  );
				break; 
			case MIXER_8KG:
				strncpy( achType, "MIXER_8KG", sizeof(achType)  );
				break;
			case MIXER_8KH:
				strncpy( achType, "MIXER_8KH", sizeof(achType)  );
				break;
			case MIXER_8KI:
				strncpy( achType, "MIXER_8KI", sizeof(achType)  );
				break; 
			default:
				strncpy( achType, "MIXER_UNKNOWN", sizeof(achType)  );
				break;
			}
		}
		break;
	case EQP_TYPE_RECORDER:
		strncpy( achType, "RECORD", sizeof(achType)  );
		break;
	case EQP_TYPE_DCS:
		strncpy( achType, "DCS", sizeof(achType)  );
		break;
	case EQP_TYPE_BAS:
		strncpy( achType, "BAS", sizeof(achType)  );
		break;
	case EQP_TYPE_VMP:
		strncpy( achType, "VMP", sizeof(achType)  );
		break;
    case EQP_TYPE_PRS:
        strncpy( achType, "PRS", sizeof(achType)  );
        break;
    case EQP_TYPE_VMPTW:
        strncpy( achType, "MPW", sizeof(achType)  );
        break;
	case EQP_TYPE_HDU:
		{
			u8 byHduSubtype = HDU_SUBTYPE_HDU_M;
			if(g_cMcuAgent.GetHduSubTypeByEqpId(m_byEqpId,byHduSubtype))
			{
				switch (byHduSubtype)
				{
				case HDU_SUBTYPE_HDU_M:
					strncpy( achType, "HDU", sizeof(achType)  );
					break; 
				case HDU_SUBTYPE_HDU_H:
					strncpy( achType, "HDU_H", sizeof(achType)  );
					break; 
				case HDU_SUBTYPE_HDU_L:
					strncpy( achType, "HDU_L", sizeof(achType)  );
					break;
				case HDU_SUBTYPE_HDU2:
					strncpy( achType, "HDU2", sizeof(achType)  );
					break;
				case HDU_SUBTYPE_HDU2_L:
					strncpy( achType, "HDU2_L", sizeof(achType)  );
					break;
				case HDU_SUBTYPE_HDU2_S:
					strncpy( achType, "HDU2_S", sizeof(achType)  );
					break;
				default:
					strncpy( achType, "N/A", sizeof(achType)  );
					break;
				}
			}
			else
			{
				strncpy( achType, "N/A", sizeof(achType)  );
			}

		}
		break;
	default:
		strncpy( achType, "N/A", sizeof(achType)  );
		break;
	}
    
	StaticLog("%3u %5u %5u %6s   %s param:%u\n", GetInsID(), m_byEqpId, m_dwEqpNode, achType,m_achAlias,param);
}

/*====================================================================
    ������      ��InstanceEntry
    ����        ��ʵ����Ϣ������ں���������override
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/19    1.0         Ѧ����        ����
====================================================================*/

#define EQP_EV_MSG(level)	MCU_EV_LOG(level, MID_MCU_EQP, "Eqp")

void CEqpSsnInst::InstanceEntry( CMessage * const pcMsg )
{
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[EqpSsn]: The received msg's pointer in the msg entry is NULL!");
		return;
	}

	switch( pcMsg->event )
	{
		// [2/25/2010 xliang] 8ke eqp reg
/*	case MIXER_MCU_REG_REQ:				//�������Ǽ�����
#if defined(_8KE_) || defined(_8KH_)  || defined(_8KI_)
	#ifdef _8KI_
			Proc8KiMixerMcuRegMsg( pcMsg );
	#else
			Proc8KEEqpMcuRegMsg( pcMsg );
	#endif                        		
#else
		ProcEqpMcuRegMsg( pcMsg );
#endif

		break;*/
	case MIXER_MCU_REG_REQ:				//�������Ǽ�����
	case VMP_MCU_REGISTER_REQ:          //����ϳ����Ǽ�����
	case BAS_MCU_REG_REQ:				//�������Ǽ�����
		//[nizhijun 2011/02/17] prs 8ke���û�
	case PRS_MCU_REGISTER_REQ:			//PRS��MCUע������
		{
			EQP_EV_MSG(LOG_LVL_KEYSTATUS);
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
			Proc8KEEqpMcuRegMsg( pcMsg );
#else
			ProcEqpMcuRegMsg( pcMsg );
#endif
			break;
		}

	case TVWALL_MCU_REG_REQ:			//����ǽ�Ǽ�����
	case REC_MCU_REG_REQ:				//¼����Ǽ�����
	case VMPTW_MCU_REGISTER_REQ:        //���ϵ���ǽ�Ǽ�����
    case HDU_MCU_REG_REQ:               //HDUע��Ǽ�
		EQP_EV_MSG(LOG_LVL_KEYSTATUS);
		ProcEqpMcuRegMsg( pcMsg );
		break;

	case OSP_DISCONNECT:				//����̨����֪ͨ
		EQP_EV_MSG(LOG_LVL_KEYSTATUS);
		ProcEqpDisconnect( pcMsg );
		break;

	//MCU���������Ϣ
	case MCU_TVWALL_START_PLAY_REQ:	    //���迪ʼ����֪ͨ
	case MCU_TVWALL_STOP_PLAY_REQ:		//����֪ͨ����֪ͨ

    case MCU_HDU_START_PLAY_REQ:        //HDU����
    case MCU_HDU_STOP_PLAY_REQ:         //HDUֹͣ����
	case MCU_HDU_CHGHDUVMPMODE_REQ:		//�л�HDUͨ�����
	case MCU_HDU_FRESHAUDIO_PAYLOAD_CMD://ˢ����Ƶ�غ�

	case MCU_REC_STARTREC_REQ:			//��ʼ¼��
	case MCU_REC_PAUSEREC_REQ:			//��ͣ¼��
	case MCU_REC_RESUMEREC_REQ:			//�ָ�¼��
	case MCU_REC_STOPREC_REQ:			//ֹͣ¼��
	case MCU_REC_STARTPLAY_REQ:			//��ʼ����
	case MCU_REC_PAUSEPLAY_REQ:			//��ͣ����
	case MCU_REC_RESUMEPLAY_REQ:		//�ָ�����
	case MCU_REC_STOPPLAY_REQ:			//ֹͣ����
	case MCU_REC_FFPLAY_REQ:			//���������
	case MCU_REC_FBPLAY_REQ:			//����������
	case MCU_REC_SEEK_REQ:				//���������ȵ���
	case MCU_REC_LISTALLRECORD_REQ:		//��ѯ¼�����¼
	case MCU_REC_DELETERECORD_REQ:      //ɾ����¼
	case MCU_REC_PUBLISHREC_REQ:        //����¼������
	case MCU_REC_CANCELPUBLISHREC_REQ:  //ȡ������¼������
    case MCU_REC_RENAMERECORD_REQ:      //¼���ļ�����
    case MCU_REC_GETRECPROG_CMD:
    case MCU_REC_GETPLAYPROG_CMD:
/*	case MCU_REC_ADJUST_BACKRTCP_CMD:    //����¼���backrtcp[liu lijiu][20100727]*/
	
	case MCU_MIXER_STARTMIX_REQ:        //MCU��ʼ��������
	case MCU_MIXER_STOPMIX_REQ:         //MCUֹͣ��������
	//tianzhiyong 2010/05/19 �����������������ֹͣ�����EAPU�������ã�
	case MCU_MIXER_STARTVAC_CMD:        //MCU��ʼ��������
	case MCU_MIXER_STOPVAC_CMD:         //MCUֹͣ��������
	case MCU_MIXER_ADDMEMBER_REQ:       //�����Ա����
	case MCU_MIXER_REMOVEMEMBER_REQ:    //ɾ����Ա����
	case MCU_MIXER_FORCEACTIVE_REQ:     //ǿ�Ƴ�Ա����
	case MCU_MIXER_CANCELFORCEACTIVE_REQ:    //ȡ����Աǿ�ƻ���
	case MCU_MIXER_SETCHNNLVOL_CMD:     //����ĳͨ��������Ϣ
	case MCU_MIXER_SETMIXDEPTH_REQ:     //���û������������Ϣ
	case MCU_MIXER_SEND_NOTIFY:         //MCU�����������Ƿ�Ҫ��������֪ͨ
	case MCU_MIXER_VACKEEPTIME_CMD:     //��������������ʱ����ʱ����Ϣ
//	case MCU_MIXER_SETMEMBER_CMD:       //���û�����Ա
    case MCU_MIXER_CHANGEMIXDELAY_CMD:
	
	case MCU_BAS_STARTADAPT_REQ:		//������������
	case MCU_BAS_STOPADAPT_REQ:			//ֹͣ��������
	case MCU_BAS_SETADAPTPARAM_CMD:		//�ı������������
	case MCU_BAS_CHANGEAUDPARAM_REQ:    //��Ƶ�����ı��������
    case MCU_BAS_FASTUPDATEPIC_CMD:     //
    case MCU_HDBAS_STARTADAPT_REQ:      //����������������
    case MCU_HDBAS_STOPADAPT_REQ:       //ֹͣ������������
    case MCU_HDBAS_SETADAPTPARAM_CMD:   //�ı���������������

	case MCU_VMP_STARTVIDMIX_REQ:       //MCU����VMP��ʼ����
	case MCU_VMP_STOPVIDMIX_REQ:        //MCU����VMPֹͣ����
	case MCU_VMP_CHANGEVIDMIXPARAM_CMD: //MCU����VMP�ı临�ϲ���
	case MCU_VMP_GETVIDMIXPARAM_REQ:    //MCU����VMP���͸��ϲ���
	case MCU_VMP_SETCHANNELBITRATE_REQ: //�ı仭��ϳɵ�����
    case MCU_VMP_UPDATAVMPENCRYPTPARAM_REQ:
    case MCU_VMP_FASTUPDATEPIC_CMD:
	case MCU_VMP_CHANGEMEMALIAS_CMD:
	case MCU_VMP_ADDREMOVECHNNL_CMD:
        
    case MCU_VMPTW_STARTVIDMIX_REQ:
	case MCU_VMPTW_STOPVIDMIX_REQ:
	case MCU_VMPTW_CHANGEVIDMIXPARAM_REQ:
	case MCU_VMPTW_GETVIDMIXPARAM_REQ:
	case MCU_VMPTW_SETCHANNELBITRATE_REQ:
    case MCU_VMPTW_UPDATAVMPENCRYPTPARAM_REQ:
    case MCU_VMPTW_FASTUPDATEPIC_CMD:
		
	case MCU_PRS_SETSRC_REQ:			//MCU����PRS��ʼ������ϢԴ
//	case MCU_PRS_ADDRESENDCH_REQ:		//MCU����PRS���һ���ش�ͨ��
//	case MCU_PRS_REMOVERESENDCH_REQ:	//MCU����PRS�Ƴ�һ���ش�ͨ��
	case MCU_PRS_REMOVEALL_REQ:			//MCU����PRS�Ƴ����е��ش�ͨ��

	case MCU_EQP_SWITCHSTART_NOTIF:		//MCU֪ͨ���轻��ͨ���Ѵ�

	case MCU_HDU_CHANGEMODEPORT_NOTIF:  //HDU���������ʽ�ı��֪ͨ����
	case MCU_HDU_CHANGEPLAYPOLICY_NOTIF://ͨ��������ʾ�����޸�
    case MCU_HDU_CHANGEVOLUME_CMD:      // ������������

	case MCU_EQP_MODSENDADDR_CMD:
		
	case MCU_VMP_STARTSTOPCHNNL_CMD:	//MCU֪ͨVMPͣ��ĳһ���ͨ������
	case MCU_HDU_CHANGEMODE_CMD:
	case MCU_REC_SETRTCP_NFY:
		EQP_EV_MSG(LOG_LVL_KEYSTATUS);
		ProcMcuToEqpMsg( pcMsg );
		break;

	case REC_MCU_STARTREC_ACK:			//��ʼ¼��Ӧ��
	case REC_MCU_STARTREC_NACK:			//��ʼ¼��Ӧ��
	case REC_MCU_PAUSEREC_ACK:			//��ͣ¼��Ӧ��
	case REC_MCU_PAUSEREC_NACK:			//��ͣ¼��Ӧ��
	case REC_MCU_RESUMEREC_ACK:			//¼���ļ�����Ӧ��
	case REC_MCU_RESUMEREC_NACK:		//¼���ļ�������Ӧ��
	case REC_MCU_STOPREC_ACK:			//ֹͣ¼��Ӧ��
	case REC_MCU_STOPREC_NACK:			//ֹͣ¼��Ӧ��
	case REC_MCU_STARTPLAY_ACK:			//��ʼ����Ӧ��
	case REC_MCU_STARTPLAY_NACK:		//��ʼ����Ӧ��
	case REC_MCU_PAUSEPLAY_ACK:			//��ͣ����Ӧ��
	case REC_MCU_PAUSEPLAY_NACK:		//��ͣ����Ӧ��
	case REC_MCU_RESUMEPLAY_ACK:		//�ָ�����Ӧ��
	case REC_MCU_RESUMEPLAY_NACK:		//�ָ�����Ӧ��
	case REC_MCU_CHANGERECMODE_ACK:		//�ı�¼��ģʽ
	case REC_MCU_CHANGERECMODE_NACK:	//�ı�¼��ģʽ
	case REC_MCU_STOPPLAY_ACK:			//ֹͣ����Ӧ��
	case REC_MCU_STOPPLAY_NACK:			//ֹͣ����Ӧ��
	case REC_MCU_FFPLAY_ACK:			//������Ӧ��
	case REC_MCU_FFPLAY_NACK:			//������Ӧ��
	case REC_MCU_FBPLAY_ACK:			//�������Ӧ��
	case REC_MCU_FBPLAY_NACK:			//�������Ӧ��
	case REC_MCU_SEEK_ACK:				//������ȵ���Ӧ��
	case REC_MCU_SEEK_NACK:				//������ȵ�����Ӧ��
	case REC_MCU_NEEDIFRAME_CMD:         //REC����ؼ�֡

	case MIXER_MCU_ACTIVEMMBCHANGE_NOTIF:  //����������Ա�ı�֪ͨ
	case MIXER_MCU_CHNNLVOL_NOTIF:      //ĳͨ������֪ͨ��Ϣ
		
//	case MIXER_MCU_SETMIXDEPTH_ACK:     //���û������Ӧ����Ϣ
//	case MIXER_MCU_SETMIXDEPTH_NACK:    //���û�����Ⱦܾ���Ϣ
	case MIXER_MCU_STARTMIX_ACK:        //ͬ�⿪ʼ����Ӧ��
	case MIXER_MCU_STARTMIX_NACK:       //�ܾ���ʼ����Ӧ��
	case MIXER_MCU_STOPMIX_ACK:         //ͬ��ֹͣ����Ӧ��
	case MIXER_MCU_STOPMIX_NACK:        //�ܾ�ֹͣ����Ӧ��
	case MIXER_MCU_ADDMEMBER_ACK:       //�����ԱӦ����Ϣ
	case MIXER_MCU_ADDMEMBER_NACK:      //�����ԱӦ����Ϣ
	case MIXER_MCU_REMOVEMEMBER_ACK:    //ɾ����ԱӦ����Ϣ
	case MIXER_MCU_REMOVEMEMBER_NACK:   //ɾ����ԱӦ����Ϣ
	case MIXER_MCU_FORCEACTIVE_ACK:     //ǿ�Ƴ�Ա����Ӧ��
	case MIXER_MCU_FORCEACTIVE_NACK:    //ǿ�Ƴ�Ա�����ܾ�
	case MIXER_MCU_CANCELFORCEACTIVE_ACK:    //ȡ����Աǿ�ƻ���Ӧ��
	case MIXER_MCU_CANCELFORCEACTIVE_NACK:   //ȡ����Աǿ�ƻ����ܾ�
	case MIXER_MCU_GRPSTATUS_NOTIF:     //������״̬֪ͨ
	
	case BAS_MCU_STARTADAPT_ACK:		//��������Ӧ��
	case BAS_MCU_STARTADAPT_NACK:		//��������ܾ�
	case BAS_MCU_CHANGEAUDPARAM_ACK:    //�����ı����Ӧ��
	case BAS_MCU_CHANGEAUDPARAM_NACK:    //�����ı�����ܾ�
	case BAS_MCU_STOPADAPT_ACK:         //ֹͣ����Ӧ��
	case BAS_MCU_STOPADAPT_NACK:        //ֹͣ����ܾ�
    case BAS_MCU_NEEDIFRAME_CMD:        //����������I֡
    case HDBAS_MCU_STARTADAPT_ACK:      //������������Ӧ��
    case HDBAS_MCU_STARTADAPT_NACK:     //������������ܾ�
    case HDBAS_MCU_STOPADAPT_ACK:       //ֹͣ��������Ӧ��
    case HDBAS_MCU_STOPADAPT_NACK:      //ֹͣ��������ܾ�
		
	case VMP_MCU_STARTVIDMIX_ACK:       //VMP��MCU��ʼ����ȷ��
	case VMP_MCU_STARTVIDMIX_NACK:      //VMP��MCU��ʼ�����ܾ�
	case VMP_MCU_STARTVIDMIX_NOTIF:     //VMP��MCU��ʼ����֪ͨ
	case VMP_MCU_STOPVIDMIX_ACK:        //VMP��MCUֹͣ����ȷ��
	case VMP_MCU_STOPVIDMIX_NACK:       //VMP��MCUֹͣ�����ܾ�
	case VMP_MCU_STOPVIDMIX_NOTIF:      //VMP��MCUֹͣ����֪ͨ
	case VMP_MCU_CHANGEVIDMIXPARAM_ACK: //VMP��MCU�ı临�ϲ���ȷ��
	case VMP_MCU_CHANGEVIDMIXPARAM_NACK://VMP��MCU�ı临�ϲ����ܾ�
	case VMP_MCU_CHANGESTATUS_NOTIF:    //VMP��MCU�ı临�ϲ���֪ͨ
	case VMP_MCU_GETVIDMIXPARAM_ACK:    //VMP��MCU���͸��ϲ���ȷ��
	case VMP_MCU_GETVIDMIXPARAM_NACK:   //VMP��MCU���͸��ϲ����ܾ�
	case VMP_MCU_SETCHANNELBITRATE_ACK: //�ı�����Ӧ��
	case VMP_MCU_SETCHANNELBITRATE_NACK://�ı����ʾܾ�
	case VMP_MCU_NEEDIFRAME_CMD:        //����ϳ�����I֡

    case VMPTW_MCU_STARTVIDMIX_ACK:       //VMPTW��MCU��ʼ����ȷ��
	case VMPTW_MCU_STARTVIDMIX_NACK:      //VMPTW��MCU��ʼ�����ܾ�
	case VMPTW_MCU_STARTVIDMIX_NOTIF:     //VMPTW��MCU��ʼ����֪ͨ
	case VMPTW_MCU_STOPVIDMIX_ACK:        //VMPTW��MCUֹͣ����ȷ��
	case VMPTW_MCU_STOPVIDMIX_NACK:       //VMPTW��MCUֹͣ�����ܾ�
	case VMPTW_MCU_STOPVIDMIX_NOTIF:      //VMPTW��MCUֹͣ����֪ͨ
	case VMPTW_MCU_CHANGEVIDMIXPARAM_ACK: //VMPTW��MCU�ı临�ϲ���ȷ��
	case VMPTW_MCU_CHANGEVIDMIXPARAM_NACK://VMPTW��MCU�ı临�ϲ����ܾ�
	case VMPTW_MCU_CHANGESTATUS_NOTIF:    //VMPTW��MCU�ı临�ϲ���֪ͨ
	case VMPTW_MCU_GETVIDMIXPARAM_ACK:    //VMPTW��MCU���͸��ϲ���ȷ��
	case VMPTW_MCU_GETVIDMIXPARAM_NACK:   //VMPTW��MCU���͸��ϲ����ܾ�
	case VMPTW_MCU_SETCHANNELBITRATE_ACK: //VMPTW��MCU�ı�����Ӧ��
	case VMPTW_MCU_SETCHANNELBITRATE_NACK://VMPTW��MCU�ı����ʾܾ�
	case VMPTW_MCU_NEEDIFRAME_CMD:        //VMPTW��MCU����I֡

	case PRS_MCU_SETSRC_ACK:			//PRS��MCU������ϢԴȷ��
	case PRS_MCU_SETSRC_NACK:			//PRS��MCU������ϢԴ�ܾ�
//	case PRS_MCU_ADDRESENDCH_ACK:		//PRS��MCUֹͣ����ȷ��
//	case PRS_MCU_ADDRESENDCH_NACK:		//PRS��MCUֹͣ�����ܾ�
//	case PRS_MCU_REMOVERESENDCH_ACK:	//PRS��MCU�ı临�ϲ���ȷ��
//	case PRS_MCU_REMOVERESENDCH_NACK:	//PRS��MCU�ı临�ϲ����ܾ�
	case PRS_MCU_REMOVEALL_ACK:			//PRS��MCU�ı临�ϲ���ȷ��
	case PRS_MCU_REMOVEALL_NACK:		//PRS��MCU�ı临�ϲ����ܾ�
		
	case HDU_MCU_NEEDIFRAME_CMD:        // HDU����ؼ�֡
	case HDU_MCU_START_PLAY_ACK:        //hdu����Ӧ��
	case HDU_MCU_START_PLAY_NACK:       //hdu���žܾ�
	case HDU_MCU_STOP_PLAY_ACK:         //ֹͣ����Ӧ��
    case HDU_MCU_STOP_PLAY_NACK:        //ֹͣ����Ӧ��
	case HDU_MCU_CHGHDUVMPMODE_ACK:		//�л�HDUͨ��Ӧ��
	case HDU_MCU_CHGHDUVMPMODE_NACK:	//�л�HDUͨ��Ӧ��
		EQP_EV_MSG(LOG_LVL_DETAIL);
		ProcEqpToMcuMsg( pcMsg );
		break;

	case TVWALL_MCU_STATUS_NOTIF:	    //����ǽ״̬֪ͨ
    case HDU_MCU_STATUS_NOTIF:          //�������ǽ״̬֪ͨ
	case REC_MCU_RECSTATUS_NOTIF:		//¼���״̬֪ͨ
	case REC_MCU_RECORDCHNSTATUS_NOTIF:	//¼���¼���ŵ�״̬֪ͨ
	case REC_MCU_PLAYCHNSTATUS_NOTIF:	//¼��������ŵ�״̬֪ͨ
	case REC_MCU_RECORDPROG_NOTIF:		//��ǰ¼�����֪ͨ
	case REC_MCU_PLAYPROG_NOTIF:		//��ǰ�������֪ͨ
	case REC_MCU_EXCPT_NOTIF:			//¼����쳣֪ͨ��Ϣ
	
	case MIXER_MCU_MIXERSTATUS_NOTIF:   //������״̬֪ͨ
	
	case BAS_MCU_BASSTATUS_NOTIF:		//������״̬֪ͨ
	case BAS_MCU_CHNNLSTATUS_NOTIF:     //����ͨ��״̬֪ͨ
    case HDBAS_MCU_BASSTATUS_NOTIF:     //����������״̬֪ͨ
    case HDBAS_MCU_CHNNLSTATUS_NOTIF:   //����������ͨ��״̬֪ͨ
    case HDU_MCU_CHNNLSTATUS_NOTIF:     //�������ǽͨ��״̬֪ͨ
    case VMP_MCU_VMPSTATUS_NOTIF:       //����ϳ���״̬֪ͨ
    case VMPTW_MCU_VMPTWSTATUS_NOTIF:   //���ϵ���ǽ״̬֪ͨ
	case PRS_MCU_PRSSTATUS_NOTIF:		//VMP��MCU��״̬�ϱ�
	case PRS_MCU_SETSRC_NOTIF:			//PRS��MCU�ı�����ϢԴ���
//	case PRS_MCU_ADDRESENDCH_NOTIF:		//PRS��MCU��ֹͣ���
//	case PRS_MCU_REMOVERESENDCH_NOTIF:	//PRS��MCU��״̬�ı���
	case PRS_MCU_REMOVEALL_NOTIF:		//PRS��MCU��״̬�ı���

	case REC_MCU_LISTALLRECORD_ACK:     //¼�����Ӧ��(���)��Ϣ
	case REC_MCU_LISTALLRECORD_NACK:    //¼����ܾ�¼���б�����
	case REC_MCU_LISTALLRECORD_NOTIF:	//¼�����¼֪ͨ
    case REC_MCU_DELETERECORD_ACK:      //ɾ����¼Ӧ����Ϣ
	case REC_MCU_DELETERECORD_NACK:     //ɾ����¼�ܾ���Ϣ
    case REC_MCU_RENAMERECORD_ACK:      //���ļ�¼Ӧ����Ϣ
	case REC_MCU_RENAMERECORD_NACK:     //���ļ�¼�ܾ���Ϣ
	case REC_MCU_PUBLISHREC_ACK:		//����¼��Ӧ��
	case REC_MCU_PUBLISHREC_NACK:		//����¼��ܾ�
	case REC_MCU_CANCELPUBLISHREC_ACK:  //ȡ������¼��Ӧ��
	case REC_MCU_CANCELPUBLISHREC_NACK: //ȡ������¼��ܾ�
	case AUDBAS_MCU_CHNNLSTATUS_NOTIF:
		EQP_EV_MSG(LOG_LVL_DETAIL);
		ProcEqpToMcuDaemonConfMsg( pcMsg );
		break;

	case TVWALL_MCU_START_PLAY_ACK:
	case TVWALL_MCU_START_PLAY_NACK:
	case TVWALL_MCU_STOP_PLAY_ACK:
	case TVWALL_MCU_STOP_PLAY_NACK:
		EQP_EV_MSG(LOG_LVL_KEYSTATUS);
		break;
	
    //����qosֵ
    case MCU_EQP_SETQOS_CMD:
        {
			EQP_EV_MSG(LOG_LVL_KEYSTATUS);
            CServMsg cMsg(pcMsg->content, pcMsg->length);                    
            ProcMcuEqpSetQos(cMsg);
        }        
        break;
	// ȡ��������״̬
	case EQP_MCU_GETMSSTATUS_REQ:
    case MCU_MSSTATE_EXCHANGE_NTF:
		{
			EQP_EV_MSG(LOG_LVL_KEYSTATUS);
			ProcEqpGetMsStatusReq(pcMsg);
		}
		break;
    //���û���ϳɷ��
    case MCU_VMP_SETSTYLE_CMD:
        {
			EQP_EV_MSG(LOG_LVL_KEYSTATUS);
            if(EQP_TYPE_VMP == m_byEqpType)
            {
                SendMsgToPeriEqp(pcMsg->event, pcMsg->content, pcMsg->length);
                LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[EqpSsn] send vmp style msg to eqp<%d> success\n", m_byEqpId);
            }
        }
        break;

	case TEST_MCU_DISCONNECTEQP_CMD:
		EQP_EV_MSG(LOG_LVL_KEYSTATUS);
		ProcMcuDisconnectEqp(pcMsg);
		break;
	default:
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[EqpSsn]: Wrong message %u(%s) received! Inst.%d\n", pcMsg->event, 
			::OspEventDesc( pcMsg->event ), GetInsID() );
		break;
	}
}

/*=============================================================================
  �� �� ���� ProcMcuEqpSetQos
  ��    �ܣ� qos msg
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CEqpSsnInst::ProcMcuEqpSetQos(CServMsg &cMsg)
{
    switch( m_byEqpType )
    {
    case EQP_TYPE_TVWALL:        
    case EQP_TYPE_MIXER:  
    case EQP_TYPE_RECORDER:    
    case EQP_TYPE_BAS:      
    case EQP_TYPE_VMP:        
    case EQP_TYPE_VMPTW:
        SendMsgToPeriEqp(cMsg.GetEventId(), cMsg.GetServMsg(), cMsg.GetServMsgLen());
        LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ProcMcuMtSetQos] send qos msg to eqp<%d> success\n", m_byEqpId);
        break;
        
    default:        
        break;
    }
}

/*=============================================================================
  �� �� ���� ProcEqpGetMsStatusReq
  ��    �ܣ� ȡ��������״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CEqpSsnInst::ProcEqpGetMsStatusReq(CMessage* const pcMsg)
{
    // guzh [4/11/2007] ���û��ͨ��ע��ͷ��ͻ�ȡ���󣬺��п�������������ǰ�����ӣ������
    if ( CurState() == STATE_IDLE )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[ProcEqpGetMsStatusReq]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
        
        return;
    }

    if (MCU_MSSTATE_EXCHANGE_NTF == pcMsg->event)
    {
        // ���ͬ��ʧ��,�Ͽ���Ӧ������
        u8 byIsSwitchOk = *pcMsg->content;
        if (0 == byIsSwitchOk)
        {
            OspDisconnectTcpNode( m_dwEqpNode );
            return;
        }        
    }
    
	if( MCU_MSSTATE_ACTIVE == g_cMSSsnApp.GetCurMSState() )
    {
        TMcuMsStatus tMsStatus;
        tMsStatus.SetMsSwitchOK(g_cMSSsnApp.IsMsSwitchOK());
        
        CServMsg cServMsg;
        cServMsg.SetMsgBody((u8*)&tMsStatus, sizeof(tMsStatus));
        SendMsgToPeriEqp(MCU_EQP_GETMSSTATUS_ACK, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

        LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ProcEqpGetMsStatusReq] IsMsSwitchOK :%d.\n", tMsStatus.IsMsSwitchOK());
    }
    return;
}
/*=============================================================================
  �� �� ���� ProcMcuDisconnectEqp
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CEqpSsnInst::ProcMcuDisconnectEqp(CMessage * const pcMsg)
{
	u8 EqpId = *(u8*)pcMsg->content;
	if ( EqpId == m_byEqpId )
	{
		if ( OspIsValidTcpNode( m_dwEqpNode ) ) 
		{
			if ( OspDisconnectTcpNode( m_dwEqpNode ) )
				LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[Int EqpSsn]: The Eqp%u disconnected!\n" );	
			else
				LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[Int EqpSsn]: Disconnect the Eqp%u error!\n" );
			return;
		}
		else
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[Int EqpSsn]: The Eqp%u disconnected before!\n" );
			return ;
		}
	}
}
		
/*====================================================================
    ������      ��DaemonInstanceEntry
    ����        ��ʵ����Ϣ������ں���������override
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
				  , CApp * pcApp
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/05/29    1.0         
====================================================================*/
void CEqpSsnInst::DaemonInstanceEntry( CMessage * const pcMsg, CApp * pcApp )
{
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: The received msg's pointer in the msg entry is NULL!" );
		return;
	}

	if(pcMsg->event != MCU_APPTASKTEST_REQ)
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[Eqpssn Daem]: %u(%s) in\n", pcMsg->event, ::OspEventDesc( pcMsg->event ));
	}
	
	switch( pcMsg->event )
	{
	case MCU_APPTASKTEST_REQ:			//GUARD Probe Message
		DaemonProcAppTaskRequest( pcMsg );
		break;

		// [2/25/2010 xliang] 8ke ����ע������daemon,��������ע�᲻��daemon,ֱ�ӽ���ͨʵ��
	case MIXER_MCU_REG_REQ:				//�������Ǽ�����
#if defined(_8KE_) || defined(_8KH_)|| defined(_8KI_)//������������Ǽ��˱����
        DaemonProc8KMixerMcuRegMsg( pcMsg );
#endif
		break;
    case BAS_MCU_REG_REQ:				//�������Ǽ�����
    case VMP_MCU_REGISTER_REQ:          //����ϳ����Ǽ�����
		//[nizhijun 2011/02/17]prs 8ke���û�
	case PRS_MCU_REGISTER_REQ:			//PRS��MCUע������
#if defined(_8KE_) || defined(_8KH_)|| defined(_8KI_)//������������Ǽ��˱����
        DaemonProcEqpMcuRegMsg( pcMsg );
#endif
		break;
	default:
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: Wrong message %u(%s) received! AppId.%d\n", pcMsg->event, 
			::OspEventDesc( pcMsg->event ),pcApp!=NULL?pcApp->appId:0 );
		break;
	}
}

/*=============================================================================
�� �� ���� IsEqpReged
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const u8 &byEqpType
           const u32 &dwEqpIp
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2009/2/16   4.0		    �ܹ��                  ����
=============================================================================*/
BOOL32 CEqpSsnInst::IsEqpReged( const u8 &byEqpType, const u32 &dwEqpIp )
{
    u8 byMinId = 0;
    u8 byMaxId = 0;

    if( EQP_TYPE_MIXER == byEqpType)
    {
        byMinId = MIXERID_MIN;
        byMaxId = MIXERID_MAX;       
    }

    else if( EQP_TYPE_BAS == byEqpType )
    {
        byMinId = BASID_MIN;
        byMaxId = BASID_MAX; 
    }
    
    else  if( EQP_TYPE_VMP == byEqpType )
    {
        byMinId = VMPID_MIN;
        byMaxId = VMPID_MAX; 
    }

    else if( EQP_TYPE_PRS == byEqpType )
    {
        byMinId = PRSID_MIN;
        byMaxId = PRSID_MAX; 
    }

    else
    {
        return FALSE;
    }
    
    u8 byInstID = 0;
    CEqpSsnInst * pcEqpInst = NULL;
    CApp *pcApp = &g_cEqpSsnApp;
    for (byInstID = byMinId; byInstID <= byMaxId; byInstID++)
    {
        pcEqpInst = (CEqpSsnInst *)pcApp->GetInstance(byInstID);
        if (NULL != pcEqpInst)
        {
            if( STATE_NORMAL == pcEqpInst->CurState() &&
                pcEqpInst->m_dwEqpIpAddr == dwEqpIp )
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/*=============================================================================
�� �� ���� GetIdleInst
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const u8 &byEqpType
�� �� ֵ�� u8 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2009/2/16   4.0		    �ܹ��                  ����
=============================================================================*/
u8 CEqpSsnInst::GetIdleInst( const u8 &byEqpType,u8 byIsAudBas )
{   
    u8 byMinId = 0;
    u8 byMaxId = 0;

    if( EQP_TYPE_MIXER == byEqpType )
    {
        byMinId = MIXERID_MIN;
        byMaxId = MIXERID_MAX;       
    }
    
    if( EQP_TYPE_BAS == byEqpType )
    {
        byMinId = BASID_MIN;
        byMaxId = BASID_MAX; 
		if( 1 == byIsAudBas )
		{
			byMinId = BASID_MIN + 3;
		}
    }
    
    if( EQP_TYPE_VMP == byEqpType )
    {
        byMinId = VMPID_MIN;
        byMaxId = VMPID_MAX; 
    }
    
    if( EQP_TYPE_PRS == byEqpType )
    {
        byMinId = PRSID_MIN;
        byMaxId = PRSID_MAX; 
    }

    u8 byInstID = 0;
    CEqpSsnInst * pcEqpInst = NULL;
    CApp *pcApp = &g_cEqpSsnApp;
    for (byInstID = byMinId; byInstID <= byMaxId; byInstID++)
    {
        pcEqpInst = (CEqpSsnInst *)pcApp->GetInstance(byInstID);
        if (NULL != pcEqpInst)
        {
            if( STATE_IDLE == pcEqpInst->CurState() )
            {
				pcEqpInst->m_curState = STATE_NORMAL;//[03/01/2010] zjl �����Ƚ�ʵ��״̬����normal
                return byInstID;
            }
        }
    }
    return 0;
}


/*=============================================================================
�� �� ���� DaemonProcEqpMcuRegMsg
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2009/2/16   4.0		�ܹ��                  ����
=============================================================================*/
void CEqpSsnInst::DaemonProcEqpMcuRegMsg( const CMessage * pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u32 dwEqpNetId = pcMsg->srcid;
    u32 dwEqpNetNode = pcMsg->srcnode;
	
    TEqpRegReq tRegEqp;    

	const u16 wMsgBodyLen = cServMsg.GetMsgBodyLen();
    memcpy(&tRegEqp, cServMsg.GetMsgBody(), min(sizeof(TEqpRegReq), wMsgBodyLen));
	
    u8  byEqpType = tRegEqp.GetEqpType(); 
    u32 dwEqpIP = 0;
    if ( 0 == dwEqpNetNode )
    {
        dwEqpIP = tRegEqp.GetEqpIpAddr();
        if (dwEqpIP == 0)
        {
            dwEqpIP = ntohl( g_cMcuAgent.GetMpcIp() );      
        }
        
        if (dwEqpIP == 0)
        {
            dwEqpIP = ntohl(INET_ADDR("127.0.0.1"));
        }
    }
    else
    {
        dwEqpIP = ntohl( OspNodeIpGet(dwEqpNetNode) );
    }
    
	tRegEqp.SetEqpIpAddr( dwEqpIP );
    LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "EqpSsn: [DaemonProcEqpMcuRegMsg] Eqp.<%s> reg! \n", StrOfIP( dwEqpIP) );

	/* [03/01/2010] 8000-e pc�����趼Ϊͬһip,���ﲻ��ͬһipУ�飬
	                ��GetIdleInstֱ�ӷ���eqpid����������״̬����Ϊnormal,��֤Ψһ��

    if ( IsEqpReged( byEqpType, dwEqpIP ) )
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "EqpSsn: Reg Eqp.%s<Ip: 0x%x> has been reged ,Nack it\n", 
			tRegEqp.GetEqpAlias(), dwEqpIP );
        
        //Nack 
        cServMsg.SetErrorCode(ERR_MCU_PERIEQP_REGISTERED);
        post(pcMsg->srcid, pcMsg->event + 2, cServMsg.GetServMsg(),
            cServMsg.GetServMsgLen(), pcMsg->srcnode);
        
        ::OspDisconnectTcpNode(pcMsg->srcnode);
        return;
    }
	*/

	u8 byIsAudBas = 0;
	if( EQP_TYPE_BAS == byEqpType && cServMsg.GetMsgBodyLen() > sizeof(TEqpRegReq) )
	{
		byIsAudBas = *(u8*)(cServMsg.GetMsgBody() + sizeof(TEqpRegReq) );
	}

    u8 byIdleInstId = GetIdleInst( byEqpType,byIsAudBas );
    if ( 0 == byIdleInstId )
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "EqpSsn: No idle ssn for Eqp.%s<Ip: 0x%x>, Nack it\n", 
            tRegEqp.GetEqpAlias(), dwEqpIP );
        
        //Nack 
        cServMsg.SetErrorCode(ERR_MCU_NOIDLEEQPSSN);
        post(pcMsg->srcid, pcMsg->event + 2, cServMsg.GetServMsg(),
            cServMsg.GetServMsgLen(), pcMsg->srcnode);
        
        ::OspDisconnectTcpNode(pcMsg->srcnode);
        return;
    }
    tRegEqp.SetEqpId( byIdleInstId );

	LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "EqpSsn: [DaemonProcEqpMcuRegMsg] GetIdleInst.%d, set it to EqpId!\n", byIdleInstId);
	
    cServMsg.SetMsgBody( (u8*)&tRegEqp, sizeof(tRegEqp) );
    cServMsg.CatMsgBody( (u8*)&dwEqpNetId, sizeof(dwEqpNetId) );
    cServMsg.CatMsgBody( (u8*)&dwEqpNetNode, sizeof(dwEqpNetNode) );
    post( MAKEIID(AID_MCU_PERIEQPSSN, byIdleInstId), pcMsg->event, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
    return;
}
/*=============================================================================
�� �� ���� DaemonProc8KMixerMcuRegMsg
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2012/5/17   4.7		    ��־��                  ����
=============================================================================*/
void CEqpSsnInst::DaemonProc8KMixerMcuRegMsg( const CMessage * pcMsg )
{
	//8Kϵ�л�����ͨ����DAEMONע������ȡ���������ã��Ա����������Ӧ��ʼ������
	CServMsg cMixerCfgToEqp;
	TEqpMixerInfo tTempMixerInfo;
	u8 byMixerId = 0;
	u8 byMixMemNum = 0;
	char achAlias [ MAXLEN_ALIAS + 1 ] = {0};
	u16 wEqpRcvStartPort = 0;
	u8 byMixerIdx = 0;
	BOOL32 bIsHasMixer = FALSE;
	for(byMixerIdx = MIXERID_MIN;byMixerIdx <= MIXERID_MAX;byMixerIdx++)
	{
		if (SUCCESS_AGENT != g_cMcuAgent.GetEqpMixerCfg( byMixerIdx , &tTempMixerInfo ))
		{
			LogPrint( LOG_LVL_WARNING, MID_MCU_MIXER,"[DaemonProc8KMixerMcuRegMsg]GetEqpMixerCfg(%d) failed!\n", byMixerIdx);
			break;
		}
		if (tTempMixerInfo.GetMaxChnInGrp() == 0)
		{
			LogPrint( LOG_LVL_WARNING, MID_MCU_MIXER,"[DaemonProc8KMixerMcuRegMsg]GetEqpMixerCfg(%d) MixNum(0)!\n",byMixerIdx);
			continue;
		}
		bIsHasMixer = TRUE;
		byMixerId = tTempMixerInfo.GetEqpId();
		byMixMemNum = tTempMixerInfo.GetMaxChnInGrp();
		wEqpRcvStartPort = tTempMixerInfo.GetEqpRecvPort();
		strcpy(achAlias, tTempMixerInfo.GetAlias());
		LogPrint( LOG_LVL_DETAIL, MID_MCU_MIXER,"[DaemonProc8KMixerMcuRegMsg]GetEqpMixerCfg(%d) MixMemNum(%d) RcvStartPort(%d) Alias(%s)!\n", 
			byMixerId,byMixMemNum,wEqpRcvStartPort,achAlias);
		if (byMixerIdx == MIXERID_MIN) 
		{
			cMixerCfgToEqp.SetMsgBody( (u8*)&byMixerId, sizeof(u8) );
			cMixerCfgToEqp.CatMsgBody( (u8*)&byMixMemNum, sizeof(u8) );
			cMixerCfgToEqp.CatMsgBody( (u8*)&wEqpRcvStartPort, sizeof(u16) );
			cMixerCfgToEqp.CatMsgBody( (u8*)&achAlias[0], sizeof(achAlias) );
		}
		else
		{
			cMixerCfgToEqp.CatMsgBody( (u8*)&byMixerId, sizeof(u8) );
			cMixerCfgToEqp.CatMsgBody( (u8*)&byMixMemNum, sizeof(u8) );
			cMixerCfgToEqp.CatMsgBody( (u8*)&wEqpRcvStartPort, sizeof(u16) );
			cMixerCfgToEqp.CatMsgBody( (u8*)&achAlias[0], sizeof(achAlias) );
		}
/*#ifndef _8KI_*/
		//��Ӧʵ��״̬Ҫ��ת���Ա�����������Ǳ���ͨINSTANCE����������MCUע�ᣬ��8KI����������ͨ���̣�״̬��ת����ͨINSע��ʱ
		CEqpSsnInst * pcEqpInst = NULL;
		CApp *pcApp = &g_cEqpSsnApp;
		pcEqpInst = (CEqpSsnInst *)pcApp->GetInstance(byMixerId);
		if (NULL != pcEqpInst)
		{
			if( STATE_IDLE == pcEqpInst->CurState() )
			{
				pcEqpInst->m_curState = STATE_NORMAL;
			}
		}
		else
		{
			LogPrint( LOG_LVL_DETAIL, MID_MCU_MIXER,"[DaemonProc8KMixerMcuRegMsg]NULL != pcEqpInst EqpId(%d)!\n", byMixerId);
		}
/*#endif*/
	}
	if (bIsHasMixer)
	{
		post(pcMsg->srcid, pcMsg->event + 1, cMixerCfgToEqp.GetServMsg(),cMixerCfgToEqp.GetServMsgLen(), pcMsg->srcnode);
	}
	else
	{
		post(pcMsg->srcid, pcMsg->event + 2, cMixerCfgToEqp.GetServMsg(),cMixerCfgToEqp.GetServMsgLen(), pcMsg->srcnode);
	}
    return;
}

/*=============================================================================
�� �� ���� Proc8KEEqpMcuRegMsg
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2009/12/22   		    Ѧ��                  ����
=============================================================================*/
void CEqpSsnInst::Proc8KEEqpMcuRegMsg(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TEqpRegReq tRegEqp = *(TEqpRegReq *)cServMsg.GetMsgBody();

	//save information to member variable
    m_dwEqpIId = *(u32*)(cServMsg.GetMsgBody()+sizeof(TEqpRegReq));
	//8KG�Լ�8KH������ע��ֱ�Ӵ��������ͨINSTANCE���͹��������ٹ�DAEMONʵ����
	if (MIXER_MCU_REG_REQ == pcMsg->event) 
	{
		m_dwEqpIId = pcMsg->srcid;
		m_dwEqpNode = pcMsg->srcnode;
	}
	else
	{
		m_dwEqpIId = *(u32*)(cServMsg.GetMsgBody()+sizeof(TEqpRegReq));
		m_dwEqpNode = *(u32*)(cServMsg.GetMsgBody()+sizeof(TEqpRegReq)+sizeof(u32));
	}
    m_dwEqpIpAddr  = tRegEqp.GetEqpIpAddr();
    m_byEqpType    = tRegEqp.GetEqpType();
    strncpy(m_achAlias, tRegEqp.GetEqpAlias(), sizeof(m_achAlias)-1);
    m_achAlias[MAXLEN_EQP_ALIAS-1] = '\0';

	LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[EqpSsn]: Rcv Msg %u(%s) from PeriEqp.%s<0x%x Type%u> , InstId=%u, version %d\n", 
		pcMsg->event, ::OspEventDesc(pcMsg->event), tRegEqp.GetEqpAlias(),
                  tRegEqp.GetEqpIpAddr(), tRegEqp.GetEqpType(), GetInsID(), tRegEqp.GetVersion());
    
    switch(CurState())
    {
    case STATE_NORMAL://STATE_IDLE:
        {
            // �����޸�״̬����ֹ����
            //NEXTSTATE(STATE_NORMAL);

            m_byEqpId = (u8)GetInsID();
#if defined(_8KH_) || defined(_8KI_)
            if (m_byEqpType == EQP_TYPE_MIXER && tRegEqp.GetEqpId() != m_byEqpId )
            {
                LogPrint(LOG_LVL_WARNING, MID_MCU_EQP,"[Proc8KEEqpMcuRegMsg] tRegEqp.GetEqpId()(%d) != m_byEqpId(%d)\n", 
					tRegEqp.GetEqpId(), m_byEqpId);
                NEXTSTATE(STATE_IDLE);
                return;
            }
#endif
            u32 dwMcuRecvIP = 0;
            u16 wMcuStartPort = 0;
            u16 wEqpStartPort = 0;
            g_cMpManager.GetSwitchInfo( m_byEqpId, dwMcuRecvIP, wMcuStartPort, wEqpStartPort );

			LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[Proc8KEEqpMcuRegMsg] GetSwitchInfo--wMcuStartPort: %d, wEqpStartPort: %d\n", 
				wMcuStartPort, wEqpStartPort);

            // Ϊ����������ѵ�������������Ҫ��127.0.0.1�ı�����ַ����ת����ת������ע���ϵ������������MCU IP
            if ( dwMcuRecvIP == ntohl( INET_ADDR("127.0.0.1") ) )
            {
                if ( 0 != m_dwEqpNode )
                {
                    dwMcuRecvIP = ntohl(OspNodeLocalIpGet(m_dwEqpNode));
                }
            }      
          
            //��ȡ�汾�� [12/17/2009 xliang] ��У��8000E����汾��
			/*
			u16 wEqpVersion = 0;
            switch(tRegEqp.GetEqpType())
            {
            //������
            case EQP_TYPE_MIXER:
                wEqpVersion = DEVVER_MIXER;
                break;

            //����������
            case EQP_TYPE_BAS:
                wEqpVersion = DEVVER_BAS;
                break;

            //����ϳ���
            case EQP_TYPE_VMP:
                wEqpVersion = DEVVER_VMP;
                break;

            //�����ش�
            case EQP_TYPE_PRS:
                wEqpVersion = DEVVER_PRS;
                break;

            default:
                break;
            }

 
			// �汾��֤ʧ�ܣ��ܾ�ע��
        
			if ( tRegEqp.GetVersion() != 0 &&
				 tRegEqp.GetVersion() != wEqpVersion )
			{
				cServMsg.SetErrorCode(ERR_MCU_EQPVER);
				post(pcMsg->srcid, pcMsg->event + 2,
					 cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);

				LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: Eqp.%d(0x%x) version %d differs from MCU %d, NACK!\n",                       
						  m_byEqpId,
						  tRegEqp.GetEqpIpAddr(),
						  tRegEqp.GetVersion(),
						  wEqpVersion );

				// �����ã�����
				if (m_dwEqpIpAddr != dwMcuRecvIP)
				{
					::OspDisconnectTcpNode(pcMsg->srcnode);
				}

				NEXTSTATE(STATE_IDLE);
				return;
			}
			*/

			// ��־��¼
			//g_cMcuLogMgr.InsertLogItem( "%s.%d<%s> online!\n", GetEqpName(m_byEqpType), m_byEqpId, strofip(m_dwEqpIpAddr) );

			//֪ͨ VC daemon // [12/17/2009 xliang] ��Ϣ�����: ����ע����Ϣ, mcu����ip,port, �������port
			cServMsg.SetMsgBody( (u8*)&tRegEqp, sizeof(tRegEqp) );
			u32 dwIp = htonl(dwMcuRecvIP);
			cServMsg.CatMsgBody( (u8*)&dwIp, sizeof(dwIp) );  // ������
			u16 wPort = htons(wMcuStartPort);
			cServMsg.CatMsgBody( (u8*)&wPort, sizeof(wPort) );  // ������
			wPort = htons(wEqpStartPort);
			cServMsg.CatMsgBody( (u8*)&wPort, sizeof(wPort) );  // ������
			g_cMcuVcApp.SendMsgToDaemonConf(MCU_EQPCONNECTED_NOTIF, 
				cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

			//ACK
			TEqpRegAck tRegAck;
			tRegAck.SetMcuIpAddr(dwMcuRecvIP);  // ����ֻ������MCU���ص�MP
			tRegAck.SetMcuStartPort(wMcuStartPort);
			tRegAck.SetEqpStartPort(wEqpStartPort);
			tRegAck.SetEqpId( m_byEqpId );
			if (tRegEqp.GetEqpType() == EQP_TYPE_MIXER && !g_cMcuVcApp.Set8kxMixerCfg(tRegAck))
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_EQP,"[Proc8KEEqpMcuRegMsg]Mixer(%d) Set8kxMixerCfg() Failed!\n",m_byEqpId);
				NEXTSTATE(STATE_IDLE);
				return;
			}
			//[nizhijun 2011/02/17]8000E�����ö����ش�����������mcucfg.ini���õĲ�����ã�
			//��ʹû��prs���ߣ�Ҳ��mcucfg.ini�����õĲ���������������
			TPrsTimeSpan tPrsTimeSpan;
			TEqpPrsInfo tEqpPrsInfo;
			//����prsidд������Ϊ����8000E��˵��ȡ�����ļ��ж����ش�����ʱ������Ҫ��֪prsid��
			//����д������Ҫ��Ϊ����Ӧ�ӿ��У����ڷ�prsid�Ĺ��ˡ�
			if ( SUCCESS_AGENT == g_cMcuAgent.GetEqpPrsCfg( PRSID_MIN, tEqpPrsInfo ) )
			{
				tPrsTimeSpan.m_wFirstTimeSpan  = htons( tEqpPrsInfo.GetFirstTimeSpan() );
				tPrsTimeSpan.m_wSecondTimeSpan =  htons( tEqpPrsInfo.GetSecondTimeSpan() );
				tPrsTimeSpan.m_wThirdTimeSpan  =  htons( tEqpPrsInfo.GetThirdTimeSpan() );
				tPrsTimeSpan.m_wRejectTimeSpan =  htons( tEqpPrsInfo.GetRejectTimeSpan() );
				cServMsg.SetMsgBody((u8 *)&tRegAck, sizeof(tRegAck));
				cServMsg.CatMsgBody((u8 *)&tPrsTimeSpan, sizeof(tPrsTimeSpan));
			}
			else
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[Proc8KEEqpMcuRegMsg] eqp:%d GetEqpPrsCfg Error!\n",tRegEqp.GetEqpId());
			}
			
			// ����֪ͨMTU�ĳ���, zgc, 2007-04-02
			if( tRegEqp.GetEqpType() == EQP_TYPE_VMP
				|| tRegEqp.GetEqpType() == EQP_TYPE_BAS
				|| tRegEqp.GetEqpType() == EQP_TYPE_MIXER)
			{
				TNetWorkInfo tTempInfo;
				g_cMcuAgent.GetNetWorkInfo( &tTempInfo );
				u16 wMTUSize = DEFAULT_MTU_SIZE; //tTempInfo.GetMTUSize();
				wMTUSize = htons(wMTUSize);
				cServMsg.CatMsgBody( (u8*)&wMTUSize, sizeof(wMTUSize) );
			}
			// ����֪ͨ�Ƿ��������෢��, zgc, 2007-07-25
			if ( tRegEqp.GetEqpType() == EQP_TYPE_MIXER )
			{
				//TEqpMixerInfo tTempMixerInfo;
				//g_cMcuAgent.GetEqpMixerCfg( m_byEqpId, &tTempMixerInfo );
				u8 byIsSendRedundancy = 0;
				cServMsg.CatMsgBody( (u8*)&byIsSendRedundancy, sizeof(byIsSendRedundancy) );
			}
			
			LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[Proc8KEEqpMcuRegMsg] Send Ack to Eqp.Id%d!\n",tRegEqp.GetEqpId());
			SendMsgToPeriEqp( pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			
			// set qos info        
			TQosInfo tAgentQos;
			u16 wRet = g_cMcuAgent.GetQosInfo(&tAgentQos);
			if(SUCCESS_AGENT == wRet)
			{
				TMcuQosCfgInfo tMcuQosCfg;
				tMcuQosCfg.SetQosType(tAgentQos.GetQosType());
				tMcuQosCfg.SetAudLevel(tAgentQos.GetAudioLevel());
				tMcuQosCfg.SetVidLevel(tAgentQos.GetVideoLevel());
				tMcuQosCfg.SetDataLevel(tAgentQos.GetDataLevel());
				tMcuQosCfg.SetSignalLevel(tAgentQos.GetSignalLevel());
				tMcuQosCfg.SetIpServiceType(tAgentQos.GetIpServiceType());
				
				CServMsg cQosMsg;
				cQosMsg.SetEventId(MCU_EQP_SETQOS_CMD);
				cQosMsg.SetMsgBody((u8*)&tMcuQosCfg, sizeof(tMcuQosCfg));            
				ProcMcuEqpSetQos(cQosMsg);            
			}
			else
			{
				LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[ProcEqpMcuRegMsg] get qos info failed, ret:%d!\n", wRet);
			}             
			
			//���ö������ʱ�䣬ͬ���������������
			::OspSetHBParam(m_dwEqpNode, 10, 0);
			//ע���������ʵ��
			::OspNodeDiscCBRegQ(m_dwEqpNode, GetAppID(), GetInsID());
			
			
			break;
        }

    default:
        LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: Wrong message %u(%s) received in current state %u!\n",
                  pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
        break;
    }
}


/*====================================================================
    ������      ��ProcEqpMcuRegMsg
    ����        ����������ĵǼ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/21    1.0         LI Yi         ����
====================================================================*/
void CEqpSsnInst::ProcEqpMcuRegMsg(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TPeriEqpRegReq tRegEqp;

    u8  byEqpId;   //MCU �豸�б��е�ID
    u8  byEqpType = 0; //MCU �豸�б��е�����
    u32 dwEqpIP = 0;   //MCU �豸�б��е�IP

    u32 dwEqpRcvIP;
    //u16 wEqpRcvPort;

    TPeriEqpRegAck tRegAck;
    TPrsTimeSpan tPrsTimeSpan;

	u16 wMsgBodyLen = cServMsg.GetMsgBodyLen();
    memcpy(&tRegEqp, cServMsg.GetMsgBody(), min(sizeof(TPeriEqpRegReq), wMsgBodyLen));

    LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[EqpSsn]: Rcv Msg %u(%s) from PeriEqp.%u Type.%u, InsId.%u, Ver.%d\n", 
                  pcMsg->event, ::OspEventDesc(pcMsg->event),
	              tRegEqp.GetEqpId(), tRegEqp.GetEqpType(), GetInsID(), tRegEqp.GetVersion());

	dwEqpRcvIP  = tRegEqp.GetPeriEqpIpAddr();
    //wEqpRcvPort = tRegEqp.GetStartPort();

    byEqpId  = tRegEqp.GetEqpId();
    m_dwEqpIpAddr = dwEqpRcvIP;

    LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "EqpSsn: Eqp.%u Type%u EqpIp.%s:%u reg request received!\n", 
                  tRegEqp.GetEqpId(), tRegEqp.GetEqpType(),
                  StrOfIP(tRegEqp.GetPeriEqpIpAddr()), tRegEqp.GetStartPort());

    u16 wRet = 0;
    dwEqpIP = 0;
    if (byEqpId >= VMPID_MIN && byEqpId <= VMPID_MAX && tRegEqp.IsHDEqp())
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "EqpSsn: GetPeriInfo Eqp.%u Type%u EqpIp.%s reg as HDVMP!\n", 
                tRegEqp.GetEqpId(), tRegEqp.GetEqpType(), StrOfIP(dwEqpIP) );
    }
    else
    {
        //����豸�б����Ƿ��и�ע������ID
        wRet = g_cMcuAgent.GetPeriInfo(byEqpId, &dwEqpIP, &byEqpType);
        if (SUCCESS_AGENT != wRet)
        {
            LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: Reg Eqp.%d is invalid Eqp id ,Nack it, ret:%d\n", tRegEqp.GetEqpId(), wRet);

            //Nack 
            cServMsg.SetErrorCode(ERR_MCU_REGEQP_INVALID);
            post(pcMsg->srcid, pcMsg->event + 2, cServMsg.GetServMsg(),
                 cServMsg.GetServMsgLen(), pcMsg->srcnode);

            ::OspDisconnectTcpNode(pcMsg->srcnode);
            return;
        }
        else
        {
            LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "EqpSsn: GetPeriInfo Eqp.%u Type%u EqpIp.%s reg info!\n", 
                          tRegEqp.GetEqpId(), tRegEqp.GetEqpType(), StrOfIP(dwEqpIP) );
        }
    }


    //��Eqp��Id�����ж�
    if (byEqpId != (u8)GetInsID())
    {
        //NACK
        LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpId = % , GetInsId() = %d,not equal to InsId, nack!\n", byEqpId, GetInsID());
        cServMsg.SetErrorCode(ERR_MCU_REGEQP_INVALID);
        post(pcMsg->srcid, pcMsg->event + 2,
             cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);
        return;
    }

    
    switch(CurState())
    {
    case STATE_IDLE:
    //zbq[12/11/2007]״̬���ϸ�ת
    //case STATE_NORMAL://?�ظ�ע��
        {
        if (byEqpId >= VMPID_MIN && byEqpId <= VMPID_MAX && tRegEqp.IsHDEqp())
        {
        }
        else
        {
			if( tRegEqp.IsLocal() 
				&& tRegEqp.GetEqpType() == 12 //ԭEQP_TYPE_EMIXERֵΪ12������V4R7��ǰEAPU[2/15/2012 chendaiwei]
				&& byEqpType == EQP_TYPE_MIXER)
			{
				tRegEqp.SetEqpType(EQP_TYPE_MIXER);
			}
			else if (tRegEqp.IsLocal()
					&& byEqpType == EQP_TYPE_HDU
					&& (tRegEqp.GetEqpType() == 13 || //ԭ���壬�����ϰ汾HDU�汾��#define EQP_TYPE_HDU_H 13
					    tRegEqp.GetEqpType() == 14 || //ԭ���壬�����ϰ汾HDU�汾��#define EQP_TYPE_HDU_L 14
						tRegEqp.GetEqpType() == 15 || //ԭ���壬�����ϰ汾HDU�汾��#define EQP_TYPE_HDU2 15
						tRegEqp.GetEqpType() == 12))  //ԭ���壬�����ϰ汾HDU�汾��#define EQP_TYPE_HDU2_L 12
			{
				tRegEqp.SetEqpType(EQP_TYPE_HDU);
			}
            //���Ϸ���
            else if (!tRegEqp.IsLocal() ||
                byEqpType  != tRegEqp.GetEqpType() ||
                dwEqpRcvIP != dwEqpIP)
            {
				
                LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: MCU%uEQP%uType%u reg and was refused!\n",
                    tRegEqp.GetMcuId(),
                    tRegEqp.GetEqpId(),
                    tRegEqp.GetEqpType());

                if ( dwEqpRcvIP != dwEqpIP )
                {
					LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: dwEqpRcvIp(0x%x) is not equal to dwEqpIp(0x%x)\n",
						dwEqpRcvIP, dwEqpIP);
                }
                
                cServMsg.SetErrorCode(ERR_MCU_REGEQP_INVALID);
                post(pcMsg->srcid, pcMsg->event + 2,
                    cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);
                
                //����
                if (m_dwEqpIpAddr != g_cMcuAgent.GetMpcIp())
                {
                    ::OspDisconnectTcpNode(pcMsg->srcnode);
                }				
				
                return;
            }
        }

        u32 dwRcvIP = INADDR_NONE;
        u16 wRcvPort = 0;
        BOOL32 bGetSwitchSuccess = TRUE;
        // MCU Ҫ�������汾��
        u16 wEqpVersion = 0;
		u8 byRecCodeFormat = emenCoding_GBK;

        //��ȡ������ַ�Ͱ汾��
        switch(tRegEqp.GetEqpType())
        {
        //¼���	
        case EQP_TYPE_RECORDER:
            if (!g_cMpManager.GetRecorderSwitchAddr(byEqpId, dwRcvIP, wRcvPort))
            {
                bGetSwitchSuccess = FALSE;
                LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: Get Rec.%d Switch addr failed!\n", byEqpId);
            }
            wEqpVersion = DEVVER_RECORDER;

			//  [5/17/2013 guodawei] ��ȡ�������Ͳ�����
			if (wMsgBodyLen >= sizeof(TPeriEqpRegReq) + sizeof(u8))
			{
				byRecCodeFormat = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TPeriEqpRegReq));
			}
			g_cMcuVcApp.SetEqpCodeFormat(byEqpId, byRecCodeFormat);

            break;

        //������
        case EQP_TYPE_MIXER:
			{
				if (!g_cMpManager.GetMixerSwitchAddr(byEqpId, dwRcvIP, wRcvPort))
				{
					bGetSwitchSuccess = FALSE;
					LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: Get Mix.%d Switch addr failed!\n", byEqpId);
				}
				u8 byMixerSubtype = UNKONW_MIXER;
				g_cMcuAgent.GetMixerSubTypeByEqpId(byEqpId,byMixerSubtype);
				switch (byMixerSubtype)
				{
				case APU_MIXER:
					wEqpVersion = DEVVER_MIXER;
					break;
				case EAPU_MIXER:
					wEqpVersion = DEVVER_EMIXER;
					break;
				case APU2_MIXER:
					wEqpVersion = DEVVER_APU2;
					break; 
				default:
					wEqpVersion = 0;
					break;
				}
				break;
			}

        //����������
        case EQP_TYPE_BAS:
            if (!g_cMpManager.GetBasSwitchAddr(byEqpId, dwRcvIP, wRcvPort))
            {
                bGetSwitchSuccess = FALSE;
                LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: Get BAS.%d Switch addr failed!\n", byEqpId);
            }              
			// xliang [1/8/2009] BAS Develop Version defers:
			//APU2 Bas����IF����[3/22/2013 chendaiwei]
			if(g_cMcuAgent.IsMpuBas(byEqpId) )
			{
				//wEqpVersion = DEVVER_MPU;
				wEqpVersion = g_cMcuAgent.GetBasEqpVersion(byEqpId);
			}
            else
			{
				wEqpVersion = g_cMcuAgent.IsEqpBasHD( byEqpId ) ? DEVVER_HDBAS : DEVVER_BAS;
            }
            break;

        //����ϳ���
        case EQP_TYPE_VMP:
            if (!g_cMpManager.GetVmpSwitchAddr(byEqpId, dwRcvIP, wRcvPort))
            {
                bGetSwitchSuccess = FALSE;
                LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: Get VMP.%d Switch addr failed!\n", byEqpId);
            }
			// xliang [1/8/2009] VMP Develop Version defers:
			if( g_cMcuAgent.IsSVmp(byEqpId) 
				//|| g_cMcuAgent.IsDVmp(byEqpId) 
				//|| g_cMcuAgent.IsEVpu(byEqpId) 
				)
			{
				//wEqpVersion = DEVVER_MPU;
				wEqpVersion = g_cMcuAgent.GetVmpEqpVersion(byEqpId);
			}
			else
			{
				wEqpVersion = DEVVER_VMP;
			}	
            break;

        //�����ش�
        case EQP_TYPE_PRS:
            if (!g_cMpManager.GetPrsSwitchAddr(byEqpId, dwRcvIP, wRcvPort))
            {
                bGetSwitchSuccess = FALSE;
                LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: Get Prs.%d Switch addr failed!\n", byEqpId);
            }
            wEqpVersion = DEVVER_PRS;
            break;
        //����ǽ
        case EQP_TYPE_TVWALL:
            wEqpVersion = DEVVER_TW;    
            break;
        //�໭�����ǽ
        case EQP_TYPE_VMPTW:
            wEqpVersion = DEVVER_MPW;    
            break;
		//�������ǽ
		case EQP_TYPE_HDU://HDU // xliang [12/31/2008] 
			wEqpVersion = g_cMcuAgent.GetHDUEqpVersion(byEqpId);
			break;
        default:
            break;
        }

        //��ȡ������ַʧ�ܣ��ܾ�ע��
        if (!bGetSwitchSuccess)
        {
            cServMsg.SetErrorCode(ERR_MCU_REGEQP_INVALID);
            post(pcMsg->srcid, pcMsg->event + 2,
                 cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);

            //����
            if (m_dwEqpIpAddr != g_cMcuAgent.GetMpcIp())
            {
                ::OspDisconnectTcpNode(pcMsg->srcnode);
            }
            return;
        }

        // �汾��֤ʧ�ܣ��ܾ�ע��
        if ( tRegEqp.GetVersion() != wEqpVersion )
        {
            cServMsg.SetErrorCode(ERR_MCU_VER_UNMATCH);
            post(pcMsg->srcid, pcMsg->event + 2,
                 cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);

            LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: Eqp.%d(0x%x) version %d differs from MCU %d, NACK!\n",                       
                      GetInsID(),
                      tRegEqp.GetPeriEqpIpAddr(),
                      tRegEqp.GetVersion(),
                      wEqpVersion );


            // �����ã�����
            if (m_dwEqpIpAddr != g_cMcuAgent.GetMpcIp())
            {
                ::OspDisconnectTcpNode(pcMsg->srcnode);
            }
            return;
        }

        //ACK
        tRegAck.SetMcuIpAddr(dwRcvIP);
        tRegAck.SetMcuStartPort(wRcvPort);
        if (g_cMSSsnApp.IsDoubleLink())
        {
            tRegAck.SetAnotherMpcIp(g_cMSSsnApp.GetAnotherMcuIP());
        }
        else
        {
            tRegAck.SetAnotherMpcIp(0);
        }
		u32 dwSysSSrc = g_cMSSsnApp.GetMSSsrc();
        LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[Ssn: ProcEqpMcuRegMsg] Reg Ack with System SSRC.%u.\n", dwSysSSrc);
        tRegAck.SetMSSsrc(dwSysSSrc);

#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
		if ( tRegEqp.GetEqpType() == EQP_TYPE_HDU ||
			 tRegEqp.GetEqpType() == EQP_TYPE_RECORDER ||
			 tRegEqp.GetEqpType() == EQP_TYPE_MIXER
			)
		{
			//[nizhijun 2011/02/17]8000H�����ö����ش�����������mcucfg.ini���õĲ�����ã�
			//��ʹû��prs���ߣ�Ҳ��mcucfg.ini�����õĲ���������������
			TEqpPrsInfo tEqpPrsInfo;
			//����prsidд������Ϊ����8000h��˵��ȡ�����ļ��ж����ش�����ʱ������Ҫ��֪prsid��
			//����д������Ҫ��Ϊ����Ӧ�ӿ��У����ڷ�prsid�Ĺ��ˡ�
			if ( SUCCESS_AGENT == g_cMcuAgent.GetEqpPrsCfg( PRSID_MIN, tEqpPrsInfo ) )
			{
				tPrsTimeSpan.m_wFirstTimeSpan  = htons( tEqpPrsInfo.GetFirstTimeSpan() );
				tPrsTimeSpan.m_wSecondTimeSpan =  htons( tEqpPrsInfo.GetSecondTimeSpan() );
				tPrsTimeSpan.m_wThirdTimeSpan  =  htons( tEqpPrsInfo.GetThirdTimeSpan() );
				tPrsTimeSpan.m_wRejectTimeSpan =  htons( tEqpPrsInfo.GetRejectTimeSpan() );
				cServMsg.SetMsgBody((u8 *)&tRegAck, sizeof(tRegAck));
				cServMsg.CatMsgBody((u8 *)&tPrsTimeSpan, sizeof(tPrsTimeSpan));
			}
			else
			{
				LogPrint(LOG_LVL_ERROR,MID_MCU_EQP,"[Proc8KEEqpMcuRegMsg] eqp:%d GetEqpPrsCfg Error!\n",tRegEqp.GetEqpId());
			}
		}
#else      
        GetPrsTimeSpan(&tPrsTimeSpan);
        tPrsTimeSpan.m_wFirstTimeSpan = htons(tPrsTimeSpan.m_wFirstTimeSpan);
        tPrsTimeSpan.m_wSecondTimeSpan = htons(tPrsTimeSpan.m_wSecondTimeSpan);
        tPrsTimeSpan.m_wThirdTimeSpan = htons(tPrsTimeSpan.m_wThirdTimeSpan);
        tPrsTimeSpan.m_wRejectTimeSpan = htons(tPrsTimeSpan.m_wRejectTimeSpan);
        cServMsg.SetMsgBody((u8 *)&tRegAck, sizeof(tRegAck));
        cServMsg.CatMsgBody((u8 *)&tPrsTimeSpan, sizeof(tPrsTimeSpan));
#endif
        // �����¼�����֪ͨMCU����
        if ( tRegEqp.GetEqpType() == EQP_TYPE_RECORDER )
        {
			/*guoidawei*/
			s8 szMcuAlias[MAX_ALIAS_LENGTH] = {0};
			s8 szMcuGBKAlias[MAX_ALIAS_LENGTH] = {0};
			g_cMcuAgent.GetMcuAlias( szMcuAlias, MAX_ALIAS_LENGTH );
			szMcuAlias[MAX_ALIAS_LENGTH - 1] = '\0';
#ifdef _UTF8
			if (byRecCodeFormat == emenCoding_GBK)
			{
				//UTF8-GBKת��
				utf8_to_gb2312(szMcuAlias, szMcuGBKAlias, sizeof(szMcuGBKAlias) - 1);
				memcpy(szMcuAlias, szMcuGBKAlias, sizeof(szMcuGBKAlias));
			}
#endif
			cServMsg.CatMsgBody( (u8*)szMcuAlias, MAX_ALIAS_LENGTH );
        }
        
		// ����֪ͨMTU�ĳ���, zgc, 2007-04-02
		if( tRegEqp.GetEqpType() == EQP_TYPE_VMP
		 || tRegEqp.GetEqpType() == EQP_TYPE_BAS
		 || tRegEqp.GetEqpType() == EQP_TYPE_MIXER)
		{
			TNetWorkInfo tTempInfo;
			g_cMcuAgent.GetNetWorkInfo( &tTempInfo );
			u16 wMTUSize = tTempInfo.GetMTUSize();
			wMTUSize = htons(wMTUSize);
			cServMsg.CatMsgBody( (u8*)&wMTUSize, sizeof(wMTUSize) );
		}
		// ����֪ͨ�Ƿ��������෢��, zgc, 2007-07-25
		if ( tRegEqp.GetEqpType() == EQP_TYPE_MIXER )
		{
			TEqpMixerInfo tTempMixerInfo;
			g_cMcuAgent.GetEqpMixerCfg( byEqpId, &tTempMixerInfo );
			u8 byIsSendRedundancy = tTempMixerInfo.IsSendRedundancy() ? 1 : 0;
			cServMsg.CatMsgBody( (u8*)&byIsSendRedundancy, sizeof(byIsSendRedundancy) );
		}

        // BAS-HD ���պͷ���������ʼ�˿ڡ���������Ϣ��һ����ͬ����ע����Ϣ��,��һ����֪ͨ���衣guzh 2008-08-05
        if( tRegEqp.GetEqpType() == EQP_TYPE_BAS &&
            tRegEqp.IsHDEqp() )
        {
            TEqpBasHDInfo tInfo;
            TEqpBasHDCfgInfo tCfgInfo;
            g_cMcuAgent.GetEqpBasHDCfg(tRegEqp.GetEqpId(), &tInfo);
            tCfgInfo.SetAlias(tInfo.GetAlias());
            tCfgInfo.SetEqpStartPort(tInfo.GetEqpRecvPort());
            tCfgInfo.SetType(tInfo.GetHDBasType());
			tCfgInfo.SetEqpId( tInfo.GetEqpId() );

            tRegEqp.SetEqpAlias( (s8*)tCfgInfo.GetAlias() );
            tRegEqp.SetStartPort( tCfgInfo.GetEqpStartPort() );
            cServMsg.CatMsgBody((u8*)&tCfgInfo, sizeof(tCfgInfo));
        }
        else if (tRegEqp.GetEqpType() == EQP_TYPE_VMP &&
                 tRegEqp.IsHDEqp())
        {
            TEqpBasHDCfgInfo tCfgInfo;
            tCfgInfo.SetAlias("vmphd");
            tCfgInfo.SetEqpStartPort(VMP_EQP_STARTPORT);
            cServMsg.CatMsgBody((u8*)&tCfgInfo, sizeof(tCfgInfo));
            g_cMcuVcApp.SetPeriEqpIsValid( tRegEqp.GetEqpId() );
        }
		
        if (tRegEqp.GetEqpType() == EQP_TYPE_HDU   /*||
			tRegEqp.GetEqpType() == EQP_TYPE_HDU_H ||
			tRegEqp.GetEqpType() == EQP_TYPE_HDU_L ||
			tRegEqp.GetEqpType() == EQP_TYPE_HDU2  ||
			tRegEqp.GetEqpType() == EQP_TYPE_HDU2_L*/)
        {
			TEqpHduInfo tInfo;
			TEqpHduCfgInfo tCfgInfo;
			g_cMcuAgent.GetEqpHduCfg(tRegEqp.GetEqpId(), &tInfo);
			tCfgInfo.SetAlias( tInfo.GetAlias() );
			tCfgInfo.SetEqpStartPort( tInfo.GetEqpRecvPort() );
            THduChnlModePortAgt tHduChnModePortAgt;
			THduChnlModePort tHduChnModePort;
			memset(&tHduChnModePortAgt, 0x0, sizeof(tHduChnModePortAgt));
			memset(&tHduChnModePort, 0x0, sizeof(tHduChnModePort));
            tInfo.GetHduChnlModePort(0, tHduChnModePortAgt);
            tHduChnModePort.SetOutModeType(tHduChnModePortAgt.GetOutModeType());
			tHduChnModePort.SetOutPortType(tHduChnModePortAgt.GetOutPortType());
			tHduChnModePort.SetZoomRate(tHduChnModePortAgt.GetZoomRate());
			tCfgInfo.SetHduChnlModePort(0, tHduChnModePort);
            tInfo.GetHduChnlModePort(1, tHduChnModePortAgt);
            tHduChnModePort.SetOutModeType(tHduChnModePortAgt.GetOutModeType());
			tHduChnModePort.SetOutPortType(tHduChnModePortAgt.GetOutPortType());
			tHduChnModePort.SetZoomRate( tHduChnModePortAgt.GetZoomRate() );
			tCfgInfo.SetHduChnlModePort(1, tHduChnModePort);

            tRegEqp.SetEqpAlias( (s8*)tCfgInfo.GetAlias() );
            tRegEqp.SetStartPort( tCfgInfo.GetEqpStartPort() );
            cServMsg.CatMsgBody((u8*)&tCfgInfo, sizeof(tCfgInfo));

        }


        if ( tRegEqp.GetEqpType() == EQP_TYPE_VMP )
        {
#ifdef _UTF8
            //[4/8/2013 liaokang] ���뷽ʽ
            u8 byEncoding = emenCoding_Utf8;
            cServMsg.CatMsgBody( &byEncoding, sizeof(u8));
#endif
        }

        post(pcMsg->srcid, pcMsg->event + 1,
             cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);
                
        //save information
        m_dwEqpNode = pcMsg->srcnode;
        m_dwEqpIId  = pcMsg->srcid;
        m_byEqpId   = byEqpId;
        m_byEqpType = tRegEqp.GetEqpType();

		//  [5/16/2013 guodawei] ¼���������UTF8ת��
#ifdef _UTF8
		if (tRegEqp.GetEqpType() == EQP_TYPE_RECORDER && byRecCodeFormat == emenCoding_GBK)
		{
			s8 achUTF8Alias[MAXLEN_EQP_ALIAS] = {0};

			//��GBK-UTF8ת��
			gb2312_to_utf8(tRegEqp.GetEqpAlias(), achUTF8Alias, sizeof(achUTF8Alias) - 1);
			memcpy(m_achAlias, achUTF8Alias, MAXLEN_EQP_ALIAS);
		}
		else
		{
			memcpy(m_achAlias, tRegEqp.GetEqpAlias(), MAXLEN_EQP_ALIAS);
		}
#else
        memcpy(m_achAlias, tRegEqp.GetEqpAlias(), MAXLEN_EQP_ALIAS);
#endif

        m_achAlias[MAXLEN_EQP_ALIAS-1] = '\0';

        // set qos info        
        TQosInfo tAgentQos;
        wRet = g_cMcuAgent.GetQosInfo(&tAgentQos);
        if(SUCCESS_AGENT == wRet)
        {
            TMcuQosCfgInfo tMcuQosCfg;
            tMcuQosCfg.SetQosType(tAgentQos.GetQosType());
            tMcuQosCfg.SetAudLevel(tAgentQos.GetAudioLevel());
            tMcuQosCfg.SetVidLevel(tAgentQos.GetVideoLevel());
            tMcuQosCfg.SetDataLevel(tAgentQos.GetDataLevel());
            tMcuQosCfg.SetSignalLevel(tAgentQos.GetSignalLevel());
            tMcuQosCfg.SetIpServiceType(tAgentQos.GetIpServiceType());
            
            CServMsg cQosMsg;
            cQosMsg.SetEventId(MCU_EQP_SETQOS_CMD);
            cQosMsg.SetMsgBody((u8*)&tMcuQosCfg, sizeof(tMcuQosCfg));            
            ProcMcuEqpSetQos(cQosMsg);            
        }
        else
        {
            LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[ProcEqpMcuRegMsg] get qos info failed, ret:%d!\n", wRet);
        }             
             
		//::OspSetHBParam(pcMsg->srcnode, 20, 5);
		
        //ע���������ʵ��
        ::OspNodeDiscCBRegQ(pcMsg->srcnode, GetAppID(), GetInsID());

        //֪ͨ VC daemon
        cServMsg.SetMsgBody((u8*)&tRegEqp, sizeof(tRegEqp));

        g_cMcuVcApp.SendMsgToDaemonConf(MCU_EQPCONNECTED_NOTIF, 
                                        cServMsg.GetServMsg(),
                                        cServMsg.GetServMsgLen());

        NEXTSTATE(STATE_NORMAL);

        break;
        }

    default:
        LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: [ProcEqpMcuRegMsg]Wrong message %u(%s) received in current state %u!\n",
                  pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
        break;
    }
}
/*====================================================================
    ������      Proc8KiMixerMcuRegMsg
    ����        ������8KI����������ĵǼ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    12/05/18    4.7         ��־��         ����
====================================================================*/
void CEqpSsnInst::Proc8KiMixerMcuRegMsg(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TPeriEqpRegReq tRegEqp;
    TPeriEqpRegAck tRegAck;
    TPrsTimeSpan tPrsTimeSpan;

	const u16 wMsgBodyLen = cServMsg.GetMsgBodyLen();
    memcpy(&tRegEqp, cServMsg.GetMsgBody(), min(sizeof(TPeriEqpRegReq), wMsgBodyLen));

    LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[Proc8KiMixerMcuRegMsg]: Rcv Msg %u(%s) from PeriEqp.%u Type.%u, InsId.%u, Ver.%d\n", 
                  pcMsg->event, ::OspEventDesc(pcMsg->event),
                  tRegEqp.GetEqpId(), tRegEqp.GetEqpType(), GetInsID(), tRegEqp.GetVersion());

    m_dwEqpIpAddr = tRegEqp.GetPeriEqpIpAddr();

    LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[Proc8KiMixerMcuRegMsg]: Eqp.%u Type%u EqpIp.%s:%u reg request received!\n", 
                  tRegEqp.GetEqpId(), tRegEqp.GetEqpType(),
                  StrOfIP(tRegEqp.GetPeriEqpIpAddr()), tRegEqp.GetStartPort());


    //��Eqp��Id�����ж�
    if (tRegEqp.GetEqpId() != (u8)GetInsID())
    {
        //NACK
        LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[Proc8KiMixerMcuRegMsg]EqpId = % , GetInsId() = %d,not equal to InsId, nack!\n", tRegEqp.GetEqpId(), GetInsID());
        cServMsg.SetErrorCode(ERR_MCU_REGEQP_INVALID);
        post(pcMsg->srcid, pcMsg->event + 2,
             cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);
        return;
    }

    
    switch(CurState())
    {
    case STATE_IDLE:
        {
		u16 wEqpVersion = 0;
		u8 byMixerSubtype = UNKONW_MIXER;
		g_cMcuAgent.GetMixerSubTypeByEqpId(tRegEqp.GetEqpId(),byMixerSubtype);
		switch (byMixerSubtype)
		{
		case APU_MIXER:
			wEqpVersion = DEVVER_MIXER;
			break;
		case EAPU_MIXER:
			wEqpVersion = DEVVER_EMIXER;
			break;
		case APU2_MIXER:
			wEqpVersion = DEVVER_APU2;
			break; 
		default:
			wEqpVersion = 0;
			break;
		}
		if (!tRegEqp.IsLocal() || tRegEqp.GetVersion() != wEqpVersion)
        {
			
            LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[Proc8KiMixerMcuRegMsg]: MCU(%u)EQP(%u) Ver(%d) LocalVer(%d) reg and was refused!\n",
                tRegEqp.IsLocal(),tRegEqp.GetEqpId(),tRegEqp.GetVersion(),wEqpVersion);
            post(pcMsg->srcid, pcMsg->event + 2,cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);
            return;
        }
        //ACK
		TEqpMixerInfo tTempMixerInfo;
		if (SUCCESS_AGENT != g_cMcuAgent.GetEqpMixerCfg( tRegEqp.GetEqpId() , &tTempMixerInfo ))
		{
			LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR,"[Proc8KiMixerMcuRegMsg] GetEqpMixerCfg(%d) failed!\n", tRegEqp.GetEqpId());
			return;	
		}
        tRegAck.SetMcuIpAddr(m_dwEqpIpAddr);
        tRegAck.SetMcuStartPort(tTempMixerInfo.GetMcuRecvPort());
        if (g_cMSSsnApp.IsDoubleLink())
        {
            tRegAck.SetAnotherMpcIp(g_cMSSsnApp.GetAnotherMcuIP());
        }
        else
        {
            tRegAck.SetAnotherMpcIp(0);
        }
		u32 dwSysSSrc = g_cMSSsnApp.GetMSSsrc();
        LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[Proc8KiMixerMcuRegMsg] Reg Ack with System SSRC.%u.\n", dwSysSSrc);
        tRegAck.SetMSSsrc(dwSysSSrc);
  
        GetPrsTimeSpan(&tPrsTimeSpan);
        tPrsTimeSpan.m_wFirstTimeSpan = htons(tPrsTimeSpan.m_wFirstTimeSpan);
        tPrsTimeSpan.m_wSecondTimeSpan = htons(tPrsTimeSpan.m_wSecondTimeSpan);
        tPrsTimeSpan.m_wThirdTimeSpan = htons(tPrsTimeSpan.m_wThirdTimeSpan);
        tPrsTimeSpan.m_wRejectTimeSpan = htons(tPrsTimeSpan.m_wRejectTimeSpan);
        cServMsg.SetMsgBody((u8 *)&tRegAck, sizeof(tRegAck));
        cServMsg.CatMsgBody((u8 *)&tPrsTimeSpan, sizeof(tPrsTimeSpan));
        
		// ����֪ͨMTU�ĳ���
		TNetWorkInfo tTempInfo;
		g_cMcuAgent.GetNetWorkInfo( &tTempInfo );
		u16 wMTUSize = tTempInfo.GetMTUSize();
		wMTUSize = htons(wMTUSize);
		cServMsg.CatMsgBody( (u8*)&wMTUSize, sizeof(wMTUSize) );
		// ����֪ͨ�Ƿ��������෢��
		u8 byIsSendRedundancy = tTempMixerInfo.IsSendRedundancy() ? 1 : 0;
		cServMsg.CatMsgBody( (u8*)&byIsSendRedundancy, sizeof(byIsSendRedundancy) );

        post(pcMsg->srcid, pcMsg->event + 1,
             cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);
                
        //save information
        m_dwEqpNode = pcMsg->srcnode;
        m_dwEqpIId  = pcMsg->srcid;
        m_byEqpId   = tRegEqp.GetEqpId();
        m_byEqpType = tRegEqp.GetEqpType();
        memcpy(m_achAlias, tRegEqp.GetEqpAlias(), MAXLEN_EQP_ALIAS);
        m_achAlias[MAXLEN_EQP_ALIAS-1] = 0;

        // set qos info        
        TQosInfo tAgentQos;
        u16 wRet = g_cMcuAgent.GetQosInfo(&tAgentQos);
        if(SUCCESS_AGENT == wRet)
        {
            TMcuQosCfgInfo tMcuQosCfg;
            tMcuQosCfg.SetQosType(tAgentQos.GetQosType());
            tMcuQosCfg.SetAudLevel(tAgentQos.GetAudioLevel());
            tMcuQosCfg.SetVidLevel(tAgentQos.GetVideoLevel());
            tMcuQosCfg.SetDataLevel(tAgentQos.GetDataLevel());
            tMcuQosCfg.SetSignalLevel(tAgentQos.GetSignalLevel());
            tMcuQosCfg.SetIpServiceType(tAgentQos.GetIpServiceType());
            
            CServMsg cQosMsg;
            cQosMsg.SetEventId(MCU_EQP_SETQOS_CMD);
            cQosMsg.SetMsgBody((u8*)&tMcuQosCfg, sizeof(tMcuQosCfg));            
            ProcMcuEqpSetQos(cQosMsg);            
        }
        else
        {
            LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[Proc8KiMixerMcuRegMsg] get qos info failed, ret:%d!\n", wRet);
        }             
                
        //ע���������ʵ��
        ::OspNodeDiscCBRegQ(pcMsg->srcnode, GetAppID(), GetInsID());

        //֪ͨ VC daemon
        cServMsg.SetMsgBody((u8*)&tRegEqp, sizeof(tRegEqp));
        g_cMcuVcApp.SendMsgToDaemonConf(MCU_EQPCONNECTED_NOTIF, 
                                        cServMsg.GetServMsg(),
                                        cServMsg.GetServMsgLen());

        NEXTSTATE(STATE_NORMAL);

        break;
        }

    default:
        LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[Proc8KiMixerMcuRegMsg]Wrong message %u(%s) received in current state %u!\n",
                  pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
        break;
    }
}
/*====================================================================
    ������      ��ProcEqpDisconnect
    ����        ���������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/21    1.0         LI Yi        ����    
====================================================================*/
void CEqpSsnInst::ProcEqpDisconnect( const CMessage * pcMsg )
{
    LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP,  "EqpSsn: Rcv Msg %u(%s) ! Eqp%u.Type%u disconnected!\n", 
        pcMsg->event, ::OspEventDesc( pcMsg->event ), m_byEqpId, m_byEqpType );
    
	CServMsg	cServMsg;
	TMt	tEqp;

	switch( CurState() )
	{
	case STATE_NORMAL:
		if( *( u32 * )pcMsg->content == m_dwEqpNode )	//��ʵ����Ӧ���Ӷ�
		{		
            if (INVALID_NODE != m_dwEqpNode)
            {
                OspDisconnectTcpNode(m_dwEqpNode);
            }
            
			//send notification to VC Daemon
			tEqp.SetMcuEqp( LOCAL_MCUID, m_byEqpId, m_byEqpType );
			cServMsg.SetMsgBody( ( u8 * )&tEqp, sizeof( tEqp ) );
			g_cMcuVcApp.SendMsgToDaemonConf( MCU_EQPDISCONNECTED_NOTIF, cServMsg.GetServMsg(), 
				cServMsg.GetServMsgLen() );

			//clear
			DeleteAlias();
			m_dwEqpNode = INVALID_NODE;
			m_byEqpId   = 0;
			m_byEqpType = 0;

			NEXTSTATE( STATE_IDLE );
		}
		break;
	default:
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "CEqpInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcEqpToMcuMsg
    ����        ���������赽MCU��һ����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/19    1.0         Ѧ����        ����
    02/08/25    1.0         LI Yi         �޸�
====================================================================*/
void CEqpSsnInst::ProcEqpToMcuMsg( const CMessage * pcMsg )
{
	CServMsg	cServMsg;

    LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[Eqpssn]: Recv %u(%s) from eqp.%d!\n",
                  pcMsg->event, ::OspEventDesc(pcMsg->event), m_byEqpId);

	switch( CurState() )
	{
	case STATE_NORMAL:
		cServMsg.SetServMsg( pcMsg->content, pcMsg->length );
		if(PRS_MCU_SETSRC_ACK == pcMsg->event ||
		   PRS_MCU_REMOVEALL_ACK == pcMsg->event)
		{
			cServMsg.SetMsgBody(&m_byEqpId, sizeof(m_byEqpId));
		}
		if( !cServMsg.GetConfId().IsNull() )
		{
			// [1/7/2010 xliang] Set srcSsnId value manually for BAS_MCU_NEEDIFRAME_CMD
			if(BAS_MCU_NEEDIFRAME_CMD == pcMsg->event  || 
			   HDU_MCU_START_PLAY_ACK == pcMsg->event  ||
			   HDU_MCU_STOP_PLAY_ACK  == pcMsg->event  ||
			   VMP_MCU_STARTVIDMIX_NOTIF == pcMsg->event ||
			   VMP_MCU_STOPVIDMIX_NOTIF == pcMsg->event ||
			   VMP_MCU_CHANGESTATUS_NOTIF == pcMsg->event ||
			   VMP_MCU_NEEDIFRAME_CMD == pcMsg->event
			   )
			{
// 				LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[Eqpssn]: recv BAS_MCU_NEEDIFRAME_CMD in EqpSsnInst.%d ---- cServMsg.GetSrcSsnId() is %d !\n",
// 					GetInsID(), cServMsg.GetSrcSsnId());
				cServMsg.SetSrcSsnId((u8)GetInsID());	
			}
			g_cMcuVcApp.SendMsgToConf( cServMsg.GetConfId(), pcMsg->event, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		}	
		break;
	default:
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: [ProcEqpToMcuMsg]Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��ProcMcuToEqpMsg
    ����        ������ҵ��������һ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/19    1.0         Ѧ����        ����
====================================================================*/
void CEqpSsnInst::ProcMcuToEqpMsg(const CMessage *pcMsg)
{
    LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[Eqpssn]: Send %u(%s) to eqp.%d!\n",
                  pcMsg->event, ::OspEventDesc(pcMsg->event), m_byEqpId);

	switch( CurState() )
	{
	case STATE_NORMAL:
		SendMsgToPeriEqp( pcMsg->event, pcMsg->content, pcMsg->length );
		break;
	default:

		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn:[ProcMcuToEqpMsg] Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );

		break;
	}
}


/*====================================================================
    ������      ��ProcEqpToMcuDaemonConfMsg
    ����        ���������赽MCU Daemonʵ����һ����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/10    1.0         LI Yi         ����
====================================================================*/
void CEqpSsnInst::ProcEqpToMcuDaemonConfMsg( const CMessage * pcMsg )
{
	switch( CurState() )
	{
	case STATE_NORMAL:
		g_cMcuVcApp.SendMsgToDaemonConf( pcMsg->event, pcMsg->content, pcMsg->length );
		break;
	default:
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: [ProcEqpToMcuDaemonConfMsg]Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    ������      ��SendMsgToPeriEqp
    ����        ������Ϣ����ʵ����Ӧ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wEvent, �¼���
				  u8 * const pbyMsg, ���͵���Ϣָ�룬ȱʡΪNULL
				  u16 wLen, ��Ϣ���ȣ�ȱʡΪ0
    ����ֵ˵��  ���ɹ�����TRUE����������δ�ǼǷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/20    1.0         LI Yi         ����
====================================================================*/
BOOL32 CEqpSsnInst::SendMsgToPeriEqp( u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	if( OspIsValidTcpNode( m_dwEqpNode ) || m_dwEqpIpAddr == g_cMcuAgent.GetMpcIp() )
	{
		post( m_dwEqpIId, wEvent, pbyMsg, wLen, m_dwEqpNode );
		return( TRUE );
	}
	else
	{
		log( LOGLVL_IMPORTANT, "CEqpSsnInst: PeriEqp%u is offline now: Node=%u, CurState=%u, InstId=%u\n", 
			m_byEqpId, m_dwEqpNode, CurState(), GetInsID() );
		return( FALSE );
	}
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
    03/04/21    1.0         ���         ����
====================================================================*/
void CEqpSsnInst::DaemonProcAppTaskRequest( const CMessage * pcMsg )
{
	post( pcMsg->srcid, MCU_APPTASKTEST_ACK, pcMsg->content, pcMsg->length );
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEqpConfig::CEqpConfig()
{

}

CEqpConfig::~CEqpConfig()
{
}

/*====================================================================
    ������      ��SendMsgToPeriEqpSsn
    ����        ������Ϣ��ָ���������Ӧ�ĻỰʵ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byPeriEqpId, ָ���������
				  u16 wEvent, �¼���
				  u8 * const pbyMsg, ���͵���Ϣָ�룬ȱʡΪNULL
				  u16 wLen, ��Ϣ���ȣ�ȱʡΪ0
    ����ֵ˵��  ���ɹ�����TRUE����������δ�ǼǷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/19    1.0         Ѧ����        ����
====================================================================*/
BOOL32 CEqpConfig::SendMsgToPeriEqpSsn( u8 byPeriEqpId, u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	if (byPeriEqpId > MAXNUM_MCU_PERIEQP || 0 == byPeriEqpId)
	{
#ifdef _DEBUG
		::OspLog( LOGLVL_IMPORTANT, "CEqpConfig: PeriEqp%u invalid, not exist!\n", byPeriEqpId );
#endif
		return FALSE;
	}
	else
	{
		//�����������������ⲿ��AppͶ����Ϣ��������
		if (FALSE == g_cMSSsnApp.JudgeSndMsgPass())
		{
			return TRUE;
		}

		::OspPost( MAKEIID( AID_MCU_PERIEQPSSN, byPeriEqpId), wEvent, pbyMsg, wLen );
		return TRUE;
	}
}

/*====================================================================
    ������      ��BroadcastToAllPeriEqpSsn
    ����        ������Ϣ�������Ѿ��Ǽ����ӵ������Ӧ�ĻỰʵ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wEvent, �¼���
				  u8 * const pbyMsg, ���͵���Ϣָ�룬ȱʡΪNULL
				  u16 wLen, ��Ϣ���ȣ�ȱʡΪ0
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/19    1.0         Ѧ����         ����
====================================================================*/
void CEqpConfig::BroadcastToAllPeriEqpSsn( u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	//�����������������ⲿ��AppͶ����Ϣ��������
	if (FALSE == g_cMSSsnApp.JudgeSndMsgPass())
	{
		return;
	}

	::OspPost(MAKEIID( AID_MCU_PERIEQPSSN, CInstance::EACH ), wEvent, pbyMsg, wLen);

	return;
}


//END OF FILE
