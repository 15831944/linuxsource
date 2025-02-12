/*****************************************************************************
   模块名      : Board Agent
   文件名      : imtinit.cpp
   相关文件    : 
   文件实现功能: 单板启动及相关普通函数定义
   作者        : jianghy
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/08/25  1.0         jianghy       创建
   2004/12/05  3.5         李 博        新接口的修改
******************************************************************************/
#include "osp.h"
#include "imtagent.h"
#include "imtguard.h"
#include "evagtsvc.h"
#include "mcuconst.h"
#include "mcuver.h"

/*
*该信号量向MPC取配置信息时使用
*/
SEMHANDLE      g_semImt;
extern s32 g_nLogLevel;

API void pimtmsg(void)
{
	g_nLogLevel = 1;
    return;
}

API void npimtmsg(void)
{
	g_nLogLevel = 0;
	return;
}

/*====================================================================
    函数名      ：IptoStr
    功能        ：将IP地址(主机序)转换成字符串
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：IP字符串
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/01/12    1.0         jianghy       创建
====================================================================*/
static char* IptoStr( u32 dwIpAddr )
{
	static char achPeerIpAddr[20];
	char* pchPeerIpAddr;
	u32 dwIp = htonl( dwIpAddr );

#ifdef _VXWORKS_   /*将IP地址转换成字符串*/ 
	inet_ntoa_b(*(struct in_addr*)&dwIp, achPeerIpAddr);
#else // WIN32 and linux
	pchPeerIpAddr = inet_ntoa( *(struct in_addr*)&dwIp );
	strncpy( achPeerIpAddr, pchPeerIpAddr, sizeof(achPeerIpAddr) );
	achPeerIpAddr[ sizeof(achPeerIpAddr) - 1 ] = '\0';	
#endif

	return (char*)achPeerIpAddr;
}

/*====================================================================
    函数名      ：SetMmpAgentDebug
    功能        ：设置MMPAGENT打印为DEBUG级别
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/01/12    1.0         jianghy       创建
====================================================================*/
API void SetImtAgentDebug()
{
	OspSetLogLevel( AID_MCU_BRDAGENT, 255, 255);	 
	OspSetTrcFlag( AID_MCU_BRDAGENT, 255, 255);

	OspSetLogLevel( AID_MCU_BRDGUARD, 255, 255);	 
	OspSetTrcFlag( AID_MCU_BRDGUARD, 255, 255);
	return;
}

/*====================================================================
    函数名      ：MmpAgentSelfTest
    功能        ：打印当前的配置信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/01/12    1.0         jianghy       创建
====================================================================*/
API void ImtAgentSelfTest()
{
	s8 abyBuf[100];
	memset(abyBuf, 0, sizeof(abyBuf));

	//普通信息
	OspPrintf(TRUE, FALSE, "Board Id = %d \n", g_cBrdAgentApp.GetBoardId() ); 

	OspPrintf(TRUE, FALSE, "Connect Mcu Id = %d \n", g_cBrdAgentApp.GetConnectMcuId() ); 
	OspPrintf(TRUE, FALSE, "Connect Mcu Ip = %s \n", IptoStr(ntohl(g_cBrdAgentApp.GetConnectMcuIpAddr())) ); 
	OspPrintf(TRUE, FALSE, "Connect Mcu Port = %d \n", g_cBrdAgentApp.GetConnectMcuPort() ); 

#ifndef WIN32
	//单板位置, zgc, 07/02/02
	TBrdPosition tBrdPos;
	BrdQueryPosition( &tBrdPos );
	OspPrintf( TRUE, FALSE, "Brd Layer: %d, Brd Slot: %d\n", tBrdPos.byBrdLayer, tBrdPos.byBrdSlot );
#endif

	if( g_cBrdAgentApp.IsRunMixer() )
		OspPrintf(TRUE, FALSE, "Mixer Run On This Board\n");

	if( g_cBrdAgentApp.IsRunTVWall() )
		OspPrintf(TRUE, FALSE, "TVWall Run On This Board\n");

	if( g_cBrdAgentApp.IsRunBas() )
		OspPrintf(TRUE, FALSE, "Bas Run On This Board\n");

	if( g_cBrdAgentApp.IsRunVMP() )
		OspPrintf(TRUE, FALSE, "VMP Run On This Board\n");


	//vmp 配置信息
	OspPrintf(TRUE, FALSE, "\n\nVMP Id              = %d \n", g_cBrdAgentApp.GetVMPId() ); 
	OspPrintf(TRUE, FALSE, "VMP Type            = %d \n", g_cBrdAgentApp.GetVMPType() ); 
	memset(abyBuf, 0, sizeof(abyBuf));
	g_cBrdAgentApp.GetVMPAlias(abyBuf,100);
	OspPrintf(TRUE, FALSE, "VMP Alias           = %s \n", abyBuf ); 
	OspPrintf(TRUE, FALSE, "VMP IpAddr          = %s \n", IptoStr(ntohl(g_cBrdAgentApp.GetVMPIpAddr())) ); 
	OspPrintf(TRUE, FALSE, "VMP Recv Start Port = %d \n", g_cBrdAgentApp.GetVMPRecvStartPort() ); 
	OspPrintf(TRUE, FALSE, "VMP Decode Number   = %d \n", g_cBrdAgentApp.GetVMPDecodeNumber() ); 

	OspPrintf(TRUE, FALSE, "      MAP TABLE:\n\n");
	OspPrintf(TRUE, FALSE, "      Id    CoreSpeed     MemSpeed     MemSize     Port \n"); 

	u8 byLoop = 0;
	for( byLoop = 0; byLoop<g_cBrdAgentApp.GetVMPMAPId((u8*)abyBuf, 100); byLoop++)
	{
		u32 dwCoreSpeed, dwMemSpeed, dwMemSize, dwPort;
		g_cBrdAgentApp.GetMAPInfo(abyBuf[byLoop], &dwCoreSpeed, &dwMemSpeed, &dwMemSize, &dwPort);
		OspPrintf(TRUE, FALSE, "      %d    %d    %d    %d    %d \n", abyBuf[byLoop], dwCoreSpeed, dwMemSpeed, dwMemSize, dwPort ); 
	}

	//bas 配置信息
	OspPrintf(TRUE, FALSE, "\n\nBAS Id          = %d \n", g_cBrdAgentApp.GetBasId() ); 
	OspPrintf(TRUE, FALSE, "BAS Type            = %d \n", g_cBrdAgentApp.GetBasType() ); 
	memset(abyBuf, 0, sizeof(abyBuf));
	g_cBrdAgentApp.GetBasAlias(abyBuf,100);
	OspPrintf(TRUE, FALSE, "BAS Alias           = %s \n", abyBuf ); 
	OspPrintf(TRUE, FALSE, "BAS IpAddr          = %s \n", IptoStr(ntohl(g_cBrdAgentApp.GetBasIpAddr())) ); 
	OspPrintf(TRUE, FALSE, "BAS Recv Start Port = %d \n", g_cBrdAgentApp.GetBasRecvStartPort() ); 
	OspPrintf(TRUE, FALSE, "Map's Max Adp Chnl  = %d \n", g_cBrdAgentApp.GetBasMaxAdpChannel() ); 

	OspPrintf(TRUE, FALSE, "      MAP TABLE:\n\n");
	OspPrintf(TRUE, FALSE, "      Id    CoreSpeed     MemSpeed     MemSize     Port \n"); 
	for( byLoop=0; byLoop<g_cBrdAgentApp.GetBasMAPId((u8*)abyBuf, 100); byLoop++)
	{
		u32 dwCoreSpeed, dwMemSpeed, dwMemSize, dwPort;
		g_cBrdAgentApp.GetMAPInfo(abyBuf[byLoop], &dwCoreSpeed, &dwMemSpeed, &dwMemSize, &dwPort);
		OspPrintf(TRUE, FALSE, "      %d    %d    %d    %d    %d \n", abyBuf[byLoop], dwCoreSpeed, dwMemSpeed, dwMemSize, dwPort ); 
	}

	//tvwall 配置信息
	OspPrintf(TRUE, FALSE, "\n\nTVWall Id          = %d \n", g_cBrdAgentApp.GetTWId() ); 
	OspPrintf(TRUE, FALSE, "TVWall Type            = %d \n", g_cBrdAgentApp.GetTWType() ); 
	memset(abyBuf, 0, sizeof(abyBuf));
	g_cBrdAgentApp.GetTWAlias(abyBuf,100);
	OspPrintf(TRUE, FALSE, "TVWall Alias           = %s \n", abyBuf ); 
	OspPrintf(TRUE, FALSE, "TVWall IpAddr          = %s \n", IptoStr(ntohl(g_cBrdAgentApp.GetTWIpAddr())) ); 
	OspPrintf(TRUE, FALSE, "TVWall Recv Start Port = %d \n", g_cBrdAgentApp.GetTWRecvStartPort() ); 
	OspPrintf(TRUE, FALSE, "TVWall Div Style       = %d \n", g_cBrdAgentApp.GetTWDivStyle() ); 
	OspPrintf(TRUE, FALSE, "TVWall Div Num         = %d \n", g_cBrdAgentApp.GetTWDivNum() ); 

	OspPrintf(TRUE, FALSE, "      MAP TABLE:\n\n");
	OspPrintf(TRUE, FALSE, "      Id    CoreSpeed     MemSpeed     MemSize     Port \n"); 
	for( byLoop=0; byLoop<g_cBrdAgentApp.GetTVWallMAPId((u8*)abyBuf, 100); byLoop++)
	{
		u32 dwCoreSpeed, dwMemSpeed, dwMemSize, dwPort;
		g_cBrdAgentApp.GetMAPInfo(abyBuf[byLoop], &dwCoreSpeed, &dwMemSpeed, &dwMemSize, &dwPort);
		OspPrintf(TRUE, FALSE, "      %d    %d    %d    %d    %d \n", abyBuf[byLoop], dwCoreSpeed, dwMemSpeed, dwMemSize, dwPort ); 
	}

	//Mixer的配置信息
	OspPrintf(TRUE, FALSE, "\n\nMixer Id          = %d \n", g_cBrdAgentApp.GetMixerId() ); 
	OspPrintf(TRUE, FALSE, "Mixer Type            = %d \n", g_cBrdAgentApp.GetMixerType() ); 
	memset(abyBuf, 0, sizeof(abyBuf));
	g_cBrdAgentApp.GetMixerAlias(abyBuf,100);
	OspPrintf(TRUE, FALSE, "Mixer Alias           = %s \n", abyBuf ); 
	OspPrintf(TRUE, FALSE, "Mixer IpAddr          = %s \n", IptoStr(ntohl(g_cBrdAgentApp.GetMixerIpAddr())) ); 
	OspPrintf(TRUE, FALSE, "Mixer Recv Start Port = %d \n", g_cBrdAgentApp.GetMixerRecvStartPort() ); 
	OspPrintf(TRUE, FALSE, "Map's Max Mix Group   = %d \n", g_cBrdAgentApp.GetMixerMaxMixGroupNum() ); 
	OspPrintf(TRUE, FALSE, "Max Channel in Group  = %d \n", g_cBrdAgentApp.GetMixerMaxChannelInGrp() ); 

	OspPrintf(TRUE, FALSE, "      MAP TABLE:\n\n");
	OspPrintf(TRUE, FALSE, "      Id    CoreSpeed     MemSpeed     MemSize     Port \n"); 
	for( byLoop=0; byLoop<g_cBrdAgentApp.GetMixerMAPId((u8*)abyBuf, 100); byLoop++)
	{
		u32 dwCoreSpeed, dwMemSpeed, dwMemSize, dwPort;
		g_cBrdAgentApp.GetMAPInfo(abyBuf[byLoop], &dwCoreSpeed, &dwMemSpeed, &dwMemSize, &dwPort);
		OspPrintf(TRUE, FALSE, "      %d    %d    %d    %d    %d \n", abyBuf[byLoop], dwCoreSpeed, dwMemSpeed, dwMemSize, dwPort ); 
	}

	//prs 配置信息
	if( g_cBrdAgentApp.IsRunPrs() )
	{
		OspPrintf(TRUE, FALSE, "\n\nPrs Id          = %d \n", g_cBrdAgentApp.GetPrsId() ); 
		OspPrintf(TRUE, FALSE, "Prs Type            = %d \n", g_cBrdAgentApp.GetPrsType() ); 
		memset(abyBuf, 0, sizeof(abyBuf));
		g_cBrdAgentApp.GetPrsAlias(abyBuf,100);
		OspPrintf(TRUE, FALSE, "Prs Alias           = %s \n", abyBuf ); 
		OspPrintf(TRUE, FALSE, "Prs IpAddr          = %s \n", IptoStr(ntohl(g_cBrdAgentApp.GetPrsIpAddr())) ); 
		OspPrintf(TRUE, FALSE, "Prs Recv Start Port = %d \n", g_cBrdAgentApp.GetPrsRecvStartPort() ); 

		u16 wTimeSpan[4];
		g_cBrdAgentApp.GetPrsRetransPara(wTimeSpan, wTimeSpan+1, wTimeSpan+2, wTimeSpan+3 );

		OspPrintf(TRUE, FALSE, "Prs First Time Span  = %d \n", wTimeSpan[0] ); 
		OspPrintf(TRUE, FALSE, "Prs Second Time Span = %d \n", wTimeSpan[1] ); 
		OspPrintf(TRUE, FALSE, "Prs Third Time Span  = %d \n", wTimeSpan[2] ); 
		OspPrintf(TRUE, FALSE, "Prs Reject Time Span = %d \n", wTimeSpan[3] ); 

	
	}

	return;
}

/*====================================================================
    函数名      ：imtagtver
    功能        ：打印当前模块的版本信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/01/12    1.0         jianghy       创建
====================================================================*/
API void imtagtver(void)
{
	OspPrintf( TRUE, FALSE, "ImtAgent: %s  compile time: %s    %s\n",  
		VER_IMTAGENT, __DATE__, __TIME__ );
}

/*=============================================================================
函 数 名： brdagthelp
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： API void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/5/17   4.0			周广程                  创建
=============================================================================*/
API void brdagthelp()
{
	OspPrintf( TRUE, FALSE, "imtagtver:				imtagt version command\n");
	OspPrintf( TRUE, FALSE, "SetImtAgentDebug:		set imtagt debug command\n");
	OspPrintf( TRUE, FALSE, "ImtAgentSelfTest:		self test debug command\n");
	OspPrintf( TRUE, FALSE, "pimtmsg:				print imt msg\n");
	OspPrintf( TRUE, FALSE, "npimtmsg:				stop printing imt msg\n");
}

/*====================================================================
    函数名      ：ImtAgentAPIEnableInLinux
    功能        ：向Linux注册API函数
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/02/05    4.0         周广程         创建
====================================================================*/
void ImtAgentAPIEnableInLinux()
{
#ifdef _LINUX_ 
    OspRegCommand("imtagtver", (void*)imtagtver, "imtagt version command");    
    OspRegCommand("SetImtAgentDebug",   (void*)SetImtAgentDebug,   "set imtagt debug command");  
    OspRegCommand("ImtAgentSelfTest", (void*)ImtAgentSelfTest, "self test debug command");
	OspRegCommand("pimtmsg", (void*)pimtmsg, "print imt msg");
	OspRegCommand("npimtmsg", (void*)npimtmsg, "stop printing imt msg");
	OspRegCommand("brdagthelp",	(void*)brdagthelp, "brd agent command help");
#endif
    
    return;
}

/*====================================================================
    函数名      ：CreateftpDir
    功能        ：创建FTP目录
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/01/12    1.0         jianghy       创建
====================================================================*/
void CreateftpDir( void )
{
#ifdef WIN32
	CreateDirectory( ( LPCTSTR )DIR_FTP, NULL );
#elif defined( _VXWORKS_ )
	mkdir( ( s8* )DIR_FTP );
#else // linux
	mkdir( (s8*)DIR_FTP, 0777 );
    chmod( (s8*)DIR_FTP, 0777 );
#endif
}


/*====================================================================
    函数名      ：InitMmpAgent
    功能        ：初始化MMP单板代理
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：成功返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/01/12    1.0         jianghy       创建
====================================================================*/
API BOOL InitImtAgent()
{
	ImtAgentAPIEnableInLinux();

    BOOL bResult;

	static u16 wFlag;
	if( wFlag != 0 )
	{
		printf("BrdAgent: Already Inited!\n" );
		return TRUE;
	}

	wFlag++;
	if( !OspSemBCreate(&g_semImt) )
	{
		return FALSE;
	}

#ifdef _LINUX_
	if( ERROR == BrdInit() )
	{
		printf("[InitAgent] call BrdInit fail.\n");
		return FALSE;
	}
#endif
	
	OspSemTake(g_semImt);

	//得到配置信息
    bResult = g_cBrdAgentApp.ReadConfig();
    if( bResult != TRUE )
    {
        return FALSE;
    }
	//创建ftp目录
	CreateftpDir( );

	//创建单板代理应用
	g_cBrdAgentApp.CreateApp( "ImtAgent", AID_MCU_BRDAGENT, APPPRI_BRDAGENT );
	//创建单板守卫应用
	g_cBrdGuardApp.CreateApp( "ImtGuard", AID_MCU_BRDGUARD, APPPRI_BRDGUARD);
	//单板代理开始工作
	OspPost( MAKEIID(AID_MCU_BRDAGENT,0,0), BOARD_AGENT_POWERON );
	//单板守卫模块开始工作
	OspPost( MAKEIID(AID_MCU_BRDGUARD,0,0), BOARD_GUARD_POWERON );

	OspSetLogLevel( AID_MCU_BRDAGENT, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDAGENT, 0, 0);

	OspSetLogLevel( AID_MCU_BRDGUARD, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDGUARD, 0, 0);

	//wait here
	OspSemTake(g_semImt);
	return TRUE;
}

API void FanAlarm()
{
	u8 byBuf[2];

	byBuf[0] = 1;
	byBuf[1] = 1;
	OspPost( MAKEIID(AID_MCU_BRDAGENT, 1), MCU_BRD_FAN_STATUS, byBuf, sizeof(byBuf) );
	return;
}




