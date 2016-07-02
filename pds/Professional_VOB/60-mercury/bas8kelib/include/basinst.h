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
#include "bascfg.h"
#include "eqpcfg.h"

OSPEVENT( EV_BAS_TIMER,                EV_BAS_BGN );                //�ڲ���ʱ��
OSPEVENT( EV_BAS_NEEDIFRAME_TIMER,     EV_BAS_BGN + 1 );            //�ؼ�֡��ʱ��
OSPEVENT( EV_BAS_CONNECT_TIMER,        EV_BAS_BGN + 2 );            //���Ӷ�ʱ��
OSPEVENT( EV_BAS_CONNECT_TIMERB,       EV_BAS_BGN + 3 );            //���Ӷ�ʱ��
OSPEVENT( EV_BAS_REGISTER_TIMER,       EV_BAS_BGN + 4 );            //ע�ᶨʱ��
OSPEVENT( EV_BAS_REGISTER_TIMERB,      EV_BAS_BGN + 5 );            //ע�ᶨʱ��
OSPEVENT( TIME_GET_MSSTATUS,           EV_BAS_BGN + 6 );            //ȡ��������״̬
OSPEVENT( TIME_ERRPRINT,               EV_BAS_BGN + 7 );            //�����ӡ��ʱ��
OSPEVENT( TIME_BAS_FLASHLOG,           EV_BAS_BGN + 8 );            //ˢ����־��¼��ʱ��
OSPEVENT( EV_BAS_SHOW,				   EV_BAS_BGN + 9 );			//��ʾ������״̬
OSPEVENT( EV_BAS_INI,				   EV_BAS_BGN + 10 );           //bas��ʼ��
OSPEVENT( EV_BAS_QUIT,				   EV_BAS_BGN + 11 );			//ֹͣbas

#define BAS_CONNETC_TIMEOUT         (u16)3*1000  //connect time
#define BAS_REGISTER_TIMEOUT        (u16)6*1000  //register time
#define CHECK_IFRAME_INTERVAL       (u16)1000    //�ؼ�֡������ʱ��
#define FIRST_REGACK                (u8)1        //��һ���յ�ע��Ack
#if defined(_8KH_) || defined(_8KI_)
	#define MAXNUM_BAS_ENCNUM           (u8)5      //BAS������ͨ����
#else
	#define MAXNUM_BAS_ENCNUM           (u8)4      //BAS������ͨ����
#endif
#define BAS_FLASHLOG_TIMEOUT        (u32)(5*60*1000)    //��־ˢ�¼�¼
#define BAS_REG_NUM					(u8)3        //������ע�����

class CBASInst : public CInstance
{
	enum //ʵ��״̬
	{
		IDLE   = 0,
		NORMAL = 1,
		RUNNING = 2
	};
public:
	CBASInst();
	~CBASInst();
private:

    C8KEVMPParam  m_tParam[MAXNUM_BAS_ENCNUM];			// �ϳɲ���
	TNetAddress   m_tSndAddr[MAXNUM_BAS_ENCNUM];		// ���͵�Զ�˵�ַ
    TNetAddress   m_tSndLocalAddr[MAXNUM_BAS_ENCNUM];   // ���͵ı��ص�ַ
	TNetAddress   m_tRcvLocalAddr;						// �������յ�ַ
	TNetAddress   m_tRtcpAddr;							// Զ��Rtcp�����ַ
	TNetAddress   m_tLocalSndRtcpAddr;					// ����Snd rtcp��ַ
    TMediaEncrypt m_tEncEncrypt[MAXNUM_BAS_ENCNUM];     // ������ܲ���
	//BOOL32        m_bDbVid;							// ��ǰ�Ƿ�˫��Ƶ
	u8            m_byEncNum;							// ��ǰ����·��(��������ؼ�֡)
    BOOL32        m_byNeedPrs;							// �Ƿ�֧�ֶ����ش�

    CConfId       m_cConfId;							// ��ǰ�����
	CCPParam      m_tCfg;								// ���ò���
    TVmpStyleCfgInfo  m_tStyleInfo;						// �ϳɷ�����
	T8keEqpCfg       m_tEqpCfg;							// Ϊÿ��instance����һ��������Ϣ
    u8            m_byChnType;                          // basͨ������(ע��mcuʱͨ��mcu,����������˫����ѡ������ͨ��)
	u32           m_dwMcuIId;							// ��ʵ��ͨѶ��MCU.A ��ID
	BOOL32        m_bInitOk;							// ��������ʼ���Ƿ�ɹ�
    //songkun,20110622,bas�ϳ�Iframe������������ӵȴ���������������������ٷ����ȴ�ʱ���û��䣩
    u8			m_byIframeReqCount;
private:
	//-----------����״̬--------------
	CMulPic      *m_pcHardMulPic;						// ���渴�����ķ�װ
    BOOL32        m_abChnlValid[MAXNUM_VMP_MEMBER];		// ��¼��ͨ���Ƿ��Ѿ����

	TPrsTimeSpan  m_tPrsTimeSpan;						// �ش�ʱ�����
    u32           m_dwLastFUPTick;						// ��һ���յ��ؼ�֡�����Tick��
	u16			  m_wMTUSize;							// MTU�Ĵ�С, zgc, 2007-04-02
    TCapSupportEx m_tCapSupportEx;

private:
	//------------daemonʵ��--------------------
    void DaemonInstanceEntry( CMessage* const pcMsg, CApp* pcApp );
	void DaemonProInit( CMessage * const pcMsg );                 //��ʼ��
	void DaemonProcConnectTimeOut( void );						  //����
	void DaemonProcRegisterTimeOut( void );						  //ע��	
	void DaemonProcDisconnect( CMessage * const pMsg  );          //����

private:
	//-------------instance����------------------------
	void InstanceEntry( CMessage * const pcMsg );                 
	void ProBasInit(void);										  //Ӳ����ʼ��
	void ProcRegAck( CMessage * const pcMsg );                    //ע��ɹ�
	void ProcRegNack();                   //ע��ʧ��
	void ProcStopAdapt( CMessage * const pcMsg );                 //ֹͣ����
	void ProChangeAdapt( CMessage * const pcMsg );				  //�ı��������

private:
	//-------------���ܺ���----------------------------
	BOOL32 ConnectMcu( u32& dwMcuNode );						  //����MCU   
	void ProRegister();											  //��MCUע��
	BOOL SendMsgToMcu( u16  wEvent, CServMsg* const pcServMsg );  //������Ϣ��MCU
	void SendChnNotif(void);									  //
	void SendStatusChangeMsg( u8 byOnline, u8 byState);			  //����״̬
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
    void MsgTimerNeedIFrameProc( void );
    void MsgFastUpdatePicProc( CMessage * const pMsg );
    void MsgChnnlLogOprCmd( CMessage * const pMsg );
	
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
    void Trans2EncParam( const C8KEVMPParam * ptVmpParam, TVidEncParam * ptEncparam, BOOL32 bIsSupportHp = FALSE);
    s8 GetEncChnIdxByParam( TVidEncParam * ptEncParam );
     
	void ConvertRealParam(u8 byOutIdx,THDAdaptParam &tBasParam);
};

class CBASCfg
{
public:
    CBASCfg();
    virtual ~CBASCfg();
public:
    u32           m_dwMcuNode;      //���ӵ�MCU.A �ڵ��
    u32           m_dwMcuIId;       //��ʵ��ͨѶ��MCU.A ��ȫ��ID
    BOOL32        m_bEmbed;         //��Ƕ��A��
    u8            m_byRegAckNum;    //ע��ɹ�����
    T8keEqpCfg    m_tEqpCfg;        //�����������
    BOOL32        m_bInitOk;        //��������Ƿ��ʼ���ɹ�
	u32			  m_dwIframeInterval;	//[nizhijun 2011/03/21]VMP�ؼ�֡��ʱ����ʱ��
    
public:
    void FreeStatusData();
	void ConvertToRealFR(THDAdaptParam& tAdptParm);
};

typedef zTemplate<CBASInst, BAS_REG_NUM, CBASCfg> CBasApp;
extern CBasApp g_cBasApp;

#endif //_VIDEO_MULTIPLEXERINST_H_

