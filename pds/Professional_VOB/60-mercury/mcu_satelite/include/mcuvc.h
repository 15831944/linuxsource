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
#include "evnms.h"

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
const u32 TIMESPACE_WAIT_CASCADE_CHECK  = 10 * 1000;    //�ȴ�����ͨ����(ms)
const u32 TIMESPACE_UPADATE_REGSTATUS   = 2 * 60 * 1000;//ˢ�¸�������ʵ��ʱ����
const u32 TIMESPACE_WAIT_FSTVIDCHL_SUC	= 1 * 1000;		//�ȴ���һ·�߼�ͨ������ɣ��ٳ��Եڶ�·
const u32 TIMESPACE_APPLYFREQUENCE_CHECK= 10 * 1000;    //�ȴ����ǻ�������Ƶ�ʵĳ�ʱʱ��
const u32 TIMESPACE_SAT_MULTICAST_CHECK = 1500;			//�ȴ����ǻ���ˢ�³�ʱʱ����(ms)


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


#define MCU_SAFE_DELETE(ptr)	{if(ptr){delete ptr;ptr=NULL;}}


// ���������ȫ������, len: 11K
class CMcuSingleConfAllInfo
{
public:

    TConfMtTable      *m_ptMtTable;              //�����ն˱�      
    TConfSwitchTable  *m_ptSwitchTable;          //���齻����
    TConfOtherMcTable *m_ptConfOtherMcTable;     //����Mc�ն���Ϣ

    
    TConfInfo	     m_tConf;	                 //����������Ϣ
	TConfAllMtInfo   m_tConfAllMtInfo;           //���������ն���Ϣ    
    TConfInnerStatus m_tConfInStatus;            //�����ڲ�״̬��ʶ
    TConfProtectInfo m_tConfProtectInfo;         //���鱣����Ϣ
    TConfPollParam   m_tConfPollParam;           //������ѯ����	
    TTvWallPollParam m_tTvWallPollParam;         //����ǽ��ѯ����
	THduPollParam    m_tHduPollParam;            //hdu��ѯ����
    TConfEqpModule   m_tConfEqpModule;           //�����豸ģ��

    u8               m_byConfIdx;                //����idx
    TMt              m_tCascadeMMCU; 		     //�������ϼ�MCU
    u8               m_byCreateBy;               //��¼����Ĵ�����
protected:    

    TMt     m_tVidBrdSrc;                   //��Ƶ�㲥Դ
    TMt     m_tAudBrdSrc;                   //��Ƶ�㲥Դ
    TMt     m_tDoubleStreamSrc;             //˫��Դ 
	u8		m_byLastDsSrcMtId;				//�ϴ�˫��ԴMT Id
	TLogicalChannel m_tLogicChnnl;			// ��¼lc (Ϊ��˫������)
    TH239TokenInfo m_tH239TokenOwnerInfo;   //H239˫������ӵ���ն���Ϣ
    
    TEqp    m_tVmpEqp;                      //����ϳ�����
    TEqp    m_tVmpTwEqp;                    //���ϵ���ǽ����
    TEqp	m_tMixEqp;                      //��������
    TEqp	m_tRecEqp;                      //¼������
    TEqp	m_tPlayEqp;                     //��������
    TPlayEqpAttrib m_tPlayEqpAttrib;        //������������
    TPlayFileMediaInfo m_tPlayFileMediaInfo;    //�����ļ�ý����Ϣ
    TRecStartPara m_tRecPara;               //¼�����
    
    TEqp	m_tAudBasEqp;					//��Ƶ��������
    TEqp	m_tVidBasEqp;					//��Ƶ��������
    TEqp	m_tBrBasEqp;					//������������
    TEqp    m_tCasdAudBasEqp;               //������Ƶ�ش���������
    TEqp    m_tCasdVidBasEqp;               //������Ƶ�ش���������
    TEqp    m_tPrsEqp;                      //���ش�����
    
    u8      m_byMixGrpId;					//������ID
    u8		m_byRecChnnl;					//¼���ŵ���
    u8		m_byPlayChnnl;					//�����ŵ���
    u8      m_byPrsChnnl;                   //���ش���һ��Ƶͨ��
    u8      m_byPrsChnnl2;                  //���ش��ڶ���Ƶͨ��
    u8      m_byPrsChnnlAud;                //���ش���Ƶͨ��
    u8      m_byPrsChnnlAudBas;             //��Ƶ������ش�ͨ��
    u8      m_byPrsChnnlVidBas;             //��Ƶ������ش�ͨ��
    u8      m_byPrsChnnlBrBas;              //����������ش�ͨ��

	u8		m_byPrsChnnlVmpOut1;			//�㲥Դ1080�����ش�ͨ��
	u8		m_byPrsChnnlVmpOut2;			//�㲥Դ720�����ش�ͨ��
	u8		m_byPrsChnnlVmpOut3;			//�㲥Դ4Cif�����ش�ͨ��
	u8		m_byPrsChnnlVmpOut4;			//�㲥ԴCif��other�����ش�ͨ��
	u8		m_byPrsChnnlDsVidBas;			//˫��������ش�ͨ��	
	
    u8      m_byAudBasChnnl;				//��Ƶ����ͨ����
    u8      m_byVidBasChnnl;				//��Ƶ����ͨ����
    u8      m_byBrBasChnnl;					//��������ͨ����
    u8      m_byCasdAudBasChnnl;            //������Ƶ�ش�����ͨ����
    u8      m_byCasdVidBasChnnl;            //������Ƶ�ش�����ͨ����
    u16     m_wVidBasBitrate;				//��Ƶ���������    
    u16     m_wBasBitrate;					//�������������
    u16     m_awVMPBrdBitrate[MAXNUM_MPU_OUTCHNNL];               //���ٻ���VMP�Ĺ㲥����
#ifdef _MINIMCU_
    u8      m_byIsDoubleMediaConf;          //�Ƿ���˫ý�����(�����ʲ�������ʽ�������ʲ��ø���ʽ
#endif

	u32		m_dwVmpLastVCUTick;				//��һ����VMP��������ؼ�֡����
	u32		m_dwVmpTwLastVCUTick;			//��һ����VMPTw��������ؼ�֡����
	u32		m_dwVidBasChnnlLastVCUTick;		//��һ����BAS����ͬͨ������������ؼ�֡����
	u32		m_dwAudBasChnnlLastVCUTick;
	u32		m_dwBrBasChnnlLastVCUTick;
	u32		m_dwCasdAudBasChnnlLastVCUTick;
	u32		m_dwCasdVidBasChnnlLastVCUTick;
    
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
    TVMPParam	m_tLastVmpParam;			//��һ�λ���ϳɳ�Ա������Vmp��̬�ֱ��ʻָ�
    TVMPParam	m_tLastVmpTwParam;          //��һ��VMPTW��Ա������Vmp��̬�ֱ��ʻָ� 

    TMixParam m_tLastMixParam;              //����������ǰ��״̬

    TAcctSessionId m_tChargeSsnId;          //������Ʒ�SessionId

    TConfPollSwitchParam m_tPollSwitchParam;           //Radvision��ʽ��ѯ�ش�����
    u8      m_abyMixMtId[MAXNUM_MIXER_CHNNL+1];        //���ڼ�¼����������ն˵�MtId
    u8      m_abyCasChnCheckTimes[MAXNUM_CONF_MT];     //�򿪼���ͨ���ĳ��Դ���    

    u8      m_abySerHdr[SERV_MSGHEAD_LEN];  //��¼�Ĵ�������ʱ����Ϣͷ
    TMt     m_tRollCaller;                  //������
    TMt     m_tRollCallee;                  //��������

	u8	    m_bySpeakerVidSrcSpecType;		//������Դ�Ƿ����ɻ����Զ�ָ��
	u8		m_bySpeakerAudSrcSpecType;
    
    CRcvGrp m_cMtRcvGrp;                    //��������ն˽���Ⱥ�����
    CBasMgr m_cBasMgr;                      //�������BASͨ����״̬����

	u8		m_byMtIdNotInvite;				// xliang [12/26/2008] Mt ������MCU��
											//ģ�崴�����ȱʡ��������£�
											//��Ǹ�MTԤ�Ȳ��������ն�

	//****************  VMP related  ****************
	//--seize related:
	TVmpChnnlInfo   m_tVmpChnnlInfo;						//vmpͨ����Ϣ����Ҫ��¼VMPǰ�����Ա
	TVMPMember	    m_atVMPTmpMember[MAXNUM_MPUSVMP_MEMBER];//��ռ����¼��
	BOOL32	m_bNewMemberSeize;								//�����˴���ռ��������һ����Ա��������ռ����ĳ��Ա����ΪVIP��ݴ�������ռ
	u8		m_abyMtNeglectedByVmp[MAXNUM_CONF_MT/8];		//����Զ�vmpʱ����Ӧ�����MT// xliang [12/17/2008] Ŀǰ����, ��Ԥ��
	
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

	CVCCStatus m_cVCSConfStatus;                // VCS����Ķ��صĻ���״̬	
    CSelChnGrp m_cSelChnGrp;                    // ѡ������Ⱥ������


    TPrsInfoManage m_tConfPrsInfo;              // ����ʹ�õ�PRS��ϸ��Ϣ,Ŀǰ�����HDBAS

    //����ƽ̨�������ǻ���

    //������Ϣ����
    u32		  m_dwMultiFreqSeq;                 //����������������кţ��������ֻ�Ӧ������
    u32		  m_dwReceiveFreqSeq;               //����������������кţ��������ֻ�Ӧ������
	u32		  m_dwSeq;                          //���к�

    //��ʱ�������±����Խ������ǻ��飻δ����������������
    BOOL32    m_bSchedStart;
    u32       m_dwOldDuration;
    u32       m_dwOldStart;
    u16       m_wOpMtId;
    u8        m_byConfSPortNum;

	u16		  m_wTicks;							//���ն˶ಥ��Ϣ,ʱ϶
	u16		  m_wTimes;							//���ն˶ಥ��Ϣ,ʱ��
	u8        m_bySendBitMap;					//ʱ϶����Ҫ�������ݵı��

	u16       m_wMtTopoNum;						//���淢�͵�TOPO��MT�ţ���Ϊ���ֶܷ����
	u16		  m_wMtKeyNum;						//���淢�͵�Key��MT�ţ���Ϊ���ֶܷ����
};


// ҵ��ʵ��(Osp������)
class CMcuVcInst : public CInstance,
                   public CMcuSingleConfAllInfo
{
public:
	enum 
	{ 
		STATE_IDLE,       //����
		STATE_WAITEQP,    //�ȴ�����
        STATE_WAITAUTH,   //���ǻ���ȴ�����Ƶ������Ӧ��
		STATE_PERIOK,	  //������ص��ⲿ����׼��ok
		STATE_ONGOING,    //��ʱ
		STATE_SCHEDULED,  //ԤԼ
        STATE_MASTER,
        STATE_SLAVE
	};

public:	
    CMcuVcInst();
	virtual ~CMcuVcInst();

protected:

    /************************************************************************/
    /*                                                                      */
    /*                       һ��daemon������                             */
    /*                                                                      */
    /************************************************************************/

	//1. daemonʵ��������
	void DaemonProcCommonNotif( const CMessage * pcMsg );

    void DaemonProcPowerOn( const CMessage * pcMsg );
    void DaemonProcMcuMSStateNtf( const CMessage * pcMsg );    

    //2. daemon ����/ģ�����
    void DaemonProcMcsMcuCreateConfReq(const CMessage * pcMsg);
    void DaemonProcMcsMcuCreateConfByTemplateReq(const CMessage * pcMsg);
    void DaemonProcMcsMcuSaveConfToTemplateReq( const CMessage * pcMsg );
    void DaemonProcCreateConfNPlus(const CMessage * pcMsg);
    void DaemonProcMcsMcuTemplateOpr(const CMessage * pcMsg);
    void DaemonProcMtMcuCreateConfReq( const CMessage * pcMsg );
    void DaemonProcMcsMcuListAllConfReq( const CMessage * pcMsg, CApp* pcApp );
    void DaemonProcDcsMcuReleaseConfRsp( const CMessage * pcMsg );  //2006-01-11
    BOOL32 NtfMcsMcuCurListAllConf( CServMsg &cServMsg );

    //3. daemon GK ����
    void DaemonProcGkRegRsp(const CMessage * pcMsg);
    void DaemonProcGKChargeRsp(const CMessage * pcMsg);
    void DaemonProcMcuReRegisterGKNtf( const CMessage * pcMsg );
	
    //4. daemon �������
	void ProcBasChnnlStatusNotif( const CMessage * pcMsg );
    void DaemonProcHDBasChnnlStatusNotif( const CMessage * pcMsg );
	void DaemonProcMcsMcuGetPeriEqpStatusReq( const CMessage * pcMsg );
	void DaemonProcRecMcuProgNotif( const CMessage * pcMsg );
	void DaemonProcRecMcuChnnlStatusNotif( const CMessage * pcMsg );
    void DaemonProcHduMcuChnnlStatusNotif( const CMessage * pcMsg );

    void DaemonProcMpFluxNotify( const CMessage * pcMsg );
    void DaemonProcPeriEqpMcuStatusNotif( const CMessage * pcMsg );
	void DaemonProcMcsMcuStopSwitchMcReq( const CMessage * pcMsg );
	void DaemonProcMcsMcuStopSwitchTWReq( const CMessage * pcMsg );
	
	void DaemonProcMcsMcuGetMcuStatusReq( const CMessage * pcMsg, CApp* pcApp );
	void DaemonProcMcsMcuSetMcuCfgCmd( const CMessage * pcMsg );	

	void DaemonProcMcuMtConnectedNotif( const CMessage * pcMsg );
	void DaemonProcMcuEqpConnectedNotif( const CMessage * pcMsg );
	void DaemonProcMcuEqpDisconnectedNotif( const CMessage * pcMsg );
	void DaemonProcMcuDcsConnectedNtf( const CMessage * pcMsg );	//2005-12-15
	void DaemonProcMcuDcsDisconnectedNtf( const CMessage * pcMsg );	//2005-12-15	
	void DaemonProcMcuMcsConnectedNotif( const CMessage * pcMsg );
	void DaemonProcMcuMcsDisconnectedNotif( const CMessage * pcMsg );
	void DaemonProcMtMcuApplyJoinReq( const CMessage * pcMsg );
    
    void DaemonProcNPlusRegBackupRsp( const CMessage * pcMsg );    
    void DaemonProcAppTaskRequest( const CMessage * pcMsg );
	void DaemonProcHDIFullNtf ( const CMessage *pcMsg );
    
    //5��Test
	void DaemonProcTestMcuGetAllMasterConfIdCmd( const CMessage* pcMsg );
	void DaemonProcTestMcuGetConfFullInfoCmd( const CMessage* pcMsg );	
    //�����ȱ��ݵ�Ԫ�����ú���
    void DaemonProcUnitTestMsg( CMessage *pcMsg );
    void ProcUnitTestGetTmpInfoReq( CMessage *pcMsg );
    void ProcUnitTestGetConfInfoReq( CMessage *pcMsg );
    void ProcUnitTestGetMtListInfoReq( CMessage *pcMsg );
    void ProcUnitTestGetAddrbookReq( CMessage *pcMsg );
    void ProcUnitTestGetMcuCfgReq( CMessage *pcMsg );
	
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
	void ProcTimerVacChange( const CMessage * pcMsg );
    void ProcTimerAutoRec( const CMessage * pcMsg );
    void ProcWaitEqpToCreateConf(CMessage * const pcMsg);

    void CreateConfEqpInsufficientNack( u8 byCreateMtId, u8 byOldInsId, const s32 nErrCode, CServMsg &cServMsg );
    void CreateConfSendMcsNack( u8 byOldInsId, const s32 nErrCode, CServMsg &cServMsg, BOOL32 bDeleteAlias = FALSE );
    BOOL32 IsConfInfoCheckPass( const CServMsg &cSrcMsg, TConfInfo &tConfInfo, u16 &wErrCode, BOOL32 bTemplate = FALSE);
    BOOL32 IsEqpInfoCheckPass( const CServMsg &cSrcMsg, u16 &wErrCode);
    void ConfDataCoordinate(CServMsg &cMsg, u8 &byMtNum, u8 &byCreateMtId);
    void AddSecDSCapByDebug(TConfInfo &tConfInfo);

	BOOL32 PrepareAllNeedBasForConf(u16* pwErrCode = NULL);
	void   RlsAllBasForConf();

    void ReleaseConf( BOOL32 bRemoveFromFile = TRUE);
    u32  PackMtInfoListIntoBuf(const TMt atMt[], const TMtAlias	atMtAlias[],
                               u8	byArraySize, u8 *pbyBuf, u32 dwBufLen );	
    void ChangeConfLockMode( CServMsg &cMsg );
    
    void SendReplyBack( CServMsg & cReplyMsg, u16 wEvent );
    BOOL32 IsMtCmdFromThisMcu( const CServMsg & cCmdReply ) const;
    BOOL32 IsMcCmdFromThisMcu( const CServMsg & cCmdReply ) const;
    BOOL32 SendMsgToMt( u8 byMtId, u16 wEvent, CServMsg & cServMsg );
    BOOL32 SendMsgToEqp( u8 byEqpId, u16 wEvent, CServMsg & cServMsg );
    BOOL32 SendMsgToMcs( u8 byMcsId, u16 wEvent, CServMsg & cServMsg );
    BOOL32 SendOneMsgToMcs( CServMsg &cServMsg, u16 wEvent );
    void SendMsgToAllMcs( u16 wEvent, CServMsg & cServMsg );
    void SendMsgToAllMp( u16 wEvent, CServMsg & cServMsg);  // libo [4/27/2005] add
    void SendMtListToMcs(u8 buMcuId);
    void SendMtAliasToMcs(TMt tMt);
    void BroadcastToAllSubMtJoinedConf( u16 wEvent, CServMsg & cServMsg );
    void BroadcastToAllMcu( u16 wEvent, CServMsg & cServMsg, u8 byExceptMc = 0 );
    BOOL32 SendMsgToDcsSsn( u8 byInst, CServMsg &cServMsg ); //2005-12-16
    
    void SendTrapMsg( u16 wEvent, u8 * const pbyMsg, u16 wLen );    
    
    void NotifyMtSend( u8 byDstMtId, u8 byMode = MODE_BOTH, BOOL32 bStart = TRUE );
	void NotifyEqpSend( u8 byDstEqpId, u8 byMode = MODE_BOTH, BOOL32 bStart = TRUE );
    void NotifyMtReceive(  const TMt & tSrc, u8 byDstMtId );	
    void StartMtSendbyNeed( TSwitchChannel* ptSwitchChannel );
    void StopMtSendbyNeed( TSwitchChannel* ptSwitchChannel );
    void NotifyRecStartPlaybyNeed( TSwitchChannel* ptSwitchChannel ); // zgc, 2007-02-27
    void NotifyOtherMtSend( u8 byDstMtId, BOOL32 bStart );  //֪ͨ���������ն��Ƿ�����Ƶ����        	
    // guzh [7/27/2007] ����VCU�����͸��նˣ�����
    void NotifyFastUpdate(const TMt &tDst, u8 byChnlMode, BOOL32 bSetTimer = FALSE);
    void NotifyMtFastUpdate(u8 byMtId, u8 byChnlMode, BOOL32 bSetTimer = FALSE);
    void NotifyEqpFastUpdate(const TMt &tDst, u8 byChnlMode, BOOL32 bSetTimer = FALSE);
    

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
	void ProcTimerRecreateDataConf( const CMessage * pcMsg );
    
    //3��gk �Ʒ�
    void ProcConfRegGkAck( const CMessage * pcMsg );
	void ProcConfRegGkNack( const CMessage * pcMsg );
    void ProcGKChargeRsp( const CMessage * pcMsg );
	
	//4����ͨ������� 
    void ChangeVidBrdSrc( TMt * ptNewVidBrdSrc );
    void ChangeAudBrdSrc( TMt * ptNewAudBrdSrc );
    void ChangeSpeaker(  TMt * ptNewSpeaker, BOOL32 bPolling = FALSE, BOOL32 bAddToVmp = TRUE );
    void ChangeChairman(  TMt * ptNewChairman );
    void AdjustChairmanSwitch( TMt * ptOldChair = NULL );
	void AdjustChairmanVidSwitch( TMt *ptOldChair = NULL );
	void AdjustChairmanAudSwitch( void );
    
    TMt  GetLocalSpeaker( void );
    BOOL32 HasJoinedSpeaker( void );
	BOOL32 HasJoinedChairman( void );
	BOOL32 IsSelectedbyOtherMt( u8 bySrcMtId,u8 byMode = MODE_VIDEO);
	BOOL32 IsSpeakerCanBrdVid(void);
	BOOL32 IsSpeakerCanBrdAud(void);
	void   AdjustOldSpeakerSwitch( TMt tOldSpeaker, BOOL32 bIsHaveNewSpeaker );
	void   AdjustNewSpeakerSwitch( TMt tNewSpeaker, BOOL32 bAddToVmp );
	void   ChangeSpeakerSrc( u8 byMode, emChangeSpeakerSrcReason emReason);
	void   ChangeOldSpeakerSrc( u8 byMode, TMt tOldSpeaker, BOOL32 bIsHaveNewSpeaker );
    
    void ConfStatusChange( void );
    void ConfModeChange( void );
    void MtStatusChange( u8 byMtId = 0, BOOL32 bForcely = FALSE );
    void MtInfoChange( void );
    void MtOnlineChange( TMt tMt, BOOL32 bOnline, u8 byReason );
    void NotifyMcsAlarmInfo(u8 byMcsId, u16 wErrorCode);    //byMcsIdΪ0��֪ͨ����mcs
    void SendConfInfoToChairMt(void);
    void ProcTimerRefreshMcs(const CMessage * pcMsg);    

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
	void ProcMcsMcuPollMsg(const CMessage * pcMsg);
	void ProcPollingChangeTimerMsg(const CMessage *pcMsg);    
    void ProcStopConfPoll();
    void ProcMcsMcuHduPollMsg(const CMessage *pcMsg);
	void ProcMcsMcuHduBatchPollMsg(const CMessage *pcMsg);    //  ����hdu������ѯ
    void ProcMcsMcuTWPollMsg(const CMessage *pcMsg);
    void ProcTWPollingChangeTimerMsg(const CMessage *pcMsg);
	void ProcHduPollingChangeTimerMsg(const CMessage *pcMsg);
    void ProcHduBatchPollChangeTimerMsg(const CMessage *pcMsg);  // hdu������ʱ����
	void ProcMcsMcuMcuMediaSrcReq(const CMessage *pcMsg);
	TMtPollParam *GetNextMtPolled(u8 &byPollPos, TPollInfo& tPollInfo);
    TMtPollParam *GetMtTWPollParam(u8 &byPollPos);
    TMtPollParam *GetMtHduPollParam(u8 &byPollPos);

	void ProcMcsMcuLockSMcuReq(const CMessage *pcMsg);
	void ProcMcsMcuGetMcuLockStatusReq(const CMessage *pcMsg);
    void NoifyMtToBePolledNext( void );
    void PollParamChangeNotify( void );
    void ProcMcsMcuRollCallMsg( const CMessage * pcMsg );
    void RollCallStop( CServMsg &cServMsg );
    void ProcMixStart( CServMsg &cServMsg );
    void ProcMixStop( CServMsg &cServMsg, BOOL32 bRollCallStop = FALSE );
	// xliang [12/12/2008] mpu-vmp related
	void ProcMcsMcuVmpPriSeizeRsp(const CMessage *pcMsg);//vmp��ռӦ��
	void ProcMtSeizeVmpTimer(const CMessage *pcMsg);	//���Ҫ��ռ��MT������ռ����
	void ProcVmpBatchPollTimer(const CMessage *pcMsg);	//vmp����poll
	void ProcMcsMcuVmpCmd(const CMessage * pcMsg);		//��������poll PAUSE,RESUME������
	void ProcChangeVmpParamReq(CServMsg &cServMsg);		//�ı仭��ϳɲ���
	void ProcStartVmpReq(CServMsg &cServMsg);			//��ʼ����ϳ�
	void ClearOneVmpMember(u8 byChnnl, TVMPParam &tVmpParam, BOOL32 bRecover = FALSE);
    //5�����Ƿ�ɢ����
    u8   GetSatCastChnnlNum(u8 bySrcMtId);
    BOOL32 IsOverSatCastChnnlNum(u8 bySrcMtId, u8 byIncNum = 1);
    BOOL32 IsOverSatCastChnnlNum(TVMPParam &tVmpParam);
    void ChangeSatDConfMtRcvAddr(u8 byMtId, u8 byChnnlType, BOOL32 bMulti = TRUE);
    BOOL32 IsMtSendingVidToOthers(TMt tMt, BOOL32 bConfPoll/*with audio*/, BOOL32 bTwPoll, u8 bySelDstMtId);


    //6��N+1�������
    void ProcNPlusConfInfoUpdate(BOOL32 bStart);
    void ProcNPlusConfMtInfoUpdate(void);
    void ProcNPlusChairmanUpdate(void);
    void ProcNPlusSpeakerUpdate(void);
    void ProcNPlusVmpUpdate(void);
    void ProcNPlusConfDataUpdate(const CMessage * pcMsg);
    void GetNPlusDataFromConf(TNPlusConfData &tNPlusData, BOOL32 bCharge = FALSE);
    
    //7�������ȱ��ݣ�M-S Data-Exchange
public:
    BOOL32 GetInstState( u8 &byState );
    BOOL32 SetInstState( u8 byState );
    
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
 
	//8��VCS���鴦��
	// VCS�����Ϣ�������
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
	void ProcVcsMcuMuteReq( const CMessage * pcMsg );
	// �����ն˳�ʱ����
	void ProcVCMTOverTime(const CMessage * pcMsg);
	// ����ն�������
	void ProcVcsMcuGroupCallMtReq(const CMessage * pcMsg);
	void ProcVcsMcuGroupDropMtReq(const CMessage * pcMsg);
	// ��ϯ��ѯ
    void ProcVcsMcuStartChairPollReq(const CMessage *pcMsg);
    void ProcVcsMcuStopChairPollReq(const CMessage *pcMsg);
    void ProcChairPollTimer(const CMessage *pcMsg);
	// ��ʱ�����ն�
	void ProcVcsMcuAddMtReq(const CMessage *pcMsg);
	void ProcVcsMcuDelMtReq(const CMessage *pcMsg);
	// ��ռ����, �Ƿ�ͬ���ͷ��ն�
	void ProcVcsMcuRlsMtMsg(const CMessage * pcMsg);


	// VCS��������������Դ�ļ��
	BOOL32 IsVCSEqpInfoCheckPass(u16& wErrCode);
	// VCS������Ϣ������
    void VCSConfDataCoordinate(CServMsg& cServMsg, u8 &byMtNum, u8 &byCreateMtId);
	// VCS������ʵ�����ģʽ��ѯ
	u8   VCSGetCallMode(TMt tMt);

	// VCS���йҶ��ն�
	void VCSCallMT(CServMsg& cServMsgHdr, TMt& tMt, u8 byCallType);
	void VCSDropMT(TMt tMt);

	// VCSѡ������
	BOOL VCSConfSelMT(TSwitchInfo tSwitchInfo);
	void VCSConfStopMTSel(TMt tDstMt, u8 byMode);    // ȡ���ն�ѡ��
	void VCSConfStopMTSeled(TMt tSrcMt, u8 byMode);  // ȡ���ն˱�ѡ��
    BOOL IsNeedSelApt(u8 bySrcMtId, u8 byDstMtId, u8 bySwitchMode);

	BOOL VCSConfMTToTW(TTWSwitchInfo tTWSwitchInfo);
	void VCSConfStopMTToTW(TMt tMt);
    // ���ն˾�����
	void VCSMTMute(TMt tMt, u8 byMuteOpenFlag, u8 byMuteType);


	// ���ݻ���״̬����������δ��ɵ�ѡ������
	void GoOnSelStep(TMt tMt, u8 byMediaType, u8 bySelDirect);
	// ���ݻ���״̬���л���ǰ�����ն�
	void ChgCurVCMT(TMt tNewCurVCMT);

	void VCSSwitchSrcToDst(const TSwitchInfo &tSwitchInfo);
	u16  FindUsableTWChan(u16 wCurUseChanIndex, TMt* ptDropOutMT = NULL, TEqp* tEqp = NULL, u8* byChanIdx = NULL, TMt* ptInMT = NULL);
	BOOL FindUsableVMP();
	u8   GetVCSAutoVmpMember(TMt* ptVmpMember);
	void SetVCSVmpMemInChanl(TMt* pVmpMember, u8 byVmpSytle);
    void ChangeSpecMixMember(TMt* ptMixMember, u8 byMemberNum);
	void RestoreVCConf(u8 byNewMode);
	void ChangeVFormat(TMt tMt, u8 byNewFormat);
	void VCSMTAbilityNotif(TMt tMt, u8 byMediaType);
	void VCSChairOffLineProc();
    void VCSConfStatusNotif();

	TMt  VCSGetNextPollMt();
	BOOL32 SwitchToAutoMixing();

	
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
	void ProcPolyMCUH239Rsp( const CMessage * pcMsg );
	void ProcMtMcuStopSwitchMtReq( const CMessage * pcMsg );
	void ProcMtMcuStartSwitchMtReq( const CMessage * pcMsg );
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
	void ProcMcuMtFastUpdatePic( const CMessage * pcMsg );
	void ProcMtMcuMsdRsp( const CMessage * pcMsg );
	void ProcMtMcuMatrixMsg( const CMessage * pcMsg );
	void ProcMtMcuOtherMsg( const CMessage * pcMsg );
	void ProcMtMcuFastUpdatePic( const CMessage * pcMsg );
	void ProcMtMcuMixReq( const CMessage * pcMsg );
	void ProcMtMcuReleaseMtReq( const CMessage * pcMsg );
	void ProcTimerInviteUnjoinedMt( const CMessage * pcMsg );
	u8   AddMt( TMtAlias &tMtAlias, u16 wMtDialRate = 0, u8 byCallMode = CONF_CALLMODE_TIMER, BOOL bPassiveAdd = FALSE );
	void AddJoinedMt( TMt & tMt );
	void RemoveMt( TMt & tMt, BOOL32 bMsg );
	void RemoveJoinedMt(TMt & tMt, BOOL32 bMsg, u8 byReason = MTLEFT_REASON_NORMAL,BOOL32 bNeglectMtType = FALSE);
	BOOL32 DealMtMcuEnterPwdRsp(TMt tMt, s8 *pszPwd, s32 nPwdLen);
    void ProcMtMcuBandwidthNotif(const CMessage *pcMsg);
	void RestoreVidSrcBitrate(TMt tMt, u8 byMediaType);
	void ProcMcsMcuSetMtVolumeCmd(const CMessage * pcMsg);
    void MtVideoSourceSwitched(CServMsg & cServMsg);
    void ProcMtMcuVideoSourceSwitched(const CMessage * pcMsg);
    void ChangeMemberVolume( const TMt & tMember, u8 byVolume );
    void SetMemberVolumeReq( const TMt & tMember, u8 byVolume );

    BOOL32 AssignMpForMt( TMt & tMt );
    void ReleaseMtRes( u8 byMtId, BOOL32 bForced = FALSE );	
    void InviteUnjoinedMt( CServMsg& cServMsgHdr, const TMt* ptMt = NULL, 
		                   BOOL32 bSendAlert = FALSE, BOOL32 bBrdSend = FALSE,
						   u8 byCallType = VCS_FORCECALL_REQ);

    void ProcMtCapSet( TMt tMt, TCapSupport &tCapSupport, const TCapSupportEx &tCapSupportEx );	

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
    void ProcMcuMcuAutoSwitchTimer( const CMessage *pcMsg );
	
	void CascadeAdjMtRes( TMt tMt, u8 bNewFormat, BOOL32 bStart = TRUE, u8 byVmpStyle = ~0, u8 byPos = ~0);
	void ProcMcuMcuAdjustMtResReq ( const CMessage *pcMsg );	
	void ProcMcuMcuAdjustMtResAck ( const CMessage *pcMsg );	

	void OnStartMixToSMcu(CServMsg *pcSerMsg);
	void OnStopMixToSMcu(CServMsg *pcSerMsg);
	void OnGetMixParamToSMcu(CServMsg *pcSerMsg);
	void OnAddRemoveMixToSMcu(CServMsg *pcSerMsg, BOOL32 bAdd);

	void OnStartDiscussToAllSMcu(CServMsg *pcSerMsg);
	void OnStopDiscussToAllSMcu(CServMsg *pcSerMsg);

	void OnGetMtStatusCmdToSMcu( u8 byDstMcuId );
	void OnNtfMtStatusToMMcu( u8 byDstMcuId, u8 byMtId = 0 );

	void NotifyMcuNewMt(TMt tMt);
	void NotifyMcuDelMt(TMt tMt);
	void NotifyMcuDropMt(TMt tMt);
	
	void ProcMcuMcuSendMsgReq( const CMessage * pcMsg );

	TConfMcInfo *GetConfMcInfo( u8 byMcuId );
	BOOL32 GetMcData( u8 byMcuId, TConfMtInfo &tConfMtInfo, TConfMcInfo &tConfOtherMcInfo );

	TMt  GetLocalMtFromOtherMcuMt( TMt tMt );
	
	TMt  GetMcuMediaSrc(u8 byMcuId);
	
	void OnMMcuSetIn(TMt &tMt, u8 byMcsSsnId, u8 bySwitchMode, BOOL32 bPolling = FALSE);
    void OnSetOutView(TMt &tSetInMt);

    // guzh [5/14/2007] ��ȡ��������Ƶ��Ϣ
    void GetConfViewInfo(u8 byMcuId, TCConfViewInfo &tInfo);
    void GetLocalAudioInfo(TCConfAudioInfo &tInfo);    
	   
    // guzh [5/14/2007] ֪ͨ��������MCU����ý��Դ
    void NotifyAllSMcuMediaInfo(u8 byTargetMcuId, u8 byMode);
    void NofityMMcuMediaInfo();

    //���Զ�δ򿪼���ͨ��
    void ProcTimerReopenCascadeChannel();

    //mcu source
    void SendMcuMediaSrcNotify(BOOL32 bFource = FALSE);
    void ProcTimerMcuSrcCheck(const CMessage * pcMsg);
    
    void ProcMcuMtFastUpdateTimer( const CMessage * pcMsg );
    void ProcMcuEqpFastUpdateTimer( const CMessage * pcMsg );
    void ProcMcuMtSecVideoFastUpdateTimer( const CMessage * pcMsg );
    
    void ProcTimerChangeVmpParam(const CMessage * pcMsg);

    //3��˫�����
    void StopDoubleStream( BOOL32 bMsgSrcMt, BOOL32 bForceClose );
    void StartDoubleStream(TMt &tMt, TLogicalChannel &tLogicChnnl);
    void StartSwitchDStreamToFirstLChannel( TMt &tDstMt );
    void StopSwitchDStreamToFirstLChannel( TMt &tDstMt );
    BOOL32 JudgeIfSwitchFirstLChannel( TMt &tSrc, u8 bySrcChnnl, TMt &tDstMt );
    void ClearH239TokenOwnerInfo(TMt *ptMt);
    void UpdateH239TokenOwnerInfo(TMt &tMt);
    void NotifyH239TokenOwnerInfo(TMt *ptMt);
    void McuMtOpenDoubleStreamChnnl(TMt &tMt, const TLogicalChannel &tLogicChnnl, const TCapSupport &tCapSupport);//��˫��ͨ��
    void ProcSendFlowctrlToDSMtTimeout(const CMessage * pcMsg );
    void ProcSmcuOpenDVideoChnnlTimer(const CMessage * pcMsg);
    u16    GetDoubleStreamVideoBitrate(u16 wDialBitrate, BOOL32 bFst = TRUE); //��ȡ˫����Ƶ���ʣ�Ĭ�ϵ�һ·
	BOOL32 IsDStreamMtSendingVideo(TMt tMt);
    
    //4��ѡ�����
    void RestoreMtSelectStatus( u8 byMtId, u8 bySelMode );       
    BOOL ChangeSelectSrc(TMt tSrcMt, TMt tDstMt, u8 byMode = MODE_BOTH);
    void StopSelectSrc(TMt tDstMt, u8 byMode = MODE_BOTH, u8 byIsRestore = TRUE );
    //ѡ��ģʽ��ѡ��Դ��Ŀ�ĵ��������Ƿ�ƥ��
    BOOL32 IsSelModeAndCapMatched( u8               &bySwitchMode,
                                   const TSimCapSet &tSimCapSrc,
                                   const TSimCapSet &tSimCapDst,
                                   BOOL32           &bAccord2Adp);
    void ProcMtSelectdByOtherMt( u8 byMtId, BOOL32 bStartSwitch  );    

    //5������
    void AdjustMtVideoSrcBR( u8 byMtId, u16 wBitrate, u8 byMode = MODE_VIDEO );
    BOOL32 IsNeedAdjustMtSndBitrate(u8 byMtId, u16 &wSndBitrate);
    BOOL32 IsMtMatchedSrc( u8 byMtId, u8 &byChnRes, BOOL32 &bAccord2MainCap);
	void	SwitchNewVmpToSingleMt(TMt tMt);
	BOOL32 GetMtMatchedRes(u8 byMtId, u8 byChnType, u8& byChnRes);
	BOOL32 FindAcceptCodeChnl(u8 byMtId, u8 byMode, u8 byEqpId, u8 byChnIdx, u8& byOutChnlIdx);
	BOOL32 IsNeedNewSelAdp(u8 bySrcId, u8 byDstId,u8 bySelMode,u8* pbyEqpId = NULL, u8* pbyOutChnlIdx = NULL);
public:
    TMt  GetVidBrdSrc(void);

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
    void StartPlayReq(CServMsg & cServMsg);
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
	void ProcRecPlayWaitMpAckTimer( const CMessage * pcMsg);
    
    void ProcBasMcuRsp( const CMessage * pcMsg );
    void ProcMcuBasDisconnectedNotif( const CMessage * pcMsg );
    void ProcMcuBasConnectedNotif( const CMessage * pcMsg );
    void ProcBasMcuCommand(const CMessage * pcMsg);
    void ProcBasTestCommand( const CMessage * pcMsg );

    void ProcHdBasMcuRsp( const CMessage * pcMsg );
    void ProcHdBasMcuBrdAdpRsp( CServMsg &cServMsg );
    void ProcHdBasMcuSelRsp( CServMsg &cServMsg );
    void ProcHdBasChnnlStatusNotif( const CMessage * pcMsg );
	void ProcHduMcuChnnlStatusNotif( const CMessage * pcMsg );

    void ProcVmpMcuRsp( const CMessage * pcMsg ); 
    void ProcVmpMcuNotif( const CMessage * pcMsg ); 
    void ProcVmpRspWaitTimer( const CMessage * pcMsg );
	void ProcVMPStopSucRsp(void);
    
    void ProcVmpTwMcuRsp(const CMessage * pcMsg);
    void ProcVmpTwMcuNotif(const CMessage * pcMsg);
    
    void ProcPrsConnectedNotif( const CMessage * pcMsg );
    void ProcPrsDisconnectedNotif( const CMessage * pcMsg );
    void ProcPrsMcuRsp( const CMessage * pcMsg );
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
    void ProcMcsMcuChangeHduVolume( const CMessage * pcMsg );       // ����hdu����
	void ProcHduConnectedNotif(const CMessage * pcMsg);             // hdu����֪ͨ
    void ProcHduDisconnectedNotif( const CMessage * pcMsg );        // ����֪ͨ
	void ProcHduMcuStatusNotif( const CMessage * pcMsg );           // ״̬֪ͨ
    void ProcHduMcuNeedIFrameCmd( const CMessage * pcMsg );         // hdu����ؼ���	

    
    //2�����ֻ���������
	BOOL32 StartMixing( u8 byMixMode );
	void StopMixing( void );
	void ProcMixStopSucRsp( void ); // ������ֹͣ�ɹ���mcu��Ӧ��ҵ����
	BOOL32 AddMixMember( const TMt *ptMt, u8 byVolume = DEFAULT_MIXER_VOLUME, BOOL32 bForce = FALSE );
	void RemoveMixMember( const TMt *ptMt, BOOL32 bForce = FALSE );
	void SetMemberVolume( const TMt *ptMt, u8 byVolume );
	BOOL32 NotifyMixerSendToMt( BOOL32 bSend = TRUE );
	void SetMixerSensitivity( u8 bySensVal );
    void SetMixDelayTime(u16 wDelayTime);
    void SwitchMixMember(const TMt *ptMt, BOOL32 bNMode = FALSE);
    void VACChange(const TMixParam &tMixParam, u8 byExciteChn);
    
	void AddRemoveSpecMixMember( TMt *ptMt = NULL, u8 byMtNum = 0, BOOL32 bAdd = TRUE, BOOL32 bStopMixNonMem = FALSE );
    void SwitchDiscuss2SpecMix();

    void MixerStatusChange(void); 
    u8   GetMixMtNumInGrp(void);
    u8   GetMaxMixNum(void);    //��ȡ��������Ա������Ŀǰ����ɢ����4��������56����
    u8   GetMixChnPos( u8 byMtId, BOOL32 bRemove = FALSE ); //��ȡ������Աͨ��λ�ã��������ֽ����˿ں�
    u8   GetMaxMixerDepth() const;

	//3������������غ���  
    BOOL32 StartAdapt( u8 byAdaptType, u16 wBitRate, TSimCapSet *pDstSCS = NULL, TSimCapSet *pSrcSCS = NULL );
    BOOL32 StartBasAdapt( TAdaptParam* ptParam , u8 byAdaptType, TSimCapSet *pSrcSCS );
    BOOL32 ChangeAdapt(u8 byAdaptType, u16 wBitRate, TSimCapSet *pDstSCS = NULL, TSimCapSet *pSrcSCS = NULL);
    BOOL32 ChangeBasAdaptParam(TAdaptParam *ptParam, u8 byAdaptType, TSimCapSet *pSrcSCS = NULL);
    void   ProcBasStartupTimeout( const CMessage * pcMsg );
    BOOL32 StopBasAdapt( u8 byAdaptType );
    BOOL32 StopBasSwitch( u8 byAdaptType );
    BOOL32 IsMtNeedAdapt(u8 byAdaptType, u8 byMtId, TMt *ptSrcMt = NULL);
    BOOL32 IsMtSrcBas(u8 byMtId, u8 byMode, u8 &byAdpType);
    BOOL32 IsMtSrcVmp2(u8 byMtId);
    BOOL32 IsNeedMdtpAdapt( TSimCapSet &tDstSCS, TSimCapSet &tSrcSCS );
	u16	   GetLstRcvGrpMtBr(BOOL32 bPrimary, u8 byMediaType, u8 byRes, u8 bySrcMtId = 0);
    u16    GetLeastMtReqBitrate(BOOL32 bPrimary = TRUE, u8 byMediaType = MEDIA_TYPE_NULL, u8 bySrcMtId = 0);
    u16    GetLeastMtReqBR(TSimCapSet &tSrcSCS, u8 bySrtMtId = 0, BOOL32 bPrimary = TRUE);
    u8     GetLeastMtReqFramerate(u8 byMediaType = MEDIA_TYPE_NULL, u8 bySrcMtId = 0);
    BOOL32 IsNeedAudAdapt(TSimCapSet &tDstSCS, TSimCapSet &tSrcSCS, TMt *ptSrcMt = NULL);
    BOOL32 IsNeedVidAdapt(TSimCapSet &tDstSCS, TSimCapSet &tSrcSCS, u16 &wAdaptBitRate, TMt *ptSrcMt = NULL);
    BOOL32 IsNeedBrAdapt(TSimCapSet &tDstSCS, TSimCapSet &tSrcSCS, u16 &wAdaptBitRate, TMt *ptSrcMt = NULL);
    BOOL32 IsNeedCifAdp( void );
    BOOL32 IsRecordSrcBas( u8 byType, TMt &tRecSrc, u8 byRecChnlIdx);
    
    u32	   GetBasLastVCUTick( u8 byBasChnnl );
	void   SetBasLastVCUTick( u8 byBasChnnl, u32 dwTick );

    //4 ������������غ���

    //new
    BOOL32 StartHDMVBrdAdapt(void);
    BOOL32 StopHDMVBrdAdapt(void);

    BOOL32 StartHDDSBrdAdapt(void);
    BOOL32 StopHDDSBrdAdapt(void);

    BOOL32 ChangeHDAdapt(const TEqp &tEqp, u8 byChnId, BOOL32 bStart = FALSE);
    BOOL32 StopHDAdapt(const TEqp &tEqp, u8 byChnId);

    void   GetMVBrdSrcSim(TSimCapSet &tSim);
    void   GetDSBrdSrcSim(TDStreamCap &tDSim);
    void   GetSelSrcSim(const TEqp &tEqp, u8 byChnId, TSimCapSet &tSim);
    void   GetSelSrcSim(const TEqp &tEqp, u8 byChnId, TDStreamCap &tDSim);


    void   RefreshRcvGrp(u8 byMtId = 0);
    BOOL32 RefreshBasMVAdpParam(u8 byMtId, BOOL32 bSet = FALSE);
    BOOL32 RefreshBasDSAdpParam(u8 byMtId, BOOL32 bSet = FALSE);
    BOOL32 RefreshBasParam(u8 byMtId, BOOL32 bSet = FALSE, BOOL32 bDual = FALSE);
    BOOL32 RefreshBasParam4AllMt(BOOL32 bSet = FALSE, BOOL32 bDual = FALSE);
    BOOL32 RefreshBasParam4MVBrd( void );
    BOOL32 RefreshBasParam4DSBrd( void );

    BOOL32 RefreshBasMVSelAdpParam(const TEqp &tEqp, u8 byChnId);
    BOOL32 RefreshBasDSSelAdpParam(const TEqp &tEqp, u8 byChnId);

    BOOL32 IsDSSrcH263p( void );
    BOOL32 GetProximalGrp(u8 byMediaType, u8 byRes, u8 &byGrpType, u8 &byGrpRes);

    BOOL32 StartHDCascadeAdp( TMt *ptMt = NULL );
    BOOL32 StopHDCascaseAdp( void );

    BOOL32 StartHdVidSelAdp(TMt tSrc, TMt tDst, u8 byMode = MODE_VIDEO);
    BOOL32 StopHdVidSelAdp(TMt tSrc, TMt tDst, u8 byMode = MODE_VIDEO);
	void   ReleaseResbySel(TMt tSelDstMt, u8 bySelMode);

	BOOL32 StartAudSelAdp(TMt tSrc, TMt tDst);
	BOOL32 StopAudSelAdp(TMt tSrc, TMt tDst);


    //old
    BOOL32 IsRecordSrcHDBas( void );

	//5�������ش���غ��� 
	void ChangePrsSrc( TMt tMt, u8 byPrsChanMode, BOOL32 bSrcBas = FALSE );
    BOOL ChangePrsSrc(u8 byPrsId, u8 byPrsChnlPos, const TMt& tEqp, u8 byEqpOutChnl = 0, u8 byEqpInChnl = 0);
	void AddRemovePrsMember( TMt tMt, BOOL32 bAdd, u8 byPrsChanMode  );
	void AddRemovePrsMember(u8 byMemId, u8 byPrsId, u8 byPrsChl, u8 byPrsMode = MODE_VIDEO, u8 bAdd = TRUE);
	void BuildRtcpAndRtpSwithForPrs(u8 byPrsId, u8 byPrsChl);
	void StopPrs( u8 byPrsChanMode, BOOL32 bNotify = FALSE );
	void StopPrs(u8 byPrsId, u8 byPrsChnl);
	void SwitchVideoRtcpToDst(u32 dwDstIp, u16 wDstPort, 
                              TMt tSrc, u8 bySrcChnnl = 0, 
		                      u8 byMode = MODE_VIDEO,
                              u8 bySwitchMode = SWITCH_MODE_BROADCAST, BOOL32 bMapSrcAddr = FALSE);
	BOOL32 AssignPrsChnnl4HdConf( );
	BOOL32 AssignPrsChnnl4SdConf( );
	BOOL32 AssignPrsChnnl(void);
	BOOL32 GetMtRealSrc(u8 byMtId, u8 byMode, TMt& tMtSrc, u8& byOutChnl);
	void   OccupyPrsChnl(u8 byPrsId, u8 byPrsChnl);
	void   RlsPrsChnl(u8 byPrsId, u8 byPrsChnl);

    //6��VMP��غ��� 
	BOOL32 CheckVmpParam(TVMPParam& tVmpParam, u16& wErrorCode);
	BOOL32 CheckMpuMember(TVMPParam& tVmpParam, u16& wErrorCode, BOOL32 bChgParam = FALSE);
	void CheckMpuAudPollConflct(TVMPParam& tVmpParam, BOOL32 bChgParam = FALSE);
	void CheckSameMtInMultChnnl(TVMPParam & tVmpParam);
	void CheckAdpChnnlLmt(u8 byVmpId, TVMPParam& tVmpParam, CServMsg &cServMsg, BOOL32 bChgParam = FALSE);
	void CheckAdpChnnlAtStart(TVMPParam &tVmpParam, u8 byMaxHdChnnlNum, CServMsg &cServMsg);
	void CheckAdpChnnlAtChange(TVMPParam &tVmpParam, u8 byMaxHdChnnlNum, CServMsg &cServMsg);
    void ChangeVmpStyle(TMt tMt, BOOL32 bAddMt, BOOL32 bChangeVmpParam = TRUE);
    void ChangeVmpSwitch( u8 bySrcType = 0, u8 byState = VMP_STATE_START );
	void CancelOneVmpFollowSwitch( u8 byMemberType, BOOL32 bNotify );
    void ChangeVmpChannelParam(TMt * ptNewSrc, u8 byMemberChnl, TMt * ptOldSrc = NULL);
    void ChangeVmpParam( TVMPParam *ptVMPParam = NULL, BOOL32 bStart = FALSE );
    void ChangeVmpBitRate(u16 wBitRate, u8 byVmpChanNo = 1); 
    u8   AdjustVmpBrdBitRate( TMt *ptMt = NULL);
	u8   GetVmpChnnl( void );
	void SetVmpChnnl( TMt tMt, u8 byChnnl, u8 byMemberType, BOOL32 bMsgMcs = FALSE );
	BOOL32 IsDynamicVmp( void );
	u8   GetVmpDynStyle( u8 byMtNum );
    BOOL32 IsChangeVmpParam(TMt * ptMt);
    BOOL32 IsVMPSupportVmpStyle(u8 byStyle, u8 byEqpId, u16 &wError);
    BOOL32 IsMCUSupportVmpStyle(u8 byStyle, u8 &byEqpId, u8 byEqpType, u16 &wError );
    

	void	AdjustVmpParam(TVMPParam *ptVMPParam, BOOL32 bStart = FALSE);
	BOOL32  ChangeMtVideoFormat( TMt   tMt, TVMPParam * ptVMPParam, BOOL32 bStart = TRUE, BOOL32 bSeize = TRUE, BOOL32 bConsiderVmpBrd = TRUE );  //֧�ּ����Ļ���ϳɷֱ��ʵ���
	void	ChgMtFormatInSdVmp( u8 byMtId, TVMPParam * ptVMPParam, BOOL32 bStart = TRUE );	//��Ա���VMP�ĵ����ֱ���
	BOOL32  ChgMtFormatInMpu( u8 byMtId,	TVMPParam *ptVmpParam,	BOOL32 bStart = TRUE, BOOL32 bSeize = TRUE, BOOL32 bConsiderVmpBrd = TRUE );
	BOOL32	RecoverMtResInMpu(TMt tMt);
	void	AddToVmpNeglected( u8 byMtId);
	BOOL32	IsMtNeglectedbyVmp( u8 byMtId);
	void	ChangeNewVmpParam(TVMPParam *ptVMPParam, BOOL32 bStart = FALSE);
	void	ProcMcsMcuStartSwitchVmpMtReq(const CMessage * pcMsg);
	void	ChairmanSeeVmp(TMt tDstMt, CServMsg &cServMsg );
	void	AdjustVmpParambyMemberType(TMt *ptSpeaker,TVMPParam &tVmpParam, u8 byMembertype = VMP_MEMBERTYPE_SPEAKER,TMt *ptOldSrc = NULL);
    void	UpdataVmpEncryptParam(TMt tMt, u8 byChnlNo);
	BOOL32  VidResAdjust(u8 byVmpStyle, u8 byPos, u8 byMtStandardFormat, u8 &byReqRes);
    BOOL32  IsDelayVidBrdVCU(void);	

    //7��vmp tvwall��غ��� 
    void ChangeVmpTwChannelParam(TMt * ptNewSrc, u8 byMemberType);
    void ChangeVmpTwParam(TVMPParam *ptVmpParam = NULL, BOOL32 bStart = FALSE);
    void ChangeVmpTwSwitch(u8 bySrcType = 0, u8 byState = VMP_STATE_START);
    void SetVmpTwChnnl(TMt tMt, u8 byChnnl, u8 byMemberType, BOOL32 bMsgMcs = FALSE);

    //8��tvall��غ��� 
    void ChangeTvWallSwitch(TMt * ptSrc, u8 byTvWallId, u8 byChnlIdx,u8 byTWMemberType, u8 byState, BOOL32 bEqpStop = TRUE);
	BOOL32 IsMtNotInOtherTvWallChnnl(u8 byMtId, u8 byTvWallId, u8 byChnlId);
    
    //9��recorder��غ��� 
	void StopAllMtRecoderToPeriEqp();
    void AdjustRecordSrcStream( u8 byMode );   
	
	//10��Hdu��غ���
	void ChangeHduSwitch(TMt * ptSrc, u8 byHduId, u8 byChnlIdx,u8 byHduMemberType, u8 byState, u8 byMode = MODE_BOTH, BOOL32 bBatchPoll = FALSE);
    BOOL32 IsMtNotInOtherHduChnnl(u8 byMtId, u8 byHduId, u8 byChnlId);
	BOOL32 HduBatchPollOfOneCycle( BOOL32 bFirstCycle );            // hduһ�����ڵ�������ѯ
	void   StopHduBatchPollSwitch( BOOL32 bStopPlay = FALSE );

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
	BOOL32 StartSwitchToSubMt(TMt tSrc, 
                              u8 bySrcChnnl,
                              u8 byDstMtId,
                              u8 byMode = MODE_BOTH,
                              u8 bySwitchMode = SWITCH_MODE_BROADCAST,
                              BOOL32 bMsgStatus = TRUE,
                              BOOL32 bSndFlowCtrl = TRUE,
                              BOOL32 bIsSrcBrd = FALSE );

	void   StopSwitchToSubMt( u8 byDstMtId,
                              u8 byMode = MODE_BOTH,
                              u8 bySwitchMode = SWITCH_MODE_BROADCAST,
                              BOOL32 bMsgStatus = TRUE,
                              BOOL32 bStopSelByMcs = TRUE,
                              BOOL32 bSelSelf = TRUE );	

	BOOL32 StartSwitchToMcu( TMt tSrc,
                             u8 bySrcChnnl,
                             u8 byDstMtId,
                             u8 byMode = MODE_BOTH,
                             u8 bySwitchMode = SWITCH_MODE_BROADCAST,
                             BOOL32 bMsgStatus = TRUE );

	BOOL32 RestoreAllSubMtJoinedConfWatchingSrcMt(TMt tSrc, u8 byMode = MODE_BOTH );
	
    void   StopSwitchToAllSubMtJoinedConfNeedAdapt( BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH, u8 byAdaptType = ADAPT_TYPE_VID );
	BOOL32 StartSwitchToAllSubMtJoinedConfNeedAdapt( TMt tSrc, u8 bySrcChnnl, u8 byMode = MODE_BOTH, u8 byAdaptType = ADAPT_TYPE_VID );
    
    BOOL32 StartSwitchToSubMtNeedAdp(const TEqp &tEqp, u8 byChnId);
    BOOL32 StopSwitchToSubMtNeedAdp(const TEqp &tEqp, u8 byChnId);
    
    BOOL32 StartSwitchToSubMtFromAdp(u8 byMtId, BOOL32 bDual = FALSE);
    BOOL32 StopSwitchToSubMtFromAdp(u8 byMtId, BOOL32 bDual = FALSE);

    BOOL32 StartHdBasSwitch(const TEqp &tEqp, u8 byChnId);
    BOOL32 StopHdBasSwitch(const TEqp &tEqp, u8 byChnId);

    BOOL32 StartAllHdBasSwitch(BOOL32 bDual = FALSE);
    BOOL32 StopAllHdBasSwitch(BOOL32 bDual = FALSE);

    BOOL32 CheckHdBasChnMode(u8 byMode, u8 &byMediaMode, u8 &bySwitchMode);

    // ����ͬ��
    BOOL32 StartSwitchAud2MtNeedAdp(u8 byDstMtId, u8 bySwitchMode = SWITCH_MODE_BROADCAST);

    // guzh [3/20/2007]�ն˹㲥�������
    BOOL32 StartSwitchFromBrd(const TMt &tSrc, u8 bySrcChnnl, u8 byDstMtNum, const TMt* const ptDst);
    BOOL32 StartSwitchToAllSubMtJoinedConf( const TMt &tSrc, u8 bySrcChnnl );     // ��Ƶ
    BOOL32 StartSwitchToAllSubMtJoinedConf( const TMt &tSrc, u8 bySrcChnnl, const TMt &tOldSrc, BOOL32 bForce ); // ��Ƶ
    void   RestoreRcvMediaBrdSrc( u8 byMtId, u8 byMode = MODE_BOTH, BOOL32 bMsgMtStutas = TRUE );
    void   SwitchSrcToDst( const TSwitchInfo & tSwitchInfo, const CServMsg & cServMsgHdr, BOOL32 bBatchPollChg = FALSE );
    BOOL32 SwitchSrcToDstAccord2Adp( const TSwitchInfo & tSwitchInfo, const CServMsg & cServMsgHdr );
    void   StopSwitchToAllSubMtJoinedConf( u8 byMode = MODE_BOTH );
    void   ProcVidBrdSrcChanged( CMessage * const pcMsg );
    BOOL32 CanMtRecvVideo(const TMt &tDst, const TMt &tSrc);

	//2�����轻��
	BOOL32 StartSwitchToPeriEqp(TMt tSrc,
                                u8  bySrcChnnl,
                                u8  byEqpId,
                                u16 wDstChnnl           = 0,
                                u8  byMode              = MODE_BOTH,
                                u8  bySwitchMode        = SWITCH_MODE_BROADCAST,
                                BOOL32 bDstHDBas        = FALSE,
                                BOOL32 bStopBeforeStart = TRUE,
                                BOOL32 bBatchPollChg    = FALSE);

	void StopSwitchToPeriEqp( u8  byEqpId,
							  u16 wDstChnnl		  = 0,
							  BOOL32 bMsg		  = TRUE,
							  u8     byMode		  = MODE_BOTH,
							  u8     bySwitchMode = SWITCH_MODE_BROADCAST); 

	void StopSwitchToAllPeriEqpWatchingSrcMt( TMt tSrc, BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH );
	void StopSwitchToAllPeriEqpWatchingSrcMcu(  u16 wMcuId, BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH );
	void StopSwitchToAllPeriEqpInConf( BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH );
    BOOL32 StartSwitchToRecNeedHDVidAdapt( void );

	//3����ؽ���
	BOOL32 StartSwitchToMc( TMt tSrc, u8 bySrcChnnl, u16 wMcInstId, u8 byDstChnnl = 0, u8 byMode = MODE_BOTH );
	void StopSwitchToMc( u16 wMcInstId,  u8 byDstChnnl = 0, BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH, BOOL32 bFilterConn = TRUE );
	void StopSwitchToAllMcInConf( BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH );
	void StopSwitchToAllMcWatchingSrcMt( TMt tSrc, BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH );
	void StopSwitchToAllMcWatchingSrcMcu(  u16 wMcuId, BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH );
    BOOL32 IsMtInMcSrc(u8 byMtId, u8 byMode);

    //4���Ž���
    BOOL32 AdjustSwitchBridge(u8 byMpId, BOOL32 bInsert);


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
    /*                          �ˡ����ǻ����������                        */
    /*                                                                      */
    /************************************************************************/
private:

    //1�����鿪�أ�����Ƶ�ʿ���

    void DaemonProcSendMsgToNms(const CMessage* pcMsg);
    void ProcGetFreqFromNMS(const CMessage * pcMsg);
    void ApplySatFrequence();
    void ReleaseSatFrequence();
    u32  GetSerialId();
    void ReleaseConfFreq();

    //2��������Ϣ��������
    u32  SendCmdToNms( u16 wCmd , u8 byBrdNum, u32 dwBrdBit, u16 wNum, u32 dwBitRate, u32 dwFreq[5], u32 dwBrdFreq );
    void ProcApplyFreqTimeOut( const CMessage * pcMsg );
    void ProcNmsMcuReshedconfNotif(  const CMessage*pcMsg );
    void ProcNmsMcuSchedConf( const CMessage*pcMsg );
    void ProcDelayTimeOut( const CMessage* pcMsg );	
    void ProcNmsAddMt(const CMessage* pcMsg );
    void ProcNmsDelMt(const CMessage* pcMsg );
    void ProcNmsAddMtOut( const CMessage* pcMsg );
    void ProcNmsDelMtOut( const CMessage* pcMsg );
    void ProcSchedStartNow( const CMessage* pcMsg );

    //3��MODEM����
    void ProcModemConfRegRsp( const CMessage * pcMsg );
    void ProcModemMcuStatusNotif( const CMessage * pcMsg );
    BOOL StartConfModem(u8 byNum);
    BOOL StopConfModem( u8 byNum );
    void DaemonProcModemReg(const CMessage* pcMsg);

    //4��������ն˿���
    void ProcCreateConfAccordNms( const CMessage * pcMsg );
    void RefreshConfState( void );
	void RefreshConfState( BOOL32 bHdGrp );
	void RefreshMtBitRate( void );
	BOOL RefreshConfMtKey( void );
	void DaemonProcTimerMultiCast( const CMessage * pcMsg, CApp * pcApp );
    
    //5���������ǻ�����ƺ���
    void ProcSchedConfTimeOut( const CMessage * pcMsg );
	u8   GetMtSrc( u8 byMtId, u8 byMode );

    /************************************************************************/
    /*                                                                      */
    /*                          �š���ӡ���                                */
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
	void ConfLog( BOOL32 bFile, s8 * fmt, ... );    
	void ShowVmpAdaptMember( void );
};

typedef zTemplate< CMcuVcInst, MAXNUM_MCU_CONF, CMcuVcData, sizeof( CConfId ) > CMcuVcApp;	//Daemonʵ�����ڴ���ǻ�������

extern  CMcuVcApp	        g_cMcuVcApp;	//MCUҵ��Ӧ��ʵ��
extern  CAgentInterface     g_cMcuAgent;    
extern  CMSManagerSsnApp    g_cMSSsnApp;
extern  CMpManager          g_cMpManager;   //MP������ȫ�ֱ���

#define STATECHECK \
if( CurState() != STATE_ONGOING )\
{\
	ConfLog( FALSE, "Wrong message %u(%s) received in state %u at Line.%u!\n", \
		pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), __LINE__ );\
	return;\
}

#define ISGROUPMODE(byMode) \
((byMode >= VCS_GROUPSPEAK_MODE && byMode <= VCS_GROUPVMP_MODE) ? TRUE : FALSE)


#endif

//END OF FILE

