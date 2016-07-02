/*****************************************************************************
   ģ����      : mcu
   �ļ���      : mcuinnerstruct.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: MCU�ڲ�ʹ�õĽṹ
   ����        : �´�ΰ
   �汾        : V4.7  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2012/05/17  4.7         chendaiei    ����
******************************************************************************/
#ifndef __MCUINNERSTRUCT_H_
#define __MCUINNERSTRUCT_H_

#include "osp.h"
#include <string.h>
#include <stdlib.h>
#include "kdvsys.h"
#include "mcuconst.h"
#include "kdvdef.h"
#include "vccommon.h"
#include "mcustruct.h"

#ifdef WIN32
    #pragma comment( lib, "ws2_32.lib" ) 
    #pragma pack( push )
    #pragma pack( 1 )
    #define window( x )	x
#else
    #include <netinet/in.h>
    #define window( x )
#endif

#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
#define  MAXNUM_CONF_PRS_CHNNL			30				//����ʹ�õ����PRSͨ��
#else
#define  MAXNUM_CONF_PRS_CHNNL			32				//����ʹ�õ����PRSͨ��
#endif

#define  MPU2_BASCHN_MAXENCABILITY		3000				//MPU2 BAS����������
#define	 MPU2_BASCHN_DIVISION_ABILITY	1000				//MPU2 BAS�����ֶ�����(��Ӧý�ز㰴ÿ1000�����ֶ�)
#define	 MPU2_BASCHN_ENCARRAY_NUM		3					//MPU2_BASCHN_MAXENCABILITY/MPU2_BASCHN_DIVISION_ABILITY
#define  MCS_MAXNUM_PLAN                8					//MCS���Ԥ����
#define  MAXNUM_CONF_VMP                16					//����֧�ֵ����vmp����
#if defined _LINUX
#endif

#ifdef _VXWORKS_
	#ifdef _MINIMCU_
		#define     MAXNUM_MCU_SCHEDULE_CONF         4   //ԤԼ��������ܺ� 
	#else
		#define     MAXNUM_MCU_SCHEDULE_CONF            4  //ԤԼ��������ܺ� 
	#endif 
#elif defined(_LINUX_)
	#ifdef _MINIMCU_
		#define     MAXNUM_MCU_SCHEDULE_CONF            4 //ԤԼ��������ܺ� 
	#else
		#ifdef _LINUX12_
			#define     MAXNUM_MCU_SCHEDULE_CONF        16 //MPC2 //ԤԼ��������ܺ� 
		#else
			#define     MAXNUM_MCU_SCHEDULE_CONF            4  //ԤԼ��������ܺ� 
		#endif 
	#endif
#else   // WIN32
	#define     MAXNUM_MCU_SCHEDULE_CONF                4 //ԤԼ��������ܺ� 
#endif


enum emFakeMask                      //�ն����������ٵ�maskֵ
{
	emMainStreamFakedMask = 0x01,   //����Ƶ����������mask
		emDoubleStreamFakedMask = 0x02, //˫������������mask
		emAudioDescFakedMask = 0x04     //��Ƶ����������mask
};

#define IS_MAINSTREAMCAP_FAKE(byMask)   !((byMask & emMainStreamFakedMask) == 0)									
#define IS_DOUBLESTREAMCAP_FAKE(byMask)   !( (byMask & emDoubleStreamFakedMask) == 0)
#define IS_AUDIODESC_FAKE(byMask)  !((byMask & emAudioDescFakedMask) == 0)

#define SET_MAINSTREAM_FAKED(byMask)  byMask |= emMainStreamFakedMask;
#define SET_DOUBLESTREAM_FAKED(byMask)  byMask |= emDoubleStreamFakedMask;
#define SET_AUDIODESC_FAKED(byMask)  byMask |= emAudioDescFakedMask; 


/*------------------------------------------------------------- 
*�����鳣�� */
#define KEY_mcunetIsGKRRQUsePwd					(const s8*)"mcunetIsGkUseRRQPwd"
#define KEY_mcunetGKRRQPwd					    (const s8*)"mcunetGKRRQPwd"

//DMZ�������ó���
#define SECTION_ProxyLocalInfo                  (const s8*)"LocalInfo"
#define KEY_mcuUseDMZ                           (const s8*)"UseDmz"
#define KEY_mcuIpDmzAddr                        (const s8*)"IpDmzAddr"

// [pengguofeng 4/12/2013] ���뷽ʽ������������/conf/xxx.ini
#define SECTION_McuEncode						(const s8*)"mcuEncode"
#define KEY_encoding							(const s8*)"encoding"

//8KIVMP����ǰ����ͨ����Ŀ
#define		MAXNUM_8KIVMP_ADPHDCHL_2				(u8)2	//8KIVMP����ǰ����ͨ����Ŀ
#define		MAXNUM_8KIVMP_ADPHDCHL_4				(u8)4	//8KIVMP����ǰ����ͨ����Ŀ
#define		MAXNUM_8KIVMP_ADPHDCHL_9				(u8)9	//8KIVMP����ǰ����ͨ����Ŀ
#define		MAXNUM_8KIVMP_ADPHDCHL_16				(u8)16	//8KIVMP����ǰ����ͨ����Ŀ

struct TMixMsgInfo
{
private:
	u8 m_byMtNum;
	u8 m_byIsMMcuMix;
	u8 m_byUseMixerEqpId;
	TMt *m_ptMtArray;
	u16 m_wErrorCode;
	u8  m_bySpecMixReplace;
	
public:
	TMixMsgInfo ( void )
	{
		Clear();
	}
	
	void Clear( void )
	{
		memset(this,0,sizeof(*this));
	}
	
	void SetMixMtNum ( u8 byMtNUm )
	{
		m_byMtNum = byMtNUm;
	}

	u8	GetMixMtNum ( void )
	{
		return m_byMtNum;
	}
	
	u8 GetEqpId ( void )
	{
		return m_byUseMixerEqpId;
	}

	void SetEqpId ( u8 byEqpId )
	{
		m_byUseMixerEqpId = byEqpId;
	}
	
	BOOL32 IsCascadeMixMsg ( void )
	{
		return (m_byIsMMcuMix == 1);
	}

	void SetCascadeMixMsg ( void )
	{
		m_byIsMMcuMix = 1;
	}
	
	emMcuMixMode GetMixMode ( void )
	{
		return m_byMtNum > 0 ? mcuPartMix:mcuWholeMix;
	}

	void SetMixMemberPointer ( TMt *pMt)
	{
		m_ptMtArray = pMt;
	}
	
	TMt * GetMixMemberPointer ( void )
	{
		return m_ptMtArray;
	}

	void SetErrorCode ( u16 wErrorcode)
	{
		m_wErrorCode = wErrorcode;
	}

	u16 GetErrorCode ( void )
	{
		return m_wErrorCode;
	}

	void SetReplaceMemberFlag( u8 bySpecMixReplace)
	{
		m_bySpecMixReplace = bySpecMixReplace;
	}

	u8 GetReplaceMemberFlag ( void )
	{
		return m_bySpecMixReplace;
	}
};

//MtAdpע����Ϣ
struct TMtAdpReg
{
protected:    
	u8      m_byDriId;         //��MtAdp���
	u8      m_byAttachMode;    //MtAdp�ĸ�����ʽ
	u8      m_byProtocolType;  //�����Э������//H323��H320��SIP
	u8      m_byMaxMtNum;      //�����֧������ն���
	u32     m_dwIpAddr;        //Ip��ַ��������
	s8      m_achMtAdpAlias[MAXLEN_ALIAS];  //MtAdp����
    u16     m_wQ931Port;
    u16     m_wRasPort;
	//4.0R4��չ�ֶ�
    u16     m_wVersion;     //�汾��Ϣ	ZGC	2007-09-28
    //4.0R5
	u8      m_byMaxHDMtNum;   //Mtadp֧�ֵ������������� //  [7/28/2011 chendaiwei]
	u8		m_abyMacAddr[6];  //Mtadp�����ĵ����MAC��ַ[5/16/2012 chendaiwei]
	u16     m_wMaxAudMtNum;   //������������ն���

public:
	TMtAdpReg()
	{
		memset(this, 0, sizeof(TMtAdpReg)); 
		m_byProtocolType = PROTOCOL_TYPE_H323;
		m_byMaxMtNum = MAXNUM_DRI_MT;
	}
	
	VOID	SetDriId(u8 byDriId){ m_byDriId = byDriId;}
	u8  	GetDriId() const { return m_byDriId;}
	VOID	SetAttachMode(u8 byAttachMode){ m_byAttachMode = byAttachMode;} 
	u8  	GetAttachMode( void ) const { return m_byAttachMode; }
	VOID	SetIpAddr(u32 dwIp){m_dwIpAddr = htonl(dwIp);}
	u32   	GetIpAddr() const { return ntohl(m_dwIpAddr);}
	VOID	SetProtocolType(u8 byProtocolType){ m_byProtocolType = byProtocolType;}
	u8  	GetProtocolType() const { return m_byProtocolType;}
	VOID	SetMaxMtNum(u8 byMaxMtNum){ m_byMaxMtNum = byMaxMtNum;}
	u8  	GetMaxMtNum() const { return m_byMaxMtNum;}
    void    SetQ931Port(u16 wQ931Port){ m_wQ931Port = htons(wQ931Port);} 
    u16     GetQ931Port( void ) const { return ntohs(m_wQ931Port); }
    void    SetRasPort(u16 wRasPort){ m_wRasPort = htons(wRasPort);} 
    u16     GetRasPort( void ) const { return ntohs(m_wRasPort); }	

	VOID    SetMaxHDMtNum(u8 byMaxHdMtNum){ m_byMaxHDMtNum = byMaxHdMtNum; }
	u8      GetMaxHDMtNum() const{ return m_byMaxHDMtNum; }

	VOID    SetMaxAudMtNum(u16 wMaxAudMtNum){m_wMaxAudMtNum = wMaxAudMtNum;}
	u16     GetMaxAudMtNum(){ return m_wMaxAudMtNum; }

	VOID SetAlias(LPCSTR lpszAlias)
	{
		if(lpszAlias != NULL)
			strncpy(m_achMtAdpAlias, lpszAlias, MAXLEN_ALIAS - 1);
	}

	VOID GetAlias(s8 *pchBuf, u16  wBufLen)
	{
		if(pchBuf != NULL)
			strncpy(pchBuf, m_achMtAdpAlias, wBufLen);
	}

	u16 GetVersion(void) const { return ntohs(m_wVersion); }
	void SetVersion(u16 wVersion) { m_wVersion = htons(wVersion); }
	void GetMacAddr( u8 *pabyMacAddr) const{ if(pabyMacAddr != NULL) memcpy(pabyMacAddr,m_abyMacAddr,sizeof(m_abyMacAddr));}
	void SetMacAddr( u8 *pabyMacAddr ){ if(pabyMacAddr != NULL) memcpy(m_abyMacAddr,pabyMacAddr,sizeof(m_abyMacAddr));}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


struct TProxyDMZInfo
{
private:
	u8   m_byIsUseDMZ;						//�Ƿ�����DMZ
	u32  m_dwDMZIp;							//������DMZ IP���ã�
	u8   m_byEthIndx;						//DMZ���õ�����indx
public:
	TProxyDMZInfo ( void )
	{
		memset(this,0,sizeof(*this));
	}

	~TProxyDMZInfo ( void )
	{
		memset(this,0,sizeof(*this));
	}

	void SetIsUseDMZ ( u8 byIsUseDMZ )
	{
		m_byIsUseDMZ = byIsUseDMZ;
	}

	BOOL32 IsUseDMZ ( void )
	{
		return (m_byIsUseDMZ == 1);
	}
	
	//dwIP ������
	void SetDMZIpAddr ( u32 dwIp)
	{
		m_dwDMZIp = htonl(dwIp);
	}
	
	//����������
	u32 GetDMZIpAddr ( void )
	{
		return ntohl(m_dwDMZIp);
	}

	void SetDMZEthIndx ( u8 byEthIndx)
	{
		m_byEthIndx = byEthIndx;
	}
	
	u8 GetDMZEthIndx ( void )
	{
		return m_byEthIndx;
	}

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//MCU����������Ϣ
//�̳���MCU general��Ϣ����չGK�û���������[3/19/2013 chendaiwei]
struct TMcuNewGeneralCfg: public TMcuGeneralCfg
{
public:
	TMcuNewGeneralCfg ( void ){memset(this,0,sizeof(*this));}
	~TMcuNewGeneralCfg (){memset(this,0,sizeof(*this));}
	void SetGkRRQUsePwdFlag( u8 byIsUseGKPwd)
    {
		m_byIsGkUseRRQPwd = byIsUseGKPwd;
    }
    
    u8	GetGkRRQUsePwdFlag() const
    {
        return m_byIsGkUseRRQPwd;
	}
	
    void SetGkRRQPassword(LPCSTR lpszAlias)
    {
        if(lpszAlias != NULL)
            strncpy(m_achRRQPassword, lpszAlias, GK_RRQ_PWD_LEN);
    }
    
    LPCSTR GetGkRRQPassword() const
    {
        return m_achRRQPassword;
	}

	void SetMcuGeneralCfg(TMcuGeneralCfg *ptGenralCfg)
	{
		if( ptGenralCfg != NULL)
		{
			memcpy(this,ptGenralCfg,sizeof(*ptGenralCfg));
		}
	}

	void SetDMZInfo ( TProxyDMZInfo *ptDMz)
	{
		if(ptDMz != NULL)
		{
			memcpy(&m_tProxyDMZInfo,ptDMz,sizeof(m_tProxyDMZInfo));
		}
	}

	TProxyDMZInfo GetDMZInfo ( void )
	{
		return m_tProxyDMZInfo;
	}

	void Print( void )
	{
		TMcuGeneralCfg::Print();
		StaticLog("Is GK RRQ Use Pwd:%d\n",m_byIsGkUseRRQPwd);
		StaticLog("GK RRQ password:%s\n",m_achRRQPassword);
		StaticLog("IsUseDMZ:%d\n",m_tProxyDMZInfo.IsUseDMZ());
		StaticLog("DMZ IP:0x%x\n",m_tProxyDMZInfo.GetDMZIpAddr());
	}

public:
	u8	 m_byIsGkUseRRQPwd;					//�Ƿ�����GK����RRQע��
	s8	 m_achRRQPassword[GK_RRQ_PWD_LEN];	//GK RRQ��Կ
	TProxyDMZInfo m_tProxyDMZInfo;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//ÿ��ip��Ϣ
struct TNetCfg:public TIpRouteCfg
{
protected:
    s8 m_achName[MAXLEN_NOTES];		//��ע��Ϣ

public:
	TNetCfg()
    {
        Clear();
    }

	void Clear()
	{
		memset(this, 0, sizeof(*this));
	}

	void SetNotes( LPCSTR lpzNotes )
	{
		if(lpzNotes != NULL )
		{
            memset( m_achName, 0, sizeof(m_achName));
			strncpy( m_achName, lpzNotes, MAXLEN_NOTES -1 );
		}
	}
	
	const s8 *GetNotes(void)
	{
		return m_achName;
	}

	void Print(void)
	{
		u32 dwIp = GetIpAddr();
		if(dwIp != 0)
		{
			u32 dwMask = GetIpMask();
			u32 dwGW = GetGwIp();
			s8	achIp[32] = {0};
			sprintf(achIp, "%d.%d.%d.%d%c", (dwIp>>24)&0xFF, (dwIp>>16)&0xFF, (dwIp>>8)&0xFF, dwIp&0xFF, 0);
			s8	achMask[32] = {0};
			sprintf(achMask, "%d.%d.%d.%d%c", (dwMask>>24)&0xFF, (dwMask>>16)&0xFF, (dwMask>>8)&0xFF, dwMask&0xFF, 0);
			s8	achGw[32] = {0};
			sprintf(achGw, "%d.%d.%d.%d%c", (dwGW>>24)&0xFF, (dwGW>>16)&0xFF, (dwGW>>8)&0xFF, dwGW&0xFF, 0);
			
			OspPrintf( TRUE, FALSE, "Notes:%s\nachIp:%s\nachMask:%s\nachGw:%s\n",m_achName,achIp,achMask,achGw);
		}		
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//ÿ����������
struct TEthCfg
{
protected:
	u32  m_dwDMZIp;								//������DMZ IP���ã�
	TNetCfg	 m_atNetCfg[MCU_MAXNUM_ADAPTER_IP];	//IP��Ŀ


public:
	TEthCfg()
    {
        Clear();
    }
	
	void Clear()
	{
		 memset(this, 0, sizeof(*this));
	}
	
	BOOL32 IsUseDMZ ( void )
	{
		return (m_dwDMZIp != 0);
	}
	
	//dwIP ������
	void SetDMZIpAddr ( u32 dwIp)
	{
		m_dwDMZIp = htonl(dwIp);
	}
	
	//����������
	u32 GetDMZIpAddr ( void )
	{
		return ntohl(m_dwDMZIp);
	}

	void SetNetCfg(u8 byIdx, const TNetCfg &tNetCfg)
	{
		if( byIdx >= MCU_MAXNUM_ADAPTER_IP )
			return;
		
		m_atNetCfg[byIdx] = tNetCfg;
	}
	
	BOOL32 GetNetCfg(u8 byIdx, TNetCfg &tNetCfg) const
	{
		tNetCfg.Clear();
		if( byIdx >= MCU_MAXNUM_ADAPTER_IP )
			return  FALSE;

		tNetCfg = m_atNetCfg[byIdx];		
		return TRUE;
	}	

	void Print(void)
	{
		s8	achDMZIp[32] = {0};
		u32 dwDMZIp = GetDMZIpAddr();
		sprintf(achDMZIp, "%d.%d.%d.%d%c", (dwDMZIp>>24)&0xFF, (dwDMZIp>>16)&0xFF, (dwDMZIp>>8)&0xFF, dwDMZIp&0xFF, 0);
		
		OspPrintf( TRUE, FALSE, "m_dwDMZIp:%s\n",achDMZIp);
		for(u8 byIdx = 0; byIdx < MCU_MAXNUM_ADAPTER_IP; byIdx++)
		{
			if(m_atNetCfg[byIdx].GetIpAddr() == 0)
			{
				continue;
			}

			OspPrintf( TRUE, FALSE, "[Ip-%d]:\n", byIdx);
			m_atNetCfg[byIdx].Print();
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//�µ���������
struct TNewNetCfg
{
protected:
	enUseModeType	m_enUseModeType;	// 3����ģʽ || ����ģʽ
	enNetworkType	m_enNetworkType;    // �����ַ���ͣ�LAN or WAN 
	BOOL32 m_bIsUseSip;					//�Ƿ�����sip
	u8				m_bySipInEthIdx;	// SipIp���ĸ�������
	u32				m_dwSipIpAddr;		// SipIp
	u32				m_dwSipMaskAddr;	// SipMask
	u32				m_dwSipGwAddr;		// SipGw
	TEthCfg	m_atEthCfg[MAXNUM_ETH_INTERFACE];	//��������
	
public:
	TNewNetCfg()
    {
        Clear();
    }

	void Clear()
	{
		memset(this, 0, sizeof(*this));
		m_enNetworkType = enLAN;			//Ĭ������Ϊ˽��
		m_enUseModeType = MODETYPE_3ETH;	//Ĭ������Ϊ3����ģʽ
	}

	//ģʽ����
	void SetUseModeType (enUseModeType enType)
	{
		m_enUseModeType = enType;	
	}
	
	enUseModeType GetUseModeType( void ) const
	{
		return  m_enUseModeType;
	}

	// ������������
	void SetNetworkType(enNetworkType enType) 
    {

		m_enNetworkType = enType;

	}
	
	// ��ȡ��������
    enNetworkType GetNetworkType() const
    {
        return m_enNetworkType;
    }
	
    BOOL32 IsWan() const
    {
        return (m_enNetworkType == enWAN);
    }
	
    BOOL32 IsLan() const
    {
        return (m_enNetworkType == enLAN);
    }
	
	BOOL32 IsLanAndWan() const
    {
        return (m_enNetworkType == enBOTH);
    }

	void SetIsUseSip ( BOOL32 bIsUseSip )
	{
		m_bIsUseSip = bIsUseSip;
	}
	
	BOOL32 IsUseSip ( void ) const
	{
		return m_bIsUseSip;
	}

	void SetSipInEthIdx( u8 bySipInEthIdx )
	{
		m_bySipInEthIdx = bySipInEthIdx;
	}
	
	u8 GetSipInEthIdx ( void ) const
	{
		return m_bySipInEthIdx;
	}	
	//SipIp����
	void SetSipIpAddr(u32 dwSipIpAddr) { m_dwSipIpAddr = htonl(dwSipIpAddr); }
	u32  GetSipIpAddr()const {return ntohl(m_dwSipIpAddr); }

	void SetSipMaskAddr(u32 dwSipMaskAddr) { m_dwSipMaskAddr = htonl(dwSipMaskAddr); }
	u32  GetSipMaskAddr()const {return ntohl(m_dwSipMaskAddr); }
	
	void SetSipGwAddr(u32 dwSipGwAddr) { m_dwSipGwAddr = htonl(dwSipGwAddr); }
	u32  GetSipGwAddr()const {return ntohl(m_dwSipGwAddr); }

	//��������
	void SetEthCfg(u8 byIdx, const TEthCfg &tEthCfg)
    {
		if( byIdx >= MAXNUM_ETH_INTERFACE )
			return;
		m_atEthCfg[byIdx] = tEthCfg;
    }
	
    BOOL32 GetEthCfg(u8 byIdx, TEthCfg &tEthCfg) const
    {
		tEthCfg.Clear();
		if( byIdx >= MAXNUM_ETH_INTERFACE )
			return  FALSE;

		tEthCfg = m_atEthCfg[byIdx];
		return TRUE;
    }

	void Print(void)
	{
		s8	achSipIpAddr[32] = {0};
		u32 dwSipIpAddr = GetSipIpAddr();
		sprintf(achSipIpAddr, "%d.%d.%d.%d%c", (dwSipIpAddr>>24)&0xFF, (dwSipIpAddr>>16)&0xFF, (dwSipIpAddr>>8)&0xFF, dwSipIpAddr&0xFF, 0);

		OspPrintf( TRUE, FALSE, "\n[NewNetCfgInfo]:\nm_enUseModeType:%s\nm_enNetworkType:%s\nm_dwSipIpAddr:%s\n",	\
					(m_enUseModeType == MODETYPE_3ETH ? "MODETYPE_3ETH":"MODETYPE_BAK"),	\
					(m_enNetworkType == enLAN ? "enLAN":(m_enNetworkType == enWAN ? "enWAN":"enBOTH")),	\
					achSipIpAddr);
		for(u8 byIdx = 0; byIdx < MAXNUM_ETH_INTERFACE; byIdx++)
		{
			OspPrintf( TRUE, FALSE, "\n[8KI-Eth%d]:\n", byIdx);
			m_atEthCfg[byIdx].Print();
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//MtAdpע����Ӧ��Ϣ(len:148)
struct TMtAdpRegRsp 
{
protected:
	u32  m_dwVcIpAddr;                      //ҵ��IP��ַ��������
	u32  m_dwGkIpAddr;                      //GK IP��ַ��������
	s8   m_achMcuAlias[MAXLEN_ALIAS];       //MCU����
	u8   m_byMcuNetId;
	u16  m_wHeartBeatInterval;              //����ն˶���ʱ����(sec)
	u8   m_byHeartBeatFailTimes;            //�ն˶�������
	u8	 m_byMtAdpMaster;					//�Ƿ�Ϊ������ 1:��, 0:����
//	u16  m_wQ931Port;
//	u16  m_wRasPort;
	u16  m_wH225H245Port;                   //H225H245 �Ľ�����ʼ�˿�
	u16  m_wH225H245MtNum;                  //H225H245 �Ľ����ն���
    u8   m_byGkCharge;                      //�Ƿ����GK�Ʒ�
	u8	 m_byMcuType;						//Mcu����, zgc, 2007-04-03
    u8   m_byCascadeAliasType;              //����ʱMCU�ڻ����б�������ʾ��ʽ
    u8   m_byCascadeAdminLevel;              //������������(1-4)	
    u8   m_byUseCallingMatch;               //������ѭƥ��Լ��

	u32  m_dwMsSSrc;                        //����ʱ�����ڱ�עһ��ϵͳ���У�ֻҪ��ֵ�仯��˵������������ͬʱ����
    s8   m_achMcuVersion[MAXLEN_ALIAS];     //MCU �汾�ţ����ڳ�ʼ��Э��ջ
	u8	 m_byIsGkUseRRQPwd;						//�Ƿ�����GKRRQ����ע��
	s8	 m_achRRQPassword[GK_RRQ_PWD_LEN];	    //GK RRQ��Կ


public:
	TMtAdpRegRsp()
    {
		m_byCascadeAdminLevel = 4;
        memset(this, 0, sizeof(TMtAdpRegRsp));
    }

	VOID	SetVcIp(u32    dwIp){m_dwVcIpAddr = htonl(dwIp);}
	u32   	GetVcIp() const {return ntohl(m_dwVcIpAddr);}

	VOID	SetGkIp(u32    dwIp){m_dwGkIpAddr = htonl(dwIp);}
	u32   	GetGkIp() const {return ntohl(m_dwGkIpAddr);}

	VOID	SetAlias(LPCSTR lpszAlias)
	{
		if(lpszAlias != NULL)
			strncpy(m_achMcuAlias, lpszAlias, MAXLEN_ALIAS - 1);
	}

	VOID	GetAlias(char *pchBuf, u16  wBufLen) const
	{
		if(pchBuf != NULL)
			strncpy(pchBuf, m_achMcuAlias, wBufLen);
	}

    // guzh [9/3/2007] MCU �汾�ţ����H323Config
    VOID	SetMcuVersion(LPCSTR lpszAlias)
    {
        if(lpszAlias != NULL)
            strncpy(m_achMcuVersion, lpszAlias, MAXLEN_ALIAS - 1);
    }
    
    LPCSTR	GetMcuVersion() const
    {
        return m_achMcuVersion;
	}
	
    void   SetMcuNetId(u8 byMcuId)	{ m_byMcuNetId = byMcuId; }	
    u8     GetMcuNetId() const	{ return m_byMcuNetId; }    
	
	void   SetHeartBeatInterval( u16 wHeartBeatInterval ){ m_wHeartBeatInterval = htons(wHeartBeatInterval); } 
	u16     GetHeartBeatInterval( void ) const { return ntohs(m_wHeartBeatInterval); }
	void   SetHeartBeatFailTimes(u8 byHeartBeatFailTimes){ m_byHeartBeatFailTimes = byHeartBeatFailTimes; } 
	u8     GetHeartBeatFailTimes( void ) const { return m_byHeartBeatFailTimes; }
//	void   SetQ931Port(u16 wQ931Port){ m_wQ931Port = htons(wQ931Port);} 
//	u16    GetQ931Port( void ) const { return ntohs(m_wQ931Port); }
//	void   SetRasPort(u16 wRasPort){ m_wRasPort = htons(wRasPort);} 
//	u16    GetRasPort( void ) const { return ntohs(m_wRasPort); }
	void   SetH225H245Port(u16 wH225H245Port){ m_wH225H245Port = htons(wH225H245Port);} 
	u16    GetH225H245Port( void ) const { return ntohs(m_wH225H245Port); }
	void   SetH225H245MtNum(u16 wH225H245MtNum){ m_wH225H245MtNum = htons(wH225H245MtNum);} 
	u16    GetH225H245MtNum( void ) const { return ntohs(m_wH225H245MtNum); }
    void   SetMtAdpMaster( BOOL32 bMtAdpMaster ){ m_byMtAdpMaster = (bMtAdpMaster ? 1:0);}
    BOOL32 IsMtAdpMaster() const { return (1 == m_byMtAdpMaster) ? TRUE:FALSE;}
    void   SetIsGKCharge(BOOL32 bCharge) { m_byGkCharge = bCharge ? 1 : 0;    }
    BOOL32 GetIsGKCharge( void ) { return m_byGkCharge == 1 ? TRUE : FALSE;   }
	
	//Mcu����, zgc, 2007-04-03
	void	SetMcuType( u8 byMcuType ) { m_byMcuType = byMcuType; }
	u8		GetMcuType( void ) { return m_byMcuType; }

    //MCU����������ʾ��ʽ
    void    SetCasAliasType(u8 byType) { m_byCascadeAliasType = byType; }
    u8      GetCasAliasType() const {return m_byCascadeAliasType;}

    void   SetAdminLevel(u8 byLevel){ m_byCascadeAdminLevel = byLevel;}
    u8     GetAdminLevel() const {return m_byCascadeAdminLevel; }

    void    SetUseCallingMatch(u8 byUseMatch) { m_byUseCallingMatch = byUseMatch; }
    u8      GetUseCallingMatch( void ) const { return m_byUseCallingMatch;    }

	void SetMSSsrc(u32 dwSSrc)
    {
        m_dwMsSSrc = htonl(dwSSrc);
    }
    u32 GetMSSsrc(void) const
    {
        return ntohl(m_dwMsSSrc);
    }

	void SetGkRRQUsePwdFlag( u8 byIsUseGKPwd)
    {
		m_byIsGkUseRRQPwd = byIsUseGKPwd;
    }
    
    u8	GetGkRRQUsePwdFlag() const
    {
        return m_byIsGkUseRRQPwd;
	}

    VOID	SetGkRRQPassword(LPCSTR lpszAlias)
    {
        if(lpszAlias != NULL)
            strncpy(m_achRRQPassword, lpszAlias, GK_RRQ_PWD_LEN);
    }
    
    LPCSTR	GetGkRRQPassword() const
    {
        return m_achRRQPassword;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TPlayFileHpAttrib 
{
protected:
    emProfileAttrb m_emFirstVideoHpAttrib;
	emProfileAttrb m_emSecVideoHpAttrib;
	
public:
    TPlayFileHpAttrib(void)
    {
        m_emFirstVideoHpAttrib = emBpAttrb;
		m_emSecVideoHpAttrib = emBpAttrb;
    }
	
    void Reset()
    {
        m_emFirstVideoHpAttrib = emBpAttrb;
		m_emSecVideoHpAttrib = emBpAttrb;
    }
	
    emProfileAttrb GetFirstVideHpAttrib(void) const
    {
		//���õĲ���emHpAttrb,����Ϊ��bp
        return (ntohl(m_emFirstVideoHpAttrib) == 1) ? emHpAttrb :emBpAttrb ;
    }
    void SetFirstVideHpAttrib(emProfileAttrb emFirstVidHpAtt)
    {
        m_emFirstVideoHpAttrib = (emProfileAttrb)htonl(emFirstVidHpAtt);
    }
	
	emProfileAttrb GetSecVideHpAttrib(void) const
    {
		//���õĲ���emHpAttrb,����Ϊ��bp
        return (ntohl(m_emSecVideoHpAttrib) == 1)?emHpAttrb:emBpAttrb;
    }
    void SetSecVideHpAttrib(emProfileAttrb emSecVidHpAtt)
    {
        m_emSecVideoHpAttrib = (emProfileAttrb)htonl(emSecVidHpAtt);
    }
	
    void Print() const
    {
        StaticLog( "m_emFirstVideoHpAttrib: %d\n", ntohl(m_emFirstVideoHpAttrib));
		StaticLog( "m_emSecVideoHpAttrib: %d\n", ntohl(m_emSecVideoHpAttrib));
    }    
}
#ifndef WIN32
__attribute__ ((packed))
#endif
;

struct TVrsRecChnnlStatus : public TRecChnnlStatus
{
protected:
	u8		m_byVrsMtId;
	//֧�ֱ������ݣ�����129. 129-64+1=66
	char	m_achRecordExName[MAXLEN_RECORD_NAME+2];//��¼��
public:
	TVrsRecChnnlStatus(){ Clear(); }
	void Clear(){ memset(this, 0, sizeof(TVrsRecChnnlStatus)); }
	u8 GetVrsMtId() { return m_byVrsMtId; }
	void SetVrsMtId(u8 byVrsMtId) { m_byVrsMtId = byVrsMtId; }
	//��ȡ��¼��
	BOOL32 GetRecordName( char *szAlias, u16 wBufLen )	
	{
		if (wBufLen < KDV_NAME_MAX_LENGTH+1)
		{
			return FALSE;
		}
		strncpy( szAlias, m_achRecordName, sizeof(m_achRecordName)-1 );//�����ƽ�����
		strncpy( szAlias+sizeof(m_achRecordName)-1, m_achRecordExName, sizeof(m_achRecordExName) );
		return TRUE; 
	}
	//���ü�¼��
	void SetRecordName( LPCSTR lpszName ) 
	{
		u16 wStrLen = strlen(lpszName);
		strncpy( m_achRecordName, lpszName, sizeof( m_achRecordName ) );//�ȿ�ǰ��45���ַ�
		m_achRecordName[sizeof( m_achRecordName ) - 1] ='\0';//��֤�����ַ�����ȷ
		memset(m_achRecordExName, 0, sizeof(m_achRecordExName));
		if (wStrLen >= MAXLEN_RECORD_NAME)
		{
			// �ٴӵ�46���ַ���ʼ����ʣ���ַ�����m_achRecordExName��
			strncpy( m_achRecordExName, lpszName+sizeof( m_achRecordName )-1, min(wStrLen-sizeof( m_achRecordName )+1, sizeof(m_achRecordExName)-1));
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TPlayEqpAttrib : public TPlayFileAttrib
{
	/*��������*/
	enum EPlayReqType
	{
		TYPE_NOREQ		= 0,
		TYPE_FILELIST	= 1,
		TYPE_STARTPLAY	= 2
	};
protected:
    u8 m_byIsDStreamPlay;
	u8 m_byAudTrackNum;
	s8 m_szRecName[255];
	u8 m_byNackTryNum;
	TEqp m_tEqp;
	u8 m_byReqType;//������Ϣ����(0������/1�ļ�����/2��������)
	u32 m_dwGroupId;//����id
	u32 m_dwPageNo;//�ڼ�ҳ
	u32 m_dwListNum;//��ҳ����
	u32 m_dwFileId;//�����ļ����ļ�ID,����
	//s8 m_szVrsPlayData[135];//���濪������������ݣ�vrsʵ�����ߺ���Ϣ�ã���������ʶ�����޸�,Ŀǰ����128+u32=132
	//u16 m_wVrsPlayDataLen;
	u16 m_wMStremBR;//��¼�������ʣ���ͣ����ʱ��Flowcontrol 0���ָ�ʱ����������Ϣ
	u8  m_byListRecordSrcSsnId;//��¼��ǰ�����ļ��б��
	
public:
    TPlayEqpAttrib(void)
    {
        m_byIsDStreamPlay = 0;
		m_byAudTrackNum = 0;
		memset( m_szRecName,0,sizeof(m_szRecName) );
		m_byNackTryNum = 0;
		m_tEqp.SetNull();
		m_byReqType = 0;
		m_dwGroupId = 0;
		m_dwPageNo = 0;
		m_dwListNum = 0;
		m_dwFileId = 0;
		m_byListRecordSrcSsnId = 0;
    }
	
    void Reset()
    {
        m_byFileType = RECFILE_NORMAL;
        m_byAudioType = MEDIA_TYPE_NULL;
        m_byVideoType = MEDIA_TYPE_NULL;
        m_byDVideoType = MEDIA_TYPE_NULL;
        m_byIsDStreamPlay = 0;
		m_byAudTrackNum = 0;
		memset( m_szRecName,0,sizeof(m_szRecName) );
		m_byNackTryNum = 0;
		m_tEqp.SetNull();
		m_byReqType = 0;
		m_dwGroupId = 0;
		m_dwPageNo = 0;
		m_dwListNum = 0;
		m_dwFileId = 0;
		m_byListRecordSrcSsnId = 0;
    }
	
    BOOL32 IsDStreamPlay(void) const
    {
        return (m_byIsDStreamPlay == 1) ? TRUE : FALSE;
    }
    void SetDStreamPlay(u8 byIsDStreamPlay)
    {
        m_byIsDStreamPlay = byIsDStreamPlay;
    }

	u8 GetAudTrackNum(void) const
    {
        return m_byAudTrackNum;
    }
    void SetAudTrackNum(u8 byAudTrackNum)
    {
        m_byAudTrackNum = byAudTrackNum;
    }

	s8* GetRecName(void) 
    {
        return &m_szRecName[0];
    }
    void SetRecName(s8* pRecName)
    {
        u16 wStrLen = strlen(pRecName);
		if( wStrLen <= 255 )
		{
			strncpy( m_szRecName,pRecName,255 - 1 );
		}
    }
	
	u8 GetNackTryNum(void) const
    {
        return m_byNackTryNum;
    }

    void SetNackTryNum(u8 byNackTryNum)
    {
        m_byNackTryNum = byNackTryNum;
    }
	
	u8 GetListRecordSrcSsnId(void) const
    {
        return m_byListRecordSrcSsnId;
    }
	
    void SetListRecordSrcSsnId(u8 byListRecordSrcSsnId)
    {
        m_byListRecordSrcSsnId = byListRecordSrcSsnId;
    }

	TEqp GetEqp(void) 
    {
        return m_tEqp;
    }

    void SetEqp(TEqp tEqp)
    {
        m_tEqp = tEqp;
    }
	
	u8 GetReqType(void) const
    {
        return m_byReqType;
    }
    void SetReqType(u8 byReqType)
    {
        m_byReqType = byReqType;
    }
	
	u32 GetGroupId(void) const
    {
        return ntohl(m_dwGroupId);
    }
    void SetGroupId(u32 dwGroupId)
    {
        m_dwGroupId = htonl(dwGroupId);
    }
	
	u32 GetPageNo(void) const
    {
        return ntohl(m_dwPageNo);
    }
    void SetPageNo(u32 dwPageNo)
    {
        m_dwPageNo = htonl(dwPageNo);
    }
	
	u32 GetListNum(void) const
    {
        return ntohl(m_dwListNum);
    }
    void SetListNum(u32 dwListNum)
    {
        m_dwListNum = htonl(dwListNum);
    }
	
	u32 GetFileId(void) const
    {
        return ntohl(m_dwFileId);
    }
    void SetFileId(u32 dwFileId)
    {
        m_dwFileId = htonl(dwFileId);
    }
	
	u16 GetMStremBR(void) const
    {
        return ntohs(m_wMStremBR);
    }
    void SetMStremBR(u16 wMStremBR)
    {
        m_wMStremBR = htons(wMStremBR);
    }
	
    void Print() const
    {
        TPlayFileAttrib::Print();
        StaticLog( "m_byIsDStreamPlay: %d\n", m_byIsDStreamPlay);
		StaticLog( "m_byAudTrackNum: %d\n", m_byAudTrackNum);
		StaticLog( "m_szRecName: %s\n", m_szRecName);
		StaticLog( "m_byNackTryNum: %d\n", m_byNackTryNum);
		StaticLog( "m_tEqp: %d.%d\n", m_tEqp.GetMcuId(),m_tEqp.GetMtId() );
		StaticLog( "m_byReqType: %d\n", m_byReqType);
		StaticLog( "m_dwGroupId: %d\n", GetGroupId());
		StaticLog( "m_dwPageNo: %d\n", GetPageNo());
		StaticLog( "m_dwListNum: %d\n", GetListNum());
		StaticLog( "m_dwFileId: %d\n", GetFileId());
		StaticLog( "m_wMStremBR: %d\n", GetMStremBR());
    }    
}
#ifndef WIN32
__attribute__ ((packed))
#endif
;



//���廭��ϳɺ������б� (len:35)
struct TKDVVMPOutParam
{
protected:
	TVideoStreamCap m_atVmpOutMember[MAXNUM_MPU2_OUTCHNNL];
public:
	void Initialize()
	{
		for (u8 byIdx=0;byIdx<MAXNUM_MPU2_OUTCHNNL;byIdx++)
		{
			m_atVmpOutMember[byIdx].Clear();
		}
	}
	TKDVVMPOutParam()
	{
		Initialize();
	}
	//����ָ��ͨ��������
	TVideoStreamCap GetVmpOutCapIdx(u8 byIdx) const
	{
		return m_atVmpOutMember[byIdx];
	}
	
	//���غ�����ͨ������
	u8 GetVmpOutCount() const
	{
		u8 byCount = 0;
		for (; byCount<MAXNUM_MPU2_OUTCHNNL; byCount++)
		{
			if (MEDIA_TYPE_NULL == m_atVmpOutMember[byCount].GetMediaType())
			{
				break;
			}
		}
		return byCount;
	}

	/*==============================================================================
	������    :  GetCorrectChnnlByStrCap
	����      :  ���������������ʵ�mpu2������ͨ����(���ڻ�С������ӽ���������ͨ����)���hdu
	�㷨ʵ��  :  
	����˵��  :  TVideoStreamCap &tStrCap	[in]
				 u8 byOnlyAttrb	[in] �޶�ֻ����BP��HP���͵�ͨ��
	����ֵ˵��:  u8 ͨ����
	-------------------------------------------------------------------------------
	�޸ļ�¼  :  
	��  ��       �汾          �޸���          �߶���          �޸ļ�¼
	2011-12-5				yanghuaizhi
	==============================================================================*/
	u8 GetCorrectChnnlByStrCap(const TVideoStreamCap &tStrCap, u8 byOnlyAttrb = 0xFF) const
	{
		u8 byChnNum = ~0;
		u8 byRes = tStrCap.GetResolution();
		u8 byFps = tStrCap.GetUserDefFrameRate();
		TVideoStreamCap tTmpCap;
		//��֧��H264
		if (MEDIA_TYPE_H264 != tStrCap.GetMediaType())
		{
			return byChnNum;
		}
		// ���Һ�������
		for (u8 byNum=0; byNum<MAXNUM_MPU2_OUTCHNNL; byNum++)
		{
			//����δ�ҵ�
			if (MEDIA_TYPE_H264 != m_atVmpOutMember[byNum].GetMediaType())
			{
				//�Ƿ�264��ʽ��ͨ��
				if (MEDIA_TYPE_NULL != m_atVmpOutMember[byNum].GetMediaType())
				{
					byChnNum = byNum;
				}
				break;
			}
			tTmpCap = m_atVmpOutMember[byNum];
			//��������Ϊ0��ͨ����8kivmp��0·������ʱ����Ϊ0
			if (tTmpCap.GetMaxBitRate() == 0)
			{
				continue;
			}
			//ֻ����HP��BPʱ,������ͬ��ʽ,֧��ֻƥ��BP��HP
			if (0XFF != byOnlyAttrb && byOnlyAttrb != tTmpCap.GetH264ProfileAttrb())
			{
				continue;
			}
			// �ҵ�һ��С�ڵ��ڴ�������ͨ��
			if ((tTmpCap.IsH264CapEqual(byRes,byFps) && (tStrCap.IsSupportHP() || !tTmpCap.IsSupportHP()))
				|| tTmpCap.IsH264CapLower(byRes,byFps))//�ҵ�һ������С�ڵ���tStrCap��
			{
				byChnNum = byNum;
				break;
			}
		}
		
		return byChnNum;
	}

	/*==============================================================================
	������    :  GetVmpOutChnnlByStrCap
	����      :  ����������ö�Ӧmpu2������ͨ����
	�㷨ʵ��  :  
	����˵��  :  TVideoStreamCap &tStrCap	[in]
	����ֵ˵��:  u8 ͨ����
	-------------------------------------------------------------------------------
	�޸ļ�¼  :  
	��  ��       �汾          �޸���          �߶���          �޸ļ�¼
	2011-12-5				yanghuaizhi
	==============================================================================*/
	u8 GetVmpOutChnnlByStrCap(const TVideoStreamCap &tStrCap) const
	{
		u8 byChnNum = ~0;
		BOOL32 bIs1080P = FALSE;
		// 1080��Ҫ��֡��,�����ʽ������Ҫ�Ƚ�֡��
		if (MEDIA_TYPE_H264 == tStrCap.GetMediaType() &&
			VIDEO_FORMAT_HD1080 == tStrCap.GetResolution())
		{
			bIs1080P = TRUE;
		}
		TVideoStreamCap tTmpCap;
		// �����������
		for (u8 byNum=0; byNum<MAXNUM_MPU2_OUTCHNNL; byNum++)
		{
			//����δ�ҵ�
			if (MEDIA_TYPE_NULL == m_atVmpOutMember[byNum].GetMediaType())
			{
				break;
			}
			tTmpCap = m_atVmpOutMember[byNum]; 
			if (tTmpCap.GetMediaType() == tStrCap.GetMediaType() &&
				tTmpCap.GetResolution() == tStrCap.GetResolution() &&
				(!bIs1080P || tTmpCap.GetUserDefFrameRate() == tStrCap.GetUserDefFrameRate()) &&
				(tTmpCap.IsSupportHP() == tStrCap.IsSupportHP()))
			{
				byChnNum = byNum;
			}
		}

		return byChnNum;
	}
	
	/*==============================================================================
	������    :  GetStrCapByVmpOutChnnl
	����      :  ����mpu2������ͨ���Ż�ö�Ӧ����
	�㷨ʵ��  :  
	����˵��  :  u8 byChnnl		[in]
				 TVideoStreamCap &tStrCap	[out]
	����ֵ˵��:  BOOL32
	-------------------------------------------------------------------------------
	�޸ļ�¼  :  
	��  ��       �汾          �޸���          �߶���          �޸ļ�¼
	2011-12-5				yanghuaizhi
	==============================================================================*/
	BOOL32 GetStrCapByVmpOutChnnl(u8 byChnnl, TVideoStreamCap &tStrCap)
	{
		if (MEDIA_TYPE_NULL == m_atVmpOutMember[byChnnl].GetMediaType() || MAXNUM_MPU2_OUTCHNNL <= byChnnl)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[GetStrCapByVmpOutChnnl]Err,the channel is null.\n");
			return FALSE;
		}

		tStrCap = m_atVmpOutMember[byChnnl];
		return TRUE;
	}
	
	/*==============================================================================
	������    :  AddVmpOutMember
	����      :  ��Ӻ�����ͨ��
	�㷨ʵ��  :  
	����˵��  :  TVideoStreamCap &tStrCap		[in]
			 
	����ֵ˵��:  BOOL32
	-------------------------------------------------------------------------------
	�޸ļ�¼  :  
	��  ��       �汾          �޸���          �߶���          �޸ļ�¼
	2011-12-5				yanghuaizhi
	==============================================================================*/
	BOOL32 AddVmpOutMember(const TVideoStreamCap &tStrCap)
	{
		// ������Idx���������ͨ���б�
		for (u8 byNum=0; byNum<MAXNUM_MPU2_OUTCHNNL; byNum++)
		{
			// ��Idx�Ѿ����б���
			if (m_atVmpOutMember[byNum] == tStrCap)
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_VMP, "[AddVmpOutMember]Warning, this CapIdx is already exists.\n");
				return FALSE;
			}
			// �ҵ���λ,�������
			else if (MEDIA_TYPE_NULL == m_atVmpOutMember[byNum].GetMediaType())
			{
				m_atVmpOutMember[byNum] = tStrCap;
				return TRUE;
			}	
		}
		LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[AddVmpOutMember]Err,Add Member failed because channel is full.\n");
		return FALSE;
	}
	
	//����ָ��ͨ������
	BOOL32 UpdateVmpOutMember(u8 byNum, const TVideoStreamCap &tStrCap) 
	{ 
		m_atVmpOutMember[byNum] = tStrCap; 
		return TRUE;
	}

	// ���ָ��ͨ������,����MPU2 BASIC vicp��Դ����ʱ,���1080p60fps
	void ClearOneVmpOutChnnl(u8 byIdx)
	{
		if (byIdx >= MAXNUM_MPU2_OUTCHNNL)
		{
			return;
		}
		// ������ĳ�Ա��Ǩ��
		for (u8 byTmpIdx=byIdx+1; byTmpIdx < MAXNUM_MPU2_OUTCHNNL; byTmpIdx++)
		{
			m_atVmpOutMember[byTmpIdx-1] = m_atVmpOutMember[byTmpIdx];
		}
		// ������һ��ͨ��
		m_atVmpOutMember[MAXNUM_MPU2_OUTCHNNL-1].Clear();

	}
	
	void print(void)//FIXME
	{
		u8 byOutNum = GetVmpOutCount();
		StaticLog("Vmp Out Chnnl Num: %u\n", byOutNum);
		StaticLog("---------Vmp Out Channel Members------------\n");
		for(u8 byLoop = 0; byLoop < byOutNum; byLoop++)
		{
			StaticLog("Channel[%u]:", byLoop);
			m_atVmpOutMember[byLoop].Print();
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//vmp��ͨ����Ϣ
struct TVmpChnnlInfo
{	
private:
	u8 m_byHDChnnlNum;		//����ͨ����ռ��Ŀ
	u8 m_byMaxNumHdChnnl;	//���ǰ������Ŀ
	u8 m_byMaxStyleNum;		//֧���������(��������������轵�ֱ���)
	//u8 m_byCifChnnlNum;		//Ŀǰ����
	//u8 m_byAllChnnlNum;		//���б��õ�ͨ������Ŀǰ����
	//u8 m_byHdChnnlIdx;		//ͨ��idx
	TChnnlMemberInfo	m_tHdChnnlInfo[MAXNUM_VMP_MEMBER];

public:
	TVmpChnnlInfo(void)
	{
		clear();
	}
	void clear()
	{
		SetMaxNumHdChnnl(0);
		SetMaxStyleNum(0);
		clearHdChnnl();
	}
	void clearHdChnnl() //���ͨ����Ϣ,���������ǰ�������,���ڷ�ֹͣvmpʱ
	{
		SetHDChnnlNum(0);
		memset(m_tHdChnnlInfo, 0, sizeof(m_tHdChnnlInfo));
	}
	void SetHDChnnlNum(u8 byHDChnnlNum){m_byHDChnnlNum = byHDChnnlNum;}
	u8	 GetHDChnnlNum( void ){ return m_byHDChnnlNum; }
	void SetMaxNumHdChnnl(u8 byMaxNumHdChnnl){m_byMaxNumHdChnnl = byMaxNumHdChnnl;}
	u8	 GetMaxNumHdChnnl( void ){ return m_byMaxNumHdChnnl; }
	void SetMaxStyleNum(u8 byMaxStyleNum){m_byMaxStyleNum = byMaxStyleNum;}
	u8	 GetMaxStyleNum( void ){ return m_byMaxStyleNum; }
	//void SetCifChnnlNum(u8 byCifChnnlNum){m_byCifChnnlNum = byCifChnnlNum;}
	//u8	 GetCifChnnlNum( void ){ return m_byCifChnnlNum; }
	//void SetAllChnnlNum(u8 byAllChnnlNum){m_byAllChnnlNum = byAllChnnlNum;}
	
	void SetHdChnnlInfo(u8 byChnnlIdx, const TChnnlMemberInfo *ptChnnlMemberInfo)
	{
		if (NULL == ptChnnlMemberInfo)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[SetHdChnnlInfo]Err:ptChnnlMemberInfo is null.\n");
			return;
		}

		if (m_tHdChnnlInfo[byChnnlIdx].GetMt().IsNull() &&
			!ptChnnlMemberInfo->GetMt().IsNull())
		{
			m_byHDChnnlNum++;
		}
		else if (!m_tHdChnnlInfo[byChnnlIdx].GetMt().IsNull() &&
			ptChnnlMemberInfo->GetMt().IsNull())
		{
			m_byHDChnnlNum--;
		}
		m_tHdChnnlInfo[byChnnlIdx] = *ptChnnlMemberInfo;
	}
	
	/*==============================================================================
	������    :  GetHdChnnlInfo
	����      :  ��ǰ����Idx��ö�Ӧ������Ϣ
	�㷨ʵ��  :  
	����˵��  :  u8 byChnnlIdx		[in]
				 TChnnlMemberInfo *ptChnnlMemberInfo	[out]
	����ֵ˵��:  void
	-------------------------------------------------------------------------------
	�޸ļ�¼  :  
	��  ��       �汾          �޸���          �߶���          �޸ļ�¼
	2011-12-5				yanghuaizhi
	==============================================================================*/
	void GetHdChnnlInfo(u8 byChnnlIdx, TChnnlMemberInfo *ptChnnlMemberInfo)
	{
		if (NULL == ptChnnlMemberInfo)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[GetHdChnnlInfo]Err:ptChnnlMemberInfo is null.\n");
			return;
		}

		u8 byTmpIdx = 0;
		for(u8 byLoop=0; byLoop< MAXNUM_VMP_MEMBER; byLoop++)
		{
			if(!m_tHdChnnlInfo[byLoop].GetMt().IsNull())
			{
				if (byTmpIdx == byChnnlIdx)
				{
					*ptChnnlMemberInfo = m_tHdChnnlInfo[byLoop];
					return;
				}
				byTmpIdx++;
			}
		}
		//�Ҳ������
		ptChnnlMemberInfo->SetNull();
	}
	
	/*==============================================================================
	������    :  GetHdChnnlInfoByPos
	����      :  ��ǰ����Idx��ö�Ӧ������Ϣ
	�㷨ʵ��  :  
	����˵��  :  u8 byChnnlIdx		[in]
				 TChnnlMemberInfo *ptChnnlMemberInfo	[out]
	����ֵ˵��:  void
	-------------------------------------------------------------------------------
	�޸ļ�¼  :  
	��  ��       �汾          �޸���          �߶���          �޸ļ�¼
	2011-12-5				yanghuaizhi
	==============================================================================*/
	void GetHdChnnlInfoByPos(u8 byPos, TChnnlMemberInfo *ptChnnlMemberInfo)
	{
		if (NULL == ptChnnlMemberInfo || byPos >= MAXNUM_VMP_MEMBER)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[GetHdChnnlInfoByPos]Err:ptChnnlMemberInfo is null or byPos[%d] is wrong.\n",byPos);
			return;
		}

		*ptChnnlMemberInfo = m_tHdChnnlInfo[byPos];
	}
	
	/*�ҵ���һ�����е�ͨ��,û���򷵻�-1
	s32 FindFreeHdChnnl()
	{
		for(u8 byLoop=0; byLoop< MAXNUM_SVMPB_HDCHNNL; byLoop++)
		{
			if(m_tHdChnnlInfo[byLoop].GetMt().IsNull())
			{
				return byLoop;
			}
		}
		return -1;
	}*/
	/*==============================================================================
	������    :  GetChnlCountByMt
	����      :  ���Mtռ��ǰ����ͨ������
	�㷨ʵ��  :  
	����˵��  :  const TMt &tMt		[in]
			 
	����ֵ˵��:  ͨ������
	-------------------------------------------------------------------------------
	�޸ļ�¼  :  
	��  ��       �汾          �޸���          �߶���          �޸ļ�¼
	2011-12-5				yanghuaizhi
	==============================================================================*/
	u8 GetChnlCountByMt(const TMt &tMt)
	{
		u8 byCount = 0;
		if (tMt.IsNull())
		{
			return byCount;
		}

		for (u8 byLoop=0; byLoop<MAXNUM_VMP_MEMBER; byLoop++)
		{
			//����˫������ͨ��
			if (m_tHdChnnlInfo[byLoop].IsAttrDstream())
			{
				continue;
			}

			if (m_tHdChnnlInfo[byLoop].GetMt().GetMcuIdx() == tMt.GetMcuIdx() &&
				m_tHdChnnlInfo[byLoop].GetMt().GetMtId() == tMt.GetMtId())
			{
				byCount++;
			}
		}

		return byCount;
	}

	/*==============================================================================
	������    :  UpdateInfoToHdChnl
	����      :  ��ͨ���Ÿ���ǰ������Ϣ
	�㷨ʵ��  :  
	����˵��  :  TChnnlMemberInfo *ptChnnlMemberInfo [in]
				 u8 byVmpPos [in]
			 
	����ֵ˵��:  BOOL32 ����TRUE �쳣FALSE
	-------------------------------------------------------------------------------
	�޸ļ�¼  :  
	��  ��		�汾			�޸���		�߶���		�޸ļ�¼
	2011-11-29					�־
	==============================================================================*/
	BOOL32 UpdateInfoToHdChnl(TChnnlMemberInfo *ptChnnlMemberInfo, u8 byVmpPos)
	{
		if (NULL == ptChnnlMemberInfo || byVmpPos >= MAXNUM_VMP_MEMBER)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[UpdateInfoToHdChnl]Err:ptChnnlMemberInfo is null or byVmpPos[%d] is wrong.\n",byVmpPos);
			return FALSE;
		}

		if (m_byHDChnnlNum >= m_byMaxNumHdChnnl &&
			m_tHdChnnlInfo[byVmpPos].GetMt().IsNull())
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[UpdateInfoToHdChnl]Err:HdChnl is fool.\n");
			return FALSE;
		}

		if (!m_tHdChnnlInfo[byVmpPos].GetMt().IsNull())//����
		{
			m_tHdChnnlInfo[byVmpPos] = *ptChnnlMemberInfo;
		} 
		else { //ռ�ø�ͨ��
			m_tHdChnnlInfo[byVmpPos] = *ptChnnlMemberInfo;
			m_byHDChnnlNum++;
		}

		return TRUE;
	}

	/*==============================================================================
	������    :  ClearChnlByMt
	����      :  �����ն����������ͨ��,������Ȩ��ȡ��
	�㷨ʵ��  :  
	����˵��  :  const TMt &tMt		[in]
	����ֵ˵��:  void
	-------------------------------------------------------------------------------
	�޸ļ�¼  :  
	��  ��		�汾			�޸���		�߶���		�޸ļ�¼
	2011-11-29					�־
	==============================================================================*/
	void ClearChnlByMt(const TMt &tMt)
	{
		if (tMt.IsNull())
		{
			return;
		}

		for (u8 byLoop=0; byLoop<MAXNUM_VMP_MEMBER; byLoop++)
		{
			// ˫��Դ��֧�ְ��ն���ǰ����ͨ��
			if (m_tHdChnnlInfo[byLoop].IsAttrDstream())
			{
				continue;
			}

			if (m_tHdChnnlInfo[byLoop].GetMt().GetMcuIdx() == tMt.GetMcuIdx() &&
				m_tHdChnnlInfo[byLoop].GetMt().GetMtId() == tMt.GetMtId())
			{
				m_tHdChnnlInfo[byLoop].SetNull();
				m_byHDChnnlNum--;
			}
		}
	}

	/*==============================================================================
	������    :  ClearChnlByVmpPos
	����      :  ����vmpͨ�������������ǰ����ͨ��
	�㷨ʵ��  :  
	����˵��  :  const TMt &tMt		[in]
	����ֵ˵��:  BOOL32 ����ɹ�TRUE ʧ��FALSE
	-------------------------------------------------------------------------------
	�޸ļ�¼  :  
	��  ��		�汾			�޸���		�߶���		�޸ļ�¼
	2011-11-29					�־
	==============================================================================*/
	BOOL32 ClearChnlByVmpPos(u8 byVmpPos)
	{
		if (byVmpPos >= MAXNUM_VMP_MEMBER)
		{
			return FALSE;
		}
		if (m_tHdChnnlInfo[byVmpPos].GetMt().IsNull())
		{
			return FALSE;
		}
		else {
			m_tHdChnnlInfo[byVmpPos].SetNull();
			m_byHDChnnlNum--;
		}
		
		return TRUE;
	}
	
	/*==============================================================================
	������    :  GetChnlInfoList
	����      :  ��ð��ն˻��ֵ�ǰ������Ϣ,ͬһ�ն˿���ռ���ǰ����ͨ��
	�㷨ʵ��  :  
	����˵��  :  u8 &byMtNum	[o] �ն˸���
				 TChnnlMemberInfo *ptChnlInfo [o] �ն˶�Ӧǰ������Ϣ
				 u8 *abyChnlNo	[o] ��Ӧ�ն�ռ��ǰ����ͨ������
	����ֵ˵��:  void
	-------------------------------------------------------------------------------
	�޸ļ�¼  :  
	��  ��		�汾			�޸���		�߶���		�޸ļ�¼
	2011-12-13					�־
	==============================================================================*/
	void GetChnlInfoList(u8 &byMtNum,u8 *abyChnlNo, TChnnlMemberInfo *ptChnlInfo)
	{
		if (NULL == abyChnlNo || NULL == ptChnlInfo)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[GetChnlInfoList]Err:abyChnlNo or ptChnlInfo is null.\n");
			return;
		}
		byMtNum = 0;
		TVmpChnnlInfo tTmpChnlInfo = *this;
		for(u8 byPos = 0; byPos < MAXNUM_VMP_MEMBER; byPos++)	//����ռ��HD��ͨ����MT��Ϣ
		{
			TMt tTmpMt = m_tHdChnnlInfo[byPos].GetMt();
			if (!tTmpMt.IsNull())
			{
				// �Ѿ��������
				if (0 == tTmpChnlInfo.GetChnlCountByMt(tTmpMt))
				{
					continue;
				}
				*ptChnlInfo = m_tHdChnnlInfo[byPos];
				*abyChnlNo = GetChnlCountByMt(tTmpMt);
				ptChnlInfo++;
				abyChnlNo++;
				byMtNum++;
				//������Ϣ�������ͳ�ƹ����ն�
				tTmpChnlInfo.ClearChnlByMt(tTmpMt);
			}
		}
	}
	
	/*==============================================================================
	������    :  GetWeakestVmpVipChl
	����      :  �������vip����ͨ��
	�㷨ʵ��  :  
	����˵��  :  
	����ֵ˵��:  
	-------------------------------------------------------------------------------
	�޸ļ�¼  :  
	��  ��       �汾          �޸���          �߶���          �޸ļ�¼
	2013-07					yanghuaizhi							
	==============================================================================*/
	u8 GetWeakestVmpVipChl()
	{
		u8 byWeakestChl = 0XFF;
		TChnnlMemberInfo tWeakestChlInfo;
		// �ҵ�����ǰ��������
		for (u8 byLoop=0; byLoop<MAXNUM_VMP_MEMBER; byLoop++)
		{
			if (!m_tHdChnnlInfo[byLoop].IsAttrNull())
			{
				// ѡ���������ҵ�ѡ��vip����ֹͣ
				if (m_tHdChnnlInfo[byLoop].IsAttrSelected())
				{
					byWeakestChl = byLoop;
					break;
				}

				// �������벻�ɽ��ֱ���vip����
				if (byWeakestChl >= MAXNUM_VMP_MEMBER)
				{
					tWeakestChlInfo = m_tHdChnnlInfo[byLoop];// ��¼��һ���������Ƿ�����Ҳ�������⳧��Mt
					byWeakestChl = byLoop;
				}
				else
				{
					// ���ɽ��ֱ���vip�ȷ�������
					if (tWeakestChlInfo.IsAttrSpeaker() && !m_tHdChnnlInfo[byLoop].IsCanAdjResMt())
					{
						tWeakestChlInfo = m_tHdChnnlInfo[byLoop];
						byWeakestChl = byLoop;
					}
				}
			}
		}

		return byWeakestChl;
	}

	/*==============================================================================
	������    :  GetReplaceVmpVipChl
	����      :  ����ǰ����ͨ����Ϣ����ñȸ�����С�Ŀ�ռ��ͨ��
	�㷨ʵ��  :  ������vip > ���ɽ��ֱ���vip > ��ѡ��vip
	����˵��  :  
	����ֵ˵��:  
	-------------------------------------------------------------------------------
	�޸ļ�¼  :  
	��  ��       �汾          �޸���          �߶���          �޸ļ�¼
	2013-07					yanghuaizhi							
	==============================================================================*/
	void GetReplaceVmpVipChl(const TChnnlMemberInfo &tChlMemInfo, u8 &byChlPos, TChnnlMemberInfo &tReplaceMemInfo)
	{
		byChlPos = 0XFF;
		if (GetMaxNumHdChnnl() == 0 || tChlMemInfo.IsAttrNull())
		{
			return;
		}
		// ѡ��������ͣ���֧�ֺ�̨��ռ
		if (tChlMemInfo.IsAttrSelected())
		{
			return;
		}
		u8 byWeakestChlPos = GetWeakestVmpVipChl();
		if (byWeakestChlPos < MAXNUM_VMP_MEMBER)
		{
			// ������vip��ѡ�����ֱ�ӷ���
			if (m_tHdChnnlInfo[byWeakestChlPos].IsAttrSelected())
			{
				byChlPos = byWeakestChlPos;
				tReplaceMemInfo = m_tHdChnnlInfo[byWeakestChlPos];
			}
			// �����˱Ȳ��ɽ��ֱ���vip��ǿ
			else if (tChlMemInfo.IsAttrSpeaker() && !m_tHdChnnlInfo[byWeakestChlPos].IsCanAdjResMt())
			{
				byChlPos = byWeakestChlPos;
				tReplaceMemInfo = m_tHdChnnlInfo[byWeakestChlPos];
			}
			else
			{
				// �Ҳ����ȴ�����������ͨ��
			}
		}

		return;
	}

	void print(void)//FIXME
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "MaxStyleNum: %u\nMax HdChnnl Num: %u\nSeized HdChnnl Num: %u\n", 
			m_byMaxStyleNum, m_byMaxNumHdChnnl, m_byHDChnnlNum);
		LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "---------Mt info in Hd Channels------------\n");
		for(u8 byLoop = 0; byLoop < MAXNUM_VMP_MEMBER; byLoop++)
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "Hd Channel[%u]-->Mt.(%u,%u)\n",
				byLoop, m_tHdChnnlInfo[byLoop].GetMt().GetMcuId(), m_tHdChnnlInfo[byLoop].GetMt().GetMtId()  );
		}

	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//��¼��ռ������Ҫ�Ĳ���
struct TVmpAdaptChnSeizeOpr
{
public:
	TMt m_tMt;		//��ռ���ն�
	u16 m_wEventId;
	TSwitchInfo m_tSwitchInfo;
	u8  m_bySrcSsnId;  //Դ�Ự��,��������ѡ���Ƿ���MCS��קѡ��,MCS��ק��������ʾС�۾�
public:
	TVmpAdaptChnSeizeOpr(void)
	{
		Clear();
	}
	void Clear( void )
	{
		memset(this, 0, sizeof(TVmpAdaptChnSeizeOpr));
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


struct TSwitchChannelExt:public TSwitchChannel
{
    TSwitchChannelExt()
    {
        Clear( );
    }
protected:
	
	u32  m_dwMappedIP;        //��Ҫ���͵�Ŀ��IP��ַ��Ӧ��Դ����ip(������)
	u16  m_wMappedPort;       //��Ҫ���͵�Ŀ��˿ڵ�Դ����port (������)
	
public:
	
	void Clear( ){ memset( this, 0, sizeof(TSwitchChannelExt) ); }
	void SetMapIp(u32  dwMappedIp){m_dwMappedIP = htonl(dwMappedIp); }
	u32  GetMapIp()const { return ntohl(m_dwMappedIP); }
	void SetMapPort(u16 wMappedPort){m_wMappedPort = htons(wMappedPort);}
	u16  GetMapPort()const{return ntohs(m_wMappedPort);}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//���������ṹ
struct TSendMutePack
{
	u32 m_dwDstIp;//Ҫ����������Ŀ��ip
	u16 m_wDstPort;//Ҫ����������Ŀ��port
	u8  m_byConfIdx;//��������
	u8  m_byMtId;//���������mtid
	u8  m_byPayloadType;    //Ҫ����������payloadtype
	u8  m_byAudioTrackNum;//������
	void Clear()
	{	
		m_dwDstIp = 0;
		m_wDstPort = 0;
		m_byConfIdx = 0xFF;
		m_byMtId = 0;
		m_byPayloadType = MEDIA_TYPE_NULL;
		m_byAudioTrackNum = 1;//Ĭ�ϵ�����
	}
	TSendMutePack(){Clear();}
	~TSendMutePack(){Clear();}
	void SetConfIdx(const u8 &byConfIdx)
	{
		m_byConfIdx = byConfIdx;
	}
	u8 GetConfIdx()const
	{
		return m_byConfIdx;
	}
	void SetMtId(const u8 & byMtId)
	{
		m_byMtId = byMtId;
	}
	u8 GetMtId()const
	{
		return m_byMtId;
	}
	void SetDstIp(const u32 &dwDstIp)
	{
		m_dwDstIp = htonl(dwDstIp);
	}
	u32 GetDstIp()const
	{
		return ntohl(m_dwDstIp);
	}
	void SetDstPort(const u16 &wDstport)
	{
		m_wDstPort = htons(wDstport);
	}
	u16 GetDstPort() const
	{
		return ntohs(m_wDstPort);
	}

	void SetPayloadType(const u8 &byPayload)
	{
		m_byPayloadType = byPayload;
	}
	u8  GetPayloadType()const
	{
		return m_byPayloadType;
	} 
	void SetAudioTrackNum(const u8 &byAudioTrackNum)
	{
		m_byAudioTrackNum = byAudioTrackNum;
	}
	u8  GetAudioTrackNum()const
	{
		return m_byAudioTrackNum;
	} 

	BOOL32 IsNull()
	{
		if ( m_dwDstIp == 0 || m_wDstPort == 0 || m_byConfIdx > MAX_CONFIDX || m_byPayloadType == MEDIA_TYPE_NULL )
		{
			return TRUE;
		}
		return FALSE;
	}
	void OspPrint()
	{
		OspPrintf(TRUE,FALSE,"m_dwDstIp:0x%x\n",GetDstIp());
		OspPrintf(TRUE,FALSE,"m_wDstPort:%d\n",GetDstPort());
		OspPrintf(TRUE,FALSE,"m_byConfIdx:%d\n",GetConfIdx());
		OspPrintf(TRUE,FALSE,"m_byMtId:%d\n",GetMtId());
		OspPrintf(TRUE,FALSE,"m_byPayloadType:%d\n",GetPayloadType());
		OspPrintf(TRUE,FALSE,"m_byAudioTrackNum:%d\n",GetAudioTrackNum());
	}
	void Print()
	{
		StaticLog("m_dwDstIp:0x%x\n",GetDstIp());
		StaticLog("m_wDstPort:%d\n",GetDstPort());
		StaticLog("m_byConfIdx:%d\n",GetConfIdx());
		StaticLog("m_byMtId:%d\n",GetMtId());
		StaticLog("m_byPayloadType:%d\n",GetPayloadType());
		StaticLog("m_byAudioTrackNum:%d\n",GetAudioTrackNum());
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//���ڼ���V4R6B2��8000H��յ�Ԥ�������ṹ[11/14/2012 chendaiwei]
struct TMtVCSPlanAliasV4R6B2
{
public:
    u8				m_AliasType;               //��������
    s8				m_achAlias[32];  //�����ַ���
    TTransportAddr	m_tTransportAddr;          //�����ַ
	u8				m_byReserve;
public:
	TMtVCSPlanAliasV4R6B2( void ){ memset( this, 0, sizeof(TMtVCSPlanAliasV4R6B2) ); }
	BOOL operator ==( const TMtVCSPlanAliasV4R6B2 & tObj ) const;
	BOOL IsNull( void ) const { if( m_AliasType == 0 )return TRUE; return FALSE; }
	void SetNull( void ){ memset(this, 0, sizeof(TMtVCSPlanAliasV4R6B2)); }
	BOOL IsAliasNull( void ) const { return 0 == strlen(m_achAlias); }
	void SetE164Alias( LPCSTR lpzAlias )
	{
		if( lpzAlias != NULL )
		{
			m_AliasType = mtAliasTypeE164;
            memset( m_achAlias, 0, sizeof(m_achAlias));
			strncpy( m_achAlias, lpzAlias, 32-1 );
		}
	}
	void SetH323Alias( LPCSTR lpzAlias )
	{
		if( lpzAlias != NULL )
		{
			m_AliasType = mtAliasTypeH323ID;
            memset( m_achAlias, 0, sizeof(m_achAlias));
			strncpy( m_achAlias, lpzAlias, 32-1);
		}
	}
	void SetH320Alias( u8 byLayer, u8 bySlot, u8 byChannel )
	{
		m_AliasType = mtAliasTypeH320ID;
		sprintf( m_achAlias, "��%d-��%d-ͨ��%d%c", byLayer, bySlot, byChannel, 0 );
	}
	BOOL GetH320Alias ( u8 &byLayer, u8 &bySlot, u8 &byChannel )
	{
		BOOL bRet = FALSE;
		byLayer   = 0;
		bySlot    = 0; 
		byChannel = 0; 
		if( mtAliasTypeH320ID != m_AliasType )
		{
			return bRet;
		}

		s8 *pachLayerPrefix   = "��";
		s8 *pachSlotPrefix    = "-��";
		s8 *pachChannelPrefix = "-ͨ��";
		s8 *pachAlias = m_achAlias;

		if( 0 == memcmp( pachAlias, pachLayerPrefix, strlen("��") ) )
		{
			s32  nPos = 0;
			s32  nMaxCHLen = 3; //һ�ֽڵ�����ִ�����
			s8 achLayer[4] = {0};

			//��ȡ���
			pachAlias += strlen("��");
			for( nPos = 0; nPos < nMaxCHLen; nPos++ )
			{
				if( '-' == pachAlias[nPos] )
				{
					break;
				}
				achLayer[nPos] = pachAlias[nPos];
			}
			if( 0 == nPos || nPos >= nMaxCHLen )
			{
				return bRet;
			}
			achLayer[nPos+1] = '\0';
			byLayer = atoi(achLayer);

			//��ȡ�ۺ�
			pachAlias += nPos;
			if( 0 == memcmp( pachAlias, pachSlotPrefix, strlen("-��") ) )
			{
				pachAlias += strlen("-��");
				for( nPos = 0; nPos < nMaxCHLen; nPos++ )
				{
					if( '-' == pachAlias[nPos] )
					{
						break;
					}
					achLayer[nPos] = pachAlias[nPos];
				}
				if( 0 == nPos || nPos >= nMaxCHLen )
				{
					return bRet;
				}
				achLayer[nPos+1] = '\0';
				bySlot = atoi(achLayer);
			
				//��ȡͨ����
				pachAlias += nPos;
				if( 0 == memcmp( pachAlias, pachChannelPrefix, strlen("-ͨ��") ) )
				{
					pachAlias += strlen("-ͨ��");
					memcpy( achLayer, pachAlias, nMaxCHLen );
					achLayer[nMaxCHLen] = '\0';
					byChannel = atoi(achLayer);
					bRet = TRUE;
				}
			}
		}
		return bRet;
	}
	void Print( void ) const
	{
		if( m_AliasType == 0 )
		{
			StaticLog("null alias" );
		}
		else if( m_AliasType == mtAliasTypeTransportAddress )
		{
			StaticLog("IP: 0x%x:%d", 
				     m_tTransportAddr.GetIpAddr(), m_tTransportAddr.GetPort() );
		}		
		else if( m_AliasType == mtAliasTypeE164 )
		{
			StaticLog("E164: %s", m_achAlias ); 
		}
		else if( m_AliasType == mtAliasTypeH323ID )
		{
			StaticLog("H323ID: %s", m_achAlias ); 
		}
		else if( m_AliasType == mtAliasTypeH320ID )
		{
			StaticLog("H320ID: %s", m_achAlias ); 
		}
        else if( m_AliasType == mtAliasTypeH320Alias )
        {
            StaticLog("H320Alias: %s", m_achAlias );
        }
		else
		{
			StaticLog("Other type alias!" ); 
		}
		StaticLog("\n" );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// mcu������ʹ��vmp��ͨ��Ϣ
struct TVmpCommonAttrb
{
	u8	m_byVmpStyle;	//�ϳɷ��
	u8	m_byMemberNum;	//��Ա����
	u8	m_byNeedPrs;	//�Ƿ���ҪPRS
	TVMPMemberEx		m_atMtMember[MAXNUM_VMP_MEMBER];		//���ϳ�Ա
	TMediaEncrypt		m_tVideoEncrypt[MAXNUM_VMP_MEMBER];     //��Ƶ���ܲ���
	TDoublePayload		m_tDoublePayload[MAXNUM_VMP_MEMBER];	//˫�غ�
	u8					m_abyRcvH264DependInMark[MAXNUM_VMP_MEMBER];//����8ke/h/i vmp�����ⷽʽ�������ã���Ի�Ϊ�նˣ�
	TVmpStyleCfgInfo	m_tVmpStyleCfgInfo;		//������Ϣ
	TVMPExCfgInfo		m_tVmpExCfgInfo;		//��չ������Ϣ
	
	TVmpCommonAttrb()
	{
		memset(this, 0, sizeof(TVmpCommonAttrb));
	}
	
    void Print(void)
    {
        StaticLog( "VmpStyle:%d, MemberNum:%d, NeedPrs:%d\n",
			m_byVmpStyle, m_byMemberNum, m_byNeedPrs);
        for(u8 byIndex = 0; byIndex < MAXNUM_VMP_MEMBER; byIndex++)
        {
            if(0 != m_atMtMember[byIndex].GetMcuId() && 0 != m_atMtMember[byIndex].GetMtId())
            {
                StaticLog( "McuId:%d, MtId:%d, MemType:%d, MemStatus:%d, RealPayLoad:%d, ActPayLoad:%d, EncrptMode:%d\n", 
					m_atMtMember[byIndex].GetMcuId(), m_atMtMember[byIndex].GetMtId(), 
					m_atMtMember[byIndex].GetMemberType(), m_atMtMember[byIndex].GetMemStatus(),
					m_tDoublePayload[byIndex].GetRealPayLoad(), m_tDoublePayload[byIndex].GetActivePayload(),
					m_tVideoEncrypt[byIndex].GetEncryptMode());
            }            
        }        
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// mpu2����ʹ�ù�ͨ������Ϣ
struct TMPU2CommonAttrb
{
	u8	m_byVmpStyle;	//�ϳɷ��
	u8	m_byMemberNum;	//��Ա����
	u8	m_byNeedPrs;	//�Ƿ���ҪPRS
	TVMPMemberEx		m_atMtMember[MAXNUM_MPU2VMP_MEMBER];		//���ϳ�Ա
	TMediaEncrypt		m_tVideoEncrypt[MAXNUM_MPU2VMP_MEMBER];     //��Ƶ���ܲ���
	TDoublePayload		m_tDoublePayload[MAXNUM_MPU2VMP_MEMBER];	//˫�غ�
	TVmpStyleCfgInfo	m_tVmpStyleCfgInfo;		//������Ϣ
	TVMPExCfgInfo		m_tVmpExCfgInfo;		//��չ������Ϣ
	
	TMPU2CommonAttrb()
	{
		memset(this, 0, sizeof(TMPU2CommonAttrb));
	}
	
    void Print(void)
    {
        StaticLog( "VmpStyle:%d, MemberNum:%d, NeedPrs:%d\n",
			m_byVmpStyle, m_byMemberNum, m_byNeedPrs);
        for(u8 byIndex = 0; byIndex < MAXNUM_MPU2VMP_MEMBER; byIndex++)
        {
            if(0 != m_atMtMember[byIndex].GetMcuId() && 0 != m_atMtMember[byIndex].GetMtId())
            {
                StaticLog( "McuId:%d, MtId:%d, MemType:%d, MemStatus:%d, RealPayLoad:%d, ActPayLoad:%d, EncrptMode:%d\n", 
					m_atMtMember[byIndex].GetMcuId(), m_atMtMember[byIndex].GetMtId(), 
					m_atMtMember[byIndex].GetMemberType(), m_atMtMember[byIndex].GetMemStatus(),
					m_tDoublePayload[byIndex].GetRealPayLoad(), m_tDoublePayload[byIndex].GetActivePayload(),
					m_tVideoEncrypt[byIndex].GetEncryptMode());
            }            
        }        
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/* ����ϳɳ�Ա�ṹ(Mcu -- Vmp) 8kh/8ki�²��ع�׼��
struct TVMPMember128Ex : public TVMPMember
{
public:
	void  SetMember( TVMPMember& tMember )
	{
		memcpy(this, &tMember, sizeof(tMember));
	}
	
    BOOL32  SetMbAlias( u8 byAliasLen, const s8 *pAlias ) 
    { 
        if( NULL == pAlias )
        {
            return FALSE;
        }
        memset(m_achMbAlias, 0, sizeof(m_achMbAlias));
        memcpy(m_achMbAlias, pAlias, min(byAliasLen, MAXLEN_ALIAS-1));
        m_achMbAlias[MAXLEN_ALIAS-1] = '\0';
        return TRUE; 
    }
	const s8* GetMbAlias(void) { return m_achMbAlias; }
	
protected:
	s8   m_achMbAlias[MAXLEN_ALIAS];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 8ke8kh8ki����ʹ�ù�ͨ������Ϣ,һ�������Ҫ���Ǽ���
struct T8KEVmpCommonAttrb
{
	u8	m_byVmpStyle;	//�ϳɷ��
	u8	m_byMemberNum;	//��Ա����
	u8	m_byNeedPrs;	//�Ƿ���ҪPRS
	TVMPMember128Ex		m_atMtMember[MAXNUM_SDVMP_MEMBER];		//���ϳ�Ա,������128
	TMediaEncrypt		m_tVideoEncrypt[MAXNUM_SDVMP_MEMBER];     //��Ƶ���ܲ���
	TDoublePayload		m_tDoublePayload[MAXNUM_SDVMP_MEMBER];	//˫�غ�
	u8					m_abyRcvH264DependInMark[MAXNUM_SDVMP_MEMBER];//����8ke/h/i vmp�����ⷽʽ�������ã���Ի�Ϊ�նˣ�
	TVmpStyleCfgInfo	m_tVmpStyleCfgInfo;		//������Ϣ
	TVMPExCfgInfo		m_tVmpExCfgInfo;		//��չ������Ϣ
	
	T8KEVmpCommonAttrb()
	{
		memset(this, 0, sizeof(T8KEVmpCommonAttrb));
	}
	
    void Print(void)
    {
        StaticLog( "VmpStyle:%d, MemberNum:%d, NeedPrs:%d\n",
			m_byVmpStyle, m_byMemberNum, m_byNeedPrs);
        for(u8 byIndex = 0; byIndex < MAXNUM_SDVMP_MEMBER; byIndex++)
        {
            if(0 != m_atMtMember[byIndex].GetMcuId() && 0 != m_atMtMember[byIndex].GetMtId())
            {
                StaticLog( "McuId:%d, MtId:%d, MemType:%d, MemStatus:%d, RealPayLoad:%d, ActPayLoad:%d, EncrptMode:%d\n", 
					m_atMtMember[byIndex].GetMcuId(), m_atMtMember[byIndex].GetMtId(), 
					m_atMtMember[byIndex].GetMemberType(), m_atMtMember[byIndex].GetMemStatus(),
					m_tDoublePayload[byIndex].GetRealPayLoad(), m_tDoublePayload[byIndex].GetActivePayload(),
					m_tVideoEncrypt[byIndex].GetEncryptMode());
            }            
        }        
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
*/

// Vmp׷����Ϣ������ [7/3/2013 liaokang]
enum emVmpCommType
{
    emVmpStart = (u8)0,
    emVmpMbAlias,          // ��Ա����
};

//��Ƶ������Ϣ
struct TAudioCapInfo
{
private:
	TAudioTypeDesc m_tAudioTypeDesc;
	u8             m_byActivePayload;
	u32            reserved;
public:
	TAudioCapInfo( void )
	{
		Clear();
	}
	void Clear( void )
	{
		m_tAudioTypeDesc.Clear();
		m_byActivePayload = 0;
	}
	void SetAudioMediaType( u8 byAudioType)
	{
		m_tAudioTypeDesc.SetAudioMediaType(byAudioType);
	}
	void SetAudioTrackNum( u8 byAudioTrackNum)
	{
		m_tAudioTypeDesc.SetAudioTrackNum(byAudioTrackNum);
	}
	void SetActivePayLoad( u8 byActivePayload)
	{
		m_byActivePayload = byActivePayload;
	}
	u8 GetAudioMediaType( void )
	{
		return m_tAudioTypeDesc.GetAudioMediaType();
	}
	u8 GetAudioTrackNum( void )
	{
		return m_tAudioTypeDesc.GetAudioTrackNum();
	}
	u8 GetActivePayLoad( void )
	{
		return m_byActivePayload;
	}
}
#ifndef WIN32
__attribute__((packed)) 
#endif
;

//ֹͣ��������Ϣ
struct H460StopHoleInfo
{
public:
    u16 wLocalPort;
    u32 dwRemoteIp;
    u16 wRemotePort;    
    
public:
    u16  GetLocalPort(){ return ntohs(wLocalPort); }
    void SetLocalPort( u16 wPort ){ wLocalPort = htons(wPort); }
    
    u32  GetRemoteIp(){ return ntohl(dwRemoteIp); }
    void SetRemoteIp( u32 dwIp ){ dwRemoteIp = htonl(dwIp); }
    
    u16  GetRemotePort(){ return ntohs(wRemotePort); }
    void SetRemotePort( u16 wPort ){ wRemotePort = htons(wPort); }
    
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

#ifdef WIN32
#pragma pack( pop )
#endif

#endif
