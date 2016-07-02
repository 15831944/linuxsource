/*****************************************************************************
   ģ����      : BAS-HD
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

#include "eqpcfg.h"

#include "kdvmedianet.h"
#include "kdvadapter_hd.h"
#ifdef _LINUX_
#include "libsswrap.h"
#endif


#define STATUS_CIF				(u8)1
#define STATUS_CIF_ADJ2HD		(u8)2
#define STATUS_HD				(u8)3
#define STATUS_HD_ADJ2CIF		(u8)4


#define MAXNUM_RCV              (u8)4   //��·����

//#define MAXNUM_BASHD_CHANNEL    (u8)2
#define MAXNUM_BASHD_CHANNEL    (u8)1   //һ��ʵ��

#define MAX_VIDEO_FRAME_SIZE (512 * 1024)
#define QC_MODE_QUALITY      ((u8)0x1)
#define FRAME_RATE           (25)
#define PORT_SNDBIND         20000

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
    MEDIA_TYPE_H264,          0,            300,       51,      10,      2048,       0,       0,     25,    QC_MODE_QUALITY,       576,     720
};

//��������
class CHdAdpGroup
{
private:
	BOOL32           m_bIsVga;
	BOOL32           m_bIsStart;
	u32              m_dwNetBand;
	u32              m_dwRtcpBackAddr;   //rtcp�ط���ַ
	u16              m_wRtcpBackPort;    //rtcp�ط��˿�
    u16              m_wLocalStartPort;  //Ϊ���ؽ��յ�rtp�˿ڣ�rtcp�˿�Ϊrtp�˿�+1
    u32              m_dwDestAddr;       //���͵�Ŀ�ĵ�ַ
	u16              m_wDestPort;        //��һ·����Ŀ�Ķ˿ڣ��ڶ�·���Ͷ˿�Ϊ��һ·����Ŀ�Ķ˿� + 10
    
	
    TVideoEncParam   m_atVidEncParam[2];            //�������
	TMediaEncrypt    m_atMediaDec[MAXNUM_RCV];      //���ܲ���
	TMediaEncrypt    m_atMediaEnc[2];               //���ܲ���
	TDoublePayload   m_atDbPayload[MAXNUM_RCV];     //�غ�
	BOOL32           m_bNeedRS[MAXNUM_RCV];         //�Ƿ���Ҫ�ش�
	TRSParam         m_tRsParam[MAXNUM_RCV];        //�ش�����
	u16              m_awBufTime[2];                //���ͻ���ʱ��
    

	//FIXME����ʱ�ſ����Ժ��ṩ�ӿ�
public:
	CKdvMediaRcv    *m_pcMediaRcv[MAXNUM_RCV];                 //��·����
	CKdvMediaSnd     m_cMediaSnd[2];                //�����·����
	CKdvAdapterGroup m_cAptGrp;                     //һ������


public:
	CHdAdpGroup(BOOL32 bIsVgaGroup = FALSE):m_bIsVga(bIsVgaGroup),
                                            m_dwNetBand(0),
                                            m_bIsStart(FALSE)
	{
	}
	~CHdAdpGroup()
	{
		StopAdapter();
		m_cAptGrp.DestroyGroup();
	}

	BOOL IsVga(void)
	{
		return m_bIsVga;
	}

	BOOL IsStart(void)
	{
		return m_bIsStart;
	}

	BOOL Create(u32                   dwMaxRcvFrameSize, 
				u16                   wRtcpBackPort,    //rtcp�ط��˿�
			    u16                   wLocalStartPort,  //rtp�˿ڣ�rtcp�˿�Ϊrtp�˿�+1
			    u32                   dwDestIp,         //����Ŀ��Ip��ַ��
	            u16                   dwDestPort,
                u8                    byFpgaErrTimes,
				u32				      dwContext);      //��һ·����Ŀ�Ķ˿ڣ��ڶ�·���Ͷ˿�Ϊ��һ·����Ŀ�Ķ˿� + 10
 

	//�˳���
	BOOL DestroyGroup(void)
	{
		m_cAptGrp.DestroyGroup();
		return TRUE;
	}

	//��ʼ����
	BOOL StartAdapter(BOOL32 bStartMediaRcv = FALSE);

	//ֹͣ����
	BOOL StopAdapter(BOOL32 bStopMediaRcv = TRUE);

	//������Ƶ�������
    BOOL SetVideoEncParam(TVideoEncParam * ptVidEncParam, BOOL32 bSetNetParam = TRUE);

    //�ı���Ƶ����������
    BOOL ChangeVideoDecParam(TVideoDecParam * ptVidDecParam);

	//��ȡ��Ƶ�������
    BOOL GetVideoEncParam(TVideoEncParam& tVidEncParam);

	//��Ƶ����������
	BOOL GetSecVideoEncParam(TVideoEncParam& tVidEncParam);
	
	/*����ͼ�����������ش�����*/
	BOOL  SetNetRecvFeedbackVideoParam(TRSParam tNetRSParam, BOOL32 bRepeatSnd = FALSE);

	/*����ͼ������緢���ش�����*/
	BOOL  SetNetSendFeedbackVideoParam(u16 wBufTimeSpan, BOOL32 bRepeatSnd = FALSE);

	/*���õڶ�·����ͼ������緢���ش�����*/
	BOOL  SetNetSecSendFeedbackVideoParam(u16 wBufTimeSpan, BOOL32 bRepeatSnd = FALSE);

	//������Ƶ�������key�ִ��Լ� ����ģʽ Aes or Des
    BOOL SetVidEncryptKey(const TMediaEncrypt& tMediaEnc)
	{
		u8  aEncKey[MAXLEN_KEY];
		s32 iLen = 0;
		const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptKey(aEncKey, &iLen);
		m_cMediaSnd[0].SetEncryptKey((s8*)aEncKey, iLen, GetMediaEncMode(const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptMode()));
		return TRUE;
	}

    //���õڶ�·��Ƶ�������key�ִ��Լ� ����ģʽ Aes or DesBOOL SetSecVidEncryptKey(const TMediaEncrypt& tMediaEnc)
    BOOL SetSecVidEncryptKey(const TMediaEncrypt& tMediaEnc)    
	{
		u8  aEncKey[MAXLEN_KEY];
		s32 iLen = 0;
		const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptKey(aEncKey, &iLen);
		m_cMediaSnd[1].SetEncryptKey((s8*)aEncKey, iLen, GetMediaEncMode(const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptMode()));
		return TRUE;
	}
	
	//���� H.263+/H.264 �ȶ�̬��Ƶ�غɵ� Playloadֵ
    BOOL SetVideoActivePT(u8 byIdx, u8 byRmtActivePT, u8 byRealPT )
	{
        m_pcMediaRcv[byIdx]->SetActivePT(byRmtActivePT, byRealPT);
		return TRUE;
	}

	//���� ��Ƶ����key�ִ� �Լ� ����ģʽ Aes or Des
    BOOL SetVidDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode)
	{
        for(u8 byIdx = 0; byIdx < MAXNUM_RCV; byIdx++)
        {
		    m_atMediaDec[byIdx].SetEncryptKey((u8*)pszKeyBuf, wKeySize);
		    m_atMediaDec[byIdx].SetEncryptMode(byDecryptMode);
		    m_pcMediaRcv[byIdx]->SetDecryptKey(pszKeyBuf, wKeySize, byDecryptMode);
        }
		return TRUE;
	}

    BOOL SetVidDecryptKey(const TMediaEncrypt & tMediaEnc)
    {
        u8  aEncKey[MAXLEN_KEY];
        s32 iLen = 0;
        const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptKey(aEncKey, &iLen);

        for(u8 byIdx = 0; byIdx < MAXNUM_RCV; byIdx++)
        {
            m_atMediaDec[byIdx].SetEncryptKey((u8*)aEncKey, iLen);
            m_atMediaDec[byIdx].SetEncryptMode(GetMediaEncMode(const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptMode()));
            m_pcMediaRcv[byIdx]->SetDecryptKey((s8*)aEncKey, iLen, GetMediaEncMode(const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptMode()));
        }
		return TRUE;
    }

	//����ؼ�֡
	BOOL SetFastUpdata(void);

    // ƽ������֧��
	void SetSmoothSendRule(void);
	void ClearSmoothSendRule(void);
    void StopSmoothSend();	

	//�õ����״̬
    BOOL GetGroupStatus(TAdapterGroupStatus &tAdapterGroupStatus);

	//�õ���Ƶͨ����ͳ����Ϣ
	BOOL GetVideoChannelStatis(u8 byChnNo, TAdapterChannelStatis &tAdapterChannelStatis);

	//��ʾͨ����Ϣ,����·���붼��ӡ����
	void ShowChnInfo(void);

    void UpdateNetParam(u16 wRtcpBackPort,    //rtcp�ط��˿�
                        u16 wLocalStartPort,  //rtp�˿ڣ�rtcp�˿�Ϊrtp�˿�+1
                        u32 dwDestIp,         //����Ŀ��Ip��ַ��
                        u16 dwDestPort);

private:

	BOOL SetVideoEncSend(u32 dwNetBand,
		                 u16 wLoacalStartPort, //Ϊ���ؽ��յ�rtp�˿ڣ�rtcp�˿�Ϊrtp�˿�+1
			             u32 dwDestIp, u16 wDestPort);      //��һ·����Ŀ�Ķ˿ڣ��ڶ�·���Ͷ˿�Ϊ��һ·����Ŀ�Ķ˿� + 10)


	BOOL SetSecVideoEncSend(u32 dwNetBand,
		                    u16 wLoacalStartPort, //Ϊ���ؽ��յ�rtp�˿ڣ�rtcp�˿�Ϊrtp�˿�+1
			                u32 dwDestIp,         //����Ŀ��Ip��ַ��
	                        u16 dwDestPort);      //��һ·����Ŀ�Ķ˿ڣ��ڶ�·���Ͷ˿�Ϊ��һ·����Ŀ�Ķ˿� + 10)

    u8   GetMediaEncMode(u8 byConfEncMode)
    {
        u8 byMediaEncMode = DES_ENCRYPT_MODE;

        switch (byConfEncMode)
        {
        case CONF_ENCRYPTMODE_AES:
            byMediaEncMode = AES_ENCRYPT_MODE;
            break;
        case CONF_ENCRYPTMODE_DES:
        case CONF_ENCRYPTMODE_NONE:
            break;
        default:
            break;
        }
        return byMediaEncMode;
    }
};

class CBasInst : public CInstance
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
	
	TChannelInfo    m_tChnInfo;
	CHdAdpGroup     *m_pAdpCroup;
public:
    CBasInst(  );
    ~CBasInst(  );

    // ----------- ��Ϣ��� ------------
    void InstanceEntry( CMessage* const pMsg );
    void DaemonInstanceEntry( CMessage* const pMsg, CApp* pcApp );
    
    // --------- ��Ϣ������ ----------
	void  DaemonProcPowerOn( CMessage* const pMsg, CApp* pcApp );    //��ʼ��
    void  DaemonProcOspDisconnect( CMessage* const pMsg, CApp* pcApp ); //��������
    void  DaemonProcConnectTimeOut( BOOL32 bIsConnectA );               //���������
    void  DaemonProcRegisterTimeOut( BOOL32 bIsRegiterA );              //ע�������    
    void  DaemonProcMcuRegAck(CMessage* const pMsg, CApp* pcApp );      //����ע��
    void  DaemonProcMcuRegNack(CMessage* const pMsg);
	void  DeamonProcGetMsStatusRsp( CMessage* const pMsg ); 
    void  DaemonProcSetQosCmd( CMessage* const pMsg );                  //����Qosֵ

    void  ProcInitBas();
    void  ProcStopBas();
    void  ProcStartAdptReq( CServMsg& );  //��ʼ����
    void  ProcChangeAdptCmd( CServMsg& ); //�����������
    void  ProcStopAdptReq( CServMsg& );   //ֹͣ����    
	void  ProcFastUpdatePicCmd( CServMsg& );                    //Mcu����Bas��ؼ�֡
    void  ProcTimerNeedIFrame();  //Bas����Mcu���ؼ�֡

	// ----------- ���ܺ��� ------------
    BOOL32 ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode );      //�������Ӻ���
	void Register( BOOL32 bIsRegiterA, u32 dwMcuNode );           //��MCUע��
    s32  SendMsgToMcu( u16 wEvent, CServMsg& cServMsg );
    void  SendChnNotif( BOOL32 bUsed);                            //����ͨ��״̬��Mcu

    void ComplexQos( u8& byValue, u8 byPrior );                       //���Qosֵ
	void ClearInstStatus( void );                                     //��ձ�ʵ�������״̬��Ϣ
	   


    BOOL32 GetMediaType( u8 byType, s8* aszTypeStr ); //ȡý������
	
	//��ʾ��Ϣ
	void StatusShow(void);
    // ����ͨ��״̬
    void SendChnStatus(void);

	void AdpStatusAdjust(void);


    /************************************************************************/
    /*                          VMPHD                                       */
    /************************************************************************/
    
    void MsgStartVidMixProc( CMessage * const pcMsg );            //��ʼ����
    void MsgStopVidMixProc( CMessage * const pcMsg );             //ֹͣ����
    void MsgChangeVidMixParamProc( CMessage * const pcMsg );      //�ı临�ϲ���
    void MsgGetVidMixParamProc( CMessage * const pcMsg );         //��ѯ���ϲ���
    void MsgSetBitRate( CMessage * const pcMsg );
    void ProcSetQosInfo( CMessage * const pcMsg );
    void MsgFastUpdatePicProc(void);

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
    ~CBasCfg(  );
public:
    u32              m_dwMcuIId;                 //ͨ��MCU.A��ʾ
    u32              m_dwMcuIIdB;                //ͨ��MCU.B��ʾ
    u32              m_dwMcuNode;                //��MCU.Aͨ�Žڵ��
    u32              m_dwMcuNodeB;               //��MCU.Bͨ�Žڵ��

    BOOL32           m_bEmbedMcu;                //�Ƿ���Ƕ��MCU.A��
    BOOL32           m_bEmbedMcuB;               //�Ƿ���Ƕ��MCU.B��

    u32              m_dwMcuRcvIp;               //MCU.���յ�ַ
    u16              m_wMcuRcvStartPort;         //MCU.������ʼ�˿ں�

    u8               m_byRegAckNum;              //��һ��ע��ɹ�
	u32              m_dwMpcSSrc;                // guzh [6/12/2007] ҵ���ỰУ��ֵ 
	u16				 m_wMTUSize;				 //MTU��С, zgc, 2007-04-02
    TEqpCfg          m_tCfg;                     //����������
    TEqp             m_tEqp;                     //���豸
    TPrsTimeSpan     m_tPrsTimeSpan;             //�ش�ʱ����

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
void baslogall( s8* pszFmt, ...);
s8*  ipStr( u32 dwIP );	

typedef zTemplate< CBasInst , MAXNUM_BASHD_CHANNEL, CBasCfg > CBasApp;
extern CBasApp g_cBasApp;

#endif //!_BASINST_H_
