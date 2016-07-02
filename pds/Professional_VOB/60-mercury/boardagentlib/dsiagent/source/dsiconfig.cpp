/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : dsiconfig.cpp
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
#include "dsiconfig.h"

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
CDsiConfig::CDsiConfig()
{
	bIsRunPrs = FALSE;
}

//��������
CDsiConfig::~CDsiConfig()
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
BOOL CDsiConfig::IsRunPrs()
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
u8 CDsiConfig::GetBoardId()
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
u32 CDsiConfig::GetBrdIpAddr( )
{
	return htonl( m_dwBrdIpAddr );
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
u32 CDsiConfig::GetConnectMcuIpAddr()
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
u16 CDsiConfig::GetConnectMcuPort()
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
BOOL CDsiConfig::GetPrsCfg( TPrsCfg* ptCfg )
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
BOOL CDsiConfig::SetBrdIpAddr( u32 dwIp )
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
BOOL CDsiConfig::ReadConnectMcuInfo()
{
	s8    achProfileName[32];
    BOOL    bResult;
    s8    achDefStr[] = "Cannot find the section or key";
    s8    achReturn[MAX_VALUE_LEN + 1];
    s32  nValue;

	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);

    bResult = GetRegKeyString( achProfileName, "BoardSystem", "MpcIpAddr", 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( FALSE == bResult )  
	{
		OspPrintf( TRUE, TRUE, "[BoardAgent] Wrong profile while reading %s!\n", "MpcIpAddr" );
		m_dwMpcIpAddr = 0;
	}
    else
    {
        m_dwMpcIpAddr = ntohl( INET_ADDR( achReturn ) );
		if( 0xffffffff == m_dwMpcIpAddr )
		{
			m_dwMpcIpAddr = 0;
		}
    }

	bResult = GetRegKeyInt( achProfileName, "BoardSystem", "MpcPort", 0, &nValue );
	if( FALSE == bResult)  
	{
		OspPrintf( TRUE, TRUE, "[Agent] Wrong profile while reading %s!\n", "MpcPort" );
		nValue = MCU_LISTEN_PORT;
	}
	m_wMpcPort = ( u16 )nValue;

    memset(achReturn, '\0', sizeof(achReturn));
    bResult = GetRegKeyString( achProfileName, "BoardSystem", "MpcIpAddrB", 
                                            achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( FALSE == bResult )  
	{
		OspPrintf( TRUE, TRUE, "[Dri] Wrong profile while reading %s!\n", "MpcIpAddrB" );
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
        nValue = MCU_LISTEN_PORT;
    }
    m_wMpcPortB = (u16)nValue;

    if(m_dwMpcIpAddr == m_dwMpcIpAddrB)
    {
        m_dwMpcIpAddrB = 0;
        m_wMpcPortB = 0;
    }

    bResult = GetRegKeyInt(achProfileName, "IsFront", "Flag", 1, &nValue);
	if( FALSE == bResult )  
	{
		OspPrintf( TRUE, TRUE, "[Agent] Wrong profile while reading %s!\n", "IsFront" );
        nValue = 1;
	}
    m_byChoice = (u8)nValue;

#ifdef WIN32
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "brdcfgdebug.ini");
	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Layer", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, TRUE, "[Agent] Wrong profile while reading %s!\n", "Layer" );
		return( FALSE );
	}
	u8 byBrdLayer = (u8)nValue;

	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Slot", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, TRUE, "[Agent] Wrong profile while reading %s!\n", "Slot" );
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
        OspPrintf(TRUE, FALSE, "[Dsi] The A and B Mpc's Ip are all 0.\n");
        return FALSE;
    }

	return( TRUE );
}


BOOL CDsiConfig::ResumeE1RelayMode()
{
#ifndef WIN32
	char    achProfileName[32];
    BOOL    bResult;
    s32  sdwValue;

	memset( achProfileName, 0, sizeof(achProfileName) );
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "linkcfg.ini");

	bResult = GetRegKeyInt( achProfileName, "LinkConfig", "Link0", 0, &sdwValue );
	if( bResult == TRUE )  
	{
		u8 byMode = (u8)sdwValue;
		/*BrdDSISetRelayMode( 0, byMode );*/
                BrdSetE1RelayLoopMode( 0, byMode );
	}

	bResult = GetRegKeyInt( achProfileName, "LinkConfig", "Link1", 0, &sdwValue );
	if( bResult == TRUE )  
	{
		u8 byMode = (u8)sdwValue;
		/*BrdDSISetRelayMode( 1, byMode );*/
                BrdSetE1RelayLoopMode( 1, byMode );
	}

	bResult = GetRegKeyInt( achProfileName, "LinkConfig", "Link2", 0, &sdwValue );
	if( bResult == TRUE )  
	{
		u8 byMode = (u8)sdwValue;
		/*BrdDSISetRelayMode( 2, byMode );*/
                BrdSetE1RelayLoopMode( 2, byMode );
	}

	bResult = GetRegKeyInt( achProfileName, "LinkConfig", "Link3", 0, &sdwValue );
	if( bResult == TRUE )  
	{
		u8 byMode = (u8)sdwValue;
		/*BrdDSISetRelayMode( 3, byMode );*/
                BrdSetE1RelayLoopMode( 3, byMode );
	}
#endif
	
	return TRUE;	
}


/*====================================================================
���ܣ���E1��RELAY MODE���浽�ļ�
��������
����ֵ��TRUE/FALSE
====================================================================*/
BOOL CDsiConfig::SaveE1RelayMode( u8* byInfo, u16 wInfoLen )
{
#ifndef WIN32
	char    achProfileName[32];
    BOOL    bResult;
    s32  sdwValue;
    FILE    *stream;

	if( wInfoLen == 0 )
	{
		return FALSE;
	}
	
	memset( achProfileName, 0, sizeof(achProfileName) );
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "linkcfg.ini");

	//���ȿ���û��LINKCFG.INI�ļ�
    stream = fopen( achProfileName, "rb" );
    if( stream != NULL )
    {
		fclose( stream );
    }
	else
	{
		//û�У�Ҫ�������ļ�
		stream = fopen( achProfileName, "w" );
		fclose( stream );
	}

	for( u8 byLoop = 0; byLoop < byInfo[0]; byLoop++ )
	{
		u8 abyKey[32];
		memset( abyKey, 0, sizeof(abyKey) );

		sprintf( (char*)abyKey, (char*)"Link%u", byInfo[ byLoop*2 + 1] );
		bResult = SetRegKeyInt( achProfileName, (s8*)"LinkConfig", (s8*)abyKey, byInfo[ byLoop*2 + 2 ] );
		if( bResult == FALSE)  
		{
			OspPrintf( TRUE, TRUE, "Error Save E1 Link%u Info\n", byInfo[ byLoop*2 + 1] );
		}
	}

#endif
	
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
BOOL CDsiConfig::SetPrsConfig( TEqpPrsEntry *ptPrsCfg )
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

