/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : bapagent.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: �������
   ����        : guzh
   �汾        : V4.5  Copyright(C) 2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
******************************************************************************/

#include "bapagent.h"

CBrdAgentApp	g_cBrdAgentApp;	//�������Ӧ��ʵ��

BOOL32 CBoardConfig::ReadConfig()
{
    BOOL32 bRet = CBBoardConfig::ReadConnectMcuInfo();
    if (!bRet)
    {
        return bRet;
    }

    s8    achProfileName[64] = {0};
    s32   sdwValue;
    
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);

    // EqpId
	bRet = GetRegKeyInt( achProfileName, SECTION_Eqp, KEY_Id, BASID_MIN, &sdwValue );
	if( !bRet )  
	{
		OspPrintf( TRUE, TRUE, "[BrdAgent] Wrong profile while reading %s%s!\n", SECTION_Eqp, KEY_Id );
		return FALSE;
	}
    m_byEqpId = ( u8 )sdwValue;
    return TRUE;
}


void AgentAPIEnableInLinux()
{
#ifdef _LUNUX_
    
#endif
}

API BOOL InitBrdAgent()
{
    BOOL bResult;

	static u16 wFlag;
	if( wFlag != 0 )
	{
		printf("[BrdAgent] Already Inited!\n" );
		return TRUE;
	}
    
#ifndef _VXWORKS_
    if( ERROR == BrdInit() )
    {
        printf("[BrdAgent] BrdInit fail.\n");
        return FALSE;
    }
#endif

    ::OspInit(TRUE, BRD_TELNET_PORT);
    
    //Osp telnet ��ʼ��Ȩ [11/28/2006-zbq]
#ifndef WIN32
    OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );
#endif

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

	// ����API
	AgentAPIEnableInLinux();

	wFlag++;


	//�õ�������Ϣ
    bResult = g_cBrdAgentApp.ReadConfig();
    if( bResult != TRUE )
    {
        printf("[BrdAgent] ReadConfig failed.\n");
        return FALSE;
    }

	//�����������Ӧ��
	g_cBrdAgentApp.CreateApp( "BapAgent", AID_MCU_BRDAGENT, APPPRI_BRDAGENT );

	//�������ʼ����
	CBBoardConfig *pBBoardConfig = (CBBoardConfig*)&g_cBrdAgentApp;
	OspPost( MAKEIID(AID_MCU_BRDAGENT, 1), OSP_POWERON, &pBBoardConfig, sizeof(pBBoardConfig) );

	OspSetLogLevel( AID_MCU_BRDAGENT, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDAGENT, 0, 0);

	return TRUE;
}


API u32 BrdGetDstMcuNode()
{
    return g_cBrdAgentApp.m_dwDstMcuNodeA;
}

API u32 BrdGetDstMcuNodeB()
{
    return g_cBrdAgentApp.m_dwDstMcuNodeB;
}
