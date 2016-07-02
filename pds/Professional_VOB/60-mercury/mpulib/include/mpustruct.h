/*****************************************************************************
   ģ����      : mpulib
   �ļ���      : mpustruct.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: mpulib�ṹ�ͻص����� ͷ�ļ�
   ����        : ����
   �汾        : V4.6  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��       �汾        �޸���      �޸�����
   2009/3/14    4.6         �ű���      ע��
******************************************************************************/
#ifndef _MPUSTRUCT_H_
#define _MPUSTRUCT_H_

#include "mcustruct.h"
#include "eqpcfg.h"
#include "kdvmulpic_hd.h"
#include "kdvmedianet.h"
#include "kdvadapter_hd.h"

extern s8	MPULIB_CFG_FILE[KDV_MAX_PATH];

void CBMpuBasRecvFrame( PFRAMEHDR pFrmHdr, u32 dwContext );
void CBMpuBasSendFrame( PTFrameHeader pFrmHdr, void* dwContext );
void CBMpuDVmpRecvFrame( PFRAMEHDR pFrmHdr, u32 dwContext );
void CBMpuSVmpRecvFrame( PFRAMEHDR pFrmHdr, u32 dwContext );
void CBMpuEVpuRecvFrame( PFRAMEHDR pFrmHdr, u32 dwContext );
void VMPCALLBACK( PTFrameHeader pFrameInfo, void* pContext );
void CBMpuBasAudRecvRtpPk(TRtpPack *pRtpPack, u32 dwContext);

#define MIN_BITRATE_OUTPUT             (u16)128
#define MPU_CONNETC_TIMEOUT            (u16)3*1000     // ���ӳ�ʱֵ3s
#define MPU_REGISTER_TIMEOUT           (u16)5*1000     // ע�ᳬʱֵ5s
#define CHECK_IFRAME_INTERVAL          (u16)1000       // ���ؼ�֡ʱ����(ms)
#define FIRST_REGACK                   (u8)1           // ��һ���յ�ע��Ack
#define PORT_SNDBIND                   20000
#define VMP_SMOOTH_OPEN	               (u8)1

#ifdef WIN32//songkun,20110628,����MPU WIN32 B�����
    #define MAXNUM_MPUSVMP_CHANNEL		 (u8)4
#else
    #ifndef _MPUB_256_
        #define MAXNUM_MPUSVMP_CHANNEL		 (u8)3
    #else
        #define MAXNUM_MPUSVMP_CHANNEL		 (u8)4
    #endif
#endif

#define  MAXNUM_VMPDEFINE_PIC MAXNUM_MPUSVMP_MEMBER

struct TVmpMemPicRoute
{
private:
	s8 m_szVmpMemAlias[VALIDLEN_ALIAS+1];
	s8 m_szVmpMemPicRoute[255];
public:
	TVmpMemPicRoute()
	{
		memset(m_szVmpMemAlias, 0, sizeof(m_szVmpMemAlias));
		memset(m_szVmpMemPicRoute, 0, sizeof(m_szVmpMemPicRoute));
	}	
	
	void SetRoute(const s8* pszRoute)
    {
        memset( m_szVmpMemPicRoute, 0, sizeof(m_szVmpMemPicRoute));
        strncpy( m_szVmpMemPicRoute, pszRoute,  sizeof(m_szVmpMemPicRoute));
		m_szVmpMemPicRoute[sizeof(m_szVmpMemPicRoute)-1] = '\0';
    }
	
	const s8* GetRoute() const
    {
        return m_szVmpMemPicRoute;
    }
	
	void SetVmpMemAlias(const s8* pszMemAlias)
    {
        memset( m_szVmpMemAlias, 0, sizeof(m_szVmpMemAlias));
        strncpy( m_szVmpMemAlias, pszMemAlias,  sizeof(m_szVmpMemAlias));
		m_szVmpMemAlias[sizeof(m_szVmpMemAlias)-1] = '\0';
    }
	
	const s8* GetVmpMemAlias() const
    {
        return m_szVmpMemAlias;
    }
};

struct TDebugVal
{
protected:
    BOOL32          m_bEnableBitrateCheat;   // �Ƿ����������������
    u16             m_wDefault;              // ȱʡ�µ��İٷֱ�
    u32             m_dwAudioDeferNum;       // ��Ƶ�ӳٵ�֡��
	s32             m_nVidEncHWMode;        //  [11/9/2009 pengjie] BAS ͼ���߲�ƥ��ʱ����1���ñߣ�0���Ӻڱ�
	s32				m_nVidSDMode;	    	//[nizhijun 2010/11/9] VMP//ģʽ0:�Ӻڱߣ�1:�ñߣ�2:�ǵȱ����죬Ĭ��0
	u32				m_dwVmpIframeInterval;		 //[nizhijun 2011/03/21] VMP��ʱ���ؼ�֡����ʱ��(��λ������)
	u32				m_dwBasIframeInterval;		 //[nizhijun 2011/03/21] BAS��ʱ���ؼ�֡����ʱ��(��λ������)
	TVmpMemPicRoute	m_atVmpMemPic[MAXNUM_VMPDEFINE_PIC];
public:
    void   SetEnableBitrateCheat( BOOL32 bEnable )
    {
        m_bEnableBitrateCheat = bEnable;
    }
    BOOL32 IsEnableBitrateCheat() const
    {
        return m_bEnableBitrateCheat;
    }

    u16    GetDecRateDebug( u16 wOriginRate ) const
    {
        // û���򷵻�ȱʡֵ
        wOriginRate = wOriginRate * m_wDefault / 100;

        if ( wOriginRate > MIN_BITRATE_OUTPUT )
        {
            return wOriginRate;
        }
        else
        {
            return MIN_BITRATE_OUTPUT;
        }
    }
    void   SetDefaultRate( u16 wPctRate )
    {
        m_wDefault = wPctRate;
    }

	u16    GetOrigRate(u16 wDbgRate) const
    {
		if( m_wDefault == 0 )
		{
			return wDbgRate;
		}
        return (wDbgRate * 100 / m_wDefault);
    }

    void   SetAudioDeferNum(u32 dwDeferNum)
    {
        //����������Ϊ0
        if (0 == dwDeferNum)
        {
            dwDeferNum = 1;
        }
        m_dwAudioDeferNum = htonl(dwDeferNum);
    }

    u32    GetAudioDeferNum(void)
    {
        return ntohl(m_dwAudioDeferNum);
    }

	void   SetVidEncHWMode(s32 nMode)
    {
        m_nVidEncHWMode = htonl(nMode);
    }

	u32    GetVidEncHWMode(void)
    {
        return ntohl( m_nVidEncHWMode );
    }

	void	SetVidSDMode(s32 nMode)
	{
		m_nVidSDMode = htonl(nMode);
	}
	
	u32		GetVidSDMode(void)
	{
		return ntohl(m_nVidSDMode);
	}
	
	void  SetVmpIframeInterval(u32 dwIframeInterval) { m_dwVmpIframeInterval = dwIframeInterval; }
    u32   GetVmpIframeInterval(void) { return m_dwVmpIframeInterval; }
	
	void  SetBasIframeInterval(u32 dwIframeInterval) { m_dwBasIframeInterval = dwIframeInterval; }
    u32   GetBasIframeInterval(void) { return m_dwBasIframeInterval; }

	void SetRoute(u8 byIndex, const s8* pszRoute)
    {
		if (byIndex >= MAXNUM_VMPDEFINE_PIC || NULL == pszRoute)
		{
			return;
		}
        m_atVmpMemPic[byIndex].SetRoute(pszRoute);
    }
	
	const s8* GetVmpMemRoute(u8 byIndex) const
    {
        return m_atVmpMemPic[byIndex].GetRoute();
    }
	
	void SetVmpMemAlias(u8 byIndex, const s8* pszMemAlias)
    {
		if (byIndex >= MAXNUM_VMPDEFINE_PIC || NULL == pszMemAlias)
		{
			return;
		}
		m_atVmpMemPic[byIndex].SetVmpMemAlias(pszMemAlias);
    }
	
	const s8* GetVmpMemAlias(u8 byIndex) const
    {
        return m_atVmpMemPic[byIndex].GetVmpMemAlias();
    }


	void Print()
	{
		OspPrintf(TRUE,FALSE,"IsEnableBitrateCheat: %d\n",m_bEnableBitrateCheat);
		OspPrintf(TRUE,FALSE,"BiteRates default adjust to: %d\n",m_wDefault);
		OspPrintf(TRUE,FALSE,"BAS:AudioDeferNum: %d\n",GetAudioDeferNum());
		OspPrintf(TRUE,FALSE,"BAS:VidEncHWMode: %d\n",GetVidEncHWMode());
		OspPrintf(TRUE,FALSE,"VMP:VidSDMode: %d\n",GetVidSDMode());
		OspPrintf(TRUE,FALSE,"VMP:IframeInterval: %d\n",GetVmpIframeInterval());
		OspPrintf(TRUE,FALSE,"Bas:IframeInterval: %d\n",GetVmpIframeInterval());
		OspPrintf(TRUE,  FALSE, "VmpPic as follows:\n");
		for (u8 byIndex = 0; byIndex < MAXNUM_VMPDEFINE_PIC; byIndex++)
		{
			OspPrintf(TRUE, FALSE, "Index.%d -> Alias.%s -> Route.%s\n", 
				byIndex,
				m_atVmpMemPic[byIndex].GetVmpMemAlias(),
				m_atVmpMemPic[byIndex].GetRoute());
		}
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

// ����CMpuBasCfg��CMpuBasAdpGroup��TMpuBasChannelInfo��Ϊbas����ṹ����ʱ��EBap��Bas����
class CMpuBasCfg
{
public:
	CMpuBasCfg()
	{
		m_byRegAckNum = 0;
	}

    virtual ~CMpuBasCfg(){}
public:
    u32              m_dwMcuIId;                 // ͨ��MCU.A��ʾ
    u32              m_dwMcuIIdB;                // ͨ��MCU.B��ʾ
    u32              m_dwMcuNode;                // ��MCU.Aͨ�Žڵ��
    u32              m_dwMcuNodeB;               // ��MCU.Bͨ�Žڵ��

    BOOL32           m_bEmbedMcu;                //�Ƿ���Ƕ��MCU.A��
    BOOL32           m_bEmbedMcuB;               //�Ƿ���Ƕ��MCU.B��

    u32              m_dwMcuRcvIp;               // MCU.���յ�ַ
    u16              m_wMcuRcvStartPort;         // MCU.������ʼ�˿ں�

    u8               m_byRegAckNum;              // ��һ��ע��ɹ�
	u32              m_dwMpcSSrc;                // ҵ���ỰУ��ֵ 

	u16				 m_wMTUSize;				 // MTU��С
    TEqpCfg          m_tCfg;                     // ����������
    TEqp             m_tEqp;                     // ���豸
    TPrsTimeSpan     m_tPrsTimeSpan;             // �ش�ʱ����
    TDebugVal        m_tDebugVal;                // ����ֵ	
	BOOL32           m_bIsUseSmoothSend;         // �Ƿ��ƽ�����͵Ŀ��� 
    
    u8               m_byWorkMode;               // ����ʹ��ȫ�ֱ���,�ڴ˱���WorkMode

public:
    void FreeStatusDataA( void );
    void FreeStatusDataB( void );

    void ReadDebugValues();
    BOOL32 IsEnableCheat() const
    {
        return m_tDebugVal.IsEnableBitrateCheat();
    }
    u16 GetDecRateDebug( u16 wOriginRate ) const
    {
        return m_tDebugVal.GetDecRateDebug( wOriginRate );
    }

	u16 GetOrigRate(u16 wDbgRate) const
    {
        return m_tDebugVal.GetOrigRate(wDbgRate);
    }

    u32 GetAudioDeferNum( void )
    {
        return m_tDebugVal.GetAudioDeferNum();
    }

	s32 GetVidEncHWMode( void )
    {
        return m_tDebugVal.GetVidEncHWMode();
    }

    void GetDefaultParam(u8 byEnctype,TVideoEncParam& TEncparam);

	u32   GetIframeInterval(void) { return m_tDebugVal.GetBasIframeInterval(); }
};

class CMpuBasAdpGroup // ���ܸ��ݵײ�ӿڵ���������
{
private:
	BOOL32           m_bIsStart;
	u32              m_dwNetBand;
	u32              m_dwRtcpBackAddr;   //rtcp�ط���ַ
	u16              m_wRtcpBackPort;    //rtcp�ط��˿�
    u16              m_wLocalStartPort;  //Ϊ���ؽ��յ�rtp�˿ڣ�rtcp�˿�Ϊrtp�˿�+1
    u32              m_dwDestAddr;       //���͵�Ŀ�ĵ�ַ
	u16              m_wDestPort;        //��һ·����Ŀ�Ķ˿ڣ��ڶ�·���Ͷ˿�Ϊ��һ·����Ŀ�Ķ˿� + 10
    	
    TVideoEncParam   m_atVidEncParam[MAXNUM_MPU_H_VOUTPUT]; //�������
	TMediaEncrypt    m_tMediaDec;        //���ܲ���
	TMediaEncrypt    m_atMediaEnc[MAXNUM_MPU_H_VOUTPUT];    //���ܲ���,����ܲ���һ�£������õ���
	TDoublePayload   m_tDbPayload;       //�غ�
	BOOL32           m_bNeedRS;          //�Ƿ���Ҫ�ش�
	TRSParam         m_tRsParam;         //�ش�����
	u16              m_awBufTime[MAXNUM_MPU_H_VOUTPUT];     //���ͻ���ʱ��

    u8               m_byAudDecPT;       //��ת��Ƶ���غ�
    u8               m_byAdpMode;        //�����л���ʶ�������õ�

	//FIXME����ʱ�ſ����Ժ��ṩ�ӿ�
public:
	CKdvMediaRcv*    m_pcMediaRcv;							 //һ������
	CKdvMediaSnd*    m_pcMediaSnd[MAXNUM_MPU_H_VOUTPUT];     //���6·����(mpuΪ2·��mpu_hΪ6·)
	CKdvAdapterGroup m_cAptGrp;								 //һ������
	
public:
	CKdvMediaRcv*    m_pcMediaAudRcv;     //����һ·��Ƶ����		
	CKdvMediaSnd*    m_pcMediaAudSnd[MAXNUM_MPU_H_VOUTPUT];  //����һ·��Ƶ����

public:
	void   SetAdpMode( u8 byType ) { m_byAdpMode = byType; }         
	u8     GetAdpMode( void ) const  { return m_byAdpMode; } 
	
    CMpuBasAdpGroup() : m_dwNetBand(0), m_bIsStart(FALSE), m_byAudDecPT(MEDIA_TYPE_NULL)
	{
	}
	~CMpuBasAdpGroup()
	{
		StopAdapter();
		m_cAptGrp.DestroyGroup();
	}
	BOOL IsStart( void )
	{
		return m_bIsStart;
	}
	BOOL Create( /*u32 dwMaxRcvFrameSize,*/ 
				 u16 wRtcpBackPort,    //rtcp�ط��˿�
			     u16 wLocalStartPort,  //rtp�˿ڣ�rtcp�˿�Ϊrtp�˿�+1
			     u32 dwDestIp,         //����Ŀ��Ip��ַ��
	             u16 dwDestPort,
			   	 u32 dwContext,
                 u8  byChnId );      //��һ·����Ŀ�Ķ˿ڣ��ڶ�·���Ͷ˿�Ϊ��һ·����Ŀ�Ķ˿� + 10
	void ResetDestPort(u16 wNewDstPort)
	{
		m_wDestPort = wNewDstPort;
	}

    //��ʼ����
	BOOL StartAdapter( BOOL32 bStartMediaRcv = FALSE );

	//ֹͣ����
	BOOL StopAdapter( BOOL32 bStopMediaRcv = TRUE );

	//������Ƶ�������
    BOOL SetVideoEncParam( TVideoEncParam * ptVidEncParam, u8 byChnIdx, u8 byVailedChnNum, BOOL32 bSetNetParam = TRUE );

    //������Ƶ���ղ���
    BOOL SetAudioParam( u8 byAudDecType, u8 byChnIdx);

    //������Ƶ�������key�ִ��Լ� ����ģʽ Aes or Des
    BOOL SetVidEncryptKey( const TMediaEncrypt& tMediaEnc , u8 byVailedChnNum )
	{
		if (0 == byVailedChnNum ||byVailedChnNum > MAXNUM_MPU_H_VOUTPUT )
		{
			OspPrintf(TRUE, FALSE, "[SetVidEncryptKey] byVailedChnNum is unexpected.%d!\n", byVailedChnNum);
		}

		u8  aEncKey[MAXLEN_KEY];
		s32 iLen = 0;
		const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptKey( aEncKey, &iLen );
		for (u8 byIdx = 0; byIdx < byVailedChnNum; byIdx++)
		{
			m_pcMediaSnd[byIdx]->SetEncryptKey( (s8*)aEncKey, iLen, const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptMode() );
		}
		//m_pcMediaSnd[0]->SetEncryptKey( (s8*)aEncKey, iLen, const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptMode() );
		return TRUE;
	}

    //���õڶ�·��Ƶ�������key�ִ��Լ� ����ģʽ Aes or DesBOOL SetSecVidEncryptKey(const TMediaEncrypt& tMediaEnc)
    BOOL SetSecVidEncryptKey( const TMediaEncrypt& tMediaEnc )    
	{
		u8  aEncKey[MAXLEN_KEY];
		s32 iLen = 0;
		const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptKey( aEncKey, &iLen );
		m_pcMediaSnd[1]->SetEncryptKey( (s8*)aEncKey, iLen, const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptMode() );
		return TRUE;
	}

	//���� H.263+/H.264 �ȶ�̬��Ƶ�غɵ� Playloadֵ
    BOOL SetVideoActivePT( u8 byRmtActivePT, u8 byRealPT )
	{
        m_pcMediaRcv->SetActivePT( byRmtActivePT, byRealPT );
		return TRUE;
	}

    void SetKeyandPT( u8* abyKeyBuf, u8 byKenLen, u8 byEncryptMode, u8 *pbySndActive, u8 byVailedOutNum, u8 byActivePT = 0, u8 byRealPT = 0/*u8 bySndActivePT0 = 0, u8 bySndActivePT1 = 0*/ )
    {
		if (0 == byVailedOutNum)
		{
			OspPrintf(TRUE, FALSE, "[SetKeyandPT] byVailedOutNum is 0\n");
			return;
		}
        // ��
        m_pcMediaRcv->SetActivePT( byActivePT, byRealPT );
        m_pcMediaRcv->SetDecryptKey( (s8*)abyKeyBuf, byKenLen, byEncryptMode );
        // ��
		for (u8 byIdx = 0; byIdx < byVailedOutNum; byIdx++)
		{		
			 m_pcMediaSnd[byIdx]->SetActivePT(pbySndActive[byIdx]);
			 m_pcMediaSnd[byIdx]->SetEncryptKey( (s8*)abyKeyBuf, byKenLen, byEncryptMode );
		}
//         m_pcMediaSnd[0]->SetActivePT( bySndActivePT0 );
//         m_pcMediaSnd[1]->SetActivePT( bySndActivePT1 );
//         m_pcMediaSnd[0]->SetEncryptKey( (s8*)abyKeyBuf, byKenLen, byEncryptMode );
//         m_pcMediaSnd[1]->SetEncryptKey( (s8*)abyKeyBuf, byKenLen, byEncryptMode );
    }

	/*����ͼ�����������ش�����*/
	BOOL  SetNetRecvFeedbackVideoParam( TRSParam tNetRSParam, BOOL32 bRepeatSnd = FALSE );

	/*����ͼ������緢���ش�����*/
	BOOL  SetNetSendFeedbackVideoParam( u16 wBufTimeSpan, u8 byVailedOutNum, BOOL32 bRepeatSnd = FALSE );

	/*���õڶ�·����ͼ������緢���ش�����*/
	BOOL  SetNetSecSendFeedbackVideoParam( u16 wBufTimeSpan, BOOL32 bRepeatSnd = FALSE );

    // ƽ������֧��
// 	void SetSmoothSendRule( void );
// 	void ClearSmoothSendRule( void );
	//  void StopSmoothSend();	
	
	// [pengjie 2010/12/30] ���ã����������������ƽ�����͹���, FALSE:���, TRUE:����
	void SetSmoothSendRule( BOOL32 bNeedSet );	

    //����ؼ�֡
	BOOL SetFastUpdata( u8 byChnIdx );

	//�õ���Ƶͨ����ͳ����Ϣ
	BOOL GetVideoChannelStatis( u8 byChnNo, TAdapterChannelStatis &tAdapterChannelStatis );

    void ShowChnInfo( void );
	
	// Ϊ�������������緢��IP��ַ
	void ModNetSndIpAddr(u32 dwSndIp);

	//����bas����ģʽ��ȡ���ͨ����
	BOOL32 GetOutputChnNumAcd2WorkMode(u8 byWorkMode, u8 &byOutputChnNum);

private:
	BOOL SetVideoEncSend( u8  byIdx,            //���Ͷ�������
						  u32 dwNetBand,
		                  u16 wLoacalStartPort, //Ϊ���ؽ��յ�rtp�˿ڣ�rtcp�˿�Ϊrtp�˿�+1
			              u32 dwDestIp, 
                          u16 wDestPort );      //��һ·����Ŀ�Ķ˿ڣ��ڶ�·���Ͷ˿�Ϊ��һ·����Ŀ�Ķ˿� + 10)

// 	BOOL SetSecVideoEncSend( u32 dwNetBand,
// 		                     u16 wLoacalStartPort, //Ϊ���ؽ��յ�rtp�˿ڣ�rtcp�˿�Ϊrtp�˿�+1
// 			                 u32 dwDestIp,         //����Ŀ��Ip��ַ��
// 	                         u16 dwDestPort );     //��һ·����Ŀ�Ķ˿ڣ��ڶ�·���Ͷ˿�Ϊ��һ·����Ŀ�Ķ˿� + 10)
    
    BOOL SetAudioSend( u8  byIdx,				//���Ͷ�������
					   u32 dwNetBand,
                       u16 wLocalStartPort,     //Ϊ���ؽ��յ�rtp�˿ڣ�rtcp�˿�Ϊrtp�˿�+1
                       u32 dwDestIp,            //����Ŀ��Ip��ַ��
                       u16 wDestPort);          //��һ·��Ƶ����Ŀ�Ķ˿ڣ���Ƶ��Ӧ�˿ڣ�2���ڶ�·���Ͷ˿�Ϊ��һ·����Ŀ�Ķ˿� + 10)

//     BOOL SetSecAudioSend( u32 dwNetBand,
//                           u16 wLocalStartPort,  //Ϊ���ؽ��յ�rtp�˿ڣ�rtcp�˿�Ϊrtp�˿�+1
//                           u32 dwDestIp,         //����Ŀ��Ip��ַ��
//                           u16 wDestPort);       //��һ·��Ƶ����Ŀ�Ķ˿ڣ���Ƶ��Ӧ�˿ڣ�2���ڶ�·���Ͷ˿�Ϊ��һ·����Ŀ�Ķ˿� + 10)
};

struct TMpuBasChannelInfo
{
    CConfId             m_cChnConfId;
    u8                  m_byChnConfType;    //�����ڸ��廹�Ǳ������,���ܻ��õ�
    u8                  m_byCurrentState;   //���ã�ԤԼ���Ѿ�ʹ�á����ܻ��õ�

    u32                 m_dwLastFUPTick;    //����յ��ؼ�֡�����Tick��

    THDBasVidChnStatus  m_tChnStatus;       //�Ƿ�Σ�գ�����ṹ�彫��ΪChnnlNotify����Ϣ��

public:
    TMpuBasChannelInfo()
    {
        m_dwLastFUPTick = 0;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

/***********************************************************************************************************************************/
/*  MEDIA_TYPE_H262,          0,            300,       31,      2,       2048,       0,       0,     25,    5,       576,     720, */
/***********************************************************************************************************************************/
const u16 EnVidFmt[6][12]=
{
//����               ѹ�����ʿ��Ʋ��� ���ؼ�֡ ������� ��С���� ���������K ͼ����ʽ �ɼ��˿� ֡��       ͼ������         ��Ƶ�߶� ��Ƶ���
//mpeg4
 MEDIA_TYPE_MP4,           0,            25,        31,      2,       1200,       0,       0,     25,    1/*QC_MODE_QUALITY*/,       576,     720,
//261
 MEDIA_TYPE_H261,          0,            25,        31,      2,       1200,       0,       0,     25,    1/*QC_MODE_QUALITY*/,       576,     720,
//262(mpeg-2)
 MEDIA_TYPE_H262,          0,            25,        31,      2,       2048,       0,       0,     25,    1/*QC_MODE_QUALITY*/,       576,     720,
//263
 MEDIA_TYPE_H263,          1,            25,        31,      2,       1200,       0,       0,     25,    1/*QC_MODE_QUALITY*/,       576,     720,
 //h263+
 MEDIA_TYPE_H263PLUS,      0,            300,       31,      2,       1200,       0,       0,     25,    1/*QC_MODE_QUALITY*/,       576,     720,
//h264
 MEDIA_TYPE_H264,          0,            300,       51,      2,       2048,       0,       0,     25,    1/*QC_MODE_QUALITY*/,       576,     720
};

// ����Ϊvmp����ṹ
class TMpuVmpCfg
{
public:
	//TMulPicParam m_tMulPicParam;   //ͼ���������
    u32  m_adwMaxSendBand[MAXNUM_MPUSVMP_CHANNEL];        //������緢�ʹ���
    
public:    
    // ͼ��������
	TVideoEncParam m_tVideoEncParam[MAXNUM_MPUSVMP_CHANNEL];
    TDebugVal m_tDebugVal;           //����ֵ

public:
    TMpuVmpCfg(){}
    ~TMpuVmpCfg(){}
	void PrintEncParam( u8 byVideoIdx ) const;
	void GetDefaultParam( u8 byEnctype, TVideoEncParam& TEncparam, u32& byNetBand );
    void ReadDebugValues();
    u8   GetVmpChlNumByStyle(u8 byStyle);

    BOOL32 IsEnableBitrateCheat() const
    {
        return m_tDebugVal.IsEnableBitrateCheat();
    }
    u16 GetDecRateDebug( u16 wOriginRate ) const
    {
        return m_tDebugVal.GetDecRateDebug( wOriginRate );
    }
    u32 GetAudioDeferNum( void )
    {
        return m_tDebugVal.GetAudioDeferNum();
    }
	
	u16 GetOrigRate(u16 wDbgRate) const
    {
        return m_tDebugVal.GetOrigRate(wDbgRate);
    }

	u32 GetVidSDMode(void) 
	{
		return m_tDebugVal.GetVidSDMode();
	}

	u32 GetIframeInterval(void){ return m_tDebugVal.GetVmpIframeInterval(); }

	const s8* GetVmpMemAlias(u8 byIndex) const
    {
        return m_tDebugVal.GetVmpMemAlias(byIndex);
    }
	
	const s8* GetVmpMemRoute(u8 byIndex) const
    {
        return m_tDebugVal.GetVmpMemRoute(byIndex);
    }
	
	BOOL32 TableMemoryFree( void **ppMem, u32 dwEntryNum );

	void Print()
	{
		m_tDebugVal.Print();
	}
};

/************************************************************************/
/*                                                                      */
/*                       ����ͬ��������ݽṹ                           */
/*                                                                      */
/************************************************************************/

#define MAXNUM_BUF_LEN                  (u16)(1024 * 2)
#define MAXNUM_BUF_FRM                  (u16)(100)

//2k�ľ�̬buf
struct TBuf
{
public:
    void SetNull(void)
    {
        memset(&m_abyBuf, 0, sizeof(m_abyBuf));
        m_dwLen = 0;
    }
public:
    u32 m_dwLen;
    u8  m_abyBuf[MAXNUM_BUF_LEN];
};

//ĳ�����������
struct TRtpPkQueue
{
public:
    TRtpPkQueue::TRtpPkQueue()
    {
        memset(this, 0, sizeof(TRtpPkQueue));
    }

    //  �����ڴ�
	//  pengjie[8/7/2009] �޸��������ڴ�ʧ�ܵĴ���
    void Init(u32 dwLen)
    {
        if ( 0 == dwLen ) 
        {
            OspPrintf( TRUE, FALSE, "[TRtpPkQueue::Init] err: Len.%d, Init failed !\n", dwLen );
            return;
        }

        m_ptBuf   = new TBuf[dwLen];
        m_ptRtpPk = new TRtpPack[dwLen];
		if((NULL != m_ptBuf) && (NULL != m_ptRtpPk))
		{
			memset(m_ptBuf, 0, sizeof(TBuf) * dwLen);
			memset(m_ptRtpPk, 0, sizeof(TRtpPack) * dwLen);
			m_dwLen = dwLen;
		}
		else
		{
            delete [] m_ptBuf; 
			delete [] m_ptRtpPk;
            m_ptBuf = NULL;  
            m_ptRtpPk = NULL;

            OspPrintf( TRUE, FALSE, "[TRtpPkQueue::Init] Init failed !\n" );
			return;
		}
		return;
    }

    //�ͷ��ڴ�
    void Quit(void)
    {
        if ( NULL != m_ptBuf ) 
        {
            delete [] m_ptBuf;
            m_ptBuf = NULL;
        }
        if ( NULL != m_ptRtpPk)
        {
            delete [] m_ptRtpPk;
            m_ptRtpPk = NULL;
        }
        m_dwLen = 0;
        m_dwHead = 0;
        m_dwTail = 0;
        return;
    }

    //����buf����β
    BOOL32 IncBuf( u8 *pUdpData, u32 dwUdpLen, TRtpPack &tRtpPk )
    {
        /*
        //Ϊ����߻ص�����Ч�ʣ���Ӱ�칦�ܣ�ע��֮
        if ( IsQueueFull() )
        {
            OspPrintf(TRUE, FALSE, "[TRtpPkQueue::IncBuf] Buffer Full!\n");
            return FALSE;
        }*/

        if (!IsQueueNull())
        {
            //��������������յ�ǰ����
            if (tRtpPk.m_dwSSRC != m_ptRtpPk[m_dwHead].m_dwSSRC)
            {
                Clear();
                OspPrintf(TRUE, FALSE, "[TRtpPkQueue][IncBuf] new SSRC.%d !eq HeadSSRC.%d, clear queue!\n",
                                        tRtpPk.m_dwSSRC,
                                        m_ptRtpPk[m_dwHead].m_dwSSRC);
            }
        }

        if (dwUdpLen >= MAXNUM_BUF_LEN)
        {
            OspPrintf(TRUE, FALSE, "[TRtpPkQueue::IncBuf] new Udp data len.%d is unexpected!\n", dwUdpLen);
            return FALSE;
        }

        m_ptBuf[m_dwTail].m_dwLen = min(dwUdpLen, MAXNUM_BUF_LEN);
        memcpy(&m_ptBuf[m_dwTail].m_abyBuf, pUdpData, min(dwUdpLen, MAXNUM_BUF_LEN));
        m_ptRtpPk[m_dwTail] = tRtpPk;

        m_dwTail = (m_dwTail + 1) % m_dwLen;

        //OspPrintf(TRUE, FALSE, "[TRtpPkQueue::IncBuf] head.%d, tail.%d\n", m_dwHead, m_dwTail);
        return TRUE;
    }

    //����������ȡ��ɾ����ͷ��buf
    BOOL32 DecBuf( u8 *pUdpData, u32 &dwUdpLen, TRtpPack &tRtpPk )
    {
        if (IsQueueNull())
        {
            OspPrintf(TRUE, FALSE, "[TRtpPkQueue::DecBuf] no udp data in exist, ignore it!\n");
            return FALSE;
        }
        /*
        if (dwUdpLen == 0 || dwUdpLen < m_ptBuf[m_dwHead].m_dwLen)
        {
            OspPrintf(TRUE, FALSE, "[TRtpPkQueue::DecBuf] udp buf len.%d is less than.%d, ignore it!\n",
                                    dwUdpLen, m_ptBuf[m_dwHead].m_dwLen);
            return FALSE;
        }*/

        dwUdpLen = m_ptBuf[m_dwHead].m_dwLen;
        memcpy(pUdpData, m_ptBuf[m_dwHead].m_abyBuf, dwUdpLen);
        tRtpPk = m_ptRtpPk[m_dwHead];

        m_ptBuf[m_dwHead].SetNull();

        m_dwHead = (m_dwHead + 1) % m_dwLen;
        
        //OspPrintf(TRUE, FALSE, "[TRtpPkQueue::DecBuf] head.%d been remove, tail.%d\n", m_dwHead, m_dwTail);
        
        return TRUE;
    }

    BOOL32 IsQueueFull(void)
    {
        BOOL32 bFull = m_ptBuf[m_dwTail].m_dwLen != 0;
        /*
        OspPrintf(TRUE, FALSE, "[TRtpPkQueue::IsQueueFull] head.%d, tail.%d, len.%d, full.%d!\n",
                                m_dwHead, m_dwTail, m_ptBuf[m_dwTail].m_dwLen, bFull);
                                */
        return bFull;
    }
    BOOL32 IsQueueNull(void)
    {
        return m_ptBuf[m_dwHead].m_dwLen == 0;
    }
    u32 GetPackLen(void)
    {
        return m_dwLen;
    }

    void Clear()
    {
        m_dwHead = 0;
        m_dwTail = 0;
        for(u8 byIdx = 0; byIdx < m_dwLen; byIdx++)
        {
            m_ptBuf[byIdx].SetNull();
        }
    }

    void Print()
    {
        OspPrintf(TRUE, FALSE, "\nTFrmQueue <H.%d, T.%d, L.%d>:\n\n", m_dwHead, m_dwTail, m_dwLen);

        for (u8 byIdx = 0; byIdx < m_dwLen; byIdx ++)
        {
            if (byIdx == m_dwHead && byIdx == m_dwTail)
            {
                OspPrintf(TRUE, FALSE, "\t[%d] m_pBufLen.%d, Type.%d, Seq.%d  <-- HEAD/TAIL\n", 
                    byIdx,
                    m_ptBuf[byIdx].m_dwLen,
                    m_ptRtpPk[byIdx].m_byPayload,
                    m_ptRtpPk[byIdx].m_wSequence );
            }
            else if (byIdx == m_dwHead)
            {
                OspPrintf(TRUE, FALSE, "\t[%d] m_pBufLen.%d, Type.%d, Seq.%d <-- HEAD\n", 
                    byIdx,
                    m_ptBuf[byIdx].m_dwLen,
                    m_ptRtpPk[byIdx].m_byPayload,
                    m_ptRtpPk[byIdx].m_wSequence );
            }
            else if (byIdx == m_dwTail)
            {
                OspPrintf(TRUE, FALSE, "\t[%d] m_pBufLen.%d, Type.%d, Seq.%d  <-- TAIL\n", 
                    byIdx,
                    m_ptBuf[byIdx].m_dwLen,
                    m_ptRtpPk[byIdx].m_byPayload,
                    m_ptRtpPk[byIdx].m_wSequence );
            }
            else
            {
                OspPrintf(TRUE, FALSE, "\t[%d] m_pBufLen.%d, Type.%d, Seq.%d \n", 
                    byIdx,
                    m_ptBuf[byIdx].m_dwLen,
                    m_ptRtpPk[byIdx].m_byPayload,
                    m_ptRtpPk[byIdx].m_wSequence );
            }
        }
    }
private:

    u32 m_dwLen;
    u32 m_dwHead;
    u32 m_dwTail;
    TBuf *m_ptBuf;
    TRtpPack *m_ptRtpPk;
};

#endif // !_MPUSTRUCT_H_

//END OF FILE

