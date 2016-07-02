/*****************************************************************************
   ģ����      : MpwLib�໭�渴�ϵ���ǽ
   �ļ���      : mpwapp.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ʵ��mpw�����������Խӿ�
   ����        : john
   �汾        : V4.0  Copyright(C) 2001-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2005/09/25  1.0         john         ����
******************************************************************************/
#include "mpwcfg.h"
#include "mpwinst.h"
#include "evmpw.h"
#include "mcuver.h"

extern CMpwApp g_cMpwApp;

extern s32 g_nMpwlog;

void MpwAPIEnableInLinux(void);

/*=============================================================================
  �� �� ���� MpwInit
  ��    �ܣ� ����Mpw
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TVmpCfg& tVmpCfg
  �� �� ֵ�� BOOL32��TRUE���ɹ���FALSE��ʧ�� 
=============================================================================*/
BOOL32 MpwInit(TVmpCfg& tVmpCfg)
{
    MpwAPIEnableInLinux();
    
    if(0 == tVmpCfg.dwConnectIP && 0 == tVmpCfg.dwConnectIpB)
    {
        OspPrintf(TRUE, FALSE, "[Mpw] The mcu's ip are all 0\n");
        return FALSE;
    }

    if(tVmpCfg.dwConnectIP == tVmpCfg.dwConnectIpB)
    {
        tVmpCfg.dwConnectIpB = 0;
        tVmpCfg.wConnectPortB = 0;
    }

    g_cMpwApp.CreateApp( "MpwInst", AID_VMPTW, APPPRI_MPW );
    OspPrintf( TRUE, FALSE, "[Mpw] Initalizing the Mpw !\n" );

    #undef OSPEVENT
    #define OSPEVENT(x, y) OspAddEventDesc(#x, y)
#ifdef _EV_MCUEQP_H_
    #undef _EV_MCUEQP_H_
    #include "evmcueqp.h"
    #define _EV_MCUEQP_H_
#else
    #include "evmcueqp.h"
    #undef _EV_MCUEQP_H_
#endif
    
    OspPost( MAKEIID(AID_VMPTW, 1), EV_VMPTW_INIT, &tVmpCfg, sizeof(tVmpCfg) );
    return TRUE;
}

/*=============================================================================
  �� �� ���� mpwvershow
  ��    �ܣ� ��ʾ�汾
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� API void 
=============================================================================*/
API void mpwvershow(void)
{
	s8 gs_VersionBuf[128] = {0};
	
    strcpy(gs_VersionBuf, KDV_MCU_PREFIX);
    
    s8 achMon[16] = {0};
    u32 byDay = 0;
    u32 byMonth = 0;
    u32 wYear = 0;
    s8 achFullDate[24] = {0};
    
    s8 achDate[32] = {0};
    sprintf(achDate, "%s", __DATE__);
    StrUpper(achDate);
    
    sscanf(achDate, "%s %d %d", achMon, &byDay, &wYear );
    
    if ( 0 == strcmp( achMon, "JAN") )		 
        byMonth = 1;
    else if ( 0 == strcmp( achMon, "FEB") )
        byMonth = 2;
    else if ( 0 == strcmp( achMon, "MAR") )
        byMonth = 3;
    else if ( 0 == strcmp( achMon, "APR") )		 
        byMonth = 4;
    else if ( 0 == strcmp( achMon, "MAY") )
        byMonth = 5;
    else if ( 0 == strcmp( achMon, "JUN") )
        byMonth = 6;
    else if ( 0 == strcmp( achMon, "JUL") )
        byMonth = 7;
    else if ( 0 == strcmp( achMon, "AUG") )
        byMonth = 8;
    else if ( 0 == strcmp( achMon, "SEP") )		 
        byMonth = 9;
    else if ( 0 == strcmp( achMon, "OCT") )
        byMonth = 10;
    else if ( 0 == strcmp( achMon, "NOV") )
        byMonth = 11;
    else if ( 0 == strcmp( achMon, "DEC") )
        byMonth = 12;
    else
        byMonth = 0;
    
    if ( byMonth != 0 )
    {
        sprintf(achFullDate, "%04d%02d%02d", wYear, byMonth, byDay);
        sprintf(gs_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
    }
    else
    {
        // for debug information
        sprintf(gs_VersionBuf, ":%s%s", KDV_MCU_PREFIX, achFullDate);        
    }
	OspPrintf( TRUE, FALSE, "MPW Version: %s\n", gs_VersionBuf);
    OspPrintf( TRUE, FALSE, "[Help] Mpw version: %s\n", VER_MPW );
    OspPrintf( TRUE, FALSE, "[Help] Mpw compile time: %s %s\n", __DATE__, __TIME__ );
    return;
}

/*=============================================================================
  �� �� ���� mpwhelpshow
  ��    �ܣ� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� API void 
=============================================================================*/
API void mpwhelp(void)
{
    OspPrintf( TRUE, FALSE, "[Help] mpwshowstatus --  show mpw status\n" );
    OspPrintf( TRUE, FALSE, "[Help] mpwvershow    --  show mpw version\n" );
    OspPrintf( TRUE, FALSE, "[Help] pmpwmsg       --  print mpw message\n" );
    OspPrintf( TRUE, FALSE, "[Help] npmpwmsg      --  not print mpw message\n" );
    OspPrintf( TRUE, FALSE, "[Help] mpwtau(UsrName, Pwd) -- MPW telnet author\n" );
    return;
}

/*=============================================================================
  �� �� ���� mpwshowstatus
  ��    �ܣ� ��ʾmpw״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� API void 
=============================================================================*/
API void mpwshowstatus(void)
{
    OspPost( MAKEIID(AID_VMPTW, 1), EV_VMPTW_DISPLAYALL, NULL, 0 );
    return;
}

/*=============================================================================
  �� �� ���� mpwtau
  ��    �ܣ� MPW ��telnet��Ȩ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� API void 
=============================================================================*/
API void mpwtau( LPCSTR lpszUsrName = NULL, LPCSTR lpszPwd = NULL )
{
    OspTelAuthor( lpszUsrName, lpszPwd );
}

/*=============================================================================
  �� �� ���� mpwtest
  ��    �ܣ� ��ӡ������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� API void 
=============================================================================*/
API void pmpwmsg(void)
{
    g_nMpwlog = 1;
    return;
}

/*=============================================================================
  �� �� ���� npmpwmsg
  ��    �ܣ� �رյ��Դ�ӡ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� API void 
=============================================================================*/
API void npmpwmsg(void)
{
    g_nMpwlog = 0;
    return;
}


/*=============================================================================
�� �� ���� MpwAPIEnableInLinux
��    �ܣ� ע��linux���Խӿ�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/8/15  4.0			������                  ����
=============================================================================*/
void MpwAPIEnableInLinux(void)
{
#ifdef _LINUX_    
    OspRegCommand("mpwvershow",        (void*)mpwvershow,       "mpwvershow");
    OspRegCommand("mpwhelp",           (void*)mpwhelp,          "mpwhelpshow");
    OspRegCommand("mpwshowstatus",     (void*)mpwshowstatus,    "mpwshowstatus");
    OspRegCommand("pmpwmsg",           (void*)pmpwmsg,          "pmpwmsg");
    OspRegCommand("npmpwmsg",          (void*)npmpwmsg,         "npmpwmsg");
    OspRegCommand("mpwtau",            (void*)mpwtau,           "mpw telnet author");
#endif
}
