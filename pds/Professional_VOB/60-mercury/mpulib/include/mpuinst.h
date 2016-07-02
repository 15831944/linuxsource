/*****************************************************************************
   ģ����      : mpulib
   �ļ���      : mpuinst.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: mpulibӦ��ͷ�ļ�
   ����        : ����
   �汾        : V4.6  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��       �汾        �޸���      �޸�����
   2009/3/14    4.6         �ű���      ע��
   2009/7/19    4.6         �ű���      �߼�����
******************************************************************************/
#ifndef _MPUINST_H_
#define _MPUINST_H_

#include "mpustruct.h"
#include "eqpcfg.h"
#include "mpuutility.h"

#ifdef _LINUX_
#include "libsswrap.h"
#endif


#define MAXNUM_MPUBAS_CHANNEL        (u8)4
#define MAXNUM_MPUBAS_H_CHANNEL		 (u8)2
#define MAX_VIDEO_FRAME_SIZE         (u32)(512 * 1024)
#define MAX_AUDIO_FRAME_SIZE         (u32)(8 * 1024)
#define FRAME_RATE                   (u8)25

extern s32 g_nmpubaslog;

/************************************************************************/
/*                                                                      */
/*                          ����ΪCMpuBasInst                          */
/*                                                                      */
/************************************************************************/

class CMpuBasInst : public CInstance
{
protected:
    enum ESTATE
    {
        IDLE,
        READY,
        NORMAL,
    };

//��ʱ�ͷų������Ժ�����
//private:
public:	
	TMpuBasChannelInfo    m_tChnInfo;
	CMpuBasAdpGroup       *m_pAdpCroup;
    TRtpPkQueue             *m_ptFrmQueue;
	//songkun,201106230,bas�ϳ�Iframe������������ӵȴ���������������������ٷ����ȴ�ʱ���û��䣩
    u8			m_byIframeReqCount;
      
public:
    CMpuBasInst()
    {	    
        m_pAdpCroup = NULL;
        m_ptFrmQueue = NULL;
		m_byIframeReqCount = 0;
    }
    virtual ~CMpuBasInst()
    {
    }

    // ----------- ��Ϣ��� ------------
    void  InstanceEntry( CMessage* const pMsg );
    void  DaemonInstanceEntry( CMessage* const pMsg, CApp* pcApp );

    // --------- ��Ϣ������ ----------
	void  DaemonProcPowerOn( const CMessage* const pMsg/*, CApp* pcApp*/ );       //��ʼ��
    void  DaemonProcOspDisconnect( CMessage* const pMsg/*, CApp* pcApp*/ ); //��������
    void  DaemonProcConnectTimeOut( BOOL32 bIsConnectA );               //���������
    void  DaemonProcRegisterTimeOut( BOOL32 bIsRegiterA );              //ע�������    
    void  DaemonProcMcuRegAck(const CMessage* const pMsg/*, CApp* pcApp */);      //����ע��
    void  DaemonProcMcuRegNack(CMessage* const pMsg) const;
	void  DeamonProcGetMsStatusRsp( CMessage* const pMsg ); 
    //void  DaemonProcSetQosCmd( CMessage* const pMsg );                  //����Qosֵ
	void  DeamonProcModSendIpAddr( CMessage* const pMsg );
	void  DeamonShowBasDebugInfo();

    void  ProcInitBas();
    void  ProcStopBas();
    void  ProcStartAdptReq( CServMsg& );     //��ʼ����
    void  ProcChangeAdptCmd( CServMsg& );    //�����������
    void  ProcStopAdptReq( CServMsg& );      //ֹͣ����
	void  ProcFastUpdatePicCmd( /*CServMsg&*/ ); //Mcu����MpuBas��ؼ�֡
    void  ProcTimerNeedIFrame();             //MpuBas����Mcu���ؼ�֡
	void  ProcModSndAddr();                  //�޸����緢�͵�ַ


	// ----------- ���ܺ��� ------------
    BOOL32 ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode );            //�������Ӻ���
	void   Register( /*BOOL32 bIsRegiterA,*/ u32 dwMcuNode );               //��MCUע��
    void   SendMsgToMcu( u16 wEvent, CServMsg& cServMsg );
    void   SendChnNotif();                                                  //����ͨ��״̬��Mcu
    BOOL32 GetBasInitChnlNumAcd2WorkMode(u8 byWorkMode, u8 &byInitChnlNum); //����bas������ģʽ������ʼ�����ٸ�(��)ͨ��
	BOOL32 GetBasOutChnlNumAcd2WorkMode(u8 byWorkMode, u8 &byOutChnlNum);   //����bas������ģʽ������ʼ�����ٸ�(��)ͨ��
    void   StatusShow( void );
	void   ClearInstStatus( void );                                     //��ձ�ʵ�������״̬��Ϣ
};

typedef zTemplate< CMpuBasInst , MAXNUM_MPUBAS_CHANNEL, CMpuBasCfg > CMpuBasApp;
extern CMpuBasApp g_cMpuBasApp;


 
/************************************************************************/
/*                                                                      */
/*                          ����ΪCMpuSVmpInst                          */
/*                                                                      */
/************************************************************************/

class CMpuSVmpInst : public CInstance
{
	enum //ʵ��״̬
	{
		IDLE   = 0,
		NORMAL = 1,
		RUNNING = 2
	};

public:
    CMpuSVmpInst() 
    {
        Clear();
    }
    virtual ~CMpuSVmpInst() 
    {
        u8 byLoop = 0;
        for ( ; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++ )
        {
            if ( NULL != m_pcMediaRcv[byLoop] )
            {
                delete m_pcMediaRcv[byLoop];
                m_pcMediaRcv[byLoop] = NULL;
            }
        }
        byLoop = 0;
        for ( ; byLoop < MAXNUM_MPUSVMP_CHANNEL; byLoop++ )
        {
            if ( NULL != m_pcMediaSnd[byLoop] )
            {
                delete m_pcMediaSnd[byLoop];
                m_pcMediaSnd[byLoop] = NULL;
            }
        }
    }

private:
    u32           m_dwMcuNode;       // ���ӵ�MCU.A �ڵ��
    u32           m_dwMcuNodeB;      // ���ӵ�MCU.B �ڵ��
    u32           m_dwMcuIId;        // ��ʵ��ͨѶ��MCU.A ��ȫ��ID
    u32           m_dwMcuIIdB;       // ��ʵ��ͨѶ��MCU.B ��ȫ��ID
    u8            m_byRegAckNum;     // ע��ɹ�����
	u32           m_dwMpcSSrc;       // ҵ���ỰУ��ֵ

    CConfId       m_cConfId;		 // ��ǰ�����
    u8            m_byChnConfType;   // �����ڸ��廹�Ǳ������,���ܻ��õ�

    TVmpStyleCfgInfo  m_tStyleInfo;  // �ϳɷ�����	
    

public:
	CHardMulPic   m_cHardMulPic;   
	CKdvMediaRcv* m_pcMediaRcv[MAXNUM_MPUSVMP_MEMBER];   // 20������
	CKdvMediaSnd* m_pcMediaSnd[MAXNUM_MPUSVMP_CHANNEL];  // ���4·���� 
    
	CKDVNewVMPParam   m_tParam[MAXNUM_MPUSVMP_CHANNEL];	 // �ϳɲ���
    CKDVNewVMPParam   m_tParamPrevious[MAXNUM_MPUSVMP_CHANNEL];  // ��һ�κϳɲ���
    u8            m_byHardStyle;     // ����ϳɷ��m_tParam[0].m_byVMPStyle��mcu�õĲ��ǵײ��õģ� 
	u32           m_dwMcuRcvIp;      // MCU���յ�ַ
    u16           m_wMcuRcvStartPort;// MCU������ʼ�˿ں�

	TEqpCfg       m_tCfg;            // ��������
    TEqp          m_tEqp;            // ���豸
	TMpuVmpCfg    m_tMpuVmpCfg;      // �߼����ã���ȡ�ͱ���mcueqp.ini.ע����TMpuVmpCfg������TMpuDVmpCfg

	BOOL32        m_bAddVmpChannel[MAXNUM_MPUSVMP_MEMBER];// ��¼��ͨ���Ƿ��Ѿ����

	BOOL32		  m_bUpdateMediaEncrpyt[MAXNUM_MPUSVMP_CHANNEL];// ��¼����������Ϣ�Ƿ��Ѹ���

    //songkun,20110622,����ϳ�Iframe�������
    //[nizhijun 2011/03/24]�ò�����ʾ���ϳɲ��������仯ʱ(����/�ı�)����ʱ1S����Ĺؼ�֡�����Ƿ񷢳���MCU
    u8			m_byIframeReqCount[MAXNUM_MPUSVMP_MEMBER];
private:
	//-----------����״̬--------------                 // ���渴�����ķ�װ
	TPrsTimeSpan  m_tPrsTimeSpan;                       // �ش�ʱ�����
	u16			  m_wMTUSize;		                    // MTU�Ĵ�С
    u32           m_adwLastFUPTick[MAXNUM_MPUSVMP_CHANNEL];   // ��һ���յ��ؼ�֡�����Tick��

public:
	//------------��Ϣ��Ӧ--------------------
    void Clear();
	void InstanceEntry( CMessage * const pcMsg );                    //��Ϣ���
	void Init( CMessage * const pcMsg );                             //Ӳ����ʼ��
	void MsgRegAckProc( CMessage * const pcMsg );                    //ȷ��
	void MsgRegNackProc( CMessage * const pcMsg );                   //�ܾ�
	void MsgStartVidMixProc( CMessage * const pcMsg );               //��ʼ����
	void MsgStopVidMixProc( CMessage * const pcMsg );                //ֹͣ����
	void MsgChangeVidMixParamProc( CMessage * const pcMsg );         //�ı临�ϲ���
	void MsgSetBitRate( CMessage * const pcMsg );
	void MsgModSendAddrProc( CMessage* const pMsg );
	
	void SendStatusChangeMsg( u8 byOnline, u8 byState/*, u8 byStyle */); //����״̬
    s32  SendMsgToMcu( u16 wEvent, CServMsg& cServMsg );  

public:
	//-------------����------------------------
    void ProcConnectTimeOut( BOOL32 bIsConnectA );
	BOOL32 ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode );         //����MCU
	void Disconnect( CMessage * const pMsg  );                       //�Ͽ�����
    void ProcRegisterTimeOut( BOOL32 bIsRegisterA );
	void Register(/* BOOL32 bIsRegisterA, */u32 dwMcuNode );             //��MCUע��
	void StatusShow( void );                                         //��ʾ���е�״̬��ͳ����Ϣ
	BOOL StartNetRcvSnd(u8 byNeedPrs);                               //��ʼ�����շ�
    BOOL StopNetRcvSnd( void );                                      //ֹͣ�����շ�
    BOOL ChangeChnAndNetRcv(/*u8 byNeedPrs,*/ BOOL32 bStyleChged);       //���б��������պ�ͨ��
	BOOL ChangeVMPStyle();                                           //�ı�ϳɷ��
	u8   ConvertVcStyle2HardStyle( u8 byVcStyle );                   //ҵ�񵽵ײ�ĺϳ�ģʽת��
    u8   ConvertHardStyle2VcStyle( u8 byHardStyle );                 //�ײ�ĺϳ�ģʽ��ҵ��ת��
	void ClearCurrentInst( void );                                   //��յ�ǰʵ��
	void ProcGetMsStatusRsp( CMessage * const pcMsg );               //ȡ��������״̬

	void ParamShow( void );							                 //��ʾҵ��ϳɲ���
    void MsgUpdataVmpEncryptParamProc( CMessage * const pMsg );
    void SetEncryptParam( u8 byChnNo, TMediaEncrypt *  ptVideoEncrypt, u8 byRealPT );
    void SetDecryptParam( u8 byChnNo, TMediaEncrypt *  ptVideoEncrypt/*, TDoublePayload * ptDoublePayload */);
    void SetPrs( u8 byNeedPrs );
	
    void InitMediaSnd( u8 byChnNo );
    BOOL InitMediaRcv( u8 byChnNo );  
    void StartMediaRcv( u8 byChnNo );
	BOOL SetMediaRcvNetParam( u8 byChnNo );

	
    void MsgTimerNeedIFrameProc( void );
    void MsgFastUpdatePicProc( CMessage * const pMsg );
	
    //FIXME: QOS
    /*
    void ProcSetQosInfo( CMessage * const pcMsg );
    void ComplexQos( u8& byValue, u8 byPrior ); */

    BOOL32 SetVmpAttachStyle( TVmpStyleCfgInfo&  tStyleInfo );
    void GetRGBColor( u32 dwColor, u8& byRColor, u8& byGColor, u8& byBColor );
    u8   VmpGetActivePayload(u8 byRealPayloadType);

    // ƽ������֧��
	//[2010/12/31 zhushz]ƽ�����ͽӿڷ�װ
//  void SetSmoothSendRule( u32 dwDestIp, u16 wPort, u32 dwRate );
	void SetSmoothSendRule( BOOL32 bSetFlag );
//  void ClearSmoothSendRule( u32 dwDestIp, u16 wPort ); 
    
	void ProcSetFrameRateCmd( CMessage * const pcMsg );
	
	void MsgAddRemoveRcvChnnl(CMessage * const pMsg);	//����/ɾ��ĳ����ͨ��// [9/15/2010 xliang] 

	void ShowVMPDebugInfo();							//��ʾVMPdebug��Ϣ [nizhijun 2010/11/30]


	void ProcChangeMemAliasCmd(CMessage* const pMsg);

protected:
	void Trans2EncParam(u8 byEncChnIdx,CKDVNewVMPParam* ptKdvVmpParam, TVideoEncParam* ptVideEncParam);//��kdvnewvmpparamת��Ϊ��Ӧ��TVideoEncParam
};

class TMpuSVmpRcvCB
{
public:
    CMpuSVmpInst*  m_pThis;
    u8             m_byChnnlId; 
};

class CMpuSVmpCfg
{
public:
    CMpuSVmpCfg() {}
    virtual ~CMpuSVmpCfg() {}

public:
    u8               m_byWorkMode;               // ����ʹ��ȫ�ֱ���,�ڴ˱���WorkMode
    u8               m_bEmbed;
    TMpuSVmpRcvCB    m_atMpuSVmpRcvCB[MAXNUM_MPUSVMP_MEMBER];  
};

typedef zTemplate< CMpuSVmpInst , 1, CMpuSVmpCfg > CMpuSVmpApp;
extern CMpuSVmpApp g_cMpuSVmpApp;

extern TResBrLimitTable g_tResBrLimitTable; //���ʷֱ���������Ϣ��


#endif // !_MPUINST_H_

//END OF FILE

