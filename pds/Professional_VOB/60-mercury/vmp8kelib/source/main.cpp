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
#include "vmpinst.h"
//#include "mcustruct.h"
//#include "evmcueqp.h"
//#include "eqpcfg.h"
#include "mcuver.h"

extern s32 g_nVmplog;
extern u16 g_wVmpLastEvent;
extern u32 g_dwVmpLastFileLine;

CVmpApp g_cVMPApp;
//CLogManager g_cVmpLogMgr;
/*lint -save -e765 -e526 -e714*/
void VmpAPIEnableInLinux(void);

BOOL vmpinit(T8keVmpCfg * ptVmpCfg)
{
    VmpAPIEnableInLinux();
    
    if( 0 == ptVmpCfg->GetConnectIp()  )
    {
		LogPrint(LOG_LVL_ERROR,MID_MCU_EQP,"\n[Vmp]The param error\n");
        OspPrintf(TRUE, FALSE, "\n[Vmp]The param error\n");
        return FALSE;
    }

    g_cVMPApp.CreateApp("vmp", AID_VMP, APPPRI_VMP);

    #undef OSPEVENT
    #define OSPEVENT(x, y) OspAddEventDesc(#x, y)


    
    // ��ʼ����־
    //g_cVmpLogMgr.SetDirPath( DIR_LOG );
    //g_cVmpLogMgr.SetFileTitle( "vmplog" );
    //g_cVmpLogMgr.SetLogPrefix( "vmp" );
    
    //FIXME here
    OSPEVENT( EV_VMP_TIMER,                EV_VMP_BGN );                //�ڲ���ʱ��
    OSPEVENT( EV_VMP_NEEDIFRAME_TIMER,     EV_VMP_BGN + 1 );            //�ؼ�֡��ʱ��
    OSPEVENT( EV_VMP_CONNECT_TIMER,        EV_VMP_BGN + 21 );            //���Ӷ�ʱ��
    OSPEVENT( EV_VMP_CONNECT_TIMERB,       EV_VMP_BGN + 22 );            //���Ӷ�ʱ��
    OSPEVENT( EV_VMP_REGISTER_TIMER,       EV_VMP_BGN + 23 );            //ע�ᶨʱ��
    OSPEVENT( EV_VMP_REGISTER_TIMERB,      EV_VMP_BGN + 24 );            //ע�ᶨʱ��
    OSPEVENT( TIME_GET_MSSTATUS,           EV_VMP_BGN + 25 );            //ȡ��������״̬
    OSPEVENT( TIME_ERRPRINT,               EV_VMP_BGN + 26 );            //�����ӡ��ʱ��

    /*int nRet = */::OspPost(MAKEIID(AID_VMP, 1), OSP_POWERON, ptVmpCfg, sizeof(T8keVmpCfg));

    return TRUE;
}

API void vmpstatus()
{
        OspPost(MAKEIID(AID_VMP, 1), EV_VMP_DISPLAYALL);
}

API void pvmpmsg(void)
{
    g_nVmplog = 1;
}

API void npvmpmsg(void)
{
    g_nVmplog = 0;
}

API void vmpshowlastevent()
{
    OspPrintf(TRUE, FALSE, "g_wVmpLastEvent=%u(%s), File Line: %u\n", g_wVmpLastEvent, ::OspEventDesc(g_wVmpLastEvent), g_dwVmpLastFileLine);
}
API void vmpver(void)
{
    OspPrintf(TRUE, FALSE, "vmp version:%s\t",VER_VMP);
    OspPrintf(TRUE, FALSE, "compile time:%s:%s\n", __DATE__, __TIME__);
}

API void vmphelp()
{
    OspPrintf(TRUE, FALSE, "vmp version:%s\t",VER_VMP);
    OspPrintf(TRUE, FALSE, "compile time:%s:%s\n", __DATE__, __TIME__);
    OspPrintf(TRUE, FALSE, "vmpstatus  ---- Display vmp status\n");
    OspPrintf(TRUE, FALSE, "pvmpmsg    ---- Display the debug information.\n");
    OspPrintf(TRUE, FALSE, "npvmpmsg   ---- Not display the debug information.\n");
    OspPrintf(TRUE, FALSE, "vmpshowlastevent   ---- Display lastest event procceeding.\n");
	OspPrintf(TRUE, FALSE, "showvmpparam		 ---- Show vmp param\n");
    
}


// ��ӡҵ��ϳɲ���, zgc, 2008-03-19
API void showvmpparam(void)
{
    OspPost(MAKEIID(AID_VMP, 1), EV_VMP_SHOWPARAM);
}

// ���Բ�����ֹͣVMP�, zgc, 2007/04/24
// API void stopvmpeqp(void)
// {
// 	OspPost( MAKEIID(AID_VMP, 1), MCU_VMP_STOPMAP_CMD );
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
API void setBBOut( u8 byOut )
{
    if ( byOut > 1 )
    {
        byOut = 1;
    }
    CServMsg cServMsg;
    cServMsg.SetMsgBody( (u8*)&byOut, sizeof(u8) );
    OspPost( MAKEIID(AID_VMP, 1), EV_VMP_BACKBOARD_OUT_CMD, 
             cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
}

/*=============================================================================
�� �� ���� VmpAPIEnableInLinux
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
/*lint -save -e611*/
void VmpAPIEnableInLinux(void)
{
#ifdef _LINUX_      
    OspRegCommand("vmpstatus",          (void*)vmpstatus,           "vmpstatus");
	OspRegCommand("showvmpparam",		(void*)showvmpparam,		"showvmpparam");
    OspRegCommand("pvmpmsg",            (void*)pvmpmsg,             "pvmpmsg");
    OspRegCommand("npvmpmsg",           (void*)npvmpmsg,            "npvmpmsg");
    OspRegCommand("vmpver",             (void*)vmpver,              "vmpver");
    OspRegCommand("vmphelp",            (void*)vmphelp,             "vmphelp");
    OspRegCommand("vmpshowlastevent",   (void*)vmpshowlastevent,    "vmpshowlastevent");    
#endif
}
/*lint -restore*/
