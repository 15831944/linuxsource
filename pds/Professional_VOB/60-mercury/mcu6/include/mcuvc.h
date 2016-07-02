/*****************************************************************************
   ģ����      : mcu
   �ļ���      : mcuvc.h
   ����ļ�    : mcuvc.cpp
   �ļ�ʵ�ֹ���: MCUҵ��Ӧ����ͷ�ļ�
   ����        : ����
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2002/07/25  0.9         ����        ����
   2002/07/25  0.9         ����        ����
   2005/01/25  3.6         ����      MCU���ɲ��Խӿ�����
   2005/02/19  3.6         ����      �����޸ġ���3.5�汾�ϲ�
   2007/06/28  4.0         ����      �ඨ�������Ż�
   2008/11/18  4.5         �ű���      �����Ż���������HD���� 
******************************************************************************/

#ifndef __MCUVC_H_
#define __MCUVC_H_

#include "mcudata.h"
#include "agentinterface.h"
#include "nplusmanager.h"
#include "kdvlog.h"
#include "loguserdef.h"
#include "tvwall.h"
#include "evmcumcs.h"
#include "h460pinhole.h"

//��ʱ����(����)
const u32 TIMESPACE_SCHEDULED_CHECK		= 60 * 1000;	//��ʱ���ԤԼ�����Ƿ�ʱ
const u32 TIMESPACE_ONGOING_CHECK		= 60 * 1000;	//��ʱ��鼴ʱ�����Ƿ����
const u32 TIMESPACE_INVITE_UNJOINEDMT	= 30 * 1000;	//��ʱ����δ����ն�
const u32 TIMERSPACE_SEND_FASTUPDATEPIC = 2  * 1000;    //��ʱ�Զ�����ͼ��
const u32 TIMESPACE_SWITCHDELAY			= 500;			//�л���ʱ
const u32 TIMESPACE_WAIT_AUTOINVITE		= 50;			//�ȴ�MT����Ӧ��(��)
const u32 TIMESPACE_WAIT_MPREG  		= 5 * 1000;	    //�ȴ�MPע��
const u32 TIMESPACE_MCUSRC_CHECK        = 2 * 1000;     //MCUԴˢ��
const u32 TIMESPACE_CHANGEVMPPARAM      = 1 * 1000;     //�ȴ��ı仭�渴��ͨ��������ʱ��
const u32 TIMESPACE_RECREATE_DATACONF   = 5 * 1000;		//���´������ݻ���(ms)
const u32 TIMESPACE_WAIT_CASCADE_CHECK  = 20 * 1000;    //�ȴ�����ͨ����(ms)
const u32 TIMESPACE_UPADATE_REGSTATUS   = 2 * 60 * 1000;//ˢ�¸�������ʵ��ʱ����
const u32 TIMESPACE_WAIT_FSTVIDCHL_SUC	= 1 * 1000;		//�ȴ���һ·�߼�ͨ������ɣ��ٳ��Եڶ�·
const u32 TIMESPACE_WAIT_PLAY_SWITCHOK  = 500;          //�ȴ����󽻻�������ɺ�ʼ����(ms)
const u32 TIMESPACE_WAIT_ALLVMPPRESETIN_ACK = 1500;//�ȴ����б��λ���ϳ�presetinAck��ʱ����
const u32 TIMESPACE_WAIT_VMPRSP =	8*1000;	//�ȴ����б��λ���ϳɲ�������Ӧ��ʱ���

//Daemon��ӡ��Ϣ��ڲ���
const u16 DUMPMCINFO		= 1;
const u16 DUMPSRC			= 2;
const u16 DUMPEQPINFO		= 3;
const u16 DUMPEQPSTATUS		= 4;
const u16 DUMPTEMPLATE		= 5;
const u16 DUMPCONFMAP		= 6;

//�����ӡ��Ϣ��ڲ���
const u16 DUMPCONFALL       = 1;
const u16 DUMPCONFINFO      = 2;
const u16 DUMPCONFSWITCH    = 3;
const u16 DUMPCONFMT        = 4;
const u16 DUMPCONFEQP       = 5;
const u16 DUMPMTSTAT        = 6;
const u16 DUMPMTMONITOR     = 7;
const u16 DUMPBASINFO		= 8;
//const u16 DUMPADPGRPINFO	= 9;
//const u16 DUMPBASCHAN       = 10;
const u16 DUMPRLSCONF       = 11;
const u16 DUMPRLSBAS        = 12;
const u16 DUMPCHGMTRES      = 13;
const u16 DUMPVMPADAPT		= 14;
const u16 DUMPMULTISPY		= 15;
const u16 DUMPCONFMIX		= 16;
const u16 DUMPSPEAKLIST		= 17;
const u16 DUMPPRTSPEAKLIST  = 18;
const u16 DUMPNPRTSPEAKLIST	= 19;
const u16 DUMPBASLIST		= 20;
const u16 DUMPCONFMCU		= 21;
const u16 DUMPTWINFO	    = 22;
const u16 DUMPPRSINFO	    = 23;
const u16 DUMPSMSTIMER		= 25;
const u16 DUMPTOKENOWNER    = 26;
// [12/7/2010 liuxu]��Ӱ��չ��ܴ�ӡ
const u16 DUMPFUNCINFO		= 24;
const u16 DUMPMCOTHERTABLE	= 27;
const u16 DUMPCFGEXTRADATA	= 28;


#define IS_PTR_NULL(ptr)        ((ptr == NULL)? TRUE:FALSE)
#define MCU_SAFE_DELETE(ptr)	{if(ptr){delete ptr;ptr=NULL;}}

// ��Ϣ����ش�С(liuxu)
#define		MAX_MT_NUM_IN_BUF						(u32)100					
#define		MAX_MCU_MTLIST_BUF						(u32)( 1 + MAXNUM_CONFSUB_MCU)
// ��Ϣ����ʱ��(liuxu)
#define		TIMER_SPAN_SEND_MTSTATUS_MMCU			(s32)(1000)
#define		TIMER_SPAN_SEND_ALLMTLIST_ALLMCS		(s32)(1000)
#define		TIMER_SPAN_SEND_MTSTATUS_MCS			(s32)(1000)

// ���������ȫ������, len: 11K
class CMcuSingleConfAllInfo
{
public:
	enum _emSatReplaceType  //���ǻ��������ն�ҵ���������
	{
		emDefault = 0,            //Ĭ��
		emSpeaker,                //������
		emChairMan,               //��ϯ
		emStartVmp,               //��������ϳ�
		emVmpChnChange,           //����ϳ�ͨ���滻
		emStartPoll,	          //������ѵ
		emStartOrChangeHdu,       //HDU��أ��滻,��ͨ����ѵ
		emHduBatch,               //HDU������ѵ
		emSelect,                 //ѡ��
		emStartOrChangeMonitor,   //���
		emstartDs,                //˫��
		emStartRecord,            //¼��

		emSpeakerSrc,
		emChairSel, //��ϯѡ��
		emStopChairSel, //ֹͣ��ϯѡ��
		emPoll,
		emVmpBrd,	//Vmp�㲥����������������Ƿ��Ƿ�����
		emVmpNoBrd, //Vmp�ǹ㲥
		emVmpStop,	//ֹͣVMP������ȥ�жϵ�ǰVMP��Ա
		emTvWall,	//TvWall
	};
	typedef enum _emSatReplaceType emSatReplaceType;

    TConfMtTable      *m_ptMtTable;              //�����ն˱�      
    TConfSwitchTable  *m_ptSwitchTable;          //���齻����
    
    TConfInfo	     m_tConf;	                 //����������Ϣ
	TConfInfoEx		 m_tConfEx;					 //������չ��Ϣ��V4R7��ʼ֧�֣������������ѡ[12/26/2011 chendaiwei]
	
	TConfAllMcuInfo	 m_tConfAllMcuInfo;			 //��������mcu��Ϣ
	TConfAllMtInfo   m_tConfAllMtInfo;           //���������ն���Ϣ    
    TConfInnerStatus m_tConfInStatus;            //�����ڲ�״̬��ʶ
    TConfProtectInfo m_tConfProtectInfo;         //���鱣����Ϣ
    TConfPollParam   m_tConfPollParam;           //������ѯ����	
    //TTvWallPollParam m_tTvWallPollParam;       //����ǽ��ѯ����
	//THduPollParam    m_tHduPollParam;          //hdu��ѯ����
	//[03/04/2010] zjl add (����ǽ��ͨ����ѯ����ϲ�)
    TTWMutiPollParam m_tTWMutiPollParam;         //����ǽ��ͨ����ѯ��Ϣ(����tvwall��hdu)
    TConfEqpModule   m_tConfEqpModule;           //�����豸ģ��

    u8               m_byConfIdx;                //����idx
    TMt              m_tCascadeMMCU; 		     //�������ϼ�MCU
    u8               m_byCreateBy;               //��¼����Ĵ�����
private:    
    TMt     m_tVidBrdSrc;                   //��Ƶ�㲥Դ
    TMt     m_tAudBrdSrc;                   //��Ƶ�㲥Դ
protected:
    TConfOtherMcTable *m_ptConfOtherMcTable;     //����Mc�ն���Ϣ
	
	//u8  	m_abyMtCountPoint[MAXNUM_CONF_MT/8];	//�ն��ڽ������ĵ�����¼
public:
    TMt     m_tDoubleStreamSrc;             //˫��Դ ������Ϊ�˸�MpManager�ã�������public�Ľӿڴ��棡
protected:
	TMt		m_tSecVidBrdSrc;				//�ڶ���Ƶ�㲥Դ
	u8		m_byNewTokenReqMtid;			//��˫��ԴΪKEDAMCUʱ�������ն����뷢��˫����
											//��ʱ��¼һ�¸��նˣ����ϵ�KEDAMCU˫��Դ�����ͷ�����֪ͨ����˫��
    TH239TokenInfo m_tH239TokenOwnerInfo;   //H239˫������ӵ���ն���Ϣ
    
    TEqp    m_tVmpEqp;                      //����ϳ�����
    TEqp    m_tVmpTwEqp;                    //���ϵ���ǽ����
    TEqp	m_tMixEqp;                      //��������
    TEqp	m_tRecEqp;                      //¼������
    TEqp	m_tPlayEqp;                     //��������
    TPlayEqpAttrib m_tPlayEqpAttrib;        //������������
    TPlayFileMediaInfo m_tPlayFileMediaInfo;    //�����ļ�ý����Ϣ
    TRecStartPara m_tRecPara;               //¼�����
	TPlayFileHpAttrib m_tPlayFileHpAttrib;    //�����ļ�����Ƶ��HP/BP��Ϣ
    
//     TEqp	m_tAudBasEqp;					//��Ƶ��������
//     TEqp	m_tVidBasEqp;					//��Ƶ��������
//     TEqp	m_tBrBasEqp;					//������������
//     TEqp    m_tCasdAudBasEqp;               //������Ƶ�ش���������
//     TEqp    m_tCasdVidBasEqp;               //������Ƶ�ش���������
//    TEqp    m_tPrsEqp;                      //���ش�����
    
    u8      m_byMixGrpId;					//������ID
    u8		m_byRecChnnl;					//¼���ŵ���
    u8		m_byPlayChnnl;					//�����ŵ���
//     u8      m_byPrsChnnl;                   //���ش���һ��Ƶͨ��
//     u8      m_byPrsChnnl2;                  //���ش��ڶ���Ƶͨ��
//     u8      m_byPrsChnnlAud;                //���ش���Ƶͨ��
//     u8      m_byPrsChnnlAudBas;             //��Ƶ������ش�ͨ��
//     u8      m_byPrsChnnlVidBas;             //��Ƶ������ش�ͨ��
//     u8      m_byPrsChnnlBrBas;              //����������ش�ͨ��

// 	u8		m_byPrsChnnlVmpOut1;			//�㲥Դ1080�����ش�ͨ��
// 	u8		m_byPrsChnnlVmpOut2;				//�㲥Դ720�����ش�ͨ��
// 	u8		m_byPrsChnnlVmpOut3;			//�㲥Դ4Cif�����ش�ͨ��
// 	u8		m_byPrsChnnlVmpOut4;		//�㲥ԴCif��other�����ش�ͨ��
// 	u8		m_byPrsChnnlDsVidBas;			//˫��������ش�ͨ��	
	
//     u8      m_byAudBasChnnl;				//��Ƶ����ͨ����
//     u8      m_byVidBasChnnl;				//��Ƶ����ͨ����
//     u8      m_byBrBasChnnl;					//��������ͨ����
//     u8      m_byCasdAudBasChnnl;            //������Ƶ�ش�����ͨ����
//     u8      m_byCasdVidBasChnnl;            //������Ƶ�ش�����ͨ����
    u16     m_wVidBasBitrate;				//��Ƶ���������    
    u16     m_wBasBitrate;					//�������������
    //u16     m_awVMPBrdBitrate[MAXNUM_MPU2_OUTCHNNL];               //���ٻ���VMP�Ĺ㲥����
#ifdef _MINIMCU_
    u8      m_byIsDoubleMediaConf;          //�Ƿ���˫ý�����(�����ʲ�������ʽ�������ʲ��ø���ʽ
#endif

	u32		m_dwVmpLastVCUTick;				//��һ����VMP��������ؼ�֡����
	/*u32		m_dwVmpTwLastVCUTick;			//��һ����VMPTw��������ؼ�֡����
	u32		m_dwVidBasChnnlLastVCUTick;		//��һ����BAS����ͬͨ������������ؼ�֡����
	u32		m_dwAudBasChnnlLastVCUTick;
	u32		m_dwBrBasChnnlLastVCUTick;
	u32		m_dwCasdAudBasChnnlLastVCUTick;
	u32		m_dwCasdVidBasChnnlLastVCUTick;*/
    
    //TBasCapSet m_tBasCapSet;              //���������������    
    //u8      m_byRegGKDriId;               //������ע��GK��MtAdp(��ʵ�������л��鶼ͨ��������ע�ᣬ�õض����岻��
        
    TRefreshParam m_tRefreshParam;          //Mcsˢ����Ϣ����   

    u32     m_dwSpeakerViewId;				//���ڼ���ʱ�ķ�������ƵԴ
    u32     m_dwVmpViewId;					//���ڼ���ʱ����Ƶ������ƵԴ
    u32     m_dwSpeakerVideoId;				//���ڼ���ʱ�ķ�������ƵԴ����Ƶ����ID
    u32     m_dwVmpVideoId;					//���ڼ���ʱ����Ƶ������ƵԴ����Ƶ����ID
    u32     m_dwSpeakerAudioId;				//���ڼ���ʱ�ķ����˵���ƵID
    u32     m_dwMixerAudioId;				//���ڼ���ʱ�Ļ���������ƵID
    TMt     m_tLastSpyMt;                   //��һ�λش�ͨ�����նˣ���Ϊ�����Լ�(for h320 mcu cascade select)       
        
    u8		m_byDcsIdx;						//���ݻ��������id����(Dcs�Ự��ʵ����)

    u16		m_wRestoreTimes;		        //����ָ����Դ���	
                  
    TMt     m_tVacLastSpeaker;              //��������������Ա
    TMt     m_tLastSpeaker;                 //��һ�εķ����� 
    //TVMPParam	m_tLastVmpParam;			//��һ�λ���ϳɳ�Ա������Vmp��̬�ֱ��ʻָ�
    TVMPParam	m_tLastVmpTwParam;          //��һ��VMPTW��Ա������Vmp��̬�ֱ��ʻָ� 

	u8      m_byLastMixMode;                //����������ǰ�Ļ������ģʽ

    TAcctSessionId m_tChargeSsnId;          //������Ʒ�SessionId

    TConfPollSwitchParam m_tPollSwitchParam;           //Radvision��ʽ��ѯ�ش�����
    
	//tianzhiyong 2010/03/21  ԭ��APU�����56��������Ա��EAPU�¸�Ϊ64��
    u8      m_abyMixMtId[MAXNUM_MIXER_CHNNL + 1];      //���ڼ�¼����������ն˵�MtId 

//    u8      m_abyCasChnCheckTimes[MAXNUM_CONF_MT];     //�򿪼���ͨ���ĳ��Դ���    

    u8      m_abySerHdr[SERV_MSGHEAD_LEN];  //��¼�Ĵ�������ʱ����Ϣͷ
    TMt     m_tRollCaller;                  //������
    TMt     m_tRollCallee;                  //��������

	u8	    m_bySpeakerVidSrcSpecType;		//������Դ�Ƿ����ɻ����Զ�ָ��
	u8		m_bySpeakerAudSrcSpecType;
    
    //CRcvGrp m_cMtRcvGrp;                    //��������ն˽���Ⱥ�����
    //CBasMgr *m_pcBasMgr;                    //�������BASͨ����״̬����
    //CMcuBasMgr *m_pcMcuBasMgr;				//�������BASͨ����״̬����
	u8		m_byMtIdNotInvite;				// xliang [12/26/2008] Mt ������MCU��
											//ģ�崴�����ȱʡ��������£�
											//��Ǹ�MTԤ�Ȳ��������ն�

	//****************  VMP related  ****************
public:

protected:

	//--��������Ϣ�б�
	//TKDVVMPOutParam m_tVMPOutParam;
	//--seize related:
	//TVmpAdaptChnSeizeOpr	m_tVmpAdaptChnSeizeOpr;			//��¼��ռ������Ҫ�Ĳ���
	//TVmpChnnlInfo m_tVmpChnnlInfo;							// vmpͨ����Ϣ����Ҫ��¼VMPǰ�����Ա
	//TVMPMember	m_atVMPTmpMember[MAXNUM_MPUSVMP_MEMBER];	//��ռ����¼��
	//BOOL32	m_bNewMemberSeize;								//�����˴���ռ��������һ����Ա��������ռ����ĳ��Ա����ΪVIP��ݴ�������ռ
	u8		m_abyMtNeglectedByVmp[MAXNUM_CONF_MT/8];		//����Զ�vmpʱ����Ӧ�����MT// xliang [12/17/2008] Ŀǰ����, ��Ԥ��
	u8 m_abyVmpEqpId[MAXNUM_CONF_VMP];	//����ռ��vmp�ϳ����������������ռ��16��
	TEqp m_tModuleVmpEqp;
	
	//--speaker follow related:
	TLastVmpchnnlMemInfo m_tLastVmpChnnlMemInfo;			//���ڷ����˸��棬��¼ǰ�η�����ռ��ͨ����Ա�Ա�ָ�
	
	//--Batch poll related:
	TVmpBatchPollInfo	m_tVmpBatchPollInfo;				//vmp������ѯ��Ϣ // xliang [12/18/2008] 
	TVmpPollParam		m_tVmpPollParam;					//vmp������ѯ���� // xliang [12/18/2008]

	//--system performance related:
	u8		m_byVmpOperating;								//���VMP���ڲ���	

	//u8	m_abyMtVmpChnnl[MAXNUM_CONF_MT];				//tVMPMember��Ӧ��VMP chnnl����ռ�ɹ���,��ͨ�����ó�
	//TVmpCasMemInfo m_tVmpCasMemInfo;						//VMP ������Ա��Ϣ��¼��������SrcMt�������ο���	

	//********************** vmp related END *********************

    THduBatchPollInfo   m_tHduBatchPollInfo;    //hdu������ѯ��Ϣ
	THduPollSchemeInfo  m_tHduPollSchemeInfo;   //hdu������ѯԤ����Ϣ

	CVCCStatus  m_cVCSConfStatus;            // VCS����Ķ��صĻ���״̬
	TMtListInfo m_tOrginMtList;              // VCS�����������б�

    //TPrsInfoManage m_tConfPrsInfo;        // ����ʹ�õ�PRS��ϸ��Ϣ
	
	BOOL32	m_bMMcuSpeakerValid;			// xliang [10/10/2009] ������������£��ϼ���˫�����Ҷ��¼��ٺ��룬
											// �¼��Ƿ��ܱ���˫��ͨ�����ж�ͨ������ʱ������

	//zjj20091102 ����ģ�������Ϣ����(������Щ����TConfStore�ṹ�У���Ҫ��̬����������)
	//����ֻ�����vcs�����еĵ���ǽԤ��
	CConfInfoFileExtraDataMgr m_cCfgInfoRileExtraDataMgr;

	TApplySpeakQue m_tApplySpeakQue;        // �������뷢�Զ���

	//lukunpeng 2010/06/10 ���Ļش�������ı�������ע�ͣ��Ա�����
	//��Ҫ��¼���ػش����ϼ��Ļش���Ա
	CConfSendSpyInfo		m_cLocalSpyMana;		// ���ڼ�¼��������ش����ϼ�mcu�Ļش���Ա��ռ�õ´�����Ϣ
	//��Ҫ��¼�¼��ش��������¼��ն���Ϣ
	CConfRecvSpyInfo		m_cSMcuSpyMana;			// ���ڼ�¼�¼�����ش����¼���Ա��ռ�õĴ�����Ϣ
	TMt     m_atMixMt[MAXNUM_MIXER_CHNNL+1];         //���ڼ�¼����������ն˵�����tmt��Ϣ��δ��local����
//	CMcChnMgr               m_cMcChnnlMgr;      // ��¼��ؼ��ͨ����Ϣ

	u8 m_byWaitStartVmp;

	CRecAdaptMgr m_cRecAdaptMgr;		//����¼�����
	
	u8      m_byOtherConfMtId;

	// [1/7/2011 xliang] add smscontrol
	CSmsControl m_cSmsControl;					//���Ͷ���Ϣ����
	
	// [9/27/2011 liuxu] �������ն�״̬���ն˻����
	CStaticBuf<TMt, MAX_MT_NUM_IN_BUF>		m_cMtStatusSendBuf;
	
	// [11/14/2011 liuxu] ���巢��AllMtInfoToAllMcs
	u8		m_byAllMtToAllMcsTimerFlag;
	CStaticBuf<u16, MAX_MCU_MTLIST_BUF>		m_cMcuMtListSendBuf;

	u8		m_byMtStatusToMMcuTimerFlag;
	CStaticBuf<u8,	MAXNUM_CONF_MT>			m_cLocalMtStatusToMMcuBuf;
	CStaticBuf<u16, MAXNUM_CONFSUB_MCU>		m_cSMcuMtStatusToMMcuBuf;

	u32 m_dwBrdFastUpdateTick;

	u8 m_byFastNeglectNum;

	u32 m_dwRealSndSpyBandWidth;

	//���һλ��ʾ�Ƿ��ڵȴ����巢�������ݣ������ݲ��������壨�����״̬Ӧ��ΪFALSE�������յ�instancestate�󣬷�IDLE��������ΪTRUE�������������꣬��ΪFALSE��
	//��͵ڶ�λ��ʾ�򵥼�������ʱ�Ƿ��Ѿ�������mcu������뱾���𣬺��뱾��Ļ����ڱ������е�mtid�ñ���m_byOtherConfMtId��ʾ
	u8          m_byConfFlag;
	s16         m_swCurSatMtNum;       //��ǰʣ��Ŀ������ǵ���

private:
	CMcuBasMgr     *m_pcMcuBasMgr;				//�������BASͨ����״̬����
	TPrsInfoManage m_tConfPrsInfo;              // ����ʹ�õ�PRS��ϸ��Ϣ

public:

	 u16   GetDoubleStreamVideoBitrate(u16 wDialBitrate, BOOL32 bFst = TRUE); //��ȡ˫����Ƶ���ʣ�Ĭ�ϵ�һ·
	/*---------------------------bas��������ؽӿ�------------------------*/
	//����bas����ռ�
	BOOL32 NewMcuBasMgr();
	//����bas����ռ�
	void   DeleteMcuBasMgr();
	//��ȡbas����������
	u32    GetBasMgrBuf(u8 *pbyBuf);
	//����bas����������
	u32    SetBasMgrBuf(u8 *pbyBuf);
	//���bas����������
	void   ClearBasMgr();

	//Ϊ�㲥���ҿ���ͨ��
	BOOL32	FindBasChnByCapForVidBrd(TVideoStreamCap &tSimCapSet, u8 byMediaMode, TBasOutInfo &tOutInfo, BOOL32 bIsExactMatch = FALSE);
	BOOL32	FindBasChnForAudBrd(TAudioTypeDesc &tAudCap,TBasOutInfo &tOutInfo);
	//Ϊѡ�����ҿ���ͨ��
	BOOL32	FindBasChnByVidCapForSel(TMt &tSrc,TVideoStreamCap &tDstSimCapSet, u8 byMediaMode, TBasOutInfo &tOutInfo);
	BOOL32	FindBasChnByAudCapForSel(TMt &tSrc,TAudioTypeDesc &tDstAudCap, TBasOutInfo &tOutInfo);

	//��ȡ�㲥Դ
	TMt	   GetBrdSrc(u8 byMediaMode);
	//1.�㲥����
	//�㲥���䣺 ������Դ����������
	BOOL32 AssignBasBrdChn(CBasChn **aptBasChn, TBasChnCapData *atBasChnData, u8 byChnNum,const CConfId &cConId);	
	//�㲥���䣺 ����ͨ���ӹ㲥���Ƴ�
	BOOL32 ReMoveBasBrdChn(const TEqp &tEqp, u8 byChnId);
	//�㲥���䣺��ù㲥ͨ��
	BOOL32 GetBasBrdChnGrp(u8 &byNum, CBasChn **pcBasChn, u8 byMediaMode);	
	//�㲥���䣺 ��ȡ���й㲥��ͨ��
	BOOL32 GetBasAllBrdChn(u8 &byNum, CBasChn **pcBasChn);	
	//�㲥���䣺 ��ȡ�㲥����ռ�õĲ�ͬ��������Ϣ
	BOOL32 GetDiffBrdEqp(u8 byMediaMode, u8 &byEqpNum, TEqp *ptBas);
	//�㲥���䣺ˢ�¹㲥������������˫other�ĵ��ٻ��������Ƶ����
	BOOL32	RefreshSpecialCapForVidBrd(TConfInfo &tConfInfo,TVideoStreamCap &tSimCapSet, u8 byMediaMode);	
	BOOL32  RefreshSpecialCapForAudBrd(TConfInfo &tConfInfo,TConfInfoEx &tConfInfoEx, TAudioTypeDesc &tSrcAudCap);
	//�㲥���䣺ˢ�¹㲥����
	BOOL32	RefreshBasBrdParam(u8 byMediaMode,u16 wBiteRate);

	//2.ѡ������
	//ѡ�����䣺������Դ����ѡ����
	BOOL32 AssignBasSelChn(CBasChn *pcBasChn,const TBasChnCapData &tBasChnIdxParam, const CConfId &cConId);
	//ѡ�����䣺������Դ��ѡ����ɾ��
	BOOL32 ReMoveBasSelChn(const TEqp &tEqp, u8 byChnId);
	//ѡ�����䣺��Ӳ���
	BOOL32 AddBasSelChnParam(const TEqp &tEqp, u8 byChnId, const  TBasChnCapData &tBasChnIdxParam);
	//ѡ�����䣺��ȡ��Ӧģʽ��ͨ��
	BOOL32 GetBasSelChnGrp(u8 &byNum, CBasChn **pcBasChn, u8 byMediaMode);
	//ѡ�����䣺�²�
	BOOL32 SetBasSelChnParam(const TEqp &tEqp, u8 byChnId, u8 byMediaMode, THDAdaptParam &tBasParam);
	BOOL32 SetBasSelAudChnParam(const TEqp &tEqp, u8 byChnId, TAudAdaptParam &tBasParam);
	//ѡ�����䣺 ��ȡ���й㲥��ͨ��
	BOOL32 GetBasAllSelChn(u8 &byNum, CBasChn **pcBasChn);	
	
	//��������
	//��ȡ��Ӧͨ��������
	u8     GetBasGrpType(const TEqp &tEqp, u8 byChnId);	
	//���ö�Ӧͨ��ģʽ
	BOOL32 SetBasChnMode(const TEqp &tEqp, u8 byChnId, u8 byMediaMode);
	//��ȡ��Ӧͨ��ģʽ
	u8     GetBasChnMode(const TEqp &tEqp, u8 byChnId);	
	//�ж�BAS�����Ƿ���ͨ���ڸû�����
	BOOL32 IsExistChnOnEqp(const TEqp &tEqp);
	//����ͨ��Դ��Ϣ
	BOOL32 SetBasChnSrc(const TEqp &tEqp, u8 byChnId, TMt tSrc);
	//��ȡͨ��Դ��Ϣ
	TMt    GetBasChnSrc(const TEqp &tEqp, u8 byChnId);	
	//����tick
	BOOL32 SetBasVcuTick(const TEqp &tEqp, u8 byChnId, u32 dwTick);
	//��ȡtick
	u32    GetBasVcuTick(const TEqp &tEqp, u8 byChnId);	
	//��ȡָ��ͨ��������(���ڶ�ʱ)
	u8     GetBasPosition(const TEqp &tEqp, u8 byChnId);	
	//��ӡ
	void   PrintBas();
	
	//ˢ��ͨ��ĳһ��������֡��������ˢ��
	BOOL32	RefreshBasParambyOutIdx(TBasOutInfo &tBasOutInfo, THDAdaptParam tBasParam);

	//����ĳ��ͨ��������
	BOOL32	StartBasChnAdapt(const TEqp &tEqp, u8 byChnId, TBasAdaptParamExt &tBasParamExt);

	//ֹͣĳ��ͨ��������
	BOOL32	StopBasChnAdapt(const TEqp &tEqp, u8 byChnId);

	//���ĳ��ͨ�������·��
	BOOL32	GetBasChnOutputNum(const TEqp &tEqp, u8 byChnId, u8 &byOutNum, u8 &byFrontOutNum);

	//��ö�Ӧͨ��������
	BOOL32	GetBasResDataByEqp(TEqp tDiscBas, TNeedVidAdaptData *patBasResData);

	BOOL32 GetBasResAudDataByEqp(TEqp tDiscBas, TNeedAudAdaptData *patBasResData);

	/*-----------------------------------------------------------------------*/

	/*--------------------------Prs��ؽӿ�----------------------------------*/
	//����Prsͨ��
	BOOL32 AssignPrsChn(u8 byPrsId, u8 byPrsChnlId, u8 byMediaMode,  BOOL bBrdChn = FALSE);
	//�Ƴ�prsͨ��
	BOOL32 RemovePrsChn(u8 byPrsId, u8 byPrsChnId);

	//�㲥PRSͨ���Ƿ��ѷ���
	BOOL32 IsBrdPrsAssigned();
	//Ϊָ��Prsͨ������Դ��Ϣ
	BOOL32 SetPrsChnSrc(u8 byPrsId, u8 byPrsChnId, const TMt &tPrsSrc, u8 bySrcOutChn);
	//����ָ���㲥ý��ģʽ��ȡԤռ��prsͨ��
	BOOL32 FindPrsChnForBrd(u8 byBrdMode, u8 &byPrsId, u8 &byPrsChnId);
	//Ϊָ��ԴѰ�Ҷ�Ӧռ�õ�prsͨ��
	BOOL32 FindPrsChnForSrc(const TMt& tPrsSrc, u8 bySrcOutChnl, u8 byMode, u8 &byPrsId, u8 &byPrsChnlId);
	//Ϊָ��prsͨ��Ѱ�Ҷ�Ӧ��Դ��Ϣ
	BOOL32 FindPrsChnlSrc(u8 byPrsId, u8 byPrsChnlId, u8& byMediaMode, TMt& tPrsSrc, u8& bySrcOutChnl);
	//��ȡָ��prs�����ڵ�ǰ����ʹ�õ�����ͨ��
	u8 GetSpecPrsIdAllChnForConf(u8 byPrsId, u8 *pbyPrsChnId);
	//��ȡ��ǰ����ռ�õ�����prsͨ��
	u8 GetConfAllPrsChn(u8 *pbyPrsId, u8 *pbyChnId);
	//��ȡĳ��prsͨ����ý��ģʽ
	u8 GetPrsChnMediaMode(u8 byPrsId, u8 byPrsChnId);
	//��ǰprsͨ���Ƿ��ѿ���
	BOOL32 IsPrsChnStart(u8 byPrsId, u8 byPrsChnId);
	BOOL32 SetPrsChnStart(u8 byPrsId, u8 byPrsChnId, BOOL32 bStart);
	//��ȡPrs����������
	u32    GetPrsMgrBuf(u8 *pbyBuf);
	//����Prs����������
	u32    SetPrsMgrBuf(u8 *pbyBuf);
	//��ӡ
	void PrintPrs();
	/*-----------------------------------------------------------------------*/


	TMt GetVidBrdSrc();
	TMt GetSecVidBrdSrc();
	TMt GetLocalVidBrdSrc();
	void SetVidBrdSrc(TMt& tMt);
	void SetVidBrdSrcNull();
	BOOL32 IsVidBrdSrcNull();

	TMt GetAudBrdSrc();
	TMt GetLocalAudBrdSrc();
	void SetAudBrdSrc(TMt& tMt);
	void SetAudBrdSrcNull();
	BOOL32 IsAudBrdSrcNull();

    BOOL32 IsMultiCastMt( u8 byMtId ); //���ǻ���ר��
	BOOL32 IsSatMtSend(const TMt &tMt ); // [pengguofeng 2/18/2013]�ն��ǲ����Ѿ��ڷ�����

	TMt GetLocalMtFromOtherMcuMt(const TMt& tMt);
	TMt GetMcuIdxMtFromMcuIdMt( const TMt& tMt );
	TMt GetMcuIdMtFromMcuIdxMt( const TMt &tMt );
	u8  GetFstMcuIdFromMcuIdx( u16 wMcuIdx );
	u16 GetMcuIdxFromMcuId( u8 byMcuId );
	u16 GetMcuIdxFromMcuId( u8 *pMcuId,u8 byLevel = 1 );

	TMultiCacMtInfo BuildMultiCascadeMtInfo( const TMt &tMt,TMt &tNormalMt );

	TMt GetMtFromMultiCascadeMtInfo( const TMultiCacMtInfo &tMultiCacMtInfo,const TMt &tMt );

	//liuxu, ��ȡm_ptConfOtherMcTable
	TConfOtherMcTable * GetConfOtherMc() { return m_ptConfOtherMcTable;}

	TEqp GetVmpEqp() { return m_tVmpEqp; }

	// [9/17/2010 liuxu] ���һ�������ն�¼���ն˵Ľӿ�
protected:
	/** 
	/* @����˵���������ն�tMt��¼��״̬�����ն˿����Ǳ����նˣ�Ҳ�����Ǽ����ն�
	/* @����˵����
	/*         	 tMt : �ն˵�TMt�ṹ
	/*           bRecording �� �Ƿ�¼��Ĭ����¼��
	/* @����ֵ����
	/* @���� �� ����       
	/* @���ڣ� 2010-09-17
	*/
	void SetMtRecording(const TMt& tMt, BOOL32 bRecording = TRUE);

	virtual ~CMcuSingleConfAllInfo( ) { }
public:
// 	BOOL32 IsMtCountExtraPoint(u8 byMtId) const;
// 	BOOL32 AddMtExtraPoint(u8 byMtId);
// 	BOOL32 DelMtExtraPoint(u8 byMtId);

	/*====================================================================
	������      ��IsMSSupportCapEx
	����        ���ж������Ƿ�֧��ĳ��չ������
	����ȫ�ֱ�����
	�������˵����[IN]byRes �ֱ���
	[IN]byFps ֡��,Ĭ��Ϊ0�������Ƚϸ���
	[IN]emProfileAttrb eProf HP/BP����,Ĭ��Ϊ0�������Ƚϸ���
	[IN]u16 wMaxBitRate ���� Ĭ��Ϊ0�������Ƚϸ���
	����ֵ˵��  ��֧�ַ���TRUE�����򷵻�FALSE��
	----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	2011/11/28  4.0         chendaiwei       ����
	====================================================================*/
	BOOL32 IsMSSupportCapEx(u8 byRes, u8 byFps = 0, emProfileAttrb eProf = emBpAttrb, u16 wMaxBitRate = 0)
	{
		BOOL32 bResult = FALSE;

		TVideoStreamCap tCap[MAX_CONF_CAP_EX_NUM];
		u8 byCapNum = MAX_CONF_CAP_EX_NUM;
		m_tConfEx.GetMainStreamCapEx(&tCap[0],byCapNum);
		
		// ֻ���ҷֱ��� [12/27/2011 chendaiwei]
		if(byFps == 0)
		{
			for(u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++ )
			{
				if(tCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL)
				{
					break;
				}
				
				if(tCap[byIdx].GetResolution() == byRes )
				{
					bResult = TRUE;
					break;
				}
			}
		}
		//��������
		else if(wMaxBitRate == 0)
		{
			for(u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++ )
			{
				if(tCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL)
				{
					break;
				}
				
				if( tCap[byIdx].GetResolution() == byRes 
					&& tCap[byIdx].GetH264ProfileAttrb() == eProf 
					&& tCap[byIdx].GetUserDefFrameRate() == byFps)
				{
					bResult = TRUE;
					break;
				}
			}
		}
		//��ȫƥ��
		else
		{
			for(u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++ )
			{
				if(tCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL)
				{
					break;
				}
				
				if( tCap[byIdx].GetResolution() == byRes 
					&& tCap[byIdx].GetH264ProfileAttrb() == eProf 
					&& tCap[byIdx].GetUserDefFrameRate() == byFps
					&& tCap[byIdx].GetMaxBitRate() == wMaxBitRate)
				{
					bResult = TRUE;
					break;
				}
			}
		}
		
		return bResult;
	}

	/*====================================================================
	������      ��IsDSSupportCapEx
	����        ���ж�˫���Ƿ�֧��ĳ��չ������
	����ȫ�ֱ�����
	�������˵����[IN]byRes �ֱ���
	[IN]byFps ֡��,Ĭ��Ϊ0�������Ƚϸ���
	[IN]emProfileAttrb eProf HP/BP����,Ĭ��Ϊ0�������Ƚϸ���
	[IN]u16 wMaxBitRate ���� Ĭ��Ϊ0�������Ƚϸ���
	����ֵ˵��  ��֧�ַ���TRUE�����򷵻�FALSE��
	----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	2011/11/28  4.0         chendaiwei       ����
	====================================================================*/
	BOOL32 IsDSSupportCapEx(u8 byRes, u8 byFps = 0, emProfileAttrb eProf = emBpAttrb, u16 wMaxBitRate = 0)
	{
		BOOL32 bResult = FALSE;
		
		u8 byCapNum = MAX_CONF_CAP_EX_NUM;
		TVideoStreamCap tCap[MAX_CONF_CAP_EX_NUM];
		m_tConfEx.GetDoubleStreamCapEx(&tCap[0],byCapNum);
		
		// ֻ���ҷֱ��� [12/27/2011 chendaiwei]
		if(byFps == 0)
		{
			for(u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++ )
			{
				if(tCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL)
				{
					break;
				}
				
				if(tCap[byIdx].GetResolution() == byRes )
				{
					bResult = TRUE;
					break;
				}
			}
		}
		//��������
		else if(wMaxBitRate == 0)
		{
			for(u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++ )
			{
				if(tCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL)
				{
					break;
				}
				
				if( tCap[byIdx].GetResolution() == byRes 
					&& tCap[byIdx].GetH264ProfileAttrb() == eProf 
					&& tCap[byIdx].GetUserDefFrameRate() == byFps)
				{
					bResult = TRUE;
					break;
				}
			}
		}
		//��ȫƥ��
		else
		{
			for(u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++ )
			{
				if(tCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL)
				{
					break;
				}
				
				if( tCap[byIdx].GetResolution() == byRes 
					&& tCap[byIdx].GetH264ProfileAttrb() == eProf 
					&& tCap[byIdx].GetUserDefFrameRate() == byFps
					&& tCap[byIdx].GetMaxBitRate() == wMaxBitRate)
				{
					bResult = TRUE;
					break;
				}
			}
		}
		
		return bResult;
	}

	/*====================================================================
	������      ��FindMSMatchedCapEx
	����        ������С�ڷ�ֵ�ֱ��ʵ���������ѡ
	����ȫ�ֱ�����
	�������˵����[IN][OUT]TVideoStreamCap &tDstCapEx
				  [IN] TVideoStreamCap tSrcCap Դ������
				  [IN]u8 byRes ��ֵ�ֱ���
					
	����ֵ˵��  �����ҳɹ�����TRUE�����򷵻�FALSE��
	----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	2011/11/28  4.0         chendaiwei       ����
	====================================================================*/
	BOOL32 FindMSMatchedCapEx(TVideoStreamCap &tDstCapEx, TVideoStreamCap tSrcCap, u8 byRes, u8 byFps)
	{
		BOOL32 bResult = FALSE;
		TVideoStreamCap tCap[MAX_CONF_CAP_EX_NUM];
		u8 byCapNum = MAX_CONF_CAP_EX_NUM;
		m_tConfEx.GetMainStreamCapEx(tCap,byCapNum);

		for(u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++)
		{
			if( tCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL )
			{
				break;																	
			}
			/*
			//1)�ȷ�ֵ�ֱ��ʸ�С����Դ֡�ʸ�С����ȣ�HP/BP������ͬ
			//2)�뷧ֵ�ֱ�����ͬ����Դ֡�ʸ�С��HP/BP������ͬ
			//3)Դ1080/30/25 ��ѡ����720/60�������
			if( ((tCap[byIdx].GetResolution() == byRes 
				  && byRes == VIDEO_FORMAT_HD1080
				  && tCap[byIdx].GetUserDefFrameRate() < tSrcCap.GetUserDefFrameRate()  )
				  ||
				  (tCap[byIdx].IsResLowerWithOutMatched(tCap[byIdx].GetResolution(),byRes) 
				   && tCap[byIdx].GetUserDefFrameRate() <= tSrcCap.GetUserDefFrameRate() )
				  || 
				  (tCap[byIdx].GetResolution() == VIDEO_FORMAT_HD720 
				   && tCap[byIdx].GetUserDefFrameRate() >30
				   && tSrcCap.GetResolution() == VIDEO_FORMAT_HD1080
				   && tSrcCap.GetUserDefFrameRate() <= 30)
				)
				&& (tCap[byIdx].GetH264ProfileAttrb() == tSrcCap.GetH264ProfileAttrb())
			  )
			{
				tDstCapEx = tCap[byIdx];
				bResult = TRUE;
				break;
			}*/

			//1)�ȷ�ֵ�ֱ��ʸ�С���ȷ�ֵ֡�ʸ�С����ȣ�HP/BP������ͬ
			//2)�뷧ֵ�ֱ�����ͬ���ȷ�ֵ֡�ʸ�С��HP/BP������ͬ
			if( ((tCap[byIdx].GetResolution() == byRes 
				&& tCap[byIdx].GetUserDefFrameRate() < byFps  )
				||
				(tCap[byIdx].IsResLowerWithOutMatched(tCap[byIdx].GetResolution(),byRes) 
				&& tCap[byIdx].GetUserDefFrameRate() <= tSrcCap.GetUserDefFrameRate() ))
				&& (tCap[byIdx].GetH264ProfileAttrb() == tSrcCap.GetH264ProfileAttrb())
				)
			{
				tDstCapEx = tCap[byIdx];
				bResult = TRUE;
				break;
			}
		}

		if(!bResult)
		{	
			for(u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++)
			{
				if( tCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL )
				{
					break;																	
				}
				//�ڴ��ڵ��ڷ�ֵ��С�ڵ���Դ�����Ļ���������Сƥ��������
				if( !tCap[byIdx].IsResLowerWithOutMatched(tCap[byIdx].GetResolution(),byRes)
					&& !tSrcCap.IsResLowerWithOutMatched(tSrcCap.GetResolution(),tCap[byIdx].GetResolution())
					&& tCap[byIdx].GetUserDefFrameRate() <= tSrcCap.GetUserDefFrameRate()
					&& tCap[byIdx].GetUserDefFrameRate() >= byFps
					&& (tCap[byIdx].GetH264ProfileAttrb() == tSrcCap.GetH264ProfileAttrb()))
				{
					tDstCapEx = tCap[byIdx];
					bResult = TRUE;
				}
			}
		}

		return bResult;
	}

	//�жϻ������˫�����Ƿ����������[2/15/2012 yanghuaizhi]
	BOOL32 IsConfExcludeDDSUseAdapt()const
	{
		if (0 != m_tConf.GetSecBitRate() ||
			(MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() &&
			0 != m_tConf.GetSecVideoMediaType()))
		{
			return TRUE;
		}
		else
		{
			if (
				(MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType() &&
					 m_tConfEx.IsMainHasCapEx() //2)v4r7ģ���й�ѡ
				)
				||
				(m_tConf.GetSecAudioMediaType() != MEDIA_TYPE_NULL &&
				m_tConf.GetSecAudioMediaType() != 0 &&            
				m_tConf.GetSecAudioMediaType() != m_tConf.GetMainAudioMediaType())
				)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}
};


// ҵ��ʵ��(Osp������)
class CMcuVcInst : public CInstance,
                   public CMcuSingleConfAllInfo
{
public:
	enum 
	{ 
		STATE_IDLE,       //����
		STATE_WAITFOR,     // �ȴ�����
		STATE_SCHEDULED,  //ԤԼ
        STATE_ONGOING     //��ʱ
	};

public:	
    CMcuVcInst();
	virtual ~CMcuVcInst();
	BOOL32 IsMtIpV6( u8 byMtId ); //mp����Ҫ[pengguofeng 5/10/2012] 

protected:

    /************************************************************************/
    /*                                                                      */
    /*                       һ��daemon������                             */
    /*                                                                      */
    /************************************************************************/

	//1. daemonʵ��������
	void DaemonProcCommonNotif( const CMessage * pcMsg );

    void DaemonProcPowerOn( void );
    void DaemonProcMcuMSStateNtf( const CMessage * pcMsg );    

    //2. daemon ����/ģ�����
    void DaemonProcMcsMcuCreateConfReq(const CMessage * pcMsg);
    void DaemonProcMcsMcuCreateConfByTemplateReq(const CMessage * pcMsg);
    void DaemonProcMcsMcuSaveConfToTemplateReq( const CMessage * pcMsg );
    void DaemonProcCreateConfNPlus(const CMessage * pcMsg);
    void DaemonProcMcsMcuTemplateOpr(const CMessage * pcMsg);
    void DaemonProcMtMcuCreateConfReq( const CMessage * pcMsg );
    void DaemonProcMcsMcuListAllConfReq( const CMessage * pcMsg );
    void DaemonProcDcsMcuReleaseConfRsp( const CMessage * pcMsg );  //2006-01-11
    BOOL32 NtfMcsMcuCurListAllConf( CServMsg &cServMsg );

    //3. daemon GK ����
    void DaemonProcGkRegRsp(const CMessage * pcMsg);
    void DaemonProcGKChargeRsp(const CMessage * pcMsg);
    void DaemonProcMcuReRegisterGKNtf( void );
	
    //4. daemon �������
	void DaemonProBasChnnlStatusNotif( const CMessage * pcMsg );
    void DaemonProcHDBasChnnlStatusNotif( const CMessage * pcMsg );
	void DaemonProcAudBasChnnlStatusNotif(const CMessage *pcMsg);
	void DaemonProcMcsMcuGetPeriEqpStatusReq( const CMessage * pcMsg );
	void DaemonProcRecMcuProgNotif( const CMessage * pcMsg );
	void DaemonProcRecMcuChnnlStatusNotif( const CMessage * pcMsg );
    void DaemonProcHduMcuChnnlStatusNotif( const CMessage * pcMsg );

	void DaemonProcGetDsInfoAck( const CMessage * pcMsg );
    void DaemonProcMpFluxNotify( const CMessage * pcMsg );
    void DaemonProcPeriEqpMcuStatusNotif( const CMessage * pcMsg );
	void DaemonProcMtadpDiscAndRegMsg(const CMessage * pcMsg);

	void DaemonProc8KEPeriEqpMcuStatusNotif( const CMessage * pcMsg );
	
	void DaemonProcMcsMcuStopSwitchMcReq( const CMessage * pcMsg );
	void DaemonProcMcsMcuStopSwitchTWReq( const CMessage * pcMsg );
	
	void DaemonProcMcsMcuGetMcuStatusReq( const CMessage * pcMsg );
	void DaemonProcMcsMcuSetMcuCfgCmd( const CMessage * pcMsg );	

	void DaemonProcMcuMtConnectedNotif( const CMessage * pcMsg );
	void DaemonProcMcuEqpConnectedNotif( const CMessage * pcMsg );
	void DaemonProcMcuEqpDisconnectedNotif( const CMessage * pcMsg );
	void DaemonProcMcuDcsConnectedNtf( const CMessage * pcMsg );	//2005-12-15
	void DaemonProcMcuDcsDisconnectedNtf( const CMessage * pcMsg );	//2005-12-15	
	void DaemonProcMcuMcsConnectedNotif( const CMessage * pcMsg );
	void DaemonProcMcuMcsDisconnectedNotif( const CMessage * pcMsg );
	void DaemonProcMtMcuApplyJoinReq( const CMessage * pcMsg );
	void DaemonProcMtMcuApplyJoinReq(BOOL32 bLowLevleMcuCalledIn, const CConfId &cConfId, const CMessage * pcMsg);
    
    void DaemonProcNPlusRegBackupRsp( const CMessage * pcMsg );    
    void DaemonProcAppTaskRequest( const CMessage * pcMsg );
	void DaemonProcHDIFullNtf ( const CMessage *pcMsg );
    void DaemonProcAgtSvcE1BandWidthNotif( const CMessage *pcMsg ); 	
    
    //�����ȱ��ݵ�Ԫ�����ú���
//     void DaemonProcUnitTestMsg( void );
//     void ProcUnitTestGetTmpInfoReq( CMessage *pcMsg );
//     void ProcUnitTestGetConfInfoReq( CMessage *pcMsg );
//     void ProcUnitTestGetMtListInfoReq( CMessage *pcMsg );
//     void ProcUnitTestGetAddrbookReq( CMessage *pcMsg );
//     void ProcUnitTestGetMcuCfgReq( CMessage *pcMsg );
	
protected:
    
    /************************************************************************/
    /*                                                                      */
    /*                    ���������������                                */
    /*                                                                      */
    /************************************************************************/

    //1����ͨ�������
	void ProcMcsMcuCreateConfReq( const CMessage * pcMsg );	
    void ProcMMcuMcuCreateConfByTemplateReq( const CMessage * pcMsg );	
	void ProcMcsMcuReleaseConfReq( const CMessage * pcMsg );
	void ProcMcsMcuModifyConfReq( const CMessage * pcMsg );
	void ProcMcsMcuSaveConfReq(  const CMessage * pcMsg );
	void ProcMcsMcuChangeLockModeConfReq( const CMessage * pcMsg );
	void ProcMcsMcuChangeConfPwdReq( const CMessage * pcMsg );
	void ProcMcsMcuEnterPwdRsp( const CMessage * pcMsg );
	void ProcMcsMcuDelayConfReq( const CMessage * pcMsg );
	void ProcMcsMcuChangeVacHoldTimeReq( const CMessage * pcMsg );
	void ProcTimerOngoingCheck( const CMessage * pcMsg );
	void ProcTimerScheduledCheck( const CMessage * pcMsg );
	void ProcTimerWaitMpRegCheck( const CMessage * pcMsg );
	void ProcMsSynRecoverConfTimer( const CMessage * pcMsg );
	void PowerOnNPlusApp(); //����power_on��Ϣ��Nplusҵ��  [6/5/2012 chendaiwei]
	void ProcTimerVacChange( const CMessage * pcMsg );
    void ProcTimerAutoRec( const CMessage * pcMsg );
	//  [12/25/2009 pengjie] ˢ���Զ�¼��ʱ��
	void ProcTimerUpdataAutoRec( const CMessage * pcMsg );
	BOOL32 SetAutoRec( u16 &wErrCode );
    // End

	BOOL32 IsConfFileGBK(); // �жϻ����ļ��Ƿ�ΪGBK [pengguofeng 7/18/2013]

	// [pengjie 2010/9/29] �������Ƿ��й㲥Դ
	void ProcTimerConfHasBrdSrc( void );
	// End

	// [pengjie 2010/2/26] ˫������mcu��ʱ�����ƻ�
	void ProcTimer239TokenNtf( void );
	// End

	// 2011-8-5 add by pgf:�����û��ָ��IP����������
	BOOL32 IsCanGetSpecCapByMtIp(const s8* pIpAddr, u32 adwCapSet[] , BOOL32 bIsGetMainCapSet = TRUE );
	// 2011-8-5 add end

    void ProcWaitEqpToCreateConf(CMessage * const pcMsg);

    void CreateConfEqpInsufficientNack( u8 byCreateMtId, u8 byOldInsId, const s32 nErrCode, CServMsg &cServMsg );
    void CreateConfSendMcsNack( u8 byOldInsId, const s32 nErrCode, CServMsg &cServMsg, BOOL32 bDeleteAlias = FALSE );
    BOOL32 IsConfInfoCheckPass( const CServMsg &cSrcMsg, TConfInfo &tConfInfo, u16 &wErrCode, BOOL32 bTemplate = FALSE);
    BOOL32 IsEqpInfoCheckPass( const CServMsg &cSrcMsg, u16 &wErrCode);
    void ConfDataCoordinate(CServMsg &cMsg, u8 &byMtNum, u8 &byCreateMtId);
	void NPlusVmpParamCoordinate (CServMsg &cServMsg);
	//[12/29/2011 chendaiwei]�Ӵ�����Ϣ���TConfInfoEx����
	BOOL32 GetConfExInfoFromMsg(CServMsg &cServMsg, TConfInfoEx &tConfInfoEx);
    void AddSecDSCapByDebug(TConfInfo &tConfInfo);

	// 20130221 zhouyiliang �ж�������ԤԼ�����Ƿ���Ҫ������ʱ����
	BOOL32 IsCircleScheduleConfNeedStart( );
	TKdvTime GetNextOngoingTime(const TConfInfo& tConfInfo, const TDurationDate& tValidDuraDate,BOOL32 bAcceptCurOlder = FALSE);
	//����starttime�ж��Ƿ���������ԤԼ����
	BOOL32 IsCircleScheduleConf(const TKdvTime& tStartTime);
	//��֤durastart��ʱ����Ϊ00:00:00,duraend(��ֵ�Ļ�)��ʱ����Ϊ23:59:59
	void   ModifyCircleScheduleConfDuraDate( TDurationDate& tDuarationDate );


	BOOL32 PrepareAllNeedBasForConf(u16* pwErrCode = NULL);
	void   RlsAllBasForConf();

    void ReleaseConf( BOOL32 bRemoveFromFile = TRUE);
    u32  PackMtInfoListIntoBuf(const TMt atMt[], const TMtAlias	atMtAlias[],
                               u8	byArraySize, u8 *pbyBuf, u32 dwBufLen );	
    void ChangeConfLockMode( CServMsg &cMsg );
    
    void SendReplyBack( CServMsg & cReplyMsg, u16 wEvent );
    BOOL32 IsMtCmdFromThisMcu( const CServMsg & cCmdReply ) const;
    BOOL32 IsMcCmdFromThisMcu( const CServMsg & cCmdReply ) const;
	BOOL32 IsMsgNeedTranslate( u16 wEventId );
	void TranslateMsg( u16 wEvent,CServMsg & cServMsg );
    BOOL32 SendMsgToMt( u8 byMtId, u16 wEvent, CServMsg & cServMsg );
    BOOL32 SendMsgToEqp( u8 byEqpId, u16 wEvent, CServMsg & cServMsg );
    BOOL32 SendMsgToMcs( u8 byMcsId, u16 wEvent, CServMsg & cServMsg );
    BOOL32 SendOneMsgToMcs( CServMsg &cServMsg, u16 wEvent );
    void SendMsgToAllMcs( u16 wEvent, CServMsg & cServMsg );
    void SendMsgToAllMp( u16 wEvent, CServMsg & cServMsg);  // libo [4/27/2005] add
	void SendMtListToMcs(u16 wMcuIdx, const BOOL32 bForce = FALSE, const BOOL32 bMMcuReq = FALSE );//(u8 byMcuId)
	void SendAllMtInfoToAllMcs( u16 wEvent, CServMsg & cServMsg, const BOOL32 bForce = FALSE );
    void SendMtAliasToMcs(TMt tMt);
    void BroadcastToAllSubMtJoinedConf( u16 wEvent, CServMsg & cServMsg );
    void BroadcastToAllMcu( u16 wEvent, CServMsg & cServMsg);
    BOOL32 SendMsgToDcsSsn( u8 byInst, CServMsg &cServMsg ); //2005-12-16
    
    void SendTrapMsg( u16 wEvent, u8 * const pbyMsg, u16 wLen );    
    
    void NotifyMtSend( u8 byDstMtId, u8 byMode = MODE_BOTH, BOOL32 bStart = TRUE );
public: //��MpManager��
    void NotifyMtReceive(  const TMt & tSrc, u8 byDstMtId );	
protected:
    void StartMtSendbyNeed( TSwitchChannel* ptSwitchChannel );
    void StopMtSendbyNeed( TSwitchChannel* ptSwitchChannel );
    void NotifyRecStartPlaybyNeed( TSwitchChannel* ptSwitchChannel ); // zgc, 2007-02-27
    void NotifyOtherMtSend( u8 byDstMtId, BOOL32 bStart );  //֪ͨ���������ն��Ƿ�����Ƶ����        	
    BOOL32 IsOtherMtCanBeStopSend( u8 byDstMtId );
    // guzh [7/27/2007] ����VCU�����͸��նˣ�����
    void NotifyFastUpdate(const TMt &tDst, u8 byChnlMode, BOOL32 bSetTimer = FALSE);
    void NotifyMtFastUpdate(u8 byMtId, u8 byChnlMode, BOOL32 bSetTimer = FALSE);
    void NotifyEqpFastUpdate(const TMt &tDst, u8 byChnlMode, BOOL32 bSetTimer = FALSE);
	void NotifyChairmanMixMode(void);

	// [8/18/2010 xliang] ֪ͨԴ��ʼ���Ͳ���ؼ�֡(Դ������MT��EQP)
	void NotifySrcSend(const TMt &tSrcMt, u8 byMode = MODE_BOTH, u8 bySrcChnnl = 0 , BOOL32 bFastUpdateTimer = TRUE);
    
	//[nizhijun 2011/09/09]����MCU״̬��Ϣͨ��
	void SendExtMcuStatusNotify(u8 byMcsId, u8 nCurIndex,  BOOL32 bIsSendAll = FALSE);

	//2�����ݻ����������
	void SendMcuDcsCreateConfReq();
	void SendMcuDcsReleaseConfReq();
	void ProcDcsMcuCreateConfRsp( const CMessage * pcMsg );
	BOOL32 FindOnlineDcs();
	void ResetInstDataStatus();

	void SendMcuDcsAddMtReq( u8 byMtId, u32 dwMtIp = 0, u16 wPort = 0 );
	void SendMcuDcsDelMtReq( u8 byMtId );
	void ReInviteHasBeenInDataConfMt();
	void ProcDcsMcuAddMtRsp( const CMessage * pcMsg );
	void ProcDcsMcuDelMtRsp( const CMessage * pcMsg );
	void ProcDcsMcuMtJoinedNtf( const CMessage * pcMsg );
	void ProcDcsMcuMtLeftNtf( const CMessage * pcMsg );
	void ProcDcsMcuStatusNotif( const CMessage * pcMsg );
	void ProcTimerRecreateDataConf( void );
    
    //3��gk �Ʒ�
    void ProcConfRegGkAck( const CMessage * pcMsg );
	void ProcConfRegGkNack( const CMessage * pcMsg );
    void ProcGKChargeRsp( const CMessage * pcMsg );
	
	//4����ͨ������� 
    void ChangeVidBrdSrc( TMt * ptNewVidBrdSrc, u16 wOldVidBrdSpyPort = SPY_CHANNL_NULL );
    void ChangeAudBrdSrc( TMt * ptNewAudBrdSrc, u16 wOldAudBrdSpyPort = SPY_CHANNL_NULL );
    void ChangeSpeaker(  TMt * ptNewSpeaker, BOOL32 bPolling = FALSE, BOOL32 bAddToVmp = TRUE,BOOL32 bNeedPreSetin = TRUE , BOOL32 bIsRecPlayChange = FALSE);
	void ChangeSecSpeaker(TMt &tNewSpeaker, BOOL32 bNeedPreSetin = TRUE);
    void ChangeChairman(  TMt * ptNewChairman, BOOL32 bAdjustSwitch = TRUE );
    void AdjustChairmanSwitch( TMt * ptOldChair = NULL );
	void AdjustChairmanVidSwitch( TMt *ptOldChair = NULL );
	void AdjustChairmanAudSwitch( void );
    
    TMt  GetLocalSpeaker( void );
    BOOL32 HasJoinedSpeaker( void );
	BOOL32 HasJoinedChairman( void );
	BOOL32 IsSelectedbyOtherMt( const TMt &tMt, u8 byMode = MODE_VIDEO);
	BOOL32 IsSelectedbyOtherMtInMultiCas( const TMt &tMt, u8 byMode = MODE_VIDEO);
	BOOL32 IsKedaMt(const TMt &tMt, BOOL32 bIsNeedLocal);	// [1/18/2010 xliang] �Ƿ��ǿƴ���ն�
	BOOL32 IsNoneKedaMtCanInMpu(const TMt & tMt);//��03/09/2011 zhouyiliang���жϷǿƴﳧ���ն��Ƿ��ܽ�����ϳ�
	BOOL32 IsG400IPCMt(const TMt & tMt); // �жϸ��ն��ǲ���IPCǰ�� [pengguofeng 10/31/2012]
	BOOL32 IsPhoneMt(const TMt & tMt); // �жϸ��ն��ǲ��ǵ绰�ն�
	

	// [8/5/2011 liuxu] ���ptNewSpeaker����, ¼����������ͣ״̬�л�Ϊ����״̬ʱ, ���Թ㲥����
	BOOL32 IsSpeakerCanBrdVid( const TMt* ptNewSpeaker = NULL );
	BOOL32 IsSpeakerCanBrdAud( const TMt* ptNewSpeaker = NULL );

	void   AdjustOldSpeakerSwitch( TMt tOldSpeaker, BOOL32 bIsHaveNewSpeaker, BOOL32 bSameInSMcu ,const TMt & tNewSpeaker);
	void   AdjustNewSpeakerSwitch( TMt tNewSpeaker, /*BOOL32 bAddToVmp, */u16 wOldSpeakerSpyPort = SPY_CHANNL_NULL );
	void   ChangeSpeakerSrc( u8 byMode, emChangeSpeakerSrcReason emReason, u16 wSpyStartPort = SPY_CHANNL_NULL);
	void   ChangeOldSpeakerSrc( u8 byMode, TMt tOldSpeaker, BOOL32 bIsHaveNewSpeaker );
	// 2011/07/06 add by peng guofeng start
	void   ChgSpeakerInMixer( TMt tOldSpeaker, TMt *ptNewSpeaker );
	BOOL32   ChgSpeakerInVmp( TMt tOldSpeaker, TMt tNewSpeaker, BOOL32 bAddToVmp );
	void   ChgSpeakerInTvWall( TMt tNewSpeaker );
	void   ChgSpeakerInHdu( TMt tNewSpeaker );
	// 2011/07/06 add end
    
    void ConfStatusChange( void );
    void ConfModeChange( void );
//  void MtStatusChange( u8 byMtId = 0, BOOL32 bForcely = FALSE );
// 	void MtStatusChange(const TMt* ptMt );
public: //��MpManagerר��
    void MtStatusChange(const TMt* ptMt = NULL, BOOL32 bForcely = FALSE );
protected:
	// [9/27/2011 liuxu] ���ͻ���������
	void SendMtStatusInBuf( const BOOL32 bClearBuf = TRUE );
	void SendAllLocalMtStatus( const u16 wEvent = MCU_MCS_MTSTATUS_NOTIF );
	void MtSecSelSrcChange(const TMt &tMt);

    void MtInfoChange( void );
    void MtOnlineChange( TMt tMt, BOOL32 bOnline, u8 byReason );
    void NotifyMcsAlarmInfo(u8 byMcsId, u16 wErrorCode);    //byMcsIdΪ0��֪ͨ����mcs
    void NotifyMcsApplyList( BOOL32 bSendToChairman = FALSE );
    void SendConfInfoToChairMt(void);
    void ProcTimerRefreshMcs( void );    

	void ProcMcsMcuSpecChairmanReq( const CMessage * pcMsg );
	void ProcMcsMcuCancelChairmanReq( const CMessage * pcMsg );
	void ProcMcsMcuSetConfChairMode( const CMessage * pcMsg );
	void ProcMcsMcuSpecSpeakerReq( const CMessage * pcMsg );
	void ProcMcsMcuCancelSpeakerReq( const CMessage * pcMsg );
	void ProcMcsMcuSeeSpeakerCmd( const CMessage * pcMsg );
    void ProcMcsMcuSpecOutViewReq( const CMessage * pcMsg );
	void ProcMcsMcuAddMtReq( const CMessage * pcMsg );
	void ProcMcsMcuAddMtExReq( const CMessage * pcMsg );
	void ProcMcsMcuDelMtReq( const CMessage * pcMsg );
	void ProcMcsMcuSendMsgReq( const CMessage * pcMsg );
	void ProcBatchMtSmsOpr(const CServMsg &cServMsg);
	void ProcSingleMtSmsOpr(const CServMsg &cServMsg, TMt *ptDstMt, CRollMsg *ptROLLMSG);
	void ProcMcsMcuGetMtListReq( const CMessage * pcMsg );
	void ProcMcsMcuRefreshMcuCmd( const CMessage * pcMsg); 
	void ProcMcsMcuGetAllMtAliasReq( const CMessage * pcMsg );
	void ProcMcsMcuGetConfInfoReq( const CMessage * pcMsg );
	void ProcMcsMcuVMPReq(const CMessage * pcMsg);
    void VmpCommonReq(CServMsg & cServMsg);
    void ProcMcsMcuVmpTwReq(const CMessage * pcMsg);
    void VmpTwCommonReq(CServMsg & cServMsg);
	void ProcMcsMcuGetConfStatusReq( const CMessage * pcMsg );
	void ProcMcsMcuGetLockInfoReq( const CMessage * pcMsg );
	void ProcMcsMcuVACReq(const CMessage * pcMsg);
    void MixerVACReq(CServMsg & cServMsg);
	void ProcMcsMcuMixReq(const CMessage * pcMsg);
    void ProcMcsMcuChgMixDelayReq(const CMessage * pcMsg);
    //void MixerCommonReq(CServMsg & cServMsg);
	void ProcMcsMcuCamCtrlCmd( const CMessage * pcMsg );
	void ProcMMcuMcuCamCtrlCmd( const CMessage * pcMsg );
	void ProcMcsMcuMtOperCmd( const CMessage * pcMsg );
	void ProcMcsMcuMtOperReq( const CMessage * pcMsg );
	void ProcMtMuteDumbReq(CServMsg &cServMsg);
	void ProcSingleMtMuteDumbOpr(CServMsg &cServMsg, TMt *ptDstMt, u8 byMuteOpenFlag, u8 byMuteType);
	void ProcMcsMcuPollMsg(const CMessage * pcMsg);
	void ProcPollingChangeTimerMsg(const CMessage *pcMsg);    
    void ProcStopConfPoll(BOOL32 bNeedAdjustVmp = TRUE);
    void ProcMcsMcuHduPollMsg(const CMessage *pcMsg);
	void ProcMcsMcuHduBatchPollMsg(const CMessage *pcMsg);    //  ����hdu������ѯ
    void ProcMcsMcuTWPollMsg(const CMessage *pcMsg);
    void ProcTWPollingChangeTimerMsg(const CMessage *pcMsg);
	//void ProcHduPollingChangeTimerMsg(const CMessage *pcMsg);
    void ProcHduBatchPollChangeTimerMsg(const CMessage *pcMsg);  // hdu������ʱ����
	void ProcMcsMcuMcuMediaSrcReq(const CMessage *pcMsg);
	TMtPollParam *GetNextMtPolled(u8 &byPollPos, TPollInfo& tPollInfo);
    TMtPollParam *GetMtTWPollParam(u8 &byPollPos, u8 byEqpId, u8 byChnId);
    //TMtPollParam *GetMtHduPollParam(u8 &byPollPos);
	void ProcMcsMcuVmpPollMsg(const CMessage * pcMsg);			// vmp��ͨ����ѯ
    void ProcVmpPollingChangeTimerMsg(const CMessage *pcMsg);	// vmp��ͨ����ѯ��ʱ����
	TMtPollParam *GetNextMtPolledForVmp(u8 &byPollPos, TPollInfo& tPollInfo);	//����һ�����ʵ���ѯ�ն�
	void ProcStopVmpPoll();		// ֹͣVmp��ͨ����ѯ
	void ProcVmpPollingByRemoveMt( const TMt &tRemoveMt );

	void ProcMcsMcuLockSMcuReq(const CMessage *pcMsg);
	void ProcMcsMcuGetMcuLockStatusReq(const CMessage *pcMsg);
    void NotifyMtToBePolledNext( void );
    void NotiyfMtConfSpeakMode( u8 byMtId, u8 byMode );
    void NotifyMtSpeakStatus( TMt tMt, emMtSpeakerStatus emStatus );
    void PollParamChangeNotify( void );
    void ProcMcsMcuRollCallMsg( const CMessage * pcMsg );
    void RollCallStop( CServMsg &cServMsg );
    void ProcMixStart( CServMsg &cServMsg );
	BOOL32 StartLocalMix( u8 byMtNum, emMcuMixMode eMixMode,TMt *ptMixMember,u8 byEqpId,u16 &wErrorCode);
	BOOL32 StartCascadeMix( u8 byMtNum, emMcuMixMode eMixMode,TMt *ptMixMember,u8 byEqpId,u8 byReplaceMember,u16 &wErrorCode);
	BOOL32 UnPackStartMixMsg (CServMsg &cServMsg, TMixMsgInfo & tMixMsg);
	//BOOL32 UnPackLocalStartMixMsg (CServMsg &cServMsg, TMixMsgInfo & tMixMsg);
	//BOOL32 UnPackStartCascadeMixMsg (CServMsg &cServMsg, TMixMsgInfo & tMixMsg);
	//BOOL32 IsStartMixCheckPass ( TMixMsgInfo & tMixMsg);
	BOOL32 IsStartLocalMixCheckPass ( u8 byMtNum, emMcuMixMode eMixMode,TMt *ptMixMember,u8 &byEqpId, u16 &wErrorCode);
	BOOL32 IsStartCascadeMixCheckPass ( u8 byMtNum, emMcuMixMode eMixMode,TMt *ptMixMember,u8 &byEqpId, u16 &wErrorCode);
	//void StartMixDataCoordinate ( TMixMsgInfo &tMixMsg);
	void StartLocalMixDataCoordinate ( u8 byMtNum, emMcuMixMode &eMixMode, TMt *ptMixMember, u8 byEqpId);
	void StartCascadeMixDataCoordinate ( u8 byMtNum, emMcuMixMode &eMixMode, TMt *ptMixMember, u8 byEqpId);
	void SwitchMixModeVacToVacWhole( void );
	void SwitchMixModeSpecToAutoMix( void );
    void ProcMixStop( CServMsg &cServMsg, BOOL32 bRollCallStop = FALSE );
	void ProcCancelSpyMtMsg( const CMessage * pcMsg );	
	// [pengjie 2010/5/17] ������չ��Ϣ֪ͨ����(�¼������Ƿ�֧�ֶ�ش�)
	void SendConfExtInfoToMcs( u8 bySrcSsnId = 0,u8 byMtId = 0 );
	void ProcConfPollingByRemoveMt( const TMt &tRemoveMt );
	
	//zhouyiliang 20120921 ��ش������뱻������ģʽ�£����¼��նˣ��ȴ�����Ӧ����
	void SetLastMultiSpyRollCallFinish(const BOOL32 bFinish);
	BOOL32 IsLastMutiSpyRollCallFinished()const
	{
		return m_tConfInStatus.IsLastMutiSpyVmpRollCallFinished();
	}
	void ProcWaitLastVmpRollCallFinishTimer();
	void ProcWaitMSConfDataTimer();

	void ProcMcsMcuGetIFrameCmd(const CMessage *pcMsg);

    //5�����Ƿ�ɢ����
    u8   GetSatCastChnnlNum(u8 bySrcMtId);
//     BOOL32 IsOverSatCastChnnlNum(u8 bySrcMtId, u8 byIncNum = 1);
// 	BOOL32 IsOverSatCastChnnlNum(TMt tMt, u8 byIncNum = 1);
    BOOL32 IsOverSatCastChnnlNum(TVMPParam_25Mem &tVmpParam);
//     void ChangeSatDConfMtRcvAddr(u8 byMtId, u8 byChnnlType, BOOL32 bMulti = TRUE);
public:
    void ChangeSatDConfMtRcvAddr(u8 byMtId, u8 byChnnlType, BOOL32 bMulti = TRUE);
	BOOL32 IsCanSatMtReceiveBrdSrc(const TMt &tMt, u8 byMode = MODE_VIDEO);
protected:
    BOOL32 IsMtSendingVidToOthers(TMt tMt, BOOL32 bConfPoll/*with audio*/, BOOL32 bTwPoll, u8 bySelDstMtId);
	// �ж��滻�ն�ʱ�����ǻش������Ƿ�ᳬ��ģ�������õ���ֵ [pengguofeng 1/23/2013]
	BOOL32 IsSatMtOverConfDCastNum(const TMt &tNewMt, emSatReplaceType emReplaceType = emDefault,
		const u8 &bySkipVcId = 0xff, const u8 &bySkipVcChnnl = 0xff,
		const u8 &bySkipVmpChnnl = 0xff, const u8 &bySkipHduId = 0xff,
		const u8 &bySkipHduChnnl = 0xff, const u8 &bySkipMtId = 0 );
	// �����ն˻᲻��ά�ַ���������״̬ [pengguofeng 2/18/2013];�����̳���������ӿ�
	BOOL32 IsMtKeepSatChnnl(const TMt &tMt, emSatReplaceType emReplaceType,
		const u8 &bySkipVcId = 0xff, const u8 &bySkipVcChnnl = 0xff,
		const u8 &bySkipVmpChnnl = 0xff, const u8 &bySkipHduId = 0xff,
		const u8 &bySkipHduChnnl = 0xff, const u8 &bySkipMtId = 0 );
	void  SetCurSatMtNum(s16  swNum){	m_swCurSatMtNum = swNum;	}
	s16   GetCurSatMtNum(){	return m_swCurSatMtNum;	}
	BOOL32 IsSatMtCanContinue(u8                byNewSatMtId      =  0, 
							  emSatReplaceType  emServiceType     =  emDefault,
							  TEqp              *ptEqp            =  NULL,
						   	  u8                byChnId           =  0xFF,
							  u8                bySubChnId        =  0xFF,
							  TVMPParam_25Mem   *pTVMPParam       =  NULL);

    //6��N+1�������
    void ProcNPlusConfInfoUpdate(BOOL32 bStart);
    void ProcNPlusConfMtInfoUpdate(void);
    void ProcNPlusChairmanUpdate(void);
    void ProcNPlusSpeakerUpdate(void);
    void ProcNPlusVmpUpdate(void);
	void ProcNPlusAutoMixUpdate(BOOL32 bStart);
    void ProcNPlusConfDataUpdate(const CMessage * pcMsg);
    void GetNPlusDataFromConf(TNPlusConfData &tNPlusData, TNPlusVmpParam *ptNplusVmpParam, u8 &byVmpNum ,BOOL32 bCharge = FALSE);
	u16 GetNPlusDataExFromConf(TNPlusConfExData &tNPlusDataEx);
    
    //7�������ȱ��ݣ�M-S Data-Exchange
public:
    BOOL32 GetInstStateAndConfIdIdx( u8 &byState ,CConfId & cConfId,u8& byConfIdx);
    BOOL32 SetInstStateAndConfIdIdx( const u8& byState ,const CConfId& cConfId, const u8& byConfIdx);
    
    BOOL32 HangupInstTimer( void );
    BOOL32 ResumeInstTimer( void );
    
    BOOL32 GetConfMtTableData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
    BOOL32 SetConfMtTableData( u8 *pbyBuf, u32 dwInBufLen );
    BOOL32 GetConfSwitchTableData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
    BOOL32 SetConfSwitchTableData( u8 *pbyBuf, u32 dwInBufLen );
    BOOL32 GetConfOtherMcTableData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
    BOOL32 SetConfOtherMcTableData( u8 *pbyBuf, u32 dwInBufLen );
    BOOL32 GetInstOtherData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
    BOOL32 SetInstOtherData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bResumeTimer );

	void   SetWaitMsConfData( BOOL32 bIsWait );
	BOOL32 IsWaitMsConfData( void );
	void   SetInOtherConf( BOOL32 bIsInOtherConf,u8 byOtherConfMtId = 0 );
	BOOL32 IsInOtherConf( void );
 
	//8��VCS���鴦��
	// 1) VCS Daemonʵ���������Ϣ
	// ���������Ϣ
	void DaemonProcVcsMcuSavePackinfoReq( const CMessage * pcMsg );	
	// ��ȡ������Ϣ��ʱ
	void DaemonProcPackInfoTimer( const CMessage * pcMsg );

	// 2)VCS��ͨʵ�������Ϣ�������
	void ProcVcsMcuMsg( const CMessage * pcMsg );
	// ����vcs����
	void ProcVcsMcuCreateConfReq( const CMessage * pcMsg );
	// �ն˵���������
    void ProcVcsMcuVCMTReq( const CMessage * pcMsg );
	// ��ǰ����ĵ���ģʽ������
	void ProcVcsMcuVCModeReq( const CMessage * pcMsg );
	// ����ģʽ�л�����
	void ProcVcsMcuVCModeChgReq( const CMessage * pcMsg );
	// ���龲����������
	void ProcVcsMcuMuteReq( CServMsg& cServMsg );
	// �����ն˳�ʱ����
	void ProcVCMTOverTime( void );
	// ����ն�������
	void ProcVcsMcuGroupCallMtReq(const CMessage * pcMsg);
	void ProcVcsMcuGroupDropMtReq(const CMessage * pcMsg);
	// ��ϯ��ѯ
    void ProcVcsMcuStartChairPollReq(const CMessage *pcMsg);
    void ProcVcsMcuStopChairPollReq(const CMessage *pcMsg);
	void VcsStopChairManPoll();
    void ProcChairPollTimer( void );
	// ��ʱ�����ն�
	void ProcVcsMcuAddMtReq(const CMessage *pcMsg);
	void ProcVcsMcuDelMtReq(const CMessage *pcMsg);
	// ��ռ����, �Ƿ�ͬ���ͷ��ն�
	void ProcVcsMcuRlsMtMsg(const CMessage * pcMsg);
	// �����ϼ�������������,���ϼ��ӹܻ������
	void ProcMMCUGetConfCtrlMsg();
    //�������ϱ�E1�ն˴���ָʾ
    void ProcAgtSvgE1BandWidthNotif( const CMessage * pcMsg );

	// VCS��������������Դ�ļ��
	BOOL32 IsVCSEqpInfoCheckPass(u16& wErrCode);
	// VCS������Ϣ������
    void VCSConfDataCoordinate(CServMsg& cServMsg, u8 &byMtNum, u8 &byCreateMtId);
	// VCS������ʵ�����ģʽ��ѯ
	u8   VCSGetCallMode(TMt tMt);
	// ͨ��VCS���������Ϣ
	void NotifyVCSPackInfo(CConfId& cConfId, u8 byVcsId = 0);


	// VCS���йҶ��ն�
	void VCSCallMT(CServMsg& cServMsgHdr, TMt& tMt, u8 byCallType);
	void VCSDropMT(TMt tMt);

	// VCSѡ������
	BOOL VCSConfSelMT(TSwitchInfo tSwitchInfo,BOOL32 bNeedPreSetin = TRUE );
	void VCSConfStopMTSel(TMt tDstMt, u8 byMode,BOOL32 bIsRestoreSeeBrd = TRUE );    // ȡ���ն�ѡ��
	void VCSConfStopMTSeled(TMt tSrcMt, u8 byMode);  // ȡ���ն˱�ѡ��
    BOOL IsNeedSelApt(u8 bySrcMtId, u8 byDstMtId, u8 bySwitchMode);

	BOOL VCSConfMTToTW(TTWSwitchInfo tTWSwitchInfo);
	void VCSConfStopMTToTW(TMt tMt);
    // ���ն˾�����
	void VCSMTMute(TMt tMt, u8 byMuteOpenFlag, u8 byMuteType);
	u8   GetVcsHduAndTwModuleChnNum();


	void GoOnSelStep(TMt tMt, u8 byMediaType, u8 bySelDirect);	// ���ݻ���״̬����������δ��ɵ�ѡ������
	TMt  GetVCMTByAlias(TMtAlias& tMtAlias, u16 wMtDialRate, u8 byForce = FALSE); 	// �����ն˱�����ȡ��ӦTMt
	void ChgCurVCMT(TMt tNewCurVCMT); 	// ���ݻ���״̬���л���ǰ�����ն�
	void ReLoadOrigMtList( BOOL32 bIsAdd = TRUE );            // ���¼��ػ�������ն��б�

	void VCSSwitchSrcToDst(const TSwitchInfo &tSwitchInfo);
	BOOL FindUsableVMP();
	u8   GetVCSAutoVmpMember(u8 byVmpId, TMt* ptVmpMember);
	void SetVCSVmpMemInChanl(TMt* pVmpMember, u8 byVmpSytle);
    void ChangeSpecMixMember(TMt* ptMixMember, u8 byMemberNum);
	void RestoreVCConf(u8 byNewMode);
	void ChangeVFormat(TMt tMt, 
					   u8 byStart = FALSE,
					   u8 byNewFormat = VIDEO_FORMAT_INVALID,
					   u8 byEqpId = 0,
					   u8 byVmpStyle = ~0,
					   u8 byPos = ~0,
					   u32 dwResW = 0,
					   u32 dwResH = 0,
					   u8 byHduChnId = 0);
	void VCSMTAbilityNotif(TMt tMt, u8 byMediaType);
	void VCSChairOffLineProc();
    void VCSConfStatusNotif();
	BOOL32 VCSChangeChairMan( void );

	TMt  VCSGetNextPollMt();
	BOOL32 SwitchToAutoMixing();

	//zjj 20090911�����¼��ն�ʱҪ��ͬ���������ն��޳�����ǽ
	void FindSmcuMtInTvWallAndStop( const TMt& tNewCurVCMT, TMt *const ptDropMt, BOOL32 bIsStopSwitch = TRUE );

	//zjj 20091026
	void ProcVcsMcuMtInTwMsg( const CMessage *pMsg );
	
	//zjj20091102
	void ProcMcsVcsMcuAddPlanName( const CMessage *pMsg );
	void ProcMcsVcsMcuDelPlanName( const CMessage *pMsg );
	void ProcMcsVcsMcuModifyPlanName( const CMessage *pMsg );
	//void ProcVcsMcuSavePlanData( const CMessage *pMsg );
	void ProcMcsVcsMcuGetAllPlanData( const CMessage *pMsg );
	void ProcVcsMcuChgTvWallMgrMode( const CMessage *pMsg );
	void ProcVcsMcuClearOneTvWallChannel( const CMessage *pMsg );
	void ProcVcsMcuClearAllTvWallChannel( void );
	void ProcVcsMcuTvWallMtStartMixing( const CMessage *pMsg );
	void ProcVcsMcuMtInTvWallInReviewMode( CServMsg& cServMsg );
	void ProcVcsMcuMtInTvWallInManuMode(CServMsg& cServMsg);
	void SetSingleMtInTW(TSwitchInfo *ptSwitchInfo, CServMsg &cServMsg);
	void SetMtInTvWallAndHduInFailPresetinAndInReviewMode( TPreSetInRsp &tPreSetInRsp );
	//void ProcVcsMcuLockPlan( const CMessage *pMsg );
	//void ProcVcsMcuUnLockPlanByVcsssnInst( const CMessage *pMsg );

	void ProcVcsMcuChangeDualStatusReq( const CMessage * pcMsg );
	void ProcVcsMcuMtJoinConfRsp( const CMessage * pcMsg );
    //[5/4/2011 zhushengze]VCS���Ʒ����˷�˫��
    void ProcMcsMcuChangeMtSecVidSendCmd( const CMessage * pcMsg );
    //[5/19/2011 zhushengze]�����������״̬����
    void ProcVcsMcuGroupModeConfLockCmd( const CMessage * pcMsg );

	//[2/23/2012 zhushengze]���桢�ն���Ϣ͸��
    void ProcMcsMcuTransparentMsgNotify( const CMessage * pcMsg );
	
	void ShowCfgExtraData();
	void NotifyAllPlanNameToConsole( BOOL32 bNotifyVCS = TRUE);
	void NotifyOnePlanDataToConsole( s8* pbyPlanName,u16 wEvent,BOOL32 bNotiyVCS = TRUE );
	void VCSClearAllTvWallChannel( const TSwitchInfo *ptSwitchInfo = NULL, const u8 byNum = 0);
	void VCSClearTvWallChannelByMt( TMt tMt,BOOL32 bOnlyClearTvWallStatus = FALSE );
	void VCSChangeTvWallMtMixStatus( BOOL32 bIsStartMixing );
	BOOL32 VCSMtNotInTvWallCanStopMixing( TMt tMt );
	BOOL32 VCSMtInTvWallCanStopMixing( TMt tMt );
	void VCSFindMtInTvWallAndChangeSwitch( TMt tMt,BOOL32 bIsStopSwitch,u8 byMode );
	void MCSFindMtInTvWallAndChangeSwitch( TMt tMt,BOOL32 bIsStopSwitch,u8 byMode );
	void VCSClearMtDisconnectReason( TMt tMt );
	void ClearHduSchemInfo();

	void ProcVcsMcuStartMonReq( const CMessage *pcMsg );// [8/17/2010 xliang] �������
	void ProcVcsMcuStopMonReq( const CMessage *pcMsg );
	void ProcVcsMcuStopAllMonCmd( const CMessage *pcMsg);
	// [chendaiwei 2010/09/11] VCS ���ݱ�������ϯģ�����ã��ж��¼�����ϯ����Ҫ��������ֵ
	u32 GetVscSMCUMaxBW();


	//8����ش����
	//lukunpeng 2010/06/10 ���ݸ�event��SetInRsp�ж�����Դ��Ŀ�����ʹ���Ϣ�����ͷŶ��ٴ���
	void EvaluateSpyFromEvent(const TPreSetInRsp &tPreSetInRsp, u8 &byAddDestSpyNum, u32 &dwCanReleaseBW, s16 &swCanRelIndex);
	
	//zhouyiliang 20100723 ���vcs�Զ�����ϳ��ϴ�������������
	void HandleVCSAutoVmpSpyRepalce( TPreSetInRsp &tPreSetInRsp );
	//zhouyiliang 20100809 ����vcs���滻�Ķ�ش��ն�
	TMt FindVCSCanReplaceSpyMt( u8 byMtInEqpType , const TMt& tSrc ,s16& swCanReplaceSpyIndex, u8& byCanReplaceChnnPos );
	//lukunpeng 2010/06/07 �ж����¼��Ƿ�ͬʱ֧�ֶ�ش�
	BOOL32 IsLocalAndSMcuSupMultSpy( u16 wMcuIdx );

	BOOL32 IsMMcuSupportMultiCascade( void );
	BOOL32 IsPreSetInRequired (const TMt &tMt);	 // �Ƿ���ҪpreSetIn
	BOOL32 OnMMcuPreSetIn(TPreSetInReq &tPreSetInReq);
	BOOL32 IsSupportMultCas( u16 wMcuIdx );

	void ProcMcuMcuSpyBWFull(const CMessage *pcMsg,TPreSetInRsp *ptPreSetInRsp,u16 wErrorCode = ERR_MCU_CONFSNDBANDWIDTHISFULL);
	BOOL32 JudgeMcuMcuPreSetIn(const TPreSetInReq& tPreSetInReq, const TMsgHeadMsg& tHeadMsg, TPreSetInRsp& tPreSetInRsp, u16 &wErrorCode);
	void ProcMcuMcuPreSetInReq( const CMessage *pcMsg );
	void ProcMcuMcuPreSetInAck( const CMessage *pcMsg );
	void ProcMcuMcuPreSetInNack( const CMessage *pcMsg );
	//void SendMMcuSpyNotify( const TPreSetInRsp &tPreSetInRsp );
	void SendMMcuSpyNotify( const TMt &tSpySrc, u32 dwEvId, TSimCapSet tDstCap/*, u8 byRes = VIDEO_FORMAT_INVALID*/ );
	void ProcUnfinishedCascadEvent( const TPreSetInRsp &tPreSetInRsp );
	void SendMMcuRejectSpyNotify( const TMt &tSrcMt, u32 dwSpyBW );
	BOOL32 CheckMtResAdjust( const TMt &tMt, u8 byReqRes, u16 &wErrorCode, u8 &byRealRes, u8 byStart = TRUE, u32 dwResW = 0, u32 dwResH = 0);
	//  [12/2/2009 pengjie] �õ�������Ա����Ŀ
	u8   GetCurMixerNum( void );  
	void ProcSelMtStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp );
	void ProcStartMonitorPreSetinAck( const TPreSetInRsp &tPreSetInRsp );
	void ProcVmpStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp);
	void ProcSpeakerStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp );
	// ����������δ��������߼�
	void ProcHduStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp );
	void ProcPollStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp );
	void ProcTWStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp );
	void ProcRollCallPreSetInAck( const TPreSetInRsp &tPreSetInRsp );
	void ProcRecStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp );
	void ProcDragStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp );
	u8 GetMixChnPos(const TMt &tSrc, BOOL32 bRemove = FALSE );
	BOOL32 IsMtInMixChn(const TMt &tMixMem);
	//void FreeSpyChannlInfoByMcuId(u8 byMcuId);
	BOOL32 FreeRecvSpy( const TMt tMt, u8 bySpyMode, BOOL32 bForce = FALSE);    // �ͷű������ɵĻش��ն���ռ�еĻش���Դ
	void FreeAllRecvSpyByMcuIdx( u16 wMcuIdx );
	void OnMMcuBanishSpyCmd(const TMt &tSrcMt, u8 bySpyMode, u8 bySpyNoUse = MODE_BOTH );
	void ProcMcuMcuMultSpyCapNotif( const CMessage * pcMsg );
	void ProcMcuMcuSpyNotify( const CMessage *pcMsg );
	void ProcMcuMcuRejectSpyNotify( const CMessage *pcMsg );
	void ProcMcuMcuBanishSpyCmd( const CMessage *pcMsg );
	//[nizhijun 2010/12/15] ������ش������ش�RTCP��ؽӿ�
	void  ProcMcuMcuMtExtInfoNotif(const CMessage *pcMsg);//�ϼ��յ��¼����͵�RTCP��Ϣ��֪��������
	void  OnMMcuRtcpInfoNotif(TMt tSrc, const TSimCapSet &tDstCap,TMsgHeadMsg tHeadMsg);//�¼�������֯���RTCP��Ϣ
	void  GetSpyBasRtcpInfo(TEqp &tBas, u8 &byChnId, u8 &byFrontOutNum, u8 &byOutIdx, TTransportAddr &tRtcpAddr);
	void  GetSpyCascadeRtcpInfo(TMt tSrc, TTransportAddr &tVidRtcpAddr, TTransportAddr &tAudRtcpAddr);
	void  SendMMcuMtExtInfoNotif(TMt tSpySrc, TMsgHeadMsg tHeadMsg, u8 bySpyMode, TTransportAddr &tVidRtcpAddr, TTransportAddr &tAudRtcpAddr);
	//[nizhijun 2010/12/15] ends
//	void ProcMMcuBanishedSpyNotify( const CMessage *pcMsg );
	BOOL32 StopSpyMtSpySwitch( TMt tSrc,u8 byMode = MODE_BOTH, u8 bySpyNoUse = MODE_BOTH/*,BOOL32 bIsNotifyMMcu = FALSE */);
	BOOL32 StopAllLocalSpySwitch();
	void StopSpeakerFollowSwitch( u8 byMode );
	BOOL32 StartSwitchToMcuByMultiSpy( TMt tSrc, u16 wSrcChnnl, u8 byDstMtId, const TSimCapSet &tDstCap, u16 &wErrorCode,
		u8 byMode = MODE_BOTH, u8 bySwitchMode = SWITCH_MODE_BROADCAST, BOOL32 bMsgStatus = TRUE);
	BOOL32 IsSupportMultiSpy( void );
	void RemoveMcu( const TMt &tMt );
	BOOL32 IsMcu( const TMt& tMt,u16 *pwMcuIdx = NULL );
	//zhouyiliang 20100826 �ж�����mt�Ĵ�����ϵ
	BOOL32 IsMtInMcu(const TMt& tMcu, const TMt& tMt, const BOOL32 bDirectlyUnder = FALSE);
	
	// [10/12/2011 liuxu] �ж��ն�tmt�Ƿ���tmcu���ϴ�ͨ����Ա
	BOOL32 IsSmcuViewMtInMcu( const TMt&, const TMcu& );
	// ��ȡ�¼�mcu���ϴ�ͨ����Ա
	TMt GetSMcuViewMt( const TMcu&, const BOOL32 bDeepest = FALSE );
	// yanghuaizhi ����ն�ֱ��mcu
	TMt GetDirectMcuFromMt( const TMt &tMt );
	// [pengjie 2010/4/2] ������ش�����ؼ�֡
	void ProcMcuMcuSpyFastUpdateCmd( const CMessage *pcMsg );
	void SendMcuMcuSpyFastUpdateCmd( const TMt &tMt, u8 byMode = MODE_VIDEO );

	//lukunpeng 2010/06/10 ������ش�
	//BOOL32 FreeSpySrcBW( TMt &tMt, u32 dwSpyBW ); // ָ���ͷ�ĳ���ն˵Ĳ��ִ���

	//BOOL32 FreeSpyChannlInfo( const TMt &tMt, u8 bySpyMode = MODE_BOTH, BOOL32 bForce = FALSE ); // �����ն���Ϣ�ͷ�һ���ش�ͨ��
	BOOL32 GetMcuMultiSpyBW( u16 wMcuIdx,u32 &dwMaxSpyBW, s32 &nRemainSpyBW );
	BOOL32 SetMcuSupMultSpyRemainBW( u16 wMcuIdx, s32 nRemainSpyBW );
	void   SetRealSndSpyBandWidth( u32 dwBandWidth );
	u32    GetRealSndSpyBandWidth( void );

	TSimCapSet GetMtSimCapSetByMode( u8 byMtId );
	BOOL32 GetMinSpyDstCapSet(const TMt &tMt , TSimCapSet& tReqDstCap);
	void UpdateCurSpyDstCapSet(const TMt &tMt );

	void OnPresetinFailed( const TMt &tMt );
	//zhouyiliang 20101110 �ж��Ƿ�˫��˫��ʽ��������ʽΪh264����MP4��������ʽ��ͬ��ֻ��һ����h264����MP4��
	BOOL32 IsConfDbCapDbBitrateCapH264OrMp4();
	void  ProcMcuMcuSwitchToMcuFailNotif(const CMessage *pcMsg);//�ϼ��յ��¼����͵�RTCP��Ϣ��֪��������
	//�㲥��Ҫ��������ն�Ѱ�Һ��ʵ�bas���
	BOOL32 FindBasChn2BrdForMtExt(const TMt &tSrc, u8 byMtId, u8 byMediaMode,TBasOutInfo &tOutInfo);
	BOOL32 IsBasChnForRecAdapt(const TEqp& tBas,u8 byChnId);

	TVmpModule GetVmpModule( void );
	TVMPParam_25Mem GetVmpParam25MemFromModule( void );

private:
	void VCSTransOutMsgToInnerMsg(CServMsg& cOutMsg);	
	

	
private: 
    u32  OprInstOtherData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bGet );
	u32  GetInstOtherDataLen( void );  


    /************************************************************************/
    /*                                                                      */
    /*                       �����ն˹�������                             */
    /*                                                                      */
    /************************************************************************/

protected:	

    //1����ͨ����
	void ProcMtMcuFlowControlCmd( const CMessage * pcMsg, BOOL32 bNtfPhyBandWidth = FALSE );
	void ProcPolycomMcuFlowctrlCmd(u8 byPolyMCUId, TLogicalChannel &tLogicChnnl);
	void ProcMtMcuFlowControlIndication( const CMessage * pcMsg );
	void ProcMtMcuCapSetNotif( const CMessage * pcMsg );
	void ProcMtMcuMtAliasNotif( const CMessage * pcMsg );
	void ProcMtMcuEnterPwdReq( const CMessage * pcMsg ); 
    void ProcMtMcuEnterPwdRsp( const CMessage * pcMsg );
    void ProcMtMcuOpenLogicChnnlReq( const CMessage * pcMsg );
	void ProcMtMcuOpenLogicChnnlRsp( const CMessage * pcMsg );
	void ProcMtMcuCloseLogicChnnlNotify( const CMessage * pcMsg );
	void ProcMtMcuMediaLoopOpr( const CMessage *pcMsg );
	void ProcMtMcuGetH239TokenReq( const CMessage * pcMsg );
	void ProcMtMcuOwnH239TokenNotify( const CMessage * pcMsg );
	void ProcMtMcuReleaseH239TokenNotify( const CMessage * pcMsg );
    void procWaitOpenDSChanTimeout( void );
	void ProcPolyMCUH239Rsp( const CMessage * pcMsg );
	void ProcMtMcuStopSwitchMtReq( const CMessage * pcMsg );
	void ProcMtMcuStartSwitchMtReq( const CMessage * pcMsg );
	void ProcStartSelSwitchMt(CServMsg &cServMsg);
	void ProcMtMcuSendMcMsgReq( const CMessage * pcMsg );
	void ProcMtMcuApplyJoinReq( const CMessage * pcMsg );
	void ProcMtMcuApplyChairmanReq( const CMessage * pcMsg );
	void ProcMtMcuApplySpeakerNotif( const CMessage * pcMsg );
	void ProcMtMcuGetMtAliasReq( const CMessage * pcMsg );
	void ProcMtMcuJoinedMtListReq( const CMessage * pcMsg );
	void ProcMtMcuJoinedMtListIdReq( const CMessage * pcMsg );
	void ProcMcsMcuCallMtReq( const CMessage * pcMsg );
	void ProcMcsMcuDropMtReq( const CMessage * pcMsg );
	void ProcMcsMcuSetCallMtModeReq( const CMessage * pcMsg );
	void ProcCommonOperNack( const CMessage * pcMsg );
	void ProcCommonOperAck( const CMessage * pcMsg );
	void ProcMtMcuMtStatusNotif( const CMessage * pcMsg );
	void ProcMcsMcuGetMtStatusReq( const CMessage * pcMsg );
	void ProcMcsMcuGetAllMtStatusReq( const CMessage * pcMsg );
    void ProcMcsMcuGetMtBitrateReq( const CMessage *pcMsg );
    void ProcMcsMcuGetMtExtInfoReq( const CMessage *pcMsg );
	void ProcMtMcuInviteMtNack( const CMessage * pcMsg );
	void ProcMtMcuInviteMtAck( const CMessage * pcMsg );
	void ProcMtMcuDisconnectedNotif( const CMessage * pcMsg );
	void ProcMtMcuConnectedNotif( const CMessage * pcMsg );
	void ProcMtMcuMtJoinNotif( const CMessage * pcMsg );
	void ProcMtMcuMtTypeNotif( const CMessage * pcMsg );
	void CallFailureNotify(CServMsg	&cServMsg);
	void ProcMtMcuCallFailureNotify(const CMessage * pcMsg);
	void ProcMtMcuGetChairmanReq( const CMessage * pcMsg );
	//void ProcMcuMtFastUpdatePic( const CMessage * pcMsg );
	void ProcMtMcuMsdRsp( const CMessage * pcMsg );
	void ProcMtMcuMatrixMsg( const CMessage * pcMsg );
	void ProcMtMcuOtherMsg( const CMessage * pcMsg );
	void ProcMtMcuFastUpdatePic( const CMessage * pcMsg );
	void ProcMtMcuMixReq( const CMessage * pcMsg );
	void ProcMtMcuReleaseMtReq( const CMessage * pcMsg );
	void ProcMtMcuApplyCancelSpeakerReq( const CMessage * pcMsg );
	//  [4/1/2011 zhushengze] MCS MTC-BOX
	void ProcVcsApplyCancelSpeakerReq( const CMessage * pcMsg );
	void ProcMcsApplyCancelSpeakerReq( const CMessage * pcMsg );
	void ProcTimerInviteUnjoinedMt( const CMessage * pcMsg );
	void ProcMtAdpMcuMtListRsp( const CMessage * pcMsg ); //  [pengguofeng 7/1/2013]
	void ProcMtAdpMcuSmcuMtAliasRsp( const CMessage * pcMsg ); //  [yanghuaizhi 8/8/2013]
	void PackAndSendMtList2Mcs(u8 &byMtNum, TMtExtU *ptMtExtU, const u16 &wMcuIdx); //

    void ProcMtMcuTransParentMsgNotify( const CMessage * pcMsg );

	void ProcMcsMcuGetMtVidAliaseReq( const CMessage * pcMsg );
	void ProcMtMcuVidAliasNtf( const CMessage * pcMsg );

	u8   AddMt( TMtAlias &tMtAlias, u16 wMtDialRate = 0, u8 byCallMode = CONF_CALLMODE_TIMER, BOOL bPassiveAdd = FALSE );
	void AddJoinedMt( TMt & tMt );
	void RemoveMt( TMt & tMt, BOOL32 bMsg,BOOL32 bIsSendMsgToMcs = TRUE );
	void RemoveJoinedMt(TMt & tMt, BOOL32 bMsg, u8 byReason = MTLEFT_REASON_NORMAL,BOOL32 bNeglectMtType = FALSE);
	BOOL32 DealMtMcuEnterPwdRsp(TMt tMt, s8 *pszPwd, s32 nPwdLen);
    void ProcMtMcuBandwidthNotif(const CMessage *pcMsg);
	void ProcMMcuBandwidthNotify( const TMt& tMMcu,const u32 dwBandWidth );
	void RestoreVidSrcBitrate(TMt tMt, u8 byMediaType);
	void ProcMcsMcuSetMtVolumeCmd(const CMessage * pcMsg);
    void MtVideoSourceSwitched(CServMsg & cServMsg);
    void ProcMtMcuVideoSourceSwitched(const CMessage * pcMsg);
    void ChangeMemberVolume( const TMt & tMember, u8 byVolume );
    void SetMemberVolumeReq( const TMt & tMember, u8 byVolume );
	BOOL32 SetMtInfoByMtExtU(const u16 &wMcuIdx, const TMtExtU &tMtExtU, TMcuMcuMtInfo &tMtInfo); //����MtExt������tMtInfo
	
    BOOL32 AssignMpForMt( TMt & tMt );
    void ReleaseMtRes( u8 byMtId );	
    void InviteUnjoinedMt( const CServMsg& cServMsgHdr, const TMt* ptMt = NULL, 
		                   BOOL32 bSendAlert = FALSE, BOOL32 bBrdSend = FALSE,
						   u8 byCallType = VCS_FORCECALL_REQ, BOOL bLowLevelMcuCalledIn = FALSE);

    void ProcMtCapSet( TMt tMt, TMultiCapSupport &tCapSupport, u8 bMtHasVidCap = 1);	

	BOOL32 IsMtCanCapabilityCheat( s8 *pachProductID );
	//zjl[20091208]�Ƴ�����ģ���е���ǽ��Ա
	void RemoveMtInMutiTvWallMoudle(TMt tMt);
	// [2013/08/13 chenbing] ���ָ��ͨ���ĳ�Ա 
	BOOL32 ClearHduChnnlInHduMoudle(u8 byHduId, u8 byHduChnId, u8 byHduSubChnId = 0, u8 byHduMode = HDUCHN_MODE_ONE);

	//lukunpeng 2010/07/23 �ն����뷢��/�������ϲ�
	void ProcMcsMcuChangeSpeakModeReq(const CMessage * pcMsg);
	void ProcMcsMcuGetSpeakListReq(const CMessage * pcMsg);

	// [pengjie 2010/4/16] ��ɾ�����Ҷϡ��ն˵��ߵ������������������е���Ϣ������������������Ӧ����
	void RemoveMtFormPeriInfo( const TMt &tMt, u8 byReason );
	// [yanghuaizhi 2012/7/12] ɾ���������ն�ʱ,�����������Ϣ
	void RemoveUnjoinedMtFormPeriInfo( const TMt &tMt);
	// Ϊ8KH��8KE����˫�������� [4/14/2012 chendaiwei]
	void ConstructDSCapExFor8KH8KE(const TCapSupport & tCapsupport, TVideoStreamCap *pTDSCapEx);

	//2������MCU-MCU
	void ProcMcuMcuRegisterNotify( const CMessage * pcMsg );
	void ProcMcuMcuRosterNotify( const CMessage * pcMsg );
	void ProcMcuMcuMtListReq( const CMessage * pcMsg );
	void ProcMcuMcuMtListAck( const CMessage * pcMsg );
	void ProcMcuMcuVideoInfoReq( const CMessage * pcMsg );
	void ProcMcuMcuVideoInfoAck( const CMessage * pcMsg );
	void ProcMcuMcuConfViewChgNtf( const CMessage * pcMsg );
	void ProcMcuMcuAudioInfoReq( const CMessage * pcMsg );
	void ProcMcuMcuAudioInfoAck( const CMessage * pcMsg );
	void ProcMcuMcuAddMtReq( const CMessage * pcMsg );
	void ProcMcuMcuDropMtNotify( const CMessage * pcMsg );
	void ProcMcuMcuDelMtNotify( const CMessage * pcMsg );
	void ProcMcuMcuSetMtChanReq( const CMessage * pcMsg);
	void ProcMcuMcuSetMtChanNotify( const CMessage * pcMsg);
	void ProcMcuMcuNewMtNotify( const CMessage * pcMsg);
	void ProcMcuMcuCallAlertMtNotify( const CMessage * pcMsg);

	void ProcMcuMcuSetInReq( const CMessage * pcMsg);
	void ProcMcuMcuSetOutReq( const CMessage * pcMsg);
	void ProcMcuMcuSetOutNotify( const CMessage * pcMsg);

	void ProcMcuMcuStartMixerCmd( const CMessage * pcMsg);
	void ProcMcuMcuStartMixerNotif( const CMessage * pcMsg);
	void ProcMcuMcuStopMixerCmd( const CMessage * pcMsg);
	void ProcMcuMcuStopMixerNotif( const CMessage * pcMsg);
	void ProcMcuMcuGetMixerParamReq( const CMessage * pcMsg);
	void ProcMcuMcuGetMixerParamAck( const CMessage * pcMsg);
	void ProcMcuMcuGetMixerParamNack( const CMessage * pcMsg);
	void ProcMcuMcuMixerParamNotif( const CMessage * pcMsg);
	void ProcMcuMcuAddMixerMemberCmd( const CMessage * pcMsg);
	void ProcMcuMcuRemoveMixerMemberCmd( const CMessage * pcMsg);

	void ProcMcuMcuLockReq( const CMessage * pcMsg);
	void ProcMcuMcuLockAck( const CMessage * pcMsg);
	void ProcMcuMcuLockNack( const CMessage * pcMsg);
	
	void ProcMcuMcuMtStatusCmd( const CMessage *pcMsg );
	void ProcMcuMcuMtStatusNotif( const CMessage *pcMsg );
    
    void ProcMcuMcuAutoSwitchReq( const CMessage *pcMsg );
    void ProcMcuMcuAutoSwitchRsp( const CMessage *pcMsg );
    void ProcMcuMcuAutoSwitchTimer( void );
	
	void CascadeAdjMtRes( TMt tMt,
						  u8 bNewFormat,
						  BOOL32 bStart = TRUE,
						  u8 byVmpStyle = ~0,
						  u8 byPos = ~0,
						  u32 dwResW = 0,
						  u32 dwResH = 0,
						  u8 byEqpId = 0,
						  u8 byHduChnId = 0);

	void CascadeAdjMtBitrate( TMt tMt, u16 wBitRate,BOOL32 bIsRecover = FALSE );
	// [10/18/2011 liuxu] �ָ��ն˵ķֱ���, Tvwall��Vmpͨ��
	void RecoverMtVFormat( const TMt&, const BOOL32 bRecoverSMcuViewMt = FALSE );
	// ���͵���ǽ���ն˵ķֱ���
	void DecreaseMtVFormatInTw( const TMt& );

	void ProcMcuMcuAdjustMtResReq ( const CMessage *pcMsg );	
	void ProcMcuMcuAdjustMtResAck ( const CMessage *pcMsg );	
	void ProcMcuMcuAdjustMtResNack( const CMessage *pcMsg );

    void ProcMcuMcuCancelMeSpeakerRsp( const CMessage *pcMsg );
	void ProcMcuMcuSpeakStatusNtf( const CMessage *pcMsg );
    void ProcMcuMcuApplySpeakerRsp( const CMessage *pcMsg );
	void ProcMcuMcuMuteDumbReq( const CMessage *pcMsg );


	void ProcMcuMcuAdjustMtBitRateCmd( const CMessage *pcMsg );


	void ProcMcuMcuMtOperateRsp( const CMessage *pcMsg );
	// [pengjie 2010/4/23] ��ͨ�ն˵�֡��(����)
	void SendChangeMtFps( u8 byMtId, u8 byChnnlType, u8 byFps );
	// [pengjie 2010/4/23] ������֡��
	void CascadeAdjMtFps( TMt tMt, BOOL32 bStart, u8 byChnnlType, u8 byFps = 0 );
	// [pengjie 2010/9/13] ��ͨ�ն˵��ֱ���
	BOOL32 SendChgMtVidFormat( u8 byMtId, u8 byChlType, u8 byFormat, BOOL32 bMmcuCmd = FALSE, BOOL32 bStart = TRUE, u32 wResW = 0, u32 wResH = 0);
	// End

	void SendChgMtFps(const TMt& tMt, u8 byChnnlType, u8 byFps,BOOL32 bStart = TRUE );
	void ProcMcuMcuAdjustMtFpsReq ( const CMessage *pcMsg );
	void ProcMcuMcuAdjustMtFpsAck ( const CMessage *pcMsg );	
	void ProcMcuMcuAdjustMtFpsNack( const CMessage *pcMsg );
	void SendSMcuMtInfoToMMcu( u16 wMcuIdx );
	void SendAllSMcuMtInfoToMMcu( void );
	void SendSMcuMtStatusToMMcu( u16 wMcuIdx );
	void SendAllSMcuMtStatusToMMcu( void );
	void SendSMcuUpLoadMtToMMcu(void);
	// End
	void OnStartMixToSMcu(CServMsg *pcSerMsg);
	void OnStopMixToSMcu(CServMsg *pcSerMsg);
	void OnGetMixParamToSMcu(CServMsg *pcSerMsg);
	void OnAddRemoveMixToSMcu(CServMsg *pcSerMsg, BOOL32 bAdd,BOOL32 bStopMixerNoMember = TRUE);

	void OnStartDiscussToAllSMcu(CServMsg *pcSerMsg);
	void OnStopDiscussToAllSMcu(CServMsg *pcSerMsg);

	void OnGetMtStatusCmdToSMcu( u8 byDstMcuId );
	void OnNtfMtStatusToMMcu( u8 byDstMcuId, u8 byMtId = 0 );
	void OnNtfDsMtStatusToMMcu(void);
	TMt  GetConfRealDsMt(void);
	// [11/15/2011 liuxu] ��ȡ���巢�ͱ����ն�״̬
	void OnBufSendMtStatusToMMcu( );

	void NotifyMcuNewMt(TMt tMt);
	void NotifyMcuDelMt(TMt tMt);
	void NotifyMcuDropMt(TMt tMt);
	
	void ProcMcuMcuSendMsgReq( const CMessage * pcMsg );

	// [10/19/2011 liuxu] ��ȡmcu��Mc��Ϣ
	TConfMcInfo *GetMcuMcInfo( const TMt& );
	BOOL32 GetMcData( u8 byMcuId, TConfMtInfo &tConfMtInfo, TConfMcInfo &tConfOtherMcInfo );

	//TMt  GetLocalMtFromOtherMcuMt( TMt tMt );
	
	TMt  GetMcuMediaSrc(u16 wMcuIdx);
	
	void OnMMcuSetIn(TMt tMt, u8 byMcsSsnId, u8 bySwitchMode);
    void OnSetOutView(const TMt &tMcuSetInMt, u8 byMode = MODE_BOTH);

	BOOL32 IsCanSetOutView( TMt &tSetInMt, u8 byMode = MODE_BOTH );
    // guzh [5/14/2007] ��ȡ��������Ƶ��Ϣ
    void GetConfViewInfo(u16 wMcuIdx, TCConfViewInfo &tInfo);
    void GetLocalAudioInfo(TCConfAudioInfo &tInfo,TMsgHeadMsg &tHeadMsg);    
	   
    // guzh [5/14/2007] ֪ͨ��������MCU����ý��Դ
    void NotifyAllSMcuMediaInfo(u8 byTargetMcuId, u8 byMode);
    void NofityMMcuMediaInfo();

	BOOL32 IsRosterRecved( const u16 wMcuIdx );

    //���Զ�δ򿪼���ͨ��
//     void ProcTimerReopenCascadeChannel();

    //mcu source
    void SendMcuMediaSrcNotify(BOOL32 bFource = FALSE);
    void ProcTimerMcuSrcCheck(const CMessage * pcMsg);
    
    void ProcMcuMtFastUpdateTimer( const CMessage * pcMsg );
    void ProcMcuEqpFastUpdateTimer( const CMessage * pcMsg );
    void ProcMcuMtSecVideoFastUpdateTimer( const CMessage * pcMsg );
    
    void ProcTimerChangeVmpParam(const CMessage * pcMsg);
    //[5/4/2011 zhushengze]VCS���Ʒ����˷�˫��
    void ProcMcuMcuChangeMtSecVidSendCmd(const CMessage * pcMsg);
	
	//[2/23/2012 zhushengze]���桢�ն���Ϣ͸��
    void ProcMcuMcuTransparentMsgNotify(const CMessage * pcMsg);

    //3��˫�����
    void StopDoubleStream( BOOL32 bMsgSrcMt, BOOL32 bForceClose,BOOL32 bIsNeedAdjustVmpParam = TRUE);
    void StartDoubleStream(TMt &tMt, TLogicalChannel &tLogicChnnl);
    void StartSwitchDStreamToFirstLChannel( TMt &tDstMt );
    void StopSwitchDStreamToFirstLChannel( TMt &tDstMt );
    BOOL32 JudgeIfSwitchFirstLChannel( TMt &tSrc, u8 bySrcChnnl, TMt &tDstMt );
    void ClearH239TokenOwnerInfo(TMt *ptMt);
    void UpdateH239TokenOwnerInfo(TMt &tMt);
    void NotifyH239TokenOwnerInfo(TMt *ptMt);
    void McuMtOpenDoubleStreamChnnl(TMt &tMt, const TLogicalChannel &tLogicChnnl/*, const TCapSupport &tCapSupport*/);//��˫��ͨ��
    void ProcSendFlowctrlToDSMtTimeout(void );
    void ProcSmcuOpenDVideoChnnlTimer(const CMessage * pcMsg);
	BOOL32 IsDStreamMtSendingVideo(TMt tMt);
	BOOL32	CheckDualMode(TMt &tMt);
    void  ShowToken();
    //4��ѡ�����
    //void RestoreMtSelectStatus( u8 byMtId, u8 bySelMode );       
    BOOL ChangeSelectSrc(TMt tSrcMt, TMt tDstMt, u8 byMode = MODE_BOTH);
    void StopSelectSrc(TMt tDstMt, u8 byMode = MODE_BOTH, u8 byIsRestore = TRUE,u8 byIsStopSwitch = TRUE );
	void AdjustFitstSrcMtBit(u8 byMtId,  BOOL32 bIsFirstBrdAdp, BOOL32 bIsCancelSel = FALSE);
    //ѡ��ģʽ��ѡ��Դ��Ŀ�ĵ��������Ƿ�ƥ��
    BOOL32 IsSelModeAndCapMatched( u8               &bySwitchMode,
                                   const TSimCapSet &tSimCapSrc,
                                   const TSimCapSet &tSimCapDst,
                                   BOOL32           &bAccord2Adp);
    void ProcMtSelectdByOtherMt( u8 byMtId, BOOL32 bStartSwitch  );    

    //5������
    void AdjustMtVideoSrcBR( u8 byMtId, u16 wBitrate, u8 byMode = MODE_VIDEO, BOOL32 bForceFlowCtrlSrc = FALSE);
    BOOL32 IsNeedAdjustMtSndBitrate(u8 byMtId, u16 &wSndBitrate,u8 byMediaMode = MODE_VIDEO);
    //BOOL32 IsMtMatchedSrc( u8 byMtId, u8 &byChnRes, BOOL32 &bAccord2MainCap);
	void	SwitchNewVmpToSingleMt(TMt tMt);
	BOOL32 GetMtMatchedRes(u8 byMtId, u8 byChnType, u8& byChnRes);
	BOOL32 GetMtLegalCapExAccord2CallBR(u16 wCallBR,TVideoStreamCap tSrcCap,TVideoStreamCap &tMatchedCap);
	// [pengjie 2010/4/23] �ж��Ƿ�����Ҫ����֡�ʵ��նˣ���7820/7920������Ϊ��Щ�ն�֡�ʿ��ܴﵽ50/60֡��
	//                     ����Щ�ط��ǲ�֧����ô�ߵ�֡�ʵģ��续��ϳ�ǰ���䣩
    BOOL32 IsNeedChangeFpsMt( u8 byMtId );
	BOOL32 IsNeedAdjustSpyFps(const TMt &tSrc, const TSimCapSet &tDstCap);
	//lukunpeng 2010/07/09 �ָ��¼��ն˵Ļ���ϳ�
	void RestoreSubMtInVmp(TMt& tMt);

	void RestoreMtInTvWallOrHdu( const TMt& tMt );

    //֪ͨת�����������ն˴�
    void Starth460PinHoleNotify( const u32 dwLocalAddr,
                                 const u16 wLocalPort,
                                 const u32 dwRemoteAddr,
                                 const u16 wRemotePort,
                                 const u8 byPinHoleType,
                                 const u8 byPayLoad);

    //֪ͨת����ֹͣ�������ն˴�
    void StopH460PinHoleNotifyByMtId( const u8 byMtId);
    void StopH460PinHole( const u8 byMpId, 
                          const u16 wLocalPort, 
                          const u32 dwRemoteAddr,
                          const u16 wRemotePort);

public:
    //TMt  GetVidBrdSrc(void);
	u16 GetLstRcvMediaResMtBr(BOOL32 bPrimary, u8 byMediaType, u8 byRes, u8 bySrcMtId = 0);
    //songkun,20110530,VMP���е���û�н��٣��Ҷϵ���û������
	u16 GetMinMtRcvBitByVmpChn(u8 byVmpId, BOOL32 bPrimary, u8 byVmpOutChnnl,u8 bySrcMtId = 0);

	BOOL32	GetSrcRealAbility(TMt &tSrc, u8 byMediaMode, TBasAdaptParamExt &tBasParamExt)
	{
		return GetBasParamExt(tSrc,byMediaMode,tBasParamExt);
	}
protected:
    void AdjustKedaMcuAndTaideMuteInfo( TMt * ptMt, BOOL32 bDecodeAud, BOOL32 bOpenFlag );    
    void AdjustKedaMcuAndTaideAudioSwitch( u8 byDstMtId, BOOL32 bDecodeAud, BOOL32 bOpenFlag );

    u8 	GetSpeakerSrcSpecType(u8 byMode);
    void SetSpeakerSrcSpecType(u8 byMode, u8 bySpecType);
    void ProcPolycomSecVidChnnl(CMessage * const pcMsg);

    //����ϵͳ������Ϣ������
    void ProcBuildCaseSpecialMessage(const CMessage * pcMsg);


    /************************************************************************/
    /*                                                                      */
    /*                       �ġ������������                             */
    /*                                                                      */
    /************************************************************************/

    //1��δ����
    void ProcMixerMcuRsp( const CMessage * pcMsg );
    void ProcMixerMcuNotif( const CMessage * pcMsg );
    void ProcMixerRspWaitTimer( const CMessage * pcMsg );
    void ProcMcsMcuListAllRecordReq( const CMessage * pcMsg );
    void ProcRecMcuProgNotif( const CMessage * pcMsg );
    void ProcRecMcuListAllRecNotif( const CMessage * pcMsg );
    void ProcRecMcuPlayChnnlStatusNotif( const CMessage * pcMsg );	
    void ProcRecMcuRecChnnlStatusNotif( const CMessage * pcMsg );
    void ProcRecMcuResp(const CMessage * pcMsg);
	void ProcRecMcuNeedIFrameCmd(const CMessage * pcMsg);     //����¼����ؼ�֡����  jlb  2008/12/18
    void ProcMcsMcuSeekReq( const CMessage * pcMsg );
    void ProcMcsMcuStopPlayReq( const CMessage * pcMsg );
    void ProcMcsMcuResumePlayReq( const CMessage * pcMsg );
    void ProcMcsMcuPausePlayReq( const CMessage * pcMsg );
    void ProcMcsMcuStartPlayReq( const CMessage * pcMsg );
    void StartPlayReq(CServMsg & cServMsg,u8 byNackTryNum =0);
    void ProcMcsMcuStopRecReq( const CMessage * pcMsg );
    void ProcMcsMcuResumeRecReq( const CMessage * pcMsg );
    void ProcMcsMcuPauseRecReq( const CMessage * pcMsg );
    void ProcMcsMcuStartRecReq( const CMessage * pcMsg );
    void ProcMcsMcuChangeRecModeReq( const CMessage * pcMsg );
    void ProcMcsMcuDeleteRecordReq( const CMessage * pcMsg );
    void ProcMcsMcuRenameRecordReq( const CMessage * pcMsg );
    void ProcMcsMcuPublishRecReq( const CMessage * pcMsg );
    void ProcMcsMcuCancelPublishRecReq( const CMessage * pcMsg );
    void ProcMcuRecConnectedNotif(const CMessage * pcMsg);
    void ProcMcuRecDisconnectedNotif( const CMessage * pcMsg );
    void ProcMcsMcuGetRecPlayProgCmd( const CMessage * pcMsg );
	void ProcRecPlayWaitMpAckTimer( void );
    
	//new bas
	//��������Ӧ�ӿ�ͳһ����
    void ProcBasMcuRsp( const CMessage * pcMsg );
	//�㲥������Ӧ����
	void ProBasMcuBrdAdpRsp(CServMsg &cServMsg);
	//ѡ��������Ӧ����
	void ProBasMcuSelAdpRsp(CServMsg &cServMsg);

    void ProcMcuBasDisconnectedNotif( const CMessage * pcMsg );
    //void ProcMcuBasConnectedNotif( const CMessage * pcMsg );
    void ProcBasMcuCommand(const CMessage * pcMsg);
    void ProcBasTestCommand( const CMessage * pcMsg );

	//[2011/08/27/zhangli]8kebas״̬֪ͨ����
	void ProcBasStatusNotif( const CMessage * pcMsg );
	void ProcBasChnnlStatusNotif( const CMessage * pcMsg );
    void ProcHdBasChnnlStatusNotif( const CMessage * pcMsg );
	void ProcAudBasChnnlStatusNotif( const CMessage * pcMsg );
	void ProcHduMcuChnnlStatusNotif( void );

    void ProcVmpMcuRsp( const CMessage * pcMsg ); 
    void ProcVmpMcuNotif( const CMessage * pcMsg ); 
    void ProcVmpRspWaitTimer( const CMessage * pcMsg );
	void ProcVMPStopSucRsp( u8 byVmpId );
    void ProcWaitAllVMPPrestInAckTimer( const CMessage * pcMsg );
	void ProcUnfinishedChangeVmpCmd( u8 byVmpId );
    
    void ProcVmpTwMcuRsp(const CMessage * pcMsg);
    void ProcVmpTwMcuNotif(const CMessage * pcMsg);
    
    void ProcPrsConnectedNotif( const CMessage * pcMsg );
    void ProcPrsDisconnectedNotif( const CMessage * pcMsg );
    void ProcPrsMcuRsp( const CMessage * pcMsg );
	void ShowPrsInfo();

    void ProcMcStopSwitch( const CMessage * pcMsg );
    void ProcMcsMcuStopSwitchTWReq( const CMessage * pcMsg );
    void ProcMcsMcuStartSwitchTWReq( const CMessage * pcMsg );
    void StartSwitchTWReq(CServMsg & cServMsg);
    void ProcTvwallConnectedNotif(const CMessage * pcMsg);
    void ProcTvwallDisconnectedNotif( const CMessage * pcMsg );
    void ProcMcsMcuAddMixMemberCmd( const CMessage * pcMsg );
    void ProcMcsMcuRemoveMixMemberCmd( const CMessage * pcMsg );
    void ProcMcsMcuReplaceMixMemberCmd( const CMessage * pcMsg );
	void ProcMtMcuApplyMixNotify( const CMessage * pcMsg );
    void ProcMcsMcuStopSwitchHduReq( const CMessage * pcMsg );      // ֹͣhdu����
    void ProcMcsMcuStartSwitchHduReq( const CMessage * pcMsg );     // ��ʼhdu����
	void ProcMcsMcuChangeHduVmpMode( const CMessage * pcMsg );		// �л�HDUͨ��ģʽ
    void ProcMcsMcuChangeHduVolume( const CMessage * pcMsg );       // ����hdu����
	void ProcHduConnectedNotif(const CMessage * pcMsg);             // hdu����֪ͨ
    void ProcHduDisconnectedNotif( const CMessage * pcMsg );        // ����֪ͨ
	//void ProcHduMcuStatusNotif( const CMessage * pcMsg );           // ״̬֪ͨ
    void ProcHduMcuNeedIFrameCmd( const CMessage * pcMsg );         // hdu����ؼ���
	void StartSwitchHduReq(CServMsg & cServMsg);
	void ProcHduMcuRsp( const CMessage * pcMsg );


    //2�����ֻ���������
	//tianzhiyong 2010/03/21 ���ӿ�������ģʽ������EAPU�¸�֪����ģʽ��EAPU�»��������������ɵ���������
	//�Լ��ֶ��������������� byEqpId ���ֶ������Զ�������������0 �� �Զ� ��0 ��������EQPID Ĭ���Զ�
	BOOL32 StartMixing(u8 byMixMode , u8 byEqpId = 0);
	void StopMixing();
	//tianzhiyong 2010/03/21 ����EQPID�жϸ������Ƿ�֧�ֵ�ǰ������Ƶ����
	BOOL32 CheckMixerIsValide(u8 byMixerId);
	//tianzhiyong 2010/03/21 ���ӻ�ȡ������ȫ��ͨ���ӿڣ�EAPU�¸���������ȫ��ͨ�������������������Ա���仯���仯
	u8  GetMixerNModeChn();
	BOOL32 StartEmixerVac( u8 byEmixerId );
	void ProcMixStopSucRsp( BOOL32 bIsSendToMMcuNotify = TRUE ); // ������ֹͣ�ɹ���mcu��Ӧ��ҵ����
	BOOL32 AddMixMember( const TMt *ptMt, u8 byVolume = DEFAULT_MIXER_VOLUME, BOOL32 bForce = FALSE );
	BOOL32 AddMixMember( const u8 byMtNum, const TMt *ptMt, u8 byVolume = DEFAULT_MIXER_VOLUME, BOOL32 bForce = FALSE );
	void RemoveMixMember( const TMt *ptMt, BOOL32 bForce = FALSE );
	void AddRemoveMixRtcpMember(TMt tMt, BOOL32 IsAdd = FALSE);
	void AddRemoveMixRtcpMember(u8 byDstMtNum, TMt *ptDstMt, BOOL32 IsAdd = FALSE);
	void SetMemberVolume( const TMt *ptMt, u8 byVolume );
	BOOL32 NotifyMixerSendToMt( BOOL32 bSend = TRUE );
	void SetMixerSensitivity( u8 bySensVal );
    void SetMixDelayTime(u16 wDelayTime);
    void SwitchMixMember(const TMt *ptMt, BOOL32 bNMode = FALSE);
	void SwitchMixMember( BOOL32 bNMode );
    void VACChange(const TMixParam &tMixParam, u8 byExciteChn);
	void UpdateUploadMtMixingToMcs(TMt tLocalSmcuMt,CServMsg &cServMsg);
	BOOL32 IsNeedCheapMcsMtMixing(TMt tUnLocalMt);
    
	BOOL32 AddSpecMixMember(const TMt *ptMt = NULL, u8 byMtNum = 0, BOOL32 bAutoInSpec = TRUE);

	BOOL32 RemoveSpecMixMember(const TMt *ptMt = NULL, u8 byMtNum = 0, BOOL32 bCancelForceMix = FALSE, BOOL32 bStopMixNonMem = TRUE);

	//�˺��������ٱ��ⲿ���ã���������������AddSpecMixMember ��RemoveSpecMixMember����
	BOOL32 AddRemoveSpecMixMember(const TMt *ptMt, u8 byMtNum, BOOL32 bAdd, BOOL32 bStopMixNonMem = TRUE, BOOL32 bAutoInSpec = TRUE, BOOL32 bCancelForceMix = FALSE, BOOL32 bIsReplace = FALSE);
    
	void SwitchDiscuss2SpecMix();

    void MixerStatusChange(void); 
    u8   GetMixMtNumInGrp(void);
    u8   GetMaxMixNum(u8 byMixerId);    //��ȡ��������Ա������Ŀǰ����ɢ����4��������56����
//    u8   GetMixChnPos( u8 byMtId, BOOL32 bRemove = FALSE ); //��ȡ������Աͨ��λ�ã��������ֽ����˿ں�
	
	//  [12/1/2009 pengjie] ��ش�֧�֣���¼����һ��δ��local��tmt��Ϣ
	//u8   GetMixChnPos( TMt &tSrc, BOOL32 bRemove = FALSE );
	 
	//  [12/3/2009 pengjie] �ͷ�ĳ���ն���ռ�õĻ���ͨ����bForce = TRUE �ͷ����л���ͨ����Ĭ��ΪFALSE
	//BOOL32 FreeMixChn( TMt &tSrc, BOOL32 bForce = FALSE );
	//  [12/4/2009 pengjie] �ͷ�ĳ����ش��ն�ռ�õĻ�����Դ
	//BOOL32 FreeSpyMixerMember( TMt &tSrc );

    u8   GetMaxMixerDepth( u8 byMixerId ) const;
	void ChairmanAutoSpecMix();     //������ϯ�Զ��������ƻ���
//    void ChangeConfRestoreMixParam();
	BOOL32 RestoreMixingByOtherMixer( u8 byMixMode );
    u16  GetCurConfMixMtNum(u16 wMcuIdx = INVALID_MCUIDX);
	void ClearSMcuAllMtMixStatusToAuto(u16 wMcuIdx = INVALID_MCUIDX);
	void GetMtListGroupBySmcu(TMt *ptMtIn, u8 byMtNum, u8 &bySmcuNum, u8 *pbyMtNum = NULL, TMt *ptMtOut = NULL);
	
	/*---------------------------�������-----------------------*/
	//�Ƿ���Ҫ����
	BOOL32 IsNeedAdapt(TMt tSrc, TMt tDst, u8 byMediaMode);
	//�Ƿ���Ҫѡ������
	BOOL32 IsNeedSelAdpt(TMt tSrc, TMt tDst, u8 byMediaMode);
	//�Ƿ���Ҫ�ش�����
	BOOL32 IsNeedSpyAdpt(const TMt &tSrc, const TSimCapSet &tDstCap, u8 byMediaMode);
	//�����㲥����
	BOOL32 StartBrdAdapt(u8 byMediaMode);
	//ֹͣ�㲥����
	BOOL32 StopBrdAdapt(u8 byMediaMode,BOOL32 bSelSelf = TRUE);
	//����Դ��Ŀ��˫������
	//BOOL32 StartDSAdapt(TMt tSrc, TMt tDst);
	//ֹͣԴ��Ŀ��˫������
	//BOOL32 StopDSAdapt(TMt tSrc, TMt tDst);
	//����ѡ������
	BOOL32 StartSelAdapt(TMt tSrc, TMt tDst, u8 byMediaMode, BOOL32 bInnerSelect = TRUE);
	//�����ش�����
	BOOL32 StartSpyAdapt(TMt tSrc, const TSimCapSet &tDstCap, u8 byMediaMode);
	BOOL32 StartSpyAudAdapt(TMt tSrc);
	//ֹͣѡ������
	BOOL32 StopSelAdapt(TMt tSrc, TMt tDst, u8 byMediaMode, BOOL32 bIsStopSwitchToMt = TRUE, BOOL32 bIsStopSpySwitch = TRUE);
	//ֹͣ�ش�����
	BOOL32 StopSpyAdapt(TMt tSrc, const TSimCapSet &tDstCap, u8 byMediaMode);
	//���BAS����������Ϣ
	BOOL32 GetBasParamExt(TMt &tSrc, u8 byMediaMode, TBasAdaptParamExt &tBasParamExt);
	//��������ͨ������
	BOOL32 ChangeAdapt(const TEqp &tEqp, u8 byChnId);
	//����bas��prs��֧��
	void   StartBasPrsSupport(const TEqp &tEqp, u8 byChnId);
	//ֹͣbas��prs��֧��
	void   StopBasPrsSupport(const TEqp &tEqp, u8 byChnId);
	//����bas��prs��֧��()
	void   StartBasPrsSupportEx(u8 byMediaMode);
	//����������ش�bas��prs��֧��(���¼�ʹ��)������BAS��Դ��RTCP����
	//void   StartMultiSpyBasPrsSupport(const TEqp &tEqp, u8 byChnId, u8 byChnMode, TMt tBasSrc);
	//ֹͣ����ͨ������
	BOOL32 StopBasAdapt(const TEqp &tEqp, u8 byChnId,BOOL32 bSelSelf = TRUE);
	//����¼�񱸷ݻָ�ʱֹͣ���䣬��ͣ������REC�Ľ���
	BOOL32 StopBasAdaptOnlyForRecAdp(const TEqp &tEqp, u8 byChnId);
	//��������ͨ�����佻��
	BOOL32 StartBasSwitch(const TEqp &tEqp, u8 byChnId);
	//�������ͨ�����佻��
	BOOL32 StopBasSwitch(const TEqp &tEqp, u8 byChnId,BOOL32 bSelSelf = TRUE);
	//ֹͣ���й㲥����
	void   StopAllBrdAdapt();
	//ֹͣ����ѡ������
	void   StopAllSelAdapt();
	//�ͷŵ�����ռ�õ�������Դ
	void   ReleaseBasChn();
	//����ˢ�¹㲥�������
	BOOL32 RefreshBasBrdAdpParam(u8 byMediaMode);
	//����ˢ����Ƶ�������
	BOOL32 RefreshBasParam4VidBrd( u16 wBiteRate );
	//����ˢ��˫���������
	BOOL32 RefreshBasParam4DSBrd( u16 wBiteRate );
	//����ˢ����Ƶ�������
	BOOL32 RefreshBasParam4AudBrd(u16 wBiteRate);
	//������������ն���Ϣˢ�¹㲥�������
	BOOL32 RefreshBrdBasParam4AllMt(u8 byMediaMode);
	//���ݵ����ն���Ϣˢ�¹㲥�������
	BOOL32 RefreshBrdBasParamForSingleMt(u8 byMtId, u8 byMediaMode, BOOL32 bUpAdjust = FALSE, BOOL32 bIncludeSelf = FALSE);
	//���ݵ����ն���Ϣˢ��ѡ���������
	BOOL32 RefreshSelBasParam(TMt tSrc, TMt tDst, u8 byMediaMode, BOOL32 bUpAdjust = FALSE, BOOL32 bIncludeSelf = FALSE);
	//���ݵ����ն���Ϣˢ�»ش��������
	BOOL32 RefreshSpyBasParam(TMt tSrc, TSimCapSet &tDstCap, u8 byMediaMode, BOOL32 bUpAdjust = FALSE,BOOL32 bIncludeSelf = FALSE);
	//����¼��ˢ���������
	BOOL32 RefreshBasParamForLowBrUse();
	//��ȡ����¼���������
	BOOL32 GetLowBrUseBasOutPutInfo(TEqp &tBas, u8 &byChnId, u8 &byOutNum, u8 &byOutIdx);
	//��ȡ����ģʽ
	u8     GetAdaptMode(u8 byMediaMode);
	//Ϊ�㲥��Ҫ��������ն�Ѱ�Һ��ʵ�bas���
	BOOL32 FindBasChn2BrdForMt(u8 byMtId,  u8 byMediaMode, TBasOutInfo &tOutInfo);
	//Ϊѡ����Ҫ��������ն�Ѱ�Һ��ʵ�bas���
	BOOL32 FindBasChn2SelForMt(TMt tSrc,  TMt tDst,  u8 byMediaMode, TBasOutInfo &tOutInfo);
	//Ϊѡ������ͨ��
	BOOL32	GetBasChnForSel(const TMt &tSrc, TSimCapSet &tDstCap, u8 byMediaMode,CBasChn **pcBasChn, TBasChnCapData &tChnCapData, BOOL32 bRecursive = TRUE);
	BOOL32  GetBasChnForAudSel(const TMt &tSrc, TAudioTypeDesc &tDstAudCap,CBasChn **pcBasChn, TBasChnCapData &tChnCapData);
	//Ϊ��ش�����ͨ��
	BOOL32	GetBasChnForMultiSpy(const TMt &tSpySrc,TSimCapSet &tDstCap,u8 byMediaMode,CBasChn **pcBasChn, TBasChnCapData &tChnCapData);

	//����Ŀ������Ϊѡ����Ҫ��������ն�Ѱ�Һ��ʵ�bas���(��ش���)
	BOOL32 FindBasChn2SpyForMt(TMt tSrc,  const TSimCapSet &tDstCap,  u8 byMediaMode, TBasOutInfo &tOutInfo);
	//ͨ����������BAS��BASͨ�����Ҷ�ӦBAS���
	BOOL32 FindBasOutIdxByVidCapSet(TEqp &tBas, u8 byChnId, u8 byMediaMode,TVideoStreamCap &tSimCapSet,u8 &byOutIdx, u8 &byFrontOutNum);
	//ͨ����������BAS��BASͨ�����Ҷ�ӦBAS���
	BOOL32 FindBasOutIdxByAudCap(TEqp &tBas, u8 byChnId,TAudioTypeDesc &tAudCap,u8 &byOutIdx, u8 &byFrontOutNum);
	//Ϊ����������ͨ��Ѱ�Һ��ʵ����
	BOOL32 FindBasChnForReplace(TEqp *ptDiscBas, u8 byDiscNum, CBasChn **ptRplChn, TBasChnCapData *ptBasChnCapData);
	//Ϊ����������¼��ͨ��Ѱ�Һ��ʵ����
	BOOL32 FindBasChnForRecAptReplace(u8 byMediaMode);
	//ˢ�±���BAS����ͨ���Ĳ���
	BOOL32 RefreshBasParamForReplace(CBasChn* pBasChn,const TBasChnCapData &tChnEncCapData,u8 byMediaMode);
	//����Դ��Ŀ����Ϣ�ж��Ƿ���ͷŶ�Ӧ����ͨ��
	BOOL32 IsCanRlsBasChn(const TMt &tRlsSrc, const TMt &tSpySrc, const TSimCapSet &tDstCap, u8 byRlsMode);
	//���ݻش�Դ��Ϣ��Ҫ������ķֱ��ʣ����µ������佻��
	BOOL32 AdjustSpyStreamFromAdp(TMt tSpySrc, u8 byRes, TMsgHeadMsg tHeadMsg);
	BOOL32 AdjustSpyStreamFromAdpForMMcu(TMt tSpySrc, u8 byRes, TMsgHeadMsg tHeadMsg);
	//����Դ��Ŀ��������һ���ܸ��������ֱ���ǿˢ������ͨ��
	BOOL32	FindBasChnForAdjustSpyStream(const TMt &tSpySrc, const TSimCapSet &tDstCap, u8 byMediaMode,TBasOutInfo &tOutInfo);
	//�Ƿ���Ҫ����ѡ��BAS����
	BOOL32 IsNeedAdjustSelParam(const TMt &tSrc,const TMt &tDst,TBasOutInfo &tOutInfo, u8 byMediaMode,CBasChn **pcBasChn, TBasChnCapData &tChnCapData);

	// ����basԴ�������ý�����ͻ�ȡbas�Ľ�������
	const BOOL32 GetBasDecPT(const TMt& tBasSrc, const u8 byMode, u8& byVidBasDecPT, u8& byAudBasDecPt, TLogicalChannel &tSrcLgcChn );

	//[nizhijun 2011/02/22]�Ƿ�����˫��ʽ����BAS��ѡ��
	BOOL32 IsSelInDoubleMedia(const TMt &tSrc,const TMt &tDst);	
	/*-------------------------------------------------------*/

	/*---------------------------¼�����-----------------------*/
	BOOL32	UnPackStartRecMsg(CServMsg &cServMsg, TStartRecMsgInfo &tRecMsg,u16* pwErrCode); //��Ϣ����
	
	void	SendStartRecReq(const TStartRecMsgInfo &tStartRecMsg,const TStartRecData &tRecData,const TSpyRecInfo *ptSpyRecInfo = NULL);

	//����¼��
	BOOL32	IsConfRecCheckPass(TStartRecMsgInfo &tRecMsg,u16* pwErrCode);					//����¼��У��
	void	PrepareDataForConfRec(const TStartRecMsgInfo &tRecMsg,TStartRecData &tRecData);	//��������¼������
	void	StartConfRec(const TStartRecMsgInfo &tRecMsg);									//��������¼��

	//�ն�¼��
	BOOL32	IsMtRecCheckPass(TStartRecMsgInfo & tRecMsg,u16* pwErrCode);					 //�ն�¼��У��
	void	PrepareDataForMtRec(const TStartRecMsgInfo &tRecMsg, TStartRecData &tRecData);	 //�����ն�¼������			
	void	StartMtRec(const TStartRecMsgInfo &tRecMsg, BOOL32 bIsNeedPreSetIn = TRUE,const TSpyRecInfo *ptSpyRecInfo = NULL);//�����ն�¼��

	//vrs��¼��֧��
	void	StartVrsRec(const TStartRecMsgInfo &tRecMsg, u8 byRecState, BOOL32 bNeedPresetin = TRUE); //����vrs��¼��¼��
	u8		GetVrsRecMtId(TMt tRecSrcMt);// ͨ��¼���ն�ƥ���Ӧ��¼��ʵ��MtId
	u8		AddVrsMt(TMtAlias &tMtAlias, u8 byType, TMt tSrc, u16 wMtDialRate = 0, u8 byCallMode = CONF_CALLMODE_NONE);//���vrs��¼����Ա
	void	ReleaseVrsMt(u8 byVrsMtId, u16 wErrorCode = 0, BOOL32 bMcsMsg = TRUE);// �ͷ�Vrs��¼��������Ϣ��״̬��ΪIDLE
	BOOL32  CheckVrsMtCapSet(TMt tMt, TMultiCapSupport &tCapSupport);// ��¼�������Ƚ�
	BOOL32  PrepareStartRecMsgInfo(TMt tRecSrcMt, TStartRecMsgInfo &tStartVrsMsgInfo);//׼��Ҫ����vrs��¼������Ϣ
	void	SendStartVrsRecReq(const TStartRecMsgInfo &tStartRecMsg); //����vrs��¼��¼��
    void	ProcRecMcuVrsRecChnnlStatusNotif( CServMsg &cServMsg ); //vrs��¼��¼��״̬�ϱ�����
	void	ProcMcsMcuGetRecChnnlStatusCmd(const CMessage * pcMsg); //��ѯ¼���״̬
	void	ProcGetVrsListRecord(TMtAlias &tMtAlias, TMt tVrs, u8 byListType, u32 dwGroupId, u32 dwPageNo, u32 dwListNum);//��¼�������б���
	void	ProcVrsMcuListAllRecNotif(const CMessage * pcMsg);//vrs�����б��ϱ������������б����ļ��б�
	void	ProcMcsMcuStartVrsPlayReq(const CMessage * pcMsg);//����vrs����������
	void	StartVrsPlay(TMtAlias &tMtAlias, TPlayEqpAttrib tPlayAttrib);//����vrs����,�������д���
    void	ProcRecMcuVrsPlayChnnlStatusNotif( const CMessage * pcMsg ); //vrs��¼������״̬�ϱ�����
	void	PrintVrsInfo(void);//��ӡ��ǰvrs��Ϣ
	BOOL32	IsVrsRecMt(const TMt &tMt);//�Ƿ���vrs��¼��ʵ��
	void	ProcMcsMcuCloseListRecordCmd(const CMessage * pcMsg); //��عر��ļ��б����ڹҶ�vrs��¼��ʵ�壩
	BOOL32	StartSwitchToRecorder(TMt tSrc, u16 wSrcChnnl, TEqp tRec, u8 byDstChnnl, u8 byMode, u8 bySwitchMode,
								  BOOL32 bStopBeforeStart, BOOL32 bAddRtcp); //��¼�����������֧��¼���������vrs��¼��
	void	StopSwitchToRecorder(TEqp tRec, u8 byDstChnnl, BOOL32 bMsg, u8 byMode);//��¼����Ľ�����֧��¼���������vrs��¼��
	BOOL32	GetVrsCallAliasByVrsCfgId(u8 byVrsId, TMtAlias &tVrsAlias);


	BOOL32 IsRecNeedAdapt(u8 byMediaMode = MODE_BOTH);		// �ж��Ƿ��û��Զ�������¼��
	BOOL32 IsRecAdaptUseBas();								//�ж��û��Զ�������¼���Ƿ���Ҫ������
	BOOL32 InitRecAdptParam();	    						// ���Զ����ʽ¼��ĸ���������г�ʼ��
	BOOL32 InitRecAdptParamByMediaMode(u8 byMediaMode);
	void ResetRecAdptParam(u8 byMediaMode = MODE_BOTH);     			// ���Զ����ʽ¼��ĸ��������������
	BOOL32 AdjustRecAdptParam(u8 byMediaMode);							// ���Զ����ʽ�������е���
	void OnMtRecDStreamSrcChanged(const BOOL32 bIsStart);
	/*-------------------------------------------------------*/

	
    //void   ProcBasStartupTimeout( const CMessage * pcMsg );
    BOOL32 IsMtSrcVmp2(u8 byMtId);
    BOOL32 IsNeedMdtpAdapt( TSimCapSet &tDstSCS, TSimCapSet &tSrcSCS );
public:
    u16    GetLeastMtReqBitrate(BOOL32 bPrimary = TRUE, u8 byMediaType = MEDIA_TYPE_NULL, u8 bySrcMtId = 0);
    u16    GetLeastMtReqBR(TSimCapSet &tSrcSCS, u8 bySrtMtId = 0, BOOL32 bPrimary = TRUE);
    u8     GetLeastMtReqFramerate(u8 byMediaType = MEDIA_TYPE_NULL, u8 bySrcMtId = 0);
	BOOL32 GetMtRealSrc(u8 byMtId, u8 byMode, TMt& tMtSrc, u8& byOutChnl);
	BOOL32 GetMtCurVidSrc(u8 byMtId, TMt& tMtSrc, u8& byOutChnl);
protected:
    BOOL32 IsRecordSrcBas( u8 byType, TMt &tRecSrc, u8 &byChnId, u8 &byRecInIdx);

    //4 ������������غ���
	void   GetDSBrdSrcSim(TDStreamCap &tDSim);

    BOOL32 IsDSSrcH263p( void );
    BOOL32 GetProximalGrp(TVideoStreamCap &tSimCapSet);

	void   ReleaseResbySel(TMt tSelDstMt, u8 bySelMode, BOOL32 bStopDstSwitch = TRUE);
    //old
    //BOOL32 IsRecordSrcHDBas( void );

	/*----------------------------------5.PRS��غ���---------------------------*/
	//Ϊ��������ԤռPrsͨ��
	BOOL32 AssignPrsChnnl4Conf();
	//�����㲥prs
	void StartBrdPrs(u8 byMediaMode);
	//ֹͣ�㲥prs
	void StopBrdPrs(u8 byMediaMode);
	//�л�prsԴ
	BOOL32 ChangePrsSrc(u8 byPrsId, u8 byPrsChnId, u8 byMediaMode, TMt tPrsSrc, u8 byPrsSrcOutChn = 0);
	//��ȡprsԴ��rtcp������ַ
	BOOL32 GetRemoteRtcpAddr(TMt tRtpSrc, u8 byRtpSrcOutChn, u8 byMediaMode, TTransportAddr &tRemoteAddr);
    //vmp��̬����prs
	void StartPrsForVmp(TEqp tVmpEqp);
	//mixer��̬ռ��prs
	void StartPrsForMixer(void);
	//vmp��ֹ̬ͣ�ͷ�prs
	void StopPrsForVmp(TEqp tVmpEqp);
	//mixer��ֹ̬ͣ�ͷ�prs
	void StopPrsForMixer(void);
	//����Ŀ���ն˵�prs��rtcp����
	void AddRemovePrsMember(u8 byMemId, u8 byPrsId, u8 byPrsChl, u8 byPrsMode = MODE_VIDEO, u8 bAdd = TRUE,u16 wSpyStartPort = SPY_CHANNL_NULL);
	//20110412 rtcp���
	void AddRemovePrsMember(u8 byDstMtNum,
								TMt *pDstMt,
								u8 byPrsId,
								u8 byPrsChl,
								u8 byPrsMode = MODE_VIDEO , 
								u8 bAdd  = TRUE ,
							    u16 wSpyStartPort  = SPY_CHANNL_NULL);

	//�����Ե�ǰprsͨ��ΪĿ�ĵ�rtcp����
	void BuildRtcpAndRtpSwithForPrs(u8 byPrsId, u8 byPrsChl);
	//ֹͣ����prsͨ������
	void StopPrs(u8 byPrsId, u8 byPrsChnl, u8 byChnMode);
	//ֹͣ��ǰ��������prsͨ������
	void StopAllPrsChn();
	//�ͷŵ�ǰ����ռ�õ�����prsͨ��
	void ReleaseAllPrsChn();
	void AddRemoveRtcpToDst(u32		dwDstIp,
							u16		wDstPort, 
                            TMt		tSrc,
							u8		bySrcChnnl = 0, 
		                    u8		byMode = MODE_VIDEO,
							BOOL32	bMapSrcAddr = FALSE,
							u16		wSpyStartPort = SPY_CHANNL_NULL,
							BOOL32	bAddRtcp = TRUE
							/*u8	byHduSubChnId = 0*/);

    //[liu lijiu][20100902]��������������ش�ͨ����RTCP����
	//void SwitchLocalMultipleSpyRtcp( TMt tSrc, u8 bySrcChnnl, TMt tDst, u8 byMode, BOOL32 bIsBuildSwitch);
	void BuildRtcpSwitchForSrcToDst(TMt tSrc, TMt tUnlocalDst, 
								 u8 byMediaMode, u8 bySrcChnnl = 0, 
								 /*u8 bySwitchMode = SWITCH_MODE_BROADCAST, */
								 u8 byDstChnnl = 0,
								 BOOL32 bAddRtcp = TRUE);
	//ͣ��Ŀ���ն˵�rtcp����
	void StopPrsMemberRtcp(TMt tDstMt, u8 byMode);
	//ֹͣ��Ŀ��ʵ�������Rtcp����
	void StopRtcpSwitchAlltoOne(const TMt &tRtcpDst, u8 byDstChn, u8 byMode);
	

	
	//BOOL32 GetMtRealSrc(u8 byMtId, u8 byMode, TMt& tMtSrc, u8& byOutChnl);
	void   OccupyPrsChnl(u8 byPrsId, u8 byPrsChnl);
	void   RlsPrsChnl(u8 byPrsId, u8 byPrsChnl);

    //6��VMP��غ��� 
	void ProcMcsMcuVmpPriSeizeRsp(const CMessage *pcMsg);//vmp��ռӦ��
	void ProcMtSeizeVmpTimer(const CMessage *pcMsg);	//���Ҫ��ռ��MT������ռ����
	void ProcVmpBatchPollTimer(void);	//vmp����poll
	void ProcMcsMcuVmpCmd(const CMessage * pcMsg);		//��������poll PAUSE,RESUME������
	void ProcStartVmpReq(CServMsg &cServMsg);			//��ʼ����ϳ�
	void StartVmpBrd(TEqp tVmpEqp);							//��ʼMPU-VMP�㲥����
	void StopVmpBrd(TEqp tVmpEqp);					//ֹͣMPU-VMP�㲥����
	void ProcChangeVmpParamReq(CServMsg &cServMsg);		//�ı仭��ϳɲ���
	void ProcStartVmpBatchPollReq(CServMsg &cServMsg);	//batchpoll
	void ClearOneVmpMember(u8 byVmpId, u8 byChnnl, TVMPParam_25Mem &tVmpParam, BOOL32 bRecover = FALSE);
	void RefreshVmpChlMemalias(u8 byVmpId);//ˢ��vmp����ͨ��̨��
	void GetSmcuMtAliasInVmp(u8 byVmpId); //��û���ϳ����¼��ն˱���(��������֧��)
public:
	u8	GetVmpOutChnnlByRes(u8 byVmpId, u8 byRes,  u8 byMediaType = MEDIA_TYPE_H264, u8 byFrameRate=0, emProfileAttrb emAttrb=emBpAttrb);
	u8	GetResByVmpOutChnnl(u8 byVmpId, u8 &byMediaType, u8 &byFrameRate, u8 &byAttrb, u8 byChnnl);
	u8  GetVmpOutChnnlByDstMtId(u8 byDstMtId, u8 byVmpId, BOOL32 bNeedCheckSelSrc = TRUE);//׷�Ӳ�����
	u8  GetVmpSpecialOutChnnlByDstMtId(u8 byDstMtId);
	u8  GetMtNumSeeVmp(u8 byVmpId, u8 byChnnl, u8 byNeglectMtId = 0);
    BOOL32 GetMtAliasToVmp(TMt tMt, s8 *pchAlias);     // vmp����ն˱������ݣ�����������ʾ�ϳ�ͼƬ [7/4/2013 liaokang]
protected:	
      
	
	BOOL32 CheckMpuMember(u8 byVmpId, TVMPParam_25Mem& tVmpParam, u16& wErrorCode, BOOL32 bChgParam = FALSE);
	u16 CheckMpuAudPollConflct(u8 byVmpId, TVMPParam_25Mem& tVmpParam, BOOL32 bChgParam = FALSE);
	u16 CheckSameMtInMultChnnl(u8 byVmpId, TVMPParam_25Mem & tVmpParam, BOOL32 bChgParam = FALSE);
	BOOL32 CheckAdpChnnlLmt(u8 byVmpId, TVMPParam_25Mem& tVmpParam, CServMsg &cServMsg, BOOL32 bChgParam = FALSE);
	BOOL32 CheckAdpChnnlAtStart(u8 byVmpId, TVMPParam_25Mem &tVmpParam, u8 byMaxHdChnnlNum, CServMsg &cServMsg);
	BOOL32 CheckAdpChnnlAtChange(u8 byVmpId, TVMPParam_25Mem &tVmpParam, u8 byMaxHdChnnlNum, CServMsg &cServMsg);
	
	// yanghuaizhi ����ն���vmp�е�����λ��,����mcu�ϴ��ն����⴦��
	void GetChlOfMtInVmpParam(const TVMPParam_25Mem& tVmpParam, TMt tMt, u8 &byPosNum, u8 *pbyPos = NULL);

	BOOL32 CheckVmpParam(const u8 byVmpId, TVMPParam_25Mem& tVmpParam, u16& wErrorCode);
    void ChangeVmpStyle(TMt tMt, BOOL32 bAddMt/*, BOOL32 bChangeVmpParam = TRUE*/);
	// 2011-7-29 mod by peng guofeng : �Ƴ�����
    void ChangeVmpSwitch( u8 byVmpId, u8 byState = VMP_STATE_START );
	// 2011-7-29 mod
	void CancelOneVmpFollowSwitch( u8 byMemberType, BOOL32 bNotify );
    void ChangeVmpChannelParam(TMt * ptNewSrc, u8 byMemberChnl, TMt * ptOldSrc = NULL);
    void ChangeVmpBitRate(u16 wBitRate, u8 byVmpChanNo = 1); 
    void AdjustVmpBrdBitRate(TMt *ptMt = NULL);
    void AdjustVmpSelBitRate(TEqp tVmpEqp);//����ѡ��vmp��ָ��ͨ������
	u8   GetVmpChnnl( void );
	void SetVmpChnnl(u8 byVmpId, TMt tMt, u8 byChnnl, u8 byMemberType, BOOL32 bMsgMcs = FALSE );
	BOOL32 IsDynamicVmp( u8 byVmpId );
	u8   GetVmpDynStyle(u8 byVmpId, u8 byMtNum );

// ����һ����Ϊ�����Ľӿڣ������ж��м����ϳɳ�Ա����ʲô��ӽ��ķ��
	u8   GetVmpStylebyMtNum( u8 byMtNum ); 

	// 2011-7-29 mod by peng guofeng : �Ƴ��������
    BOOL32 IsChangeVmpParam(/*TMt * ptMt*/);
	// 2011-7-29 mod end
    BOOL32 IsVMPSupportVmpStyle(u8 byStyle, u8 byEqpId, u16 &wError);
    BOOL32 IsMCUSupportVmpStyle(u8 byStyle, u8 &byEqpId, u8 byEqpType, u16 &wError );
    
	// yanghuaizhi 20111130 ��ռ��ʾ��Ϣ���
	void SendVmpPriSeizeReq(u8 byVmpId, TMt &tSeizeMt, u8 byForceSeize = 0);
	// yanghuaizhi 20111201 �Ƿ�����ͬһ�ն˽����ͨ��
	BOOL32 IsAllowVmpMemRepeated(const u8 byVmpId);
	// yanghuaizhi 20111209 �ж�ѡ���vmp�Ƿ��ܱ��ݾɵ�vmp
	BOOL32 IsVmpSupportBackup(TVMPParam_25Mem &tVmpParam, u8 byOldVmpId, u8 byNewVmpId);
	// yanghuaizhi 20111209 ����vmpid�����֧�����ǰ�������
	u8 GetMaxHdChnlNumByVmpId(const TConfInfo &tConfInfo, u8 byVmpId, u8 &byMaxStyleNum);
	// yanghuaizhi 20120215 (8KH��ֲ)�ж��Ƿ���8kh1080p/30fps��������鿪����vmp
	BOOL32 Is8khVmpInConf1080p30fps(const TConfInfo &tConfInfo, u8 byVmpId);
	// yanghuaizhi 20111201 ����ģ��׼��vmp�ĺ����������б�,���ж��Ƿ񳬳�����
	BOOL32 PrepareVmpOutCap(u8 byVmpId, BOOL32 bBrd = TRUE);
	// yanghuaizhi 20111201 ����ģ��׼��vmp�ĺ����������б�,���ж��Ƿ񳬳�����
	BOOL32 PrepareMpu2VmpOutCap(u8 byVmpId);
	// yanghuaizhi 20130801 ����ģ��׼��8kivmp�Ĺ㲥�����������б�8KIVMP�㲥��ǹ㲥���벻ͬ
	BOOL32 Prepare8kiVmpOutCap(u8 byVmpId, BOOL32 bBrd);
	// yanghuaizhi 20111212 ����vmpoutparam��õ�ǰvmp��Ҫ��vicp��Դ
	s32 GetVicpResByVmpOutParam(const TKDVVMPOutParam &tVMPOutParam);
	// yanghuaizhi 20111219 ����ǰ������Ϣ,����Ҫռ����δռ��ǰ����ĳ�Ա,���п���,��ռǰ����
	void AdjustVmpHdChnlInfo(const TEqp &tVmpEqp);
	// yanghuaizhi 20120709 ֪ͨ��ϯ�ն�ˢ��vmpparam��Ϣ
	void SendVmpParamToChairMan();
	// yanghuaizhi 20120731 ����vmp˫��������ʵ˫��Դ,�ϱ����
	void UpdateVmpDStream(const TMt tMt);
	// yanghuaizhi 20130305 �ж��Ƿ�Ϊ��ȷ��VmpId������mcuutility.h��
	//BOOL32 IsValidVmpId(u8 byVmpId);
	// yanghuaizhi 20130305 ��VmpParam��Ϣ��PeriVmpParam�и��µ�ConfVmpParam
	void UpdateConfVmpParam(u8 byVmpId);
	// yanghuaizhi 20130305 ������ͨ����Ա�ֱ���
	void AdjustMtResInLastVmpParam(u8 byVmpId);
	// yanghuaizhi 20130320 ���vcs�л�����ʱ��Ĭ��VmpParam��Ϣ
	TVMPParam_25Mem GetVcsRestoreConfVmpParam(void);
	// yanghuaizhi 20130320 �жϵ�ǰ����ռ�õĺϳ������Ƿ���ָ��UseState�ĺϳ���
	BOOL32 IsVmpStatusUseStateInVmpList(u8 byUseState);
	// yanghuaizhi 20130320 ��ý��е�Ĭ�Ϻϳ���
	u8 GetTheOnlyVmpIdFromVmpList(void);
	// yanghuaizhi 20130320 ���������ռ�úϳ�������
	u8 GetVmpCountInVmpList();
	// yanghuaizhi 20130320 ��ָ��vmp��ӵ�����
	void AddVmpIdIntoVmpList(u8 byVmpId);
	// yanghuaizhi 20130320 ��ָ��vmp������ɾ��
	void RemoveVmpIdFromVmpList(u8 byVmpId);
	// yanghuaizhi 20130321 �жϻ���vmpList���Ƿ���ָ��VmpId
	BOOL32 IsVmpIdInVmpList(u8 byVmpId);
	// ����Ϣ�����л��tVmpParam��Ϣ
	BOOL32 GetVmpParamFormBuf(u8 *pbyBuf, u16 wBufLen, TVMPParam_25Mem &tVmpParam);
	// �ж��Ƿ���Ҫ�������ն˷ֱ���,����Ҫ��������Ҫ�����ķֱ���(�ṩ�ӿ�)
	BOOL32 IsNeedAdjustMtVidFormat(const TMt &tMt, TExInfoForResFps &tExInfoForRes);
	// �ж��Ƿ���Ҫ�������ն�֡��,����Ҫ��������Ҫ������֡��(�ṩ�ӿ�)
	BOOL32 IsNeedAdjustMtVidFps(TMt &tMt, TExInfoForResFps &tExInfoForFps);
	// yanghuaizhi 20130325 ����ն���vmp����Ϣ��ռ��ͨ��������Сͨ���ֱ��ʡ��Ƿ�Ҫ�����ֱ��ʣ�
	BOOL32 IsMtNeedAdjustResInConfVmp(const TMt &tMt, TExInfoForResFps &tExInfoForRes, u8 &byVmpChlRes, u8 &byChlCount);
	// yanghuaizhi 20130325 �ж��ն������Ƿ���vmp��vip
	TChnnlMemberInfo IsVipMemForVmp(const TMt &tMt, TExInfoForResFps &tExInfoForRes);
	// yanghuaizhi 20130326 ��������ϳ����ն˷ֱ���
	BOOL32  ChangeMtResFpsInVmp(u8 byVmpId, TMt tMt, TVMPParam_25Mem *ptVMPParam, BOOL32 bOccupy = TRUE, u8 byMemberType = VMP_MEMBERTYPE_NULL, u8 byPos = MAXNUM_VMP_MEMBER, BOOL32 bMsgMcs = FALSE);
	// yanghuaizhi 20130326 ��ĳ�ն���vmp������ǰ����
	void ClearAdpChlInfoInAllVmp(TMt tMt);
	// yanghuaizhi 20130327 ���һ����Ҫvmp�����ڵ���ʱ�ж���ϳ�����ָ��һ����������
	u8 GetTheMainVmpIdFromVmpList();
	// yanghuaizhi 20130325 ����ն��Ƿ��ڽ�֡�ʵ�vmp��
	BOOL32 IsMtNeedAdjustFpsInConfVmp(TMt &tMt, TExInfoForResFps &tExInfoForFps);
	// ���vmp��Ӧͨ������
	u16 GetVMPOutChlBitrate(TEqp tVmpEqp, u8 byIdx);
	// ����vmp��Ӧͨ������
	void SetVMPOutChlBitrate(TEqp tVmpEqp, u8 byIdx, u16 wBr);
	// ����vmp��Ӧ�㲥ģʽ
	void SetVMPBrdst(TEqp tVmpEqp, u8 byVMPBrdst);
	// ΪN+1����֧�ֶ໭��ϳ��ṩ�����ʵ�vmp����ӿ�
	BOOL32 IsVmpSupportNplus(u8 byVmpSubType, u8 byVmpStyle, u8 &byEqualAbilityVmpId, u8 &bySlightlyLargerAbilityVmpId, u8 &byMinAbilityVmpId);
	// yanghuaizhi 20130412 ��������ϳ����ն˷ֱ���
	BOOL32  ChangeMtResFpsInTW(TMt tMt, BOOL32 bOccupy = TRUE);
	// yanghuaizhi 20130417 �ж�ĳMCU�Ƿ��Ǽ򵥼�����MCU
	BOOL32 IsNotSupportCascadeMcu(const TMt &tMt);
	// yanghuaizhi 20130417 ��ϯ�ն��Ƿ���ѡ������ϳ�
	BOOL32 IsVmpSeeByChairman();
	// yanghuaizhi 20130425 vmp����ն˱���������������ʾ�ϳ�ͼƬ
	//BOOL32 GetMtAliasInVmp(TMt tMt, s8 *pchAlias);
    BOOL32 GetMtAliasInVmp(TMt tMt, s8 *pchAlias, u8 byMaxAliasLen = VALIDLEN_ALIAS );    // ���� [7/4/2013 liaokang]
	// yanghuaizhi 20130425 �ն˿�ͨ������ģ�濪���Ļ��ָ������ϳ�
	void AdjustCtrlModeVmpModuleByMt(const TMt &tMt);
	// yanghuaizhi 20130527 �¼��Ҷϱ����ϴ�ͨ���ն�
	void ProcMMcuSpyMtByRemoveMt( const TMt &tRemoveMt );
	// yanghuaizhi 20130527 �����ϳ�������
	void ProcStopVmpReq(CServMsg &cServMsg);
	// yanghuaizhi 20130527 �����ϳɹ㲥������
	void ProcStartVmpBrdReq(CServMsg &cServMsg);
	// yanghuaizhi 20130527 ֹͣ�ϳɹ㲥������
	void ProcStopVmpBrdReq(CServMsg &cServMsg);
	// yanghuaizhi 20130527 vmp�㲥���������
	void ChgSpeakerResInVmpBrd();
	// yanghuaizhi 20130527 8kh�ϳɹ㲥��Ϊ�ǹ㲥ʱ���⴦��
	void AdjustVmpOutChnlInChgVmpBrd(TEqp tVmpEqp, BOOL32 bStartVmpBrd);
	// yanghuaizhi 20130527 ���տ�̨�����ĺϳ���Ϣ���и�ʽ��
	void FormatVmpParamFromMTCMsg(TVMPParam_25Mem &tVmpParam);
	// yanghuaizhi 20130527 ����ϳɿ���ʱ�ĸ���Check
	BOOL32 CheckStartVmpReq(u8 &byVmpId, TVMPParam_25Mem &tVmpParam, BOOL32 bIsVmpModule, CServMsg &cServMsg, u16 &wError);
	// yanghuaizhi 20130527 �Һ���vmpid����ָ��vmpidʱ���Ƿ�֧��
	BOOL32 FindVmpSupportVmpParam(u8 &byVmpId,u8 byVmpStyle, u8 byVmpCount, u16 &wError);
	// yanghuaizhi 20130527 ���ݻ�����Ϣ����vmp��Ա
	void UpdateVmpMembersWithConfInfo(const TEqp &tVmpEqp, TVMPParam_25Mem& tVmpParam);
	// yanghuaizhi 20130527 �����ָ���ĳ�Ա�Ƿ��е��ش�mcu�µ��ն�ռ�ö��ͨ�������
	u16 UpdateVmpMembersWithMultiSpy(const TEqp &tVmpEqp, TVMPParam_25Mem& tVmpParam);
	// yanghuaizhi 20130529 ����ϳɵ���ʱ�ĸ���Check
	BOOL32 CheckChangeVmpReq(u8 &byVmpId, TVMPParam_25Mem &tVmpParam, u16 &wError);
	// yanghuaizhi 20130529 ��vmpͨ�������ն˵Ľ���,���ն˷��齨����
	void StartVmpSwitchGrp2AllMt(const TEqp &tVmpEqp);
	// yanghuaizhi 20130529 ����ϳɼ�����ش�����
	BOOL32 SendPreSetInReqForVmp(const TEqp &tVmpEqp, TVMPParam_25Mem *ptVmpParam, BOOL32 bStart, BOOL32 &bSendPreSetIn);
	// yanghuaizhi 20130529 ׼�������跢����Ϣ�Ĺ�ͨ����
	TVmpCommonAttrb GetVmpCommonAttrb(u8 byVmpId, TVMPParam_25Mem *ptVMPParam, BOOL32 bStart);
	// yanghuaizhi 20130529 ���8kgvmp��Ӧ�����Ϣ
	u8 GetResBy8KEVmpOutChnnl(u8 &byMediaType, u8 byChnnl);
	// yanghuaizhi 20130529 ���8khvmp��Ӧ�����Ϣ
	u8 GetResBy8KHVmpOutChnnl(u8 &byMediaType, u8 &byFrameRate, u8 &byAttrb, u8 byChnnl);
	// yanghuaizhi 20130529 ���mpu��Ӧ�����Ϣ
	u8 GetResByMpuOutChnnl(u8 &byMediaType, u8 byBoardVer, u8 byChnnl);
	// yanghuaizhi 20130529 ���ݷֱ��ʻ��8kgvmp��Ӧ���ͨ��
	u8 Get8KEVmpOutChnnlByRes(u8 byRes,  u8 byMediaType);
	// yanghuaizhi 20130529 ���ݷֱ��ʻ��8khvmp��Ӧ���ͨ��
	u8 Get8KHVmpOutChnnlByRes(const TEqp &tVmpEqp, u8 byRes,  u8 byMediaType, u8 byFrameRate, emProfileAttrb emAttrb);
	// yanghuaizhi 20130529 ���ݷֱ��ʻ��mpu��Ӧ���ͨ��
	u8 GetMpuOutChnnlByRes(u8 byRes,  u8 byMediaType, u8 byBoardVer);
	// yanghuaizhi 20130530 VMP��MCU��ʼ����֪ͨ
	void ProcVmpMcuStartVmpNotify(CServMsg &cServMsg);
	// yanghuaizhi 20130530 VMP����֪ͨ
	void ProcVmpConnectedNotify(CServMsg &cServMsg);
	// yanghuaizhi 20130530 VMP����֪ͨ
	void ProcVmpDisconnectedNotify(CServMsg &cServMsg);
	// yanghuaizhi 20130530 ֹͣVMP������н���
	void StopVmpSwitch(const TEqp &tVmpEqp);
	// yanghuaizhi 20130530 ���ָ��ģʽ����������
	void ChangeCtrlModeVmpSwitch(const TEqp &tVmpEqp, u8 byState,TVMPParam_25Mem &tVMPParam);
	// yanghuaizhi 20130530 �Զ�ģʽ����������
	void ChangeAutoModeVmpSwitch(const TEqp &tVmpEqp, u8 byState,TVMPParam_25Mem &tVMPParam);
	// yanghuaizhi 20130531 ����ͨ���ϳɳ�Ա�����ж�
	BOOL32 IsVmpChannelParamChged(const TEqp &tVmpEqp, TVMPParam_25Mem &tVmpParam, TMt * ptNewSrc, u8 byMemberType, TMt * ptOldSrc);
	// yanghuaizhi 20130531 ���ݱ���ռѡ����в���
	void ExecuteMtBeSeizedChoice(const TEqp &tVmpEqp, TVMPParam_25Mem &tVMPParam, TSeizeChoice &tSeizeChoice);
	// yanghuaizhi 20130531 ����ռ�ն˴���
	void ExecuteMtSeizeChoice(const TEqp &tVmpEqp, TVMPParam_25Mem &tVMPParam,const TMt &tSeizeMt);
	// yanghuaizhi 20130531 �ն�ѡ������ϳ�Check
	BOOL32 CheckTMtSeeVmp(const TEqp &tVmpEqp, const TMt &tDstMt, BOOL32 &bIsChairManSeeVmp, u16 &wError);
	// yanghuaizhi 20130531 ����vpu�㲥����
	void AdjustVpuBrdBitRate(TMt *ptMt);
	// yanghuaizhi 20130531 ����mpu�㲥����
	void AdjustMpuBrdBitRate();
	// yanghuaizhi 20130531 vmp�����ж��¼����ش��³�Աִ��OnMMcuSetIn
	void OnMMcuSetInForVmp(TVMPParam_25Mem &tConfVMPParam, TVMPParam_25Mem &tPeriVmpParam);
	// yanghuaizhi 20130708 �Ժϳɳ�Աͨ�����򣬰�������>���ɽ��ֱ���Mt>�����ն�
	void GetVmpMemberSort(TVMPParam_25Mem &tConfVMPParam, u8 &byLen, u8 *pbyMemSort);
	// yanghuaizhi 20131105 ���ն��Ƿ�Ҫ�����ⷽʽ����
	BOOL32 IsRcvH264DependInMark(u8 byMtId);
	
	//lukunpeng 2010/07/16 ��Աת������
	u8 BatchTransVmpMemberToTVMPMemberEx(TVMPMember *ptVmpMemberArray, u8 byMaxMemberNum, TVMPMemberEx *ptVmpMemberExArray, u8 byMaxMemberExNum);
	void AdjustVmpParamMember(u8 byVmpId, TVMPParam_25Mem *ptVMPParam, u8 byVmpSubType, BOOL32 bStart);
	void	AdjustVmpParam(u8 byVmpId, TVMPParam_25Mem *ptVMPParam, BOOL32 bStart = FALSE, BOOL32 bNeedPreSetIn = TRUE);
	BOOL32  CheckSelSrcVmpAdaptChnl(TMt tSrcMt, TMt tDstMt, u8 SelMode, TEqp &tVmpEqp);
	BOOL32  ChgMtVidFormatRequired(TMt tMt, BOOL32 bNeglectVmpSubType = FALSE );
	BOOL32  ChangeMtVideoFormat( TMt   tMt, BOOL32 bConsiderVmpBrd = TRUE, BOOL32 bNonKeda = FALSE, BOOL32 bSeizePromt = FALSE);  //���ָ��λ�õ��ֱ���
	void	ChgMtFormatInSdVmp( u8 byMtId, TVMPParam * ptVMPParam, BOOL32 bStart = TRUE );	//��Ա���VMP�ĵ����ֱ���
	//BOOL32 GetMtFormat(const TMt &tMt, TVMPParam *ptVmpParam, u8 &byNewFormat, BOOL32 bStart = TRUE, BOOL32 bSeize = TRUE, BOOL32 bConsiderVmpBrd = TRUE, BOOL32 bNonKeda = FALSE);
	BOOL32 GetMtFormat( u8 byVmpId,
						const TMt &tMt,
						TVMPParam_25Mem *ptVmpParam,
						u8 &byNewFormat,
						BOOL32 bConsiderVmpBrd = TRUE,
						BOOL32 bNonKeda = FALSE,
						BOOL32 bForceVip = FALSE
						);
	BOOL32 GetMtFormat(const TMt &tMt, u8 &byNewFormat, BOOL32 bConsiderVmpBrd);
	
	BOOL32 GetMtFormatInSdVmp(const TMt &tMt, TVMPParam *ptVmpParam, u8 &byNewFormat, BOOL32 bStart = TRUE);
	BOOL32  ChgMtFormatInSdVmp( TMt &tMt,	
								TVMPParam *ptVmpParam,	
								BOOL32 bStart = TRUE,
								u8 byMemberType = VMP_MEMBERTYPE_NULL
								);	//�½ӿ�
	//	BOOL32  ChgMtFormatInMpu( u8 byMtId,	TVMPParam *ptVmpParam,	BOOL32 bStart = TRUE, BOOL32 bSeize = TRUE, BOOL32 bConsiderVmpBrd = TRUE, BOOL32 bNonKeda = FALSE );
	
	BOOL32 GetMtFormatInMpu(const TMt &tMt,
							TVMPParam_25Mem *ptVmpParam,
							u8 &byNewFormat,
							CVmpChgFormatResult &cVmpChgFormatRes,
							BOOL32 bStart,
							BOOL32 bConsiderVmpBrd,
							BOOL32 bNonKeda,
							BOOL32 bForceVip = FALSE,
							u8 byMemberType = VMP_MEMBERTYPE_NULL);
	
	
	
	BOOL32  ChgMtFormatInMpu (	TMt &tMt,	
								TVMPParam_25Mem *ptVmpParam,	
								BOOL32 bStart, 
								const TExInfoForInVmpRes &tExInfoForInVmpRes,
								u8 &byNewFormat
								);

	u8 GetMtFpsInVmp( const TMt &tMt, u8 byRes, BOOL32 bStart = TRUE);
		

	BOOL32  ChgMtFpsInVmp(	u8 byVmpId,
							const TMt &tMt,
							BOOL32 bStart,
							u8 byNewFormat
							);

	BOOL32 GetMtAlias(TMt tMt, s8 *pchAlias);
	BOOL32	RecoverMtResInMpu(TMt tMt);
	void	AddToVmpNeglected( u8 byMtId);
	BOOL32	IsMtNeglectedbyVmp( u8 byMtId);
	void	ProcMcsMcuStartSwitchVmpMtReq(const CMessage * pcMsg);
	void	TMtSeeVmp(CServMsg &cServMsg, BOOL32 bNeglectVmpMode = FALSE);//�ն�ѡ������ϳ�
	void	MCSSeeVmp(CServMsg &cServMsg, BOOL32 bNeglectVmpMode = FALSE);//20120329 yhz ���ѡ������ϳɴ���
	void	AdjustVmpParambyMemberType(u8 byVmpId, const TMt *ptSpeaker,TVMPParam_25Mem &tVmpParam, const u8& byMembertype = VMP_MEMBERTYPE_SPEAKER,const TMt *ptOldSrc = NULL);
    void	UpdataVmpEncryptParam(TMt tMt, u8 byChnlNo, u8 byVmpEqpId = 0);
	BOOL32  VidResAdjust(u8 byVmpId, u8 byVmpStyle, u8 byPos, u8 byMtStandardFormat, u8 &byReqRes);
    BOOL32  IsDelayVidBrdVCU(void);	

	u8		GetVmpSubType( u8 byEqpId );

	void ChangeVmpParam(u8 byVmpId, TVMPParam_25Mem *ptVMPParam = NULL, BOOL32 bStart = FALSE );
	void ConstructMsgAndSendtoVmp(u8 byVmpId, CParamToVmp &cParamToVmp, BOOL32 bStart = FALSE);

    //7��vmp tvwall��غ��� 
	BOOL32 IsMtInTvWall(const TMt &tMt);	
    void ChangeVmpTwChannelParam(const TMt * ptNewSrc, const u8& byMemberType);
    void ChangeVmpTwParam(TVMPParam *ptVmpParam = NULL, const BOOL32& bStart = FALSE);
	// 2011-7-29 mod by peng guofeng : �Ƴ�����
    void ChangeVmpTwSwitch(/*u8 bySrcType = 0, */u8 byState = VMP_STATE_START);
	// 2011-7-29 mod end
    void SetVmpTwChnnl(TMt tMt, u8 byChnnl, u8 byMemberType, BOOL32 bMsgMcs = FALSE);
	// 2011-11-3 add by pgf
	u8 GetVmpChnnlNumBySpecMemberType(u8 byMemberType, TMt* ptMt = NULL);
	void GetTvWallChnnlNumBySpecMemberType(u8 byMemberType, u8 &byVidNum, u8 &byAudNum);
	void GetHduChnnlNumBySpecMemberType(u8 byMemberType, u8 &byVidNum, u8 &byAudNum);

    //8��tvall��غ��� 
    void ChangeTvWallSwitch(TMt * ptSrc, u8 byTvWallId, u8 byChnlIdx,u8 byTWMemberType, u8 byState, BOOL32 bNeedPreSetin = TRUE, u8 byMode = MODE_BOTH);
	BOOL32 IsMtNotInOtherTvWallChnnl(u8 byMtId, u8 byTvWallId, u8 byChnlId);
	BOOL32 IsMtNotInOtherTvWallChnnl(TMt tMt, u8 byTvWallId, u8 byChnlId,BOOL32 bIsExceptSelf = FALSE,BOOL32 bIsFakeInTvWall = TRUE);
	BOOL32 IsHasMtInHduOrTwByMcuIdx(u16 wSmcuIdx);
    
    //9��recorder��غ��� 
	//void StopAllMtRecoderToPeriEqp();
    void AdjustConfRecordSrcStream(u8 byMode, BOOL32 bIsStart);
	void AdjustMtRecSrcStream(u8 byMode, const TEqp &tEqp, u8 byRecChl, const TMt &tSrc, BOOL32 bIsStart);
	void FindMtAndAdjustMtRecSrcStream( const TMt &tMcu,const u8 byMode,const BOOL32 bIsStart );
	void AddRemoveRecRtcp(u8 byMode, const TEqp &tEqp, u8 byRecChl, const TMt &tSrc, u8 byRecChnlIdx, BOOL32 isAdd);
	
// [pengjie 2010/9/29] ��������¼��״̬
	void AdjustConfRecByBrdSrc( void );
// End
	
	//10��Hdu��غ���
	BOOL32 ChangeHduSwitch(TMt/*const*/ * ptSrc, 
		const u8 byHduId, 
		const u8 byChnlIdx, 
		const u8 byHduSubChnId,
		const u8 byHduMemberType, 
		const u8 byState, 
		const u8 byMode = MODE_BOTH, 
		const BOOL32 bBatchPoll = FALSE,
		const BOOL32 bCanSendStopPlayMsg = TRUE,
		const BOOL32 bNeedPreSetin = TRUE,
		const u8 byMulitModeHdu = HDUCHN_MODE_ONE
		);

	// [9/21/2011 liuxu] ��ȡhduͨ����ʵ������Ƶģʽ
	u8 GetHduChnnlRealMode( const u8 byHduId, const u8 byChidx);

    BOOL32 IsMtNotInOtherHduChnnl(const u8 byMtId, const u8 byHduId, const u8 byChnlId, const u8 bySubChnId = 0);
	BOOL32 IsMtNotInOtherHduChnnl(const TMt tMt, const u8 byHduId, const u8 byChnlId,BOOL32 bIsExceptSelf = FALSE, BOOL32 bIsFakeInHdu = TRUE);
	BOOL32 IsHduChnnlMcsSpec(const TMt tMt, const u8 byHduId, const u8 byChnlId, const u8 bySubChnId = 0);
	BOOL32 HduBatchPollOfOneCycle( const BOOL32 bFirstCycle );            // hduһ�����ڵ�������ѯ
	void   StopHduBatchPollSwitch( const BOOL32 bStopPlay = FALSE );
	BOOL32 CheckHduAbility(TMt tSrc, const u8 byHduId, const u8 byChnId, u8 *pbyVmpOutChl = NULL,BOOL32 bIsSecVideo = FALSE);
	BOOL32 CheckTwAbility(TMt tSrc, const u8 byTwId, const u8 byChnId);

	BOOL32 ChangeMtResFpsInHduVmp(TMt tMt,
								  u8 byHduId, 
								  u8 byHduChnId,
								  u8 byHduSubChnId,
								  BOOL32& bIsStartSwitch,
								  BOOL32 bIsStart = TRUE,
								  BOOL32 bForceAdjust = FALSE
								  );
	// [2013/03/11 chenbing]
	void ChangeHduVmpMode(CServMsg cServMsg);
	void SendChangeHduModeReq(CServMsg cServMsg);

	// [2013/04/11 chenbing]
	BOOL32 IsOldViewMtAndNewViewMtSame(TMt tNewtMt, TMt tOldtMt);
	BOOL32 IsCanAdjustMtRes(TMt tMt);
	BOOL32 IsCanAdjustMtFps(TMt tMt);
	BOOL32 IsViewMtInHduVmp(TMt tMt, TMt *ptOldTMt = NULL, BOOL32 bIsNewViewMt = TRUE);
	BOOL32 IsMtInHduVmp(TMt tMt,
						u8 byHduId = 0,
						u8 byHduChnId = 0,
						u8 byHduSubChnId = 0,
						BOOL32 bIsIgnoreCurChn = FALSE,
						BOOL32 bIsIgnoreCurEqpId = FALSE,
						BOOL32 bIsIgnoreCurState = FALSE
						);
	BOOL32 IsBatchPollMtInHduVmp( TMt tMt,BOOL32 bIsIgnoreCurState = FALSE );
	TSimCapSet GetHduChnSupportCap(TMt tSrc, u8 byHduId = 0, u8 byMode = MODE_BOTH);
	void   StopViewToAllHduVmpSwitch(TMt tMt);

	// [5/25/2011 liuxu] ���Vcs����ǽ��������ؽӿ�
	// ��������(���������õ�hduͨ��������)��ȡhduͨ��״̬
	BOOL32 GetConfHduChnnlInfo( const u8 byChnnlIdx, THduChnStatus& tHduChnnStatus, THduMember& tHduMember);
	
	// ���ݻ������õ�hduͨ���Ż�ȡ��Ӧ��ͨ��(hdu)
	BOOL32 GetConfHduChnnl( const u8 byChnnlIdx, CConfTvwChnnl& cChnnl);
	// ���ݻ������õ���ͨ����ǽͨ���Ż�ȡ��Ӧ��ͨ��(tvwall)
	BOOL32 GetConfTvwChnnl( const u8 byChnnlIdx, CConfTvwChnnl& cChnnl);
	// ���ݻ������õĵ���ǽ��Ż�ȡ��Ӧ�ĵ���ǽͨ��(hdu or tvwall)
	BOOL32 GetCfgTvwChnnl( const u8 byChnnlIdx, CConfTvwChnnl& cChnnl);

	// �����豸�ź�ͨ�����Ҹõ���ǽͨ���ڻ����ж�Ӧ��ͨ����
	u8 GetConfCfgedTvwIdx( const u8 byEqpId, const u8 byTvwIdx );

	// Ѱ��Ϊĳ���¼�mcu�����hduͨ��. ��һ����������¼����ӿ�ͳһΪFindTvwChnnlByMcu
	u8 FindHduChnnlByMcu( const TMt& tSubMt, CConfTvwChnnl* pacChnnl = NULL, const u8 byChnnlNum = 0);
	// Ѱ��Ϊĳ���¼�mcu�����tvwallͨ��. ��һ����������¼����ӿ�ͳһΪFindTvwChnnlByMcu
	u8 FindTvwChnnlByMcu( const TMt& tSubMt, CConfTvwChnnl* pacChnnl = NULL, const u8 byChnnlNum = 0);
	// Ѱ��Ϊĳ���¼�mcu����ĵ���ǽͨ��, ����˳����hdu->tvwall->hdtvall
	u8 FindAllTvwChnnlByMcu( const TMt& tSubMt, CConfTvwChnnl* pacChnnl = NULL, const u8 byChnnlNum = 0);

	// Ѱ��ԴΪtSrc��HDU����ǽͨ��
	u8 FindHduChnnl( const TMt& tSrc, CConfTvwChnnl* pacChnnl, const u8 byChnnlNum );
	// Ѱ��ԴΪtSrc��Tvwall����ǽͨ��
	u8 FindTvwChnnl( const TMt& tSrc, CConfTvwChnnl* pacChnnl, const u8 byChnnlNum );
	// Ѱ��ԴΪtSrc�����е���ǽͨ��
	u8 FindAllTvwChnnl( const TMt& tSrc, CConfTvwChnnl* pacChnnl, const u8 byChnnlNum );

	// ��ȡ���������õ����е���ǽͨ��
	u8 GetAllCfgedTvwChnnl( CConfTvwChnnl* pacChnnl, const u8 byChnnlNum );

	// �����豸�ź�ͨ���Ż�ȡͨ��
	BOOL32 GetTvwChnnl( const u8 byTvwEqpId, const u8 byChnnlIdx, CTvwChnnl& cChnnl );
	
	// �ն��Ƿ���Ҫ�ı�ֱ���(�ն˽�����˳���ͨ Tvwall�豸ʱ��)
	BOOL32 NeedChangeVFormat( const TMt& tMt );

	// Ѱ����һ������ǽͨ��
	u16 FindNextTvwChnnl( const u16 wCurChnnlIdx, 
		const TMt* ptNextMt = NULL, 
		CConfTvwChnnl* pcNextChnnl = NULL,
		const BOOL32 bUseSingleSpyFirst = TRUE);
	// [5/25/2011 liuxu] VCS����ǽ��ؽӿ�������

	// [pengjie 2011/9/21] ˢ��ĳ�ն˵ĵ���ǽ״̬��֧��ˢ�¸������ǽ
	void RefreshMtStatusInTw( TMt tMt, BOOL32 bInTw, BOOL32 bTypeHdu, u8 byHduMemberType = TW_MEMBERTYPE_NULL );
	// [10/12/2011 liuxu] �ж��¼��ն��Ƿ��ǵ���ǽͨ����mcu��Ա���ϴ���Ա
	BOOL32 IsSMcuViewMtAndInTw( const TMt&, const BOOL32 IsHdu );
	
	// ˢ����������ǽͨ��״̬, ���ش�ʱ��Ҫ������ͬ���ն����ڵ���ǽͨ������
	void UpdateTvwOnSMcuViewMtChg( const TMt& tNewSMcuViewMt, const BOOL32 bHduEqp, const TMt* ptOldViewMt = NULL );
	
	// �ϴ�ͨ���ն˸ı�ʱ���¼��ͨ���ն���Ϣ
	void UpdateMontiorOnSMcuViewMtChg( const TMt& tNewSMcuViewMt);
	// [7/21/2011 liuxu] ��hdu/tvwall��ص����κλ���ģʽ��صĵĲ�������
	// ����ͨ����Ա���ͻ��Ա��ֹͣ�����ڵ�HDUͨ���Ľ���
	void FindConfHduHduChnnlAndStop( TMt* const ptTvwMember = NULL, 
		const u8 byTvwMemberType = TW_MEMBERTYPE_NULL,
		const BOOL32 bStopRunning = FALSE );

	// �������е���ǽͨ��,������Ա����ptOldTvwMember��byTvwMemberType,����Ľ���
	void FindConfHduChnnlAndReplace( TMt* const ptNewTvwMember,
		TMt* const ptOldTvwMember = NULL, 		
		const u8 byTvwMemberType = TW_MEMBERTYPE_NULL,
		const u8 byMediaMode = MODE_BOTH );
	// [7/21/2011 liuxu] Over

	void FindConfTvWallChnnlAndStop( TMt* const ptTvwMember = NULL, 
		const u8 byTvwMemberType = TW_MEMBERTYPE_NULL,
		const BOOL32 bStopRunning = FALSE );

	// [pengjie 2010/8/26] ��������ǽ��ѯ
	BOOL32 ProcHduBatchPollSwitch( TMt tFstMt, TMt tNextMt, 
		const THduChnlPollInfo &tHduChnlPollInfo, 
		const BOOL32 bFirstCycle,
		BOOL32 bIsPoll = FALSE);

	BOOL32 GetNextNeedPollMt( const TMt &tCurrMt, TMt &tNextMt );
	// End

	// �޸ĵ���ǽ������ѯBug�����ӿ�
	BOOL32 FindBatchPollMt(TMt& tCurrt, BOOL32 bIsFindNext = FALSE);

	BOOL32 HasUnLocalMemberInEqp(u8 byEqpId, u8 byMemberType = 0); 

	// 11��SMS
	void ProcSmsPackTimer( void );

    /************************************************************************/
    /*                                                                      */
    /*                        �塢mp/mtadp����                              */
    /*                                                                      */
    /************************************************************************/
    void ProcMpMessage( CMessage * const pcMsg );
    void ProcMpDissconnected( CMessage * const pcMsg );
	void ProcMtAdpDissconnected( CMessage * const pcMsg );	


    /************************************************************************/
    /*                                                                      */
    /*                       ����������������                             */
    /*                                                                      */
    /************************************************************************/
	
    //1���ն˽���
// 	BOOL32 StartSwitchToSubMt(TMt tSrc, 
//                               u8 bySrcChnnl,
//                               u8 byDstMtId,
//                               u8 byMode = MODE_BOTH,
//                               u8 bySwitchMode = SWITCH_MODE_BROADCAST,
//                               BOOL32 bMsgStatus = TRUE,
//                               BOOL32 bSndFlowCtrl = TRUE,
//                               BOOL32 bIsSrcBrd = FALSE,
// 							  u16 wSpyPort = SPY_CHANNL_NULL );
    //����1��n Rtp����
	BOOL32 StartSwitchToAll( TMt tSrc, 
							const u8 bySwitchGrpNum,
							TSwitchGrp *pSwitchGrp,
							u8 byMode,
							u8 bySwitchMode = SWITCH_MODE_BROADCAST,
							BOOL32 bMsgStatus = TRUE,
							BOOL32 bSndFlowCtrl = TRUE,
							u16 wSpyStartPort = SPY_CHANNL_NULL,
							BOOL32 bInnerSelect = TRUE);
	//zyl 20121116 ����ѡ����mcsdragmod
	void AddRemoveMcsDragMod(const TMt& tUnlocalSrc,
							const u8 &byDstMtId,
							const u8& byMediaMode, 
							u8 bySwitchMode = SWITCH_MODE_BROADCAST,
							u16 wSpyStartPort = SPY_CHANNL_NULL,
							BOOL32 bMsgStatus = TRUE,
							BOOL32 bInnerSelect = TRUE);

	
// 	void   StopSwitchToSubMt( u8 byDstMtId, 
// 							  u8 byMode = MODE_BOTH,
// 							  u8 bySwitchMode = SWITCH_MODE_BROADCAST,
// 							  BOOL32 bMsgStatus = TRUE, 
// 							  BOOL32 bStopSelByMcs = TRUE,
// 							  BOOL32 bSelSelf = TRUE,
// 							  u16 wSpyPort = SPY_CHANNL_NULL );	

	void   StopSwitchToSubMt(u8 byMtNum, 
							TMt *ptDstMt,
							u8 byMode, 
		                    BOOL32 bMsgStatus = TRUE,
							BOOL32 bStopSelByMcs = TRUE, 
							BOOL32 bSelSelf = TRUE, 
							u16    wSpyPort = SPY_CHANNL_NULL);

	BOOL32 StartSwitchToMcu(TMt tSrc, u8 bySrcChnnl, u8 byDstMtId, u8 byMode = MODE_BOTH,u8 bySwitchMode = SWITCH_MODE_BROADCAST,BOOL32 bMsgStatus = TRUE );
	BOOL32 RestoreAllSubMtJoinedConfWatchingSrcMt(TMt tSrc, u8 byMode = MODE_BOTH,BOOL32 bIsStopSel = TRUE );	
    BOOL32 StopSpyMtCascaseSwitch( u8 byMode = MODE_BOTH );

	//����������Ҫ���䵽���ն�
    BOOL32 StartSwitchToSubMtNeedAdp(const TEqp &tEqp, u8 byChnId, BOOL32 bIsConsiderBiteRate = FALSE);
	//�������Ҫ������ն˽���
    BOOL32 StopSwitchToSubMtNeedAdp(const TEqp &tEqp, u8 byChnId,BOOL32 bSelSelf = TRUE);
    //�����ն˽���������bas�����Ľ���
    BOOL32 StartSwitchToSubMtFromAdp(u8 byMtId, u8 byMediaMode);
	//������Ӧģʽ�Ĺ㲥���佻��
	BOOL32 StartAllBasSwitch(u8 byMediaMode);
	//�����Ӧģʽ�Ĺ㲥���佻��
	BOOL32 StopAllBasSwitch(u8 byMediaMode);


    // ����ͬ��
	BOOL32 StartSwitchAud2Perieqp(TMt tSrc);
    BOOL32 StartSwitchAud2MtNeedAdp(TMt tSrc, TMt tDst, u8 bySwitchMode = SWITCH_MODE_BROADCAST);
	BOOL32 GetAudBufOutPutInfo(TEqp &tBas, u8 &byChnId, u8 &byOutNum, u8 &byOutIdx);

	//zjj20090925 ��������ͬ��״̬��bas�������ն˵���Ƶ����
	BOOL32 StartSwitchAud3MtNeedAdp( u8 bySwitchMode = SWITCH_MODE_BROADCAST );


    // guzh [3/20/2007]�ն˹㲥�������
    BOOL32 StartSwitchFromBrd(const TMt &tSrc, u8 bySrcChnnl, u8 byDstMtNum, const TMt* const ptDstBase, u16 wSpyStartPort = SPY_CHANNL_NULL);
    BOOL32 StartSwitchToAllSubMtJoinedConf( const TMt &tSrc, u8 bySrcChnnl, u16 wSpyStartPort = SPY_CHANNL_NULL );     // ��Ƶ
    BOOL32 StartSwitchToAllSubMtJoinedConf( const TMt &tSrc, u8 bySrcChnnl, const TMt &tOldSrc, BOOL32 bForce, u16 wSpyStartPort = SPY_CHANNL_NULL ); // ��Ƶ   
	BOOL32 StartSwitchToAllNeedAdpWhenSrcChanged(TMt tSrc, u8 byDstNum, TMt *ptDstMt, u8 byMode);

	void RestoreRcvMediaBrdSrc( u8 byMtId, u8 byMode = MODE_BOTH, BOOL32 bMsgMtStutas = TRUE, u16 wSpyStartPort = SPY_CHANNL_NULL );
	void RestoreRcvMediaBrdSrc( u8 byMtNum, TMt *ptDstMt, u8 byMode = MODE_BOTH, 
		                      BOOL32 bMsgMtStutas = TRUE, u16 wSpyStartPort = SPY_CHANNL_NULL );
	void RestoreRcvSecMediaBrdSrc(u8 byMtNum, TMt *ptDstMt, u8 byMode = MODE_VIDEO2SECOND, BOOL32 bMsgMtStutas = TRUE, u16 wSpyStartPort = SPY_CHANNL_NULL);
	
	void SwitchSrcToDst(
		const TSwitchInfo & tSwitchInfo,
		const CServMsg & cServMsgHdr,
		BOOL32 bBatchPollChg = FALSE,
		const THduSwitchInfo *ptHduSwitchInfo = NULL,
		const u8 byMulitModeHdu = HDUCHN_MODE_ONE);
 
	void StopSwitchToAllSubMtJoinedConf( u8 byMode = MODE_BOTH );
    void ProcVidBrdSrcChanged( CMessage * const pcMsg );
    BOOL32 CanMtRecvVideo(const TMt &tDst, const TMt &tSrc);

	//2010/07/13 lukunpeng ����¼������״̬��ȡ�ն���Ӧ��������Ϣ
	BOOL32 GetMtRecordInfo( const TMt &tMt, TEqp& tRecEqp, u8& byRecChannel );
	BOOL32 RemoveMtFromRec(const TMt* ptMt = NULL, const BOOL32 bStopAll = FALSE );
public: //��MpManager����
	BOOL32 GetMtStatus(const TMt& tMt, TMtStatus& tMtStatus);

	void SetMtStatus(const TMt& tMt,  const TMtStatus& tMtStatus);
protected:
	//2�����轻��
	BOOL32 StartSwitchToPeriEqp(TMt tSrc,
                                u16 wSrcChnnl,
                                u8  byEqpId,
                                u8  byDstChnnl           = 0,
                                u8  byMode              = MODE_BOTH,
                                u8  bySwitchMode        = SWITCH_MODE_BROADCAST,
                                BOOL32 bDstHDBas        = FALSE,
                                BOOL32 bStopBeforeStart = TRUE,
                                BOOL32 bBatchPollChg    = FALSE,
								BOOL32 bAddRtcp			= TRUE,
								BOOL32 bSendMsgToEqp    = TRUE,
								const u8 byHduSubChnId  = 0,
								const u8 byMulitModeHdu = HDUCHN_MODE_ONE);

	void StopSwitchToPeriEqp( u8 byEqpId, u8 byDstChnnl = 0, BOOL32 bMsg = TRUE,
			u8 byMode = MODE_BOTH,u8 bySwitchMode = SWITCH_MODE_BROADCAST, u8 byHduSubChnId = 0); 
	void StopSwitchToAllPeriEqpWatchingSrcMt( TMt tSrc, BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH );
	//void StopSwitchToAllPeriEqpWatchingSrcMcu(  u16 wMcuId, BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH );
	void StopSwitchToAllPeriEqpInConf( BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH );
	void GetHduMemberRtcpAddrInfo(const TMt tSrc, 
									const u8 bySrcChl,
								  const u8 byMode, 
								  u32 &dwVidIpAddr, 
								  u16 &wVidPort, 
								  u32 &dwAudIpAddr, 
								  u16 &wAudPort);

	//3����ؽ���
	BOOL32 StartSwitchToMc( TMt tSrc, u8 bySrcChnnl, u16 wMcInstId, u8 byDstChnnl = 0, u8 byMode = MODE_BOTH, u16 wSpyPort = SPY_CHANNL_NULL );
	void StopSwitchToMc( u16 wMcInstId,  u8 byDstChnnl = 0, BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH, BOOL32 bFilterConn = TRUE );
	void StopSwitchToAllMcInConf( BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH );
	void AdjustSwitchToAllMcWatchingSrcMt( TMt tSrc, BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH,BOOL32 bIsStop=TRUE,u8 byChl = 0xFF );
	void StopSwitchToAllMcWatchingSrcMcu(  u16 wMcuId, BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH );
    BOOL32 IsMtInMcSrc(u8 byMtId, u8 byMode);
	u8 GetCanRlsNumInVcsMc(TMt &tMt, u8 byMode/*, u8 byChnl = VCS_VCMT_MCCHLIDX*/);

	BOOL32	StartSwitchToMonitor(TSwitchDstInfo &tSwitchDstInfo, u16 wMcInstId);
	void	NotifyUIStartMonitor(TSwitchDstInfo &tSwitchDstInfo, u16 wMcInstId);
	BOOL32  StopSwitchToMonitor(TSwitchDstInfo &tSwitchDstInfo, u16 wMcInstId, BOOL32 bIsOnlySwitch = FALSE);

	void    StopSwitchToAllMonitor(BOOL32 bConf = FALSE);
	BOOL32  StopSwitchToMonitor(u16 wMcInstId, BOOL32 bConf = FALSE);
	void	AdjustSwitchToMonitorWatchingSrc(const TMt &tSrc,u8 byMode = MODE_NONE,BOOL32 bIsStop = TRUE,BOOL32 bIsOnlySwitch = FALSE);
    
	//4���Ž���
    BOOL32 AdjustSwitchBridge(u8 byMpId, BOOL32 bInsert);
	void   Proc8khmMultiSwitch(); // 8000H-M���⴦����ʱ�л��鲥��ַ [pengguofeng 6/6/2013]

	//5����ش�
	BOOL32 AddSpyChnnlInfo(const TMt &tMt, s16 &swIndex,BOOL32 bIsPreAdd = FALSE);

	void NotifyMcsCriMacAddrInfo( void ); //֪ͨMCS���н��빦�ܵ���Mac��ַ��Ϣ
	void DaemonProcMcsMcuGetCriMacAddrInfoReq(  const CMessage * pcMsg ); //����MCS��ȡ���н��빦�ܵ���Mac��ַ��Ϣ������

    /************************************************************************/
    /*                                                                      */
    /*                          �ߡ�ʵ��һ�㺯��                            */
    /*                                                                      */
    /************************************************************************/
private:
    void ClearVcInst( void );
    u8   AssignIdleConfInsID(u8 byStartInsId = 1);
    
	void InstanceDump( u32 param = 0 );
	void InstanceEntry( CMessage * const pcMsg );
	void DaemonInstanceEntry( CMessage * const pMsg, CApp * pcApp );
	void DaemonInstanceDump( u32 param = 0 );

    /************************************************************************/
    /*                                                                      */
    /*                          �ˡ���ӡ���                                */
    /*                                                                      */
    /************************************************************************/
protected:
	void ShowConfMt( u8 byMtId );
	void ShowConfSwitch( void );
	void ShowConfEqp( void );
	void ShowMtStat( void );
	void ShowConfMod( void );
	void ShowMtMonitor( void );
	void ShowBasInfo( u8 byBasId = 0 );
	void ConfPrint( const u8 byLevel, const u16 wModule, const s8* pszFormat, ...);
	void ShowVmpAdaptMember( void );

	void ShowConfMix( void );

	void ShowConfMultiSpy( void );
// 	void ShowMultiSpyMMcuInfo( void );
// 	void ShowMultiSpySMcuInfo( void );

	void ShowConfMcu( void );
	void ShowConfMc(void);//zhouyiliang 20101216��ӡ��������mc
	void ShowMcOtherTable(void);

};

typedef zTemplate< CMcuVcInst, MAXNUM_MCU_CONF, CMcuVcData, sizeof( CConfId ) > CMcuVcApp;	//Daemonʵ�����ڴ���ǻ�������

extern  CMcuVcApp	        g_cMcuVcApp;	//MCUҵ��Ӧ��ʵ��
extern  CAgentInterface     g_cMcuAgent;    
extern  CMSManagerSsnApp    g_cMSSsnApp;
extern  CMpManager          g_cMpManager;   //MP������ȫ�ֱ���

#define STATECHECK \
if( CurState() != STATE_ONGOING )\
{\
	ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u at Line.%u!\n", \
		       pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), __LINE__ );\
	return;\
}

// ����pclint��ISGROUPMODE�Ĵ����Ϊ666�ļ��--byMode����������һ����ʽʱ,�ᱨ��ĸ����õ�warning
/*lint -save -esym(666, ISGROUPMODE)*/
#define ISGROUPMODE(byMode) \
((byMode >= VCS_GROUPSPEAK_MODE && byMode <= VCS_GROUPROLLCALL_MODE) ? TRUE : FALSE)

// [8/8/2011 liuxu] ��������ʱ�Ի���newָ����м��, �ǿ���nack
#ifndef START_CONF_CHECK_MEMORY
#define START_CONF_CHECK_MEMORY( p, m, c )	\
	do{										\
	if(!p){								\
	m.SetErrorCode( ERR_MCU_CONFOVERFLOW );				\
	SendReplyBack( m, m.GetEventId() + 2 );		\
	printf("Conference %s failure because no memory for %s\n", c.GetConfName(), #p);			\
	ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s failure because No memory for %s!\n", c.GetConfName(), #p );		\
	return;			\
	}\
	}while(0)
#endif

#endif

//END OF FILE

