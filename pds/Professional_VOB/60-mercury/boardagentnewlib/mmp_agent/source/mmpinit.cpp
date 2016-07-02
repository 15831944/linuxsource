/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : mmpinit.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ���������������ͨ��������
   ����        : jianghy
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/08/25  1.0         jianghy       ����
   2004/11/11  3.5         �� ��         �½ӿڵ��޸�
******************************************************************************/
#include "osp.h"
#include "mmp_agent.h"
//#include "evagtsvc.h"
#include "mcuver.h"
#include "boardguardbasic.h"
/*lint -save -e765*/
/*
*���ź�����MPCȡ������Ϣʱʹ��
*/
SEMHANDLE      g_semMMP;

/*====================================================================
    ������      ��SetMmpAgentDebug
    ����        ������MMPAGENT��ӡΪDEBUG����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/01/12    1.0         jianghy       ����
====================================================================*/
API void SetMmpAgentDebug()
{
	OspSetLogLevel( AID_MCU_BRDAGENT, 255, 255);	 
	OspSetTrcFlag( AID_MCU_BRDAGENT, 255, 255);

	OspSetLogLevel( AID_MCU_BRDGUARD, 255, 255);	 
	OspSetTrcFlag( AID_MCU_BRDGUARD, 255, 255);
	return;
}

/*====================================================================
    ������      ��IptoStr
    ����        ����IP��ַ(������)ת�����ַ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��IP�ַ���
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/01/12    1.0         jianghy       ����
====================================================================*/
static char* IptoStr( u32 dwIpAddr )
{
	static char achPeerIpAddr[20];
	char* pchPeerIpAddr;
	u32 dwIp = htonl( dwIpAddr );

#ifdef _VXWORKS_   /*��IP��ַת�����ַ���*/ 
	inet_ntoa_b(*(struct in_addr*)&dwIp, achPeerIpAddr);
#else // WIN32 and linux
	pchPeerIpAddr = inet_ntoa( *(struct in_addr*)(void*)&dwIp );
	strncpy( achPeerIpAddr, pchPeerIpAddr, sizeof(achPeerIpAddr) );
	achPeerIpAddr[ sizeof(achPeerIpAddr) - 1 ] = '\0';	
#endif

	return (char*)achPeerIpAddr;
}

/*====================================================================
    ������      ��MmpAgentSelfTest
    ����        ����ӡ��ǰ��������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/01/12    1.0         jianghy       ����
====================================================================*/
API void MmpAgentSelfTest()
{
	char abyBuf[100];
	memset(abyBuf, 0, sizeof(abyBuf));

	//��ͨ��Ϣ
	OspPrintf(TRUE, FALSE, "Board Id = %d \n", g_cBrdAgentApp.GetBoardId() ); 

	OspPrintf(TRUE, FALSE, "Connect Mcu Id = %d \n", g_cBrdAgentApp.GetMcuId() ); 
	OspPrintf(TRUE, FALSE, "Connect Mcu Ip = %s \n", IptoStr(ntohl(g_cBrdAgentApp.GetMpcIpA())) ); 
	OspPrintf(TRUE, FALSE, "Connect Mcu Port = %d \n", g_cBrdAgentApp.GetMpcPortA() ); 

#ifndef WIN32
	//����λ��, zgc, 07/02/02
	TBrdPosition tBrdPos;
	BrdQueryPosition( &tBrdPos );  //�˴�����תΪTBrdPos,��Ϊֻ�Ǵ�ӡ��jlb_081120
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


	//vmp ������Ϣ
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
	u8 byLoop=0;
	for( byLoop=0; byLoop<g_cBrdAgentApp.GetVMPMAPId((u8*)abyBuf, 100); byLoop++)
	{
		u32 dwCoreSpeed, dwMemSpeed, dwMemSize, dwPort;
		g_cBrdAgentApp.GetMAPInfo(abyBuf[byLoop], &dwCoreSpeed, &dwMemSpeed, &dwMemSize, &dwPort);
		OspPrintf(TRUE, FALSE, "      %d    %d    %d    %d    %d \n", abyBuf[byLoop], dwCoreSpeed, dwMemSpeed, dwMemSize, dwPort ); 
	}

	//bas ������Ϣ
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

	//tvwall ������Ϣ
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

	//Mixer��������Ϣ
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

	//prs ������Ϣ
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
    ������      ��mmpagtver
    ����        ����ӡ��ǰģ��İ汾��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/01/12    1.0         jianghy       ����
====================================================================*/
API void mmpagtver(void)
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
	OspPrintf( TRUE, FALSE, "MmpAgent: %s  compile time: %s    %s\n",  
		VER_MMPAGENT, __DATE__, __TIME__ );
	OspPrintf( TRUE, FALSE, "BasicBoardAgent: %s  compile time: %s    %s\n",  
		VER_BASICBOARDAGENT, __DATE__, __TIME__ );
}

/*=============================================================================
�� �� ���� brdagthelp
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� API void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/5/17   4.0			�ܹ��                  ����
=============================================================================*/
API void brdagthelp()
{
	OspPrintf( TRUE, FALSE, "mmpagtver:			mmpagt version command\n");
	OspPrintf( TRUE, FALSE, "SetMmpAgentDebug:	set mmpagt debug command\n");
	OspPrintf( TRUE, FALSE, "MmpAgentSelfTest:	self test debug command\n");
	OspPrintf( TRUE, FALSE, "pmmpmsg:			print mmp msg\n");
	OspPrintf( TRUE, FALSE, "npmmpmsg:			stop printing mmp msg\n");
}

/*====================================================================
    ������      MmpAgentAPIEnableInLinux
    ����        ����Linuxע��API����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/02/05    4.0         �ܹ��         ����
====================================================================*/
/*lint -save -e611*/
void MmpAgentAPIEnableInLinux()
{
#ifdef _LINUX_ 
    OspRegCommand("mmpagtver", (void*)mmpagtver, "mmpagt version command");    
    OspRegCommand("SetMmpAgentDebug",   (void*)SetMmpAgentDebug,   "set mmpagt debug command");  
    OspRegCommand("MmpAgentSelfTest", (void*)MmpAgentSelfTest, "self test debug command");
	OspRegCommand("pmmpmsg", (void*)pmmpmsg, "print mmp msg");
	OspRegCommand("npmmpmsg", (void*)npmmpmsg, "stop printing mmp msg");
	OspRegCommand("brdagthelp", (void*)brdagthelp, "brd agent command help");
#endif
    
    return;
}

/*====================================================================
    ������      ��CreateftpDir
    ����        ������FTPĿ¼
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/01/12    1.0         jianghy       ����
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
    ������      ��InitMmpAgent
    ����        ����ʼ��MMP�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ���ɹ�����TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/01/12    1.0         jianghy       ����
====================================================================*/
/*lint -save -e537*/
API BOOL InitMmpAgent()
{
	
    BOOL bResult;

	static u16 wFlag = 0;
	if( wFlag != 0 )
	{
		printf("BrdAgent: Already Inited!\n" );
		return TRUE;
	}
    
#undef OSPEVENT
#define OSPEVENT( x, y ) OspAddEventDesc( #x, y )
    
#ifdef _EV_AGTSVC_H_
#undef _EV_AGTSVC_H_
#include "evagtsvc.h"
#define _EV_AGTSVC_H_
#else
#include "evagtsvc.h"
#undef _EV_AGTSVC_H_
#endif    

	// ����API��zgc, 2007-03-28
	MmpAgentAPIEnableInLinux();

	wFlag++;
	if( !OspSemBCreate(&g_semMMP) )
	{
        printf("[InitMmpAgent] OspSemBCreate failed\n");
		return FALSE;
	}

#ifndef _VXWORKS_
	if( ERROR == BrdInit() )
	{
		printf("[InitAgent] call BrdInit fail.\n");
		return FALSE;
	}
#endif

	bResult = OspSemTake(g_semMMP);
    if (bResult == FALSE)
    {
        OspPrintf( TRUE, FALSE, "BrdAgent: OspSemTake(g_semMMP) error!\n" );
    }

	//�õ�������Ϣ
    bResult = g_cBrdAgentApp.ReadConfig();
    if( bResult != TRUE )
    {
        printf("[InitMmpAgent] ReadConfig failed\n");
        return FALSE;
    }
	//����ftpĿ¼
	CreateftpDir( );

	//�����������Ӧ��
	g_cBrdAgentApp.CreateApp( "MmpAgent", AID_MCU_BRDAGENT, APPPRI_BRDAGENT );
	//������������Ӧ��
	g_cBrdGuardApp.CreateApp( "MmpGuard", AID_MCU_BRDGUARD, APPPRI_BRDGUARD);
	//�������ʼ����
	CBBoardConfig *pBBoardConfig = (CBBoardConfig*)&g_cBrdAgentApp;
	OspPost( MAKEIID(AID_MCU_BRDAGENT,0,0), OSP_POWERON, &pBBoardConfig, sizeof(pBBoardConfig) );
	// �����������浥��������Ϣ��������ݲ�ͬ���������в�ͬ�澯 [12/1/2011 chendaiwei]
	u8 byBrdId = pBBoardConfig->GetBrdPosition().byBrdID;
	OspPost( MAKEIID(AID_MCU_BRDGUARD,0,0), BOARD_GUARD_POWERON,&byBrdId,sizeof(u8));

	OspSetLogLevel( AID_MCU_BRDAGENT, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDAGENT, 0, 0);

	OspSetLogLevel( AID_MCU_BRDGUARD, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDGUARD, 0, 0);

	//wait here
	bResult = OspSemTake(g_semMMP);
    if (bResult == FALSE)
    {
        OspPrintf( TRUE, FALSE, "BrdAgent: OspSemTake(g_semMMP) error!\n" );
    }

    /*For Test*/
    //u8 byState = 1;
/*    CMessage tmpmesg;
    tmpmesg.event = SVC_AGT_MEMORY_STATUS;
    tmpmesg.content = &byState;
    ProcBoardMemeryStatus(&tmpmesg);
*/
    //OspPost( MAKEIID(AID_MCU_BRDAGENT,0), SVC_AGT_MEMORY_STATUS );

	return TRUE;
}
/*lint -restore*/





