/*****************************************************************************
    ģ����      : EqpNmc
    �ļ���      : eqpagt.cpp
    ����ļ�    : 
    �ļ�ʵ�ֹ���: EqpAgt�ӿڼ�ʵ��
    ����        : liaokang
    �汾        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    �޸ļ�¼:
    ��  ��      �汾        �޸���          �޸�����
    2012/06/18  4.7         liaokang        ����
******************************************************************************/
#include "eqpagt.h"

CEqpAgt g_cEqpAgt;
// �ź�������
extern SEMHANDLE g_hEqpAgtCfg;

// ���캯��
CEqpAgt::CEqpAgt()
{
    memset( &m_achIpCfgFile, 0, sizeof(m_achIpCfgFile));
    memset( &m_tNetAdaptInfoAll, 0, sizeof(m_tNetAdaptInfoAll));
}

// ��������
CEqpAgt::~CEqpAgt()
{
    memset( &m_achIpCfgFile, 0, sizeof(m_achIpCfgFile));
    memset( &m_tNetAdaptInfoAll, 0, sizeof(m_tNetAdaptInfoAll));
}

/*====================================================================
    ������      : NmsSetSysState
    ����        : ��������ϵͳ״̬����Ҫ����reboot��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: u32 dwSysState ϵͳ״̬
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void NmsSetSysState( u32 dwSysState )
{
    EqpAgtLog( LOG_DETAIL, "[NmsSetSysState] System State: %u!\n", dwSysState);

    BOOL32 bRet = TRUE;

    if ( EQP_SYSSTATE_REBOOT == dwSysState )
    {
#ifdef WIN32
        u32 dwVersion;          // �汾��
        dwVersion = GetVersion(); //�õ�WINDOWS NT��Win32�İ汾��
        if(dwVersion < 0x80000000)
        {
            // ������Ȩ��
            // �����ػ���ҪSE_SHUTDOWN_NAMEȨ��
            // Զ�̹ػ���ҪSE_REMOTE_SHUTDOWN_NAMEȨ��
            // ���Գ�����Ҫ������SE_DEBUG_NAMEȨ�ޡ�
            HANDLE hToken;
            TOKEN_PRIVILEGES tkp;
            // �õ���ǰ���̵�access token
            bRet = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken);
            EqpAgtLog( LOG_DETAIL, "[NmsSetSysState] OpenProcessToken: %d!\n", bRet );
            if ( bRet )
            {
                // �õ�ָ��Ȩ�޵�LUID
                bRet = LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
                EqpAgtLog( LOG_DETAIL, "[NmsSetSysState] LookupPrivilegeValue: %d!\n", bRet );
                tkp.PrivilegeCount = 1;
                tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
                // ��ָ����access token�ϴ򿪻�ر��ض�Ȩ�ޣ���Ҫ TOKEN_ADJUST_PRIVILEGES Ȩ��������
                bRet = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
                EqpAgtLog( LOG_DETAIL, "[NmsSetSysState] AdjustTokenPrivileges: %d!\n", bRet );
                bRet = ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
                EqpAgtLog( LOG_DETAIL, "[NmsSetSysState] ExitWindowsEx: %d!\n", bRet );
            }
        }
        else //WINϵ������ϵͳ
        {
            ExitWindowsEx(EWX_REBOOT | EWX_FORCE,0);
        }

#endif

#ifdef _LINUX_ //  linux
// ����֧��        
#endif
    }
}

/*====================================================================
    ������      : NmsSetIpInfo
    ����        : ��������IP��Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: TNetAdaptInfoAll& tNetAdapterInfoAll IP��Ϣ
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void NmsSetIpInfo( TNetAdaptInfoAll& tNetAdapterInfoAll )
{
    /************************************************************************/
    /*                ��ʱ���ε�������֧��  begin                           */
    /************************************************************************/


    /************************************************************************/
    /*                 ��ʱ���ε�������֧��  end                            */
    /************************************************************************/
}

/*====================================================================
    ������      : GetSysState
    ����        : ��ȡϵͳ״̬ ����ʱ���� running��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: void
    ����ֵ˵��  : u32 ϵͳ״̬
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
u32 CEqpAgt::GetSysState( void )
{
    return EQP_SYSSTATE_RUNNING;
}

/*====================================================================
    ������      : GetSoftwareVer
    ����        : ��ȡ����汾�� ����ʱ��Ϊ unknown��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: LPSTR lpszSoftwareVer ����汾��
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgt::GetSoftwareVer( LPSTR lpszSoftwareVer )
{
    if ( NULL == lpszSoftwareVer )
    {
        EqpAgtLog( LOG_ERROR, "[GetSoftwareVer] The input param is null!\n");
        return;
    }
    // ��ʱ��Ϊδ֪
    sprintf(lpszSoftwareVer, "software unknown");
}

/*====================================================================
    ������      : GetHardwareVer
    ����        : ��ȡӲ���汾�� ����ʱ��Ϊ unknown��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: LPSTR lpszHardwareVer Ӳ���汾��
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgt::GetHardwareVer( LPSTR lpszHardwareVer )
{
    if ( NULL == lpszHardwareVer )
    {
        EqpAgtLog( LOG_ERROR, "[GetHardwareVer] The input param is null!\n");
        return;
    }
    // ��ʱ��Ϊδ֪
    sprintf(lpszHardwareVer, "hardware unknown");
}

/*====================================================================
    ������      : GetCompileTime
    ����        : ��ȡ����ʱ��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: LPSTR lpszCompileTime ����ʱ��
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgt::GetCompileTime( LPSTR lpszCompileTime )
{
	if ( NULL == lpszCompileTime )
	{
        EqpAgtLog( LOG_ERROR, "[GetCompileTime] The input param is null!\n");
        return;
    }    
    sprintf(lpszCompileTime, "%s %s", __DATE__, __TIME__);
}

/*====================================================================
    ������      : GetOsType
    ����        : ��ȡ�豸����ϵͳ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: void
    ����ֵ˵��  : u32 ����ϵͳ
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
u32 CEqpAgt::GetOsType()
{    
    u32 dwOsType = EQP_OSTYPE_UNKNOWN;
#ifdef WIN32
    dwOsType = EQP_OSTYPE_WIN32;
#elif defined _LINUX_
    dwOsType = EQP_OSTYPE_LINUX;
#endif
    return dwOsType;
}

/*====================================================================
    ������      : GetSubType
    ����        : ��ȡ�豸������
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: u32* pdwSubType �豸������
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgt::GetSubType( u32* pdwSubType )
{    
    if ( NULL == pdwSubType )
    {
        EqpAgtLog( LOG_ERROR, "[GetSubType] The input param is null!\n");
        return FALSE;
    }

    // ������Ϣ��/д�ź�������
    ENTER( g_hEqpAgtCfg );

    s8      achProfileName[32] = {0};
    s8      achDefStr[] = "Cannot find the section or key";
    sprintf( achProfileName, "%s/%s", DIR_CONFIG, EQPAGTCFGFILENAME );
    
    // �ж������ļ��Ƿ���ڡ��������ļ�
    FILE* hFileR = NULL;
    s32 nFopenErr = 0;
    hFileR = fopen( achProfileName, "r" );
    if( NULL != hFileR ) // exist
    {
        BOOL32  bResult = FALSE;
        s8      achReturn[EQPAGT_MAX_LENGTH] = {0};       
        
        BOOL32 bSucceedRead = TRUE;
        bResult = GetRegKeyString( achProfileName, SECTION_EqpSubType, KEY_EqpSubType, 
            achDefStr, achReturn, MAX_VALUE_LEN + 1 );
        if( bResult == FALSE )  
        {
            EqpAgtLog( LOG_ERROR, "[GetSubType] Wrong profile while reading %s!\n", KEY_EqpSubType );
            bSucceedRead = FALSE;
        }
        else
        {

            if ( !strcmp( EQPSUBTYPE_KDV2K, achReturn ) )            // 2k
            {
                *pdwSubType = EQP_SUBTYPE_KDV2K;
            }
            else if ( !strcmp( EQPSUBTYPE_KDV2KE, achReturn ) )      // 2ke
            {
                *pdwSubType = EQP_SUBTYPE_KDV2KE;
            }
        }
        fclose( hFileR );
        /*lint -save -esym(438, hFileR)*/
        hFileR = NULL;
        /*lint -restore*/
        return bSucceedRead;
    }
    else
    {
        
#ifdef _LINUX_
        nFopenErr = errno;	
#endif
        
#ifdef WIN32
        nFopenErr = GetLastError(); 
#endif
        //�������ļ�ʧ�ܵĴ���ź�ʱ��
        time_t tiCurTime = ::time(NULL);
        EqpAgtLog( LOG_ERROR, "[GetSubType] error:%s,time:%s !\n", strerror(nFopenErr), ctime(&tiCurTime) );
        return FALSE;
    }
}

/*====================================================================
    ������      : GetDiskPartionName
    ����        : ��ȡ�̷���
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: LPSTR lpszDiskPartionName �̷���
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgt::GetDiskPartionName( LPSTR lpszDiskPartionName )
{
    if ( NULL == lpszDiskPartionName )
    {
        EqpAgtLog( LOG_ERROR, "[GetDiskPartionName] The input param is null!\n");
        return FALSE;
    }
    
    // ������Ϣ��/д�ź�������
    ENTER( g_hEqpAgtCfg );

    s8      achProfileName[32] = {0};
    sprintf( achProfileName, "%s/%s", DIR_CONFIG, EQPAGTCFGFILENAME );

    // �ж������ļ��Ƿ���ڡ��������ļ�
    FILE* hFileR = NULL;
    s32 nFopenErr = 0;
    hFileR = fopen( achProfileName, "r" );
    if( NULL != hFileR ) // exist
    {
        BOOL32  bResult = FALSE;
        s8      achReturn[EQPAGT_MAX_LENGTH] = {0};       

        BOOL32 bSucceedRead = TRUE;
        bResult = GetRegKeyString( achProfileName, SECTION_DiskPartionName, KEY_DiskPartionName, 
            DEF_DiskPartionName, achReturn, MAX_VALUE_LEN + 1 );
        if( bResult == FALSE )  
        {
            EqpAgtLog( LOG_ERROR, "[GetDiskPartionName] Wrong profile while reading %s!\n", KEY_DiskPartionName );
            bSucceedRead = FALSE;
        }
        else
        {
            memcpy( lpszDiskPartionName, achReturn, sizeof(achReturn) );
        }
        fclose( hFileR );
        hFileR = NULL;
        return bSucceedRead;
    }
    else
    {
        nFopenErr = GetLastError(); 
        //�������ļ�ʧ�ܵĴ���ź�ʱ��
        time_t tiCurTime = ::time(NULL);
        EqpAgtLog( LOG_ERROR, "[GetDiskPartionName] error:%s,time:%s !\n", strerror(nFopenErr), ctime(&tiCurTime) );
        return FALSE;
    }
}

/*====================================================================
    ������      : GetIpInfoAll
    ����        : ��ȡIP��Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: TNetAdaptInfoAll& tNetAdapterInfoAll IP��Ϣ
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgt::GetIpInfoAll( TNetAdaptInfoAll* ptNetAdapterInfoAll )
{
    if( NULL == ptNetAdapterInfoAll )
    {
        EqpAgtLog( LOG_ERROR, "[GetIpInfoAll] The param is null!\n");
        return FALSE;
    }

    if ( !GetNetAdapterInfoAll( ptNetAdapterInfoAll ) ) 
    {
        EqpAgtLog( LOG_ERROR, "[GetIpInfoAll] GetNetAdapterInfoAll wrong!\n");
        return FALSE;
    }  
    EqpAgtLog( LOG_DETAIL, "[GetIpInfoAll] GetNetAdapterInfoAll right!\n");
    return TRUE;
}

/*====================================================================
    ������      : SetIpInfoAll
    ����        : ����IP��Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: TNetAdaptInfoAll& tNetAdapterInfoAll IP��Ϣ
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgt::SetIpInfoAll( TNetAdaptInfoAll& tNetAdapterInfoAll )
{
    /************************************************************************/
    /*                ��ʱ���ε�������֧��                                  */
    /************************************************************************/

    return TRUE;    
}

/*====================================================================
    ������      : IpCfgInit
    ����        : ���ipConfig.ini�ļ��Ƿ����
                  ���ڣ���IP������Ϣ���ڴ��У�����IP��Ϣ
                  �����ڣ���ϵͳIP��Ϣ������IP�����ļ�
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: void
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgt::IpCfgInit(void)
{
    sprintf( m_achIpCfgFile, "%s/%s", DIR_CONFIG, IPCFGFILENAME );

    // ��IP������Ϣ
    BOOL32 bRet = ReadIpCfg( m_achIpCfgFile );
    if ( !bRet )
    {
        // ����Ĭ�������ļ�
        bRet = PretreatIpCfgFile( DIR_CONFIG, IPCFGFILENAME );
        if ( !bRet )
        {
            EqpAgtLog( LOG_ERROR, "[IpCfgInit] Create ip config file failed!\n");
            return FALSE;
        }
        EqpAgtLog( LOG_KEYSTATUS, "[IpCfgInit] Create ip config file succecced!\n");
        return TRUE;
    }

    // ���������ļ�����IP
    SetIpInfoAll( m_tNetAdaptInfoAll );
    EqpAgtLog( LOG_KEYSTATUS, "[IpCfgInit] Ip config !\n");
    return TRUE;
}

/*====================================================================
    ������      : ReadIpCfg
    ����        : ��IP������Ϣ���ڴ���
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: LPCSTR lpszCfgFileFullPath  �ļ�ȫ·��
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgt::ReadIpCfg( LPCSTR lpszCfgFileFullPath )
{
    if( NULL == lpszCfgFileFullPath )
    {
        EqpAgtLog( LOG_ERROR, "[ReadIpCfg] The param is null!\n");
        return FALSE;
    }

    // �ж������ļ��Ƿ���ڡ��������ļ�
    FILE* hFileR = NULL;
    s32 nFopenErr = 0;
    hFileR = fopen( lpszCfgFileFullPath, "r" );
    if( NULL != hFileR ) // exist
    {
        // ��ȡ�����ļ��е�������Ϣ
        TNetAdaptInfoAll tNetAdapterInfoAll;
        memset( &tNetAdapterInfoAll, 0, sizeof(tNetAdapterInfoAll));
        BOOL32 bRet = ReadIpInfoTable( &tNetAdapterInfoAll );
        fclose( hFileR );
        /*lint -save -esym(438, hFileR)*/
        hFileR = NULL;
        /*lint -restore*/
        if ( !bRet )
        {
            EqpAgtLog( LOG_ERROR, "[ReadIpCfg] GetIpInfoAll failed!\n");
            return FALSE;
        }
        // ��������Ϣ���浽��Ա����
        MEMCPY( &m_tNetAdaptInfoAll, &tNetAdapterInfoAll, sizeof(m_tNetAdaptInfoAll) );
        return TRUE;
    }
    else
    {
        nFopenErr = GetLastError(); 
        //�������ļ�ʧ�ܵĴ���ź�ʱ��
        time_t tiCurTime = ::time(NULL);
        EqpAgtLog( LOG_ERROR, "[ReadIpCfg] error:%s,time:%s !\n", strerror(nFopenErr), ctime(&tiCurTime) );
        return FALSE;
    }

}

/*====================================================================
    ������      : CreateDir
    ����        : ����Ŀ¼
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: LPCSTR pPathName   Ĭ��Ŀ¼��
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgt::CreateDir( LPCSTR pPathName )
{
#ifdef WIN32
    /* =======================================
    ������·���ȶ�ԭ��:
    ·���ȶ� c:\tt  ������һ���ļ�(����չ������
    ·���ȶ�Ҳ��õ���ֵ��ʵ����Ŀ¼��������
    ========================================*/
    CreateDirectory( ( LPCTSTR )pPathName, NULL );
#endif

#ifdef _LINUX_
    mkdir( ( s8* )pPathName, 700 );
#endif
    
	return;
}

/*====================================================================
    ������      : PretreatIpCfgFile
    ����        : ����Ĭ�ϵ�IP��Ϣ�����ļ�
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: LPCSTR lpszCfgPath  �ļ�·��
                  LPCSTR lpszCfgName  �ļ���
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgt::PretreatIpCfgFile( LPCSTR lpszCfgPath, LPCSTR lpszCfgName )
{

    if( NULL == lpszCfgPath || NULL == lpszCfgName )
    {
        EqpAgtLog( LOG_ERROR, "[PretreatIpCfgFile] The param is null!\n");
        return FALSE;
    }

    CreateDir( lpszCfgPath );

    s8 achProfileName[EQPAGT_MAX_LENGTH] = {0};    
    sprintf( achProfileName, "%s/%s", lpszCfgPath, lpszCfgName );

    // �����ļ�
    FILE* hFileC = NULL;
    hFileC = fopen( achProfileName, "w" );
    if( NULL == hFileC )
    { 
        EqpAgtLog( LOG_ERROR, "[PretreatIpCfgFile] Create config file failed!\n");
        return FALSE;
    }
    
    BOOL32 bRet = TRUE;
    //��ȡ���������Ϣ
    TNetAdaptInfoAll tNetAdapterInfoAll;
    memset( &tNetAdapterInfoAll, 0, sizeof(tNetAdapterInfoAll));
    bRet = GetIpInfoAll( &tNetAdapterInfoAll );
    if ( !bRet )
    {
        EqpAgtLog( LOG_ERROR, "[PretreatIpCfgFile] GetIpInfoAll failed!\n");
        return FALSE;
    }    

    // ��������Ϣ��ȡ����(����Ϣ�ᱣ�浽��Ա������IP�����ļ���)
    MEMCPY( &m_tNetAdaptInfoAll, &tNetAdapterInfoAll, sizeof(m_tNetAdaptInfoAll) );

    // ����������ļ���
    bRet = WriteIpInfoTable( tNetAdapterInfoAll );
    if ( !bRet )
    {
        EqpAgtLog( LOG_ERROR, "[PretreatIpCfgFile] WriteIpInfoTable failed!\n");
    }
    
    fclose(hFileC);	
    /*lint -save -esym(438, hFileC)*/
    hFileC = NULL;
    /*lint -restore*/
    return TRUE;
}

/*====================================================================
    ������      : ReadIpInfoTable
    ����        : ��IP�����ļ��ж�ȡ��Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: TNetAdaptInfoAll tNetAdapterInfoAll  IP��Ϣ
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgt::ReadIpInfoTable( TNetAdaptInfoAll* ptNetAdapterInfoAll )
{
    /************************************************************************/
    /*                ��ʱ���ε�������֧��  begin                           */
    /************************************************************************/
    
    
    /************************************************************************/
    /*                 ��ʱ���ε�������֧��  end                            */
    /************************************************************************/
    return TRUE;
} 
/*====================================================================
    ������      : WriteIpInfoTable
    ����        : ��IP��Ϣд�������ļ���
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: TNetAdaptInfoAll tNetAdapterInfoAll  IP��Ϣ
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgt::WriteIpInfoTable( TNetAdaptInfoAll tNetAdapterInfoAll )
{
    /************************************************************************/
    /*                ��ʱ���ε�������֧��  begin                           */
    /************************************************************************/
    
    
    /************************************************************************/
    /*                 ��ʱ���ε�������֧��  end                            */
    /************************************************************************/
    return TRUE;
}

/*====================================================================
    ������      : SetEqpAgtCfgDefValue
    ����        : ����EqpAgtCfg�����ļ����Ĭ��ֵ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: LPCSTR lpszFileName  �������û��������
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void SetEqpAgtCfgDefValue( LPCSTR lpszFileName )
{
    // �����̷�Ĭ��ֵ D:
    BOOL32 bResult = SetRegKeyString( lpszFileName, SECTION_DiskPartionName, KEY_DiskPartionName, "D:");
    if ( !bResult)
    {
        EqpAgtLog( LOG_ERROR,"[PretreatCfgFile->SetEqpAgtCfgDefValue] Write default DiskPartionName failed" );
    }

    // �����豸������Ĭ��ֵ(�ÿ�)
    bResult = SetRegKeyString( lpszFileName, SECTION_EqpSubType, KEY_EqpSubType, "");
    if ( !bResult)
    {
        EqpAgtLog( LOG_ERROR,"[PretreatCfgFile->SetEqpAgtCfgDefValue] Write default eqp subtype failed" );
    }

}

/*====================================================================
    ������      : EqpAgtInit
    ����        : EqpAgt��ʼ��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: void
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgt::EqpAgtInit(void)
{
    BOOL32 bRet = TRUE;

    if( !CreateSemHandle(g_hEqpAgtCfg) ) // �����ź��� 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtInit] Fail to create cfg semHandle\n");
        FreeSemHandle( g_hEqpAgtCfg );
    }

    /************************************************************************/
    /*                ��ʱ���ε�������֧��  begin                           */
    /************************************************************************/
    // IP������Ϣ��ʼ��
    /*
    bRet = IpCfgInit();
    if ( !bRet )
    {
        printf( "[EqpAgtInit] IpCfgInit error!\n" );
        EqpAgtLog( LOG_ERROR,"[EqpAgtInit] IpCfgInit error" );
    }
    */
    /************************************************************************/
    /*                 ��ʱ���ε�������֧��  end                            */
    /************************************************************************/

    // ע�ắ�� ���������ļ���Ĭ��ֵ
    EqpAgtCfgDefValueOper( SetEqpAgtCfgDefValue );
    
    // EqpAgt���������ʼ��
    u16 wRet = EqpAgtCommBaseInit( AID_EQPAGT_SCAN, APPPRI_EQPAGT_SCAN, DIR_CONFIG, EQPAGTCFGFILENAME );
    if ( EQPAGT_SUCCESS != wRet )
    {
        printf( "[EqpAgtInit] init error code %u!\n", wRet );
        EqpAgtLog( LOG_ERROR,"[EqpAgtInit] EqpAgtCommBaseInit error code %u", wRet );
    }

    // ��ؽӿڳ�ʼ��
    bRet = g_cEqpAgtSysInfo.Initialize( NmsSetSysState, NmsSetIpInfo );
    if ( TRUE == bRet )
    {
        g_cEqpAgtSysInfo.SupportNmsSetSysInfo();
    }
    g_cEqpAgtSysInfo.SupportNmsGetSysInfo();
    g_cEqpAgtSysInfo.SupportNmsTrapSysInfo( SCAN_STATE_TIME_OUT );

    // ϵͳ״̬
    g_cEqpAgtSysInfo.SetSysState( GetSysState() );
    // ����汾
    s8 achSoftwareVer[EQPAGT_MAX_LENGTH] = {'\0'};
	GetSoftwareVer(achSoftwareVer);
    g_cEqpAgtSysInfo.SetSoftwareVer(achSoftwareVer);
    // Ӳ���汾
    s8 achHardwareVer[EQPAGT_MAX_LENGTH] = {'\0'};
	GetHardwareVer(achHardwareVer);
    g_cEqpAgtSysInfo.SetHardwareVer(achHardwareVer);
    // ����ʱ��
    s8 achCompileTime[EQPAGT_MAX_LENGTH] = {'\0'};
	GetCompileTime(achCompileTime);
    g_cEqpAgtSysInfo.SetCompileTime(achCompileTime);
    // ����ϵͳ
    g_cEqpAgtSysInfo.SetEqpOsType(GetOsType());
    // �豸������
    u32 dwSubType = EQP_SUBTYPE_UNKNOWN;
    GetSubType( &dwSubType );
    g_cEqpAgtSysInfo.SetEqpSubType( dwSubType );
    // �����̷���
    s8 achDiskPartionName[EQPAGT_MAX_LENGTH] = {'\0'};
	GetDiskPartionName(achDiskPartionName);
    g_cEqpAgtSysInfo.SetDiskPartionName(achDiskPartionName); 


    /************************************************************************/
    /*                ��ʱ���ε��������޸�  begin                           */
    /************************************************************************/
    // ����ip��Ϣ
    /*
    TLocalIpInfo tLocalIpInfo;
    memset( &tLocalIpInfo, 0, sizeof(tLocalIpInfo) );
    GetLocalIpInfo( &tLocalIpInfo );
    g_cEqpAgtSysInfo.SetLocalIpInfo( tLocalIpInfo );
    */
    /************************************************************************/
    /*                 ��ʱ���ε��������޸�  end                            */
    /************************************************************************/
}

/*====================================================================
    ������      : EqpAgtQuit
    ����        : EqpAgt�˳�
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: void
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgt::EqpAgtQuit(void)
{
    EqpAgtCommBaseQuit();
}

// �汾��Ϣ
API void eqpnmcver( void )
{
    OspPrintf( TRUE, FALSE, "eqpnmc:         %s          compile time: %s    %s\n", VER_EQPNMC, __DATE__, __TIME__ );
    OspPrintf( TRUE, FALSE, "eqpagtcommbase: %s  compile time: %s    %s\n", VER_EQPAGTCOMMBASE, __DATE__, __TIME__ );
    OspPrintf( TRUE, FALSE, "eqpagtsysinfo:  %s   compile time: %s    %s\n", VER_EQPAGTSYSINFO, __DATE__, __TIME__ );
}

// ������Ϣ
API void eqpnmchelp( void )
{   
    eqpnmcver();    
    OspPrintf(TRUE, FALSE, "  \teqpnmcver       ----> show eqpnmc ver info.\n");

    // ������ʾ��ǰ״̬
    OspPrintf(TRUE, FALSE, "  \tshowsysstate    ----> show syetem state.\n");
    OspPrintf(TRUE, FALSE, "  \tshowsoftwarever ----> show software vertion.\n");
    OspPrintf(TRUE, FALSE, "  \tshowhardwarever ----> show hardware vertion.\n");
    OspPrintf(TRUE, FALSE, "  \tshowcompiletime ----> show compile time.\n");
    OspPrintf(TRUE, FALSE, "  \tshoweqpostype   ----> show eqp ostype.\n");
    OspPrintf(TRUE, FALSE, "  \tshoweqpsubtype  ----> show eqp subtype.\n");
    OspPrintf(TRUE, FALSE, "  \tshowcpuinfo     ----> show cpu info.\n");
    OspPrintf(TRUE, FALSE, "  \tshowmeminfo     ----> show mem inf.\n");
    OspPrintf(TRUE, FALSE, "  \tshowdiskinfo    ----> show disk info.\n");

    // debug ����������ڷ���trap
    OspPrintf(TRUE, FALSE, "  \ttrapcpu         ----> trap cpu info info.\n");
    OspPrintf(TRUE, FALSE, "  \ttrapmem         ----> trap mem info.\n");
    OspPrintf(TRUE, FALSE, "  \ttrapdisk        ----> trap disk info.\n");
    // debug ����������ڴ�/�ر���Ӧ����trap
    OspPrintf(TRUE, FALSE, "  \tcputrapoper     ----> open/close and change scan time span of cpu trap.\n");
    OspPrintf(TRUE, FALSE, "  \tmemtrapoper     ----> open/close and change scan time span of mem trap.\n");
    OspPrintf(TRUE, FALSE, "  \tdisktrapoper    ----> open/close and change scan time span of disk trap.\n");
    // debug �����������ģ��NMS����
    OspPrintf(TRUE, FALSE, "  \tsetsysstate     ----> set syetem state.\n");
    
    OspPrintf(TRUE, FALSE, "  \tIsAutoRun       ----> Judge the exe is autorun or not.\n");
    OspPrintf(TRUE, FALSE, "  \tAutoRun         ----> Start autorun.\n");
    OspPrintf(TRUE, FALSE, "  \tCancelAutoRun   ----> Cancel autorun.\n");
    OspPrintf(TRUE, FALSE, "  \tquit            ----> quit exe.\n");
    return;
}