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
#include "mcuconst.h"
#include "vccommon.h"
#include "driconfig.h"

#ifdef _VXWORKS_
#include "brddrvLib.h"
#endif

// [pengjie 2010/3/9] CRI2/MPC2 ֧��
#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
        #include "brdwrapperdef.h"
        #include "nipwrapper.h"
        #include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
#endif

//���캯��
CDriConfig::CDriConfig()
{
	bIsRunPrs = FALSE;
    m_dwBrdIpAddr= 0;
    m_dwMpcIpAddr = 0;
    m_wMpcPort = 0;
    m_dwMpcIpAddrB = 0;
    m_wMpcPortB = 0;
    m_wDiscHeartBeatTime = DEF_OSPDISC_HBTIME;
    m_byDiscHeartBeatNum = DEF_OSPDISC_HBNUM;
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
    ������      ��GetBoardId
    ����        ����ȡ����Ĳ�λ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��IDֵ��0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/20    1.0         jianghy       ����
====================================================================*/
u8 CDriConfig::GetBoardId()
{
	return m_byBrdId ;
}

/*====================================================================
    ������      ��GetBrdIpAddr
    ����        ���õ�����IP��ַ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ������IP��ַ(������)
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/20    1.0         jianghy       ����
====================================================================*/
u32 CDriConfig::GetBrdIpAddr( )
{
	return htonl( m_dwBrdIpAddr );
}

/*=============================================================================
  �� �� ���� GetMpcBIp
  ��    �ܣ� ȡMpcB��Ip
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� u32 ��������
=============================================================================*/
u32 CDriConfig::GetMpcBIp(void)
{
    return htonl(m_dwMpcIpAddrB);
}

/*=============================================================================
  �� �� ���� SetMpcBIp
  ��    �ܣ� ����MpcB��Ip
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwIp��������
  �� �� ֵ�� void 
=============================================================================*/
void CDriConfig::SetMpcBIp(u32 dwIp)
{
    m_dwMpcIpAddrB = ntohl(dwIp);
}

/*=============================================================================
  �� �� ���� GetMpcAIp
  ��    �ܣ� ����MpcA��Ip
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� u32 ��������
=============================================================================*/
u32 CDriConfig::GetMpcAIp(void)
{
    return htonl(m_dwMpcIpAddr);
}

/*=============================================================================
  �� �� ���� SetMpcAIp
  ��    �ܣ� ����MpcB��Ip
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwIp��������
  �� �� ֵ�� void
=============================================================================*/
void CDriConfig::SetMpcAIp(u32 dwIp)
{
    m_dwMpcIpAddr = ntohl(dwIp);
}

/*=============================================================================
  �� �� ���� GetMpcAPort
  ��    �ܣ� ȡMpcA�˿�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u16 
=============================================================================*/
u16 CDriConfig::GetMpcAPort()
{
    return m_wMpcPort;
}
void CDriConfig::SetMpcAPort(u16 wPort)
{
    m_wMpcPort = wPort;
}

/*=============================================================================
  �� �� ���� GetMpcBPort
  ��    �ܣ� ȡMpcB�˿�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u16 
=============================================================================*/
u16 CDriConfig::GetMpcBPort()
{
    return m_wMpcPortB;
}
void CDriConfig::SetMpcBPort(u16 wPort)
{
    m_wMpcPortB = wPort;
}

/*=============================================================================
  �� �� ���� SetDriIp
  ��    �ܣ� ����dri ip 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwIp��������
  �� �� ֵ�� void 
=============================================================================*/
void CDriConfig::SetDriIp(u32 dwIp)
{
    m_dwBrdIpAddr = ntohl(dwIp);
}

/*=============================================================================
  �� �� ���� GetDriIp
  ��    �ܣ� ȡDri��Ip
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� u32 ��������
=============================================================================*/
u32 CDriConfig::GetDriIp(void)
{
    return ntohl(m_dwBrdIpAddr);
}

/*====================================================================
    ������      ��GetConnectMcuIpAddr
    ����        ����ȡҪ���ӵ�MCU��IP��ַ(������)
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��MCU��IP��ַ(������)��0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/20    1.0         jianghy       ����
====================================================================*/
u32 CDriConfig::GetConnectMcuIpAddr()
{
	return htonl(m_dwMpcIpAddr);
}

/*====================================================================
    ������      ��GetConnectMcuPort
    ����        ����ȡ���ӵ�MCU�Ķ˿ں�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��MCU�Ķ˿ںţ�0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/20    1.0         jianghy       ����
====================================================================*/
u16 CDriConfig::GetConnectMcuPort()
{
	return m_wMpcPort;
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
    ������      ��SetBrdIpAddr
    ����        ���õ�����IP��ַ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ������IP��ַ(������)
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/20    1.0         jianghy       ����
====================================================================*/
BOOL CDriConfig::SetBrdIpAddr( u32 dwIp )
{
	m_dwBrdIpAddr = dwIp;
	return TRUE;
}

/*====================================================================
    ������      ��ReadConnectMcuInfo
    ����        ����ȡ���ӵ�MCU��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��MCU�Ķ˿ںţ�0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/20    1.0         jianghy       ����
====================================================================*/
BOOL CDriConfig::ReadConnectMcuInfo()
{
	s8    achProfileName[32];
    BOOL    bResult;
    s8    achDefStr[] = "Cannot find the section or key";
    s8    achReturn[MAX_VALUE_LEN + 1];
    s32   nValue;

	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);

    bResult = GetRegKeyString( achProfileName, "BoardSystem", "MpcIpAddr", 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[BoardAgent] Wrong profile while reading %s!\n", "MpcIpAddr" );
		return( FALSE );
	}
    m_dwMpcIpAddr = ntohl( INET_ADDR( achReturn ) );
	if( 0xffffffff == m_dwMpcIpAddr )
	{
		m_dwMpcIpAddr = 0;
	}

	bResult = GetRegKeyInt( achProfileName, "BoardSystem", "MpcPort", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[Agent] Wrong profile while reading %s!\n", "MpcPort" );
		return( FALSE );
	}
	m_wMpcPort = ( u16 )nValue;

    memset(achReturn, '\0', sizeof(achReturn));
    bResult = GetRegKeyString( achProfileName, "BoardSystem", "MpcIpAddrB", 
                                            achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( FALSE == bResult )  
	{
		OspPrintf( TRUE, FALSE, "[Dri] Wrong profile while reading %s!\n", "MpcIpAddrB" );
        m_dwMpcIpAddrB = 0;
	}
    else
    {
        m_dwMpcIpAddrB = ntohl( INET_ADDR( achReturn ) );
		if( 0xffffffff == m_dwMpcIpAddrB )
		{
			m_dwMpcIpAddrB = 0;
		}
    }

    bResult = GetRegKeyInt(achProfileName, "BoardSystem", "MpcPortB", 0, &nValue);
    if(FALSE == bResult)
    {
        OspPrintf(TRUE, FALSE, "[Dri] Wrong profile while reading %s\n", "MpcPortB");
        nValue = 0;
    }
    m_wMpcPortB = (u16)nValue;

    if(m_dwMpcIpAddr == m_dwMpcIpAddrB)// ������
    {
        m_dwMpcIpAddrB = 0;
        m_wMpcPortB = 0;
    }
	
    bResult = GetRegKeyInt(achProfileName, "IsFront", "Flag", 1, &nValue);
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[Agent] Wrong profile while reading %s!\n", "IsFront" );
        nValue = 1;
	}
    m_byChoice = (u8)nValue;

#ifdef WIN32
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "brdcfgdebug.ini");
	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Layer", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[Agent] Wrong profile while reading %s!\n", "Layer" );
		return( FALSE );
	}
	u8 byBrdLayer = (u8)nValue;

	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Slot", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[Agent] Wrong profile while reading %s!\n", "Slot" );
		return( FALSE );
	}
	m_byBrdId = (byBrdLayer<<4) | ((u8)nValue+1);    

#else
	TBrdPosition tBoardPosition;
	BrdQueryPosition( &tBoardPosition );
	m_byBrdId = (tBoardPosition.byBrdLayer<<4) | (tBoardPosition.byBrdSlot+1);
#endif

    if(0 == m_dwMpcIpAddr && 0 == m_dwMpcIpAddrB)
    {
        OspPrintf(TRUE, FALSE, "[Dri] The A and B Mpc's Ip are all 0.\n");
        return FALSE;
    }

    //��ȡosp����������
    GetRegKeyInt( achProfileName, "BoardSystem", "HeartBeatTime", DEF_OSPDISC_HBTIME, &nValue );
    if (nValue >= 3)
    {
        m_wDiscHeartBeatTime = (u16)nValue;
    }
    
    GetRegKeyInt( achProfileName, "BoardSystem", "HeartBeatNum", DEF_OSPDISC_HBNUM, &nValue );
    if (nValue >= 1)
    {
        m_byDiscHeartBeatNum = (u8)nValue;
    }    

	return( TRUE );
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
	
    m_prsCfg.dwConnectIpB = m_dwMpcIpAddrB;
    m_prsCfg.wConnectPortB = m_wMpcPortB;
	return TRUE;
}

