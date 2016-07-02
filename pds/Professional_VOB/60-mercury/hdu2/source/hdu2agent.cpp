/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : hdu2agent.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: �������
   ����        : ��־��
   �汾        : V4.7  Copyright(C) 2011 KDV, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   11/10/27    4.7         ��־��        ����
******************************************************************************/

#include "hdu2agent.h"
CHdu2BrdAgentApp  g_cBrdAgentApp;
static SEMHANDLE        g_semHdu;
/*=====================================================================
����  : InstanceEntry
����  : HDU2�������Ϣ�����
����  : CMessage * const pcMsg
���  : ��
����  : void
ע    :
-----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2011/11/28  4.7         ��־��        ����
=====================================================================*/
void CHdu2BoardAgent::InstanceEntry(CMessage * const pcMsg)
{
	if ( pcMsg == NULL )
	{
		hdu2agtLog(LOG_LVL_ERROR, MID_MCU_EQP,"[CHdu2BoardAgent::InstanceEntry]pcMsg == NULL,So Return!\n" );
		return;
	}
	hdu2agtLog(LOG_LVL_DETAIL, MID_MCU_EQP,"[CHdu2BoardAgent::InstanceEntry]CHdu2BoardAgent Rcv Message %u(%s) passed!\n", pcMsg->event, ::OspEventDesc(pcMsg->event) );
	switch (pcMsg->event)
	{
	    case MPC_BOARD_GET_CONFIG_ACK:       //ȡ������Ϣȷ��Ӧ��
		    ProcBoardGetConfigAck( pcMsg );
		    break;
		case MPC_BOARD_GET_CONFIG_NACK:      //ȡ������Ϣ��Ӧ��
		    ProcBoardGetConfigNAck( pcMsg );
		    break;
		default:
			CBBoardAgent::InstanceEntry(pcMsg);
			break;
	}
}
/*=====================================================================
����  : ProcBoardGetConfigAck
����  : HDU2������ȡ�����óɹ�����
����  : CMessage * const pcMsg
���  : ��
����  : void
ע    :
-----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2011/11/28  4.7         ��־��        ����
=====================================================================*/
void  CHdu2BoardAgent::ProcBoardGetConfigAck( CMessage* const pcMsg )
{
	if( pcMsg == NULL )
    {
        hdu2agtLog(LOG_LVL_ERROR, MID_MCU_EQP,"[ProcBoardGetConfigAck]pcMsg == NULL,So Return!\n" );
        return;
    }
    
	TEqpHduEntry *ptHduEntry = (TEqpHduEntry*)(pcMsg->content + sizeof(u8));
	if ( 0 != g_cBrdAgentApp.GetEqpId() && g_cBrdAgentApp.GetEqpId() != ptHduEntry->GetEqpId() )
	{
		BrdHwReset();
	}
    g_cBrdAgentApp.SetEqpId( ptHduEntry->GetEqpId() );
	g_cBrdAgentApp.SetStartMode(ptHduEntry->GetStartMode());
	THduChnlModePortAgt tHduChnModePort;
	THduModePort tModePort;
	for (u8 byChnIdx = 0 ;byChnIdx < MAXNUM_HDU_CHANNEL;byChnIdx++)
	{
		ptHduEntry->GetHduChnlModePort(byChnIdx,tHduChnModePort);
		tModePort.SetOutModeType(tHduChnModePort.GetOutModeType());
		tModePort.SetOutPortType(tHduChnModePort.GetOutPortType());
		tModePort.SetZoomRate(tHduChnModePort.GetZoomRate());
		tModePort.SetScalingMode(tHduChnModePort.GetScalingMode());
		g_cBrdAgentApp.SetChnCfg(byChnIdx,tModePort);
	}

	if ( !OspSemGive(g_semHdu) )
	{
		hdu2agtLog(LOG_LVL_ERROR, MID_MCU_EQP,"[ProcBoardGetConfigAck] OspSemGive fail! in ProcBoardGetConfigAck()\n");
	}
	KillTimer( BRDAGENT_GET_CONFIG_TIMER );
	NEXTSTATE( STATE_NORMAL );

	/*u8 abyBuf[32];
	//TODO ����汾�Ŵ�ͳһ���壬�Ƿ��ú궨��[12/13/2011 chendaiwei]
	u32 dwSoftWareVersion = 1212;
	TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
	memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) );
	memcpy(abyBuf+sizeof(TBrdPos),&dwSoftWareVersion,sizeof(u32));
	
	//֪ͨMPC�����������汾����Ϣ [12/13/2011 chendaiwei]
	PostMsgToManager( BOARD_MPC_SOFTWARE_VERSION_NOTIFY, abyBuf, 
		sizeof(u32)+sizeof(TBrdPos) );
		*/
}
/*=====================================================================
����  : ProcBoardGetConfigNAck
����  : HDU2������ȡ�����óɹ�����
����  : CMessage * const pcMsg
���  : ��
����  : void
ע    :
-----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2011/11/28  4.7         ��־��        ����
=====================================================================*/
void  CHdu2BoardAgent::ProcBoardGetConfigNAck( CMessage* const pcMsg )
{
	hdu2agtLog(LOG_LVL_WARNING, MID_MCU_EQP, "[ProcBoardGetConfigNack]The function ProcBoardGetConfigNAck()\n",
		pcMsg != NULL ? pcMsg->srcnode : 0);
}

/*====================================================================
    ������      ��ProcGetVersionReq
    ����        ��MPC���͸�����Ļ�ȡ�汾��Ϣ��Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    11/12/14    4.7         �´�ΰ        ����
====================================================================*/
/*void CHdu2BoardAgent::ProcGetVersionReq( CMessage* const pcMsg )
{
	s8 abyBuf[MAX_SOFT_VER_LEN+1+sizeof(TBrdPos)] = {0};
	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:
		//ȡ�汾��Ϣ
		{
		memset(abyBuf, 0, sizeof(abyBuf) );

		TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
		memcpy(abyBuf, &tBrdPos,sizeof(tBrdPos) );
		
		//TODO ����汾�Ŵ�ͳһ���壬�Ƿ��ú궨��[12/13/2011 chendaiwei]
		sprintf(abyBuf+sizeof(tBrdPos),"softwareversin:hdu2[4.7]");

		//���͸�MANAGER��Ӧ
		PostMsgToManager( BOARD_MPC_GET_VERSION_ACK, (u8*)abyBuf, sizeof(abyBuf) );
		break;
		}
		
	default:
		LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}*/

/*=====================================================================
����  : ProcBoardGetConfigNack
����  : HDU2������ȡ�����óɹ�����
����  : CMessage * const pcMsg
���  : ��
����  : void
ע    :
-----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2011/11/28  4.7         ��־��        ����
=====================================================================*/
BOOL32 CHdu2BoardConfig::ReadConfig()
{
	BOOL32 bIsTest = FALSE;
	s32 sdwReturnValue = 0;
	BOOL32 bRet = FALSE;
	s8    achProfileName[64] = {0};
	memset((void*)achProfileName, 0x0, sizeof(achProfileName));
	sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
#ifdef _LINUX_
	bIsTest = BrdGetE2promTestFlag();
#else
	bRet = GetRegKeyInt( achProfileName, SECTION_BoardDebug, "IsTest", 0, &sdwReturnValue );
	if( !bRet )  
	{
		printf( "[BrdAgent] Wrong profile while reading [%s][%s]!\n", SECTION_BoardDebug, "IsTest" );
	}
	bIsTest = (sdwReturnValue != 0);
#endif
	g_cBrdAgentApp.SetIsTest( bIsTest );
	if (bIsTest)
	{
		GetBoardInfo();//��������ʱҪ������HDU2�廹��HDU2_L��
		printf("[ReadConfig]bIsTest,So Return!\n");
		return TRUE;
	}
    bRet = CBBoardConfig::ReadConnectMcuInfo();
    if ( !bRet )
    {
		printf("[ReadConfig]CBBoardConfig::ReadConnectMcuInfo Failed,So Return!\n");
        return bRet;
    }

	TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
	bRet = GetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Layer, 0, &sdwReturnValue );
	if( !bRet )  
	{
		printf( "[BrdAgent] Wrong profile while reading [%s][%s]!\n", SECTION_BoardConfig, KEY_Layer );
		return FALSE;
	}
	tBrdPos.byBrdLayer = (u8)sdwReturnValue;
	BOOL32 bIsSimuHdu = FALSE;
	bRet = GetRegKeyInt( achProfileName, SECTION_BoardConfig, "IsSimuHdu", 0, &sdwReturnValue );
	if( !bRet )  
	{
		printf( "[BrdAgent] Wrong profile while reading [%s][%s]!\n", SECTION_BoardConfig, "IsSimuHdu" );
	}
	else
	{
		bIsSimuHdu = (sdwReturnValue != 0);
		if (bIsSimuHdu)
		{
			if (tBrdPos.byBrdID == BRD_TYPE_HDU2)
			{
				tBrdPos.byBrdID = BRD_TYPE_HDU;
				//��ȡ��������ӿ����� ����YPbPr:1 VGA:2 DVI:3 HDMI:4
				//û�����û������Ƿ�ֵ���ʾ���������ý������ã��������������������
				bRet = GetRegKeyInt( achProfileName, SECTION_BoardConfig, "OutPortType0", 0, &sdwReturnValue );
				g_cBrdAgentApp.SetOutPortTypeInCfg(0,0);
				g_cBrdAgentApp.SetOutPortTypeInCfg(1,0);
				if(bRet)
				{
					if ((u8)sdwReturnValue <= 4)
					{
						g_cBrdAgentApp.SetOutPortTypeInCfg(0,(u8)sdwReturnValue);
					}
				}
				bRet = GetRegKeyInt( achProfileName, SECTION_BoardConfig, "OutPortType1", 0, &sdwReturnValue );
				if(bRet)
				{
					if ((u8)sdwReturnValue <= 4)
					{
						g_cBrdAgentApp.SetOutPortTypeInCfg(1,(u8)sdwReturnValue);
					}
				}
			}
			else
			{
				printf( "[ReadConfig]tBrdPos.byBrdID(%d) Can't SimuHDU(%d),So Return!!!!\n",tBrdPos.byBrdID, BRD_TYPE_HDU );
				return FALSE;
			}
		}
	}
	g_cBrdAgentApp.SetBrdPosition(tBrdPos);
    return TRUE;
}
/*=====================================================================
����  : InitBrdAgent
����  : HDU2������ʼ��
����  : void
���  : ��
����  : BOOL32
ע    :
-----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2011/11/28  4.7         ��־��        ����
=====================================================================*/
API BOOL32 InitBrdAgent(void)
{
	static u16 wFlag = 0;
	if( wFlag != 0 )
	{
		printf("[InitBrdAgent]Hdu2BrdAgent Already Inited! So Return TURE!\n" );
		return TRUE;
	}
   	wFlag++;

#ifndef _VXWORKS_
    if( ERROR == BrdInit() )
    {
        printf("[InitBrdAgent]BrdInit failed,So Return FALSE!\n");
        return FALSE;
    }
#endif

    if ( !OspInit(TRUE, BRD_TELNET_PORT) )
    {
        printf("[InitBrdAgent]OspInit failed,So Return FALSE!\n");
        return FALSE;
    }

#ifndef WIN32
    OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );
#endif

	//�õ�������Ϣ
    if( !g_cBrdAgentApp.ReadConfig() )
    {
        printf("[InitBrdAgent] ReadConfig failed.\n");
        return FALSE;
    }

	if(g_cBrdAgentApp.GetIsTest())		//����������·��
	{
		u32 dwBrdIpAddr = 0;

#ifdef _LINUX_
		BrdGetE2promIpAddr(&dwBrdIpAddr);//�����ڼ�IP��E2PROM�ж�ȡ
		if(dwBrdIpAddr == 0)
		{
			printf( "[InitBrdAgent]Read Board Ip from E2PROM error!\n" );
			return FALSE;
		}
		printf("[InitBrdAgent]Board Ip(network sequence): %x\n", dwBrdIpAddr);
        TBrdEthParam tEtherPara;
        tEtherPara.dwIpAdrs = dwBrdIpAddr;
        tEtherPara.dwIpMask = inet_addr("255.0.0.0");
        memset( tEtherPara.byMacAdrs, 0, sizeof(tEtherPara.byMacAdrs) );
        if( ERROR == BrdSetEthParam( 0, Brd_SET_IP_AND_MASK, &tEtherPara ) )
        {
            printf("[InitBrdAgent]Set Eth 0 IP failed\n");
        }
#endif

		g_cBrdAgentApp.SetBrdIpAddr(dwBrdIpAddr);
		return TRUE;
	}

    if ( !OspSemBCreate(&g_semHdu) )
	{
        printf("[InitBrdAgent]OspSemBCreate failed,So Return FALSE!\n");
		return FALSE;
	}
	
    if ( !OspSemTake( g_semHdu ) )
    {
        printf("[InitBrdAgent]OspSemTake failed,So Return FALSE!\n");
		return FALSE;
    }

	//�����������Ӧ��
	g_cBrdAgentApp.CreateApp( "Hdu2Agent", AID_MCU_BRDAGENT, APPPRI_BRDAGENT );
	//������������Ӧ��
	g_cBrdGuardApp.CreateApp( "Hdu2Guard", AID_MCU_BRDGUARD, APPPRI_BRDGUARD);
	//�������ʼ����
	CBBoardConfig *pBBoardConfig = (CBBoardConfig*)&g_cBrdAgentApp;
	OspPost( MAKEIID(AID_MCU_BRDAGENT, 1), OSP_POWERON, &pBBoardConfig, sizeof(pBBoardConfig) );
	//��������ģ�鿪ʼ����
	// �����������浥��������Ϣ��������ݲ�ͬ���������в�ͬ�澯 [12/1/2011 chendaiwei]
	u8 byBrdId = pBBoardConfig->GetBrdPosition().byBrdID;
	OspPost( MAKEIID(AID_MCU_BRDGUARD,0,0), BOARD_GUARD_POWERON,&byBrdId,sizeof(u8));
	//wait here
    if ( !OspSemTake( g_semHdu) )
    {
        printf("[InitBrdAgent] OspSemTake failed,So Return FALSE!\n");
		return FALSE;
    }
	OspSetLogLevel( AID_MCU_BRDAGENT, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDAGENT, 0, 0);
	return TRUE;
}
/*=====================================================================
����  : BrdGetDstMcuNode
����  : ��ȡ������MCU��NODE�ڵ��
����  : void
���  : ��
����  : u32
ע    :
-----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2011/11/28  4.7         ��־��        ����
=====================================================================*/
u32 BrdGetDstMcuNode(void)
{
    return g_cBrdAgentApp.m_dwDstMcuNodeA;
}
/*=====================================================================
����  : BrdGetDstMcuNodeB
����  : ��ȡ���ӱ�MCU��NODE�ڵ��
����  : void
���  : ��
����  : u32
ע    :
-----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2011/11/28  4.7         ��־��        ����
=====================================================================*/
u32 BrdGetDstMcuNodeB(void)
{
    return g_cBrdAgentApp.m_dwDstMcuNodeB;
}
