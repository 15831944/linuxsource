/*****************************************************************************
   ģ����      : ¼���Ӧ��
   �ļ���      : RecApp.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ����¼���Ӧ��
   ����        : 
   �汾        : V3.5  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
******************************************************************************/
#include "ChnInst.h"
//#include "ConfAgent.h"
//#include "evrec.h"
#include "mcuver.h"
#include "RecApp.h"
//#include "KdvMediaNet.h"

#ifdef _RECSERV_
#include "RecServ_Def.h"
#include "RecServInst.h"
extern CRecServApp g_RecServApp;
#endif


extern CRecApp g_cRecApp;


/*====================================================================
    ������	     ��UserInit
	����		 ��¼���Ӧ��ʵ����ں���
	����ȫ�ֱ��� ����
    �������˵�� ��ptConf - TCfgAgentָ��
	����ֵ˵��   ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
API void UserInit()
{
	#undef OSPEVENT
	#define OSPEVENT( x, y ) OspAddEventDesc( #x, y )
	
#ifdef _EV_MCUEQP_H_
	#undef _EV_MCUEQP_H_
	#include "evmcueqp.h"
	#define _EV_MCUEQP_H_
#else
	#include "evmcueqp.h"
    #undef  _EV_MCUEQP_H_
    #undef _EV_MCUEQP_H_
#endif

#ifdef EVREC_H
	#undef EVREC_H
	#include "evrec.h"
	#define EVREC_H
#else
	#include "evrec.h"
	#undef EVREC_H
#endif

#ifdef _DEBUG
    ::OspInit(TRUE, BRD_TELNET_PORT);
#else
    ::OspInit(FALSE, BRD_TELNET_PORT);
    
    //Osp telnet ��ʼ��Ȩ [11/28/2006-zbq]
    OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );

#endif

   	// ��ʼ��KdvLogģ��
	Err_t err =LogInit(g_tCfg.m_szCfgFilename);
	if(LOG_ERR_NO_ERR != err)
	{
		printf("KdvLog Init failed, err is %d\n", err);
	}

	g_cRecApp.CreateApp( "Recorder", AID_RECORDER, APPPRI_REC );
    
#ifdef _RECSERV_
    if (OSP_OK != g_RecServApp.CreateApp( "RecServer", AID_REC_SERVER, APPPRI_REC ))
    {
        return;
    }    
#endif // end of _RECSERV_
	::OspSetPrompt("Rec");

	::recSetReleaseScreen();

    ::OspPost( MAKEIID(AID_RECORDER, CInstance::DAEMON), EV_REC_POWERON );
    ::OspPost( MAKEIID(AID_REC_SERVER, CInstance::DAEMON), EV_REC_POWERON );
    
    return;
}

/*=============================================================================
  �� �� ���� RestartRecorder
  ��    �ܣ� ����¼���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� API void 
=============================================================================*/
API void RestartRecorder(HWND hMainDlg)
{
    ::OspPost( MAKEIID(AID_RECORDER, CInstance::DAEMON), EV_REC_RESTART );

    PostMessage( hMainDlg, WM_REC_RESTART, NULL, NULL );
}

/*====================================================================
    ������	     ��recver
	����		 ����ӡrecorderģ���Լ���ؿ�İ汾��Ϣ
	����ȫ�ֱ��� ����
    �������˵�� ����
	����ֵ˵��   ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
API void recver()
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
        sprintf(gs_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
    }
	OspPrintf( TRUE, FALSE, "RECORDER Version: %s\n", gs_VersionBuf);
    ::OspPrintf( TRUE, FALSE, "=======================================================================\n" );
	::OspPrintf( TRUE, FALSE, "Recorder Compile Time : %s, %s\n", __TIME__, __DATE__ );
	::OspPrintf( TRUE, FALSE, "Recorder Verion       : %s\n", VER_RECORDER );
    ::OspPrintf( TRUE, FALSE, "=======================================================================\n\n" );

	kdvmedianetver();
	rpver();
	ospver();
}

/*====================================================================
    ������	     ��rechelp
	����		 ����ӡrecorderģ�������Ϣ
	����ȫ�ֱ��� ����
    �������˵�� ����
	����ֵ˵��   ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
API void rechelp()
{
    ::OspPrintf( TRUE, FALSE, "=======================================================================\n" );
	::OspPrintf( TRUE, FALSE, "Recorder Compile Time : %s, %s\n", __TIME__, __DATE__ );
	::OspPrintf( TRUE, FALSE, "Recorder Verion       : %s\n", VER_RECORDER );
    ::OspPrintf( TRUE, FALSE, "=======================================================================\n" );

	::OspPrintf( TRUE, FALSE, "precmsg               : Print Record Message .\n");	
	::OspPrintf( TRUE, FALSE, "recStatusShow         : Show Recorder Status\n");
	::OspPrintf( TRUE, FALSE, "showdebuginfo         : Show Recorder debug info\n");

	::OspPrintf( TRUE, FALSE, "kdvmedianethelp       : \n");
	::OspPrintf( TRUE, FALSE, "rphelp                : \n");
    ::OspPrintf( TRUE, FALSE, "rectau(UsrName, Pwd)  : recorder telnet author.\n");

    ::OspPrintf( TRUE, FALSE, "recservhelp           : Show Helps about recorder server\n");
}

/*====================================================================
    ������	     ��recSetDebugScreen
	����		 ��������Ļ��ӡ����
	����ȫ�ֱ��� ����
    �������˵�� ����
	����ֵ˵��   ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
API void recscreen()
{
	::OspSetScrnLogLevel( 0, LOGLVL_DEBUG2 );
	::OspSetScrnLogLevel( AID_RECORDER, LOGLVL_DEBUG2 );
}

/*====================================================================
    ������	     ��recSetReleaseScreen
	����		 ��������Ļ��ӡ����
	����ȫ�ֱ��� ����
    �������˵�� ����
	����ֵ˵��   ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
API void recSetReleaseScreen()
{
	::OspSetScrnLogLevel( 0,  LOGLVL_IMPORTANT );
	::OspSetScrnLogLevel( AID_RECORDER,  LOGLVL_IMPORTANT );
}

/*====================================================================
    ������	     : status show
	����		 ��
	����ȫ�ֱ��� ����
    �������˵�� ����
	����ֵ˵��   ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
API void recStatusShow()
{
	::OspPost( MAKEIID( AID_RECORDER,CInstance::DAEMON), EV_REC_STATUSSHOW);
}

/*====================================================================
    ������	     ��recQuit
	����		 ��recorder�˳�
	����ȫ�ֱ��� ����
    �������˵�� ����
	����ֵ˵��   ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
API void recQuit()
{
	// [2013/1/7 zhangli]�˽ӿ���ʱû���õ������ṩ�˽ӿڹ��ܡ���ϢEV_REC_QUIT����Ϊ¼����˳�
// 	 OspSend(MAKEIID(AID_RECORDER, CInstance::DAEMON), EV_REC_QUIT, 
// 		            0, 0, 0,
// 					MAKEIID(INVALID_APP, INVALID_INS),
// 					INVALID_NODE,
// 					NULL, 0, NULL, 
// 					10000);
// 	::OspQuit();
}

/*====================================================================
    ������	     : rectau
	����		 ��
	����ȫ�ֱ��� ����
    �������˵�� ����
	����ֵ˵��   ����
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    06/11/28    4.0         �ű���          ����
====================================================================*/
API void rectau( LPCSTR lpszUsrName = NULL, LPCSTR lpszPwd = NULL )
{
    OspTelAuthor( lpszUsrName, lpszPwd );
} 