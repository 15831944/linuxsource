/*****************************************************************************
   ģ����      : WatchDog API ʵ��
   �ļ���      : watchdog.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ����WatchDog���������ע������״̬��

   ����        : ����
   �汾        : V4.0  Copyright(C) 2003-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2005/05/26  4.0         ����      ����
******************************************************************************/

//#include "kdvsys.h"
//#include "mcuconst.h"
#include "watchdogssn.h"
#include "bindwatchdog.h"
//#include "dsccomm.h"
#include "mcuconst.h"

// MPC2 ֧��
#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
        #include "brdwrapperdef.h"
        #include "nipwrapper.h"
        #include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
#endif

CWatchDogSsnApp g_cWatchDogSsn;
BOOL32          g_bWDLog;

// ����
struct TWDParam
{
    TWDParam()
    {
        bStart = FALSE;
    }
    BOOL32 bStart;
};
TWDParam g_tParam;

API void pwdmsg()
{
    g_bWDLog = TRUE;
}

API void npwdmsg()
{
    g_bWDLog = FALSE;
}

API void showwdstatus()
{
    g_cWatchDogSsn.PrintModuleState();
}
/*====================================================================
    ������      ��WDStartWatchDog
    ����        ���������Ź�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2005/05/26  4.0         ����      ����
====================================================================*/
void ReadConfig()
{
#ifdef _LINUX_
    /*
    s8   achProfileName[32];
    BOOL bResult;
    s8   achDefStr[] = "Error String";
    s8   achReturn[MAX_VALUE_LEN + 1];
    s32  nValue;

	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "modcfg.ini");

    bResult = GetRegKeyInt( achProfileName, "Modules", "WatchDogClient", 0, &nValue );
    */

    TBrdPosition tPosition; 
    STATUS status = BrdQueryPosition(&tPosition);
    if (status == OK)
    {
        if ( tPosition.byBrdID == BRD_TYPE_MDSC/*DSL8000_BRD_MDSC*/ ||
             tPosition.byBrdID == BRD_TYPE_HDSC/*DSL8000_BRD_HDSC*/ || 
             tPosition.byBrdID == BRD_TYPE_DSC/*KDV8000B_MODULE*/ )
        {
            g_tParam.bStart = TRUE;        
        }
        else
        {
            g_tParam.bStart = FALSE;    
        }
    }
    else
    {
        g_tParam.bStart = FALSE;        
    }
    OspPrintf(TRUE, FALSE, "Now We are ready to start WatchDog & Brd(%d)? %s\n", 
              tPosition.byBrdID, 
              g_tParam.bStart ? "YES" : "NO" );
#endif
}

/*====================================================================
    ������      ��WDStartWatchDog
    ����        ���������Ź�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2005/05/26  4.0         ����      ����
====================================================================*/
API BOOL32 WDStartWatchDog ( emModuleType enType)
{
#ifdef _LINUX_

    OspRegCommand("pwdmsg",             (void*)pwdmsg,  "Print WatchDog Client Msg");
    OspRegCommand("npwdmsg",            (void*)npwdmsg, "Not Print WatchDog Client Msg");
    OspRegCommand("showwdstatus",       (void*)showwdstatus, "Print WatchDog Module State");

    ReadConfig();
    if (!g_tParam.bStart)
    {
        // �û�����Ϊ������������û���ҵ������ļ��ȣ�Ĭ�ϲ�����
        return FALSE;
    }

    if ( ! IsOspInitd() )
	{
		OspInit(TRUE, 2500);
	}

    //Osp telnet ��ʼ��Ȩ [11/28/2006-zbq]
    OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );

    g_cWatchDogSsn.CreateApp("WatchDogClntApp", WD_CLT_APP_ID, 160, 10, 10<<10);
    
    u8 byNewIdx = g_cWatchDogSsn.AddModule(enType);
    if (byNewIdx == 0)
    {
        return FALSE;
    }

    OspPost( MAKEIID(WD_CLT_APP_ID, 1), OSP_POWERON );

    return TRUE;
#else
    return FALSE;
#endif 
}


#ifdef WIN32
int main()
{
	/*lint -save -e522*/
    WDStartWatchDog( emMP );
    WDStartWatchDog( emPRS );
    WDStartWatchDog( emMTADP );
    Sleep(INFINITE);
	/*lint -restore*/

    return 1;
}
#endif

