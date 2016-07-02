/*****************************************************************************
ģ����      : ������뿨
�ļ���      : hduapp.cpp
����ļ�    : hduapp.h
�ļ�ʵ�ֹ���: HDU������ʼ��
����        : ���ֱ�
�汾        : V1.0  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
2008/11/26              ���ֱ�       ����    
******************************************************************************/
#undef OSPEVENT
#define OSPEVENT( x, y ) OspAddEventDesc( #x, y )
#include "hduinst.h"
//#include "evhduid.h"
//#include "mcuver.h"
#include "hduapp.h"
#include "hduhwautotest.h"
#include "hduhwautotest_evid.h"

s32 g_nHduPLog = 0;
/*=============================================================================
�� �� ���� hduInit
��    �ܣ� hdu��ʼ��
�㷨ʵ�֣� 
ȫ�ֱ����� g_cHduApp
��    ���� TEqpCfg*
�� �� ֵ�� BOOL 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/11/27  4.6			���ֱ�                  ����
=============================================================================*/
BOOL hduInit(THduCfg* ptCfg)
{    
	if (NULL == ptCfg)
	{
		printf("[hduInit] ptCfg is Null!\n");
		return FALSE;
	}
	printf("enter hduInit()!\n");
    HduAPIEnableInLinux();

	if ( ptCfg->GetAutoTest() )
	{
		g_cHduAutoTestApp.CreateApp("hduauto", AID_HDUAUTO_TEST, APPPRI_HDUAUTO);	//����HDUAUTOTESTӦ��
	}
	else
	{
		// ��֤Ip��ַ����Ч��
		if( 0 == ptCfg->dwConnectIP && 0 == ptCfg->dwConnectIpB )
		{
			OspPrintf(TRUE, FALSE, "[hdu] The mcu's ip are all 0\n");
			return FALSE;
		}
		// ����һ��ͬһIp
		if(ptCfg->dwConnectIP == ptCfg->dwConnectIpB)
		{
			ptCfg->dwConnectIpB = 0;
			ptCfg->wConnectPortB = 0;
		}
		g_cHduApp.CreateApp("hdu", AID_HDU, APPPRI_HDU);	//����HDUӦ��
	}
	
    // �����¼�����
//  	#undef OSPEVENT
// 	#define OSPEVENT( x, y ) OspAddEventDesc( #x, y )
// 
// #ifdef _EVENET_HDU_H_
// 	#undef _EVENET_HDU_H_
// 	#include "evhduid.h"
// 	#define _EVENET_HDU_H_
// #else
// 	#include "evhduid.h"
//     #undef _EVENET_HDU_H_
// #endif
// 
// #ifdef _EV_MCUEQP_H_
// 	#undef _EV_MCUEQP_H_
// 	#include "evmcueqp.h"
// 	#define _EV_MCUEQP_H_
// #else
// 	#include "evmcueqp.h"
//     #undef  _EV_MCUEQP_H_
// #endif
	
	OspStatusMsgOutSet(FALSE);

	if ( ptCfg->GetAutoTest() )
	{
//		OspTaskDelay(10000);

		printf( "[hduautotest] enter auto test!\n" );
		// ��ʼ�ϵ��ʼ��
		u32 dwRet = 0;
		dwRet = ::OspPost(MAKEIID(AID_HDUAUTO_TEST, CInstance::DAEMON), EV_C_INIT, ptCfg, sizeof(THduCfg));
        if ( 0 == dwRet )
        {
			printf( "[hduInit] OspPost Sucecced! dwRet = %d\n", dwRet );
        }
		else
		{
            printf( "[hduInit] OspPost failed!\n dwRet = %d\n", dwRet );
		}
	}
	else
	{
		printf( "[hdu]  enter hdu app!\n" );
		// ��ʼ�ϵ��ʼ��
		::OspPost(MAKEIID(AID_HDU, CInstance::DAEMON), OSP_POWERON, ptCfg, sizeof(THduCfg));
	}

	//zjj20120107�����ɹ���־��ǰ��ע��mcu�ɹ�ǰ
/*
#ifdef _LINUX_
    s16 wRet = BrdSetSysRunSuccess();
    if ( wRet == UPDATE_VERSION_ROLLBACK )
    {
        printf( "[hduInit] Update failed, rollback version!\n" );
    }
    else if ( wRet == SET_UPDATE_FLAG_FAILED )
    {
        printf( "[hduInit] BrdSetSysRunSuccess() failed!\n" );
    }
	else if ( wRet == UPDATE_VERSION_SUCCESS )
	{
        printf( "[hduInit] BrdSetSysRunSuccess() succeed!\n" );
	}    
    else
	{
		printf( "[hduInit] BrdSetSysRunSuccess(), NO_UPDATE_OPERATION\n");
	}
#endif
*/

    if ( ptCfg->GetAutoTest() )
	{
		OspSetPrompt("hduauto");
	}
	else
	{
		OspSetPrompt("hdu");
	}

	return TRUE;
}

/*=============================================================================
�� �� ���� HduAPIEnableInLinux
��    �ܣ� ע��linux���Խӿ�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/11/27  4.6			���ֱ�                  ����
=============================================================================*/
void HduAPIEnableInLinux(void)
{
	//lint -save -e611
#ifdef _LINUX_  
    OspRegCommand("hduhelp",        (void*)hduhelp,         "hdu help");
    OspRegCommand("sethdulog",      (void*)sethdulog,       "set hdulog(0,1,2)");
    OspRegCommand("hdushow",        (void*)hdushow,         "hdu show");
    OspRegCommand("hduinfo",        (void*)hduinfo,         "hdu info");
    OspRegCommand("hduver",         (void*)hduver,          "hdu ver");
    OspRegCommand("hdutau",         (void*)hdutau,          "hdu telnet author");
    OspRegCommand("setmode",        (void*)sethdumode,      "set output mode");
    OspRegCommand("showmode",       (void*)showmode,        "show output mode");
    OspRegCommand("setautotest",    (void*)setautotest,      "change auto test");
	RegsterCommands();
#endif
	//lint -restore
}

/*=============================================================================
�� �� ���� hduver
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/11/27  4.6			���ֱ�                  ����
=============================================================================*/
API void hduver(void)
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
    OspPrintf( TRUE, FALSE, "HDU Version: %s\n", gs_VersionBuf );
	OspPrintf(TRUE, FALSE, "\thdu version : %s compile time: %s    %s\n", VER_HDU, __TIME__, __DATE__);
}


/*=============================================================================
�� �� ���� hduhelp
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/11/27  4.6			���ֱ�                  ����
=============================================================================*/
API void hduhelp(void)
{
    OspPrintf( TRUE, FALSE, "[Hdu]: ---- All the help selection as follows ----\n");
	hduver();
	OspPrintf( TRUE, FALSE, "\thdushow     : HDU status show \n" );
	OspPrintf( TRUE, FALSE, "\tsethdulog(i): Set debug level on screen. 0-nprint, 1/2-print\n\n" );
    OspPrintf( TRUE, FALSE, "\thdutau(UsrName, Pwd) : HDU telnet author \n" );
}


/*=============================================================================
�� �� ���� sethdulog
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/11/27  4.6			���ֱ�                  ����
=============================================================================*/
API void sethdulog(int nlvl)
{
    g_nHduPLog = nlvl;   
    return;
}

/*=============================================================================
�� �� ���� hduscreen
��    �ܣ� ������Ļ��ӡ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/11/27  4.6			���ֱ�                  ����
=============================================================================*/
API void hduscreen(void)
{
    sethdulog(1);
	return;
}

/*=============================================================================
�� �� ���� hdushow
��    �ܣ� ��ʾ״̬
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/11/27  4.6			���ֱ�                  ����
=============================================================================*/
API void hdushow(void)
{
	::OspPost(MAKEIID( AID_HDU,CInstance::EACH ), EV_HDU_STATUSSHOW);
    return;
}

/*=============================================================================
�� �� ���� hduinfo
��    �ܣ� ��ʾ������Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/11/27  4.6			���ֱ�                  ����
=============================================================================*/
API	void hduinfo(void)
{
	::OspPost(MAKEIID( AID_BAS,CInstance::EACH ), EV_HDU_CONFIGSHOW);
    return;
}

/*=============================================================================
�� �� ���� hdutau
��    �ܣ� telnet��Ȩ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/11/27  4.6			���ֱ�                  ����
=============================================================================*/
API void hdutau( LPCSTR lpszUsrName/* = NULL */, LPCSTR lpszPwd /* = NULL */ )
{
    OspTelAuthor( lpszUsrName, lpszPwd );
    return;
}

/*=============================================================================
�� �� ���� showmode
��    �ܣ� ��ʾhdu���������ʽ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/11/27  4.6			���ֱ�                  ����
=============================================================================*/
API void showmode(void)
{
	THduModePort tModePort1, tModePort2;
	memset(&tModePort1, 0x0, sizeof(tModePort1));
	memset(&tModePort2, 0x0, sizeof(tModePort2));
	g_cHduApp.m_tCfg.GetHduModePort(0, tModePort1);
	g_cHduApp.m_tCfg.GetHduModePort(1, tModePort2);
				
	OspPrintf(TRUE, FALSE," m_byOutPortType1 = %d, m_byOutModeType1 = %d\nm_byOutPortType2 = %d, m_byOutModeType2 = %d\n",
					tModePort1.GetOutPortType(), tModePort1.GetOutModeType(),
					tModePort2.GetOutPortType(), tModePort2.GetOutModeType()
		       );
	return;
}

/*=============================================================================
�� �� ���� sethdumode
��    �ܣ� ����hdu���������ʽ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byNum  ��hduͨ����  0��1
           u8 byPort ��VGA��YPbPr
		   u8 byMode
		   u8 byPlayScale--->0��ʾ4:3  1��ʾ16:9
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/11/27  4.6			���ֱ�                  ����
=============================================================================*/
API void sethdumode( u8 byNum, u8 byPort, u8 byMode, u8 byPlayScale)
{
	THduModePort tModePort;
	memset(&tModePort, 0x0, sizeof(tModePort));
	tModePort.SetOutModeType(byMode);
	tModePort.SetOutPortType(byPort);
	tModePort.SetZoomRate( byPlayScale );
	g_cHduApp.m_tCfg.SetHduModePort(byNum, tModePort);
	::OspPost(MAKEIID( AID_HDU, byNum+1 ), EV_HDU_SETMODE);
    return;
}

/*=============================================================================
�� �� ���� setautotest
��    �ܣ� �����Ƿ�Ӳ���Զ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� s32 :���Ա�־��0 --> ���Զ����ԣ�1 --> �Զ�����
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/11/27  4.6			���ֱ�                  ����
=============================================================================*/
API void setautotest( s32 nIsAutoTest )
{
    ::OspPost( MAKEIID( AID_HDUAUTO_TEST, CHduAutoTestClient::DAEMON ), EV_C_CHANGEAUTOTEST_CMD, &nIsAutoTest, sizeof(s32) );
    return;
}


