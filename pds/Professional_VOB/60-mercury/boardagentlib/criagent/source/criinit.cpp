/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : BoardInit.cpp
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
#include "criagent.h"
#include "criguard.h"
#include "evagtsvc.h"
#include "mcuconst.h"
#include "mcuver.h"

/*
*���ź�����MPCȡ������Ϣʱʹ��
*/
SEMHANDLE      g_semCRI;

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
	pchPeerIpAddr = inet_ntoa( *(struct in_addr*)&dwIp );
	strncpy( achPeerIpAddr, pchPeerIpAddr, sizeof(achPeerIpAddr) );
	achPeerIpAddr[ sizeof(achPeerIpAddr) - 1 ] = '\0';	
#endif

	return (char*)achPeerIpAddr;
}

/*====================================================================
    ������      ��CriAgentSelfTest
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
API void CriAgentSelfTest()
{
	OspPrintf(TRUE, FALSE, "Board Id    = %d \n", g_cBrdAgentApp.GetBoardId() ); 
	OspPrintf(TRUE, FALSE, "Mcu IpAddr  = %s \n", IptoStr(ntohl(g_cBrdAgentApp.GetConnectMcuIpAddr())) ); 
	OspPrintf(TRUE, FALSE, "Mcu Port	= %d \n", g_cBrdAgentApp.GetConnectMcuPort() ); 
	OspPrintf(TRUE, FALSE, "Brd Port	= %s \n", IptoStr(ntohl(g_cBrdAgentApp.GetBrdIpAddr())) ); 

#ifndef WIN32
	//����λ��, zgc, 07/02/02
	TBrdPosition tBrdPos;
	BrdQueryPosition( &tBrdPos );
	OspPrintf( TRUE, FALSE, "Brd Layer: %d, Brd Slot: %d\n", tBrdPos.byBrdLayer, tBrdPos.byBrdSlot );
#endif

	//prs ������Ϣ
	if( g_cBrdAgentApp.IsRunPrs() )
	{
		TPrsCfg tPrsCfg;
		g_cBrdAgentApp.GetPrsCfg(&tPrsCfg);
		OspPrintf(TRUE, FALSE, "\n\nPrs Id          = %d \n", tPrsCfg.byEqpId ); 
		OspPrintf(TRUE, FALSE, "Prs Type            = %d \n", tPrsCfg.byEqpType ); 
		OspPrintf(TRUE, FALSE, "Prs Alias           = %s \n", tPrsCfg.achAlias ); 
		OspPrintf(TRUE, FALSE, "Prs IpAddr          = %s \n", IptoStr(ntohl(tPrsCfg.dwLocalIP)) ); 
		OspPrintf(TRUE, FALSE, "Prs Recv Start Port = %d \n", tPrsCfg.wRcvStartPort ); 

		OspPrintf(TRUE, FALSE, "Prs First Time Span  = %d \n", tPrsCfg.m_wFirstTimeSpan ); 
		OspPrintf(TRUE, FALSE, "Prs Second Time Span = %d \n", tPrsCfg.m_wSecondTimeSpan ); 
		OspPrintf(TRUE, FALSE, "Prs Third Time Span  = %d \n", tPrsCfg.m_wThirdTimeSpan ); 
		OspPrintf(TRUE, FALSE, "Prs Reject Time Span = %d \n", tPrsCfg.m_wRejectTimeSpan ); 

	
	}


	return;
}

/*====================================================================
    ������      ��criagtver
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
API void criagtver(void)
{
	OspPrintf( TRUE, FALSE, "CriAgent: %s  compile time: %s    %s\n",  
		VER_CRIAGENT, __DATE__, __TIME__ );
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
	mkdir( (s8*)DIR_FTP, 700 );
#endif
}

/*====================================================================
    ������      ��SetCriAgentDebug
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
API void SetCriAgentDebug()
{
	OspSetLogLevel( AID_MCU_BRDAGENT, 255, 255);	 
	OspSetTrcFlag( AID_MCU_BRDAGENT, 255, 255);

	OspSetLogLevel( AID_MCU_BRDGUARD, 255, 255);	 
	OspSetTrcFlag( AID_MCU_BRDGUARD, 255, 255);
	return;
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
	OspPrintf( TRUE, FALSE, "criagtver:			criagt version command\n");
	OspPrintf( TRUE, FALSE, "SetCriAgentDebug:	set criagt debug command\n");
	OspPrintf( TRUE, FALSE, "CriAgentSelfTest:	self test debug command\n");
	OspPrintf( TRUE, FALSE, "pcrimsg:			print crimsg command\n");
	OspPrintf( TRUE, FALSE, "npcrimsg:			not print pcrimsg command\n");
}

/*====================================================================
    ������      ��CriAgentAPIEnableInLinux
    ����        ����Linuxע��API����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/02/02    4.0         �ܹ��         ����
====================================================================*/
void CriAgentAPIEnableInLinux()
{
#ifdef _LINUX_ 
    OspRegCommand("criagtver", (void*)criagtver, "criagt version command");    
    OspRegCommand("SetCriAgentDebug",   (void*)SetCriAgentDebug,   "set criagt debug command");  
    OspRegCommand("CriAgentSelfTest", (void*)CriAgentSelfTest, "self test debug command");
	OspRegCommand("pcrimsg",   (void*)pcrimsg,   "print crimsg command");
	OspRegCommand("npcrimsg",   (void*)npcrimsg,   "not print pcrimsg command");
	OspRegCommand("brdagthelp",	(void*)brdagthelp, "brd agent command help");
#endif
    
    return;
}

/*====================================================================
    ������      ��InitCriAgent
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
API BOOL InitCriAgent()
{

	CriAgentAPIEnableInLinux();

	static u16 wFlag;
	if( wFlag != 0 )
	{
		printf("BrdAgent: Already Inited!\n" );
		return TRUE;
	}

	wFlag++;
	if( !OspSemBCreate(&g_semCRI) )
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

	OspSemTake(g_semCRI);

	//������
	if( !g_cBrdAgentApp.ReadConnectMcuInfo() )
	{
		return FALSE;
	}
	//����ftpĿ¼
	CreateftpDir( );

/*	if( !g_cBrdAgentApp.ReadPrsInfo() )
	{
		return FALSE;
	}
*/

	//�����������Ӧ��
	g_cBrdAgentApp.CreateApp( "CriAgent", AID_MCU_BRDAGENT, APPPRI_BRDAGENT );
	//������������Ӧ��
	g_cBrdGuardApp.CreateApp( "CriGuard", AID_MCU_BRDGUARD, APPPRI_BRDGUARD);
	//�������ʼ����
	OspPost( MAKEIID(AID_MCU_BRDAGENT,0,0), BOARD_AGENT_POWERON );
	//��������ģ�鿪ʼ����
	OspPost( MAKEIID(AID_MCU_BRDGUARD,0,0), BOARD_GUARD_POWERON );

	OspSetLogLevel( AID_MCU_BRDAGENT, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDAGENT, 0, 0);

	OspSetLogLevel( AID_MCU_BRDGUARD, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDGUARD, 0, 0);

    //SetCriAgentDebug();

	//wait here
	OspSemTake(g_semCRI);
	
	return TRUE;
}

/*====================================================================
    ������      ��GetBoardSlot
    ����        ����ȡ��Ĳۺ�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/01/12    1.0         jianghy       ����
====================================================================*/
API u8 GetBoardSlot()
{
	return g_cBrdAgentApp.GetBoardId();
}


