/*****************************************************************************
   ģ����      : MpwLib�໭�渴�ϵ���ǽ
   �ļ���      : mpwinst.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ������Ϣ��
   ����        : john
   �汾        : V4.0  Copyright( C) 2001-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2005/09/25  1.0         john        ����
   2003/03/21  4.0         �ű���      �����Ż�
******************************************************************************/
#ifndef _VIDEO_MPWINST_H_
#define _VIDEO_MPWINST_H_

#include "evmcueqp.h"
#include "mpwcfg.h"
#include "eqpcfg.h"

#define MPW_ID              (u8)9 // Mpw ID

class CMpwInst : public CInstance
{
	enum //ʵ��״̬
	{
		IDLE   = 0,
		NORMAL = 1,
		RUNNING = 2
	};
public:
	CMpwInst(  );
	virtual ~CMpwInst(  );

private:
	BOOL32        m_bDbVid;         // �Ƿ�˫��Ƶ	
    TNetAddress   m_tRtcpAddr;      // Rtcp ��ַ
	CKDVVMPParam  m_tParam;
    CConfId       m_cConfId;
    
	// ����״̬
	CHardMulPic   m_cHardMulPic;    // ���渴�����ķ�װ
    BOOL32        m_bAddVmpChannel[MAXNUM_VMP_MEMBER];
    u32           m_dwLastFUPTick;  //����յ��ؼ�֡�����Tick��
    TNetAddress   m_tSndAddr[2];    // �ϳɺ������ַ
    TCapSupportEx m_tCapSupportEx;

private:
	// ��Ϣ��Ӧ
	void InstanceEntry( CMessage *const pcMsg );                // ��Ϣ���    
	void MsgRegAckProc( CMessage* const pcMsg );                // ȷ��
	void MsgRegNackProc( CMessage* const pcMsg );               // �ܾ�
	void MsgStartVidMixProc( CMessage* const pcMsg );           // ��ʼ����
	void MsgStopVidMixProc( CMessage* const pcMsg );            // ֹͣ����
	void MsgChangeMixParamProc( CMessage* const pcMsg );        // �ı临�ϲ���
	void MsgGetMixParam( CMessage* const pcMsg );               // ��ѯ���ϲ���
	void MsgSetBitRate( CMessage* const pcMsg );
	void MsgGetMpwData( CMessage* const pcMsg );
    void MsgNeedIFrameProc( void );                             // �Ƿ���Ҫ�ؼ�֡
    void MsgFastUpdatePic( void );                              // ��ʱ��һ���ؼ�֡

private:
	// ���ܴ���
    void Init( CMessage* const pcMsg );                         // Ӳ����ʼ��
    void ProcConnectTimeOut( BOOL32 bIsConnectA );
    void ProcRegisterTimeOut( BOOL32 bIsRegisterA );
	void ProcDisconnect( CMessage *const pcMsg );               // �Ͽ�����
	void ProcGetMsStatusRsp( CMessage* const pcMsg );           // ȡ��������״̬

	BOOL32 ConnectToMcu( BOOL32 bIsConnectA, u32& dwMcuNode );  // ����MCU
	void   RegisterToMcu( BOOL32 bIsRegisterA, u32 dwMcuNode ); // ��MCUע��
	void   StopAndClearInstStatus( void );                      // ֹͣ�����ʵ��״̬
	BOOL32 StartHardwareMix( CKDVVMPParam& tParam );            // ��ʼӲ���ϳ�
	BOOL32 AdjustMpwStyle( CKDVVMPParam& tParam );              // �ı�ϳɷ��
	BOOL32 ChangeMpwChannel( CKDVVMPParam& tParam );            // �ı��߼�ͨ��

	u8   ConvertVc2Hard(  u8 byOldstyle  );                     // ҵ�񵽵ײ�ĺϳ�ģʽת��
	u8   GetFavirateStyle( u32 dwTotalmap, u8 &byChannel );     // ����MAP��Ŀ�������ϳ�����
    u8   GetMpwNumOfStyle( u8 byMpwStyle );                     // �ɺϳɷ��ȡ������
    u8   GetActivePayload( u8 byRealPayloadType );              // ȡ��̬�غ�ֵ

    void UpdataEncryptParam( CMessage * const pcMsg );          // �޸ļ��ܲ���
    void SetEncryptParam( u8 byChnNo, TMediaEncrypt* ptVideoEncrypt, TDoublePayload* ptDoublePayload );
    void SetDecryptParam( u8 byChnNo, TMediaEncrypt* ptVideoEncrypt, TDoublePayload* ptDoublePayload );
    
    void MpwStatusShow( void );                                 // ��ʾ���е�״̬��ͳ����Ϣ
    void SendStatusMsg( u8 online, u8 state, u8 style );        // ����״̬
	void ProcSetQosInfo( CMessage* const pcMsg );               // ����Qos��Ϣ
    void ComplexQos( u8& byValue, u8 byPrior );                                    
    void mpwlog( s8* pszFmt, ...);
	BOOL32 SendMsgToMcu( u16  wEvent, CServMsg* const pcServMsg ); // ������Ϣ��MCU
    
    u16 GetSendRtcpPort(u8 byChanNo);
};

class CMpwConfig
{
public:
    CMpwConfig();
    ~CMpwConfig();
public:
    u8            m_byChannelnum;   // ͨ���� 
    CMpwCfg       m_tCfg;           // ���ò���
    u32           m_dwMcuNode;      // MCU.A �Ľڵ��
    u32           m_dwMcuNodeB;     // MCU.B �Ľڵ��
    u32           m_dwMcuIId;       // ��ʵ��ͨѶ��MCU.A ��ȫ��ID
    u32           m_dwMcuIIdB;      // ��ʵ��ͨѶ��MCU.B ��ȫ��ID
    BOOL32        m_bEmbed;         // ��Ƕ��A�� 
    BOOL32        m_bEmbedB;        // ��Ƕ��B��
    u8            m_byRegAckNum;    // Reg Ack num
	u32           m_dwMpcSSrc;      // guzh [6/12/2007] ҵ���ỰУ��ֵ 
public:
    void FreeStatusDataA();
    void FreeStatusDataB();
};

typedef zTemplate< CMpwInst, 1, CMpwConfig > CMpwApp;
extern CMpwApp g_cMpwApp;

#endif 
