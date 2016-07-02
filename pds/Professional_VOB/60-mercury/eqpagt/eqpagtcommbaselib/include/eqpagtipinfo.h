/*****************************************************************************
    ģ����      : EqpAgt
    �ļ���      : eqpagtipinfo.h
    ����ļ�    : 
    �ļ�ʵ�ֹ���: EqpAgt IP��Ϣ����Ҫ�ο�commonlib��
    ����        : liaokang
    �汾        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    �޸ļ�¼:
    ��  ��      �汾        �޸���          �޸�����
    2012/06/18  4.7         liaokang        ����
******************************************************************************/
#ifndef _EQPAGT_IPINFO_H_
#define _EQPAGT_IPINFO_H_

#include "eqpagtutility.h"

#ifdef WIN32
#include <wtypes.h>
#include <objbase.h>
#include <setupapi.h>
#pragma comment(lib,"setupapi.lib")
#endif

// IP�����ļ�
#define  IPCFGFILENAME             ( LPCSTR )"ipconfig.ini"

#ifdef _LINUX_
#define  ROUTE_CFGFILE              "/proc/net/route"
#define  NETWORK_CFGFILE            "/etc/sysconfig/network"
#define  NETIF_CFGFILE_DIR          "/etc/sysconfig/network-scripts/"  
#define  RC_LOCAL                   "/etc/rc.d/rc.local"

#define  GW_KEY                     (LPCSTR)"GATEWAY"
#define  IP_KEY                     (LPCSTR)"IPADDR"
#define  HWADDR_KEY                 (LPCSTR)"HWADDR"
#define  DEVICE_KEY                 (LPCSTR)"DEVICE"
#define  NETMAS_KEY                 (LPCSTR)"NETMASK"
#define  TYPE_KEY                   (LPCSTR)"Ethernet"
#define  ONBOOT_KEY                 (LPCSTR)"ONBOOT"
#define  BOOTPR_KEY                 (LPCSTR)"BOOTPROTO"
#endif // _LINUX_



// ������������Ϣ��ȡ
#define EQPAGT_MAX_ADAPTER_DESCRIPTION_LEN      (u32)128 
#define EQPAGT_MAX_ADAPTER_NAME_LEN             (u32)256 
#define EQPAGT_MAX_ADAPTER_ADDRESS_LEN          (u32)8 

#define EQPAGT_MAX_ADAPTER_IP_NUM               (u32)16
#define EQPAGT_MAX_ADAPTER_GW_NUM               (u32)16
#define EQPAGT_MAX_ADAPTER_ROUTE_NUM            (u32)16
#define EQPAGT_MAX_ADAPTER_NUM                  (u32)16
#define EQPAGT_MAX_MULTINET_NUM					(u32)(EQPAGT_MAX_ADAPTER_IP_NUM-2)

#define EQPAGT_MIB_IF_TYPE_OTHER                1
#define EQPAGT_MIB_IF_TYPE_ETHERNET             6
#define EQPAGT_MIB_IF_TYPE_TOKENRING            9
#define EQPAGT_MIB_IF_TYPE_FDDI                 15
#define EQPAGT_MIB_IF_TYPE_PPP                  23
#define EQPAGT_MIB_IF_TYPE_LOOPBACK             24
#define EQPAGT_MIB_IF_TYPE_SLIP                 28

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

struct TNetAdaptInfo
{
protected:
    s8  m_achAdapterName[EQPAGT_MAX_ADAPTER_NAME_LEN + 4];
    s8  m_achDescription[EQPAGT_MAX_ADAPTER_DESCRIPTION_LEN + 4];
    u32 m_dwMacAddressLength;
    u8  m_abyMacAddress[EQPAGT_MAX_ADAPTER_ADDRESS_LEN];
    u32 m_dwIndex;
    u32 m_dwType;
	u32 m_dwDefGWIpAddr;	// [3/25/2010 xliang] ������Ĭ������ip��ַ
    TNetParam m_tCurrentIpAddress;

public:
    LinkArray<TNetParam, EQPAGT_MAX_ADAPTER_IP_NUM> m_tLinkIpAddr;          // ������IP��ַ
    LinkArray<TNetParam, EQPAGT_MAX_ADAPTER_GW_NUM> m_tLinkDefaultGateway;  // ������Ĭ������

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
        case EQPAGT_MIB_IF_TYPE_OTHER:
            StaticLog( "Other\n");
            break;
        case EQPAGT_MIB_IF_TYPE_ETHERNET:
            StaticLog( "Ethernet\n");
            break;
        case EQPAGT_MIB_IF_TYPE_TOKENRING:
            StaticLog( "Tokenring\n");
            break;
        case EQPAGT_MIB_IF_TYPE_FDDI:
            StaticLog( "FDDI\n");
            break;            
        case EQPAGT_MIB_IF_TYPE_PPP:
            StaticLog( "PPP\n");
            break;
        case EQPAGT_MIB_IF_TYPE_LOOPBACK:
            StaticLog( "LoopBack\n");
            break;
        case EQPAGT_MIB_IF_TYPE_SLIP:
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
            for ( dwLop = 0; dwLop < dwLength && dwLop < EQPAGT_MAX_ADAPTER_ADDRESS_LEN; dwLop++ )
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
    LinkArray<TNetAdaptInfo, EQPAGT_MAX_ADAPTER_NUM> m_tLinkNetAdapt;
    
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

struct TEthParam{
    u32 dwIpAdrs;         /*�����ֽ���*/
    u32 dwIpMask;         /*�����ֽ���*/
    u8  byMacAdrs[6];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
/* ��̫�������ṹ */
struct TAllEthParam{
    u32 dwIpNum;/*��Ч��IP��ַ��*/
    TEthParam atEthParam[EQPAGT_MAX_ADAPTER_NUM];/*���IP��ַ����Ϣ������*/
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// ����IP��ַ�������Ƿ���Ч
BOOL32 IsValidIpV4( LPCSTR lpszIPStr );
BOOL32 IsValidIpV4( u32 dwIP);
BOOL32 IsValidIpMask( LPCSTR lpszIpMask );
BOOL32 IsValidIpMask( u32 dwIpMask );

BOOL32 GetNetAdapterInfoActive( TNetAdaptInfoAll * ptNetAdaptInfoAll );
BOOL32 GetNetAdapterInfoAll( TNetAdaptInfoAll * ptNetAdaptInfoAll );
BOOL32 GetNetAdapterInfo( TNetAdaptInfoAll * ptNetAdaptInfoAll, BOOL32 bActive = TRUE );
BOOL32 GetAdapterIp( u32 dwIfIdx, TAllEthParam* ptAllEthParm);

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<��win32�� begin>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef WIN32
// windows������/��ȡIp, maskIp, Gateway��ַ
// set������WIN7/VISTA/SERVER 2008����������Թ���Ա������г��������Ч��
/*=============================================================================
�� �� ���� RegGetIpAdr
��    �ܣ� ����IP�ַ�������'\0'�ָ������������'\0'��ʾ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� s8* lpszAdapterName
s8* pIpAddr
u32 &dwLen [IN/OUT]//IP���鳤��
�� �� ֵ�� u16 �ɹ�����ERROR_SUCCESS���������ERROR_NOT_ENOUGH_MEMORY����wlen����ʵ����Ҫ��u32����
=============================================================================*/
u32 RegGetIpAdr( LPSTR lpszAdapterName, LPSTR lpszIpAddr, u32 &dwLen );

/*=============================================================================
�� �� ���� RegGetIpAdr
��    �ܣ� �����u32��ʽ��ʾ��IP��ַ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� s8* lpszAdapterName
           u32* pdwIpAddr       �������ַ
           u32 &dwLen           ������ռ䳤��
�� �� ֵ�� u32 ���ɹ�����ERROR_SUCCESS���������ERROR_NOT_ENOUGH_MEMORY����wlen����ʵ����Ҫ��u32����
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2009/6/24   4.0			�ܹ��                  ����
=============================================================================*/
u32 RegGetIpAdr( LPSTR lpszAdapterName, u32* pdwIpAddr, u32 &dwLen );

/*=============================================================================
�� �� ���� RegGetNetMask
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� s8* lpszAdapterName
           s8* pNetMask
           u32 &dwLen  [IN/OUT] //NetMask���鳤��
�� �� ֵ�� u32 �ɹ�����ERROR_SUCCESS���������ERROR_NOT_ENOUGH_MEMORY����wlen����ʵ����Ҫ��u32���� 
=============================================================================*/
u32 RegGetNetMask( LPSTR lpszAdapterName, LPSTR lpszNetMask, u32 &dwLen );

/*=============================================================================
�� �� ���� RegGetNetMask
��    �ܣ� �����u32��ʽ��ʾ������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� s8* lpszAdapterName
           u32* pdwNetMask
           u32 &dwLen
�� �� ֵ�� u16 �ɹ�����ERROR_SUCCESS���������ERROR_NOT_ENOUGH_MEMORY����dwlen����ʵ����Ҫ��u32���� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2009/6/24   4.0			�ܹ��                  ����
=============================================================================*/
u32 RegGetNetMask( LPSTR lpszAdapterName, u32* pdwNetMask, u32 &dwLen );

/*=============================================================================
�� �� ���� RegGetNetGate
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� s8* lpszAdapterName
           s8* pNetGate
           u32 &dwLen
�� �� ֵ�� u32 
=============================================================================*/
u32 RegGetNetGate( LPSTR lpszAdapterName, LPSTR lpszNetGate, u32 &dwLen );

/*=============================================================================
�� �� ���� RegGetNetGate
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� s8* lpszAdapterName
           u32* pdwNetGate
           u32 &dwLen
�� �� ֵ�� u32 
=============================================================================*/
u32 RegGetNetGate( LPSTR lpszAdapterName, u32* pdwNetGate, u32 &dwLen );

/*=============================================================================
�� �� ���� RegSetIpAdr
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� s8* pIpAddr
u32 dwLen //IP���鳤��
�� �� ֵ�� u32 
=============================================================================*/
u32 RegSetIpAdr(s8* lpszAdapterName, s8* pIpAddr, u32 dwLen);

/*=============================================================================
�� �� ���� RegSetIpAdr
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� s8* lpszAdapterName
u32 * pdwIpAddr
u32 dwLen
�� �� ֵ�� u32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2009/6/24   4.0			�ܹ��                  ����
=============================================================================*/
u32 RegSetIpAdr(s8* lpszAdapterName, u32 * pdwIpAddr, u32 dwLen);

/*=============================================================================
�� �� ���� RegSetNetMask
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� s8* pNetMask
u32 dwLen   [IN] //NetMask���鳤��
�� �� ֵ�� u32 
=============================================================================*/
u32 RegSetNetMask(s8* lpszAdapterName, s8* pNetMask, u32 dwLen);

/*=============================================================================
�� �� ���� RegSetNetMask
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� s8* lpszAdapterName
u32* pdwNetMask
u32 dwLen
�� �� ֵ�� u32 
=============================================================================*/
u32 RegSetNetMask(s8* lpszAdapterName, u32* pdwNetMask, u32 dwLen);

/*=============================================================================
�� �� ���� RegSetNetGate
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const s8* lpszAdapterName
s8* pNetGate
u32 dwLen
�� �� ֵ�� u32 
=============================================================================*/
u32 RegSetNetGate(const s8* lpszAdapterName, s8* pNetGate, u32 dwLen);

/*=============================================================================
�� �� ���� RegSetNetGate
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const s8* lpszAdapterName
u32* pdwNetGate
u32 dwLen
�� �� ֵ�� u32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2009/6/24   4.0			�ܹ��                  ����
=============================================================================*/
u32 RegSetNetGate(const s8* lpszAdapterName, u32* pdwNetGate, u32 dwLen);

//������������
/*=============================================================================
�� �� ���� GetRegistryProperty
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� HDEVINFO DeviceInfoSet
           PSP_DEVINFO_DATA DeviceInfoData
           ULONG Property
           PVOID Buffer
           PULONG Length
�� �� ֵ�� u32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2009/6/23   4.0			�ܹ��                  ����
=============================================================================*/
u32  GetRegistryProperty( HDEVINFO hDevInfo, PSP_DEVINFO_DATA DeviceInfoData, ULONG Property, PVOID Buffer, PULONG Length );

#endif //WIN32
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<��win32��   end>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


//<<<<<<<<<<<<<<<<<<<<<<<<��LINUXƽ̨��ʹ�� begin>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef _LINUX_

#endif	// LINUX
//<<<<<<<<<<<<<<<<<<<<<<<<��LINUXƽ̨��ʹ��   end>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


#endif  // _EQPAGT_IPINFO_H_