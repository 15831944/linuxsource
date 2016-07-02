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
#include "mcustruct.h"
#include "eqpcfg.h"
#include "mmpcommon.h"
#if defined(_8KH_) || defined(_8KE_)
#include "audiomixer.h"
#else
#include "audiomixer_gpu.h"
#endif
#include "commonlib.h"
#include "mcuinnerstruct.h"

#if( MAXNUM_MIXER_DEPTH < MAX_MIXER_DEPTH  )
#error( "MAXNUM_MIXER_DEPTH > MAX_MIXER_DEPTH" )
#endif
#include "kdvlog.h"
#include "loguserdef.h"

#define FIRST_REGACK                    (u8 )1           // ��һ���յ�ע��Ack
#define MIX_CONNETC_TIMEOUT             (u16)(3 * 1000)  // ���ӳ�ʱֵ3s
#define MIX_REGISTER_TIMEOUT            (u16)(4 * 1000)  // ע�ᳬʱֵ4s

#if defined(_8KE_)
#define MAXNUM_8000X_MIXER              (u8)1
#elif defined(_8KH_)
#define MAXNUM_8000X_MIXER              (u8)2
#else
#define MAXNUM_8000X_MIXER              (u8)5
#endif

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
	//�ϵ紦��
	void DaemonProcPowerOnMsg(CMessage * const pcMsg); // �����ʼ��
	//������ʱ
	void   DaemonProcConnetTimerOut();
	//ע�ᳬʱ
	void   DaemonProcRegTimerOut();
	//ע��ɹ�
	void   DaemonProcRegAck(CMessage * const pcMsg, CApp* pcApp);
	//ע��ʧ��
	void   DaemonProcRegNack();
	//��������
	void   DaemonProcOspDisconnect();
	void   DaemonProcMixerStatus();                    // ��ʾ������״̬��Ϣ
	void   ProcMsgMixerInit(CMessage * const pcMsg);
    void ProcRegisterTimeOut( void );                                // ����ע�����
    void MsgDisconnectProc();                                        // ��ͨʵ���������
    void MsgRegAckProc( CMessage * const pcMsg);                     // ����ע��ɹ�
    void MsgRegNackProc( CMessage * const pcMsg );                   // ����ע��ʧ��

    // << ����ͨ��ʵ���е��ú���>>
    void MsgAddMixMemberProc( CMessage * const pcMsg );
    void MsgRemoveMixMemberPorc( CMessage * const pcMsg );

    void MsgStartMixProc( CMessage * const pcMsg );           // ����ʼ������Ϣ
    void MsgStopMixProc( CMessage * const pcMsg );            // ����ֹͣ������Ϣ
    void MsgSetVolumeProc();          // ��������
    void MsgChangeMixDelay( CMessage * const pcMsg );         // �޸Ļ�����ʱ
    void MsgForceActiveProc( CMessage * const pcMsg );        // ǿ�ƻ���
    void MsgCancelForceActiveProc( CMessage * const pcMsg );  // ȡ��ǿ�ƻ���
    void MsgActiveMmbChangeProc( CMessage * const pcMsg );    // �����Ա�仯
    void MsgSetMixDepthProc();        // ���û������
    void MsgSetMixChannelProc( CMessage * const pcMsg );      // ���û���ͨ��
    void MsgSetMixSendProc( CMessage * const pcMsg );         // ���û������Ƿ�������
    void MsgSetVacKeepTimeProc( CMessage * const pcMsg );     // ������������
    void MsgGroupShowProc();                                  // ��ʾ�������ͨ����Ϣ

	///////////////////////////////////////////////////////////
	////////// ���ܺ���
	///////////////////////////////////////////////////////////
    BOOL32 DestroyeMixGroup();                                // ���������ͷŵ�
	BOOL32 ConnectMcu( u32& dwMcuNode );                      // ����Mcu
	void   Register();                        // ��MCUע��
    BOOL32 StartMix( TMediaEncrypt * ptMediaEncrypt, 
                     TDoublePayload * ptDoublePayload,u8 byNChlNum = 0,TAudioCapInfo *ptAudioCapInfo = NULL );      // ��ʼ����
    BOOL32 StopMix();                                         // ֹͣ����
	void   SendGrpNotif();                                    // ���ͻ�����״̬
    BOOL32 SendMsgToMcu( u16 wEvent, CServMsg const &cServMsg );
    void   SetEncryptParam( TMediaEncrypt * ptMediaEncrypt, TDoublePayload * ptDoublePayload,u8 byNModeChlNum,TAudioCapInfo *ptAudioCapInfo );

    // xsl [7/29/2006] ����ͨ����mtid��ӳ�䴦����
    void SetMtIdPosInChnIdx( u8 byMtId, u8 byChanIdx, BOOL32 bRemove = FALSE );
	void ClearMapTable( void );
#if !defined(_8KH_) && !defined(_8KE_)
	u8 GetAudioMode(const u8 byAudioType, u8 byAudioTrackNum);
#endif
    
protected:  
private:
    CAudioMixer      *m_pcAMixer;            //������ʵ��
    CConfId          m_cConfId;              //���������Ļ���ID
    TMixerGrpStatus  m_tGrpStatus;           //������״̬
    BOOL32           m_bNeedPrs;
    u8		         m_abyMtId2ChnIdx[MAXNUM_MIXER_CHNNL + 1];
	u32              m_dwMcuIId;             //ͨ��MCU.A ʵ��ID
    u8               m_byRegAckNum;          //��һ���յ�ע��ACK
	u16		         m_wMTUSize;			 //MTU size, zgc, 2007-04-02
	BOOL32           m_bIsSendRedundancy;	 //�Ƿ����෢��, zgc, 2007-07-25
    TNetRSParam      m_tNetRSParam;
	T8keAudioMixerCfg m_tMixerCfg;            //MIXER��������Ϣ
	u32              m_dwMcuNode;
};


typedef zTemplate< CAudMixInst, MAXNUM_8000X_MIXER > CMixerApp;
extern CMixerApp g_cMixerApp;

#endif  //_AUDIO_MIXER_INSTANCE_H_
