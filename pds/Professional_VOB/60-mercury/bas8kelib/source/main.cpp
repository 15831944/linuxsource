/*****************************************************************************
   ģ����      : ���渴��������
   �ļ���      : main.cpp
   ����ʱ��    : 2003�� 12�� 4��
   ʵ�ֹ���    : 
   ����        : zhangsh
   �汾        : 
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
******************************************************************************/
#include "osp.h"
#include "basinst.h"
//#include "mcustruct.h"
//#include "evmcueqp.h"
//#include "eqpcfg.h"
#include "mcuver.h"


extern s32 g_nBaslog;
extern u16 g_wBasLastEvent;
extern u32 g_dwBasLastFileLine;

CBasApp g_cBasApp;
//CLogManager g_cBasLogMgr;


void BasAPIEnableInLinux(void);
/*lint -save -e714*/
BOOL basInit(T8keBasCfg * ptBasCfg)
{
    BasAPIEnableInLinux();

    if( 0 == ptBasCfg->GetConnectIp()  )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "\n[Bas]The param error\n");
		printf("\n[Bas]The param error\n");
        return FALSE;
    }
	g_cBasApp.CreateApp("bas", AID_BAS, APPPRI_BAS, 80, 20<<20);
	
    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "bas change stack to 20M\n");
	printf( "bas change stack to 20M\n");
    #undef OSPEVENT
    #define OSPEVENT(x, y) OspAddEventDesc(#x, y)
	/*lint -save -e750*/
#ifdef _EV_MCUEQP_H_
    #undef _EV_MCUEQP_H_
    //#include "evmcueqp.h"
    #define _EV_MCUEQP_H_
#else
    //#include "evmcueqp.h"
    #undef _EV_MCUEQP_H_
#endif   
    
    // ��ʼ����־
    //g_cBasLogMgr.SetDirPath( DIR_LOG );
    //g_cBasLogMgr.SetFileTitle( "baslog" );
    //g_cBasLogMgr.SetLogPrefix( "bas" );
    
    //FIXME here
    OSPEVENT( EV_BAS_TIMER,                EV_BAS_BGN );                //�ڲ���ʱ��
    OSPEVENT( EV_BAS_NEEDIFRAME_TIMER,     EV_BAS_BGN + 1 );            //�ؼ�֡��ʱ��
    OSPEVENT( EV_BAS_CONNECT_TIMER,        EV_BAS_BGN + 2 );            //���Ӷ�ʱ��
    OSPEVENT( EV_BAS_CONNECT_TIMERB,       EV_BAS_BGN + 3 );            //���Ӷ�ʱ��
    OSPEVENT( EV_BAS_REGISTER_TIMER,       EV_BAS_BGN + 4 );            //ע�ᶨʱ��
    OSPEVENT( EV_BAS_REGISTER_TIMERB,      EV_BAS_BGN + 5 );            //ע�ᶨʱ��
    OSPEVENT( TIME_GET_MSSTATUS,           EV_BAS_BGN + 6 );            //ȡ��������״̬
    OSPEVENT( TIME_ERRPRINT,               EV_BAS_BGN + 7 );            //�����ӡ��ʱ��
     
	/*int nRet = */::OspPost(MAKEIID(AID_BAS, CInstance::DAEMON), OSP_POWERON, ptBasCfg, sizeof(T8keBasCfg));

    return TRUE;
}

/*lint -save -e765*/
API void basstatus()
{
    OspPost(MAKEIID(AID_BAS, 1), EV_BAS_SHOW);
}

API void pbasmsg(void)
{
    g_nBaslog = 1;
	logsetlvl(LOG_LVL_DETAIL);
}

API void npbasmsg(void)
{
    g_nBaslog = 0;
}

API void basshowlastevent()
{
    OspPrintf(TRUE, FALSE, "g_wVmpLastEvent=%u(%s), File Line: %u\n", g_wBasLastEvent, ::OspEventDesc(g_wBasLastEvent), g_dwBasLastFileLine);
}
API void basver(void)
{
    OspPrintf(TRUE, FALSE, "bas version:%s\t",VER_BAS);
    OspPrintf(TRUE, FALSE, "compile time:%s:%s\n", __DATE__, __TIME__);
}

API void bashelp()
{
    OspPrintf(TRUE, FALSE, "bas version:%s\t",VER_BAS);
    OspPrintf(TRUE, FALSE, "compile time:%s:%s\n", __DATE__, __TIME__);
    OspPrintf(TRUE, FALSE, "basstatus  ---- Display bas status\n");
    OspPrintf(TRUE, FALSE, "pbasmsg    ---- Display the debug information.\n");
    OspPrintf(TRUE, FALSE, "npbasmsg   ---- Not display the debug information.\n");
    OspPrintf(TRUE, FALSE, "basshowlastevent   ---- Display lastest event procceeding.\n");
	OspPrintf(TRUE, FALSE, "basshow		 ---- Show bas param\n");
    
}


// ��ӡҵ��ϳɲ���, zgc, 2008-03-19
API void basshow(void)
{
    //OspPost(MAKEIID(AID_BAS, 1), EV_VMP_SHOWPARAM);
}

// ���Բ�����ֹͣVMP�, zgc, 2007/04/24
// API void stopbaseqp(void)
// {
// 	//OspPost( MAKEIID(AID_BAS, 1), MCU_VMP_STOPMAP_CMD );
// }

/*=============================================================================
 �� �� ���� setBBOut
 ��    �ܣ� �Ƿ�ʹ�ñ������
 �㷨ʵ�֣� 
 ȫ�ֱ����� 
 ��    ���� void
 �� �� ֵ�� void 
 ----------------------------------------------------------------------
 �޸ļ�¼    ��
 ��  ��		�汾		�޸���		�߶���    �޸�����
 2006/8/15  4.0			������                  ����
=============================================================================*/
// API void setBBOut( u8 byOut )
// {
//     if ( byOut > 1 )
//     {
//         byOut = 1;
//     }
//     CServMsg cServMsg;
//     cServMsg.SetMsgBody( (u8*)&byOut, sizeof(u8) );
//     OspPost( MAKEIID(AID_VMP, 1), EV_VMP_BACKBOARD_OUT_CMD, 
//              cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
// }


void BasAPIEnableInLinux(void)
{
	/*lint -save -e611*/
#ifdef _LINUX_      
    OspRegCommand("basstatus",          (void*)basstatus,           "basstatus");
	OspRegCommand("basshow",		    (void*)basshow,		        "basshow");
    OspRegCommand("pbasmsg",            (void*)pbasmsg,             "pbasmsg");
    OspRegCommand("npbasmsg",           (void*)npbasmsg,            "npbasmsg");
    OspRegCommand("basver",             (void*)basver,              "basver");
    OspRegCommand("bashelp",            (void*)bashelp,             "bashelp");
    OspRegCommand("basshowlastevent",   (void*)basshowlastevent,    "basshowlastevent");    
#endif
	/*lint -restore*/
}
