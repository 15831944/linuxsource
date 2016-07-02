/*****************************************************************************
   ģ����      : mcu
   �ļ���      : mcuutility.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: MCUҵ���ڲ�ʹ�ýṹ����
   ����        : ������
   �汾        : V3.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/11/7   3.0         ������      ����
   2005/02/19  3.6         ����      �����޸ġ���3.5�汾�ϲ�
******************************************************************************/

#ifndef __MCUUTILITY_H_
#define __MCUUTILITY_H_

#include "osp.h"
#include "mcuconst.h"
#include "mcustruct.h"
#include "channelinfo.h"
#include "vcsstruct.h"
#include "confinfoconvert.h"
#include "mcuinnerstruct.h"

#ifndef SETBITSTATUS
#define SETBITSTATUS(StatusValue, StatusMask, bStatus)  \
    if (bStatus)    StatusValue |= StatusMask;          \
else            StatusValue &= ~StatusMask;

#define GETBITSTATUS(StatusValue, StatusMask)  (0 != (StatusValue&StatusMask))   
#endif

#ifdef _LINUX_
#ifndef _MINIMCU_
#define APPNAME       (LPCSTR)"/usr/bin/mcu"
#else
#ifndef _MCU8000C_
#define APPNAME       (LPCSTR)"/usr/bin/mcu_8000b"
#else
#define APPNAME       (LPCSTR)"/usr/bin/mcu_8000c"
#endif
#endif
#endif

#ifdef _VXWORKS_
#define APPNAME       (LPCSTR)"/mcu"
#endif

#ifdef WIN32
#define APPNAME       (LPCSTR)"mcu.exe"
#endif

// delete ָ��(new)
#define SAFE_DEL_PTR(p)			\
	if(p){						\
		delete p;				\
		p = NULL;				\
	}

// delete����
#define SAFE_DEL_ARRAY(p)		\
	if(p){						\
		delete []p;				\
		p = NULL;				\
	}

// free malloc/alloc�����ָ��
#define SAFE_FREE_PTR(p)		\
	if(p) {						\
		free(p);				\
		p = NULL;				\
	}	

// [9/15/2011 liuxu] ��ӡInstance����Ϣ�ŵĺ궨��
#define MCU_EV_LOG( level, mid, prestr)	\
	do{					\
	const s8* pEvStr = ::OspEventDesc( pcMsg->event );\
	if(pEvStr)				\
	LogPrint(level, mid,  "%s%u: %s in\n", prestr, GetInsID(), pEvStr);		\
	else	\
	LogPrint(level, mid,  "%s%u: %u in\n", prestr, GetInsID(), pcMsg->event);\
	}while(0)


#define  MAXNUM_CHEATMTVIDEOCAP (u8)100         //����������ʱ�����������������������Ƶ��
#define  MAXNUM_CHEATMTAUDIOCAP (u8)20         //����������ʱ�����������������������Ƶ��
#define  MAXNUM_CHEATMTNUM (u8)100         //����������ʱ����������ն˵ĸ���


s8  *StrOfIP( u32 dwIP );
//�ն��߼�ͨ��
struct TMtLogicalChannel
{
	TLogicalChannel	m_tPrimaryVideoChannel;		//����Ƶ�ŵ�
	TLogicalChannel	m_tSecondVideoChannel;		//�ڶ�����Ƶ�ŵ�
	TLogicalChannel	m_tAudioChannel;		    //��Ƶ�ŵ�
	TLogicalChannel	m_tT120DataChannel;		    //T120�����ŵ�
	TLogicalChannel	m_tH224DataChannel;		    //H224�����ŵ�
	TLogicalChannel m_tMmcuDataChannel;         //���������ŵ�
	
};

//�ն�¼����Ϣ�ṹ
struct TMtRecInfo
{
	TEqp        m_tRecEqp;					//Ϊ���ն�¼���¼���
	u8			m_byRecChannel;				//Ϊ���ն˷���¼�����ͨ����
	TRecState   m_tRecState;				//�ն�¼��״̬
	TRecProg	m_tRecProg;					//�ն�¼�����
};

// xsl [8/3/2006] �ն˱����̽ṹ
struct TSimMtAlias
{
protected:
    u8				m_AliasType;                    //��������
    s8      	    m_achAlias[VALIDLEN_ALIAS + MAXLEN_CONFNAME];     //�����ַ���
    TTransportAddr	m_tTransportAddr;               //�����ַ
public:
	TSimMtAlias( void ){ memset( this, 0, sizeof(TSimMtAlias) ); }
	BOOL32 IsNull( void ){ return (m_AliasType == 0); }
	void SetNull( void ){ memset(this, 0, sizeof(TSimMtAlias)); }
	const TSimMtAlias & operator= (const TMtAlias &tAlias)
    {
        m_AliasType = tAlias.m_AliasType;
        m_tTransportAddr = tAlias.m_tTransportAddr;

        memset(m_achAlias, 0, sizeof(m_achAlias) );
        strncpy(m_achAlias, tAlias.m_achAlias, sizeof( m_achAlias ) - 1);
#ifdef _UTF8
		// utf8��������ת��by zoujunlong [pengguofeng 4/25/2013]
#else
        // guzh [8/31/2006] ����˫�ֽ����ֲ�����ضϣ����ǿ����ܴ�
        s8 nLen = strlen(m_achAlias);
        s8 nLoop = nLen - 1;
        u8 byWideCharCount = 0;
        while ( (signed char)nLoop >= 0  && (signed char)0 > (signed char)m_achAlias[nLoop])
        {
            byWideCharCount ++;
            nLoop --;
        }
        if ( byWideCharCount % 2 == 1 )
        {
            m_achAlias[nLen-1] = '\0';
        }
#endif
        return (*this);
    }    

    TMtAlias GetMtAlias(void)
    {
        TMtAlias tAlias;
        tAlias.m_AliasType = m_AliasType;
        tAlias.m_tTransportAddr = m_tTransportAddr;
        memset(tAlias.m_achAlias, 0, sizeof(tAlias.m_achAlias));
        strncpy(tAlias.m_achAlias, m_achAlias, 
			min(sizeof( m_achAlias ),sizeof(tAlias.m_achAlias) ) );
        tAlias.m_achAlias[MAXLEN_ALIAS-1] = '\0';
        return tAlias;
    }  
} ;

struct TVidSrcAliasInfo
{
public:
	TVidSrcAliasInfo()
	{
		Clean();
	}

	void Clean()
	{
		memset( this, 0, sizeof(TVidSrcAliasInfo) );
		byVidPortIdx = INVALID_MTVIDSRCPORTIDX;
	}

public:
	u8 byVidPortIdx;
	s8 achVidAlias[MT_MAX_PORTNAME_LEN];
};

//�ն����ݽṹ(len:944)
struct TMtData
{
	TMtStatus         m_tMtStatus;                          //���ն�״̬�����ն������ϱ�
	TSimMtAlias       m_atMtAlias[5];                       //�ն˱���,����һ������
	TSimMtAlias       m_tDialAlias;                         //���ű���
	TMtLogicalChannel m_tFwdChannel;                        //MCU��MT���߼�ͨ��
	TMtLogicalChannel m_tRvsChannel;                        //MT��MCU���߼�ͨ��
	TMt               m_tSelVSrc;                           //���ն����տ���ƵԴ
	TMt               m_tSelASrc;                           //���ն���������ƵԴ	
	TMultiCapSupport  m_tCapSupport;                        //�ն�������
	//lukunpeng 2010/07/14 ɾ��¼�����Ϣ����������ն�״̬��ͬ����
	//TMtRecInfo        m_tMtRecInfo;                         //�ն�¼����Ϣ
	BOOL32            m_bNotInvited;                        //���ն˲��Ǳ������,�������������
    u8                m_byAddMtMode;                        //1: �ն��Ǳ�MCS����� 2: �ն��Ǳ���ϯ�����
	BOOL32            m_bMulticasting;                      //MCU�Ƿ����鲥���ն�����
	u8                m_byMonitorDstMt[MAXNUM_MT_CHANNL];   //��ظ��ն˵��ն��б�
	u8                m_byMonitorSrcMt[MAXNUM_MT_CHANNL];   //���ն�����ص��ն��б�
	TTransportAddr    m_tSwitchAddr;                        //���ո��ն����ݵĴ����ַ 
	TTransportAddr    m_tRtcpDstAddr;                       //���ն���ƵRTCPĿ�ĵ�ַ
	TTransportAddr    m_tAudRtcpDstAddr;                    //���ն���ƵRTCPĿ�ĵ�ַ
	TTransportAddr    m_tDcsAddr;							//���ն˵����ݻ����ַ
	u8                m_byMpId;                             //���ո��ն����ݵĽ������
	u32	              m_dwLastRefreshTick;                  //�����FastUpDate��Tick��
	u32               m_dwSecVideoLastRefreshTick;          //�����FastUpDate��Tick��
	BOOL32            m_bMaster;                            //�Ƿ������ն�
	BOOL32			  m_bInDataConf;						//�Ƿ������ݻ����� 2005-12-20
	//BOOL32	      m_bCallingIn;							//�Ƿ��������� 2006-01-06
                                                            // zbq[11/06/2007]��m_bNotInvited�ظ����������ֶ�
    u8                m_byCurrVidNo;                        //��ǰ��ƵԴ��
    u16               m_wSndBandWidth;                      //�ն˷��ʹ���(��Ƶ)
    u16               m_wRcvBandWidth;                      //�ն˽��մ���(��Ƶ)
    TCapSupportEx     m_tCapSupportEx;                      //�ն���չ������
    //FIXME: ����������ն˵�ҵ���߼�������
    BOOL32            m_bMtE1;                              //��ǰ�ն�ΪE1�ն�
    BOOL32            m_bBandWidthLowed;                    //��ǰ���ն��Ƿ񱻽�����.ֻ���E1�ն�.
    u16               m_wLowedRcvBandWidth;                 //�����Ժ��ն��ܽ��մ���
	BOOL32			  m_bRcvBandAjusted;					//E1�ն��Ƿ��Ѿ��������ٵ�����
	u8                m_byMTCapSpecByMCS;                    //�ն�ʹ�õ���������MCSָ��

	//zjj20091014
	u8				  m_byDisconnetReason;					//�ն�����ԭ��

	//lukunpeng 04/01/2010 �Ƿ������ƶ��ն˽����ƻ���
	u8				  m_bySpecInMix;
	
	u8                m_byDisconnectDRI;					//��¼�����Ľ���ת����

	TVidSrcAliasInfo  m_tVidAliasInfo[MT_MAXNUM_VIDSOURCE]; //�ն���ƵԴ������Ϣ
	TMt				  m_tSecSelSrc;							//˫ѡ��Դ
    
	BOOL32  m_bIsPinHole;                                     //�ն˴�״̬ 
    TTransportAddr m_tAudRtpSndTransportAddr;                 //��Ƶrtp���͵�ַ
    TTransportAddr m_tVidRtpSndTransportAddr;                 //��Ƶrtp���͵�ַ
    TTransportAddr m_tSecRtpSndTransportAddr;                 //˫��rtp���͵�ַ
};


class CVmpChgFormatResult
{
public:
	enum emResMask
	{
		Mask_bKeepResInAdpChnnl		= 0x01,		//��ǰ����ͨ������Բ����ֱ���
		Mask_bSendMsg2Mt			= 0x02,		//��Ҫ���ͷֱ�������
		Mask_bSeizePromt			= 0x04		//��Ҫ��ռ��ʾ
	};

	CVmpChgFormatResult()
    {
        Reset();
    }
	
    void Reset()
    {
        m_byResult = 2;
    }
	
	void SetKeepResInAdpChnnl(BOOL32 bStatus) 
    { 
        SETBITSTATUS(m_byResult, Mask_bKeepResInAdpChnnl, bStatus); 
    }
   
    BOOL32 IsKeepResInAdpChnnl(void) const 
    {
        return GETBITSTATUS(m_byResult, Mask_bKeepResInAdpChnnl);
    }

	void SetSendMsg2Mt(BOOL32 bStatus) 
    { 
        SETBITSTATUS(m_byResult, Mask_bSendMsg2Mt, bStatus); 
    }
	
    BOOL32 IsSendMsg2Mt(void) const 
    {
        return GETBITSTATUS(m_byResult, Mask_bSendMsg2Mt);
    }

	void SetSeizePromt(BOOL32 bStatus) 
    { 
        SETBITSTATUS(m_byResult, Mask_bSeizePromt, bStatus); 
    }
	
    BOOL32 IsSeizePromt(void) const 
    {
        return GETBITSTATUS(m_byResult, Mask_bSeizePromt);
    }

private:
	u8	m_byResult;
};


//�����ڲ���չ��״̬�ṹ
struct TConfInnerStatus
{
    //���鲿������״̬
    enum TConfInnerPeriEqpStateMask
    {
        // Prs ͨ���������
        Mask_Status_PrsChnl1Start        =   0x01000000,
        Mask_Status_PrsChnl2Start        =   0x02000000,
        Mask_Status_PrsChnlAudStart      =   0x04000000,
        Mask_Status_PrsChnlAudBasStart   =   0x08000000,
        Mask_Status_PrsChnlVidBasStart   =   0x10000000,
        Mask_Status_PrsChnlBrBasStart    =   0x20000000,
        Mask_Status_PrsChnlHDVidBasStart =   0x40000000,    // ������������Ƶ�����ش�, zgc, 2008-08-13
		
		Mask_Status_PrsChnlBrd1080Start	 =   0x00100000,
		Mask_Status_PrsChnlBrd720Start	 =   0x00200000,
		Mask_Status_PrsChnlBrd4CifStart	 =   0x00400000,
		Mask_Status_PrsChnlBrdCifStart	 =   0x00800000,

		Mask_Status_PrsChnlDsBasStart    =   0x00010000,

        Mask_Status_PrsChnlAllStart      =   0xFF000000,
        
		Mask_Status_HDPrsChnlAllStart	 =   0x0FF10000,

        // ���ݻ���
        Mask_Status_DataConfOnGoing     =    0x00000001,
        Mask_Status_DataConfReCreated   =    0x00000002,

        // ��ѯ�ش�������Radvision��ʽ��
        Mask_Status_PollSwitch          =    0x00010000
    };

    //����״̬��ת����ʱ��״̬
    enum TConfInnerStateMachineMask
    {     
		Mask_Status_LastRollCallFinish		 =   0x00100000,         //��һ�ε��������Ƿ���ɣ���֤���Դ���
			
		Mask_Status_VmpMixerDiscInRoll	 =	 0x00200000,		 //VMP or mixer�Ƿ����ڵ��������е���
        Mask_Status_LastSpeakerInMixing  =   0x01000000,         //��һ�η������Ƿ��ڶ��ƻ����б���
        Mask_Status_NtfMtStatus2MMcu     =   0x02000000,         //�Ƿ����ڼ����ϼ�MCU��ĵ�һ���¼�MCU�ն�״̬ǿ��֪ͨ
        Mask_Status_StartVacOnMixerRecn  =   0x04000000,         //�Ƿ���MIXER������������������
        Mask_Status_SwitchDiscuss2Mix    =   0x08000000,         //�Ƿ��ǻ��������л������ƻ���
        Mask_Status_InviteOnGkReged      =   0x10000000,         //�Ƿ�ȴ�����ע��GK�ɹ��������ն����
        Mask_Status_RegGKNackNtf         =   0x20000000,         //�Ƿ����ڽ�������GKע���ʧ�ܵĵ�һ��ǿ��֪ͨ
        Mask_Status_VmpModuleUsed        =   0x40000000,         //Vmpģ���Ƿ��Ѿ�ʹ�ù���
        Mask_Status_VmpNotify            =   0x80000000          //VMP�ϳ�״̬֪ͨ����֤���Դ���        
    };
    

public:
    TConfInnerStatus()
    {
        Reset();
    }

    void Reset()
    {
        memset(this, 0, sizeof(TConfInnerStatus));
 		SetLastMutiSpyRollCallFinished(TRUE);	//��ֵ��ʼΪTURE
    }
    
    //�����Ƿ�����ѯ�ش�������Radvision��ʽ��
    void SetPollSwitch(BOOL32 bIsPollSwitch) 
    { 
        SETBITSTATUS(m_adwStatus[1], Mask_Status_PollSwitch, bIsPollSwitch); 
    }
    //�����Ƿ�����ѯ�ش�������Radvision��ʽ��
    BOOL32 IsPollSwitch(void) const 
    {
        return GETBITSTATUS(m_adwStatus[1], Mask_Status_PollSwitch);
    }

    //Prs ͨ���������
//     void SetPrsChnlAllStop()
//     {
//         SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlAllStart, FALSE); 
//     }
//     BOOL32 IsPrsChnlAnyStart() const
//     {
//         return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlAllStart);
//     }
// 
// 	void SetHDPrsChnlAllStop()
//     {
//         SETBITSTATUS(m_adwStatus[1], Mask_Status_HDPrsChnlAllStart, FALSE); 
//     }
//     BOOL32 IsHDPrsChnlAnyStart() const
//     {
//         return GETBITSTATUS(m_adwStatus[1], Mask_Status_HDPrsChnlAllStart);
//     }
// 
//     void SetPrsChnl1Start(BOOL32 bStart) 
//     { 
//         SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnl1Start, bStart); 
//     }
//     BOOL32 IsPrsChnl1Start(void) const 
//     {
//         return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnl1Start);
//     }
//     void SetPrsChnl2Start(BOOL32 bStart) 
//     { 
//         SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnl2Start, bStart); 
//     }
//     BOOL32 IsPrsChnl2Start(void) const 
//     {
//         return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnl2Start);
//     }
//     void SetPrsChnlAudStart(BOOL32 bStart) 
//     { 
//         SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlAudStart, bStart); 
//     }
//     BOOL32 IsPrsChnlAudStart(void) const 
//     {
//         return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlAudStart);
//     }
//     void SetPrsChnlAudBasStart(BOOL32 bStart) 
//     { 
//         SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlAudBasStart, bStart); 
//     }
//     BOOL32 IsPrsChnlAudBasStart(void) const 
//     {
//         return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlAudBasStart);
//     }
//     void SetPrsChnlVidBasStart(BOOL32 bStart) 
//     { 
//         SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlVidBasStart, bStart); 
//     }
//     BOOL32 IsPrsChnlVidBasStart(void) const 
//     {
//         return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlVidBasStart);
//     }
//     void SetPrsChnlBrBasStart(BOOL32 bStart) 
//     { 
//         SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlBrBasStart, bStart); 
//     }
//     BOOL32 IsPrsChnlBrBasStart(void) const 
//     {
//         return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlBrBasStart);
//     }
//     // ��������Ƶ�����ش��������, zgc, 2008-08-13
//     void PrsChnlHDVidBasStart(BOOL32 bStart) 
//     { 
//         SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlHDVidBasStart, bStart); 
//     }
//     BOOL32 IsPrsChnlHDVidBasStart(void) const 
//     {
//         return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlHDVidBasStart);
//     }
// 	
// 	void SetPrsChnlVmpOut1Start(BOOL32 bStart) 
//     { 
//         SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlBrd1080Start, bStart); 
//     }
//     BOOL32 IsPrsChnlVmpOut1Start(void) const 
//     {
//         return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlBrd1080Start);
//     }
// 	void SetPrsChnlVmpOut2Start(BOOL32 bStart) 
//     { 
//         SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlBrd720Start, bStart); 
//     }
//     BOOL32 IsPrsChnlVmpOut2Start(void) const 
//     {
//         return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlBrd720Start);
//     }
// 	void SetPrsChnlVmpOut3Start(BOOL32 bStart) 
//     { 
//         SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlBrd4CifStart, bStart); 
//     }
//     BOOL32 IsPrsChnlVmpOut3Start(void) const 
//     {
//         return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlBrd4CifStart);
//     }
// 	void SetPrsChnlVmpOut4Start(BOOL32 bStart) 
//     { 
//         SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlBrdCifStart, bStart); 
//     }
//     BOOL32 IsPrsChnlVmpOut4Start(void) const 
//     {
//         return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlBrdCifStart);
//     }
// 
// 	void SetPrsChnlDsBasStart(BOOL32 bStart) 
//     { 
//         SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlDsBasStart, bStart); 
//     }
//     BOOL32 IsPrsChnlDsBasStart(void) const 
//     {
//         return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlDsBasStart);
//     }

    void SetDataConfOngoing(BOOL32 bStart) 
    { 
        SETBITSTATUS(m_adwStatus[1], Mask_Status_DataConfOnGoing, bStart); 
    }
    BOOL32 IsDataConfOngoing(void) const 
    {
        return GETBITSTATUS(m_adwStatus[1], Mask_Status_DataConfOnGoing);
    }

    void SetDataConfRecreated(BOOL32 bRecr) 
    { 
        SETBITSTATUS(m_adwStatus[1], Mask_Status_DataConfReCreated, bRecr); 
    }
    BOOL32 IsDataConfRecreated(void) const 
    {
        return GETBITSTATUS(m_adwStatus[1], Mask_Status_DataConfReCreated);
    }

    ////////////////////״̬��״̬///////////////////////////////////////////

    //������һ�η������Ƿ��ڶ��ƻ����б���
    void SetLastSpeakerInMixing(BOOL32 bInMixing)
    {
        SETBITSTATUS(m_adwStatus[0], Mask_Status_LastSpeakerInMixing, bInMixing); 
    }
    //������һ�η������Ƿ��ڶ��ƻ����б���
    BOOL32 IsLastSpeakerInMixing(void) const 
    {
        return GETBITSTATUS(m_adwStatus[0], Mask_Status_LastSpeakerInMixing);
    }

    //�����Ƿ����ϼ�MCU��ĵ�һ���¼�MCU�ն�״̬ǿ��֪ͨ
    void SetNtfMtStatus2MMcu(BOOL32 bNtf)
    {
        SETBITSTATUS(m_adwStatus[0], Mask_Status_NtfMtStatus2MMcu, bNtf); 
    }
    //�����Ƿ����ϼ�MCU��ĵ�һ���¼�MCU�ն�״̬ǿ��֪ͨ
    BOOL32 IsNtfMtStatus2MMcu(void) const 
    {
        return GETBITSTATUS(m_adwStatus[0], Mask_Status_NtfMtStatus2MMcu);
    }

    //�����Ƿ���MIXER������������������
    void SetStartVacOnMixReconnect(BOOL32 bStart)
    {
        SETBITSTATUS(m_adwStatus[0], Mask_Status_StartVacOnMixerRecn, bStart); 
    }
    //�����Ƿ���MIXER������������������
    BOOL32 IsStartVacOnMixReconnect(void) const 
    {
        return GETBITSTATUS(m_adwStatus[0], Mask_Status_StartVacOnMixerRecn);
    }

    //�����Ƿ��ǻ��������л������ƻ���
    void SetSwitchDiscuss2Mix(BOOL32 bStepOne)
    {
        SETBITSTATUS(m_adwStatus[0], Mask_Status_SwitchDiscuss2Mix, bStepOne); 
    }
    //�����Ƿ��Ѿ�ִ�й���������ֹͣ
    BOOL32 IsSwitchDiscuss2Mix(void) const 
    {
        return GETBITSTATUS(m_adwStatus[0], Mask_Status_SwitchDiscuss2Mix);
    }

    //�����Ƿ�ȴ�����ע��GK�ɹ��������ն����
    void SetInviteOnGkReged(BOOL32 bInvite)
    {
        SETBITSTATUS(m_adwStatus[0], Mask_Status_InviteOnGkReged, bInvite); 
    }
    //�����Ƿ�ȴ�����ע��GK�ɹ��������ն����
    BOOL32 IsInviteOnGkReged(void) const 
    {
        return GETBITSTATUS(m_adwStatus[0], Mask_Status_InviteOnGkReged);
    }

    //�����Ƿ����ڽ�������GKע���ʧ�ܵĵ�һ��ǿ��֪ͨ
    void SetRegGkNackNtf(BOOL32 bNtf)
    {
        SETBITSTATUS(m_adwStatus[0], Mask_Status_RegGKNackNtf, bNtf); 
    }
    //�����Ƿ����ڽ�������GKע���ʧ�ܵĵ�һ��ǿ��֪ͨ
    BOOL32 IsRegGkNackNtf(void) const 
    {
        return GETBITSTATUS(m_adwStatus[0], Mask_Status_RegGKNackNtf);
    }

    //�����Ƿ�Vmpģ���Ƿ��Ѿ�ʹ�ù���
    void SetVmpModuleUsed(BOOL32 bUsed)
    {
        SETBITSTATUS(m_adwStatus[0], Mask_Status_VmpModuleUsed, bUsed); 
    }
    //�����Ƿ�Vmpģ���Ƿ��Ѿ�ʹ�ù���
    BOOL32 IsVmpModuleUsed(void) const 
    {
        return GETBITSTATUS(m_adwStatus[0], Mask_Status_VmpModuleUsed);
    }

    //����VMP�ϳ�״̬֪ͨ
    void SetVmpNotify(BOOL32 bNtf)
    {
        SETBITSTATUS(m_adwStatus[0], Mask_Status_VmpNotify, bNtf); 
    }
    //����VMP�ϳ�״̬֪ͨ
    BOOL32 IsVmpNotify(void) const 
    {
        return GETBITSTATUS(m_adwStatus[0], Mask_Status_VmpNotify);
    }

	//�����ϴε����Ƿ�����ȫ
	BOOL32 IsLastMutiSpyVmpRollCallFinished(void) const 
    {
        return GETBITSTATUS(m_adwStatus[0], Mask_Status_LastRollCallFinish);
    }
	//�����ϴε����Ƿ�����ȫ
    void SetLastMutiSpyRollCallFinished(BOOL32 bFinish)
    {
        SETBITSTATUS(m_adwStatus[0], Mask_Status_LastRollCallFinish, bFinish); 
    }

	//�Ƿ��ڵ����������������
	BOOL32 IsVmpMixerDiscInRoll(void) const
	{
		return GETBITSTATUS(m_adwStatus[0], Mask_Status_VmpMixerDiscInRoll);
	}
	
	//�����Ƿ�����������������
	void SetVmpMixerDiscInRoll(BOOL32 bInRoll)
	{
		SETBITSTATUS(m_adwStatus[0], Mask_Status_VmpMixerDiscInRoll, bInRoll);
	}


// 	BOOL32 IsPrsChnStart(u8 byPrsMode)
// 	{
// 		BOOL32 bIsChnStart = FALSE;
// 		switch (byPrsMode)
// 		{
// 		case PRSCHANMODE_FIRST:    bIsChnStart = IsPrsChnl1Start();       break;
// 		case PRSCHANMODE_SECOND:   bIsChnStart = IsPrsChnl2Start();       break;
// 		case PRSCHANMODE_AUDIO:    bIsChnStart = IsPrsChnlAudStart();     break;
// 		case PRSCHANMODE_VMPOUT1:  bIsChnStart = IsPrsChnlVmpOut1Start(); break;
// 		case PRSCHANMODE_VMPOUT2:  bIsChnStart = IsPrsChnlVmpOut2Start(); break;
// 		case PRSCHANMODE_VMPOUT3:  bIsChnStart = IsPrsChnlVmpOut3Start(); break;
// 		case PRSCHANMODE_VMPOUT4:  bIsChnStart = IsPrsChnlVmpOut4Start(); break;
// 		case PRSCHANMODE_VMP2:	   bIsChnStart = IsPrsChnlBrBasStart();   break;
// 		default:
// 			LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[IsPrsChnStart] unexpected PrsMode:%d\n", byPrsMode);
// 			break;	
// 		}
// 		return bIsChnStart;
// 	}

    void Print() const
    {
        StaticLog("Conf Inner Status: \n");
//         StaticLog("    IsPrsChnl1Start: %d\n", IsPrsChnl1Start());
//         StaticLog("    IsPrsChnl2Start: %d\n", IsPrsChnl2Start());
//         StaticLog("    IsPrsChnlAudStart: %d\n", IsPrsChnlAudStart());
//         StaticLog("    IsPrsChnlAudBasStart: %d\n", IsPrsChnlAudBasStart());
//         StaticLog("    IsPrsChnlVidBasStart: %d\n", IsPrsChnlVidBasStart());
//         StaticLog("    IsPrsChnlBrBasStart: %d\n", IsPrsChnlBrBasStart());

        StaticLog("    IsPollSwitch: %d\n", IsPollSwitch());

        StaticLog("    IsDataConfOngoing: %d\n", IsDataConfOngoing());
        StaticLog("    IsDataConfRecreated: %d\n", IsDataConfRecreated());

        StaticLog("    IsLastSpeakerInMixing: %d\n", IsLastSpeakerInMixing());
        StaticLog("    IsNtfMtStatus2MMcu: %d\n", IsNtfMtStatus2MMcu());
        StaticLog("    IsStartVacOnMixReconnect: %d\n", IsStartVacOnMixReconnect());
        StaticLog("    IsSwitchDiscuss2Mix: %d\n", IsSwitchDiscuss2Mix());
        StaticLog("    IsInviteOnGkReged: %d\n", IsInviteOnGkReged());
        StaticLog("    IsRegGkNackNtf: %d\n", IsRegGkNackNtf());
        StaticLog("    IsVmpModuleUsed: %d\n", IsVmpModuleUsed());
        StaticLog("    IsVmpNotify: %d\n", IsVmpNotify());
		StaticLog("    IsLastMutiSpyVmpRollCallFinished:%d\n",IsLastMutiSpyVmpRollCallFinished());
        
    }
protected:
    u32 m_adwStatus[2];
}
;

//�����ն˱�(len:187972)
struct TConfMtTable
{
public:
	u8    m_byMaxNumInUse;                  //���ʹ����
	TMtExt	m_atMtExt[MAXNUM_CONF_MT];      //�ն��б�
	TMtData m_atMtData[MAXNUM_CONF_MT];     //�ն����ݱ�
	TAddMtCapInfo m_tMTInfoEx[MAXNUM_CONF_MT];//���԰������ն˸�����Ϣ����MCSָ���Ĳ����ն�������
	TVrsRecChnnlStatus m_atVrsRecChlStatus[MAXNUM_PERIEQP]; //׷��vrs��¼��״̬��Ϣ֧�֣���16���洢

protected:
    TMtExt2 m_atMtExt2[MAXNUM_CONF_MT];     //�ն���չ��Ϣ��(Ŀǰֻ��汾��Ϣ)

public:
	//construct
    TConfMtTable( void )
	{
		m_byMaxNumInUse = 0;
		memset( m_atMtExt, 0, sizeof( m_atMtExt ) );
		memset( m_atMtData, 0, sizeof( m_atMtData ) );	
		memset( m_atVrsRecChlStatus, 0, sizeof( m_atVrsRecChlStatus ) );
	}

	// m_tVrsRecChlStatus
	u8 GetVrsChlIdxbyMtId(u8 byMtId); //ͨ��VrsMtId��ö�Ӧ��VrsChlIdx
	BOOL32 AddVrsRecChlbyMtId(u8 byMtId); //����һ���µ�vrsͨ����Ϣ
	TRecChnnlStatus GetRecChnnlStatus(u8 byMtId);
	void ResetRecChnnlStatus(u8 byMtId);
	//void	SetRecChnnlStatus(u8 byMtId, TRecChnnlStatus tRecChnnlStatus); �ݲ��ṩֱ�����ýӿ�
	BOOL32	GetRecordName(u8 byMtId, char *szAlias, u16 wBufLen );
	void	SetRecordName(u8 byMtId, LPCSTR lpszName);
	u8		GetRecChlType(u8 byMtId); //ͨ������
	void	SetRecChlType(u8 byMtId, u8 byRecChlType);
	u8		GetRecChlState(u8 byMtId); //ͨ��״̬
	void	SetRecChlState(u8 byMtId, u8 byRecChlState);
	u8		GetRecMode(u8 byMtId); //¼��ʽ
	void	SetRecMode(u8 byMtId, u8 byRecMode);
	TRecProg	GetRecProg(u8 byMtId); //��ǰ¼���������
	void	SetRecProg(u8 byMtId, TRecProg tRecProg);
	TMtNoConstruct	GetRecSrc(u8 byMtId); //��ǰ¼��Դ
	void	SetRecSrc(u8 byMtId, TMtNoConstruct tSrc);
	

	//m_atMtExt
	void   SetMcuId( u8 byMtId, u16 wMcuId ); 
	u16     GetMcuId( u8 byMtId );
	void   SetDriId( u8 byMtId, u8 byDriId );
	u8     GetDriId( u8 byMtId );
	void   SetConfIdx( u8 byMtId, u8 byConfIdx); 
    u8     GetConfIdx( u8 byMtId );
	void   SetMtType( u8 byMtId, u8 byMtType );
    u8     GetMtType( u8 byMtId );
	u8	   GetMainType( u8 byMtId );
	void   SetMainType( u8 byMtId, u8 byMtType );
	u8     GetProtocolType( u8 byMtId );
    void   SetProtocolType( u8 byMtId, u8 byProtocolType);
    
    void   SetManuId( u8 byMtId, u8 byManuId);
    u8     GetManuId( u8 byMtId );
    void   SetCallMode( u8 byMtId, u8 byCallMode );
    u8     GetCallMode( u8 byMtId ) ;
	void   SetCallLeftTimes( u8 byMtId, u32 dwCallLeftTimes );
	u32    GetCallLeftTimes( u8 byMtId );
    void   SetIPAddr( u8 byMtId, u32 dwIPAddr ); 
    u32	   GetIPAddr( u8 byMtId );

	//m_atMtData
	void   SetMtLogicChnnl( u8 byMtId, u8 byChannel, const TLogicalChannel * ptChnnl, BOOL32 bForwardChnnl );
	BOOL32 GetMtLogicChnnl( u8 byMtId, u8 byChannel, TLogicalChannel * ptChnnl, BOOL32 bForwardChnnl );
	//BOOL32 GetMtLogicChnnl( const TMt &tMt, u8 byChannel, TLogicalChannel * ptChnnl, BOOL32 bForwardChnnl );// [1/18/2010 xliang] 
	void   ClearMtLogicChnnl( u8 byMtId );
	BOOL32 IsLogicChnnlOpen( u8 byMtId, u8 byChannel, BOOL32 bForwardChnnl );
	void   SetMtSrc( u8 byMtId, const TMt * ptSrc, u8 byMode );
	BOOL32 GetMtSrc( u8 byMtId, TMt * ptSrc, u8 byMode );
	void   SetMtStatus( u8 byMtId, const TMtStatus * ptStatus );
	BOOL32 GetMtStatus( u8 byMtId, TMtStatus * ptStatus );
	void   SetMtCapSupport( u8 byMtId, const TCapSupport * ptCapSupport );
    BOOL32 GetMtCapSupport( u8 byMtId, TCapSupport * ptCapSupport );
    void   SetMtMultiCapSupport(u8 byMtId, const TMultiCapSupport * ptMultiCapSupport);
    BOOL32 GetMtMultiCapSupport(u8 byMtId, TMultiCapSupport * ptMultiCapSupport);
	
    void   SetMtCapSupportEx( u8 byMtId, const TCapSupportEx * ptCapSupport );
    BOOL32 GetMtCapSupportEx( u8 byMtId, TCapSupportEx * ptCapSupport );
	BOOL32 SetMtAlias( u8 byMtId, const TMtAlias * ptAlias );
	BOOL32 GetMtAlias( u8 byMtId, mtAliasType AliasType, TMtAlias * ptAlias );
	BOOL32 SetPuAlias( u8 byMtId, const TMtAlias * ptAlias );

	void   SetNotInvited( u8 byMtId, BOOL32 bNotInvited = TRUE );
	BOOL32 IsNotInvited( u8 byMtId );
	void   SetAddMtMode(u8 byMtId, u8 byAddMtMode);
	u8     GetAddMtMode(u8 byMtId);
	u16    GetMtReqBitrate(u8 byMtId, BOOL32 bPrimary = TRUE);
	void   SetMtReqBitrate(u8 byMtId, u16 wBitRate, u8 byChnlMode = LOGCHL_VIDEO_BOTH);
	u16    GetMtSndBitrate(u8 byMtId, u8 byChannel = LOGCHL_VIDEO);
	void   SetMtSndBitrate(u8 byMtId, u16 wBitRate);
	void   SetMtSwitchAddr(u8 byMtId, u32 dwRcvIp, u16 wRcvPort);
    void   ClearMtSwitchAddr(u8 byMtId);
	BOOL32 GetMtSwitchAddr(u8 byMtId, u32 &dwRcvIp, u16 &wRcvPort);
	void   SetMtRtcpDstAddr(u8 byMtId, u32 dwDstIp, u16 wDstPort, u8 byMode = MODE_VIDEO);
	BOOL32 GetMtRtcpDstAddr(u8 byMtId, u32 &dwDstIp, u16 &wDstPort, u8 byMode = MODE_VIDEO);
	TSimCapSet GetSrcSCS(u8 byMtId);
	TSimCapSet GetDstSCS(u8 byMtId);
	TSimCapSet GetSrcDSSCS(u8 byMtId);
	TSimCapSet GetDstDSSCS(u8 byMtId);
	BOOL32 IsMtMulticasting(u8 byMtId);
    void   SetMtMulticasting(u8 byMtId, BOOL32 bMulticasting = TRUE);
	void   AddMonitorSrcMt(u8 byMtId, u8 byMonitorMtId);
	void   RemoveMonitorSrcMt(u8 byMtId, u8 byMonitorMtId);
	u8     GetMonitorSrcMtList(u8 byMtId, u8 byMtList[]);
	void   AddMonitorDstMt(u8 byMtId, u8 byDstMtId);
	void   RemoveMonitorDstMt(u8 byMtId, u8 byDstMtId);
	BOOL32 HasMonitorDstMt(u8 byMtId);
	void   ClearMonitorSrcAndDst(u8 byMtId);
	void   SetMpId(u8 byMtId, u8 byMpId);
	u8     GetMpId(u8 byMtId);
	void   SetLastTick(u8 byMtId, u32 dwLastTick, BOOL32 bSecVideo = FALSE);
	u32    GetLastTick(u8 byMtId, BOOL32 bSecVideo = FALSE);

	// ����������
	void   SetMtCapSpecByMCS( u8 byMtId, u8 bySpecByMCS);
	u8     IsMtCapSpecByMCS( u8 byMtId);

	//Special For Terminal Recording
	//lukunpeng 2010/07/14 ɾ��¼�����Ϣ����������ն�״̬��ͬ����
	//void   SetMtRecording( u8 byMtId, const TEqp& tRecEqp, const u8& byRecChannel );
	void   SetMtRecording( u8 byMtId );
	void   SetMtNoRecording( u8 byMtId );
	void   SetMtRecPause( u8 byMtId );
	//BOOL32 GetMtRecordInfo( u8 byMtId, TEqp* ptRecEqp, u8 * pbyRecChannel );
	//u8     GetMtIdFromRecordInfo( const TEqp& tRecEqp, const u8& byRecChannel );
	BOOL32 GetMtRecState( u8 byMtId, TRecState * ptRecState ); 
	BOOL32 IsMtRecording( u8 byMtId );
	BOOL32 IsMtNoRecording( u8 byMtId );
	BOOL32 IsMtRecPause( u8 byMtId );
	BOOL32 IsMtRecSkipFrame( u8 byMtId ) const ;
	void   SetMtRecSkipFrame( u8 byMtId, BOOL32 bSkipFrame );
	//void   SetMtRecProg( u8 byMtId, const TRecProg & tRecProg );
	//BOOL32 GetMtRecProg( u8 byMtId, TRecProg * ptRecProg );

	//other
	TMt GetMt( u8 byMtId );
    u8  AddMt( u32 dwIpAddr, BOOL32 bRepeatFilter = TRUE );
    u8  AddMt( TMtAlias * ptAlias, BOOL32 bRepeatFilter = TRUE );
	void   SetDialAlias( u8 byMtId, const TMtAlias * ptAlias );
	BOOL32 GetDialAlias( u8 byMtId, TMtAlias * ptAlias );
		
	void SetDialBitrate( u8 byMtId, u16 wDialBitrate );
	u16  GetDialBitrate( u8 byMtId );

    void SetSndBandWidth(u8 byMtId, u16 wBandWidth);
    u16  GetSndBandWidth(u8 byMtId);
    void SetRcvBandWidth(u8 byMtId, u16 wBandWidth);
    u16  GetRcvBandWidth(u8 byMtId);

    BOOL32 DelMt( u8 byMtId );
    u8 GetMtIdByIp( u32 dwIpAddr );
	u8 GetMtIdByAlias( const TMtAlias * ptAlias );
	u8 GetVrsRecMtIdByAlias(const TMtAlias * ptAlias, u8 byType, TMt tSrc);//���ݱ�������Ϣ����vrs��¼��ʵ��Id
	
	//Mt send and receive
	void   SetMtVideoSend( u8 byMtId, BOOL32 bSending );
	BOOL32 IsMtVideoSending( u8 byMtId );
	void   SetMtAudioSend( u8 byMtId, BOOL32 bSending );
	BOOL32 IsMtAudioSending( u8 byMtId );
	void   SetMtVideoRecv( u8 byMtId, BOOL32 bRecving, u16 wBitRate = 0 );
	BOOL32 IsMtVideoRecving( u8 byMtId );
	void   SetMtAudioRecv( u8 byMtId, BOOL32 bRecving );
	BOOL32 IsMtAudioRecv( u8 byMtId );
	void   SetMtVideo2Send( u8 byMtId, BOOL32 bSending );
	BOOL32 IsMtVideo2Sending( u8 byMtId );
	void   SetMtVideo2Recv( u8 byMtId, BOOL32 bSending );
	BOOL32 IsMtVideo2Recving( u8 byMtId );
	void   SetMtInMixing( u8 byMtId, BOOL32 bMtInMixing, BOOL32 bAutoInSpec );
	BOOL32 IsMtInMixing( u8 byMtId );
	BOOL32 IsMtAutoInSpec( u8 byMtId );
    void   SetMtInMixGrp( u8 byMtId, BOOL32 bDiscuss );
    BOOL32 IsMtInMixGrp( u8 byMtId );
	BOOL32 IsMtAudioMute( u8 byMtId );
	BOOL32 IsMtAudioDumb( u8 byMtId );
    
    void   SetMtInTvWall(u8 byMtId, BOOL32 bMtInTvWall = TRUE);
	BOOL32 IsMtInTvWall(u8 byMtId);
	
	//4.6 �¼�
	void   SetMtInHdu(u8 byMtId, BOOL32 bMtInHdu = TRUE);
	BOOL32 IsMtInHdu(u8 byMtId);


	void   SetMtIsMaster(u8 byMtId, BOOL32 bMaster);
	BOOL32 IsMtIsMaster(u8 byMtId);

	s8*  GetMtAliasFromExt( u8 byMtId ) const;

    void SetCurrVidSrcNo(u8 byMtId, u8 byCurrVidNo);
    u8   GetCurrVidSrcNo(u8 byMtId);

	void   SetMtInDataConf( u8 byMtId, BOOL32 bInDataConf );
	BOOL32 IsMtInDataConf( u8 byMtId );

	void   SetMtDcsAddr(u8 byMtId, u32 dwDstIp, u16 wDstPort, u8 byMode = MODE_DATA);
	BOOL32 GetMtDcsAddr(u8 byMtId, u32 &dwDstIp, u16 &wDstPort, u8 byMode = MODE_DATA);

    //TMtExt2
    void SetHWVerID(u8 byMtId, u8 byHWVer);
    u8   GetHWVerID(u8 byMtId) const;

    void   SetSWVerID(u8 byMtId, LPCSTR lpszSWVer);
    LPCSTR GetSWVerID(u8 byMtId) const;

    void   SetProductId(u8 byMtId, LPCSTR lpszProductId);
    LPCSTR GetProductId(u8 byMtId) const;

    void   SetVersionId(u8 byMtId, LPCSTR lpszVersionId);
    LPCSTR GetVersionId(u8 byMtId) const;

    void    SetMtBRBeLowed( u8 byMtId, BOOL32 bLowed );
    BOOL32  GetMtBRBeLowed( u8 byMtId );

    void    SetMtTransE1( u8 byMtId, BOOL32 bTransE1 );
    BOOL32  GetMtTransE1( u8 byMtId );

    void    SetLowedRcvBandWidth( u8 byMtId, u16 wLowedBandWidth );
    u16     GetLowedRcvBandWidth( u8 byMtId );
	
	void    SetRcvBandAdjusted( u8 byMtId, BOOL32 bAjusted );
	BOOL32  GetRcvBandAdjusted( u8 byMtId );

	//zjj20091014
	u8		GetDisconnectReason( u8 byMtId );
	void	SetDisconnectReason( u8 byMtId,u8 byReason );

	u8      GetDisconnectDRI( u8 byMtId );
    void	SetDisconnectDRI( u8 byMtId,u8 byDRIId );
    // End

	BOOL32 SetMtVidAlias( u8 byMtId, u8 byVidNum, const TVidSrcAliasInfo *ptVidAliasInfo );
	u8 GetMtVidPortNum( u8 byMtId );
	TVidSrcAliasInfo GetMtVidAliasbyVidIdx( u8 byMtId, u8 byIdx );
	void InitMtVidAlias( u8 byMtId );
	TMt GetMtSelMtByMode(u8 byMtId, u8 byMode);
	void SetMtSelMtByMode(u8 byMtId, u8 byMode, TMt &tSrcMt);
	void RemoveMtSelMtByMode(u8 byMtId, u8 byMode);
};

// xsl [8/3/2006] ������Ϣ���ٴ��ն�״̬�̳�(len: 52)
struct TMcMtStatus : public TMt
{
    enum TMcMtStatusMask
    {
        Mask_AudSend    =   0x0001,
        Mask_AudRcv     =   0x0002,
        Mask_VidSend    =   0x0004,
        Mask_VidRcv     =   0x0008,
        Mask_Vid2Send   =   0x0010,
        Mask_Vid2Rcv    =   0x0020,
        Mask_Fecc       =   0x0040,
        Mask_Mixing     =   0x0080,
        Mask_VidLose    =   0x0100,
        Mask_InHdu		=   0x0200,
		Mask_InTvWall	=	0x0400
    };

protected:
    u16 m_wMtStatus;
    // guzh [6/27/2007] ����4���ֶ� u8 �� MEDIA_TYPE_xxx 
	u8  m_byAudioIn;
	u8  m_byAudioOut;
	u8  m_byVideoIn;
	u8  m_byVideoOut;
//	u8  m_byVideo2In;
//	u8  m_byVideo2Out;
    u8  m_byCurVideo;
    u8  m_byCurAudio;
    u8  m_byBoardType;
	TMtVideoInfo m_tPartVideoInfo;
	//zjj20100327
	u8  m_byMtDisconnectReason;

	//lukunpeng 2010/07/06 ����¼�����ʾ
	TRecState m_tRecState;//�ն�¼��״̬

	// [pengjie 2010/8/11] ��ʾ�ն��Ƿ��Զ�������
	u8 m_byIsMtManualInMix;

public:
    TMcMtStatus(void) 
    { 
        memset(this, 0, sizeof(TMcMtStatus)); 
        SetCurVideo(1);
		SetCurAudio(1);
    }
    
    void    SetSendAudio(BOOL32 bSendAudio){ SETBITSTATUS(m_wMtStatus, Mask_AudSend, bSendAudio) } 
    BOOL32  IsSendAudio( void ) const { return GETBITSTATUS(m_wMtStatus, Mask_AudSend); }
    
    void    SetReceiveAudio(BOOL32 bRcv) { SETBITSTATUS(m_wMtStatus, Mask_AudRcv, bRcv) }
    BOOL32  IsReceiveAudio() const { return GETBITSTATUS(m_wMtStatus, Mask_AudRcv); }

    void    SetSendVideo(BOOL32 bSendVideo){ SETBITSTATUS(m_wMtStatus, Mask_VidSend, bSendVideo) } 
    BOOL32  IsSendVideo( void ) const { return GETBITSTATUS(m_wMtStatus, Mask_VidSend); }

    void    SetReceiveVideo(BOOL32 bReceiveVideo){ SETBITSTATUS(m_wMtStatus, Mask_VidRcv, bReceiveVideo) } 
    BOOL32  IsReceiveVideo( void ) const { return GETBITSTATUS(m_wMtStatus, Mask_VidRcv); }

    void    SetSndVideo2(BOOL32 bSend) { SETBITSTATUS(m_wMtStatus, Mask_Vid2Send, bSend) }
	BOOL32  IsSndVideo2() const { return GETBITSTATUS(m_wMtStatus, Mask_Vid2Send); }

    void    SetRcvVideo2(BOOL32 bRcv) { SETBITSTATUS(m_wMtStatus, Mask_Vid2Rcv, bRcv) }
    BOOL32  IsRcvVideo2() const { return GETBITSTATUS(m_wMtStatus, Mask_Vid2Rcv); }    
    
    void    SetIsEnableFECC(BOOL32 bCamRCEnable){ SETBITSTATUS(m_wMtStatus, Mask_Fecc, bCamRCEnable) } 
    BOOL32  IsEnableFECC( void ) const { return GETBITSTATUS(m_wMtStatus, Mask_Fecc); }   
    
    void    SetInMixing(BOOL32 bMixing) { SETBITSTATUS(m_wMtStatus, Mask_Mixing, bMixing) }
    BOOL32  IsInMixing(void) const { return GETBITSTATUS(m_wMtStatus, Mask_Mixing); }

    void    SetVideoLose(BOOL32 bVideoLose){ SETBITSTATUS(m_wMtStatus, Mask_VidLose, bVideoLose) }
    BOOL32  IsVideoLose( void ) const { return GETBITSTATUS(m_wMtStatus, Mask_VidLose); }

    void    SetInTvWall(BOOL32 bInTvWall){ SETBITSTATUS(m_wMtStatus, Mask_InTvWall, bInTvWall) }
    BOOL32  IsInTvWall( void ) const { return GETBITSTATUS(m_wMtStatus, Mask_InTvWall); }

    void    SetInHdu(BOOL32 bInHdu){ SETBITSTATUS(m_wMtStatus, Mask_InHdu, bInHdu) }
    BOOL32  IsInHdu( void ) const { return GETBITSTATUS(m_wMtStatus, Mask_InHdu); }

    void    SetAudioIn(u8 byType) { m_byAudioIn = byType; }
    u8      GetAudioIn(void) const { return m_byAudioIn; }

    void    SetAudioOut(u8 byType) { m_byAudioOut = byType; }
    u8      GetAudioOut(void) const { return m_byAudioOut; }

    void    SetVideoIn(u8 byType) { m_byVideoIn = byType; }
    u8      GetVideoIn(void) const { return m_byVideoIn; }

    void    SetVideoOut(u8 byType) { m_byVideoOut = byType; }
    u8      GetVideoOut(void) const { return m_byVideoOut; }

//    void    SetVideo2In(u8 byType) { m_byVideo2In = byType; }
//    u8      GetVideo2In(void) const { return m_byVideo2In; }
//    
//    void    SetVideo2Out(u8 byType) { m_byVideo2Out = byType; }
//    u8      GetVideo2Out(void) const { return m_byVideo2Out; }

    void    SetCurVideo(u8 byVideoIndex) { m_byCurVideo = byVideoIndex; }
    u8      GetCurVideo(void) const { return m_byCurVideo; }

    void    SetCurAudio(u8 byAudioIndex) { m_byCurAudio = byAudioIndex; }
    u8      GetCurAudio(void) const { return m_byCurAudio; }

    void    SetMtBoardType(u8 byType) { m_byBoardType = byType; }
    u8      GetMtBoardType(void) const { return m_byBoardType; }

    void    SetMtVideoInfo(TMtVideoInfo tInfo) { memcpy(&m_tPartVideoInfo, &tInfo, sizeof(TMtVideoInfo)); }
    TMtVideoInfo  GetMtVideoInfo(void) const { return m_tPartVideoInfo; }

	//zjj20100327
	void	SetMtDisconnectReason( u8 byReason ){ m_byMtDisconnectReason = byReason; }
	u8		GetMtDisconnectReason( void ) const { return m_byMtDisconnectReason; }

	
	BOOL IsNoRecording( void ) const	{ return m_tRecState.IsNoRecording(); }
	void SetNoRecording( void )	{ m_tRecState.SetNoRecording(); }
	BOOL IsRecording( void ) const	{ return m_tRecState.IsRecording(); }
	void SetRecording( void )	{ m_tRecState.SetRecording(); }
	BOOL IsRecPause( void ) const	{ return m_tRecState.IsRecPause(); }
	void SetRecPause( void )	{ m_tRecState.SetRecPause(); }
	//�Ƿ��ڳ�֡¼��״̬
	BOOL IsRecSkipFrame() const { return m_tRecState.IsRecSkipFrame(); }
	void SetRecSkipFrame( BOOL bSkipFrame ) { m_tRecState.SetRecSkipFrame(bSkipFrame);}
	
	TRecState GetRecState() const { return  m_tRecState;}
	void SetRecState(const TRecState& tRecState) { memcpy(&m_tRecState, &tRecState, sizeof(m_tRecState));}

	void SetIsAutoInMix( const u8 byIsAuto ) { m_byIsMtManualInMix = (byIsAuto==0) ? 1 : 0;}
	u8   GetIsAutoInMix( void ) const 
	{ 
		return m_byIsMtManualInMix == 0 ? 1 : 0;
	}


    TMtStatus   GetMtStatus(void) const
    {
        TMtStatus tStatus;
        memcpy(&tStatus, this, sizeof(TMt));
        tStatus.SetSendAudio(IsSendAudio());
        tStatus.SetSendVideo(IsSendVideo());
        tStatus.SetSndVideo2(IsSndVideo2());
        tStatus.SetReceiveAudio(IsReceiveAudio());
        tStatus.SetReceiveVideo(IsReceiveVideo());
        tStatus.SetRcvVideo2(IsRcvVideo2());
        tStatus.SetIsEnableFECC(IsEnableFECC());
        tStatus.SetMtBoardType(GetMtBoardType());
        tStatus.SetCurAudio(GetCurAudio());
        tStatus.SetCurVideo(GetCurVideo());
        tStatus.SetInMixing(IsInMixing());
        tStatus.SetVideoLose(IsVideoLose());
        tStatus.SetInTvWall(IsInTvWall());
        tStatus.SetInHdu(IsInHdu());
		//zjj20100327
		tStatus.SetMtDisconnectReason( GetMtDisconnectReason() );

		//2010/07/13 lukunpeng ����¼�¼���״̬
		memcpy(&tStatus.m_tRecState, &m_tRecState, sizeof(m_tRecState));
        return tStatus;
    }
	
	void SetMtStatus(const TMtStatus &tStatus)
	{
		memcpy(this, &tStatus, sizeof(TMt));
		SetSendAudio(tStatus.IsSendAudio());
		SetSendVideo(tStatus.IsSendVideo());
        SetSndVideo2(tStatus.IsSndVideo2());
        SetReceiveAudio(tStatus.IsReceiveAudio());
        SetReceiveVideo(tStatus.IsReceiveVideo());
        SetRcvVideo2(tStatus.IsRcvVideo2());
        SetIsEnableFECC(tStatus.IsEnableFECC());
        SetMtBoardType(tStatus.GetMtBoardType());
        SetCurAudio(tStatus.GetCurAudio());
        SetCurVideo(tStatus.GetCurVideo());
        SetInMixing(tStatus.IsInMixing());
        SetVideoLose(tStatus.IsVideoLose());
        SetInTvWall(tStatus.IsInTvWall());
        SetInHdu(tStatus.IsInHdu());
		//zjj20100327
		SetMtDisconnectReason( tStatus.GetMtDisconnectReason() );
		memcpy(&m_tRecState, &tStatus.m_tRecState, sizeof(m_tRecState));
	}
	
	
	void PrintTitle() const 
	{
		s8 achFmt[255] = { 0 };
		u8 byLength = (u8)sprintf(achFmt,  "%5s %4s %6s %4s %4s %4s %4s %4s "  , 
			"McuId", "MtId", "Status", "FECC", "SndA", "RcvA", "SndV", "RcvV");
		
		byLength += (u8)sprintf(achFmt + byLength, "%5s %5s %5s %3s %3s %3s %4s %4s %4s %7s\n",
			"SndV2", "RcvV2", "LoseV", "Mix", "Hdu", "Tvw", "CurA", "CurV", "BrdT", "ManuMix");
		
		StaticLog(achFmt);	
	}
	
    void Print( void ) const
	{
		// [11/1/2011 liuxu] ��ӡ����
		s8 achFmt[255] = { 0 };
		u8 byLength = (u8)sprintf(achFmt,  "%5d %4d %6d %4d %4d %4d %4d %4d "  , 
			GetMcuId(), 
			GetMtId(),
			m_wMtStatus, 
			IsEnableFECC(), 
			IsSendAudio(), 
			IsReceiveAudio(), 
			IsSendVideo(), 
			IsReceiveVideo());
		
		sprintf(achFmt + byLength, "%5d %5d %5d %3d %3d %3d %4d %4d %4d %7d\n",
			IsSndVideo2(), 
			IsRcvVideo2(), 
			IsVideoLose(), 
			IsInMixing(), 
			IsInHdu(), 
			IsInTvWall(), 
			GetCurAudio(), 
			GetCurVideo(), 
			GetMtBoardType(),
			(GetIsAutoInMix() == 0 ? 1 : 0));
		
		StaticLog(achFmt);	
		
		
// 		StaticLog( "Mcu%dMt%d status:\n", GetMcuId(), GetMtId() ); 
//         StaticLog( "	m_wMtStatus     = 0x%x\n", m_wMtStatus );
// 		StaticLog( "	m_byIsEnableFECC = %d\n", IsEnableFECC() );
// 		StaticLog( "	IsSendAudio     = %d\n", IsSendAudio() );
//         StaticLog( "	IsReceiveAudio  = %d\n", IsReceiveAudio() );
// 		StaticLog( "	IsSendVideo     = %d\n", IsSendVideo() );
//         StaticLog( "	IsReceiveVideo  = %d\n", IsReceiveVideo() );
//         StaticLog( "	IsSndVideo2     = %d\n", IsSndVideo2() );
//         StaticLog( "	IsRcvVideo2     = %d\n", IsRcvVideo2() );
// 		StaticLog( "	IsInVideoLose   = %d\n", IsVideoLose() );
// 		StaticLog( "	IsInMixing      = %d\n", IsInMixing() );
// 		StaticLog( "	IsInHdu         = %d\n", IsInHdu() );
// 		StaticLog( "	IsInTvwall      = %d\n", IsInTvWall() );
// 	    StaticLog( "	m_byCurAudio    = %d\n", GetCurAudio() );
// 		StaticLog( "	m_byCurVideo    = %d\n", GetCurVideo() );		
//         StaticLog( "	m_byBoardType   = %d\n", GetMtBoardType() );
// 		StaticLog( "	m_byIsMtManualInMix   = %d\n", (GetIsAutoInMix() == 0 ? 1 : 0));
    }
    
};
//����Mc�ϵ��ն���Ϣ(len:17864)
struct TConfMcInfo
{
protected:
	u16					m_wMcuIdx;                      //Mcu��
public:
    u8                  m_byLastPos;                    //��һ����λ��
	TMtExt				m_atMtExt[MAXNUM_CONF_MT];		//�ն��б�
	TMcMtStatus			m_atMtStatus[MAXNUM_CONF_MT];	//�ն�״̬
	TCConfViewInfo      m_tConfViewInfo; 
	TCConfAudioInfo     m_tConfAudioInfo; 
	u32                 m_dwSpyViewId;                  //ѡ����ͼ
	u32                 m_dwSpyVideoId;                 //ѡ����ͼ����Ƶ����ID
	TMt                 m_tSpyMt;						//���뵽mcuѡ����ͼ���ն�
	TMt                 m_tSrcMt;                       //Mcu���´��ն�
	u8                  m_byIsLocked;                   //��Mcu�Ƿ�����
	TMt                 m_tMMcuViewMt;                  //��MCU���ϼ�MCU�����ն���Ϣ
	TMt					m_tLastMMcuViewMt;				
	u8                  m_byIsSupMultSpy;               //��mcu�Ƿ�֧�ֶ໥��
	u32					m_dwMaxSpyBW;					//��mcu��֧�ֵ����ش�����
	s32					m_nRemainSpyBW;					//��mcuĿǰʣ�����
	u8					m_byIsSupMultCas;				//��mcu�Ƿ�֧�ֶ༶��(3��������)
public:
	TConfMcInfo(){ SetNull(); }
	void SetNull() { memset(this, 0, sizeof(TConfMcInfo)); m_wMcuIdx = INVALID_MCUIDX;}
	BOOL32 IsNull( void ) { return m_wMcuIdx == INVALID_MCUIDX ? TRUE : FALSE; }

	u16 GetMcuIdx( void ){ return ntohs(m_wMcuIdx); }
	void SetMcuIdx( u16 wMcuIdx ){
		m_wMcuIdx = htons( wMcuIdx );
	}
	
	TMcMtStatus *GetMtStatus(const TMt& tMt)
	{
		for(s32 nLoop=0; nLoop<MAXNUM_CONF_MT; nLoop++)
		{
			if(m_atMtStatus[nLoop].GetMcuId() == tMt.GetMcuId()&&
				m_atMtStatus[nLoop].GetMtId() == tMt.GetMtId())
			{
				return &(m_atMtStatus[nLoop]);
			}
		}
		return NULL;
	}

	void SetMtStatus( const u8 byMtId, const TMcMtStatus tMtStatus )
	{
		if( byMtId > 0 && byMtId <= MAXNUM_CONF_MT)
		{
			memcpy( &m_atMtStatus[byMtId-1], &tMtStatus, sizeof(m_atMtStatus[byMtId-1]) );
		}
	}

	//2010/07/16 lukunpeng ��ȡĳ�ն���չ��Ϣ
	TMtExt *GetMtExt(TMt &tMt)
	{
		for(s32 nLoop=0; nLoop<MAXNUM_CONF_MT; nLoop++)
		{
			if(m_atMtExt[nLoop].GetMcuId() == tMt.GetMcuId()&&
				m_atMtExt[nLoop].GetMtId() == tMt.GetMtId())
			{
				return &(m_atMtExt[nLoop]);
			}
		}
		return NULL;
	}

	void SetIsLocked(BOOL32 bLock) { m_byIsLocked = GETBBYTE(bLock); }
	BOOL32 IsLocked() { return ISTRUE(m_byIsLocked); }

	// [pengjie 2010/8/11] �ն��Ƿ��Զ���������ʾ
	void SetMtAutoInMix( const TMt& tMt, BOOL32 bIsAuto )
	{ 
		for(s32 nLoop = 0; nLoop < MAXNUM_CONF_MT; nLoop++)
		{
			if( m_atMtStatus[nLoop].GetMcuId() == tMt.GetMcuId()&&
				m_atMtStatus[nLoop].GetMtId() == tMt.GetMtId())
			{
				m_atMtStatus[nLoop].SetIsAutoInMix( GETBBYTE(bIsAuto) );
				return;
			}
		}
	}

	BOOL32 GetMtAutoInMix( const TMt& tMt )
	{
		for(s32 nLoop = 0; nLoop < MAXNUM_CONF_MT; nLoop++)
		{
			if( m_atMtStatus[nLoop].GetMcuId() == tMt.GetMcuId()&&
				m_atMtStatus[nLoop].GetMtId() == tMt.GetMtId())
			{
				return ISTRUE( m_atMtStatus[nLoop].GetIsAutoInMix() );
			}
		}
		return FALSE;
	}
	// End

};

// ����������Ϣ
struct TEqpReqInfo{
	
public:

	TEqpReqInfo()
	{
		m_bSupport = FALSE;
		m_byChannels = 0;
	}

	BOOL32 IsNeedEqp( ) const  // is need the eqp
	{ 
		return m_bSupport; 
	}
	void SetNeedEqp( BOOL32 bNeed )  // set 
	{
		m_bSupport = bNeed;
	}
	void SetNeedChannles( u8 byChannles )
	{
		m_byChannels = byChannles;
	}
	u8  GetSupportChannels( void ) const
	{
		return m_byChannels;
	}

private:
	BOOL32 m_bSupport;
	u8     m_byChannels;  // ��Bas,ָ��������Ƶͨ������Rec��ָ����¼��ͨ��
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// bas ������
struct TBasReqInfo : public TEqpReqInfo{

public:
	TBasReqInfo(){
		m_byAudChannels = 0;
	}
	
	u8 GetAudChannels(void) const
	{
		return m_byAudChannels;
	}
	void SetAudChannels(u8 byChannels )
	{
		m_byAudChannels = byChannels;
	}
private:
	u8 m_byAudChannels;  // audio channels
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// hdbas ������
struct THDBasReqInfo : public TEqpReqInfo
{
    
public:
    THDBasReqInfo()
    {
        m_byDVidChannels = 0;
    }
    
    u8 GetDVidChannels(void) const
    {
        return m_byDVidChannels;
    }
    void SetDVidChannels(u8 byChannels )
    {
        m_byDVidChannels = byChannels;
    }
    u8 GetDVidH263pChn(void) const
    {
        return m_byDVidH263pChn;
    }
    void SetDVidH263pChn(u8 byChn)
    {
        m_byDVidH263pChn = byChn;
    }
    u8 GetMpuChn(void) const
    {
        return m_byMpuChnNum;
    }
    void SetMpuChn(u8 byChn)
    {
        m_byMpuChnNum = byChn;
    }
private:
    u8 m_byDVidChannels;// double video channels
    u8 m_byDVidH263pChn;
    u8 m_byMpuChnNum;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// Rec ������
struct TRecReqInfo : public TEqpReqInfo{

public:
	TRecReqInfo(){
		m_byPlayChannels = 0;
	}

	u8 GetPlayChannels(void) const
	{
		return m_byPlayChannels;
	}

	void SetPlayChannels( u8 byPlayChannels )
	{
		m_byPlayChannels = byPlayChannels;
	}
	
private:
	u8 m_byPlayChannels; // ����ͨ��
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

enum emBasResDataType
{
	emMODE_VIDEO = 0,
	emMODE_SECVIDEO,
	emMODE_END
};

//ռ��ͨ��������Դ
struct TNeedVidAdaptData
{
	u8				m_byDecNeedRes;							//�����������
	TVideoStreamCap	m_atSimCapSet[MAX_CONF_BAS_ADAPT_NUM];	//ý�����Ͷ�Ӧ�������������������
	u8				m_byRealEncNum;							//ʵ�����������·��,H264��other
	u8				m_byMediaMode;							//ý������
	
	TNeedVidAdaptData()
	{
		m_byDecNeedRes = 0;
		m_byRealEncNum = 0;
		m_byMediaMode  = MODE_NONE;
	}

	void  SetVieoSimCapSet(u8 byIdx, const TVideoStreamCap &tVideoCapSet)
	{
		if ( byIdx >=MAX_CONF_BAS_ADAPT_NUM  )
		{
			return;
		}
		
		m_atSimCapSet[byIdx] = tVideoCapSet;
	}

	//���⴦�����720 30 �� 720 60ͬʱ���ڣ������720  60
	void   Filter720P60FPS()
	{	
		if ( m_byMediaMode != MODE_VIDEO && m_byMediaMode != MODE_SECVIDEO)
		{
			return;
		}

		//������¼720P������λ���±꣬��һ��720P 30BP���ڶ���720 60BP��������720 30HP�����ĸ�720 60HP
		u8 awRecordIdx[4] = {0XFF,0XFF,0XFF,0XFF};
		for (u8 byIdx =0; byIdx< m_byRealEncNum; byIdx++)
		{
			if ( m_atSimCapSet[byIdx].GetMediaType() == MEDIA_TYPE_NULL )
			{
				continue;
			}

			if ( MEDIA_TYPE_H264 == m_atSimCapSet[byIdx].GetMediaType() && 
				VIDEO_FORMAT_HD720 == m_atSimCapSet[byIdx].GetResolution()
				)
			{
				if ( emBpAttrb == m_atSimCapSet[byIdx].GetH264ProfileAttrb() )
				{
					if ( 30 == m_atSimCapSet[byIdx].GetUserDefFrameRate() || 
						25 == m_atSimCapSet[byIdx].GetUserDefFrameRate()
						)
					{
						awRecordIdx[0] = byIdx;
					}
					else if ( 60 == m_atSimCapSet[byIdx].GetUserDefFrameRate() || 
						50 == m_atSimCapSet[byIdx].GetUserDefFrameRate()
						)
					{
						awRecordIdx[1] = byIdx;
					}
				}
				else if (emHpAttrb == m_atSimCapSet[byIdx].GetH264ProfileAttrb() )
				{
					if ( 30 == m_atSimCapSet[byIdx].GetUserDefFrameRate() || 
						25 == m_atSimCapSet[byIdx].GetUserDefFrameRate()
						)
					{
						awRecordIdx[2] = byIdx;
					}
					else if ( 60 == m_atSimCapSet[byIdx].GetUserDefFrameRate() || 
						50 == m_atSimCapSet[byIdx].GetUserDefFrameRate()
						)
					{
						awRecordIdx[3] = byIdx;
					}	
				}
			}
		}
		
		BOOL32 bIsNeedChange = FALSE;
		u8 byChangeIdx = 0;
		//ȥ��720 BP �� 60
		if ( awRecordIdx[0] != 0XFF && 
			awRecordIdx[1] != 0XFF
			)
		{
			byChangeIdx= awRecordIdx[1];
			m_atSimCapSet[byChangeIdx].Clear();
			bIsNeedChange = TRUE;
		}
		
		//ȥ��720 HP �� 60
		if ( awRecordIdx[2] != 0XFF && 
			awRecordIdx[3] != 0XFF
			)
		{
			byChangeIdx= awRecordIdx[3];
			m_atSimCapSet[byChangeIdx].Clear();
			bIsNeedChange = TRUE;
		}

		if ( TRUE == bIsNeedChange )
		{
			TVideoStreamCap	atTempSimCapSet[MAX_CONF_BAS_ADAPT_NUM];
			u8			byTempEncNum = 0;
			for (u8 byIdx =0; byIdx< m_byRealEncNum; byIdx++)
			{
				if ( m_atSimCapSet[byIdx].GetMediaType() == MEDIA_TYPE_NULL )
				{
					continue;
				}
				atTempSimCapSet[byTempEncNum] = m_atSimCapSet[byIdx];
				byTempEncNum++;
				m_atSimCapSet[byIdx].Clear();
			}
			memcpy(m_atSimCapSet,atTempSimCapSet,sizeof(atTempSimCapSet));
			m_byRealEncNum = byTempEncNum;
		}
		return;
	}

	void Filter720P60FPSOnlyFor8KH()
	{
		//Ŀǰ8KH �����������и�ǰ�� ģ�����������ѡʱ1080 30BP��720 HP֮�以��
		//Filter720P60FPSOnlyFor8KH ��������(2)(3)(4)(5)������
		//(2)��ѡ��72060  BP,û��ѡ720 30BP, ���⴦��720 60 BP��һ�������720 30BP��ռͨ��
		//(3)��ѡ��72060  BP,��ѡ720 30BP, ���⴦��720 60 BP��һ�������720 30BP��ռͨ��
		//(4)��ѡ��720 60 HP,û��ѡ720 30HP�����⴦��720 60 HP��һ�������720 30HP��ռͨ��
		//(5)��ѡ��72060  HP,��ѡ720 30HP, ���⴦��720 60 BP��һ�������720 30HP��ռͨ��
		//�ȵ���Filter720P60FPS����һ�� 720 60,720 30һ��ѡ�����
		Filter720P60FPS();
		//Ȼ�����������֮������������Ƿ���720 60�ģ��еĻ��ͽ�720 60����
		for (u8 byIdx =0; byIdx< m_byRealEncNum; byIdx++)
		{
			if ( m_atSimCapSet[byIdx].GetMediaType() == MEDIA_TYPE_NULL )
			{
				continue;
			}
			
			u8 byFrameRate = m_atSimCapSet[byIdx].GetUserDefFrameRate();
			if ( byFrameRate == 60 || byFrameRate == 50 )
			{
				m_atSimCapSet[byIdx].SetUserDefFrameRate(byFrameRate/2);
			}
		}
	}
};

struct TNeedAudAdaptData
{
	TAudioTypeDesc		m_atAudTypeDesc[MAXNUM_CONF_AUDIOTYPE];		//��Ƶ������������������
	u8					m_byNeedAudEncNum;							//������Ƶ�������·��

	TNeedAudAdaptData()
	{
		m_byNeedAudEncNum = 0;
	}
};


struct	TBasAdaptParamExt
{
	TMediaEncrypt		m_tEncrypt;		//���ܲ���
	TDoublePayload		m_tDVPayload;	//��Ƶ˫�غ�
	TDoublePayload		m_tDAPayload;	//��Ƶ˫�غ�
	u8					m_byBasAudDecPT;	//��Ƶ�����ʽ
	u8					m_byBasVidDectPT;	//��Ƶ�����ʽ
	u8					m_byIsRcvH264DependInMark;//medianet��264���ղ���,����Bug00158425
	//CConfId				m_cConfId;	
	TCapSupportEx	    m_tCapSupportEx;
	TLogicalChannel		m_tSrcLgcChn;
	BOOL32				m_bIsSpecialChn;		//���MPU2,�Ƿ��ǲ���Ҫ�ı������ͨ����Ŀǰ����¼�����ļ������Բ���Ҫ�ı������
	TBasAdaptParamExt()
	{
		m_bIsSpecialChn = FALSE;
	}
};


// ����������
struct TEqpCapacity{
	
public:
	TEqpReqInfo  m_tPrsCap;
	TEqpReqInfo  m_tTvCap;
	TEqpReqInfo  m_tVmpCap;
	TBasReqInfo  m_tBasCap;

	TEqpReqInfo  m_tMpwCap;
	TEqpReqInfo  m_tMixCap;
	TRecReqInfo  m_tRecCap;

    THDBasReqInfo m_tHDBasCap;
	
// add method to operate the member
	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//����Mc�ϵ�(len:1000384)
struct TConfOtherMcTable
{
private:
	TConfMcInfo m_atConfOtherMcInfo[MAXNUM_CONFSUB_MCU];  //����Mc�ϵ��ն���Ϣ����

public:
	static const u16 GetMaxMcuNum() { return MAXNUM_CONFSUB_MCU; }

	TConfMcInfo* AddMcInfo( const u16 wMcuIdx );
	// liuxu, ����McuIdx��������TConfMcInfo
	BOOL32		SetMcInfo(const u16 wMcuIdx, const TConfMcInfo& tMcInfo);	

	BOOL32      RemoveMcInfo(const u16 wMcuIdx);

	TConfMcInfo* GetMcInfo( const u16 wMcuIdx);
    BOOL32      SetMtInMixing(const TMt& tMt);
    BOOL32      IsMtInMixing(const TMt& tMt) ;
    BOOL32      ClearMtInMixing(const TMcu& tMcu);

	BOOL32      GetIsMcuSupMultSpy(const u16 wMcuIdx); // �ж�ĳ���¼�mcu�Ƿ�֧�ֶ�ش�   
	BOOL32      SetMcuSupMultSpy( const u16 wMcuIdx );  // ��־ĳ���¼���mcu֧�ֶ�ش�
	BOOL32		GetMultiSpyBW( const u16 wMcuIdx, u32 &dwMaxSpyBW, s32 &nRemainSpyBW);
	BOOL32		SetMcuSupMultSpyMaxBW( const u16 wMcuIdx, const u32 dwMaxSpyBW);
	BOOL32		SetMcuSupMultSpyRemainBW( const u16 wMcuIdx, const s32 nRemainSpyBW);
	
	void Init( void )
	{
		u16 wIdx = 0;
		while( wIdx < MAXNUM_CONFSUB_MCU )
		{
			m_atConfOtherMcInfo[wIdx].SetNull();
			++wIdx;
		}
	}
	
	BOOL32		SetMcuSupMultCas( const u16 wMcuIdx ); // ��־ĳ���¼���mcu֧�ֶ༶�� 
	BOOL32		IsMcuSupMultCas( const u16 wMcuIdx );	// �ж�ĳ���¼�mcu�Ƿ�֧�ֶ༶��
};

//������Ϣ�ṹ(len:8500)
struct TSwitchTable
{
	TSwitchChannel m_atSwitchChannel[MAX_SWITCH_CHANNEL];//����ͨ���б�
};

//���齻���� (len: 544000/17000(80000B))
struct TConfSwitchTable
{ 
protected:
    //������Ϣ������ �±�7��15���ڱ������⽻��
#ifndef _MINIMCU_
    TSwitchTable m_tSwitchTable[MAXNUM_DRI];  
#else
    TSwitchTable m_tSwitchTable[2];     // 8000B ֻ��Ҫ���� MPC/xDSC 2��MP��
                                        // �±�0����xDSC���±�1����MPC����������
#endif
	u16	m_wBrdSrcRcvPort;
public:	
	TConfSwitchTable( void )
	{
		memset( m_tSwitchTable, 0, sizeof( m_tSwitchTable ) );
	}

    BOOL32 AddSwitchTable( u8 byMpId, TSwitchChannel *ptSwitchChannel );//���ӽ�����Ϣ��	
	BOOL32 RemoveSwitchTable( u8 byMpId, TSwitchChannel *ptSwitchChannel );//�Ƴ�������Ϣ��
    void ProcMultiToOneSwitch( TSwitchChannel *ptSwitchChannel, u8 byAct );//�����㵽һ��Ľ���
    void ProcBrdSwitch( u8 byNum, TSwitchChannel *ptSwitchChannel, u8 byAct, u8 byMpId, BOOL32 bForcely = FALSE);   //����㲥����
	TSwitchChannel *GetSwitchChannel( u8 byMpId, u16 wIdx )
	{
        #ifdef _MINIMCU_
        if (byMpId > 1)
        {
            byMpId = 2;
        }
        #endif
		return &m_tSwitchTable[byMpId-1].m_atSwitchChannel[wIdx];
	}
	BOOL32 IsValidSwitchSrcIp( u32 dwSrcIp, u16 wRcvPort, u8 byMode );
    BOOL32 IsValidCommonSwitchSrcIp( u32 dwSrcIp, u16 wRcvPort, u8 byMode );
    //BOOL32 IsValidBroadSwitchSrcIp( u32 dwSrcIp, u16 wRcvPort, u8 byMode );
    void ClearSwitchTable(u8 byMpId);
	void ClearRtcpSwitchTable(u8 byMpId, u32 dwMpIp);

	u16 GetBrdSrcRcvPort(){ return m_wBrdSrcRcvPort; }
	void SetBrdSrcRcvPort( u16 wBrdSrcRcvPort ){ m_wBrdSrcRcvPort = wBrdSrcRcvPort; }

	TSwitchTable *GetSwitchTable(u8 byMpId)
	{
#ifdef _MINIMCU_
        if (byMpId > 1)
        {
            byMpId = 2;
        }
#endif
		return &m_tSwitchTable[byMpId-1]; 
	}

	BOOL32 SetSwitchTable(u8 byMpId, TSwitchTable *ptSwitchTable)
	{
		if (NULL == ptSwitchTable)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[SetSwitchTable] ptSwitchTable is null!\n");
			return FALSE;
		}

		u8 byMaxIdx = 0;
#ifdef _MINIMCU_
        if (byMpId > 1)
        {
            byMpId = 2;
        }
#endif
		if (byMpId > MAXNUM_DRI)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[SetSwitchTable] byMpId.%d is larger than MAXNUM_DRI!\n",
									byMpId, MAXNUM_DRI);
			return FALSE;
		}

		memcpy(&m_tSwitchTable[byMpId-1], ptSwitchTable, sizeof(TSwitchTable));
		return TRUE;
	}
};

struct TTvWallInfo
{
    TEqp m_tTvWallEqp;
	//zjl[20091208]�¶���(Ԥ��ͨ������56)
    TMt  m_atTvWallMember[MAXNUM_TVWALL_CHNNL_INSMOUDLE];

public:
    TTvWallInfo(void)
    {
        memset(this, 0, sizeof(TTvWallInfo));
    }

    void SetTvWallEqp(TEqp tTvWall)
    {
        m_tTvWallEqp = tTvWall;
    }

    TEqp GetTvWallEqp(void)
    {
        return m_tTvWallEqp;
    }

    void SetTvWallMember(u8 byMtPos, TMt tMember)
    {
		//zjl[20091208]�¶���(Ԥ��ͨ������56)
        if(byMtPos < MAXNUM_TVWALL_CHNNL_INSMOUDLE)
        { 
            m_atTvWallMember[byMtPos] = tMember; 
        }
    }

    void RemoveMtByMtIdx(u8 byMtIdx, u8& byTvId )
    {
        byTvId = 0;
		//zjl[20091208]�¶���(Ԥ��ͨ������56)
        for(u8 byMtLp = 0; byMtLp < MAXNUM_TVWALL_CHNNL_INSMOUDLE; byMtLp++)
        {
            if( m_atTvWallMember[byMtLp].GetMtId() == byMtIdx )
            {
                byTvId = m_tTvWallEqp.GetEqpId();
                m_atTvWallMember[byMtLp].SetNull();
            }
        }    
    }

    void ClearTvMember(void)
    {
        memset(m_atTvWallMember, 0, sizeof(m_atTvWallMember));
    }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// VCS����ģ��
struct THDUModule 
{
public:
	THDUModule()
	{
		SetNull();
	}

	void SetNull() 
	{
		m_byHduChnlNum = 0;
		EmptyHduChnlInfo();
	}

	void EmptyHduChnlInfo()
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_CONF_HDUBRD * MAXNUM_HDU_CHANNEL; byIdx++)
		{
			m_atHduChnlInfo[byIdx].SetNull();
		}
	}

	void   SetHduChnlNum(u8 byHduChnlNum)       { m_byHduChnlNum = byHduChnlNum; }
	
	u8     GetHduChnlNum( const BOOL32 bVcsConf ) const 
	{
		if (!bVcsConf)
		{
			return m_byHduChnlNum;
		}else
		{
			u8 byVcsChnnlNum = 0;
			for ( u8 byLoop = 0; byLoop < m_byHduChnlNum && byLoop < (MAXNUM_CONF_HDUBRD * MAXNUM_HDU_CHANNEL); byLoop++ )
			{
				if( m_atHduChnlInfo[byLoop].GetMemberType() == TW_MEMBERTYPE_VCSAUTOSPEC )
				{
					byVcsChnnlNum++;
				}
			}
			
			return byVcsChnnlNum;
		}		
	}

	void   SetHduModuleInfo(u8 byHduChnlNum, const s8* pbyHduChnlInfo)
	{
		if (byHduChnlNum >= MAXNUM_HDUBRD * MAXNUM_CONF_HDUBRD)
		{			
			return;
		}

		EmptyHduChnlInfo();
		m_byHduChnlNum = byHduChnlNum;
		memcpy(m_atHduChnlInfo, pbyHduChnlInfo, byHduChnlNum * sizeof(THduModChnlInfo));
	}
	u8*   GetHduModuleInfo()  { return (u8*)m_atHduChnlInfo; }
	const THduModChnlInfo& GetOneHduChnlInfo(u8 byChnlIdx) { return m_atHduChnlInfo[byChnlIdx]; }

private:
	u8                m_byHduChnlNum;                                             //���ø�����ĵ���ǽͨ������
    THduModChnlInfo   m_atHduChnlInfo[MAXNUM_CONF_HDUBRD * MAXNUM_HDU_CHANNEL];        //����ͨ����Ϣ
	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//����ǽ�໭��ͨ����ϸ��Ϣ(len:43bytes)
struct THduVmpChannlInfo
{
public:
	u8 m_byHduEqpId;
	u8 m_byChIdx;
	u8 m_byVmpStyle;   //����ϳɷ�� ֵ1��ʾ4�����4����ͨ����
	u8 m_abyTvWallMember[HDUCHN_MODE_MAXCHNNUM]; //�ն�ID�������ն�ID��
	u8 m_abyMemberType[HDUCHN_MODE_MAXCHNNUM];   //����ǽ��Ա��������,TW_MEMBERTYPE_MCSSPEC,TW_MEMBERTYPE_SPEAKER...

public:
	void Clear( void ){memset(this,0,sizeof(THduVmpChannlInfo));}
	
	BOOL32 IsValid ( void ) const
	{
		return (m_byHduEqpId != 0 && m_byVmpStyle != 0);
	}

	void Print( void )
	{
		if(m_byHduEqpId != 0)
		{
			StaticLog("HduVmpChnnlInfo:EqpId<%d> ChnnIdx<%d> VmpStyle<%d>\n",m_byHduEqpId,m_byChIdx,m_byVmpStyle);
			for(u8 byIdx = 0; byIdx < HDU_MODEFOUR_MAX_SUBCHNNUM; byIdx++ )
			{
				if( m_abyTvWallMember[byIdx]!=0 || m_abyMemberType[byIdx]!=0 )
				{
					StaticLog("\tSubChnIdx:%d MemberId:%d MemberType:%d\n",byIdx,m_abyTvWallMember[byIdx],m_abyMemberType[byIdx]);
				}
			}
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//����ǽ�໭��ͨ��ģ��Ԥ�������������ǽ������(len:15303bytes)
struct THduVmpModuleOrTHDTvWall
{
public:
	u8 m_byUnionType;	//��ʶ������UHduModuleUnion���ô洢THDTvWall�ṹ����THduVmpChannlInfo����
			  			    //1��ʾTHDTvWall��2��ʾTHduVmpChannlInfo����
	union UHduModuleUnion
	{
		THDTvWall  m_tHDTvWall; //�洢VCS�������ǽģ����Ϣ, �Ѿ�����
		THduVmpChannlInfo m_atHduVmpChnInfo[(HDUID_MAX-HDUID_MIN+1)*MAXNUM_HDU_CHANNEL]; //����ǽ�໭��ͨ����Ϣ(6020 byte)
	}m_union;
public:
	THduVmpModuleOrTHDTvWall ( void )
	{
		Clear();
	}

	void Clear( void )
	{
		memset(this,0,sizeof(THduVmpModuleOrTHDTvWall));
	}

	//�ж��������Ƿ��ŵ���HDU�໭��ͨ����Ϣ
	BOOL32 IsUnionStoreHduVmpInfo( void ) const
	{
		return (m_byUnionType==2);
	}

	//�ж��������Ƿ��ŵ���HDTvwall��Ϣ
	BOOL32 IsUnionStoreHDTvwall( void ) const
	{
		return (m_byUnionType==1);
	}

	/*==============================================================================
	������    :  SetHduVmpChnnlInfo
	����      :  ����HDU�໭��ͨ����Ϣ
	�㷨ʵ��  :  
	����˵��  :  THduVmpChannlInfo *ptVmpChnInfo[IN]�໭��ͨ�������׵�ַ
				 u8 byChnnlNum[IN] ͨ����Ŀ
	����ֵ˵��:  �ɹ�����TRUE������FALSE
	-------------------------------------------------------------------------------
	�޸ļ�¼  :  
	��  ��       �汾          �޸���          �߶���          �޸ļ�¼
	2013-3-8	 4.7.2         chendaiwei						create        
	==============================================================================*/
	BOOL32 SetHduVmpChnnlInfo( THduVmpChannlInfo *ptVmpChnInfo, u8 byChnnlNum)
	{
		BOOL32 bResult = FALSE;	
		
		if ( ptVmpChnInfo == NULL || byChnnlNum == 0 || byChnnlNum > (HDUID_MAX-HDUID_MIN+1)*MAXNUM_HDU_CHANNEL )
		{ 
			StaticLog("[SetVmpChnnlInfo] invalid param!\n");
		}
		else
		{
			m_byUnionType = 2;
			memset(&m_union,0,sizeof(m_union));
			memcpy(&m_union.m_atHduVmpChnInfo[0],ptVmpChnInfo,sizeof(THduVmpChannlInfo)*byChnnlNum);

			bResult = TRUE;
		}

		return bResult;
	}

	/*==============================================================================
	������    :  SetHduVmpSubChnlMember
	����      :  ����HDU�໭����ͨ����ԱΪmtAliasidx
	�㷨ʵ��  :  
	����˵��  :  u8 byLoopChIdx ͨ�������±�
				 u8 bySubChIdx  ��ͨ�������±�
				 u8 byMtaliasIdx �ն˱��������±�
	����ֵ˵��:  �ɹ�����TRUE������FALSE
	-------------------------------------------------------------------------------
	�޸ļ�¼  :  
	��  ��       �汾          �޸���          �߶���          �޸ļ�¼
	2013-3-8	 4.7.2         chendaiwei						create        
	==============================================================================*/
	BOOL32 SetHduVmpSubChnlMember(u8 byLoopChIdx, u8 bySubChIdx, u8 byMtAliasIdx)
	{
		if(byLoopChIdx >= (HDUID_MAX-HDUID_MIN+1)*MAXNUM_HDU_CHANNEL 
			|| bySubChIdx >=HDU_MODEFOUR_MAX_SUBCHNNUM
			|| byMtAliasIdx > MAXNUM_CONF_MT)
		{
			return FALSE;
		}
		else
		{
			m_union.m_atHduVmpChnInfo[byLoopChIdx].m_abyTvWallMember[bySubChIdx] = byMtAliasIdx;

			return TRUE;
		}
	}

	void Print ( void )
	{
		if( m_byUnionType == 2)
		{
			for( u8 byIdx = 0; byIdx <(HDUID_MAX-HDUID_MIN+1)*MAXNUM_HDU_CHANNEL; byIdx++)
			{
				m_union.m_atHduVmpChnInfo[byIdx].Print();
			}
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//����ϳ�ģ����Ϣ(len: 58)
struct TVmpModuleInfo 
{
public:
	u8	m_byVMPAuto;      //�Ƿ����Զ�����ϳ� 0-�� 1-��
	u8	m_byVMPBrdst;     //�ϳ�ͼ���Ƿ����ն˹㲥 0-�� 1-�� 
	u8  m_byVMPStyle;     //����ϳɷ��,�μ�mcuconst.h�л���ϳɷ����
    u8  m_byVMPSchemeId;  //�ϳɷ�񷽰����,���֧��5�׷���,1-5
    u8  m_byVMPMode;      //ͼ�񸴺Ϸ�ʽ: 0-��ͼ�񸴺� 1-��ػ���ϯ����ͼ�񸴺� 2-�Զ�ͼ�񸴺�(��̬���������ó�Ա)
	u8  m_byRimEnabled;	  //�Ƿ�ʹ�ñ߿�: 0-��ʹ��(Ĭ��) 1-ʹ��;
	u8	m_byVMPBatchPoll; // xliang [12/18/2008] �Ƿ���������ѯ 0-��
	u8	m_byVMPSeeByChairman;	// �Ƿ���ϯMTѡ��	0-��

	u8  m_abyVmpMember[MAXNUM_MPU2VMP_MEMBER];        //���ָ���Ļ���ϳɳ�Ա������������ʱTMtAlias��������+1��
    u8  m_abyMemberType[MAXNUM_MPU2VMP_MEMBER];       //����ϳɳ�Ա�ĸ��淽ʽ, VMP_MEMBERTYPE_MCSSPEC, VMP_MEMBERTYPE_SPEAKER...

public:
	TVmpModuleInfo ( void )
	{
		Clear();
	}

	~TVmpModuleInfo( void )
	{
		Clear();
	}

	void Clear ( void )
	{
		memset(this,0,sizeof(*this));
	}

	/*====================================================================
	������      ��SetVmpChnnlMember
	����        ������vmpģ���Ա����
	�㷨ʵ��    ��
	����ȫ�ֱ�����
	�������˵����[IN]byNum �����TVmpChnnlMember��Ŀ��
				  [IN]ptMember �����TVmpChnnlMember�����׵�ַ 
	����ֵ˵��  ������ɹ�����TRUE�����򷵻�FALSE
	----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	2013/03/15  4.0         chendaiwei       ����
	====================================================================*/
	BOOL32 SetVmpChnnlMember ( TVmpChnnlMember * ptMember, u8 byNum)
	{
		BOOL32 bResult = FALSE;	
		
		if ( ptMember == NULL || byNum == 0 || byNum > (MAXNUM_MPU2VMP_MEMBER-MAXNUM_MPUSVMP_MEMBER) )
		{ 
			StaticLog("[SetVmpChnnlMember] invalid param!\n");
		}
		else
		{
			for( u8 byIdx = 0; byIdx < byNum;  byIdx++ )
			{
				if(ptMember[byIdx].m_byChnIdx < MAXNUM_MPU2VMP_MEMBER)
				{
					m_abyVmpMember[ptMember[byIdx].m_byChnIdx] = ptMember[byIdx].m_byVmpMember;
					m_abyMemberType[ptMember[byIdx].m_byChnIdx] = ptMember[byIdx].m_byMemberType;
				}
			}

			bResult = TRUE;
		}

		return bResult;
	}
		
	void EmptyAllVmpMember ( void )
	{
		for( u8 byIdx = 0; byIdx < sizeof(m_abyVmpMember);byIdx++ )
		{
			//�Ǹ�������
			if(m_abyVmpMember[byIdx] != MAXNUM_CONF_MT+1)
			{
				m_abyVmpMember[byIdx] = 0;
			}
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//�����豸ģ��(len: 2502)
struct TConfEqpModule
{
public:
	TTvWallInfo m_tTvWallInfo[MAXNUM_PERIEQP_CHNNL];	// MCS����ʱ��
    
	TMultiTvWallModule m_tMultiTvWallModule;			//�洢����ǽģ��, MCS/VCS
	TVmpModuleInfo m_tVmpModuleInfo; //58 bytes

	THduVmpModuleOrTHDTvWall m_tHduVmpModuleOrHDTvWall;
	
	THDUModule m_tHduModule;							//vcs����ר��, �洢HDU����ǽģ��          

public:
	TConfEqpModule( void )
	{
		memset( this, 0, sizeof(TConfEqpModule) );
	}//����
    
	void SetVmpMember( u8 byPos, TMt tMember ){ m_tVmpModuleInfo.m_abyVmpMember[byPos] = tMember.GetMtId();}
	void SetVmpMemberType (u8 byPos, u8 byType) { m_tVmpModuleInfo.m_abyMemberType[byPos] = byType;}
    BOOL32 SetTvWallModule(u8 byTvPos, TTvWallModule tTvWallModule){ return m_tMultiTvWallModule.SetTvModule(byTvPos, tTvWallModule); } 
    BOOL32 GetTvWallModule( u8 byTvId, TTvWallModule& tTvWallModule ) { return m_tMultiTvWallModule.GetTvModuleByTvId(byTvId, tTvWallModule); }
	void   SetVmpModule(TVmpModuleInfo& tVmpModule){ m_tVmpModuleInfo = tVmpModule; }

	//���û���ϳɻ�����������ͨ����Ա�ͳ�Ա���ͣ�
	void   SetVmpModuleParam (TVMPParam &tParam)
	{
		u32 byBasicParamLen = sizeof(tParam)-MAXNUM_MPUSVMP_MEMBER*sizeof(TVMPMember);
		memcpy(&m_tVmpModuleInfo,&tParam,byBasicParamLen);
	}

    // ��������2������
    BOOL32 GetTvWallMemberByMt( u8 byTvId, TMt tMt, u8 &byPos, u8 &byMemberType) // �õ��ն���ָ������ǽ�е�λ��
    {
        for(u8 byTvPos = 0; byTvPos < MAXNUM_PERIEQP_CHNNL; byTvPos++)
        {
            if( m_tTvWallInfo[byTvPos].m_tTvWallEqp.GetEqpId() == byTvId )
            {
                for(u8 byMtLp = 0; byMtLp < MAXNUM_TVWALL_CHNNL_INSMOUDLE; byMtLp++)
                {
                    if( m_tTvWallInfo[byTvPos].m_atTvWallMember[byMtLp] == tMt )
                    {
                        byPos = byMtLp;
                        byMemberType = m_tMultiTvWallModule.m_atTvWallModule[byTvPos].m_abyMemberType[byMtLp];
                        return TRUE;
                    }
                }
            }
        }
        return FALSE;
    }

	BOOL32 IsMtInVmp( TMt tMt ) 
	{
		for( u8 byLoop=0; byLoop < MAXNUM_MPU2VMP_MEMBER; byLoop++ )
		{
			if( !tMt.IsNull() && m_tVmpModuleInfo.m_abyVmpMember[byLoop] == tMt.GetMtId())
			{                
				return TRUE;
			}
		}
		return FALSE;
	}

	// 2011-9-1 add by pgf: �Ƴ�VMPģ���е�ָ����Ա
	void RemoveSpecMtFromModul(TMt tMt)
	{
		if ( tMt.IsNull())
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "NULL tMt\n");
		}
		for( u8 byLoop=0; byLoop < MAXNUM_MPU2VMP_MEMBER; byLoop++ )
		{
			if( !tMt.IsNull() && m_tVmpModuleInfo.m_abyVmpMember[byLoop] == tMt.GetMtId() )
			{
				m_tVmpModuleInfo.m_abyVmpMember[byLoop] = 0;
			}
		}
	}

	BOOL32 IsMtAtVmpChannel( u8 byChl, TMt tMt, u8 &byMemberType ) // ���ӷ��س�Ա����
	{
		if( !tMt.IsNull() && m_tVmpModuleInfo.m_abyVmpMember[byChl] == tMt.GetMtId() )
		{
            byMemberType = m_tVmpModuleInfo.m_abyMemberType[byChl];
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
 
	BOOL32 IsMtInTvwall( TMt tMt )
	{
		for(u8 byTvPosLp = 0; byTvPosLp < MAXNUM_PERIEQP_CHNNL; byTvPosLp++)
        {
            for(u8 byMtPosLp = 0; byMtPosLp < MAXNUM_TVWALL_CHNNL_INSMOUDLE; byMtPosLp++)
            {
                if( m_tTvWallInfo[byTvPosLp].m_atTvWallMember[byMtPosLp] == tMt )
		        {
                    return TRUE;
		        }
            }
        }

        return FALSE;
	}

	BOOL32 IsMtInHduVmpChnnl( TMt tMt )
	{
		if(m_tHduVmpModuleOrHDTvWall.IsUnionStoreHduVmpInfo())
		{
			for(u8 byChIdx = 0; byChIdx < (HDUID_MAX-HDUID_MIN+1)*MAXNUM_HDU_CHANNEL; byChIdx++ )
			{
				for(u8 bySubChnIdx = 0; bySubChnIdx < HDU_MODEFOUR_MAX_SUBCHNNUM; bySubChnIdx++ )
				{
					if( tMt.IsLocal()
						&& m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byChIdx].m_abyTvWallMember[bySubChnIdx] == tMt.GetMtId())
					{
						return TRUE;
					}
				}
			}
		}

		return FALSE;
	}

	/*==============================================================================
	������    :  IsMtInHduVmpChnnl
	����      :  �ж��ն��Ƿ����ض�HDU2����Ķ໭����ͨ���У�����ڷ��ظ���ͨ��idx��
				 ��Ա����
	�㷨ʵ��  :  
	����˵��  :  u8 byEqpId[IN] HDU2����ID
			     u8 byChnnlIdx[IN]HDU2�໭��ͨ��idx
				 TMt tMt[IN]�ն�
				 u8 &byMemberType[IN][OUT]��Ա����
				 u8��bySubChnIdx[IN]��ͨ��idx
	����ֵ˵��:  �ն��ڷ���TRUE������FALSE

	-------------------------------------------------------------------------------
	�޸ļ�¼  :  
	��  ��       �汾          �޸���          �߶���          �޸ļ�¼
	2013-3-8	 4.7.2         chendaiwei						create        
	==============================================================================*/
	BOOL32 IsMtInHduVmpChnnl(u8 byEqpId,u8 byChnnlIdx,u8 bySubChnIdx,TMt tMt,u8 &byMemberType)
	{
		byMemberType = 0;

		if(m_tHduVmpModuleOrHDTvWall.IsUnionStoreHduVmpInfo() && tMt.IsLocal())
		{
			for(u8 byLoopChIdx = 0; byLoopChIdx < (HDUID_MAX-HDUID_MIN+1)*MAXNUM_HDU_CHANNEL; byLoopChIdx++ )
			{
				if( m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byLoopChIdx].m_byHduEqpId == byEqpId
					&& m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byLoopChIdx].m_byChIdx == byChnnlIdx
					&& m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byLoopChIdx].m_abyTvWallMember[bySubChnIdx] == tMt.GetMtId())
				{
					byMemberType = m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byLoopChIdx].m_abyMemberType[bySubChnIdx];

					return TRUE;
				}
			}
		}

		return FALSE;
	}

	/*==============================================================================
	������    :  IsMtInHduVmpChnnl
	����      :  �ж��ն��Ƿ����ض�HDU2����Ķ໭����ͨ����
	�㷨ʵ��  :  
	����˵��  :  
			     u8 byLoopChnnlIdx[IN]�໭��ͨ�������±�
				 u8��bySubChnIdx[IN]��ͨ��idx
	����ֵ˵��:  �ն��ڷ���TRUE������FALSE

	-------------------------------------------------------------------------------
	�޸ļ�¼  :  
	��  ��       �汾          �޸���          �߶���          �޸ļ�¼
	2013-3-8	 4.7.2         chendaiwei						create        
	==============================================================================*/
	BOOL32 IsMtInHduVmpChnnl(u8 byLoopChnnlIdx,u8 bySubChnIdx,TMt tMt)
	{
		if(m_tHduVmpModuleOrHDTvWall.IsUnionStoreHduVmpInfo()
	      && m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byLoopChnnlIdx].m_abyTvWallMember[bySubChnIdx] == tMt.GetMtId())
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

    BOOL32 IsMtInTvWallChannel( u8 byTvId, u8 byChannel, TMt tMt, u8 &byMemberType) // ���ӷ��س�Ա����
    {
        for(u8 byTvPos = 0; byTvPos < MAXNUM_PERIEQP_CHNNL; byTvPos++)
        {
            if( m_tTvWallInfo[byTvPos].m_tTvWallEqp.GetEqpId() == byTvId )
            {
                if( m_tTvWallInfo[byTvPos].m_atTvWallMember[byChannel] == tMt )
		        {
                    byMemberType = m_tMultiTvWallModule.m_atTvWallModule[byTvPos].m_abyMemberType[byChannel];
                    return TRUE;
		        }
                else
                {
                    return FALSE;
                }
            }   
        }
		
        return FALSE;
    }

    void GetMultiTvWallModule(TMultiTvWallModule& tMultiTvWallModule)
    { 
        tMultiTvWallModule = m_tMultiTvWallModule;
    }

    void SetMultiTvWallModule(TMultiTvWallModule tMultiTvWallModule )
    { 
        m_tMultiTvWallModule = tMultiTvWallModule;
    }

    void SetTvWallInfo(TMultiTvWallModule& tMultiTvWallModule)
    {
        TTvWallModule tTvWallModule;
        for(u8 byTvLp = 0; byTvLp < tMultiTvWallModule.GetTvModuleNum(); byTvLp++ )
        {
            tTvWallModule.Clear();
            tMultiTvWallModule.GetTvModuleByIdx(byTvLp, tTvWallModule);
            m_tTvWallInfo[byTvLp].SetTvWallEqp(tTvWallModule.GetTvEqp() );
        }
    }

    void SetTvWallMemberInTvInfo( u8 byTvId, u8 byMtPos, TMt tMember)
    { 
        for(u8 byTvLp = 0; byTvLp < MAXNUM_PERIEQP_CHNNL; byTvLp++)
        {
            if(m_tTvWallInfo[byTvLp].GetTvWallEqp().GetEqpId() == byTvId)
            {
                m_tTvWallInfo[byTvLp].SetTvWallMember(byMtPos, tMember);
                break;
            }
        }
    }

    void SetHDTvWall(THDTvWall* ptHDTvWall)
	{
	 	if (NULL == ptHDTvWall)
	 	{
	 		return;
	 	}

		if(ptHDTvWall->IsHDTWCfg())
		{
			m_tHduVmpModuleOrHDTvWall.m_byUnionType = 1;
	 		memcpy(&m_tHduVmpModuleOrHDTvWall.m_union.m_tHDTvWall, ptHDTvWall, sizeof(THDTvWall));
		}
	}

	void GetHDTvWall(THDTvWall& tHDTvWall) 
	{ 
		if(m_tHduVmpModuleOrHDTvWall.IsUnionStoreHDTvwall())
		{
			memcpy(&tHDTvWall, &m_tHduVmpModuleOrHDTvWall.m_union.m_tHDTvWall, sizeof(THDTvWall));
		}
		else
		{
			tHDTvWall.SetNull();
		}
	}

	/*==============================================================================
	������    :  RemoveMtInHduVmpModule
	����      :  ��Hdu�໭��ͨ��Ԥ����ɾ���ն�
	�㷨ʵ��  :  
	����˵��  :  TMt tMt �����ն� 
	����ֵ˵��:  �ɹ�����TRUE������FALSE
	-------------------------------------------------------------------------------
	�޸ļ�¼  :  
	��  ��       �汾          �޸���          �߶���          �޸ļ�¼
	2013-3-8	 4.7.2         chendaiwei						create        
	==============================================================================*/
	void RemoveMtInHduVmpModule (TMt tMt)
	{
		if(m_tHduVmpModuleOrHDTvWall.IsUnionStoreHduVmpInfo())
		{
			for(u8 byChIdx = 0; byChIdx < (HDUID_MAX-HDUID_MIN+1)*MAXNUM_HDU_CHANNEL; byChIdx++ )
			{
				for(u8 bySubChnIdx = 0; bySubChnIdx < HDU_MODEFOUR_MAX_SUBCHNNUM; bySubChnIdx++ )
				if( tMt.IsLocal()
					&& m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byChIdx].m_abyTvWallMember[bySubChnIdx] == tMt.GetMtId())
				{
					m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byChIdx].m_abyTvWallMember[bySubChnIdx] = 0;
					m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byChIdx].m_abyMemberType[bySubChnIdx] = TW_MEMBERTYPE_NULL;
				}
			}
		}

		return;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

typedef struct tagTRefreshParam
{
public:
	u32   dwMcsRefreshInterval;         //second
	u32   dwMcsLastTick;
//	u32   dwMcuCascadeRefreshInterval;  //second
//	u32   dwMcuCascadeStatLastTick;
//	u32   dwMcuCascadeinfoLastTick;
public:
	u32   GetMcsRefreshInterval( void ) //ticks
	{
		return dwMcsRefreshInterval * OspClkRateGet();
	}
//	u32 GetMcuCascadeRefreshInterval( void ) //ticks
//	{					
//		return dwMcuCascadeRefreshInterval * OspClkRateGet();		
//	}
}TRefreshParam;

/*
//������ѯ��Ϣ 
#define POLLING_POS_START   (u8)0xFF
#define POLLING_POS_INVALID (u8)0xFE

struct TConfPollParam
{
protected:
	u8 m_byPollMtNum;  //�μ���ѯ�ն˸���
	u8 m_byCurPollPos; //��ѯ��ǰλ��
    u8 m_bySpecPos;    //�û�ǿ�����õ���ѯλ��
	TMtPollParam m_atMtPollParam[MAXNUM_CONF_MT]; //�μ���ѯ���ն˲����б�
public:

    TConfPollParam()
    {
        memset(this, 0, sizeof(TConfPollParam));
        ClearSpecPos();
    }
    
    void InitPollParam(u8 byMtNum, TMtPollParam* ptParam);
    void SetPollList(u8 byMtNum, TMtPollParam* ptParam);
    u8 GetPolledMtNum() const;
    TMtPollParam* const GetPollMtByIdx(u8 byIdx);
    void SetPolledMtNum(u8 byMtNum){m_byPollMtNum = byMtNum;} // xliang [12/31/2008] 
    
    void SetCurrentIdx(u8 byIdx);
    u8 GetCurrentIdx() const;
    TMtPollParam* const GetCurrentMtPolled();

    BOOL32 IsExistMt(const TMt &tMt, u8 &byIdx);
    void RemoveMtFromList(const TMt &tMt);

    void SpecPollPos(u8 byIdx)
    {
        if ( byIdx < m_byPollMtNum || byIdx == POLLING_POS_START )
        {
            m_bySpecPos = byIdx;
        }
    }
    BOOL32 IsSpecPos() const
    {
        return ( m_bySpecPos != POLLING_POS_INVALID );
    }
    u8   GetSpecPos() const
    {
        u8 byRet = m_bySpecPos;
        return byRet;
    }
    void ClearSpecPos()
    {
        m_bySpecPos = POLLING_POS_INVALID;
    }
    
    void Print() const
    {
        StaticLog("TConfPollParam:\n");
        StaticLog("\t m_byPollMtNum: %d(Current.%d, Spec.%d)\n", m_byPollMtNum, m_byCurPollPos, m_bySpecPos);
        for (u8 byIdx = 0; byIdx < m_byPollMtNum; byIdx ++)
        {
            StaticLog("\t\t Idx.%d Mt: <%d,%d>\n", byIdx, m_atMtPollParam[byIdx].GetMcuId(), m_atMtPollParam[byIdx].GetMtId());
        }
    }
};
*/
// xliang [12/18/2008] ------------
//����ϳ�������ѯ��Ϣ
struct TVmpBatchPollInfo : public TPollInfo
{
protected:
	u32 m_dwFirstPollT;	//��ѯ��һ��ʱ��T
	u32 m_dwPollt;		//��ѯʱ��t
	u8  m_byLoopNoLimit;//��ѯ��������ѭ��(mcs���������ֶ�)
	u8  m_byTmpt;		//��ʱ������ʱʱ��(mcs���������ֶ�)
public:
	TVmpBatchPollInfo()
	{
		memset( this, 0, sizeof(TVmpBatchPollInfo) );
	}
	u32		GetFirstPollT(){	return m_dwFirstPollT; }
	u32		GetPollt(){	return m_dwPollt; }

	BOOL32	IsLoopNoLimit(){ return (m_byLoopNoLimit == 1);}
	void	SetLoopNoLimt(BOOL32 bLoopNoLimit) { m_byLoopNoLimit = bLoopNoLimit ? 1 : 0; }
	
	void	SetTmpt(u8 byTimer) {m_byTmpt = byTimer;}
	u8		GetTmpt(){return m_byTmpt;}
};

/*����ϳ���ѯ����(Vmp������ѯ����ϲ�,��mcusturct.hʵ��)
struct TVmpPollParam: public TConfPollParam
{
protected:
	u8 m_byCurPollMtNum;		//��ǰ�ѽ�����ѯ��mt��Ŀ
	u8 m_byChnlIdx;				//��ǰ�õ�vmpͨ��
	u8 m_byFirst;				//��һ����
	u8 m_byVmpPollOver;			//vmpһ����ѯ����
	//	TMtPollParamEx atMtPollParamEx[192];	//�ն˶�Ӧ��vmpͨ���� FIXME��vcinst���и����Ƶ�
	u8	m_abyCurPollBlokMtId[MAXNUM_MPUSVMP_MEMBER];	//��ŵ�ǰ���������MTid

public:
	TVmpPollParam()
	{
		memset( this, 0, sizeof(TVmpPollParam) );
	}
	u8 GetVmpPolledMtNum()	{ return m_byCurPollMtNum; }
	void SetVmpPolledMtNum(u8 byCurPollMtNum){ m_byCurPollMtNum = byCurPollMtNum ;}
	
	u8 GetVmpChnnlIdx() { return m_byChnlIdx; }
	void SetVmpChnnlIdx( u8 byChnlIdx) {m_byChnlIdx = byChnlIdx; }

	void SetIsVmpPollOver(BOOL32 bVmpPollOver){m_byVmpPollOver = (bVmpPollOver == TRUE? 1:0);}
	BOOL32 IsVmpPollOver(){return (m_byVmpPollOver == 1)?TRUE:FALSE;}

	void SetIsBatchFirst(BOOL32 bFirst){ m_byFirst = (bFirst == TRUE)?1:0;}
	BOOL32 IsBatchFirst(){return (m_byFirst == 1)?TRUE:FALSE;}

    /*==============================================================================
	������    :  SetCurPollBlokMtId
	����      :  ��¼��ǰ�����MT Id
	�㷨ʵ��  :  
	����˵��  :  u8 byMemberId	[in]
				 u8 byMtId		[in]	
	����ֵ˵��:  void
	-------------------------------------------------------------------------------
	�޸ļ�¼  :  
	��  ��       �汾          �޸���          �߶���          �޸ļ�¼
	2008-12-24					Ѧ��							����
	==============================================================================/
	void SetCurPollBlokMtId(u8 byMemberId,u8 byMtId)
	{
		if( (byMemberId < MAXNUM_MPUSVMP_MEMBER ) 
			&& (0 < byMtId && byMtId <= MAXNUM_CONF_MT) )
		{
			m_abyCurPollBlokMtId[byMemberId] = byMtId;
		}
		else
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "Invalid member Id or mt Id.\n");
		}
	}

	/*==============================================================================
	������    :  IsMtInCurPollBlok
	����      :  �ж�ĳ�ն��Ƿ�����ڵ�ǰ������
	�㷨ʵ��  :  
	����˵��  :  u8 byMtId	[in]  Ҫ�жϵ��ն�ID	
				 u8 *byMbId	[out] �ն˶�Ӧ��Member ID
	����ֵ˵��:  BOOL32 
	-------------------------------------------------------------------------------
	�޸ļ�¼  :  
	��  ��       �汾          �޸���          �߶���          �޸ļ�¼
	2008-12-24					Ѧ��							create
	==============================================================================/
	BOOL32 IsMtInCurPollBlok(u8 byMtId,u8 *byMbId)
	{
		for(u8 byMemberId = 0; byMemberId < MAXNUM_MPUSVMP_MEMBER; byMemberId ++)
		{
			if(m_abyCurPollBlokMtId[byMemberId] == byMtId)
			{
				*byMbId = byMemberId;
				return TRUE;
			}
		}
		return FALSE;
	}
	/*==============================================================================
	������    :  GetMtInCurPollBlok
	����      :  ��ȡ��ǰ����ĳͨ���е�MT ID
	�㷨ʵ��  :  
	����˵��  :  u8 byMemberId [in]
	����ֵ˵��:  u8 
	-------------------------------------------------------------------------------
	�޸ļ�¼  :  
	��  ��       �汾          �޸���          �߶���          �޸ļ�¼
	2008-12-24					Ѧ��							create
	==============================================================================/
	u8 GetMtInCurPollBlok(u8 byMemberId)
	{
		return m_abyCurPollBlokMtId[byMemberId];
	}

};*/
/*
//����ǽ��ѯ����
struct TTvWallPollParam : public TConfPollParam
{
protected:
    TEqp m_tTvWall; 
    u8   m_byTvWallChnnl;
	
public:
	TTvWallPollParam()
	{
		memset( this, 0, sizeof(TTvWallPollParam) );
	}

	TEqp GetTvWallEqp( void );              	//��õ���ǽ�豸
	void SetTvWallEqp( const TEqp &tTvWall );	//���õ���ǽ�豸

	u8   GetTvWallChnnl( void );	            //��õ���ǽͨ����
	void SetTvWallChnnl( u8 byChnIndex );		//���õ���ǽͨ����

    void Print() const
    {
        StaticLog("TTvWallPollParam:\n");
        StaticLog("\t m_tTvWall: %d-Chnl.%d\n", m_tTvWall.GetEqpId(), m_byTvWallChnnl);
        TConfPollParam::Print();
    }
};

struct THduPollParam:public TConfPollParam
{
protected:
    TEqp m_tTvWall; 
    u8   m_byTvWallChnnl;
	u8   m_byMode;  // MODE_AUDIO, MODE_VIDEO, MODE_BOTH
	u8   m_byIsStartAsPause;    // �Ƿ�����ͣ״̬��ʼ������ѯ  0 -->���ǣ�1-->��
	u8   m_byReserved2;
	
public:
	THduPollParam()
	{
		memset( this, 0, sizeof(THduPollParam) );
	}

	u8   GetIsStartAsPause( void ){ return m_byIsStartAsPause; }            //���ģʽ
	void SetIsStartAsPause( u8 byIsStartAsPause ){ m_byIsStartAsPause = byIsStartAsPause; }	    //����ģʽ

	u8   GetMode( void ){ return m_byMode; }            //���ģʽ
	void SetMode( u8 byMode ){ m_byMode = byMode; }	    //����ģʽ

	TEqp GetTvWallEqp( void ){ return m_tTvWall; }              	//��õ���ǽ�豸
	void SetTvWallEqp( const TEqp &tTvWall ){ m_tTvWall = tTvWall; }	//���õ���ǽ�豸
	
	u8   GetTvWallChnnl( void ){ return m_byTvWallChnnl; }	            //��õ���ǽͨ����
	void SetTvWallChnnl( u8 byChnIndex ){ m_byTvWallChnnl = byChnIndex; }		//���õ���ǽͨ����
	
    void Print() const
    {
        StaticLog("TTvWallPollParam:\n");
        StaticLog("\t m_tTvWall: %d-Chnl.%d Mode: %d\n", m_tTvWall.GetEqpId(), m_byTvWallChnnl, m_byMode);
        TConfPollParam::Print();
    }
}
;
*/

//�����ǽͨ����ѯ����
//zjlhdupoll
struct TTWMutiPollParam
{
	TTvWallPollParam m_tTWPollParam[MAXNUM_TVWALL_CHNNL];

public:
	TTWMutiPollParam()
	{
		memset( this, 0, sizeof(TTWMutiPollParam));
	}

	//������ѯ��Ϣ
	void SetTWPollInfo(u8 byEqpId, u8 byChnId, TTvWallPollInfo tTWPollInfo)
	{
		for(u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if ((m_tTWPollParam[byIdx].GetTvWall().GetEqpId() == byEqpId &&
				 m_tTWPollParam[byIdx].GetTWChnnl() == byChnId) ||
			 	 m_tTWPollParam[byIdx].GetTvWall().IsNull())
			{
				 m_tTWPollParam[byIdx].SetTWPollInfo(tTWPollInfo);
				 return ;
			}
		}		
	}
    
	//��ȡ��ѯ��Ϣ
	BOOL32 GetTWPollInfo(u8 byEqpId, u8 byChnId, TTvWallPollInfo &tTWPollInfo)
	{
		for(u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if (m_tTWPollParam[byIdx].GetTvWall().GetEqpId() == byEqpId &&
				 m_tTWPollParam[byIdx].GetTWChnnl() == byChnId)
			{
				 tTWPollInfo = m_tTWPollParam[byIdx].GetTWPollInfo();
				 return TRUE;
			}
		}	
		return FALSE;
	}

	BOOL32 SetTWPollParam(u8 byEqpId, u8 byChnId, TTvWallPollParam tTWPollParam)
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if (m_tTWPollParam[byIdx].GetTvWall().GetEqpId() == byEqpId &&
				m_tTWPollParam[byIdx].GetTWChnnl() == byChnId)
			{	
				m_tTWPollParam[byIdx] = tTWPollParam;
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL32 GetTWPollParam(u8 byEqpId, u8 byChnId, TTvWallPollParam &tTWPollParam)
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if (m_tTWPollParam[byIdx].GetTvWall().GetEqpId() == byEqpId &&
				m_tTWPollParam[byIdx].GetTWChnnl() == byChnId)
			{	
				tTWPollParam = m_tTWPollParam[byIdx];
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL32 GetIsStartAsPause(u8 byEqpId, u8 byChnId, u8 &byIsStartAsPause)
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if (m_tTWPollParam[byIdx].GetTvWall().GetEqpId() == byEqpId &&
				m_tTWPollParam[byIdx].GetTWChnnl() == byChnId)
			{
				byIsStartAsPause = m_tTWPollParam[byIdx].GetIsStartAsPause();
				return TRUE;
			}
		}
		return FALSE;
	}
	
	BOOL32 SetIsStartAsPause(u8 byEqpId, u8 byChnId, u8 byIsStartAsPause)
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if (m_tTWPollParam[byIdx].GetTvWall().GetEqpId() == byEqpId &&
				m_tTWPollParam[byIdx].GetTWChnnl() == byChnId)
			{
				m_tTWPollParam[byIdx].SetIsStartAsPause(byIsStartAsPause);
				return TRUE;
			}
		}
		return FALSE;
	}
    
	BOOL32 GetMode(u8 byEqpId, u8 byChnId, u8 &byMode)
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if (m_tTWPollParam[byIdx].GetTvWall().GetEqpId() == byEqpId &&
				m_tTWPollParam[byIdx].GetTWChnnl() == byChnId)
			{
				byMode = m_tTWPollParam[byIdx].GetMediaMode();
				return TRUE;
			}
		}
		return FALSE;
	}
	
    BOOL32 SetMode(u8 byEqpId, u8 byChnId, u8 byMode)
	{
        for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if (m_tTWPollParam[byIdx].GetTvWall().GetEqpId() == byEqpId &&
				m_tTWPollParam[byIdx].GetTWChnnl() == byChnId)
			{
				m_tTWPollParam[byIdx].SetMediaMode(byMode);
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL32 InitPollParam(u8 byEqpId, u8 byChnId, u8 byMtNum, TMtPollParam* ptParam)
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if (m_tTWPollParam[byIdx].GetTvWall().GetEqpId() == byEqpId &&
				m_tTWPollParam[byIdx].GetTWChnnl() == byChnId)
			{
				m_tTWPollParam[byIdx].InitPollParam(byMtNum, ptParam);
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL32 GetPollMtByIdx(u8 byEqpId, u8 byChnId, u8 byMtIdx, TMtPollParam *ptTMtPollParam)
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if (m_tTWPollParam[byIdx].GetTvWall().GetEqpId() == byEqpId &&
				m_tTWPollParam[byIdx].GetTWChnnl() == byChnId)
			{
				ptTMtPollParam = m_tTWPollParam[byIdx].GetPollMtByIdx(byMtIdx);
		    	return TRUE;	
			}
		}
		return FALSE;
	}

	BOOL32 GetCurrentIdx(u8 byEqpId, u8 byChnId, u8 &byCurIdx)
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if (m_tTWPollParam[byIdx].GetTvWall().GetEqpId() == byEqpId &&
				m_tTWPollParam[byIdx].GetTWChnnl() == byChnId)
			{
				byCurIdx = m_tTWPollParam[byIdx].GetCurrentIdx();
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL32 SetCurrentIdx(u8 byEqpId,u8 byChnId, u8 byCurIdx)
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if (m_tTWPollParam[byIdx].GetTvWall().GetEqpId() == byEqpId &&
				m_tTWPollParam[byIdx].GetTWChnnl() == byChnId)
			{
				m_tTWPollParam[byIdx].SetCurrentIdx(byCurIdx);
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL32 GetPolledMtNum(u8 byEqpId,u8 byChnId, u8 &byPolledMtNum)
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if (m_tTWPollParam[byIdx].GetTvWall().GetEqpId() == byEqpId &&
				m_tTWPollParam[byIdx].GetTWChnnl() == byChnId)
			{
				byPolledMtNum = m_tTWPollParam[byIdx].GetPolledMtNum();
				return TRUE;
			}
		}
		return FALSE;
	}
    
	BOOL32 SetPolledMtNum(u8 byEqpId, u8 byChnId, u8 byPolledMtNum)
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if (m_tTWPollParam[byIdx].GetTvWall().GetEqpId() == byEqpId &&
				m_tTWPollParam[byIdx].GetTWChnnl() == byChnId)
			{
				m_tTWPollParam[byIdx].SetPolledMtNum(byPolledMtNum);
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL32 SetPollState(u8 byEqpId, u8 byChnId, u8 byPollState)
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if (m_tTWPollParam[byIdx].GetTvWall().GetEqpId() == byEqpId &&
				m_tTWPollParam[byIdx].GetTWChnnl() == byChnId)
			{
				m_tTWPollParam[byIdx].SetPollState(byPollState);
				return TRUE;
			}
		}
		return FALSE;
	}
	
	BOOL32 GetPollState(u8 byEqpId, u8 byChnId, u8 &byPollState)
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if (m_tTWPollParam[byIdx].GetTvWall().GetEqpId() == byEqpId &&
				m_tTWPollParam[byIdx].GetTWChnnl() == byChnId)
			{
				byPollState = m_tTWPollParam[byIdx].GetPollState();
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL32 SetKeepTime(u8 byEqpId, u8 byChnId, u32 dwKeepTime)
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if (m_tTWPollParam[byIdx].GetTvWall().GetEqpId() == byEqpId &&
				m_tTWPollParam[byIdx].GetTWChnnl() == byChnId)
			{
				m_tTWPollParam[byIdx].SetKeepTime(dwKeepTime);
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL32 GetKeepTime(u8 byEqpId, u8 byChnId, u32 &dwKeepTime)
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if (m_tTWPollParam[byIdx].GetTvWall().GetEqpId() == byEqpId &&
				m_tTWPollParam[byIdx].GetTWChnnl() == byChnId)
			{
				dwKeepTime = m_tTWPollParam[byIdx].GetKeepTime();
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL32 SetPollNum(u8 byEqpId, u8 byChnId, u32 dwPollNum)
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if (m_tTWPollParam[byIdx].GetTvWall().GetEqpId() == byEqpId &&
				m_tTWPollParam[byIdx].GetTWChnnl() == byChnId)
			{
				m_tTWPollParam[byIdx].SetPollNum(dwPollNum);
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL32 GetPollNum(u8 byEqpId, u8 byChnId, u32 &dwPollNum)
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if (m_tTWPollParam[byIdx].GetTvWall().GetEqpId() == byEqpId &&
				m_tTWPollParam[byIdx].GetTWChnnl() == byChnId)
			{
				dwPollNum = m_tTWPollParam[byIdx].GetPollNum();
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL32 GetTimerIdx(u8 byEqpId, u8 byChnId, u32 &dwTimerIdx)
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if(m_tTWPollParam[byIdx].GetTvWall().GetEqpId() == byEqpId &&
				m_tTWPollParam[byIdx].GetTWChnnl() == byChnId)
			{
				dwTimerIdx = byIdx;
				return TRUE;
			}
		}
		return FALSE;
	}
	
	//����ʱ��������ȡ��Ӧͨ����ѯ��Ϣ
	TTvWallPollParam  *GetTWPollParamByTimerIdx(u8 byTimerIdx)
	{
		if (byTimerIdx < MAXNUM_TVWALL_CHNNL)
		{
		   return &m_tTWPollParam[byTimerIdx];
		}
		return NULL;
	}

	//����ͨ����ǰ��ѯ�ն�
	void SetCurPolledMt(u8 byEqpId,u8 byChnId, TMtPollParam tTMtPollParam)
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if( m_tTWPollParam[byIdx].GetTvWall().GetEqpId() == byEqpId &&
				m_tTWPollParam[byIdx].GetTWChnnl() == byChnId)
			{
				m_tTWPollParam[byIdx].SetMtPollParam(tTMtPollParam);
			}
		}
	}
	
	TMt GetCurPolledMt(u8 byEqpId,u8 byChnId)
	{
	    for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if( m_tTWPollParam[byIdx].GetTvWall().GetEqpId() == byEqpId &&
				m_tTWPollParam[byIdx].GetTWChnnl() == byChnId)
			{
				return m_tTWPollParam[byIdx].GetMtPollParam().GetTMt();
			}
		}
		TMt tMt;
		tMt.SetNull();
		return tMt;
	}
	
	//��ǰ�Ƿ���������ѯ�ĵ���ǽͨ��
	BOOL32 HasTWIsPolling()
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if (POLL_STATE_NONE != m_tTWPollParam[byIdx].GetPollState())
			{
				return TRUE;
			}
		}
		return FALSE;
	}

	void Print()
	{
		StaticLog("------------TWMutiPollInfo----------\n");
		
		for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
		{
			if (!m_tTWPollParam[byIdx].GetTvWall().IsNull())
			{
				StaticLog("---------------TableId:%d------------------\n", byIdx);
				StaticLog("---------TW<EqpID:%d, ChnId:%d>---------\n", 
									m_tTWPollParam[byIdx].GetTvWall().GetEqpId(),
									m_tTWPollParam[byIdx].GetTWChnnl());
				StaticLog("---------SchemeIdx:%d--------------------\n", m_tTWPollParam[byIdx].GetSchemeIdx());
				StaticLog("---------PolledMtNum:%d------------------\n", m_tTWPollParam[byIdx].GetPolledMtNum());
				StaticLog("---------MediaMode:%d--------------------\n", m_tTWPollParam[byIdx].GetMediaMode());			
				StaticLog("---------PollNum:%d----------------------\n", m_tTWPollParam[byIdx].GetPollNum());
				StaticLog("---------IsStartAsPause:%d---------------\n", m_tTWPollParam[byIdx].GetIsStartAsPause());
				StaticLog("---------PollStatus:%d-------------------\n", m_tTWPollParam[byIdx].GetPollState());
				StaticLog("---------CurPollMt<McuId:%d, MtId:%d>----\n", 
									m_tTWPollParam[byIdx].GetMtPollParam().GetMcuId(),
									m_tTWPollParam[byIdx].GetMtPollParam().GetMtId());

				TMtPollParam *ptMtPollParam = m_tTWPollParam[byIdx].GetPollMtByIdx(0);
				if (NULL != ptMtPollParam)
				{				
					for (u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
					{
						if (ptMtPollParam[byIdx].GetMcuId() != 0 &&
							ptMtPollParam[byIdx].GetMtId() != 0)
						{
							StaticLog("---------MtList<PosId:%d, McuId:%d, MtId:%d>------\n",
								byIdx,
								ptMtPollParam[byIdx].GetMcuId(),
								ptMtPollParam[byIdx].GetMtId());
						}
					}
				}
			}			
		}
	}
};

// hdu������ѯʱԤ����ÿ��ͨ����Ӧ����ͨ����Ϣ�Ľṹ
struct THduChnlPollInfo
{
protected:
    u8 m_byEqpId;     // hduId
	u8 m_byChnlIdx;   // hduͨ�������� 0��1
    u8 m_byPos;       // ��ǰͨ����������ѯ�����е�λ��
    u8 m_byReserved;
public:
	THduChnlPollInfo()
	{
		memset(this, 0x0, sizeof(THduChnlPollInfo));
	}

	u8    GetEqpId( void ) const { return m_byEqpId; }
    void  SetEqpId( u8 byEqpId ){ m_byEqpId = byEqpId; }

	u8    GetChnlIdx( void ) const { return m_byChnlIdx; }
    void  SetChnlIdx( u8 byChnlIdx ){ m_byChnlIdx = byChnlIdx; }

	u8    GetPosition( void ) const { return m_byPos; }
    void  SetPosition( u8 byPos ){ m_byPos = byPos; }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//hdu������ѯ��Ϣ
struct THduBatchPollInfo 
{
protected:
    u8    m_byHduSchemeIdx;     //  Ԥ��������
	// [pengjie 2010/8/27]
	TMt   m_tNextMt;            //��Ҫ��ѯ����һ���ն�
	// End
	u8    m_byCycles;           //  ��ѯ��������0-->������ѯ
	u8    m_byCurrentCycle;     //  ��ǰ��ѯ��������
	u32   m_byKeepTime;         //  ������ѯʱ����

	u8    m_byChnlPollNum;      //  ����������ѯ��ͨ����
	u8    m_byStatus;           //  ��ǰ������ѯ״̬
	u8    m_byReserved2;
	u8    m_byReserved3;

public:
    THduChnlPollInfo  m_tChnlBatchPollInfo[MAXNUM_HDUCFG_CHNLNUM];    //  ��¼�����Ԥ��������ѯ��hduͨ����Ϣ

public:
	THduBatchPollInfo()
	{
        SetNull();
	}
	void SetNull( void )
	{
// [pengjie 2010/8/28] HDU��ѯ��������
//		m_byHduSchemeIdx = 0; 
//		m_byCurrentMtId = 0;
// 		m_byCycles = 0;
// 		m_byCurrentCycle = 0; 
// 		m_byKeepTime = 0;		
// 		m_byChnlPollNum = 0;
// 	    m_byStatus = POLL_STATE_NONE;
        memset( this, 0x0, sizeof(THduBatchPollInfo) );		
	}

	u8   GetHduSchemeIdx( void ){ return m_byHduSchemeIdx; }
	void SetHduSchemeIdx( u8 byHduSchemeIdx ){ m_byHduSchemeIdx = byHduSchemeIdx; }

	// [pengjie 2010/8/27]
	TMt GetNextPollMt( void )
	{
		return m_tNextMt;
	}
	// End
	void SetNextPollMt( const TMt &tNextMt )
	{
		m_tNextMt = tNextMt;
	}

// [pengjie 2010/8/28] HDU��ѯ��������
// 	void   GetCurrentMtInfo( u8 &byCurrentMcuId,u8 &byCurrentMtId )
// 	{ 
// 		byCurrentMcuId = m_byCurrentMcuId; 
// 		byCurrentMtId  = m_byCurrentMtId; 
// 	}
// 	void SetCurrentMtInfo( u8 byCurrentMcuId,u8 byCurrentMtId )
// 	{ 
// 		m_byCurrentMcuId = byCurrentMcuId; 
// 		m_byCurrentMtId  = byCurrentMtId; 
// 	}

	u32   GetKeepTime( void ){ return m_byKeepTime; }
	void SetKeepTime( u32 byKeepTime ){ m_byKeepTime = byKeepTime; }

	u8   GetCurrentCycle( void ){ return m_byCurrentCycle; }
	void SetCurrentCycle( u8 byCurrentCycle ){ m_byCurrentCycle = byCurrentCycle; }
	void SetNextCycle( void ){ m_byCurrentCycle ++; }

	u8   GetCycles( void ){ return m_byCycles; }
	void SetCycles( u8 byCycles ){ m_byCycles = byCycles; }

	u8   GetStatus( void ){ return m_byStatus; }
	void SetStatus( u8 byStatus ){ m_byStatus = byStatus; }

	u8   GetChnlPollNum( void ){ return m_byChnlPollNum; }
	void SetChnlPollNum( u8 byChnlPollNum ){ m_byChnlPollNum = byChnlPollNum; }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TConfPollSwitchParam
{
protected:
    s32  m_nTimeSpan;    //��ѯʱ����
    s32  m_nLevel;       //?
    s32  m_nLayerId;     //?
    u8   m_byCurPollPos; //��ѯ��ǰλ��
    u8   m_bySpecPos;    //�û�ǿ�����õ���ѯλ��
public:
    TConfPollSwitchParam()
    {
        Reset();
    }

    void Reset()
    {
        memset(this, 0, sizeof(TConfPollSwitchParam));
        ClearSpecPos();        
    }

    void SetTimeSpan(s32 nTimeSpan)
    {
        m_nTimeSpan = nTimeSpan;
    }
    s32 GetTimeSpan() const
    {
        return m_nTimeSpan;
    }
    void SetLevel(s32 nLevel)
    {
        m_nLevel = nLevel;
    }
    s32 GetLevel() const
    {
        return m_nLevel;
    }
    void SetLid(s32 nLid)
    {
        m_nLayerId = nLid;
    }
    s32 GetLid() const
    {
        return m_nLayerId;
    }

    void SetCurrPos(u8 byIdx)
    {
        m_byCurPollPos = byIdx;
    }
    u8 GetCurrPos() const
    {
        return m_byCurPollPos;
    }
        
    void SpecPollPos(u8 byIdx)
    {
        m_bySpecPos = byIdx;
    }
    BOOL32 IsSpecPos() const
    {
        return ( m_bySpecPos != POLLING_POS_INVALID );
    }
    u8   GetSpecPos() const
    {
        u8 byRet = m_bySpecPos;
        return byRet;
    }
    void ClearSpecPos()
    {
        m_bySpecPos = POLLING_POS_INVALID;
    }    

    void Print() const
    {
        StaticLog("Auto poll switch param:\n");
        StaticLog("  m_nTimeSpan: %d\n", m_nTimeSpan);
        StaticLog("  m_nLevel: %d\n", m_nLevel);
        StaticLog("  m_byCurPollPos: %d\n", m_byCurPollPos);
    }
};

//���鱣����Ϣ
struct TConfProtectInfo
{
protected:    
    u8 m_abMcsPwdPass[MAXNUM_MCU_MC]; //���ͨ��������֤���
	u8 m_byLockMcs;                   //��������غ�
	u8 m_byLockMcu;                   //�����˻����MCU    

public:
    void SetLockByMcs(u8 bySsnId)
    {
        m_byLockMcs = bySsnId;
        //m_byLockMcu = 0;
    }
    void SetLockByMcu(u8 byMcuId)   // MCU ���ȼ�����
    {
        m_byLockMcu = byMcuId;
        if (byMcuId != 0)
        {
            m_byLockMcs = 0; 
        }        
    }
    u8 GetLockedMcSsnId() const
    {
        return m_byLockMcs;
    }
    u8 GetLockedMcuId() const
    {
        return m_byLockMcu;
    }    
    BOOL32 IsLockByMcs() const
    {
        return (m_byLockMcs != 0);
    }
    BOOL32 IsLockByMcu() const
    {
        return (m_byLockMcu != 0);
    }
    void SetMcsPwdPassed(u8 bySsnId, BOOL32 bPassed = TRUE)
    {
        if (bySsnId > 0 && bySsnId <= MAXNUM_MCU_MC)
            m_abMcsPwdPass[bySsnId-1] = GETBBYTE(bPassed);
    }
    void ResetMcsPwdPassed()
    {
        memset(m_abMcsPwdPass, 0, sizeof(m_abMcsPwdPass));
    }
    BOOL32 IsMcsPwdPassed(u8 bySsnId) const
    {
        if (bySsnId > 0 && bySsnId <= MAXNUM_MCU_MC)
            return ISTRUE(m_abMcsPwdPass[bySsnId-1]);
        else
            return FALSE;
    }


    void Print() const
    {
        StaticLog("TConfProtectInfo:\n");
        StaticLog("m_byLockMcs: %d\n", m_byLockMcs);
        StaticLog("m_byLockMcu: %d\n", m_byLockMcu);
        StaticLog("m_abMcsPwdPass: ");
        for (u8 byLoop = 0; byLoop < MAXNUM_MCU_MC; byLoop ++)
            StaticLog("%d:%s ", byLoop+1, ISTRUE(m_abMcsPwdPass[byLoop]) ? "Y" : "N" );
        StaticLog("\n");
    }
};


//Ip��ַ��Ϣ
//struct TIpAddrRes
//{
//	u8  m_byUsed;
//    u8  m_byConfIdx;
//	u32 m_dwIpAddr;
//};

//�����ն����ݶ˿���Ϣ
struct TRecvMtPort
{
	u8  m_byPortInUse;
    u8  m_byConfIdx;
	u8  m_byMtId;
};

#define CONFINFOFILENAME          ( LPCSTR )"confinfo.dat"      //ԭ��ͳһ�洢������Ϣ�� �洢�ļ��� 4.0�汾����ʹ��
#define CONFINFOFILENAME_PREFIX   ( LPCSTR )"confinfo_"         //�����ģ����Ϣ_�洢�ļ���ǰ׺
#define CONFINFOFILENAME_POSTFIX  ( LPCSTR )".dat"              //�����ģ����Ϣ_�洢�ļ�����׺
#define CONFINFOFILENAME_HEADINFO ( LPCSTR )"confinfo_head.dat" //����ͷ��Ϣ��ȱʡ������Ϣ_�洢�ļ���
#define CONFHEADINFO_BACKUP       ( LPCSTR )"confinfo_head.bak" //����ͷ��Ϣ��ȱʡ������Ϣ�����ļ�,����������ͬ���޸�
#define USERINFOFILENAME          ( LPCSTR )"login.usr"
#define USERGRPINFOFILENAME       ( LPCSTR )"usrgrp.usr"
#define USERTASKINFOFILENAME      ( LPCSTR )"usrtask.dat"        //�洢��������ʻ��Ŀɵ��ȵĻ�����Ϣ

#define MCUADDRFILENAME         ( LPCSTR )"addrbook.kdv"

// ���UTF8���ɵ����ļ������������� [7/31/2013 liaokang]
#define MCU_VCSUSER_FILENAME_UTF8 ( LPCSTR )"kdvvcslogin.usr"
#define USERINFOFILENAME_UTF8     ( LPCSTR )"kdvlogin.usr"
#define MCUADDRFILENAME_UTF8      ( LPCSTR )"kdvaddrbook.kdv"


#define MCUHTMLFILENAME         ( LPCSTR )"channel.htm"

// confinfo debug, zgc, 20070524
#define CONFFILEHEAD			( LPCSTR )"KEDACOM CONFERENCE INFO FILE"
// confinfo debug end


#define UNPROCCONFINFOFILENAME_PREFIX   ( LPCSTR )"unprocconfinfo_"   //�����ģ����ص�mcu����Ҫ�������Ϣ

//����Pack�����Ļ���洢��Ϣ 
#ifdef WIN32
    #pragma pack( push )
    #pragma pack( 1 )
#endif

struct TPackConfStore
{
    TPackConfStore()
    {
        m_byMtNum = 0;
        m_wAliasBufLen = 0;
    }
    TConfInfo m_tConfInfo;
	u8        m_byMtNum;
	u16       m_wAliasBufLen;       //(������ �洢���ļ���) ����Packʱָ������� m_pbyAliasBuf �ֽڳ���
	//u8       *m_pbyAliasBuf;        //m_wAliasBufLen �ֽ�
	//TTvWallModule m_tTvWallModule;  //(��ѡ��ȡ���ڻ�������) 
	//TVmpModule    m_tVmpModule;     //(��ѡ��ȡ���ڻ�������) 
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

#ifdef WIN32
#pragma pack( pop )
#endif

// ������� [pengguofeng 5/15/2013]
#ifdef _UTF8
#define CONFINFO_EX_BUFFER_LENGTH (sizeof(u16)+8*sizeof(u8)+8*sizeof(u16)+sizeof(TVideoStreamCap)*2*MAX_CONF_CAP_EX_NUM+MAXNUM_CONF_AUDIOTYPE*sizeof(TAudioTypeDesc))+\
	sizeof(TDurationDate)+sizeof(TKdvTime)+sizeof(THduVmpChnnl)*(HDUID_MAX-HDUID_MIN+1)*1+sizeof(THduVmpSubChnnl)*(HDUID_MAX-HDUID_MIN+1)*1*HDU_MODEFOUR_MAX_SUBCHNNUM+\
	sizeof(TVmpChnnlMember)*(MAXNUM_MPU2VMP_MEMBER-MAXNUM_MPUSVMP_MEMBER) + 2*sizeof(u8) + sizeof(u16)
#else
#define CONFINFO_EX_BUFFER_LENGTH (sizeof(u16)+8*sizeof(u8)+8*sizeof(u16)+sizeof(TVideoStreamCap)*2*MAX_CONF_CAP_EX_NUM+MAXNUM_CONF_AUDIOTYPE*sizeof(TAudioTypeDesc))+\
	sizeof(TDurationDate)+sizeof(TKdvTime)+sizeof(THduVmpChnnl)*(HDUID_MAX-HDUID_MIN+1)*1+sizeof(THduVmpSubChnnl)*(HDUID_MAX-HDUID_MIN+1)*1*HDU_MODEFOUR_MAX_SUBCHNNUM+\
	sizeof(TVmpChnnlMember)*(MAXNUM_MPU2VMP_MEMBER-MAXNUM_MPUSVMP_MEMBER)
#endif
//!ע�⣺����CMcuVcData::GetVcDeamonTemplateData�������漰��С�жϣ��˴�����������

//�����µĽ�����ʽ������m_tConfInfoEx��Ϣ����������
//	ռ��u16(EX��Ϣ�ܴ�С)+u8(emMainStreamCapEx)+u16(emMainStreamCapEx�������ݴ�С)+sizoeof(TVideoStreamCap)*20
//  +u8(emDoubleStreamCapEx)+u16(emDoubleStreamCapEx�������ݴ�С)+sizoeof(TVideoStreamCap)*20
//  +u8(emMainAudioTypeDesc)+u16(emMainAudioTypeDesc�������ݴ�С)+sizeof(TAudioTypeDesc)
//  +u8(emHduVmpChnnl)+u16(emHduVmpChnnl�������ݴ�С)+sizeof(THduVmpChnnl)*140
//  +u8(emHduVmpSubChnnl)+u16(emHduVmpSubChnnl�������ݴ�С)+sizeof(THduVmpSubChnnl)*140*20 
//  +u8(emVmpChnnlMember)+u16(emVmpChnnlmember�������ݴ�С)+sizeof(TVmpChnnlMember)*5(len:18bytes)
//  +u8(emEncoding)     +u16(emEncoding���ʹ�С) ��������+sizeof(u8) (len:4bytes)[pengguofeng 4/15/2013]

//����洢��Ϣ 
struct TConfStore : public TPackConfStore 
{
    TMtAlias  m_atMtAlias[MAXNUM_CONF_MT];
	u16       m_awMtDialBitRate[MAXNUM_CONF_MT];
    TMultiTvWallModule m_tMultiTvWallModule;
	TVmpModule m_atVmpModule;
	THDTvWall   m_tHDTWInfo;     // VCS �������ǽ������Ϣ
	TVCSSMCUCfg m_tVCSSMCUCfg;   // VCS ����ʵ���������Ϣ	
	THDUModule  m_tHduModule;     // HDU ����ǽ������Ϣ
	u8          m_byMTPackExist; // �Ƿ������˷��� 
	TMtAlias    m_tVCSBackupChairMan;//VCS������ַ����
	u8			m_byVCAutoMode;		//�Զ�����ĳģʽ
	u8			m_byConInfoExBuf[CONFINFO_EX_BUFFER_LENGTH];	
public:
    
	TConfStore ()
	{
		memset(m_byConInfoExBuf,0,sizeof(m_byConInfoExBuf));
	}

    void EmptyAllTvMember(void)
    {
        for(u8 byTvLp = 0; byTvLp < MAXNUM_PERIEQP_CHNNL; byTvLp++)
        {
            m_tMultiTvWallModule.m_atTvWallModule[byTvLp].EmptyTvMember();
        }    
    }

    void EmptyAllVmpMember(void)
    {
		for( u8 byIdx = 0; byIdx < sizeof(m_atVmpModule.m_abyVmpMember);byIdx++ )
		{
			//�Ǹ�������
			if(m_atVmpModule.m_abyVmpMember[byIdx] != MAXNUM_CONF_MT+1)
			{
				m_atVmpModule.m_abyVmpMember[byIdx] = 0;
			}
		}
    }

    void SetMtInTvChannel(u8 byTvId, u8 byChannel, u8 byMtIdx, u8 byMemberType)
    {
        for(u8 byTvLp = 0; byTvLp < MAXNUM_PERIEQP_CHNNL; byTvLp++)
        {
            if(m_tMultiTvWallModule.m_atTvWallModule[byTvLp].m_tTvWall.GetEqpId() == byTvId)
            {
				//zjl[20091208]����ԭ�����õ�SetTvChannelͨ��������byTvLp��ΪbyChannel
                m_tMultiTvWallModule.m_atTvWallModule[byTvLp].SetTvChannel(byChannel, byMtIdx, byMemberType);
                break;
            }
        }
    }
    
	// �Ƿ������˸������ǽ
	BOOL IsHDTWCfg()   { return m_tHDTWInfo.IsHDTWCfg(); }
	// �Ƿ�������VCS�¼�mcu
	BOOL IsVCSMCUCfg() { return m_tVCSSMCUCfg.IsVCSMCUCfg(); }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


// MCU ��������
struct TMcuPerfLimit
{
public:
    u16     m_wMaxConnMtNum;                //mcu����������  
	u16		m_wMaxConnHDMtNum;				//mcu���������mt����	// [3/10/2010 xliang] 
	u16		m_wMaxConnPCMtNum;				//mcu������PCMT����	// [3/10/2010 xliang] 
    u8		m_byIsMpcRunMp;					//MPC�Ƿ�����MP
	u8		m_byIsMpcRunMtAdp;				//MPC�Ƿ�����MtAdp
    u8		m_byIsRestrictFlux;     		//�Ƿ���������ת�����ת������
    u16		m_wMpcMaxMpAbility;			    //MPC�ϵ�MP���ת������
    u16		m_wCriMaxMpAbility;			    //CRI���ϵ�MP���ת������
    
    u8		m_byIsRestrictMtNum;        	//�Ƿ������ն��������ն˽�������
    u8		m_byMpcMaxMtAdpConnMtAbility;	//MPC���ն���������ն˽�������
    u8		m_byMpcMaxMtAdpConnSMcuAbility;	//MPC���ն���������¼�MCU��������
    u8		m_byTuiMaxMtAdpConnMtAbility;	//TUI�����ն���������ն˽�������
	u8		m_byTuiMaxMtAdpConnSMcuAbility;	//TUI�����ն���������¼�MCU��������
    u8      m_byHdiMaxMtAdpConnMtAbility;	//HDI�����ն���������ն˽�������

    u8      m_byMaxCasConfNum;              //Mcu֧�ֵ���༶����������
    u8      m_byMaxConfSMcuNum;             //�����������SMcu����

    u32     m_dwMpcMaxPkNum;                //MPC�ϵ�MP���ת����Pk��
    u32     m_dwCriMaxPkNum;                //CRI�ϵ�MP���ת����Pk��

	u16     m_wMaxVCSAccessNum;             //MPC������vcs������
	u16     m_wMaxConnAudMtNum;             //MPC�������������ն���

public:
    TMcuPerfLimit()
    {
        m_wMaxConnMtNum = MAXNUM_MCU_MT;

		//[liu lijiu][2010.07.20]��mcuͬʱ����vcs�ĸ�����Ĭ��ֵ����Ϊ0
		m_wMaxVCSAccessNum = 0;

        m_byIsMpcRunMp = 0;				    // Ĭ�ϲ���������ת��,�����ر�����
		m_byIsMpcRunMtAdp = 0;				// Ĭ�ϲ����������ն˽���,�����ر�����
        
        m_byIsRestrictFlux = 1;        
        m_wMpcMaxMpAbility = MAXLIMIT_MPC_MP;			
        m_wCriMaxMpAbility = MAXLIMIT_CRI_MP;			
        
        m_byIsRestrictMtNum = 1;	
        m_byMpcMaxMtAdpConnMtAbility = MAXLIMIT_MPC_MTADP;
        m_byMpcMaxMtAdpConnSMcuAbility = MAXLIMIT_MPC_MTADP_SMCU;		
        m_byTuiMaxMtAdpConnMtAbility = MAXLIMIT_CRI_MTADP;
		m_byTuiMaxMtAdpConnSMcuAbility = MAXLIMIT_CRI_MTADP_SMCU;
        m_byHdiMaxMtAdpConnMtAbility = MAXLIMIT_HDI_MTADP;

        m_byMaxCasConfNum  = MAXLIMIT_MCU_CASCONF;
        m_byMaxConfSMcuNum = MAXLIMIT_CONF_SMCU;

        m_dwMpcMaxPkNum = MAXLIMIT_MPC_MP_PMNUM * 1024;
        m_dwCriMaxPkNum = MAXLIMIT_CRI_MP_PMNUM * 1024;
    }

    BOOL32 IsMpcRunMp() const
    {
        return (m_byIsMpcRunMp == 1 ? TRUE : FALSE );
    }
    BOOL32 IsMpcRunMtAdp() const
    {
        return ( m_byIsMpcRunMtAdp == 1 ? TRUE : FALSE );
	}
    BOOL32 IsLimitMp() const
    {
        return (m_byIsRestrictFlux == 1 ? TRUE : FALSE );
    }
    BOOL32 IsLimitMtNum() const
    {
        return (m_byIsRestrictMtNum == 1 ? TRUE : FALSE );
    }
    
    void Print() const
    {
        StaticLog("Performance Limit: \n\n");

        StaticLog("\t Licensed Max Mt Num: %d\n", m_wMaxConnMtNum);
		StaticLog("\t Licensed Max HDMt Num: %d\n", m_wMaxConnHDMtNum);
	    StaticLog("\t Licensed Max PCMt Num: %d\n", m_wMaxConnPCMtNum);
		StaticLog("\t m_wMaxVCSAccessNum: %d\n", m_wMaxVCSAccessNum);
        
		StaticLog("\t m_byIsMpcRunMtAdp: %d\n", m_byIsMpcRunMtAdp);
        StaticLog("\t m_byIsMpcRunMp: %d\n", m_byIsMpcRunMp);

        StaticLog("\t m_byIsRestrictFlux: %d\n", m_byIsRestrictFlux);
        StaticLog("\t m_wMpcMaxMpAbility: %d Mb\n", m_wMpcMaxMpAbility);
        StaticLog("\t m_wCriMaxMpAbility: %d Mb\n", m_wCriMaxMpAbility);
        
        StaticLog("\t m_byIsRestrictMtNum: %d\n", m_byIsRestrictMtNum);
        StaticLog("\t m_byMpcMaxMtAdpConnMtAbility: %d\n", m_byMpcMaxMtAdpConnMtAbility);
        StaticLog("\t m_byMpcMaxMtAdpConnSMcuAbility: %d\n", m_byMpcMaxMtAdpConnSMcuAbility);		
        StaticLog("\t m_byTuiMaxMtAdpConnMtAbility: %d\n", m_byTuiMaxMtAdpConnMtAbility);
		StaticLog("\t m_byTuiMaxMtAdpConnSMcuAbility: %d\n", m_byTuiMaxMtAdpConnSMcuAbility);
        StaticLog("\t m_byHdiMaxMtAdpConnMtAbility: %d\n", m_byHdiMaxMtAdpConnMtAbility);

        StaticLog("\t m_byMaxCasConfNum: %d\n", m_byMaxCasConfNum);
        StaticLog("\t m_byMaxConfSMcuNum: %d\n", m_byMaxConfSMcuNum);

    }
};

typedef struct tCheatMtParam
{
public:
	u8     m_byMtIdType;  //��ʾMt�ķ�����ֵΪ1��ʾ��IP��ʾ��2��ʾ�ó����ͺ�(ProductID)��ʾ,3��ʾ��manuId��ʾ
	s8	   m_aszMtAlias[MAXLEN_PRODUCTID]; //������m_byMtIdType���ã�m_byMtIdType��1���洢IP��m_byMtIdType��2�����ProductId ...��
	u8     m_byMainCapId;    //������������
	u8     m_byDualCapId;    //˫����������
	u8     m_byAudioCapId;   //��Ƶ��������
public:
	
	tCheatMtParam( void )
	{
		Clear();
	}
	
	void Clear(void)
	{
		m_byMtIdType = 0;
		memset(m_aszMtAlias,0,sizeof(m_aszMtAlias));
		m_byMainCapId = 0xFF;  //��ֹ��ѡ0��ͻ
		m_byDualCapId = 0xFF;
		m_byAudioCapId = 0xFF;
	}
}TCheatMtParam;

typedef struct tCheatCapSet
{
public:
	u8    m_byMediaType;    //��ʽ
	u8    m_byRes;        //�ֱ���
	u8    m_byFps;        //֡��
	u16   m_wMaxBR;        //����������
}TCheatCapSet;

//ȡ����������Ҫ���Լ������������⳧���ն�
typedef struct tagSendSelfMutePackMt
{
	u8 m_byManuId;						//��Ҫ���Լ������������⳧��id
	s8 m_achProductId[MAXLEN_PRODUCTID];//productid
	tagSendSelfMutePackMt(){Clear();}
	~tagSendSelfMutePackMt(){Clear();}
	void Clear()
	{
		memset(this,0,sizeof(tagSendSelfMutePackMt));
	}
}TSendSelfMutePackMt;

typedef struct TMcuDebugFile
{
	u16     m_wMcuListenPort;
	u16     m_wMcuTelnetPort;
	
	u32     m_dwMcsRefreshInterval;
	u32		m_dwRefressMtInfoInterval;		// �ն���Ϣ�����ϱ���mcsʱ��(����)
	u32		m_dwRefreshSMcuMtInterval;		// �¼��б����ϱ�ʱ����(����)
	u32		m_dwRefreshMtAdpSMcuList;		// Mcu->MtAdp->Mcu��MtAdp�л����¼��ն��б�ʱ����(����)
	
	u8		m_byEnableMcuMcsMtInfoBuf;		// �Ƿ�֧��Mcu->Mcs֮���MtInfo\list�Ļ���
	u8		m_byEnableMcuMcsMtStatusBuf;	// �Ƿ�֧��Mcu->Mcs֮���Mt Status�Ļ���
	u8		m_byEnableMcuMcuMtStatusBuf;	// �Ƿ�֧��Mcu->Mcu֮���MtInfo\list�Ļ���
	u8		m_byEnableMtAdpSMcuListBuf;		// �Ƿ�֧����Mcu->MtAdp->Mcu��MtAdp�л����¼��ն��б�

	BOOL32  m_bWDEnable;                    //�Ƿ񼤻�Ź�	

	BOOL32  m_bMSDetermineType;             //vxworks��Ӳ��ƽ̨֧�������ù��ܣ���ֱ����os��ȡ�����ý����Ĭ�ϲ�֧��
    u16		m_wMsSynTime;					//����ͬ��ʱ��

    u8      m_byIsMMcuSpeaker;              //Ĭ��ָ���ϼ���������            
    u8      m_byIsAutoDetectMMcuDupCall;    //���ϼ�MCU���º���ʱ������Ƿ���ͬһ���ϼ�MCU
    u8      m_byCascadeAliasType;           //����ʱMCU�ڻ����б�������ʾ��ʽ
                                            //(0 - MCU Alias+Conf H.323ID,  1 - Mcu Alias Only, 2 - Conf H.323ID Only)
	//[nizhijun 2011/02/14]ȡ�����Ƿ���ʾ�ϼ�MCU�����ն��б���mcucfg.ini��Ϊ׼
	//u8      m_byShowMMcuMtList;             //����ʱ�Ƿ���ʾ�ϼ�MCU���ն��б�
	//[03/04/2010] zjl modify (�侯�������ϲ�)
	u8      m_byIsStartSwitchToMMcuWithoutCasBas;//�¼��Ƿ�������ֱ�ӽ����������ϼ�
    u16     m_wSMcuCasPort;                 //�ϼ����ú����¼��ļ����˿�

    u8      m_byBitrateScale;               //����ٷֱ�

	u8		m_byIsApplyChairToZxMcu;	    //�Ƿ�������mcu������ϯ
    u8      m_byIsTransmitMtShortMsg;       //�Ƿ�ת���ն˶���Ϣ
    u8      m_byIsChairDisplayMtApplyInfo;  //�Ƿ��ն�������ϯ�����˵���ʾ��Ϣ������ϯ�ն�
    u8      m_byIsSelInDoubleMediaConf;     //�Ƿ�����˫��ʽ����ѡ��
    u8      m_byIsChairAutoSpecMix;         //�Ƿ�֧��������ϯ�Զ��������ƻ���

    TKdvTime m_tExpireDate;                 //mcu ʹ�ý�ֹ����

    u8      m_byIsNotLimitAccessByMtModal;  //�Ƿ�����ն��ͺţ���Ʒ���ͣ����ƽ�������	

	// MCU��������
    TMcuPerfLimit m_tPerfLimit;

    u8      m_byConfFPS;					//����֡������
    u8      m_byIsSupportSecDSCap;          //�����Ƿ�֧�ֵڶ�˫������
	u8      m_byIsSendFakeCap2Taide;        //��Ӧ����VMP�Ľ�����ޣ�Taide���ն˷�cif������
    u8      m_byIsSendFakeCap2TaideHD;      //��Ӧ����Tandberg�������������ޣ�Taide���ն˷�720p������
    u8      m_byIsSendFakeCap2Polycom;      //��Ӧ����VMP�Ľ�����ޣ�polycom���ն˷�480p������
	u8      m_byIsDistingtishSDHDMt;		//��Ӧ����VMP�Ľ�����ޣ������ֻ�����SD��HD���ն�
	u8		m_byIsVidAdjustless4Polycom;	//�Ƿ���ѡ�������ʱ������polycom�ĵ���
    u8      m_byIsAllowVmpMemRepeated;      //VMPͬһ��Ա�Ƿ���Խ���ͨ��
    u8      m_byIsSelAccord2Adp;            //ֱ��ѡ��ʧ�ܣ��Ƿ���ռ������
    u8      m_byIsAdpResourceCompact;       //�Ƿ�������Դ���մ���
    u8      m_byIsSVmpOutput1080i;          //mpu��svmpʱ��һ·�����Ƿ�ǿ��Ϊ1080i
    
    u8      m_byBandWidthReserved4HdBas;     //�ڰ�mp��ΪHDBas������Ԥ��
    u8      m_byBandWidthReserved4HdVmp;     //�ڰ�mp��ΪHDVmp������Ԥ��

    //u8      m_bIsSendFakeCap2HuaweiSDMcu;    //�Ƿ�Լ����Ϊ�ı���MCU��2CIF����Ƶ������CIF
    u8      m_byMcuAdminLevel;               //��������	
    u8      m_byMaxOngoingConf;              //mcu���ʱ��������

	u8		m_byTelnetEn;					//�Ƿ�����telnet(0:��1:��)
	u8		m_byIsCanConfCapabilityCheat;	//�Ƿ���Է����ն˵Ļ���������ƭ(0:��1:��)
	u8		m_byConfCapCheatMtBoardTypeNum; //����������ƭ���ն�������
	u8		m_pachCheatMtBoardType[MAXNUM_CONFCAPCHEATMTBOARDNUM][MAXLEN_PRODUCTID];//��ſ�����ƭ�����������ն��ͺŵ�����(��ʱ��λ20���ͺ�)
	u8		m_byConfHDMtBoardTypeNum;
	u8		m_pachHDMtBoardType[MAXNUM_CONFHDMTBOARDNUM][MAXLEN_PRODUCTID];//����ڸ�������²�������ƭ�����������ն��ͺŵ�����(һ��ֻ��Ÿ����ն������ͺ�,��ʱ��λ10���ͺ�)
	u8		m_byLowBrCalledMtChlOpenMode;	//���ٺ����ն�ͨ������ģʽ 0.�ѻ������������������ն� 1.���������mpeg4��������ֻ������������ն� 2.���������h264��������ֻ������������ն�

    u8      m_byIsSupportChgLGCResAcdBR;    //�Ƿ�֧�ָ����ն˺������ʵ������߼�ͨ���ֱ���
	u16     m_w1080p6050FPSBpLeastBR;		// [3/31/2011 chendaiwei]1080p/60/50֡BP�ٽ����
	u16     m_w1080p6050FPSHpLeastBR;		// [3/31/2011 chendaiwei]1080p/60/50֡HP�ٽ����
	u16		m_w1080p3025FPSBpLeastBR;		// [3/31/2011 chendaiwei]1080p/30/25֡BP�ٽ����
	u16		m_w1080p3025FPSHpLeastBR;		// [3/31/2011 chendaiwei]1080p/30/25֡BP�ٽ����
	u16     m_w720P6050FpsBpLeastBR;        // 720p 60/50 BP�����ٽ����
	u16     m_w720P3025FpsBpLeastBR;        // 720p 30/25 BP�����ٽ����
	u16     m_w720P6050FpsHpLeastBR;        // 720p 60/50֡ HP�����ٽ����
	u16     m_w720P3025FpsHpLeastBR;        // 720p 30/25֡ HP�����ٽ����
    u16     m_w4CifLeastBR;                 // 4Cif�����ٽ����
    u16     m_wCifLeastBR;                  // Cif�����ٽ����
	
	u8		m_byVcsMtOverTimeInterval;         //vcs����ĺ��г�ʱʱ�䣬��λ��
	u8      m_byIsVASimultaneous;			   //�Ƿ�֧�ִ���ͬ��

	u16     m_wMmcuOlcTimerOut;				//olc mmcu ��ʱ���
    u8      m_byWaitMixerRspTimeout;

	u8      m_byDelayReleaseConfTime;       // vcs�쳣�������ӳٽ���ʱ�䣨���ӣ�
    u8      m_byFpsAdp;                     //����֡��

	u8		m_byMtFastUpdateInterval;

	u8		m_byMtFastUpdateNeglectNum;
	u8		m_byFastUpdateToSpeakerNum;     //MCU����������ؼ�֡���� [4/6/2012 chendaiwei]


	u8		 m_byNoneKedaMtAbilityCheatNum;

	u32		m_arrdwNoneKedaMtIp[60];

	u8		m_arrbyNoneKedaMtAbility[60];

	u8		m_byIsPolyComMtSendDoubleStream;

	u8		m_byIsChaoRanMtAllowInVmpChnnl;

	u8					m_byCheatMtNum;                 // �����ն˸���
	u8				    m_byCheatCapNum;                // �����������ܸ�������Ƶ����Ƶ���ܺ�
	u8                  m_byCheatVideoCapNum;           //���׵���Ƶ����������
	TCheatMtParam       m_tCheatMtParam[MAXNUM_CHEATMTNUM];
	TDStreamCap		m_tCheatVideoCapSet[MAXNUM_CHEATMTVIDEOCAP+1];//���׵���Ƶ������������������˫��,��˫���Ľṹ��ʾ��˫���������ṹ��󣩣�
	TAudioTypeDesc		m_tCheatAudioCapSet[MAXNUM_CHEATMTAUDIOCAP+1]; //���׵���Ƶ������

	u8      m_byNeedMapIpNum;              //��Ҫ���ٵ�Ŀ��ip����
	u32     m_adwNeedMappedIp[100];          //��Ҫ���ٵ�Ŀ��ip(������)
	u8		m_byIsSendStaticText;

    u32     m_adwMultiCastMtTable[MAXNUM_CONF_MT]; //pgf���ǻ��鶨�壺�����鲥�����ն�ip�б�,������
    u8      m_byMultiCastMtTableSize;
   
	u32     m_dwPinHoleInterval;                   //��ʱ����

	u8      m_byMutePackManuEntryNum;				   //��Ҫ�����������Ĳ�Ʒ��
	TSendSelfMutePackMt m_atSndSelfMutePackMt[20];     //��Ҫ���Լ������������ն�manuid��productid
	u8      m_byMutePackSendInterval;				   //������������ʱ����(msΪ��λ)
	u8      m_byMutePackNum;						   //������������Ŀ

	u8		m_byReverseG7221cNum;						//��Ҫ��g7221.c��Ƶ��ʽ����������ż�Ի��Ĳ�Ʒ��
	TSendSelfMutePackMt m_atReverseG7221cMt[5];	//��Ҫ��g7221.c��Ƶ��ʽ����������ż�Ի����ն�manuid��productid,����TSendSelfMutePackMt�ṹ
	
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	u8      m_byRcvH264DependInMarkNum;					//��Ҫ�����ⷽʽ����Ĳ�Ʒ��,����8KG/8KH
	TSendSelfMutePackMt m_atRcvH264DependInMarkMt[5];	//��Ҫ�����ⷽʽ������ն�manuid��productid
#endif

    TMcuDebugFile()
    {
        m_wMcuListenPort = MCU_LISTEN_PORT;
        m_wMcuTelnetPort = MCU_TELNET_PORT;
        m_dwMcsRefreshInterval = 1000;
		m_dwRefreshSMcuMtInterval = 1000;
		m_dwRefressMtInfoInterval = 1000;
		m_dwRefreshMtAdpSMcuList = 500;
		
		m_byEnableMcuMcsMtInfoBuf = 1;		// �Ƿ�֧��Mcu->Mcs֮���MtInfo\list�Ļ���
		m_byEnableMcuMcsMtStatusBuf = 1;	// �Ƿ�֧��Mcu->Mcs֮���Mt Status�Ļ���
		m_byEnableMcuMcuMtStatusBuf = 1;	// �Ƿ�֧��Mcu->Mcu֮���MtInfo\list�Ļ���
		m_byEnableMtAdpSMcuListBuf = 1;		// �Ƿ�֧����Mcu->MtAdp->Mcu��MtAdp�л����¼��ն��б�
		
        m_bWDEnable = TRUE;                       
	    m_bMSDetermineType = TRUE;
        m_wMsSynTime = 300; //Ĭ��ͬ��ʱ��
        
        m_byIsMMcuSpeaker = 0;
        m_byCascadeAliasType = 0;
		m_byIsStartSwitchToMMcuWithoutCasBas = 0; //[03/04/2010] zjl modify (�侯�������ϲ�)
        m_wSMcuCasPort = 3337;
        m_byIsAutoDetectMMcuDupCall = 0;

        m_byBitrateScale = 0;
        m_byIsApplyChairToZxMcu = 0;
        m_byIsTransmitMtShortMsg = 1;
        m_byIsChairDisplayMtApplyInfo = 1;
        m_byIsSelInDoubleMediaConf = 0;
        m_byIsNotLimitAccessByMtModal = 0; // Ĭ������
		m_byConfFPS	= 0;
        m_byIsSupportSecDSCap = 0;
		m_byIsSendFakeCap2TaideHD = 0;
		m_byIsSendFakeCap2Taide = 0;
        m_byIsSendFakeCap2Polycom = 0;
		m_byIsDistingtishSDHDMt = 0;
		m_byIsVidAdjustless4Polycom = 0;
        m_byIsSelAccord2Adp = 0;
        m_byIsAdpResourceCompact = 1;
        m_byIsAllowVmpMemRepeated = 0;
        m_byIsSVmpOutput1080i = 0;
        m_byBandWidthReserved4HdBas = 0;
        m_byBandWidthReserved4HdVmp = 0;

        //m_bIsSendFakeCap2HuaweiSDMcu = 0;
		m_byMcuAdminLevel = 4;
		m_byMaxOngoingConf = MAXNUM_ONGO_CONF;

		m_byIsCanConfCapabilityCheat = 0;
		m_byLowBrCalledMtChlOpenMode = 0;

		m_byIsSupportChgLGCResAcdBR = 0;

		m_w1080p6050FPSBpLeastBR = 3072;
		m_w1080p6050FPSHpLeastBR = 1536;   // [3/31/2011 chendaiwei]1080p/60/50֡HP�ٽ����
		m_w1080p3025FPSBpLeastBR = 2560;   // [3/31/2011 chendaiwei]1080p/30/25֡BP�ٽ����
		m_w1080p3025FPSHpLeastBR = 1024;   // [3/31/2011 chendaiwei]1080p/30/25֡BP�ٽ����
		m_w720P6050FpsBpLeastBR = 2560;		// 720p 60/50 BP�����ٽ����
		m_w720P3025FpsBpLeastBR = 1536;		// 720p 30/25 BP�����ٽ����
		m_w720P6050FpsHpLeastBR = 832;     // 720p 60/50֡ HP�����ٽ����
		m_w720P3025FpsHpLeastBR = 512;     // 720p 30/25֡ HP�����ٽ����
		m_w4CifLeastBR = 1024;              // 4Cif�����ٽ����

		m_byVcsMtOverTimeInterval = DEFAULT_VCS_MTOVERTIME_INTERVAL;//Ĭ��vcs������г�ʱʱ��10s

		m_byTelnetEn  = 1;
		m_byIsVASimultaneous = 0;
		m_byIsChairAutoSpecMix = 0;

		m_wMmcuOlcTimerOut = 20000;
		m_byWaitMixerRspTimeout = 6;

		m_byDelayReleaseConfTime = 30;
        m_byFpsAdp = 0;

		m_byMtFastUpdateInterval = DEFAULT_MT_MCU_FASTUPDATE_INTERVAL;

		m_byMtFastUpdateNeglectNum = 0;
		m_byFastUpdateToSpeakerNum = 0;

		m_byNoneKedaMtAbilityCheatNum = 0;

		memset( m_arrdwNoneKedaMtIp,0,sizeof(m_arrdwNoneKedaMtIp) );

		memset( m_arrbyNoneKedaMtAbility,0,sizeof(m_arrbyNoneKedaMtAbility) );

		m_byIsPolyComMtSendDoubleStream = 0;

		m_byIsChaoRanMtAllowInVmpChnnl = 0;

        m_byCheatMtNum = 0;
		m_byCheatCapNum = 0;
		m_byCheatVideoCapNum = 0;
		m_byNeedMapIpNum = 0;
		memset(m_adwNeedMappedIp,0,sizeof(m_adwNeedMappedIp));
		for ( u8 byIdx = 0; byIdx < 100; byIdx++)
		{
			m_tCheatMtParam[byIdx].Clear();
//			m_tCheatCapSet[byIdx].Clear();
		}


		m_byIsSendStaticText = 1;
// ���Ǳ�����ʼ�� [pengguofeng 3/5/2013]
        memset( m_adwMultiCastMtTable, 0, sizeof(m_adwMultiCastMtTable) );
        m_byMultiCastMtTableSize = 0;
        
        //��ʱ��Ĭ��5��
		m_dwPinHoleInterval = 5;     

		m_byMutePackManuEntryNum = 0;
		memset(m_atSndSelfMutePackMt,0,sizeof(m_atSndSelfMutePackMt));
		m_byMutePackSendInterval = 1;
		m_byMutePackNum = 5;
		//g7221.c��Ƶ��ʽ����������ż�Ի�����
		m_byReverseG7221cNum = 0;
		memset(m_atReverseG7221cMt,0,sizeof(m_atReverseG7221cMt));

#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
		m_byRcvH264DependInMarkNum = 0;
		memset(m_atRcvH264DependInMarkMt,0,sizeof(m_atRcvH264DependInMarkMt));
#endif

    }

		/*=============================================================================
	  �� �� ���� IsAudioType
	  ��    �ܣ� �ж�ý�������Ƿ�Ϊ��Ƶ
	  �㷨ʵ�֣� 
	  ȫ�ֱ����� 
	  ��    ���� u8 byMediaType:ý������

	  �� �� ֵ��
	  ----------------------------------------------------------------------
	  �޸ļ�¼    ��
	  ��  ��		�汾		�޸���		�߶���    �޸�����
	  2013/04/02    4.7			zhouyiliang                  ����
	=============================================================================*/
	BOOL32 IsAudioType(u8 byMediaType )
	{
		if( (byMediaType == MEDIA_TYPE_PCMU) ||	(byMediaType == MEDIA_TYPE_PCMA) ||
				(byMediaType == MEDIA_TYPE_G728) || (byMediaType == MEDIA_TYPE_G722) ||
				(byMediaType == MEDIA_TYPE_G729) || (byMediaType == MEDIA_TYPE_G7231) ||
				(byMediaType == MEDIA_TYPE_ADPCM) || (byMediaType == MEDIA_TYPE_G7221C) ||
				(byMediaType == MEDIA_TYPE_MP3) || (byMediaType == MEDIA_TYPE_AACLC) ||
				(byMediaType == MEDIA_TYPE_AACLD) || (byMediaType == MEDIA_TYPE_G719) || 
				(byMediaType == MEDIA_TYPE_G721) || (byMediaType == MEDIA_TYPE_G7221)
		  )
		{
				return TRUE;
		}
		return FALSE;
	}

    void Print() const
	{
        StaticLog("McuDebugFile Information:\n");
        StaticLog("m_wMcuListenPort: %d\n", m_wMcuListenPort);
        StaticLog("m_wMcuTelnetPort: %d\n", m_wMcuTelnetPort);
        StaticLog("m_dwMcsRefreshInterval: %d\n", m_dwMcsRefreshInterval);
		StaticLog("m_dwRefressMtInfoInterval: %d\n", m_dwRefressMtInfoInterval);
		StaticLog("m_dwRefreshSMcuMtInterval: %d\n", m_dwRefreshSMcuMtInterval);
		StaticLog("m_dwRefreshMtAdpSMcuList: %d\n", m_dwRefreshMtAdpSMcuList);
		
		
		StaticLog("m_byEnableMcuMcsMtInfoBuf: %d\n", m_byEnableMcuMcsMtInfoBuf);
		StaticLog("m_byEnableMcuMcsMtStatusBuf: %d\n", m_byEnableMcuMcsMtStatusBuf);
		StaticLog("m_byEnableMcuMcuMtStatusBuf: %d\n", m_byEnableMcuMcuMtStatusBuf);
		StaticLog("m_byEnableMtAdpSMcuListBuf: %d\n", m_byEnableMtAdpSMcuListBuf);
		
        StaticLog("m_bWDEnable: %d\n", m_bWDEnable);
        StaticLog("m_bMSDetermineType: %d\n", m_bMSDetermineType);
		StaticLog("m_wMsSynTime: %d\n", m_wMsSynTime );        
        StaticLog("m_byIsMMcuSpeaker: %d\n", m_byIsMMcuSpeaker);        
        StaticLog("m_byIsAutoDetectMMcuDupCall: %d\n", m_byIsAutoDetectMMcuDupCall);
        StaticLog("m_byCascadeAliasType: %d\n", m_byCascadeAliasType);
       // StaticLog("m_byShowMMcuMtList: %d\n", m_byShowMMcuMtList);    
		StaticLog("m_byIsStartSwitchToMMcuWithoutCaseBas:%d\n", m_byIsStartSwitchToMMcuWithoutCasBas);//[03/04/2010] zjl modify (�侯�������ϲ�)
        StaticLog("m_wSMcuCasPort: %d\n", m_wSMcuCasPort);
        
        StaticLog("m_byBitrateScale: %d\n", m_byBitrateScale);
        StaticLog("m_byIsApplyChairToZxMcu: %d\n", m_byIsApplyChairToZxMcu);
        StaticLog("m_byIsTransmitMtShortMsg: %d\n", m_byIsTransmitMtShortMsg);
        StaticLog("m_byIsChairDisplayMtApplyInfo: %d\n", m_byIsChairDisplayMtApplyInfo);
        StaticLog("m_byIsSelInDoubleMediaConf: %d\n", m_byIsSelInDoubleMediaConf);
        StaticLog("m_byIsNotLimitAccessByMtModal: %d\n", m_byIsNotLimitAccessByMtModal);
        StaticLog("m_byIsAllowVmpMemRepeated: %d\n", m_byIsAllowVmpMemRepeated);
		StaticLog("m_byIsSendFakeCap2Taide: %d\n", m_byIsSendFakeCap2Taide);
        StaticLog("m_byIsSendFakeCap2TaideHD: %d\n", m_byIsSendFakeCap2TaideHD);
        StaticLog("m_byIsSendFakeCap2Polycom: %d\n", m_byIsSendFakeCap2Polycom);
		StaticLog("m_byIsDistingtishSDHDMt: %d\n", m_byIsDistingtishSDHDMt);
		StaticLog("m_byIsVidAdjustless4Polycom: %d\n", m_byIsVidAdjustless4Polycom);
        StaticLog("m_byIsAdpResourceCompact.%d\n", m_byIsAdpResourceCompact);
        StaticLog("m_byIsSelAccord2Adp: %d\n", m_byIsSelAccord2Adp);
        StaticLog("m_byIsSVmpOutput1080i:%d\n", m_byIsSVmpOutput1080i);
		StaticLog("m_byConfFPS:%d\n", m_byConfFPS);
        StaticLog("m_byIsSupportSecDSCap.%d\n", m_byIsSupportSecDSCap);
        StaticLog("m_byBandWidthReserved4HdBas.%d\n", m_byBandWidthReserved4HdBas);
        StaticLog("m_byBandWidthReserved4HdVmp.%d\n", m_byBandWidthReserved4HdVmp);
        //StaticLog("m_bIsSendFakeCap2HuaweiSDMcu.%d\n", m_bIsSendFakeCap2HuaweiSDMcu);
        StaticLog("m_byMcuAdminLevel: %d\n", m_byMcuAdminLevel);
        StaticLog("m_byMaxOngoingConf: %d\n", m_byMaxOngoingConf);
		StaticLog("m_byLowBrCalledMtChlOpenMode: %d\n", m_byLowBrCalledMtChlOpenMode);		
		StaticLog("m_byIsCanConfCapabilityCheat: %d\n", m_byIsCanConfCapabilityCheat);
		StaticLog("m_byConfCapCheatMtBoardTypeNum: %d\n", m_byConfCapCheatMtBoardTypeNum);

		StaticLog("m_byIsSupportChgLGCResAcdBR: %d\n", m_byIsSupportChgLGCResAcdBR);

		StaticLog("m_w1080p6050FPSBpLeastBR: %d\n", m_w1080p6050FPSBpLeastBR);
		StaticLog("m_w1080p3025FPSBpLeastBR: %d\n", m_w1080p3025FPSBpLeastBR);
		StaticLog("m_w720P6050FpsBpLeastBR: %d\n", m_w720P6050FpsBpLeastBR);
		StaticLog("m_w720P3025FpsBpLeastBR: %d\n", m_w720P3025FpsBpLeastBR);
		StaticLog("m_w4CifLeastBR: %d\n", m_w4CifLeastBR);

		StaticLog("m_w1080p6050FPSHpLeastBR: %d\n", m_w1080p6050FPSHpLeastBR);
		StaticLog("m_w1080p3025FPSHpLeastBR: %d\n", m_w1080p3025FPSHpLeastBR);
		StaticLog("m_w720P6050FpsHpLeastBR: %d\n", m_w720P6050FpsHpLeastBR);
		StaticLog("m_w720P3025FpsHpLeastBR: %d\n", m_w720P3025FpsHpLeastBR);

		StaticLog("m_byVcsMtOverTimeInterval:  %d\n", m_byVcsMtOverTimeInterval);

		u8 byIndex = 0;
		StaticLog("m_abyCheatMtBoardType: \n");
		for( byIndex = 0;byIndex < MAXNUM_CONFCAPCHEATMTBOARDNUM;byIndex++ )
		{
			StaticLog( "BoardType%d:%s\n",byIndex+1,m_pachCheatMtBoardType[byIndex] );
		}

		StaticLog("m_byConfHDMtBoardTypeNum: %d\n", m_byConfHDMtBoardTypeNum);
		for( byIndex = 0;byIndex < MAXNUM_CONFHDMTBOARDNUM;byIndex++ )
		{
			StaticLog( "HDBoardType%d:%s\n",byIndex+1,m_pachHDMtBoardType[byIndex] );
		}

        StaticLog("License Expired Date: %.4d-%.2d-%.2d\n\n", 
                  m_tExpireDate.GetYear(), m_tExpireDate.GetMonth(), m_tExpireDate.GetDay());

		m_tPerfLimit.Print();

		StaticLog("m_byTelnetEn: %d\n", m_byTelnetEn);
		StaticLog("m_byIsVASimultaneous :%d\n", m_byIsVASimultaneous);
		StaticLog("m_byIsChairAutoSpecMix:%d\n", m_byIsChairAutoSpecMix);

		StaticLog("m_wMmcuOlcTimerOut: %d\n", m_wMmcuOlcTimerOut);
		StaticLog("m_byWaitMixerRspTimeout: %d\n",m_byWaitMixerRspTimeout);
        StaticLog("m_byFpsAdp: %d\n", m_byFpsAdp);

		StaticLog("m_byMtFastUpdateInterval: %d\n",m_byMtFastUpdateInterval );
		StaticLog("m_byMtFastUpdateNeglectNum: %d\n",m_byMtFastUpdateNeglectNum );
		StaticLog("OspClkRateGet: %d\n",OspClkRateGet() );		

		StaticLog("m_byFastUpdateToSpeakerNum: %d\n",m_byFastUpdateToSpeakerNum );

		StaticLog("m_byIsPolyComMtSendDoubleStream: %d\n",m_byIsPolyComMtSendDoubleStream );
		
		StaticLog("m_byNoneKedaMtAbilityCheatNum: %d\n", m_byNoneKedaMtAbilityCheatNum);

		StaticLog("m_byIsChaoRanMtAllowInVmpChnnl: %d\n",m_byIsChaoRanMtAllowInVmpChnnl );

		StaticLog("m_byIsSendStaticText: %d\n",m_byIsSendStaticText );

		for( byIndex = 0;byIndex<m_byNoneKedaMtAbilityCheatNum;++byIndex )
		{
			StaticLog("Ip:%d.%d.%d.%d(0x%08X) VideoFormat:%d\n", 
				m_arrdwNoneKedaMtIp[byIndex]>>24,(m_arrdwNoneKedaMtIp[byIndex]>>16) & 0xff,
				 (m_arrdwNoneKedaMtIp[byIndex]>>8) & 0xff,m_arrdwNoneKedaMtIp[byIndex] & 0xff,										
					m_arrdwNoneKedaMtIp[byIndex],
					m_arrbyNoneKedaMtAbility[byIndex]
					);
		}

        StaticLog("\tCheatMtNum:\t[%d]\n", m_byCheatMtNum);
		for ( byIndex = 0; byIndex < m_byCheatMtNum; byIndex++)
		{
			u8 byMtIdType =  m_tCheatMtParam[byIndex].m_byMtIdType;
			s8 *pszMtAlias = (s8 *)&m_tCheatMtParam[byIndex].m_aszMtAlias[0];
			if ( 0 == byMtIdType )
			{
				continue; //��ֵ����ӡ
			}
			StaticLog("[ID:%3d]MtIdType:%d\tMtAlias:%s\tMainCapId:%d\tDualCapId:%d\tAudioCapId:%d\n", byIndex,
				byMtIdType,pszMtAlias,(s8)m_tCheatMtParam[byIndex].m_byMainCapId, (s8)m_tCheatMtParam[byIndex].m_byDualCapId,
				(s8)m_tCheatMtParam[byIndex].m_byAudioCapId);
		}
		
		StaticLog("\tCheatCapSetNum:\t[%d]\n", m_byCheatCapNum);
		for (byIndex = 0; byIndex < m_byCheatVideoCapNum; byIndex++)
		{	
			//�ȴ�ӡ��Ƶ�ģ��ٴ�ӡ��Ƶ��
			if ( byIndex < MAXNUM_CHEATMTVIDEOCAP 
				 && MEDIA_TYPE_NULL != m_tCheatVideoCapSet[byIndex].GetMediaType()
				&& 0 != m_tCheatVideoCapSet[byIndex].GetResolution()
				//|| 0 == m_tCheatCapSet[byIndex].GetMaxBitRate()
				&& 0 != m_tCheatVideoCapSet[byIndex].GetUserDefFrameRate())
			{
				StaticLog("VideoCap:[ID:%3d]MediaType:%d\tFPS:%d\tRES:%d\tMaxBR:%d\tHP:%d\tIsSupportH239:%d\n", byIndex, m_tCheatVideoCapSet[byIndex].GetMediaType(),
					m_tCheatVideoCapSet[byIndex].GetUserDefFrameRate(), m_tCheatVideoCapSet[byIndex].GetResolution(),
				m_tCheatVideoCapSet[byIndex].GetMaxBitRate(),m_tCheatVideoCapSet[byIndex].GetH264ProfileAttrb(),m_tCheatVideoCapSet[byIndex].IsSupportH239());
			}
		}
		for (byIndex = 0; byIndex < m_byCheatCapNum - m_byCheatVideoCapNum; byIndex++)
		{
			//��Ƶ
			if ( byIndex < MAXNUM_CHEATMTAUDIOCAP 
				&& MEDIA_TYPE_NULL != m_tCheatAudioCapSet[byIndex].GetAudioMediaType()
				&& 0 != m_tCheatAudioCapSet[byIndex].GetAudioTrackNum()
				)
			{
				StaticLog("Audio:[ID:%3d]MediaType:%d\tTrackNum:%d\n", byIndex, m_tCheatAudioCapSet[byIndex].GetAudioMediaType(),
					m_tCheatAudioCapSet[byIndex].GetAudioTrackNum());
			}
			
		
 	 	}
		StaticLog("\tm_byNeedMapIpNum:%d\n",m_byNeedMapIpNum);
		for (byIndex = 0; byIndex < m_byNeedMapIpNum; byIndex++)
		{
			if ( m_adwNeedMappedIp[byIndex] == 0  )
			{
				continue; //��ֵ�Ͳ�Ҫ��ӡ��
			}
			
			StaticLog("[DstNeedMappedIp:%d]:%s\n", byIndex,StrOfIP( ntohl(m_adwNeedMappedIp[byIndex]) ) );
 	 	}

// ���ǻ����ӡ [pengguofeng 3/5/2013]
        StaticLog("m_byMultiCastMtTableSize:%d\n", m_byMultiCastMtTableSize);
        for (byIndex = 0; byIndex < m_byMultiCastMtTableSize; byIndex++)
        {
            if (m_adwMultiCastMtTable[byIndex] == 0)
            {
                continue;
            }
            
            StaticLog("\tMultiCastMtTable[%d] = %s\n", byIndex, StrOfIP(ntohl(m_adwMultiCastMtTable[byIndex])));
            
        }

        StaticLog("m_dwPinHoleInterval: %d\n", m_dwPinHoleInterval);


		StaticLog("m_byMutePackManuEntryNum:%d\n",m_byMutePackManuEntryNum);
		StaticLog("m_byMutePackNum:%d\n",m_byMutePackNum);
		StaticLog("m_byMutePackSendInterval:%d\n",m_byMutePackSendInterval);
		for (byIndex = 0; byIndex < m_byMutePackManuEntryNum; byIndex++)
		{
			if (m_atSndSelfMutePackMt[byIndex].m_byManuId == 0)
			{
				continue;
			}
			
			StaticLog("m_atSndSelfMutePackMt[%d] manuId:%d,productid:%s\n", byIndex,m_atSndSelfMutePackMt[byIndex].m_byManuId,m_atSndSelfMutePackMt[byIndex].m_achProductId);
		}
		//g7221.c��Ƶ��ʽ����������ż�Ի�����
		StaticLog("\tm_byReverseG7221cNum:%d\n",m_byReverseG7221cNum);
		for (byIndex = 0; byIndex < m_byReverseG7221cNum; byIndex++)
		{
			if (m_atReverseG7221cMt[byIndex].m_byManuId == 0)
			{
				continue;
			}
			
			StaticLog("m_atReverseG7221cMt[%d] manuId:%d,productid:%s\n", byIndex,m_atReverseG7221cMt[byIndex].m_byManuId,m_atReverseG7221cMt[byIndex].m_achProductId);
		}
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
		StaticLog("\tm_byRcvH264DependInMarkNum:%d\n",m_byRcvH264DependInMarkNum);
		for (byIndex = 0; byIndex < m_byRcvH264DependInMarkNum; byIndex++)
		{
			if (m_atRcvH264DependInMarkMt[byIndex].m_byManuId == 0)
			{
				continue;
			}
			
			StaticLog("m_atSndSelfMutePackMt[%d] manuId:%d,productid:%s\n", byIndex,m_atRcvH264DependInMarkMt[byIndex].m_byManuId,m_atRcvH264DependInMarkMt[byIndex].m_achProductId);
		}
#endif
    }
}TMcuDebugVal;

//ָ���ն˵�Э���������Դ������ת������Դ������
struct TMtCallInterface
{	
public:
	u32      m_dwMtadpIpAddr; //Э�������IP��ַ, ������
	u32      m_dwMpIpAddr;    //����ת����IP��ַ, ������    
    TIpSeg   m_tIpSeg;        //IP��ַ��
    s8       m_szE164[MAXLEN_E164+1];   //E164����
    u8       m_byAliasType;   //��������

public:
    TMtCallInterface()
    {
        SetNull();
    }
    void SetNull()
    {
        memset(this, 0, sizeof(TMtCallInterface));
    }
    
    void SetE164Alias(LPCSTR lpszE164)
    {
        m_byAliasType = mtAliasTypeE164;
        memset(m_szE164, 0, sizeof(m_szE164));
        strncpy(m_szE164, lpszE164, MAXLEN_E164);
    }
    BOOL32 IsE164Same(LPCSTR lpszE164) const
    {
        // ֧��E164ͨ�����ƥ��ǰ�μ���
        return (strncmp(m_szE164, lpszE164, strlen(m_szE164)) == 0);
    }
    
    void SetIpSeg(u32 dwIpStart, u32 dwIpEnd)   //Ҫ��������
    { 
        m_byAliasType = mtAliasTypeTransportAddress;
        m_tIpSeg.SetSeg( dwIpStart, dwIpEnd );
    }
};

struct TDri2E1Cfg
{
protected:
    u32 m_wDriIp;
    u8  m_byPortNO;
    u32 m_dwIpAddr;
    u16 m_wRealBandWidth;
    u8  m_byCfgE1Num;       //����E1��
	
public:
    TDri2E1Cfg() : m_wDriIp(0),
        m_byPortNO(0),
        m_dwIpAddr(0),
        m_wRealBandWidth(0){}
    u32 GetDriIp(){ return m_wDriIp; };
    void SetDriIp( u32 wDriId){ m_wDriIp = wDriId; }
	
    u8  GetPortNO(){ return m_byPortNO; }
    void SetPortNO( u8 byPortNO ){ m_byPortNO = byPortNO; }
	
    u32 GetIpAddr(){ return m_dwIpAddr; }
    void SetIpAddr( u32 dwIpAddr){ m_dwIpAddr = dwIpAddr; }
	
	u16 GetRealBandWidth(){ return m_wRealBandWidth;}
	void SetRealBandWidth (u16 wRealBandwidth){ m_wRealBandWidth = wRealBandwidth;}
	
    u8 GetCfgE1Num(){ return m_byCfgE1Num; }
    void SetCfgE1Num(u8 byE1Num){ m_byCfgE1Num = byE1Num; }
	
    BOOL32 IsInvalid()
    {
        if (m_wDriIp == 0 || m_dwIpAddr == 0)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
};

//�ն��б�����ṹ len��3192
struct TMtListDiscard
{
public:
    TMtListDiscard(){   Clear(); }
    void Clear(void)
    {
        memset( &m_awMixMcuIdx, INVALID_MCUIDX, sizeof(m_awMixMcuIdx));
        memset( &m_aabyMixMtId, 0, sizeof(m_aabyMixMtId));
    }

    void SetMtList(TMt *ptMtList, u16 wMtNum)
    {
        if ( NULL == ptMtList )
        {
            return;
        }
        
		if ( 0 == wMtNum || wMtNum > MAXNUM_CONFSUB_MCU * MAXNUM_MIXING_MEMBER )
        {
            return;
        }

        for( u8 byIdx = 0; byIdx < wMtNum; byIdx ++)
        {
            SetNewMt(*ptMtList);
            ptMtList ++;
        }
    }

    void GetMtList( /*IN*/ u16 wBufLen, 
					/*OUT*/ u8 &byGrpNum, 
					/*OUT*/ u8 *pbyMtNum, 
					/*OUT*/ TMt * ptMt)
    {
        if ( NULL == pbyMtNum || NULL == ptMt )
        {
            return;
        }

        u16 wMaxRetMtNum = wBufLen / sizeof(TMt);
        u16 wRetMtNum = 0;
        
        for( u16 wMcuIdx = 0; wMcuIdx < MAXNUM_CONFSUB_MCU; wMcuIdx++)
        {
            if ( IsValidSubMcuId(m_awMixMcuIdx[wMcuIdx]) )
            {
                byGrpNum ++;

                for( u8 byMtIdx = 0; byMtIdx < MAXNUM_MIXING_MEMBER; byMtIdx++)
                {
                    if ( 0 != m_aabyMixMtId[wMcuIdx][byMtIdx] )
                    {
                        wRetMtNum ++;
                        if ( wRetMtNum > wMaxRetMtNum )
                        {
                            LogPrint(LOG_LVL_ERROR, MID_MCU_MT,"[TMtListDiscard]: [GetMtList] Cur space.%d is not enough for the left MTs, need to relocated\n", wBufLen );
                            break;
                        }

                        TMt tMt;
                        tMt.SetMtId(m_aabyMixMtId[wMcuIdx][byMtIdx]);
                        tMt.SetMcuIdx(m_awMixMcuIdx[wMcuIdx]);
                        *ptMt = tMt;

                        ptMt ++;
                        (*pbyMtNum) ++;
                    }
                    else
                    {
                        break;
                    }
                }
                
                pbyMtNum ++;
            }
			else
			{
				break;
			}
        }
    }
    
    void SetNewMt(const TMt &tMt)
    {
        u16 wMcuIdx = 0;
        if (IsMcuMixing(tMt.GetMcuId(), wMcuIdx) &&
            IsMtMixing(wMcuIdx, tMt.GetMtId()))
        {
            return;
        }

        SetMtMixing(tMt);
        return;
    }

    BOOL32 IsMcuMixing(u16 wMcuId)
    {
        BOOL32 bMixing = FALSE;
        for(u16 wIdx = 0; wIdx < MAXNUM_CONFSUB_MCU; wIdx++)
        {
            if ( m_awMixMcuIdx[wIdx] == wMcuId )
            {
                bMixing = TRUE;
                break;
            }
        }
        return bMixing;
    }

protected:
    void SetMtMixing(const TMt &tMt)
    {
        u16 wMcuIdx = 0;
        if (IsMcuMixing(tMt.GetMcuId(), wMcuIdx))
        {
            SetMtMixing(tMt, wMcuIdx, FALSE);
        }
        else
        {
            u16 wIdleMcuIdx = INVALID_MCUIDX; 
            if ( GetIdleMcuIdx(wIdleMcuIdx) )
            {
                SetMtMixing(tMt, wIdleMcuIdx, TRUE);
            }
            else
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_MT, "[TMtListDiscard][SetMtMixing] No idle mcu idx exist for mt<%d.%d>, impossible\n", tMt.GetMcuId(), tMt.GetMtId() );
            }
        }
    }

    void SetMtMixing(const TMt &tMt, u16 wMcuIdx, BOOL32 bFst)
    {
		// liuxu, wMcuIdx��֤
		if (!IsValidSubMcuId(wMcuIdx))
		{
			return;
		}

        if ( !bFst )
        {
            u8 byNullMtIdx = MAXNUM_MIXING_MEMBER;
            
            for( u8 byMtIdx = 0; byMtIdx < MAXNUM_MIXING_MEMBER; byMtIdx++)
            {
                if ( 0 == m_aabyMixMtId[wMcuIdx][byMtIdx] )
                {
                    m_aabyMixMtId[wMcuIdx][byMtIdx] = tMt.GetMtId();
                    byNullMtIdx = byMtIdx;
                    break;
                }
            }
        }
        else
        {
            m_aabyMixMtId[wMcuIdx][0]= tMt.GetMtId();
            m_awMixMcuIdx[wMcuIdx] = tMt.GetMcuId();
        }

        return;
    }

    BOOL32 IsMtMixing(u16 wMcuIdx, u8 byMtId)
    {
		// liuxu, wMcuIdx��֤
		if (!IsValidSubMcuId(wMcuIdx))
		{
			return FALSE;
		}

        BOOL32 bMtMixing = FALSE;
        for( u8 byMtIdx = 0; byMtIdx < MAXNUM_MIXING_MEMBER; byMtIdx ++)
        {
            if (m_aabyMixMtId[wMcuIdx][byMtIdx] == byMtId)
            {
                bMtMixing = TRUE;
                break;
            }            
        }

        return bMtMixing;
    }

    BOOL32 IsMcuMixing(u16 wMcuId, u16 &wMcuIdx)
    {
        // liuxu, ��MAXNUM_CONFSUB_MCU��ΪINVALID_MCUIDX
        wMcuIdx = INVALID_MCUIDX;

		// liuxu, ���˱���mcu. �����������mcu������й���, ���¿��ܻ�ɾ��
		if (IsLocalMcuId(wMcuId))
		{
			return FALSE;
		}

        BOOL32 bMixing = FALSE;
        for(u16 wIdx = 0; wIdx < MAXNUM_CONFSUB_MCU; wIdx++)
        {
	        if ( m_awMixMcuIdx[wIdx] == wMcuId )
            {
                wMcuIdx = wIdx;
                bMixing = TRUE;
                break;
            }
        }

        return bMixing;
    }

    BOOL32 GetIdleMcuIdx(u16 &wMcuIdx)
    {
        BOOL32 bIdleExist = FALSE;

		// liuxu, ��MAXNUM_CONFSUB_MCU��ΪINVALID_MCUIDX
        wMcuIdx = INVALID_MCUIDX;

        for(u16 wIdx = 0; wIdx < MAXNUM_CONFSUB_MCU; wIdx++)
        {
            if ( !IsLocalMcuId(wIdx)									// �Ǳ���mcu id
				&& !IsValidSubMcuId(m_awMixMcuIdx[wIdx]))				// m_awMixMcuIdx[wIdx]ֵΪ��
            {
                bIdleExist = TRUE;
                wMcuIdx = wIdx;
                break;
            }
        }

        return bIdleExist;        
    }

protected:
    u8 m_aabyMixMtId[MAXNUM_CONFSUB_MCU][MAXNUM_MIXING_MEMBER];
    u16 m_awMixMcuIdx[MAXNUM_CONFSUB_MCU];
};

//��������ȴ�����
struct TApplySpeakQue
{
public:
    TApplySpeakQue();
    ~TApplySpeakQue();
    
    void   Init( void );
    void   Quit( void );
    BOOL32 ProcQueueInfo( TMt &tMt, BOOL32 &bIsSendToChairman, BOOL32 bInc = TRUE, BOOL32 bDel = TRUE );
    BOOL32 IsQueueFull( void );
    BOOL32 IsQueueNull( void );
    BOOL32 IsMtInQueue( const TMt &tMt, u8* pbyPos = NULL );
    void   ClearQueue( void );
    void   ShowQueue( void );
    void   PrtQueueInfo( BOOL32 bPrt = TRUE);
    void   GetQueueList( /*IN OUT*/ TMt *ptMtList, /*IN OUT*/ u8 &byLen );
    void   QueueLog( s8* pszStr, ... );
	BOOL32 PopQueueHead( TMt &tMt );
	BOOL32 GetQueueHead( TMt &tMt );
	BOOL32 GetQueueNextMt( const TMt tCurMt,TMt &tNextMt );
	BOOL32 RemoveMtByMcuIdx( u16 wMcuIdx,BOOL32 &bIsSendToChairman );
	BOOL32 IsMcuInQueue( TMt &tMt, u8* pbyPos = NULL );
    
protected:
    TMt             m_atMtList[MAXNUM_CONF_MT];
    u8              m_byLen;
    s32             m_nHead;
    s32             m_nTail;
    BOOL32          m_bPrtChgInfo; //�Ƿ�ʵʱ��ӡ���ж��еĳ�������Ϣ
};

/*==============================================================================
����    :  CVmpMemVidLmt
����    :  ����ϳɳ�Ա��Ƶ�ֱ��ʵ���������
��Ҫ�ӿ�:  
��ע    :  
-------------------------------------------------------------------------------
�޸ļ�¼:  
��  ��     �汾          �޸���          �߶���          �޸ļ�¼
2009-2-26                Ѧ��							 create
==============================================================================*/
class CVmpMemVidLmt
{
public:
	CVmpMemVidLmt( void )
	{
		Init();
	}
	
    void Init( void );
    BOOL32 GetMtVidLmtForHd ( u8 byStyle, u8 byMemPos, u8 &byMtRes, u16 &wMtBandWidth);
    BOOL32 GetMtVidLmtForStd ( u8 byStyle, u8 byMemPos, u8 &byMtRes, u16 &wMtBandWidth);
	
private:
	TMtVidLmt m_atMtVidLmtHd[VMPSTYLE_NUMBER][25];	//MAXNUM_MPUSVMP_MEMBER��δ����Ϊ25����25��ʱ���� yhz~
	TMtVidLmt m_atMtVidLmtStd[VMPSTYLE_NUMBER][25];	//MAXNUM_MPUSVMP_MEMBER��δ����Ϊ25����25��ʱ����
	
};

/*==============================================================================
����    :  CSmsControl
����    :  ����Ϣ���Ϳ���
��Ҫ�ӿ�:  
��ע    :  
-------------------------------------------------------------------------------
�޸ļ�¼:  
��  ��     �汾          �޸���          �߶���          �޸ļ�¼
2011-1-19                Ѧ��							 create
==============================================================================*/
class CSmsControl
{
	/*lint -save -e1566*/
public:
	enum EnumUseState 
	{ 
		IDLE,		
		BUSY
	};
	
	CSmsControl( void);
	void	Init(void);
	
	void	SetServMsg( const CServMsg &cServMsg);
	CServMsg *GetServMsg(void);
	
	void	SetTimerSpan(u16 wTimerSpan);
	u16		GetTimerSpan(void) const;

	void	SetMtPos( u16 m_wPos );
	u16		GetMtPos( void ) const;

	void	SetState(u8 byState);
	BOOL32	IsStateIdle() const;
	
private:
	CServMsg	m_cServMsg;
	u16			m_wTimerSpan;
	u16			m_wPos;			
	u8			m_byState;

};

/*size is 1056
struct TVmpCommonAttrb
{
	
	u8		m_byMemberNum;
	BOOL32	m_bPRS;
	u8		m_byVmpStyle;
	TVMPMemberEx    m_atMtMember[MAXNUM_VMP_MEMBER];
	TMediaEncrypt   m_tVideoEncrypt[MAXNUM_VMP_MEMBER];     //��Ƶ���ܲ���
	TDoublePayload  m_tDoublePayload[MAXNUM_VMP_MEMBER];
	TVmpStyleCfgInfo m_tVmpStyleCfgInfo;

	TVmpCommonAttrb()
	{
		memset(this, 0, sizeof(TVmpCommonAttrb));
	}
};*/

/*==============================================================================
����    :  CParamToVmp
����    :  ��VMP�²�
��Ҫ�ӿ�:  ConstructMsgToVmp
��ע    :  �����
-------------------------------------------------------------------------------
�޸ļ�¼:  
��  ��     �汾          �޸���          �߶���          �޸ļ�¼
2011-2-18                 Ѧ��							 create
==============================================================================*/
class CParamToVmp
{
public:
	//construct
	CParamToVmp(TVmpCommonAttrb &tVmpCommonAttrb, u8 byConfIdx, u8 byVmpId)
	{
		m_byVmpId   = byVmpId;
		m_byConfIdx = byConfIdx;
		memcpy(&m_tVmpCommonAttrb, &tVmpCommonAttrb, sizeof(TVmpCommonAttrb));
		
	};
	CParamToVmp(TVmpCommonAttrb &tVmpCommonAttrb, u8 byConfIdx, u8 byVmpId, TKDVVMPOutParam &tVMPOutParam)
	{
		m_byVmpId   = byVmpId;
		m_byConfIdx = byConfIdx;
		memcpy(&m_tVMPOutParam, &tVMPOutParam, sizeof(TKDVVMPOutParam));
		memcpy(&m_tVmpCommonAttrb, &tVmpCommonAttrb, sizeof(TVmpCommonAttrb));
		
	};

	virtual void	ConstructMsgToVmp(CServMsg &cServMsg) = 0;
//	virtual	void	SendMsgToVmp();
protected:
	u8		m_byVmpId;
	u8		m_byConfIdx;
	TKDVVMPOutParam m_tVMPOutParam;
	TVmpCommonAttrb	m_tVmpCommonAttrb;
	
}; 

class CParamToOldVmp : public CParamToVmp
{
public:
	CParamToOldVmp(TVmpCommonAttrb &tVmpCommonAttrb, u8 byConfIdx, u8 byVmpId)
		:CParamToVmp(tVmpCommonAttrb, byConfIdx, byVmpId){};
	virtual void	ConstructMsgToVmp(CServMsg &cServMsg);
//	virtual	void	SendMsgToVmp();
};

class CParamToMpuVmp : public CParamToVmp
{
public:
	CParamToMpuVmp( TVmpCommonAttrb &tVmpCommonAttrb, u8 byConfIdx, u8 byVmpId)
		:CParamToVmp(tVmpCommonAttrb, byConfIdx, byVmpId){};
	virtual void	ConstructMsgToVmp(CServMsg &cServMsg);
//	virtual	void	SendMsgToVmp();
};

// ��׷���࣬����MPU2���Ժ�ϳ���
class CParamToKDVVmp : public CParamToVmp
{
public:
	CParamToKDVVmp( TVmpCommonAttrb &tVmpCommonAttrb, u8 byConfIdx, u8 byVmpId, TKDVVMPOutParam &tVMPOutParam)
		:CParamToVmp(tVmpCommonAttrb, byConfIdx, byVmpId, tVMPOutParam){};
	virtual void	ConstructMsgToVmp(CServMsg &cServMsg);
	//	virtual	void	SendMsgToVmp();
};

class CParamTo8kg8khVmp : public CParamToVmp
{
public:
	CParamTo8kg8khVmp( TVmpCommonAttrb &tVmpCommonAttrb, u8 byConfIdx, u8 byVmpId)
		:CParamToVmp(tVmpCommonAttrb, byConfIdx, byVmpId){};

	virtual void	ConstructMsgToVmp(CServMsg &cServMsg);
//	virtual	void	SendMsgToVmp();
};

class CParamTo8kiVmp : public CParamToVmp
{
public:
	CParamTo8kiVmp( TVmpCommonAttrb &tVmpCommonAttrb, u8 byConfIdx, u8 byVmpId, TKDVVMPOutParam &tVMPOutParam)
		:CParamToVmp(tVmpCommonAttrb, byConfIdx, byVmpId, tVMPOutParam){};
	
	virtual void	ConstructMsgToVmp(CServMsg &cServMsg);
	//	virtual	void	SendMsgToVmp();
};

//Ϊȷ��������ϳ�Ҫ��ֱ��ʶ�����Ķ�����Ϣ
struct TExInfoForInVmpRes
{
	BOOL32 m_bConsiderVmpBrd;	//�Ƿ���VMP�㲥���
	BOOL32 m_bNonKeda;			//�Ƿ��϶���Ϊ�⳧���նˣ���TRUE���򼴱������ӿ�����������[��������в�׼]��Ҳ��Ϊ���⳧���նˣ�
	u8     m_byMemberType;		//Ҫ������mt��ͨ������,����֧��˫������ͨ��
	u8     m_byPos;				//ָ������mt��ͨ����
	u32	   m_dwReserved;
	u8     m_byVmpId;

	TExInfoForInVmpRes(void)
	{
		//memset(this, 0, sizeof(TExInfoForInVmpRes));
		m_bConsiderVmpBrd = TRUE;
		m_bNonKeda = FALSE;
		m_byMemberType = VMP_MEMBERTYPE_NULL;
		m_byPos = MAXNUM_VMP_MEMBER;
		m_dwReserved = 0;
		m_byVmpId = 0;
	}
};

//Ϊȷ��������Ҫ��ֱ��ʶ�����Ķ�����Ϣ
enum TExInfoForResFpsEvMask
{
	EvMask_Invalid = 0,
	EvMask_TVWall,		// �������ǽ���
	EvMask_HDUVMP,		// hdu-vmp
	EvMask_VMP,			// vmp���
	EvMask_Speaker,		// ���������
	EvMask_BeSelected		// ��ѡ��
};
enum TExInfoForResFpsOccupy
{
	Occupy_Invalid = 0,
	Occupy_InChannel,
	Occupy_OutChannel,
	Occupy_BeSelected,
	Occupy_StopSelect,
	Occupy_BeSpeaker,
	Occupy_CancelSpeaker
};

struct TExInfoForResFps
{
	
	u8		m_byEvent;				//������ʽ
	u8		m_byOccupy;				//ռ��ͨ��/ȡ��ռ��ͨ��
	BOOL32	m_bConsiderVmpBrd;		//�Ƿ���VMP�㲥���
	BOOL32	m_bSeizePromt;			//�Ƿ�����ռ������Ҫ��ռ��ʾ
	u8		m_byEqpid;				//��Դ��������Eqpid����Ҫ����vmp��Ӧ����
	u8		m_byPos;				//��Դ����ͨ���ţ���Ҫ����vmp��Ӧ����
	u8		m_byHduSubChnId;		//(HDU�໭��ʹ��)
	u8		m_byMemberType;			//Ҫ������mt��ͨ������,����֧��˫������ͨ��
	BOOL32	m_bNonKeda;				//�Ƿ��϶���Ϊ�⳧���նˣ���TRUE���򼴱������ӿ�����������[��������в�׼]��Ҳ��Ϊ���⳧���նˣ�
	TVMPParam_25Mem m_tVmpParam;	//�ϳ���Ϣ�����ڲ��ı�ԭ�ϳɳ�Ա���ն˷ֱ�����Ԥ�У���ռǰ����(������ش�����ʱ��)
	
	//Ҫ�������Ľ��
	u8		m_byNewFormat;			//Ҫ�����ķֱ���
	u8		m_byNewFps;				//Ҫ������֡��
	BOOL32	m_bResultSetVmpChl;		//��������Ƿ񽨽���
	BOOL32	m_bResultSeizePromt;	//��������Ƿ�����ռ��ʾ
	BOOL32	m_bResultStart;			//��������ն˵������ǻָ�(�ն˲����κ�����ͨ��ʱΪ�ָ�)
	BOOL32  m_bIsForceAdjust;		//ǿ�Ƶ����ֱ���(HDU�໭��ʹ��)

	TExInfoForResFps(void)
	{
		Clear();
	}
	
	void Clear()
	{
		m_byEvent = EvMask_Invalid;
		m_byOccupy = Occupy_Invalid;
		m_bConsiderVmpBrd = TRUE;
		m_bSeizePromt = FALSE;
		m_byEqpid = 0;
		m_byPos = MAXNUM_VMP_MEMBER;
		m_byHduSubChnId = 0;
		m_byMemberType = VMP_MEMBERTYPE_NULL;
		m_bNonKeda = FALSE;
		m_byNewFormat = VIDEO_FORMAT_INVALID;
		m_byNewFps = 0;
		m_bResultSetVmpChl = FALSE;
		m_bResultSeizePromt = FALSE;
		m_bResultStart = FALSE;
		m_bResultSetVmpChl = FALSE;
		m_bResultSeizePromt = FALSE;
		m_bResultStart = FALSE;
		m_bIsForceAdjust = FALSE;
	}
};


//��ȡMCU Debug�ļ�key value
void McuGetDebugKeyValue(TMcuDebugVal &tTMcuDebugVal);

//��ȡMCU Debug�ļ�����ָ���ն˵�Э���������Դ������ת������Դ������
BOOL32 McuGetMtCallInterfaceInfo( TMtCallInterface *ptMtCallInterface, u32 &dwEntryNum );

u8     GetMtAliasTypeFromString(s8* pszAliasString);
BOOL32 IsIPTypeFromString(s8* pszAliasString);
BOOL32 IsE164TypeFromString(s8 *pszSrc, s8* pszDst);
BOOL32 IsVidFormatHD(u8 byVidFormat);

BOOL32 IsHDConf( const TConfInfo &tConfInfo );
BOOL32 IsConfDualEqMV( const TConfInfo &tConfInfo );
BOOL32 IsConfDoubleDual( const TConfInfo &tConfInfo );
BOOL32 IsConfFR50OR60(const TConfInfo &tConfInfo);
BOOL32 IsDSFR50OR60(const TConfInfo &tConfInfo);

//�����ȡ����
BOOL32 CreateConfStoreFile(void);

//zjj20091102 ��һ�������������ϵ�TConfStoreָ�룬������޸�ģ��Ͳ�Ϊ�գ�����ģ��ʱΪ��
//Ϊ���������޸ĺ���������Ϊ�޸�ʱҪ�ѻ�����Ϣ�ļ��еĶ�̬�����������ó���(���ڽ���ʱvcs�����е�Ԥ��)
BOOL32 AddConfToFile( TConfStore &tConfStore, BOOL32 bDefaultConf = FALSE,TConfStore *ptOldConfStore =	NULL );

BOOL32 DeleteConfFromFile( CConfId cConfId );
BOOL32 GetConfFromFile( u8 byIndex, TPackConfStore *ptPackConfStore );
u8     GetOneConfIndexFromFile( CConfId cConfId );
CConfId GetOneConfHeadFromFile( u8 byIndex );
BOOL32 SetOneConfHeadToFile( u8 byIndex, CConfId cConfId );
BOOL32 GetAllConfHeadFromFile( CConfId *pacConfId, u32 dwInBufLen );
BOOL32 SetAllConfHeadToFile( CConfId *pacConfId, u32 dwInBufLen );
void   BackConfHeadInfo( void );
BOOL32 GetConfDataFromFile( u8 byIndex, CConfId cConfId, u8 *pbyBuf, u32 dwInBufLen, u16 &wOutBufLen,BOOL32 bReadAllData = FALSE );
BOOL32 SetConfDataToFile( u8 byIndex, CConfId &cConfId, u8 *pbyBuf, u16 wInBufLen, BOOL32 bWriteAllData = FALSE );

//Unpack���ݵ�TConfInfoEx��[12/29/2011 chendaiwei]
//���������չ pengguofeng
void   UnPackConfInfoEx(TConfInfoEx & tConfInfoEx,const u8 *pbyBuf,
						u16 &wUnPackConfExLen,BOOL32 &bExistUnkonwInfo,
						THduVmpModuleOrTHDTvWall *ptHduVmpModule=NULL,
						TVmpModuleInfo *ptVmpModule = NULL,
						u8 *pbyEncoding = NULL);
//pack TConfInfoEx�е�����
// pbyEncoding��ģ�������Ϣ [pengguofeng 4/12/2013]
void   PackConfInfoEx(const TConfInfoEx &tConfInfoEx, u8 * pbyOutbuf,
					  u16 &wPackConfDataLen,
					  THduVmpModuleOrTHDTvWall *ptHduVmpModule=NULL,
					  TVmpModuleInfo *ptVmpModule = NULL,
					  u8 *pbyEncoding = NULL);
TConfInfoEx GetConfInfoExFromConfAttrb(const TConfInfo& tConfInfo);

BOOL32 PackConfStore( TConfStore &tConfStore, TPackConfStore *ptPackConfStore, u16 &wPackConfDataLen );
BOOL32 UnPackConfStore( TPackConfStore *ptPackConfStore, TConfStore &tConfStore, u16 &wPackConfDataLen, BOOL32 bUnPackExInfo = TRUE);
BOOL32 UpdateConfStoreModule( const TConfStore &tConfStore, 
                            u8 byOldIdx, u8 byNewIdx,
                            TMultiTvWallModule &tOutTvwallModule, 
                            TVmpModule &tOutVmpModule,
							THduVmpModuleOrTHDTvWall &tHduVmpModule,
							TVmpModuleInfo &tVmpModuleEx);   // guzh [7/5/2007] �����ն��������µ���ǽ�ͻ���ϳ�ģ��
BOOL32 SetUnProConfDataToFile(u8 byIndex, s8 *pbyBuf, u32 dwInBufLen, u8 byOverWrite = TRUE);
u32    GetUnProConfDataToFileLen(u8 byIndex);
BOOL32 GetUnProConfDataToFile(u8 byIndex, s8 *pbyBuf, u32& dwOutBufLen, u32 dwBeginPos = 0);

//zjj20091102 ��û����ļ��������ݴ�С������ֻ�����vcs�����еĵ���ǽԤ��
//����Ԥ�����ݳ��Ⱥ���չ��Ϣ����[8/16/2012 chendaiwei]
BOOL32 GetConfExtraDataLenFromFile( TConfStore tConfStore, u16 &wPlanDataLen, u16 &wExInfoLenth,BOOL32 bIsDefaultConf = FALSE );
BOOL32 GetConfExtraDataFromFile( TConfStore tConfStore,	u8 *pbyBuf,u16 wBufInLen, u8*pbyExInfoBuf,u16 wExInfoBufInLen, BOOL32 &bhasFix2PlanData,BOOL32 bIsDefaultConf = FALSE);
BOOL32 SaveConfExtraPlanDataToFile( TConfStore &tConfStore, u8 *pbyBuf,u16 wDataLen,BOOL32 bIsDefaultConf = FALSE );

//��������
LPCSTR GetMediaStr( u8 byMediaType );
LPCSTR GetResStr( u8 byResolution );
LPCSTR GetManufactureStr( u8 byManufacture );
void AddEventStr();
void StartLocalEqp( void );
void StartLocalMp( void );
void StopLocalMp( void );

void StartLocalMtadp( void );
u8   GetVmpChlNumByStyle( u8 byVMPStyle );
BOOL32 IsValidVmpId(u8 byVmpId);
//zhouyiliang 20110117 ��������
//u8	GetVmpOutChnnlByRes(u8 byRes, u8 byVmpId, const TConfInfo &tConfInfo, u8 byMediaType = MEDIA_TYPE_H264);
//u8	GetResByVmpOutChnnl(u8 &byMediaType, u8 byChnnl, const TConfInfo &tConfInfo, u8 byVmpId);

BOOL32 PackTMtAliasArray( TMtAlias *ptMtAlias, u16* pawMtDialRate, u8 byMtNum, char* const pszBuf, u16 &wBufLen );
BOOL32 UnPackTMtAliasArray(const char *pszBuf, u16 wBufLen, const TConfInfo *ptConfInfo, TMtAlias *ptMtAlias, u16* pawMtDialRate, u8 &byMtNum );
u16  GetAudioBitrate( u8 byAudioType );

u16 GetMaxAudioBiterate(const TConfInfoEx &tConInfo);

void GetPrsTimeSpan( TPrsTimeSpan *ptPrsTimeSpan );

u8  *GetRandomKey();
void TableMemoryFree( s8 **ppMem, u32 dwEntryNum );

u8   GetActivePayload(const TConfInfo &tConfInfo, u8 byRealPayloadType);
u8   GetActivePL(const TConfInfo &tConfInfo, u8 byMediaType);

BOOL32 IsDSResMatched(u8 bySrcRes, u8 byDstRes);
BOOL32 IsResPresentation(u8 byRes);
BOOL32 IsResLive(u8 byRes);
BOOL32 IsResGE(u8 bySrcRes, u8 byDstRes);
BOOL32 IsResG(u8 bySrcRes, u8 byDstRes);

//��������ģ�����Ƿ�����Ӧ�Ĺ�ѡ֧������С��ĳ������
BOOL32 IsConfSupportMainCap(const TConfInfoEx& tConfInfoEx, const TSimCapSet &tMainSimCapSet);
//����˫��ģ�����Ƿ�����Ӧ�Ĺ�ѡ֧������С��ĳ������
BOOL32 IsConfSupportDSCap(const TConfInfoEx& tConfInfoEx, const TDStreamCap &tSimCapSet);
//�����Ƿ���Խ�������
BOOL32 IsConfCanCompactAdapt(const TConfInfo& tConfinfo, const TConfInfoEx &tConfInfoEx);
//�����Ƿ񵥷ֱ���
BOOL32 IsConfSingleRes(const TConfInfo& tConfinfo, const TConfInfoEx &tConfInfoEx);
//�����Ƿ�������Դ�������
BOOL32 IsConfAdpOptimization(const TConfInfo& tConfinfo);


//��������ʽ�Ƿ���ҪԤ��
BOOL32	IsConfNeedReserveMainCap(const TConfInfo& tConfinfo, const TConfInfoEx &tConfInfoEx);
//˫������ʽ�Ƿ���ҪԤ��
BOOL32	IsConfNeedReserveDSMainCap(const TConfInfo& tConfinfo, const TConfInfoEx &tConfInfoEx); 
//8000i�Ƿ�˫�������264����
BOOL32 Is8KINeedDsAdaptH264SXGA( const TConfInfo& tConfinfo );
//��ǰ���������Ƿ���bp�������й�ѡbp����
BOOL32 IsConfHasBpCapOrBpExCap( const TConfInfo& tConfinfo,const TConfInfoEx& tConfEx );

u8 TransFRtoReal(u8 byMediaType, u8 byFrame);

u8     GetChnType(const TEqp &tEqp, u8 byChnId);
BOOL32 GetBasInputChnNumAcd2Type(u8 byHDType, u8 &byInputChnNum);		  //����bas���ͻ�ȡ������ͨ����

void GetMacStrSegmentedbySep(u8 *pbyBufIn, s8 *pchBufOut, s8 *pchSep);	  //��ָ���ָ���õ�mac��ַ	

s8  *GetAddrBookPath(void);

const s8* GetMtLeftReasonStr(u8 byReason);

u8 GetMcuPdtType(void);  // ��ȡMCU��Ʒ(���)����
const s8* GetMcuVersion();  //��ȡMCU��ϵͳ�汾��
u8 GetResByWH( u16 wWidth, u16 wHeight );   // ���ݿ�߻�÷ֱ���
void GetWHByRes(u8 byRes, u16 &wWidth, u16 &wHeight); // ���ݷֱ��ʻ�ÿ��
BOOL32 IsSrcResThanDst(u8 bySrcRes, u8 byDstRes);// �ֱ��ʱȽ�
s32 ResWHCmp(u8 bySrcRes, u8 byDstRes); // �ֱ��ʱȽ�
s32 WHCmp(u16 wSrcWidth, u16 wSrcHeight, u16 wDstWidth, u16 wDstHeight);	//��߷ֱ�Ƚ�
u8  GetSuitableRes(u8 byLmtRes, u8 byOrgRes);
u8  GetSuitableResByWH(u16 wLmtWidth, u16 wLmtHeight, u16 wOrgWidth, u16 wOrgHeight);
//[22/6/2011zhangli]����Ӧ�ֱ�������£��������ʵõ��ֱ���
u8 GetAutoResByBitrate(u8 byRes, u16 wBitrate);
u8 FrameRateMac2Real(u8 byFrameRate);
u8 GetDownStandRes(u8 byRes);
//[2011/08/27/zhangli]��ȡ�ȵ�ǰ�ֱ��ʴ��һ����׼�ֱ���
u8 GetUpStandRes(u8 byRes);
u8 GetMinResAcdWHProduct(u8 bySrcRes, u8 byDstRes);
//[20120913 yanghuaizhi]���ݿ��,���һ��ͨ�õķֱ���
u8 GetNormalRes(u16 wResW, u16 wResH);
u8 GetMcuSupportedRes(u16 wResW, u16 wResH);

//����TSimCapSet��ö�ӦTNeedBasResData
BOOL32	GetBasNeedResDataByCap(const TSimCapSet &tSimCapSet,const TSimCapSet &tDstCap, u8 byMediMode, TNeedVidAdaptData &tBasResData);

//��ö�Ӧ�ֱ���֡���ڱ���ʱ�������Դ��λ
u16 GetResourceUnitAcd2Cap(u8 byMediaType, u8 byRes, u8 byFrameRate);
u16 GetResourceUnitAcd2Cap(const TVideoStreamCap &tSimCapSet);

//�Ƿ���Ҫ����Դ���ֱ���
BOOL32 IsNeedAdjustCapBySrc(const TVideoStreamCap &tVidSrcCap,TVideoStreamCap &tVidDstCap);

//���idx��Ӧ�ı�����Դ
u16 GetResourceUnitAcd2CapArray(TSimCapSet *tSimCapSet,u8 byNum);

u8 GetAACChnlTypeByAudioTrackNum( u8 byAudioTrackNum );
BOOL32 IsV4R6B2VcsTemplate ( FILE * hConfFile, u16 wFileLenth);
u16 GetPlanDataLenByV4R6B2PlanData( u8*pbyV4R6B2PlanData, u16 wV4R6B2Length);
BOOL32 TransferV4R6B2planDataToV4R7(u8*pbyV4R6B2PlanData,u16 wV4R6B2DataLen,u8*pbyV4R7PlanData,u16 wV4R7DataLen );
/** 
/* @�� �� �� : GetMtPosInMtArray
/* @��    �� : ���ն��б�ptMtList�л�ȡ�����ն�tSpecMt������λ��
/* @��    �� : [in]tSpecMt  -- ��Ѱ�ҵ��ն�
			   [in]ptMtList -- �ն��б�
			   [in]byMtNum  -- �ն��б���Ŀ
/* @�� �� ֵ : �ն�tSpecMt����ptMtList�е�����λ��[0��byMtNum),û�ҵ�����-1
/* @�� �� �� : liuxu         ʱ  �� : 2011/06/13/
*/
extern const s16 GetMtPosInMtArray( const TMtAlias& tSpecMt, const TMtAlias* ptMtList, const u8 byMtNum );

/** 
/* @��    �� : ����ͬʱ��������ȡ��֮��Ӧ�Ľ�������
/* @��    �� : [in]TSimCapSet, �ն˻������ͬʱ������
/* @�� �� ֵ : TBasChnData::DecAbility
/* @�� �� �� : liuxu         ʱ  �� : 2010/11/1/
*/
extern const s16 GetDecAbility( const TSimCapSet& );

/*
*	���ܣ���ȡ8000H-M�ĵ�ǰ������������ַ
*  ԭ���˵�ַҲ��Ĭ��·�ɵĵ�ַ
*  ���أ�0�� ^0
*/
extern u32  GetSwitchSndBindIp();
//extern BOOL32 CorrectUtf8Str( s8 * pStr, const u16 wStrLen);

//mcu���Դ�ӡ�ӿ�
API void pdcsmsg( void );
API void npdcsmsg( void );
API void pmcsmsg( void );
API void npmcsmsg( void );
API void pvcsmsg( void );
API void npvcsmsg( void );
API void peqpmsg( void );
API void npeqpmsg( void );
API void pmmcumsg(void);
API void npmmcumsg(void);
API void pmt2msg( void );
API void npmt2msg( void );
API void pcallmsg( void );
API void npcallmsg( void );
API void pmpmgrmsg(void);
API void npmpmgrmsg(void);
API void pcfgmsg( void );
API void npcfgmsg( void );
API void pmtmsg( void );
API void npmtmsg( void );
API void pgdmsg( void );
API void npgdmsg( void );
API void pnplusmsg( void );
API void npnplusmsg( void );
API void ppfmmsg( void );
API void nppfmmsg( void );
API void sconftotemp( s8* psConfName = NULL );
API void resetlogin( void );
API void showbas( u8 byConfIdx = 0, u8 byBasId = 0 );
API void showbaslist();
API void showprs();
API void pspymsg( void );
API void npspymsg( void );
API void showmcothertable(void);
API void mculogflush(void);

extern BOOL32 g_bPrintEqpMsg;
extern BOOL32 g_bPrintCfgMsg;
extern BOOL32 g_bPrintMcsMsg;
extern BOOL32 g_bPrintNPlusMsg;
extern BOOL32 g_bPrintMtMsg;

#undef SETBITSTATUS
#undef GETBITSTATUS

#endif
