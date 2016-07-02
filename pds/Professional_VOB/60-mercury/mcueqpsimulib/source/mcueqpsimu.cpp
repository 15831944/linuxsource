#include "mcueqpsim.h"
#include "evmcueqp.h"
#include "eqpcfg.h"
#include "evmcu.h"
#include "codecwrapperdef_hd.h"
#include "kdvdef.h"

CMcuEqpSimApp g_cMcuEqpSimSsnApp;

u32 g_dwLocalIpAddr;

extern "C" void McuEqpSimuInit(u32 dwLocalIpAddr)
{
    g_cMcuEqpSimSsnApp.CreateApp("McuSimu", AID_MCU_PERIEQPSSN, 80);
	//::OspPost(MAKEIID(AID_MCU_PERIEQPSSN, CInstance::EACH), OSP_POWERON);

    for (u16 wInstNo=1; wInstNo<=MAXNUM_MCU_PERIEQP; wInstNo++)
    {
        ::OspPost(MAKEIID(AID_MCU_PERIEQPSSN, wInstNo),
                  OSP_POWERON, (u8*)&dwLocalIpAddr, sizeof(u32));
    }

    //g_dwLocalIpAddr = dwLocalIpAddr;

    //OspPrintf(TRUE, FALSE, "g_dwLocalIpAddr = 0x%x\n", g_dwLocalIpAddr);
}

CMcuInst::CMcuInst()
{
    //m_dwMcuIpAddr = g_dwLocalIpAddr;
    //OspPrintf(TRUE, FALSE, "77m_dwMcuIpAddr = 0x%x\n", ntohl(m_dwMcuIpAddr));
}

/*====================================================================
    ������      ClearInst
    ����        ���ʵ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/09    3.0         zhangsh         ����
====================================================================*/
void CMcuInst::ClearInst()
{
	m_dwEqpIpAddr = 0; //������յ�ַ
	m_wEqpStartPort = 0; //������ʼ�˿�
	m_byEqpId = 0;		//�豸ID��
	m_byEqpType = 0;    //��������
	m_dwEqpNode = 0;
	m_dwEqpIId = 0;
	memset ( (u8 *)m_cServMsg, 0, sizeof(m_cServMsg) );
}

/*====================================================================
    ������      InstanceEntry
    ����        ��Ϣʵ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/09    3.0         zhangsh         ����
====================================================================*/
void CMcuInst::InstanceEntry( CMessage* const pMsg )
{
    //OspPrintf(TRUE, TRUE, "[CMcuInst]Recv msg %u(%s).\n", pMsg->event, ::OspEventDesc(pMsg->event));

	switch ( pMsg->event )
	{
	case OSP_POWERON:
        m_dwMcuIpAddr = *(u32*)pMsg->content;
        //log(LOGLVL_EXCEPTION, "1m_dwMcuIpAddr = 0x%x\n", m_dwMcuIpAddr);
        //::OspPrintf(TRUE, TRUE, "2m_dwMcuIpAddr = 0x%x\n", ntohl(m_dwMcuIpAddr));
		if(!Init())
		{
			exit(0);
		}
		NEXTSTATE(IDLE);
		break;

	case HDU_MCU_REG_REQ:               //HDU�Ǽ�ע��    //4.6
	case TVWALL_MCU_REG_REQ:			//����ǽ�Ǽ�����
	case REC_MCU_REG_REQ:				//¼����Ǽ�����
	case MIXER_MCU_REG_REQ:				//�������Ǽ�����
	case BAS_MCU_REG_REQ:				//�������Ǽ�����
	case VMP_MCU_REGISTER_REQ:          //����ϳ����Ǽ�����
	case PRS_MCU_REGISTER_REQ:			//PRS��MCUע������
		MsgRegProc(pMsg);
		break;

	case OSP_DISCONNECT:
		Disconnect();
		break;
	//MCU���������Ϣ
    //4.6HDU 
	case MCU_HDU_START_PLAY_REQ:        // HDU��ʼ����֪ͨ
	case MCU_HDU_STOP_PLAY_REQ:         // HDUֹͣ����֪ͨ
	case MCU_HDU_GETSTATUS_REQ:         // ȡHDU״̬

	case MCU_TVWALL_START_PLAY_REQ:	    //���迪ʼ����֪ͨ
	case MCU_TVWALL_STOP_PLAY_REQ:		//����֪ͨ����֪ͨ
		
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
		
	case MCU_MIXER_STARTMIX_REQ:        //MCU��ʼ��������
	case MCU_MIXER_STOPMIX_REQ:         //MCUֹͣ��������
	case MCU_MIXER_ADDMEMBER_REQ:       //�����Ա����
	case MCU_MIXER_REMOVEMEMBER_REQ:    //ɾ����Ա����
	case MCU_MIXER_FORCEACTIVE_REQ:     //ǿ�Ƴ�Ա����
	case MCU_MIXER_CANCELFORCEACTIVE_REQ:    //ȡ����Աǿ�ƻ���
	case MCU_MIXER_SETCHNNLVOL_CMD:     //����ĳͨ��������Ϣ
	case MCU_MIXER_SETMIXDEPTH_REQ:     //���û������������Ϣ
	case MCU_MIXER_SEND_NOTIFY:         //MCU�����������Ƿ�Ҫ��������֪ͨ
	case MCU_MIXER_VACKEEPTIME_CMD:     //��������������ʱ����ʱ����Ϣ
	case MCU_MIXER_SETMEMBER_CMD:       //���û�����Ա
		
	case MCU_BAS_STARTADAPT_REQ:		//������������
	case MCU_BAS_STOPADAPT_REQ:			//ֹͣ��������
	case MCU_BAS_SETADAPTPARAM_CMD:		//�ı������������
		
	case MCU_VMP_STARTVIDMIX_REQ:       //MCU����VMP��ʼ����
	case MCU_VMP_STOPVIDMIX_REQ:        //MCU����VMPֹͣ����
	case MCU_VMP_CHANGEVIDMIXPARAM_CMD: //MCU����VMP�ı临�ϲ���
	case MCU_VMP_GETVIDMIXPARAM_REQ:    //MCU����VMP���͸��ϲ���
	case MCU_VMP_SETCHANNELBITRATE_REQ: //�ı仭��ϳɵ�����
		
	case MCU_PRS_SETSRC_REQ:			//MCU����PRS��ʼ������ϢԴ
	case MCU_PRS_ADDRESENDCH_REQ:		//MCU����PRS���һ���ش�ͨ��
	case MCU_PRS_REMOVERESENDCH_REQ:	//MCU����PRS�Ƴ�һ���ش�ͨ��
	case MCU_PRS_REMOVEALL_REQ:			//MCU����PRS�Ƴ����е��ش�ͨ��
/*
        if (pMsg->event == MCU_PRS_SETSRC_REQ)
        {
            CServMsg cServMsg( pMsg->content, pMsg->length );
            TPrsParam tParam;
            cServMsg.GetMsgBody( (u8 *)&tParam, sizeof(tParam) );
            OspPrintf( TRUE, FALSE,"[MCU PRS]:RTP---0x%x : %x\n",
                tParam.m_tLocalAddr.GetIpAddr(),
                tParam.m_tLocalAddr.GetPort());
            OspPrintf( TRUE, FALSE,"[MCU PRS]:RTCP---0x%x : %x\n",
                tParam.m_tRemoteAddr.GetIpAddr(),
                tParam.m_tRemoteAddr.GetPort());
        }
*/
		ProcMcuToEqpMsg( pMsg );
		break;
	//�����Ӧ��Ϣ
    //4.6 
	case HDU_MCU_START_PLAY_ACK:        // ��ʼ����Ӧ��
    case HDU_MCU_START_PLAY_NACK:       // ��ʼ����Ӧ��
	case HDU_MCU_STOP_PLAY_ACK:         // ֹͣ����Ӧ��
	case HDU_MCU_STOP_PLAY_NACK:        // ֹͣ����Ӧ��

	case HDU_MCU_GETSTATUS_ACK:         // ȡ״̬Ӧ��
	case HDU_MCU_GETSTATUS_NACK:        // ȡ״̬Ӧ��

	case REC_MCU_STARTREC_ACK:			//��ʼ¼��Ӧ��
	case REC_MCU_STARTREC_NACK:			//��ʼ¼��Ӧ��
	case REC_MCU_PAUSEREC_ACK:			//��ͣ¼��Ӧ��
	case REC_MCU_PAUSEREC_NACK:			//��ͣ¼��Ӧ��
	case REC_MCU_RESUMEREC_ACK:			//�ָ�¼��Ӧ��
	case REC_MCU_RESUMEREC_NACK:		//�ָ�¼��Ӧ��
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
	case REC_MCU_RECSTATUS_NOTIF:		//¼���״̬֪ͨ
	case REC_MCU_RECORDCHNSTATUS_NOTIF:	//¼���¼���ŵ�״̬֪ͨ
	case REC_MCU_PLAYCHNSTATUS_NOTIF:	//¼��������ŵ�״̬֪ͨ
	case REC_MCU_RECORDPROG_NOTIF:		//��ǰ¼�����֪ͨ
	case REC_MCU_PLAYPROG_NOTIF:		//��ǰ�������֪ͨ
	case REC_MCU_EXCPT_NOTIF:			//¼����쳣֪ͨ��Ϣ
	case REC_MCU_LISTALLRECORD_ACK:     //¼�����Ӧ��(���)��Ϣ
	case REC_MCU_LISTALLRECORD_NACK:    //¼����ܾ�¼���б�����
	case REC_MCU_LISTALLRECORD_NOTIF:	//¼�����¼֪ͨ
    case REC_MCU_DELETERECORD_ACK:      //ɾ����¼Ӧ����Ϣ
	case REC_MCU_DELETERECORD_NACK:      //ɾ����¼�ܾ���Ϣ
	case REC_MCU_PUBLISHREC_ACK:		//����¼��Ӧ��
	case REC_MCU_PUBLISHREC_NACK:		//����¼��ܾ�
	case REC_MCU_CANCELPUBLISHREC_ACK:  //ȡ������¼��Ӧ��
	case REC_MCU_CANCELPUBLISHREC_NACK: //ȡ������¼��ܾ�
	case MIXER_MCU_ACTIVEMMBCHANGE_NOTIF:  //����������Ա�ı�֪ͨ
	case MIXER_MCU_CHNNLVOL_NOTIF:      //ĳͨ������֪ͨ��Ϣ
	case MIXER_MCU_SETMIXDEPTH_ACK:     //���û������Ӧ����Ϣ
	case MIXER_MCU_SETMIXDEPTH_NACK:    //���û�����Ⱦܾ���Ϣ
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
		
	case BAS_MCU_STARTADAPT_ACK:		//��������Ӧ��
	case BAS_MCU_STARTADAPT_NACK:		//��������ܾ�
	case BAS_MCU_STOPADAPT_ACK:         //ֹͣ����Ӧ��
	case BAS_MCU_STOPADAPT_NACK:        //ֹͣ����ܾ�
		
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
		
		
	case PRS_MCU_SETSRC_ACK:			//PRS��MCU������ϢԴȷ��
	case PRS_MCU_SETSRC_NACK:			//PRS��MCU������ϢԴ�ܾ�
	case PRS_MCU_ADDRESENDCH_ACK:		//PRS��MCUֹͣ����ȷ��
	case PRS_MCU_ADDRESENDCH_NACK:		//PRS��MCUֹͣ�����ܾ�
	case PRS_MCU_REMOVERESENDCH_ACK:	//PRS��MCU�ı临�ϲ���ȷ��
	case PRS_MCU_REMOVERESENDCH_NACK:	//PRS��MCU�ı临�ϲ����ܾ�
	case PRS_MCU_REMOVEALL_ACK:			//PRS��MCU�ı临�ϲ���ȷ��
	case PRS_MCU_REMOVEALL_NACK:		//PRS��MCU�ı临�ϲ����ܾ�
	case PRS_MCU_ADDRESENDCH_NOTIF:		//PRS��MCU��ֹͣ���
	case PRS_MCU_REMOVERESENDCH_NOTIF:	//PRS��MCU��״̬�ı���
	case PRS_MCU_REMOVEALL_NOTIF:		//PRS��MCU��״̬�ı���
	case TVWALL_MCU_START_PLAY_ACK:
	case TVWALL_MCU_START_PLAY_NACK:
	case TVWALL_MCU_STOP_PLAY_ACK:
	case TVWALL_MCU_STOP_PLAY_NACK:
		ProcEqpMcuNotif( pMsg );
		break;
	case MIXER_MCU_GRPSTATUS_NOTIF:     //������״̬֪ͨ
	case MIXER_MCU_MIXERSTATUS_NOTIF:   //������״̬֪ͨ
		ProcMixNotif( pMsg );
		break;
	case BAS_MCU_BASSTATUS_NOTIF:		//������״̬֪ͨ
	case BAS_MCU_CHNNLSTATUS_NOTIF:     //����ͨ��״̬֪ͨ
		ProcBasNotif( pMsg );
		break;
	case PRS_MCU_PRSSTATUS_NOTIF:		//VMP��MCU��״̬�ϱ�
	case PRS_MCU_SETSRC_NOTIF:			//PRS��MCU�ı�����ϢԴ���
		ProcPrsNotif( pMsg );
		break;
	case TVWALL_MCU_STATUS_NOTIF:	    //����ǽ״̬֪ͨ
		ProcTvWallNotif( pMsg );
		break;
	case VMP_MCU_VMPSTATUS_NOTIF:       //����ϳ���״̬֪ͨ
		ProcVmpNotif( pMsg );
		break;
    case HDU_MCU_CHNNLSTATUS_NOTIF:
        ProcHduNotif( pMsg );
		break;
	
	//�����������Ͳ�ѯ�����
	case 10:
		{
			SetSyncAck(&m_tEqpStatus[m_byEqpId] ,sizeof(TPeriEqpStatus) );
		}
		break;
	//���������������ó�ʼ����Ϣ
	case 11:
		SetDefaultData(pMsg);
		break;
	case 12:
		ClearAckBuf();
		break;
	default:
		break;
	}
}

/*====================================================================
    ������      Init
    ����        ��ʼ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/09    3.0         zhangsh         ����
====================================================================*/
BOOL CMcuInst::Init()
{
	ClearInst();

	//nodeid = ::OspCreateTcpNode(INADDR_ANY, 2020);
    //::OspPrintf(TRUE, TRUE, "CMcuInst::Init() nodeid = %d\n", nodeid);
	memset( m_tEqpStatus ,0 ,sizeof(m_tEqpStatus) );

	return TRUE;
}

/*====================================================================
	����  : MsgRegProc
	����  : ע����Ϣ������
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
void CMcuInst::MsgRegProc(CMessage* const pMsg)
{
	CServMsg cServMsg(pMsg->content, pMsg->length);
	BOOL bCheck = TRUE;

	TPeriEqpRegReq* ptReg = (TPeriEqpRegReq *)cServMsg.GetMsgBody();

	if ( m_dwEqpIpAddr != 0 && m_dwEqpIpAddr != ptReg->GetPeriEqpIpAddr() )
	{
			bCheck = FALSE;
	}
	else
		m_dwEqpIpAddr = ptReg->GetPeriEqpIpAddr();

	if (m_wEqpStartPort != 0 && m_wEqpStartPort != ptReg->GetStartPort() )
	{
			bCheck = FALSE;
	}
	else
		m_wEqpStartPort = ptReg->GetStartPort();

	if ( m_byEqpId != 0 && m_byEqpId != ptReg->GetEqpId())
	{
			bCheck = FALSE;
	}
	else
		m_byEqpId = ptReg->GetEqpId();

	
	memset( m_tEqpStatus,0,sizeof(m_tEqpStatus) );
	
	
	m_dwEqpIId       = pMsg->srcid;
	m_dwEqpNode        = pMsg->srcnode;

	::OspNodeDiscCBReg( m_dwEqpNode, GetAppID(), 1 );//ע�������Ϣ

	TPeriEqpRegAck   tRegAck;
    tRegAck.SetMcuIpAddr(g_dwLocalIpAddr);
	//tRegAck.SetMcuIpAddr( inet_addr("127.0.0.1") );
	tRegAck.SetMcuStartPort( 13000 );
	
	cServMsg.SetMsgBody( (u8*)&tRegAck ,sizeof( tRegAck ));
	TPrsTimeSpan tPrs;
	tPrs.m_wFirstTimeSpan = 10;
	tPrs.m_wRejectTimeSpan = 40;
	tPrs.m_wSecondTimeSpan = 20;
	tPrs.m_wThirdTimeSpan = 30;
	cServMsg.CatMsgBody( (u8*)&tPrs, sizeof(TPrsTimeSpan) );
	if ( bCheck )
	{
		OspPrintf( TRUE, FALSE, "Eqp:%s-%u Type-%u Registered\n", ptReg->GetEqpAlias(), ptReg->GetEqpId(), ptReg->GetEqpType() );
		m_tEqpStatus[m_byEqpId].m_byOnline = TRUE;
		post( m_dwEqpIId, pMsg->event+1, cServMsg.GetServMsg(),cServMsg.GetServMsgLen(),m_dwEqpNode);
		NextState( (u32)NORMAL );
	}
	else
	{
		post( m_dwEqpIId, pMsg->event+2, cServMsg.GetServMsg(),cServMsg.GetServMsgLen(),m_dwEqpNode);
	}
}

/*====================================================================
    ������      MsgDisconnectProc
    ����        ������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/09    3.0         zhangsh         ����
====================================================================*/
void CMcuInst::MsgDisconnectProc()
{
	memset( &m_tEqpStatus[m_byEqpId] ,0 ,sizeof(TPeriEqpStatus) );
	ClearInst();
	printf("Disconnectd!\n");
	NEXTSTATE( IDLE );
}

/*====================================================================
    ������      Disconnect
    ����        �˿�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/09    3.0         zhangsh         ����
====================================================================*/
void CMcuInst::Disconnect()
{
	ClearInst();
	::OspDisconnectTcpNode( m_dwEqpNode);
	memset( &m_tEqpStatus[m_byEqpId] ,0,sizeof(TPeriEqpStatus) );
	NEXTSTATE( IDLE );
}

/*====================================================================
    ������      SetDefaultData
    ����        ��������Ļ������ݣ�������֤ע��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/09    3.0         zhangsh         ����
====================================================================*/
void CMcuInst::SetDefaultData(CMessage* const pMsg)
{
	TEqpCfg tEqpCfg = *(TEqpCfg*)pMsg->content;
	m_dwEqpIpAddr = tEqpCfg.dwLocalIP; //������յ�ַ
	m_wEqpStartPort = tEqpCfg.wRcvStartPort; //������ʼ�˿�
	m_byEqpId = tEqpCfg.byEqpId;		//�豸ID��
	m_byEqpType = tEqpCfg.byEqpType;    //��������
}

/*====================================================================
    ������      ProcMcuToEqpMsg
    ����        ����Ϣ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/09    3.0         zhangsh         ����
====================================================================*/
BOOL CMcuInst::ProcMcuToEqpMsg( CMessage * const pMsg )
{
	if( OspIsValidTcpNode( m_dwEqpNode ) )
	{
        CServMsg cServMsg( pMsg->content, pMsg->length );                
        post( m_dwEqpIId, pMsg->event, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), m_dwEqpNode );
		//post( m_dwEqpIId, pMsg->event, pMsg, pMsg->length, m_dwEqpNode );
		return( TRUE );
	}
	else
	{
		log( LOGLVL_IMPORTANT, "CMcuInst: PeriEqp%u is offline now: Node=%u, CurState=%u, InstId=%u\n", 
			m_byEqpId, m_dwEqpNode, CurState(), GetInsID() );
		return( FALSE );
	}
}

/*====================================================================
    ������      ProcEqpMcuNotif
    ����        ���践�ص���Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/09    3.0         zhangsh         ����
====================================================================*/
void CMcuInst::ProcEqpMcuNotif( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	cServMsg.SetEventId( pcMsg->event );
	switch( CurState() )
	{
	case NORMAL:
		AddToAckBuf( cServMsg );
 		break;

	default:
		break;
	}
}

/*====================================================================
    ������      AddToAckBuf
    ����        ҵ����Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CServMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/09    3.0         zhangsh         ����
====================================================================*/
void CMcuInst::AddToAckBuf( CServMsg cServMsg )
{
	for ( u8 byLoop = 0; byLoop < 10 ; byLoop ++ )
	{
		if ( cServMsg.GetEventId() == 0 )
		{
			memcpy( &m_cServMsg[byLoop], &cServMsg, sizeof(CServMsg) );
			break;
		}
	}
}

/*====================================================================
    ������      ClearAckBuf
    ����        ���ҵ����Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CServMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/09    3.0         zhangsh         ����
====================================================================*/
void CMcuInst::ClearAckBuf( )
{
	memset( (u8*)m_cServMsg, 0 , sizeof(m_cServMsg) );
}

/*====================================================================
    ������      ProcMixNotif
    ����        ������״̬�ı�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CServMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/09    3.0         zhangsh         ����
====================================================================*/
void CMcuInst::ProcMixNotif( CMessage *const pMsg )
{
	CServMsg cServMsg( pMsg->content, pMsg->length );
	TMixerGrpStatus tMixerGrpStatus;
	TPeriEqpStatus tMixerStatus;
	switch( pMsg->event) {
	case MIXER_MCU_GRPSTATUS_NOTIF:
		tMixerGrpStatus = *(TMixerGrpStatus*)cServMsg.GetMsgBody();
		m_tEqpStatus[m_byEqpId].m_tStatus.tMixer.m_atGrpStatus[tMixerGrpStatus.m_byGrpId] = tMixerGrpStatus;
		break;
	case MIXER_MCU_MIXERSTATUS_NOTIF:
		tMixerStatus = *(TPeriEqpStatus*)cServMsg.GetMsgBody();
		m_tEqpStatus[m_byEqpId] = tMixerStatus;
		break;
	default:
		break;
	}
}

void CMcuInst::ProcBasNotif(CMessage* const pMsg )
{
	CServMsg cServMsg( pMsg->content, pMsg->length );
	char byChnIdx;
	switch(pMsg->event) {
	case BAS_MCU_BASSTATUS_NOTIF:
		memcpy( &m_tEqpStatus[m_byEqpId] ,cServMsg.GetMsgBody(),sizeof(TPeriEqpStatus) );
		break;
	case BAS_MCU_CHNNLSTATUS_NOTIF:
		byChnIdx = cServMsg.GetChnIndex();
		memcpy( &m_tEqpStatus[m_byEqpId].m_tStatus.tBas.tChnnl[byChnIdx],
			(char*)cServMsg.GetMsgBody() +1,
			sizeof(TBasChnStatus) );
		break;
	default:
		break;
	}
}

void CMcuInst::ProcPrsNotif(CMessage* const pMsg )
{
	CServMsg cServMsg( pMsg->content, pMsg->length );
	switch(pMsg->event) {
	case PRS_MCU_PRSSTATUS_NOTIF:
	case PRS_MCU_SETSRC_NOTIF:
		memcpy( &m_tEqpStatus[m_byEqpId] ,cServMsg.GetMsgBody(),sizeof(TPeriEqpStatus) );
		break;
	default:
		break;
	}
}

void CMcuInst::ProcTvWallNotif(CMessage* const pMsg )
{
	CServMsg cServMsg( pMsg->content, pMsg->length );
	m_tEqpStatus[m_byEqpId] = *(TPeriEqpStatus*)cServMsg.GetMsgBody();
}

void CMcuInst::ProcVmpNotif(CMessage* const pMsg )
{
	CServMsg cServMsg( pMsg->content, pMsg->length );
	m_tEqpStatus[m_byEqpId] = *(TPeriEqpStatus*)cServMsg.GetMsgBody();
}

void CMcuInst::ProcHduNotif(CMessage* const pMsg )
{
	CServMsg cServMsg( pMsg->content, pMsg->length );
	m_tEqpStatus[m_byEqpId] = *(TPeriEqpStatus*)cServMsg.GetMsgBody();
}
