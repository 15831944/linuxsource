/*****************************************************************************
   ģ����      : BAP HD
   �ļ���      : baphd.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: BAP-HD(MT�汾)ҵ���װ
   ����        : ����
   �汾        : V4.5  Copyright(C) 2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
******************************************************************************/

#include "osp.h"
#include "mcuconst.h"
#include "mcuver.h"
#include "eqplib.h"
#include "boardagent.h"
#include "bapagent.h"

#ifdef _LINUX_
#include "nipdebug.h"
#endif

#define APPNAME     (LPCSTR)"/usr/bin/kdvbap"
LPCSTR g_szAppName;

/////////////////////////////////////////////////////Linux ���쳣������
#ifdef _LINUX_
// ��ʼ��׽��ں���
// Ret - 0 ��������
s32 StartCaptureException();
#endif //_LINUX_
/////////////////////////////////////////////////////�쳣������

API void bapstart()
{

	// ���ý����쳣����ص�
#ifdef _LINUX_
    s32 nRet = StartCaptureException();
	if (0 != nRet)
	{
		printf("Bap: StartCaptureException Error(%d), Ignored.\n", nRet);
	}
    else
    {
        printf("Bap: StartCaptureException Succeeded\n");
    }    
#endif
		
    //need to activate kdvsyslib.a and DataSwitch.a on VxWorks
#ifdef _VXWORKS_    
    Trim("Kdv Mcu 4.0");
#endif
			     
    if (!InitBrdAgent())
    {
        printf("Init Agent Failed!!!!!!\n");
        return;
    }
    else
    {
        printf("Init Agent Succeeded!\n");
    }
        
    // start up bitstream adapter server
    TEqpCfg tBasEqpCfg;
    tBasEqpCfg.dwConnectIP    = g_cBrdAgentApp.GetMpcIpA(); // single link
    tBasEqpCfg.wConnectPort   = g_cBrdAgentApp.GetMpcPortA();
    tBasEqpCfg.byEqpType      = EQP_TYPE_BAS;
    tBasEqpCfg.byEqpId        = g_cBrdAgentApp.GetEqpId();
    tBasEqpCfg.wMcuId         = LOCAL_MCUID;
    tBasEqpCfg.dwLocalIP      = g_cBrdAgentApp.GetBrdIpAddr();

    printf("Starting BAS HD ......\n");
    if (!basInit(&tBasEqpCfg))
    {
        printf("Init BAS failed!\r");
    }
    else
    {
        printf("Starting BAS success !!!\n");
    }
}

// API void bappver()
// {
//     OspPrintf(TRUE, TRUE, "vmphd version:%s\t", VER_MMP);
//     OspPrintf(TRUE, TRUE, "compile time:%s:%s\n", __DATE__, __TIME__);
// 
//     basver();
//     ospver();
//}


BOOL32 g_bQuit = FALSE;

#ifdef _LINUX_
void OspDemoQuitFunc( int nSignalNum )
{
    g_bQuit = TRUE;
}
#endif

int main( int argc, char** argv )
{
    g_szAppName = APPNAME;
    printf("[AppEntry]Starting: %s\n", g_szAppName);

	bapstart();

#ifdef _LINUX_
    //ע���˳��������
	OspRegQuitFunc( OspDemoQuitFunc );
#endif

		
	while(!g_bQuit)
	{
	    OspDelay(1000);
	}   
    

    printf("Quit normally!\n");
	return 0;
}	


#ifdef _LINUX_
/////////////////////////////////////////////////////Linux ���쳣������


/*=============================================================================
    �� �� ���� StartCaptureException
    ��    �ܣ� ���ý����쳣����ص�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� s32 0-�ɹ�����0-������
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/7/11   1.0			����                  ����
=============================================================================*/
s32 StartCaptureException()
{
    return init_nipdebug(0, NULL, DIR_EXCLOG);
}

#endif // _LINUX

/////////////////////////////////////////////////////�쳣������
