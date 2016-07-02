/*****************************************************************************
   ģ����      : HDU
   �ļ���      : hdu.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: HDUҵ���װ
   ����        : ���ֱ�
   �汾        : V4.6  Copyright(C) 2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
******************************************************************************/

#include "osp.h"
#include "mcuconst.h"
#include "mcuver.h"
#include "eqplib.h"
//#include "boardagent.h"
#include "hduagent.h"
#include "hdu.h"
#include "hduapp.h"

#ifdef _LINUX_
#include "nipdebug.h"
#endif

#define APPNAME     (LPCSTR)"/usr/bin/kdvhdu"
static LPCSTR g_szAppName;

/////////////////////////////////////////////////////Linux ���쳣������
#ifdef _LINUX_
// ��ʼ��׽��ں���
// Ret - 0 ��������
s32 StartCaptureException();
#endif //_LINUX_
/////////////////////////////////////////////////////�쳣������

//BOOL hduInit(THduCfg* ptCfg);    //HDU��ʼ��
//API void hdubrdver(void);

API void hdustart(void)
{
	BrdHduAPIEnableInLinux();

	// ���ý����쳣����ص�
#ifdef _LINUX_
    s32 nRet = StartCaptureException();
	if (0 != nRet)
	{
		printf("Hdu: StartCaptureException Error(%d), Ignored.\n", nRet);
	}
    else
    {
        printf("Hdu: StartCaptureException Succeeded\n");
    }    
#endif
		
    //need to activate kdvsyslib.a and DataSwitch.a on VxWorks
#ifdef _VXWORKS_    
    Trim("Kdv Mcu 4.0");
#endif

	s32 nReturn = 0;
	s8 achCommand1[] = "cp /usr/bin/dft_16to9.bmp /usr/etc/config/dft_16to9.bmp";
	nReturn = system( achCommand1 );
	printf( "[hdustart] Copy dft_16to9.bmp Call system() return: %d \n", nReturn );
	
	s8 achCommand2[] = "cp /usr/bin/dft_vga.bmp /usr/etc/config/dft_vga.bmp";
	nReturn = system( achCommand2 );
	printf( "[hdustart] Copy dft_4to3.bmp Call system() return: %d \n", nReturn );

	//zjj20120107�����ɹ���־��ǰ��ע��mcu�ɹ�ǰ
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
    THduCfg tHduEqpCfg;
    tHduEqpCfg.dwConnectIP    = g_cBrdAgentApp.GetMpcIpA(); // double link
    tHduEqpCfg.wConnectPort   = g_cBrdAgentApp.GetMpcPortA();
	tHduEqpCfg.dwConnectIpB    = g_cBrdAgentApp.GetMpcIpB(); // double link
    tHduEqpCfg.wConnectPortB   = g_cBrdAgentApp.GetMpcPortB();
    

	//����PID��mpc��֪������ģʽ�ж��Ƿ�ƥ��
	
	u8 byStartMode = g_cBrdAgentApp.GetStartMode();
	tHduEqpCfg.SetHduSubType(byStartMode); //HDU������������startmodeֵһһ��Ӧ
	tHduEqpCfg.byEqpType = EQP_TYPE_HDU;

// 	if (STARTMODE_HDU_H == byStartMode)
// 	{
// 		tHduEqpCfg.byEqpType = EQP_TYPE_HDU_H;
// 		printf("[hdustart]: start hdu with  mode %d!\n", STARTMODE_HDU_H);
// 	}	
// 	else if(STARTMODE_HDU_M == byStartMode)
// 	{
// 		tHduEqpCfg.byEqpType = EQP_TYPE_HDU;
// 		printf("[hdustart]: start hdu with  mode %d!\n", STARTMODE_HDU_M);
// 	}
// 	else if (STARTMODE_HDU_L == byStartMode)
// 	{	
// 		tHduEqpCfg.byEqpType = EQP_TYPE_HDU_L;
// 		printf("[hdustart]: start hdu with  mode %d!\n", STARTMODE_HDU_L);
// 	}
// 	else
// 	{
// 		tHduEqpCfg.byEqpType = EQP_TYPE_HDU;
// 		printf("[hdustart]warning: start hdu with default mode %d!\n", STARTMODE_HDU_M);
// 	}

    tHduEqpCfg.byEqpId        = g_cBrdAgentApp.GetEqpId();
    tHduEqpCfg.wMcuId         = LOCAL_MCUID;
    tHduEqpCfg.dwLocalIP      = g_cBrdAgentApp.GetBrdIpAddr();
	tHduEqpCfg.SetAutoTest( g_cBrdAgentApp.GetIsTest() );

	printf("[hdustart]tHduEqpCfg.byEqpId = %d",tHduEqpCfg.byEqpId);
    printf("Starting HDU ......\n");
    if (!hduInit(&tHduEqpCfg))
    {
        printf("Init HDU failed!\r");
    }
    else
    {
        printf("Starting HDU success !!!\n");
    }
}

API void hdubrdver(void)
{
    OspPrintf(TRUE, TRUE, "hdu version:%s\t", VER_HDU);
    OspPrintf(TRUE, TRUE, "compile time:%s:%s\n", __DATE__, __TIME__);

/*    hduver();*/
    ospver();
}

/*lint -save -e843*/
static BOOL32 g_bQuit = FALSE;

#ifdef _LINUX_
void OspDemoQuitFunc( int nSignalNum )
{
	OspPrintf(1, 0, "[OspDemoQuitFunc] nSignalNum.%d!\n", nSignalNum);
    g_bQuit = TRUE;
}
#endif

int main()
{
	g_szAppName = APPNAME;
    printf("[AppEntry]Starting: %s\n", g_szAppName);

	hdustart();

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

/*lint -save -e765*/
API void phdumsg(void)
{
	g_cBrdAgentApp.m_bPrintBrdLog = TRUE;
}

API void nphdumsg( void )
{
	g_cBrdAgentApp.m_bPrintBrdLog = FALSE;
}

//ע��Linux���Խӿ�
void BrdHduAPIEnableInLinux(void)
{
	//lint -save -e611
#ifdef _LINUX_  
    OspRegCommand("hdustart",       (void*)hdustart,        "hdu start");
    OspRegCommand("hdubrdver",      (void*)hdubrdver,       "hdubrdver");
	OspRegCommand("phdumsg",		(void*)phdumsg,         "phdumsg");
	OspRegCommand("nphdumsg",		(void*)nphdumsg,        "nphdumsg");
#endif
	//lint -restore
}


