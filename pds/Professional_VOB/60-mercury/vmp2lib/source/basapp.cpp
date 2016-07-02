/*****************************************************************************
   ģ����      : BAS
   �ļ���      : basapp.cpp
   ����ļ�    : basapp.h
   �ļ�ʵ�ֹ���: BAS������ʼ��
   ����        : ����
   �汾        : V1.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/11/10  2.0         zsh         ����
   2008/08/08  4.5         ����      BAS-HD
******************************************************************************/
#include "basapp.h"
#include "basinst.h"
#include "evbas.h"
#include "mcuver.h"

extern CBasApp g_cBasApp;

API void vmpver()
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
    OspPrintf( TRUE, FALSE, "Vmp Version: %s\n", gs_VersionBuf );

	OspPrintf(TRUE, FALSE, "\tvmp version : %s compile time: %s    %s\n", VER_VMPHD, __TIME__, __DATE__);
}

API void vmphelp()
{
    OspPrintf( TRUE, FALSE, "[Bas]: ---- All the help selection as follows ----\n");
	vmpver();
	OspPrintf( TRUE, FALSE, "\tvmpshow     : vmp status show \n" );
	OspPrintf( TRUE, FALSE, "\tsetvmplog(i): Set debug level on screen. 0-nprint, 1/2-print\n\n" );
    OspPrintf( TRUE, FALSE, "\tvmptau(UsrName, Pwd) : vmp telnet author \n" );
}


// ���õ�����Ϣ
extern s32 g_nbaslog;

API void setvmplog(int nlvl)
{
    g_nbaslog = nlvl;   
    return;
}

// ������Ļ��ӡ����
API void vmpscreen()
{
    setvmplog(1);
}

// ��ʾ״̬
API void vmpshow()
{
	::OspPost(MAKEIID( AID_BAS,CInstance::EACH ), EV_BAS_SHOW);
}

// ��ʾ������Ϣ
API	void vmpinfo()
{
	::OspPost(MAKEIID( AID_BAS,CInstance::EACH ), EV_BAS_SHOWINFO);
}

// telnet��Ȩ
API void vmptau( LPCSTR lpszUsrName = NULL, LPCSTR lpszPwd = NULL )
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
#ifdef _LINUX_  
    OspRegCommand("vmphelp",        (void*)vmphelp,         "vmp help");
    OspRegCommand("setvmplog",      (void*)setvmplog,       "set vmplog(0,1,2)");
    OspRegCommand("vmpshow",        (void*)vmpshow,         "vmp show");
    //OspRegCommand("basinfo",        (void*)basinfo,         "bas info");
    OspRegCommand("vmpver",         (void*)vmpver,          "vmp ver");
    OspRegCommand("vmptau",         (void*)vmptau,          "vmp telnet author");
#endif
}

// ��ʼ��
BOOL basInit(TEqpCfg* ptCfg)
{    
    BasAPIEnableInLinux();

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
	#include "evbas.h"
	#define _EVENT_BAS_H_
#else
	#include "evbas.h"
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
	::OspPost(MAKEIID(AID_BAS, CInstance::DAEMON), OSP_POWERON, ptCfg, sizeof(TEqpCfg));

#ifdef _LINUX_
    u8 byRet = BrdSetSysRunSuccess();
    if ( byRet == UPDATE_VERSION_ROLLBACK )
    {
        OspPrintf( TRUE, FALSE, "[basInit] Update failed, rollback version!\n" );
    }
    else if ( byRet == SET_UPDATE_FLAG_FAILED )
    {
        OspPrintf( TRUE, FALSE, "[basInit] BrdSetSysRunSuccess() failed!\n" );
    }
#endif
	
	return TRUE;
}


