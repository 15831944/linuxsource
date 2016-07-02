/*****************************************************************************
ģ����      : ������뵥Ԫ
�ļ���      : hduinst.h
����ļ�    : 
�ļ�ʵ�ֹ���: hduʵ��ͷ�ļ�
����        : ���ֱ�
�汾        : V1.0  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
08/11/26    4.6         ���ֱ�        ����
******************************************************************************/
#ifndef _HDU_INST_H_
#define _HDU_INST_H_

#include "osp.h"
#include "mcustruct.h"
#include "eqpcfg.h"

#include "mcuconst.h"
#include "kdvmedianet.h"
#include "codecwrapper_hd.h"
#include "codecwrapperdef_hd.h"
#include <fstream>

#include "hduagent.h"

#include "evhduid.h"
#include "hdustruct.h"
#include "errhdu.h"
#include "evmcueqp.h"
#include "mcuver.h"


#define MAX_VIDEO_FRAME_SIZE (512 * 1024)

#define MAX_AUDIO_FRAME_SIZE (8 * 1024)


/*     ��ӡ����    */
#define LOG_CRIT            (u8)0
#define LOG_ERR             (u8)1
#define LOG_WARNING            (u8)2
#define LOG_INFO            (u8)3



class CDecoderGrp
{
public:
	CKdvMediaRcv     	m_cVidMediaRcv;        	//һ����Ƶ����
	CKdvMediaRcv     	m_cAudMediaRcv;        	//һ����Ƶ����
	CKdvVidDec    	    m_cVidDecoder;          //��Ƶ������
    CKdvAudDec          m_cAudDecoder;          //��Ƶ������
	
private:
	BOOL32           	m_bIsStart;             //�Ƿ�����
	u8					m_byLastOutPutMode;		//��¼�ϴ����ģʽ
	u32              	m_dwNetBand;            //�����
	u32              	m_dwRtcpBackAddr;       //rtcp�ط���ַ
	u16              	m_wRtcpBackPort;    	//rtcp�ط��˿�
	u16              	m_wLocalStartPort;  	//Ϊ���ؽ��յ�rtp�˿ڣ�rtcp�˿�Ϊrtp�˿�+1
	
	TMediaEncrypt       m_tMediaDec;            //���ܲ���
	BOOL32           	m_bIsNeedRS;         	//�Ƿ���Ҫ�ش�
	TRSParam         	m_tRsParam;         	//�ش�����
public:
	CDecoderGrp()
	{
		m_bIsStart = 0;
		m_byLastOutPutMode = 0;
		m_dwNetBand = 0;
		m_dwRtcpBackAddr = 0;
		m_wRtcpBackPort = 0;
		m_wLocalStartPort = 0;
		m_bIsNeedRS = 0;
		memset((void*)&m_tMediaDec, 0x0, sizeof(m_tMediaDec));
        memset((void*)&m_tRsParam, 0x0, sizeof(m_tRsParam));
	}
	~CDecoderGrp()
	{
        //�Ƿ�Ҫֹͣ������Ӧ������Ƶ������
	}
public:
	BOOL32    IsStart(void){ return m_bIsStart; }
	void      SetIsStart( BOOL32 bIsStart ){ m_bIsStart = bIsStart; }

	u8        GetLastOutPutMode(void){ return m_byLastOutPutMode; }
	void      SetLastOutPutMode( u8	byLastOutPutMode ){ m_byLastOutPutMode = byLastOutPutMode; }

    u32       GetNetBand( void ){ return m_dwNetBand; }
	void      SetNetBand( u32 dwNetBand ){ m_dwNetBand = dwNetBand; }
    
	u32       GetRtcpBackAddr( void ){ return m_dwRtcpBackAddr; }
	void      SetRtcpBackAddr( u32 dwRtcpBackAddr ){ m_dwRtcpBackAddr = dwRtcpBackAddr; }
	
	u16       GetRtcpBackPort( void ){ return m_wRtcpBackPort; }
	void      SetRtcpBackPort( u16 wRtcpBackPort ){ m_wRtcpBackPort = wRtcpBackPort; }
    
	u16       GetLocalStartPort(void){ return m_wLocalStartPort; }
	void      SetLocalStartPort(u16 wLocalStartPort){ m_wLocalStartPort = wLocalStartPort; }

	BOOL32    GetMediaEncrypt(TMediaEncrypt& tMediaEncrypt)
	{
        memcpy( &tMediaEncrypt, &m_tMediaDec, sizeof(TMediaEncrypt) );
		return TRUE;
	}
	void      SetMediaEncrypt(TMediaEncrypt *ptMediaEncrypt)
	{
		memcpy( &m_tMediaDec, ptMediaEncrypt, sizeof(TMediaEncrypt) );
		return;
	}

	BOOL32    IsNeedRS(void){ return m_bIsNeedRS; }
	void      SetIsNeedRS(BOOL32 bIsNeedRS){ m_bIsNeedRS = bIsNeedRS; }

    BOOL32    GetRsParam(TRSParam& tRsParam)
	{ 
		tRsParam.m_wFirstTimeSpan  = m_tRsParam.m_wFirstTimeSpan;
		tRsParam.m_wSecondTimeSpan = m_tRsParam.m_wSecondTimeSpan;                                          
		tRsParam.m_wThirdTimeSpan  = m_tRsParam.m_wThirdTimeSpan;
		tRsParam.m_wRejectTimeSpan = m_tRsParam.m_wRejectTimeSpan;
	    return TRUE;
	}	
	void      SetRsParam(TRSParam &tRsParam)
	{ 
	    m_tRsParam.m_wFirstTimeSpan  = tRsParam.m_wFirstTimeSpan;
	    m_tRsParam.m_wSecondTimeSpan = tRsParam.m_wSecondTimeSpan;                                          
	    m_tRsParam.m_wThirdTimeSpan  = tRsParam.m_wThirdTimeSpan;
	    m_tRsParam.m_wRejectTimeSpan = tRsParam.m_wRejectTimeSpan;
	    return;
	}

	//����KdvMediaRcv,KdvVidDec,KdvAudDec
    BOOL Create(u32    dwChnId,                  //ͨ��ID    		 
		        u32    dwRtcpBackAddr,         //rtcp�ط�Ip��ַ��
				u16    wRtcpBackPort,          //rtcp�ط��˿�
	 	        u16    wLocalStartPort,        //rtp�˿ڣ�rtcp�˿�Ϊrtp�˿�+1
				u32    dwContext);             //��Ӧ����͸��������ʵ��ָ��

	//��ʼ����
	BOOL StartDecode(BOOL32 bStartMediaRcv = FALSE, u8 byOutPutMode = HDU_OUTPUTMODE_BOTH);

	//ֹͣ����
    BOOL StopDecode( BOOL32 bStartMediaRcv = TRUE, u8 byOutPutMode = HDU_OUTPUTMODE_BOTH );

   	//��ʼ��Ƶ����
	BOOL StartVidDec(BOOL32 bStartMediaRcv = FALSE);
	
	//ֹͣ��Ƶ����
	BOOL StopVidDec(BOOL32 bStopMediaRcv = TRUE, BOOL32 bStopDec = FALSE);
 
	//��ʼ��Ƶ����
	BOOL StartAudDec(BOOL32 bStartMediaRcv = FALSE);
	
	//ֹͣ��Ƶ����
	BOOL StopAudDec(BOOL32 bStopMediaRcv = TRUE, BOOL32 bStopDec = FALSE);

	//����ͼ�����������ش�����
	BOOL SetNetRecvFeedbackVideoParam(TRSParam tNetRSParam, BOOL32 bRepeatSnd = FALSE);

	//���� H.263+/H.264 �ȶ�̬��Ƶ�غɵ� Playloadֵ
    BOOL SetVideoActivePT(u8 byRmtActivePT, u8 byRealPT );

	//������Ƶ��̬�غ� playloadֵ
	BOOL SetAudActivePT(u8 byRmtActivePT, u8 byRealPT );

	//���� ��Ƶ����key�ִ� �Լ� ����ģʽ Aes or Des
    BOOL SetVidDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode);

	//������Ƶ�����ִ�������ģʽ
	BOOL SetAudDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode);

	//������Ƶ����������ش�����
	BOOL SetNetRecvFeedbackAudioParam(TRSParam tNetRSParam, BOOL32 bRepeatSnd = FALSE);

	//��ȡ��Ƶ������״̬
	BOOL GetVidDecoderStatus(TKdvDecStatus &tKdvDecStatus);

    //��ȡ��Ƶ��������ͳ����Ϣ
	BOOL GetVidDecoderStatis(TKdvDecStatis &tKdvDecStatis);	

	//��ȡ��Ƶ������״̬
	BOOL GetAudDecoderStatus(TKdvDecStatus &tKdvDecStatus);

    //��ȡ��Ƶ��������ͳ����Ϣ
	BOOL GetAudVidDecoderStatis(TKdvDecStatis &tKdvDecStatis);	
    
	//������Ƶ���Ŷ˿����� VGA or DVI
	BOOL SetVidPlyPortType(u32 dwType);

	//������Ƶ����ź���ʽ��NULL��ʾ�Զ�����
    BOOL SetVideoPlyInfo(TVidSrcInfo* ptInfo);
	
	//�������ű���
	BOOL SetPlayScales( u16 wWidth, u16 wHeigh );

    //������Ƶ���Ŷ˿����� HDMI or C
    BOOL SetAudOutPort(u32 dwAudPort);

	//������Ƶ���յ�ַ����(���еײ��׽��ӵĴ������󶨶˿ڵȶ���)
    BOOL SetVidNetRcvLocalParam ( TLocalNetParam *ptLocalNetParam );

	//������Ƶ���յ�ַ����(���еײ��׽��ӵĴ������󶨶˿ڵȶ���)
    BOOL SetAudNetRcvLocalParam ( TLocalNetParam *ptLocalNetParam );
	
	//���������������
	u16  SetAudioVolume(u8 byVolume ); 
	
	//�õ������������
    u16  GetAudioVolume(u8 &pbyVolume );
	
	//���þ���
    u16  SetAudioMute(BOOL32 bMute);

	//��ʾͨ����Ϣ,����·���붼��ӡ����
	void ShowChnInfo(void);
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

class CHduInstance: public CInstance
{
	enum ESTATE
    {
        eIDLE,
		eREADY,
		eNORMAL,
    };
public:
	TChannelInfo         m_tChnInfo;                                    //�����
	CDecoderGrp     	*m_pDecCroup;                                   //���Ͻ�����ָ��
public:
	CHduInstance();
	virtual ~CHduInstance();
	void InstanceEntry( CMessage* const pMsg );                         
	void DaemonInstanceEntry( CMessage* const pMsg, CApp* pcApp );      

	/* --------- DaemonInstance��Ϣ������ -------------*/
	void  DaemonProcPowerOn( CMessage* const pMsg);       //��ʼ��
	void  DaemonProcHduStartPlay( CMessage* const pMsg );               //��ʼ����
    void  DaemonProcHduStopPlay( CMessage* const pMsg );                //ֹͣ����
	void  DaemonProcOspDisconnect( CMessage* const pMsg);				//��������
	void  DaemonProcConnectTimeOut( BOOL32 bIsConnectA );               //��MCU����
	void  DaemonProcRegisterTimeOut( BOOL32 bIsRegiterA );              //��MCUע��    
	void  DaemonProcMcuRegAck(CMessage* const pMsg);					//����ע��ACK��Ӧ
    void  DaemonProcMcuRegNack(CMessage* const pMsg);                   //����ע��NACK��Ӧ
	void  DaemonProcModePortChangeNotif( CMessage* const pMsg );        //��ʽ�ı�֪ͨ
	void  DeamonProcGetMsStatusRsp( CMessage* const pMsg );             //ȡ��������״̬
	void  DaemonProcSetQosCmd( CMessage* const pMsg );                  //����Qosֵ
    void  DaemonProcHduChangeVolumeReq( CMessage* const pMsg );         //��������
	void  DaemonProcChangePlayPolicy( CMessage* const pMsg );           //ͨ������ʵ�ֲ��Ըı�֪ͨ
	
	/* --------- Instance��Ϣ������ -------------*/
	void  ProcInitHdu(void);                                            //��ʼ��HDU
	void  ProcInitHduChnl( CMessage* const pMsg );                      //��ʼ��hduͨ����Ϣ
	void  ProcStopHdu( void );                                          //ֹͣHDU�豸
	void  ProcStartPlayReq( CServMsg &cServMsg );  		                //��ʼ����
	void  ProcStopPlayReq( CServMsg &cServMsg );   		                //ֹͣ����
	void  ProcTimerNeedIFrame( void );                                  //Hdu����Mcu���ؼ�֡
	void  ProcSetMode( CMessage* const pMsg );                          //�������������ʽ
    void  ProcHduChangeVolumeReq( CServMsg &cServMsg );                 //������������
	void  ProcChangePlayPolicy( CMessage* const pMsg );                 //ͨ������ʵ�ֲ��Ըı�֪ͨ
	
	/* ---------------- ���ܺ��� ------------------*/
	BOOL32 ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode );            //��MCU
	void   Register(u32 dwMcuNode );               //��MCUע��
	BOOL32 SendMsgToMcu( u16 wEvent, CServMsg& cServMsg );              //��MCU������Ϣͨ�ú���
	void   SendChnNotif( BOOL32 bFirstNotif = FALSE );                          //ͨ��״̬֪ͨ 
	void   ClearInstStatus( void );                                     //��ձ�ʵ�������״̬��Ϣ
	void   StatusShow( void );                                          //״̬��ʾ����ʾ���Ͻ�����״̬
	void   StatusNotify(void);                                          //HDU״̬֪ͨ                                  
	void   ComplexQos( u8& byValue, u8 byPrior );                       //���Qosֵ
    BOOL32 SetEncryptParam( TMediaEncrypt *ptMediaEncryptVideo, 
		                    TDoublePayload *ptDoublePayloadVideo );     // ������Ƶ�������
	BOOL32 SetAudEnctypt( TMediaEncrypt *ptMediaEncryptVideo, 
		                  TDoublePayload *ptDoublePayloadVideo );       // ������Ƶ�������
	BOOL Play( u8 byMode );						                        // ��ʼ����
	BOOL Stop( void );						                            // ֹͣͨ������
	// ����HDU����ӿں������ʽ�õ���ƵԴ��Ϣ
	BOOL GetVidSrcInfoByTypeAndMode(u8 byOutPortType, u8 byOutModeType, TVidSrcInfo *ptVidSrcInfo);

}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

#ifndef MIN_BITRATE_OUTPUT
#define MIN_BITRATE_OUTPUT      (u16)128
#endif

struct THduDebugVal
{
protected:
    BOOL32          m_bEnableBitrateCheat;                              // �Ƿ����������������
    u16             m_wDefault;                                         // ȱʡ�µ��İٷֱ�
	s32             m_nVidDecResizeMode;                                // 0���Ӻڱߣ�1���ñߣ�2��ȫ�� (��ʼ�����ã��������ļ�)
																		
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
		//û���򷵻�ȱʡֵ
        wOriginRate = wOriginRate * m_wDefault/100;

        if (wOriginRate > MIN_BITRATE_OUTPUT)
            return wOriginRate;
        else
            return MIN_BITRATE_OUTPUT;
    }
    void   SetDefaultRate(u16 wPctRate)
    {
        m_wDefault = wPctRate;
    }

	void   SetVidDecResizeMode(s32 nMode)
    {
        m_nVidDecResizeMode = htonl(nMode);
    }

	u32    GetVidDecResizeMode(void)
    {
        return ntohl( m_nVidDecResizeMode );
    }	
	
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

class CHduData
{
public:
	CHduData();
	virtual ~CHduData();
public:
	u8                  m_bMaxChnNum;                   //�����ŵ���

	u8                  m_byOutPutMode;                 //���ģʽ0:�����ģʽ��1:��Ƶ��2:��Ƶ�� 3:����Ƶ

//	BOOL32              m_bMute[MAXNUM_HDU_CHANNEL];    // ���澲��״̬���Ա���ѯ��ָ�    
	u32              	m_dwMcuIId;           	        // ͨ��MCU.A��ʾ
	u32              	m_dwMcuIIdB;           	        // ͨ��MCU.B��ʾ
	u32              	m_dwMcuNode;          	        // ��MCU.Aͨ�Žڵ��
	u32             	m_dwMcuNodeB;         	        // ��MCU.Bͨ�Žڵ��
	
    BOOL32              m_bEmbedMcu;                    // �Ƿ���Ƕ��MCU.A��
    BOOL32              m_bEmbedMcuB;                   // �Ƿ���Ƕ��MCU.B��

	u32              	m_dwMcuRcvIp;          	        // MCU.���յ�ַ
//	u16              	m_wMcuRcvStartPort;             // MCU.������ʼ�˿ں�
	
	u8               	m_byRegAckNum;        	        // ��һ��ע��ɹ�
	u32              	m_dwMpcSSrc;           	        // ҵ���ỰУ��ֵ 
	u16  		     	m_wMTUSize;                     // ����䵥Ԫ
	THduCfg         	m_tCfg;                         // HDU����
	TEqp             	m_tHdu;                      	// ���豸
	TPrsTimeSpan     	m_tPrsTimeSpan;                 // �ش�ʱ����
	THduDebugVal    	m_tDebugVal;                    // ����Ԥ��
    THduStatus          m_tHduStatus;                   // HDU״̬

public:
	void SetNull(void);
    void FreeStatusDataA(void);                         // ���A״̬����
    void FreeStatusDataB(void);                         // ���B״̬����

	void ReadDebugValues(void);                         // �������ļ��ж�ȡ��������ֵ
    BOOL32 IsEnableCheat(void) const;                   // ��ȡ�Ƿ�����
    u16 GetDecRateDebug(u16 wOriginRate) const;         // ��ȡ��������ֵ
	s32 GetVidDecResizeMode( void )
    {
        return m_tDebugVal.GetVidDecResizeMode();
    }
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;
void hdulog( s8* pszFmt, ...);
void hdulogall( s8* pszFmt, ...);
s8*  ipStr( u32 dwIP );	

typedef zTemplate< CHduInstance, MAXNUM_HDU_CHANNEL, CHduData > CHduApp;
extern CHduApp g_cHduApp;

#endif //end _TV_HDU_INST_H_


