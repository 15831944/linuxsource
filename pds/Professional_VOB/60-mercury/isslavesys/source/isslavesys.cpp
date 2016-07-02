/*****************************************************************************
ģ����      : Board Agent
�ļ���      : isslavesys.cpp
����ļ�    : isslavesys.h
�ļ�ʵ�ֹ���: IS��ϵͳ��IS2.2 8313��ҵ��
����        : liaokang
�汾        : V4R7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
2012/05/05  4.7         liaokang          ����
******************************************************************************/
#include "isslavesys.h"

#ifdef _LINUX12_
    #include "nipdebug.h"
#endif

CBrdSlaveSysApp	g_cBrdSlaveSysApp;

//���캯��
CBrdSlaveSysInst::CBrdSlaveSysInst()
{
/*lint -save -e1566 */
    ClearInst();
/*lint -restore*/
}

//��������
CBrdSlaveSysInst::~CBrdSlaveSysInst()
{
/*lint -save -e1551 */
    ClearInst();
/*lint -restore*/
}

/*====================================================================
    ������      : InstanceEntry
    ����        : ʵ����Ϣ������ں���������override
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  :
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2012/03/26  4.7         liaokang      ����
====================================================================*/
void CBrdSlaveSysInst::InstanceEntry(CMessage* const pcMsg)
{
    if( NULL == pcMsg )
    {
        IsSlaveSysLog( LOG_ERROR, "[InstanceEntry] NULL pointer!\n");
        return;
	}

    IsSlaveSysLog( LOG_DETAIL, "[InstanceEntry] %u(%s) received!\n",
		pcMsg->event, OspEventDesc(pcMsg->event));

	switch( pcMsg->event )
	{
    case OSP_POWERON:                               // ����
        ProcBrdSlaveSysPowerOn( pcMsg ) ;
        break;

    case BRDSYS_SLAVE_MASTER_CONNECT_TIMER:         // ����TimeOut
        ProcConnectBrdMasterSysTimeOut();
        break;

    case BRDSYS_MASTER_SLAVE_REG_ACK:               // ע��ACK
    case BRDSYS_MASTER_SLAVE_REG_NACK:              // ע��NACK
        ProcRegToBrdMasterSysRsp( pcMsg );
		break;
                
    case BRDSYS_SLAVE_MASTER_REG_TIMER:		        // ע��TimeOut
        ProcRegToBrdMasterSysTimeOut( );
		break;

    case BRDSYS_SLAVE_SCAN_STATE_TIMER:		        // ��ʱɨ��
        ProcBrdSlaveSysStateScan();
		break;

    case MPC_BOARD_UPDATE_SOFTWARE_CMD:			    // ����
        ProcBrdSlaveSysUpdateSoftwareCmd( pcMsg );
		break;

    case MPC_BOARD_RESET_CMD:                       // ������
        ProcBrdSlaveSysResetCmd( pcMsg );
		break;

    case OSP_DISCONNECT:                            // ��������
        ProcDisconnect();
		break;
 
    default:
        IsSlaveSysLog( LOG_WARNING, "[InstanceEntry] unknown msg %u(%s) received!\n", 
            pcMsg->event, OspEventDesc(pcMsg->event));
        break;
	}
	return;
}

/*====================================================================
    ������      : ProcBrdSlaveSysPowerOn
    ����        : ����
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  :
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2012/05/05  4.7         liaokang      ����
====================================================================*/
void CBrdSlaveSysInst::ProcBrdSlaveSysPowerOn( CMessage* const pcMsg )
{
    if ( NULL == pcMsg )
    {
        IsSlaveSysLog( LOG_ERROR, "[ProcBrdSlaveSysPowerOn] Null pointer!\n" );
        return;
    }

    SetTimer( BRDSYS_SLAVE_MASTER_CONNECT_TIMER, 10 ); // ���� 
	return;
}

/*====================================================================
    ������      : ProcConnectBrdMasterSysTimeOut
    ����        : ����IS��ϵͳ��IS2.2 8548��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2012/05/05  4.7         liaokang      ����
====================================================================*/
void CBrdSlaveSysInst::ProcConnectBrdMasterSysTimeOut()
{
    u32 dwMpcIp = g_cBrdSlaveSysApp.GetBrdMasterSysInnerIP();
	u16 wMpcPort = g_cBrdSlaveSysApp.GetBrdMasterSysListenPort();
    switch( CurState() ) 
	{
    case STATE_IDLE:
        m_dwBrdMasterSysNode = OspConnectTcpNode( htonl( dwMpcIp ), wMpcPort );
        
        if ( !OspIsValidTcpNode( m_dwBrdMasterSysNode ) )
        {
            m_dwBrdMasterSysNode = INVALID_NODE;
            IsSlaveSysLog( LOG_ERROR, "[ProcConnectBrdMasterSysTimeOut] CreateTcpNode Failed! MasterSys Ip: %x, MasterSys Port: %d!\n", dwMpcIp, wMpcPort );
            SetTimer( BRDSYS_SLAVE_MASTER_CONNECT_TIMER, CONNECT_BRDMASTERSYS_TIMEOUT );
            return;
        }
        
        OspNodeDiscCBRegQ( m_dwBrdMasterSysNode, GetAppID(), GetInsID());
        OspSetHBParam( m_dwBrdMasterSysNode, HEART_BEAT_TIME, HEART_BEAT_NUM );
        IsSlaveSysLog( LOG_ERROR, "[ProcConnectBrdMasterSysTimeOut] Connect Success! MasterSys Ip: %x, MasterSys Port: %d,  Node:%d!\n", dwMpcIp, wMpcPort, m_dwBrdMasterSysNode );
        NextState( STATE_INIT );
        //����ע������
        SetTimer( BRDSYS_SLAVE_MASTER_REG_TIMER, 10 );

        break;
        
    default:
        IsSlaveSysLog( LOG_ERROR, "[ProcConnectBrdMasterSysTimeOut] Wrong state %u !\n", CurState() );
        break;
    }

	return;
}

/*====================================================================
    ������      : ProcRegToBrdMasterSysRsp
    ����        : ע��IS��ϵͳ��IS2.2 8548����Ӧ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  :
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2012/05/05  4.7         liaokang      ����
====================================================================*/
void CBrdSlaveSysInst::ProcRegToBrdMasterSysRsp( CMessage* const pcMsg )
{
    u16 wCurEvent = pcMsg->event;

    KillTimer( BRDSYS_SLAVE_MASTER_REG_TIMER );

    if ( BRDSYS_MASTER_SLAVE_REG_ACK == wCurEvent ) // ACK
    {  
        if( STATE_INIT == CurState() )
        { 
            NEXTSTATE(STATE_NORMAL);
            SetTimer( BRDSYS_SLAVE_SCAN_STATE_TIMER, 10);      // ����ɨ�裬ͬ��led������״̬
            return;
        }        
    }
    else // NACK 
    {
        SetTimer( BRDSYS_SLAVE_MASTER_REG_TIMER, REGISTER_BRDMASTERSYS_TIMEOUT );
    }

}

/*====================================================================
    ������      : ProcRegToBrdMasterSysTimeOut
    ����        : ע��IS��ϵͳ��IS2.2 8548��TimeOut
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��:
    ����ֵ˵��  :
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2012/05/05  4.7         liaokang      ����
====================================================================*/
void CBrdSlaveSysInst::ProcRegToBrdMasterSysTimeOut( )
{ 
    switch( CurState() ) 
    {
    case STATE_INIT:
        PostMsgToBrdMasterSys( BRDSYS_SLAVE_MASTER_REG_REQ ); // ע��
        SetTimer( BRDSYS_SLAVE_MASTER_REG_TIMER, REGISTER_BRDMASTERSYS_TIMEOUT );
        break;
    default:
        IsSlaveSysLog( LOG_ERROR, "[ProcRegToBrdMasterSysTimeOut] Wrong state %u !\n", CurState() );
        break;
    }
}

/*====================================================================
    ������      : ProcBrdSlaveSysStateScan
    ����        : ��ʱɨ��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: void
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2012/05/05  4.7         liaokang      ����
====================================================================*/
void CBrdSlaveSysInst::ProcBrdSlaveSysStateScan(void)
{
    TBrdLedState tBrdLedState;
    SWEthInfo    tBrdEthPortState;
    memset( &tBrdLedState, 0, sizeof( TBrdLedState ) );
    memset( &tBrdEthPortState, 0, sizeof( SWEthInfo ) );

    // led
    BrdQueryLedState( &tBrdLedState );
    if( memcmp( &tBrdLedState, &m_tBrdSlaveSysLedState, sizeof(TBrdLedState)) != 0 )
    {
        IsSlaveSysLog( LOG_ERROR, "[ProcBrdSlaveSysStateScan] Led State changed!\n" );
        memcpy( &m_tBrdSlaveSysLedState, &tBrdLedState, sizeof(TBrdLedState) );
        PostMsgToBrdMasterSys( BRDSYS_SLAVE_MASTER_LED_STATUS_NOTIFY, &m_tBrdSlaveSysLedState, sizeof(TBrdLedState) );
	}

    // ����
#ifdef _LINUX12_
    SwGetEthNego( &tBrdEthPortState );
#endif
    if( memcmp( &tBrdEthPortState, &m_tBrdSlaveSysEthPortState, sizeof( SWEthInfo )) != 0 )
    {
        IsSlaveSysLog( LOG_KEYSTATUS, "[ProcBrdSlaveSysStateScan] EthPort State changed!\n" );
        memcpy( &m_tBrdSlaveSysEthPortState, &tBrdEthPortState, sizeof( SWEthInfo ) );
        PostMsgToBrdMasterSys( BRDSYS_SLAVE_MASTER_ETHPORT_STATUS_NOTIFY, &m_tBrdSlaveSysEthPortState, sizeof( SWEthInfo ) );
	}

    SetTimer( BRDSYS_SLAVE_SCAN_STATE_TIMER, SCAN_BRDSLAVESYS_STATE_TIMEOUT);      // ����ɨ�趨ʱ
}

/*====================================================================
    ������      : ProcBrdSlaveSysUpdateSoftwareCmd
    ����        : ����
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  :
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2012/05/05  4.7         liaokang      ����
====================================================================*/
/*lint -save -e715 */
void CBrdSlaveSysInst::ProcBrdSlaveSysUpdateSoftwareCmd( CMessage* const pcMsg )
{
    // �������䣬���pcmsg���ô�����ȥ��pclint����
    // ����ȥ��pcmsg��ȥ��pclint����
    return;
}
/*lint -restore*/

/*====================================================================
    ������      : ProcBrdSlaveSysResetCmd
    ����        : ������
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  :
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2012/05/05  4.7         liaokang      ����
====================================================================*/
/*lint -save -e715 */
void CBrdSlaveSysInst::ProcBrdSlaveSysResetCmd( CMessage* const pcMsg )
{
	IsSlaveSysLog( LOG_ERROR, "[ProcBrdSlaveSysResetCmd] reboot 8313 board\n");
	OspDelay(1000);		
    BrdHwReset();

    // �������䣬���pcmsg���ô�����ȥ��pclint����
    // ����ȥ��pcmsg��ȥ��pclint����
    return;
}
/*lint -restore*/

/*====================================================================
    ������      : ProcDisconnect
    ����        : ��������
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: void
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2012/05/05  4.7         liaokang      ����
====================================================================*/
void CBrdSlaveSysInst::ProcDisconnect( void )
{ 
    IsSlaveSysLog( LOG_ERROR, "[ProcOspDisconnect] node(%d) disconnect. connecting...\n", m_dwBrdMasterSysNode );
    KillTimer( BRDSYS_SLAVE_MASTER_REG_TIMER );
    KillTimer( BRDSYS_SLAVE_SCAN_STATE_TIMER );
    ClearInst();
    SetTimer( BRDSYS_SLAVE_MASTER_CONNECT_TIMER, 10 );

	IsSlaveSysLog( LOG_ERROR, "[ProcOspDisconnect] reboot 8313 board\n");
	OspDelay(1000);		
    BrdHwReset();

	return;
}

/*====================================================================
    ������      : PostMsgToBrdMasterSys
    ����        : ������Ϣ��IS��ϵͳ��IS2.2 8548��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  :
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2012/05/05  4.7         liaokang      ����
====================================================================*/
void CBrdSlaveSysInst::PostMsgToBrdMasterSys( u16 wEvent, void * pMsg, u16 wLen)
{
    switch( CurState() ) 
    {
    case STATE_INIT:
    case STATE_NORMAL:

        if( OspIsValidTcpNode( m_dwBrdMasterSysNode ) )
        {
            post( MAKEIID( AID_MCU_BRDAGENT, 1 ), wEvent, pMsg, wLen, m_dwBrdMasterSysNode );
            IsSlaveSysLog( LOG_DETAIL, "[PostMsgToBrdMasterSys] Post message %u(%s) to Master system!\n",  wEvent, ::OspEventDesc( wEvent ) );
        }
        else
        {
            IsSlaveSysLog( LOG_ERROR, "[PostMsgToBrdMasterSys] Post message %u(%s) to Master system  ERROR!\n",  wEvent, ::OspEventDesc( wEvent ) );
        }
        break;

    default:
        IsSlaveSysLog( LOG_ERROR, "[PostMsgToBrdMasterSys] Wrong state %u !\n", CurState() );
        break;
    }
}

/*====================================================================
    ������      : ClearInst
    ����        : ����ʵ��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  :
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2012/05/05  4.7         liaokang      ����
====================================================================*/
void CBrdSlaveSysInst::ClearInst()
{
    memset( &m_tBrdSlaveSysLedState, 0, sizeof( TBrdLedState ) );
    memset( &m_tBrdSlaveSysEthPortState, 0, sizeof( SWEthInfo ) );
    m_dwBrdMasterSysNode = 0;
    m_curState = STATE_IDLE;
}

//���캯��
CBrdSlaveSysCfg::CBrdSlaveSysCfg():m_dwBrdMasterSysInnerIP(0),
                                   m_wBrdMasterSysListenPort(0)
{
}

//��������
CBrdSlaveSysCfg::~CBrdSlaveSysCfg()
{
    m_dwBrdMasterSysInnerIP = 0;    // �ڲ�ͨ�� ip��second ip��
    m_wBrdMasterSysListenPort = 0;  // ��ϵͳ��IS2.2 8548�� Listen Port
}

/*====================================================================
    ������      : ReadConnectConfigInfo
    ����        : ������������Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    ����ֵ˵��  : BOOL32
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2012/05/05  4.7         liaokang      ����
====================================================================*/
BOOL32 CBrdSlaveSysCfg::ReadConnectConfigInfo()
{
    s8      achProfileName[32] = {0};
    BOOL32  bResult = FALSE;
    s8      achDefStr[] = "Cannot find the section or key";
    s8      achReturn[MAX_VALUE_LEN + 1] = {0};
    s32     nValue =0 ;
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);

	// ��ϵͳ��IS2.2 8548���ڲ�ͨ�� ip��second ip��
    bResult = GetRegKeyString( achProfileName, SECTION_BoardMasterSystem, KEY_BrdMasterSysInnerIp, 
						achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )  
	{
        IsSlaveSysLog( LOG_ERROR, "[ReadConnectConfigInfo] Wrong profile while reading %s!\n", KEY_BrdMasterSysInnerIp );
		return FALSE;
	}
	m_dwBrdMasterSysInnerIP =  ntohl( INET_ADDR( achReturn ) );
    if( 0xffffffff == m_dwBrdMasterSysInnerIP ||  0 == m_dwBrdMasterSysInnerIP )
    {
        m_dwBrdMasterSysInnerIP = 0;
        IsSlaveSysLog( LOG_ERROR, "[ReadConnectConfigInfo] The BrdMasterSys second Ip is 0!\n" );
        return FALSE;
    }
    
    // ��ϵͳ��IS2.2 8548�� Listen Port
    bResult = GetRegKeyInt( achProfileName, SECTION_BoardMasterSystem, KEY_BrdMasterSysListenPort, 
        DEFVALUE_BrdMasterSysListenPort, &nValue );
	if( bResult == FALSE )  
	{
        IsSlaveSysLog( LOG_ERROR, "[ReadConnectConfigInfo] Wrong profile while reading %s!\n", KEY_BrdMasterSysListenPort );
	}
	m_wBrdMasterSysListenPort = ( u16 )nValue;

    return TRUE;
}

/*====================================================================
    ������      : InitLocalCfgInfoByCfgFile
    ����        : �������ļ���ʼ������������Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2012/05/05  4.7         liaokang      ����
====================================================================*/
BOOL32 CBrdSlaveSysCfg::InitLocalCfgInfoByCfgFile()
{    
    s8      achProfileName[32] = {0};
    BOOL32  bResult = FALSE;
    s8      achDefStr[] = "Cannot find the section or key";
    s8      achReturn[MAX_VALUE_LEN + 1] = {0};
    s32     nValue = 0;
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);

    // ֧��ͨ�������ļ����õ����ϵͳ��IS2.2�� 8313ϵͳ��ip
    BOOL32  bIsSupportIpConfig = 0;         // �Ƿ�֧�����õ����ϵͳ��IS2.2�� 8313ϵͳ����0���ǣ�1
    u8      byEthChoice = 1;                // ��ϵͳ��IS2.2 8313�� ǰ/������  ǰ���ڣ�0�������ڣ�1
    u32     dwBrdSlaveSysIP = 0;            // ��ϵͳ��IS2.2 8313�� ip
    u32     dwBrdSlaveSysInnerIP = 0;       // ��ϵͳ��IS2.2 8313�� second ip
    u32     dwBrdSlaveSysIPMask = 0;        // ��ϵͳ��IS2.2 8313�� ����
    u32     dwBrdSlaveSysDefGateway = 0;    // ��ϵͳ��IS2.2 8313�� Ĭ������
    TBrdEthParam tEthParam;                 // ��̫�������ṹ
    memset( &tEthParam, 0, sizeof(tEthParam) );
    TBrdEthParamAll tBrdEthParamAll;        // ��̫�������ṹ
    memset( &tBrdEthParamAll, 0, sizeof(tBrdEthParamAll) );
    
    // 1���Ƿ�֧�����õ����ϵͳ��IS2.2�� 8313ϵͳ��
    bResult = GetRegKeyInt( achProfileName, SECTION_IpConfig, KEY_IsSupportIpConfig, DEFVALUE_IsSupportIpConfig, &nValue );
    if( bResult == FALSE )  
    {
        IsSlaveSysLog( LOG_ERROR, "[InitLocalCfgInfoByCfgFile] Wrong profile while reading %s!\n", KEY_IsSupportIpConfig );
        return FALSE;
    }
	bIsSupportIpConfig = ( BOOL32 )nValue; 
    if ( 0 == bIsSupportIpConfig ) // ��֧��
    {
        IsSlaveSysLog( LOG_KEYSTATUS, "[InitLocalCfgInfoByCfgFile] Do not support Ip Config in config file!\n" );
        return TRUE;
    }

    // 2����ϵͳ��IS2.2 8313������
    bResult = GetRegKeyString( achProfileName, SECTION_IpConfig, KEY_BrdSlaveSysDefGateway, 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )  
    {
        IsSlaveSysLog( LOG_ERROR, "[InitLocalCfgInfoByCfgFile] Wrong profile while reading %s!\n", KEY_BrdSlaveSysDefGateway );
    }
    dwBrdSlaveSysDefGateway =  ntohl( INET_ADDR( achReturn ) );

    /* BrdSetDefGateway BrdGetDefGateway ���������� */
    if (  0xffffffff == dwBrdSlaveSysDefGateway || 0 == dwBrdSlaveSysDefGateway )
    {   
        IsSlaveSysLog( LOG_ERROR, "[InitLocalCfgInfoByCfgFile] The default gateway is 0 in config file\n" );
#ifdef _LINUX12_
        u32 dwDefGateway = ntohl( BrdGetDefGateway() );
        if ( 0 == dwDefGateway )
        {
            IsSlaveSysLog( LOG_ERROR, "[InitLocalCfgInfoByCfgFile] Get default gateway failed!\n" );
            return FALSE;
        }  
#endif
    }
    else
    {
#ifdef _LINUX12_
        // ���ô�ϵͳ��IS2.2 8313������
        s32 nRet = BrdSetDefGateway( htonl( dwBrdSlaveSysDefGateway ) );
        if ( ERROR == nRet )
        {
            IsSlaveSysLog( LOG_ERROR, "[InitLocalCfgInfoByCfgFile] Set default gateway : %0x failed!\n", dwBrdSlaveSysDefGateway );
            return FALSE;
        }
        else
        {
            IsSlaveSysLog( LOG_DETAIL, "[InitLocalCfgInfoByCfgFile] Set default gateway : %0x success!\n", dwBrdSlaveSysDefGateway );
        }
#endif
    }

    // 3����ϵͳ��IS2.2�� 8313ϵͳ��ǰ/������,Ĭ�Ϻ�����
    bResult = GetRegKeyInt( achProfileName, SECTION_IpConfig, KEY_EthChoice, DEFVALUE_EthChoice, &nValue );
    if( bResult == FALSE )
    {
        IsSlaveSysLog( LOG_ERROR, "[InitLocalCfgInfoByCfgFile] Wrong profile while reading %s!\n", KEY_EthChoice );
    }
	byEthChoice = ( u8 )nValue; 
    IsSlaveSysLog( LOG_DETAIL, "[InitLocalCfgInfoByCfgFile] Set Ethernet Choice : %u!\n", byEthChoice );  

    // 4����ϵͳ��IS2.2 8313������
    bResult = GetRegKeyString( achProfileName, SECTION_IpConfig, KEY_BrdSlaveSysIPMask, 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )  
    {
        IsSlaveSysLog( LOG_ERROR, "[InitLocalCfgInfoByCfgFile] Wrong profile while reading %s!\n", KEY_BrdSlaveSysIPMask );
    }
    dwBrdSlaveSysIPMask =  ntohl( INET_ADDR( achReturn ) );
    if( 0xffffffff == dwBrdSlaveSysIPMask )
    {
        dwBrdSlaveSysIPMask = 0;
    }
    if ( 0 == dwBrdSlaveSysIPMask )
    {   
        IsSlaveSysLog( LOG_ERROR, "[InitLocalCfgInfoByCfgFile] The ip mask is 0 in config file!\n" );
        // Ĭ�ϲ���ԭ������
        memset( &tBrdEthParamAll, 0, sizeof( TBrdEthParamAll ) );
        s32 nRet = BrdGetEthParamAll( byEthChoice, &tBrdEthParamAll );
        if ( ERROR == nRet
            || 0 == tBrdEthParamAll.dwIpNum 
            || ( tBrdEthParamAll.dwIpNum > 0 && 0 == tBrdEthParamAll.atBrdEthParam[0].dwIpMask ) )
        {
            IsSlaveSysLog( LOG_KEYSTATUS, "[InitLocalCfgInfo] The primary IpMask is not exist in board system! Please config...\n" );
            return FALSE;
        }
        dwBrdSlaveSysIPMask = ntohl( tBrdEthParamAll.atBrdEthParam[0].dwIpMask );
    }

    // 5����ϵͳ��IS2.2 8313��ip
    bResult = GetRegKeyString( achProfileName, SECTION_IpConfig, KEY_BrdSlaveSysIp, 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )  
    {
        IsSlaveSysLog( LOG_ERROR, "[InitLocalCfgInfoByCfgFile] Wrong profile while reading %s!\n", KEY_BrdSlaveSysIp );
    }
    dwBrdSlaveSysIP =  ntohl( INET_ADDR( achReturn ) );

     /* Must add primary ip before adding secondary ip */
    if ( 0xffffffff == dwBrdSlaveSysIP || 0 == dwBrdSlaveSysIP )
    {   
        IsSlaveSysLog( LOG_ERROR, "[InitLocalCfgInfoByCfgFile] The primary IpAddr is 0 in config file!\n" );  
#ifdef _LINUX12_
        // �жϴ�ϵͳ��IS2.2 8313����һIP�Ƿ��Ѿ�����        
        memset( &tBrdEthParamAll, 0, sizeof(TBrdEthParamAll) );
        s32 nRet = BrdGetEthParamAll( byEthChoice, &tBrdEthParamAll );
        if ( ERROR == nRet
            || 0 == tBrdEthParamAll.dwIpNum 
            || ( tBrdEthParamAll.dwIpNum > 0 && 0 == tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs )
            || ( tBrdEthParamAll.dwIpNum > 0 && 0 == tBrdEthParamAll.atBrdEthParam[0].dwIpMask ) )
        {
            IsSlaveSysLog( LOG_KEYSTATUS, "[InitLocalCfgInfo] The primary IpAddr or IpMask is not exist in board system! Please config...\n" );
            return FALSE;
        }
#endif
    }
    else
    {
#ifdef _LINUX12_
        // ���ô�ϵͳ��IS2.2 8313����һIP
        memset( &tEthParam, 0, sizeof(tEthParam) );
        tEthParam.dwIpAdrs = htonl( dwBrdSlaveSysIP );
        tEthParam.dwIpMask = htonl( dwBrdSlaveSysIPMask );        
        s32 nRet = BrdSetEthParam( byEthChoice, Brd_SET_IP_AND_MASK, &tEthParam );
        if ( ERROR == nRet )
        {
            IsSlaveSysLog( LOG_ERROR, "[InitLocalCfgInfoByCfgFile] Set primary IpAddr:%0x, IPMask:%0x in Ethernet Choice : %u failed!\n", dwBrdSlaveSysIP, dwBrdSlaveSysIPMask, byEthChoice );
            return FALSE;       
        }
        else
        {
            IsSlaveSysLog( LOG_DETAIL, "[InitLocalCfgInfoByCfgFile] Set primary IpAddr:%0x, IPMask:%0x in Ethernet Choice : %u success!\n", dwBrdSlaveSysIP, dwBrdSlaveSysIPMask, byEthChoice );
        }
#endif
    }

    // 6����ϵͳ��IS2.2 8313���ڲ�ͨ�� ip
    bResult = GetRegKeyString( achProfileName, SECTION_IpConfig, KEY_BrdSlaveSysInnerIp, 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )  
    {
        IsSlaveSysLog( LOG_ERROR, "[InitLocalCfgInfoByCfgFile] Wrong profile while reading %s!\n", KEY_BrdSlaveSysInnerIp );
        return FALSE;
    }
    dwBrdSlaveSysInnerIP =  ntohl( INET_ADDR( achReturn ) );
    if( 0xffffffff == dwBrdSlaveSysInnerIP ||  0 == dwBrdSlaveSysInnerIP )
    {
        IsSlaveSysLog( LOG_ERROR, "[InitLocalCfgInfoByCfgFile] The inner ip is 0 in config file!\n" );
        return FALSE;
    }
    // ���ô�ϵͳ��IS2.2 8313���ڲ�ͨ�� ip
#ifdef _LINUX12_
    /* ֱ�����second ip ���ܽ��: �����ͬ����ip�����ͻ����ʧ��
                                   �粻���ڣ������һ��second ip */ 

    /* Լ�� �ڲ�ͨ�� ip ��Ϊ second ip �ĵ�һ��ip */ 
    
    memset( &tBrdEthParamAll, 0, sizeof(TBrdEthParamAll) );
    s32 nRet = BrdGetEthParamSecIP( byEthChoice, &tBrdEthParamAll );
    // ������second ip
    if ( OK == nRet && 0 == tBrdEthParamAll.dwIpNum )
    {
        // ����ڲ�ͨ�� ip
        memset( &tEthParam, 0, sizeof(tEthParam) );
        tEthParam.dwIpAdrs = htonl( dwBrdSlaveSysInnerIP );
        tEthParam.dwIpMask = htonl( dwBrdSlaveSysIPMask ); 
        nRet = BrdSetEthParam( byEthChoice, Brd_SET_ETH_SEC_IP, &tEthParam );
        if ( ERROR == nRet )
        {
            IsSlaveSysLog( LOG_ERROR, "[InitLocalCfgInfoByCfgFile] Set inner Ip:%0x, IPMask:%0x in Ethernet Choice : %u failed!\n", dwBrdSlaveSysInnerIP, dwBrdSlaveSysIPMask, byEthChoice );
        }
        else
        {
            IsSlaveSysLog( LOG_DETAIL, "[InitLocalCfgInfoByCfgFile] Set inner Ip:%0x, IPMask:%0x in Ethernet Choice : %u success!\n", dwBrdSlaveSysInnerIP, dwBrdSlaveSysIPMask, byEthChoice );
        }
        return FALSE;
    }
    // ����second ip
    else if ( OK == nRet && tBrdEthParamAll.dwIpNum > 0 )
    {
        // <1> �����¼����ʱ����
        TBrdEthParamAll tTempBrdEthParamAll;
        memset( &tTempBrdEthParamAll, 0, sizeof(TBrdEthParamAll) );
        // �������õ��ڲ�ͨ�� ip�洢����ʱ�����0λ
        tTempBrdEthParamAll.atBrdEthParam[0].dwIpAdrs = htonl( dwBrdSlaveSysInnerIP );
        tTempBrdEthParamAll.atBrdEthParam[0].dwIpMask = htonl( dwBrdSlaveSysIPMask );
        tTempBrdEthParamAll.dwIpNum = 1;
        // ��˳�򽫵�1��Num - 1λ��ip�洢����ʱ����ĵ�1��Num - 1λ
        for ( u8 byLop = 1; byLop < tBrdEthParamAll.dwIpNum; byLop++ )
        {
            tTempBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs = tBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs;
            tTempBrdEthParamAll.atBrdEthParam[byLop].dwIpMask = tBrdEthParamAll.atBrdEthParam[byLop].dwIpMask;
            tTempBrdEthParamAll.dwIpNum ++;
        }
        // <2> ɾ��ԭ��second ip 
        for ( u8 byLop = 0; byLop < tBrdEthParamAll.dwIpNum; byLop++ )
        {
            memset( &tEthParam, 0, sizeof(tEthParam) );
            tEthParam.dwIpAdrs = tBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs;
            tEthParam.dwIpMask = tBrdEthParamAll.atBrdEthParam[byLop].dwIpMask; 
            nRet = BrdDelEthParamSecIP( byEthChoice, &tEthParam );
            if ( ERROR == nRet )
            {
                IsSlaveSysLog( LOG_ERROR, "[InitLocalCfgInfoByCfgFile] Set second Ip:%0x, IPMask:%0x in Ethernet Choice : %u failed!\n", dwBrdSlaveSysInnerIP, dwBrdSlaveSysIPMask, byEthChoice );
            }
            else
            {
                IsSlaveSysLog( LOG_DETAIL, "[InitLocalCfgInfoByCfgFile] Set second Ip:%0x, IPMask:%0x in Ethernet Choice : %u success!\n", dwBrdSlaveSysInnerIP, dwBrdSlaveSysIPMask, byEthChoice );
            }
        }
        // <3> ������ʱ����������¼��second ip
        for ( u8 byLop = 0; byLop < tTempBrdEthParamAll.dwIpNum; byLop++ )
        {
            memset( &tEthParam, 0, sizeof(tEthParam) );
            tEthParam.dwIpAdrs = tTempBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs;
            tEthParam.dwIpMask = tTempBrdEthParamAll.atBrdEthParam[byLop].dwIpMask; 
            nRet = BrdSetEthParam( byEthChoice, Brd_SET_ETH_SEC_IP, &tEthParam );
            if ( ERROR == nRet )
            {
                IsSlaveSysLog( LOG_ERROR, "[InitLocalCfgInfoByCfgFile] Set second Ip:%0x, IPMask:%0x in Ethernet Choice : %u failed!\n", dwBrdSlaveSysInnerIP, dwBrdSlaveSysIPMask, byEthChoice );
            }
            else
            {
                IsSlaveSysLog( LOG_DETAIL, "[InitLocalCfgInfoByCfgFile] Set second Ip:%0x, IPMask:%0x in Ethernet Choice : %u success!\n", dwBrdSlaveSysInnerIP, dwBrdSlaveSysIPMask, byEthChoice );
            }
        }
    }
#endif

    return TRUE;
}

/*====================================================================
    ������      : GetBrdMasterSysInnerIP
    ����        : ��ȡ��ϵͳ��IS2.2 8548�� second ip
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    ����ֵ˵��  : u32
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2012/05/05  4.7         liaokang      ����
====================================================================*/
u32 CBrdSlaveSysCfg::GetBrdMasterSysInnerIP()
{
    return m_dwBrdMasterSysInnerIP;
}

/*====================================================================
    ������      : GetBrdMasterSysListenPort
    ����        : ��ȡ��ϵͳ��IS2.2 8548�� Listen Port
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    ����ֵ˵��  : u16
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2012/05/05  4.7         liaokang      ����
====================================================================*/
u16 CBrdSlaveSysCfg::GetBrdMasterSysListenPort()
{
    return m_wBrdMasterSysListenPort;
}

/*====================================================================
������      : IsSlaveSysLog
����        : ��ӡʵ��
�㷨ʵ��    :
����ȫ�ֱ���:
�������˵��: u8 byPrintLvl    ��ӡ�ȼ�
              s8* pszFmt, ...  ��ʽ
����ֵ˵��  : void
----------------------------------------------------------------------
�޸ļ�¼    :
��  ��      �汾        �޸���        �޸�����
12/06/18    4.7         liaokang       ����
====================================================================*/
/*lint -save -e530 */
/*lint -save -esym(438, argptr)*/
/*lint -save -esym(526, __builtin_va_start)*/
/*lint -save -esym(628, __builtin_va_start)*/
void IsSlaveSysLog( u8 byPrintLvl, s8* pszFmt, ... )
{
    s8  achPrintBuf[1024] = { 0 };
    va_list argptr;
    s32 nBufLen = 0;
    nBufLen = sprintf( achPrintBuf, "[IsSlaveSys]" );
    va_start( argptr, pszFmt );
    vsnprintf( achPrintBuf + nBufLen, 1024-1, pszFmt, argptr );
    va_end(argptr);
    LogPrint( byPrintLvl, MID_PUB_ALWAYS, achPrintBuf);
    return;
}

// ��̬��ӡ
void staticlog( LPCSTR lpszFmt, ...)
{
    s8 achBuf[1024] = { 0 };
    va_list argptr;    
    va_start( argptr, lpszFmt );
    vsnprintf(achBuf, 1024-1, lpszFmt, argptr );
    OspPrintf( TRUE, FALSE, achBuf);
    va_end( argptr );
}
/*lint -restore*/

API void isslavesysver()
{
    s8 gs_VersionBuf[128] = {0};
    {
        strcpy(gs_VersionBuf, KDV_MCU_PREFIX);
        
        s8 achMon[16] = {0};
        unsigned int byDay = 0;
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
    staticlog( "isslavesys Version: %s\n", gs_VersionBuf );
    staticlog( "isslavesys: %s, compile time:%s:%s\n", VER_ISSLAVE, __DATE__, __TIME__ );
}

// ������Ϣ
API void isslavesyshelp()
{   
    isslavesysver();    
    staticlog( "  \tisslavesysver     ---->   show IS slave system ver info.\n");
    staticlog( "  \tisslavesysinit    ---->   Init IS slave system.\n");
    staticlog( "  \tquit              ---->   quit the program.\n");
    return;
}

/*=====================================================================
    ������      : BrdSlaveSysAPIEnableInLinux
    ����        : ע��LINUX�µ��Ժ���
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    ����ֵ˵��  : void
-----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2012/05/05  4.7         liaokang      ����
=====================================================================*/
void BrdSlaveSysAPIEnableInLinux(void)
{
#ifdef _LINUX12_
/*lint -save -e611 */
    OspRegCommand("isslavesyshelp",  (void*)isslavesyshelp,   "isslavesyshelp");
    OspRegCommand("isslavesysver",   (void*)isslavesysver,    "isslavesysver");
    OspRegCommand("isslavesysinit",  (void*)isslavesysinit,   "isslavesysinit");
    OspRegCommand("quit",            (void*)quit,             "quit");
/*lint -restore*/
#endif
}

/*=====================================================================
    ������      : StartCaptureException
    ����        : �����쳣����
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    ����ֵ˵��  : s32
-----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2012/05/05  4.7         liaokang      ����
=====================================================================*/
#ifdef _LINUX12_
s32 StartCaptureException(void)
{
    return init_nipdebug(0, NULL, DIR_EXCLOG);
}
#endif

/*====================================================================
    ������      : AddEventStr
    ����        : �����Ϣ�ַ���
    �㷨ʵ��    : 
    ����ȫ�ֱ���: ��
    �������˵��: 
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2012/05/05  4.7         liaokang      ����
====================================================================*/
void AddEventStr()
{
#ifdef OSPEVENT
#undef OSPEVENT
#endif
#define OSPEVENT( x, y ) OspAddEventDesc( #x, y )

#ifdef _EV_AGTSVC_H_
    #undef _EV_AGTSVC_H_
/*lint -save -e537 */
    #include "evagtsvc.h"
/*lint -restore*/
#else
    #include "evagtsvc.h"
#endif 
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
    2012/05/05  4.7         liaokang      ����
====================================================================*/
void CreateftpDir( void )
{
#ifdef WIN32
    CreateDirectory( ( LPCTSTR )DIR_FTP, NULL );
#else // linux
    mkdir( (s8*)DIR_FTP, 700 );
#endif
}

/*=====================================================================
    ������      : isslavesysinit
    ����        : ��ϵͳ��IS2.2 8313����ʼ��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    ����ֵ˵��  : API void 
-----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2012/05/05  4.7         liaokang      ����
=====================================================================*/
API void isslavesysinit(void)
{
	// ��ֹIS22Mpc8313������ʼ��
	static u16 wFlag = 0;
	if( wFlag != 0 )
	{
		IsSlaveSysLog( LOG_KEYSTATUS, "[isslavesysinit]Brd Already Inited!\n" );
		return;
	}
   	wFlag++;

    if( ERROR == BrdInit() )
    {
        IsSlaveSysLog( LOG_ERROR, "[isslavesysinit]BrdInit failed!\n" );
        return;
    }

	// OSP��ʼ��
    if ( !OspInit(TRUE, BRD_TELNET_PORT) )
    {
		IsSlaveSysLog( LOG_ERROR, "[isslavesysinit]OspInit failed!\n" );
		OspQuit();
        return;
    }
    
    OspSetPrompt("isslavesys");

    //Osp telnet ��ʼ��Ȩ
#ifndef WIN32
    OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );
#endif
    
    // ������������Ϣ
    if( !g_cBrdSlaveSysApp.ReadConnectConfigInfo() )
    {
        IsSlaveSysLog( LOG_ERROR, "[isslavesysinit] Read brd cfg failed!\n" );
        return ;
    }
    // ��ʼ������������Ϣ
    g_cBrdSlaveSysApp.InitLocalCfgInfoByCfgFile();

    //����ftpĿ¼
	CreateftpDir( );

    // �����߳�
    g_cBrdSlaveSysApp.CreateApp( "BrdSlaveSys", AID_MCU_BRDGUARD, APPPRI_BRDGUARD);
    OspPost( MAKEIID( AID_MCU_BRDGUARD, 1 ), OSP_POWERON );

    OspSetLogLevel( AID_MCU_BRDGUARD, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDGUARD, 0, 0);

	return;
}

// ȫ���ź���
static SEMHANDLE g_tSemHandle;

// �˳�
API void quit(void) 
{
    // �˳�ǰ��flush
    OspDelay(20);
    logflush();
    OspDelay(20);

    // �˳�OSP
    OspQuit();
    // �ͷ�ȫ���ź���
    OspSemGive( g_tSemHandle );
}

// linuxϵͳ
#ifdef _LINUX12_
// main����
int main()
{
    // KdvLogģ���ʼ��
    Err_t err = LogInit( BRDSLAVESYS_KDVLOG_FILE );
    if (LOG_ERR_NO_ERR != err)
    {
        printf("[main]LogInit() failed! ERRCODE = [%d]\n",err);
	}

	AddEventStr();

    s32 nReturn = 0;

    //���ý����쳣����ص�
    nReturn = StartCaptureException();
    if ( nReturn != 0 )
    {
        staticlog( "[main]StartCaptureException() failed! ERRCODE = [%d]\n",nReturn);
    }
    else
    {
        staticlog( "[main]StartCaptureException() Successed!\n");
    }

	// ע��LINUX�µ��Ժ���
    BrdSlaveSysAPIEnableInLinux();

	isslavesysinit();


    s8 byRet = BrdSetSysRunSuccess();
    switch (byRet)
    {
    case NO_UPDATE_OPERATION:
        printf("[BrdSetSysRunSuccess]start mcu normally\n");
        staticlog( "[BrdSetSysRunSuccess]start mcu normally!\n");
        break;
    case UPDATE_VERSION_ROLLBACK:
        printf("[BrdSetSysRunSuccess]fail to update, rollback version\n");
        staticlog( "[BrdSetSysRunSuccess]fail to update, rollback version!\n");
        break;
    case UPDATE_VERSION_SUCCESS:
        printf("[BrdSetSysRunSuccess]succeed to update\n");
        staticlog( "[BrdSetSysRunSuccess]succeed to update!\n");
        break;		
    case SET_UPDATE_FLAG_FAILED:
        printf("[BrdSetSysRunSuccess]fail to set run flag\n");
        staticlog( "[BrdSetSysRunSuccess]fail to set run flag!\n");
        break;
    default:
        printf("[BrdSetSysRunSuccess]bad callback param\n");
        staticlog( "[BrdSetSysRunSuccess]bad callback param!\n");
        break;
	}

    // �����ź���
    if ( !OspSemBCreate(&g_tSemHandle) )
    {
        printf( "[main] OspSemBCreate failed\n" );
        return 0;
    }
    // Take���ζ�Ԫ�ź���������    
    if ( !OspSemTake( g_tSemHandle ) )
    {
        printf( "[main] OspSemTake 1 failed\n" );
        return 0;
    }
    if ( !OspSemTake( g_tSemHandle ) )
    {
        printf( "[main] OspSemTake 2 failed\n" );
        return 0;
    }

    printf("[main]Quit normally!\n");
    staticlog( "[main]Quit normally!\n");
	return 0;
}
#endif


#if defined( WIN32 )
void main(void)
{
	// KdvLogģ���ʼ��
    Err_t err = LogInit( BRDSLAVESYS_KDVLOG_FILE );
    if (LOG_ERR_NO_ERR != err)
    {
        printf("[main]LogInit() failed! ERRCODE = [%d]\n",err);
	}	
	AddEventStr();

	isslavesysinit();

    // �����ź���
    if ( !OspSemBCreate(&g_tSemHandle) )
    {
        printf( "[main] OspSemBCreate failed\n" );
        return;
    }
    // Take���ζ�Ԫ�ź���������    
    if ( !OspSemTake( g_tSemHandle ) )
    {
        printf( "[main] OspSemTake 1 failed\n" );
        return;
    }
    if ( !OspSemTake( g_tSemHandle ) )
    {
        printf( "[main] OspSemTake 2 failed\n" );
        return;
    }
    
    printf("[main]Quit normally!\n");
    staticlog( "[main]Quit normally!\n");
	return;
}
#endif