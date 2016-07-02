/*****************************************************************************
   ģ����      : mpulib
   �ļ���      : mpubas.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: mpulib bas̬��֧ʵ��
   ����        : ����
   �汾        : V4.5  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��       �汾        �޸���      �޸�����
   2009/3/14    4.6         �ű���      ע��
******************************************************************************/
#include "mpuinst.h"
//#include "mpustruct.h"
#include "evmpu.h"
#include "mcuver.h"
#include "evmcueqp.h"
#include "baserr.h"
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
    2008/12/04  4.5         ����        ����
====================================================================*/
/*lint -save -e715*/
void CMpuBasInst::DaemonInstanceEntry( CMessage* const pMsg, CApp* pcApp )
{
    switch ( pMsg->event )
    {
    //�ϵ��ʼ��
    case OSP_POWERON:
		DaemonProcPowerOn( pMsg/*, pcApp*/ );
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
        DaemonProcRegisterTimeOut( TRUE );
        break;
	// ע��
    case EV_BAS_REGISTERB:  	
        DaemonProcRegisterTimeOut( FALSE );
        break;

    // MCU ע��Ӧ����Ϣ
    case MCU_BAS_REG_ACK:
        DaemonProcMcuRegAck( pMsg/*, pcApp*/ );
        break;

    // MCU�ܾ���������ע��
    case MCU_BAS_REG_NACK:
        DaemonProcMcuRegNack( pMsg );
        break;

    // OSP ������Ϣ
    case OSP_DISCONNECT:
        DaemonProcOspDisconnect( pMsg/*, pcApp*/ );
        break;

    // ����Qosֵ
    case MCU_EQP_SETQOS_CMD:
        //DaemonProcSetQosCmd( pMsg );
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
            u8 byChnIdx = cServMsg.GetChnIndex();
            u8 byChnId = byChnIdx + 1;
            post( MAKEIID( GetAppID(), byChnId ), pMsg->event, pMsg->content, pMsg->length );
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
    2008/08/04  4.5         ����         ����
====================================================================*/
void CMpuBasInst::InstanceEntry( CMessage* const pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    cServMsg.SetEventId(pcMsg->event);
    
    if ( pcMsg->event != EV_BAS_NEEDIFRAME_TIMER &&
         pcMsg->event != EV_BAS_STATUSCHK_TIMER )
    {
        mpulog( MPU_INFO, "[BasInst.%d] Receive msg.%d<%s>\n", GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event) );
    }

    switch ( pcMsg->event )    
    {
    // ��ʼ��ͨ��
    case EV_BAS_INI:
        ProcInitBas();
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
        ProcFastUpdatePicCmd(/*cServMsg*/);
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
		log( LOGLVL_EXCEPTION, "[Error]Unexcpet Message %u(%s).\n", pcMsg->event, ::OspEventDesc(pcMsg->event) );
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
  2008/12/04  4.5         ����        ����
=============================================================================*/
void CMpuBasInst::DaemonProcPowerOn( const CMessage* const pcMsg/*, CApp* pcApp*/ )
{
    if ( pcMsg->length == sizeof(TMpuCfg) )
    {
        memcpy( (u8*)(&g_cMpuBasApp.m_tCfg), pcMsg->content, sizeof(TEqpCfg) );
    }
    //g_cMpuBasApp.m_byWorkMode = TYPE_MPUBAS;
	
    // ��������
    sprintf( MPULIB_CFG_FILE, "%s/mcueqp.ini", DIR_CONFIG );
    g_cMpuBasApp.ReadDebugValues();    

    u16 wRet = KdvSocketStartup();
    
    if ( MEDIANET_NO_ERROR != wRet )
    {
        mpulog( MPU_CRIT, "KdvSocketStartup failed, error: %d\n", wRet );
        printf( "KdvSocketStartup failed, error: %d\n", wRet );
        return ;
    }

#ifdef _LINUX_
	if (TYPE_MPUBAS == g_cMpuBasApp.m_byWorkMode)
	{
		wRet = HardMPUInit( (u32)INITMODE_HDBAS );
	}
	else if(TYPE_MPUBAS_H == g_cMpuBasApp.m_byWorkMode)
	{
		wRet = HardMPUInit( (u32)INITMODE_HD2BAS );
	}
	else
	{
		printf("[DaemonProcPowerOn] unexpected Mpu WorkMode:%d!\n", g_cMpuBasApp.m_byWorkMode);
	}
    
    if ( (u16)Codec_Success != wRet )
    {
        mpulog( MPU_CRIT, "HardMPUInit failed, error: %d\n", wRet );
        printf( "HardMPUInit failed, error: %d\n", wRet );
        return ;
    }
#endif	  
    
    // ��ʼ��MCUע��
    if ( TRUE == g_cMpuBasApp.m_bEmbedMcu )
    {
        mpulog( MPU_INFO, "[DaemonProcPowerOn] bas embed in Mcu A\n" );
        g_cMpuBasApp.m_dwMcuNode = 0;                              //��Ƕʱ�����ڵ����Ϊ0�Ϳ���
        SetTimer( EV_BAS_REGISTER, MPU_REGISTER_TIMEOUT );   //ֱ��ע��
    }
    
    if( TRUE == g_cMpuBasApp.m_bEmbedMcuB )
    {
        mpulog( MPU_INFO, "[DaemonProcPowerOn] bas embed in Mcu B\n" );
        g_cMpuBasApp.m_dwMcuNodeB = 0;
        SetTimer( EV_BAS_REGISTERB, MPU_REGISTER_TIMEOUT );  //ֱ��ע��
    } 
    
    if( 0 != g_cMpuBasApp.m_tCfg.dwConnectIP && FALSE == g_cMpuBasApp.m_bEmbedMcu )
    {
        SetTimer( EV_BAS_CONNECT, MPU_CONNETC_TIMEOUT );     //3s���ٽ���
    }
    if( 0 != g_cMpuBasApp.m_tCfg.dwConnectIpB && FALSE == g_cMpuBasApp.m_bEmbedMcuB )
    {
        SetTimer( EV_BAS_CONNECTB, MPU_CONNETC_TIMEOUT );    //3s���ٽ���
    }

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
  2008/12/04  4.5         ����        ����
=============================================================================*/
void CMpuBasInst::DaemonProcConnectTimeOut( BOOL32 bIsConnectA )
{
    BOOL32 bRet = FALSE;
    if( TRUE == bIsConnectA )
    {
        bRet = ConnectMcu( bIsConnectA, g_cMpuBasApp.m_dwMcuNode );
        if ( TRUE == bRet )
        {  
            SetTimer( EV_BAS_REGISTER, MPU_REGISTER_TIMEOUT ); 
        }
        else
        {
            SetTimer( EV_BAS_CONNECT, MPU_CONNETC_TIMEOUT );   //3s���ٽ���
        }
    }
    else
    {
        bRet = ConnectMcu( bIsConnectA, g_cMpuBasApp.m_dwMcuNodeB );
        if ( TRUE == bRet )
        { 
            SetTimer( EV_BAS_REGISTERB, MPU_REGISTER_TIMEOUT ); 
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
  2008/08/04  4.5         ����         ����
====================================================================*/
BOOL32 CMpuBasInst::ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode )
{
    BOOL32 bRet = TRUE;
    if( !OspIsValidTcpNode(dwMcuNode) ) // ���˲��Ϸ��ڵ�
    {
        if(TRUE == bIsConnectA)
        {
            // zw 20081208 ò��ȥOspConnect����ʵû������ȥȡ��
            dwMcuNode = BrdGetDstMcuNode();  
        }
        else
        {  
            dwMcuNode = BrdGetDstMcuNodeB(); 
        }

	    if ( ::OspIsValidTcpNode(dwMcuNode) )
	    {
			printf("[ConnectMcu]Connect Mcu %s Success, node: %d!\n", 
                   bIsConnectA ? "A" : "B", dwMcuNode);

		    mpulog(MPU_CRIT, "Connect Mcu %s Success, node: %d!\n", 
                   bIsConnectA ? "A" : "B", dwMcuNode);
		    ::OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), GetInsID());// ������Ϣ����
	    }
	    else 
	    {
			printf("[ConnectMcu]Connect Mcu %s failed, node: %d\n", 
                   bIsConnectA ? "A" : "B", dwMcuNode);
		    mpulog(MPU_CRIT, "Connect to Mcu failed, will retry\n");
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
  2008/12/04  4.5         ����        ����
=============================================================================*/
void CMpuBasInst::DaemonProcRegisterTimeOut( BOOL32 bIsRegiterA )
{
    if( TRUE == bIsRegiterA )
    {
		printf("[DaemonProcRegisterTimeOut] send BAS_MCU_REG_REQ to NodeA.%d\n", g_cMpuBasApp.m_dwMcuNode);
        Register( /*bIsRegiterA,*/ g_cMpuBasApp.m_dwMcuNode );
        SetTimer( EV_BAS_REGISTER, MPU_REGISTER_TIMEOUT );  //ֱ��ע��
    }
    else
    {
		printf("[DaemonProcRegisterTimeOut] send BAS_MCU_REG_REQ to NodeB.%d\n",g_cMpuBasApp.m_dwMcuNodeB);
        Register(/* bIsRegiterA,*/ g_cMpuBasApp.m_dwMcuNodeB );
		SetTimer( EV_BAS_REGISTERB, MPU_REGISTER_TIMEOUT );  //ֱ��ע��
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
  2008/12/04  4.5         ����        ����
====================================================================*/
void CMpuBasInst::Register(/* BOOL32 bIsRegiterA,*/ u32 dwMcuNode )
{
    CServMsg       cSvrMsg;
    TPeriEqpRegReq tReg;

    tReg.SetMcuEqp( (u8)g_cMpuBasApp.m_tCfg.wMcuId, g_cMpuBasApp.m_tCfg.byEqpId, g_cMpuBasApp.m_tCfg.byEqpType );
    tReg.SetPeriEqpIpAddr( htonl(g_cMpuBasApp.m_tCfg.dwLocalIP) );
    tReg.SetVersion( DEVVER_MPU );
    tReg.SetStartPort( g_cMpuBasApp.m_tCfg.wRcvStartPort );
	tReg.SetEqpAlias( g_cMpuBasApp.m_tCfg.achAlias );
	// fxh bas����
    tReg.SetHDEqp( TRUE );

    cSvrMsg.SetMsgBody( (u8*)&tReg, sizeof(tReg) );
        
    post( MAKEIID(AID_MCU_PERIEQPSSN, g_cMpuBasApp.m_tCfg.byEqpId),
            BAS_MCU_REG_REQ,
            cSvrMsg.GetServMsg(),
            cSvrMsg.GetServMsgLen(),
            dwMcuNode );

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
  2008/12/04  4.5         ����        ����
====================================================================*/
void CMpuBasInst::DaemonProcMcuRegAck( const CMessage* const pMsg/*, CApp* pcApp*/ )
{
	if( NULL == pMsg )
    {
        mpulog( MPU_CRIT, "Recv Reg Ack Msg, but the msg's pointer is Null\n" );
        return;
    }

	TPeriEqpRegAck* ptRegAck;

	CServMsg cServMsg( pMsg->content, pMsg->length );
	u8* pMsgBody = cServMsg.GetMsgBody();

	ptRegAck = (TPeriEqpRegAck*)pMsgBody;
	pMsgBody += sizeof(TPeriEqpRegAck);
	OspPrintf(TRUE, FALSE, "[DaemonProcMcuRegAck] ����ת����Ip:%s!\n", StrOfIP(ptRegAck->GetMcuIpAddr()));
    TPrsTimeSpan tPrsTimeSpan = *(TPrsTimeSpan*)pMsgBody;
	pMsgBody += sizeof(TPrsTimeSpan);

	u16 wMTUSize = *(u16*)pMsgBody;
	pMsgBody += sizeof(u16);
	wMTUSize = ntohs( wMTUSize );

	TEqpBasHDCfgInfo tBasConfig = *(TEqpBasHDCfgInfo*)pMsgBody;

    if( pMsg->srcnode == g_cMpuBasApp.m_dwMcuNode ) // ע��Ӧ��
    {
	    g_cMpuBasApp.m_dwMcuIId = pMsg->srcid;
        g_cMpuBasApp.m_byRegAckNum++;
        KillTimer(EV_BAS_REGISTER);// �رն�ʱ��

        mpulog( MPU_INFO, "[RegAck] Regist success to mcu A\n" );
    }
    else if ( pMsg->srcnode == g_cMpuBasApp.m_dwMcuNodeB )
    {
        g_cMpuBasApp.m_dwMcuIIdB = pMsg->srcid;
        g_cMpuBasApp.m_byRegAckNum++;

        KillTimer(EV_BAS_REGISTERB);

        mpulog( MPU_INFO, "[RegAck] Regist success to mcu B\n" );       
    }

	// guzh [6/12/2007] У��Ự����
    if ( g_cMpuBasApp.m_dwMpcSSrc == 0 )
    {
        g_cMpuBasApp.m_dwMpcSSrc = ptRegAck->GetMSSsrc();
    }
    else
    {
        // �쳣������Ͽ������ڵ�
        if ( g_cMpuBasApp.m_dwMpcSSrc != ptRegAck->GetMSSsrc() )
        {
            mpulog( MPU_CRIT, "[RegAck] MPC SSRC ERROR(%u<-->%u), Disconnect Both Nodes!\n", 
                      g_cMpuBasApp.m_dwMpcSSrc, ptRegAck->GetMSSsrc());
            if ( OspIsValidTcpNode(g_cMpuBasApp.m_dwMcuNode) )
            {
                OspDisconnectTcpNode(g_cMpuBasApp.m_dwMcuNode);
            }
            if ( OspIsValidTcpNode(g_cMpuBasApp.m_dwMcuNodeB) )
            {
                OspDisconnectTcpNode(g_cMpuBasApp.m_dwMcuNodeB);
            }      
            return;
        }
    }
    
	u8 byInitChnlNum = 0;
	if (!GetBasInitChnlNumAcd2WorkMode(g_cMpuBasApp.m_byWorkMode, byInitChnlNum))
	{
		OspPrintf(TRUE,  FALSE, "[DaemonProcMcuRegAck] GetBasInitChnlNumAcd2WorkMode failed!\n");
		return;
	}
    OspPrintf(TRUE, FALSE, "[RegAck] mpu init ChnNum:%d\n", byInitChnlNum);

    if( FIRST_REGACK == g_cMpuBasApp.m_byRegAckNum ) // ��һ���յ�ע��ɹ���Ϣ
    {
        g_cMpuBasApp.m_dwMcuRcvIp       = ptRegAck->GetMcuIpAddr();
	    g_cMpuBasApp.m_wMcuRcvStartPort = ptRegAck->GetMcuStartPort();

        // ��������ʱû�л�ȡ�������ȡ����һ��MCU�������ȡ��ȥ����
        u32 dwOtherMcuIp = htonl( ptRegAck->GetAnotherMpcIp() );
        if ( g_cMpuBasApp.m_tCfg.dwConnectIpB == 0 && dwOtherMcuIp != 0 )
        {
            g_cMpuBasApp.m_tCfg.dwConnectIpB = dwOtherMcuIp;
            SetTimer( EV_BAS_CONNECTB, MPU_CONNETC_TIMEOUT );

            mpulog( MPU_CRIT, "[RegAck] Found another Mpc IP: %s, try connecting...\n", 
                      StrOfIP(dwOtherMcuIp) );
        }

		memcpy(g_cMpuBasApp.m_tCfg.achAlias, tBasConfig.GetAlias(), MAXLEN_EQP_ALIAS);
        mpulog( MPU_INFO, "[RegAck] Local Recv Start Port.%u, Mcu Start Port.%u, Alias.%s\n",
               g_cMpuBasApp.m_tCfg.wRcvStartPort, g_cMpuBasApp.m_wMcuRcvStartPort , g_cMpuBasApp.m_tCfg.achAlias );

        g_cMpuBasApp.m_tPrsTimeSpan = tPrsTimeSpan;
        mpulog( MPU_INFO, "[RegAck] The Bas Prs span: first: %d, second: %d, three: %d, reject: %d\n", 
                g_cMpuBasApp.m_tPrsTimeSpan.m_wFirstTimeSpan,
                g_cMpuBasApp.m_tPrsTimeSpan.m_wSecondTimeSpan,
                g_cMpuBasApp.m_tPrsTimeSpan.m_wThirdTimeSpan,
                g_cMpuBasApp.m_tPrsTimeSpan.m_wRejectTimeSpan );

		g_cMpuBasApp.m_wMTUSize = wMTUSize;
		mpulog( MPU_INFO, "[RegAck]The Network MTU is : %d\n", g_cMpuBasApp.m_wMTUSize );

        // zw 20081208 �ڵ�һ��ע��ɹ�֮�󣬽�ȷ�Ϻ��m_tCfg�е���Ϣ��m_tCfg����PowerOnʱ������������ģ�ת����m_tEqp��
        g_cMpuBasApp.m_tEqp.SetMcuEqp( (u8)g_cMpuBasApp.m_tCfg.wMcuId, g_cMpuBasApp.m_tCfg.byEqpId, g_cMpuBasApp.m_tCfg.byEqpType );	

        // DAEMON ʵ������ NORMAL ״̬����û��IDLE��READY״̬��û����ʲô���á�
        NEXTSTATE( (u32)NORMAL ); 

        u8 byLoop;
		//����mpubas����ģʽ��������ʼ������ͨ��
		
        for ( byLoop = 1; byLoop <= byInitChnlNum/*MAXNUM_MPUBAS_CHANNEL*/; byLoop++ )
        {
            post( MAKEIID(GetAppID(), byLoop), EV_BAS_INI );
        }
    }

    TPeriEqpStatus tEqpStatus;
    tEqpStatus.SetMcuEqp( (u8)g_cMpuBasApp.m_tCfg.wMcuId, 
                          g_cMpuBasApp.m_tCfg.byEqpId,
                          g_cMpuBasApp.m_tCfg.byEqpType );
    tEqpStatus.m_byOnline = 1;
    tEqpStatus.SetAlias( g_cMpuBasApp.m_tCfg.achAlias );

	//����bas����ģʽ��mcu����TYPE_MPU��TYPE_MPU_H����
	if (TYPE_MPUBAS == g_cMpuBasApp.m_byWorkMode)
	{
		tEqpStatus.m_tStatus.tHdBas.SetEqpType(TYPE_MPU);
	}
	else if (TYPE_MPUBAS_H == g_cMpuBasApp.m_byWorkMode)
	{
		tEqpStatus.m_tStatus.tHdBas.SetEqpType(TYPE_MPU_H);
	}
	else
	{
		OspPrintf(TRUE, FALSE, "[DaemonProcMcuRegAck] unexpected mpu workmode:%d!\n", g_cMpuBasApp.m_byWorkMode);
	}
    
    cServMsg.SetMsgBody( (u8*)&tEqpStatus, sizeof(tEqpStatus) );

	SendMsgToMcu( BAS_MCU_BASSTATUS_NOTIF, cServMsg );
	
    for (u8 byChnlIdx = 1; byChnlIdx <= byInitChnlNum/*MAXNUM_MPUBAS_CHANNEL*/; byChnlIdx++ )
    {
		post( MAKEIID(GetAppID(), byChnlIdx), EV_BAS_SENDCHNSTATUS);
    }
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
  2008/12/09  4.5         ����        ����
====================================================================*/
void CMpuBasInst::DaemonProcMcuRegNack( CMessage* const pMsg ) const
{
    if ( pMsg->srcnode == g_cMpuBasApp.m_dwMcuNode )    
    {
        mpulog( MPU_CRIT, "Bas register be refused by A.\n" );
    }
    if ( pMsg->srcnode == g_cMpuBasApp.m_dwMcuNodeB )    
    {
        mpulog( MPU_CRIT, "Bas register be refused by B.\n" );
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
  2008/08/04  4.5         ����         ����
=============================================================================*/
void CMpuBasInst::DeamonProcGetMsStatusRsp( CMessage* const pMsg )
{
	BOOL bSwitchOk = FALSE;
	CServMsg cServMsg( pMsg->content, pMsg->length );
	if( MCU_EQP_GETMSSTATUS_ACK == pMsg->event )
	{
		TMcuMsStatus *ptMsStatus = (TMcuMsStatus*)cServMsg.GetMsgBody();
        
        KillTimer( EV_BAS_GETMSSTATUS );
        mpulog( MPU_INFO, "[DeamonProcGetMsStatus]. receive msg MCU_EQP_GETMSSTATUS_ACK. IsMsSwitchOK :%d\n", 
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
		mpulog(MPU_INFO, "[DeamonProcGetMsStatusRsp] ClearCurrentInst!\n");

		if ( OspIsValidTcpNode(g_cMpuBasApp.m_dwMcuNode))
		{
			mpulog(MPU_INFO, "[DeamonProcGetMsStatusRsp] OspDisconnectTcpNode A!\n");
			OspDisconnectTcpNode(g_cMpuBasApp.m_dwMcuNode);
		}
		if ( OspIsValidTcpNode(g_cMpuBasApp.m_dwMcuNodeB))
		{
			mpulog(MPU_INFO, "[DeamonProcGetMsStatusRsp] OspDisconnectTcpNode B!\n");
			OspDisconnectTcpNode(g_cMpuBasApp.m_dwMcuNodeB);
		}

		if ( INVALID_NODE == g_cMpuBasApp.m_dwMcuNode )// �п��ܲ�����������Connect�������
		{
			mpulog(MPU_INFO, "[DeamonProcGetMsStatusRsp] EV_BAS_CONNECT!\n");
			SetTimer( EV_BAS_CONNECT, MPU_CONNETC_TIMEOUT );
		}
		if ( INVALID_NODE == g_cMpuBasApp.m_dwMcuNodeB )
		{
			mpulog(MPU_INFO, "[DeamonProcGetMsStatusRsp] EV_BAS_CONNECTB!\n");
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
    2008/12/09  4.5         ����          ����
====================================================================*/
void CMpuBasInst::DaemonProcOspDisconnect( CMessage* const pMsg/*, CApp* pcApp*/ )
{
    if ( NULL == pMsg )  
    {
        mpulog( MPU_CRIT, "[DaemonProcOspDisconnect] message's pointer is Null\n" );
        return;
    }

    u32 dwNode = *(u32*)pMsg->content;

    if ( INVALID_NODE != dwNode )
    {
        OspDisconnectTcpNode( dwNode );
    } 
    if( dwNode == g_cMpuBasApp.m_dwMcuNode ) // ����
    {
        mpulog( MPU_INFO, "[DaemonProcOspDisconnect] McuNode.A disconnect\n" );
        g_cMpuBasApp.FreeStatusDataA();
        SetTimer( EV_BAS_CONNECT, MPU_CONNETC_TIMEOUT );
    }
    else if ( dwNode == g_cMpuBasApp.m_dwMcuNodeB )
    {
        mpulog( MPU_INFO, "[DaemonProcOspDisconnect] McuNode.B disconnect\n" );
        g_cMpuBasApp.FreeStatusDataB();
        SetTimer( EV_BAS_CONNECTB, MPU_CONNETC_TIMEOUT );
    }
	// ������һ����������Mcuȡ��������״̬���ж��Ƿ�ɹ�
	if ( INVALID_NODE != g_cMpuBasApp.m_dwMcuNode || INVALID_NODE != g_cMpuBasApp.m_dwMcuNodeB )
	{
		if ( OspIsValidTcpNode(g_cMpuBasApp.m_dwMcuNode) )
		{
			post( g_cMpuBasApp.m_dwMcuIId, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cMpuBasApp.m_dwMcuNode );           
			mpulog( MPU_INFO, "[DaemonProcOspDisconnect] GetMsStatusReq. McuNode.A\n" );
		}
		else if ( OspIsValidTcpNode(g_cMpuBasApp.m_dwMcuNodeB) )
		{
			post( g_cMpuBasApp.m_dwMcuIIdB, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cMpuBasApp.m_dwMcuNodeB );        
			mpulog( MPU_INFO, "[DaemonProcMcuDisconnect] GetMsStatusReq. McuNode.B\n" );
		}

		SetTimer( EV_BAS_GETMSSTATUS, WAITING_MSSTATUS_TIMEOUT );
		return;
	}
	// �����ڵ㶼�ϣ����״̬��Ϣ
    if ( INVALID_NODE == g_cMpuBasApp.m_dwMcuNode && INVALID_NODE == g_cMpuBasApp.m_dwMcuNodeB )
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
    2008/12/09  4.5         ���㻪          ����
====================================================================*/
void CMpuBasInst::DeamonProcModSendIpAddr( CMessage* const pMsg )
{
    if ( NULL == pMsg->content )
    {
        mpulog( MPU_CRIT, " The pointer cannot be Null (MsgStartVidMixProc)\n" );
        return;
    } 

	u32 dwSendIP = *(u32*)pMsg->content;
	g_cMpuBasApp.m_dwMcuRcvIp = ntohl(dwSendIP);
	u8 byInitNum = 0;
	if(!GetBasInitChnlNumAcd2WorkMode(g_cMpuBasApp.m_byWorkMode, byInitNum))
	{
		OspPrintf(TRUE, FALSE, "[DeamonProcModSendIpAddr] GetBasInitChnlNumAcd2WorkMode failed!\n");
		return;
	}

    for (u8 byLoop = 1; byLoop <= byInitNum/*MAXNUM_MPUBAS_CHANNEL*/; byLoop++ )
    {
        post( MAKEIID(GetAppID(), byLoop), EV_BAS_MODSENDADDR);
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
void CMpuBasInst::DeamonShowBasDebugInfo()
{
	g_cMpuBasApp.m_tDebugVal.Print();	
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
  2008/12/09  4.5         ����        ����
====================================================================*/
void CMpuBasInst::ProcInitBas()
{
    BOOL bRet = TRUE;

	u8 byInitNum = 0;
	if(!GetBasOutChnlNumAcd2WorkMode(g_cMpuBasApp.m_byWorkMode, byInitNum))
	{
		printf("[DaemonProcRegAck] GetBasOutChnlNumAcd2WorkMode failed!");
		OspPrintf(TRUE, FALSE, "[DaemonProcRegAck] GetBasOutChnlNumAcd2WorkMode failed!\n");
		return;
	}
	
	u16 wDstPort = g_cMpuBasApp.m_wMcuRcvStartPort + ( GetInsID() - 1 ) * PORTSPAN * byInitNum/*2*/;
    if ( m_pAdpCroup == NULL )
    {
        // start new       
        m_pAdpCroup = new CMpuBasAdpGroup;
/*lint -save -e734*/
        u16 wRecvPort = g_cMpuBasApp.m_tCfg.wRcvStartPort + ( GetInsID() - 1 ) * PORTSPAN * byInitNum/*2*/;	
		printf("[ProcInitBas] Chn.%d , RcvPort:%u, DstPort:%u!\n", GetInsID()-1, wRecvPort, wDstPort);
		OspPrintf(TRUE, FALSE, "[ProcInitBas] Chn.%d , RcvPort:%u, DstPort:%u!\n", GetInsID()-1, wRecvPort, wDstPort);

        bRet = m_pAdpCroup->Create( /*MAX_VIDEO_FRAME_SIZE, */
                                    wRecvPort + 1,
                                    wRecvPort,
                                    g_cMpuBasApp.m_dwMcuRcvIp,
                                    wDstPort,
								    (u32)this,
                                    GetInsID()-1 );
/*lint -restore*/
        // new frm queue
        m_ptFrmQueue = new TRtpPkQueue;

        if (NULL != m_ptFrmQueue)
        {
            m_ptFrmQueue->Init(g_cMpuBasApp.GetAudioDeferNum());
        }

		//  [11/9/2009 pengjie] ���üӺڱ߲ñ�	
		printf( "[MPUdebug][ProcInitBas][SetResizeMode] Mode = %d \n", g_cMpuBasApp.GetVidEncHWMode() );
		if( (u16)Codec_Success != m_pAdpCroup->m_cAptGrp.SetResizeMode( (u32)g_cMpuBasApp.GetVidEncHWMode() ) )
		{
			printf( "[MPUdebug][ProcInitBas][SetResizeMode] Mode = %d Failed\n", g_cMpuBasApp.GetVidEncHWMode() );
		}

	}
	else
	{
		m_pAdpCroup->ResetDestPort( wDstPort);
		m_pAdpCroup->ModNetSndIpAddr(g_cMpuBasApp.m_dwMcuRcvIp);
		OspPrintf(TRUE, FALSE, "[ProcInitBas] set new DstIp:%s,new DstPort:%u!\n",StrOfIP(g_cMpuBasApp.m_dwMcuRcvIp), wDstPort);
	}

    if (bRet)
    {
        NextState( (u32)READY );
       	OspPrintf(TRUE, FALSE, "[ProcInitBas]set EV_BAS_NEEDIFRAME_TIMER : %d\n", g_cMpuBasApp.GetIframeInterval());
        SetTimer( EV_BAS_NEEDIFRAME_TIMER, g_cMpuBasApp.GetIframeInterval() );   // ��ʱ��ѯ�������Ƿ���Ҫ�ؼ�֡
    }
  
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
  2008/12/09  4.5         ����        ����
====================================================================*/
void CMpuBasInst::ProcStopBas()
{
    KillTimer( EV_BAS_NEEDIFRAME_TIMER );

    NextState( (u32)IDLE );
    //�������״̬
	memset(&m_tChnInfo.m_tChnStatus, 0, sizeof(THDBasVidChnStatus));
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
  2008/12/09  4.5         ����        ����
====================================================================*/
void CMpuBasInst::SendChnNotif()
{
	u8 byChnIdx = GetInsID() - 1;

    CServMsg cServMsg;
	cServMsg.SetConfId( m_tChnInfo.m_cChnConfId );
  
    m_tChnInfo.m_tChnStatus.SetEqp( g_cMpuBasApp.m_tEqp );
    m_tChnInfo.m_tChnStatus.SetChnIdx( byChnIdx );
    m_tChnInfo.m_tChnStatus.SetStatus( (u8)CurState() );
    
    u8 byChnType = HDBAS_CHNTYPE_MPU;
    cServMsg.SetMsgBody((u8*)&byChnType, sizeof(u8));
    cServMsg.CatMsgBody((u8*)&m_tChnInfo.m_tChnStatus, sizeof(m_tChnInfo.m_tChnStatus));
    
    mpulog( MPU_INFO, "[SendChnNotif] Inst.%d Channel.%u state.%u\n",
               GetInsID(),
               byChnIdx,
               CurState() );
    
	SendMsgToMcu( HDBAS_MCU_CHNNLSTATUS_NOTIF, cServMsg );
    return;
}
/*====================================================================
	����  : SendMsgToMcu
	����  : ��MCU������Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� void
    �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���      �޸�����
  2008/08/04  4.5         ����         ����
====================================================================*/
void CMpuBasInst::SendMsgToMcu( u16 wEvent, CServMsg& cServMsg )
{
    if ( GetInsID() != CInstance::DAEMON )
    {
        cServMsg.SetChnIndex( (u8)GetInsID() - 1 );
    }
    
    if ( g_cMpuBasApp.m_bEmbedMcu || OspIsValidTcpNode(g_cMpuBasApp.m_dwMcuNode) ) 
	{
		post( g_cMpuBasApp.m_dwMcuIId, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), g_cMpuBasApp.m_dwMcuNode );
		mpulog( MPU_INFO, "Send Message %u(%s) to Mcu A\n",
                wEvent, ::OspEventDesc(wEvent) );
		printf( "Send Message %u(%s) to Mcu A\n",
                wEvent, ::OspEventDesc(wEvent) );
	}
	else
	{
		mpulog( MPU_CRIT, "SendMsgToMcuA (event:%s) error \n", ::OspEventDesc( wEvent ));
		printf("SendMsgToMcuA (event:%s) error \n", ::OspEventDesc( wEvent ));
	}

    if ( g_cMpuBasApp.m_bEmbedMcuB || OspIsValidTcpNode(g_cMpuBasApp.m_dwMcuNodeB) )
    {
	    post( g_cMpuBasApp.m_dwMcuIIdB, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), g_cMpuBasApp.m_dwMcuNodeB );
		mpulog( MPU_INFO, "Send Message %u(%s) to Mcu B\n",
                wEvent, ::OspEventDesc(wEvent) );
		printf( "Send Message %u(%s) to Mcu B\n",
                wEvent, ::OspEventDesc(wEvent) );
    }
    else
    {
		mpulog( MPU_CRIT, "SendMsgToMcuB (event:%s) error \n", ::OspEventDesc( wEvent ));
		printf("SendMsgToMcuA (event:%s) error \n", ::OspEventDesc( wEvent ));
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
  2008/12/09  4.5         ����        ����
====================================================================*/
void CMpuBasInst::ProcStartAdptReq( CServMsg& cServMsg )
{   
    u8 byChnIdx = (u8)GetInsID() - 1;

    mpulog( MPU_INFO, "[ProcStartAdptReq] Start adp, The channel is %d\n", byChnIdx );

	if( NULL == m_pAdpCroup )
	{
        cServMsg.SetErrorCode( ERR_BAS_OPMAP );
		cServMsg.SetMsgBody( (u8*)&g_cMpuBasApp.m_tEqp, sizeof(g_cMpuBasApp.m_tEqp) );
		SendMsgToMcu( cServMsg.GetEventId() + 2, cServMsg);
		mpulog( MPU_CRIT, "[ProcStartAdptReq] Channel.%d create failed !\n", GetInsID() );	
		return;
	}
	
	if (m_pAdpCroup->IsStart())
	{
	//	m_pAdpCroup->m_cAptGrp.ClearVideo();
	}

    TMediaEncrypt  tMediaEncryptVideo;
	TDoublePayload tDoublePayloadVideo;
	
	//����ģʽ���������������
	u8 byOutPutNum = 0;
	if (TYPE_MPUBAS == g_cMpuBasApp.m_byWorkMode)
	{
		byOutPutNum = MAXNUM_VOUTPUT;
	}
	else if(TYPE_MPUBAS_H == g_cMpuBasApp.m_byWorkMode)
	{
		byOutPutNum = MAXNUM_MPU_H_VOUTPUT;
	}
	else
	{
		OspPrintf(TRUE, FALSE, "[ProcStartAdptReq] unexpected mpu workmode:%d!\n", g_cMpuBasApp.m_byWorkMode);
		return;
	}
	mpulog( MPU_INFO, "[ProcStartAdptReq] BasOutNum is %d\n", byOutPutNum);

	THDAdaptParam  atParm[MAXNUM_MPU_H_VOUTPUT];
	for (u8 byOutIdx = 0; byOutIdx < byOutPutNum; byOutIdx++)
	{
		atParm[byOutIdx] = *(THDAdaptParam*)(cServMsg.GetMsgBody() + byOutIdx * sizeof(THDAdaptParam));
	}

	//zjl
    //THDAdaptParam  atParm[2];
	//atParm[0] = *(THDAdaptParam*)cServMsg.GetMsgBody();
	//atParm[1] = *(THDAdaptParam*)( cServMsg.GetMsgBody() + sizeof(THDAdaptParam) );

	for (u8 byIdx = 0; byIdx < byOutPutNum; byIdx++)
	{
		ConvertToRealFR(atParm[byIdx]);
		ConverToRealParam(GetInsID()-1,byIdx,atParm[byIdx]);
	}

    
    if ( g_cMpuBasApp.IsEnableCheat())
    {
		for (u8 byOutId = 0; byOutId < byOutPutNum; byOutId++)
		{
			if (MEDIA_TYPE_NULL == atParm[byOutId].GetVidType() ||
				0 == atParm[byOutId].GetVidType())
			{  
				continue;
			}
			u16 wBitrate = atParm[byOutId].GetBitrate();
			wBitrate = g_cMpuBasApp.GetDecRateDebug( wBitrate );
			atParm[byOutId].SetBitRate( wBitrate );
		}
		/*zjl
        u16 wBitrate = atParm[0].GetBitrate();
        wBitrate = g_cMpuBasApp.GetDecRateDebug( wBitrate );
        atParm[0].SetBitRate( wBitrate );

        wBitrate = atParm[1].GetBitrate();
        wBitrate = g_cMpuBasApp.GetDecRateDebug( wBitrate );
        atParm[1].SetBitRate( wBitrate );
		*/
    }

    tMediaEncryptVideo = *(TMediaEncrypt*)( cServMsg.GetMsgBody()  + byOutPutNum * sizeof(THDAdaptParam) );
    tDoublePayloadVideo= *(TDoublePayload*)( cServMsg.GetMsgBody() + byOutPutNum * sizeof(THDAdaptParam) + sizeof(TMediaEncrypt) );
	
    u8 byAudDecType = MEDIA_TYPE_NULL;
    if (cServMsg.GetMsgBodyLen() > sizeof(THDAdaptParam) * byOutPutNum + sizeof(TMediaEncrypt) + sizeof(TDoublePayload))
    {
        byAudDecType = *(cServMsg.GetMsgBody() + byOutPutNum * sizeof(THDAdaptParam) + sizeof(TMediaEncrypt)+ sizeof(TDoublePayload));
    }
	
	switch( CurState() )
	{
	case READY:  
	case NORMAL: //������ʱ֧��
		{
            //u16 wRet = 0;
			BOOL32 bCreatSendObj = FALSE;
			TVideoEncParam atVidEncPar[MAXNUM_MPU_H_VOUTPUT];


			// fxh Ŀǰmcu���ñ�׼4CIF(704* 576), ������MPEG4,��ȡD1(720 * 576)
			for(u8 byOutChnlIdx = 0; byOutChnlIdx < byOutPutNum; byOutChnlIdx++)
			{
				if (MEDIA_TYPE_MP4 == atParm[byOutChnlIdx].GetVidType() &&
					704 == atParm[byOutChnlIdx].GetWidth() &&
					576 == atParm[byOutChnlIdx].GetHeight())
				{
					atParm[byOutChnlIdx].SetResolution(720, 576);
				}

			    g_cMpuBasApp.GetDefaultParam( atParm[byOutChnlIdx].GetVidType(), atVidEncPar[byOutChnlIdx] );

				//���õ�һ·�������
				atVidEncPar[byOutChnlIdx].m_wVideoHeight = atParm[byOutChnlIdx].GetHeight();
				atVidEncPar[byOutChnlIdx].m_wVideoWidth  = atParm[byOutChnlIdx].GetWidth();
				atVidEncPar[byOutChnlIdx].m_wBitRate     = atParm[byOutChnlIdx].GetBitrate(); 
				atVidEncPar[byOutChnlIdx].m_byFrameRate  = atParm[byOutChnlIdx].GetFrameRate();

				//zbq[09/04/2008] FIXME: ��ʱ�������Ժ������MCU
				//��Ȼ������
	// 			if ( atVidEncPar[0].m_wVideoWidth == 720 &&
	// 				 atVidEncPar[0].m_wVideoHeight == 576 )
	// 			{
	// 				atVidEncPar[0].m_wVideoWidth = 704;
	// 			}


				//zjl[20100531]��ʱ�ſ�
// 				if ( atVidEncPar[byOutChnlIdx].m_byFrameRate == 30 )
// 				{
// 					atVidEncPar[byOutChnlIdx].m_byFrameRate = 25;
// 				}

				//zbq[10/10/2008] ���ݷֱ��ʵ������ؼ�֡ ���
				if ( (atVidEncPar[byOutChnlIdx].m_wVideoWidth == 1280 &&
					  atVidEncPar[byOutChnlIdx].m_wVideoHeight == 720) ||
					  (atVidEncPar[byOutChnlIdx].m_wVideoWidth == 1920 &&
					   atVidEncPar[byOutChnlIdx].m_wVideoHeight == 1088) ||
					   (atVidEncPar[byOutChnlIdx].m_wVideoWidth == 1920 &&
					   atVidEncPar[byOutChnlIdx].m_wVideoHeight == 544))
				{
					atVidEncPar[byOutChnlIdx].m_byMaxKeyFrameInterval = 3000;
				}
				else if( (atVidEncPar[byOutChnlIdx].m_wVideoWidth == 704 &&
						  atVidEncPar[byOutChnlIdx].m_wVideoHeight == 576) ||
						 (atVidEncPar[byOutChnlIdx].m_wVideoWidth == 720 &&
						  atVidEncPar[byOutChnlIdx].m_wVideoHeight == 576) )
				{
					atVidEncPar[byOutChnlIdx].m_byMaxKeyFrameInterval = 1000;
				}
				else if ( atVidEncPar[byOutChnlIdx].m_wVideoWidth == 352 &&
						  atVidEncPar[byOutChnlIdx].m_wVideoHeight == 288 )
				{
					atVidEncPar[byOutChnlIdx].m_byMaxKeyFrameInterval = 500;
				}

				//������������
				if (MEDIA_TYPE_H264 == atParm[byOutChnlIdx].GetVidType())
				{
					//CIF���ϸ�51��10
					if (atParm[byOutChnlIdx].GetHeight() > 288)
					{
						atVidEncPar[byOutChnlIdx].m_byMaxQuant = 51;
						atVidEncPar[byOutChnlIdx].m_byMinQuant = 10;
					}
					else
					{
						atVidEncPar[byOutChnlIdx].m_byMaxQuant = 45;
						atVidEncPar[byOutChnlIdx].m_byMinQuant = 20;
					}
				}
				else
				{
					atVidEncPar[byOutChnlIdx].m_byMaxQuant = 31;
					atVidEncPar[byOutChnlIdx].m_byMinQuant = 3;
				}

				mpulog( MPU_INFO, "Start Video Enc Param: %u, W*H: %dx%d, Bitrate.%dKbps, FrmRate.%d, MaxKeyFrmInt.%d\n",
								   atParm[byOutChnlIdx].GetVidType(), 
								   atVidEncPar[byOutChnlIdx].m_wVideoWidth, 
								   atVidEncPar[byOutChnlIdx].m_wVideoHeight,
								   atVidEncPar[byOutChnlIdx].m_wBitRate,
								   atVidEncPar[byOutChnlIdx].m_byFrameRate,
								   atVidEncPar[byOutChnlIdx].m_byMaxKeyFrameInterval );
			}

			for (u8 byPriOut = 0; byPriOut < byOutPutNum;  byPriOut++)
			{
				if (atParm[byPriOut].GetVidType() != 
					m_tChnInfo.m_tChnStatus.GetOutputVidParam(byPriOut)->GetVidType())
				{
					bCreatSendObj = TRUE;
					OspPrintf(TRUE, FALSE, "[ProcStartAdptReq] CreatSendObj again!\n");
					break;
				}
			}

			if ( TRUE == m_pAdpCroup->SetVideoEncParam(atVidEncPar, byChnIdx, byOutPutNum, bCreatSendObj))
			{
				// ��¼ͨ��״̬
				for (u8 byOut = 0; byOut < byOutPutNum;  byOut++)
				{
					THDAdaptParam* ptAdpParam = m_tChnInfo.m_tChnStatus.GetOutputVidParam(byOut);
					if (NULL != ptAdpParam)
					{
						ptAdpParam->SetAudType( atParm[byOut].GetAudType() );
						ptAdpParam->SetVidType( atParm[byOut].GetVidType() );
						ptAdpParam->SetBitRate( atParm[byOut].GetBitrate() );
						ptAdpParam->SetResolution( atParm[byOut].GetWidth(), atParm[byOut].GetHeight() );
						ptAdpParam->SetFrameRate( atParm[byOut].GetFrameRate() );
					}
				}
//zjl
// 	    		THDAdaptParam* ptAdpParam = m_tChnInfo.m_tChnStatus.GetOutputVidParam(0);
// 
// 	    		if ( NULL != ptAdpParam )
// 				{
// 		    		ptAdpParam->SetAudType( atParm[0].GetAudType() );
// 		    		ptAdpParam->SetVidType( atParm[0].GetVidType() );
// 		    		ptAdpParam->SetBitRate( atParm[0].GetBitrate() );
// 			    	ptAdpParam->SetResolution( atParm[0].GetWidth(), atParm[0].GetHeight() );
// 					ptAdpParam->SetFrameRate( atParm[0].GetFrameRate() );
// 				}
// 
//                 THDAdaptParam* ptAdpParam1 = m_tChnInfo.m_tChnStatus.GetOutputVidParam(1);
//                 
//                 if ( NULL != ptAdpParam1 )
//                 {
//                     ptAdpParam1->SetAudType( atParm[1].GetAudType() );
//                     ptAdpParam1->SetVidType( atParm[1].GetVidType() );
//                     ptAdpParam1->SetBitRate( atParm[1].GetBitrate() );
//                     ptAdpParam1->SetResolution( atParm[1].GetWidth(), atParm[1].GetHeight() );
// 					ptAdpParam1->SetFrameRate( atParm[1].GetFrameRate() );
// 				}
			}

            //��Ƶ��ת��Net��������
			//mpu_h��Ƶ��ʱ���账��
            if(!m_pAdpCroup->SetAudioParam( byAudDecType, byChnIdx ) )
			{
				OspPrintf(TRUE, FALSE, "SetAudioParam failed!\n");
			}

			OspPrintf(TRUE, FALSE, "ActivePayLoad:%u, RealPayLoad:%u!\n", tDoublePayloadVideo.GetActivePayload(), tDoublePayloadVideo.GetRealPayLoad());
           
			u8 byEncryptMode = tMediaEncryptVideo.GetEncryptMode();
			u8 abySndActPt[MAXNUM_MPU_H_VOUTPUT];
			u8 byLoop = 0;
            if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
            {
                mpulog( MPU_CRIT, "CONF_ENCRYPTMODE_NONE == byEncryptMode\n" );
				for(;byLoop < byOutPutNum; byLoop ++)
				{
					abySndActPt[byLoop] = atParm[byLoop].GetVidActiveType();
					if(atParm[byLoop].GetVidType() == MEDIA_TYPE_H263 || atParm[byLoop].GetVidType() == MEDIA_TYPE_MP4 )
					{
						abySndActPt[byLoop] = 0;
					}
					OspPrintf(TRUE, FALSE, "abySndActPt[%d] is: %u!\n", byLoop, abySndActPt[byLoop]);
				}
				//�ӿڵ�������ָ��abySndActPt����Ч���ͨ����byOutPutNum
                m_pAdpCroup->SetKeyandPT( NULL, 0, 0,
										  abySndActPt, byOutPutNum,
                                          tDoublePayloadVideo.GetActivePayload(), 
										  tDoublePayloadVideo.GetRealPayLoad());

				/*mpulog( MPU_INFO, "abySndActPt[0] is: %u, abySndActPt[1] is: %u\n",abySndActPt[0], abySndActPt[1] );*/
//                 m_pAdpCroup->SetKeyandPT( NULL, 0, 0,
//                                           tDoublePayloadVideo.GetActivePayload(), tDoublePayloadVideo.GetRealPayLoad(),
//                                           abySndActPt[0], abySndActPt[1] );
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
                m_pAdpCroup->SetKeyandPT( abyKeyBuf, byKenLen, byEncryptMode, 
										  abySndActPt, byOutPutNum,
                                          tDoublePayloadVideo.GetActivePayload(), 
										  tDoublePayloadVideo.GetRealPayLoad());
			
                                         
//                 m_pAdpCroup->SetKeyandPT( abyKeyBuf, byKenLen, byEncryptMode, 
//                                           tDoublePayloadVideo.GetActivePayload(), tDoublePayloadVideo.GetRealPayLoad(),
//                                           atParm[0].GetVidActiveType(), atParm[1].GetVidActiveType() );
            }

			//���ü��� �ӿ�ͳһ���з��Ͷ������Կ����
			if(!m_pAdpCroup->SetVidEncryptKey( tMediaEncryptVideo, byOutPutNum) )
			{
				mpulog(MPU_CRIT, "[ProcStartAdptReq] SetVidEncryptKey failed\n");
			}

			//���ü��� ������
			//m_pAdpCroup->SetSecVidEncryptKey( tMediaEncryptVideo );

			//���ö�̬�غ�
			if ( !m_pAdpCroup->SetVideoActivePT( tDoublePayloadVideo.GetActivePayload(), tDoublePayloadVideo.GetRealPayLoad() ) )
			{
				mpulog(MPU_CRIT, "[ProcStartAdptReq] SetVideoActivePT failed\n");
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
			OspPrintf(TRUE, FALSE, "[ProcStartAdptReq] IsNeedPrs:%d!\n", bIsNeedPrs);
            if ( bIsNeedPrs/*atParm[0].IsNeedbyPrs()*/ )		//�Ƿ���Ҫ���ش��� 1: �ش�, 0: ���ش�
			{		
				mpulog( MPU_INFO, "[ProcStartAdptReq] Bas is needed by Prs!\n" );
                tNetRSParam.m_wFirstTimeSpan  = g_cMpuBasApp.m_tPrsTimeSpan.m_wFirstTimeSpan;
				tNetRSParam.m_wSecondTimeSpan = g_cMpuBasApp.m_tPrsTimeSpan.m_wSecondTimeSpan;
				tNetRSParam.m_wThirdTimeSpan  = g_cMpuBasApp.m_tPrsTimeSpan.m_wThirdTimeSpan;
				tNetRSParam.m_wRejectTimeSpan = g_cMpuBasApp.m_tPrsTimeSpan.m_wRejectTimeSpan;
				//zjl ͳһ�ӿ�SetNetSendFeedbackVideoParam�������з���ͨ�����ش�����
				if( !m_pAdpCroup->SetNetSendFeedbackVideoParam( 2000, byOutPutNum, TRUE ))
				{
					mpulog( MPU_CRIT, "[ProcStartAdptReq]SetNetSendFeedbackVideoParam failed\n ");
				}
				if ( !m_pAdpCroup->SetNetRecvFeedbackVideoParam( tNetRSParam, TRUE ) )
				{
					mpulog( MPU_CRIT, "[ProcStartAdptReq]SetNetRecvFeedbackVideoParam failed\n ");
				}
			}
			else
			{
				mpulog( MPU_INFO, "Bas is not needed by Prs!\n" );
                tNetRSParam.m_wFirstTimeSpan  = 0;
				tNetRSParam.m_wSecondTimeSpan = 0;
				tNetRSParam.m_wThirdTimeSpan  = 0;
				tNetRSParam.m_wRejectTimeSpan = 0;
				if (!m_pAdpCroup->SetNetSendFeedbackVideoParam( 2000, byOutPutNum, FALSE ))
				{
					mpulog( MPU_CRIT, "[ProcStartAdptReq]SetNetSendFeedbackVideoParam failed\n ");
				}
				//m_pAdpCroup->SetNetSecSendFeedbackVideoParam( 2000, FALSE );
				if (!m_pAdpCroup->SetNetRecvFeedbackVideoParam( tNetRSParam, FALSE ))
				{
					mpulog( MPU_CRIT, "[ProcStartAdptReq]SetNetRecvFeedbackVideoParam failed\n ");

				}
			}

			//��ʱ��������
			if( !m_pAdpCroup->IsStart() )
			{
				BOOL bStartOk;
                bStartOk = m_pAdpCroup->StartAdapter( TRUE );
				if ( !bStartOk )
				{
					cServMsg.SetErrorCode( ERR_BAS_OPMAP );
					cServMsg.SetMsgBody( (u8*)&g_cMpuBasApp.m_tEqp, sizeof(g_cMpuBasApp.m_tEqp) );
					SendMsgToMcu( cServMsg.GetEventId() + 2, cServMsg );
					return;
				}
			}

            // ƽ������
			// [pengjie 2010/12/30] ƽ���߼�����
			m_pAdpCroup->SetSmoothSendRule( g_cMpuBasApp.m_bIsUseSmoothSend );
			
            cServMsg.SetMsgBody( (u8*)&g_cMpuBasApp.m_tEqp, sizeof(g_cMpuBasApp.m_tEqp) );
            SendMsgToMcu( cServMsg.GetEventId() + 1, cServMsg );
            
            NextState( (u32)NORMAL );            
            SendChnNotif();

			
			//[nizhijun 2011/03/21] BAS�����ı���Ҫ��1����ؼ�֡����
			KillTimer( EV_BAS_NEEDIFRAME_TIMER );
			mpulog(MPU_INFO, "[ProcStartAdptReq]reset EV_BAS_NEEDIFRAME_TIMER from %d to: %d\n", 
				g_cMpuBasApp.GetIframeInterval(), CHECK_IFRAME_INTERVAL);
			m_byIframeReqCount = 0;
			SetTimer( EV_BAS_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL );   // ��ʱ��ѯ�������Ƿ���Ҫ�ؼ�֡
		}
		break;

	default:
		cServMsg.SetErrorCode( ERR_BAS_CHNOTREAD );
		cServMsg.SetMsgBody( (u8*)&g_cMpuBasApp.m_tEqp, sizeof(g_cMpuBasApp.m_tEqp) );
		SendMsgToMcu( cServMsg.GetEventId() + 2, cServMsg );
		mpulog( MPU_CRIT, "the chn.%d in unknown state !\n", byChnIdx );
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
  2008/12/09  4.5         ����        ����
====================================================================*/
void CMpuBasInst::ProcChangeAdptCmd( CServMsg& cServMsg )
{
    u8 byChnIdx = (u8)GetInsID() - 1;
	
	if ( NULL == m_pAdpCroup )
    {
		mpulog( MPU_CRIT, "[ProcChangeAdptCmd] Channel.%d create failed !\n", byChnIdx );	
		return;
	}

	if (m_pAdpCroup->IsStart())
	{
	//	m_pAdpCroup->m_cAptGrp.ClearVideo();
	}

    TMediaEncrypt tMediaEncryptVideo;
	TDoublePayload tDoublePayloadVideo;

    mpulog( MPU_INFO, "[ProcChangeAdptCmd] Adjust adp param, The channel is: %d\n", byChnIdx );

	//����ģʽ���������������
	u8 byOutPutNum = 0;
	if (TYPE_MPUBAS == g_cMpuBasApp.m_byWorkMode)
	{
		byOutPutNum = MAXNUM_VOUTPUT;
	}
	else if(TYPE_MPUBAS_H == g_cMpuBasApp.m_byWorkMode)
	{
		byOutPutNum = MAXNUM_MPU_H_VOUTPUT;
	}
	else
	{
		OspPrintf(TRUE, FALSE, "[ProcChangeAdptCmd] unexpected mpu workmode:%d!\n", g_cMpuBasApp.m_byWorkMode);
		return;
	}
	mpulog( MPU_INFO, "[ProcStartAdptReq] BasOutNum is %d\n", byOutPutNum);

    THDAdaptParam  atParm[MAXNUM_MPU_H_VOUTPUT];
	for (u8 byOutIdx = 0; byOutIdx < byOutPutNum; byOutIdx++)
	{
		atParm[byOutIdx] = *(THDAdaptParam*)(cServMsg.GetMsgBody() + byOutIdx * sizeof(THDAdaptParam));
	}

// 	atParm[0]  = *(THDAdaptParam*)cServMsg.GetMsgBody();
// 	atParm[1] = *(THDAdaptParam*)( cServMsg.GetMsgBody() + sizeof(THDAdaptParam) );

	for (u8 byIdx = 0; byIdx < byOutPutNum; byIdx++)
	{
		ConvertToRealFR(atParm[byIdx]);
		ConverToRealParam(GetInsID()-1,byIdx,atParm[byIdx]);
	}
	
    tMediaEncryptVideo = *(TMediaEncrypt*)(cServMsg.GetMsgBody()  + byOutPutNum * sizeof(THDAdaptParam));
    tDoublePayloadVideo= *(TDoublePayload*)(cServMsg.GetMsgBody() + byOutPutNum * sizeof(THDAdaptParam) + sizeof(TMediaEncrypt));

    u8 byAudDecType = MEDIA_TYPE_NULL;
    if (cServMsg.GetMsgBodyLen() > sizeof(THDAdaptParam) * byOutPutNum + sizeof(TMediaEncrypt) + sizeof(TDoublePayload))
    {
        byAudDecType = *(cServMsg.GetMsgBody() + byOutPutNum * sizeof(THDAdaptParam) + sizeof(TMediaEncrypt)+ sizeof(TDoublePayload));
    }


    //֧����������
    if ( g_cMpuBasApp.IsEnableCheat() )
    {
		for (u8 byOutId = 0; byOutId < byOutPutNum; byOutId++)
		{
			if (MEDIA_TYPE_NULL == atParm[byOutId].GetVidType() ||
				0 == atParm[byOutId].GetVidType())
			{  
				continue;
			}
			u16 wBitrate = atParm[byOutId].GetBitrate();
			wBitrate = g_cMpuBasApp.GetDecRateDebug( wBitrate );
			atParm[byOutId].SetBitRate( wBitrate );
		}
//         u16 wBitrate = atParm[0].GetBitrate();
//         wBitrate = g_cMpuBasApp.GetDecRateDebug( wBitrate );
//         atParm[0].SetBitRate(wBitrate);
// 
//         wBitrate = atParm[1].GetBitrate();
//         wBitrate = g_cMpuBasApp.GetDecRateDebug( wBitrate );
//         atParm[1].SetBitRate(wBitrate);
    }
    
	switch( CurState() ) 
	{
	case NORMAL:
		{
			BOOL32 bCreatSendObj = FALSE;
            TVideoEncParam atVidParam[MAXNUM_MPU_H_VOUTPUT];
            memset(&atVidParam[0], 0, sizeof(TVideoEncParam));
 
			// fxh Ŀǰmcu���ñ�׼4CIF(704* 576), ������MPEG4,��ȡD1(720 * 576)
			for(u8 byOutChnlIdx = 0; byOutChnlIdx < byOutPutNum; byOutChnlIdx++)
			{
				if (MEDIA_TYPE_MP4 == atParm[byOutChnlIdx].GetVidType() &&
					704 == atParm[byOutChnlIdx].GetWidth() &&
					576 == atParm[byOutChnlIdx].GetHeight())
				{
					atParm[byOutChnlIdx].SetResolution(720, 576);
				}	
				
			    g_cMpuBasApp.GetDefaultParam( atParm[byOutChnlIdx].GetVidType(), atVidParam[byOutChnlIdx] );

				//���õ�һ·�������
				atVidParam[byOutChnlIdx].m_wVideoHeight = atParm[byOutChnlIdx].GetHeight();
				atVidParam[byOutChnlIdx].m_wVideoWidth  = atParm[byOutChnlIdx].GetWidth();
				atVidParam[byOutChnlIdx].m_wBitRate     = atParm[byOutChnlIdx].GetBitrate(); 
				atVidParam[byOutChnlIdx].m_byFrameRate  = atParm[byOutChnlIdx].GetFrameRate();

				//zbq[09/04/2008] FIXME: ��ʱ�������Ժ������MCU
				//��Ȼ������
	// 			if ( atVidEncPar[0].m_wVideoWidth == 720 &&
	// 				 atVidEncPar[0].m_wVideoHeight == 576 )
	// 			{
	// 				atVidEncPar[0].m_wVideoWidth = 704;
	// 			}


				if ( atVidParam[byOutChnlIdx].m_byFrameRate == 30 )
				{
					atVidParam[byOutChnlIdx].m_byFrameRate = 25;
				}

				//zbq[10/10/2008] ���ݷֱ��ʵ������ؼ�֡ ���
				if ( atVidParam[byOutChnlIdx].m_wVideoWidth == 1280 &&
					 atVidParam[byOutChnlIdx].m_wVideoHeight == 720 )
				{
					atVidParam[byOutChnlIdx].m_byMaxKeyFrameInterval = 3000;
				}
				else if( (atVidParam[byOutChnlIdx].m_wVideoWidth == 704 &&
						  atVidParam[byOutChnlIdx].m_wVideoHeight == 576) ||
						 (atVidParam[byOutChnlIdx].m_wVideoWidth == 720 &&
						  atVidParam[byOutChnlIdx].m_wVideoHeight == 576) )
				{
					atVidParam[byOutChnlIdx].m_byMaxKeyFrameInterval = 1000;
				}
				else if ( atVidParam[byOutChnlIdx].m_wVideoWidth == 352 &&
						  atVidParam[byOutChnlIdx].m_wVideoHeight == 288 )
				{
					atVidParam[byOutChnlIdx].m_byMaxKeyFrameInterval = 500;
				}

				//������������
				if (MEDIA_TYPE_H264 == atParm[byOutChnlIdx].GetVidType())
				{
					//CIF���ϸ�51��10
					if (atParm[byOutChnlIdx].GetHeight() > 288)
					{
						atVidParam[byOutChnlIdx].m_byMaxQuant = 51;
						atVidParam[byOutChnlIdx].m_byMinQuant = 10;
					}
					else
					{
						atVidParam[byOutChnlIdx].m_byMaxQuant = 45;
						atVidParam[byOutChnlIdx].m_byMinQuant = 20;
					}
				}
				else
				{
					atVidParam[byOutChnlIdx].m_byMaxQuant = 31;
					atVidParam[byOutChnlIdx].m_byMinQuant = 3;
				}

				mpulog( MPU_INFO, "[ProcChangeAdptCmd]Change Video Enc Param: %u, W*H: %dx%d, Bitrate.%dKbps, FrmRate.%d, MaxKeyFrmInt.%d\n",
								   atParm[byOutChnlIdx].GetVidType(), 
								   atVidParam[byOutChnlIdx].m_wVideoWidth, 
								   atVidParam[byOutChnlIdx].m_wVideoHeight,
								   atVidParam[byOutChnlIdx].m_wBitRate,
								   atVidParam[byOutChnlIdx].m_byFrameRate,
								   atVidParam[byOutChnlIdx].m_byMaxKeyFrameInterval );
			}            

            
            {
				for (u8 byPriOut = 0; byPriOut < byOutPutNum;  byPriOut++)
				{
					if (atParm[byPriOut].GetVidType() != 
												m_tChnInfo.m_tChnStatus.GetOutputVidParam(byPriOut)->GetVidType())
					{
						bCreatSendObj = TRUE;
						OspPrintf(TRUE, FALSE, "[ProcChangeAdptCmd] CreatSendObj again!\n");
						break;
					}
				}

                //zbq[10/09/2008] �ݴ���
                BOOL bRet = m_pAdpCroup->SetVideoEncParam( atVidParam, byChnIdx, byOutPutNum, bCreatSendObj/*!(m_pAdpCroup->IsStart())*/ );  
                if ( TRUE != bRet )
			    {
				    mpulog( MPU_INFO, "Change Video Enc Param failed!(%d)\n", bRet );
                    return;
			    }

                //��Ƶ��ת��Net��������
                if (!m_pAdpCroup->SetAudioParam( byAudDecType, byChnIdx ))
				{
					mpulog( MPU_CRIT, "[ProcChangeAdptCmd]SetAudioParam failed\n");
				}

				// ��¼ͨ��״̬
	    		//THDAdaptParam* ptAdpParam = m_tChnInfo.m_tChnStatus.GetOutputVidParam(0);
				
				// ��¼ͨ��״̬
				for (u8 byOut = 0; byOut < byOutPutNum;  byOut++)
				{
					THDAdaptParam* ptAdpParam = m_tChnInfo.m_tChnStatus.GetOutputVidParam(byOut);
					if (NULL != ptAdpParam)
					{
						ptAdpParam->SetAudType( atParm[byOut].GetAudType() );
						ptAdpParam->SetVidType( atParm[byOut].GetVidType() );
						ptAdpParam->SetBitRate( atParm[byOut].GetBitrate() );
						ptAdpParam->SetResolution( atParm[byOut].GetWidth(), atParm[byOut].GetHeight() );
						ptAdpParam->SetFrameRate( atParm[byOut].GetFrameRate() );
					}
				}

// 	    		if ( NULL != ptAdpParam )
// 				{
// 		    		ptAdpParam->SetAudType( atParm[0].GetAudType() );
// 		    		ptAdpParam->SetVidType( atParm[0].GetVidType() );
// 		    		ptAdpParam->SetBitRate( atParm[0].GetBitrate() );
// 			    	ptAdpParam->SetResolution( atParm[0].GetWidth(), atParm[0].GetHeight() );
// 					ptAdpParam->SetFrameRate( atParm[0].GetFrameRate() );
// 				}
// 
//                 THDAdaptParam* ptAdpParam1 = m_tChnInfo.m_tChnStatus.GetOutputVidParam(1);
//                 
//                 if ( NULL != ptAdpParam1 )
//                 {
//                     ptAdpParam1->SetAudType( atParm[1].GetAudType() );
//                     ptAdpParam1->SetVidType( atParm[1].GetVidType() );
//                     ptAdpParam1->SetBitRate( atParm[1].GetBitrate() );
//                     ptAdpParam1->SetResolution( atParm[1].GetWidth(), atParm[1].GetHeight() );
// 					ptAdpParam1->SetFrameRate( atParm[1].GetFrameRate() );
// 				}

				OspPrintf(TRUE, FALSE, "ActivePayLoad:%d, RealPayLoad:%d!\n", tDoublePayloadVideo.GetActivePayload(), tDoublePayloadVideo.GetRealPayLoad());

				u8 byEncryptMode = tMediaEncryptVideo.GetEncryptMode();
				u8 abySndActPt[MAXNUM_MPU_H_VOUTPUT];
				u8 byLoop = 0;
				if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
				{
					mpulog( MPU_CRIT, "CONF_ENCRYPTMODE_NONE == byEncryptMode\n" );
					for(;byLoop < byOutPutNum; byLoop ++)
					{
						abySndActPt[byLoop] = atParm[byLoop].GetVidActiveType();
						if(atParm[byLoop].GetVidType() == MEDIA_TYPE_H263 || atParm[byLoop].GetVidType() == MEDIA_TYPE_MP4 )
						{
							abySndActPt[byLoop] = 0;
						}
						OspPrintf(TRUE, FALSE, "abySndActPt[%d] is: %u\n", byLoop, abySndActPt[byLoop]);
					}
					m_pAdpCroup->SetKeyandPT( NULL, 0, 0,
											  abySndActPt, byOutPutNum,
											  tDoublePayloadVideo.GetActivePayload(), tDoublePayloadVideo.GetRealPayLoad());


// 					m_pAdpCroup->SetKeyandPT( NULL, 0, 0,
// 											  tDoublePayloadVideo.GetActivePayload(), tDoublePayloadVideo.GetRealPayLoad(),
// 											  abySndActPt[0], abySndActPt[1] );
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
					m_pAdpCroup->SetKeyandPT( abyKeyBuf, byKenLen, byEncryptMode, 
											  abySndActPt, byOutPutNum,
											  tDoublePayloadVideo.GetActivePayload(), 
										      tDoublePayloadVideo.GetRealPayLoad());

// 					m_pAdpCroup->SetKeyandPT( abyKeyBuf, byKenLen, byEncryptMode, 
// 											  tDoublePayloadVideo.GetActivePayload(), tDoublePayloadVideo.GetRealPayLoad(),
// 											  atParm[0].GetVidActiveType(), atParm[1].GetVidActiveType() );
				}

				//���ü���
				if( !m_pAdpCroup->SetVidEncryptKey( tMediaEncryptVideo, byOutPutNum) )
				{
					mpulog(MPU_CRIT, "[ProcChangeAdptCmd] SetVidEncryptKey failed\n");
				}
               
                //���ü���
                //m_pAdpCroup->SetSecVidEncryptKey( tMediaEncryptVideo );
                
                //���ö�̬�غ�
                if (m_pAdpCroup->SetVideoActivePT( tDoublePayloadVideo.GetActivePayload(), tDoublePayloadVideo.GetRealPayLoad() ) )
				{
					mpulog(MPU_CRIT, "[ProcChangeAdptCmd] SetVideoActivePT failed\n");
				}

				if ( m_pAdpCroup->IsStart() == FALSE )
				{
					if( !m_pAdpCroup->StartAdapter( TRUE ) )
					{
						mpulog(MPU_CRIT, "[ProcChangeAdptCmd] StartAdapter failed\n");
						return;
					}
				}

                // ���������ش�
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
				OspPrintf(TRUE, FALSE, "[ProcChangeAdptCmd] IsNeedPrs:%d!\n", bIsNeedPrs);
                if ( bIsNeedPrs/*atParm[0].IsNeedbyPrs()*/ )		//�Ƿ���Ҫ���ش��� 1: �ش�, 0: ���ش�
                {		
                    mpulog( MPU_INFO, "Bas is needed by Prs!\n" );
                    tNetRSParam.m_wFirstTimeSpan  = g_cMpuBasApp.m_tPrsTimeSpan.m_wFirstTimeSpan;
                    tNetRSParam.m_wSecondTimeSpan = g_cMpuBasApp.m_tPrsTimeSpan.m_wSecondTimeSpan;
                    tNetRSParam.m_wThirdTimeSpan  = g_cMpuBasApp.m_tPrsTimeSpan.m_wThirdTimeSpan;
                    tNetRSParam.m_wRejectTimeSpan = g_cMpuBasApp.m_tPrsTimeSpan.m_wRejectTimeSpan;
                    if( !m_pAdpCroup->SetNetSendFeedbackVideoParam( 2000, byOutPutNum, TRUE ) )
					{
						mpulog(MPU_CRIT, "[ProcChangeAdptCmd]SetNetSendFeedbackVideoParam failed\n");
					}
                    //m_pAdpCroup->SetNetSecSendFeedbackVideoParam( 2000, TRUE );
                    if( !m_pAdpCroup->SetNetRecvFeedbackVideoParam( tNetRSParam, TRUE ) )
					{
						mpulog(MPU_CRIT, "[ProcChangeAdptCmd]SetNetRecvFeedbackVideoParam failed\n");
					}
                }
                else
                {
                    mpulog( MPU_INFO, "Bas is not needed by Prs!\n" );
                    tNetRSParam.m_wFirstTimeSpan  = 0;
                    tNetRSParam.m_wSecondTimeSpan = 0;
                    tNetRSParam.m_wThirdTimeSpan  = 0;
                    tNetRSParam.m_wRejectTimeSpan = 0;
                    if (!m_pAdpCroup->SetNetSendFeedbackVideoParam( 2000, byOutPutNum, FALSE ))
					{
						mpulog(MPU_CRIT, "[ProcChangeAdptCmd]SetNetSendFeedbackVideoParam failed\n");
					}
                    //m_pAdpCroup->SetNetSecSendFeedbackVideoParam( 2000, FALSE );
                    if (!m_pAdpCroup->SetNetRecvFeedbackVideoParam( tNetRSParam, FALSE ))
					{
						mpulog(MPU_CRIT, "[ProcChangeAdptCmd]SetNetRecvFeedbackVideoParam failed\n");
					}
			    }
	
				// ƽ������
				// [pengjie 2010/12/30] ƽ���߼�����
				m_pAdpCroup->SetSmoothSendRule( g_cMpuBasApp.m_bIsUseSmoothSend );
            }   
			
            cServMsg.SetMsgBody( (u8*)&g_cMpuBasApp.m_tEqp, sizeof(g_cMpuBasApp.m_tEqp) );

			//CMD msg no reply
            //SendMsgToMcu(cServMsg.GetEventId() + 2, cServMsg);

            //change����Ҫ��״̬�ϸ棬��Ϊ�Ѿ���running
            //SendChnNotif();

			//[nizhijun 2011/03/21] BAS�����ı���Ҫ��1����ؼ�֡����
			KillTimer( EV_BAS_NEEDIFRAME_TIMER );
			mpulog(MPU_INFO, "[ProcChangeAdptCmd]reset EV_BAS_NEEDIFRAME_TIMER from %d to: %d\n", 
				g_cMpuBasApp.GetIframeInterval(), CHECK_IFRAME_INTERVAL);
			m_byIframeReqCount = 0;
			SetTimer( EV_BAS_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL );   // ��ʱ��ѯ�������Ƿ���Ҫ�ؼ�֡
		}
		break;

	default:
        cServMsg.SetErrorCode( ERR_BAS_CHNOTREAD );
        cServMsg.SetMsgBody( (u8*)&g_cMpuBasApp.m_tEqp,sizeof(g_cMpuBasApp.m_tEqp) );

		//CMD msg no reply
        //SendMsgToMcu(cServMsg.GetEventId() + 2, cServMsg);
        mpulog(MPU_CRIT, "the chn.%d is not runing ,refuse change adap req\n", byChnIdx );		
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
  2008/12/09  4.5         ����        ����
====================================================================*/
void CMpuBasInst::ProcStopAdptReq( CServMsg& cServMsg )
{
    u8 byChnIdx = (u8)GetInsID() - 1;
    mpulog( MPU_INFO, "[ProcStopAdptReq] Stop adp, The channel is %d\n", byChnIdx );

    BOOL wRet;
    CConfId    m_cTmpConfId = cServMsg.GetConfId();

	if ( !(m_tChnInfo.m_cChnConfId == m_cTmpConfId) )
	{
		cServMsg.SetErrorCode( ERR_BAS_OPMAP );
		SendMsgToMcu( cServMsg.GetEventId() + 2, cServMsg );
		mpulog( MPU_CRIT, "[ProcStopAdptReq] Channel.%d unmatch conf id in stop adpt req\n", byChnIdx );
		return;
	}

	switch( CurState() )
	{
	case NORMAL:
        wRet = m_pAdpCroup->StopAdapter();
		
		// [pengjie 2010/12/29] ֹͣ����ʱ�����ǰ��ƽ�����͹���
		m_pAdpCroup->SetSmoothSendRule( FALSE );
		
		//�����Ƶ�������
		if (NULL != m_ptFrmQueue)
		{
			m_ptFrmQueue->Clear();
		}		
        if( TRUE != wRet )
        {	
			cServMsg.SetErrorCode( ERR_BAS_OPMAP );
			SendMsgToMcu( cServMsg.GetEventId() + 2, cServMsg );
			mpulog( MPU_CRIT, "Remove channel %d failed when stop adpt, error: %d\n",
                        byChnIdx, wRet );
			return;				           
        }        		
		memset(&m_tChnInfo.m_tChnStatus, 0, sizeof(THDBasVidChnStatus));
		cServMsg.SetMsgBody( (u8*)&g_cMpuBasApp.m_tEqp, sizeof(g_cMpuBasApp.m_tEqp) );
		SendMsgToMcu( cServMsg.GetEventId() + 1, cServMsg );
        NextState( (u32)READY );
        
        SendChnNotif();
		break;

    case READY:
        {
            cServMsg.SetErrorCode( ERR_BAS_CHREADY );
            cServMsg.SetMsgBody( (u8*)&g_cMpuBasApp.m_tEqp,sizeof(g_cMpuBasApp.m_tEqp) );
            SendMsgToMcu( cServMsg.GetEventId() + 2, cServMsg );
            //return;
        }
		break;

	default:
		cServMsg.SetErrorCode( ERR_BAS_CHNOTREAD );
		cServMsg.SetMsgBody( (u8*)&g_cMpuBasApp.m_tEqp, sizeof(g_cMpuBasApp.m_tEqp) );
		SendMsgToMcu( cServMsg.GetEventId() + 2, cServMsg );
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
  2008/12/09  4.5         ����        ����
====================================================================*/
void CMpuBasInst::ProcFastUpdatePicCmd( /*CServMsg& cServMsg*/ )
{
	printf("[ProcFastUpdatePicCmd for Chn.%d !\n", GetInsID() - 1 );
    mpulog(MPU_INFO, "[ProcFastUpdatePicCmd for Chn.%d !\n", GetInsID() - 1 );

    //u8 byChnIdx = cServMsg.GetChnIndex();
    u32 dwTimeInterval = OspClkRateGet();
    u32 dwLastFUPTick = m_tChnInfo.m_dwLastFUPTick;
    
	//����ģʽ���������������
	u8 byOutPutNum = 0;
	if (TYPE_MPUBAS == g_cMpuBasApp.m_byWorkMode)
	{
		byOutPutNum = MAXNUM_VOUTPUT;
	}
	else if(TYPE_MPUBAS_H == g_cMpuBasApp.m_byWorkMode)
	{
		byOutPutNum = MAXNUM_MPU_H_VOUTPUT;
	}
	else
	{
		OspPrintf(TRUE, FALSE, "[ProcStartAdptReq] unexpected mpu workmode:%d!\n", g_cMpuBasApp.m_byWorkMode);
		return;
	}

    u32 dwCurTick = OspTickGet();	
    if ( dwCurTick - dwLastFUPTick > dwTimeInterval )
    {
        m_tChnInfo.m_dwLastFUPTick = dwCurTick;
        
		if ( NULL != m_pAdpCroup )
		{			
			// Ŀǰ����ؼ�֡��ȡͬʱ������·
			for (u8 byIdx = 0; byIdx < byOutPutNum; byIdx++)
			{
				if (!m_pAdpCroup->SetFastUpdata(byIdx))
				{
					mpulog(MPU_CRIT, "SetFastUpdata(%u) failed \n", byIdx);
				}
			}
//     		m_pAdpCroup->SetFastUpdata(0);
// 			m_pAdpCroup->SetFastUpdata(1);
		}
		else
		{
			printf("m_pAdpGroup for Chnnl.%d cannot set fastupdate due to its not ready\n!", GetInsID()-1 );
			mpulog(MPU_CRIT, "m_pAdpGroup for Chnnl.%d cannot set fastupdate due to its not ready\n!", GetInsID()-1 );
		}

    }
    else
    {
		printf("[ProcFastUpdatePicCmd tick intval is %d - %d !\n", dwCurTick , dwLastFUPTick );
        mpulog(MPU_INFO, "[ProcFastUpdatePicCmd tick intval is %d - %d !\n", dwCurTick , dwLastFUPTick );
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
    2008/12/09     4.5         ����         ����
====================================================================*/
void CMpuBasInst::ProcTimerNeedIFrame()
{
    CServMsg cServMsg;    
    TAdapterChannelStatis tAdapterChannelStatis;    
    memset( &tAdapterChannelStatis, 0, sizeof(TAdapterChannelStatis) );
    static u8 byPrintCount = 1;

	KillTimer(EV_BAS_NEEDIFRAME_TIMER);
    mpulog(MPU_DETAIL, "[ProcTimerNeedIFrame] Chn.%d IFrm check timer!\n", GetInsID()-1);

    if( byPrintCount > 20)
    {
        byPrintCount = 1;
    }
	if ( m_pAdpCroup && m_pAdpCroup->IsStart() )
	{      
        //songkun,20110630,Iframe����TimerOut���Ե���  
        if ( 0 == m_byIframeReqCount )
		{
            if (!m_pAdpCroup->GetVideoChannelStatis( 0, tAdapterChannelStatis ))
            {
                mpulog(MPU_CRIT, "[ProcTimerNeedIFrame] GetVideoChannelStatis failed!\n");
            }
            if( tAdapterChannelStatis.m_bVidCompellingIFrm )
            {
                cServMsg.SetConfId( m_tChnInfo.m_cChnConfId );
                cServMsg.SetChnIndex( GetInsID()-1 );
                cServMsg.SetSrcSsnId( g_cMpuBasApp.m_tCfg.byEqpId );
                SendMsgToMcu( BAS_MCU_NEEDIFRAME_CMD, cServMsg );
                m_byIframeReqCount = (u8)(g_cMpuBasApp.GetIframeInterval() / CHECK_IFRAME_INTERVAL);
    		    mpulog( MPU_INFO, "[ProcTimerNeedIFrame]Bas channel:%d request iframe!!,next time is %u\n", GetInsID() - 1,m_byIframeReqCount);
            }
            else
            {
                mpulog( MPU_INFO, "[ProcTimerNeedIFrame]No iframe request for bas channel :%d\n", GetInsID() - 1);
            }
        }        
	}
    if ( 0 != m_byIframeReqCount)
    {
        --m_byIframeReqCount;                                    
    }
    mpulog(MPU_INFO, "[ProcTimerNeedIFrame]set EV_BAS_NEEDIFRAME_TIMER : %d\n", CHECK_IFRAME_INTERVAL);
    SetTimer( EV_BAS_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL);   // ��ʱ��ѯ�������Ƿ���Ҫ�ؼ�֡

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
2008/12/09     4.5      ����        ����
=============================================================================*/
void CMpuBasInst::ClearInstStatus( void )
{
    //CServMsg cServMsg;
    // ��ֹͣ����
    u8 byInitNum = 0;
    if(!GetBasInitChnlNumAcd2WorkMode(g_cMpuBasApp.m_byWorkMode, byInitNum))
    {
        OspPrintf(TRUE, FALSE, "[ClearInstStatus] GetBasInitChnlNumAcd2WorkMode failed!\n");
        return;
    }	
    
    u8 byLoop;
    for ( byLoop = 1; byLoop <= byInitNum/*MAXNUM_MPUBAS_CHANNEL*/; byLoop++ )
    {
        post( MAKEIID(GetAppID(), byLoop), EV_BAS_QUIT );
    }
    
    NEXTSTATE( (u32)IDLE );   // DAEMON ʵ���������״̬
    g_cMpuBasApp.m_byRegAckNum = 0;
    g_cMpuBasApp.m_dwMpcSSrc = 0;
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
    2008/12/09     4.5         ���㻪         ����
====================================================================*/
void CMpuBasInst::ProcModSndAddr()
{
	if (m_pAdpCroup != NULL)
	{
		m_pAdpCroup->ModNetSndIpAddr(g_cMpuBasApp.m_dwMcuRcvIp);
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
  2008/12/09     4.5      ����        ����
=============================================================================*/
void CMpuBasInst::StatusShow( void )
{
    if ( GetInsID() != CInstance::DAEMON)
    {
        if(m_pAdpCroup)
        {
            m_pAdpCroup->ShowChnInfo();
        }
        else
        {
            OspPrintf(TRUE, FALSE, "m_pAdpCroup is NULL\n");
        }
    }
    
    //OspPrintf( TRUE, FALSE, "[StatusShow] g_cBasApp.m_byMauType.%d\n", g_cBasApp.GetType());
    OspPrintf(TRUE, FALSE, "[StatusShow]CMpuBasInst:%d status:%d\n",GetInsID(),CurState());
    OspPrintf( TRUE, FALSE, "[StatusShow] server for conf:");

    m_tChnInfo.m_cChnConfId.Print();
    
    m_ptFrmQueue->Print();
    return;
}

BOOL32 CMpuBasInst::GetBasInitChnlNumAcd2WorkMode(u8 byWorkMode, u8 &byInitChnlNum)
{
	byInitChnlNum = 0;
	switch (byWorkMode)
	{
	case TYPE_MPUBAS:
		byInitChnlNum = MAXNUM_MPUBAS_CHANNEL;
		break;
	case TYPE_MPUBAS_H:
		byInitChnlNum = MAXNUM_MPUBAS_H_CHANNEL;
		break;
	default:
		OspPrintf(TRUE, FALSE, "[GetBasInitChnlNumAcd2WorkMode] unexpected mpu workmode:%d!\n", byWorkMode);
		break;
	}
	return 0 != byInitChnlNum ;
}

BOOL32 CMpuBasInst::GetBasOutChnlNumAcd2WorkMode(u8 byWorkMode, u8 &byOutChnlNum)
{
	byOutChnlNum = 0;
	switch (byWorkMode)
	{
	case TYPE_MPUBAS:
		byOutChnlNum = MAXNUM_VOUTPUT;
		break;
	case TYPE_MPUBAS_H:
		byOutChnlNum = MAXNUM_MPU_H_VOUTPUT;
		break;
	default:
		OspPrintf(TRUE, FALSE, "[GetBasOutChnlNumAcd2WorkMode] unexpected mpu workmode:%d!\n", byWorkMode);
		break;
	}
	return 0 != byOutChnlNum ;
}
//END OF FILE
