/*****************************************************************************
    ģ����      : EqpAgt
    �ļ���      : eqpagtutility.h
    ����ļ�    : 
    �ļ�ʵ�ֹ���: EqpAgtҵ���ڲ�ʹ�ýṹ����
    ����        : liaokang
    �汾        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    �޸ļ�¼:
    ��  ��      �汾        �޸���          �޸�����
    2012/06/18  4.7         liaokang        ����
******************************************************************************/
#include "eqpagtutility.h"

/*====================================================================
    ������      : CreateSemHandle
    ����        : �����ź���
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: SEMHANDLE &hSem �ź���
    �������˵��: 
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CreateSemHandle( SEMHANDLE &hSem )
{
    BOOL32 bRet = TRUE;    
    // �����ź���
    if( !OspSemBCreate(&hSem) )
    {
        OspSemDelete( hSem );
        hSem = NULL;
        StaticLog( "[CreateSemHandle] Create semHandle failed!\n");
        bRet = FALSE;
    }
    return bRet;
}

/*====================================================================
    ������      : FreeSemHandle
    ����        : �ͷ��ź���
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: SEMHANDLE &hSem �ź���
    �������˵��: 
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 FreeSemHandle( SEMHANDLE &hSem )
{
    // ɾ���ź���
    if( NULL != hSem )
    {
        OspSemDelete( hSem );
        hSem = NULL;
    }
    return TRUE;
}

/*====================================================================
    ������      : GetIpFromU32
    ����        : ��U32�õ�Ip�ַ���
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: u32 dwIpInfo     Ip��Ϣ��u32 ������
    �������˵��: LPSTR lpszDstStr Ip�ַ���
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void GetIpFromU32( LPSTR lpszDstStr, u32 dwIpInfo )
{
    if( NULL != lpszDstStr )    
    {
        u32 dwTmpIp = dwIpInfo;
        if( 0 != dwTmpIp )
        {
            sprintf(lpszDstStr, "%d.%d.%d.%d%c", (dwTmpIp>>24)&0xFF, (dwTmpIp>>16)&0xFF, (dwTmpIp>>8)&0xFF, dwTmpIp&0xFF, 0);
        }
        else
        {
            memcpy(lpszDstStr, "0", sizeof("0"));
        }
    }    
    return;
}

/*====================================================================
    ������      : EqpAgtLog
    ����        : ��ӡʵ��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: u8 byPrintLvl    ��ӡ�ȼ�
                  s8* pszFmt, ...  ��ʽ
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
/*lint -save -e530 */
/*lint -save -esym(438, argptr)*/
/*lint -save -esym(526, __builtin_va_start)*/
/*lint -save -esym(628, __builtin_va_start)*/
void EqpAgtLog( u8 byPrintLvl, s8* pszFmt, ... )
{
    s8  achPrintBuf[1024] = { 0 };
    va_list argptr;
    s32 nBufLen = 0;
    nBufLen = sprintf( achPrintBuf, "[EqpAgt]" );
    va_start( argptr, pszFmt );
    vsnprintf( achPrintBuf + nBufLen, 1024-1, pszFmt, argptr );
    va_end(argptr);
    LogPrint( byPrintLvl, MID_SNMP_EQPAGT, achPrintBuf);
    return;
}

// ��̬��ӡ
void StaticLog( LPCSTR lpszFmt, ...)
{
    s8 achBuf[1024] = { 0 };
    va_list argptr;    
    va_start( argptr, lpszFmt );
    vsnprintf(achBuf, 1024-1, lpszFmt, argptr );
    LogPrint( LOG_ERROR, MID_PUB_ALWAYS, achBuf);
    va_end( argptr );
}
/*lint -restore*/
