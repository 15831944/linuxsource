/*****************************************************************************
   ģ����      : BAS
   �ļ���      : basapp.cpp
   ����ļ�    : basapp.h
   �ļ�ʵ�ֹ���: BAS������ʼ��
   ����        : zsh
   �汾        : V1.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/11/10  2.0         zsh         ����
******************************************************************************/
#include "basapp.h"
#include "basinst.h"
#include "evbas.h"
#include "mcuver.h"

extern CBasApp g_cBasApp;

API void basver()
{
    s8 gs_VersionBuf[128] = {0};
    {
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
            sprintf(gs_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
        }
    }
    OspPrintf( TRUE, FALSE, "Bas Version: %s\n", gs_VersionBuf );
	OspPrintf(TRUE, FALSE, "\tbas version : %s compile time: %s    %s\n", VER_BAS, __TIME__, __DATE__);
}

API void bashelp()
{
    OspPrintf( TRUE, FALSE, "[Bas]: ---- All the help selection as follows ----\n");
	basver();
	OspPrintf( TRUE, FALSE, "\tbasshow     : BAS status show \n" );
	OspPrintf( TRUE, FALSE, "\tsetbaslog(i): Set debug level on screen. 0-nprint, 1-print\n\n" );
    OspPrintf( TRUE, FALSE, "\tbastau(UsrName, Pwd) : BAS telnet author \n" );
}

// ������Ļ��ӡ����
API void basscreen()
{
	::OspSetScrnLogLevel(0, LOGLVL_DEBUG2);
	::OspSetScrnLogLevel(AID_BAS, LOGLVL_DEBUG2);
}

// ���õ�����Ϣ
extern s32 g_nbaslog;

API void setbaslog(int nlvl)
{
    if ( nlvl >= 1 )
    {
        g_nbaslog = 1;
    }    
    return;
}

// ��ʾ״̬
API void basshow()
{
	::OspPost(MAKEIID( AID_BAS,CInstance::DAEMON ), EV_BAS_SHOW);
}

// ��ʾ������Ϣ
API	void basinfo()
{
	::OspPost(MAKEIID( AID_BAS,CInstance::DAEMON ), EV_BAS_SHOWINFO);
}

// telnet��Ȩ
API void bastau( LPCSTR lpszUsrName = NULL, LPCSTR lpszPwd = NULL )
{
    OspTelAuthor( lpszUsrName, lpszPwd );
}


/*=============================================================================
�� �� ���� BasAPIEnableInLinux
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
void BasAPIEnableInLinux(void)
{
	//lint -save -e611
#ifdef _LINUX_  
    OspRegCommand("bashelp",        (void*)bashelp,         "bas help");
    OspRegCommand("setbaslog",      (void*)setbaslog,       "set baslog");
    OspRegCommand("basshow",        (void*)basshow,         "bas show");
    OspRegCommand("basinfo",        (void*)basinfo,         "bas info");
    OspRegCommand("basver",         (void*)basver,          "bas ver");
    OspRegCommand("bastau",         (void*)bastau,          "bas telnet author");
#endif
	//lint -restore
}

// ��ʼ��
BOOL basInit(TEqpCfg* ptCfg)
{    
	/*lint -save -e522*/
    BasAPIEnableInLinux();
	/*lint -restore*/

	// ��֤Ip��ַ����Ч��
	if( 0 == ptCfg->dwConnectIP && 0 == ptCfg->dwConnectIpB )
    {
        OspPrintf(TRUE, FALSE, "[bas] The mcu's ip are all 0\n");
        return FALSE;
    }
	// ����һ��ͬһIp
    if(ptCfg->dwConnectIP == ptCfg->dwConnectIpB)
    {
        ptCfg->dwConnectIpB = 0;
        ptCfg->wConnectPortB = 0;
    }

	g_cBasApp.CreateApp("bas", AID_BAS, APPPRI_BAS);	//����BASӦ��

// �����¼�����
 	#undef OSPEVENT
	#define OSPEVENT( x, y ) OspAddEventDesc( #x, y )

#ifdef _EVENT_BAS_H_
	#undef _EVENT_BAS_H_
	//#include "evbas.h"
	#define _EVENT_BAS_H_
#else
	//#include "evbas.h"
    #undef _EVENT_BAS_H_
#endif

#ifdef _EV_MCUEQP_H_
	#undef _EV_MCUEQP_H_
	#include "evmcueqp.h"
	#define _EV_MCUEQP_H_
#else
	#include "evmcueqp.h"
    #undef  _EV_MCUEQP_H_
#endif
	
	OspStatusMsgOutSet(FALSE);
	
	// ��ʼ�ϵ��ʼ��
	::OspPost(MAKEIID(AID_BAS, CInstance::DAEMON), EV_BAS_INI, ptCfg, sizeof(TEqpCfg));
	
	return TRUE;
}


