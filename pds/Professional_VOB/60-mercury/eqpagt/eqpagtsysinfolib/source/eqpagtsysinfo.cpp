/*****************************************************************************
    ģ����      : EqpAgt
    �ļ���      : eqpagtsysinfo.cpp
    ����ļ�    : 
    �ļ�ʵ�ֹ���: ҵ����ģ��: EqpAgt ϵͳ��Ϣ 
    ����        : liaokang
    �汾        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    �޸ļ�¼:
    ��  ��      �汾        �޸���          �޸�����
    2012/06/18  4.7         liaokang        ����
******************************************************************************/
#include "eqpagtsysinfo.h"
#include "eqpagtsnmp.h"

CEqpAgtSysInfo	g_cEqpAgtSysInfo;

// ����
CEqpAgtSysInfo::CEqpAgtSysInfo()
{
/*lint -save -e1566 */
    Free();
/*lint -restore*/
}

// ����
CEqpAgtSysInfo::~CEqpAgtSysInfo()
{
/*lint -save -e1551 */
    Free();
/*lint -restore*/
}

/*====================================================================
    ������      : Free
    ����        : ���
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: void
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtSysInfo::Free(void)
{
    m_dwSysState = 0;
    m_dwEqpOsType = 0;
    m_dwEqpSubType = 0;
    m_byCpuStatus = 0;
    m_byMemStatus = 0;
    m_byDiskStatus = 0;
    memset( m_achSoftwareVer, 0, sizeof(m_achSoftwareVer) );
    memset( m_achHardwareVer, 0, sizeof(m_achHardwareVer) );
    memset( m_achCompileTime, 0, sizeof(m_achCompileTime) );
    memset( m_achPartionName, 0, sizeof(m_achPartionName) );
    memset( &m_tNetAdapterInfoAll, 0, sizeof(m_tNetAdapterInfoAll) );
    m_pfNmsSetSysState = NULL;
    m_pfNmsSetIpInfo = NULL;
}

/*====================================================================
    ������      : Initialize
    ����        : ��ʼ�����洢����ָ��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: TNmsSetSysState pfNmsSetSysState  ϵͳ״̬����ָ��
                  TNmsSetIpInfo pfNmsSetIpInfo      ip��Ϣ����ָ��
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtSysInfo::Initialize( TNmsSetSysState pfNmsSetSysState,
                                   TNmsSetIpInfo pfNmsSetIpInfo )
{
    if ( NULL == pfNmsSetSysState || NULL == pfNmsSetIpInfo )
    {
        EqpAgtLog( LOG_ERROR, "[Initialize] The input param is null!\n" );
        return FALSE;
    }
    
    m_pfNmsSetSysState = pfNmsSetSysState;
    m_pfNmsSetIpInfo = pfNmsSetIpInfo;
    return TRUE;
}



/*====================================================================
    ������      : SupportNmsGetBaseInfo
    ����        : ֧�����ܷ�������ȡϵͳ��Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: void
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtSysInfo::SupportNmsGetSysInfo( void )
{ 
    EqpAgtGetOper( NODE_COMMSYSSTATE, EqpAgtGetSysInfo );       // ϵͳ״̬
    EqpAgtGetOper( NODE_COMMSOFTWAREVER, EqpAgtGetSysInfo );    // ����汾��
    EqpAgtGetOper( NODE_COMMHARDWAREVER, EqpAgtGetSysInfo );    // Ӳ���汾��
    EqpAgtGetOper( NODE_COMMCOMPILETIME, EqpAgtGetSysInfo );    // ����ʱ��
    EqpAgtGetOper( NODE_COMMOSTYPE, EqpAgtGetSysInfo );         // �豸����ϵͳ   
    EqpAgtGetOper( NODE_COMMSUBTYPE, EqpAgtGetSysInfo );        // �豸������
    EqpAgtGetOper( NODE_COMMCPURATE, EqpAgtGetSysInfo );        // cpu ʹ����    
    EqpAgtGetOper( NODE_COMMMEMRATE, EqpAgtGetSysInfo );        // �ڴ� ʹ����   
    EqpAgtGetOper( NODE_COMMDISKRATE, EqpAgtGetSysInfo );       // ���̿ռ� ʹ����

    /************************************************************************/
    /*                ��ʱ���ε�������֧��  begin                           */
    /************************************************************************/ 
    /*
    EqpAgtGetOper( NODE_COMMIP, EqpAgtGetSysInfo );             // ip    
    EqpAgtGetOper( NODE_COMMMASKIP, EqpAgtGetSysInfo );         // ip mask   
    EqpAgtGetOper( NODE_COMMGATEWAYIP, EqpAgtGetSysInfo );      // gateway
    */
    /************************************************************************/
    /*                 ��ʱ���ε�������֧��  end                            */
    /************************************************************************/
}

/*====================================================================
    ������      : SupportNmsSetSysInfo
    ����        : ֧�����ܷ���������ϵͳ��Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: void
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtSysInfo::SupportNmsSetSysInfo( void )
{  
    EqpAgtSetOper( NODE_COMMSYSSTATE, EqpAgtSetSysInfo );       // ϵͳ״̬

    /************************************************************************/
    /*                ��ʱ���ε�������֧��  begin                           */
    /************************************************************************/       
    /*
    EqpAgtSetOper( NODE_COMMIP, EqpAgtSetSysInfo );             // ip    
    EqpAgtSetOper( NODE_COMMMASKIP, EqpAgtSetSysInfo );         // ip mask   
    EqpAgtSetOper( NODE_COMMGATEWAYIP, EqpAgtSetSysInfo );      // gateway
    */
    /************************************************************************/
    /*                 ��ʱ���ε�������֧��  end                            */
    /************************************************************************/
}

/*====================================================================
    ������      : SupportNmsTrapBaseInfo
    ����        : ֧��ɨ���쳣��ı��ϵͳ��Ϣ���ϱ������ܷ�����
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: u32 dwScanTimeSpan ɨ��ʱ����
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtSysInfo::SupportNmsTrapSysInfo( u32 dwScanTimeSpan )
{
    EqpAgtTrapOper( NODE_COMMCPURATE, EqpAgtTrapCpuRate, dwScanTimeSpan );      // cpu ʹ����   
    EqpAgtTrapOper( NODE_COMMMEMRATE, EqpAgtTrapMemRate, dwScanTimeSpan );      // �ڴ� ʹ����
    EqpAgtTrapOper( NODE_COMMDISKRATE, EqpAgtTrapDiskRate, dwScanTimeSpan );    // ���̿ռ� ʹ����
}

/*====================================================================
    ������      : SetSysState
    ����        : �洢ϵͳ״̬
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: u32 dwSysState ϵͳ״̬
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtSysInfo::SetSysState( u32 dwSysState )
{
    m_dwSysState = dwSysState;
}

/*====================================================================
    ������      : GetSysState
    ����        : ��ȡϵͳ״̬����һ������ EqpAgt ----> Nms ��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: void
    ����ֵ˵��  : u32 ϵͳ״̬
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
u32 CEqpAgtSysInfo::GetSysState( void )
{    
    return m_dwSysState;
}

/*====================================================================
    ������      : SysStateOfEqpAgt2Nms
    ����        : NMS����Get��ϵͳ״̬ EqpAgt ----> Nms
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: void * pBuf:       �������
                  u16 *pwBufLen:     ���ݳ���
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtSysInfo::SysStateOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    u32 dwSysState = htonl(m_dwSysState);
    *pwBufLen = sizeof(u32);
    memcpy( pBuf, &dwSysState, *pwBufLen);
}

/*====================================================================
    ������      : SysStateOfNms2EqpAgt
    ����        : Nms����ϵͳ״̬
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: void * pBuf  ��������
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtSysInfo::SysStateOfNms2EqpAgt(void * pBuf)
{
    if ( NULL == m_pfNmsSetSysState )
    {
        EqpAgtLog( LOG_ERROR, "[SysStateOfNms2EqpAgt] m_pfNmsSetSysState is Null!\n" );
        return FALSE;
    }
    u32 dwSysState =  *(u32*)pBuf;
    dwSysState = ntohl(dwSysState);
    (*m_pfNmsSetSysState)( dwSysState );
    return TRUE; 
}

/*====================================================================
    ������      : SetSoftwareVer
    ����        : �洢����汾��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: LPCSTR lpszSoftwareVer ����汾��
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtSysInfo::SetSoftwareVer( LPCSTR lpszSoftwareVer )
{
    if ( NULL == lpszSoftwareVer )
    {
        EqpAgtLog( LOG_ERROR, "[SetSoftwareVer] The input param is null!\n" );
        return FALSE;
    }

    memcpy( m_achSoftwareVer, lpszSoftwareVer, sizeof(m_achSoftwareVer) ); 
    m_achSoftwareVer[sizeof(m_achSoftwareVer)-1] = '\0';
    EqpAgtLog( LOG_DETAIL, "[SetSoftwareVer] Set software ver: %s!\n", m_achSoftwareVer );
    return TRUE;
}

/*====================================================================
    ������      : SetHardwareVer
    ����        : �洢Ӳ���汾��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: LPCSTR lpszHardwareVer Ӳ���汾��
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtSysInfo::SetHardwareVer( LPCSTR lpszHardwareVer )
{
    if ( NULL == lpszHardwareVer )
    {
        EqpAgtLog( LOG_ERROR, "[SetHardwareVer] The input param is null!\n" );
        return FALSE;
    }
    
    memcpy( m_achHardwareVer, lpszHardwareVer, sizeof(m_achHardwareVer) ); 
    m_achHardwareVer[sizeof(m_achHardwareVer)-1] = '\0';
    EqpAgtLog( LOG_DETAIL, "[SetHardwareVer] Set hardware ver: %s!\n", m_achHardwareVer );
    return TRUE;
}

/*====================================================================
    ������      : SetCompileTime
    ����        : �洢����ʱ��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: LPCSTR lpszCompileTime ����ʱ��
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtSysInfo::SetCompileTime( LPCSTR lpszCompileTime )
{
    if ( NULL == lpszCompileTime )
    {
        EqpAgtLog( LOG_ERROR, "[SetCompileTime] The input param is null!\n" );
        return FALSE;
    }
    
    memcpy( m_achCompileTime, lpszCompileTime, sizeof(m_achCompileTime) ); 
    m_achCompileTime[sizeof(m_achCompileTime)-1] = '\0';
    EqpAgtLog( LOG_DETAIL, "[SetCompileTime] Set compile time: %s!\n", m_achCompileTime );
    return TRUE;
}

/*====================================================================
    ������      : SetEqpOsType
    ����        : �洢�豸����ϵͳ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: u32 dwEqpOsType ����ϵͳ
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtSysInfo::SetEqpOsType( u32 dwEqpOsType )
{
    m_dwEqpOsType = dwEqpOsType;
    EqpAgtLog( LOG_DETAIL, "[SetEqpOsType] Set eqp OS type: %d!\n", m_dwEqpOsType );
}

/*====================================================================
    ������      : SetEqpSubType
    ����        : �洢�豸������
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: u32 dwEqpSubType �豸������
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtSysInfo::SetEqpSubType( u32 dwEqpSubType )
{
    m_dwEqpSubType = dwEqpSubType;
    EqpAgtLog( LOG_DETAIL, "[SetEqpSubType] Set eqp sub type: %d!\n", m_dwEqpSubType );
}

/*====================================================================
    ������      : GetSoftwareVer
    ����        : ��ȡ����汾��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: LPSTR lpszSoftwareVer ����汾��
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtSysInfo::GetSoftwareVer( LPSTR lpszSoftwareVer )
{
    if ( NULL == lpszSoftwareVer )
    {
        EqpAgtLog( LOG_ERROR, "[GetSoftwareVer] The input param is null!\n" );
        return FALSE;
    }    
    memcpy( lpszSoftwareVer, m_achSoftwareVer, sizeof(m_achSoftwareVer) ); 
    return TRUE;
}

/*====================================================================
    ������      : GetHardwareVer
    ����        : ��ȡӲ���汾��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: LPSTR lpszHardwareVer Ӳ���汾��
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtSysInfo::GetHardwareVer( LPSTR lpszHardwareVer )
{
    if ( NULL == lpszHardwareVer )
    {
        EqpAgtLog( LOG_ERROR, "[GetSoftwareVer] The input param is null!\n" );
        return FALSE;
    }    
    memcpy( lpszHardwareVer, m_achHardwareVer, sizeof(m_achHardwareVer) ); 
    return TRUE;
}

/*====================================================================
    ������      : GetCompileTime
    ����        : ��ȡ����ʱ��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: LPSTR lpszCompileTime ����ʱ��
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtSysInfo::GetCompileTime( LPSTR lpszCompileTime )
{
    if ( NULL == lpszCompileTime )
    {
        EqpAgtLog( LOG_ERROR, "[GetSoftwareVer] The input param is null!\n" );
        return FALSE;
    }    
    memcpy( lpszCompileTime, m_achCompileTime, sizeof(m_achCompileTime) ); 
    return TRUE;
}

/*====================================================================
    ������      : GetEqpOsType
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
u32 CEqpAgtSysInfo::GetEqpOsType( void )
{    
    return m_dwEqpOsType;
}

/*====================================================================
    ������      : GetEqpSubType
    ����        : ��ȡ�豸������
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: void
    ����ֵ˵��  : u32 �豸������
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
u32 CEqpAgtSysInfo::GetEqpSubType( void )
{
    return m_dwEqpSubType;
}

/*====================================================================
    ������      : SoftwareVerOfEqpAgt2Nms
    ����        : ����汾��  EqpAgt ----> Nms
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: void * pBuf:       �������
                  u16 *pwBufLen:     ���ݳ���
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtSysInfo::SoftwareVerOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    // pBufֵ
    *pwBufLen = (u16)(strlen(m_achSoftwareVer)); 
    memcpy( pBuf, m_achSoftwareVer, *pwBufLen );
    EqpAgtLog( LOG_DETAIL, "[SoftwareVerOfEqpAgt2Nms] pBuf: %s, len: %u !\n", pBuf, *pwBufLen );
}

/*====================================================================
    ������      : HardwareVerOfEqpAgt2Nms
    ����        : Ӳ���汾��  EqpAgt ----> Nms
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: void * pBuf:       �������
                  u16 *pwBufLen:     ���ݳ���
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtSysInfo::HardwareVerOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    // pBufֵ
    *pwBufLen = (u16)(strlen(m_achHardwareVer)); 
    memcpy( pBuf, m_achHardwareVer, *pwBufLen );
    EqpAgtLog( LOG_DETAIL, "[HardwareVerOfEqpAgt2Nms] pBuf: %s, len: %u !\n", pBuf, *pwBufLen );
}

/*====================================================================
    ������      : CompileTimeOfEqpAgt2Nms
    ����        : ����ʱ��    EqpAgt ----> Nms
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: void * pBuf:       �������
                  u16 *pwBufLen:     ���ݳ���
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtSysInfo::CompileTimeOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    // pBufֵ
    *pwBufLen = (u16)(strlen(m_achCompileTime)); 
    memcpy( pBuf, m_achCompileTime, *pwBufLen );
    EqpAgtLog( LOG_DETAIL, "[CompileTimeOfEqpAgt2Nms] pBuf: %s, len: %u !\n", pBuf, *pwBufLen );
}

/*====================================================================
    ������      : EqpOsTypeOfEqpAgt2Nms
    ����        : �豸����ϵͳEqpAgt ----> Nms
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: void * pBuf:       �������
                  u16 *pwBufLen:     ���ݳ���
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtSysInfo::EqpOsTypeOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    // pBufֵ
    *pwBufLen = (u16)(sizeof(u32)); 
    u32 dwEqpOsType = htonl( m_dwEqpOsType );
    memcpy( pBuf, &dwEqpOsType, *pwBufLen );
}

/*====================================================================
    ������      : EqpSubTypOfEqpAgt2Nms
    ����        : �豸������  EqpAgt ----> Nms
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: void * pBuf:       �������
                  u16 *pwBufLen:     ���ݳ���
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtSysInfo::EqpSubTypOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    // pBufֵ
    *pwBufLen = (u16)(sizeof(u32)); 
    u32 dwEqpSubType = htonl( m_dwEqpSubType );
    memcpy( pBuf, &dwEqpSubType, *pwBufLen );
} 

/*====================================================================
    ������      : SetDiskPartionName
    ����        : �洢���ô��̷�����
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��:LPSTR lpszPartionName ������ ����󳤶� 255��
                                        �磺win32 : "c:"
					                    Linux : "/"
    ����ֵ˵��  : TRUE   �ɹ�
                  FALSE  ʧ��
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtSysInfo::SetDiskPartionName(LPCSTR lpszDiskPartionName)
{
    if ( NULL == lpszDiskPartionName )
    {
        EqpAgtLog( LOG_ERROR, "[SetDiskPartionName] Null param!\n" );
        return FALSE;
    }
    strncpy( m_achPartionName, lpszDiskPartionName, sizeof(m_achPartionName) );
    m_achPartionName[sizeof(m_achPartionName) - 1] = '\0';
    return TRUE;
}

/*====================================================================
    ������      : GetCpuInfo
    ����        : ��ȡcpu��Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��:
    �������˵��: u8& byCpuRate  cpuʹ���ʣ��ٷֱȣ�
    ����ֵ˵��  : TRUE   �ɹ�
                  FALSE  ʧ��
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtSysInfo::GetCpuInfo( TCpuInfo* ptCpuInfo )
{
    // Ŀǰͨ��OSP��ȡcpuʹ���ʣ�����һ��ֵ 
    TOspCpuInfo tCpuInfo;
    memset( &tCpuInfo, 0, sizeof(tCpuInfo) );
    if ( !OspGetCpuInfo( &tCpuInfo ) )
    {
        EqpAgtLog( LOG_ERROR, "[GetCpuRate] Get cpu rate failed!\n" );
        return FALSE;
    }
    ptCpuInfo->m_byCpuAllocRate = 100 - tCpuInfo.m_byIdlePercent;
    return TRUE;
}

/*====================================================================
    ������      : GetMemInfo
    ����        : ��ȡ�ڴ���Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: TMemInfo* ptMemInfo �ڴ���Ϣ
    ����ֵ˵��  : TRUE   �ɹ�
                  FALSE  ʧ��
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtSysInfo::GetMemInfo( TMemInfo* ptMemInfo )
{
    TOspMemInfo tOspMemInfo;
    memset( &tOspMemInfo, 0, sizeof(tOspMemInfo) );
    if ( !OspGetMemInfo( &tOspMemInfo ) )
    {
        EqpAgtLog( LOG_ERROR, "[GetMemRate] Get mem rate failed!\n" );
        return FALSE;
    }
    ptMemInfo->m_dwMemAllocSize = tOspMemInfo.m_dwAllocSize;
    ptMemInfo->m_dwMemPhysicsSize = tOspMemInfo.m_dwPhysicsSize;

    return TRUE;
}

/*====================================================================
    ������      : GetDiskInfo
    ����        : ��ȡ���̿ռ���Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: LPSTR lpszPartionName ������ ����󳤶� 255��
                                    �磺win32 : "c:"
					                    Linux : "/"
    �������˵��: TDiskInfo* ptDiskInfo ���̿ռ���Ϣ
    ����ֵ˵��  : TRUE   �ɹ�
                  FALSE  ʧ��
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtSysInfo::GetDiskInfo( LPSTR lpszPartionName, 
                                    TDiskInfo* ptDiskInfo )
{
    TOspDiskInfo tOspDiskInfo;
    memset( &tOspDiskInfo, 0, sizeof(tOspDiskInfo) );
    if ( !OspGetDiskInfo( lpszPartionName, &tOspDiskInfo ) )
    {
        EqpAgtLog( LOG_ERROR, "[GetDiskInfo] Get disk info failed!\n" );
        return FALSE;
    }
    ptDiskInfo->m_dwDiskAllocSize = tOspDiskInfo.m_dwUsedSize;
    ptDiskInfo->m_dwDiskPhysicsSize = tOspDiskInfo.m_dwPhysicsSize;

    return TRUE;
}

/*====================================================================
    ������      : GetCpuAllInfo
    ����        : ��ȡcpu��ǰ��������Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��:
    �������˵��: TCpuAllInfo* ptCpuAllInfo  cpu������Ϣ
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtSysInfo::GetCpuAllInfo( TCpuAllInfo* ptCpuAllInfo )
{
    if ( NULL == ptCpuAllInfo )
    {
        EqpAgtLog( LOG_ERROR, "[GetCpuAllInfo] Null param!\n" );
        return FALSE;
    }

    TCpuInfo tCpuInfo;
    memset( &tCpuInfo, 0, sizeof(tCpuInfo) );
    if ( !GetCpuInfo( &tCpuInfo ) )
    {
        EqpAgtLog( LOG_ERROR, "[GetCpuAllInfo] GetCpuInfo wrong!\n" );
        return FALSE;
    }

    ptCpuAllInfo->m_byCpuStatus = m_byCpuStatus;
    ptCpuAllInfo->tCpuInfo.m_byCpuAllocRate = tCpuInfo.m_byCpuAllocRate; 
    return TRUE;
}

/*====================================================================
    ������      : GetMemAllInfo
    ����        : ��ȡ�ڴ浱ǰ��������Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: TMemAllInfo* ptMemAllInfo  �ڴ�������Ϣ
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtSysInfo::GetMemAllInfo( TMemAllInfo* ptMemAllInfo )
{

    if ( NULL == ptMemAllInfo )
    {
        EqpAgtLog( LOG_ERROR, "[GetMemAllInfo] Null param!\n" );
        return FALSE;
    }

    TMemInfo tMemInfo;
    memset( &tMemInfo, 0, sizeof(tMemInfo) );
    if ( !GetMemInfo( &tMemInfo ) )
    {
        EqpAgtLog( LOG_ERROR, "[GetMemAllInfo] GetMemInfo wrong!\n" );
        return FALSE; 
    }
    
    ptMemAllInfo->m_byMemStatus = m_byMemStatus;
    ptMemAllInfo->tMemInfo.m_dwMemAllocSize = tMemInfo.m_dwMemAllocSize;
    ptMemAllInfo->tMemInfo.m_dwMemPhysicsSize = tMemInfo.m_dwMemPhysicsSize;

    return TRUE;
}

/*====================================================================
    ������      : GetDiskAllInfo
    ����        : ��ȡ���̿ռ䵱ǰ��������Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: TDiskAllInfo* ptDiskAllInfo  ���̿ռ�������Ϣ
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtSysInfo::GetDiskAllInfo( TDiskAllInfo* ptDiskAllInfo )
{
    if ( NULL == ptDiskAllInfo )
    {
        EqpAgtLog( LOG_ERROR, "[GetDiskAllInfo] Null param!\n" );
        return FALSE;
    }

    TDiskInfo tDiskInfo;
    memset( &tDiskInfo, 0, sizeof(tDiskInfo) );
    if ( !GetDiskInfo( m_achPartionName, &tDiskInfo ) )
    {
        EqpAgtLog( LOG_ERROR, "[GetDiskAllInfo] GetDiskInfo wrong!\n" );
        return FALSE; 
    }

    memcpy( ptDiskAllInfo->m_achPartionName, m_achPartionName, sizeof(ptDiskAllInfo->m_achPartionName) );
    ptDiskAllInfo->m_byDiskStatus = m_byDiskStatus;
    ptDiskAllInfo->tDiskInfo.m_dwDiskAllocSize = tDiskInfo.m_dwDiskAllocSize;
    ptDiskAllInfo->tDiskInfo.m_dwDiskPhysicsSize = tDiskInfo.m_dwDiskPhysicsSize;
    return TRUE;
}

/*====================================================================
    ������      : CpuRateOfEqpAgt2Nms
    ����        : ��ȡcpu��Ϣ����������֯Ϊ��Ӧ�ڵ�buf�ṹ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: void *pBuf       ���̿ռ���Ϣ����֯Ϊ��Ӧ�ڵ�buf�ṹ��
                  u16  *pwBufLen   ���ݳ���
    ����ֵ˵��  : TRUE   �ɹ�
                  FALSE  ʧ��
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtSysInfo::CpuRateOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    // pBuf �ṹ��
    // u8(N�� cpu) + u8(��һ�� cpuʹ����) + ���� + u8(��N�� cpuʹ����)
    // ʵ�ʣ�Ŀǰͨ��OSP��ȡcpuʹ����:һ�� + �ٷֱ� ��
    // ��ȡCPUռ������Ϣ
    TCpuInfo tCpuInfo;
    memset( &tCpuInfo, 0, sizeof(tCpuInfo) );
    u8  byCpuNum = 1;
    s8  abyCpuInfo[3] = {'\0'};
    if ( !GetCpuInfo( &tCpuInfo ) )
    {
        return FALSE;
    }

    // pBufֵ
    memcpy( abyCpuInfo, &byCpuNum, sizeof(u8) );
    memcpy( abyCpuInfo + sizeof(u8), &tCpuInfo.m_byCpuAllocRate, sizeof(u8) );
    *pwBufLen = (u16)(sizeof(u8) + sizeof(u8)); 
    memcpy( pBuf, abyCpuInfo, *pwBufLen );
    EqpAgtLog( LOG_DETAIL, "[CpuRateOfEqpAgt2Nms] Cpu AllocRate %d !\n",tCpuInfo.m_byCpuAllocRate );
    return TRUE;
}

/*====================================================================
    ������      : MemRateOfEqpAgt2Nms
    ����        : ��ȡ�ڴ���Ϣ����������֯Ϊ��Ӧ�ڵ�buf�ṹ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: void *pBuf       ���̿ռ���Ϣ����֯Ϊ��Ӧ�ڵ�buf�ṹ��
                  u16  *pwBufLen   ���ݳ���
    ����ֵ˵��  : TRUE   �ɹ�
                  FALSE  ʧ��
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtSysInfo::MemRateOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    // pBuf �ṹ��
    // ��1���ֽڱ�ʶ�ڴ浥λ��1����KB 2����MB  3����GB  4����TB��
    // ��2-5���ֽڴ���ǰʹ���ڴ棨������
    // ��6-9���ֽڴ����ܹ��ж����ڴ棨������
    u8  byMemUnitType = 0;
    TMemInfo tMemInfo;
    memset( &tMemInfo, 0, sizeof(tMemInfo) );
    s8  achMemInfo[10] = {'\0'};
    if ( !GetMemInfo( &tMemInfo ) )
    {  
        return FALSE; 
    }

    // pBufֵ
    byMemUnitType = 2;  // MB
    u32 dwMemAllocSize = tMemInfo.m_dwMemAllocSize>>10;      // KB ----> MB
    u32 dwMemPhysicsSize = tMemInfo.m_dwMemPhysicsSize>>10;  // KB ----> MB
    EqpAgtLog( LOG_DETAIL, "[MemRateOfEqpAgt2Nms] Memory Info: UsedSize %d (MB), TotalSize %d (MB)\n",
        dwMemAllocSize, dwMemPhysicsSize);
    dwMemAllocSize = htonl(dwMemAllocSize);
    dwMemPhysicsSize = htonl(dwMemPhysicsSize);
    memcpy( achMemInfo, &byMemUnitType, sizeof(u8) );
    memcpy( achMemInfo + sizeof(u8), &dwMemAllocSize, sizeof(u32) );
    memcpy( achMemInfo + sizeof(u8) + sizeof(u32), &dwMemPhysicsSize, sizeof(u32) );
    *pwBufLen = (u16)(sizeof(u8) + sizeof(u32) + sizeof(u32)); 
    memcpy( pBuf, achMemInfo, *pwBufLen );
    return TRUE;
}

/*====================================================================
    ������      : DiskRateOfEqpAgt2Nms
    ����        : ��ȡ���̿ռ���Ϣ����������֯Ϊ��Ӧ�ڵ�buf�ṹ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: void *pBuf       ���̿ռ���Ϣ����֯Ϊ��Ӧ�ڵ�buf�ṹ��
                  u16  *pwBufLen   ���ݳ���
    ����ֵ˵��  : TRUE   �ɹ�
                  FALSE  ʧ��
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtSysInfo::DiskRateOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    // pBuf �ṹ��
    // ��1���ֽڱ�ʶ���̿ռ䵥λ��1����KB 2����MB  3����GB  4����TB��
    // ��2-5���ֽڴ���ǰʹ�ô��̿ռ䣨������
    // ��6-9���ֽڴ����ܹ��ж��ٴ��̿ռ䣨������
    u8  byDiskUnitType = 0;
    TDiskInfo tDiskInfo;
    memset( &tDiskInfo, 0, sizeof(tDiskInfo) );
    s8  achDiskInfo[10] = {'\0'};
    if ( !GetDiskInfo( m_achPartionName, &tDiskInfo ) )
    {  
        return FALSE; 
    }
    
    // pBufֵ
    byDiskUnitType = 3;  // GB
    u32 dwDiskAllocSize = tDiskInfo.m_dwDiskAllocSize>>10;      // MB ----> GB
    u32 dwDiskPhysicsSize = tDiskInfo.m_dwDiskPhysicsSize>>10;  // MB ----> GB
    EqpAgtLog( LOG_DETAIL, "[DiskRateOfEqpAgt2Nms] Disk %s Info: UsedSize %d (GB), TotalSize %d (GB)\n", 
        m_achPartionName, dwDiskAllocSize, dwDiskPhysicsSize);
    dwDiskAllocSize = htonl(dwDiskAllocSize);
    dwDiskPhysicsSize = htonl(dwDiskPhysicsSize);
    memcpy( achDiskInfo, &byDiskUnitType, sizeof(u8) );
    memcpy( achDiskInfo + sizeof(u8), &dwDiskAllocSize, sizeof(u32) );
    memcpy( achDiskInfo + sizeof(u8) + sizeof(u32), &dwDiskPhysicsSize, sizeof(u32) );
    *pwBufLen = (u16)(sizeof(u8) + sizeof(u32) + sizeof(u32)); 
    memcpy( pBuf, achDiskInfo, *pwBufLen );
    return TRUE;
}

/*====================================================================
    ������      : ScanCpuInfo
    ����        : ɨ��cpu
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: LPSTR lpszCpuInfo2Trap  cpu�澯��Ϣ����֯Ϊ��Ӧ�ڵ�buf�ṹ��
                  u16& wCpuInfoLen        �澯���ݳ���
    ����ֵ˵��  : NO_ALARM   �޸澯
                  ON_ALARM   �澯����
                  OFF_ALARM  �澯�ָ�
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
u8 CEqpAgtSysInfo::ScanCpuInfo( LPSTR lpszCpuInfo2Trap, u16& wCpuInfoLen )
{
    u8 byRet = NO_ALARM;

    // pBuf �ṹ��
    // u8(N�� cpu) + u8(��һ�� cpuʹ����) + ���� + u8(��N�� cpuʹ����)
    // ʵ�ʣ�Ŀǰͨ��OSP��ȡcpuʹ����:һ�� + �ٷֱ� ��
    TCpuInfo tCpuInfo;
    memset( &tCpuInfo, 0, sizeof(tCpuInfo) );
    u8  byCpuNum = 1;
    s8  achCpuInfo[3] = {'\0'};
    wCpuInfoLen = 2;
    if ( !GetCpuInfo( &tCpuInfo ) )
    {
        return FALSE;
    }

    // Cpuռ���ʳ���85%������¼Ϊ����״̬
    if ( tCpuInfo.m_byCpuAllocRate > CPU_THRESHOLD && m_byCpuStatus == STATUS_NORMAL)
    {
        m_byCpuStatus = STATUS_ABNORMAL;
        byRet = ON_ALARM;
    }    
    // Cpuռ����δ����85%������¼Ϊ�쳣״̬
    if ( ( tCpuInfo.m_byCpuAllocRate <= CPU_THRESHOLD) && ( m_byCpuStatus == STATUS_ABNORMAL) )
    {
        m_byCpuStatus = STATUS_NORMAL;
        byRet = OFF_ALARM;
    }    
    EqpAgtLog( LOG_DETAIL, "[ScanCpuInfo] Cpu AllocRate %d, Cpu status is %d <0:normal,1:abnormal>, Alarm: %d <0:no alarm,1:on alarm,2:off alarm>!\n",
        tCpuInfo.m_byCpuAllocRate, m_byCpuStatus, byRet );

    memcpy( achCpuInfo, &byCpuNum, sizeof(u8) );
    memcpy( achCpuInfo + sizeof(u8), &tCpuInfo.m_byCpuAllocRate, sizeof(u8) );
    memcpy( lpszCpuInfo2Trap, achCpuInfo, wCpuInfoLen );

    return byRet;
}

/*====================================================================
    ������      : ScanMemInfo
    ����        : ɨ���ڴ�
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: LPSTR lpszMemInfo2Trap  �ڴ�澯��Ϣ����֯Ϊ��Ӧ�ڵ�buf�ṹ��
                  u16& wMemInfoLen        �澯���ݳ���
    ����ֵ˵��  : NO_ALARM   �޸澯
                  ON_ALARM   �澯����
                  OFF_ALARM  �澯�ָ�
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
u8 CEqpAgtSysInfo::ScanMemInfo( LPSTR lpszMemInfo2Trap, u16& wMemInfoLen )
{  
    u8 byRet = NO_ALARM;

    // pBuf �ṹ��
    // ��1���ֽڱ�ʶ�ڴ浥λ��1����KB 2����MB  3����GB  4����TB��
    // ��2-5���ֽڴ���ǰʹ���ڴ棨������
    // ��6-9���ֽڴ����ܹ��ж����ڴ棨������
    u8  byMemUnitType = 0;
    TMemInfo tMemInfo;
    memset( &tMemInfo, 0, sizeof(tMemInfo) );
    s8  achMemInfo[10] = {'\0'};
    wMemInfoLen = 9;
    if ( !GetMemInfo( &tMemInfo ) )
    {  
        return FALSE; 
    }

    // pBufֵ
    byMemUnitType = 2;  // MB
    tMemInfo.m_dwMemAllocSize = tMemInfo.m_dwMemAllocSize>>10;      // KB ----> MB
    tMemInfo.m_dwMemPhysicsSize = tMemInfo.m_dwMemPhysicsSize>>10;  // KB ----> MB
    
    u8 byMemAllocRate = (u8)(tMemInfo.m_dwMemAllocSize * 100 / tMemInfo.m_dwMemPhysicsSize );
    // �ڴ�ռ���ʳ���85%������¼Ϊ����״̬
    if ( byMemAllocRate > DISK_THRESHOLD && m_byMemStatus == STATUS_NORMAL)
    {
        m_byMemStatus = STATUS_ABNORMAL;
        byRet = ON_ALARM;
    }    
    // �ڴ�ռ����δ����85%������¼Ϊ�쳣״̬
    if ( ( byMemAllocRate <= DISK_THRESHOLD) && ( m_byMemStatus == STATUS_ABNORMAL) )
    {
        m_byMemStatus = STATUS_NORMAL;
        byRet = OFF_ALARM;
    }
    EqpAgtLog( LOG_DETAIL, "[ScanMemInfo] MemAllocRate:%d(MB)/%d(MB), Memory status is %d <0:normal,1:abnormal>, Alarm: %d <0:no alarm,1:on alarm,2:off alarm>!\n",
        tMemInfo.m_dwMemAllocSize, tMemInfo.m_dwMemPhysicsSize, m_byMemStatus, byRet );

    tMemInfo.m_dwMemAllocSize = htonl(tMemInfo.m_dwMemAllocSize);
    tMemInfo.m_dwMemPhysicsSize = htonl(tMemInfo.m_dwMemPhysicsSize);
    memcpy( achMemInfo, &byMemUnitType, sizeof(u8) );
    memcpy( achMemInfo + sizeof(u8), &tMemInfo.m_dwMemAllocSize, sizeof(u32) );
    memcpy( achMemInfo + sizeof(u8) + sizeof(u32), &tMemInfo.m_dwMemPhysicsSize, sizeof(u32) );
    memcpy( lpszMemInfo2Trap, achMemInfo, wMemInfoLen );    
    return byRet;
}

/*====================================================================
    ������      : ScanDiskInfo
    ����        : ɨ����̿ռ�
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: LPSTR lpszDiskInfo2Trap  ���̿ռ�澯��Ϣ����֯Ϊ��Ӧ�ڵ�buf�ṹ��
                  u16& wDiskInfoLen        �澯���ݳ���
    ����ֵ˵��  : NO_ALARM   �޸澯
                  ON_ALARM   �澯����
                  OFF_ALARM  �澯�ָ�
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
u8 CEqpAgtSysInfo::ScanDiskInfo( LPSTR lpszDiskInfo2Trap, u16& wDiskInfoLen )
{
    u8 byRet = NO_ALARM;
    
    // pBuf �ṹ��
    // ��1���ֽڱ�ʶ���̿ռ䵥λ��1����KB 2����MB  3����GB  4����TB��
    // ��2-5���ֽڴ���ǰʹ�ô��̿ռ䣨������
    // ��6-9���ֽڴ����ܹ��ж��ٴ��̿ռ䣨������
    u8  byDiskUnitType = 0;
    TDiskInfo tDiskInfo;
    memset( &tDiskInfo, 0, sizeof(tDiskInfo) );
    s8  achDiskInfo[10] = {'\0'};
    wDiskInfoLen = 9;
    if ( !GetDiskInfo( m_achPartionName, &tDiskInfo ) )
    { 
        return FALSE; 
    }  
    
    byDiskUnitType = 3;  // GB
    tDiskInfo.m_dwDiskAllocSize = tDiskInfo.m_dwDiskAllocSize>>10;      // MB ----> GB
    tDiskInfo.m_dwDiskPhysicsSize = tDiskInfo.m_dwDiskPhysicsSize>>10;  // MB ----> GB

    u8 byDiskAllocRate = (u8)(tDiskInfo.m_dwDiskAllocSize * 100 / tDiskInfo.m_dwDiskPhysicsSize);
    // ���̿ռ�ռ���ʳ���80%������¼Ϊ����״̬
    if ( byDiskAllocRate > DISK_THRESHOLD && m_byDiskStatus == STATUS_NORMAL)
    {
        m_byDiskStatus = STATUS_ABNORMAL;
        byRet = ON_ALARM;
    }    
    // ���̿ռ�ռ����δ����80%������¼Ϊ�쳣״̬
    if ( ( byDiskAllocRate <= DISK_THRESHOLD) && ( m_byDiskStatus == STATUS_ABNORMAL) )
    {
        m_byDiskStatus = STATUS_NORMAL;
        byRet = OFF_ALARM;
    }
    EqpAgtLog( LOG_DETAIL, "[ScanDiskInfo] DiskAllocRate:%d(GB)/%d(GB), Disk status is %d <0:normal,1:abnormal>, Alarm: %d <0:no alarm,1:on alarm,2:off alarm>!\n",
        tDiskInfo.m_dwDiskAllocSize, tDiskInfo.m_dwDiskPhysicsSize, m_byDiskStatus, byRet );

    tDiskInfo.m_dwDiskAllocSize = htonl(tDiskInfo.m_dwDiskAllocSize);
    tDiskInfo.m_dwDiskPhysicsSize = htonl(tDiskInfo.m_dwDiskPhysicsSize);
    memcpy( achDiskInfo, &byDiskUnitType, sizeof(u8) );
    memcpy( achDiskInfo + sizeof(u8), &tDiskInfo.m_dwDiskAllocSize, sizeof(u32) );
    memcpy( achDiskInfo + sizeof(u8) + sizeof(u32), &tDiskInfo.m_dwDiskPhysicsSize, sizeof(u32) );
    memcpy( lpszDiskInfo2Trap, achDiskInfo, wDiskInfoLen );

    return byRet;
}

/*====================================================================
    ������      : SetLocalIpInfo
    ����        : �洢����IP��Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: TNetAdaptInfoAll* ptNetAdapterInfoAll IP��Ϣ
    �������˵��: 
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtSysInfo::SetIpInfo( TNetAdaptInfoAll& tNetAdapterInfoAll )
{
    if ( NULL == &tNetAdapterInfoAll )
    {
        EqpAgtLog( LOG_ERROR, "[SetLocalIpInfo] The input param is null!\n" );
        return FALSE;
    }    
    memcpy( &m_tNetAdapterInfoAll, &tNetAdapterInfoAll, sizeof(m_tNetAdapterInfoAll) ); 
    return TRUE;
}
/*====================================================================
    ������      : SetLocalIpInfo
    ����        : ��ȡ����IP��Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: TNetAdaptInfoAll* ptNetAdapterInfoAll IP��Ϣ
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtSysInfo::GetIpInfo( TNetAdaptInfoAll* ptNetAdapterInfoAll )
{
    if ( NULL == ptNetAdapterInfoAll )
    {
        EqpAgtLog( LOG_ERROR, "[GetLocalIpInfo] The input param is null!\n" );
        return FALSE;
    }    
    memcpy( ptNetAdapterInfoAll, &m_tNetAdapterInfoAll, sizeof(m_tNetAdapterInfoAll) ); 
    return TRUE;
}

/*====================================================================
    ������      : LocalIpAddrOfEqpAgt2Nms
    ����        : NMS����Get������IP EqpAgt ----> Nms
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: 
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtSysInfo::LocalIpAddrOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    /************************************************************************/
    /*                ��ʱ���ε�������֧��  begin                           */
    /************************************************************************/
    
    
    /************************************************************************/
    /*                 ��ʱ���ε�������֧��  end                            */
    /************************************************************************/
}

/*====================================================================
    ������      : LocalIpMaskOfEqpAgt2Nms
    ����        : NMS����Get������IP Mask  EqpAgt ----> Nms
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: 
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtSysInfo::LocalIpMaskOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    /************************************************************************/
    /*                ��ʱ���ε�������֧��  begin                           */
    /************************************************************************/
    
    
    /************************************************************************/
    /*                 ��ʱ���ε�������֧��  end                            */
    /************************************************************************/
}

/*====================================================================
    ������      : LocalGateWayOfEqpAgt2Nms
    ����        : NMS����Get������GateWay  EqpAgt ----> Nms
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: 
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtSysInfo::LocalGateWayOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    /************************************************************************/
    /*                ��ʱ���ε�������֧��  begin                           */
    /************************************************************************/
    
    
    /************************************************************************/
    /*                 ��ʱ���ε�������֧��  end                            */
    /************************************************************************/
}

/*====================================================================
    ������      : LocalIpInfoOfNms2EqpAgt
    ����        : NMSд��Set������IP��Ϣ     Nms ----> EqpAgt
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: u32 dwNodeName:    ����ڵ���
                  void * pBuf:       ��������
    �������˵��: 
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtSysInfo::IpInfoOfNms2EqpAgt(u32 dwNodeValue, void * pBuf)
{
    if ( NULL == m_pfNmsSetIpInfo )
    {
        EqpAgtLog( LOG_ERROR, "[LocalIpInfoOfNms2EqpAgt] m_pfNmsSetSysState is Null!\n" );
        return FALSE;
    }

    TNetAdaptInfoAll tNetAdapterInfoAll;
    memset( &tNetAdapterInfoAll, 0, sizeof(tNetAdapterInfoAll) );
    memcpy( &tNetAdapterInfoAll, &m_tNetAdapterInfoAll, sizeof(tNetAdapterInfoAll) );

    /************************************************************************/
    /*                ��ʱ���ε�������֧��  begin                           */
    /************************************************************************/
    /*   
    switch(dwNodeValue) 
    {        
 
        
    default:
        EqpAgtLog( LOG_ERROR, "[EqpAgtSetSysInfo] unexpected node(0x%x) received !\n", dwNodeValue);
        return FALSE;
    }
    */
    /************************************************************************/
    /*                 ��ʱ���ε�������֧��  end                            */
    /************************************************************************/

    (*m_pfNmsSetIpInfo)( tNetAdapterInfoAll );
    return TRUE; 
}

/*====================================================================
    ������      : EqpAgtGetBaseInfo
    ����        : ��ȡϵͳ��Ϣ���ص�����ʵ�֣�
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: u32 dwNodeName:    ����ڵ���
    �������˵��: void * pBuf:       �������
                  u16 *pwBufLen:     ���ݳ���
    ����ֵ˵��  : SNMP_SUCCESS       �ɹ�
                  SNMP_GENERAL_ERROR ����
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
u16 EqpAgtGetSysInfo(u32 dwNodeName, void * pBuf, u16 * pwBufLen)
{
    u32    dwNodeValue = GET_NODENAME(dwNodeName);
    u16    wResult = SNMP_SUCCESS;
    BOOL32 bRet = TRUE;

    switch(dwNodeValue) 
    {
    case NODE_COMMSYSSTATE :
        g_cEqpAgtSysInfo.SysStateOfEqpAgt2Nms( pBuf, pwBufLen );
        break;

    case NODE_COMMSOFTWAREVER :
        g_cEqpAgtSysInfo.SoftwareVerOfEqpAgt2Nms( pBuf, pwBufLen );
        break;

    case NODE_COMMHARDWAREVER :
        g_cEqpAgtSysInfo.HardwareVerOfEqpAgt2Nms( pBuf, pwBufLen );
        break;

    case NODE_COMMCOMPILETIME :
        g_cEqpAgtSysInfo.CompileTimeOfEqpAgt2Nms( pBuf, pwBufLen );
        break;

    case NODE_COMMOSTYPE :
        g_cEqpAgtSysInfo.EqpOsTypeOfEqpAgt2Nms( pBuf, pwBufLen );
        break;

    case NODE_COMMSUBTYPE :
        g_cEqpAgtSysInfo.EqpSubTypOfEqpAgt2Nms( pBuf, pwBufLen );
        break;

    case NODE_COMMCPURATE : 
        bRet = g_cEqpAgtSysInfo.CpuRateOfEqpAgt2Nms( pBuf, pwBufLen );
        if ( !bRet )
        {
            wResult = SNMP_GENERAL_ERROR;
        }
        break;

    case NODE_COMMMEMRATE : 
        bRet = g_cEqpAgtSysInfo.MemRateOfEqpAgt2Nms( pBuf, pwBufLen );
        if ( !bRet )
        {
            wResult = SNMP_GENERAL_ERROR;
        }
        break;

    case NODE_COMMDISKRATE : 
        bRet = g_cEqpAgtSysInfo.DiskRateOfEqpAgt2Nms( pBuf, pwBufLen );
        if ( !bRet )
        {
            wResult = SNMP_GENERAL_ERROR;
        }
        break;

    /************************************************************************/
    /*                ��ʱ���ε�������֧��  begin                           */
    /************************************************************************/
    /*
    case NODE_COMMIP :
        g_cEqpAgtSysInfo.LocalIpAddrOfEqpAgt2Nms( pBuf, pwBufLen );
        break;

    case NODE_COMMMASKIP :
        g_cEqpAgtSysInfo.LocalIpMaskOfEqpAgt2Nms( pBuf, pwBufLen );
        break;

    case NODE_COMMGATEWAYIP :
        g_cEqpAgtSysInfo.LocalGateWayOfEqpAgt2Nms( pBuf, pwBufLen );
        break;
    */
    /************************************************************************/
    /*                 ��ʱ���ε�������֧��  end                            */
    /************************************************************************/

    default:
        EqpAgtLog( LOG_ERROR, "[EqpAgtGetBaseInfo] unexpected node(0x%x) received !\n", dwNodeValue);
        wResult = SNMP_GENERAL_ERROR;
        break;
    }
    return wResult;
}

/*====================================================================
    ������      : EqpAgtSetSysInfo
    ����        : ����ϵͳ��Ϣ���ص�����ʵ�֣�
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: u32 dwNodeName:    ����ڵ���
                  void * pBuf:       �������
                  u16 *pwBufLen:     ���ݳ���
    �������˵��: 
    ����ֵ˵��  : SNMP_SUCCESS       �ɹ�
                  SNMP_GENERAL_ERROR ����
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
/*lint -save -esym(715, pwBufLen) */
u16 EqpAgtSetSysInfo(u32 dwNodeName, void * pBuf, u16 * pwBufLen)
/*lint -restore*/
{
    u32    dwNodeValue = GET_NODENAME(dwNodeName);
    u16    wResult = SNMP_SUCCESS;
    BOOL32 bRet = TRUE;

    switch(dwNodeValue) 
    {
    case NODE_COMMSYSSTATE :
        bRet = g_cEqpAgtSysInfo.SysStateOfNms2EqpAgt( pBuf );
        if ( !bRet )
        {
            wResult = SNMP_GENERAL_ERROR;
        }
        break;

    /************************************************************************/
    /*                ��ʱ���ε�������֧��  begin                           */
    /************************************************************************/        
    /*
    case NODE_COMMIP :
    case NODE_COMMMASKIP :
    case NODE_COMMGATEWAYIP :
        bRet = g_cEqpAgtSysInfo.LocalIpInfoOfNms2EqpAgt( dwNodeValue, pBuf );
        if ( !bRet )
        {
            wResult = SNMP_GENERAL_ERROR;
        }
        break;
    */
    /************************************************************************/
    /*                 ��ʱ���ε�������֧��  end                            */
    /************************************************************************/
        
    default:
        EqpAgtLog( LOG_ERROR, "[EqpAgtSetSysInfo] unexpected node(0x%x) received !\n", dwNodeValue);
        wResult = SNMP_GENERAL_ERROR;
        break;
    }

    return wResult;
}

/*====================================================================
    ������      : EqpAgtTrapCpuRate
    ����        : ��֯cpu trapֵ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: CNodes& cNodes trapֵ
    ����ֵ˵��  : NO_TRAP        ��trap����
                  GENERATE_TRAP  ����trap
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
u16 EqpAgtTrapCpuRate( CNodes& cNodes )
{
    u16  wResult = NO_TRAP;
    u8   byRet = NO_ALARM;
    u16  wTrapType = INVALID_TRAP_TYPE;
    u16  wRet = SNMP_GENERAL_ERROR;

    // pBuf �ṹ��
    // u8(N�� cpu) + u8(��һ�� cpuʹ����) + ���� + u8(��N�� cpuʹ����)
    // ʵ�ʣ�Ŀǰͨ��OSP��ȡcpuʹ����:һ�� + �ٷֱ� ��
    s8   achCpuInfo2Trap[3] = {'\0'};
    u16  wCpuInfoLen = 0;
    byRet = g_cEqpAgtSysInfo.ScanCpuInfo( achCpuInfo2Trap, wCpuInfoLen );
    switch (byRet) 
    {
    case NO_ALARM:
        return wResult;
    case ON_ALARM:
        wTrapType = TRAP_ALARM_GENERATE;
        break;
    case OFF_ALARM:
        wTrapType = TRAP_ALARM_RESTORE;
        break; 
    default:
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapCpuRate] Wrong return value of ScanCpuInfo !\n" );
        return wResult; 
    }
    
    wRet = cNodes.Clear();
    if ( SNMP_SUCCESS != wRet ) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapCpuRate] cNodes clear failed !\n" );
        return wResult;
    }
    
    wRet = cNodes.SetTrapType(wTrapType);
    if (SNMP_SUCCESS != wRet) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapCpuRate] cNodes set trap type failed !\n" );
        return wResult;
    }

    // pBuf�ṹ��
    // u8���澯�ڵ�N����+ u32����һ���澯�ڵ�� ������+ ���� + u32����N���澯�ڵ�� ������
    s8  achNodeInfo[sizeof(u32) * MAXNUM_ALARM_NODES + 1 + 1 ]={'\0'};
    u16 wNodeInfoLen = 0;
    u32 dwNodeName = 0;
    u8  byNodeNum = 0;
    byNodeNum = 1;  // ��CPU�ڵ��
    memcpy( achNodeInfo, &byNodeNum, sizeof(u8) );
    dwNodeName = NODE_COMMCPURATE;
    dwNodeName = htonl(dwNodeName);
    memcpy( achNodeInfo + sizeof(u8), &dwNodeName, sizeof(u32) );
    wNodeInfoLen = 5;
    wRet = cNodes.AddNodeValue( NODE_COMMALARMNOTIFY, achNodeInfo, wNodeInfoLen);
    if (SNMP_SUCCESS != wRet) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapCpuRate] cNodes add node(0x%x) failed !\n", NODE_COMMALARMNOTIFY );
        return wResult;
    }
    
    wRet = cNodes.AddNodeValue( NODE_COMMCPURATE, achCpuInfo2Trap, wCpuInfoLen );
    if (SNMP_SUCCESS != wRet) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapCpuRate] cNodes add node(0x%x) failed !\n", NODE_COMMCPURATE );
        return wResult;
    }

    wResult = GENERATE_TRAP;
    return wResult;
}

/*====================================================================
    ������      : EqpAgtTrapMemRate
    ����        : ��֯�ڴ�trapֵ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: CNodes& cNodes trapֵ
    ����ֵ˵��  : NO_TRAP        ��trap����
                  GENERATE_TRAP  ����trap
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
u16 EqpAgtTrapMemRate( CNodes& cNodes )
{
    u16  wResult = NO_TRAP;
    u8   byRet = NO_ALARM;
    u16  wTrapType = INVALID_TRAP_TYPE;
    u16  wRet = SNMP_GENERAL_ERROR;
    
    // pBuf �ṹ��
    // ��1���ֽڱ�ʶ�ڴ浥λ��1����KB 2����MB  3����GB  4����TB��
    // ��2-5���ֽڴ���ǰʹ���ڴ棨������
    // ��6-9���ֽڴ����ܹ��ж����ڴ棨������
    s8   achMemInfo2Trap[10] = {'\0'};
    u16  wMemInfoLen = 0;
    byRet = g_cEqpAgtSysInfo.ScanMemInfo( achMemInfo2Trap, wMemInfoLen );
    switch (byRet) 
    {
    case NO_ALARM:
        return wResult;
    case ON_ALARM:
        wTrapType = TRAP_ALARM_GENERATE;
        break;
    case OFF_ALARM:
        wTrapType = TRAP_ALARM_RESTORE;
        break; 
    default:
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapMemRate] Wrong return value of ScanMemInfo !\n" );
        return wResult; 
    }
    
    wRet = cNodes.Clear();
    if ( SNMP_SUCCESS != wRet ) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapMemRate] cNodes clear failed !\n" );
        return wResult;
    }
    
    wRet = cNodes.SetTrapType(wTrapType);
    if (SNMP_SUCCESS != wRet) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapMemRate] cNodes set trap type failed !\n" );
        return wResult;
    }
    
    // pBuf�ṹ��
    // u8���澯�ڵ�N����+ u32����һ���澯�ڵ�� ������+ ���� + u32����N���澯�ڵ�� ������
    s8  achNodeInfo[sizeof(u32) * MAXNUM_ALARM_NODES + 1 + 1 ]={'\0'};
    u16 wNodeInfoLen = 0;
    u32 dwNodeName = 0;
    u8  byNodeNum = 0;
    byNodeNum = 1;  // ��CPU�ڵ��
    memcpy( achNodeInfo, &byNodeNum, sizeof(u8) );
    dwNodeName = NODE_COMMMEMRATE;
    dwNodeName = htonl(dwNodeName);
    memcpy( achNodeInfo + sizeof(u8), &dwNodeName, sizeof(u32) );
    wNodeInfoLen = 5;
    wRet = cNodes.AddNodeValue( NODE_COMMALARMNOTIFY, achNodeInfo, wNodeInfoLen );
    if (SNMP_SUCCESS != wRet) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapMemRate] cNodes add node(0x%x) failed !\n", NODE_COMMALARMNOTIFY );
        return wResult;
    }
    
    wRet = cNodes.AddNodeValue( NODE_COMMMEMRATE, achMemInfo2Trap, wMemInfoLen );
    if (SNMP_SUCCESS != wRet) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapMemRate] cNodes add node(0x%x) failed !\n", NODE_COMMMEMRATE );
        return wResult;
    }
    
    wResult = GENERATE_TRAP;
    return wResult;
}

/*====================================================================
    ������      : EqpAgtTrapDiskRate
    ����        : ��֯���̿ռ�trapֵ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    �������˵��: CNodes& cNodes trapֵ
    ����ֵ˵��  : NO_TRAP        ��trap����
                  GENERATE_TRAP  ����trap
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
u16 EqpAgtTrapDiskRate( CNodes& cNodes )
{
    u16  wResult = NO_TRAP;
    u8   byRet = NO_ALARM;
    u16  wTrapType = INVALID_TRAP_TYPE;
    u16  wRet = SNMP_GENERAL_ERROR;
    
    // pBuf �ṹ��
    // ��1���ֽڱ�ʶ���̿ռ䵥λ��1����KB 2����MB  3����GB  4����TB��
    // ��2-5���ֽڴ���ǰʹ�ô��̿ռ䣨������
    // ��6-9���ֽڴ����ܹ��ж��ٴ��̿ռ䣨������
    s8   achDiskInfo2Trap[10] = {'\0'};
    u16  wDiskInfoLen = 0;
    byRet = g_cEqpAgtSysInfo.ScanDiskInfo( achDiskInfo2Trap, wDiskInfoLen );
    switch (byRet) 
    {
    case NO_ALARM:
        return wResult;
    case ON_ALARM:
        wTrapType = TRAP_ALARM_GENERATE;
        break;
    case OFF_ALARM:
        wTrapType = TRAP_ALARM_RESTORE;
        break; 
    default:
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapDiskRate] Wrong return value of ScanDiskInfo !\n" );
        return wResult; 
    }
    
    wRet = cNodes.Clear();
    if ( SNMP_SUCCESS != wRet ) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapDiskRate] cNodes clear failed !\n" );
        return wResult;
    }
    
    wRet = cNodes.SetTrapType(wTrapType);
    if (SNMP_SUCCESS != wRet) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapDiskRate] cNodes set trap type failed !\n" );
        return wResult;
    }
    
    // pBuf�ṹ��
    // u8���澯�ڵ�N����+ u32����һ���澯�ڵ�� ������+ ���� + u32����N���澯�ڵ�� ������
    s8  achNodeInfo[sizeof(u32) * MAXNUM_ALARM_NODES + 1 + 1 ]={'\0'};
    u16 wNodeInfoLen = 0;
    u32 dwNodeName = 0;
    u8  byNodeNum = 0;
    byNodeNum = 1;  // ��CPU�ڵ��
    memcpy( achNodeInfo, &byNodeNum, sizeof(u8) );
    dwNodeName = NODE_COMMDISKRATE;
    dwNodeName = htonl(dwNodeName);
    memcpy( achNodeInfo + sizeof(u8), &dwNodeName, sizeof(u32) );
    wNodeInfoLen = 5;
    wRet = cNodes.AddNodeValue( NODE_COMMALARMNOTIFY, achNodeInfo, wNodeInfoLen);
    if (SNMP_SUCCESS != wRet) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapDiskRate] cNodes add node(0x%x) failed !\n", NODE_COMMALARMNOTIFY );
        return wResult;
    }
    
    wRet = cNodes.AddNodeValue( NODE_COMMDISKRATE, achDiskInfo2Trap, wDiskInfoLen );
    if (SNMP_SUCCESS != wRet) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapDiskRate] cNodes add node(0x%x) failed !\n", NODE_COMMDISKRATE );
        return wResult;
    }
    
    wResult = GENERATE_TRAP;
    return wResult;
}

/*====================================================================
    ������      : showsysstate
    ����        : ������ʾϵͳ״̬
    �㷨ʵ��    :
    ����ȫ�ֱ���: 
    �������˵��: void
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
API void showsysstate(void)
{
    StaticLog( "[showsysstate] The system state is %u", g_cEqpAgtSysInfo.GetSysState() );
}

/*====================================================================
    ������      : showsoftwarever
    ����        : ������ʾ����汾��
    �㷨ʵ��    :
    ����ȫ�ֱ���: 
    �������˵��: void
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
API void showsoftwarever(void)
{
    s8  achSoftwareVer[EQPAGT_MAX_LENGTH] = {'\0'};
    g_cEqpAgtSysInfo.GetSoftwareVer( achSoftwareVer );
    StaticLog( "[showsoftwarever] The software version is %s", achSoftwareVer );
}

/*====================================================================
    ������      : showhardwarever
    ����        : ������ʾӲ���汾��
    �㷨ʵ��    :
    ����ȫ�ֱ���: 
    �������˵��: void
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
API void showhardwarever(void)
{    
    s8  achHardwareVer[EQPAGT_MAX_LENGTH] = {'\0'};
    g_cEqpAgtSysInfo.GetHardwareVer( achHardwareVer );
    StaticLog( "[showhardwarever] The hardware version is %s", achHardwareVer );
}

/*====================================================================
    ������      : showcompiletime
    ����        : ������ʾ����ʱ��
    �㷨ʵ��    :
    ����ȫ�ֱ���: 
    �������˵��: void
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
API void showcompiletime(void)
{
    s8  achCompileTime[EQPAGT_MAX_LENGTH] = {'\0'};
    g_cEqpAgtSysInfo.GetCompileTime( achCompileTime );
    StaticLog( "[showcompiletime] The compile time is %s", achCompileTime );
}

/*====================================================================
    ������      : showeqpostype
    ����        : ������ʾ��ǰ�豸����ϵͳ
    �㷨ʵ��    :
    ����ȫ�ֱ���: 
    �������˵��: void
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
API void showeqpostype(void)
{
    StaticLog( "[showeqpostype] The eqp os type is %u", g_cEqpAgtSysInfo.GetEqpOsType() );
}

/*====================================================================
    ������      : showeqpsubtype
    ����        : ������ʾ��ǰ�豸������
    �㷨ʵ��    :
    ����ȫ�ֱ���: 
    �������˵��: void
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
API void showeqpsubtype(void)
{
    StaticLog( "[showeqpsubtype] The eqp subtype is %u", g_cEqpAgtSysInfo.GetEqpSubType() );
}

/*====================================================================
    ������      : showcpuinfo
    ����        : ������ʾ��ǰcpu��Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���: g_CEqpAgtSysInfo
    �������˵��: void
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
API void showcpuinfo(void)
{
    TCpuAllInfo tCpuAllInfo;
    memset( &tCpuAllInfo, 0, sizeof(tCpuAllInfo));
    BOOL32 bRet = g_cEqpAgtSysInfo.GetCpuAllInfo( &tCpuAllInfo );
    if ( !bRet )
    {
        StaticLog( "[showcpuinfo] GetCpuAllInfo wrong");
        return;
    }
    StaticLog( "[showcpuinfo] Cpu AllocRate:%u, Status is %u <0:normal,1:abnormal>",
        tCpuAllInfo.tCpuInfo.m_byCpuAllocRate, tCpuAllInfo.m_byCpuStatus );
    return;
}

/*====================================================================
    ������      : showmeminfo
    ����        : ������ʾ��ǰ�ڴ���Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���: g_CEqpAgtSysInfo
    �������˵��: void
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
API void showmeminfo(void)
{
    TMemAllInfo tMemAllInfo;
    memset( &tMemAllInfo, 0, sizeof(tMemAllInfo));
    BOOL32 bRet = g_cEqpAgtSysInfo.GetMemAllInfo( &tMemAllInfo );
    if ( !bRet )
    {
        StaticLog( "[showmeminfo] GetMemAllInfo wrong");
        return;
    }
    StaticLog( "[showmeminfo] Memory AllocRate:%u(MB)/%u(MB), Status is %u <0:normal,1:abnormal>",
        tMemAllInfo.tMemInfo.m_dwMemAllocSize>>10, tMemAllInfo.tMemInfo.m_dwMemPhysicsSize>>10, tMemAllInfo.m_byMemStatus );
    return;
}

/*====================================================================
    ������      : showdiskinfo
    ����        : ������ʾ��ǰ���̿ռ���Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���: g_CEqpAgtSysInfo
    �������˵��: void
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
API void showdiskinfo(void)
{
    TDiskAllInfo tDiskAllInfo;
    memset( &tDiskAllInfo, 0, sizeof(tDiskAllInfo));
    BOOL32 bRet = g_cEqpAgtSysInfo.GetDiskAllInfo( &tDiskAllInfo );
    if ( !bRet )
    {
        StaticLog( "[showdiskinfo] GetDiskAllInfo wrong");
        return;
    }
    StaticLog( "[showdiskinfo] Disk: %s AllocRate:%u(GB)/%u(GB), Status is %u <0:normal,1:abnormal>!\n",
        tDiskAllInfo.m_achPartionName, tDiskAllInfo.tDiskInfo.m_dwDiskAllocSize>>10, 
        tDiskAllInfo.tDiskInfo.m_dwDiskPhysicsSize>>10, tDiskAllInfo.m_byDiskStatus );
    return;
}

/*====================================================================
    ������      : 
    ����        : 
    �㷨ʵ��    :
    ����ȫ�ֱ���: 
    �������˵��: void
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
/************************************************************************/
/*                ��ʱ���ε��������޸�  begin                           */
/************************************************************************/
/*
API void showipinfo(void)
{
    TLocalIpInfo tLocalIpInfo;
    memset( &tLocalIpInfo, 0, sizeof(tLocalIpInfo));
    g_cEqpAgtSysInfo.GetLocalIpInfo( &tLocalIpInfo );
    s8  achIpAddr[32] = {'\0'};
    s8  achIpMask[32] = {'\0'};
    s8  achGateway[32] = {'\0'};
    GetIpFromU32( achIpAddr, tLocalIpInfo.GetLocalIPAddr());
    GetIpFromU32( achIpMask, tLocalIpInfo.GetLocalIPMask());
    GetIpFromU32( achGateway, tLocalIpInfo.GetLocalGateWay());
    OspPrintf( TRUE, FALSE, "[showlocalipinfo] IP:%s, Mask:%s, Gateway:%s!\n", achIpAddr, achIpMask, achGateway );
}
*/
/************************************************************************/
/*                 ��ʱ���ε��������޸�  end                            */
/************************************************************************/


/*====================================================================
    ������      : trapcpu
    ����        : debug ����������ڷ���cpu trap
    �㷨ʵ��    :
    ����ȫ�ֱ���: g_cEqpAgtSnmp
    �������˵��: u8 byCpuAllocRate   cpuʹ����
                  u8 byCpuStatus      cpu״̬������/�쳣��
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
API void trapcpu( u8 byCpuAllocRate, u8 byCpuStatus )
{
    u16  wTrapType = INVALID_TRAP_TYPE;    
    // Cpuռ���ʳ���85%������¼Ϊ����״̬
    if ( byCpuAllocRate > CPU_THRESHOLD && byCpuStatus == STATUS_NORMAL)
    {
        wTrapType = TRAP_ALARM_GENERATE;
    }    
    // Cpuռ����δ����85%������¼Ϊ�쳣״̬
    if ( ( byCpuAllocRate <= CPU_THRESHOLD) && ( byCpuStatus == STATUS_ABNORMAL) )
    {
        wTrapType = TRAP_ALARM_RESTORE;
    } 

    if ( INVALID_TRAP_TYPE == wTrapType )
    {   
        StaticLog( "[trapcpu] No Trap!" );
        return;
    }

    CNodes cNodes;
    cNodes.Clear();
    u16  wRet = SNMP_GENERAL_ERROR; 

    wRet = cNodes.Clear();
    if ( SNMP_SUCCESS != wRet ) 
    {
        StaticLog( "[trapcpu] cNodes clear failed !\n" );
        return;
    }
    
    wRet = cNodes.SetTrapType(wTrapType);
    if ( SNMP_SUCCESS != wRet ) 
    {
        StaticLog( "[trapcpu] cNodes set trap type failed !\n" );
        return;
    }
    
    // pBuf�ṹ��
    // u8���澯�ڵ�N����+ u32����һ���澯�ڵ�� ������+ ���� + u32����N���澯�ڵ�� ������
    s8  achNodeInfo[sizeof(u32) * MAXNUM_ALARM_NODES + 1 + 1 ]={'\0'};
    u16 wNodeInfoLen = 0;
    u32 dwNodeName = 0;
    u8  byNodeNum = 0;
    byNodeNum = 1;  // ��CPU�ڵ��
    memcpy( achNodeInfo, &byNodeNum, sizeof(u8) );
    dwNodeName = NODE_COMMCPURATE;
    dwNodeName = htonl(dwNodeName);
    memcpy( achNodeInfo + sizeof(u8), &dwNodeName, sizeof(u32) );
    wNodeInfoLen = 5;
    wRet = cNodes.AddNodeValue( NODE_COMMALARMNOTIFY, achNodeInfo, wNodeInfoLen);
    if (SNMP_SUCCESS != wRet) 
    {
        StaticLog( "[trapcpu] cNodes add node(0x%x) failed !\n", NODE_COMMALARMNOTIFY );
        return;
    }
    
    // pBuf �ṹ��
    // u8(N�� cpu) + u8(��һ�� cpuʹ����) + ���� + u8(��N�� cpuʹ����)
    // ʵ�ʣ�Ŀǰͨ��OSP��ȡcpuʹ����:һ�� + �ٷֱ� ��
    u8  byCpuNum = 1;
    s8  achCpuInfo2Trap[3] = {'\0'};
    u16 wCpuInfoLen = 2;
    memcpy( achCpuInfo2Trap, &byCpuNum, sizeof(u8) );
    memcpy( achCpuInfo2Trap + sizeof(u8), &byCpuAllocRate, sizeof(u8) );
    wRet = cNodes.AddNodeValue( NODE_COMMCPURATE, achCpuInfo2Trap, wCpuInfoLen );
    if (SNMP_SUCCESS != wRet) 
    {
        StaticLog( "[trapcpu] cNodes add node(0x%x) failed !\n", NODE_COMMCPURATE );
        return;
    } 

    wRet = g_cEqpAgtSnmp.EqpAgtSendTrap( cNodes );
    if ( SNMP_GENERAL_ERROR == wRet ) // ���ʹ���
    {                    
        StaticLog( "[trapcpu] Send Trap failed!\n" );
    }
    else // ������ȷ
    {
        StaticLog( "[trapcpu] Send Trap Success!\n" );
    }
    return;
}

/*====================================================================
    ������      : trapmem
    ����        : debug ����������ڷ����ڴ�trap
    �㷨ʵ��    :
    ����ȫ�ֱ���: g_cEqpAgtSnmp
    �������˵��: u8 byMemAllocSize      ʹ���ڴ��С(MByte)
                  u8 byMemPhysicsSize    �����ڴ��С(MByte)
                  u8 byMemStatus         �ڴ�״̬������/�쳣��
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
API void trapmem( u32 dwMemAllocSize, u32 dwMemPhysicsSize, u8 byMemStatus )
{
    u16  wTrapType = INVALID_TRAP_TYPE;
    u8 byMemAllocRate = (u8)(dwMemAllocSize * 100 / dwMemPhysicsSize );
    // �ڴ�ռ���ʳ���85%������¼Ϊ����״̬
    if ( byMemAllocRate > DISK_THRESHOLD && byMemStatus == STATUS_NORMAL)
    {
        wTrapType = TRAP_ALARM_GENERATE;
    }    
    // �ڴ�ռ����δ����85%������¼Ϊ�쳣״̬
    if ( ( byMemAllocRate <= DISK_THRESHOLD) && ( byMemStatus == STATUS_ABNORMAL) )
    {
        wTrapType = TRAP_ALARM_RESTORE;
    }    
    if ( INVALID_TRAP_TYPE == wTrapType )
    {   
        StaticLog( "[trapmem] No Trap!" );
        return;
    }
    
    CNodes cNodes;
    cNodes.Clear();
    u16  wRet = SNMP_GENERAL_ERROR; 
    
    wRet = cNodes.Clear();
    if ( SNMP_SUCCESS != wRet ) 
    {
        StaticLog( "[trapmem] cNodes clear failed !\n" );
        return;
    }
    
    wRet = cNodes.SetTrapType(wTrapType);
    if ( SNMP_SUCCESS != wRet ) 
    {
        StaticLog( "[trapmem] cNodes set trap type failed !\n" );
        return;
    }
    
    // pBuf�ṹ��
    // u8���澯�ڵ�N����+ u32����һ���澯�ڵ�� ������+ ���� + u32����N���澯�ڵ�� ������
    s8  achNodeInfo[sizeof(u32) * MAXNUM_ALARM_NODES + 1 + 1 ]={'\0'};
    u16 wNodeInfoLen = 0;
    u32 dwNodeName = 0;
    u8  byNodeNum = 0;
    byNodeNum = 1;  // ��CPU�ڵ��
    memcpy( achNodeInfo, &byNodeNum, sizeof(u8) );
    dwNodeName = NODE_COMMMEMRATE;
    dwNodeName = htonl(dwNodeName);
    memcpy( achNodeInfo + sizeof(u8), &dwNodeName, sizeof(u32) );
    wNodeInfoLen = 5;
    wRet = cNodes.AddNodeValue( NODE_COMMALARMNOTIFY, achNodeInfo, wNodeInfoLen );
    if (SNMP_SUCCESS != wRet) 
    {
        StaticLog( "[trapmem] cNodes add node(0x%x) failed !\n", NODE_COMMALARMNOTIFY );
        return;
    }    

    // pBuf �ṹ��
    // ��1���ֽڱ�ʶ�ڴ浥λ��1����KB 2����MB  3����GB  4����TB��
    // ��2-5���ֽڴ���ǰʹ���ڴ棨������
    // ��6-9���ֽڴ����ܹ��ж����ڴ棨������
    u8  byMemUnitType = 2;  // MB
    dwMemAllocSize = htonl(dwMemAllocSize);
    dwMemPhysicsSize = htonl(dwMemPhysicsSize);
    s8  achMemInfo2Trap[10] = {'\0'};
    u16 wMemInfoLen = 9;
    memcpy( achMemInfo2Trap, &byMemUnitType, sizeof(u8) );
    memcpy( achMemInfo2Trap + sizeof(u8), &dwMemAllocSize, sizeof(u32) );
    memcpy( achMemInfo2Trap + sizeof(u8) + sizeof(u32), &dwMemPhysicsSize, sizeof(u32) );
    wRet = cNodes.AddNodeValue( NODE_COMMMEMRATE, achMemInfo2Trap, wMemInfoLen );
    if (SNMP_SUCCESS != wRet) 
    {
        StaticLog( "[trapmem] cNodes add node(0x%x) failed !\n", NODE_COMMMEMRATE );
        return;
    }
    
    wRet = g_cEqpAgtSnmp.EqpAgtSendTrap( cNodes );
    if ( SNMP_GENERAL_ERROR == wRet ) // ���ʹ���
    {                    
        StaticLog( "[trapmem] Send Trap failed!\n" );
    }
    else // ������ȷ
    {
        StaticLog( "[trapmem] Send Trap Success!\n" );
    }
    return;
}

/*====================================================================
    ������      : trapdisk
    ����        : debug ����������ڷ��ʹ��̿ռ�trap
    �㷨ʵ��    :
    ����ȫ�ֱ���: g_cEqpAgtSnmp
    �������˵��: u8 dwDiskAllocSize      ʹ�ô��̿ռ��С(GByte)
                  u8 dwDiskPhysicsSize    ������̿ռ���С(GByte)
                  u8 byDiskStatus         ���̿ռ�״̬������/�쳣��
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
API void trapdisk( u32 dwDiskAllocSize, u32 dwDiskPhysicsSize, u8 byDiskStatus )
{
    u16  wTrapType = INVALID_TRAP_TYPE;
    u8 byDiskAllocRate = (u8)(dwDiskAllocSize * 100 / dwDiskPhysicsSize );
    // �ڴ�ռ���ʳ���85%������¼Ϊ����״̬
    if ( byDiskAllocRate > DISK_THRESHOLD && byDiskStatus == STATUS_NORMAL)
    {
        wTrapType = TRAP_ALARM_GENERATE;
    }    
    // �ڴ�ռ����δ����85%������¼Ϊ�쳣״̬
    if ( ( byDiskAllocRate <= DISK_THRESHOLD) && ( byDiskStatus == STATUS_ABNORMAL) )
    {
        wTrapType = TRAP_ALARM_RESTORE;
    }    
    if ( INVALID_TRAP_TYPE == wTrapType )
    {   
        StaticLog( "[trapmem] No Trap!" );
        return;
    }
    
    CNodes cNodes;
    cNodes.Clear();
    u16  wRet = SNMP_GENERAL_ERROR; 
    
    wRet = cNodes.Clear();
    if ( SNMP_SUCCESS != wRet ) 
    {
        StaticLog( "[trapdisk] cNodes clear failed !\n" );
        return;
    }
    
    wRet = cNodes.SetTrapType(wTrapType);
    if ( SNMP_SUCCESS != wRet ) 
    {
        StaticLog( "[trapdisk] cNodes set trap type failed !\n" );
        return;
    }
    
    // pBuf�ṹ��
    // u8���澯�ڵ�N����+ u32����һ���澯�ڵ�� ������+ ���� + u32����N���澯�ڵ�� ������
    s8  achNodeInfo[sizeof(u32) * MAXNUM_ALARM_NODES + 1 + 1 ]={'\0'};
    u16 wNodeInfoLen = 0;
    u32 dwNodeName = 0;
    u8  byNodeNum = 0;
    byNodeNum = 1;  // ��CPU�ڵ��
    memcpy( achNodeInfo, &byNodeNum, sizeof(u8) );
    dwNodeName = NODE_COMMDISKRATE;
    dwNodeName = htonl(dwNodeName);
    memcpy( achNodeInfo + sizeof(u8), &dwNodeName, sizeof(u32) );
    wNodeInfoLen = 5;
    wRet = cNodes.AddNodeValue( NODE_COMMALARMNOTIFY, achNodeInfo, wNodeInfoLen);
    if (SNMP_SUCCESS != wRet) 
    {
        StaticLog( "[trapdisk] cNodes add node(0x%x) failed !\n", NODE_COMMALARMNOTIFY );
        return;
    }
    
    // pBuf �ṹ��
    // ��1���ֽڱ�ʶ�ڴ浥λ��1����KB 2����MB  3����GB  4����TB��
    // ��2-5���ֽڴ���ǰʹ�ô��̿ռ䣨������
    // ��6-9���ֽڴ����ܹ��ж��ٴ��̿ռ䣨������
    u8  byDiskUnitType = 3;  // GB
    dwDiskAllocSize = htonl(dwDiskAllocSize);
    dwDiskPhysicsSize = htonl(dwDiskPhysicsSize);
    s8  achDiskInfo2Trap[10] = {'\0'};
    u16 wDiskInfoLen = 9;
    memcpy( achDiskInfo2Trap, &byDiskUnitType, sizeof(u8) );
    memcpy( achDiskInfo2Trap + sizeof(u8), &dwDiskAllocSize, sizeof(u32) );
    memcpy( achDiskInfo2Trap + sizeof(u8) + sizeof(u32), &dwDiskPhysicsSize, sizeof(u32) );
    wRet = cNodes.AddNodeValue( NODE_COMMDISKRATE, achDiskInfo2Trap, wDiskInfoLen );
    if (SNMP_SUCCESS != wRet) 
    {
        StaticLog( "[trapmem] cNodes add node(0x%x) failed !\n", NODE_COMMDISKRATE );
        return;
    }
    
    wRet = g_cEqpAgtSnmp.EqpAgtSendTrap( cNodes );
    if ( SNMP_GENERAL_ERROR == wRet ) // ���ʹ���
    {                    
        StaticLog( "[trapdisk] Send Trap failed!\n" );
    }
    else // ������ȷ
    {
        StaticLog( "[trapdisk] Send Trap Success!\n" );
    }
    return;
}

/*====================================================================
    ������      : cputrapoper
    ����        : ��/�ر� cpu ʹ���� trap
    �㷨ʵ��    :
    ����ȫ�ֱ���: 
    �������˵��: u32 dwScanTimeSpan  ɨ��ʱ��
                  BOOL32 bEnable      ��/�ر�
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
API void cputrapoper( u32 dwScanTimeSpan, BOOL32 bEnable )
{
    EqpAgtTrapOper( NODE_COMMCPURATE, EqpAgtTrapCpuRate, dwScanTimeSpan, bEnable);
}

/*====================================================================
    ������      : memtrapoper
    ����        : ��/�ر� �ڴ� ʹ���� trap
    �㷨ʵ��    :
    ����ȫ�ֱ���: 
    �������˵��: u32 dwScanTimeSpan  ɨ��ʱ��
                  BOOL32 bEnable      ��/�ر�
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
API void memtrapoper( u32 dwScanTimeSpan, BOOL32 bEnable )
{ 
    EqpAgtTrapOper( NODE_COMMMEMRATE, EqpAgtTrapMemRate, dwScanTimeSpan, bEnable);
}

/*====================================================================
    ������      : disktrapoper
    ����        : ��/�ر� ���̿ռ� ʹ���� trap
    �㷨ʵ��    :
    ����ȫ�ֱ���: 
    �������˵��: u32 dwScanTimeSpan  ɨ��ʱ��
                  BOOL32 bEnable      ��/�ر�
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
API void disktrapoper( u32 dwScanTimeSpan, BOOL32 bEnable )
{ 
    EqpAgtTrapOper( NODE_COMMDISKRATE, EqpAgtTrapDiskRate, dwScanTimeSpan, bEnable);
}

/*====================================================================
    ������      : setsysstate
    ����        : ����ϵͳ״̬ debug �����������ģ��NMS����
    �㷨ʵ��    :
    ����ȫ�ֱ���: 
    �������˵��: u32 dwSysState ϵͳ״̬
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
API void setsysstate( u32 dwSysState )
{
    dwSysState = htonl( dwSysState );
    g_cEqpAgtSysInfo.SysStateOfNms2EqpAgt( &dwSysState );
}

/*====================================================================
    ������      : 
    ����        : 
    �㷨ʵ��    :
    ����ȫ�ֱ���: 
    �������˵��: void
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
/************************************************************************/
/*                ��ʱ���ε��������޸�  begin                           */
/************************************************************************/
/*
API void setipinfo( u32 dwIPAddr, u32 dwIPMask, u32 dwGateWay )
{ 
    g_cEqpAgtSysInfo.LocalIpInfoOfNms2EqpAgt( NODE_COMMIP, &dwIPAddr );
    g_cEqpAgtSysInfo.LocalIpInfoOfNms2EqpAgt( NODE_COMMMASKIP, &dwIPMask );
    g_cEqpAgtSysInfo.LocalIpInfoOfNms2EqpAgt( NODE_COMMGATEWAYIP, &dwGateWay );
}
*/
/************************************************************************/
/*                 ��ʱ���ε��������޸�  end                            */
/************************************************************************/