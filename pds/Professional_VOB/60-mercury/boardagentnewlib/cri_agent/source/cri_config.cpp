/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : criconfig.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ��������ṩ���ϲ�Ӧ�õĽӿ�ʵ��
   ����        : jianghy
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/09/12  1.0         jianghy       ����
   2004/11/11  3.5         �� ��         �½ӿڵ��޸�
******************************************************************************/
#include "osp.h"
#include "cri_config.h"

// ���캯��
CCriConfig::CCriConfig()
{
	bIsRunPrs = FALSE;
}

// ��������
CCriConfig::~CCriConfig()
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
BOOL CCriConfig::IsRunPrs()
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
BOOL CCriConfig::GetPrsCfg( TPrsCfg* ptCfg )
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
BOOL CCriConfig::SetPrsConfig( TEqpPrsEntry *ptPrsCfg )
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

/*=============================================================================
�� �� ���� IsPrsCfgEqual
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const TPrsCfg &tPrsCfg
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/9/18   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CCriConfig::IsPrsCfgEqual( const TPrsCfg &tPrsCfg)
{
	if ( tPrsCfg.m_wFirstTimeSpan == m_prsCfg.m_wFirstTimeSpan
		&& tPrsCfg.m_wRejectTimeSpan == m_prsCfg.m_wRejectTimeSpan
		&& tPrsCfg.m_wSecondTimeSpan == m_prsCfg.m_wSecondTimeSpan
		&& tPrsCfg.m_wThirdTimeSpan == m_prsCfg.m_wThirdTimeSpan )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CCriConfig::GetBoardSequenceNum( u8 *pBuf, s8* achSep )
{
//#ifndef WIN32
	TBrdEthParam tBrdEthParam;

#if defined (_IS22_)
	s8    achProfileName[32] = {0};
	s32   dwFlag;
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);

	BOOL32 bResult = GetRegKeyInt(achProfileName, SECTION_IsFront, KEY_Flag, 1, &dwFlag);
	 if(FALSE == bResult)
	 {
		 printf( "[GetBoardSequenceNum] Wrong profile while reading %s!\n", "IsFront" );
		 dwFlag = 1;
	 }
	u8 byEthId = (u8)dwFlag;   // 0: front , 1: back
	printf("===========Is2.2 mtadp eth choice.%d===========\n",byEthId);
#else

	u8 byEthId = GetBrdEthChoice();//  GetInterface(); // ǰ������ѡ��
#endif

	
    BrdGetEthParam( byEthId, &tBrdEthParam );
	
	s8 achMacBuf[64];
	memset( achMacBuf, 0, sizeof(achMacBuf) );
    s8 szTmp[3] = {0};
	u8 byLp = 0;
	while(byLp < 6)
	{
        sprintf(szTmp, "%.2X", tBrdEthParam.byMacAdrs[byLp] );
        strcat( achMacBuf, szTmp);
        if (byLp < 5)
        {
            // guzh [7/3/2007] �Զ���ָ��
            strcat(achMacBuf, achSep);
        }
        byLp ++;
	}
	
	memcpy( pBuf, achMacBuf, strlen(achMacBuf) );
	
	return;
}

/*====================================================================
    ������      : InitLocalCfgInfoByCfgFile
    ����        : �������ļ���ʼ������������Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2012/05/05  4.7         liaokang      ����
====================================================================*/
BOOL32 CCriConfig::InitLocalCfgInfoByCfgFile()
{    
    s8     achProfileName[32] = {0};
    BOOL32 bResult = FALSE;
    s8     achDefStr[] = "Cannot find the section or key";
    s8     achReturn[MAX_VALUE_LEN + 1] = {0};
    s32    nValue = 0;
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);

    // ֧��ͨ�������ļ����õ�����ϵͳip
    BOOL32 bIsSupportIpConfig = 0;         // �Ƿ�֧�����õ�����ϵͳ ��0���ǣ�1
    u8     byEthChoice = 1;                // ������ϵͳ ǰ/������  ǰ���ڣ�0�������ڣ�1
    u32    dwBrdMasterSysIP = 0;           // ������ϵͳ primary ip
    u32    dwBrdMasterSysInnerIP = 0;      // ������ϵͳ �ڲ�ͨ��(second ip)
    u32    dwBrdMasterSysIPMask = 0;       // ������ϵͳ ����
    u32    dwBrdMasterSysDefGateway = 0;   // ������ϵͳ Ĭ������
    TBrdEthParam tEthParam;
    memset( &tEthParam, 0, sizeof( TBrdEthParam ) );
    TBrdEthParamAll tBrdEthParamAll;
    memset( &tBrdEthParamAll, 0, sizeof( TBrdEthParamAll ) );

    // 1���Ƿ�֧�����õ�����ϵͳ
    bResult = GetRegKeyInt( achProfileName, SECTION_IpConfig, KEY_IsSupportIpConfig, DEFVALUE_IsSupportIpConfig, &nValue );
    if( bResult == FALSE )  
    {
        LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS,
            "[InitLocalCfgInfoByCfgFile] Wrong profile while reading %s!\n", KEY_IsSupportIpConfig );
        return FALSE;
    }
	bIsSupportIpConfig = ( BOOL )nValue; 
    if ( 0 == bIsSupportIpConfig ) // ��֧��
    {
        LogPrint( LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS,
            "[InitLocalCfgInfoByCfgFile] Do not support Ip Config in config file!\n" );
        return TRUE;
    }



    // 2��������ϵͳ����
    bResult = GetRegKeyString( achProfileName, SECTION_IpConfig, KEY_BrdMasterSysDefGateway, 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )  
    {
        LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS,
            "[InitLocalCfgInfoByCfgFile] Wrong profile while reading %s!\n", KEY_BrdMasterSysDefGateway );
    }
    dwBrdMasterSysDefGateway =  ntohl( INET_ADDR( achReturn ) );
    if( 0xffffffff == dwBrdMasterSysDefGateway || 0 == dwBrdMasterSysDefGateway )
    {
        LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS, 
            "[InitLocalCfgInfoByCfgFile] The default gateway is 0 in config file\n" );
#ifdef _LINUX_
        u32 dwDefGateway = ntohl( BrdGetDefGateway() );
        if ( 0 == dwDefGateway )
        {
            LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS,
                "[InitLocalCfgInfoByCfgFile] Get default gateway failed!\n" );
            return FALSE;
        }  
#endif
    }
    else
    {
#ifdef _LINUX_
        // ���õ�����ϵͳ����
        s32 nRet = BrdSetDefGateway( htonl( dwBrdMasterSysDefGateway ) );
        if ( ERROR == nRet )
        {
            LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS, 
                "[InitLocalCfgInfoByCfgFile] Set default gateway : %0x failed!\n", dwBrdMasterSysDefGateway );
            return FALSE;
        }
        else
        {
            LogPrint( LOG_LVL_DETAIL, MID_PUB_ALWAYS,
                "[InitLocalCfgInfoByCfgFile] Set default gateway : %0x success!\n", dwBrdMasterSysDefGateway );
        }
#endif
    }

    // 3��������ϵͳ ǰ/������,Ĭ�Ϻ�����
    bResult = GetRegKeyInt( achProfileName, SECTION_IpConfig, KEY_EthChoice, DEFVALUE_EthChoice, &nValue );
    if( bResult == FALSE )
    {
        LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS,
            "[InitLocalCfgInfoByCfgFile] Wrong profile while reading %s!\n", KEY_EthChoice );
    }
	byEthChoice = ( u8 )nValue; 
    LogPrint( LOG_LVL_DETAIL, MID_PUB_ALWAYS,
                "[InitLocalCfgInfoByCfgFile] Set Ethernet Choice : %u!\n", byEthChoice );  

    // 4��������ϵͳ ����
    bResult = GetRegKeyString( achProfileName, SECTION_IpConfig, KEY_BrdMasterSysIPMask, 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )  
    {
        LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS,
            "[InitLocalCfgInfoByCfgFile] Wrong profile while reading %s!\n", KEY_BrdMasterSysIPMask );
    }
    dwBrdMasterSysIPMask =  ntohl( INET_ADDR( achReturn ) );
    if( 0xffffffff == dwBrdMasterSysIPMask || 0 == dwBrdMasterSysIPMask )
    {
        LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS,
            "[InitLocalCfgInfoByCfgFile] The ip mask is 0 in config file!\n" );
        // Ĭ�ϲ���ԭ������
        memset( &tBrdEthParamAll, 0, sizeof( TBrdEthParamAll ) );
        s32 nRet = BrdGetEthParamAll( byEthChoice, &tBrdEthParamAll );
        if ( ERROR == nRet
            || tBrdEthParamAll.dwIpNum == 0 
            || ( tBrdEthParamAll.dwIpNum > 0 && 0 == tBrdEthParamAll.atBrdEthParam[0].dwIpMask ) )
        {
            LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS,
                "[InitLocalCfgInfo] The primary IpMask is not exist in board system! Please config...\n" );
            return FALSE;
        }
        dwBrdMasterSysIPMask = ntohl( tBrdEthParamAll.atBrdEthParam[0].dwIpMask );
    }

    // 5��������ϵͳ ip
    bResult = GetRegKeyString( achProfileName, SECTION_IpConfig, KEY_BrdMasterSysIp, 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )  
    {
        LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS,
            "[InitLocalCfgInfoByCfgFile] Wrong profile while reading %s!\n", KEY_BrdMasterSysIp );
    }
    dwBrdMasterSysIP =  ntohl( INET_ADDR( achReturn ) );
     /* ��ӵڶ�IP֮ǰ����һIP������� */
    if( 0xffffffff == dwBrdMasterSysIP || 0 == dwBrdMasterSysIP )
    { 
        LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS,
            "[InitLocalCfgInfoByCfgFile] The primary IpAddr is 0 in config file!\n" );  
#ifdef _LINUX_
        // �ж� ������ϵͳ primary ip �Ƿ��Ѿ�����        
        memset( &tBrdEthParamAll, 0, sizeof(TBrdEthParamAll) );
        s32 nRet = BrdGetEthParamAll( byEthChoice, &tBrdEthParamAll );
        if ( ERROR == nRet
            || tBrdEthParamAll.dwIpNum == 0 
            || ( tBrdEthParamAll.dwIpNum > 0 && 0 == tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs )
            || ( tBrdEthParamAll.dwIpNum > 0 && 0 == tBrdEthParamAll.atBrdEthParam[0].dwIpMask ) )
        {
            LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS,
                "[InitLocalCfgInfo] The primary IpAddr or IpMask is not exist in board system! Please config...\n" );
            return FALSE;
        }
#endif
    }
    else
    {
#ifdef _LINUX_
        // ���õ�����ϵͳ primary ip
        memset( &tEthParam, 0, sizeof(tEthParam) );
        tEthParam.dwIpAdrs = htonl( dwBrdMasterSysIP );
        tEthParam.dwIpMask = htonl( dwBrdMasterSysIPMask );        
        s32 nRet = BrdSetEthParam( byEthChoice, Brd_SET_IP_AND_MASK, &tEthParam );
        if ( ERROR == nRet )
        {
            LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS,
                "[InitLocalCfgInfoByCfgFile] Set primary IpAddr:%0x, IPMask:%0x in Ethernet Choice : %u failed!\n", dwBrdMasterSysIP, dwBrdMasterSysIPMask, byEthChoice );
            return FALSE;       
        }
        else
        {
            LogPrint( LOG_LVL_DETAIL, MID_PUB_ALWAYS,
                 "[InitLocalCfgInfoByCfgFile] Set primary IpAddr:%0x, IPMask:%0x in Ethernet Choice : %u success!\n", dwBrdMasterSysIP, dwBrdMasterSysIPMask, byEthChoice );
        }
#endif
    }

    // 6��������ϵͳ �ڲ�ͨ�� ip
    bResult = GetRegKeyString( achProfileName, SECTION_IpConfig, KEY_BrdMasterSysInnerIp, 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )  
    {
        LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS,
            "[InitLocalCfgInfoByCfgFile] Wrong profile while reading %s!\n", KEY_BrdMasterSysInnerIp );
        return FALSE;
    }
    dwBrdMasterSysInnerIP =  ntohl( INET_ADDR( achReturn ) );
    if( 0xffffffff == dwBrdMasterSysInnerIP ||  0 == dwBrdMasterSysInnerIP )
    {
        LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS, 
            "[InitLocalCfgInfoByCfgFile] The inner ip is 0 in config file!\n" );
        return FALSE;
    }
    // ���õ�����ϵͳ�ڲ�ͨ��ip
#ifdef _LINUX12_  // Ŀǰ��nipwrapper.h֧�ֵڶ�IP
    /* ֱ�����second ip ���ܽ��: �����ͬ����ip�����ͻ����ʧ��
                                   �粻���ڣ������һ��second ip */ 

    /* Լ�� �ڲ�ͨ�� ip ��Ϊ second ip �ĵ�һ��ip */ 
    
    memset( &tBrdEthParamAll, 0, sizeof(TBrdEthParamAll) );
    s32 nRet = BrdGetEthParamSecIP( byEthChoice, &tBrdEthParamAll );
    // ������second ip
    if ( OK == nRet && 0 == tBrdEthParamAll.dwIpNum )
    {
        // ����ڲ�ͨ�� ip
        memset( &tEthParam, 0, sizeof(tEthParam) );
        tEthParam.dwIpAdrs = htonl( dwBrdMasterSysInnerIP );
        tEthParam.dwIpMask = htonl( dwBrdMasterSysIPMask ); 
        nRet = BrdSetEthParam( byEthChoice, Brd_SET_ETH_SEC_IP, &tEthParam );
        if ( ERROR == nRet )
        {
            LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS,
                "[InitLocalCfgInfoByCfgFile] Set inner Ip:%0x, IPMask:%0x in Ethernet Choice : %u failed!\n", dwBrdMasterSysInnerIP, dwBrdMasterSysIPMask, byEthChoice );
        }
        else
        {
            LogPrint( LOG_LVL_DETAIL, MID_PUB_ALWAYS,
                "[InitLocalCfgInfoByCfgFile] Set inner Ip:%0x, IPMask:%0x in Ethernet Choice : %u success!\n", dwBrdMasterSysInnerIP, dwBrdMasterSysIPMask, byEthChoice );
        }
        return FALSE;
    }
    // ����second ip
    else if ( OK == nRet && tBrdEthParamAll.dwIpNum > 0 )
    {
        // <1> �����¼����ʱ����
        TBrdEthParamAll tTempBrdEthParamAll;
        memset( &tTempBrdEthParamAll, 0, sizeof(TBrdEthParamAll) );
        // �������õ��ڲ�ͨ�� ip�洢����ʱ�����0λ
        tTempBrdEthParamAll.atBrdEthParam[0].dwIpAdrs = htonl( dwBrdMasterSysInnerIP );
        tTempBrdEthParamAll.atBrdEthParam[0].dwIpMask = htonl( dwBrdMasterSysIPMask );
        tTempBrdEthParamAll.dwIpNum = 1;
        // ��˳�򽫵�1��Num - 1λ��ip�洢����ʱ����ĵ�1��Num - 1λ
        for ( u8 byLop = 1; byLop < tBrdEthParamAll.dwIpNum; byLop++ )
        {
            tTempBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs = tBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs;
            tTempBrdEthParamAll.atBrdEthParam[byLop].dwIpMask = tBrdEthParamAll.atBrdEthParam[byLop].dwIpMask;
            tTempBrdEthParamAll.dwIpNum ++;
        }
        // <2> ɾ��ԭ��second ip 
        for ( u8 byLop = 0; byLop < tBrdEthParamAll.dwIpNum; byLop++ )
        {
            memset( &tEthParam, 0, sizeof(tEthParam) );
            tEthParam.dwIpAdrs = tBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs;
            tEthParam.dwIpMask = tBrdEthParamAll.atBrdEthParam[byLop].dwIpMask; 
            nRet = BrdDelEthParamSecIP( byEthChoice, &tEthParam );
            if ( ERROR == nRet )
            {
                LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS,
                    "[InitLocalCfgInfoByCfgFile] Set second Ip:%0x, IPMask:%0x in Ethernet Choice : %u failed!\n", dwBrdMasterSysInnerIP, dwBrdMasterSysIPMask, byEthChoice );
            }
            else
            {
                LogPrint( LOG_LVL_DETAIL, MID_PUB_ALWAYS,
                    "[InitLocalCfgInfoByCfgFile] Set second Ip:%0x, IPMask:%0x in Ethernet Choice : %u success!\n", dwBrdMasterSysInnerIP, dwBrdMasterSysIPMask, byEthChoice );
            }
        }
        // <3> ������ʱ����������¼��second ip
        for ( u8 byLop = 0; byLop < tTempBrdEthParamAll.dwIpNum; byLop++ )
        {
            memset( &tEthParam, 0, sizeof(tEthParam) );
            tEthParam.dwIpAdrs = tTempBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs;
            tEthParam.dwIpMask = tTempBrdEthParamAll.atBrdEthParam[byLop].dwIpMask; 
            nRet = BrdSetEthParam( byEthChoice, Brd_SET_ETH_SEC_IP, &tEthParam );
            if ( ERROR == nRet )
            {
                LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS,
                    "[InitLocalCfgInfoByCfgFile] Set second Ip:%0x, IPMask:%0x in Ethernet Choice : %u failed!\n", dwBrdMasterSysInnerIP, dwBrdMasterSysIPMask, byEthChoice );
            }
            else
            {
                LogPrint( LOG_LVL_DETAIL, MID_PUB_ALWAYS,
                    "[InitLocalCfgInfoByCfgFile] Set second Ip:%0x, IPMask:%0x in Ethernet Choice : %u success!\n", dwBrdMasterSysInnerIP, dwBrdMasterSysIPMask, byEthChoice );
            }
        }
    }
#endif //end _LINUX12_

    return TRUE;
}

/*====================================================================
    ������      : SetBrdMasterSysListenPort
    ����        : ���õ�����ϵͳ�ļ����˿�
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: u16 wListenPort �����˿ں�
    ����ֵ˵��  : BOOL32 
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2012/05/05  4.7         liaokang      ����
====================================================================*/
BOOL32 CCriConfig::SetBrdMasterSysListenPort( u16 wListenPort )
{
    s8     achProfileName[32] = {0};
    BOOL32 bResult = FALSE;
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);
    // ������ϵͳ��IS2.2 8548�� Listen Port
    bResult = SetRegKeyInt( achProfileName, SECTION_BoardMasterSystem, KEY_BrdMasterSysListenPort, wListenPort );
    if( bResult == FALSE )
    {
        LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS,
            "[GetBrdMasterSysListenPort] Wrong profile while writeing %s!\n", KEY_BrdMasterSysListenPort );
    }  
    return bResult;
}

/*====================================================================
    ������      : GetBrdMasterSysListenPort
    ����        : ��ȡ������ϵͳ�ļ����˿�
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    ����ֵ˵��  : u16 �����˿ں�
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2012/05/05  4.7         liaokang      ����
====================================================================*/
u16 CCriConfig::GetBrdMasterSysListenPort(void)
{
    s8     achProfileName[32] = {0};
    BOOL32 bResult = FALSE;
    s32    nValue = 0;
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);

    // ��ȡ��ϵͳ��IS2.2 8548�� Listen Port
    bResult = GetRegKeyInt( achProfileName, SECTION_BoardMasterSystem, KEY_BrdMasterSysListenPort, 
        DEFVALUE_BrdMasterSysListenPort, &nValue );
    if( bResult == FALSE )  
    {
        LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS,
            "[GetBrdMasterSysListenPort] Wrong profile while reading %s!\n", KEY_BrdMasterSysListenPort );
    }  
    return ( u16 )nValue;
}
