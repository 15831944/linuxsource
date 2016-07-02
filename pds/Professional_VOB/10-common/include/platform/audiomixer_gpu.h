/*****************************************************************************
  ģ����      : ����ģ��
  �ļ���      : audiomixer.h
  ����ļ�    : 
  �ļ�ʵ�ֹ���: ��Ƶ����
  ����        : ��С��
  �汾        : V1.0  Copyright(C) 1997-2009 KDC, All rights reserved.
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2009/03/24  V1.0        ��С��       ����
******************************************************************************/
#ifndef _AUDIO_MIXER_GPU_H_
#define _AUDIO_MIXER_GPU_H_

#include "osp.h"
#include "kdvmedianet.h"
#include "codecwrapper_common.h"


/*������ͳ����Ϣ*/
typedef struct tagKdvEncStatis
{
    u32  m_dwFrameRate;  /*֡��*/
    u32  m_dwFrameNum;	//����֡��
    u32  m_dwBitRate;    /*�����ٶ�*/
    u32  m_dwPackLose;   /*��֡��*/
    u32  m_dwPackError;  /*��֡��*/
	u32  m_wAvrBitRate;  /*1��������Ƶ����ƽ������*/
}TKdvEncStatis;



#define AUDMIX_MAX_CHANNEL_USR 64          //������·��
#define AUDMIX_MAX_CHANNEL     64          //������·��

#define MAX_MIXER_DEPTH     8 //�������
#define MAX_MIX_NUM  2

#define AUDIO_ADAPTOR_MAX_CHNL		4		//��Ƶ������ͨ����
#define AUDIO_ADAPTOR_MAX_ENCNUM	2		//��Ƶ����ͨ����������

#define AUDIO_NMODE_MAX_CHNL		3			//������Nģʽ����·��
enum emErrorCode
{
	Codec_Success = 0,
	Codec_ERROR_NOMEM = 30000,
	Codec_ERROR_NOCREATE,
	Codec_ERROR_RECREATE,
	Codec_ERROR_Param,
	Codec_ERROR_AEncCreate,
	
};
enum
{
	AUDIO_MODE_MP3 = 4,
	AUDIO_MODE_PCMA = 5,
	AUDIO_MODE_PCMU,
	AUDIO_MODE_G723_6,
	AUDIO_MODE_G723_5,
	AUDIO_MODE_G728,
	AUDIO_MODE_G722,
	AUDIO_MODE_G729,
	AUDIO_MODE_G719,
	AUDIO_MODE_G7221,
	AUDIO_MODE_ADPCM,
    AUDIO_MODE_AACLC_32,     //32K ˫����
    AUDIO_MODE_AACLC_32_M,   //32K ������
    AUDIO_MODE_AACLC_48,     //48K ˫����
    AUDIO_MODE_AACLC_48_M,   //48K ������
    AUDIO_MODE_AACLD_32,
    AUDIO_MODE_AACLD_32_M,
    AUDIO_MODE_AACLD_48,
    AUDIO_MODE_AACLD_48_M
};

//������ͨ����Ϣ
typedef struct tagMixChnInfo
{ 
     BOOL32   m_bForceChannel;   //�Ƿ���ǿ�ƻ���ͨ��
     u32     m_dwChannelId;    //     ͨ����
} TMixChnInfo;

//����ͨ��
typedef struct
{    
    TNetAddress m_tSrcNetAddr;     //ͨ��Դ��ַ(���ն���)
    //ͨ��ԴRTCP��ַ����ͨ��Դ��ַ�Ķ˿ڼ�һ(���ն���)
    TNetAddress m_tRtcpBackAddr;   //ͨ��RTCP������ַ(���ն���)
	TNetAddress m_tRtcpLocalAddr;
    TNetAddress m_tLocalNetAddr;   //ͨ�����ص�ַ(���Ͷ���)
    //ͨ������RTCP��ַ����ͨ�����ص�ַ�Ķ˿ڼ�һ(���Ͷ���)
    TNetAddress m_tDstNetAddr;     //ͨ��Ŀ�ĵ�ַ(���Ͷ���)
}TMixerChannelParam;

typedef struct tagMixerChannel
{
	u8     m_byChannelIndex;//ͨ��������
	TNetAddress m_tSrcNetAddr;     //ͨ��Դ��ַ(���ն���)
	//ͨ��ԴRTCP��ַ����ͨ��Դ��ַ�Ķ˿ڼ�һ(���ն���)
    TNetAddress m_tRtcpBackAddr;   //ͨ��RTCP������ַ(���ն���)
    TNetAddress m_tLocalNetAddr;   //ͨ�����ص�ַ(���Ͷ���)
	//ͨ������RTCP��ַ����ͨ�����ص�ַ�Ķ˿ڼ�һ(���Ͷ���)
    TNetAddress m_tDstNetAddr;     //ͨ��Ŀ�ĵ�ַ(���Ͷ���)
}TMixerChannel;

typedef struct tagMixerGroupStatus
{
    BOOL32   m_bMixerStart;      //�����Ƿ�ʼ
    u8     m_byCurChannelNum;  //�������ͨ����
    TNetAddress m_tNModeDstAddr[AUDIO_NMODE_MAX_CHNL];    //Nģʽ�������ַ
    TMixerChannel m_atChannel[AUDMIX_MAX_CHANNEL_USR];  //���е�ͨ��	
}TMixerGroupStatus;

//ͨ��ͳ����Ϣ
typedef struct tagMixerChannelStatis
{
    u32  m_dwRecvBitRate;        //��������
    u32  m_dwRecvPackNum;        //�յ��İ���
    u32  m_dwRecvLosePackNum;    //�������ȱ�ٵİ���
    u32  m_dwRecvFps;		//֡��
    u32  m_dwSendBitRate;        //��������
    u32  m_dwSendPackNum;        //���͵İ���
    u32  m_dwMixerDiscardPackNum;//���������İ���
}TMixerChannelStatis;

typedef struct tagAudType
{
	u32 dwAudType;
	u32 dwAudMode;
}TAudType;
//����ͨ��״̬
typedef struct tagAudAdapterStatus
{
	BOOL32		bAdpUsed;	//��ǰͨ���Ƿ�ʹ��
	TAudType	tAudType;	//��Ƶ��ʽ
}TAudAdapterStatus;

typedef struct tagAudAdapterStatis
{
	u32  m_dwRecvBitRate;        //��������
	u32  m_dwRecvPackNum;        //�յ��İ���
	u32  m_dwRecvLosePackNum;    //�������ȱ�ٵİ���
	
}TAudAdapterStatis;
typedef struct tagMixerChnlInfo
{
	u32 dwMixerId;	//������ID
	u32 dwChID;		//ͨ����
	BOOL32 bNMode;	//�Ƿ�Nģʽ�����ڻ�����
}TMixChnlInfo;

//���������ص���byExciteChn��ʾ��ǰ������ͨ����, Ϊ0��ʾû�м�����ͨ��
typedef void (*TMixerCallBack)(unsigned char byExciteChn, unsigned int dwContext);
//�������ص�,dwChID:�����ID,  dwEncID:������ID
typedef void (*AudAdapterCallBack)(u32 dwChID, u32 dwEncID,  PFRAMEHDR pFrmHdr, void* pContext);

class CODECWRAPPER_API CAudioMixer
{
public:
    CAudioMixer(void);
    ~CAudioMixer();
 /*************************************************************
    ������    : Init
    ����      ����������ʼ��
    ����˵��  ��   byMixNum [I]�� �������ܵ�ͨ������������  �ܵ��������64·
                                    ������2�����������ֵΪ32, 4��������Ϊ64/4=16

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
   BOOL32 Init(u8 byMixNum);
    
    /*************************************************************
    ������    : StartMixer
    ����      ����ʼ����
    ����˵��  ����            

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
    BOOL32 StartMixer();

    /*************************************************************
    ������    : StopMixerSnd
    ����      ��ֹͣ����
    ����˵��  ��            

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
    BOOL32 StopMixer();
    
     /*************************************************************
    ������    : SetNModeOutput
    ����      ��Nģʽ������Ϣ����
    ����˵��  ��ptLocalAddr             [I] ����IP,�˿ڣ����ڷ���
    ptDstNetAddr            [I] Զ��IP,�˿ڣ����ڷ���
    wTimeSan                [I] �ش�ʱ��
	bRepeat                 [I] �Ƿ��ش���־
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
    BOOL32 SetNModeOutput(TNetAddress* ptLocalAddr, TNetAddress* ptDstNetAddr, u16 wTimeSan, BOOL32 bRepeat = FALSE);
	 
     /*************************************************************
    ������    : SetNModeOutput
    ����      ��֧�ֶ��ʽ��Nģʽ������Ϣ����
    ����˵��  ��
    ptLocalAddr             [I] ����IP,�˿ڣ����ڷ���
    ptDstNetAddr           [I] Զ��IP,�˿ڣ����ڷ���
    dwNetAddrNum		 [I] ָ���������ַ�ĸ���
    wTimeSan                [I] �ش�ʱ��
	bRepeat                [I] �Ƿ��ش���־
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/	 
    BOOL32 SetNModeOutput(TNetAddress* ptLocalAddr, TNetAddress* ptDstNetAddr,u32 dwNetAddrNum, u16 wTimeSan, BOOL32 bRepeat = FALSE); 
     /*************************************************************
    ������    : SetAudEncParam
    ����      �����ñ������
    ����˵��  ��    [I]byChnNo      ͨ����       
    bNmode                     [I]�Ƿ�Ϊָ��Nģʽ�ĸ�ʽ
    dwEncType                [I] ��������
    dwEncMode                [I] ����ģʽ
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
    BOOL32 SetAudEncParam(u8 byChnNo, u32 dwEncType, u32 dwEncMode,BOOL32 bNMode=FALSE);

     /*************************************************************
    ������    : SetAudioDecParam
    ����      �����ý������---��AACLC/AACLD��Ч
    ����˵��  ��    [I]byChnNo      ͨ����       
    dwDecType                 [I]��������
    dwDecMode                [I] ����ģʽ
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
    BOOL32 SetAudioDecParam(u8 byChnNo, u32 dwDecType, u32 dwDecMode);	 
    /*************************************************************
    ������    : AddMixerChannel
    ����      �����ý��ս���
    ����˵��  ��byChnNo             [I] ͨ�����
                ptMixerChannelParam [I] ����ָ��
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE     
    **************************************************************/
    BOOL32 AddMixerChannel(u8 byChnNo, TMixerChannelParam* ptMixerChannelParam);

    /*************************************************************
    ������    : DeleteMixerChannel
    ����      ��ɾ����Ļ���ͨ��
    ����˵��  ��byChnNo      [I] ͨ�����            

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE   
    **************************************************************/
    BOOL32 DeleteMixerChannel(u8 byChnNo);
    
    /*************************************************************
    ������    : SetMixerCallBack
    ����      �����������ص�
    ����˵��  ��tMixerCallBack    [I] �ص�����
                dwContext         [I] �ص���������

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE     
    **************************************************************/
    BOOL32 SetMixerCallBack(TMixerCallBack tMixerCallBack, int dwContext);
    
    /*************************************************************
    ������    : SetMixerCallBack
    ����      �����û������
    ����˵��  ��byDepth    [I] �������           

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE     
    **************************************************************/
    BOOL32 SetMixerDepth(u8 byDepth);     
    
    /*************************************************************
    ������    : SetMixerCallBack
    ����      ������������������ʱ����, ��λ����
    ����˵��  ��dwKeepTime    [I] ������������ʱ����, ��λ����          

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE 
    --------------------------------------------------------------
    �޸ļ�¼  ��
    ��  ��      �汾        �޸���      �߶���        �޸ļ�¼
    2009/04/14  V1.0        ��С��                      ����
    **************************************************************/
    BOOL32 SetExciteKeepTime(u32 dwKeepTime);  

    /*************************************************************
    ������    : SetMixerDelay
    ����      �����û�����ʱ(ms)
    ����˵��  ��dwTime    [I] ������ʱ(ms)

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE  
    **************************************************************/
    BOOL32 SetMixerDelay(u32 dwTime);      
    
    /*************************************************************
    ������    : SetForceAudMix
    ����      ��ǿ�ƻ���
    ����˵��  ��byChnNo    [I] ǿ�ƻ���ͨ���ţ��ӱ�Ŵӣ���ʼ

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE  
    **************************************************************/
    BOOL32 SetForceAudMix(u8 byChnNo);     //ǿ�ƻ���

    /*************************************************************
    ������    : CancelForceAudMix
    ����      ��ȡ������ǿ��
    ����˵��  ����

    ����ֵ˵����TRUE:Ϊ����  FALSE:Ϊֹͣ
    **************************************************************/
    BOOL32 CancelForceAudMix();            //ȡ��ǿ��

    /*************************************************************
    ������    : StopMixerSnd
    ����      ��
    ����˵��  ��bStopSnd      [I] TRUE: ֹͣ���з���, FALSE: ��ʼ���з���            

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
    BOOL32 StopMixerSnd(BOOL32 bStopSnd);	//TRUE: ֹͣ���з���, FALSE: ��ʼ���з���

    /*************************************************************
    ������    : GetActiveStatus
    ����      ��ȡ����/ֹͣ ״̬
    ����˵��  ����

    ����ֵ˵����TRUE:Ϊ����  FALSE:Ϊֹͣ    
    **************************************************************/
    BOOL32 GetActiveStatus(void);    
	
    /*************************************************************
    ������    : SetDecryptKey
    ����      �����ý��ս���key�ִ�, pszKeyBuf����ΪNULL-��ʾ������
    ����˵��  ��byChnNo        [I] ���ս���ͨ�����
                pszKeyBuf      [I]
                wKeySize       [I]
                byDecryptMode  [I]

    ����ֵ˵����TRUE:Ϊ����  FALSE:Ϊֹͣ    
    **************************************************************/
    BOOL32 SetDecryptKey(u8 byChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode=DES_ENCRYPT_MODE); 
    
    /*************************************************************
    ������    : SetAudioResend
    ����      �����ü���key�ִ������������Ķ�̬�غ�PTֵ, pszKeyBuf����ΪNULL-��ʾ������
    ����˵��  ��byChnNo          [I] ���뷢��ͨ�����
                pszKeyBuf        [I]
                wKeySize         [I]
                byEncryptMode    [I]

    ����ֵ˵����TRUE:Ϊ����  FALSE:Ϊֹͣ    
    **************************************************************/
    BOOL32 SetEncryptKey(u8 byChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode=DES_ENCRYPT_MODE, BOOL32 bNMode=FALSE);

    /*************************************************************
    ������    : SetAudioResend
    ����      ��������Ƶ�ط�nSendTimes���ط�������nSendSpan��ÿ���ط��������Ҫ�ѷ����ش���
    ����˵��  ��byChnNo          [I] ͨ�����
                nSendTimes       [I]
                nSendSpan        [I]

    ����ֵ˵����TRUE:Ϊ����  FALSE:Ϊֹͣ    
    **************************************************************/
    BOOL32 SetAudioResend(u8 byChnNo, s32 nSendTimes, s32 nSendSpan,BOOL32 bNMode=FALSE);

    /*************************************************************
    ������    : SetAudioActivePT
    ����      ������ ��̬�غɵ� Playloadֵ, byRmtActivePT����Ϊ0-��ʾ��� ���˶�̬�غɱ��
    ����˵��  ��byChnNo       [I] ���ս���ͨ�����
                byRmtActivePT [I] ��̬�غɲ���
                byRealPT      [I] ��̬�غɲ���

    ����ֵ˵����TRUE:Ϊ����  FALSE:Ϊֹͣ
    **************************************************************/
    BOOL32 SetAudioActivePT(u8 byChnNo, u8 byRmtActivePT, u8 byRealPT);  
    
    /*************************************************************
    ������    : SaveDecIn
    ����      ���������ǰ����
    ����˵��  ��l32ChnNo         ������ͨ����
                l32FrameNum      Ҫ�����֡��
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
    BOOL32 SaveDecIn(l32 l32ChnNo, l32 l32FrameNum);

    /*************************************************************
    ������    : SaveDecOut
    ����      ����������pcm
    ����˵��  ��l32ChnNo         ������ͨ����
                l32FrameNum      Ҫ�����֡��
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
    BOOL32 SaveDecOut(l32 l32ChnNo, l32 l32FrameNum);

    /*************************************************************
    ������    : SaveEncIn
    ����      ���������ǰpcm
    ����˵��  ��l32ChnNo         ������ͨ����
                l32FrameNum      Ҫ�����֡��
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
    BOOL32 SaveEncIn(l32 l32ChnNo, l32 l32FrameNum);

    /*************************************************************
    ������    : SaveEncOut
    ����      ��������������
    ����˵��  ��l32ChnNo         ������ͨ����
                l32FrameNum      Ҫ�����֡��
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
    BOOL32 SaveEncOut(l32 l32ChnNo, l32 l32FrameNum);

    /*************************************************************
    ������    : GetMixStatus
    ����      ����û�����״̬��Ϣ
    ����˵��  TMixerGroupStatus  �ṹ��
    
    ����ֵ˵����
    **************************************************************/
	void   GetMixStatus(TMixerGroupStatus* tStatus);

    /*************************************************************
    ������    : GetMixStatis
    ����      ����û�����ͳ����Ϣ
    ����˵��  TMixerGroupStatus  �ṹ��
    
    ����ֵ˵����
    **************************************************************/
	void   GetMixStatis(u32 dwChnID, TMixerChannelStatis* tStatis);
    /*************************************************************
    ������    : GetNMixStatis
    ����      �����N ģʽ�ı���ͳ��
    ����˵��		dwChnId    ͨ����
    					TMixerChannelStatis    ͳ��ֵ
    ����ֵ˵����
    **************************************************************/
	void   GetNMixStatis(u32 dwChnId,TMixerChannelStatis* tStatis );

	/*************************************************************
    ������    : SetFeedBackParam
    ����      �������ش����Ʋ���
    ����˵��	channelID
				TNetRSParam  �ṹ��
    
    ����ֵ˵����
    **************************************************************/
	BOOL32 SetFeedbackParam(u32 dwId, TNetRSParam tParam, BOOL32 bRepeat);

	/*************************************************************
    ������    : SetSndFeedBackParam
    ����      �������ش����Ʋ���
    ����˵��	channelID
				TNetRSParam  �ṹ��
    
    ����ֵ˵����
    **************************************************************/
	BOOL32 SetSndFeedbackParam(u32 dwId, u16 wTimeSan, BOOL32 bRepeat,BOOL32 bNMode=FALSE);

	/*************************************************************
    ������    : SetAACChannel
    ����      ������AAC��Ƶ������
    ����˵��	dwChannel    ������
   
    ����ֵ˵����
    **************************************************************/
	BOOL32 SetAACChannel(l32 dwChannel);

	    /*************************************************************
    ������    : SetActivePT
    ����      ������ ��̬�غɵ� Playloadֵ
    ����˵��  ��byChnNo       [I] ����ͨ�����
	            byMediaType   [I] ��̬�غɲ���

    ����ֵ˵����TRUE:Ϊ����  FALSE:Ϊֹͣ
    **************************************************************/
	
	BOOL32 SetActivePT(u8 dwChnlId, u8 byMediaType,BOOL32 bNMode=FALSE);

	void Free();

	void* m_pcMixer;                  //����

    void* m_pListSphExcit;            //���������б�
    void* m_pListEnc;                 //���ձ����б�
    void* m_pListDec;                 //���뷢���б� 
	void* m_pListDecBackBuf;          //���뱸��Buf�б����ڲ�ͬʱ����Ƶ����
    
    //����б�
    void* m_pListSelChnNo;            //ѡ�еı���б�
    void* m_pListForceMixChnNo;       //Ҫǿ�Ƽ���������б�
    void* m_pListMissMixerChnNo;      //û�н���������б�
    void* m_pListComMixChnNo;         //��ͨͨ���б��������������û�н���
    
    void* m_ptLockProcess;            //�߳���
    void* m_ptLock;                   //�߳���   

    void* m_ptCond; 
    l32 m_l32Period; 
    l32 m_l32Delay;
    BOOL32 m_bNmode;
    u32	m_dwNmodeNum;	//ʵ��Nģʽ�����·���
    u8 m_u8Depth;
    u8 m_u8SphChn;
    u8 m_u8SphChnBak;

	u8 m_byMixNum;

//
    TMixChnInfo     m_tMixChId[MAX_MIXER_DEPTH];  //��¼��������ͨ��id
    TMixChnInfo     m_tSaveMixChId[MAX_MIXER_DEPTH];  //�ϴμ�¼����ͨ��id
    u32     m_dwChSlientTime[MAX_MIXER_DEPTH]; //����ͨ����ʱ
    u32    m_dwKeepTime;  //ͨ������ʱ��
    u32     m_dwMixChNum;  //��ǰ�������
    u32     m_dwSaveMixChNum;  //�ϴε�ǰ�������
    u32    m_pdwChEXite[AUDMIX_MAX_CHANNEL_USR];
    BOOL32 GetOneFreeMixCh(u32& dwMixChId,BOOL32 bLast = FALSE);   //ȡ��һ·���еĻ�����ͨ��, ���ػ�����ͨ��id
    BOOL32  ChIsInMixer(u32 dwChid, u32& dwMixChId, BOOL32 bLast = FALSE) ;   //��ѯͨ�����Ƿ��Ѿ��ڻ�������
   void  PreMixProc(); //����ǰ����
   BOOL32 MixerIsChanged();
    TMixerCallBack m_tMixerCallBack;
    u32 m_dwContext;
	u32 m_TimerID;
    BOOL32 m_bActive;    
    
    void* ClearNumList(void* ptParam);
    void* RemoveNumListNode(void* ptParam, u8 u8Data);
	TMixerGroupStatus m_tMixStatus;
    u8 m_dwId;
    u32 m_dwBitRate;
	u32 m_dwDecType;
	l32 m_l32Channel;
};
class CAudioAdaptorPro;
class CODECWRAPPER_API CAudioAdaptor
{
public:
	CAudioAdaptor();
	~CAudioAdaptor();

	//����
	u16 Create();
	//����
	u16 Destroy();
	
	u16 StartAdaptor(BOOL32 bStart=TRUE);
	
	//ͨ������������
	//u32 dwChID��ͨ����Ϊ0 ~ AUDIO_ADAPTOR_MAX_CHNL-1
	u16 AddChannel(u32 dwChID);
	
	//ͨ���˳�������
	//u32 dwChID��ͨ����Ϊ0 ~ AUDIO_ADAPTOR_MAX_CHNL-1
	u16 DelChannel(u32 dwChID);
	
	//��������Ƶ����������Ƶ�������
	//��addchannel�������ñ������
	//����ͨ���ţ�0 ~ AUDIO_ADAPTOR_MAX_CHNL-1
	//���������0~AUDIO_ADAPTOR_MAX_ENCNUM-1
	//dwChID��ʾ�ڼ�·������0 ~ AUDIO_ADAPTOR_MAX_CHNL-1��dwEncIndex��ʾ�����ı���·��0~AUDIO_ADAPTOR_MAX_ENCNUM-1
	u16 SetAudEncParam(u32 dwChID, u32 dwEncIndex, u32 dwEncType, u32 dwEncMode);
	
	//������Ƶ��������
	u16 SetAudDecData(u32 dwChID, const PFRAMEHDR pFrmHdr);
	
	//��Ƶ���������AAC_LC/LD��Ч
	u16 SetAudioDecParam(u32 dwChID, u32 dwDecType, u32 dwDecMode);
	
	//������Ƶ����ص�
	//AUDADAPTENCFRAMECALLBACK fAudEncCB���ص�����ָ�룻void* pContext������
	u16 SetAudEncDataCallBack(AudAdapterCallBack fAudAdaptEncCB, void* pContext);

	//���ÿ��ջ�����
	u16 SetMixerRcvBufs(u32 dwFastRcvBufs);

	//ȡ������������ͨ����ǰ״̬
	u16 GetAdaptChnlStatus(u32 dwChID, TAudAdapterStatus& tDecChnlStatus,s8 ** pchTypeName=NULL, s8 **pchModeName=NULL);
	//ȡ������������ͨ����ǰ״̬
	//dwChID��ʾ�ڼ�·������0 ~ AUDIO_ADAPTOR_MAX_CHNL-1��dwEncIndex��ʾ�����ı���·��0~AUDIO_ADAPTOR_MAX_ENCNUM-1
	u16 GetAdaptEncChnlStatus(u32 dwChID, u32 dwEncIndex, TAudAdapterStatus& tEncChnlStatus,s8 **pTypeName=NULL, s8 **pModeName=NULL);
	
	//ȡ������������ͨ���ŵ�ǰͳ��
	u16 GetAdaptChnlStatis(u32 dwChID, TKdvDecStatis& tDecChnlStatis);
	//ȡ������������ͨ���ŵ�ǰͳ��
	//dwChID��ʾ�ڼ�·������0 ~ AUDIO_ADAPTOR_MAX_CHNL-1��dwEncIndex��ʾ�����ı���·��0~AUDIO_ADAPTOR_MAX_ENCNUM-1
	u16 GetAdaptEncChStatis(u32 dwChID, u32 dwEncIndex, TKdvEncStatis& tEncChnlStatis);

	BOOL32 SaveDecIn(l32 l32ChnNo, l32 l32FrameNum);

	BOOL32 SaveDecOut(l32 l32ChnNo, l32 l32FrameNum);

	BOOL32 SaveEncIn(l32 l32ChnNo, l32 l32EncId, l32 l32FrameNum);

	BOOL32 SaveEncOut(l32 l32ChnNo, l32 l32EncId, l32 l32FrameNum);
private:
	CAudioAdaptorPro *m_pAudioAdpPro;	//��Ƶ���䴦��
};
#endif /*end of _AUDIO_MIXER_GPU_H_*/
