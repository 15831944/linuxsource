
#include "hduautotest.h"
#include "hduhwautotest.h"
#include "hduhwautotest_evid.h"
//#include "mcuver.h"


//#define  SERIAL_FILE_LEN                100
#define  CONNECT_TIMER_LENGTH           (10*1000)
#ifndef WIN32
	#define  VERSION_LEN                    32
#endif

//static SEMHANDLE s_semPingValue;

const s8 Host_IpAddr[] = "10.1.1.1";

CHduAutoTestClientApp g_cHduAutoTestApp;

CHduAutoTestClient::CHduAutoTestClient()
{
    m_dwHduAutoTestDstNode = INVALID_NODE;
    m_dwHduAutoTestDstInst = INVALID_INS;
}

CHduAutoTestClient::~CHduAutoTestClient()
{

}

/*====================================================================
  �� �� ���� DaemonInstanceEntry
  ��    �ܣ� Daemon���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
  --------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2009/05/04    4.6		    ���ֱ�                  ����
======================================================================*/
void CHduAutoTestClient::DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp )
{
	if (NULL == pcApp)
	{
		hdulog("[CHduAutoTestClient::DaemonInstanceEntry] pcApp is null!\n");
		return;
	}
	switch ( pMsg->event )
	{
	case EV_C_INIT:
        DaemonInit();
		break;

	case EV_C_CONNECT_TIMER:
		ConnectAndLoginTestServer();
		break;

    case S_C_REG_ACK:
		DaemonProcRegAckRsp();
		break;
 
	case EV_C_CHANGEAUTOTEST_CMD:
#ifndef WIN32
		DaemonProcChangeAutoTestCmd( pMsg );
#endif
		break;

	case S_C_REG_NACK:
		break;

	case ev_HDUAUTOTEST_CMD:
	{
		u8 byChlIndex;
		THDUAutoTestMsg tHduAutoTestMsg = *(THDUAutoTestMsg*)pMsg->content;

		tHduAutoTestMsg.Print();
		if ( 0 != tHduAutoTestMsg.GetAutoTestStart() )
		{
			printf("[CHduAutoTestClient] DaemonInstanceEntry: enter start!\n");

			for ( byChlIndex = 0; byChlIndex < MAXNUM_HDU_CHANNEL; byChlIndex ++ )
			{
				OspPost( MAKEIID( AID_HDUAUTO_TEST, byChlIndex + 1 ), EV_C_STARTSWITCH_CMD, pMsg->content, pMsg->length);
			}
			
			return;
		}
		
		if ( 0 == tHduAutoTestMsg.GetAutoTestStart() )
		{
			printf("[CHduAutoTestClient] DaemonInstanceEntry: enter stop!\n");

			for ( byChlIndex = 0; byChlIndex < MAXNUM_HDU_CHANNEL; byChlIndex ++ )
			{
				OspPost( MAKEIID( AID_HDUAUTO_TEST, byChlIndex + 1 ), EV_C_STOPSWITCH_CMD, pMsg->content, pMsg->length);
			}
			
			return;
		}
	}
		break;

	case ev_HDURESTORE_CMD:
#ifndef WIN32
        DaemonRestoreDefault( pMsg );
#endif
		break;

	case OSP_DISCONNECT:
		DaemonDisconnect();

		break;
		
	default:
		printf( "[DaemonInstanceEntry] message type (%u) is not exit!\n" ,pMsg->event );
		OspPrintf( TRUE, FALSE, "[DaemonInstanceEntry] message type (%u) is not exit!\n", pMsg->event );
	}

    return;
}
/*====================================================================
  �� �� ���� InstanceEntry
  ��    �ܣ� ��ͨʵ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
  --------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2009/05/04    4.6		    ���ֱ�                  ����
======================================================================*/
void CHduAutoTestClient::InstanceEntry( CMessage* const pMsg )
{
	printf("[CHduAutoTestClient] enter InstanceEntry! message type %d\n",pMsg->event );

    switch ( pMsg->event )
    {
	case EV_C_INITCHL:
		ProcInitChnl();		
		break;

	case EV_C_STARTSWITCH_CMD:
		ProcStartSwitchReq(pMsg);
        break;

	case EV_C_STOPSWITCH_CMD:
		ProcStopSwitchReq();		
		break;

	default:
		printf("[CHduAutoTestClient] wrong message type %u!\n", pMsg->event );

	}

    return;
}

/*====================================================================
  �� �� ���� DaemonInit
  ��    �ܣ� ��ʼ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
  --------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2009/05/04    4.6		    ���ֱ�                  ����
======================================================================*/
void CHduAutoTestClient::DaemonInit()
{
    printf( "[CHduAutoTestClient] enter DaemonInit!\n" );

	s32 nRet = 0;
#ifdef _HDUDVI_
	nRet = HardMPUInit(INITMODE_HDU_D);
	hdulog("Enter HardMPUInit(INITMODE_HDU_D)!\n");
    printf("Enter HardMPUInit(INITMODE_HDU_D)!\n");
#else
	nRet = HardMPUInit(u32(INITMODE_HDU));
	hdulog("Enter HardMPUInit(INITMODE_HDU)!\n");
    printf("Enter HardMPUInit(INITMODE_HDU)!\n");
#endif

	if ( CODEC_NO_ERROR != nRet )
	{
        hdulog("[CHduAutoTestClient] HardMPUInit error %d\n", nRet);
        printf("[CHduAutoTestClient] HardMPUInit error %d\n", nRet);
		return;
	}
	else
	{
        hdulog("[CHduAutoTestClient] HardMPUInit return %d\n", nRet);
        printf("[CHduAutoTestClient] HardMPUInit return %d\n", nRet);
	}
	
    u16 wRet = KdvSocketStartup();
    if ( MEDIANET_NO_ERROR != wRet)
    {
        OspPrintf(TRUE, FALSE, "KdvSocketStartup failed, error: %d\n", wRet);
        return ;
    }

	if ( !ConnectAndLoginTestServer() )
	{
		SetTimer( EV_C_CONNECT_TIMER, CONNECT_TIMER_LENGTH );
	}

	return;
}

/*====================================================================
  �� �� ���� DaemonProcConnectServerCmd
  ��    �ܣ� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
  --------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2009/05/04    4.6		    ���ֱ�                  ����
======================================================================*/
void CHduAutoTestClient::DaemonProcConnectServerCmd()
{
	KillTimer( EV_C_CONNECT_TIMER );
    if( !ConnectAndLoginTestServer() )
    {
        SetTimer( EV_C_CONNECT_TIMER, CONNECT_TIMER_LENGTH );
    }

    return;
}

/*====================================================================
  �� �� ���� DaemonProcRegAckRsp
  ��    �ܣ� ע���Ӧack
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
  --------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2009/05/04    4.6		    ���ֱ�                  ����
======================================================================*/
void CHduAutoTestClient::DaemonProcRegAckRsp()
{
	printf("[CHduAutoTestClient] enter DaemonProcRegAckRsp!\n");

	u8 byChlIndex;

	for ( byChlIndex = 0; byChlIndex < MAXNUM_HDU_CHANNEL; byChlIndex ++ )
	{
        BOOL bRet;
		bRet =OspPost( MAKEIID(AID_HDUAUTO_TEST, byChlIndex + 1), EV_C_INITCHL );
	    if (0 == bRet)
	    {
			printf("[CHduAutoTestClient] DaemonProcRegAckRsp:OspPost Sucessful!\n");
	    }
		else
		{
             printf("[CHduAutoTestClient] DaemonProcRegAckRsp:OspPost failed!\n");
		}
	}

    return;
}
 
/*====================================================================
  �� �� ���� DaemonProcRegNackRsp
  ��    �ܣ� ע���Ӧnack
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
  --------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2009/05/04    4.6		    ���ֱ�                  ����
======================================================================*/
void CHduAutoTestClient::DaemonProcRegNackRsp()
{
    return;
}

/*====================================================================
  �� �� ���� DaemonDisconnect
  ��    �ܣ� ��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
  --------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2009/05/04    4.6		    ���ֱ�                  ����
======================================================================*/
void CHduAutoTestClient::DaemonDisconnect()
{
	m_dwHduAutoTestDstInst = INVALID_INS;	
    m_bConnected = FALSE;
    
    if(INVALID_NODE == m_dwHduAutoTestDstNode)
	{
		OspPrintf( TRUE, FALSE, "[CHduAutoTestClient] DaemonDisconnect: m_dwHduAutoTestDstNode is invalid!\n" );
        return;   
	}

    OspDisconnectTcpNode(m_dwHduAutoTestDstNode);
	
    m_dwHduAutoTestDstNode = INVALID_NODE;
	SetTimer( EV_C_CONNECT_TIMER, CONNECT_TIMER_LENGTH );
    return;
}

/*====================================================================
  �� �� ���� ProcStartSwitchReq
  ��    �ܣ� ��ʼ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
  --------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2009/05/04    4.6		    ���ֱ�                  ����
======================================================================*/
void CHduAutoTestClient::DaemonProcStartSwitchReq( CMessage* const pMsg )
{
	for ( u8 byChlIndex = 0; byChlIndex < MAXNUM_HDU_CHANNEL; byChlIndex ++ )
	{
		post( MAKEIID( AID_HDUAUTO_TEST, byChlIndex + 1 ), EV_C_STARTSWITCH_CMD, pMsg->content, pMsg->length);
	}
	return;
}
  
/*====================================================================
  �� �� ���� ProcStopSwitchReq
  ��    �ܣ� ֹͣ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
  --------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2009/05/04    4.6		    ���ֱ�                  ����
======================================================================*/
void CHduAutoTestClient::DaemonProcStopSwitchReq( CMessage* const pMsg )
{
	for ( u8 byChlIndex = 0; byChlIndex < MAXNUM_HDU_CHANNEL; byChlIndex ++ )
	{
		post( MAKEIID( AID_HDUAUTO_TEST, byChlIndex + 1 ), EV_C_STOPSWITCH_CMD, pMsg->content, pMsg->length);
	}

	return;
}

/*====================================================================
  �� �� ���� DaemonRestoreDefault
  ��    �ܣ� �ָ�Ĭ��ֵ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
  --------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2009/05/04    4.6		    ���ֱ�                  ����
  2009/07/07    4.6         Ѧ��					���������ļ�����дE2PROM������
======================================================================*/
#ifndef WIN32
void CHduAutoTestClient::DaemonRestoreDefault( CMessage* const pMsg )
{
	BOOL32 bRet = FALSE;
    bRet = BrdClearE2promTestFlag();	//����Ա�־λ.����0��ɹ�
	u8 bySucessRestore = bRet?0:1;
	s32 sdwRet;
    sdwRet = post(pMsg->srcid, ev_HDURESTORE_NOTIFY, &bySucessRestore, sizeof(u8), pMsg->srcnode);
    if ( sdwRet != OSP_OK )
    {
        printf( "[DaemonRestoreDefault] post failed!\n" );
    }
	else
	{
		printf( "[DaemonRestoreDefault] post sucessful!\n" );
	}
	//reset
	OspDelay(2000);
	BrdHwReset();

    return;
//     s8   achProfileName[64] = {0};
// 
//     memset((void*)achProfileName, 0x0, sizeof(achProfileName));
// 
//     sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
// 
// 	BOOL32 bRet;
//     //IsAutoTest
//     s32  sdwTest = 0;
//     bRet = SetRegKeyInt( achProfileName, SECTION_BoardDebug, KEY_IsAutoTest, sdwTest );
//     if( !bRet )  
// 	{
// 	    printf( "[DaemonRestoreDefault] Wrong profile while reading %s%s!\n", SECTION_BoardDebug, KEY_IsAutoTest );
// 	    return;
// 	}
//     
// 	u8 bySucessRestore = bRet?1:0;
// 	s32 sdwRet;
//     sdwRet = post(pMsg->srcid, ev_HDURESTORE_NOTIFY, &bySucessRestore, sizeof(u8), pMsg->srcnode);
//     if ( sdwRet != OSP_OK )
//     {
//         printf( "[DaemonRestoreDefault] post failed!\n" );
//     }
// 	else
// 	{
// 		printf( "[DaemonRestoreDefault] post sucessful!\n" );
// 	}
}

/*====================================================================
  �� �� ���� ProcStartSwitchReq
  ��    �ܣ� ��ʼ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
  --------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2009/05/04    4.6		    ���ֱ�                  ����
======================================================================*/
void CHduAutoTestClient::DaemonProcChangeAutoTestCmd( CMessage* const pMsg )
{
	if ( NULL == pMsg)
	{
		OspPrintf( TRUE, FALSE, "[DaemonProcChangeAutoTestCmd] pMsg is Null!\n" );
		return;
	}
	s32 sdwAutoTest = *(s32*)pMsg->content;
	if( sdwAutoTest != 0)
	{
		BrdSetE2promTestFlag();
		OspPrintf( TRUE, FALSE, "[DaemonProcChangeAutoTestCmd] Set e2prom test flag 1\n" );
	}
	else 
	{
		BrdClearE2promTestFlag();
		OspPrintf( TRUE, FALSE, "[DaemonProcChangeAutoTestCmd] Set e2prom test flag 0\n" );
	}
/*
    s32 sdwAutoTest = *(s32*)pMsg->content;

	s8   achProfileName[64] = {0};
	
    memset((void*)achProfileName, 0x0, sizeof(achProfileName));
	
    sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
	
	BOOL32 bRet;
    //IsAutoTest
    bRet = SetRegKeyInt( achProfileName, SECTION_BoardDebug, KEY_IsAutoTest, sdwAutoTest );
    if( !bRet )  
	{
		printf( "[DaemonRestoreDefault] Wrong profile while reading %s%s!\n", SECTION_BoardDebug, KEY_IsAutoTest );
		return;
	}
   */
// 	BrdHwReset();
}
#endif
/*====================================================================
  �� �� ���� ProcStartSwitchReq
  ��    �ܣ� ��ʼ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
  --------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2009/05/04    4.6		    ���ֱ�                  ����
======================================================================*/
void CHduAutoTestClient::ProcStartSwitchReq( CMessage* const pMsg )
{
	printf("[CHduAutoTestClient] enter ProcStartSwitchReq!\n");

    THDUAutoTestMsg tHduAutoMsg = *(THDUAutoTestMsg*)pMsg->content;
    u32 dwHduTestItem = (u32)tHduAutoMsg.GetAutoTestType();
	TVidSrcInfo tVidSrcInfo;
	memset( &tVidSrcInfo, 0x0, sizeof(tVidSrcInfo) );
	
    TMediaEncrypt tMediaEncrypt;
    TDoublePayload tVidPayload, tAudPayload;
	BOOL bAudioTest =FALSE;      // ������Ƶ
	u8 byPortType = 255;
	u8 byModeType = 255;
	switch ( dwHduTestItem )
	{
	case 	emVGASXGA60Hz:
        byPortType = HDU_OUTPUT_VGA;
		byModeType = HDU_VGA_SXGA_60HZ;
		break;

    case	emVGAXGA60Hz:
        byPortType = HDU_OUTPUT_VGA;
		byModeType = HDU_VGA_XGA_60HZ;
        break;
	
	case    emYPbPr1080P60fps:
        byPortType = HDU_OUTPUT_YPbPr;
		byModeType = HDU_YPbPr_1080P_60fps;
		break;

	case    emYPbPr576i50Hz:
        byPortType = HDU_OUTPUT_YPbPr;
		byModeType = HDU_YPbPr_576i_50HZ;
		break;

    case    emAudio:
		bAudioTest = TRUE;
		break;

	default:
		OspPrintf(TRUE, FALSE, "[CHduAutoTestClient] output type is wrong!\n");
		return;
	}

	u16 wRecvPort = PRODUCETEST_RECVBASEPORT + (GetInsID()-1) * 4;
	u32 dwIpAddr = inet_addr(Host_IpAddr);
	BOOL bRet;
	if ( !bAudioTest )
	{
		printf("start video test!");
		bRet = GetVidSrcInfoByTypeAndMode(byPortType, 
			byModeType, 
			&tVidSrcInfo);
		if (!bRet)
		{
			printf("[CHduAutoTestClient] [ProcInitHdu]:GetVidSrcInfoByTypeAndMode fail\n");
			return;
		}
		
		bRet = m_pDecCroup->SetVideoPlyInfo(&tVidSrcInfo);
		if (!bRet)
		{
			printf("[CHduAutoTestClient] [ProcInitHdu] SetVideoPlyInfo failed!\n");
			return;
		}
		else
		{
			printf("[CHduAutoTestClient] [hdu]:m_pDecCroup->SetVideoPlyInfo ChnlIdx%d m_wWidth=%d, m_wHeight=%d, m_bProgressive=%d, m_dwFrameRate= %d\n", 
				GetInsID() - 1, tVidSrcInfo.m_wWidth, tVidSrcInfo.m_wHeight,
				tVidSrcInfo.m_bProgressive, tVidSrcInfo.m_dwFrameRate);
		}
		
		hdulog( "[CHduAutoTestClient] [Play Video] Process to Start Channel: %d\n", GetInsID()-1 );
		
		bRet = m_pDecCroup->StartDecode(TRUE, HDU_OUTPUTMODE_VIDEO);
		if(!bRet)
		{
			hdulog("[CHduInstance] Chn.%d Play failed as mode.%d \n",
				GetInsID()-1, HDU_OUTPUTMODE_VIDEO);
			printf("[CHduInstance] Chn.%d Play failed as mode.%d \n",
				GetInsID()-1, HDU_OUTPUTMODE_VIDEO);
		}
		else
		{
			printf( "[ProcStartSwitchReq] Start video decode sucessful!\n" );
		}

		tVidPayload.SetActivePayload( tHduAutoMsg.GetVideoType() );
		tVidPayload.SetRealPayLoad( tHduAutoMsg.GetVideoType() );	
		
		SetEncryptParam( &tMediaEncrypt, &tVidPayload );	

		TLocalNetParam tLocalNetParm;
        memset(&tLocalNetParm, 0, sizeof(TLocalNetParam));
        tLocalNetParm.m_tLocalNet.m_wRTPPort  = wRecvPort;
        tLocalNetParm.m_tLocalNet.m_wRTCPPort = wRecvPort + 1;
        tLocalNetParm.m_dwRtcpBackAddr        = htonl(dwIpAddr);
        tLocalNetParm.m_wRtcpBackPort         = wRecvPort + 1;
		bRet = m_pDecCroup->SetVidNetRcvLocalParam(&tLocalNetParm);
		
        if (!bRet)
        {
            hdulog("m_pDecCroup->ProcStartSwitchReq SetVidNetRcvLocalParam failed !\n");
            return;
        }
	}
	else
	{
		printf("start audio test!");
		hdulog( "[CHduAutoTestClient] [Play Audio] Process to Start Channel: %d\n", GetInsID()-1 );
		
		bRet = m_pDecCroup->StartDecode(TRUE, HDU_OUTPUTMODE_AUDIO);
		if(!bRet)
		{
			hdulog("[CHduInstance] Chn.%d Play failed as mode.%d \n",
				GetInsID()-1, HDU_OUTPUTMODE_AUDIO);
		}
		else
		{
			printf( "[ProcStartSwitchReq] Start audiodecode sucessful!\n" );
		}

		tMediaEncrypt.SetEncryptMode( CONF_ENCRYPTMODE_NONE );
		tAudPayload.SetActivePayload( tHduAutoMsg.GetAudioType() );
		tAudPayload.SetRealPayLoad( tHduAutoMsg.GetAudioType() );
		SetAudEnctypt(&tMediaEncrypt, &tAudPayload );

		TLocalNetParam tLocalNetParm;
        memset(&tLocalNetParm, 0, sizeof(TLocalNetParam));
        tLocalNetParm.m_tLocalNet.m_wRTPPort  = wRecvPort + 2;
        tLocalNetParm.m_tLocalNet.m_wRTCPPort = wRecvPort + 3;
        tLocalNetParm.m_dwRtcpBackAddr        = htonl(dwIpAddr);
        tLocalNetParm.m_wRtcpBackPort         = wRecvPort + 3;
		bRet = m_pDecCroup->SetAudNetRcvLocalParam(&tLocalNetParm);
		
        if (!bRet)
        {
            hdulog("m_pDecCroup->ProcStartSwitchReq SetAudNetRcvLocalParam failed !\n");
            return;
        }
	}

    return;
}
 
/*====================================================================
  �� �� ���� ProcStopSwitchReq
  ��    �ܣ� ֹͣ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
  --------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2009/05/04    4.6		    ���ֱ�                  ����
======================================================================*/
void CHduAutoTestClient::ProcStopSwitchReq()
{
	BOOL bRet;
	bRet = m_pDecCroup->StopDecode(TRUE, HDU_OUTPUTMODE_BOTH);
	if(!bRet)
	{
		hdulog("[ProcStopSwitchReq] Chn.%d Play failed as mode.%d \n",
			GetInsID()-1, HDU_OUTPUTMODE_BOTH);
	}
	else
	{
		printf( "[ProcStopSwitchReq] Stop audiodecode sucessful!\n" );
	}

	return;
}

/*====================================================================
  �� �� ���� ProcStopSwitchReq
  ��    �ܣ� ��ʼ��hduͨ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
  --------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2009/05/04    4.6		    ���ֱ�                  ����
======================================================================*/
void CHduAutoTestClient::ProcInitChnl()
{
    printf("[CHduAutoTestClient] enter ProcInitChnl! ChlIndex = %d\n", GetInsID() - 1 );

	BOOL bRet = TRUE;
    u32 dwChnIdx = GetInsID()-1;
    if ( m_pDecCroup == NULL )
    {
	    m_pDecCroup = new CDecoderGrp( );

        u16 wRecvPort = PRODUCETEST_RECVBASEPORT + dwChnIdx * 4;
	    u32 dwIpAddr = inet_addr(Host_IpAddr);

		hdulog("[CHduAutoTestClient][hdu] start Create!\n");
		printf("[CHduAutoTestClient][hdu] start Create!\n");
        bRet = m_pDecCroup->Create(dwChnIdx, 
 			                       dwIpAddr,
				                   wRecvPort + 1,
				                   wRecvPort,
				                   (u32)this);
		if ( !bRet )
		{
			printf("[CHduAutoTestClient][ProcInitHdu] Create failed!\n");
			return;
		}
	}
	else
	{
		printf("[ProcInitHdu] ChnlIdx%d: m_pcDecGrp is not Null!\n", dwChnIdx);
		return;
	}
    
    return;
}
/*====================================================================
  �� �� ���� ConnectAndLoginTestServer
  ��    �ܣ� ���Ӳ��Է������������½����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL
  --------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2009/05/04    4.6		    ���ֱ�                  ����
======================================================================*/
BOOL CHduAutoTestClient::ConnectAndLoginTestServer()
{
	printf("[ConnectAndLoginTestServer] enter!\n");
	
// 	BOOL bRet = TRUE;
// 	s8    achProfileName[64] = {0};
//     memset((void*)achProfileName, 0x0, sizeof(achProfileName));
// 	sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
// 	
// 	s8 achServeIpAddr[16] = {0};
// 	bRet = GetRegKeyString( achProfileName, SECTION_BoardConfig, KEY_ServerIpAddr, "0.0.0.0", achServeIpAddr, sizeof(achServeIpAddr) );
// 	
// 	if(!bRet)
// 	{
// 		printf("[ConnectAndLoginTestServer] Get Ip from brdcfgdebug.ini fail!\n");
// 		printf( "[ConnectAndLoginTestServer] connect to Server %s failed!\n",achServeIpAddr);
// 		return FALSE;
// 	}
	
//	u32 dwServerIpAddr = inet_addr(achServeIpAddr);

    u32 dwServerIpAddr = inet_addr(Host_IpAddr);
    if(INVALID_NODE != m_dwHduAutoTestDstNode) 
    {
        OspDisconnectTcpNode( m_dwHduAutoTestDstNode );
        m_dwHduAutoTestDstNode = INVALID_NODE;
    }

	printf("server ip is: %d\n",dwServerIpAddr);
    m_dwHduAutoTestDstNode = OspConnectTcpNode(dwServerIpAddr, 60000, 10, 3);
    
    if(INVALID_NODE == m_dwHduAutoTestDstNode)
    {
        hdulog("[CHduAutoTestClient::ConnectAndLoginTestServer]:Connect to Server Failed!\n");
		printf("[CHduAutoTestClient::ConnectAndLoginTestServer]:Connect to Server Failed!\n");
        return FALSE;
    }
    
    //����֪ͨע��
    OspNodeDiscCBReg( m_dwHduAutoTestDstNode, AID_HDUAUTO_TEST, CHduAutoTestClient::DAEMON );

    //���͵�½��Ϣ: ev_MTAutoTestLoginReq ��Ϣ�壺�汾�� + �û���Ϣ
    u8 abyTemp[sizeof(u8)*32 + sizeof(CLoginRequest) + sizeof(CDeviceInfo)];
    memset(abyTemp, 0, sizeof(abyTemp));
    
    CLoginRequest cLoginReq;
    cLoginReq.Empty();
    cLoginReq.SetName("admin");
    cLoginReq.SetPassword("admin");
    memcpy(abyTemp + sizeof(u8)*32, &cLoginReq, sizeof(CLoginRequest));
    
    CDeviceInfo cDeviceinfo;
	memset( &cDeviceinfo, 0x0, sizeof( cDeviceinfo ) );
    SetHduDeviceInfo(cDeviceinfo);
    memcpy(abyTemp + sizeof(u8)*32 + sizeof(CLoginRequest), &cDeviceinfo, sizeof(CDeviceInfo));
    post( MAKEIID( HDU_SERVER_APPID, 1 ), 
		C_S_REG_REQ, 
		abyTemp, 
        sizeof(u8) * 32  + sizeof(CLoginRequest) + sizeof(CDeviceInfo),
        m_dwHduAutoTestDstNode
		);

    return TRUE;
}

/*====================================================================
  �� �� ���� SetHduDeviceInfo
  ��    �ܣ� �����豸��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CDeviceInfo &cDeviceInfo
  �� �� ֵ�� 
  --------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2009/05/04    4.6		    ���ֱ�                  ����
======================================================================*/
void CHduAutoTestClient::SetHduDeviceInfo(CDeviceInfo &cDeviceInfo)
{
	s8 gs_VersionBuf[128] = {0};
    {
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
    }
	
    s8* pSoftVer = (s8*)gs_VersionBuf;
    cDeviceInfo.setSoftVersion( pSoftVer );
    
//     s8* pSoftVer = (s8*)VER_HDU;
    /*s8 achSerial[12] = {0};*/
	
    TBrdEthParam tBrdEthParam;
	memset( &tBrdEthParam, 0x0, sizeof(tBrdEthParam) );
    BrdGetEthParam( 0, &tBrdEthParam );
    
	u32 dwIp = tBrdEthParam.dwIpAdrs;
    u32 dwMask = tBrdEthParam.dwIpMask; 

    cDeviceInfo.setIp( dwIp );
    cDeviceInfo.setSubMask( dwMask );    
//     cDeviceInfo.setSoftVersion( pSoftVer );

#ifndef WIN32
    s8 achSerial[12] = {0};
	TBrdE2PromInfo tBrdE2PromInfo;
	s8 achHWversion[VERSION_LEN];
	s8 achMac[VERSION_LEN];
	memset( achMac, 0x0, sizeof(achMac) );
	memset( achHWversion, 0x0, sizeof(achHWversion) );
	memset( &tBrdE2PromInfo, 0x0, sizeof(tBrdE2PromInfo) );
    BrdGetE2PromInfo( &tBrdE2PromInfo );

	sprintf(achMac,
        "%02X-%02X-%02X-%02X-%02X-%02X",
        tBrdEthParam.byMacAdrs[0],
        tBrdEthParam.byMacAdrs[1],
        tBrdEthParam.byMacAdrs[2],
        tBrdEthParam.byMacAdrs[3],
        tBrdEthParam.byMacAdrs[4],
        tBrdEthParam.byMacAdrs[5]
        );  
    sprintf( achHWversion, "%d", tBrdE2PromInfo.dwHardwareVersion );
    memcpy( achSerial, tBrdE2PromInfo.chDeviceSerial, sizeof(tBrdE2PromInfo.chDeviceSerial) );
	cDeviceInfo.setHardVersion( achHWversion );
    cDeviceInfo.setMac( achMac );
    cDeviceInfo.setSerial( achSerial ); 
	
#endif

	return;
}

//END OF FILE

