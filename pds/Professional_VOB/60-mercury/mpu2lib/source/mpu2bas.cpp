/*****************************************************************************
   ģ����      : mpu2lib
   �ļ���      : mpu2bas.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: mpu2lib bas̬��֧ʵ��
   ����        : ��־��
   �汾        : V4.7  Copyright(C) 2011-2012KDC, All rights reserved.
******************************************************************************/
#include "mpu2inst.h"
#include "evmpu2.h"
#include "mcuver.h"
#include "evmcueqp.h"
#include "mpu2baserr.h"
#include "boardagent.h"

/*====================================================================
	����  : DaemonInstanceEntry
	����  : Daemonʵ����Ϣ���
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
    ��  ��      �汾        �޸���      �޸�����
    2011/12/06  4.7         ��־��       ����
====================================================================*/
/*lint -save -e715*/
void CMpu2BasInst::DaemonInstanceEntry( CMessage* const pMsg, CApp* pcApp )
{
    switch ( pMsg->event )
    {
    //�ϵ��ʼ��
    case OSP_POWERON:
		DaemonProcPowerOn( pMsg);
		break;

    // ����
    case EV_BAS_CONNECT:
        DaemonProcConnectTimeOut( TRUE );
        break;
	// ����Mcu
    case EV_BAS_CONNECTB:
        DaemonProcConnectTimeOut( FALSE );
        break;

    // ע����Ϣ
    case EV_BAS_REGISTER:
	case EV_SECOND_BAS_REGISTER:
        DaemonProcRegisterTimeOut( pMsg, TRUE );
        break;
	// ע��
    case EV_BAS_REGISTERB: 
	case EV_SECOND_BAS_REGISTERB:
        DaemonProcRegisterTimeOut( pMsg, FALSE );
        break;

    // MCU ע��Ӧ����Ϣ
    case MCU_BAS_REG_ACK:
#ifdef _8KI_
		DaemonProc8KIMcuRegAck(pMsg);
#else
		DaemonProcMcuRegAck( pMsg );
#endif  
        break;

    // MCU�ܾ�BASע��
    case MCU_BAS_REG_NACK:
        DaemonProcMcuRegNack( pMsg );
        break;

    // OSP ������Ϣ
    case OSP_DISCONNECT:
        DaemonProcOspDisconnect( pMsg/*, pcApp*/ );
        break;

    // ȡ��������״̬
    case EV_BAS_GETMSSTATUS:
    case MCU_EQP_GETMSSTATUS_ACK:
        DeamonProcGetMsStatusRsp(pMsg);
		break;

	case MCU_EQP_MODSENDADDR_CMD:
		DeamonProcModSendIpAddr(pMsg);
		break;

    // ��ʼ����
    case MCU_HDBAS_STARTADAPT_REQ:
    // ֹͣ����
    case MCU_HDBAS_STOPADAPT_REQ:
    // �����������
    case MCU_HDBAS_SETADAPTPARAM_CMD:
    // MCU�����һ���ؼ�֡
    case MCU_BAS_FASTUPDATEPIC_CMD:
        {
            CServMsg cServMsg( pMsg->content, pMsg->length );
            u8 byChnIdx = cServMsg.GetChnIndex();	//BASͨ��idx��0��ʼƫ��
            u8 byChnId = byChnIdx + 1;				//ת����instidʱ��Ҫ��1
			u8 byIdx;						     	//BAS����idx��0��ʼƫ��
			for (byIdx = 0; byIdx < g_cMpu2BasApp.GetBasNum(); byIdx++)
			{
				TMpu2BasCfg tMpu2BasCfg;
				if ( !g_cMpu2BasApp.GetMpu2BasCfg(byIdx,tMpu2BasCfg) )
				{
					ErrorLog("[DaemonInstanceEntry]GetMpu2BasCfg idx:%d failed!\n",byIdx);
					continue;
				}
				
				if ( (u16)pMsg->srcid == tMpu2BasCfg.m_tEqp.GetEqpId() )
				{
					break;
				}
			}
			u8 byInitNum = 0;
			if(!GetBasInitChnlNumAcd2WorkMode(g_cMpu2BasApp.GetBasWorkMode(), byInitNum))
			{
				ErrorLog("[DaemonInstanceEntry] GetBasInitChnlNumAcd2WorkMode failed!\n");
				return;
			}
            post( MAKEIID( GetAppID(), byChnId + byIdx*byInitNum), pMsg->event, pMsg->content, pMsg->length );
            break;        
        }
	case EV_BAS_SHOWDEBUG:
		{
			DeamonShowBasDebugInfo();
			break;
		}
    default:
        break;
    }
    return;
}
/*lint -restore*/
/*====================================================================
	����  : InstanceEntry
	����  : ��ͨʵ����Ϣ���
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
    ��  ��      �汾        �޸���      �޸�����
    2012/07/26  4.7         ��־��         ����
====================================================================*/
void CMpu2BasInst::InstanceEntry( CMessage* const pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    cServMsg.SetEventId(pcMsg->event);
    
    if ( pcMsg->event != EV_BAS_NEEDIFRAME_TIMER &&
         pcMsg->event != EV_BAS_STATUSCHK_TIMER )
    {
        DetailLog("instid:%d, basidx:%d,[BasInst.%d] Receive msg.%d<%s>\n", 
				  GetInsID(), m_byBasIdx, m_byBasChnIdx, pcMsg->event, ::OspEventDesc(pcMsg->event) );
    }

    switch ( pcMsg->event )    
    {
    // ��ʼ��ͨ��
    case EV_BAS_INI:
        ProcInitBas(pcMsg);
        break;
    case EV_BAS_QUIT:
        ProcStopBas();        
        break;

	case EV_BAS_SENDCHNSTATUS:
		SendChnNotif();
		break;

    // ��ʼ����
    case MCU_HDBAS_STARTADAPT_REQ:
        ProcStartAdptReq(cServMsg);
        break;        
        // ֹͣ����
    case MCU_HDBAS_STOPADAPT_REQ:
        ProcStopAdptReq(cServMsg);
        break;
        
        // �����������
    case MCU_HDBAS_SETADAPTPARAM_CMD:
        ProcChangeAdptCmd(cServMsg);
        break;
        
        // �����һ���ؼ�֡
    case MCU_BAS_FASTUPDATEPIC_CMD:
        ProcFastUpdatePicCmd();
        break;        	
        
        // �������ؼ�֡��ʱ��
    case EV_BAS_NEEDIFRAME_TIMER:
        ProcTimerNeedIFrame();
        break;        
        
        // ��ʾ����״̬    
     case EV_BAS_SHOW:
        StatusShow();
        break;

	 case EV_BAS_MODSENDADDR:
		 ProcModSndAddr();
		 break;

    default:
		ErrorLog("[Error]Unexcpet Message %u(%s).\n", pcMsg->event, ::OspEventDesc(pcMsg->event) );
        break;
    }
    return;
}

/*=============================================================================
  �� �� ���� DaemonProcPowerOn
  ��    �ܣ� ��ʼ������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
             CApp* pcApp
  �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2012/07/26  4.7         ��־��        ����
=============================================================================*/
void CMpu2BasInst::DaemonProcPowerOn( const CMessage* const pcMsg/*, CApp* pcApp*/ )
{
	NEXTSTATE((u32)DAEMON_IDLE);
	if ( NULL == pcMsg)
	{
		ErrorLog("[DaemonProcPowerOn]Msg Is Null\n");
		printf("[DaemonProcPowerOn]Msg Is Null\n");
		return;
	}
	
    if ( pcMsg->length != sizeof(TMpu2Cfg) )
    {
		ErrorLog("[DaemonProcPowerOn] Msg length error\n");
		printf("[DaemonProcPowerOn] Msg length error\n");
		return; 
    }
	
	//BAS�������ݱ���
	g_cMpu2BasApp.SetBasData( (TMpu2Cfg*)pcMsg->content );
	
	//BAS����ֵ��ȡ
    g_cMpu2BasApp.ReadDebugValues();    
	
	//socket��ʼ��
    u16 wRet = KdvSocketStartup();
   
	if ( MEDIANET_NO_ERROR != wRet )
    {
        ErrorLog("[DaemonProcPowerOn]KdvSocketStartup failed, error: %d\n", wRet );
        printf( "[DaemonProcPowerOn]KdvSocketStartup failed, error: %d\n", wRet );
        return ;
    }

	u8 byEqpNum = 1;
	if ( g_cMpu2BasApp.GetBasNum() == MAX_BAS_NUM &&
		 (g_cMpu2BasApp.GetBasWorkMode()!= TYPE_MPUBAS_H || g_cMpu2BasApp.GetBasWorkMode()!= TYPE_MPUBAS) &&
		 g_cMpu2BasApp.GetBasWorkMode() == TYPE_MPU2BAS_BASIC
	   )
	{
		byEqpNum = MAX_BAS_NUM;
	}

	for ( u8 byLoop =0; byLoop<byEqpNum; byLoop++ )
	{
		//ý�س�ʼ��
		wRet = g_cMpu2BasApp.InitVideoAdapter(byLoop);
		if ( (u16)Codec_Success != wRet )
		{
			ErrorLog( "[DaemonProcPowerOn]InitVideoAdapter byBasIdx:%d  errno: %d \n", byLoop,wRet );
			printf( "[DaemonProcPowerOn]InitVideoAdapter  byBasIdx:%d  errno: %d \n", byLoop,wRet );
			return ;
		}
		else
		{
			KeyLog("[DaemonProcPowerOn]InitVideoAdapter success byBasIdx:%d\n",byLoop);
			printf( "[DaemonProcPowerOn]InitVideoAdapter success byBasIdx:%d\n",byLoop);
		}
	}

	//ƽ�����͹����������̵߳����ȼ�
#ifdef _LINUX_
	
	int nRet = SsInit();
	if ( nRet == 0 )
	{
		ErrorLog("SsInit failed!\n");
	}
	
#endif
	//��ʼ��ת�Ƶ�ÿ��ͨ��
// #ifdef _LINUX_
// 	if (TYPE_MPU2BAS_ENHANCED == g_cMpu2BasApp.GetBasWorkMode())
// 	{
// 		//todo ý�ؽӿ���Ӧ
// 		wRet = HardMPUInit( (u32)INITMODE_HDBAS );
// 	}
// 	else if(TYPE_MPU2BAS_BASIC == g_cMpu2BasApp.GetBasWorkMode())
// 	{
// 		//todo ý�ؽӿ���Ӧ
// 		wRet = HardMPUInit( (u32)INITMODE_HD2BAS );
// 	}
// 	else if (TYPE_MPUBAS == g_cMpu2BasApp.GetBasWorkMode())
// 	{
// 		wRet = HardMPUInit( (u32)INITMODE_HDBAS );
// 	}
// 	else if(TYPE_MPUBAS_H == g_cMpu2BasApp.GetBasWorkMode())
// 	{
// 		wRet = HardMPUInit( (u32)INITMODE_HD2BAS );
// 	}
// 	else
// 	{ 
// 		ErrorLog( "[DaemonProcPowerOn] unexpected Mpu WorkMode: %d\n", g_cMpu2BasApp.GetBasWorkMode() );
// 		printf( "[DaemonProcPowerOn] unexpected Mpu WorkMode:%d!\n", g_cMpu2BasApp.GetBasWorkMode() );
// 	}
//     
//     if ( (u16)Codec_Success != wRet )
//     {
//         ErrorLog("[DaemonProcPowerOn]HardMPUInit failed, error: %d\n", wRet );
//         printf( "[DaemonProcPowerOn]HardMPUInit failed, error: %d\n", wRet );
//         return ;
//     }
// #endif	  
    
    // ��ʼ��MCUע��   
	//����ʹ�õ�һ�����������е�IPע�ᣬ��Ϊ��һ������ʱ��Ȼ���ڵ�
	TMpu2BasCfg tBasCfg;
	if (!g_cMpu2BasApp.GetMpu2BasCfg(0, tBasCfg))
	{
		ErrorLog("[DaemonProcPowerOn]GetMpu2BasCfg failed,idx:0!\n");
		return;
	}

	//������������MPC ��ʱ��
    if( 0 != tBasCfg.m_tCfg.dwConnectIP )
    {
        SetTimer( EV_BAS_CONNECT, MPU_CONNETC_TIMEOUT );     //3s���ٽ���
    }
	//�������ӱ���MPC ��ʱ��
    if( 0 != tBasCfg.m_tCfg.dwConnectIpB )
    {
        SetTimer( EV_BAS_CONNECTB, MPU_CONNETC_TIMEOUT );    //3s���ٽ���
    }
	
	NEXTSTATE((u32)DAEMON_INIT);
	return;
}

/*=============================================================================
  �� �� ���� DaemonProcConnectTimeOut
  ��    �ܣ� ����ע�ᳬʱ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� BOOL32 bIsConnectA
  �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2012/07/26  4.7         ��־��        ����
=============================================================================*/
void CMpu2BasInst::DaemonProcConnectTimeOut( BOOL32 bIsConnectA )
{
    BOOL32 bRet = FALSE;
	u32 dwNode = INVALID_NODE;
    if( TRUE == bIsConnectA )
    {
		dwNode = g_cMpu2BasApp.GetMcuANode();
        bRet = ConnectMcu( bIsConnectA, dwNode);
        if ( TRUE == bRet )
        {  
			g_cMpu2BasApp.SetMcuANode(dwNode);
			//SetTimer's third param means the idx of BAS TEqp
            SetTimer( EV_BAS_REGISTER, MPU_REGISTER_TIMEOUT, 0);
			
			//if exsit second bas,set secondbas register timer.
			if (g_cMpu2BasApp.GetBasNum() == MAX_BAS_NUM &&
				(g_cMpu2BasApp.GetBasWorkMode()!= TYPE_MPUBAS_H || g_cMpu2BasApp.GetBasWorkMode()!= TYPE_MPUBAS)
				)
			{
				SetTimer( EV_SECOND_BAS_REGISTER, MPU_SECOND_REGISTER_TIMEOUT, 1);
			}
        }
        else
        {
            SetTimer( EV_BAS_CONNECT, MPU_CONNETC_TIMEOUT );   //3s���ٽ���
        }
    }
    else
    {
		dwNode = g_cMpu2BasApp.GetMcuBNode();
        bRet = ConnectMcu( bIsConnectA, dwNode );
        if ( TRUE == bRet )
        { 
			g_cMpu2BasApp.SetMcuBNode(dwNode);
			//SetTimer,the third param means the idx of BAS TEqp
            SetTimer( EV_BAS_REGISTERB, MPU_REGISTER_TIMEOUT, 0);
			
			//if exsit second bas,set secondbas register timer.
			if (g_cMpu2BasApp.GetBasNum() == MAX_BAS_NUM &&
				(g_cMpu2BasApp.GetBasWorkMode()!= TYPE_MPUBAS_H || g_cMpu2BasApp.GetBasWorkMode()!= TYPE_MPUBAS)
			   )
			{
				SetTimer( EV_SECOND_BAS_REGISTERB, MPU_SECOND_REGISTER_TIMEOUT, 1);
			}
        }
        else
        {
            SetTimer( EV_BAS_CONNECTB, MPU_CONNETC_TIMEOUT );   //3s���ٽ���
        }
    }
    return;
}

/*====================================================================
	����  : ConnectMcu
	����  : ��MCU����
	����  : ��
	���  : ��
	����  : ��
	ע    :
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2012/07/26  4.7         ��־��         ����
====================================================================*/
BOOL32 CMpu2BasInst::ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode )
{
    BOOL32 bRet = TRUE;
    if( !OspIsValidTcpNode(dwMcuNode) ) // ���˲��Ϸ��ڵ�
    {
#ifdef _8KI_
		TMpu2BasCfg tTempBasCfg;
		if ( g_cMpu2BasApp.GetMpu2BasCfg(0,tTempBasCfg) )
		{
			dwMcuNode = OspConnectTcpNode(htonl(tTempBasCfg.m_tCfg.dwConnectIP), tTempBasCfg.m_tCfg.wConnectPort, 10, 0, 100 );  
		}
#else
		if(TRUE == bIsConnectA)
        {
            dwMcuNode = BrdGetDstMcuNode();  
        }
        else
        {  
            dwMcuNode = BrdGetDstMcuNodeB(); 
        }
#endif
	    if ( ::OspIsValidTcpNode(dwMcuNode) )
	    {
			KeyLog("[ConnectMcu]Connect Mcu %s Success, node: %d!\n", 
                   bIsConnectA ? "A" : "B", dwMcuNode);
//			printf("[ConnectMcu]Connect Mcu %s Success, node: %d!\n", 
//                  bIsConnectA ? "A" : "B", dwMcuNode);
	   
		    ::OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), GetInsID());// ������Ϣ����
	    }
	    else 
	    {
			KeyLog("[ConnectMcu]Connect to Mcu failed, will retry\n");
//			printf("[ConnectMcu]Connect Mcu %s failed, node: %d\n", 
//                   bIsConnectA ? "A" : "B", dwMcuNode);
            bRet = FALSE;
	    }
    }
    return bRet;
}

/*=============================================================================
  �� �� ���� DaemonProcRegisterTimeOut
  ��    �ܣ� ����ע�ᳬʱ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� BOOL32 bIsRegiterA
  �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2012/07/26  4.7         ��־��         ����
=============================================================================*/
void CMpu2BasInst::DaemonProcRegisterTimeOut( CMessage* const pMsg, BOOL32 bIsRegiterA )
{
	if ( NULL == pMsg)
	{
		ErrorLog("[DaemonProcRegisterTimeOut]pMsg is Null\n");
		return;
	}

	u8 byIdx = *(u8*)pMsg->content;
    if( TRUE == bIsRegiterA )
    {
        Register(byIdx, g_cMpu2BasApp.GetMcuANode() );
		
		if ( byIdx == 0 ) //ʹ�õ�һ������ע��
		{
			SetTimer( EV_BAS_REGISTER, MPU_REGISTER_TIMEOUT, 0 ); 
		}
		else if( byIdx == 1 ) //ʹ�õڶ�������ע��
		{
			SetTimer( EV_SECOND_BAS_REGISTER, MPU_SECOND_REGISTER_TIMEOUT, 1); 
		}
       
    }
    else
    {
        Register(byIdx, g_cMpu2BasApp.GetMcuBNode() );

		if ( byIdx == 0 ) //ʹ�õ�һ������ע��
		{
			SetTimer( EV_BAS_REGISTERB, MPU_REGISTER_TIMEOUT, 0 ); 
		}
		else if( byIdx == 1 ) //ʹ�õڶ�������ע��
		{
			SetTimer( EV_SECOND_BAS_REGISTERB, MPU_SECOND_REGISTER_TIMEOUT, 1); 
		}
    }
    return;
}

/*==================================================================
	����  : Register
	����  : ��MCUע��
	����  : ��
	���  : ��
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2012/07/26  4.7         ��־��        ����
====================================================================*/
void CMpu2BasInst::Register( u8 byIdx, u32 dwMcuNode )
{
#ifdef  _8KI_
	CServMsg cSvrMsg;
	TEqpRegReq tReg;
	memset(&tReg, 0, sizeof(tReg));
	TMpu2BasCfg tTempBasCfg;
	if ( g_cMpu2BasApp.GetMpu2BasCfg(0,tTempBasCfg) )
	{
		
		tReg.SetEqpIpAddr( tTempBasCfg.m_tCfg.dwLocalIP );
		tReg.SetEqpType(tTempBasCfg.m_tCfg.byEqpType);
		tReg.SetEqpAlias(tTempBasCfg.m_tCfg.achAlias);
		tReg.SetVersion(DEVVER_BAS);
		tReg.SetMcuId( (u8)tTempBasCfg.m_tCfg.wMcuId );
		
		cSvrMsg.SetMsgBody((u8*)&tReg, sizeof(tReg));
		
		post( MAKEIID( AID_MCU_PERIEQPSSN, CInstance::DAEMON ), BAS_MCU_REG_REQ,
             cSvrMsg.GetServMsg(), cSvrMsg.GetServMsgLen(), g_cMpu2BasApp.GetMcuANode());
	}
	else
	{
		ErrorLog("[Register]GetMpu2BasCfg error\n");
	}
#else
    CServMsg       cServMsg;
    TPeriEqpRegReq tReg;
	
	TMpu2BasCfg tBasCfg;
	if ( !g_cMpu2BasApp.GetMpu2BasCfg(byIdx, tBasCfg) )
	{
		ErrorLog("[Register]GetMpu2BasCfg failed,idx:%d!\n", byIdx);
		return;
	}
	
	tReg.SetMcuEqp( (u8)tBasCfg.m_tCfg.wMcuId, tBasCfg.m_tCfg.byEqpId, tBasCfg.m_tCfg.byEqpType );
	tReg.SetPeriEqpIpAddr( htonl(tBasCfg.m_tCfg.dwLocalIP) );
	if ( TYPE_MPUBAS == g_cMpu2BasApp.GetBasWorkMode() ||
		TYPE_MPUBAS_H == g_cMpu2BasApp.GetBasWorkMode()
		)
	{
		tReg.SetVersion( DEVVER_MPU );
	}
	else
	{
		tReg.SetVersion( DEVVER_MPU2 ); 
	}
	
	tReg.SetStartPort( tBasCfg.m_tCfg.wRcvStartPort );
	tReg.SetEqpAlias( tBasCfg.m_tCfg.achAlias );
	tReg.SetHDEqp( TRUE );
    cServMsg.SetMsgBody( (u8*)&tReg, sizeof(tReg) );
	
    post( MAKEIID(AID_MCU_PERIEQPSSN, tBasCfg.m_tCfg.byEqpId),
		BAS_MCU_REG_REQ,
		cServMsg.GetServMsg(),
		cServMsg.GetServMsgLen(),
		dwMcuNode );
#endif
	KeyLog("[Register]send BAS_MCU_REG_REQ to mcu:%d\n", dwMcuNode);
	return;
}

/*====================================================================
	����  : DaemonProcMcuRegAck 
	����  : MCU 8KIӦ��ע����Ϣ������
	����  : ��
	���  : ��
	����  : ��
	ע    :
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2011/12/06  4.7         ��־��        ����
====================================================================*/
void CMpu2BasInst::DaemonProc8KIMcuRegAck( const CMessage* const pMsg )
{
	if ( CurState() == (u32)DAEMON_IDLE
		)
	{
		ErrorLog( "[RegAck]CurState:%d is error\n", CurState());
		return;
	}
	
	if( NULL == pMsg )
    {
        ErrorLog( "[RegAck]pMsg is Null\n" );
        return;
    }
	u8 byInitChnlNum = 0;
	if (!GetBasInitChnlNumAcd2WorkMode(g_cMpu2BasApp.GetBasWorkMode(), byInitChnlNum))
	{
		ErrorLog( "[RegAck] GetBasInitChnlNumAcd2WorkMode:%d failed!\n",g_cMpu2BasApp.GetBasWorkMode() );
		return;
	}
	
	TMpu2BasCfg tMpu2BasCfg;
	//NodeУ��
	u8			byBasIdx = 0XFF;	//��¼BAS�����������е�idxֵ

	byBasIdx = 0;
	if ( !g_cMpu2BasApp.GetMpu2BasCfg(byBasIdx,tMpu2BasCfg) )
	{
		ErrorLog("[DaemonProcMcuRegAck]GetMpu2BasCfg error!\n");
		return;
	}
	
	NEXTSTATE((u32)DAEMON_IDLE);
	KillTimer(EV_BAS_REGISTER);
    KeyLog("bas register to mcu succeed !\n");
    
	CServMsg		cServMsg(pMsg->content, pMsg->length);
    TEqpRegAck		t8kiRegAck ;
	TPrsTimeSpan	t8kiPrsTimeSpan;
	u16				w8kiMTUSize;
	
	t8kiRegAck= *(TEqpRegAck*)cServMsg.GetMsgBody();
	t8kiPrsTimeSpan = *(TPrsTimeSpan*)(cServMsg.GetMsgBody() + sizeof(TEqpRegAck));
	w8kiMTUSize = *(u16*)( cServMsg.GetMsgBody() + sizeof(TEqpRegAck) + sizeof(TPrsTimeSpan) );
	w8kiMTUSize = ntohs( w8kiMTUSize );
	
    tMpu2BasCfg.m_dwMcuIId = pMsg->srcid;
    tMpu2BasCfg.m_byRegAckNum++;
	g_cMpu2BasApp.SetMpu2BasCfg(byBasIdx,tMpu2BasCfg);
	if( FIRST_REGACK == tMpu2BasCfg.m_byRegAckNum ) // ��һ���յ�ע��ɹ���Ϣ
    {
		tMpu2BasCfg.m_dwMcuRcvIp	   = t8kiRegAck.GetMcuIpAddr();
		tMpu2BasCfg.m_wMcuRcvStartPort = t8kiRegAck.GetMcuStartPort();
		tMpu2BasCfg.m_tCfg.wRcvStartPort = t8kiRegAck.GetEqpStartPort();
		g_cMpu2BasApp.SetMTUSize(w8kiMTUSize);
		
		tMpu2BasCfg.m_tPrsTimeSpan.m_wFirstTimeSpan = ntohs(t8kiPrsTimeSpan.m_wFirstTimeSpan);
		tMpu2BasCfg.m_tPrsTimeSpan.m_wSecondTimeSpan = ntohs(t8kiPrsTimeSpan.m_wSecondTimeSpan);
		tMpu2BasCfg.m_tPrsTimeSpan.m_wThirdTimeSpan = ntohs(t8kiPrsTimeSpan.m_wThirdTimeSpan);
		tMpu2BasCfg.m_tPrsTimeSpan.m_wRejectTimeSpan = ntohs(t8kiPrsTimeSpan.m_wRejectTimeSpan);
        DetailLog("[RegAck] The Bas Prs span: first: %d, second: %d, three: %d, reject: %d\n", 
			tMpu2BasCfg.m_tPrsTimeSpan.m_wFirstTimeSpan,
			tMpu2BasCfg.m_tPrsTimeSpan.m_wSecondTimeSpan,
			tMpu2BasCfg.m_tPrsTimeSpan.m_wThirdTimeSpan,
			tMpu2BasCfg.m_tPrsTimeSpan.m_wRejectTimeSpan );
		
		tMpu2BasCfg.m_tCfg.byEqpId = t8kiRegAck.GetEqpId();
		tMpu2BasCfg.m_tEqp.SetMcuEqp( (u8)tMpu2BasCfg.m_tCfg.wMcuId, tMpu2BasCfg.m_tCfg.byEqpId, tMpu2BasCfg.m_tCfg.byEqpType );	
		
		//����tMpu2BasCfg ��������
		g_cMpu2BasApp.SetMpu2BasCfg(byBasIdx,tMpu2BasCfg);
	}

	//BAS״̬�ϱ�,��Ϊmcu��������Ҫ��workmodeͨ�������Ϣ��ת����
	TPeriEqpStatus tEqpStatus;
    tEqpStatus.SetMcuEqp( (u8)tMpu2BasCfg.m_tCfg.wMcuId, tMpu2BasCfg.m_tCfg.byEqpId, tMpu2BasCfg.m_tCfg.byEqpType);
    tEqpStatus.m_byOnline = 1;
    tEqpStatus.SetAlias( tMpu2BasCfg.m_tCfg.achAlias );
	if (TYPE_MPUBAS == g_cMpu2BasApp.GetBasWorkMode())
	{
		tEqpStatus.m_tStatus.tHdBas.SetEqpType(TYPE_MPU);
		tEqpStatus.m_tStatus.tHdBas.tStatus.tSpeBas.m_byType = TYPE_MPU ;
	}
	else if (TYPE_MPUBAS_H == g_cMpu2BasApp.GetBasWorkMode())
	{
		tEqpStatus.m_tStatus.tHdBas.SetEqpType(TYPE_MPU_H);
		tEqpStatus.m_tStatus.tHdBas.tStatus.tSpeBas.m_byType = TYPE_MPU_H ;
	}
	else if (TYPE_MPU2BAS_BASIC == g_cMpu2BasApp.GetBasWorkMode())
	{
		tEqpStatus.m_tStatus.tHdBas.SetEqpType(TYPE_MPU2_BASIC);
	}
	else if (TYPE_MPU2BAS_ENHANCED == g_cMpu2BasApp.GetBasWorkMode())
	{
		tEqpStatus.m_tStatus.tHdBas.SetEqpType(TYPE_MPU2_ENHANCED);
	}
	else
	{
		ErrorLog("[DaemonProcMcuRegAck] unexpected mpu workmode:%d!\n", g_cMpu2BasApp.GetBasWorkMode());
		return;
	}
    
    cServMsg.SetMsgBody( (u8*)&tEqpStatus, sizeof(tEqpStatus) );
	
	SendMsgToMcu( BAS_MCU_BASSTATUS_NOTIF, cServMsg );
	
	//��ͨ����ʼ��
	if( FIRST_REGACK == tMpu2BasCfg.m_byRegAckNum )
	{
		u8 byIdx = 0;
		for ( ; byIdx < byInitChnlNum; byIdx++)
		{
			post( MAKEIID(GetAppID(), byIdx+1), EV_BAS_INI, (void*)&byBasIdx, sizeof(u8) );
		}
	}

	//ͨ��״̬�ϱ�
	for (u8 byChnlIdx = 1; byChnlIdx <= byInitChnlNum; byChnlIdx++ )
    {
		post( MAKEIID(GetAppID(), byChnlIdx+byBasIdx*byInitChnlNum), EV_BAS_SENDCHNSTATUS);
    }
	
	NEXTSTATE((u32)DAEMON_NORMAL);
    return;
}
/*====================================================================
	����  : DaemonProcMcuRegAck 
	����  : MCUӦ��ע����Ϣ������
	����  : ��
	���  : ��
	����  : ��
	ע    :
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2011/12/06  4.7         ��־��        ����
====================================================================*/
void CMpu2BasInst::DaemonProcMcuRegAck( const CMessage* const pMsg )
{
	if ( CurState() == (u32)DAEMON_IDLE
		)
	{
		ErrorLog( "[RegAck]CurState:%d is error\n", CurState());
		return;
	}

	if( NULL == pMsg )
    {
        ErrorLog( "[RegAck]pMsg is Null\n" );
        return;
    }
	u8 byInitChnlNum = 0;
	if (!GetBasInitChnlNumAcd2WorkMode(g_cMpu2BasApp.GetBasWorkMode(), byInitChnlNum))
	{
		ErrorLog( "[RegAck] GetBasInitChnlNumAcd2WorkMode:%d failed!\n",g_cMpu2BasApp.GetBasWorkMode() );
		return;
	}

	TMpu2BasCfg tMpu2BasCfg;
	//NodeУ��
	u8			byBasIdx = 0XFF;	//��¼BAS�����������е�idxֵ
	//�����Ϣ������
	TPeriEqpRegAck   tRegAck;			//RegACK�ṹ
	TPrsTimeSpan	 tPrsTimeSpan;		//�����ش�����
	u16				 wMTUSize;			//mtuֵ
	TEqpBasHDCfgInfo tBasConfig;		//BAS����

	CServMsg cServMsg( pMsg->content, pMsg->length );
	
	u8* pMsgBody = cServMsg.GetMsgBody();
	tRegAck = *(TPeriEqpRegAck*)pMsgBody;
	KeyLog( "[RegAck] ����ת����Ip:%s!\n", StrOfIP(tRegAck.GetMcuIpAddr()) );

	pMsgBody += sizeof(TPeriEqpRegAck);
	tPrsTimeSpan = *(TPrsTimeSpan*)pMsgBody;
	
	pMsgBody += sizeof(TPrsTimeSpan);
	wMTUSize = *(u16*)pMsgBody;
	wMTUSize = ntohs( wMTUSize );

	pMsgBody += sizeof(u16);
	tBasConfig = *(TEqpBasHDCfgInfo*)pMsgBody;

	

    if( pMsg->srcnode == g_cMpu2BasApp.GetMcuANode() ) 
    {
		for ( u8 byIdx = 0 ; byIdx<g_cMpu2BasApp.GetBasNum(); byIdx++)
		{
			if ( !g_cMpu2BasApp.GetMpu2BasCfg(byIdx,tMpu2BasCfg) )
			{
				ErrorLog("[RegAck]GetMpu2BasCfg idx:%d failed!\n",byIdx);
				continue;
			}

			if ( TYPE_MPUBAS_H == g_cMpu2BasApp.GetBasWorkMode() ||
				TYPE_MPUBAS == g_cMpu2BasApp.GetBasWorkMode() 
				)
			{
				KeyLog("[RegAck]simulating mpu\n");
			}
			else
			{
				if ( tMpu2BasCfg.m_tCfg.byEqpId != tBasConfig.GetEqpId())
				{
					ErrorLog("[RegAck]byEqpId:%d is different from byEqpId:%d!\n",tMpu2BasCfg.m_tCfg.byEqpId,tBasConfig.GetEqpId());
					continue;
				}
			}

					
			tMpu2BasCfg.m_dwMcuIId = pMsg->srcid;
			tMpu2BasCfg.m_byRegAckNum++;
			byBasIdx = byIdx;

			if (byIdx == 0)
			{
				KillTimer(EV_BAS_REGISTER);
			}
			else 
			{
				KillTimer(EV_SECOND_BAS_REGISTER);
			}
			g_cMpu2BasApp.SetMpu2BasCfg(byBasIdx,tMpu2BasCfg);
			//�ҵ���Ӧ���������ú󣬾��Ƴ�ѭ��
			break;
		}
    }
    else if ( pMsg->srcnode == g_cMpu2BasApp.GetMcuBNode() )
    {
		for ( u8 byIdx = 0 ; byIdx<g_cMpu2BasApp.GetBasNum(); byIdx++)
		{
			if ( !g_cMpu2BasApp.GetMpu2BasCfg(byIdx,tMpu2BasCfg) )
			{
				ErrorLog("[RegAck]GetMpu2BasCfg idx:%d failed!\n",byIdx);
				continue;
			}
			
			if ( TYPE_MPUBAS_H == g_cMpu2BasApp.GetBasWorkMode() ||
				TYPE_MPUBAS == g_cMpu2BasApp.GetBasWorkMode() 
				)
			{
				KeyLog("[RegAck]simulating mpu\n");
			}
			else
			{
				if ( tMpu2BasCfg.m_tCfg.byEqpId != tBasConfig.GetEqpId())
				{
					ErrorLog("[RegAck]byEqpId:%d is different from byEqpId:%d!\n",tMpu2BasCfg.m_tCfg.byEqpId,tBasConfig.GetEqpId());
					continue;
				}
			}


			tMpu2BasCfg.m_dwMcuIIdB = pMsg->srcid;
			tMpu2BasCfg.m_byRegAckNum++;
			byBasIdx = byIdx;

			if (byIdx == 0)
			{
				KillTimer(EV_BAS_REGISTERB);
			}
			else 
			{
				KillTimer(EV_SECOND_BAS_REGISTERB);
			}
			g_cMpu2BasApp.SetMpu2BasCfg(byBasIdx,tMpu2BasCfg);
			//�ҵ���Ӧ���������ú󣬾��Ƴ�ѭ��
			break;
		}
    }
	else
	{
		ErrorLog("[RegAck]pMsg->srcnode:%d is not correct node!\n");
		return;
	}

	if ( byBasIdx == 0XFF)
	{
		ErrorLog("[RegAck]can't find bas!\n");
		return;
	}

	//ssrcУ��
	if ( tMpu2BasCfg.m_dwSSrc == 0 )
    {
        tMpu2BasCfg.m_dwSSrc = tRegAck.GetMSSsrc();
    }
    else
    {
        // �쳣������Ͽ������ڵ�
        if ( tMpu2BasCfg.m_dwSSrc != tRegAck.GetMSSsrc() )
        {
            ErrorLog("[RegAck] MPC SSRC ERROR(%u<-->%u), Disconnect Both Nodes!\n", 
				tMpu2BasCfg.m_dwSSrc, tRegAck.GetMSSsrc() );
            if ( OspIsValidTcpNode(g_cMpu2BasApp.GetMcuANode()) )
            {
                OspDisconnectTcpNode(g_cMpu2BasApp.GetMcuANode());
            }
            if ( OspIsValidTcpNode(g_cMpu2BasApp.GetMcuBNode()) )
            {
                OspDisconnectTcpNode(g_cMpu2BasApp.GetMcuBNode());
            } 
			NEXTSTATE((u32)DAEMON_IDLE);
            return;
        }
    }


	if( FIRST_REGACK == tMpu2BasCfg.m_byRegAckNum ) // ��һ���յ�ע��ɹ���Ϣ
    {
		tMpu2BasCfg.m_dwMcuRcvIp	   = tRegAck.GetMcuIpAddr();
		tMpu2BasCfg.m_wMcuRcvStartPort = tRegAck.GetMcuStartPort();
		
        // ��������ʱû�л�ȡ�������ȡ����һ��MCU�������ȡ��ȥ����
        u32 dwOtherMcuIp = htonl( tRegAck.GetAnotherMpcIp() );
        if ( tMpu2BasCfg.m_tCfg.dwConnectIpB == 0 && dwOtherMcuIp != 0 )
        {
            tMpu2BasCfg.m_tCfg.dwConnectIpB = dwOtherMcuIp;
            SetTimer( EV_BAS_CONNECTB, MPU_CONNETC_TIMEOUT );
			
            ErrorLog("[RegAck] Found another Mpc IP: %s, try connecting...\n", 
				StrOfIP(dwOtherMcuIp) );
        }
		
		memcpy(tMpu2BasCfg.m_tCfg.achAlias, tBasConfig.GetAlias(), MAXLEN_EQP_ALIAS);
        KeyLog( "[RegAck] Local Recv Start Port.%u, Mcu Start Port.%u, Alias.%s\n",
			tMpu2BasCfg.m_tCfg.wRcvStartPort, tMpu2BasCfg.m_wMcuRcvStartPort , tMpu2BasCfg.m_tCfg.achAlias );
		
        tMpu2BasCfg.m_tPrsTimeSpan.m_wFirstTimeSpan = ntohs(tPrsTimeSpan.m_wFirstTimeSpan);
		tMpu2BasCfg.m_tPrsTimeSpan.m_wSecondTimeSpan = ntohs(tPrsTimeSpan.m_wSecondTimeSpan);
		tMpu2BasCfg.m_tPrsTimeSpan.m_wThirdTimeSpan = ntohs(tPrsTimeSpan.m_wThirdTimeSpan);
		tMpu2BasCfg.m_tPrsTimeSpan.m_wRejectTimeSpan = ntohs(tPrsTimeSpan.m_wRejectTimeSpan);
        DetailLog("[RegAck] The Bas Prs span: first: %d, second: %d, three: %d, reject: %d\n", 
			tMpu2BasCfg.m_tPrsTimeSpan.m_wFirstTimeSpan,
			tMpu2BasCfg.m_tPrsTimeSpan.m_wSecondTimeSpan,
			tMpu2BasCfg.m_tPrsTimeSpan.m_wThirdTimeSpan,
			tMpu2BasCfg.m_tPrsTimeSpan.m_wRejectTimeSpan );
		
		g_cMpu2BasApp.SetMTUSize(wMTUSize);
		DetailLog( "[RegAck]The Network MTU is : %d\n",wMTUSize );
		
        // zw 20081208 �ڵ�һ��ע��ɹ�֮�󣬽�ȷ�Ϻ��m_tCfg�е���Ϣ��m_tCfg����PowerOnʱ������������ģ�ת����m_tEqp��
        tMpu2BasCfg.m_tEqp.SetMcuEqp( (u8)tMpu2BasCfg.m_tCfg.wMcuId, tMpu2BasCfg.m_tCfg.byEqpId, tMpu2BasCfg.m_tCfg.byEqpType );	
		
		//����tMpu2BasCfg ��������
		g_cMpu2BasApp.SetMpu2BasCfg(byBasIdx,tMpu2BasCfg);
    }
	else
	{
		KeyLog("[RegAck]This time is :%d regack,basIdx:%d\n",tMpu2BasCfg.m_byRegAckNum,byBasIdx);
	}


	//BAS״̬�ϱ�,��Ϊmcu��������Ҫ��workmodeͨ�������Ϣ��ת����
	TPeriEqpStatus tEqpStatus;
    tEqpStatus.SetMcuEqp( (u8)tMpu2BasCfg.m_tCfg.wMcuId, tMpu2BasCfg.m_tCfg.byEqpId, tMpu2BasCfg.m_tCfg.byEqpType);
    tEqpStatus.m_byOnline = 1;
    tEqpStatus.SetAlias( tMpu2BasCfg.m_tCfg.achAlias );
	if (TYPE_MPUBAS == g_cMpu2BasApp.GetBasWorkMode())
	{
		tEqpStatus.m_tStatus.tHdBas.SetEqpType(TYPE_MPU);
		tEqpStatus.m_tStatus.tHdBas.tStatus.tSpeBas.m_byType = TYPE_MPU ;
	}
	else if (TYPE_MPUBAS_H == g_cMpu2BasApp.GetBasWorkMode())
	{
		tEqpStatus.m_tStatus.tHdBas.SetEqpType(TYPE_MPU_H);
		tEqpStatus.m_tStatus.tHdBas.tStatus.tSpeBas.m_byType = TYPE_MPU_H ;
	}
	else if (TYPE_MPU2BAS_BASIC == g_cMpu2BasApp.GetBasWorkMode())
	{
		tEqpStatus.m_tStatus.tHdBas.SetEqpType(TYPE_MPU2_BASIC);
	}
	else if (TYPE_MPU2BAS_ENHANCED == g_cMpu2BasApp.GetBasWorkMode())
	{
		tEqpStatus.m_tStatus.tHdBas.SetEqpType(TYPE_MPU2_ENHANCED);
	}
	else
	{
		ErrorLog("[DaemonProcMcuRegAck] unexpected mpu workmode:%d!\n", g_cMpu2BasApp.GetBasWorkMode());
		return;
	}
    
    cServMsg.SetMsgBody( (u8*)&tEqpStatus, sizeof(tEqpStatus) );
	
	SendMsgToMcu( BAS_MCU_BASSTATUS_NOTIF, cServMsg );

	//ͨ����ʼ��
	if( FIRST_REGACK == tMpu2BasCfg.m_byRegAckNum )
	{
		u8 byLoop;
		for ( byLoop = 1; byLoop <= byInitChnlNum; byLoop++ )
		{
			post( MAKEIID(GetAppID(), byLoop+byBasIdx*byInitChnlNum), EV_BAS_INI, (void*)&byBasIdx, sizeof(u8) );
		}
	}

	//ͨ��״̬�ϱ�
	for (u8 byChnlIdx = 1; byChnlIdx <= byInitChnlNum; byChnlIdx++ )
    {
		post( MAKEIID(GetAppID(), byChnlIdx+byBasIdx*byInitChnlNum), EV_BAS_SENDCHNSTATUS);
    }

	NEXTSTATE((u32)DAEMON_NORMAL);
    return;
}

/*====================================================================
	����  : DaemonProcMcuRegNack
	����  : MCU �ܾ�ע����Ϣ������(Ŀǰ�Ĳ��Բ��Ǳ��ܺ����ע��)
	����  : ��
	���  : ��
	����  : ��
	ע    :
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2012/07/26  4.7         ��־��        ����
====================================================================*/
void CMpu2BasInst::DaemonProcMcuRegNack( CMessage* const pMsg ) const
{
	CServMsg    cServMsg(pMsg->content,pMsg->length);
	TPeriEqpRegReq tReg;
	u16 wMsgBodyLen = cServMsg.GetMsgBodyLen();
	memcpy(&tReg, cServMsg.GetMsgBody(), min(sizeof(TPeriEqpRegReq), wMsgBodyLen));
    if ( pMsg->srcnode == g_cMpu2BasApp.GetMcuANode() )    
    {
        WarningLog("Bas(eqpid:%d) register be refused by A.\n",tReg.GetEqpId());
    }

    if ( pMsg->srcnode == g_cMpu2BasApp.GetMcuBNode() )    
    {
        WarningLog("Bas(eqpid:%d) register be refused by B.\n",tReg.GetEqpId() );
    }
    return;
}

/*=============================================================================
  �� �� ���� DeamonProcGetMsStatus
  ��    �ܣ� ����ȡ��������״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2012/07/26  4.7         ��־��         ����
=============================================================================*/
void CMpu2BasInst::DeamonProcGetMsStatusRsp( CMessage* const pMsg )
{
	BOOL bSwitchOk = FALSE;
	CServMsg cServMsg( pMsg->content, pMsg->length );
	if( MCU_EQP_GETMSSTATUS_ACK == pMsg->event )
	{
		TMcuMsStatus *ptMsStatus = (TMcuMsStatus*)cServMsg.GetMsgBody();
        
        KillTimer( EV_BAS_GETMSSTATUS );
        DetailLog("[DeamonProcGetMsStatus]. receive msg MCU_EQP_GETMSSTATUS_ACK. IsMsSwitchOK :%d\n", 
                ptMsStatus->IsMsSwitchOK() );

        if ( ptMsStatus->IsMsSwitchOK() ) // �����ɹ�
        {
            bSwitchOk = TRUE;
        }
	}

	// ����ʧ�ܻ��߳�ʱ
	if ( !bSwitchOk )
	{
		//20110914 zjl ��ʱ������״̬ �ٶ��� ������
		ClearInstStatus();
		DetailLog("[DeamonProcGetMsStatusRsp] ClearCurrentInst!\n");

		if ( OspIsValidTcpNode( g_cMpu2BasApp.GetMcuANode() ) )
		{
			DetailLog("[DeamonProcGetMsStatusRsp] OspDisconnectTcpNode A!\n");
			OspDisconnectTcpNode( g_cMpu2BasApp.GetMcuANode() );
		}

		if ( OspIsValidTcpNode( g_cMpu2BasApp.GetMcuBNode() ) )
		{
			DetailLog("[DeamonProcGetMsStatusRsp] OspDisconnectTcpNode B!\n");
			OspDisconnectTcpNode( g_cMpu2BasApp.GetMcuBNode() );
		}

		if ( INVALID_NODE == g_cMpu2BasApp.GetMcuANode() )// �п��ܲ�����������Connect�������
		{
			DetailLog("[DeamonProcGetMsStatusRsp] EV_BAS_CONNECT!\n");
			SetTimer( EV_BAS_CONNECT, MPU_CONNETC_TIMEOUT );
		}
		if ( INVALID_NODE == g_cMpu2BasApp.GetMcuBNode() )
		{
			DetailLog("[DeamonProcGetMsStatusRsp] EV_BAS_CONNECTB!\n");
			SetTimer( EV_BAS_CONNECTB, MPU_CONNETC_TIMEOUT );
		}
	}
	return;
}

/*====================================================================
	����  : DaemonProcOspDisconnect
	����  : Osp������Ϣ������
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2012/07/26  4.7         ��־��          ����
====================================================================*/
void CMpu2BasInst::DaemonProcOspDisconnect( CMessage* const pMsg/*, CApp* pcApp*/ )
{
    if ( NULL == pMsg )  
    {
        ErrorLog("[DaemonProcOspDisconnect] message's pointer is Null\n" );
        return;
    }

    u32 dwNode = *(u32*)pMsg->content;

    if ( INVALID_NODE != dwNode )
    {
        OspDisconnectTcpNode( dwNode );
    } 

	KeyLog("[DaemonProcOspDisconnect]dwNode:%d Disconnect mcuA:%d,mcuB:%d\n",
		   dwNode,g_cMpu2BasApp.GetMcuANode(), g_cMpu2BasApp.GetMcuBNode() );

    if( dwNode == g_cMpu2BasApp.GetMcuANode() ) // ����
    {
        KeyLog("[DaemonProcOspDisconnect] McuNode.A disconnect\n" );
        g_cMpu2BasApp.FreeStatusDataA();
        SetTimer( EV_BAS_CONNECT, MPU_CONNETC_TIMEOUT );
    }
    else if ( dwNode == g_cMpu2BasApp.GetMcuBNode() )
    {
        KeyLog("[DaemonProcOspDisconnect] McuNode.B disconnect\n" );
        g_cMpu2BasApp.FreeStatusDataB();
        SetTimer( EV_BAS_CONNECTB, MPU_CONNETC_TIMEOUT );
    }

	// ������һ����������Mcuȡ��������״̬���ж��Ƿ�ɹ�
	if ( INVALID_NODE != g_cMpu2BasApp.GetMcuANode() || INVALID_NODE != g_cMpu2BasApp.GetMcuBNode() )
	{
		if ( OspIsValidTcpNode(g_cMpu2BasApp.GetMcuANode()) )
		{
			for ( u8 byIdx = 0; byIdx < g_cMpu2BasApp.GetBasNum(); byIdx++)
			{
				TMpu2BasCfg tMpu2BasCfg;
				if ( !g_cMpu2BasApp.GetMpu2BasCfg(byIdx,tMpu2BasCfg) )
				{
					ErrorLog("[DaemonProcOspDisconnect]GetMpu2BasCfg idx:%d failed!\n",byIdx);
					continue;
				}
				post( tMpu2BasCfg.m_dwMcuIId, EQP_MCU_GETMSSTATUS_REQ, NULL, 0,g_cMpu2BasApp.GetMcuANode() );    
				KeyLog("[DaemonProcOspDisconnect] GetMsStatusReq. McuNode.A EqpId:%d\n", tMpu2BasCfg.m_tEqp.GetEqpId());
			}
		}
		else if ( OspIsValidTcpNode( g_cMpu2BasApp.GetMcuBNode() ) )
		{
			for ( u8 byIdx = 0; byIdx < g_cMpu2BasApp.GetBasNum(); byIdx++)
			{
				TMpu2BasCfg tMpu2BasCfg;
				if ( !g_cMpu2BasApp.GetMpu2BasCfg(byIdx,tMpu2BasCfg) )
				{
					ErrorLog("[DaemonProcOspDisconnect]GetMpu2BasCfg idx:%d failed!\n",byIdx);
					continue;
				}
				post( tMpu2BasCfg.m_dwMcuIIdB, EQP_MCU_GETMSSTATUS_REQ, NULL, 0,g_cMpu2BasApp.GetMcuBNode() );    
				KeyLog("[DaemonProcOspDisconnect] GetMsStatusReq. McuNode.A EqpId:%d\n", tMpu2BasCfg.m_tEqp.GetEqpId());
			}
		}

		SetTimer( EV_BAS_GETMSSTATUS, WAITING_MSSTATUS_TIMEOUT );
		return;
	}
	// �����ڵ㶼�ϣ����״̬��Ϣ
    if ( INVALID_NODE == g_cMpu2BasApp.GetMcuANode() && INVALID_NODE == g_cMpu2BasApp.GetMcuBNode() )
    {
        ClearInstStatus();
    }
    return;
}
/*====================================================================
	����  : DeamonProcModSendIpAddr
	����  : ����MCUǿ�������޸ķ���Ip��ַ
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/06  4.7         ��־��          ����
====================================================================*/
void CMpu2BasInst::DeamonProcModSendIpAddr( CMessage* const pMsg )
{
    if ( NULL == pMsg->content )
    {
        ErrorLog("[DeamonProcModSendIpAddr]msg is Null\n" );
        return;
    } 

	u32 dwSendIP = *(u32*)pMsg->content;
	u8	byIdx = 0;
	BOOL32 bIsFindBas = FALSE;
	for (byIdx = 0; byIdx < g_cMpu2BasApp.GetBasNum(); byIdx++)
	{
		TMpu2BasCfg tMpu2BasCfg;
		if ( !g_cMpu2BasApp.GetMpu2BasCfg(byIdx,tMpu2BasCfg) )
		{
			ErrorLog("[DeamonProcModSendIpAddr]GetMpu2BasCfg idx:%d failed!\n",byIdx);
			continue;
		}
		
		if ( (u16)pMsg->srcid == tMpu2BasCfg.m_tEqp.GetEqpId() )
		{
			bIsFindBas = TRUE;
			tMpu2BasCfg.m_dwMcuRcvIp = ntohl(dwSendIP);
			g_cMpu2BasApp.SetMpu2BasCfg(byIdx,tMpu2BasCfg);
			break;
		}
	}

	if ( TRUE == bIsFindBas )
	{
		u8 byInitNum = 0;
		if(!GetBasInitChnlNumAcd2WorkMode(g_cMpu2BasApp.GetBasWorkMode(), byInitNum))
		{
			ErrorLog("[DeamonProcModSendIpAddr] GetBasInitChnlNumAcd2WorkMode failed!\n");
			return;
		}
		
		for (u8 byLoop = 1; byLoop <= byInitNum; byLoop++ )
		{
			post( MAKEIID(GetAppID(), byLoop + byInitNum*byIdx), EV_BAS_MODSENDADDR);
		}	
	}
	else
	{
		ErrorLog("[DeamonProcModSendIpAddr] can't find bas for modify ipAddr!\n");
		return;
	}

}

/*====================================================================
	����  : DeamonShowBasDebugInfo
	����  : ��ʾBAS��debug������Ϣ
	����  : 
	���  : ��
	����  : ��
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2010/11/30  4.6         ��־��        ����
====================================================================*/
void CMpu2BasInst::DeamonShowBasDebugInfo()
{
	TDebugVal tDebugVal = g_cMpu2BasApp.GetDebugVal();
	tDebugVal.Print();
	OspPrintf(TRUE,FALSE,"[DeamonShowBasDebugInfo]BasCfgData like follows:\n");
	g_cMpu2BasApp.ShowBasCfgData();
	return;
}

/*====================================================================
	����  : ProcInitBas
	����  : 
	����  : 
	���  : ��
	����  : ��
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2011/12/06  4.7         ��־��       ����
====================================================================*/
void CMpu2BasInst::ProcInitBas(CMessage* const pcMsg)
{
	//��������BAS idx�±�
	if (NULL == pcMsg)
	{
		ErrorLog("[ProcInitBas]pcMsg is NULL\n");
		return;
	}
	m_byBasIdx = *(u8*)pcMsg->content;
	TMpu2BasCfg tMpu2BasCfg;
	if ( !g_cMpu2BasApp.GetMpu2BasCfg(m_byBasIdx,tMpu2BasCfg) )
	{
//		printf("[DaemonProcRegAck] GetMpu2BasCfg idx:%d failed!", m_byBasIdx);
		ErrorLog("[DaemonProcRegAck] GetMpu2BasCfg idx:%d failed!\n", m_byBasIdx);	
		return;
	}
		
    if ( m_pAdpCroup == NULL )
    {    
		u8 byInitNum = 0;
		if(!GetBasInitChnlNumAcd2WorkMode(g_cMpu2BasApp.GetBasWorkMode(), byInitNum))
		{
//			printf("[DaemonProcRegAck] GetBasInitChnlNumAcd2WorkMode:%d failed!",g_cMpu2BasApp.GetBasWorkMode());
			ErrorLog("[DaemonProcRegAck] GetBasInitChnlNumAcd2WorkMode:%d failed!\n",g_cMpu2BasApp.GetBasWorkMode());
			return;
		}	
		
		//���chnid�����㿪ʼ��ʼ
		m_byBasChnIdx =  ( 0 == ( GetInsID() ) % byInitNum ) ? (GetInsID()- m_byBasIdx* byInitNum-1) : ( ( GetInsID() ) % byInitNum -1 );
		
		//���m_byChnType���ϱ�MCUʱʹ��
		if ( TYPE_MPUBAS == g_cMpu2BasApp.GetBasWorkMode() ||
			TYPE_MPUBAS_H == g_cMpu2BasApp.GetBasWorkMode()
			)
		{
			m_byChnType = HDBAS_CHNTYPE_MPU;
		}
		else
		{
			if ( m_byBasChnIdx == 0)
			{
				m_byChnType = MPU2_BAS_CHN_MV;
			}
			else if (m_byBasChnIdx == 1)
			{
				m_byChnType = MPU2_BAS_CHN_DS;
			}
			else
			{
				m_byChnType = MPU2_BAS_CHN_SEL;
			}
		}

		//���ͨ�����·��
		u8 byOutChnlNum = 0;
		if(!GetBasOutChnlNumAcd2WorkModeAndChnl(g_cMpu2BasApp.GetBasWorkMode(), m_byChnType, byOutChnlNum))
		{
//			printf("[DaemonProcRegAck] GetBasOutChnlNumAcd2WorkMode:%d failed!",g_cMpu2BasApp.GetBasWorkMode());
			ErrorLog("[DaemonProcRegAck] GetBasOutChnlNumAcd2WorkMode:%d failed!\n",g_cMpu2BasApp.GetBasWorkMode());
			return;
		}	
		
		//���BASͨ��������ƵRTP��������ʼ�˿� 
		u8 byOutFrontOutNum = 0;
		if ( !GetFrontOutNumAcd2WorckModeAndChnl(g_cMpu2BasApp.GetBasWorkMode(),m_byChnType, m_byBasChnIdx, byOutFrontOutNum) )
		{
//			printf("[DaemonProcRegAck] GetFrontNumAcd2WorckMode:%d-%d failed!\n",g_cMpu2BasApp.GetBasWorkMode(),m_byBasChnIdx );
			ErrorLog("[DaemonProcRegAck] GetFrontNumAcd2WorckMode:%d-%d failed!\n",g_cMpu2BasApp.GetBasWorkMode(),m_byBasChnIdx );
			return;
		}
		
		//BASͨ��������ƵRTP�����Ķ˿�
        u16 wBasRecvPort = tMpu2BasCfg.m_tCfg.wRcvStartPort + byOutFrontOutNum* PORTSPAN ;	

        u16 wDstStartPort = tMpu2BasCfg.m_wMcuRcvStartPort + PORTSPAN * byOutFrontOutNum;

//		printf("[ProcInitBas] basidx:%d instid:%d, Chn.%d , RcvPort:%u, DstPort:%u!\n", m_byBasIdx, GetInsID(),m_byBasChnIdx, wBasRecvPort, wDstStartPort);
		KeyLog("[ProcInitBas] basidx:%d instid:%d, Chn.%d , RcvPort:%u, DstPort:%u!\n", m_byBasIdx, GetInsID(), m_byBasChnIdx, wBasRecvPort, wDstStartPort);

		m_pAdpCroup = new CMpu2BasAdpGroup;
		if ( NULL == m_pAdpCroup )
		{
			ErrorLog("[ProcInitBas]new CMpu2BasAdpGroup failed!\n");
//			printf("[ProcInitBas]new CMpu2BasAdpGroup failed!\n");
			return;
		}

		if (  !m_pAdpCroup->Init( wBasRecvPort,wBasRecvPort+1, tMpu2BasCfg.m_dwMcuRcvIp, wDstStartPort, (u32)this,m_byBasChnIdx,byOutChnlNum,g_cMpu2BasApp.GetBasWorkMode(), m_byBasIdx)  )
		{
			g_cMpu2BasApp.DestroyVideoAdapter(m_byBasIdx);
			ErrorLog("[ProcInitBas]m_pAdpCroup->Init,instid:%d chnid:%d failed!\n", GetInsID(), m_byBasChnIdx);
//			printf("[ProcInitBas]m_pAdpCroup->Init,instid:%d chnid:%d failed!\n", GetInsID(), m_byBasChnIdx);
			return;
		 }
        
		// new frm queue
        m_ptFrmQueue = new TRtpPkQueue;

        if (NULL == m_ptFrmQueue)
        {
			ErrorLog("[ProcInitBas]new m_ptFrmQueue failed!\n");
//			printf("[ProcInitBas]new m_ptFrmQueue failed!\n");
			return; 
        }
		m_ptFrmQueue->Init(g_cMpu2BasApp.GetAudioDeferNum());

		//  [11/9/2009 pengjie] ���üӺڱ߲ñ�	
		DetailLog( "[ProcInitBas][SetResizeMode] Mode = %d \n", g_cMpu2BasApp.GetVidEncHWMode() );
		if( (u16)Codec_Success != m_pAdpCroup->SetResizeMode( (u32)g_cMpu2BasApp.GetVidEncHWMode() ) )
		{
			ErrorLog( "[ProcInitBas][SetResizeMode] Mode = %d Failed\n", g_cMpu2BasApp.GetVidEncHWMode());
//			printf( "[ProcInitBas][SetResizeMode] Mode = %d Failed\n", g_cMpu2BasApp.GetVidEncHWMode() );
		}
	}
	else
	{
		m_pAdpCroup->ModNetSndIpAddr(tMpu2BasCfg.m_dwMcuRcvIp);
	}

    NextState( (u32)READY );
    KeyLog("[ProcInitBas]set EV_BAS_NEEDIFRAME_TIMER : %d\n", g_cMpu2BasApp.GetIframeInterval());
    SetTimer( EV_BAS_NEEDIFRAME_TIMER, g_cMpu2BasApp.GetIframeInterval() );   // ��ʱ��ѯ�������Ƿ���Ҫ�ؼ�֡
    SendChnNotif();    
  
    return;
}

/*====================================================================
	����  : ProcStopAdptReq
	����  : ֹͣ����
	����  : 
	���  : ��
	����  : ��
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2012/07/26  4.7         ��־��        ����
====================================================================*/
void CMpu2BasInst::ProcStopBas()
{
    KillTimer( EV_BAS_NEEDIFRAME_TIMER );

    NextState( (u32)IDLE );
    //�������״̬
	memset(&m_tChnInfo.m_tChnStatus, 0, sizeof(THDBasVidChnStatus));
	//�ͷ��������
	if ( m_pAdpCroup != NULL )
	{
		delete m_pAdpCroup;
		m_pAdpCroup = NULL;
	}
	if ( m_ptFrmQueue != NULL )
	{
		delete m_ptFrmQueue;
		m_ptFrmQueue = NULL;
	}
    SendChnNotif();
}

/*====================================================================
	����  : SendChnNotif
	����  : ��MCU����ͨ��״̬֪ͨ
	����  : 
	���  : ��
	����  : �ɹ�����TRUE����FALSE
	ע    : 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2011/12/06  4.7         ��־��        ����
====================================================================*/
void CMpu2BasInst::SendChnNotif()
{
	TMpu2BasCfg tMpu2BasCfg;
	if ( !g_cMpu2BasApp.GetMpu2BasCfg(m_byBasIdx,tMpu2BasCfg) )
	{
//		printf("[SendChnNotif] GetMpu2BasCfg idx:%d failed!", m_byBasIdx);
		ErrorLog("[SendChnNotif] GetMpu2BasCfg idx:%d failed!\n", m_byBasIdx);	
		return;
	}

    CServMsg cServMsg;
	cServMsg.SetConfId( m_tChnInfo.m_cChnConfId );
  
    m_tChnInfo.m_tChnStatus.SetEqp( tMpu2BasCfg.m_tEqp );
    m_tChnInfo.m_tChnStatus.SetChnIdx( m_byBasChnIdx );
    m_tChnInfo.m_tChnStatus.SetStatus( (u8)CurState() );
    
    cServMsg.SetMsgBody((u8*)&m_byChnType, sizeof(u8));
    cServMsg.CatMsgBody((u8*)&m_tChnInfo.m_tChnStatus, sizeof(m_tChnInfo.m_tChnStatus));
    
    DetailLog( "[SendChnNotif] eqpid:%d, Inst.%d Channel.%u state.%u\n",
			   tMpu2BasCfg.m_tEqp.GetEqpId(),
               GetInsID(),
               m_byBasChnIdx,
               CurState() );
    
	SendMsgToMcu( HDBAS_MCU_CHNNLSTATUS_NOTIF, cServMsg );
    return;
}
/*====================================================================
	����  : SendMsgToMcu
	����  : ��MCU������Ϣ��ʹ����Ϊinstance
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2011/12/06  4.7         ��־��         ����
====================================================================*/
void CMpu2BasInst::SendMsgToMcu( u16 wEvent, CServMsg& cServMsg )
{
    if ( GetInsID() != CInstance::DAEMON )
    {
        cServMsg.SetChnIndex( m_byBasChnIdx );
    }
   
	TMpu2BasCfg tMpu2BasCfg;
	if ( !g_cMpu2BasApp.GetMpu2BasCfg(m_byBasIdx,tMpu2BasCfg) )
	{
//		printf("[SendMsgToMcu] GetMpu2BasCfg idx:%d failed!", m_byBasIdx);
		ErrorLog("[SendMsgToMcu] GetMpu2BasCfg idx:%d failed!\n", m_byBasIdx);	
		return;
	}

    if (OspIsValidTcpNode(g_cMpu2BasApp.GetMcuANode()) ) 
	{
		post( tMpu2BasCfg.m_dwMcuIId, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), g_cMpu2BasApp.GetMcuANode() );
		DetailLog("Send Message %u(%s) to Mcu A\n",wEvent, ::OspEventDesc(wEvent) );
//		printf( "Send Message %u(%s) to Mcu A\n",wEvent, ::OspEventDesc(wEvent) );
	}
	

    if (  OspIsValidTcpNode(g_cMpu2BasApp.GetMcuBNode()) )
    {
	    post( tMpu2BasCfg.m_dwMcuIIdB, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), g_cMpu2BasApp.GetMcuBNode() );
		DetailLog("Send Message %u(%s) to Mcu B\n",wEvent, ::OspEventDesc(wEvent) );
//		printf( "Send Message %u(%s) to Mcu B\n",wEvent, ::OspEventDesc(wEvent) );
    }
    

    return ;
}

/*====================================================================
    ����  : ProcStartAdptReq
    ����  : ��ʼ���䴦��
    ����  : 
    ���  : ��
    ����  : ��
    ע    : 
----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2011/12/07  4.7         ��־��       ����
====================================================================*/
void CMpu2BasInst::ProcStartAdptReq( CServMsg& cServMsg )
{   
    u8 byInstId = (u8)GetInsID();
    DetailLog("[ProcStartAdptReq] Start adp,InstId:%d---basidx:%d--ChnId%d\n", byInstId, m_byBasIdx, m_byBasChnIdx);
	TMpu2BasCfg tMpu2BasCfg;
	if ( !g_cMpu2BasApp.GetMpu2BasCfg(m_byBasIdx,tMpu2BasCfg) )
	{
		cServMsg.SetErrorCode( ERR_BAS_GETCFGFAILED );
		cServMsg.SetMsgBody( (u8*)&tMpu2BasCfg.m_tEqp, sizeof(tMpu2BasCfg.m_tEqp) );
		SendMsgToMcu( cServMsg.GetEventId() + 2, cServMsg);
		ErrorLog("[ProcStartAdptReq] GetMpu2BasCfg idx:%d failed!\n", m_byBasIdx);	
		return;
	}
	switch( CurState() )	
	{
	case READY:  //req��Ϣֻ��ready�ܹ���
		{		
			if( NULL == m_pAdpCroup )
			{
				cServMsg.SetErrorCode( ERR_BAS_OPMAP );
				cServMsg.SetMsgBody( (u8*)&tMpu2BasCfg.m_tEqp, sizeof(tMpu2BasCfg.m_tEqp) );
				SendMsgToMcu( cServMsg.GetEventId() + 2, cServMsg);
				ErrorLog("[ProcStartAdptReq] m_pAdpCroup is NULL, InstId:%d---ChnId%d\n", byInstId, m_byBasChnIdx);
				return;
			}
			
			
			u8 byOutPutNum = 0;
			if ( !GetBasOutChnlNumAcd2WorkModeAndChnl(g_cMpu2BasApp.GetBasWorkMode(),m_byChnType,byOutPutNum) )
			{
				cServMsg.SetErrorCode( ERR_BAS_GETCFGFAILED );
				cServMsg.SetMsgBody( (u8*)&tMpu2BasCfg.m_tEqp, sizeof(tMpu2BasCfg.m_tEqp) );
				SendMsgToMcu( cServMsg.GetEventId() + 2, cServMsg);
				ErrorLog("[ProcStartAdptReq] GetBasOutChnlNumAcd2WorkModeAndChnl idx:%d failed!\n", m_byBasIdx);	
				return;
			}
			
			
			THDAdaptParam  atParm[MAXNUM_BASOUTCHN];	//ȡmpu2 bas ͨ���������ĺ�
			u8 byOutIdx =0;
			for ( byOutIdx= 0; byOutIdx < byOutPutNum; byOutIdx++)
			{
				atParm[byOutIdx] = *(THDAdaptParam*)(cServMsg.GetMsgBody() + byOutIdx * sizeof(THDAdaptParam));
				if ( m_byChnType == HDBAS_CHNTYPE_MPU )
				{
					atParm[byOutIdx].SetProfileType(0);
				}
			}

			AdjustParamForMpuSimulating(atParm,byOutPutNum);

			THDAdaptParam  atSpecialParm[MAXNUM_BASOUTCHN];
			//MPU2ģ��MPUʱ��MPU�ڶ������ʹ��ԭʼ����
			if ( m_byChnType == HDBAS_CHNTYPE_MPU )
			{
				for ( byOutIdx = 0; byOutIdx < byOutPutNum; byOutIdx++)
				{
					atSpecialParm[byOutIdx] = atParm[byOutIdx];
				}
			}
			else
			{		
				for ( byOutIdx = 0; byOutIdx < byOutPutNum; byOutIdx++)
				{
					atSpecialParm[byOutIdx] = *(THDAdaptParam*)(cServMsg.GetMsgBody() + (byOutIdx+byOutPutNum) * sizeof(THDAdaptParam));
				}
			}

			
			for (u8 byIdx = 0; byIdx < byOutPutNum; byIdx++)
			{
				ConvertToRealFR(atParm[byIdx]);
				ConvertToRealFR(atSpecialParm[byIdx]);
				//�տ���ʱ���ڴ˴�ת���ֱ��ʣ����õײ㰴�������Դ�����ͨ������ת��
				//ConverToRealParam(m_byBasIdx,m_byBasChnIdx,byIdx,atParm[byIdx]);
			}
			
			//���ڸ������ʵ����ֱ�ConverToRealParam�ƺ����ʸ�����������Ҳ�ƺ���
// 			if ( g_cMpu2BasApp.IsEnableCheat())
// 			{
// 				for (u8 byOutId = 0; byOutId < byOutPutNum; byOutId++)
// 				{
// 					if (MEDIA_TYPE_NULL == atParm[byOutId].GetVidType() ||
// 						0 == atParm[byOutId].GetVidType())
// 					{  
// 						continue;
// 					}
// 					u16 wBitrate = atParm[byOutId].GetBitrate();
// 					wBitrate = g_cMpu2BasApp.GetDecRateDebug( wBitrate );
// 					atParm[byOutId].SetBitRate( wBitrate );
// 				}
// 			}
			
			TMediaEncrypt  tMediaEncryptVideo;
			TDoublePayload tDoublePayloadVideo;
			if ( m_byChnType == HDBAS_CHNTYPE_MPU )
			{
				tMediaEncryptVideo = *(TMediaEncrypt*)( cServMsg.GetMsgBody()  + byOutPutNum * sizeof(THDAdaptParam) );
				tDoublePayloadVideo= *(TDoublePayload*)( cServMsg.GetMsgBody() + byOutPutNum * sizeof(THDAdaptParam) + sizeof(TMediaEncrypt) );
			}
			else
			{
				tMediaEncryptVideo = *(TMediaEncrypt*)( cServMsg.GetMsgBody()  + (byOutPutNum*2) * sizeof(THDAdaptParam) );
				tDoublePayloadVideo= *(TDoublePayload*)( cServMsg.GetMsgBody() + (byOutPutNum*2) * sizeof(THDAdaptParam) + sizeof(TMediaEncrypt) );
			}
			
			u8 byAudDecType = MEDIA_TYPE_NULL;
			if ( m_byChnType == HDBAS_CHNTYPE_MPU )
			{
				if (cServMsg.GetMsgBodyLen() > sizeof(THDAdaptParam) * byOutPutNum + sizeof(TMediaEncrypt) + sizeof(TDoublePayload))
				{
					byAudDecType = *(cServMsg.GetMsgBody() + (byOutPutNum*2) * sizeof(THDAdaptParam) + sizeof(TMediaEncrypt)+ sizeof(TDoublePayload));
				}
			}
			else
			{
				if (cServMsg.GetMsgBodyLen() > sizeof(THDAdaptParam) * (byOutPutNum*2) + sizeof(TMediaEncrypt) + sizeof(TDoublePayload))
				{
					byAudDecType = *(cServMsg.GetMsgBody() + (byOutPutNum*2) * sizeof(THDAdaptParam) + sizeof(TMediaEncrypt)+ sizeof(TDoublePayload));
				}
			}


			BOOL32 bCreatSendObj = FALSE;
			TAdapterEncParam atVidEncPar[MAXNUM_BASOUTCHN];
			memset(atVidEncPar, 0, sizeof(TAdapterEncParam)*MAXNUM_BASOUTCHN);
			u8 byValidOutNum = 0;			//��Ч�ı������
			// fxh Ŀǰmcu���ñ�׼4CIF(704* 576), ������MPEG4,��ȡD1(720 * 576)
			for(u8 byOutChnlIdx = 0; byOutChnlIdx < byOutPutNum; byOutChnlIdx++)
			{
				if ( atParm[byOutChnlIdx].GetVidType() == MEDIA_TYPE_NULL ||
					 0 == atParm[byOutChnlIdx].GetVidType()
					)
				{
					continue;
				}
				byValidOutNum++;
				atVidEncPar[byOutChnlIdx].m_dwEncoderId = byOutChnlIdx;
				if (MEDIA_TYPE_MP4 == atParm[byOutChnlIdx].GetVidType() &&
					704 == atParm[byOutChnlIdx].GetWidth() &&
					576 == atParm[byOutChnlIdx].GetHeight())
				{
					atParm[byOutChnlIdx].SetResolution(720, 576);
				}

				
				if (MEDIA_TYPE_MP4 == atSpecialParm[byOutChnlIdx].GetVidType() &&
					704 == atSpecialParm[byOutChnlIdx].GetWidth() &&
					576 == atSpecialParm[byOutChnlIdx].GetHeight())
				{
					atSpecialParm[byOutChnlIdx].SetResolution(720, 576);
				}

			    g_cMpu2BasApp.GetDefaultParam( atParm[byOutChnlIdx].GetVidType(), atVidEncPar[byOutChnlIdx].m_tVideoEncParam );

				//���õ�һ·�������
				atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoHeight = atParm[byOutChnlIdx].GetHeight();
				atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoWidth  = atParm[byOutChnlIdx].GetWidth();
				atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wBitRate     = atParm[byOutChnlIdx].GetBitrate(); 
				atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_byFrameRate  = atParm[byOutChnlIdx].GetFrameRate();
				atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_dwProfile    = atParm[byOutChnlIdx].GetProfileType();

				//zbq[10/10/2008] ���ݷֱ��ʵ������ؼ�֡ ���
				if ( (atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoWidth == 1280 &&
					  atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoHeight == 720) ||
					  (atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoWidth == 1920 &&
					   atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoHeight == 1088) ||
					   (atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoWidth == 1920 &&
					   atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoHeight == 544)  ||
					   (atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoWidth == 1600 &&
					   atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoHeight == 1200)  ||
					   (atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoWidth == 1280 &&
					   atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoHeight == 1024)
					)
				{
					atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_dwMaxKeyFrameInterval = 3000;
				}
				else if( (atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoWidth == 704 &&
						  atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoHeight == 576) ||
						 (atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoWidth == 720 &&
						  atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoHeight == 576) )
				{
					atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_dwMaxKeyFrameInterval = 1000;
				}
				else if ( atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoWidth == 352 &&
						  atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoHeight == 288 )
				{
					atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_dwMaxKeyFrameInterval = 500;
				}

				//������������
				if (MEDIA_TYPE_H264 == atParm[byOutChnlIdx].GetVidType())
				{
					//3��HD�ĳ�20-45����ǰ2��HD CIF���ϸ�51��10
					if (atParm[byOutChnlIdx].GetHeight() > 288)
					{
						atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_byMaxQuant = 45;
						atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_byMinQuant = 20;
					}
					else
					{
						atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_byMaxQuant = 45;
						atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_byMinQuant = 20;
					}					
				}
				else
				{
					atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_byMaxQuant = 31;
					atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_byMinQuant = 3;
				}

				DetailLog( "Start Video Enc Param: %u, W*H: %dx%d, Bitrate.%dKbps, FrmRate.%d, MaxKeyFrmInt.%d, ProFileType:%d, MaxQuant:%d, MinQuant:%d\n",
								   atParm[byOutChnlIdx].GetVidType(), 
								   atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoWidth, 
								   atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoHeight,
								   atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wBitRate,
								   atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_byFrameRate,
								   atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_dwMaxKeyFrameInterval,
								   atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_dwProfile,
								   atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_byMaxQuant,
								   atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_byMinQuant
						 );
			}

			for (u8 byPriOut = 0; byPriOut < byOutPutNum;  byPriOut++)
			{
				if (atParm[byPriOut].GetVidType() != 
					m_tChnInfo.GetOutChnlVidTpye(byPriOut))
				{
					bCreatSendObj = TRUE;
					KeyLog("[ProcStartAdptReq] CreatSendObj again,BasIdx:%d,ChnIdx:%d,OutIdx:%d !\n",
							m_byBasIdx,m_byBasChnIdx,byPriOut);
					break;
				}
			}

			if ( TRUE == m_pAdpCroup->SetVideoEncParam(tDoublePayloadVideo.GetRealPayLoad(),atVidEncPar,atParm,atSpecialParm, byValidOutNum, bCreatSendObj))
			{
				// ��¼ͨ��״̬
				for (u8 byOut = 0; byOut < byOutPutNum;  byOut++)
				{
					THDAdaptParam* ptAdpParam = m_tChnInfo.GetBasOutPutVidParam(byOut);
					if (NULL != ptAdpParam)
					{
						ptAdpParam->SetAudType( atParm[byOut].GetAudType() );
						ptAdpParam->SetVidType( atParm[byOut].GetVidType() );
						ptAdpParam->SetBitRate( atParm[byOut].GetBitrate() );
						ptAdpParam->SetResolution( atParm[byOut].GetWidth(), atParm[byOut].GetHeight() );
						ptAdpParam->SetFrameRate( atParm[byOut].GetFrameRate() );
						ptAdpParam->SetProfileType( atParm[byOut].GetProfileType() );
					}
				}
			}

            //����ͬ����������
			if ( byAudDecType != MEDIA_TYPE_NULL  )
			{
				if(!m_pAdpCroup->SetAudioParam( byAudDecType) )
				{
					ErrorLog("SetAudioParam failed, BasIdx:%d,ChnIdx:%d!\n",m_byBasIdx,m_byBasChnIdx);
				}
			}

			DetailLog("ActivePayLoad:%u, RealPayLoad:%u!\n", tDoublePayloadVideo.GetActivePayload(), tDoublePayloadVideo.GetRealPayLoad());
           
			u8 byEncryptMode = tMediaEncryptVideo.GetEncryptMode();
			u8 abySndActPt[MAXNUM_BASOUTCHN];
			u8 byLoop = 0;
            if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
            {
                WarningLog("CONF_ENCRYPTMODE_NONE == byEncryptMode\n" );
				for(;byLoop < byOutPutNum; byLoop ++)
				{
					abySndActPt[byLoop] = atParm[byLoop].GetVidActiveType();
					if(atParm[byLoop].GetVidType() == MEDIA_TYPE_H263 || atParm[byLoop].GetVidType() == MEDIA_TYPE_MP4 )
					{
						abySndActPt[byLoop] = 0;
					}
					DetailLog("abySndActPt[%d] is: %u!\n", byLoop, abySndActPt[byLoop]);
				}
				//�ӿڵ�������ָ��abySndActPt����Ч���ͨ����byOutPutNum
                m_pAdpCroup->SetKeyAndPT( NULL, 0, 0,
										  abySndActPt, byOutPutNum,
                                          tDoublePayloadVideo.GetActivePayload(), 
										  tDoublePayloadVideo.GetRealPayLoad());
            }
            else if ( CONF_ENCRYPTMODE_DES == byEncryptMode || CONF_ENCRYPTMODE_AES == byEncryptMode )
            {
                u8 abyKeyBuf[MAXLEN_KEY];
                s32 byKenLen = 0;
                tMediaEncryptVideo.GetEncryptKey( abyKeyBuf, &byKenLen );
                if ( CONF_ENCRYPTMODE_DES == byEncryptMode )
                {
                    byEncryptMode = DES_ENCRYPT_MODE;
                }
                else
                {
                    byEncryptMode = AES_ENCRYPT_MODE;
                }
				//����SetKeyandPT�ӿڵ�����������ʱ��֯����
				memset(abySndActPt, 0, sizeof(abySndActPt));
				for (byLoop = 0; byLoop < byOutPutNum; byLoop++)
				{
					abySndActPt[byLoop] = atParm[byLoop].GetVidActiveType();
				}
                m_pAdpCroup->SetKeyAndPT( abyKeyBuf, byKenLen, byEncryptMode, 
										  abySndActPt, byOutPutNum,
                                          tDoublePayloadVideo.GetActivePayload(), 
										  tDoublePayloadVideo.GetRealPayLoad());
            }

            //����ConfId
			m_tChnInfo.m_cChnConfId = cServMsg.GetConfId();

			// �趪���ش�����		
			BOOL32 bIsNeedPrs = FALSE;
			for (u8 byEncIdx = 0; byEncIdx < byOutPutNum; byEncIdx++)
			{
				if (atParm[byEncIdx].IsNeedbyPrs())
				{
					bIsNeedPrs = TRUE;
					break;
				}
			}
			TRSParam tNetRSParam;
			KeyLog("[ProcStartAdptReq] IsNeedPrs:%d!\n", bIsNeedPrs);
            if ( bIsNeedPrs )
			{		
				DetailLog("[ProcStartAdptReq] Bas is needed by Prs!\n" );
                tNetRSParam.m_wFirstTimeSpan  = tMpu2BasCfg.m_tPrsTimeSpan.m_wFirstTimeSpan;
				tNetRSParam.m_wSecondTimeSpan = tMpu2BasCfg.m_tPrsTimeSpan.m_wSecondTimeSpan;
				tNetRSParam.m_wThirdTimeSpan  = tMpu2BasCfg.m_tPrsTimeSpan.m_wThirdTimeSpan;
				tNetRSParam.m_wRejectTimeSpan = tMpu2BasCfg.m_tPrsTimeSpan.m_wRejectTimeSpan;
				//zjl ͳһ�ӿ�SetNetSendFeedbackVideoParam�������з���ͨ�����ش�����
				if( !m_pAdpCroup->SetNetSendFeedbackVideoParam( 2000, byOutPutNum, TRUE ))
				{
					ErrorLog("[ProcStartAdptReq]SetNetSendFeedbackVideoParam failed\n ");
				}
				if ( !m_pAdpCroup->SetNetRecvFeedbackVideoParam( tNetRSParam, TRUE ) )
				{
					ErrorLog("[ProcStartAdptReq]SetNetRecvFeedbackVideoParam failed\n ");
				}
			}
			else
			{
				DetailLog("Bas is not needed by Prs!\n" );
                tNetRSParam.m_wFirstTimeSpan  = 0;
				tNetRSParam.m_wSecondTimeSpan = 0;
				tNetRSParam.m_wThirdTimeSpan  = 0;
				tNetRSParam.m_wRejectTimeSpan = 0;
				if (!m_pAdpCroup->SetNetSendFeedbackVideoParam( 2000, byOutPutNum, FALSE ))
				{
					ErrorLog("[ProcStartAdptReq]SetNetSendFeedbackVideoParam failed\n ");
				}
			
				if (!m_pAdpCroup->SetNetRecvFeedbackVideoParam( tNetRSParam, FALSE ))
				{
					ErrorLog("[ProcStartAdptReq]SetNetRecvFeedbackVideoParam failed\n ");

				}
			}

			if( !m_pAdpCroup->IsStart() )
			{
				BOOL bStartOk;
                bStartOk = m_pAdpCroup->StartAdapter(m_byBasIdx, TRUE );
				if ( !bStartOk )
				{
					cServMsg.SetErrorCode( ERR_BAS_OPMAP );
					cServMsg.SetMsgBody( (u8*)&tMpu2BasCfg.m_tEqp, sizeof(tMpu2BasCfg.m_tEqp) );
					SendMsgToMcu( cServMsg.GetEventId() + 2, cServMsg );
					ErrorLog("[ProcStartAdptReq]m_pAdpCroup->StartAdapter error!\n");
					return;
				}
				{
					KeyLog("[ProcStartAdptReq]m_pAdpCroup->StartAdapter ok!\n");
				}
			}

            // ƽ������
			m_pAdpCroup->SetSmoothSendRule( g_cMpu2BasApp.IsEnableSmoothSend() );
			
            cServMsg.SetMsgBody( (u8*)&tMpu2BasCfg.m_tEqp, sizeof(tMpu2BasCfg.m_tEqp) );
            SendMsgToMcu( cServMsg.GetEventId() + 1, cServMsg );
            
            NextState( (u32)RUNNING );            
            SendChnNotif();

			
			//[nizhijun 2011/03/21] BAS�����ı���Ҫ��1����ؼ�֡����
			KillTimer( EV_BAS_NEEDIFRAME_TIMER );
			mpulog(MPU_INFO, "[ProcStartAdptReq]reset EV_BAS_NEEDIFRAME_TIMER from %d to: %d\n", 
				g_cMpu2BasApp.GetIframeInterval(), CHECK_IFRAME_INTERVAL);
			m_bIsIframeFirstReq = FALSE;
			SetTimer( EV_BAS_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL );   // ��ʱ��ѯ�������Ƿ���Ҫ�ؼ�֡
		}
		break;
	default:
		cServMsg.SetErrorCode( ERR_BAS_CHNOTREAD );
		cServMsg.SetMsgBody( (u8*)&tMpu2BasCfg.m_tEqp, sizeof(tMpu2BasCfg.m_tEqp) );
		SendMsgToMcu( cServMsg.GetEventId() + 2, cServMsg );
		ErrorLog("recv startbasreq,but the InstanceId.%d--BasIdx.%d--ChnIdx.%d in wrong state:%d !\n", 
			    byInstId,m_byBasIdx,m_byBasChnIdx, CurState());
		break;
	}

    return;
}

/*====================================================================
	����  : ProcChangeAdptCmd
	����  : �����������
	����  : 
	���  : ��
	����  : ��
	ע��  : 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2012/07/26  4.7         ��־��         ����
====================================================================*/
void CMpu2BasInst::ProcChangeAdptCmd( CServMsg& cServMsg )
{
	u8 byInstId = (u8)GetInsID();
    DetailLog("[ProcChangeAdptCmd] Start adp,InstId:%d---basidx:%d--ChnId%d\n", byInstId, m_byBasIdx, m_byBasChnIdx);
	
	switch( CurState() )	
	{
	case RUNNING:  //cmd��Ϣֻ��running�ܹ���
		{		
			TMpu2BasCfg tMpu2BasCfg;
			if ( !g_cMpu2BasApp.GetMpu2BasCfg(m_byBasIdx,tMpu2BasCfg) )
			{
				ErrorLog("[ProcChangeAdptCmd] GetMpu2BasCfg idx:%d failed!\n", m_byBasIdx);	
				return;
			}
			
			if( NULL == m_pAdpCroup )
			{
				ErrorLog("[ProcChangeAdptCmd] m_pAdpCroup is NULL, InstId:%d---ChnId%d\n", byInstId, m_byBasChnIdx);
				return;
			}
			
			u8 byOutPutNum = 0;
			if ( !GetBasOutChnlNumAcd2WorkModeAndChnl(g_cMpu2BasApp.GetBasWorkMode(),m_byChnType,byOutPutNum) )
			{
				ErrorLog("[ProcChangeAdptCmd] GetBasOutChnlNumAcd2WorkModeAndChnl idx:%d failed!\n", m_byBasIdx);	
				return;
			}
				
			THDAdaptParam  atParm[MAXNUM_BASOUTCHN];	//ȡmpu2 bas ͨ���������ĺ�
			u8 byOutIdx =0;
			for ( byOutIdx = 0; byOutIdx < byOutPutNum; byOutIdx++)
			{
				atParm[byOutIdx] = *(THDAdaptParam*)(cServMsg.GetMsgBody() + byOutIdx * sizeof(THDAdaptParam));
				if ( m_byChnType == HDBAS_CHNTYPE_MPU )
				{
					atParm[byOutIdx].SetProfileType(0);
				}
			}

			AdjustParamForMpuSimulating(atParm,byOutPutNum);

			THDAdaptParam  atSpecialParm[MAXNUM_BASOUTCHN];
			//MPU2ģ��MPUʱ��MPU�ڶ������ʹ��ԭʼ����
			if ( m_byChnType == HDBAS_CHNTYPE_MPU )
			{
				for ( byOutIdx = 0; byOutIdx < byOutPutNum; byOutIdx++)
				{
					atSpecialParm[byOutIdx] = atParm[byOutIdx];
				}
			}
			else
			{		
				for ( byOutIdx = 0; byOutIdx < byOutPutNum; byOutIdx++)
				{
					atSpecialParm[byOutIdx] = *(THDAdaptParam*)(cServMsg.GetMsgBody() + (byOutIdx+byOutPutNum) * sizeof(THDAdaptParam));
				}
			}


			for (u8 byIdx = 0; byIdx < byOutPutNum; byIdx++)
			{
				ConvertToRealFR(atParm[byIdx]);
				ConvertToRealFR(atSpecialParm[byIdx]);
			}
					
			TMediaEncrypt  tMediaEncryptVideo;
			TDoublePayload tDoublePayloadVideo;
			if ( m_byChnType == HDBAS_CHNTYPE_MPU )
			{
				tMediaEncryptVideo = *(TMediaEncrypt*)( cServMsg.GetMsgBody()  + byOutPutNum * sizeof(THDAdaptParam) );
				tDoublePayloadVideo= *(TDoublePayload*)( cServMsg.GetMsgBody() + byOutPutNum * sizeof(THDAdaptParam) + sizeof(TMediaEncrypt) );
			}
			else
			{
				tMediaEncryptVideo = *(TMediaEncrypt*)( cServMsg.GetMsgBody()  + (byOutPutNum*2) * sizeof(THDAdaptParam) );
				tDoublePayloadVideo= *(TDoublePayload*)( cServMsg.GetMsgBody() + (byOutPutNum*2) * sizeof(THDAdaptParam) + sizeof(TMediaEncrypt) );
			}
			
			u8 byAudDecType = MEDIA_TYPE_NULL;
			if ( m_byChnType == HDBAS_CHNTYPE_MPU )
			{
				if (cServMsg.GetMsgBodyLen() > sizeof(THDAdaptParam) * byOutPutNum + sizeof(TMediaEncrypt) + sizeof(TDoublePayload))
				{
					byAudDecType = *(cServMsg.GetMsgBody() + (byOutPutNum*2) * sizeof(THDAdaptParam) + sizeof(TMediaEncrypt)+ sizeof(TDoublePayload));
				}
			}
			else
			{
				if (cServMsg.GetMsgBodyLen() > sizeof(THDAdaptParam) * (byOutPutNum*2) + sizeof(TMediaEncrypt) + sizeof(TDoublePayload))
				{
					byAudDecType = *(cServMsg.GetMsgBody() + (byOutPutNum*2) * sizeof(THDAdaptParam) + sizeof(TMediaEncrypt)+ sizeof(TDoublePayload));
				}
			}


			BOOL32 bCreatSendObj = FALSE;
			TAdapterEncParam atVidEncPar[MAXNUM_BASOUTCHN];
			memset(atVidEncPar, 0, sizeof(TAdapterEncParam)*MAXNUM_BASOUTCHN);
			u8 byValidOutNum = 0;			//��Ч�ı������
			// fxh Ŀǰmcu���ñ�׼4CIF(704* 576), ������MPEG4,��ȡD1(720 * 576)
			for(u8 byOutChnlIdx = 0; byOutChnlIdx < byOutPutNum; byOutChnlIdx++)
			{
				if ( atParm[byOutChnlIdx].GetVidType() == MEDIA_TYPE_NULL ||
					0 == atParm[byOutChnlIdx].GetVidType()
					)
				{
					continue;
				}
				
				byValidOutNum++;

				atVidEncPar[byOutChnlIdx].m_dwEncoderId = byOutChnlIdx;
				if (MEDIA_TYPE_MP4 == atParm[byOutChnlIdx].GetVidType() &&
					704 == atParm[byOutChnlIdx].GetWidth() &&
					576 == atParm[byOutChnlIdx].GetHeight())
				{
					atParm[byOutChnlIdx].SetResolution(720, 576);
				}

				if (MEDIA_TYPE_MP4 == atSpecialParm[byOutChnlIdx].GetVidType() &&
					704 == atSpecialParm[byOutChnlIdx].GetWidth() &&
					576 == atSpecialParm[byOutChnlIdx].GetHeight())
				{
					atSpecialParm[byOutChnlIdx].SetResolution(720, 576);
				}

			    g_cMpu2BasApp.GetDefaultParam( atParm[byOutChnlIdx].GetVidType(), atVidEncPar[byOutChnlIdx].m_tVideoEncParam );

				//���õ�һ·�������
				atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoHeight = atParm[byOutChnlIdx].GetHeight();
				atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoWidth  = atParm[byOutChnlIdx].GetWidth();
				atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wBitRate     = atParm[byOutChnlIdx].GetBitrate(); 
				atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_byFrameRate  = atParm[byOutChnlIdx].GetFrameRate();
				atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_dwProfile	 = atParm[byOutChnlIdx].GetProfileType();

				//zbq[10/10/2008] ���ݷֱ��ʵ������ؼ�֡ ���
				if ( (atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoWidth == 1280 &&
					  atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoHeight == 720) ||
					  (atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoWidth == 1920 &&
					   atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoHeight == 1088) ||
					   (atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoWidth == 1920 &&
					   atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoHeight == 544)  ||
					   (atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoWidth == 1600 &&
					   atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoHeight == 1200)  ||
					   (atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoWidth == 1280 &&
					   atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoHeight == 1024)
					   )
				{
					atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_dwMaxKeyFrameInterval = 3000;
				}
				else if( (atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoWidth == 704 &&
						  atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoHeight == 576) ||
						 (atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoWidth == 720 &&
						  atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoHeight == 576) )
				{
					atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_dwMaxKeyFrameInterval = 1000;
				}
				else if ( atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoWidth == 352 &&
						  atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoHeight == 288 )
				{
					atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_dwMaxKeyFrameInterval = 500;
				}

				//������������
				if (MEDIA_TYPE_H264 == atParm[byOutChnlIdx].GetVidType())
				{
					//CIF���ϸ�51��10
					if (atParm[byOutChnlIdx].GetHeight() > 288)
					{
						atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_byMaxQuant = 45;
						atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_byMinQuant = 20;
					}
					else
					{
						atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_byMaxQuant = 45;
						atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_byMinQuant = 20;
					}					
				}
				else
				{
					atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_byMaxQuant = 31;
					atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_byMinQuant = 3;
				}

				DetailLog( "Start Video Enc Param: %u, W*H: %dx%d, Bitrate.%dKbps, FrmRate.%d, MaxKeyFrmInt.%d, ProFileType:%d, MaxQuant:%d, MinQuant:%d\n",
									atParm[byOutChnlIdx].GetVidType(), 
									atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoWidth, 
									atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wVideoHeight,
									atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_wBitRate,
									atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_byFrameRate,
									atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_dwMaxKeyFrameInterval,
									atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_dwProfile,
									atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_byMaxQuant,
									atVidEncPar[byOutChnlIdx].m_tVideoEncParam.m_byMinQuant
						 );
			}

			for (u8 byPriOut = 0; byPriOut < byOutPutNum;  byPriOut++)
			{
				if (atParm[byPriOut].GetVidType() != 
					m_tChnInfo.GetOutChnlVidTpye(byPriOut))
				{
					bCreatSendObj = TRUE;
					KeyLog("[ProcChangeAdptCmd] CreatSendObj again,BasIdx:%d,ChnIdx:%d,OutIdx:%d !\n",
							m_byBasIdx,m_byBasChnIdx,byPriOut);
					break;
				}
			}

			if ( TRUE == m_pAdpCroup->SetVideoEncParam(tDoublePayloadVideo.GetRealPayLoad(), atVidEncPar,atParm,atSpecialParm, byValidOutNum, bCreatSendObj,TRUE))
			{
				// ��¼ͨ��״̬
				for (u8 byOut = 0; byOut < byOutPutNum;  byOut++)
				{
					THDAdaptParam* ptAdpParam = m_tChnInfo.GetBasOutPutVidParam(byOut);
					if (NULL != ptAdpParam)
					{
						ptAdpParam->SetAudType( atParm[byOut].GetAudType() );
						ptAdpParam->SetVidType( atParm[byOut].GetVidType() );
						ptAdpParam->SetBitRate( atParm[byOut].GetBitrate() );
						ptAdpParam->SetResolution( atParm[byOut].GetWidth(), atParm[byOut].GetHeight() );
						ptAdpParam->SetFrameRate( atParm[byOut].GetFrameRate() );
						ptAdpParam->SetProfileType( atParm[byOut].GetProfileType() );
					}
				}
			}

            //����ͬ����������
			if ( byAudDecType != MEDIA_TYPE_NULL  )
			{
				if(!m_pAdpCroup->SetAudioParam( byAudDecType) )
				{
					ErrorLog("[ProcChangeAdptCmd]SetAudioParam failed, BasIdx:%d,ChnIdx:%d!\n",m_byBasIdx,m_byBasChnIdx);
				}
			}

			DetailLog("ActivePayLoad:%u, RealPayLoad:%u!\n", tDoublePayloadVideo.GetActivePayload(), tDoublePayloadVideo.GetRealPayLoad());
           
			u8 byEncryptMode = tMediaEncryptVideo.GetEncryptMode();
			u8 abySndActPt[MAXNUM_BASOUTCHN];
			u8 byLoop = 0;
            if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
            {
                WarningLog("[ProcChangeAdptCmd]CONF_ENCRYPTMODE_NONE == byEncryptMode\n" );
				for(;byLoop < byOutPutNum; byLoop ++)
				{
					abySndActPt[byLoop] = atParm[byLoop].GetVidActiveType();
					if(atParm[byLoop].GetVidType() == MEDIA_TYPE_H263 || atParm[byLoop].GetVidType() == MEDIA_TYPE_MP4 )
					{
						abySndActPt[byLoop] = 0;
					}
					DetailLog("[ProcChangeAdptCmd]abySndActPt[%d] is: %u!\n", byLoop, abySndActPt[byLoop]);
				}
				//�ӿڵ�������ָ��abySndActPt����Ч���ͨ����byOutPutNum
                m_pAdpCroup->SetKeyAndPT( NULL, 0, 0,
										  abySndActPt, byOutPutNum,
                                          tDoublePayloadVideo.GetActivePayload(), 
										  tDoublePayloadVideo.GetRealPayLoad());
            }
            else if ( CONF_ENCRYPTMODE_DES == byEncryptMode || CONF_ENCRYPTMODE_AES == byEncryptMode )
            {
                u8 abyKeyBuf[MAXLEN_KEY];
                s32 byKenLen = 0;
                tMediaEncryptVideo.GetEncryptKey( abyKeyBuf, &byKenLen );
                if ( CONF_ENCRYPTMODE_DES == byEncryptMode )
                {
                    byEncryptMode = DES_ENCRYPT_MODE;
                }
                else
                {
                    byEncryptMode = AES_ENCRYPT_MODE;
                }
				//����SetKeyandPT�ӿڵ�����������ʱ��֯����
				memset(abySndActPt, 0, sizeof(abySndActPt));
				for (byLoop = 0; byLoop < byOutPutNum; byLoop++)
				{
					abySndActPt[byLoop] = atParm[byLoop].GetVidActiveType();
				}
                m_pAdpCroup->SetKeyAndPT( abyKeyBuf, byKenLen, byEncryptMode, 
										  abySndActPt, byOutPutNum,
                                          tDoublePayloadVideo.GetActivePayload(), 
										  tDoublePayloadVideo.GetRealPayLoad());
            }

            //����ConfId
			m_tChnInfo.m_cChnConfId = cServMsg.GetConfId();

			// �趪���ش�����		
			BOOL32 bIsNeedPrs = FALSE;
			for (u8 byEncIdx = 0; byEncIdx < byOutPutNum; byEncIdx++)
			{
				if (atParm[byEncIdx].IsNeedbyPrs())
				{
					bIsNeedPrs = TRUE;
					break;
				}
			}
			TRSParam tNetRSParam;
			KeyLog("[ProcChangeAdptCmd] IsNeedPrs:%d!\n", bIsNeedPrs);
            if ( bIsNeedPrs )
			{		
				DetailLog("[ProcChangeAdptCmd] Bas is needed by Prs!\n" );
                tNetRSParam.m_wFirstTimeSpan  = tMpu2BasCfg.m_tPrsTimeSpan.m_wFirstTimeSpan;
				tNetRSParam.m_wSecondTimeSpan = tMpu2BasCfg.m_tPrsTimeSpan.m_wSecondTimeSpan;
				tNetRSParam.m_wThirdTimeSpan  = tMpu2BasCfg.m_tPrsTimeSpan.m_wThirdTimeSpan;
				tNetRSParam.m_wRejectTimeSpan = tMpu2BasCfg.m_tPrsTimeSpan.m_wRejectTimeSpan;
				//zjl ͳһ�ӿ�SetNetSendFeedbackVideoParam�������з���ͨ�����ش�����
				if( !m_pAdpCroup->SetNetSendFeedbackVideoParam( 2000, byOutPutNum, TRUE ))
				{
					ErrorLog("[ProcChangeAdptCmd]SetNetSendFeedbackVideoParam failed\n ");
				}
				if ( !m_pAdpCroup->SetNetRecvFeedbackVideoParam( tNetRSParam, TRUE ) )
				{
					ErrorLog("[ProcChangeAdptCmd]SetNetRecvFeedbackVideoParam failed\n ");
				}
			}
			else
			{
				DetailLog("[ProcChangeAdptCmd]Bas is not needed by Prs!\n" );
                tNetRSParam.m_wFirstTimeSpan  = 0;
				tNetRSParam.m_wSecondTimeSpan = 0;
				tNetRSParam.m_wThirdTimeSpan  = 0;
				tNetRSParam.m_wRejectTimeSpan = 0;
				if (!m_pAdpCroup->SetNetSendFeedbackVideoParam( 2000, byOutPutNum, FALSE ))
				{
					ErrorLog("[ProcChangeAdptCmd]SetNetSendFeedbackVideoParam failed\n ");
				}
			
				if (!m_pAdpCroup->SetNetRecvFeedbackVideoParam( tNetRSParam, FALSE ))
				{
					ErrorLog("[ProcChangeAdptCmd]SetNetRecvFeedbackVideoParam failed\n ");

				}
			}

			if( !m_pAdpCroup->IsStart() )
			{
				BOOL bStartOk;
                bStartOk = m_pAdpCroup->StartAdapter( m_byBasIdx,TRUE );
				if ( !bStartOk )
				{
					ErrorLog("[ProcChangeAdptCmd]StartAdapter error!\n");
					return;
				}
			}
			m_pAdpCroup->SetSmoothSendRule( g_cMpu2BasApp.IsEnableSmoothSend() );
	
            NextState( (u32)RUNNING );            
			//change����Ҫ��״̬�ϸ棬��Ϊ�Ѿ���running
			//SendChnNotif();

			//[nizhijun 2011/03/21] BAS�����ı���Ҫ��1����ؼ�֡����
			KillTimer( EV_BAS_NEEDIFRAME_TIMER );
			mpulog(MPU_INFO, "[ProcChangeAdptCmd]reset EV_BAS_NEEDIFRAME_TIMER from %d to: %d\n", 
				  g_cMpu2BasApp.GetIframeInterval(), CHECK_IFRAME_INTERVAL);
			m_bIsIframeFirstReq = FALSE;
			SetTimer( EV_BAS_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL );   // ��ʱ��ѯ�������Ƿ���Ҫ�ؼ�֡
		}
		break;
	default:
		ErrorLog("[ProcChangeAdptCmd] recv ChangeAdptCmd,but the InstanceId.%d--BasIdx.%d--ChnIdx.%d in wrong state:%d !\n", 
			    byInstId,m_byBasIdx,m_byBasChnIdx, CurState());
		break;
	}

    return;
}

/*====================================================================
	����  : ProcStopAdptReq
	����  : ֹͣ����
	����  : 
	���  : ��
	����  : ��
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2012/07/26  4.7         ��־��         ����
====================================================================*/
void CMpu2BasInst::ProcStopAdptReq( CServMsg& cServMsg )
{
    u8 byChnIdx = (u8)GetInsID() - 1;
    KeyLog("[ProcStopAdptReq] Stop adp, instid:%d, basidx:%d, chnidx:%d,CurState:%d\n", 
			   byChnIdx, m_byBasIdx, m_byBasChnIdx,CurState());

    CConfId    m_cTmpConfId = cServMsg.GetConfId();
	if ( !(m_tChnInfo.m_cChnConfId == m_cTmpConfId) )
	{
		cServMsg.SetErrorCode( ERR_BAS_ERRCONFID );
		SendMsgToMcu( cServMsg.GetEventId() + 2, cServMsg );
		ErrorLog("[ProcStopAdptReq]  instid:%d, basidx:%d, Channel.%d unmatch conf id in stop adpt req\n", 
				 byChnIdx, m_byBasIdx, m_byBasChnIdx );
//		printf("[ProcStopAdptReq]  instid:%d, basidx:%d, Channel.%d unmatch conf id in stop adpt req\n", 
//				 byChnIdx, m_byBasIdx, m_byBasChnIdx);
		return;
	}

	TMpu2BasCfg tMpu2BasCfg;
	if ( !g_cMpu2BasApp.GetMpu2BasCfg(m_byBasIdx,tMpu2BasCfg) )
	{
		ErrorLog("[ProcStopAdptReq] GetMpu2BasCfg idx:%d failed!\n", m_byBasIdx);	
//		printf("[ProcStopAdptReq] GetMpu2BasCfg idx:%d failed!\n", m_byBasIdx);
		return;
	}

	BOOL wRet = FALSE;
	switch( CurState() )
	{
	case RUNNING:
		{
			wRet = m_pAdpCroup->StopAdapter();
			
			if( TRUE != wRet )
			{	
				cServMsg.SetErrorCode( ERR_BAS_OPMAP );
				SendMsgToMcu( cServMsg.GetEventId() + 2, cServMsg );
				ErrorLog("Remove  instid:%d, basidx:%d, Channel.%d failed when stop adpt\n",
					byChnIdx, m_byBasIdx, m_byBasChnIdx);
				return;				           
			}        
			
			// [pengjie 2010/12/29] ֹͣ����ʱ�����ǰ��ƽ�����͹���
			m_pAdpCroup->SetSmoothSendRule( FALSE );
			
			//�����Ƶ�������
			if (NULL != m_ptFrmQueue)
			{
				m_ptFrmQueue->Clear();
			}	
			
			memset(&m_tChnInfo.m_tChnStatus, 0, sizeof(THDBasVidChnStatus));
			cServMsg.SetMsgBody( (u8*)&tMpu2BasCfg.m_tEqp, sizeof(tMpu2BasCfg.m_tEqp) );
			SendMsgToMcu( cServMsg.GetEventId() + 1, cServMsg );
			NextState( (u32)READY );
			SendChnNotif();
		}
		break;

    case READY:
        {
            cServMsg.SetErrorCode( ERR_BAS_CHREADY );
            cServMsg.SetMsgBody( (u8*)&tMpu2BasCfg.m_tEqp,sizeof(tMpu2BasCfg.m_tEqp) );
            SendMsgToMcu( cServMsg.GetEventId() + 2, cServMsg );
        }
		break;

	default:
		{
			cServMsg.SetErrorCode( ERR_BAS_CHNOTREAD );
			cServMsg.SetMsgBody( (u8*)&tMpu2BasCfg.m_tEqp, sizeof(tMpu2BasCfg.m_tEqp) );
			SendMsgToMcu( cServMsg.GetEventId() + 2, cServMsg );
		}
		break;
	}

    return;
}

/*====================================================================
	����  : ProcFastUpdatePicCmd
	����  : 
	����  : 
	���  : ��
	����  : ��
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2012/07/26  4.7         ��־��         ����
====================================================================*/
void CMpu2BasInst::ProcFastUpdatePicCmd()
{
    KeyLog("[ProcFastUpdatePicCmd] for instid:%d, basidx:%d, Channel.%d!\n", 
				GetInsID() - 1, m_byBasIdx, m_byBasChnIdx );

    u32 dwTimeInterval = OspClkRateGet();
    u32 dwLastFUPTick = m_tChnInfo.m_dwLastFUPTick;
    
	//����ģʽ���������������
	u8 byOutPutNum = 0;
	if ( !GetBasOutChnlNumAcd2WorkModeAndChnl( g_cMpu2BasApp.GetBasWorkMode(), m_byChnType,byOutPutNum ) )
	{
		ErrorLog("[ProcFastUpdatePicCmd]GetBasOutChnlNumAcd2WorkModeAndChnl mode:%d error\n", g_cMpu2BasApp.GetBasWorkMode());
		return;
	}

    u32 dwCurTick = OspTickGet();	
    if ( dwCurTick - dwLastFUPTick > dwTimeInterval )
    {
        m_tChnInfo.m_dwLastFUPTick = dwCurTick;
        
		if ( NULL != m_pAdpCroup )
		{			
			m_pAdpCroup->SetFastUpdata();
		}
		else
		{
			ErrorLog("[ProcFastUpdatePicCmd]m_pAdpGroup for instid.%d basidx:%d, Channel.%d cannot set fastupdate due to its not ready\n!", 
					GetInsID()-1, m_byBasIdx, m_byBasChnIdx );
		}
    }
    else
    {
        DetailLog("[ProcFastUpdatePicCmd] tick intval is %d - %d !\n", dwCurTick , dwLastFUPTick );
    }

    return;
}

/*====================================================================
	����  : ProcTimerNeedIFrame
	����  : 
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
    ��  ��         �汾        �޸���       �޸�����
    2012/07/26  4.7         ��־��          ����
====================================================================*/
void CMpu2BasInst::ProcTimerNeedIFrame()
{

	CServMsg cServMsg;    
    TKdvDecStatis tAdapterDecStatis;    
    memset( &tAdapterDecStatis, 0, sizeof(TKdvDecStatis) );
	
	KillTimer(EV_BAS_NEEDIFRAME_TIMER);
    DetailLog("[ProcTimerNeedIFrame] instid:%d, basidx:%d, Chn.%d IFrm check timer!\n", 
				GetInsID()-1, m_byBasIdx, m_byBasChnIdx);
	
	TMpu2BasCfg tMpu2BasCfg;
	if ( !g_cMpu2BasApp.GetMpu2BasCfg(m_byBasIdx,tMpu2BasCfg) )
	{
		ErrorLog("[ProcTimerNeedIFrame] GetMpu2BasCfg idx:%d failed!\n", m_byBasIdx);	
		return;
	}

	if ( m_pAdpCroup && m_pAdpCroup->IsStart() )
	{        
        m_pAdpCroup->GetVideoChannelStatis(tAdapterDecStatis );
		
        if ( tAdapterDecStatis.m_bVidCompellingIFrm )
		{
			m_bIsIframeFirstReq = TRUE;
            cServMsg.SetConfId( m_tChnInfo.m_cChnConfId );
            cServMsg.SetChnIndex( GetInsID()-1 );
            cServMsg.SetSrcSsnId( tMpu2BasCfg.m_tCfg.byEqpId );
            SendMsgToMcu( BAS_MCU_NEEDIFRAME_CMD, cServMsg );
			
			KeyLog( "[ProcTimerNeedIFrame]instid:%d, basidx:%d, Chn.%d request iframe!!\n", 
						GetInsID() - 1 , m_byBasIdx, m_byBasChnIdx);
		}
		else if (FALSE == m_bIsIframeFirstReq)
		{
			DetailLog("[ProcTimerNeedIFrame] instid:%d, basidx:%d, Chn.%d settimer EV_BAS_NEEDIFRAME_TIMER :%d\n",
						GetInsID() - 1 , m_byBasIdx, m_byBasChnIdx, CHECK_IFRAME_INTERVAL);
			m_bIsIframeFirstReq = FALSE;
			SetTimer(EV_BAS_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL);
			return;
		}
		else
		{
			DetailLog("[ProcTimerNeedIFrame]No iframe request for bas instid:%d, basidx:%d, Chn.%d\n", 
						GetInsID() - 1, m_byBasIdx, m_byBasChnIdx);
		}
	}
	
	DetailLog("[ProcTimerNeedIFrame]bas instid:%d, basidx:%d, Chn.%d set EV_BAS_NEEDIFRAME_TIMER : %d\n", 
			   GetInsID() - 1, m_byBasIdx, m_byBasChnIdx, g_cMpu2BasApp.GetIframeInterval());
    SetTimer( EV_BAS_NEEDIFRAME_TIMER, g_cMpu2BasApp.GetIframeInterval() );   // ��ʱ��ѯ�������Ƿ���Ҫ�ؼ�֡
	
    return;
}
/*=============================================================================
�� �� ���� ClearInstStatus
��    �ܣ� ��ձ�ʵ�������״̬��Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2012/07/26  4.7         ��־��         ����
=============================================================================*/
void CMpu2BasInst::ClearInstStatus( void )
{
	//���״̬
	for ( u8 byIdx = 0; byIdx <  g_cMpu2BasApp.GetBasNum(); byIdx++)
	{
		// ��ֹͣ����
		u8 byInitNum = 0;
		if(!GetBasInitChnlNumAcd2WorkMode(g_cMpu2BasApp.GetBasWorkMode(), byInitNum))
		{
			ErrorLog("[ClearInstStatus] GetBasInitChnlNumAcd2WorkMode failed!\n");
			return;
		}	
		
		u8 byLoop;
		for ( byLoop = 1; byLoop <= byInitNum; byLoop++ )
		{
			post( MAKEIID(GetAppID(), byLoop+byIdx*byInitNum), EV_BAS_QUIT );
		}
    
		TMpu2BasCfg tTempMpu2BasCfg;
		memset(&tTempMpu2BasCfg, 0 , sizeof(TMpu2BasCfg));
		if ( g_cMpu2BasApp.GetMpu2BasCfg(byIdx,tTempMpu2BasCfg) )
		{
			tTempMpu2BasCfg.m_byRegAckNum = 0;
			tTempMpu2BasCfg.m_dwSSrc = 0;
			g_cMpu2BasApp.SetMpu2BasCfg(byIdx, tTempMpu2BasCfg);
		}
	}
	
	KeyLog("[ClearInstStatus] tMpu2BasCfg is set NULL\n");

	// DAEMON ʵ���������״̬
    NEXTSTATE( (u32)DAEMON_INIT ); 
    return;
}

/*====================================================================
	����  : ProcModSndAddr
	����  : 
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
    ��  ��         �汾        �޸���       �޸�����
    2012/07/26  4.7         ��־��          ����
====================================================================*/
void CMpu2BasInst::ProcModSndAddr()
{
	TMpu2BasCfg tMpu2BasCfg;
	if ( !g_cMpu2BasApp.GetMpu2BasCfg(m_byBasIdx,tMpu2BasCfg) )
	{
		ErrorLog("[ProcModSndAddr] GetMpu2BasCfg idx:%d failed!\n", m_byBasIdx);	
		return;
	}

	if (m_pAdpCroup != NULL)
	{
		m_pAdpCroup->ModNetSndIpAddr(tMpu2BasCfg.m_dwMcuRcvIp);
	}

}

/*====================================================================
	����  : AdjustParamForMpuSimulating
	����  : 
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
    ��  ��         �汾        �޸���       �޸�����
    2013/01/18  4.7         ��־��          ����
====================================================================*/
void CMpu2BasInst::AdjustParamForMpuSimulating(THDAdaptParam *patParam, u8 byOutPutNum)
{
	if ( patParam == NULL )
	{
		ErrorLog("[AdjustParamForMpuSimulating]patParam is NULL\n!");
		return;
	}

	u8 byOutIdx=0;
	u16 wFakeWidth = 0;
	u16 wFakeHeight = 0;
	for ( byOutIdx =0 ; byOutIdx<byOutPutNum; byOutIdx++ )
	{
		wFakeWidth = 0;
		wFakeHeight = 0;
		//����һ�£������ʽ�У���֡�ʡ��ֱ��ʲ��ԣ�ֱ�����ó�CIF
		if ( patParam[byOutIdx].GetVidType() != MEDIA_TYPE_NULL &&
			0 != patParam[byOutIdx].GetVidType() &&
			(patParam[byOutIdx].GetWidth() ==0 ||
			patParam[byOutIdx].GetHeight() ==0||
			patParam[byOutIdx].GetFrameRate()==0
			) 
			)
		{
			GetWHViaRes(VIDEO_FORMAT_CIF,wFakeWidth,wFakeHeight);
			patParam[byOutIdx].SetResolution( wFakeWidth, wFakeHeight );
			patParam[byOutIdx].SetFrameRate(25);
			patParam[byOutIdx].SetProfileType(0);
			if ( patParam[byOutIdx].GetBitrate() == 0 )
			{
				patParam[byOutIdx].SetBitRate(1000);
			}
		}

		if ( patParam[byOutIdx].GetVidType() == MEDIA_TYPE_NULL ||
			0 == patParam[byOutIdx].GetVidType()
			)
		{
			patParam[byOutIdx].SetBitRate(1000);
			if ( TYPE_MPUBAS== g_cMpu2BasApp.GetBasWorkMode() )
			{
				if ( byOutIdx == 0 )
				{
					GetWHViaRes(VIDEO_FORMAT_HD1080,wFakeWidth,wFakeHeight);
					patParam[byOutIdx].SetVidType(MEDIA_TYPE_H264);
					patParam[byOutIdx].SetResolution( wFakeWidth, wFakeHeight );
					patParam[byOutIdx].SetFrameRate(30);
					patParam[byOutIdx].SetProfileType(0);
				}
				else if ( byOutIdx == 1 )
				{
					GetWHViaRes(VIDEO_FORMAT_CIF,wFakeWidth,wFakeHeight);
					patParam[byOutIdx].SetVidType(MEDIA_TYPE_H264);
					patParam[byOutIdx].SetResolution( wFakeWidth, wFakeHeight );
					patParam[byOutIdx].SetFrameRate(25);
					patParam[byOutIdx].SetProfileType(0);
				}
			}
			else if ( TYPE_MPUBAS_H == g_cMpu2BasApp.GetBasWorkMode() )
			{
				if ( byOutIdx == 0 )
				{
					GetWHViaRes(VIDEO_FORMAT_CIF,wFakeWidth,wFakeHeight);
					patParam[byOutIdx].SetVidType(MEDIA_TYPE_H264);
					patParam[byOutIdx].SetResolution( wFakeWidth, wFakeHeight );
					patParam[byOutIdx].SetFrameRate(25);
					patParam[byOutIdx].SetProfileType(0);
				}
				else if ( byOutIdx == 1 )
				{
					GetWHViaRes(VIDEO_FORMAT_HD720,wFakeWidth,wFakeHeight);
					patParam[byOutIdx].SetVidType(MEDIA_TYPE_H264);
					patParam[byOutIdx].SetResolution( wFakeWidth, wFakeHeight );
					patParam[byOutIdx].SetFrameRate(30);
					patParam[byOutIdx].SetProfileType(0);
				}
				else if ( byOutIdx == 2 )
				{
					GetWHViaRes(VIDEO_FORMAT_4CIF,wFakeWidth,wFakeHeight);
					patParam[byOutIdx].SetVidType(MEDIA_TYPE_H264);
					patParam[byOutIdx].SetResolution( wFakeWidth, wFakeHeight );
					patParam[byOutIdx].SetFrameRate(25);
					patParam[byOutIdx].SetProfileType(0);
				}
				else if ( byOutIdx == 3 )
				{
					GetWHViaRes(VIDEO_FORMAT_CIF,wFakeWidth,wFakeHeight);
					patParam[byOutIdx].SetVidType(MEDIA_TYPE_H264);
					patParam[byOutIdx].SetResolution( wFakeWidth, wFakeHeight );
					patParam[byOutIdx].SetFrameRate(25);
					patParam[byOutIdx].SetProfileType(0);
				}
				else if ( byOutIdx == 4 )
				{
					GetWHViaRes(VIDEO_FORMAT_4CIF,wFakeWidth,wFakeHeight);
					patParam[byOutIdx].SetVidType(MEDIA_TYPE_H264);
					patParam[byOutIdx].SetResolution( wFakeWidth, wFakeHeight );
					patParam[byOutIdx].SetFrameRate(25);
					patParam[byOutIdx].SetProfileType(0);
				}
				else if ( byOutIdx == 5 )
				{
					GetWHViaRes(VIDEO_FORMAT_4CIF,wFakeWidth,wFakeHeight);
					patParam[byOutIdx].SetVidType(MEDIA_TYPE_H264);
					patParam[byOutIdx].SetResolution( wFakeWidth, wFakeHeight );
					patParam[byOutIdx].SetFrameRate(25);
					patParam[byOutIdx].SetProfileType(0);
				}
			}
		}
	}

}


/*=============================================================================
  �� �� ���� StatusShow
  ��    �ܣ� ��ʾ��ǰͨ��״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2012/07/26  4.7         ��־��         ����
=============================================================================*/
void CMpu2BasInst::StatusShow( void )
{
    if ( GetInsID() != CInstance::DAEMON)
    {
        if(m_pAdpCroup)
        {
            m_pAdpCroup->ShowChnInfo();
        }
        else
        {
            WarningLog("m_pAdpCroup is NULL\n");
        }
    }
    m_tChnInfo.m_cChnConfId.Print();
    m_ptFrmQueue->Print();
    return;
}

BOOL32 CMpu2BasInst::GetBasInitChnlNumAcd2WorkMode(u8 byWorkMode, u8 &byInitChnlNum)
{
	byInitChnlNum = 0;
	switch (byWorkMode)
	{
	case TYPE_MPU2BAS_BASIC:
		byInitChnlNum = MAXNUM_MPU2BAS_BASIC_CHANNEL;
		break;
	case TYPE_MPU2BAS_ENHANCED:
		byInitChnlNum = MAXNUM_MPU2BAS_EHANCED_CHANNEL;
		break;
	case TYPE_MPUBAS:
		byInitChnlNum = MAXNUM_MPUBAS_CHANNEL;
		break;
	case TYPE_MPUBAS_H:
		byInitChnlNum = MAXNUM_MPUBAS_H_CHANNEL;
		break;
	default:
		ErrorLog("[GetBasInitChnlNumAcd2WorkMode] unexpected mpu workmode:%d!\n", byWorkMode);
		break;
	}
	return 0 != byInitChnlNum ;
}

BOOL32 CMpu2BasInst::GetBasOutChnlNumAcd2WorkModeAndChnl(u8 byWorkMode,  u8 byChnType, u8 &byOutChnlNum)
{
	byOutChnlNum = 0;
	switch (byWorkMode)
	{
	case TYPE_MPU2BAS_BASIC:
		{
			if ( byChnType ==  MPU2_BAS_CHN_MV)
			{
				byOutChnlNum = MPU2_BAS_BASIC_MVOUTPUT;
			}
			else if (byChnType == MPU2_BAS_CHN_DS)
			{
				byOutChnlNum = MPU2_BAS_BASIC_DSOUTPUT;
			}
			else if (byChnType == MPU2_BAS_CHN_SEL)
			{
				byOutChnlNum = MPU2_BAS_BASIC_SELOUTPUT;
			}
			else
			{
				ErrorLog("[GetBasOutChnlNumAcd2WorkModeAndChnl] error,wokrmode:%d-ChnType:%d\n",byWorkMode,byChnType);
			}
		}
		break;
	case TYPE_MPU2BAS_ENHANCED:
		{
			if ( byChnType ==  MPU2_BAS_CHN_MV)
			{
				byOutChnlNum = MPU2_BAS_ENHACNED_MVOUTPUT;
			}
			else if (byChnType == MPU2_BAS_CHN_DS)
			{
				byOutChnlNum = MPU2_BAS_ENHACNED_DSOUTPUT;
			}
			else if (byChnType == MPU2_BAS_CHN_SEL)
			{
				byOutChnlNum = MPU2_BAS_ENHACNED_SELOUTPUT;
			}
			else
			{
				ErrorLog("[GetBasOutChnlNumAcd2WorkModeAndChnl] error,wokrmode:%d-ChnType:%d\n",byWorkMode,byChnType);
			}
		}
		break;
	case TYPE_MPUBAS:
		byOutChnlNum = MAXNUM_VOUTPUT;
		break;
	case TYPE_MPUBAS_H:
		byOutChnlNum = MAXNUM_MPU_H_VOUTPUT;
		break;
	default:
		ErrorLog("[GetBasOutChnlNumAcd2WorkModeAndChnl] error,wokrmode:%d-ChnType:%d\n",byWorkMode,byChnType);
		break;
	}
	return 0 != byOutChnlNum ;
}

BOOL32 CMpu2BasInst::GetFrontOutNumAcd2WorckModeAndChnl(u8 byWorkMode, u8 byChnType,u8 byChnId, u8 &byOutFrontOutNum)
{
	byOutFrontOutNum = 0;
	BOOL32 bIsGetOk = FALSE;
	switch (byWorkMode)
	{
	case TYPE_MPU2BAS_BASIC:
		{
			if ( byChnType ==  MPU2_BAS_CHN_MV)
			{
				byOutFrontOutNum = 0;
				bIsGetOk = TRUE;
			}
			else if (byChnType == MPU2_BAS_CHN_DS)
			{
				byOutFrontOutNum = MPU2_BAS_BASIC_MVOUTPUT;
				bIsGetOk = TRUE;
			}
			else if (byChnType == MPU2_BAS_CHN_SEL)
			{
				//MPU2 BASѡ��ͨ�����ں��棬0��1�ֱ���������˫��ͨ��
				if (byChnId > 1)
				{
					byOutFrontOutNum = MPU2_BAS_BASIC_MVOUTPUT + MPU2_BAS_BASIC_DSOUTPUT + (byChnId-2)*MPU2_BAS_BASIC_SELOUTPUT;
					bIsGetOk = TRUE;
				}
				else
				{
					ErrorLog("[GetBasOutChnlNumAcd2WorkModeAndChnl] error,wokrmode:%d-ChnType:%d--chid:%d\n",
						byWorkMode,byChnType,byChnId);
					bIsGetOk =  FALSE;
				}
			}
			else
			{
				ErrorLog("[GetBasOutChnlNumAcd2WorkModeAndChnl] error,wokrmode:%d-ChnType:%d--chid:%d\n",
						byWorkMode,byChnType,byChnId);
				bIsGetOk =  FALSE;
			}
		}
		break;
	case TYPE_MPU2BAS_ENHANCED:
		{
			if ( byChnType ==  MPU2_BAS_CHN_MV)
			{
				byOutFrontOutNum = 0;
				bIsGetOk = TRUE;
			}
			else if (byChnType == MPU2_BAS_CHN_DS)
			{
				byOutFrontOutNum = MPU2_BAS_ENHACNED_MVOUTPUT;
				bIsGetOk = TRUE;
			}
			else if (byChnType == MPU2_BAS_CHN_SEL)
			{
				//MPU2 BASѡ��ͨ�����ں��棬0��1�ֱ���������˫��ͨ��
				if (byChnId > 1)
				{
					byOutFrontOutNum = MPU2_BAS_ENHACNED_MVOUTPUT + MPU2_BAS_ENHACNED_DSOUTPUT + (byChnId-2)*MPU2_BAS_ENHACNED_SELOUTPUT;
					bIsGetOk = TRUE;
				}
				else
				{
					ErrorLog("[GetBasOutChnlNumAcd2WorkModeAndChnl] error,wokrmode:%d-ChnType:%d--chid:%d\n",
						byWorkMode,byChnType,byChnId);
					bIsGetOk = FALSE;
				}
			}
			else
			{
				ErrorLog("[GetFrontOutNumAcd2WorckModeAndChnl] error,wokrmode:%d-ChnType:%d--chid:%d\n",
						 byWorkMode,byChnType,byChnId);
				bIsGetOk = FALSE;
			}
		}
		break;
	case TYPE_MPUBAS:
		{
			byOutFrontOutNum = byChnId*MAXNUM_VOUTPUT;
			bIsGetOk = TRUE;
		}
		break;
	case TYPE_MPUBAS_H:
		{
			byOutFrontOutNum = byChnId*MAXNUM_MPU_H_VOUTPUT;
			bIsGetOk = TRUE;
		}
		break;
	default:
		{
			ErrorLog("[GetFrontOutNumAcd2WorckModeAndChnl] error,wokrmode:%d-ChnType:%d--chid:%d\n",
				byWorkMode,byChnType,byChnId);
			bIsGetOk = FALSE;
		}
		break;
	}
	return bIsGetOk ;
}
//END OF FILE
