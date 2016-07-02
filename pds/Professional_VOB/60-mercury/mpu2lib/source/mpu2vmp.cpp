/*****************************************************************************
   ģ����      : mpu2lib
   �ļ���      : mpu2vmp.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: mpu2lib vmpҵ��ʵ��
   ����        : ������
   �汾        : V4.7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��       �汾        �޸���      �޸�����
   2011/12/07    4.7         ������      ����
******************************************************************************/
#include "mpu2inst.h"
#include "evmpu2.h"
#include "mcuver.h"
#include "evmcueqp.h"
#include "boardagent.h"
#ifdef _LINUX_
#include "brdwrapper.h"
#endif
BOOL32 g_bPauseSS = FALSE;

/*====================================================================
	����  : Clear
	����  : ���״̬����̬�ڴ��
	����  : CMessage
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpInst::Clear()
{
	//1��delete new�Ķ�̬�ڴ�
    u8 byLoop = 0;
    for ( ; byLoop < MAXNUM_MPU2VMP_MEMBER; byLoop++ )
    {
		if( NULL != m_pcMediaRcv[byLoop] ) 
		{
			delete m_pcMediaRcv[byLoop];
			 m_pcMediaRcv[byLoop] = NULL;
		}
       
    }
    byLoop = 0;
    for (; byLoop < m_byMaxVmpOutNum; byLoop++)
    {
		if ( NULL !=  m_pcMediaSnd[byLoop] ) 
		{
			delete  m_pcMediaSnd[byLoop];
			m_pcMediaSnd[byLoop] = NULL;
		}
        
    }
	//2�����AddChannel����
    byLoop = 0;
    for ( ; byLoop < MAXNUM_MPU2VMP_MEMBER; byLoop++ )
    {
        m_bAddVmpChannel[byLoop] = FALSE;
    }
	//3�������Ա������ֵ
    m_dwMcuIId         = 0;        
    m_dwMcuIIdB        = 0;       
    m_byRegAckNum      = 0;     
	m_dwMpcSSrc        = 0;      
    m_dwMcuRcvIp       = 0;      
    m_wMcuRcvStartPort = 0;
    m_byHardStyle      = 0;   
	m_wMTUSize         = 0;	
    m_emMcuEncoding  = emenCoding_GBK;
    memset(m_abChnlIframeReqed,0,sizeof(m_abChnlIframeReqed));
	memset( m_adwLastFUPTick, 0, sizeof(m_adwLastFUPTick));

	memset ( m_bUpdateMediaEncrpyt, 0 , sizeof(m_bUpdateMediaEncrpyt));
	m_cHardMulPic.Destory();
	SetIsSimulateMpu(FALSE);
	SetIsDisplayMbAlias(FALSE);
	m_byMaxVmpOutNum = MAXNUM_MPU2VMP_CHANNEL;//Ĭ����mpu2 vmp ��9·
	m_byValidVmpOutNum = 0;
	if (IsSimulateMpu())
	{
		m_byMaxVmpOutNum = MAXNUM_MPUSVMP_CHANNEL;
		m_byValidVmpOutNum = MAXNUM_MPUSVMP_CHANNEL;
	}

	
	m_bCreateHardMulPic = FALSE;
	m_dwInitVmpMode = 0;
	m_bFastUpdate = FALSE;

       
}
/*====================================================================
	����  : DaemonProcParamShow
	����  : Daemon�����ӡvmpparam����Ϣ���ַ�������instance
	����  : CMessage
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��          �汾        �޸���            �޸�����
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpInst::DaemonProcParamShow()
{
	for (u8 byLoop = 0 ; byLoop< g_cMpu2VmpApp.m_byVmpEqpNum ;byLoop++)
	{
		CServMsg cServMsg;
		post(MAKEIID(AID_MPU2,byLoop+1 ),EV_VMP_SHOWPARAM,cServMsg.GetServMsg(),cServMsg.GetServMsgLen());
	}
}
/*====================================================================
	����  : DaemonProcStatusShow
	����  : Daemon�����ӡstatus����Ϣ���ַ�������instance
	����  : CMessage
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��          �汾        �޸���            �޸�����
   2011/12/07		v4.7    	zhouyiliang			create
====================================================================*/
void CMpu2VmpInst::DaemonProcStatusShow()
{
	for (u8 byLoop = 0 ; byLoop< g_cMpu2VmpApp.m_byVmpEqpNum ;byLoop++)
	{
		CServMsg cServMsg;
		post(MAKEIID(AID_MPU2,byLoop+1 ),EV_VMP_DISPLAYALL,cServMsg.GetServMsg(),cServMsg.GetServMsgLen());
	}
}

/*====================================================================
	����  : DaemonInstanceEntry
	����  : Daemon��Ϣ�ַ�
	����  : CMessage��pApp
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	11/11/17	v4.7		������			create
====================================================================*/
void CMpu2VmpInst::DaemonInstanceEntry( CMessage *const pcMsg, CApp* pApp )
{
	if (NULL == pcMsg || NULL == pApp)
	{
		ErrorLog("[DaemonInstanceEntry]wrong message recived. pcMsg is Null or pApp is Null\n ");
		return;
	}
	switch( pcMsg->event ) 
	{
		case EV_VMP_INIT:
			DaemonInit(pcMsg);
			break;
		 // ��������
		case OSP_DISCONNECT:
			DaemonProcDisconnect( pcMsg );
			break;
		// ����
		case EV_VMP_CONNECT_TIMER:
			DaemonProcConnectTimeOut( TRUE );
			break;
		//���ӱ���
		case EV_VMP_CONNECT_TIMERB:
			DaemonProcConnectTimeOut( FALSE );
			break;
		//��ӡ״̬	
		 case EV_VMP_DISPLAYALL:
			DaemonProcStatusShow();
			break;
		//��ӡ����
 		case EV_VMP_SHOWPARAM:
 			DaemonProcParamShow();
			break;
			default:
			break;
	}
}

/*====================================================================
	����  : InstanceEntry
	����  : ��Ϣ�ַ�
	����  : CMessage
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��          �汾        �޸���        �޸�����
	11/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpInst::InstanceEntry( CMessage *const pMsg )
{
    if ( NULL == pMsg )
    {
		ErrorLog("[InstanceEntry]wrong message recived. pMsg is Null \n");
        return;
    }

	if ( pMsg->event >=EV_VMP_NEEDIFRAME_TIMER && (pMsg->event< EV_VMP_NEEDIFRAME_TIMER+MAXNUM_MPU2VMP_MEMBER) )
	{
		//���VMP�Ƿ���Ҫ�ؼ�֡
		MsgTimerNeedIFrameProc(pMsg);
		return;
	}
	else
    {
		KeyLog("Message %u(%s) received in InstanceEntry\n", pMsg->event, ::OspEventDesc(pMsg->event) );
        //KeyLog( "Message %u(%s) received in InstanceEntry\n", pMsg->event, ::OspEventDesc(pMsg->event) );
    }
    switch ( pMsg->event )
    {
    //--------�ڲ���Ϣ------------
   
	// ע��
    case EV_VMP_REGISTER_TIMER:
        ProcRegisterTimeOut( TRUE );
        break;
	//����ע��
    case EV_VMP_REGISTER_TIMERB:
        ProcRegisterTimeOut( FALSE );
        break;
	//OSP����
	case OSP_DISCONNECT:
		Disconnect();
		break;
	
	
    //--------�ⲿ��Ϣ------------
    // ������ʼ��
    case EV_VMP_INIT:
        Init( pMsg );
        break;

     // ��ʾ���е�״̬��ͳ����Ϣ
     case EV_VMP_DISPLAYALL:
        StatusShow();
        break;
	//��ӡ����
 	case EV_VMP_SHOWPARAM:
 		ParamShow();
		break;
	//���ı���֡��
	case EV_VMP_SETFRAMERATE_CMD:
		ProcSetFrameRateCmd(pMsg);
		break;

    // ע��Ӧ����Ϣ
    case MCU_VMP_REGISTER_ACK:
        MsgRegAckProc( pMsg );
        break;

    // ע��ܾ���Ϣ
    case MCU_VMP_REGISTER_NACK:
        MsgRegNackProc( pMsg );
        break;
	//����mcu�Ľ��յ�ַ
	case MCU_EQP_MODSENDADDR_CMD:
		MsgModMcuRcvAddrProc( pMsg );
		break;

    // ��ʼ����
    case MCU_VMP_STARTVIDMIX_REQ:
        MsgStartVidMixProc( pMsg );
        break;

    // ֹͣ����
    case MCU_VMP_STOPVIDMIX_REQ:
        MsgStopVidMixProc( pMsg );
        break;

    // �����ı�
    case MCU_VMP_CHANGEVIDMIXPARAM_CMD:
        MsgChangeVidMixParamProc( pMsg );
        break;

    // ����ͨ������
    case MCU_VMP_SETCHANNELBITRATE_REQ:
        MsgSetBitRate( pMsg );
        break;


    // ���ı������
    case MCU_VMP_UPDATAVMPENCRYPTPARAM_REQ:
        MsgUpdateVmpEncryptParamProc( pMsg );
        break;

    // ǿ�ƹؼ�֡	//FIXME: �Ż��ɴ�����(��Ӧ4��)
    case MCU_VMP_FASTUPDATEPIC_CMD:
        MsgFastUpdatePicProc( pMsg );
        break;

		//�����ϳɳ�Ա����
	case MCU_VMP_CHANGEMEMALIAS_CMD:
		ProcChangeMemAliasCmd(pMsg);
		break;


	// ȡ��������״̬
	case TIME_GET_MSSTATUS:
	case MCU_EQP_GETMSSTATUS_ACK:
		ProcGetMsStatusRsp( pMsg );
		break;
	//��ӡ�߼�������Ϣ
	case EV_VMP_SHOWDEBUG:
		ShowVMPDebugInfo();
		break;
	case MCU_VMP_ADDREMOVECHNNL_CMD:
		MsgAddRemoveRcvChnnl(pMsg);
		break;
    // δ�������Ϣ
    default:
        //log( LOGLVL_EXCEPTION, "[Error]Unexcpet Message %u(%s).\n", pMsg->event, ::OspEventDesc(pMsg->event) );
		ErrorLog("[Error]Unexcpet Message %u(%s).\n", pMsg->event, ::OspEventDesc(pMsg->event) );
        break;
    }
}

/*====================================================================
	����  : DaemonInit
	����  : Daemon�ĳ�ʼ��
	����  : CMessage
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��          �汾        �޸���        �޸�����
	11/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpInst::DaemonInit(CMessage* const pcMsg)
{
	//1������У�飨�Ϸ��ԣ���������ã�
	if (NULL == pcMsg)
	{
		ErrorLog("[DaemonInit]invalid param pcMsg:Null\n");
		return;
	}
	TMpu2Cfg tMpu2Cfg;
	tMpu2Cfg = *(TMpu2Cfg*)pcMsg->content;
	u8 byEqpNum = tMpu2Cfg.GetMpu2EqpNum();
	if (byEqpNum == 0 || byEqpNum > MAXNUM_MPU2_EQPNUM)
	{
		
		ErrorLog("[DaemonInit]Error eqpNum:%d\n",byEqpNum);
		return;
	}
	NEXTSTATE((u32)DAEMON_INIT);
	//2������eqp���÷�����Ӧ��instance��һ��instance��Ӧһ��eqp���ã�
	BOOL32 bHasConnectIp = FALSE;
	BOOL32 bHasConnectIpB = FALSE;
	for (u8 byLoop = 0; byLoop < byEqpNum; byLoop++)
	{

		TEqpBasicCfg tCfg = tMpu2Cfg.GetMpu2EqpCfg(byLoop);
		KeyLog("[DaemonInit]eqpNo:%d 's config:eqpId:%d,eqpRcvStartPort:%d\n",byLoop,tCfg.byEqpId,tCfg.wRcvStartPort);
		post(MAKEIID(AID_MPU2,byLoop+1),EV_VMP_INIT,(u8*)&tCfg,sizeof(TEqpBasicCfg));
		if (0 != tCfg.dwConnectIP) 
		{
			bHasConnectIp = TRUE;
		}
		if (0 != tCfg.dwConnectIpB) 
		{
			bHasConnectIpB = TRUE;
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


	//connect mcuͬʱע�������Ϣ
	if (bHasConnectIp)
	{
		 SetTimer( EV_VMP_CONNECT_TIMER, MPU_CONNETC_TIMEOUT );
	}
    if (bHasConnectIpB) 
	{
		SetTimer( EV_VMP_CONNECT_TIMERB, MPU_CONNETC_TIMEOUT);
    }

	

}

/*====================================================================
	����  : DaemonProcDisconnect
	����  : Daemon����dsiconnect��Ϣ
	����  : CMessage
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��          �汾        �޸���        �޸�����
	11/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpInst::DaemonProcDisconnect( CMessage * const pMsg )
{
	//1������У��
	if ( NULL == pMsg )  
    {
	    ErrorLog("[DaemonProcDisconnect]disconnect message's pointer is Null\n" );
        return;
    }
	//2������������node���Ͽ�����
    u32 dwNode = *(u32*)pMsg->content;

    if ( INVALID_NODE != dwNode )
    {
        OspDisconnectTcpNode( dwNode );
    } 
	//3������connect��timer����
    if( dwNode == g_cMpu2VmpApp.m_dwMcuNode ) // ����
    {
		KeyLog("[DaemonProcDisconnect] McuNode.A disconnect\n" );
        g_cMpu2VmpApp.m_dwMcuNode = INVALID_NODE;
        m_dwMcuIId  = 0;
        SetTimer( EV_VMP_CONNECT_TIMER, MPU_CONNETC_TIMEOUT );
    }
    else if ( dwNode == g_cMpu2VmpApp.m_dwMcuNodeB )
    {
		KeyLog("[DaemonProcDisconnect] McuNode.B disconnect\n" );
        g_cMpu2VmpApp.m_dwMcuNodeB = INVALID_NODE;
        m_dwMcuIIdB  = 0;
        SetTimer( EV_VMP_CONNECT_TIMERB, MPU_CONNETC_TIMEOUT );
    }
	//4����disconnect��Ϣ�ַ�������instance��ÿ����������Ӧ��disconnect����
	for (u8 byLoop = 0; byLoop < g_cMpu2VmpApp.m_byVmpEqpNum;byLoop++)
	{

		post(MAKEIID(AID_MPU2,byLoop+1),OSP_DISCONNECT,pMsg,pMsg->length);
	}
	//5��״̬ת��
	if (INVALID_NODE == g_cMpu2VmpApp.m_dwMcuNode && INVALID_NODE == g_cMpu2VmpApp.m_dwMcuNodeB) 
	{
		NEXTSTATE((u32)DAEMON_IDLE);
	}
	
}



/*====================================================================
	����  : StatusShow
	����  : ��ӡ��ǰ����״̬
	����  : 
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��          �汾        �޸���        �޸�����
	11/12/07		v1.0		zhouyiliang			create
====================================================================*/
void CMpu2VmpInst::StatusShow()
{

    TMulPicStatus tMulPicStatus;
//    TMulPicStatis tMulPicStatis;
    memset(&tMulPicStatus, 0, sizeof(tMulPicStatus));
//    memset(&tMulPicStatis, 0, sizeof(tMulPicStatis));

    m_cHardMulPic.GetStatus(0, tMulPicStatus);
//    m_cHardMulPic.GetStatis(tMulPicStatis);


    StaticLog( "\t===========The state of VMP is below CurEqpId:(%d)===============\n",m_tEqp.GetEqpId());

	StaticLog("The MTU Size is : %d\n", m_wMTUSize );

    StaticLog("  IDLE =%d, NORMAL=%d, RUNNING=%d, VMP Current State =%d, CreateHardMulPic:%s\n",
		IDLE, NORMAL, RUNNING, CurState(),m_bCreateHardMulPic?("SUCCEED"):"FAILED");

    StaticLog( "\t===========The status of VMP is below===============\n");
    StaticLog( "Merge=%d\t Style=%d\t CurChannel=%d\n",
                          tMulPicStatus.bMergeStart,
                          tMulPicStatus.byType,
                          tMulPicStatus.byCurChnNum);

    StaticLog( "\t-------------The Video Encoding Params--------------\n");
	for ( u8 byLoop =0; byLoop < m_byValidVmpOutNum;byLoop++ )
	{
		StaticLog( "\t-------------OutCHNNL:%d--------------\n",byLoop);
		m_cHardMulPic.GetStatus(byLoop, tMulPicStatus);
		StaticLog( "\t EncType=%d\t ComMode=%d\t KeyFrameInter=%d\t MaxQuant=%d\t MinQuant=%d\t\n",
			tMulPicStatus.tVideoEncParam.m_byEncType,
			tMulPicStatus.tVideoEncParam.m_byRcMode,
			tMulPicStatus.tVideoEncParam.m_dwMaxKeyFrameInterval,
			tMulPicStatus.tVideoEncParam.m_byMaxQuant,
			tMulPicStatus.tVideoEncParam.m_byMinQuant);
		
		StaticLog( "BitRate=%d\t FrameRate=%d\t ImageQuality=%d\t VideoWidth=%d\t VideoHeight=%d,profileType=%d\t\n",
			tMulPicStatus.tVideoEncParam.m_wBitRate,
			tMulPicStatus.tVideoEncParam.m_byFrameRate,
			tMulPicStatus.tVideoEncParam.m_byImageQulity,
			tMulPicStatus.tVideoEncParam.m_wVideoWidth,
            tMulPicStatus.tVideoEncParam.m_wVideoHeight,
			 tMulPicStatus.tVideoEncParam.m_dwProfile );
	}
	
    return;
}

/*====================================================================
	����  : ParamShow
	����  : ��ӡ��ǰ�ı������
	����  : 
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��          �汾        �޸���        �޸�����
	11/12/07		v1.0		zhouyiliang			create
====================================================================*/
void CMpu2VmpInst::ParamShow(void)
{
   if ( CurState() != RUNNING   ) 
   {
		::OspPrintf(TRUE, FALSE, "\t=============CurrentState wrong ,it is :%d=============\t\n",CurState());
		return;
   }

//    if ( IsSimulateMpu() ) 
//    {
// 
// 		StaticLog( "\t=============The Param of VMP is below=============\t\n");
// 		StaticLog(  "\t------------ The first video chnnl ----------------\t\n");
// 		m_tMpuParam[0].Print();
// 		StaticLog( "\n");
// 		StaticLog(  "\t------------ The second video chnnl ----------------\t\n");
// 		m_tMpuParam[1].Print();
// 		StaticLog(  "\t----------------------------------------------------\t\n");
// 		StaticLog(  "\n");
// 		StaticLog(  "\t------------ The third video chnnl -----------------\t\n");
// 		m_tMpuParam[2].Print();
// 		StaticLog(  "\t----------------------------------------------------\t\n\n");
// 		StaticLog(  "\t------------ The fourth video chnnl -----------------\t\n");
// 		m_tMpuParam[3].Print();
// 		StaticLog(  "\t----------------------------------------------------\t\n");
// 		
//    }
//    else
   {
	   StaticLog(  "\t=============The Param of VMP is below CurEqpId:(%d)=============\t\n",m_tEqp.GetEqpId());
	   m_tVmpCommonAttrb.Print();
	   for (u8 byLoop = 0; byLoop < m_byMaxVmpOutNum;byLoop++)
	   {
		   StaticLog( "\t--------Mpu2Vmp outChannel:%d--------\t\n",byLoop);
		   	m_tParam[byLoop].Print();
	   }
	
   }

   StaticLog("==============Follows are the resbitratelimttable(after sort)===============\n");
   g_tResBrLimitTable.printTable();

    StaticLog("==============Follows are the Debug configs===============\n");
   m_tMpuVmpCfg.Print();

	

}

/*====================================================================
	����  : Init
	����  : �����ʼ������
	����  : CMessage
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��          �汾        �޸���        �޸�����
	11/12/07		v1.0		zhouyiliang			create
====================================================================*/
void CMpu2VmpInst::Init( CMessage *const pMsg )
{
    //1���������� 
    if ( NULL == pMsg )
    {
        ErrorLog( "[Init] Init params cannot be NULL!\n" );
        return;
    }
	//2��������Ϣ��TEqpBasicCfg��ֱ�ӱ��浽m_tCfg
   
    memcpy( &m_tCfg, pMsg->content, sizeof(TEqpBasicCfg) );

#ifdef _LINUX_
    if ( m_tCfg.dwLocalIP == m_tCfg.dwConnectIP )
    {
        ErrorLog( "[Init] m_tCfg.dwLocalIP == m_tCfg.dwConnectIP\n" );   
        return;
    }
#endif
	KeyLog("[CMpu2VmpInst::Init]EqpId:%d m_tCfg.dwConnectIP is 0x%X\n",m_tCfg.byEqpId,m_tCfg.dwConnectIP);
	KeyLog("[CMpu2VmpInst::Init]EqpId:%d m_tCfg.dwConnectIPB is 0x%X\n",m_tCfg.byEqpId,m_tCfg.dwConnectIpB);

	//3��ý�ز�ĳ�ʼ��


	TMultiPicCreate tMPCreate;

	tMPCreate.m_dwDevVersion = (u32)en_MPU2_Board;


	tMPCreate.m_dwMode = VMP_BASIC;
	if ( g_cMpu2VmpApp.m_byWorkMode == TYPE_MPU2VMP_ENHANCED)
	{
		tMPCreate.m_dwMode = VMP_ENHANCED;
	}
	m_dwInitVmpMode = tMPCreate.m_dwMode;
    s32 nRet = m_cHardMulPic.Create( tMPCreate );
	printf("[init]m_cHardMulPic.Create( tMPCreate )ret:%d\n",nRet);
    if ( HARD_MULPIC_OK == nRet )
    {
        KeyLog( "[Init] Create %s Success!\n", m_tCfg.achAlias );
		m_bCreateHardMulPic = TRUE;
    }
    else
    {
        ErrorLog( "[Init] Fail to Create. errcode.%d\n", nRet );
		m_bCreateHardMulPic = FALSE;
        return;
    }

	//4��������ʼ��
    u16 wRet = KdvSocketStartup();
    
    if ( MEDIANET_NO_ERROR != wRet )
    {
        ErrorLog( "[Init] KdvSocketStartup failed, error: %d\n", wRet );
		m_cHardMulPic.Destory();
        return ;
    }
	
	KeyLog("[Init]KdvSocketStartup ok!!!\n");

	//5��init���ն���
    u8 byLoop = 0;
    for ( ; byLoop < MAXNUM_MPU2VMP_MEMBER; byLoop++  )
    {
        m_pcMediaRcv[byLoop] = new(CKdvMediaRcv);
        if ( NULL == m_pcMediaRcv[byLoop] )
        {
            ErrorLog( "new(CKdvMediaRcv) fail!\n" );
			Clear();
            return;
        }
		if (!InitMediaRcv(byLoop))
		{
            ErrorLog( "InitMediaRcv %d fail!\n", byLoop );
			Clear();
		
			return;
		}
		
    }

	//6��new���Ͷ���,���÷��ͻص�
	if ( g_cMpu2VmpApp.m_byWorkMode == TYPE_MPUSVMP ) //�����ģ��mpu�����4·��
	{
		OspSetPrompt("mpu");
		SetIsSimulateMpu(TRUE);
		m_byMaxVmpOutNum = MAXNUM_MPUSVMP_CHANNEL;
		m_byValidVmpOutNum = MAXNUM_MPUSVMP_CHANNEL;
	}

	byLoop =0;
	for (;byLoop < m_byMaxVmpOutNum;byLoop++)
	{

        m_pcMediaSnd[byLoop] = new(CKdvMediaSnd); 
        if ( NULL == m_pcMediaSnd[byLoop] )
        {
			Clear();
            ErrorLog( " new(CKdvMediaSnd) fail!\n" );
            return;
        }

    
	}
   

   
	//7�����߼�������Ϣ����Ϣ
	sprintf( MPULIB_CFG_FILE, "%s/mcueqp.ini", DIR_CONFIG );
    m_tMpuVmpCfg.ReadDebugValues();

	// ���üӺڱߡ��ñߡ��ǵȱ�����
	m_cHardMulPic.SetEncResizeMode(m_tMpuVmpCfg.GetVidSDMode());
	KeyLog("[Init] SVMP SetEncResizeMode mode is %d\n", m_tMpuVmpCfg.GetVidSDMode());
	printf("[init]end instid:%d\n",GetInsID());

    //[5/17/2013 liaokang] 
    m_cBmpDrawer.LoadFont();
    return;
}

/*====================================================================
	����  : Disconnect
	����  : ��ͨʵ������Osp������Ϣ
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::Disconnect( )
{

	//1�� ������һ����������Mcuȡ��������״̬���ж��Ƿ�ɹ�
	if ( INVALID_NODE != g_cMpu2VmpApp.m_dwMcuNode || INVALID_NODE != g_cMpu2VmpApp.m_dwMcuNodeB )
	{
		if ( OspIsValidTcpNode(g_cMpu2VmpApp.m_dwMcuNode) )
		{
			post( m_dwMcuIId, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cMpu2VmpApp.m_dwMcuNode );           
			KeyLog( "[Disconnect] try GetMsStatusReq. from McuNode.A(%d)\n",g_cMpu2VmpApp.m_dwMcuNode );
		}
		else if ( OspIsValidTcpNode(g_cMpu2VmpApp.m_dwMcuNodeB) )
		{
			post( m_dwMcuIIdB, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cMpu2VmpApp.m_dwMcuNodeB );        
			KeyLog( "[Disconnect] try GetMsStatusReq. from McuNode.B(%d)\n",g_cMpu2VmpApp.m_dwMcuNodeB );
		}

		SetTimer( TIME_GET_MSSTATUS, WAITING_MSSTATUS_TIMEOUT);
		return;
	}
	//2�� �����ڵ㶼�ϣ����״̬��Ϣ
    if ( INVALID_NODE == g_cMpu2VmpApp.m_dwMcuNode && INVALID_NODE == g_cMpu2VmpApp.m_dwMcuNodeB )
    {
        ClearCurrentInst();
    }
    return;
}

/*=============================================================================
  �� �� ���� ClearCurrentInst
  ��    �ܣ� ��յ�ǰʵ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::ClearCurrentInst( void )
{
	switch ( CurState() )
    {
    case NORMAL:
    case RUNNING:
        {
            s32 nRet = m_cHardMulPic.StopMerge();
            KillTimer( EV_VMP_NEEDIFRAME_TIMER );
            KeyLog( "[VMP Stop]:Stop work, the return code: %d!\n", nRet );
			//ֹͣ�ϳɱ���������ص�
			for ( u8 byChnNo = 0; byChnNo < m_byValidVmpOutNum; byChnNo++ )
			{
				m_cHardMulPic.SetVidDataCallback( byChnNo, NULL, 0 );
			
			}
        }
        break;
	default:
		break;
    }

    NEXTSTATE((u32) IDLE );

    m_byRegAckNum = 0;
	m_dwMpcSSrc = 0;
	m_bFastUpdate = FALSE;
	
	memset( m_bAddVmpChannel, 0, sizeof(m_bAddVmpChannel) );
	memset( m_bUpdateMediaEncrpyt, 0, sizeof(m_bUpdateMediaEncrpyt));
	memset( m_abChnlIframeReqed, 0, sizeof(m_abChnlIframeReqed) );
	SetIsDisplayMbAlias(FALSE);
	m_cHardMulPic.ChangeMode(m_dwInitVmpMode);

	//���previous��صĳ�Ա
	memset(m_tParamPrevious, 0, sizeof(m_tParamPrevious));
	memset(&m_tPreviousVmpCommonAttrb, 0, sizeof(m_tPreviousVmpCommonAttrb));
    memset(m_tVmpMbAlias, 0, sizeof(m_tVmpMbAlias));

    KeyLog( "[Vmp] Disconnect with A and B board\n");

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
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::DaemonProcConnectTimeOut( BOOL32 bIsConnectA )
{
    BOOL32 bRet = FALSE;
    if( TRUE == bIsConnectA )
    {
        bRet = ConnectMcu( bIsConnectA, g_cMpu2VmpApp.m_dwMcuNode );
        if ( TRUE == bRet )
        {  
           //���ӳɹ�����Register��Ϣ��ʵ���������ǿ�ʼע��
			for (u8 byLoop = 0 ; byLoop< g_cMpu2VmpApp.m_byVmpEqpNum ;byLoop++)
			{
				CServMsg cServMsg;
				post(MAKEIID(AID_MPU2,byLoop+1),EV_VMP_REGISTER_TIMER,cServMsg.GetServMsg(),cServMsg.GetServMsgLen());
			}
			//TODO:��ʱ�л�״̬�Ƿ���ȷ�����connect mcub���ɹ��أ�������
			NEXTSTATE((u32)DAEMON_NORMAL);
			
        }
        else
        {
            SetTimer( EV_VMP_CONNECT_TIMER, MPU_CONNETC_TIMEOUT );   
        }
    }
    else //���ӱ���
    {
        bRet = ConnectMcu( bIsConnectA, g_cMpu2VmpApp.m_dwMcuNodeB );
        if ( TRUE == bRet )
        { 
			//���ӳɹ�����Register��Ϣ��ʵ���������ǿ�ʼע��
			for (u8 byLoop = 0 ; byLoop< g_cMpu2VmpApp.m_byVmpEqpNum ;byLoop++)
			{
				CServMsg cServMsg;
				post(MAKEIID(AID_MPU2,byLoop+1),EV_VMP_REGISTER_TIMERB,cServMsg.GetServMsg(),cServMsg.GetServMsgLen());
			}
				//TODO:��ʱ�л�״̬�Ƿ���ȷ������������
			NEXTSTATE((u32)DAEMON_NORMAL);
        }
        else
        {
            SetTimer( EV_VMP_CONNECT_TIMERB, MPU_CONNETC_TIMEOUT );   
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
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
BOOL32 CMpu2VmpInst::ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode )
{
    BOOL32 bRet = TRUE;

	//ȡ���ӵ�node
    if( !OspIsValidTcpNode(dwMcuNode) ) 
    {
#ifdef _8KI_
		dwMcuNode = OspConnectTcpNode(htonl(m_tCfg.dwConnectIP),m_tCfg.wConnectPort,10,3,100);
	
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
		    KeyLog( "[ConnectMcu]Connect Mcu %s Success, node: %d!\n", 
                   bIsConnectA ? "A" : "B", dwMcuNode );
		    ::OspNodeDiscCBRegQ( dwMcuNode, GetAppID(), GetInsID() );
	    }
	    else 
	    {
		    ErrorLog( "[ConnectMcu]Connect to Mcu failed, will retry\n" );
            bRet = FALSE;
	    }
    }
    return bRet;
}

/*=============================================================================
  �� �� ���� ProcRegisterTimeOut
  ��    �ܣ� ����ע�ᳬʱ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� BOOL32 bIsRegiterA
  �� �� ֵ�� void 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::ProcRegisterTimeOut( BOOL32 bIsRegiterA )
{
    if( TRUE == bIsRegiterA )
    {
        Register( g_cMpu2VmpApp.m_dwMcuNode );
        SetTimer( EV_VMP_REGISTER_TIMER, MPU_REGISTER_TIMEOUT );  
    }
    else
    {
        Register( g_cMpu2VmpApp.m_dwMcuNodeB );
		SetTimer( EV_VMP_REGISTER_TIMERB, MPU_REGISTER_TIMEOUT ); 
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
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::Register( /*BOOL32 bIsRegiterA,*/ u32 dwMcuNode )
{
    CServMsg       cSvrMsg;
    TPeriEqpRegReq tReg;

    tReg.SetMcuEqp( (u8)m_tCfg.wMcuId, m_tCfg.byEqpId, m_tCfg.byEqpType );
    tReg.SetPeriEqpIpAddr( htonl(m_tCfg.dwLocalIP) );
	tReg.SetStartPort( m_tCfg.wRcvStartPort );
	if ( IsSimulateMpu() ) 
	{
		tReg.SetVersion( DEVVER_MPU );
	}
	else
	{
		tReg.SetVersion( DEVVER_MPU2 );
	}
    
    
    cSvrMsg.SetMsgBody( (u8*)&tReg, sizeof(tReg) );
        
    post( MAKEIID(AID_MCU_PERIEQPSSN, m_tCfg.byEqpId),
            VMP_MCU_REGISTER_REQ,
            cSvrMsg.GetServMsg(),
            cSvrMsg.GetServMsgLen(),
            dwMcuNode );

	return;
}

/*====================================================================
	������  ��SendMsgToMcu
	����    ����MCU������Ϣ
	����    ��wEvent - �¼�
			  
	���    ����
	����ֵ  ���ɹ����� TRUE�����򷵻�FALSE
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
BOOL CMpu2VmpInst::SendMsgToMcu( u16 wEvent, CServMsg& cServMsg )
{
    KeyLog( "[SendMsgToMcu] Message %u(%s).\n", wEvent, ::OspEventDesc(wEvent) );

    if ( OspIsValidTcpNode(g_cMpu2VmpApp.m_dwMcuNode) )
    {
        post( m_dwMcuIId, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), g_cMpu2VmpApp.m_dwMcuNode );
    }

    if ( OspIsValidTcpNode(g_cMpu2VmpApp.m_dwMcuNodeB) )
    {
        post( m_dwMcuIIdB, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), g_cMpu2VmpApp.m_dwMcuNodeB );
    }

    return TRUE;
}

/*====================================================================
	����  : MsgRegAckProc 
	����  : MCUӦ��ע��ɹ���Ϣ������
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::MsgRegAckProc( CMessage* const pMsg )
{
	//1�������Ϸ����ж�
	if ( NULL == pMsg )
    {
        ErrorLog( "[MsgRegAckProc]Recv Reg Ack Msg, but the msg's pointer is Null\n" );
        return;
    }

	//2������ACK��Ϣ��
	TPeriEqpRegAck* ptRegAck;

	CServMsg cServMsg( pMsg->content, pMsg->length );
	ptRegAck = (TPeriEqpRegAck*)cServMsg.GetMsgBody();
	
    TPrsTimeSpan tPrsTimeSpan = *(TPrsTimeSpan*)(cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck));
	u16 wMTUSize = *(u16*)( cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck) + sizeof(TPrsTimeSpan) );
	wMTUSize = ntohs( wMTUSize );
    //[4/9/2013 liaokang] ���뷽ʽ
    u8 byMcuEncoding = emenCoding_GBK;
    if( cServMsg.GetMsgBodyLen() > sizeof(TPeriEqpRegAck) + sizeof(TPrsTimeSpan) + sizeof(u16) )
    {
        byMcuEncoding = *(u8*)( cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck) + sizeof(TPrsTimeSpan) + sizeof(u16) );
    }

	//3��Kill��Ӧ��ע��timer
    if( pMsg->srcnode == g_cMpu2VmpApp.m_dwMcuNode ) 
    {
	    m_dwMcuIId = pMsg->srcid;
        m_byRegAckNum++;
        KillTimer( EV_VMP_REGISTER_TIMER );

        KeyLog( "[MsgRegAckProc] Regist success to mcu A\n" );
    }
    else if ( pMsg->srcnode == g_cMpu2VmpApp.m_dwMcuNodeB )
    {
        m_dwMcuIIdB = pMsg->srcid;
        m_byRegAckNum++;
        KillTimer( EV_VMP_REGISTER_TIMERB );

        KeyLog( "[MsgRegAckProc] Regist success to mcu B\n" );       
    }

	// guzh [6/12/2007] У��Ự����
    if ( m_dwMpcSSrc == 0 )
    {
        m_dwMpcSSrc = ptRegAck->GetMSSsrc();
    }
    else
    {
        // �쳣������Ͽ������ڵ�
        if ( m_dwMpcSSrc != ptRegAck->GetMSSsrc() )
        {
            ErrorLog( "[RegAck] MPC SSRC ERROR(%u<-->%u), Disconnect Both Nodes!\n", 
                       m_dwMpcSSrc, ptRegAck->GetMSSsrc() );
            if ( OspIsValidTcpNode(g_cMpu2VmpApp.m_dwMcuNode) )
            {
                OspDisconnectTcpNode(g_cMpu2VmpApp.m_dwMcuNode);
            }
            if ( OspIsValidTcpNode(g_cMpu2VmpApp.m_dwMcuNodeB) )
            {
                OspDisconnectTcpNode(g_cMpu2VmpApp.m_dwMcuNodeB);
            }      
            return;
        }
    }

    //4�� ��һ���յ�ע��ɹ���Ϣ
    if( FIRST_REGACK == m_byRegAckNum ) 
    {
        m_dwMcuRcvIp       = ptRegAck->GetMcuIpAddr();
	    m_wMcuRcvStartPort = ptRegAck->GetMcuStartPort();

        // ��������ʱû�л�ȡ�������ȡ����һ��MCU�������ȡ��ȥ����
        u32 dwOtherMcuIp = htonl( ptRegAck->GetAnotherMpcIp() );
        if ( m_tCfg.dwConnectIpB == 0 && dwOtherMcuIp != 0 )
        {
            m_tCfg.dwConnectIpB = dwOtherMcuIp;
            SetTimer( EV_VMP_CONNECT_TIMERB, MPU_CONNETC_TIMEOUT );

            ErrorLog( "[MsgRegAckProc] Found another Mpc IP: %s, try connecting...\n", 
                      ipStr(dwOtherMcuIp) );
        }

        KeyLog( "[MsgRegAckProc]eqpId:%d Local Recv Start Port.%u, Mcu Start Port.%u, Alias.%s\n",
                m_tCfg.byEqpId,m_tCfg.wRcvStartPort, m_wMcuRcvStartPort , m_tCfg.achAlias );

        m_tPrsTimeSpan = tPrsTimeSpan;
        KeyLog( "[MsgRegAckProc] The Vmp Prs span: first: %d, second: %d, three: %d, reject: %d\n", 
                m_tPrsTimeSpan.m_wFirstTimeSpan,
                m_tPrsTimeSpan.m_wSecondTimeSpan,
                m_tPrsTimeSpan.m_wThirdTimeSpan,
                m_tPrsTimeSpan.m_wRejectTimeSpan );

		m_wMTUSize = wMTUSize;
		KeyLog( "[MsgRegAckProc]The Network MTU is : %d\n", m_wMTUSize );

        // zw 20081208 �ڵ�һ��ע��ɹ�֮�󣬽�ȷ�Ϻ��m_tCfg�е���Ϣ��m_tCfg����PowerOnʱ������������ģ�ת����m_tEqp��
        m_tEqp.SetMcuEqp( (u8)m_tCfg.wMcuId, m_tCfg.byEqpId, m_tCfg.byEqpType );	
	    
        SetMcuEncoding((emenCodingForm)byMcuEncoding);

        NEXTSTATE((u32) NORMAL ); 

    }

	SendStatusChangeMsg((u8)ONLINE,(u8)USESTATE_IDLE);



    return;
}

/*====================================================================
	����  : MsgRegNackProc
	����  : MCU �ܾ�ע����Ϣ������
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::MsgRegNackProc( CMessage* const pMsg )
{
    if ( pMsg->srcnode == g_cMpu2VmpApp.m_dwMcuNode )    
    {
        KeyLog( "VMP registe be refused by A.\n" );
    }
    if ( pMsg->srcnode == g_cMpu2VmpApp.m_dwMcuNodeB )    
    {
        KeyLog( "VMP registe be refused by B.\n" );
    }
    return;
}
/*====================================================================
	����  : MsgModMcuRcvAddrProc
	����  : MCUǿ��Ҫ���޸ĵ�ַ
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::MsgModMcuRcvAddrProc( CMessage* const pMsg )
{
	//1�������Ϸ���У��
    if ( NULL == pMsg )
    {
        ErrorLog( " The pointer cannot be Null (MsgModMcuRcvAddrProc)\n" );
        return;
    }    
	//2��������Ϣ�壬�޸ĺ��mcu����ip
	u32 dwSendIP = *(u32*)pMsg->content;
	m_dwMcuRcvIp = ntohl(dwSendIP);
	//3���޸�mcu�Ľ��յ�ַ
    if (CurState() == RUNNING)
    {
		for ( u8 byChnNo = 0; byChnNo < m_byMaxVmpOutNum; byChnNo++ )
		{

			SetMediaSndNetParam(byChnNo);
		}

		//[2011/08/26/zhangli]���½�����Ϣ
		for (u8 byChn = 0; byChn < MAXNUM_MPU2VMP_MEMBER; byChn++)
		{
			SetMediaRcvNetParam( byChn);
		}

		BOOL32 bSetRule = !g_bPauseSS;
		SetSmoothSendRule( bSetRule );
    }	
}
/*====================================================================
	������  :MsgStartVidMixProc
	����    :��ʼ���渴����Ϣ������
	����    :��Ϣ
	���    :��
	����ֵ  :��
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::MsgStartVidMixProc( CMessage * const pMsg )
{
	u32 dwTick = OspTickGet();
	KeyLog("===========[MsgStartVidMixProc] in Tick:%d,tickrate:%d\n",dwTick,OspClkRateGet());
	//0�������Ϸ����ж�
    if ( NULL == pMsg )
    {
        ErrorLog( " The pointer cannot be Null (MsgStartVidMixProc)\n" );
        return;
    }  
	//��NORMAL״̬start����
	if ( CurState() != NORMAL ) 
	{
		ErrorLog( "[MsgStartVidMixProc]Wrong State(not NORMAL),CurrentState:%d\n",CurState() );
		return;
	}
	//2��mpu2 ������Ϣ�� TMPU2CommonAttrb+9* CKDVVMPOutParam
	CServMsg cServMsg( pMsg->content, pMsg->length );
    UnpackVmpWholeOperMsg(cServMsg, TRUE);


	KeyLog( "/*---------------------------------------------------------*/\n\n" );
	KeyLog( "Follows are the params that mcu send to the mpu2vmp:\n\n" );
	s32 nIdx = 0;
	for(nIdx = 0; nIdx < m_byMaxVmpOutNum; nIdx++)
	{
		KeyLog( "CHNNL.%d: Type=%d BitRate=%d VideoHeight:%d VideoWidth:%d,framerate:%d,HP:%d \n",
						   nIdx,
						   m_tParam[nIdx].GetEncType(), m_tParam[nIdx].GetBitRate(), 
						   m_tParam[nIdx].GetVideoHeight(), m_tParam[nIdx].GetVideoWidth(),
						   m_tParam[nIdx].GetFrameRate(),m_tParam[nIdx].GetProfileType() );
	}
	KeyLog( "/*---------------------------------------------------------*/\n\n" );

	s32 nRet = 0;
	//��0·��������ߵ�һ·�����С�ڵ���1080 30�����л���VMP_ADP_1080P30DECģʽ
	if ( m_tParam[0].GetVideoWidth() < 1920 || 
			(m_tParam[0].GetVideoWidth() == 1920 && m_tParam[0].GetFrameRate() <= 30) )
	{
		nRet = m_cHardMulPic.ChangeMode(VMP_ADP_1080P30DEC);
		
	}
	
	u8 byNeedPrs = m_tVmpCommonAttrb.m_byNeedPrs;
	m_tStyleInfo = m_tVmpCommonAttrb.m_tVmpStyleCfgInfo;
	TVMPExCfgInfo tVMPExCfgInfo = m_tVmpCommonAttrb.m_tVmpExCfgInfo;

	//3�����ÿ���ͨ����ʾģʽ����������һ֡���Զ���ͼƬ...��
	nRet = SetIdleChnShowMode(tVMPExCfgInfo.m_byIdleChlShowMode,TRUE);

	SetIsDisplayMbAlias( (tVMPExCfgInfo.m_byIsDisplayMmbAlias == 1) ? TRUE:FALSE);

	//4.���ֲ�������,�������ף�I֡�����P֡��Ŀ������������
	u8 byValidEncNum = 0;
	for(nIdx = 0; nIdx < m_byMaxVmpOutNum; nIdx++)
	{
		if ( IsEnableBitrateCheat() )
		{
			//ԭ��������0��ά������Ϊ0
			if(0 != m_tParam[nIdx].GetBitRate())
			{
				m_tParam[nIdx].SetBitRate( GetBitrateAfterCheated(m_tParam[nIdx].GetBitRate()) );
			}
			
			KeyLog( "New Enc.%d Bitrate: %d\n", nIdx, m_tParam[nIdx].GetBitRate() );
		}

		// ������Ƶ�������(�����ĸ�ʽ)
		// ��ɶ�m_tMpuVmpCfg.m_tVideoEncParam[]�Ļ���������ֵ
		m_tMpuVmpCfg.GetDefaultParam( m_tParam[nIdx].GetEncType(),
									  m_tMpuVmpCfg.m_tVideoEncParam[nIdx],
									  m_tMpuVmpCfg.m_adwMaxSendBand[nIdx] );


		
		// ����Ϊm_tMpuVmpCfg.m_tVideoEncParam[]����������ֵ�͵���
		TransFromMpu2Param2EncParam( &m_tParam[nIdx], &m_tMpuVmpCfg.m_tVideoEncParam[nIdx] );
		
		if ( m_tMpuVmpCfg.m_adwMaxSendBand[nIdx] == 0 )
		{
			// ���ʹ�����Ȼʹ�ó�ʼ���ʽ�������
			m_tMpuVmpCfg.m_adwMaxSendBand[nIdx] = (u32)(m_tParam[nIdx].GetBitRate() * 2 + 100);
		}
		
		//mpu2 �ղβ��£�mpuÿ·����
		if ( !IsSimulateMpu() &&	
			( m_tMpuVmpCfg.m_tVideoEncParam[nIdx].m_wVideoHeight == 0 || 0 == m_tMpuVmpCfg.m_tVideoEncParam[nIdx].m_wVideoWidth ) )
		{
			continue;
		}
	
		byValidEncNum++;
		// ��ӡ��ʼ�Ĳ���
		KeyLog( "Follows are the params we used to start the vmp:\n\n" );
		KeyLog( "/*----------  video channel.%d  -----------*/\n", nIdx );
		m_tMpuVmpCfg.PrintEncParam( nIdx );	
		KeyLog( "/*----------------------------------------*/\n\n" );
	}


	//5. ���÷��
	m_byHardStyle = ConvertVcStyle2HardStyle( m_tVmpCommonAttrb.m_byVmpStyle );
	dwTick = OspTickGet();
	nRet = m_cHardMulPic.SetMulPicType(m_byHardStyle);
	KeyLog("=============[MsgStartVidMixProc] m_cHardMulPic.SetMulPicType Tick:%d\n",OspTickGet() - dwTick);
	if ( HARD_MULPIC_OK != nRet )
	{
		ErrorLog( " SetMulPicType failed, Error code: %u\n", nRet );
		cServMsg.SetErrorCode( 1 );                 
		SendMsgToMcu( pMsg->event + 2, cServMsg );
		return;
	}
	KeyLog( "SetMulPicType to %d.\n", m_byHardStyle);

	//6. ͨ������
	
	u8 byMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle( m_tVmpCommonAttrb.m_byVmpStyle );	
	for (u8 byChnNo = 0; byChnNo < byMaxChnNum; byChnNo++ )
	{
		if ( m_tVmpCommonAttrb.m_atMtMember[byChnNo].IsNull() )
		{
			m_bAddVmpChannel[byChnNo] = FALSE;
		}
		else
		{
			dwTick = OspTickGet();
			nRet = m_cHardMulPic.AddChannel( byChnNo );
			KeyLog("================[MsgStartVidMixProc]after m_cHardMulPic.AddChannel(%d) Tick:%d\n",byChnNo,OspTickGet() - dwTick);
			if ( HARD_MULPIC_OK != nRet )
			{
				mpulog(MPU_CRIT, "[MsgStartVidMixProc] add Chn.%d failed, ret=%u!\n", byChnNo, nRet);
			}
			else
			{
				m_bAddVmpChannel[byChnNo] = TRUE;
		
			
			}
			KeyLog( "Add channel-%u return %u(%u is correct)!\n", byChnNo, nRet, HARD_MULPIC_OK );
			KeyLog( "RealPayLoad: %d, ActivePayload: %d\n",
							   m_tVmpCommonAttrb.m_tDoublePayload[byChnNo].GetRealPayLoad(),
							   m_tVmpCommonAttrb.m_tDoublePayload[byChnNo].GetActivePayload() );
		}
	}
	

	//7. ��ʼ�ϳ�
	m_byValidVmpOutNum = byValidEncNum;
	KeyLog("[MsgStartVidMixProc ]m_byValidVmpOutNum:%d\n",m_byValidVmpOutNum);
	TVideoEncParam atOrigEncParam[MAXNUM_MPU2VMP_CHANNEL];
	memcpy(atOrigEncParam,&m_tMpuVmpCfg.m_tVideoEncParam[0],sizeof(atOrigEncParam));
	for (u8 byLoop = 0 ; byLoop < m_byValidVmpOutNum;byLoop++)
	{
		u16 wWidth = m_tParam[byLoop].GetVideoWidth();
		if( wWidth == 704 && MEDIA_TYPE_MP4 == m_tParam[byLoop].GetEncType() )
		{
			wWidth = 720;
		}
		atOrigEncParam[byLoop].m_wVideoWidth = wWidth;
		atOrigEncParam[byLoop].m_wVideoHeight = m_tParam[byLoop].GetVideoHeight();
		atOrigEncParam[byLoop].m_byFrameRate = m_tParam[byLoop].GetFrameRate();
	}


	//�������ķֱ��ʣ���ֹstart��ʱ���е������ն˽����ȱ�С�ֱ��ʣ�Ȼ��ɾ�����ն˺���ֱ��ʣ�ý�ز�֧��
	dwTick = OspTickGet();
	nRet = m_cHardMulPic.SetVideoEncParam(&atOrigEncParam[0], 
										m_byValidVmpOutNum);
	KeyLog("=================[MsgStartVidMixProc] m_cHardMulPic.SetVideoEncParam Tick:%d\n",OspTickGet() - dwTick );
	KeyLog("[MsgStartVidMixProc]byValidEncNum:%d\n",byValidEncNum);
	if ( HARD_MULPIC_OK != nRet )
	{
		ErrorLog( " SetVideoEncParam failed, Error code: %u\n", nRet );
		cServMsg.SetErrorCode( 1 );              
		SendMsgToMcu( pMsg->event + 2, cServMsg );
		return;
	}
	// ����ʼ�����շ�������ǰ��startmerge���ܱ�֤���ʻص�������ʱ��㣬���ܻᵼ�´���
	if (!StartNetRcvSnd(byNeedPrs))
	{
		ErrorLog( "StartNetRcvSnd failed!\n");
		cServMsg.SetErrorCode( 1 );                 // FIXME: �������ϱ�����
		SendMsgToMcu( pMsg->event + 2, cServMsg );
		return;
	}
	dwTick = OspTickGet();
	nRet = m_cHardMulPic.StartMerge();
	KeyLog("============[MsgStartVidMixProc] m_cHardMulPic.StartMerge Tick:%d\n",OspTickGet() - dwTick);
	if ( HARD_MULPIC_OK != nRet )
	{
		ErrorLog( " startmerge failed, Error code: %u\n", nRet );
		cServMsg.SetErrorCode( 1 );              
		SendMsgToMcu( pMsg->event + 2, cServMsg );
		return;
	}
	//��ʵ������Ҫ����ķֱ���
	for ( u8 byChnLoop = 0 ; byChnLoop < m_byValidVmpOutNum;byChnLoop++)
	{
		if ( atOrigEncParam[byChnLoop].m_wVideoWidth != m_tMpuVmpCfg.m_tVideoEncParam[byChnLoop].m_wVideoWidth
			|| atOrigEncParam[byChnLoop].m_wVideoHeight != m_tMpuVmpCfg.m_tVideoEncParam[byChnLoop].m_wVideoHeight 
			|| atOrigEncParam[byChnLoop].m_byFrameRate != m_tMpuVmpCfg.m_tVideoEncParam[byChnLoop].m_byFrameRate )
		{
			dwTick = OspTickGet();
			nRet = m_cHardMulPic.ChangeVideoEncParam(byChnLoop,&m_tMpuVmpCfg.m_tVideoEncParam[byChnLoop]);
			KeyLog("============[MsgStartVidMixProc] m_cHardMulPic.ChangeVideoEncParam(%d) Tick:%d\n",byChnLoop,OspTickGet() -dwTick);
			if ( HARD_MULPIC_OK != nRet )
			{
				ErrorLog("[MsgStartVidMixProc ]chnl:%d,changevideoEncParam failed!\n",byChnLoop);
				cServMsg.SetErrorCode( 1 );              
				SendMsgToMcu( pMsg->event + 2, cServMsg );
				return;
			}
		}
		
	}
	//8. ���ñ߿򱳾���
	if( !SetVmpAttachStyle( m_tStyleInfo ) )
	{
		ErrorLog( " Fail to set vmp attach style(MsgStartVidMixProc)\n" );
		cServMsg.SetMsgBody( NULL, 0 );
		cServMsg.SetErrorCode( 1 );          
		SendMsgToMcu( pMsg->event + 2, cServMsg );
		return;
	}
	for (u8 byChNum = 0;byChNum < byMaxChnNum;byChNum++)
	{
		if (m_bAddVmpChannel[byChNum] == TRUE)
		{
			//�ն˱�����ʾҪ�ŵ�startmerge֮����
			if ( IsDisplayMbAlias() )
			{
				//DisplayMbAlias(byChNum,m_tVmpCommonAttrb.m_atMtMember[byChNum].GetMbAlias());
                DisplayMbAlias(byChNum,m_tVmpMbAlias[byChNum].GetMbAlias());                
			}
		}
	}
	KeyLog( "StartMerge succeed!\n" );


	//9. ����ƽ������
	BOOL32 bSetRule = !g_bPauseSS;
	dwTick = OspTickGet();
	SetSmoothSendRule( bSetRule );
	KeyLog("===================[MsgStartVidMixProc] SetSmoothSendRule Tick:%d\n",OspTickGet() -dwTick);


	//11��״̬��ΪRUNNING��ͬʱ��mcu��ACK��NOTIFY��ͨ��״̬
	NEXTSTATE((u32)RUNNING);

	SendMsgToMcu( pMsg->event + 1, cServMsg );
	SendMsgToMcu( VMP_MCU_STARTVIDMIX_NOTIF, cServMsg );

	m_cConfId = cServMsg.GetConfId();
	SendStatusChangeMsg( (u8)ONLINE, (u8)USESTATE_START ); 
	KeyLog( "Vmp works properly now, cur state.%u\n", CurState() );

	//12��������ؼ�֡��timer
	//[nizhijun 2011/03/21]VMP�����ı���Ҫ��1����ؼ�֡����
	//[nizhijun 2011/06/21]���ÿ��ͨ����ʱ����ؼ�֡��
	memset( m_abChnlIframeReqed, 0, sizeof(m_abChnlIframeReqed) );
	for (u8 byChNumIdx = 0;byChNumIdx < byMaxChnNum;byChNumIdx++)
	{
		if (m_bAddVmpChannel[byChNumIdx] == TRUE)
		{
			SetTimer(EV_VMP_NEEDIFRAME_TIMER+byChNumIdx,CHECK_IFRAME_INTERVAL,byChNumIdx);
		}
	}

	// ��Կ����������ش��¼���������ִ��һ��start+���change��Ա������ʱ�ϳ�������mcu����vmp��ؼ�֡�Ӻ��ն˽��ճ�����������
	// ׷��һ����־���ڿ���ʱ�����˱�־��TRUE�����������������change���ǹؼ�֡������change�������ʱ׷�ӶԸ�·��������ؼ�֡���ٽ���־��FALSE������Чһ��
	// ���������յ����ǹؼ�֡�������־λ��FALSE��ֱ��ʧЧ
	m_bFastUpdate = TRUE;

	KeyLog("===========[MsgStartVidMixProc] End Tick:%d\n",OspTickGet());
    return;
}

/*====================================================================
	������  :UnpackVmpWholeOperMsg
	����    :����VMP���������Ϣ
	����    :
	���    :
	����ֵ  :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2013/07/03  4.7         liaokang        ����
====================================================================*/
void CMpu2VmpInst::UnpackVmpWholeOperMsg(CServMsg & cServMsg, BOOL32 bStart)
{
    u8* pContent = cServMsg.GetMsgBody();
    u16 wMsgBufLen =0;

    BOOL32 bVmpMbAliasExtend = FALSE;

    //1���ж��Ƿ�ģ��mpu
    if ( IsSimulateMpu() )
    {       
        CKDVNewVMPParam tParam[MAXNUM_MPUSVMP_CHANNEL];    // ��·��Ƶ���ͨ��
        memset( tParam, 0, sizeof(tParam) );
        memcpy( (u8*)tParam, pContent, sizeof(tParam)); 
        pContent += sizeof(tParam);
        wMsgBufLen += sizeof(tParam);
        
        KeyLog( "/*---------simulate mpu param---------------------------------------------------------*/\n\n" );
        KeyLog( "******************************************:\n\n" );
        s32 nIdx = 0;
        for(nIdx = 0; nIdx < m_byMaxVmpOutNum; nIdx++)
        {
            KeyLog( "CHNNL.%d: Type=%d BitRate=%d VideoHeight:%d VideoWidth:%d,framerate:%d \n",
                nIdx,
                tParam[nIdx].m_byEncType, tParam[nIdx].GetBitRate(), 
                tParam[nIdx].GetVideoHeight(), tParam[nIdx].GetVideoWidth(),
                tParam[nIdx].GetFrameRate());
        }
        KeyLog( "/*************************************/\n\n" );        
        
        //2��mpu2 ������Ϣ�� TMPU2CommonAttrb+9* CKDVVMPOutParam
        memcpy(m_tVmpCommonAttrb.m_atMtMember,tParam[0].m_atMtMember,sizeof(TVMPMemberEx)*MAXNUM_MPUSVMP_MEMBER);
        m_tVmpCommonAttrb.m_byMemberNum = tParam[0].m_byMemberNum;
        m_tVmpCommonAttrb.m_byVmpStyle = tParam[0].m_byVMPStyle;
        memcpy(m_tVmpCommonAttrb.m_tDoublePayload ,tParam[0].m_tDoublePayload,sizeof(TDoublePayload)*MAXNUM_MPUSVMP_MEMBER);
        memcpy(m_tVmpCommonAttrb.m_tVideoEncrypt,tParam[0].m_tVideoEncrypt,sizeof(TMediaEncrypt)*MAXNUM_MPUSVMP_MEMBER);

        m_tVmpCommonAttrb.m_byNeedPrs = *(u8*)pContent;
        pContent += sizeof(u8);
        wMsgBufLen += sizeof(u8);

        m_tStyleInfo = *(TVmpStyleCfgInfo*)pContent;
        pContent += sizeof(TVmpStyleCfgInfo);
        wMsgBufLen += sizeof(TVmpStyleCfgInfo);
        m_tVmpCommonAttrb.m_tVmpStyleCfgInfo = m_tStyleInfo;

        //  [1/12/2010 pengjie] Modify ���ÿ���ͨ����ʾģʽ����������һ֡��ͼƬ...��
        TVMPExCfgInfo tVMPExCfgInfo;
        if( cServMsg.GetMsgBodyLen() > wMsgBufLen )
        {
            tVMPExCfgInfo = *(TVMPExCfgInfo*)pContent;
            pContent += sizeof(TVMPExCfgInfo);
            wMsgBufLen += sizeof(TVMPExCfgInfo);
        }
        else
        {
            tVMPExCfgInfo.m_byIdleChlShowMode = VMP_SHOW_BLACK_MODE;
        }
        
        if( tVMPExCfgInfo.m_byIdleChlShowMode > VMP_SHOW_USERDEFPIC_MODE )
        {
            tVMPExCfgInfo.m_byIdleChlShowMode = VMP_SHOW_BLACK_MODE; // Ĭ����ʾ����
        }
        KeyLog( "[TransMpuParam2Mpu2Param] Vmp IdleChlShowMode: %d \n", tVMPExCfgInfo.m_byIdleChlShowMode );
        m_tVmpCommonAttrb.m_tVmpExCfgInfo = tVMPExCfgInfo;
        
#ifdef WIN32//songkun,20110628,����MPU WIN32 B�����
        ;
#else
#ifndef _MPUB_256_	//A��
        memset(&tParam[1], 0, sizeof(CKDVNewVMPParam) );
#endif
#endif		
        
        for (u8 byLoop  = 0 ; byLoop  <  m_byMaxVmpOutNum ; byLoop++)
        {
            //���mpu�����Ƿ��пղΣ��еĻ��͸ĳɺϷ��Ĳ���������������Ϊ0
            if ( tParam[byLoop].m_byEncType == MEDIA_TYPE_NULL || tParam[byLoop].GetVideoWidth() == 0 || 
                (tParam[byLoop].GetVideoHeight() == 0 ) )
            {
                MpuNullParam2ValidParam(tParam[byLoop],byLoop);
            }
            else if ( tParam[byLoop].GetFrameRate() == 0 )
            {
                MpuFrameRateCheck(tParam[byLoop]);
            }
            //1080 720 60/50����֡��Ҫ��Ϊ30/25
            u8 byFrameRate = tParam[byLoop].GetFrameRate();
            if ( byFrameRate >=  50 )
            {
                tParam[byLoop].SetFrameRate(byFrameRate/2);
            }
            m_tParam[byLoop].SetBitRate(tParam[byLoop].GetBitRate());
            m_tParam[byLoop].SetEncType(tParam[byLoop].m_byEncType);
            m_tParam[byLoop].SetFrameRate(tParam[byLoop].GetFrameRate());
            m_tParam[byLoop].SetProfileType(0);
            m_tParam[byLoop].SetVideoHeight(tParam[byLoop].GetVideoHeight());
            m_tParam[byLoop].SetVideoWidth(tParam[byLoop].GetVideoWidth());
        }	      
    }
    else 
    { 
        memcpy((u8*)&m_tVmpCommonAttrb,pContent,sizeof(TMPU2CommonAttrb));
        pContent += sizeof(TMPU2CommonAttrb);
        wMsgBufLen += sizeof(TMPU2CommonAttrb);

        memcpy( (u8*)m_tParam,pContent,MAXNUM_MPU2VMP_CHANNEL*sizeof(CKDVVMPOutMember));
        pContent += (MAXNUM_MPU2VMP_CHANNEL*sizeof(CKDVVMPOutMember));
        wMsgBufLen += (MAXNUM_MPU2VMP_CHANNEL*sizeof(CKDVVMPOutMember));

        if (cServMsg.GetMsgBodyLen() > wMsgBufLen)
        {    
            u8 byTypeNum = *(u8*)pContent;
            pContent += sizeof(u8);
            wMsgBufLen += sizeof(u8);

            u16 wLength = 0;

            while( (byTypeNum-- > 0) && (cServMsg.GetMsgBodyLen() > wMsgBufLen) )
            {
                emVmpCommType etype = (emVmpCommType)(*pContent);
                pContent ++;
                wLength = ntohs( *(u16*)pContent);
                pContent += sizeof(u16);
                switch (etype)
                {
                case emVmpMbAlias:
                    {
                        bVmpMbAliasExtend = TRUE;
						// ����ʱ��������б�����Ϣ���ı��Աʱ�����жϳ�Ա
						if (bStart)
						{
							memset(m_tVmpMbAlias, 0, sizeof(m_tVmpMbAlias));
						}
						else
						{
							for (u8 byIdx=0; byIdx<MAXNUM_MPU2VMP_MEMBER;byIdx++)
							{
								if (!(m_tVmpCommonAttrb.m_atMtMember[byIdx] == m_tPreviousVmpCommonAttrb.m_atMtMember[byIdx]))
								{
									memset(&m_tVmpMbAlias[byIdx], 0, sizeof(TVmpMbAlias));
								}
							}
						}

                        u16 wTypeLen = 0;
                        u8 byChnlNum = *(u8*)pContent;
                        pContent ++;
                        wTypeLen ++;
                        u8 byChnlNo = 0;
                        u8 byChnlAliasLen = 0;
                        for(u8 byLoop = 0; byLoop < byChnlNum; byLoop++)
                        {
                            byChnlNo = *(u8*)pContent;
                            pContent ++;
                            wTypeLen ++;
                            byChnlAliasLen = *(u8*)pContent;
                            pContent ++;
                            wTypeLen ++;
                            if ( byChnlNo < MAXNUM_MPU2VMP_MEMBER )
                            {
								// ����ʱ��ֱ�����ñ���
								if (bStart)
								{
									m_tVmpMbAlias[byChnlNo].SetMbAlias(byChnlAliasLen,(s8*)pContent);
								}
								else
								{
									// ����ʱ�������¾�ͨ����Ա�Ƿ�ı䣬��Աδ�ı�ʱ�������±���
									if (!(m_tVmpCommonAttrb.m_atMtMember[byChnlNo] == m_tPreviousVmpCommonAttrb.m_atMtMember[byChnlNo]))
									{
										m_tVmpMbAlias[byChnlNo].SetMbAlias(byChnlAliasLen,(s8*)pContent);
									}
								}
                            }
                            
                            pContent += byChnlAliasLen;
                            wTypeLen += byChnlAliasLen;
                        }
                        pContent -= wTypeLen;
                    }
                    break;
                default:
                    KeyLog("Unknow Vmp Communication Type %d!\n", etype);
                    break;
                }
                pContent += wLength;
                wMsgBufLen += sizeof(wLength);
            }
        }	
	}

    //����
    if ( FALSE == bVmpMbAliasExtend )
    {
        u8 byMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle( m_tVmpCommonAttrb.m_byVmpStyle );
        for (u8 byChnNo = 0; byChnNo < byMaxChnNum; byChnNo++ )
        {
            if ( !m_tVmpCommonAttrb.m_atMtMember[byChnNo].IsNull())
            {
                m_tVmpMbAlias[byChnNo].SetMbAlias(strlen(m_tVmpCommonAttrb.m_atMtMember[byChnNo].GetMbAlias()),
                    m_tVmpCommonAttrb.m_atMtMember[byChnNo].GetMbAlias());
            }
        }
    }
}

/*====================================================================
	������  :MpuNullParam2ValidParam
	����    :mpu�ղ�����Ϊ��Ч����,��������Ҫ����Ϊ0
	����    :
	���    :
	����ֵ  :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2013/01/18  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::MpuNullParam2ValidParam(CKDVNewVMPParam& tInOutMpuParam, u8 byChnIdx)
{
	if ( byChnIdx >= MAXNUM_MPUSVMP_CHANNEL)
	{
		ErrorLog("[MpuNullParam2ValidParam]invalid chidx:%d\n",byChnIdx);
		return;
	}
	//mpu 0:1080,1:720,2:cif(other),3(4cif)
	u8 byVideoFormat[]={VIDEO_FORMAT_HD1080,VIDEO_FORMAT_HD720,VIDEO_FORMAT_CIF,VIDEO_FORMAT_4CIF};
	u16 witdth = 0,wheight = 0 ;
	GetWHViaRes(byVideoFormat[byChnIdx],witdth,wheight);
	tInOutMpuParam.SetVideoWidth(witdth);
	tInOutMpuParam.SetVideoHeight(wheight);
	switch (byChnIdx)
	{
	case 0:
		tInOutMpuParam.m_byEncType = MEDIA_TYPE_H264;
		tInOutMpuParam.SetBitRate(0);
		tInOutMpuParam.SetFrameRate(30);
		break;
	case 1:
		tInOutMpuParam.m_byEncType = MEDIA_TYPE_H264;
		tInOutMpuParam.SetBitRate(0);
		tInOutMpuParam.SetFrameRate(30);
		break;
	case 2:
		tInOutMpuParam.m_byEncType = MEDIA_TYPE_H264;
		tInOutMpuParam.SetBitRate(0);
		tInOutMpuParam.SetFrameRate(25);
		break;
	case 3:
		tInOutMpuParam.m_byEncType = MEDIA_TYPE_H264;
		tInOutMpuParam.SetBitRate(0);
		tInOutMpuParam.SetFrameRate(25);
		break;
	default:
		break;
	}
	
}

/*====================================================================
	������  :MpuFrameRateCheck
	����    :mpu framerate��0�Ļ�����������
	����    :
	���    :
	����ֵ  :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2013/01/18  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::MpuFrameRateCheck(CKDVNewVMPParam& tInOutMpuParam)
{
	if (tInOutMpuParam.GetFrameRate() != 0)
	{
		return;
	}
	switch (tInOutMpuParam.GetVideoWidth())
	{
	case 1920:
		tInOutMpuParam.SetFrameRate(30);
		break;
	case 1280:
		tInOutMpuParam.SetFrameRate(30);
		break;
	case 352:
		tInOutMpuParam.SetFrameRate(25);
		break;
	case 704:
	case 720:
			tInOutMpuParam.SetFrameRate(25);
		break;
	default:
		break;
	}
	
}
/*====================================================================
	������  :MsgStopVidMixProc
	����    :ֹͣ������������
	����    :CMessage
	���    :
	����ֵ  :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::MsgStopVidMixProc( CMessage * const pMsg )
{
    CServMsg cServMsg( pMsg->content, pMsg->length );

    switch ( CurState() )
    {
    case RUNNING:
        break;
    default:
		SendMsgToMcu( pMsg->event + 2, cServMsg );
        ErrorLog( "[MsgStopVidMixProc] Wrong State:%d\n", CurState());
        return;
    }

    u8 byChnNo = 0;
	//0. ֹͣ�ϳɱ���������ص�
    for ( byChnNo = 0; byChnNo < m_byValidVmpOutNum; byChnNo++ )
    {
        m_cHardMulPic.SetVidDataCallback( byChnNo, NULL, 0 );
    }
    //1. ֹͣ����
    StopNetRcvSnd();

 


	//3��ֹͣ����
    s32 nRetCode = m_cHardMulPic.StopMerge();
	KeyLog( "Call stopmerge() error code= %d (0 is ok)\n", nRetCode );
    if ( HARD_MULPIC_OK == nRetCode )
    {
        cServMsg.SetErrorCode(0);
    }
    else
    {
        cServMsg.SetErrorCode(1);
    }
	//4�����ͨ��
	u8 byMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle(m_tVmpCommonAttrb.m_byVmpStyle);;

	
    for ( byChnNo = 0; byChnNo < byMaxChnNum; byChnNo++ )
    {
        m_cHardMulPic.RemoveChannel( byChnNo );
		if (IsDisplayMbAlias())
		{
			m_cHardMulPic.StopAddIcon(byChnNo);
			KeyLog("[MsgStopVidMixProc]Stop AddIcon chnl:%d\n",byChnNo);
		}
		KeyLog("[MsgStopVidMixProc] remove chnl:%d\n",byChnNo);
    } 

	//5����mcu����Ӧ
	SendMsgToMcu( pMsg->event + 1, cServMsg );
    SendMsgToMcu( VMP_MCU_STOPVIDMIX_NOTIF, cServMsg );
    SendStatusChangeMsg( (u8)ONLINE, (u8)USESTATE_IDLE );

	memset(m_abChnlIframeReqed,0,sizeof(m_abChnlIframeReqed));
	memset( m_bAddVmpChannel, 0, sizeof(m_bAddVmpChannel) );
    for (u8 byChn =0; byChn < MAXNUM_MPU2VMP_MEMBER; byChn++ )
	{
		KillTimer(EV_VMP_NEEDIFRAME_TIMER+byChn);
    }
	SetIsDisplayMbAlias(FALSE);

	   //2��ֹͣƽ��
	//[2010/12/31 zhushz]ƽ�����ͽӿڷ�װ
	SetSmoothSendRule(FALSE);
	
	//��vmp��ģʽ�޸Ļس�ʼģʽ
	m_cHardMulPic.ChangeMode(m_dwInitVmpMode);
	
	//���previous��صĳ�Ա
	memset(m_tParamPrevious, 0, sizeof(m_tParamPrevious));
	memset(&m_tPreviousVmpCommonAttrb, 0, sizeof(m_tPreviousVmpCommonAttrb));
    memset(m_tVmpMbAlias, 0, sizeof(m_tVmpMbAlias));
	
#ifdef _LINUX_
	BOOL32 bRet = CloseSmoothSending();
	KeyLog("close smooth result: %u\n", bRet);
#endif
	//״̬�б�Ϊnoraml
    NEXTSTATE((u32)NORMAL);

//     #ifndef WIN32
// 	    BrdLedStatusSet( LED_SYS_LINK, BRD_LED_OFF );// ��MLINK����
// 	#endif

    return ;
} 

/*====================================================================
	����  : MsgChangeVidMixParamProc
	����  : �ı仭�渴�ϲ�����Ϣ��Ӧ
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::MsgChangeVidMixParamProc( CMessage* const pMsg )
{
	//0�������ĺϷ����ж�
	if ( NULL == pMsg )
    {
        ErrorLog( " The pointer cannot be Null (MsgChangeVidMixParamProc)\n" );
        return;
    }    
	if ( CurState() != RUNNING ) 
	{
		 ErrorLog( "[MsgChangeVidMixParamProc] Wrong State:%d !\n", CurState() );
		return;
	}

	//1��������һ�β���
	memset(m_tParamPrevious, 0, sizeof(m_tParamPrevious));
	memcpy(m_tParamPrevious, m_tParam, sizeof(m_tParam));
	memset(&m_tPreviousVmpCommonAttrb,0,sizeof(TMPU2CommonAttrb));
	memcpy(&m_tPreviousVmpCommonAttrb,&m_tVmpCommonAttrb,sizeof(TMPU2CommonAttrb));
	CServMsg cServMsg( pMsg->content, pMsg->length ); 
    UnpackVmpWholeOperMsg(cServMsg, FALSE);

    BOOL32 bCheckRet = FALSE;
    
    m_tStyleInfo = m_tVmpCommonAttrb.m_tVmpStyleCfgInfo;
    
    //3�����ͱ߿��б�

	//���û���ϳɷ��
	bCheckRet = ChangeVMPStyle();
	if ( bCheckRet )
	{
		cServMsg.SetErrorCode( 0 ); // ok
	}
	else
	{
		 ErrorLog( " Failed to set ChangeVMPStyle (MsgChangeVidMixParamProc)\n" );
		cServMsg.SetMsgBody( NULL, 0 );
		cServMsg.SetErrorCode( 1 );
		SendMsgToMcu( pMsg->event + 2, cServMsg );
		return;
	}

	//���ñ߿����ɫ
	BOOL32 bRet = SetVmpAttachStyle( m_tStyleInfo );
	if ( !bRet )
	{
		ErrorLog( " Failed to set vmp attach style (MsgChangeVidMixParamProc)\n" );
		cServMsg.SetMsgBody( NULL, 0 );
		cServMsg.SetErrorCode( 1 );
		SendMsgToMcu( pMsg->event + 2, cServMsg );
		return;
	}

	SetIdleChnShowMode(m_tVmpCommonAttrb.m_tVmpExCfgInfo.m_byIdleChlShowMode,FALSE);

    //4��ͨ���Ͷ�Ӧ��net���� �б�
	BOOL32 bStyleChange = ( m_tPreviousVmpCommonAttrb.m_byVmpStyle != m_tVmpCommonAttrb.m_byVmpStyle );
    ChangeChnAndNetRcv( bStyleChange );
	
	//5���ı�����
	for(u8 byChnnlId = 0; byChnnlId < MAXNUM_MPU2VMP_CHANNEL; byChnnlId ++)
	{  
		//�������ף���������
		if ( IsEnableBitrateCheat() )
		{   
			m_tParam[byChnnlId].SetBitRate( GetBitrateAfterCheated(m_tParam[byChnnlId].GetBitRate()) );
			KeyLog( "New Enc Bitrate(chl.%d): %d\n", byChnnlId, m_tParam[byChnnlId].GetBitRate() );	
		}
		
		if( m_tParam[byChnnlId].GetBitRate() != m_tParamPrevious[byChnnlId].GetBitRate() )
		{
			//change br
			m_cHardMulPic.ChangeBitRate( byChnnlId, m_tParam[byChnnlId].GetBitRate() );
			//�������ʣ������ʷֱ���������Ϣ���õ��µķֱ��ʺ�֡�ʣ����Ƿ���ϵ���ͬ������ͬ�������²�
			u16 wVideoOldWith = m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId].m_wVideoWidth;
			u16 wVideoOldHeight = m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId].m_wVideoHeight;
			u8 byOldFramerate = m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId].m_byFrameRate;
			m_tMpuVmpCfg.GetDefaultParam( m_tParam[byChnnlId].GetEncType(),
				m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId],
				m_tMpuVmpCfg.m_adwMaxSendBand[byChnnlId] );
			TransFromMpu2Param2EncParam(&m_tParam[byChnnlId],&m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId]);
			if (	m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId].m_wVideoWidth != wVideoOldWith || 
					m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId].m_wVideoHeight != wVideoOldHeight || 
					byOldFramerate != m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId].m_byFrameRate
					)
			{
				m_cHardMulPic.ChangeVideoEncParam( byChnnlId,&m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId] );
//				m_cHardMulPic.SetEncChnlParam( &m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId], byChnnlId );
				mpulog( MPU_INFO,"[MsgSetBitRate] set encchan:%d new encparam:\n",byChnnlId);
				m_tMpuVmpCfg.PrintEncParam( byChnnlId );	
			}
		
		}
    }   



	//[2010/12/31 zhushz]ƽ�����ͽӿڷ�װ
	//6������ƽ��
	BOOL32 bSetRule = !g_bPauseSS;
	SetSmoothSendRule( bSetRule );


	//7����ACK����Notify����״̬
	SendMsgToMcu( pMsg->event + 1, cServMsg );
    SendMsgToMcu( VMP_MCU_CHANGESTATUS_NOTIF, cServMsg );
    KeyLog( "[[MsgChangeVidMixParamProc]]change param success,vmp work according to new params!\n" );
    
    SendStatusChangeMsg( (u8)ONLINE, (u8)USESTATE_START );
	
	// ׷������ؼ�֡����,��Ժ�����¼���ش��ն˵Ŀ����ϳɣ���һ��change���ӱ���ͨ���ؼ�֡����
	if (m_bFastUpdate)
	{
		s32 nRet;
		m_bFastUpdate = FALSE;// ����Ч1��
		for (u8 byIdx=0; byIdx<MAXNUM_MPU2VMP_CHANNEL; byIdx++)
		{
			if (byIdx == m_byValidVmpOutNum)
			{
				break;
			}
			nRet = m_cHardMulPic.SetFastUpdata( byIdx );
			if (nRet != HARD_MULPIC_OK)
			{
				KeyLog("[MsgChangeVidMixParamProc]m_cHardMulPic.SetFastUpdata(%u) failed!\n",byIdx );
			}
			else
			{
				KeyLog("[MsgChangeVidMixParamProc]m_cHardMulPic.SetFastUpdata(%u)! succeed\n",byIdx );
			}
		}
	}

    //8��������ؼ�֡��timer
	//[nizhijun 2011/06/21]���ÿ��ͨ����ʱ����ؼ�֡
	memset( m_abChnlIframeReqed, 0, sizeof(m_abChnlIframeReqed) );
	u8 byMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle(m_tVmpCommonAttrb.m_byVmpStyle);
	for (u8 byChNum = 0;byChNum < byMaxChnNum; byChNum++)
	{
		if (m_bAddVmpChannel[byChNum] == TRUE)
		{
			KillTimer(EV_VMP_NEEDIFRAME_TIMER+byChNum);
			SetTimer(EV_VMP_NEEDIFRAME_TIMER+byChNum,CHECK_IFRAME_INTERVAL,byChNum);
		}
	}
    return ;
}

/*====================================================================
	����  : MsgSetBitRate
	����  : �ı䷢������
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::MsgSetBitRate( CMessage* const pMsg )
{
	//1�������Ϸ���У��
	if (NULL == pMsg)
	{
		ErrorLog("[MsgSetBitRate]Invalid pMsg��NULL��\n");
		return;
	}
	//2��������Ϣ�壬ͨ���ź�Ҫ���õ�����
    CServMsg cServMsg( pMsg->content, pMsg->length );
    u8 byChalNum = cServMsg.GetChnIndex();
    u16 wBitrate = 0;


    cServMsg.GetMsgBody( (u8*)&wBitrate, sizeof(u16) ) ;
    wBitrate = ntohs(wBitrate);

    KeyLog( "[MsgSetBitRate]now we set the encode-%u bitrate to %u!\n", byChalNum, wBitrate );

	//3���������׵���
    // guzh [10/18/2007] ֧����������
	u16 wCheatBit = wBitrate;
    if ( IsEnableBitrateCheat() )
    {
        wCheatBit = GetBitrateAfterCheated( wBitrate );
    }   
	
    m_cHardMulPic.ChangeBitRate( byChalNum, wCheatBit );
	


	//4������ͨ��������
// 	if ( IsSimulateMpu() )
// 	{
// 		m_tMpuParam[byChalNum].SetBitRate(wCheatBit);
// 		if( m_tMpuParam[byChalNum].GetBitRate() != m_tMpuParamPrevious[byChalNum].GetBitRate() )
// 		{
// 			m_tMpuParamPrevious[byChalNum].SetBitRate(wCheatBit);
// 			//�������ʣ������ʷֱ���������Ϣ���õ��µķֱ��ʺ�֡�ʣ����Ƿ���ϵ���ͬ������ͬ�������²�
// 			u16 wVideoOldWith = m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_wVideoWidth;
// 			u16 wVideoOldHeight = m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_wVideoHeight;
// 			u8 byOldFramerate = m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_byFrameRate;
// 			m_tMpuVmpCfg.GetDefaultParam( m_tMpuParam[byChalNum].m_byEncType,
// 				m_tMpuVmpCfg.m_tVideoEncParam[byChalNum],
// 				m_tMpuVmpCfg.m_adwMaxSendBand[byChalNum] );
// 			TransFromMpuParam2EncParam(&m_tMpuParam[byChalNum],&m_tMpuVmpCfg.m_tVideoEncParam[byChalNum]);
// 			if ( m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_wVideoWidth != wVideoOldWith 
// 					|| m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_wVideoHeight != wVideoOldHeight 
// 					|| m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_byFrameRate != byOldFramerate)
// 			{
// 				m_cHardMulPic.ChangeVideoEncParam( byChalNum,&m_tMpuVmpCfg.m_tVideoEncParam[byChalNum] );
// //				m_cHardMulPic.SetEncChnlParam( &m_tMpuVmpCfg.m_tVideoEncParam[byChalNum], byChalNum );
// 				mpulog( MPU_INFO,"[MsgSetBitRate] set encchan:%d new encparam:\n",byChalNum);
// 				m_tMpuVmpCfg.PrintEncParam( byChalNum );	
// 			}
// 			
// 		}
// 	}
// 	else
	{
		m_tParam[byChalNum].SetBitRate(wCheatBit);
		if( m_tParam[byChalNum].GetBitRate() != m_tParamPrevious[byChalNum].GetBitRate() )
		{
			m_tParamPrevious[byChalNum].SetBitRate(wCheatBit);
			
			//�������ʣ������ʷֱ���������Ϣ���õ��µķֱ��ʺ�֡�ʣ����Ƿ���ϵ���ͬ������ͬ�������²�
			u16 wVideoOldWith = m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_wVideoWidth;
			u16 wVideoOldHeight = m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_wVideoHeight;
			u8 byOldFramerate = m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_byFrameRate;
			m_tMpuVmpCfg.GetDefaultParam( m_tParam[byChalNum].GetEncType(),
				m_tMpuVmpCfg.m_tVideoEncParam[byChalNum],
				m_tMpuVmpCfg.m_adwMaxSendBand[byChalNum] );
			TransFromMpu2Param2EncParam(&m_tParam[byChalNum],&m_tMpuVmpCfg.m_tVideoEncParam[byChalNum]);
			if (	m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_wVideoWidth != wVideoOldWith || 
					m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_wVideoHeight != wVideoOldHeight ||
					m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_byFrameRate != byOldFramerate)
			{
				m_cHardMulPic.ChangeVideoEncParam( byChalNum,&m_tMpuVmpCfg.m_tVideoEncParam[byChalNum] );
				mpulog( MPU_INFO,"[MsgSetBitRate] set encchan:%d new encparam:\n",byChalNum);
				m_tMpuVmpCfg.PrintEncParam( byChalNum );	
			}
		}
	}

	
	//[2010/12/31 zhushz]ƽ�����ͽӿڷ�װ
	BOOL32 bSetRule = !g_bPauseSS;
	SetSmoothSendRule( bSetRule );



    SendMsgToMcu( VMP_MCU_SETCHANNELBITRATE_ACK, cServMsg );
}

/*====================================================================
	����  : SendStatusChangeMsg
	����  : ״̬�ı��֪ͨMCU
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::SendStatusChangeMsg( u8 byOnline, u8 byState/*, u8 byStyle */)
{
    CServMsg cServMsg;
    TPeriEqpStatus tInitStatus;
    memset( &tInitStatus,0,sizeof(TPeriEqpStatus) );

    tInitStatus.m_byOnline = byOnline;
    tInitStatus.SetMcuEqp( (u8)m_tEqp.GetMcuId(), m_tEqp.GetEqpId(), m_tEqp.GetEqpType() );
    tInitStatus.SetAlias( m_tCfg.achAlias );

	//boardver��mpu2�Ļ����ӿ��Ͳ����ӿ��ϱ���mpu��A��B��
	if ( IsSimulateMpu() )  //ģ��mpu
	{
		  tInitStatus.m_tStatus.tVmp.m_tVMPParam.SetVMPStyle( m_tVmpCommonAttrb.m_byVmpStyle );
		  tInitStatus.m_tStatus.tVmp.m_byChlNum = MAXNUM_MPUSVMP_MEMBER;
		#ifdef WIN32//songkun,20110628,����MPU WIN32 B�����
			tInitStatus.m_tStatus.tVmp.m_byBoardVer = MPU_BOARD_B256;
		#else    
			#ifndef _MPUB_256_ 
				tInitStatus.m_tStatus.tVmp.m_byBoardVer = MPU_BOARD_A128;
			#else
				tInitStatus.m_tStatus.tVmp.m_byBoardVer = MPU_BOARD_B256;
			#endif
		#endif
	}
	else //mpu2
	{
		 tInitStatus.m_tStatus.tVmp.m_tVMPParam.SetVMPStyle( m_tVmpCommonAttrb.m_byVmpStyle );
		 tInitStatus.m_tStatus.tVmp.m_byChlNum = MAXNUM_MPU2VMP_MEMBER;
		  BOOL32 bHasEcard = FALSE;
#ifdef _LINUX_
		  if ( BrdMpu2HasEcard() == 1)
		  {
			  bHasEcard = TRUE;
		  }
#else //win32
		  if ( g_cMpu2VmpApp.m_byWorkMode == TYPE_MPU2VMP_ENHANCED ||
				(g_cMpu2VmpApp.m_byWorkMode == TYPE_MPU2VMP_BASIC && g_cMpu2VmpApp.m_byVmpEqpNum == 2)
			  )
		  {
			  bHasEcard = TRUE;
		  }
#endif
		if ( bHasEcard ) 
		{
			tInitStatus.m_tStatus.tVmp.m_byBoardVer = MPU2_BOARD_ECARD;
		}
		else      //�����ӿ�
		{
			tInitStatus.m_tStatus.tVmp.m_byBoardVer = MPU2_BOARD;
		}
	}
  
    tInitStatus.m_tStatus.tVmp.m_byUseState = byState;

    KeyLog( "Before: alias: %s EqpId: %u EqpType: %u McuId: %u\n",
                                m_tCfg.achAlias, 
                                m_tEqp.GetEqpId(), 
                                m_tEqp.GetEqpType(), 
                                m_tEqp.GetMcuId() );
    KeyLog( "After: alias: %s EqpId: %u EqpType: %u McuId: %u\n",
                                m_tCfg.achAlias, 
                                tInitStatus.GetEqpId(), 
                                tInitStatus.GetEqpType(), 
                                tInitStatus.GetMcuId() );

   

	//ȡvmp��subtype
	u8 bySubType = GetSubTypeByWorkMode( g_cMpu2VmpApp.m_byWorkMode );
	tInitStatus.m_tStatus.tVmp.m_bySubType = bySubType; 
	
	KeyLog( "[SendStatusChangeMsg] mpu boardVer is %u\n", tInitStatus.m_tStatus.tVmp.m_byBoardVer);

    tInitStatus.SetAlias(m_tCfg.achAlias);
    cServMsg.SetMsgBody( (u8*)&(tInitStatus), sizeof(TPeriEqpStatus) );
    SendMsgToMcu( VMP_MCU_VMPSTATUS_NOTIF, cServMsg );
    return;
}

/*====================================================================
	����  : GetSubTypeByWorkMode
	����  : ͨ��workmodeȡ����Ӧ��vmpsubtype
	����  : u8 byWorkMode
	���  : u8 subtype
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
u8 CMpu2VmpInst::GetSubTypeByWorkMode(u8 byWorkMode)
{
	u8 bySubType = MPU2_VMP_BASIC;
	switch( byWorkMode ) 
	{
	case TYPE_MPUSVMP:
		bySubType = MPU_SVMP;
		break;
	case TYPE_MPU2VMP_BASIC:
		bySubType = MPU2_VMP_BASIC;
		break;
	case TYPE_MPU2VMP_ENHANCED:
		bySubType = MPU2_VMP_ENHACED;
		break;
	default:
		break;
	}
	return bySubType;
}

/*=============================================================================
  �� �� ���� MsgUpdateVmpEncryptParamProc
  ��    �ܣ� ���ļ��ܲ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pMsg
  �� �� ֵ�� void 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::MsgUpdateVmpEncryptParamProc( CMessage * const pMsg )
{ 
	//1�������Ϸ���У��
    if( NULL == pMsg )
	{
		ErrorLog( "[MsgUpdataVmpEncryptParamProc] Error input parameter.\n" );
		return;
	}
	//2��������Ϣ��
	u8  byChannelNo = 0;
    TMediaEncrypt  tVideoEncrypt;
    TDoublePayload tDoublePayload;

    CServMsg cServMsg( pMsg->content, pMsg->length );

    byChannelNo    = *(u8 *)( cServMsg.GetMsgBody() );
    tVideoEncrypt  = *(TMediaEncrypt *)( cServMsg.GetMsgBody() + sizeof(u8) );
    tDoublePayload = *(TDoublePayload *)( cServMsg.GetMsgBody() + sizeof(u8) + sizeof(TMediaEncrypt) );

    KeyLog( "byChannelNo: %d, ActivePayLoad: %d, RealPayLoad: %d\n", 
                                                        byChannelNo,
                                                        tDoublePayload.GetActivePayload(),
                                                        tDoublePayload.GetRealPayLoad() );

    SetDecryptParam( byChannelNo, &tVideoEncrypt/*, &tDoublePayload*/ );
    if ( !m_bAddVmpChannel[byChannelNo] )
    {      
        s32 nRet;
        nRet = m_cHardMulPic.AddChannel( byChannelNo );
        if ( HARD_MULPIC_OK == nRet )
        {
            m_bAddVmpChannel[byChannelNo] = TRUE;
        }

        KeyLog( "[Info]Channel-%u add return value is %u(correct is %u),the Ip is %u Port is %u!\n",
                                   byChannelNo, nRet, HARD_MULPIC_OK );

    }

	//3������payload
// 	if ( IsSimulateMpu() )
// 	{
// 		m_tMpuParamPrevious[0].m_tDoublePayload[byChannelNo].SetActivePayload(tDoublePayload.GetActivePayload());
// 		m_tMpuParamPrevious[0].m_tDoublePayload[byChannelNo].SetRealPayLoad(tDoublePayload.GetRealPayLoad());
// 
// 	}
// 	else
	{
		m_tPreviousVmpCommonAttrb.m_tDoublePayload[byChannelNo].SetActivePayload(tDoublePayload.GetActivePayload());
		m_tPreviousVmpCommonAttrb.m_tDoublePayload[byChannelNo].SetRealPayLoad(tDoublePayload.GetRealPayLoad());

	}

	//4���������б�������ļ��ܲ���
	// [7/31/2010 xliang] �������ñ������
	for(u8 byEncChnl = 0; byEncChnl < m_byMaxVmpOutNum; byEncChnl++)
	{
		if(!m_bUpdateMediaEncrpyt[byEncChnl])
		{
			u8 byEncType = m_tParam[byEncChnl].GetEncType();
// 			if ( IsSimulateMpu() ) 
// 			{
// 				byEncType = m_tMpuParam[byEncChnl].m_byEncType;
// 			}
			SetEncryptParam( byEncChnl, &tVideoEncrypt, byEncType );
			m_bUpdateMediaEncrpyt[byEncChnl] = TRUE;
		}
	}


    
	cServMsg.SetMsgBody( NULL, 0 );
    SendMsgToMcu( VMP_MCU_CHANGESTATUS_NOTIF, cServMsg );

    SendStatusChangeMsg( (u8)ONLINE, (u8)USESTATE_START );

	return;
}

/*====================================================================
	����  : MsgFastUpdatePicProc
	����  : 
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::MsgFastUpdatePicProc( CMessage * const pMsg )
{
	//1�������Ϸ���У��
	if ( NULL == pMsg ) 
	{
		ErrorLog("[MsgFastUpdatePicProc]Null pointer CMessage\n");
		return;
	}
	//2��������Ϣ�壬�������ĸ�����ͨ���Ĺؼ�֡
    CServMsg cServMsg( pMsg->content, pMsg->length );
	u8 byChnnlId = cServMsg.GetChnIndex();
	//3��1s�ڵ��ظ��������
	u32 dwTimeInterval = 1 * OspClkRateGet();
    u32 dwCurTick = OspTickGet();	
    if( dwCurTick - m_adwLastFUPTick[byChnnlId] > dwTimeInterval 
		)
    {
        m_adwLastFUPTick[byChnnlId] = dwCurTick;

		s32 nRet = m_cHardMulPic.SetFastUpdata( byChnnlId );
		if (nRet != HARD_MULPIC_OK)
		{
			ErrorLog("[MsgFastUpdatePicProc]m_cHardMulPic.SetFastUpdata(%u) failed!\n",byChnnlId );
		}
		else
		{
			KeyLog( "[MsgFastUpdatePicProc]m_cHardMulPic.SetFastUpdata(%u)! succeed\n",byChnnlId );
		}
        
    }
	
	// ���������յ��ؼ�֡���󣬵�������ؼ�֡����ʧЧ
	m_bFastUpdate = FALSE;

	return;
}

/*====================================================================
	����  : MsgTimerNeedIFrameProc
	����  : 
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::MsgTimerNeedIFrameProc(  CMessage * const pMsg )
{
	//1�������Ϸ���У��
	if ( NULL == pMsg ) 
	{
		ErrorLog("[MsgTimerNeedIFrameProc]Null pointer CMessage\n");
		return;
	}
	//2����RUNNING ״̬��������ؼ�֡�������壩
	KillTimer(pMsg->event);
	if ( CurState() != RUNNING ) 
	{
		ErrorLog("[MsgTimerNeedIFrameProc]wrong state:%d\n",CurState());
		return;
	}
	//3����mcu������ؼ�֡��Ϣ
	u8 byChnNo = *(u8 *)pMsg->content;
	if ( byChnNo < MAXNUM_MPU2VMP_MEMBER )
	{
		CServMsg cServMsg;
		cServMsg.SetConfId( m_cConfId );
		TMulPicChnStatis tDecStatis;
		if(m_bAddVmpChannel[byChnNo])
		{
			memset(&tDecStatis, 0, sizeof(TMulPicChnStatis));
			m_cHardMulPic.GetDecStatis( byChnNo, tDecStatis );
			if ( tDecStatis.m_bWaitKeyFrame	)
			{			
				m_abChnlIframeReqed[byChnNo]=TRUE;
				cServMsg.SetChnIndex(byChnNo);
				SendMsgToMcu(VMP_MCU_NEEDIFRAME_CMD, cServMsg);
				KeyLog("[MsgTimerNeedIFrameProc]vmp channel:%d request iframe!!\n", byChnNo);
				KeyLog("[MsgTimerNeedIFrameProc]vmp channel:%d  set EV_VMP_NEEDIFRAME_TIMER: %d !\n", byChnNo, m_tMpuVmpCfg.GetIframeInterval());
				SetTimer(pMsg->event, m_tMpuVmpCfg.GetIframeInterval(),byChnNo);
			}
			else
			{
				//�����ļ��Ķ�ʱδ��⵽������ݱ�־λ�ж��Ƿ��������ļ� 
				if ( m_abChnlIframeReqed[byChnNo]==TRUE )
				{
				
					DetailLog( "[MsgTimerNeedIFrameProc]vmp channel:%d  set EV_VMP_NEEDIFRAME_TIMER: %d !\n", byChnNo, m_tMpuVmpCfg.GetIframeInterval());
					SetTimer(pMsg->event,m_tMpuVmpCfg.GetIframeInterval(),byChnNo);
				}
				else
				{
				
					DetailLog( "[MsgTimerNeedIFrameProc]vmp channel:%d  set EV_VMP_NEEDIFRAME_TIMER: %d !\n", byChnNo, CHECK_IFRAME_INTERVAL);
					SetTimer(pMsg->event, CHECK_IFRAME_INTERVAL,byChnNo);
				}			
			}
		}
	}

	
}

/*=============================================================================
  �� �� ���� ProcGetMsStatusRsp
  ��    �ܣ� ����ȡ��������״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::ProcGetMsStatusRsp( CMessage* const pMsg )
{
	//1�������Ϸ���У��
	if ( NULL == pMsg ) 
	{
		ErrorLog("[ProcGetMsStatusRsp]Null pointer CMessage\n");
		return;
	}
	//2��ȡ��������״̬
	BOOL bSwitchOk = FALSE;
	CServMsg cServMsg( pMsg->content, pMsg->length );
	if( MCU_EQP_GETMSSTATUS_ACK == pMsg->event ) // Mcu��Ӧ��Ϣ
	{
		TMcuMsStatus *ptMsStatus = (TMcuMsStatus *)cServMsg.GetMsgBody();
        
        KillTimer( TIME_GET_MSSTATUS );
        KeyLog( "[ProcGetMsStatus]. receive msg MCU_EQP_GETMSSTATUS_ACK. IsMsSwitchOK :%d\n", 
                  ptMsStatus->IsMsSwitchOK() );

        if ( ptMsStatus->IsMsSwitchOK() ) // �����ɹ�
        {
            bSwitchOk = TRUE;
        }
	}
	//3�� ����ʧ�ܻ��߳�ʱ
	if( !bSwitchOk )
	{
		ClearCurrentInst();
		KeyLog( "[ProcGetMsStatus] ClearCurrentInst!\n");

		if ( OspIsValidTcpNode(g_cMpu2VmpApp.m_dwMcuNode))
		{
			KeyLog( "[ProcGetMsStatus] OspDisconnectTcpNode A!\n");
			OspDisconnectTcpNode(g_cMpu2VmpApp.m_dwMcuNode );
		}
		if ( OspIsValidTcpNode(g_cMpu2VmpApp.m_dwMcuNodeB))
		{
			KeyLog( "[ProcGetMsStatus] OspDisconnectTcpNode B!\n");
			OspDisconnectTcpNode(g_cMpu2VmpApp.m_dwMcuNodeB );
		}

		if ( INVALID_NODE == g_cMpu2VmpApp.m_dwMcuNode )
		{
			KeyLog( "[ProcGetMsStatus] EV_VMP_CONNECT_TIMER A!\n");
			//SetTimer( EV_VMP_CONNECT_TIMER, MPU_CONNETC_TIMEOUT );
			post(MAKEIID(AID_MPU2,CInstance::DAEMON),EV_VMP_CONNECT_TIMER);

		}
		if ( INVALID_NODE == g_cMpu2VmpApp.m_dwMcuNodeB )
		{
			KeyLog( "[ProcGetMsStatus] EV_VMP_CONNECT_TIMER B!\n");
			//SetTimer( EV_VMP_CONNECT_TIMERB, MPU_CONNETC_TIMEOUT );
			post(MAKEIID(AID_MPU2,CInstance::DAEMON),EV_VMP_CONNECT_TIMERB);

		}
	}
	return;
}

/*=============================================================================
  �� �� ���� ProcChangeMemAliasCmd
  ��    �ܣ� �ı��Ա��ʾ�ı���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2012/06/06  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::ProcChangeMemAliasCmd( CMessage* const pMsg )
{
	//1�������Ϸ���У��
	if ( NULL == pMsg ) 
	{
		ErrorLog("[ProcChangeMemAliasCmd]Null pointer CMessage\n");
		return;
	}
	if ( CurState() != RUNNING ) 
	{
		ErrorLog( "[ProcChangeMemAliasCmd] Wrong State:%d !\n", CurState() );
		return;
	}
	//2���ж��û��Ƿ�������Ҫ��ʾ�ն˱�����û���������յ������ϢҲ������
	CServMsg cServMsg( pMsg->content, pMsg->length );
	u8 byChnNo = cServMsg.GetChnIndex();
	
	// ��ȡ������ͨ����֧�ֱ������ݣ�֧�ֶ�ͨ��ͬʱ����
	s8 *pchAlias;
	u16 wMsgLen = 0;
	u8 byIndex = 0;
	s32 nReturn = Codec_Success;
	do 
	{
		pchAlias = (s8*)(cServMsg.GetMsgBody() + wMsgLen);
		if (strlen(pchAlias) > 0)
		{
			wMsgLen += MAXLEN_ALIAS;//��������
			byChnNo = *(cServMsg.GetMsgBody() + wMsgLen);
			wMsgLen++;
			if (byChnNo >= MAXNUM_MPU2VMP_MEMBER)
			{
				// ͨ����У��
				break;
			}
			// ����ͨ���ն˱���
			m_tVmpMbAlias[byChnNo].SetMbAlias(strlen(pchAlias), pchAlias);
			
			if ( TRUE == IsDisplayMbAlias() )
			{
				DisplayMbAlias(byChnNo,pchAlias);
			}
			
			//���Ƿ����Զ���ͼƬҪ��Ӧ
			for (byIndex = 0; byIndex < MAXNUM_VMPDEFINE_PIC; byIndex++)
			{
				if(!m_tVmpCommonAttrb.m_atMtMember[byChnNo].IsNull() &&
					strlen(pchAlias) > 0 &&
					0 == strcmp(pchAlias, m_tMpuVmpCfg.GetVmpMemAlias(byIndex)))
				{
					//���ýӿ�
					KeyLog( "[ProcChangeMemAliasCmd]index.%d -> alias.%s -> pic.%s!\n", 
						byChnNo, 
						m_tMpuVmpCfg.GetVmpMemAlias(byIndex), 
						m_tMpuVmpCfg.GetVmpMemRoute(byIndex));
					
					nReturn = SetNoStreamBakByBmpPath((u32)EN_PLAY_BMP_USR,m_tMpuVmpCfg.GetVmpMemRoute(byIndex),byChnNo);
					if ( (s32)Codec_Success != nReturn )
					{
						ErrorLog( "[ProcChangeMemAliasCmd] Index.%d SetNoStreamBak usrbmp failed(ErrCode.%d)!\n", byChnNo, nReturn);
					}
					break;
				}
			}
		}
		else
		{
			wMsgLen += MAXLEN_ALIAS;//��������			
			wMsgLen++;
		}
	} while (cServMsg.GetMsgBodyLen() > wMsgLen);

	return;
}

/*==============================================================================
������    :  MsgAddRemoveRcvChnnl
����      :  ����/ɾ��ĳ����ͨ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2012/11/19  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::MsgAddRemoveRcvChnnl(CMessage * const pMsg)
{
	CServMsg cServMsg( pMsg->content, pMsg->length );
	u8 byChnNo = cServMsg.GetChnIndex();
//	u8 byNoUse = *cServMsg.GetMsgBody();
	u8 byAdd = *(cServMsg.GetMsgBody()+1);
	s32 nRet = FALSE;
	
	if (byAdd == 0)
	{
		if(m_bAddVmpChannel[byChnNo] == FALSE)
		{
			return;
		}
		nRet = m_cHardMulPic.RemoveChannel( byChnNo );
		if (IsDisplayMbAlias())
		{
			nRet = m_cHardMulPic.StopAddIcon(byChnNo);
			KeyLog("[MsgAddRemoveRcvChnnl]Stop AddIcon chnl:%d,ret:%d\n",byChnNo,nRet);
		}
		if ( HARD_MULPIC_OK != nRet )
		{
			ErrorLog( "[MsgAddRemoveRcvChnnl] Remove channel.%d error, ret.%d(correct is %u)\n", byChnNo, nRet, HARD_MULPIC_OK);
			return;
		}
		else
		{
			KeyLog("[MsgAddRemoveRcvChnnl]RemoveChannel:%d successfully \n",byChnNo);
			m_bAddVmpChannel[byChnNo] = FALSE;
// 			if (IsSimulateMpu())
// 			{
// 				m_tMpuParam[0].m_atMtMember[byChnNo].SetNull();
// 				m_tMpuParam[0].m_tDoublePayload[byChnNo].Reset();
// 				
// 			}
// 			else
			{
				m_tVmpCommonAttrb.m_atMtMember[byChnNo].SetNull();
				m_tVmpCommonAttrb.m_tDoublePayload[byChnNo].Reset();
				memset(&m_tVmpMbAlias[byChnNo], 0, sizeof(m_tVmpMbAlias[byChnNo]));
				//���ñ߿����ɫ
				if ( !SetVmpAttachStyle( m_tStyleInfo ) )
				{
					ErrorLog( " Failed to set vmp attach style (MsgAddRemoveRcvChnnl)\n" );
					return;
				}
			}
		}
	}
	else
	{
		if( m_bAddVmpChannel[byChnNo] == TRUE )
		{
			return;
		}
		nRet = m_cHardMulPic.AddChannel( byChnNo );
		if ( HARD_MULPIC_OK != nRet )
		{
			ErrorLog(  "[MsgAddRemoveRcvChnnl] AddChannel.%u error, ret.%u(correct is %u)\n", byChnNo, nRet, HARD_MULPIC_OK );
			return;
		}
		else
		{    
			KeyLog("[MsgAddRemoveRcvChnnl]addchnnel:%d successfully \n",byChnNo);
			m_bAddVmpChannel[byChnNo] = TRUE;
			//���ö�Ӧͨ���Ľ������
			StartMediaRcv(byChnNo);
		}
	}
}
/*====================================================================
	����  : ChangeVMPStyle
	����  : �ı仭�渴�ϲ���(�ı��˷��)��Ϣ��Ӧ
	����  : ��
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
BOOL CMpu2VmpInst::ChangeVMPStyle()
{
    s32 nRet = 0;

    //1����ȡ�µķ��
	u8 byStyle = ConvertVcStyle2HardStyle( m_tVmpCommonAttrb.m_byVmpStyle );
// 	if ( IsSimulateMpu() ) //ģ��mpu
// 	{
// 		byStyle = ConvertVcStyle2HardStyle( m_tMpuParam[0].m_byVMPStyle );
// 	}
	
    KeyLog( "[ChangeVMPStyle] OldHardStyle: %d, NewHardStyle: %d\n",m_byHardStyle, byStyle  );

    //2������б�
    if ( m_byHardStyle != byStyle )
    {
        //��ֹͣ�ɷ���µ�ͨ������
        u8 byOldStyle = ConvertHardStyle2VcStyle(m_byHardStyle);
        u8 byOldMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle(byOldStyle);

		TVMPMemberEx tLoopMember, tLoopPreviousMember;
        for (u8 byChnNo = 0; byChnNo < byOldMaxChnNum; byChnNo ++)
        {
			// �¾ɳ�Ա��һ��ʱ����Ҫ��ͣ�ٿ�
			tLoopMember = m_tVmpCommonAttrb.m_atMtMember[byChnNo];
			tLoopPreviousMember = m_tPreviousVmpCommonAttrb.m_atMtMember[byChnNo];
			if (tLoopMember == tLoopPreviousMember)
			{
				continue;
			}
            m_pcMediaRcv[byChnNo]->StopRcv();
        }
        //�з��
        nRet = m_cHardMulPic.SetMulPicType( byStyle );
        if ( HARD_MULPIC_OK != nRet )
        {
            ErrorLog( " Fail to call: SetMulPicType, Error code: %d\n", nRet );
            return FALSE;
        }

        m_byHardStyle = byStyle;
    }

    return TRUE;
}




/*=============================================================================
  �� �� ���� ConvertHardStyle2VcStyle
  ��    �ܣ� �ɵײ���õ���ط��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byHardStyle
  �� �� ֵ�� u8 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
u8 CMpu2VmpInst::ConvertHardStyle2VcStyle(u8 byHardStyle)
{
    u8 byStyle = 0;
    switch ( byHardStyle )
    {
    case MULPIC_TYPE_ONE:  // һ����
        byStyle = VMP_STYLE_ONE;  // ������
        break;
        
    case MULPIC_TYPE_VTWO: // �����棺���ҷ�
        byStyle = VMP_STYLE_VTWO;  // �����棺���ҷ�
        break;
        
    case MULPIC_TYPE_ITWO:  // ������: һ��һС
        byStyle = VMP_STYLE_HTWO; // �����棺 һ��һС������ֳ�9�ݷ��ͣ�
        break;
        
    case MULPIC_TYPE_THREE: // ������
        byStyle = VMP_STYLE_THREE; // ������
        break;
        
    case MULPIC_TYPE_FOUR: // �Ļ���
        byStyle = VMP_STYLE_FOUR;  // �Ļ���
        break;
        
    case MULPIC_TYPE_SIX: //������
        byStyle = VMP_STYLE_SIX; //������
        break;
        
    case MULPIC_TYPE_EIGHT: //�˻���
        byStyle = VMP_STYLE_EIGHT; //�˻���
        break;

    case MULPIC_TYPE_NINE: //�Ż���
        byStyle = VMP_STYLE_NINE; //�Ż���
        break;
        
    case MULPIC_TYPE_TEN: //ʮ����
        byStyle = VMP_STYLE_TEN; //ʮ���棺�����С������ֳ�4�ݷ��ͣ�
        break;
        
    case MULPIC_TYPE_THIRTEEN:  //ʮ������
        byStyle = VMP_STYLE_THIRTEEN; //ʮ������
        break;
        
    case MULPIC_TYPE_SIXTEEN: //ʮ������
        byStyle = VMP_STYLE_SIXTEEN; //ʮ������
        break;
        
    case MULPIC_TYPE_TWENTY: //��ʮ����
        byStyle = VMP_STYLE_TWENTY;
        break;
        
    case MULPIC_TYPE_SFOUR://�����Ļ��� 
        byStyle = VMP_STYLE_SPECFOUR; //�����Ļ��棺һ����С������ֳ�4�ݷ��ͣ�
        break;
        
    case MULPIC_TYPE_SEVEN: //�߻���
        byStyle = VMP_STYLE_SEVEN; //�߻��棺������С������ֳ�4�ݷ��ͣ�
        break;

	case MULPIC_TYPE_THREE_2IBR:	   //�����棬2��С���ڵײ�����
		byStyle = (u8)VMP_STYLE_THREE_2BOTTOMRIGHT; 
		break;

	case MULPIC_TYPE_THREE_2IB :	   //�����棬2��С���ڵײ�����
		byStyle = (u8)VMP_STYLE_THREE_2BOTTOM2SIDE; 
		break;

	case MULPIC_TYPE_THREE_2IBL :	   //�����棬2��С���ڵײ�����
		byStyle = (u8)VMP_STYLE_THREE_2BOTTOMLEFT; 
		break;

	case MULPIC_TYPE_VTHREE :	    //��ֱ�ȴ�3����
		byStyle = (u8)VMP_STYLE_THREE_VERTICAL; 
		break;

	case MULPIC_TYPE_FOUR_3IB :	  //�Ļ��棬3���ڵײ�����
		byStyle = (u8)VMP_STYLE_FOUR_3BOTTOMMIDDLE; 
		break;
		
	case MULPIC_TYPE_TEN_MID_LR :	  //ʮ���棬�м����������¸�4��
		byStyle = (u8)VMP_STYLE_TEN_M; 
		break;

	case MULPIC_TYPE_THIRTEEN_MID :	  //ʮ�����棬�м�һ����ģ������м���
		byStyle = (u8)VMP_STYLE_THIRTEEN_M; 
		break;
	case MULPIC_TYPE_TWENTYFIVE : //��ʮ�廭��
		byStyle = (u8)VMP_STYLE_TWENTYFIVE;
		break;
        
    default:
        byStyle = VMP_STYLE_VTWO;    // Ĭ�������棺���ҷ�
        break;
    }
    return byStyle;
}

/*=============================================================================
  �� �� ���� SetVmpAttachStyle
  ��    �ܣ� ���ø��Ϸ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TVmpStyleCfgInfo
  �� �� ֵ�� void 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
BOOL32 CMpu2VmpInst::SetVmpAttachStyle( TVmpStyleCfgInfo&  tStyleInfo )
{


	//1���õ�stylecfginfo����ͼ�񱳾�ɫ����Ӧ��ϯ�������ˣ����ڵı߿���ɫ
	memcpy( (u8*)&m_tStyleInfo, (void*)&tStyleInfo, sizeof(TVmpStyleCfgInfo) );
    TMulPicColor tBGDColor;
    GetRGBColor( tStyleInfo.GetBackgroundColor(), tBGDColor.RColor,
                                                 tBGDColor.GColor,
                                                 tBGDColor.BColor );
    
    u32 dwChairColor    = tStyleInfo.GetChairFrameColor();
    u32 dwSpeakerColor  = tStyleInfo.GetSpeakerFrameColor();
    u32 dwAudienceColor = tStyleInfo.GetFrameColor();

    u8 byRColorChair = 0;
    u8 byGColorChair = 0;
    u8 byBColorChair = 0;

    u8 byRColorAudience = 0;
    u8 byGColorAudience = 0;
    u8 byBColorAudience = 0;

    u8 byRColorSpeaker = 0;
    u8 byGColorSpeaker = 0;
    u8 byBColorSpeaker = 0;

    GetRGBColor( dwChairColor,    byRColorChair,    byGColorChair,    byBColorChair );
    GetRGBColor( dwSpeakerColor,  byRColorSpeaker,  byGColorSpeaker,  byBColorSpeaker );
    GetRGBColor( dwAudienceColor, byRColorAudience, byGColorAudience, byBColorAudience );

	//2�����ݸ�������ͨ���ĳ�Ա�����ԣ����ø�ͨ����Ӧ�ı߿���ɫ
    TMulPicColor atSlidelineColor[/*MAXNUM_MPUSVMP_MEMBER*/MULPIC_MAX_CHNS];
    memset( atSlidelineColor, 0, sizeof(atSlidelineColor) );
	
	s32 nRet = 0;
    u8 byLoop;
    for ( byLoop = 0; byLoop < MAXNUM_MPU2VMP_MEMBER; byLoop++ )
    {

		TVMPMemberEx tLoopMember ;
		tLoopMember.SetNull();
// 		if ( IsSimulateMpu() ) //ģ��mpu
// 		{
// 			tLoopMember = m_tMpuParam[0].m_atMtMember[byLoop];
// 		}
// 		else
		{
			tLoopMember  = m_tVmpCommonAttrb.m_atMtMember[byLoop];
		}

        if (  tLoopMember.IsNull() )//fanchaoҪ�����ն�ͨ��ҲҪ����Ϊ���ڵ���ɫ
        {
			atSlidelineColor[byLoop].RColor = 246;
            atSlidelineColor[byLoop].GColor = 246;
            atSlidelineColor[byLoop].BColor = 246;
            continue;
        }
		
        if ( MT_STATUS_AUDIENCE == tLoopMember.GetMemStatus() )
        {
            atSlidelineColor[byLoop].RColor = byRColorAudience;
            atSlidelineColor[byLoop].GColor = byGColorAudience;
            atSlidelineColor[byLoop].BColor = byBColorAudience;
			
            KeyLog( "The channel: %d, The Audience Color Value: %0x\n", byLoop, dwAudienceColor);
        }

        else if ( MT_STATUS_SPEAKER == tLoopMember.GetMemStatus() )
        {
            atSlidelineColor[byLoop].RColor = byRColorSpeaker;
            atSlidelineColor[byLoop].GColor = byGColorSpeaker;
            atSlidelineColor[byLoop].BColor = byBColorSpeaker;
            KeyLog( "The channel: %d, The Speaker  Color Value: %0x\n", byLoop, dwSpeakerColor);
        }
        else if ( MT_STATUS_CHAIRMAN == tLoopMember.GetMemStatus() )
        {
            atSlidelineColor[byLoop].RColor = byRColorChair;
            atSlidelineColor[byLoop].GColor = byGColorChair;
            atSlidelineColor[byLoop].BColor = byBColorChair;
            KeyLog( "The channel: %d, The Chairman Color Value: %0x\n", byLoop, dwChairColor);
        }

        if (m_tStyleInfo.GetIsRimEnabled())
        {
            atSlidelineColor[byLoop].byDefault = 1;
        }
		else
		{
			atSlidelineColor[byLoop].byDefault = 0;
		}

		OspPrintf(TRUE,FALSE,"[SetVipChnl]--%d\n",atSlidelineColor[byLoop].byDefault);
		nRet = m_cHardMulPic.SetVipChnl(atSlidelineColor[byLoop], byLoop);
		if ( HARD_MULPIC_OK != nRet )
		{
			ErrorLog( " Fail to call the SetVipChnl. (ProcSetVmpAttachStyle)\n" );
			return FALSE;
		}
    }
    KeyLog( "The Background Color Value: 0x%x\n", tStyleInfo.GetBackgroundColor() );
    KeyLog( "The RGB Color of Chairman : R<%d> G<%d> B<%d> \n", byRColorChair,    byGColorChair,    byBColorChair );
    KeyLog( "The RGB Color of Speaker  : R<%d> G<%d> B<%d> \n", byRColorSpeaker,  byGColorSpeaker,  byBColorSpeaker );
    KeyLog( "The RGB Color of Audience : R<%d> G<%d> B<%d> \n", byRColorAudience, byGColorAudience, byBColorAudience );
    KeyLog( "The RGB Color of Backgroud: R<%d> G<%d> B<%d> \n", tBGDColor.RColor, tBGDColor.GColor, tBGDColor.BColor );
	
    

	//3������ϳɷ�񱳾��ӱ߿�
	//fanchaoҪ��mpu2����SetBGDAndSidelineColor��ʱ����TRUE
// 	if ( m_tStyleInfo.GetIsRimEnabled() )
// 	{
		u32 dwTick = OspTickGet();
 		nRet = m_cHardMulPic.SetBGDAndSidelineColor( tBGDColor, atSlidelineColor, TRUE );
		KeyLog("============= m_cHardMulPic.SetBGDAndSidelineColor Tick:%d\n",OspTickGet()-dwTick);
//  	}
//  	else
//  	{
// 		u32 dwTick = OspTickGet();
//  		nRet = m_cHardMulPic.SetBGDAndSidelineColor( tBGDColor, atSlidelineColor, FALSE );
// 		KeyLog("============= m_cHardMulPic.SetBGDAndSidelineColor Tick:%d\n",OspTickGet() -dwTick);
//  	}
    
    
	KeyLog( "Set VMP Scheme:%d with RimEnabled:%d successfully !\n", m_tStyleInfo.GetSchemeId(), m_tStyleInfo.GetIsRimEnabled() );
    
	return (HARD_MULPIC_OK == nRet);
}

/*=============================================================================
  �� �� ���� GetRGBColor
  ��    �ܣ� ȡ����ɫ��Ӧ��R,G,Bֵ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwColor
             u8& byRColor
             u8& byGColor
             u8& byBColor
  �� �� ֵ�� void 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::GetRGBColor( u32 dwColor, u8& byRColor, u8& byGColor, u8& byBColor )
{
    byRColor = (u8)dwColor & 0x000000ff;
    byGColor = (u8)( (dwColor & 0x0000ff00) >> 8 );
    byBColor = (u8)( (dwColor & 0x00ff0000) >> 16 );
    return;
}

/*=============================================================================
    �� �� ���� VmpGetActivePayload
    ��    �ܣ� ͨ��RealPayloadȡ��Ӧ��ActivePayload
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� u8
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
u8 CMpu2VmpInst::VmpGetActivePayload( u8 byRealPayloadType )
{
    u8 byAPayload = MEDIA_TYPE_NULL;

    switch ( byRealPayloadType )
    {
    case MEDIA_TYPE_PCMA:   byAPayload = ACTIVE_TYPE_PCMA;  break;
    case MEDIA_TYPE_PCMU:   byAPayload = ACTIVE_TYPE_PCMU;  break;
    case MEDIA_TYPE_G721:   byAPayload = ACTIVE_TYPE_G721;  break;
    case MEDIA_TYPE_G722:   byAPayload = ACTIVE_TYPE_G722;  break;
    case MEDIA_TYPE_G7231:  byAPayload = ACTIVE_TYPE_G7231; break;
    case MEDIA_TYPE_G728:   byAPayload = ACTIVE_TYPE_G728;  break;
    case MEDIA_TYPE_G729:   byAPayload = ACTIVE_TYPE_G729;  break;
    case MEDIA_TYPE_G7221:  byAPayload = ACTIVE_TYPE_G7221; break;
    case MEDIA_TYPE_G7221C: byAPayload = ACTIVE_TYPE_G7221C;break;
    case MEDIA_TYPE_ADPCM:  byAPayload = ACTIVE_TYPE_ADPCM; break;
    case MEDIA_TYPE_H261:   byAPayload = ACTIVE_TYPE_H261;  break;
    case MEDIA_TYPE_H262:   byAPayload = ACTIVE_TYPE_H262;  break;
    case MEDIA_TYPE_H263:   byAPayload = ACTIVE_TYPE_H263;  break;
    //case MEDIA_TYPE_ADPCM:  byAPayload = bNewActivePT ? ACTIVE_TYPE_ADPCM : byRealPayloadType;  break;
    //case MEDIA_TYPE_G7221C: byAPayload = bNewActivePT ? ACTIVE_TYPE_G7221C : byRealPayloadType; break;
    default:                byAPayload = byRealPayloadType; break;
    }        

    return byAPayload;
}

/*====================================================================
	������  :StartNetRcvSnd
	����    :��ʼ�����շ�
	�㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byNeedPrs:�Ƿ���Ҫ�����ش�
    �� �� ֵ�� BOOL
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
BOOL CMpu2VmpInst::StartNetRcvSnd(u8 byNeedPrs)
{

	//1. ���뷢�����
    u8 byChnNo = 0;
	//u8 byActPt = 0;

	// media encrypt modify
	TMediaEncrypt tMediaEncrypt;
	s32 nKeyLen = 0;
	u8  abyKeyBuf[MAXLEN_KEY] = {0};
	for (byChnNo = 0; byChnNo < MAXNUM_MPU2VMP_MEMBER; byChnNo ++)
	{
// 		if ( IsSimulateMpu() ) //ģ��mpu
// 		{
// 			if ( CONF_ENCRYPTMODE_NONE == m_tMpuParam[0].m_tVideoEncrypt[byChnNo].GetEncryptMode() )
// 			{
// 				continue;
// 			}
// 			m_tMpuParam[0].m_tVideoEncrypt[byChnNo].GetEncryptKey( abyKeyBuf, &nKeyLen );
// 			tMediaEncrypt.SetEncryptMode(m_tMpuParam[0].m_tVideoEncrypt[byChnNo].GetEncryptMode());
// 			tMediaEncrypt.SetEncryptKey( abyKeyBuf, nKeyLen );
// 			break;
// 		}
// 		else
		{
			if ( CONF_ENCRYPTMODE_NONE == m_tVmpCommonAttrb.m_tVideoEncrypt[byChnNo].GetEncryptMode() )
			{
				continue;
			}
			m_tVmpCommonAttrb.m_tVideoEncrypt[byChnNo].GetEncryptKey( abyKeyBuf, &nKeyLen );
			tMediaEncrypt.SetEncryptMode(m_tVmpCommonAttrb.m_tVideoEncrypt[byChnNo].GetEncryptMode());
			tMediaEncrypt.SetEncryptKey( abyKeyBuf, nKeyLen );
			break;
		}
		
	}

    for ( byChnNo = 0; byChnNo < m_byValidVmpOutNum; byChnNo++ )
    {

        InitMediaSnd( byChnNo );
		u8 byEncType = m_tParam[byChnNo].GetEncType();
// 		if ( IsSimulateMpu() ) 
// 		{
// 			byEncType = m_tMpuParam[byChnNo].m_byEncType;
// 		}
// 		
        SetEncryptParam( byChnNo, &tMediaEncrypt, byEncType );
		if (NULL != m_pcMediaSnd[byChnNo])
		{
			u32 dwTick = OspTickGet();
			m_cHardMulPic.SetVidDataCallback( byChnNo, VMPCALLBACK, m_pcMediaSnd[byChnNo] );
			KeyLog("==============[StartNetRcvSnd]after m_cHardMulPic.SetVidDataCallback(%d) Tick:%d\n",byChnNo,OspTickGet() - dwTick);
		}
		
       
    }

    //�����ش�����
    SetPrs(byNeedPrs);


    //2. ����������
	for (byChnNo = 0; byChnNo < MAXNUM_MPU2VMP_MEMBER; byChnNo ++)
	{
        //MediaRcv ���������������
        SetDecryptParam( byChnNo, &tMediaEncrypt/*, &m_tParam[0].m_tDoublePayload[byChnNo]*/ );
		SetMediaRcvNetParam( byChnNo);
	}

	u8 byMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle(m_tVmpCommonAttrb.m_byVmpStyle);
// 	if ( IsSimulateMpu() )
// 	{
// 		 byMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle(m_tMpuParam[0].m_byVMPStyle);
// 	}
   
    for ( byChnNo = 0; byChnNo < byMaxChnNum; byChnNo++ )
    {
		TVMPMemberEx tLoopMember;
		tLoopMember.SetNull();
// 		if ( IsSimulateMpu() ) 
// 		{
// 			tLoopMember = m_tMpuParam[0].m_atMtMember[byChnNo];
// 		}
// 		else
		{
			tLoopMember = m_tVmpCommonAttrb.m_atMtMember[byChnNo];
		}
        if ( !tLoopMember.IsNull() )
        { 
            //���ö�Ӧͨ���Ľ������
            StartMediaRcv(byChnNo);
        }
        else
        {
			m_pcMediaRcv[byChnNo]->StopRcv();
			KeyLog( "%d StopRcv!\n", byChnNo);
        }
    }

    return TRUE;
}

/*====================================================================
	������  :ChangeChnAndNetRcv
	����    :�б�net�շ� �� ͨ��
	����    :��Ϣ
	���    :��
	����ֵ  :��
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
BOOL CMpu2VmpInst::ChangeChnAndNetRcv(/*u8 byNeedPrs,*/ BOOL32 bStyleChged)
{
    s32 nRet = 0;
    
    //1������ͨ�������¿�ʼ����
    u8 byMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle(m_tVmpCommonAttrb.m_byVmpStyle);
// 	if ( IsSimulateMpu() )//ģ��mpu
// 	{
// 		byMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle(m_tMpuParam[0].m_byVMPStyle);
// 	}

    for(u8 byChnNo = 0; byChnNo < MAXNUM_MPU2VMP_MEMBER; byChnNo++ )
    {
		if ( byChnNo >= byMaxChnNum )
		{
			m_bAddVmpChannel[byChnNo] = FALSE;
			continue;
		}
		TVMPMemberEx tLoopMember, tLoopPreviousMember;
		tLoopMember.SetNull();
		tLoopPreviousMember.SetNull();
		TDoublePayload tLoopPreDoublePayload;
// 		if ( IsSimulateMpu() )
// 		{
// 			tLoopMember = m_tMpuParam[0].m_atMtMember[byChnNo];
// 			tLoopPreviousMember = m_tMpuParamPrevious[0].m_atMtMember[byChnNo];
// 			tLoopPreDoublePayload = m_tMpuParamPrevious[0].m_tDoublePayload[byChnNo];
// 		}
// 		else
		{
			tLoopMember = m_tVmpCommonAttrb.m_atMtMember[byChnNo];
			tLoopPreviousMember = m_tPreviousVmpCommonAttrb.m_atMtMember[byChnNo];
			tLoopPreDoublePayload = m_tPreviousVmpCommonAttrb.m_tDoublePayload[byChnNo];
		}
		//ͨ����ԱΪnull��Remove
        if ( tLoopMember.IsNull() )
        {
			if ( TRUE == m_bAddVmpChannel[byChnNo] )
			{
				nRet = m_cHardMulPic.RemoveChannel( byChnNo ); 
				KeyLog( "[ChangeChnAndNetRcv] Remove channel.%d, ret.%d(correct is %u)\n", byChnNo, nRet, HARD_MULPIC_OK);
				if (IsDisplayMbAlias())
				{
					nRet = m_cHardMulPic.StopAddIcon(byChnNo);
					KeyLog("[ChangeChnAndNetRcv]Stop AddIcon chnl:%d,ret:%d\n",byChnNo,nRet);
				}
			}
            
            m_bAddVmpChannel[byChnNo] = FALSE;
        }
        else
        {
            // zbq[05/14/2009] �Ƿ���б䣬�����Ѿ����ڵ�ͨ������ִ�б�����
            if ( !tLoopPreviousMember.IsNull()
                 && tLoopPreDoublePayload.GetRealPayLoad() != MEDIA_TYPE_NULL	// xliang [5/18/2009] ��Ӧģ�����Զ�����ϳɵ����
                )
            {
                // ͳһ�����СΪ22�� [8/5/2013 liaokang]
                // ˢ��������ֹ����л���̨��������С������ [7/8/2013 liaokang]
                //m_cHardMulPic.StopAddIcon(byChnNo);
                //DisplayMbAlias(byChnNo,m_tVmpMbAlias[byChnNo].GetMbAlias());

				// [pengjie 2010/2/9] ���ö�Ӧͨ���Ľ��������mpu2�з��ʱǰ��ͨ����Ա���ʱ����ͣ�ٿ�
				if( !(tLoopMember == tLoopPreviousMember)/* || bStyleChged */)
				{	
                    DisplayMbAlias(byChnNo,m_tVmpMbAlias[byChnNo].GetMbAlias());
					//DisplayMbAlias(byChnNo,m_tVmpMbAlias[byChnNo].GetMbAlias());
					KeyLog( "[ChangeChnAndNetRcv] chn.%d, StartMediaRcv because of need StyleChged or ChnnlMember change!\n", byChnNo);
					StartMediaRcv(byChnNo);
					
				}
				// End 

                KeyLog( "[ChangeChnAndNetRcv] chn.%d eq to previous one, check next directly!\n", byChnNo);
                continue;
            }
            
            // ���ͨ��
            nRet = m_cHardMulPic.AddChannel( byChnNo );
            if ( HARD_MULPIC_OK != nRet )
            {
				ErrorLog("[ChangeChnAndNetRcv]AddChannel:%d failed.\n",byChnNo);
				m_bAddVmpChannel[byChnNo] = FALSE;
				continue;
            }
            else
            {        
                m_bAddVmpChannel[byChnNo] = TRUE;
				if ( IsDisplayMbAlias() )
				{
					DisplayMbAlias(byChnNo,m_tVmpMbAlias[byChnNo].GetMbAlias());
				}
				
            }
            KeyLog( "[ChangeChnAndNetRcv] AddChannel.%u, ret.%u(correct is %u)\n", byChnNo, nRet, HARD_MULPIC_OK );
            
            //���ö�Ӧͨ���Ľ������
            StartMediaRcv(byChnNo);
        }
    }

    return TRUE;
}

/*====================================================================
	������  :StopNetRcvSnd
	����    :ֹͣNet�շ�
	����    :��Ϣ
	���    :��
	����ֵ  :��
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
BOOL CMpu2VmpInst::StopNetRcvSnd()
{
    u8 byChnNo = 0;
    //1��ֹͣ20·��
    for ( ; byChnNo < MAXNUM_MPU2VMP_MEMBER; byChnNo++ )
    {
        //��ն�̬�غɺͼ���
        TMediaEncrypt tVideoEncrypt;
        memset( &tVideoEncrypt, 0, sizeof(TMediaEncrypt) );
        TDoublePayload tDoublePayload;
        memset( &tDoublePayload, 0, sizeof(TDoublePayload) );
        SetDecryptParam( byChnNo, &tVideoEncrypt/*, &tDoublePayload*/ );
        
        //ֹͣ����
        m_pcMediaRcv[byChnNo]->StopRcv();
    }
    
    //2�����±�����Կ����գ�
    for ( byChnNo = 0; byChnNo < m_byMaxVmpOutNum; byChnNo++ )
    {
        TMediaEncrypt tVideoEncrypt;
        memset( &tVideoEncrypt, 0, sizeof(TMediaEncrypt) );
        SetEncryptParam( byChnNo, &tVideoEncrypt, 0 );
		m_bUpdateMediaEncrpyt[byChnNo] = FALSE;

    }

    //���PRS
    SetPrs(0);////////////Prs��ա���������

    return TRUE;
}

/*=============================================================================
  �� �� ���� SetEncryptParam
  ��    �ܣ� ���ü��ܲ�������ʼ�ϳ�ʱ���ã�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byChnNo        ��ͨ����
             TMediaEncrypt     �����ܲ���
             u8 byDoublePayload�������غ�
  �� �� ֵ�� void 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::SetEncryptParam( u8 byChnNo, TMediaEncrypt * ptVideoEncrypt, u8 byRealPT )
{
	//1�������Ϸ���У��
    if( NULL == ptVideoEncrypt )
	{
		ErrorLog("[SetEncryptParam]ERROR:chnl:%d null pointer\n");
		return;
	}
	//2�����÷��Ͷ���ļ��ܲ���
    u8 byDoublePayload = VmpGetActivePayload(byRealPT);

    u8  abyKeyBuf[MAXLEN_KEY];
	memset( abyKeyBuf, 0, MAXLEN_KEY );
    s32 nKeyLen = 0;

    u8 byEncryptMode = ptVideoEncrypt->GetEncryptMode();
    if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
    {
        m_pcMediaSnd[byChnNo]->SetEncryptKey( NULL, 0, 0 );

        if (MEDIA_TYPE_H264 == byRealPT)
        {
            m_pcMediaSnd[byChnNo]->SetActivePT(byDoublePayload);
        }
        else
        {
            m_pcMediaSnd[byChnNo]->SetActivePT(0);
        }

		KeyLog( "[SetEncryptParam]OutChannel.%u: no encrypt, so set key to NULL, set ActivePt.%u\n",
			byChnNo, byDoublePayload);
 
    }
    else if ( CONF_ENCRYPTMODE_DES == byEncryptMode || CONF_ENCRYPTMODE_AES == byEncryptMode ) // ���������
    {
        if ( CONF_ENCRYPTMODE_DES == byEncryptMode ) // ���²�궨��ת��
        {
            byEncryptMode = DES_ENCRYPT_MODE;
        }
        else
        {
            byEncryptMode = AES_ENCRYPT_MODE;
        }
        ptVideoEncrypt->GetEncryptKey( abyKeyBuf, &nKeyLen );

        m_pcMediaSnd[byChnNo]->SetEncryptKey( (s8*)abyKeyBuf, (u16)nKeyLen, byEncryptMode );
        m_pcMediaSnd[byChnNo]->SetActivePT( byDoublePayload );

        KeyLog( "Mode: %u KeyLen: %u PT: %u \n",
                ptVideoEncrypt->GetEncryptMode(), 
                nKeyLen, 
                byDoublePayload );
    }
    return;
}

/*=============================================================================
  �� �� ���� InitMediaSnd
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byChnNo        ��ͨ����
  �� �� ֵ�� void 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::InitMediaSnd( u8 byChnNo )
{
	//
    u8 byFrameRate = m_tMpuVmpCfg.m_tVideoEncParam[byChnNo].m_byFrameRate;
    u8 byMediaType = m_tMpuVmpCfg.m_tVideoEncParam[byChnNo].m_byEncType;
    u32 dwNetBand  = m_tMpuVmpCfg.m_adwMaxSendBand[byChnNo];

//    // ��ӦMCU��Ⱥ���Ż�����������������
//    if ( byFrameRate > 60 )
//    {
//        byFrameRate = 30;
//    }

	//1��������緢�ʹ������
    if ( dwNetBand * 1024 > 8000000 )
    {
        if ( m_tParam[0].GetBitRate() <= 8128 )
        {
			dwNetBand = m_tMpuVmpCfg.m_tVideoEncParam[byChnNo].m_wBitRate * 1024;
        }
        else
        {
            dwNetBand = 0;
        }
    }
    else
    {
        dwNetBand = dwNetBand * 1024;
    }
    KeyLog( "[InitMediaSnd]m_cMediaSnd[%d]: dwNetBand = %d\n",byChnNo, dwNetBand);


    //2��Create ���Ͷ���
    s32 wRet = m_pcMediaSnd[byChnNo]->Create( MAX_VIDEO_FRAME_SIZE,
		    	                      dwNetBand,
			                          byFrameRate,
			    				      byMediaType );

    if ( MEDIANET_NO_ERROR != wRet )
    {
        ErrorLog( "[InitMediaSnd] m_cMediaSnd[%d].Create fail, Error code is:%d, NetBand.%d, FrmRate.%d, MediaType.%d\n", byChnNo, wRet, dwNetBand, byFrameRate, byMediaType );
    	return;
    }
    else
    {
        KeyLog( "[InitMediaSnd] m_cMediaSnd[%d].Create as NetBand.%d, FrmRate.%d, MediaType.%d\n", byChnNo, dwNetBand, byFrameRate, byMediaType );
    }
	
	//3������HDFlag
	BOOL32 bHDFlag = TRUE;

	//h264 CIF, other ����HD Flag; ����h264��4cif��720��1080����HD Flag
	if (GetEncCifChnNum() == byChnNo ||  GetEncOtherChnNum() == byChnNo) 
	{
		bHDFlag = FALSE;
	}
	m_pcMediaSnd[byChnNo]->SetHDFlag( bHDFlag );
	KeyLog( "[InitMediaSnd] m_cMediaSnd[%d].SetHDFlag: %d\n", byChnNo, bHDFlag );

	//4�����÷���ͨ����netparam
    SetMediaSndNetParam(byChnNo);

    return;
}

/*=============================================================================
  �� �� ���� SetMediaRcvNetParam
  ��    �ܣ� ����Medianet Receive�����������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byChnNo        ��ͨ����
  �� �� ֵ�� BOOL 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
BOOL CMpu2VmpInst::SetMediaRcvNetParam( u8 byChnNo)
{
	TLocalNetParam tlocalNetParm;
    memset(&tlocalNetParm, 0, sizeof(TLocalNetParam));
    tlocalNetParm.m_tLocalNet.m_wRTPPort  = m_tCfg.wRcvStartPort + byChnNo * PORTSPAN;
    tlocalNetParm.m_tLocalNet.m_wRTCPPort = m_tCfg.wRcvStartPort + 1 + byChnNo * PORTSPAN;
    tlocalNetParm.m_dwRtcpBackAddr        = htonl(m_dwMcuRcvIp);
    //tlocalNetParm.m_wRtcpBackPort         = m_tCfg.wRcvStartPort + 1 + byChnNo * PORTSPAN;
	// xliang [5/6/2009] ����ԭ�� ChnNo		Port(��MPU�󶨵�MP����rtcp�˿�)
	//							  0~7	===> 2~9 (��39002,...,39009)
	//							  8~15	===> 12~19 (��39012,...,39019)
	//16~23	===> 22~29	
	//24~25 ====> 32~33	
	u16 wRtcpBackPort;
	if (byChnNo >= 24)
	{
		wRtcpBackPort = m_wMcuRcvStartPort+8+byChnNo;
	}
	else if (byChnNo >= 16)
	{
		wRtcpBackPort = m_wMcuRcvStartPort + 6 + byChnNo;
	}
	else if (byChnNo >= 8)
	{
		wRtcpBackPort = m_wMcuRcvStartPort + 4 + byChnNo;
	}
	else
	{
		wRtcpBackPort = m_wMcuRcvStartPort + 2 + byChnNo;
    }
	
	tlocalNetParm.m_wRtcpBackPort = wRtcpBackPort;//Զ�˵�rtcp port
	
    KeyLog( "m_cMediaRcv[%d]:\n", byChnNo );
    KeyLog( "\t tlocalNetParm.m_tLocalNet.m_wRTPPort[%d]:\n", tlocalNetParm.m_tLocalNet.m_wRTPPort );
    KeyLog( "\t tlocalNetParm.m_tLocalNet.m_wRTCPPort[%d]:\n", tlocalNetParm.m_tLocalNet.m_wRTCPPort );
    KeyLog( "\t tlocalNetParm.m_dwRtcpBackAddr[%s]:\n", StrOfIP(tlocalNetParm.m_dwRtcpBackAddr) );
    KeyLog( "\t tlocalNetParm.m_wRtcpBackPort[%d]:\n", tlocalNetParm.m_wRtcpBackPort );
	
	s32 nRet = 0;
    nRet = m_pcMediaRcv[byChnNo]->SetNetRcvLocalParam( tlocalNetParm );
    if( MEDIANET_NO_ERROR !=  nRet )
    {
        ErrorLog( " m_cMediaRcv[%d].SetNetRcvLocalParam fail, Error code is: %d \n", byChnNo, nRet );        
        return FALSE;
	}
    else
    {
        KeyLog( "m_cMediaRcv[%d].SetNetRcvLocalParam succeed!\n", byChnNo );
    }
	return TRUE;
}
/*=============================================================================
  �� �� ���� InitMediaRcv
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byChnNo        ��ͨ����
  �� �� ֵ�� BOOL 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
BOOL32 CMpu2VmpInst::InitMediaRcv( u8 byChnNo )
{
	//1��У����������ý��ջص�
	u16 byInstIdx = GetInsID();
	if ( byInstIdx > MAXNUM_MPU2_EQPNUM || byInstIdx < 1 || byChnNo >= MAXNUM_MPU2VMP_MEMBER) 
	{
		ErrorLog("[InitMediaRcv]Invalid param,InstID:%d,byChnNo:%d\n",byInstIdx,byChnNo);
		return FALSE;
	}
    g_cMpu2VmpApp.m_atMpu2VmpRcvCB[byInstIdx -1][byChnNo].m_byChnnlId = byChnNo;
    g_cMpu2VmpApp.m_atMpu2VmpRcvCB[byInstIdx -1][byChnNo].m_pHardMulPic  = &m_cHardMulPic;
	//2��create
    s32 wRet = m_pcMediaRcv[byChnNo]->Create( MAX_VIDEO_FRAME_SIZE,
			                                  CBMpuSVmpRecvFrame,   
				    		                  (u32)&g_cMpu2VmpApp.m_atMpu2VmpRcvCB[byInstIdx -1][byChnNo] );

    if( MEDIANET_NO_ERROR !=  wRet )
	{
		ErrorLog( " m_cMediaRcv[%d].Create fail, Error code is: %d \n", byChnNo, wRet );
		return FALSE;
	}
    else
    {
        KeyLog( "m_cMediaRcv[%d].Create succeed!\n", byChnNo );
    }

	//ý�أ�medianet���ն���Ҫ����SetHdFlag�ӿڻ������ò���Ϊfalse��
	//�������ΪTRUE��net��֡ʱ���һ��2K��ͷ�����ͷ�ǲ���Ҫ�ģ�ֻ�����Ӹ�����
    m_pcMediaRcv[byChnNo]->SetHDFlag( FALSE );

    return TRUE;
}
/*=============================================================================
  �� �� ���� StartMediaRcv
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byChnNo        ��ͨ����
  �� �� ֵ�� void 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::StartMediaRcv(u8 byChnNo)
{
	if (byChnNo >= MAXNUM_MPU2VMP_MEMBER)
	{
		ErrorLog( "[StartMediaRcv] wrong input param byChnNo = %d\n", byChnNo);
//		printf("[StartMediaRcv] wrong input param byChnNo = %d\n", byChnNo);
        return;
	}

	if (NULL == m_pcMediaRcv[byChnNo])
	{
		ErrorLog( "[StartMediaRcv] m_pcMediaRcv[%d] still null\n", byChnNo);
//		printf("[StartMediaRcv] m_pcMediaRcv[%d] still null\n", byChnNo);
		return;
	}

    m_pcMediaRcv[byChnNo]->StartRcv();

    return;
}

/*=============================================================================
  �� �� ���� SetDecryptParam
  ��    �ܣ� ���ý������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byChnNo
             TMediaEncrypt *  ptVideoEncrypt
             TDoublePayload * ptDoublePayload
             BOOL32 bSetDPOnly: �Ƿ�ֻ���ö�̬�غ�
  �� �� ֵ�� void 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::SetDecryptParam(u8 byChnNo,
                                   TMediaEncrypt *  ptVideoEncrypt/*,TDoublePayload * ptDoublePayload*/)
{
    u8 byEncryptMode;
    u8 abyKeyBuf[MAXLEN_KEY];
    s32 byKenLen = 0;
    memset(abyKeyBuf, 0, MAXLEN_KEY);

    byEncryptMode = ptVideoEncrypt->GetEncryptMode();
    if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
    {
         m_pcMediaRcv[byChnNo]->SetDecryptKey( NULL, 0, 0 );

        //u8 byRealPayload = ptDoublePayload->GetRealPayLoad();
        //u8 byActivePayload = ptDoublePayload->GetActivePayload();
        //m_pcMediaRcv[byChnNo]->SetActivePT( byActivePayload, byRealPayload );
     
         //2009-7-22 Ϊ��ֹ������̬�غ��л�����meidanet���ң�ͳһ�����غ�Ϊ MEDIA_TYPE_H264
        u16 wRet = m_pcMediaRcv[byChnNo]->SetActivePT( MEDIA_TYPE_H264, MEDIA_TYPE_H264 );
		KeyLog( "Set key to NULL, PT(real): %u, SetActivePt result is:%d \n", MEDIA_TYPE_H264, wRet );
    }
    else if ( CONF_ENCRYPTMODE_DES == byEncryptMode || CONF_ENCRYPTMODE_AES == byEncryptMode )
    {
        if ( CONF_ENCRYPTMODE_DES == byEncryptMode )
        {
            byEncryptMode = DES_ENCRYPT_MODE;
        }
        else
        {
            byEncryptMode = AES_ENCRYPT_MODE;
        }

        //u8 byRealPayload = ptDoublePayload->GetRealPayLoad();
        //u8 byActivePayload = ptDoublePayload->GetActivePayload();
        //m_pcMediaRcv[byChnNo]->SetActivePT( byActivePayload, byRealPayload );

        //2009-7-22 Ϊ��ֹ������̬�غ��л�����meidanet���ң�ͳһ�����غ�Ϊ MEDIA_TYPE_H264
        m_pcMediaRcv[byChnNo]->SetActivePT( MEDIA_TYPE_H264, MEDIA_TYPE_H264 );

        ptVideoEncrypt->GetEncryptKey(abyKeyBuf, &byKenLen);
        m_pcMediaRcv[byChnNo]->SetDecryptKey( (s8*)abyKeyBuf, (u16)byKenLen, byEncryptMode );

        KeyLog( "Mode: %u KeyLen: %u PT: %u\n",
                ptVideoEncrypt->GetEncryptMode(), byKenLen, MEDIA_TYPE_H264 );
    }
    return;
}

/*=============================================================================
 �� �� ���� SetPrs
 ��    �ܣ� ����Prs
 �㷨ʵ�֣� setPrs�ڿ���VMPʱֻ��һ�Σ��������е�ͨ��
 ȫ�ֱ����� 
 ��    ���� 
 �� �� ֵ�� 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
====================================================================*/
void CMpu2VmpInst::SetPrs( u8 byNeedPrs )
{
	TRSParam tNetRSParam;
    if ( byNeedPrs )		//�Ƿ���Ҫ���ش��� 1: �ش�, 0: ���ش�
	{		
		KeyLog( "[SetPrs] needed Prs!\n" );
		//regack��������timespan��������,ת������
        tNetRSParam.m_wFirstTimeSpan  = ntohs(m_tPrsTimeSpan.m_wFirstTimeSpan);
		tNetRSParam.m_wSecondTimeSpan = ntohs(m_tPrsTimeSpan.m_wSecondTimeSpan);
		tNetRSParam.m_wThirdTimeSpan  = ntohs(m_tPrsTimeSpan.m_wThirdTimeSpan);
		tNetRSParam.m_wRejectTimeSpan = ntohs(m_tPrsTimeSpan.m_wRejectTimeSpan);
		
        s32	wRet;
        u8 byIndex = 0;
        for ( byIndex = 0; byIndex < m_byMaxVmpOutNum; byIndex++ )
        {
            wRet = m_pcMediaSnd[byIndex]->ResetRSFlag( 2000, TRUE );
            if( MEDIANET_NO_ERROR != wRet )
            {
	            ErrorLog( "m_cMediaSnd[%d].ResetRSFlag fail,Error code is:%d\n", byIndex, wRet );
	            continue;
            }
        }
        for ( byIndex = 0; byIndex < MAXNUM_MPU2VMP_MEMBER; byIndex++ )
        {
         
			//TODO:mpu2�Ƿ��mpuһ����������������
            GetRSParamByPos(byIndex, tNetRSParam);

            wRet = m_pcMediaRcv[byIndex]->ResetRSFlag( tNetRSParam, TRUE );
	        if( MEDIANET_NO_ERROR != wRet )
	        {
		        ErrorLog( "m_cMediaRcv[%d].ResetRSFlag fail,Error code is:%d\n", byIndex, wRet );
		        return;
	        }
            else
            {
                KeyLog( "m_cMediaRcv[%d].ResetRSParam<F:%d, S.%d, T.%d, R.%d>\n",
									byIndex,
                                    tNetRSParam.m_wFirstTimeSpan,
                                    tNetRSParam.m_wSecondTimeSpan,
                                    tNetRSParam.m_wThirdTimeSpan,
                                    tNetRSParam.m_wRejectTimeSpan);
            }
        }
	}
	else
	{
		KeyLog( "[SetPrs] not need Prs!\n" );
		return;
	}
}



/*lint -save -e715*/
/*=============================================================================
�� �� ���� SetSmoothSendRule
��    �ܣ� ����ƽ�����͹���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� BOOL32 bSetFlag TRUE:SetSSRule(����);	FALSE:UnsetSSRule(���)
�� �� ֵ�� void
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2010/12/31   			��ʤ��                  ����
=============================================================================*/
void CMpu2VmpInst::SetSmoothSendRule(BOOL32 bSetFlag)
{
#ifdef _LINUX_    
	s32 nRet = 0;
	
	if (bSetFlag)
	{
		//����ƽ������
		for (u8 byChannlNO = 0; byChannlNO < m_byValidVmpOutNum; byChannlNO++)
		{
			//��ԭ����ǰ���ʣ�ƽ�����Ͳ�������ȡ��������
			u32 dwRate 	= m_tParam[byChannlNO].GetBitRate();
			u32 dwPrevRate = m_tParamPrevious[byChannlNO].GetBitRate(); 
//  			if ( IsSimulateMpu() )
//  			{
//  				dwRate = m_tMpuParam[byChannlNO].GetBitRate();
//  				dwPrevRate = m_tMpuParamPrevious[byChannlNO].GetBitRate();
//  			}
 			if (dwRate == dwPrevRate)
			{
 				KeyLog("[SetSmoothSendRule] previous and now param bitrate the same ,no need setssrule !\n");
 				continue;
 			}

			dwRate = m_tMpuVmpCfg.GetOrigRate(dwRate);    
			dwRate = dwRate >> 3;
			//��ֵΪ120%
			u32 dwPeak = dwRate + dwRate / 5; 
			KeyLog( "[SetSmoothSendRule] Setting ssrule for 0x%x:%d, rate/peak=%u/%u KByte.\n",
				m_dwMcuRcvIp, m_wMcuRcvStartPort + byChannlNO * PORTSPAN, dwRate, dwPeak );
			//nRet = SetSSRule(m_dwMcuRcvIp, m_wMcuRcvStartPort + byChannlNO * PORTSPAN, dwRate, dwPeak, 8);	
			nRet = BatchAddSSRule(m_dwMcuRcvIp, m_wMcuRcvStartPort + byChannlNO * PORTSPAN, dwRate, dwPeak, 8);
			if ( 0 == nRet )
			{
				ErrorLog( "[SetSmoothSendRule]BatchAddSSRule failed. SSErrno=%d. \n", SSErrno);
			}
		}	
		nRet = BatchExecSSRule();
		if (0 == nRet)
		{
			ErrorLog( "[SetSmoothSendRule]BatchExecSSRule failed. SSErrno=%d. \n", SSErrno);
		}
	}
	else
	{
		//���ƽ������
		for (u8 byChannlNO = 0; byChannlNO < m_byValidVmpOutNum; byChannlNO++)
		{
			KeyLog( "[SetSmoothSendRule] Unsetting ssrule for 0x%x:%d.\n",
				m_dwMcuRcvIp, m_wMcuRcvStartPort + byChannlNO * PORTSPAN );
			//nRet = UnsetSSRule( m_dwMcuRcvIp, m_wMcuRcvStartPort + byChannlNO * PORTSPAN );	
			nRet = BatchAddUnsetSSRule( m_dwMcuRcvIp, m_wMcuRcvStartPort + byChannlNO * PORTSPAN );	
			if ( 0 == nRet )
			{
				ErrorLog( "[SetSmoothSendRule] BatchAddUnsetSSRule  failed. SSErrno=%d. \n", SSErrno);
            }
		}
		nRet = BatchExecUnsetSSRule();
		if (0 == nRet)
		{
			ErrorLog( "[SetSmoothSendRule]BatchExecUnsetSSRule failed. SSErrno=%d. \n", SSErrno);
		}
	}
	

#endif
	return;
}
/*lint -restore*/



/*=============================================================================
�� �� ���� ProcSetFrameRateCmd
��    �ܣ� ���ñ���֡��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����CMessage
�� �� ֵ�� void
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2011/12/07  			������                  ����
=============================================================================*/
void CMpu2VmpInst::ProcSetFrameRateCmd( CMessage * const pcMsg )
{
	u8 byFrameRate = *(pcMsg->content);
	for( u8 byLoop = 0; byLoop < m_byMaxVmpOutNum; byLoop ++)
	{
		m_tMpuVmpCfg.m_tVideoEncParam[byLoop].m_byFrameRate =  byFrameRate;
	}
	
	s32 nRet = m_cHardMulPic.StartMerge(&m_tMpuVmpCfg.m_tVideoEncParam[0], 
                                                m_byMaxVmpOutNum);

	if ( HARD_MULPIC_OK != nRet )
	{
		ErrorLog("Set Vmp FrameRate to %d failed!\n", byFrameRate);
	}
	else
	{
		KeyLog( "Set Vmp FrameRate to %d successfully!\n", byFrameRate);
	}
}

/*====================================================================
    ������      : SetMcuCodingForm
    ����        : �洢mcu���뷽ʽ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    ����ֵ˵��  :
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2013/04/08              liaokang      ����
====================================================================*/
void CMpu2VmpInst::SetMcuEncoding(emenCodingForm emMcuEncoding)
{
    m_emMcuEncoding = emMcuEncoding;
}

/*====================================================================
    ������      : GetMcuCodingForm
    ����        : ��ȡMCU���뷽ʽ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    ����ֵ˵��  : 
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2013/04/08              liaokang      ����
====================================================================*/
emenCodingForm CMpu2VmpInst::GetMcuEncoding()
{
    return m_emMcuEncoding;
}

/*==============================================================================
������    :  ShowVMPDebugInfo
����      :  ��ӡ�߼�������Ϣ
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2011/12/07  			������                  ����
=============================================================================*/
void CMpu2VmpInst::ShowVMPDebugInfo()
{
	m_tMpuVmpCfg.Print();
}


/*==============================================================================
������    :  IsEnableBitrateCheat
����      :  �Ƿ�������������
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  TRUE���������������ף�FALSE��û������
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2011/12/07  			������                  ����
=============================================================================*/
BOOL32 CMpu2VmpInst::IsEnableBitrateCheat()const
{
	return m_tMpuVmpCfg.IsEnableBitrateCheat();
}

/*==============================================================================
������    :  SetNoStreamBakByBmpPath
����      :  ����ĳһͨ����������ʾ
�㷨ʵ��  :  
����˵��  :  dwBakType��Ϊý���õ�ģʽ������ý�أ�pBmpPath����ӦͼƬ��·����
			dwChnlId:���õ�ͨ����
����ֵ˵��:  s32 ý�����ÿ���ͨ����ʾģʽ�ķ���ֵ
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2011/12/1   4.7           zhouyiliang						create
==============================================================================*/
s32 CMpu2VmpInst::SetNoStreamBakByBmpPath(u32 dwBakType,const s8* pBmpPath,u32 dwChnlId /* = MULPIC_MAX_CHNS */)
{
	s32 nReturn = 0;
	FILE* pBmp = NULL;
	u8* pBuf = NULL;//ý��û���Ƴ��ȣ�ֻ�����˿�ߣ���new
	u32 dwBmpLen = 0;
	switch (dwBakType)
	{
	case EN_PLAY_BLACK:
	case EN_PLAY_LAST:
		break;
	case EN_PLAY_BMP:
	case EN_PLAY_BMP_USR:
		if ( pBmpPath != NULL && strlen(pBmpPath) != 0 )
		{
			pBmp = fopen(pBmpPath,"r");
			if (NULL != pBmp)
			{
				fseek(pBmp,0,SEEK_END);
				dwBmpLen = ftell(pBmp);
				fseek(pBmp,0,SEEK_SET);
				pBuf = new u8[dwBmpLen];
				if (NULL != pBuf)
				{
					fread(pBuf,1,dwBmpLen,pBmp);
				}
			}
		}
		break;
	default:
		ErrorLog("[SetNoStreamBakByBmpPath]Invalid baktype:%d",dwBakType);
		break;
	}



	KeyLog("[SetNoStreamBakByBmpPath]dwBakType:%d,pBmpPath:%s,dwChnId:%d\n",dwBakType,pBmpPath,dwChnlId);
	nReturn = m_cHardMulPic.SetNoStreamBak( dwBakType ,pBuf,dwBmpLen,dwChnlId);
	if (NULL != pBmp)
	{
		fclose(pBmp);
	}
	if (NULL != pBuf)
	{
		delete[] pBuf;
	}
	return nReturn;
}

/*==============================================================================
������    :  SetIdleChnShowMode
����      :  ���ÿ���ͨ����ʾģʽ
�㷨ʵ��  :  
����˵��  :  u8:byShowMode,mcu�²ι�������ʾģʽ����Ҫת��Ϊý���õ�ģʽ������ý��
����ֵ˵��:  s32 ý�����ÿ���ͨ����ʾģʽ�ķ���ֵ
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2011/12/1   4.7           zhouyiliang						create
==============================================================================*/
s32 CMpu2VmpInst::SetIdleChnShowMode(u8 byShowMode,BOOL32 bStart)
{

	if( byShowMode > VMP_SHOW_USERDEFPIC_MODE )
	{
		byShowMode = VMP_SHOW_BLACK_MODE; // Ĭ����ʾ����
	}
	KeyLog( "[SetIdleChnShowMode] Set Vmp IdleChlShowMode: %d \n", byShowMode );
	
	s32 nReturn = 0;
	s8 szBmpFilePath[255] = {0};
	if (bStart)//Start��ʱ��������ͳһģʽ��Ȼ�����ñ�����Ӧ��ͨ��
	{
		// ���ײ����ÿ���ͨ����ʾģʽ
		u32 dwHardShowMode = (u32)EN_PLAY_BLACK;
		switch( byShowMode )
		{
			case VMP_SHOW_BLACK_MODE://VMP����ͨ����ʾΪ��ɫ��Ĭ��ֵ�������ٸ�ֵ
				break;
			case VMP_SHOW_LASTFRAME_MODE://VMP����ͨ����ʾ���һ֡
				dwHardShowMode = (u32)EN_PLAY_LAST;
				break;
			case VMP_SHOW_DEFPIC_MODE://VMP����ͨ����ʾĬ��ͼƬ
				dwHardShowMode = (u32)EN_PLAY_BMP;
				sprintf(szBmpFilePath,"%s",DEFAULT_BMP__PATH);
				break;
			case VMP_SHOW_USERDEFPIC_MODE://VMP����ͨ����ʾ�û��Զ���ͼƬ
				
				dwHardShowMode = (u32)EN_PLAY_BMP_USR;
				sprintf(szBmpFilePath,"%s",USR_BMP_PATH);
				break;
			default:		
			break;
		}
		nReturn = SetNoStreamBakByBmpPath(dwHardShowMode,szBmpFilePath);
		if ( (s32)Codec_Success != nReturn )
		{
			ErrorLog( "[SetIdleChnShowMode] start SetNoStreamBakByBmpPath mode:%d failed(ErrCode.%d)!\n",byShowMode, nReturn);
		}
	
		for (u8 nIdx = 0; nIdx < MAXNUM_MPU2VMP_MEMBER; nIdx++)
		{
			//20111108 zjl modify ֧��vmp��Ա��ʾ�Զ���ͼƬ
			for (u8 byIndex = 0; byIndex < MAXNUM_VMPDEFINE_PIC; byIndex++)
			{
				if(!m_tVmpCommonAttrb.m_atMtMember[nIdx].IsNull() &&
					strlen(m_tVmpMbAlias[nIdx].GetMbAlias()) > 0 &&
					0 == strcmp(m_tVmpMbAlias[nIdx].GetMbAlias(), 
					m_tMpuVmpCfg.GetVmpMemAlias(byIndex)))
				{
					//���ýӿ�
					KeyLog( "[SetIdleChnShowMode]index.%d -> alias.%s -> pic.%s!\n", 
						nIdx, 
						m_tMpuVmpCfg.GetVmpMemAlias(byIndex), 
						m_tMpuVmpCfg.GetVmpMemRoute(byIndex));

					nReturn = SetNoStreamBakByBmpPath((u32)EN_PLAY_BMP_USR,m_tMpuVmpCfg.GetVmpMemRoute(byIndex),nIdx);
					if ( (s32)Codec_Success != nReturn )
					{
						ErrorLog( "[SetIdleChnShowMode] Index.%d SetNoStreamBak usrbmp failed(ErrCode.%d)!\n", nIdx, nReturn);
					}
					break;
				}
			}			
			//
		}

	}
	else //change��ʱ��Ҫ���ж�������ͼƬ��ͨ����Ȼ���ٰ�����ģʽ�����иı��ͨ��
	{
		BOOL32 bSetBmp = FALSE;
		for (u8 byIdx = 0; byIdx < MAXNUM_MPU2VMP_MEMBER; byIdx++)
		{	
			bSetBmp = FALSE;
			if( !m_tVmpCommonAttrb.m_atMtMember[byIdx].IsNull())
			{
				for (u8 byIndex = 0; byIndex < MAXNUM_VMPDEFINE_PIC; byIndex++)
				{
					// �˴���byIdx��byIndex�ǲ�ͬ�ģ��޸�ʱע��
					if ( strlen(m_tVmpMbAlias[byIdx].GetMbAlias()) >0 && 
						0 == strcmp(m_tVmpMbAlias[byIdx].GetMbAlias(), m_tMpuVmpCfg.GetVmpMemAlias(byIndex)))
					{
						//���ýӿ�
						KeyLog( "[MsgChangeVidMixParamProc]index.%d -> alias.%s -> pic.%s!\n", 
							byIdx, 
							m_tMpuVmpCfg.GetVmpMemAlias(byIndex), 
							m_tMpuVmpCfg.GetVmpMemRoute(byIndex));
						
						nReturn = SetNoStreamBakByBmpPath((u32)EN_PLAY_BMP_USR,m_tMpuVmpCfg.GetVmpMemRoute(byIndex),byIdx);
						if ((s32)Codec_Success != nReturn)
						{
							ErrorLog( "[MsgChangeVidMixParamProc] SetNoStreamBak bmp .%d failed(ErrCode.%d)!\n", byIdx, nReturn);
						}
						else
						{
							bSetBmp = TRUE;
						}
						break;
					}
				}
			}
			if ( m_tVmpCommonAttrb.m_atMtMember[byIdx] == m_tPreviousVmpCommonAttrb.m_atMtMember[byIdx] || 
				m_tVmpCommonAttrb.m_atMtMember[byIdx].IsNull() && m_tPreviousVmpCommonAttrb.m_atMtMember[byIdx].IsNull() )
			{
				KeyLog("[MsgChangeVidMixParamProc]Idx :%d,previous and now member is same mt,no need to reset nostreambak,contiue.\n",byIdx);
				continue;
			}
			if (!bSetBmp )
			{
				u32 dwHardShowMode = (u32)EN_PLAY_BLACK;
				switch( byShowMode )
				{
					case VMP_SHOW_BLACK_MODE://VMP����ͨ����ʾΪ��ɫ��Ĭ��ֵ�������ٸ�ֵ
						break;
					case VMP_SHOW_LASTFRAME_MODE://VMP����ͨ����ʾ���һ֡
						dwHardShowMode = (u32)EN_PLAY_LAST;
						break;
					case VMP_SHOW_DEFPIC_MODE://VMP����ͨ����ʾĬ��ͼƬ
						dwHardShowMode = (u32)EN_PLAY_BMP;
						sprintf(szBmpFilePath,"%s",DEFAULT_BMP__PATH);
						break;
					case VMP_SHOW_USERDEFPIC_MODE://VMP����ͨ����ʾ�û��Զ���ͼƬ
						
						dwHardShowMode = (u32)EN_PLAY_BMP_USR;
						sprintf(szBmpFilePath,"%s",USR_BMP_PATH);
						break;
					default:		
					break;
				}
			
				nReturn = SetNoStreamBakByBmpPath(dwHardShowMode,szBmpFilePath,byIdx);
			
				if ((s32)Codec_Success != nReturn)
				{
					ErrorLog( "[MsgChangeVidMixParamProc] Index.%d SetNoStreamBak .%d failed(ErrCode.%d)!\n", byIdx, byShowMode, nReturn);
				}
				else
				{
					KeyLog( "[MsgChangeVidMixParamProc] Index.%d SetNoStreamBak .%d success!\n", byIdx, byShowMode);
				}
			}	
		}
	}
	
	KeyLog( "[SetIdleChnShowMode][SetNoStreamBak] return %d \n", nReturn );

	return nReturn;


}


/*==============================================================================
������    :  SetMediaSndNetParam
����      :  ���÷��Ͷ�����������
�㷨ʵ��  :  
����˵��  :  u8��byChnNo ���õĶ���index
����ֵ˵��:  TRUE�����óɹ���FALSE������ʧ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2011/12/07  			������                  ����
=============================================================================*/
BOOL32 CMpu2VmpInst::SetMediaSndNetParam(u8 byChnNo)
{
	 TNetSndParam tNetSndPar;
    memset( &tNetSndPar, 0x00, sizeof(tNetSndPar) );
	
    tNetSndPar.m_byNum  = 1;           
#ifdef _8KI_
	tNetSndPar.m_tLocalNet.m_wRTPPort       = VMP_8KEOR8KH_LOCALSND_PORT +  byChnNo * PORTSPAN; 
#else
	tNetSndPar.m_tLocalNet.m_wRTPPort       = PORT_SNDBIND + (GetInsID() -1) * PORTSPAN*2 + byChnNo * 2; //2*9 =18 ���20�պ�
#endif
    
    tNetSndPar.m_tLocalNet.m_wRTCPPort      = m_wMcuRcvStartPort + byChnNo * PORTSPAN + 1;/*PORT_SNDBIND + byChnNo * 2 + 1*/
    tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr  = htonl(m_dwMcuRcvIp);
    tNetSndPar.m_tRemoteNet[0].m_wRTPPort   = m_wMcuRcvStartPort + byChnNo * PORTSPAN;
    tNetSndPar.m_tRemoteNet[0].m_dwRTCPAddr = htonl(m_dwMcuRcvIp);
    tNetSndPar.m_tRemoteNet[0].m_wRTCPPort  = m_wMcuRcvStartPort + byChnNo * PORTSPAN + 1;

    KeyLog( "[InitMediaSnd] Snd[%d]: RTPAddr(%s)\n", byChnNo, StrOfIP(m_dwMcuRcvIp));

    u16 wRet = m_pcMediaSnd[byChnNo]->SetNetSndParam( tNetSndPar );
    if ( MEDIANET_NO_ERROR != wRet )
	{
    	ErrorLog( "[SetMediaSndNetParam] m_cMediaSnd[%d].SetNetSndParam fail, Error code is: %d \n", byChnNo, wRet );
       	return FALSE;
	}
	return TRUE;
}


/*==============================================================================
������    :  GetEncCifChnNum
����      :  ��ȡ��ǰ��cif������ͨ��
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  u8����cif������ͨ���ţ�û�г�cif��ͨ��ʱ���أ�0xFF
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2011/12/07  			������                  ����
=============================================================================*/
u8 CMpu2VmpInst::GetEncCifChnNum()
{
	u8 byCifChn = 0xFF;
	//����mpu2��cif���̶���mpu�ǹ̶��ڵ�3·(chnnum:2)��cif/other
	if (!IsSimulateMpu())
	{
		for (u8 byLoop  = 0 ; byLoop < MAXNUM_MPU2VMP_CHANNEL; byLoop++) 
		{
			if ( m_tParam[byLoop].GetEncType() == MEDIA_TYPE_H264 &&
				 m_tParam[byLoop].GetVideoWidth() == 352 && m_tParam[byLoop].GetVideoHeight() == 288 )
			{
				byCifChn = byLoop;
				break;
			}
		}
	}
	else
	{
		byCifChn = 2;
	}


	return byCifChn;
}


/*==============================================================================
������    :  GetEncOtherChnNum
����      :  ��ȡ��ǰ��other������ͨ��
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  u8����cif������ͨ���ţ�û�г�cif��ͨ��ʱ���أ�0xFF
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2011/12/07  			������                  ����
=============================================================================*/
u8 CMpu2VmpInst::GetEncOtherChnNum()
{
	u8 byOhterChn = 0xFF;
	//����mpu2��other���̶���mpu�ǹ̶��ڵ�3·(chnnum:2)��cif/other
	if (!IsSimulateMpu())
	{
		for (u8 byLoop  = 0 ; byLoop < MAXNUM_MPU2VMP_CHANNEL; byLoop++) 
		{
			if ( m_tParam[byLoop].GetEncType() != MEDIA_TYPE_H264  )
			{
				byOhterChn = byLoop;
				break;
			}
		}
	}
	else
	{
		byOhterChn = 2;
	}
	return byOhterChn;
}

/*==============================================================================
������    :  TransFromMpuParam2EncParam
����      :  ��mpu��paramת��Ϊencparam
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2011/12/07  			������                  ����
=============================================================================*/
void CMpu2VmpInst::TransFromMpuParam2EncParam(CKDVNewVMPParam* ptKdvVmpParam, TVideoEncParam* ptVideEncParam)
{
	if (NULL == ptVideEncParam || NULL == ptKdvVmpParam)
	{
		mpulog(MPU_CRIT,"[Trans2EncParam] error param (null)!");
		return;
	}
	
	// ������ֵ�͵���
	
	ptVideEncParam->m_byEncType    = ptKdvVmpParam->m_byEncType;
	ptVideEncParam->m_wBitRate     = ptKdvVmpParam->GetBitRate();
	
	
	u8 byFrameRate =  ptKdvVmpParam->GetFrameRate();
	u8 byRes = GetResViaWH(ptKdvVmpParam->GetVideoWidth(), ptKdvVmpParam->GetVideoHeight());
	//��ֱ�������������Ϣ�������ֱ��ʺ�֡��
	//������ԭʼ������
	u16 wOrigBitrate = m_tMpuVmpCfg.GetOrigRate(  ptKdvVmpParam->GetBitRate() );
	BOOL32 bGetRet = g_tResBrLimitTable.GetRealResFrameRatebyBitrate(ptKdvVmpParam->m_byEncType,byRes,
		byFrameRate,wOrigBitrate);
	if (!bGetRet)
	{
		OspPrintf(TRUE,FALSE,"[MsgStartVidMixProc]ERROR: GetRealResFrameratebyBitrate return FALSE\n");
	}
	u16 wVideoWith = 0;
	u16 wVideoHeight = 0;
	GetWHViaRes(byRes,wVideoWith,wVideoHeight);
	if( VIDEO_FORMAT_4CIF == byRes && MEDIA_TYPE_MP4 == ptKdvVmpParam->m_byEncType )
	{
		wVideoWith = 720;
		wVideoHeight = 576;
	}	
	ptVideEncParam->m_wVideoHeight = wVideoHeight; 
	ptVideEncParam->m_wVideoWidth  = wVideoWith;
	
	//֡�ʵ���
	ptVideEncParam->m_byFrameRate = 0;
	if( ptKdvVmpParam->GetFrameRate() != 0 )
	{
		mpulog( MPU_INFO, "Set vmp frame rate to %d\n", byFrameRate);
		
		ptVideEncParam->m_byFrameRate = byFrameRate;
	}

	if ( (ptVideEncParam->m_wVideoWidth == 1920 &&
		ptVideEncParam->m_wVideoHeight == 1088) ||
		(ptVideEncParam->m_wVideoWidth == 1920 &&
		ptVideEncParam->m_wVideoHeight == 1080) )
	{
		ptVideEncParam->m_dwMaxKeyFrameInterval = 3000;
	}
	else if ( ptVideEncParam->m_wVideoWidth == 1280 &&
		ptVideEncParam->m_wVideoHeight == 720 )
	{
		ptVideEncParam->m_dwMaxKeyFrameInterval = 3000;
	}
	else if ( (ptVideEncParam->m_wVideoWidth == 720 ||
		ptVideEncParam->m_wVideoWidth == 704) &&
		ptVideEncParam->m_wVideoHeight == 576 )
	{
		ptVideEncParam->m_dwMaxKeyFrameInterval = 1000;
	}
	else if ( ptVideEncParam->m_wVideoWidth == 352 &&
		ptVideEncParam->m_wVideoHeight == 288 )
	{
		ptVideEncParam->m_dwMaxKeyFrameInterval = 500;
	}
	
	// xliang [5/26/2009] ������������
	if(ptVideEncParam->m_byEncType == MEDIA_TYPE_H264 )
	{
		ptVideEncParam->m_byMaxQuant = 45;
		ptVideEncParam->m_byMinQuant = 20;
	}
	else//h263 ,mp4
	{
		mpulog( MPU_INFO, "set h263/mp4 Quant and MaxKeyFrameInterval!\n");
		ptVideEncParam->m_byMaxQuant = 30;
		ptVideEncParam->m_byMinQuant = 3;
		ptVideEncParam->m_dwMaxKeyFrameInterval = 300;
	}
	
}

/*==============================================================================
������    :  TransFromMpuParam2EncParam
����      :  ��mpu2��paramת��Ϊencparam
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2011/12/07  			������                  ����
=============================================================================*/
void CMpu2VmpInst::TransFromMpu2Param2EncParam(CKDVVMPOutMember* ptKdvVmpParam,TVideoEncParam* ptVideEncParam)
{
	if (NULL == ptVideEncParam || NULL == ptKdvVmpParam)
	{
		ErrorLog("[TransFromMpu2Param2EncParam] error param (null)!");
		return;
	}
	
	// ������ֵ�͵���
	
	ptVideEncParam->m_byEncType    = ptKdvVmpParam->GetEncType();
	ptVideEncParam->m_wBitRate     = ptKdvVmpParam->GetBitRate();
	
	
	u8 byFrameRate =  ptKdvVmpParam->GetFrameRate();
	u8 byRes = GetResViaWH(ptKdvVmpParam->GetVideoWidth(), ptKdvVmpParam->GetVideoHeight());

	//��ֱ�������������Ϣ�������ֱ��ʺ�֡��
	//������ԭʼ����
	u16 wOrigBitrate = m_tMpuVmpCfg.GetOrigRate(  ptKdvVmpParam->GetBitRate() );
	KeyLog("[TransFromMpu2Param2EncParam]Before GetRealResFrameRatebyBitrate \n:width:%d,height:%d,framerate:%d,wOrigBitrate:%d,ProfileType:%d\n",
		ptKdvVmpParam->GetVideoWidth(),ptKdvVmpParam->GetVideoHeight(),ptKdvVmpParam->GetFrameRate(),wOrigBitrate,ptKdvVmpParam->GetProfileType());
	BOOL32 bGetRet = g_tResBrLimitTable.GetRealResFrameRatebyBitrate( ptKdvVmpParam->GetEncType(),byRes,
		byFrameRate,wOrigBitrate,ptKdvVmpParam->GetProfileType());
	if (!bGetRet)
	{
		OspPrintf(TRUE,FALSE,"[TransFromMpu2Param2EncParam]ERROR: GetRealResFrameratebyBitrate return FALSE\n");
		ErrorLog("[TransFromMpu2Param2EncParam]ERROR: GetRealResFrameratebyBitrate return FALSE\n");
	}
	u16 wVideoWith = 0;
	u16 wVideoHeight = 0;
	GetWHViaRes(byRes,wVideoWith,wVideoHeight);
	if( VIDEO_FORMAT_4CIF == byRes && MEDIA_TYPE_MP4 == ptKdvVmpParam->GetEncType() )
	{
		wVideoWith = 720;
		wVideoHeight = 576;
	}
	
	KeyLog("[TransFromMpu2Param2EncParam]After GetRealResFrameRatebyBitrate \n:width:%d,height:%d,framerate:%d,ProfileType:%d\n",
		wVideoWith,wVideoHeight,byFrameRate,ptKdvVmpParam->GetProfileType());
	ptVideEncParam->m_wVideoHeight = wVideoHeight; 
	ptVideEncParam->m_wVideoWidth  = wVideoWith;
	ptVideEncParam->m_dwProfile = ptKdvVmpParam->GetProfileType();
	//֡�ʵ���
	ptVideEncParam->m_byFrameRate = 0;
	if( ptKdvVmpParam->GetFrameRate() != 0 )
	{
		KeyLog("Set vmp frame rate to %d\n", byFrameRate);
		ptVideEncParam->m_byFrameRate = byFrameRate;
	}
	
	
	
	
	if ( (ptVideEncParam->m_wVideoWidth == 1920 &&
		ptVideEncParam->m_wVideoHeight == 1088) ||
		(ptVideEncParam->m_wVideoWidth == 1920 &&
		ptVideEncParam->m_wVideoHeight == 1080) )
	{
		ptVideEncParam->m_dwMaxKeyFrameInterval = 3000;
	}
	else if ( ptVideEncParam->m_wVideoWidth == 1280 &&
		ptVideEncParam->m_wVideoHeight == 720 )
	{
		ptVideEncParam->m_dwMaxKeyFrameInterval = 3000;
	}
	else if ( (ptVideEncParam->m_wVideoWidth == 720 ||
		ptVideEncParam->m_wVideoWidth == 704) &&
		ptVideEncParam->m_wVideoHeight == 576 )
	{
		ptVideEncParam->m_dwMaxKeyFrameInterval = 1000;
	}
	else if ( ptVideEncParam->m_wVideoWidth == 352 &&
		ptVideEncParam->m_wVideoHeight == 288 )
	{
		ptVideEncParam->m_dwMaxKeyFrameInterval = 500;
	}
	
	// xliang [5/26/2009] ������������
	if(ptVideEncParam->m_byEncType == MEDIA_TYPE_H264 )
	{
		ptVideEncParam->m_byMaxQuant = 45;
		ptVideEncParam->m_byMinQuant = 20;
	}
	else//h263 ,mp4
	{
		mpulog( MPU_INFO, "set h263/mp4 Quant and MaxKeyFrameInterval!\n");
		ptVideEncParam->m_byMaxQuant = 30;
		ptVideEncParam->m_byMinQuant = 3;
		ptVideEncParam->m_dwMaxKeyFrameInterval = 300;
	}
	
}
/*==============================================================================
������    :  DisplayMbAlias
����      :  ��ʾ�ն˱���
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2012/06/06  			������                  ����
=============================================================================*/
void CMpu2VmpInst::DisplayMbAlias(u8 const byChnnl,const s8* pMemberAlias)
{
	//param check 
	if ( NULL == pMemberAlias || byChnnl >= MAXNUM_MPU2VMP_MEMBER)
	{
		ErrorLog("[DisplayMbAlias]Error:pMemberAlias is Null Pointer byChnl:%d.\n",byChnnl);
		return;
	}
	if ( FALSE == IsDisplayMbAlias() )
	{
		ErrorLog("[DisplayMbAlias]Current NotDisplayMbAlias\n");
		return;
	}

	s8 szAlias[MAXLEN_ALIAS]={0};
	memcpy( szAlias,pMemberAlias, min(strlen(pMemberAlias), MAXLEN_ALIAS-1) );
	
	// ����������Ϊ0������ʾ����
	if (0 == strlen(szAlias))
	{
		ErrorLog("[DisplayMbAlias] Chl:%d Alias length is 0.\n", byChnnl);
		m_cHardMulPic.StopAddIcon(byChnnl);
		return;
	}

	//��ȡ��ͨ���Ĵ�С
	u8 byStyle = VMP_STYLE_NONE;
    byStyle = m_tVmpCommonAttrb.m_byVmpStyle;
	if (VMP_STYLE_NONE == byStyle || byStyle >= VMPSTYLE_NUMBER)
	{
		ErrorLog("[DisplayMbAlias] error style:%d\n",byStyle);
		return;
	}
	u8 byChnRes = m_tMpuVmpCfg.m_aVMPStyleChnlRes[byStyle][byChnnl];
	u16 wChnlWidth = 0;
	u16 wChnlHeight = 0 ;
	GetWHViaRes(byChnRes,wChnlWidth,wChnlHeight);
	if ( 0 == wChnlWidth  || 0 == wChnlHeight )
	{
		ErrorLog("[DisplayMbAlias] GetWHViaRes error, byChnRes:%d\n",byChnRes);
		return;
	}
    KeyLog("[DisplayMbAlias] Style=%d,Chnl=%d,ChnRes=%d,ChnWidth=%d,ChnHeight=%d\n",byStyle,byChnnl,byChnRes,wChnlWidth,wChnlHeight);
	
    //��ȡdrawbmp������Ŀ�ߵ���Ϣ
	TDrawParam tDrawParam;
	tDrawParam.SetText(szAlias);

    // ͳһ�����СΪ22�� [8/5/2013 liaokang]
    tDrawParam.SetFontSize(22);
    // ����ͨ�����������С [7/8/2013 liaokang] 
    // ͨ����ȣ�1920��1280��960��640��480��352 
    // StartAddIcon��ý���޶��˴�С600*50, ����ײ�10������Ԥ��,����޶�Ϊ600*4
    // 16�����֣�30������ռ500*40   24������ռ376*32��16������ռ256*24
//     if ( wChnlWidth >=1280 )
//     {
//         tDrawParam.SetFontSize(FONT_SIZE32);
//     }
//     else if ( wChnlWidth > 480 && wChnlWidth < 1280 )
//     {
//         tDrawParam.SetFontSize(FONT_SIZE24);
//     }
//     else
//     {
//         tDrawParam.SetFontSize(FONT_SIZE16);
//     }

    tDrawParam.SetTextEncode((u8)GetMcuEncoding());
	TPic tPic;
	BOOL32 bDrawBmp = m_cBmpDrawer.DrawBmp(tPic, tDrawParam);
	if (!bDrawBmp)
	{
		ErrorLog("[DisplayMbAlias] DrawBmp return error, byChnnl:%d\n",byChnnl);
		return;
	}
	KeyLog("[DisplayMbAlias] Chnl:%d,drawbmp succeed,width=%d,height=%d\n",byChnnl,tPic.GetPicWidth(),tPic.GetPicHeight());
	u32 dwXpos = 0;
	u32 dwYpos = 0;
	if ( wChnlWidth < tPic.GetPicWidth() || wChnlHeight < (tPic.GetPicHeight() + 10) )
	{
		ErrorLog("[DisplayMbAlias] byChnnl:%d drawbmp return pic height or width is bigger than channel height or width\n",byChnnl);
		return;
	}
	dwXpos = (wChnlWidth - tPic.GetPicWidth())/2;
	dwYpos = wChnlHeight - tPic.GetPicHeight() - 10;//����ײ�10������Ԥ��
	TBackBGDColor tBackBGDColor;
	memset(&tBackBGDColor,0,sizeof(TBackBGDColor));

	u8 abyPicBuf[MAX_BMPFILE_SIZE] = {0};
	memcpy(abyPicBuf,tPic.GetPic(),tPic.GetPicSize() );
	s32 nRet = m_cHardMulPic.StartAddIcon(byChnnl, abyPicBuf , tPic.GetPicSize(), dwXpos, dwYpos,  
		tPic.GetPicWidth(),tPic.GetPicHeight(),  tBackBGDColor,  0);
	KeyLog("[DisplayMbAlias] Chnl:%d,displayAlias:%s,nRet:%d(CORRECT:%d)\n",byChnnl,szAlias,nRet,HARD_MULPIC_OK);
}
//END OF FILE

