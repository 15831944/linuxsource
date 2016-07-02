/*****************************************************************************
   ģ����      : ����������
   �ļ���      : basinst.h
   ����ʱ��    : 2003�� 6�� 25��
   ʵ�ֹ���    : ����ʵ��ͷ�ļ�
   ����        : ������
   �汾        : 
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2006/03/21  4.0         �ű���      �����Ż�
******************************************************************************/
#ifndef _BASINST_H_
#define _BASINST_H_

#include "osp.h"
#include "mcuconst.h"
#include "basstruct.h"
#include "mcustruct.h"
#include "kdvadapter.h"
#include "eqpcfg.h"
#ifdef _LINUX_
#include "libsswrap.h"
#endif

const u16 EnVidFmt[6][12]=
{
    //����               ѹ�����ʿ��Ʋ��� ���ؼ�֡ ������� ��С���� ���������K ͼ����ʽ �ɼ��˿� ֡��       ͼ������         ��Ƶ�߶� ��Ƶ���
    //mpeg4
    MEDIA_TYPE_MP4,           0,            75,        31,      2,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
    //261
    MEDIA_TYPE_H261,          0,            75,        31,      2,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
    //262(mpeg-2)
    MEDIA_TYPE_H262,          0,            75,        31,      2,       2048,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
    //263
    MEDIA_TYPE_H263,          1,            75,        31,      2,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
    //h263+
    MEDIA_TYPE_H263PLUS,      0,            300,       31,      2,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
    //h264
    MEDIA_TYPE_H264,          0,            300,       51,      2,       2048,       0,       0,     25,    QC_MODE_QUALITY,       576,     720
};

class CBasInst : public CInstance
{
protected:
    enum ESTATE
    {
        IDLE,
        NORMAL,
    };
protected:
    u8  m_byAudioSN;                // ���ڼ�����Ƶͨ���˿ں�
    u8  m_byVideoSN;                // ���ڼ�����Ƶͨ���˿ں�

    u8  m_byNextChnIdx;             // use in inital 
    u8  m_byAudioChnNum;            // total audio channel
    u8  m_byNextAudioChnIdx;
    u8  m_byAudioStartChnIdx;       // audio channel start index
    u8  m_byAudioGrpNum;            // ��Ƶ��������( Ŀǰֻ֧��һ����Ƶ������)
    u8  m_byAudioGrpStartIdx;       // audio start adp group 
    u8  m_byNextAudioGrpIdx;
    
    u8  m_byVideoChnNum;            // total video channel 
    u8  m_byVideoStartChnIdx;
    u8  m_byNextVideoChnIdx;
    u8  m_byVideoGrpNum;            // ��ƵƵ��������
    u8  m_byVideoGrpStartIdx;       // video start adp group 
    u8  m_byNextVideoGrpIdx;

    u8  m_byAllGrpNum;              // total adp group num
    u8  m_byAllChnNum;              // total channel group num

    TChannelInfo    m_atChnInfo[MAX_CHANNELS_BAS];
    TAdpGrpInfo     m_atAdpInfo[MAXNUM_BASGRP];

public:
    CBasInst(  );
    ~CBasInst(  );

    // ----------- ��Ϣ��� ------------
    void InstanceEntry( CMessage* const pMsg );
    void DaemonInstanceEntry( CMessage* const pMsg, CApp* pcApp );
    
    // --------- ��Ϣ������ ----------
	void  DaemonProcInit( CMessage* const pMsg, CApp* pcApp );    //��ʼ��
    void  ProcConnectTimeOut( BOOL32 bIsConnectA );               //���������
    void  ProcRegisterTimeOut( BOOL32 bIsRegiterA );              //ע�������
    void  MsgDisconnectProc( CMessage* const pMsg, CApp* pcApp ); //��������
    void  MsgRegAckProc( CMessage* const pMsg, CApp* pcApp );     //����ע��ɹ�
    void  MsgRegNackProc( CMessage* const pMsg );                 //����ע��ʧ��
	void  DeamonProcGetMsStatusRsp( CMessage* const pMsg );       //����ȡMpc����������״̬

    void  SendChnNotif( u8 byChnIdx );                            //����ͨ��״̬��Mcu
    void  MsgStartAdptProc( u8 byChnIdx, CMessage* const pMsg );  //��ʼ����
    void  MsgAdjustAdptProc( u8 byChnIdx, CMessage* const pMsg ); //�����������
    void  MsgStopAdptProc( u8 byChnIdx, CMessage* const pMsg );   //ֹͣ����
	void  MsgFastUpdatePicProc( u8 byChnIdx );                    //Mcu����Bas��ؼ�֡
    void  MsgTimerNeedIFrameProc( void );                         //Bas����Mcu���ؼ�֡
    void  ProcSetQosMsg( CMessage* const pMsg );                  //����Qosֵ

	void  DaemonProcReconnectBCmd( CMessage* const pMsg, CApp* pcApp ); //MCU ֪ͨ�Ự ����MPCB ��Ӧʵ��, zgc, 2007/04/30


	// ----------- ���ܺ��� ------------
    s32  AddChannel( u8 byChnIdx, TAdaptParam* ptParm );          //����ͨ��
	s32  Init( CMessage* const pMsg, CApp* pcApp );	              //��ʼ��
	BOOL32 ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode );      //�������Ӻ���
	void Register( BOOL32 bIsRegiterA, u32 dwMcuNode );           //��MCUע��
    u16  AddAudioChannel(  u8 byChnIdx, TAdaptParam& tParm  );    //������Ƶͨ��
    u16  AddVideoChannel(  u8 byChnIdx, TAdaptParam& tParm  );    //������Ƶͨ��
	s32  SendMsgToMcu( u16 wEvent, CServMsg const &cServMsg );
        
    u8   GetGrpNoFromChnIdx( u8 byChnIdx );                           //����ͨ���õ������ڵ�������
    void ComplexQos( u8& byValue, u8 byPrior );                       //���Qosֵ
	void ClearInstStatus( void );                                     //��ձ�ʵ�������״̬��Ϣ
	
	/*lint -save -sem(CBasInst::InitalData,initializer)*/
    void InitalData( void );                          //�����ڲ����ݵĳ�ʼ��
	void ProcEmbed( void );
	u8   ProcFreeConfChns( u8 byChnIdx, CConfId&  cConfId );    
	s8*  ipStr( u32 dwIP );	
	void BasInfo( void );                             //���Խӿ�
    void StatusShow( CApp* pcApp );                   //״̬��ʾ
    BOOL32 GetMediaType( u8 byType, s8* aszTypeStr ); //ȡý������
    
	// ���ñ������
    void SetEncryptParam( u8 byChnIdx, TMediaEncrypt*  ptMediaEncryptVideo,
                          TDoublePayload* ptDoublePayloadVideo,
                          TMediaEncrypt*  ptMediaEncryptAudio,
                          TDoublePayload* ptDoublePayloadAudio,
                          u8 byVidEncryptPT, u8 byAudEncryptPT,
						  u8 byRealEncodePT);

    // ƽ������֧��
    void SetSmoothSendRule(u32 dwDestIp, u16 wPort, u32 dwRate);
    void ClearSmoothSendRule(u32 dwDestIp, u16 wPort);
    void StopSmoothSend();
};

#define MIN_BITRATE_OUTPUT      (u16)128

struct TBasDebugVal
{
protected:
    BOOL32          m_bEnableBitrateCheat;   // �Ƿ����������������
    u16             m_wDefault;              // ȱʡ�µ��İٷֱ�
public:
    void   SetEnableBitrateCheat(BOOL32 bEnable)
    {
        m_bEnableBitrateCheat = bEnable;
    }
    BOOL32 IsEnableBitrateCheat() const
    {
        return m_bEnableBitrateCheat;
    }

    u16    GetDecRateDebug(u16 wOriginRate) const
    {
        // guzh [10/19/2007] û���򷵻�ȱʡֵ
        wOriginRate = wOriginRate * m_wDefault / 100;

        if (wOriginRate > MIN_BITRATE_OUTPUT)
            return wOriginRate;
        else
            return MIN_BITRATE_OUTPUT;
    }
    void   SetDefaultRate(u16 wPctRate)
    {
        m_wDefault = wPctRate;
    }

};
class CBasCfg
{
public:
    CBasCfg(  );
    virtual ~CBasCfg(  );
public:
    u32              m_dwMcuIId;                 //ͨ��MCU.A��ʾ
    u32              m_dwMcuIIdB;                //ͨ��MCU.B��ʾ
    u32              m_dwMcuNode;                //��MCU.Aͨ�Žڵ��
    u32              m_dwMcuNodeB;               //��MCU.Bͨ�Žڵ��
    u32              m_dwMcuRcvIp;               //MCU.A ���յ�ַ
    u32              m_dwMcuRcvIpB;              //MCU.B ���յ�ַ
    u16              m_wMcuRcvStartPort;         //MCU.A ������ʼ�˿ں�
    u16              m_wMcuRcvStartPortB;        //MCU.A ������ʼ�˿ں�
    BOOL32           m_bEmbedMcu;                //�Ƿ���Ƕ��MCU.A��
    BOOL32           m_bEmbedMcuB;               //�Ƿ���Ƕ��MCU.B��
    u8               m_byChnNum;                 //ͨ������
    u8               m_byRegAckNum;              //��һ��ע��ɹ�
	u32              m_dwMpcSSrc;                // guzh [6/12/2007] ҵ���ỰУ��ֵ 
	u16				 m_wMTUSize;				 //MTU��С, zgc, 2007-04-02
    TEqpCfg          m_tCfg;                     //����������
    TEqp             m_tEqp;                     //���豸
    TPrsTimeSpan     m_tPrsTimeSpan;             //�ش�ʱ����
    CKdvAdapterGroup m_cAptGrp[MAXNUM_BASGRP+1]; //һ����ΪRTPͷ������
	
    
    TBasDebugVal     m_tDebugVal;                //����ֵ

	// �Ƿ��ƽ�����͵Ŀ���, zgc, 2008-03-03
	BOOL32 m_bIsUseSmoothSend;
public:
    void FreeStatusDataA( void );
    void FreeStatusDataB( void );

    void ReadDebugValues();
    BOOL32 IsEnableCheat() const
    {
        return m_tDebugVal.IsEnableBitrateCheat();
    }
    u16 GetDecRateDebug(u16 wOriginRate) const
    {
        return m_tDebugVal.GetDecRateDebug(wOriginRate);
    }

    void GetDefaultParam(u8 byEnctype,TVideoEncParam& TEncparam);
    void GetDefaultDecParam( u8 mediatype, TVideoDecParam& tParam );  //ȡĬ�Ͻ������
};

void baslog( s8* pszFmt, ...);

typedef zTemplate< CBasInst , MAXNUM_BAS_CHNNL, CBasCfg > CBasApp;
extern CBasApp g_cBasApp;

#endif //!_BASINST_H_
