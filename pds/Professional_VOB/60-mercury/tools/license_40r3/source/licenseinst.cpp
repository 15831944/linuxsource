/*****************************************************************************
   ģ����      : license generate
   �ļ���      : licenseinst.cpp
   ����ļ�    : licenseinst.h
   �ļ�ʵ�ֹ���: ͨѶ�࣬ʵ�ָ�����˵���ϢͨѶ
   ����        : john
   �汾        : V0.9  Copyright(C) 2001-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2006/06/23  0.9         john         ����
******************************************************************************/
#include "licenseinst.h"
#include "evmcu.h"
#include "mcuconst.h"
#include "mcustruct.h"

#define IN
#define OUT 

CLicenseGenApp	g_cLicenseGenApp;

CLicenseGen::CLicenseGen()
{
	ClearInst();
}

CLicenseGen::~CLicenseGen()
{
	ClearInst();
}

void CLicenseGen::InstanceEntry( CMessage* const pcMsg )
{
	if( NULL == pcMsg )
	{
		return;
	}

	switch( pcMsg->event ) 
	{
	case OSP_POWERON:
		ProcPowerOn( pcMsg );
		break;

	case LGEN_MCUGD_GENERATELICENSE_REQ:
		ProcGenerateLicenseReq( pcMsg );
		break;

	case MCUGD_LGEN_GENERATELICENSE_ACK:
	case MCUGD_LGEN_GENERATELICENSE_NACK:
		ProcGenerateLicenseRsp( pcMsg );
		break;
		
	default:
		break;
	}
	return;
}

/*=============================================================================
  �� �� ���� ClearInst
  ��    �ܣ� ���ʵ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CLicenseGen::ClearInst( void )
{
	m_dwservernode = INVALID_NODE;
	m_dwserveripaddr = 0;
	m_wserverport = 0;
}

/*=============================================================================
  �� �� ���� ProcPowerOn
  ��    �ܣ� �ϵ磨���ӷ������ˣ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage *const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CLicenseGen::ProcPowerOn( const CMessage *const pcMsg )
{
	if( NULL == pcMsg )
	{
		return;
	}

	m_dwserveripaddr = htonl( *(u32*)pcMsg->content ); // host 
	m_wserverport = *(u16*)( pcMsg->content + sizeof(u32) ); // host

	
	if( INVALID_NODE != m_dwservernode )  // first disconnect to server
	{
		::OspDisconnectTcpNode( m_dwservernode );
	}

	m_dwservernode = OspConnectTcpNode( m_dwserveripaddr, m_wserverport );// connect to server
	
	if( INVALID_NODE == m_dwservernode )
	{
		// fail to connect to sever 
		OspPrintf(TRUE, FALSE, "connect to server fail.\n");
	}
	else
	{
		OspPrintf(TRUE, FALSE, "connect to server success.\n");
	}
	
	return;
}

/*=============================================================================
  �� �� ���� ProcGenerateLicenseRsp
  ��    �ܣ� ����ӷ���˷��ص�����license�Ļ�Ӧ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage * pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CLicenseGen::ProcGenerateLicenseRsp(const CMessage * pcMsg )
{
	if( NULL == pcMsg )
	{
		return;
	}
	
	if( MCUGD_LGEN_GENERATELICENSE_NACK == pcMsg->event )
	{
		OspPrintf( TRUE, FALSE, " invalid user name.\n");
		AfxMessageBox("generate license key failed");
		return;
	}
	else
	{
		AfxMessageBox("generate license key success");
	}
	return;

}

/*=============================================================================
  �� �� ���� ProcGenerateLicenseReq
  ��    �ܣ� ����ӽ��淢����������license����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CMessage *const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CLicenseGen::ProcGenerateLicenseReq( const CMessage *const pcMsg )
{
	if( NULL == pcMsg )
	{
		return;
	}

	// ��Ϣ�壺TLicenseParam
	post( MAKEIID( AID_MCU_GUARD, 1), LGEN_MCUGD_GENERATELICENSE_REQ,
		       pcMsg->content, pcMsg->length, m_dwservernode);
	
	return;
}
