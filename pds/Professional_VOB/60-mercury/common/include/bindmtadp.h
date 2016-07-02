/*****************************************************************************
   ģ����      : MtAdp
   �ļ���      : bindmtadp.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: MtAdpģ����ͣ�ӿ�
   ����        : ̷��
   �汾        : V0.1  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/11     0.1         ̷��        ����
******************************************************************************/
#ifndef __BINDMTADP_H_
#define __BINDMTADP_H_

#include "cristruct.h"

//MtAdp��Ϣ
struct TMtAdp
{
protected:
    u32  m_dwIpAddr;        //��MtAdpIp��ַ
    u8   m_byMtAdpId;       //��MtAdp���
    u8   m_byAttachMode;    //��MtAdp�ĸ�����ʽ
public:
    void SetMtadpId(u8 byMtAdpId){ m_byMtAdpId = byMtAdpId; }
    u8   GetMtadpId(void) const { return m_byMtAdpId; }
    void SetAttachMode(u8 byAttachMode) { m_byAttachMode = byAttachMode; }
    u8   GetAttachMode(void) const { return m_byAttachMode; }
    void SetIpAddr(u32 dwIP){ m_dwIpAddr = htonl(dwIP); }
    u32  GetIpAddr(void) { return ntohl(m_dwIpAddr); }

	TMtAdp( void )
	{
		memset( this, 0, sizeof( TMtAdp ) );
	};
};

struct TMtAdpConnectParam
{
public:
    u32  m_dwMcuTcpNode;  //0 when AttachVc
    u32  m_dwMcuIp;       //Mcu IP to connect to(net order)
    u16  m_wMcuPort;      //Mcu Port to connect to(host order)
    BOOL m_bSupportHD;    //Supoort or not
public:
    void SetMcuIpAddr(u32 dwIP){ m_dwMcuIp = htonl(dwIP); }
    u32  GetMcuIpAddr(void) { return ntohl(m_dwMcuIp); }
	
	TMtAdpConnectParam( void )
	{
		memset( this, 0, sizeof( TMtAdpConnectParam ) );
	};
};

/*=============================================================================
    �� �� ���� MtAdpStart
    ��    �ܣ� ����MtAdp
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TMtAdpConnectParam *ptConnectParam
               TMtAdp *ptMtadp
               TMtAdpConnectParam *ptConnectParam2 = NULL��when doublelinking is used��
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/9/10   4.0			����                  ����
=============================================================================*/
BOOL32 MtAdpStart(TMtAdpConnectParam *ptConnectParam, TMtAdp *ptMtadp, TMtAdpConnectParam *ptConnectParam2 = NULL, u8 *pabyBrdEthMac = NULL);

/*=============================================================================
    �� �� ���� MtAdpStart
    ��    �ܣ� ����MtAdp
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TMtAdpConnectParam *ptConnectParam
               TMtAdp *ptMtadp
               TMtAdpConnectParam *ptConnectParam2 = NULL��when doublelinking is used��
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/9/10   4.0			pgf                   ����
=============================================================================*/
BOOL32 MtAdpStart(	TMtIpMapTab* tMtIpMapTab, 
					TMtAdpConnectParam *ptConnectParam, TMtAdp *ptMtadp,				   
				    TCriIpMapTab* tCriIpMapTab,
				    TGKIpMapTab* tGKIpMapTab,
				    s8 *pchV6Ip,
				    s16 swScopeId,
					TMtAdpConnectParam *ptConnectParam2 = NULL);

/*=============================================================================
    �� �� ���� MtAdpStop
    ��    �ܣ� ֹͣMtAdp
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� void  
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/9/10   4.0			����                  ����
=============================================================================*/
void MtAdpStop();

#endif // __BINDMTADP_H_
