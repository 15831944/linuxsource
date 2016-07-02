/*****************************************************************************
    ģ����      : EqpAgt
    �ļ���      : eqpagtcfg.cpp
    ����ļ�    : 
    �ļ�ʵ�ֹ���: �����豸������Ϣ
    ����        : liaokang
    �汾        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    �޸ļ�¼:
    ��  ��      �汾        �޸���          �޸�����
    2012/06/18  4.7         liaokang        ����
******************************************************************************/
#include "eqpagtcfg.h"

CEqpAgtCfg	g_cEqpAgtCfg;
CEqpAgtCfgDefValueList   g_cEqpAgtCfgDefValueList;
// �ź�������
SEMHANDLE   g_hEqpAgtCfg;


// ����
CEqpAgtCfg::CEqpAgtCfg() : m_byTrapSrvNum(0)
{
    memset( m_achCfgPath, '\0', sizeof(m_achCfgPath));
    memset( m_achCfgName, '\0', sizeof(m_achCfgName));
/*    memset( m_achCfgBakName, '\0', sizeof(m_achCfgBakName));*/
    memset( m_atEqpAgtInfoTable, '\0', sizeof(m_atEqpAgtInfoTable));
}

// ����
CEqpAgtCfg::~CEqpAgtCfg()
{
}

/*====================================================================
    ������      : EqpAgtCfgInit
    ����        : ��ʼ�������ļ�������
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: LPCSTR lpszCfgFileFullPath  �����ļ�ȫ·��
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtCfg::EqpAgtCfgInit( LPCSTR lpszCfgPath, LPCSTR lpszCfgName )
{
    if( NULL == lpszCfgPath || NULL == lpszCfgName )
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtCfgInit] The param is null!\n");
        return FALSE;
    }

    // ��¼�����ļ�ȫ·��
    memcpy( m_achCfgPath, lpszCfgPath, sizeof( m_achCfgPath ) );
    memcpy( m_achCfgName, lpszCfgName, sizeof( m_achCfgName ) );
    s8 achProfileName[EQPAGT_MAX_LENGTH] = {0};
    // eqpagt�����ļ���
    sprintf( achProfileName, "%s/%s", m_achCfgPath, m_achCfgName );
    // 1����Trap��
    BOOL32 bRet = TRUE;
    bRet = ReadTrapTable( achProfileName );
    if ( bRet )
    {
        EqpAgtLog( LOG_KEYSTATUS, "[EqpAgtCfgInit] Read cfg file successed!\n" );
        return TRUE;
    }
  
    // ����Ĭ�������ļ�
    if( TRUE == PretreatCfgFile( m_achCfgPath, m_achCfgName ) )
    {
        EqpAgtLog( LOG_KEYSTATUS, "[EqpAgtCfgInit] Create default cfg file successed!\n" );	
    }
    else
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtCfgInit] Create default cfg file failed!\n" );
        return FALSE;
    }

    return TRUE;
}

/*====================================================================
    ������      : ReadEqpAgtInfoTable
    ����        : �����EqpAgt��Ϣ��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: u8 byTrapSrvNum                   trap����������
                  TEqpAgtInfo* ptEqpAgtInfoTable    EqpAgt��Ϣ��
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtCfg::ReadEqpAgtInfoTable( u8* pbyTrapSrvNum, 
                                       TEqpAgtInfo* ptEqpAgtInfoTable )
{
    // ������Ϣ��/д�ź�������
    ENTER( g_hEqpAgtCfg );

    BOOL32 bResult = TRUE;
    if( NULL == pbyTrapSrvNum || NULL == ptEqpAgtInfoTable )
    {
        EqpAgtLog( LOG_ERROR, "[ReadTrapTable] The param is null!\n");
        bResult =  FALSE;
        return bResult;
    }

    // �������ļ��ж�ȡ����ֹд�ļ�ʱ���ļ����ݺͱ�����ͳһ��
    s8 achProfileName[EQPAGT_MAX_LENGTH] = {0};
    // eqpagt�����ļ���
    sprintf( achProfileName, "%s/%s", m_achCfgPath, m_achCfgName );
    ReadTrapTable( achProfileName );
    
    *pbyTrapSrvNum = m_byTrapSrvNum;
    EqpAgtLog( LOG_KEYSTATUS, "[ReadTrapTable] The trap server num: %d !\n", m_byTrapSrvNum );
    for( u8 byLoop = 0; byLoop < m_byTrapSrvNum; byLoop++ )
    {
        if( NULL != &ptEqpAgtInfoTable[byLoop] )
        {
            memcpy( &ptEqpAgtInfoTable[byLoop], &m_atEqpAgtInfoTable[byLoop], sizeof(TEqpAgtInfo) );
            EqpAgtLog( LOG_KEYSTATUS, "[ReadTrapTable] Read trap server id: %d !\n", byLoop );
        }
        else
        { 
            EqpAgtLog( LOG_ERROR, "[ReadTrapTable] The EqpAgtInfo list num is too small!\n");
            bResult =  FALSE;
        }
    }
    return bResult;
}

/*====================================================================
    ������      : WriteEqpAgtInfoTable
    ����        : ����дEqpAgt��Ϣ��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: u8 byTrapSrvNum                   trap����������
                  TEqpAgtInfo* ptEqpAgtInfoTable    EqpAgt��Ϣ��
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtCfg::WriteEqpAgtInfoTable( u8 byTrapSrvNum, 
                                        TEqpAgtInfo* ptEqpAgtInfoTable )
{
    // ������Ϣ��/д�ź�������
    ENTER( g_hEqpAgtCfg );

    BOOL32 bRet = TRUE;
    s8  szEqpAgtSectionInfo[MAX_WRITE_SECTIONONECE_LEN ] = {'\0'};
    s8  szRowEqpAgtInfo[MAX_VALUE_LEN + 1 ]={'\0'};
    u16 wSectionOffSet = 0;

    // �ж�ָ���Ƿ�Ϊ��
    if( NULL == ptEqpAgtInfoTable )
    {
        EqpAgtLog( LOG_ERROR, "[WriteEqpAgtInfoTable] The param is null!\n");
        return FALSE;
    }
    s8 achProfileName[EQPAGT_MAX_LENGTH] = {0};
    sprintf( achProfileName, "%s/%s", m_achCfgPath, m_achCfgName );

    // дTrap��������Ŀ
    bRet = SetRegKeyInt( achProfileName, (s8*)SECTION_EqpAgtInfoTable, (s8*)ENTRY_NUM, (s32)byTrapSrvNum );
    if( !bRet )
    {
        EqpAgtLog( LOG_ERROR, "[WriteEqpAgtInfoTable] Fail to write profile in %s\n", SECTION_EqpAgtInfoTable );
        return FALSE;
    }

    // д���óɹ���ͬ�����±�������ֹ�����˱�����д���ò��ɹ�
    m_byTrapSrvNum = byTrapSrvNum;

    // дEqpAgt��Ϣ����
    for( u8 byLoop = 0; byLoop < m_byTrapSrvNum; byLoop++ )
    {
        memset(szRowEqpAgtInfo, 0, sizeof(szRowEqpAgtInfo));
        bRet = OrganizeEqpAgtInfoTable( byLoop, &ptEqpAgtInfoTable[byLoop], szRowEqpAgtInfo );
        if (strlen(szRowEqpAgtInfo) > MAX_VALUE_LEN  )
        {
            szRowEqpAgtInfo[MAX_VALUE_LEN] = '\0';
        }
        //д�����ݶ���2k����Ϊ���д
        if ( (strlen(szEqpAgtSectionInfo) + strlen(szRowEqpAgtInfo)) > MAX_WRITE_SECTIONONECE_LEN  )
        {
            if ( !SetRegKeySection(achProfileName, (s8*)SECTION_EqpAgtInfoTable, szEqpAgtSectionInfo, wSectionOffSet, ENTRY_NUM) )
            {
                EqpAgtLog( LOG_ERROR, "[WriteEqpAgtInfoTable] SetRegKeySection error!\n");
            }
            wSectionOffSet += strlen(szEqpAgtSectionInfo);
            memset(szEqpAgtSectionInfo, 0, sizeof(szEqpAgtSectionInfo));            
        }
     	sprintf( szEqpAgtSectionInfo, "%s%s", szEqpAgtSectionInfo, szRowEqpAgtInfo);
        if( !bRet )
        {
            break;            
		}
    }

    if ( strlen(szEqpAgtSectionInfo) > 0 )
    {
        if ( !SetRegKeySection(achProfileName, (s8*)SECTION_EqpAgtInfoTable, szEqpAgtSectionInfo, wSectionOffSet, ENTRY_NUM) )
        {
            EqpAgtLog( LOG_ERROR, "[WriteEqpAgtInfoTable] SetRegKeySection error!\n");
            return FALSE;
        }
        // д���óɹ���ͬ�����±�������ֹ�����˱�����д���ò��ɹ�
        memcpy( m_atEqpAgtInfoTable, ptEqpAgtInfoTable, sizeof(m_atEqpAgtInfoTable) );    
	 }
    else
    {
        if ( !SetRegKeySection(achProfileName, (s8*)SECTION_EqpAgtInfoTable, NULL, wSectionOffSet, ENTRY_NUM) )
        {
            EqpAgtLog( LOG_ERROR, "[WriteEqpAgtInfoTable] SetRegKeySection error!\n");
            return FALSE;
        }
        // д���óɹ���ͬ�����±�������ֹ�����˱�����д���ò��ɹ�
        memset( &m_atEqpAgtInfoTable, 0, sizeof(m_atEqpAgtInfoTable) ); 
    }
    return TRUE;
}

/*====================================================================
    ������      : TrapIpsOfEqpAgt2Nms
    ����        : Trap Ip��Ϣ EqpAgt ----> Nms
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: void * pBuf:       �������
                  u16 *pwBufLen:     ���ݳ���
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtCfg::TrapIpsOfEqpAgt2Nms( void *pBuf, u16 *pwBufLen )
{
    BOOL32 bRet = TRUE;
    // �����ã���ֹ�������������
    u8 byTrapSrvNum = 0;
    TEqpAgtInfo tEqpAgtInfoTable[MAXNUM_EQPAGTINFO_LIST];
    memset( tEqpAgtInfoTable, 0, sizeof(tEqpAgtInfoTable));
    bRet = ReadEqpAgtInfoTable( &byTrapSrvNum, tEqpAgtInfoTable );
    if ( !bRet )
    {
        EqpAgtLog( LOG_ERROR, "[TrapIpsOfEqpAgt2Nms] ReadEqpAgtInfoTable wrong!\n" );
        return FALSE;
    }
    
    // pBuf �ṹ��
    // u8(N�� IP) + u32(��һ��IP) + ���� + u32(��N��IP)

    u32 dwTempIp = 0;
    s8  abyTrapIps[1024] = {'\0'};
    s8  abyBuf[1024] = {'\0'};
    s8 *pBuff = abyBuf;
    pBuff += sprintf(pBuff, "[TrapIpsOfEqpAgt2Nms] Trap server Ip Info: total num %d\n", byTrapSrvNum);

    // 1��u8(N�� IP)
    memcpy( abyTrapIps, &byTrapSrvNum, sizeof(u8) );
    // 2����¼N�� IP
    if ( 0 < byTrapSrvNum )
    {
        for ( u8 byLoop = 0; byLoop < byTrapSrvNum; byLoop++)
        {
            dwTempIp = htonl(tEqpAgtInfoTable[byLoop].GetTrapIp());
            pBuff += sprintf(pBuff, "No: %u, Ip(net): %x\n", byLoop, dwTempIp);
            // ȡIP����ֵ
            memcpy( abyTrapIps + sizeof(u8) + sizeof(u32) * byLoop, &dwTempIp, sizeof(u32) );
        }
    }
    *pwBufLen = (u16)(sizeof(u8) + sizeof(u32) * byTrapSrvNum); 
    memcpy( pBuf, abyTrapIps, *pwBufLen );
    EqpAgtLog( LOG_DETAIL, abyBuf);
    return TRUE;
}


/*====================================================================
    ������      : TrapIpsOfNms2EqpAgt
    ����        : Trap Ip��Ϣ Nms ----> EqpAgt
    �㷨ʵ��    : ��Ϊ���󣬴˴�������trap ip���������Ĭ��ֵ
    ����ȫ�ֱ���:
    �������˵��: void * pBuf:       ��������
                  u16 *pwBufLen:     ���ݳ���
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtCfg::TrapIpsOfNms2EqpAgt( void *pBuf, u16 *pwBufLen )
{
    // pBuf �ṹ��
    // u8(N�� IP) + u32(��һ��IP) + ���� + u32(��N��IP)

    BOOL32 bRet = TRUE;
    u32 dwTempIp = 0;
    s8  abyTrapIps[sizeof(u32)*MAXNUM_EQPAGTINFO_LIST+1+1] = {'\0'};
    u8 byLoop = 0;
    u8 byTrapSrvNum = 0;
    TEqpAgtInfo tEqpAgtInfoTable[MAXNUM_EQPAGTINFO_LIST];	// EqpAgt��Ϣ��    
    memcpy( abyTrapIps, pBuf, *pwBufLen );
    memcpy( &byTrapSrvNum, abyTrapIps, sizeof(byTrapSrvNum));
    EqpAgtLog( LOG_KEYSTATUS, "[TrapIpsOfNms2EqpAgt] Total Trap server num :%d\n", byTrapSrvNum);

    // ���IP��¼����Ϊ0��
    for ( byLoop = 0; byLoop < m_byTrapSrvNum; byLoop++)
    {
        tEqpAgtInfoTable[byLoop].SetTrapIp( 0 );
    }
    
    // ��¼IP
    for ( byLoop = 0; byLoop < byTrapSrvNum; byLoop++)
    {
        memcpy( &dwTempIp, abyTrapIps + sizeof(u8) + sizeof(u32) * byLoop, sizeof(dwTempIp));
        dwTempIp = ntohl( dwTempIp );
        EqpAgtLog( LOG_KEYSTATUS, "[TrapIpsOfNms2EqpAgt] Trap server No: %u, Ip(host order): %x \n", byLoop, dwTempIp);
        tEqpAgtInfoTable[byLoop].SetTrapIp( dwTempIp );
    }

    bRet = WriteEqpAgtInfoTable( byTrapSrvNum, tEqpAgtInfoTable );
    if( !bRet )
    {
        EqpAgtLog( LOG_ERROR, "[TrapIpsOfNms2EqpAgt] Write EqpAgt info table failed!\n" );
        return FALSE;
    }

    return TRUE;
}

/*====================================================================
    ������      : GetTrapServerNum
    ����        : ��ȡTrap��������
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: void
    ����ֵ˵��  : u8 m_byTrapSrvNum Trap��������
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
u8 CEqpAgtCfg::GetTrapServerNum( void )
{
    return m_byTrapSrvNum;
}

/*====================================================================
    ������      : GetTrapTarget
    ����        : ȡĳһTrap��������Ӧ��Trap��Ϣ�ṹ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: u8 byIdx              ���
    �������˵��: TTarget& tTrapTarget  ��Ӧ��Trap��Ϣ�ṹ
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtCfg::GetTrapTarget( u8 byIdx, TTarget& tTrapTarget )
{
    tTrapTarget.dwIp = m_atEqpAgtInfoTable[byIdx].GetTrapIp();
    tTrapTarget.wPort = m_atEqpAgtInfoTable[byIdx].GetTrapPort();
    memcpy( tTrapTarget.achCommunity, 
        m_atEqpAgtInfoTable[byIdx].GetReadCom(), 
        strlen(m_atEqpAgtInfoTable[byIdx].GetReadCom()) );
    return;
}

/*====================================================================
    ������      : GetSnmpParam
    ����        : ȡSNMP����
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: TSnmpAdpParam& tParam   snmp����
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtCfg::GetSnmpParam( TSnmpAdpParam& tParam ) const
{
    BOOL32 bRet = FALSE;
    memset( &tParam, 0, sizeof(tParam) );
    
    if( 0 != m_byTrapSrvNum )
    {
        tParam.dwLocalIp = htonl(m_atEqpAgtInfoTable[0].GetTrapIp());
        
        TEqpAgtInfo tEqpAgtInfo;
        memcpy( &tEqpAgtInfo, &m_atEqpAgtInfoTable[0], sizeof(tEqpAgtInfo) );
        
        strncpy( tParam.achReadCommunity, tEqpAgtInfo.GetReadCom(), MAX_COMMUNITY_LEN );
        
        tParam.achReadCommunity[MAX_COMMUNITY_LEN-1] = '\0';
        
        strncpy( tParam.achWriteCommunity, tEqpAgtInfo.GetWriteCom(), MAX_COMMUNITY_LEN );
        
        tParam.achWriteCommunity[MAX_COMMUNITY_LEN-1] = '\0';
        
        tParam.wGetSetPort = m_atEqpAgtInfoTable[0].GetGSPort();
        tParam.wTrapPort = m_atEqpAgtInfoTable[0].GetTrapPort();
        
        EqpAgtLog( LOG_KEYSTATUS, "GetSnmpParam succeed!\n");
       
        bRet = TRUE;
    }
    else
    {
        memset(&tParam, 0, sizeof(tParam));
    }
    
	return bRet;
}

/*====================================================================
    ������      : HasSnmpNms
    ����        : �Ƿ�����������
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtCfg::HasSnmpNms( void ) const
{
    return ( 0 != m_byTrapSrvNum );
}


/*====================================================================
    ������      : OrganizeEqpAgtInfoTable
    ����        : ��֯����д�ļ��ĵ���EqpAgt����������
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: u8 byIdx                  ���
                  TEqpAgtInfo* pEqpAgtInfo  ����EqpAgt��������������
    �������˵��: s8* ptInfTobeWrited       ����д�ļ��ĵ���EqpAgt��������������
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE 
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtCfg::OrganizeEqpAgtInfoTable(u8 byIdx,
                                           TEqpAgtInfo* pEqpAgtInfo,
                                           s8* ptInfTobeWrited )
{
    if ( MAXNUM_EQPAGTINFO_LIST <= byIdx 
        || NULL == pEqpAgtInfo 
        || NULL == ptInfTobeWrited )
    {
        BOOL32 bRet1 = (NULL == pEqpAgtInfo);
        BOOL32 bRet2 = (NULL == ptInfTobeWrited);
        EqpAgtLog( LOG_ERROR, "[OrganizeEqpAgtInfoTable] organize error,byIdx:%d,pEqpAgtInfo Null?:%d,ptInfTobeWrited Null?:%d",
            byIdx, bRet1, bRet2 );
        return FALSE;
    }
    
    s8  achSnmpIp[32];
    s8  achEntryStr[32];
    memset(achSnmpIp, '\0', sizeof(achSnmpIp));
    memset(achEntryStr, '\0', sizeof(achEntryStr));    
    
    sprintf(achEntryStr, "%s%d", ENTRY_KEY, byIdx);
    GetIpFromU32( achSnmpIp, pEqpAgtInfo->GetTrapIp() );
    
    sprintf(ptInfTobeWrited, 
        "%s = %s\t%s\t%s\t%d\t%d\n", 
        achEntryStr,
        achSnmpIp,
        pEqpAgtInfo->GetReadCom(), 
        pEqpAgtInfo->GetWriteCom(),
        pEqpAgtInfo->GetGSPort(),
        pEqpAgtInfo->GetTrapPort() );
    return TRUE;
} 

/*====================================================================
    ������      : ReadTrapTable
    ����        : �������ļ�
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: LPCSTR lpszCfgFileFullPath  �ļ�ȫ·��
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE 
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtCfg::ReadTrapTable( LPCSTR lpszCfgFileFullPath )
{
    // �ж������ļ��Ƿ���ڡ��������ļ�
    FILE* hFileR = NULL;
    s32 nFopenErr = 0;
    hFileR = fopen( lpszCfgFileFullPath, "r" );
    if( NULL != hFileR ) // exist
    {
        // �������ļ�        
        s8    achSeps[] = " \t";       // �ָ���
        s8    *pchToken = NULL;
        s32    nMemEntryNum = 0;
        s8     achPortStr[32];        // ��ʱ����ַ���
        memset(achPortStr, '\0', sizeof(achPortStr));
        
        BOOL32 bSucceedRead = TRUE;
        BOOL32 bResult = GetRegKeyInt( lpszCfgFileFullPath, SECTION_EqpAgtInfoTable, 
                                     ENTRY_NUM, 0, &nMemEntryNum );
        if( !bResult ) 
        {
            EqpAgtLog( LOG_ERROR, "[ReadCfgFile] Wrong profile while reading %s %s!\n", 
                SECTION_EqpAgtInfoTable, ENTRY_NUM );
            bSucceedRead = FALSE;
        }

        // ����TRAP����ķ�����������Ϊ0�����ֱ���˳�
        m_byTrapSrvNum = (u8)nMemEntryNum;
        if( 0 < nMemEntryNum ) 
        {
            // ����ռ�
            s8* *ppszTable = new s8*[(u32)nMemEntryNum];
            if( NULL == ppszTable )
            {
                EqpAgtLog( LOG_ERROR, "[ReadCfgFile] Error to alloc memory!\n" );
                return FALSE;
            }
            
            u32  dwLoop = 0;
            for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
            {
                ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
                if( NULL == ppszTable[dwLoop] )
                {
                    FREE_TABLE_MEM( ppszTable, dwLoop );
                    return FALSE;
                }
            }
            
            // ȡ������
			bResult = GetRegKeyStringTable(lpszCfgFileFullPath, SECTION_EqpAgtInfoTable,
				"fail", ppszTable, (u32 *)&nMemEntryNum, MAX_VALUE_LEN + 1 );
			if( !bResult)
			{
				FREE_TABLE_MEM( ppszTable, nMemEntryNum );
				EqpAgtLog( LOG_ERROR, "[ReadCfgFile] Wrong profile while reading %s snmp infomation!\n", SECTION_EqpAgtInfoTable );				
				m_byTrapSrvNum = 0;
				return FALSE;
			}
            
            // �����ִ�
            for( dwLoop = 0; dwLoop < m_byTrapSrvNum; dwLoop++ )
            {
                // IpAddr 
                pchToken = strtok( ppszTable[dwLoop], achSeps );
                if( NULL == pchToken)
                {
                    EqpAgtLog( LOG_ERROR, "[ReadCfgFile] Wrong profile while reading %s !\n", FIELD_TrapSrvIpAddr );				
                    bSucceedRead = FALSE;
                    continue;
                }
                else
                {
                    m_atEqpAgtInfoTable[dwLoop].SetTrapIp( ntohl(INET_ADDR( pchToken )) );
                }
                
                // ����ͬ��
                pchToken = strtok( NULL, achSeps );
                if( NULL == pchToken )
                {
                    EqpAgtLog( LOG_ERROR, "[ReadCfgFile] Wrong profile while reading %s !\n", FIELD_ReadCommunity );				
                    bSucceedRead = FALSE;
                    continue;
                }
                else
                {
                    m_atEqpAgtInfoTable[dwLoop].SetReadCom( pchToken );
                }
                
                // д��ͬ��
                pchToken = strtok(NULL, achSeps);
                if(NULL == pchToken)
                {
                    EqpAgtLog( LOG_ERROR, "[ReadCfgFile] Wrong profile while reading %s !\n", FIELD_WriteCommunity );
                    bSucceedRead = FALSE;
                    continue;
                }
                else
                {
                    m_atEqpAgtInfoTable[dwLoop].SetWriteCom( pchToken );
                }
                
                // ȡ�����ö˿�
                pchToken = strtok(NULL, achSeps);
                if(NULL == pchToken)
                {
                    EqpAgtLog( LOG_ERROR, "[ReadCfgFile] Wrong profile while reading %s !\n", FIELD_GetSetPort );
                    m_atEqpAgtInfoTable[dwLoop].SetGSPort( SNMP_PORT );
                    bSucceedRead = FALSE;
                    continue;
                }
                else
                {
                    strncpy( achPortStr, pchToken, MAXLEN_PORT);
                    m_atEqpAgtInfoTable[dwLoop].SetGSPort( atoi(achPortStr) );
                }
                
                // Trap �˿�
                memset(achPortStr, '\0', MAXLEN_PORT);
                pchToken = strtok(NULL, achSeps);
                if(NULL == pchToken)
                {
                    EqpAgtLog( LOG_ERROR, "[ReadCfgFile] Wrong profile while reading %s !\n", FIELD_SendTrapPort );
                    m_atEqpAgtInfoTable[dwLoop].SetTrapPort( TRAP_PORT );
                    bSucceedRead = FALSE;
                    continue;
                }
                else
                {
                    strncpy(achPortStr, pchToken, MAXLEN_PORT);
                    m_atEqpAgtInfoTable[dwLoop].SetTrapPort( atoi(achPortStr) );
                }
            }
            // �ͷ���ʱ�ռ�
            FREE_TABLE_MEM( ppszTable, nMemEntryNum );
        }

        fclose( hFileR );
/*lint -save -esym(438, hFileR)*/
        hFileR = NULL;
/*lint -restore*/
        return bSucceedRead;
    }
    else
    {
        m_byTrapSrvNum = 0;

#ifdef _LINUX_
        nFopenErr = errno;	
#endif
        
#ifdef WIN32
        nFopenErr = GetLastError(); 
#endif
        //�������ļ�ʧ�ܵĴ���ź�ʱ��
        time_t tiCurTime = ::time(NULL);
        EqpAgtLog( LOG_ERROR, "[ReadCfgFile] error:%s,time:%s !\n", strerror(nFopenErr), ctime(&tiCurTime) );
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
void CEqpAgtCfg::CreateDir( LPCSTR pPathName )
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
    ������      : PretreatCfgFile
    ����        : ����Ĭ�������ļ�
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: LPCSTR lpszCfgPath   ����·��
                  LPCSTR lpszCfgName   �����ļ���
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE 
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtCfg::PretreatCfgFile( LPCSTR lpszCfgPath, LPCSTR lpszCfgName )
{
    // ����Ŀ¼
    CreateDir( lpszCfgPath );
    s8 achProfileName[EQPAGT_MAX_LENGTH] = {0};
    // eqpagt�����ļ���
    sprintf( achProfileName, "%s/%s", lpszCfgPath, lpszCfgName );

    FILE* hFileW = NULL;
    hFileW = fopen( achProfileName, "w" );
	if( NULL == hFileW )
    {
        EqpAgtLog( LOG_ERROR, "[PretreatCfgFile] Create eqpagtcfg file failed!\n" );
		return FALSE;	
	}
    
    // ����TRAP��ַĬ��ֵ
    BOOL32 bRet = SetRegKeyInt( achProfileName, SECTION_EqpAgtInfoTable, (s8*)ENTRY_NUM, 0 );
    if ( !bRet ) 
    {
        EqpAgtLog( LOG_ERROR, "[PretreatCfgFile] Write default EqpAgtInfoTable entrynum failed!\n" );
    }

    u16 wFuncNum = g_cEqpAgtCfgDefValueList.Size();
    if ( 0!= wFuncNum )
    {
        TEqpAgtCfgDefValue pfTempFunc = NULL;
        for( u16 wLoop = 0; wLoop < wFuncNum; wLoop++ )
        {
            bRet = g_cEqpAgtCfgDefValueList.GetEqpAgtCfgDefValue( wLoop, &pfTempFunc );
            if ( bRet )
            {
                (*pfTempFunc)( achProfileName );
            }
        }
    }

	fclose(hFileW);	
/*lint -save -esym(438, hFileW)*/
    hFileW = NULL;
/*lint -restore*/    
    return TRUE;
}

// ����
CEqpAgtCfgDefValueList::CEqpAgtCfgDefValueList():m_wEqpAgtCfgDefValueSize(0)
{
    m_pEqpAgtCfgDefValueHead = NULL;
}

// ����
CEqpAgtCfgDefValueList::~CEqpAgtCfgDefValueList()
{
    m_wEqpAgtCfgDefValueSize = 0;
    m_pEqpAgtCfgDefValueHead = NULL;
}

/*====================================================================
    ������      : GetEqpAgtCfgDefValue
    ����        : ��ȡ��Ӧ��ŵĻص������ṹ��Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: u16 wIdx                   ���
    �������˵��: TEqpAgtCfgDefValue* pfFunc     ��Ӧ�Ļص�����ָ��
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtCfgDefValueList::GetEqpAgtCfgDefValue( u16 wIdx, TEqpAgtCfgDefValue* pfFunc )
{
    EqpAgtCfgDefValue *pEqpAgtCfgDefValue = m_pEqpAgtCfgDefValueHead;
    while( NULL != pEqpAgtCfgDefValue ) 
    {
        if( wIdx == pEqpAgtCfgDefValue->m_wIdx ) 
        {
            *pfFunc = pEqpAgtCfgDefValue->m_pfFunc;
            return TRUE;
        } 
        pEqpAgtCfgDefValue = pEqpAgtCfgDefValue->m_pNext; 
    }

    EqpAgtLog( LOG_ERROR, "[GetEqpAgtCfgDefValue] Do not find right param match to the index %u !\n", wIdx );
    return FALSE;
}

/*====================================================================
    ������      : PushBack
    ����        : ������ĩβ�洢�ص������ṹ��Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: TEqpAgtCfgDefValue pfFunc �ص�����ָ��
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtCfgDefValueList::PushBack( TEqpAgtCfgDefValue pfFunc )
{
    if( NULL == pfFunc )
    {
        EqpAgtLog( LOG_ERROR, "[CEqpAgtCfgDefValueList::PushBack] The input param is null!\n" );
        return;
    }
    
    EqpAgtCfgDefValue *pNew = new EqpAgtCfgDefValue[1];
    if( NULL == pNew )
    {
        EqpAgtLog( LOG_ERROR, "[CEqpAgtCfgDefValueList::PushBack] Allocate pNew Memory failed!\n" );
        return;
    }
    pNew->m_pfFunc = pfFunc;
    pNew->m_wIdx = m_wEqpAgtCfgDefValueSize;
    m_wEqpAgtCfgDefValueSize++;
    EqpAgtCfgDefValue *pTail = m_pEqpAgtCfgDefValueHead;
    if ( NULL == pTail )
    {
        pTail = pNew;
        pTail->m_pNext = NULL;
    }
    else
    {
        while ( NULL != pTail->m_pNext )
        {
            pTail = pTail->m_pNext;
        }
        pTail->m_pNext = pNew;
        pTail = pTail->m_pNext;
        pTail->m_pNext = NULL;
    }
    
    if( NULL == m_pEqpAgtCfgDefValueHead )
    {
        m_pEqpAgtCfgDefValueHead = pTail;
    }    
    return;
}

/*====================================================================
    ������      : EqpAgtCfgDefValueOper
    ����        : ����EqpAgtCfg�����ļ�Ĭ��ֵ�Ļص�����ע��
    �㷨ʵ��    :
    ����ȫ�ֱ���: g_cEqpAgtCfgDefValueList
    �������˵��: TEqpAgtCfgDefValue pfFunc �ص�����ҵ�������ʵ��
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void EqpAgtCfgDefValueOper( TEqpAgtCfgDefValue pfFunc )
{
    // 1���ж��������Ƿ��Ѿ����ڶ�Ӧ����,��ֹ����ע��ͬһ����
    u16 wFuncNum = g_cEqpAgtCfgDefValueList.Size();
    BOOL32 bRet = TRUE;
    u16 wLoop = 0;
    if ( 0!= wFuncNum )
    {
        TEqpAgtCfgDefValue pfTempFunc = NULL;
        for( wLoop = 0; wLoop < wFuncNum; wLoop++ )
        {
            bRet = g_cEqpAgtCfgDefValueList.GetEqpAgtCfgDefValue( wLoop, &pfTempFunc );
            if ( bRet )
            {
                if( pfFunc == pfTempFunc ) 
                {                   
                    return;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                return;
            }
        } 
    }    
    
    // 2�������ڣ����
    g_cEqpAgtCfgDefValueList.PushBack( pfFunc );
    return;
}