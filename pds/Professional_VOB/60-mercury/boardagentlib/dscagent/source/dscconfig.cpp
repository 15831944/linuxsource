/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : dscconfig.cpp
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
#include "dscconfig.h"

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
    #include <sys/types.h>
    #include <dirent.h>
#endif

void Dsclog(char * fmt, ...);

/*=============================================================================
��    �ܣ� ������IP��ַת���ַ���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u32 dwIP   ������IP��ַ
�� �� ֵ�� 
=============================================================================*/
void GetIpFromU32(s8* achDstStr, u32 dwIpAddr)
{
    if( NULL != achDstStr )    
    {
        u32 dwTmpIp = ntohl(dwIpAddr);
        if( 0 != dwTmpIp )
        {
            sprintf(achDstStr, "%d.%d.%d.%d%c", (dwTmpIp>>24)&0xFF, (dwTmpIp>>16)&0xFF, (dwTmpIp>>8)&0xFF, dwTmpIp&0xFF, 0);
        }
        else
        {
            memcpy(achDstStr, "0", sizeof("0"));
        }
    }    
    return;
}

char * StrOfIP( u32 dwIpAddr )
{
    dwIpAddr = htonl(dwIpAddr);
    static char strIP[17];  
    u8 *p = (u8 *)&dwIpAddr;
    sprintf(strIP,"%d.%d.%d.%d%c",p[0],p[1],p[2],p[3],0);
    return strIP;
}

//���캯��
CDscConfig::CDscConfig()
{
	m_bIsRunPrs = FALSE;
	m_dwBrdIpAddr = 0;
    m_dwLocalInnerIp = 0;

    m_dwMpcIpAddr = 0;
    m_dwMpcIpAddrB = 0;
    m_wMpcPort = 0;
    m_wMpcPortB = 0;

    m_wDiscHeartBeatTime = DEF_OSPDISC_HBTIME;
    m_byDiscHeartBeatNum = DEF_OSPDISC_HBNUM;    

    m_bIsRunPrs = FALSE;
    m_byChoice = 0;
}

//��������
CDscConfig::~CDscConfig()
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
BOOL CDscConfig::IsRunPrs()
{
	return m_bIsRunPrs;
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
u8	CDscConfig::GetBoardId()
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
u32 CDscConfig::GetBrdIpAddr()
{
	return htonl(m_dwBrdIpAddr);
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
u32 CDscConfig::GetConnectMcuIpAddr()
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
u16 CDscConfig::GetConnectMcuPort()
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
BOOL CDscConfig::GetPrsCfg( TPrsCfg* ptCfg )
{
	if( NULL == ptCfg)
    {
        return FALSE;
    }

	if( FALSE == m_bIsRunPrs)
    {
        return FALSE;
    }

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
BOOL CDscConfig::SetBrdIpAddr( u32 dwIp )
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
BOOL CDscConfig::ReadConnectMcuInfo()
{
	if ( !CreatBrdCfgFile() )
	{
		OspPrintf(TRUE, FALSE, "[ReadConnectMcuInfo] Creat brdcfg.ini failed! File isn't exist!\n");
		return FALSE;
	}

	s8    achProfileName[32];
    BOOL    bResult;
    s8    achDefStr[] = "Cannot find the section or key";
    s8    achReturn[MAX_VALUE_LEN + 1];
    s32   nValue;

	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);

    bResult = GetRegKeyString( achProfileName, SECTION_BoardSystem, KEY_MpcIpAddr, 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( FALSE == bResult)  
	{
		printf("[BoardAgent] Wrong profile while reading %s!\n", KEY_MpcIpAddr );
		m_dwMpcIpAddr = ntohl(INET_ADDR(DEFAULT_MCU_INNERIP));
	}
    else
    {
        m_dwMpcIpAddr = ntohl( INET_ADDR( achReturn ) );
		if( 0xffffffff == m_dwMpcIpAddr || 0 == m_dwMpcIpAddr )
		{
			m_dwMpcIpAddr = ntohl(INET_ADDR(DEFAULT_MCU_INNERIP));
		}
    }

	bResult = GetRegKeyInt( achProfileName, SECTION_BoardSystem, KEY_MpcPort, 0, &nValue );
	if( FALSE == bResult)  
	{
		printf("[BoardAgent] Wrong profile while reading %s!\n", KEY_MpcPort );
		nValue = MCU_LISTEN_PORT;
	}
	m_wMpcPort = ( u16 )nValue;

/*
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

    if(m_dwMpcIpAddr == m_dwMpcIpAddrB || 0xffffffff == m_dwMpcIpAddrB)
    {
        m_dwMpcIpAddrB = 0;
        m_wMpcPortB = 0;
    }

    bResult = GetRegKeyInt( achProfileName, "IsFront", "Flag", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, TRUE, "[Agent] Wrong profile while reading %s!\n", "IsFront" );
        m_byChoice = 0;
	}
    
    m_byChoice = nValue;
*/
    m_byChoice = 0;

	TBrdPosition tBrdPos;
	if ( OK == BrdQueryPosition( &tBrdPos ) )
	{
		m_byBrdId = ( tBrdPos.byBrdLayer<<4 ) | ( tBrdPos.byBrdSlot+1 );
	}
//#ifdef WIN32
//	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "brdcfgdebug.ini");
//	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Layer", 0, &nValue );
//	if( FALSE == bResult)  
//	{
//		printf("[BoardAgent] Wrong profile while reading %s!\n", "Layer" );
//		return( FALSE );
//	}
//	u8 byBrdLayer = (u8)nValue;
//
//	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Slot", 0, &nValue );
//	if( FALSE == bResult)  
//	{
//		printf("[BoardAgent] Wrong profile while reading %s!\n", "Slot" );
//		return( FALSE );
//	}
//	m_byBrdId = (byBrdLayer<<4) | ((u8)nValue+1);
//#else

//	m_byBrdId = 1; // 8000b only be 1

//#endif

    if(0 == m_dwMpcIpAddr)
    {
        printf("[BoardAgent] The Mpc's Ip is 0.\n");
        return FALSE;
    }

    //��ȡosp����������
    GetRegKeyInt( achProfileName, SECTION_BoardSystem, KEY_HeartBeatTime, DEF_OSPDISC_HBTIME, &nValue );
    if (nValue >= 3)
    {
        m_wDiscHeartBeatTime = (u16)nValue;
    }
    
    GetRegKeyInt( achProfileName, SECTION_BoardSystem, KEY_HeartBeatNum, DEF_OSPDISC_HBNUM, &nValue );
    if (nValue >= 1)
    {
        m_byDiscHeartBeatNum = (u8)nValue;
    }    

    // guzh [1/12/2007] ����ȡģ����Ϣ
    ReadModuleInfoFromBinCfg(m_tLocalModuleCfg);
    
	return( TRUE );
}

/*====================================================================
���ܣ�д��������Ϣ�ļ�
��������
����ֵ��TRUE/FALSE  �Ƿ���д��
====================================================================*/
BOOL32 CDscConfig::WriteConnectMcuInfo()
{
	if ( !CreatBrdCfgFile() )
	{
		OspPrintf(TRUE, FALSE, "[WriteConnectMcuInfo] Creat brdcfg.ini failed! File isn't exist!\n");
		return FALSE;
	}

    s8    achProfileName[32] = {0};  
     sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);

     u32 dwIp = m_tRunningModuleCfg.GetMcuInnerIp(); 

     if ( dwIp != m_dwMpcIpAddr ||
          m_wMpcPort != m_tRunningModuleCfg.GetMcuPort() )
     {
         SetRegKeyString(achProfileName, "BoardSystem", "MpcIpAddr", StrOfIP(dwIp));         
         SetRegKeyInt(achProfileName, "BoardSystem", "MpcPort", m_tRunningModuleCfg.GetMcuPort() );
         
         return TRUE;
     }
     else
     {
         return FALSE;
     }
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
BOOL CDscConfig::SetPrsConfig( TEqpPrsEntry *ptPrsCfg )
{
	if( ptPrsCfg == NULL )
		return FALSE;
	
	m_bIsRunPrs = TRUE;
	
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

u16  CDscConfig::GetMpcBPort(void)
{
    return m_wMpcPortB;
}
u16  CDscConfig::SetMpcBPort(u16 wPort)
{
    m_wMpcPortB = wPort;
    return 0;
}

u32  CDscConfig::GetMpcBIp(void)
{
    return htonl(m_dwMpcIpAddrB);
}

u16  CDscConfig::SetMpcBIp(u32 dwIp)
{
    m_dwMpcIpAddrB = ntohl(dwIp);
    return 0;
}


/*====================================================================
    ������      ��SetDscRunningModule
    ����        ���������е�ģ����Ϣ������ͱ��������ļ���һ�£�����±����ļ�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��TRUE/FALSE        �Ƿ�����д��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/01/12    4.0         ����        ����
====================================================================*/
BOOL32 CDscConfig::SetDscRunningModule(const TDSCModuleInfo &tInfo, BOOL32 bCompareModule)
{
    memcpy( &m_tRunningModuleCfg, &tInfo, sizeof(TDSCModuleInfo) );

	/*
	printf("\n");
	printf("Local module cfg:\n");
	m_tLocalModuleCfg.cosPrint();
	printf("New module cfg:\n");
	m_tRunningModuleCfg.cosPrint();
	printf("\n");
	*/
	// �Ƚϱ�����Ϣ��ֻ���� module.cfg ��ָ����3��
	BOOL32 bDiff = ( m_tRunningModuleCfg.IsStartGk() != m_tLocalModuleCfg.IsStartGk() );
	bDiff |= ( m_tRunningModuleCfg.IsStartProxy() != m_tLocalModuleCfg.IsStartProxy() );
	bDiff |= ( m_tRunningModuleCfg.IsStartDcs() != m_tLocalModuleCfg.IsStartDcs() );

	if (bDiff)
	{
		printf("Get Config Difference from local, saving...\n");
		OspPrintf(TRUE, FALSE, "Get Config Difference from local, saving...\n");
		// ��ͬ��д���ļ���Ȼ������
		SaveModuleInfo2BinCfg( m_tRunningModuleCfg );
		
		return TRUE;
	}

	// ���ӱȽ�����, zgc, 2007-08-14
	if ( bCompareModule )
    {
		bDiff |= ( m_tRunningModuleCfg.IsStartMp() != m_tLastRunningModuleCfg.IsStartMp() );
		bDiff |= ( m_tRunningModuleCfg.IsStartMtAdp() != m_tLastRunningModuleCfg.IsStartMtAdp() );
		if ( bDiff )
		{
			printf(" Mp or Mtadp is difference!\n");
			OspPrintf(TRUE, FALSE, "Mp or Mtadp is difference!\n" );
			return TRUE;
		}
	}
		
    return FALSE;
}

/*====================================================================
    ������      ��ReadModuleInfoFromBinCfg
    ����        ����ȡ�����ļ� /usr/bin/module.cfg
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��TRUE/FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/01/12    4.0         ����        ����
====================================================================*/
void CDscConfig::ReadModuleInfoFromBinCfg(TDSCModuleInfo &tInfo)
{
	if ( !CreatModCfgFile() )
	{
		OspPrintf(TRUE,FALSE, "[ReadModuleInfoFromBinCfg] Creat modcfg.ini failed! File isn't exist!\n");
		return;
	}

    memset(&tInfo, 0, sizeof(tInfo));
/*
    autorun.inf ��������

    [apps]
    app = /usr/bin/watchdog;
    type = 0;
    
    app = /usr/bin/kdvgk;
    type = 0;
    
    app = /usr/bin/sysconfig.sh;
    type = 0;
    
    app = /usr/bin/pxyserverapp;
    type = 0;
    
    app = /usr/bin/mp8000b;
    type = 0;
    
    app = /usr/bin/dcserver_r;
    type = 0;
*/
#ifdef _LINUX_
    FILE *fp = fopen( FILENAME_MODULECFG, "r" );
    if (fp == NULL)
    {
        // ��ȡʧ��
        return;
    }

    // �����ļ�
   s8 szLine[256];
   s8 szTmp[256];
   const s8 strSep[] = " =";
   s8 * pchPos ;
   
   while ( fgets( szLine, 255, fp ) != NULL )
   {
        strcpy(szTmp, szLine);
        pchPos = strtok( szTmp, strSep );
        // ����
        if (pchPos == NULL)
            continue;

        // �����app
        if ( strstr( pchPos, "app" ) )
        {
            // ���ĺ��������
            s8* pValue = strchr( szLine, '=' );
            if (pValue == NULL)
                continue;

            pValue ++;

            if ( strstr(pValue, "kdvgk") )
            {
                tInfo.SetStartGk(TRUE);
            }
            else if ( strstr(pValue, "pxyserverapp") )
            {
                tInfo.SetStartProxy(TRUE);
            }
            else if ( strstr(pValue, "dcserver_r") )
            {
                tInfo.SetStartDcs(TRUE);
            }
        }
   }

    fclose(fp);
#endif    
    OspPrintf(TRUE, FALSE, "[ReadModuleInfoFromBinCfg] Get Local Config: \n");
    tInfo.Print();
}

/*====================================================================
    ������      ��SaveModuleInfo2BinCfg
    ����        ��������±����ļ� /usr/bin/module.cfg
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��TRUE/FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/01/12    4.0         ����        ����
====================================================================*/
void CDscConfig::SaveModuleInfo2BinCfg(const TDSCModuleInfo &tInfo)
{
/*
    autorun.inf ��������

    [apps]
    app = /usr/bin/watchdog;
    type = 0;
    
    app = /usr/bin/kdvgk;
    type = 0;
    
    app = /usr/bin/sysconfig.sh;
    type = 0;
    
    app = /usr/bin/pxyserverapp;
    type = 0;
    
    app = /usr/bin/mp8000b;
    type = 0;
    
    app = /usr/bin/dcserver_r;
    type = 0;
*/
#ifdef _LINUX_
    // ��д����ʱ�ļ�
    s8* szTmpFilename = "/ramdisk/tmp.inf";

    FILE *fp = fopen(szTmpFilename, "w");
    if (fp == NULL)
    {
        return;
    }

    s8 abyBuf[256] = {0};
    strcpy( abyBuf, "[apps]\n\n" );    
    fwrite( abyBuf, 1, strlen(abyBuf), fp );

    // 1. watchdog ��Ȼ����
    strcpy( abyBuf, "app = /usr/bin/watchdog;\ntype = 0;\n\n" );
    fwrite( abyBuf, 1, strlen(abyBuf), fp );

	// �������Ƶ��������GK��PROXY��DCS����, zgc, 2007-10-11
	// 4.DSCAgent ��Ȼ����
    strcpy( abyBuf, "app = /usr/bin/mp8000b;\ntype = 0;\n\n" );
    fwrite( abyBuf, 1, strlen(abyBuf), fp );

    // 2.GK
    if ( tInfo.IsStartGk() )
    {
        strcpy( abyBuf, "app = /usr/bin/kdvgk;\ntype = 0;\n\n" );
        fwrite( abyBuf, 1, strlen(abyBuf), fp );
    }

    // 3.PROXY
    if ( tInfo.IsStartProxy() )
    {
        strcpy( abyBuf, "app = /usr/bin/sysconfig.sh;\ntype = 0;\n\n" );
        fwrite( abyBuf, 1, strlen(abyBuf), fp );
        strcpy( abyBuf, "app = /usr/bin/pxyserverapp;\ntype = 0;\n\n" );
        fwrite( abyBuf, 1, strlen(abyBuf), fp );
    }

    // 5.DCS
    if ( tInfo.IsStartDcs() )
    {
        strcpy( abyBuf, "app = /usr/bin/dcserver_r;\ntype = 0;\n\n" );
        fwrite( abyBuf, 1, strlen(abyBuf), fp );
    }

    fclose(fp);
#endif
#ifdef _LINUX_
    // ���� Appupdate
    BrdUpdateAppFile (szTmpFilename, FILENAME_MODULECFG); 
#endif
}

/*=============================================================================
�� �� ���� ReadDefaultGWFromCfg
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  TMINIMCUNetParamAll tNetParamAll
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/18   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CDscConfig::ReadDefaultGWFromCfg( TMINIMCUNetParamAll &tNetParamAll )
{
	memset( &tNetParamAll, 0, sizeof(tNetParamAll) );
	TMINIMCUNetParam tNetParam;
#ifdef _LINUX_
	FILE *stream = fopen( FILENAME_PXYSRVCFG, "rb" );
    if( stream == NULL )
    {
        return( FALSE );
    }
	//debug
	printf("MoveToSectionStart will RUN\n");
	//debug end
	s32 nSecStartOffset = MoveToSectionStart( stream, SECTION_defaultGW, FALSE );
	//debug
	printf("MoveToSectionStart  RUN\n");
	//debug end
	if ( nSecStartOffset == -1 )
	{
		Dsclog("[ReadDefaultGWFromCfg] Move to section %s start failed!\n", SECTION_defaultGW );
		return FALSE;
	}
	Dsclog("[ReadDefaultGWFromCfg] Sec %s start offset : %d\n", SECTION_defaultGW, nSecStartOffset );
	//debug
	printf("MoveToSectionEnd will RUN\n");
	//debug end
	s32 nSecEndOffset = MoveToSectionEnd( stream, SECTION_defaultGW, FALSE );
	//debug
	printf("MoveToSectionEnd RUN\n");
	//debug end
	if ( nSecEndOffset == -1 )
	{
		Dsclog("[ReadDefaultGWFromCfg] Move to section %s end failed!\n", SECTION_defaultGW );
		return FALSE;
	}
	Dsclog("[ReadDefaultGWFromCfg] Sec %s end offset : %d\n", SECTION_defaultGW, nSecStartOffset );
	fseek( stream, nSecStartOffset, SEEK_SET );
	u8 byIdx = 0;
	s8 achRow[MAX_LINE_LEN + 1];
	s8 achRowKeyName[MAX_KEYNAME_LEN + 1];
	s8 achRowKeyValue[MAX_VALUE_LEN + 1];
	s8 *lpszEqualPos = NULL;
	u32 dwIpAdr = 0;
	u32 dwIpGw = 0;
	while( ftell(stream) < nSecEndOffset && byIdx < ETH_IP_MAXNUM )
	{
		//debug
		printf("KEY%d will RUN\n", byIdx);
		//debug end
		memset(&tNetParam, 0, sizeof(tNetParam));
		memset(achRow, 0, sizeof(achRow) );
		memset(achRowKeyName, 0, sizeof(achRowKeyName) );
		memset(achRowKeyValue, 0, sizeof(achRowKeyValue) );
		fgets( achRow, MAX_LINE_LEN, stream );

		/* if arrive at a comment or cannot find a '=', continue */
        if( strstr( achRow, PROFILE_COMMENT1 ) == achRow ||
            strstr( achRow, PROFILE_COMMENT2 ) == achRow ||
            strstr( achRow, PROFILE_COMMENT3 ) == achRow ||
            strstr( achRow, PROFILE_COMMENT4 ) == achRow ||
            ( lpszEqualPos = strchr( achRow, '=' ) ) == NULL )
        {
            continue;
        }

		memcpy( achRowKeyName, achRow, lpszEqualPos - achRow );
        achRowKeyName[lpszEqualPos - achRow] = '\0';
        Trim( achRowKeyName );
		Trim( lpszEqualPos + 1 );
		strncpy( achRowKeyValue, lpszEqualPos + 1, MAX_VALUE_LEN );
        achRowKeyValue[MAX_VALUE_LEN] = '\0';
		dwIpAdr = ntohl(INET_ADDR( achRowKeyName ));
		dwIpGw = ntohl(INET_ADDR( achRowKeyValue ));
		tNetParam.SetLanParam(dwIpAdr, 0, dwIpGw);
		printf("[ReadDefaultGWFromCfg] GW %d : ip = 0x%x, IpGW = 0x%x\n", byIdx, dwIpAdr, dwIpGw );
		tNetParamAll.AddOneNetParam(tNetParam);
		//debug
		printf("KEY%d RUN\n", byIdx);
		printf("ftell(stream)=%d, nSecEndOffset=%d\n", ftell(stream), nSecEndOffset);
		//debug end
		byIdx++;
	}
	//debug
	printf("fclose(stream) will RUN\n", byIdx);
	//debug end
	fclose(stream);
	//debug
	printf("fclose(stream) RUN\n", byIdx);
	//debug end

#endif
	return TRUE;
}

/*=============================================================================
�� �� ���� SaveDefaultGWToCfg
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const TMINIMCUNetParamAll &tNetParamAll
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/18   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CDscConfig::SaveDefaultGWToCfg( const TMINIMCUNetParamAll &tNetParamAll)
{
	if ( !tNetParamAll.IsValid() )
	{
		OspPrintf(TRUE, FALSE, "[SaveDefaultGWToCfg] the param is invalid!\n");
		printf("[SaveDefaultGWToCfg] the param is invalid!\n");
		return FALSE;
	}

	if ( !CreatPxySrvCfgFile() )
	{
		OspPrintf(TRUE, FALSE, "[SaveDefaultGWToCfg] The pxy config file isn't exist and can't be created!\n");
		return FALSE;
	}

#ifdef _LINUX_
	TMINIMCUNetParamAll tCfgParamAll;
//	ReadDefaultGWFromCfg( tCfgParamAll );
//	if ( !tCfgParamAll.IsEqualTo( tNetParamAll ) )
	{
		DelDefaultGWInCfg();
	}

	u32 dwIpAddr = 0;
	u32 dwGatewayIp = 0;
	u8 byLop = 0;
	s8 asIpAddr[17];
	s8 asGWIpAddr[17];
	u8 bySuccessNum = 0;
	u32 dwPxyLocalIp = 0;
	
	TMINIMCUNetParam tNetParam;
	for ( byLop = 0; byLop < tNetParamAll.GetNetParamNum(); byLop++ )
	{
		memset( &tNetParam, 0, sizeof(tNetParam) );
		memset(asIpAddr, 0, sizeof(asIpAddr));
		memset(asGWIpAddr, 0, sizeof(asGWIpAddr));
		if ( tNetParamAll.GetNetParambyIdx( byLop, tNetParam ) )
		{
			dwIpAddr = tNetParam.GetIpAddr();
			// ��¼��IP, zgc, 2007-09-24
			if ( byLop == 0)
			{
				dwPxyLocalIp = dwIpAddr;
			}

			dwGatewayIp = tNetParam.GetGatewayIp();        
            GetIpFromU32(asIpAddr, htonl(dwIpAddr));
            GetIpFromU32(asGWIpAddr, htonl(dwGatewayIp));
			if ( !SetRegKeyString( FILENAME_PXYSRVCFG, SECTION_defaultGW, asIpAddr, asGWIpAddr ) )
			{
				OspPrintf(TRUE, FALSE, "[SaveDefaultGWToCfg] Save GW error! Ip = %s, GWIp = %s\n", asIpAddr, asGWIpAddr );
			}
			else
			{
				bySuccessNum++;
			}
		}
	}

	// �޸�[LocalInfo]��IpAddr, zgc, 2007-09-24
	s8 achIpStr[17] = {0};
	sprintf(achIpStr, "%d.%d.%d.%d", (dwPxyLocalIp>>24)&0xFF, (dwPxyLocalIp>>16)&0xFF, (dwPxyLocalIp>>8)&0xFF, dwPxyLocalIp&0xFF);
	BOOL32 bResult = SetRegKeyString( FILENAME_PXYSRVCFG, SETCION_LocalInfo, KEY_IpAddr, achIpStr );
	if ( !bResult )
	{
		Dsclog( "[SaveDefaultGWToCfg] Write Section[%s]Key[%s] failed!\n", SETCION_LocalInfo, KEY_IpAddr );
	}

	return ( bySuccessNum == tNetParamAll.GetNetParamNum() ) ? TRUE : FALSE; 
#endif
	return TRUE;
}

/*=============================================================================
�� �� ���� DelDefaultGWInCfg
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/18   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CDscConfig::DelDefaultGWInCfg( void )
{
#ifdef _LINUX_
	FILE *stream = fopen( FILENAME_PXYSRVCFG, "rb+" );
    if( stream == NULL )
    {
        return( FALSE );
    }
	s32 nOffset = DelSection( stream, SECTION_defaultGW );
	fclose( stream );
	if ( nOffset == -1 )
	{
		return FALSE;
	}
	else
	{
		TrimCfgFile( FILENAME_PXYSRVCFG );
		return TRUE;
	}
/*
	u32 dwIpAddr = 0;
	u8 byLop = 0;
	s8 asIpAddr[17];
	u8 bySuccessNum = 0;

	TMINIMCUNetParam tNetParam;
	for ( byLop = 0; byLop < tNetParamAll.GetNetParamNum(); byLop++ )
	{
		memset( &tNetParam, 0, sizeof(tNetParam) );
		memset(asIpAddr, 0, sizeof(asIpAddr));
		if ( tNetParamAll.GetNetParambyIdx( byLop, tNetParam ) )
		{
			dwIpAddr = tNetParam.GetIpAddr();
            GetIpFromU32(asIpAddr, dwIpAddr);
			if ( !DeleteRegKey(FILENAME_PXYSRVCFG, SECTION_defaultGW, asIpAddr) )
			{
				OspPrintf(TRUE, FALSE, "[DelDefaultGWInCfg] Del GW error! Ip = %s\n", asIpAddr );
			}
			else
			{
				bySuccessNum++;
			}
		}
	}
	return ( bySuccessNum == tNetParamAll.GetNetParamNum() ) ? TRUE : FALSE; 
*/
#endif
	return TRUE;
}

/*=============================================================================
�� �� ���� TableMemoryFree
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void **ppMem
           u32 dwEntryNum
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/18   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CDscConfig::TableMemoryFree( void **ppMem, u32 dwEntryNum )
{
    if( NULL == ppMem)
    {
        return FALSE;
    }
    for( u32 dwLoop = 0; dwLoop < dwEntryNum; dwLoop++ )
    {
        if( NULL != ppMem[dwLoop] )
        {
            delete [] (s8*)ppMem[dwLoop];
            ppMem[dwLoop] = NULL;
        }
    }
    delete [] (s8*)ppMem;
    ppMem = NULL;
	return TRUE;
}

/*=============================================================================
�� �� ���� SaveDscLocalInfoToNip
��    �ܣ� ��DSC info�е�CallAddr���õ�DSC����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TDSCModuleInfo * ptDscInfo
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/19   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CDscConfig::SaveDscLocalInfoToNip( TDSCModuleInfo * ptDscInfo )
{
	TDSCModuleInfo tTempDscInfo;
	memcpy( &tTempDscInfo, ptDscInfo, sizeof(TDSCModuleInfo) );

	u8 byLop = 0;
	u8 byCallAddrNum = tTempDscInfo.GetCallAddrNum();
	TMINIMCUNetParam tNetParam;
	TMINIMCUNetParamAll tNetParamAll;
	u8 byIpNum = 0;
//#ifdef _LINUX_
#ifndef _VXWORKS_
	TBrdEthParamAll tBrdEthParamAll;
	STATUS nRet = BrdGetEthParamAll( 0, &tBrdEthParamAll );
	if ( ERROR == nRet )
	{
		printf( "[SaveDscLocalInfoToNip] BrdGetEthParamAll() failed!\n");
		return FALSE;
	}
	byIpNum = tBrdEthParamAll.dwIpNum;
	u8 byBrdType = BrdGetBoardID();

	if ( byIpNum > 1 )
	{
		// ɾ�� �����ڿ�IP��������и�IP, zgc, 2007-07-17
		for ( byLop = byIpNum-1; byLop > 0; byLop-- )
		{
			if ( 
				 ( byBrdType != BRD_TYPE_HDSC/*DSL8000_BRD_HDSC*/ && tTempDscInfo.GetDscInnerIp() != ntohl(tBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs) )
				 || byBrdType == BRD_TYPE_HDSC/*DSL8000_BRD_HDSC*/ 
			   )
			{
				nRet = BrdSetSecondEthParam(0, Brd_DEL_SEC_IP_AND_MASK, &tBrdEthParamAll.atBrdEthParam[byLop]);
				if ( ERROR == nRet )
				{
					printf( "[SaveDscLocalInfoToNip] Del sec ip.0x%x, mask.0x%x failed, wRet.%d \n", 
							  ntohl(tBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs), 
							  ntohl(tBrdEthParamAll.atBrdEthParam[byLop].dwIpMask), 
							  nRet );
					return FALSE;
				}
			}
		}
	}
	// ���õ�һ��IP
	
	tTempDscInfo.GetCallAddrAll( tNetParamAll );
	tNetParamAll.GetNetParambyIdx( 0, tNetParam );
	TBrdEthParam tOneParam;
	tOneParam.dwIpAdrs = htonl( tNetParam.GetIpAddr() );
	tOneParam.dwIpMask = htonl( tNetParam.GetIpMask() );
	nRet = BrdSetEthParam(0, Brd_SET_IP_AND_MASK, &tOneParam);
	if ( ERROR == nRet )
	{
		printf( "[SaveDscLocalInfoToNip] BrdSetEthParam() failed, ip.0x%x, mask.0x%x, wRet.%d \n", 
					   tNetParam.GetIpAddr(), tNetParam.GetIpMask(), nRet );
		return FALSE;
	}
	// ���ø�IP
	for ( byLop = 1; byLop < byCallAddrNum; byLop++ )
	{
		tNetParamAll.GetNetParambyIdx(byLop, tNetParam);
		tOneParam.dwIpAdrs = htonl( tNetParam.GetIpAddr() );
		tOneParam.dwIpMask = htonl( tNetParam.GetIpMask() );
		nRet = BrdSetSecondEthParam(0, Brd_SET_SEC_IP_AND_MASK, &tOneParam);
		if ( ERROR == nRet )
		{
			printf( "[SaveDscLocalInfoToNip] Set sec ip.0x%x, mask.0x%x failed, wRet.%d \n", 
					   tNetParam.GetIpAddr(), tNetParam.GetIpMask(), nRet );
			return FALSE;
		}
	}
	BrdSaveNipConfig();

	if ( BRD_TYPE_MDSC/*DSL8000_BRD_MDSC*/ == byBrdType || BRD_TYPE_DSC/*KDV8000B_MODULE*/ == byBrdType )
	{
		// ���������õ�IP���м��飬��ֹ�ڿ�IP�����󸲸ǻ�ɾ��, zgc, 2007-07-17
		nRet = BrdGetEthParamAll( 0, &tBrdEthParamAll );
		if ( ERROR == nRet )
		{
			printf( "[SaveDscLocalInfoToNip] BrdGetEthParamAll() failed!\n");
			return FALSE;
		}
		byIpNum = tBrdEthParamAll.dwIpNum;
		for ( byLop = 0; byLop < byIpNum; byLop++ )
		{
			if ( tTempDscInfo.GetDscInnerIp() == ntohl(tBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs) )
				break;
		}
		if ( byLop == byIpNum )
		{
			tOneParam.dwIpAdrs = htonl( tTempDscInfo.GetDscInnerIp() );
			tOneParam.dwIpMask = htonl( tTempDscInfo.GetInnerIpMask() );
			nRet = BrdSetSecondEthParam(0, Brd_SET_SEC_IP_AND_MASK, &tOneParam);
			if ( ERROR == nRet )
			{
				printf( "[SaveDscLocalInfoToNip] Set sec ip.0x%x, mask.0x%x failed, wRet.%d \n", 
						   tTempDscInfo.GetDscInnerIp(), tTempDscInfo.GetInnerIpMask(), nRet );
				return FALSE;
			}
		}
	}
	
	BrdSaveNipConfig();
//#endif
//#ifdef _VXWORKS_
#else
	tTempDscInfo.GetCallAddrAll( tNetParamAll );
	tNetParamAll.GetNetParambyIdx(0, tNetParam);
	TBrdEthParam tOneParam;
	tOneParam.dwIpAdrs = htonl( tNetParam.GetIpAddr() );
	tOneParam.dwIpMask = htonl( tNetParam.GetIpMask() );
	STATUS nRet = BrdSetEthParam(m_byChoice, Brd_SET_IP_AND_MASK, &tOneParam);
	if ( ERROR == nRet )
	{
		OspPrintf(TRUE, FALSE, "[SaveDscLocalInfoToNip] BrdSetEthParam() failed, ip.0x%x, mask.0x%x, wRet.%d \n", 
					   tNetParam.GetIpAddr(), tNetParam.GetIpMask(), nRet );
		return FALSE;
	}
	BrdSaveNipConfig();
#endif

//#ifndef WIN32
	u8 byLoop = 0;
	
	// ��ɾ���������е�·��
	// ɾ��Ĭ��·��
    /*
	if ( 0 != BrdGetDefGateway() )
	{
		nRet = BrdDelDefGateway();
		if ( ERROR == nRet )
		{
			OspPrintf(TRUE, FALSE, "[SaveDscLocalInfoToNip] Del DefGateway error!\n");
		}
	}
	BrdSaveNipConfig();
	*/
    //·�ɲ���ǿ��ɾ��������������
    /*
	TBrdAllIpRouteInfo tBrdAllIpRouteInfo;
	nRet = BrdGetAllIpRoute( &tBrdAllIpRouteInfo );
	if ( ERROR == nRet )
	{
		OspPrintf(TRUE, FALSE, "[SaveDscLocalInfoToNip] Get Ip routes error!\n" );
	}
	u8 byIpRouteNum = (u8)tBrdAllIpRouteInfo.dwIpRouteNum;
	for ( byLoop = 0; byLoop < byIpRouteNum ; byLoop++ )
	{
		nRet = BrdDelOneIpRoute( &tBrdAllIpRouteInfo.tBrdIpRouteParam[byLoop] );
		if ( ERROR == nRet )
		{
			OspPrintf(TRUE, FALSE, "[SaveDscLocalInfoToNip] Del Ip routes error! DesIpNet.0x%x, DesIpMask.0x%x, GwIp.0x%x\n",
				tBrdAllIpRouteInfo.tBrdIpRouteParam[byLoop].dwDesIpNet,
				tBrdAllIpRouteInfo.tBrdIpRouteParam[byLoop].dwDesIpMask,
				tBrdAllIpRouteInfo.tBrdIpRouteParam[byLoop].dwGwIpAdrs);
		}
	}*/

	// �����ڲ�·��
	TBrdIpRouteParam tBrdIpRouteParam;
	byIpNum = tTempDscInfo.GetCallAddrNum();
	BOOL32 bFirstWan = TRUE;
	tTempDscInfo.GetCallAddrAll( tNetParamAll );
	for ( byLoop = 0; byLoop < byIpNum; byLoop++ )
	{
		memset( &tBrdIpRouteParam, 0, sizeof(TBrdIpRouteParam) );
		memset( &tNetParam, 0, sizeof(tNetParam) );
		tNetParamAll.GetNetParambyIdx(byLoop, tNetParam);
		if ( tNetParam.IsLan() )
		{
			if ( NETTYPE_LAN == tTempDscInfo.GetNetType() )
			{
				//nRet = BrdSetDefGateway( htonl(tNetParam.GetGatewayIp()) );
                TBrdIpRouteParam tRouteParam;
                tRouteParam.dwDesIpMask = 0xffffffff;
                tRouteParam.dwGwIpAdrs = htonl(tNetParam.GetGatewayIp());
                tRouteParam.dwDesIpNet = htonl(tNetParam.GetGatewayIp());

                nRet = BrdAddOneIpRoute(&tRouteParam);
				if ( ERROR == nRet )
				{
					OspPrintf(TRUE, FALSE, "[SaveDscLocalInfoToNip] Add Ip routes error! GwIp.0x%x\n", 
							tNetParam.GetGatewayIp());
				}
			}
			else
			{
				OspPrintf(TRUE, FALSE, "[SaveDscLocalInfoToNip] NetParam.%d NetType: %d, DscInfo NetType: %d, not accroding!\n ",byLoop, tNetParam.GetNetworkType(), tTempDscInfo.GetNetType() );
			}
		}
		if ( tNetParam.IsWan() && TRUE == bFirstWan )
		{			
			//nRet = BrdSetDefGateway( htonl(tNetParam.GetGatewayIp()) );
            TBrdIpRouteParam tRouteParam;
            tRouteParam.dwDesIpMask = 0xffffffff;
            tRouteParam.dwGwIpAdrs = htonl(tNetParam.GetGatewayIp());
            tRouteParam.dwDesIpNet = htonl(tNetParam.GetGatewayIp());
            
            nRet = BrdAddOneIpRoute(&tRouteParam);
			if ( ERROR == nRet )
			{
				OspPrintf(TRUE, FALSE, "[SaveDscLocalInfoToNip] Add Ip routes error! DesIpNet.0x%x, DesIpMask.0x%x, GwIp.0x%x\n", 
						tBrdIpRouteParam.dwDesIpNet,
						tBrdIpRouteParam.dwDesIpMask,
						tBrdIpRouteParam.dwGwIpAdrs);
			}
			bFirstWan = FALSE;
		}
	}
	BrdSaveNipConfig();
//#endif
	return TRUE;
}

/*=============================================================================
�� �� ���� SaveGKInfoToCfg
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  u32 dwGKIp ��������
           u16 wGKPort
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/21   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CDscConfig::SaveGKInfoToCfg( u32 dwGKIp, u16 wGKPort )
{
	if ( !CreatPxySrvCfgFile() )
	{
		OspPrintf(TRUE, FALSE, "[SaveGKInfoToCfg] The pxy config file isn't exist and can't be created!\n");
		return FALSE;
	}

	s8 asGKIpStr[32];
	memset( asGKIpStr, 0, sizeof(asGKIpStr) );
    GetIpFromU32(asGKIpStr, htonl(dwGKIp) );

	printf( "[SaveGKInfoToCfg] GKIp = %s, Port = %d\n", asGKIpStr, wGKPort);
	OspPrintf(TRUE, FALSE, "[SaveGKInfoToCfg] GKIp = %s, Port = %d\n", asGKIpStr, wGKPort);
#ifdef _LINUX_
	if ( !SetRegKeyString( FILENAME_PXYSRVCFG, SECTION_GKInfo, KEY_GKIpAddr, asGKIpStr ) )
	{
		printf( "[SaveGKInfoToCfg] Error! GKIp = %s\n", asGKIpStr);
		OspPrintf(TRUE, FALSE, "[SaveGKInfoToCfg] Error! GKIp = %s\n", asGKIpStr);
		return FALSE;
	}
	// RAS port��ʱ������
    /*
	if ( !SetRegKeyInt( FILENAME_PXYSRVCFG, SECTION_GKInfo, KEY_GKRasPort, wGKPort ) )
	{
		printf("[SaveGKInfoToCfg] Error! GKPort = %\n", wGKPort);
		OspPrintf(TRUE, FALSE, "[SaveGKInfoToCfg] Error! GKPort = %\n", wGKPort);
		return FALSE;
	}
	*/
#endif
    return TRUE;
}

/*=============================================================================
�� �� ���� PreTreatDscNIP
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/30   4.0			�ܹ��                  ����
=============================================================================*/
void CDscConfig::PreTreatDscNIP(void)
{
#ifndef _VXWORKS_
	STATUS nRet = OK;
	u8 byBrdType = BrdGetBoardID();
	TBrdEthParam tEthParam;
	memset( &tEthParam, 0, sizeof(&tEthParam) );
	if ( BRD_TYPE_MDSC/*DSL8000_BRD_MDSC*/ == byBrdType || BRD_TYPE_DSC/*KDV8000B_MODULE*/ == byBrdType )
	{
		BrdGetEthParam( 0, &tEthParam );
		printf("[PreTreatDscNIP] Get eth0, ip = 0x%x, ipmask = 0x%x\n", tEthParam.dwIpAdrs, tEthParam.dwIpMask );
		if ( 0 == tEthParam.dwIpAdrs )
		{
			memset( &tEthParam, 0, sizeof(tEthParam) );
			tEthParam.dwIpAdrs = INET_ADDR(DEFAULT_DSC_IP);
			tEthParam.dwIpMask = INET_ADDR(DEFAULT_IPMASK);
			nRet = BrdSetEthParam( 0, Brd_SET_IP_AND_MASK, &tEthParam );
			if ( ERROR == nRet )
			{
				printf("[PreTreatDscNIP]Set eth0 failed! ip = 0x%x, ipmask = 0x%x!\n", tEthParam.dwIpAdrs, tEthParam.dwIpMask);
			}

			memset( &tEthParam, 0, sizeof(tEthParam) );
			tEthParam.dwIpAdrs = INET_ADDR(DEFAULT_DSC_INNERIP);
			tEthParam.dwIpMask = INET_ADDR(DEFAULT_INNERIPMASK);
			nRet = BrdSetSecondEthParam( 0, Brd_SET_SEC_IP_AND_MASK, &tEthParam );
			if ( ERROR == nRet )
			{
				printf("[PreTreatDscNIP]Set eth0 sec failed! ip = 0x%x, ipmask = 0x%x!\n", tEthParam.dwIpAdrs, tEthParam.dwIpMask);
			}
		}
	}
	else
	{
		BrdGetEthParam( 0, &tEthParam );
		printf("[PreTreatDscNIP] Get eth0, ip = 0x%x, ipmask = 0x%x\n", tEthParam.dwIpAdrs, tEthParam.dwIpMask );
		if ( 0 == tEthParam.dwIpAdrs )
		{
			memset( &tEthParam, 0, sizeof(tEthParam) );
			tEthParam.dwIpAdrs = INET_ADDR(DEFAULT_DSC_IP);
			tEthParam.dwIpMask = INET_ADDR(DEFAULT_IPMASK);
			nRet = BrdSetEthParam( 0, Brd_SET_IP_AND_MASK, &tEthParam );
			if ( ERROR == nRet )
			{
				printf("[PreTreatDscNIP]Set eth0 failed! ip = 0x%x, ipmask = 0x%x!\n", tEthParam.dwIpAdrs, tEthParam.dwIpMask);
			}
		}
		memset( &tEthParam, 0, sizeof(tEthParam) );
		BrdGetEthParam( 1, &tEthParam );
		printf("[PreTreatDscNIP] Get eth1, ip = 0x%x, ipmask = 0x%x\n", tEthParam.dwIpAdrs, tEthParam.dwIpMask );
		if ( 0 == tEthParam.dwIpAdrs )
		{
			memset( &tEthParam, 0, sizeof(tEthParam) );
			tEthParam.dwIpAdrs = INET_ADDR(DEFAULT_DSC_INNERIP);
			tEthParam.dwIpMask = INET_ADDR(DEFAULT_INNERIPMASK);
			nRet = BrdSetEthParam( 1, Brd_SET_IP_AND_MASK, &tEthParam );
			if ( ERROR == nRet )
			{
				printf("[PreTreatDscNIP]Set eth1 failed! ip = 0x%x, ipmask = 0x%x!\n", tEthParam.dwIpAdrs, tEthParam.dwIpMask);
			}
		}
	}
#endif
	return;
}

/*=============================================================================
�� �� ���� MoveToSectionStart
��    �ܣ� ���α��Ƶ�ָ���Ķ�����һ�п�ʼ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� FILE *stream, Profile����   
           const s8* lpszSectionName, Profile�еĶ���
           BOOL32 bCreate, ���û�иö����Ƿ񴴽�
�� �� ֵ�� ����ƫ����Offset�����ʧ�ܷ���-1
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/31   4.0			�ܹ��                  ����
=============================================================================*/
s32	CDscConfig::MoveToSectionStart( FILE *stream, const s8* lpszSectionName, BOOL32 bCreate )
{
	char    achRow[MAX_LINE_LEN + 1];
    char    achSectionCopy[MAX_SECTIONNAME_LEN + 1];
    u32  dwLen;
    s8*   lpszReturn;

    ASSERT( strlen( lpszSectionName ) <= MAX_SECTIONNAME_LEN );

    /* move from the beginning */
    if ( fseek( stream, 0, SEEK_SET ) != 0 )
    {
        return( -1 );
    }

    /* copy the section name and change it */
    strncpy( achSectionCopy, lpszSectionName, MAX_SECTIONNAME_LEN );
    achSectionCopy[MAX_SECTIONNAME_LEN] = '\0';
    Trim( achSectionCopy );
    StrUpper( achSectionCopy );

    do
    {
        /* if error or arrive at the end of file */
		memset( achRow, 0, sizeof(achRow) );
        if( fgets( achRow, MAX_LINE_LEN, stream ) == NULL )
        {
            if( bCreate && feof( stream ) )    /* create the section */
            {
                fputs( (const s8*)STR_RETURN, stream );
                fputs( (const s8*)"[", stream );
                fputs( (const s8*)lpszSectionName, stream );
                fputs( (const s8*)"]", stream );
                fputs( (const s8*)STR_RETURN, stream );
                fseek( stream, 0, SEEK_END );
                return( ftell( stream ) );
            }
            else
            {
                return( -1 );
            }
        }

        /* eliminate the return key */
        if( ( lpszReturn = strstr( achRow, "\r\n" ) ) != NULL )
        {
            lpszReturn[0] = '\0';
        }
        else if( ( lpszReturn = strstr( achRow, "\n" ) ) != NULL )
        {
            lpszReturn[0] = '\0';
        }
        Trim( achRow );
        dwLen = strlen( achRow );
        
        /* must be a section */
        if( dwLen <= 2 || achRow[0] != '[' || achRow[dwLen - 1] != ']' )
        {
            continue;
        }
        
        /* verify the section name */
        strncpy( achRow, achRow + 1, dwLen - 2 );
        achRow[dwLen - 2] = '\0';
        Trim( achRow );
        StrUpper( achRow );
        if( strcmp( achRow, achSectionCopy ) == 0 )    /* identical */
        {
            return( ftell( stream ) );
        }
    } while( TRUE );
}

/*=============================================================================
�� �� ���� MoveToSectionEnd
��    �ܣ� ���α��Ƶ�ָ���Ķ�������һ�ο�ʼ�������ļ���β
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� FILE *stream, Profile����   
           const s8* lpszSectionName, Profile�еĶ���
           BOOL32 bCreate, ���û�иö����Ƿ񴴽�
�� �� ֵ�� ����ƫ����Offset�����ʧ�ܷ���-1
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/31   4.0			�ܹ��                  ����
=============================================================================*/
s32	CDscConfig::MoveToSectionEnd( FILE *stream, const s8* lpszSectionName, BOOL32 bCreate )
{
	char    achRow[MAX_LINE_LEN + 1];
    u32		dwLen;
    s8*		lpszReturn;

	/* get section start offset */
	s32 nStartOffset = MoveToSectionStart( stream, lpszSectionName, bCreate );
	if ( -1 == nStartOffset )
	{
		Dsclog("[MoveToSectionEnd] Move to section %s start failed!\n", lpszSectionName );
		return nStartOffset;
	}

	/* move from the beginning */
    if ( fseek( stream, nStartOffset, SEEK_SET ) != 0 )
    {
        return( -1 );
    }

	s32 nScanOffset = nStartOffset;
	do
    {
        /* if error or arrive at the end of file */
		memset( achRow, 0, sizeof(achRow) );
        if( fgets( achRow, MAX_LINE_LEN, stream ) == NULL )
        {
            if ( feof(stream) != 0 )
			{
				/* eliminate the return key */
				if( ( lpszReturn = strstr( achRow, "\r\n" ) ) != NULL )
				{
					lpszReturn[0] = '\0';
				}
				else if( ( lpszReturn = strstr( achRow, "\n" ) ) != NULL )
				{
					lpszReturn[0] = '\0';
				}
				Trim( achRow );
				dwLen = strlen( achRow );
        
				/* must be a section */
				if( dwLen <= 2 || achRow[0] != '[' || achRow[dwLen - 1] != ']' )
				{
					return ftell(stream);
				}
				else
				{
					return nScanOffset;
				}
			}
			else
			{
				return (-1);
			}
        }

        /* eliminate the return key */
		if( ( lpszReturn = strstr( achRow, "\r\n" ) ) != NULL )
		{
			lpszReturn[0] = '\0';
		}
		else if( ( lpszReturn = strstr( achRow, "\n" ) ) != NULL )
		{
			lpszReturn[0] = '\0';
		}
		Trim( achRow );
		dwLen = strlen( achRow );

		/* must be a section */
		if( dwLen <= 2 || achRow[0] != '[' || achRow[dwLen - 1] != ']' )
		{
			nScanOffset = ftell(stream);
			continue;
		}
		else
		{
			return nScanOffset;
		}

    } while( TRUE );
}

/*=============================================================================
�� �� ���� DelSection
��    �ܣ� ɾ��ָ�������µ�����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� FILE *stream, Profile����   
           const s8* lpszSectionName, Profile�еĶ���
�� �� ֵ�� ����ƫ����Offset�����ʧ�ܷ���-1
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/31   4.0			�ܹ��                  ����
=============================================================================*/
s32	CDscConfig::DelSection( FILE *stream, const s8* lpszSectionName )
{
	if ( stream == NULL || lpszSectionName == NULL )
	{
		return -1;
	}
	fseek( stream, 0, SEEK_SET );
	s32 nSecStartOffset = MoveToSectionStart( stream, lpszSectionName, FALSE );
	if ( -1 == nSecStartOffset )
	{
		Dsclog("[DelSection] Move to section %s start failed!\n", lpszSectionName );
		return -1;
	}
	Dsclog( "[DelSection] Sec %s start offset: %d\n", lpszSectionName, nSecStartOffset );
	s32 nSetEndOffset = MoveToSectionEnd( stream, lpszSectionName, FALSE );
	if ( -1 == nSetEndOffset )
	{
		Dsclog( "[DelSection] Move to section %s end failed!\n", lpszSectionName );
		return -1;
	}
	Dsclog( "[DelSection] Sec %s end offset: %d\n", lpszSectionName, nSetEndOffset );
	u32 dwLen = nSetEndOffset - nSecStartOffset;
	if ( 0 == dwLen )
	{
		return ftell(stream);
	}

	fseek( stream, 0, SEEK_END );
	u32 dwCpyLen = ftell(stream) - nSetEndOffset;
	
	s8 *lpszCpyBuf = (s8*)malloc( dwCpyLen + dwLen );
	if ( lpszCpyBuf == NULL )
	{
		return -1;
	}
	memset( lpszCpyBuf, 0, dwCpyLen + dwLen );
	fseek( stream, nSetEndOffset, SEEK_SET );
	dwCpyLen = fread( lpszCpyBuf, sizeof( char ), dwCpyLen, stream );
	fseek( stream, nSecStartOffset, SEEK_SET );
	dwCpyLen = fwrite( lpszCpyBuf, sizeof( char ), dwCpyLen+dwLen, stream );

	free(lpszCpyBuf);
	return ftell(stream);
}


/*=============================================================================
�� �� ���� ModifyMtadpPort
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  u32 dwRasPort
           u32 dwQ931Port
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/8/2   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CDscConfig::ModifyMtadpPort( u32 dwRasPort, u32 dwQ931Port )
{
	if ( !CreatMtadpDebugFile() )
	{
		OspPrintf(TRUE, FALSE, "[ModifyMtadpPort] The mtadpdebug file isn't exist and can't be created!\n");
		return FALSE;
	}
	
	s8    achProfileName[64];
	memset(achProfileName, 0, sizeof(achProfileName));
    BOOL32  bResult = TRUE;

	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_MTADPDEBUG_INI);

    bResult = SetRegKeyInt( achProfileName, SECTION_calldata, KEY_rasport, (s32)dwRasPort);
	bResult &= SetRegKeyInt( achProfileName, SECTION_calldata, KEY_q931port, (s32)dwQ931Port);
	return bResult;
}

/*=============================================================================
�� �� ���� TrimCfgFile
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const s8* lpszCfgFilePath
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/8/3   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CDscConfig::TrimCfgFile( const s8* lpszCfgFilePath )
{
	FILE *stream = fopen( lpszCfgFilePath, "rb+" );
    if( stream == NULL )
    {
        return( FALSE );
    }
	fseek( stream, 0, SEEK_END );
	u32 dwFileLen = ftell( stream );
	s8 *lpszFileBuf = (s8*)malloc( dwFileLen+3 );
	memset( lpszFileBuf, 0 , dwFileLen );

	s8 *lpszTemp = lpszFileBuf;
	s8 *lpszReturn = NULL;
	u32 dwLen = 0;
	u32 dwTrimLen = 0;
	char achRow[MAX_LINE_LEN + 1];
	fseek( stream, 0, SEEK_SET );
	do 
	{
		memset( achRow, 0, sizeof(achRow) );
		if( fgets( achRow, MAX_LINE_LEN, stream ) == NULL )
		{
			if ( 0 == feof(stream) )
			{
				return FALSE;
			}
		}

		/* eliminate the return key */
		if( ( lpszReturn = strstr( achRow, "\r\n" ) ) != NULL )
		{
			lpszReturn[0] = '\0';
		}
		else if( ( lpszReturn = strstr( achRow, "\n" ) ) != NULL )
		{
			lpszReturn[0] = '\0';
		}
		Trim( achRow );
		dwLen = strlen( achRow );
		
		if ( dwLen > 0 )
		{
			sprintf( lpszTemp, "%s", achRow );
			lpszTemp += dwLen;
			sprintf( lpszTemp, "%s", STR_RETURN );
			lpszTemp += strlen(STR_RETURN);
		}
		
	} while( 0 == feof(stream) );

	dwTrimLen = lpszTemp - lpszFileBuf;

	fclose( stream );
	stream = fopen( lpszCfgFilePath, "w" );
	fwrite( lpszFileBuf, sizeof(s8), dwTrimLen, stream );

	free( lpszFileBuf );
	fclose( stream );
	
	return TRUE;
}

/*=============================================================================
�� �� ���� CreatPxySrvCfgFile
��    �ܣ� ����޸��ļ����򴴽����ļ�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/9/15   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CDscConfig::CreatPxySrvCfgFile()
{
	FILE *stream = fopen( FILENAME_PXYSRVCFG, "r" );
    if( stream != NULL )
    {
		fclose(stream);
        return(	TRUE );
    }

	//�ļ������ڣ��������ļ�
	s8 ch = '/';
	s8 achDirPath[64];
	memset( achDirPath, 0, sizeof(achDirPath) );
	sprintf( achDirPath, "%s", FILENAME_PXYSRVCFG );
	s8* pchDest = strrchr( achDirPath, ch );
	*pchDest = '\0';
	if ( CreatDir( achDirPath ) )
	{
		stream = fopen( FILENAME_PXYSRVCFG, "w" );
		if ( stream == NULL )
		{
			return FALSE;
		}
		fclose(stream);
	}
	else
	{
		return FALSE;
	}
	
	BOOL32 bResult = TRUE;
	// [StarInfo]
	bResult = SetRegKeyInt( FILENAME_PXYSRVCFG, SECTION_StartInfo, KEY_StartPxy, (u32)DEFVALUE_StartPxy );
	if ( !bResult )
	{
		Dsclog( "[CreatPxySrvCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_StartInfo, KEY_StartPxy );
	}

	bResult = SetRegKeyInt(  FILENAME_PXYSRVCFG, SECTION_StartInfo, KEY_StartMp, (u32)DEFVALUE_StartMp );
	if ( !bResult )
	{
		Dsclog( "[CreatPxySrvCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_StartInfo, KEY_StartMp );
	}

	// [LocalInfo]
	s8 achIpStr[17] = {0};
	u32 dwIpAddr = ntohl(GetBrdIpAddr());
	sprintf(achIpStr, "%d.%d.%d.%d", (dwIpAddr>>24)&0xFF, (dwIpAddr>>16)&0xFF, (dwIpAddr>>8)&0xFF, dwIpAddr&0xFF);
	bResult = SetRegKeyString( FILENAME_PXYSRVCFG, SETCION_LocalInfo, KEY_IpAddr, achIpStr );
	if ( !bResult )
	{
		Dsclog( "[CreatPxySrvCfgFile] Write Section[%s]Key[%s] failed!\n", SETCION_LocalInfo, KEY_IpAddr );
	}

	bResult = SetRegKeyInt(  FILENAME_PXYSRVCFG, SETCION_LocalInfo, KEY_RasPort, (u32)DEFVALUE_RasPort );
	if ( !bResult )
	{
		Dsclog( "[CreatPxySrvCfgFile] Write Section[%s]Key[%s] failed!\n", SETCION_LocalInfo, KEY_RasPort );
	}

	bResult = SetRegKeyInt(  FILENAME_PXYSRVCFG, SETCION_LocalInfo, KEY_H245Port, (u32)DEFVALUE_H245Port );
	if ( !bResult )
	{
		Dsclog( "[CreatPxySrvCfgFile] Write Section[%s]Key[%s] failed!\n", SETCION_LocalInfo, KEY_H245Port );
	}

	bResult = SetRegKeyInt(  FILENAME_PXYSRVCFG, SETCION_LocalInfo, KEY_OSPPORT, (u32)DEFVALUE_OSPPORT );
	if ( !bResult )
	{
		Dsclog( "[CreatPxySrvCfgFile] Write Section[%s]Key[%s] failed!\n", SETCION_LocalInfo, KEY_OSPPORT );
	}

	bResult = SetRegKeyInt(  FILENAME_PXYSRVCFG, SETCION_LocalInfo, KEY_RTPPORT, (u32)DEFVALUE_RTPPORT );
	if ( !bResult )
	{
		Dsclog( "[CreatPxySrvCfgFile] Write Section[%s]Key[%s] failed!\n", SETCION_LocalInfo, KEY_RTPPORT );
	}

	bResult = SetRegKeyInt(  FILENAME_PXYSRVCFG, SETCION_LocalInfo, KEY_RTCPPORT, (u32)DEFVALUE_RTCPPORT );
	if ( !bResult )
	{
		Dsclog( "[CreatPxySrvCfgFile] Write Section[%s]Key[%s] failed!\n", SETCION_LocalInfo, KEY_RTCPPORT );
	}

	bResult = SetRegKeyInt(  FILENAME_PXYSRVCFG, SETCION_LocalInfo, KEY_MEDIABASEPORT, (u32)DEFVALUE_MEDIABASEPORT );
	if ( !bResult )
	{
		Dsclog( "[CreatPxySrvCfgFile] Write Section[%s]Key[%s] failed!\n", SETCION_LocalInfo, KEY_MEDIABASEPORT );
	}

	// [GKInfo]
	memset(achIpStr, 0, sizeof(achIpStr));
	dwIpAddr = ntohl(GetBrdIpAddr());
	sprintf(achIpStr, "%d.%d.%d.%d", (dwIpAddr>>24)&0xFF, (dwIpAddr>>16)&0xFF, (dwIpAddr>>8)&0xFF, dwIpAddr&0xFF);
	bResult = SetRegKeyString( FILENAME_PXYSRVCFG, SECTION_GKInfo, KEY_GKIpAddr, achIpStr );
	if ( !bResult )
	{
		Dsclog( "[CreatPxySrvCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_GKInfo, KEY_GKIpAddr );
	}

	bResult = SetRegKeyInt(  FILENAME_PXYSRVCFG, SECTION_GKInfo, KEY_GKRasPort, (u32)DEFVALUE_GKRasPort );
	if ( !bResult )
	{
		Dsclog( "[CreatPxySrvCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_GKInfo, KEY_GKRasPort );
	}

	return TRUE;
}

/*=============================================================================
�� �� ���� CreatMtadpDebugFile
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/9/15   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CDscConfig::CreatMtadpDebugFile()
{
	s8 achProfileName[64];
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_MTADPDEBUG_INI);

	FILE *stream = fopen( achProfileName, "r" );
    if( stream != NULL )
    {
		fclose(stream);
        return(	TRUE );
    }

	if ( CreatDir( DIR_CONFIG ) )
	{
		stream = fopen( achProfileName, "w" );
		if ( stream == NULL )
		{
			return FALSE;
		}
		fclose(stream);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*=============================================================================
�� �� ���� CreatBrdCfgFile
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/9/21   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CDscConfig::CreatBrdCfgFile()
{
	s8 achProfileName[64];
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);

	FILE *stream = fopen( achProfileName, "r" );
    if( stream != NULL )
    {
		fclose(stream);
        return(	TRUE );
    }

	if ( CreatDir( DIR_CONFIG ) )
	{
		stream = fopen( achProfileName, "w" );
		if ( stream == NULL )
		{
			return FALSE;
		}
		fclose(stream);
	}
	else
	{
		return FALSE;
	}

	BOOL32 bResult = TRUE;
	// [BoardSystem]
	bResult = SetRegKeyString( achProfileName, SECTION_BoardSystem, KEY_MpcIpAddr, DEFAULT_MCU_INNERIP );
	if ( !bResult )
	{
		Dsclog( "[CreatBrdCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_BoardSystem, KEY_MpcIpAddr );
	}

	bResult = SetRegKeyInt(  achProfileName, SECTION_BoardSystem, KEY_MpcPort, (u32)MCU_LISTEN_PORT );
	if ( !bResult )
	{
		Dsclog( "[CreatBrdCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_BoardSystem, KEY_MpcPort );
	}

	bResult = SetRegKeyInt( achProfileName, SECTION_BoardSystem, KEY_HeartBeatTime, (u32)DEF_OSPDISC_HBTIME );
	if ( !bResult )
	{
		Dsclog( "[CreatBrdCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_BoardSystem, KEY_HeartBeatTime );
	}

	bResult = SetRegKeyInt(  achProfileName, SECTION_BoardSystem, KEY_HeartBeatNum, (u32)DEF_OSPDISC_HBNUM );
	if ( !bResult )
	{
		Dsclog( "[CreatBrdCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_BoardSystem, KEY_HeartBeatNum );
	}

	// IsFront
	bResult = SetRegKeyInt(  achProfileName, SECTION_IsFront, KEY_Flag, (u32)DEFVALUE_Flag );
	if ( !bResult )
	{
		Dsclog( "[CreatBrdCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_IsFront, KEY_Flag );
	}

	return TRUE;
}

/*=============================================================================
�� �� ���� CreatModCfgFile
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/9/21   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CDscConfig::CreatModCfgFile()
{
	s8 achProfileName[64];
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_MODCFG_INI);

	FILE *stream = fopen( achProfileName, "r" );
    if( stream != NULL )
    {
		fclose(stream);
        return(	TRUE );
    }

	if ( CreatDir( DIR_CONFIG ) )
	{
		stream = fopen( achProfileName, "w" );
		if ( stream == NULL )
		{
			return FALSE;
		}
		fclose(stream);
	}
	else
	{
		return FALSE;
	}

	BOOL32 bResult = TRUE;
	// [Modules]
	bResult = SetRegKeyInt( achProfileName, SETCION_Modules, KEY_mtadp, (u32)DEFVALUE_mtadp );
	if ( !bResult )
	{
		Dsclog( "[CreatModCfgFile] Write Section[%s]Key[%s] failed!\n", SETCION_Modules, KEY_mtadp );
	}

	bResult = SetRegKeyInt(  achProfileName, SETCION_Modules, KEY_mp, (u32)DEFVALUE_mp );
	if ( !bResult )
	{
		Dsclog( "[CreatModCfgFile] Write Section[%s]Key[%s] failed!\n", SETCION_Modules, KEY_mp );
	}

	bResult = SetRegKeyInt( achProfileName, SETCION_Modules, KEY_prs, (u32)DEFVALUE_prs );
	if ( !bResult )
	{
		Dsclog( "[CreatModCfgFile] Write Section[%s]Key[%s] failed!\n", SETCION_Modules, KEY_prs );
	}

	// mpcfg
	bResult = SetRegKeyInt(  achProfileName, SECTION_mpcfg, KEY_waitingIFrame, (u32)DEFVALUE_waitingIFrame );
	if ( !bResult )
	{
		Dsclog( "[CreatModCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_mpcfg, KEY_waitingIFrame );
	}

	return TRUE;
}

/*=============================================================================
�� �� ���� CreatDcsCfgFile
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/9/24   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CDscConfig::CreatDcsCfgFile()
{
	FILE *stream = fopen( FILENAME_DCSCONFIG, "r" );
    if( stream != NULL )
    {
		fclose(stream);
        return(	TRUE );
    }

	//�ļ������ڣ��������ļ�
	s8 ch = '/';
	s8 achDirPath[64];
	memset( achDirPath, 0, sizeof(achDirPath) );
	sprintf( achDirPath, "%s", FILENAME_DCSCONFIG );
	s8* pchDest = strrchr( achDirPath, ch );
	*pchDest = '\0';
	if ( CreatDir( achDirPath ) )
	{
		stream = fopen( FILENAME_DCSCONFIG, "w" );
		if ( stream == NULL )
		{
			return FALSE;
		}
		fclose(stream);
	}
	else
	{
		return FALSE;
	}

	BOOL32 bResult = TRUE;
	// DCS ip
	s8 achIpStr[17] = {0};
	u32 dwIpAddr = ntohl(GetBrdIpAddr());
	sprintf(achIpStr, "%d.%d.%d.%d", (dwIpAddr>>24)&0xFF, (dwIpAddr>>16)&0xFF, (dwIpAddr>>8)&0xFF, dwIpAddr&0xFF);
	bResult = SetRegKeyString( FILENAME_DCSCONFIG, SECTION_DCServer, KEY_DCSIP, achIpStr );
	if ( !bResult )
	{
		Dsclog( "[CreatDcsCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_DCServer, KEY_DCSIP );
	}

	// MCU ip
	dwIpAddr = ntohl(GetConnectMcuIpAddr());
	memset( achIpStr, 0, sizeof(achIpStr) );
	sprintf(achIpStr, "%d.%d.%d.%d", (dwIpAddr>>24)&0xFF, (dwIpAddr>>16)&0xFF, (dwIpAddr>>8)&0xFF, dwIpAddr&0xFF);
	bResult = SetRegKeyString( FILENAME_DCSCONFIG, SECTION_DCServer, KEY_DCSMCUIP, achIpStr );
	if ( !bResult )
	{
		Dsclog( "[CreatDcsCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_DCServer, KEY_DCSMCUIP );
	}

	// GK ip
	dwIpAddr = GetGKIpAddr();
	memset( achIpStr, 0, sizeof(achIpStr) );
	sprintf(achIpStr, "%d.%d.%d.%d", (dwIpAddr>>24)&0xFF, (dwIpAddr>>16)&0xFF, (dwIpAddr>>8)&0xFF, dwIpAddr&0xFF);
	bResult = SetRegKeyString( FILENAME_DCSCONFIG, SECTION_DCServer, KEY_DCSGKIP, achIpStr );
	if ( !bResult )
	{
		Dsclog( "[CreatDcsCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_DCServer, KEY_DCSGKIP );
	}

	// DCST120Port
	bResult = SetRegKeyInt( FILENAME_DCSCONFIG, SECTION_DCServer, KEY_DCST120Port, (u32)DEFVALUE_DCST120Port );
	if ( !bResult )
	{
		Dsclog( "[CreatDcsCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_DCServer, KEY_DCST120Port );
	}

	//DCSDccPort
	bResult = SetRegKeyInt( FILENAME_DCSCONFIG, SECTION_DCServer, KEY_DCSDccPort, (u32)DEFVALUE_DCSDccPort );
	if ( !bResult )
	{
		Dsclog( "[CreatDcsCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_DCServer, KEY_DCSDccPort );
	}

	//DCSRASPort
	bResult = SetRegKeyInt( FILENAME_DCSCONFIG, SECTION_DCServer, KEY_DCSRASPort, (u32)DEFVALUE_DCSRASPort );
	if ( !bResult )
	{
		Dsclog( "[CreatDcsCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_DCServer, KEY_DCSRASPort );
	}

	//DCSH323Port
	bResult = SetRegKeyInt( FILENAME_DCSCONFIG, SECTION_DCServer, KEY_DCSH323Port, (u32)DEFVALUE_DCSH323Port );
	if ( !bResult )
	{
		Dsclog( "[CreatDcsCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_DCServer, KEY_DCSH323Port );
	}

	//DCSTelnetPort
	bResult = SetRegKeyInt( FILENAME_DCSCONFIG, SECTION_DCServer, KEY_DCSTelnetPort, (u32)DEFVALUE_DCSTelnetPort );
	if ( !bResult )
	{
		Dsclog( "[CreatDcsCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_DCServer, KEY_DCSTelnetPort );
	}

	//DCSMCUPort
	bResult = SetRegKeyInt( FILENAME_DCSCONFIG, SECTION_DCServer, KEY_DCSMCUPort, (u32)DEFVALUE_DCSMCUPort );
	if ( !bResult )
	{
		Dsclog( "[CreatDcsCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_DCServer, KEY_DCSMCUPort );
	}

	//DCSBACKUPMCUIP
	bResult = SetRegKeyInt( FILENAME_DCSCONFIG, SECTION_DCServer, KEY_DCSBACKUPMCUIP, (u32)DEFVALUE_DCSBACKUPMCUIP );
	if ( !bResult )
	{
		Dsclog( "[CreatDcsCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_DCServer, KEY_DCSBACKUPMCUIP );
	}

	//DCSBACKMCUPORT
	bResult = SetRegKeyInt( FILENAME_DCSCONFIG, SECTION_DCServer, KEY_DCSBACKUPMCUPort, (u32)DEFVALUE_DCSBACKUPMCUPort );
	if ( !bResult )
	{
		Dsclog( "[CreatDcsCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_DCServer, KEY_DCSBACKUPMCUPort );
	}

	//DCSGKPort
	bResult = SetRegKeyInt( FILENAME_DCSCONFIG, SECTION_DCServer, KEY_DCSGKPort, (u32)DEFVALUE_DCSGKPort );
	if ( !bResult )
	{
		Dsclog( "[CreatDcsCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_DCServer, KEY_DCSGKPort );
	}

	//DCSGKRegNum
	bResult = SetRegKeyInt( FILENAME_DCSCONFIG, SECTION_DCServer, KEY_DCSGKRegNum, (u32)DEFVALUE_DCSGKRegNum );
	if ( !bResult )
	{
		Dsclog( "[CreatDcsCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_DCServer, KEY_DCSGKRegNum );
	}

	s8 achStr[32];
	//DCSAlias
	memset(achStr, 0, sizeof(achStr));
	bResult = SetRegKeyString( FILENAME_DCSCONFIG, SECTION_DCServer, KEY_DCSAlias, DEFVALUE_DCSAlias );
	if ( !bResult )
	{
		Dsclog( "[CreatDcsCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_DCServer, KEY_DCSAlias );
	}

	//DCSEmail
	memset(achStr, 0, sizeof(achStr));
	bResult = SetRegKeyString( FILENAME_DCSCONFIG, SECTION_DCServer, KEY_DCSEmail, DEFVALUE_DCSEmail );
	if ( !bResult )
	{
		Dsclog( "[CreatDcsCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_DCServer, KEY_DCSEmail );
	}

	//DCSLocation
	memset(achStr, 0, sizeof(achStr));
	bResult = SetRegKeyString( FILENAME_DCSCONFIG, SECTION_DCServer, KEY_DCSLocation, DEFVALUE_DCSLocation );
	if ( !bResult )
	{
		Dsclog( "[CreatDcsCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_DCServer, KEY_DCSLocation );
	}

	//DCSPhoneNum
	memset(achStr, 0, sizeof(achStr));
	bResult = SetRegKeyString( FILENAME_DCSCONFIG, SECTION_DCServer, KEY_DCSPhoneNum, DEFVALUE_DCSPhoneNum );
	if ( !bResult )
	{
		Dsclog( "[CreatDcsCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_DCServer, KEY_DCSPhoneNum );
	}

	return TRUE;
}

/*=============================================================================
�� �� ���� ModifyDcsConfig
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  u32 dwDcsIp (������)
           u32 dwMcuIp (������)
           u32 dwGKIp (������)
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/9/24   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CDscConfig::ModifyDcsConfig( u32 dwDcsIp, u32 dwMcuIp, u32 dwGKIp )
{
	if ( !CreatDcsCfgFile() )
	{
		OspPrintf(TRUE, FALSE, "[ModifyDcsConfig] The DCS config file isn't exist and can't be created!\n");
		return FALSE;
	}

	BOOL32 bResult = TRUE;
	// DCS ip
	s8 achIpStr[17] = {0};
	sprintf(achIpStr, "%d.%d.%d.%d", (dwDcsIp>>24)&0xFF, (dwDcsIp>>16)&0xFF, (dwDcsIp>>8)&0xFF, dwDcsIp&0xFF);
	bResult = SetRegKeyString( FILENAME_DCSCONFIG, SECTION_DCServer, KEY_DCSIP, achIpStr );
	if ( !bResult )
	{
		Dsclog( "[CreatDcsCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_DCServer, KEY_DCSIP );
	}

	// MCU ip
	if ( dwMcuIp != 0 )
	{
		memset( achIpStr, 0, sizeof(achIpStr) );
		sprintf(achIpStr, "%d.%d.%d.%d", (dwMcuIp>>24)&0xFF, (dwMcuIp>>16)&0xFF, (dwMcuIp>>8)&0xFF, dwMcuIp&0xFF);
		bResult = SetRegKeyString( FILENAME_DCSCONFIG, SECTION_DCServer, KEY_DCSMCUIP, achIpStr );
		if ( !bResult )
		{
			Dsclog( "[CreatDcsCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_DCServer, KEY_DCSMCUIP );
		}
	}

	// GK ip
	if ( dwGKIp != 0 )
	{
		memset( achIpStr, 0, sizeof(achIpStr) );
		sprintf(achIpStr, "%d.%d.%d.%d", (dwGKIp>>24)&0xFF, (dwGKIp>>16)&0xFF, (dwGKIp>>8)&0xFF, dwGKIp&0xFF);
		bResult = SetRegKeyString( FILENAME_DCSCONFIG, SECTION_DCServer, KEY_DCSGKIP, achIpStr );
		if ( !bResult )
		{
			Dsclog( "[CreatDcsCfgFile] Write Section[%s]Key[%s] failed!\n", SECTION_DCServer, KEY_DCSGKIP );
		}
	}

	return TRUE;
}

/*=============================================================================
�� �� ���� CreatDir
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const s8* lpszDirPath
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/9/15   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CDscConfig::CreatDir(const s8* lpszDirPath)
{
	if ( NULL == lpszDirPath )
	{
		return FALSE;
	}

	s32 nRet = 0;
	s8 achDirPath[64];
	memset(achDirPath, 0, sizeof(achDirPath));
	sprintf(achDirPath, "%s", lpszDirPath);
	s8 ch = '/';
	s8 *pchDest = NULL;

	pchDest = strrchr( achDirPath, ch );
#ifdef _LINUX_
    DIR* pdir = opendir(achDirPath);
    if ( pdir != NULL )
	{
		closedir(pdir);
		chmod(achDirPath, 0777);
		return TRUE;
	}

	if ( pchDest == NULL || pchDest == achDirPath )
	{
		nRet = mkdir( achDirPath, 0777 );
		if( 0 != nRet )
	    {
		    printf("make diretory<%s> Error :%s\n",achDirPath,strerror(errno)); 
            return FALSE;
	    }
	}
	else
	{
		*pchDest = '\0';
		if ( CreatDir(achDirPath) )
		{
			nRet = mkdir( lpszDirPath, 0777 );
			if( 0 != nRet )
			{
				printf("make diretory<%s> Error :%s\n",lpszDirPath,strerror(errno));  
				return FALSE;
			}
        }
		else
		{
			return FALSE;
		}
	}		                
#endif
#ifdef WIN32
	if ( !CreateDirectory( DIR_CONFIG, NULL ) )
	{
		return FALSE;
	}
#endif
	return TRUE;
}
