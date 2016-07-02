/*****************************************************************************
   ģ����      : ������
   �ļ���      : MixerInst.h
   ����ʱ��    : 2003�� 12�� 9��
   ʵ�ֹ���    : 
   ����        : ������
   �汾        : 
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   03/12/09    3.0         zmy         ����
******************************************************************************/
#ifndef _AUDIO_MIXER_INSTANCE_H_
#define _AUDIO_MIXER_INSTANCE_H_

#include "osp.h"
#include "kdvsys.h"
#include "kdvmixer.h"
#include "mcustruct.h"
#include "eqpcfg.h"

/*
#if( MAXNUM_MIXER_DEPTH < MAX_MIXER_DEPTH  )
#error( "MAXNUM_MIXER_DEPTH > MAX_MIXER_DEPTH" )
#endif*/

#define FIRST_REGACK                    (u8 )1           // ��һ���յ�ע��Ack
#define MIX_CONNETC_TIMEOUT             (u16)(3 * 1000)  // ���ӳ�ʱֵ3s
#define MIX_REGISTER_TIMEOUT            (u16)(4 * 1000)  // ע�ᳬʱֵ4s
#define MIXER_PRI                       (u16)60
#define MIXER_LOCALSTARTPORT            (u16)10000
#define MIXER_LOCALRCVSTARTPORT         (u16)20000

class CAudMixInst : public CInstance
{
public:
    CAudMixInst();
    ~CAudMixInst();
    
	//////////////////////////////////////////////////////////////////////////
	///////   ��Ϣ������
	//////////////////////////////////////////////////////////////////////////
	
    //	<< ��Ϣ��� >>
    void InstanceEntry( CMessage * const pcMsg );
    void DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp );

    // << ��DAEMON ʵ���е��ú��� >>
	void DaemonProcInitalMsg( CMessage * const pcMsg, CApp* pcApp ); // �����ʼ��(Daemon)
    void ProcConnectMcuTimeOut( BOOL32 bIsConnectA );                // �����������
    void ProcRegisterTimeOut( BOOL32 bIsRegisterA );                 // ����ע�����
    void MsgMixerQuit( CMessage * const pcMsg );                     // ��������˳�
    void MsgDisconnectProc( CMessage * const pcMsg, CApp* pcApp );   // �������
    void MsgRegAckProc( CMessage * const pcMsg, CApp* pcApp );       // ����ע��ɹ�
    void MsgRegNackProc( CMessage * const pcMsg );                   // ����ע��ʧ��
    void DaemonProcGetMsStatusRsp( CMessage * const pcMsg, CApp* pcApp ); // ����ȡ��������״̬
	//test, zgc, 2007/04/24
	void DaemonProcStopEqpCmd(CApp* pcApp ); //ֹͣ��������MAP
	void DaemonProcStartEqpCmd( CMessage * const pcMsg, CApp* pcApp ); //������������MAP
	
	void DaemonProcReconnectBCmd( CMessage * const pcMsg); // Mcu ֪ͨ�������� MPCB ��, zgc, 2007/04/30
	void DeamonProcModSendIpAddr( CMessage* const pMsg );

    // << ����ͨ��ʵ���е��ú���>>
    void MsgAddMixMemberProc( CMessage * const pcMsg );
    void MsgRemoveMixMemberPorc( CMessage * const pcMsg );

    void MsgStartMixProc( CMessage * const pcMsg );           // ����ʼ������Ϣ
    void MsgStopMixProc( CMessage * const pcMsg );            // ����ֹͣ������Ϣ
    void MsgSetVolumeProc( CMessage * const pcMsg );          // ��������
    void MsgChangeMixDelay( CMessage * const pcMsg );         // �޸Ļ�����ʱ
    void MsgForceActiveProc( CMessage * const pcMsg );        // ǿ�ƻ���
    void MsgCancelForceActiveProc( CMessage * const pcMsg );  // ȡ��ǿ�ƻ���
    void MsgActiveMmbChangeProc( CMessage * const pcMsg );
    void MsgSetMixDepthProc( CMessage * const pcMsg );        // ���û������
    void MsgSetMixChannelProc();      // ���û���ͨ��
    void MsgSetMixSendProc( CMessage * const pcMsg );
    void MsgSetVacKeepTimeProc( CMessage * const pcMsg );     // ������������
    
    void MsgGroupShowProc();                                  // ��ʾ�������ͨ����Ϣ
    void MsgGrpChnShowProc();                                 // ��ʾ����Ļ���ͨ�����ն˶�Ӧ��Ϣ

	///////////////////////////////////////////////////////////
	////////// ���ܺ���
	///////////////////////////////////////////////////////////
	BOOL32 Init(CApp* pcApp );	      // ��ʼ��
    BOOL32 InitMixGroup();                                    // ��ʼ��������
    BOOL32 DestroyeMixGroup();                                // ���������ͷŵ�
	BOOL32 ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode );  // ����Mcu
	void   Register( BOOL32 bIsRegisterA, u32& dwMcuNode );   // ��MCUע��
    BOOL32 StartMix( u8 byAudType, u8 byDepth, u8 byAll,      // ��ʼ����
                     TMediaEncrypt * ptMediaEncrypt, 
                     TDoublePayload * ptDoublePayload, u8 byFECMode = FECTYPE_NONE );
    BOOL32 StopMix();                                         // ֹͣ����
	void   SendGrpNotif();                                    // ���ͻ�����״̬
    BOOL32 SendMsgToMcu( u16 wEvent, CServMsg const &cServMsg );
    BOOL32 SendMsgToMcuA( u16 wEvent, CServMsg const &cServMsg );
    BOOL32 SendMsgToMcuB( u16 wEvent, CServMsg const &cServMsg );
    void   SetEncryptParam( TMediaEncrypt * ptMediaEncrypt, TDoublePayload * ptDoublePayload );
    void   ProcSetQosInfo( CMessage * const pcMsg );          // ����Qos��Ϣ
    void   ComplexQos( u8& byValue, u8 byPrior );             // �󸴺�Qosֵ
	void   ShowAllMixGroupStatus( CApp* pcApp );              // ��ʾ����״̬
    void   ShowAllMixGrpChnStatus( CApp* pcApp );             // ��ʾͨ��״̬
	void   StopAllMixGroup( CApp* pcApp );                    // ֹͣ���л�����
	BOOL32 ReadConfigFileApu( u32& dwStartRcvBufs, u32& dwFastRcvBufs );

    // xsl [7/29/2006] ����ͨ����mtid��ӳ�䴦����
    void SetMtIdPosInChnIdx( u8 byMtId, u8 byChanIdx, BOOL32 bRemove = FALSE );
	void ClearMapTable( void );
	
	void ProcModSndAddr();
    
protected:
    //DAEMON ʵ��״̬
    enum ESTATE
    {
        IDLE,
        NORMAL,
    };
    
protected:
    s32              m_nMaxMmbNum;          //�û�����������������ͨ����
    CKdvMixerGroup   m_cMixGrp;             //������ʵ��
    CConfId          m_cConfId;             //���������Ļ���ID
    u16              m_wLocalRcvPort;       //���ؽ�����ʼ�˿ں�
    TMixerGrpStatus  m_tGrpStatus;          //������״̬
    BOOL32           m_bNeedPrs;
    u8		         m_abyMtId2ChnIdx[MAX_CHANNEL_NUM+1];
};

class CMixerCfg
{
public:
    CMixerCfg();
    ~CMixerCfg();
public:
    u32     m_dwMcuIId;             //ͨ��MCU.A ʵ��ID
    u32     m_dwMcuIIdB;            //ͨ��MCU.B ʵ��ID
    u32     m_dwMcuNode;            //��MCU.A ͨ�Žڵ��
    u32     m_dwMcuNodeB;           //��MCU.B ͨ�Žڵ��
    u32     m_dwMcuRcvIp;           //MCU.A ���յ�ַ
    u32     m_dwMcuRcvIpB;          //MCU.B ���յ�ַ
    u16     m_wMcuRcvStartPort;     //MCU.A ������ʼ�˿ں�
    u16     m_wMcuRcvStartPortB;    //MCU.B ������ʼ�˿ں�
    BOOL32  m_bEmbed;               //�Ƿ񱾻�MCU.A
    BOOL32  m_bEmbedB;              //�Ƿ���Ƕ��B����MCU.B
    u8      m_byRegAckNum;          //��һ���յ�ע��ACK
	u16		m_wMTUSize;				//MTU size, zgc, 2007-04-02
	BOOL32  m_bIsSendRedundancy;	//�Ƿ����෢��, zgc, 2007-07-25

	u32     m_dwMpcSSrc;            // guzh [6/12/2007] ҵ���ỰУ��ֵ
    
    TAudioMixerCfg m_tMixerCfg;     //MIXER��������Ϣ
    TNetRSParam    m_tNetRSParam;

    
public:
    void FreeStatusDataA( void );
    void FreeStatusDataB( void );	
};

typedef zTemplate< CAudMixInst, MAXNUM_MIXER_GROUP, CMixerCfg > CMixerApp;
extern CMixerApp g_cMixerApp;

#endif  //_AUDIO_MIXER_INSTANCE_H_
