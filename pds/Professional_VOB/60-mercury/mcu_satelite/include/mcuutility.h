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
#include "satstruct.h"
#include "modemssn.h"
#include "mtssn.h"

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
    s8      	    m_achAlias[VALIDLEN_ALIAS];     //�����ַ���
    TTransportAddr	m_tTransportAddr;               //�����ַ
public:
	TSimMtAlias( void ){ memset( this, 0, sizeof(TSimMtAlias) ); }
	BOOL32 IsNull( void ){ return (m_AliasType == 0); }
	void SetNull( void ){ memset(this, 0, sizeof(TSimMtAlias)); }
	const TSimMtAlias & operator= (const TMtAlias &tAlias)
    {
        m_AliasType = tAlias.m_AliasType;
        m_tTransportAddr = tAlias.m_tTransportAddr;

        memset(m_achAlias, 0, VALIDLEN_ALIAS);
        strncpy(m_achAlias, tAlias.m_achAlias, VALIDLEN_ALIAS - 1);
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
        
        return (*this);
    }    

    TMtAlias GetMtAlias(void)
    {
        TMtAlias tAlias;
        tAlias.m_AliasType = m_AliasType;
        tAlias.m_tTransportAddr = m_tTransportAddr;
        memset(tAlias.m_achAlias, 0, sizeof(tAlias.m_achAlias));
        strncpy(tAlias.m_achAlias, m_achAlias, VALIDLEN_ALIAS);
        tAlias.m_achAlias[MAXLEN_ALIAS-1] = '\0';
        return tAlias;
    }  
} ;

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
	TCapSupport       m_tCapSupport;                        //�ն�������
	TMtRecInfo        m_tMtRecInfo;                         //�ն�¼����Ϣ
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
	u8                m_byMTCapSpecByMCS;                   //�ն�ʹ�õ���������MCSָ��
    BOOL32            m_bMtSigned;                          //���ǻ��飺�ն��Ƿ����ǩ��
    BOOL32            m_bCurrUpLoad;                        //���ǻ��飺�ն��Ƿ������ϴ�����
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
    void SetPrsChnlAllStop()
    {
        SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlAllStart, FALSE); 
    }
    BOOL32 IsPrsChnlAnyStart() const
    {
        return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlAllStart);
    }

	void SetHDPrsChnlAllStop()
    {
        SETBITSTATUS(m_adwStatus[1], Mask_Status_HDPrsChnlAllStart, FALSE); 
    }
    BOOL32 IsHDPrsChnlAnyStart() const
    {
        return GETBITSTATUS(m_adwStatus[1], Mask_Status_HDPrsChnlAllStart);
    }

    void SetPrsChnl1Start(BOOL32 bStart) 
    { 
        SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnl1Start, bStart); 
    }
    BOOL32 IsPrsChnl1Start(void) const 
    {
        return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnl1Start);
    }
    void SetPrsChnl2Start(BOOL32 bStart) 
    { 
        SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnl2Start, bStart); 
    }
    BOOL32 IsPrsChnl2Start(void) const 
    {
        return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnl2Start);
    }
    void SetPrsChnlAudStart(BOOL32 bStart) 
    { 
        SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlAudStart, bStart); 
    }
    BOOL32 IsPrsChnlAudStart(void) const 
    {
        return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlAudStart);
    }
    void SetPrsChnlAudBasStart(BOOL32 bStart) 
    { 
        SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlAudBasStart, bStart); 
    }
    BOOL32 IsPrsChnlAudBasStart(void) const 
    {
        return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlAudBasStart);
    }
    void SetPrsChnlVidBasStart(BOOL32 bStart) 
    { 
        SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlVidBasStart, bStart); 
    }
    BOOL32 IsPrsChnlVidBasStart(void) const 
    {
        return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlVidBasStart);
    }
    void SetPrsChnlBrBasStart(BOOL32 bStart) 
    { 
        SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlBrBasStart, bStart); 
    }
    BOOL32 IsPrsChnlBrBasStart(void) const 
    {
        return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlBrBasStart);
    }
    // ��������Ƶ�����ش��������, zgc, 2008-08-13
    void PrsChnlHDVidBasStart(BOOL32 bStart) 
    { 
        SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlHDVidBasStart, bStart); 
    }
    BOOL32 IsPrsChnlHDVidBasStart(void) const 
    {
        return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlHDVidBasStart);
    }
	
	void SetPrsChnlVmpOut1Start(BOOL32 bStart) 
    { 
        SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlBrd1080Start, bStart); 
    }
    BOOL32 IsPrsChnlVmpOut1Start(void) const 
    {
        return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlBrd1080Start);
    }
	void SetPrsChnlVmpOut2Start(BOOL32 bStart) 
    { 
        SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlBrd720Start, bStart); 
    }
    BOOL32 IsPrsChnlVmpOut2Start(void) const 
    {
        return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlBrd720Start);
    }
	void SetPrsChnlVmpOut3Start(BOOL32 bStart) 
    { 
        SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlBrd4CifStart, bStart); 
    }
    BOOL32 IsPrsChnlVmpOut3Start(void) const 
    {
        return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlBrd4CifStart);
    }
	void SetPrsChnlVmpOut4Start(BOOL32 bStart) 
    { 
        SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlBrdCifStart, bStart); 
    }
    BOOL32 IsPrsChnlVmpOut4Start(void) const 
    {
        return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlBrdCifStart);
    }

	void SetPrsChnlDsBasStart(BOOL32 bStart) 
    { 
        SETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlDsBasStart, bStart); 
    }
    BOOL32 IsPrsChnlDsBasStart(void) const 
    {
        return GETBITSTATUS(m_adwStatus[1], Mask_Status_PrsChnlDsBasStart);
    }

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

    void Print() const
    {
        OspPrintf(TRUE, FALSE, "Conf Inner Status: \n");
        OspPrintf(TRUE, FALSE, "    IsPrsChnl1Start: %d\n", IsPrsChnl1Start());
        OspPrintf(TRUE, FALSE, "    IsPrsChnl2Start: %d\n", IsPrsChnl2Start());
        OspPrintf(TRUE, FALSE, "    IsPrsChnlAudStart: %d\n", IsPrsChnlAudStart());
        OspPrintf(TRUE, FALSE, "    IsPrsChnlAudBasStart: %d\n", IsPrsChnlAudBasStart());
        OspPrintf(TRUE, FALSE, "    IsPrsChnlVidBasStart: %d\n", IsPrsChnlVidBasStart());
        OspPrintf(TRUE, FALSE, "    IsPrsChnlBrBasStart: %d\n", IsPrsChnlBrBasStart());

        OspPrintf(TRUE, FALSE, "    IsPollSwitch: %d\n", IsPollSwitch());

        OspPrintf(TRUE, FALSE, "    IsDataConfOngoing: %d\n", IsDataConfOngoing());
        OspPrintf(TRUE, FALSE, "    IsDataConfRecreated: %d\n", IsDataConfRecreated());

        OspPrintf(TRUE, FALSE, "    IsLastSpeakerInMixing: %d\n", IsLastSpeakerInMixing());
        OspPrintf(TRUE, FALSE, "    IsNtfMtStatus2MMcu: %d\n", IsNtfMtStatus2MMcu());
        OspPrintf(TRUE, FALSE, "    IsStartVacOnMixReconnect: %d\n", IsStartVacOnMixReconnect());
        OspPrintf(TRUE, FALSE, "    IsSwitchDiscuss2Mix: %d\n", IsSwitchDiscuss2Mix());
        OspPrintf(TRUE, FALSE, "    IsInviteOnGkReged: %d\n", IsInviteOnGkReged());
        OspPrintf(TRUE, FALSE, "    IsRegGkNackNtf: %d\n", IsRegGkNackNtf());
        OspPrintf(TRUE, FALSE, "    IsVmpModuleUsed: %d\n", IsVmpModuleUsed());
        OspPrintf(TRUE, FALSE, "    IsVmpNotify: %d\n", IsVmpNotify());
        
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

protected:
    TMtExt2 m_atMtExt2[MAXNUM_CONF_MT];     //�ն���չ��Ϣ��(Ŀǰֻ��汾��Ϣ)

public:
	//construct
    TConfMtTable( void )
	{
		m_byMaxNumInUse = 0;
		memset( m_atMtExt, 0, sizeof( m_atMtExt ) );
		memset( m_atMtData, 0, sizeof( m_atMtData ) );	
	}

	//m_atMtExt
	void   SetMcuId( u8 byMtId, u8 byMcuId ); 
	u8     GetMcuId( u8 byMtId );
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
	void   ClearMtLogicChnnl( u8 byMtId );
	BOOL32 IsLogicChnnlOpen( u8 byMtId, u8 byChannel, BOOL32 bForwardChnnl );
	void   SetMtSrc( u8 byMtId, const TMt * ptSrc, u8 byMode );
	BOOL32 GetMtSrc( u8 byMtId, TMt * ptSrc, u8 byMode );
	void   SetMtStatus( u8 byMtId, const TMtStatus * ptStatus );
	BOOL32 GetMtStatus( u8 byMtId, TMtStatus * ptStatus );
	void   SetMtCapSupport( u8 byMtId, const TCapSupport * ptCapSupport );
	BOOL32 GetMtCapSupport( u8 byMtId, TCapSupport * ptCapSupport );
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
	void   SetMtRecording( u8 byMtId, const TEqp& tRecEqp, const u8& byRecChannel );
	void   SetMtNoRecording( u8 byMtId );
	void   SetMtRecPause( u8 byMtId );
	BOOL32 GetMtRecordInfo( u8 byMtId, TEqp* ptRecEqp, u8 * pbyRecChannel );
	u8     GetMtIdFromRecordInfo( const TEqp& tRecEqp, const u8& byRecChannel );
	BOOL32 GetMtRecState( u8 byMtId, TRecState * ptRecState ); 
	BOOL32 IsMtRecording( u8 byMtId );
	BOOL32 IsMtNoRecording( u8 byMtId );
	BOOL32 IsMtRecPause( u8 byMtId );
	BOOL32 IsMtRecSkipFrame( u8 byMtId ) const ;
	void   SetMtRecSkipFrame( u8 byMtId, BOOL32 bSkipFrame );
	void   SetMtRecProg( u8 byMtId, const TRecProg & tRecProg );
	BOOL32 GetMtRecProg( u8 byMtId, TRecProg * ptRecProg );

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
	
	//Mt send and receive
	void   SetMtVideoSend( u8 byMtId, BOOL32 bSending );
	BOOL32 IsMtVideoSending( u8 byMtId );
	void   SetMtAudioSend( u8 byMtId, BOOL32 bSending );
	BOOL32 IsMtAudioSending( u8 byMtId );
	void   SetMtVideoRecv( u8 byMtId, BOOL32 bRecving );
	BOOL32 IsMtVideoRecving( u8 byMtId );
	void   SetMtAudioRecv( u8 byMtId, BOOL32 bRecving );
	BOOL32 IsMtAudioRecv( u8 byMtId );
	void   SetMtVideo2Send( u8 byMtId, BOOL32 bSending );
	BOOL32 IsMtVideo2Sending( u8 byMtId );
	void   SetMtVideo2Recv( u8 byMtId, BOOL32 bSending );
	BOOL32 IsMtVideo2Recving( u8 byMtId );
	void   SetMtInMixing( u8 byMtId, BOOL32 bMtInMixing );
	BOOL32 IsMtInMixing( u8 byMtId );
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

    void    SetMtBRBeLowed( u8 byMtId, BOOL32 bLowed );
    BOOL32  GetMtBRBeLowed( u8 byMtId );

    void    SetMtTransE1( u8 byMtId, BOOL32 bTransE1 );
    BOOL32  GetMtTransE1( u8 byMtId );

    void    SetLowedRcvBandWidth( u8 byMtId, u16 wLowedBandWidth );
    u16     GetLowedRcvBandWidth( u8 byMtId );
	
	//���ǻ�����ش���
	void    SetRcvBandAdjusted( u8 byMtId, BOOL32 bAjusted );
	BOOL32  GetRcvBandAdjusted( u8 byMtId );

    void    SetMtSigned( u8 byMtId, BOOL32 bSigned );
    BOOL32  IsMtSigned( u8 byMtId );

    void    SetMtCurrUpLoad( u8 byMtId, BOOL32 bUpLoad );
    BOOL32  IsMtCurrUpLoad( u8 byMtId );
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
        Mask_VidLose    =   0x0100
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
        return tStatus;
    }

    void Print( void ) const
	{
		OspPrintf( TRUE, FALSE, "Mcu%dMt%d status:\n", GetMcuId(), GetMtId() ); 
        OspPrintf( TRUE, FALSE, "	m_wMtStatus     = 0x%x\n", m_wMtStatus );
		OspPrintf( TRUE, FALSE, "	m_byIsEnableFECC = %d\n", IsEnableFECC() );
		OspPrintf( TRUE, FALSE, "	IsSendAudio     = %d\n", IsSendAudio() );
        OspPrintf( TRUE, FALSE, "	IsReceiveAudio  = %d\n", IsReceiveAudio() );
		OspPrintf( TRUE, FALSE, "	IsSendVideo     = %d\n", IsSendVideo() );
        OspPrintf( TRUE, FALSE, "	IsReceiveVideo  = %d\n", IsReceiveVideo() );
        OspPrintf( TRUE, FALSE, "	IsSndVideo2     = %d\n", IsSndVideo2() );
        OspPrintf( TRUE, FALSE, "	IsRcvVideo2     = %d\n", IsRcvVideo2() );
		OspPrintf( TRUE, FALSE, "	IsInVideoLose   = %d\n", IsVideoLose() );
		OspPrintf( TRUE, FALSE, "	IsInMixing      = %d\n", IsInMixing() );
	    OspPrintf( TRUE, FALSE, "	m_byCurAudio    = %d\n", GetCurAudio() );
		OspPrintf( TRUE, FALSE, "	m_byCurVideo    = %d\n", GetCurVideo() );		
        OspPrintf( TRUE, FALSE, "	m_byBoardType   = %d\n", GetMtBoardType() );		
    }
    
};
//����Mc�ϵ��ն���Ϣ(len:17864)
struct TConfMcInfo
{
public:
	u8					m_byMcuId;                      //Mcu��
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
public:
	TConfMcInfo(){ SetNull(); }
	void SetNull() { memset(this, 0, sizeof(TConfMcInfo)); }
	BOOL32 IsNull( void ) { return m_byMcuId == 0 ? TRUE : FALSE; }
	
	TMcMtStatus *GetMtStatus(TMt& tMt)
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
	void SetIsLocked(BOOL32 bLock) { m_byIsLocked = GETBBYTE(bLock); }
	BOOL32 IsLocked() { return ISTRUE(m_byIsLocked); }
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
public:
	TConfMcInfo m_atConfOtherMcInfo[MAXNUM_SUB_MCU];  //����Mc�ϵ��ն���Ϣ����
public:
	TConfMcInfo* AddMcInfo(u8 byMcuId);
	BOOL32      RemoveMcInfo(u8 byMcuId);
	TConfMcInfo* GetMcInfo(u8 byMcuId);
    BOOL32      SetMtInMixing(TMt tMt);
    BOOL32      IsMtInMixing(TMt tMt);
    BOOL32      ClearMtInMixing(TMcu tMcu);
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
public:	
	TConfSwitchTable( void )
	{
		memset( m_tSwitchTable, 0, sizeof( m_tSwitchTable ) );
	}

    BOOL32 AddSwitchTable( u8 byMpId, TSwitchChannel *ptSwitchChannel );//���ӽ�����Ϣ��	
	BOOL32 RemoveSwitchTable( u8 byMpId, TSwitchChannel *ptSwitchChannel );//�Ƴ�������Ϣ��
    void ProcMultiToOneSwitch( TSwitchChannel *ptSwitchChannel, u8 byAct );//�����㵽һ��Ľ���
    void ProcBrdSwitch( u8 byNum, TSwitchChannel *ptSwitchChannel, u8 byAct, u8 byMpId );   //����㲥����
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
    BOOL32 IsValidBroadSwitchSrcIp( u32 dwSrcIp, u16 wRcvPort, u8 byMode );
    void ClearSwitchTable(u8 byMpId);
};

struct TTvWallInfo
{
    TEqp m_tTvWallEqp;
    TMt  m_atTvWallMember[MAXNUM_PERIEQP_CHNNL];

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
        if(byMtPos < MAXNUM_PERIEQP_CHNNL)
        { 
            m_atTvWallMember[byMtPos] = tMember; 
        }
    }

    void RemoveMtByMtIdx(u8 byMtIdx, u8& byTvId )
    {
        byTvId = 0;
        for(u8 byMtLp = 0; byMtLp < MAXNUM_PERIEQP_CHNNL; byMtLp++)
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
		for (u8 byIdx = 0; byIdx < MAXNUM_HDUBRD * MAXNUM_HDU_CHANNEL; byIdx++)
		{
			m_atHduChnlInfo[byIdx].SetNull();
		}
	}

	void   SetHduChnlNum(u8 byHduChnlNum)       { m_byHduChnlNum = byHduChnlNum; }
	u8     GetHduChnlNum()                      { return m_byHduChnlNum; }

	void   SetHduModuleInfo(u8 byHduChnlNum, const s8* pbyHduChnlInfo)
	{
		if (byHduChnlNum >= MAXNUM_HDUBRD * MAXNUM_HDU_CHANNEL)
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
    THduModChnlInfo   m_atHduChnlInfo[MAXNUM_HDUBRD * MAXNUM_HDU_CHANNEL];        //����ͨ����Ϣ
     
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//�����豸ģ��(len: 2502)
struct TConfEqpModule
{
public:
	TTvWallInfo m_tTvWallInfo[MAXNUM_PERIEQP_CHNNL];
    TMt  m_atVmpMember[MAXNUM_MPUSVMP_MEMBER];        //����ϳɳ�Ա
    
	TMultiTvWallModule m_tMultiTvWallModule;           //�洢����ǽģ��
	TVmpModule m_tVmpModule;                      //�洢����ϳ�ģ��
	THDTvWall  m_tHDTvWall;                       //�洢VCS�������ǽģ����Ϣ
	THDUModule m_tHduModule;                      //�洢HDU����ǽģ��          

public:
	TConfEqpModule( void )
	{
		memset( this, 0, sizeof(TConfEqpModule) );
	}//����
    
	void SetVmpMember( u8 byPos, TMt tMember ){ m_atVmpMember[byPos] = tMember; }
    BOOL32 SetTvWallModule(u8 byTvPos, TTvWallModule tTvWallModule){ return m_tMultiTvWallModule.SetTvModule(byTvPos, tTvWallModule); } 
    BOOL32 GetTvWallModule( u8 byTvId, TTvWallModule& tTvWallModule ) { return m_tMultiTvWallModule.GetTvModuleByTvId(byTvId, tTvWallModule); }
    void   SetVmpModule(TVmpModule& tVmpModule){ m_tVmpModule = tVmpModule; } 
    TVmpModule  GetVmpModule( void ) const { return m_tVmpModule; }       

    // ��������2������
    BOOL32 GetTvWallMemberByMt( u8 byTvId, TMt tMt, u8 &byPos, u8 &byMemberType) // �õ��ն���ָ������ǽ�е�λ��
    {
        for(u8 byTvPos = 0; byTvPos < MAXNUM_PERIEQP_CHNNL; byTvPos++)
        {
            if( m_tTvWallInfo[byTvPos].m_tTvWallEqp.GetEqpId() == byTvId )
            {
                for(u8 byMtLp = 0; byMtLp < MAXNUM_PERIEQP_CHNNL; byMtLp++)
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
		for( u8 byLoop=0; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++ )
		{
			if( m_atVmpMember[byLoop] == tMt )
			{                
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL32 IsMtAtVmpChannel( u8 byChl, TMt tMt, u8 &byMemberType ) // ���ӷ��س�Ա����
	{
		if( m_atVmpMember[byChl] == tMt )
		{
            byMemberType = m_tVmpModule.m_abyMemberType[byChl];
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
            for(u8 byMtPosLp = 0; byMtPosLp < MAXNUM_PERIEQP_CHNNL; byMtPosLp++)
            {
                if( m_tTvWallInfo[byTvPosLp].m_atTvWallMember[byMtPosLp] == tMt )
		        {
                    return TRUE;
		        }
            }
        }
        return FALSE;
	}

    BOOL32 IsMtInTvWallChannel( u8 byTvId, u8 byChannel, TMt tMt, u8 &byMemberType ) // ���ӷ��س�Ա����
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
    
    void SetTvWallMemberInTvInfo( u8 byTvId, u8 byMtPos, TMt tMember )
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
		memcpy(&m_tHDTvWall, ptHDTvWall, sizeof(THDTvWall));
	}
	void GetHDTvWall(THDTvWall& tHDTvWall) const { memcpy(&tHDTvWall, &m_tHDTvWall, sizeof(THDTvWall)); }

    
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
        OspPrintf(TRUE, FALSE, "TConfPollParam:\n");
        OspPrintf(TRUE, FALSE, "\t m_byPollMtNum: %d(Current.%d, Spec.%d)\n", m_byPollMtNum, m_byCurPollPos, m_bySpecPos);
        for (u8 byIdx = 0; byIdx < m_byPollMtNum; byIdx ++)
        {
            OspPrintf(TRUE, FALSE, "\t\t Idx.%d Mt: <%d,%d>\n", byIdx, m_atMtPollParam[byIdx].GetMcuId(), m_atMtPollParam[byIdx].GetMtId());
        }
    }
};
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


//����ϳ���ѯ����
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
	==============================================================================*/
	void SetCurPollBlokMtId(u8 byMemberId,u8 byMtId)
	{
		if( (byMemberId < MAXNUM_MPUSVMP_MEMBER ) 
			&& (0 < byMtId && byMtId <= MAXNUM_CONF_MT) )
		{
			m_abyCurPollBlokMtId[byMemberId] = byMtId;
		}
		else
		{
			OspPrintf(TRUE,FALSE,"Invalid member Id or mt Id.\n");
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
	==============================================================================*/
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
	==============================================================================*/
	u8 GetMtInCurPollBlok(u8 byMemberId)
	{
		return m_abyCurPollBlokMtId[byMemberId];
	}

};

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
        OspPrintf(TRUE, FALSE, "TTvWallPollParam:\n");
        OspPrintf(TRUE, FALSE, "\t m_tTvWall: %d-Chnl.%d\n", m_tTvWall.GetEqpId(), m_byTvWallChnnl);
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
        OspPrintf(TRUE, FALSE, "TTvWallPollParam:\n");
        OspPrintf(TRUE, FALSE, "\t m_tTvWall: %d-Chnl.%d Mode: %d\n", m_tTvWall.GetEqpId(), m_byTvWallChnnl, m_byMode);
        TConfPollParam::Print();
    }
}
;

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

	u8    GetEqpId( void ){ return m_byEqpId; }
    void  SetEqpId( u8 byEqpId ){ m_byEqpId = byEqpId; }

	u8    GetChnlIdx( void ){ return m_byChnlIdx; }
    void  SetChnlIdx( u8 byChnlIdx ){ m_byChnlIdx = byChnlIdx; }

	u8    GetPosition( void ){ return m_byPos; }
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
	u8    m_byCurrentMtId;      //  ��һ��������ѯʱ��ʼ���ն�ID
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
		m_byHduSchemeIdx = 0; 
		m_byCurrentMtId = 0;
		m_byCycles = 0;
		m_byCurrentCycle = 0; 
		m_byKeepTime = 0;		
		m_byChnlPollNum = 0;
	    m_byStatus = POLL_STATE_NONE;
        memset( this, 0x0, sizeof(THduBatchPollInfo) );		
	}

	u8   GetHduSchemeIdx( void ){ return m_byHduSchemeIdx; }
	void SetHduSchemeIdx( u8 byHduSchemeIdx ){ m_byHduSchemeIdx = byHduSchemeIdx; }

	u8   GetCurrentMtId( void ){ return m_byCurrentMtId; }
	void SetCurrentMtId( u8 byCurrentMtId ){ m_byCurrentMtId = byCurrentMtId; }

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
        OspPrintf(TRUE, FALSE, "Auto poll switch param:\n");
        OspPrintf(TRUE, FALSE, "  m_nTimeSpan: %d\n", m_nTimeSpan);
        OspPrintf(TRUE, FALSE, "  m_nLevel: %d\n", m_nLevel);
        OspPrintf(TRUE, FALSE, "  m_byCurPollPos: %d\n", m_byCurPollPos);
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
        OspPrintf(TRUE, FALSE, "TConfProtectInfo:\n");
        OspPrintf(TRUE, FALSE, "m_byLockMcs: %d\n", m_byLockMcs);
        OspPrintf(TRUE, FALSE, "m_byLockMcu: %d\n", m_byLockMcu);
        OspPrintf(TRUE, FALSE, "m_abMcsPwdPass: ");
        for (u8 byLoop = 0; byLoop < MAXNUM_MCU_MC; byLoop ++)
            OspPrintf(TRUE, FALSE, "%d:%s ", byLoop+1, ISTRUE(m_abMcsPwdPass[byLoop]) ? "Y" : "N" );
        OspPrintf(TRUE, FALSE, "\n");
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
#define USERINFOFILENAME          ( LPCSTR )"login.usr"
#define USERGRPINFOFILENAME       ( LPCSTR )"usrgrp.usr"
#define USERTASKINFOFILENAME      ( LPCSTR )"usrtask.dat"        //�洢��������ʻ��Ŀɵ��ȵĻ�����Ϣ

#define MCUADDRFILENAME         ( LPCSTR )"addrbook.kdv"

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
public:
    
    void EmptyAllTvMember(void)
    {
        for(u8 byTvLp = 0; byTvLp < MAXNUM_PERIEQP_CHNNL; byTvLp++)
        {
            m_tMultiTvWallModule.m_atTvWallModule[byTvLp].EmptyTvMember();
        }    
    }

    void EmptyAllVmpMember(void)
    {
        memset((void*)m_atVmpModule.m_abyVmpMember, 0, sizeof(m_atVmpModule.m_abyVmpMember));
    }

    void SetMtInTvChannel(u8 byTvId, u8 byChannel, u8 byMtIdx, u8 byMemberType)
    {
        for(u8 byTvLp = 0; byTvLp < MAXNUM_PERIEQP_CHNNL; byTvLp++)
        {
            if(m_tMultiTvWallModule.m_atTvWallModule[byTvLp].m_tTvWall.GetEqpId() == byTvId)
            {
                m_tMultiTvWallModule.m_atTvWallModule[byTvLp].SetTvChannel(byTvLp, byMtIdx, byMemberType);
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

public:
    TMcuPerfLimit()
    {
        m_wMaxConnMtNum = MAXNUM_MCU_MT;
		m_wMaxVCSAccessNum = MAXNUM_MCU_VC;

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
        OspPrintf(TRUE, FALSE, "Performance Limit: \n\n");

        OspPrintf(TRUE, FALSE, "\t Licensed Max Mt Num: %d\n", m_wMaxConnMtNum);
		OspPrintf(TRUE, FALSE, "\t m_wMaxVCSAccessNum: %d\n", m_wMaxVCSAccessNum);
        
		OspPrintf(TRUE, FALSE, "\t m_byIsMpcRunMtAdp: %d\n", m_byIsMpcRunMtAdp);
        OspPrintf(TRUE, FALSE, "\t m_byIsMpcRunMp: %d\n", m_byIsMpcRunMp);

        OspPrintf(TRUE, FALSE, "\t m_byIsRestrictFlux: %d\n", m_byIsRestrictFlux);
        OspPrintf(TRUE, FALSE, "\t m_wMpcMaxMpAbility: %d Mb\n", m_wMpcMaxMpAbility);
        OspPrintf(TRUE, FALSE, "\t m_wCriMaxMpAbility: %d Mb\n", m_wCriMaxMpAbility);
        
        OspPrintf(TRUE, FALSE, "\t m_byIsRestrictMtNum: %d\n", m_byIsRestrictMtNum);
        OspPrintf(TRUE, FALSE, "\t m_byMpcMaxMtAdpConnMtAbility: %d\n", m_byMpcMaxMtAdpConnMtAbility);
        OspPrintf(TRUE, FALSE, "\t m_byMpcMaxMtAdpConnSMcuAbility: %d\n", m_byMpcMaxMtAdpConnSMcuAbility);		
        OspPrintf(TRUE, FALSE, "\t m_byTuiMaxMtAdpConnMtAbility: %d\n", m_byTuiMaxMtAdpConnMtAbility);
		OspPrintf(TRUE, FALSE, "\t m_byTuiMaxMtAdpConnSMcuAbility: %d\n", m_byTuiMaxMtAdpConnSMcuAbility);
        OspPrintf(TRUE, FALSE, "\t m_byHdiMaxMtAdpConnMtAbility: %d\n", m_byHdiMaxMtAdpConnMtAbility);

        OspPrintf(TRUE, FALSE, "\t m_byMaxCasConfNum: %d\n", m_byMaxCasConfNum);
        OspPrintf(TRUE, FALSE, "\t m_byMaxConfSMcuNum: %d\n", m_byMaxConfSMcuNum);

    }
};

typedef struct TMcuDebugFile
{
	u16     m_wMcuListenPort;
	u16     m_wMcuTelnetPort;
	u32     m_dwMcsRefreshInterval;
	BOOL32  m_bWDEnable;                    //�Ƿ񼤻�Ź�	

	BOOL32  m_bMSDetermineType;             //vxworks��Ӳ��ƽ̨֧�������ù��ܣ���ֱ����os��ȡ�����ý����Ĭ�ϲ�֧��
    u16		m_wMsSynTime;					//����ͬ��ʱ��

    u8      m_byIsMMcuSpeaker;              //Ĭ��ָ���ϼ���������            
    u8      m_byIsAutoDetectMMcuDupCall;    //���ϼ�MCU���º���ʱ������Ƿ���ͬһ���ϼ�MCU
    u8      m_byCascadeAliasType;           //����ʱMCU�ڻ����б�������ʾ��ʽ
                                            //(0 - MCU Alias+Conf H.323ID,  1 - Mcu Alias Only, 2 - Conf H.323ID Only)
    u8      m_byShowMMcuMtList;             //����ʱ�Ƿ���ʾ�ϼ�MCU���ն��б�
    u16     m_wSMcuCasPort;                 //�ϼ����ú����¼��ļ����˿�

    u8      m_byBitrateScale;               //����ٷֱ�

	u8		m_byIsApplyChairToZxMcu;	    //�Ƿ�������mcu������ϯ
    u8      m_byIsTransmitMtShortMsg;       //�Ƿ�ת���ն˶���Ϣ
    u8      m_byIsChairDisplayMtApplyInfo;  //�Ƿ��ն�������ϯ�����˵���ʾ��Ϣ������ϯ�ն�
    u8      m_byIsSelInDoubleMediaConf;     //�Ƿ�����˫��ʽ����ѡ��

    TKdvTime m_tExpireDate;                 //mcu ʹ�ý�ֹ����

    u8      m_byIsNotLimitAccessByMtModal;  //�Ƿ�����ն��ͺţ���Ʒ���ͣ����ƽ�������	

	// MCU��������
    TMcuPerfLimit m_tPerfLimit;
    
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

	u8      m_byConfAdpManually;			//�ֹ��������


    TMcuDebugFile()
    {
        m_wMcuListenPort = MCU_LISTEN_PORT;
        m_wMcuTelnetPort = MCU_TELNET_PORT;
        m_dwMcsRefreshInterval = 3;
        m_bWDEnable = TRUE;                       

        m_bMSDetermineType = TRUE;
        m_wMsSynTime = 300; //Ĭ��ͬ��ʱ��
        
        m_byIsMMcuSpeaker = 0;
        m_byCascadeAliasType = 0;
        m_byShowMMcuMtList = 1;
        m_wSMcuCasPort = 3337;
        m_byIsAutoDetectMMcuDupCall = 0;

        m_byBitrateScale = 0;
        m_byIsApplyChairToZxMcu = 0;
        m_byIsTransmitMtShortMsg = 1;
        m_byIsChairDisplayMtApplyInfo = 1;
        m_byIsSelInDoubleMediaConf = 0;
        m_byIsNotLimitAccessByMtModal = 0; // Ĭ������
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

		m_byConfAdpManually = 0;
    }

    void Print() const
    {
        OspPrintf(TRUE, FALSE, "McuDebugFile Information:\n");
        OspPrintf(TRUE, FALSE, "m_wMcuListenPort: %d\n", m_wMcuListenPort);
        OspPrintf(TRUE, FALSE, "m_wMcuTelnetPort: %d\n", m_wMcuTelnetPort);
        OspPrintf(TRUE, FALSE, "m_dwMcsRefreshInterval: %d\n", m_dwMcsRefreshInterval);
        OspPrintf(TRUE, FALSE, "m_bWDEnable: %d\n", m_bWDEnable);

        OspPrintf(TRUE, FALSE, "m_bMSDetermineType: %d\n", m_bMSDetermineType);
		OspPrintf(TRUE, FALSE, "m_wMsSynTime: %d\n", m_wMsSynTime );        
        OspPrintf(TRUE, FALSE, "m_byIsMMcuSpeaker: %d\n", m_byIsMMcuSpeaker);        
        OspPrintf(TRUE, FALSE, "m_byIsAutoDetectMMcuDupCall: %d\n", m_byIsAutoDetectMMcuDupCall);
        OspPrintf(TRUE, FALSE, "m_byCascadeAliasType: %d\n", m_byCascadeAliasType);
        OspPrintf(TRUE, FALSE, "m_byShowMMcuMtList: %d\n", m_byShowMMcuMtList);        
        OspPrintf(TRUE, FALSE, "m_wSMcuCasPort: %d\n", m_wSMcuCasPort);
        
        OspPrintf(TRUE, FALSE, "m_byBitrateScale: %d\n", m_byBitrateScale);
        OspPrintf(TRUE, FALSE, "m_byIsApplyChairToZxMcu: %d\n", m_byIsApplyChairToZxMcu);
        OspPrintf(TRUE, FALSE, "m_byIsTransmitMtShortMsg: %d\n", m_byIsTransmitMtShortMsg);
        OspPrintf(TRUE, FALSE, "m_byIsChairDisplayMtApplyInfo: %d\n", m_byIsChairDisplayMtApplyInfo);
        OspPrintf(TRUE, FALSE, "m_byIsSelInDoubleMediaConf: %d\n", m_byIsSelInDoubleMediaConf);
        OspPrintf(TRUE, FALSE, "m_byIsNotLimitAccessByMtModal: %d\n", m_byIsNotLimitAccessByMtModal);
        OspPrintf(TRUE, FALSE, "m_byIsAllowVmpMemRepeated: %d\n", m_byIsAllowVmpMemRepeated);
		OspPrintf(TRUE, FALSE, "m_byIsSendFakeCap2Taide: %d\n", m_byIsSendFakeCap2Taide);
        OspPrintf(TRUE, FALSE, "m_byIsSendFakeCap2TaideHD: %d\n", m_byIsSendFakeCap2TaideHD);
        OspPrintf(TRUE, FALSE, "m_byIsSendFakeCap2Polycom: %d\n", m_byIsSendFakeCap2Polycom);
		OspPrintf(TRUE, FALSE, "m_byIsDistingtishSDHDMt: %d\n", m_byIsDistingtishSDHDMt);
		OspPrintf(TRUE, FALSE, "m_byIsVidAdjustless4Polycom: %d\n", m_byIsVidAdjustless4Polycom);
        OspPrintf(TRUE, FALSE, "m_byIsAdpResourceCompact.%d\n", m_byIsAdpResourceCompact);
        OspPrintf(TRUE, FALSE, "m_byIsSelAccord2Adp: %d\n", m_byIsSelAccord2Adp);
        OspPrintf(TRUE, FALSE, "m_byIsSVmpOutput1080i:%d\n", m_byIsSVmpOutput1080i);
        OspPrintf(TRUE, FALSE, "m_byIsSupportSecDSCap.%d\n", m_byIsSupportSecDSCap);
        OspPrintf(TRUE, FALSE, "m_byBandWidthReserved4HdBas.%d\n", m_byBandWidthReserved4HdBas);
        OspPrintf(TRUE, FALSE, "m_byBandWidthReserved4HdVmp.%d\n", m_byBandWidthReserved4HdVmp);

		OspPrintf(TRUE, FALSE, "m_byConfAdpManually.%d\n", m_byConfAdpManually);

        OspPrintf(TRUE, FALSE, "License Expired Date: %.4d-%.2d-%.2d\n\n", 
                  m_tExpireDate.GetYear(), m_tExpireDate.GetMonth(), m_tExpireDate.GetDay());

		m_tPerfLimit.Print();
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


//�ն��б�����ṹ len��3192
struct TMtListDiscard
{
public:
    TMtListDiscard(){   Clear(); }
    void Clear(void)
    {
        memset( &m_abyMixMcuId, 0, sizeof(m_abyMixMcuId));
        memset( &m_aabyMixMtId, 0, sizeof(m_aabyMixMtId));
    }

    void SetMtList(TMt *ptMtList, u16 wMtNum)
    {
        if ( NULL == ptMtList )
        {
            return;
        }
        if ( 0 == wMtNum || wMtNum > MAXNUM_SUB_MCU*MAXNUM_MIXING_MEMBER )
        {
            return;
        }
        for( u8 byIdx = 0; byIdx < wMtNum; byIdx ++)
        {
            SetNewMt(*ptMtList);
            ptMtList ++;
        }
    }

    void GetMtList( /*IN*/ u16 wBufLen, /*OUT*/ u8 &byGrpNum, /*OUT*/ u8 *pbyMtNum, /*OUT*/ TMt * ptMt)
    {
        if ( NULL == pbyMtNum || NULL == ptMt )
        {
            return;
        }
        u16 wMaxRetMtNum = wBufLen / sizeof(TMt);
        u16 wRetMtNum = 0;
        
        for( u8 byMcuIdx = 0; byMcuIdx < MAXNUM_SUB_MCU; byMcuIdx++)
        {
            if (0 != m_abyMixMcuId[byMcuIdx])
            {
                byGrpNum ++;

                for( u8 byMtIdx = 0; byMtIdx < MAXNUM_MIXING_MEMBER; byMtIdx++)
                {
                    if ( 0 != m_aabyMixMtId[byMcuIdx][byMtIdx] )
                    {
                        wRetMtNum ++;
                        if ( wRetMtNum > wMaxRetMtNum )
                        {
                            OspPrintf( TRUE, FALSE, "[TMtListDiscard]: [GetMtList] Cur space.%d is not enough for the left MTs, need to relocated\n", wBufLen );
                            break;
                        }

                        TMt tMt;
                        tMt.SetMtId(m_aabyMixMtId[byMcuIdx][byMtIdx]);
                        tMt.SetMcuId(m_abyMixMcuId[byMcuIdx]);
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
        }
    }
    
    void SetNewMt(TMt &tMt)
    {
        u8 byMcuIdx = 0;
        if (IsMcuMixing(tMt.GetMcuId(), byMcuIdx) &&
            IsMtMixing(byMcuIdx, tMt.GetMtId()))
        {
            return;
        }
        SetMtMixing(tMt);
        return;
    }

    BOOL32 IsMcuMixing(u8 byMcuId)
    {
        BOOL32 bMixing = FALSE;
        for(u8 byIdx = 0; byIdx <= MAXNUM_SUB_MCU; byIdx++)
        {
            if ( m_abyMixMcuId[byIdx] == byMcuId )
            {
                bMixing = TRUE;
                break;
            }
        }
        return bMixing;
    }
protected:
    void SetMtMixing(TMt &tMt)
    {
        u8 byMcuIdx = 0;
        if (IsMcuMixing(tMt.GetMcuId(), byMcuIdx))
        {
            SetMtMixing(tMt, byMcuIdx, FALSE);
        }
        else
        {
            u8 byIdleMcuIdx = 0; 
            if ( GetIdleMcuIdx(byIdleMcuIdx) )
            {
                SetMtMixing(tMt, byIdleMcuIdx, TRUE);
            }
            else
            {
                OspPrintf( TRUE, FALSE, "[TMtListDiscard][SetMtMixing] No idle mcu idx exist for mt<%d.%d>, impossible\n", tMt.GetMcuId(), tMt.GetMtId() );
            }
        }
    }

    void SetMtMixing(TMt &tMt, u8 byMcuIdx, BOOL32 bFst)
    {
        if ( !bFst )
        {
            u8 byNullMtIdx = MAXNUM_MIXING_MEMBER;
            
            for( u8 byMtIdx = 0; byMtIdx < MAXNUM_MIXING_MEMBER; byMtIdx++)
            {
                if ( 0 == m_aabyMixMtId[byMcuIdx][byMtIdx] )
                {
                    m_aabyMixMtId[byMcuIdx][byMtIdx] = tMt.GetMtId();
                    byNullMtIdx = byMtIdx;
                    break;
                }
            }
        }
        else
        {
            m_aabyMixMtId[byMcuIdx][0]= tMt.GetMtId();
            m_abyMixMcuId[byMcuIdx] = tMt.GetMcuId();
        }
        return;
    }

    BOOL32 IsMtMixing(u8 byMcuIdx, u8 byMtId)
    {
        BOOL32 bMtMixing = FALSE;
        for( u8 byMtIdx = 0; byMtIdx < MAXNUM_MIXING_MEMBER; byMtIdx ++)
        {
            if (m_aabyMixMtId[byMcuIdx][byMtIdx] == byMtId)
            {
                bMtMixing = TRUE;
                break;
            }            
        }
        return bMtMixing;
    }
    BOOL32 IsMcuMixing(u8 byMcuId, u8 &byMcuIdx)
    {
        byMcuIdx = MAXNUM_SUB_MCU;
        BOOL32 bMixing = FALSE;
        for(u8 byIdx = 0; byIdx <= MAXNUM_SUB_MCU; byIdx++)
        {
            if ( m_abyMixMcuId[byIdx] == byMcuId )
            {
                byMcuIdx = byIdx;
                bMixing = TRUE;
                break;
            }
        }
        return bMixing;
    }
    BOOL32 GetIdleMcuIdx(u8 &byMcuIdx)
    {
        BOOL32 bIdleExist = FALSE;
        byMcuIdx = MAXNUM_SUB_MCU;

        for(u8 byIdx = 0; byIdx <= MAXNUM_SUB_MCU; byIdx++)
        {
            if ( m_abyMixMcuId[byIdx] == 0 )
            {
                bIdleExist = TRUE;
                byMcuIdx = byIdx;
                break;
            }
        }
        return bIdleExist;        
    }

protected:
    u8 m_aabyMixMtId[MAXNUM_SUB_MCU][MAXNUM_MIXING_MEMBER];
    u8 m_abyMixMcuId[MAXNUM_SUB_MCU];
};


/* LBAND��70M֮���Ƶ��ת�� */
#define FREQ_70M                (u32)70   * 1000 * 1000
#define FREQ_LBAND              (u32)1273 * 1000 * 1000
#define FREQ_70M_MIN            (u32)50   * 1000 * 1000
#define FREQ_70M_MAX            (u32)90   * 1000 * 1000
#define FREQ_LBAND_MIN          (u32)950  * 1000 * 1000
#define FREQ_LBAND_MAX          (u32)1750 * 1000 * 1000

static u32 NMSConvertFreq(u32 freq)
{
    /* ��70M֮�䣬��ת��ΪLBAND */
    if ((freq >= FREQ_70M_MIN) && (freq <= FREQ_70M_MAX))
    {
        freq = FREQ_LBAND + freq - FREQ_70M;
    }
    /* ��LBAND֮�䣬��ת��Ϊ70M */
    else if ((freq >= FREQ_LBAND_MIN) && (freq <= FREQ_LBAND_MAX))
    {
        u32 diff = FREQ_70M - FREQ_70M_MIN;
        /* ���Ƶ�ʳ���70M��Ƶ�ʷ�Χ����ֱ�����߻������ */
        if (freq > (FREQ_LBAND + diff))
        {
            freq = FREQ_70M_MAX;
        } 
        else if (freq < (FREQ_LBAND - diff))
        {
            freq = FREQ_70M_MIN;
        } else {
            freq = FREQ_70M + freq - FREQ_LBAND;
        }
    } 
    else 
    {
        /* �Ƿ�Ƶ�� */
        freq = 0;
    }
    return freq;
}

//��ȡMCU Debug�ļ�key value
void McuGetDebugKeyValue(TMcuDebugVal &tTMcuDebugVal);
void McuGetSateliteConfigValue(TMcuSatInfo &tSatInfo);

//��ȡMCU Debug�ļ�����ָ���ն˵�Э���������Դ������ת������Դ������
BOOL32 McuGetMtCallInterfaceInfo( TMtCallInterface *ptMtCallInterface, u32 &dwEntryNum );

u8     GetMtAliasTypeFromString(s8* pszAliasString);
BOOL32 IsIPTypeFromString(s8* pszAliasString);
BOOL32 IsE164TypeFromString(s8 *pszSrc, s8* pszDst);
BOOL32 IsVidFormatHD(u8 byVidFormat);

BOOL32 IsHDConf( const TConfInfo &tConfInfo );
BOOL32 IsConfDualEqMV( const TConfInfo &tConfInfo );
BOOL32 IsConfDoubleDual( const TConfInfo &tConfInfo );

//�����ȡ����
BOOL32 CreateConfStoreFile(void);
BOOL32 AddConfToFile( TConfStore &tConfStore, BOOL32 bDefaultConf = FALSE );
BOOL32 DeleteConfFromFile( CConfId cConfId );
BOOL32 GetConfFromFile( u8 byIndex, TPackConfStore *ptPackConfStore );
u8     GetOneConfIndexFromFile( CConfId cConfId );
CConfId GetOneConfHeadFromFile( u8 byIndex );
BOOL32 SetOneConfHeadToFile( u8 byIndex, CConfId cConfId );
BOOL32 GetAllConfHeadFromFile( CConfId *pacConfId, u32 dwInBufLen );
BOOL32 SetAllConfHeadToFile( CConfId *pacConfId, u32 dwInBufLen );
BOOL32 GetConfDataFromFile( u8 byIndex, CConfId cConfId, u8 *pbyBuf, u32 dwInBufLen, u16 &wOutBufLen );
BOOL32 SetConfDataToFile( u8 byIndex, CConfId &cConfId, u8 *pbyBuf, u16 wInBufLen );
BOOL32 PackConfStore( TConfStore &tConfStore, TPackConfStore *ptPackConfStore, u16 &wPackConfDataLen );
BOOL32 UnPackConfStore( TPackConfStore *ptPackConfStore, TConfStore &tConfStore );
BOOL32 UpdateConfStoreModule( const TConfStore &tConfStore, 
                            u8 byOldIdx, u8 byNewIdx,
                            TMultiTvWallModule &tOutTvwallModule, 
                            TVmpModule &tOutVmpModule );   // guzh [7/5/2007] �����ն��������µ���ǽ�ͻ���ϳ�ģ��
BOOL32 SetUnProConfDataToFile(u8 byIndex, s8 *pbyBuf, u16 wInBufLen);
BOOL32 GetUnProConfDataToFile(u8 byIndex, s8 *pbyBuf, u16& wOutBufLen);

//��������
LPCSTR GetMediaStr( u8 byMediaType );
LPCSTR GetResStr( u8 byResolution );
LPCSTR GetManufactureStr( u8 byManufacture );
void AddEventStr();
void StartLocalEqp( void );
void StartLocalMp( void );
void StartLocalMtadp( void );
u8   GetVmpChlNumByStyle( u8 byVMPStyle );
u8	 GetVmpOutChnnlByRes(u8 byRes, u8 byVmpId, u8 byMediaType = MEDIA_TYPE_H264);
u8	 GetResByVmpOutChnnl(u8 &byMediaType, u8 byChnnl, const TConfInfo &tConfInfo, u8 byVmpId);

BOOL32 PackTMtAliasArray( TMtAlias *ptMtAlias, u16* pawMtDialRate, u8 byMtNum, char* const pszBuf, u16 &wBufLen );
BOOL32 UnPackTMtAliasArray(const char *pszBuf, u16 wBufLen, const TConfInfo *ptConfInfo, TMtAlias *ptMtAlias, u16* pawMtDialRate, u8 &byMtNum );
u16    GetAudioBitrate( u8 byAudioType );

void GetPrsTimeSpan( TPrsTimeSpan *ptPrsTimeSpan );

u8  *GetRandomKey();
void TableMemoryFree( s8 **ppMem, u32 dwEntryNum );

u8   GetActivePayload(const TConfInfo &tConfInfo, u8 byRealPayloadType);

BOOL32 IsDSResMatched(u8 bySrcRes, u8 byDstRes);
BOOL32 IsResPresentation(u8 byRes);
BOOL32 IsResLive(u8 byRes);
BOOL32 IsResGE(u8 bySrcRes, u8 byDstRes);
BOOL32 IsResG(u8 bySrcRes, u8 byDstRes);

u8     GetChnType(const TEqp &tEqp, u8 byChnId);

s8  *StrOfIP( u32 dwIP );
s8  *GetAddrBookPath(void);

const s8* GetMtLeftReasonStr(u8 byReason);

u8   GetMcuPdtType(void);  // ��ȡMCU��Ʒ(���)����
const s8* GetMcuVersion();  //��ȡMCU��ϵͳ�汾��
u8   GetResByWH( u16 wWidth, u16 wHeight );   // ���ݿ�߻�÷ֱ���
void GetWHByRes(u8 byRes, u16 &wWidth, u16 &wHeight); // ���ݷֱ��ʻ�ÿ��
s32  ResWHCmp(u8 bySrcRes, u8 byDstRes); // �ֱ��ʱȽ�

u8   GetSatMediaType( u8 byType );
u8   GetSatRes( u8 byRes );

u8  ReadMcuTopoTable( LPCSTR lpszProfileName, TMcuTopo atMcuTopo[], u16 wBufSize );
u16 ReadMtTopoTable( LPCSTR lpszProfileName, TMtTopo atMtTopo[], u16 wBufSize );


//mcu���Դ�ӡ�ӿ�
void DcsLog( s8 * pszFmt, ... );
void CallLog( s8 * pszFmt, ... );
void MMcuLog( s8 * pszFmt, ... );
void MtLog( s8 * pszFmt, ... );
void Mt2Log( s8 * pszFmt, ... );
void EqpLog( s8 * pszFmt, ... );
void McsLog( s8 * pszFmt, ... );
void VcsLog( s8 * pszFmt, ... );
void MpManagerLog( s8 * pszFmt, ... );
void CfgLog( s8 * pszFmt, ... );
void GuardLog( s8 * pszFmt, ... );
void NPlusLog( s8 * pszFmt, ... );
void SatLog( s8 * pszFmt, ... );

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
API void psatmsg( void );
API void npsatmsg( void );


extern BOOL32 g_bPrintEqpMsg;
extern BOOL32 g_bPrintCfgMsg;
extern BOOL32 g_bPrintMcsMsg;
extern BOOL32 g_bPrintNPlusMsg;
extern BOOL32 g_bPrintMtMsg;

#undef SETBITSTATUS
#undef GETBITSTATUS

#endif
