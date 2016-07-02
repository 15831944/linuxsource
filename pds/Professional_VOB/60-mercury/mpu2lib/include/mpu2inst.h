/*****************************************************************************
   ģ����      : mpu2lib
   �ļ���      : mpu2inst.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: mpu2libӦ��ͷ�ļ�
   ����        : ��־��
   �汾        : V4.7  Copyright(C) 2012-2015 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��       �汾        �޸���      �޸�����
   2012/07/26  4.7         ��־��		����
******************************************************************************/
#ifndef _MPUINST_H_
#define _MPUINST_H_

#include "mpu2struct.h"
#include "eqpcfg.h"
#include "mpu2utility.h"
#include "mcuinnerstruct.h"

#ifdef _LINUX_
#include "libsswrap.h"
#endif
#include "drawbmp.h"

#define MAXNUM_MPU2BAS_BASIC_CHANNEL         (u8)4
#define MAXNUM_MPU2BAS_EHANCED_CHANNEL		 (u8)7
#define MAXNUM_MPUBAS_CHANNEL        (u8)4
#define MAXNUM_MPUBAS_H_CHANNEL		 (u8)2
#define MAX_VIDEO_FRAME_SIZE         (u32)(512 * 1024)
#define MAX_AUDIO_FRAME_SIZE         (u32)(8 * 1024)
#define FRAME_RATE                   (u8)25

extern s32 g_nmpubaslog;

extern u16 g_wKdvlogMid; //��ǰ��ӡ��kdvlog��ģ��



/************************************************************************/
/*                                                                      */
/*                          ����ΪCMpuBasInst                          */
/*                                                                      */
/************************************************************************/

class CMpu2BasInst : public CInstance
{
private:	
	TMpuBasChannelInfo		 m_tChnInfo;			//BASͨ��״̬��Ϣ
	CMpu2BasAdpGroup		 *m_pAdpCroup;			//����ͨ��Ӧ�÷�װ(����)
    TRtpPkQueue              *m_ptFrmQueue;			//��Ƶ���������
	BOOL32					m_bIsIframeFirstReq;  //[nizhijun 2011/03/24]�ò�����ʾ��������������仯ʱ(����/�ı�)����ʱ1S����Ĺؼ�֡�����Ƿ񷢳���MCU 
    u8						m_byBasIdx;			//���ڵڼ�������
	u8						m_byChnType;		//mpu2 basÿ��ͨ��������ͬ������Ҫ��8KE BASͨ���ϱ�һ��������
	u8						m_byBasChnIdx;		//basͨ���ż�¼����Ϊ����������������������Ҫ��¼��ֵ
protected:
	//daemon״̬��
	enum EDaemonInstancStatus
	{
			DAEMON_IDLE ,
			DAEMON_INIT ,
			DAEMON_NORMAL ,
	};
	
	//instance״̬��
    enum EInstanceState
    {
        IDLE,
		READY,
		RUNNING,
    };
	
public:
    CMpu2BasInst()
    {	    
        m_pAdpCroup = NULL;
        m_ptFrmQueue = NULL;
		m_bIsIframeFirstReq = FALSE;
    }
    virtual ~CMpu2BasInst()
    {
    }
	
    // ----------- ��Ϣ��� ------------
    void  InstanceEntry( CMessage* const pMsg );
    void  DaemonInstanceEntry( CMessage* const pMsg, CApp* pcApp );
	
    // --------- ��Ϣ������ ----------
	void  DaemonProcPowerOn( const CMessage* const pMsg);         //��ʼ��
    void  DaemonProcOspDisconnect( CMessage* const pMsg );			//��������
    void  DaemonProcConnectTimeOut( BOOL32 bIsConnectA );							//���������
    void  DaemonProcRegisterTimeOut( CMessage* const pMsg, BOOL32 bIsRegiterA );    //ע�������    
    void  DaemonProcMcuRegAck(const CMessage* const pMsg);        //����ע��
	void  DaemonProc8KIMcuRegAck(const CMessage* const pMsg);
    void  DaemonProcMcuRegNack(CMessage* const pMsg) const;
	void  DeamonProcGetMsStatusRsp( CMessage* const pMsg ); 
	void  DeamonProcModSendIpAddr( CMessage* const pMsg );
	void  DeamonShowBasDebugInfo();
	
    void  ProcInitBas(CMessage* const pcMsg);
    void  ProcStopBas();
    void  ProcStartAdptReq( CServMsg& );     //��ʼ����
    void  ProcChangeAdptCmd( CServMsg& );    //�����������
    void  ProcStopAdptReq( CServMsg& );      //ֹͣ����
	void  ProcFastUpdatePicCmd(); //Mcu����MpuBas��ؼ�֡
    void  ProcTimerNeedIFrame();             //MpuBas����Mcu���ؼ�֡
	void  ProcModSndAddr();                  //�޸����緢�͵�ַ

	CMpu2BasAdpGroup* GetBasAdpGroup() const { return m_pAdpCroup; }
	TRtpPkQueue*	  GetRtpPkQueue() const { return m_ptFrmQueue; }

	void  AdjustParamForMpuSimulating(THDAdaptParam *paParam, u8 byOutPutNum);
	
private:
	// ----------- ���ܺ��� ------------
    BOOL32 ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode );            //�������Ӻ���
	void   Register( u8 byIdx, u32 dwMcuNode );               //��MCUע��
    void   SendMsgToMcu( u16 wEvent, CServMsg& cServMsg );
    void   SendChnNotif();                                                  //����ͨ��״̬��Mcu
    BOOL32 GetBasInitChnlNumAcd2WorkMode(u8 byWorkMode, u8 &byInitChnlNum); //����bas������ģʽ������ʼ�����ٸ�(��)ͨ��
	BOOL32 GetBasOutChnlNumAcd2WorkModeAndChnl(u8 byWorkMode, u8 byChnType, u8 &byOutChnlNum);   //����bas������ģʽ��ͨ���ţ���õ�ǰͨ�������ͨ����
	BOOL32 GetFrontOutNumAcd2WorckModeAndChnl(u8 byWorkMode, u8 byChnType, u8 byChnId,u8 &byOutFrontOutNum);   //����bas������ģʽ��ͨ���ţ����֮ǰ����ͨ���ĵ����ͨ�����ۺ�
    void   StatusShow( void );
	void   ClearInstStatus( void );                                     //��ձ�ʵ�������״̬��Ϣ
};

typedef zTemplate< CMpu2BasInst , MAXNUM_MPU2BAS_BASIC_CHANNEL*2, CMpu2BasData > CMpu2BasApp;
extern CMpu2BasApp g_cMpu2BasApp;


 
/************************************************************************/
/*                                                                      */
/*                          ����ΪCMpu2VmpInst                          */
/*                                                                      */
/************************************************************************/

class CMpu2VmpInst : public CInstance
{
	enum //ʵ��״̬
	{
		IDLE   = 0,
		NORMAL = 1,
		RUNNING = 2
	};

	enum eDaemonInstStatus
	{
		DAEMON_IDLE = 0,
		DAEMON_INIT = 1 ,
		DAEMON_NORMAL = 2
	};
	
	enum eOnLineState
	{
		OFFLINE = 0,
		ONLINE = 1
	};
	enum EnumUseState //��mcu���usestateֵ����һ��
	{ 
		USESTATE_IDLE,		//δ��ռ��

		USESTATE_START = 3,      //�����л���ϳ�
	};
public:
    CMpu2VmpInst() 
    {
        Clear();
	
    }
    virtual ~CMpu2VmpInst() 
    {
      Clear();
    }

private:
	u32           m_dwMcuIId;        // ��ʵ��ͨѶ��MCU.A ��ȫ��ID
    u32           m_dwMcuIIdB;       // ��ʵ��ͨѶ��MCU.B ��ȫ��ID
    u8            m_byRegAckNum;     // ע��ɹ�����
	u32           m_dwMpcSSrc;       // ҵ���ỰУ��ֵ
    
    CConfId       m_cConfId;		 // ��ǰ�����

	BOOL32        m_bCreateHardMulPic; //��ǰʵ������hardmulpic�Ƿ�ɹ�

    emenCodingForm  m_emMcuEncoding; //[4/9/2013 liaokang] ���뷽ʽ

public:
	CHardMulPic   m_cHardMulPic;   
protected:
	CKdvMediaRcv* m_pcMediaRcv[MAXNUM_MPU2VMP_MEMBER];   // 20������
	CKdvMediaSnd* m_pcMediaSnd[MAXNUM_MPU2VMP_CHANNEL];  // ���9·���� 
    
	//ģ��MPU��ʱ�õĽṹ
//	CKDVNewVMPParam   m_tMpuParam[MAXNUM_MPUSVMP_CHANNEL];	 // �ϳɲ���
//    CKDVNewVMPParam   m_tMpuParamPrevious[MAXNUM_MPUSVMP_CHANNEL];  // ��һ�κϳɲ���
	TVmpStyleCfgInfo  m_tStyleInfo;  // �ϳɷ�����	
    
	//MPU2 VMP�õĽṹ
	CKDVVMPOutMember   m_tParam[MAXNUM_MPU2VMP_CHANNEL];	 // �ϳɲ���
    CKDVVMPOutMember   m_tParamPrevious[MAXNUM_MPU2VMP_CHANNEL];  //��һ�κϳɲ���
	TMPU2CommonAttrb   m_tVmpCommonAttrb;
	TMPU2CommonAttrb   m_tPreviousVmpCommonAttrb;
    TVmpMbAlias        m_tVmpMbAlias[MAXNUM_MPU2VMP_MEMBER]; // Vmp�������� [7/3/2013 liaokang]

    u8            m_byHardStyle;     // ����ϳɷ��m_tParam[0].m_byVMPStyle��mcu�õĲ��ǵײ��õģ� 
	u32           m_dwMcuRcvIp;      // MCU���յ�ַ
    u16           m_wMcuRcvStartPort;// MCU������ʼ�˿ں�

	TEqpBasicCfg       m_tCfg;            // ��������
    TEqp          m_tEqp;            // ���豸
	TMpuVmpCfg    m_tMpuVmpCfg;      // �߼����ã���ȡ�ͱ���mcueqp.ini.ע����TMpuVmpCfg������TMpuDVmpCfg
	BOOL32        m_bAddVmpChannel[MAXNUM_MPU2VMP_MEMBER];// ��¼��ͨ���Ƿ��Ѿ����

	BOOL32		  m_bUpdateMediaEncrpyt[MAXNUM_MPU2VMP_CHANNEL];// ��¼����������Ϣ�Ƿ��Ѹ���


private:
	//-----------����״̬--------------                 // ���渴�����ķ�װ
	TPrsTimeSpan  m_tPrsTimeSpan;                       // �ش�ʱ�����
	u16			  m_wMTUSize;		                    // MTU�Ĵ�С
    u32           m_adwLastFUPTick[MAXNUM_MPU2VMP_CHANNEL];   // ��һ���յ��ؼ�֡�����Tick��
	u8            m_byMaxVmpOutNum; //vmp���ĺ������������mpu2 9·��ģ��mpuʱ4·/A��3·��A��Ӧ�ò����ˣ���
	u8            m_byValidVmpOutNum;//vmp��ǰʵ�ʱ����·��
    BOOL32		  m_abChnlIframeReqed[MAXNUM_MPU2VMP_MEMBER];//�ؼ�֡�����Ƿ��Ѿ��������������������ļ���ʱ
	BOOL32        m_bIsSimMpu;   //��ǰ�Ƿ���ģ��mpu
	BOOL32		  m_bIsDisplayMbAlias; //��ǰ����ϳ��Ƿ�Ҫ��ʾ�ն˱���
	CDrawBmp      m_cBmpDrawer;
	u32			  m_dwInitVmpMode;//��ʼ��ģʽVMP_BASIC����VMP_ENHANCED
	BOOL32        m_bFastUpdate; //��������ؼ�֡,���ڿ���������¼���ش���Ա�ĺϳ�
	
public:
	//------------��Ϣ��Ӧ--------------------
    void Clear();
	void DaemonInstanceEntry(CMessage *const pcMsg, CApp* pApp );//Daemon��Ϣ���
	void InstanceEntry( CMessage * const pcMsg ); //��Ϣ���
	void DaemonInit(CMessage* const pcMsg);
	void DaemonProcDisconnect( CMessage * const pMsg  );
	void Init( CMessage * const pcMsg );                             //Ӳ����ʼ��
	void MsgRegAckProc( CMessage * const pcMsg );                    //ȷ��
	void MsgRegNackProc( CMessage * const pcMsg );                   //�ܾ�

	void MsgStartVidMixProc( CMessage * const pcMsg );               //��ʼ����	
	void MsgChangeVidMixParamProc( CMessage * const pcMsg );         //�ı临�ϲ���  
	void MsgStopVidMixProc( CMessage * const pcMsg );                //ֹͣ����

	void MsgSetBitRate( CMessage * const pcMsg );
	void MsgModMcuRcvAddrProc( CMessage* const pMsg );
	
	void SendStatusChangeMsg( u8 byOnline, u8 byState/*, u8 byStyle */); //����״̬
    s32  SendMsgToMcu( u16 wEvent, CServMsg& cServMsg );  

	void MsgAddRemoveRcvChnnl(CMessage * const pMsg);//����ɾ��ĳ��������ͨ��

	void MpuNullParam2ValidParam(CKDVNewVMPParam& tInOutMpuParam,u8 byChnIdx);
	
	void MpuFrameRateCheck(CKDVNewVMPParam& tInOutMpuParam);

public:
	//-------------����------------------------
	void DaemonProcStatusShow();
	void DaemonProcParamShow();
    void DaemonProcConnectTimeOut( BOOL32 bIsConnectA );
	BOOL32 ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode );         //����MCU
	void Disconnect();                       //�Ͽ�����
    void ProcRegisterTimeOut( BOOL32 bIsRegisterA );
	void Register(/* BOOL32 bIsRegisterA, */u32 dwMcuNode );             //��MCUע��
	void StatusShow( void );                                         //��ʾ���е�״̬��ͳ����Ϣ
	BOOL StartNetRcvSnd(u8 byNeedPrs);                               //��ʼ�����շ�
    BOOL StopNetRcvSnd( void );                                      //ֹͣ�����շ�
    BOOL ChangeChnAndNetRcv(/*u8 byNeedPrs,*/ BOOL32 bStyleChged);       //���б��������պ�ͨ��
	BOOL ChangeVMPStyle();                                           //�ı�ϳɷ��
//	u8   ConvertVcStyle2HardStyle( u8 byVcStyle );                   //ҵ�񵽵ײ�ĺϳ�ģʽת��
    u8   ConvertHardStyle2VcStyle( u8 byHardStyle );                 //�ײ�ĺϳ�ģʽ��ҵ��ת��
	void ClearCurrentInst( void );                                   //��յ�ǰʵ��
	void ProcGetMsStatusRsp( CMessage * const pcMsg );               //ȡ��������״̬
	void ProcChangeMemAliasCmd(CMessage* const pcMsg);

	void ParamShow( void );							                 //��ʾҵ��ϳɲ���
    void MsgUpdateVmpEncryptParamProc( CMessage * const pMsg );
    void SetEncryptParam( u8 byChnNo, TMediaEncrypt *  ptVideoEncrypt, u8 byRealPT );
    void SetDecryptParam( u8 byChnNo, TMediaEncrypt *  ptVideoEncrypt/*, TDoublePayload * ptDoublePayload */);
    void SetPrs( u8 byNeedPrs );
	
    void InitMediaSnd( u8 byChnNo );
    BOOL32 InitMediaRcv( u8 byChnNo );  
    void StartMediaRcv( u8 byChnNo );
	BOOL SetMediaRcvNetParam( u8 byChnNo );

    void MsgTimerNeedIFrameProc(  CMessage * const pMsg );
    void MsgFastUpdatePicProc( CMessage * const pMsg );


    BOOL32 SetVmpAttachStyle( TVmpStyleCfgInfo&  tStyleInfo );
    void GetRGBColor( u32 dwColor, u8& byRColor, u8& byGColor, u8& byBColor );
    u8   VmpGetActivePayload(u8 byRealPayloadType);

    // ƽ������֧��
	//[2010/12/31 zhushz]ƽ�����ͽӿڷ�װ
	void SetSmoothSendRule( BOOL32 bSetFlag );
    
	void ProcSetFrameRateCmd( CMessage * const pcMsg );
	


protected:
    //[4/9/2013 liaokang] MCU���뷽ʽ    
    void    SetMcuEncoding(emenCodingForm emMcuEncoding);
    emenCodingForm GetMcuEncoding();

	//�߼�������صĳ��ýӿڷ�װ
	void ShowVMPDebugInfo();							//��ʾVMPdebug��Ϣ [nizhijun 2010/11/30]
	BOOL32 IsEnableBitrateCheat()const;
	u16   GetBitrateAfterCheated(u16 wOriginBitrate)
	{
		return m_tMpuVmpCfg.GetDecRateDebug(wOriginBitrate);
	}
	
	u8 GetSubTypeByWorkMode(u8 byWorkMode);
	s32 SetIdleChnShowMode(u8 byShowMode,BOOL32 bStart);
	BOOL32 SetMediaSndNetParam(u8 byChnNo);
	//��ǰ�Ƿ�ģ��mpu
	BOOL32 IsSimulateMpu()const
	{
		return m_bIsSimMpu;
	}
	//���õ�ǰģ��mpu
	void SetIsSimulateMpu(BOOL32 bIsSimMpu)
	{
		m_bIsSimMpu = bIsSimMpu;
	}

	
	//��ǰ����ϳ��Ƿ���Ҫ��ʾ����
	BOOL32 IsDisplayMbAlias()const
	{
		return m_bIsDisplayMbAlias;
	}
	//���õ�ǰ����ϳ��Ƿ���Ҫ��ʾ����
	void SetIsDisplayMbAlias(BOOL32 const& bIsDisplay)
	{
		m_bIsDisplayMbAlias = bIsDisplay;
	}
	//�����ն˱�����ʾ
	void DisplayMbAlias(u8 const byChnnl,const s8* pMemberAlias);
	


	u8 GetEncCifChnNum(); //��ȡ��ǰ��cif������ͨ��
	u8 GetEncOtherChnNum();//��ȡ��ǰ��other������ͨ��


	void TransFromMpuParam2EncParam(CKDVNewVMPParam* ptKdvVmpParam, TVideoEncParam* ptVideEncParam);

	void TransFromMpu2Param2EncParam(CKDVVMPOutMember* ptKdvVmpPamam,TVideoEncParam* ptVideEncParam);

	s32  SetNoStreamBakByBmpPath(u32 dwBakType,const s8* pBmpPath,u32 dwChnlId = MULPIC_MAX_CHNS);

    // ����Vmp���������Ϣ [7/3/2013 liaokang]
    void UnpackVmpWholeOperMsg(CServMsg & cServMsg, BOOL32 bStart);

};


class TMpu2VmpRcvCB
{
public:
    CHardMulPic*  m_pHardMulPic;
    u8             m_byChnnlId; 
};

class CMpu2VmpCfg
{
public:
    CMpu2VmpCfg() 
	{
		m_byVmpEqpNum = 0;
		m_dwMcuNode = 0;
		m_dwMcuNodeB = 0;
		m_byWorkMode = 0;
		memset(m_atMpu2VmpRcvCB,0,sizeof(TMpu2VmpRcvCB));
	}
    virtual ~CMpu2VmpCfg() {}

public:
    u8            m_byWorkMode;        // ����ʹ��ȫ�ֱ���,�ڴ˱���WorkMode
	u32           m_dwMcuNode;       // ���ӵ�MCU.A �ڵ��
    u32           m_dwMcuNodeB;      // ���ӵ�MCU.B �ڵ��
	u8            m_byVmpEqpNum;     //���õ�vmp�������
	
    TMpu2VmpRcvCB    m_atMpu2VmpRcvCB[MAXNUM_MPU2_EQPNUM][MAXNUM_MPU2VMP_MEMBER];  
};

typedef zTemplate< CMpu2VmpInst , MAXNUM_MPU2_EQPNUM, CMpu2VmpCfg > CMpu2VmpApp;
extern CMpu2VmpApp g_cMpu2VmpApp;

extern TResBrLimitTable g_tResBrLimitTable;


#endif // !_MPUINST_H_

//END OF FILE


