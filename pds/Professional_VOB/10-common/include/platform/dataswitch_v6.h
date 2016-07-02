/*****************************************************************************
   ģ����      : DataSwitch
   �ļ���      : dataswitch.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: DataSwitchģ��ӿں�������
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2005/03/18  4.0         ��־ҵ        ����
   2005/08/10  4.0         ��־ҵ        ʵ��3.6�������Ĺ���
******************************************************************************/

#ifndef DATASWITCH_H
#define DATASWITCH_H


#include "osp.h"

#ifdef _MSC_VER
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#ifdef _LINUX_
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>
#endif


#define DSVERSION       "Dataswitch_socket(IPv6) 40.20.01.03.120525"

/* DataSwitch ��� */
#define DSID    u32
	
/*DataSwitch ����ֵ*/
#define DSOK    1
#define DSERROR 0

#define INVALID_DSID  0xffffffff   /*��Ч�ľ��ֵ*/


// DataSwitch�����ַ���Ͷ���
#define DS_SUPPORT_NONE 0x00
#define DS_SUPPORT_IPV4 0x01
#define DS_SUPPORT_IPV6 0x02

#define DS_CFLAG_IPV6

#if defined(DS_CFLAG_IPV6)
  #undef  DS_SUPPORT_TYPE
  #define DS_SUPPORT_TYPE   DS_SUPPORT_IPV6
#else
  #undef  DS_SUPPORT_TYPE
  #define DS_SUPPORT_TYPE   DS_SUPPORT_IPV4
#endif


#ifdef _LINUX_
  #define IN_ADDR    in_addr
  #define IN6_ADDR   in6_addr
#endif

#ifdef _MSC_VER
  #ifndef snprintf
    #define snprintf   _snprintf   
  #endif
  #ifndef vsnprintf
    #define vsnprintf  _vsnprintf
  #endif
#endif

#ifndef _MSC_VER
	#define SOCKHANDLE			 int

	#ifndef SOCKADDR
		#define SOCKADDR		 sockaddr
	#endif

	#define SOCKADDR_IN 		 sockaddr_in
	#define SOCKADDR_IN6 		 sockaddr_in6
#endif

#if (DS_SUPPORT_TYPE & DS_SUPPORT_IPV6)
  #define DS_SOCKET_SOCKADDR_SIZE sizeof(struct sockaddr_in6)
#else
  #define DS_SOCKET_SOCKADDR_SIZE sizeof(struct sockaddr_in)
#endif




#define DS_TYPE_NONE           (u8)0         //δ�����ַ����
#define DS_TYPE_IPV4           (u8)1         //IPv4��ַ����
#define DS_TYPE_IPV6           (u8)2         //IPv6��ַ����

#define DS_IPV6_ADDRSIZE   16
#define DS_MAX_IPSTR_SIZE  64

/**
* ��ַ���ͽṹ, ��֧��IPv4��IPv6
*/
typedef struct tagDSNetAddr
{
	u8  byIPType;
	u16 wPort;
	u32 dwScopeID;
	union
	{
		u32 dwV4IP;                      // IPv4��ַ, ������
		u8  abyV6IP[DS_IPV6_ADDRSIZE];   // IPv6��ַ, ������		
	};

	tagDSNetAddr()
	{
		Clear();
	}

	void Clear()
	{
		memset( this, 0, sizeof(tagDSNetAddr) );
	}

	void SetV4IPAddress( u32 dwV4Addr )
	{
		byIPType = DS_TYPE_IPV4;
		dwV4IP = dwV4Addr;
	}
	
	void SetV6IPAddress( u8 *pbyV6Addr )
	{
		byIPType = DS_TYPE_IPV6;
		memcpy( abyV6IP, pbyV6Addr, DS_IPV6_ADDRSIZE );
	}

	u32 GetV4IPAddress()
	{
		return dwV4IP;
	}
	
	u8 *GetV6IPAddress()
	{
		return abyV6IP;
	}

	void SetType(u8 byType)
	{
		byIPType = byType;
	}
	
	u8 GetType( )
	{
		return byIPType;
	}

	void SetPort( u16 port )
	{
		wPort = port;
	}

	u16 GetPort(  )
	{
		return wPort;
	}

	void SetScopeID( u32 scopeid )
	{
		dwScopeID = scopeid;
	}

	u32 GetScopeID(  )
	{
		return dwScopeID;
	}


	BOOL32 SetIPStr( u8 byType, s8* pchIPStr )
	{
		BOOL32 bRet = FALSE; 
		
		if ( byType == DS_TYPE_IPV4 )
		{
			bRet = SetV4IPStr( pchIPStr );
		} 
		else if  ( byType == DS_TYPE_IPV6 )
		{
			bRet = SetV6IPStr( pchIPStr );
		}
		
		return bRet;
	}
	
	const s8 *GetIPStr( s8* pchIPStr, u8 bySize )
	{
		if ( byIPType == DS_TYPE_IPV4 )
		{
			return GetV4IPStr( pchIPStr, bySize );
		} 
		if ( byIPType == DS_TYPE_IPV6 )
		{
			return GetV6IPStr( pchIPStr, bySize );
		}
		else
		{
			return NULL;
		}
	}
	
	BOOL32 SetV4IPStr( s8* pchIPStr )
	{
		BOOL32 bRet = inet_pton( AF_INET, pchIPStr, &dwV4IP );
		if ( TRUE == bRet ) 
		{
			byIPType = DS_TYPE_IPV4;
			return TRUE;
		} 
		else
		{
			return FALSE;
		}
	}
	
	BOOL32 SetV6IPStr( s8* pchIPStr )
	{
		BOOL32 bRet = inet_pton( AF_INET6, pchIPStr, abyV6IP );
		if ( TRUE == bRet ) 
		{
			byIPType = DS_TYPE_IPV6;
			return TRUE;
		} 
		else
		{
			return FALSE;
		}
	}
	
	const s8 *GetV4IPStr( s8* pchIPStr, u8 bySize )
	{
		return inet_ntop( AF_INET, &(dwV4IP), pchIPStr, bySize );
	}
	
	const s8 *GetV6IPStr( s8* pchIPStr, u8 bySize )
	{
		return inet_ntop( AF_INET6, abyV6IP, pchIPStr, bySize );
	}


}TDSNetAddr;
 
 /*
 * @func FilterFunc
 * @brief ���չ��˺���
 *
 * ÿ�����ս����һ�����˺��������˽����յ�UDP���ݰ�ʱִ�д˺�����
 * �����ݺ����ķ���ֵ������̬�ؾ����Ƿ�Դ����ݰ�ת����
 *
 * @param ptRecvAddr     - ���յ�ַ
 * @param ptSrcAddr      - Դ��ַ
 * @param pData          - [in, out]���ݰ��������޸ģ�
 * @param uLen           - [in, out]���ݰ����ȣ��޸ĺ�ĳ��Ȳ��ɳ������ֵ
 * @return 0, �����ݰ���Ч�� ����ֵ�������ݰ���Ч��
 */	
typedef u32 (*FilterFunc)(TDSNetAddr* ptRecvAddr, TDSNetAddr* ptSrcAddr, u8* pData, u32 uLen);


#define SENDMEM_MAX_MODLEN        (u8)32        // ����ʱ���������޸ĳ���
    
/**
 * ���ݰ�ת��Ŀ�꣺ָ������ĵ�ַ��
 * DS�����ƵĶ��IP���������ݰ���ת��Ӧ�ò�����Ӱ�졣Ҳ����˵�����
 * ������IP�����Խ�һ�����ݰ���ȷ�ĵ�ת����ȥ����ʹ����һ��IP��û��
 * ��ϵ�ġ����ԣ�ת������ӿ�IP�ǲ���Ҫ�ġ�
 */
typedef struct tagNetSndMember
{
	TDSNetAddr tDstAddr; /*ת��Ŀ�ĵ�ַ*/
    long       lIdx;     /* �ӿ����� */
    u32        errNum;   /* �������*/
    long       errNo;    /* ����� */
    void *     lpuser;   /* user info */
    /* Ϊʵ�ַ���ʱ�޸����ݶ����ӵĲ��� */
    u16  wOffset;        /* �޸Ĳ��ֵ���ʼλ��; Ŀǰδʹ�� */
    u16  wLen;		/* �޸Ĳ��ֵĳ��ȣ�����Ϊ4�ı�����Ϊ���ʾ���޸� */
    char            pNewData[SENDMEM_MAX_MODLEN];
    /* Ҫ�޸ĵ����� */
    void *          pAppData;       /* �û��Զ���Ļص������Ĳ��� */

}TNetSndMember;


typedef void (*SendCallback)(TDSNetAddr* ptRecvAddr,
                             TDSNetAddr* ptSrcAddr,
                             TNetSndMember *ptSends,            // ת��Ŀ���б�
                             u16* pwSendNum,                    // ת��Ŀ����� 
                             u8* pUdpData, 
							 u32 dwUdpLen);


/************************************************************
 * ��������dsCreate
 *
 * ���ܣ�  ����DataSwitchģ��
 *
 * ʵ��˵���� 
 * Dataswitch֧�֡����û�����ÿ�ε���dsCreate�ͻ᷵��һ��
 * �û���ʶ����ӡ�ɾ��ʱ��Ҫƥ���û���־��ת��ʱ�򲻿��ǡ�
 *
 * ����ֵ�� 
 * @return INVALID_DSID: ��ʾʧ�� ; ����ɹ�
 ************************************************************/
API DSID dsCreate( u32 dwIfNum,  TDSNetAddr atIP[] );

/************************************************************
 * ��������dsDestroy
 *
 * ���ܣ� ��ֹDataSwitch������
 *
 * ʵ��˵����
 * һ��ɾ��һ���û���ֱ�������û���ɾ����������Դ��ȫ�ͷš�
 *
 * ����˵����
 * @param dsId  - �û���ʶ
 * 
 * ����ֵ��
 ************************************************************/
API void dsDestroy( DSID dsId );
 
/************************************************************
 * �������� dsAdd
 *
 * ���ܣ� ����ת��Ŀ��Ψһ��ת������
 *
 * ʵ��˵����
 * ��ĿǰΨһ���ĺ����ǣ�����Ѿ����ڵĽ���������ת��Ŀ��
 * �뵱ǰ�������ͬ������ɾ����Щ����
 *
 * ����˵����
 * @param dsId          - �û���ʶ
 * @param ptRecvAddr    - ���յ�ַ�����������ݰ���Ŀ�ĵ�ַ
 * @param ptInLocalIP   - �������ݰ�������ӿ�
 * @param ptSendtoAddr  - ת��Ŀ�ĵ�ַ
 * @param ptOutLocalIP  - ת�����ݰ����ñ���IP
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/
API u32 dsAdd(DSID dsId,
			  TDSNetAddr* ptRecvAddr,
			  TDSNetAddr* ptInLocalIP,
			  TDSNetAddr* ptSendtoAddr,
              TDSNetAddr* ptOutLocalIP = NULL);

/************************************************************
 * ��������dsRemove
 *
 * ���ܣ� ɾ��ת��Ŀ��Ϊָ����ַ��ת������
 * 
 * ʵ��˵����
 *
 * ����˵����
 * @param dsId          - �û���ʶ
 * @param ptSendtoAddr  - ת��Ŀ�ĵ�ַ
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/
API u32 dsRemove(DSID dsId, TDSNetAddr* ptSendtoAddr);

/************************************************************
 * ��������dsAddDump
 *
 * ���ܣ� ����Dump����
 * DUMP���򣺴�ָ����ַ���յ����ݰ�����ת����
 * ���һ�����յ�ַֻ��DUMP��������յ����ݰ�����ת����
 * �����������ת������������������ת����
 *
 * ʵ��˵����
 * DUMP��������������Σ���Ϊ�������������ǿ��Բ���ģ�
 * ����������
 *
 * ����˵����
 * @param dsId          - �û���ʶ
 * @param ptRecvAddr    - ���յ�ַ�����������ݰ���Ŀ�ĵ�ַ
 * @param ptInLocalIP   - �������ݰ�������ӿ�
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/
API u32 dsAddDump(DSID dsId, TDSNetAddr* ptRecvAddr, TDSNetAddr* ptInLocalIP);

/************************************************************
 * ��������dsRemoveDump
 *
 * ���ܣ� ɾ��Dump����
 *
 * ʵ��˵����
 * 
 * ����˵����
 * @param dsId          - �û���ʶ
 * @param ptRecvAddr    - ���յ�ַ�����������ݰ���Ŀ�ĵ�ַ
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/
API u32 dsRemoveDump(DSID dsId, TDSNetAddr* ptRecvAddr);

/************************************************************
 * ��������dsAddManyToOne
 * 
 * ���ܣ� ���Ӷ��һ��ת������
 * ע�⣬����dsAdd�����������෴�����߻��ʹ��ʱҪС�ġ�
 * 
 * ʵ��˵����
 *
 * ����˵����
 * @param dsId           - �û���ʶ
 * @param ptRecvAddr     - ���յ�ַ�����������ݰ���Ŀ�ĵ�ַ
 * @param ptInLocalIP    - �������ݰ�������ӿ�
 * @param ptSendtoAddr   - ת��Ŀ�ĵ�ַ
 * @param ptOutLocalIP   - ת�����ݰ����ñ���IP��������ת��Ŀ�ĵ�ַͬЭ���壩
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/
API u32 dsAddManyToOne(DSID dsId ,
					   TDSNetAddr* ptRecvAddr,
					   TDSNetAddr* ptInLocalIP,
					   TDSNetAddr* ptSendtoAddr,
                       TDSNetAddr* ptOutLocalIP = NULL);

/************************************************************
 * ��������dsRemoveAllManyToOne
 * 
 * ���ܣ� ɾ������ת��Ŀ��Ϊָ����ַ�Ķ��һ����
 * 
 * ע�⣺����������dsRemove��ȫ��ͬ�����Դӽӿڲ�����֤����
 * �ṩ����ӿ���Ϊ�˲������಻ͬ����Ľӿڻ��ʹ�á�һ��
 * �����У�Ӧ��ʼ��ʹ��ĳһ��ӿڡ�
 *
 * ʵ��˵����
 * 
 * ����˵����
 * @param dsId           - �û���ʶ
 * @param ptSendtoAddr   - ת��Ŀ�ĵ�ַ
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/
API u32 dsRemoveAllManyToOne(DSID dsId, TDSNetAddr* ptSendtoAddr );

/************************************************************
 * ��������dsRemoveManyToOne
 *
 * ���ܣ� ɾ��ָ���Ķ��һ����
 *
 * ʵ��˵����
 * 
 * ����˵����
 * @param dsId           - �û���ʶ
 * @param ptRecvAddr     - ���յ�ַ�����������ݰ���Ŀ�ĵ�ַ
 * @param ptSendtoAddr   - ת��Ŀ�ĵ�ַ
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/
API u32 dsRemoveManyToOne(DSID dsId ,
                          TDSNetAddr* ptRecvAddr,
                          TDSNetAddr* ptSendtoAddr );

/************************************************************
 * ��������dsAddSrcSwitch
 *
 * ���ܣ� ���Ӱ�Դת���Ĺ���
 * 
 * ��Դת����ʹ�ô˹���ʱ���������ݰ���Դ��ַ����������е�Դ
 * ��ַ��ȡ�ÿ��ת��ʱ����Ҫ�ȸ������ݰ��е�Դ��ַ����ת����
 * ������Ҳ�������ʹ��Ĭ��Դ��ַ����0@0����ת������
 * ע�⣺�ýӿ�֧�ֶ�㵽һ�㡣���tSrcAddr��SrcIP��SrcPort��Ϊ�㣬
 * ������ȫ��ͬ��dsAddManyToOne��
 *
 * ʵ��˵����
 *
 * ����˵����
 * @param dsId           - �û���ʶ
 * @param ptRecvAddr     - ���յ�ַ�����������ݰ���Ŀ�ĵ�ַ
 *                        ���Դ��ַΪIPv6��ַ����õ�ַ����ΪIPv6��ַ��
 *                        ���齫�õ�ַͳһ����ΪIPv6��ַ
 * @param ptInLocalIP    - �������ݰ�������ӿ�
 * @param ptSrcAddr      - �������ݰ���Դ��ַ
 * @param ptSendtoAddr   - ת��Ŀ�ĵ�ַ
 * @param ptOutLocalIP   - ת�����ݰ����ñ���IP��������Ŀ�ĵ�ַͬЭ���壩
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/      
API u32 dsAddSrcSwitch(DSID dsId ,
                       TDSNetAddr* ptRecvAddr,
                       TDSNetAddr* ptInLocalIP,
					   TDSNetAddr* ptSrcAddr,
					   TDSNetAddr* ptSendtoAddr,
                       TDSNetAddr* ptOutLocalIP = NULL);

/************************************************************
 * ��������dsRemoveAllSrcSwitch
 *
 * ���ܣ� ɾ������ָ���İ�Դת������
 *
 * ʵ��˵����
 * 
 * ����˵����
 * @param dsId           - �û���ʶ
 * @param ptRecvAddr     - ���յ�ַ�����������ݰ���Ŀ�ĵ�ַ
 * @param ptSrcAddr      - �������ݰ���Դ��ַ
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/      
API u32 dsRemoveAllSrcSwitch(DSID dsId,
                             TDSNetAddr* ptRecvAddr,
                             TDSNetAddr* ptSrcAddr );

/************************************************************
 * ��������dsRemoveSrcSwitch
 * 
 * ���ܣ� ɾ��ָ���İ�Դת���Ĺ���
 *
 * ʵ��˵����
 * 
 * ����˵����
 * @param dsId          - �û���ʶ
 * @param ptRecvAddr     - ���յ�ַ�����������ݰ���Ŀ�ĵ�ַ
 * @param ptSrcAddr      - �������ݰ���Դ��ַ
 * @param ptSendtoAddr   - ת��Ŀ�ĵ�ַ
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/      
API u32 dsRemoveSrcSwitch(DSID dsId,
                          TDSNetAddr* ptRecvAddr,
                          TDSNetAddr* ptSrcAddr,
                          TDSNetAddr* ptSendtoAddr );

/************************************************************
 * ��������dsSetFilterFunc
 * 
 * ���ܣ� ���ù��˺���
 *
 * ע�⣺����ָ�����Ϊ�գ���ʾ���ԭ�е����ã����⣬����
 * DataSwitch����������߳��е��ô˺����ģ�Ҫ���õĺ�����
 * ����ȫ�ֺ��������õĲ�����Ҳ������ȫ����Ч�ġ�
 *
 * ʵ��˵����
 * 
 * ����˵����
 * @param dsId          - �û���ʶ
 * @param ptRecvAddr    - ���յ�ַ�����������ݰ���Ŀ�ĵ�ַ
 * @param ptFunc        - ���˺���ָ��
 * @param FuncParam     - Ϊ���˺������ݵĲ���
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/
API u32 dsSetFilterFunc(DSID dsId,
						TDSNetAddr* ptRecvAddr,
                        FilterFunc ptFunc);
 
/************************************************************
 * ��������dsRemoveAll
 * 
 * ���ܣ� ɾ�����е�ת������
 * 
 * ʵ��˵����
 * 
 * ����˵����
 * @param dsId          - �û���ʶ
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/      
API u32 dsRemoveAll( DSID dsId );

/**********************************************************
 * ����: dsSpecifyFwdSrc                                  *
 * ����: Ϊָ�����յ�ַ����ת�����ݰ�������Դ��ַ(IPv4) *
 * ����:
 * @param dsId           - DSID
 * @param ptOrigAddr     - ԭʼ��ַ
 * @param ptV4MappedAddr - IPv4ӳ���ַ
 * ���: ��                                               *
 * ����: �ɹ�����DSOK ����DSERROR                         *
 **********************************************************/
API u32 dsSpecifyFwdSrc(DSID dsId, 
						TDSNetAddr* ptOrigAddr,
						TDSNetAddr* ptV4MappedAddr );

/**********************************************************
 * ����: dsSpecifyFwdSrcV6                                 *
 * ����: Ϊָ�����յ�ַ����ת�����ݰ�������Դ��ַ(IPv6)  *
 * ����:
 * @param dsId           - DSID
 * @param ptOrigAddr     - ԭʼ��ַ
 * @param ptMappedAddrV6 - IPv6ӳ���ַ
 * ���: ��                                               *
 * ����: �ɹ�����DSOK ����DSERROR                         *
 **********************************************************/
API u32 dsSpecifyFwdSrcV6(DSID dsId, 
						TDSNetAddr* ptOrigAddr,
						TDSNetAddr* ptMappedAddrV6 );

/**********************************************************
 * ����: dsResetFwdSrc                                    *
 * ����: �ָ�ָ����ַת�����ݰ���Դ��ַ
 * ����:
 * @param dsId          - DSID
 * @param ptOrigAddr    - ԭʼIP
 * ���: ��                                               *
 * ����: �ɹ�����DSOK ����DSERROR                         *
 **********************************************************/
API u32 dsResetFwdSrc(DSID dsId, TDSNetAddr* ptOrigAddr );

/**********************************************************
 * ����: dsResetFwdSrcV6                                    *
 * ����: �ָ�ָ����ַת�����ݰ���Դ��ַ(IPv6)
 * ����:
 * @param dsId          - DSID
 * @param ptOrigAddr    - ԭʼIP
 * ���: ��                                               *
 * ����: �ɹ�����DSOK ����DSERROR                         *
 **********************************************************/
API u32 dsResetFwdSrcV6(DSID dsId, TDSNetAddr* ptOrigAddr );


 /**********************************************************
 * ����: dsSetSendCallback                                *
 * ����: ���÷��ͻص�����                                 *
 *       ȡ��ʱ�����ú���ָ��ΪNULL����
 * ����: dsID           - ��������ģ��ʱ�ľ��            *
 *       ptRcvAddr      - ���ؽ��յ�ַ                    *
 *       ptSrcAddr      - ת��Ŀ�ĵ�ַ                    *
 *       pfCallback     - �ص�����
 * ���: ��                                               *
 * ����: ��                                               *
 **********************************************************/    
API u32 dsSetSendCallback( DSID dsId, 
						   TDSNetAddr* ptRcvAddr,
						   TDSNetAddr* ptSrcAddr, 
						   SendCallback pfCallback);

 /**********************************************************
 * ����: dsSetAppDataForSend                              *
 * ����: Ϊ����Ŀ������һ���Զ����ָ��                   *
 *       ȡ��ʱ�����ú���ָ��ΪNULL����
 * ����: dsID           - ��������ģ��ʱ�ľ��            *
 *       ptRcvAddr      - ���ؽ��յ�ַ                    *
 *       ptSrcAddr      - Դ��ַ                          *
 *       ptDstAddr      - ת��Ŀ�ĵ�ַ                    *
 *       pAppData       - �Զ���ָ��
 * ���: ��                                               *
 * ����: 
 *     DSOK:�ɹ� DSERROR:ʧ��                             *
 **********************************************************/    
API u32 dsSetAppDataForSend( DSID dsId, 
							 TDSNetAddr* ptRcvAddr, 
							 TDSNetAddr* ptSrcAddr, 
							 TDSNetAddr* ptDstAddr, 
							 void * pAppData);
                
/**********************************************************
 * ����: dsGetRecvPktCount                                *
 * ����: ��ѯ�����ܰ���                                   *
 * ����: dsID            - ��������ģ��ʱ�ľ��           *
 *       ptRcvAddr       - ���ؽ��յ�ַ                   *
 *       ptSrcAddr       - Դ��ַ                         *
 *       dwRecvPktCount  - �����ܰ���                     *
 * ���: ��                                               *
 * ����: �ɹ�����DSOK ����DSERROR                         *
 **********************************************************/    
API u32 dsGetRecvPktCount( DSID dsId , 
						   TDSNetAddr* ptRcvAddr,
						   TDSNetAddr* ptSrcAddr,
						   u32& dwRecvPktCount );

/**********************************************************
 * ����: dsGetSendPktCount                                *
 * ����: ��ѯ�����ܰ���                                   *
 * ����: dsID            - ��������ģ��ʱ�ľ��           *
 *       ptRcvAddr       - ���ؽ��յ�ַ                   *
 *       ptSrcAddr       - Դ��ַ                         *
 *       ptSendtoAddr    - ת��Ŀ�ĵ�ַ                   *
 *       dwSendPktCount  - �����ܰ���                     *
 * ���: ��                                               *
 * ����: �ɹ�����DSOK ����DSERROR                         *
 **********************************************************/    
API u32 dsGetSendPktCount( DSID dsId , 
						   TDSNetAddr* ptRcvAddr, 
						   TDSNetAddr* ptSrcAddr, 
						   TDSNetAddr* ptSendtoAddr,
						   u32& dwSendPktCount );
						  
/**********************************************************
 * ����: dsGetRecvBytesCount	                          *
 * ����: ��ѯ�������ֽ���                                 *
 * ����:												  *
 * ���: ��                                               *
 * ����: ���ؼ�ʱ�����ֽ���								  *
 **********************************************************/
API s64 dsGetRecvBytesCount( );

/**********************************************************
 * ����: dsGetSendBytesCount	                          *
 * ����: ��ѯ�������ֽ���                                 *
 * ����:												  *
 * ���: ��                                               *
 * ����: ���ؼ�ʱ�ֽ���									  *
 **********************************************************/
API s64 dsGetSendBytesCount( );

/************************************************************
 * ������ dsinfo
 * ���ܣ� ��ʾ���е�ת�����򣬼��������ڼ����Ķ˿� 
 * ���룺
 * �����
 * ���أ�
 ************************************************************/      
API void dsinfo();

/************************************************************
 * ������ dsver
 * ���ܣ� ��ʾDataswitch�İ汾��Ϣ
 * ���룺
 * �����
 * ���أ�
 ************************************************************/      
API void dsver();

/************************************************************
 * ������ dshelp
 * ���ܣ� ��ʾDataswitch���ṩ������İ�����Ϣ
 * ���룺
 * �����
 * ���أ�
 ************************************************************/      
API void dshelp();

/************************************************************
 * ������ dedebug
 * ���ܣ� ��/�رյ�����Ϣ
 * 
 * ���룺
 * @param op            - ָ�������û��������set, clear
 *                        (����WIN32��Ч)
 * �����
 * ���أ�
 ************************************************************/      
API void dsdebug(char* op = NULL);

/************************************************************
 * ������ dedebug
 * ���ܣ� ��/�رո���һ��������Ϣ
 * �������ô˺�������Ϊ������Ϣ��Ӱ���������� 
 * 
 * ���룺
 * @param op            - ָ�������û��������set, clear
 *                        (����WIN32��Ч)
 * �����
 * ���أ�
 ************************************************************/      
API void dsdebug2();

#endif
