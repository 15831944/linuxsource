/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : driconfig.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ��������ṩ���ϲ�Ӧ�õĽӿ�ʵ��
   ����        : jianghy
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/09/12  1.0         jianghy       ����
   2004/12/05  3.5         �� ��        �½ӿڵ��޸�
******************************************************************************/
#include "osp.h"
#include "dri_config.h"


//���캯��
CDriConfig::CDriConfig()
{
	bIsRunPrs = FALSE;
}

//��������
CDriConfig::~CDriConfig()
{
}

/*====================================================================
    ������      ��IsRunPrs
    ����        ���Ƿ�����prs
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������ 
    ����ֵ˵��  �����з���TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/20    1.0         jianghy       ����
====================================================================*/
BOOL CDriConfig::IsRunPrs()
{
	return bIsRunPrs;
}

/*====================================================================
    ������      ��GetPrsCfg
    ����        ����ȡprs������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��TRUE / FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/20    1.0         jianghy       ����
====================================================================*/
BOOL CDriConfig::GetPrsCfg( TPrsCfg* ptCfg )
{
	if( ptCfg == NULL )
		return FALSE;

	if( bIsRunPrs == FALSE)
		return FALSE;

	*ptCfg = m_prsCfg;
	return TRUE;
}

/*====================================================================
    ������      ��SetPrsConfig
    ����        ������PRS��������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����Prs������Ϣ�ṹָ��
    ����ֵ˵��  ��TRUE/FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/20    1.0         jianghy       ����
====================================================================*/
BOOL CDriConfig::SetPrsConfig( TEqpPrsEntry *ptPrsCfg )
{
	if( ptPrsCfg == NULL )
		return FALSE;
	
	bIsRunPrs = TRUE;
	
	m_prsCfg.wMcuId = ptPrsCfg->GetMcuId();
	m_prsCfg.dwConnectIP  = ptPrsCfg->GetMcuIp();
	m_prsCfg.wConnectPort = ptPrsCfg->GetMcuPort();
	m_prsCfg.byEqpType = ptPrsCfg->GetType();
	
	m_prsCfg.byEqpId = ptPrsCfg->GetEqpId();
	strncpy( m_prsCfg.achAlias, ptPrsCfg->GetAlias(), sizeof(m_prsCfg.achAlias) );
	m_prsCfg.achAlias[ sizeof(m_prsCfg.achAlias) - 1 ] = '\0';
	m_prsCfg.dwLocalIP = ptPrsCfg->GetIpAddr();
	m_prsCfg.wRcvStartPort = ptPrsCfg->GetEqpRecvPort();
	m_prsCfg.m_wFirstTimeSpan = ptPrsCfg->GetFirstTimeSpan();
	m_prsCfg.m_wSecondTimeSpan = ptPrsCfg->GetSecondTimeSpan();
	m_prsCfg.m_wThirdTimeSpan = ptPrsCfg->GetThirdTimeSpan();
	m_prsCfg.m_wRejectTimeSpan = ptPrsCfg->GetRejectTimeSpan();
	
    m_prsCfg.dwConnectIpB = GetMpcIpB();
    m_prsCfg.wConnectPortB = GetMpcPortB();
	return TRUE;
}

