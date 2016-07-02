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
#include "eqpssn.h"
#include "mcuinnerstruct.h"
#include "rpctrl.h"

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
	TTransportAddr  m_tVideoRtcpDstAddr[MAXNUM_VMP_MEMBER];   //�����ŵ���ƵRTCPĿ�ĵ�ַ
	TTransportAddr  m_tAudioRtcpDstAddr[MAXNUM_VMP_MEMBER];   //�����ŵ���ƵRTCPĿ�ĵ�ַ

	//  [5/17/2013 guodawei] ������������������ GBK/UTF8
	u8 m_byEqpCodeFormat;
};

//2���������
struct TMcData
{
	BOOL32	m_bConnected;					//����Ƿ�Ǽ�ΪFALSE��ʾδ�����Ǽ�
	u8		m_byFwdChannelNum;				//MCU������ŵ���
	TLogicalChannel	m_tFwdVideoChannel;		//MCU�������ʼ��Ƶ�ŵ�
	TLogicalChannel	m_tFwdAudioChannel;		//MCU�������ʼ��Ƶ�ŵ�
	TLogicalChannel	m_tFwdDualChannel;		//MCU�������ʼ˫���ŵ�
	TMt		m_atVidSrc[MAXNUM_MC_CHANNL];	//�û��ѡ����ƵԴ
	TMt		m_atAudSrc[MAXNUM_MC_CHANNL];	//�û��ѡ����ƵԴ
	TMt		m_atSecVidSrc[MAXNUM_MC_CHANNL];//�û��ѡ��˫��Դ
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
    u8   m_abyMtType[MAX_CONFIDX][MAXNUM_CONF_MT];           //��������ն�id
    u16  m_wMtNum;                          //mtadp�����ն���
	u8   m_byRegGKConfNum;                  //ע��GK�Ļ�����
    u16  m_wQ931Port;                       //������(�����������gkע��)    
    u16  m_wRasPort;                        //������
	u8   m_byMaxHDMtNum;                    //����֧�ֵ����HD�ն���Ŀ //  [7/27/2011 chendaiwei]
//	u8   m_abyHDMt[MAXHDLIMIT_MPC_MTADP];    //HD �ն�
	TMtAdpHDChnnlInfo m_atHdChnnlInfo[MAXHDLIMIT_MPC_MTADP];	//ռ��Ƕ����������Դ���ն���Ϣ
	u8   m_abyMacAddr[6];					//mtadp���ڽ�����Mac��ַ 
	u16  m_wMaxAudMtNum;					//��������֧�������ն˽�����  zjl 20120814
};

//6��Prsͨ������
// struct TPrsChannel
// {	
// public:
// 	TPrsChannel()
// 	{
// 		memset(this, 0, sizeof(TPrsChannel));
// 		for(u8 byLp =0; byLp < MAXNUM_PRS_CHNNL; byLp++)
// 		{
// 			m_abyPrsChannels[byLp] = EQP_CHANNO_INVALID;
// 		}
// 	}
// 
// 	u8  GetPrsId(void) const
// 	{
// 		return m_byEqpPrsId;
// 	}
// 	u8  GetChannelsNum(void) const
// 	{
// 		return m_byChannelNum;
// 	}
// 	void SetPrsId(u8 byPrsId)
// 	{
// 		m_byEqpPrsId = byPrsId;
// 	}
// 
// 	void SetChannelNum(u8 byChlsNum )
// 	{
// 		m_byChannelNum = byChlsNum;
// 	}
// 
// 	u8  m_abyPrsChannels[MAXNUM_PRS_CHNNL]; // ͨ����
// private:	
// 	u8  m_byEqpPrsId;          // Prs Id
// 	u8  m_byChannelNum;        // ͨ����	
// }
// #ifndef WIN32
// __attribute__ ( (packed) ) 
// #endif
// ;

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
		StaticLog("m_byPrsId = %d, m_byPrsChnlId = %d, m_byPrsMode = %d\n", m_byPrsId, m_byPrsChnlId, m_byPrsMode);
		StaticLog("m_tPrsSrc = (srcid:%d, srctype:%d), m_byPrsSrcOutChnl = %d, m_byIsBrdChn:%d,m_byIsStart:%d\n\n", 
			      m_tPrsSrc.GetMtId(), m_tPrsSrc.GetMtType(), m_byPrsSrcOutChnl, m_byIsBrdChn, m_byIsStart);
	}

	//ͨ������prs
	void SetPrsId(u8 byPrsId) {m_byPrsId = byPrsId;} 
	u8 GetPrsId(void){ return m_byPrsId;}

	//ͨ����
	void SetPrsChnId(u8 byPrsChnId){m_byPrsChnlId = byPrsChnId;}
	u8 GetPrsChnId(void){return m_byPrsChnlId;}
	
	//ͨ��Դ
	void SetPrsChnSrc(TMt tPrsSrc){ m_tPrsSrc = tPrsSrc;}
	TMt  GetPrsChnSrc(void){return m_tPrsSrc;}

	//ͨ��Դ���ͨ����
    void SetPrsSrcOutChn(u8 byPrsSrcOutChn){m_byPrsSrcOutChnl = byPrsSrcOutChn;}
	u8   GetPrsSrcOutChn(void){return m_byPrsSrcOutChnl;}

	//ͨ��ģʽ(����Ƶ˫��)
	void SetPrsMode(u8 byPrsMode){m_byPrsMode = byPrsMode;}
	u8 GetPrsMode(void){return m_byPrsMode;}
    
	//ͨ���Ƿ����ڹ㲥
	void SetPrsChnBrd(BOOL bBrdChn ){ m_byIsBrdChn = bBrdChn;}
	BOOL IsBrdPrsChn(void){ return m_byIsBrdChn;}

	//ͨ���Ƿ���
	void   SetChnStart(BOOL bIsStart){m_byIsStart = bIsStart;}
	BOOL32 IsChnStart(void){return m_byIsStart;}
private:
	u8 m_byPrsId;
	u8 m_byPrsChnlId;
	u8 m_byPrsMode;
	TMt m_tPrsSrc;
	u8  m_byPrsSrcOutChnl;
	//zjl �����Ƿ�㲥ͨ�� + �Ƿ���
	u8  m_byIsBrdChn;
	u8  m_byIsStart;
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
		for (u8 byIdx = 0; byIdx <MAXNUM_CONF_PRS_CHNNL; byIdx++)
		{
			m_tPrsChnlInfo[byIdx].SetNull();
// 			m_byIsStart[byIdx] = FALSE;
		}
	}

	u8 GetAllPrsChn(u8 *pbyPrsId, u8 *pbyPrsChnId)
	{
		if (NULL == pbyPrsId || NULL == pbyPrsChnId)
		{
			return 0;
		}
		u8 byChnNum = 0;
		for (u8 byChnIdx = 0; byChnIdx < MAXNUM_CONF_PRS_CHNNL; byChnIdx++)
		{
			if (!m_tPrsChnlInfo[byChnIdx].IsNull())
			{
				pbyPrsId[byChnNum]    = m_tPrsChnlInfo[byChnIdx].GetPrsId();
				pbyPrsChnId[byChnNum] = m_tPrsChnlInfo[byChnIdx].GetPrsChnId();
				byChnNum++;
			}
		}
		return byChnNum;
	}

	BOOL32 AssignPrsChnl(u8 byPrsId, u8 byPrsChnlId, u8 byPrsMode, BOOL bBrdChn)
	{
		for (u8 byChnlId = 0; byChnlId < MAXNUM_CONF_PRS_CHNNL; byChnlId++)
		{
			if (m_tPrsChnlInfo[byChnlId].IsNull())
			{
				m_tPrsChnlInfo[byChnlId].SetPrsId(byPrsId);
				m_tPrsChnlInfo[byChnlId].SetPrsChnId(byPrsChnlId);
				m_tPrsChnlInfo[byChnlId].SetPrsMode(byPrsMode);
				m_tPrsChnlInfo[byChnlId].SetPrsChnBrd(bBrdChn);
				return TRUE;
			}
		}
		return FALSE;

	}

	BOOL32 SetPrsChnSrc(u8 byPrsId, u8 byPrsChnId, TMt tPrsSrc, u8 bySrcOutChn)
	{
		if (tPrsSrc.IsNull() || byPrsId < PRSID_MIN || byPrsId > PRSID_MAX)
		{
			return FALSE;
		}
		for (u8 byChnIdx = 0; byChnIdx < MAXNUM_CONF_PRS_CHNNL; byChnIdx++)
		{
			if (m_tPrsChnlInfo[byChnIdx].GetPrsId() == byPrsId &&
				m_tPrsChnlInfo[byChnIdx].GetPrsChnId() == byPrsChnId)
			{
				m_tPrsChnlInfo[byChnIdx].SetPrsChnSrc(tPrsSrc);
				m_tPrsChnlInfo[byChnIdx].SetPrsSrcOutChn(bySrcOutChn);
				return TRUE;
			}	
		}
		return FALSE;
	}

	u8 GetPrsChnMediaMode(u8 byPrsId, u8 byPrsChnId)
	{
		for (u8 byChnIdx = 0; byChnIdx < MAXNUM_CONF_PRS_CHNNL; byChnIdx++)
		{
			if (!m_tPrsChnlInfo[byChnIdx].IsNull() &&
				m_tPrsChnlInfo[byChnIdx].GetPrsId() == byPrsId &&
				m_tPrsChnlInfo[byChnIdx].GetPrsChnId() == byPrsChnId)
			{
				return m_tPrsChnlInfo[byChnIdx].GetPrsMode();
			}
		}
		return MODE_NONE;
	}

	//ͳ��ָ�������ڵ�ǰ������ռ�õ�ͨ��
	u8 GetSpecPrsIdAllChnForConf(u8 byPrsId, u8 *pbyPrsChnId)
	{
		if (NULL == pbyPrsChnId)
		{
			return FALSE;
		}
		u8 byChnNum = 0;
		for (u8 byChnIdx = 0; byChnIdx < MAXNUM_CONF_PRS_CHNNL; byChnIdx++)
		{
			if (!m_tPrsChnlInfo[byChnIdx].IsNull() &&
				m_tPrsChnlInfo[byChnIdx].GetPrsId() == byPrsId)
			{
				pbyPrsChnId[byChnNum] = m_tPrsChnlInfo[byChnIdx].GetPrsChnId();
				byChnNum++;
			}
		}
		return byChnNum;
	}

	BOOL32 FindPrsChnForBrd(u8 byMediaMode, u8 &byPrsId, u8 &byPrsChnId)
	{
		if (MODE_VIDEO     != byMediaMode &&
			MODE_AUDIO	   != byMediaMode &&
			MODE_SECVIDEO  != byMediaMode)
		{
			return FALSE;
		}
		for (u8 byChnIdx = 0; byChnIdx < MAXNUM_CONF_PRS_CHNNL; byChnIdx++)
		{
			if (!m_tPrsChnlInfo[byChnIdx].IsNull() &&
				m_tPrsChnlInfo[byChnIdx].IsBrdPrsChn() &&
				m_tPrsChnlInfo[byChnIdx].GetPrsMode() == byMediaMode)
			{
				byPrsId = m_tPrsChnlInfo[byChnIdx].GetPrsId();
				byPrsChnId = m_tPrsChnlInfo[byChnIdx].GetPrsChnId();
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL32 RemovePrsChnl(u8 byPrsId, u8 byPrsChnlId)
	{
		for (u8 byChnlId = 0; byChnlId < MAXNUM_CONF_PRS_CHNNL; byChnlId++)
		{
			if (m_tPrsChnlInfo[byChnlId].GetPrsId() == byPrsId &&
				m_tPrsChnlInfo[byChnlId].GetPrsChnId() == byPrsChnlId)
			{
				m_tPrsChnlInfo[byChnlId].SetNull();
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL32 FindPrsChnlSrc(u8 byPrsId, u8 byPrsChnlId, u8& byPrsMode, TMt& tSrc, u8& bySrcOutChnl)
	{
		if (byPrsId < PRSID_MIN || byPrsId > PRSID_MAX)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[FindPrsChnlSrc]wrong prsid%d\n", byPrsId);
			return FALSE;
		}

		for (u8 byChnlId = 0; byChnlId < MAXNUM_CONF_PRS_CHNNL; byChnlId++)
		{
			if (m_tPrsChnlInfo[byChnlId].GetPrsId() == byPrsId &&
				m_tPrsChnlInfo[byChnlId].GetPrsChnId() == byPrsChnlId)
			{
				byPrsMode    = m_tPrsChnlInfo[byChnlId].GetPrsMode();
				tSrc         = m_tPrsChnlInfo[byChnlId].GetPrsChnSrc();
				bySrcOutChnl = m_tPrsChnlInfo[byChnlId].GetPrsSrcOutChn();
				return TRUE;
			}
		}
		return	FALSE;
	}

	BOOL32 FindPrsChnForSrc(const TMt& tSrc, u8 bySrcOutChnl, u8 byMode, u8& byPrsId, u8& byPrsChnlId)
	{
		for (u8 byChnlId = 0; byChnlId < MAXNUM_CONF_PRS_CHNNL; byChnlId++)
		{
			if (m_tPrsChnlInfo[byChnlId].GetPrsChnSrc() == tSrc &&
				m_tPrsChnlInfo[byChnlId].GetPrsSrcOutChn() == bySrcOutChnl &&
				m_tPrsChnlInfo[byChnlId].GetPrsMode() == byMode)
			{
				byPrsId     = m_tPrsChnlInfo[byChnlId].GetPrsId();
				byPrsChnlId = m_tPrsChnlInfo[byChnlId].GetPrsChnId();
				return TRUE;
			}
		}
		return	FALSE;
	}

	TUsedPrsChnlInfo GetPrsChnInfo(u8 byPrsIdx)
	{
		return m_tPrsChnlInfo[byPrsIdx];
	}

	BOOL32 IsPrsChnStart(u8 byPrsId, u8 byChnId)
	{
		if (byPrsId < PRSID_MIN || byPrsId > PRSID_MAX)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[IsPrsChnStart]wrong prsid%d\n", byPrsId);
			return FALSE;
		}

		for (u8 byIdx = 0; byIdx < MAXNUM_CONF_PRS_CHNNL; byIdx++)
		{
			if (m_tPrsChnlInfo[byIdx].GetPrsId() == byPrsId &&
				m_tPrsChnlInfo[byIdx].GetPrsChnId() == byChnId)
			{
				return m_tPrsChnlInfo[byIdx].IsChnStart();
			}
		}
		return FALSE;
	}

	BOOL32 SetPrsChnStart(u8 byPrsId, u8 byChnId, BOOL32 bStart)
	{
		if (byPrsId < PRSID_MIN || byPrsId > PRSID_MAX)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[SetPrsChnStart]wrong prsid%d\n", byPrsId);
			return FALSE;
		}
		for (u8 byIdx = 0; byIdx < MAXNUM_CONF_PRS_CHNNL; byIdx++)
		{
			if (m_tPrsChnlInfo[byIdx].GetPrsId() == byPrsId &&
				m_tPrsChnlInfo[byIdx].GetPrsChnId() == byChnId)
			{
				m_tPrsChnlInfo[byIdx].SetChnStart(bStart);
				return TRUE;
			}
		}
		return FALSE;
	}

	void Print()
	{
		StaticLog("Dynamic Prs Chns as follows!\n");
		for (u8 byIdx = 0; byIdx < MAXNUM_CONF_PRS_CHNNL; byIdx++)
		{
			m_tPrsChnlInfo[byIdx].Print();
		}
	}
private:
// 	u8               m_byIsStart[MAXNUM_PRS_CHNNL];
	TUsedPrsChnlInfo m_tPrsChnlInfo[MAXNUM_CONF_PRS_CHNNL];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMonitorData
{
	TMt		m_tSrc;				//monitorѡ����ƵԴ
	TTransportAddr m_DstAddr;	//monitor�ĵ�ַ
	u8		m_byMode;

public:
	u8   GetMode(void) { return m_byMode; }
	void SetMode(u8 byMode){ m_byMode = byMode; }
	
	void SetDstAddr(const TTransportAddr &tDstAddr) { m_DstAddr = tDstAddr ;}
	TTransportAddr GetDstAddr(void) { return m_DstAddr; }

	TMonitorData & operator = (const TMonitorData &tMonitorData)
	{
		m_tSrc = tMonitorData.m_tSrc;
		m_DstAddr = tMonitorData.m_DstAddr;
		m_byMode = tMonitorData.m_byMode;

		return (*this);
	}
	
	BOOL32 SetMonitorSrc(const TMt &tMt)
	{
		m_tSrc = tMt;
		
		return TRUE;
	}
	
	TMt  GetMonitorSrc(void) const
	{
		return m_tSrc;
	}
	
	BOOL32 IsVacant(void)
	{
		BOOL32 bRet = FALSE;
		
		if( GetMonitorSrc().IsNull() )
		{
			bRet = TRUE;
		}
		
		return bRet;
	}
	
};

struct TMonitorMgr
{
	TMonitorData  m_atMonitorDate[MAXNUM_MONITOR_NUM];
	
public:
	
	BOOL32 AddMonitorData(TMonitorData tMonitorData);						//��Ӽ������
	BOOL32 GetMonitorSrc(u8 byMode, const TTransportAddr &tTransportAddr, TMt *ptSrc);	//��ȡĳ��ص�Դ

	BOOL32 GetMonitorData(u16 wIdx, TMonitorData &tMonitorData);			//��ȡĳ��ص�����

private:
	
	BOOL32 GetMonitorSrc(u16 wIdx, TMt *ptSrc);								//��ȡĳ��ص�Դ
	BOOL32 SetMonitorData(u16 wIdx, TMonitorData tMonitorData);				//����ĳ��ص�����
	u16 GetMonitorIdx( u8 byMode, const TTransportAddr &tTransportAddr);	//��ȡ�������������dst��ַ��	
	u16 AssignMonitorIdx(void);												//����������
};

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

    BOOL32 IsEmpty(void) const { return (0 == m_byConfIdx); }
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

//zjj20091102 ����ģ�������Ϣ����(������Щ����TConfStore�ṹ�У���Ҫ��̬����������)
//����ֻ�����vcs�����еĵ���ǽԤ��
class CConfInfoFileExtraDataMgr
{
protected:
	u8 *m_pPlanData;
	u8 m_byTvWallPlanNum;
	u8 m_byIsReadPlan;
	u16 m_wBufLen;
	//TConfStore m_tConfStore;

public:
	CConfInfoFileExtraDataMgr() 
	{ 
		//m_tConfStore.m_tConfInfo.Reset();
		m_pPlanData = NULL;m_byTvWallPlanNum = m_byIsReadPlan = 0;
		m_wBufLen = 0; 
	}
	~CConfInfoFileExtraDataMgr(){ Clear(); }
	void Clear()
	{
		if( m_pPlanData )
		{
			delete []m_pPlanData;	
			m_pPlanData = NULL;
		}
		m_byTvWallPlanNum = m_byIsReadPlan = 0;
		m_wBufLen = 0;
	}

	void GetAllPlanName( s8 **pPlanData,u8 &byPlanNum );

	BOOL32 GetMtPlanAliasByPlanName( s8 *pPlanName, 
							TMtVCSPlanAlias *pMtVCSPlanAlias,
							u8 &byMtAliasNum );
	
	BOOL32 CheckPlanData( void );

	BOOL32 AddPlanName( s8 *pPlanName, u8 byConfIdx, BOOL32 bIsDefaultConf = FALSE );

	BOOL32 DelPlanName( s8 *pPlanName, u8 byConfIdx, BOOL32 bIsDefaultConf = FALSE );

	BOOL32 ModifyPlanName( s8 *pPlanOldName,s8 *pPlanNewName, u8 byConfIdx,BOOL32 bIsDefaultConf = FALSE );

	BOOL32 IsHasPlanName( s8 *pPlanName );

	BOOL32 SaveMtPlanAliasByPlanName( s8 *pPlanName, 
							TMtVCSPlanAlias *pMtVCSPlanAlias,
							u8 byMtAliasNum,
							u8 byConfIdx,
							BOOL32 bIsDefaultConf = FALSE);

	// ��Ӳ������Ƿ�Ҫ��תΪGBK�����ڶ�MCS�ϰ汾Ԥ�� [pengguofeng 7/24/2013]
	BOOL32 ReadExtraDataFromConfInfoFile(/* TConfStore tConfStore */u8 byConfIdx, BOOL32 bIsDefaultConf = FALSE,
		BOOL32 bNeedChg2GBK = FALSE);

//	BOOL32 SaveExtraDataToConfInfoFile( TConfStore tConfStore );
	void   TransEncoding2Utf8();  // ��Ԥ���е�����תΪUTF8 [pengguofeng 7/17/2013]

	BOOL32 IsReadPlan();

	u8 GetMtPlanAliasNumByPlanName( s8 *pPlanName );

	u8 GetMtPlanNum();

	u16 GetMSData(u8 *pbyBuf);

	u16 SetMSData(u8 *pbyBuf);
	
	u16 GetMSDataLen();
	
	BOOL32 GetDefaultConfStore( TConfStore & tConfStore );

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
    s32  GetGrpNum() const
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
		StaticLog("UserName:%s\n", m_achUserName);
		StaticLog("TaskNum :%d\n", m_wTaskNum);
		for (u16 wIndex = 0; wIndex < m_wTaskNum; wIndex++)
		{
			StaticLog("Conf%d:\n", wIndex);
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

	// ��ӡ [pengguofeng 5/15/2013]
	void  Print(void)
	{
		for ( u8 byUserNum = 0; byUserNum < MAXNUM_VCSUSERNUM; byUserNum++)
		{
			if ( m_abyUsed[byUserNum] == 1)
			{
				m_tUserTaskInfo[byUserNum].Print();
			}
		}
	}

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

	u8	GetCurVCMTMcIdx()				  { return m_byVCMTMcIdx;}
	void SetCurVCMTMcIdx( u8 byIndex )	  { m_byVCMTMcIdx = byIndex; }
	BOOL32 GetIsTvWallOperating(){ return ( 1 == m_byIsTvWallOperating); }
	void SetIsTvWallOperating( BOOL32 bIsOperating )
	{
		if( bIsOperating )
		{
			m_byIsTvWallOperating = 1;
		}
		else
		{
			m_byIsTvWallOperating = 0;
		}
	}

	BOOL32 GetIsPlanDataOperation(){ return 1 == m_byIsPlanDataOperation; }
	void SetIsPlanDataOperation( BOOL32 bIsOperating )
	{
		if( bIsOperating )
		{
			m_byIsPlanDataOperation = 1;
		}
		else
		{
			m_byIsPlanDataOperation = 0;
		}
	}

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
		m_byIsTvWallOperating = 0;
		m_byIsPlanDataOperation = 0;
		memset( m_achLockedPlanName,0,sizeof( m_achLockedPlanName ) );
		memset( m_abyLockedVcsSsnId,0,sizeof( m_abyLockedVcsSsnId ) );
		//memset( m_abyVmpMemReplcePrio,0,sizeof( m_abyVmpMemReplcePrio ) );
	}

	BOOL32 IsPlanLockedByOtherVcs( u8 *pbyPlanName,u8 byStrLen,u8 bySsnId )
	{
		for( u8 byIdx = 0;byIdx < VCS_MAXNUM_PLAN;byIdx++ )
		{
			if( bySsnId == m_abyLockedVcsSsnId[byIdx] &&
				0 == memcmp( &m_achLockedPlanName[byIdx][0],pbyPlanName,byStrLen) &&
				strlen( &m_achLockedPlanName[byIdx][0] ) == byStrLen 
				)
			{
				return FALSE;
			}		
		}
		return TRUE;
	}

	BOOL32 LockPlan( u8 *pbyPlanName,u8 byStrLen,u8 bySsnId )
	{
		for( u8 byIdx = 0;byIdx < VCS_MAXNUM_PLAN;byIdx++ )
		{
			if( 0 == m_abyLockedVcsSsnId[byIdx] )
			{
				m_abyLockedVcsSsnId[byIdx] = bySsnId;
				memcpy( &m_achLockedPlanName[byIdx][0],pbyPlanName,byStrLen );
				m_achLockedPlanName[byIdx][byStrLen] = '\0';
				return TRUE;
			}		
		}
		return FALSE;
	}

	
	BOOL32 UnlockPlan( u8 *pbyPlanName,u8 byStrLen,u8 bySrcSsnId )
	{
		for( u8 byIdx = 0;byIdx < VCS_MAXNUM_PLAN;byIdx++ )
		{
			if( bySrcSsnId == m_abyLockedVcsSsnId[byIdx] &&
				0 == memcmp( &m_achLockedPlanName[byIdx][0],pbyPlanName,byStrLen) &&
				strlen( &m_achLockedPlanName[byIdx][0] ) == byStrLen
				)
			{
				m_abyLockedVcsSsnId[byIdx] = 0;
				memset( &m_achLockedPlanName[byIdx][0],0,VCS_MAXLEN_ALIAS );
				return TRUE;
			}		
		}
		return FALSE;
	}

	void UnlockPlan( u8 bySrcSsnId )
	{
		if( 0 == bySrcSsnId )
		{
			return;
		}
		
		for( u8 byIdx = 0;byIdx < VCS_MAXNUM_PLAN;byIdx++ )
		{
			if( bySrcSsnId == m_abyLockedVcsSsnId[byIdx] )
			{
				m_abyLockedVcsSsnId[byIdx] = 0;
				memset( &m_achLockedPlanName[byIdx][0],0,VCS_MAXLEN_ALIAS );
				//return TRUE;
			}
		}
		return;
	}

	/*void SetVmpMemberReplacePrio(u8 byVmpChnIndex,u8 byMemprio)
	{
		if ( byVmpChnIndex >= MAXNUM_VMP_MEMBER )//�Ƿ�����
		{
			return ;
		}
		//m_abyVmpMemReplcePrio[byVmpChnIndex] = byMemprio;
		
	}

	u8	GetVmpMemberReplacePrio( u8 byVmpChnIndex )
	{
		if (byVmpChnIndex >= MAXNUM_VMP_MEMBER)//�Ƿ�����
		{
			return 0;
		}
		return m_abyVmpMemReplcePrio[byVmpChnIndex];
	}*/

	void VCCPrint()
	{
		Print();
		StaticLog("m_tReqVCMT:mcuid��%d mtid��%d\n", m_tReqVCMT.GetMcuId(), m_tReqVCMT.GetMtId());
		StaticLog("m_wCurUseTWChanInd:%d\n", m_wCurUseTWChanInd);
		StaticLog("m_wCurUseVMPChanInd:%d\n", m_wCurUseVMPChanInd);
		StaticLog("m_byCurSrcSsnId:%d\n", m_byCurSrcSsnId);
		StaticLog("m_byCurVCMTMcIdx:%d\n", m_byVCMTMcIdx);
		StaticLog("m_byIsTvWallOperating:%d\n", m_byIsTvWallOperating);
		StaticLog("m_byIsPlanDataOperation:%d\n", m_byIsPlanDataOperation);
		for( u8 byIdx = 0;byIdx < VCS_MAXNUM_PLAN;byIdx++ )
		{
			if( 0 != m_abyLockedVcsSsnId[byIdx] )
			{
				StaticLog("LockedPlan(%d):%s(SrcSsnId:%d)\n",
							byIdx+1,
							&m_achLockedPlanName[byIdx][0],
							m_abyLockedVcsSsnId[byIdx]
							);
			}
		}
	}
	void SetVcsBackupChairMan(const TMt& tBackChairMan ){  m_tVCSBackChairMan =  tBackChairMan;  }
	TMt  GetVcsBackupChairMan( void ){ return m_tVCSBackChairMan; }

protected:
	u16 m_wCurUseTWChanInd;              // �Զ�ģʽ�£���ǰʹ�õĵ���ǽͨ��������
	u16 m_wCurUseVMPChanInd;             // �Զ�ģʽ�£���ǰ��ʹ�õĻ���ϳ�����ʼͨ����
	TMt m_tReqVCMT;                      // ��ǰ������ȵ��ն�
	u8 m_byCurSrcSsnId;                  // ��ǰ����������VCS��ʵ��ID��
	u8 m_byVCMTMcIdx;					 // ��ǰ�����ն˽���ļ��ͨ����

	

	s8	m_achLockedPlanName[VCS_MAXNUM_PLAN][VCS_MAXLEN_ALIAS];
	u8	m_abyLockedVcsSsnId[VCS_MAXNUM_PLAN];

	//zjj20091102 �Ƿ����ڶԵ���ǽ�������������𽻻���������״̬��
	u8 m_byIsTvWallOperating;//0-�� 1-��

	//zjj20091102 �Ƿ����ڶ�Ԥ�����в���
	u8 m_byIsPlanDataOperation;//0-�� 1-��

	//u8 m_abyVmpMemReplcePrio[MAXNUM_VMP_MEMBER];//zhouyiliang 20100721����ϳɳ�Ա���滻���ȼ���0Ϊ��ͣ�ֵԽ��Խ��
	TMt  m_tVCSBackChairMan;
	
};



/************************************************************************/
/*                                                                      */
/*                    ��������������Դ���� �ṹȺ                       */
/*                                                                      */
/************************************************************************/
/*--------------------------new bas----------------------------*/
enum BasChnType
{
  ILLEGAL_BAS_CHN =0,
  OLD_BAS_CHN,
  MPU2_BAS_CHN,
  APU2_BAS_CHN,
  X868KI_AUDBAS_CHN
};

//BASͨ������
class CBasChn
{
public:	
	//��������
	enum DecAbility
	{
		emDecAud = 0,
		emDecSD,
		emDecHD,
		emDecHighFps,
		emDecH261,//mpu��֧��h261
		emDecHp,
		emDecEnd,
	};

	CBasChn()
	{
		Clear();
	}

	virtual ~CBasChn()
	{

	}
	
public:
	void Clear()
	{
		m_tBas.SetNull();
		m_tSrc.SetNull();
		m_byChnId		 = 0;	
		m_byEncNum		 = 0;
		m_byDecAbility    = 0;
		m_byMediaMode	 = MODE_NONE;
		m_bIsReserved	 = FALSE;
		m_bIsOnLine		 = FALSE;
	}
	//��������
	void SetBas(TEqp tEqp){ m_tBas = tEqp; }
	TEqp GetBas()const{ return m_tBas; }
	
	//����Դ
	void SetSrc(TMt tSrc)
	{
		m_tSrc = tSrc; 
	}
	
	TMt  GetSrc()const{ return m_tSrc; }
	
	//������ͨ��
	void SetChnId(u8 byChnId){ m_byChnId = byChnId; }
	u8   GetChnId()	{ return m_byChnId; }

	//����ý��ģʽ
	void SetMediaMode(u8 byMediaMode){ m_byMediaMode = byMediaMode;}
	u8   GetMediaMode(){ return m_byMediaMode;}
	
	//���ý�������
	void SetDecAbility(u8 byDecAbility){ m_byDecAbility |= (u8)1 <<byDecAbility; }
	//�Ƿ�֧�ֽ�������
	BOOL32 IsSupportDecParam(u8 byDecAbility)
	{
		u8 byTempDecAbility = m_byDecAbility;
		return byDecAbility &= byTempDecAbility; 
	}
	
	u16	GetMaxDecAbility()
	{
		return m_byDecAbility;
	}

	
	//����״̬ռ��
	void SetIsReserved(BOOL32 bIsReserved)
	{ 
		m_bIsReserved = bIsReserved;
		//�ͷ�ռ�ã�ͬʱҲ�ָ�ͨ������
		if ( FALSE == bIsReserved )
		{
			RestoreEncArray();
		}
	}

	BOOL32 IsReserved(){ return m_bIsReserved;}
	
	//��������
	void SetIsOnLine(BOOL32 bIsOnLine)
	{
		m_bIsOnLine = bIsOnLine;
	}
	BOOL32 IsOnLine(){ return m_bIsOnLine;} 
	
	//�������ͨ����
	void SetEncNum(u8 byEncNum)
	{
		if ( byEncNum >MAXNUM_BASOUTCHN )
		{
			OspPrintf(TRUE,FALSE,"[SetEncNum]byEncNum:%d is over\n",byEncNum);
			m_byEncNum = MAXNUM_BASOUTCHN;
		}
		m_byEncNum = byEncNum;
	}
	u8   GetEncNum(){ return m_byEncNum;}	

	void SendMsgToBas( u16 wEvent, CServMsg & cServMsg, const CConfId &cConId)
	{
		if( m_tBas.GetEqpId() == 0 )
		{
			LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, "[SendMsgToBas] ERROR: Send Message.%d<%s> To Eqp %d not exist.\n",
					  wEvent, OspEventDesc(wEvent), m_tBas.GetEqpId() 
					 );
			return ;
		}
		else
		{
			cServMsg.SetConfId( cConId );
			cServMsg.SetConfIdx( m_tBas.GetConfIdx() );
			cServMsg.SetMcuId( LOCAL_MCUID );       
			g_cEqpSsnApp.SendMsgToPeriEqpSsn( m_tBas.GetEqpId(), wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );		 
		}	
	}

	
	//����tick
	virtual void SetVcuTick(u32 dwTick)
	{ 
		return;
	}

	virtual u32  GetVcuTick()
	{ 
		return 0;
	}

	//BASͨ���Ƿ���������
	virtual BOOL32 IsBasChnSupportNeeds(TNeedVidAdaptData &tNeedBasData, TVideoStreamCap *ptOccpiedResData)
	{
		return FALSE;
	}

	virtual BOOL32 IsBasChnSupportAudNeeds(TNeedAudAdaptData &tNeedAudData, TAudioTypeDesc *ptOccpiedResData = NULL, BOOL32 bIsCheckChn = TRUE)
	{
		return FALSE;
	}
	
	//�������������
	virtual u16	   GetMaxEncAbility()
	{
		return 0;
	}
	
	//��ӡ
	virtual void   Print()
	{
		return;
	}

	//�ָ�
	virtual void   RestoreEncArray()
	{
		return;
	}
	
	//ͨ�����ò����ӿ�
	virtual	BOOL32	SetAdaptParam(THDAdaptParam &tAdaptParam)
	{
		return FALSE;
	}
		
	virtual BOOL32 SetAudAdaptParam(TAudAdaptParam &tAudParam)
	{
		return FALSE;
	}


	//ǿ�Ƹ�ĳһ·����²�
	virtual BOOL32	SetAdaptParamForcely(THDAdaptParam &tAdaptParam, u8 byOutIdx)
	{
		return FALSE;
	}

	virtual BOOL32 SetAudAdaptParamForcely(TAudAdaptParam &tAudParam, u8 byOutIdx)
	{
		return FALSE;
	}

	//��������
	virtual BOOL32  StartAdapt( const TBasAdaptParamExt &tBasParamExt,CConfId &cConfId)
	{
		return FALSE;
	}

	//ֹͣ����
	virtual	BOOL32  StopAdapt(CConfId &cConfId)
	{
		return FALSE;
	}

	//ĳһ�������Ƿ�֧��ĳһ�ֱ���
	virtual BOOL32 IsSupportEncCap(const TVideoStreamCap &tSimCap, u8 byEncIdx)
	{
		return FALSE;
	}

private:
	TEqp  		m_tBas;								//������Ϣ
	u8    		m_byChnId;							//ͨ��ID
	u8    	    m_byMediaMode;						//ý�����ͣ���Ƶ/��Ƶ��
	TMt			m_tSrc;							    //Դ
	u8			m_byEncNum;							//����ͨ����
	u8			m_byDecAbility;						//��������			
	BOOL32      m_bIsReserved;						//�Ƿ�ռ��
	BOOL32      m_bIsOnLine;						//�Ƿ�����		

};


//BASͨ����������
class CMpu2BasChn: public CBasChn
{
private:
	u16		m_wMaxEncAbility;		//����������
	u8		m_byEncOtherNum;		//����other·��
	u16		m_awEncArray[MPU2_BASCHN_ENCARRAY_NUM];		//��Ӧý�أ���VICP�ֶ�
	u32     m_adwLastVCUTick;					//tick��	
private:
	//���ñ��������
	void	SetChnAbility(u8 byChnId, u8 byEqpType);

	//����BAS�����������ͷֶ�����
	void	SetEncAbility( u16 wEncAbility );


	//����other�������·��
	void SetOtherEncNum(u8 byEncNum){ m_byEncOtherNum = byEncNum;}
	u8   GetOtherEncNum()const{ return m_byEncOtherNum;}

	//����������Ƿ�����VICP����
	BOOL32 IsNeedVicpOccupy(const TVideoStreamCap &tCapSet);

	//����ͨ��ʣ������
	BOOL32  CalculateLeftEncAbility(u16 &wLeftEncAbility, u8 &wLeftEncNum, u8 &wOtherLeftEncNum);

	//����ĳ��VICP�ܺ�ռ��ͨ��
	u16	OccupyCapByEncVal(u16 wEncVal, TNeedVidAdaptData &tNeedBasData, u8 &byLeftEncNum, u8 &byLeftEncOtherNum,u8 *abyRecord);


public:
	CMpu2BasChn(const TEqp &tEqp, u8 byChnId, u8 byEqpType )
	{
		m_adwLastVCUTick = 0;
		SetBas(tEqp);
		SetChnId(byChnId);
		SetChnAbility(byChnId,byEqpType);
	}

	//�ù���ר�Ÿ�N+1����ʹ��
	CMpu2BasChn(){	m_adwLastVCUTick = 0;}
	void SetMpu2BasAttr(u8 byChnId, u8 byChnType) 
	{
		SetChnAbility(byChnId,byChnType);
	}

	~CMpu2BasChn()
	{

	}
	
	void SetVcuTick(u32 dwTick)
	{
		m_adwLastVCUTick = dwTick;
	}
	u32  GetVcuTick()
	{ 
		return m_adwLastVCUTick;
	}

	//�Ƿ�֧�ֱ����
	BOOL32 IsBasChnSupportNeeds(TNeedVidAdaptData &tNeedBasData, TVideoStreamCap *ptOccpiedResData);
	//��ȡ����������
	u16	  GetMaxEncAbility();
	//��ӡ
	void Print();	

	//ͨ����ͨˢ��
	BOOL32	SetAdaptParam(THDAdaptParam &tAdaptParam);
	//��ͨ��ĳһ��ǿ��ˢ��
	BOOL32	SetAdaptParamForcely(THDAdaptParam &tAdaptParam, u8 byOutIdx);
	//ͨ����������
	BOOL32  StartAdapt(const TBasAdaptParamExt &tBasParamExt,CConfId &cConfId);
	//ͨ��ֹͣ����
	BOOL32  StopAdapt(CConfId &cConfId);
	//�ָ�BAS �ֶ�����
	void RestoreEncArray();
	//�Ƿ�֧��ĳһ·����
	BOOL32 IsSupportEncCap(const TSimCapSet &tSimCap, u8 byEncIdx)
	{
		return TRUE;
	}

	//�Ƿ�֧����Ƶ����
	BOOL32 IsBasChnSupportAudNeeds(TNeedAudAdaptData &tNeedAudData, TAudioTypeDesc *ptOccpiedResData= NULL, BOOL32 bIsCheckChn = TRUE)
	{
		return FALSE;
	}

	BOOL32 SetAudAdaptParam(TAudAdaptParam &tAudParam)
	{
		return FALSE;
	}

	BOOL32 SetAudAdaptParamForcely(TAudAdaptParam &tAudParam, u8 byOutIdx)
	{
		return FALSE;
	}
};

class COldBasChn:public CBasChn
{
private:
	//��������
	enum EncAbility
	{
			emAudType  = 0,
		//	emH263plus,
		//	emDSCif,
		//	emDS4Cif,
		//	emDS720p,
			emXGA,
			emOther,			
			emSXGA,			
			emCif,	
			em4Cif,				
			emHD720p,
			emHD720pHighFps,
			emHD720pHighProfile,
			emHD1080p ,		
			emHD1080pHighFps,	
			emHD1080pHighProfile,
	};
	u16 m_awEncAbility[MAXNUM_BASOUTCHN];
	
	enum ChnEqpType
	{
		em8KHType  = 0,
		em8KGType   = 1,
		emVpuAudType = 2,
		em8KIType = 3,
		emOtherType = 4,
	};

	u8	m_byEqpType;		//��עͨ�����������ͣ�����һЩ�궨���ж�

	u32     m_adwLastVCUTick;					//tick��
private:
	//ͨ������
	void SetChnAbility(u8 byEqpType);
	void SetChnAbilityByChnTpye(u8 byChnType);
	void SetEncAbility(u8 byEncIdx, u16 wEncAbility);

	BOOL32 ChangeCapToEncAbility(const TVideoStreamCap *aptSimCapSet, u8 byCapSetNum, u16 &wEncAbility,u8 *pbyEncNum, u8 *pbyH264EncIdx);

	BOOL32 ChangeEncAbilityToCap(u16 wEncAbility,  u8 *pbyEncIdx, u8 *pbyEncNum,u8 byOccupiedOtherNum, TNeedVidAdaptData &tNeedBasData);

	BOOL32 GetOtherCap(TVideoStreamCap *patSimCap, u8 byNum, TVideoStreamCap *patOtherCap, u8 byOtherNum);

	//�Ƿ�֧�ֱ�������
	BOOL32 IsSupportEncParam(u8 byEncIdx, u16 wEncAbility)
	{
		if (byEncIdx >= MAXNUM_BASOUTCHN)
		{
			OspPrintf(TRUE, FALSE, "[IsSupportEncParam] byEncIdx:%d is unexpected!\n",byEncIdx);
			return FALSE;
		}
		
		if (wEncAbility > emHD1080pHighProfile)
		{
			OspPrintf(TRUE, FALSE, "[IsSupportEncParam] wEncAbility:%d is Wrong!\n",wEncAbility);
			return FALSE;
		}
		
		u16 wTempEncAbility = m_awEncAbility[byEncIdx];
		return wTempEncAbility &= 1 << wEncAbility;
	}
public:
	COldBasChn(const TEqp &tEqp, u8 byChnId, TPeriEqpStatus &tBasStatus);

	//�ù���ר�Ÿ�N+1����ʹ��
	COldBasChn(){	m_adwLastVCUTick = 0; };
	void NPlusSetAttr(u8 byChnType, BOOL32 bIsHDBas,BOOL32 bIsVPU = FALSE);	

	void SetVcuTick(u32 dwTick)
	{
		m_adwLastVCUTick = dwTick;
	}
	u32  GetVcuTick()
	{ 
		return m_adwLastVCUTick;
	}

	//�Ƿ�֧�ֱ����
	BOOL32 IsBasChnSupportNeeds(TNeedVidAdaptData &tNeedBasData, TVideoStreamCap *ptOccpiedResData);
	//��ȡ����������(������)
	u16  GetMaxEncAbility();
	//��ӡ
	void Print();

	//ͨ����ͨˢ��
	BOOL32	SetAdaptParam(THDAdaptParam &tAdaptParam);
	//ͨ��ĳһ��ǿ��ˢ��
	BOOL32	SetAdaptParamForcely(THDAdaptParam &tAdaptParam, u8 byOutIdx);
	//ͨ����������
	BOOL32  StartAdapt(const TBasAdaptParamExt &tBasParamExt,CConfId &cConfId);
	//ͨ��ֹͣ����
	BOOL32  StopAdapt(CConfId &cConfId);
	//�ָ�
	void   RestoreEncArray()
	{
		return;
	}
	//�Ƿ�֧��ĳһ·����
	BOOL32 IsSupportEncCap(const TVideoStreamCap &tSimCap, u8 byEncIdx);

	//�Ƿ�֧����Ƶ����
	BOOL32 IsBasChnSupportAudNeeds(TNeedAudAdaptData &tNeedAudData, TAudioTypeDesc *ptOccpiedResData= NULL, BOOL32 bIsCheckChn = TRUE);

	BOOL32 SetAudAdaptParam(TAudAdaptParam &tAudParam);

	BOOL32 SetAudAdaptParamForcely(TAudAdaptParam &tAudParam, u8 byOutIdx);
};


enum emAudAbility
{
	emMp3=0,
	emG722,
	emG711ULAW,
	emG711ALAW,
	emG729,
	emG728,
	emG7221POLYCOM,
	emG719,
	emAACLC,
	emAACLD,
	emAACLCDouble,
	emAACLDDouble,
	emHigh,
};



class CAudBasChn:public CBasChn
{
protected:
	u32 m_dwAudDecAbility;
	u32 m_dwAudEncAbility;
protected:
	//���ý�������
	void SetDecAbility(u8 byDecAbility);
	//���ñ�������
	void SetEncAbility(u8 byEncAbility);
	//�Ƿ�֧����������
	BOOL32 IsSupportDecAndEnc(u8 *pabyAbility, u8 byNum);
	//�������ĳ�ö������
	void ChangeAudCapToAbility(const TAudioTypeDesc *aptAudTypeDesc, u8 &byRealNum, u8 *pabyAbility);
	//���ʣ��ı������·��
	u8 GetLeftEncNum();

public:

	CAudBasChn(){}
	CAudBasChn(const TEqp &tEqp, u8 byChnId)
	{
		SetBas(tEqp);
		SetChnId(byChnId);
		//SetChnAbility();
	}

	//�ù���ר�Ÿ�N+1����ʹ��
	
	/*void SetApu2BasAttr() 
	{
		SetChnAbility();
	}*/

	~CAudBasChn(){};
public:
    //�ж�ͨ���Ƿ�֧������
    BOOL32 IsBasChnSupportAudNeeds(TNeedAudAdaptData &tNeedBasData, TAudioTypeDesc *ptOccpiedResData= NULL, BOOL32 bIsCheckChn = TRUE);
	
    //��ͨ���²�
    BOOL32 SetAudAdaptParam(TAudAdaptParam  &tAdaptParam);
	
    //��ͨ��ĳһ·ǿ���²�
    BOOL32 SetAudAdaptParamForcely(TAudAdaptParam &tAudParam, u8 byOutIdx);
	
    //ͨ����������
    BOOL32  StartAdapt(const TBasAdaptParamExt &tBasParamExt,CConfId &cConfId);
    
    //ͨ��ֹͣ����
    BOOL32  StopAdapt(CConfId &cConfId);

	//��ӡ
	virtual void Print() = 0;
};


class CApu2BasChn : public CAudBasChn
{
protected:
	void SetChnAbility();
public:
	CApu2BasChn(const TEqp &tEqp, u8 byChnId) : CAudBasChn(tEqp,byChnId)
	{		
		SetChnAbility();
	}

	//�ù���ר�Ÿ�N+1����ʹ��
	CApu2BasChn() : CAudBasChn(){}
	void SetApu2BasAttr() 
	{
		SetChnAbility();
	}
	~CApu2BasChn(){};

	void Print();

};

class C8KIAudBasChn : public CAudBasChn
{
protected:
	void SetChnAbility();
public:
	C8KIAudBasChn(const TEqp &tEqp, u8 byChnId) : CAudBasChn(tEqp,byChnId)
	{		
		SetChnAbility();
	}

	//�ù���ר�Ÿ�N+1����ʹ��
	C8KIAudBasChn() : CAudBasChn(){}
	void Set8KIAudBasAttr() 
	{
		SetChnAbility();
	}
	~C8KIAudBasChn(){};

	void Print();

};


//����ͨ�����
struct CBasChnNode
{  
	CBasChnNode()
	{
		m_ptData = NULL;
		m_ptLast = NULL;
		m_ptNext = NULL;
	}
public:
	void SetBasChnData(CBasChn *ptBasChnData){ m_ptData = ptBasChnData;}
	CBasChn *GetBasChnData(){ return m_ptData;}
	
	void SetLast(CBasChnNode *ptBasChnNode){ m_ptLast = ptBasChnNode;}
	CBasChnNode *GetLast(){ return m_ptLast;}
	
	void SetNext(CBasChnNode *ptBasChnNode){ m_ptNext = ptBasChnNode;}
	CBasChnNode *GetNext(){ return m_ptNext;}
	
	void Print()
	{
		if(GetBasChnData())
		{
			GetBasChnData()->Print();
		}		
	}
private:
	CBasChn  *m_ptData;						 //ͨ����Ϣ
	CBasChnNode  *m_ptLast;                      //ǰһ���ڵ�
    CBasChnNode  *m_ptNext;						 //��һ���ڵ�
};

//����ͨ��״̬
enum emBASCHNSTATUS
{
	BASCHN_STATE_INVALIED  = 0,
    BASCHN_STATE_READY     = 1,
	BASCHN_STATE_WAITSTART = 2,         
	BASCHN_STATE_RUNNING   = 3
} ;  


struct TVideoCommonAttr
{
    u16     m_wMaxBitRate;  //������HP������
    u8      m_byResolution; 
    u8      m_byFrameRate; 
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

struct TBasChnCap
{
private:
    u8                           m_byMediaType;      //ý������
    union UCommonCap
    {
        TVideoCommonAttr		 m_tVidCap;          //��Ƶͨ������
        u8                       m_byAudioTrackNum;  //��Ƶͨ����
    }m_tCommonCap;
public:
    TBasChnCap()
	{
		Clear();
	}
    ~TBasChnCap()
	{
		Clear();
	}

    void Clear()
	{
		m_byMediaType = MEDIA_TYPE_NULL;
	}
	
	BOOL32 IsNull() const
	{
		if ( m_byMediaType == MEDIA_TYPE_NULL )
		{
			return TRUE;
		}

		return FALSE;
	}

    //������Ƶ����
    void SetVidepCapSet(const TVideoStreamCap &tVidCap)
	{
		if ( tVidCap.GetMediaType() != MEDIA_TYPE_NULL &&
			tVidCap.GetMediaType() != 0
			)
		{
			m_byMediaType = tVidCap.GetMediaType();
			m_tCommonCap.m_tVidCap.m_byFrameRate = tVidCap.GetUserDefFrameRate();
			m_tCommonCap.m_tVidCap.m_byResolution = tVidCap.GetResolution();
			u16 wTempBitRate = tVidCap.GetMaxBitRate();
			if ( tVidCap.IsSupportHP() )
			{
				wTempBitRate |= 0x8000;
			}
			else
			{
				wTempBitRate &=0x7FFF;
			}
			m_tCommonCap.m_tVidCap.m_wMaxBitRate = wTempBitRate;
		}
		return;
	}

    BOOL32 GetVideoCapSet(TVideoStreamCap &tVidCap)
	{
		if ( m_byMediaType == MEDIA_TYPE_NULL || m_byMediaType == 0 )
		{
			return FALSE;
		}
		
		tVidCap.SetMediaType( m_byMediaType );
		tVidCap.SetUserDefFrameRate( m_tCommonCap.m_tVidCap.m_byFrameRate );
		tVidCap.SetResolution( m_tCommonCap.m_tVidCap.m_byResolution );
		tVidCap.SetMaxBitRate( m_tCommonCap.m_tVidCap.m_wMaxBitRate );

		if ( m_tCommonCap.m_tVidCap.m_wMaxBitRate & 0x8000 )
		{
			tVidCap.SetH264ProfileAttrb( emHpAttrb );
		}
		else
		{
			tVidCap.SetH264ProfileAttrb( emBpAttrb );
		}
		
		return TRUE;
	}
	
    //������Ƶ����
    void SetAudCapSet(const TAudioTypeDesc &tAudCap)
	{
		if ( tAudCap.GetAudioMediaType() != MEDIA_TYPE_NULL )
		{
			m_byMediaType = tAudCap.GetAudioMediaType();
			m_tCommonCap.m_byAudioTrackNum = tAudCap.GetAudioTrackNum();
		}

		return;
	}

    BOOL32 GetAudCapSet(TAudioTypeDesc &tAudCap)
	{
		if ( m_byMediaType == MEDIA_TYPE_NULL )
		{
			return FALSE;
		}

		tAudCap.SetAudioMediaType( m_byMediaType );
		tAudCap.SetAudioTrackNum( m_tCommonCap.m_byAudioTrackNum );
		
		return TRUE;
	}

	//���ý������
	u8 GetMediaType() const
	{
		return m_byMediaType;
	}

	//�����Ƶ�ֱ���
	u8 GetVideoResolution() const
	{
		return m_tCommonCap.m_tVidCap.m_byResolution;
	}

	//�����Ƶ֡��
	u8 GetFrameRate() const
	{
		return m_tCommonCap.m_tVidCap.m_byFrameRate;
	}

	//�����Ƶ����
	u16		GetVideoMaxBitRate() const 
	{ 
		u16 wTmpBitRate = m_tCommonCap.m_tVidCap.m_wMaxBitRate;
		wTmpBitRate = wTmpBitRate & 0x7FFF;
		return wTmpBitRate; 
	}

	//�����Ƶprofiletype
	emProfileAttrb GetVideoProfileType ( void ) const
	{
		u16 wTmpBitRate = m_tCommonCap.m_tVidCap.m_wMaxBitRate;
		if(wTmpBitRate & 0x8000)
		{
			return emHpAttrb;
		}
		else
		{
			return emBpAttrb;
		}
	}

	//�����Ƶ������
	u8 GetTrackNum() const
	{
		return m_tCommonCap.m_byAudioTrackNum;
	}
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;


//����ͨ����Ӧ�ı����������ṹ
struct TBasChnCapData
{
	TBasChnCap	atBasChnCap[MAX_CONF_BAS_ADAPT_NUM];
};



//BASͨ�����������
struct CBasChnListMgr
{
public:
    CBasChnListMgr(void)
	{
		m_pHeadNode = NULL;
	}
	
	~CBasChnListMgr(void)
	{
		m_pHeadNode = NULL;
	}

public:
	//����BASͨ��
	BOOL32  InsertBasChn(const TEqp &tEqp, u8 byChnId);		
	//ͨ���Ƿ����
	BOOL32  IsNodeExist(const TEqp &tEqp, u8 byChnId, BOOL32 bIsCanSetOnlineState = FALSE );	
	//��������
	void    SetBasOnLine(u8 byEqpId, BOOL32 bOnLine);	
	//Ϊ�㲥�����ʺ�BASͨ��
    BOOL32  CheckBasEnoughForReq(TNeedVidAdaptData *ptReqResData,TNeedAudAdaptData *pReqAudResource, CBasChn **ptConfNeedChns,  TBasChnCapData*ptBasChnCapData, u16 &wErrorCode); 
	//Ϊѡ�������ʺϵ�BASͨ��
	BOOL32  GetSuitableBasChnForVidSel(TNeedVidAdaptData &tNeedBasData, CBasChn **ppcBasChnData,  TBasChnCapData *ptBasChnCapData);
	BOOL32  GetSuitableBasChnForAudSel(TNeedAudAdaptData &tNeedBasData, CBasChn **ppcBasChnData);
	//ռ�ý��
	BOOL32  OcuppyBasChn(u8 byEqpId, u8 byChnId);			
	//�ͷŽ��
	BOOL32  ReleaseBasChn(u8 byEqpId, u8 byChnId);	
	//���ĳ��ͨ���ڸ�ͨ�����������ϣ�ǰ��ͨ�����������
	BOOL32  GetBasChnFrontOutPutNum(const TEqp &tEqp, u8 byChnId, u8 &byOutNum,  u8 &byFrontOutNum);
	
	//����ʱ���BAS������Ϣ
	u32 GetBasListBuf(u8 *pbyBuf);
	u32 SetBasListBuf(u8 *pbyBuf);	

	CBasChn* GetBasChnAddr(const TEqp &tEqp, u8 byChnId);
	//��ӡ
	void	Print();
private:
	//��BASͨ����������
	BOOL32  AddBasChnBySort(CBasChn *pcBasChn);

	//�����Ƶ�㲥BASͨ��
	BOOL32  GetBasChnsForVidBrd(TNeedVidAdaptData *ptReqResData, CBasChn **ptBasConfNeed,  TBasChnCapData *ptBasChnCapData, u8 &byRetNum);

	CBasChn*  GetOneIdleSuitableBasChn(TNeedVidAdaptData &tLeftReqResData,TVideoStreamCap *ptOccpiedResData, u8 byResDataNum);

	//�����Ƶ�㲥BASͨ��
	BOOL32 GetBasChnsForAudBrd(TNeedAudAdaptData *ptReqAudData, CBasChn **ptBasConfNeed, TBasChnCapData *ptBasChnCapData, u8 &byRetNum);

	CBasChn* GetOneIdleSuitableAudBasChn(TNeedAudAdaptData &tLeftReqResData, TAudioTypeDesc *ptOccpiedResData);

protected:
	CBasChnNode  *m_pHeadNode;										
};


class CConfBasChnData
{
private:
	CBasChn		*m_pcBasChn;			//ͨ��ָ��
	TBasChnCapData	m_tBasChnCapData;		//ͨ������
	CConfId		m_cConfId;
	//[2012/03/29 nizhijun]BAS����ʹ��
	TEqp		m_tBas;				
	u8			m_byChnId;			
public:
	CConfBasChnData()
	{
		m_pcBasChn = NULL;
	}

	CConfBasChnData(CBasChn * const pBasChn, const  TBasChnCapData&tBasChnIdxParam, CConfId cConfId)
	{
		m_pcBasChn = pBasChn;
		m_cConfId = cConfId;
		m_tBas = pBasChn->GetBas();
		m_byChnId = pBasChn->GetChnId();
		SetBasChnCapParam(tBasChnIdxParam);
	}
	
	void Clear()
	{
		m_pcBasChn = NULL;
	}

	CBasChn * GetBasChn()
	{
		return m_pcBasChn;
	}
	
	CConfId	GetConfId()
	{
		return m_cConfId;
	}
	

	void SetBasChnCapParam(const  TBasChnCapData &tBasChnParam)
	{
		memcpy( &m_tBasChnCapData,&tBasChnParam,sizeof(TBasChnCapData) );
	}


	TBasChnCapData GetBasChnCapParam()
	{
		return m_tBasChnCapData;
	}


	BOOL32 IsBasChnExist(const TEqp &tEqp, u8 byChnId)
	{
		if ( NULL == m_pcBasChn )
		{
			return FALSE;
		}
		
		if ( m_pcBasChn->GetBas() == tEqp &&
			 m_pcBasChn->GetChnId() == byChnId	 
		   )
		{
			return TRUE;
		}

		return FALSE;
	}

	BOOL32	StartAdapt(const TBasAdaptParamExt &tBasParamExt)
	{
		if ( NULL == m_pcBasChn   )
		{
			return FALSE;
		}
		
		if ( !m_pcBasChn->StartAdapt(tBasParamExt,m_cConfId) )
		{
			return FALSE;
		}
		return TRUE;
	}


	BOOL32	StopAdapt()
	{
		if ( NULL == m_pcBasChn   )
		{
			return FALSE;
		}
		
		return m_pcBasChn->StopAdapt(m_cConfId);
	}

	void ResignBasChnAddr();
};

struct TBasOutInfo
{
	TEqp	m_tBasEqp;
	u8		m_byChnId;
	u8		m_byOutIdx;
	u8		m_byOutNum;
	u8		m_byFrontOutNum;
	TBasOutInfo()
	{
		clear();
	}
	
	void clear()
	{
		m_tBasEqp.SetNull();
		m_byChnId = 0;
		m_byOutIdx = 0;
		m_byOutNum = 0;
		m_byFrontOutNum = 0;
	}
};


//����ͨ��������ṹ
struct TMcuBasChnGrp
{
public:
	//1.ͨ�ýӿڣ�

	//�����Ƿ���ڸ�ͨ��
	BOOL32 IsChnExist(const TEqp &tEqp, u8 byChnId, u8 &byArrayIdx);
	//������������ͨ��
	BOOL32 AddChn(CBasChn * const pcBasChn, const  TBasChnCapData &tBasChnIdxParam, const CConfId &cConId);
	//�����Ƴ�����ͨ��
	BOOL32 ReMoveChn(const TEqp &tEqp, u8 byChnId);
	//��ȡƥ��ģʽ������ͨ��
	BOOL32 GetChn(u8 &byNum, CBasChn **pcChn, u8 byMediaMode);
	//��ȡ��������ͨ��
	BOOL32 GetAllChn(u8 &byNum, CBasChn **pcBasChn);
	
	//����ͨ��ģʽ
	BOOL32 SetChnMode(const TEqp &tEqp, u8 byChnId, u8 byMediaMode, u8 byIdx);
	//��ȡͨ��ģʽ
	u8	   GetChnMode(const TEqp &tEqp, u8 byChnId, u8 byIdx);
	
	//����ͨ��Դ��Ϣ
	BOOL32 SetChnSrc(const TEqp &tEqp, u8 byChnId, TMt tSrc, u8 byIdx);
	//��ȡͨ��Դ��Ϣ
	TMt    GetChnSrc(const TEqp &tEqp, u8 byChnId, u8 byIdx);
	
	//����tick
	BOOL32 SetVcuTick(const TEqp &tEqp, u8 byChnId, u32 dwTick, u8 byIdx);
	//��ȡtick
	u32    GetVcuTick(const TEqp &tEqp, u8 byChnId, u8 byIdx);
	
	//��ӡ
	void   Print();

	//��ö�Ӧͨ��������
	BOOL32	GetBasResDataByEqp(TEqp tDiscBas, TNeedVidAdaptData *patBasResData);
	BOOL32	GetBasResAudDataByEqp(TEqp tDiscBas, TNeedAudAdaptData *patBasResData);

	//�㲥���䣺���ù㲥�������
	//BOOL32 SetOutPutParam(const TConfInfo& tConfInfo, THDAdaptParam tParam, u8 byMediaMode);
	//�㲥���䣺��չ㲥�������
	BOOL32 ClearAdaptParam(u8 byMediaMode);
	//��ĳһ��ͨ����ĳһ��ǿ��ˢ��
	BOOL32	RefreshBasParambyOut(TBasOutInfo &tBasOutInfo, THDAdaptParam &tBasParam);

	//�㲥ר��
	//�Թ㲥ͨ����������ˢ��
	BOOL32 RefreshSpecialVidCap(TConfInfo &tConfInfo, TVideoStreamCap &tSimCapSet, u8 byMediaMode);
	BOOL32 RefreshSpecialAudCap(TConfInfo &tConfInfo, TConfInfoEx &tConfInfoEx,TAudioTypeDesc &tAudSrcCap);
	//�㲥ͨ������ˢ��
	BOOL32 RefreshAllChnsParam(TConfInfo &tConfInfo, u8 byMediaMode,u16 wBiteRate);
	//Ѱ�ҹ㲥����ͨ��
	BOOL32	FindBasChnForVidBrd(const TVideoStreamCap &tSimCapSet,TBasOutInfo &tOutInfo,u8 byMediaMode,BOOL32 bIsExactMatch = FALSE);
	BOOL32  FindBasChnForAudBrd( const TAudioTypeDesc &tAudCap, TBasOutInfo &tOutInfo );

	//ѡ��ר��
	//ˢ��ѡ��ͨ������
	//����ѡ��ͨ������
	BOOL32	StartAdapt(const TEqp &tEqp, u8 byChnId,TBasAdaptParamExt &tBasParamExt, u8 byIdx);
	//����ѡ��ͨ������
	BOOL32	StopAdapt(const TEqp &tEqp, u8 byChnId,BOOL32 bIsSelChn = FALSE);
	//Ѱ��ѡ������ͨ��
	BOOL32	FindBasChnForVidSel(TMt &tSrc,const TVideoStreamCap &tSimCapSet,TBasOutInfo &tOutInfo,u8 byMediaMode);
	BOOL32  FindBasChnForAudSel(TMt &tSrc,const TAudioTypeDesc &tAudCap,TBasOutInfo &tOutInfo);
	//Ѱ�ҹ㲥����ͨ��
	BOOL32	SetSelChnParam(const TEqp &tEqp, u8 byChnId, u8 byMediaMode, THDAdaptParam &tBasParam);
	BOOL32	SetSelAudChnParam(const TEqp &tEqp, u8 byChnId, TAudAdaptParam &tBasParam);
	//��Ӳ�����¼
	BOOL32 AddBasSelChnParam(const TEqp &tEqp, u8 byChnId, const TBasChnCapData &tBasChnIdxParam);
	
	//����ʹ��
	void ResignBasChn();

protected:
	CConfBasChnData     m_atConfBasChn[MAXNUM_PERIEQP];//ͨ����

};



//���浥������bas������Ϣ
class CMcuBasMgr
{
public:
	CMcuBasMgr(){ Clear();}
	~CMcuBasMgr(){Clear();};
public:
	void   Clear(){ memset(this, 0, sizeof(CMcuBasMgr));} 


	/*----------------�㲥����---------------*/
	//�㲥���䣺 ������Դ����������
	BOOL32 AssignBrdChn(CBasChn **aptBasChn, TBasChnCapData *patBasChnCapData, u8 byChnNum, const CConfId &cConId);	
	//�㲥���䣺 ����ͨ���ӹ㲥���Ƴ�
	BOOL32 ReMoveBrdChn(const TEqp &tEqp, u8 byChnId);
	//�㲥���䣺 ���ù㲥�������
	//BOOL32 SetBrdAdaptParam(const TConfInfo& tConfInfo, THDAdaptParam tParam, u8 byMediaMode);
	//�㲥���䣺 ��չ㲥�������
	BOOL32 ClearBrdAdaptParam(u8 byMediaMode);	
	//�㲥���䣺 ��ȡ��Ӧģʽ��ͨ��
	BOOL32 GetBrdChnGrp(u8 &byNum, CBasChn **pcBasChn, u8 byMediaMode);	
	//�㲥���䣺 ��ȡ���й㲥��ͨ��
	BOOL32 GetAllBrdChn(u8 &byNum, CBasChn **pcBasChn);	
	//�㲥���䣺 ��ȡ�㲥����ռ�õĲ�ͬ��������Ϣ
	BOOL32 GetDiffBrdEqp(u8 byMediaMode, u8 &byEqpNum, TEqp *ptBas);
	//�㲥���䣺�㲥ˢ�������
	BOOL32	RefreshSpecialCapForVidBrd(TConfInfo &tConfInfo,TVideoStreamCap &tSimCapSet, u8 byMediaMode);
	BOOL32  RefreshSpecialCapForAudBrd(TConfInfo &tConfInfo, TConfInfoEx &tConfInfoEx,TAudioTypeDesc &tAudSrcCap);
	//�㲥���䣺�㲥ˢ��
	BOOL32	RefreshBrdParam(TConfInfo &tConfInfo, u8 byMediaMode,u16 wBiteRate);
	//���ҿ��ù㲥ͨ��
	BOOL32 FindBasChnByCapForVidBrd(const TVideoStreamCap &tSimCapSet, u8 byMediaMode, TBasOutInfo &tBasChnInfo, BOOL32 bIsExactMatch = FALSE);
	BOOL32 FindBasChnByCapForAudBrd(const TAudioTypeDesc &tAudCap, TBasOutInfo &tBasChnInfo);


	/*--------------ѡ������-----------------*/
	//ѡ�����䣺������Դ����ѡ����
	BOOL32 AssignSelChn(CBasChn *pcBasChn, const TBasChnCapData &tBasChnIdxParam,const CConfId &cConId);
	//ѡ�����䣺������Դ��ѡ����ɾ��
	BOOL32 ReMoveSelChn(const TEqp &tEqp, u8 byChnId);
	//ѡ�����䣺��Ӳ���
	BOOL32 AddBasSelChnParam(const TEqp &tEqp, u8 byChnId, const  TBasChnCapData &tBasChnIdxParam);
	//ѡ�����䣺��ȡ��Ӧģʽ��ͨ��
	BOOL32 GetSelChnGrp(u8 &byNum, CBasChn **pcBasChn, u8 byMediaMode);
	//ѡ�����䣺���ҿ���ѡ��ͨ��
	BOOL32 FindBasChnByVidCapForSel(TMt &tSrc,const TVideoStreamCap &tSimCapSet, u8 byMediaMode, TBasOutInfo &tBasChnInfo);
	BOOL32 FindBasChnByAudCapForSel(TMt &tSrc,const TAudioTypeDesc &tAudCap, TBasOutInfo &tBasChnInfo);
	//ѡ�����䣺�²�
	BOOL32 SetSelChnParam(const TEqp &tEqp, u8 byChnId, u8 byMediaMode, THDAdaptParam &tBasParam);
	BOOL32 SetSelAudChnParam(const TEqp &tEqp, u8 byChnId, TAudAdaptParam &tBasParam);
	//ѡ�����䣺 ��ȡ����ѡ����ͨ��
	BOOL32 GetAllSelChn(u8 &byNum, CBasChn **pcBasChn);	

	/*--------------�����ӿ�------------------*/
	//��ȡ��Ӧͨ��������
	u8     GetGrpType(const TEqp &tEqp, u8 byChnId);	
	//���ö�Ӧͨ��ģʽ
	BOOL32 SetChnMode(const TEqp &tEqp, u8 byChnId, u8 byMediaMode);
	//��ȡ��Ӧͨ��ģʽ
	u8     GetChnMode(const TEqp &tEqp, u8 byChnId);	
	//����ͨ��Դ��Ϣ
	BOOL32 SetChnSrc(const TEqp &tEqp, u8 byChnId, TMt tSrc);
	//��ȡͨ��Դ��Ϣ
	TMt    GetChnSrc(const TEqp &tEqp, u8 byChnId);	
	//����tick
	BOOL32 SetVcuTick(const TEqp &tEqp, u8 byChnId, u32 dwTick);
	//��ȡtick
	u32	   GetVcuTick(const TEqp &tEqp, u8 byChnId);	
	//��ȡָ��ͨ��������(���ڶ�ʱ)
	u8     GetPosition(const TEqp &tEqp, u8 byChnId);	
	//��ӡ
	void   Print();
	//ˢ��ͨ��ĳһ��������֡��������ˢ��
	BOOL32	RefreshBasParambyOut(TBasOutInfo &tBasOutInfo, THDAdaptParam &tBasParam);
	//����ĳ��ͨ������
	BOOL32	StartBasAdapt(const TEqp &tEqp,u8 byChnId, TBasAdaptParamExt &tBasParamExt);
	//ֹͣĳ��ͨ������
	BOOL32	StopBasAdapt(const TEqp &tEqp, u8 byChnId);	
	//��ö�Ӧͨ��������
	BOOL32	GetBasResDataByEqp(TEqp tDiscBas, TNeedVidAdaptData *patBasResData);
	BOOL32	GetBasResAudDataByEqp(TEqp tDiscBas, TNeedAudAdaptData *patBasResData);

	//����ʹ��
	void ResignBasChn();
private:
	TMcuBasChnGrp	  m_tBrdChn;          //������㲥������
	TMcuBasChnGrp     m_tSelChn;          //������ѡ��������
};
/*------------------------------------------------------------------*/

/************************************************************************/
/*                                                                      */
/*                    �ߡ�����ϳ���Ϣ���� �ṹȺ                       */
/*                                                                      */
/************************************************************************/
/*--------------------------new vmp----------------------------*/
//�ϳ�����ռ�����Ϣ len:(8+1+2+15+1)=27
struct TVmpPriSeizeInfo
{
public:
	TVMPMember m_tPriSeizeMember;	//Ҫ��ռ�ĳ�Ա
	u8 m_byChlIdx;					//Ҫ��ռ��Ա��ͨ��
	u16 m_wEventId;					//��Ϣid
	TSwitchInfo m_tSwitchInfo;		//������Ϣ(����ѡ��)
	u8 m_bySrcSsnId;				//Դssnid(����ѡ��ˢͼ��)
public:
	TVmpPriSeizeInfo()
	{
		Clear();
	}
	void Clear()
	{
		m_tPriSeizeMember.SetNull();
		m_byChlIdx = MAXNUM_VMP_MEMBER;
		m_wEventId = 0;
		memset(&m_tSwitchInfo, 0, sizeof(m_tSwitchInfo));
		m_bySrcSsnId = 0;
	}
};

//�ϳ���ػ�����Ϣ
struct TVmpInfo
{
	TVMPParam_25Mem m_tConfVMPParam;
	TVMPParam_25Mem m_tLastVmpParam;
	TKDVVMPOutParam m_tVMPOutParam;
	TVmpChnnlInfo m_tVmpChnnlInfo;
	TVmpPriSeizeInfo m_tVmpPriSeizeInfo;
public:
	TVmpInfo()
	{
		Clear();
	}
	void Clear()
	{
		m_tConfVMPParam.Clear();
		m_tLastVmpParam.Clear();
		m_tVMPOutParam.Initialize();
		m_tVmpChnnlInfo.clear();
		m_tVmpPriSeizeInfo.Clear();
	}
};

//N+1����ϳ�ģ����Ϣ
struct TNplusVmpModule
{
public:
	u8 m_byConfIdx;
	u8 m_byEqpId;
	u8 m_byMtId[MAXNUM_VMP_MEMBER];
	u8 m_byMemberType[MAXNUM_VMP_MEMBER];

public:
	TNplusVmpModule ( void ) 
	{ 
		Clear();
	}

	void Clear ( void )
	{
		memset(this,0,sizeof(*this));
		m_byConfIdx = 0xFF;
	}

	BOOL32 IsNull ( void )
	{
		if( m_byEqpId == 0 && m_byConfIdx == 0xFF )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	BOOL32 HasVmpMember ( void )
	{
		for( u8 byChIdx = 0; byChIdx < MAXNUM_VMP_MEMBER; byChIdx++ )
		{
			if(m_byMtId[byChIdx]!= 0 || m_byMemberType[byChIdx]!=0)
			{
				return TRUE;
			}
		}

		return FALSE;
	}

	void Print ( void )
	{
		if( m_byConfIdx!= 0xFF && m_byEqpId != 0)
		{
			StaticLog("[N+1]Vmp Module ConfIdx.%d EqpId.%d\n",m_byConfIdx,m_byEqpId);
			for (u8 byChidx = 0; byChidx < MAXNUM_VMP_MEMBER; byChidx++ )
			{
				if(m_byMemberType[byChidx]!=0 || m_byMtId[byChidx]!= 0)
				{
					StaticLog("    ChnIndex[%d] MemberType[%d] MtId[%d]\n",byChidx,m_byMemberType[byChidx],m_byMtId[byChidx]);
				}
			}
		}
	}
};

//����ϳ���Ϣ������
class CVmpInfoMgr
{
public:
	// ���ָ��vmp����ϳ���Ϣ
	TVMPParam_25Mem GetConfVmpParam(TEqp tVmpEqp)
	{
		TVMPParam_25Mem tConfVmpParam;
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			tConfVmpParam = m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tConfVMPParam;
		}
		return tConfVmpParam;
	}
	// ����ָ��vmp����ϳ���Ϣ
	void SetConfVmpParam(TEqp tVmpEqp, TVMPParam_25Mem tConfVmpParam)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tConfVMPParam = tConfVmpParam;
		}
	}
	// ���ָ��vmp��Last�ϳ���Ϣ
	TVMPParam_25Mem GetLastVmpParam(TEqp tVmpEqp)
	{
		TVMPParam_25Mem tLastVmpParam;
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			tLastVmpParam = m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tLastVmpParam;
		}
		return tLastVmpParam;
	}
	// ����ָ��vmp��Last�ϳ���Ϣ
	void SetLastVmpParam(TEqp tVmpEqp, TVMPParam_25Mem tLastVmpParam)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tLastVmpParam = tLastVmpParam;
		}
	}
	// ���ָ��vmp����ͨ����Ϣ
	TKDVVMPOutParam GetVMPOutParam(TEqp tVmpEqp)
	{
		TKDVVMPOutParam tVMPOutParam;
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			tVMPOutParam = m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVMPOutParam;
		}
		return tVMPOutParam;
	}
	// ����ָ��vmp����ͨ����Ϣ
	void SetVMPOutParam(TEqp tVmpEqp, TKDVVMPOutParam tVMPOutParam)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVMPOutParam = tVMPOutParam;
		}
	}
	// ���ָ��vmpǰ���������Ϣ
	TVmpChnnlInfo GetVmpChnnlInfo(TEqp tVmpEqp)
	{
		TVmpChnnlInfo tVmpChnnlInfo;
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			tVmpChnnlInfo = m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVmpChnnlInfo;
		}
		return tVmpChnnlInfo;
	}
	// ����ָ��vmpǰ���������Ϣ
	void SetVmpChnnlInfo(TEqp tVmpEqp, TVmpChnnlInfo tVmpChnnlInfo)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVmpChnnlInfo = tVmpChnnlInfo;
		}
	}
	// ���ָ��vmpǰ������ռ�����Ϣ
	TVmpPriSeizeInfo GetVmpPriSeizeInfo(TEqp tVmpEqp)
	{
		TVmpPriSeizeInfo tVmpPriSeizeInfo;
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			tVmpPriSeizeInfo = m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVmpPriSeizeInfo;
		}
		return tVmpPriSeizeInfo;
	}
	// ����ָ��vmpǰ������ռ�����Ϣ
	void SetVmpPriSeizeInfo(TEqp tVmpEqp, TVmpPriSeizeInfo tVmpPriSeizeInfo)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVmpPriSeizeInfo = tVmpPriSeizeInfo;
		}
	}

	void SetConfVmpMode(TEqp tVmpEqp, u8 byVMPMode)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tConfVMPParam.SetVMPMode(byVMPMode);
		}
	}
	
	u16 GetVMPOutChlBitrate(TEqp tVmpEqp, u8 byIdx)
	{
		u16 wBr = 0;
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX && byIdx < MAXNUM_MPU2_OUTCHNNL)
		{
			wBr = m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVMPOutParam.GetVmpOutCapIdx(byIdx).GetMaxBitRate();
		}
		return wBr;
	}

	void SetVMPOutChlBitrate(TEqp tVmpEqp, u8 byIdx, u16 wBr)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX && byIdx < MAXNUM_MPU2_OUTCHNNL)
		{
			TVideoStreamCap tVidStrCap = m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVMPOutParam.GetVmpOutCapIdx(byIdx);
			tVidStrCap.SetMaxBitRate(wBr);
			m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVMPOutParam.UpdateVmpOutMember(byIdx, tVidStrCap);
		}
	}
	
	void SetVMPBrdst(TEqp tVmpEqp, u8 byVMPBrdst)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tConfVMPParam.SetVMPBrdst(byVMPBrdst);
			m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tLastVmpParam.SetVMPBrdst(byVMPBrdst);
		}
	}

	void ClearVmpInfo(TEqp tVmpEqp)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].Clear();
		}
	}

	void ClearVmpAdpHdChl(TEqp tVmpEqp)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVmpChnnlInfo.clearHdChnnl();
		}
	}

	u8 GetVmpAdpMaxStyleNum(TEqp tVmpEqp)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			return m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVmpChnnlInfo.GetMaxStyleNum();
		}
		return 0;
	}

	u8 GetVmpAdpHDChlNum(TEqp tVmpEqp)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			return m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVmpChnnlInfo.GetHDChnnlNum();
		}
		return 0;
	}

	u8 GetVmpAdpMaxNumHdChl(TEqp tVmpEqp)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			return m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVmpChnnlInfo.GetMaxNumHdChnnl();
		}
		return 0;
	}

	u8 GetVmpAdpChnlCountByMt(TEqp tVmpEqp, TMt tMt)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			return m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVmpChnnlInfo.GetChnlCountByMt(tMt);
		}
		return 0;
	}

private:
	TVmpInfo m_atVmpInfo[MAXNUM_PERIEQP];//�ϳ������16������
};
/*------------------------------------------------------------------*/

/************************************************************************/
/*                                                                      */
/*                     �ˡ�����¼�������                         */
/*                                                                      */
/************************************************************************/
struct TRecBasChn
{
	TEqp	m_tBas;
	u8		m_byChnId;
	TRecBasChn()
	{
		Clear();
	}
	~TRecBasChn()
	{
		Clear();
	}

	void Clear()
	{
		m_tBas.SetNull();
		m_byChnId = 0XFF;
	}
};

#define  MAX_RECBASCHN_NUM	3
class CRecAdaptMgr
{
public:
	CRecAdaptMgr();
	~CRecAdaptMgr();

	void Clear(u8 byMediaMode = MODE_BOTH);

	void	SetRecVideoCapSet(u8 byMediaMode, const TVideoStreamCap &tVideoCap);
	BOOL32  GetRecVideoCapSet(u8 byMediaMode, TVideoStreamCap &tVideoCap);

	void	SetRecAudCapSet(const TAudioTypeDesc &tAudCap);
	BOOL32  GetRecAudCapSet(TAudioTypeDesc &tAudCap);

	void	AddRecBasChn(u8 byMediaMode, const TRecBasChn &tRecBasChn);
	BOOL32  IsExistRecBasChn(u8 byMediaMode,TRecBasChn &tRecBasChn);
	
	BOOL32  IsExistMediaModeForRecAdpat(const TRecBasChn &tRecBasChn,u8& byMediaMode);
	void   ClearBasChnForAud();

	void Print();

private:
	enum emAdaptMode
	{
		RECBAS_VIDEO = 0,
		RECBAS_SECVIDEO,
		RECBAS_AUDIO
	};

	TVideoStreamCap		m_tRecMainParam;
	TVideoStreamCap		m_tRecDSParam;
	TAudioTypeDesc		m_tRecAudParam;
	TRecBasChn			m_atRecBasChn[MAX_RECBASCHN_NUM];
}
#ifndef WIN32
__attribute__( (packed) )

#endif

;

enum emNetCapState
{
	emNetCapIdle =  0,
	emNetCapStart  = 1,
};


struct TNetCapMgr
{
public:
	TNetCapMgr()
	{
		Clear();
	}
	~TNetCapMgr()
	{
		Clear();
	}
	void Clear()
	{
		memset(this,0,sizeof(TNetCapMgr));
	}
	void SetNetCapStatus( const emNetCapState& emNetCapStatus)
	{
		m_byNetCapStatus = (u8)emNetCapStatus;
	}
	emNetCapState GetNetCapStatus()const
	{
		return (emNetCapState)m_byNetCapStatus;
	}
	void SetOprNetCapMcsInstID(const u8& byMcsInstId)
	{
		m_byOprNetCapMcsInstID = byMcsInstId;
	}
	u8 GetOprNetCapMcsInstID()const
	{
		return m_byOprNetCapMcsInstID;
	}

	void StartNetCap()
	{
#if defined(_8KE_) || defined(_8KH_)
		FILE* pCapChoice = fopen("/opt/mcu/pcap/CapChoice","w");
#else
		FILE* pCapChoice = fopen("/usr/mcu/temp/CapChoice","w");
#endif
		if (NULL != pCapChoice)
		{
			fwrite("1",1,1,pCapChoice);
			fclose(pCapChoice);
		}
	
	}
	void StopNetCap()
	{
		s8 szStartCapCmd[256] = {0};
#if defined(_8KE_) || defined(_8KH_)
		sprintf(szStartCapCmd,"for file in /opt/mcu/pcap/UsrNetCap* ; do \n mv -f $file $file.pcap \n  done;  killall tcpdump");
#else
		sprintf(szStartCapCmd,"for file in /usr/mcu/temp/UsrNetCap* ; do \n mv -f $file $file.pcap \n  done;  killall tcpdump");
#endif
		system(szStartCapCmd);

// 		FILE* pCapChoice = fopen("/opt/mcu/pcap/CapChoice","w");
// 		if (NULL != pCapChoice)
// 		{
// 			fwrite("0",1,1,pCapChoice);
// 			fclose(pCapChoice);
// 		}
	}
private:
	u8 m_byNetCapStatus;          //��ǰץ��״̬
	u8 m_byOprNetCapMcsInstID;    //��ǰ���ڲ���ץ����mcs instid

}
PACKED
;

struct TStartRecMsgInfo
{
	TMt m_tRecordMt;		
	TEqp m_tRecEqp;
	TRecStartPara m_tRecPara;
	s8 m_aszRecName[MAX_FILE_NAME_LEN];
	BOOL32 bIsRecAdaptConf;
	u16    m_wSerialNO;	//��ش�ʹ��
	u8	   m_bySrcSsnId;//��ش�ʹ��
	TMtAlias m_tVrsRecAlias;//��¼��������Ϣ
	TStartRecMsgInfo()
	{
		m_tRecordMt.SetNull();
		m_tRecEqp.SetNull();
		m_tRecPara.Reset();
		memset(m_aszRecName,0,sizeof(m_aszRecName));
		bIsRecAdaptConf = FALSE;
		m_tVrsRecAlias.SetNull();
	}
}
PACKED
;

struct TStartRecData
{
	TDoublePayload m_tDVPayload;
	TDoublePayload m_tDAPayload;
	TDoublePayload m_tDDVPayload;
	TRecRtcpBack   m_tRtcpBack;
	TStartRecData()
	{
		m_tDVPayload.Reset();
		m_tDAPayload.Reset();
		m_tDVPayload.Reset();
		m_tRtcpBack.Reset();
	}
}
PACKED
;

/************************************************************************/
/*                                                                      */
/*                     �š�������ش���ع�����                         */
/*                                                                      */
/************************************************************************/
/*==============================================================================
����    :  CMultiSpyMgr
����    :  ������ش�����
��Ҫ�ӿ�:  
��ע    :  
-------------------------------------------------------------------------------
�޸ļ�¼:  
��  ��     �汾          �޸���          �߶���          �޸ļ�¼
2009-9-28                 Ѧ��
==============================================================================*/
class CMultiSpyMgr
{
/*lint -save -sem(CMultiSpyMgr::Init,initializer)*/
public:

	CMultiSpyMgr(void);
    ~CMultiSpyMgr(void);

	BOOL32 AssignSpyChnnl ( u16 &wChnnlIdx );  // ����mcu���еĻش�ͨ��

	BOOL32 ReuseSpyChnnl( u16 wChnnlIdx );
	void   ReleaseSpyChnnl( u16 wChnnlIdx );  // �ͷŻش�ͨ��
		
	u16    GetMSMultiSpy(u8 *pbyBuf);
	u16    SetMSMultiSpy(u8 *pbyBuf);

protected:
	void Init( void );

private:

	u8	    m_byIsMultiSpy[MAXNUM_ACCOMMODATE_SPY];		//�������������spyChnnl�������Ԫ�ؼ�¼��ͨ��(ͨ����:����ֵ * PORTSPAN + SPY_MCU_STARTPORT)�Ƿ��ѱ�ռ��
	SEMHANDLE	m_hSpyRW;
};

struct TSpyPrsResource
{
protected:
	TEqp	m_tPrsEqp;//prs�豸
	u8		m_byChannel;//ռ�õ�prsͨ����

public:
	TSpyPrsResource(){ Clear(); }

	void Clear( void )
	{
		m_tPrsEqp.SetNull();
		m_byChannel = 0;
	}

	void SetPrsInfo( TEqp tPrsEqp,u8 byChannel )
	{
		m_tPrsEqp = tPrsEqp;
		m_byChannel = byChannel;
	}

	void GetPrsInfo( TEqp &tPrsEqp,u8 &byChannel )
	{
		tPrsEqp = m_tPrsEqp;
		byChannel = m_byChannel;
	}	

	TEqp GetPrsEqp( void )
	{
		return m_tPrsEqp;
	}

	u8 GetPrsChlIdx( void )
	{
		return m_byChannel;
	}

	BOOL32 IsNull( void )
	{
		return m_tPrsEqp.IsNull();
	}
}
PACKED
;
//��ش��¼��ն�ռ��bas����Ϣ
struct TSpyBasResource
{
protected:
	TEqp	m_tVideoBas;//��Ƶ�����õ�bas
	u8		m_byVideoChlIndex;//��Ƶ��������Ƶʹ�õ�basͨ����
	TEqp	m_tAudioBas;//��Ƶ�����õ�bas
	u8		m_byAudioChlIndex;//��Ƶ��Ƶʹ�õ�basͨ����
	u8		m_byMode;

public:
	TSpyBasResource( void )	{ Clear(); }

	void Clear(){ 
		m_tVideoBas.SetNull();
		m_tAudioBas.SetNull();
		m_byVideoChlIndex=m_byAudioChlIndex =0;
		m_byMode = MODE_NONE;	
	}

	void Clear( u8 byMode )
	{
		switch( byMode )
		{
			case MODE_VIDEO:
				m_tVideoBas.SetNull();
				m_byVideoChlIndex = 0;
				if( MODE_BOTH == m_byMode )
				{
					m_byMode = MODE_AUDIO;
				}
				break;
			case MODE_AUDIO:
				m_tAudioBas.SetNull();
				m_byAudioChlIndex = 0;
				if( MODE_BOTH == m_byMode )
				{
					m_byMode = MODE_VIDEO;
				}
				break;
			case MODE_BOTH:
				Clear();
				break;
			default:
				break;
		}
	}

	void SetBasInfo( TEqp tBas,u8 byChlIndex,u8 byMode )
	{
		switch( byMode )
		{
		case MODE_VIDEO:
			m_tVideoBas = tBas;
			m_byVideoChlIndex = byChlIndex;			
			break;
		case MODE_AUDIO:
			m_tAudioBas = tBas;
			m_byAudioChlIndex = byChlIndex;	
			break;
		default:
			break;
		}
		SetMode( byMode );
	}

	TEqp GetVideoBas( void ){return m_tVideoBas;}
	TEqp GetAudioBas( void ){return m_tAudioBas;}
	//void SetBas( TEqp tBas ){m_tBas = tBas;}

	u8 GetVideoChlIndex(void){ return m_byVideoChlIndex; }
	u8 GetAudioChlIndex(void){ return m_byAudioChlIndex; }
	//void SetChlIndex( u8 byChlIndex ){ m_byChlIndex = byChlIndex; }

	u8 GetMode( void ){ return m_byMode; }
	void SetMode( u8 byMode )
	{ 
		if( MODE_NONE == m_byMode )
		{			
			m_byMode = byMode; 
		}
		else if( MODE_VIDEO == m_byMode )
		{
			if( MODE_AUDIO == byMode )
			{
				m_byMode = MODE_BOTH;
			}
		}
		else if( MODE_AUDIO == m_byMode )
		{
			if( MODE_VIDEO == byMode )
			{
				m_byMode = MODE_BOTH;
			}
		}
		
	}

	BOOL32 IsNull()
	{
		return ( m_tVideoBas.IsNull() && m_tAudioBas.IsNull() );
	}


}
PACKED
;

typedef TSpyResource CSendSpy;


// ��¼���������г�Ա���ش����ϼ�����Ϣ
class CConfSendSpyInfo
{
public:

	CConfSendSpyInfo();
	void Clear( void );

    BOOL32 GetSpyChannlInfo( TMt &tMt, CSendSpy &tSrcSpyInfo );  // �����ն���Ϣ���õ������ش����ϼ��Ļش���Ա��Ϣ
	BOOL32 AddSpyChannlInfo( CSendSpy &tSrcSpyInfo );  // �����ն���Ϣ��������Ӧ�Ļش�ͨ����Ϣ
	//lukunpeng 2010/06/10 ����Ҫ���������ϴ����ж��ٴ�����ã���ȫ�����ϼ�����
//     u32    GetConfMaxSpyBW( void );  
// 	void   SetConfMaxSpyBW( u32 dwMaxSpyBW );  
//     u32    GetConfRemainSpyBW( void );  
// 	void   SetConfRemainSpyBW( u32 dwRemainSpyBW );
	BOOL32 FreeSpyChannlInfo(const TMt &tMt, u8 bySpyMode); // �����ն���Ϣ�ͷ�һ���ش�ͨ��	
	u8		GetSpyMode(const TMt &tMt );		// ��ȡspy mode ��Video, audio, both��
	//BOOL32 SetSpyMode( TMt &tMt, u8 byMode );   // ��¼��ǰ���ش�ģʽ
//	BOOL32 SetSpySimCap(const TMt &tMt, TSimCapSet &tDstSimCap );  // ���ûش���Ա�ĵ�Ŀ�Ķ˵�����
//	BOOL32 GetSpySimCap(const TMt &tMt, TSimCapSet &tDstSimCap );  // �õ��ش���Ա�ĵ�Ŀ�Ķ˵�����
	//BOOL32 IsMtInSendSpyMember( TMt &tMt );  // �Ƿ��ڻش���Ա�б�
	//BOOL32 SetOldMode( TMt &tMt, u8 byOldMode );   // ��¼�ϵ��ش�ģʽ
	//u8     GetOldMode( TMt &tMt );

	CSendSpy* GetSendSpy(u16 wIndex);
//	u16 GetSendSpyNum();
	
	//zhouyiliang 20100728 �����Ƿ��г�tmt�⻹����wSpyPort�Ļش��ն� 
	BOOL32 IsOtherSpyMtUsePort( const TMt & tMt, const u16 wSpyPort );
	
	BOOL32 IsRepeatedSpy( CSendSpy &tSrcSpyInfo, BOOL32 bNeglectMode = FALSE, BOOL32 bNeglectSpyPort = FALSE);

    BOOL32 SaveSpySimCap( const TMt &tSpyMt, const TSimCapSet &tSimCap );
	
	
private:

    CSendSpy m_cSpyMember[MAXNUM_CONF_SPY];      // ��ǰ�����лش����ϼ�����ĳ�Ա
//	u16 m_wSpyNum;								 // 20110531 zjl Ŀǰ�������û����
	//lukunpeng 2010/06/10 ����Ҫ���������ϴ����ж��ٴ�����ã���ȫ�����ϼ�����
// 	u32			   m_dwMaxSpyBW;		               // �������õ�������ڻش����ϼ�����Ļش�����
// 	u32            m_dwRemainSpyBW;                    // ��ǰ����ʣ��Ļش����ϼ�����Ļش�����
};
struct TSpyStatus
{
	enum
	{
		IDLE = 0,	//����
		NORMAL,		//����ʹ��
		WAIT_FREE,	//�ȴ��ͷ�
		WAIT_USE	//Ԥռ�ã��ȴ�����ʹ��
	};
}
PACKED
;


class CRecvSpy:public TSpyResource
{
public:

	CRecvSpy()
	{
        Clear();
	}

	void Clear()
	{
		memset( this, 0, sizeof(CRecvSpy) );
	}

	void GetSpyResource( TSpyResource &tSpySrc )
	{
		tSpySrc.m_tSpy = m_tSpy;
		tSpySrc.m_tSimCapset = m_tSimCapset;
		tSpySrc.m_tSpyAddr = m_tSpyAddr;
		tSpySrc.m_tVideoRtcpAddr = m_tVideoRtcpAddr;
		tSpySrc.m_tAudioRtcpAddr = m_tAudioRtcpAddr;
		tSpySrc.m_bySpyMode = m_bySpyMode;
		tSpySrc.m_byReserved1 = m_byReserved1;
		tSpySrc.m_dwReserved2 = m_dwReserved2;
	}

public:
	u32	m_dwVidBW;		// ��Spy�ش���Ƶռ�ô���
	u32 m_dwAudBW;		// ��Spy�ش���Ƶռ�ô���
	u8 m_byVSpyDstNum;	// ��¼�ش�Դ֮Ŀ����Ŀ(��Ƶ)
	u8 m_byASpyDstNum;  // ��¼�ش�Դ֮Ŀ����Ŀ(��Ƶ)
	u8 m_byUseState;	// ��¼�Ƿ�Ԥռ�� ��-1 ��-0
	u8 m_byPreAddMode;	// ��¼Ԥռ��ģʽ
	s8 m_byPreAddDstNum;//��¼Ԥռ������Ŀ����
};

//  [11/27/2009 pengjie] ������ش�֧�֣���¼��ص�ͨ����Դ��ӳ���ϵ
class CMcChnnlInfo
{
	/*lint -save -sem(CMcChnnlInfo::Clear,initializer)*/
public:

	CMcChnnlInfo();
	void   Clear( void );
    BOOL32 SetMcChnnlMode( u8 byMode );
	u8     GetMcChnnlMode( void );
	BOOL32 SetMcSrc( TMt &tmt );
	TMt    GetMcSrc( void );

private:

	u8 m_byMode;    // ��ص�ģʽ MODE_VIDEO,MODE_AUDIO ...
	TMt m_tSrc;     // �����ص�Դ�ն�
};

class CMcChnMgr  // ��ؼ��ͨ��������
{
public:

	CMcChnMgr();
	void Clear( void );

	//BOOL32 SetMcChnnlInfo( u8 byChnnl, CMcChnnlInfo &cMcChnnlInfo );
	//BOOL32 GetMcChnnlInfo( u8 byChnnl, CMcChnnlInfo &cMcChnnlInfo );
	BOOL32 FreeMcChnnl( u8 byChnnl );

private:

	CMcChnnlInfo m_cMcChnnl[MAXNUM_MC_CHANNL];
};

// ��¼�������������ɵ��¼��ش�Դ��Ϣ
class CConfRecvSpyInfo
{
public:
	CConfRecvSpyInfo();
	void   Clear( CMultiSpyMgr *pcMultiSpyMgr = NULL );
	BOOL32 IsMtInSpyMember(const TMt &tMt, u8 bySpyMode = MODE_NONE);	// ĳ�ն��Ƿ��ڻش���Ա��,����жϻش�ģʽ
	//BOOL32 IsMtInSpyMember(const TMt &tMt );                 // ĳ�ն��Ƿ��ڻش���Ա��
	//BOOL32 IsMcuMtInSpyMember(const u8 byMcuId );              // �ж�ĳ���¼�mcu�����Ƿ����ն��ڻش�
	//BOOL32 IsUsedSpyMode(const TMt &tMt, u8 bySpyMode = MODE_BOTH ); // �ж��Ƿ��ܹ��ͷ�һ���ش�ͨ��(��������ĳ�ֻش�ģʽ)

	void LeftOnceToFree(const TMt &tMt, u8 bySpyMode, u32 &dwCanReleaseBW, s16& swCanRelIndex);

	void IsCanFree( const TMt &tMt, u8 bySpyMode, u8 byVideoDstNum, u8 byAudioDstNum, u32 &dwCanReleaseBW, s16& swCanRelIndex);
	//BOOL32 AddSpyMode(const TMt &tMt, u8 bySpyMode, u32 dwSpyBW = 0, u8 byAddDstNum = 0);  // �����ն���Ϣ��������Ӧ�Ļش�ͨ����Ϣ, ����ն�δ�ڻش���Ա�У�����False
	//BOOL32 FreeSpyChannlInfo(const TMt &tMt, u8 bySpyMode = MODE_BOTH, BOOL32 bForce = FALSE ); // �����ն���Ϣ�ͷ�һ���ش�ͨ��

	BOOL32 GetRecvSpy(const TMt &tMt, CRecvSpy &tSrcSpyInfo );      // �����նˣ��õ���Ӧ�Ļش���Ϣ (CRecvSpy�洢�˻ش�Դ������ƵĿ����Ŀ��������Щ��һ��) 
//	BOOL32 GetSpyMtBySpyPort( u16 wSpyPort,TMt &tSrc );
//	void   FreeSpyChannlInfoByMcuId( u16 wMcuIdx, CMultiSpyMgr *pcMultiSpyMgr );

	//s16 FindSpyMt(const TMt &tMt);

	s16 FindSpyMt(const TMt &tMt, u8 bySpyMode = MODE_NONE);

//	BOOL32 IncSpyModeDst(const TMt &tMt, u8 bySpyMode, u8 byDstNum = 1, BOOL32 bIsPreAdd = FALSE);
//	BOOL32 DecSpyModeDst(const TMt &tMt, u8 bySpyMode, u8 byDstNum = 1);

	//BOOL32 AddSpyMode(const TMt &tMt, u8 bySpyMode,	BOOL32 bIsPreAdd = FALSE );
	BOOL32 RemoveSpyMode(const TMt &tMt, u8 bySpyMode, u32 &dwReleaseBW, s16& swChnnlID, u8& byRelSpyMode);   // �Ƴ��ش�ͨ����ģʽ
	BOOL32 AddSpyChnnlInfo(const TMt &tMt, u16 SpyStartPort, s16 &swIndex,BOOL32 bIsPreAdd = FALSE );         // Ϊĳ���ն����ö˿�

	BOOL32 SaveSpySimCap( const TMt &tSpyMt, const TSimCapSet &tSimCap );

	BOOL32 GetSpySimCap( const TMt &tSpyMt, TSimCapSet &tSimCap) const;

	//���ݻش�ԴĿ���������ش�ģʽ
	void AdjustSpyMode(const TMt &tMt, u32 &dwReleaseBW, s16& swChnnlID, u8& byRelSpyMode);

	BOOL32 DecAndAdjustSpyMode(const TMt &tMt, u8 bySpyMode, u32& dwReleaseBW, s16& swChnnlID, u8& byRelSpyMode);

	const CRecvSpy* GetSpyMemberInfo( u16 wIndex );//�õ�һ���ش���Ա����Ϣ

//	u16 GetSpyNum();

	void ReplaceSpy(s16 swSpyIndex, const TMt &tMt);

//private:

	BOOL32 GetRecvSpy(s16 swSpyIndex, CRecvSpy &tSrcSpyInfo );

	void   AdjustSpyMode(s16 swSpyIndex, u32 &dwReleaseBW, s16& swChnnlID, u8& byRelSpyMode);
	
	BOOL32 AddSpyMode(s16 swSpyIndex, u8 bySpyMode,BOOL32 bIsPreAdd = FALSE);

	BOOL32 IncSpyModeDst(s16 swSpyIndex, u8 bySpyMode, s16 swDstNum = 1, BOOL32 bIsPreAdd = FALSE);

	BOOL32 DecSpyModeDst(s16 swSpyIndex, u8 bySpyMode, s16 swDstNum = 1);

	BOOL32 ModifySpyModeBW(s16 swSpyIndex, u8 bySpyMode, u32 dwVidBW, u32 dwAudBW);

	BOOL32 FreeSpyModeBW(s16 swSpyIndex, u8 bySpyMode, u32 &dwReleaseBW);

	BOOL32 ClearSpyModeDst(s16 swSpyIndex, u8 bySpyMode);
	BOOL32 ReleasePreAddRes( const TMt &tMt,u32 &dwReleaseBW,s16& swChnnlID );

	BOOL32 ModifyUseState( const TMt &tMt,u8 byState );

	BOOL32 ModifyUseState( s16 swSpyIndex,u8 byState );

	BOOL32 GetFstSpyMtByMcuIdx( u16 wMcuIdx,TMt &tMt );
	
	void SetSpyBackVidRtcpAddr(s16 swSpyIndex, u8 bySpyMode, TTransportAddr tVideoRtcpAddr);
	void SetSpyBackAudRtcpAddr(s16 swSpyIndex, u8 bySpyMode, TTransportAddr tAudRtcpAddr);
	void GetSpyBackVidRtcpAddr(s16 swSpyIndex, u8 bySpyMode, TTransportAddr& tVideoRtcpAddr);
	void GetSpyBackAudRtcpAddr(s16 swSpyIndex, u8 bySpyMode, TTransportAddr& tAudRtcpAddr);

private:

	CRecvSpy m_cSpyMember[MAXNUM_CONF_SPY];     // ��ǰ������ɵĻش���Ա
//	u16 m_wSpyNum;								// 20110531 zjl Ŀǰ�������û����
};


/************************************************************************/
/*                                                                      */
/*                      ʮ��MCU ���� ȫ��������                          */
/*                                                                      */
/************************************************************************/

class CMcuVcInst;

class CMcuVcData : public CPeriEqpChnnlMModeMgr
{
	/*lint -save -sem(CMcuVcData::ClearVcData,initializer)*/
	
	#define USERNUM_PERPKT  32      //ÿ����Ϣ���û�����
    #define ADDRENTRYNUM_PERPKT 64  //ÿ����Ϣ�е�ַ����Ŀ����
    #define ADDRGROUPNUM_PERPKT 16  //ÿ����Ϣ�е�ַ����Ŀ�����
	

	friend class CMpManager;
    friend class CMcuVcInst;

protected:
	CMcuVcInst      *m_apConfInst[MAXNUM_MCU_CONF];         //������Ϣ��
	TPeriEqpData     m_atPeriEqpTable[MAXNUM_MCU_PERIEQP];  //������Ϣ������ż�1Ϊ�����±�

	TDcsInfo		 m_atPeriDcsTable[MAXNUM_MCU_DCS];		//DCS��Ϣ��, DCS id�ż�1Ϊ�����±� 2005-12-14
	TMcData          m_atMcTable[MAXNUM_MCU_MC + MAXNUM_MCU_VC];            //�����Ϣ�����ʵ���ż�1Ϊ�����±�
	TMonitorMgr		 m_atMonitorTable[MAXNUM_MCU_MC + MAXNUM_MCU_VC];		//���������Ϣ
	TMpData          m_atMpData[MAXNUM_DRI];                //Mp��Ϣ��
	TMtAdpData       m_atMtAdpData[MAXNUM_DRI];             //MtAdp��Ϣ��
	
	//zjj 20121225 �ϼ�mcu�������,���ǻ�ռ����˿ڵ�,���Զ˿ڵ���Ӧ�ö���1��,������������ն˺���ͨ���ͷ��䲻����Ϊ0
	TRecvMtPort      m_atRecvMtPort[MAXNUM_MCU_MT+1];
//	TIpAddrRes       m_atMulticastIp[MAXNUM_MCU_CONF];
	TRecvMtPort      m_atMulticastPort[MAXNUM_MCU_MT+1];

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
	u8               m_abyTempEditer[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE]; // �༭ָ��ģ����û���Ϣ
	CVmpMemVidLmt    m_cVmpMemVidLmt;			// mpu-vmp ��ͨ����������(��Ҫ�Ƿֱ���)

    u32              m_adwEqpIp[MAXNUM_MCU_PERIEQP];
	BOOL32           m_bRRQDriTransferred;      //������� Ǩ�Ʊ�ʶ

	CMultiSpyMgr	 m_cMultiSpyMgr;	//������ش�����
	
	u32				 m_dwMcuStartTick;	//ϵͳ��ʾtick��
	CBasChnListMgr   m_cBasChnListMgr;
	
	BOOL32           m_bUSBKEYFlag;     //[liu lijiu][2010/10/28]USBKEY��Ȩ�����Ƿ���֤�ɹ�������8000Gʹ��
	u16              m_wUSBKEYErrorCode; //����USBKEY��֤�Ĵ�����

	u16				 m_wCurrentHDMtNum;  //MPC�Ѿ�����ĸ����ն��� //  [7/28/2011 chendaiwei]
	u8				 m_abyHDMtData[MAX_CONFIDX][MAXNUM_CONF_MT/8]; //��¼�ն��Ƿ�ռ�ø������� //  [7/27/2011 chendaiwei]
	TDri2E1Cfg       m_atDri2E1CfgTable[MAXNUM_SUB_MCU];            //����Dri2��E1��������
	u16              m_wCurrentAudMtNum; //��ǰ�Ѿ�����������ն���     
	u8				 m_abyAudMtData[MAX_CONFIDX][MAXNUM_CONF_MT/8]; //��¼�ն��Ƿ�ռ����������� zjl 20120815
	CVmpInfoMgr      m_cVmpInfoMgr;		// ����ϳ���Ϣ����
#if defined(_8KH_) ||  defined(_8KE_) || defined(_8KI_)
	TNetCapMgr       m_tNetCapMgr;
#endif

	TNplusVmpModule	m_tNplusVmpModule[MAXNUM_PERIEQP];
	
public:
    
    // ��ȡMCU״̬
    BOOL32 GetMcuCurStatus( TMcuStatus &tMcuStatus );
	//���MCU ext ״̬
	BOOL32	GetMcuExtCurStatus( u8 byCurEqpId, u8 &byPeriEqpNum, TEqp *patPeriEqp, u8 *pbyEqpOnline, u32 *pdwPeriEqpIpAddr );
	
	u32	   GetMcuStartTick();
	void   SetMcuStartTick(u32 dwMcuStartTick);

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
	//zjj20091102 ��һ�������������ϵ�TConfStoreָ�룬������޸�ģ��Ͳ�Ϊ�գ�����ģ��ʱΪ��
	//Ϊ���������޸ĺ���������Ϊ�޸�ʱҪ�ѻ�����Ϣ�ļ��еĶ�̬�����������ó���(���ڽ���ʱvcs�����е�Ԥ��)
	BOOL32 SaveConfToFile( u8 byConfIdx, BOOL32 bTemplate, BOOL32 bDefaultConf = FALSE,TConfStore *ptOldConfStore = NULL );
	BOOL32 RemoveConfFromFile( CConfId cConfId );
	BOOL32 GetConfFromFile(u8 byConfIdx, TPackConfStore *ptPackConfStore);
	void   RefreshHtml( void );
	BOOL32 IsConfNameRepeat( LPCSTR lpszConfName, BOOL32 IsTemple ,BOOL32 bIsOngoingConf );
	BOOL32 IsConfE164Repeat( LPCSTR lpszConfE164, BOOL32 IsTemple ,BOOL32 bIsOngoingConf );
	BOOL32 IsE164Repeated( LPCSTR lpszE164, BOOL32 IsTemple,BOOL32 bIsOngoingConf  );
	BOOL32 IsConfOnGoing(CConfId cConfId);
	u8   GetOngoingConfIdxByE164( LPCSTR lpszConfE164 );
    u8   GetTemConfIdxByE164( LPCSTR lpszConfE164 );
    u16  GetConfRateByConfIdx( u16 wConfIdx, u16& wFirstRate, u16& wSecondRate, BOOL32 bTemplate = FALSE ); // ���ݻ������ȡ��������
    u8   GetConfNameByConfId( const CConfId cConfId, LPCSTR &lpszConfName );  // ���ݻ���IDȡ��������
    CConfId GetConfIdByName(LPCSTR lpszConfName, BOOL32 bTemplate  ,BOOL32 bIsOngoingConf);  // ���ݻ�������ȡCConfId
	CConfId GetConfIdByE164( LPCSTR lpszConfE164, BOOL32 bTemplate ,BOOL32 bIsOngoingConf); // ���ݻ���164��ȡCConfId

	void RegisterConfToGK( u8 byConfIdx, u8 byDriId = 0, BOOL32 bTemplate = FALSE, BOOL32 bUnReg = FALSE );
    void ConfChargeByGK( u8 byConfIdx, u8 byDriId, BOOL32 bStopCharge = FALSE, u8 byCreateBy = CONF_CREATE_MCS, u8 byMtNum = 0 );
    void ConfInfoMsgPack( CMcuVcInst *pcSchInst, CServMsg &cServMsg );//������Ϣ��Ϣ���

    CConfId MakeConfId( u8 byConfIdx, u8 byTemplate, u8 byUsrGrpId, u8 byConfSource = MCS_CONF );
    u32     GetMakeTimesFromConfId(const CConfId& cConfId) const;
    
    void SetInsState( u8 byInsId, BOOL32 bState );
    u8   GetIdleInsId( void );       

    BOOL32 IsSavingBandwidth(void);             //�Ƿ��ʡ����

	BOOL32 IsHoldDefaultConf(void);				//�Ƿ�֧���ٿ�ȱʡ����

	void    GetMcuEqpCapacity(TEqpCapacity& tMcuEqpCap); // get the mcu's cap of equipment
    void    GetConfEqpDemand(TConfInfo& tConfInfo, TEqpCapacity& tConfEqpDemand );
	BOOL32  AnalyEqpCapacity( const TConfInfo &tConfInfo, const TConfInfoEx &tConfInfoEx,TEqpCapacity& tConfEqpDemand, TEqpCapacity& tMcuSupportCap);
	void GetBasCapacity( TBasReqInfo& tBasCap ); // ȡBas������
	void GetPrsCapacity( TEqpReqInfo& tPrsCap ); // ȡPrs������    
    u8   GetConfFECType( u8 byConfIdx, u8 byMode );  // ��ǰ����ǰ�����
    u8   GetConfEncryptMode( u8 byConfIdx );         // ��ǰ�������������ģʽ

	//������ش�
	CMultiSpyMgr* GetCMultiSpyMgr( void );
    
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
    void   Msg2TemInfo( CServMsg &cMsg, TTemplateInfo &tTemInfo );
    void   TemInfo2Msg( TTemplateInfo &tTemInfo, CServMsg &cMsg );
    void   ShowTemplate( void );
    void   ShowConfMap( void );
	
    u8     GetTempEditerInfo(u8 byConfPos );
	void   SetTempEditerInfo(u8 byConfPos, u8 byInstId = 0);
	//����ģ����Ϣ�е���չ��Ϣ
	void UpdateExInfoInTemplate(TTemplateInfo &  tTemp,const emCommunicateType& byExenumType,const u16& wInfoLen,u8* pbyBuf);

    /************************************************************************/
    /*                                                                      */
    /*                     3��������������                                */
    /*                                                                      */
    /************************************************************************/
	void   InitPeriEqpList();
	void   SetPeriEqpConnected( u8 byEqpId, BOOL32 bConnected = TRUE, BOOL32 bIsValid = TRUE );
	BOOL32 IsPeriEqpConnected( u8 byEqpId );
	void   SetPeriEqpLogicChnnl( u8 byEqpId, u8 byMediaType, u8 byChnnlNum, const TLogicalChannel * ptStartChnnl, BOOL32 bForwardChnnl );
	BOOL32 GetPeriEqpLogicChnnl( u8 byEpqId, u8 byMediaType, u8 * pbyChnnlNum, TLogicalChannel * ptStartChnnl, BOOL32 bForwardChnnl );
	void   SetEqpRtcpDstAddr( u8 byEqpId, u8 byChnnl, u32 dwDstIp, u16 wDstPort, u8 byMode = MODE_VIDEO );
	BOOL32 GetEqpRtcpDstAddr( u8 byEqpId, u8 byChnnl, u32 &dwDstIp, u16 &wDstPort, u8 byMode = MODE_VIDEO );	
	u32    GetEqpIpAddr(u8 byEqpId) const;	// [12/18/2009 xliang] 8000E��
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
	//20100708_tzy ���ӻ�ȡAPU��������EAPU������,������С��������,����ȡһ�������ȸò�����Ļ�����
	u8     GetIdleEapuMixer(u8 byMinMemberNum,u8 *pbyMixMemberNum = NULL);
	u8     GetIdleApuMixer(u8 byMinMemberNum,u8 *pbyMixMemberNum = NULL);
	u8     GetIdleMixer(u8 byMinMemberNum, u8 byAudCapNum, TAudioTypeDesc *ptAudioTypeDesc); 
	u8     GetIdleApu2Mixer(u8 byMinMemberNum,u8 *pbyMixMemberNum = NULL );
	u8     GetIdle8kxMixer(u8 byMinMemberNum);
	//u8     GetIdleBasChl( u8 byAdaptType, u8 &byEqpId, u8 &byChlId );
    //BOOL32 GetIdleHDBasVidChl( u8 &byEqpId, u8 &byChlIdx );
	//BOOL32 IsIdleHDBasVidChlExist(u8* pbyEqpId = NULL, u8* pbyChlIdx = NULL);
	//BOOL32 IsIdleBasVidChlExist( u8 byAdaptType,u8* pbyEqpId = NULL, u8* pbyChlIdx = NULL );

//    void   GetIdleMau(u8 &byNVChn, u8 &byVGAChn, u8 &byH263pChn);
//    void   GetIdleMpu(u8 &byChnNum);
    void   GetMpuNum(u8 &byMpuNum);
//	void   GetIdle8keBas(u8 &byMvChnNum, u8 &byDsChnNum);//[03/01/2010] zjl add 

	BOOL32 IsMultiModeHdu(const u8 byHduId);

	//  [5/17/2013 guodawei]
	u8 GetEqpCodeFormat(const u8 byEqpId);
	BOOL32 SetEqpCodeFormat(u8 byEqpId, u8 byCodeFormat);

/*------------------------new bas-------------------------------*/
	//ͳ��bas��Դ
	void   AddMcuBasChnlInfo(const TEqp &tEqp, u8 byChnId);								
	//bas��Դ�Ƿ������������
	BOOL32 IsBasChnlEnoughForConf(const TConfInfo &tConfInfo, const TConfInfoEx &tConfInfoEx,  
								  CBasChn **ptBasChnConfNeed,  TBasChnCapData *ptBasChnCapData,u16 &wErrorCode);	
	//��ȡ����ı��������
	BOOL32 GetBasCapConfNeed(const TConfInfo &tConfInfo, const TConfInfoEx &tConfInfoEx, 
							 TNeedVidAdaptData *ptReqResource, TNeedAudAdaptData *pReqAudResource);	
	//�㲥ռ��basͨ��(�������reserved ������ reserved)
	BOOL32 OcuppyBasChn(CBasChn **aptBasChn, u8 byChnNum );	
	//����ͨ��ռ��(�������reserved ������ reserved)
	BOOL32 OcuppyBasChn(u8 byEqpId, u8 byChnId);
	//��ǰ����ͨ���Ƿ���������
	emBASCHNSTATUS GetBasChnStatus(TEqp tBas, u8 byChnId);
	//��������ͨ��״̬
    BOOL32 UpdateBasChnStatus(const TEqp &tEqp, u8 byChnId, emBASCHNSTATUS byState);
	//����Դ��Ŀ��������ȡ��������ͨ��
	BOOL32 GetIdleBasChn(TSimCapSet &tSrcCap, TSimCapSet &tDstCap, u8 byMediaMode, CBasChn **ppcBasChnData,  TBasChnCapData *ptBasChnCapData);
	BOOL32 GetIdleAudBasChn(TAudioTypeDesc &tAudSrcCap, TAudioTypeDesc&tAudDstCap,CBasChn  **ppcBasChnData);
	//����Դ��Ŀ��������ȡ��������ͨ��, �粻����ͨ�����ͱ�������ѭ����
	BOOL32 GetIdleBasChnPossible(TSimCapSet tSrcCap, TSimCapSet &tDstCap, u8 byMediaMode, CBasChn **ppcBasChnData,  TBasChnCapData *ptBasChnCapData);
	//���ݱ����������ȡ��Ӧ��������ͨ��
	BOOL32 IsBasChnEnoughForReq(TNeedVidAdaptData *ptReqResData, TNeedAudAdaptData *pReqAudResource, CBasChn **ptNeedChns,  TBasChnCapData*ptBasChnCapData);
	//���BASͨ����ַ
	CBasChn* GetBasChnAddr(const TEqp &tEqp, u8 byChnId);
	//��������������״̬
	void   SetBasOnLine(u8 byEqpId, BOOL32 bOnLine);
	//�ͷ�BASͨ��
	void   ReleaseBasChn(const TEqp &tEqp, u8 byChnId);
	//��ø���BASͨ��״̬
	BOOL32 GetHDBasChnStatus(const TEqp &tEqp, u8 byChnId, THDBasVidChnStatus &tChnStatus);
	//�����ƵBASͨ��״̬
	BOOL32 GetAudBasChnStatus(const TEqp &tEqp, u8 byChnId, TAudBasChnStatus &tChnStatus);

	//���ĳ��BASͨ���ڸ�ͨ�����������ϵ����ƫ��
	BOOL32 GetBasChnFrontOutPutNum(const TEqp &tEqp, u8 byChnId,  u8 &byOutNum, u8 &byFrontOutNum);
/*--------------------------------------------------------------*/

/*------------------------new vmp-------------------------------*/
	TVMPParam_25Mem GetConfVmpParam(TEqp tVmpEqp) { return m_cVmpInfoMgr.GetConfVmpParam(tVmpEqp); }
	void SetConfVmpParam(TEqp tVmpEqp, TVMPParam_25Mem tConfVmpParam) { m_cVmpInfoMgr.SetConfVmpParam(tVmpEqp, tConfVmpParam); }
	TVMPParam_25Mem GetLastVmpParam(TEqp tVmpEqp) { return m_cVmpInfoMgr.GetLastVmpParam(tVmpEqp); }
	void SetLastVmpParam(TEqp tVmpEqp, TVMPParam_25Mem tLastVmpParam) { m_cVmpInfoMgr.SetLastVmpParam(tVmpEqp, tLastVmpParam); }
	TKDVVMPOutParam GetVMPOutParam(TEqp tVmpEqp) { return m_cVmpInfoMgr.GetVMPOutParam(tVmpEqp); }
	void SetVMPOutParam(TEqp tVmpEqp, TKDVVMPOutParam tVMPOutParam) { m_cVmpInfoMgr.SetVMPOutParam(tVmpEqp, tVMPOutParam); }
	TVmpChnnlInfo GetVmpChnnlInfo(TEqp tVmpEqp) { return m_cVmpInfoMgr.GetVmpChnnlInfo(tVmpEqp); }
	void SetVmpChnnlInfo(TEqp tVmpEqp, TVmpChnnlInfo tVmpChnnlInfo) { m_cVmpInfoMgr.SetVmpChnnlInfo(tVmpEqp, tVmpChnnlInfo); }
	TVmpPriSeizeInfo GetVmpPriSeizeInfo(TEqp tVmpEqp) { return m_cVmpInfoMgr.GetVmpPriSeizeInfo(tVmpEqp); }
	void SetVmpPriSeizeInfo(TEqp tVmpEqp, TVmpPriSeizeInfo tVmpPriSeizeInfo) { m_cVmpInfoMgr.SetVmpPriSeizeInfo(tVmpEqp, tVmpPriSeizeInfo); }
	BOOL32 IsBrdstVMP(TEqp tVmpEqp) { return m_cVmpInfoMgr.GetConfVmpParam(tVmpEqp).GetVMPMode() != CONF_VMPMODE_NONE && m_cVmpInfoMgr.GetConfVmpParam(tVmpEqp).IsVMPBrdst(); }
	void SetVMPBrdst(TEqp tVmpEqp, u8 byVMPBrdst) { m_cVmpInfoMgr.SetVMPBrdst(tVmpEqp, byVMPBrdst); }
	u8 GetVMPMode(TEqp tVmpEqp) { return m_cVmpInfoMgr.GetConfVmpParam(tVmpEqp).GetVMPMode(); }
	void SetVMPMode(TEqp tVmpEqp, u8 byMode) { m_cVmpInfoMgr.SetConfVmpMode(tVmpEqp, byMode); }
	void ClearVmpInfo(TEqp tVmpEqp) { m_cVmpInfoMgr.ClearVmpInfo(tVmpEqp); }
	u16 GetVMPOutChlBitrate(TEqp tVmpEqp, u8 byIdx) {return m_cVmpInfoMgr.GetVMPOutChlBitrate(tVmpEqp, byIdx);}
	void SetVMPOutChlBitrate(TEqp tVmpEqp, u8 byIdx, u16 wBr) { m_cVmpInfoMgr.SetVMPOutChlBitrate(tVmpEqp, byIdx, wBr); }
	void ClearVmpAdpHdChl(TEqp tVmpEqp) { m_cVmpInfoMgr.ClearVmpAdpHdChl(tVmpEqp); }
	u8 GetVmpAdpMaxStyleNum(TEqp tVmpEqp) { return m_cVmpInfoMgr.GetVmpAdpMaxStyleNum(tVmpEqp); }
	u8 GetVmpAdpHDChlNum(TEqp tVmpEqp) { return m_cVmpInfoMgr.GetVmpAdpHDChlNum(tVmpEqp); }
	u8 GetVmpAdpMaxNumHdChl(TEqp tVmpEqp) { return m_cVmpInfoMgr.GetVmpAdpMaxNumHdChl(tVmpEqp); }
	u8 GetVmpAdpChnlCountByMt(TEqp tVmpEqp, TMt tMt) { return m_cVmpInfoMgr.GetVmpAdpChnlCountByMt(tVmpEqp, tMt); }
/*--------------------------------------------------------------*/

//    u8     GetIdlePrsChl( u8 &byEqpId, u8 &byChlId, u8 &byChlId2, u8 &byChlIdAud );
//	BOOL32 GetIdlePrsChls( u8 byChannelNum, TPrsChannel& tPrsChannel);
	//prs����
	BOOL32 GetIdlePrsChls(u8 byNeedChnNum, u8 *pbyPrsId = NULL, u8 *pbyPrsChnId = NULL);
	u8     GetAllIdlePrsChnsNum();

	u8     GetMaxIdleChlsPrsId(u8& byEqpPrsId );
//	BOOL32 GetIdlePrsChls(u8 byPrsId, u8 byChannelNum, TPrsChannel& tPrsChannel);
    u8     GetIdlePrsChl( u8 &byEqpId, u8 &byChlId );
    BOOL32 IsRecorderOnline (u8 byEqpId);
	void   SetEqpAlias( u8 byEqpId, LPCSTR lpszAlias );
	LPCSTR GetEqpAlias( u8 byEqpId );
//    BOOL32 IsEqpH263pMau( u8 byEqpId );

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

	
	void UpdateAgentEqpStat(u8 byEqpType);
	void UpdateAgentAuthMtNum();


	//�������Ͳ���
	u8 GetHduChnNumAcd2Eqp(const TEqp tEqp);
	BOOL32 Set8kxMixerCfg(TEqpRegAck &tRegAck);
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

	//��������
	//��ȡ����������ͨ����
	u8     GetBasInPutChnNum(const TEqp &tEqp);
	//���ö�Ӧ����ͨ������
// 	BOOL32 SetBasAdaptParam(const TConfInfo& tConfInfo,
// 							const TEqp &tEqp, u8 byChnId, 
// 							THDAdaptParam tHDParm, BOOL32 bForceSet = FALSE, BOOL32 bAdjRes = FALSE);
	//��ն�Ӧ����ͨ������
	BOOL32 ClearAdaptParam(const TEqp &tEqp, u8 byChnId, u8 byMediaMode);
	//��ȡ��Ӧ����ͨ������
	BOOL32 GetBasOutPutParam(const TEqp &tEqp, u8 byChnId, u8 &byOutNum,  THDAdaptParam *ptParam);
	BOOL32 GetAudBasOutPutParam(const TEqp &tEqp, u8 byChnId, u8 &byOutNum,  TAudAdaptParam *ptParam);
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
	BOOL32 GetMcsRegInfo( u16 wMcInstId, TMcsRegInfo *ptMcsRegInfo );
	u16    GetMcIns(u32 dwMcIp, u32 dwMcSSrc, u8 byConfSource = MCS_CONF);

	/************************************************************************/
    /*                                                                      */
    /*                     6B����ر��������								*/
    /*                                                                      */
    /************************************************************************/
	BOOL32 GetMonitorSrc( u16 wMcInstId, u8 byMode, const TTransportAddr &tTransportAddr, TMt * ptSrc );
	BOOL32 SetMonitorSrc( u16 wMcInstId, u8 byMode, const TTransportAddr &tTransportAddr, const TMt &tSrc);
	BOOL32 GetMonitorData( u16 wMcInstId, u16 wIdx, TMonitorData &tMonitorData);
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
	void	ReAssignMpForEqp(u8 byEqpId);
	void	AssignNewMpToReplace(u8 byOldMpId);
	u8		GetNextValidMp(u8 byMpId);
	void	ChkAndRefreshMpForEqp(u8 byEqpId);
    
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
//	void IncMtAdpMtNum( u8 byDriId, u8 byConfIdx, u8 byMtId, u16 wIncExtraNum = 0);
    void IncMtAdpMtNum( u8 byDriId, u8 byConfIdx, u8 byMtId, u8 byMtType = TYPE_MT);
	void DecMtAdpMtNum( u8 byDriId, u8 byConfIdx, u8 byMtId, BOOL32 bHDMt = FALSE
		                /*BOOL32 bOnlyAlterNum = FALSE, BOOL32 bIsMcu = FALSE */);

	BOOL32 IncAudMtAdpNum(u8 byConfIdx, u8 byMtId, u8 byMtHasVidCap, u16& wErrorCode);
	BOOL32 DecAudMtAdpNum(u8 byConfIdx, u8 byMtId);

	void ChangeDriRegConfNum( u8 byDriId, BOOL32 bInc  );
	u8   GetRegConfDriId( void );
    u8   AssignH323MtDriId( u8 byConfIdx, TMtAlias &tMtAlias, u8 byMtId );
	u8   AssignH320MtDriId( u8 byConfIdx, TMtAlias &tMtAlias, u8 byMtId );
#if	defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	u8   AssignH323MtDriIdFor8KE( u8 byConfIdx, TMtAlias &tMtAlias, u8 byMtId );
#endif
	void ShowDri( u8 byDriId );
    BOOL32 IsMtAssignInDri( u8 byDriId, u8 byConfIdx, u8 byMtId );

	u16	 GetMpcHDAccessNum();

	BOOL32 IncMpcCurrentHDMtNum (u8 byConIdx, u8 byMtId,BOOL32 bIsMcu = FALSE );
	BOOL32 DecMpcCurrentHDMtNum (u8 byConIdx, u8 byMtId,BOOL32 bIsMcu = FALSE );
	BOOL32 IsOccupyHDAccessPoint (u8 byConIdx, u8 byMtId);
	BOOL32 IsOccupyAudAccessPoint(u8 byConIdx, u8 byMtId);

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
  
	void   SetRRQDriTransed( BOOL32 bTrans ) { m_bRRQDriTransferred = bTrans;   }
    BOOL32 GetRRQDriTransed( void ) { return m_bRRQDriTransferred;   }
   
    /************************************************************************/
    /*                                                                      */
    /*                     12��Debug���Լ�����������Ϣ                      */
    /*                                                                      */
    /************************************************************************/   
    
	// 1. ��ȡ����������Ϣ
	void   GetBaseInfoFromDebugFile( );
	BOOL32 IsTelnetEnable();
	u16    GetMcuTelnetPort( );
	u16    GetMcuListenPort( );
	u32    GetMcsRefreshInterval( );

	u32	   GetRefreshMtInfoInterval( ) const { return m_tMcuDebugVal.m_dwRefressMtInfoInterval; }
	u32	   GetRefreshSMcuMtInterval( ) const { return m_tMcuDebugVal.m_dwRefreshSMcuMtInterval; }
	u32	   GetRefreshMtAdpSMcuListInterval() const { return m_tMcuDebugVal.m_dwRefreshMtAdpSMcuList; }
	
	u8	   GetMcuMcsMtStatusBufFlag( ) const { return m_tMcuDebugVal.m_byEnableMcuMcsMtStatusBuf; }
	u8	   GetMcuMcuMtStatusBufFlag( ) const { return m_tMcuDebugVal.m_byEnableMcuMcuMtStatusBuf; }
	u8	   GetMtAdpSMcuListBufFlag( ) const { return m_tMcuDebugVal.m_byEnableMtAdpSMcuListBuf; }
	u8	   GetMcuMcsMtInfoBufFlag( ) const { 
		// �ն�״̬�������ն���Ϣ���б�֮����! �������û�������ն��б�Ļ��巢��, ��������״̬��Ϣ���б�Ļ��巢��
		u8 byFlagRet = m_tMcuDebugVal.m_byEnableMcuMcsMtInfoBuf 
			& m_tMcuDebugVal.m_byEnableMcuMcsMtStatusBuf;
		return byFlagRet; 
	}
	
	BOOL32 IsWatchDogEnable( );
    BOOL32 GetMSDetermineType( );
	u16	   GetMsSynTime(void);	//�������ͬ��ʱ��, zgc, 2007-04-02
    BOOL32 GetBitrateScale();

    BOOL32 GetDri2E1CfgTableFromFile();
    BOOL32 GetDri2E1CfgTable( TDri2E1Cfg* tDri2E1CfgTable );
	void SaveRealBandwidth(u16 wRealBandWidth, u8 byIdx, u8 byE1Num);
    u32 GetCfgIpByIdx( u8 byIndx );
	BOOL32 GetRealBandwidthByIp(u32 dwIp, u16 &wRealBandwidth);

	
	BOOL32 GetFakeCapParamByMtInfo(u32 dwIp, s8* pszProductId,u8 byManuId,TSimCapSet &tMainCapSet, TDStreamCap &tDSCapSet,TAudioTypeDesc & tAudioDesc, u8& byFakeMask);
	BOOL32 GetFakeCapIdByMtInfo(u32 dwIp, s8* pszProductId,u8 byManuId, u8 &byMainCapId, u8 &byDualCapId,u8 & byAudioCapId);

    //��ȡ��ʱ����
    u32 GetPinHoleInterval(void){ return m_tMcuDebugVal.m_dwPinHoleInterval; }
   
    // 2. ��ȡ����ָ���ն˵�Э���������Դ������ת������Դ������
	BOOL32 GetMtCallInterfaceInfoFromDebugFile( );
	BOOL32 GetMpIdAndH323MtDriIdFromMtAlias( TMtAlias &tMtAlias, u32 &dwMtadpIpAddr, u32 &dwMpIpAddr );
	    
    
    // 3���Ӽ���SMcu�Ķ˿ڣ��¼��������˿ڣ�
    BOOL32 IsMMcuSpeaker() const;
    BOOL32 IsAutoDetectMMcuDupCall() const;
    BOOL32 IsShowMMcuMtList() const;
	BOOL32 IsStartSwitchToMMcuWithOutCasBas() const;//[03/04/2010] zjl modify (����������Դ��ֱ�ӽ����������ϼ�mcu)
    u8     GetCascadeAliasType() const;
	u8     GetCascadeAdminLevel( ) const;
    void   SetSMcuCasPort(u16 wPort);
    u16    GetSMcuCasPort() const;

    // 4. ͨ�ò���        
	BOOL32 IsApplyChairToZxMcu() const;
    BOOL32 IsTransmitMtShortMsg() const;
    BOOL32 IsChairDisplayMtApplyInfo() const;
    BOOL32 IsSelInDoubleMediaConf() const;
    BOOL32 IsLimitAccessByMtModal() const;
	u8	   GetDbgConfFPS() const;
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
    u8     GetBandWidthReserved4HdBas() const;
    u8     GetBandWidthReserved4HdVmp() const;    
	BOOL32 IsSupportChgLGCResAcdBR() const;
	u16    Get1080P6050FpsBpLeastBR() const;
	u16    Get1080P3025FpsBpLeastBR() const;
	u16	   Get720P6050FpsBpLeastBR() const;
	u16    Get720P3025FpsBpLeastBR() const;
	u16    Get1080P6050FpsHpLeastBR() const;
	u16    Get1080P3025FpsHpLeastBR() const;
	u16    Get720P6050FpsHpLeastBR() const;
	u16    Get720P3025FpsHpLeastBR() const;
    u16    Get4CifLeastBR() const;
    u16    GetCifLeastBR() const;
	BOOL32 GetFakeCap2HuaweiSDEp( const u32 dwMtIp,u8 &byVideoFormat ) const;
	
	u8     GetVcsMtOverTimeInterval() const;//��ȡvcs������г�ʱʱ��
	BOOL32 IsVASimultaneous() const;
	
	u16		GetMmcuOlcTimerOut() const;		//��ȡ�������߼�ͨ����ʱʱ��
	u8		GetFastUpdateToSpeakerNum() const;
    // 5. �������Ʋ���
    BOOL32 SetLicenseNum( u16 wLicenseValue );
    u16    GetLicenseNum( void );

	BOOL32 SetAudMtLicenseNum(u16 wAudLicenseValue);
	u16    GetAudMtLicenseNum(void);
	u16    GetMpcCurrentAudMtAdpNum(void);

	BOOL32 SetVCSAccessNum( u16 wAccessNum );
	u16    GetVCSAccessNum( void );
	u8     GetMaxMcsOngoingConfNum(void) const;
	
	BOOL32	SetLicenseHDMtNum(u16 wLicenseValue);
	u16		GetLicenseHDMtNum(void);
	BOOL32	SetLicensePCMtNum(u16 wLicenseValue);
	u16		GetLicensePCMtNum(void);
	
    void   SetMcuExpireDate( TKdvTime &tExpireDate );
    TKdvTime GetMcuExpireDate( void );
    BOOL32 IsMcuExpiredDate(void);

    BOOL32 IsMpcRunMp();
    TMcuPerfLimit& GetPerfLimit();
    u8 GetFpsAdp();
	BOOL32 IsChairAutoSpecMix();

	//[add][liulijiu][2010.07.15]
	BOOL32 ReadLicenseFromUSBKEY(u16 &wErrorCode);    //��֤USBKEY�����Ȩ�����Ƿ���ȷ
	void   SetLicesenUSBKEYFlag(void);                //����USBKEY flag
	BOOL32 GetLicesenUSBKEYFlag(u16 &wErrorCode);     //�õ�USBKEY flag
	//[add][liulijiu][2010.07.15] end

    BOOL32 IsMtNumOverLicense( );         //�Ƿ񳬹�License���Ƶ�����ն˽�������
	BOOL32 IsVCSNumOverLicense(void);     //�Ƿ񳬹�License���Ƶ����VCS��������
    BOOL32 IsConfNumOverCap( BOOL32 bOngoing, u8 byConfSource = MCS_CONF );           //���������Ƿ���
    u8     GetMcuCasConfNum();                            //��ȡ������������
    BOOL32 IsCasConfOverCap();                            //���������Ƿ���
	//  [12/22/2010 chendaiwei]Fix me: �ú���Ӧ��ֻ���ڷ���ĳ��DRI MtAdp�ϵ��ն˻���Mcu����
    BOOL32 GetMtNumOnDri( u8 byDriId, BOOL32 bOnline, u8 &byMtNum, u8 &byMcuNum );    //���ĳ��DRI MtAdp�ϵ��ն˻���Mcu�����������Ƿ���
	//  [12/22/2010 chendaiwei] ���ĳ��DRI MtAdp�ϵ��ն˻���Mcu�����������Ƿ���
	BOOL32 IsMtNumOverDriLimit( u8 byDriId, BOOL32 bOnline, u8 &byMtNum, u8 &byMcuNum );    
	u16	   GetPcmtAccessPointNum(void);
	u16	   GetAccessPointNum( void );
	BOOL32 IsMtNumOverMcuLicense(u8 byConfIdx, TMt tMt, const TCapSupport &tCapSupport, u16 &wErrorCode);
	BOOL32 IsHdMtNumOverMcuLicense( u8 byConfIdx, const TMt &tMt, const TCapSupport &tCapSupport, u16 &wErrorCode);
	u8 GetMixerWaitRspTimeout(void);
	u8 GetDelayReleaseConfTime(void);
	u8 GetMtFastUpdateInterval( void ) const;
	u8 GetMtFastUpdateNeglectNum( void ) const;
	BOOL32 IsPolyComMtCanSendDoubleStream( void ) const;
	BOOL32 IsChaoRanMtAllowInVmpChnnl( void ) const;
	BOOL32 IsSendStaticText( void ) const;

	
	BOOL32 IsNeedMapDstIpAddr(u32 dwDstIpAddr)const; //������Ŀ��ip�Ƿ���Ҫ�����٣�����mapip�Ͷ˿�
	BOOL32 IsNeedSendSelfMutePack(u8 byManuId,const s8* pProductId);//��Ӧ��manuid��productid�Ƿ���Ҫ���Լ���������
	u8 GetSendMutePackNum ( void ) const;
	u8 GetSendMutePackInterval ( void ) const;
	BOOL32 IsRcvH264DependInMark(u8 byManuId,const s8* pProductId);//��Ӧ��manuid��productid�Ƿ���Ҫ�����ⷽʽ����
	BOOL32 IsReverseG7221c(u8 byManuId,const s8* pProductId);//��Ӧ��manuid��productid�Ƿ���Ҫ��g7221.c��Ƶ��ʽ����������ż�Ի�

	u16 GetCurrentAudMtNum (void);

    BOOL32 IsMultiCastMtIp(u32 dwIpAddr)const; //�Ƿ�����鲥����

    // 6. ��ӡ
    void ShowDebugInfo();
    void ShowBasInfo();
	void ShowBasList();
	void ShowTW(u8 byHduId);
	void ShowPrsInfo();
	BOOL32 IsCanConfCapabilityCheat( void );
	
	BOOL32 IsMtBoardTypeCanCapabilityCheat( s8* pachProductID );

	BOOL32 IsHDMt( s8* pachProductID );

	BOOL32 IsLowCalledMtChlOpenMp4( void );
	BOOL32 IsLowCalledMtChlOpenH264( void );
    
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
    //sgx
	BOOL32 VcsReloadMcuUserList( void );  
	
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

	//ץ�����
#if defined(_8KH_) ||  defined(_8KE_) || defined(_8KI_)

	void SetNetCapStatus( const emNetCapState& emNetCapStatus);
	emNetCapState GetNetCapStatus()const;

	void SetOprNetCapMcsInstID(const u8& byMcsInstId);

	u8 GetOprNetCapMcsInstID()const;

	BOOL32 StartNetCap(const u8& byMcsInstId);

	BOOL32 StopNetCap();
#endif

public:
	BOOL32 MsgPassCheck( u16 wEvent, u8 * const pbyMsg, u16 wLen = 0 );
	void   BroadcastToAllConf( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	BOOL32 SendMsgToConf( u8 byConfIdx, u16 wEvent, u8 * const pbyMsg, u16 wLen );
	BOOL32 SendMsgToConf( const CConfId & cConfId, u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	BOOL32 SendMsgToDaemonConf( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	BOOL32 SendMsgToEqp( u8 byEqpId,  u16 wEvent, u8 * const pbyMsg, u16 wLen );
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
	BOOL32 GetVcDeamonOtherData( u8 *pbyBuf, u32 &dwOutBufLen );
	BOOL32 SetVcDeamonOtherData( u8 *pbyBuf );

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
    BOOL32 GetAddrbookFileDataUtf8( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr );
	BOOL32 SetAddrbookFileDataUtf8( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr );
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

	void ConstructCriMacInfoMsg(CServMsg & cMsg); //��������MAC��ַ��Ϣ��Ϣ��[5/16/2012 chendaiwei]
	BOOL32 FindMatchedMpIpForMt(u32 dwMtIp, u32 &dwMpIp); //����Mt����ת�����IP��ַ����IP��ַӦ��Mt IPͬ���Σ����8000H/G/I��[5/25/2012 chendaiwei]
	
private: 
	u32  OprVcDeamonOtherData( u8 *pbyBuf, BOOL32 bGet );
//	u32  GetVcDeamonOtherDataLen( void );


    /************************************************************************/
    /*                                                                      */
    /*                     15.N+1��ع���
    /*                                                                      */
    /************************************************************************/ 
public:
	BOOL32 NplusSaveVmpModuleByConfIdx ( u8 byConfIdx, TNplusVmpModule *ptVmpMod, u8 byModNum);
	void NplusRemoveVmpModuleByConfIdx ( u8 byConfIdx );
	void NplusRemoveVmpModuleEqpId ( u8 byEqpId );
	BOOL32 NPlusIsMtInVmpModule(u8 byLoopChIx, u8 byMtId, u8 byConfIdx, u8 byEqpId, u8 &byMember);
	void NplusRemoveVmpMember( u8 byMtId,u8 byConfIdx,u8 byEqpId = 0);
	BOOL32 NPlusIsVmpOccupyByConf (u8 byEqpId,u8 byConfIdx);
	void GetNPlusVmpMember(u8 byEqpId,u8 byChIdx,u8 &byMtId,u8 &byMemberType);
};
#endif

//END OF FILE





