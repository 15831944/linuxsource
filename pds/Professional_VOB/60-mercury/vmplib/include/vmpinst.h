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
// #include "drawinterface.h"
#ifdef _LINUX_
#include "libsswrap.h"
#endif

OSPEVENT( EV_VMP_TIMER,                EV_VMP_BGN );                //�ڲ���ʱ��
OSPEVENT( EV_VMP_NEEDIFRAME_TIMER,     EV_VMP_BGN + 1 );            //�ؼ�֡��ʱ��
OSPEVENT( EV_VMP_CONNECT_TIMER,        EV_VMP_BGN + 2 );            //���Ӷ�ʱ��
OSPEVENT( EV_VMP_CONNECT_TIMERB,       EV_VMP_BGN + 3 );            //���Ӷ�ʱ��
OSPEVENT( EV_VMP_REGISTER_TIMER,       EV_VMP_BGN + 4 );            //ע�ᶨʱ��
OSPEVENT( EV_VMP_REGISTER_TIMERB,      EV_VMP_BGN + 5 );            //ע�ᶨʱ��
OSPEVENT( TIME_GET_MSSTATUS,           EV_VMP_BGN + 6 );            //ȡ��������״̬

#define VMP_CONNETC_TIMEOUT         (u16)3*1000  //connect time
#define VMP_REGISTER_TIMEOUT        (u16)6*1000  //register time
#define CHECK_IFRAME_INTERVAL       (u16)1000    //�ؼ�֡������ʱ��
#define FIRST_REGACK                (u8)1        //��һ���յ�ע��Ack

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
    u8            m_byMaxChannelNum;	
	
    CKDVVMPParam  m_tParam[2];			// �ϳɲ���
	TNetAddress   m_tSndAddr[2];		// �ϳɺ������ַ
	BOOL32        m_bDbVid;				// �Ƿ�˫��Ƶ

    CConfId       m_cConfId;		// ��ǰ�����
	TVMPCfg       m_tCfg;           // ���ò���
    TVmpStyleCfgInfo  m_tStyleInfo; // �ϳɷ�����

	// CDrawInterface m_cDrawInterface; // ��ͼ��
private:
	//-----------����״̬--------------
	CHardMulPic   m_cHardMulPic;   // ���渴�����ķ�װ
    BOOL32        m_bAddVmpChannel[MAXNUM_MPUSVMP_MEMBER/*MAXNUM_SDVMP_MEMBER*/];	// ��¼��ͨ���Ƿ��Ѿ����

	TPrsTimeSpan  m_tPrsTimeSpan;   // �ش�ʱ�����
    u32           m_dwLastFUPTick;  // ��һ���յ��ؼ�֡�����Tick��

	u16			  m_wMTUSize;		// MTU�Ĵ�С, zgc, 2007-04-02
    TCapSupportEx m_tCapSupportEx;

private:
	//------------��Ϣ��Ӧ--------------------
	void InstanceEntry( CMessage * const pcMsg );                 //��Ϣ���
	void Init( CMessage * const pcMsg );                          //Ӳ����ʼ��
	void MsgRegAckProc( CMessage * const pcMsg );                 //ȷ��
	void MsgRegNackProc( CMessage * const pcMsg );                //�ܾ�
	void MsgStartVidMixProc( CMessage * const pcMsg );            //��ʼ����
	void MsgStopVidMixProc( CMessage * const pcMsg );             //ֹͣ����
	void MsgChangeVidMixParamProc( CMessage * const pcMsg );      //�ı临�ϲ���
	void MsgGetVidMixParamProc( CMessage * const pcMsg );         //��ѯ���ϲ���
	void MsgSetBitRate( CMessage * const pcMsg );
	void SendStatusChangeMsg( u8 byOnline, u8 byState, u8 byStyle ); //����״̬

	void ProcReconnectBCmd( CMessage * const pcMsg );	//MCU ֪ͨ�Ự ����MPCB ��Ӧʵ��, zgc, 2007/04/30
private:
	//-------------����------------------------
    void ProcConnectTimeOut( BOOL32 bIsConnectA );
	BOOL32 ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode );      //����MCU
	void Disconnect( CMessage * const pMsg  );                    //�Ͽ�����
    void ProcRegisterTimeOut( BOOL32 bIsRegisterA );
	void Register( BOOL32 bIsRegisterA, u32 dwMcuNode );                                           //��MCUע��
	BOOL SendMsgToMcu( u16  wEvent, CServMsg* const pcServMsg );  //������Ϣ��MCU
	void StatusShow( void );                                      //��ʾ���е�״̬��ͳ����Ϣ
	BOOL StartHardwareMix( CKDVVMPParam& tParam );                //��ʼӲ���ϳ�
	BOOL ChangeVMPStyle( CKDVVMPParam& tParam );                  //�ı�ϳɷ��
	BOOL ChangeVMPChannel( CKDVVMPParam& tParam );                //�ı��߼�ͨ��
	u8   GetFavirateStyle( u32 totalmap,u8 &channel );            //����MAP��Ŀ�������ϳ�����
	u8   ConvertVcStyle2HardStyle(  u8 oldstyle  );               //ҵ�񵽵ײ�ĺϳ�ģʽת��
	void ClearCurrentInst( void );                                //��յ�ǰʵ��
	void ProcGetMsStatusRsp( CMessage * const pcMsg );            //ȡ��������״̬

	void ParamShow(void);							// ��ʾҵ��ϳɲ���, zgc, 2008-03-19
    u8   GetVmpNumOfStyle( u8 byVMPStyle );
    void MsgUpdataVmpEncryptParamProc( CMessage * const pMsg );
    void SetEncryptParam( u8 byChnNo, TMediaEncrypt *  ptVideoEncrypt, u8 byDoublePayload );
    void SetDecryptParam( u8 byChnNo, TMediaEncrypt *  ptVideoEncrypt, TDoublePayload * ptDoublePayload );

    void MsgTimerNeedIFrameProc( void );
    void MsgFastUpdatePicProc( void );
	
    void ProcSetQosInfo( CMessage * const pcMsg );
    void ComplexQos( u8& byValue, u8 byPrior );
    void ProcSetStyleMsg( CMessage * const pcMsg );
    BOOL32 SetVmpAttachStyle( TVmpStyleCfgInfo&  tStyleInfo );
	// ֹͣMAP�Ĺ���, zgc, 2007/04/24
	void ProcStopMapCmd( CMessage * const pcMsg );
    void ProcSetBackBoardOutCmd( CMessage * const pcMsg );

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
        
};

class CVMPCfg
{
public:
    CVMPCfg();
    virtual ~CVMPCfg();
public:
    u32           m_dwMcuNode;      //���ӵ�MCU.A �ڵ��
    u32           m_dwMcuNodeB;     //���ӵ�MCU.B �ڵ��
    u32           m_dwMcuIId;       //��ʵ��ͨѶ��MCU.A ��ȫ��ID
    u32           m_dwMcuIIdB;      //��ʵ��ͨѶ��MCU.B ��ȫ��ID
    BOOL32        m_bEmbed;         //��Ƕ��A��
    BOOL32        m_bEmbedB;        //��Ƕ��B��
    u8            m_byRegAckNum;    //ע��ɹ�����

	u32           m_dwMpcSSrc;      // guzh [6/12/2007] ҵ���ỰУ��ֵ
public:
    void FreeStatusDataA();
    void FreeStatusDataB();
};

typedef zTemplate<CVMPInst, 1, CVMPCfg> CVmpApp;
extern CVmpApp g_cVMPApp;

#endif //_VIDEO_MULTIPLEXERINST_H_
