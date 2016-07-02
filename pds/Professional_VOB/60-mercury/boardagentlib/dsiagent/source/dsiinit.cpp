    /*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : dsiinit.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ���������������ͨ��������
   ����        : jianghy
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/08/25  1.0         jianghy       ����
   2004/12/05  3.5         �� ��        �½ӿڵ��޸�
******************************************************************************/
#include "osp.h"
#include "dsiagent.h"
#include "dsiguard.h"
#include "evagtsvc.h"
#include "mcuconst.h"
#include "mcuver.h"
/*
*���ź�����MPCȡ������Ϣʱʹ��
*/
SEMHANDLE      g_semDSI;

extern s32 g_nLogLevel;

/*=============================================================================
  �� �� ���� setdsilog
  ��    �ܣ� ��ӡDsi����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� s32 nLvl��0������ӡ��1����ӡһ����Ϣ��2����ӡ��Ҫ��Ϣ
  �� �� ֵ�� API void 
=============================================================================*/
API void setdsilog(s32 nLvl)
{
	if( nLvl > 2 ) 
	{
		nLvl = 2;
	}
    g_nLogLevel = nLvl;
    return;
}

/*====================================================================
    ������      ��SetDsiAgentDebug
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
API void SetDsiAgentDebug()
{
	OspSetLogLevel( AID_MCU_BRDAGENT, 255, 255);	 
	OspSetTrcFlag( AID_MCU_BRDGUARD, 255, 255);

	OspSetLogLevel( AID_MCU_BRDAGENT, 255, 255);	 
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
	pchPeerIpAddr = inet_ntoa( *(struct in_addr*)&dwIp );
	strncpy( achPeerIpAddr, pchPeerIpAddr, sizeof(achPeerIpAddr) );
	achPeerIpAddr[ sizeof(achPeerIpAddr) - 1 ] = '\0';	
#endif

	return (char*)achPeerIpAddr;
}

/*====================================================================
    ������      ��DriAgentSelfTest
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
API void DsiAgentSelfTest()
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


API u8 GetBoardSlot()
{
	return g_cBrdAgentApp.GetBoardId();
}

/*====================================================================
    ������      ��dsiagtver
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
API void dsiagtver(void)
{
	OspPrintf( TRUE, FALSE, "DsiAgent: %s  compile time: %s    %s\n",  
		VER_DSIAGENT, __DATE__, __TIME__ );
}

/*=============================================================================
�� �� ���� dsihelp
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
API void dsihelp()
{
	OspPrintf( TRUE, FALSE, "dsiagtver:				dsiagt version command\n");
	OspPrintf( TRUE, FALSE, "SetDsiAgentDebug:		set dsiagt debug command\n");
	OspPrintf( TRUE, FALSE, "DsiAgentSelfTest:		self test debug command\n");
}

/*====================================================================
    ������      ��DsiAgentAPIEnableInLinux
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
void DsiAgentAPIEnableInLinux()
{
#ifdef _LINUX_    
    OspRegCommand("dsiagtver", (void*)dsiagtver, "dsiagt version command");    
    OspRegCommand("DsiAgentSelfTest", (void*)DsiAgentSelfTest, "self test debug command");
	OspRegCommand("SetDsiAgentDebug", (void*)SetDsiAgentDebug, "set dsiagt debug command");
	OspRegCommand("setdsilog", (void*)setdsilog, "set dsi log level: 0-always print, 1-common info, 2-important info");
	OspRegCommand("dsihelp",	(void*)dsihelp, "dsi command help");
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
    ������      ��InitDsiAgent
    ����        ����ʼ��DSI�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ���ɹ�����TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/01/12    1.0         jianghy       ����
====================================================================*/
API BOOL InitDsiAgent()
{

	DsiAgentAPIEnableInLinux();

	static u16 wFlag;
	if( wFlag != 0 )
	{
		printf("BrdAgent: Already Inited!\n" );
		return TRUE;
	}

	wFlag++;
	if( !OspSemBCreate(&g_semDSI) )
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

	OspSemTake(g_semDSI);

	//������
	if( !g_cBrdAgentApp.ReadConnectMcuInfo() )
	{
		return FALSE;
	}
	//����ftpĿ¼
	CreateftpDir( );

	//����������Ϣ�ָ�E1��·��RELAY MODE
#ifndef WIN32
	g_cBrdAgentApp.ResumeE1RelayMode();
#endif

	//�����������Ӧ��
	g_cBrdAgentApp.CreateApp( "DsiAgent", AID_MCU_BRDAGENT, APPPRI_BRDAGENT );
	//������������Ӧ��
	g_cBrdGuardApp.CreateApp( "DsiGuard", AID_MCU_BRDGUARD, APPPRI_BRDGUARD);
	//�������ʼ����
	OspPost( MAKEIID(AID_MCU_BRDAGENT,0,0), BOARD_AGENT_POWERON );
	//��������ģ�鿪ʼ����
	OspPost( MAKEIID(AID_MCU_BRDGUARD,0,0), BOARD_GUARD_POWERON );

	OspSetLogLevel( AID_MCU_BRDAGENT, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDGUARD, 0, 0);

	OspSetLogLevel( AID_MCU_BRDAGENT, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDGUARD, 0, 0);

	//wait here
	OspSemTake(g_semDSI);

	return TRUE;
}

