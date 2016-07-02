/*****************************************************************************
   ģ����      : ���渴����( Video Multiplexer)
   �ļ���      : vmpInst.h
   ����ʱ��    : 2003�� 12�� 4��
   ʵ�ֹ���    : VMPInst.cpp
   ����        : zhangsh
   �汾        : 
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   03/12/4	v1.0	       zhangsh	    create
   2006/03/21  4.0         �ű���      �����Ż�
******************************************************************************/
#ifndef _VIDEO_MULTIPLEXERINST_H_
#define _VIDEO_MULTIPLEXERINST_H_

#include "kdvtype.h"
#include "osp.h"
#include "evmcueqp.h"
#include "mcustruct.h"
#include "mcuconst.h"
#include "vmpcfg.h"
#include "eqpcfg.h"
#include "kdvlog.h"
#include "loguserdef.h"
#include "drawbmp.h"

OSPEVENT( EV_VMP_TIMER,                EV_VMP_BGN );                //�ڲ���ʱ��
OSPEVENT( EV_VMP_NEEDIFRAME_TIMER,     EV_VMP_BGN + 1 );            //�ؼ�֡��ʱ��
OSPEVENT( EV_VMP_CONNECT_TIMER,        EV_VMP_BGN + 21 );            //���Ӷ�ʱ��
OSPEVENT( EV_VMP_CONNECT_TIMERB,       EV_VMP_BGN + 22 );            //���Ӷ�ʱ��
OSPEVENT( EV_VMP_REGISTER_TIMER,       EV_VMP_BGN + 23 );            //ע�ᶨʱ��
OSPEVENT( EV_VMP_REGISTER_TIMERB,      EV_VMP_BGN + 24 );            //ע�ᶨʱ��
OSPEVENT( TIME_GET_MSSTATUS,           EV_VMP_BGN + 25 );            //ȡ��������״̬
OSPEVENT( TIME_ERRPRINT,               EV_VMP_BGN + 26 );            //�����ӡ��ʱ��
OSPEVENT( TIME_VMP_FLASHLOG,           EV_VMP_BGN + 27 );            //ˢ����־��¼��ʱ��

#define VMP_CONNETC_TIMEOUT         (u16)3*1000  //connect time
#define VMP_REGISTER_TIMEOUT        (u16)6*1000  //register time
#define CHECK_IFRAME_INTERVAL       (u16)1000    //�ؼ�֡������ʱ��
#define FIRST_REGACK                (u8)1        //��һ���յ�ע��Ack
#ifdef _8KI_
	#define MAXNUM_VMP_ENCNUM           (u8)6      //8ki����ϳ���������ͨ����
#elif defined _8KH_
	#define MAXNUM_VMP_ENCNUM           (u8)5      //8kh����ϳ���������ͨ����
#else
	#define MAXNUM_VMP_ENCNUM           (u8)4      //8ke����ϳ���������ͨ����
#endif

#define VMP_FLASHLOG_TIMEOUT        (u32)(5*60*1000)    //��־ˢ�¼�¼

class CVMPInst : public CInstance
{
	enum //ʵ��״̬
	{
		IDLE   = 0,
		NORMAL = 1,
		RUNNING = 2
	};
public:
	CVMPInst();
	~CVMPInst();
private:

    C8KEVMPParam  m_tParam[MAXNUM_VMP_ENCNUM];			// �ϳɲ���
	TNetAddress   m_tSndAddr[MAXNUM_VMP_ENCNUM];		// �ϳɺ������ַ
    TNetAddress   m_tSndLocalAddr[MAXNUM_VMP_ENCNUM];   // ���͵ı��ص�ַ
    TMediaEncrypt m_tEncEncrypt[MAXNUM_VMP_ENCNUM];     // ������ܲ���
	u8			  m_abyChanProfileType[MAXNUM_VMP_ENCNUM]; //�ϳ�ͨ����hp/bp����

	BOOL32        m_bDbVid;				// ��ǰ�Ƿ�˫��Ƶ
    BOOL32        m_byNeedPrs;
	

    CConfId       m_cConfId;		// ��ǰ�����
	
    TVmpStyleCfgInfo  m_tStyleInfo; // �ϳɷ�����

private:
	//-----------����״̬--------------
	CMulPic      *m_pcHardMulPic;   // ���渴�����ķ�װ
    BOOL32        m_abChnlValid[MAXNUM_SDVMP_MEMBER];	// ��¼��ͨ���Ƿ��Ѿ����
	BOOL32        m_abChnlSetBmp[MAXNUM_SDVMP_MEMBER];
	BOOL32        m_abChnlIframeReqed[MAXNUM_SDVMP_MEMBER];	//�ؼ�֡�����Ƿ��Ѿ��������������������ļ���ʱ
    TVmpMbAlias   m_tVmpMbAlias[MAXNUM_SDVMP_MEMBER]; // Vmp��Ա���� [7/4/2013 liaokang]

	TPrsTimeSpan  m_tPrsTimeSpan;   // �ش�ʱ�����
    u32           m_dwLastFUPTick;  // ��һ���յ��ؼ�֡�����Tick��

	u16			  m_wMTUSize;		// MTU�Ĵ�С, zgc, 2007-04-02
    TCapSupportEx m_tCapSupportEx;
	CCPParam      m_tCfg;           // ���ò���
	TDebugVal	  m_tDbgVal;
    CDrawBmp      m_cBmpDrawer;    // ��Ӧsetlogo�½ӿڣ�����bitmap [6/9/2013 liaokang]

private:
	//------------��Ϣ��Ӧ--------------------
    void DaemonInstanceEntry( CMessage* const pcMsg, CApp* pcApp );
	void InstanceEntry( CMessage * const pcMsg );                 //��Ϣ���
	void Init( CMessage * const pcMsg );                          //Ӳ����ʼ��
	void MsgRegAckProc( CMessage * const pcMsg );                 //ȷ��
	void MsgRegNackProc( CMessage * const pcMsg );                //�ܾ�
	void MsgStopVidMixProc( CMessage * const pcMsg );             //ֹͣ����
	void MsgChangeVidMixParamProc( CMessage * const pcMsg );      //�ı临�ϲ���
	void MsgGetVidMixParamProc( CMessage * const pcMsg );         //��ѯ���ϲ���
	void MsgSetBitRate( CMessage * const pcMsg );
    void SendStatusChangeMsg( u8 byOnline, u8 byState); //����״̬

private:
	//-------------����------------------------
    void ProcConnectTimeOut( void );
	BOOL32 ConnectMcu( u32& dwMcuNode );      //����MCU
	void Disconnect( CMessage * const pMsg  );                    //�Ͽ�����
    void ProcRegisterTimeOut( void );
	void Register( u32 dwMcuNode );                                           //��MCUע��
	BOOL SendMsgToMcu( u16  wEvent, CServMsg* const pcServMsg );  //������Ϣ��MCU
	void StatusShow( void );                                      //��ʾ���е�״̬��ͳ����Ϣ
	void ChangeCP(CServMsg& cServMsg);                            //�ı�ϳɲ���
	u32  ConvertVcStyle2HardStyle(  u8 oldstyle  );               //ҵ�񵽵ײ�ĺϳ�ģʽת��
	void ClearCurrentInst( void );                                //��յ�ǰʵ��

    void ProcVmpFlashLogTimeOut( void );

	void ParamShow(void);							// ��ʾҵ��ϳɲ���, zgc, 2008-03-19

    void MsgUpdataVmpEncryptParamProc( CMessage * const pMsg );

    BOOL32 SetEncryptParam( u8 byChnNo, TMediaEncrypt *ptVideoEncrypt, u8 byMeidaType );
    BOOL32 GetEncryptParam( C8KEVMPParam *ptParam, TMediaEncrypt *ptVideoEncrypt );

    BOOL32 SetDecryptParam( u8 byChnNo, TMediaEncrypt *ptVideoEncrypt, TDoublePayload * ptDoublePayload );

	void SetNeedIFrameTimer4VmpChl(void); //���ÿ��ͨ����ʱ����ؼ�֡
    void MsgTimerNeedIFrameProc( CMessage * const pMsg );
    void MsgFastUpdatePicProc( CMessage * const pMsg );
    void MsgChnnlLogOprCmd( CMessage * const pMsg );
	
	void ProcChangeMemAliasCmd(CMessage * const pMsg);
	
    void ProcSetQosInfo( CMessage * const pcMsg );
    void ComplexQos( u8& byValue, u8 byPrior );
    void ProcSetStyleMsg( CMessage * const pcMsg );
    BOOL32 SetVmpAttachStyle( TVmpStyleCfgInfo&  tStyleInfo, C8KEVMPParam & tParam );

    u8 GetRColor( u32 dwColorValue );
    u8 GetGColor( u32 dwColorValue );
    u8 GetBColor( u32 dwColorValue );
    void GetRGBColor( u32 dwColor, u8& byRColor, u8& byGColor, u8& byBColor );
    
    u16 GetSendRtcpPort(u8 byChanNo);
    u8  VmpGetActivePayload(u8 byRealPayloadType);
    u8 GetVmpChlNumByStyle( u8 byVMPStyle );

    // ƽ������֧��
    void SetSmoothSendRule(u32 dwDestIp, u16 wPort, u32 dwRate);
    void ClearSmoothSendRule(u32 dwDestIp, u16 wPort);
    void StopSmoothSend();

    BOOL32 IsSameEncParam( TVidEncParam * ptEncParam1, TVidEncParam * ptEncParam2 );
    void Trans2EncParam(  C8KEVMPParam * ptVmpParam, TVidEncParam * ptEncparam );
    s8 GetEncChnIdxByParam( TVidEncParam * ptEncParam );
	//zhouyiliang20110331 ���Ӵ���ͣ������ͨ��������
	BOOL32 SetOutputChnnlActive(u8 byOutChnlIdx,BOOL32 bActive);
	void ProcStartStopChnnlCmd(CMessage * const pcMsg);
	void MsgAddRemoveRcvChnnl( CMessage * const pcMsg);
//	void ProcStartStopAllChnnlCmd(CMessage * const pcMsg);
	//zhouyiliang 20110629 �����޸�ĳһ����ͨ���������
//	void ProcChangeSingleChnlEncParam(CMessage* const pcMsg);

    void SetLogo(u8 byChnNo, BOOL32 bDisplay, s8 *pszBuf = NULL);
        
};

class CVMPCfg
{
public:
    CVMPCfg();
    virtual ~CVMPCfg();
public:
    u32           m_dwMcuNode;      //���ӵ�MCU.A �ڵ��
    u32           m_dwMcuIId;       //��ʵ��ͨѶ��MCU.A ��ȫ��ID
    BOOL32        m_bEmbed;         //��Ƕ��A��
    u8            m_byRegAckNum;    //ע��ɹ�����
    T8keEqpCfg       m_tEqpCfg;        //�����������
    BOOL32        m_bInitOk;        //��������Ƿ��ʼ���ɹ�
	u32			  m_dwIframeInterval;	//[nizhijun 2011/03/21]VMP�ؼ�֡��ʱ����ʱ��
    
public:
    void FreeStatusData();
};

typedef zTemplate<CVMPInst, 1, CVMPCfg> CVmpApp;
extern CVmpApp g_cVMPApp;


#endif //_VIDEO_MULTIPLEXERINST_H_

