/*****************************************************************************
  ģ����      : libeapu.a
  �ļ���      : eapuinst.h
  ����ļ�    : kdvmixer_eapu.h
  �ļ�ʵ�ֹ���: 
  ����        : ��־��
  �汾        : V1.0  Copyright(C) 2009-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
2010/01/20  1.0         ��־��        ����
******************************************************************************/

#include "osp.h"
#include "kdvmedianet.h"   //����֧��
#include "eqpcfg.h"        //����֧��
#include "mcustruct.h"     //CServer��֧��
#include "mcuver.h"
#include "kdvmixer_eapu.h"
#include "eveapumixer.h"
#include "boardagent.h"
#define MIXER_EAPU_CHN_MIXNUM           (u16)2
#define FIRST_REGACK                    (u8 )1              // ��һ���յ�ע��Ack
#define MIX_CONNETC_TIMEOUT             (u16)(3 * 1000)     // ���ӳ�ʱֵ3s
#define MIX_REGISTER_TIMEOUT            (u16)(4 * 1000)     // ע�ᳬʱֵ4s
#define MAX_AUDIO_FRAME_SIZE            (u32)8 * 1024       //���ܴ�С
#define BRD_EAPU_SNDPORT                (u16)10000          //
#define ERR_MIXER_NONE               ( ERR_MIXER_BGN  )     //!�޴��� 
#define ERR_MIXER_GRPNOTEXIST        ( ERR_MIXER_BGN + 1 )  //!ָ���Ļ����鲻����
#define ERR_MIXER_MMBNOTEXIST        ( ERR_MIXER_BGN + 2 )  //!ָ���Ļ�����Ա������
#define ERR_MIXER_NOFREEGRP          ( ERR_MIXER_BGN + 3 )  //!û�п��еĻ�����
#define ERR_MIXER_NOFREEMMB          ( ERR_MIXER_BGN + 4 )  //!û�п��еĻ���ͨ��
#define ERR_MIXER_ILLEGALBODY        ( ERR_MIXER_BGN + 5 )  //!��Ϣ�е���Ϣ��Ƿ�
#define ERR_MIXER_CALLDEVICE         ( ERR_MIXER_BGN + 6 )  //!���õײ㺯��ʧ��
#define ERR_MIXER_MIXING             ( ERR_MIXER_BGN + 7 )  //!���ڻ���״̬
#define ERR_MIXER_NOTMIXING          ( ERR_MIXER_BGN + 8 )  //!δ����
#define ERR_MIXER_ERRCONFID          ( ERR_MIXER_BGN + 9 )  //!��Ч�Ļ���ID
#define ERR_MIXER_GRPINVALID         ( ERR_MIXER_BGN + 10 ) //��Ч�Ļ�����ID
#define ERR_MIXER_CHNINVALID         ( ERR_MIXER_BGN + 11 ) //��Ч�Ļ�����ID
#define ERR_MIXER_STATUIDLE          ( ERR_MIXER_BGN + 12 )

#define ERR_MIXER_MIXMODEAUTOMIX     ( ERR_MIXER_BGN + 13 )  //!�������ܻ���ģʽ
#define ERR_MIXER_MIXMODESPECMIX     ( ERR_MIXER_BGN + 14 )  //!�����ƻ���ģʽ
#define ERR_MIXER_MIXMODEVAC         ( ERR_MIXER_BGN + 15 )  //!������������ģʽ
#define ERR_MIXER_MIXMODEAUTOANDVAL  ( ERR_MIXER_BGN + 16 )  //!���������������������ͬʱ����ģʽ
#define MAXVALUE_EMIXER_VOLUME       (u8)31          //EAPU�������������ֵ
//��ӡ������
#define MIXLOG_LEVER_CRIT                (u8)0       //�ؼ���/����/�쳣
#define MIXLOG_LEVER_WARN                (u8)1       //����
#define MIXLOG_LEVER_INFO                (u8)2       //����
#define MIXLOG_LEVER_RCVDATA             (u8)3       //��������
#define MIXLOG_LEVER_SNDDATA             (u8)4       //��������
#define MIXLOG_LEVER_DETAIL              (u8)5       //����ϸ��/��ϸ���Ե�

//R3FULLƽ̨֧��EAPUģ��APU
#define DEVVER_MIXER_R3FULL              (u16)4301
class CEapuMixInst;
class CRcvCallBack
{
public:
    CEapuMixInst* m_pcMixerInst;    //�ص��Ļ�����
    u32           m_byChnnlId;      //�ص���ͨ���� 
	CRcvCallBack()
	{
		m_pcMixerInst = NULL;
		m_byChnnlId = 0;
	}
};
struct CRegAckInfo
{
public:
    u32     m_dwMcuIId;             //ͨ��MCU.A ʵ��ID
    u32     m_dwMcuIIdB;            //ͨ��MCU.B ʵ��ID
    u8      m_byRegAckNum;          //��һ���յ�ע��ACK
    u32     m_dwMpcSSrc;            //ҵ���ỰУ��ֵ
    TPrsTimeSpan m_tPrsTimeSpan;
/*    void Reset()
    {
        m_dwMcuIId       = INVALID_INS;
        m_dwMcuIIdB      = INVALID_INS;
        m_dwMpcSSrc      = 0;
    }*/
    CRegAckInfo()
    {
        m_dwMcuIId       = INVALID_INS;
        m_dwMcuIIdB      = INVALID_INS;
        m_dwMpcSSrc      = 0;
        m_byRegAckNum    = 0;
    }
    void print()
    {
        OspPrintf(TRUE,FALSE,"RegAckInfo:McuIId[%d]McuIIdB[%d]RegAckNum[%d]MpcSSrc[%d]",
			m_dwMcuIId,m_dwMcuIIdB,m_byRegAckNum,m_dwMpcSSrc);
    }
};
class CEapuMixInst : public CInstance
{
public:
    CEapuMixInst();
    ~CEapuMixInst();
    CKdvMixer m_cMixer;                                          //���������󣬵ײ��ṩ
    //��Ϣ���
    void DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp );
    void InstanceEntry( CMessage * const pcMsg );
    CKdvMediaSnd* GetMediaSndPtr(u32 dwChID, BOOL32 bDMode);         //���ĳͨ����Ӧ�ķ��Ͷ���
    u8     GetAudioMode(u8 byAudioType , BOOL32 bIsSingleChn = FALSE);
    //��Ϣ������
private:
    //��������ʼ��,��Ϣ��ΪTEapuMixerCfg
    void DaemonProcInitalMsg(CMessage* const pMsg);     //�����ʼ����Ϣ(Daemon)
    void DaemonProcGetMsStatusRsp( CMessage * const pcMsg); // ����ȡ��������״̬
    void ProcGetMsStatusRsp( CMessage * const pcMsg); // ����ȡ��������״̬
    void MsgProcInitalMsg(CMessage* const pMsg);                     //���ɻ�����
    void MsgAddMixMemberProc(CMessage* const pMsg);                  //�����������ӻ�����Ա�������ӻ���ͨ��
    void MsgRemoveMixMemberPorc(CMessage *const pMsg);               //�ӻ��������Ƴ�������Ա�����������ͨ��
    void MsgStartMixProc(CMessage* const pMsg);                      //����������������Ϣ
    void MsgStopMixProc(CMessage* const pMsg);                       //����ֹͣ��������Ϣ
    void MsgForceActiveProc( CMessage * const pcMsg );               // ǿ�ƻ���
    void MsgCancelForceActiveProc( CMessage * const pcMsg );         // ȡ��ǿ�ƻ���
    void MsgSetMixDepthProc(CMessage *const pMsg);                   //���û��������
	void MsgStartVacProc(CMessage *const pMsg);
	void MsgStopVacProc(CMessage *const pMsg);
    void MsgSetVolumeProc(CMessage* const pMsg);                     //���û�����Ա����
    void MsgSetVacKeepTimeProc( CMessage * const pcMsg );            // ������������
    void ProcConnectMcuTimeOut( BOOL32 bIsConnectA );                // �����������
    void ProcRegisterTimeOut( BOOL32 bIsRegisterA );                 // ����ע�����
    void ProcSetQosInfo(CMessage* const pMsg);
    void DaemonMsgDisconnectProc( CMessage * const pcMsg);           // ����ʵ���������
    void MsgDisconnectProc();                 // ��ͨʵ���������
    void MsgRegAckProc( CMessage * const pcMsg);                     // ����ע��ɹ�
    void MsgRegNackProc( CMessage * const pcMsg );                   // ����ע��ʧ��
    void MsgActiveMmbChangeProc(CMessage * const pMsg);
    void MsgSetMixSendProc( CMessage * const pcMsg );
	void MsgModifySendAddr(CMessage * const pcMsg);


    void   StopAllTask();
    void   SendGrpNotif();
    BOOL32 ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode );         // ����Mcu
    void   Register(u32& dwMcuNode );          // ��MCUע��
    BOOL32 SendMsgToMcu( u16 wEvent, CServMsg const &cServMsg );    //������MCU������Ϣ
    BOOL32 SendMsgToMcuA( u16 wEvent, CServMsg const &cServMsg);    //������MCU������Ϣ
    BOOL32 SendMsgToMcuB( u16 wEvent, CServMsg const &cServMsg );    //������MCU������Ϣ
    void   ComplexQos(u8& byValue, u8 byPrior);
    u16    SetRcvObjectParaAndStartRcv(u8 byChnIdx);
    u16    SetSndObjectPara(u32 dwDstIp,u16 wDstPort,u8 byChnIdx);
    u16    FreeAllRcv();
    void   ShowAllMixGroupStatus( CApp* pcApp );              // ��ʾ����״̬
    void   ShowGrpStatus();
    BOOL32 IsSupportAudiotype(u8 byAudioType);
    u8   GetMtId2ChnIdx( u8 byMtId, BOOL32 bAddMt = TRUE );

private:
    //DAEMON ʵ��״̬
    enum ESTATE
    {
        IDLE = 0 ,
        NORMAL,
    };
    u8               m_byMixMemNum;
    u8               m_byRcvOff;                             //����ƫ��
    u8               m_bySndOff;                             //����ƫ��
    BOOL32           m_bIsDMode;                             //�û������Ƿ�Ϊ���ʽ������
    CConfId          m_cConfId;                              //����������Ļ���ID
    TMixerGrpStatus  m_tGrpStatus;                           //������״̬
    u8               m_byAudType[2];                         //��¼�û���ʹ�õ�������Ƶ��ʽ
    u8               m_abyMtId2ChnIdx[MAXNUM_MIXER_CHNNL];//
    BOOL32           m_bNeedPrs;
    TMediaEncrypt    m_tMediaEncrypt;
    TDoublePayload   m_tDoublePayload;
    TCapSupportEx    m_tCapSupportEx;
    BOOL32           m_bIsSendRedundancy;    
	BOOL32           m_bIsStartVac;
	BOOL32           m_bIsStartMix;
    TRSParam         m_tNetRSParam;                                                 //���ܶ������ش�����
	CRegAckInfo      m_tRegAckInfo;
    u32              m_dwMcuRcvIp;           //MCU.A ���յ�ַ
    u32              m_dwMcuRcvIpB;          //MCU.B ���յ�ַ
    u16              m_wMcuRcvStartPort;     //MCU.A ������ʼ�˿ں�
    u16              m_wMcuRcvStartPortB;    //MCU.B ������ʼ�˿ں�
	TEapuMixerCfg    m_tEapuMixerCfg;
};
class CEapuCfg
{
public:
    CEapuCfg();
    virtual ~CEapuCfg();
public:
    u32              m_dwMcuNode;
    u32              m_dwMcuNodeB;
    BOOL32           m_bEmbed;               //�Ƿ񱾻�MCU.A
    BOOL32           m_bEmbedB;              //�Ƿ���Ƕ��B����MCU.B
    TEapuCfg         m_tEapuCfg;                    //MIXER��������Ϣ
    CRcvCallBack     m_acRcvCB[MAXNUM_MIXER_CHNNL];                          //���ջص�
    CKdvMediaRcv     m_acAudRcv[MAXNUM_MIXER_CHNNL];                             //ͨ����Ӧý���������
    BOOL32           m_bIsChnUsed[MAXNUM_MIXER_CHNNL];                           //��ͨ���Ƿ�ʹ��
    CKdvMediaSnd     m_acAudSnd[MAXNUM_MIXER_CHNNL + 2 * MAXNUM_MIXER_AUDTYPE];   //ͨ����Ӧý�巢������
};
typedef zTemplate< CEapuMixInst, MAXNUM_EAPU_MIXER, CEapuCfg > CEapuMixerApp;
extern CEapuMixerApp g_cEapuMixerApp;
