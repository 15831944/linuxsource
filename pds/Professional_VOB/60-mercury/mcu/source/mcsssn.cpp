/*****************************************************************************
   ģ����      : mcu
   �ļ���      : mcsssn.cpp
   ����ļ�    : mcsssn.h
   �ļ�ʵ�ֹ���: MCU��������̨�Ự��
   ����        : ����
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2002/07/25  0.9         ����        ����
   2003/10/27  3.0         ������      �ش��޸�
   2005/02/19  3.6         ����      �����޸ġ���3.5�汾�ϲ�
******************************************************************************/

#include "evmcu.h"
#include "evmcumcs.h"
#include "evmcuvcs.h"
#include "evmcutest.h"
#include "mcuvc.h"
#include "mcsssn.h"
#include "commonlib.h"
//#include "mcuerrcode.h"

#ifndef _UMCONST_H_
#include "umconst.h"
#endif

#include "nplusmanager.h"
#include "mcucfg.h"
#include "vcsssn.h"

#ifdef _VXWORKS_
#include "brddrvLib.h"
#endif

// MPC2 ֧��
#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
        #include "brdwrapperdef.h"
        #include "nipwrapper.h"
        //#include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
#endif

CMcsSsnApp	g_cMcsSsnApp;	//�������̨�ỰӦ��ʵ��
#ifdef _LINUX_
	#ifdef _UTF8
	CUsrManage  g_cUsrManage((u8*)DIR_DATA, NULL, ENCODE_UTF8, TRUE);   //�û��������
	#else
	CUsrManage  g_cUsrManage((u8*)DIR_DATA);   //�û��������
	#endif
#else
	#ifdef _UTF8
	CUsrManage  g_cUsrManage((u8*)DIR_DATA, NULL, ENCODE_UTF8, TRUE);   //�û��������
	#else
	CUsrManage  g_cUsrManage;   //�û��������
	#endif
#endif
CAddrBook   *g_pcAddrBook = NULL;  //��ַ���������

u8 CMcsSsn::m_abyUsrGrp[MAXNUM_MCU_MC + MAXNUM_MCU_VC];
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMcsSsnInst::CMcsSsnInst()
{
	m_dwMcsNode = INVALID_NODE;		//��ʼû�л������̨����
	m_dwMcsIp = 0;
	cCurConfId.SetNull();
}

CMcsSsnInst::~CMcsSsnInst()
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
    02/08/18    1.0         LI Yi         ����
====================================================================*/
void CMcsSsnInst::InstanceDump( u32 param )
{
	StaticLog("%6u %4u 0x%x\n", GetInsID(), m_dwMcsNode,param );
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
    02/07/25    1.0         LI Yi         ����
====================================================================*/
#define MCS_EV_LOG(level)	MCU_EV_LOG(level, MID_MCU_MCS, "Mcs")

void CMcsSsnInst::InstanceEntry( CMessage * const pcMsg )
{
	// ��������Ϊ��Ч״̬��ʵ���������κ���Ϣ����
	if (CurState() == STATE_INVALID)
	{
		return;
	}

	if (NULL == pcMsg)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "CMcsSsnInst: The received msg's pointer in the msg entry is NULL!");
		return;
	}

	switch (pcMsg->event)
	{
	case OSP_POWERON:
		{
			MCS_EV_LOG(LOG_LVL_KEYSTATUS);
			// ����MCS����32��ʵ����ֻ��ǰ16��ʵ����Ч��֧��16��MCSͬʱ����
			if (GetInsID() > MAXNUM_MCU_MC)
			{
				NEXTSTATE( STATE_INVALID );
			}
		}
    	break;
	case OSP_DISCONNECT:				//����̨����֪ͨ
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcsDisconnect(pcMsg);
		break;
	case OSP_OVERFLOW:
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcReoOverFlow(pcMsg);		//����ʵ������
		break;

	/*----------------------����MCU�õ��Ļ����Ϣ----------------------------------*/
	
    case MCS_MCU_GETMCUSTATUS_CMD:          //��ѯMCU״̬����
	case MCS_MCU_GETMCUSTATUS_REQ:	              
	case MCS_MCU_LISTALLCONF_REQ:	        //�г���MCU�����л�����Ϣ
	case MCS_MCU_STOPSWITCHMC_REQ:	        //�������ֹ̨ͣ��������
	case MCS_MCU_STOP_SWITCH_TW_REQ:	    //�������̨������ֹͣ��������
	case MCS_MCU_GETMCUPERIEQPSTATUS_REQ:	//��ѯMCU����״̬
	case MCS_MCU_GETRECSTATUS_REQ:	        //��ѯ¼���״̬����
	case MCS_MCU_GETMIXERSTATUS_REQ:	    //��ѯ������״̬����
	case MCS_MCU_GETPERIDCSSTATUS_REQ:		//��ѯ���ֻ���״̬����

	//¼���ļ�����
	case MCS_MCU_LISTALLRECORD_REQ:         //����б�����
	case MCS_MCU_DELETERECORD_REQ:          //�������ɾ���ļ�
	case MCS_MCU_RENAMERECORD_REQ:          //�����������ļ���
	case MCS_MCU_PUBLISHREC_REQ:			//����¼������
	case MCS_MCU_CANCELPUBLISHREC_REQ:      //ȡ������¼������

    case MCS_MCU_CREATECONF_BYTEMPLATE_REQ:     //�������̨��MCU�ϰ�����ģ�崴��һ����ʱ����
    case MCS_MCU_CREATESCHCONF_BYTEMPLATE_REQ:  //����ģ�崴��ԤԼ����
    case MCS_MCU_CREATETEMPLATE_REQ:            //����ģ��
    case MCS_MCU_MODIFYTEMPLATE_REQ:            //�޸�ģ��
    case MCS_MCU_DELTEMPLATE_REQ:               //ɾ��ģ��

	case MCS_MCU_SAVECONFTOTEMPLATE_REQ:	//�������̨���󽫵�ǰ���鱣��Ϊ����ģ��(Ԥ��), zgc, 2007/04/20

    case MCS_MCU_CREATECONF_REQ:                //�������ֱ̨�Ӵ�������

	//�����ȱ��ݵ�Ԫ���Խӿ�
	case EV_TEST_TEMPLATEINFO_GET_REQ:
	case EV_TEST_CONFINFO_GET_REQ:
	case EV_TEST_CONFMTLIST_GET_REQ:
	case EV_TEST_ADDRBOOK_GET_REQ:
	case EV_TEST_MCUCONFIG_GET_REQ:
	case MCS_MCU_GETCRIMAC_REQ: 
		ProcMcsMcuDaemonConfMsg(pcMsg);
		break;   
		
	//��������
	case MCS_MCU_CONNECT_REQ:		  //����̨֪ͨMCU����׼��
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcsMcuConnectReq(pcMsg);
		break;
	case MCS_MCU_CURRTIME_REQ :		  //����̨��ѯmcu��ǰʱ��
		ProcMcsInquiryMcuTime(pcMsg);
		break;
        
        //��ȡmcu����״̬
    case MCS_MCU_GETMSSTATUS_REQ:        
    case MCU_MSSTATE_EXCHANGE_NTF:
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
        ProcMcsMcuGetMsStatusReq(pcMsg);
        break;

        //����̨�޸�mcuϵͳʱ��
    case MCS_MCU_CHANGESYSTIME_REQ:
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
        ProcMcsMcuChgSysTime(pcMsg);
        break;
    
	//�������
	case MCS_MCU_RELEASECONF_REQ:     //�������̨����MCU����һ������
	case MCS_MCU_CHANGECONFLOCKMODE_REQ: //�������̨����MCU��������
	case MCS_MCU_CHANGECONFPWD_REQ:   //�������̨����MCU���Ļ�������
	case MCS_MCU_GETLOCKINFO_REQ:     //�õ����������Ϣ
	case MCS_MCU_ENTERCONFPWD_ACK:    //�������̨��ӦMCU����������   
	case MCS_MCU_ENTERCONFPWD_NACK:   //�������̨�ܾ�MCU���������� 
	case MCS_MCU_SAVECONF_REQ:        //�������̨����MCU�������
	case MCS_MCU_MODIFYCONF_REQ:	  //����޸�ԤԼ����
	case MCS_MCU_DELAYCONF_REQ:       //�������̨����MCU�ӳ�����
	case MCS_MCU_CHANGEVACHOLDTIME_REQ://�������̨����MCU�ı����������л�ʱ��

	//�������
	case MCS_MCU_SPECCHAIRMAN_REQ:    //�������ָ̨��һ̨�ն�Ϊ��ϯ
	case MCS_MCU_CANCELCHAIRMAN_REQ:  //�������̨ȡ����ǰ������ϯ
	case MCS_MCU_SETCHAIRMODE_CMD:    //�������̨���û������ϯ��ʽ
	case MCS_MCU_MTSEESPEAKER_CMD:    //ǿ�ƹ㲥������
	case MCS_MCU_SPECSPEAKER_REQ:     //�������ָ̨��һ̨�ն˷���
	case MCS_MCU_CANCELSPEAKER_REQ:   //�������̨ȡ�����鷢����
    case MCS_MCU_SPECOUTVIEW_REQ:     //�������ָ̨���ش�ͨ��
	case MCS_MCU_ADDMT_REQ:           //�������̨�����ն����	
	case MCS_MCU_ADDMTEX_REQ:         //�������̨�����ն��������������Ϣ������������ͨ����ʹ�õ�������
	case MCS_MCU_DELMT_REQ:           //�������̨�����ն����
	case MCS_MCU_STARTSWITCHMC_REQ:   //���ѡ���ն�
	case MCS_MCU_SENDRUNMSG_CMD:      //�������̨����MCU���ն˷��Ͷ���Ϣ���ն˺�Ϊ0��ʾ���������ն�
	case MCS_MCU_GETMTLIST_REQ:       //�������̨����MCU�ĵõ��ն��б�����
	case MCS_MCU_REFRESHMCU_CMD:	  //�������̨����MCU��ˢ������MCU������
	case MCS_MCU_GETCONFINFO_REQ:     //�������̨��MCU��ѯ������Ϣ
    case MCS_MCU_GETMAUSTATUS_REQ:    //�������̨��MCU��ѯMAU��Ϣ
	case MCS_MCU_STARTSWITCHVMPMT_REQ: // xliang [1/17/2009] ��ϯѡ��VMP
	//�������---��ѯ����
	case MCS_MCU_STARTPOLL_CMD:       //�������̨����û��鿪ʼ��ѯ�㲥
	case MCS_MCU_STOPPOLL_CMD:        //�������̨����û���ֹͣ��ѯ�㲥  
	case MCS_MCU_PAUSEPOLL_CMD:       //�������̨����û�����ͣ��ѯ�㲥   
	case MCS_MCU_RESUMEPOLL_CMD:      //�������̨����û��������ѯ�㲥
	case MCS_MCU_GETPOLLPARAM_REQ:    //�������̨��MCU��ѯ������ѯ����
    case MCS_MCU_CHANGEPOLLPARAM_CMD: //�������̨����û��������ѯ�б�
	case MCS_MCU_SPECPOLLPOS_REQ:	  //�������ָ̨����ѯλ��

    //����ǽ��ѯ����
    case MCS_MCU_STARTTWPOLL_CMD:
    case MCS_MCU_STOPTWPOLL_CMD:
    case MCS_MCU_PAUSETWPOLL_CMD:
    case MCS_MCU_RESUMETWPOLL_CMD:
    case MCS_MCU_GETTWPOLLPARAM_REQ:

	//hdu��ѯ����
    case MCS_MCU_STARTHDUPOLL_CMD:
    case MCS_MCU_STOPHDUPOLL_CMD:
    case MCS_MCU_PAUSEHDUPOLL_CMD:
    case MCS_MCU_CHANGEHDUPOLLPARAM_CMD:	
    case MCS_MCU_RESUMEHDUPOLL_CMD:
    case MCS_MCU_GETHDUPOLLPARAM_REQ:

   //�������---�����������Ʒ���
	case MCS_MCU_STARTVAC_REQ:        //�������̨����MCU��ʼ�����������Ʒ���		
	case MCS_MCU_STOPVAC_REQ:         //�������̨����MCUֹͣ�����������Ʒ���
	
	//�������---��������
	case MCS_MCU_STARTDISCUSS_REQ:    //��ʼ������������
	case MCS_MCU_STOPDISCUSS_REQ:     //����������������
	case MCS_MCU_ADDMIXMEMBER_CMD:    //���ӻ�����Ա
	case MCS_MCU_REMOVEMIXMEMBER_CMD: //�Ƴ�������Ա
    case MCS_MCU_CHANGEMIXDELAY_REQ:
    case MCS_MCU_REPLACEMIXMEMBER_CMD:  //�滻������Ա

    //zbq[11/01/2007] �����Ż�
    //case MCS_MCU_STARTDISCUSS_CMD:    //��ʼ������������ - ���ڿ缶��ʼ���۲���
    //case MCS_MCU_STOPDISCUSS_CMD:     //���������������� - ���ڿ缶��ʼ���۲���
    //case MCS_MCU_GETDISCUSSPARAM_REQ: //��ѯ���۲�������
    case MCS_MCU_GETMIXPARAM_REQ:
        
	//�������---��Ƶ���Ͽ���
	case MCS_MCU_STARTVMP_REQ:        //��ʼ��Ƶ��������
	case MCS_MCU_STOPVMP_REQ:         //������Ƶ��������	
	case MCS_MCU_CHANGEVMPPARAM_REQ:  //�������̨����MCU�ı���Ƶ���ϲ���			
	case MCS_MCU_GETVMPPARAM_REQ:     //��ѯ��Ƶ���ϳ�Ա����
	case MCS_MCU_STARTVMPBRDST_REQ:   //�������MCU�㲥��Ƶ��������
	case MCS_MCU_STOPVMPBRDST_REQ:    //�������MCUֹͣ�㲥��Ƶ��������
        
	case MCS_MCU_VMPPRISEIZE_ACK:		//vmp��ռӦ��// xliang [1/20/2009] 
	case MCS_MCU_VMPPRISEIZE_NACK:

	case MCS_MCU_STARTVMPPOLL_CMD:		//��ʼVmp��ͨ����ѯ
	case MCS_MCU_STOPVMPPOLL_CMD:		//ֹͣVmp��ͨ����ѯ
	case MCS_MCU_PAUSEVMPPOLL_CMD:		//��ͣVmp��ͨ����ѯ
	case MCS_MCU_RESUMEVMPPOLL_CMD:		//����Vmp��ͨ����ѯ
	case MCS_MCU_GETVMPPOLLPARAM_REQ:	//��ѯVmp��ͨ����ѯ��������
		
	case MCS_MCU_START_VMPBATCHPOLL_REQ://��ʼ����ϳ�������ѯ
	case MCS_MCU_PAUSE_VMPBATCHPOLL_CMD://��ͣ����ϳ�������ѯ
	case MCS_MCU_STOP_VMPBATCHPOLL_CMD: //ֹͣ����ϳ�������ѯ	
    //�������---���ϵ���ǽ����
	case MCS_MCU_STARTVMPTW_REQ:       //��ʼ���ϵ���ǽ����
	case MCS_MCU_STOPVMPTW_REQ:        //�������ϵ���ǽ����	
	case MCS_MCU_CHANGEVMPTWPARAM_REQ: //�������̨����MCU�ı临�ϵ���ǽ����			
//	case MCS_MCU_GETVMPPARAM_REQ:      //��ѯ��Ƶ���ϳ�Ա����
//	case MCS_MCU_STARTVMPBRDST_REQ:    //�������MCU�㲥��Ƶ��������
//	case MCS_MCU_STOPVMPBRDST_REQ:     //�������MCUֹͣ�㲥��Ƶ��������

	//�������---�õ�����״̬
	case MCS_MCU_GETCONFSTATUS_REQ:   //�������̨��MCU��ѯ����״̬

    //�������---����
    case MCS_MCU_STARTROLLCALL_REQ:
    case MCS_MCU_STOPROLLCALL_REQ:
    case MCS_MCU_CHANGEROLLCALL_REQ:
	//�������ȡ���ش�ͨ���е��ն�
	case MCS_MCU_CANCELCASCADESPYMT_REQ:
        
	//�ն˿���
	case MCS_MCU_CALLMT_REQ:          //�������̨����MCU�����ն�
	case MCS_MCU_SETCALLMTMODE_REQ:   //�������̨����MCU�����ն˷�ʽ
	case MCS_MCU_DROPMT_REQ:          //�������̨�Ҷ��ն�����
	case MCS_MCU_STARTSWITCHMT_REQ:   //���ǿ��Ŀ���ն�ѡ��Դ�ն�
	case MCS_MCU_STOPSWITCHMT_REQ:    //���ȡ��Ŀ���ն�ѡ��Դ�ն�
	case MCS_MCU_GETMTSTATUS_REQ:     //�����MCU��ѯ�ն�״̬
	case MCS_MCU_GETALLMTSTATUS_REQ:  //�����MCU��ѯ�����ն�״̬
	case MCS_MCU_GETMTALIAS_REQ:      //�����MCU��ѯ�ն˱���
	case MCS_MCU_GETALLMTALIAS_REQ:	  //�����MCU��ѯ�����ն˱���
	case MCS_MCU_SETMTBITRATE_CMD:    //�������̨����MCU�����ն�����
	case MCS_MCU_SETMTVIDSRC_CMD:     //���Ҫ��MCU�����ն���ƵԴ
	case MCS_MCU_MTAUDMUTE_REQ:       //���Ҫ��MCU�����ն˾���
    case MCS_MCU_GETMTBITRATE_REQ:    //��ز�ѯ�ն�����
	case MCS_MCU_SETMTVOLUME_CMD:	  //���ʵʱ�����ն�������zgc,2006-12-26
    case MCS_MCU_GETMTEXTINFO_REQ:    //��������ն˵Ķ�����չ��Ϣ���汾��

	//�ն�����ͷ����	
	case MCS_MCU_MTCAMERA_CTRL_CMD:         //�������̨�����ն��������ͷ�˶�
	case MCS_MCU_MTCAMERA_CTRL_STOP:        //�������̨�����ն��������ͷֹͣ�˶�
	case MCS_MCU_MTCAMERA_RCENABLE_CMD:     //�������̨�����ն������ң��ʹ��	
	case MCS_MCU_MTCAMERA_SAVETOPOS_CMD:    //�������̨�����ն����������ǰλ����Ϣ����ָ��λ��	
	case MCS_MCU_MTCAMERA_MOVETOPOS_CMD:    //�������̨�����ն������������ָ��λ��

	//�ն����þ������
    case MCS_MCU_MATRIX_GETALLSCHEMES_CMD:
    case MCS_MCU_MATRIX_GETONESCHEME_CMD:
    case MCS_MCU_MATRIX_SAVESCHEME_CMD:
    case MCS_MCU_MATRIX_SETCURSCHEME_CMD:
    case MCS_MCU_MATRIX_GETCURSCHEME_CMD:
        
    //�ն����þ���
    case MCS_MCU_EXMATRIX_GETINFO_CMD:      //��ȡ�ն����þ�������
    case MCS_MCU_EXMATRIX_SETPORT_CMD:      //�������þ������Ӷ˿ں�
    case MCS_MCU_EXMATRIX_GETPORT_REQ:      //�����ȡ���þ������Ӷ˿�
    case MCS_MCU_EXMATRIX_SETPORTNAME_CMD:  //�������þ������Ӷ˿���
    case MCS_MCU_EXMATRIX_GETALLPORTNAME_CMD://�����ȡ���þ�������ж˿���

    //�ն���չ��ƵԴ
    case MCS_MCU_GETVIDEOSOURCEINFO_CMD:
    case MCS_MCU_SETVIDEOSOURCEINFO_CMD:
        
    //�ն��л���չ��ƵԴ
    case MCS_MCU_SELECTEXVIDEOSRC_CMD:

	//¼�������
	case MCS_MCU_STARTREC_REQ:              //�����MCU����¼��
	case MCS_MCU_PAUSEREC_REQ:              //�����MCU��ͣ¼��
	case MCS_MCU_RESUMEREC_REQ:             //�����MCU�ָ�¼��
	case MCS_MCU_STOPREC_REQ:               //�����MCUֹͣ¼��
	case MCS_MCU_CHANGERECMODE_REQ:			//����ı�¼��ģʽ
	case MCS_MCU_STARTPLAY_REQ:             //��ؿ�ʼ��������
	case MCS_MCU_PAUSEPLAY_REQ:             //�����ͣ��������
	case MCS_MCU_RESUMEPLAY_REQ:            //��ػָ���������
	case MCS_MCU_STOPPLAY_REQ:              //���ֹͣ��������
	case MCS_MCU_FFPLAY_REQ:                //��ؿ����������
	case MCS_MCU_FBPLAY_REQ:                //��ؿ��˷�������
	case MCS_MCU_SEEK_REQ:                  //��ص��������������
    case MCS_MCU_GETRECPROG_CMD:            //��ز�ѯ¼�����
    case MCS_MCU_GETPLAYPROG_CMD:           //��ز�ѯ�������
	case MCS_MCU_CLOSELISTRECORD_CMD:		//��عر��ļ��б����ڹҶ�vrs��¼��ʵ�壩
	case MCS_MCU_GETRECSTATUS_CMD:			//mcs����¼��״̬

	//����ǽ����
	case MCS_MCU_START_SWITCH_TW_REQ:       //�������̨����ָ��Mt��ͼ�񽻻���ָ�������ָ��������ͨ����
	case MCS_MCU_MCUMEDIASRC_REQ:
	case MCS_MCU_LOCKSMCU_REQ:
	case MCS_MCU_GETMCULOCKSTATUS_REQ: 
		
	//Hdu����   4.6.1�¼Ӱ汾  jlb
	case MCS_MCU_START_SWITCH_HDU_REQ: 
	case MCS_MCU_STOP_SWITCH_HDU_REQ:
	case MCS_MCU_CHANGEHDUVOLUME_REQ:
	case MCS_MCU_CHGHDUVMPMODE_CMD:

	//������ѯ
	case MCS_MCU_STARTHDUBATCHPOLL_REQ:
	case MCS_MCU_STOPHDUBATCHPOLL_REQ:
	case MCS_MCU_RESUMEHDUBATCHPOLL_REQ:
	case MCS_MCU_PAUSEHDUBATCHPOLL_REQ:
	case MCS_MCU_GETBATCHPOLLSTATUS_CMD:

	//����ģʽ
    case MCS_MCU_CHGSPEAKMODE_REQ:

	case MCS_MCU_GETIFRAME_CMD:
	
		//���桢�ն���Ϣ͸��
    case MCS_MCU_TRANSPARENTMSG_NOTIFY:

	// ��ػ�ȡ�ն���ƵԴ������Ϣ
	case MCS_MCU_GETMTVIDEOALIAS_REQ:

	case VCS_MCU_ADDPLANNAME_REQ:			//MCS�������Ԥ��
	case VCS_MCU_DELPLANNAME_REQ:			//MCS����ɾ��Ԥ��
	case VCS_MCU_MODIFYPLANNAME_REQ:		//MCS�����޸�Ԥ������
	case VCS_MCU_GETALLPLANDATA_REQ:		//MCS����������Ԥ��������Ϣ
	case MCS_MCU_CHANGEMTSECVIDSEND_CMD:    //[5/4/2011 zhushengze]VCS���Ʒ����˷�˫��
		MCS_EV_LOG(LOG_LVL_DETAIL);
		ProcMcsMcuMsg(pcMsg);
		break;
	/*---------------------����MCU������ص���Ϣ------------------------------------------*/

    //3.2	�������
    //3.2.1	��������б�	  
	case MCU_MCS_CONFINFO_NOTIF:    //����������Ϣ֪ͨ

    //3.2.2	��������	     
	case MCU_MCS_CONFREGGKSTATUS_NOTIF: //������GK��ע��״̬֪ͨ
    case MCU_MCS_TEMSCHCONFINFO_NOTIF:
    case MCU_MCS_MODIFYTEMPLATE_ACK:
    case MCU_MCS_MODIFYTEMPLATE_NACK:
    case MCU_MCS_DELTEMPLATE_ACK:
    case MCU_MCS_DELTEMPLATE_NACK:
    case MCU_MCS_DELTEMPLATE_NOTIF:
	case MCU_MCS_MMCUCALLIN_NOTIF: // guzh [6/20/2007] �ϼ�MCU����֪ͨ
                    
    //3.2.3	��������   
	case MCU_MCS_RELEASECONF_ACK:  //MCU�ɹ���������Ӧ��  
	case MCU_MCS_RELEASECONF_NACK: //MCU�ܾ���������   
	case MCU_MCS_RELEASECONF_NOTIF://MCU�ܾ���������

    //3.2.4	��������     
	case MCU_MCS_CHANGECONFLOCKMODE_ACK:   //MCU�ɹ���������Ӧ��  
	case MCU_MCS_CHANGECONFLOCKMODE_NACK:  //MCU�ܾ���������  
	case MCU_MCS_CHANGECONFPWD_ACK:    //MCU�ɹ����Ļ�������Ӧ��
	case MCU_MCS_CHANGECONFPWD_NACK:   //MCU�ܾ����Ļ�������
	case MCU_MCS_ENTERCONFPWD_REQ:     //MCUҪ��������̨�����������
	case MCU_MCS_WRONGCONFPWD_NOTIF:   //MCU��ʾ�������̨�������
	case MCU_MCS_LOCKUSERINFO_NOTIFY:  //MCU��ʾ�������̨��ǰ�����ص���Ϣ
	case MCU_MCS_GETLOCKINFO_ACK:      //�õ��������Ӧ��
	case MCU_MCS_GETLOCKINFO_NACK:     //�õ��������ܾ�

    //3.2.5	�������   
	case MCU_MCS_SAVECONF_ACK: //MCU�ɹ��������Ӧ��   
	case MCU_MCS_SAVECONF_NACK://MCU�ܾ��������


    //3.2.6	�޸�ԤԼ����   
	case MCU_MCS_MODIFYCONF_ACK://MCU�ɹ��޸�ԤԼ����   
	case MCU_MCS_MODIFYCONF_NACK: //MCU�ܾ��޸�ԤԼ����


    //3.2.7	�ӳ�����	
	case MCU_MCS_DELAYCONF_ACK:	//MCU�ɹ��ӳ�����Ӧ��	
	case MCU_MCS_DELAYCONF_NACK://MCU�ܾ��ӳ�����Ӧ��
	case MCU_MCS_DELAYCONF_NOTIF://MCU�ӳ�����֪ͨ
	case MCU_MCS_CONFTIMELEFT_NOTIF://MCU֪ͨ��ػ������ʣ��ʱ��
		
    //3.3	�������
    //3.3.1	ָ����ϯ	
	case MCU_MCS_SPECCHAIRMAN_ACK://MCU�ɹ�ָ��������ϯӦ��	
	case MCU_MCS_SPECCHAIRMAN_NACK://MCU�ܾ�ָ��������ϯӦ��
	case MCU_MCS_SPECCHAIRMAN_NOTIF://ָ����ϯ֪ͨ
	case MCU_MCS_MTAPPLYCHAIRMAN_NOTIF://�ն�������ϯ֪ͨ

    //3.3.2	ȡ����ϯ		
	case MCU_MCS_CANCELCHAIRMAN_ACK://MCU�ɹ�ȡ��������ϯӦ��	
	case MCU_MCS_CANCELCHAIRMAN_NACK://����������ϯ	
	case MCU_MCS_CANCELCHAIRMAN_NOTIF://ȡ����ϯ֪ͨ

    //3.3.3	ָ������	  
	case MCU_MCS_SPECSPEAKER_ACK://MCU�ɹ�ָ�������� 
	case MCU_MCS_SPECSPEAKER_NACK://ָ��֮ǰ�ķ����߱�ȡ�� 
	case MCU_MCS_SPECSPEAKER_NOTIF://ָ������֪ͨ
	case MCU_MCS_MTAPPLYSPEAKER_NOTIF://�ն����뷢��
	case MCU_MCS_MTAPPLYSPEAKERLIST_NOTIF://�ն����뷢��

    //3.3.4	ȡ������  
	case MCU_MCS_CANCELSPEAKER_ACK: //MCU�ɹ�ȡ��������ϯӦ��   
	case MCU_MCS_CANCELSPEAKER_NACK://ȡ��������ʧ��
	case MCU_MCS_CANCELSPEAKER_NOTIF://ȡ��������֪ͨ

    //3.3.5 ָ���ش�
    case MCU_MCS_SPECOUTVIEW_ACK:   //MCU�ɹ�ָ�����᳡���ϼ��ش����ն�
    case MCU_MCS_SPECOUTVIEW_NACK:  //ָ�����᳡���ϼ��ش����ն�ʧ��

    //3.3.6	�����ն�	
	case MCU_MCS_ADDMT_ACK://�����ն˳ɹ��������Ӧ��	
	case MCU_MCS_ADDMT_NACK://�����ն�û�м������

    //3.3.7	ɾ���ն�		
	case MCU_MCS_DELMT_ACK://�ն˱��ɹ�����	
	case MCU_MCS_DELMT_NACK://��Ϣ�������ܾ������ն�
	
    //3.3.10	���Ͷ���Ϣ		
	case MCU_MCS_SENDRUNMSG_NOTIF://MCU���������̨�ķ��Ͷ���Ϣ֪ͨ

    //3.3.11	�����ն��б�   
	case MCU_MCS_GETMTLIST_ACK: //MCU��Ӧ�������̨�ĵõ��ն��б�����   
	case MCU_MCS_GETMTLIST_NACK://MCU�ܾ����������б�   
	case MCU_MCS_MTLIST_NOTIF:  //MCU�����������̨���ն��б�֪ͨ
	case MCU_MCS_GETMTLIST_NOTIF: //MCU�����淢���ն��б���Ϣ
	case MCU_MCS_MTTYPECHANGE_NOTIF:    //MCU�����������̨���ն����͸ı�֪ͨ
	case MCU_MCS_CONFALLMTINFO_NOTIF:   //MCU�����������̨���ն�������Ϣ֪ͨ		

    case MCU_MCS_GETMTEXTINFO_ACK:      //��ȡ�ն˶�����չ��Ϣͬ��Ӧ��
    case MCU_MCS_GETMTEXTINFO_NACK:     //��ȡ�ն˶�����չ��Ϣ�ܾ�Ӧ��

    //3.3.12	��ѯ������Ϣ  
	case MCU_MCS_GETCONFINFO_ACK://MCU��Ӧ�������̨�Ļ�����Ϣ����   
	case MCU_MCS_GETCONFINFO_NACK://MCU�ܾ��������̨�Ļ�����Ϣ����

    //3.3.13    ��ѯMAU��Ϣ
    case MCU_MCS_GETMAUSTATUS_ACK:  //MCU��Ӧ�������̨��MAU��Ϣ����   
    case MCU_MCS_GETMAUSTATUS_NACK: //MCU�ܾ��������̨��MAU��Ϣ����
    case MCU_MCS_MAUSTATUS_NOTIFY:  //MCU֪ͨ�������̨��MAU��Ϣ

	//3.3.14    ����ģʽ����
    case MCU_MCS_CHGSPEAKMODE_ACK:  //MCU��Ӧͬ�ⷢ��ģʽ����
    case MCU_MCS_CHGSPEAKMODE_NACK: //MCU��Ӧ�ܾ�����ģʽ����	
    //3.4	�������
    //3.4.1	��ѯ����
    //3.4.1.1	��ѯ����   
    //3.4.1.2	��ѯ��ѯ����     
	case MCU_MCS_GETPOLLPARAM_ACK:      //MCUӦ��������̨��ѯ������ѯ����   
	case MCU_MCS_GETPOLLPARAM_NACK:     //MCU�ܾ��������̨��ѯ������ѯ����   
	case MCU_MCS_POLLSTATE_NOTIF:       //MCU֪ͨ�������̨��ǰ����ѯ״̬
    case MCU_MCS_POLLPARAMCHANGE_NTF:   //MCU֪ͨ���еĻ��ˢ����ѯ�ն��б�
	case MCU_MCS_SPECPOLLPOS_ACK:		//MCUӦ��������ָ̨����ѯλ��
	case MCU_MCS_SPECPOLLPOS_NACK:		//MCU�ܾ��������ָ̨����ѯλ��

    //����ǽ��ѯ
    case MCU_MCS_GETTWPOLLPARAM_ACK:
    case MCU_MCS_GETTWPOLLPARAM_NACK:
    case MCU_MCS_TWPOLLSTATE_NOTIF:

	//hdu��ѯ
    case MCU_MCS_GETHDUPOLLPARAM_ACK:
    case MCU_MCS_GETHDUPOLLPARAM_NACK:
    case MCU_MCS_HDUPOLLSTATE_NOTIF:

	//vmp��ͨ����ѯ
    case MCU_MCS_GETVMPPOLLPARAM_ACK:
    case MCU_MCS_GETVMPPOLLPARAM_NACK:
    case MCU_MCS_VMPPOLLSTATE_NOTIF:

    //3.4.2	������������
    //3.4.2.1	���������������Ʒ���     
	case MCU_MCS_STARTVAC_ACK://MCUͬ��������̨��ʼ�����������Ʒ��Ե�����   
	case MCU_MCS_STARTVAC_NACK://MCU�ܾ��������̨��ʼ�����������Ʒ��Ե�����
	case MCU_MCS_STARTVAC_NOTIF://MCU���������̨��ʼ�����������Ʒ��Ե�֪ͨ

    //3.4.2.2	ȡ�������������Ʒ���     
	case MCU_MCS_STOPVAC_ACK://MCUͬ��������ֹ̨ͣ�����������Ʒ��Ե�����  
	case MCU_MCS_STOPVAC_NACK://MCU�ܾ��������ֹ̨ͣ�����������Ʒ��Ե�����
	case MCU_MCS_STOPVAC_NOTIF:// libo [5/20/2005]

	//3.4.2.3  �������̨�ı����������л�ʱ��
	case MCU_MCS_CHANGEVACHOLDTIME_ACK:  //MCUͬ��������̨�ı����������л�ʱ�������  
	case MCU_MCS_CHANGEVACHOLDTIME_NACK: //MCU�ܾ��������̨�ı����������л�ʱ�������
	case MCU_MCS_CHANGEVACHOLDTIME_NOTIF://MCU���������̨�ı����������л�ʱ���֪ͨ

    //3.4.3	�����������
    //3.4.3.1	��ʼ����	
	case MCU_MCS_STARTDISCUSS_ACK:      //MCUͬ�⿪ʼ��������
	case MCU_MCS_STARTDISCUSS_NACK:     //MCU�ܾ���ʼ��������
	//case MCU_MCS_STARTDISCUSS_NOTIF:  //MCU��ʼ��������֪ͨ

    //3.4.3.2	��������		
	case MCU_MCS_STOPDISCUSS_ACK:       //MCUͬ��������̨�����������۵�����	
	case MCU_MCS_STOPDISCUSS_NACK:      //MCU�ܾ��������̨�����������۵�����
	//case MCU_MCS_STOPDISCUSS_NOTIF:   //MCU���������̨�����������۵�֪ͨ

    //3.4.3.3	��ѯ��������

    //zbq[11/01/2007] �����Ż�
	//case MCU_MCS_GETDISCUSSPARAM_ACK:  //MCUͬ���ѯ���۲���	
	//case MCU_MCS_GETDISCUSSPARAM_NACK: //MCU�ܾ���ѯ���۲���
	//case MCU_MCS_DISCUSSPARAM_NOTIF:   //MCU���������̨�����۲���֪ͨ
    case MCU_MCS_MIXPARAM_NOTIF:
    case MCU_MCS_GETMIXPARAM_ACK:

	//3.4.3.4  ������Ա����	
	case MCU_MCS_MTAPPLYMIX_NOTIF:    //�ն�����������֪ͨ

	//3.4.3.5  ���ƻ����»�����Ա�����滻����
//	case MCU_MCS_MODIFYMIXMEMBER_ACK:  //MCUͬ�ⶨ�ƻ����»�����Ա�����滻����
//	case MCU_MCS_MODIFYMIXMEMBER_NACK: //MCU�ܾ����ƻ����»�����Ա�����滻����

        //������ʱ����
    case MCU_MCS_CHANGEMIXDELAY_ACK:
    case MCU_MCS_CHANGEMIXDELAY_NACK:
    case MCU_MCS_CHANGEMIXDELAY_NOTIF:

    //3.4.4	��Ƶ���Ͽ���
    //3.4.4.1	��ʼ��Ƶ����		
	case MCU_MCS_STARTVMP_ACK:   //MCUͬ����Ƶ��������	
	case MCU_MCS_STARTVMP_NACK:  //MCU��ͬ����Ƶ��������
	case MCU_MCS_STARTVMP_NOTIF: //��Ƶ���ϳɹ���ʼ֪ͨ

    //3.4.4.2 ֹͣ��Ƶ����		
	case MCU_MCS_STOPVMP_ACK:   //MCUͬ����Ƶ������������	
	case MCU_MCS_STOPVMP_NACK:  //MCU��ͬ�������Ƶ��������
	case MCU_MCS_STOPVMP_NOTIF: //��Ƶ���ϳɹ�����֪ͨ
	
    //3.4.4.3	�ı���Ƶ���ϲ���	
	case MCU_MCS_CHANGEVMPPARAM_ACK://MCUͬ��������̨�ĸı������������	
	case MCU_MCS_CHANGEVMPPARAM_NACK://MCU�ܾ��������̨�ĸı������������
    case MCU_MCS_CHANGEVMPPARAM_NOTIF:

    //3.4.4.4	��ѯ��Ƶ���ϲ���			
	case MCU_MCS_GETVMPPARAM_ACK://MCUͬ���ѯ������Ա	
	case MCU_MCS_GETVMPPARAM_NACK://MCU��ͬ���ѯ������Ա	
	case MCU_MCS_VMPPARAM_NOTIF://MCU���������̨�Ļ�������֪ͨ
	
    //3.4.4.5	������Ƶ���㲥����
	case MCU_MCS_STARTVMPBRDST_ACK:  //MCUͬ��㲥��Ƶ��������	
	case MCU_MCS_STARTVMPBRDST_NACK: //MCU�ܾ��㲥��Ƶ��������	
	case MCU_MCS_STOPVMPBRDST_ACK:   //MCUͬ��ֹͣ�㲥��Ƶ��������	
	case MCU_MCS_STOPVMPBRDST_NACK:  //MCU�ܾ�ֹͣ�㲥��Ƶ��������

    //3.4.5	��ѯ����״̬		
	case MCU_MCS_GETCONFSTATUS_ACK://MCU������ѯ����״̬Ӧ��	
	case MCU_MCS_GETCONFSTATUS_NACK://MCU�ܾ�������ѯ����״̬Ӧ��	
	case MCU_MCS_CONFSTATUS_NOTIF://MCU���������̨�Ļ���״̬֪ͨ
        
    //------------------------------------------------------------------
    case MCU_MCS_STARTVMPTW_ACK:
    //MCU��ͬ�⸴�ϵ���ǽ����, ��Ϣ�壺 ��
    case MCU_MCS_STARTVMPTW_NACK:
    //���ϵ���ǽ�ɹ���ʼ֪ͨ, ��Ϣ�壺  TVMPParam
    case MCU_MCS_STARTVMPTW_NOTIF:

//3.4.4.2 ֹͣ���ϵ���ǽ
    //MCUͬ��������ϵ���ǽ����, ��Ϣ�壺  ��
    case MCU_MCS_STOPVMPTW_ACK:
    //MCU��ͬ��������ϵ���ǽ����, ��Ϣ�壺  ��
    case MCU_MCS_STOPVMPTW_NACK:
    //���ϵ���ǽ�ɹ�����֪ͨ, ��Ϣ�壺  ��
    case MCU_MCS_STOPVMPTW_NOTIF:

//3.4.4.3	�ı临�ϵ���ǽ����
    //MCUͬ��������̨�ĸı临�ϵ���ǽ��������, ��Ϣ�壺 TVMPParam
    case MCU_MCS_CHANGEVMPTWPARAM_ACK:
    //MCU�ܾ��������̨�ĸı临�ϵ���ǽ��������, ��Ϣ�壺  ��
    case MCU_MCS_CHANGEVMPTWPARAM_NACK:
    //MCU���������̨����Ƶ���ϲ���֪ͨ, ��Ϣ�壺  TVMPParam
	case MCU_MCS_VMPTWPARAM_NOTIF:

    //��������
    case MCU_MCS_STARTROLLCALL_ACK:
    case MCU_MCS_STARTROLLCALL_NACK:
    case MCU_MCS_STARTROLLCALL_NOTIF:

    case MCU_MCS_STOPROLLCALL_ACK:
    case MCU_MCS_STOPROLLCALL_NACK:
    case MCU_MCS_STOPROLLCALL_NOTIF:
        
    case MCU_MCS_CHANGEROLLCALL_ACK:
    case MCU_MCS_CHANGEROLLCALL_NACK:
    case MCU_MCS_CHANGEROLLCALL_NOTIF:
	//ȡ���ش�ͨ�����ն˺ͽ���
	case MCU_MCS_CANCELCASCADESPYMT_ACK:
	case MCU_MCS_CANCELCASCADESPYMT_NACK:
        
    //3.5	�ն˲���
    //3.5.1	�����ն�	
	case MCU_MCS_CALLMT_ACK://�������ն˳ɹ�Ӧ��	
	case MCU_MCS_CALLMT_NACK://�������ն�û��Ӧ��
	case MCU_MCS_MTONLINECHANGE_NOTIF: //�ն�����״̬�仯֪ͨ
	case MCU_MCS_CALLMTFAILURE_NOTIF: //�����ն�ʧ��֪ͨ
		
    //3.5.2	���ú����ն˷�ʽ	
	case MCU_MCS_SETCALLMTMODE_ACK://�ɹ�����MCU�����ն˷�ʽ	
	case MCU_MCS_SETCALLMTMODE_NACK://�ܾ�����MCU�����ն˷�ʽ	

    //3.5.3	�Ҷ��ն�		
	case MCU_MCS_DROPMT_ACK://�ն˱��ɹ��Ҷ�	
	case MCU_MCS_DROPMT_NACK://MCU�ܾ��Ҷϸ��ն�


    //3.5.4	ǿ���ն�ѡ��		
	case MCU_MCS_STARTSWITCHMT_ACK://MCUӦ����ǿ��Ŀ���ն�ѡ��Դ�ն�	
	case MCU_MCS_STARTSWITCHMT_NACK://�ܾ����ǿ��Ŀ���ն�ѡ��Դ�ն�
		
    //3.5.5	ȡ���ն�ѡ��		
	case MCU_MCS_STOPSWITCHMT_ACK://MCUӦ����ȡ��Ŀ���ն�ѡ��Դ�ն�	
	case MCU_MCS_STOPSWITCHMT_NACK://MCU�ܾ����ȡ��Ŀ���ն�ѡ��Դ�ն�


    //3.5.6	��ѯ�ն�״̬		
	case MCU_MCS_GETMTSTATUS_ACK://MCUӦ��������̨��ѯ�ն�״̬	
	case MCU_MCS_GETMTSTATUS_NACK://MCU�ܾ��������̨�Ĳ�ѯ�ն�״̬	
	case MCU_MCS_MTSTATUS_NOTIF://MCU���������̨�Ĳ�ѯ�ն�״̬֪ͨ
	case MCU_MCS_ALLMTSTATUS_NOTIF://�����ն�״̬֪ͨ

	case MCU_MCS_GETALLMTSTATUS_ACK:	//MCUӦ��������̨��ѯ�����ն�״̬
	case MCU_MCS_GETALLMTSTATUS_NACK:	//MCU�ܾ��������̨�Ĳ�ѯ�����ն�״̬

    //3.5.7	��ѯ�ն˱���		
	case MCU_MCS_GETMTALIAS_ACK://MCUӦ��������̨��ѯ�ն˱���	
	case MCU_MCS_GETMTALIAS_NACK://MCUû���ն˵ı�����Ϣ	
	case MCU_MCS_MTALIAS_NOTIF://MCU���������̨���ն˱���֪ͨ
	case MCU_MCS_ALLMTALIAS_NOTIF: //MCU���������̨�������ն˱���֪ͨ

	case MCU_MCS_GETALLMTALIAS_ACK:			//MCUӦ��������̨��ѯ�����ն˱���
	case MCU_MCS_GETALLMTALIAS_NACK:		//MCU�ܾ��������̨�Ĳ�ѯ�����ն˱���

    //3.5.10	�����ն���ƵԴ	
	//case MCU_MCS_SETMTVIDSRC_ACK://MCUӦ��������̨�����ն���ƵԴ��Ҫ��	
	//case MCU_MCS_SETMTVIDSRC_NACK://MCU�ܾ��������̨�����ն���ƵԴ��Ҫ��	
	//case MCU_MCS_SETMTVIDSRC_NOTIF://MCU֪ͨ�������̨�����������ն���ƵԴ

    //3.5.11	�����ն���ƵԴ	
	//case MCU_MCS_SETMTAUDSRC_ACK://MCUӦ��������̨�����ն���ƵԴ��Ҫ��	
	//case MCU_MCS_SETMTAUDSRC_NACK://MCU�ܾ��������̨�����ն���ƵԴ��Ҫ��	
	//case MCU_MCS_SETMTAUDSRC_NOTIF://MCU֪ͨ�������̨�����������ն���ƵԴ
	
    //3.5.13	�����ն˾���	
	case MCU_MCS_MTAUDMUTE_ACK://MCUӦ��������̨�����ն˾���	
	case MCU_MCS_MTAUDMUTE_NACK://MCU�ܾ��������̨�����ն˾���	
	case MCU_MCS_MTAUDMUTE_NOTIF://MCU֪ͨ�������̨�����ն˾���


    //3.5.15�й���Ƶ���þ������
    case MCU_MCS_MATRIX_ALLSCHEMES_NOTIF:
    case MCU_MCS_MATRIX_ONESCHEME_NOTIF:
    case MCU_MCS_MATRIX_SAVESCHEME_NOTIF:
    case MCU_MCS_MATRIX_SETCURSCHEME_NOTIF:
    case MCU_MCS_MATRIX_CURSCHEME_NOTIF:

        //���þ���
    case MCU_MCS_EXMATRIXINFO_NOTIFY:   
    case MCU_MCS_EXMATRIX_GETPORT_ACK:
    case MCU_MCS_EXMATRIX_GETPORT_NACK:
    case MCU_MCS_EXMATRIX_GETPORT_NOTIF:
    case MCU_MCS_EXMATRIX_PORTNAME_NOTIF:
    case MCU_MCS_EXMATRIX_ALLPORTNAME_NOTIF:

        //��չ��ƵԴ
    case MCU_MCS_ALLVIDEOSOURCEINFO_NOTIF:
    case MCU_MCS_VIDEOSOURCEINFO_NOTIF:
		
    //3.6.1.1.1	����¼��	
	case MCU_MCS_STARTREC_ACK://MCUͬ���ط�������	
	case MCU_MCS_STARTREC_NACK://MCU�ܾ���ط�������	
	case MCU_MCS_RECPROG_NOTIF://MCU֪ͨ���¼�����

    //3.6.1.1.2	��ͣ¼������	
	case MCU_MCS_PAUSEREC_ACK://MCUͬ������ͣ��������	
	case MCU_MCS_PAUSEREC_NACK://MCU�ܾ������ͣ��������

    //3.6.1.1.3	�ָ�¼��	
	case MCU_MCS_RESUMEREC_ACK://MCUͬ���ػָ���������	
	case MCU_MCS_RESUMEREC_NACK://MCU�ܾ���ػָ���������

    //3.6.1.1.4	ֹͣ¼��		
	case MCU_MCS_STOPREC_ACK://MCUͬ����ֹͣ��������	
	case MCU_MCS_STOPREC_NACK://MCU�ܾ����ֹͣ��������

    //3.6.1.2	�������
    //3.6.1.2.1	��ʼ����		
	case MCU_MCS_STARTPLAY_ACK://MCU��ʼ����Ӧ��	
	case MCU_MCS_STARTPLAY_NACK://MCU�ܾ���ʼ����	
	case MCU_MCS_PLAYPROG_NOTIF://�������֪ͨ

	case MCU_MCS_CHANGERECMODE_ACK:			//����ı�¼��ģʽ
	case MCU_MCS_CHANGERECMODE_NACK:		//����ı�¼��ģʽ
	
    //3.6.1.2.2	��ͣ����		
	case MCU_MCS_PAUSEPLAY_ACK://MCU��ͣ����Ӧ��	
	case MCU_MCS_PAUSEPLAY_NACK://MCU�ܾ���ͣ����

    //3.6.1.2.3	�ָ�����		
	case MCU_MCS_RESUMEPLAY_ACK://MCU�ָ�����Ӧ��	
	case MCU_MCS_RESUMEPLAY_NACK://MCU�ܾ��ָ�����

    //3.6.1.2.4	ֹͣ����	
	case MCU_MCS_STOPPLAY_ACK://MCUֹͣ����Ӧ��	
	case MCU_MCS_STOPPLAY_NACK://MCUֹͣ�ָ�����

    //3.6.1.2.5	����������		
	case MCU_MCS_FFPLAY_ACK://MCU�������Ӧ��	
	case MCU_MCS_FFPLAY_NACK://MCU�������

    //3.6.1.2.6	������˷���	
	case MCU_MCS_FBPLAY_ACK://MCU�������Ӧ��	
	case MCU_MCS_FBPLAY_NACK://MCU�������

    //3.6.1.2.7	������ȵ���	
	case MCU_MCS_SEEK_ACK://MCUͬ������������Ӧ��	
	case MCU_MCS_SEEK_NACK://MCU�ܾ������������Ӧ��	

    //3.6.3	����ǽ����	
	case MCU_MCS_START_SWITCH_TW_ACK:      //MCUͬ�⽻��ͼ��	
	case MCU_MCS_START_SWITCH_TW_NACK:     //MCUͬ��ܾ�ͼ��		
	case MCU_MCS_STOP_SWITCH_TW_ACK:       //MCUͬ�⽻��ͼ��	
	case MCU_MCS_STOP_SWITCH_TW_NACK:      //MCU�ܾ�����ͼ��
		
	case MCU_MCS_GETPERIDCSSTATUS_ACK:		//MCUӦ���ѯ���ֻ���״̬����
	case MCU_MCS_GETPERIDCSSTATUS_NACK:		//MCU�ܾ���ѯ���ֻ���״̬����
	
	case MCU_MCS_MCUMEDIASRC_NOTIF:
	case MCU_MCS_MCUMEDIASRC_ACK:
	case MCU_MCS_MCUMEDIASRC_NACK:
	case MCU_MCS_LOCKSMCU_ACK:
	case MCU_MCS_LOCKSMCU_NACK:
	case MCU_MCS_GETMCULOCKSTATUS_ACK:
	case MCU_MCS_GETMCULOCKSTATUS_NACK:
	case MCU_MCS_MCULOCKSTATUS_NOTIF:

    case MCU_MCS_CALLMTMODE_NOTIF:
    case MCU_MCS_CONFLOCKMODE_NOTIF:
    case MCU_MCS_CONFCHAIRMODE_NOTIF:
    case MCU_MCS_CHANGECONFPWD_NOTIF:
    case MCU_MCS_CONFMODE_NOTIF:
    case MCU_MCS_SIMCONFINFO_NOTIF:

    case MCU_MCS_GETMTBITRATE_ACK:
    case MCU_MCS_GETMTBITRATE_NACK:
    case MCU_MCS_MTBITRATE_NOTIF:

    case MCU_MCS_MCUPERIDCSSTATUS_NOTIF:
    case MCU_MCS_LISTALLRECORD_NOTIF:  //¼���¼�б�֪ͨ

	case MCU_MCS_MPFLUXOVERRUN_NOTIFY: //zgc [2007/01/12] ֪ͨMCS����ת����������

	case MCU_MCS_VMPPRISEIZE_REQ:	   // xliang [1/20/2009] VMP ��ռ��ʾ����
	case MCU_MCS_VMPOVERHDCHNNLLMT_NTF:
	case MCU_MCS_STARTSWITCHVMPMT_ACK:	// xliang [3/31/2009] ��ϯѡ��VMPӦ��
	case MCU_MCS_STARTSWITCHVMPMT_NACK:

    // 4.6 HDU
	case MCU_MCS_START_SWITCH_HDU_ACK:   //  jlb  [2009/02/05] 
	case MCU_MCS_START_SWITCH_HDU_NACK:
	case MCU_MCS_STOP_SWITCH_HDU_ACK:
	case MCU_MCS_STOP_SWITCH_HDU_NACK:
	case MCU_MCS_CHGHDUVMPMODE_NOTIF:
		
	//�޸�HduԤ��Ӧ��
	case MCU_MCS_CHANGEHDUSCHEMEINFO_ACK:
	case MCU_MCS_CHANGEHDUSCHEMEINFO_NACK:
	case MCU_MCS_HDUSCHEMEINFO_NOTIF:
    
	//������ѯӦ��
	case MCU_MCS_STARTHDUBATCHPOLL_ACK:
	case MCU_MCS_STARTHDUBATCHPOLL_NACK:
	case MCU_MCS_STOPHDUBATCHPOLL_ACK:
	case MCU_MCS_STOPHDUBATCHPOLL_NACK:
	case MCU_MCS_RESUMEHDUBATCHPOLL_ACK:
	case MCU_MCS_RESUMEHDUBATCHPOLL_NACK:
	case MCU_MCS_PAUSEHDUBATCHPOLL_ACK:
	case MCU_MCS_PAUSEHDUBATCHPOLL_NACK:
	
	//������ѯ֪ͨ��Ϣ
	case MCU_MCS_HDUBATCHPOLL_STATUS_NOTIF:

	// [pengjie 2010/5/18] �¼�������չ����֪ͨ���棨�Ƿ�֧�ֶ�ش���
	case MCU_MCS_CONFEXTINFO_NOTIF:
	
	//[2/28/2012 zhushengze]���桢�ն���Ϣ͸��
    case MCU_MCS_TRANSPARENTMSG_NOTIFY:

	//  �ն���ƵԴ������Ϣ֪ͨ����
	case MCU_MCS_GETMTVIDEOALIAS_ACK:
	case MCU_MCS_GETMTVIDEOALIAS_NACK:
	case MCU_MCS_MTVIDEOALIAS_NOTIF:

	case MCU_VCS_ADDPLANNAME_ACK:
	case MCU_VCS_ADDPLANNAME_NACK:
	case MCU_VCS_DELPLANNAME_ACK:
	case MCU_VCS_DELPLANNAME_NACK:
	case MCU_VCS_MODIFYPLANNAME_ACK:
	case MCU_VCS_MODIFYPLANNAME_NACK:
	case MCU_VCS_ALLPLANNAME_NOTIFY:
	case MCU_VCS_ONEPLANDATA_NOTIFY:
	case MCU_VCS_GETALLPLANDATA_ACK:
	case MCU_VCS_GETALLPLANDATA_NACK:
	case MCU_VCS_ALLPLANDATA_SINGLE_NOTIFY:
	case MCU_VCS_MODIFYPLANNAME_NOTIFY:
	case MCU_MCS_NETCAPSTATUS_NOTIFY: //ץ��״̬ͨ��
	case MCU_MCS_OPENDEBUGMOED_NOTIFY:
		if(MCU_MCS_CALLMTFAILURE_NOTIF != pcMsg->event
			&& MCU_MCS_CONFALLMTINFO_NOTIF != pcMsg->event
			&& MCU_MCS_RECPROG_NOTIF != pcMsg->event
			&& MCU_MCS_PLAYPROG_NOTIF != pcMsg->event
			&& MCU_MCS_MTSTATUS_NOTIF != pcMsg->event)
		{
			MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		}else
		{
			MCS_EV_LOG(LOG_LVL_DETAIL);
		}
		ProcMcuMcsMsg(pcMsg, TRUE);
		break;

        //��ʾ��Ϣ֪ͨ��Ϣ
    case MCU_MCS_ALARMINFO_NOTIF:

        // �û���Ϣ��ҵ����ƹ��ˣ�����������
    case MCU_MCS_ADDUSER_NOTIF:
    case MCU_MCS_CHANGEUSER_NOTIF:
    case MCU_MCS_USERGRP_NOTIF:
    case MCU_MCS_DELUSERGRP_NOTIF:
        
        // ��ַ����Ϣ��������
    case MCU_MCS_ADDRBOOK_ADDENTRY_NOTIF:
    case MCU_MCS_ADDRBOOK_DELENTRY_NOTIF:
    case MCU_MCS_ADDRBOOK_MODIFYENTRY_NOTIF:
    case MCU_MCS_ADDRBOOK_ADDGROUP_NOTIF:
    case MCU_MCS_ADDRBOOK_DELGROUP_NOTIF:
    case MCU_MCS_ADDRBOOK_MODIFYGROUP_NOTIF:
    case MCU_MCS_ADDRBOOK_ADDENTRYTOGROUP_NOTIF:
    case MCU_MCS_ADDRBOOK_DELENTRYFROMGROUP_NOTIF:

        // ��������CConfId������

    ////3.2	���������������NACK���ܻ�û��CConfId����
	case MCU_MCS_CREATECONF_ACK:  //�������̨��MCU�ϴ���һ������ɹ�Ӧ��  
	case MCU_MCS_CREATECONF_NACK: //�������̨��MCU�ϴ���һ������ʧ��
	case MCU_MCS_CREATECONF_BYTEMPLATE_ACK://����ģ��ɹ���ת��ɼ�ʱ����
	case MCU_MCS_CREATECONF_BYTEMPLATE_NACK://����ģ��ת��ɼ�ʱ����ʧ��
    case MCU_MCS_CREATESCHCONF_BYTEMPLATE_ACK:
    case MCU_MCS_CREATESCHCONF_BYTEMPLATE_NACK:
    case MCU_MCS_CREATETEMPLATE_ACK:
    case MCU_MCS_CREATETEMPLATE_NACK:
	case MCU_MCS_SAVECONFTOTEMPLATE_ACK:
	case MCU_MCS_SAVECONFTOTEMPLATE_NACK:
        
	//3.1.1	����    
	case MCU_MCS_CONNECT_ACK:  //MCU׼��Ӧ��   
	case MCU_MCS_CONNECT_NACK: //MCU�ܾ�Ӧ��

    //3.1.2	ʱ��ͬ��  
	case MCU_MCS_CURRTIME_ACK:   //mcu�ɹ�Ӧ��   
	case MCU_MCS_CURRTIME_NACK:  //mcu�ܾ�Ӧ��
	
    //3.1.3	��ѯMCU״̬
	case MCU_MCS_GETMCUSTATUS_ACK:       //���صĳɹ�Ӧ�� 
	case MCU_MCS_GETMCUSTATUS_NACK:      //���ص�ʧ��Ӧ��
	case MCU_MCS_MCUSTATUS_NOTIF:        //MCU״̬֪ͨ
	case MCU_MCS_MCUEXTSTATUS_NOTIFY:	 //mcu����״̬֪ͨ	
	case MCU_MCS_MCUPERIEQPSTATUS_NOTIF: //����״̬֪ͨ

	case MCU_MCS_LISTALLCONF_ACK:   //Ӧ��MCS_MCU_LISTALLCONF_REQ 
	case MCU_MCS_LISTALLCONF_NACK:  //MCU��û�л���  

	case MCU_MCS_RECORDEREXCPT_NOTIF://��Ϣ����������¼�����쳣֪ͨ
	case MCU_MCS_RECORDCHNSTATUS_NOTIF://��¼��¼��״̬�ϱ�

    //3.6.1.3	��ѯ����¼�ļ�����
    //3.6.1.3.1	��ѯ¼���״̬	
	case MCU_MCS_GETRECSTATUS_ACK://MCU��ѯ¼���״̬Ӧ��	
	case MCU_MCS_GETRECSTATUS_NACK://MCU��ѯ¼���״̬�ܾ�

    //3.6.1.3.2	��¼�б�����
	case MCU_MCS_LISTALLRECORD_ACK:	   //��ѯ��¼Ӧ��
	case MCU_MCS_LISTALLRECORD_NACK:   //��ѯ��¼�ܾ�	

    //3.6.1.3.3	ɾ����¼�ļ�����	
	case MCU_MCS_DELETERECORD_ACK:   //�������ɾ���ļ�Ӧ��	
	case MCU_MCS_DELETERECORD_NACK:  //�������ɾ���ļ��ܾ�
        
    //3.6.1.3.3	ɾ����¼�ļ�����	
	case MCU_MCS_RENAMERECORD_ACK:   //�����������ļ�Ӧ��	
	case MCU_MCS_RENAMERECORD_NACK:  //�����������ļ��ܾ�

	//3.6.1.4 �ļ���������
	//3.6.1.4.1 �������MCU�����ļ�
	case MCU_MCS_PUBLISHREC_ACK:			//����¼��Ӧ��	
	case MCU_MCS_PUBLISHREC_NACK:			//����¼��ܾ�

	//3.6.1.4.2 �������MCUȡ�������ļ�
    case MCU_MCS_CANCELPUBLISHREC_ACK:     //ȡ������Ӧ��
	case MCU_MCS_CANCELPUBLISHREC_NACK:    //ȡ�������ܾ�
		
    //3.6.2	����������	
	case MCU_MCS_GETMIXERSTATUS_ACK:       //��ѯ����������Ӧ��	
	case MCU_MCS_GETMIXERSTATUS_NACK:      //��ѯ����������ܾ�

	case MCU_MCS_GETMCUPERIEQPSTATUS_ACK:  //MCUӦ��������̨�Ĳ�ѯ����״̬����	
	case MCU_MCS_GETMCUPERIEQPSTATUS_NACK: //MCU�ܻ������̨�Ĳ�ѯ����״̬����

    //3.3.8	���ѡ��		
	case MCU_MCS_STARTSWITCHMC_ACK://MCUӦ��������̨ѡ���ն�	
	case MCU_MCS_STARTSWITCHMC_NACK://MCU�ܾ��������̨ѡ���ն�	
	case MCU_MCS_STARTPLAY_NOTIF://MCU����صĿ�ʼѡ��֪ͨ

    //3.3.9	���ȡ��ѡ��	
	case MCU_MCS_STOPSWITCHMC_ACK://MCUӦ��������̨ѡ���ն�	
	case MCU_MCS_STOPSWITCHMC_NACK://MCU�ܾ��������̨ѡ���ն�	
	case MCU_MCS_STOPPLAY_NOTIF://MCU����ص�ֹͣѡ��֪ͨ
    
	// 4.6.1 �ı�hdu������С
	case MCU_MCS_CHANGEHDUVOLUME_ACK:     // �ı�������СӦ��
	case MCU_MCS_CHANGEHDUVOLUME_NACK:    // �ı�������С�ܾ�
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcuMcsMsg(pcMsg, FALSE);
		break;

    case MCU_MCS_DELUSER_NOTIF:
        // ɾ���û����⴦�������ɾ�����Լ������Mcs����
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcuMcsMsg(pcMsg, FALSE);
        ProcMcuDelUser(pcMsg);
		break;


        //MCU�û��������
    case MCS_MCU_ADDUSER_REQ:           //�������̨����MCU����û�
    case MCS_MCU_DELUSER_REQ:           //�������̨����MCUɾ���û�
    case MCS_MCU_GETUSERLIST_REQ:       //�������̨����MCU�õ��û��б�
    case MCS_MCU_CHANGEUSER_REQ:        //�������̨����MCU�޸��û�����

    case MCS_MCU_ADDUSERGRP_REQ:        //�������̨����MCU����û���
    case MCS_MCU_CHANGEUSERGRP_REQ:     //�������̨����MCU�޸��û���
    case MCS_MCU_DELUSERGRP_REQ:        //�������̨����MCUɾ���û���
    case MCS_MCU_GETUSERGRP_REQ:        //�������̨����MCU��ȡ�û�����Ϣ
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
        ProcUserManagerMsg( pcMsg );
        break;
        
        //MCU��ַ������
    case MCS_MCU_ADDRBOOK_GETENTRYLIST_REQ:
    case MCS_MCU_ADDRBOOK_GETGROUPLIST_REQ:
    case MCS_MCU_ADDRBOOK_ADDENTRY_REQ:
    case MCS_MCU_ADDRBOOK_DELENTRY_REQ:
    case MCS_MCU_ADDRBOOK_MODIFYENTRY_REQ:
    case MCS_MCU_ADDRBOOK_ADDGROUP_REQ:
    case MCS_MCU_ADDRBOOK_DELGROUP_REQ:
    case MCS_MCU_ADDRBOOK_MODIFYGROUP_REQ:
    case MCS_MCU_ADDRBOOK_ADDENTRYTOGROUP_REQ:
    case MCS_MCU_ADDRBOOK_DELENTRYFROMGROUP_REQ:
    case MCU_ADDRBOOK_GETENTRYLIST_NOTIF:
    case MCU_ADDRBOOK_GETGROUPLIST_NOTIF:
        ProcAddrBookMsg( pcMsg );
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
        break;
        
    //mcu���ý��滯��Ϣ
    case MCS_MCU_CPUPERCENTAGE_REQ:         //MCS����mcu ��cpuռ����
    case MCS_MCU_CPUPERCENTAGE_CMD:    
    case MCS_MCU_CHANGEMCUEQPCFG_REQ:       //MCS�����޸�mcu�豸������Ϣ     
    case MCS_MCU_GETMCUEQPCFG_REQ:          //MCS����mcu�豸������Ϣ        
    case MCS_MCU_CHANGEBRDCFG_REQ:          //mcs�޸ĵ���������Ϣ        
    case MCS_MCU_GETBRDCFG_REQ:             //mcs ���󵥰�������Ϣ   
    case MCS_MCU_CHANGEMCUGENERALCFG_REQ:   //mcs�޸�mcu������Ϣ��������        
    case MCS_MCU_GETMCUGENERALCFG_REQ:      // mcs ����mcu����������Ϣ
    case MCS_MCU_GETVMPSCHEMEINFO_REQ:
    case MCS_MCU_REBOOTBRD_REQ:
//	case MCS_MCU_UPLOADFILENAME_REQ:
    case MCS_MCU_UPDATEBRDVERSION_CMD:
	case MCS_MCU_STARTUPDATEDSCVERSION_REQ:		// mcs�������DSC��汾, zgc, 2007-08-20
	case MCS_MCU_DSCUPDATEFILE_REQ:
	case MCS_MCU_UPDATETELNETLOGININFO_REQ:		//�������̨����MCU����telnet��½��Ϣ, zgc, 2007/10/19
        //Զ������
    case MCS_MCU_REBOOT_CMD:	
    case MCS_MCU_GETHDUSCHEMEINFO_REQ:      //��ȡԤ��
    case MCS_MCU_CHANGEHDUSCHEMEINFO_REQ:   //�޸�Ԥ��
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
        ProcMcuCfgMsg( pcMsg );
        break;
    case MCU_MCS_DOWNLOADCFG_REQ:           // MCS ���������������ļ�
    case MCU_MCS_UPLOADCFG_REQ:
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
        ProcMcuCfgFileMsg( pcMsg );
        break;

    //��Ӧ��Ϣ    
    case MCU_MCS_CPUPERCENTAGE_ACK:
    case MCU_MCS_CPUPERCENTAGE_NACK:
    case MCU_MCS_CPUPERCENTAGE_NOTIF:
    case MCU_MCS_CHANGEMCUEQPCFG_ACK:
    case MCU_MCS_CHANGEMCUEQPCFG_NACK:
    case MCU_MCS_GETMCUEQPCFG_ACK:
    case MCU_MCS_GETMCUEQPCFG_NACK:
    case MCU_MCS_MCUEQPCFG_NOTIF:
    case MCU_MCS_CHANGEBRDCFG_ACK:
    case MCU_MCS_CHANGEBRDCFG_NACK:
    case MCU_MCS_GETBRDCFG_ACK:
    case MCU_MCS_GETBRDCFG_NACK:
    case MCU_MCS_BRDCFG_NOTIF:
    case MCU_MCS_CHANGEMCUGENERALCFG_ACK:
    case MCU_MCS_CHANGEMCUGENERALCFG_NACK:
    case MCU_MCS_GETMCUGENERALCFG_ACK:
    case MCU_MCS_GETMCUGENERALCFG_NACK:
    case MCU_MCS_MCUGENERALCFG_NOTIF:
    case MCU_MCS_REBOOTBRD_ACK:
    case MCU_MCS_REBOOTBRD_NACK:
    case MCU_MCS_GETVMPSCHEMEINFO_ACK:
    case MCU_MCS_GETVMPSCHEMEINFO_NACK:
    case MCU_MCS_BOARDSTATUS_NOTIFY:
    case MCU_MCS_UPDATEBRDVERSION_NOTIF:
//	case MCU_MCS_UPLOADFILENAME_ACK:
//	case MCU_MCS_UPLOADFILENAME_NACK:
	case MCU_MCS_NEWDSCREGREQ_NOTIFY:  //zgc [2007/03/06] ֪ͨMCS�����û����µ�DSC�巢��ע��

	case MCU_MCS_STARTUPDATEDSCVERSION_ACK:		//��ʼDSC�����ɹ���Ӧ
	case MCU_MCS_STARTUPDATEDSCVERSION_NACK:	//��ʼDSC����ʧ�ܻ�Ӧ
	case MCU_MCS_DSCUPDATEFILE_ACK:				//�ļ������ճɹ���Ӧ
	case MCU_MCS_DSCUPDATEFILE_NACK:			//�ļ�������ʧ�ܻ�Ӧ

	case MCU_MCS_UPDATETELNETLOGININFO_ACK:		//����telnet��½�ɹ�, zgc, 2007/10/19
	case MCU_MCS_UPDATETELNETLOGININFO_NACK:	//����telnet��½ʧ��, zgc, 2007/10/19

	case MCU_MCS_GETHDUSCHEMEINFO_ACK:
	case MCU_MCS_GETHDUSCHEMEINFO_NACK:
	case MCU_MCS_GETCRIMAC_ACK:
	case MCU_MCS_GETCRIMAC_NACK:
	case MCU_MCS_GETCRIMAC_NOTIF:
        MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcuCfgRsp(pcMsg);
        break;

    //N+1������Ϣ
    case MCS_MCU_NPLUSROLLBACK_REQ:
    case MCS_MCU_NPLUSRESET_CMD:
		MCS_EV_LOG(LOG_LVL_DETAIL);
        ProcMcsNPlusReq(pcMsg);
        break;

    case MCU_MCS_NPLUSROLLBACK_ACK:
    case MCU_MCS_NPLUSROLLBACK_NACK:
		MCS_EV_LOG(LOG_LVL_DETAIL);
        ProcNPlusMcsRsp(pcMsg);
        break;
            
	//��Ԫ������Ϣ: (1) �����ȱ���
	case EV_TEST_TEMPLATEINFO_GET_ACK:
	case EV_TEST_CONFINFO_GET_ACK:
	case EV_TEST_CONFMTLIST_GET_ACK:
	case EV_TEST_ADDRBOOK_GET_ACK:
	case EV_TEST_MCUCONFIG_GET_ACK:
		MCS_EV_LOG(LOG_LVL_DETAIL);
		ProcMcuUnitTestMsg(pcMsg);
		break;
		
	// [6/28/2011 liuxu] ����֪ͨˢ��log
	case MCS_MCU_FLUSHLOG_CMD:
		mculogflush();
		break;
#if defined(_8KH_) ||  defined(_8KE_) || defined(_8KI_)
	//ץ�����
	case MCS_MCU_STARTNETCAP_CMD:
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcsMcuStartNetCapCMD(pcMsg);
		break;
	case MCS_MCU_STOPNETCAP_CMD:
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcsMcuStoptNetCapCmd(pcMsg);
		break;
	case MCS_MCU_GETNETCAPSTATUS_REQ:
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcsMcuGetNetCapStatusReq(pcMsg);
		break;
	case MCS_MCU_OPENDEBUGMODE_CMD:
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcsMcuOpenDebugModeCmd(pcMsg);
		break;
	case MCS_MCU_STOPDEBUGMODE_CMD:
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcsMcuStopDebugModeCmd(pcMsg);
		break;
	case MCS_MCU_GETDEBUGMODE_REQ:
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcsMcuGetDebugModeReq(pcMsg);
		break;
#endif
	//ת��
	default:
		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[InstanceEntry]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
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
void CMcsSsnInst::DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp )
{
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "CMcsSsnInst: The received msg's pointer in the msg entry is NULL!");
		return;
	}
	
	switch( pcMsg->event )
	{
    case OSP_POWERON:
        DaemonProcPowerOn();
        break;
	case MCU_APPTASKTEST_REQ:			//GUARD Probe Message
		DaemonProcAppTaskRequest(pcMsg);
		break;
	case TEST_MCU_TESTUSERINFO_CMD:
		DaemonProcIntTestUserInfo(pcMsg);
		break;
        //N+1ע��ɹ�����Ҫͬ���û�����û���Ϣ
    case NPLUS_VC_DATAUPDATE_NOTIF:
        DaemonProcNPlusUsrGrpNotif( pcMsg );
        break;
	case MCU_ADDRINITIALIZED_NOTIFY:
		DaemonProcAddrInitialized(pcMsg);
		break;

	default:
		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[DaemonInstanceEntry]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u appId.%u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode,pcApp!=NULL?pcApp->appId:0);
		break;
	}
}

/*====================================================================
    ������      ��ProcMcsMcuConnectReq
    ����        ������̨֪ͨMCU����׼�봦����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/26    1.0         LI Yi         ����
====================================================================*/
void CMcsSsnInst::ProcMcsMcuConnectReq( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMcsRegInfo	tRegReq = *( TMcsRegInfo * )cServMsg.GetMsgBody();
    // guzh [10/11/2007] �ֶ����ܣ����紫��������Ǽ��ܵ����ݣ�
    tRegReq.DecPwd();

    //[4/9/2013 liaokang] ���뷽ʽ
	/*lint -save -esym(550,byMcsEncoding)*/
    u8 byMcsEncoding = emenCoding_GBK;
    /*lint -restore*/
    if( cServMsg.GetMsgBodyLen() >= (sizeof(TMcsRegInfo) + sizeof(u8) + sizeof(TMcsRegInfo)) )
    {
        byMcsEncoding = *(u8*)( cServMsg.GetMsgBody() + sizeof(TMcsRegInfo) );
#ifdef _UTF8
        if( emenCoding_Utf8 == byMcsEncoding )
        {
            memset( &tRegReq, 0, sizeof(TMcsRegInfo) );
            tRegReq.SetMcuMcsVer();
            tRegReq = *( TMcsRegInfo * )(cServMsg.GetMsgBody() + sizeof(TMcsRegInfo) + sizeof(u8));
            tRegReq.DecPwd();
        }
#endif
    }
   
	u8 byInstId = ( u8 )GetInsID();
	CLoginRequest LoginRequest;
	
	u32 dwSysSSrc = g_cMSSsnApp.GetMSSsrc();
    
	switch( CurState() )
	{
	case STATE_IDLE:
        {
		    //������Ϣ
		    m_dwMcsNode = pcMsg->srcnode;	
		    m_dwMcsIId  = pcMsg->srcid;
			m_dwMcsIp = tRegReq.m_dwMcsIpAddr;

            emMCUMSState emLocalMSState = MCU_MSSTATE_ACTIVE;
		    emLocalMSState  = g_cMSSsnApp.GetCurMSState();                
		    if (MCU_MSSTATE_OFFLINE == emLocalMSState)
		    {
			    //MCU��δ��ʼ����ϣ��ܾ����Ͽ�����
				cServMsg.SetErrorCode( ERR_MCU_INITIALIZING );
			    SendMsgToMcs( pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), FALSE );	
                OspDelay(100);
                
			    ::OspDisconnectTcpNode( m_dwMcsNode );
			    m_dwMcsNode = INVALID_NODE;	            
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[Mcsssn][ProcMcsMcuConnectReq]: invalid ms state, NACK.\n");

			    return;
		    }           
 
			// ����VCS�������ж����Ƿ���Ȩ

			u16 wInstID = GetInsID();
#ifndef _NOVCSCHECK_
			if (wInstID > MAXNUM_MCU_MC && wInstID <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC)
				&& g_cMcuVcApp.IsVCSNumOverLicense())
			{
                cServMsg.SetErrorCode(ERR_MCU_VCSMCS_LOGVCSNUMOVER);
                SendMsgToMcs( pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), FALSE );		
                
                OspDelay(100);
                
                ::OspDisconnectTcpNode( m_dwMcsNode );
			    m_dwMcsNode = INVALID_NODE;	            
                LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[Mcsssn][ProcMcsMcuConnectReq]: Over VCS access ability\n");
				return;
			}
#endif
			// MCS��VCS��½����
// 			u8  byEnableLog = TRUE;
// 			if (wInstID > 0 && wInstID <= MAXNUM_MCU_MC)
// 			{
// 				for (u16 wInstIdx = MAXNUM_MCU_MC + 1; wInstIdx <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); wInstIdx++)
// 				{
// 					if (g_cMcuVcApp.IsMcConnected(wInstIdx))
// 					{
// 						byEnableLog = FALSE;
// 						break;
// 					}
// 				}
// 			}
// 			else
// 			{
// 				for (u16 wInstIdx = 1; wInstIdx <= MAXNUM_MCU_MC; wInstIdx++)
// 				{
// 					if (g_cMcuVcApp.IsMcConnected(wInstIdx))
// 					{
// 						byEnableLog = FALSE;
// 						break;
// 					}
// 				}
// 
// 			}
// 
// 			if(!byEnableLog)
// 			{
//                 cServMsg.SetErrorCode(ERR_MCU_VCSMCS_LOGONLYONE);
//                 SendMsgToMcs( pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), FALSE );		
//                 
//                 OspDelay(100);
//                 
//                 ::OspDisconnectTcpNode( m_dwMcsNode );
// 			    m_dwMcsNode = INVALID_NODE;	            
//                 LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[Mcsssn][ProcMcsMcuConnectReq]: MCS VCS login only one\n");
// 				return;
// 			}
			
            //У��汾
            if ( tRegReq.GetMcuMcsVer() != MCUMCS_VER
#ifdef _UTF8
                || emenCoding_GBK == byMcsEncoding    //[5/10/2013 liaokang] ֧�ֶ������  MCU��UTF-8��ʽ���룬���ټ�����MCS
#endif
                )
            {
                cServMsg.SetErrorCode(ERR_MCU_VER_UNMATCH);
                SendMsgToMcs( pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), FALSE );		
                
                OspDelay(100);
                
                ::OspDisconnectTcpNode( m_dwMcsNode );
			    m_dwMcsNode = INVALID_NODE;	            
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcMcsMcuConnectReq]: mcs version(%d) differs from mcu(%d), NACK.\n",
                          tRegReq.GetMcuMcsVer(), MCUMCS_VER);

                return;
            }		    
       
		    LoginRequest.SetName( (char *)tRegReq.GetUser() );
		    LoginRequest.SetPassword( (char *)tRegReq.GetPwd() ); 

			// 1��16 17��32��ȡ�û��ʻ��ֿ�����
            u8 byGrpId = USRGRPID_INVALID;
			BOOL32 dwValid = FALSE;
			u16 wErrCode = 0;
			if (wInstID > 0 && wInstID <= MAXNUM_MCU_MC)
			{
 				dwValid  = CMcsSsn::CheckLoginValid(g_cUsrManage, &LoginRequest, byGrpId);
				wErrCode = g_cUsrManage.GetLastError(); 
			}
			else
			{
				dwValid  = CMcsSsn::CheckLoginValid(g_cVCSUsrManage, &LoginRequest, byGrpId);
				wErrCode = g_cVCSUsrManage.GetLastError();
			}

		    if (!dwValid)
		    {
			    //�ܾ�
			    cServMsg.SetErrorCode( wErrCode );
			    SendMsgToMcs( pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), FALSE );		
			    
                // �ȴ�Osp������Ϣ��������Ϣ���Ͳ���
                OspDelay(100);

			    //�Ͽ�����                
			    ::OspDisconnectTcpNode( m_dwMcsNode );
			    m_dwMcsNode = INVALID_NODE;
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcMcsMcuConnectReq]: user name/pwd invalid, NACK.\n");

			    return;
		    }

            // ��¼���û���
		    memcpy( m_achUser, tRegReq.GetUser(), MAXLEN_PWD );
		    m_achUser[ MAXLEN_PWD -1 ] = 0;

			CMcsSsn::SetUserGroup(GetInsID(), byGrpId);
            
            LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuConnectReq]: User Logged in, Name: %s, Group Id: %d.\n", 
                    tRegReq.GetUser(), 
                    byGrpId );

        
		    //ע���������ʵ��
		    ::OspNodeDiscCBRegQ( m_dwMcsNode, GetAppID(), GetInsID() );
		    
		    //֪ͨ daemon
		    cServMsg.SetMsgBody( ( u8 * )&byInstId, sizeof( byInstId ) );
		    cServMsg.CatMsgBody( ( u8 * )&tRegReq, sizeof( tRegReq ) );
		    g_cMcuVcApp.SendMsgToDaemonConf( MCU_MCSCONNECTED_NOTIF, cServMsg.GetServMsg(), 
			    cServMsg.GetServMsgLen() );

		    //Ӧ��
            cServMsg.SetSrcSsnId( ( u8 )GetInsID() );
		    cServMsg.SetMcuId( ( u8 )LOCAL_MCUID );

            TMcsRegRsp tRsp;
            tRsp.SetMcu( GetMcuPdtType(), LOCAL_MCUID );
		    
            TMtAlias tMcuAlias;
	        s8   achMcuAlias[32];
	        g_cMcuAgent.GetMcuAlias( achMcuAlias, sizeof(achMcuAlias) );
            u32 dwLocalIp = g_cMcuAgent.GetMpcIp();
		    tMcuAlias.SetH323Alias( achMcuAlias );
            tMcuAlias.m_tTransportAddr.SetNetSeqIpAddr(dwLocalIp);
            tRsp.SetMcuAlias(tMcuAlias);

            u32  dwAnotherMcuIP = 0;
            if (g_cMSSsnApp.IsDoubleLink())
            {
                dwAnotherMcuIP = g_cMSSsnApp.GetAnotherMcuIP();
	        }
            tRsp.SetRemoteMpcIp(dwAnotherMcuIP);

			BOOL32 bIsActive = FALSE;
			if(MCU_MSSTATE_ACTIVE == emLocalMSState)
			{
				bIsActive = TRUE;
			}
            tRsp.SetLocalActive(bIsActive);
            tRsp.SetUsrGrpId(byGrpId);

            u8 byOsType = 0;
        #ifdef WIN32
            byOsType = OS_TYPE_WIN32;
        #elif defined(_VXWORKS_)
            byOsType = OS_TYPE_VXWORKS;
        #else   
            byOsType = OS_TYPE_LINUX;
        #endif
            tRsp.SetMcuOsType(byOsType);
			
			tRsp.SetSysSSrc(dwSysSSrc);

		    cServMsg.SetMsgBody( (u8*)&tRsp, sizeof(tRsp) );

	    #ifdef _MINIMCU_
            TMcu8kbPfmFilter tFilter = CMcuPfmLmt::GetFilter();
			cServMsg.CatMsgBody( (u8*)&tFilter, sizeof(TMcu8kbPfmFilter) );
	    #endif
			
			//����MCU�汾����Ϣ[12/31/2011 chendaiwei]
			u8 bySupportHP = 1;
			cServMsg.CatMsgBody(&bySupportHP,sizeof(u8));

            LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[Mcsssn][ProcMcsMcuConnectReq]: Mcs Connect Ack: \n");
            if (g_bPrintMcsMsg)
            {                
                tRsp.Print();
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MCS, "[Mcsssn] support HP: %d\n",bySupportHP);
            }

            //[4/8/2013 liaokang] ֧�ֶ������  MCU��UTF-8��ʽ����
#ifdef _UTF8
            u8 byCodingForm = emenCoding_Utf8;
            cServMsg.CatMsgBody(&byCodingForm,sizeof(u8));
#endif

		    SendMsgToMcs( pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), FALSE );
                           
		    NEXTSTATE( STATE_NORMAL );
        }
		break;

	default:
		LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcsMcuConnectReq]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
		break;
	}
}


/*=============================================================================
�� �� ���� ProcMcsMcuGetMsStatusReq
��    �ܣ� ��ȡmcu����״̬
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/2/22  4.0			������                  ����
=============================================================================*/
void CMcsSsnInst::ProcMcsMcuGetMsStatusReq( const CMessage * pcMsg )
{
    // guzh [4/11/2007] ���û��ͨ��ע��ͷ��ͻ�ȡ���󣬺��п�������������ǰ�����ӣ������
    if ( CurState() == STATE_IDLE )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcMcsMcuGetMsStatusReq]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
        
        return;
    }
    
    if (MCU_MSSTATE_EXCHANGE_NTF == pcMsg->event)
    {
        // ���ͬ��ʧ��,�Ͽ���Ӧ������
        u8 byIsSwitchOk = *pcMsg->content;
        if (0 == byIsSwitchOk)
        {
            OspDisconnectTcpNode( m_dwMcsNode );
            return;
        }        
    }
    
    if( MCU_MSSTATE_ACTIVE == g_cMSSsnApp.GetCurMSState() )
    {
        TMcuMsStatus tMsStatus;
        tMsStatus.SetMsSwitchOK(g_cMSSsnApp.IsMsSwitchOK());
    
        CServMsg cServMsg(pcMsg->content, pcMsg->length);
        cServMsg.SetMsgBody((u8*)&tMsStatus, sizeof(tMsStatus));
        SendMsgToMcs(MCU_MCS_GETMSSTATUS_ACK, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

        LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuGetMsStatusReq] IsMsSwitchOK :%d.\n", tMsStatus.IsMsSwitchOK());
    }
        
    return;
}

/*====================================================================
    ������      ��ProcMcsInquiryMcuTime
    ����        ���������̨��ѯmcu��ǰʱ�䴦����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/02/08    1.0         ���ؾ�         ����
====================================================================*/
void CMcsSsnInst::ProcMcsInquiryMcuTime( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TKdvTime tCurMcuTime;
	
	LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[Mcsssn]: Message %u(%s) received, Current State %d!\n",
			                pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());

	switch( CurState() )
	{
	case STATE_NORMAL ://���ӽ���
		time_t tMcuTime;
		::time( &tMcuTime );
		tCurMcuTime.SetTime( &tMcuTime ); 

		cServMsg.SetSrcSsnId( ( u8 )GetInsID() );
		cServMsg.SetMsgBody( (u8 *)&tCurMcuTime, sizeof( tCurMcuTime ) );
		SendMsgToMcs( pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		break;
	default :
		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcMcsInquiryMcuTime]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
		break;
	}

	return;
}

/*=============================================================================
  �� �� ���� ProcMcsMcuChgSysTime
  ��    �ܣ� ����̨�޸�mcuϵͳʱ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMcsSsnInst::ProcMcsMcuChgSysTime( const CMessage * pcMsg)
{
    CServMsg cMsg(pcMsg->content, pcMsg->length);
    if (sizeof(TKdvTime) != cMsg.GetMsgBodyLen())
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcMcsMcuChgSysTime] msg body is invalid! len: %d\n", cMsg.GetMsgBodyLen());
        return;
    }
    
    // xsl [10/25/2006] ��license�����˽�ֹ���ڣ�������ʱ��ͬ��
    if (g_cMcuVcApp.GetMcuExpireDate().GetYear() != 0)
    {
        cMsg.SetMsgBody(NULL, 0);
		cMsg.SetSrcSsnId( ( u8 )GetInsID() );
        cMsg.SetErrorCode(ERR_MCU_OPERATION_NOPERMIT);
		SendMsgToMcs(pcMsg->event + 2, cMsg.GetServMsg(), cMsg.GetServMsgLen());
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "can't syn time for mcu expire date.\n");
        return;
    }

    TKdvTime *ptTime = (TKdvTime *)cMsg.GetMsgBody();
    time_t tTime;
    ptTime->GetTime(tTime);
    u16 wRet = g_cMcuAgent.SetSystemTime(tTime);
    if (SUCCESS_AGENT != wRet)
    {	
		cMsg.SetMsgBody(NULL, 0);
		cMsg.SetSrcSsnId( ( u8 )GetInsID() );
		SendMsgToMcs(pcMsg->event + 2, cMsg.GetServMsg(), cMsg.GetServMsgLen());
		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcMcsMcuChgSysTime] change system time failed, ret: %d\n", wRet);        
    }
    else
    {		
		TKdvTime tCurMcuTime;
		time_t tMcuTime;
		::time( &tMcuTime );
		tCurMcuTime.SetTime( &tMcuTime );         
    
		cMsg.SetSrcSsnId( ( u8 )GetInsID() );
		cMsg.SetMsgBody( (u8 *)&tCurMcuTime, sizeof( tCurMcuTime ) );
		SendMsgToMcs( pcMsg->event + 1, cMsg.GetServMsg(), cMsg.GetServMsgLen() );
    }

    return;
}

/*====================================================================
    ������      ��ProcMcsDisconnect
    ����        ������̨����֪ͨ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/26    1.0         LI Yi         ����
====================================================================*/
void CMcsSsnInst::ProcMcsDisconnect( const CMessage * pcMsg )
{
    if (STATE_NORMAL != CurState())
    {
        return;
    }

    //LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[Mcsssn%u]: Message %u(%s) received!\n", GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event));

	u8 byInstId = ( u8 )GetInsID();
    
	CServMsg cServMsg;
	if ( *(u32*)pcMsg->content == m_dwMcsNode )	//��ʵ����Ӧ���Ӷ�
	{		
        if (INVALID_NODE != m_dwMcsNode)
        {
            OspDisconnectTcpNode(m_dwMcsNode);
        }
        
		//send notification to VC Daemon
		cServMsg.SetMsgBody( ( u8 * )&byInstId, sizeof( byInstId ) );
        cServMsg.SetEventId( MCU_MCSDISCONNECTED_NOTIF );
		g_cMcuVcApp.SendMsgToDaemonConf( cServMsg.GetEventId(), 
                                         cServMsg.GetServMsg(), 
				                         cServMsg.GetServMsgLen() );
		//clear
		m_dwMcsNode = INVALID_NODE;

        // ����û�����Ϣ
        CMcsSsn::SetUserGroup( GetInsID(), USRGRPID_INVALID );
		cServMsg.SetSrcSsnId(byInstId);
		post( MAKEIID( AID_MCU_CONFIG, 1 ), MCSSSN_MCUCFG_MCSDISSCONNNECT_NOTIFY, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

		NEXTSTATE( STATE_IDLE );
	}	

	return;
}

/*====================================================================
    ������      ��SendMsgToMc
    ����        ������Ϣ����ʵ����Ӧ�Ļ������̨
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wEvent, �¼���
				  u8 * const pbyMsg, ���͵���Ϣָ�룬ȱʡΪNULL
				  u16 wLen, ��Ϣ���ȣ�ȱʡΪ0
    ����ֵ˵��  ���ɹ�����TRUE����ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/28    1.0         LI Yi         ����
====================================================================*/
BOOL32 CMcsSsnInst::SendMsgToMcs( u16 wEvent, u8 * const pbyMsg, u16 wLen, BOOL32 bMsFilter )
{
    if (bMsFilter && !g_cMSSsnApp.JudgeSndMsgPass())
    {
        return TRUE;
    }

//    CServMsg cMsg(pbyMsg, wLen);
//    LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[SendMsgToMcs] confid :");
//    cMsg.GetConfId().Print();
    LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[SendMsgToMcs] Event %u(%s) To Console %u\n", wEvent, OspEventDesc(wEvent), GetInsID() );
    
	if (OspIsValidTcpNode(m_dwMcsNode))
	{
		post( m_dwMcsIId, wEvent, pbyMsg, wLen, m_dwMcsNode );
		return( TRUE );
	}
	else
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "CMcsSsnInst: ABNORMAL McSsn: Node=%u(IP: 0x%x), CurState=%u, InstId=%d\n", 
			                     m_dwMcsNode, OspNodeIpGet(m_dwMcsNode), CurState(), GetInsID() );
		return( FALSE );
	}
}

/*====================================================================
    ������      ��ProcMcsMcuMsg
    ����        ���������̨������Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/03/21    1.0         Qzj           ����
====================================================================*/
void CMcsSsnInst::ProcMcsMcuMsg(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    switch (CurState())
    {
    case STATE_NORMAL:
        cServMsg.SetMcuId(LOCAL_MCUID);
        cServMsg.SetSrcSsnId((u8)GetInsID());
        cServMsg.SetSrcMtId(0);
        cServMsg.SetEventId(pcMsg->event);
        
        g_cMcuVcApp.SendMsgToConf(cServMsg.GetConfId(), pcMsg->event,
                                  cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        break;

    default:
		LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcsMcuMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
        break;
    }
}


/*====================================================================
    ������      ��ProcMcuMcsResponse
    ����        �����л������̨����Ӧ����Ϣͳһ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/01    1.0         LI Yi         ����
====================================================================*/
void CMcsSsnInst::ProcMcuMcsMsg( const CMessage * pcMsg, BOOL32 bCheckGrp )
{
	CServMsg cServMsg(pcMsg->content,pcMsg->length);

    BOOL32 bSend = TRUE;
    u8 byGrpId = CMcsSsn::GetUserGroup( GetInsID() );

	switch( CurState() )
	{
	case STATE_NORMAL:
        
        if ( bCheckGrp )
        {
            // Ҫ�����Ȩ�ޣ�������Լ�������ǳ�������Ա�����߷�����Ϣ��ͨ��            
            if ( byGrpId == USRGRPID_SADMIN || 
                 byGrpId == cServMsg.GetConfId().GetUsrGrpId() ||
                 cServMsg.GetConfId().IsNull() ||
                 cServMsg.GetConfId().GetUsrGrpId() == 0 )
            {
                bSend = TRUE;
            }
            else
            {
                bSend = FALSE;
            }
        }
        if (bSend)
        {
            SendMsgToMcs( pcMsg->event, pcMsg->content, pcMsg->length );
        }
        else
        {
            LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcMcuMcsMsg]: Message %u(%s) In InsID.%u is Dropped for CConf Group Id(%d) <--> Ssn Group Id(%d)\n",
                    pcMsg->event, ::OspEventDesc( pcMsg->event ), GetInsID(), 
                    cServMsg.GetConfId().GetUsrGrpId(), byGrpId);
        }
		
		break;
	default:
		LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcuMcsMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
		break;
	}
}

/*====================================================================
    ������      ��ProcMcsMcuDaemonConfMsg
    ����        ������̨�����ĸ�ҵ��Daemonʵ����Ϣ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/26    1.0         LI Yi         ����
====================================================================*/
void CMcsSsnInst::ProcMcsMcuDaemonConfMsg( const CMessage * pcMsg )
{
	CServMsg	cServMsg(pcMsg->content,pcMsg->length);
    
    // xsl [11/28/2006] n+1����ģʽ��������ģ��
    if (pcMsg->event == MCS_MCU_CREATECONF_BYTEMPLATE_REQ ||
        pcMsg->event == MCS_MCU_CREATESCHCONF_BYTEMPLATE_REQ ||
        pcMsg->event == MCS_MCU_CREATETEMPLATE_REQ ||
        pcMsg->event == MCS_MCU_SAVECONFTOTEMPLATE_REQ)
    {
        if (g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_IDLE ||
            g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_SWITCH)
        {
            LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcMcsMcuDaemonConfMsg] mcs can't create template in n+1 mode!\n");
            cServMsg.SetErrorCode(ERR_MCU_NPLUS_CREATETEMPLATE);
            SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
            return;
        }
    }    

	switch( CurState() )
	{
	case STATE_NORMAL:
	    cServMsg.SetMcuId( LOCAL_MCUID );
		cServMsg.SetSrcSsnId( ( u8 )GetInsID() );
		cServMsg.SetSrcMtId( 0 );
		cServMsg.SetEventId( pcMsg->event );
		if (pcMsg->event == MCS_MCU_LISTALLRECORD_REQ && !cServMsg.GetConfId().IsNull())
		{
			g_cMcuVcApp.SendMsgToConf(cServMsg.GetConfId(), pcMsg->event,
				cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		}
		else
		{
			g_cMcuVcApp.SendMsgToDaemonConf( cServMsg.GetEventId(), cServMsg.GetServMsg(), 
			                             cServMsg.GetServMsgLen() );
		}
		break;
	default:
		LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcsMcuDaemonConfMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
		break;
	}
}

/*====================================================================
    ������      ��ProcReoOverFlow
    ����        ������ʵ��������ʧ�ܴ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/01    1.0         LI Yi         ����
====================================================================*/
void CMcsSsnInst::ProcReoOverFlow( const CMessage * pcMsg )
{
	CServMsg cServMsg;

	LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[Mcsssn]: Message %u(%s) received, Conference instance full!\n",
			                    pcMsg->event, ::OspEventDesc(pcMsg->event));

	switch( CurState() )
	{
	case STATE_NORMAL:
		cServMsg.SetServMsg( pcMsg->content, pcMsg->length );
		cServMsg.SetErrorCode( ERR_MCU_CONFOVERFLOW );
		SendMsgToMcs( cServMsg.GetEventId() + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), FALSE );
		break;
	default:
		LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcReoOverFlow]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
		break;
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
    03/05/29    1.0         
====================================================================*/
void CMcsSsnInst::DaemonProcAppTaskRequest( const CMessage * pcMsg )
{
	post( pcMsg->srcid, MCU_APPTASKTEST_ACK, pcMsg->content, pcMsg->length );
}

/*====================================================================
    ������      ��DaemonProcPowerOn
    ����        �������ϵ����������
    �㷨ʵ��    �����ص�ַ�����û�����ģ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/05/29    1.0         
====================================================================*/
void CMcsSsnInst::DaemonProcPowerOn( void )
{
	//������ַ������  
// 	g_pcAddrBook = new CAddrBook(MAX_ADDRENTRY, MAX_ADDRGROUP);
// 	if(NULL == g_pcAddrBook)
// 	{
// 		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "MCU: memory allocation for address book failed!\n");
// 	}
// 	else
// 	{
// 		if(!g_pcAddrBook->LoadAddrBook(GetAddrBookPath()))
// 		{
// 			printf("MCU: LoadAddrBook() from %s failed!\n", GetAddrBookPath());
// 		}
// 	}

	//�û��б�ģ�����´��ļ�װ���û��б�
	g_cMcuVcApp.ReloadMcuUserList();

	// ��ʼ����ͨʵ����״̬
	for (u16 wInsID = 1; wInsID <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); wInsID++)
	{
		post( MAKEIID(GetAppID(), wInsID), OSP_POWERON );
	}

    
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMcsSsn::CMcsSsn() 
{
    memset( m_abyUsrGrp, USRGRPID_INVALID, sizeof(m_abyUsrGrp));
	m_byCurrendDebugMode = (u8)emDebugModeNone;
	m_byCurrendDebugInsId = 0;
}

CMcsSsn::~CMcsSsn()
{

}

/*====================================================================
    ������      ��BroadcastToAllMcsSsn
    ����        ������Ϣ�������Ѿ����ӵĻ������̨��Ӧ�ĻỰʵ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wEvent, �¼���
				  u8 * const pbyMsg, ���͵���Ϣָ�룬ȱʡΪNULL
				  u16 wLen, ��Ϣ���ȣ�ȱʡΪ0
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/30    1.0         LI Yi         ����
	08/11/21                ���㻪        ���ڹ㲥����Ϣ�������������ͨ�ŵ�APP
====================================================================*/
void CMcsSsn::BroadcastToAllMcsSsn( u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	//�����������������ⲿ��AppͶ����Ϣ��������
	if (FALSE == g_cMSSsnApp.JudgeSndMsgPass())
	{
		return;
	}
	
	// CInstance::EACH ���͸����еķǿ���ʵ��
	::OspPost(MAKEIID( AID_MCU_MCSSN, CInstance::EACH ), wEvent, pbyMsg, wLen);
	::OspPost(MAKEIID( AID_MCU_VCSSN, CInstance::EACH ), wEvent, pbyMsg, wLen);
/* // for test [pengguofeng 7/8/2013]
	CServMsg cMsg(pbyMsg, wLen);
	TMtExtU atMtExtU[200];
	memset(atMtExtU, 0, sizeof(atMtExtU));
	
	u8 byMtNum = 0; 
	TMcu tMcu;
	tMcu.SetNull();

	if ( wEvent == MCU_MCS_GETMTLIST_NOTIF 
		)
	{
		
		{
			if ( HandleMtListNtfUtf8(cMsg, atMtExtU, byMtNum, tMcu) )
				StaticLog("�����ɹ�\n");
			else
				StaticLog("����ʧ��\n");
		}
	}

	if ( wEvent == MCU_MCS_MTLIST_NOTIF )
	{
		HandleMtListNtf(cMsg, atMtExtU, byMtNum, tMcu);
	}
*/
	return;
}

/*====================================================================
    ������      ��SendMsgToMcsSsn
    ����        ������Ϣ��ָ���Ļ������̨��Ӧ�ĻỰʵ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wInstId, ��Ӧ��ʵ����
				  u16 wEvent, �¼���
				  u8 * const pbyMsg, ���͵���Ϣָ�룬ȱʡΪNULL
				  u16 wLen, ��Ϣ���ȣ�ȱʡΪ0
    ����ֵ˵��  ���ɹ�����TRUE�������¼��ն�δ�ǼǷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/28    1.0         LI Yi         ����
	08/11/21                ���㻪        ͨ��ʵ���ŷ�Χ�������಻ͬ�ĸ������ͨ��
	                                      ��APP(g_cMcsSsnApp, g_cVcsSsnApp)
====================================================================*/
BOOL32 CMcsSsn::SendMsgToMcsSsn( u16 wInstId, u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	// g_cMcsSsnApp ��1��16��ʵ��ʵ����Ч��g_cVcsSsnApp ��16��32��ʵ��ʵ����Ч
	if (wInstId > 0 && wInstId <= MAXNUM_MCU_MC)
	{
		::OspPost(MAKEIID( AID_MCU_MCSSN, wInstId ), wEvent, pbyMsg, wLen);
	}
	else if (wInstId > MAXNUM_MCU_MC && wInstId <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC))
	{
		::OspPost(MAKEIID( AID_MCU_VCSSN, wInstId), wEvent, pbyMsg, wLen);
	}
	else if (CInstance::DAEMON == wInstId)
	{
		::OspPost(MAKEIID( AID_MCU_VCSSN, wInstId), wEvent, pbyMsg, wLen);
	}
	else
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "CMcsSsn: wrong instance id %u!\n", wInstId);
		return FALSE;
	}

	return TRUE;
}

/*====================================================================
    ������      ��Broadcast2SpecGrpMcsSsn
    ����        ������Ϣ��ͬ�û���Ļ������̨��Ӧ�ĻỰʵ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wEvent, �¼���
				  u8 * const pbyMsg, ���͵���Ϣָ�룬ȱʡΪNULL
				  u16 wLen, ��Ϣ���ȣ�ȱʡΪ0
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/30    1.0         LI Yi         ����
====================================================================*/
void CMcsSsn::Broadcast2SpecGrpMcsSsn( u8 byGrdId, u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	//�����������������ⲿ��AppͶ����Ϣ��������
	if (FALSE == g_cMSSsnApp.JudgeSndMsgPass())
	{
		return;
	}

    u8 bySsnGrpId = 0;
	for (u8 byInstID = 1; byInstID <= MAXNUM_MCU_MC; byInstID++)
	{
        bySsnGrpId = CMcsSsn::GetUserGroup( byInstID );
        if ( bySsnGrpId != USRGRPID_INVALID )
        {
            if ( bySsnGrpId == byGrdId ||
                 bySsnGrpId == USRGRPID_SADMIN )
            {
                // ������ض�����߳����û���
                ::OspPost(MAKEIID( AID_MCU_MCSSN, byInstID ), wEvent, pbyMsg, wLen);
            }
            else
            {
            
                LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[Broadcast2SpecGrpMcsSsn]: Message %u(%s) In InsID.%u is Dropped for Spec Group Id(%d) <--> Ssn Group Id(%d)\n",
                        wEvent, ::OspEventDesc( wEvent ), byInstID, 
                        byGrdId, bySsnGrpId);
            }
        }
	}

	return;
}

/*====================================================================
    ������      ��SetCurrentDebugMode
    ����        �����õ�ǰ����ģʽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����emDebugMode emMode
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2013/10/18  4.7.3       �ܾ���         ����
====================================================================*/
void CMcsSsn::SetCurrentDebugMode( emDebugMode emMode )
{
	m_byCurrendDebugMode = (u8)emMode;
}

/*====================================================================
    ������      ��GetCurrentDebugMode
    ����        ����ȡ��ǰ����ģʽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����void
    ����ֵ˵��  ��emDebugMode
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2013/10/18  4.7.3       �ܾ���         ����
====================================================================*/
emDebugMode CMcsSsn::GetCurrentDebugMode( void )
{
	return (emDebugMode)m_byCurrendDebugMode;
}

/*====================================================================
    ������      ��SetCurrentDebugModeInsId
    ����        �����ÿ�������ģʽ��mcs
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byInsId 
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2013/10/18  4.7.3       �ܾ���         ����
====================================================================*/
void CMcsSsn::SetCurrentDebugModeInsId( u8 byInsId )
{
	m_byCurrendDebugInsId = byInsId;
}

/*====================================================================
    ������      ��GetCurrentDebugModeInsId
    ����        ����ȡ��������ģʽmcsʵ��id
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����void
    ����ֵ˵��  ��u8
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2013/10/18  4.7.3       �ܾ���         ����
====================================================================*/
u8 CMcsSsn::GetCurrentDebugModeInsId( void )
{
	return m_byCurrendDebugInsId;
}

// ����McSsn���û���
void CMcsSsn::SetUserGroup( u16 byInsId, u8 byGrpId ) 
{
    if ( byInsId < 1 || byInsId > MAXNUM_MCU_MC + MAXNUM_MCU_VC )
        return;

    m_abyUsrGrp[byInsId - 1] = byGrpId;
}

// ��ȡMcSsn���û���
u8 CMcsSsn::GetUserGroup( u16 byInsId )
{
    if ( byInsId < 1 || byInsId > MAXNUM_MCU_MC + MAXNUM_MCU_VC )
    {
        return USRGRPID_INVALID;
    }
    else
    {
        return m_abyUsrGrp[byInsId - 1];
    }
}

// �����½
BOOL32 CMcsSsn::CheckLoginValid(CUsrManage& cUsrManageObj, CLoginRequest *pcUsr, u8 &byUserRole)
{
    //��ΪN+1���ݹ���ģʽ�����л��󣩣��ֶ�����½
    //��������û�����ģ��
    BOOL32 bValid = FALSE;
    byUserRole = USRGRPID_INVALID;
    CNPlusInst *pcInst = NULL;
    u8 byMaxUsrNum = 0;
    u8 byUsrNum = 0;

    if (MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
    {
        u8 byInsId = g_cNPlusApp.GetMcuSwitchedInsId();
        if (0 == byInsId || byInsId > MAXNUM_NPLUS_MCU)
        {
            LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[GetUsrGrpUserCount] McuSwitchedInsId is invalid in NPlus switched mode.\n");
            return FALSE;
        }
        else
        {
            CApp *pcApp = &g_cNPlusApp;
            pcInst = (CNPlusInst *)pcApp->GetInstance(byInsId);
            if (NULL != pcInst)
            {
                // ����֧�ֱ���admin��½
                if ( strncmp( pcUsr->GetName(), "admin" , MAXLEN_PWD) == 0 )
                {
                    bValid = g_cUsrManage.CheckLoginValid(pcUsr);
                    if (bValid)
                    {
                        byUserRole = USRGRPID_SADMIN;
                        return TRUE;
                    }
                }

                // ����֧��N�������û��ĵ�½����
                pcInst->GetGrpUsrCount(USRGRPID_SADMIN, byMaxUsrNum, byUsrNum);
                CExUsrInfo *pcUsersInfo = pcInst->GetUserPtr();

                for (u8 byLoop = 0; byLoop < byUsrNum; byLoop ++)
                {
                    if ( pcUsersInfo[byLoop].IsEqualName(  pcUsr->GetName() ) &&
                         pcUsersInfo[byLoop].IsPassWordEqual( pcUsr->GetPassword() ) )
                    {
                        byUserRole = pcUsersInfo[byLoop].GetUsrGrpId();
                        return TRUE;
                    }
                }
                

            }
            return FALSE;
        }
    }


    // ��ͨ�������N+1
    bValid = cUsrManageObj.CheckLoginValid(pcUsr);
    if (!bValid)
    {
        return FALSE;
    }

    // ���Ҳ���¼�±��û�������Ϣ    
    if ( strncmp( pcUsr->GetName(), "admin", MAXLEN_PWD ) == 0 )
    {
        byUserRole = USRGRPID_SADMIN;
    }
    else
    {
        s32 nUsrNum = cUsrManageObj.GetUserNum();
        CExUsrInfo cUsrInfo;
        for (s32 nUsrLoop = 0; nUsrLoop < nUsrNum; nUsrLoop ++)
        {
            cUsrManageObj.GetUserFullInfo( &cUsrInfo, nUsrLoop );
            if ( cUsrInfo.IsEqualName( pcUsr->GetName() ) )
            {
                byUserRole = cUsrInfo.GetUsrGrpId();
                break;
            }
        }
    }
    return TRUE;
}


/*=============================================================================
  �� �� ���� ProcMcuCfgMsg
  ��    �ܣ� mcu������Ϣ�ַ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/9/15    4.0			������                  ����
=============================================================================*/
void CMcsSsnInst::ProcMcuCfgMsg( const CMessage *const pcMsg)
{
	CServMsg cMsg(pcMsg->content, pcMsg->length);
	cMsg.SetSrcSsnId( ( u8 )GetInsID() );
	cMsg.SetEventId(pcMsg->event);

	//�������߻��߱�����δ��ȫͬ������(MTADP/MP/MC/���� ������Ϣ����)��
	if (FALSE == g_cMSSsnApp.JudgeRcvMsgPass())
	{
		return;
	}

	switch (CurState())
	{
	case STATE_NORMAL:
        // guzh [7/17/2007] Ȩ�޿���
        switch (pcMsg->event)
        {
        case MCS_MCU_CHANGEMCUEQPCFG_REQ:       //MCS�����޸�mcu�豸������Ϣ     
        case MCS_MCU_CHANGEBRDCFG_REQ:          //mcs�޸ĵ���������Ϣ        
        case MCS_MCU_CHANGEMCUGENERALCFG_REQ:   //mcs�޸�mcu������Ϣ��������        
        case MCS_MCU_REBOOT_CMD:
        case MCS_MCU_REBOOTBRD_REQ:
            if (CMcsSsn::GetUserGroup(GetInsID()) != USRGRPID_SADMIN)
            {
                u16 wNackEvent = pcMsg->event + 2;
                if (MCS_MCU_REBOOT_CMD == pcMsg->event)
                {
                    wNackEvent = MCU_MCS_ALARMINFO_NOTIF;
                }
                // NACK��û��Ȩ��
                cMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
                cMsg.SetMsgBody();
                SendMsgToMcs(wNackEvent, cMsg.GetServMsg(), cMsg.GetServMsgLen() );
                return;
            }
            break;
        default:
            break;
        }
        if (MCS_MCU_REBOOT_CMD == pcMsg->event)
        {
            McsMcuRebootCmd(pcMsg);
        }
        else
        {
            ::OspPost( MAKEIID( AID_MCU_CONFIG, 1 ), cMsg.GetEventId(), cMsg.GetServMsg(), cMsg.GetServMsgLen() );
        }		
		break;
	default:
		LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[CMcsSsnInst]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode );
		break;
	}   

	return;
}

/*=============================================================================
  �� �� ���� ProcMcuCfgRsp
  ��    �ܣ� mcu������Ϣ��Ӧ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/9/15    4.0			������                  ����
=============================================================================*/
void CMcsSsnInst::ProcMcuCfgRsp(const CMessage * pcMsg)
{
    BOOL32 bUseMsgFilter = TRUE;
    switch( CurState() )
    {
    case STATE_NORMAL:
        // ���������ɹ��Ļ�Ӧ�������ϱ����
        if ( pcMsg->event == MCU_MCS_UPDATEBRDVERSION_NOTIF ||
             pcMsg->event == MCU_MCS_BOARDSTATUS_NOTIFY )
        {
            // ������Ϣ��Զͨ��
            bUseMsgFilter = FALSE;
        }
        SendMsgToMcs(pcMsg->event, pcMsg->content, pcMsg->length, bUseMsgFilter);
        break;
    default:
		LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcuCfgRsp]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
        break;
    }   

    return;
}

/*=============================================================================
  �� �� ���� ProcMcuCfgFileMsg
  ��    �ܣ� mcu�����ļ���������Ϣ��Ӧ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/4/4     4.0			����                ����
=============================================================================*/
void CMcsSsnInst::ProcMcuCfgFileMsg(const CMessage * pcMsg)
{
	CServMsg cMsg(pcMsg->content, pcMsg->length);

	//�������߻��߱�����δ��ȫͬ������(MTADP/MP/MC/���� ������Ϣ����)��
	if (FALSE == g_cMSSsnApp.JudgeRcvMsgPass())
	{
		return;
	}

    u8  abyCfgContent[MAXLEN_MCUCFG_INI];
    u32 dwCfgFileLen = MAXLEN_MCUCFG_INI;
    
    switch( CurState() )
    {
    case STATE_NORMAL:
        if (CMcsSsn::GetUserGroup(GetInsID()) != USRGRPID_SADMIN)
        {
            // NACK��û��Ȩ��
            cMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
            cMsg.SetMsgBody();
            SendMsgToMcs(pcMsg->event+2, cMsg.GetServMsg(), cMsg.GetServMsgLen() );
            return;
        }
        // ��������
        if (MCU_MCS_DOWNLOADCFG_REQ == pcMsg->event)
        {
            BOOL32 bRet = g_cMcuVcApp.GetCfgFileData(abyCfgContent, 
                                                   dwCfgFileLen, 
                                                   dwCfgFileLen,
                                                   TRUE);
            if (!bRet)
            {
                // ��ȡʧ�ܣ�nack
                cMsg.SetErrorCode(ERR_MCU_CFG_FILEERR);
                SendMsgToMcs(pcMsg->event + 2,
                             cMsg.GetServMsg(), 
                             cMsg.GetServMsgLen());
            }
            else
            {
                u32 dwNetFileLen = htonl(dwCfgFileLen);
                cMsg.SetMsgBody((u8*)(&dwNetFileLen), sizeof(u32));
                cMsg.CatMsgBody(abyCfgContent, (u16)dwCfgFileLen);

                SendMsgToMcs(pcMsg->event + 1,
                             cMsg.GetServMsg(), 
                             cMsg.GetServMsgLen());

            }            
        }
        // �ϴ�����
        else
        {
            dwCfgFileLen = ntohl(*(u32*)cMsg.GetMsgBody());
            dwCfgFileLen = (dwCfgFileLen > MAXLEN_MCUCFG_INI) ?
                            MAXLEN_MCUCFG_INI :
                            dwCfgFileLen;
            memcpy( abyCfgContent, 
                    (u8*)(cMsg.GetMsgBody()+sizeof(u32)), 
                    dwCfgFileLen );

            BOOL32 bRet = g_cMcuVcApp.SetCfgFileData( abyCfgContent, 
                                                    dwCfgFileLen, 
                                                    TRUE);
            cMsg.SetMsgBody(NULL);
            if (!bRet)
            {
                // д�ļ�ʧ��
                cMsg.SetErrorCode(ERR_MCU_CFG_FILEERR);
                SendMsgToMcs(pcMsg->event + 2,
                             cMsg.GetServMsg(), 
                             cMsg.GetServMsgLen());
            }
            else
            {
			#if defined(_8KH_) || defined(_8KE_) || defined(_8KI_)
				
				TGKProxyCfgInfo tGkPxyCfgInfo;
				g_cMcuAgent.GetGkProxyCfgInfoFromCfgFile(tGkPxyCfgInfo);
				g_cMcuAgent.SetGkProxyCfgInfo(tGkPxyCfgInfo);

				//[֧�������ļ����뵼��]��mcucfg.ini�е�DMZ��Ϣд��pxysrvcfg.ini[5/14/2013 chendaiwei]
				TProxyDMZInfo tDMzInfo;
				g_cMcuAgent.GetProxyDMZInfoFromMcuCfgFile(tDMzInfo);
				g_cMcuAgent.SetProxyDMZInfo(tDMzInfo);

				//[֧�������ļ����뵼��]��mcucfg.ini�еĶ���Ӫ����Ϣд��multimanucfg.ini[5/14/2013 chendaiwei]
				TMultiManuNetAccess tMultiNetAccess;
				g_cMcuAgent.GetMultiManuNetAccess(tMultiNetAccess,TRUE);
				g_cMcuAgent.WriteMultiManuNetAccess(tMultiNetAccess);

				TMultiManuNetCfg tMultNetCfg;
				memcpy(&tMultNetCfg.m_tMultiManuNetAccess,&tMultiNetAccess,sizeof(tMultiNetAccess));
				tMultNetCfg.SetGkIp(tGkPxyCfgInfo.GetGkIpAddr());
				tMultNetCfg.m_byEnableMultiManuNet = (u8)tGkPxyCfgInfo.IsProxyUsed();
				g_cMcuAgent.SetMultiManuNetCfg(tMultNetCfg);

			#endif

                SendMsgToMcs(pcMsg->event + 1,
                             cMsg.GetServMsg(), 
                             cMsg.GetServMsgLen());
            }
        }

        break;
    default:
		LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcuCfgFileMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
        break;
    }   

    return;
}


/*=============================================================================
  �� �� ���� ProcMcsNPlusReq
  ��    �ܣ� mcu N-Plus��Ϣ��Ӧ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/12/15    4.0			����                  ����
=============================================================================*/
void CMcsSsnInst::ProcMcsNPlusReq(const CMessage * pcMsg)
{
    CServMsg cMsg(pcMsg->content, pcMsg->length);

    switch( CurState() )
    {
    case STATE_NORMAL:
        if (CMcsSsn::GetUserGroup(GetInsID()) != USRGRPID_SADMIN)
        {
            // NACK��û��Ȩ��
            cMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
            SendMsgToMcs(pcMsg->event + 2,
                         cMsg.GetServMsg(), 
                         cMsg.GetServMsgLen() );
            return;
        }
        OspPost(MAKEIID(AID_MCU_NPLUSMANAGER, CInstance::DAEMON), pcMsg->event, pcMsg->content, pcMsg->length);        

        break;
    default:
		LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcuCfgRsp]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
        break;
    }   

    return;
}

/*=============================================================================
  �� �� ���� ProcNPlusMcsRsp
  ��    �ܣ� mcu N-Plus��Ϣ��Ӧ�ظ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/12/15    4.0			����                  ����
=============================================================================*/
void CMcsSsnInst::ProcNPlusMcsRsp(const CMessage * pcMsg)
{
    switch( CurState() )
    {
    case STATE_NORMAL:
        SendMsgToMcs(pcMsg->event, pcMsg->content, pcMsg->length);
        break;

    default:
		LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcuCfgRsp]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
        break;
    }
    return;
}



/*=============================================================================
�� �� ���� ProcMcuUnitTestMsg
��    �ܣ� ����MCU��Ԫ������Ϣ����Ӧ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/2/15    4.0			�ű���                  ����
=============================================================================*/
void CMcsSsnInst::ProcMcuUnitTestMsg( const CMessage * pcMsg )
{
	switch( CurState() )
	{
	case STATE_NORMAL:
		SendMsgToMcs(pcMsg->event, pcMsg->content, pcMsg->length, FALSE);
		break;
	default:
		LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcuUnitTestMsg]: Wrong inst state in state.%u InsID.%u srcnode.%u!\n", 
			CurState(), GetInsID(), pcMsg->srcnode);
		break;
	}
	return;
}

/*=============================================================================
  �� �� ���� McsMcuRebootCmd
  ��    �ܣ� ���Զ������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
#ifdef WIN32
extern HWND g_hwndMain;
#endif

void CMcsSsnInst::McsMcuRebootCmd( const CMessage * const pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u32 dwMpcIp = *(u32*)cServMsg.GetMsgBody();

#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
    LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[McsMcuRebootCmd] User Request Reboot MPC IP: 0x%x, cur mpcip:0x%x\n", 
              ntohl(dwMpcIp), ntohl(g_cMcuAgent.GetMpcIp()));
    if(0 != dwMpcIp && dwMpcIp != g_cMcuAgent.GetMpcIp())
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[McsMcuRebootCmd] failed due to mpcip.0x%x, cur mpcip.0x%x\n",
                                 dwMpcIp, g_cMcuAgent.GetMpcIp());
        return;
    }

#endif
	
	// [6/29/2011 liuxu] �˳�ǰ��flush
	mculogflush();
	OspDelay(20);
	
//[2011/02/11 zhushz] mcs�޸�mcu ip �ŵ�rebootʱ�޸�,��ֹ��������
#ifndef _MINIMCU_
	if (g_cMcuAgent.IsMpcNetCfgBeModifedByMcs())
	{
  /*#if defined (_8KI_)
		g_cCfgParse.Make8KINetCfgEffect();
  #else*/
		//ȡ���µ���������
		TMcuNewNetCfg tMcuNewNetCfg;
		g_cMcuAgent.GetNewNetCfg(tMcuNewNetCfg);
		u32 dwMpcNewIp = tMcuNewNetCfg.GetNewMpcIP();
 		u8  byMpcNewInterface = tMcuNewNetCfg.GetNewMpcInterface();
		u32 dwMpcNewMaskIp = tMcuNewNetCfg.GetNewMpcMaskIp();
		u32 wMpcNewGateWay = tMcuNewNetCfg.GetNewMpcDefGateWay();

		//�޸���������
		g_cMcuAgent.SetMpcIp(dwMpcNewIp, byMpcNewInterface);
		g_cMcuAgent.SetInterface(byMpcNewInterface);
		g_cMcuAgent.SetMaskIp(dwMpcNewMaskIp, byMpcNewInterface);
		g_cMcuAgent.SetGateway(wMpcNewGateWay);
  //#endif

		//�ָ����������޸ı�־λ
		g_cMcuAgent.SetIsNetCfgBeModifed(FALSE);
	}
#endif
// ����DSCinfo�е�netparam�ŵ�������Ϣ����������ֹ�ͻ�ض���
#ifdef _MINIMCU_
	TDSCModuleInfo tDscInfo, tLastDscInfo;
	g_cMcuAgent.GetDscInfo( &tDscInfo );
	g_cMcuAgent.GetLastDscInfo( &tLastDscInfo );
	printf("[McsMcuRebootCmd] The new dscinfo is :\n");
	tDscInfo.cosPrint();
	printf("[McsMcuRebootCmd] The old dscinfo is :\n");
	tLastDscInfo.cosPrint();
	TMINIMCUNetParamAll tNetParamAllNew, tNetParamAllLast;
	tDscInfo.GetMcsAccessAddrAll( tNetParamAllNew );
	tLastDscInfo.GetMcsAccessAddrAll( tNetParamAllLast );
	if ( !tNetParamAllLast.IsEqualTo(tNetParamAllNew) )
	{
		BOOL32 bSuccess = TRUE;
		bSuccess = g_cMcuAgent.SaveDscLocalInfoToNip( &tDscInfo );
		bSuccess &= g_cMcuAgent.SaveRouteToNipByDscInfo( &tDscInfo );
		if ( !bSuccess )
		{
			printf("[McsMcuRebootCmd] Save dsc localinfo to nip failed!Roll back!\n");
			bSuccess = TRUE;
			bSuccess &= ( SUCCESS_AGENT == g_cMcuAgent.SetDscInfo( &tLastDscInfo, TRUE ) ) ? TRUE : FALSE;
			bSuccess &= g_cMcuAgent.SaveDscLocalInfoToNip( &tLastDscInfo );
			bSuccess &= g_cMcuAgent.SaveRouteToNipByDscInfo( &tLastDscInfo );
			if ( !bSuccess )
			{
				printf("[McsMcuRebootCmd] Mcu dscinfo roll back ERROR!\n");
			}				
		}
		else
		{
			printf("[McsMcuRebootCmd] Save mcs access addr successfully!\n" );
		}
	}
	else
	{
		printf("[McsMcuRebootCmd] The mcu McsAccessAddr is not changed!\n");
		// ����·��
		if (  !g_cMcuAgent.SaveRouteToNipByDscInfo( &tDscInfo ) )
		{
			printf("[McsMcuRebootCmd] Save route failed! roll back!\n");
			BOOL32 bSuccess = TRUE;
			bSuccess &= ( SUCCESS_AGENT == g_cMcuAgent.SetDscInfo( &tLastDscInfo, TRUE ) ) ? TRUE : FALSE;
			bSuccess &= g_cMcuAgent.SaveRouteToNipByDscInfo( &tLastDscInfo );
			if ( !bSuccess )
			{
				printf( "[McsMcuRebootCmd]Save route roll back failed!\n" );
			}
		}
	}

#endif
	//LogPrint(LOG_LVL_DETAIL, MID_PUB_ALWAYS, "[McsMcuRebootCmd]mcs req reboot\n");
	//printf("[McsMcuRebootCmd]mcs req reboot\n");

	// reboot����MPU(Bas)���� [5/3/2012 chendaiwei]
	if(g_cMcuAgent.IsNeedRebootAllMpuBas())
	{
		u8 byMpuBasNum = 0;
		TEqpMpuBasInfo atMpuBasInfo[MAXNUM_PERIEQP];     
		u16 wRet = g_cMcuAgent.ReadMpuBasTable(&byMpuBasNum, atMpuBasInfo);

		if (SUCCESS_AGENT != wRet)
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_MCS,"Read MpuBas Table error,failed to reboot All MPU brd!\n");
		}
		else
		{
			TBoardInfo atBrdInfo[MAX_BOARD_NUM];
			u8 byBrdNum = 0;
			wRet = g_cMcuAgent.ReadBrdTable(&byBrdNum,atBrdInfo);

			if(SUCCESS_AGENT != wRet)
			{
				LogPrint(LOG_LVL_ERROR,MID_MCU_MCS,"Read Brd Table error,failed to reboot All MPU brd!\n");
			}
			else
			{
				for( u8 byBasIdx = 0; byBasIdx < byMpuBasNum; byBasIdx ++ )
				{
					for(u8 byIdx = 0; byIdx < byBrdNum; byIdx ++)
					{
						if(atBrdInfo[byIdx].GetBrdId() == atMpuBasInfo[byBasIdx].GetRunBrdId())
						{
							wRet = g_cMcuAgent.RebootBoard(atBrdInfo[byIdx].GetLayer(), atBrdInfo[byIdx].GetSlot(), atBrdInfo[byIdx].GetType());
							if (SUCCESS_AGENT != wRet)
							{
								LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[McsMcuRebootCmd] Reboot MPU Board failed, ret :%d\n", wRet);
							}

							break;
						}
					}
				}
			}
		}
	}

#ifdef WIN32
    PostMessage( g_hwndMain, WM_CLOSE, NULL, NULL );

#elif (defined(_8KE_) || defined(_8KH_) || defined(_8KI_) ) && defined(_LINUX_)	
	//��ҵ�������������
//	s8    achProfileName[MAXLEN_MCU_FILEPATH] = {0};
//	sprintf(achProfileName, "%s/%s", DIR_DATA, RUNSTATUS_8KE_CHKFILENAME);
//	s32 dwRunSt = 0;
//	SetRegKeyInt( achProfileName, SECTION_RUNSTATUS, KEY_MCU8KE, dwRunSt );
	//execute reboot
	s8   chCmdline[256] =  {0};   
	sprintf(chCmdline,  "reboot");
	system(chCmdline);

	McuAgentQuit(FALSE);
    OspDelay(100);
#else

	// ���Է��֣�������� OspQuit ����������OSP_DISCONNECT��Ϣ������Ӳ�������л�ʧ��
    // ��ʱ���˳�Osp���öԶ�Osp��⵽����
    // ����ᵼ������������ܾòż�⵽����,�������������ɹ��������״̬����

	McuAgentQuit(FALSE);
    OspDelay(100);	

    if (!g_cMSSsnApp.IsDoubleLink())
    {
        // ����˫��
        OspQuit();
    }

	BrdHwReset();
#endif


    
    return;
}

/*====================================================================
    ������      ��ProcUserManagerMsg
    ����        �������û�������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ���ɹ�����TRUE�������¼��ն�δ�ǼǷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/28    1.0         LI Yi         ����
    06/06/21    4.0         ����        �����û����߼�
====================================================================*/
void CMcsSsnInst::ProcUserManagerMsg( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	CExUsrInfo  *ptUserFullInfo = NULL;
    TUsrGrpInfo tUsrGrpInfo;
    
    // ��ǰ��ص��û�������
    u8 byMcsGrpId = CMcsSsn::GetUserGroup( GetInsID() );

    // ��������������
    u8 byUsrGrpId;
    
    // guzh [12/14/2006] ����Ǳ����л���Ĺ�����ʽ,û���޸�Ȩ��
    if (MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
    {
        switch (pcMsg->event)
        {
        case MCS_MCU_ADDUSER_REQ:
        case MCS_MCU_DELUSER_REQ:
        case MCS_MCU_CHANGEUSER_REQ:
        case MCS_MCU_ADDUSERGRP_REQ:
        case MCS_MCU_CHANGEUSERGRP_REQ:
        case MCS_MCU_DELUSERGRP_REQ:
			cServMsg.SetErrorCode(ERR_MCU_NPLUS_EDITUSER);
			SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			return;
        default:
            break;
        }
    }

	switch (pcMsg->event)
	{
	case MCS_MCU_ADDUSER_REQ:       //�������̨����MCU����û�
        {
		    ptUserFullInfo = (CExUsrInfo*)cServMsg.GetMsgBody();

		    cServMsg.SetMsgBody((u8*)ptUserFullInfo->GetName(), MAX_CHARLENGTH);
            byUsrGrpId = ptUserFullInfo->GetUsrGrpId();

            if ( byMcsGrpId != USRGRPID_SADMIN && 
                 byMcsGrpId != byUsrGrpId )
            {
                // ���ܲ�����������
			    cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
			    SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			    return;
            }

           /* BOOL32 bRet = FALSE;*/
            u8 byMaxNum = 0;
            u8 byNum = 0;

            /*bRet = */
			g_cMcuVcApp.GetMCSUsrGrpUserCount( byUsrGrpId, byMaxNum, byNum );
            if ( byNum >= byMaxNum)
            {
                // ��ǰ���û���
			    cServMsg.SetErrorCode(UM_NOT_ENOUGHMEM);
			    SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			    return;
            }

		    if (g_cUsrManage.AddUser(ptUserFullInfo))
		    {       
			    SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

			    cServMsg.SetMsgBody((u8*)ptUserFullInfo, sizeof(CExUsrInfo));

                // ֪ͨ�����û���
			    g_cMcsSsnApp.Broadcast2SpecGrpMcsSsn( byUsrGrpId, MCU_MCS_ADDUSER_NOTIF, 
											         cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		    }
		    else
		    {
			    cServMsg.SetErrorCode(g_cUsrManage.GetLastError());
			    SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		    }
        }
		break;

	case MCS_MCU_DELUSER_REQ:       //�������̨����MCUɾ���û�

		ptUserFullInfo = (CExUsrInfo*)cServMsg.GetMsgBody();
        byUsrGrpId = ptUserFullInfo->GetUsrGrpId();
		
        if ( byMcsGrpId != USRGRPID_SADMIN && 
             byMcsGrpId != byUsrGrpId )
        {
            // ���ܲ�����������
			cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
			SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			return;
        }

		if (0 == strncmp(m_achUser, ptUserFullInfo->GetName(), (MAXLEN_PWD-1)))
		{
            // ����ɾ���Լ�
			cServMsg.SetErrorCode(UM_ACTOR_ONLY);
			SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			return;
		}

        cServMsg.SetMsgBody((u8*)ptUserFullInfo->GetName(), MAX_CHARLENGTH);
		if (g_cUsrManage.DelUser(ptUserFullInfo))
		{
			SendMsgToMcs( pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

            // ֪ͨ�����û���
			g_cMcsSsnApp.Broadcast2SpecGrpMcsSsn( byUsrGrpId, MCU_MCS_DELUSER_NOTIF, 
											      cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		}
		else
		{
			cServMsg.SetErrorCode( g_cUsrManage.GetLastError() );
			SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		}

		break;

	case MCS_MCU_CHANGEUSER_REQ:  //�������̨����MCU�޸��û�

		ptUserFullInfo = (CExUsrInfo*)cServMsg.GetMsgBody();
        byUsrGrpId = ptUserFullInfo->GetUsrGrpId();

		cServMsg.SetMsgBody((u8*)ptUserFullInfo->GetName(), MAX_CHARLENGTH);

        if ( byMcsGrpId != USRGRPID_SADMIN && 
             byMcsGrpId != byUsrGrpId )
        {
            // ���ܲ�����������
			cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
			SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());		
            return;
        }
        
		if (g_cUsrManage.ModifyInfo(ptUserFullInfo))
		{
			SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

			cServMsg.SetMsgBody((u8*)ptUserFullInfo, sizeof(CExUsrInfo));
			g_cMcsSsnApp.Broadcast2SpecGrpMcsSsn( byUsrGrpId, MCU_MCS_CHANGEUSER_NOTIF, 
											      cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		}
		else
		{
			cServMsg.SetErrorCode( g_cUsrManage.GetLastError() );
			SendMsgToMcs( pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		}
		
		break;
		
	case MCS_MCU_GETUSERLIST_REQ:   //�������̨����MCU�õ��û��б�
		{
			CLongMessage cLongMsg;
			u8 abyBuf[sizeof(CExUsrInfo) * USERNUM_PERPKT];   // CExUsrInfo size 249

			u8 byUsrItr = 0; // �����û���ָ��
			u8 byUserNumInPack = 0;

            BOOL32 bRet = FALSE;
            u8 byUsrMaxNum = 0;
            u8 byUsrNum = 0;

            bRet = g_cMcuVcApp.GetMCSUsrGrpUserCount(byMcsGrpId, byUsrMaxNum, byUsrNum);
            
            // ������Ҫ�����ܰ���
            cLongMsg.m_uTotal = byUsrNum / USERNUM_PERPKT;
            if ( byUsrNum % USERNUM_PERPKT != 0 )
            {
                cLongMsg.m_uTotal ++;
            }
            cLongMsg.m_uCurr = 1;

			if ( !bRet || byUsrNum == 0)
			{
                if (!bRet)
                {
                    // ��ȡʧ��,Nack
                    SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());  // Nack
                }
                else
                {
                    // ����Ϊ0,Ack
                    SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()); // Ack
                }
				
				LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcUserManagerMsg] No users in current group\n");
			}
			else
			{
				SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()); // Ack

                // Notify
				while ( cLongMsg.m_uCurr <= cLongMsg.m_uTotal ) 
				{
					byUserNumInPack = 0;
					if ( !g_cMcuVcApp.GetMCSCurUserList(byMcsGrpId, abyBuf, byUsrItr, byUserNumInPack) )
					{
						LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcUserManagerMsg] GetMcuCurUserList Failed at index: %d\n", byUsrItr);
                        return;
					}
					cLongMsg.m_uNumUsr = byUserNumInPack;

					cServMsg.SetMsgBody((u8*)&cLongMsg, sizeof(cLongMsg));
					cServMsg.CatMsgBody(abyBuf, byUserNumInPack * sizeof(CExUsrInfo));

					SendMsgToMcs( MCU_MCS_USERLIST_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

                    cLongMsg.m_uCurr ++;
				}
			}
		}
		break;
    case MCS_MCU_ADDUSERGRP_REQ:    //�������̨����û���
        // ֻҪ��������Ա��Ȩ��
        {
            tUsrGrpInfo = *(TUsrGrpInfo*)cServMsg.GetMsgBody();

            if ( byMcsGrpId != USRGRPID_SADMIN  )
            {
                // ���ܲ���
			    cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
                cServMsg.SetMsgBody( (u8*)tUsrGrpInfo.GetUsrGrpName(), MAX_CHARLENGTH );
			    SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
                return;
            }
            else
            {
                u16 wErrorCode = 0;
                byUsrGrpId = g_cMcuVcApp.AddMCSUserGroup(tUsrGrpInfo, wErrorCode);
                if ( byUsrGrpId == USRGRPID_INVALID )
                {
			        cServMsg.SetErrorCode( wErrorCode );
                    cServMsg.SetMsgBody( (u8*)tUsrGrpInfo.GetUsrGrpName(), MAX_CHARLENGTH );
			        SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
                }
                else
                {
                    tUsrGrpInfo.SetUsrGrpId(byUsrGrpId);
                    cServMsg.SetMsgBody( (u8*)&tUsrGrpInfo, sizeof(TUsrGrpInfo) );

                    SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	

                    cServMsg.SetEventId( MCU_MCS_USERGRP_NOTIF );
			        g_cMcsSsnApp.Broadcast2SpecGrpMcsSsn( byUsrGrpId, MCU_MCS_USERGRP_NOTIF, 
											              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

                }
            }
        }
        break;
        
    case MCS_MCU_CHANGEUSERGRP_REQ:    //�������̨�޸��û���
        // ֻҪ��������Ա��Ȩ��
        {
            tUsrGrpInfo = *(TUsrGrpInfo*)cServMsg.GetMsgBody();
            u8 byTargetId = tUsrGrpInfo.GetUsrGrpId();
            if ( byMcsGrpId != USRGRPID_SADMIN  )
            {
                // ���ܲ���
			    cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);                
                cServMsg.SetMsgBody( (u8*)&byTargetId, sizeof(u8) );
			    SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
                return;
            }
            else
            {
                u16 wErrorCode = 0;
                if ( !g_cMcuVcApp.ChgMCSUserGroup(tUsrGrpInfo, wErrorCode) )
                {
			        cServMsg.SetErrorCode( wErrorCode );
                    cServMsg.SetMsgBody( (u8*)&byTargetId, sizeof(u8) );
			        SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
                }
                else
                {
                    SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	

                    cServMsg.SetEventId( MCU_MCS_USERGRP_NOTIF );

			        g_cMcsSsnApp.Broadcast2SpecGrpMcsSsn( byMcsGrpId, MCU_MCS_USERGRP_NOTIF, 
											              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

                }
            }
        }
        break;

    case MCS_MCU_DELUSERGRP_REQ:    //�������̨ɾ���û��鼰�����Դ
        // ֻҪ��������Ա��Ȩ��
        {
            u8 byDelGrpId = *cServMsg.GetMsgBody();
            if ( byMcsGrpId != USRGRPID_SADMIN )
            {
                // ���ܲ���
			    cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
			    SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());		
                return;
            }
            else
            {
                u16 wErrorCode = 0;
                if ( !g_cMcuVcApp.DelMCSUserGroup(byDelGrpId, wErrorCode) )
                {
			        cServMsg.SetErrorCode( wErrorCode );
			        SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
                }
                else
                {
                    SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	

                    // Nofity
                    cServMsg.SetEventId( MCU_MCS_DELUSERGRP_NOTIF );
                    
			        g_cMcsSsnApp.Broadcast2SpecGrpMcsSsn( byMcsGrpId, MCU_MCS_DELUSERGRP_NOTIF, 
											              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

                }
            }
        }
        break;
        
    case MCS_MCU_GETUSERGRP_REQ:    //�������̨�����û�����Ϣ
        // ֻҪ��������Ա��Ȩ��
        {
            if ( byMcsGrpId != USRGRPID_SADMIN )
            {
                // ���ܲ���
			    cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
                cServMsg.SetMsgBody();
			    SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());		
            }
            else
            {
                u8 byNum = 0;
                TUsrGrpInfo *ptInfo = NULL;
                g_cMcuVcApp.GetMCSUserGroupInfo( byNum, &ptInfo );
                
                cServMsg.SetMsgBody( &byNum, sizeof(u8) );
                cServMsg.CatMsgBody( (u8*)ptInfo, sizeof(TUsrGrpInfo) * byNum );

                SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
            }
        }
        break;

    default:
        break;
	}

	return;
}


/*=============================================================================
�� �� ���� ProcAddrBookMsg
��    �ܣ� ��ַ����Ϣ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/2/14  4.0			������                  ����
=============================================================================*/
void CMcsSsnInst::ProcAddrBookMsg( const CMessage * pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    if(NULL == g_pcAddrBook)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[CMcsSsnInst][ProcAddrBookMsg] g_pcAddrBook == NULL!\n");
		if (cServMsg.GetSrcSsnId() != 0)
		{
			cServMsg.SetErrorCode(ERR_MCU_ADDRBOOKINIT);
			SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		}
        return;
    }
    switch(pcMsg->event)
    {
    case MCS_MCU_ADDRBOOK_GETENTRYLIST_REQ:
    case MCU_ADDRBOOK_GETENTRYLIST_NOTIF:    
        {
            if(MCS_MCU_ADDRBOOK_GETENTRYLIST_REQ == pcMsg->event)
            {
                SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()); //ack
            }

            u32 dwSize = g_pcAddrBook->GetAddrEntryUsedSize();
			if(0 == dwSize)
            {
                return;
            }

            u8 byTotalPktNum = (u8)(dwSize/ADDRENTRYNUM_PERPKT);
            if(dwSize%ADDRENTRYNUM_PERPKT > 0)
            {
                byTotalPktNum++;
            }
            cServMsg.SetTotalPktNum(byTotalPktNum);
            
            u8 byCurPktIdx = 0;
            u32 dwCurNum = 0;
			u32 dwCurV1Num = 0;
            CAddrEntry cAddrEntry;
            TMcuAddrEntry atAddrEntry[ADDRENTRYNUM_PERPKT];
			// [6/25/2013 guodawei]
			TMcuAddrEntryV1 atAddrEntryV1[ADDRENTRYNUM_PERPKT];

            u32 dwStartIdx = g_pcAddrBook->GetFirstAddrEntry( &cAddrEntry );
            if(INVALID_INDEX == dwStartIdx)
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] get first addr entry failed!\n");
                return;
            }
            AddrEntryOut2In(&cAddrEntry, &atAddrEntry[dwCurNum++]);
			AddrEntryV1Out2In(&cAddrEntry, &atAddrEntryV1[dwCurV1Num++]);

            while( 0 != g_pcAddrBook->GetNextAddrEntry(dwStartIdx, &cAddrEntry))
            {                    
                AddrEntryOut2In(&cAddrEntry, &atAddrEntry[dwCurNum++]);
				AddrEntryV1Out2In(&cAddrEntry, &atAddrEntryV1[dwCurV1Num++]);

                dwStartIdx = cAddrEntry.GetEntryIndex();
                
                if(ADDRENTRYNUM_PERPKT == dwCurNum || ADDRENTRYNUM_PERPKT == dwCurV1Num)
                {
                    cServMsg.SetCurPktIdx(byCurPktIdx++);

					//  [6/25/2013 guodawei]
					u8 byMemberNum = atAddrEntryV1->GetMemberNum();
					cServMsg.SetMsgBody((u8*)&byMemberNum, sizeof(u8));
					for (u8 byIdx = 0; byIdx < atAddrEntryV1->GetMemberNum(); byIdx++)
					{
						u16 wMemberLen = htons(atAddrEntryV1->GetMemberLen(byIdx));
						cServMsg.CatMsgBody((u8*)&wMemberLen, sizeof(u16));
					}
					cServMsg.CatMsgBody((u8*)atAddrEntryV1, sizeof(TMcuAddrEntryV1) * dwCurV1Num);
					SendMsgToMcs(MCU_MCS_ADDRBOOK_GETENTRYLISTEXT_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

                    dwCurNum = 0;
					dwCurV1Num = 0;
                }                
            }    
            
            if(dwCurNum > 0 || dwCurV1Num > 0)
            {
                cServMsg.SetCurPktIdx(byCurPktIdx++);

				u8 byMemberNum = atAddrEntryV1->GetMemberNum();
				cServMsg.SetMsgBody((u8*)&byMemberNum, sizeof(u8));
				for (u8 byIdx = 0; byIdx < atAddrEntryV1->GetMemberNum(); byIdx++)
				{
					u16 wMemberLen = htons(atAddrEntryV1->GetMemberLen(byIdx));
					cServMsg.CatMsgBody((u8*)&wMemberLen, sizeof(u16));
				}
				cServMsg.CatMsgBody((u8*)atAddrEntryV1, sizeof(TMcuAddrEntryV1) * dwCurV1Num);
				SendMsgToMcs(MCU_MCS_ADDRBOOK_GETENTRYLISTEXT_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
            }
        }
        break;

    case MCS_MCU_ADDRBOOK_GETGROUPLIST_REQ:
    case MCU_ADDRBOOK_GETGROUPLIST_NOTIF:
        {
            if(MCS_MCU_ADDRBOOK_GETGROUPLIST_REQ == pcMsg->event)
            {
                SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()); //ack
            }
                        
            u32 dwSize = g_pcAddrBook->GetAddrGroupUsedSize();
			if(0 == dwSize)
            {                                
                return;
            }
            
            u8 byTotalPktNum = (u8)(dwSize/ADDRGROUPNUM_PERPKT);
            if(dwSize%ADDRGROUPNUM_PERPKT > 0)
            {
                byTotalPktNum++;
            }
            cServMsg.SetTotalPktNum(byTotalPktNum);
            
            u8 byCurPktIdx = 0;
            u32 dwCurNum = 0;
			u32 dwCurV1Num = 0;
            CAddrMultiSetEntry cAddrGroup;
            TMcuAddrGroup atAddrGroup[ADDRGROUPNUM_PERPKT];
			TMcuAddrGroupV1 atAddrGroupV1[ADDRGROUPNUM_PERPKT];
            u32 dwStartIdx = g_pcAddrBook->GetFirstAddrMultiSetEntry( &cAddrGroup );
            if(INVALID_INDEX == dwStartIdx)
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] get first addr group failed!\n");
                return;
            }
            AddrGroupOut2In(&cAddrGroup, &atAddrGroup[dwCurNum++]);
			AddrGroupV1Out2In(&cAddrGroup, &atAddrGroupV1[dwCurV1Num++]);

            while( 0 != g_pcAddrBook->GetNextAddrMultiSetEntry(dwStartIdx, &cAddrGroup))
            {                    
                AddrGroupOut2In(&cAddrGroup, &atAddrGroup[dwCurNum++]);
				AddrGroupV1Out2In(&cAddrGroup, &atAddrGroupV1[dwCurV1Num++]);

                dwStartIdx = cAddrGroup.GetGroupIndex();

                if(ADDRGROUPNUM_PERPKT == dwCurNum || ADDRGROUPNUM_PERPKT == dwCurV1Num)
                {
                    cServMsg.SetCurPktIdx(byCurPktIdx++);

					u8 byMemberNum = atAddrGroupV1->GetMemberNum();
					cServMsg.SetMsgBody((u8*)&byMemberNum, sizeof(u8));
					for (u8 byIdx = 0; byIdx < atAddrGroupV1->GetMemberNum(); byIdx++)
					{
						u16 wMemberLen = htons(atAddrGroupV1->GetMemberLen(byIdx));
						cServMsg.CatMsgBody((u8*)&wMemberLen, sizeof(u16));
					}
					cServMsg.CatMsgBody((u8*)atAddrGroupV1, sizeof(TMcuAddrGroupV1) * dwCurV1Num);
                    SendMsgToMcs(MCU_MCS_ADDRBOOK_GETGROUPLISTEXT_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

                    dwCurNum = 0;
					dwCurV1Num = 0;
                }                
            }        

            if(dwCurNum > 0 || dwCurV1Num > 0)
            {
                cServMsg.SetCurPktIdx(byCurPktIdx++);

				u8 byMemberNum = atAddrGroupV1->GetMemberNum();
				cServMsg.SetMsgBody((u8*)&byMemberNum, sizeof(u8));
				for (u8 byIdx = 0; byIdx < atAddrGroupV1->GetMemberNum(); byIdx++)
				{
					u16 wMemberLen = htons(atAddrGroupV1->GetMemberLen(byIdx));
					cServMsg.CatMsgBody((u8*)&wMemberLen, sizeof(u16));
				}
				cServMsg.CatMsgBody((u8*)atAddrGroupV1, sizeof(TMcuAddrGroupV1) * dwCurV1Num);
                SendMsgToMcs(MCU_MCS_ADDRBOOK_GETGROUPLISTEXT_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
            }
        }
        break;

    case MCS_MCU_ADDRBOOK_ADDENTRY_REQ:
        {
            TMcuAddrEntry *ptAddrEntry = (TMcuAddrEntry *)cServMsg.GetMsgBody();
            TADDRENTRY tAddrEntry;
            AddrEntryIn2Out(ptAddrEntry, &tAddrEntry);
	
			//  [6/17/2013 guodawei]
			s8 *lpMsgBody = (s8 *)(cServMsg.GetMsgBody() + sizeof(TMcuAddrEntry));
			tagAddrEntryInfo tAddrInfo = {0};
			memcpy(&tAddrInfo, &tAddrEntry.GetEntryInfo(), sizeof(tagAddrEntryInfo));

			u16 wEntryNameLen = ntohs(*(u16 *)lpMsgBody);
			lpMsgBody += sizeof(u16);
			memcpy(tAddrInfo.achEntryName, lpMsgBody, wEntryNameLen);
			lpMsgBody += wEntryNameLen;

			u16 wMtAliasLen = ntohs(*(u16 *)lpMsgBody);
			lpMsgBody += sizeof(u16);
			memcpy(tAddrInfo.achMtAlias, lpMsgBody, wMtAliasLen);
			lpMsgBody += wMtAliasLen;

			tAddrEntry.SetEntryInfo(&tAddrInfo);

            if(INVALID_INDEX != g_pcAddrBook->IsAddrEntryExist(tAddrEntry.GetEntryInfo()))
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[CMcsSsnInst][ProcAddrBookMsg] addr entry is already exist\n");
                cServMsg.SetErrorCode(ADDRBOOK_EXIST_ELEMENT);
                SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                return;
            }
            
            u32 dwIndex = g_pcAddrBook->AddAddrEntry( tAddrEntry.GetEntryInfo() );
            if ( INVALID_INDEX == dwIndex )
            {
				LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] add addr entry failed\n");
                cServMsg.SetErrorCode((u16)g_pcAddrBook->GetLastError());
                SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                return;
            }

            ptAddrEntry->SetEntryIdx(dwIndex);
            SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_ADDRBOOK_ADDENTRY_NOTIF, 
                                              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
        break;

    case MCS_MCU_ADDRBOOK_DELENTRY_REQ:
        {
            u32 dwIdx = ntohl(*(u32 *)cServMsg.GetMsgBody());
            if(!g_pcAddrBook->DelAddrEntry( dwIdx ))
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] del addr entry failed\n");
                cServMsg.SetErrorCode((u16)g_pcAddrBook->GetLastError());
                SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                return;
            }
            SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_ADDRBOOK_DELENTRY_NOTIF, 
                                              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
        break;

    case MCS_MCU_ADDRBOOK_MODIFYENTRY_REQ:
        {
            TMcuAddrEntry *ptAddrEntry = (TMcuAddrEntry *)cServMsg.GetMsgBody();
            TADDRENTRY tAddrEntry;
            AddrEntryIn2Out(ptAddrEntry, &tAddrEntry);

			//  [6/17/2013 guodawei]
			s8 *lpMsgBody = (s8 *)(cServMsg.GetMsgBody() + sizeof(TMcuAddrEntry));
			tagAddrEntryInfo tAddrInfo = {0};
			memcpy(&tAddrInfo, &tAddrEntry.GetEntryInfo(), sizeof(tagAddrEntryInfo));
			
			u16 wEntryNameLen = ntohs(*(u16 *)lpMsgBody);
			lpMsgBody += sizeof(u16);
			memcpy(tAddrInfo.achEntryName, lpMsgBody, wEntryNameLen);
			lpMsgBody += wEntryNameLen;
			
			u16 wMtAliasLen = ntohs(*(u16 *)lpMsgBody);
			lpMsgBody += sizeof(u16);
			memcpy(tAddrInfo.achMtAlias, lpMsgBody, wMtAliasLen);
			lpMsgBody += wMtAliasLen;
			
			tAddrEntry.SetEntryInfo(&tAddrInfo);
                        
            BOOL32 bRet = g_pcAddrBook->ModifyAddrEntry( tAddrEntry.GetEntryIndex(), tAddrEntry.GetEntryInfo() );
            if ( !bRet )
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] modify addr entry failed\n");
                cServMsg.SetErrorCode((u16)g_pcAddrBook->GetLastError());
                SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                return;
            }
            
            SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
            CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_ADDRBOOK_MODIFYENTRY_NOTIF, 
                                              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
        break;

    case MCS_MCU_ADDRBOOK_ADDGROUP_REQ:
        {
            TMcuAddrGroup *ptAddrGroup = (TMcuAddrGroup *)cServMsg.GetMsgBody();
            TADDRMULTISETENTRY tAddrMultiSetEntry;
            AddrGroupIn2Out(ptAddrGroup, &tAddrMultiSetEntry);

			//  [6/17/2013 guodawei]
			s8 *lpMsgBody = (s8 *)(cServMsg.GetMsgBody() + sizeof(TMcuAddrGroup));
			tagMultiSetEntryInfo tEntryInfo;
			memcpy(&tEntryInfo, &tAddrMultiSetEntry.GetEntryInfo(), sizeof(tagAddrEntryInfo));
			
			u16 wGroupNameLen = ntohs(*(u16 *)lpMsgBody);
			lpMsgBody += sizeof(u16);
			memcpy(tEntryInfo.achGroupName, lpMsgBody, wGroupNameLen);
			lpMsgBody += wGroupNameLen;

			tAddrMultiSetEntry.SetEntryInfo(&tEntryInfo);

            if(INVALID_INDEX != g_pcAddrBook->IsAddrMultiSetEntryExist(tAddrMultiSetEntry.GetEntryInfo()))
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] addr group is already exist\n");
                cServMsg.SetErrorCode(ADDRBOOK_EXIST_ELEMENT);
                SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                return;
            }
            
            u32 dwIndex = g_pcAddrBook->AddAddrMultiSetEntry( tAddrMultiSetEntry.GetEntryInfo() );
            if ( INVALID_INDEX == dwIndex )
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] add addr group failed\n");
                cServMsg.SetErrorCode((u16)g_pcAddrBook->GetLastError());
                SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                return;
            }
            
            ptAddrGroup->SetEntryIdx(dwIndex);
            SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

            u8 byConfTableLen = (u8)tAddrMultiSetEntry.GetListEntryNum();
            u32 adwConfEntryTable[ MAXNUM_ENTRY_TABLE ] = { 0 };
            tAddrMultiSetEntry.GetListEntryIndex( adwConfEntryTable, byConfTableLen );
            for ( u8 byIdx = 0; byIdx < byConfTableLen; byIdx++ )
            {
                g_pcAddrBook->AddAddrEntry( dwIndex, adwConfEntryTable[byIdx] );
            }

            CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_ADDRBOOK_ADDGROUP_NOTIF, 
                                              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
        break;

    case MCS_MCU_ADDRBOOK_DELGROUP_REQ:
        {
            u32 dwIdx = ntohl(*(u32 *)cServMsg.GetMsgBody());
            if(!g_pcAddrBook->DelAddrMultiSetEntry( dwIdx ))
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] del addr group failed\n");
                cServMsg.SetErrorCode((u16)g_pcAddrBook->GetLastError());
                SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                return;
            }
            SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
            CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_ADDRBOOK_DELGROUP_NOTIF, 
                                              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
        break;

    case MCS_MCU_ADDRBOOK_MODIFYGROUP_REQ:
        {
            TMcuAddrGroup *ptAddrGroup = (TMcuAddrGroup *)cServMsg.GetMsgBody();
            TADDRMULTISETENTRY tAddrMultiSetEntry;
            AddrGroupIn2Out(ptAddrGroup, &tAddrMultiSetEntry);

			//  [6/17/2013 guodawei]
			s8 *lpMsgBody = (s8 *)(cServMsg.GetMsgBody() + sizeof(TMcuAddrGroup));
			tagMultiSetEntryInfo tEntryInfo;
			memcpy(&tEntryInfo, &tAddrMultiSetEntry.GetEntryInfo(), sizeof(tagAddrEntryInfo));
			
			u16 wGroupNameLen = ntohs(*(u16 *)lpMsgBody);
			lpMsgBody += sizeof(u16);
			memcpy(tEntryInfo.achGroupName, lpMsgBody, wGroupNameLen);
			lpMsgBody += wGroupNameLen;
			
			tAddrMultiSetEntry.SetEntryInfo(&tEntryInfo);
            
            CAddrMultiSetEntry cAddrEntry;
            g_pcAddrBook->GetAddrMultiSetEntry( &cAddrEntry, tAddrMultiSetEntry.GetEntryIndex() );
            BOOL32 bRet = g_pcAddrBook->ModifyAddrGroup( tAddrMultiSetEntry.GetEntryIndex(), 
                                                         tAddrMultiSetEntry.GetEntryInfo(),
                                                         cAddrEntry.GetEntryCount(),
                                                         cAddrEntry.GetEntryTable() );
            if ( !bRet )
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] modify addr group failed\n");
                cServMsg.SetErrorCode((u16)g_pcAddrBook->GetLastError());
                SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                return;
            }
            
            SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());          
            CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_ADDRBOOK_MODIFYGROUP_NOTIF, 
                                              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
        break;

    case MCS_MCU_ADDRBOOK_ADDENTRYTOGROUP_REQ:
        {
            TMcuAddrGroup *ptAddrGroup = (TMcuAddrGroup *)cServMsg.GetMsgBody();
            u32 dwGroupEntryIndex = ptAddrGroup->GetEntryIdx();
            u32 dwAddrEntryNum = ptAddrGroup->GetEntryNum();
            u32 adwConfEntryTable[ MAXNUM_ENTRY_TABLE ] = { 0 };
            ptAddrGroup->GetAllEntryIdx(adwConfEntryTable, dwAddrEntryNum);
            
            if( !g_pcAddrBook->IsAddrMultiSetEntryValid(dwGroupEntryIndex))
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] add addr to group failed, entry idx invalid\n");
                cServMsg.SetErrorCode((u16)g_pcAddrBook->GetLastError());
                SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                return;
            }
     
            TMcuAddrGroup tMcuAddrGroup;
            tMcuAddrGroup.SetEntryIdx(ptAddrGroup->GetEntryIdx());
            tMcuAddrGroup.SetGroupName(ptAddrGroup->GetGroupName());
            u32 adwAddEntryTable[ MAXNUM_ENTRY_TABLE ] = { 0 };
            u32 dwAddEntryNum = 0;
            for ( u32 dwIdx = 0; dwIdx < dwAddrEntryNum; dwIdx++ )
            {

                u32 dwIndex = g_pcAddrBook->AddAddrEntry( dwGroupEntryIndex, adwConfEntryTable[dwIdx] );
                if ( dwIndex == INVALID_INDEX )
                {
                    LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] add addr to group failed, EntryNum :%d\n", dwAddrEntryNum);
                    cServMsg.SetErrorCode((u16)g_pcAddrBook->GetLastError());
                    SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

                    if (dwAddEntryNum >= 1)
                    {
                        tMcuAddrGroup.SetAllEntryIdx(adwAddEntryTable, dwAddEntryNum);
                        CServMsg cNotifMsg;
                        cNotifMsg.SetMsgBody((u8*)&tMcuAddrGroup, sizeof(TMcuAddrGroup));

						//  [6/17/2013 guodawei]
						cNotifMsg.CatMsgBody((cServMsg.GetMsgBody() + sizeof(TMcuAddrGroup)), 
							cServMsg.GetMsgBodyLen() - sizeof(TMcuAddrGroup));

                        CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_ADDRBOOK_ADDENTRYTOGROUP_NOTIF, 
                                              cNotifMsg.GetServMsg(), cNotifMsg.GetServMsgLen());
                        LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcAddrBookMsg] not all entry be added to Group:%d, actual add num is :%d\n",
                            dwGroupEntryIndex, dwAddEntryNum);

                    }

                    //�в�����Ŀ��ӳɹ�Ҫд�ļ�
                    //return;
                    break;
                }
                adwAddEntryTable[dwAddEntryNum] = dwIndex;
                dwAddEntryNum++;

            }

            SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());          
            CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_ADDRBOOK_ADDENTRYTOGROUP_NOTIF, 
                                              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
        break;

    case MCS_MCU_ADDRBOOK_DELENTRYFROMGROUP_REQ:
        {
            TMcuAddrGroup *ptAddrGroup = (TMcuAddrGroup *)cServMsg.GetMsgBody();
            
            u32 dwGroupEntryIndex = ptAddrGroup->GetEntryIdx();
            u32  dwAddrEntryNum = ptAddrGroup->GetEntryNum();
            u32 adwConfEntryTable[ MAXNUM_ENTRY_TABLE ] = { 0 };
            ptAddrGroup->GetAllEntryIdx( adwConfEntryTable, dwAddrEntryNum );

            if(! g_pcAddrBook->IsAddrMultiSetEntryValid(dwGroupEntryIndex))
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] del addr from group failed, entry idx invalid\n");
                cServMsg.SetErrorCode((u16)g_pcAddrBook->GetLastError());
                SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                return;
            }
            
            for ( u32 dwIdx = 0; dwIdx < dwAddrEntryNum; dwIdx++ )
            {
                g_pcAddrBook->DelAddrEntry( dwGroupEntryIndex, adwConfEntryTable[dwIdx] );                
            }

            SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());          
            CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_ADDRBOOK_DELENTRYFROMGROUP_NOTIF, 
                                              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
        break;
        
    default: return;
    }

    if (pcMsg->event == MCS_MCU_ADDRBOOK_GETENTRYLIST_REQ ||
        pcMsg->event == MCU_ADDRBOOK_GETENTRYLIST_NOTIF ||
        pcMsg->event == MCS_MCU_ADDRBOOK_GETGROUPLIST_REQ ||
        pcMsg->event == MCU_ADDRBOOK_GETGROUPLIST_NOTIF)
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[CMcsSsnInst][ProcAddrBookMsg] Get addrbook info, no necessary to save!\n");
        return;
    }
    else if(!g_pcAddrBook->SaveAddrBook(GetAddrBookPath()))          
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[CMcsSsnInst][ProcAddrBookMsg] SaveAddrBook failed!\n");
    }

    return;
}


/*=============================================================================
�� �� ���� AddrEntryOut2In
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CAddrEntry &cAddrIn
           TMcuAddrEntry &tAddrOut
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/2/15  4.0			������                  ����
=============================================================================*/
void CMcsSsnInst::AddrEntryOut2In(CAddrEntry *pcAddrIn, TMcuAddrEntry *ptAddrOut, u8 byNum)
{
    if(NULL == pcAddrIn || NULL == ptAddrOut)
    {
        return;
    }
    
    for(u8 byIdx = 0; byIdx < byNum; byIdx++)
    {
        ptAddrOut[byIdx].SetEntryIdx(pcAddrIn[byIdx].GetEntryIndex());
        TADDRENTRYINFO tInfo;
        pcAddrIn[byIdx].GetEntryInfo(&tInfo);
        ptAddrOut[byIdx].SetCallRate(/*ntohs(*/tInfo.wCallRate/*)*/);   //�ڴ��к����������������
        ptAddrOut[byIdx].SetEntryName(tInfo.achEntryName);
        ptAddrOut[byIdx].SetH320Id(tInfo.achH320Id);
        ptAddrOut[byIdx].SetMtAlias(tInfo.achMtAlias);
        ptAddrOut[byIdx].SetMtE164(tInfo.achMtNumber);
        ptAddrOut[byIdx].SetMtIpAddr(ntohl(tInfo.dwMtIpAddr));
        if(0 == tInfo.bH320Terminal)
        {
            ptAddrOut[byIdx].SetMtProtocolType(PROTOCOL_TYPE_H323);
        }
        else
        {
            ptAddrOut[byIdx].SetMtProtocolType(PROTOCOL_TYPE_H320);
        }
    }    

    return;
}


/*=============================================================================
�� �� ���� AddrGroupOut2In
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CAddrMultiSetEntry *pcGroupIn
           TMcuAddrGroup *ptGroupOut
           u8 byNum
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/2/18  4.0			������                  ����
=============================================================================*/
void CMcsSsnInst::AddrGroupOut2In(CAddrMultiSetEntry *pcGroupIn, TMcuAddrGroup *ptGroupOut, u8 byNum)
{
    if(NULL == pcGroupIn || NULL == ptGroupOut)
    {
        return;
    }

    for(u8 byIdx = 0; byIdx < byNum; byIdx++)
    {
        ptGroupOut[byIdx].SetEntryIdx(pcGroupIn[byIdx].GetGroupIndex());
        TMULTISETENTRYINFO tInfo;
        memset(&tInfo, 0, sizeof(tInfo));
        pcGroupIn[byIdx].GetGroupInfo(&tInfo);
        ptGroupOut[byIdx].SetGroupName(tInfo.achGroupName);
        u8 byGroupNum = pcGroupIn[byIdx].GetEntryCount();
        ptGroupOut[byIdx].SetAllEntryIdx(pcGroupIn[byIdx].GetEntryTable(), byGroupNum);
    }

    return;
}

/*=============================================================================
�� �� ���� AddrEntryOut2In
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CAddrEntry &cAddrIn
           TMcuAddrEntry &tAddrOut
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/2/15  4.0			������                  ����
=============================================================================*/
void CMcsSsnInst::AddrEntryV1Out2In(CAddrEntry *pcAddrIn, TMcuAddrEntryV1 *ptAddrOut, u8 byNum)
{
    if(NULL == pcAddrIn || NULL == ptAddrOut)
    {
        return;
    }
    
    for(u8 byIdx = 0; byIdx < byNum; byIdx++)
    {
        ptAddrOut[byIdx].SetEntryIdx(pcAddrIn[byIdx].GetEntryIndex());
        TADDRENTRYINFO tInfo;
        pcAddrIn[byIdx].GetEntryInfo(&tInfo);
        ptAddrOut[byIdx].SetCallRate(/*ntohs(*/tInfo.wCallRate/*)*/);   //�ڴ��к����������������
        ptAddrOut[byIdx].SetEntryName(tInfo.achEntryName);
        ptAddrOut[byIdx].SetH320Id(tInfo.achH320Id);
        ptAddrOut[byIdx].SetMtAlias(tInfo.achMtAlias);
        ptAddrOut[byIdx].SetMtE164(tInfo.achMtNumber);
        ptAddrOut[byIdx].SetMtIpAddr(ntohl(tInfo.dwMtIpAddr));
        if(0 == tInfo.bH320Terminal)
        {
            ptAddrOut[byIdx].SetMtProtocolType(PROTOCOL_TYPE_H323);
        }
        else
        {
            ptAddrOut[byIdx].SetMtProtocolType(PROTOCOL_TYPE_H320);
        }
    }    

    return;
}


/*=============================================================================
�� �� ���� AddrGroupOut2In
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CAddrMultiSetEntry *pcGroupIn
           TMcuAddrGroup *ptGroupOut
           u8 byNum
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/2/18  4.0			������                  ����
=============================================================================*/
void CMcsSsnInst::AddrGroupV1Out2In(CAddrMultiSetEntry *pcGroupIn, TMcuAddrGroupV1 *ptGroupOut, u8 byNum)
{
    if(NULL == pcGroupIn || NULL == ptGroupOut)
    {
        return;
    }

    for(u8 byIdx = 0; byIdx < byNum; byIdx++)
    {
        ptGroupOut[byIdx].SetEntryIdx(pcGroupIn[byIdx].GetGroupIndex());
        TMULTISETENTRYINFO tInfo;
        memset(&tInfo, 0, sizeof(tInfo));
        pcGroupIn[byIdx].GetGroupInfo(&tInfo);
        ptGroupOut[byIdx].SetGroupName(tInfo.achGroupName);
        u8 byGroupNum = pcGroupIn[byIdx].GetEntryCount();
        ptGroupOut[byIdx].SetAllEntryIdx(pcGroupIn[byIdx].GetEntryTable(), byGroupNum);
    }

    return;
}


/*=============================================================================
�� �� ���� AddrEntryIn2Out
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TMcuAddrEntry *ptAddrIn
           TADDRENTRY *ptAddrOut
           u8 byNum
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/2/18  4.0			������                  ����
=============================================================================*/
void CMcsSsnInst::AddrEntryIn2Out(TMcuAddrEntry *ptAddrIn, TADDRENTRY *ptAddrOut)
{
    if(NULL == ptAddrIn || NULL == ptAddrOut)
    {
        return;
    }
    
    TADDRENTRYINFO tAddrEntryInfo;
    strcpy(tAddrEntryInfo.achEntryName, ptAddrIn->GetEntryName());
    strcpy(tAddrEntryInfo.achMtAlias, ptAddrIn->GetMtAlias());
    strcpy(tAddrEntryInfo.achH320Id, ptAddrIn->GetH320Id());
    strcpy(tAddrEntryInfo.achMtNumber, ptAddrIn->GetMtE164());
    tAddrEntryInfo.dwMtIpAddr = htonl(ptAddrIn->GetMtIpAddr());
    tAddrEntryInfo.wCallRate = htons(ptAddrIn->GetCallRate());
    tAddrEntryInfo.bH320Terminal = (PROTOCOL_TYPE_H320 == ptAddrIn->GetMtProtocolType()) ? 1 : 0;

    ptAddrOut->SetEntryIndex(ptAddrIn->GetEntryIdx());
    ptAddrOut->/*SetEntryInfo*/SetEntry(&tAddrEntryInfo);       //��֤�õ������������
    
    return;
}


/*=============================================================================
�� �� ���� AddrGroupIn2Out
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TMcuAddrGroup *ptGroupIn
           TADDRMULTISETENTRY *ptGroupOut
           u8 byNum
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/2/18  4.0			������                  ����
=============================================================================*/
void CMcsSsnInst::AddrGroupIn2Out(TMcuAddrGroup *ptGroupIn, TADDRMULTISETENTRY *ptGroupOut)
{
    if(NULL == ptGroupIn || NULL == ptGroupOut)
    {
        return;
    }

    TMULTISETENTRYINFO tGroupEntryInfo;
    memset(&tGroupEntryInfo, 0, sizeof(tGroupEntryInfo));
    strcpy(tGroupEntryInfo.achGroupName, ptGroupIn->GetGroupName());

    ptGroupOut->SetEntryIndex(ptGroupIn->GetEntryIdx());
    ptGroupOut->SetEntryInfo(&tGroupEntryInfo);
        					    
    u32 adwEntryIdx[MAXNUM_CONF_MT];
    u32 dwEntryNum = MAXNUM_CONF_MT/*sizeof(adwEntryIdx)*/;		
    ptGroupIn->GetAllEntryIdx(adwEntryIdx, dwEntryNum);

    ptGroupOut->SetListEntryNum(dwEntryNum);
    ptGroupOut->SetListEntryIndex(adwEntryIdx, dwEntryNum);

    return;
}

/*====================================================================
    ������      DaemonProcIntTestUserInfo
    ����        �������û������ɲ�����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CMessage * pcMsg, �������Ϣ
    ����ֵ˵��  ���ɹ�����TRUE�������¼��ն�δ�ǼǷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/28    1.0         LI Yi         ����
====================================================================*/
void CMcsSsnInst::DaemonProcIntTestUserInfo( CMessage *pcMsg )
{
	u8 buf[33] = {"0"};
	u8 byResult = 255;
	u16 wError;
	memcpy( buf, pcMsg->content, 33 );
	switch( buf[32] ) 
	{
	case 1: //����û��Ƿ����
		{
			CLoginRequest cLogin;
			cLogin.SetName( (char*)buf );
			cLogin.SetPassword( "~" );
			g_cUsrManage.CheckLoginValid( &cLogin );
			wError = g_cUsrManage.GetLastError();
			if ( wError == UM_LOGIN_ERRPSW )
				byResult = 1;
			else
				byResult = 0;
		}
		break;
	case 2:
		break;
	default :
		break;
	}
	SetSyncAck( &byResult,sizeof(byResult) );
	return ;
}

/*=============================================================================
�� �� ���� DaemonProcNPlusUsrGrpNotif
��    �ܣ� N+1ע��ɹ���ͬ���û���Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/23  4.0			������                  ����
=============================================================================*/
void CMcsSsnInst::DaemonProcNPlusUsrGrpNotif( const CMessage * pcMsg )
{
     ProcNPlusGrpInfo(pcMsg);
     ProcNPlusUsrInfo(pcMsg);
     return;
}
/*=============================================================================
�� �� ���� DaemonProcAddrInitialized
��    �ܣ� �����ַ����ʼ�������Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2010/11/17  4.0			���㻪                 ����
=============================================================================*/
void CMcsSsnInst::DaemonProcAddrInitialized( const CMessage * pcMsg )
{
	if (!pcMsg) return;
	
	if(pcMsg->length == sizeof(CAddrBook*))
	{
		g_pcAddrBook = *(CAddrBook* *)(pcMsg->content);
		CMcsSsn::BroadcastToAllMcsSsn(MCU_ADDRBOOK_GETENTRYLIST_NOTIF);
		CMcsSsn::BroadcastToAllMcsSsn(MCU_ADDRBOOK_GETGROUPLIST_NOTIF);
	}
	else
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS,"[DaemonProcAddrInitialized]bad message body(reallenth:%d required lenth:%d)\n", pcMsg->length, sizeof(CAddrBook*));
	}

    return;
}

/*=============================================================================
�� �� ���� ProcNPlusGrpInfo
��    �ܣ� N+1�û��鴦����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/23  4.0			������                  ����
=============================================================================*/
void CMcsSsnInst::ProcNPlusGrpInfo( const CMessage * pcMsg )
{
    CUsrGrpsInfo *pcGrpsInfo = g_cMcuVcApp.GetUserGroupInfo();
    
    CServMsg cServMsg;
    cServMsg.SetEventId(MCU_NPLUS_USRGRPUPDATE_REQ);
    cServMsg.SetMsgBody((u8*)pcGrpsInfo, sizeof(CUsrGrpsInfo));
    
    //��Ϊ��mcu��srcidΪdaemonʵ������Ϊ����mcu�ع�ģʽ��srcidΪ����ʵ��
    OspPost(pcMsg->srcid, VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());    
    return;
}

/*=============================================================================
�� �� ���� ProcNPlusUsrInfo
��    �ܣ� N+1�û��û�������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/23  4.0			������                  ����
=============================================================================*/
void CMcsSsnInst::ProcNPlusUsrInfo( const CMessage * pcMsg )
{        
	u8 abyBuf[sizeof(CExUsrInfo) * USERNUM_PERPKT];   // CExUsrInfo size 249
    u8 byPackNum;
    s32 nUserNum = g_cUsrManage.GetUserNum();
	if (nUserNum % USERNUM_PERPKT > 0)
	{
		byPackNum = (u8)(nUserNum/USERNUM_PERPKT+1);
	}
	else
	{
		byPackNum = (u8)(nUserNum/USERNUM_PERPKT);
	}

    CServMsg cServMsg;
    cServMsg.SetEventId(MCU_NPLUS_USRINFOUPDATE_REQ);
    cServMsg.SetTotalPktNum(byPackNum);

    u8 byUsrItr = 0;
    u8 byUserNumInPack = 0;
    for (u8 byPackIdx = 0; byPackIdx < byPackNum; byPackIdx++)
    {
        cServMsg.SetCurPktIdx(byPackIdx);
        if ( !g_cMcuVcApp.GetMCSCurUserList(USRGRPID_SADMIN, abyBuf, byUsrItr, byUserNumInPack) )
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcNPlusUsrInfo] GetMcuCurUserList Failed at index: %d\n", byUsrItr);
            return;
		}
        cServMsg.SetMsgBody(abyBuf, byUserNumInPack * sizeof(CExUsrInfo));
        
        //��Ϊ��mcu��srcidΪdaemonʵ������Ϊ����mcu�ع�ģʽ��srcidΪ����ʵ��
        OspPost(pcMsg->srcid, VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
    }	
    
    return;
}

// ɾ���û�
void CMcsSsnInst::ProcMcuDelUser( CMessage * const pcMsg )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );

    s8* szName = (s8*)cServMsg.GetMsgBody();

    if ( strncmp(szName, m_achUser, MAX_CHARLENGTH) == 0 )
    {
        // ������Լ���Ͽ���MCS������
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcMcuDelUser] My Ssn User %s has been deleted, Disconnect with MCS!\n ", m_achUser );
        
        OspDisconnectTcpNode(m_dwMcsNode);
    }

}
#if defined(_8KH_) ||  defined(_8KE_) || defined(_8KI_)
/*====================================================================
������         ProcMcsMcuStartNetCapCMD
����        �� 
�㷨ʵ��    �� ����MCS��ʼץ������
����ȫ�ֱ�����
�������˵���� 
����ֵ˵��  �� void
----------------------------------------------------------------------
�޸ļ�¼    ��
 ��  ��        �汾        �޸���        �޸�����
2013/03/22     4.7         ������         ����
====================================================================*/
void CMcsSsnInst::ProcMcsMcuStartNetCapCMD(  const CMessage * pcMsg ) 
{
	CServMsg cServMsg(pcMsg->content,pcMsg->length);

	//��ʼץ��
	if ( !g_cMcuVcApp.StartNetCap((u8)GetInsID()) )
	{
		cServMsg.SetErrorCode(1);
		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS,"[ProcMcsMcuStartNetCapCMD]ERROR,StartNetCap failed problely curnetcapstate is not idle!\n");
		//error notify
		//CServMsg cServMsg;
		cServMsg.SetEventId(MCU_MCS_ALARMINFO_NOTIF);
		cServMsg.SetErrorCode(ERR_MCU_STARTNETCAP_ALREADYSTART);
		SendMsgToMcs( MCU_MCS_ALARMINFO_NOTIF, cServMsg.GetMsgBody(),cServMsg.GetMsgBodyLen());
		return;
	}
	//2��֪ͨ��ǰץ��״̬
	NotifyAllMcsCurNetCapStatus();

}

/*====================================================================
������         ProcMcsMcuStoptNetCapCmd
����        �� 
�㷨ʵ��    �� ����MCSֹͣץ��
����ȫ�ֱ�����
�������˵���� 
����ֵ˵��  �� void
----------------------------------------------------------------------
�޸ļ�¼    ��
 ��  ��        �汾        �޸���        �޸�����
2013/03/22     4.7         ������         ����
====================================================================*/
void CMcsSsnInst::ProcMcsMcuStoptNetCapCmd(  const CMessage * pcMsg ) 
{
	CServMsg cServMsg(pcMsg->content,pcMsg->length);
	//1��ֹͣץ��
	if (!g_cMcuVcApp.StopNetCap())
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS,"[ProcMcsMcuStoptNetCapCmd]StopNetCap failed problely CurNetCapStatus is not start,return!\n");
		//error notify
		CServMsg cServMsg;
		cServMsg.SetEventId(MCU_MCS_ALARMINFO_NOTIF);
		cServMsg.SetErrorCode(ERR_MCU_STOPNETCAP_NOTSTARTYET);
		SendMsgToMcs( MCU_MCS_ALARMINFO_NOTIF, cServMsg.GetMsgBody(),cServMsg.GetMsgBodyLen());
		return;
	}
	//2��״̬�ϱ�
	NotifyAllMcsCurNetCapStatus();

}

/*====================================================================
������         ProcMcsMcuGetNetCapStatusReq
����        �� 
�㷨ʵ��    �� ����MCS��ȡ��ǰץ��״̬����
����ȫ�ֱ�����
�������˵���� 
����ֵ˵��  �� void
----------------------------------------------------------------------
�޸ļ�¼    ��
 ��  ��        �汾        �޸���        �޸�����
2013/03/22     4.7         ������         ����
====================================================================*/
void CMcsSsnInst::ProcMcsMcuGetNetCapStatusReq(  const CMessage * pcMsg ) 
{
	CServMsg cServMsg(pcMsg->content,pcMsg->length);
	emNetCapState emCurNetCapState = g_cMcuVcApp.GetNetCapStatus();
	u8 byCurState = (u8)emCurNetCapState;
	cServMsg.SetMsgBody(&byCurState,sizeof(u8));
	SendMsgToMcs( pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), FALSE );
}
/*====================================================================
������         NotifyAllMcsCurNetCapStatus
����        �� 
�㷨ʵ��    �� ֪ͨ����MCS��ǰץ��״̬
����ȫ�ֱ�����
�������˵���� 
����ֵ˵��  �� void
----------------------------------------------------------------------
�޸ļ�¼    ��
 ��  ��        �汾        �޸���        �޸�����
2013/03/22     4.7         ������         ����
====================================================================*/
void CMcsSsnInst::NotifyAllMcsCurNetCapStatus( )
{
	CServMsg cServMsg;
	emNetCapState emCurNetCapState = g_cMcuVcApp.GetNetCapStatus();
	u8 byCurState = (u8)emCurNetCapState;
	cServMsg.SetMsgBody(&byCurState,sizeof(u8));
	cServMsg.SetMcuId( LOCAL_MCUID );
	CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_NETCAPSTATUS_NOTIFY,cServMsg.GetServMsg(),cServMsg.GetServMsgLen());
} 

/*====================================================================
������         ProcMcsMcuOpenDebugModeCmd
����        �� 
�㷨ʵ��    �� ����MCS��������ģʽ����
����ȫ�ֱ�����
�������˵���� 
����ֵ˵��  �� void
----------------------------------------------------------------------
�޸ļ�¼    ��
 ��  ��        �汾        �޸���        �޸�����
2013/10/08     4.7.3         �ܾ���         ����
====================================================================*/
void CMcsSsnInst::ProcMcsMcuOpenDebugModeCmd( const CMessage* pcMsg )
{
	//CServMsg cServMsg(pcMsg->content,pcMsg->length);
	
	
	//u8 byMode = *(u8*)cServMsg.GetMsgBody();

	/*if( emDebugModeftp != byMode || emDebugModeDebug != byMode )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MCS,"[ProcMcsMcuOpenDebugModeCmd]error mode.%d\n",byMode );
		return;
	}*/
	
#ifdef _LINUX_
	/*if( emDebugModeftp == byMode )
	{		
		//SetFtpUp();
	}
	else */
	//if( emDebugModeDebug == byMode )
	//{
	SetTelnetUp();
	SetSshUp();
	//SetFtpUp();
	//}
#endif
	g_cMcsSsnApp.SetCurrentDebugMode( emDebugModeDebug );
	g_cMcsSsnApp.SetCurrentDebugModeInsId( (u8)GetInsID() );

	NotifyAllMcsCurDebugMode();	
}

/*====================================================================
������         ProcMcsMcuStopDebugModeCmd
����        �� 
�㷨ʵ��    �� ֹͣ����ģʽ
����ȫ�ֱ�����
�������˵���� 
����ֵ˵��  �� void
----------------------------------------------------------------------
�޸ļ�¼    ��
 ��  ��        �汾        �޸���        �޸�����
2013/10/08     4.7.3        �ܾ���         ����
====================================================================*/
void CMcsSsnInst::ProcMcsMcuStopDebugModeCmd( const CMessage* pcMsg )
{	
	g_cMcsSsnApp.SetCurrentDebugMode( emDebugModeNone );
	g_cMcsSsnApp.SetCurrentDebugModeInsId( 0 );
	
#ifdef _LINUX_
	SetTelnetDown();
	SetSshDown();
	//SetFtpDown();
#endif
	NotifyAllMcsCurDebugMode();
}

/*====================================================================
������         NotifyAllMcsCurDebugMode
����        �� 
�㷨ʵ��    �� ֪ͨmcs��ǰ����ģʽ
����ȫ�ֱ�����
�������˵���� 
����ֵ˵��  �� void
----------------------------------------------------------------------
�޸ļ�¼    ��
 ��  ��        �汾        �޸���        �޸�����
2013/10/08     4.7.3        �ܾ���         ����
====================================================================*/
void CMcsSsnInst::NotifyAllMcsCurDebugMode()
{
	CServMsg cServMsg;
	emDebugMode emCurDebugMode = g_cMcsSsnApp.GetCurrentDebugMode();
	u8 byCurMode = (u8)emCurDebugMode;
	cServMsg.SetMsgBody(&byCurMode,sizeof(u8));
	cServMsg.SetMcuId( LOCAL_MCUID );
	CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_OPENDEBUGMOED_NOTIFY,cServMsg.GetServMsg(),cServMsg.GetServMsgLen());
}

/*====================================================================
������         ProcMcsMcuGetDebugModeReq
����        �� 
�㷨ʵ��    �� mcs��ȡ����ģʽ״̬
����ȫ�ֱ�����
�������˵���� 
����ֵ˵��  �� void
----------------------------------------------------------------------
�޸ļ�¼    ��
 ��  ��        �汾        �޸���        �޸�����
2013/10/08     4.7.3        �ܾ���         ����
====================================================================*/
void CMcsSsnInst::ProcMcsMcuGetDebugModeReq(const CMessage* pcMsg )
{
	CServMsg cServMsg(pcMsg->content,pcMsg->length);
	emDebugMode emCurDebugMode = g_cMcsSsnApp.GetCurrentDebugMode();
	u8 byCurMode = (u8)emCurDebugMode;
	cServMsg.SetMsgBody(&byCurMode,sizeof(u8));
	SendMsgToMcs( pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), FALSE );
}
#endif

//END OF FILE
