#include "osp.h"
#include "mcustruct.h"
#include "mcuver.h"
#include "bindmtadp.h"
#include "bindmp.h"
#include "dataswitch.h"
//#include "kdvsys.h"
#include "eqplib.h"
#include "boardagent.h"
//#include <stdio.h>

// [pengjie 2010/2/27] CRI2 pxy ֧��
#include "firewallproxyserverlib.h" 
// [pengjie 2010/6/12] CRI2 pxy������
#include "protocolnipinterface.h"

//#include "kdvlog.h"
//#include "loguserdef.h"

#if defined(WIN32) || defined(_WIN32)
#define CRI_KDVLOG_CONF_FILE	(s8*)"./conf/kdvlog_cri.ini"
#else
#define CRI_KDVLOG_CONF_FILE	(s8*)"/usr/etc/config/conf/kdvlog_cri.ini"
#endif


/////////////////////////////////////////////////////Linux ���쳣������
#ifdef _LINUX_

//#include <stdlib.h>
#include <elf.h>
//#include <sys/types.h>
//#include <unistd.h>
//#include <signal.h>
#include <ucontext.h>
//#include <errno.h>
//#include <string.h>
#include "nipdebug.h"

Elf32_Sym *get_symtab(Elf32_Sym *symtab, int tabsize, int addr);
Elf32_Shdr * read_elf_section(FILE *file, Elf32_Ehdr *ehdr);
void *read_symtable(FILE *file, Elf32_Shdr *shdr, Elf32_Ehdr *ehdr,   int *size);
void *read_strtable(FILE *file, Elf32_Shdr *shdr, Elf32_Ehdr *ehdr,  int *size);
int  read_elf_head(FILE *file, Elf32_Ehdr *ehdr);
Elf32_Sym *get_func_symb_by_addr(Elf32_Sym *symtab, int tabsize, int addr);
int printf_symbl_name(int pc);

Elf32_Sym *sym;
char *strtab;
int symsize, strsize;

#define SECTION_HEADER_INDEX(I)				\
  ((I) < SHN_LORESERVE					\
   ? (I)						\
   : ((I) <= SHN_HIRESERVE				\
      ? 0						\
      : (I) - (SHN_HIRESERVE + 1 - SHN_LORESERVE)))

#define APPNAME     (LPCSTR)"/usr/bin/kdvcri"
LPCSTR g_szAppName;

// ��ʼ��׽��ں���
// Ret - 0 ��������
s32 StartCaptureException();

#endif //_LINUX_

/////////////////////////////////////////////////////�쳣������

#include "cri_agent.h"

BOOL ReadBoardConfig(u8 &byBoardId, BOOL &bAttached, u32 &dwMcuIp, u16 &wMcuPort, u32 &dwRegisterIp)
{
	printf( "[ReadBoardConfig] Now come to ReadBoardConfig! \n" );
	if(!InitCriAgent())
	{
		OspPrintf(1, 0, "Starting up board agent failed. Exit!\n");	
		printf("Starting up board agent failed. Exit!\n");
		//cdw
		return FALSE;
	}

	if( g_cBrdAgentApp.GetBoardId() == MCU_BOARD_MPC ||
        g_cBrdAgentApp.GetBoardId() == MCU_BOARD_MPCD )//running on mpc
	{
		bAttached = TRUE;
	}
	else //running on dri
	{
		bAttached = FALSE;
		//byBoardId = g_cBrdAgentApp.GetBoardId() % 16;
		byBoardId = g_cBrdAgentApp.GetBoardId();
		dwMcuIp = g_cBrdAgentApp.GetMpcIpA();
		wMcuPort = g_cBrdAgentApp.GetMpcPortA();
		dwRegisterIp = g_cBrdAgentApp.GetBrdIpAddr();
	}
	return TRUE;
}

BOOL GetMpcBInfo(u32& dwMpcIp, u16& wMpcPort)  // get mpcB's info
{
    dwMpcIp = g_cBrdAgentApp.GetMpcIpB();
    wMpcPort = g_cBrdAgentApp.GetMpcPortB();
    return TRUE;
}

BOOL ReadModuleConfig(BOOL &bStartMtadp, BOOL &bStartMp, BOOL &bStartPrs, BOOL &bStartPxy)
{	
	printf("[ReadModuleConfig] come in! \n");
    int nTemp;	   

    s8 pFileName[KDV_MAX_PATH] = {0};
    sprintf(pFileName, "%s/modcfg.ini", DIR_CONFIG);
	
	//�Ƿ�����prs
	bStartPrs = g_cBrdAgentApp.IsRunPrs();

	FILE *tmpFp = fopen(pFileName, "r");
	if(tmpFp == NULL)
	{
		OspPrintf(TRUE, TRUE, "%s not found.\n", pFileName);
		return FALSE;
	}
	else
    {
		fclose(tmpFp);
    }
		
	//�Ƿ�����MtAdp
	GetRegKeyInt( pFileName, "Modules", "mtadp" , 0 , &nTemp);
	bStartMtadp = ((u8)nTemp == 1);
	
	//�Ƿ�����mp
	GetRegKeyInt( pFileName, "Modules", "mp" , 1 , &nTemp);
	bStartMp = ((u8)nTemp == 1);

#if defined(_LINUX12_) && defined(_IS22_)
	//�Ƿ�����Pxy
	GetRegKeyInt( pFileName, "Modules", "pxy" , 1 , &nTemp);
	bStartPxy = ((u8)nTemp == 1);	

	/*TBrdEthParam tEthPara;
	memset(&tEthPara,0,sizeof(tEthPara));
	BrdGetEthParam(g_cBrdAgentApp.GetBrdEthChoice(),&tEthPara);
	s8 achIP[20];
	sprintf(achIP, "%d.%d.%d.%d", tEthPara.dwIpAdrs >> 24 & 0xff,
		tEthPara.dwIpAdrs >> 16 & 0xff,
		tEthPara.dwIpAdrs >> 8 & 0xff,
		tEthPara.dwIpAdrs & 0xff);

	printf("[ReadModuleConfig] IsStartPxy = %d ip.%s ethchoise.%d\n", bStartPxy,achIP,g_cBrdAgentApp.GetBrdEthChoice() );
	
	sprintf(pFileName, "/usr/etc/pxy/conf/pxysrvCfg.ini" );
	BOOL32 bResult = SetRegKeyString(pFileName, "LocalInfo", "IpAddr", achIP);
    if ( !bResult)
    {        
        printf("[WriteStringToFile] Wrong profile while setting %s!\n", "LocalInfo");
    }
	

	bResult = SetRegKeyString(pFileName, "GKInfo", "IpAddr", achIP);
    if ( !bResult)
    {        
        printf("[WriteStringToFile] Wrong profile while setting %s!\n", "GKInfo");
    }*/
	


#endif

	return TRUE;

}


BOOL BrdIsHdi()
{
#ifdef _MTACCESS_HDI_
    return TRUE;
#else
    return FALSE;
#endif
}

// [pengjie 2011/2/16] is2.2 ֧��
void CopyCfgFile()
{
#ifdef _LINUX_
	// 1�����û��Ŀ¼����Ŀ¼
	mkdir(DIR_CONFIG,0777);
	chmod(DIR_CONFIG,0777);

	// 2.������������Ŀ¼
	/*mkdir("/usr/etc/pxy",0777);
	chmod("/usr/etc/pxy",0777);
	mkdir("/usr/etc/pxy/conf",0777);
	chmod("/usr/etc/pxy/conf",0777);*/
	

	// 2������ѹ�����������ļ�������ָ��Ŀ¼��
	s32 nReturn = 0;
	s8 achCommand[MAX_PATH];
	memset( achCommand, 0, sizeof(achCommand) ); 
	
	s8 achFileName[MAX_PATH];
	memset( achFileName, 0, sizeof(achFileName) ); 
	sprintf( achFileName, "%s/%s", DIR_CONFIG, FILE_MODCFG_INI );

	FILE * hFile = fopen(achFileName,"r");
	if(hFile != NULL )
	{
		fclose(hFile);
		hFile = NULL;
	}
	else
	{
		sprintf( achCommand, "%s %s/%s %s/%s", "cp", DIR_BIN, FILE_MODCFG_INI, DIR_CONFIG, FILE_MODCFG_INI );
		nReturn = system( achCommand );
		printf( "CopyCfgFile modcfg.ini return: %d \n", nReturn );
	}

	memset( achFileName, 0, sizeof(achFileName) ); 
	sprintf( achFileName, "%s/%s", DIR_CONFIG, FILE_MTADPDEBUG_INI );
	hFile = fopen(achFileName,"r");
	if(hFile != NULL )
	{
		fclose(hFile);
		hFile = NULL;
	}
	else
	{
		memset( achCommand, 0, sizeof(achCommand) );
		sprintf( achCommand, "%s %s/%s %s/%s", "cp", DIR_BIN, FILE_MTADPDEBUG_INI, DIR_CONFIG, FILE_MTADPDEBUG_INI );
		nReturn = system( achCommand );
		printf( "CopyCfgFile mtadpdebug.ini return: %d \n", nReturn );
	}

	/*memset( achCommand, 0, sizeof(achCommand) );
	sprintf( achCommand, "%s %s/%s %s/%s", "cp", DIR_BIN, PXYCFG_INI_FILENAME, "/usr/etc/pxy/conf", PXYCFG_INI_FILENAME );
	nReturn = system( achCommand );
	printf( "CopyCfgFile %s return: %d \n", PXYCFG_INI_FILENAME,nReturn );*/


	// 3�����֮ǰ��ѹ����ʱ�ļ�
	memset( achCommand, 0, sizeof(achCommand) );
	sprintf( achCommand, "%s %s/%s", "rm -f", DIR_BIN, FILE_MODCFG_INI );
	nReturn = system( achCommand );
	printf( "RemoveCfgFile modcfg.ini return: %d \n", nReturn );

	memset( achCommand, 0, sizeof(achCommand) );
	sprintf( achCommand, "%s %s/%s", "rm -f", DIR_BIN, FILE_MTADPDEBUG_INI );
	nReturn = system( achCommand );
	printf( "RemoveCfgFile mtadpdebug.ini return: %d \n", nReturn );


	/*memset( achCommand, 0, sizeof(achCommand) );
	sprintf( achCommand, "%s %s/%s", "rm -f", DIR_BIN, PXYCFG_INI_FILENAME );
	nReturn = system( achCommand );
	printf( "RemoveCfgFile mtadpdebug.ini return: %d \n", nReturn );*/

#endif
	return;
}

API void userinit()
{	
	printf( "[userinit] Now try to userinit! \n" );

    Err_t err = LogInit(CRI_KDVLOG_CONF_FILE);
    if(LOG_ERR_NO_ERR != err)
    {
        printf("KdvLog Init failed, err is %d\n", err);
	}

	BOOL bAttached = TRUE;
	u8   byBoardId = 1;
	u32  dwMcuIp = 0;
	u16  wMcuPort = MCU_LISTEN_PORT;
	u32  dwRegisterIp = 0;
	
	u32  dwMcuIpB = 0;
	u16  wMcuPortB = 0;
	bool bDoubleLink = FALSE; //�Ƿ�֧��˫�㽨��

	BOOL bStartMtadp = FALSE;
	BOOL bStartMp = TRUE;
	BOOL bStartPrs = FALSE;
	BOOL bStartPxy = FALSE;

	// ���ý����쳣����ص�
#ifdef _LINUX_
    s32 nRet = StartCaptureException();
	if (0 != nRet)
	{
		printf("CRI: StartCaptureException Error(%d), Ignored.\n", nRet);
	}
    else
    {
        printf("CRI: StartCaptureException Succeeded\n");
    } 
	CopyCfgFile();
#endif

	if(!OspInit(TRUE, BRD_TELNET_PORT))
	{
		OspPrintf(1, 0, "Starting up OSP failed. Exit!\n");
		OspQuit();
		return ;
	}
    //Osp telnet ��ʼ��Ȩ [11/28/2006-zbq]
#ifndef WIN32
    OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );
#endif

#ifdef OSPEVENT
#undef OSPEVENT
#endif

#define OSPEVENT( x, y ) OspAddEventDesc( #x, y )

#ifdef _EV_MCUMT_H_
	#undef _EV_MCUMT_H_
	#include "evmcumt.h"
#else
	#include "evmcumt.h"
#endif
/*lint -save -esym(529, EV_MCUMT_SAT_END)*/	
#ifdef _EV_MCU_H_
	#undef _EV_MCU_H_
	#include "evmcu.h" 
#else
	#include "evmcu.h"
#endif
/*lint -restore*/
#ifdef _EV_MCUGK_H_
#undef _EV_MCUGK_H_
#include "evmcugk.h"
#else
#include "evmcugk.h"
#endif
	
    //need to activate kdvsyslib.a and DataSwitch.a on VxWorks
#ifdef _VXWORKS_
    Trim("Kdv Mcu 4.0");
#endif
   	
	//��ȡ��������
	if(!ReadBoardConfig(byBoardId, bAttached, dwMcuIp, wMcuPort, dwRegisterIp))
	{	
		OspPrintf(1, 1, "Configuration error. Exit!\n");
		return;
	}
    
	//�Ƿ��������mcu��ַ��Ҫ˫�㽨��֧��
	GetMpcBInfo(dwMcuIpB, wMcuPortB);
	if (0 != dwMcuIpB && dwMcuIp != dwMcuIpB)
	{
		bDoubleLink = TRUE;
	}
    
#ifndef _IS21APP_
	//��ȡģ������
	ReadModuleConfig(bStartMtadp, bStartMp, bStartPrs, bStartPxy);
	
	u32 dwDstMcuNode  = BrdGetDstMcuNode();
	u32 dwDstMcuNodeB = BrdGetDstMcuNodeB();
	
	if(bStartMtadp) //�������Mtadp
	{
#ifdef _IS22_
		printf( "start mtadp\n" );
		system("chmod 777 /usr/bin/mtaccessmtadp");
		system("/usr/bin/mtaccessmtadp &");
#else
		TMtAdpConnectParam tConnectParam;
		TMtAdpConnectParam tConnectParamB;
		TMtAdp tMtadp;
	
		tMtadp.SetMtadpId( byBoardId );
		tMtadp.SetAttachMode( bAttached ? 1 : 255 );
		tMtadp.SetIpAddr(dwRegisterIp);
		tConnectParam.m_dwMcuTcpNode = dwDstMcuNode;
		tConnectParam.m_wMcuPort     = wMcuPort;
		tConnectParam.SetMcuIpAddr( ntohl(dwMcuIp) );	
        tConnectParam.m_bSupportHD = BrdIsHdi();
		if (bDoubleLink)
		{
			tConnectParamB.m_dwMcuTcpNode = dwDstMcuNodeB;
			tConnectParamB.m_wMcuPort     = wMcuPortB;
			tConnectParamB.SetMcuIpAddr( ntohl(dwMcuIpB) );
            tConnectParamB.m_bSupportHD = BrdIsHdi();
		}
		
		//����빦�ܵĵ�����Ҫ�ϱ�MAC��ַ[5/16/2012 chendaiwei]
		TBrdEthParam tEthPara;
		memset(&tEthPara,0,sizeof(tEthPara));
		BrdGetEthParam(g_cBrdAgentApp.GetBrdEthChoice(),&tEthPara);

		if ( !MtAdpStart(&tConnectParam, &tMtadp, (bDoubleLink ? &tConnectParamB : NULL),(u8*)tEthPara.byMacAdrs) )
		{

#if defined _LINUX_ && defined _LINUX12_
			//[chendaiwei 2010/11/18] ����ҵ��������Cri2��ص�
			BrdLedStatusSet(LED_FUN_ID1,BRD_LED_OFF);
			BrdLedStatusSet(LED_FUN_ID7,BRD_LED_OFF);
#endif
			OspPrintf(TRUE, TRUE, "Starting Mtadp failed.\n");
			OspQuit();
			return;
		}		
		else
		{
#if defined _LINUX_ && defined _LINUX12_
			//[chendaiwei 2010/11/18] ����ҵ��������Cri2����
			//HDI����
			if(BrdIsHdi())
			{
				BrdLedStatusSet(LED_FUN_ID7,BRD_LED_ON);				
			}
			else
			{
				BrdLedStatusSet(LED_FUN_ID1,BRD_LED_ON);
			}

			OspPrintf(TRUE, TRUE, "Starting Mtadp OK!\n");
#endif
		}
#endif//end_IS22_
	}
	
	if(bStartMp) //�������Mp
	{		
		TMp tMp;
		tMp.SetIpAddr(dwRegisterIp);
		tMp.SetMpId(byBoardId);
		tMp.SetAttachMode(bAttached ? 1 : 255);
		TStartMpParam tMpParamSlave;
		tMpParamSlave.dwMcuIp = dwMcuIpB;
		tMpParamSlave.wMcuPort = wMcuPortB;

		if(!mpstart(dwMcuIp, wMcuPort, &tMp, &tMpParamSlave))
		{
#if defined _LINUX_ && defined _LINUX12_
			//[chendaiwei 2010/11/18] ת��ҵ������ʧ�ܣ�Cri2��ص�
			BrdLedStatusSet(LED_FUN_ID2,BRD_LED_OFF);
#endif
			OspPrintf(1, 1, "Starting mp failed.\n");
			OspQuit();
			return;
		}
		else
		{
#if defined _LINUX_ && defined _LINUX12_
			//[chendaiwei 2010/11/18] ת��ҵ�������ɹ���Cri2����
			BrdLedStatusSet(LED_FUN_ID2,BRD_LED_ON);
#endif
			OspPrintf(1, 1, "Starting mp OK!\n");
		}	
	}
	
	if(bStartPrs) //�������Prs
	{	
		TPrsCfg tPrsCfg;
		memset(&tPrsCfg, 0, sizeof(tPrsCfg));
		g_cBrdAgentApp.GetPrsCfg(&tPrsCfg);
		logenablemod(MID_PRSEQP_COMMON);
		if( !prsinit( &tPrsCfg ) )
		{
#if defined _LINUX_ && defined _LINUX12_
			//[chendaiwei 2010/11/18] �����ش�ҵ������ʧ�ܣ�Cri2��ص�
			BrdLedStatusSet(LED_FUN_ID3,BRD_LED_OFF);
#endif
			OspPrintf(1, 1, "Starting prs failed\n" );
		}
		else
		{
#if defined _LINUX_ && defined _LINUX12_
			//[chendaiwei 2010/11/18] �����ش�ҵ�������ɹ���Cri2����
			BrdLedStatusSet(LED_FUN_ID3,BRD_LED_ON);
#endif
			OspPrintf(1, 1, "Starting prs OK!\n" );
		}
	}		

	
#if defined(_LINUX12_) && defined(_IS22_)
	printf("[userinit] IsStartPxy = %d \n", bStartPxy);
	if( bStartPxy )
	{		
		InitPxyNipFun();
	    
		u16 wRet = InitPxy(); // ��ʼ��
		printf("[userinit] InitPxy() = %d \n", wRet);
		OspSetPrompt("");
	}
#endif

#if defined _LINUX_ && defined _LINUX12_
	//[chendaiwei 2010/11/29] CRI2ҵ��ɹ�����������M/S��
    BrdLedStatusSet(LED_CRI2_MS,BRD_LED_ON);
#endif

#endif // _IS21APP_
}

#ifndef _IS21APP_
API void userquit()
{
	MtAdpStop();
	mpstop();

	
#if defined _LINUX_ && defined _LINUX12_
#ifdef _IS22_
	UnInitPxyServer();
#endif
	//[chendaiwei 2010/11/18] ת��ҵ������ʧ�ܣ�Cri2��ص�
	BrdLedStatusSet(LED_FUN_ID2,BRD_LED_OFF);
	//[chendaiwei 2010/11/18] �����ش�ҵ������ʧ�ܣ�Cri2��ص�
	BrdLedStatusSet(LED_FUN_ID3,BRD_LED_OFF);
	//[chendaiwei 2010/11/18] ����ҵ��������Cri2��ص�
	BrdLedStatusSet(LED_FUN_ID1,BRD_LED_OFF);
	BrdLedStatusSet(LED_FUN_ID7,BRD_LED_OFF);
	//[chendaiwei 2010/11/29] ͣCRI2ҵ��Ϩ��M/S��
	BrdLedStatusSet(LED_CRI2_MS,BRD_LED_OFF);
#endif
	OspQuit();
}
#endif // _IS21APP_

#ifndef _IS21APP_
API void version()
{
	mtadpver();
	mpver();	
	dsver();
}
#endif // _IS21APP_

#ifdef _LINUX_
BOOL32 g_bQuit = FALSE;
void OspDemoQuitFunc( int nSignalNum )
{
	g_bQuit = TRUE;
}

int main( int argc, char** argv )
{
// [pengjie 2010/12/9] ����IS2.1Ŀǰ����ҵ�񣬵�����ʱ��Ҫҵ��������������ɹ���־λ
#ifdef _IS21APP_
 	printf("=============IS2.1 APP Start !==================\n");
// 	if( ERROR == BrdInit() )
// 	{
// 		printf("Call BrdInit() fail !\n");
// 		return 0;
// 	}
// 	
// 	u8 byRes = BrdSetSysRunSuccess();
// 	if( UPDATE_VERSION_SUCCESS == byRes )
// 	{
// 		printf("[BrdSetSysRunSuccess] succeed to update !\n");
// 	}
// 	else
// 	{
// 		printf( "[BrdSetSysRunSuccess] update filed Ret.%d !\n", byRes );
// 	}
// 	printf("=============IS2.1 APP quit normally !==========\n");
// 	return 0;
#endif
// End

    if (argc > 0)
    {
        g_szAppName = argv[0];
    }
    else
    {
        g_szAppName = APPNAME;
    }
    printf("[AppEntry]Starting: %s\n", g_szAppName);
    printf( "[AppEntry] userinit will begain! \n" );

	//zjj20120107 �����ɹ����÷ŵ�ע��mcu�ɹ�֮ǰ������
#ifdef _LINUX12_
	s8 byRet = BrdSetSysRunSuccess();
	switch (byRet)
	{
	case NO_UPDATE_OPERATION:
		printf("[BrdSetSysRunSuccess]start cri normally\n");
		break;
	case UPDATE_VERSION_ROLLBACK:
		printf("[BrdSetSysRunSuccess]fail to update, rollback version\n");
		break;
	case UPDATE_VERSION_SUCCESS:
		printf("[BrdSetSysRunSuccess]succeed to update\n");
		break;		
	case SET_UPDATE_FLAG_FAILED:
		printf("[BrdSetSysRunSuccess]fail to set run flag\n");
		break;
	default:
		printf("[BrdSetSysRunSuccess]bad callback param\n");
		break;
	}
#endif //_LINUX12_

	userinit();

	printf( "[AppEntry] userinit over! \n" );

    //ע���˳��������
	OspRegQuitFunc( OspDemoQuitFunc );
    g_bQuit = FALSE;

/*#ifdef _LINUX12_
	s8 byRet = BrdSetSysRunSuccess();
	switch (byRet)
	{
	case NO_UPDATE_OPERATION:
		printf("[BrdSetSysRunSuccess]start cri normally\n");
		break;
	case UPDATE_VERSION_ROLLBACK:
		printf("[BrdSetSysRunSuccess]fail to update, rollback version\n");
		break;
	case UPDATE_VERSION_SUCCESS:
		printf("[BrdSetSysRunSuccess]succeed to update\n");
		break;		
	case SET_UPDATE_FLAG_FAILED:
		printf("[BrdSetSysRunSuccess]fail to set run flag\n");
		break;
	default:
		printf("[BrdSetSysRunSuccess]bad callback param\n");
		break;
	}
#endif //_LINUX12_
*/

    while (!g_bQuit)
    {
        OspDelay(1000);
    }	

    printf("cri quit normally!\n");
    return 0;
}
#endif


#if defined( WIN32 )
void main(void)
{
	userinit();
	while (true)
    {
        Sleep(INFINITE);
    }
}
#endif


/////////////////////////////////////////////////////Linux ���쳣������

#ifdef _LINUX_


#ifdef _X86_

/* parse cpu context information is architecture depent, 
   encapsulate as helper function can be easy */
int parse_x86_context(void* tmp)
{
	struct ucontext* ct = (struct ucontext*)tmp;	
	int* regs = (int*)ct->uc_mcontext.gregs;
	int  epc = regs[14];
	int *presp = (int *)regs[6];

	printf("\nexception instruction address: 0x%x\n", epc);
	
	printf("\n===================CPU registers==============================\n");

	printf("cpu register value:\n");
	printf("GS:0x%x\t FS:0x%x\t ES:0x%x\t DS:0x%x\n", 
	       regs[0], regs[1], regs[2], regs[3]);

	printf("CS:EIP:   0x%x:0x%x\nSS:UESP:  0x%x:0x%x\n", 
	       regs[15],regs[14], regs[18], regs[17]);

	printf("EDI:0x%x\t ESI:0x%x\t EBP:0x%x\n",regs[4], regs[5], regs[6]);
	printf("ESP:0x%x\t EBX:0x%x\t EDX:0x%x\n",regs[7], regs[8], regs[9]);
	printf("ECX:0x%x\t\t EAX:0x%x\n",regs[10], regs[11]);
	printf("TRAPNO:0x%x\t ERR:0x%x\t EFL:0x%x\n",regs[12], regs[13], regs[16]);

	printf("\n=================== call trace ==============================\n");


	/* ebp will record frame-pointer */
#if 0	
	*(stack-1) = epc;/* put the exception addr in the stack */
	for(i = -1;; i++) 
		if (printf_symbl_name(*(stack+i)) == -1)
			break;
#else
	printf_symbl_name(epc);/*fault addr*/
	while(1)
	{
		int ret = printf_symbl_name(*(presp + 1));
		if (ret == -1 || !ret)
		{
			if (!ret)
				printf("programs's user stack error !!\n");
			break;
		}
		presp = (int *)(*(presp));/* pre function's base sp */		
	}
	
#endif			
	free(sym);
	free(strtab);
	return 0;
}

#else   // Power PC 

void writeexclog(const s8 *pachBuf, s32 nBufLen)
{
	if (NULL == pachBuf || nBufLen <= 0)
    {
        printf("invalid param in writeexclog. pachBuf.%d nBufLen.%d\n",
                (int)pachBuf, nBufLen);
        return;
    }		
	
	FILE *hLogFile = fopen(DIR_EXCLOG, "r+b");
    if (NULL == hLogFile)
    {
        printf("exc.log not exist and create it\n");
        hLogFile = fopen(DIR_EXCLOG, "w+b");
        if (NULL == hLogFile)
        {
            printf("create exc.log failed for %s\n", strerror(errno));     
            return;
        }
    }

    fseek(hLogFile, 0, SEEK_END);
	s32 nLen = fwrite(pachBuf, nBufLen, 1, hLogFile);
	if (0 == nLen)
    {
        printf("write to exc.log failed\n");
        return;
    }
	
	fclose(hLogFile);
	return;
}

/* parse cpu context information is architecture depent, 
   encapsulate as helper function can be easy */
int parse_ppc_context(void* tmp)
{
	struct ucontext* ct = (struct ucontext*)tmp;
	int* regs = (int*)ct->uc_mcontext.regs;
	int  epc = regs[32];
	int* stack = (int*)regs[1];
	int  nLink = regs[36];/* link register */
	int* presp = (int *)(*stack);

	printf("\nexception instruction address: 0x%x\n", epc);

	printf("\n=================== call trace ==============================\n");

    // xsl [8/16/2006] write to log
    char achInfo[255];
    time_t tiCurTime = ::time(NULL);             
    int nLen = sprintf(achInfo, "\nsytem time %s\n", ctime(&tiCurTime));
    writeexclog(achInfo, nLen);
    nLen = sprintf(achInfo, "exception instruction address: 0x%x\n", epc);
    writeexclog(achInfo, nLen);
    nLen = sprintf(achInfo, "\n=========== call trace ============\n");
    writeexclog(achInfo, nLen);
	
	/* ebp will record frame-pointer */

	int func_size ;
	func_size = printf_symbl_name(epc);/*fault addr*/
	if (func_size > 0)/* link register and faule addr may be in the same function so do follows */
	{
		if ((epc - nLink < 0) || (epc - nLink >= func_size))
			printf_symbl_name(nLink);
	}
	
	while(1)
	{
		int ret = printf_symbl_name(*(presp + 1));
		if (ret == -1 || !ret)
		{
			if (!ret)
				printf("programs's user stack error !!\n");
			break;
		}
		presp = (int *)(*(presp));/* pre function's base sp */		
	}
	
	free(sym);
	free(strtab);
	return 0;
}
#endif


void parse_context(void* uc)
{
#ifdef _X86_
	parse_x86_context(uc);
#else   // Power PC
	parse_ppc_context(uc);
#endif
}


int printf_symbl_name(int pc)
{
	Elf32_Sym *syml = get_func_symb_by_addr(sym,symsize, pc);
	if (syml) {
		if ((s32)syml->st_name >= strsize)
		{
			printf("st_name if big then strtable size\r\n");
			return -1;
		}
		//return (strtab+syml->st_name);
		printf("pc:%x %s\r\n", pc, (strtab+syml->st_name));

        // xsl [8/16/2006] write to log
        char achInfo[255];
        int nLen = sprintf(achInfo, "pc:%x %s\r\n", pc, (strtab+syml->st_name));
        writeexclog(achInfo, nLen);
		
		if (!strcmp((strtab+syml->st_name), "main"))		
			return -1;
			
		return syml->st_value;	
	}	
	return 0;
}
void my_sigaction(int signo, siginfo_t* info, void* ct)
{
	if(signo == SIGSEGV) {
		printf("\r\ncatch segment fault! pid = %d\n", getpid());
	}
	if(info->si_code == SI_KERNEL)
		printf("signal is send by Kernel\n");
		
	printf("caused by:\t");

	if(info->si_code == SEGV_MAPERR)
		printf("access a fault address: %p\n", info->si_addr);

	else if(info->si_code == SEGV_ACCERR)
		printf("access a no permission address: %p\n", info->si_addr);
	else
		printf("unknow reason\n");
	if (ct)
		parse_context(ct);
	else
		printf("process context is NULL\n");	

	/* sigment fault is critical fault, we should exit right now */
	exit(1);
}

void install_sigaction()
{
	struct sigaction sigact;

	sigemptyset (&sigact.sa_mask);
	//memset(&sigact, 0, sizeof (struct sigaction));
	sigact.sa_flags = (s32)(SA_ONESHOT | SA_SIGINFO);
	sigact.sa_sigaction = my_sigaction;
	sigaction(SIGSEGV , &sigact, NULL);	
}



/* get the function'symbol that the "addr" is in the function or the same of the function */
Elf32_Sym *get_func_symb_by_addr(Elf32_Sym *symtab, int tabsize, int addr)
{	
	for (;tabsize > 0; tabsize -= sizeof(Elf32_Sym), symtab ++)
	{		
#if 1
		if (ELF32_ST_TYPE(symtab->st_info) != STT_FUNC || symtab->st_shndx == 0)/* we only find the function symbol and not a und  */	
			continue;
		if (!symtab->st_value)
			continue;
		if (addr >= (s32)symtab->st_value && addr < (s32)(symtab->st_size+symtab->st_value))
			return symtab;	
#else
		if (ELF32_ST_TYPE(symtab->st_info) == STT_FUNC)/* we only find the function symbol */	
			printf("name %s addr %x size %x\r\n", (strtab+symtab->st_name), symtab->st_value, symtab->st_size);
#endif			
	}
	return NULL;
}	


Elf32_Sym *get_symbol(Elf32_Sym *symtab, int tabsize, int addr)
{
	for (;tabsize > 0; tabsize -= sizeof(Elf32_Sym), symtab ++)
		if (symtab->st_value == addr)
			return symtab;

	return NULL;		
}
Elf32_Shdr * read_elf_section(FILE *file, Elf32_Ehdr *ehdr)
{
	Elf32_Shdr *pshdr;	
	int s_size = ehdr->e_shentsize * ehdr->e_shnum;
	
	pshdr = (Elf32_Shdr *)malloc(s_size);
	if (pshdr == NULL) 
	{
		printf("malloc shdr error\r\n");
		return NULL;
	}	

	if (fseek (file, ehdr->e_shoff, SEEK_SET))
	{
        free(pshdr);
		printf("fseek error\r\n");
		return NULL;
	}	
	
	if (fread(pshdr, s_size, 1, file) != 1)
	{
        free(pshdr);
		printf("read file error in func read_elf_section\r\n");
		return NULL;
	}

	return pshdr;
}


void *read_symtable(FILE *file, Elf32_Shdr *shdr, Elf32_Ehdr *ehdr, int *size)
{
	int sec_num;
	int tb_size;
	void *pSym;

	
	for (sec_num = 0; sec_num < ehdr->e_shnum; sec_num ++, shdr ++)
	{
		if (shdr->sh_type == SHT_SYMTAB)
			break;
	}	
	
	if (sec_num == ehdr->e_shnum)
	{
		printf("No symbol table\n");
		return NULL;
	}
	*size = tb_size = shdr->sh_size;
	
	pSym = (void *)malloc(tb_size);
	if (pSym ==NULL)
	{	
		printf("malloc error in func read_symtable\r\n");
		return NULL;
	}
	if (fseek (file, shdr->sh_offset, SEEK_SET))
	{
        free(pSym);
		printf("fseek error\r\n");
		return NULL;
	}	

	if (fread(pSym, tb_size, 1, file) != 1)
	{
        free(pSym);
		printf("read file error in func read_symtable\r\n");
		return NULL;
	}	
	return pSym;
}

void *read_strtable(FILE *file, Elf32_Shdr *shdr, Elf32_Ehdr *ehdr,  int *size)
{
	int sec_num;
	int tb_size;
	void *pSym;
	Elf32_Shdr *lshdr = shdr;
	
	for (sec_num = 0; sec_num < ehdr->e_shnum; sec_num ++, shdr ++)
	{
		if (shdr->sh_type == SHT_SYMTAB)
			break;
	}	

	shdr  = lshdr + SECTION_HEADER_INDEX(shdr->sh_link);
	
	*size = tb_size = shdr->sh_size;
	
	pSym = (void *)malloc(tb_size);
	if (pSym ==NULL)
	{	
		printf("malloc error in func read_strtable\r\n");
		return NULL;
	}
	if (fseek (file, shdr->sh_offset, SEEK_SET))
	{
        free(pSym);
		printf("fseek error\r\n");
		return NULL;
	}	

	if (fread(pSym, tb_size, 1, file) != 1)
	{
        free(pSym);
		printf("read file error in func read_strtable\r\n");
		return NULL;
	}	

	return pSym;
}

int  read_elf_head(FILE *file, Elf32_Ehdr *ehdr)
{
	if (fread(ehdr, sizeof(Elf32_Ehdr), 1, file) != 1)
		return -1;

	return 0;
}


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
#ifndef _X86_
    return init_nipdebug(0, NULL, DIR_EXCLOG);
#else
	Elf32_Ehdr ehdr;
	Elf32_Shdr *shdr;
	FILE *file;

	install_sigaction();
	
	file = fopen(g_szAppName, "rb");
	if (file == NULL)
	{
		printf("CRI: Open file %s error: %s\n\n", g_szAppName, strerror(errno));
		return errno;
	}

	/* read the elf file's head */
	if (read_elf_head(file, &ehdr) == -1)
		return -1;
	
	/* read all sections of the elf file */
	shdr = read_elf_section(file, &ehdr);
	if (shdr == NULL)
		return -3;

	/* read static symbol table through symbol section*/
	sym = (Elf32_Sym *)read_symtable(file, shdr, &ehdr,  &symsize);
	if (sym == NULL)
		return -4;
	
	/* read string table through string section */
	strtab = (char *)read_strtable(file, shdr, &ehdr, &strsize);
	if (strtab == NULL)
		return -5;

	free(shdr);

    fclose(file);

	return 0;
#endif
}


#endif // _LINUX

/////////////////////////////////////////////////////�쳣������
