/*****************************************************************************
   ģ����      : mcu
   �ļ���      : mcudata.h
   ����ļ�    : mcudata.cpp
   �ļ�ʵ�ֹ���: MCU����ģ��Ӧ����ͷ�ļ�
   ����        : ����
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/05/29  0.9         ����          ����
   2006/01/11  4.0		   �ű���		 ���Ӽ���T120�ṹ
******************************************************************************/

#ifndef __MCUDATA_H_
#define __MCUDATA_H_

#include "osp.h"
#include "mcuconst.h"
#include "mcustruct.h"
#include "mcuutility.h"
#include "msmanagerssn.h"
#include "mpmanager.h"
#include "usermanage.h"
#include "vcsstruct.h"

//�������ݻ��������ն˵ȴ���ʱֵ
#define TIME_WAITFORJOIN			(u8)3		//DCS�ȴ��¼�����ĳ�ʱֵ
#define TIME_WAITFORINVITE			(u8)3		//DCS�ȴ��ϼ�����ĳ�ʱֵ

extern int	g_nInitUdpHandle;
extern u32  g_dwVCUInterval;

/************************************************************************/
/*                                                                      */
/*                      һ������һ����Ϣ����                            */
/*                                                                      */
/************************************************************************/

//1����������
struct TPeriEqpData
{
	BOOL32  m_bIsValid;                          //���豸�Ƿ�������������
	u8	    m_byFwdChannelNum;					 //MCU�������ŵ���
	TLogicalChannel	m_tFwdVideoChannel;		     //MCU��������ʼ��Ƶ�ŵ�
	TLogicalChannel	m_tFwdAudioChannel;		     //MCU��������ʼ��Ƶ�ŵ�
	u8	m_byRvsChannelNum;					     //������MCU�ŵ���
	TLogicalChannel	m_tRvsVideoChannel;		     //������MCU��ʼ��Ƶ�ŵ�
	TLogicalChannel	m_tRvsAudioChannel;		     //������MCU��ʼ��Ƶ�ŵ�
	TPeriEqpStatus	m_tPeriEqpStatus;		     //������״̬�������������ϱ�
	TMt m_atVidSrc[MAXNUM_PERIEQP_CHNNL];        //��ҪΪTW,VMP����
	TMt m_atAudSrc[MAXNUM_PERIEQP_CHNNL];        //��ҪΪTW,VMP����
	char m_achAliase[MAXLEN_EQP_ALIAS];          //�������
	TTransportAddr  m_tVideoRtcpDstAddr[MAXNUM_PERIEQP_CHNNL];   //�����ŵ���ƵRTCPĿ�ĵ�ַ
	TTransportAddr  m_tAudioRtcpDstAddr[MAXNUM_PERIEQP_CHNNL];   //�����ŵ���ƵRTCPĿ�ĵ�ַ    
};

//2���������
struct TMcData
{
	BOOL32	m_bConnected;					//����Ƿ�Ǽ�ΪFALSE��ʾδ�����Ǽ�
	u8		m_byFwdChannelNum;				//MCU������ŵ���
	TLogicalChannel	m_tFwdVideoChannel;		//MCU�������ʼ��Ƶ�ŵ�
	TLogicalChannel	m_tFwdAudioChannel;		//MCU�������ʼ��Ƶ�ŵ�
	TMt		m_atVidSrc[MAXNUM_MC_CHANNL];	//�û��ѡ����ƵԴ
	TMt		m_atAudSrc[MAXNUM_MC_CHANNL];	//�û��ѡ����ƵԴ
	TMcsRegInfo m_tMcsRegInfo;              //���ע����Ϣ
};

//3��MP����
struct TMpData
{
    BOOL32 m_bConnected;      //�Ƿ�����
    TMp  m_tMp;               //MP��Ϣ
	u8   m_abyMtId[MAX_CONFIDX][MAXNUM_CONF_MT];    //��������ն�id
    u16  m_wMtNum;            //mp�����ն���
    u16  m_wNetBandAllowed;   //�����������
    u16  m_wNetBandReal;      //�Ѿ�ռ�õ��������
    u32  m_dwPkNumAllowed;    //��MP�����Pk��(һ���ն˵�1kbps������ת��Ϊһ��Pk��������������ȡ��������) 02/09/2007-zbq
    u32  m_dwPkNumReal;       //�����򡣱�MP�Ѿ������Pk��
    u16  m_wNetBandReserved;  //Ԥ�����������Ϊĳ���裩
public:
    
    TMpData() : m_bConnected(FALSE),
                m_wMtNum(0),
                m_wNetBandAllowed(0),
                m_wNetBandReal(0),
                m_dwPkNumAllowed(0),
                m_dwPkNumReal(0),
                m_wNetBandReserved(0)
    {
        memset( &m_tMp, 0, sizeof(TMp) );
        memset( &m_abyMtId, 0, sizeof(m_abyMtId) );
    }

    BOOL32 IsThePkSupport(u32 dwAddPkNum) 
    {
        return dwAddPkNum + m_dwPkNumReal <= m_dwPkNumAllowed ? TRUE : FALSE;
    }
    // ��MP��ǰ�ķ�æָ����(Pk��æָ�� + ����ת����æָ��)/2
    u8  GetCurBusyPercent(void)
    {
        //����ת����æָ�� > 80%, ���ٸ���Pk�ĸ��سе�
        if ((m_wNetBandReal+m_wNetBandReserved) * 100/m_wNetBandAllowed >= 80)
        {
            return 100;
        }
        //Ԥ���������ʵ���� ��ͬ��Ϊ����ת���ĸ��ش���
        else
        {
            return (u8)((m_dwPkNumReal * 100)/m_dwPkNumAllowed + ((m_wNetBandReal+m_wNetBandReserved) * 100)/m_wNetBandAllowed)/2;
        }
    }
    void SetNull(void)
    {
        m_bConnected = FALSE;
        m_wMtNum = 0;
        m_wNetBandReal = 0;
        m_wNetBandAllowed = 0;
        m_dwPkNumReal = 0;
        m_dwPkNumAllowed = 0;
        m_wNetBandReserved = 0;
        memset( &m_tMp, 0, sizeof(TMp) );
        memset( &m_abyMtId, 0, sizeof(m_abyMtId) );
    }
};

//4��ռ��Ƕ����������Դ���ն���Ϣ// xliang [4/3/2009]  
struct TMtAdpHDChnnlInfo
{
public:
	u8 m_byHDMtId;		//HD �ն�ID
	u8 m_byConfIdx;		//����IDx
public:
	void SetNull() { m_byConfIdx = 0; m_byHDMtId = 0; }
	void SetConfIdx( u8 byConfIdx ) { m_byConfIdx = byConfIdx; }
	void SetHDMtId( u8 byHdMtId ) { m_byHDMtId = byHdMtId; } 
	u8	GetHDMtId() const { return m_byHDMtId; }
	u8	GetConfIdx() const { return m_byConfIdx; }
	BOOL   operator ==( const TMtAdpHDChnnlInfo & tObj ) const //�ж��Ƿ����
	{
		if( tObj.GetConfIdx() == GetConfIdx() 
			&& tObj.GetHDMtId() == GetHDMtId() 
			)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
};

//5��MtAdp����
struct TMtAdpData
{
	BOOL32 m_bConnected;                      //�Ƿ�����
	u8   m_byMtAdpId;                       //��Mtadp��
	u32  m_dwIpAddr;                        //Ip��ַ��������
	u8   m_byProtocolType;                  //�����Э������//H323��H320��SIP
	u8   m_byMaxMtNum;                      //�����֧������ն���
    u8   m_byMaxSMcuNum;                    //�����֧������¼�Mcu��
	u8   m_abyMtId[MAX_CONFIDX][MAXNUM_CONF_MT];           //��������ն�id
    u16  m_wMtNum;                          //mtadp�����ն���
	u8   m_byRegGKConfNum;                  //ע��GK�Ļ�����
    u16  m_wQ931Port;                       //������(�����������gkע��)    
    u16  m_wRasPort;                        //������
    u8   m_byIsSupportHD;                   //�Ƿ�֧��HD����
//	u8   m_abyHDMt[MAXHDLIMIT_MPC_MTADP];    //HD �ն�
	TMtAdpHDChnnlInfo m_atHdChnnlInfo[MAXHDLIMIT_MPC_MTADP];	//ռ��Ƕ����������Դ���ն���Ϣ
};

//6��Prsͨ������
struct TPrsChannel
{	
public:
	TPrsChannel()
	{
		memset(this, 0, sizeof(TPrsChannel));
		for(u8 byLp =0; byLp < MAXNUM_PRS_CHNNL; byLp++)
		{
			m_abyPrsChannels[byLp] = EQP_CHANNO_INVALID;
		}
	}

	u8  GetPrsId(void) const
	{
		return m_byEqpPrsId;
	}
	u8  GetChannelsNum(void) const
	{
		return m_byChannelNum;
	}
	void SetPrsId(u8 byPrsId)
	{
		m_byEqpPrsId = byPrsId;
	}

	void SetChannelNum(u8 byChlsNum )
	{
		m_byChannelNum = byChlsNum;
	}

	u8  m_abyPrsChannels[MAXNUM_PRS_CHNNL]; // ͨ����
private:	
	u8  m_byEqpPrsId;          // Prs Id
	u8  m_byChannelNum;        // ͨ����	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TUsedPrsChnlInfo 
{
public:
	TUsedPrsChnlInfo()
	{
		SetNull();
	}

	void SetNull() 
	{
		memset(this, 0, sizeof(TUsedPrsChnlInfo));
	}

	BOOL IsNull()
	{
		return (m_byPrsId < PRSID_MIN || m_byPrsId > PRSID_MAX); 
	}

	void Print()
	{
		OspPrintf(TRUE, FALSE, "m_byPrsId = %d, m_byPrsChnlId = %d, m_byPrsMode = %d\n", m_byPrsId, m_byPrsChnlId, m_byPrsMode);
		OspPrintf(TRUE, FALSE, "m_tPrsSrc = (srcid:%d, srctype:%d), m_byPrsSrcOutChnl = %d\n\n", 
			      m_tPrsSrc.GetMtId(), m_tPrsSrc.GetMtType(), m_byPrsSrcOutChnl);
	}

	u8 m_byPrsId;
	u8 m_byPrsChnlId;
	u8 m_byPrsMode;

	TMt m_tPrsSrc;
	u8  m_byPrsSrcOutChnl;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TPrsInfoManage
{
public:
	TPrsInfoManage()
	{
		SetNull();
	}
	
	void SetNull()
	{
		for (u8 byIdx = 0; byIdx <MAXNUM_PRS_CHNNL; byIdx++)
		{
			m_tPrsChnlInfo[byIdx].SetNull();
// 			m_byIsStart[byIdx] = FALSE;
		}
	}

	BOOL AddPrsChnl(u8 byPrsId, u8 byPrsChnlId, u8 byPrsMode, TMt tSrc, u8 bySrcOutChnl = 0)
	{
		for (u8 byChnlId = 0; byChnlId < MAXNUM_PRS_CHNNL; byChnlId++)
		{
			if (m_tPrsChnlInfo[byChnlId].IsNull())
			{
				m_tPrsChnlInfo[byChnlId].m_byPrsId = byPrsId;
				m_tPrsChnlInfo[byChnlId].m_byPrsChnlId = byPrsChnlId;
				m_tPrsChnlInfo[byChnlId].m_byPrsMode = byPrsMode;
				m_tPrsChnlInfo[byChnlId].m_tPrsSrc = tSrc;
				m_tPrsChnlInfo[byChnlId].m_byPrsSrcOutChnl = bySrcOutChnl;
				return TRUE;
			}
		}
		return FALSE;

	}

	void RemovePrsChnl(u8 byPrsId, u8 byPrsChnlId)
	{
		for (u8 byChnlId = 0; byChnlId < MAXNUM_PRS_CHNNL; byChnlId++)
		{
			if (m_tPrsChnlInfo[byChnlId].m_byPrsId == byPrsId &&
				m_tPrsChnlInfo[byChnlId].m_byPrsChnlId == byPrsChnlId)
			{
				m_tPrsChnlInfo[byChnlId].SetNull();
			}
		}
	}

	BOOL FindPrsChnlSrc(u8 byPrsId, u8 byPrsChnlId, u8& byPrsMode, TMt& tSrc, u8& bySrcOutChnl)
	{
		if (byPrsId < PRSID_MIN || byPrsId > PRSID_MAX)
		{
			return FALSE;
		}

		for (u8 byChnlId = 0; byChnlId < MAXNUM_PRS_CHNNL; byChnlId++)
		{
			if (m_tPrsChnlInfo[byChnlId].m_byPrsId == byPrsId &&
				m_tPrsChnlInfo[byChnlId].m_byPrsChnlId == byPrsChnlId)
			{
				byPrsMode    = m_tPrsChnlInfo[byChnlId].m_byPrsMode;
				tSrc         = m_tPrsChnlInfo[byChnlId].m_tPrsSrc;
				bySrcOutChnl = m_tPrsChnlInfo[byChnlId].m_byPrsSrcOutChnl;
				return TRUE;
			}
		}
		return	FALSE;
	}

	BOOL FindPrsForSrc(const TMt& tSrc, u8 bySrcOutChnl, u8& byPrsId, u8& byPrsChnlId)
	{
		for (u8 byChnlId = 0; byChnlId < MAXNUM_PRS_CHNNL; byChnlId++)
		{
			if (m_tPrsChnlInfo[byChnlId].m_tPrsSrc == tSrc &&
				m_tPrsChnlInfo[byChnlId].m_byPrsSrcOutChnl == bySrcOutChnl)
			{
				byPrsId     = m_tPrsChnlInfo[byChnlId].m_byPrsId;
				byPrsChnlId = m_tPrsChnlInfo[byChnlId].m_byPrsChnlId;
				return TRUE;
			}
		}
		return	FALSE;
	}

	
private:
// 	u8               m_byIsStart[MAXNUM_PRS_CHNNL];
	TUsedPrsChnlInfo m_tPrsChnlInfo[MAXNUM_PRS_CHNNL];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
/************************************************************************/
/*                                                                      */
/*                        ��������ģ����Ϣ����                          */
/*                                                                      */
/************************************************************************/

//����ģ����Ϣ
struct TTemplateInfo : public TConfStore
{
    u8   m_byConfIdx;           //����id

public:
    TTemplateInfo(void) { Clear(); }
    void Clear(void) { memset(this, 0, sizeof(TTemplateInfo)); }

    BOOL32 IsEmpty(void) { return (0 == m_byConfIdx); }
}
;

struct TConfMapData
{
protected:
    u8  m_byTemIndex;           //ģ����������
    u8  m_byInsId;              //��ʱ����ʵ��id   
    
public:
    TConfMapData(void) { Clear(); }
    void Clear(void) 
    {
        m_byTemIndex = MAXNUM_MCU_TEMPLATE;
        m_byInsId = 0;
    }

    void   SetInsId(u8 byInsId) { m_byInsId = byInsId; }
    u8     GetInsId(void)  { return m_byInsId; }
    void   SetTemIndex(u8 byIndex) { m_byTemIndex = byIndex; }
    u8     GetTemIndex(void) { return m_byTemIndex; }
    
    BOOL32 IsValidConf(void) { return (m_byInsId>=MIN_CONFIDX && m_byInsId <=MAXNUM_MCU_CONF); }//�Ƿ�Ϊռ�û���ʵ������Ч���� ��ʱ��ԤԼ
    BOOL32 IsTemUsed(void) { return (m_byTemIndex < MAXNUM_MCU_TEMPLATE); }
};


/************************************************************************/
/*                                                                      */
/*                       �������ݻ�����Ϣ����                           */
/*                                                                      */
/************************************************************************/

//1�����ݻ����������Ϣ
struct TDcsInfo
{	
public:
	TPeriDcsStatus m_tDcsStatus;	// DCS��ǰ��״̬
	BOOL32  m_bIsValid;				// DCS�Ƿ��������д���
	u32		m_dwDcsIp;				// DCS IP��ַ
    u16		m_wDcsPort;				// DCS�˿�
    u16		m_wReservedPortStart;   // Ԥ���˿ڷ�Χ��ʼ�˿ڣ�Ĭ��9000
    u16		m_wReservedPortRang;    // Ԥ���˿ڷ�Χ��С��Ĭ��100
    u8		m_byMaxConfCount;		// ����������Ĭ��32
    u8		m_byMaxMtCount;			// ����������ն�����Ĭ��64
    u8		m_byMaxDirectMtCount;   // ���ֱ���¼��ն�����Ĭ��32
    u8		m_byMaxHeight;			// ��߼�������Ĭ��16
	u8		m_byDcsId;				// DCS Id( MCU�� )
public:
	TDcsInfo(){  Clear(); }
	
	void Clear()
	{
		m_tDcsStatus.SetNull();
		m_bIsValid = FALSE;
		m_dwDcsIp  = 0;
		m_wDcsPort = 0;
		m_wReservedPortStart = 0;
		m_wReservedPortRang  = 0; 
		m_byMaxConfCount     = 0;
		m_byMaxMtCount       = 0;	
		m_byMaxDirectMtCount = 0;
		m_byMaxHeight        = 0;
		m_byDcsId            = 0;
	}
	BOOL32 IsNull()
	{
		if ( 0 == m_dwDcsIp || 0 == m_byDcsId )
		{
			return TRUE;
		}
		return FALSE;
	}
	void SetNull()
	{
        m_byDcsId = 0;
		m_dwDcsIp = 0;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//2�����ݻ�����Ϣ
struct TDcsConfInfo
{
protected:
	s8		m_achConfName[MAXLEN_CONFNAME + 1];	
	s8		m_achConfPwd[MAXLEN_PWD + 1];
	u16		m_wBandwidth;
	BOOL32	m_bSupportJoinedMt;
public:
	TDcsConfInfo() { Clear(); }

	void Clear()
	{
		memset( &m_achConfName, 0, sizeof(m_achConfName) );
		memset( &m_achConfPwd, 0 , sizeof(m_achConfPwd) );
		m_wBandwidth = 0;
		m_bSupportJoinedMt = FALSE;
	}
	void SetConfName( LPCSTR lpszConfName )
	{
		if( NULL != lpszConfName )
		{
			strncpy( m_achConfName, lpszConfName, sizeof( m_achConfName ) );
			m_achConfName[sizeof(m_achConfName) - 1] = '\0';
		}
		else
		{
			memset( m_achConfName, 0, sizeof( m_achConfName ) );
		}
	}	
	LPCSTR GetConfName() const { return m_achConfName; }
	
	void   SetConfPwd( LPCSTR lpszConfPwd )
	{ 
		if( lpszConfPwd != NULL )
		{
			strncpy( m_achConfPwd, lpszConfPwd, sizeof( m_achConfPwd ) );
			m_achConfPwd[sizeof(m_achConfPwd) - 1] = '\0';
		}
		else
		{
			memset( m_achConfPwd, 0, sizeof( m_achConfPwd ) );
		}
	}
	LPCSTR GetConfPwd() const { return m_achConfPwd; }

	BOOL32 IsSupportJoinMt()
	{
		if ( FALSE == m_bSupportJoinedMt )
		{
			return FALSE;
		}
		return TRUE;
	}
	void SetSupportJoinMt( BOOL32 bSupport )
	{
		m_bSupportJoinedMt = bSupport;
		return;
	}

	void SetBandwidth( u16 wBandwith ) { m_wBandwidth = wBandwith;	 }
	u16	 GetBandwidth() { return m_wBandwidth;	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//3�����ݻ����ն���Ϣ
struct TDcsMtInfo
{
	//���������ն˷�ʽ
	typedef enum 
	{
		emAddNone,			//none
		emBelowJoin,		//�ȴ��¼�����
		emInviteBelow,		//�����¼�����
		emJoinAbove,		//�����ϼ�
		emAboveInvite		//�ϼ��������
	}mtAddType;
	
	//���߻��������ն˵�����
	typedef enum
	{
		emT120None,			//none
		emT120Mt,			//T120Э���µ�MT
		emT120Mcu			//T120Э���µ�MCU
	}mtDataType;

public:
	u8  m_byMtId;
	u32 m_dwMtIp;
	u16 m_wPort;
	u16 m_wTimeOut;
	mtAddType  m_emJoinedType;
	mtDataType m_byDeviceType;	
public:
	TDcsMtInfo()
	{
		Clear();
	}
	void Clear()
	{
		m_byMtId    = 0;
		m_dwMtIp	= 0;
		m_wPort		= 0;
		m_wTimeOut	= 0;
		m_emJoinedType = emAddNone;
		m_byDeviceType = emT120None;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


/************************************************************************/
/*                                                                      */
/*                          �ġ��û���Ϣ����                            */
/*                                                                      */
/************************************************************************/

//1���û�����Ϣ
class CUsrGrpsInfo
{
public:
    CUsrGrpsInfo()
    {
        // m_nGrpNum = 0;
        // guzh [9/7/2006] ��������
        // m_nGrpNum = MAXNUM_USRGRP;
        SetGrpNum( MAXNUM_USRGRP );
    }
    ~CUsrGrpsInfo() {};

public:
    TUsrGrpInfo m_atInfo[MAXNUM_USRGRP];
protected:
    s32         m_nGrpNum;  // guzh [9/7/2006] ����Ա�Ѿ���Ч
                            // zhbq [12/29/2006] ����Ϊ������
public:
    
    // ����GroupId�����飬û�ҵ�����False
    BOOL32 GetGrpById(u8 byGrpId, TUsrGrpInfo &tGrpInfo) const;

    // �������ֲ����飬û�ҵ�����False
    BOOL32 GetGrpByName( const s8* szName,  TUsrGrpInfo &tGrpInfo ) const;

    // ����顣������Id���������� USRGRPID_INVALID
    u8 AddGrp ( const TUsrGrpInfo &tGrpInfo )  ;
    
    // ɾ������������������������Դ�ͷŲ����������Ƿ��ҵ�
    BOOL32 DelGrp ( u8 byGrpId );

    // �޸�������������Ƿ��ҵ�
    BOOL32 ChgGrp ( const TUsrGrpInfo &tGrpInfo ) ;

    // ���浽�ļ�
    BOOL32 Save() const;
    // ��ȡ
    BOOL32 Load();
    
    // ��ȡ�û������
    s32  GetGrpNum()
    {
        return ntohl(m_nGrpNum);
    }
    // �����û������ ֻ�ڹ����ʱ���õ�
    void SetGrpNum( s32 nGrpNum )
    {
        m_nGrpNum = htonl(nGrpNum);
        return;
    }

    // ��ӡ
    void Print() const;
};

//2���û�����Ŀɲ���������Ϣ
struct TUserTaskInfo
{
public:
	TUserTaskInfo()
	{
		SetNull();
	}
	
	void SetNull()
	{
		memset(m_achUserName, 0, sizeof(m_achUserName));
		memset(m_cTaskID, 0, sizeof(CConfId) * MAXNUM_MCU_VCCONF);
		m_wTaskNum = 0;
	}

	// ��ȡ�������û���
	const s8* GetUserName()      { return m_achUserName; }
	BOOL      SetUserName(s8* pchUserName)
	{
		if (NULL == pchUserName)
		{
			return FALSE;
		}

		memcpy(m_achUserName, pchUserName, MAX_CHARLENGTH);	
		return TRUE;
	}

	// ��ȡ������������Ϣ
	const CConfId* GetUserTaskInfo() const
	{
		return m_cTaskID;
	}
	u16 GetUserTaskNum() const
	{
		return m_wTaskNum;
	}

	BOOL SetUserTaskInfo(s8* pchUserName, u16 wTaskNum, const CConfId* pConfID)
	{
		if (pConfID == NULL || pchUserName == NULL || wTaskNum > MAXNUM_MCU_VCCONF)
		{
			return FALSE;
		}

		SetNull();
		memcpy(m_achUserName, pchUserName, MAX_CHARLENGTH);	
		m_wTaskNum = wTaskNum;
		memcpy(m_cTaskID, pConfID, sizeof(CConfId) * wTaskNum);
		return TRUE;
	}
	// �Ƿ����ָ��������byDelΪTrue��ͬʱɾ��ָ������
	BOOL IsYourTask(const CConfId& cConfId, BOOL byDel = FALSE)
	{
		BOOL byYourTask = FALSE;
		for(u16 wIdx = 0; wIdx < m_wTaskNum; wIdx++)
		{
			if (cConfId == m_cTaskID[wIdx])
			{
				byYourTask = TRUE;
			}
			if (byYourTask && byDel)
			{
				CConfId* pcConfId = m_cTaskID;
				memcpy(pcConfId + wIdx, pcConfId + wIdx + 1, sizeof(CConfId) * (m_wTaskNum - wIdx - 1) );
				m_wTaskNum--;
				return TRUE;
			}
		}
		return byYourTask;
	}

	void Print() const
	{
		OspPrintf(TRUE, FALSE, "UserName:%s\n", m_achUserName);
		OspPrintf(TRUE, FALSE, "TaskNum :%d\n", m_wTaskNum);
		for (u16 wIndex = 0; wIndex < m_wTaskNum; wIndex++)
		{
			OspPrintf(TRUE, FALSE, "Conf%d:\n", wIndex);
			m_cTaskID[wIndex].Print();
		}
	}

protected:
	s8         m_achUserName[MAX_CHARLENGTH];
	u16        m_wTaskNum;
	CConfId    m_cTaskID[MAXNUM_MCU_VCCONF];
};

//3���û���������
class CUsersTaskInfo
{
public:
    CUsersTaskInfo()
    {
        SetNULL();
    }
    ~CUsersTaskInfo() {};

	void SetNULL()
	{
		memset(m_abyUsed, 0, sizeof(TUserTaskInfo) * MAXNUM_VCSUSERNUM);
		memset(m_tUserTaskInfo, 0, sizeof(TUserTaskInfo) * MAXNUM_VCSUSERNUM);
	}

	// ��ӡ�ɾ�����޸��û������������Ϣ
	BOOL  AddUserTaskInfo(TUserTaskInfo& tUserTaskInfo);
	BOOL  DelUserTaskInfo(s8* pachUserName);
	BOOL  ChgUserTaskInfo(TUserTaskInfo& tUserTaskInfo);

	// ��ȡָ���û���������Ϣ
	BOOL  GetSpecUserTaskInfo(const s8* pachUserName, TUserTaskInfo& tUserTaskInfo);
	BOOL  GetSpecUserTaskInfo(u16 dwIdx, TUserTaskInfo& tUserTaskInfo);

	// �жϸ��û��Ƿ��в��������Ȩ��
	BOOL  IsYourTask(const s8* pachUserName, const CConfId& cConfId);
	// �������û��е�ָ������ɾ��
	void  DelSpecTaskInfo(CConfId cConfId, u16* const pdwChgIdx, u16& dwChgNum);


	// ���桢��ȡ�û������������Ϣ
	BOOL  SaveUsersTaskInfo() const;
	BOOL  LoadUsersTaskInfo();

protected:
	u8             m_abyUsed[MAXNUM_VCSUSERNUM];     // 0:��Ӧm_tUserTaskInfo��Ч 1:��Ч
	TUserTaskInfo  m_tUserTaskInfo[MAXNUM_VCSUSERNUM];

};


/************************************************************************/
/*                                                                      */
/*                          �塢VCS��������״̬                         */
/*                                                                      */
/************************************************************************/
class CVCCStatus : public CBasicVCCStatus
{
public:
	CVCCStatus()
	{
		VCCDefaultStatus();
	}


	const TMt& GetReqVCMT()           { return m_tReqVCMT; }
	void  SetReqVCMT(TMt& tMt)        { memcpy(&m_tReqVCMT, &tMt, sizeof(TMt)); }

	u16   GetCurUseTWChanInd()           { return m_wCurUseTWChanInd; }
	void  SetCurUseTWChanInd(u16 wIndex) { m_wCurUseTWChanInd = wIndex; }

	u16   GetCurUseVMPChanInd()           { return m_wCurUseVMPChanInd; }
	void  SetCurUseVMPChanInd(u16 wIndex) { m_wCurUseVMPChanInd = wIndex; }

	u8   GetCurSrcSsnId()                 { return m_byCurSrcSsnId; }
	void  SetCurSrcSsnId(u8 byIndex)      { m_byCurSrcSsnId = byIndex; }

	// ����ģʽ�л�ʱ������ǰ���ȵ�Srcssn������״̬������Ҫ�ָ�Ϊԭʼֵ
	void  VCCRestoreStatus()
	{
		RestoreStatus();
		m_wCurUseTWChanInd  = 0;
		m_wCurUseVMPChanInd = 1;
		m_tReqVCMT.SetNull();
	}

	// ���������ģʽ�л�
	void VCCRestoreGroupStatus()
	{
		RestoreGroupStatus();
		m_wCurUseTWChanInd  = 0;
		m_wCurUseVMPChanInd = 1;
		m_tReqVCMT.SetNull();		
	}

	// Ĭ��״̬
	void VCCDefaultStatus()
	{
		DefaultStatus();
		m_wCurUseTWChanInd  = 0;
		m_wCurUseVMPChanInd = 1;
		m_byCurSrcSsnId     = 0;
		m_tReqVCMT.SetNull();
	}

	void VCCPrint()
	{
		Print();
		OspPrintf(TRUE, FALSE, "m_tReqVCMT:mcuid��%d mtid��%d\n", m_tReqVCMT.GetMcuId(), m_tReqVCMT.GetMtId());
		OspPrintf(TRUE, FALSE, "m_wCurUseTWChanInd:%d\n", m_wCurUseTWChanInd);
		OspPrintf(TRUE, FALSE, "m_wCurUseVMPChanInd:%d\n", m_wCurUseVMPChanInd);
		OspPrintf(TRUE, FALSE, "m_byCurSrcSsnId:%d\n", m_byCurSrcSsnId);
	}

protected:
	u16 m_wCurUseTWChanInd;              // �Զ�ģʽ�£���ǰʹ�õĵ���ǽͨ��������
	u16 m_wCurUseVMPChanInd;             // �Զ�ģʽ�£���ǰ��ʹ�õĻ���ϳ�����ʼͨ����
	TMt m_tReqVCMT;                      // ��ǰ������ȵ��ն�
	u8 m_byCurSrcSsnId;                  // ��ǰ����������VCS��ʵ��ID��
};



/************************************************************************/
/*                                                                      */
/*                    ��������������Դ���� �ṹȺ                       */
/*                                                                      */
/************************************************************************/

//1��basͨ���ṹ�����ṹ������len:8��
struct TBasChn
{
public:
    TBasChn():m_byChnId(0),
              m_byType(0)
    {
        m_tEqp.SetNull();
    }

    BOOL32 SetEqp(const TEqp &tEqp)
    {
        if (tEqp.IsNull())
        {
            return FALSE;
        }
        m_tEqp = tEqp;

        return TRUE;
    }
    TEqp GetEqp()
    {
        TEqp tEqp;
        tEqp.SetNull();
        if (m_tEqp.IsNull())
        {
            return tEqp;
        }
        return m_tEqp;
    }

    u8 GetEqpId()
    {
        return m_tEqp.IsNull() ? 0 : m_tEqp.GetEqpId();
    }
    u8 GetChnId()
    {
        return m_tEqp.IsNull() ? 0xff : m_byChnId;
    }
    void SetChnId(u8 byChnId)
    {
        m_byChnId = byChnId;
        return;
    }
    void SetType(u8 byType) { m_byType = byType; }
    u8   GetType(void) const { return m_byType; }
    
    void SetConfIdx(u8 byConfIdx) { m_tEqp.SetConfIdx(byConfIdx); }

    BOOL32 IsNull()
    {
        return m_tEqp.IsNull();
    }

protected:
    TEqp m_tEqp;
    u8   m_byChnId;
    u8   m_byType;
};

//2��ѡ�����䣺��ý������������len��8��
struct TMediaCap
{
public:
    TMediaCap():m_byType(MEDIA_TYPE_NULL),
                m_byFormat(0),
                m_byFrameRate(0),
                m_wBitRate(0)
    {
    }


private:
    u16 m_wBitRate;
    u8  m_byType;
    u8  m_byFormat;
    u8  m_byFrameRate;
    u8  m_abyReserved[3];
};

//3��ѡ�����䣺��ͨ��������len��386��6��29��2345��
struct TSelChn
{
public:
private:
    TBasChn m_tChn;
    TMt     m_tSrc;
    TMt     m_atSDDst[MAXNUM_CONF_MT];
    TMt     m_atHDDst[MAXNUM_CONF_MT];
    u32     m_adwLastVCUTick;
    u8      m_bySelMode;        //VIDEO or SECVIDEO
    TMediaCap m_tSrcCap;
    TMediaCap m_tDstSDCap;
    TMediaCap m_tDstHDCap;
};

//4��ѡ�����䣺ͨ����������len��2345��192��450240��
struct TSelChnGrp
{
public:
    TSelChnGrp()
    {
        return;
    }
    BOOL32 IsChnExist(const TEqp &tEqp, u8 byChnId, u8 byType)
    {
        return TRUE;
    }

    BOOL32 GetChnStatus(const TEqp &tEqp, u8 byChnId, THDBasVidChnStatus &tChn);
    BOOL32 UpdateChnStatus(const TEqp &tEqp, u8 byChnId, THDBasVidChnStatus &tChn);

    u8     GetChnPos(const TEqp &tEqp, u8 byChnId, u8 byType)
    {
        return 0;
    }

private:
    TSelChn m_atSelChn[MAXNUM_CONF_MT];
};

//5���㲥���䣺�����飨len��36��
struct TMVChnGrp
{
public:
    TMVChnGrp();

    //ͨ�����ӡ���ȡ������
    BOOL32 AddChn(const TEqp &tEqp, u8 byChnId, u8 byType);
    BOOL32 GetChn(u8 &byNum, TBasChn *ptBasChn);
    BOOL32 GetChn(u8 byIdx, THDBasVidChnStatus &tChn);
    BOOL32 UpdateChn(u8 byIdx, THDBasVidChnStatus &tChn);

    //ͨ��VCUˢ�¹���
    u32    GetChnVcuTick(const TEqp &tEqp, u8 byChnIdx);
    BOOL32 SetChnVcuTick(const TEqp &tEqp, u8 byChnIdx, u32 dwCurTicks);

    //ͨ����Ȩ
    u8     GetChnPos(const TEqp &tEqp, u8 byChnIdx, u8 byType);
    BOOL32 IsChnExist(const TEqp &tEqp, u8 byChnIdx, u8 byType);

    //�������������
    BOOL32 GetBasResource(u8 byMediaType, u8 byRes, TEqp &tHDBas, u8 &byChnId, u8 &byOutIdx);

    void Clear(void);

private:
    TBasChn m_atChn[MAXNUM_CONF_MVCHN];
    u32 m_adwLastVCUTick[MAXNUM_CONF_MVCHN];

};

//6���㲥���䣺˫���飨len��36��
struct TDSChnGrp
{
public:
    TDSChnGrp();

    //ͨ�����ӡ���ȡ������
    BOOL32 AddChn(const TEqp &tEqp, u8 byChnId, u8 byType);
    BOOL32 GetChn(u8 &byNum, TBasChn *ptBasChn);
    BOOL32 GetChn(u8 byIdx, THDBasVidChnStatus &tChn);
    BOOL32 UpdateChn(u8 byIdx, THDBasVidChnStatus &tChn);

    //ͨ��VCUˢ�¹���
    u32    GetChnVcuTick(const TEqp &tEqp, u8 byChnIdx);
    BOOL32 SetChnVcuTick(const TEqp &tEqp, u8 byChnIdx, u32 dwCurTicks);

    //ͨ����Ȩ
    BOOL32 IsChnExist(const TEqp &tEqp, u8 byChnIdx, u8 byType);
    u8     GetChnPos(const TEqp &tEqp, u8 byChnIdx, u8 byType);

    //�������������
    BOOL32 GetBasResource(u8 byMediaType, u8 byRes, TEqp &tHDBas, u8 &byChnId, u8 &byOutIdx, BOOL32 bH263p = FALSE);

    BOOL32 IsGrpMpu(void);

    void Clear(void);

private:
    TBasChn m_atChn[MAXNUM_CONF_DSCHN];
    u32 m_adwLastVCUTick[MAXNUM_CONF_DSCHN];
};


//7����������ࣺ�㲥���䣫ѡ�����䣨len��4��36��36��450240��450240��900556��

class CBasMgr
{
    enum emChnType
    {
        emBegin,
            emNV    = 1,    //��ͨ����ͨ��
            emVGA   = 2,    //��̬˫������ͨ��
            emH263p = 3,    //H263+˫������ͨ��
        emEnd
    };

    enum emMauState
    {
        emBgn,
            emOnline    = 1,
            emReserved  = 2,
        emEd
    };

    enum emDualType
    {
        emBgin,
            emDualVGA   = 1,
            emDualEqMV  = 2,
            emDualDouble= 3,
        emEnd1
    };

    enum emAdpMode
    {
        emModeBg,
            emModeMau   = 1,
            emModeMpu   = 2,
        emModeEd
    };
public:
    CBasMgr();
    ~CBasMgr();

    /*************************************************/
    /*                                               */
    /*                 �㲥�������                  */
    /*                                               */
    /*************************************************/

    //�㲥���䣺������Դռ��
    BOOL32 OcuppyHdBasChn(const TConfInfo &tConfInfo);

    //�㲥���䣺������Դ�ͷ�
    void   ReleaseHdBasChn(void);

    //�㲥���䣺��Դʣ��Ԥ��
    BOOL32 IsHdBasSufficient(const TConfInfo &tConfInfo);

    //�㲥���䣺��������Ԥ��
    void   GetNeededMau(const TConfInfo &tConfInfo, u8 &byNVChn, u8 &byH263Chn, u8 &byVGAChn);
    void   GetNeededMpu(const TConfInfo &tConfInfo, u8 &byNVChn, u8 &byDSChn);

    //�㲥���䣺��ȡ��������ͨ��������ģʽ
    void   GetChnGrp(u8 &byNum, TBasChn *ptBasChn, u8 byAdpMode);
    u8     GetChnMode(const TEqp &TEqp, u8 byChnId);

    //�㲥���䣺��ȡ�������������״��
    void   GetHdBasStatus(TMcuHdBasStatus &tMauStatus, TConfInfo &tCurConf);

    //�㲥���䣺������������֧��
    BOOL32 GetLowBREqp(const TConfInfo &tConfInfo, TEqp &tEqp, u8 &byChnId);

    //�㲥���䣺��ȡĳȺ����������
    BOOL32 GetBasResource(u8    byMediaType,
                          u8    byRes,
                          TEqp &tHDBas,
                          u8   &byChnId,
                          u8   &byOutIdx,
                          BOOL32 bDual = FALSE, BOOL32 bH263p = FALSE);

    //�㲥���䣺���ӡ���ȡ��ˢ��ͨ��
    void   AddSelChn(const TEqp &tEqp, u8 byChnId, u8 byChnType, BOOL32 bDual = FALSE);
    BOOL32 GetSelChn(const TEqp &tEqp, u8 byChnId, THDBasVidChnStatus &tStatus);
    BOOL32 UpdateSelChn(const TEqp &tEqp, u8 byChnId, THDBasVidChnStatus &tStatus);

    //�㲥���䣺����ͨ������
    void   AssignBasChn(const TConfInfo &tConfInfo, u8 byMode);

    /*************************************************/
    /*                                               */
    /*                 ѡ���������                  */
    /*                                               */
    /*************************************************/


    /*************************************************/
    /*                                               */
    /*                   ��������                    */
    /*                                               */
    /*************************************************/
    //���졢��ӡ
    void Clear(void);
    void Print(void);
    
    //��ȡͨ��������ȫ����Ψһ��
    u8     GetChnId(const TEqp &tEqp, u8 byChIdx);

    //��ȡ������ͨ��״̬
    BOOL32 GetChnStatus(const TEqp &tEqp, u8 byChnId, THDBasVidChnStatus &tStatus);
    BOOL32 UpdateChn(const TEqp &tEqp, u8 byChnId, THDBasVidChnStatus &tMVStatus);
    
    //����ͨ��Tick����ȡ��ˢ�£�����VCU
    BOOL32 SetChnVcuTick(const TEqp &tEqp, u8 byChnIdx, u32 dwCurTicks);
    u32    GetChnVcuTick(const TEqp &tEqp, u8 byChnIdx);

    //��ȡ������Դ����
    BOOL32 IsBrdGrpMpu(void);

private:

    //�㲥���䣺������Դռ��
    BOOL32 OcuppyMau(const TConfInfo &tConfInfo);
    BOOL32 OcuppyMpu(const TConfInfo &tConfInfo);

    //�㲥���䣺��Դʣ��Ԥ��
    BOOL32 IsMauSufficient(const TConfInfo &tConfInfo);
    BOOL32 IsMpuSufficient(const TConfInfo &tConfInfo);

    //�㲥���䣺����ͨ������
    void   AssignMVBasChn(const TConfInfo &tConfInfo);
    void   AssignDSBasChn(const TConfInfo &tConfInfo);

    //�㲥���䣺����ͨ��
    void   AddBrdChn(const TEqp &tEqp, u8 byChnId, u8 byChnType, BOOL32 bDual = FALSE);
    
    //�㲥���䣺˫��ģʽ�ӿ�
    void   SetDualVGA(BOOL32 bVGA = TRUE) { m_emType = bVGA ? emDualVGA : m_emType;   }
    BOOL32 IsDualVGA(void) const { return (m_emType == emDualVGA);   }
    void   SetDualDouble(BOOL32 bDouble = TRUE) { m_emType = bDouble ? emDualDouble : m_emType;   }
    BOOL32 IsDualDouble(void) const { return (m_emType == emDualDouble);    }
    void   SetDualEqMv(BOOL32 bEq = TRUE) { m_emType = bEq ? emDualEqMV : m_emType;    }
    BOOL32 IsDualEqMv(void) const { return (m_emType == emDualEqMV); }
    
    //��ȡ����Դ�Ķ�̬�غ�
    u8     GetActivePL(const TConfInfo &tConfInfo, u8 byMediaType);

protected:

private:
    emDualType  m_emType;           //����˫������
    TMVChnGrp   m_tMVBrdChn;        //���������е������㲥����ͨ��
    TDSChnGrp   m_tDSBrdChn;        //���������е�˫���㲥����ͨ��
    TSelChnGrp  m_tMVSelChn;        //���������е�����ѡ������ͨ��
    TSelChnGrp  m_tDSSelChn;        //���������е�˫��ѡ������ͨ��
};

//ѡ������֧��(���ɽṹ)
struct TSelChnTmp
{
public:
    
    u8   GetChnIdx(void) const { return m_byChnIdx; }
    void SetChnIdx(u8 byChnId) { m_byChnIdx = byChnId; }

    void SetBas(const TEqp &tEqp) { m_tBas = tEqp;    }
    TEqp GetBas(void) {  return m_tBas;   }

	void SetSelMode(u8 bySelMode) { m_bySelMode = bySelMode; }
	u8   GetSelMode()             { return m_bySelMode; }

    u8   GetSrcId(void) { return m_bySrcId; }
    void SetSrcId(u8 byId) { m_bySrcId = byId; };
    
    void GetDstId(u8 *pbyDstId, u8 &byLen)
    {
        byLen = min(byLen, GetDstNum());
        memcpy(pbyDstId, &m_abyDstId, byLen);
        return;
    }
    BOOL32 SetDstId(u8 byDstId)
    {
        u8 byFstIdle = 0xff;
        BOOL32 bExist = FALSE;

        for (u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
        {
            if (0 == byIdx)
            {
                byFstIdle = byIdx;
            }
            if (byDstId == m_abyDstId[byIdx])
            {
                return TRUE;
            }
        }

        if (byFstIdle != 0xff)
        {
            m_abyDstId[byFstIdle] = byDstId;
            return TRUE;
        }
        OspPrintf(TRUE, FALSE, "[TSelChn::SetDstId] no pos for dst.%d!\n", byDstId);
        return FALSE;
    }

    BOOL32 RemoveDstId(u8 byDstId)
    {
        u8 byIdx = 0;
        u8 byExistPos = 0xff;
        for (byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
        {
            if (byDstId == m_abyDstId[byIdx])
            {
                byExistPos = byIdx;
                break;
            }
        }
        if (0xff != byExistPos)
        {
            //���Ŀ��
            m_abyDstId[byExistPos] = 0;

            //��������
            for (byIdx = byExistPos+1; byIdx < MAXNUM_CONF_MT; byIdx++)
            {
                if (m_abyDstId[byIdx] == 0)
                {
                    break;
                }
                m_abyDstId[byIdx-1] = m_abyDstId[byIdx];
            }

            //�Ƿ������
            if (0 == GetDstNum())
            {
                SetNull();
            }
			return TRUE;
        }
        return  FALSE;

    }
    BOOL32 IsDstExist(u8 byDstId)
    {
        BOOL32 bExist = FALSE;
        
        for (u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
        {
            if (byDstId == m_abyDstId[byIdx])
            {
                bExist = TRUE;
                break;
            }
        }
        return bExist;
    }

    void SetNull()
    {
        m_tBas.SetNull();
        m_byChnIdx = 0;
        m_bySrcId = 0;
		m_bySelMode = MODE_NONE;
        memset(&m_abyDstId, 0, sizeof(m_abyDstId));
    }

    BOOL32 IsNull()
    {
        return m_tBas.IsNull() && m_bySrcId != 0 && GetDstNum() != 0;
    }

    void Print()
    {
        OspPrintf(TRUE, FALSE, "\tBas:\t%d\n", m_tBas.GetEqpId());
        OspPrintf(TRUE, FALSE, "\tChn:\t%d\n", m_byChnIdx);
        OspPrintf(TRUE, FALSE, "\tSrc:\n");
        OspPrintf(TRUE, FALSE, "\t\tMt.%d\n", m_bySrcId);
        OspPrintf(TRUE, FALSE, "\tDst(s):\n");
        for (u8 byIdx = 0; byIdx < GetDstNum(); byIdx++)
        {
            OspPrintf(TRUE, FALSE, "\t\tMt.%d\n", m_abyDstId[byIdx]);
        }
		OspPrintf(TRUE, FALSE, "\tSelMode:\t%d\n", m_bySelMode);
    }

    u8 GetDstNum(void)
    {
        u8 byLen = 0;
        for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
        {
            if (0 == m_abyDstId[byIdx])
            {
                break;
            }
            byLen++;
        }
        return byLen;
    }

private:
    u8 m_bySrcId;
    u8 m_abyDstId[MAXNUM_CONF_MT];
    TEqp m_tBas;
    u8 m_byChnIdx;
	u8 m_bySelMode;
};

class CSelChnGrp
{
public:
    CSelChnGrp()
    {
        Clear();
    }
    BOOL32 AddSel(u8 bySrcId, u8 byDstId, u8 bySelMode, TEqp &tEqp, u8 byChnIdx)
    {
        u8 byFstIdle = 0xff;
        u8 byExistChn = 0xff;

        for(u8 byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx++)
        {
            if (m_atSelChn[byIdx].GetSrcId() == 0 &&
				0xff == byFstIdle)
            {
                byFstIdle = byIdx;
            }
            //���汾��֧��ͬԴ�Ҷ�BAS�����, 4.6.1����֧��
            if (m_atSelChn[byIdx].GetSrcId() == bySrcId &&
				m_atSelChn[byIdx].GetSelMode())
            {
                byExistChn = byIdx;
                break;
            }
        }

        //�����ڣ�������λ��
        if (0xff == byExistChn)
        {
            m_atSelChn[byFstIdle].SetBas(tEqp);
            m_atSelChn[byFstIdle].SetSrcId(bySrcId);
            m_atSelChn[byFstIdle].SetDstId(byDstId);
            m_atSelChn[byFstIdle].SetChnIdx(byChnIdx);
			m_atSelChn[byFstIdle].SetSelMode(bySelMode);
            return TRUE;
        }
        //���ڣ�����Ŀ�Ķ�
        else
        {
            m_atSelChn[byExistChn].SetDstId(byDstId);
            return TRUE;
        }
        return FALSE;
    }

    BOOL32 GetSelBasChn(u8 bySrcId, u8 byDstId, u8 bySelMode, TEqp &tEqp, u8 &byChnIdx, BOOL32 &bRlsChn)
    {
        for(u8 byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx++)
        {
            if (m_atSelChn[byIdx].GetSrcId() == bySrcId &&
                m_atSelChn[byIdx].IsDstExist(byDstId) &&
				m_atSelChn[byIdx].GetSelMode() == bySelMode)
            {
                tEqp = m_atSelChn[byIdx].GetBas();
                byChnIdx = m_atSelChn[byIdx].GetChnIdx();
                bRlsChn = m_atSelChn[byIdx].GetDstNum() == 1;   //ֻʣ��ǰ���һ��Ŀ�ģ��ⲿ�����ͷ�ͨ��
                return TRUE;
            }
        }
        return FALSE;
    }

    BOOL32 RemoveSel(u8 bySrcId, u8 byDstId, u8 bySelMode)
    {
        for(u8 byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx++)
        {
            if (m_atSelChn[byIdx].GetSrcId() == bySrcId &&
                m_atSelChn[byIdx].IsDstExist(byDstId) &&
				bySelMode == m_atSelChn[byIdx].GetSelMode())
            {
                return m_atSelChn[byIdx].RemoveDstId(byDstId);
            }
        }
        return FALSE;
    }

    BOOL32 GetSelSrc(const TEqp &tEqp, u8 byChnIdx, u8 &bySrcId)
    {
        for(u8 byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx++)
        {
            if (m_atSelChn[byIdx].GetBas() == tEqp &&
                m_atSelChn[byIdx].GetChnIdx() == byChnIdx)
            {
                bySrcId = m_atSelChn[byIdx].GetSrcId();
                return TRUE;
            }
        }
        return FALSE;
    }

    BOOL32 GetSelDst(const TEqp &tEqp, u8 byChnIdx, u8 *pbyDstId, u8 &byNum)
    {
        for(u8 byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx++)
        {
            if (m_atSelChn[byIdx].GetBas() == tEqp &&
                m_atSelChn[byIdx].GetChnIdx() == byChnIdx)
            {
                m_atSelChn[byIdx].GetDstId(pbyDstId, byNum);
                return TRUE;
            }
        }
        return FALSE;
    }

	BOOL32 FindSelSrc(u8 bySelSrcId, u8 bySelMode, u8& byAdpChnNum, u8* pbyEqpId, u8* pbyChnIdx)
	{
		byAdpChnNum = 0;
        for(u8 byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx++)
        {
			if (bySelSrcId == m_atSelChn[byIdx].GetSrcId() &&
				bySelMode == m_atSelChn[byIdx].GetSelMode())
			{
				pbyEqpId[byAdpChnNum]  = m_atSelChn[byIdx].GetBas().GetEqpId();
				pbyChnIdx[byAdpChnNum] = m_atSelChn[byIdx].GetChnIdx();
				byAdpChnNum++;
			}
		}
		if (byAdpChnNum != 0)
		{
			return TRUE;
		}
		
		return FALSE;
	}

	BOOL32 IsMtInSelAdpGroup(u8 bySelSrcId, u8 bySelDstId, TEqp* ptEqp = NULL, u8* pbyChnlIdx = NULL)
	{
		for(u8 byGroupId = 0; byGroupId < MAXNUM_PERIEQP; byGroupId++)
		{
			if (bySelSrcId == m_atSelChn[byGroupId].GetSrcId())
			{
				u8 abyDstId[MAXNUM_CONF_MT];
				u8 byDstNum = MAXNUM_CONF_MT;
				m_atSelChn[byGroupId].GetDstId(abyDstId, byDstNum);
				for (u8 byDstIdx = 0; byDstIdx < byDstNum; byDstIdx++)
				{
					if (bySelDstId == abyDstId[byDstIdx])
					{
						if(ptEqp != NULL)
						{
							*ptEqp = m_atSelChn[byGroupId].GetBas();
						}
						if (pbyChnlIdx != NULL)
						{
							*pbyChnlIdx = m_atSelChn[byGroupId].GetChnIdx();
						}	
						return TRUE;
					}
				}				
			}
		}
		return FALSE;
	}
	
	void GetUsedAdptChnlInfo(u8& byChnlNum, u8* abyBasId, u8* abyChnlIdx)
	{
		byChnlNum = 0;
		if (NULL == abyBasId || NULL == abyChnlIdx)
		{
			OspPrintf(TRUE, FALSE, "GetUsedAdptChnlInfo with wrong param\n");
			return;
		}
		for (u8 byChnIdx = 0; byChnIdx < MAXNUM_PERIEQP; byChnIdx++)
		{
			if (!m_atSelChn[byChnIdx].GetBas().IsNull())
			{
				abyBasId[byChnlNum]   = m_atSelChn[byChnIdx].GetBas().GetEqpId();
				abyChnlIdx[byChnlNum] = m_atSelChn[byChnIdx].GetChnIdx();
				byChnlNum++;
			}
		}
		return;
	}

    void Clear() {  memset(this, 0, sizeof(m_atSelChn)); }

    void Print()
    {
        OspPrintf(TRUE, FALSE, "\nSelGrp as follow:\n\n");

        for(u8 byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx++)
        {
            if (!m_atSelChn[byIdx].IsNull())
            {
                OspPrintf(TRUE, FALSE, "-----------------------------");
                OspPrintf(TRUE, FALSE, "No[%d]:\n", byIdx);
                m_atSelChn[byIdx].Print();
            }
        }
    }

private:
    TSelChnTmp m_atSelChn[MAXNUM_PERIEQP];
};



/************************************************************************/
/*                                                                      */
/*                    �ߡ�����Ŀ��Ⱥ����� �ṹȺ                       */
/*                                                                      */
/************************************************************************/

//1����������Ⱥ�飨len��24*8��192��FIXME: δ��������Ϊ��264

struct TMVRcvGrp 
{
    enum emType
    {
        emBegin     = 0,
            em1080  = 1,
            em720p  = 2,
            em4Cif  = 3,
            emCif   = 4,
            emH263  = 5,
            emMp4   = 6,
        emEnd
    };
public:
    
    //����
    TMVRcvGrp() {   Clear();    }

    //��Ա����ɾ
    void AddMem(u8 byMtId, emType type, BOOL32 bExceptEither = FALSE);
    void RemoveMem(u8 byMtId, emType type);
    
    //�������Ա����ɾ
    void AddExcept(u8 byMtId);
	void ClearExpt( void );

    //Ⱥ���Ա��ȡ
    void GetMtList(u8 byRes, u8 &byNum, u8 *pbyMt, BOOL32 bForce = FALSE);

    //����
    void ResIn2Out(u8 byRes, u8 &byConfRes, u8 &byConfType);
    BOOL32 GetMtMediaRes(u8 byMtId, u8 &byMediaType, u8 &byRes);
    BOOL32 IsNeedAdp(void);
    BOOL32 IsMtInExcept(u8 byMtId);

    void Clear(){   memset(this, 0, sizeof(TMVRcvGrp)); }
    void Print(void);

protected:
    BOOL32 IsGrpNull(emType type);

private:
    u8 m_aabyGrp[emEnd][24];
    u8 m_abyExcept[24];   //������������
};

//2��˫������Ⱥ�飨len: 24*11=264�� FIXME: δ����1080˫��

struct TDSRcvGrp
{
    enum emType
    {
        emBegin     = 0,
            em720p  = 1,
            em4Cif  = 2,
            emCif   = 3,
            emH263plus = 4,
            emUXGA  = 5,
            emSXGA  = 6,
            emXGA   = 7,
            emSVGA  = 8,
            emVGA   = 9,
        emEnd
    };

    //����
    TDSRcvGrp() {   Clear();    }

    //��Ա����ɾ
    void AddMem(u8 byMtId, emType type, BOOL32 bExceptEither = FALSE);
    void RemoveMem(u8 byMtId, emType type);

    //�������Ա����ɾ
    void AddExcept(u8 byMtId);
	void ClearExpt(void);

    //Ⱥ���Ա��ȡ
    void GetMtList(u8 byRes, u8 &byNum, u8 *pbyMt);
    
    //����
    void ResIn2Out(u8 byRes, u8 &byConfRes, u8 &byConfType);
    BOOL32 IsNeedAdp(void);
    BOOL32 IsMtInExcept(u8 byMtId);
    BOOL32 GetMtMediaRes(u8 byMtId, u8 &byMediaType, u8 &byRes);

    void Clear(){   memset(this, 0, sizeof(TDSRcvGrp));    }
    void Print(void);

protected:
    u8 m_aabyGrp[emEnd][24];
    u8 m_abyExcept[24];   //˫����������
};


//3������Ⱥ�鼯�ɣ�len: 264*192=456��

class CRcvGrp
{
public:

    //��Ա����ɾ
    void AddMem(u8 byMtId, TSimCapSet &tSim, BOOL32 bExc = FALSE);
    void AddMem(u8 byMtId, TDStreamCap &tDCap, BOOL32 bExc = FALSE);
    void RemoveMem(u8 byMtId, TSimCapSet &tSim);
    void RemoveMem(u8 byMtId, TDStreamCap &tDCap);

    //�������Ա����ɾ
    void AddExcept(u8 byMtId, BOOL32 bDual = FALSE);
	void ClearExpt(BOOL32 bDual = FALSE);

    //��ȡ�ڲ��ֱ��ʱ�ʶ
    u8   GetMVRes(u8 byType, u8 byRes);
    u8   GetDSRes(u8 byType, u8 byRes);

    //Ⱥ���Ա��ȡ
    void GetMVMtList(u8 byType, u8 byRes, u8 &byNum, u8 *pbyMt, BOOL32 bForce = FALSE);
    void GetDSMtList(u8 byType, u8 byRes, u8 &byNum, u8 *pbyMt);

    //����
    BOOL32 GetMtMediaRes(u8 byMtId, u8 &byMediaType, u8 &byRes, BOOL32 bDual = FALSE);
    BOOL32 IsMtNeedAdp(u8 byMtId, BOOL32 bMV = TRUE);
    BOOL32 IsNeedAdp(BOOL32 bMV = TRUE);

    void Clear();
    void Print();

protected:
    TMVRcvGrp m_tMVGrp;
    TDSRcvGrp m_tDSGrp;    
};



/************************************************************************/
/*                                                                      */
/*                     �ˡ�MCU ���� ȫ��������                          */
/*                                                                      */
/************************************************************************/

class CMcuVcInst;

class CMcuVcData
{
	#define USERNUM_PERPKT  32      //ÿ����Ϣ���û�����
    #define ADDRENTRYNUM_PERPKT 64  //ÿ����Ϣ�е�ַ����Ŀ����
    #define ADDRGROUPNUM_PERPKT 16  //ÿ����Ϣ�е�ַ����Ŀ�����
	
	friend class CMpManager;
    friend class CMcuVcInst;

public:
    
    // ��ȡMCU״̬
    BOOL32 GetMcuCurStatus( TMcuStatus &tMcuStatus );
    
    /************************************************************************/
    /*                                                                      */
    /*                     1��������Ϣ���������                            */
    /*                                                                      */
    /************************************************************************/
	BOOL32 AddConf( CMcuVcInst *pConfInst, BOOL32 bRefreshHtml = TRUE );
	BOOL32 RemoveConf( u8 byConfIdx, BOOL32 bRefreshHtml = TRUE );
	u8     GetConfIdx( const CConfId & cConfId ) const;
    CConfId GetConfId( const u8 &byConfIdx ) const;
	u8     GetAllConf( TConfInfo atConfBuf[], u8 byBufSize, u8 byMode = 0 ) const;
	u8     GetConfNum( BOOL32 bIncOngoing, BOOL32 bIncSched, BOOL32 bIncTempl, u8 byConfSource = MCS_CONF ) const;    // ͳ��ָ���Ļ����ģ������
	BOOL32 GetConfMtTable( u8 byConfIdx, TConfMtTable * ptMtTable ) const;
	BOOL32 GetConfProtectInfo( u8 byConfIdx, TConfProtectInfo *tConfProtectInfo ) const;
	BOOL32 GetConfAllMtInfo( u8 byConfIdx, TConfAllMtInfo * pConfAllMtInfo ) const;
	TConfMtTable     *GetConfMtTable( u8 byConfIdx ) const;
	TConfSwitchTable *GetConfSwitchTable( u8 byConfIdx ) const;
	TConfProtectInfo *GetConfProtectInfo( u8 byConfIdx ) const;
	TConfEqpModule   *GetConfEqpModule( u8 byConfIdx ) const;
	TConfAllMtInfo   *GetConfAllMtInfo( u8 byConfIdx ) const;
	CMcuVcInst       *GetConfInstHandle( u8 byConfIdx );
	TPeriEqpData     *GetEqpData( u8 byEqpId );
	BOOL32 SaveConfToFile( u8 byConfIdx, BOOL32 bTemplate, BOOL32 bDefaultConf = FALSE );
	BOOL32 RemoveConfFromFile( CConfId cConfId );
	BOOL32 GetConfFromFile(u8 byConfIdx, TPackConfStore *ptPackConfStore);
	void   RefreshHtml( void );
	BOOL32 IsConfNameRepeat( LPCSTR lpszConfName, BOOL32 IsTemple );
	BOOL32 IsConfE164Repeat( LPCSTR lpszConfE164, BOOL32 IsTemple );
	u8   GetOngoingConfIdxByE164( LPCSTR lpszConfE164 );
    u8   GetTemConfIdxByE164( LPCSTR lpszConfE164 );
    u16  GetConfRateByConfIdx( u16 wConfIdx, u16& wFirstRate, u16& wSecondRate, BOOL32 bTemplate = FALSE ); // ���ݻ������ȡ��������
    u8   GetConfNameByConfId( const CConfId cConfId, LPCSTR &lpszConfName );  // ���ݻ���IDȡ��������
    CConfId GetConfIdByName(LPCSTR lpszConfName, BOOL32 bTemplate );  // ���ݻ�������ȡCConfId
	CConfId GetConfIdByE164( LPCSTR lpszConfE164, BOOL32 bTemplate ); // ���ݻ���164��ȡCConfId

	void RegisterConfToGK( u8 byConfIdx, u8 byDriId = 0, BOOL32 bTemplate = FALSE, BOOL32 bUnReg = FALSE );
    void ConfChargeByGK( u8 byConfIdx, u8 byDriId, BOOL32 bStopCharge = FALSE, u8 byCreateBy = CONF_CREATE_MCS, u8 byMtNum = 0 );
    void ConfInfoMsgPack( CMcuVcInst *pcSchInst, CServMsg &cServMsg );//������Ϣ��Ϣ���

    CConfId MakeConfId( u8 byConfIdx, u8 byTemplate, u8 byUsrGrpId, u8 byConfSource = MCS_CONF );
    u32     GetMakeTimesFromConfId(const CConfId& cConfId) const;
    
    void SetInsState( u8 byInsId, BOOL32 bState );
    u8   GetIdleInsId( void );       

    BOOL32 IsSavingBandwidth(void);             //�Ƿ��ʡ����

    void    GetMcuEqpCapacity(TEqpCapacity& tMcuEqpCap); // get the mcu's cap of equipment
    void    GetConfEqpDemand(TConfInfo& tConfInfo, TEqpCapacity& tConfEqpDemand );
	BOOL32  AnalyEqpCapacity( TEqpCapacity& tConfEqpDemand, TEqpCapacity& tMcuSupportCap);
	void GetBasCapacity( TBasReqInfo& tBasCap ); // ȡBas������
	void GetPrsCapacity( TEqpReqInfo& tPrsCap ); // ȡPrs������
    
    void GetHDBasCapacity( THDBasReqInfo& tHDBasCap ); // ȡHDBas������, zgc, 2008-08-11
    
    u8   GetConfFECType( u8 byConfIdx, u8 byMode );  // ��ǰ����ǰ�����
    u8   GetConfEncryptMode( u8 byConfIdx );         // ��ǰ�������������ģʽ
    
    /************************************************************************/
    /*                                                                      */
    /*                     2������ģ���������                              */
    /*                                                                      */
    /************************************************************************/
    BOOL32 CreateTemplate( void ); 
    BOOL32 AddTemplate( TTemplateInfo &tTemInfo );
    BOOL32 ModifyTemplate( TTemplateInfo &tTemInfo, BOOL32 bSameE164AndName );
    BOOL32 DelTemplate( u8 byConfIdx );
    BOOL32 GetTemplate( u8 byConfIdx, TTemplateInfo &tTemInfo );
    BOOL32 SetConfMapInsId( u8 byConfIdx, u8 byInsId );
    u8     GetConfMapInsId( u8 byConfIdx );
    BOOL32 SetTemRegGK( u8 byConfIdx, BOOL32 bReg );
    BOOL32 IsTemRegGK( u8 byConfIdx );
    TConfMapData   GetConfMapData( u8 byConfIdx );
    u8     GetIdleConfidx( void );
    void   Msg2TemInfo( CServMsg &cMsg, TTemplateInfo &tTemInfo, 
						s8** pszUnProcInfoHead = NULL, u16* pwUnProcLen = NULL );
    void   TemInfo2Msg( TTemplateInfo &tTemInfo, CServMsg &cMsg );
    void   ShowTemplate( void );
    void   ShowConfMap( void );
    
    //���ǻ�����ʱʹ��
    u32    GetExistSatCastIp();
    u16    GetExistSatCastPort();
	
    /************************************************************************/
    /*                                                                      */
    /*                     3��������������                                */
    /*                                                                      */
    /************************************************************************/
	void   InitPeriEqpList();
	void   SetPeriEqpConnected( u8 byEqpId, BOOL32 bConnected = TRUE );
	BOOL32 IsPeriEqpConnected( u8 byEqpId );
	void   SetPeriEqpLogicChnnl( u8 byEqpId, u8 byMediaType, u8 byChnnlNum, const TLogicalChannel * ptStartChnnl, BOOL32 bForwardChnnl );
	BOOL32 GetPeriEqpLogicChnnl( u8 byEpqId, u8 byMediaType, u8 * pbyChnnlNum, TLogicalChannel * ptStartChnnl, BOOL32 bForwardChnnl );
	void   SetEqpRtcpDstAddr( u8 byEqpId, u8 byChnnl, u32 dwDstIp, u16 wDstPort, u8 byMode = MODE_VIDEO );
	BOOL32 GetEqpRtcpDstAddr( u8 byEqpId, u8 byChnnl, u32 &dwDstIp, u16 &wDstPort, u8 byMode = MODE_VIDEO );	
	void   SetPeriEqpStatus( u8 byEqpId, const TPeriEqpStatus * ptStatus );
	BOOL32 GetPeriEqpStatus( u8 byEqpId, TPeriEqpStatus * ptStatus );
	void   SendPeriEqpStatusToMcs(u8 byEqpId);
	void   SetPeriEqpSrc( u8 byEqpId, const TMt * ptSrc, u8 byChnnlNo, u8 byMode );
	BOOL32 GetPeriEqpSrc( u8 byEqpId, TMt * ptSrc, u8 byChnnlNo, u8 byMode );
	BOOL32 IsPeriEqpValid( u8 byEqpId );
	void   SetPeriEqpIsValid( u8 byEqpId );
    TEqp   GetEqp( u8 byEqpId );
	u8     GetEqpType( u8 byEqpId );
	BOOL32 GetIdleVMP( u8 * pbyVMPBuf, u8 &byNum, u8 byVMPLen );
    BOOL32 GetIdleVmpTw( u8 * pbyMPWBuf, u8 &byNum, u8 byMPWLen );
	u8     GetIdleMixGroup( u8 &byEqpId, u8 &byGrpId );
	u8     GetIdleBasChl( u8 byAdaptType, u8 &byEqpId, u8 &byChlId );
    BOOL32 GetIdleHDBasVidChl( u8 &byEqpId, u8 &byChlIdx );
	BOOL32 IsIdleHDBasVidChlExist(u8* pbyEqpId = NULL, u8* pbyChlIdx = NULL);

    void   GetIdleMau(u8 &byNVChn, u8 &byVGAChn, u8 &byH263pChn);
    void   GetIdleMpu(u8 &byChnNum);
    void   GetMpuNum(u8 &byMpuNum);

    BOOL32 GetIdleMpuChn(u8 &byEqpId, u8 &byChnId);
    BOOL32 GetIdleMauChn(u8 byChnType, u8 &byEqpId, u8 &byChnId);

    void   ReleaseHDBasChn(u8 byEqpId, u8 byChnId);
    void   ResetHDBasChn(u8 byEqpId, u8 byChnId);

    u8     GetIdlePrsChl( u8 &byEqpId, u8 &byChlId, u8 &byChlId2, u8 &byChlIdAud );
	BOOL32 GetIdlePrsChls( u8 byChannelNum, TPrsChannel& tPrsChannel);
	u8     GetMaxIdleChlsPrsId(u8& byEqpPrsId );
	BOOL32 GetIdlePrsChls(u8 byPrsId, u8 byChannelNum, TPrsChannel& tPrsChannel);
    u8     GetIdlePrsChl( u8 &byEqpId, u8 &byChlId );
    BOOL32 IsRecorderOnline (u8 byEqpId);
	void   SetEqpAlias( u8 byEqpId, LPCSTR lpszAlias );
	LPCSTR GetEqpAlias( u8 byEqpId );
    BOOL32 IsEqpH263pMau( u8 byEqpId );

    void   SetEqpIp(u8 byId, u32 &dwIp)
    {
        if (byId < MAXNUM_MCU_PERIEQP)
        {
            m_adwEqpIp[byId] = dwIp;
        }
        return;
    }
    u32    GetEqpIp(u8 byId)
    {
        if (byId >= MAXNUM_MCU_PERIEQP)
        {
            return 0;
        }
        return m_adwEqpIp[byId];
    }



    /************************************************************************/
    /*                                                                      */
    /*                     4��DCS���������                                 */
    /*                                                                      */
    /************************************************************************/
	void	InitPeriDcsList();	//��ʼ����ǰDCS�б�
	void	SetPeriDcsConnected( u8 byDcsId, BOOL32 bConnected = TRUE );
	void	SetPeriDcsValid( u8 byDcsId );
	BOOL32  IsPeriDcsConfiged( u32 dwDcsIp );
	BOOL32	IsPeriDcsConnected( u8 byDcsId );
	BOOL32	IsPeriDcsValid( u8 byDcsId );
	void	SetDcsAlias( u8 byDcsId );
	LPCSTR  GetDcsAlias( u8 byDcsId );
    TEqp    GetDcs( u8 byDcsId );
    BOOL32  GetPeriDcsStatus( u8 byDcsId, TPeriDcsStatus * ptStatus );
    BOOL32  SetPeriDcsStatus( u8 byDcsId, const TPeriDcsStatus * ptStatus );
	
    /************************************************************************/
    /*                                                                      */
    /*                     5��������״̬��Ϣ�Ķ�д����                      */
    /*                                                                      */
    /************************************************************************/
    void SetBasChanStatus( u8 byEqpId, u8 byChanNo, u8 byStatus );
    void SetBasChanReserved( u8 byEqpId, u8 byChanNo, BOOL32 bReserved );

    u8   GetTvWallOutputMode( u8 byEqpId );
	u8   GetHduOutputMode( u8 byEqpId );    //4.6.1  �¼�  jlb
	
    /************************************************************************/
    /*                                                                      */
    /*                     6���������̨���������                          */
    /*                                                                      */
    /************************************************************************/
	void   SetMcConnected( u16 wMcInstId, BOOL32 bConnected = TRUE );
	BOOL32 IsMcConnected( u16 wMcInstId );
	void   SetMcLogicChnnl( u16 wMcInstId, u8 byMediaType, u8 byChnnlNum, const TLogicalChannel * ptStartChnnl );
	BOOL32 GetMcLogicChnnl( u16 wMcInstId, u8 byMediaType, u8 * pbyChnnlNum, TLogicalChannel * ptStartChnnl );
	void   SetMcSrc( u16 wMcInstId, const TMt * ptSrc, u8 byChnnlNo, u8 byMode );
	BOOL32 GetMcSrc( u16 wMcInstId, TMt * ptSrc, u8 byChnnlNo, u8 byMode );
	void   SetMcsRegInfo( u16 wMcInstId, TMcsRegInfo tMcsRegInfo );
	void   GetMcsRegInfo( u16 wMcInstId, TMcsRegInfo *ptMcsRegInfo );

    /************************************************************************/
    /*                                                                      */
    /*                     7��Mp���������                                  */
    /*                                                                      */
    /************************************************************************/
	BOOL32 AddMp( TMp tMp );
	BOOL32 RemoveMp( u8 byMpId );
	BOOL32 IsMpConnected( u8 byMpId );
	u32    GetMpIpAddr( u8 byMpId );
    u8     GetMpNum( void );
	u8     FindMp( u32 dwIp );
	u32    GetAnyValidMp( void );
	u8     AssignMpIdByBurden( u8 byConfIdx, TMtAlias &tMtAlias, u8 byMtId, u16 wConfBR );  // ���ӱ������룬����������IP��E164+IP
	BOOL32 IncMpMtNum( u8 byMpId, u8 byConfIdx, u8 byMtId, u16 wConfBR );
	void   DecMpMtNum( u8 byMpId, u8 byConfIdx, u8 byMtId, u16 wConfBR );    
	u8     GetMpMulticast( u8 byMpId );
    BOOL32 IsMtAssignInMp( u8 byMpId, u8 byConfIdx, u8 byMtId );

    
    /************************************************************************/
    /*                                                                      */
    /*                     8��MtAdp��������                                 */
    /*                                                                      */
    /************************************************************************/
	void AddMtAdp( TMtAdpReg &tMtAdpReg );
	void RemoveMtAdp( u8 byMtAdpId );
	BOOL32 IsMtAdpConnected( u8 byMtAdpId );
	u32  GetMtAdpIpAddr( u8 byMtAdpId );
	u8   GetMtAdpProtocalType( u8 byMtAdpId );
	u8   GetMtAdpSupportMtNum( u8 byMtAdpId );
    u8   GetMtAdpSupportSMcuNum( u8 byMtAdpId );
	u8   GetMtAdpNum( u8 byProtocolType = PROTOCOL_TYPE_H323 );
	u8   FindMtAdp( u32 dwIp, u8 byProtocolType = PROTOCOL_TYPE_H323 );
	void IncMtAdpMtNum( u8 byDriId, u8 byConfIdx, u8 byMtId );
	void DecMtAdpMtNum( u8 byDriId, u8 byConfIdx, u8 byMtId, BOOL32 bHDMt = FALSE, BOOL32 bOnlyAlterNum = FALSE);
	void ChangeDriRegConfNum( u8 byDriId, BOOL32 bInc  );
	u8   GetRegConfDriId( void );
    u8   AssignH323MtDriId( u8 byConfIdx, TMtAlias &tMtAlias, u8 byMtId );
	u8   AssignH320MtDriId( u8 byConfIdx, TMtAlias &tMtAlias, u8 byMtId );
	void ShowDri( u8 byDriId );
    BOOL32 IsMtAssignInDri( u8 byDriId, u8 byConfIdx, u8 byMtId );

    /************************************************************************/
    /*                                                                      */
    /*                     9���˿ڹ���                                      */
    /*                                                                      */
    /************************************************************************/

	//�����ն����ݶ˿ڹ���
	u16  AssignMtPort( u8 byConfIdx, u8 byMtId );
	void ReleaseMtPort( u8 byConfIdx, u8 byMtId );
    //�鲥��ַ����
    u32  AssignMulticastIp( u8 byConfIdx );
	//�鲥�˿ڹ���
	u16  AssignMulticastPort( u8 byConfIdx, u8 byMtId );
    //����鲥��ַ�Ƿ��Ѿ�����ʱ����ռ��
    BOOL32 IsMulticastAddrOccupied(u32 dwCastIp, u16 wCastPort) ;
	void ReleaseMulticastPort( u8 byConfIdx, u8 byMtId );    
    // ����ɢ������鲥��ַ�Ƿ����
    BOOL32 IsDistrConfCastAddrOccupied( u32 dwCastIp, u16 wCastPort) ;

    /************************************************************************/
    /*                                                                      */
    /*                     10������洢��Ϣ����                             */
    /*                                                                      */
    /************************************************************************/
	//����洢��Ϣ����(����ʱ���Զ��ָ��Ļ���Idx�б�)
	BOOL32 HasConfStore( u8 byConf ){ return m_abyConfStoreInfo[byConf]; }
	void SetConfStore( u8 byConf, BOOL32 bRestore ) { m_abyConfStoreInfo[byConf] = bRestore; }
	
	void SetConfRegState( u8 byConfID, u8 byRegState ){ m_abyConfRegState[byConfID] = byRegState; } 
	u8   GetConfRegState( u8 byConfID ){ return m_abyConfRegState[byConfID]; }
	//void ClearConfRegState( void ){ memset(m_abyConfStoreInfo, 0, sizeof(m_abyConfStoreInfo)); }
    void SetConfIdMakeTimes(u32 dwTimes) { m_dwMakeConfIdTimes = dwTimes; }

    /************************************************************************/
    /*                                                                      */
    /*                     11��GK��Ϣ����                                   */
    /*                                                                      */
    /************************************************************************/
	void SetRegGKDriId( u8 byRegGKDriId ){ m_byRegGKDriId = byRegGKDriId; } 
	u8   GetRegGKDriId( void ){ return m_byRegGKDriId; }

	void SetH323GKIDAlias( TH323EPGKIDAlias *ptH323GKIDAlias ){ memcpy( (void*)&m_tGKID, (void*)ptH323GKIDAlias, sizeof(TH323EPGKIDAlias)); } 
	TH323EPGKIDAlias *GetH323GKIDAlias( void ){ return &m_tGKID; }
	void SetH323EPIDAlias( TH323EPGKIDAlias *ptH323EPIDAlias ){ memcpy( (void*)&m_tEPID, (void*)ptH323EPIDAlias, sizeof(TH323EPGKIDAlias)); } 
	TH323EPGKIDAlias *GetH323EPIDAlias( void ){ return &m_tEPID; }

    void   SetChargeRegOK( BOOL32 bRegOK ) { m_byChargeRegOK = bRegOK ? 1 : 0;  }
    BOOL32 GetChargeRegOK( void ) { return m_byChargeRegOK == 1 ? TRUE : FALSE;    }
    
    /************************************************************************/
    /*                                                                      */
    /*                     12��Debug���Լ�����������Ϣ                      */
    /*                                                                      */
    /************************************************************************/   
    
	// 1. ��ȡ����������Ϣ
	void   GetBaseInfoFromDebugFile( );
	u16    GetMcuTelnetPort( );
	u16    GetMcuListenPort( );
	u32    GetMcsRefreshInterval( );
	BOOL32 IsWatchDogEnable( );
    BOOL32 GetMSDetermineType( );
	u16	   GetMsSynTime(void);	//�������ͬ��ʱ��, zgc, 2007-04-02
    BOOL32 GetBitrateScale();

	// 2. ��ȡ����ָ���ն˵�Э���������Դ������ת������Դ������
	BOOL32 GetMtCallInterfaceInfoFromDebugFile( );
	BOOL32 GetMpIdAndH323MtDriIdFromMtAlias( TMtAlias &tMtAlias, u32 &dwMtadpIpAddr, u32 &dwMpIpAddr );
	    
    
    // 3���Ӽ���SMcu�Ķ˿ڣ��¼��������˿ڣ�
    BOOL32 IsMMcuSpeaker() const;
    BOOL32 IsAutoDetectMMcuDupCall() const;
    BOOL32 IsShowMMcuMtList() const;
    u8     GetCascadeAliasType() const;
    void   SetSMcuCasPort(u16 wPort);
    u16    GetSMcuCasPort() const;

    // 4. ͨ�ò���        
	BOOL32 IsApplyChairToZxMcu() const;
    BOOL32 IsTransmitMtShortMsg() const;
    BOOL32 IsChairDisplayMtApplyInfo() const;
    BOOL32 IsSelInDoubleMediaConf() const;
    BOOL32 IsLimitAccessByMtModal() const;
    BOOL32 IsSupportSecDSCap() const;
    BOOL32 IsSendFakeCap2Polycom() const;
	BOOL32 IsSendFakeCap2Taide() const;
    BOOL32 IsSendFakeCap2TaideHD() const;
    BOOL32 IsAllowVmpMemRepeated() const;
    BOOL32 IsDistinguishHDSDMt() const;
	BOOL32 IsVidAdjustless4Polycom() const;
    BOOL32 IsSelAccord2Adp() const;
    BOOL32 IsAdpResourceCompact() const;
    BOOL32 IsSVmpOutput1080i() const;
	BOOL32 IsConfAdpManually() const;
    u8     GetBandWidthReserved4HdBas() const;
    u8     GetBandWidthReserved4HdVmp() const;

    // 5. �������Ʋ���
    BOOL32 SetLicenseNum( u16 wLicenseValue );
    u16    GetLicenseNum( void );
	BOOL32 SetVCSAccessNum( u16 wAccessNum );
	u16    GetVCSAccessNum( void );
	
    void   SetMcuExpireDate( TKdvTime &tExpireDate );
    TKdvTime GetMcuExpireDate( void );
    BOOL32 IsMcuExpiredDate(void);

    BOOL32 IsMpcRunMp();
    TMcuPerfLimit& GetPerfLimit();

    BOOL32 IsMtNumOverLicense( );         //�Ƿ񳬹�License���Ƶ�����ն˽�������
	BOOL32 IsVCSNumOverLicense(void);     //�Ƿ񳬹�License���Ƶ����VCS��������
    BOOL32 IsConfNumOverCap( BOOL32 bOngoing, u8 byConfSource = MCS_CONF );           //���������Ƿ���
    u8     GetMcuCasConfNum();                            //��ȡ������������
    BOOL32 IsCasConfOverCap();                            //���������Ƿ���
    BOOL32 GetMtNumOnDri( u8 byDriId, BOOL32 bOnline, u8 &byMtNum, u8 &byMcuNum );    //���ĳ��DRI MtAdp�ϵ��ն˻���Mcu�����������Ƿ���
    
    // 6. ���ǻ������û�ȡ
    void   GetSatInfoFromDebugFile();

    // 7. ��ӡ
    void ShowDebugInfo();
    void ShowBasInfo();

    
    /************************************************************************/
    /*                                                                      */
    /*                     13���û����û������                             */
    /*                                                                      */
    /************************************************************************/
	//��ȡ��ǰ�û������û����������Ϣ
	BOOL32 GetMcuCurUserList(CUsrManage& cUsrManageObj, u8 byGrpId, u8 *pbyBuf, u8 &byUsrItr, u8 &byUserNumInPack );
	BOOL32 GetMCSCurUserList(u8 byGrpId, u8 *pbyBuf, u8 &byUsrItr, u8 &byUserNumInPack );
	BOOL32 GetVCSCurUserList(u8 byGrpId, u8 *pbyBuf, u8 &byUsrItr, u8 &byUserNumInPack );

	BOOL32 ReloadMcuUserList( void );
	
    // ��ȡ�����û���ǰ���������ֵ
    BOOL32  GetUsrGrpUserCount(CUsrManage& cUsrManageObj, 
							   CUsrGrpsInfo& cUsrGrpObj,u8 byGrpId, 
							   u8 &byMaxNum, u8 &byNum) ;
	BOOL32  GetMCSUsrGrpUserCount(u8 byGrpId, u8 &byMaxNum, u8 &byNum);
	BOOL32  GetVCSUsrGrpUserCount(u8 byGrpId, u8 &byMaxNum, u8 &byNum);	
	

    // ���/ɾ��/�޸��û��飨����ɾ�������Ӧ��ҵ���߼���
    u8      AddUserGroup( CUsrGrpsInfo &cUsrGrpObj,  const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo );
	u8      AddMCSUserGroup( const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo  );
	u8      AddVCSUserGroup( const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo  );

    BOOL32  ChgUserGroup( CUsrGrpsInfo &cUsrGrpObj, const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo );
	BOOL32  ChgMCSUserGroup(const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo );
	BOOL32  ChgVCSUserGroup(const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo );
	
    BOOL32  DelMCSUserGroup( u8 byGrpId, u16 &wErrorNo );
	BOOL32  DelVCSUserGroup( u8 byGrpId, u16 &wErrorNo );

    // ��ȡ�û������Ϣ
    void    GetMCSUserGroupInfo ( u8 &byNum, TUsrGrpInfo **ptInfo );
    void    GetVCSUserGroupInfo ( u8 &byNum, TUsrGrpInfo **ptInfo );

    CUsrGrpsInfo * GetUserGroupInfo( void );

    // �ж��ն�IP�Ƿ���ĳ���û��������IP����
    BOOL32  IsMtIpInAllowSeg ( u8 byGrpId, u32 dwIp );

    // ��ӡ�û���
    void    PrtUserGroup();

	// �û������������
	// ��ӡ�ɾ�����޸�һ�û���������Ϣ
	BOOL  AddVCSUserTaskInfo(TUserTaskInfo& tUserTaskInfo);
	BOOL  DelVCSUserTaskInfo(s8* pachUserName);
	BOOL  ChgVCSUserTaskInfo(TUserTaskInfo& tUserTaskInfo);
	// ��ȡָ�����û�������Ϣ
	BOOL  GetSpecVCSUserTaskInfo(s8* pachUserName, TUserTaskInfo& tUserTaskInfo);
	// ���ļ��ж�ȡ�����û�������Ϣ
	BOOL  LoadVCSUsersTaskInfo();
	// �ж�ָ�������Ƿ�Ϊָ���û�������Χ��
	BOOL  IsYourTask(const s8* pachUserName, const CConfId& cConfId);
	// ɾ��ָ������
	void  DelVCSSpecTaskInfo(CConfId cConfId);


public:
	BOOL32 MsgPassCheck( u16 wEvent, u8 * const pbyMsg, u16 wLen = 0 );
	void   BroadcastToAllConf( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	BOOL32 SendMsgToConf( u8 byConfIdx, u16 wEvent, u8 * const pbyMsg, u16 wLen );
	BOOL32 SendMsgToConf( const CConfId & cConfId, u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	BOOL32 SendMsgToDaemonConf( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );

	void ClearVcData();
	CMcuVcData();
	virtual ~CMcuVcData();
	
	void EqpStatusShow( u8 byEqpId );


    /************************************************************************/
    /*                                                                      */
    /*                     14�������ȱ������ݵ���                           */
    /*                                                                      */
    /************************************************************************/ 
public:	
	BOOL32 GetVcDeamonEnvState( TMSSynEnvState &tMSSynEnvState );
	BOOL32 IsEqualToVcDeamonEnvState( TMSSynEnvState *ptMSSynEnvState, BOOL32 bPrintErr, TMSSynState *ptMSSynState );
	BOOL32 GetVcDeamonPeriEqpData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
	BOOL32 SetVcDeamonPeriEqpData( u8 *pbyBuf, u32 dwInBufLen );
	BOOL32 GetVcDeamonMCData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
	BOOL32 SetVcDeamonMCData( u8 *pbyBuf, u32 dwInBufLen );
	BOOL32 GetVcDeamonMpData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
	BOOL32 SetVcDeamonMpData( u8 *pbyBuf, u32 dwInBufLen );
	BOOL32 GetVcDeamonMtadpData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
	BOOL32 SetVcDeamonMtadpData( u8 *pbyBuf, u32 dwInBufLen );
	BOOL32 GetVcDeamonTemplateData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
	BOOL32 SetVcDeamonTemplateData( u8 *pbyBuf, u32 dwInBufLen );
	BOOL32 GetVcDeamonOtherData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
	BOOL32 SetVcDeamonOtherData( u8 *pbyBuf, u32 dwInBufLen );

	BOOL32 HangupAllVcInstTimer( void );
	BOOL32 ResumeAllVcInstTimer( void );

	BOOL32 GetAllVcInstState( TMSVcInstState &tVcInstState );
	BOOL32 SetAllVcInstState( TMSVcInstState &tVcInstState );
	
	u8     FindNextConfInstIDOfNotIdle( u8 byPreInstID );
    u8     GetTakeModeOfInstId( u8 byInstId ); // guzh [4/18/2007] ��ȡָ��������ٿ�����(ongoing / schedule)
	
	BOOL32 GetOneVcInstConfMtTableData( u8 byInsID, u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
	BOOL32 SetOneVcInstConfMtTableData( u8 byInsID, u8 *pbyBuf, u32 dwInBufLen );
	BOOL32 GetOneVcInstConfSwitchTableData( u8 byInsID, u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
	BOOL32 SetOneVcInstConfSwitchTableData( u8 byInsID, u8 *pbyBuf, u32 dwInBufLen );
	BOOL32 GetOneVcInstConfOtherMcTableData( u8 byInsID, u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
	BOOL32 SetOneVcInstConfOtherMcTableData( u8 byInsID, u8 *pbyBuf, u32 dwInBufLen );
	BOOL32 GetOneVcInstOtherData( u8 byInsID, u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
	BOOL32 SetOneVcInstOtherData( u8 byInsID, u8 *pbyBuf, u32 dwInBufLen, BOOL32 bResumeTimer );
	
	BOOL32 GetCfgFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr );
	BOOL32 SetCfgFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr );
	BOOL32 GetDebugFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr );
	BOOL32 SetDebugFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr );
	BOOL32 GetAddrbookFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr );
	BOOL32 SetAddrbookFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr );
	BOOL32 GetConfinfoFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr );
	BOOL32 SetConfinfoFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr );
	BOOL32 GetLoguserFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr );
	BOOL32 SetLoguserFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr );
    //�û�����չ��Ϣ
	BOOL32 GetUserExFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr );
	BOOL32 SetUserExFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr );

	BOOL32 GetUnProcConfinfoFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen);
	BOOL32 SetUnProcConfinfoFileData( u8 *pbyBuf, u32 dwInBufLen);
	BOOL32 GetVCSLoguserFileData(u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen);
	BOOL32 SetVCSLoguserFileData(u8 *pbyBuf, u32 dwInBufLen);
	BOOL32 GetVCSUserTaskFileData(u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen);
	BOOL32 SetVCSUserTaskFileData(u8 *pbyBuf, u32 dwInBufLen);
	
private: 
	u32  OprVcDeamonOtherData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bGet );
	u32  GetVcDeamonOtherDataLen( void );
    
    
    /************************************************************************/
    /*                                                                      */
    /*                     15�����ǻ���֧���������                         */
    /*                                                                      */
    /************************************************************************/ 
public:
    //��ȡƵ��
    void GetApplyFreqInfo(u32 &dwSatIp, u16 &wSatPort);

    //Ƶ����Ӧ�˿�
    u16  GetApplyRcvPort( void ) const;
    
    //�Ƿ�õ�Ƶ��
    void SetConfGetSendFreq( u8 byConfIdx , BOOL bTrue );
    void SetConfGetReceiveFreq( u8 byConfIdx , BOOL bTrue );
    
    BOOL IsConfGetAllFreq( u8 byConfIdx );
    BOOL IsConfGetSendFreq( u8 byConfIdx );
	BOOL IsConfGetReceiveFreq( u8 byConfIdx );

	//����Ƶ�ʹ���
    void SetConfBitRate( u8 byConfIdx, u32 dwSend, u32 dwRecv );
    u32  GetConfRcvBitRate( u8 byConfIdx );
    u32  GetConfSndBitRate( u8 byConfIdx );

    void SetConfFreq( u8 byConfIdx, u32 dwSend, u8 byPos,u32 dwRecv );
    u32  GetConfRcvFreq( u8 byConfIdx , u8 byPos );
    u32  GetConfSndFreq( u8 byConfIdx );
    
    void SetConfInfo( u8 byConfIdx );
    void ReleaseConfInfo( u8 byConfIdx );
    
    u8   GetConfRcvNum( u8 byConfIdx );
	void SetConfRcvNum( u8 byConfIdx , u8 byTotal );

    u16  GetRcvMtSignalPort( void ) const;
    u32  GetTimeRefreshIpAddr( void ) const;
    u16  GetTimeRefreshPort( void ) const;

    //MODEM��������
    void SetMcuModemConnected( u16 wModemId, BOOL bConnected );
    u8   GetIdleMcuModemForSnd();
    u8   GetIdleMcuModemForRcv();
    void ReleaseSndMcuModem( u8 byModemId );
    void ReleaseRcvMcuModem( u8 byModemId );
    
    void SetMcuModemSndData( u8 byModemId, u8 byConfIdx, u32 dwSndFreq, u32 dwSndBit );
    void SetMcuModemRcvData( u8 byModemId, u8 byConfIdx, u32 dwRevFreq, u32 dwRevBit );
    void GetMcuModemRcvData( u8 byModemId, u32 &dwRcvFreq, u32 &dwRcvBit );
	void GetMcuModemSndData( u8 byModemId, u32 &dwSndFreq, u32 &dwSndBit );

    void SetModemSportNum( u8 byModemId, u8 byNum );
	u8   GetModemSportNum( u8 byModemId );

	u8   GetConfRcvModem( u8 byConfIdx );
	u8   GetConfSndModem( u8 byConfIdx );


	void SetMtOrEqpUseMcuModem( TMt tMt, u8 byModemId, BOOL bRcv, BOOL bUsed );
	u8	 GetConfMtUsedMcuModem( u8 byConfIdx, TMt tMt, BOOL bRcv );

	u8   GetConfIdleMcuModem( u8 byConfIdx, BOOL bRcv );
	u8   ReleaseConfUsedMcuModem( u8 byConfIdx, TMt tMt, BOOL bRcv );

    void DisplayModemInfo( void );
	void ShowConfFreqInfo( void );
	void ShowSatInfo( void );

    void SetMtModemConnected( u16 wMtId, BOOL bConnected);
    BOOL IsMtModemConnected( u16 wMtId );
    void SetMtModemStatus( u16 wMtId, BOOL bError );

	u32  GetMcuMulticastDataIpAddr( void );
	u16  GetMcuMulticastDataPort( void );

	u32  GetMcuSecMulticastIpAddr( void );

	u16  GetMcuRcvMtMediaStartPort( void );
	u16  GetMtRcvMcuMediaPort( void );

	u8   GetConfRefreshTimes( u8 byConfIdx );
	void SetConfRefreshTimes( u8 byConfIdx, u8 byTimes );
	BOOL IsConfRefreshNeeded( void );

protected:
    CMcuVcInst      *m_apConfInst[MAXNUM_MCU_CONF];         //������Ϣ��
    TPeriEqpData     m_atPeriEqpTable[MAXNUM_MCU_PERIEQP];  //������Ϣ������ż�1Ϊ�����±�	
    TDcsInfo		 m_atPeriDcsTable[MAXNUM_MCU_DCS];		//DCS��Ϣ��, DCS id�ż�1Ϊ�����±� 2005-12-14
    TMcData          m_atMcTable[MAXNUM_MCU_MC + MAXNUM_MCU_VC];            //�����Ϣ�����ʵ���ż�1Ϊ�����±�
    TMpData          m_atMpData[MAXNUM_DRI];                //Mp��Ϣ��
    TMtAdpData       m_atMtAdpData[MAXNUM_DRI];             //MtAdp��Ϣ��
    
    TRecvMtPort      m_atRecvMtPort[MAXNUM_MCU_MT];
    //	TIpAddrRes       m_atMulticastIp[MAXNUM_MCU_CONF];
    TRecvMtPort      m_atMulticastPort[MAXNUM_MCU_MT];
    
    u8               m_abyConfStoreInfo[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE];   //����ʱ���Զ��ָ��Ļ���Idx�б�
    
    u8               m_abyConfRegState[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];  //state of mcu/conf gk registration
    u8               m_byRegGKDriId; //�������ʱ����GKע��İ�ID
    TH323EPGKIDAlias m_tGKID;        //GatekeeperID������������������RRQʱ����֤��Ϣ��¼
    TH323EPGKIDAlias m_tEPID;        //EndpointID������������������RRQʱ����֤��Ϣ��¼
    u8               m_byChargeRegOK;//�Ʒ���·�Ѿ�����
    
    u32              m_dwMakeConfIdTimes;   //����Ų�������,����MakeConfId���������
    TMcuDebugVal     m_tMcuDebugVal;
    
    u32              m_dwMtCallInterfaceNum;
    TMtCallInterface m_atMtCallInterface[MAXNUM_MCU_MT];  //ָ���ն˵�Э���������Դ������ת������Դ������
    
    TTemplateInfo   *m_ptTemplateInfo;                    //ģ����Ϣ
    TConfMapData     m_atConfMapData[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE];
    
    CUsrGrpsInfo     m_cUsrGrpInfo;    // MCS�û�����Ϣ 
    CUsrGrpsInfo     m_cVCSUsrGrpInfo; // VCS�û�����Ϣ
    CUsersTaskInfo   m_cVCSUsrTaskInfo;// VCS�û�����ϯ������Ϣ	
    CVmpMemVidLmt    m_cVmpMemVidLmt;			// mpu-vmp ��ͨ����������(��Ҫ�Ƿֱ���)
    
    u32              m_adwEqpIp[MAXNUM_MCU_PERIEQP];


    u8               m_abyMtModemConnected[MAXNUM_MCU_MT];      //�ն�Modem״̬(448)
    u8               m_abyMtModemStatus[MAXNUM_MCU_MT];         //�ն�Modem״̬(448)


protected:
    TMcuSatInfo      m_tSatInfo;                                 //���ǻ�������������Ϣ
    TConfFreqBitRate m_atConfFreq[MAXNUM_ONGO_CONF]; 			 //���������Ǵ����¼��Ϣ
    u8				 m_abySendConfFreq[MAX_CONFIDX];			 //��¼���������Ƿ���䷢��Ƶ��
    u8				 m_abyReceiveConfFreq[MAX_CONFIDX];			 //��¼���������Ƿ�������Ƶ��
    TModemData       m_atModemData[MAXNUM_MCU_MODEM];            //MCU��16��MODEM״̬
	u8               m_abyConfRefreshTimes[MAX_CONFIDX];		 //��ʱ������� �� ��Ϣ���͵�ʱ������

};

#endif

//END OF FILE





