/*****************************************************************************
   ģ����      : mcu
   �ļ���      : mcustruct.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: MCU�ṹ
   ����        : ������
   �汾        : V3.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2004/09/16  3.5         ������      ����
   2005/02/19  3.6         ����      �����޸ġ���3.5�汾�ϲ�
******************************************************************************/
#ifndef __MCUSTRUCT_H_
#define __MCUSTRUCT_H_

#include "osp.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "kdvsys.h"
#include "mcuconst.h"
#include "kdvdef.h"
#include "vccommon.h"
#include "ummessagestruct.h"
#include "vcsstruct.h"
#include "lanman.h"


#ifdef WIN32
    #pragma comment( lib, "ws2_32.lib" ) 
    #pragma pack( push )
    #pragma pack( 1 )
    #define window( x )	x
#else
    #include <netinet/in.h>
    #define window( x )
#endif


#ifdef WIN32
#ifndef vsnprintf
#define vsnprintf   _vsnprintf
#endif
#ifndef snprintf
#define snprintf    _snprintf
#endif
#endif

#ifdef _LINUX_

#ifndef min
#define min(a,b) ((a)>(b)?(b):(a))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef VOID
#define VOID void 
#endif

#endif  /* end _LINUX_ */

//����MCU�ṹ
typedef TMt TMcu;

//�ն���չ�ṹ (len:34)
struct TMtExt : public TMt
{
protected:
	u8    m_byManuId;          //���̱��,�μ����̱�Ŷ���
	u8    m_byCallMode;        //�����ն˷�ʽ��0-�������նˣ��ֶ����� 1-����һ�� 2-��ʱ����
	u16   m_wDialBitRate;      //�����ն����ʣ������򣩵�λKBPS
	u32   m_dwCallLeftTimes;   //��ʱ�����ն˵�ʣ����д���
	u32   m_dwIPAddr;          //IP��ַ��������
	char  m_achAlias[VALIDLEN_ALIAS];  //�ն˱�������Ϊ16Byte
    u8    m_byProtocolType;         // �ն˵�Э������
public:
    TMtExt() : m_byManuId(0),
               m_byCallMode(0),
               m_wDialBitRate(0),
               m_dwCallLeftTimes(0),
               m_dwIPAddr(0),
               m_byProtocolType(0)
    {
        memset( m_achAlias, 0, sizeof(m_achAlias) );
    }
    void   SetManuId(u8   byManuId){ m_byManuId = byManuId;} 
    u8     GetManuId( void ) const { return m_byManuId; }
    void   SetCallMode(u8   byCallMode){ m_byCallMode = byCallMode;} 
    u8     GetCallMode( void ) const { return m_byCallMode; }
    void   SetDialBitRate(u16 wDialBitRate ){ m_wDialBitRate = htons(wDialBitRate);} 
    u16    GetDialBitRate( void ) const { return ntohs(m_wDialBitRate); }
    void   SetCallLeftTimes( u32 dwCallLeftTimes){ m_dwCallLeftTimes = dwCallLeftTimes;} 
    u32    GetCallLeftTimes( void ) const { return m_dwCallLeftTimes; }
    void   SetIPAddr(u32    dwIPAddr){ m_dwIPAddr = htonl(dwIPAddr); } 
    u32    GetIPAddr( void ) const { return ntohl(m_dwIPAddr); }
    void   SetProtocolType(u8 byProtocolType) { m_byProtocolType = byProtocolType;}
    u8     GetProtocolType(void) const { return m_byProtocolType;}
	char*  GetAlias( void ) const{ return (s8 *)m_achAlias; }
	void   SetAlias( char* pszAlias )
	{
        memset( m_achAlias, 0, sizeof( m_achAlias ) );
		if( pszAlias != NULL )
		{
			strncpy( m_achAlias, pszAlias, sizeof( m_achAlias ) - 1);
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//�ն���չ�ṹUTF8���� by pengguofeng (len:34)
struct TMtExtU : public TMt
{
private:
#define MEMBER_NUM   13         //��Ա����
protected:
	u8    m_byManuId;          //���̱��,�μ����̱�Ŷ���
	u8    m_byCallMode;        //�����ն˷�ʽ��0-�������նˣ��ֶ����� 1-����һ�� 2-��ʱ����
	u16   m_wDialBitRate;      //�����ն����ʣ������򣩵�λKBPS
	u32   m_dwCallLeftTimes;   //��ʱ�����ն˵�ʣ����д���
	u32   m_dwIPAddr;          //IP��ַ��������
    u8    m_byProtocolType;         // �ն˵�Э������
	char  m_achAlias[VALIDLEN_ALIAS_UTF8];  //�ն˱�������Ϊ16Byte
public:
    TMtExtU() : m_byManuId(0),
		m_byCallMode(0),
		m_wDialBitRate(0),
		m_dwCallLeftTimes(0),
		m_dwIPAddr(0),
		m_byProtocolType(0)
    {
        memset( m_achAlias, 0, sizeof(m_achAlias) );
    }
    void   SetManuId(u8   byManuId){ m_byManuId = byManuId;} 
    u8     GetManuId( void ) const { return m_byManuId; }
    void   SetCallMode(u8   byCallMode){ m_byCallMode = byCallMode;} 
    u8     GetCallMode( void ) const { return m_byCallMode; }
    void   SetDialBitRate(u16 wDialBitRate ){ m_wDialBitRate = htons(wDialBitRate);} 
    u16    GetDialBitRate( void ) const { return ntohs(m_wDialBitRate); }
    void   SetCallLeftTimes( u32 dwCallLeftTimes){ m_dwCallLeftTimes = dwCallLeftTimes;} 
    u32    GetCallLeftTimes( void ) const { return m_dwCallLeftTimes; }
    void   SetIPAddr(u32    dwIPAddr){ m_dwIPAddr = htonl(dwIPAddr); } 
    u32    GetIPAddr( void ) const { return ntohl(m_dwIPAddr); }
    void   SetProtocolType(u8 byProtocolType) { m_byProtocolType = byProtocolType;}
    u8     GetProtocolType(void) const { return m_byProtocolType;}
	char*  GetAlias( void ) const{ return (s8 *)m_achAlias; }
	void   SetAlias( char* pszAlias )
	{
        memset( m_achAlias, 0, sizeof( m_achAlias ) );
		if( pszAlias != NULL )
		{
			strncpy( m_achAlias, pszAlias, sizeof( m_achAlias ) - 1);
		}
	}

	//��Ա����
	u8 GetMemNum(void)
	{
		return MEMBER_NUM;
	}

	u32 GetTotalMemLen(void)
	{
		return GetMemNum()*sizeof(u16);
	}
	//�õ�ĳ��Ա�Ĵ�С
	u16 GetMemLen(u8 byMemId )
	{
		switch ( byMemId )
		{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 12:
			return sizeof(u8);
		case 9:
			return sizeof(u16);
		case 10:
		case 11:
			return sizeof(u32);
		case 13:
			return sizeof(m_achAlias);
		default:
			return 0;
		}
	}

	//�ж�ĳ��ԱΪ�ַ���
	bool IsMemString(u8 byMemId )
	{
		if ( byMemId == 13 )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void PrintContent() const
	{
		StaticLog("| %5d | %5d | %5d | %5d | %5d | %5d | %s \n",
			GetType(), GetEqpType(), GetMcuIdx(), GetMtId(), GetDriId(), GetConfIdx(),
			GetAlias());
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//�ն���Ϣ������չ�ṹ len:38
struct TMtExt2 : public TMt
{
public:
    TMtExt2():m_byHWVerId(0)
    {
        memset(m_achSWVerId, 0, sizeof(m_achSWVerId));
    }
    void SetMt(TMt &tMt)
    {
        memcpy(this, &tMt, sizeof(TMt));
    }
    void SetHWVerId( u8 byVerId ) { m_byHWVerId = byVerId;    }
    u8   GetHWVerId( void ) const { return m_byHWVerId;    }
    void SetSWVerId( LPCSTR lpszSWVerId )
    {
        if ( lpszSWVerId == NULL )
        {
            return;
        }
        u16 wLen = min(strlen(lpszSWVerId), sizeof(m_achSWVerId)-1);
        strncpy(m_achSWVerId, lpszSWVerId, wLen);
        m_achSWVerId[sizeof(m_achSWVerId)-1] = '\0';
    }
    LPCSTR GetSWVerId( void ) const { return m_achSWVerId;    }

    void SetProductId( LPCSTR lpszProductId )
    {
        if (NULL == lpszProductId)
        {
            return;
        }
        u16 wLen = min(strlen(lpszProductId), sizeof(m_achProductId)-1);
        strncpy(m_achProductId, lpszProductId, wLen);
        m_achProductId[sizeof(m_achProductId)-1] = '\0';
    }
    LPCSTR GetProductId( void ) const { return m_achProductId;  };

    void SetVersionId( LPCSTR lpszVersionId )
    {
        if (NULL == lpszVersionId)
        {
            return;
        }
        u16 wLen = min(strlen(lpszVersionId), sizeof(m_achVersionId)-1);
        strncpy(m_achVersionId, lpszVersionId, wLen);
        m_achVersionId[sizeof(m_achVersionId)-1] = '\0';
    }
    LPCSTR GetVersionId( void ) const { return m_achVersionId;  };

protected:
    u8  m_byHWVerId;                    //�ն�Ӳ���汾��
    s8  m_achSWVerId[MAXLEN_SW_VER-1];  //�ն�����汾��
    s8  m_achProductId[MAXLEN_PRODUCTID];//�ն��豸��
    s8  m_achVersionId[MAXLEN_VERSIONID];//�ն��豸�汾��
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

#ifndef SETBITSTATUS
#define SETBITSTATUS(StatusValue, StatusMask, bStatus)  \
    if (bStatus)    StatusValue |= StatusMask;          \
    else            StatusValue &= ~StatusMask;

#define GETBITSTATUS(StatusValue, StatusMask)  (0 != (StatusValue&StatusMask))   
#endif
    
//���ϼ�mcu�ϱ����˵��ն�״̬ʱ���õĽṹ (len:10)
struct TSMcuMtStatus : public TMt
{
    enum TSMcuMtStatusMask
    {
        Mask_Fecc    =  0x01,
        Mask_Mixing  =  0x02,
        Mask_VidLose  =  0x04,
		Mask_CallMode =  0x08,
        Mask_SendingVideo = 0x10,
        Mask_SendingAudio = 0x20,
        Mask_RcvingVideo = 0x40,
        Mask_RecvingAudio = 0x80		
    };

protected:
	u8  m_byIsStatus;     //0-bit�Ƿ���ң������ͷ,1-bit�Ƿ��ڻ���,2-bit�Ƿ���ƵԴ��ʧ
	u8  m_byCurVideo;     //��ǰ��ƵԴ(1)
	u8  m_byCurAudio;     //��ǰ��ƵԴ(1)
	u8  m_byMtBoardType;  //�ն˵İ忨����(MT_BOARD_WIN-0, MT_BOARD_8010-1, MT_BOARD_8010A-2, MT_BOARD_8018-3, MT_BOARD_IMT-4,MT_BOARD_8010C-5)
	//zjj20100327
	u8	m_byMtExtInfo;   //���λ��ʾ�ն��Ƿ��ڷ��͵ڶ�·��Ƶ�����±�ʾ�ն˺���ʧ��ԭ��

public:
	u8	m_byCasLevel;
	u8	m_abyMtIdentify[MAX_CASCADELEVEL]; //��Ӧ�������ն˵ı�ʶ 
public:
	TSMcuMtStatus( void )
	{ 
		memset( this, 0, sizeof(TSMcuMtStatus));
		SetIsEnableFECC(FALSE);
		SetCurVideo(1);
		SetCurAudio(1);
		SetIsAutoCallMode(FALSE);
		SetMtExInfo( MTLEFT_REASON_NONE );
	}
	void SetIsEnableFECC(BOOL bCamRCEnable){ SETBITSTATUS(m_byIsStatus, Mask_Fecc, bCamRCEnable) } 
	BOOL IsEnableFECC( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_Fecc); }
    void SetIsMixing(BOOL bMixing) { SETBITSTATUS(m_byIsStatus, Mask_Mixing, bMixing) } 
    BOOL IsMixing(void) const { return GETBITSTATUS(m_byIsStatus, Mask_Mixing); }
    void SetVideoLose(BOOL bVidLose){ SETBITSTATUS(m_byIsStatus, Mask_VidLose, bVidLose) }
    BOOL IsVideoLose( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_VidLose);    }
	void SetCurVideo(u8 byCurVideo){ m_byCurVideo = byCurVideo;} 
	u8   GetCurVideo( void ) const { return m_byCurVideo; }
	void SetCurAudio(u8 byCurAudio){ m_byCurAudio = byCurAudio;} 
	u8   GetCurAudio( void ) const { return m_byCurAudio; }
	void SetMtBoardType(u8 byType) { m_byMtBoardType = byType; }
	u8   GetMtBoardType() const {return m_byMtBoardType; }
	void SetIsAutoCallMode( BOOL byAutoMode )   { SETBITSTATUS(m_byIsStatus, Mask_CallMode, byAutoMode); }
	BOOL IsAutoCallMode( void )                 { return GETBITSTATUS(m_byIsStatus, Mask_CallMode); }
    void SetSendVideo(BOOL bSend) { SETBITSTATUS(m_byIsStatus, Mask_SendingVideo, bSend) } 
    BOOL IsSendVideo( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_SendingVideo); }
    void SetSendAudio(BOOL bSend) { SETBITSTATUS(m_byIsStatus, Mask_SendingAudio, bSend) } 
    BOOL IsSendAudio( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_SendingAudio); }
    void SetRecvVideo(BOOL bRecv) { SETBITSTATUS(m_byIsStatus, Mask_RcvingVideo, bRecv) } 
    BOOL IsRecvVideo( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_RcvingVideo); }
    void SetRecvAudio(BOOL bRecv) { SETBITSTATUS(m_byIsStatus, Mask_RecvingAudio, bRecv) } 
    BOOL IsRecvAudio( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_RecvingAudio); }	
	//zjj20100327
	void SetDisconnectReason( u8 byReason ){ m_byMtExtInfo = (0x7F & byReason); }
	u8	 GetDisconnectReason( void ){ return m_byMtExtInfo & 0x7F; }
	void SetSendVideo2( BOOL bSend ){ bSend ? (m_byMtExtInfo |= 0x80) : (m_byMtExtInfo &= 0x7F);}
	BOOL IsSendVideo2( void ){ return ((m_byMtExtInfo & 0x80) == 0x80) ? TRUE :FALSE; }
	void SetMtExInfo(u8 byMtExInfo){ m_byMtExtInfo = byMtExInfo;} 
	u8   GetMtExInfo( void ) const { return m_byMtExtInfo; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//���ϼ�mcu�ϱ����˵��ն�״̬ʱ���õĽṹ (len:8)
struct TMcuToMcuMtStatus
{
    enum TMcuToMcuMtStatusMask
    {
        Mask_Fecc   =   0x01,
        Mask_Mixing =   0x02,
        Mask_VideoLose = 0x04,
		Mask_CallMode =  0x08,
        Mask_SendingVideo = 0x10,
        Mask_SendingAudio = 0x20,
        Mask_RcvingVideo = 0x40,
        Mask_RecvingAudio = 0x80
    };
protected:
	u8  m_byIsStatus;     //0-bit�Ƿ���ң������ͷ,1-bit�Ƿ��ڻ���,2-bit�Ƿ���ƵԴ��ʧ
	u8  m_byCurVideo;     //��ǰ��ƵԴ(1)
	u8  m_byCurAudio;     //��ǰ��ƵԴ(1)
	u8  m_byMtBoardType;  //�ն˵İ忨����(MT_BOARD_WIN-0, MT_BOARD_8010-1, MT_BOARD_8010A-2, MT_BOARD_8018-3, MT_BOARD_IMT-4,MT_BOARD_8010C-5)
	u32 m_dwPartId;       //������
	u8  m_byMtExtInfo;    //���λ��ʾ�ն��Ƿ��ڷ��͵ڶ�·��Ƶ�����±�ʾ�ն˺���ʧ��ԭ��

public:
	u8	m_byCasLevel;
	u8	m_abyMtIdentify[MAX_CASCADELEVEL]; //��Ӧ�������ն˵ı�ʶ 
public:
	TMcuToMcuMtStatus( void )
	{ 
		memset( this, 0, sizeof(TMcuToMcuMtStatus));
		SetIsEnableFECC(FALSE);
        SetIsMixing(FALSE);
        SetIsVideoLose(FALSE);
		SetCurVideo(1);
		SetCurAudio(1);
		SetIsAutoCallMode(FALSE);
		SetMtExInfo( MTLEFT_REASON_NONE );
	}
	void SetIsEnableFECC(BOOL bCamRCEnable){ SETBITSTATUS(m_byIsStatus, Mask_Fecc, bCamRCEnable) } 
	BOOL IsEnableFECC( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_Fecc); }
    void SetIsMixing(BOOL bMixing) { SETBITSTATUS(m_byIsStatus, Mask_Mixing, bMixing) } 
    BOOL IsMixing(void) const { return GETBITSTATUS(m_byIsStatus, Mask_Mixing); }
    void SetIsVideoLose(BOOL bVideoLose) { SETBITSTATUS(m_byIsStatus, Mask_VideoLose, bVideoLose) }
    BOOL IsVideoLose(void) const { return GETBITSTATUS(m_byIsStatus, Mask_VideoLose);    }
	void SetCurVideo(u8 byCurVideo){ m_byCurVideo = byCurVideo;} 
	u8   GetCurVideo( void ) const { return m_byCurVideo; }
	void SetCurAudio(u8 byCurAudio){ m_byCurAudio = byCurAudio;} 
	u8   GetCurAudio( void ) const { return m_byCurAudio; }
	void SetMtBoardType(u8 byType) { m_byMtBoardType = byType; }
	u8   GetMtBoardType() const {return m_byMtBoardType; }
	void SetPartId(u32 dwPartId){ m_dwPartId = htonl(dwPartId);} 
	u32  GetPartId( void ) const { return ntohl(m_dwPartId); }
	void SetIsAutoCallMode( BOOL byAutoMode )   { SETBITSTATUS(m_byIsStatus, Mask_CallMode, byAutoMode); }
	BOOL IsAutoCallMode( void )                 { return GETBITSTATUS(m_byIsStatus, Mask_CallMode); }
    void SetSendVideo(BOOL bSend) { SETBITSTATUS(m_byIsStatus, Mask_SendingVideo, bSend) } 
    BOOL IsSendVideo( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_SendingVideo); }
    void SetSendAudio(BOOL bSend) { SETBITSTATUS(m_byIsStatus, Mask_SendingAudio, bSend) } 
    BOOL IsSendAudio( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_SendingAudio); }
    void SetRecvVideo(BOOL bRecv) { SETBITSTATUS(m_byIsStatus, Mask_RcvingVideo, bRecv) } 
    BOOL IsRecvVideo( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_RcvingVideo); }
    void SetRecvAudio(BOOL bRecv) { SETBITSTATUS(m_byIsStatus, Mask_RecvingAudio, bRecv) } 
    BOOL IsRecvAudio( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_RecvingAudio); }
	void SetMtExInfo(u8 byMtExInfo){ m_byMtExtInfo = byMtExInfo;} 
	u8   GetMtExInfo( void ) const { return m_byMtExtInfo; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//����������������RRQʱ����֤��Ϣ��¼ GatekeeperID/EndpointID
//TH323TransportAddress�ṹӦ��radstackЭ��ջ��Ӧ�ṹ cmTransportAddress һ�� (len:44)
struct TH323TransportAddress
{
    u16  m_wlength; /* length in bytes of route */
    u32  m_dwip;
    u16  m_wport;
    int  m_ntype;
    u32  m_adwroute[7];
    int  m_ndistribution;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//(len:310)
struct TH323EPGKIDAlias
{
protected:
	int  m_ntype;        //������
    u16  m_wlength;      //������
    char m_szAlias[256];
    int  m_npnType;      //������ 
    TH323TransportAddress m_transport;
public:
	void SetIDtype( int ntype ){ m_ntype = htonl(ntype); }
	int  GetIDtype( void ){ return ntohl(m_ntype); }
	void SetIDlength( u16 wlength ){ m_wlength = htons(wlength); }
	u16  GetIDlength( void ){ return ntohs(m_wlength); }
	void SetIDpnType( int npnType ){ m_npnType = htonl(npnType); }
	int  GetIDpnType( void ){ return ntohl(m_npnType); }
	void SetIDAlias( char *pszAlias ){ memcpy( m_szAlias, pszAlias, 256 ); }
	char*  GetIDAlias( void ){ return m_szAlias; }
	void SetIDtransport( TH323TransportAddress *ptransport ){ memcpy( (void*)&m_transport, (void*)ptransport, sizeof(TH323TransportAddress) ); }
	TH323TransportAddress*  GetIDtransport( void ){ return &m_transport; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//TRASInfo����N+1���ݣ������RAS��Ϣͬ��������(len:622)
struct TRASInfo 
{
protected:
    u32 m_dwGKIp;                   // �����gk��ַ��������
    u32 m_dwRRQIp;                  // ����ĺ��з����ַ��calladdress��������
    TH323EPGKIDAlias m_tGKIDAlias;  // �����GKID
    TH323EPGKIDAlias m_tEPIDAlias;  // �����EPID
    s8  m_achE164Num[MAXLEN_E164+1];// �����E164

public:
    TRASInfo() : m_dwGKIp(0),
                 m_dwRRQIp(0)
    {
        memset( &m_tGKIDAlias, 0, sizeof(m_tGKIDAlias) );
        memset( &m_tEPIDAlias, 0, sizeof(m_tEPIDAlias) );
        memset( &m_achE164Num, 0, sizeof(m_achE164Num) );
    }
    void SetGKIp(u32 dwIp){ m_dwGKIp = htonl(dwIp);    }
    u32  GetGKIp(void) { return ntohl(m_dwGKIp);    }
    void SetRRQIp(u32 dwIp){ m_dwRRQIp = htonl(dwIp);    }
    u32  GetRRQIp(void) { return ntohl(m_dwRRQIp);    }
    TH323EPGKIDAlias *GetGKID(void) { return &m_tGKIDAlias;    }
    void SetGKID(TH323EPGKIDAlias *ptH323GKID){ memcpy( &m_tGKIDAlias, ptH323GKID, sizeof(TH323EPGKIDAlias));    }
    TH323EPGKIDAlias *GetEPID(void) { return &m_tEPIDAlias;    }
    void SetEPID(TH323EPGKIDAlias *ptH323EPID){ memcpy( &m_tEPIDAlias, ptH323EPID, sizeof(TH323EPGKIDAlias));    }
    LPCSTR GetMcuE164(void) { return m_achE164Num; }
    void SetMcuE164(LPCSTR lpszE164Alias) 
    {
        if ( NULL != lpszE164Alias )
        {
            memcpy( m_achE164Num, lpszE164Alias, sizeof(m_achE164Num));    
            m_achE164Num[sizeof(m_achE164Num)-1] = '\0';
        }
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//ҵ����Ϣ�࣬�����28K������Ϣ
class CServMsg
{
protected:
	u16     m_wSerialNO;            //��ˮ��
	u8      m_bySrcDriId;           //ԴDRI���
	u8      m_bySrcMtId;            //Դ�ն˺�
	u8      m_bySrcSsnId;           //Դ�Ự��
	u8      m_byDstDriId;           //Ŀ��DRI���
    u8      m_byDstMtId;            //Ŀ���ն˺�
	u8      m_byMcuId;              //MCU��
	u8      m_byChnIndex;           //ͨ��������
    u8      m_byConfIdx;            //����������
	CConfId m_cConfId;              //�����
	u16		m_wEventId;             //�¼���
	u16		m_wTimer;               //��ʱ
	u16		m_wErrorCode;           //������
	u16		m_wMsgBodyLen;          //��Ϣ�峤��
    u8      m_byTotalPktNum;        //�ܰ�����������Ҫ�а����͵���Ϣ��
    u8      m_byCurPktIdx;          //��ǰ����������0��ʼ��
	u8		m_byEqpId;				//��Ӧ�����
	u8      m_abyReserverd[11];     //�����ֽ�
	u8      m_abyMsgBody[SERV_MSG_LEN-SERV_MSGHEAD_LEN];    //��Ϣ��
public:
	void  SetSerialNO(u16  wSerialNO){ m_wSerialNO = htons(wSerialNO);} 
    u16   GetSerialNO( void ) const { return ntohs(m_wSerialNO); }
    void  SetSrcDriId(u8   bySrcDriId){ m_bySrcDriId = bySrcDriId;} 
    u8    GetSrcDriId( void ) const { return m_bySrcDriId; }
    void  SetSrcMtId(u8   bySrcMtId){ m_bySrcMtId = bySrcMtId;} 
    u8    GetSrcMtId( void ) const { return m_bySrcMtId; }
    void  SetSrcSsnId(u8   bySrcSsnId){ m_bySrcSsnId = bySrcSsnId;} 
    u8    GetSrcSsnId( void ) const { return m_bySrcSsnId; }
    void  SetDstDriId(u8   byDstDriId){ m_byDstDriId = byDstDriId;} 
    u8    GetDstDriId( void ) const { return m_byDstDriId; }  
    void  SetDstMtId(u8   byDstMtId){ m_byDstMtId = byDstMtId;} 
    u8    GetDstMtId( void ) const { return m_byDstMtId; }
    void  SetMcuId(u8   byMcuId){ m_byMcuId = byMcuId;} 
    u8    GetMcuId( void ) const { return m_byMcuId; }
    void  SetChnIndex(u8   byChnIndex){ m_byChnIndex = byChnIndex;} 
    u8    GetChnIndex( void ) const { return m_byChnIndex; }
    void  SetConfIdx(u8   byConfIdx){ m_byConfIdx = byConfIdx;} 
    u8    GetConfIdx( void ) const { return m_byConfIdx; } 
    void  SetEventId(u16  wEventId){ m_wEventId = htons(wEventId);} 
    u16   GetEventId( void ) const { return ntohs(m_wEventId); }
    void  SetTimer(u16  wTimer){ m_wTimer = htons(wTimer);} 
    u16   GetTimer( void ) const { return ntohs(m_wTimer); }
    void  SetErrorCode(u16  wErrorCode){ m_wErrorCode = htons(wErrorCode);} 
    u16   GetErrorCode( void ) const { return ntohs(m_wErrorCode); }
    void  SetTotalPktNum(u8 byPktNum) { m_byTotalPktNum = byPktNum; }
    u8    GetTotalPktNum( void ) { return m_byTotalPktNum; }
    void  SetCurPktIdx(u8 byPktIdx) { m_byCurPktIdx = byPktIdx; }
    u8    GetCurPktIdx( void ) { return m_byCurPktIdx; }
    void  SetEqpId(u8 byEqpId) { m_byEqpId = byEqpId; }
    u8    GetEqpId( void ) { return m_byEqpId; }

	void Init( void );
	void SetNoSrc(){ SetSrcSsnId( 0 ); }
	void SetMsgBodyLen( u16  wMsgBodyLen );
	CServMsg( void );//constructor
	CServMsg( u8   * const pbyMsg, u16  wMsgLen );//constructor
	~CServMsg( void );//distructor
	void ClearHdr( void );//��Ϣͷ����
	CConfId GetConfId( void ) const;//��ȡ�������Ϣ
	void SetConfId( const CConfId & cConfId );//���û������Ϣ
	void SetNullConfId( void );//���û������ϢΪ��
	u16  GetMsgBodyLen( void ) const;//��ȡ��Ϣ�峤����Ϣ
	u16  GetMsgBody( u8   * pbyMsgBodyBuf, u16  wBufLen ) const;//��ȡ��Ϣ�壬���û�����BUFFER�������С���ضϴ���
	u8   * const GetMsgBody( void ) const;//��ȡ��Ϣ��ָ�룬�û������ṩBUFFER
	void SetMsgBody( u8   * const pbyMsgBody = NULL, u16  wLen = 0 );//������Ϣ��
	void CatMsgBody( u8   * const pbyMsgBody, u16  wLen );//�����Ϣ��
	u16  GetServMsgLen( void ) const;//��ȡ������Ϣ����
	u16  GetServMsg( u8   * pbyMsgBuf, u16  wBufLen ) const;//��ȡ������Ϣ�����û�����BUFFER�������С���ضϴ���
	u8   * const GetServMsg( void ) const;//��ȡ������Ϣָ�룬�û������ṩBUFFER
	void SetServMsg( u8   * const pbyMsg, u16  wLen );//����������Ϣ
	const CServMsg & operator= ( const CServMsg & cServMsg );//����������
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//�ش��ṹ (len:12)
struct TPrsParam
{
public:
	TTransportAddr   m_tLocalAddr;		//Ϊ���ؽ���RTP��ַ
	TTransportAddr	 m_tRemoteAddr;		//ΪԶ�˽���RTCP��ַ
public:
    void   SetLocalAddr(TTransportAddr tLocalAddr){ memcpy(&m_tLocalAddr,&tLocalAddr,sizeof(TTransportAddr)); } 
    TTransportAddr GetLocalAddr( void ) const { return m_tLocalAddr; }
    void   SetRemoteAddr(TTransportAddr tRemoteAddr){ memcpy(&m_tRemoteAddr,&tRemoteAddr,sizeof(TTransportAddr)); } 
    TTransportAddr GetRemoteAddr( void ) const { return m_tRemoteAddr; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//�ش�ʱ���Ƚṹ (len:16)
struct TPrsTimeSpan
{
    TPrsTimeSpan()
    {
        memset(this, 0, sizeof(TPrsTimeSpan));
    }
    u16  m_wFirstTimeSpan;	  //��һ���ش�����
	u16  m_wSecondTimeSpan;   //�ڶ����ش�����
	u16  m_wThirdTimeSpan;    //�������ش�����
	u16  m_wRejectTimeSpan;   //���ڶ�����ʱ����
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//�ش��豸״̬ (len:3)
struct TPrsPerChStatus
{
public:
	u8  byUsed;         //0--δ������1--����
	u8	byCount;	    //���ٸ�������
	u8  byChlReserved;  //�Ƿ�Ϊ����ͨ��
	u8  byConfIndex;		//��¼ͨ������Ļ���Idx, zgc, 2007/04/24
public:
    void   SetUsed( void ){ byUsed = 1; }
    void   SetIdle( void ){ byUsed = 0; }
    BOOL   IsUsed( void ){ return byUsed; }
    
    u8     GetFeedNum( void ){ return byCount; }
    void   FeedNumAdd( void ){ byCount++; }
    void   FeedNumDec( void ){ byCount--; }

	void   SetReserved( BOOL bReserved ){ byChlReserved = bReserved; }
	BOOL   IsReserved( void ){ return byChlReserved; }

	//��¼ͨ������Ļ���Idx, zgc, 2007/04/24
	void   SetConfIdx( u8 byConfIdx )
	{
		if( byConfIdx > MAXNUM_MCU_CONF )
		{
			OspPrintf( TRUE, FALSE, "The conf idx %d is error!\n", byConfIdx );
			return;
		}
		byConfIndex = byConfIdx; 
	}
	u8	   GetConfIdx( void ){ return byConfIndex; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//�ش��ṹ
struct TPrsStatus
{
public:
	TPrsPerChStatus   m_tPerChStatus[MAXNUM_PRS_CHNNL];		//���ؽ���RTP/RTCP���ĵ�ַ
	u8 GetIdleChlNum(void)
	{
		u8 nSum = 0;
		for(u8 byLp = 0; byLp < MAXNUM_PRS_CHNNL; byLp++)
		{
			if( !m_tPerChStatus[byLp].IsReserved() )
			{
				nSum++;
			}
		}
		return nSum;
	}

	//��¼ͨ������Ļ���Idx, zgc, 2007/04/24
	void SetChnConfIdx( u8 byChnIdx, u8 byConfIdx )
	{ 
		if( byChnIdx > MAXNUM_PRS_CHNNL || byConfIdx > MAXNUM_MCU_CONF )
		{
			OspPrintf( TRUE, FALSE, "The chn idx %d or conf idx %d is error!\n", byChnIdx, byConfIdx );
			return;
		}
		m_tPerChStatus[byChnIdx].SetConfIdx(byConfIdx); 
	}
	u8   GetChnConfIdx( u8 byChnIdx ){ return m_tPerChStatus[byChnIdx].GetConfIdx(); }
	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//���������£����˺ͶԶ��ⲿģ������ͬ�����(len: 2)
struct TMSSynState  
{
    //��ͻ����
    enum EntityType
    {
        emNone      = 0,
        emMC        = 1,
        emMp        = 2,
        emMtAdp     = 3,
        emPeriEqp   = 4,
        emDcs       = 5,
        emMpc       = 0xFF
    };
    
protected:
    u8  m_byEntityType;       //δͬ����ʵ������
    u8  m_byEntityId;         //δͬ����ʵ���ID
    
public:
    TMSSynState(void){ SetNull(); }
    
    void SetNull(void){ memset( this, 0, sizeof(TMSSynState) ); }
    
    void SetEntityType(u8 byEntityType){ m_byEntityType = byEntityType; }
    u8   GetEntityType( void ) const{ return m_byEntityType; }
    void SetEntityId(u8 byEntityId){ m_byEntityId = byEntityId; }
    u8   GetEntityId( void ) const { return m_byEntityId; }
    
    BOOL32 IsSynSucceed(void) const { return m_byEntityType == emNone ? TRUE : FALSE; }    
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//����MCU״̬�ṹ (len: 6+1 + 128*11 + 1+1*11 + 4 + 5 + 2 + 7 + 4 = 1449 )
struct TMcuStatus: public TMcu
{
	enum eMcuRunningStatusMask
    {
        Mask_Mp				=  0x00000004,
        Mask_Mtadp			=  0x00000008,
		Mask_HDSC			=  0x00000020,
		Mask_MDSC			=  0x00000040,
		Mask_DSC			=  0x00000080,
		Mask_McuRunOk		=  0x80000000
    };
	
	u8		m_byPeriEqpNum;	                        //������Ŀ
	TEqp	m_atPeriEqp[MAXNUM_OLDMCU_PERIEQP];	    //��������
	u8      m_byEqpOnline[MAXNUM_OLDMCU_PERIEQP];      //�����Ƿ�����
	u32     m_dwPeriEqpIpAddr[MAXNUM_OLDMCU_PERIEQP];  //����Ip��ַ
    
    u8      m_byPeriDcsNum;                         //DCS��Ŀ
    TEqp    m_atPeriDcs[MAXNUM_MCU_DCS];            //DCS����
    u8      m_byDcsOnline[MAXNUM_MCU_DCS];          //DCS�Ƿ�����
    u32     m_dwPeriDcsIpAddr[MAXNUM_MCU_DCS];      //DCS ip��ַ

    // xsl [8/26/2005] 
    u8      m_byOngoingConfNum;                     //mcu�ϼ�ʱ�������
    u8      m_byScheduleConfNum;                    //mcu��ԤԼ�������
    u16     m_wAllJoinedMtNum;                      //mcu����������ն˸���
    
    // ���� [5/29/2006]
    u8      m_byRegedGk;                            //�Ƿ�ɹ�ע��GK�����δ���û���ע��ʧ�ܣ���Ϊ0
	u16     m_wLicenseNum;							//��ǰLicense��
    TMSSynState m_tMSSynState;                      //��ǰ�������(������������Ч)
    
    // guzh [9/4/2006] 
    u8      m_byRegedMpNum;                         //��ǰ��ע���Mp����
    u8      m_byRegedMtAdpNum;                      //��ǰ��ע���H323 MtAdp����

    // guzh  [12/15/2006]
    u8      m_byNPlusState;                         //���ݷ���������״̬(mcuconst.h NPLUS_BAKSERVER_IDLE ��)
    u32     m_dwNPlusReplacedMcuIp;                 //������ݷ��������ڽ��湤���������ķ�����IP

	//zgc [12/21/2006]
	u8      m_byMcuIsConfiged;						//MCU�Ƿ����ù��ı�ʶ
	//zgc [07/26/2007]
	u8		m_byMcuCfgLevel;						// MCU������Ϣ�Ŀɿ�����
	
	//zgc [07/25/2007]
	//[31��24 23��16 15��8 7��0]
	//bit 2: no mp? ([0]no; [1]yes)
	//bit 3: no mtadp? ([0]no; [1]yes)
	//bit 4: no HDCS module?  ([0]no; [1]yes)
	//bit 5: no MDCS module?  ([0]no; [1]yes)
	//bit 7: no DSC module? ([0]no; [1]yes)
	//bit 31: MCU��ǰ�Ƿ���������? ([0]no; [1]yes)
	u32		m_dwMcuRunningState;					// MCU����״̬
	

protected:
    u32     m_dwPersistantRunningTime;              // MCU����ʱ�䣨��λ:s, linux����:497day��vx����:828day��
public:
	// xliang [11/20/2008] 
	u16		m_wAllHdiAccessMtNum;					// HDI��Ȩ����MT��������,����8000E��˵�����ֶα�������������
//	u16		m_wStdCriAccessMtNum;					// ��������������ݲ�֧�֣�Ԥ����
	u16	    m_wAudioMtAccessNum;					// �����ն˽���������滻�����m_wAccessPCMtNum����Ϊ���ֶ�û�б��õ��� [10/18/2012 chendaiwei]
	//u16		m_wAccessPCMtNum;						// ����Ԥ��תΪPCMT�������� // [3/10/2010 xliang] 
protected:
	u16		m_wVcsAccessNum;

	
public:
	u16 GetVcsAccessNum( void )
	{
		return ntohs(m_wVcsAccessNum);
	}

	void SetVcsAccessNum( u16 wVcsAccessNum )
	{
		m_wVcsAccessNum = htons( wVcsAccessNum );
	}

	void SetIsExistMp( BOOL32 IsExistMp ) 
	{ 
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_Mp, IsExistMp) 
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistMp(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_Mp);
	}
	void SetIsExistMtadp( BOOL32 IsExistMtadp ) 
	{ 
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_Mtadp, IsExistMtadp)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistMtadp(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_Mtadp);
	}
	void SetIsExistDSC( BOOL32 IsExistDSC ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_DSC, IsExistDSC)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistDSC(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_DSC); 
	}
	void SetIsExistMDSC( BOOL32 IsExistDSC ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_MDSC, IsExistDSC)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistMDSC(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_MDSC); 
	}
	void SetIsExistHDSC( BOOL32 IsExistDSC ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_HDSC, IsExistDSC)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistHDSC(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_HDSC); 
	}
	void SetIsMcuRunOk( BOOL32 IsMcuRunOk ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_McuRunOk, IsMcuRunOk)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsMcuRunOk(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_McuRunOk);
	}
    void SetPersistantRunningTime(u32 dwTime) { m_dwPersistantRunningTime = htonl(dwTime);    }
    u32  GetPersistantRunningTime(void) const { return ntohl(m_dwPersistantRunningTime);    }

    void Print(void) const
    {
        StaticLog( "MCU Current Status: \n" );
        StaticLog( "\tIs Run OK:%d\n", IsMcuRunOk() );
        StaticLog( "\tExist mp: %d, Exist mtadp: %d, Exist dsc module<DSC.%d, MDSC.%d, HDSC.%d>\n",
				IsExistMp(), IsExistMtadp(), IsExistDSC(), IsExistMDSC(), IsExistHDSC());

        StaticLog( "\tConfig file state: ");
        switch(m_byMcuCfgLevel) 
        {
        case MCUCFGINFO_LEVEL_NEWEST:
            StaticLog( "Success\n");
            break;
        case MCUCFGINFO_LEVEL_PARTNEWEST:
            StaticLog( "Partly success\n");
            break;
        case MCUCFGINFO_LEVEL_LAST:
            StaticLog( "Read fail\n");
            break;
        case MCUCFGINFO_LEVEL_DEFAULT:
            StaticLog( "No cfg file\n");
            break;
        default:
            StaticLog( "level error!\n");
            break;
		}
        StaticLog( "\tIs Mcu Configed: %d\n", m_byMcuIsConfiged);

        StaticLog( "\tIsReggedGk:%d, Mp Num:%d, H323MtAdp Num:%d, PeirEqpNum:%d, DcsNum:%d\n", 
                  m_byRegedGk, m_byRegedMpNum, m_byRegedMtAdpNum, m_byPeriEqpNum, m_byPeriDcsNum);
        
        StaticLog( "\tOngoingConf:%d, ScheduleConf:%d, AllJoinedMt:%d\n", 
                  m_byOngoingConfNum, m_byScheduleConfNum, ntohs(m_wAllJoinedMtNum));

        if ( m_byNPlusState != NPLUS_NONE )
        {
            StaticLog( "\tNPlusState:%d, NPlus Replaced Mcu:0x%x\n", 
                m_byNPlusState, ntohl(m_dwNPlusReplacedMcuIp));
        }
        
        if (!m_tMSSynState.IsSynSucceed() )
        {
            StaticLog( "\tMS conflict entity ID:%d, Type:%d\n", 
                      m_tMSSynState.GetEntityId(), m_tMSSynState.GetEntityType() );
        }
        {
            u32 dwPersistantTime = GetPersistantRunningTime();

            u32 dwCutOffTime = 0;
            u32 dwDay = dwPersistantTime/(3600*24);
            dwCutOffTime += (3600*24) * dwDay;
            u32 dwHour = (dwPersistantTime - dwCutOffTime)/3600;
            dwCutOffTime += 3600 * dwHour;
            u32 dwMinute = (dwPersistantTime - dwCutOffTime)/60;
            dwCutOffTime += 60 * dwMinute;
            u32 dwSecond = dwPersistantTime - dwCutOffTime;
            if ( 0 == dwDay )
            {
                StaticLog( "\tPersistant running time: %d.h %d.m %d.s\n",
                                         dwHour, dwMinute, dwSecond );                
            }
            else
            {
                StaticLog( "\tPersistant running time: %d.day %d.h %d.m %d.s\n",
                                         dwDay, dwHour, dwMinute, dwSecond );                
            }
        }
		// xliang [11/20/2008] HDI �����ն�����
		StaticLog( "\tHDI access Mt Num: %d\n", ntohs(m_wAllHdiAccessMtNum));
		StaticLog( "\taccess audio Mt Num: %d\n", ntohs(m_wAudioMtAccessNum));
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;



struct TMcuStatusAfterV4R6B2: public TMcu
{
	enum eMcuRunningStatusMask
    {
        Mask_Mp				=  0x00000004,
        Mask_Mtadp			=  0x00000008,
		Mask_HDSC			=  0x00000020,
		Mask_MDSC			=  0x00000040,
		Mask_DSC			=  0x00000080,
		Mask_McuRunOk		=  0x80000000
    };
	
	u8		m_byPeriEqpNum;	                        //������Ŀ
	TEqp	m_atPeriEqp[MAXNUM_MCU_PERIEQP];	    //��������
	u8      m_byEqpOnline[MAXNUM_MCU_PERIEQP];      //�����Ƿ�����
	u32     m_dwPeriEqpIpAddr[MAXNUM_MCU_PERIEQP];  //����Ip��ַ
    
    u8      m_byPeriDcsNum;                         //DCS��Ŀ
    TEqp    m_atPeriDcs[MAXNUM_MCU_DCS];            //DCS����
    u8      m_byDcsOnline[MAXNUM_MCU_DCS];          //DCS�Ƿ�����
    u32     m_dwPeriDcsIpAddr[MAXNUM_MCU_DCS];      //DCS ip��ַ

    // xsl [8/26/2005] 
    u8      m_byOngoingConfNum;                     //mcu�ϼ�ʱ�������
    u8      m_byScheduleConfNum;                    //mcu��ԤԼ�������
    u16     m_wAllJoinedMtNum;                      //mcu����������ն˸���
    
    // ���� [5/29/2006]
    u8      m_byRegedGk;                            //�Ƿ�ɹ�ע��GK�����δ���û���ע��ʧ�ܣ���Ϊ0
	u16     m_wLicenseNum;							//��ǰLicense��
    TMSSynState m_tMSSynState;                      //��ǰ�������(������������Ч)
    
    // guzh [9/4/2006] 
    u8      m_byRegedMpNum;                         //��ǰ��ע���Mp����
    u8      m_byRegedMtAdpNum;                      //��ǰ��ע���H323 MtAdp����

    // guzh  [12/15/2006]
    u8      m_byNPlusState;                         //���ݷ���������״̬(mcuconst.h NPLUS_BAKSERVER_IDLE ��)
    u32     m_dwNPlusReplacedMcuIp;                 //������ݷ��������ڽ��湤���������ķ�����IP

	//zgc [12/21/2006]
	u8      m_byMcuIsConfiged;						//MCU�Ƿ����ù��ı�ʶ
	//zgc [07/26/2007]
	u8		m_byMcuCfgLevel;						// MCU������Ϣ�Ŀɿ�����
	
	//zgc [07/25/2007]
	//[31��24 23��16 15��8 7��0]
	//bit 2: no mp? ([0]no; [1]yes)
	//bit 3: no mtadp? ([0]no; [1]yes)
	//bit 4: no HDCS module?  ([0]no; [1]yes)
	//bit 5: no MDCS module?  ([0]no; [1]yes)
	//bit 7: no DSC module? ([0]no; [1]yes)
	//bit 31: MCU��ǰ�Ƿ���������? ([0]no; [1]yes)
	u32		m_dwMcuRunningState;					// MCU����״̬
	

protected:
    u32     m_dwPersistantRunningTime;              // MCU����ʱ�䣨��λ:s, linux����:497day��vx����:828day��
public:
	// xliang [11/20/2008] 
	u16		m_wAllHdiAccessMtNum;					// HDI��Ȩ����MT��������,����8000E��˵�����ֶα�������������
//	u16		m_wStdCriAccessMtNum;					// ��������������ݲ�֧�֣�Ԥ����
	u16		m_wAccessPCMtNum;						// ����Ԥ��תΪPCMT�������� // [3/10/2010 xliang] 
public:
	u8 GetPeriEqpNum()
	{
		return m_byPeriEqpNum;
	}

	void SetIsExistMp( BOOL32 IsExistMp ) 
	{ 
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_Mp, IsExistMp) 
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistMp(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_Mp);
	}
	void SetIsExistMtadp( BOOL32 IsExistMtadp ) 
	{ 
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_Mtadp, IsExistMtadp)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistMtadp(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_Mtadp);
	}
	void SetIsExistDSC( BOOL32 IsExistDSC ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_DSC, IsExistDSC)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistDSC(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_DSC); 
	}
	void SetIsExistMDSC( BOOL32 IsExistDSC ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_MDSC, IsExistDSC)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistMDSC(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_MDSC); 
	}
	void SetIsExistHDSC( BOOL32 IsExistDSC ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_HDSC, IsExistDSC)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistHDSC(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_HDSC); 
	}
	void SetIsMcuRunOk( BOOL32 IsMcuRunOk ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_McuRunOk, IsMcuRunOk)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsMcuRunOk(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_McuRunOk);
	}
    void SetPersistantRunningTime(u32 dwTime) { m_dwPersistantRunningTime = htonl(dwTime);    }
    u32  GetPersistantRunningTime(void) const { return ntohl(m_dwPersistantRunningTime);    }

    void Print(void) const
    {
        OspPrintf(TRUE, FALSE, "MCU Current Status: \n" );
        OspPrintf(TRUE, FALSE, "\tIs Run OK:%d\n", IsMcuRunOk() );
        OspPrintf(TRUE, FALSE, "\tExist mp: %d, Exist mtadp: %d, Exist dsc module<DSC.%d, MDSC.%d, HDSC.%d>\n",
				IsExistMp(), IsExistMtadp(), IsExistDSC(), IsExistMDSC(), IsExistHDSC());

        OspPrintf(TRUE, FALSE, "\tConfig file state: ");
        switch(m_byMcuCfgLevel) 
        {
        case MCUCFGINFO_LEVEL_NEWEST:
            OspPrintf(TRUE, FALSE, "Success\n");
            break;
        case MCUCFGINFO_LEVEL_PARTNEWEST:
            OspPrintf(TRUE, FALSE, "Partly success\n");
            break;
        case MCUCFGINFO_LEVEL_LAST:
            OspPrintf(TRUE, FALSE, "Read fail\n");
            break;
        case MCUCFGINFO_LEVEL_DEFAULT:
            OspPrintf(TRUE, FALSE, "No cfg file\n");
            break;
        default:
            OspPrintf(TRUE, FALSE, "level error!\n");
            break;
		}
        OspPrintf(TRUE, FALSE, "\tIs Mcu Configed: %d\n", m_byMcuIsConfiged);

        OspPrintf(TRUE, FALSE, "\tIsReggedGk:%d, Mp Num:%d, H323MtAdp Num:%d, PeirEqpNum:%d, DcsNum:%d\n", 
                  m_byRegedGk, m_byRegedMpNum, m_byRegedMtAdpNum, m_byPeriEqpNum, m_byPeriDcsNum);
        
        OspPrintf(TRUE, FALSE, "\tOngoingConf:%d, ScheduleConf:%d, AllJoinedMt:%d\n", 
                  m_byOngoingConfNum, m_byScheduleConfNum, ntohs(m_wAllJoinedMtNum));

        if ( m_byNPlusState != NPLUS_NONE )
        {
            OspPrintf(TRUE, FALSE, "\tNPlusState:%d, NPlus Replaced Mcu:0x%x\n", 
                m_byNPlusState, ntohl(m_dwNPlusReplacedMcuIp));
        }
        
        if (!m_tMSSynState.IsSynSucceed() )
        {
            OspPrintf(TRUE, FALSE, "\tMS conflict entity ID:%d, Type:%d\n", 
                      m_tMSSynState.GetEntityId(), m_tMSSynState.GetEntityType() );
        }
        {
            u32 dwPersistantTime = GetPersistantRunningTime();

            u32 dwCutOffTime = 0;
            u32 dwDay = dwPersistantTime/(3600*24);
            dwCutOffTime += (3600*24) * dwDay;
            u32 dwHour = (dwPersistantTime - dwCutOffTime)/3600;
            dwCutOffTime += 3600 * dwHour;
            u32 dwMinute = (dwPersistantTime - dwCutOffTime)/60;
            dwCutOffTime += 60 * dwMinute;
            u32 dwSecond = dwPersistantTime - dwCutOffTime;
            if ( 0 == dwDay )
            {
                OspPrintf(TRUE, FALSE, "\tPersistant running time: %d.h %d.m %d.s\n",
                                         dwHour, dwMinute, dwSecond );                
            }
            else
            {
                OspPrintf(TRUE, FALSE, "\tPersistant running time: %d.day %d.h %d.m %d.s\n",
                                         dwDay, dwHour, dwMinute, dwSecond );                
            }
        }
		// xliang [11/20/2008] HDI �����ն�����
		OspPrintf(TRUE, FALSE, "\tHDI access Mt Num: %d\n", ntohs(m_wAllHdiAccessMtNum));
		OspPrintf(TRUE, FALSE, "\taccess PCMt Num: %d\n", ntohs(m_wAccessPCMtNum));
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


struct TRecState 
{
protected:	
	u8      m_byRecState;  //��ǰ�ն�¼��״̬
							//bit3-4: 00:��¼�� 01:¼�� 10:¼����ͣ
							//bit0:   0:ʵʱ¼�� 1:��֡¼��
							//bit5-6:00:¼��ͨ��idxΪ0��01:¼��ͨ��idxΪ1��10��¼��ͨ��idxΪ2
							//bit1:   0:������ 1:����
public:	
	BOOL IsNoRecording( void ) const	{ return( ( m_byRecState & 0x18 ) == 0x00 ? TRUE : FALSE ); }
	void SetNoRecording( void )	{ m_byRecState &= ~0x18; }
	BOOL IsRecording( void ) const	{ return( ( m_byRecState & 0x18 ) == 0x08 ? TRUE : FALSE ); }
	void SetRecording( void )	{ SetNoRecording(); m_byRecState |= 0x08; }
	BOOL IsRecPause( void ) const	{ return( ( m_byRecState & 0x18 ) == 0x10 ? TRUE : FALSE ); }
	void SetRecPause( void )	{ SetNoRecording(); m_byRecState |= 0x10; }
	//�Ƿ��ڳ�֡¼��״̬
	BOOL IsRecSkipFrame() const { return ( !IsNoRecording() && ( ( m_byRecState & 0x01 ) == 0x01 ) ); }
	void SetRecSkipFrame( BOOL bSkipFrame )      { if( bSkipFrame ) m_byRecState |= 0x01;else m_byRecState &= ~0x01;};
	//�ն�¼���Ƿ񷢲�
	BOOL IsRecPublish() const { return ( ( m_byRecState & 0x02 ) == 0x02 ); }
	void SetRecPublish( BOOL bPublish )      { if( bPublish ) m_byRecState |= 0x02;else m_byRecState &= ~0x02;};
	
	void SetRecChannel(u8 byChannelIdx)
	{
		if (IsNoRecording())
		{
			return;
		}
		//�������channelidx��λ
		m_byRecState &= 0x9F;//10011111 
		//����Ϊchannelidx��Ӧ��ֵ
		m_byRecState |= byChannelIdx<<5; //�磺channel idx:1<<5=0��01��00000 �õ���Ӧ��mask������|
	}
	u8 GetRecChannelIdx()const
	{
		if (IsNoRecording())
		{
			return 0xFF;
		}
		else
		{
			return (m_byRecState & 0x60)>>5;//��01100000��&��ȡ����5-6λ����0��ʼ����Ӧ��channelidx
		}
		
	}

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//�ն����ʽṹ len: 14
struct TMtBitrate : public TMt
{
protected:
    u16     m_wSendBitRate;              //�ն˷�������(��λ:Kbps,1K=1024)
    u16     m_wRecvBitRate;              //�ն˽�������(��λ:Kbps,1K=1024)
    u16     m_wH239SendBitRate;          //�ն˵ڶ�·��Ƶ��������(��λ:Kbps,1K=1024)
    u16     m_wH239RecvBitRate;          //�ն˵ڶ�·��Ƶ��������(��λ:Kbps,1K=1024)
    
public:
    TMtBitrate(void) { memset(this, 0, sizeof(TMtBitrate)); }
    void   SetSendBitRate(u16  wSendBitRate){ m_wSendBitRate = htons(wSendBitRate);} 
    u16    GetSendBitRate( void ) const { return ntohs(m_wSendBitRate); }
    void   SetRecvBitRate(u16  wRecvBitRate){ m_wRecvBitRate = htons(wRecvBitRate);} 
    u16    GetRecvBitRate( void ) const { return ntohs(m_wRecvBitRate); }
    void   SetH239SendBitRate(u16  wH239SendBitRate){ m_wH239SendBitRate = htons(wH239SendBitRate);} 
    u16    GetH239SendBitRate( void ) const { return ntohs(m_wH239SendBitRate); }
    void   SetH239RecvBitRate(u16  wH239RecvBitRate){ m_wH239RecvBitRate = htons(wH239RecvBitRate);} 
    u16    GetH239RecvBitRate( void ) const { return ntohs(m_wH239RecvBitRate); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//�ն�״̬�ṹ(len:64)
struct TMtStatus : public TMtBitrate
{
protected:
    u8      m_byDecoderMute;             //�Ƿ���뾲��
	u8      m_byCaptureMute;             //�Ƿ�ɼ�����
    u8      m_byHasMatrix;               //�Ƿ��о���
	u8      m_byIsEnableFECC;            //�Ƿ���ң������ͷ
	u8      m_bySendAudio;               //�Ƿ����ڴ�����Ƶ
	u8      m_bySendVideo;               //�Ƿ����ڴ�����Ƶ
	u8      m_byReceiveVideo;            //�Ƿ����ڽ�����Ƶ
	u8      m_byReceiveAudio;            //�Ƿ����ڽ�����Ƶ
	u8      m_byRcvVideo2;				 //�Ƿ��ڽ��յڶ���Ƶ
	u8      m_bySndVideo2;               //�Ƿ��ڷ��͵ڶ���Ƶ
	u8      m_byInMixing;                //�Ƿ����ڲμӻ���
	//zjj20100329 ע���Ƿ񶳽�ͼ��,�����ն�����ԭ��
	//u8      m_byVideoFreeze;             //�Ƿ񶳽�ͼ��
	//zjj20100327
	u8		m_byMtDisconnectReason;		 //�ն˺���ʧ��ԭ��
    // guzh [3/7/2007] ���ֶδ�4.0R4��ʼȡ��������Ϊ�Ƿ���ƵԴ��ʧ
	//u8      m_bySelPolling;              //�Ƿ�������ѯѡ��
    u8      m_byVideoLose;               //�Ƿ���ƵԴ��ʧ
	u8      m_bySelByMcsDrag;            //�Ƿ��ǻ������ѡ��
    u8      m_byDecoderVolume;           //��������
	u8      m_byCaptureVolume;           //�ɼ�����
	u8  	m_byCurVideo;	             //��ǰ��ƵԴ(1)
	u8  	m_byCurAudio;	             //��ǰ��ƵԴ(1)    
	u8      m_byH239VideoFormat;         //�ն˵ڶ�·��Ƶ�ֱ���
	u8      m_byMtBoardType;             //�ն˵İ忨����(MT_BOARD_WIN, MT_BOARD_8010 ��)
    u8      m_byInTvWall;                //�ն��Ƿ����ڵ���ǽ��
    u8      m_byInHdu;                   //�ն��Ƿ�����HDU��   4.6 �¼�   jlb
    u8      m_byExVideoSourceNum;        //�ն���չ��ƵԴ����
	u8		m_byVideoMediaLoop;			 //�ն�Զ�˻���״̬
	u8		m_byAudioMediaLoop;			 //�ն�Զ�˻���״̬
    u8      m_byIsInMixGrp;              //�Ƿ��ڻ�������
public:
	TMt		m_tVideoMt;		             //��ǰ���յ���Ƶ�ն�
	TMt		m_tAudioMt;		             //��ǰ���յ���Ƶ�ն�
	TRecState m_tRecState;               //�ն�¼��״̬
protected:
    TMt     m_tLastSelectVidMt;          //ѡ������Ƶ�ն�
	TMt	    m_tLastSelectAudMt;          //ѡ������Ƶ�ն�
public:
	TMtStatus( void )
	{
        Clear();
    }

    void Clear()
    {
		memset( this,0,sizeof(TMtStatus));
		SetDecoderMute(FALSE);
		SetCaptureMute(FALSE);
		SetHasMatrix(TRUE);
		SetIsEnableFECC(FALSE);
		SetSendAudio(FALSE);
		SetSendVideo(FALSE);
		SetInMixing(FALSE);
		SetReceiveVideo(FALSE);
        SetReceiveAudio(FALSE);
		//SetVideoFreeze(FALSE);
		//SetSelPolling(FALSE);
        SetVideoLose(FALSE);
		SetSelByMcsDrag(FALSE);
		SetDecoderVolume(16);
		SetCaptureVolume(16);
		SetCurVideo(1);
		SetCurAudio(1);		
		SetRcvVideo2(FALSE);
		SetSndVideo2(FALSE);
        SetInTvWall(FALSE);
        SetIsInMixGrp(FALSE);
        SetMtBoardType(MT_BOARD_UNKNOW);
	}

    void   SetDecoderMute(BOOL bDecoderMute){ m_byDecoderMute = GETBBYTE(bDecoderMute);} 
    BOOL   IsDecoderMute( void ) const { return ISTRUE(m_byDecoderMute); }
    void   SetCaptureMute(BOOL bCaptureMute){ m_byCaptureMute = GETBBYTE(bCaptureMute);} 
    BOOL   IsCaptureMute( void ) const { return ISTRUE(m_byCaptureMute); }
    void   SetHasMatrix(BOOL bHasMatrix){ m_byHasMatrix = GETBBYTE(bHasMatrix);} 
    BOOL   IsHasMatrix( void ) const { return ISTRUE(m_byHasMatrix); }
    void   SetIsEnableFECC(BOOL bCamRCEnable){ m_byIsEnableFECC = GETBBYTE(bCamRCEnable);} 
    BOOL   IsEnableFECC( void ) const { return ISTRUE(m_byIsEnableFECC); }
    void   SetSendAudio(BOOL bSendAudio){ m_bySendAudio = GETBBYTE(bSendAudio);} 
    BOOL   IsSendAudio( void ) const { return ISTRUE(m_bySendAudio); }
    void   SetSendVideo(BOOL bSendVideo){ m_bySendVideo = GETBBYTE(bSendVideo);} 
    BOOL   IsSendVideo( void ) const { return ISTRUE(m_bySendVideo); }
    void   SetInMixing(BOOL bInMixing){ m_byInMixing = GETBBYTE(bInMixing);} 
    BOOL   IsInMixing( void ) const { return ISTRUE(m_byInMixing); }
    void   SetReceiveAudio(BOOL bRcv) { m_byReceiveAudio = GETBBYTE(bRcv); }
    BOOL   IsReceiveAudio() const { return ISTRUE(m_byReceiveAudio); }
	void   SetReceiveVideo(BOOL bReceiveVideo){ m_byReceiveVideo = GETBBYTE(bReceiveVideo);} 
    BOOL   IsReceiveVideo( void ) const { return ISTRUE(m_byReceiveVideo); }
    /*void   SetVideoFreeze(BOOL bVideoFreeze){ m_byVideoFreeze = GETBBYTE(bVideoFreeze);} 
    BOOL   IsVideoFreeze( void ) const { return ISTRUE(m_byVideoFreeze); }*/
    /*
    void   SetSelPolling(BOOL bSelPolling){ m_bySelPolling = GETBBYTE(bSelPolling);} 
    BOOL   IsSelPolling( void ) const { return ISTRUE(m_bySelPolling); }
    */
    void   SetVideoLose(BOOL bIsLose){ m_byVideoLose = GETBBYTE(bIsLose);} 
    BOOL   IsVideoLose( void ) const { return ISTRUE(m_byVideoLose); }

    void   SetIsInMixGrp(BOOL bDiscuss) { m_byIsInMixGrp = GETBBYTE(bDiscuss); }
    BOOL   IsInMixGrp(void) const { return ISTRUE(m_byIsInMixGrp); }    
    void   SetSelByMcsDrag( u8 bySelMediaMode ){ m_bySelByMcsDrag = bySelMediaMode;} 
    u8     GetSelByMcsDragMode( void ) const { return m_bySelByMcsDrag; }
    void   SetDecoderVolume(u8   byDecoderVolume){ m_byDecoderVolume = byDecoderVolume;} 
    u8     GetDecoderVolume( void ) const { return m_byDecoderVolume; }
    void   SetCaptureVolume(u8   byCaptureVolume){ m_byCaptureVolume = byCaptureVolume;} 
    u8     GetCaptureVolume( void ) const { return m_byCaptureVolume; }
    void   SetCurVideo(u8   byCurVideo){ m_byCurVideo = byCurVideo;} 
    u8     GetCurVideo( void ) const { return m_byCurVideo; }
    void   SetCurAudio(u8   byCurAudio){ m_byCurAudio = byCurAudio;} 
    u8     GetCurAudio( void ) const { return m_byCurAudio; }    
    void   SetH239VideoFormat(u8 byH239VideoFormat){ m_byH239VideoFormat = byH239VideoFormat;} 
    u8     GetH239VideoFormat( void ) const { return m_byH239VideoFormat; }
    void   SetMtBoardType(u8 byType) { m_byMtBoardType = byType; }
	u8     GetMtBoardType() const {return m_byMtBoardType; }
	void   SetVideoMt(TMt tVideoMt){ m_tVideoMt = tVideoMt;} 
    TMt    GetVideoMt( void ) const { return m_tVideoMt; }
    void   SetAudioMt(TMt tAudioMt){ m_tAudioMt = tAudioMt;} 
    TMt    GetAudioMt( void ) const { return m_tAudioMt; }

	//zjj20100327
	void   SetMtDisconnectReason( u8 byReason ){ m_byMtDisconnectReason = byReason; }
	u8	   GetMtDisconnectReason( void ) const { return m_byMtDisconnectReason; }	

    void	SetMediaLoop(u8 byMode, BOOL32 bOn) 
    { 
        if (MODE_VIDEO == byMode)
        {
            m_byVideoMediaLoop = GETBBYTE(bOn);
        }
        else if (MODE_AUDIO == byMode)
        {
            m_byAudioMediaLoop = GETBBYTE(bOn);
        }
    }
    BOOL    IsMediaLoop(u8 byMode)
    {
        if (MODE_VIDEO == byMode)
        {
            return ISTRUE(m_byVideoMediaLoop); 
        }
        else if (MODE_AUDIO == byMode)
        {
            return ISTRUE(m_byAudioMediaLoop); 
        }

        return FALSE;
    }
    // guzh [2/28/2007] ������Ϊѡ���б�ͻָ�ʹ�ã������ʹ��
	void   SetSelectMt(TMt tSeleteMt, u8 byMode ) 
    {
        if ( byMode == MODE_AUDIO || byMode == MODE_BOTH )
        {
            m_tLastSelectAudMt = tSeleteMt;
        }
        if ( byMode == MODE_VIDEO || byMode == MODE_BOTH )
        {
            m_tLastSelectVidMt = tSeleteMt;
        }        
    }
	TMt    GetSelectMt( u8 byMode ) const 
    { 
        if ( byMode == MODE_AUDIO )
        {
            return m_tLastSelectAudMt;
        }
        else if ( byMode == MODE_VIDEO )
        {
            return m_tLastSelectVidMt;
        }    
        else 
        {
            TMt tNullMt;
            tNullMt.SetNull();
            return tNullMt;
        }
    }

	void   SetRcvVideo2(BOOL bRcv) { m_byRcvVideo2 = GETBBYTE(bRcv);}
    BOOL   IsRcvVideo2() const {return ISTRUE(m_byRcvVideo2);}
    void   SetSndVideo2(BOOL bSend) { m_bySndVideo2 = GETBBYTE(bSend); }
	BOOL   IsSndVideo2() const { return ISTRUE(m_bySndVideo2); }
    void   SetExVideoSrcNum(u8 byNum) { m_byExVideoSourceNum = byNum; }
    u8     GetExVideoSrcNum(void) const { return m_byExVideoSourceNum; }

    void   SetInTvWall(BOOL bInTvWall){ m_byInTvWall = GETBBYTE(bInTvWall);} 
    BOOL   IsInTvWall( void ) const { return ISTRUE(m_byInTvWall); }
	//4.6 �¼� jlb
	void   SetInHdu(BOOL bInHdu){ m_byInHdu = GETBBYTE(bInHdu);} 
    BOOL   IsInHdu( void ) const { return ISTRUE(m_byInHdu); }

	void  SetTMt( TMt tMt ){ memcpy( this, &tMt, sizeof(tMt) ); }
	const TMt & GetMt( void ) const; //��ȡ�ն˽ṹ

	//m_bySelByMcsDrag����λ��ʶѡ��ģʽ
	//MODE_VIDEO		0001
	//MODE_AUDIO		0010
	//MODE_BOTH			0011
	//MODE_VIDEO2SECOND	1100
	void  AddSelByMcsMode(u8 byMode)
	{
		if (MODE_AUDIO != byMode && MODE_VIDEO != byMode && MODE_BOTH != byMode && MODE_VIDEO2SECOND != byMode)
		{
			return;
		}
		m_bySelByMcsDrag = m_bySelByMcsDrag | byMode;
	}

	BOOL32  HasModeInSelMode(u8 byMode)
	{
		if (MODE_BOTH == byMode)
		{
			return (m_bySelByMcsDrag & MODE_AUDIO) && (m_bySelByMcsDrag & MODE_VIDEO);
		}
		else if (MODE_AUDIO == byMode || MODE_VIDEO == byMode || MODE_VIDEO2SECOND == byMode)
		{
			return m_bySelByMcsDrag & byMode;
		}
		return FALSE;
	}
	
	void RemoveSelByMcsMode(u8 byMode)
	{
		if (MODE_AUDIO != byMode && MODE_VIDEO != byMode && MODE_BOTH != byMode && MODE_VIDEO2SECOND != byMode)
		{
			return;
		}

		if (m_bySelByMcsDrag == 0)
		{
			return;
		}
		
		m_bySelByMcsDrag = (~byMode) & m_bySelByMcsDrag;
	}

	void Print( void ) const
	{
		StaticLog( "Mcu%dMt%d status:\n", GetMcuId(), GetMtId() ); 
        StaticLog( "	m_byMtBoardType = %d\n", m_byMtBoardType ); 
		StaticLog( "	m_byDecoderMute = %d\n", m_byDecoderMute ); 
		StaticLog( "	m_byCaptureMute = %d\n", m_byCaptureMute );
		StaticLog( "	m_byHasMatrix = %d\n", m_byHasMatrix );
        StaticLog( "	m_byExVideoSourceNum = %d\n", m_byExVideoSourceNum );
		StaticLog( "	m_byIsEnableFECC = %d\n", m_byIsEnableFECC );
		StaticLog( "	m_bySendAudio = %d\n", m_bySendAudio );
		StaticLog( "	m_bySendVideo = %d\n", m_bySendVideo );
		StaticLog( "	m_bySendVideo2 = %d\n", m_bySndVideo2 );
        StaticLog( "   m_byRecvAudio = %d\n", m_byReceiveAudio );
        StaticLog( "   m_byRecvVideo = %d\n", m_byReceiveVideo );
		StaticLog( "   m_byRecvVideo2 = %d\n", m_byRcvVideo2 );
		StaticLog( "	m_byInMixing = %d\n", m_byInMixing );
		StaticLog( "	m_byInTvWall = %d\n", m_byInTvWall );
		StaticLog( "	m_byInHdu = %d\n",    m_byInHdu );		
        StaticLog( "	m_byIsInMixGrp = %d\n", m_byIsInMixGrp );
		StaticLog( "	m_byVideoLose = %d\n", m_byVideoLose );
		StaticLog( "	m_bySelByMcsDrag = %d\n", m_bySelByMcsDrag );
		StaticLog( "	m_byDecoderVolume = %d\n", m_byDecoderVolume );
		StaticLog( "	m_byCurVideo = %d\n", m_byCurVideo );
        StaticLog( "	m_byCurAudio = %d\n", m_byCurAudio );
		StaticLog( "	m_wSendBitRate = %d\n", ntohs(m_wSendBitRate) );
		StaticLog( "	m_wRecvBitRate = %d\n", ntohs(m_wRecvBitRate) );
		StaticLog( "	m_wH239SendBitRate = %d\n", ntohs(m_wH239SendBitRate) );
		StaticLog( "	m_wH239RecvBitRate = %d\n", ntohs(m_wH239RecvBitRate) );
		StaticLog( "	m_byH239VideoFormat = %d\n", m_byH239VideoFormat );
        StaticLog( "	m_byVideoMediaLoop = %d\n", m_byVideoMediaLoop );
        StaticLog( "	m_byAudioMediaLoop = %d\n", m_byAudioMediaLoop );
		StaticLog( "	m_byMtDisconnectReason = %d\n", m_byMtDisconnectReason );
		StaticLog("		IsNoRecoding = %d\n", m_tRecState.IsNoRecording());
		StaticLog("		RecChnnelIdx = %d\n", m_tRecState.GetRecChannelIdx());
		if( m_tVideoMt.GetType() == TYPE_MT )
		{
			StaticLog( "	SrcVideo: mcu%dmt%d\n", m_tVideoMt.GetMcuId(), m_tVideoMt.GetMtId() );
		}
		else
		{
            if (m_tVideoMt.GetEqpId() != 0)
            {
                StaticLog( "	SrcVideo: eqp%d\n", m_tVideoMt.GetEqpId() );
            }
            else
            {
                StaticLog( "	SrcVideo: NULL\n" );
            }
			
		}

		if( m_tAudioMt.GetType() == TYPE_MT )
		{
			StaticLog( "	SrcAudio: mcu%dmt%d\n", m_tAudioMt.GetMcuId(), m_tAudioMt.GetMtId() );
		}
		else
		{
            if (m_tAudioMt.GetEqpId() != 0)
            {
                StaticLog( "	SrcAudio: eqp%d\n", m_tAudioMt.GetEqpId() );
            }
            else
            {
                StaticLog( "	SrcAudio: NULL\n" );
            }			
		}

        if (!m_tLastSelectVidMt.IsNull())
        {   
            StaticLog( "	Select Video MT: mcu%dmt%d\n", 
                m_tLastSelectVidMt.GetMcuId(), m_tLastSelectVidMt.GetMtId() );
        }
        else
        {
            StaticLog( "	Select Video MT: NULL\n" );
        }
        if (!m_tLastSelectAudMt.IsNull())
        {   
            StaticLog( "	Select Audio MT: mcu%dmt%d\n", 
                   m_tLastSelectAudMt.GetMcuId(), m_tLastSelectAudMt.GetMtId() );
        }
        else
        {
            StaticLog( "	Select Audio MT: NULL\n" );
        }      
	}

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TTWMember : public TMtNoConstruct
{
    u8  byMemberType;     //��Ա����
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// [5/25/2011 liuxu] ����Ҫ�ٶ���THduMember, ֱ�Ӹ���TTWMember����
typedef TTWMember THduMember;
//HDU�ն˳�Ա
// struct THduMember:public TMtNoConstruct
// {
//     u8 byMemberType;
// }
// #ifndef WIN32
// __attribute__ ( (packed) ) 
// #endif
// ;


//�������ֵ���ǽ״̬�ṹ
struct TTvWallStatus
{
    u8        byOutputMode;  //TW_OUTPUTMODE_AUDIO : ֻ�����Ƶ
                             //TW_OUTPUTMODE_VIDEO : ֻ�����Ƶ
                             //TW_OUTPUTMODE_BOTH  : ͬʱ�������Ƶ
    u8        byChnnlNum;    //�ŵ���
    TTWMember atVideoMt[MAXNUM_PERIEQP_CHNNL]; //ÿ���ŵ���Ӧ�նˣ�MCU��Ϊ0��ʾ��
}
#ifndef WIN32
__attribute__((packed))
#endif
;

/*----------------------------------------------------------------------
�ṹ����THduChnStatus
��;  ��
----------------------------------------------------------------------*/
struct THduChnStatus
{
    enum EHduChnStatus
    {
        eIDLE        = 0,   //����
        eREADY       = 2,   //׼��(������ж�״̬����1ʱ��ʾHDUͨ������)
		eWAITSTART   = 3,	//�ȴ���ʼ
		eWAITSTOP    = 4,	//�ȴ��ر�
		eWAITCHGMODE = 5,	//�ȴ�����л�
		eRUNNING     = 6    //��ʾHDU��ĳ��ͨ�����ڱ���ʹ��
    };

public:

    void   SetStatus( u8 byStatus ) { m_byStatus = byStatus; };
    u8     GetStatus( void )  const { return m_byStatus; }
    
    BOOL32 IsNull( void ) const { return m_tHdu.IsNull(); }
    void   SetNull( void ) { m_tHdu.SetNull(); }
        
    void   SetChnIdx( u8 byChnIdx ) { m_byChnIdx = byChnIdx; }
    u8     GetChnIdx( void ) const { return m_byChnIdx; }
    
    void   SetEqp( TEqp tHdu ) { m_tHdu = tHdu; }
    TEqp   GetEqp( void ) const { return m_tHdu; }
    u8     GetEqpId( void ) const { return m_tHdu.GetEqpId(); }	

    void   SetSchemeIdx( u8 bySchemeIdx ) { m_bySchemeIdx = bySchemeIdx; };
    u8     GetSchemeIdx( void ) { return m_bySchemeIdx; }

	u8     GetVolume( void ) const { return m_byVolume; }
	void   SetVolume( u8 byVolume ){ m_byVolume = byVolume; }

	BOOL32   GetIsMute( void ) const { return m_byMute; }
	void   SetIsMute( BOOL32 byIsMute ){ m_byMute = GETBBYTE(byIsMute); }

private:
	u8 m_byStatus;	  // HduChnStatus
	u8 m_byChnIdx;
    u8 m_bySchemeIdx; // ���ڱ�ʹ��Ԥ���������� 
    u8 m_byVolume;
	u8 m_byMute;      // �Ƿ���
    TEqpNoConstruct m_tHdu;

} 
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;


//HDU״̬�ṹ
struct THduStatus
{
    u8        byOutputMode;  //HDU_OUTPUTMODE_AUDIO : ֻ�����Ƶ
	                         //HDU_OUTPUTMODE_VIDEO : ֻ�����Ƶ
	                         //HDU_OUTPUTMODE_BOTH  : ͬʱ�������Ƶ
    u8        byChnnlNum;    //�ŵ���

    THduMember atVideoMt[MAXNUM_HDU_CHANNEL]; //ÿ���ŵ���Ӧ�նˣ�MCU��Ϊ0��ʾ�� //???

	THduChnStatus atHduChnStatus[MAXNUM_HDU_CHANNEL];
private:

	u8            m_abyChnMaxVmpMode[MAXNUM_HDU_CHANNEL];        //����ͨ��֧�ֵ������ͨ����
	u8            m_abyChnCurVmpMode[MAXNUM_HDU_CHANNEL];        //��ǰ����ͨ�����ں��ַ��
	THduMember    m_atHduVmpChnMt[MAXNUM_HDU_CHANNEL][HDU_MODEFOUR_MAX_SUBCHNNUM - 1];     //�¼�ͨ���ն���Ϣ
	THduChnStatus m_atHduVmpChnStatus[MAXNUM_HDU_CHANNEL][HDU_MODEFOUR_MAX_SUBCHNNUM - 1]; //�¼�ͨ��״̬��Ϣ

public:

	//����HDUͨ����
	void SetChnnlNum(u8 byChnnNum)
	{
		byChnnlNum = byChnnNum;
	}

	u8 GetChnnlNum()
	{
		return byChnnlNum;
	}

	// [2013/03/11 chenbing] ���������ͨ���� 
	void SetChnMaxVmpMode(u8 byChnIdx, u8 byStyle)
	{
		m_abyChnMaxVmpMode[byChnIdx] = byStyle;
	}

	// [2013/03/11 chenbing] ��ȡ�����ͨ���� 
	u8 GetChnMaxVmpMode(u8 byChnIdx)
	{
		return m_abyChnMaxVmpMode[byChnIdx];
	}

	// [2013/03/11 chenbing] ���õ�ǰͨ����� 
	void SetChnCurVmpMode(u8 byChnIdx, u8 byStyle)
	{
		if (byStyle != HDUCHN_MODE_ONE && byStyle != HDUCHN_MODE_FOUR)
		{
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[SetChnCurVmpMode]byChnIdx(%d)  byStyle(%d) invalid param\n",byChnIdx,byStyle);
			return;
		}
		m_abyChnCurVmpMode[byChnIdx] = byStyle;
	}

	// [2013/03/11 chenbing] ��ȡ��ǰͨ�����
	u8 GetChnCurVmpMode(u8 byChnIdx, BOOL32 bIsOtherChnVmpMode = FALSE)
	{
		if (bIsOtherChnVmpMode)
		{
			for (u8 byIndex=0; byIndex<MAXNUM_HDU_CHANNEL; byIndex++)
			{
				if (   byIndex != byChnIdx
					&& HDUCHN_MODE_FOUR == m_abyChnCurVmpMode[byIndex]
				   )
				{
					return m_abyChnCurVmpMode[byIndex];
				}
				
			}
			return HDUCHN_MODE_ONE;
		}
		else
		{
			return m_abyChnCurVmpMode[byChnIdx];
		}
	}

	// [2013/03/11 chenbing] ���õ�ǰͨ��
	void SetChnIdx(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			atHduChnStatus[byChnIdx].SetChnIdx(byChnIdx);
		} 
		else
		{
			m_atHduVmpChnStatus[byChnIdx][bySubChnIdx - 1].SetChnIdx(bySubChnIdx);
		}
	}

	// [2013/03/11 chenbing] ��ȡ��ǰͨ��
	u8 GetChnIdx(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return atHduChnStatus[byChnIdx].GetChnIdx();
		} 
		else
		{
			return m_atHduVmpChnStatus[byChnIdx][bySubChnIdx - 1].GetChnIdx();
		}
	}

	// [2013/03/11 chenbing] ���õ�ǰͨ����Ա����
	void SetMemberType(u8 byMemberType, u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			atVideoMt[byChnIdx].byMemberType = byMemberType;
		} 
		else
		{
			m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].byMemberType = byMemberType;
		}
	}

	// [2013/03/11 chenbing] ��ȡ��ǰͨ����Ա����
	u8 GetMemberType(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return atVideoMt[byChnIdx].byMemberType;
		} 
		else
		{
			return m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].byMemberType;
		}
	}
	
	// [2013/03/11 chenbing] ���õ�ǰͨ��Mt
	void SetChnMt(TMt tMt, u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			memcpy(&atVideoMt[byChnIdx], &tMt, sizeof(TMt));
		} 
		else
		{
			memcpy(&m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1], &tMt, sizeof(TMt));
		}
	}

	// [2013/03/11 chenbing] ��ȡ��ǰͨ��Mt
	TMt GetChnMt(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		TMt tMt;
		if (bySubChnIdx == 0)
		{
			return atVideoMt[byChnIdx];
		} 
		else
		{
			return m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1];
		}
	}

	
	// [2013/03/11 chenbing] ��ȡ��ǰͨ����Ա
	THduMember GetHduMember(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return atVideoMt[byChnIdx];
		} 
		else
		{
			return m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1];
		}
	}

	// [2013/03/11 chenbing] ���õ�ǰͨ��״̬
	void SetChnStatus(u8 byChnIdx, u8 bySubChnIdx = 0, u8 byStatus = THduChnStatus::eIDLE )
	{
		if (bySubChnIdx == 0)
		{
			atHduChnStatus[byChnIdx].SetStatus(byStatus);
		} 
		else
		{
			m_atHduVmpChnStatus[byChnIdx][bySubChnIdx - 1].SetStatus(byStatus);
		}
	}

	// [2013/03/11 chenbing] ��ȡ��ǰͨ��״̬
	u8 GetChnStatus(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return atHduChnStatus[byChnIdx].GetStatus();
		} 
		else
		{
			return m_atHduVmpChnStatus[byChnIdx][bySubChnIdx - 1].GetStatus();
		}
	}

	void SetMcuEqp(u8 byChnIdx, u8 bySubChnIdx, u8 byMcuId, u8 byEqpId, u8 byEqpType)
	{
		if (bySubChnIdx == 0)
		{
			atVideoMt[byChnIdx].SetMcuEqp(byMcuId, byEqpId, byEqpType);
		} 
		else
		{
			m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].SetMcuEqp(byMcuId, byEqpId, byEqpType);
		}
	}

	void SetMcuId(u16 wMcuId, u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			atVideoMt[byChnIdx].SetMcuId(wMcuId);
		} 
		else
		{
			m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].SetMcuId(wMcuId);
		}
	}

	u16 GetMcuId(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return atVideoMt[byChnIdx].GetMcuId();
		} 
		else
		{
			return m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].GetMcuId();
		}
	}

	u8 GetMtId(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return atVideoMt[byChnIdx].GetMtId();
		} 
		else
		{
			return m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].GetMtId();
		}
	}

	u8 GetEqpId(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return atVideoMt[byChnIdx].GetEqpId();
		} 
		else
		{
			return m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].GetEqpId();
		}
	}

	u8 GetEqpType(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return atVideoMt[byChnIdx].GetEqpType();
		} 
		else
		{
			return m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].GetEqpType();
		}
	}

	// [2013/03/11 chenbing] ���õ�ǰͨ�������
	void SetConfIdx(u8 byConfIdx, u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			atVideoMt[byChnIdx].SetConfIdx(byConfIdx);
		} 
		else
		{
			// �Ժ���ͨ�����ڲ�ͬ������ڴ����û����
			// Ŀǰ������ͨ��Ϊͬһ�����
			m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].SetConfIdx(byConfIdx);
		}
	}

	// [2013/03/11 chenbing] ��ȡ��ǰͨ�������
	u8 GetConfIdx(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return atVideoMt[byChnIdx].GetConfIdx();
		} 
		else
		{
			// Ŀǰ������ͨ��Ϊͬһ�����
			return atVideoMt[byChnIdx].GetConfIdx();
		}
	}

	void SetType(u8 byChnIdx, u8 bySubChnIdx = 0, u8 byType = 0)
	{
		if (bySubChnIdx == 0)
		{
			atVideoMt[byChnIdx].SetType(byType);
		} 
		else
		{
			m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].SetType(byType);
		}
	}

	u8 GetType(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return atVideoMt[byChnIdx].GetType();
		} 
		else
		{
			return m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].GetType();
		}
	}

	void SetEqpType(u8 byChnIdx, u8 bySubChnIdx = 0, u8 byEqpType = 0)
	{
		if (bySubChnIdx == 0)
		{
			atVideoMt[byChnIdx].SetEqpType(byEqpType);
		} 
		else
		{
			m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].SetEqpType(byEqpType);
		}
	}

	u8 SetEqpType(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			atVideoMt[byChnIdx].GetEqpType();
		} 
		else
		{
			m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].GetEqpType();
		}
	}

	// [2013/03/11 chenbing] ��ǰͨ���Ƿ�û��Mt
  	BOOL32 IsChnNull(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return (atVideoMt[byChnIdx].IsNull());
		} 
		else
		{
			return (m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].IsNull());
		}
	}

	// [2013/03/11 chenbing] ��յ�ǰͨ���е�Mt
	void SetChnNull(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			atVideoMt[byChnIdx].SetNull();
		} 
		else
		{
			m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].SetNull();
		}
	}

	BOOL32 IsNull(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return atHduChnStatus[byChnIdx].IsNull();
		} 
		else
		{
			return m_atHduVmpChnStatus[byChnIdx][bySubChnIdx - 1].IsNull();
		}
	}

	void SetStatusNull(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			atHduChnStatus[byChnIdx].SetNull();
		} 
		else
		{
			m_atHduVmpChnStatus[byChnIdx][bySubChnIdx - 1].SetNull();
		}
	}

	void SetSchemeIdx(u8 bySchemeIdx, u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			atHduChnStatus[byChnIdx].SetSchemeIdx(bySchemeIdx);
		} 
		else
		{
			m_atHduVmpChnStatus[byChnIdx][bySubChnIdx - 1].SetSchemeIdx(bySchemeIdx);
		}
	}

	u8 GetSchemeIdx(u8 byChnIdx, u8 bySubChnIdx)
	{
		if (bySubChnIdx == 0)
		{
			return atHduChnStatus[byChnIdx].GetSchemeIdx();
		} 
		else
		{
			return m_atHduVmpChnStatus[byChnIdx][bySubChnIdx - 1].GetSchemeIdx();
		}
	}
}
#ifndef WIN32
__attribute__((packed))
#endif
;

// [9/28/2011 liuxu] ��������, ���ڱ���ͻ�ȡ����ͨ����ǰ�Ķ�ý��ģʽ
// ��������: ����ͨ��������MAXNUM_PERIEQP_CHNNL��, �����Ľ������
class CPeriEqpChnnlMModeMgr
{
public:
	CPeriEqpChnnlMModeMgr() { memset(this, 0, sizeof(CPeriEqpChnnlMModeMgr)); }

	void Clear(){ memset(this, 0, sizeof(CPeriEqpChnnlMModeMgr)); } 

	// ��ȡָ���豸�ź�ͨ���ŵĵ�ǰ��ý��ģʽ
	u8 GetChnnlMMode( const u8 byEqpId, const u8 byChnnlIdx ) const 
	{
		if ( byEqpId == 0 || MAXNUM_MCU_PERIEQP < byEqpId || MAXNUM_PERIEQP_CHNNL <= byChnnlIdx )
		{
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "Err, GetChnnlMMode(%u,%u) received invalid param\n", byEqpId, byChnnlIdx);
			return MODE_NONE;
		}

		return m_abyHduChnnlMMode[byEqpId - 1][byChnnlIdx];
	}

	// ����ָ���豸�ź�ͨ���ŵĵ�ǰ��ý��ģʽ
	void SetChnnlMMode( const u8 byEqpId, const u8 byChnnlIdx, const u8 byMMode )
	{
		if ( byEqpId == 0 || MAXNUM_MCU_PERIEQP < byEqpId || MAXNUM_PERIEQP_CHNNL <= byChnnlIdx )
		{
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "Err, SetChnnlMMode(%u,%u) received invalid param\n", byEqpId, byChnnlIdx);
			return;
		}

		m_abyHduChnnlMMode[byEqpId - 1][byChnnlIdx] = byMMode;
	}	
	
	u16 GetChnModeData(u8 *pbyBuf)
	{
		if (NULL == pbyBuf)
		{
			return 0;
		}
		memcpy(pbyBuf, m_abyHduChnnlMMode,sizeof(m_abyHduChnnlMMode));
		return sizeof(m_abyHduChnnlMMode);
	}
				
	u16 SetChnModeData(u8 *pbyBuf)
	{
		if (NULL == pbyBuf)
		{
			return 0;
		}
		memcpy(m_abyHduChnnlMMode, pbyBuf, sizeof(m_abyHduChnnlMMode));	
		return sizeof(m_abyHduChnnlMMode);
	}

private:
	u8		m_abyHduChnnlMMode[MAXNUM_MCU_PERIEQP][MAXNUM_PERIEQP_CHNNL];
};



//˫�غ�
struct TDoublePayload
{
protected:
	u8  m_byRealPayload;    //ԭý���ʽ
	u8  m_byActivePayload;  //�ý���ʽ
public:
    TDoublePayload()
	{
		Reset();
	}
	void Reset()
	{
		m_byRealPayload = MEDIA_TYPE_NULL;
		m_byActivePayload = MEDIA_TYPE_NULL;
	}
	void SetRealPayLoad(u8 byRealPayload)
	{
		m_byRealPayload = byRealPayload;
	}
	u8 GetRealPayLoad()
	{
		return m_byRealPayload;
	}

	void SetActivePayload(u8 byActivePayload)
	{
		m_byActivePayload = byActivePayload;
	}

	u8 GetActivePayload()
	{
		return m_byActivePayload;
	}

	void Print() const
	{
		StaticLog( "m_byRealPayload is %d, m_byActivePayload is %d\n",
			m_byRealPayload, m_byActivePayload);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TTvWallStartPlay
{
protected:
    TMt m_tMt;
    TTransportAddr m_tVidRtcpBackAddr; //��ƵRTCP������ַ
	//TTransportAddr m_tAudRtcpBackAddr; //��ƵRtcp������ַ
    u8  m_byNeedPrs;
    TMediaEncrypt m_tVideoEncrypt;

public:
    TTvWallStartPlay(void)
    {
        Reset();
    }

    void Reset(void)
    {
		m_byNeedPrs = 0;
		m_tVidRtcpBackAddr.SetNull();
		//m_tAudRtcpBackAddr.SetNull();
        m_tMt.SetNull();
        m_tVideoEncrypt.Reset();
    }

    void SetMt(const TMt &tMt)
    {
        m_tMt  = tMt;
    }

    TMt& GetMt(void)
    {
        return m_tMt;
    }

    TMediaEncrypt& GetVideoEncrypt(void)
    {
        return m_tVideoEncrypt;
    }

    void SetVideoEncrypt(TMediaEncrypt& tMediaEncrypt)
    {
        memcpy(&m_tVideoEncrypt, &tMediaEncrypt, sizeof(tMediaEncrypt));
    }

    void SetIsNeedByPrs(BOOL bNeedPrs)
    {
        m_byNeedPrs = bNeedPrs==TRUE?1:0;
    }

    BOOL IsNeedByPrs(void)
    {
        return (m_byNeedPrs != 0);
    }

	//[liu lijiu][20100919]����RTCP������ַ
	//������Ƶ��RTCP������ַ
	void SetVidRtcpBackAddr(u32 dwRtcpBackIp, u16 wRtcpBackPort)
    {
        m_tVidRtcpBackAddr.SetIpAddr(dwRtcpBackIp);
        m_tVidRtcpBackAddr.SetPort(wRtcpBackPort);
		return;
    }

	//��ȡ��Ƶ��RTCP������ַ
    void GetVidRtcpBackAddr(u32 & dwRtcpBackIp, u16 & wRtcpBackPort)
    {
        dwRtcpBackIp = m_tVidRtcpBackAddr.GetIpAddr();
        wRtcpBackPort = m_tVidRtcpBackAddr.GetPort();
        return;
    }

	//������Ƶ��RTCP������ַ
	/*
	void SetAudRtcpBackAddr(u32 dwRtcpBackIp, u16 wRtcpBackPort)
    {
        m_tAudRtcpBackAddr.SetIpAddr(dwRtcpBackIp);
        m_tAudRtcpBackAddr.SetPort(wRtcpBackPort);
		return;
    }

	//��ȡ��Ƶ��RTCP������ַ
	void GetAudRtcpBackAddr(u32 & dwRtcpBackIp, u16 & wRtcpBackPort)
    {
        dwRtcpBackIp = m_tAudRtcpBackAddr.GetIpAddr();
        wRtcpBackPort = m_tAudRtcpBackAddr.GetPort();
        return;
    }*/
}
#ifndef WIN32
__attribute__ ((packed))
#endif
;

//4.6�¼� �汾 jlb
struct THduStartPlay: public TTvWallStartPlay
{
protected:
	u8 m_byMode;   // MODE_AUDIO, MODE_VIDEO, MODE_BOTH
	u8 m_byReserved;
public:
	THduStartPlay()
	{
		m_byMode = 0;
		m_byReserved = 0;
	}

	void SetMode( u8 byMode ){ m_byMode = byMode; }
    u8   GetMode( void ){ return m_byMode; }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// [1/5/2010 xliang] T8KEMixerStart 
struct T8KEMixerStart
{
protected:
    u8 m_byMixGroupId; //������Id
    u8 m_byMixDepth;   //�������
    u8 m_byAudioMode;  //������ʽ
    TMediaEncrypt m_tAudioEncrypt;	//���ܲ���
    u8 m_byNeedPrs;
public:
    T8KEMixerStart(void)
    {
        Reset();
    }
    void Reset(void)
    {
        m_byMixGroupId = MIXER_INVALID_GRPID;
        m_byMixDepth = 0;
        m_byAudioMode = MEDIA_TYPE_NULL;
        m_byNeedPrs = 0;
        m_tAudioEncrypt.Reset();
    }
	
    void SetMixGroupId(u8 byGroupID)
    {
        m_byMixGroupId = byGroupID;
    }
	
    u8 GetMixGroupId() const
    {
        return m_byMixGroupId;
    }
    void SetMixDepth(u8 byDepth)
    {
        m_byMixDepth = byDepth;
    }
    u8 GetMixDepth() const
    {
        return m_byMixDepth;
    }
	
    void SetAudioMode(u8 byAudioMode)
    {
        m_byAudioMode = byAudioMode;
    }
    u8  GetAudioMode() const
    {
        return m_byAudioMode;
    }
	
    TMediaEncrypt& GetAudioEncrypt()
    {
        return m_tAudioEncrypt;
    }
	
    void SetAudioEncrypt(TMediaEncrypt& tMediaEncrypt)
    {
        memcpy(&m_tAudioEncrypt, &tMediaEncrypt, sizeof(tMediaEncrypt));
    }
	
    void SetIsNeedByPrs(BOOL32 bNeedPrs)
    {
        m_byNeedPrs = (TRUE == bNeedPrs) ? 1 : 0;
    }
	
    BOOL32 IsNeedByPrs(void) const
    {
        return (m_byNeedPrs != 0);
    }

	void Print(void) const
	{
		StaticLog( "m_byMixGroupId is %d, m_byMixDepth is %d, m_byAudioMode is %d\n",
			m_byMixGroupId, m_byMixDepth, m_byAudioMode);
	}
}
#ifndef WIN32
__attribute__((packed))
#endif
;

struct TMixerStart
{
protected:
    u8 m_byMixGroupId; //������Id
    u8 m_byMixDepth;   //�������
    u8 m_byAudioMode;  //������ʽ
    u8 m_byAudioMode2; //�ڶ�������ʽ(����Ϊ����Ϊ˫������ʽ)
    u8 m_byIsAllMix;   //ȫ�����
    TMediaEncrypt m_tAudioEncrypt;	//���ܲ���
    u8 m_byNeedPrs;
public:
    TMixerStart(void)
    {
        Reset();
    }
    void Reset(void)
    {
        m_byMixGroupId = MIXER_INVALID_GRPID;
        m_byMixDepth = 0;
        m_byAudioMode = MEDIA_TYPE_NULL;
        m_byAudioMode2 = MEDIA_TYPE_NULL;
        m_byIsAllMix = 0;
        m_byNeedPrs = 0;
        m_tAudioEncrypt.Reset();
    }

    void SetMixGroupId(u8 byGroupID)
    {
        m_byMixGroupId = byGroupID;
    }
    u8 GetMixGroupId()
    {
        return m_byMixGroupId;
    }
    void SetMixDepth(u8 byDepth)
    {
        m_byMixDepth = byDepth;
    }
    u8 GetMixDepth()
    {
        return m_byMixDepth;
    }
    void SetAudioMode(u8 byAudioMode)
    {
        m_byAudioMode = byAudioMode;
    }
    u8  GetAudioMode()
    {
        return m_byAudioMode;
    }
    void SetSecAudioMode(u8 byAudioMode)
    {
        m_byAudioMode2 = byAudioMode;
    }
    u8  GetSecAudioMode()
    {
        return m_byAudioMode2;
    }

    void SetIsAllMix(BOOL32 bIsAllMix)
    {
        m_byIsAllMix = (bIsAllMix ? 1:0 );
    }
    BOOL32 IsAllMix()
    {
        return (m_byIsAllMix != 0);
    }
    TMediaEncrypt& GetAudioEncrypt()
    {
        return m_tAudioEncrypt;
    }

    void SetAudioEncrypt(TMediaEncrypt& tMediaEncrypt)
    {
        memcpy(&m_tAudioEncrypt, &tMediaEncrypt, sizeof(tMediaEncrypt));
    }

    void SetIsNeedByPrs(BOOL32 bNeedPrs)
    {
        m_byNeedPrs = (TRUE == bNeedPrs) ? 1 : 0;
    }

    BOOL32 IsNeedByPrs(void)
    {
        return (m_byNeedPrs != 0);
    }
}
#ifndef WIN32
__attribute__((packed))
#endif
;

//������Ա�ṹ
struct TMixMember
{
    TMt m_tMember;
    u8  m_byVolume;                 //��Ա����
    u8  m_byAudioType;              //ý������
    TTransportAddr m_tAddr;         //��Ա�Ĵ����ַ
    TTransportAddr m_tRtcpBackAddr; //RTCP������ַ
}
#ifndef WIN32
__attribute__((packed)) 
#endif
;

//������״̬
struct TMixerGrpStatus
{
	enum EState 
	{ 
		IDLE   = 0,		//�û�����δʹ�� 
		READY  = 1,		//���鴴��,��δ��ʼ����
		MIXING = 2,		//���ڻ���
        WAIT_BEGIN = 200,
        WAIT_START_SPECMIX = 201,   //�ȴ���Ӧ״̬
        WAIT_START_AUTOMIX = 202,
        WAIT_START_VAC = 203,
        WAIT_STOP = 204,
		WAIT_START_AUTOMIXANDVAC = 205,
	};

	u8       m_byGrpId;		  //������ID(0-4)
	u8       m_byGrpState;	  //��״̬
	u8       m_byGrpMixDepth; //�������
    u8       m_byConfId;      //����ID
protected:
    u8       m_abyMixMmb[MAXNUM_CONF_MT>>3];//���뾺��������Ա�б�
    u8       m_abyActiveMmb[MAXNUM_MIXER_DEPTH];//��ѡ�еĻ�����Ա�б�,0��ʾ��Ч��Ա
public:
	u8       m_byMixGrpChnNum ;//tianzhiyong 100201 EAPU�¸����������ͨ����Ŀ
	//����״̬��λ
	void Reset()
	{
		m_byGrpId       = 0xff;
		m_byGrpState    = IDLE;
		m_byGrpMixDepth = 0;
		memset( m_abyMixMmb ,0 ,sizeof(m_abyMixMmb) );
		memset( m_abyActiveMmb ,0 ,sizeof(m_abyActiveMmb) );
	}
    //��Ӳ��뾺���Ļ�����Ա
	// ���ó�ԱID�Ϸ�����TRUE,���򷵻�FALSE
    BOOL  AddMmb( u8   byMmbId)
	{
		u8   byByteIdx ,byBitIdx,byMask;
		if( byMmbId > MAXNUM_CONF_MT ||byMmbId==0)
			return FALSE;
		byMmbId--;
		byByteIdx = byMmbId>>3;
		byBitIdx  = byMmbId%8;
		byMask    = 1<<byBitIdx;

		m_abyMixMmb[byByteIdx] |= byMask;

		return TRUE;
	}

	//ɾ�����뾺���Ļ�����Ա
	// ���ó�ԱID�Ϸ�����TRUE,���򷵻�FALSE
	BOOL  RemoveMmb( u8   byMmbId)
	{

		u8   byByteIdx ,byBitIdx,byMask;
		if( byMmbId > MAXNUM_CONF_MT ||byMmbId==0)
			return FALSE;
		byMmbId--;
		byByteIdx = byMmbId>>3;
		byBitIdx  = byMmbId%8;
		byMask    = 1<<byBitIdx;

		m_abyMixMmb[byByteIdx] &= ~byMask;

		return TRUE;
	}

	//���ĳ��Ա�Ƿ�����ڲ��뾺���ı��У�
	//������ڷ���TRUE ,���򷵻�FALSE
	BOOL  IsContain( u8   byMmbId )
	{
		
		u8   byByteIdx ,byBitIdx,byMask;
		if( byMmbId > MAXNUM_CONF_MT ||byMmbId==0)
			return FALSE;
		byMmbId--;
		byByteIdx = byMmbId>>3;
		byBitIdx  = byMmbId%8;
		byMask    = 1<<byBitIdx;
		
		return (m_abyMixMmb[byByteIdx]&byMask)==0 ? FALSE:TRUE;
	}

	//������ڲ��뾺����������ĳ�Ա����
	u8    GetMmbNum()
	{
		u8   byNum=0;
		for( u8   byByteIdx=0 ;byByteIdx<(MAXNUM_CONF_MT>>3) ;byByteIdx++ )
		{
			for( u8   byBitIdx =0 ;byBitIdx<8 ;byBitIdx++ )
			{
				byNum += (m_abyMixMmb[byByteIdx]>>byBitIdx)&0x1;
			}
		}
		return byNum;
	}

	//������ڲ��뾺���Ļ�����ԱID
	//��� - abyMmb[] ���ڴ洢�μӻ����ĳ�ԱID
	//���� - bySize abyMmb[] �Ĵ�С
	//���� �μӻ����ĳ�Ա��
	u8    GetMixMmb( u8   abyMmb[] ,u8   bySize)
	{
		u8   byNum=0;
		for( u8   byByteIdx=0 ;byByteIdx<(MAXNUM_CONF_MT>>3) ;byByteIdx++ )
		{
			for( u8   byBitIdx =0 ;byBitIdx<8 ;byBitIdx++ )
			{
				if((m_abyMixMmb[byByteIdx]>>byBitIdx)&0x1 )
				{
					if( bySize <= byNum )
						return bySize;
					abyMmb[byNum]= byByteIdx*8+byBitIdx+1;
					byNum++;
				}
			}
		}
		return byNum;
	}
	
	//�ж�ָ����Ա�Ƿ�ѡ�л���
	BOOL IsActive( u8   byMmbId )
	{
		if( byMmbId ==0 )return FALSE;
		for( u8   byIdx =0 ;byIdx< MAXNUM_MIXER_DEPTH ;byIdx++ )
		{
			if( m_abyActiveMmb[byIdx]== byMmbId )
				return TRUE;
		}
		return FALSE;
	}
	
	//��ñ�ѡ�л�����Ա����
	u8   GetActiveMmbNum()
	{
		u8   byCount=0;
		for( u8   byIdx =0 ;byIdx< MAXNUM_MIXER_DEPTH ;byIdx++ )
			if( m_abyActiveMmb[byIdx] )byCount++;
		return byCount;
	}
	
	//��ñ�ѡ�л�����Ա
	//���: abyMmb[] - �洢��ѡ�л�����ԱID
	//����: bySize   - abyMmb ��С
	//����: abyMmb����Ч��Ա����
	u8   GetActiveMmb( u8   abyMmb[] ,u8   bySize )
	{
		u8   byCnt=0;
    	for( u8   byIdx =0 ;byIdx< min(bySize ,MAXNUM_MIXER_DEPTH) ;byIdx++ )
			if( m_abyActiveMmb[byIdx] )
			{
				abyMmb[byIdx] = m_abyActiveMmb[byIdx];
				byCnt++;
			}
			return byCnt;		
	}

	BOOL UpdateActiveMmb( u8   abyMmb[MAXNUM_MIXER_DEPTH] )
	{
		memcpy( m_abyActiveMmb ,abyMmb ,MAXNUM_MIXER_DEPTH);
		return TRUE;
	}
	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//������״̬
struct TMixerStatus
{
	u8 m_byGrpNum; //����������
	TMixerGrpStatus	m_atGrpStatus[MAXNUM_MIXER_GROUP];
	u8  m_bIsMultiFormat; //tianzhiyong 100201  ��Ǹû������Ƿ�֧�ֶ��ʽ
	u8  m_byMixOffChnNum ;//tianzhiyong 100201 EAPU�¸����������ƫ��ͨ����Ŀ
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//����ϳ���״̬
struct TVmpStatus
{
	enum EnumUseState 
	{ 
		IDLE,		//δ��ռ��
	    RESERVE,    //Ԥ��
        WAIT_START, //�ȴ���ʼ��Ӧ
		START,      //�����л���ϳ�
		WAIT_STOP,  //�ȴ�������Ӧ
	};
public:	
	u8        m_byUseState;	    //ʹ��״̬ 0-δʹ�� 1-ʹ��
	u8        m_byChlNum;       //ͨ����
#ifdef _ZGCDEBUG_
	u8		  m_byEncodeNum;	//����·��, zgc, 2007-06-20
#endif
    TVMPParam m_tVMPParam;      //�ϳɲ���(��ʱ������֮���Ϊprotect)
	u8		  m_bySubType;		//vmp�����ͣ����mcuconst.h�еĶ���// xliang [12/29/2008] 
	u8		  m_byBoardVer;		//����MPU A��/B��
	TVMPMember  m_atVMPMember[MAXNUM_MPUSVMP_MEMBER]; //����20����Ա������20��Աʱʹ��
public:
	TVMPParam_25Mem GetVmpParam()
	{
		TVMPParam_25Mem tVmpParam;
		memcpy(&tVmpParam, &m_tVMPParam, sizeof(m_tVMPParam));//����20��Ա�ĺϳ���Ϣ
		u8 byMaxNum = MAXNUM_MPU2VMP_MEMBER - MAXNUM_MPUSVMP_MEMBER;
		for (u8 byIdx=0; byIdx<byMaxNum; byIdx++)
		{
			tVmpParam.SetVmpMember(MAXNUM_MPUSVMP_MEMBER+byIdx, m_atVMPMember[byIdx]);
		}
		return tVmpParam;
	}
	void SetVmpParam(TVMPParam tVmpParam)
	{
		memcpy(&m_tVMPParam, &tVmpParam, sizeof(m_tVMPParam));
	}
	void SetVmpParam(TVMPParam_25Mem tVmpParam)
	{
		memcpy(&m_tVMPParam, &tVmpParam, sizeof(m_tVMPParam));
		u8 byMaxNum = MAXNUM_MPU2VMP_MEMBER - MAXNUM_MPUSVMP_MEMBER;
		for (u8 byIdx=0; byIdx<byMaxNum; byIdx++)
		{
			if (NULL != tVmpParam.GetVmpMember(MAXNUM_MPUSVMP_MEMBER+byIdx))
			{
				m_atVMPMember[byIdx] = *tVmpParam.GetVmpMember(MAXNUM_MPUSVMP_MEMBER+byIdx);
			}
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// ����ϳɳ�Ա�ṹ(Mcu -- Vmp)
struct TVMPMemberEx : public TVMPMember
{
public:
	void  SetMember( TVMPMember& tMember )
	{
		memcpy(this, &tMember, sizeof(tMember));
	}

	BOOL  SetMbAlias( const s8 *pAlias ) 
    { 
        BOOL bRet = FALSE;
        if( NULL != pAlias )
        {
            memcpy(achMbAlias, pAlias, sizeof(achMbAlias));
            achMbAlias[VALIDLEN_ALIAS] = '\0';
            bRet = TRUE;
        }
#ifdef _UTF8
		// UTF8�����ֽ�������Ϊ2 ~6�ֽڣ��ʲ���������ж� [pengguofeng 5/23/2013]
#else
		s8 nLen = strlen(achMbAlias);
        s8 nLoop = nLen - 1;
        u8 byWideCharCount = 0;
        while ( (signed char)nLoop >= 0  && (signed char)0 > (signed char)achMbAlias[nLoop])
        {
            byWideCharCount ++;
            nLoop --;
        }
        if ( byWideCharCount % 2 == 1 )
        {
            achMbAlias[nLen-1] = '\0';
        }
#endif
        return bRet; 
    }
	const s8* GetMbAlias(void) { return achMbAlias; }

protected:
	s8   achMbAlias[VALIDLEN_ALIAS+1];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//����ϳ�������Ϣ
struct TVmpStyleCfgInfo
{    
protected:
    u8  m_bySchemeId;                   //����id��1��5
	u8	m_byRimEnabled;					//�Ƿ�ʹ�ñ߿�: 0-��ʹ��(Ĭ��) 1-ʹ��;
										//���ֶ�Ŀǰֻ�Է���0��Ч, ���������ݲ��漰���ֶε����ú��ж�
	u8	m_byReserved1;					//�����ֶ�1
	u8	m_byReserved2;					//�����ֶ�2
    u32 m_dwBackgroundColor;            //ͼ�񱳾�ɫ
    u32 m_dwFrameColor;                 //ͼ��߿�ɫ
    u32 m_dwSpeakerFrameColor;          //�����˱߿�ɫ
    u32 m_dwChairFrameColor;            //��ϯ�߿�ɫ 
	u8  m_byFontType;                   // �ϳ���������
	u8  m_byFontSize;					// �ϳ������ֺ�
	u8  m_byAlignment;                  // ���뷽ʽ
	u32  m_dwTextColor;                  // �ϳ�����������ɫ
	u32  m_dwTopicBkColor;               // �ϳ����ⱳ��ɫ
	u32  m_dwDiaphaneity;                // �ϳ�����͸����
    s8  m_achSchemeAlias[MAX_VMPSTYLE_ALIASLEN];  // ����ϳɷ�������

public:
    TVmpStyleCfgInfo(void) { Clear();   }

	void	Clear(void)  { memset(this, 0, sizeof(TVmpStyleCfgInfo)); }
	void	SetNull(void) { Clear(); }
	BOOL32  IsNull(void) 
    { 
        TVmpStyleCfgInfo tInfo;
        return (0 == memcmp(this, &tInfo, sizeof(TVmpStyleCfgInfo)));
    }
    void    SetSchemeId(u8 bySchemeId) { m_bySchemeId = bySchemeId; }
    u8      GetSchemeId(void) { return m_bySchemeId; }    
    void    SetBackgroundColor(u32 dwColor) { m_dwBackgroundColor = htonl(dwColor); }
    u32     GetBackgroundColor(void) { return ntohl(m_dwBackgroundColor); }
    void    SetFrameColor(u32 dwColor) { m_dwFrameColor = htonl(dwColor); }
    u32     GetFrameColor(void) { return ntohl(m_dwFrameColor); }
    void    SetSpeakerFrameColor(u32 dwColor) { m_dwSpeakerFrameColor = htonl(dwColor); }
    u32     GetSpeakerFrameColor(void) { return ntohl(m_dwSpeakerFrameColor); }
    void    SetChairFrameColor(u32 dwColor) { m_dwChairFrameColor = htonl(dwColor); }
    u32     GetChairFrameColor(void) { return ntohl(m_dwChairFrameColor); }
	void	SetIsRimEnabled(BOOL32 bEnabled){ m_byRimEnabled = GETBBYTE(bEnabled); }
	BOOL32	GetIsRimEnabled(void) const { return m_byRimEnabled == 1 ? TRUE : FALSE; }
	void    SetFontType(u8 byFontType) { m_byFontType = byFontType;}
	u8      GetFontType(void) { return m_byFontType;}
	void    SetFontSize(u8 byFontSize ) { m_byFontSize = byFontSize;}
	u8      GetFontSize(void) { return m_byFontSize;}
	void    SetAlignment(u8 byValue ) { m_byAlignment = byValue;}
	u8      GetAlignment(void) { return m_byAlignment;}
	void    SetTextColor(u32 dwTextColor ) { m_dwTextColor = htonl(dwTextColor);}
	u32     GetTextColor(void) { return ntohl(m_dwTextColor);}
	void    SetTopicBkColor(u32 dwBkColor) { m_dwTopicBkColor = htonl(dwBkColor);}
	u32     GetTopicBkColor(void) { return ntohl(m_dwTopicBkColor);}
	void    SetDiaphaneity(u32 dwDiaphaneity ) { m_dwDiaphaneity = htonl(dwDiaphaneity); }
	u32     GetDiaphaneity(void) { return ntohl(m_dwDiaphaneity);}

    void    SetSchemeAlias(s8* pchSchemeAlias, u16 wLen = MAX_VMPSTYLE_ALIASLEN)
    {
		u32 dwCharNum = 0, dwChnNum = 0, dwIdx = 0;
		memset(m_achSchemeAlias, 0, sizeof(m_achSchemeAlias));
        memcpy(m_achSchemeAlias, pchSchemeAlias, min(sizeof(m_achSchemeAlias), wLen));
		for (dwIdx = 0; dwIdx < (MAX_VMPSTYLE_ALIASLEN - 1); dwIdx++)
		{
			if (m_achSchemeAlias[dwIdx] < 0)
			{
				dwChnNum++;
			}
			else
			{
				dwCharNum++;
			}
		}
		if (dwChnNum % 2 == 0)
		{
			m_achSchemeAlias[MAX_VMPSTYLE_ALIASLEN - 1] = '\0';
		}
		else
		{
			m_achSchemeAlias[MAX_VMPSTYLE_ALIASLEN - 2] = '\0';
		}
    }
    
    void    GetSchemeAlias(s8* pchSchemeAlias, u8 byBufferLen)
    {
        u8 byAliasLen = (byBufferLen < sizeof(m_achSchemeAlias) ) ? byBufferLen : sizeof(m_achSchemeAlias);
        memcpy(pchSchemeAlias, m_achSchemeAlias, byAliasLen);
    }
    
    void    ResetDefaultColor(void)
    {
        SetBackgroundColor(VMPSTYLE_DEFCOLOR);
        SetSpeakerFrameColor(VMPSTYLE_DEFCOLOR);
        SetChairFrameColor(VMPSTYLE_DEFCOLOR);
        SetFrameColor(VMPSTYLE_DEFCOLOR);
		SetFontType(FONT_SONG);
		SetFontSize(FONT_SIZE_24);
		SetAlignment(MIDDLE_ALIGN);
		SetTextColor(DEFAULT_TEXT_COLOR);
		SetTopicBkColor(DEFAULT_BACKGROUND_COLOR);
		SetDiaphaneity(DEFAULT_DIAPHANEITY);
    }

	inline BOOL operator == (const TVmpStyleCfgInfo& tVmpStyle )
	{
		BOOL bRet = TRUE;
		if( this == &tVmpStyle )
		{
			return bRet;
		}
		
		if ( 0 != memcmp(this, &tVmpStyle, sizeof(TVmpStyleCfgInfo)) )
		{
			bRet = FALSE;
		}
		return bRet;
	}

    void    Print(void)
    {	
		StaticLog( "VmpStyleInfo(SchemeId:%d)\n BackColor:0x%x, AudienceColor:0x%x, SpeakerColor:0x%x, ChairColor:0x%x, IsRimEnabled:%d\n",
			GetSchemeId(), GetBackgroundColor(), GetFrameColor(), GetSpeakerFrameColor(), GetChairFrameColor(), GetIsRimEnabled() );
		StaticLog( "VmpStyleInfo: \n FontType:%d, FontSize:%d, TextColor:0x%x, BkColor:0x%x, Diaphaneity:0x%x\n",
			GetFontType(), GetFontSize(), GetTextColor(), GetTopicBkColor(), GetDiaphaneity() );
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// VMP��չ����
struct TVMPExCfgInfo
{
	u8 m_byIdleChlShowMode;  // ����ͨ����ʾģʽ VMP_SHOW_GRAY_MODE
	
	u8  m_byIsDisplayMmbAlias; // �Ƿ���ʾ������1 Ϊ��ʾ��0Ϊ����ʾ��
	u16 m_wReserved;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//����ϳɲ���(MCU��8000EVMPͨѶ��)
class C8KEVMPParam
{
public:
	u8              m_byVMPStyle;      //����ϳɷ��,�μ�mcuconst.h�л���ϳɷ����
	u8    	        m_byEncType;       //ͼ���������
	u16             m_wBitRate;        //���������(Kbps)
	u16   	        m_wVideoWidth;     //ͼ����(default:720)
	u16   		    m_wVideoHeight;    //ͼ��߶�(default:576)
	u8      	    m_byMemberNum;     //�μ���Ƶ���ϵĳ�Ա����
	u8              m_byIsDisplayMmbAlias;    //�Ƿ���ʾ�ϳɳ�Ա�ı���, zgc, 2009-07-24// [12/21/2009 xliang] 
	u8				m_byFrameRate;	   //֡��
	
	TVMPMemberEx    m_atMtMember[MAXNUM_SDVMP_MEMBER];    //��Ƶ���ϳ�Ա�����ո��Ϸ�ʽ���߼����˳��
	TMediaEncrypt   m_tVideoEncrypt[MAXNUM_SDVMP_MEMBER];     //��Ƶ���ܲ���
	TDoublePayload  m_tDoublePayload[MAXNUM_SDVMP_MEMBER];
	
public:
	void SetBitRate( u16 wBitRate ) { m_wBitRate = htons( wBitRate); }
	u16  GetBitRate() { return ntohs(m_wBitRate); }
	void SetVideoWidth( u16 wVidWidth ) { m_wVideoWidth = htons(wVidWidth); }
	u16  GetVideoWidth() { return ntohs(m_wVideoWidth); }
	void SetVideoHeight( u16 wVidHeight ) { m_wVideoHeight = htons(wVidHeight); }
	u16	 GetVideoHeight() { return ntohs(m_wVideoHeight); }
	
    void Print(void)
    {
        StaticLog( "VmpStyle:%d, EncType:%d, Bitrate:0x%x, VidWidth:%d, VidHeight:%d, MemNum:%d\n",
			m_byVMPStyle, m_byEncType, ntohs(m_wBitRate), m_wVideoWidth, m_wVideoHeight, m_byMemberNum);
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

//����ϳɲ���(MCU��VMPͨѶ��)
class CKDVVMPParam
{
public:
	u8              m_byVMPStyle;      //����ϳɷ��,�μ�mcuconst.h�л���ϳɷ����
	u8    	        m_byEncType;       //ͼ���������
	u16             m_wBitRate;        //���������(Kbps)
	u16   	        m_wVideoWidth;     //ͼ����(default:720)
	u16   		    m_wVideoHeight;    //ͼ��߶�(default:576)
  	u8      	    m_byMemberNum;     //�μ���Ƶ���ϵĳ�Ա����
	TVMPMemberEx    m_atMtMember[MAXNUM_MPUSVMP_MEMBER];    //��Ƶ���ϳ�Ա�����ո��Ϸ�ʽ���߼����˳��
	TMediaEncrypt   m_tVideoEncrypt[MAXNUM_MPUSVMP_MEMBER];     //��Ƶ���ܲ���
	TDoublePayload  m_tDoublePayload[MAXNUM_MPUSVMP_MEMBER];	//vpu�е���������20��Ա��

public:
    void Print(void)
    {
        StaticLog( "VmpStyle:%d, EncType:%d, Bitrate:0x%x, VidWidth:%d, VidHeight:%d, MemNum:%d\n",
                  m_byVMPStyle, m_byEncType, ntohs(m_wBitRate), m_wVideoWidth, m_wVideoHeight, m_byMemberNum);
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
//VMP���������(MCU��VMPͨѶ��)
class CKDVVMPOutMember
{
private:
	u8    	        m_byEncType;		//ͼ���������
	u8    	        m_byProfileType;	//BP/HP
	u8				m_byFrameRate;		//֡��
	u16             m_wBitRate;			//���������(Kbps)
	u16   	        m_wVideoWidth;		//ͼ����(default:720)
	u16   		    m_wVideoHeight;		//ͼ��߶�(default:576)
public:
	void Clear() { memset(this, 0, sizeof(CKDVVMPOutMember)); }
	void SetBitRate( u16 wBitRate ) { m_wBitRate = htons( wBitRate); }
	u16  GetBitRate() { return ntohs(m_wBitRate); }
	void SetVideoWidth( u16 wVidWidth ) { m_wVideoWidth = htons(wVidWidth); }
	u16  GetVideoWidth() { return ntohs(m_wVideoWidth); }
	void SetVideoHeight( u16 wVidHeight ) { m_wVideoHeight = htons(wVidHeight); }
	u16	 GetVideoHeight() { return ntohs(m_wVideoHeight); }
	void SetFrameRate( u8 byFrameRate ) { m_byFrameRate = byFrameRate ; }
	u8	 GetFrameRate() { return m_byFrameRate; }
	void SetEncType( u8 byEncType ) { m_byEncType = byEncType ; }
	u8	 GetEncType() { return m_byEncType; }
	void SetProfileType( u8 byProfileType ) { m_byProfileType = byProfileType ; }
	u8	 GetProfileType() { return m_byProfileType; }
	
    void Print(void)
    {
        StaticLog( "\tEncType:\t%d\n\tProfileType:\t%d\n\tFrameRate:\t%d\n\tBitrate:\t%d\n\tVidWidth:\t%d\n\tVidHeight:\t%d\n",
			m_byEncType, m_byProfileType, m_byFrameRate, GetBitRate(), GetVideoWidth(), GetVideoHeight());    
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//����ϳɲ���(MCU��VMPͨѶ��)
class CKDVNewVMPParam
{
public:
	u8              m_byVMPStyle;      //����ϳɷ��,�μ�mcuconst.h�л���ϳɷ����
	u8    	        m_byEncType;       //ͼ���������
	u8      	    m_byMemberNum;     //�μ���Ƶ���ϵĳ�Ա����
	TVMPMemberEx    m_atMtMember[MAXNUM_MPUSVMP_MEMBER];    //��Ƶ���ϳ�Ա�����ո��Ϸ�ʽ���߼����˳��
	TMediaEncrypt   m_tVideoEncrypt[MAXNUM_MPUSVMP_MEMBER];     //��Ƶ���ܲ���
	TDoublePayload  m_tDoublePayload[MAXNUM_MPUSVMP_MEMBER];

private:
	u16             m_wBitRate;        //���������(Kbps)
	u16   	        m_wVideoWidth;     //ͼ����(default:720)
	u16   		    m_wVideoHeight;    //ͼ��߶�(default:576)
	u8				m_byFrameRate;	   //֡��
public:
	void SetBitRate( u16 wBitRate ) { m_wBitRate = htons( wBitRate); }
	u16  GetBitRate() { return ntohs(m_wBitRate); }
	void SetVideoWidth( u16 wVidWidth ) { m_wVideoWidth = htons(wVidWidth); }
	u16  GetVideoWidth() { return ntohs(m_wVideoWidth); }
	void SetVideoHeight( u16 wVidHeight ) { m_wVideoHeight = htons(wVidHeight); }
	u16	 GetVideoHeight() { return ntohs(m_wVideoHeight); }
	void SetFrameRate( u8 byFrameRate ) { m_byFrameRate = byFrameRate ; }
	u8	 GetFrameRate() { return m_byFrameRate; }

    void Print(void)
    {
        StaticLog( "\tVmpStyle:\t%d\n\tEncType:\t%d\n\tBitrate:\t%d\n\tVidWidth:\t%d\n\tVidHeight:\t%d\n\tMemNum:\t\t%d\nFrameRate:\t%d\n",
			m_byVMPStyle, m_byEncType, GetBitRate(), GetVideoWidth(), GetVideoHeight(), m_byMemberNum, m_byFrameRate);

        StaticLog( "\nMem as follows:\n");

        for(u8 byIndex = 0; byIndex < MAXNUM_MPUSVMP_MEMBER; byIndex++)
        {
            if(0 != m_atMtMember[byIndex].GetMcuId() && 0 != m_atMtMember[byIndex].GetMtId())
            {
                StaticLog( "\tMemIdx[%d]:\n", byIndex);
                StaticLog( "\t\tMcuId:\t\t%d\n\t\tMtId:\t\t%d\n\t\tMemType:\t%d\n\t\tMemStatus:\t%d\n\t\tRealPayLoad:\t%d\n\t\tActPayLoad:\t%d\n\t\tEncrptMode:\t%d\n", 
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

struct TRecRtcpBack
{
protected:
    u32  m_tVideoIp;      //��ƵRTCP������ַ
    u16  m_tVideoPort;    //��ƵRTCP�����˿�
    u32  m_tAudioIp;      //��ƵRTCP������ַ
    u16  m_tAudioPort;    //��ƵRTCP�����˿�
    u32  m_tDStreamIp;    //˫��RTCP������ַ
    u16  m_tDStreamPort;  //˫��RTCP�����˿�

public:
    TRecRtcpBack(void)
	{
		Reset();
	}
	void Reset(void)
	{
		m_tVideoIp = 0xFFFFFFFF;
        m_tVideoPort = 0xFFFF;
        m_tAudioIp = 0xFFFFFFFF;
        m_tAudioPort = 0xFFFF;
        m_tDStreamIp = 0xFFFFFFFF;
        m_tDStreamPort = 0xFFFF;
	}

    void SetVideoAddr(u32 dwVideoIp, u16 wVideoPort)
    {
        m_tVideoIp = htonl(dwVideoIp);
        m_tVideoPort = htons(wVideoPort);
    }

    void GetVideoAddr(u32 &dwVideoIp, u16 &wVideoPort)
    {
        dwVideoIp = ntohl(m_tVideoIp);
        wVideoPort = ntohs(m_tVideoPort);
    }

    
    void SetAudioAddr(u32 dwAudioIp, u16 wAudioPort)
    {
        m_tAudioIp = htonl(dwAudioIp);
        m_tAudioPort = htons(wAudioPort);
    }

    void GetAudioAddr(u32 &dwAudioIp, u16 &wAudioPort)
    {
        dwAudioIp = ntohl(m_tAudioIp);
        wAudioPort = ntohs(m_tAudioPort);
    }

    
    void SetDStreamAddr(u32 dwDStreamIp, u16 wDStreamPort)
    {
        m_tDStreamIp = htonl(dwDStreamIp);
        m_tDStreamPort = htons(wDStreamPort);
    }

    void GetDStreamAddr(u32 &dwDStreamIp, u16 &wDStreamPort)
    {
        dwDStreamIp = ntohl(m_tDStreamIp);
        wDStreamPort = ntohs(m_tDStreamPort);
    }
}
#ifndef WIN32
__attribute__((packed))
#endif
;

//¼����ŵ�״̬
struct TRecChnnlStatus
{
	/*ͨ����״̬*/
	enum ERecChnnlState
	{
		STATE_IDLE       = 0,	  /*������һ��δ��ͨ��*/

		STATE_CALLING    = 5,	  /*����vrs��¼�����ں���ʵ��*/

		STATE_RECREADY   = 11,	  /*׼��¼��״̬  */
		STATE_RECORDING  = 12,	  /*����¼��  */
		STATE_RECPAUSE   = 13,	  /*��ͣ¼��״̬*/

		STATE_PLAYREADY  = 21,	  /*׼������״̬ */
		STATE_PLAYREADYPLAY = 22,   /*���ò���׼��״̬*/
		STATE_PLAYING    = 23,	  /*���ڻ򲥷�*/
		STATE_PLAYPAUSE  = 24,	  /*��ͣ����*/
		STATE_FF         = 25,	  /*���(������ͨ����Ч)*/
		STATE_FB         = 26	  /*����(������ͨ����Ч)*/
	};

	/*ͨ�����Ͷ���*/
	enum ERecChnnlType
	{
		TYPE_UNUSE      =  0,	/*δʼ�õ�ͨ��*/
		TYPE_RECORD     =  1,	/*¼��ͨ��  */   
		TYPE_PLAY       =  2	/*����ͨ��  */
	};

	//¼��ʽ
	enum ERecMode
	{
		MODE_SKIPFRAME = 0,//��֡¼��
		MODE_REALTIME  = 1  //ʵʱ¼��
	};
public:
	u8  		m_byType;		//ͨ�����ͣ�ERecChnnlType
	u8  		m_byState;		//ͨ��״̬��ERecChnnlState
	u8          m_byRecMode;    //¼��ʽ:
	TRecProg	m_tProg;	//��ǰ¼���������
	TMtNoConstruct			m_tSrc;
protected:
	char	m_achRecordName[MAXLEN_RECORD_NAME];//��¼��
	
public:
	//��ȡ��¼��
	LPCSTR GetRecordName( void ) const	{ return m_achRecordName; }
	//���ü�¼��
	void SetRecordName( LPCSTR lpszName ) 
	{
		strncpy( m_achRecordName, lpszName, sizeof( m_achRecordName ) );
		m_achRecordName[sizeof( m_achRecordName ) - 1] ='\0';
	}

	LPCSTR GetStatusStr( u8 eStatus )
	{
		switch(eStatus) 
		{
		case STATE_IDLE:
			return "IDLE";
			break;
		case STATE_CALLING:
			return "CALLING";
			break;
		case STATE_RECREADY:
			return "REC Ready";
			break;
		case STATE_RECORDING:
			return "Recording";
			break;
		case STATE_RECPAUSE:
			return "REC Pause";
			break;
		case STATE_PLAYREADY:
			return "PLAY Ready";
			break;
		case STATE_PLAYING:
			return "Playing";
			break;
		case STATE_PLAYPAUSE:
			return "Play Pause";
			break;
		case STATE_FF:
			return "Play FF";
			break;
		case STATE_FB:
			return "Play FB";
		default:
			return "UNKOWN";
		}
	}

	void print()
	{
        StaticLog( "[%s]:\n ChlType.%d  State.%s  byRecMode.%d  Prog.(%d/%d)  tSrc(%d,%d)\n",
			GetRecordName(), m_byType, GetStatusStr(m_byState), m_byRecMode, m_tProg.GetCurProg(), m_tProg.GetTotalTime(),
			m_tSrc.GetMcuId(), m_tSrc.GetMtId());
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//¼���״̬, size = 2412
struct TRecStatus
{
protected:
	u8  	m_byRecChnnlNum;		//¼���ŵ�������
	u8  	m_byPlayChnnlNum;		//�����ŵ�������
	u8      m_bSupportPublic;       // �Ƿ�֧�ַ���(TRUE: ֧��, FALSE:��֧��)
	u8      m_byRemain;             // ����
 	u32   	m_dwFreeSpace;        //¼���ʣ����̿ռ�(��λΪMB)
	u32   	m_dwTotalSpace;		//¼����ܴ��̿ռ�(��λΪMB)
	TRecChnnlStatus m_tChnnlStatus[MAXNUM_RECORDER_CHNNL];	//��¼���ŵ�������ŵ�

public:
	//��ȡ¼��ͨ������
	u8   GetRecChnnlNum( void ) const	{ return( m_byRecChnnlNum ); }
	//��ȡ����ͨ������
	u8   GetPlayChnnlNum( void ) const	{ return( m_byPlayChnnlNum ); }
	//����ͨ������¼��ͷ���ͨ���������벻����MAXNUM_RECORDER_CHNNL
	//ʧ�ܷ���FALSE
	BOOL SetChnnlNum( u8   byRecChnnlNum, u8   byPlayChnnlNum )
	{
		if( byRecChnnlNum + byPlayChnnlNum <= MAXNUM_RECORDER_CHNNL )
		{
			m_byRecChnnlNum = byRecChnnlNum;
			m_byPlayChnnlNum = byPlayChnnlNum;
			return( TRUE );
		}
		else
		{
			OspPrintf( TRUE, FALSE, "TRecStatus: Exception - Wrong recorder and play channel number: %u and %u!\n", 
				byRecChnnlNum, byPlayChnnlNum );
			return( FALSE );
		}
	}
		/*====================================================================
    ����        ������¼���ʣ����̿ռ�
    �������˵����u32    dwFreeSpace  - ʣ��ռ��С(��λ Mb)
    ����ֵ˵��  ����
	====================================================================*/
	void SetFreeSpaceSize( u32    dwFreeSpace )
	{
		m_dwFreeSpace  = htonl(dwFreeSpace);
	}
	
	/*====================================================================
    ����        ����ȡ¼���ʣ����̿ռ�
    �������˵������
    ����ֵ˵��  ��ʣ��ռ��С(��λ MB)
	====================================================================*/
	u32    GetFreeSpaceSize( void ) const	{ return ntohl( m_dwFreeSpace ); }

		/*====================================================================
    ����        ������¼����ܴ��̿ռ�
    �������˵����u32    dwTotalSpace  - �ܿռ��С(��λ MB)
    ����ֵ˵��  ����
	====================================================================*/
	void SetTotalSpaceSize( u32    dwTotalSpace )
	{
		m_dwTotalSpace  = htonl( dwTotalSpace );
	}
	// �Ƿ�֧�ַ���
	BOOL IsSupportPublic() const { return m_bSupportPublic;}
    // �����Ƿ�֧�ַ���
	void SetPublicAttribute(BOOL bSupportPublic ) { m_bSupportPublic = bSupportPublic;}

	/*====================================================================
    ����        ����ȡ¼����ܴ��̿ռ�
    �������˵������
    ����ֵ˵��  ���ܿռ��С(��λ MB)
	====================================================================*/
	u32    GetTotalSpaceSize( void ) const	{ return ntohl( m_dwTotalSpace ); }

	//����¼����ŵ�״̬��Ϣ������������¼�����ŵ����ٵ��ô˺���
	BOOL SetChnnlStatus( u8   byChnnlIndex, u8   byChnnlType, const TRecChnnlStatus * ptStatus );
	//����¼����ŵ�״̬��Ϣ������������¼�����ŵ����ٵ��ô˺���
	BOOL GetChnnlStatus( u8   byChnnlIndex, u8   byChnnlType, TRecChnnlStatus * ptStatus ) const;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// �����Ӧ�Ƚ�С�������������
#define RECLIST_PACKSIZE    (u8)16
struct TRecFileListNotify
{
	u16 wListSize;//¼������ļ�����
	u16 wStartIdx;//��֪ͨ����ʼ�ļ����
	u16 wEndIdx;  //��֪ͨ�н����ļ����
    
	s8 achFileName[RECLIST_PACKSIZE][MAXLEN_CONFNAME+MAXLEN_RECORD_NAME+1];
	u8 abyStatus[RECLIST_PACKSIZE];
public:
	TRecFileListNotify()
	{
		wListSize = 0;
		wStartIdx = 0;
		wEndIdx   = 0;
		memset ( achFileName ,0,sizeof(achFileName) );
		memset( abyStatus ,0 ,sizeof(abyStatus) );
	}

	//��ȡ¼�����¼����
	u16 GetListSize()
	{
		return ntohs(wListSize);
	}
	
	//��ȡ��֪ͨ����ʼ�ļ����
	u16 GetStartIdx()
	{
		return ntohs(wStartIdx);
	}

	//��ȡ��֪ͨ����ʼ�ļ����
	u16 GetEndIdx()
	{
		return ntohs(wEndIdx);
	}

	//���ָ����ŵ��ļ���,�粻���ڷ���NULL
	char* GetFileName(u16 wIdx )
	{
		if( wIdx < GetStartIdx() || wIdx > GetEndIdx() )
			return NULL;
		return achFileName[wIdx - GetStartIdx()];
	}

	//�ж�ָ����ŵ��ļ��Ƿ񷢲�
	BOOL IsPublic(u16 wIdx)
	{
		if( wIdx < GetStartIdx() || wIdx > GetEndIdx() )
			return FALSE;
		return abyStatus[wIdx - GetStartIdx()]&0x1 ? TRUE:FALSE;
	}

	//����ָ����ŵ��ļ�����
	BOOL Public(u16 wIdx)
	{
		if( wIdx < GetStartIdx() || wIdx > GetEndIdx() )
			return FALSE;

		abyStatus[wIdx - GetStartIdx()] |=0x1;
		return TRUE;
	}

	/////////////////////////////////////////////
	//����¼�����¼����
	void Reset()
	{
		wListSize = 0;
	    wStartIdx =0;
	    wEndIdx =0;
	    memset( achFileName,0,sizeof(achFileName) );
		memset( abyStatus , 0,sizeof(abyStatus) );
	}
	void SetListSize( u16 wSize)
	{
		wListSize = htons(wSize);
	}
	
	//���ñ�֪ͨ����ʼ�ļ����
	void SetStartIdx( u16 wIdx)
	{
		wStartIdx = htons(wIdx);
	}

	//���ñ�֪ͨ����ʼ�ļ����
	void SetEndIdx( u16 wIdx)
	{
		wEndIdx = htons(wIdx);
	}

	// ����ļ�
	BOOL AddFile(char* filename ,BOOL bPublic = FALSE )
	{
		if ( IsFull() )
			return FALSE;

        u16 wIdx = GetEndIdx() - GetStartIdx();

		strncpy( achFileName[wIdx], filename ,sizeof(achFileName[wIdx]) - 1);
		achFileName[wIdx][sizeof(achFileName)-1]='\0';

		if ( bPublic ) 
        {
            abyStatus[wIdx] |=0x1;
        }
		else 
        {
            abyStatus[wIdx] &=~0x01;
        }

        // ����Զ�����
        SetEndIdx( GetEndIdx() + 1 );
		return TRUE;
	}

	//�жϻ����Ƿ�����
	BOOL32 IsFull()
	{
		u16 wSize = GetEndIdx() - GetStartIdx();
		if( wSize >= RECLIST_PACKSIZE )
			return TRUE;
		return FALSE;
	}

    BOOL32 IsEmpty()
    {
        return (GetEndIdx() == GetStartIdx());
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

#define RECLIST_PACKSIZE    (u8)16
#define MAXLEN_FILENAME_LEN (u16)256
struct TRecListNewNotify
{
	u16 wListSize;//¼������ļ�����
	u16 wStartIdx;//��֪ͨ����ʼ�ļ����
	u16 wEndIdx;  //��֪ͨ�н����ļ����
    
	s8 achFileName[RECLIST_PACKSIZE][MAXLEN_FILENAME_LEN];
	u8 abyStatus[RECLIST_PACKSIZE];
public:
	TRecListNewNotify()
	{
		wListSize = 0;
		wStartIdx = 0;
		wEndIdx   = 0;
		memset ( achFileName ,0,sizeof(achFileName) );
		memset( abyStatus ,0 ,sizeof(abyStatus) );
	}

	//��ȡ¼�����¼����
	u16 GetListSize()
	{
		return ntohs(wListSize);
	}
	
	//��ȡ��֪ͨ����ʼ�ļ����
	u16 GetStartIdx()
	{
		return ntohs(wStartIdx);
	}

	//��ȡ��֪ͨ����ʼ�ļ����
	u16 GetEndIdx()
	{
		return ntohs(wEndIdx);
	}

	//���ָ����ŵ��ļ���,�粻���ڷ���NULL
	char* GetFileName(u16 wIdx )
	{
		if( wIdx < GetStartIdx() || wIdx > GetEndIdx() )
			return NULL;
		return achFileName[wIdx - GetStartIdx()];
	}

	//�ж�ָ����ŵ��ļ��Ƿ񷢲�
	BOOL IsPublic(u16 wIdx)
	{
		if( wIdx < GetStartIdx() || wIdx > GetEndIdx() )
			return FALSE;
		return abyStatus[wIdx - GetStartIdx()]&0x1 ? TRUE:FALSE;
	}

	//����ָ����ŵ��ļ�����
	BOOL Public(u16 wIdx)
	{
		if( wIdx < GetStartIdx() || wIdx > GetEndIdx() )
			return FALSE;

		abyStatus[wIdx - GetStartIdx()] |=0x1;
		return TRUE;
	}

	/////////////////////////////////////////////
	//����¼�����¼����
	void Reset()
	{
		wListSize = 0;
	    wStartIdx =0;
	    wEndIdx =0;
	    memset( achFileName,0,sizeof(achFileName) );
		memset( abyStatus , 0,sizeof(abyStatus) );
	}
	void SetListSize( u16 wSize)
	{
		wListSize = htons(wSize);
	}
	
	//���ñ�֪ͨ����ʼ�ļ����
	void SetStartIdx( u16 wIdx)
	{
		wStartIdx = htons(wIdx);
	}

	//���ñ�֪ͨ����ʼ�ļ����
	void SetEndIdx( u16 wIdx)
	{
		wEndIdx = htons(wIdx);
	}

	// ����ļ�
	BOOL AddFile(char* filename ,BOOL bPublic = FALSE )
	{
		if ( IsFull() )
			return FALSE;

        u16 wIdx = GetEndIdx() - GetStartIdx();

		strncpy( achFileName[wIdx], filename ,sizeof(achFileName[wIdx]) - 1);
		achFileName[wIdx][sizeof(achFileName)-1]='\0';

		if ( bPublic ) 
        {
            abyStatus[wIdx] |=0x1;
        }
		else 
        {
            abyStatus[wIdx] &=~0x01;
        }

        // ����Զ�����
        SetEndIdx( GetEndIdx() + 1 );
		return TRUE;
	}

	//�жϻ����Ƿ�����
	BOOL32 IsFull()
	{
		u16 wSize = GetEndIdx() - GetStartIdx();
		if( wSize >= RECLIST_PACKSIZE )
			return TRUE;
		return FALSE;
	}

    BOOL32 IsEmpty()
    {
        return (GetEndIdx() == GetStartIdx());
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TPlayFileAttrib
{
protected:
    u8 m_byFileType;
    u8 m_byAudioType;
    u8 m_byVideoType;
    u8 m_byDVideoType;

public:
    TPlayFileAttrib(void)
    {
        m_byFileType = RECFILE_NORMAL;
        m_byAudioType = MEDIA_TYPE_NULL;
        m_byVideoType = MEDIA_TYPE_NULL;
        m_byDVideoType = MEDIA_TYPE_NULL;
    }

    BOOL32 IsDStreamFile(void) const
    {
        return (RECFILE_DSTREAM == m_byFileType) ? TRUE : FALSE;
    }
    u8 GetFileType(void) const
    {
        return m_byFileType;
    }
    void SetFileType(u8 byFileType)
    {
        m_byFileType = byFileType;
    }

    u8 GetAudioType(void) const
    {
        return m_byAudioType;
    }
    void SetAudioType(u8 byAudioType)
    {
        m_byAudioType = byAudioType;
    }

    u8 GetVideoType(void) const
    {
        return m_byVideoType;
    }
    void SetVideoType(u8 byVideoType)
    {
        m_byVideoType = byVideoType;
    }

    u8 GetDVideoType(void) const
    {
        return m_byDVideoType;
    }
    void SetDVideoType(u8 byDVideoType)
    {
        m_byDVideoType = byDVideoType;
    }
    
    void Print() const
    {
        StaticLog( "m_byFileType: %d\n", m_byFileType);
        StaticLog( "m_byAudioType: %d\n", m_byAudioType);
        StaticLog( "m_byVideoType: %d\n", m_byVideoType);
        StaticLog( "m_byDVideoType: %d\n", m_byDVideoType);
    }
}
#ifndef WIN32
__attribute__ ((packed))
#endif
;

// zgc, 2008-08-02, MCU�ķ����ļ�ý����Ϣ
struct TPlayFileMediaInfo
{
    enum emStreamType
    {
        emAudio = 0,
        emVideo,
        emDVideo,
        emEnd
    };

private:
    //���ĸ���
    u8 m_byNum;
    //��������
    u8 m_abyMediaType[emEnd];
    u16	m_awWidth[emEnd];
	u16 m_awHeight[emEnd];

public:
    void clear( void )
    {
        m_byNum = 0;
        for ( u8 byLop = 0; byLop < emEnd; byLop++ )
        {
            m_abyMediaType[byLop] = MEDIA_TYPE_NULL;
            m_awWidth[byLop] = 0;
            m_awHeight[byLop] = 0;
        }
    }
    void SetStreamNum( u8 byNum ) { m_byNum = byNum; }
    u8   GetStreamNum( void ) const { return m_byNum; }
    void SetAudio( u8 byAudioType ) { m_abyMediaType[emAudio] = byAudioType; }
    u8   GetAudio( void ) const { return m_abyMediaType[emAudio]; }
    void SetVideo( u8 byVideoType, u16 wWidth, u16 wHeight ) 
    { 
        m_abyMediaType[emVideo] = byVideoType; 
        m_awWidth[emVideo] = htons(wWidth);
        m_awHeight[emVideo] = htons(wHeight);
        return;
    }
    void GetVideo( u8 &byVideoType, u16 &wWidth, u16 &wHeight ) const 
    { 
        byVideoType = m_abyMediaType[emVideo]; 
        wWidth = ntohs(m_awWidth[emVideo]);
        wHeight = ntohs(m_awHeight[emVideo]);
        return;
    }
    void SetDVideo( u8 byDVideoType, u16 wWidth, u16 wHeight ) 
    { 
        m_abyMediaType[emDVideo] = byDVideoType; 
        m_awWidth[emDVideo] = htons(wWidth);
        m_awHeight[emDVideo] = htons(wHeight);
        return;
    }
    void GetDVideo( u8 &byDVideoType, u16 &wWidth, u16 &wHeight ) const 
    { 
        byDVideoType = m_abyMediaType[emDVideo]; 
        wWidth = ntohs(m_awWidth[emDVideo]);
        wHeight = ntohs(m_awHeight[emDVideo]);
        return;
    }
}
#ifndef WIN32
__attribute__ ((packed))
#endif
;

/*----------------------------------------------------------------------
�ṹ����CConfVidTypeMgr
��;  �������������͹���
˵��  �����ն���Ŀǰ�������ն˺�¼���
----------------------------------------------------------------------*/
/*
class CConfVidTypeMgr
{  
public:
    enum emMediaType
    {        
        emH264_1080 = 0,
        emH264_720,
        emH264_4CIF,
        emH264_CIF,
        emOther, // ����Ƶ��ʽ/˫��ͬ����Ƶ��ʽ

		emH239H264_UXGA,
        emH239H264_SXGA,
        emH239H264_XGA,
        emH239H264_SVGA,
        emH239H264_VGA,
        emDVidOther // ˫����ʽ
    };
    
public:
    u8 m_abyMediaType[emOther+1];
    u8 m_abyDualMediaType[emDVidOther+1];     // ˫��������H264�����ʽ��Other��������
    
public:
    CConfVidTypeMgr () 
    { 
        memset( m_abyMediaType, 0, sizeof(m_abyMediaType) ); 
        memset( m_abyDualMediaType, 0, sizeof(m_abyDualMediaType) );
    }

    void ConvertIn2Out( u8 emType, u8 &byOutType, u8 &byOutRes )
    {
        emMediaType emIntype = (emMediaType)emType;
        if ( emIntype == emOther ||
             emIntype == emDVidOther )
        {
            byOutType = MEDIA_TYPE_NULL;
            byOutRes = 0;
            return;
        }
        else
        {
            byOutType = MEDIA_TYPE_H264;
        }

        switch( emIntype )
        {
        case emH264_1080:
            byOutRes = VIDEO_FORMAT_HD1080;
            break;
        case emH264_720:
            byOutRes = VIDEO_FORMAT_HD720;
            break;
        case emH264_4CIF:
            byOutRes = VIDEO_FORMAT_4CIF;
            break;
        case emH264_CIF:
            byOutRes = VIDEO_FORMAT_CIF;
            break;
        case emH239H264_VGA:
            byOutRes = VIDEO_FORMAT_VGA;
            break;
        case emH239H264_SVGA:
            byOutRes = VIDEO_FORMAT_SVGA;
            break;
        case emH239H264_XGA:
            byOutRes = VIDEO_FORMAT_XGA;
            break;
        case emH239H264_SXGA:
            byOutRes = VIDEO_FORMAT_SXGA;
            break;
		case emH239H264_UXGA:
			byOutRes = VIDEO_FORMAT_UXGA;
			break;
        default:
            byOutType = MEDIA_TYPE_NULL;
            byOutRes = 0;
            break;
        }
        return;
    }
    
    u8 ConvertOut2In( u8 byOutType, u8 byOutRes, BOOL32 bDVid = FALSE )
    {
        emMediaType emIntype;
        switch( byOutType )
        {
        case MEDIA_TYPE_H264:
            {
                switch( byOutRes )
                {
                case VIDEO_FORMAT_CIF:
                    emIntype = emH264_CIF;
                    break;
                case VIDEO_FORMAT_4CIF:
                    emIntype = emH264_4CIF;
                    break;
                case VIDEO_FORMAT_HD720:
                    emIntype = emH264_720;
                    break;
                case VIDEO_FORMAT_HD1080:
                    emIntype = emH264_1080;
                    break;
                case VIDEO_FORMAT_VGA:
                    emIntype = emH239H264_VGA;
                    break;
                case VIDEO_FORMAT_SVGA:
                    emIntype = emH239H264_SVGA;
                    break;
                case VIDEO_FORMAT_XGA:
                    emIntype = emH239H264_XGA;
                    break;
                case VIDEO_FORMAT_SXGA:
                    emIntype = emH239H264_SXGA;
                    break;
				case VIDEO_FORMAT_UXGA:
					emIntype = emH239H264_UXGA;
					break;
                default:
                    emIntype = bDVid ? emDVidOther : emOther;
                    break;
                }
            }
            break;
        default:
            emIntype = bDVid ? emDVidOther : emOther;
            break;
        }
        return (u8)emIntype;
    }
    
    BOOL32 IsTypeExist( u8 emType, BOOL32 bDVid = FALSE )
    {
        if ( !bDVid )
        {
            if ( !IsVGAType(emType) )
            {
                return ( m_abyMediaType[(emMediaType)emType] > 0 );
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            return ( m_abyDualMediaType[(emMediaType)emType] > 0 );
        }
    }

    void AddType( u8 emType, BOOL32 bDVid = FALSE )
    {
        if ( !bDVid )
        {
            if ( !IsVGAType(emType) )
            {
                m_abyMediaType[(emMediaType)emType]++;
            }
        }
        else
        {
            m_abyDualMediaType[(emMediaType)emType]++;
        }
        return;
    }

    void DelType( u8 emType, BOOL32 bDVid = FALSE )
    {
        if ( IsTypeExist(emType, bDVid) )
        {
            if ( !bDVid )
            {
                m_abyMediaType[(emMediaType)emType]--;
            }
            else
            {
                m_abyDualMediaType[(emMediaType)emType]--;
            }
        }
        return;
    }
    
    u8 GetTypeNum(BOOL32 bDVid = FALSE)
    {
        u8 byNum = 0;
        if ( !bDVid )
        {          
            for( u8 byLop = 0; byLop < sizeof(m_abyMediaType); byLop++ )
            {
                if ( m_abyMediaType[(emMediaType)byLop] > 0 )
                {
                    byNum++;
                }
            }
        }
        else
        {
            for( u8 byLop = 0; byLop < sizeof(m_abyDualMediaType); byLop++ )
            {
                if ( m_abyDualMediaType[(emMediaType)byLop] > 0 )
                {
                    byNum++;
                }
            }
        }
        return byNum;
    }
    
    u8 GetH264TypeNum(BOOL32 bDVid = FALSE)
    {
        u8 byNum = 0;
        if ( !bDVid )
        {          
            for( u8 byLop = 0; byLop < sizeof(m_abyMediaType); byLop++ )
            {
                if ( m_abyMediaType[(emMediaType)byLop] > 0 && (emMediaType)byLop != emOther )
                {
                    byNum++;
                }
            }
        }
        else
        {
            for( u8 byLop = 0; byLop < sizeof(m_abyDualMediaType); byLop++ )
            {
                if ( m_abyDualMediaType[(emMediaType)byLop] > 0 &&
                     (emMediaType)byLop != emOther &&
                     (emMediaType)byLop != emDVidOther )
                {
                    byNum++;
                }
            }
        }
        return byNum;
    }

    u8 GetAdaptTypeNum( u8 bySrcOutType, u8 bySrcOutRes, BOOL32 bDVid = FALSE )
    {
        u8 emType = ConvertOut2In( bySrcOutType, bySrcOutRes, bDVid );
        u8 byRecvTypeNum = GetAdaptTypeNum( emType, bDVid );
        return byRecvTypeNum;
    }

    u8 GetAdaptTypeNum( u8 emSrcType, BOOL32 bDVid = FALSE )
    {
        u8 byRecvTypeNum = 0;
        if ( !bDVid )
        {
            for ( u8 byLop = (emMediaType)emSrcType+1; byLop < sizeof(m_abyMediaType); byLop++ )
            {
                if ( IsTypeExist(byLop, bDVid) )
                {
                    byRecvTypeNum++;
                }
            }
        }
        else
        {
            for ( u8 byLop = (emMediaType)emSrcType+1; byLop < sizeof(m_abyDualMediaType); byLop++ )
            {
                if ( IsTypeExist(byLop, bDVid) &&
                     // ����Ŀ��Ӧ�ú�Դ��˫��������ͬ
                     IsVGAType(emSrcType) == IsVGAType(byLop) &&
                     byLop != emOther && byLop != emDVidOther )
                {
                    byRecvTypeNum++;
                }
                if ( IsVGAType(emSrcType) != IsVGAType(byLop) )
                {
                    break;
                }
            }
        }
        return byRecvTypeNum;
    }

    BOOL32 IsVGAType( u8 emType )
    {
        emMediaType emInType = (emMediaType)emType;
        return ( emInType <= emOther ) ? FALSE : TRUE;
    }

    u8  GetMinH264AdaptType( u8 emSrcType, BOOL32 bDVid = FALSE )
    {
        emMediaType emIntype = (emMediaType)emSrcType;
        emMediaType emMinType = emIntype;
        if ( !bDVid )
        {
            for ( u8 byLop = emIntype+1; byLop < emOther; byLop++ )
            {
                if ( IsTypeExist(byLop, bDVid) )
                {
                    emMinType = (emMediaType)byLop;
                }
            }
        }
        else
        {
            for ( u8 byLop = emIntype+1; byLop < emDVidOther; byLop++ )
            {
                if ( IsTypeExist(byLop, bDVid) && byLop != emOther )
                {
                    emMinType = (emMediaType)byLop;
                }
            }
        }

        return (u8)emMinType;
    }
	void Print( void )
	{
		u8 byType = 0;

		StaticLog( "�����������Ⱥ��Ϊ��\n" );
		for(byType = emH264_1080; byType <= emOther; byType ++)
		{
			if (m_abyMediaType[byType] != 0)
			{
				StaticLog( "\tType.%d, num.%d\n", byType, m_abyMediaType[byType] );
			}
		}
		StaticLog( "\n" );

		StaticLog( "˫���������Ⱥ��Ϊ��\n" );
		for(byType = emH239H264_UXGA; byType <= emDVidOther; byType ++)
		{
			if (m_abyDualMediaType[byType] != 0)
			{
				StaticLog( "\tType.%d, num.%d\n", byType, m_abyDualMediaType[byType] );
			}
		}
		StaticLog( "\n" );
	}
}
#ifndef WIN32
__attribute__ ((packed))
#endif
;*/

// [12/23/2009 xliang] --------------------------------------------8000E
//modify bas 2
//���������������ṹ
/*
struct T8KEAdaptParam
{
protected:
	u8         m_byCodeType;  //�����������
	u8         m_byActiveType;//���������̬�غ�����

	u16 	   m_wBitRate;//��������
	u16 	   m_wWidth; //ͼ����
	u16 	   m_wHeight;//ͼ����
	
	u8         m_byNeedPrs;

    u8         m_byFps; //����֡��, zgc, 2009-03-16

public:
    u16  GetWidth(void) const	{ return (ntohs(m_wWidth)); };
    u16  GetHeight(void) const	{ return (ntohs(m_wHeight)); };
    u16  GetBitrate(void) const { return (ntohs(m_wBitRate));};
    void SetResolution(u16 wWidth, u16 wHeight)
    {
        m_wWidth = htons(wWidth);
        m_wHeight = htons(wHeight);
    }
	
	void SetBitRate(u16 wBitrate)
	{
	    m_wBitRate = htons(wBitrate);
	}
	
	void SetCodeType(u8 byType)
	{
		m_byCodeType = byType;
	}
	u8 GetCodeType(void){ return m_byCodeType ;}

	void SetActiveType(u8 byType)
	{
		m_byActiveType = (u8) byType;
	}
	u8 GetActiveType(void){ return m_byActiveType; }

	void SetIsNeedByPrs(BOOL32 bNeedPrs)
	{
		m_byNeedPrs = (TRUE == bNeedPrs) ? 1 : 0;
	}

	BOOL32 IsNeedbyPrs(void)
	{
		return (0 != m_byNeedPrs);
	}

    void SetFps(u8 byFps)
    {
        m_byFps = byFps;
    }
	u8 GetFps(void){ return m_byFps; }
    
    u8 GetMeidaMode(void)
    {
        u8 byMediaMode = MODE_NONE;
        switch( m_byCodeType ) 
        {
        case MEDIA_TYPE_H261:
        case MEDIA_TYPE_H262:
        case MEDIA_TYPE_H263:
        case MEDIA_TYPE_H263PLUS:
        case MEDIA_TYPE_H264:
        case MEDIA_TYPE_MP4:
            byMediaMode = MODE_VIDEO;
            break;
        case MEDIA_TYPE_PCMA:
        case MEDIA_TYPE_PCMU:
        case MEDIA_TYPE_G722:
        case MEDIA_TYPE_G7221C:
        case MEDIA_TYPE_G7231:
        case MEDIA_TYPE_G728:
        case MEDIA_TYPE_G729:
        case MEDIA_TYPE_MP3:
            byMediaMode = MODE_AUDIO;
            break;
        default:
            break;
        }
	    return byMediaMode;
    }
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;
*/


//modify bas 2
//���������������ṹ
struct TAdaptParam
{
    enum EVideoType
    {
        vNONE  = MEDIA_TYPE_NULL,//��Ƶ����������
        vMPEG4 = MEDIA_TYPE_MP4,
        vH261  = MEDIA_TYPE_H261,
        vH263  = MEDIA_TYPE_H263,
        vH264  = MEDIA_TYPE_H264,
    };
    enum EAudioType
    {
        aNONE  = MEDIA_TYPE_NULL,//��Ƶ����������
        aMP3   = MEDIA_TYPE_MP3,
        aPCMA  = MEDIA_TYPE_PCMA,
        aPCMU  = MEDIA_TYPE_PCMU,
        aG7231 = MEDIA_TYPE_G7231,
        aG728  = MEDIA_TYPE_G728,
        aG729  = MEDIA_TYPE_G729,
    };

public:
    u16  GetWidth(void) const	{ return (ntohs(m_wWidth)); };
    u16  GetHeight(void) const	{ return (ntohs(m_wHeight)); };
    u16  GetBitrate(void) const { return (ntohs(m_wBitRate));};

    void SetResolution(u16 wWidth, u16 wHeight)
    {
        m_wWidth = htons(wWidth);
        m_wHeight = htons(wHeight);
    }
	
	void SetBitRate(u16 wBitrate)
	{
	    m_wBitRate = htons(wBitrate);
	}
	
	void SetVidType(u8 eType)
	{
		m_byVidCodeType = (u8)eType;
	}
	u8 GetVidType(void){ return m_byVidCodeType ;}
	
	void SetAudType(u8 eType)
	{
		m_byAudCodeType = (u8)eType;
	}
	u8 GetAudType(void){ return m_byAudCodeType; }

	void SetVidActiveType(u8 eType)
	{
		m_byVidActiveType = (u8)eType;
	}
	u8 GetVidActiveType(void){ return m_byVidActiveType; }
	
	void SetAudActiveType(u8 eType)
	{
		m_byAudActiveType = (u8)eType;
	}
	u8 GetAudActiveType(void){ return m_byAudActiveType; }

	void SetIsNeedByPrs(BOOL32 bNeedPrs)
	{
		m_byNeedPrs = (TRUE == bNeedPrs) ? 1 : 0;
	}

	BOOL32 IsNeedbyPrs(void)
	{
		return (0 != m_byNeedPrs);
	}

    BOOL32 IsNull(void) const
    {
        return (MEDIA_TYPE_NULL == m_byAudCodeType || 0 == m_byAudCodeType ) &&
               (MEDIA_TYPE_NULL == m_byVidCodeType || 0 == m_byVidCodeType );
    }
    void Clear(void)
    {
        m_byAudCodeType = MEDIA_TYPE_NULL;
        m_byVidCodeType = MEDIA_TYPE_NULL;
        m_byAudActiveType = MEDIA_TYPE_NULL;
        m_byVidActiveType = MEDIA_TYPE_NULL;
        m_wBitRate = 0;
        m_wWidth = 0;
        m_wHeight = 0;
        m_byNeedPrs = 0;
    }

	// [11/1/2011 liuxu] ��ӡ����
	void PrintTitle() const 
	{
        StaticLog( "%8s %8s %8s %8s %8s %8s %8s %8s\n", 
			"AudCodeT", "VidCodeT", "AudActT", "VidActT",
			"Bitrate", "Width", "Height", "NeedPrs" );
	}

    void Print(void) const
    {
		StaticLog( "%8d %8d %8d %8d %8d %8d %8d %8d\n", 
			m_byAudCodeType, m_byVidCodeType, m_byAudActiveType, m_byVidActiveType,
			GetBitrate(), GetWidth(), GetHeight(), m_byNeedPrs );
    }

private:
    u8         m_byAudCodeType;  //������Ƶ�������
    u8         m_byVidCodeType;  //������Ƶ�������
    u8         m_byAudActiveType;//������Ƶ�����̬�غ�����
    u8         m_byVidActiveType;//������Ƶ�����̬�غ�����
    
    u16 	   m_wBitRate;//��������
    u16 	   m_wWidth; //ͼ����
    u16 	   m_wHeight;//ͼ����
    
    u8         m_byNeedPrs;
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

/*----------------------------------------------------------------------
�ṹ����THDAdaptParam
��;  ��HD�������
----------------------------------------------------------------------*/
struct THDAdaptParam : public TAdaptParam
{
public:
    u8 GetFrameRate( void ) { return m_byFrameRate; }
    void SetFrameRate(u8 byFrameRate) { m_byFrameRate = byFrameRate; }

	u8 GetProfileType( void ) { return m_byProfileType; }
    void SetProfileType(u8 byProfileType) { m_byProfileType = byProfileType; }

    void Reset(void)
    {
        Clear();
        m_byFrameRate = 0;
        m_byProfileType = 0;
        m_byReserve2 = 0;
        m_byReserve3 = 0;
    }

	// [11/1/2011 liuxu] ��ӡ����
	void PrintTitle() const 
	{
		StaticLog("--------------------------------------------------------------------------------\n");
		StaticLog( "%7s ", "FrmRate");
		StaticLog( "%7s ", "ProType");
		TAdaptParam::PrintTitle();
		StaticLog("--------------------------------------------------------------------------------\n");
	}
	
    void PrintHd(void) const
    {
        StaticLog( "%7d ", m_byFrameRate);
		StaticLog( "%7d ", m_byProfileType);
		TAdaptParam::Print();
    }

private:
    u8 m_byFrameRate;
    u8 m_byProfileType;
    u8 m_byReserve2;
    u8 m_byReserve3;
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;


//������ͨ��״̬�ṹ
struct T8KEBasChnStatus : THDAdaptParam   //T8KEAdaptParam    //[03/01/2010] zjl modify bas �������ͳһ����ΪTHDAdaptParam��mcu�࿿£
{
    enum EBasStatus
    {
        IDLE   = 0, //����
        READY  = 1, //������δ����
        RUNING = 2  //������������
    };
    
protected:
    u8 m_byStatus;          // ͨ��״̬
    u8 m_byIsReserved;      // �Ƿ�ռ��
    u8 m_byChanType;        // ��Ƶͨ��: 1(BAS_CHAN_AUDIO)  ��Ƶͨ��: 2(BAS_CHAN_VIDEO)
    CConfId m_cChnConfId;   // ����ID, ���ֶ�ȡ��
#ifdef _CARRIER_
    u32 m_byConfIdx;
#else
    u8 m_byConfIdx;
#endif
    u8 m_byReservedBis1;    // �����ռ�
    u16 m_wResvedBits2;     // �����ռ�
    
public:
    void Init(void)
    {
        m_byStatus = IDLE;
        m_byIsReserved = 0;
        m_byChanType = 0;
        m_cChnConfId.SetNull();
        SetVidType(MEDIA_TYPE_NULL);
		SetAudType(MEDIA_TYPE_NULL);
    }
    
    void SetStatus( u8 byStatus) { m_byStatus = byStatus; }
    u8 GetStatus(void) const { return m_byStatus; }
    
    void SetIsReserved( BOOL32 bIsReserved ) { m_byIsReserved = (TRUE == bIsReserved) ? 1 : 0; }
    // ռ��ͨ��ʱ���������, zgc, 2009-03-27
#ifdef _CARRIER_
    void SetReserved( u32 byConfIdx ) 
#else
    void SetReserved( u8 byConfIdx )
#endif
    {
        SetIsReserved( TRUE );
        SetConfIdx( byConfIdx );
    }
    BOOL32 IsReserved(void) const { return (0 == m_byIsReserved) ? FALSE : TRUE ; }
    void UnReserved(void)
    {
        SetIsReserved( FALSE );
        SetConfIdx( 0 );
    }
    
    void SetChanType( u8 byBasChanType ) { m_byChanType = byBasChanType; }
    u8 GetChanType(void) const { return m_byChanType; }
    
    void SetConfId( const CConfId & cConfId ) { memcpy(&m_cChnConfId, &cConfId, sizeof(cConfId)); }
    CConfId GetConfId(void) const { return m_cChnConfId; }

#ifdef _CARRIER_
    u32 GetConfIdx( void ) const { return ntohl(m_byConfIdx); }
    void SetConfIdx( u32 dwConfIdx ) { m_byConfIdx = htonl(dwConfIdx); }
#else
    u8 GetConfIdx( void ) const { return m_byConfIdx; }
    void SetConfIdx( u8 byConfIdx ) { m_byConfIdx = byConfIdx; }
#endif
}
#ifndef WIN32
__attribute__ ((packed))
#endif
;



//������״̬�ṹ��MCU����������С��ԪΪһ��BAS��
struct T8KEBasStatus
{    
	u8            byChnNum;
    T8KEBasChnStatus tChnnl[MAXNUM_BAS_CHNNL];

    u8            byReserved;
#ifdef _CARRIER_
    u32           m_byConfIdx;
#else
    u8            m_byConfIdx;
#endif

public:
    void Reset(void)
    {
        UnReserved();
        byChnNum = 0;
        for ( u8 byIdx = 0; byIdx < MAXNUM_BAS_CHNNL; byIdx++ )
        {
            tChnnl[byIdx].Init();
        }
    }
#ifdef _CARRIER_
    void SetReserved( u32 byConfIdx )
#else
    void SetReserved( u8 byConfIdx )
#endif
    {
        byReserved = 1;
#ifdef _CARRIER_
        m_byConfIdx = htonl(byConfIdx);
#else
        m_byConfIdx = byConfIdx;
#endif
        
        // Ŀǰ������������Ϊ��λռ�ã�������ͨ��Ϊ��λ�����ｫͨ��Ҳռ�ã�
        // Ϊ�Ժ���ܵ��޸ļ���, zgc, 2009-03-27
        for ( u8 byIdx = 0; byIdx < MAXNUM_BAS_CHNNL; byIdx++ )
        {
            tChnnl[byIdx].SetReserved( byConfIdx );
        }
    }
    BOOL32 IsReserved() const
    {
        return ISTRUE(byReserved);
    }
    void UnReserved()
    {
        byReserved = 0; 
        m_byConfIdx = 0;
    }
#ifdef _CARRIER_
    u32   GetConfIdx()
    {
        return ntohl(m_byConfIdx);
    }
#else
    u8   GetConfIdx()
    {
        return m_byConfIdx;
    }
#endif
}
#ifndef WIN32
__attribute__((packed))
#endif
;

//������״̬(8KE)
struct T8KEMixerGrpStatus
{
	enum EState 
	{
		IDLE   = 0,		//�û�����δʹ�� 
		READY  = 1,		//���鴴��,��δ��ʼ����
		MIXING = 2,		//���ڻ���
	};

	u8       m_byGrpId;		  //������ID(0-4)
	u8       m_byGrpState;	  //��״̬
	u8       m_byGrpMixDepth; //�������
#ifdef _CARRIER_
    u32      m_byConfIdx;      //����ID
#else
    u8       m_byConfIdx;      //����ID
#endif
    u8       m_byIsReserved;  //�Ƿ�ռ��
public:
	//����״̬��λ
	void Reset()
	{
		m_byGrpId       = 0xff;
		m_byGrpState    = IDLE;
		m_byGrpMixDepth = 0;
        m_byIsReserved  = 0;
        m_byConfIdx = 0;
	}

    void SetReady()
    {
        m_byGrpState = READY;
    }
    BOOL32 IsReady() const
    {
        return ( m_byGrpState == READY ) ? TRUE : FALSE;
    }
    void SetMixing()
    {
        m_byGrpState = MIXING;
    }
    BOOL32 IsMixing() const
    {
        return ( m_byGrpState == MIXING ) ? TRUE : FALSE;
    }
#ifdef _CARRIER_
    void SetReserved( u32 dwConfIdx )
    {
        m_byIsReserved = 1;
        m_byConfIdx = htonl(dwConfIdx);
    }
    u32 GetConfIdx(void)
    {
        return ntohl(m_byConfIdx);
    }
#else
    void SetReserved( u8 byConfIdx )
    {
        m_byIsReserved = 1;
        m_byConfIdx = byConfIdx;
    }
    u8 GetConfIdx(void)
    {
        return m_byConfIdx;
    }
#endif

    BOOL32 IsReserved() const
    {
        return (m_byIsReserved == 0) ? FALSE : TRUE;
    }
    void UnReserved(void)
    {
        m_byIsReserved = 0;
        m_byConfIdx = 0;
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;



//������״̬ 8KE
struct T8KEMixerStatus
{
	u8 m_byGrpNum; //����������
	T8KEMixerGrpStatus	m_atGrpStatus[MAXNUM_MIXER_GROUP_8KE];

public:
    void Reset(void)
    {
        m_byGrpNum = 0;
        for ( u32 dwIdx = 0; dwIdx < MAXNUM_MIXER_GROUP_8KE; dwIdx++ )
        {
            m_atGrpStatus[dwIdx].Reset();
        }
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;



//------------------------------------------


/*----------------------------------------------------------------------
�ṹ����THDBasChnStatus
��;  ��HDBas����ͨ��
----------------------------------------------------------------------*/
struct TBasBaseChnStatus
{
    enum EBasStatus
    {
        IDLE   = 0,    //����
        READY  = 1,    //׼������
        RUNING = 2,    //��������
		WAIT_START = 3 //�ȴ�����
    };

public:
    void   SetStatus( u8 byStatus ) { m_byStatus = byStatus; };
    u8     GetStatus( void ) { return m_byStatus; }
    
    BOOL32 IsNull( void ) const { return m_tBas.IsNull(); }
    void   SetNull( void ) { m_tBas.SetNull(); }
    
    u8     GetReservedType( void ) { return m_byIsReserved; }
    BOOL32 IsReserved ( void ) const { return ( 1 == m_byIsReserved ); }
    void   SetIsReserved ( BOOL32 bIsReserved ) { m_byIsReserved = bIsReserved ? 1 : 0; }
    BOOL32 IsTempReserved ( void ) const { return ( 2 == m_byIsReserved ); }
    void   SetIsTempReserved ( BOOL32 bIsTempReserved ) { m_byIsReserved = bIsTempReserved ? 2 : 0; } 
    void   UnReserved( void ) { m_byIsReserved = 0; }
    
    void   SetChnIdx( u8 byChnIdx ) { m_byChnIdx = byChnIdx; }
    u8     GetChnIdx( void ) const { return m_byChnIdx; }
    
    void   SetEqp( TEqp tBas ) { m_tBas = tBas; }
    TEqp   GetEqp( void ) const { return m_tBas; }
    u8     GetEqpId( void ) const { return m_tBas.GetEqpId(); }	
    void   SetType( u8 byType ) { m_byType = byType; };
    u8     GetType( void ) { return m_byType; }


    void   Print() const
    {
		// [11/4/2011 liuxu] ��ӡ����
		StaticLog("Bas Eqp:\n");
		TEqp::PrintTitle();
		m_tBas.PrintContent();

		StaticLog("\nChnnl Info:\n");
		StaticLog("--------------------------------------\n");
		StaticLog("%10s %8s %8s %8s\n", "EBasStatus", "Reserved", "ChnnlIdx", "BasType");
		StaticLog("--------------------------------------\n");
		StaticLog("%10d %8d %8d %8d\n", m_byStatus, m_byIsReserved, m_byChnIdx, m_byType);
    }

private:
    TEqpNoConstruct m_tBas;
    u8 m_byStatus;	// EBasStatus
    u8 m_byIsReserved; // 0 -- not reserve, 1 -- reserve, 2 -- temply reserve
	u8 m_byChnIdx;
    u8 m_byType; // zw 20081114
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

struct TAudAdaptParam 
{
private:
	u8	m_byAudCodeType;
	u8	m_byAudActiveTpye;
	u8	m_byTrackNum;
	u8  m_byIsNeedPrs;
	u8	m_byReserve1;
	u8	m_byReserve2;
	
public:
	//������Ƶ��������
	void SetAudCodeType( u8 byAudCodeType )
	{
		m_byAudCodeType = byAudCodeType;
	}
	
	u8 GetAudCodeType()
	{
		return m_byAudCodeType;
	}
	
	//������Ƶ�����غ�
	void SetAudActiveType( u8 byAudActiveType )
	{
		m_byAudActiveTpye = byAudActiveType;
	}
	
	u8 GetAudActiveType()
	{
		return m_byAudActiveTpye;
	}
	
	//������Ƶ����������
	void SetTrackNum( u8 byTrackNum )
	{
		m_byTrackNum = byTrackNum;
	}
	
	u8 GetTrackNum( )
	{
		return m_byTrackNum;
	}
	
	
	void SetIsNeedByPrs(BOOL32 bNeedPrs)
	{
		m_byIsNeedPrs = (TRUE == bNeedPrs) ? 1 : 0;
	}
	
	BOOL32 IsNeedbyPrs(void)
	{
		return (0 != m_byIsNeedPrs);
	}
	
	
    void Clear(void)
    {
        m_byAudCodeType = MEDIA_TYPE_NULL;
		m_byAudActiveTpye = MEDIA_TYPE_NULL;
		m_byTrackNum = 0;
    }
	
	BOOL32 IsNull()
	{
		if ( m_byAudCodeType == MEDIA_TYPE_NULL)
		{
			return TRUE;
		}
		return FALSE;
	}
	
	void PrintTitle() const 
	{
		StaticLog("--------------------------------------------------------------------------------\n");
		StaticLog( "%7s ", "CodType");
		StaticLog( "%7s ", "ActType");
		StaticLog( "%7s ", "TrackNm");
		StaticLog( "%7s \n","NeedPrs");
		StaticLog("--------------------------------------------------------------------------------\n");
	}
	
    void PrintParam(void) const
    {
        StaticLog( "%7d ", m_byAudCodeType);
		StaticLog( "%7d ", m_byAudActiveTpye);
		StaticLog( "%7d",  m_byTrackNum);
		StaticLog( "%7d\n", m_byIsNeedPrs);
    }
	
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

//BAS��Ƶͨ��״̬
struct TAudBasChnStatus:public TBasBaseChnStatus
{
public:
    TAudAdaptParam  m_atOutPutAudParam[MAXNUM_BASOUTCHN];   //����ͨ�������������
public:
    TAudAdaptParam* GetOutputAudParam( u8 byChnIdx )
    {
        if ( byChnIdx < MAXNUM_BASOUTCHN )
        {
            return &m_atOutPutAudParam[byChnIdx]; 
        }
        else
        {
            return NULL;
        }
    }
    void SetOutputAudParam(TAudAdaptParam &tVidParam, u8 byChnIdx)
    {
        m_atOutPutAudParam[byChnIdx] = tVidParam;
        return;
    }
    
    void ClrOutputAudParam()
    {
        TAudAdaptParam tNullAdpParam;
        tNullAdpParam.Clear();  
        for (u8 byChnlIdx = 0; byChnlIdx < MAXNUM_BASOUTCHN; byChnlIdx++)
        {
            m_atOutPutAudParam[byChnlIdx] = tNullAdpParam;
        }
    }
    
    void PrintInfo() const
    {
        StaticLog( "\nAudChnStatus info as follows:\n");
        Print();
        
        StaticLog("\nChnnl Output Param:\n");
        m_atOutPutAudParam[0].PrintTitle();
        for(u8 byIdx = 0; byIdx < MAXNUM_BASOUTCHN; byIdx ++)
        {
            m_atOutPutAudParam[byIdx].PrintParam();
        }
        
        StaticLog("\n");
    }
}
#ifndef WIN32
__attribute__((packed))
#endif
;

//apu2-bas����״̬
struct TApu2BasStatus
{
public:
    TAudBasChnStatus m_atAudChnStatus[MAXNUM_APU2_BASCHN];  //APU2 BAS����ͨ������
public:
    void SetAudChnStatus (TAudBasChnStatus &tAudChnStatus, u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_APU2_BASCHN )
        {
            m_atAudChnStatus[byChnIdx] = tAudChnStatus;
        }
        return;
    }
    TAudBasChnStatus * GetAudChnStatus(u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_APU2_BASCHN )
        {
            return &m_atAudChnStatus[byChnIdx];
        }
        else
        {
            return NULL;
        }
    }
    void Print() const
    {
        StaticLog( "TApu2BasStatus is as follows:\n");
        u8 byIdx = 0;
        for(byIdx = 0; byIdx < MAXNUM_APU2_BASCHN; byIdx++)
        {
            if(!m_atAudChnStatus[byIdx].IsNull())
            {
                m_atAudChnStatus[byIdx].PrintInfo();
            }           
        }
    }
}
#ifndef WIN32
__attribute__((packed))
#endif
;


//apu2-bas����״̬
struct T8kiAudBasStatus
{
public:
    TAudBasChnStatus m_atAudChnStatus[MAXNUM_8KI_AUD_BASCHN];  //APU2 BAS����ͨ������
public:
    void SetAudChnStatus (TAudBasChnStatus &tAudChnStatus, u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_8KI_AUD_BASCHN )
        {
            m_atAudChnStatus[byChnIdx] = tAudChnStatus;
        }
        return;
    }
    TAudBasChnStatus * GetAudChnStatus(u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_8KI_AUD_BASCHN )
        {
            return &m_atAudChnStatus[byChnIdx];
        }
        else
        {
            return NULL;
        }
    }
    void Print() const
    {
        StaticLog( "T8kiAudBasStatus is as follows:\n");
        u8 byIdx = 0;
        for(byIdx = 0; byIdx < MAXNUM_8KI_AUD_BASCHN; byIdx++)
        {
            if(!m_atAudChnStatus[byIdx].IsNull())
            {
                m_atAudChnStatus[byIdx].PrintInfo();
            }           
        }
    }
}
#ifndef WIN32
__attribute__((packed))
#endif
;

/*----------------------------------------------------------------------
�ṹ����THDBasVidChnStatus
��;  ��HDBas��Ƶͨ����¼״̬
----------------------------------------------------------------------*/
struct THDBasVidChnStatus : public TBasBaseChnStatus //len:39
{
private:
	THDAdaptParam m_atOutputVidParam[MAXNUM_BASOUTCHN/*MAXNUM_VOUTPUT*/]; //[03/01/2010] zjl modify 

public:	
    THDAdaptParam * GetOutputVidParam( u8 byChnIdx )
    {
        if ( byChnIdx < MAXNUM_BASOUTCHN/*MAXNUM_VOUTPUT*/ ) //[03/01/2010] zjl modify 
        {
            return &m_atOutputVidParam[byChnIdx]; 
        }
        else
        {
            return NULL;
        }
    }
    void SetOutputVidParam(THDAdaptParam &tVidParam, u8 byChnIdx , BOOL32 bH263plusChn = FALSE, u8 byChnType = 0)
    {
// 		if (BAS_8KECHN_MV !=byChnType && 
// 			BAS_8KECHN_DS !=byChnType &&
// 			BAS_8KECHN_SEL !=byChnType &&
// 			byChnIdx < MAXNUM_VOUTPUT)
// 		{
// 			if ((tVidParam.GetVidType() != MEDIA_TYPE_H264 ||
// 				(tVidParam.GetVidType() == MEDIA_TYPE_H264 && 
// 				tVidParam.GetWidth() == 352 && tVidParam.GetHeight() == 288)
// 			 	)
// 		    	)
// 			{
// 				if (0 == byChnIdx && !bH263plusChn)
// 				{
// 					byChnIdx = 1;
// 					OspPrintf(TRUE, FALSE, "[SetOutputVidParam]change outputchanl to 1 automaticly\n");
// 				} 
// 			}
// 			else
// 			{
// 				if (byChnIdx == 1)
// 				{
// 					byChnIdx = 0;
// 					OspPrintf(TRUE, FALSE, "[SetOutputVidParam]change outputchanl to 0 automaticly\n");
// 				}
// 			}
// 			m_atOutputVidParam[byChnIdx] = tVidParam;
// 		}
// 		else if ((BAS_8KECHN_MV == byChnType || 
// 			      BAS_8KECHN_DS == byChnType || 
// 				  BAS_8KECHN_SEL == byChnType ) && 
// 				  byChnIdx < MAXNUM_8KEBAS_VOUTPUT)
// 		{
// 			m_atOutputVidParam[byChnIdx] = tVidParam;
// 			OspPrintf(TRUE, FALSE, "[SetOutputVidParam] SetOutputVidParam to 8kechn:%d!\n", byChnIdx);
// 		}
// 		else
// 		{
// 			OspPrintf(TRUE, FALSE, "[SetOutputVidParam] SetOutputVidParam failed! ChnType:%d, ChnIdx:%d, tVidParam<Enctype:%d, Width:%d, Height:%d>\n",
// 				byChnType, byChnIdx, 
// 				tVidParam.GetVidType(), tVidParam.GetWidth(), tVidParam.GetHeight());
// 		}
		m_atOutputVidParam[byChnIdx] = tVidParam;
        return;
    }

	void ClrOutputVidParam()
	{
 		THDAdaptParam tNullAdpParam;
		tNullAdpParam.Reset();  
		for (u8 byChnlIdx = 0; byChnlIdx < MAXNUM_BASOUTCHN/*MAXNUM_VOUTPUT*/; byChnlIdx++)
		{
			m_atOutputVidParam[byChnlIdx] = tNullAdpParam;
		}
	}

    void PrintInfo() const
    {
		// [11/1/2011 liuxu] ��ӡ����
        StaticLog( "\nHDBasVidChnStatus info as follows:\n");
        Print();
		
		StaticLog("\nChnnl Output Param:\n");
		m_atOutputVidParam[0].PrintTitle();
        for(u8 byIdx = 0; byIdx < MAXNUM_BASOUTCHN; byIdx ++)
        {
            m_atOutputVidParam[byIdx].PrintHd();
        }
		
		StaticLog("\n");
    }
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

typedef THDBasVidChnStatus THDBasDVidChnStatus;

/*----------------------------------------------------------------------
�ṹ����TMauBasStatus
��;  ��MauBas״̬�࣬�ӽ�TPeriEqpStatus
----------------------------------------------------------------------*/
class TMauBasStatus
{
public:
    void SetVidChnStatus (THDBasVidChnStatus &tVidChnStatus, u8 byChnIdx = 0)
    {
        if ( byChnIdx < MAXNUM_MAU_VCHN )
        {
            m_atVidChnStatus[byChnIdx] = tVidChnStatus;
        }
        return;
    }
	void SetDVidChnStatus (THDBasDVidChnStatus &tDVidChnStatus, u8 byChnIdx = 0)
    {
        if ( byChnIdx < MAXNUM_MAU_DVCHN )
        {
            m_atDVidChnStatus[byChnIdx] = tDVidChnStatus;
        }
        return;
    }
	THDBasVidChnStatus * GetVidChnStatus(u8 byChnIdx = 0)
    {
        if ( byChnIdx < MAXNUM_MAU_VCHN )
        {
            return &m_atVidChnStatus[byChnIdx];
        }
        else
        {
            return NULL;
        }
    }
	THDBasDVidChnStatus * GetDVidChnStatus(u8 byChnIdx = 0)
    {
        if ( byChnIdx < MAXNUM_MAU_DVCHN )
        {
            return &m_atDVidChnStatus[byChnIdx];
        }
        else
        {
            return NULL;
        }
    }
    void Print() const
    {
        StaticLog( "TMauBasStatus is as follows:\n");
        u8 byIdx = 0;
        for(byIdx = 0; byIdx < MAXNUM_MAU_VCHN; byIdx++)
        {
            m_atVidChnStatus[byIdx].PrintInfo();
        }
        for(byIdx = 0; byIdx < MAXNUM_MAU_DVCHN; byIdx++)
        {
            m_atDVidChnStatus[byIdx].PrintInfo();
        }
    }
private:
    THDBasVidChnStatus m_atVidChnStatus[MAXNUM_MAU_VCHN];	
    THDBasDVidChnStatus m_atDVidChnStatus[MAXNUM_MAU_DVCHN];
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

/*----------------------------------------------------------------------
�ṹ����TMpuBasStatus
��;  ��MpuBas״̬�࣬�ӽ�TPeriEqpStatus
----------------------------------------------------------------------*/
class TMpuBasStatus
{
public:
    void SetVidChnStatus (THDBasVidChnStatus &tVidChnStatus, u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_MPU_CHN )
        {
            m_atVidChnStatus[byChnIdx] = tVidChnStatus;
        }
        return;
    }
	THDBasVidChnStatus * GetVidChnStatus(u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_MPU_CHN )
        {
            return &m_atVidChnStatus[byChnIdx];
        }
		else
        {
            return NULL;
        }
    }
    void Print() const
    {
        StaticLog( "TMpuBasStatus is as follows:\n");
        u8 byIdx = 0;
        for(byIdx = 0; byIdx < MAXNUM_MPU_VCHN; byIdx++)
        {
			if(!m_atVidChnStatus[byIdx].IsNull())
			{
				m_atVidChnStatus[byIdx].PrintInfo();
			}           
        }
    }
private:
    THDBasVidChnStatus m_atVidChnStatus[MAXNUM_MPU_VCHN];
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

//[03/01/2010] zjl add 8kebasStatus
/*----------------------------------------------------------------------
�ṹ����T8keBasStatus
��;  ��T8keBasStatus״̬�࣬�ӽ�TPeriEqpStatus  
----------------------------------------------------------------------*/
class T8keBasStatus
{
public:
    void SetVidChnStatus (THDBasVidChnStatus &tVidChnStatus, u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_8KEBAS_VCHN )
        {
            m_atVidChnStatus[byChnIdx] = tVidChnStatus;
        }
        return;
    }
	THDBasVidChnStatus * GetVidChnStatus(u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_8KEBAS_VCHN )
        {
            return &m_atVidChnStatus[byChnIdx];
        }
        else
        {
            return NULL;
        }
    }
    void Print() const
    {
        StaticLog( "T8keBasStatus is as follows:\n");
        u8 byIdx = 0;
        for(byIdx = 0; byIdx < MAXNUM_8KEBAS_VCHN; byIdx++)
        {
            m_atVidChnStatus[byIdx].PrintInfo();
        }
    }
private:
    THDBasVidChnStatus m_atVidChnStatus[MAXNUM_8KEBAS_VCHN];
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

//[03/31/2011] nzj add 8khbasStatus
/*----------------------------------------------------------------------
�ṹ����T8khBasStatus
��;  ��T8khBasStatus״̬�࣬�ӽ�TPeriEqpStatus  
----------------------------------------------------------------------*/
class T8khBasStatus
{
public:
    void SetVidChnStatus (THDBasVidChnStatus &tVidChnStatus, u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_8KHBAS_VCHN )
        {
            m_atVidChnStatus[byChnIdx] = tVidChnStatus;
        }
        return;
    }
	THDBasVidChnStatus * GetVidChnStatus(u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_8KHBAS_VCHN )
        {
            return &m_atVidChnStatus[byChnIdx];
        }
        else
        {
            return NULL;
        }
    }
    void Print() const
    {
        OspPrintf(TRUE, FALSE, "T8khBasStatus is as follows:\n");
        u8 byIdx = 0;
        for(byIdx = 0; byIdx < MAXNUM_8KHBAS_VCHN; byIdx++)
        {
            m_atVidChnStatus[byIdx].PrintInfo();
        }
    }
private:
    THDBasVidChnStatus m_atVidChnStatus[MAXNUM_8KHBAS_VCHN];
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

class T8kiVidBasStatus
{
public:
    void SetVidChnStatus (THDBasVidChnStatus &tVidChnStatus, u8 byChnIdx)
    {
		//ͨ������ȷ��[7/30/2013 chendaiwei]
        if ( byChnIdx < 3 )
        {
            m_atVidChnStatus[byChnIdx] = tVidChnStatus;
        }
        return;
    }
	THDBasVidChnStatus * GetVidChnStatus(u8 byChnIdx)
    {
        if ( byChnIdx < 3 )
        {
            return &m_atVidChnStatus[byChnIdx];
        }
        else
        {
            return NULL;
        }
    }
    void Print() const
    {
        OspPrintf(TRUE, FALSE, "T8kiVidBasStatus is as follows:\n");
        u8 byIdx = 0;
        for(byIdx = 0; byIdx < 3; byIdx++)
        {
            m_atVidChnStatus[byIdx].PrintInfo();
        }
    }
private:
    THDBasVidChnStatus m_atVidChnStatus[3];
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;


/*----------------------------------------------------------------------
�ṹ����TMpu2BasStatus
��;  ��Mpu2Bas״̬�࣬�ӽ�TPeriEqpStatus
----------------------------------------------------------------------*/
class TMpu2BasStatus
{
public:
    void SetVidChnStatus (THDBasVidChnStatus &tVidChnStatus, u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_MPU2_EHANCED_CHN )
        {
            m_atVidChnStatus[byChnIdx] = tVidChnStatus;
        }
        return;
    }
	THDBasVidChnStatus * GetVidChnStatus(u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_MPU2_EHANCED_CHN )
        {
            return &m_atVidChnStatus[byChnIdx];
        }
		else
        {
            return NULL;
        }
    }
    void Print() const
    {
        StaticLog( "TMpu2BasStatus is as follows:\n");
        u8 byIdx = 0;
        for(byIdx = 0; byIdx < MAXNUM_MPU2_EHANCED_CHN; byIdx++)
        {
			if(!m_atVidChnStatus[byIdx].IsNull())
			{
				m_atVidChnStatus[byIdx].PrintInfo();
			}           
        }
    }
private:
    THDBasVidChnStatus m_atVidChnStatus[MAXNUM_MPU2_EHANCED_CHN];
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;
/*----------------------------------------------------------------------
�ṹ����TMpuBasStatus
��;  ��MpuBas״̬�࣬�ӽ�TPeriEqpStatus
----------------------------------------------------------------------*/
//TSpecialBasStatus�ýṹ��Ҫ������MPU2ģ���MPU�������ϵ�MCUʱʹ��
//��Ϊ�ϵ�union tStatus ���Ϊ400�ֽ�(sizeof(tMpuBas));���µ�����ÿ��ͨ������·�����ӣ�
//union tStatus����ֽڴﵽ��910(sizeof(tMpu2Bas) );�����ϵ�MCU�õ���m_byType����
//��ͨ��TSpecialBasStatus������401�ֽڵ�m_byType���������ؾ���ʹ��
struct TSpecialBasStatus
{
	u8  abyFillInData[400];
	u8	m_byType;
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

struct TAudBasStatus
{
public:
	union
	{
		u8 m_abyArray[910];//zjj20130922 Ϊ�˺�hdbas�ṹ��Сһ��(Ҫ�������m_byType),������������ṹ
		TApu2BasStatus m_tApu2BasChnStatus;
		T8kiAudBasStatus m_t8KIAudBasStatus;
	}m_tAudBasStaus;

public:
    void SetEqpType(u8 byType)
    {
        if (byType != TYPE_APU2_BAS && byType != TYPE_8KI_AUD_BAS)
        {
            OspPrintf(TRUE, FALSE, "[TAudBasStatus][SetType] unexpected type.%d\n", m_byType);
            return;
        }
        m_byType = byType;
        return;
    }
    u8   GetEqpType(void) const { return m_byType; }
    
public:
    u8 m_byType;
}
#ifndef WIN32
__attribute__((packed))
#endif
;

class THdBasStatus
{
public:
    union
    {
        TMauBasStatus tMauBas;
        TMpuBasStatus tMpuBas;
		T8keBasStatus t8keBas;  //[03/01/2010] zjl add 8kebas
		T8khBasStatus t8khBas;	//[03/31/2011] nzj add 8khbas
		TMpu2BasStatus tMpu2Bas; 
		TSpecialBasStatus tSpeBas;
		T8kiVidBasStatus t8kiBas;
    }tStatus;

public:
    void SetEqpType(u8 byType)
    {
        if (byType != TYPE_MPU &&
            byType != TYPE_MAU_NORMAL &&
            byType != TYPE_MAU_H263PLUS &&
			byType != TYPE_8KE_BAS &&
			byType != TYPE_8KH_BAS &&
			byType != TYPE_MPU_H &&
			byType != TYPE_MPU2_BASIC &&
			byType != TYPE_MPU2_ENHANCED &&
			byType != TYPE_8KI_VID_BAS)
        {
            OspPrintf(TRUE, FALSE, "[THdBasStatus][SetType] unexpected type.%d\n", m_byType);
            return;
        }
        m_byType = byType;
        return;
    }
    u8   GetEqpType(void) const { return m_byType; }

public:
    u8 m_byType;
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;


//��������״̬�ṹ, size = 2435(TRecStatus = 2412)
struct T8KEPeriEqpStatus : public TEqp
{
public:
	u8  	m_byOnline;		//�Ƿ�����
	union UStatus
	{
		TTvWallStatus	tTvWall;
		T8KEMixerStatus	tMixer;
		TVmpStatus      tVmp;
		TRecStatus		tRecorder;
		//T8KEBasStatus		tBas;
		THdBasStatus	tHdBas;		//[03/01/2010] zjl 8000e bas����״̬��mcu��£
		TPrsStatus		tPrs;
	} m_tStatus;
	
protected:
	char    m_achAlias[MAXLEN_EQP_ALIAS];
public:
	//constructor
	T8KEPeriEqpStatus( void )
	{
		memset(this, 0, sizeof(T8KEPeriEqpStatus));
	}
	
	//��ȡ�������
	const s8 * GetAlias() const 
	{ 
		return m_achAlias;
	}
	
	//�����������
	void SetAlias(const s8 * lpszAlias)
	{ 
        if ( NULL == lpszAlias )
        {
            memset(m_achAlias, '\0', sizeof(m_achAlias) );
        }
        else
        {
            strncpy(m_achAlias, lpszAlias, sizeof(m_achAlias));
			m_achAlias[MAXLEN_EQP_ALIAS-1] = '\0';
        }
	}
	
    BOOL32 IsOnline() const
    {
        return ISTRUE(m_byOnline);
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//������ͨ��״̬�ṹ
struct TBasChnStatus : TAdaptParam
{
    enum EBasStatus
    {
        IDLE   = 0,		//����
        READY  = 1,		//׼������
        RUNING = 2,		//��������
		WAIT_START = 3  //�ȴ�����
    };

protected:
    u8 m_byStatus;
	u8 m_byReserved;    //�Ƿ�Ϊ����ͨ��
    u8 m_byChanType;    //��Ƶͨ��: 1(BAS_CHAN_AUDIO)  ��Ƶͨ��: 2(BAS_CHAN_VIDEO)

public:

    void SetStatus(u8 byStatus)
    {
        m_byStatus = byStatus;
    }

    u8 GetStatus(void)
    {
        return m_byStatus;
    }

    void SetReserved(BOOL32 bReserved)
    {
        m_byReserved = (TRUE == bReserved) ? 1 : 0;
    }

    BOOL32 IsReserved(void)
    {
        return (0 != m_byReserved);
    }

    u8 GetChannelType(void)
    {
        return m_byChanType;
    }

    void SetChannelType(u8 byBasChanType)
    {
        m_byChanType = byBasChanType;
    }
}
#ifndef WIN32
__attribute__ ((packed))
#endif
;

//������������ģ��
struct TBasCapSet
{
public:
    u8 m_bySupportAudio;
    u8 m_bySupportVideo;
    u8 m_bySupportBitrate;

public:
    void SetAudioCapSet(BOOL32 bSupportAudio)
    {
        if (TRUE == bSupportAudio)
        {
            m_bySupportAudio = 1;
        }
        else
        {
            m_bySupportAudio = 0;
        }
    }

    BOOL32 IsSupportAudio(void)
    {
        if (0 == m_bySupportAudio)
        {
            return FALSE;
        }
        return TRUE;
    }

    void SetVideoCapSet(BOOL32 bSupportVideo)
    {
        if (TRUE == bSupportVideo)
        {
            m_bySupportVideo = 1;
        }
        else
        {
            m_bySupportVideo = 0;
        }
    }

    BOOL32 IsSupportVideo(void)
    {
        if (0 == m_bySupportVideo)
        {
            return FALSE;
        }
        return TRUE;
    }

    void SetBitrateCapSet(BOOL32 bSupportBitrate)
    {
        if (TRUE == bSupportBitrate)
        {
            m_bySupportBitrate = 1;
        }
        else
        {
            m_bySupportBitrate = 0;
        }
    }

    BOOL32 IsSupportBitrate(void)
    {
        if (0 == m_bySupportBitrate)
        {
            return FALSE;
        }
        return TRUE;
    }
}
#ifndef WIN32
__attribute__((packed))
#endif
;

//������ͨ��״̬�ṹ
struct TBasStatus
{
	u8            byChnNum;
    TBasCapSet    tBasCapSet;
    TBasChnStatus tChnnl[MAXNUM_BAS_CHNNL];
}
#ifndef WIN32
__attribute__((packed))
#endif
;


//����DCS״̬�ṹ
struct TPeriDcsStatus : public TEqp
{
public:
	u8	m_byOnline;			//�Ƿ�����
protected:
	s8	m_achDcsAlias[MAXLEN_EQP_ALIAS];
public:
	TPeriDcsStatus()
	{
		memset( this, 0, sizeof(TPeriDcsStatus) );
	}

	//��ȡDCS����
	const s8 * GetDcsAlias() const
	{
		return m_achDcsAlias;
	}
	//�����������
	void SetDcsAlias( const s8 * lpszAlias )
	{
		strncpy( m_achDcsAlias, lpszAlias, sizeof(m_achDcsAlias) );
		m_achDcsAlias[MAXLEN_EQP_ALIAS - 1] = '\0';
	}
	//�ÿ�
	void SetNull()
	{
		memset( this, 0, sizeof(TPeriDcsStatus) );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//��������״̬�ṹ, size = 2435(TRecStatus = 2412)
struct TPeriEqpStatus : public TEqp
{
public:
	u8  	m_byOnline;		//�Ƿ�����
	union UStatus
	{
		TTvWallStatus	tTvWall;
		TMixerStatus	tMixer;
		TVmpStatus      tVmp;
		TRecStatus		tRecorder;
		TBasStatus		tBas;
		TPrsStatus		tPrs;
        THdBasStatus    tHdBas;
		THduStatus      tHdu;
		TAudBasStatus	tAudBas;
	} m_tStatus;
	
protected:
	char    m_achAlias[MAXLEN_EQP_ALIAS];
public:
	//constructor
	TPeriEqpStatus( void )
	{
		memset(this, 0, sizeof(TPeriEqpStatus));

		// [2013/03/11 chenbing]  
		// HDUͨ��0��ͨ��1��Ĭ��Ϊ1���
		m_tStatus.tHdu.SetChnCurVmpMode(0, HDUCHN_MODE_ONE);
		m_tStatus.tHdu.SetChnCurVmpMode(1, HDUCHN_MODE_ONE);
	}

	//��ȡ�������
	const s8 * GetAlias() const 
	{ 
		return m_achAlias;
	}

	//�����������
	void SetAlias(const s8 * lpszAlias)
	{ 
		strncpy(m_achAlias, lpszAlias, sizeof(m_achAlias));
		m_achAlias[MAXLEN_EQP_ALIAS-1] = '\0';
	}

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//����Ҫ���ӵ��ն���Ϣ
struct TAddMtInfo: public TMtAlias
{
	u8   m_byCallMode;
	u16  m_wDialBitRate;//��������//��λKbps
public:
	TAddMtInfo(){Init();}

	void Init()
	{
		SetCallMode(255);
		SetCallBitRate(0);
	}

	u16  GetCallBitRate() { return ntohs(m_wDialBitRate); }
	void SetCallBitRate(u16 wDialBitRate) { m_wDialBitRate = htons(wDialBitRate); }

    u8   GetCallMode() { return m_byCallMode; }
    void SetCallMode(u8 byCallMode) { m_byCallMode = byCallMode; }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//���԰棬�����ն�ʱmcsָ������������Ϣ
struct TAddMtCapInfo
{
private:
	TVideoStreamCap  m_tMainVideoCap;
	TVideoStreamCap  m_tDStreamVideoCap;
public:
	TAddMtCapInfo()	{ clear(); }
	void clear()
	{
		m_tMainVideoCap.Clear();
		m_tDStreamVideoCap.Clear();		
	}
	const TVideoStreamCap& GetMainVideoCap() const    { return m_tMainVideoCap; }
	void SetMainVideoCap(TVideoStreamCap& tVideoSteamCap) 
	{
		memcpy( &m_tMainVideoCap, &tVideoSteamCap, sizeof(TVideoStreamCap) );
	}

	const TVideoStreamCap& GetDStreamVideoCap() const { return m_tDStreamVideoCap; }
	void SetDStreamVideoCap(TVideoStreamCap& tVideoSteamCap) 
	{
		memcpy( &m_tDStreamVideoCap, &tVideoSteamCap, sizeof(TVideoStreamCap) );
	}
	
	// ����Ƶ����
    void	SetMainMaxBitRate(u16 wMaxRate)  {  m_tMainVideoCap.SetMaxBitRate(wMaxRate); }	
    u16		GetMainMaxBitRate() const { return m_tMainVideoCap.GetMaxBitRate(); }
	
    void	SetMainMediaType(u8 byMediaType)  {  m_tMainVideoCap.SetMediaType(byMediaType); }	
    u8		GetMainMediaType() const { return m_tMainVideoCap.GetMediaType(); }
    
    void    SetMainResolution(u8 byRes) {   m_tMainVideoCap.SetResolution(byRes);    }
    u8      GetMainResolution(void) const   {   return m_tMainVideoCap.GetResolution();    }

    void    SetMainFrameRate(u8 byFrameRate)
    {
        if ( MEDIA_TYPE_H264 == m_tMainVideoCap.GetMediaType() )
        {
            OspPrintf( TRUE, FALSE, "Function has been preciated, try SetUserDefFrameRate\n" );
            return;
        }
        m_tMainVideoCap.SetFrameRate(byFrameRate);
		return;
    }
    u8      GetMainFrameRate(void) const
    {
        if ( MEDIA_TYPE_H264 == m_tMainVideoCap.GetMediaType() )
        {
            OspPrintf( TRUE, FALSE, "Function has been preciated, try GetUserDefFrameRate\n" );
            return 0;
        }
        return m_tMainVideoCap.GetFrameRate();
    }

    // �Զ���֡��
    void    SetUserDefMainFrameRate(u8 byActFrameRate) { m_tMainVideoCap.SetUserDefFrameRate(byActFrameRate); }
    BOOL32  IsMainFrameRateUserDefined() const {return m_tMainVideoCap.IsFrameRateUserDefined(); }
    u8      GetUserDefMainFrameRate(void) const { return m_tMainVideoCap.GetUserDefFrameRate(); }

	// ˫������
    void	SetDstreamMaxBitRate(u16 wMaxRate)  {  m_tDStreamVideoCap.SetMaxBitRate(wMaxRate); }	
    u16		GetDstreamMaxBitRate() const { return m_tDStreamVideoCap.GetMaxBitRate(); }

    void	SetDstreamMediaType(u8 byMediaType)  {  m_tDStreamVideoCap.SetMediaType(byMediaType); }	
    u8		GetDstreamMediaType() const { return m_tDStreamVideoCap.GetMediaType(); }
    
    void    SetDstreamResolution(u8 byRes) {   m_tDStreamVideoCap.SetResolution(byRes);    }
    u8      GetDstreamResolution(void) const   {   return m_tDStreamVideoCap.GetResolution();    }

    void    SetDstreamFrameRate(u8 byFrameRate)
    {
        if ( MEDIA_TYPE_H264 == m_tDStreamVideoCap.GetMediaType() )
        {
            OspPrintf( TRUE, FALSE, "Function has been preciated, try SetUserDefFrameRate\n" );
            return;
        }
        m_tDStreamVideoCap.SetFrameRate(byFrameRate);
		return;
    }
    u8      GetDstreamFrameRate(void) const
    {
        if ( MEDIA_TYPE_H264 == m_tDStreamVideoCap.GetMediaType() )
        {
            OspPrintf( TRUE, FALSE, "Function has been preciated, try GetUserDefFrameRate\n" );
            return 0;
        }
        return m_tDStreamVideoCap.GetFrameRate();
    }

    // �Զ���֡��
    void    SetUserDefDstreamFrameRate(u8 byActFrameRate) { m_tDStreamVideoCap.SetUserDefFrameRate(byActFrameRate); }
    BOOL32  IsDstreamFrameRateUserDefined() const {return m_tDStreamVideoCap.IsFrameRateUserDefined(); }
    u8      GetUserDefDstreamFrameRate(void) const { return m_tDStreamVideoCap.GetUserDefFrameRate(); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//���԰������ն˵���Ϣ������MCSָ���Ĳ����ն�������
struct TAddMtInfoEx: public TAddMtInfo
{
private:
	TAddMtCapInfo m_tMtCapInfo;

public:
	TAddMtInfoEx()	{ clear(); }
	void clear()
	{
		m_tMtCapInfo.clear();
	}

	const TAddMtCapInfo& GetMtCapInfo() const    { return m_tMtCapInfo; }
	void SetMtCapInfo(TAddMtCapInfo& tMtCapInfo) 
	{
		memcpy( &m_tMtCapInfo, &tMtCapInfo, sizeof(TAddMtCapInfo) );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;



//��������ն˽ṹ���ýṹ�����˲μӻ����MCU�������ն��б������ն��б��ʵ��
//����ն��б������߱�����ǰ�ߵ��Ӽ� 
struct TConfMtInfo
{
protected:   
	u16     m_wMcuIdx;          //��MC���ڵ�Mcu��Id��
    u8      m_byConfIdx;        //���������� 
	u8  	m_abyMt[MAXNUM_CONF_MT/8];	        //�����ն��б�
	u8  	m_abyJoinedMt[MAXNUM_CONF_MT/8];	//����ն��б�

public:
    void  SetMcuIdx(const u16 wMcuIdx){ m_wMcuIdx = htons(wMcuIdx); } 
    u16    GetMcuIdx( void ) const { return ntohs(m_wMcuIdx); }
    void  SetConfIdx(const u8  byConfIdx){ m_byConfIdx = byConfIdx; } 
    u8    GetConfIdx( void ) const { return m_byConfIdx; }

	TConfMtInfo( void );//constructor
	BOOL MtInConf(const u8 byMtId ) const;//�жϸ�MCU��ĳ�ն��Ƿ��������ն��б���
	BOOL MtJoinedConf(const u8 byMtId ) const;//�жϸ�MCU��ĳ�ն��Ƿ�������ն��б���
	void AddMt(const u8 byMtId );//���Ӹ�MCU���ն��ڻ����ն��б���
	void AddJoinedMt(const u8 byMtId );//���Ӹ�MCU���ն�������ն��б��У��ú����Զ������ն�Ҳ
	void RemoveMt(const u8 byMtId );//ɾ����MCU���ն��������ն��б��У��ú����Զ������ն�Ҳ
	void RemoveJoinedMt(const u8 byMtId );//ɾ����MCU���ն�������ն��б���
	void RemoveAllMt( void );//ɾ����MCU�������ն��������ն��б��У��ú����Զ��������ն�Ҳ
	void RemoveAllJoinedMt( void );//ɾ����MCU�������ն�������ն��б���
	u8   GetAllMtNum( void ) const;//�õ�һ���ṹ�����������ն˸���
	u8   GetAllJoinedMtNum( void ) const;//�õ�һ���ṹ����������ն˸���
	u8   GetAllUnjoinedMtNum( void ) const;//�õ�һ���ṹ������δ����ն˸���
	u8	 GetMaxMtIdInConf( void ) const;	//�õ�һ���ṹ�������ն���MtId����ֵ// xliang [12/24/2008] 

	void SetNull( void )
	{
		m_wMcuIdx = INVALID_MCUIDX;
		m_byConfIdx = 0;
		memset( &m_abyMt[0],0,sizeof(m_abyMt) );
		memset( &m_abyJoinedMt[0],0,sizeof(m_abyJoinedMt) );
	}
	
	BOOL IsNull( void ) const
	{
		if( m_wMcuIdx == INVALID_MCUIDX && m_byConfIdx == 0 )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	BOOL IsMtExists( void )
	{
		u8 byCount = sizeof(m_abyMt) / sizeof(m_abyMt[0]);
		for( u8 byIdx = 0;byIdx < byCount; ++byIdx )
		{
			if( m_abyMt[byIdx] != 0 )
			{
				return TRUE;
			}
		}
		return FALSE;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*====================================================================
    ������      ��IsValidMcuId
    ����        ��Mcu Id�ĺϷ��Լ��
    �㷨ʵ��    ��[0, MAXNUM_CONFSUB_MCU) || 19200 ��Χ�ڵ�Id���ǺϷ�Id, ���򲻺Ϸ�
    ����ȫ�ֱ�����
    �������˵����[in]wMcuIdx, Mcu Id����ֵ
    ����ֵ˵��  ��TRUE, �Ϸ�; FALSE, ���Ϸ�
	----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	10/12/30    4.6          ����          ����
====================================================================*/
inline BOOL32 IsValidMcuId(const u16 wMcuIdx) 
{
	if(wMcuIdx >= MAXNUM_CONFSUB_MCU && wMcuIdx != LOCAL_MCUIDX)
	{ 
		return FALSE; 
	}

	return TRUE;
}

/*====================================================================
    ������      ��IsLocalMcuId
    ����        ��wMcuId�Ƿ��Ǳ���McuId
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[in]wMcuId, MtId����ֵ
    ����ֵ˵��  ��TRUE, �Ϸ�; FALSE, ���Ϸ�
	----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	10/12/30    4.6          ����          ����
====================================================================*/
inline BOOL32 IsLocalMcuId(const u16 wMcuId) 
{
	return (LOCAL_MCUIDX == wMcuId);
}

/*====================================================================
    ������      ��IsValidSubMcuId
    ����        ��Sub Mcu Id�ĺϷ��Լ��
    �㷨ʵ��    ��[0, LOCAL_MCUID) || (LOCAL_MCUID, MAXNUM_CONFSUB_MCU)��Χ�ڵ�Id���ǺϷ�SUB mcuId, ���򲻺Ϸ�
    ����ȫ�ֱ�����
    �������˵����[in]wMcuIdx, Mcu Id����ֵ
    ����ֵ˵��  ��TRUE, �Ϸ�; FALSE, ���Ϸ�
	----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	10/12/30    4.6          ����          ����
====================================================================*/
inline BOOL32 IsValidSubMcuId(const u16 wMcuIdx) 
{
	BOOL32 bRet = IsValidMcuId(wMcuIdx) && !IsLocalMcuId(wMcuIdx);
	
	return bRet;
}

/*====================================================================
    ������      ��IsValidMtId
    ����        ��Mt��MtId�ĺϷ��Լ��
    �㷨ʵ��    ��(0, MAXNUM_CONF_MT)��Χ�ڵ�Id���ǺϷ�MtId, ���򲻺Ϸ�
    ����ȫ�ֱ�����
    �������˵����[in]wMtId, MtId����ֵ
    ����ֵ˵��  ��TRUE, �Ϸ�; FALSE, ���Ϸ�
	----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	10/12/30    4.6          ����          ����
====================================================================*/
inline BOOL32 IsValidMtId(const u8 byMtId) 
{
	// �ն˵�ʵ�ʱ����[1�� 192), 192��������;
	if(!byMtId || byMtId >= MAXNUM_CONF_MT)
	{ 
		return FALSE; 
	}
	
	return TRUE;
}


//��������mcu��Ϣ (len:1202)
struct TConfAllMcuInfo
{
private:
	u8			m_abyMcuInfo[MAXNUM_CONFSUB_MCU][MAX_SUBMCU_DEPTH];	
	u16			m_wSum;								// ����mcu���ܸ���, ����m_wSum <= MAXNUM_CONFSUB_MCU
													// ������, ����ֱ��ʹ��
public:
    TConfAllMcuInfo( void ){ Clear(); }

	/** 
	/* @��    �� : ���TConfAllMcuInfo�е���Ϣ
	/* @����˵�� : 
	/* @�� �� ֵ : 
	/* @�޸����� 		@��    ��            @��    ��          @��    ¼
	/*                     1.0                �ܾ���             ����
	*/
	void Clear() 
	{ 
		memset( this, 0, sizeof(TConfAllMcuInfo) ); 		
	}

	/*====================================================================
	������      ��IsMcuIdAdded
	����        �����TConfAllMcuInfo��mcu�ĸ���, �Ա�������TConfAllMcuInfo��mcu
	�㷨ʵ��    ��
	����ȫ�ֱ�����
	�������˵����
	����ֵ˵��  ������mcu������, ��������
	----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	11/01/06    4.6         liuxu         ����
	====================================================================*/
	BOOL32 GetAllMcuNum() const { return (MAXNUM_CONFSUB_MCU); } 

	/*====================================================================
		������      ��IsMcuIdAdded
		����        ���Ƿ��Ѿ����mcu��Ϣ
		�㷨ʵ��    ��
		����ȫ�ֱ�����
		�������˵����[in]pbyMcuId, ����,
					  [in]byCount, pbyMcuId��ֵ�ĸ���, ��ֵ��ΧΪ[0, MAX_SUBMCU_DEPTH]
		����ֵ˵��  �����ڷ���TRUE,�����ڷ���FALSE
	----------------------------------------------------------------------
		�޸ļ�¼    ��
		��  ��      �汾        �޸���        �޸�����
		10/07/08    4.6         �ܾ���         ����
	====================================================================*/
	BOOL32 IsMcuIdAdded( const u8* const pabyMcuId, const u8& byMcuIdCount );

	/** 
	/* @��    �� : ����mcuid ���һ��mcu
	/* @����˵�� : [in]pabyMcuId, ����,
				   [in]byMcuIdCount, pabyMcuId��ֵ�ĸ���, ��ֵ��ΧΪ(0, MAX_SUBMCU_DEPTH]
				   [out]pwMcuIdx, ��mcu��ӵ�indexֵ
	/* @�� �� ֵ : ��ӳɹ�, ����TRUE, ����FALSE
	/* @�޸����� 		@��    ��            @��    ��          @��    ¼
	/*                     1.0                �ܾ���             ����
	/*  2010/12/15         1.1                ��  ��             �޸�
	*/
	BOOL32 AddMcu( const u8* const pabyMcuId, const u8& byMcuIdCount, u16* const pwMcuIdx);

	/** 
	/* @��    �� : �����ϼ�mcu��mcu id�ҵ����������е��¼�mcu��idx
	/* @����˵�� : [in]pbyMcuId, ����,
				   [in]byCount, pbyMcuId��ֵ�ĸ���, ��ֵ��ΧΪ(0, MAX_SUBMCU_DEPTH]
				   [out]pwMcuIdx, �ҵ����¼�mcu��idx
				   [in]byMcuIdxCount, �����pwMcuIdx����ĳ���
	/* @�� �� ֵ : �ҵ����¼�mcu������, û�ҵ�����0
	/* @�޸����� 		@��    ��            @��    ��          @��    ¼
	/*                     1.0                �ܾ���             ����
	/*  2010/12/15         1.1                ��  ��             �޸�
	*/
	u8 GetMcuIdxByMcuId( const u8* pabyMcuId, 
						 const u8& byMcuIdCount, 
						 u16* const pawMcuIdx, 
						 const u8 byMcuIdxCount = MAXNUM_SUB_MCU);

	/** 
	/* @��    �� : ����MCU indexɾ��MCU
	/* @����˵�� : [in]pwMcuIdx, �ҵ����¼�mcu��idx
	/* @�� �� ֵ : ɾ���ɹ�, ����TRUE, ����FALSE
	/* @�޸����� 		@��    ��            @��    ��          @��    ¼
	/*                     1.0                �ܾ���             ����
	/*  2010/12/15         1.1                ��  ��             �޸�
	*/
	BOOL32 RemoveMcu(const u16& wMcuIdx );
	

	/** 
	/* @��    �� : ����MCU id��������Indexֵ
	/* @����˵�� : [in]pbyMcuId, ����,
				   [in]byCount, pbyMcuId��ֵ�ĸ���, ��ֵ��ΧΪ(0, MAX_SUBMCU_DEPTH]
				   [out]pwMcuIdx, �ҵ����¼�mcu��idx
	/* @�� �� ֵ : �ҵ�����TRUE, ����FALSE
	/* @�޸����� 		@��    ��            @��    ��          @��    ¼
	/*                     1.0                �ܾ���             ����
	/*  2010/12/15         1.1                ��  ��             �޸�
	*/
	BOOL32 GetIdxByMcuId( const u8* pabyMcuId, const u8& byMcuIdCount, u16* const pwMcuIdx );

	/** 
	/* @��    �� : �ж�wMcuIdx��ʾ��mcu�Ƿ��Ǳ�mcu��ֱ���¼�mcu
	/* @����˵�� : [in]wMcuIdx, �ҵ����¼�mcu��idx
	/* @�� �� ֵ : �ǵ�,����TRUE, ����FALSE
	/* @�޸����� 		@��    ��            @��    ��          @��    ¼
	/*                     1.0                �ܾ���             ����
	/*  2010/12/15         1.1                ��  ��             �޸�
	*/
	BOOL32 IsSMcuByMcuIdx( const u16& wMcuIdx );

	/** 
	/* @��    �� : ����mcu index�ҵ�����mcu id��Ϣ
	/* @����˵�� : [in]wMcuIdx, �ҵ����¼�mcu��idx
			       [out]pabyMcuId, ����, ������ҵ���mcu id
				   [in]byMcuIdCount, pabyMcuId��ֵ�ĸ���, ��ֵ��ΧΪ(0, MAX_SUBMCU_DEPTH]
	/* @�� �� ֵ : �ҵ�����TRUE, ����FALSE
	/* @�޸����� 		@��    ��            @��    ��          @��    ¼
	/*                     1.0                �ܾ���             ����
	/*  2010/12/15         1.1                ��  ��             �޸�
	*/
	BOOL32 GetMcuIdByIdx( const u16 wMcuIdx, u8* const pabyMcuId, const u8 byMcuIdCount = MAX_SUBMCU_DEPTH );

	/** 
	/* @��    �� : ����ֱ���¼�mcu������
	/* @����˵�� : 
	/* @�� �� ֵ : ����ʵ�ʵ�����
	/* @�޸����� 		@��    ��            @��    ��          @��    ¼
	/*                     1.0                �ܾ���             ����
	/*  2010/12/15         1.1                ���㻪             �޸�
	*/
	u8 GetSMcuNum( void );

	/** 
	/* @��    �� : �����¼�mcu������
	/* @����˵�� : 
	/* @�� �� ֵ : ����ʵ�ʵ�����
	/* @�޸����� 		@��    ��            @��    ��          @��    ¼
	/*  2010/12/15         1.1                ���㻪             ����
	*/
	u16 GetMcuCount( void ) { return ntohs( m_wSum );}
	
protected:
	// CompareByMcuIds�ıȽϽ������
	enum  E_CapareResult
	{
		E_CMP_RESULT_UP      = 0,						// ǰ��Ϊ���ߵ�ֱ���ϼ�mcu
		E_CMP_RESULT_EQUAL,								// ǰ��mcu���Ǻ���
		E_CMP_RESULT_DOWN,								// ǰ��Ϊ���ߵ�ֱ���¼�mcu
		E_CMP_RESULT_OTHER,								// ����û��ֱ�����¼���ϵ
		E_CMP_RESULT_INVALID,	    					// �����������,�Ƿ��Ƚ�

		E_CMP_RESULT_NUM								// �ȽϽ��������, ����������
	};

protected:
	/** 
	/* @��    �� : ���õ�ǰ������mcu������, ��������mcu
	/* @����˵�� : 
	/* @�� �� ֵ : ��
	/* @�޸����� 		@��    ��            @��    ��          @��    ¼
	/*  2010/12/15         1.1                ���㻪             ����
	*/
	void SetMcuCount( const u16& wCount )
	{
		// ���ֻ������600���¼�mcu. 
		if (wCount > MAXNUM_CONFSUB_MCU)
		{
			OspPrintf(TRUE, FALSE, "SetMcuCount Error, wCount = %d is greater than MAXNUM_CONFSUB_MCU\n", wCount);
			return;
		}

		m_wSum = htons( wCount );
	}

	/** 
	/* @��    �� : �ж�m_abyMcuInfo[wIndex]�Ƿ�Ϊ��
	/* @����˵�� : wIndex, m_abyMcuInfo�ĸ�������, ��Χ = [0, MAXNUM_CONFSUB_MCU)
	/* @�� �� ֵ : ��Ϊ��, ����True; ����False
	/* @�������� : 2010/12/15			@��    �� : 1.0                @��    �� : ����
	*/
	BOOL32 IsNull(const u16& wIndex) const;

	/** 
	/* @��    �� : ��wIndex��ʾ��mcu id����ֵ��pbyMcuId�Ƚ�
	/* @����˵�� : [in]wIndex, m_abyMcuInfo�ĸ�������, ��Χ = [0, MAXNUM_CONFSUB_MCU)
				   [in]pbyMcuId, ����,
				   [in]byCount, pbyMcuId��ֵ�ĸ���, ��ֵ��ΧΪ(0, MAX_SUBMCU_DEPTH]
	/* @�� �� ֵ : E_CMP_RESULT_UP, ����ֵΪwIndex��mcuΪpbyMcuId��ʾ��mcu���ϼ�
				   E_CMP_RESULT_EQUAL������ֵΪwIndex��mcu����pbyMcuId��ʾ��mcu
				   E_CMP_RESULT_DOWN������ֵΪwIndex��mcuΪpbyMcuId��ʾ��mcu���¼�
				   E_CMP_RESULT_OTHER, ����ֵΪwIndex��mcu��pbyMcuId��ʾ��mcuû��ֱ�����¼���ϵ
				   E_CMP_RESULT_INVALID, ��������Ƿ�, ���ܱȽ�
	/* @�������� : 2010/12/15			@��    �� : 1.0                @��    �� : ����
	*/
	E_CapareResult CompareByMcuIds(const u16& wIndex, const u8* const pabyMcuId, const u8& byMcuIdCount ) const;


	/** 
	/* @��    �� : ����mcu idѰ�Ҷ�Ӧ��idx
	/* @����˵�� : [in]pabyMcuId, ����,
				   [in]byMcuIdCount, pabyMcuId��ֵ�ĸ���, ��ֵ��ΧΪ(0, MAX_SUBMCU_DEPTH]
				   [out]pwIdleIndex, ����һ�����е�λ��
	/* @�� �� ֵ : [0��MAXNUM_CONFSUB_MCU), �ҵ���idxֵ
				   [MAXNUM_CONFSUB_MCU, INVALID_MCUIDX], û���ҵ�
	/* @�޸����� 		@��    ��            @��    ��          @��    ¼
	/* 2010/12/15         1.1                ��  ��                ����
	*/
	u16 FindMcuIdx(const u8* const pabyMcuId, const u8& byMcuIdCount, u16* const pwIdleIndex = NULL) const;


	/** 
	/* @��    �� : ����mcu id��ȡ�����������
	/* @����˵�� : [in]pabyMcuId, ����,
				   [in]byMcuIdCount, pabyMcuId��ֵ�ĸ���, ��ֵ��ΧΪ(0, MAX_SUBMCU_DEPTH]
	/* @�� �� ֵ : <= 0�� �Ƿ�ֵ
				   (0��byMcuIdCount], �ҵ��ĺϷ������ֵ
				   (byMcuIdCount, �����], �Ƿ�ֵ
	/* @�޸����� 		@��    ��            @��    ��          @��    ¼
	/* 2010/12/15         1.1                ��  ��                ����
	*/
	const s8 GetDepth(const u8* const pabyMcuId, const u8& byMcuIdCount) const;

	// ����wMcuId�Ƿ�Ϸ�, ���Ϸ�ֱ�ӷ���FALSE
#define ISVALIDMCUID(wMcuIdx) \
	if(!IsValidMcuId(wMcuIdx)) \
	    return FALSE;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//���������ն���Ϣ (len:3256)
struct TConfAllMtInfo
{
public:
	TConfMtInfo	m_tLocalMtInfo;
	
	TMt         m_tMMCU; //�������ϼ�MCU 

private:
	TConfMtInfo	m_atOtherMcMtInfo[MAXNUM_CONFSUB_MCU];

public:
    TConfAllMtInfo( void ){ Clear(); }

	void Clear() { 
		memset( this, 0, sizeof(TConfAllMtInfo) ); 
		m_tLocalMtInfo.SetNull();
		
		u16 wLoop = 0;
		while ( wLoop < MAXNUM_CONFSUB_MCU) {
			m_atOtherMcMtInfo[wLoop].SetNull();
			++wLoop;
		}
	}

	const u16 GetMaxMcuNum() const { return MAXNUM_CONFSUB_MCU; }	// liuxu, ��������mcu����,
																// ��m_atOtherMcMtInfo������

    void   SetLocalMtInfo(TConfMtInfo tLocalMtInfo){ m_tLocalMtInfo = tLocalMtInfo; } 
    TConfMtInfo  GetLocalMtInfo( void ) const { return m_tLocalMtInfo; }

	BOOL AddMt(const u8 byMtId);//����������һ�������ն�  
	BOOL AddJoinedMt(const u8 byMtId);//����������һ������նˣ��ú����Զ������ն�Ҳ
	BOOL AddJoinedMt(const TMt& tMt);//����������һ������նˣ��ú����Զ������ն�Ҳ
	
	void RemoveMt(const u8 byMtId );//������ɾ��һ���ն� 
	void RemoveJoinedMt(const u8 byMtId);//������ɾ��һ������ն�
	void RemoveJoinedMt( const TMt&  tMt );//������ɾ��һ������ն�

	void RemoveAllJoinedMt( void );//������ɾ��һ��MCU����������նˣ���ɾ��MCU��

	BOOL MtInConf( const u8 byMtId ) const;//�ж��ն��Ƿ��������ն��б���
	BOOL MtInConf( const u16 wMcuIdx, const u8 byMtId ) const;//�ж��ն��Ƿ��������ն��б���

	// [12/30/2010 liuxu][�߶�] MtJoinedConf�Ľӿ��е�ֻ࣬��Ҫpublicһ���͹��ˣ��������
	BOOL MtJoinedConf( const u16  wMcuIdx, const u8  byMtId ) const;//�ж��ն��Ƿ�������ն��б���
	BOOL MtJoinedConf( const TMt& tMt) const;	//�ɷ�ӳ����mcu�µ�mt�Ƿ���� // [1/18/2010 xliang] 
	BOOL MtJoinedConf( const u8  byMtId ) const;//�ж��ն��Ƿ�������ն��б���
	
	u8   GetLocalMtNum( void ) const;//�õ��������������ն˸��� 
	u8   GetLocalJoinedMtNum( void ) const;//�õ�������������ն˸���
	u8   GetLocalUnjoinedMtNum( void ) const;//�õ���������δ����ն˸���

    void   SetMtInfo(const TConfMtInfo& tConfMtInfo );
    TConfMtInfo  GetMtInfo( const u16 wMcuIdx ) const;
	TConfMtInfo* GetMtInfoPtr(const u16 byMcuIdx);

	u16   GetAllMtNum( void ) const;//�õ����������ṹ�����������ն˸��� 
	u32   GetAllJoinedMtNum( void ) const;//�õ����������ṹ����������ն˸���
	u8	GetMaxMtIdInConf( void )const;// xliang [12/24/2008]

	u16	GetCascadeMcuNum( void) const;

	BOOL AddMcuInfo( const u16 wMcuIdx, const u8 m_byConfIdx );   //����������һ��mcu��Ϣ��������ʱû���ն�
	BOOL RemoveMcuInfo( const u16 wMcuIdx, const u8 m_byConfIdx );//�������Ƴ�һ��mcu��Ϣ��ͬʱ������������ն���Ϣ

	void Print()
	{
		StaticLog("Local Mt online.%d And MtId is: \n", m_tLocalMtInfo.GetAllJoinedMtNum());
		for (u16 wMcuIdx = 0;wMcuIdx < MAXNUM_CONFSUB_MCU;wMcuIdx++)
		{
			if ( !m_atOtherMcMtInfo[wMcuIdx].IsNull())
			{
				StaticLog("SMcu Mt online.%d\n", m_atOtherMcMtInfo[wMcuIdx].GetAllJoinedMtNum());
			}
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//�������ǽ���ýṹ��ÿ���ṹ��ӳ��һ���ն˺Ż�MCU����ź���Ƶ����˿ںŵĶ�Ӧ��ϵ
struct TTVWallSetting
{
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//�����߼�ͨ���ṹ(len:47)
struct TLogicalChannel
{  
public:
	u8  m_byMediaType;      //ý�����ͣ�MODE_VIDEO��MODE_AUDIO
	u8  m_byChannelType;    //�ŵ����ͣ�AUDIO_G711A��VIDEO_H261��
	u8  m_byVideoFormat;    //��Ƶ�ŵ��ķֱ���
	u8  m_byH239DStream;    //�ŵ������Ƿ���H239�غ�
	u16 m_wFlowControl;     //�ŵ�����(��λ:Kbps,1K=1024)
    u16 m_wCurrFlowControl; //��ǰ�ŵ�����ֵ
	TTransportAddr m_tRcvMediaChannel;     //���ն�ý���ŵ�
	TTransportAddr m_tRcvMediaCtrlChannel; //���ն�ý������ŵ�
	TTransportAddr m_tSndMediaCtrlChannel; //���Ͷ�ý������ŵ�

	TMediaEncrypt  m_tMediaEncrypt;   //������Ϣ
	u8             m_byActivePayload; //��̬�غ�
	u8			   m_byFECType;		  //MCUǰ���������, zgc, 2007-10-10
    u8             m_byVidFPS;        //��ͨ��������֡�� MODE_VIDEO��
private:
	//emProfileAttrb m_emHPAttrb;	  //[12/9/2011 chendaiwei]HP/BP����
	u8             m_byExInfo;        //��һλ��ʾ��ƵHP/BP���ԣ�ʣ��7λ��ʾ��Ƶ������
public:
	TLogicalChannel(void)
	{ 
		Clear();
	}
    void Clear(void)
    {
        memset( this, 0, sizeof(TLogicalChannel));
		m_tMediaEncrypt.Reset();
		m_byChannelType = MEDIA_TYPE_NULL;
        m_wCurrFlowControl = 0xFFFF;
		m_byExInfo = 0;
    }
    void SetMediaType(u8 byMediaType){ m_byMediaType = byMediaType;} 
    u8   GetMediaType( void ) const { return m_byMediaType; }
    void SetChannelType(u8 byChannelType){ m_byChannelType = byChannelType;} 
    u8   GetChannelType( void ) const { return m_byChannelType; }
    void SetSupportH239(BOOL32 bH239DStream){ m_byH239DStream = (bH239DStream ? 1:0);} 
    BOOL32   IsSupportH239( void ) const { return (m_byH239DStream == 0 ? FALSE : TRUE); }
    void SetVideoFormat(u8 byVideoFormat){ m_byVideoFormat = byVideoFormat;} 
    u8   GetVideoFormat( void ) const { return m_byVideoFormat; }
    void SetFlowControl(u16  wFlowControl){ m_wFlowControl = htons(wFlowControl);} 
    u16  GetFlowControl( void ) const { return ntohs(m_wFlowControl); }
    void SetCurrFlowControl(u16  wFlowControl){ m_wCurrFlowControl = htons(wFlowControl);} 
    u16  GetCurrFlowControl( void ) const { return ntohs(m_wCurrFlowControl); }
    void SetRcvMediaChannel(TTransportAddr tRcvMediaChannel){ m_tRcvMediaChannel = tRcvMediaChannel;} 
    TTransportAddr  GetRcvMediaChannel( void ) const { return m_tRcvMediaChannel; }
    void SetRcvMediaCtrlChannel(TTransportAddr tRcvMediaCtrlChannel){ m_tRcvMediaCtrlChannel = tRcvMediaCtrlChannel;} 
    TTransportAddr  GetRcvMediaCtrlChannel( void ) const { return m_tRcvMediaCtrlChannel; }
    void SetSndMediaCtrlChannel(TTransportAddr tSndMediaCtrlChannel){ m_tSndMediaCtrlChannel = tSndMediaCtrlChannel;} 
    TTransportAddr  GetSndMediaCtrlChannel( void ) const { return m_tSndMediaCtrlChannel; }
    BOOL IsOpened( void ) const { return m_byMediaType == 0 ? FALSE : TRUE; }

	
	void SetRcvMediaChnnelIp(u32 dwIp) 
	{
		m_tRcvMediaChannel.SetIpAddr(dwIp);
		m_tRcvMediaCtrlChannel.SetIpAddr(dwIp);
	}

	void SetMediaEncrypt(TMediaEncrypt& tMediaEncrypt)
	{
		m_tMediaEncrypt = tMediaEncrypt;
	}
	TMediaEncrypt &GetMediaEncrypt()
	{
		return m_tMediaEncrypt;
	}

	void SetActivePayload(u8 byActivePayload)
	{
		m_byActivePayload = byActivePayload;
	}
	u8 GetActivePayload()
	{
		return m_byActivePayload;
	}

	void SetFECType( u8 byFECType ) { m_byFECType = byFECType; }
	u8	 GetFECType( void ) const { return m_byFECType; }
    void SetChanVidFPS( u8 byFPS ) { m_byVidFPS = byFPS;    }
    u8   GetChanVidFPS( void ) const { return m_byVidFPS;    }

	// ����/��ȡHP/BP���Խӿ� [12/9/2011 chendaiwei]
	void SetProfieAttrb(emProfileAttrb emProf)
	{
		if (emProf == emHpAttrb)
		{
			m_byExInfo |= 0x01;
		}
		else
		{
			m_byExInfo &= 0xFE;
		}
	}
	emProfileAttrb GetProfileAttrb( void )
	{
		return (emProfileAttrb)(m_byExInfo & 0x01);
	}
	void SetAudioTrackNum(u8 byAudioTrackNum)
	{
		m_byExInfo = ((m_byExInfo & 0x01) | (byAudioTrackNum << 1));
	}
	u8 GetAudioTrackNum( void ){ return ((m_byExInfo & 0xFE) >> 1); }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//H239˫������ӵ���ն���Ϣ (len:15)
typedef struct tagH239TokenInfo
{
    s32  m_nChannelId;   
    
    u8   m_bySymmetryBreaking;
	BOOL m_bIsResponseAck;

	//H239˫������ӵ���ն�
	TMt  m_tH239TokenMt;   
    
    tagH239TokenInfo()
    {
        Clear();
    }

    void Clear()
    {
        m_nChannelId         = 0;
        m_bySymmetryBreaking = 0;
		m_bIsResponseAck     = FALSE;
        m_tH239TokenMt.SetNull();
    }

    void SetSymmetryBreaking(u8 bySymmetryBreaking)
    {
        m_bySymmetryBreaking = bySymmetryBreaking;
    }
    u8 GetSymmetryBreaking()
    {
        return m_bySymmetryBreaking;
    }

	void SetResponseAck(BOOL bIsResponseAck)
    {
        m_bIsResponseAck = bIsResponseAck;
    }
    BOOL IsResponseAck()
    {
        return m_bIsResponseAck;
    }

    void SetChannelId(s32 nChannelId)
    {
        m_nChannelId = nChannelId;
    }   
    s32 GetChannelId()
    {
        return m_nChannelId;
    }

	void SetTokenMt(TMt tH239TokenMt)
    {
        m_tH239TokenMt = tH239TokenMt;
    }   
    TMt GetTokenMt()
    {
        return m_tH239TokenMt;
    }
    
}TH239TokenInfo,*PTH239TokenInfo;
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif


//���彻���ṹ
struct TSwitchInfo
{
protected:
	TMt	  m_tDstMt;		  //����Ŀ���ն�
	TMt   m_tSrcMt;		  //����Դ�ն�
	u8    m_byMode;		  //����ģʽ��MODE_VIDEO, MODE_AUDIO, MODE_BOTH
	u8    m_byDstChlIdx;  //Ŀ��ͨ������
	u8    m_bySrcChlIdx;  //Դͨ��������
public:
    void  SetDstMt(TMt tDstMt){ m_tDstMt = tDstMt;} 
    TMt   GetDstMt( void ) const { return m_tDstMt; }
    void  SetSrcMt(TMt tSrcMt){ m_tSrcMt = tSrcMt;} 
    TMt   GetSrcMt( void ) const { return m_tSrcMt; }
    void  SetMode(u8   byMode){ m_byMode = byMode;} 
    u8    GetMode( void ) const { return m_byMode; }
    void  SetDstChlIdx(u8   byDstChlIdx){ m_byDstChlIdx = byDstChlIdx;} 
    u8    GetDstChlIdx( void ) const { return m_byDstChlIdx; }
    void  SetSrcChlIdx(u8   bySrcChlIdx){ m_bySrcChlIdx = bySrcChlIdx;} 
    u8    GetSrcChlIdx( void ) const { return m_bySrcChlIdx; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//�������ǽ�����ṹ
struct TTWSwitchInfo : public TSwitchInfo
{
protected:
	u8    m_bySrcMtType;  //����ǽԴͨ����Ա����,�μ�mcuconst.h�е���ǽ��Ա���Ͷ���
	u8    m_bySchemeIdx;  // Ԥ��������
	u8    m_byDstSubChn;
	u8    m_byReserved[2];
public:
    TTWSwitchInfo()
	{
		memset( this, 0x0, sizeof(TTWSwitchInfo) );
	}

	void  SetMemberType(u8   bySrcMtType ){ m_bySrcMtType = bySrcMtType; } 
    u8    GetMemberType( void ) const { return m_bySrcMtType; }

	void  SetSchemeIdx(u8   bySchemeIdx ){ m_bySchemeIdx = bySchemeIdx; } 
    u8    GetSchemeIdx( void ) const { return m_bySchemeIdx; }
	void  SetDstSubChn(u8  bySubChn){ m_byDstSubChn = bySubChn;}
	u8    GetDstSubChn( void ) const { return m_byDstSubChn; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//����HDU�����ṹ
typedef TTWSwitchInfo THduSwitchInfo;   //4.6.1 �¼�  jlb

//����hdu�����ṹ��  ��Χ 0 -- 31
struct THduVolumeInfo:public TEqp
{
protected:
	u8  m_byChnlIdx;    // ͨ��������
	u8  m_byVolume;     // ������С
	u8  m_byIsMute;  // �Ƿ���  
	u8  m_byReserved[5];

public:
    THduVolumeInfo()
	{
        memset(this, 0x0, sizeof(THduVolumeInfo));
		m_byVolume = HDU_VOLUME_DEFAULT;
	}

	u8   GetChnlIdx( void ){ return m_byChnlIdx; }
	void SetChnlIdx( u8 byChnlIdx ){ m_byChnlIdx = byChnlIdx; }

	u8   GetVolume( void ){ return m_byVolume; }
	void SetVolume( u8 byVolume ){ m_byVolume = byVolume; }

	BOOL32   GetIsMute( void ){ return m_byIsMute; }
	void SetIsMute( BOOL32 byIsMute ){ m_byIsMute = GETBBYTE(byIsMute); }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//������󲻶Գƽṹ
struct TMaxSkew
{
	u8    byChan1;	//�ŵ�1��MODE_VIDEO, MODE_AUDIO
	u8    byChan2;	//�ŵ�2��MODE_VIDEO, MODE_AUDIO
protected:
	u16 	wMaxSkew;	//��󲻶Գ�ֵ���ŵ�2���ŵ�1���ӳٴ���������λ��ms��

public:
	u16  GetMaxSkew( void ) const;//�õ���󲻶Գ�ֵ
	void SetMaxSkew( u16  wNewMaxSkew );//������󲻶Գ�ֵ
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//8000E����ע��������Ϣ��
struct TEqpRegReq : public TEqp
{
protected:
    u32   		m_dwIpAddr;		//����IP��ַ
    s8          m_achAlias[MAXLEN_EQP_ALIAS];
    u16         m_wVersion;     //�汾��Ϣ
    u32         m_dwReserve1;   //�����ֶ�1
    u32         m_dwReserve2;   //�����ֶ�2 
    
public:
    TEqpRegReq(void) { memset(this, 0, sizeof(TEqpRegReq)); }
    //��������IP��ַ
    void SetEqpIpAddr( const u32 &dwIpAddr ) { m_dwIpAddr = htonl(dwIpAddr); }
    //�������IP��ַ
    u32  GetEqpIpAddr(void) const { return ntohl(m_dwIpAddr); }
    
    void SetEqpAlias(const char * szAlias)
    {
        if ( NULL != szAlias )
        {
            strncpy( m_achAlias, szAlias ,sizeof(m_achAlias) );
            m_achAlias[MAXLEN_EQP_ALIAS-1] ='\0';
        }
        else
        {
            memset( m_achAlias, '\0', sizeof(m_achAlias) );
        }
        return;
    }
    
    s8 * GetEqpAlias(void) const
    {
        return (s8*)m_achAlias;
    }
    
    void SetVersion(const u16 &wVersion)
    {
        m_wVersion = htons(wVersion);
    }
    u16 GetVersion(void) const
    {
        return ntohs(m_wVersion);
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//8000E��������Ǽǳɹ���Ϣ�ṹ
struct TEqpRegAck
{
protected:
    u32 m_dwMcuIpAddr;	    //MCU���յ�ַ
    u16 m_wMcuStartPort;	//MCU������ʼ�˿ں�
    u16 m_wEqpStartPort;    //EQP������ʼ�˿ں�
    u8  m_byEqpId;          //����ID
#if defined(_8KH_)
    u8  m_byMemNum;          //8KH����������ͨ����Ŀ
    u8  m_byReserver1;       //�����ֶ�1
    u16 m_wReserver2;        //�����ֶ�2
    u32 m_dwReserver3;       //�����ֶ�3
#else
    u32 m_dwReserver1;       //�����ֶ�1
    u32 m_dwReserver2;        //�����ֶ�2
#endif
	
public:
    //�õ��ϼ�MCU������������IP��ַ
    u32  GetMcuIpAddr( void ) const { return ntohl(m_dwMcuIpAddr); }   
    //�����ϼ�MCU������������IP��ַ
    void SetMcuIpAddr( const u32 &dwIpAddr ) { m_dwMcuIpAddr = htonl(dwIpAddr); }
    
    //�õ�MCU�����������ݶ˿ں�
    u16  GetMcuStartPort( void ) const { return ntohs(m_wMcuStartPort); }   
    //����MCU�����������ݶ˿ں�
    void SetMcuStartPort( const u16 &wPort ) { m_wMcuStartPort = htons(wPort); }
    
    //�õ����轻���������ݶ˿ں�
    u16  GetEqpStartPort( void ) const { return ntohs(m_wEqpStartPort); }   
    //�������轻���������ݶ˿ں�
    void SetEqpStartPort( const u16 &wPort ) { m_wEqpStartPort = htons(wPort); }
    
    //�õ�����ID
    u8   GetEqpId( void ) const { return m_byEqpId; }
    //��������ID
    void SetEqpId( const u8 &byEqpId ) { m_byEqpId = byEqpId; }
#if defined(_8KH_)
    //�õ��û��������õ�·��
    u8   GetMemNum( void ) const { return m_byMemNum; }
    //���øû��������õ�·��
    void SetMemNum( const u8 &byMemNum ) { m_byMemNum = byMemNum; }
#endif
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//����ע��������Ϣ��
struct TPeriEqpRegReq : public TEqp
{
protected:
	u32   		m_dwIpAddr;		//����IP��ַ
	u16 		m_wStartPort;	//������ʼ���ն˿�
	u8  		m_byChannlNum;	//��������ŵ���
	s8          m_achAlias[MAXLEN_EQP_ALIAS];
    //4.0R3��չ�ֶ�
    u16         m_wVersion;     //�汾��Ϣ
    //4.5������չ�ֶ�
    BOOL        m_bHDEqp;       //�Ƿ��������

public:
    TPeriEqpRegReq(void) { memset(this, 0, sizeof(TPeriEqpRegReq)); }
    //��������IP��ַ
    //���룺IP��ַ(������)
	void SetPeriEqpIpAddr( const u32    &dwIpAddr )	{ m_dwIpAddr = dwIpAddr; }
    //�������IP��ַ
    //����ֵ��IP��ַ(������)
	u32    GetPeriEqpIpAddr() const	{ return m_dwIpAddr; }
	//�������������ʼ�˿�
	void SetStartPort( const u16  & wStartPort )	{ m_wStartPort = htons(wStartPort); }
	//������������ʼ�˿�
	u16  GetStartPort() const	{ return ntohs( m_wStartPort ); }
	//������������ŵ���Ŀ
	void SetChnnlNum( const u8   & byChannlNum )	{ m_byChannlNum = byChannlNum; }
	//�����������ŵ���Ŀ
	u8   GetChnnlNum() const	{ return m_byChannlNum; }

	void SetEqpAlias(char* szAlias)
	{
		strncpy( m_achAlias ,szAlias ,sizeof(m_achAlias) );
		m_achAlias[MAXLEN_EQP_ALIAS-1] ='\0';
	}

	LPCSTR GetEqpAlias()
	{
		return m_achAlias;
	}

    void SetVersion(u16 wVersion)
    {
        m_wVersion = htons(wVersion);
    }
    u16 GetVersion() const
    {
        return ntohs(m_wVersion);
    }
    void SetHDEqp(BOOL bHD)
    {
        m_bHDEqp = bHD;
    }

    BOOL IsHDEqp() const
    {
        return m_bHDEqp;
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//��������Ǽǳɹ���Ϣ�ṹ
struct TPeriEqpRegAck
{
protected:
	u32   		m_dwIpAddr;			//MCU���յ�ַ
	u16 		m_wStartPort;		//MCU������ʼ�˿ں�
    u32         m_dwAnotherMpcIp;   //����ʱ������һ��mpc���ַ���������������õı���mpc��ַ����У�飩
	u32         m_dwMsSSrc;         //����ʱ�����ڱ�עһ��ϵͳ���У�ֻҪ��ֵ�仯��˵������������ͬʱ����

public:
	//�õ��ϼ�MCU������������IP��ַ
    u32    GetMcuIpAddr( void ) const { return ntohl(m_dwIpAddr); }

	//�����ϼ�MCU������������IP��ַ
    void SetMcuIpAddr( u32    dwIpAddr ) { m_dwIpAddr = htonl(dwIpAddr); }

	//�õ��ϼ�MCU�����������ݶ˿ں�
    u16  GetMcuStartPort( void ) const { return ntohs(m_wStartPort); }

    //�����ϼ�MCU�����������ݶ˿ں�
    void SetMcuStartPort( u16  wPort ) { m_wStartPort = htons(wPort); }

    //����һ��mpc���ַ
    u32  GetAnotherMpcIp( void ) const { return ntohl(m_dwAnotherMpcIp); }
    void SetAnotherMpcIp( u32 dwIpAddr ) { m_dwAnotherMpcIp = htonl(dwIpAddr); }

	void SetMSSsrc(u32 dwSSrc)
    {
        m_dwMsSSrc = htonl(dwSSrc);
    }
    u32 GetMSSsrc(void) const
    {
        return ntohl(m_dwMsSSrc);
    }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//�������̨ע����Ϣ(len:77)
struct TMcsRegInfo
{
public:
	u32   		m_dwMcsIpAddr;	      //�������̨��Ip��ַ��������
	u8  		m_byVideoChnNum;	  //ͨ����Ŀ
	u16 		m_wStartPort;		  //��Ƶͨ������ʼ�˿ںţ�������
	s8          m_achUser[MAXLEN_PWD];//�û���
	s8          m_achPwd[MAXLEN_PWD]; //�û�����
	u32   		m_dwMcsSSRC;	      //�������̨���������ֵ����������
	                                  //Ӧ����ͬһmcs����ͬһmcu������mpc����ͬһֵ��
	                                  //      ��ͬmcs����ͬһmcu���ò�ֵͬ
    u16         m_wMcuMcsVer;         //mcu��mcs�Ự�汾
public:
	TMcsRegInfo( void )
    { 
        memset( this, 0, sizeof(TMcsRegInfo) );
        SetMcuMcsVer();
    }
    void  SetMcsIpAddr(u32 dwMcsIpAddr){ m_dwMcsIpAddr = htonl(dwMcsIpAddr);}
    u32   GetMcsIpAddr( void ) const { return ntohl(m_dwMcsIpAddr); }
    void  SetVideoChnNum(u8 byVideoChnNum){ m_byVideoChnNum = byVideoChnNum;}
    u8    GetVideoChnNum( void ) const { return m_byVideoChnNum; }
    void  SetStartPort(u16 wStartPort){ m_wStartPort = htons(wStartPort);}
    u16   GetStartPort( void ) const { return ntohs(m_wStartPort); }
    void  SetMcsSSRC(u32 dwMcsSSRC){ m_dwMcsSSRC = htonl(dwMcsSSRC);} 
    u32   GetMcsSSRC( void ) const { return ntohl(m_dwMcsSSRC); }
    void  SetMcuMcsVer(void) { m_wMcuMcsVer = htons(MCUMCS_VER); }
    u16   GetMcuMcsVer(void) const { return ntohs(m_wMcuMcsVer); }
    void  SetUser( LPCSTR lpszUser )
	{
		memset(m_achUser, 0, sizeof(m_achUser));
		if (NULL != lpszUser)
		{
			strncpy(m_achUser, lpszUser, sizeof(m_achUser));
			m_achUser[sizeof(m_achUser)-1] = '\0';
		}
	}
	LPCSTR GetUser( void ) const{ return m_achUser; }
	BOOL IsEqualUser( LPCSTR lpszUser ) const
	{
		s8 achBuf[MAXLEN_PWD+1] = {0};
		strncpy( achBuf, lpszUser, sizeof( achBuf ) );
		achBuf[sizeof(achBuf)-1] = '\0';
		
		if(0 == strncmp(achBuf, lpszUser, MAXLEN_PWD))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	void SetPwd( LPCSTR lpszPwd )
	{
		memset(m_achPwd, 0, sizeof(m_achPwd));
		if (NULL != lpszPwd)
		{
			strncpy(m_achPwd, lpszPwd, sizeof(m_achPwd));
			m_achPwd[sizeof(m_achPwd)-1] = '\0';
            // guzh [10/11/2007] �򵥼��ܡ���GetPwd֮ǰҪ�ֶ�����
            EncPwd();
		}
	}

	LPCSTR GetPwd( void ) const{ return m_achPwd; }
    
    // guzh [10/11/2007] 
    /*
	BOOL IsEqualPwd( LPCSTR lpszPwd ) const
	{
		s8 achBuf[MAXLEN_PWD+1] = {0};
		strncpy( achBuf, lpszPwd, sizeof( achBuf ) );
		achBuf[sizeof(achBuf)-1] = '\0';
		
		if(0 == strncmp(achBuf, lpszPwd, MAXLEN_PWD))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
    */
	
	BOOL operator ==( TMcsRegInfo &tObj ) const
	{
		if (tObj.m_dwMcsIpAddr == m_dwMcsIpAddr && 
			tObj.m_dwMcsSSRC == m_dwMcsSSRC && 
			tObj.m_byVideoChnNum == m_byVideoChnNum && 
			IsEqualUser(tObj.m_achUser) && 
			/*IsEqualPwd(tObj.m_achPwd)&&*/ 
            tObj.m_wMcuMcsVer == m_wMcuMcsVer)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
        
    void DecPwd()
    {
        EncPwd();
    }
private:
    void EncPwd()
    {
        for (u8 byLoop=0; byLoop<strlen(m_achPwd); byLoop++)
        {
            m_achPwd[byLoop] ^= 0xa7;
        }
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//�������̨ע���Ӧ(len:157)
struct TMcsRegRsp
{
protected:
    TMcu                m_tMcu;             // MCU �ṹ
    TMtAlias            m_tMcuAlias;        // MCU ����
    u32                 m_dwRemoteMpcIp;    // ����ʱ�Զ˵�IP
    u8                  m_byLocalActive;    // ��ǰ��MPC�Ƿ�Ϊ���ð�
    u8                  m_byUsrGrpId;       // ��½�û�����ID
    u8                  m_byMcuOsType;      // MCU�Ĳ���ϵͳ����
	u32					m_dwSysSSrc;		// ������ʶ
public:
    void   SetMcu(u8 byMcuType, u8 byMcuId)
    {
        m_tMcu.SetMcu(byMcuId);
        m_tMcu.SetMcuType(byMcuType);
    }
    TMcu   GetMcu() const
    {
        return m_tMcu;
    }
    void   SetMcuAlias(const TMtAlias &tAlias)
    {
        m_tMcuAlias = tAlias;
    }
    TMtAlias GetMcuAlias() const
    {
        return m_tMcuAlias;
    }
    void   SetRemoteMpcIp(u32 dwIp)
    {
        m_dwRemoteMpcIp = htonl(dwIp);
    }
    u32    GetRemoteMpcIp() const
    {
        return ntohl(m_dwRemoteMpcIp);
    }
    void   SetLocalActive(BOOL32 bActive)
    {
        m_byLocalActive = GETBBYTE(bActive);
    }
    BOOL32 IsLocalActive() const
    {
        return ISTRUE(m_byLocalActive);
    }
    void   SetUsrGrpId(u8 byGrpId)
    {
        m_byUsrGrpId = byGrpId;
    }
    u8     GetUsrGrpId() const
    {
        return m_byUsrGrpId;
    }
    void   SetMcuOsType(u8 byOsType)
    {
        m_byMcuOsType = byOsType;
    }
    u8     GetMcuOsType() const
    {
        return m_byMcuOsType;
    }
	void   SetSysSSrc(u32 dwSysSSrc)
	{
		m_dwSysSSrc = htonl(dwSysSSrc);
	}
	u32    GetSysSSrc() const
	{
		return ntohl(m_dwSysSSrc);
	}

    void Print() const
    {
        StaticLog( "Mcu: (%d, %d)\n", m_tMcu.GetMcuId(), m_tMcu.GetMcuType());
        StaticLog( "Alias: %s, 0x%x\n", m_tMcuAlias.m_achAlias, m_tMcuAlias.m_tTransportAddr.GetIpAddr());
        StaticLog( "Remote MpcIP: 0x%x\n", GetRemoteMpcIp());
        StaticLog( "Local Active: %d\n", IsLocalActive());
        StaticLog( "User Group Id: %d\n", GetUsrGrpId());
        StaticLog( "OS Type: %d\n", GetMcuOsType());
		StaticLog( "Local SSRC: %d\n", GetSysSSrc());
    }
    
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//�������Ϣ
struct TConfNameInfo
{
	CConfId		m_cConfId;          //�����
	char		achConfName[MAXLEN_CONFNAME];   //������
public:
	TConfNameInfo( void )
	{
		m_cConfId.SetNull();
		memset( achConfName, 0, sizeof( achConfName ) );
	};
	void   SetConfName( LPCSTR lpszConfName )
	{
		if( lpszConfName != NULL )
		{
			strncpy( achConfName, lpszConfName, MAXLEN_CONFNAME );
		    achConfName[MAXLEN_CONFNAME - 1] = '\0';
		}
		else
		{
			memset(achConfName,0,sizeof(achConfName));
		}
	}
	LPCSTR GetConfName( void ) const { return achConfName; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;



//���彻��ͨ���ṹ SrcIp->DisIp->RcvIp->DstIp, size=34
struct TSwitchChannel
{
    TSwitchChannel()
    {
        memset(this, 0, sizeof(TSwitchChannel));
    }
protected:
	TMt  m_tSrcMt;         //����Դ�ն�
	u32  m_dwSrcIp;        //����ԴIp��ַ
	u32  m_dwDisIp;        //���ݷַ�Ip��ַ
	u32  m_dwRcvIP;        //���ؽ��յ�IP��ַ(������),�������鲥(��㲥)��ַ
	u16  m_wRcvPort;       //���ؽ��յĶ˿� (������)
	u32  m_dwRcvBindIP;    //���ؽ���bind IP��ַ(������),���ص�ַ
	u32  m_dwDstIP;        //��Ҫ���͵�Ŀ��IP��ַ(������),�������鲥(��㲥)��ַ
	u16  m_wDstPort;       //��Ҫ���͵�Ŀ��˿� (������)
	u32  m_dwSndBindIP;    //����Ŀ��ӿ�IP��ַ.(������),���ص�ַ

public:
	void SetSrcMt( TMt tMt ){ m_tSrcMt = tMt; }
	TMt  GetSrcMt( void ){ return m_tSrcMt; }
    void SetSrcIp(u32    dwSrcIp){ m_dwSrcIp = htonl(dwSrcIp);} 
    u32  GetSrcIp( void ) const { return ntohl(m_dwSrcIp); }
    void SetDisIp(u32    dwDisIp){ m_dwDisIp = htonl(dwDisIp);} 
    u32  GetDisIp( void ) const { return ntohl(m_dwDisIp); }
    void SetRcvIP(u32    dwRcvIP){ m_dwRcvIP = htonl(dwRcvIP);} 
    u32  GetRcvIP( void ) const { return ntohl(m_dwRcvIP); }
    void SetRcvPort(u16  wRcvPort){ m_wRcvPort = htons(wRcvPort);} 
    u16  GetRcvPort( void ) const { return ntohs(m_wRcvPort); }
    void SetRcvBindIP(u32    dwRcvBindIP){ m_dwRcvBindIP = htonl(dwRcvBindIP);} 
    u32  GetRcvBindIP( void ) const { return ntohl(m_dwRcvBindIP); }
    void SetDstIP(u32    dwDstIP){ m_dwDstIP = htonl(dwDstIP);} 
    u32  GetDstIP( void ) const { return ntohl(m_dwDstIP); }
    void SetDstPort(u16  wDstPort){ m_wDstPort = htons(wDstPort);} 
    u16  GetDstPort( void ) const { return ntohs(m_wDstPort); }
    void SetSndBindIP(u32    dwSndBindIP){ m_dwSndBindIP = htonl(dwSndBindIP);} 
    u32  GetSndBindIP( void ) const { return ntohl(m_dwSndBindIP); }
	BOOL IsNull( void ) const { return m_dwDstIP == 0 ? TRUE : FALSE; }
    BOOL IsSrcNull( void ) const { return m_dwSrcIp == 0 ? TRUE : FALSE; }
    BOOL IsRcvNull( void ) const { return m_dwRcvIP == 0 ? TRUE : FALSE; }
	void SetNull( ){ memset( this, 0, sizeof(TSwitchChannel) ); }
	BOOL operator ==( TSwitchChannel &tObj ) const
	{
		if( tObj.GetDstIP() == GetDstIP() && tObj.GetDstPort() == GetDstPort() )
		{
			return TRUE;
		}
        else
		{
			return FALSE;
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//Mp��Ϣ
struct TMp
{
protected:
    u8   m_byMcuId;                   //MCU��
    u8   m_byMpId;                    //��Mp���
    u8   m_byAttachMode;              //MP�ĸ�����ʽ
    u32  m_dwIpAddr;                  //Ip��ַ
    u8   m_byDoubleLink;              //˫��
    u8   m_byIsMulticast;             //�Ƿ��鲥 0: ���鲥 1���鲥
    s8   m_abyMpAlias[MAXLEN_ALIAS];  //Mp����	
	//4.0R4��չ�ֶ�
    u16  m_wVersion;			     //�汾��Ϣ	ZGC	2007-10-10
public:
    void SetMcuId(u8 byMcuId) { m_byMcuId = byMcuId; }
    u8   GetMcuId(void) const { return m_byMcuId; }
    void SetMpId(u8 byMpId){ m_byMpId = byMpId; }
    u8   GetMpId(void) const { return m_byMpId; }
    void SetAttachMode(u8 byAttachMode) { m_byAttachMode = byAttachMode; }
    u8   GetAttachMode(void) const { return m_byAttachMode; }
    void SetIpAddr(u32 dwIP){ m_dwIpAddr = htonl(dwIP); }
    u32  GetIpAddr(void) { return ntohl(m_dwIpAddr); }
    void SetDoubleLink(u8 byDoubleLink) { m_byDoubleLink = byDoubleLink; }
    u8   GetDoubleLink(void) const { return m_byDoubleLink; }
    void SetMulticast(u8 byIsMulticast) { m_byIsMulticast = byIsMulticast; }
    u8   GetMulticast(void) const { return m_byIsMulticast; }
    const s8 * GetAlias(void) const { return m_abyMpAlias; }
    void SetAlias(s8 * pchAlias)
    {
        if (pchAlias != NULL)
        {
            strncpy(m_abyMpAlias, pchAlias, sizeof(m_abyMpAlias));
            m_abyMpAlias[sizeof(m_abyMpAlias) - 1] = '\0';
        }
        else
        {
            memset(m_abyMpAlias, 0, sizeof(m_abyMpAlias));
        }
    }
	u16 GetVersion(void) const { return ntohs(m_wVersion); }
	void SetVersion(u16 wVersion) { m_wVersion = htons(wVersion); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//Э�������������Ϣ(h323)
struct TMtAdpCfg 
{
protected:
    u16 m_wPartlistRefreshTime;             //����ʱ�ն��б�ˢ��ʱ����(s)
    u16 m_wAudInfoRefreshTime;              //����ʱ��Ƶ��Ϣˢ��ʱ����(s)
    u16 m_wVidInfoRefreshTime;              //����ʱ��Ƶ��Ϣˢ��ʱ����(s)

public:
    TMtAdpCfg(void) 
    {             
        SetPartListRefreshTime(PARTLIST_REFRESHTIME);  
        SetAudInfoRefreshTime(AUDINFO_REFRESHTIME);
        SetVidInfoRefreshTime(VIDINFO_REFRESHTIME);
    }    
    
    void    SetPartListRefreshTime(u16 wTime) { m_wPartlistRefreshTime = htons(wTime); }
    u16     GetPartListRefreshTime(void) { return ntohs(m_wPartlistRefreshTime); }
    void    SetAudInfoRefreshTime(u16 wTime) { m_wAudInfoRefreshTime = htons(wTime); }
    u16     GetAudInfoRefreshTime(void) { return ntohs(m_wAudInfoRefreshTime); }
    void    SetVidInfoRefreshTime(u16 wTime) { m_wVidInfoRefreshTime = htons(wTime); }
    u16     GetVidInfoRefreshTime(void) { return ntohs(m_wVidInfoRefreshTime); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct  TCapExtraSet
{
protected:
    TVideoStreamCap m_tVideoCap;     //��һ·�������
    TAudioStreamCap  m_tAudioCap;
    TDStreamCap   m_tDStreamCap;

public:

    TCapExtraSet(void){ Clear(); }
    void Clear(void)
    {
        m_tVideoCap.Clear();
        m_tAudioCap.Clear();
        m_tDStreamCap.Reset();
    }

    BOOL32 IsEmpty(void) const
    {
        if (MEDIA_TYPE_NULL == m_tVideoCap.GetMediaType() &&
            MEDIA_TYPE_NULL == m_tAudioCap.GetMediaType() &&
            MEDIA_TYPE_NULL == m_tDStreamCap.GetMediaType())
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }       
    }

    //��Ƶ
    void SetVideoStreamCap(const TVideoStreamCap &tVideoCap)
    {
        memcpy(&m_tVideoCap, &tVideoCap, sizeof(TVideoStreamCap));
    }
    TVideoStreamCap GetVideoStreamCap(void) const
    {
        return m_tVideoCap;
    }

    //��Ƶ
    void SetAudioStreamCap(const TAudioStreamCap &tAudioCap)
    {
        memcpy(&m_tAudioCap, &tAudioCap, sizeof(TAudioStreamCap));
    }
    TAudioStreamCap GetAudioStreamCap(void) const
    {
        return m_tAudioCap;
    }

    //˫��
    void SetDStreamCap(const TDStreamCap &tDStreamCap)
    {
        memcpy(&m_tDStreamCap, &tDStreamCap, sizeof(TDStreamCap));
    }
    TDStreamCap GetDStreamCap(void) const
    {
        return m_tDStreamCap;
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

#define MAX_CAPEXTRASET_SIZE 1
//������������
struct TMultiCapSupport : public TCapSupport
{
protected:
    TCapExtraSet m_tCapExtraSet[MAX_CAPEXTRASET_SIZE];
    TDStreamCap m_tSecComDStream;
	u8 m_byTrackNum;

public:

    TMultiCapSupport(void){ Clear(); }

    void Clear(void)
    {
        TCapSupport::Clear();
        for (u8 byCapIndx = 0; byCapIndx < MAX_CAPEXTRASET_SIZE;  byCapIndx++)
        {
            m_tCapExtraSet[byCapIndx].Clear();
        }
        m_tSecComDStream.Reset();
		m_byTrackNum = 1;
    }

    //ĳһ���������
    BOOL32 SetCapExtraSet(const TCapExtraSet &tExtraSet, u8 bySetIndx)
    {
        if ( bySetIndx >= MAX_CAPEXTRASET_SIZE)
        {
            return FALSE;
        }

        memcpy(&m_tCapExtraSet[bySetIndx], &tExtraSet, sizeof(TCapExtraSet));

        return TRUE;

    }
    BOOL32 GetCapExtraSet(TCapExtraSet &tCapExtraSet, u8 bySetIndx) const
    {
        if( bySetIndx >= MAX_CAPEXTRASET_SIZE ||
            m_tCapExtraSet[bySetIndx].IsEmpty())
        {
            return FALSE;
        }
        memcpy(&tCapExtraSet, &m_tCapExtraSet[bySetIndx], sizeof(TCapExtraSet));

        return TRUE;
    }

    //��Ƶ
    BOOL32 SetExtraVideoCap(const TVideoStreamCap &tExtraVideo, u8 bySetIndx)
    {
        if (MAX_CAPEXTRASET_SIZE <= bySetIndx)
        {
            return FALSE;
        }
        
        m_tCapExtraSet[bySetIndx].SetVideoStreamCap(tExtraVideo);

        return TRUE;

    }
    BOOL32 GetExtraVideoCap(TVideoStreamCap &tExtraVideo, u8 bySetIndx) const
    {
        if ( bySetIndx >= MAX_CAPEXTRASET_SIZE )
        {
            return FALSE;
        }
        tExtraVideo = m_tCapExtraSet[bySetIndx].GetVideoStreamCap();

        return TRUE;
    }

    //��Ƶ
    BOOL32 SetExtraAudioCap(const TAudioStreamCap &tExtraAudio, u8 bySetIndx)
    {
        if (MAX_CAPEXTRASET_SIZE <= bySetIndx)
        {
            return FALSE;
        }
        
        m_tCapExtraSet[bySetIndx].SetAudioStreamCap(tExtraAudio);

        return TRUE;
        
    }
    BOOL32 GetExtraAudioCap(TAudioStreamCap &tExtraAudio, u8 bySetIndx) const
    {
        if (bySetIndx >= MAX_CAPEXTRASET_SIZE)
        {
            return FALSE;
        }

        tExtraAudio = m_tCapExtraSet[bySetIndx].GetAudioStreamCap();

        return TRUE;
    }

    //˫��
    BOOL32 SetExtraDStreamCap(const TDStreamCap &tExtraDStream, u8 bySetIndx)
    {
        if (MAX_CAPEXTRASET_SIZE <= bySetIndx)
        {
            return FALSE;
        }

        m_tCapExtraSet[bySetIndx].SetDStreamCap(tExtraDStream);

        return TRUE;
        
    }
    BOOL32 GetExtraDStreamCap(TDStreamCap &tExtraDStream, u8 bySetIndx) const
    {
        if (bySetIndx >= MAX_CAPEXTRASET_SIZE ||
            m_tCapExtraSet[bySetIndx].IsEmpty())
        {
            return FALSE;
        }

        tExtraDStream = m_tCapExtraSet[bySetIndx].GetDStreamCap();

        return TRUE;

    }

    //���õڶ���ͬ˫������
    TDStreamCap GetSecComDStreamCapSet( void ) const { return m_tSecComDStream; } 
	void        SetSecComDStreamCapSet( TDStreamCap &tDSCap ) { m_tSecComDStream = tDSCap; }

	void SetMainAudioTrackNum(const u8 byTrackNum) { m_byTrackNum = byTrackNum; }
	u8 GetMainAudioTrackNum( void ) const { return m_byTrackNum; }

    void Print(void) const
    {
        TCapSupport::Print();
        StaticLog( "Extra CapSet Support:\n");
        for (u8 byIndx = 0; byIndx < MAX_CAPEXTRASET_SIZE; byIndx++)
        {
            StaticLog( "\tExtraSet[%d]:", byIndx);
            if(m_tCapExtraSet[byIndx].IsEmpty())
                StaticLog( "NULL\n");
            else
                StaticLog( "\tType:%d\n\tRes:%d\n\tFps:%d\n\tHP:%d\n\taudtype:%d\n",				
                  m_tCapExtraSet[byIndx].GetVideoStreamCap().GetMediaType(),
                  m_tCapExtraSet[byIndx].GetVideoStreamCap().GetResolution(),
                  MEDIA_TYPE_H264 != m_tCapExtraSet[byIndx].GetVideoStreamCap().GetMediaType() ? m_tCapExtraSet[byIndx].GetVideoStreamCap().GetFrameRate() : m_tCapExtraSet[byIndx].GetVideoStreamCap().GetUserDefFrameRate(),
				  m_tCapExtraSet[byIndx].GetVideoStreamCap().IsSupportHP(),
				  m_tCapExtraSet[byIndx].GetAudioStreamCap().GetMediaType()
                   );

        }
        StaticLog( "Second Common DoubleStream:\n");
        StaticLog( "\tType:%d\n\tRes:%d\n\tFps:%d\n\tH239:%d\n\tHP:%d\n",
                  m_tSecComDStream.GetMediaType(),
                  m_tSecComDStream.GetResolution(),
                  MEDIA_TYPE_H264 != m_tSecComDStream.GetMediaType() ? m_tSecComDStream.GetFrameRate() : m_tSecComDStream.GetUserDefFrameRate(),
                  m_tSecComDStream.IsSupportH239(),
				  m_tSecComDStream.IsSupportHP());

		StaticLog( "m_byTrackNum:%d\n",m_byTrackNum );
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
//////////////////////////////////////////////////
//      ������Ϣ�õ��Ľṹ
//////////////////////////////////////////////////

struct TMcuMcuReq
{
	s8 m_szUserName[MAXLEN_PWD];
	s8 m_szUserPwd[MAXLEN_PWD];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMtViewPos
{
    s32  m_nViewID;
    u8   m_bySubframeIndex;    
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMtVideoInfo
{
    s32			m_nViewCount;                   
    TMtViewPos  m_atViewPos[MAXNUM_VIEWINCONF]; //�ն�����Щ��ͼ��
    s32			m_nOutputLID;                   //�ն˽����ĸ���ͼ
    s32			m_nOutVideoSchemeID;           //���뵽�ն˵���Ƶ����ID(��һ������֧�����ֻ����ʽ)                        
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMcuMcuMtInfo
{
    TMcuMcuMtInfo()
    {
        memset(this, 0, sizeof(TMcuMcuMtInfo));
    }
    TMt         m_tMt;
    s8 			m_szMtName[VALIDLEN_ALIAS_UTF8]; // �ն˱�������Ϊ16Byte // ��Ϊ50 [pengguofeng 7/3/2013] 
    u32			m_dwMtIp; 
    s8          m_szMtDesc[VALIDLEN_ALIAS];
    u8			m_byMtType;          // �ϼ����¼�Mcu ��Mt 
    u8          m_byManuId;          //���̱��,�μ����̱�Ŷ���
	u8			m_byVideoIn;       
    u8			m_byVideoOut;      
    u8			m_byVideo2In;     
    u8			m_byVideo2Out;     
    u8			m_byAudioIn;       
    u8			m_byAudioOut;      
    u8		    m_byIsDataMeeting;    
    u8		    m_byIsVideoMuteIn;    
    u8			m_byIsVideoMuteOut;   
    u8			m_byIsAudioMuteIn;    
    u8			m_byIsAudioMuteOut;   
    u8          m_byIsConnected;     
    u8			m_byIsFECCEnable;     
    u8          m_byProtocolType;
    TMtVideoInfo m_tPartVideoInfo;   
	u8          m_byCasLevel; //����뱾�����ն������������е���Ծ�ļ�����
	u8          m_abyMtIdentify[MAX_CASCADELEVEL]; //��Ӧ�������ն˵ı�ʶ

public:
	BOOL32 operator == ( const TMcuMcuMtInfo& tOtherMMMtInfo ) const
	{
		const u32 dwSize = sizeof(TMcuMcuMtInfo);
		return (memcmp( this, &tOtherMMMtInfo, dwSize) == 0) ? TRUE : FALSE;
	}

	void Print()
	{
		StaticLog("TMt:<%d %d>\t Name:<%s> MtIp:%x MtDesc<%s> MtType:%d Manu:%d CasLvl:%d\n",
			m_tMt.GetMcuId(), m_tMt.GetMtId(), m_szMtName, m_dwMtIp, m_szMtDesc, m_byMtType,
			m_byManuId, m_byCasLevel);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TCSchemeInfo
{
    s32		m_nVideoSchemeID;   //��Ƶ�������ID
    u32		m_dwMaxRate;        //max rate (kbps)
    u32		m_dwMinRate;        //min rate (kbps)
    BOOL	m_bCanUpdateRate;   // whether or not enable update rate
    s32     m_nFrameRate;       // frame rate (30��25)
    u8		m_byMediaType;      //
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TCViewInfo
{
    s32				m_nViewId;							//������ͼ ID
    u8              m_byRes;							//������ͼͼ��ķֱ���
    u8              m_byCurGeoIndex;					//������ͼ��ǰ����ʹ��(���)�ļ��β���,���ڼ��β����б��е�����������
    u8              m_byGeoCount;						//���β��ֵ���Ŀ
    u8              m_abyGeoInfo[MAXNUM_GEOINVIEW];		//���β����б�
    u8              m_byMtCount;						//������ͼ����ƵԴ����Ŀ
    TMt             m_atMts[MAXNUM_SUBFRAMEINGEO];		//��ƵԴ�б�
    u8              m_byVSchemeCount;					//��ѡ����Ƶ���������Ŀ
    TCSchemeInfo    m_atVSchemes[MAXNUM_VSCHEMEINVIEW]; //��Ƶ��������б�

    // guzh [7/19/2007]
    u8              m_byAutoSwitchStatus;               //�Զ���ѯ�ش�
    u8              m_byDynamic;
    s32             m_nAutoSwitchTime;                  //��ѯ�ش�ʱ����
    s32             m_nVideoRole;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TCConfViewInfo
{
	u8          m_byDefViewIndex;			//Ĭ�ϻ�����ͼ����
    u8          m_byViewCount;					//������ͼ����Ŀ
    TCViewInfo	m_atViewInfo[MAXNUM_VIEWINCONF];//������ͼ�б�
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//��������Ϣ�����ܻ��ж��speaker������ֻ���ƶ�һ��
struct TCMixerInfo
{
    s32    m_nMixerID;					//mixer ID
    TMt    m_tSpeaker;					//speaker Pid
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//MCU�Ļ���������Ϣ
struct TCConfAudioInfo
{
    u8				m_byMixerCount;						//��������Ŀ
    u8				m_byDefMixerIndex;					//ȱʡ����������������ʾ,��Ҳ�ǵ�ǰ���ڲ����Ļ�����
    TCMixerInfo     m_tMixerList[MAXNUM_MIXERINCONF];	//�������б�
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TCConfViewChangeNtf
{
	s32  m_nViewID;
    u8   m_bySubframeCount;    
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMtMediaChanStatus
{
	TMt		m_tMt;
    u8		m_byMediaMode;        //ý��ģʽ��MODE_VIDEO,MODE_AUDIO��
    u8		m_byIsDirectionIn;    //ý������������
    BOOL	m_bMute;              //true:�жϸ÷����������false:�򿪸÷����ý������
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TCAutoSwitchReq
{
    TCAutoSwitchReq(){ memset(this, 0, sizeof(TCAutoSwitchReq)); }

    s32     m_nSwitchLayerId;              
    s32     m_nSwitchSpaceTime;     //��ѵʱ����
    s32     m_nAutoSwitchLevel;     //��ѯ����,��ϸ����δ֪,1��������0��ֹͣ
    BOOL32  m_bSwitchOn;            //��ѯ��ͣ
    
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TSetInParam
{
	TMt m_tMt;
	s32 m_nViewId;
	u8  m_bySubFrameIndex;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TCConfViewOutInfo
{    
	TMt m_tMt;
    s32 m_nOutViewID;         
    s32 m_nOutVideoSchemeID;  
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TSetOutParam 
{
    s32					m_nMtCount;           
    TCConfViewOutInfo   m_atConfViewOutInfo[MAXNUM_CONF_MT];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//////////////////////////////////////////////////////////////////////////
//						���ý��滯���ýṹ
//////////////////////////////////////////////////////////////////////////

//mcu IP��ַ���������롢����IP����š��ۺ�
struct TMcuEqpCfg
{
protected:
    u32 m_dwMcuIpAddr;          //mcu Ip��ַ
    u32 m_dwMcuSubnetMask;      //mcu ��������
    u32 m_dwGWIpAddr;           //���� Ip��ַ
    u8	m_byLayer;              //���
    u8  m_bySlotId;             //��id
    u8  m_byInterface;          //ǰ������
	u8  m_byRemain;				// save
public:
    TMcuEqpCfg(void) { memset(this, 0, sizeof(TMcuEqpCfg)); }
    
    void    SetMcuIpAddr(u32 dwIpAddr) { m_dwMcuIpAddr = htonl(dwIpAddr); }     //host order
    u32     GetMcuIpAddr(void)  { return ntohl(m_dwMcuIpAddr); }
    void    SetMcuSubNetMask(u32 dwMask) { m_dwMcuSubnetMask = htonl(dwMask); }
    u32     GetMcuSubNetMask(void) { return ntohl(m_dwMcuSubnetMask); }
    void    SetGWIpAddr(u32 dwGWIpAddr) { m_dwGWIpAddr = htonl(dwGWIpAddr); }   //host order
    u32     GetGWIpAddr(void) { return ntohl(m_dwGWIpAddr); }
    void    SetLayer(u8 byLayer) { m_byLayer = byLayer; }
    u8      GetLayer(void) { return m_byLayer; }   
    void    SetSlot(u8 bySlotId) { m_bySlotId = bySlotId; }
    u8      GetSlot(void) { return m_bySlotId; }   
	void    SetInterface(u8 byInterface){ m_byInterface = byInterface;}
	u8      GetInterface(void) const { return m_byInterface;}
	
    void    Print(void)
    {
        StaticLog( "McuIpAddr:0x%x, McuSubnetMask:0x%x, GWIpAddr:0x%x, Layer:%d, SlotId:%d, Interface: %d\n",
                  GetMcuIpAddr(), GetMcuSubNetMask(), GetGWIpAddr(), GetLayer(), GetSlot(), GetInterface());
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMcuNetCfg
{
	TMcuEqpCfg	m_atMcuEqpCfg[MAXNUM_ETH_INTERFACE];
	u8			m_byInterface;
	
public:
	TMcuEqpCfg *GetMcuEqpCfg(u8 byIdx)
	{
		if( byIdx >= MAXNUM_ETH_INTERFACE )
		{
			return NULL;
		}
		return &m_atMcuEqpCfg[byIdx]; 
	}
	
	void SetMcuEqpCfg(u8 byIdx, TMcuEqpCfg tMcuEqpCfg)
	{
		if( byIdx >= MAXNUM_ETH_INTERFACE )
		{
			return;
		}
		m_atMcuEqpCfg[byIdx] = tMcuEqpCfg;
	}
	
	u8 GetInterface( void ) const
	{
		return m_byInterface;
	}
	void SetInterface( u8 byInterface)
	{
		m_byInterface = byInterface;
	}
	
	void Print(void)
	{
		for(u8 byLp = 0; byLp < MAXNUM_ETH_INTERFACE; byLp++)
		{
			m_atMcuEqpCfg[byLp].Print();		
		}
		StaticLog( "used interface is��%d\n", m_byInterface);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

enum enNetworkType	// �����ַ���ͣ�LAN or WAN 
{
	enBOTH	= 0,
	enWAN	= 1,
	enLAN	= 2
};

enum enUseModeType
{
	MODETYPE_3ETH	= 0,	//3����ģʽ
	MODETYPE_BAK	= 1		//����ģʽ
};

struct TMcu8KiNetCfg:TMcuNetCfg
{
	enUseModeType	m_enUseModeType;	// 3����ģʽ || ����ģʽ
	enNetworkType	m_enNetworkType;    // LAN or WAN 
	BOOL32			m_bIsUseSip;		// �Ƿ�����sip
	u8				m_bySipInEthIdx;	// SipIp���ĸ�������
	u32				m_dwSipIpAddr;		// SipIp
	u32				m_dwSipMaskAddr;	// SipMask
	u32				m_dwSipGwAddr;		// SipGw

public:

	TMcu8KiNetCfg()
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

	void Print(void)
	{
		Print();		
		s8	achSipIpAddr[32] = {0};
		s8	achSipMaskAddr[32] = {0};
		s8	achSipGwAddr[32] = {0};
		u32 dwSipIpAddr = GetSipIpAddr();
		u32 dwSipMaskAddr = GetSipMaskAddr();
		u32 dwSipGwAddr = GetSipGwAddr();
		sprintf(achSipIpAddr, "%d.%d.%d.%d%c", (dwSipIpAddr>>24)&0xFF, (dwSipIpAddr>>16)&0xFF, (dwSipIpAddr>>8)&0xFF, dwSipIpAddr&0xFF, 0);
		sprintf(achSipMaskAddr, "%d.%d.%d.%d%c", (dwSipMaskAddr>>24)&0xFF, (dwSipMaskAddr>>16)&0xFF, (dwSipMaskAddr>>8)&0xFF, dwSipMaskAddr&0xFF, 0);
		sprintf(achSipGwAddr, "%d.%d.%d.%d%c", (dwSipGwAddr>>24)&0xFF, (dwSipGwAddr>>16)&0xFF, (dwSipGwAddr>>8)&0xFF, dwSipGwAddr&0xFF, 0);
		
		StaticLog("\n[TMcuNetCfg]:\nm_enUseModeType:%s\nm_enNetworkType:%s	\
				  \nm_dwSipIpAddr:%s\nm_dwSipMakAddr:%s\nm_dwSipgwAddr:%s\n",	\
				(m_enUseModeType == MODETYPE_3ETH ? "MODETYPE_3ETH":"MODETYPE_BAK"),	\
				(m_enNetworkType == enLAN ? "enLAN":(m_enNetworkType == enWAN ? "enWAN":"enBOTH")),	\
				 achSipIpAddr,achSipMaskAddr,achSipGwAddr);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TNetParam
{
protected:   
    u32 m_dwIpAddr;		// ������
    u32 m_dwIpMask;		// ������
	
public:
    TNetParam()
    {
        SetNull();
    }
    
public:
    void SetNetParam( u32 dwIp, u32 dwIpMask )
    {
        m_dwIpAddr = htonl(dwIp);
        m_dwIpMask = htonl(dwIpMask); 
    }
	
    void SetNull(void)
    {
        m_dwIpAddr = 0;
        m_dwIpMask = 0;
    }
    
    BOOL32 IsValid() const
    {
        return ( 0 == m_dwIpAddr ) ? FALSE : TRUE ;
    }
    
    // ��ȡ��ַ����
    u32  GetIpAddr(void) const { return ntohl(m_dwIpAddr); }
    u32  GetIpMask(void) const{ return ntohl(m_dwIpMask); }
	
	BOOL   operator ==( const TNetParam & tObj ) const                  //�ж��Ƿ����
	{
		if ( GetIpAddr() == tObj.GetIpAddr() 
			&& GetIpMask() == tObj.GetIpMask() )
		{
			return TRUE;
		}
		return FALSE;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TRoute
{
protected:
    TNetParam m_tDstNet;
    u32 m_dwGateway;
    u32 m_dwMetric;     //Ծ����
	
public:
    TRoute(void) { SetNull(); }
	
    TNetParam GetDstNet(void) const { return m_tDstNet; }
    void    SetDstNet( const TNetParam &tDstNet ) { m_tDstNet = tDstNet; }
	
    u32 GetGateway(void) const { return ntohl(m_dwGateway); }
    void SetGateway( u32 dwGateway ) { m_dwGateway = htonl(dwGateway); }
	
    u32 GetMetric(void) const { return ntohl(m_dwGateway); }
    void SetMetric( u32 dwMetric ) { m_dwMetric = htonl(dwMetric); }
	
    void SetNull(void) 
    { 
        m_tDstNet.SetNull();
        m_dwGateway = 0;
        m_dwMetric = 0;
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TIpRouteCfg
{
	TNetParam	m_tNetParam; //ip,mask
    u32 m_dwGWIpAddr;        //���� Ip��ַ
	
	void SetNetRouteParam(u32 dwIpAddr, u32 dwNetMask, u32 dwGWIP)
	{
		m_tNetParam.SetNetParam(dwIpAddr, dwNetMask);
		m_dwGWIpAddr = htonl(dwGWIP);
	}

	u32  GetIpAddr(void) const { return m_tNetParam.GetIpAddr(); }
    u32  GetIpMask(void) const{ return m_tNetParam.GetIpMask(); }
	u32  GetGwIp(void)	const { return ntohl(m_dwGWIpAddr); }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TNetManuAlias
{
	s8  m_achName[MAXLEN_MULTINETALIAS];
	
	void SetAlias( LPCSTR lpzAlias )
	{
		if( lpzAlias != NULL )
		{
            memset( m_achName, 0, sizeof(m_achName));
			strncpy( m_achName, lpzAlias, MAXLEN_MULTINETALIAS-1 );
		}
	}

	const s8 *GetAlias(void) const
	{
		return m_achName;
	}

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TNetMANUInfo
{
	TIpRouteCfg		m_tIpRouteCfg;		
	TNetManuAlias	m_tNetManuAlias;	//����Ӫ�̱���

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//Multi-manufactory net access cfg
struct TMultiManuNetAccess
{
protected:
	//u8	m_byGkUsed;				//�Ƿ�����GK(1 Ϊ���ã�0Ϊ������)
	//u8  m_byEnableMultiManuNet;		//�Ƿ����ö���Ӫ��  (1 Ϊ���ã�0Ϊ������) (��ʵ�����������ô���)
	//TMcuEthCfg8KE	m_tMultiNetCfg;	//����������
	u8	m_byIpSecNum;				//��Ŀ����
	TNetMANUInfo m_atNetManuInfo[MCU_MAXNUM_MULTINET];
		
public:

	void Init( void ){ memset(this, 0, sizeof(TMultiManuNetAccess)); }
	
	TMultiManuNetAccess() {Init();}
	
	// ��������
	u8	AddIpSection(u32 dwIpAddr, u32 dwNetMask, u32 dwGwIpAddr, const TNetManuAlias *pchAliasName)
	{
		if (m_byIpSecNum >= MCU_MAXNUM_MULTINET)
		{
			return MCU_MAXNUM_MULTINET;	//��������ֵΪ14����fail
		}
		
		u8 byIdx = m_byIpSecNum;
		m_atNetManuInfo[byIdx].m_tIpRouteCfg.SetNetRouteParam(dwIpAddr, dwNetMask, dwGwIpAddr);
		s8* pachAlias = (s8*)(pchAliasName->m_achName);
		m_atNetManuInfo[byIdx].m_tNetManuAlias.SetAlias(pachAlias);
		m_byIpSecNum ++;
		
		return byIdx;
	}
	
	BOOL32 DelIpSection(u8 byIdx)
	{
		if(byIdx >= MCU_MAXNUM_MULTINET)
		{
			return FALSE;
		}
		
		for(u8 byLoop = byIdx; byLoop < m_byIpSecNum; byLoop ++)
		{
			
			m_atNetManuInfo[byLoop].m_tIpRouteCfg = m_atNetManuInfo[byLoop+1].m_tIpRouteCfg;

			m_atNetManuInfo[byLoop].m_tNetManuAlias.SetAlias(m_atNetManuInfo[byLoop+1].m_tNetManuAlias.GetAlias());
		}
		
		m_byIpSecNum --;
		
		return TRUE;
		
	}
	
	
	BOOL32 ModifyIpSection(u8 byIdx, u32 dwIpAddr, u32 dwNetMask, u32 dwGwIpAddr, const TNetManuAlias *pchAliasName)
	{
		if(byIdx >= MCU_MAXNUM_MULTINET)
		{
			return FALSE;
		}

		m_atNetManuInfo[byIdx].m_tIpRouteCfg.SetNetRouteParam(dwIpAddr, dwNetMask, dwGwIpAddr);
		s8* pachAlias = (s8*)(pchAliasName->m_achName);
		m_atNetManuInfo[byIdx].m_tNetManuAlias.SetAlias(pachAlias);
		
		return TRUE;
	}
	
	u8	GetIpSecNum( void ) const
	{
		return m_byIpSecNum;
	}


	//��ȡ�ľ�Ϊ������
	//��ȡĿ��IP��ַ
	u32 GetIpAddr(u8 byIdx) const
	{
		if( byIdx >= MCU_MAXNUM_MULTINET )
		{
			return 0;
		}
		
		return m_atNetManuInfo[byIdx].m_tIpRouteCfg.GetIpAddr();
	}
	
	u32 GetNetMask(u8 byIdx) const
	{
		if( byIdx >= MCU_MAXNUM_MULTINET )
		{
			return 0;
		}
		return m_atNetManuInfo[byIdx].m_tIpRouteCfg.GetIpMask();
	}
	
	u32 GetGWIp(u8 byIdx) const
	{
		if( byIdx >= MCU_MAXNUM_MULTINET )
		{
			return 0;
		}
		return m_atNetManuInfo[byIdx].m_tIpRouteCfg.GetGwIp();
	}
	
	BOOL32 GetAlias(u8 byIdx, TNetManuAlias *ptNetManuAlias) const
	{
		if( byIdx >= MCU_MAXNUM_MULTINET )
		{
			return FALSE;
		}
		
		memset(ptNetManuAlias->m_achName, 0, sizeof(ptNetManuAlias->m_achName));
		strncpy( ptNetManuAlias->m_achName, m_atNetManuInfo[byIdx].m_tNetManuAlias.GetAlias(), MAXLEN_MULTINETALIAS-1);
		
		return TRUE;
	}
	

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//�����ڶ���Ӫ����Ϣ ֧��16IP
struct TMultiEthManuNetAccess
{
protected:
	u8	m_byIpSecNum;				//��Ŀ����
	TNetMANUInfo m_atNetManuInfo[MCU_MAXNUM_ADAPTER];
	
public:
	void Init( void ){ memset(this, 0, sizeof(TMultiEthManuNetAccess)); }
	
	TMultiEthManuNetAccess() {Init();}

	// ��������
	u8	AddIpSection(u32 dwIpAddr, u32 dwNetMask, u32 dwGwIpAddr, const TNetManuAlias *pchAliasName)
	{
		
		if (m_byIpSecNum >= MCU_MAXNUM_ADAPTER)
		{
			return MCU_MAXNUM_ADAPTER;
		}
		
		u8 byIdx = m_byIpSecNum;
		m_atNetManuInfo[byIdx].m_tIpRouteCfg.SetNetRouteParam(dwIpAddr, dwNetMask, dwGwIpAddr);
		s8* pachAlias = (s8*)(pchAliasName->m_achName);
		m_atNetManuInfo[byIdx].m_tNetManuAlias.SetAlias(pachAlias);
		m_byIpSecNum ++;
		
		return byIdx;
	}
	
	BOOL32 DelIpSection(u8 byIdx)
	{
		if(byIdx >= MCU_MAXNUM_ADAPTER)
		{
			return FALSE;
		}
		
		for(u8 byLoop = byIdx; byLoop < m_byIpSecNum; byLoop ++)
		{
			
			m_atNetManuInfo[byLoop].m_tIpRouteCfg = m_atNetManuInfo[byLoop+1].m_tIpRouteCfg;

			m_atNetManuInfo[byLoop].m_tNetManuAlias.SetAlias(m_atNetManuInfo[byLoop+1].m_tNetManuAlias.GetAlias());
		}
		
		m_byIpSecNum --;
		
		return TRUE;
		
	}
	
	
	BOOL32 ModifyIpSection(u8 byIdx, u32 dwIpAddr, u32 dwNetMask, u32 dwGwIpAddr, const TNetManuAlias *pchAliasName)
	{
		if(byIdx >= MCU_MAXNUM_ADAPTER)
		{
			return FALSE;
		}

		m_atNetManuInfo[byIdx].m_tIpRouteCfg.SetNetRouteParam(dwIpAddr, dwNetMask, dwGwIpAddr);
		s8* pachAlias = (s8*)(pchAliasName->m_achName);
		m_atNetManuInfo[byIdx].m_tNetManuAlias.SetAlias(pachAlias);
		
		return TRUE;
	}
	
	u8	GetIpSecNum( void ) const
	{
		return m_byIpSecNum;
	}


	//��ȡ�ľ�Ϊ������
	//��ȡĿ��IP��ַ
	u32 GetIpAddr(u8 byIdx) const
	{
		if( byIdx >= MCU_MAXNUM_ADAPTER )
		{
			return 0;
		}
		
		return m_atNetManuInfo[byIdx].m_tIpRouteCfg.GetIpAddr();
	}
	
	u32 GetNetMask(u8 byIdx) const
	{
		if( byIdx >= MCU_MAXNUM_ADAPTER )
		{
			return 0;
		}
		return m_atNetManuInfo[byIdx].m_tIpRouteCfg.GetIpMask();
	}
	
	u32 GetGWIp(u8 byIdx) const
	{
		if( byIdx >= MCU_MAXNUM_ADAPTER )
		{
			return 0;
		}
		return m_atNetManuInfo[byIdx].m_tIpRouteCfg.GetGwIp();
	}
	
	BOOL32 GetAlias(u8 byIdx, TNetManuAlias *ptNetManuAlias) const
	{
		if( byIdx >= MCU_MAXNUM_ADAPTER )
		{
			return FALSE;
		}
		
		memset(ptNetManuAlias->m_achName, 0, sizeof(ptNetManuAlias->m_achName));
		strncpy( ptNetManuAlias->m_achName, m_atNetManuInfo[byIdx].m_tNetManuAlias.GetAlias(), MAXLEN_MULTINETALIAS-1);
		
		return TRUE;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMultiManuNetCfg
{
	u8  m_byEnableMultiManuNet;		//�Ƿ����ö���Ӫ��  (1 Ϊ���ã�0Ϊ������) (��ʵ�����������ô���)
	u32 m_dwGkIp;
	TMultiManuNetAccess	m_tMultiManuNetAccess;
	
	void SetGkIp(u32 dwGkIp) { m_dwGkIp = htonl(dwGkIp); }
	u32  GetGkIp(void) const { return ntohl(m_dwGkIp); }
	BOOL32 IsMultiManuNetEnable() const {return (1 == m_byEnableMultiManuNet)? TRUE: FALSE ;}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//mcu8000Eĳ���ڵ� IP��ַ���������롢����IP
struct TMcuEthCfg8KE
{
	enum EmNetWorkMode
	{
		AUTO,		//����Ӧ
		FULLD,			//Full Duplex
		HALFD,			//Half Duplex
		emModeEd
	};
protected:
    u32 m_dwIpAddr[MCU_MAXNUM_ADAPTER_IP];          //Ip��ַ
    u32 m_dwSubnetMask[MCU_MAXNUM_ADAPTER_IP];      //��������
    u32 m_dwGWIpAddr[MCU_MAXNUM_ADAPTER_IP];        //���� Ip��ַ
	u32 m_dwDefGWIpAddr;							//������Ĭ������ip (��ĳip��δ�����أ����ô�����)
	u8	m_byNetWorkMode;							//����ģʽ(ȫ˫������˫��, �Զ���)
	u8	m_byLinkStatus;								//����״̬(1Ϊ���ã�2Ϊ����)
	u8	m_byIpSecNum;								//��ǰ����ip�ε���Ŀ(1~16)
	u8	m_byReserved3;
	
public:
   
	void Init( void ){ memset(this, 0, sizeof(TMcuEthCfg8KE)); }
	
	TMcuEthCfg8KE(void) { Init(); }

	u8	AddIpSection(u32 dwIpAddr, u32 dwNetMask, u32 dwGwIpAddr)
	{
		if (m_byIpSecNum >= MCU_MAXNUM_ADAPTER_IP)
		{
			return MCU_MAXNUM_ADAPTER_IP;	//��������ֵΪ16����fail
		}

		u8 byIdx = m_byIpSecNum;
		m_dwIpAddr[byIdx] = htonl(dwIpAddr);
		m_dwSubnetMask[byIdx] = htonl(dwNetMask);
		m_dwGWIpAddr[byIdx] = htonl(dwGwIpAddr);
		m_byIpSecNum ++;

		return byIdx;
	}

	BOOL32 DelIpSection(u8 byIdx)
	{
		if(byIdx >= MCU_MAXNUM_ADAPTER_IP)
		{
			return FALSE;
		}

		for(u8 byLoop = byIdx; byLoop < m_byIpSecNum - 1; byLoop ++)
		{
			m_dwIpAddr[byLoop] = m_dwIpAddr[byLoop + 1];
			m_dwSubnetMask[byLoop] = m_dwSubnetMask[byLoop +1];
			m_dwGWIpAddr[byLoop] = m_dwGWIpAddr[byLoop + 1];
		}

		m_byIpSecNum --;

		return TRUE;

	}

	BOOL32 ModifyIpSection(u8 byIdx, u32 dwIpAddr, u32 dwNetMask, u32 dwGwIpAddr)
	{
		if(byIdx >= MCU_MAXNUM_ADAPTER_IP)
		{
			return FALSE;
		}
		
		m_dwIpAddr[byIdx] = htonl(dwIpAddr);
		m_dwSubnetMask[byIdx] = htonl(dwNetMask);
		m_dwGWIpAddr[byIdx] = htonl(dwGwIpAddr);

		return TRUE;

	}

	u8	GetIpSecNum( void )
	{
		return m_byIpSecNum;
	}

public:
    void    SetMcuIpAddr(u32 dwIpAddr, u8 byIdx = 0) 
	{ 
		if( byIdx >= MCU_MAXNUM_ADAPTER_IP )
		{
			return;
		}
		m_dwIpAddr[byIdx] = htonl(dwIpAddr);

	}     
    u32     GetMcuIpAddr(u8 byIdx = 0)  
	{ 
		if( byIdx >= MCU_MAXNUM_ADAPTER_IP )
		{
			return 0;
		}
		return ntohl(m_dwIpAddr[byIdx]); 
	}

    void    SetMcuSubNetMask(u32 dwMask, u8 byIdx = 0) 
	{ 
		if( byIdx >= MCU_MAXNUM_ADAPTER_IP )
		{
			return;
		}
		m_dwSubnetMask[byIdx] = htonl(dwMask); 
	}
    u32     GetMcuSubNetMask(u8 byIdx = 0) 
	{
		if( byIdx >= MCU_MAXNUM_ADAPTER_IP )
		{
			return 0;
		}
		return ntohl(m_dwSubnetMask[byIdx]); 
	}
    
	void    SetGWIpAddr(u32 dwGWIpAddr, u8 byIdx = 0) 
	{ 
		if( byIdx >= MCU_MAXNUM_ADAPTER_IP )
		{
			return;
		}
		m_dwGWIpAddr[byIdx] = htonl(dwGWIpAddr);
	}   
    u32     GetGWIpAddr(u8 byIdx = 0) 
	{ 
		if( byIdx >= MCU_MAXNUM_ADAPTER_IP )
		{
			return 0;
		}
		return ntohl(m_dwGWIpAddr[byIdx]);
	}
	
	void	SetDefGWIpAddr(u32 dwGWIpAddr) { m_dwDefGWIpAddr = htonl(dwGWIpAddr) ;}
	u32		GetDefGWIpAddr() { return ntohl(m_dwDefGWIpAddr); }

	void	SetNetWorkMode( u8 byMode ){ m_byNetWorkMode = byMode;}
	u8		GetNetWorkMode(void) { return m_byNetWorkMode; }
	
	void	SetLinkStatus( u8 byLinkStatus){ m_byLinkStatus = byLinkStatus;}
	u8		GetLinkStatus() { return m_byLinkStatus ; }
	BOOL32	IsLinkOn(){ return (m_byLinkStatus == 1) ? TRUE: FALSE ; }
	
	void    Print(void)
    {
		for(u8 byIdx = 0; byIdx < MCU_MAXNUM_ADAPTER_IP; byIdx ++)
		{
			if(GetMcuIpAddr(byIdx) == 0)
			{
				continue;
			}
			StaticLog( "[IP%u]-IpAddr:0x%x, SubnetMask:0x%x, GWIpAddr:0x%x\n",
				byIdx, GetMcuIpAddr(byIdx), GetMcuSubNetMask(byIdx), GetGWIpAddr(byIdx));
		}
        
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//mcu8000E ������������Ϣ
struct TMultiNetCfgInfo
{
	TMcuEthCfg8KE m_atMcuEthCfg8KE[MAXNUM_ETH_INTERFACE];
	
public:
	TMultiNetCfgInfo(void) 
	{
		for(u8 byLp = 0; byLp < MAXNUM_ETH_INTERFACE; byLp ++)
		{
			m_atMcuEthCfg8KE[byLp].Init();
		}
	}

	void SetMcuEthCfg(u8 byIdx, const TMcuEthCfg8KE &tMcuEthCfg8KE)
	{
		if( byIdx >= MAXNUM_ETH_INTERFACE )
			return;

		m_atMcuEthCfg8KE[byIdx] = tMcuEthCfg8KE;
	}

	TMcuEthCfg8KE* GetMcuEthCfg(u8 byIdx)
	{
		if( byIdx >= MAXNUM_ETH_INTERFACE )
			return  NULL;

		return &m_atMcuEthCfg8KE[byIdx];
	}

	u8	 GetLinkedOnEth()
	{
		u8 byEthIdx = MAXNUM_ETH_INTERFACE;
		for(u8 byLoop = 0; byLoop < MAXNUM_ETH_INTERFACE; byLoop ++)
		{
			if (m_atMcuEthCfg8KE[byLoop].IsLinkOn())
			{
				byEthIdx = byLoop;
				break;
			}
		}
		return byEthIdx; 
	}

	void Print( void )
	{
		for(u8 byLoop = 0; byLoop < MAXNUM_ETH_INTERFACE; byLoop ++)
		{
			StaticLog( "Eth.%u:\n---------\n", byLoop);
			m_atMcuEthCfg8KE[byLoop].Print();
		}
	}
	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//GK������Ϣ
struct TGKCfgInfo
{
protected:
    u32 m_dwGkIpAddr;			//GK Ip��ַ (������)
	u8	m_byGkUsed;				//�Ƿ���������GK (1 Ϊ���ã�0Ϊ������)
	
public:
    TGKCfgInfo(void) { memset(this, 0, sizeof(TGKCfgInfo)); }
    
    void    SetGkIpAddr(u32 dwIpAddr) { m_dwGkIpAddr = htonl(dwIpAddr); }   
    u32     GetGkIpAddr(void)  { return ntohl(m_dwGkIpAddr); }
	
	void    SetGkUsed( u8 byVal) { m_byGkUsed = byVal;}
	BOOL32  IsGkUsed(void) { return (m_byGkUsed == 1); }
	
	
    void    Print(void)
    {
        StaticLog( "GkIpAddr:0x%x, GkUsed:%d\n",
			GetGkIpAddr(), IsGkUsed() );
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;



//GK,����������Ϣ
struct TGKProxyCfgInfo
{
protected:
    u32 m_dwGkIpAddr;			//GK Ip��ַ (������)
	u32 m_dwProxyIpAddr;		//proxy IP
	u16	m_byProxyPort;			//�����õ�port	(������)
	u8	m_byGkUsed;				//�Ƿ���������GK (1 Ϊ���ã�0Ϊ������)
	u8  m_byProxyUsed;			//�Ƿ����ô���(1 Ϊ���ã�0Ϊ������)

public:
    TGKProxyCfgInfo(void) { memset(this, 0, sizeof(TGKProxyCfgInfo)); }
    
    void    SetGkIpAddr(u32 dwIpAddr) { m_dwGkIpAddr = htonl(dwIpAddr); }   
    u32     GetGkIpAddr(void)  { return ntohl(m_dwGkIpAddr); }
	
	void    SetProxyIpAddr(u32 dwIpAddr) { m_dwProxyIpAddr = htonl(dwIpAddr); }   
    u32     GetProxyIpAddr(void)  { return ntohl(m_dwProxyIpAddr); }

	void	SetProxyPort(u16 wPort) { m_byProxyPort = htons(wPort); }
	u16		GetProxyPort(){ return ntohs(m_byProxyPort); }

	void    SetGkUsed( u8 byVal) { m_byGkUsed = byVal;}
	BOOL32  IsGkUsed(void) { return (m_byGkUsed == 1); }

	void	SetProxyUsed( u8 byVal ){ m_byProxyUsed = byVal; }
	BOOL32	IsProxyUsed(void) { return (m_byProxyUsed == 1); }


    void    Print(void)
    {
        StaticLog( "GkIpAddr:0x%x, ProxyIpAddr:0x%x, Proxy Port:%d, GkUsed:%d, ProxyUsed: %d\n",
			GetGkIpAddr(), GetProxyIpAddr(), GetProxyPort(), IsGkUsed(), IsProxyUsed() );
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
/*
//mcu8000E������������Ϣ // �ݲ���
struct TMcuEqpCfg8KE
{
	TMcuEthCfg8KE m_atMcuEthCfg8KE[MAXNUM_ETH_INTERFACE];

public:
	void Print( void )
	{
		for(u8 byLoop = 0; byLoop < MAXNUM_ETH_INTERFACE; byLoop ++)
		{
			StaticLog( "Eth.%u:\t", byLoop);
			m_atMcuEthCfg8KE[byLoop].Print();
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
*/

//Ԥ������ͨ������������Դ�ṹ   jlb
struct THduChnlCfgInfo
{
protected:
	u8   m_byChnlIdx;               //ͨ��������
	u8   m_byEqpId;					//�豸ID
	u8   m_byChnlVolumn;            //ͨ������
	BOOL m_bIsChnlBeQuiet;          //ͨ������
	s8   m_achEqpAlias[MAXLEN_EQP_ALIAS];   //�������
public:
	THduChnlCfgInfo(){ SetNull(); }
    void SetNull(void)
	{
        memset( this, 0, sizeof(THduChnlCfgInfo) );
	    return;
	}

	u8   GetChnlIdx(void) const { return m_byChnlIdx; }
	void SetChnlIdx(u8 val) { m_byChnlIdx = val; }

	u8   GetEqpId(void) const { return m_byEqpId; }
	void SetEqpId(u8 val) { m_byEqpId = val; }

	u8   GetChnlVolumn(void) const { return m_byChnlVolumn; }
	void SetChnlVolumn(u8 val) { m_byChnlVolumn = val; }

	BOOL GetIsChnlBeQuiet(void) const { return m_bIsChnlBeQuiet; }
	void SetIsChnlBeQuiet(BOOL val) { m_bIsChnlBeQuiet = val; }

	void SetEqpAlias(LPCSTR pchEqpAlias)
    {
		//fixme
        memcpy(m_achEqpAlias, pchEqpAlias, MAXLEN_EQP_ALIAS);
        m_achEqpAlias[sizeof(m_achEqpAlias) - 1] = '\0';
    }
	
	LPCSTR GetEqpAlias(void){ return m_achEqpAlias; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
; 

//HDUԤ�跽���ṹ   jlb
struct THduStyleCfgInfo 
{
protected:
	u8   m_byStyleIdx;          //����Id��

	u8   m_byTotalChnlNum;      //������Ҫ��ͨ�������������п����õ����ͨ����
                                //���ݲ�ͬ�ķ�����Ϊ1��2��4������56�����56��
	THduChnlCfgInfo m_atHduChnlCfgTable[MAXNUM_HDUCFG_CHNLNUM];  //��ͨ����Ҫ��������Դ
	//m_bywidth * m_byHeight <= 56
	u8   m_byWidth;             //�������õĿ��
	u8   m_byHeight;            //�������õĸ߶�
    u8   m_byVolumn;            //����ͨ��ͳһ������С
	BOOL m_bIsBeQuiet;          //����ͨ���Ƿ�ͳһ����
	s8  m_achSchemeAlias[MAX_HDUSTYLE_ALIASLEN];   //Ԥ��������
	
public:
    THduStyleCfgInfo(void) { SetNull(); }
	void	SetNull(void)  { memset(this, 0, sizeof(THduStyleCfgInfo)); }
	BOOL32  IsNull(void) 
    { 
        THduStyleCfgInfo tInfo;
        return (0 == memcmp(this, &tInfo, sizeof(THduStyleCfgInfo)));
    }
    
	//patHduChnlCfgTable  Ϊ����ָ�룬Ԫ�ظ���ΪMAXNUM_HDUCFG_CHNLNUM
	void GetHduChnlCfgTable(THduChnlCfgInfo *patHduChnlCfgTable)
	{
		memcpy(patHduChnlCfgTable, m_atHduChnlCfgTable, MAXNUM_HDUCFG_CHNLNUM*sizeof(THduChnlCfgInfo));
        return;
	}
	//patHduChnlCfgTable  Ϊ����ָ�룬Ԫ�ظ���ΪMAXNUM_HDUCFG_CHNLNUM
	BOOL32 SetHduChnlCfgTable(THduChnlCfgInfo *patHduChnlCfgTable)
	{
		BOOL32 bRet = TRUE;
		if (NULL == patHduChnlCfgTable)
		{
			bRet = FALSE;
			OspPrintf(TRUE, FALSE, "[mcustruct.h]:SetHduChnlCfgTable()-->patHduChnlCfgTable is NULL\n");
		}

		memcpy(m_atHduChnlCfgTable, patHduChnlCfgTable, MAXNUM_HDUCFG_CHNLNUM*sizeof(THduChnlCfgInfo));
		return bRet;
	}

	u8   GetStyleIdx(void) const { return m_byStyleIdx; }
	void SetStyleIdx(u8 val) { m_byStyleIdx = val; }

	u8   GetTotalChnlNum(void) const { return m_byTotalChnlNum; }
	void SetTotalChnlNum(u8 val) { m_byTotalChnlNum = val; }

	u8   GetWidth(void) const { return m_byWidth; }
	void SetWidth(u8 val) { m_byWidth = val; }

	u8   GetHeight(void) const { return m_byHeight; }
	void SetHeight(u8 val) { m_byHeight = val; }

	u8   GetVolumn(void) const { return m_byVolumn; }
	void SetVolumn(u8 val) { m_byVolumn = val; }

	BOOL GetIsBeQuiet(void) const { return m_bIsBeQuiet; }
	void SetIsBeQuiet(BOOL val) { m_bIsBeQuiet = val; }

	void SetSchemeAlias(LPCSTR pchSchemeAlias)
    {
        memcpy(m_achSchemeAlias, pchSchemeAlias, sizeof(m_achSchemeAlias));
        m_achSchemeAlias[MAX_HDUSTYLE_ALIASLEN-1] = '\0';
    }
    
	LPCSTR GetSchemeAlias(void){ return m_achSchemeAlias; }

	inline BOOL operator == (const THduStyleCfgInfo& tHduStyle )
	{
		BOOL bRet = TRUE;
		if( this == &tHduStyle )
		{
			return bRet;
		}
		
		if ( 0 != memcmp(this, &tHduStyle, sizeof(THduStyleCfgInfo)) )
		{
			bRet = FALSE;
		}
		return bRet;
	}

    void Print(void)
	{
		StaticLog( "[THduStyleCfgInfo]: StyleIdx:%d, TotalChnlNum:%d, Width:%d, Height:%d, Volumn:%d, IsBeQuiet:%d\n",
			      GetStyleIdx(), GetTotalChnlNum(), GetWidth(), GetHeight(), GetVolumn(), GetIsBeQuiet());
		StaticLog( "[THduStyleCfgInfo]: Hdu Scheme Alias is:%s \n",m_achSchemeAlias);
	    
		StaticLog( "[THduStyleCfgInfo]:\n");
		u16 wIndex;
		for ( wIndex=0; wIndex<GetTotalChnlNum(); wIndex++)
	    {
			StaticLog( "[%d]:  ChnlIdx:%d, EqpId:%d, ChnlVolumn:%d, IsChnlBeQuiet:%d\n",
				       m_atHduChnlCfgTable[wIndex].GetChnlIdx(),
					   m_atHduChnlCfgTable[wIndex].GetEqpId(),
					   m_atHduChnlCfgTable[wIndex].GetChnlVolumn(),
    				   m_atHduChnlCfgTable[wIndex].GetIsChnlBeQuiet());
	    }

		return;
	}

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// hdu������ѯԤ����Ϣ
struct THduPollSchemeInfo 
{
protected:
	u32  m_dwKeepTime;                       // ��ѯ����
	u8   m_byCycles;                         // ��ѯ���ڴ���
	u8   m_byStatus;                         // ��ǰ������ѯ״̬
	u8   m_byReserved1;
    u8   m_byReserved2;
public:
	THduStyleCfgInfo m_tHduStyleCfgInfo;     // ����Ԥ��Ԥ��������Ϣ

public:
	THduPollSchemeInfo()
	{
        SetNull();
	}
	void SetNull( void )
	{
		m_dwKeepTime = 0;
		m_byCycles = 0;
		m_byStatus = POLL_STATE_NONE;
		m_byReserved1 = 0;
		m_byReserved2 = 0;
		
		m_tHduStyleCfgInfo.SetNull();
	}
	u32   GetKeepTime( void )
	{
		return ntohl(m_dwKeepTime);
	}
	void SetKeepTime( u32 dwKeepTime )
	{
		m_dwKeepTime = htonl(dwKeepTime);
	}

	u8   GetCycles( void )
	{
		return m_byCycles;
	}
	void SetCycles( u8 byCycles )
	{
		m_byCycles = byCycles;
	}

	u8   GetStatus( void )
	{
		return m_byStatus;
	}
	void SetStatus( u8 byStatus )
	{
		m_byStatus = byStatus;
	}

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// ������ѯǰ��Ԥ���б�ռ��ͨ����Ϣ
struct THduChnlInfoInUse
{
protected:
	u8    m_byConfIdx;       // ����������
	u8    m_byEqpId;         // ����ID
	u8    m_byChlIdx;        // ͨ��������
	u8    m_byResverd;       

public:
	THduChnlInfoInUse()
	{
		memset(this, 0x0, sizeof(THduChnlInfoInUse));
	}

	u8    GetConfIdx( void ){ return m_byConfIdx; }
	void  SetConfIdx( u8 byConfIdx ){ m_byConfIdx = byConfIdx; }

	u8    GetEqpId( void ){ return m_byEqpId; }
	void  SetEqpId( u8 byEqpId ){ m_byEqpId = byEqpId; }

	u8    GetChlIdx( void ){ return m_byChlIdx; }
	void  SetChlIdx( u8 byChlIdx ){ m_byChlIdx = byChlIdx; }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//�������û���
struct TEqpCfgInfo
{
protected:    
    s8  m_szAlias[MAXLEN_EQP_ALIAS];		//����
    u16 m_wMcuStartPort;					//mcu������ʼ�˿ں�
    u16 m_wEqpStartPort;					//���������ʼ�˿ں�    
    u8  m_byEqpId;							//�豸ID
    u8  m_bySwitchBrdIndex;					//����������     
    u8  m_byRunningBrdIndex;				//���а�����
    u8  m_byMapCount;						//ʹ�õ�MAP����
    u8  m_abyMapId[MAXNUM_MAP];				//Map�������

public:
    TEqpCfgInfo(void) { memset(this, 0, sizeof(TEqpCfgInfo)); }

    void    SetEqpId(u8 byEqpId) { m_byEqpId = byEqpId; }
    u8      GetEqpId(void) { return m_byEqpId; }
    void    SetMcuStartPort(u16 wPort) { m_wMcuStartPort = htons(wPort); }
    u16     GetMcuStartPort(void)   { return ntohs(m_wMcuStartPort); } 
    void    SetEqpStartPort(u16 wPort) { m_wEqpStartPort = htons(wPort); }
    u16     GetEqpStartPort(void) { return ntohs(m_wEqpStartPort); }
    void    SetSwitchBrdIndex(u8 byIndex) { m_bySwitchBrdIndex = byIndex; }
    u8      GetSwitchBrdIndex(void) { return m_bySwitchBrdIndex; }
    void    SetRunningBrdIndex(u8 byIndex) { m_byRunningBrdIndex = byIndex; }
    u8      GetRunningBrdIndex(void)    { return m_byRunningBrdIndex; }

    //���з��������������MAU���⴦����
protected:
    void    SetMapCount(u8 byCount) { m_byMapCount = byCount; }
    u8      GetMapCount(void) const { return m_byMapCount; }

public:
    void    SetAlias(LPCSTR lpszAlias)
    {
        if(NULL != lpszAlias)
        {
            strncpy(m_szAlias, lpszAlias, sizeof(m_szAlias));
            m_szAlias[sizeof(m_szAlias) - 1] = '\0';
        }        
    }
    const s8* GetAlias(void) { return m_szAlias; }

    BOOL32    SetMapId(u8 *pbyMapId, u8 byCount)
    {
        if(NULL == pbyMapId || byCount > MAXNUM_MAP)
            return FALSE;
        
        memcpy(m_abyMapId, pbyMapId, byCount);
        m_byMapCount = byCount;
        return TRUE;
    }
    BOOL32     GetMapId(u8 *pbyMapId, u8 &byCount)
    {
        if(NULL == pbyMapId)
            return FALSE;
        
        memcpy(pbyMapId, m_abyMapId, m_byMapCount);
        byCount = m_byMapCount;
        return TRUE;
    }

    void PrintMap(void)
    {
        StaticLog( "MapCount: %d, MapId: %d,%d,%d,%d,%d\n", 
            m_byMapCount, m_abyMapId[0], m_abyMapId[1], m_abyMapId[2], m_abyMapId[3], m_abyMapId[4]);
        
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// ��������Ϣ
//�豸id,����,mcu��ʼ�˿�,�豸��ʼ�˿�,����������,���а�����,ÿ��map����������,map���
struct TEqpMixerCfgInfo : public TEqpCfgInfo
{
protected:    
    u8  m_byMaxChnnlInGrp;					// ���������ͨ����
   
public:
    TEqpMixerCfgInfo(void):m_byMaxChnnlInGrp(MAXNUM_MIXER_CHNNL) {}
    
    void    SetMaxChnnlInGrp(u8 byChnnlNum) { m_byMaxChnnlInGrp = byChnnlNum; }
    u8      GetMaxChnnlInGrp(void)  { return m_byMaxChnnlInGrp; } 

    void    Print(void) 
    {
        StaticLog( "\nMixer:%s\nEqpId:%d, SwitchBrd:%d, RunBrd:%d, McuPort:%d, MixerPort:%d, MaxChnnl:%d\n",
                  GetAlias(), GetEqpId(), GetSwitchBrdIndex(), GetRunningBrdIndex(), 
                  GetMcuStartPort(), GetEqpStartPort(), GetMaxChnnlInGrp());
        PrintMap();
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// ¼�����Ϣ
//�豸id,����,mcu��ʼ�˿�,�豸��ʼ�˿�,����������,¼���ip
struct TEqpRecCfgInfo : public TEqpCfgInfo
{
protected:
    u32 m_dwIpAddr;							//���ַ���¼�����Ip��ַ

public:
    TEqpRecCfgInfo(void):m_dwIpAddr(0) {}
  
    void    SetIpAddr(u32 dwIpAddr)  { m_dwIpAddr = htonl(dwIpAddr); }  //host order
    u32     GetIpAddr(void) { return ntohl(m_dwIpAddr); }

    void    Print(void)
    {
        StaticLog( "\nRec:%s\nEqpId:%d, SwitchBrd:%d, McuPort:%d, RecPort:%d, RecIp:0x%x\n",
                  GetAlias(), GetEqpId(), GetSwitchBrdIndex(), GetMcuStartPort(), 
                  GetEqpStartPort(), GetIpAddr());
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// ����bas����Ϣ
//�豸id,����,mcu��ʼ�˿�,�豸��ʼ�˿�,����������,���а�����,����bas��ip
struct TEqpBasHDCfgInfo : public TEqpCfgInfo
{
protected:
    u32 m_dwIpAddr;							//Ip��ַ

public:
    TEqpBasHDCfgInfo(void):m_dwIpAddr(0) {}
  
    void    SetIpAddr(u32 dwIpAddr)  { m_dwIpAddr = htonl(dwIpAddr); }  //host order
    u32     GetIpAddr(void) { return ntohl(m_dwIpAddr); }
    void    SetType(u8 byType) { SetMapCount(byType); }
    u8      GetType(void) const { return GetMapCount(); }

    void    Print(void)
    {
        StaticLog( "\nRec:%s\nEqpId:%d, SwitchBrd:%d, McuPort:%d, RecPort:%d, RecIp:0x%x, Type.%d\n",
                  GetAlias(), GetEqpId(), GetSwitchBrdIndex(), GetMcuStartPort(), 
                  GetEqpStartPort(), GetIpAddr(), GetType());
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// ����ǽ��Ϣ
//�豸id,����,�豸��ʼ�˿�,����������,map���
struct TEqpTvWallCfgInfo : public TEqpCfgInfo
{     
    void    Print(void)
    {
        StaticLog( "\nTvWall:%s\nEqpId:%d, RunBrd:%d, TWPort:%d\n",
                 GetAlias(), GetEqpId(), GetRunningBrdIndex(), GetEqpStartPort());
        PrintMap();
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
struct THduChnlModePort
{
public:
	THduChnlModePort()
	{
		memset(this, 0x0, sizeof(THduChnlModePort));
	}
	u8 GetZoomRate() const { return byZoomRate; }
	void SetZoomRate(u8 val) { byZoomRate = val; }

    u8 GetOutPortType() const { return byOutPortType; }
    void SetOutPortType(u8 val) { byOutPortType = val; }
	
    u8 GetOutModeType() const { return byOutModeType; }
    void SetOutModeType(u8 val) { byOutModeType = val; }
	
	u8 GetScalingMode() const { return (reserved & 0x03);}
	void SetScalingMode(u8 byMode) { reserved = (reserved | 0x03) & (byMode | 0xFC); }

private:
    u8 byOutPortType;
    u8 byOutModeType;
    u8 byZoomRate;
	// 1)���ֶε�2λ��ʹ�ã����ڱ�ʶScalingMode [11/29/2011 chendaiwei]
    u8 reserved;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
; 

//Hdu��Ϣ  jlb
struct TEqpHduCfgInfo : public TEqpCfgInfo 
{
protected:
	
    THduChnlModePort atHduChnlModePort[MAXNUM_HDU_CHANNEL];    
	u8	m_byStartMode;//����ģʽ��(0: hdu_2 1:hdu_1 2:hdu_L )
public:
	TEqpHduCfgInfo()
	{
        memset(&atHduChnlModePort, 0X0, sizeof(atHduChnlModePort));
		SetStartMode(STARTMODE_HDU_M);
	}

	void SetStartMode(u8 byStartMode){m_byStartMode = byStartMode; }
	u8   GetStartMode(){ return m_byStartMode; }

	void GetHduChnlModePort( u8 byNum, THduChnlModePort &tHduChnModePort )
	{
		if (byNum == 0 || byNum == 1)
		{
			tHduChnModePort.SetOutModeType( atHduChnlModePort[byNum].GetOutModeType() );
			tHduChnModePort.SetOutPortType( atHduChnlModePort[byNum].GetOutPortType() );
		    tHduChnModePort.SetZoomRate( atHduChnlModePort[byNum].GetZoomRate() );
			tHduChnModePort.SetScalingMode( atHduChnlModePort[byNum].GetScalingMode() );
		}
		else
		{
			OspPrintf(TRUE, FALSE, "[TEqpHduCfgInfo]:GetHduChnlModePort()'s byNum wrong!\n");
			return;			
		}
		
		return;
	}
	
    void SetHduChnlModePort( u8 byNum, THduChnlModePort &tHduChnModePort )
	{
		if (byNum == 0 || byNum == 1)
		{
			atHduChnlModePort[byNum].SetOutModeType( tHduChnModePort.GetOutModeType() );
 			atHduChnlModePort[byNum].SetOutPortType( tHduChnModePort.GetOutPortType() );
            atHduChnlModePort[byNum].SetZoomRate( tHduChnModePort.GetZoomRate() );
			atHduChnlModePort[byNum].SetScalingMode(tHduChnModePort.GetScalingMode());
		}
		else
		{
			OspPrintf(TRUE, FALSE, "[TEqpHduCfgInfo]:SetHduChnlModePort()'s byNum wrong!\n");
			return;	
		}
		
		return;
	}
	
	void Print(void)
	{
		StaticLog( "\nHdu:%s\nEqpId:%d, RunBrd:%d, HduPort:%d\n,ZoomRate[0] = %d, ZoomRate[1] = %d\n HduOutPortType[0]:%d, HduOutModeType[0]:%d\n,HduOutPortType[1]:%d, HduOutModeType[1]:%d ,HduScaleMode[0]:%d, HduScalMode[1]:%d\n",
			GetAlias(), GetEqpId(), GetRunningBrdIndex(), GetEqpStartPort(), 
			atHduChnlModePort[0].GetZoomRate(), atHduChnlModePort[1].GetZoomRate(),
			atHduChnlModePort[0].GetOutPortType(), atHduChnlModePort[0].GetOutModeType(),
			atHduChnlModePort[1].GetOutPortType(), atHduChnlModePort[1].GetOutModeType(),
			atHduChnlModePort[0].GetScalingMode(),atHduChnlModePort[1].GetScalingMode());
	}
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

//svmp(��vmpģʽ)��Ϣ   jlb
struct TEqpSvmpCfgInfo : public TEqpCfgInfo
{
	void SetVmpType (u8 byVmpSubType) { m_abyMapId[0] = byVmpSubType; } //abyMapId��ʹ�ã������洢VMP������[5/11/2012 chendaiwei]
	u8	 GetVmpType ()const { return m_abyMapId[0]; }

	TEqpSvmpCfgInfo ()
	{
		m_abyMapId[0] = TYPE_MPUSVMP;//Ĭ����mpu��vmp
	}

    void    Print(void)
    {
        StaticLog( "\nDvmp:%s\nEqpId:%d, SwitchBrd:%d, RunBrd:%d, McuPort:%d, DvmpPort:%d,VmpType:%d\n",
			GetAlias(), GetEqpId(), GetSwitchBrdIndex(), GetRunningBrdIndex(),
			GetMcuStartPort(), GetEqpStartPort(),GetVmpType());
    }
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

//dvmp(˫vmpģʽ)�е���vmp����Ϣ   jlb
struct TEqpDvmpCfgBasicInfo : public TEqpCfgInfo
{

    void    Print(void)
    {
        StaticLog( "\nDvmp:%s\nEqpId:%d, SwitchBrd:%d, RunBrd:%d, McuPort:%d, DvmpPort:%d\n",
			GetAlias(), GetEqpId(), GetSwitchBrdIndex(), GetRunningBrdIndex(),
			GetMcuStartPort(), GetEqpStartPort());
    }
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

//˫vmpģʽ��������Ϣ   jlb
struct TEqpDvmpCfgInfo
{
public:
	TEqpDvmpCfgBasicInfo tEqpDvmpCfgBasicInfo[2];    //�������vmp����Ϣ

public:
	TEqpDvmpCfgInfo()
	{
		memset( this, 0 ,sizeof(TEqpDvmpCfgInfo) );
	}
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

//MpuBas��Ϣ    jlb
struct TEqpMpuBasCfgInfo : public TEqpCfgInfo
{
	u8 m_byStartMode;//����ģʽ(0:bap_4 , 1:bap_2)

	TEqpMpuBasCfgInfo(void)
	{
		SetStartMode(TYPE_MPUBAS); //Ĭ��ΪTYPE_MPUBAS
	}

	void SetStartMode(u8 byStartMode){ m_byStartMode = byStartMode;}
	u8   GetStartMode(){ return m_byStartMode; }

	void Print(void)
	{
	    StaticLog( "\nBas:%s\nEqpId:%d, SwitchBrd:%d, RunBrd:%d, McuPort:%d, BasPort:%d,StartMode:%d\n",
		    GetAlias(), GetEqpId(), GetSwitchBrdIndex(), GetRunningBrdIndex(),
		    GetMcuStartPort(), GetEqpStartPort(),GetStartMode());
        PrintMap();
	}
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

// Ebap ��Ϣ
//�豸id,����,mcu��ʼ�˿�,�豸��ʼ�˿�,����������,���а�����,map���
struct TEqpEbapCfgInfo : public TEqpCfgInfo
{
    void    Print(void)
    {
        StaticLog( "\nEbap:%s\nEqpId:%d, SwitchBrd:%d, RunBrd:%d, McuPort:%d, EbapPort:%d\n",
			GetAlias(), GetEqpId(), GetSwitchBrdIndex(), GetRunningBrdIndex(),
			GetMcuStartPort(), GetEqpStartPort());
        PrintMap();
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// Evpu ��Ϣ
//�豸id,����,mcu��ʼ�˿�,�豸��ʼ�˿�,����������,���а�����,map���
struct TEqpEvpuCfgInfo : public TEqpCfgInfo
{
    void    Print(void)
    {
        StaticLog( "\nVmp:%s\nEqpId:%d, SwitchBrd:%d, RunBrd:%d, McuPort:%d, VmpPort:%d\n",
            GetAlias(), GetEqpId(), GetSwitchBrdIndex(), GetRunningBrdIndex(),
            GetMcuStartPort(), GetEqpStartPort());
        PrintMap();
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


// Bas ��Ϣ
//�豸id,����,mcu��ʼ�˿�,�豸��ʼ�˿�,����������,���а�����,map���
struct TEqpBasCfgInfo : public TEqpCfgInfo
{
    void    Print(void)
    {
        StaticLog( "\nBas:%s\nEqpId:%d, SwitchBrd:%d, RunBrd:%d, McuPort:%d, BasPort:%d\n",
                GetAlias(), GetEqpId(), GetSwitchBrdIndex(), GetRunningBrdIndex(),
                GetMcuStartPort(), GetEqpStartPort());
        PrintMap();
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// Vmp ��Ϣ
//�豸id,����,mcu��ʼ�˿�,�豸��ʼ�˿�,����������,���а�����,map���,ͬʱ��Ƶ����·��
struct TEqpVmpCfgInfo : public TEqpCfgInfo
{
protected:
    u8  m_byEncodeNum;						//ͬʱ֧�ֵ���Ƶ����·��

public:
    TEqpVmpCfgInfo(void) :m_byEncodeNum(0) {}

    void    SetEncodeNum(u8 byEncodeNum) { m_byEncodeNum = byEncodeNum; }
    u8      GetEncodeNum(void)  { return m_byEncodeNum; }

    void    Print(void)
    {
        StaticLog( "\nVmp:%s\nEqpId:%d, SwitchBrd:%d, RunBrd:%d, McuPort:%d, VmpPort:%d, EncodeNum:%d\n",
            GetAlias(), GetEqpId(), GetSwitchBrdIndex(), GetRunningBrdIndex(),
            GetMcuStartPort(), GetEqpStartPort(), GetEncodeNum());
        PrintMap();
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//Mtw���ϵ���ǽ��Ϣ
//�豸id,����,�豸��ʼ�˿�,����������,map���
struct TEqpMTvwallCfgInfo : public TEqpCfgInfo
{
    void    Print(void)
    {
        StaticLog( "\nMTvwall:%s\nEqpId:%d, RunBrd:%d, MTWPort:%d\n",
            GetAlias(), GetEqpId(), GetRunningBrdIndex(), GetEqpStartPort());
        PrintMap();
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// Prs��Ϣ
//�豸id,����,mcu��ʼ�˿�,�豸��ʼ�˿�,����������,���а�����,�ش�����1,2,3,���ڶ���ʱ��
struct TPrsCfgInfo : public TEqpCfgInfo
{
protected:
    u16 m_wFirstTimeSpan;						//��һ���ش�����(ms)
    u16 m_wSecondTimeSpan;						//�ڶ����ش�����(ms)
    u16 m_wThirdTimeSpan;						//�������ش�����(ms)
    u16 m_wLoseTimeSpan;						//���ڶ�����ʱ����(ms)

public:
    TPrsCfgInfo(void)
    {
        SetFirstTimeSpan(DEF_FIRSTTIMESPAN_PRS);
        SetSecondTimeSpan(DEF_SECONDTIMESPAN_PRS);
        SetThirdTimeSpan(DEF_THIRDTIMESPAN_PRS);
        SetLoseTimeSpan(DEF_LOSETIMESPAN_PRS);
    }

    void    SetFirstTimeSpan(u16 wTimeSpan) { m_wFirstTimeSpan = htons(wTimeSpan); }
    u16     GetFirstTimeSpan(void)  { return ntohs(m_wFirstTimeSpan); }
    void    SetSecondTimeSpan(u16 wTimeSpan) { m_wSecondTimeSpan = htons(wTimeSpan); }
    u16     GetSecondTimeSpan(void) { return ntohs(m_wSecondTimeSpan); }
    void    SetThirdTimeSpan(u16 wTimeSpan) { m_wThirdTimeSpan = htons(wTimeSpan); }
    u16     GetThirdTimeSpan(void)  { return ntohs(m_wThirdTimeSpan); }
    void    SetLoseTimeSpan(u16 wTimeSpan) { m_wLoseTimeSpan = htons(wTimeSpan); }
    u16     GetLoseTimeSpan(void)   { return ntohs(m_wLoseTimeSpan); }

    void    Print(void)
    {
        StaticLog( "\nPrs:%s\nEqpId:%d, SwitchBrd:%d, RunBrd:%d, McuPort:%d\nPrsPort:%d, FstTime:%d, SndTime:%d, ThdTime:%d, LoseTime:%d\n",
            GetAlias(), GetEqpId(), GetSwitchBrdIndex(), GetRunningBrdIndex(), 
            GetMcuStartPort(), GetEqpStartPort(), GetFirstTimeSpan(), 
            GetSecondTimeSpan(), GetThirdTimeSpan(), GetLoseTimeSpan());
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// VRS��¼����Ϣ
struct TEqpVrsRecCfgInfo
{
protected:
	u8 m_byVrsId;							// vrs��¼�������ţ�1-16��
	s8 m_achAlias[MAXLEN_EQP_ALIAS+1];		// vrs��¼������
	TMtAlias m_tVrsRec;						// vrs��¼��������Ϣ
public:
    TEqpVrsRecCfgInfo(void) { memset( this, 0, sizeof(TEqpVrsRecCfgInfo)); }
    void SetVrsAlias(LPCSTR lpszAlias)
    {	
		s8 achAlias[MAXLEN_EQP_ALIAS+1] = {0};
		
        if ( NULL != lpszAlias)
		{
			strncpy(achAlias, lpszAlias, strlen(lpszAlias));
			
			//1.ȥ���ַ�����ǰ���������ո��Լ�TAB��
			Trim(achAlias); 
			achAlias[sizeof(achAlias)-1] = '\0';
			
			//2.��ȡ�Ƿ��ַ�֮ǰ���ַ���
			for(u8 byIdx = 0; byIdx < MAXLEN_EQP_ALIAS +1; byIdx++)
			{
				if(achAlias[byIdx] == ' ' || achAlias[byIdx] == ',' || achAlias[byIdx] == '\t')
				{
					achAlias[byIdx] = '\0';
					
					break;
				}
			}
			
			strncpy(m_achAlias, achAlias, sizeof(m_achAlias));
			m_achAlias[sizeof(m_achAlias)-1] = '\0';
		}
    }
    LPCSTR GetVrsAlias(void){ return m_achAlias; }
	void SetVrsCallAlias(TMtAlias tVrsRec) { m_tVrsRec =  tVrsRec;}
	TMtAlias GetVrsCallAlias(void){ return m_tVrsRec; }
	void SetVrsId(u8 byVrsId) { m_byVrsId = byVrsId; }
	u8 GetVrsId() { return m_byVrsId; }

    void    Print(void)
    {
        StaticLog( "\nVrsId.%d VrsRec:%s\n CallAliasType:%d, CallAlias:%s, VrsRecIp:0x%x\n",
			m_byVrsId, GetVrsAlias(), m_tVrsRec.m_AliasType, m_tVrsRec.m_achAlias, m_tVrsRec.m_tTransportAddr.GetIpAddr());
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//����������Ϣ
struct TBrdCfgInfo
{	
protected:
	u8				m_byIndex;					    // ������
    u8				m_byLayer;					    // ���
    u8              m_bySlotId;                     //��id
    u8              m_bySlotType;                   //������
    u8				m_byType;					    // ����
    u32				m_dwBrdIp;					    // ����Ip
    u8				m_bySelNetPort;				    // ����ѡ��
    u8				m_bySelMulticast;			    // �鲥ѡ��   
    u8              m_byBrdState;                   // ����״̬
    u8              m_byOsType;                     // �����OS���� OS_TYPE_WIN32 ...
    s8              m_szAlias[MAXLEN_BRDALIAS];     // ���������� BRD_ALIAS_TUI
public:
    TBrdCfgInfo(void) { memset(this, 0, sizeof(TBrdCfgInfo)); }

    void    SetIndex(u8 byIndex) { m_byIndex = byIndex; }
    u8      GetIndex(void) const { return m_byIndex; }
    void    SetLayer(u8 byLayer) { m_byLayer = byLayer; }
    u8      GetLayer(void) const { return m_byLayer; }    
    void    SetType(u8 byType) { m_byType = byType; }
    u8      GetType(void) const { return m_byType; }
    void    SetIpAddr(u32 dwIpAddr) { m_dwBrdIp = htonl(dwIpAddr); }    //host order
    u32     GetIpAddr(void) const { return ntohl(m_dwBrdIp); }
    void    SetSelNetPort(u8 bySel) { m_bySelNetPort = bySel; }
    u8      GetSelNetPort(void) const { return m_bySelNetPort; }
    void    SetSelMulticast(u8 bySel) { m_bySelMulticast = bySel; }
    u8      GetSelMulticast(void) const{ return m_bySelMulticast; }
    u8      GetBrdState(void) const { return m_byBrdState;}
    void    SetBrdState(u8 byBrdState) { m_byBrdState = byBrdState;}
    u8      GetOsType(void) const { return m_byOsType; }
    void    SetOsType(u8 byOsType) { m_byOsType = byOsType; }

    void SetSlot(u8 bySlotId, u8 bySlotType)
    {
        m_bySlotType = bySlotType;
        m_bySlotId  = bySlotId;      
    }

    void GetSlot(u8 &bySlotId, u8 &bySlotType) const
    { 
        bySlotId = m_bySlotId;
        bySlotType = m_bySlotType;
    }    

    void SetAlias(const s8* pszAlias)
    {
        memset( m_szAlias, 0, sizeof(m_szAlias) );
        strncpy( m_szAlias, pszAlias,  sizeof(m_szAlias) - 1);
    }
    const s8* GetAlias() const
    {
        return m_szAlias;
    }

    void Print(void) const
    {
        u8 bySlotId;
        u8 bySlotType;
        GetSlot(bySlotId, bySlotType);
        StaticLog( "Index:%d, Layer:%d, SlotId%d, SlotType%d, BrdType%d, Alias:%s, BrdIp:0x%x, EthInt:%d, Multi:%d, BrdState:%d, OsType:%d\n",
                  GetIndex(), GetLayer(), bySlotId, bySlotType, GetType(), GetAlias(), GetIpAddr(), GetSelNetPort(), GetSelMulticast(), GetBrdState(), GetOsType());
    }
}
#ifndef WIN32
__attribute__( (packed) )
#endif
;

// ����״̬
struct TBoardStatusNotify
{
public:
    TBoardStatusNotify()
    { 
        memset(this, 0, sizeof(TBoardStatusNotify));
        m_byBrdState = BOARD_OUTLINE;
    }

    u8 GetBrdLayer(void) {return m_byBrdLayer;}
    u8 GetBrdSlot(void) {return m_byBrdSlot;}
    u8 GetBrdType(void) {return m_byBrdType;}
    u8 GetBrdState(void) {return m_byBrdState;}
    u8 GetBrdOsType(void) {return m_byOsType;}

    void SetBrdLayer(u8 byLayer) { m_byBrdLayer = byLayer;}
    void SetBrdSlot(u8 bySlot) { m_byBrdSlot = bySlot;}
    void SetBrdType(u8 byType) { m_byBrdType = byType;}
    void SetBrdState(u8 byState) { m_byBrdState = byState;}
    void SetBrdOsType(u8 byType) { m_byOsType = byType; }

private: 
    u8 m_byBrdLayer;
    u8 m_byBrdSlot;
    u8 m_byBrdType;
    u8 m_byBrdState;
    u8 m_byOsType;
}
#ifndef WIN32
__attribute__( (packed) )
#endif
;

//N+1�䱸����Ϣ�ṹ
struct TNPlusInfo
{
protected:
    u32 m_dwNPlusMcuIp;                     //��֧��N+1�Ҳ��Ǳ��ݹ���ģʽ����Ҫ����N+1����mcu�ĵ�ַ
    u16 m_wNPlusRtdTime;                    //N+1����rtdʱ��(s)
    u16 m_wNPlusRtdNum;                     //N+1����rtd����
    u8  m_byNPlusRollBack;                  //�Ƿ�֧�ֹ��ϻָ���Ĭ��֧��
    u8  m_byNPlusMode;                      //�Ƿ�֧��N+1����
    u8  m_byNPlusBackupMode;                //�Ƿ�N+1���ݹ���ģʽ
    u8  m_byReserved;                       //����
    
public:
    TNPlusInfo() : m_dwNPlusMcuIp(0),
                   m_wNPlusRtdTime(0),
                   m_wNPlusRtdNum(0),
                   m_byNPlusRollBack(0),
                   m_byNPlusMode(0),
                   m_byNPlusBackupMode(0),
                   m_byReserved(0){}
    
    void    SetIsNPlusMode(BOOL32 bNPlus) { m_byNPlusMode = bNPlus ? 1 : 0; }   //�Ƿ�֧��N+1����
    BOOL32  IsNPlusMode(void) const { return (1 == m_byNPlusMode ? TRUE : FALSE); }
    void    SetIsNPlusBackupMode(BOOL32 bBackupMode) { m_byNPlusBackupMode = bBackupMode ? 1 : 0; }
    BOOL32  IsNPlusBackupMode(void) const{ return (1 == m_byNPlusBackupMode ? TRUE : FALSE); } //�Ƿ�N+1���ݹ���ģʽ
    void    SetNPlusMcuIp(u32 dwIp) { m_dwNPlusMcuIp = htonl(dwIp); }   
    u32     GetNPlusMcuIp(void)const { return ntohl(m_dwNPlusMcuIp); } //N+1����mcu�ĵ�ַ(host order)
    void    SetNPlusRtdTime(u16 wTime) { m_wNPlusRtdTime = htons(wTime); }
    u16     GetNPlusRtdTime(void) const{ return ntohs(m_wNPlusRtdTime); }
    void    SetNPlusRtdNum(u16 wNum) { m_wNPlusRtdNum = htons(wNum); }
    u16     GetNPlusRtdNum(void) const{ return ntohs(m_wNPlusRtdNum); }
    void    SetIsNPlusRollBack(BOOL32 bRollBack) { m_byNPlusRollBack = bRollBack ? 1 : 0; }
    BOOL32  GetIsNPlusRollBack(void) const{ return (1 == m_byNPlusRollBack ? TRUE : FALSE); }

    void    Print() const
    {
        StaticLog( "\nNPlusInfo:\nNPlusMode:%d, backupMode:%d, mcuIp:0x%x, rtd:%d, Num:%d\n",
                                IsNPlusMode(), IsNPlusBackupMode(), GetNPlusMcuIp(), 
                                GetNPlusRtdTime(), GetNPlusRtdNum());
    }
}
#ifndef WIN32
__attribute__( (packed) )
#endif
;

//mcu����������Ϣ
struct TMcuLocalCfgInfo
{    
protected:
    s8  m_szMcuAlias[MAXLEN_ALIAS];			// Mcu ����
    s8  m_szMcuE164[MAXLEN_E164];			// E164��
    s8  m_szMcuHardVer[MAXLEN_ALIAS];		// Mcu Ӳ���汾��   
    s8  m_szMcuSoftVer[MAXLEN_ALIAS];		// Mcu ����汾��
    
    u16 m_wDiscCheckTime;                   //�ն˶������ʱ��(s)
    u16 m_wDiscCheckTimes;                  //�ն˶���������
    u16 m_wPartlistRefreshTime;             //����ʱ�ն��б�ˢ��ʱ����(s)
    u16 m_wAudInfoRefreshTime;              //����ʱ��Ƶ��Ϣˢ��ʱ����(s)
    u16 m_wVidInfoRefreshTime;              //����ʱ��Ƶ��Ϣˢ��ʱ����(s)
    u8  m_bySaveBandWidth;                  //�Ƿ��ʡ����(0-����ʡ��1-��ʡ)
    u8  m_byReserved;                       //����

    TNPlusInfo m_tNPlusInfo;

	u8 m_byIsHoldDefaultConf;				//�Ƿ�֧��ȱʡ�����ٿ�(0-��֧�֣�1-֧��)
	u8 m_byIsShowMMcuMtList;				//�Ƿ�֧����ʾ�ϼ�mcu�ն��б�(0-��֧�֣�1-֧��)
	u8 m_byMaxMcsOnGoingConfNum;			//��������ٿ���mcs��ʱ������
	u8 m_byAdminLevel;						//�����ٿ��Ļ������������(1��4 1���)
	s8 m_byConfNameShowType;				//���¼���������ʾ�ı����Ļ������Ƶ�����(1:��������(Ĭ��ֵ) 2:mcu���ƣ���������)
	u8 m_byIsMMcuSpeaker;					//�Ƿ�Ĭ�Ͻ��ϼ�mcu��Ϊ������(0:���� 1:��

	u32 m_dwReserve;						//����

public:
    TMcuLocalCfgInfo() : m_wDiscCheckTime(DEF_DISCCHECKTIME),
                         m_wDiscCheckTimes(DEF_DISCCHECKTIMES),
                         m_wPartlistRefreshTime(PARTLIST_REFRESHTIME),
                         m_wAudInfoRefreshTime(AUDINFO_REFRESHTIME),
                         m_wVidInfoRefreshTime(VIDINFO_REFRESHTIME),
                         m_byReserved(0),
						 m_byIsHoldDefaultConf(0),
						 m_byIsShowMMcuMtList(0),
						 m_byMaxMcsOnGoingConfNum(DEF_MCSONGOINGCONFNUM_MAX),
						 m_byAdminLevel(DEF_ADMINLVEL),
						 m_byConfNameShowType(1),
						 m_byIsMMcuSpeaker(1),
						 m_dwReserve(0)
    { 
        memset(m_szMcuAlias, 0, sizeof(m_szMcuAlias));
        memset(m_szMcuE164, 0, sizeof(m_szMcuE164));
        memset(m_szMcuHardVer, 0, sizeof(m_szMcuHardVer));
        memset(m_szMcuSoftVer, 0, sizeof(m_szMcuSoftVer));
    }
    BOOL32  IsNull(void)
    {
        TMcuLocalCfgInfo tInfo;
        return (0 == memcmp(this, &tInfo, sizeof(TMcuLocalCfgInfo)));
    }

    void    SetIsSaveBandWidth(u8 bySave) { m_bySaveBandWidth = (0 == bySave) ? 0 : 1; }
    u8      GetIsSaveBandWidth(void) const { return m_bySaveBandWidth; }
    void    SetDiscCheckTime(u16 wTime) { m_wDiscCheckTime = htons(wTime); }
    u16     GetDiscCheckTime(void) const { return ntohs(m_wDiscCheckTime); }
    void    SetDiscCheckTimes(u16 wTimes) { m_wDiscCheckTimes = htons(wTimes); }
    u16     GetDiscCheckTimes(void) const { return ntohs(m_wDiscCheckTimes); }
    void    SetPartListRefreshTime(u16 wTime) { m_wPartlistRefreshTime = htons(wTime); }
    u16     GetPartListRefreshTime(void) const { return ntohs(m_wPartlistRefreshTime); }
    void    SetAudInfoRefreshTime(u16 wTime) { m_wAudInfoRefreshTime = htons(wTime); }
    u16     GetAudInfoRefreshTime(void) const { return ntohs(m_wAudInfoRefreshTime); }
    void    SetVidInfoRefreshTime(u16 wTime) { m_wVidInfoRefreshTime = htons(wTime); }
    u16     GetVidInfoRefreshTime(void) const { return ntohs(m_wVidInfoRefreshTime); }

    void    SetNPlusInfo(TNPlusInfo &tInfo) { m_tNPlusInfo = tInfo; }
    TNPlusInfo GetNPlusInfo() const {return m_tNPlusInfo;}
    void    SetIsNPlusMode(BOOL32 bNPlus) { m_tNPlusInfo.SetIsNPlusMode( bNPlus ); }   //�Ƿ�֧��N+1����
    BOOL32  IsNPlusMode(void) const{ return m_tNPlusInfo.IsNPlusMode(); }
    void    SetIsNPlusBackupMode(BOOL32 bBackupMode) { m_tNPlusInfo.SetIsNPlusBackupMode(bBackupMode); }
    BOOL32  IsNPlusBackupMode(void) const{ return m_tNPlusInfo.IsNPlusBackupMode(); } //�Ƿ�N+1���ݹ���ģʽ
    void    SetNPlusMcuIp(u32 dwIp) { m_tNPlusInfo.SetNPlusMcuIp(dwIp); }   
    u32     GetNPlusMcuIp(void) const{ return m_tNPlusInfo.GetNPlusMcuIp(); } //N+1����mcu�ĵ�ַ(host order)
    void    SetNPlusRtdTime(u16 wTime) { m_tNPlusInfo.SetNPlusRtdTime(wTime); }
    u16     GetNPlusRtdTime(void) const{ return m_tNPlusInfo.GetNPlusRtdTime(); }
    void    SetNPlusRtdNum(u16 wNum) { m_tNPlusInfo.SetNPlusRtdNum(wNum); }
    u16     GetNPlusRtdNum(void) const{ return m_tNPlusInfo.GetNPlusRtdNum(); }
    void    SetIsNPlusRollBack(BOOL32 bRollBack) { m_tNPlusInfo.SetIsNPlusRollBack(bRollBack); }
    BOOL32  GetIsNPlusRollBack(void) const{ return m_tNPlusInfo.GetIsNPlusRollBack(); }

	void	SetIsHoldDefaultConf( u8 byIsHold ){ m_byIsHoldDefaultConf = (0 == byIsHold) ? 0 : 1; }
	u8		GetIsHoldDefaultConf( void ){ return m_byIsHoldDefaultConf; }
	void	SetIsShowMMcuMtList( u8 byIsShow ){ m_byIsShowMMcuMtList = (0 == byIsShow) ? 0 : 1;  }
	u8		GetIsShowMMcuMtList( void ){ return m_byIsShowMMcuMtList; }
	void	SetMaxMcsOnGoingConfNum( u8 byNum ){ m_byMaxMcsOnGoingConfNum = byNum; }
	u8		GetMaxMcsOnGoingConfNum( void ){ return m_byMaxMcsOnGoingConfNum; };
	void	SetAdminLevel( u8 byAdminLevel ){ m_byAdminLevel = byAdminLevel; };
	u8		GetAdminLevel( void ){ return m_byAdminLevel; }
	void	SetConfNameShowType( u8 byType ) { m_byConfNameShowType = ( 2 == byType ) ? 2 : 1; }
	u8		GetConfNameShowType(void) const{ return m_byConfNameShowType; }
	void	SetIsMMcuSpeaker( u8 byIsMMcuSpeaker ){ m_byIsMMcuSpeaker = (0 == byIsMMcuSpeaker) ? 0 : 1; }
	u8		GetIsMMcuSpeaker( ){ return m_byIsMMcuSpeaker; }

    
    void SetMcuAlias(LPCSTR lpszAlias)
    {
        if(NULL != lpszAlias)
        {
            strncpy(m_szMcuAlias, lpszAlias, sizeof(m_szMcuAlias));
            m_szMcuAlias[sizeof(m_szMcuAlias) - 1] = '\0';
        }        
    }
    const s8* GetMcuAlias(void) const{ return m_szMcuAlias; }

    void SetMcuE164(LPCSTR lpszE164)
    {
        if(NULL != lpszE164)
        {
            strncpy(m_szMcuE164, lpszE164, sizeof(m_szMcuE164));
            m_szMcuE164[sizeof(m_szMcuE164) - 1] = '\0';
        }        
    }
    const s8* GetMcuE164(void) const{ return m_szMcuE164; }

    void SetMcuHardVer(LPCSTR lpszHardVer)          //mcu �ڲ�ʹ��
    {
        if(NULL != lpszHardVer)
        {
            strncpy(m_szMcuHardVer, lpszHardVer, sizeof(m_szMcuHardVer));
            m_szMcuHardVer[sizeof(m_szMcuHardVer) - 1] = '\0';
        }        
    }
    const s8* GetMcuHardVer(void) const{ return m_szMcuHardVer; }

    void SetMcuSoftVer(LPCSTR lpszSoftVer)          //mcu �ڲ�ʹ��
    {
        if(NULL != lpszSoftVer)
        {
            strncpy(m_szMcuSoftVer, lpszSoftVer, sizeof(m_szMcuSoftVer));
            m_szMcuSoftVer[sizeof(m_szMcuSoftVer) - 1] = '\0';
        }        
    }
    const s8* GetMcuSoftVer(void) const{ return m_szMcuSoftVer; }

    void    Print(void) const
    {
        StaticLog( "\nLocalInfo:\nMcuAlias:%s, E164:%s \nHardVer:%s \nSoftVer:%s",
											GetMcuAlias(), GetMcuE164(), GetMcuHardVer(), GetMcuSoftVer());

		StaticLog("\nIsSaveBW:%d DiscTime:%d, PartList:%d, Aud:%d, Vid:%d",
											GetIsSaveBandWidth(), GetDiscCheckTime(), GetPartListRefreshTime(),
										    GetAudInfoRefreshTime(), GetVidInfoRefreshTime());

		StaticLog("\nNPlusMode:%d, backupMode:%d, mcuIp:0x%x, rtd:%d, Num:%d",
											IsNPlusMode(), IsNPlusBackupMode(), GetNPlusMcuIp(), 
										    GetNPlusRtdTime(), GetNPlusRtdNum());

		StaticLog("\nIsHoldDefaultConf:%d IsShowMMcuMtList:%d MaxMcsOnGoingConfNum:%d AdminLevel:%d LocalConfNameShowType:%d m_byIsMMcuSpeaker:%d\n",
											m_byIsHoldDefaultConf,m_byIsShowMMcuMtList,
											m_byMaxMcsOnGoingConfNum,m_byAdminLevel,
											m_byConfNameShowType,m_byIsMMcuSpeaker);
    }    
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// Trap ��Ϣ
struct TTrapCfgInfo
{
protected:
    u32  m_dwTrapIp;							// Ip    
    u16  m_wReadWritePort;						// ��д�˿�
    u16  m_wSendTrapPort;						// ��Trap�˿�
    s8   m_szReadCom[MAXLEN_TRAP_COMMUNITY];	// ����ͬ��
    s8   m_szWriteCom[MAXLEN_TRAP_COMMUNITY];	// д��ͬ��

public:
    TTrapCfgInfo(void) 
    { 
        memset(this, 0, sizeof(TTrapCfgInfo)); 
        SetReadWritePort(DEF_READWRITE_TRAPPORT);
        SetSendTrapPort(DEF_SEND_TRAPPORT);
    }

    void    SetTrapIp(u32 dwIP) { m_dwTrapIp = htonl(dwIP); }
    u32     GetTrapIp(void) { return ntohl(m_dwTrapIp); }
    void    SetReadWritePort(u16 wPort) { m_wReadWritePort = htons(wPort); }
    u16     GetReadWritePort(void)  { return ntohs(m_wReadWritePort); }
    void    SetSendTrapPort(u16 wPort) { m_wSendTrapPort = htons(wPort); }
    u16     GetSendTrapPort(void)  { return ntohs(m_wSendTrapPort); }

    void SetReadCom(LPCSTR lpszReadCom)          
    {
        if(NULL != lpszReadCom)
        {
            strncpy(m_szReadCom, lpszReadCom, sizeof(m_szReadCom));
            m_szReadCom[sizeof(m_szReadCom) - 1] = '\0';
        }        
    }
    const s8* GetReadCom(void) { return m_szReadCom; }

    void SetWriteCom(LPCSTR lpszWriteCom)          
    {
        if(NULL != lpszWriteCom)
        {
            strncpy(m_szWriteCom, lpszWriteCom, sizeof(m_szWriteCom));
            m_szWriteCom[sizeof(m_szWriteCom) - 1] = '\0';
        }        
    }
    const s8* GetWriteCom(void) { return m_szWriteCom; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// ����������Ϣ 
struct TMcuNetCfgInfo
{	
protected:
    u32 m_dwGKIpAddr;                               // Gk ��ַ (��Ϊ0��ʾ��ע��gk)
    u32 m_dwRRQMtadpIpAddr;                         // ע��gk����Э��������ַ                      
    u32 m_dwMulticastIpAddr;                        // �鲥��ַ
    u16 m_wuMulticastPort;                          // �鲥�˿�
    u16 m_wRcvStartPort;                            // ������ʼ�˿�
    u16 m_w225245StartPort;                         // 245�˿�
    u16 m_wMaxMtNum;                                // mcu ����ն���
	u16 m_wMTUSize;									// MTU ��С, zgc, 2007-04-02
	u8	m_byMTUSetupMode;							// MTU����ģʽ, zgc, 2007-04-05
    u8  m_byUseMPCTransData;                        // �Ƿ�ʹ��Mpc������
    u8  m_byUseMPCStack;                            // �Ƿ�ʹ��Mpc����Э��ջ

	u8  m_byTrapListNum;                            // trap ����
	TTrapCfgInfo m_tTrapInfoList[MAXNUM_TRAP_LIST]; // trap����Ϣ
    u8  m_byGkCharge;                               // �Ƿ����GK�Ʒ�

public:
	TMcuNetCfgInfo(void) 
    { 
        memset(this, 0, sizeof(TMcuNetCfgInfo)); 
        SetMaxMtNum(MAXNUM_H225H245_MT);
    }
    BOOL32 IsNull(void)
    {
        TMcuNetCfgInfo tInfo;
        return (0 == memcmp(this, &tInfo, sizeof(TMcuNetCfgInfo)));
    }

    void SetGkIpAddr(u32 dwIpAddr) { m_dwGKIpAddr = htonl(dwIpAddr); }
    u32  GetGkIpAddr(void) { return ntohl(m_dwGKIpAddr); }
    void SetRRQMtadpIp(u32 dwIpAddr) { m_dwRRQMtadpIpAddr = htonl(dwIpAddr); }
    u32  GetRRQMtadpIp(void) { return ntohl(m_dwRRQMtadpIpAddr); }
    void SetMultiIpAddr(u32 dwIpAddr) { m_dwMulticastIpAddr = htonl(dwIpAddr); }
    u32  GetMultiIpAddr(void) { return ntohl(m_dwMulticastIpAddr); }
    void SetMultiPort(u16 wPort) { m_wuMulticastPort = htons(wPort); }
    u16  GetMultiPort(void) { return ntohs(m_wuMulticastPort); }
    void SetRcvStartPort(u16 wPort) { m_wRcvStartPort = htons(wPort); }
    u16  GetRcvStartPort(void) { return ntohs(m_wRcvStartPort); }
    void Set225245StartPort(u16 wPort) { m_w225245StartPort = htons(wPort); }
    u16  Get225245StartPort(void) { return ntohs(m_w225245StartPort); }
    void SetMaxMtNum(u16 wNum) { m_wMaxMtNum = htons(wNum); }
    u16  GetMaxMtNum(void) { return ntohs(m_wMaxMtNum); }
	void SetMTUSize(u16 wMTUSize) { m_wMTUSize = htons(wMTUSize); } // ����MTU��С, zgc, 2007-04-02
	u16  GetMTUSize(void) {return ntohs(m_wMTUSize); } // ���MTU��С, zgc, 2007-04-02
	void SetMTUSetupMode( u8 byMode ) { m_byMTUSetupMode = byMode; }
	u8	 GetMTUSetupMode( void ) { return m_byMTUSetupMode; }
    void SetIsUseMpcTransData(u8 byUse) { m_byUseMPCTransData = byUse; }
    u8   GetIsUseMpcTransData(void) { return m_byUseMPCTransData; }
    void SetIsUseMpcStack(u8 byUse) { m_byUseMPCStack = byUse; }
    u8   GetIsUseMpcStack(void) { return m_byUseMPCStack; }
    void   SetIsGKCharge(BOOL32 bCharge) { m_byGkCharge = bCharge ? 1 : 0;  }
    BOOL32 GetIsGKCharge(void) { return m_byGkCharge == 1 ? TRUE : FALSE;   }

    BOOL32 SetTrapList(TTrapCfgInfo *ptTrapInfo, u8 byNum)
    {
        if(NULL == ptTrapInfo || byNum > MAXNUM_TRAP_LIST)
            return FALSE;
        m_byTrapListNum = byNum;
        memcpy((void *)m_tTrapInfoList, (void *)ptTrapInfo, byNum*sizeof(TTrapCfgInfo));
        return TRUE;
    }
    BOOL32 GetTrapList(TTrapCfgInfo *ptTrapInfo, u8 &byNum)
    {
        if(NULL == ptTrapInfo)
            return FALSE;
        byNum = m_byTrapListNum;
        memcpy((void *)ptTrapInfo, (void *)m_tTrapInfoList, byNum*sizeof(TTrapCfgInfo));
        return TRUE;
    }
    
    void Print(void)
    {
        StaticLog( "\nNetCfg:\nGKIp:0x%x, RRQMtadpIp:0x%x, MultiIp:0x%x, MultiPort:%d\nRcvPort:%d, 225Port:%d, MaxMtNum:%d, IsMpcTrans:%d, IsMpcStack:%d, MTU size:%d, MTU mode:%d \n",
                                GetGkIpAddr(), GetRRQMtadpIp(), GetMultiIpAddr(), GetMultiPort(), GetRcvStartPort(),
                                Get225245StartPort(), GetMaxMtNum(), GetIsUseMpcTransData(), GetIsUseMpcStack(), GetMTUSize(), GetMTUSetupMode() );
        StaticLog( "traplist count: %d\n", m_byTrapListNum);
        for(u8 byTrap = 0; byTrap < m_byTrapListNum; byTrap++)
        {
            StaticLog( "%d. IpAddr:0x%x, ReadCom:%s, WriteCom:%s, RWPort:%u, SendPort:%u\n", byTrap,
                                    m_tTrapInfoList[byTrap].GetTrapIp(), m_tTrapInfoList[byTrap].GetReadCom(), 
                                    m_tTrapInfoList[byTrap].GetWriteCom(), m_tTrapInfoList[byTrap].GetReadWritePort(), 
                                    m_tTrapInfoList[byTrap].GetSendTrapPort());
        }
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// Qos ��Ϣ
struct TMcuQosCfgInfo
{	
protected:
    u8  m_byQosType;					// Qos ����
    u8  m_byAudLevel;					// ��Ƶ�ȼ�
    u8  m_byVidLevel;					// ��Ƶ�ȼ�
    u8  m_byDataLevel;					// ���ݵȼ�
    u8  m_bySignalLevel;				// �źŵȼ�
    u8  m_byIpServiceType;				// IP��������
	
public:
    TMcuQosCfgInfo(void) { memset(this, 0, sizeof(TMcuQosCfgInfo)); }
    BOOL32  IsNull(void) const
    {
        TMcuQosCfgInfo tInfo;
        return (0 == memcmp(this, &tInfo, sizeof(TMcuQosCfgInfo)));
    }

    void    SetQosType(u8 byType) { m_byQosType = byType; }
    u8      GetQosType(void) const { return m_byQosType; }
    void    SetAudLevel(u8 byLevel) { m_byAudLevel = byLevel; }
    u8      GetAudLevel(void) const { return m_byAudLevel; }
    void    SetVidLevel(u8 byLevel) { m_byVidLevel = byLevel; }
    u8      GetVidLevel(void) const { return m_byVidLevel; }
    void    SetDataLevel(u8 byLevel) { m_byDataLevel = byLevel; }
    u8      GetDataLevel(void) const { return m_byDataLevel; }
    void    SetSignalLevel(u8 byLevel) { m_bySignalLevel = byLevel; }
    u8      GetSignalLevel(void) const { return m_bySignalLevel; }
    void    SetIpServiceType(u8 byType) { m_byIpServiceType = byType; }
    u8      GetIpServiceType(void) const { return m_byIpServiceType; }

    void    Print(void) const
    {
        StaticLog( "QosCfg:  QosType:%d, Aud:%d, Vid:%d, Data:%d, Signal:%d, IpServciType:%d\n",
            GetQosType(), GetAudLevel(), GetVidLevel(), GetDataLevel(), 
            GetSignalLevel(), GetIpServiceType());
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// ��ͬ����Ϣ
struct TMcuNetSynCfgInfo
{	
protected:
    u8	m_byNetSynMode;			    	//��ͬ��ģʽ
    u8	m_byNetSynDTSlot;				//��ͬ��ʹ�õ�DT��� (0-14)
    u8	m_byNetSynE1Index;				//��ͬ��DT���ϵ�E1�� (0-7)
	
public:
    TMcuNetSynCfgInfo(void)   { memset(this, 0, sizeof(TMcuNetSynCfgInfo)); }
    BOOL32  IsNull(void)
    {
        TMcuNetSynCfgInfo tInfo;
        return (0 == memcmp(this, &tInfo, sizeof(TMcuNetSynCfgInfo)));
    }

    void    SetNetSynMode(u8 byMode) { m_byNetSynMode = byMode; }
    u8      GetNetSynMode(void) { return m_byNetSynMode; }
    void    SetNetSynDTSlot(u8 bySlot) { m_byNetSynDTSlot = bySlot; }
    u8      GetNetSynDTSlot(void) { return m_byNetSynDTSlot; }
    void    SetNetSynE1Index(u8 byIndex) { m_byNetSynE1Index = byIndex; }
    u8      GetNetSynE1Index(void) { return m_byNetSynE1Index; }

    void    Print(void)
    {
        StaticLog( "\nNetSynCfg:\nSynMode:%d, SynDTSlot:%d, SynE1Index:%d\n",
            GetNetSynMode(), GetNetSynDTSlot(), GetNetSynE1Index());
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//���ֻ��������������Ϣ
struct TDCSCfgInfo
{
protected:
	u32	m_dwDcsEqpIp;					//���ֻ���������豸id

public:
    TDCSCfgInfo(void) : m_dwDcsEqpIp(0) {}
    BOOL32  IsNull(void)
    {
        TDCSCfgInfo tInfo;
        return (0 == memcmp(this, &tInfo, sizeof(TDCSCfgInfo)));
    }

    void    SetDcsEqpIp(u32 dwEqpIp) { m_dwDcsEqpIp = htonl(dwEqpIp); }
    u32      GetDcsEqpIp(void) { return ntohl(m_dwDcsEqpIp); }
    
    void    Print(void)
    {
        StaticLog( "\nDcsEqpIp:0x%x\n", GetDcsEqpIp());
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TDscUpdateReqHead
{
	TDscUpdateReqHead()
	{
		memset( this, 0, sizeof(TDscUpdateReqHead) );
	}
	~TDscUpdateReqHead(){;}
public:
	s8 m_acFileName[MAXLEN_MCU_FILEPATH];	//�����ļ���
	u8  m_byMark;			//���һ֡��ǣ�0-�������һ֡��1-���һ֡
private:
	u32 m_dwFileSize;		//�ļ��ܳ���
	u32 m_dwFrmNum;			//�ļ���֡��
	u32 m_dwReqFrmSN;		//��ǰ������֡��
	u32 m_dwFrmSize;		//��ǰ�����ݰ�����
public:
	void SetFileSize(u32 dwFileSize) { m_dwFileSize = htonl(dwFileSize); }
	u32  GetFileSize(void) { return ntohl(m_dwFileSize); }
	void SetFrmNum(u32 dwFrmNum) { m_dwFrmNum = htonl(dwFrmNum); }
	u32  GetFrmNum(void) { return ntohl(m_dwFrmNum); }
	void SetReqFrmSN(u32 dwReqFrmSN) { m_dwReqFrmSN = htonl(dwReqFrmSN); } 
	u32  GetReqFrmSN(void) { return ntohl(m_dwReqFrmSN); }
	void SetFrmSize(u32 dwFrmSize) { m_dwFrmSize = htonl(dwFrmSize); }
	u32  GetFrmSize(void) { return ntohl(m_dwFrmSize); }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TDscUpdateRsp
{
	TDscUpdateRsp()
	{
		memset( this, 0, sizeof(TDscUpdateRsp) );
	}
	~TDscUpdateRsp(){;}
public:
	s8 m_acFileName[MAXLEN_MCU_FILEPATH];	//�����ļ���

private:
	u32 m_dwRspFrmSN;		//�������һ֡֡��

public:
	void SetRspFrmSN(u32 dwRspFrmSN) { m_dwRspFrmSN = htonl(dwRspFrmSN); }
	u32  GetRspFrmSN(void) { return ntohl(m_dwRspFrmSN); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;



struct TMINIMCUNetParam
{
public:
    enum enNetworkType
    {
        enInvalid = 0,
            enWAN = 1,
            enLAN = 2
    };
protected:

	u32 m_dwIpAddr;		// ������
	u32 m_dwIpMask;		// ������
	u32 m_dwGatewayIp;  // ������
	s8  m_asServerAlias[MAXLEN_PWD]; // ��Ӫ�̱���
    u8  m_byNetwordType;    // �����ַ���ͣ�LAN or WAN    
public:
	TMINIMCUNetParam():m_dwIpAddr(0),
					   m_dwIpMask(0),
					   m_dwGatewayIp(0),
                       m_byNetwordType(enInvalid)
	{
		memset(	m_asServerAlias, 0, sizeof(m_asServerAlias) );
	}

public:
    // ����Ϊ������ַ
    void SetWanParam( u32 dwIp, u32 dwIpMask, u32 dwGW, LPCSTR lpszAlias )
    {
        m_byNetwordType = enWAN;
        m_dwIpAddr = htonl(dwIp);
        m_dwIpMask = htonl(dwIpMask); 
        m_dwGatewayIp = htonl(dwGW);

        if (NULL != lpszAlias)
        {
            strncpy(m_asServerAlias, lpszAlias, MAXLEN_PWD-1);
            m_asServerAlias[MAXLEN_PWD-1] = 0;
        }
        else
        {
            memset(	m_asServerAlias, 0, sizeof(m_asServerAlias) );
        }
    }

    // ����Ϊ��������ַ
    void SetLanParam( u32 dwIp, u32 dwMask, u32 dwGW )
    {
        m_byNetwordType = enLAN;
        m_dwIpAddr = htonl(dwIp);
        m_dwIpMask = htonl(dwMask);
        m_dwGatewayIp = htonl(dwGW);

        // ����������Ҫ����
        memset(	m_asServerAlias, 0, sizeof(m_asServerAlias) );
    }

    BOOL32 IsValid() const
    {
        if ( 0 == m_dwIpAddr || enInvalid == m_byNetwordType )
        {
            return FALSE;
        }
        return TRUE;
    }

    // ��ȡ��������
    u8   GetNetworkType() const
    {
        return m_byNetwordType;
    }

    BOOL IsWan() const
    {
        return (m_byNetwordType == enWAN);
    }
    BOOL IsLan() const
    {
        return (m_byNetwordType == enLAN);
    }

    // ��ȡ��ַ����
    u32  GetIpAddr(void) const { return ntohl(m_dwIpAddr); }
	u32  GetIpMask(void) const{ return ntohl(m_dwIpMask); }
    u32  GetGatewayIp(void) const{ return ntohl(m_dwGatewayIp); }

    // ��ȡ����
    const s8 * GetServerAlias(void) const{ return m_asServerAlias; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMINIMCUNetParamAll
{
private:
	u8 m_byParamNum;
	TMINIMCUNetParam m_atMINIMCUNetParam[ETH_IP_MAXNUM];

public:
	TMINIMCUNetParamAll()
	{
		m_byParamNum = 0;
		memset( m_atMINIMCUNetParam, 0, sizeof(m_atMINIMCUNetParam) );
	}
	BOOL32 IsValid() const
	{
		return ( m_byParamNum != 0 );
	}
	u8 GetNetParamNum(void) const
	{
		return m_byParamNum;
	}
	u8 GetLANParamNum(void) const
	{
        u8 byNum = 0;
		for ( u8 byLop = 0; byLop < m_byParamNum; byLop++ )
		{
			if ( m_atMINIMCUNetParam[byLop].IsLan() )
			{
				byNum++;
			}
		}
		return byNum;
	}
	BOOL32 AddOneNetParam( const TMINIMCUNetParam &tNetParam )
	{
		if ( !tNetParam.IsValid() )
		{
			return FALSE;
		}
		u8 byIdx = 0;
		u8 byArrayLen = sizeof(m_atMINIMCUNetParam)/sizeof(TMINIMCUNetParam);
		for ( byIdx = 0; byIdx < byArrayLen; byIdx++ )
		{
			if( !m_atMINIMCUNetParam[byIdx].IsValid() )
			{
				memcpy( m_atMINIMCUNetParam+byIdx, &tNetParam, sizeof(TMINIMCUNetParam) );
				break;
			}
		}
		if ( byIdx == byArrayLen )
		{
			return FALSE;
		}
		m_byParamNum ++;
		return TRUE;
	}
	BOOL32 GetNetParambyIdx ( u8 byIdx, TMINIMCUNetParam &tNetParam ) const
	{
		memset( &tNetParam, 0, sizeof(TMINIMCUNetParam) );
		if ( byIdx >= m_byParamNum )
		{
			return FALSE;
		}
		if ( !m_atMINIMCUNetParam[byIdx].IsValid() )
		{
			return FALSE;
		}
		memcpy( &tNetParam, &m_atMINIMCUNetParam[byIdx], sizeof(TMINIMCUNetParam) );
		return TRUE;
	}
	BOOL32 DelNetParamByIdx(u8 byIdx)
	{
        if (m_byParamNum == 0)
        {
            return FALSE;
        }
		if ( byIdx >= m_byParamNum )
		{
			return FALSE;
		}
		u8 byLop = 0;
		for ( byLop = byIdx+1; byLop < m_byParamNum; byLop++ )
		{
			memcpy( &m_atMINIMCUNetParam[byLop-1], &m_atMINIMCUNetParam[byLop], sizeof(TMINIMCUNetParam) );
		}
		memset( &m_atMINIMCUNetParam[byLop-1], 0, sizeof(TMINIMCUNetParam) );
		m_byParamNum--;
		return TRUE;
	}
	BOOL32 IsEqualTo( const TMINIMCUNetParamAll &tMINIMCUNetParamAll )
	{
		return ( 0 == memcmp( &tMINIMCUNetParamAll, this, sizeof(TMINIMCUNetParamAll) ) ) ? TRUE : FALSE;
	}
	void Clear(void)
	{
		memset( this, 0, sizeof(TMINIMCUNetParamAll) );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//MCU8000B ����DSC����ģ��
struct T8000BDscMod
{
protected:
    u32 m_dwMcuInnerIp;          // DSC Ҫ���ӵ�MCU�ڲ�IP 
    u32 m_dwDscInnerIp;		     // DSC ���ڲ�IP
    u32 m_dwInnerIpMask;         // DSC �ڲ�IP����
    u16 m_wMcuListenPort;		 // DSC Ҫ���ӵ�MCU�˿�,һ����60000
	
	u8  m_byNetType;			 // MCU���ڵ���������
	u8  m_byLanMtProxyIpPos;	 // �����ն˴��������IP������λ�� 
	TMINIMCUNetParamAll m_tDscParamAll;	// ���е�ַ����DSC��IP��
	TMINIMCUNetParamAll m_tMcuParamAll;	// ������ӵ�ַ����MCU��0��IP��
	
    u8 m_byStartMp;
    u8 m_byStartMtAdp;
    u8 m_byStartGk;
    u8 m_byStartProxy;
    u8 m_byStartDcs;

public:
    T8000BDscMod():m_byStartMp(0),
                     m_byStartMtAdp(0),
                     m_byStartGk(0),
                     m_byStartProxy(0),
                     m_byStartDcs(0),
                     m_wMcuListenPort(MCU_LISTEN_PORT),
                     m_dwMcuInnerIp(0),
                     m_dwDscInnerIp(0),
                     m_dwInnerIpMask(0),
					 m_byNetType(NETTYPE_INVALID),
					 m_byLanMtProxyIpPos(0)
    {
		memset( &m_tDscParamAll, 0, sizeof(m_tDscParamAll) );
		memset( &m_tMcuParamAll, 0, sizeof(m_tMcuParamAll) );
	}
    
    void   SetMcuInnerIp(u32 dwIp) { m_dwMcuInnerIp = htonl(dwIp); }
    u32    GetMcuInnerIp() const { return ntohl(m_dwMcuInnerIp); }
    void   SetMcuPort(u16 wPort) { m_wMcuListenPort = htons(wPort); }
    u16    GetMcuPort() const { return ntohs(m_wMcuListenPort); }
    void   SetDscInnerIp(u32 dwIp) { m_dwDscInnerIp = htonl(dwIp); }
    u32    GetDscInnerIp() const { return ntohl(m_dwDscInnerIp); }
    void   SetInnerIpMask(u32 dwMask) { m_dwInnerIpMask = htonl(dwMask); }
    u32    GetInnerIpMask() const {return ntohl(m_dwInnerIpMask); }
	void   SetNetType(u8 byNetType) { m_byNetType = byNetType; }
	u8	   GetNetType(void) const { return m_byNetType; }
	void   SetLanMtProxyIpPos(u8 byLanMtProxyIpPos) { m_byLanMtProxyIpPos = byLanMtProxyIpPos; }
	u8	   GetLanMtProxyIpPos(void) const { return m_byLanMtProxyIpPos; }

	void SetCallAddrAll( TMINIMCUNetParamAll &tDscParamAll)
	{
		if ( tDscParamAll.IsValid() )
		{
			memcpy( &m_tDscParamAll, &tDscParamAll, sizeof(TMINIMCUNetParamAll) );
		}
	}
	void SetMcsAccessAddrAll( TMINIMCUNetParamAll &tMcuParamAll)
	{
		if ( tMcuParamAll.IsValid() )
		{
			memcpy( &m_tMcuParamAll, &tMcuParamAll, sizeof(TMINIMCUNetParamAll) );
		}
	}
	void GetCallAddrAll(TMINIMCUNetParamAll &tDscParamAll) const 
	{ 
		memcpy( &tDscParamAll, &m_tDscParamAll, sizeof(TMINIMCUNetParamAll) ); 
	}
	void GetMcsAccessAddrAll(TMINIMCUNetParamAll &tMcuParamAll) const 
	{ 
		memcpy( &tMcuParamAll, &m_tMcuParamAll, sizeof(TMINIMCUNetParamAll) ); 
	}
	u8 GetCallAddrNum(void) const 
    { 
        return m_tDscParamAll.GetNetParamNum(); 
    }
	u8 GetMcsAccessAddrNum(void) const 
    { 
        return m_tMcuParamAll.GetNetParamNum(); 
    }
    /*
    u8 GetLanCallAddrNum(void) const
    {
        return m_tDscParamAll.GetLANParamNum();
	}
	u8 GetLanMcsAccessAddrNum(void) const
	{
        return m_tMcuParamAll.GetLANParamNum();
	}
    */
   
    void   SetStartMp(BOOL32 bStart){ m_byStartMp = bStart ? 1 : 0;    }
    BOOL32 IsStartMp() const { return m_byStartMp == 1 ? TRUE : FALSE;    }
    void   SetStartMtAdp(BOOL32 bStart){ m_byStartMtAdp = bStart ? 1 : 0;    }
    BOOL32 IsStartMtAdp() const { return m_byStartMtAdp == 1 ? TRUE : FALSE;    }
    void   SetStartGk(BOOL32 bStart) { m_byStartGk = bStart ? 1 : 0; }
    BOOL32 IsStartGk() const { return m_byStartGk == 1 ? TRUE : FALSE; }
    void   SetStartProxy(BOOL32 bStart) { m_byStartProxy = bStart ? 1 : 0; }
    BOOL32 IsStartProxy() const { return m_byStartProxy == 1 ? TRUE : FALSE; }
    void   SetStartDcs(BOOL32 bStart) { m_byStartDcs = bStart ? 1 : 0; }
    BOOL32 IsStartDcs() const { return m_byStartDcs == 1 ? TRUE : FALSE; }

    void   Print() const
    {
        u8 byLop = 0;
        StaticLog( "T8000BDscMod: DscInnerIp: 0x%x, McuInnerIp: 0x%x:%d, InnerMask: 0x%x. Module: Mp.%d, MtAdp.%d, GK.%d, Proxy.%d, Dcs.%d\n", 
            m_dwDscInnerIp, m_dwMcuInnerIp, m_wMcuListenPort, m_dwInnerIpMask,
            m_byStartMp, m_byStartMtAdp, m_byStartGk, m_byStartProxy, m_byStartDcs);
		StaticLog( "Net Type: ");
		switch( m_byNetType )
		{
		case NETTYPE_INVALID:
			StaticLog( "INVALID\n");
			break;
		case NETTYPE_LAN:
			StaticLog( "LAN\n");
			break;
		case NETTYPE_WAN:
			StaticLog( "WAN\n");
			break;
		case NETTYPE_MIXING_ALLPROXY:
			printf( "MIXING_ALLPROXY\n");
			break;
		case NETTYPE_MIXING_NOTALLPROXY:
			printf( "MIXING_NOTALLPROXY\n" );
			break;
		default:
			break;
		}
		StaticLog( "LanMtProxyIpPos: %d\n", m_byLanMtProxyIpPos );
		StaticLog( "T8000BDscMod: DSC NetParam--\n");
		TMINIMCUNetParam tTemp;
		for ( byLop = 0; byLop < m_tDscParamAll.GetNetParamNum(); byLop++ )
		{
			if ( m_tDscParamAll.GetNetParambyIdx(byLop, tTemp) )
			{
				if ( tTemp.IsValid() )
				{
					StaticLog( "NetType:%s, Ip: 0x%x\tIpMask: 0x%x\tGateway: 0x%x\tServerAlias: %s\n",
                        tTemp.IsLan() ? "LAN" : "WAN",
						tTemp.GetIpAddr(), tTemp.GetIpMask(),
						tTemp.GetGatewayIp(), tTemp.GetServerAlias() );
				}
			}
		}
		StaticLog( "T8000BDscMod: MCU NetParam--\n");
		for ( byLop = 0; byLop < ETH_IP_MAXNUM; byLop++ )
		{
			if ( m_tMcuParamAll.GetNetParambyIdx(byLop, tTemp) )
			{
				if ( tTemp.IsValid() )
				{
					StaticLog( "NetType:%s, Ip: 0x%x\tIpMask: 0x%x\tGateway: 0x%x\tServerAlias: %s\n",
                        tTemp.IsLan() ? "LAN" : "WAN",
						tTemp.GetIpAddr(), tTemp.GetIpMask(),
						tTemp.GetGatewayIp(), tTemp.GetServerAlias() );
				}
			}
		}
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//Log info
struct TLoginInfo
{
public:
	s8  m_achUser[MAXLEN_PWD+1];//�û���
	s8  m_achPwd[MAXLEN_PWD+1]; //�û�����

public:
	TLoginInfo() { memset( this, 0, sizeof(TLoginInfo) ); }

	void  SetUser( LPCSTR lpszUser )
	{
		memset(m_achUser, 0, sizeof(m_achUser));
		if (NULL != lpszUser)
		{
			strncpy(m_achUser, lpszUser, sizeof(m_achUser));
			m_achUser[sizeof(m_achUser)-1] = '\0';
		}
	}

	LPCSTR GetUser( void ) const{ return m_achUser; }

	BOOL IsEqualUser( LPCSTR lpszUser ) const
	{
		s8 achBuf[MAXLEN_PWD+1] = {0};
		strncpy( achBuf, lpszUser, sizeof( achBuf ) );
		achBuf[sizeof(achBuf)-1] = '\0';
		
		if(0 == strncmp(achBuf, lpszUser, MAXLEN_PWD))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	void SetPwd( LPCSTR lpszPwd )
	{
		memset(m_achPwd, 0, sizeof(m_achPwd));
		if (NULL != lpszPwd)
		{
			strncpy(m_achPwd, lpszPwd, sizeof(m_achPwd));
			m_achPwd[sizeof(m_achPwd)-1] = '\0';
            // �򵥼��ܡ���GetPwd֮ǰҪ�ֶ�����
            EncPwd();
		}
	}

	BOOL32 GetPwd( s8* pchPwd, u8 byLength )
	{
		if ( byLength < strlen(m_achPwd)+1 )
		{
			return FALSE;
		}
		DecPwd();
		strncpy( pchPwd, m_achPwd, byLength );
		pchPwd[byLength-1] = '\0';
		EncPwd();
		return TRUE; 
	}
        
    void DecPwd()
    {
        EncPwd();
    }

	void Print(void)
	{
		StaticLog( "Username = %s\n", GetUser() );
		s8 achPwd[MAXLEN_PWD+1];
		memset(achPwd, 0, sizeof(achPwd));
		GetPwd(achPwd, sizeof(achPwd));
		StaticLog( "Password = %s\n", achPwd );
	}

private:
    void EncPwd()
    {
        for (u8 byLoop=0; byLoop<strlen(m_achPwd); byLoop++)
        {
            m_achPwd[byLoop] ^= 0x5a;
        }
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//  [1/8/2010 pengjie] Modify 
// HDU��չ����
struct THDUExCfgInfo
{
	u8  m_byIdleChlShowMode;  // ����ͨ����ʾģʽ HDU_SHOW_GRAY_MODE ...
	
	u8  m_byReserved; // �����ֶ�
	u16 m_wReserved;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


// MCU�ౣ���������չ����
struct TEqpExCfgInfo
{
public:
	THDUExCfgInfo m_tHDUExCfgInfo;
	TVMPExCfgInfo m_tVMPExCfgInfo;
	
public:
	TEqpExCfgInfo() { Init(); }
	void Init( void ) { memset(this, 0, sizeof(TEqpExCfgInfo)); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
// End Modify

//mcu general config info
struct TMcuGeneralCfg
{
    u8                  m_byVmpStyleNum;
    TVmpStyleCfgInfo    m_atVmpStyle[MAX_VMPSTYLE_NUM];
    TMcuLocalCfgInfo    m_tLocal;
    TMcuNetCfgInfo      m_tNet;
    TMcuQosCfgInfo      m_tQos;
    TMcuNetSynCfgInfo   m_tNetSyn;
    TDCSCfgInfo         m_tDcs;
    T8000BDscMod        m_tDscInfo;
	TLoginInfo			m_tLoginInfo;
	TEqpExCfgInfo       m_tEqpExCfgInfo;
	// [3/16/2010 xliang] add for 8000e 
	TMultiNetCfgInfo	m_tMultiNetCfg;
	TGKProxyCfgInfo		m_tGkProxyCfg;
	TPrsTimeSpan		m_tPrsTimeSpanCfg;
	TMultiManuNetAccess m_tMultiManuNetAccess;


public:
    void Print(void)
    {
        StaticLog( "mcu generalcfg info:\n");
        for(u8 byIndex = 0; byIndex < m_byVmpStyleNum; byIndex++)
        {
            m_atVmpStyle[byIndex].Print();
        }        
        m_tLocal.Print();
        m_tNet.Print();
        m_tQos.Print();
        m_tNetSyn.Print();
        m_tDcs.Print();
        m_tDscInfo.Print();
		m_tLoginInfo.Print();
		m_tMultiNetCfg.Print();
		m_tGkProxyCfg.Print();
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//��ַ�����ݽṹ����
//size: 143 byte, �����Ӵ˽ṹ������ע�����ÿ����Ϣ����Ŀ����(ADDRENTRYNUM_PERPKT)
struct TMcuAddrEntry
{
protected:
    u32     m_dwEntryIdx;                                   // ��Ŀ����
    s8	    m_szEntryName[MAXLEN_ADDR_ENTRYNAME+1];		    // ��Ŀ����
    s8	    m_szMtAlias[MAXLEN_ADDR_323ALIAS+1];	        // ���б���
    u32	    m_dwMtIpAddr;							        // IP��ַ(������)
    s8	    m_szMtE164[MAXLEN_ADDR_E164NUM+1];	    	    // ���к���
    u8      m_byMtProtocolType;                             // 0:H323�ն�; ��0:H320�ն�
    s8      m_szH320Id[MAXLEN_ADDR_320ID+1];                // H320ID
    u16	    m_wCallRate;							        // ��������

public:
    TMcuAddrEntry( void ) { memset(this, 0, sizeof(TMcuAddrEntry)); }
    
    void SetEntryIdx(u32 dwIndex)  { m_dwEntryIdx = htonl(dwIndex); } 
    u32  GetEntryIdx( void ) const { return ntohl(m_dwEntryIdx); }

    void SetEntryName(LPCSTR lpszEntryName)          
    {
        if(NULL != lpszEntryName)
        {
            strncpy(m_szEntryName, lpszEntryName, sizeof(m_szEntryName));
            m_szEntryName[sizeof(m_szEntryName) - 1] = '\0';
        }        
    }
    LPCSTR GetEntryName(void) const { return m_szEntryName; }

    void SetMtAlias(LPCSTR lpszMtAlias)          
    {
        if(NULL != lpszMtAlias)
        {
            strncpy(m_szMtAlias, lpszMtAlias, sizeof(m_szMtAlias));
            m_szMtAlias[sizeof(m_szMtAlias) - 1] = '\0';
        }        
    }
    LPCSTR GetMtAlias(void) const { return m_szMtAlias; }

    void SetMtIpAddr(u32 dwIp) { m_dwMtIpAddr = htonl(dwIp); }
    u32  GetMtIpAddr( void ) const { return ntohl(m_dwMtIpAddr); }

    void SetMtE164(LPCSTR lpszMtE164)
    {
        if(NULL != lpszMtE164)
        {
            strncpy(m_szMtE164, lpszMtE164, sizeof(m_szMtE164));
            m_szMtE164[sizeof(m_szMtE164) - 1] = '\0';
        }
    }
    LPCSTR GetMtE164( void ) const { return m_szMtE164; }

    void SetMtProtocolType(u8 byType) { m_byMtProtocolType = byType; }
    u8  GetMtProtocolType( void ) const { return m_byMtProtocolType; }

    void SetH320Id(LPCSTR lpszH320Id)
    {
        if(NULL != lpszH320Id)
        {
            strncpy(m_szH320Id, lpszH320Id, sizeof(m_szH320Id));
            m_szH320Id[sizeof(m_szH320Id) - 1] = '\0';
        }
    }
    LPCSTR GetH320Id( void ) const { return m_szH320Id; }

    void SetH320Alias( u8 byLayer, u8 bySlot, u8 byChannel )
    {
        m_byMtProtocolType = 1;
        sprintf( m_szH320Id, "H.320 MT, L:%d, S:%d, Ch:%d%c", byLayer, bySlot, byChannel, 0 );
    }
    BOOL GetH320Alias ( u8 &byLayer, u8 &bySlot, u8 &byChannel ) const
    {
        BOOL bRet = FALSE;
        byLayer   = 0;
        bySlot    = 0; 
        byChannel = 0; 
        if( 1 != m_byMtProtocolType )
        {
            return bRet;
        }
        
        const s8 *pachAlias = m_szH320Id;
        u8 byIndex = 0;
        for( u16 wLoop = 0; wLoop < strlen(pachAlias); wLoop++ )
        {
            if ( pachAlias[wLoop] == ':' )
            {
                s32  nPos = 0;
                s32  nMaxCHLen = 3; //һ�ֽڵ�����ִ�����
                s8   achLayer[4] = {0};
                const s8 * pachAliasTemp = &pachAlias[wLoop + 1];
                if ( 0 == byIndex )
                {//���

                    for( nPos = 0; nPos < nMaxCHLen; nPos++ )
                    {
                        if( ',' == pachAliasTemp[nPos] )
                        {
                            break;
                        }
                        achLayer[nPos] = pachAliasTemp[nPos];
                    }

                    if( 0 == nPos || nPos > nMaxCHLen )
                    {
                        return bRet;
                    }
                    achLayer[nPos+1] = '\0';
                    byLayer = atoi(achLayer);
                    byIndex++;  
                }
                else if( 1 == byIndex )
                {//�ۺ�

                    for( nPos = 0; nPos < nMaxCHLen; nPos++ )
                    {
                        if( ',' == pachAliasTemp[nPos] )
                        {
                            break;
                        }
                        achLayer[nPos] = pachAliasTemp[nPos];
                    }
                    
                    if( 0 == nPos || nPos > nMaxCHLen )
                    {
                        return bRet;
                    }
                    achLayer[nPos+1] = '\0';
                    bySlot = atoi(achLayer);
                    byIndex++;

                }
                else if( 2 == byIndex)
                {//ͨ����
                    for( nPos = 0; nPos < nMaxCHLen; nPos++ )
                    {
                        achLayer[nPos] = pachAliasTemp[nPos];
                    }
                    
                    if( 0 == nPos || nPos > nMaxCHLen )
                    {
                        return bRet;
                    }
                    achLayer[nPos+1] = '\0';
                    byChannel = atoi(achLayer);
                    break;
                }

                continue;               
            }
        }

        return bRet;
	}

    void SetCallRate(u16 wCallRate) { m_wCallRate = htons(wCallRate); }
    u16  GetCallRate( void ) const { return ntohs(m_wCallRate); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//size: 809 byte, �����Ӵ˽ṹ������ע�����ÿ����Ϣ����Ŀ�����(ADDRGROUPNUM_PERPKT)
struct TMcuAddrGroup
{
protected:
    u32 m_dwEntryIdx;								// ��Ŀ����
    u32 m_dwEntryNum;							    // ����ն˸���
    u32 m_adwEntryIdx[MAXNUM_CONF_MT];	            // ����ն�����
    s8  m_szGroupName[MAXLEN_ADDR_ENTRYNAME + 1];	// ����     

public:
    TMcuAddrGroup( void ) { memset(this, 0, sizeof(TMcuAddrGroup)); }

    void SetEntryIdx(u32 dwIndex) { m_dwEntryIdx = htonl(dwIndex); } 
    u32  GetEntryIdx( void ) const { return ntohl(m_dwEntryIdx); }

    void SetEntryNum(u32 dwEntryNum) { m_dwEntryNum = htonl(dwEntryNum); }
    u32  GetEntryNum( void )const { return ntohl(m_dwEntryNum); }

    void GetAllEntryIdx(u32 *padwEntryIdx, u32 &dwNum) const
    {
        if(NULL == padwEntryIdx) return;

        if(dwNum > GetEntryNum()) 
        {
            dwNum = GetEntryNum();
        }
        
        if(dwNum > MAXNUM_CONF_MT)
        {
            dwNum = MAXNUM_CONF_MT;
        }
        
        for(u32 dwIdx=0; dwIdx < dwNum; dwIdx++)
        {
            padwEntryIdx[dwIdx] = ntohl(m_adwEntryIdx[dwIdx]);
        }        
    }
    void SetAllEntryIdx( const u32 *padwEntryIdx, u32 dwNum)
    {
        if(NULL == padwEntryIdx) return;

        if(dwNum > MAXNUM_CONF_MT)
        {
            dwNum = MAXNUM_CONF_MT;
        }

        for(u32 dwIdx=0; dwIdx < dwNum; dwIdx++)
        {
            m_adwEntryIdx[dwIdx] = htonl(padwEntryIdx[dwIdx]);
        }

        m_dwEntryNum = htonl(dwNum);
    }
    
    void SetGroupName(LPCSTR lpszName)
    {
        if(NULL != lpszName)
        {
            strncpy(m_szGroupName, lpszName, sizeof(m_szGroupName));
            m_szGroupName[sizeof(m_szGroupName)-1] = '\0';
        }
    }
    LPCSTR GetGroupName( void ) const{ return m_szGroupName; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMcuAddrEntryV1
{
protected:
    u32     m_dwEntryIdx;                                   // ��Ŀ����
    s8	    m_szEntryName[MAXLEN_ADDR_ENTRYNAME_V1 + 1];		    // ��Ŀ����
    s8	    m_szMtAlias[MAXLEN_ADDR_323ALIAS_V1 + 1];	        // ���б���
    u32	    m_dwMtIpAddr;							        // IP��ַ(������)
    s8	    m_szMtE164[MAXLEN_ADDR_E164NUM+1];	    	    // ���к���
    u8      m_byMtProtocolType;                             // 0:H323�ն�; ��0:H320�ն�
    s8      m_szH320Id[MAXLEN_ADDR_320ID+1];                // H320ID
    u16	    m_wCallRate;							        // ��������

public:
    TMcuAddrEntryV1( void ) 
	{ 
		memset(this, 0, sizeof(TMcuAddrEntryV1));
	}

	u8 GetMemberNum(void) const
	{
		return 8;
	}

	u16 GetMemberLen(u8 byMemberId) const
	{
		switch (byMemberId)
		{
		case 0:
			return sizeof(m_dwEntryIdx);
		case 1:
			return sizeof(m_szEntryName);
		case 2:
			return sizeof(m_szMtAlias);
		case 3:
			return sizeof(m_dwMtIpAddr);
		case 4:
			return sizeof(m_szMtE164);
		case 5:
			return sizeof(m_byMtProtocolType);
		case 6:
			return sizeof(m_szH320Id);
		case 7:
			return sizeof(m_wCallRate);
		default:
			return 0;
		}
	}

	void GetTMcuAddrEntry( TMcuAddrEntry& tMcuaddr ) const
	{
		tMcuaddr.SetEntryIdx(GetEntryIdx());
		tMcuaddr.SetEntryName(GetEntryName());
		tMcuaddr.SetMtAlias(GetMtAlias());
		tMcuaddr.SetMtIpAddr(GetMtIpAddr());
		tMcuaddr.SetMtE164(GetMtE164());
		tMcuaddr.SetMtProtocolType(GetMtProtocolType());
		tMcuaddr.SetH320Id(GetH320Id());
		tMcuaddr.SetCallRate(GetCallRate());
	}

	TMcuAddrEntryV1& operator=(const TMcuAddrEntry& tAddrEntry)
	{
		SetEntryIdx(tAddrEntry.GetEntryIdx());
		SetEntryName(tAddrEntry.GetEntryName());
		SetMtAlias(tAddrEntry.GetMtAlias());
		SetMtIpAddr(tAddrEntry.GetMtIpAddr());
		SetMtE164(tAddrEntry.GetMtE164());
		SetMtProtocolType(tAddrEntry.GetMtProtocolType());
		SetH320Id(tAddrEntry.GetH320Id());
		SetCallRate(tAddrEntry.GetCallRate());	
		return *this;
	}
    
    void SetEntryIdx(u32 dwIndex) { m_dwEntryIdx = htonl(dwIndex); } 
    u32  GetEntryIdx( void )const { return ntohl(m_dwEntryIdx); }

    void SetEntryName(LPCSTR lpszEntryName)
    {
        if(NULL != lpszEntryName)
        {
            strncpy(m_szEntryName, lpszEntryName, sizeof(m_szEntryName));
            m_szEntryName[sizeof(m_szEntryName) - 1] = '\0';
        }        
    }
    LPCSTR GetEntryName(void) const { return m_szEntryName; }

    void SetMtAlias(LPCSTR lpszMtAlias)          
    {
        if(NULL != lpszMtAlias)
        {
            strncpy(m_szMtAlias, lpszMtAlias, sizeof(m_szMtAlias));
            m_szMtAlias[sizeof(m_szMtAlias) - 1] = '\0';
        }        
    }
    LPCSTR GetMtAlias(void) const { return m_szMtAlias; }

    void SetMtIpAddr(u32 dwIp) { m_dwMtIpAddr = htonl(dwIp); }
    u32  GetMtIpAddr( void ) const { return ntohl(m_dwMtIpAddr); }

    void SetMtE164(LPCSTR lpszMtE164)
    {
        if(NULL != lpszMtE164)
        {
            strncpy(m_szMtE164, lpszMtE164, sizeof(m_szMtE164));
            m_szMtE164[sizeof(m_szMtE164) - 1] = '\0';
        }
    }
    LPCSTR GetMtE164( void ) const { return m_szMtE164; }

    void SetMtProtocolType(u8 byType) { m_byMtProtocolType = byType; }
    u8  GetMtProtocolType( void ) const { return m_byMtProtocolType; }

    void SetH320Id(LPCSTR lpszH320Id)
    {
        if(NULL != lpszH320Id)
        {
            strncpy(m_szH320Id, lpszH320Id, sizeof(m_szH320Id));
            m_szH320Id[sizeof(m_szH320Id) - 1] = '\0';
        }
    }
    LPCSTR GetH320Id( void ) const { return m_szH320Id; }

    void SetH320Alias( u8 byLayer, u8 bySlot, u8 byChannel )
    {
        m_byMtProtocolType = 1;
        sprintf( m_szH320Id, "H.320 MT, L:%d, S:%d, Ch:%d%c", byLayer, bySlot, byChannel, 0);
    }
    BOOL GetH320Alias ( u8 &byLayer, u8 &bySlot, u8 &byChannel ) const
    {
        BOOL bRet = FALSE;
        byLayer   = 0;
        bySlot    = 0; 
        byChannel = 0; 
        if( 1 != m_byMtProtocolType )
        {
            return bRet;
        }
        
        const s8 *pachAlias = m_szH320Id;
        u8 byIndex = 0;
        for( u16 wLoop = 0; wLoop < strlen(pachAlias); wLoop++ )
        {
            if ( pachAlias[wLoop] == ':' )
            {
                s32  nPos = 0;
                s32  nMaxCHLen = 3; //һ�ֽڵ�����ִ�����
                s8   achLayer[4] = {0};
                const s8 * pachAliasTemp = &pachAlias[wLoop + 1];
                if ( 0 == byIndex )
                {//���

                    for( nPos = 0; nPos < nMaxCHLen; nPos++ )
                    {
                        if( ',' == pachAliasTemp[nPos] )
                        {
                            break;
                        }
                        achLayer[nPos] = pachAliasTemp[nPos];
                    }

                    if( 0 == nPos || nPos > nMaxCHLen )
                    {
                        return bRet;
                    }
                    achLayer[nPos+1] = '\0';
                    byLayer = atoi(achLayer);
                    byIndex++;  
                }
                else if( 1 == byIndex )
                {//�ۺ�

                    for( nPos = 0; nPos < nMaxCHLen; nPos++ )
                    {
                        if( ',' == pachAliasTemp[nPos] )
                        {
                            break;
                        }
                        achLayer[nPos] = pachAliasTemp[nPos];
                    }
                    
                    if( 0 == nPos || nPos > nMaxCHLen )
                    {
                        return bRet;
                    }
                    achLayer[nPos+1] = '\0';
                    bySlot = atoi(achLayer);
                    byIndex++;

                }
                else if( 2 == byIndex)
                {//ͨ����
                    for( nPos = 0; nPos < nMaxCHLen; nPos++ )
                    {
                        achLayer[nPos] = pachAliasTemp[nPos];
                    }
                    
                    if( 0 == nPos || nPos > nMaxCHLen )
                    {
                        return bRet;
                    }
                    achLayer[nPos+1] = '\0';
                    byChannel = atoi(achLayer);
                    break;
                }

                continue;               
            }
        }

        return bRet;
	}

    void SetCallRate(u16 wCallRate) { m_wCallRate = htons(wCallRate); }
    u16  GetCallRate( void ) const { return ntohs(m_wCallRate); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//size: 809 byte, �����Ӵ˽ṹ������ע�����ÿ����Ϣ����Ŀ�����(ADDRGROUPNUM_PERPKT)
struct TMcuAddrGroupV1
{
protected:
    u32 m_dwEntryIdx;								// ��Ŀ����
    u32 m_dwEntryNum;							    // ����ն˸���
    u32 m_adwEntryIdx[MAXNUM_CONF_MT];	            // ����ն�����
    s8  m_szGroupName[MAXLEN_ADDR_ENTRYNAME_V1 + 1];	// ����   
public:
    TMcuAddrGroupV1( void ) 
	{ 
		memset(this, 0, sizeof(TMcuAddrGroupV1)); 
	}

	u8 GetMemberNum(void)
	{
		return 4;
	}
	
	u16 GetMemberLen(u8 byMemberId)
	{
		switch (byMemberId)
		{
		case 0:
			return sizeof(m_dwEntryIdx);
		case 1:
			return sizeof(m_dwEntryNum);
		case 2:
			return sizeof(m_adwEntryIdx);
		case 3:
			return sizeof(m_szGroupName);
		default:
			return 0;
		}
	}

	void GetTMcuAddrGroup( TMcuAddrGroup& tAddrGroup ) const
	{
		tAddrGroup.SetEntryIdx( GetEntryIdx() );
		tAddrGroup.SetGroupName(  GetGroupName());
		u32 dwEntryNum = GetEntryNum();
		u32 adwEntryIdx[MAXNUM_CONF_MT];
		GetAllEntryIdx( adwEntryIdx, dwEntryNum );
		tAddrGroup.SetAllEntryIdx( adwEntryIdx, dwEntryNum );
	}
	
	TMcuAddrGroupV1& operator=( const TMcuAddrGroup& tAddrGroup )
	{
		SetEntryIdx( tAddrGroup.GetEntryIdx() );
		SetEntryNum( tAddrGroup.GetEntryNum() );
		SetGroupName( tAddrGroup.GetGroupName() );
		u32 dwEntryNum = MAXNUM_CONF_MT;
		u32 adwEntryIdx[MAXNUM_CONF_MT];
		tAddrGroup.GetAllEntryIdx( adwEntryIdx, dwEntryNum );
		SetAllEntryIdx( adwEntryIdx, dwEntryNum );
		return *this;
	}

    void SetEntryIdx(u32 dwIndex) { m_dwEntryIdx = htonl(dwIndex); } 
    u32  GetEntryIdx( void ) const{ return ntohl(m_dwEntryIdx); }

    void SetEntryNum(u32 dwEntryNum) { m_dwEntryNum = htonl(dwEntryNum); }
    u32  GetEntryNum( void ) const{ return ntohl(m_dwEntryNum); }

    void GetAllEntryIdx(u32 *padwEntryIdx, u32 &dwNum) const
    {
        if(NULL == padwEntryIdx) return;

        if(dwNum > GetEntryNum()) 
        {
            dwNum = GetEntryNum();
        }
        
        if(dwNum > MAXNUM_CONF_MT)
        {
            dwNum = MAXNUM_CONF_MT;
        }
        
        for(u32 dwIdx=0; dwIdx < dwNum; dwIdx++)
        {
            padwEntryIdx[dwIdx] = ntohl(m_adwEntryIdx[dwIdx]);
        }        
    }
    void SetAllEntryIdx( const u32 *padwEntryIdx, u32 dwNum)
    {
        if(NULL == padwEntryIdx) return;

        if(dwNum > MAXNUM_CONF_MT)
        {
            dwNum = MAXNUM_CONF_MT;
        }

        for(u32 dwIdx=0; dwIdx < dwNum; dwIdx++)
        {
            m_adwEntryIdx[dwIdx] = htonl(padwEntryIdx[dwIdx]);
        }

        m_dwEntryNum = htonl(dwNum);
    }
    
    void SetGroupName(LPCSTR lpszName)
    {
        if(NULL != lpszName)
        {
            strncpy(m_szGroupName, lpszName, sizeof(m_szGroupName));
            m_szGroupName[sizeof(m_szGroupName)-1] = '\0';
        }
    }
    LPCSTR GetGroupName( void ) const { return m_szGroupName; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//mcu����״̬֪ͨ
struct TMcuMsStatus
{
protected:
    u8  m_byIsMsSwitchOK;

public:
    TMcuMsStatus(void) : m_byIsMsSwitchOK(0) {}

    void    SetMsSwitchOK(BOOL32 bSwitchOK) { m_byIsMsSwitchOK = bSwitchOK ? 1:0; }
    BOOL32  IsMsSwitchOK(void) { return (1==m_byIsMsSwitchOK); }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//MCU����Ʒѽṹ len: 146
struct TConfChargeInfo
{
protected:
    s8  m_achGKUsrName[MAX_LEN_GK_USRNAME]; //GK�û���
    s8  m_achGKPwd[MAX_LEN_GK_PWD];         //GK����

    s8  m_achConfName[MAXLEN_CONFNAME+1];   //��������
    s8  m_achConfE164[MAXLEN_E164+1];       //����E164��
    
    u16 m_wBitRate;                         //��������
    u16 m_wTerNum;                          //�ն���
    u8  m_byMixerNum;                       //������
    u8  m_byVMPNum;                         //�໭����
    u8  m_byBASNum;                         //����������(��������ʽ����)
    u8  m_byUseCasd;                        //�Ƿ�ʹ�ü���
    u8  m_byUseEncrypt;                     //�Ƿ�ʹ�ü���
    u8  m_byUseDualVideo;                   //�Ƿ�ʹ��˫��
    u8  m_byUseDataConf;                    //�Ƿ��ٿ����ݻ���
    u8  m_byUseStreamBroadCast;             //�Ƿ�ʹ����ý���鲥
public:
    TConfChargeInfo() :m_wBitRate(0),
                       m_wTerNum(0),
                       m_byMixerNum(0),
                       m_byVMPNum(0),
                       m_byBASNum(0),
                       m_byUseCasd(0),
                       m_byUseEncrypt(0),
                       m_byUseDualVideo(0),
                       m_byUseDataConf(0),
                       m_byUseStreamBroadCast(0)
    {
        memset( &m_achGKUsrName, 0, sizeof(m_achGKUsrName) );
        memset( &m_achGKPwd,    0, sizeof(m_achGKPwd) );
        memset( &m_achConfName, 0, sizeof(m_achConfName) );
        memset( &m_achConfE164, 0, sizeof(m_achConfE164) );
    }
    void SetBitRate(u16 wBitRate) { m_wBitRate = htons(wBitRate);    }
    u16  GetBitRate(void) const { return ntohs(m_wBitRate);    }
    void SetTerNum(u16 wNum) { m_wTerNum = htons(wNum);    }
    u16  GetTerNum(void) const { return ntohs(m_wTerNum);    }
    void SetMixerNum(u8 byNum){ m_byMixerNum = byNum;    }
    u8   GetMixerNum(void) const { return m_byMixerNum;    }
    void SetVMPNum(u8 byVMPNum) { m_byVMPNum = byVMPNum;    }
    u8   GetVMPNum(void) const { return m_byVMPNum;    }
    void SetBasNum(u8 byBasNum) { m_byBASNum = byBasNum; }
    u8   GetBasNum(void) const { return m_byBASNum;    }
    void   SetIsUseCascade(BOOL32 bUse) { m_byUseCasd = bUse ? 1 : 0;    }
    BOOL32 GetIsUseCascade(void) const { return m_byUseCasd == 1 ? TRUE : FALSE;    }
    void   SetIsUseEncrypt(BOOL32 bUse) { m_byUseEncrypt = bUse ? 1 : 0;    }
    BOOL32 GetIsUseEncrypt(void) const { return m_byUseEncrypt == 1 ? TRUE : FALSE;    }
    void   SetIsUseDualVideo(BOOL32 bUse) { m_byUseDualVideo = bUse ? 1 : 0;    }
    BOOL32 GetIsUseDualVideo(void) const { return m_byUseDualVideo == 1 ? TRUE : FALSE;    }
    void   SetIsUseDataConf(BOOL32 bUse) { m_byUseDataConf = bUse ? 1 : 0;    }
    BOOL32 GetIsUseDataConf(void) const { return m_byUseDataConf == 1 ? TRUE : FALSE;    }
    void   SetIsUseStreamBroadCast(BOOL32 bUse) { m_byUseStreamBroadCast = bUse ? 1 : 0;    }
    BOOL32 GetIsUseStreamBroadCast(void) const { return m_byUseStreamBroadCast == 1 ? TRUE : FALSE;    }
    void SetGKUsrName(LPCSTR pszUsrName)
    {
        if ( pszUsrName != NULL )
        {
            u16 wLen = min(strlen(pszUsrName), MAX_LEN_GK_USRNAME-1);
            memcpy( m_achGKUsrName, pszUsrName, wLen );
            m_achGKUsrName[wLen] = '\0';
        }
        return;
    }
    LPCSTR GetGKUsrName(void) { return m_achGKUsrName;    }

    void SetGKPwd(LPCSTR pszPwd)
    {
        if ( pszPwd != NULL )
        {
            u16 wLen = min(strlen(pszPwd), MAX_LEN_GK_PWD-1);
            strncpy( m_achGKPwd, pszPwd, wLen );
            m_achGKPwd[wLen] = '\0';
        }
        return;
    }
    LPCSTR GetGKPwd(void) { return m_achGKPwd;    }
    
    void SetConfName(LPCSTR pszConfName)
    {
        if ( pszConfName != NULL )
        {
            u16 wLen = min(strlen(pszConfName), MAXLEN_CONFNAME);
            strncpy( m_achConfName, pszConfName, wLen );
            m_achConfName[wLen] = '\0';
        }
        return;
    }
    LPCSTR GetConfName(void) { return m_achConfName;    }

    void SetConfE164(LPCSTR pszConfE164)
    {
        if ( pszConfE164 != NULL )
        {
            u16 wLen = min(strlen(pszConfE164), MAXLEN_E164);
            strncpy( m_achConfE164, pszConfE164, wLen );
            m_achConfE164[wLen] = '\0';
        }
        return;
    }
    LPCSTR GetConfE164(void) { return m_achConfE164;    }
    
    BOOL32 IsNull(void) const { return (strlen(m_achGKUsrName) == 0 ? TRUE : FALSE); }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMpuChn
{
public:
    TMpuChn():m_dwIpAddr(0),
              m_byChnId(0)
    {
    }
    void   SetIpAddr(u32 dwIp) { m_dwIpAddr = htonl(dwIp); }
    u32    GetIpAddr(void) { return ntohl(m_dwIpAddr);  }
    void   SetChnId(u8 byId) { m_byChnId = byId; }
    u8     GetChnId(void) { return m_byChnId; }
    BOOL32 IsNull()    {    return 0 == m_dwIpAddr;    }
private:
    u32 m_dwIpAddr;
    u8 m_byChnId;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//mcu��mauʹ��״̬
struct TMcuHdBasStatus
{
    TMcuHdBasStatus()
    {
        Clear();
    }

    //�����
    BOOL32 GetConfMau(const CConfId &cConfId, 
                      u8 &byMauNum, u32 *pdwMau, u8 &byH263pMauNum, u32 *pdwH263pMau)
    {
        u8 byPos = 0xff;
        u8 byIdx = 0;

        if (!IsConfExist(cConfId, byPos))
        {
            return FALSE;
        }
        for(byIdx = 0; byIdx < MAXNUM_CONF_MAU; byIdx ++)
        {
            if (0 != m_aadwUsedMau[byPos][byIdx])
            {
                byMauNum++;
            }
        }

        if (!IsConfExist(cConfId, byPos))
        {
            return FALSE;
        }
        for(byIdx = 0; byIdx < MAXNUM_CONF_MAU; byIdx ++)
        {
            if (0 != m_aadwUsedH263pMau[byPos][byIdx])
            {
                byH263pMauNum++;
            }
        }
        return TRUE;
    }
    //�����
    BOOL32 GetConfMpuChn(const CConfId &cConfId,
                         u8 &byChnNum, TMpuChn *ptMpuChn)
    {
        u8 byPos = 0xff;
        u8 byIdx = 0;
        
        if (!IsConfExist(cConfId, byPos))
        {
            return FALSE;
        }
        for(byIdx = 0; byIdx < MAXNUM_CONF_MPU; byIdx ++)
        {
            if (!m_aatUsedMpuChn[byPos][byIdx].IsNull())
            {
                byChnNum++;
            }
        }
        memcpy(ptMpuChn, m_aatUsedMpuChn, byChnNum*sizeof(TMpuChn));
        return TRUE;
    }

    //�����
    BOOL32 GetIdleMau(u8 &byMauNum, u32 *pdwMau, u8 &byH263pMauNum, u32 *pdwH263pMau)
    {
        u8 byIdx = 0;

        for(byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx ++)
        {
            if (0 != m_adwIdleMau[byIdx])
            {
                byMauNum++;
            }
        }
        memcpy(pdwMau, &m_adwIdleMau, byMauNum);

        for(byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx ++)
        {
            if (0 != m_adwIdleH263pMau[byIdx])
            {
                byH263pMauNum++;
            }
        }
        memcpy(pdwMau, &m_adwIdleH263pMau, byH263pMauNum);

        return TRUE;
    }

    //�����(ע��: MPU�Ŀ�����Դ��ͨ��Ϊ��λ����)
    BOOL32 GetIdleMpuChn(u8 &byMpuChnNum, TMpuChn *ptMpuChn)
    {
        u8 byIdx = 0;
        
        for(byIdx = 0; byIdx < MAXNUM_PERIEQP*MAXNUM_MPU_CHN; byIdx ++)
        {
            if (!m_atIdleMpuChn[byIdx].IsNull())
            {
                byMpuChnNum++;
            }
        }
        memcpy(ptMpuChn, &m_atIdleMpuChn, byMpuChnNum);

        return TRUE;
    }

    //����ã�����ʧ�ܡ�ģ�徯��/֪ͨʱ �����������ȡ��
    void GetCurMauNeeded(u8 &byMauNum, u8 &byH263pMauNum)
    {
        byMauNum = m_byCurMau;
        byH263pMauNum = m_byCurH263pMau;
        return;
    }

    //����ã�����ʧ�ܡ�ģ�徯��/֪ͨʱ �����������ȡ��
    //ע��: mpu��Դ����̶���ͨ��Ϊ��λ�����������û����԰忨Ϊ��λ����:4 chn/mpu
    void GetCurMpuChnNeeded(u8 &byChnNum)
    {
        byChnNum = m_byCurMpuChn;
        return;
    }

    //���½ӿ�MCUʹ��

    void SetCurMauNeeded(u8 byMauNum, u8 byH263pMauNum)
    {
        m_byCurMau = byMauNum;
        m_byCurH263pMau = byH263pMauNum;
        return;
    }

    void SetCurMpuChnNeeded(u8 byMpuChn)
    {
        m_byCurMpuChn = byMpuChn;
        return;
    }

    void SetConfMau(const CConfId &cConfId, u32 dwMauIp, BOOL32 bH263p)
    {
        u8 byIdx = 0;
        u8 byPos = 0xff;
        u8 byFstIdlePos = 0xff;
        BOOL32 bExist = FALSE;
        BOOL32 bSet = FALSE;

        if (!bH263p)
        {
            if (IsConfExist(cConfId, byPos))
            {
                byFstIdlePos = 0xff;
                bExist = FALSE;
                for(; byIdx < MAXNUM_CONF_MAU; byIdx++)
                {
                    if (0 == m_aadwUsedMau[byPos][byIdx])
                    {
                        byFstIdlePos = byFstIdlePos == 0xff ? byIdx : byFstIdlePos;
                    }
                    if (dwMauIp == m_aadwUsedMau[byPos][byIdx])
                    {
                        bExist = TRUE;
                        break;
                    }
                }
                if (!bExist)
                {
                    if (0xff == byFstIdlePos)
                    {
                        OspPrintf(TRUE, FALSE, "[THdBasStatus][SetConfMau] no pos in ConfPos.%d\n", byPos);
                        return;
                    }
                    m_aadwUsedMau[byPos][byFstIdlePos] = dwMauIp;
                }
                return;
            }
            
            bSet = FALSE;
            for (byIdx = 0; byIdx < MAXNUM_ONGO_CONF; byIdx ++)
            {
                if (m_acConfId[byIdx].IsNull())
                {
                    m_acConfId[byIdx] = cConfId;
                    m_aadwUsedMau[byIdx][0] = dwMauIp;
                    bSet = TRUE;
                }
            }
            if (!bSet)
            {
                OspPrintf(TRUE, FALSE, "[THdBasStatus][SetConfMau] no pos for new conf!\n");
            }
            return;
        }

        //H263p���
        if (IsConfExist(cConfId, byPos))
        {
            byFstIdlePos = 0xff;
            bExist = FALSE;
            for(; byIdx < MAXNUM_CONF_MAU; byIdx++)
            {
                if (0 == m_aadwUsedH263pMau[byPos][byIdx])
                {
                    byFstIdlePos = byFstIdlePos == 0xff ? byIdx : byFstIdlePos;
                }
                if (dwMauIp == m_aadwUsedH263pMau[byPos][byIdx])
                {
                    bExist = TRUE;
                    break;
                }
            }
            if (!bExist)
            {
                if (0xff == byFstIdlePos)
                {
                    OspPrintf(TRUE, FALSE, "[TMcuHdBasStatus][SetConfMau] no pos in ConfPos.%d(h263p)\n", byPos);
                    return;
                }
                m_aadwUsedH263pMau[byPos][byFstIdlePos] = dwMauIp;
            }
            return;
        }
        
        bSet = FALSE;
        for (byIdx = 0; byIdx < MAXNUM_ONGO_CONF; byIdx ++)
        {
            if (m_acConfId[byIdx].IsNull())
            {
                m_acConfId[byIdx] = cConfId;
                m_aadwUsedH263pMau[byIdx][0] = dwMauIp;
                bSet = TRUE;
            }
        }
        if (!bSet)
        {
            OspPrintf(TRUE, FALSE, "[TMcuHdBasStatus][SetConfMau] no pos for new conf(h263p)!\n");
        }
        return;
    }

    void SetConfMpuChn(const CConfId &cConfId, u32 dwMpuIp, u8 byChnId)
    {
        u8 byIdx = 0;
        u8 byPos = 0xff;
        u8 byFstIdlePos = 0xff;
        BOOL32 bExist = FALSE;

        if (IsConfExist(cConfId, byPos))
        {
            byFstIdlePos = 0xff;
            bExist = FALSE;
            for(; byIdx < MAXNUM_CONF_MPU; byIdx++)
            {
                if (m_aatUsedMpuChn[byPos][byIdx].IsNull())
                {
                    byFstIdlePos = byFstIdlePos == 0xff ? byIdx : byFstIdlePos;
                }
                if (dwMpuIp == m_aatUsedMpuChn[byPos][byIdx].GetIpAddr() &&
                    byChnId == m_aatUsedMpuChn[byPos][byIdx].GetChnId())
                {
                    bExist = TRUE;
                    break;
                }
            }
            if (!bExist)
            {
                if (0xff == byFstIdlePos)
                {
                    OspPrintf(TRUE, FALSE, "[TMcuHdBasStatus][SetConfMpuChn] no pos in ConfPos.%d\n", byPos);
                    return;
                }
                m_aatUsedMpuChn[byPos][byFstIdlePos].SetIpAddr(dwMpuIp);
                m_aatUsedMpuChn[byPos][byFstIdlePos].SetChnId(byChnId);
            }
            return;
        }

        BOOL32 bSet = FALSE;
        for (byIdx = 0; byIdx < MAXNUM_ONGO_CONF; byIdx ++)
        {
            if (m_acConfId[byIdx].IsNull())
            {
                m_acConfId[byIdx] = cConfId;
                m_aatUsedMpuChn[byIdx][0].SetIpAddr(dwMpuIp);
                m_aatUsedMpuChn[byIdx][0].SetChnId(byChnId);
                bSet = TRUE;
            }
        }
        if (!bSet)
        {
            OspPrintf(TRUE, FALSE, "[TMcuHdBasStatus][SetConfMpuChn] no pos for new conf!\n");
        }
        return;
    }

    void SetIdleMau(u32 dwMauIp, BOOL32 bH263p)
    {
        u8 byFstIdlePos = 0xff;
        BOOL32 bExist = FALSE;
        u8 byIdx = 0;
        if (!bH263p)
        {
            for(byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx++)
            {
                if (0 == m_adwIdleMau[byIdx])
                {
                    byFstIdlePos = byFstIdlePos == 0xff ? byIdx : byFstIdlePos;
                }
                if (dwMauIp == m_adwIdleMau[byIdx])
                {
                    bExist = TRUE;
                    break;
                }
            }
            if (!bExist)
            {
                if (0xff == byFstIdlePos)
                {
                    OspPrintf(TRUE, FALSE, "[TMcuHdBasStatus][SetIdleMau] no pos for new mau!\n");
                    return;
                }
                m_adwIdleMau[byFstIdlePos] = dwMauIp;
            }
            return;
        }
        
        byFstIdlePos = 0xff;
        bExist = FALSE;
        for(byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx++)
        {
            if (0 == m_adwIdleH263pMau[byIdx])
            {
                byFstIdlePos = byFstIdlePos == 0xff ? byIdx : byFstIdlePos;
            }
            if (dwMauIp == m_adwIdleH263pMau[byIdx])
            {
                bExist = TRUE;
                break;
            }
        }
        if (!bExist)
        {
            if (0xff == byFstIdlePos)
            {
                OspPrintf(TRUE, FALSE, "[TMcuHdBasStatus][SetIdleMau] no pos for new mau(H263p)!\n");
                return;
            }
            m_adwIdleH263pMau[byFstIdlePos] = dwMauIp;
        }
        return;
    }

    void SetIdleMpuChn(u32 dwMpuIp, u8 byChnId)
    {
        u8 byFstIdlePos = 0xff;
        BOOL32 bExist = FALSE;
        u8 byIdx = 0;

        for(byIdx = 0; byIdx < MAXNUM_PERIEQP*MAXNUM_MPU_CHN; byIdx++)
        {
            if (m_atIdleMpuChn[byIdx].IsNull())
            {
                byFstIdlePos = byFstIdlePos == 0xff ? byIdx : byFstIdlePos;
            }
            if (dwMpuIp == m_atIdleMpuChn[byIdx].GetIpAddr() &&
                byChnId == m_atIdleMpuChn[byIdx].GetChnId() )
            {
                bExist = TRUE;
                break;
            }
        }
        if (!bExist)
        {
            if (0xff == byFstIdlePos)
            {
                OspPrintf(TRUE, FALSE, "[TMcuHdBasStatus][SetIdleMpuChn] no pos for new mau!\n");
                return;
            }
            m_atIdleMpuChn[byFstIdlePos].SetIpAddr(dwMpuIp);
            m_atIdleMpuChn[byFstIdlePos].SetChnId(byChnId);
        }
        return;
    }


    void Clear(void)
    {
        memset(m_acConfId, 0, sizeof(m_acConfId));
        memset(m_aadwUsedMau, 0, sizeof(m_aadwUsedMau));
        memset(m_adwIdleMau, 0, sizeof(m_adwIdleMau));
        memset(m_aadwUsedH263pMau, 0, sizeof(m_aadwUsedH263pMau));
        memset(m_adwIdleH263pMau, 0, sizeof(m_adwIdleH263pMau));
        m_byCurMau = 0;
        m_byCurH263pMau = 0;
    }

private:
    BOOL32 IsConfExist(const CConfId &cConfId, u8 &byPos)
    {
        byPos = 0xff;
        BOOL32 bExist = FALSE;
        for(u8 byIdx = 0; byIdx < MAXNUM_ONGO_CONF; byIdx ++)
        {
            if (m_acConfId[byIdx].IsNull())
            {
                continue;
            }
            if (m_acConfId[byIdx] == cConfId)
            {
                bExist = TRUE;
                byPos = byIdx;
                break;
            }
        }
        return bExist;
    }

private:
    CConfId m_acConfId[MAXNUM_ONGO_CONF];

    u32     m_aadwUsedMau[MAXNUM_ONGO_CONF][MAXNUM_CONF_MAU];
    u32     m_aadwUsedH263pMau[MAXNUM_ONGO_CONF][MAXNUM_CONF_MAU];
    TMpuChn m_aatUsedMpuChn[MAXNUM_ONGO_CONF][MAXNUM_CONF_MPU];
    
    u32     m_adwIdleMau[MAXNUM_PERIEQP];
    u32     m_adwIdleH263pMau[MAXNUM_PERIEQP];
    TMpuChn m_atIdleMpuChn[MAXNUM_PERIEQP*MAXNUM_MPU_CHN];

    u8      m_byCurMau;
    u8      m_byCurH263pMau;
    u8      m_byCurMpuChn;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*****************************************************************
 *
 *
 *                         �û�������չ
 *
 *
 *****************************************************************/
class CExUsrInfo: public CUserFullInfo
{
public:
    CExUsrInfo() {};
	~CExUsrInfo() {};

    CExUsrInfo(const CExUsrInfo &newInfo)
    {
        memcpy( this, &newInfo, sizeof(CUserFullInfo) );
    }

    CExUsrInfo(const CUserFullInfo &newInfo)
    {
        memcpy( this, &newInfo, sizeof(CUserFullInfo) );
    }

    const CExUsrInfo& operator=(const CExUsrInfo& newInfo)
    {
        if (this == &newInfo)
        {
            return *this;
        }

        memcpy( this, &newInfo, sizeof(CExUsrInfo) );

        return (*this);
    }

    BOOL32 operator == (const CExUsrInfo& newInfo)
    {
        return ( memcmp(this, &newInfo, sizeof(CExUsrInfo)) == 0 );
    }

    // ���ص��û���������Id������MCS����ʹ�������ṩ�ķ���

	//�õ��û�����
	char* GetDiscription()
	{
		return (discription+1);
	}
	
	//�����û�����
	void SetDiscription(char* buf)
	{
		if(buf == NULL)
        {
            return;
        }
        u8 byGrpId = discription[0];
		memset(discription, 0, 2 * MAX_CHARLENGTH);

        s32 length = (strlen(buf) >= 2*MAX_CHARLENGTH - 2 ? 2*MAX_CHARLENGTH - 2: strlen(buf));
		strncpy(discription+1, buf, length);

		discription[2*MAX_CHARLENGTH-1] = '\0';
        discription[0] = byGrpId;
	}
    void SetUsrGrpId( u8 byGrdId )
    {
        discription[0] = byGrdId;
    }
    u8 GetUsrGrpId()
    {
        return discription[0];
    }

    void Print( void )
    {
        StaticLog( "name:%s actor:%d fullname:%s des:%s\n",
                  GetName(), GetActor(), GetFullName(), GetDiscription());
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


// Ip��ַ�Σ�������
struct TIpSeg
{    
public:
	u32 dwIpStart;
	u32 dwIpEnd;

    u8  byEnabled;

public:
    TIpSeg()
    {
        SetNull();
    }

    // �����ַҪ��Ϊ������
    void SetSeg( u32 dwStart, u32 dwEnd)    
    {
        byEnabled = 1;
        dwIpStart = dwStart;
        dwIpEnd = dwEnd;
    }
    BOOL32 IsEnabled() const
    {
        return ( byEnabled == 1 ? TRUE : FALSE);
    }

    // ���ص�ַΪ������
    BOOL32 GetSeg ( u32& dwStart, u32& dwEnd ) const 
    {
        dwStart = dwIpStart;
        dwEnd   = dwIpEnd;
        return IsEnabled();
    }
    void SetNull()
    {
        memset(this, 0, sizeof(TIpSeg));
    }

    // ĳ��Ip�Ƿ��ڱ����ڡ���ַҪ����������
    BOOL32 IsIpIn(u32 dwIp) const 
    {
        u32 dwMin = dwIpStart;
        u32 dwMax = dwIpEnd;
        u32 dwTmp = 0;
        if (dwMin > dwMax)
        {
            dwTmp = dwMin;
            dwMin = dwMax;
            dwMax = dwTmp;
        }
        if (dwIp >= dwMin && dwIp <= dwMax )    
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    // ����һ��Ip���Ƿ��ڱ�����
    BOOL32 IsIpSegIn( const TIpSeg tAnother ) const
    {
        return ( IsIpIn(tAnother.dwIpStart)  &&
                 IsIpIn(tAnother.dwIpEnd) );
    }
}

#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/* add by wangliang 2006-12-20 for E164  start */
/*
// E164�����
struct TE164Seg
{
public:
    s8 szE164Start[MAXLEN_E164+1];
    s8 szE164End[MAXLEN_E164+1];
    u8 byEnabled;

public:
    TE164Seg()
    {
        SetNull();
    }

    // ȡ��E164�����
    void SetSeg(LPCSTR pszStart, LPCSTR pszEnd)
    {
        byEnabled = 1;
        if(pszStart != NULL && pszEnd != NULL)
        {
            memset(szE164Start, 0, sizeof(szE164Start));
            memset(szE164End, 0, sizeof(szE164End));
            strncpy(szE164Start, pszStart, MAXLEN_E164);
            strncpy(szE164End, pszEnd, MAXLEN_E164);
        }
        return;
    }

    BOOL32 IsEnabled() const
    {
        return (byEnabled == 1 ? TRUE : FALSE);
    }

    // ����E164�����
    BOOL32 GetSeg(s8 *pszStart, s8 *pszEnd) const 
    {
        strncpy(pszStart, szE164Start, MAXLEN_E164);
        strncpy(pszEnd, szE164End, MAXLEN_E164);
        return IsEnabled();
    }

    // �ṹ�ÿ�
    void SetNull()
    {
        memset(this, 0, sizeof(TE164Seg));
    }

    // E164�����Ƿ��ڱ�E164�������
    // FIXME: ���pszE164��szE164Start��szE164End���Ȳ�ͬ�����ܹ��ϸ��ж��Ƿ��ڶ���
    BOOL32 IsE164In(LPCSTR pszE164) const
    {
        if(strncmp(szE164Start, szE164End, MAXLEN_E164) < 0)
        {
            if(strncmp(pszE164, szE164Start, MAXLEN_E164) >= 0 
                && strncmp(pszE164, szE164End, MAXLEN_E164) <= 0)
                return TRUE;
            else
                return FALSE;
        }
        else
        {
            if(strncmp(pszE164, szE164End, MAXLEN_E164) >= 0 
                && strncmp(pszE164, szE164Start, MAXLEN_E164) <= 0)
                return TRUE;
            else
                return FALSE;
        }  
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
*/
/* add by wangliang 2006-12-20 for E164  end */

// �û�����Ϣ
struct TUsrGrpInfo
{
public:
    TUsrGrpInfo()
    {
        SetNull();
    }

public:
    void SetNull()
    {
        memset ( this, 0, sizeof(TUsrGrpInfo) );
        SetBanned(TRUE);    // Ĭ���ǽ�ֹ
    }
    // �жϱ��û����Ƿ�Ϊ��
    BOOL32 IsFree() const
    {
        return (byGrpId == 0);
    }
    // ����/��ȡ�û���ID
    void SetUsrGrpId(u8 byId)
    {
        byGrpId = byId;
    }
    u8 GetUsrGrpId() const
    {
        return byGrpId;
    }
    // ����/��ȡ�û�����
    void SetUsrGrpName(LPCSTR lpszName)
    {
        memset(szName, 0, MAX_CHARLENGTH);
        strncpy(szName, lpszName, MAX_CHARLENGTH-1);
    }
    LPCSTR GetUsrGrpName() const
    {
        return szName;
    }
    // ����/��ȡ�û�������
    void SetUsrGrpDesc(LPCSTR lpszDesc)
    {
        memset(szDesc, 0, 2*MAX_CHARLENGTH);
        strncpy(szDesc, lpszDesc, 2*MAX_CHARLENGTH-1);
    }
    LPCSTR GetUsrGrpDesc() const
    {
        return szDesc;
    }
    void SetMaxUsrNum(u8 byMaxNum)
    {
        byMaxUsrNum = byMaxNum;
    }
    u8 GetMaxUsrNum() const
    {
        return byMaxUsrNum;
    }
    void SetBanned(BOOL32 bIsBanned)
    {
        bBanned = bIsBanned ? 1 : 0;
    }
    BOOL32 IsBanned() const
    {
        return bBanned ? TRUE : FALSE;
    }

    // �ж��ն�IP�Ƿ��ںϷ����ڣ�
    // �����ַ��������
    inline BOOL32 IsMtIpInAllowSeg(u32 dwIp) const;

    void Print() const
    {
        if ( IsFree() )
        {            
            StaticLog( "\tGroup ID: FREE\n" );
            return;
        }        
        StaticLog( "\tGroup ID: %d\n", byGrpId );
        StaticLog( "\tName: %s\n", szName);
        StaticLog( "\tDesc: %s\n", szDesc);
        StaticLog( "\tMax User: %d\n", byMaxUsrNum);

        StaticLog( "\tIP Ranges: %s\n", bBanned ? "Banned" : "Allowed");
        for (s32 nLoop = 0; nLoop < MAXNUM_IPSEG; nLoop ++)
        {
            StaticLog( "\t\tNo.%d: Enabled: %d IP: 0x%x -- 0x%x\n", 
                       nLoop,
                       atIpSeg[nLoop].IsEnabled(), 
                       atIpSeg[nLoop].dwIpStart, 
                       atIpSeg[nLoop].dwIpEnd );
        }        
    }

protected:
	u8      byGrpId;
	s8      szName[MAX_CHARLENGTH];
	s8      szDesc[2*MAX_CHARLENGTH];
	u8      byMaxUsrNum;
public:
    TIpSeg  atIpSeg[MAXNUM_IPSEG];  // ��ַ��������
    u8      bBanned;     // 0 - ��ʾָ����������, 1 - ��ʾ�ǽ�ֹ
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// VCS�û���Ϣ�������˲�������ϯ�������
class CVCSUsrInfo: public CExUsrInfo
{
public:
	CVCSUsrInfo()
	{
		SetNull();
	}

    ~CVCSUsrInfo() {};

    void SetNull()
	{
		m_wTaskNum = 0;
		memset(m_cTaskID, 0, sizeof(CConfId) * MAXNUM_MCU_VCCONF);
	}

	// ָ�����û��ĵ�������
	BOOL AssignTask(u16 wTaskNum, const CConfId* pConfID)
	{
		if (pConfID == NULL)
		{
			return FALSE;
		}

		SetNull();
		m_wTaskNum = htons(wTaskNum);
		memcpy(m_cTaskID, pConfID, sizeof(CConfId) * wTaskNum);
		return TRUE;
	}

	// ��ȡ���û��ĵ�������
	const CConfId* GetTask() const
	{
		return m_cTaskID;
	}
	u16 GetTaskNum() const
	{
		return ntohs(m_wTaskNum);
	}

	// �����Ƿ���ָ������Χ��
	BOOL IsYourTask(CConfId cTaskID)
	{
		BOOL dwIsYourTask = FALSE;
		for (u16 wIndex = 0; wIndex < MAXNUM_MCU_VCCONF; wIndex++)
		{
			if (cTaskID == m_cTaskID[wIndex])
			{
				dwIsYourTask = TRUE;
				break;
			}
		}
		return dwIsYourTask;
	}

protected:
    u16     m_wTaskNum;
	CConfId m_cTaskID[ MAXNUM_MCU_VCCONF ];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*-------------------------------------------------------------------
                               CServMsg                              
--------------------------------------------------------------------*/

/*====================================================================
    ������      ��CServMsg
    ����        ��constructor
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/15    1.0         LI Yi         ����
====================================================================*/
inline CServMsg::CServMsg( void )
{
	Init();
}

/*====================================================================
    ������      ��CServMsg
    ����        ��constructor
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 * const pbyMsg, Ҫ��ֵ����Ϣָ��
				  u16 wMsgLen, Ҫ��ֵ����Ϣ���ȣ�������ڵ��� SERV_MSGHEAD_LEN
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/15    1.0         LI Yi         ����
====================================================================*/
inline CServMsg::CServMsg( u8 * const pbyMsg, u16 wMsgLen )
{
	Init();
	
	if( wMsgLen < SERV_MSGHEAD_LEN || pbyMsg == NULL )
		return;
	
	wMsgLen = min( wMsgLen, SERV_MSG_LEN );
	memcpy( this, pbyMsg, wMsgLen );
	
	//set length
	SetMsgBodyLen( wMsgLen - SERV_MSGHEAD_LEN );
}

/*====================================================================
    ������      ��~CServMsg
    ����        ��distructor
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/15    1.0         LI Yi         ����
====================================================================*/
inline CServMsg::~CServMsg( void )
{

}

/*====================================================================
    ������      ��Init
    ����        ��BUFFER��ʼ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/15    1.0         LI Yi         ����
====================================================================*/
inline void CServMsg::Init( void )
{
	memset( this, 0, SERV_MSG_LEN );	//����
}

/*====================================================================
    ������      ��ClearHdr
    ����        ����Ϣͷ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/15    1.0         LI Yi         ����
====================================================================*/
inline void CServMsg::ClearHdr( void )
{
	u16		wBodyLen = GetMsgBodyLen();

	memset( this, 0, SERV_MSGHEAD_LEN );	//����
	SetMsgBodyLen( wBodyLen );
}

/*====================================================================
    ������      ��GetConfId
    ����        ����ȡ�������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ������ţ�ȫ0��ʾ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/15    1.0         LI Yi         ����
    02/12/24    1.0         LI Yi         �޸Ľӿ�
====================================================================*/
inline CConfId CServMsg::GetConfId( void ) const
{
	return( m_cConfId );
}

/*====================================================================
    ������      ��SetConfId
    ����        �����û������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CConfId & cConfId������ţ�ȫ0��ʾ����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/15    1.0         LI Yi         ����
    02/12/24    1.0         LI Yi         �޸Ľӿ�
====================================================================*/
inline void CServMsg::SetConfId( const CConfId & cConfId )
{
	m_cConfId = cConfId;
}

/*====================================================================
    ������      ��SetNullConfId
    ����        �����û������ϢΪ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/06    1.0         LI Yi         ����
====================================================================*/
inline void CServMsg::SetNullConfId( void )
{
	CConfId		cConfId;
	
	cConfId.SetNull();
	SetConfId( cConfId );
}

/*====================================================================
    ������      ��GetMsgBodyLen
    ����        ����ȡ��Ϣ�峤����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����Ϣ�峤��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/15    1.0         LI Yi         ����
====================================================================*/
inline u16 CServMsg::GetMsgBodyLen( void ) const
{
	return( ntohs( m_wMsgBodyLen ) );
}

/*====================================================================
    ������      ��SetMsgBodyLen
    ����        ��������Ϣ�峤����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wMsgBodyLen, ��Ϣ�峤��
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/15    1.0         LI Yi         ����
====================================================================*/
inline void CServMsg::SetMsgBodyLen( u16 wMsgBodyLen )
{
	m_wMsgBodyLen = htons( wMsgBodyLen );
}

/*====================================================================
    ������      ��GetMsgBody
    ����        ����ȡ��Ϣ��ָ�룬�û������ṩBUFFER
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��u8 * constָ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/15    1.0         LI Yi         ����
====================================================================*/
inline u8 * const CServMsg::GetMsgBody( void ) const
{
	return( ( u8 * const )m_abyMsgBody );
}

/*====================================================================
    ������      ��GetMsgBodyLen
    ����        ����ȡ������Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��������Ϣ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/26    1.0         LI Yi         ����
====================================================================*/
inline u16 CServMsg::GetServMsgLen( void ) const
{
	return( GetMsgBodyLen() + SERV_MSGHEAD_LEN );
}

/*====================================================================
    ������      ��GetMsgBody
    ����        ����ȡ��Ϣ�壬���û�����BUFFER�������С���ضϴ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 * pbyMsgBodyBuf, ���ص���Ϣ��
				  u16 wBufLen, BUFFER��С
    ����ֵ˵��  ��ʵ�ʷ��ص���Ϣ�峤��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/15    1.0         LI Yi         ����
====================================================================*/
inline u16 CServMsg::GetMsgBody( u8 * pbyMsgBodyBuf, u16 wBufLen ) const
{
    u16 wActLen = min( GetMsgBodyLen(), wBufLen );
	memcpy( pbyMsgBodyBuf, m_abyMsgBody, wActLen );
	return wActLen;
}

/*====================================================================
    ������      ��SetMsgBody
    ����        ��������Ϣ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 * const pbyMsgBody, �������Ϣ�壬ȱʡΪNULL
				  u16 wLen, �������Ϣ�峤�ȣ�ȱʡΪ0
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/15    1.0         LI Yi         ����
====================================================================*/
inline void CServMsg::SetMsgBody( u8 * const pbyMsgBody, u16 wLen )
{
    /*
    if (pbyMsgBody == NULL || wLen == 0)
    {
        SetMsgBodyLen( 0 );
        return;
    }
    */
	wLen = min( wLen, SERV_MSG_LEN - SERV_MSGHEAD_LEN );
	memcpy( m_abyMsgBody, pbyMsgBody, wLen );
	SetMsgBodyLen( wLen );
}

/*====================================================================
    ������      ��CatMsgBody
    ����        �������Ϣ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 * const pbyMsgBody, �������Ϣ�壬ȱʡΪNULL
				  u16 wLen, �������Ϣ�峤�ȣ�ȱʡΪ0
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/11/07    1.0         Liaoweijiang  ����
====================================================================*/
inline void CServMsg::CatMsgBody( u8 * const pbyMsgBody, u16 wLen )
{
    /*
    if (pbyMsgBody == NULL || wLen == 0)
    {
        return;
    }
    */
	wLen = min( wLen, SERV_MSG_LEN - SERV_MSGHEAD_LEN - GetMsgBodyLen() );
	memcpy( m_abyMsgBody + GetMsgBodyLen(), pbyMsgBody, wLen );
	SetMsgBodyLen( GetMsgBodyLen() + wLen );
}

/*====================================================================
    ������      ��GetMsgBody
    ����        ����ȡ������Ϣָ�룬�û������ṩBUFFER
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��u8 * constָ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/26    1.0         LI Yi         ����
====================================================================*/
inline u8 * const CServMsg::GetServMsg( void ) const
{
	return( ( u8 * const )( this ) );
}

/*====================================================================
    ������      ��GetServMsg
    ����        ����ȡ������Ϣ�����û�����BUFFER�������С���ضϴ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 * pbyMsgBuf, ���ص���Ϣ
				  u16 wBufLen, BUFFER��С
    ����ֵ˵��  ��ʵ�ʷ��ص���Ϣ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/15    1.0         LI Yi         ����
====================================================================*/
inline u16 CServMsg::GetServMsg( u8 * pbyMsgBuf, u16 wBufLen ) const
{
	wBufLen = min(SERV_MSG_LEN,wBufLen);
	memcpy( pbyMsgBuf, this, wBufLen );
	return( min( GetMsgBodyLen() + SERV_MSGHEAD_LEN, wBufLen ) );
}

/*====================================================================
    ������      ��SetServMsg
    ����        ������������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 * const pbyMsg, �����������Ϣ
				  u16 wMsgLen, �������Ϣ����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/15    1.0         LI Yi         ����
====================================================================*/
inline void CServMsg::SetServMsg( u8 * const pbyMsg, u16 wMsgLen )
{
	if( wMsgLen < SERV_MSGHEAD_LEN )
	{
		OspPrintf( TRUE, FALSE, "CServMsg: SetServMsg() Exception -- invalid MsgLen!\n" );
		return;
	}

	wMsgLen = min( wMsgLen, SERV_MSG_LEN );
	memcpy( this, pbyMsg, wMsgLen );
	SetMsgBodyLen( wMsgLen - SERV_MSGHEAD_LEN );
}

/*====================================================================
    ������      ��operator=
    ����        ������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CServMsg & cServMsg, ��ֵ����Ϣ����
    ����ֵ˵��  ��CServMsg����Ӧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/26    1.0         LI Yi         ����
====================================================================*/
inline const CServMsg & CServMsg::operator= ( const CServMsg & cServMsg )
{
	u16	wLen = cServMsg.GetServMsgLen();

	memcpy( this, cServMsg.GetServMsg(), wLen );
	return( *this );
}

/*-------------------------------------------------------------------
                               TConfMtInfo                           
--------------------------------------------------------------------*/

/*====================================================================
    ������      ��TConfMtInfo
    ����        ��constructor
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/30    1.0         LI Yi         ����
====================================================================*/
inline TConfMtInfo::TConfMtInfo( void )
{
	memset( this, 0, sizeof( TConfMtInfo ) );
	
	// [12/30/2010 liuxu] Ӧ�û���Ҫ������ΪNULL
	SetNull();
}

/*====================================================================
    ������      ��MtInConf
    ����        ���жϸ�MCU��ĳ�ն��Ƿ��������ն��б���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/30    1.0         LI Yi         ����
====================================================================*/
inline BOOL TConfMtInfo::MtInConf( const u8 byMtId ) const
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT)return FALSE;
	return ( ( m_abyMt[(byMtId-1) / 8] & ( 1 << ( (byMtId-1) % 8 ) ) ) != 0 );
}

/*====================================================================
    ������      ��MtJoinedConf
    ����        ���жϸ�MCU��ĳ�ն��Ƿ�������ն��б���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ��TRUE/FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/24    1.0         LI Yi         ����
====================================================================*/
inline BOOL TConfMtInfo::MtJoinedConf( const u8 byMtId ) const
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT) return FALSE;
	return ( ( m_abyJoinedMt[(byMtId-1) / 8] & ( 1 << ( (byMtId-1) % 8 ) ) ) != 0 );
}

/*====================================================================
    ������      ��AddMt
    ����        �����Ӹ�MCU���ն��������ն��б���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/30    1.0         LI Yi         ����
====================================================================*/
inline void TConfMtInfo::AddMt( const u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT)return;
	m_abyMt[(byMtId-1) / 8] |= 1 << ( (byMtId-1) % 8 );
}

/*====================================================================
    ������      ��AddJoinedMt
    ����        �����Ӹ�MCU���ն�������ն��б��У��ú����Զ������ն�Ҳ
	              ���뵽�����ն��б���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/25    1.0         LI Yi         ����
====================================================================*/
inline void TConfMtInfo::AddJoinedMt( const u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT)
		return;

	m_abyJoinedMt[(byMtId-1) / 8] |= 1 << ( (byMtId-1) % 8 );
	
	m_abyMt[(byMtId-1) / 8] |= 1 << ( (byMtId-1) % 8 );
}

/*====================================================================
    ������      ��RemoveMt
    ����        ��ɾ����MCU���ն��������ն��б��У��ú����Զ������ն�Ҳ
	              ɾ��������ն��б���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/30    1.0         LI Yi         ����
====================================================================*/
inline void TConfMtInfo::RemoveMt( const u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT)return;
	m_abyMt[(byMtId-1) / 8] &= ~( 1 << ( (byMtId-1) % 8 ) );
	m_abyJoinedMt[(byMtId-1) / 8] &= ~( 1 << ( (byMtId-1) % 8 ) );
}

/*====================================================================
    ������      ��RemoveJoinedMt
    ����        ��ɾ����MCU���ն�������ն��б���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/25    1.0         LI Yi         ����
====================================================================*/
inline void TConfMtInfo::RemoveJoinedMt(const u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT)return;
	m_abyJoinedMt[(byMtId-1) / 8] &= ~( 1 << ( (byMtId-1) % 8 ) );
}

/*====================================================================
    ������      ��RemoveAllMt
    ����        ��ɾ����MCU�������ն��������ն��б��У��ú����Զ��������ն�Ҳ
	              ɾ��������ն��б���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/30    1.0         LI Yi         ����
====================================================================*/
inline void TConfMtInfo::RemoveAllMt( void )
{
	memset( m_abyMt, 0, sizeof( m_abyMt ) );
	memset( m_abyJoinedMt, 0, sizeof( m_abyMt ) );
}

/*====================================================================
    ������      ��RemoveAllJoinedMt
    ����        ��ɾ����MCU�������ն�������ն��б���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/25    1.0         LI Yi         ����
====================================================================*/
inline void TConfMtInfo::RemoveAllJoinedMt( void )
{
	memset( m_abyJoinedMt, 0, sizeof( m_abyMt ) );
}

/*====================================================================
    ������      ��GetAllMtNum
    ����        ���õ�һ���ṹ�����������ն˸���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  �������������ն˸���
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/02    1.0         LI Yi         ����
====================================================================*/
inline u8 TConfMtInfo::GetAllMtNum( void ) const
{
	u8	byMtNum = 0;

	for( u8 byLoop = 0; byLoop < MAXNUM_CONF_MT; byLoop++ )
	{
		if( MtInConf( byLoop + 1 ) )
		{
			byMtNum++;
		}
	}

	return( byMtNum );
}

/*====================================================================
    ������      ��GetLocalJoinedMtNum
    ����        ���õ�һ���ṹ����������ն˸���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ������������ն˸���
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/25    1.0         LI Yi         ����
====================================================================*/
inline u8 TConfMtInfo::GetAllJoinedMtNum( void ) const
{
	u8	byMtNum = 0;

	for( u8 byLoop = 0; byLoop < MAXNUM_CONF_MT; byLoop++ )
	{
		if( MtJoinedConf( byLoop + 1 ) )
		{
			byMtNum++;
		}
	}

	return( byMtNum );
}


/*====================================================================
    ������      ��GetLocalUnjoinedMtNum
    ����        ���õ�һ���ṹ������δ����ն˸���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��������δ����ն˸���
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/25    1.0         LI Yi         ����
====================================================================*/
inline u8 TConfMtInfo::GetAllUnjoinedMtNum( void ) const
{
	return( GetAllMtNum() - GetAllJoinedMtNum() );
}

/*==============================================================================
������    :  GetMaxMtIdInConf
����      :  �õ�һ���ṹ�������ն���MTId����Idֵ
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2008-12-24					Ѧ��							����
==============================================================================*/
inline u8 TConfMtInfo::GetMaxMtIdInConf( void ) const
{
	u8	byMaxMtId= 0;
	
	for( u8 byLoop = 0; byLoop < MAXNUM_CONF_MT; byLoop++ )
	{
		if( MtInConf( byLoop + 1 ) )
		{
			byMaxMtId = byLoop + 1;
		}
	}
	
	return( byMaxMtId );
}

/*-------------------------------------------------------------------
                           TConfAllMcuInfo                          
--------------------------------------------------------------------*/
/*====================================================================
    ������      ��IsMcuIdAdded
    ����        ���Ƿ��Ѿ����mcu��Ϣ, ����192���ܵ��ô˽ӿ�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[in]pbyMcuId, ����,
				  [in]byCount, pbyMcuId��ֵ�ĸ���, ��ֵ��ΧΪ[0, MAX_SUBMCU_DEPTH]
    ����ֵ˵��  �����ڷ���TRUE,�����ڷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    10/07/08    4.6         �ܾ���         ����
====================================================================*/
inline BOOL32 TConfAllMcuInfo::IsMcuIdAdded( const u8* const pabyMcuId, const u8& byMcuIdCount)
{
	u16 wMcuIdx = FindMcuIdx(pabyMcuId, byMcuIdCount);

	ISVALIDMCUID(wMcuIdx);

	return TRUE;
}

/*====================================================================
    ������      ��AddMcu
    ����        ������mcuid ���һ��mcu
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[in]pabyMcuId, ����,
				  [in]byMcuIdCount, pabyMcuId��ֵ�ĸ���, ��ֵ��ΧΪ(0, MAX_SUBMCU_DEPTH]
				  [out]pwMcuIdx, ��mcu��ӵ�indexֵ
    ����ֵ˵��  ���ɹ�����TRUE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    10/07/08    4.6         �ܾ���         ����
	10/12/15    4.6          ����          �߶�
====================================================================*/
inline BOOL32 TConfAllMcuInfo::AddMcu( const u8* const pabyMcuId, 
									  const u8& byMcuIdCount, 
									  u16* const pwMcuIdx)
{
	// ������֤
	if (!pabyMcuId								// pbyMcuId����Ϊ��
		|| 0 == byMcuIdCount					// byCount����Ϊ0
		|| byMcuIdCount > MAX_SUBMCU_DEPTH      // byCount���ܳ���MAX_SUBMCU_DEPTH
		|| NULL == pwMcuIdx						// pwMcuIdx�ǿ�
		)
	{
		return FALSE;
	}

	*pwMcuIdx = INVALID_MCUIDX;					// ������wMcuIdx��ʼ��Ϊ�Ƿ�ֵ

	// m_abyMcuInfo���Ѿ�����
	if (MAXNUM_CONFSUB_MCU <= GetMcuCount())
	{
		return FALSE;
	}

	u16 wFindRet = FindMcuIdx(pabyMcuId, byMcuIdCount, pwMcuIdx);
	if (wFindRet < MAXNUM_CONFSUB_MCU)
	{
		// �Ѿ�����, ֱ�ӷ���TRUE
		*pwMcuIdx = wFindRet;
		return TRUE;
	}else
	{
		// wMcuIdxΪ��һ�����е�λ��, Add Mcu�ڴ���λ�ò���
		if (*pwMcuIdx < MAXNUM_CONFSUB_MCU && !IsLocalMcuId(*pwMcuIdx))
		{
			// �Ƚ�m_abyMcuInfo[wMcuIdx]����, ����ǰ��ʹ�ú���������
			memset(m_abyMcuInfo[*pwMcuIdx], 0, MAX_SUBMCU_DEPTH);

			// �ٿ�������
			memcpy(m_abyMcuInfo[*pwMcuIdx], pabyMcuId, byMcuIdCount);		// ��ֵ
			SetMcuCount(GetMcuCount() + 1);									// ������һ
			return TRUE;
		}

		// m_abyMcuInfo���Ѿ�����, ����������µ��¼�mcu. 
		OspPrintf(TRUE, TRUE, "[Error][AddMcu] m_abyMcuInfo is full now ,but m_wSum is %d\n", GetMcuCount());
	}

	return FALSE;
}

/*====================================================================
    ������      ��RemoveMcu
    ����        ������MCU indexɾ��MCU
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[in]pwMcuIdx, �ҵ����¼�mcu��idx
    ����ֵ˵��  ���ɹ�����TRUE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    10/08/28    4.6         �ܾ���         ����
	10/12/15    4.6         ����           �߶�
====================================================================*/
inline BOOL32 TConfAllMcuInfo::RemoveMcu( const u16& wMcuIdx )
{
	ISVALIDMCUID(wMcuIdx);

	// �������ǿյ�, ֱ�ӷ���TRUE
	if (IsNull(wMcuIdx))
	{
		return TRUE;
	}

	if (IsLocalMcuId(wMcuIdx))								// ĿǰTConfAllMcuInfo��֧�ֹ�����mcu
	{
		return TRUE;
	}

	// mcu id ���
	memset( &m_abyMcuInfo[wMcuIdx][0], 0, sizeof(m_abyMcuInfo[wMcuIdx]) );
	SetMcuCount(GetMcuCount() - 1);							// ������һ

	return TRUE;
}


/*====================================================================
    ������      ��GetMcuIdxByMcuId
    ����        �������ϼ�mcu��mcu id�ҵ����������е��¼�mcu��idx
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[in]pabyMcuId, ����,
				  [in]byMcuIdCount, pabyMcuId��ֵ�ĸ���, ��ֵ��ΧΪ[0, MAX_SUBMCU_DEPTH]
				  [out]pawMcuIdx, ����, �ҵ����¼�mcu��idx
				  [in]byMcuIdxCount, �����pawMcuIdx����ĳ���
    ����ֵ˵��  ���ҵ����¼�mcu������, û�ҵ�����0
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    10/07/08    4.6         �ܾ���         ����
	10/12/15    4.6          ����          �߶�
====================================================================*/
inline u8 TConfAllMcuInfo::GetMcuIdxByMcuId( const u8* pabyMcuId, 
						 const u8& byMcuIdCount, 
						 u16* const pawMcuIdx, 
						 const u8 byMcuIdxCount/* = MAXNUM_SUB_MCU*/)
{
	// ������֤
	if (!pabyMcuId								// pbyMcuId����Ϊ��
		|| 0 == byMcuIdCount					// byCount����Ϊ0
		|| byMcuIdCount > MAX_SUBMCU_DEPTH      // byCount���ܳ���MAX_SUBMCU_DEPTH
		|| !pawMcuIdx							// pwMcuIdx����Ϊ��
		|| 0 == byMcuIdxCount)					// byMcuIdxCount����Ϊ0
	{
		return 0;
	}

	// ������pawMcuIdx����Ϊ�Ƿ�ֵ
	memset(pawMcuIdx, INVALID_MCUIDX, byMcuIdxCount);

	// ����Ĵ����ҵ�mcuΪ���¼�mcu, ֱ�ӷ���
	if (MAX_SUBMCU_DEPTH == byMcuIdCount)
	{
		return 0;
	}

	// ��byMcuIdxCount, MAXNUM_SUB_MCU, m_wSum����֮���ҳ���Сֵ
	u16 wSum = min(byMcuIdxCount, MAXNUM_SUB_MCU);
	wSum = min(wSum, GetMcuCount());

	u8 byFindNum = 0;							// ���ҵ����¼�mcu������
	// ��������mcu�б�, �ҳ����wSum���¼�mcu
	for(u16 wIndex = 0; wIndex < MAXNUM_CONFSUB_MCU && byFindNum < wSum; wIndex++)
	{
		if (E_CMP_RESULT_DOWN == CompareByMcuIds(wIndex, pabyMcuId, byMcuIdCount))
		{
			pawMcuIdx[byFindNum++] = wIndex;
		}
	}

	return byFindNum;
}


/*====================================================================
    ������      ��GetIdxByMcuId
    ����        ��ͨ��mcuid���������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[in]pabyMcuId, ����,
				  [in]byMcuIdCount, pabyMcuId��ֵ�ĸ���, ��ֵ��ΧΪ(0, MAX_SUBMCU_DEPTH]
				  [out]pwMcuIdx, �ҵ����¼�mcu��idx
    ����ֵ˵��  ���ҵ�����TRUE, ����FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    10/07/08    4.6         �ܾ���         ����
====================================================================*/
inline BOOL32 TConfAllMcuInfo::GetIdxByMcuId(const u8* pabyMcuId, const u8& byMcuIdCount, u16* const pwMcuIdx)
{	
	if (NULL == pwMcuIdx)						// pwMcuIdx�ǿ�
	{
		return FALSE;
	}

	*pwMcuIdx = INVALID_MCUIDX;					// ������wMcuIdx��ʼ��Ϊ�Ƿ�ֵ
	
	// ������֤
	if (!pabyMcuId								// pbyMcuId����Ϊ��
		|| 0 == byMcuIdCount					// byCount����Ϊ0
		|| byMcuIdCount > MAX_SUBMCU_DEPTH)     // byCount���ܳ���MAX_SUBMCU_DEPTH
	{
		return FALSE;
	}

 	// m_abyMcuInfoΪ��, ����FALSE
	if (!GetMcuCount())
	{
		return FALSE;
	}
	
	// ����pbyMcuId��byCountѰ��
	*pwMcuIdx = FindMcuIdx(pabyMcuId, byMcuIdCount);

	ISVALIDMCUID(*pwMcuIdx);	
	return TRUE;
}


/*====================================================================
    ������      ��IsSMcuByMcuIdx
    ����        ���ж�wMcuIdx��ʾ��mcu�Ƿ��Ǳ�mcu��ֱ���¼�mcu
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[in]wMcuIdx, �ҵ����¼�mcu��idx
    ����ֵ˵��  ���ǵ�,����TRUE, ����FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    10/07/08    4.6         �ܾ���         ����
	10/12/14    4.6         ���㻪         �߶�
====================================================================*/
inline BOOL32 TConfAllMcuInfo::IsSMcuByMcuIdx(const u16& wMcuIdx )
{
	ISVALIDMCUID(wMcuIdx);
	
	if (IsLocalMcuId(wMcuIdx))								// ĿǰTConfAllMcuInfo��֧�ֹ�����mcu
	{
		return FALSE;
	}

	if(m_abyMcuInfo[wMcuIdx][0] != 0 && 0 == m_abyMcuInfo[wMcuIdx][1] )
	{
		return TRUE;
	}

	return FALSE;
}

/*====================================================================
    ������      ��GetMcuIdByIdx
    ����        ������mcu index�ҵ�����mcu id��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[in]wMcuIdx, �ҵ����¼�mcu��idx
				  [out]pabyMcuId, ����, ������ҵ���mcu id
				  [in]byMcuIdCount, pabyMcuId��ֵ�ĸ���, ��ֵ��ΧΪ(0, MAX_SUBMCU_DEPTH]
    ����ֵ˵��  ���ҵ�����TRUE, ����FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    10/07/08    4.6         �ܾ���         ����
	10/12/15    4.6         liuxu          �߶�
====================================================================*/
inline BOOL32 TConfAllMcuInfo::GetMcuIdByIdx( const u16 wMcuIdx, u8* const pabyMcuId, const u8 byMcuIdCount/* = MAX_SUBMCU_DEPTH*/)
{
	// ������֤, pbyMcuId����Ϊ��, byCount����Ϊ0
	if (!pabyMcuId || 0 == byMcuIdCount)
	{
		return FALSE;
	}

	memset(pabyMcuId, 0, byMcuIdCount);				// ������pbyMcuId��ֵȫ��Ϊ0
	if (byMcuIdCount > MAX_SUBMCU_DEPTH)			// byCount���ܳ���MAX_SUBMCU_DEPTH
	{
		return FALSE;
	}

	if (IsLocalMcuId(wMcuIdx))						// ĿǰTConfAllMcuInfo�в�֧�ֱ���mcu�Ĺ���
	{
		return FALSE;
	}
	
	ISVALIDMCUID(wMcuIdx);							// wMcuIdx��Χ��֤

	// Ϊ��, ����false
	if (IsNull(wMcuIdx))
	{
		return FALSE;
	}

	// ��m_abyMcuInfo[wMcuIdx]��ֵ����byCount��ֵ��pbyMcuId��
	memcpy(pabyMcuId, m_abyMcuInfo[wMcuIdx], byMcuIdCount);
	return TRUE;
}

/*====================================================================
    ������      ��GetSMcuNum
    ����        ����ȡֱ���¼�MCU��Ŀ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  �������¼�MCU��Ŀ
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    10/07/08    4.6         �ܾ���         ����
	10/12/14    4.6         ���㻪         �߶�
====================================================================*/
inline u8 TConfAllMcuInfo::GetSMcuNum( void )
{	
	u16 wTemp = 0;
	u8 byNum = 0;
	while(wTemp < MAXNUM_CONFSUB_MCU )
	{
		if (IsSMcuByMcuIdx(wTemp))
		{
			byNum++;
		}
		++wTemp;
	}
	
	return byNum;
}


/*====================================================================
    ������      ��IsNull
    ����        ���ж�m_abyMcuInfo[wIndex]�Ƿ�Ϊ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����wIndex, m_abyMcuInfo�ĸ�������, ��Χ = [0, 192) || (192, MAXNUM_CONFSUB_MCU)
    ����ֵ˵��  ��Ϊ��, ����True; ����False
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/12/15  1.0         ����           ����
====================================================================*/
inline BOOL32 TConfAllMcuInfo::IsNull(const u16& wIndex) const
{
	// ������Χ��֤
	ISVALIDMCUID(wIndex);						// wMcuIdx��Χ��֤
	
	// ֻ��Ҫ�ж�m_abyMcuInfo[wIndex][0]�Ϳ�����. ����ֵ���Ϊ0 ����Ϊ�Ƿ�ֵ, ���ǿ�
	if (0 == m_abyMcuInfo[wIndex][0] || m_abyMcuInfo[wIndex][0] > MAXNUM_CONF_MT)
	{
		return TRUE;
	}

	return FALSE;
}


/*====================================================================
    ������      ��CompareByMcuIds
    ����        ����wIndex��ʾ��mcu id����ֵ��pbyMcuId�Ƚ�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[in]wIndex, m_abyMcuInfo�ĸ�������, ��Χ = (0, 192) || (192, MAXNUM_CONFSUB_MCU)
				  [in]pabyMcuId, �洢mcu id������,
				  [in]byMcuIdCount, pabyMcuId��ֵ�ĸ���, ��ֵ��ΧΪ(0, MAX_SUBMCU_DEPTH]
	����ֵ˵��  ��E_CMP_RESULT_UP, ����ֵΪwIndex��mcuΪpbyMcuId��ʾ��mcu���ϼ�
				  E_CMP_RESULT_EQUAL������ֵΪwIndex��mcu����pbyMcuId��ʾ��mcu
				  E_CMP_RESULT_DOWN������ֵΪwIndex��mcuΪpbyMcuId��ʾ��mcu���¼�
				  E_CMP_RESULT_OTHER, ����ֵΪwIndex��mcu��pbyMcuId��ʾ��mcuû�����¼���ϵ
				  E_CMP_RESULT_INVALID, ��������Ƿ�, ���ܱȽ�
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/12/15  1.0         ����           ����
====================================================================*/
inline TConfAllMcuInfo::E_CapareResult 
TConfAllMcuInfo::CompareByMcuIds(const u16& wIndex, const u8* const pabyMcuId, const u8& byMcuIdCount) const
{
	// ������֤
	if (!pabyMcuId									// pbyMcuId����Ϊ��
		|| 0 == byMcuIdCount						// byCount����Ϊ0
		|| byMcuIdCount > MAX_SUBMCU_DEPTH)         // byCount���ܳ���MAX_SUBMCU_DEPTH		
	{
		return E_CMP_RESULT_INVALID;
	}

	// ������֤��wIndex����Ϸ�
	if (!IsValidMcuId(wIndex)) 
	{
		return E_CMP_RESULT_INVALID;
	}

	// ������֤�����wIndexΪ��, �򷵻�
	if (IsNull(wIndex))
	{
		return E_CMP_RESULT_INVALID;
	}

	// wIndex�����Mcu Index���ڼ��������
	const s8 chDepth1 = GetDepth(m_abyMcuInfo[wIndex], MAX_SUBMCU_DEPTH);

	// pabyMcuId��byMcuIdCount�����Mcu Index���ڼ��������
	const s8 chDapth2 = GetDepth(pabyMcuId, byMcuIdCount);

	// �������¼�������mcu
	if (0 >= chDepth1 || 0 >= chDapth2)
	{
		return E_CMP_RESULT_INVALID;
	}

	// ��chDepth1��chDapth2��ȡ��Сֵ
	const s8 chMinDepth = min(chDepth1, chDapth2);
	// �Ƚ�chMinDepth���ֽڵ��ڴ�
	const s32 nCmpResult = memcmp(m_abyMcuInfo[wIndex], pabyMcuId, chMinDepth);

	// ǰ�ߺͺ��ߵĿ�ʼchMinDepth���ֽڵ��ڴ���������
	if (0 == nCmpResult)
	{
		if (chDepth1 < chDapth2)					// chMinDepth = chDepth1
		{
			return E_CMP_RESULT_UP;
		}else if (chDepth1 == chDapth2)				// chDepth1 = chDapth2
		{
			return E_CMP_RESULT_EQUAL;
		}else										// chMinDepth = chDapth2
		{
			return E_CMP_RESULT_DOWN;
		}
	}else
	{
		// ǰ����������û�й�ͬ���ڴ�����ݣ� �������ǲ�����ֱ�����¼���ϵ
		return E_CMP_RESULT_OTHER;
	}
}


/*====================================================================
    ������      ��FindMcuIdx
    ����        ������mcu idѰ�Ҷ�Ӧ��idx, ����ҵ�, ������ȷ��idx, ���ʧ��,���طǷ���idx. 
				  ͬʱ, ����¼�mcuû��ռ��, �᷵��һ�����п��õ�idx
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[in]pbyMcuId, ����,
				  [in]byCount, pbyMcuId��ֵ�ĸ���, ��ֵ��ΧΪ[0, MAX_SUBMCU_DEPTH]
				  [out]pwIdleIndex, ����¼�mcuû��ռ��ʱ, ������һ�����еĿ��õ�idx
	����ֵ˵��  ��[0��MAXNUM_CONFSUB_MCU), �ҵ���idx�Ϸ�ֵ
				  [MAXNUM_CONFSUB_MCU, INVALID_MCUIDX], û���ҵ�
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/12/15  1.0         ����           ����
====================================================================*/
inline u16 TConfAllMcuInfo::FindMcuIdx(const u8* const pabyMcuId, const u8& byMcuIdCount, u16* const pwIdleIndex/* = NULL*/) const
{
	if (pwIdleIndex)
	{
		*pwIdleIndex = INVALID_MCUIDX;					// ������wIdleIndex��Ϊ�Ƿ�ֵ
	}

	u16 wMcuIdx = INVALID_MCUIDX;						// ����ֵ, Ĭ�Ϸ��طǷ�ֵ

	// ������֤
	if (!pabyMcuId										// pbyMcuId����Ϊ��
		|| 0 == byMcuIdCount							// byCount����Ϊ0
		|| byMcuIdCount > MAX_SUBMCU_DEPTH)				// byCount���ܳ���MAX_SUBMCU_DEPTH
	{
		return wMcuIdx;
	}

	for ( u16 wIndex = 0; wIndex < MAXNUM_CONFSUB_MCU; wIndex++)
	{
		// ��һ������Indexֵ����wIdleIndex
		if (pwIdleIndex									// pwIdleIndex�ǿ�
			&& IsNull(wIndex)							// wIndex���ǿ�
			&& !IsValidSubMcuId(*pwIdleIndex))			// *pwIdleIndex��δ����Чֵ
		{
			*pwIdleIndex = wIndex;
			continue;					
		}
		
		// �ڴ�Ƚ�, ���memcmp����0�� ����ȫ���, �������
		// ����mcu index(��wIndex)��m_abyMcuInfo�е�����ֵ�������pabyMcuId���
		if (!IsValidSubMcuId(wMcuIdx))					// ��δ�ҵ�,������
		{
			if (E_CMP_RESULT_EQUAL == CompareByMcuIds(wIndex, pabyMcuId, byMcuIdCount))
			{
				wMcuIdx = wIndex;
				continue;
			}
		}

		// ֻ������pwIdleIndex��wMcuIdx�����ҵ�ʱ, �����Ѿ�����������ʱ, ��ֹͣѭ��
		if ((IsValidSubMcuId(wMcuIdx) && !pwIdleIndex)					// wMcuIdx�ҵ���pwIdleIndexΪ��
			// wMcuIdx�ҵ���pwIdleIndex��Ϊ����pwIdleIndexҲ�ҵ�
			|| (IsValidSubMcuId(wMcuIdx) && pwIdleIndex && IsValidSubMcuId(*pwIdleIndex)))
		{
			break;
		}
	}

	return wMcuIdx;
}

/*====================================================================
    ������      ��GetDepth
    ����        ������mcu id��ȡ�����������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[in]pabyMcuId, ����,
				  [in]byMcuIdCount, pabyMcuId��ֵ�ĸ���, ��ֵ��ΧΪ(0, MAX_SUBMCU_DEPTH]
	����ֵ˵��  ��<= 0�� �Ƿ�ֵ
				  (0��byMcuIdCount], �ҵ��ĺϷ������ֵ
				  (byMcuIdCount, �����], �Ƿ�ֵ
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2010/12/15  1.0         ����           ����
====================================================================*/
inline const s8 
TConfAllMcuInfo::GetDepth(const u8* const pabyMcuId, const u8& byMcuIdCount) const
{
	s8 chDepth = 0;						// ��ʼΪ0�������Լ�
	if (NULL == pabyMcuId || 0 == byMcuIdCount)
	{
		return chDepth;
	}

	const u8 byMinCount = min(byMcuIdCount, MAX_SUBMCU_DEPTH);
	for (u8 byCounter = 0; byCounter < byMinCount; byCounter++)
	{
		if ( 0 != pabyMcuId[byCounter] )
		{
			chDepth++;
			continue;
		}else
		{
			// ����mcu id������Ϊ0�� Ϊ0����յģ����ɷ���
			break;
		}
	}

	return chDepth;
}

/*-------------------------------------------------------------------
                           TConfAllMtInfo                          
--------------------------------------------------------------------*/


/*====================================================================
    ������      ��AddMt
    ����        ������������һ�������ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ���ɹ�����TRUE������FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/31    1.0         LI Yi         ����
====================================================================*/
inline BOOL TConfAllMtInfo::AddMt(const u8 byMtId)
{
	if (!IsValidMtId(byMtId))
	{
		return FALSE;
	}

	m_tLocalMtInfo.AddMt( byMtId );
	
	return TRUE;
}

/*====================================================================
    ������      ��AddJoinedMt 
    ����        ������������һ������նˣ��ú����Զ������ն�Ҳ
	              ���뵽�����ն��б���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ���ɹ�����TRUE������FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/30    1.0         LI Yi         ����
====================================================================*/
inline BOOL TConfAllMtInfo::AddJoinedMt(const u8 byMtId)
{
	if (!IsValidMtId(byMtId))
	{
		return FALSE;
	}

	m_tLocalMtInfo.AddJoinedMt( byMtId );
	m_tLocalMtInfo.AddMt( byMtId );
	
	return TRUE;
}

/*====================================================================
    ������      ��RemoveMt
    ����        ��������ɾ��һ�������նˣ��ú����Զ������ն�Ҳ
	              ɾ��������ն��б���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/31    1.0         LI Yi         ����
	10/12/30    4.6          liuxu         �޸�
====================================================================*/
inline void TConfAllMtInfo::RemoveMt(u8 byMtId )
{
	if (!IsValidMtId(byMtId))
	{
		return;
	}

	m_tLocalMtInfo.RemoveMt( byMtId );
	m_tLocalMtInfo.RemoveJoinedMt( byMtId );
}

/*====================================================================
    ������      ��RemoveJoinedMt
    ����        ��������ɾ��һ������ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/30    1.0         LI Yi         ����
====================================================================*/
inline void TConfAllMtInfo::RemoveJoinedMt( const u8 byMtId )
{
	if (!IsValidMtId(byMtId))
	{
		return;
	}

	m_tLocalMtInfo.RemoveJoinedMt( byMtId );
}

/*====================================================================
    ������      ��RemoveJoinedMt
    ����        ��������ɾ��һ������ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMt  tMt, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/30    1.0         LI Yi         ����
	10/12/30	4.6			liuxu		  �޸�
====================================================================*/
inline void TConfAllMtInfo::RemoveJoinedMt( const TMt& tMt )
{
	if (!IsValidMtId(tMt.GetMtId()))
	{
		return;
	}
	
	if( tMt.IsLocal() )
	{
		m_tLocalMtInfo.RemoveJoinedMt( tMt.GetMtId() );
	}
	else
	{
		// tMt��McuId�ĺϷ��Լ��
		if (!IsValidMcuId(tMt.GetMcuId()))
		{
			return;
		}

		if( tMt.GetMcuId() == m_atOtherMcMtInfo[tMt.GetMcuId()].GetMcuIdx() )
		{
			m_atOtherMcMtInfo[tMt.GetMcuId()].RemoveJoinedMt( tMt.GetMtId() );
		}		
	}
}

/*====================================================================
    ������      ��AddJoinedMt
    ����        ������������һ������ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMt  tMt, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/30    1.0         LI Yi         ����
	10/12/30    4.6          liuxu         �޸�
====================================================================*/
inline BOOL TConfAllMtInfo::AddJoinedMt(const TMt& tMt)
{
	if(!IsValidMtId(tMt.GetMtId()))
	{
		return FALSE;
	}

	if (!IsValidMcuId(tMt.GetMcuId()))
	{
		return FALSE;
	}

	// ���ڽ����m_tLocalMtInfo�ϲ���m_atOtherMcMtInfo��ȥ
	if( tMt.IsLocal() )
	{
		m_tLocalMtInfo.AddJoinedMt( tMt.GetMtId() );
		return TRUE;
	}
	else
	{
		if( tMt.GetMcuId() == m_atOtherMcMtInfo[tMt.GetMcuId()].GetMcuIdx() )
		{
			m_atOtherMcMtInfo[tMt.GetMcuId()].AddJoinedMt( tMt.GetMtId() );
			return TRUE;
		}		

		return FALSE;
	}
}


/*====================================================================
    ������      ��RemoveAllJoinedMt
    ����        ��������ɾ��һ��MCU����������նˣ���ɾ��MCU��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wMcuId, MCU��
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/30    1.0         LI Yi         ����
====================================================================*/
inline void TConfAllMtInfo::RemoveAllJoinedMt( void )
{
	m_tLocalMtInfo.RemoveAllJoinedMt();
}

/*====================================================================
    ������      ��MtInConf
    ����        ���ж��ն��Ƿ��������ն��б���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ��TRUE/FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/01    1.0         LI Yi         ����
====================================================================*/
inline BOOL TConfAllMtInfo::MtInConf( const u8 byMtId ) const
{
	return m_tLocalMtInfo.MtInConf( byMtId );
}

/*====================================================================
    ������      ��MtJoinedConf
    ����        ���ж��ն��Ƿ�������ն��б���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wMcuId, MCU��
				  u8 byMtId, �ն˺�
    ����ֵ˵��  ��TRUE/FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/30    1.0         LI Yi         ����
====================================================================*/
inline BOOL TConfAllMtInfo::MtJoinedConf(const u8 byMtId ) const
{
	return m_tLocalMtInfo.MtJoinedConf( byMtId );
}

/*====================================================================
    ������      ��GetAllMtNum
    ����        ���õ����������ṹ�����������ն˸���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����void
    ����ֵ˵��  �����صĻ��������������ն˸���
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/02    1.0         LI Yi         ����
====================================================================*/
inline u8   TConfAllMtInfo::GetLocalMtNum( void ) const
{
	return m_tLocalMtInfo.GetAllMtNum();
}

/*====================================================================
    ������      ��GetAllJoinedMtNum
    ����        ���õ����������ṹ����������ն˸���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����void
    ����ֵ˵��  �����صĻ�������������ն˸���
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/30    1.0         LI Yi         ����
====================================================================*/
inline u8   TConfAllMtInfo::GetLocalJoinedMtNum( void ) const
{
	return m_tLocalMtInfo.GetAllJoinedMtNum();
}

/*====================================================================
    ������      ��GetAllUnjoinedMtNum
    ����        ���õ����������ṹ������δ����ն˸��� 
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����void
    ����ֵ˵��  �����صĻ���������δ����ն˸���
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/30    1.0         LI Yi         ����
====================================================================*/
inline u8   TConfAllMtInfo::GetLocalUnjoinedMtNum( void ) const
{
	return m_tLocalMtInfo.GetAllUnjoinedMtNum();
}

/*====================================================================
    ������      ��SetMtInfo
    ����        ������ĳ��Mc���ն���Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����void
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/07/29    3.0         ������         ����
	10/12/30    4.6          liuxu         �޸�
====================================================================*/
inline void TConfAllMtInfo::SetMtInfo(const TConfMtInfo& tConfMtInfo )
{
	const u16 wMcuid = tConfMtInfo.GetMcuIdx();
	if (!IsValidMcuId(wMcuid))
	{
		return;
	}

	if (IsLocalMcuId(wMcuid))
	{
		m_tLocalMtInfo = tConfMtInfo;
	}else
	{
		m_atOtherMcMtInfo[wMcuid] = tConfMtInfo;
	}
}

/*====================================================================
    ������      ��GetMcMtInfo
    ����        ���õ�ĳ��Mc���ն���Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����void
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/07/29    3.0         ������         ����
	10/12/30    4.6          liuxu         �޸�
====================================================================*/
inline TConfMtInfo  TConfAllMtInfo::GetMtInfo( const u16 wMcuIdx ) const
{
	if( IsLocalMcuId(wMcuIdx))
	{
		return m_tLocalMtInfo;
	}

	TConfMtInfo tConfMtInfo;
	tConfMtInfo.SetNull();

	if (!IsValidMcuId(wMcuIdx))
	{
		return tConfMtInfo;
	}
	
	if( m_atOtherMcMtInfo[wMcuIdx].GetMcuIdx() != wMcuIdx )
	{
		// [12/30/2010 liuxu]����ʲô��˼?
		tConfMtInfo.SetMcuIdx( wMcuIdx );
		return tConfMtInfo;
	}
	else
	{
		tConfMtInfo = m_atOtherMcMtInfo[wMcuIdx];		
	}

	return tConfMtInfo;
}

/*====================================================================
    ������      ��GetMtInfoPtr
    ����        ���õ�ĳ��Mc���ն���Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����void
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/07/29    3.0         ������         ����
	10/12/29    3.0         liuxu          ����
====================================================================*/
inline TConfMtInfo*  TConfAllMtInfo::GetMtInfoPtr(const u16 wMcuIdx )
{
	if(wMcuIdx == m_tLocalMtInfo.GetMcuIdx())
	{
		return &m_tLocalMtInfo;
	}

	if (!IsValidMcuId(wMcuIdx))
	{
		return NULL;
	}

	if( m_atOtherMcMtInfo[wMcuIdx].GetMcuIdx() != wMcuIdx )
	{
		// [8/29/2011 liuxu] �˴�����return null, ��Ϊ��mcs�㱨�ն�״̬ʱ, 
		// Ҫ����������list���ϱ���ȥ
		/*return NULL;*/
	}
	
	return &(m_atOtherMcMtInfo[wMcuIdx]);
}

/*====================================================================
    ������      ��MtInConf
    ����        ���ж��ն��Ƿ��������ն��б���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����void
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/07/29    3.0         ������         ����
	10/12/30    4.6          liuxu         �޸�
====================================================================*/
inline BOOL TConfAllMtInfo::MtInConf( const u16  wMcuIdx, const u8  byMtId ) const
{
	if (!IsValidMcuId(wMcuIdx))
	{
		return FALSE;
	}

	TConfMtInfo tConfMtInfo = GetMtInfo( wMcuIdx );
	if( !tConfMtInfo.IsNull() )
	{
		return tConfMtInfo.MtInConf( byMtId );
	}
	else
	{
		return FALSE;
	}
}

/*====================================================================
    ������      ��MtJoinedConf
    ����        ���ж��ն��Ƿ�������ն��б���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����void
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/07/29    3.0         ������         ����
	10/12/30    4.6          liuxu         �޸�
====================================================================*/
inline BOOL TConfAllMtInfo::MtJoinedConf( const u16 wMcuIdx, const u8 byMtId ) const
{
	if( m_tLocalMtInfo.GetMcuIdx() == wMcuIdx )
	{
		return MtJoinedConf( byMtId );
	}

	if (!IsValidSubMcuId(wMcuIdx))
	{
		return FALSE;
	}

	TConfMtInfo tConfMtInfo = GetMtInfo( wMcuIdx );
	if( !tConfMtInfo.IsNull() )
	{
		return tConfMtInfo.MtJoinedConf( byMtId );
	}
	else
	{
		return FALSE;
	}
}

/*==============================================================================
������    : MtJoinedConf 
����      : �ж��ն��Ƿ�������ն��б���(�����¼�MCU�µ�ĳ�ն�)
�㷨ʵ��  :  
����˵��  :  TMt tMt
����ֵ˵��:  BOOL
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-9-30                   Ѧ��
2010-12-30                  liuxu
==============================================================================*/
inline BOOL TConfAllMtInfo::MtJoinedConf( const TMt& tMt ) const
{
	if(tMt.IsLocal())
	{
		return m_tLocalMtInfo.MtJoinedConf( tMt.GetMtId() );
	}
	else
	{
		return MtJoinedConf( tMt.GetMcuIdx(),tMt.GetMtId() );
	}
}
/*====================================================================
    ������      ��GetAllMtNum
    ����        ���õ����������ṹ�����������ն˸���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����void
    ����ֵ˵��  ���õ����������������ն���
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/07/29    3.0         ������         ����
====================================================================*/
inline u16   TConfAllMtInfo::GetAllMtNum( void ) const 
{
	// ֻ���ر�Mc���ն�
	u16 wMtNum = m_tLocalMtInfo.GetAllMtNum();

	return wMtNum;
}

/*==============================================================================
������    :  GetMaxMtIdInConf
����      :  �õ����������ṹ�����������ն���ID����ֵ
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  u8 
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2008-12-24					Ѧ��							����
==============================================================================*/
inline u8 TConfAllMtInfo::GetMaxMtIdInConf( void ) const
{
	// ֻ���ر�Mc��
	u8 byMaxMtId = 0;
	byMaxMtId = m_tLocalMtInfo.GetMaxMtIdInConf();
	return byMaxMtId;
}

/*====================================================================
    ������      ��GetCascadeMcuNum
    ����        ���õ������ı��mcu����, �����ȵ���TConfAllMcuInfo����ؽӿ�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����void
    ����ֵ˵��  ���¼�Mc�ĸ���
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/07/29    3.0         ������         ����
	10/12/30    4.6          liuxu         �޸�
====================================================================*/
inline u16  TConfAllMtInfo::GetCascadeMcuNum( void) const
{
	u16 wMcNum = 0;
	for( u16 wLoop=0; wLoop < MAXNUM_CONFSUB_MCU; wLoop++ )
	{
		// 192�Ǳ���mcu id
		if (IsLocalMcuId(wLoop))
		{
			continue;
		}

		if( !m_atOtherMcMtInfo[wLoop].IsNull() )
		{
			wMcNum++;
		}
	}

	return wMcNum;
}

/*=============================================================================
    �� �� ���� AddMcuInfo
    ��    �ܣ� ����������һ��mcu��Ϣ��������ʱû���ն�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byMcuId
	           u8 m_byConfIdx
    �� �� ֵ�� inline BOOL 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/04/22  3.6			����                  ����
	10/12/30    4.6          liuxu         �޸�
=============================================================================*/
inline BOOL TConfAllMtInfo::AddMcuInfo( u16 wMcuIdx, u8 m_byConfIdx )
{   
	if(!IsValidMcuId(wMcuIdx))
	{
		return FALSE;
	}

	// ����mcu��Ϣ��ʱ����m_tLocalMtInfo��, �Ժ�ϲ���m_atOtherMcMtInfo��
	if (IsLocalMcuId(wMcuIdx))
	{
		memset( &m_tLocalMtInfo, 0, sizeof(TConfMtInfo) );
		m_tLocalMtInfo.SetMcuIdx( wMcuIdx );
		m_tLocalMtInfo.SetConfIdx( m_byConfIdx );
	}

	memset( &m_atOtherMcMtInfo[wMcuIdx], 0, sizeof(TConfMtInfo) );
	m_atOtherMcMtInfo[wMcuIdx].SetMcuIdx( wMcuIdx );
	m_atOtherMcMtInfo[wMcuIdx].SetConfIdx( m_byConfIdx );
	
	return TRUE;
}

/*=============================================================================
    �� �� ���� RemoveMcuInfo
    ��    �ܣ� �������Ƴ�һ��mcu��Ϣ��ͬʱ������������ն���Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byMcuId
	           u8 m_byConfIdx
    �� �� ֵ�� inline BOOL 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/04/22  3.6			����                  ����
	10/12/30    4.6          liuxu                  �޸�
=============================================================================*/
inline BOOL TConfAllMtInfo::RemoveMcuInfo( const u16 wMcuIdx, const u8 m_byConfIdx )
{   
	if (!IsValidSubMcuId(wMcuIdx))
	{
		return FALSE;
	}

	// ����
	if (IsLocalMcuId(wMcuIdx))
	{
		memset( &m_tLocalMtInfo, 0, sizeof(TConfMtInfo) );
		m_tLocalMtInfo.SetNull();
		return TRUE;
	}

	// �Ǳ���mcu
	if( m_atOtherMcMtInfo[wMcuIdx].GetMcuIdx() == wMcuIdx )
	{
		memset( &m_atOtherMcMtInfo[wMcuIdx], 0, sizeof(TConfMtInfo) );
		m_atOtherMcMtInfo[wMcuIdx].SetNull();
		return TRUE;
	}

	return FALSE;
}

/*====================================================================
    ������      ��GetAllJoinedMtNum
    ����        ���õ����������ṹ����������ն˸���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����void
    ����ֵ˵��  �����м��������ն���
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/07/29    3.0         ������         ����
	10/12/30    4.6          liuxu         �޸�
====================================================================*/
inline u32 TConfAllMtInfo::GetAllJoinedMtNum( void ) const
{
	u16 wLoop = 0;
	
	const TConfMtInfo * ptConfMtInfo = NULL;

	u32 dwMtNum = 0;					// �����¼�mcu���ն˼��������ܻᳬ��0xFFFF
	//����Mc��
	while( wLoop < GetMaxMcuNum() )
	{
		// 192���ճ�����Ϊ�����ն�. �����Ż�, m_tLocalMtInfo����ϲ���m_atOtherMcMtInfo��
		if (IsLocalMcuId(wLoop))
		{
			//���ϱ�Mc���ն�
			dwMtNum += m_tLocalMtInfo.GetAllJoinedMtNum();
			continue;
		}

		ptConfMtInfo = &m_atOtherMcMtInfo[wLoop];
		if( !ptConfMtInfo->IsNull() )
		{
			dwMtNum += ptConfMtInfo->GetAllJoinedMtNum();
		}

		wLoop++;
	}

	return dwMtNum;
}

/*====================================================================
    ������      ��SetChnnlStatus
    ����        ������¼����ŵ�״̬��Ϣ������������¼�����ŵ����ٵ��ô˺���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byChnnlIndex, �ŵ�����
				  u8 byChnnlType, �ŵ����࣬TRecChnnlStatus::TYPE_RECORD��TYPE_PLAY
				  const TRecChnnlStatus * ptStatus, �ŵ�״̬���ں��ŵ�����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/11    1.0         LI Yi         ����
====================================================================*/
inline BOOL TRecStatus::SetChnnlStatus( u8 byChnnlIndex, u8 byChnnlType, const TRecChnnlStatus * ptStatus )
{
	//record channel
	if( byChnnlType == TRecChnnlStatus::TYPE_RECORD )
	{
		if( byChnnlIndex < m_byRecChnnlNum )
		{
			m_tChnnlStatus[byChnnlIndex] = *ptStatus;
			return( TRUE );
		}
		else
		{
			OspPrintf( TRUE, FALSE, "TRecStatus: Exception - Wrong record channel index%u\n", byChnnlIndex ); 
			return( FALSE );
		}
	}
	else if( byChnnlType == TRecChnnlStatus::TYPE_PLAY )	//�����ŵ�
	{
		if( byChnnlIndex < m_byPlayChnnlNum )
		{
			m_tChnnlStatus[byChnnlIndex + m_byRecChnnlNum] = *ptStatus;
			return( TRUE );
		}
		else
		{
			OspPrintf( TRUE, FALSE, "TRecStatus: Exception - Wrong play channel index%u\n", byChnnlIndex ); 
			return( FALSE );
		}
	}
	else
	{
		OspPrintf( TRUE, FALSE, "TRecStatus: Exception - Wrong channel type%u\n", byChnnlType ); 
		return( FALSE );
	}
}

/*====================================================================
    ������      ��SetChnnlStatus
    ����        ������¼����ŵ�״̬��Ϣ������������¼�����ŵ����ٵ��ô˺���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byChnnlIndex, �ŵ�����
				  u8 byChnnlType, �ŵ����࣬TRecChnnlStatus::TYPE_RECORD��TYPE_PLAY
				  TRecChnnlStatus * ptStatus, �ŵ�״̬���ں��ŵ�����
    ����ֵ˵��  ��TRUE/FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/11    1.0         LI Yi         ����
====================================================================*/
inline BOOL TRecStatus::GetChnnlStatus( u8 byChnnlIndex, u8 byChnnlType, TRecChnnlStatus * ptStatus ) const
{
	//record channel
	if( byChnnlType == TRecChnnlStatus::TYPE_RECORD )
	{
		if( byChnnlIndex < m_byRecChnnlNum )
		{
			*ptStatus = m_tChnnlStatus[byChnnlIndex];
			return( TRUE );
		}
		else
		{
			OspPrintf( TRUE, FALSE, "TRecStatus: Exception - Wrong record channel index%u\n", byChnnlIndex ); 
			return( FALSE );
		}
	}
	else if( byChnnlType == TRecChnnlStatus::TYPE_PLAY )	//�����ŵ�
	{
		if( byChnnlIndex < m_byPlayChnnlNum )
		{
			*ptStatus = m_tChnnlStatus[byChnnlIndex + m_byRecChnnlNum];
			return( TRUE );
		}
		else
		{
			OspPrintf( TRUE, FALSE, "TRecStatus: Exception - Wrong play channel index%u\n", byChnnlIndex ); 
			return( FALSE );
		}
	}
	else
	{
		//OspPrintf( TRUE, FALSE, "TRecStatus: Exception - Wrong channel type%u\n", byChnnlType ); 
		return( FALSE );
	}
}


/*====================================================================
    ������      ��IsMtIpInAllowSeg
    ����        ���ж�IP�Ƿ��ںϷ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u32, IP ��ַ��������
    ����ֵ˵��  ��TRUE/FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    06/06/26    4.0         ����        ����
====================================================================*/
inline BOOL32 TUsrGrpInfo::IsMtIpInAllowSeg(u32 dwIp) const
{
/*
 *  ���´������� ͬʱ����������ͽ�ֹ�� 
 *  Ŀǰֻ��������һ��
 *  ����ʱע��
    BOOL32 bNoAllowed = TRUE; // �Ƿ������������

    for (u8 byLoop = 0; byLoop < MAXNUM_IPSEG; byLoop ++)
    {
        if ( atIpSegAllow[byLoop].IsEnabled() )
        {
            bNoAllowed = FALSE;
            break;
        }
    }

    // ���û����������Σ����һ�����еĽ�ֹ��
    if (bNoAllowed)
    {
        for (byLoop = 0; byLoop < MAXNUM_IPSEG; byLoop ++)
        {
            if ( atIpSegBan[byLoop].IsEnabled() )
            {
                if ( atIpSegBan[byLoop].IsIpIn( dwIp ) )
                {
                    // �����ڽ�ֹ��
                    return FALSE;
                }
            }
        }
        // ���ڽ�ֹ�Σ���Ϊ����
        return TRUE;
    }
    
    // �������������Σ��ȼ���Ƿ��������
    for (byLoop = 0; byLoop < MAXNUM_IPSEG; byLoop ++)
    {
        if ( atIpSegAllow[byLoop].IsEnabled() )
        {
            if ( atIpSegAllow[byLoop].IsIpIn( dwIp ) )
            {
                // ��ĳ������Σ����ٿ��Ƿ���С���ڲ���ֹ�θ���ֹ��
                for (u8 byLoop2 = 0; byLoop2 < MAXNUM_IPSEG; byLoop2 ++)
                {
                    if ( atIpSegBan[byLoop2].IsEnabled() )
                    {
                        if (atIpSegBan[byLoop2].IsIpIn( dwIp ))
                        {
                            // ȷʵ��ֹ����˭��ΧС˭��Ч
                            if ( atIpSegAllow[byLoop].IsIpSegIn( atIpSegBan[byLoop2] ) )
                            {
                                return FALSE;
                            }
                        }
                    }
                }
                // û�н�ֹ�ι涨
                return TRUE;
            }
        }
    }
*/

    if ( bBanned )
    {
        // �ڵ�ַ���в鿴�Ƿ��ں�������
        for (u8 byLoop = 0; byLoop < MAXNUM_IPSEG; byLoop ++)
        {
            if ( atIpSeg[byLoop].IsEnabled() && atIpSeg[byLoop].IsIpIn( dwIp ) )
            {
                return FALSE;                    
            }
        }
        return TRUE;
    }
    else
    {
        // �ڵ�ַ���в鿴�Ƿ��ڰ�������
        for (u8 byLoop = 0; byLoop < MAXNUM_IPSEG; byLoop ++)
        {
            if ( atIpSeg[byLoop].IsEnabled() && atIpSeg[byLoop].IsIpIn( dwIp ) )
            {
                return TRUE;                    
            }
        }
        return FALSE;
    }

    return FALSE;
}

//�ն���Ƶ���ƣ�Ŀǰ�Ƿֱ��ʣ����ʣ�
struct TMtVidLmt
{
public:
	void SetMtVidRes( u8 byRes)	{ m_byRes = byRes; }
	u8	 GetMtVidRes( void ){ return m_byRes;}	
	void SetMtVidBW( u16 wBandWidth){m_wBandWidth = wBandWidth;}
	u16	 GetMtVidBw( void ){ return m_wBandWidth;}

private:
	
	u16 m_wBandWidth;
	u8	m_byRes;
	u8  m_byReserve;

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// MCU 8000B ���������߼�
// ʹ�þ�̬�෽���ṩ�����ӿ�
// Ҫʹ����Щ�ӿ�, �������ֻ���� ʵ���ļ�mcupfmlmt.h ͷ�ļ�һ��
// MCS �յ����ṹ�������б���,��MCU�л������ CMcuPfmLmt::SetFilter() �ӿ�����
struct TMcu8kbPfmFilter
{
    TMcu8kbPfmFilter()
    {
        byMaxNumMixSingleMp3   = MAXNUM_MIX_SINGLE_MP3;     
        byMaxNumMixMultiMp3    = MAXNUM_MIX_MULTI_MP3;
        byMaxNumMixSingleG729  = MAXNUM_MIX_SINGLE_G729;
        byMaxNumMixMultiG729   = MAXNUM_MIX_MULTI_G729;
        byMaxNumVacSingleMp3   = MAXNUM_VAC_SINGLE_MP3;
        byMaxNumVacMultiMp3    = MAXNUM_VAC_MULTI_MP3; 
        byMaxNumVacSingleG729  = MAXNUM_VAC_SINGLE_G729;
        byMaxNumVacMultiG729   = MAXNUM_VAC_MULTI_G729;

        byEnable               = 1; 
    }

    void Print() const
    {
        StaticLog( "TMcu8kbPfmFilter:\n");
        StaticLog( "\t IsEnable: %d\n", byEnable);
        StaticLog( "\t byMaxNumMixSingleMp3: %d\n", byMaxNumMixSingleMp3);
        StaticLog( "\t byMaxNumMixMultiMp3: %d\n", byMaxNumMixMultiMp3);
        StaticLog( "\t byMaxNumMixSingleG729: %d\n", byMaxNumMixSingleG729);
        StaticLog( "\t byMaxNumMixMultiG729: %d\n", byMaxNumMixMultiG729);
        StaticLog( "\t byMaxNumVacSingleMp3: %d\n", byMaxNumVacSingleMp3);
        StaticLog( "\t byMaxNumVacMultiMp3: %d\n", byMaxNumVacMultiMp3);
        StaticLog( "\t byMaxNumVacSingleG729: %d\n", byMaxNumVacSingleG729);
        StaticLog( "\t byMaxNumVacMultiG729: %d\n", byMaxNumVacMultiG729);
    }

    BOOL32 IsEnable() const
    {
        return (byEnable != 0 ? TRUE : FALSE);
    }

public:
    u8 byMaxNumMixSingleMp3; //MP3����(���ƺ�����)��������
    u8 byMaxNumMixMultiMp3;  //MP3����(���ƺ�����)������(VMP��BAS)�쿪����
    u8 byMaxNumMixSingleG729;//G729����(���ƺ�����)��������;
    u8 byMaxNumMixMultiG729; //G729����(���ƺ�����)������(VMP��BAS)�쿪����
    u8 byMaxNumVacSingleMp3; //MP3��VAC��������
    u8 byMaxNumVacMultiMp3;  //MP3��VAC������(BAS)�쿪����
    u8 byMaxNumVacSingleG729;//G729��VAC��������;
    u8 byMaxNumVacMultiG729; //G729��VAC������(BAS)�쿪����
private:
    u8 byEnable;             //�Ƿ�����

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

class CMcuPfmLmt
{
public:
    /*=============================================================================
        �� �� ���� GetMaxVmpStyle
        ��    �ܣ� �жϵ�ǰVMP���֧�ֵĻ���ϳ�·��
        �㷨ʵ�֣� 
        ȫ�ֱ����� 
        ��    ���� TConfInfo &tConfInfo         [in]  ���������Ϣ
                   u8 byMtNum                   [in]  �����ն�����
                   u8 byMixingMtNum             [in]  ���ڲ������(���ܻ���)���ն�����
                   u16 &wError                  [out] ���صĴ�����
        �� �� ֵ�� BOOL32
        ----------------------------------------------------------------------
        �޸ļ�¼    ��
        ��  ��		�汾		�޸���		�߶���    �޸�����
        2007/01/29  4.0			����                  ����
    =============================================================================*/
    static u8 GetMaxOprVmpStyle( const TConfInfo &tConfInfo, u8 byMtNum, u8 byMixingMtNum, u16 &wError );
	
    /*=============================================================================
        �� �� ���� GetMaxOprMixNum
        ��    �ܣ� �жϵ�ǰ��������õĻ���(�������ƺ�����)�ն���
        �㷨ʵ�֣� 
        ȫ�ֱ����� 
        ��    ���� TConfInfo &tConfInfo     [in]  ������Ϣ
        �� �� ֵ�� BOOL32
        ----------------------------------------------------------------------
        �޸ļ�¼    ��
        ��  ��		�汾		�޸���		�߶���    �޸�����
        2007/01/29  4.0			����                  ����
    =============================================================================*/
    static u8 GetMaxOprMixNum( const TConfInfo &tConfInfo, u16 &wError );

    /*=============================================================================
        �� �� ���� GetMaxDynVmpStyle
        ��    �ܣ� ��ȡ��ǰ��֧�ֵĶ�̬�ϳɷ��
        �㷨ʵ�֣� 
        ȫ�ֱ����� 
        ��    ���� TConfInfo &tConfInfo         [in]  ���������Ϣ
                   u8 byMtNum                   [in]  �����ն�����
                   u8 byMixingMtNum             [in]  ���ڲ������(���ܻ���)���ն�����
                   u16 &wError                  [out] ���صĴ�����
        �� �� ֵ�� BOOL32
        ˵��    �� MCS��֧��ʹ�ñ�����
        ----------------------------------------------------------------------
        �޸ļ�¼    ��
        ��  ��		�汾		�޸���		�߶���    �޸�����
        2007/01/31  4.0			����                  ����
    =============================================================================*/    
    static u8 GetMaxDynVmpStyle( const TConfInfo &tConfInfo, u8 byMtNum, u8 byMixingMtNum, u16 &wError );
    
    /*=============================================================================
        �� �� ���� IsVmpOprSupported
        ��    �ܣ� �жϵ�ǰ��VMP�����ܷ����
        �㷨ʵ�֣� 
        ȫ�ֱ����� 
        ��    ���� TConfInfo &tConfInfo         [in]  ���������Ϣ
                   u8 byNewStyle                [in]  �µ�VMP�����ķ��
                   u8 byMtNum                   [in]  �����ն�����
                   u8 byMixingMtNum             [in]  ���ڲ������(���ܻ���)���ն�����
                   u16 &wError                  [out] ���صĴ�����
        �� �� ֵ�� BOOL32
        ----------------------------------------------------------------------
        �޸ļ�¼    ��
        ��  ��		�汾		�޸���		�߶���    �޸�����
        2007/01/25  4.0			�ű���                  ����
    =============================================================================*/    
    static BOOL32 IsVmpOprSupported( const TConfInfo &tConfInfo, u8 byNewStyle, u8 byMtNum, u8 byMixingMtNum, u16 &wError );

    /*=============================================================================
        �� �� ���� IsMixOprSupported
        ��    �ܣ� �жϵ�ǰ�Ķ��ƻ��������ܷ����
        �㷨ʵ�֣� 
        ȫ�ֱ����� 
        ��    ���� TConfInfo &tConfInfo         [in]  ������Ϣ
                   u8 byMixingMtNum             [in]  ���ڲ������(���ܻ���)���ն�����
                   u8 byAddNum                  [in]  Ҫ��ӽ��������ն�����
                   u16 &wError                  [out] ���صĴ�����
        �� �� ֵ�� BOOL32
        ----------------------------------------------------------------------
        �޸ļ�¼    ��
        ��  ��		�汾		�޸���		�߶���    �޸�����
        2007/01/25  4.0			�ű���                  ����
    =============================================================================*/
    static BOOL32 IsMixOprSupported( const TConfInfo &tConfInfo, u8 byMixingNum, u8 byAddNum, u16 &wError );

    /*=============================================================================
        �� �� ���� IsVacOprSupported
        ��    �ܣ� �жϿ���VAC�Ƿ��ܱ�����
        �㷨ʵ�֣� 
        ȫ�ֱ����� 
        ��    ���� TConfInfo &tConfInfo   [in]  ������Ϣ
                   u8        byMtNum      [in]  ���鵱ǰ�����������ն���
                   u16 &wError            [out] ���صĴ�����
        �� �� ֵ�� BOOL32
        ----------------------------------------------------------------------
        �޸ļ�¼    ��
        ��  ��		�汾		�޸���		�߶���    �޸�����
        2007/01/30  4.0			����                  ����
    =============================================================================*/    
    static BOOL32 IsVacOprSupported( const TConfInfo &tConfInfo, u8 byMtNum, u16 &wError );

    /*=============================================================================
        �� �� ���� IsBasOprSupported
        ��    �ܣ� �жϿ���BAS�Ƿ��ܱ�����
        �㷨ʵ�֣� 
        ȫ�ֱ����� 
        ��    ���� TConfInfo &tConfInfo   [in]  ������Ϣ
                   u8        byMixNum     [in]  ���鵱ǰ�Ļ����ն���
                   u16 &wError            [out] ���صĴ�����
        �� �� ֵ�� BOOL32
        ----------------------------------------------------------------------
        �޸ļ�¼    ��
        ��  ��		�汾		�޸���		�߶���    �޸�����
        2007/01/30  4.0			����                  ����
    =============================================================================*/   
    static BOOL32 IsBasOprSupported( const TConfInfo &tConfInfo, u8 byMixNum, u16 &wError );
    
    /*=============================================================================
        �� �� ���� IsMtOprSupported
        ��    �ܣ� �жϵ�ǰ�������ն��Ƿ��ܱ�����
        �㷨ʵ�֣� 
        ȫ�ֱ����� 
        ��    ���� TConfInfo &tConfInfo   [in]  ������Ϣ
                   u8        byMtNum      [in]  ���鵱ǰ�����������ն���
                   u8        byMixNum     [in]  ���ڻ���(���ܡ�����)�ն���
                   u16 &wError            [out] ���صĴ�����
        �� �� ֵ�� BOOL32
        ----------------------------------------------------------------------
        �޸ļ�¼    ��
        ��  ��		�汾		�޸���		�߶���    �޸�����
        2007/01/26  4.0			�ű���                  ����
    =============================================================================*/
    static BOOL32 IsMtOprSupported( const TConfInfo &tConfInfo, u8 byMtNum, u8 byMixNum, u16 &wError );

    /*=============================================================================
        �� �� ���� SetFilter
        ��    �ܣ� �����������ƵĹ��˲���
        �㷨ʵ�֣� 
        ȫ�ֱ����� 
        ��    ���� TMcu8kbPfmFilter& tNewFilter   [in]  �µĲ���������û������ã���ȱʡ���켴��
        �� �� ֵ�� 
        ----------------------------------------------------------------------
        �޸ļ�¼    ��
        ��  ��		�汾		�޸���		�߶���    �޸�����
        2007/01/30  4.0			����                  ����
    =============================================================================*/
    static void SetFilter(const TMcu8kbPfmFilter& tNewFilter)
    {
        m_tFilter = tNewFilter;
    }

	/*=============================================================================
        �� �� ���� GetFilter
        ��    �ܣ� �����������ƵĹ��˲���
        �㷨ʵ�֣� 
        ȫ�ֱ����� 
        ��    ���� ��
        �� �� ֵ�� TMcu8kbPfmFilter
        ----------------------------------------------------------------------
        �޸ļ�¼    ��
        ��  ��		�汾		�޸���		�߶���    �޸�����
        2007/02/07  4.0			�ܹ��                  ����
    =============================================================================*/
    static TMcu8kbPfmFilter GetFilter()
    {
        return m_tFilter;
    }

    /* ���º�������MCU8000A, ���ܹ�����Լ�� */
	/*=============================================================================
	�� �� ���� IsVmpStyleSupport
	��    �ܣ� �ж�ĳ������ϳɣ������໭�����ǽ��������Ŀ�����Ƿ���֧�֣�MCU8000A��
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
	��    ���� const TConfInfo &tConfInfo   [in] ������Ϣ
			   u8 byConfIdx                 [in] ����ConfIdx��Ϣ
			   TPeriEqpStatus *ptVmpStatus  [in] ����״̬����
			   u16 wLen                     [in] ����״̬���鳤��
			   u8 byTargetStyle             [in] ָ����Ŀ����
			   u8 &bySelVmpId               [out] ���ط���������Vmp��VmpTw
	�� �� ֵ�� BOOL32 �Ƿ���֧��
	----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��		�汾		�޸���		�߶���    �޸�����
	2007/7/13   4.0			�ܹ��      ����      ����
	=============================================================================*/
	static BOOL32 IsVmpStyleSupport( const TConfInfo &tConfInfo, u8 byConfIdx, 
							   	     TPeriEqpStatus *ptVmpStatus, u16 wLen, 
								     u8 byTargetStyle, u8 &bySelVmpId );

	/*=============================================================================
	�� �� ���� GetSupportedVmpStylesFor8KE
	��    �ܣ� ����ָ�������ܹ�֧�ֵ����л���ϳɷ�����8000E,����mcs���ã�
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
	��    ����   const TConfInfo &tConfInfo   [in] ������Ϣ
				 u8 byConfIdx                 [in] ����ConfIdx��Ϣ
				 TPeriEqpStatus *ptVmpStatus  [in] ����״̬����
				 u16 wLen                     [in] ����״̬���鳤��
				 u8 *pabStyle                 [out] ���صķ������
				 u8 &byStyleSize              [I/O] ��������Ĵ�С��ͬʱ����ʵ�ʴ�С��
													���ʵ�ʷ���ֵ���ڴ���ֵ����˵���ռ䲻����ͬʱ���������û�н������
	�� �� ֵ�� void 
	----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��		�汾		�޸���		�߶���    �޸�����
	2010/03/01   4.6		Ѧ��		          ����
	=============================================================================*/
	static void GetSupportedVmpStylesFor8KE( const TConfInfo &tConfInfo, u8 byConfIdx, 
												TPeriEqpStatus *ptVmpStatus, u16 wLen, 
												u8 *pabyStyle, u8 &byArraySize );
	/*=============================================================================
	�� �� ���� GetSupportedVmpStyles
	��    �ܣ� ����ָ�������ܹ�֧�ֵ����л���ϳɷ��
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
	��    ����   const TConfInfo &tConfInfo   [in] ������Ϣ
				 u8 byConfIdx                 [in] ����ConfIdx��Ϣ
				 TPeriEqpStatus *ptVmpStatus  [in] ����״̬����
				 u16 wLen                     [in] ����״̬���鳤��
				 u8 *pabStyle                 [out] ���صķ������
				 u8 &byStyleSize              [I/O] ��������Ĵ�С��ͬʱ����ʵ�ʴ�С��
													���ʵ�ʷ���ֵ���ڴ���ֵ����˵���ռ䲻����ͬʱ���������û�н������
	�� �� ֵ�� void 
	----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��		�汾		�޸���		�߶���    �޸�����
	2007/7/13   4.0			�ܹ��      ����      ����
	=============================================================================*/
	static void GetSupportedVmpStyles( const TConfInfo &tConfInfo, u8 byConfIdx, 
										TPeriEqpStatus *ptVmpStatus, u16 wLen, 
										u8 *pabyStyle, u8 &byArraySize );

	/*=============================================================================
	�� �� ���� GetMaxCapVMPByConfInfo
	��    �ܣ� �����ݻ�����Ϣ���㵱ǰ�������֧�ֵĻ���ϳɻ�໭�����ǽ·����MCU8000A��		   
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
	��    ���� const TConfInfo &tConfInfo
	�� �� ֵ�� BOOL32 
	----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��		�汾		�޸���		�߶���    �޸�����
	2007/7/14   4.0			����      ����������������
	=============================================================================*/
    static u8 GetMaxCapVMPByConfInfo( const TConfInfo &tConfInfo );

    /*=============================================================================
	�� �� ���� IsConfFormatHD
	��    �ܣ� �������Ƿ�Ϊ������飬����H.264��Ч
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
    ��    ���� const TConfInfo &tConfInfo   [in] ������Ϣ
	�� �� ֵ�� BOOL32
	----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��		�汾		�޸���		�߶���    �޸�����
	2007/12/19  4.0			�ű���                  ����
	=============================================================================*/
    static BOOL32 IsConfFormatHD( const TConfInfo &tConfInfo );

    /*=============================================================================
	�� �� ���� IsConfFormatOverCif
	��    �ܣ� �������Ƿ�ΪD1���߸�����飬����H.264��Ч
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
    ��    ���� const TConfInfo &tConfInfo   [in] ������Ϣ
	�� �� ֵ�� BOOL32 �����H.264����ʽ���������FALSE
	----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��		�汾		�޸���		�߶���    �޸�����
	2008/03/11  4.5			����                  ����
	=============================================================================*/
    static BOOL32 IsConfFormatOverCif( const TConfInfo &tConfInfo );

	/*=============================================================================
	�� �� ���� IsConfPermitVmp
	��    �ܣ� �������Ƿ�������ϳ�
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
    ��    ���� const TConfInfo &tConfInfo   [in] ������Ϣ
	�� �� ֵ�� BOOL32 �����H.264����ʽ���������FALSE
	----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��		�汾		�޸���		�߶���    �޸�����
	2008/03/11  4.5			����                  ����
	=============================================================================*/
    static BOOL32 IsConfPermitVmp( const TConfInfo &tConfInfo );

    /*=============================================================================
	�� �� ���� IsRollCallSupported
	��    �ܣ� �������Ƿ�֧�ֻ������
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
    ��    ���� const TConfInfo &tConfInfo   [in] ������Ϣ
	�� �� ֵ�� BOOL32
	----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��		�汾		�޸���		�߶���    �޸�����
	2007/12/19  4.0			�ű���                  ����
	=============================================================================*/
    static BOOL32 IsRollCallSupported( const TConfInfo &tConfInfo );

	/*=============================================================================
	�� �� ���� IsVmpSupported
	��    �ܣ� �������Ƿ�֧�ֻ���ϳ���
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
    ��    ���� const TConfInfo &tConfInfo   [in] ������Ϣ
	�� �� ֵ�� BOOL32
	----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��		�汾		�޸���		�߶���    �޸�����
	2007/12/19  4.0			�ű���                  ����
	=============================================================================*/
    static BOOL32 IsVmpSupported( const TConfInfo &tConfInfo );
    
    /*=============================================================================
	�� �� ���� IsMPWSupported
	��    �ܣ� �������Ƿ�֧�ֶ໭�����ǽ
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
    ��    ���� const TConfInfo &tConfInfo   [in] ������Ϣ
	�� �� ֵ�� BOOL32
	----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��		�汾		�޸���		�߶���    �޸�����
	2007/12/19  4.0			�ű���                  ����
	=============================================================================*/
    static BOOL32 IsMPWSupported( const TConfInfo &tConfInfo );
    
    /*=============================================================================
	�� �� ���� IsTVWallSupported
	��    �ܣ� �������Ƿ�֧�ֵ���ǽ
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
    ��    ���� const TConfInfo &tConfInfo   [in] ������Ϣ
	�� �� ֵ�� BOOL32
	----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��		�汾		�޸���		�߶���    �޸�����
	2007/12/19  4.0			�ű���                  ����
	=============================================================================*/
    static BOOL32 IsTVWallSupported( const TConfInfo &tConfInfo );
    
    /*=============================================================================
	�� �� ���� IsMultiVidStreamSupported
	��    �ܣ� �������Ƿ�֧����Ƶ���ٶ��ʽ
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
    ��    ���� const TConfInfo &tConfInfo   [in] ������Ϣ
               u8 byMode = MODE_VIDEO       [in] ��Ƶ����Ƶ,��֧��MODE_BOTH
	�� �� ֵ�� BOOL32
	----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��		�汾		�޸���		�߶���    �޸�����
	2007/12/19  4.0			�ű���                  ����
	=============================================================================*/
    static BOOL32 IsMultiStreamSupported( const TConfInfo &tConfInfo, u8 byMode = MODE_VIDEO );

private:
	/*=============================================================================
	�� �� ���� GetMaxCapVMP
	��    �ܣ� ���㵱ǰ�������֧�ֵĻ���ϳɻ�໭�����ǽ·���Լ���Ӧ���豸ID��MCU8000A��			   
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
	��    ���� const TConfInfo &tConfInfo
			   TPeriEqpStatus *ptVmpStatus : VMP/VMPTW ״̬����
			   u16 wlen : ״̬���鳤��
			   u8 &byVmpCapChnnlNum [OUT]: 
			   u8 &bySelVmpId[OUT]:
	�� �� ֵ�� BOOL32 
	----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��		�汾		�޸���		�߶���    �޸�����
	2007/6/20   4.0			�ܹ��      ����      ����
	=============================================================================*/
	static BOOL32 GetMaxCapVMP( const TConfInfo &tConfInfo, u8 byConfIdx, 
								TPeriEqpStatus *ptVmpStatus, u16 wLen, 
								u8 &byVmpCapChnnlNum, 
                                u8 &bySelVmpId, 
								u8 byTargetStyle = 0	//0���Զ����
								);

	/*=============================================================================
	�� �� ���� GetSupportedVmpStyles
	��    �ܣ� �����ܹ�֧�ֵ����л���ϳɷ��MCU8000A��
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
	��    ���� const TConfInfo &tConfInfo
			   u8 byEqpType
			   u8 bySubType
			   u8 byCapChnnlNum             
			   u8 byVmpMaxChnnlNum
               u8 *pabyStyle                [out] ���صķ������
               u8 &byArraySize              [I/O] ��������Ĵ�С��ͬʱ����ʵ�ʴ�С��
                                                  ���ʵ�ʷ���ֵ���ڴ���ֵ����˵���ռ䲻����ͬʱ���������û�н������
	�� �� ֵ�� void 
	----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��		�汾		�޸���		�߶���    �޸�����
	2007/7/13   4.0			�ܹ��      ����      ����
	2009/2/18	4.6			Ѧ��					����������VMP���ࣻ����20���
	=============================================================================*/
	static void GetSupportedVmpStyles( const TConfInfo &tConfInfo, u8 byEqpType, u8 bySubType,u8 byCapChnnlNum, u8 byVmpMaxChnnlNum, 
                                       u8 *pabyStyle, u8 &byArraySize );
	/*=============================================================================
	�� �� ���� GetSelVmpCap
	��    �ܣ� ��ȡ��ӽ�����������vmp id��������
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
	��    ���� u8 abySelVmpIdx[]				[in] 
	TPeriEqpStatus *ptStatus		[in] 
	u8 bySelNum						[in]
	u8 byMaxChnlNumByConf			[in]
	u8 &byVmpCapChnnlNum			[out]
	u8 &bySelVmpId					[out]
	�� �� ֵ�� BOOL32
	----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��		�汾		�޸���		�߶���    �޸�����
	12/9/2008				Ѧ��					
	=============================================================================*/
	static BOOL32 GetSelVmpCap(u8 abySelVmpIdx[], TPeriEqpStatus *ptStatus, u8 bySelNum, 
		u8 byMaxChnlNumByConf,u8 &byVmpCapChnnlNum, u8 &bySelVmpId);
	
	/*====================================================================
    ������      : GetMaxChlNumByStyle
    ����        : �õ�����ϳɷ�������Ա��
    �㷨ʵ��    : 
    ����ȫ�ֱ���: ��
    �������˵��: byVMPStyle ����ϳɷ��
    ����ֵ˵��  : ��
	----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/31    3.0         ������       ����
	====================================================================*/
	static u8 GetMaxChlNumByStyle( u8 byVMPStyle );

	

private:
    static TMcu8kbPfmFilter m_tFilter;
};

// xliang [3/18/2009] VMP ռ����ǰ����ͨ������Ҫ������ǰ����ͨ����δռ�ɹ����ն���Ϣ
struct TVmpHdChnnlMemInfo
{
public:
	TMt	tMtInHdChnnl[MAXNUM_MPU2VMP_E20_HDCHNNL];
	TMt	tMtOutHdChnnl[MAXNUM_MPUSVMP_MEMBER];
public:
	TVmpHdChnnlMemInfo( void )
	{
		init();
	}
	void init()
	{
		memset(tMtInHdChnnl,0, sizeof(tMtInHdChnnl));
		memset(tMtOutHdChnnl,0,sizeof(tMtOutHdChnnl));
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// xliang [11/28/2008]VMP ͨ����Ա��Ϣ
struct TChnnlMemberInfo 
{
	enum TChnnlPriMask
    {
        Mask_BeSelected		=   0x0001,
		Mask_NoneKeda		=   0x0002,
		Mask_Speaker		=   0x0004,
		Mask_Dstream		=   0x0008,
		Mask_MMcu			=	0x0010,
		Mask_BeIpc			=	0x0020
    };
public:
//	u8	m_byMtId;			//ռ��ͨ����MT ID
	TMt m_tSeizedMt;		//ռ��ͨ����MT
	u8	m_byVmpPRIAttr;		//���ȼ�����
	u8  m_byReserved1;		//Ԥ��
	u8  m_byReserved2;		//Ԥ��
public:
	TChnnlMemberInfo(void)
	{
		SetNull();
	}
	void SetNull()
	{
		//m_byMtId = 0;
		m_tSeizedMt.SetNull();
		m_byVmpPRIAttr = 0;
	}
	void SetAttrNull()
	{
		m_byVmpPRIAttr = 0;
	}
	void SetAttrSelected()
	{
		m_byVmpPRIAttr |= Mask_BeSelected;
	}
	void SetAttrNoneKeda()
	{
		m_byVmpPRIAttr |= Mask_NoneKeda;
	}
	void SetAttrSpeaker()
	{
		m_byVmpPRIAttr |= Mask_Speaker;
	}
	void SetAttrDstream()
	{
		m_byVmpPRIAttr |= Mask_Dstream;
	}
	void SetAttrMMcu()
	{
		m_byVmpPRIAttr |= Mask_MMcu;
	}
	void SetAttrIPC()
	{
		m_byVmpPRIAttr |= Mask_BeIpc;
	}
	BOOL32 IsAttrNull() const
	{
		return ( m_byVmpPRIAttr == 0 );
	}
	BOOL32 IsCanAdjResMt() const
	{
		// �������뱻ѡ���ն��ǿɽ��ֱ���vip������Ϊ���ɽ��ֱ���vip
		if (IsAttrSpeaker() || IsAttrSelected())
		{
			return TRUE;
		}
		return FALSE;
	}
	BOOL32 IsAttrMMcu() const
	{
		return ( (m_byVmpPRIAttr & Mask_MMcu) != 0 );
	}
	BOOL32 IsAttrDstream() const
	{
		return ( (m_byVmpPRIAttr & Mask_Dstream) != 0 );
	}
	BOOL32 IsAttrSpeaker() const
	{
		return ( (m_byVmpPRIAttr & Mask_Speaker) != 0 );
	}
	BOOL32 IsAttrNoneKeda() const
	{
		return ( (m_byVmpPRIAttr & Mask_NoneKeda) != 0 );
	}
	BOOL32 IsAttrSelected() const
	{
		return ( (m_byVmpPRIAttr & Mask_BeSelected) != 0 );
	}
	
	BOOL32 IsAttrBeIPC() const
	{
		return ( (m_byVmpPRIAttr & Mask_BeIpc) != 0 );
	}
	u8 GetMtId( void ) const
	{
		return m_tSeizedMt.GetMtId();
	}
	TMt GetMt(void) const
	{
		return m_tSeizedMt;
	}
	void SetMt(TMt tMt)
	{
		m_tSeizedMt = tMt;
	}
}	
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//�ն�ִ�е���ռѡ�����
struct TSeizeChoice 
{
public:
//	u8	byMtId;
	TMt tSeizedMt;
	u8	byStopVmp;		//��MT�Ƿ�ֹͣVMP
	u8  byStopSelected;	//��MT�Ƿ�ֹͣ��ѡ��
	u8	byStopSpeaker;	//��MT�Ƿ�ȡ��������
public:
	TSeizeChoice(void)
	{
		init();
	}
	void init()
	{
		tSeizedMt.SetNull();
		byStopSelected = 0;
		byStopSpeaker  = 0;
		byStopVmp	   = 0;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//�ϴα���ʱ�����ĳͨ���е�VMP��Ա��Ϣ(�����˸����������)
struct TLastVmpchnnlMemInfo
{
public:
	u8 m_byLastVmpMemInChnnl;
	u8 m_byLastMemType;
public:
	TLastVmpchnnlMemInfo( void )
	{
		Init();
	}
	void Init()
	{
		m_byLastVmpMemInChnnl = ~0;
		m_byLastMemType = 0;
	}
	void SetLastVmpMemChnnl( u8 byMemChnnl ) { m_byLastVmpMemInChnnl = byMemChnnl; }
	u8	 GetLastVmpMemChnnl( void ) { return m_byLastVmpMemInChnnl; }
	void SetLastVmpMemType( u8 byMemType ) { m_byLastMemType = byMemType; }
	u8   GetLastVmpMemType (void ) { return m_byLastMemType; } 

};

//�¼�MCU�µ�VMP��Ա��Ϣ
struct TVmpCasMem
{
private:

	TMt	m_tMt;	
	u8	m_byPos;	// VMP�е�ͨ��λ��

public:
	TVmpCasMem( void )
	{
		Init();
	}
	void Init()
	{
		m_byPos = ~0;
		m_tMt.SetNull();
	}
	void SetMt( TMt tMt ) {m_tMt = tMt; }
	TMt  GetMt( void ) const { return m_tMt; }
	void SetPos( u8 byPos ) { m_byPos = byPos; }
	u8	 GetPos( void ) const { return m_byPos; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TVmpCasMemInfo
{
private:
	TVmpCasMem m_atVmpCasMem[MAXNUM_CONF_MT];

public:
	TVmpCasMemInfo (void)
	{
		Init();
	}
	void Init()
	{
		u8 byLoop;
		for(byLoop = 0; byLoop < MAXNUM_CONF_MT; byLoop ++)
		{
			m_atVmpCasMem[byLoop].Init();
		}
	}
	void SetMem(u8 byId, const TVmpCasMem tVmpCasMem) 
	{ 
		if( byId == 0 )
		{
			return;
		}

		m_atVmpCasMem[byId - 1].SetMt(tVmpCasMem.GetMt());
		m_atVmpCasMem[byId - 1].SetPos(tVmpCasMem.GetPos());
	}
	TMt GetMt( u8 byId ) const { return m_atVmpCasMem[byId - 1].GetMt(); }
	u8  GetPos(u8 byId ) const { return m_atVmpCasMem[byId - 1].GetPos(); }

};

//[03/04/2010] zjl add (����ǽ��ѯ����ϲ�)
//������ѯ��Ϣ 
#define POLLING_POS_START   (u8)0xFF
#define POLLING_POS_INVALID (u8)0xFE

struct TConfPollParam
{
protected:
	u8 m_byPollMtNum;  //�μ���ѯ�ն˸���
	u8 m_byCurPollPos; //��ѯ��ǰλ��
    u8 m_bySpecPos;    //�û�ǿ�����õ���ѯλ��

	// [12/21/2010 liuxu][�߶�]�����б�Ҫ����һ����ô������飿��ʹ��Ҫ���¼��ն��費��Ҫ�أ�
	TMtPollParam m_atMtPollParam[MAXNUM_CONF_MT]; //�μ���ѯ���ն˲����б�
	
	TMt m_tLastPolledMt;
public:
	
    TConfPollParam()
    {
        memset(this, 0, sizeof(TConfPollParam));
        ClearSpecPos();
    }
    
	void SetPollList(u8 byMtNum, TMtPollParam* ptParam)
	{
		if ( ptParam == NULL && byMtNum != 0)
		{
			return ;
		}
		m_byPollMtNum = byMtNum > MAXNUM_CONF_MT ? MAXNUM_CONF_MT : byMtNum;
		if ( m_byPollMtNum > 0 )
		{
			memcpy( m_atMtPollParam, ptParam, m_byPollMtNum * sizeof(TMtPollParam) );
		}    
	}

    void InitPollParam(u8 byMtNum, TMtPollParam* ptParam)
	{
		SetPollList(byMtNum, ptParam) ;		
		m_byCurPollPos = 0;
        ClearSpecPos();
		m_tLastPolledMt.SetNull();
	}

	void SetPolledMtNum(u8 byMtNum){m_byPollMtNum = byMtNum;} // xliang [12/31/2008] 
    u8   GetPolledMtNum() { return m_byPollMtNum; }

    TMtPollParam* const GetPollMtByIdx(u8 byIdx)
	{
		if ( byIdx < m_byPollMtNum )
		{
			return &m_atMtPollParam[byIdx];
		}
		else
		{
			return NULL;
		}    
	}
   
    void SetCurrentIdx(u8 byIdx){ m_byCurPollPos = byIdx;}
    u8   GetCurrentIdx() { return m_byCurPollPos;}

    TMtPollParam* const GetCurrentMtPolled(){ return &m_atMtPollParam[m_byCurPollPos];}
	
    BOOL32 IsExistMt(const TMt &tMt, u8 &byIdx)
	{
		byIdx = 0xFF;
		u8 byPos = 0;
		for( ; byPos < m_byPollMtNum; byPos++ )
		{
			if ( tMt.GetMcuId() == m_atMtPollParam[byPos].GetMcuId() && 
				tMt.GetMtId() == m_atMtPollParam[byPos].GetMtId() )
			{
				byIdx = byPos;
				return TRUE;
			}
		}		
        return FALSE;
	}
    void RemoveMtFromList(const TMt &tMt)
	{
		u8 byPos = 0;
		for( ; byPos < m_byPollMtNum; byPos++ )
		{
			if ( tMt == m_atMtPollParam[byPos] )
			{            
				//byPosΪ�ҵ����Ƴ����ն�����λ��
				//20100702_tzy �����ǰ��ѯλ�����Ƴ�λ��֮����ǰ��ѯλ����ǰ�ƶ�
				if (m_byCurPollPos > byPos)
				{
					m_byCurPollPos--;
				}
				// �ƶ��б�
				for( ; byPos < m_byPollMtNum-1; byPos++ )
				{
					m_atMtPollParam[byPos] = m_atMtPollParam[byPos+1];
				}
				//zjlhdupoll ���һ�������ĳ�ȥ
				memset(&m_atMtPollParam[byPos], 0, sizeof(TMtPollParam));
				m_byPollMtNum--;
				break;
			}
		}
	}
	
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

	TMt GetLastPolledMt( void )
	{
		return m_tLastPolledMt;
	}

	void SetLastPolledMt( TMt tMt )
	{
		m_tLastPolledMt = tMt;
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

//[03/04/2010] zjl add (����ǽ��ѯ����ϲ�)
struct TTvWallPollParam : public TConfPollParam
{
public:
	TTvWallPollInfo m_tTWPollnfo;
public:
	TTvWallPollParam()
	{
		memset( this, 0, sizeof(TTvWallPollParam) );
	}
	
    void SetTWPollInfo(TTvWallPollInfo tTWPollInfo){ m_tTWPollnfo = tTWPollInfo;}
    TTvWallPollInfo GetTWPollInfo(){ return m_tTWPollnfo; }
	
    void SetTvWall(TEqp tTvWall) { m_tTWPollnfo.SetTvWall(tTvWall); }
    TEqp GetTvWall(void) const{ return m_tTWPollnfo.GetTvWall(); } 
    
	void SetTWChnnl(u8 byChnnl) { m_tTWPollnfo.SetTWChnnl(byChnnl); }
    u8   GetTWChnnl(void) const { return m_tTWPollnfo.GetTWChnnl(); }
	
    void SetKeepTime( u32 byKeepTime ){ m_tTWPollnfo.SetKeepTime(byKeepTime); }
	u32  GetKeepTime( void ){ return m_tTWPollnfo.GetKeepTime(); } 
	
	void SetSchemeIdx(u8 bySchemeIdx) { m_tTWPollnfo.SetSchemeIdx(bySchemeIdx); }
    u8   GetSchemeIdx(void) const { return m_tTWPollnfo.GetSchemeIdx(); }
    
	void SetPollState(u8 byPollState){ m_tTWPollnfo.SetPollState(byPollState);}
	u8   GetPollState(){ return m_tTWPollnfo.GetPollState();}
    
	void SetPollNum(u32 dwPollNum){ m_tTWPollnfo.SetPollNum(dwPollNum);}
	u32  GetPollNum(){ return m_tTWPollnfo.GetPollNum(); }
	
    void SetMediaMode(u8 byMode){m_tTWPollnfo.SetMediaMode(byMode);}
	u8   GetMediaMode(){ return m_tTWPollnfo.GetMediaMode();}
	
	void SetMtPollParam(TMtPollParam tMtPollParam){ m_tTWPollnfo.SetMtPollParam(tMtPollParam);}
    TMtPollParam GetMtPollParam() const{ return m_tTWPollnfo.GetMtPollParam(); }
	
	void SetConfIdx(u8 byConfIdx){ m_tTWPollnfo.SetConfIdx(byConfIdx); }
	u8   GetConfIdx(void) const { return m_tTWPollnfo.GetConfIdx(); }
	
	u8   GetIsStartAsPause( void ){ return m_tTWPollnfo.GetIsStartAsPause(); }                                
	void SetIsStartAsPause( u8 byIsStartAsPause ){ m_tTWPollnfo.SetIsStartAsPause(byIsStartAsPause); }	   
	u16  GetChnIID(){ return MAKEWORD( m_tTWPollnfo.GetTWChnnl(), m_tTWPollnfo.GetTvWall().GetEqpId()); }
};

//Vmp��ͨ����ѯ��Ϣ(Vmp������ѯ����ϲ�)(len:42)
struct TVmpPollInfo : public TPollInfo
{
public:
	u8 m_byCurPollMtNum;		//��ǰ�ѽ�����ѯ��mt��Ŀ(Vmp������ѯ��)
	u8 m_byChnlIdx;				//��ǰ�õ�vmpͨ��(Vmp������ѯ��)
	u8 m_byFirst;				//��һ����(Vmp������ѯ��)
	u8 m_byVmpPollOver;			//vmpһ����ѯ����(Vmp������ѯ��)
	//	TMtPollParamEx atMtPollParamEx[192];	//�ն˶�Ӧ��vmpͨ���� FIXME��vcinst���и����Ƶ�
	u8	m_abyCurPollBlokMtId[MAXNUM_MPUSVMP_MEMBER];	//��ŵ�ǰ���������MTid(Vmp������ѯ��)
    
public:
	TVmpPollInfo(void){ memset( this, 0x0, sizeof( TVmpPollInfo ) ); }
	
}
/*
#ifndef WIN32
__attribute__((packed))
#endif
*/
PACKED
;

//����ϳ���ѯ����(Vmp������ѯ����ϲ�)
struct TVmpPollParam: public TConfPollParam
{
protected:
	TVmpPollInfo m_tVmpPollnfo;

public:
	TVmpPollParam()
	{
		memset( this, 0, sizeof(TVmpPollParam) );
	}
	
	// vmp��ͨ����ѯ��,Vmp��ͨ����ѯ��Ϣ�ӿ�
    void SetVmpPollInfo(TVmpPollInfo tVmpPollInfo){ m_tVmpPollnfo = tVmpPollInfo;}
    TVmpPollInfo GetVmpPollInfo(){ return m_tVmpPollnfo; }
	
	// vmp��ͨ����ѯ��,��ǰ����ѯ�㲥���ն˼�������ӿ�
	void SetMtPollParam(TMtPollParam tMtPollParam){ m_tVmpPollnfo.SetMtPollParam(tMtPollParam);}
    TMtPollParam GetMtPollParam() const{ return m_tVmpPollnfo.GetMtPollParam(); }
	
	// vmp��ͨ����ѯ��,��ѯ״̬�ӿ�
	void SetPollState(u8 byPollState){ m_tVmpPollnfo.SetPollState(byPollState);}
	u8   GetPollState(){ return m_tVmpPollnfo.GetPollState();}
    
	// vmp��ͨ����ѯ��,�ն���ѯ�Ĵ����ӿ�
	void SetPollNum(u32 dwPollNum){ m_tVmpPollnfo.SetPollNum(dwPollNum);}
	u32  GetPollNum(){ return m_tVmpPollnfo.GetPollNum(); }
	
	// vmp��ͨ����ѯ��,��ѯģʽ�ӿ�
    void SetMediaMode(u8 byMode){m_tVmpPollnfo.SetMediaMode(byMode);}
	u8   GetMediaMode(){ return m_tVmpPollnfo.GetMediaMode();}
	
	// ���¶�Ϊvmp������ѯ��ѯ�ýӿ�,��vmp��ͨ����ѯ�޹�
	u8 GetVmpPolledMtNum()	{ return m_tVmpPollnfo.m_byCurPollMtNum; }
	void SetVmpPolledMtNum(u8 byCurPollMtNum){ m_tVmpPollnfo.m_byCurPollMtNum = byCurPollMtNum ;}
	
	u8 GetVmpChnnlIdx() { return m_tVmpPollnfo.m_byChnlIdx; }
	void SetVmpChnnlIdx( u8 byChnlIdx) {m_tVmpPollnfo.m_byChnlIdx = byChnlIdx; }

	void SetIsVmpPollOver(BOOL32 bVmpPollOver){m_tVmpPollnfo.m_byVmpPollOver = (bVmpPollOver == TRUE? 1:0);}
	BOOL32 IsVmpPollOver(){return (m_tVmpPollnfo.m_byVmpPollOver == 1)?TRUE:FALSE;}

	void SetIsBatchFirst(BOOL32 bFirst){ m_tVmpPollnfo.m_byFirst = (bFirst == TRUE)?1:0;}
	BOOL32 IsBatchFirst(){return (m_tVmpPollnfo.m_byFirst == 1)?TRUE:FALSE;}

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
			m_tVmpPollnfo.m_abyCurPollBlokMtId[byMemberId] = byMtId;
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
	==============================================================================*/
	BOOL32 IsMtInCurPollBlok(u8 byMtId,u8 *byMbId)
	{
		for(u8 byMemberId = 0; byMemberId < MAXNUM_MPUSVMP_MEMBER; byMemberId ++)
		{
			if(m_tVmpPollnfo.m_abyCurPollBlokMtId[byMemberId] == byMtId)
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
		return m_tVmpPollnfo.m_abyCurPollBlokMtId[byMemberId];
	}

};

struct TSpyVmpInfo //����ϳ���صĶ�ش���Ϣ
{
//	u8		m_byRes;	//�ֱ�������
	u8		m_byPos;	//ͨ��λ��
//	u8		m_byKeepOrgRes;	//������ʹ�¼������˷ֱ���(�����keda��)���ϼ����ǿ���������ȫ�ֱ��ʽ�
    u8		m_byMemberType;     //����ϳɳ�Ա����,�μ�vccommon.h�л���ϳɳ�Ա���Ͷ���
    u8		m_byMemStatus;      //��Ա״̬(MT_STATUS_CHAIRMAN, MT_STATUS_SPEAKER, MT_STATUS_AUDIENCE)
	TEqpNoConstruct    m_tVmp;             // ����ϳ�����
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TSpySwitchDstInfo
{
	//u8				m_byMcInstId;   // ĳ����ص�id
	u8				m_bySrcChnnl;		//Դͨ���ţ������ն�Ŀǰֻ��0�����������ж���ѡ��
	TTransportAddr	m_tDstVidAddr;		//Ŀ����Ƶ��ip��port
	TTransportAddr  m_tDstAudAddr;		//Ŀ����Ƶ��ip��port ��ĿǰVid��Aud��Ip����ͬ�ģ���port��ͬ��
private:
	u32   m_dwMcIp;       //���IP
    u32   m_dwMcSSRC;
public:
	void SetMcIp(u32 dwMcIp)
	{
		m_dwMcIp = htonl(dwMcIp);
		return;
	}
	
	u32 GetMcIp(void) const
	{
		return ntohl(m_dwMcIp);
	}

	void SetMcSSRC(u32 dwMcSSRC)
	{
		m_dwMcSSRC = htonl(dwMcSSRC);
		return;
	}
	
	u32 GetMcSSRC(void) const
	{
		return ntohl(m_dwMcSSRC);
	}
}

#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


struct TSpySwitchInfo     // ѡ����صĶ�ش���ʼ��Ϣ
{
	//u8    m_byMcInstId;   // ĳ����ص�id
	u8    m_byDstChlIdx;  // Ŀ��ͨ������
	u8    m_bySrcChlIdx;  // Դͨ��������
	TMtNoConstruct   m_tDstMt;       // Ŀ���նˣ���Ϊ��ؼ�أ�Ŀ�Ķ�Ϊ�գ�

private:
	u32   m_dwMcIp;       //���IP
	u32   m_dwMcSSRC;    //��������
public:
	void SetMcIp(u32 dwMcIp)
	{
		m_dwMcIp = htonl(dwMcIp);
		return;
	}

	u32 GetMcIp(void) const
	{
		return ntohl(m_dwMcIp);
	}

	void SetMcSSRC(u32 dwMcSSRC)
	{
		m_dwMcSSRC = htonl(dwMcSSRC);
		return;
	}
	
	u32 GetMcSSRC(void) const
	{
		return ntohl(m_dwMcSSRC);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TSpyHduInfo     // ���������ǽ��صĶ�ش���ʼ��Ϣ
{
	u8    m_bySrcMtType;  // ����ǽԴͨ����Ա����
	u8    m_bySchemeIdx;  // Ԥ��������
	u8    m_byDstChlIdx;  // HDUĿ��ͨ������
	TEqpNoConstruct  m_tHdu;         // ������Ϣ
private:
	u8    m_bySubChnIdx;	//HDU�໭����ͨ��
public:
	void SetSubChnIdx(u8 bySubChnIdx)
	{
		m_bySubChnIdx = bySubChnIdx;
	}
	u8 GetSubChnIdx(void) const
	{
		return m_bySubChnIdx;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TSpyTVWallInfo     // ������ǽ��صĶ�ش���ʼ��Ϣ
{
	u8    m_bySrcMtType;  // ����ǽԴͨ����Ա����
	u8    m_bySchemeIdx;  // Ԥ��������
	u8    m_byDstChlIdx;  // TVWallĿ��ͨ������
	TEqpNoConstruct  m_tTvWall;      // ������Ϣ
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TSpyPollInfo     // ��ѯ��صĶ�ش���ʼ��Ϣ
{
	u16   m_wKeepTime;        //�ն���ѯ�ı���ʱ�� ��λ:��(s)
	u8    m_byPollingPos;        //�ն���ѯ�Ĵ���
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TSpyRollCallInfo     // ��ش����������Ϣ
{
	TMtNoConstruct m_tCaller;        // ������
	TMtNoConstruct m_tOldCaller;     // �ϵĵ�����
	TMtNoConstruct m_tOldCallee;     // �ϵı������� 
	//u8			   m_byMcInstId;   // ĳ����ص�id
private:
	u32   m_dwMcIp;       //���IP
	u32   m_dwMcSSRC;     //��������
public:
	void SetMcIp(u32 dwMcIp)
	{
		m_dwMcIp = htonl(dwMcIp);
		return;
	}
	
	u32 GetMcIp(void) const
	{
		return ntohl(m_dwMcIp);
	}

	void SetMcSSRC(u32 dwMcSSRC)
	{
		m_dwMcSSRC = htonl(dwMcSSRC);
		return;
	}
	
	u32 GetMcSSRC(void) const
	{
		return ntohl(m_dwMcSSRC);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TSpyRecInfo     //¼���·��
{
	TRecStartPara m_tRecPara;
	u8   m_byRecMode;                 // 0����¼��1�ն�¼��
	TEqpNoConstruct m_tRec;                      // ¼�������           
	s8   m_szRecFullName[KDV_MAX_PATH];   // ¼������·��
	u16  m_wSerialNO;				  // �ظ�mcs����ˮ��
	//u8   m_byMcInstId;				  // ĳ����ص�id
private:
	u32   m_dwMcIp;       //���IP
	u32   m_dwMcSSRC;     //��������
public:
	void SetMcIp(u32 dwMcIp)
	{
		m_dwMcIp = htonl(dwMcIp);
		return;
	}
	
	u32 GetMcIp(void) const
	{
		return ntohl(m_dwMcIp);
	}

	void SetMcSSRC(u32 dwMcSSRC)
	{
		m_dwMcSSRC = htonl(dwMcSSRC);
		return;
	}
	
	u32 GetMcSSRC(void) const
	{
		return ntohl(m_dwMcSSRC);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


union USpyInfo
{
	TSpySwitchInfo   m_tSpySwitchInfo;   // ѡ���������Ϣ
	TSpyHduInfo      m_tSpyHduInfo;      // ���������ǽ�������Ϣ
	TSpyPollInfo     m_tSpyPollInfo;     // ��ѯ�����Ϣ
	TSpyTVWallInfo   m_tSpyTVWallInfo;   // ������ǽ�������Ϣ
	TSpyVmpInfo	     m_tSpyVmpInfo;		 // VMP��ض�����Ϣ
	TSpyRollCallInfo m_tSpyRollCallInfo; // ���������Ϣ
	TSpyRecInfo		 m_tSpyRecInfo;		 //¼���
	TSpySwitchDstInfo	m_tSpySwitchDstInfo;	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMultiCacMtInfo
{
public:
	//TMt         m_tMt;
	u8          m_byCasLevel; //����뱾�����ն������������е���Ծ�ļ�����
	u8          m_abyMtIdentify[MAX_CASCADELEVEL]; //��Ӧ�������ն˵ı�ʶ 
public:
	TMultiCacMtInfo()
	{
		memset( this,0,sizeof( TMultiCacMtInfo ) );
	}

	BOOL32 IsNull()
	{
		for (u8 byCasLvl = 0; byCasLvl < m_byCasLevel;byCasLvl++)
		{
			if (m_abyMtIdentify[byCasLvl] != 0)
			{
				return FALSE;
			}
		}
		return TRUE;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMsgHeadMsg
{
public:

	TMultiCacMtInfo m_tMsgSrc; // ��ϢԴ
	TMultiCacMtInfo m_tMsgDst;// ��ϢĿ��
public:
	TMsgHeadMsg()
	{
		memset( this,0,sizeof(TMsgHeadMsg) );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TPreReleaseMtInfo
{
	TMt         m_tMt;
	u8          m_byCasLevel; //����뱾�����ն������������е���Ծ�ļ�����
	u8          m_abyMtIdentify[MAX_CASCADELEVEL]; //��Ӧ�������ն˵ı�ʶ 
	u8 m_byCanReleaseMode;	//���ͷ�ģʽ
private:
	s16 m_swCount;			//�����mt��Ϣ�Ǳ����µ�mcu��Ϣ��С��0 ,����Ϊ0
public:
	u8 m_byIsNeedRelease;	//���ظ��ϼ�mcu�жϸÿ��ͷ��ն������Ƿ���Ҫ�ͷ�
	u8 m_byCanReleaseVideoDstNum;//�����ͷŵ���ƵĿ����
	u8 m_byCanReleaseAudioDstNum;//�����ͷŵ���ƵĿ����
	u8 m_byIsReuseBasChl;//�Ƿ����ÿ��ͷ��ն���ռ�õ�basͨ��
						 //==MODE_VIDEO��Ƶbas�ɸ��ã�==MODE_AUDIO��Ƶbas�ɸ��ã�MODE_BOTH����Ƶbas���ɸ���
	
public:
	TPreReleaseMtInfo()
	{
		memset( this, 0, sizeof(TPreReleaseMtInfo) );
	}

	//����������[3/7/2012 chendaiwei]
	void SetCount( s16 swCount )
	{
		m_swCount = htons(swCount);
	}

	//����������
	s16 GetCount( void ) const
	{
		return ntohs(m_swCount);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// [pengjie 2010/9/11] ��ش��߼�������һ�������ݳ�Ա��Ϊ˽�еĲ��ٱ�¶���ⲿ���ⲿֱ�ӵ��ӿ�
struct TSpyResource         // ��ش�������Ϣ (len: 36)
{
public:
	TMt		       m_tSpy;		     // �ش�Դ(Mt)
	TSimCapSet     m_tSimCapset;     // ��Ӧ�ش�ʵ����������� ( �ûش�Դ ���ش����� �ش�Ŀ�Ķ˵� ���� )
	TTransportAddr m_tSpyAddr;       // ��Ӧ�ش�ʵ��Ļش�ͨ����ַ��Ϣ
	TTransportAddr m_tVideoRtcpAddr; // [liu lijiu][20100823]��¼�¼�����RTCP�ĵ�ַ
	TTransportAddr m_tAudioRtcpAddr; // [liu lijiu][20100823]��¼�¼��������RTCP�ĵ�ַ
	u8		       m_bySpyMode;	     // AUDIO,VIDEO,BOTH
	u8             m_byReserved1;    // Ԥ���ֶ�
	u32            m_dwReserved2;    // Ԥ���ֶ�
	
public:
	TSpyResource()
	{
		Clear();
	}
	
	u16 GetSpyChnnl()
	{
		return ( m_tSpyAddr.GetPort()  - CASCADE_SPY_STARTPORT ) / PORTSPAN;
	}

	void Clear( void )
	{
		memset( this, 0, sizeof(TSpyResource) );
		m_tSimCapset.Clear();
	}
	
	TMt GetSpyMt( void ) const { return m_tSpy; }
	void SetSpyMt( const TMt &tSpyMt ) { m_tSpy = tSpyMt; }
	
	TSimCapSet GetSimCapset( void ) const { return m_tSimCapset; }
	void SetSimCapset( const TSimCapSet &tSimCapset ) { m_tSimCapset = tSimCapset; }
	
	TTransportAddr GetSpyAddr( void ) { return m_tSpyAddr; }
	void SetSpyAddr( const TTransportAddr &tSpyAddr ) { m_tSpyAddr = tSpyAddr; }
	
	u8 GetSpyMode( void ) { return m_bySpyMode; }
	void SetSpyMode( const u8 bySpyMode ) { m_bySpyMode = bySpyMode; }
	
	void SetVidSpyBackRtcpAddr(TTransportAddr tVideoRtcpAddr) { m_tVideoRtcpAddr = tVideoRtcpAddr; }
	TTransportAddr GetVidSpyBackRtcpAddr(void) const { return m_tVideoRtcpAddr; }

	void SetAudSpyBackRtcpAddr(TTransportAddr tAudioRtcpAddr) { m_tAudioRtcpAddr = tAudioRtcpAddr; }
	TTransportAddr GetAudSpyBackRtcpAddr(void) const { return m_tAudioRtcpAddr; }
	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
// End

// [pengjie 2010/9/11] ��ش��߼�����
struct TPreSetInReq            // �ش���ʼ��Ϣ
{
//protected: ��һ�����ٱ�¶��Ա��Ϣ
private:
	u32 m_dwEvId;             // ��¼�ڽ��лش�����ǰ������Ӧ��ִ�е���Ϣ����ı䷢���ˣ��ն�ѡ���ȣ�
public:
	u8  m_bySpyMode;          // ����Ļش�ģʽ
    TSpyResource m_tSpyMtInfo;    // ����ش����ն���Ϣ
	USpyInfo    m_tSpyInfo;   // ��ش���ҵ����ص�һЩ��Ҫ����Ϣ
	TPreReleaseMtInfo m_tReleaseMtInfo;
	
public:
	TPreSetInReq()
	{
		memset( this, 0, sizeof(TPreSetInReq) );
	}

	//���ô���������[3/7/2012 chendaiwei]
	void SetEvId( u32 dwEvId)
	{
		m_dwEvId = htonl(dwEvId);
	}
	
	//��ȡ����������
	u32 GetEvId( void ) const
	{
		return ntohl(m_dwEvId);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//[nizhijun 2010/12/15]��ش������ش������Ϣ
struct  TMultiRtcpInfo
{
public:
	TMt					m_tSrcMt;			//�ش�Դ��Ϣ
	u8					m_bySpyMode;		//�ش�ģʽ MODE_AUDIO/MODE_VIDEO/MODE_BOTH
	TTransportAddr		m_tVidRtcpAddr;		//��ƵRTCP������Ϣ
	TTransportAddr      m_tAudRtcpAddr;		//��ƵRTCP������Ϣ
public:
	TMultiRtcpInfo()
	{
		memset( this, 0 , sizeof(TMultiRtcpInfo) ); 
	}
}
#ifndef WIN32
__attribute__( (packed) )
#endif
;

// struct TPreSetInReq            // �ش���ʼ��Ϣ
// {
// // [pengjie 2010/7/30] TPreSetInReq �ṹ����, ��Ҫ�����ش�Ŀ�Ķ���Ϣ
// 	TMt m_tSrc;               // �ش���Դ
// 	u32 m_dwEvId;             // ��¼�ڽ��лش�����ǰ������Ӧ��ִ�е���Ϣ����ı䷢���ˣ��ն�ѡ���ȣ�
// 	u8  m_bySpyMode;          // ����Ļش�ģʽ
// 	TSimCapSet	m_tSimCapSet; // Ŀ�Ķ˵�����
// 	USpyInfo    m_tSpyInfo;   // ��ش���ҵ����ص�һЩ��Ҫ����Ϣ
// 	TPreReleaseMtInfo m_tReleaseMtInfo;
// 
// public:
// 	TPreSetInReq()
// 	{
// 		memset( this, 0, sizeof(TPreSetInReq) );
// 	}
// }
// #ifndef WIN32
// __attribute__ ( (packed) ) 
// #endif
// ;


struct TPreSetInRsp			// PreSetIn Ӧ����Ϣ
{
// [pengjie 2010/7/30] TPreSetInReq �ṹ����, ��Ҫ�����ش�Ŀ�Ķ���Ϣ
	TPreSetInReq m_tSetInReqInfo; // ��¼�ϼ�Ҫ��Ļش������Ϣ
	u8  m_byRspSpyMode;		// �¼������ܴﵽ�Ļش�ģʽ(BOTH/V/A )
	
private:
	u32	m_dwVidSpyBW;		// �ش���Ƶ��ռ�ö��ٴ���
	u32	m_dwAudSpyBW;		// �ش���Ƶ��ռ�ö��ٴ���
// 	TTransportAddr m_tAudBackRtcp;//[liu lijiu][20100901]��¼��������Ϊ�¼�����������ƵRTCP�˿�
// 	TTransportAddr m_tVidBackRtcp;//[liu lijiu][20100901]��¼��������Ϊ�¼�����������ƵRTCP�˿�
	
public:
	TPreSetInRsp()
	{
		memset( this, 0, sizeof(TPreSetInRsp) );
	}

	void SetVidSpyRtcpAddr(TTransportAddr tVideoRtcpAddr)
	{
		m_tSetInReqInfo.m_tSpyMtInfo.SetVidSpyBackRtcpAddr( tVideoRtcpAddr );
	}
	void SetAudSpyRtcpAddr(TTransportAddr tAudioRtcpAddr)
	{
		m_tSetInReqInfo.m_tSpyMtInfo.SetAudSpyBackRtcpAddr( tAudioRtcpAddr );
	}
	TTransportAddr GetVidSpyRtcpAddr(void) const
	{
		return m_tSetInReqInfo.m_tSpyMtInfo.GetVidSpyBackRtcpAddr();
	}
	TTransportAddr GetAudSpyRtcpAddr(void) const
	{
		return m_tSetInReqInfo.m_tSpyMtInfo.GetAudSpyBackRtcpAddr();
	}

	//������������� [3/7/2012 chendaiwei]
	void SetVidSpyBW( u32 dwSpyBW )
	{
		m_dwVidSpyBW = htonl(dwSpyBW);
	}

	//�������������
	void SetAudSpyBW( u32 dwSpyBw )
	{
		m_dwAudSpyBW = htonl(dwSpyBw);
	}

	//����������  [3/7/2012 chendaiwei]
	u32 GetVidSpyBW ( void )
	{
		return ntohl(m_dwVidSpyBW);
	}

	//����������
	u32 GetAudSpyBW ( void )
	{
		return ntohl(m_dwAudSpyBW);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// struct TSpyResource         // ��ش�������Ϣ
// {
// public:
// 	TMt		m_tSpy;		    // ������MT��Ҳ������EQP(��mixer)
//     u16     m_wSpyStartPort;    // ��¼SpyChnnl, ��ֵ * PORTSPAN + CASCADE_SPY_STARTPORT  ��ӦMMCU ת�����ϵĻش�port
// 	u8		m_bySpyMode;	// AUDIO,VIDEO,BOTH
// 	
// 	u8		m_byOldSpyMode;   // �����ֶ�
// 	u32		m_dwReserved;
// 
// public:
// 	TSpyResource()
// 	{
// 		memset( this, 0, sizeof(TSpyResource) );
// 	}
// 
// 	u16 GetSpyChnnl()
// 	{
// 		return (m_wSpyStartPort  - CASCADE_SPY_STARTPORT ) / PORTSPAN;
// 	}
// }
// #ifndef WIN32
// __attribute__ ( (packed) ) 
// #endif
// ;

// [1/13/2010 xliang] 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<8000E �����������>>>>>>>>>>>>>>>>>>>>>>>>>
/*=============================================================================
ģ���ࣺ
������ Link
���ã� ����ڵ�
˵���� �����ر��ʼ����Ҫ����TӦ�����ṩ��Ӧ���޲������캯��
=============================================================================*/
template <class T>
class ArrayNode
{
private:
    T m_Data;                                   // ���ڱ�����Ԫ�ص�����
    u32 m_dwNextId;                             // ָ���̽���ָ��

public:      
    ArrayNode() { m_dwNextId = 0; }    
    ArrayNode(const T &Data, u32 dwNextId = 0)
    {
        SetData( Data );
        SetNext( dwNextId );
    }
    
    T * GetData( void ) { return &m_Data; }    
    void SetData( const T &Data ) { m_Data = Data; }    
    u32 GetNext( void ) { return ntohl(m_dwNextId); }
    void SetNext( u32 dwNextId ) { m_dwNextId = htonl(dwNextId); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*=============================================================================
ģ���ࣺ
������ LinkArray
���ã� ����һ����������
˵���� 1�������ر��ʼ����Ҫ����TӦ�����ṩ��Ӧ���޲������캯��
2��Ԫ��TӦ֧����������� == : ��Ҫ��ȡ��
3��2009-02-26��ȡ�� IsExist �� GetPos ����������Ϊ������������Ҫ ��T ����
����� == ��ͨ���Բ�ǿ
=============================================================================*/
template <class T, u32 dwArrayLen=1>
class LinkArray
{
private: 
    ArrayNode<T> m_atLinkArray[dwArrayLen];
    u32     m_dwHeadId;
    u32     m_dwTailId;                                  // �������ͷ��βID��, �����ŵ���ID�ż�1

private: 
    u32     GetIdByLink( ArrayNode<T> * pCur )
    {
        u32 dwArraySize = sizeof(m_atLinkArray)/sizeof(m_atLinkArray[0]);
        u32 dwIdx = 0;
        for ( dwIdx = 0; dwIdx < dwArraySize; dwIdx++ )
        {
            if ( pCur == &(m_atLinkArray[dwIdx]) )
            {
                return (dwIdx+1);
            }
        } 
        return 0;
    }
    ArrayNode<T> * GetLinkById( u32 dwId )
    {
        if ( 0 == dwId || dwId > sizeof(m_atLinkArray)/sizeof(m_atLinkArray[0]) )
        {
            return NULL;
        }
        ArrayNode<T> * pCur = &(m_atLinkArray[dwId-1]);
        return pCur;
    }

    u32     GetHeadId( void ) const { return ntohl(m_dwHeadId); }
    void    SetHeadId( u32 dwHeadId ) { m_dwHeadId = htonl(dwHeadId); }

    u32     GetTailId( void ) const { return ntohl(m_dwTailId); }
    void    SetTailId( u32 dwTailId ) { m_dwTailId = htonl(dwTailId); }

    // ��ÿ��д洢�ռ�ID
    u32 GetIdleId(void)
    {
        u32 dwArraySize = sizeof(m_atLinkArray)/sizeof(m_atLinkArray[0]);
        u32 dwIdx = 0;
        for ( dwIdx = 0; dwIdx < dwArraySize; dwIdx++ )
        {
            if ( NULL == m_atLinkArray[dwIdx].GetNext() &&
                GetTailId() != (dwIdx+1) )
            {
                return (dwIdx+1);
            }
        }
        return 0;
    }
     // �������Ա�ָ���p��Ԫ�ص�ָ��ֵ
    ArrayNode<T> * GetLink(const u32 &dwPos)        
    {
        if ( dwPos < 0 || dwPos >= Length() )
        {
            return NULL;
        }
        
        u32 dwCount = 0;
        ArrayNode<T> *pCur = GetLinkById( GetHeadId() );
        while ( pCur != NULL && dwCount < dwPos )
        {
            u32 dwNextId = pCur->GetNext();
            pCur = GetLinkById(dwNextId);
            dwCount++;
        }       
        return pCur;
    }   

public: 
    LinkArray() { Clear(); }         // ���캯��
    ~LinkArray() { Clear(); }        // ��������
    // �ж������Ƿ�Ϊ��
    BOOL32 IsEmpty() const { return ( 0 == m_dwHeadId || 0 == m_dwTailId ); }
    // ������洢�������������Ϊ�ձ�
    void   Clear()                                    
    {
        m_dwHeadId = 0;
        m_dwTailId = 0;
        u32 dwArraySize = sizeof(m_atLinkArray)/sizeof(m_atLinkArray[0]);
        u32 dwIdx = 0;
        for ( dwIdx = 0; dwIdx < dwArraySize; dwIdx++ )
        {
            m_atLinkArray[dwIdx].SetNext( 0 );
        }
    }
    // ���ش�˳���ĵ�ǰʵ�ʳ���
    u32 Length() 
    {
        ArrayNode<T> *pCur = GetLinkById( GetHeadId() );
        u32 dwCount = 0;
        
        while ( pCur != NULL ) 
        {
            u32 dwNextId = pCur->GetNext();
            pCur = GetLinkById(dwNextId);
            dwCount++;
        }
        return dwCount;
    }    
    // �ڱ�β���һ��Ԫ��value����ĳ�����1
    BOOL32 Append(const T &Value)                      
    {
        u32 dwIdleId = GetIdleId();
        ArrayNode<T> *pCur = GetLinkById( dwIdleId );
        if ( NULL == pCur )
        {
            return FALSE;
        }       
        pCur->SetData( Value );
        pCur->SetNext( 0 );
        
        u32 dwOldTailId = GetTailId();
        SetTailId(dwIdleId);

        ArrayNode<T> * pTail = GetLinkById( dwOldTailId );
        if ( NULL != pTail )
        {
            pTail->SetNext( dwIdleId );
        }
        else
        {
            SetHeadId(dwIdleId);
        }        
        return TRUE;
    }
    // �ڵ�dwPos��λ�ò�����������ΪValue���½��
    BOOL32 Insert(const u32 &dwPos, const T &Value)    
    {
        ArrayNode<T> *pCur = NULL;
        ArrayNode<T> *pNext = NULL;
        
        u32 dwIdleId = GetIdleId();
        pNext = GetLinkById( dwIdleId ); 
        if ( NULL == pNext )
        {
            return FALSE;
        }
        
        if ( dwPos == 0 )
        {
            pNext->SetData( Value );
            pNext->SetNext( GetHeadId() );
            SetHeadId( dwIdleId );
            if ( 0 == GetTailId() )
            {
                // ԭ���ı�Ϊ��
                SetTailId( dwIdleId );
            }
        }
        else 
        {
            if ( (pCur = GetLink(dwPos-1) ) == NULL) 
            {                                                     
                return FALSE;
            }
            pNext->SetData( Value );
            pNext->SetNext( pCur->GetNext() );
            pCur->SetNext( dwIdleId );
            u32 dwPreId = GetIdByLink(pCur);
            if ( dwPreId == GetTailId() )                                          
            {
                SetTailId( dwIdleId );
            }
        }    
        return TRUE;
    }
    // ɾ��λ��i�ϵ�Ԫ�أ���ĳ��ȼ� 1
    BOOL32 Delete(const u32 &dwPos)                    
    {
        ArrayNode<T> *pCur = NULL;
        ArrayNode<T> *pPre = NULL; 
        
        pCur = GetLink( dwPos );
        if ( NULL == pCur )
        {
            return FALSE;
        }
        if ( dwPos > 0 )
        {
            pPre = GetLink( dwPos-1 );
            if ( NULL == pPre )
            {
                return FALSE;
            }
        }

        u32 dwNext = pCur->GetNext();
        pCur->SetNext( 0 );
        if ( NULL == pPre )
        {                           
            SetHeadId( dwNext );                  
        }
        else
        {
            pPre->SetNext( dwNext );         
        }
        if ( GetTailId() == GetIdByLink(pCur) )
        {
            if ( dwNext != 0 )
            {
                SetTailId( dwNext );
            }
            else
            {
                SetTailId( GetIdByLink(pPre) );
            }
        } 
        return TRUE;
    }

    // ����λ��i��Ԫ��ֵ 
    T * GetValue(const u32 &dwPos)            
    {
        ArrayNode<T> *p = GetLink(dwPos);
        if ( p == NULL )
        {
            return NULL;
        }
        else
        {
            return p->GetData();
        }
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TNetAdaptInfo
{
protected:
    s8  m_achAdapterName[MCU_MAX_ADAPTER_NAME_LENGTH + 4];
    s8  m_achDescription[MCU_MAX_ADAPTER_DESCRIPTION_LENGTH + 4];
    u32 m_dwMacAddressLength;
    u8  m_abyMacAddress[MCU_MAX_ADAPTER_ADDRESS_LENGTH];
    u32 m_dwIndex;
    u32 m_dwType;
	u32 m_dwDefGWIpAddr;	// [3/25/2010 xliang] ������Ĭ������ip��ַ
    TNetParam m_tCurrentIpAddress;

public:
    LinkArray<TNetParam, MCU_MAXNUM_ADAPTER_IP> m_tLinkIpAddr;          // ������IP��ַ
    LinkArray<TNetParam, MCU_MAXNUM_ADAPTER_GW> m_tLinkDefaultGateway;  // ������Ĭ������

protected:
    void SetMacAddressLength( u32 dwMacLength ) { m_dwMacAddressLength = htonl( dwMacLength ); }
public:
    void SetDefGWIpAddr(u32 dwDefIpAddr) { m_dwDefGWIpAddr = htonl(dwDefIpAddr); }
	u32  GetDefGWIpAddr() {return ntohl(m_dwDefGWIpAddr); }

public:
    
    TNetAdaptInfo(void) { Clear(); }
    ~TNetAdaptInfo(void) { Clear(); }

    void Clear( void )
    {
        memset( m_achAdapterName, '\0', sizeof(m_achAdapterName) );
        memset( m_achDescription, '\0', sizeof(m_achDescription) );
        memset( m_abyMacAddress, 0, sizeof(m_abyMacAddress) );
        m_dwMacAddressLength = 0;
        m_dwIndex = 0;
        m_dwType = 0;
        
        m_tCurrentIpAddress.SetNull();
        m_tLinkIpAddr.Clear();
        m_tLinkDefaultGateway.Clear();
    }
    void Print( void )
    {
        u32 dwIdx = 0;
        StaticLog("AdapterName: %s\n", GetAdapterName() );
		printf( "AdapterName: %s\n", GetAdapterName() );
        StaticLog("Description: %s\n", GetDescription() );
        StaticLog("MacAddress : ");
		printf("MacAddress : ");
        for ( dwIdx = 0; dwIdx < GetMacAddressLength(); dwIdx++ )
        {
            if ( dwIdx < GetMacAddressLength()-1)
            {
                StaticLog("%02x-", m_abyMacAddress[dwIdx]);
				printf("%02x-", m_abyMacAddress[dwIdx]);
            }
            else
            {
                StaticLog("%02x\n", m_abyMacAddress[dwIdx]);
				printf( "%02x\n", m_abyMacAddress[dwIdx]);
            }
        }
        StaticLog("AdaptIdx: 0x%x\n", GetAdaptIdx() );
		printf("AdaptIdx: 0x%x\n", GetAdaptIdx() );
        StaticLog("AdaptType: %d ", GetAdaptType() );
        switch ( GetAdaptType() )    //����������
        {
        case MCU_MIB_IF_TYPE_OTHER:
            StaticLog( "Other\n");
            break;
        case MCU_MIB_IF_TYPE_ETHERNET:
            StaticLog( "Ethernet\n");
            break;
        case MCU_MIB_IF_TYPE_TOKENRING:
            StaticLog( "Tokenring\n");
            break;
        case MCU_MIB_IF_TYPE_FDDI:
            StaticLog( "FDDI\n");
            break;            
        case MCU_MIB_IF_TYPE_PPP:
            StaticLog( "PPP\n");
            break;
        case MCU_MIB_IF_TYPE_LOOPBACK:
            StaticLog( "LoopBack\n");
            break;
        case MCU_MIB_IF_TYPE_SLIP:
            StaticLog( "Slip\n");
            break;    
        default:
            StaticLog( "Unknow\n");
            break;
        }
        StaticLog( "CurrentIpAddress: Ip - 0x%x, Mask - 0x%x\n", 
            m_tCurrentIpAddress.GetIpAddr(),
            m_tCurrentIpAddress.GetIpMask() );
            for ( dwIdx = 0; dwIdx < m_tLinkIpAddr.Length(); dwIdx++ )
            {
                StaticLog( "NetAddress[%d]: Ip - 0x%x, Mask - 0x%x\n", dwIdx,
                m_tLinkIpAddr.GetValue( dwIdx )->GetIpAddr(), 
                m_tLinkIpAddr.GetValue( dwIdx )->GetIpMask() );

				printf("NetAddress[%d]: Ip - 0x%x, Mask - 0x%x\n", dwIdx,
					m_tLinkIpAddr.GetValue( dwIdx )->GetIpAddr(), 
					m_tLinkIpAddr.GetValue( dwIdx )->GetIpMask() );
            }
            for ( dwIdx = 0; dwIdx < m_tLinkDefaultGateway.Length(); dwIdx++ )
            {
                StaticLog( "DefaultGW[%d]: Ip - 0x%x\n", dwIdx,
                    m_tLinkDefaultGateway.GetValue( dwIdx )->GetIpAddr() );
            }
    }

    // ������������������
    void SetAdapterName( s8 * pchAdapterName )
    {
        if ( NULL != pchAdapterName )
        {
            strncpy( m_achAdapterName, pchAdapterName, sizeof(m_achAdapterName)-4 );
            m_achAdapterName[sizeof(m_achAdapterName)-4] = '\0';
        }
        else
        {
            memset( m_achAdapterName, '\0', sizeof(m_achAdapterName) );
        }
        return;
    }
    s8 * GetAdapterName( void )
    {
        return m_achAdapterName; 
    }

    // ����������MAC��ַ����
    void SetMacAddress( u8 *pbyMacAddress, u32 dwLength )
    {
        if ( NULL != pbyMacAddress )
        {
            u8 *pbyMac = pbyMacAddress;
            u32 dwLop = 0;
            for ( dwLop = 0; dwLop < dwLength && dwLop < MCU_MAX_ADAPTER_ADDRESS_LENGTH; dwLop++ )
            {
                m_abyMacAddress[dwLop] = *pbyMac;
                pbyMac++;
            }
            SetMacAddressLength( dwLop );
        }
        else
        {
            memset( m_abyMacAddress, 0, sizeof(m_abyMacAddress) );
            SetMacAddressLength( 0 );
        }
        return;
    }
    u8 * GetMacAddress( void ) { return m_abyMacAddress; }

    u32  GetMacAddressLength( void ) const { return ntohl(m_dwMacAddressLength); }

    // ������������������
    void SetDescription( s8 * pchDescription )
    {
        if ( NULL != pchDescription )
        {
            strncpy( m_achDescription, pchDescription, sizeof(m_achDescription)-4 );
            m_achDescription[sizeof(m_achDescription)-4] = '\0';
        }
        else
        {
            memset( m_achDescription, '\0', sizeof(m_achDescription) );
        }
        return;
    }
    s8 * GetDescription( void ) { return m_achDescription; }

    // ���������������Ų���
    void SetAdaptIdx( u32 dwIdx ) {m_dwIndex = htonl(dwIdx);}
    u32  GetAdaptIdx( void ) const { return ntohl(m_dwIndex); }

    // ��������������
    void SetAdaptType( u32 dwType ) { m_dwType = htonl(dwType); }
    u32  GetAdaptType( void ) const { return ntohl(m_dwType); }

    void SetCurretnIpAddr( const TNetParam &tCurIp ) 
    {
        m_tCurrentIpAddress = tCurIp; 
    }
    TNetParam GetCurrentIpAddr(void) const 
    { 
        return m_tCurrentIpAddress; 
    }

    s32 GetIpAddrPos( u32 dwIpAddr )
    {
        u32 dwIpNum = m_tLinkIpAddr.Length();
        for ( u32 dwIdx = 0; dwIdx < dwIpNum; dwIdx++ )
        {
            TNetParam tLinkParam = *(m_tLinkIpAddr.GetValue(dwIdx) );
            if ( tLinkParam.GetIpAddr() == dwIpAddr )
            {
                return dwIdx;
            }
        }
        return -1;
    }

    s32 GetDefaultGWPos( u32 dwGWAddr )
    {
        u32 dwIpNum = m_tLinkDefaultGateway.Length();
        for ( u32 dwIdx = 0; dwIdx < dwGWAddr; dwIdx++ )
        {
            TNetParam tLinkParam = *(m_tLinkDefaultGateway.GetValue(dwIdx) );
            if ( tLinkParam.GetIpAddr() == dwGWAddr )
            {
                return dwIdx;
            }
        }
        return -1;
    }
    
    TNetAdaptInfo & operator = (TNetAdaptInfo tNetAdapt)
    {
        if ( this != &tNetAdapt )
        {
            this->Clear();
            this->SetAdapterName( tNetAdapt.GetAdapterName() );
            this->SetAdaptIdx( tNetAdapt.GetAdaptIdx() );
            this->SetAdaptType( tNetAdapt.GetAdaptType() );
            this->SetCurretnIpAddr( tNetAdapt.GetCurrentIpAddr() );
            this->SetDescription( tNetAdapt.GetDescription() );
            this->SetMacAddress( tNetAdapt.GetMacAddress(), tNetAdapt.GetMacAddressLength() );
			this->SetDefGWIpAddr(tNetAdapt.GetDefGWIpAddr());
            u32 dwLop = 0;
            u32 dwArrayLen = 0;
            // Ip array
            TNetParam *ptNetAddr = NULL;
            dwArrayLen = tNetAdapt.m_tLinkIpAddr.Length();
            for ( dwLop = 0; dwLop < dwArrayLen; dwLop++ )
            {
                ptNetAddr = tNetAdapt.m_tLinkIpAddr.GetValue(dwLop);
                if ( NULL == ptNetAddr )
                {
                    continue;
                }
                if ( !m_tLinkIpAddr.Append( *ptNetAddr ) )
                {
                    break;
                }
            }
            // GW
            TNetParam *ptGW = NULL;
            dwArrayLen = tNetAdapt.m_tLinkDefaultGateway.Length();
            for ( dwLop = 0; dwLop < dwArrayLen; dwLop++ )
            {
                ptGW = tNetAdapt.m_tLinkDefaultGateway.GetValue(dwLop);
                if ( NULL == ptGW )
                {
                    continue;
                }
                if ( !m_tLinkDefaultGateway.Append( *ptGW ) )
                {
                    break;
                }
            }
        }
        return *this;
    }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TNetAdaptInfoAll
{
public:
    LinkArray<TNetAdaptInfo, MCU_MAXNUM_ADAPTER> m_tLinkNetAdapt;
    
public:
    TNetAdaptInfoAll(void) { Clear(); }
    ~TNetAdaptInfoAll(void) { Clear(); }
    
    void Clear(void)
    {
        u32 dwAdaptNum = m_tLinkNetAdapt.Length();
        for ( u32 dwIdx = 0; dwIdx < dwAdaptNum; dwIdx++ )
        {
            TNetAdaptInfo * ptAdaptInfo = m_tLinkNetAdapt.GetValue(dwIdx);
            ptAdaptInfo->Clear();
        }
        m_tLinkNetAdapt.Clear();
    }
    
    TNetAdaptInfoAll & operator = (TNetAdaptInfoAll & tNetAdaptInfoAll)
    {
        if ( this != &tNetAdaptInfoAll )
        {
            this->Clear();
            u32 dwLop = 0;
            u32 dwArrayLen = tNetAdaptInfoAll.m_tLinkNetAdapt.Length();
            TNetAdaptInfo * ptNetAdapt = NULL;
            for ( dwLop = 0; dwLop < dwArrayLen; dwLop++ )
            {
                ptNetAdapt = tNetAdaptInfoAll.m_tLinkNetAdapt.GetValue( dwLop );
                if ( NULL == ptNetAdapt )
                {
                    continue;
                }
                if ( !this->m_tLinkNetAdapt.Append(*ptNetAdapt) )
                {
                    break;
                }
            }
        }
        return *this;
    }
    
    BOOL32 IsValid(void)
    {
        u32 dwAdapterNum = m_tLinkNetAdapt.Length();
        u32 dwIdx = 0;
        TNetAdaptInfo * ptAdapter = NULL;
        for ( dwIdx = 0; dwIdx < dwAdapterNum; dwIdx++ )
        {
            ptAdapter = m_tLinkNetAdapt.GetValue( dwIdx );
            if ( ptAdapter->m_tLinkIpAddr.Length() == 0 )
            {
                return FALSE;
            }
        }
        return TRUE;
    }

	 // ȡ��ETHID��ȵ�������Ϣ 
    TNetAdaptInfo *GetAdapterByEthId( u32 dwEthId) 
    { 
        TNetAdaptInfo * ptAdapter = NULL; 
        for ( u8 byLoop = 0; byLoop < m_tLinkNetAdapt.Length(); byLoop++) 
        { 
            ptAdapter = m_tLinkNetAdapt.GetValue(byLoop); 
            if ( ptAdapter != NULL && ptAdapter->GetAdaptIdx() == dwEthId) 
            { 
                return ptAdapter; 
            } 
        } 
        return NULL; 
    } 

    
    void Print(void)
    {
        u32 dwAdapterNum = m_tLinkNetAdapt.Length();
        u32 dwIdx = 0;
        TNetAdaptInfo * ptAdapter = NULL;
        for ( dwIdx = 0; dwIdx < dwAdapterNum; dwIdx++ )
        {
            ptAdapter = m_tLinkNetAdapt.GetValue( dwIdx );
            ptAdapter->Print();
        }
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//MCU 8000E �������ü�·������
struct TMcu8KECfg
{
protected:
	enUseModeType	m_enUseModeType;	// 3����ģʽ || ����ģʽ
	enNetworkType	m_enNetworkType;    // LAN or WAN 
	BOOL32			m_bIsUseSip;		// �Ƿ�����sip
	u8				m_bySipInEthIdx;	// SipIp���ĸ�������
	u32				m_dwSipIpAddr;		// SipIp
	u32				m_dwSipMaskAddr;	// SipMask
	u32				m_dwSipGwAddr;		// SipGw

public:
    LinkArray<TNetAdaptInfo, MCU_MAXNUM_ADAPTER> m_tLinkNetAdap;
    LinkArray<TRoute, MCU_MAXNUM_ADAPTER_ROUTE> m_tLinkRoute;        // ������̬·��

public:
    TMcu8KECfg(void) { Clear(); }
    ~TMcu8KECfg(void) { Clear(); }

    void Clear(void)
    {
		memset(this, 0, sizeof(*this));
		m_enNetworkType = enLAN;			//Ĭ������Ϊ˽��
		m_enUseModeType = MODETYPE_3ETH;	//Ĭ������Ϊ3����ģʽ

        u32 dwAdaptNum = m_tLinkNetAdap.Length();
        for ( u32 dAdaptwIdx = 0; dAdaptwIdx < dwAdaptNum; dAdaptwIdx++ )
        {
            TNetAdaptInfo * ptAdaptInfo = m_tLinkNetAdap.GetValue(dAdaptwIdx);
            ptAdaptInfo->Clear();
        }
        m_tLinkNetAdap.Clear();

        u32 dwRouteNum = m_tLinkRoute.Length();
        for ( u32 dwRouteIdx = 0; dwRouteIdx < dwRouteNum; dwRouteIdx++ )
        {
            TRoute * ptRoute = m_tLinkRoute.GetValue( dwRouteIdx );
            ptRoute->SetNull();
        }
        m_tLinkRoute.Clear();
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

    TMcu8KECfg & operator = (TMcu8KECfg & tMcuEqp)
    {
        if ( this != &tMcuEqp )
        {
            this->Clear();
            u32 dwLop = 0;
            u32 dwArrayLen = tMcuEqp.m_tLinkNetAdap.Length();
            TNetAdaptInfo tNetAdapt;
            for ( dwLop = 0; dwLop < dwArrayLen; dwLop++ )
            {
                tNetAdapt = *(tMcuEqp.m_tLinkNetAdap.GetValue( dwLop ));
                if ( !this->m_tLinkNetAdap.Append(tNetAdapt) )
                {
                    break;
                }
            }

            u32 dwRouteNum = tMcuEqp.m_tLinkRoute.Length();
            for ( u32 dwRouteIdx = 0; dwRouteIdx < dwRouteNum; dwRouteIdx++ )
            {
                TRoute tRoute = *(tMcuEqp.m_tLinkRoute.GetValue( dwRouteIdx ));
                if ( !this->m_tLinkRoute.Append(tRoute))
                {
                    break;
                }
            }

			this->SetUseModeType(tMcuEqp.GetUseModeType());
			this->SetNetworkType(tMcuEqp.GetNetworkType());
			this->SetIsUseSip(tMcuEqp.IsUseSip());
			this->SetSipInEthIdx(tMcuEqp.GetSipInEthIdx());
			this->SetSipIpAddr(tMcuEqp.GetSipIpAddr());
			this->SetSipMaskAddr(tMcuEqp.GetSipMaskAddr());
			this->SetSipGwAddr(tMcuEqp.GetSipGwAddr());
        }
        return *this;
    }
    
    BOOL32 IsValid(void)
    {
        u32 dwAdapterNum = m_tLinkNetAdap.Length();
        u32 dwIdx = 0;
        TNetAdaptInfo * ptAdapter = NULL;
        for ( dwIdx = 0; dwIdx < dwAdapterNum; dwIdx++ )
        {
            ptAdapter = m_tLinkNetAdap.GetValue( dwIdx );
            if ( ptAdapter->m_tLinkIpAddr.Length() == 0 )
            {
                return FALSE;
            }
        }
        return TRUE;
    }
    
    void Print(void)
    {
        u32 dwAdapterNum = m_tLinkNetAdap.Length();
        u32 dwIdx = 0;
        TNetAdaptInfo * ptAdapter = NULL;
        for ( dwIdx = 0; dwIdx < dwAdapterNum; dwIdx++ )
        {
            ptAdapter = m_tLinkNetAdap.GetValue( dwIdx );
            ptAdapter->Print();
        }

        u32 dwRouteNum = m_tLinkRoute.Length();
        for ( u32 dwRouteIdx = 0; dwRouteIdx < dwRouteNum; dwRouteIdx++ )
        {
            TRoute * ptRoute = m_tLinkRoute.GetValue( dwRouteIdx );
            StaticLog( "0x%x mask 0x%x gateway 0x%x metric %d\n", 
                                    ptRoute->GetDstNet().GetIpAddr(),
                                    ptRoute->GetDstNet().GetIpMask(),
                                    ptRoute->GetGateway(),
									ptRoute->GetMetric() );

			s8	achSipIpAddr[32] = {0};
			s8	achSipMaskAddr[32] = {0};
			s8	achSipGwAddr[32] = {0};
			u32 dwSipIpAddr = GetSipIpAddr();
			u32 dwSipMaskAddr = GetSipMaskAddr();
			u32 dwSipGwAddr = GetSipGwAddr();
			sprintf(achSipIpAddr, "%d.%d.%d.%d%c", (dwSipIpAddr>>24)&0xFF, (dwSipIpAddr>>16)&0xFF, (dwSipIpAddr>>8)&0xFF, dwSipIpAddr&0xFF, 0);
			sprintf(achSipMaskAddr, "%d.%d.%d.%d%c", (dwSipMaskAddr>>24)&0xFF, (dwSipMaskAddr>>16)&0xFF, (dwSipMaskAddr>>8)&0xFF, dwSipMaskAddr&0xFF, 0);
			sprintf(achSipGwAddr, "%d.%d.%d.%d%c", (dwSipGwAddr>>24)&0xFF, (dwSipGwAddr>>16)&0xFF, (dwSipGwAddr>>8)&0xFF, dwSipGwAddr&0xFF, 0);
			
			StaticLog("\n[TMcuNetCfg]:\nm_enUseModeType:%s\nm_enNetworkType:%s	\
				\nm_dwSipIpAddr:%s\nm_dwSipMakAddr:%s\nm_dwSipgwAddr:%s\n",	\
				(m_enUseModeType == MODETYPE_3ETH ? "MODETYPE_3ETH":"MODETYPE_BAK"),	\
				(m_enNetworkType == enLAN ? "enLAN":(m_enNetworkType == enWAN ? "enWAN":"enBOTH")),	\
				 achSipIpAddr,achSipMaskAddr,achSipGwAddr);
        }
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//add by zhanghb for 8000e 090409
struct TLicenseMsg
{
	u8      m_achLocalInfo[512];    //guard������local��Ϣ������MAC��HDD(����)
	s8      m_achExpireDate[16];    //��������
	u8      m_achLicenseSN[64];      //���к�
	u16     m_wMcuAccessNum;        //mcu��������
	u16     m_wMcuPcmtAccNum;       //mcu����������PCMT������
	u16	    m_wLicenseErrorCode;	//License������
	BOOL32  m_bLicenseIsValid;      //License�Ƿ�Ϸ�
	
public:
    TLicenseMsg()
    {
        memset( this, 0, sizeof(TLicenseMsg) );
    }
	
    TKdvTime GetExpireDate(void)
    {
        TKdvTime tDate;
        s8 * pchToken = strtok( m_achExpireDate, "-" );
        if( NULL != pchToken)
        {
            tDate.SetYear( atoi( pchToken ) );
            pchToken = strtok( NULL, "-" );
            if( NULL != pchToken )
            {
                tDate.SetMonth( atoi( pchToken ) );
                pchToken = strtok( NULL, "-" );
                if( NULL != pchToken )
                {
                    tDate.SetDay( atoi( pchToken ) );
                }
            }
        }
        return tDate;	
    }
	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMcuBrdEthParam{
    u32 dwIpAdrs;/*�����ֽ���*/
    u32 dwIpMask; /*�����ֽ���*/
    u8  byMacAdrs[6];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
/* ��̫�������ṹ */
struct TMcuBrdEthParamAll{
    u32 dwIpNum;/*��Ч��IP��ַ��*/
    TMcuBrdEthParam atBrdEthParam[MCU_MAXNUM_ADAPTER];/*���IP��ַ����Ϣ������*/
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TSwitchDstInfo
{
	TMt				m_tSrcMt;			//Դ
	u8				m_bySrcChnnl;		//Դͨ���ţ������ն�Ŀǰֻ��0�����������ж���ѡ��
	u8				m_byMode;			//V/A/BOTH
	TTransportAddr	m_tDstVidAddr;		//Ŀ����Ƶ��ip��port
	TTransportAddr  m_tDstAudAddr;		//Ŀ����Ƶ��ip��port ��ĿǰVid��Aud��Ip����ͬ�ģ���port��ͬ��
	u32				m_dwReserved;		//Ԥ��
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// [9/27/2011 liuxu] ģ���ྲ̬����������
template <class T, u32 dwBufNum >
class CStaticBuf
{
public:
	CStaticBuf( ) { ClearAll(); }
	
public:
	// ȫ�����
	void ClearAll( ) { memset(this, 0, sizeof( CStaticBuf<T, dwBufNum> )); }
	
	// ������������
	u32 GetCapacity() const { return dwBufNum; }
	// ��ʹ����
	u32	GetUsedNum() const { return m_dwNum; }
	// �Ƿ�����
	BOOL32 IsFull() const { return GetUsedNum() >= GetCapacity(); }
	
	// ��ȡһ��Ԫ��
	BOOL32 Get( u32 dwIdx,  T& ) const ;
	// ���һ��Ԫ��. ��֧�ֶ������.
	BOOL Add( const T& tMt );
	// ����Ԫ��ֵɾ��һ��Ԫ��
	BOOL32 Delete( const T& TMt );
	// ɾ��ָ��λ�õ�Ԫ��
	BOOL32 Clear( const u32 dwIdx );
	// ����ֵΪt��Ԫ�ص�λ��, �����Ҳ���,����dwBufNum
	u32	Find( const T& t ) const ;
	
protected:
	// �ж�ָ��λ��Ԫ���Ƿ�Ϊ��
	BOOL32	IsNull( const u32 dwIdx ) const;
	// ����ָ��λ��Ԫ��Ϊ��
	void	SetNull( const u32 dwIdx, const BOOL32 bNull );
	// �ж������Ƿ�Ϊ�Ϸ�ֵ
	BOOL32	IsValidIdx( const u32 dwIdx ) const { return dwIdx < dwBufNum; }
	
private:
	T			m_Buf[dwBufNum];						// ������
	u8			m_abyFlag[(dwBufNum + 7) / 8 ];			// �Ƿ�Ϊ�յı�־λ
	u32			m_dwNum;								// ��ʹ�õ�����
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


template <class T, u32 dwBufNum >
BOOL32 CStaticBuf<T, dwBufNum>::Get( u32 dwIdx,  T& t ) const 
{
	// �����Ƿ���ָ��λ��Ϊ��, ����false
	if ( !IsValidIdx(dwIdx) || IsNull(dwIdx) )
	{
		return FALSE;
	}

	t = m_Buf[dwIdx];
	return TRUE;
}

template <class T, u32 dwBufNum >
BOOL CStaticBuf<T, dwBufNum>::Add( const T& t )
{
	// ��֧�ֶ������, ��������Ѿ���ӽ����, �򸲸�
	u32 dwIdx = Find(t);
	if (IsValidIdx(dwIdx))
	{
		// �Ѿ�����, �򸲸�
		m_Buf[dwIdx] = t;
		SetNull(dwIdx, FALSE);
		return TRUE;
	}

	// δ���ҵ�,������һ������λ��
	for(u32 dwLoop = 0; dwLoop < dwBufNum; dwLoop++)
	{
		// �����һ�����е�λ��
		if (IsNull(dwLoop))
		{
			m_Buf[dwLoop] = t;
			SetNull(dwLoop, FALSE);
			
			if( m_dwNum < dwBufNum ) m_dwNum++;

			return TRUE;
		}
	}

	// ����������
	return FALSE;
}


template <class T, u32 dwBufNum >
BOOL CStaticBuf<T, dwBufNum>::Delete( const T& t )
{
	// �Ȳ���
	u32 dwIdx = Find(t);

	return Clear(dwIdx);
}

template <class T, u32 dwBufNum >
BOOL CStaticBuf<T, dwBufNum>::Clear( const u32 dwIdx )
{
	// �ж��Ƿ�Ƿ�, �Ƿ�Ϊ��
	if (IsValidIdx(dwIdx) && !IsNull(dwIdx))
	{
		memset(&m_Buf[dwIdx], 0, sizeof(T) );
		SetNull(dwIdx, TRUE);

		if( m_dwNum ) m_dwNum-- ;
		
		return TRUE;
	}

	return FALSE;
}

template <class T, u32 dwBufNum >
u32 CStaticBuf<T, dwBufNum>::Find( const T& t) const 
{
	for(u32 dwLoop = 0; dwLoop < dwBufNum; dwLoop++)
	{
		// �ǿ���ֵ���
		if (!IsNull(dwLoop) && m_Buf[dwLoop] == t)
		{
			return dwLoop;
		}
	}

	return dwBufNum;
}

template <class T, u32 dwBufNum >
BOOL CStaticBuf<T, dwBufNum>::IsNull( const u32 dwIdx ) const 
{
	// ����ֵ�Ϸ�
	if (!IsValidIdx(dwIdx))
	{
		return TRUE;
	}

	// ������m_abyFlag��bitλ��
	const u32 dwBytePos = dwIdx / 8;		// ����m_abyFlag��byte��
	const u8  byBitPos = (u8)(dwIdx % 8);			// ����m_abyFlag�е�dwBytePos��byte��bit
	
	return !(m_abyFlag[dwBytePos] & (1 << byBitPos));
}

template <class T, u32 dwBufNum >
void CStaticBuf<T, dwBufNum>::SetNull( const u32 dwIdx, const BOOL32 bNull )
{
	if (!IsValidIdx(dwIdx))
	{
		return;
	}

	// ������m_abyFlag��bitλ��
	const u32 dwBytePos = dwIdx / 8;		// ����m_abyFlag��byte��
	const u8  byBitPos = (u8)(dwIdx % 8);		// ����m_abyFlag�е�dwBytePos��byte��bit
	
	if (bNull)
	{
		m_abyFlag[dwBytePos] &= ~(1 << byBitPos);
	}else
	{
		m_abyFlag[dwBytePos] |= (1 << byBitPos);
	}
}

//static func
#include "evmcumcs.h"
/*====================================================================
    ������      ��HandleMtListNtf
    ����        ������MCU_MCS_MTLIST_NOTIF
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CServMsg &cSerMsg, �������Ϣ 
				  TMtExtU *atMtExtU, �����ն��б�
				  u8 &byMtNum,���ն�����
				  TMcu &tMcu��������MCU
    ����ֵ˵��  ��true: �ɹ�����false: ʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2013/6/27   4.7.2       �����          ����
====================================================================*/
static bool HandleMtListNtf(const CServMsg &cSerMsg, TMtExtU *atMtExtU, u8 &byMtNum, TMcu &tMcu)
{
	if ( cSerMsg.GetMsgBodyLen() < sizeof(TMcu) )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[HandleMtListNtf]msg len <  sizeof TMcu\n");
		return false;
	}

	tMcu = *(TMcu *)cSerMsg.GetMsgBody();
	// by zoujunlong ԭ��mcu���eqpid������д�����Կ�������������mcu����mcuֻ�б��ص��ն˲Ż��ϱ���չ������
	byMtNum = tMcu.GetEqpId();
	u8* achAlias = NULL;

	if ( byMtNum == 0/*cSerMsg.GetMsgBodyLen() > sizeof(TMcu)*/ )
	{
		byMtNum = (cSerMsg.GetMsgBodyLen() - sizeof(TMcu) )/ sizeof(TMtExt);
	}
	else
	{
		achAlias = cSerMsg.GetMsgBody() + sizeof(TMcu) + sizeof(TMtExt) * byMtNum;
	}

	if ( byMtNum == 0 )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[HandleMtListNtf] byMtNum == 0\n");
		return false;
	}

	u8 byMtId = 0;

	TMtExt *ptMtExt = (TMtExt *)(cSerMsg.GetMsgBody() + sizeof(TMcu));
	while(byMtId < byMtNum)
	{
		if ( ptMtExt )
		{
			// ǰXX���ֽ�����ȫ��ͬ�ģ�ֱ�ӿ��� [pengguofeng 6/27/2013]
			memcpy(&atMtExtU[byMtId], ptMtExt, sizeof(TMt) + sizeof(u8)*2+sizeof(u16) + sizeof(u32)*2);
			atMtExtU[byMtId].SetProtocolType(ptMtExt->GetProtocolType());
			if ( achAlias != NULL )
			{
				atMtExtU[byMtId].SetAlias( (s8*)(&achAlias[(VALIDLEN_ALIAS + MAXLEN_CONFNAME) * byMtId]) );
			}
			else
			{
				atMtExtU[byMtId].SetAlias(ptMtExt->GetAlias());
			}
		}
		byMtId++;
		ptMtExt++;
	}

	return true;
}

// ���ڵ�һ���ֽ��Ǳ�ʾ��Ա��������һ��ֻ����һ��SMCU���б� [pengguofeng 6/27/2013]
// u8 byMtNum + u16 wMcuIdx + u8 byHasHeadInfo + [ u32 dwHeadLen + u8 byMemNum + byMemNum * u16 MemLen +] byMtNum* TMtExt_U
/*====================================================================
    ������      ��HandleMtListNtfUtf8
    ����        ������MCU_MCS_GETMTLIST_NOTIF��Ϣ�������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CServMsg &cSerMsg, �������Ϣ 
				��TMtExtU *atMtExtU,����Ա�б���Ϣ
				��u8 &byMtNum�����ն�����
				��TMcu &tMcu��������MCU
    ����ֵ˵��  ��true: �����ɹ�  false: ����ʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2013/6/27   4.7.2       �����          ����
====================================================================*/
static bool HandleMtListNtfUtf8(const CServMsg &cSerMsg, TMtExtU *atMtExtU, u8 &byMtNum, TMcu &tMcu)
{
	if ( cSerMsg.GetMsgBodyLen() < sizeof(u8)*2 + sizeof(u16) )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[HandleMtListNtfUtf8]msg len < 4\n");
		return false;
	}

	u8 *pMsg = cSerMsg.GetMsgBody();
	byMtNum = *(u8 *)pMsg;
	pMsg += sizeof(u8);
/*
	if ( byMtNum == 0 )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[HandleMtListNtfUtf8]byMtNum==0\n");
		// Bug00147556:�¼��ն���Ϊ0,ҲҪ��tMcu���� [pengguofeng 7/10/2013]
		return false;
	}
*/
	u16 wMcuIdx = *(u16 *)pMsg;
	pMsg += sizeof(u16);
	wMcuIdx = ntohs(wMcuIdx);

	tMcu.SetNull();
	tMcu.SetMcuIdx(wMcuIdx);

	if ( byMtNum == 0 )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[HandleMtListNtfUtf8]byMtNum==0\n");
		// Bug00147556:�¼��ն���Ϊ0,ҲҪ��tMcu���� [pengguofeng 7/10/2013]
		return false;
	}

	u8 byHasHeadInfo = *(u8 *)pMsg;
	pMsg += sizeof(u8);
	if ( byHasHeadInfo == 0 )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CONF,"[HandleMtListNtfUtf8] no head info\n");
		return false;
	}

	if ( cSerMsg.GetMsgBodyLen() < sizeof(u8)*2 + sizeof(u16) + sizeof(u32) )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CONF,"[HandleMtListNtfUtf8] MsgLen get invalid HeadLen\n");
		return false;
	}

	u32 dwHeadLen = *(u32 *)pMsg;
	pMsg += sizeof(u32);

	dwHeadLen = ntohl(dwHeadLen);

	if ( dwHeadLen < sizeof(u8) )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CONF,"[HandleMtListNtfUtf8] head len is wrong:%d\n", dwHeadLen);
		return false;
	}

	if ( cSerMsg.GetMsgBodyLen() < sizeof(u8)*3 + sizeof(u16) + sizeof(u32) )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CONF,"[HandleMtListNtfUtf8] MsgLen get invalid MemNum \n");
		return false;
	}

	u8 byMemNum = *(u8 *)pMsg; //��Ϣ�ṹ���ڵĽṹ��Ա��Ŀ
	pMsg += sizeof(u8);

	if ( dwHeadLen < sizeof(u8) + byMemNum * sizeof(u16) )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CONF,"[HandleMtListNtfUtf8] head len is wrong:%d byMemNum:%d\n", dwHeadLen, byMemNum);
		return false;
	}

	if ( cSerMsg.GetMsgBodyLen() < sizeof(u8)*3 + sizeof(u16) + sizeof(u32) + byMemNum * sizeof(u16) )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CONF,"[HandleMtListNtfUtf8] MsgLen get invalid MemLen\n");
		return false;
	}

	u16 *pawLen = (u16 *)OspAllocMem(byMemNum * sizeof(u16)); //new u16[byMemNum]; //��ǰ��Ϣ�����Ա�ĳ���
	memset(pawLen, 0, sizeof(u16)*byMemNum);

	u8 byIdx = 0;
	u16 wLen = 0;
	u16 wMsgStructLen = 0; //��Ϣ��Ľṹ���ܳ���,Ҫ��sizeof(TMtExtU)���Ƚϣ�ȡС
	while (byIdx < byMemNum)
	{
		wLen = *(u16 *)pMsg;
		pMsg += sizeof(u16);
		wLen = ntohs(wLen);
		wMsgStructLen += wLen;
		memcpy(&pawLen[byIdx], &wLen, sizeof(u16));
		byIdx++;
	}

	if ( cSerMsg.GetMsgBodyLen() < sizeof(u8)*3 + sizeof(u16) + sizeof(u32) + byMemNum * sizeof(u16) + byMtNum * wMsgStructLen )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CONF,"[HandleMtListNtfUtf8] MsgLen get invalid MemStruct\n");
		return false;
	}

	u8 *pStruct = (u8*)atMtExtU;

	LogPrint(LOG_LVL_WARNING,MID_MCU_CONF,"[HandleMtListNtfUtf8] wMsgStructLen :%d sizeof(TMtExtU):%d\n",
		wMsgStructLen, sizeof(TMtExtU));

	byIdx = 0; //byidx��ʱ��ʾ�ն���Ŀ
	u8 byStructNum = atMtExtU[0].GetMemNum();
	u16 wCopyLen = 0;
	while (byIdx < byMtNum )
	{
		for (u8 byMemId = 1; byMemId <= byStructNum; byMemId++)
		{
			if ( byMemId > byMemNum )
			{
				//wMsgStructLen < sizeof(TMtExtU): ʣ�µĲ��ÿ����ˣ�pStructƫ��
				for ( u8 byLeftLoop = byMemId; byLeftLoop <= byStructNum; byLeftLoop++ )
				{
					pStruct += atMtExtU[byIdx].GetMemLen(byLeftLoop);
				}
				break; //������һ��atMtExtU[x]
			}
			else if ( byMemId == byStructNum && byMemId < byMemNum )
			{
				//wMsgStructLen > sizeof(TMtExtU) �����ˣ�������Ϣ���滹�ж���ĳ�Ա
				for ( u8 byLeftLoop2 = byMemId; byLeftLoop2 <= byMemNum; byLeftLoop2++ )
				{
					pMsg += pawLen[byLeftLoop2-1];
				}
				break; //������һ��pMsg
			}
			else
			{
				//��Ⱦ�������
			}

			wCopyLen = atMtExtU[byIdx].GetMemLen(byMemId);
			if ( wCopyLen > pawLen[byMemId-1] )
			{
				wCopyLen = (u16)pawLen[byMemId-1];
			}

			memcpy(pStruct, pMsg, wCopyLen);

			pStruct += atMtExtU[byIdx].GetMemLen(byMemId);
			pMsg += (u16)pawLen[byMemId-1];
		}

		byIdx++;
	}

	//�ͷ�
	//delete pawLen;
	OspFreeMem(pawLen);

	return true;
}

/*====================================================================
    ������      ��UnPackMsg
    ����        ��������Ϣ��Ŀǰ��Ҫ����2��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const CServMsg &cSerMsg, �������Ϣ 
				��TMtExtU *atMtExtU,���ն��б�
				��u8 &byMtNum, �ն�����
				��TMcu &tMcu��������mcu
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2013/6/27   4.7.2       �����          ����
====================================================================*/
static bool UnPackMsg(const CServMsg &cSerMsg, TMtExtU *atMtExtU, u8 &byMtNum, TMcu &tMcu)
{
	switch ( cSerMsg.GetEventId() )
	{
	case MCU_MCS_MTLIST_NOTIF: 
		return HandleMtListNtf(cSerMsg, atMtExtU, byMtNum, tMcu);
	case MCU_MCS_GETMTLIST_NOTIF: 
		return HandleMtListNtfUtf8(cSerMsg, atMtExtU, byMtNum, tMcu);
	default:
		StaticLog("[UnPackMsg]unknow msg.%d(%s) recv\n", cSerMsg.GetEventId(), OspEventDesc(cSerMsg.GetEventId()));
		return false;
	}
}

/*==============================================================================
������    :  CorrectUtf8Str
����      :  ����UTF8�ַ���,�������11xx xxxx ��0,�����Ĳ���
ע��	  :  ֻ��UTF8����Ч
�㷨ʵ��  :  
����˵��  :  s8 *�����޸ĵ��ַ���
			 u16���ַ������ȣ��ᱻ�޸ģ�
����ֵ˵��:  True: ��Ҫ����������������
			 False: ��������
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013/05/23                 �����                            ����
==============================================================================*/
static BOOL32 CorrectUtf8Str(s8 * pStr, const u16 wStrLen)
{
	if ( !pStr )
	{
		return FALSE;
	}

#ifdef _UTF8
	u8 byFollowLen = 0;   // utf8�������������ֽڣ�������ͷ��11xx xxxx

	for ( u16 wLoop = wStrLen; wLoop > 0; wLoop--)
	{
		if ( pStr[wLoop-1] != '\0')
		{
			if (  (pStr[wLoop-1] & 0xfe ) == 0xfc )  // 1111 110x �����5��10xx xxxx����ͬ
			{
				byFollowLen = 5;
			}
			else if ( (pStr[wLoop-1] & 0xfc ) == 0xf8 )  // 1111 10xx: 4
			{
				byFollowLen = 4;
			}
			else if ( (pStr[wLoop-1] & 0xf8 ) == 0xf0 )  // 1111 0xxx: 3
			{
				byFollowLen = 3;
			}
			else if ( (pStr[wLoop-1] & 0xf0 ) == 0xe0 )  // 1110 xxxx: 2
			{
				byFollowLen = 2;
			}
			else if ( (pStr[wLoop-1] & 0xe0 ) == 0xc0 )  // 110x xxxx: 1
			{
				byFollowLen = 1;
			}
			else
			{
				byFollowLen = 0;
			}

			if ( wLoop + byFollowLen > wStrLen ) //������󳤶ȣ��϶��нض�
			{
				pStr[wLoop-1] = '\0';
				return TRUE;
			}
			
			for ( u8 byLoop = 0; byLoop < byFollowLen; byLoop++)
			{
				if ( pStr[wLoop+byLoop] == '\0' || (pStr[wLoop+byLoop] & 0xc0) != 0x80 ) // ���� 0 ���߲��� 10xx xxxx�ĸ�ʽ����ʾ�нض�
				{
					pStr[wLoop-1] = '\0';
					return TRUE;
				}
			}
		}
	}
#endif

	return FALSE;
}

/*==============================================================================
������    :  CorrectGBKStr
����      :  ����GBK�ַ���,���������0,�����Ĳ���
ע��	  :  ֻ��GBK��������Ч
�㷨ʵ��  :  
����˵��  :  s8 *�����޸ĵ��ַ���
			 u16���ַ������ȣ��ᱻ�޸ģ�
����ֵ˵��:  True: ��Ҫ����������������
			 False: ��������
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013/07/16                 �����                            ����
==============================================================================*/
static void CorrectGBKStr(s8 *pStr, const u16 wStrLen)
{
	s8 nLoop = wStrLen - 1;
	u8 byWideCharCount = 0;
	while ( (signed char)nLoop >= 0  && (signed char)0 > (signed char)pStr[nLoop])
	{
		byWideCharCount ++;
		nLoop --;
	}
	if ( byWideCharCount % 2 == 1 )
	{
		pStr[wStrLen-1] = '\0';
    }
}

#undef SETBITSTATUS
#undef GETBITSTATUS

#ifdef WIN32
#pragma pack( pop )
#endif

#endif /* __MCUSTRUCT_H_ */




