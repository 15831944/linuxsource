/*****************************************************************************
   ģ����      : mpu2lib
   �ļ���      : mpu2vmphwtest.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: mpu2lib vmp��������ҵ��ʵ��
   ����        : ������
   �汾        : V4.7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��       �汾        �޸���      �޸�����
   2011/12/07    4.7         ������      ����
******************************************************************************/

#include "mpuautotest.h"
#include "mpu2vmphwtest.h"
#include "mcuver.h"
#include "boardagentbasic.h"
#include "evmpu2.h"

/*lint -save -esym(1512, CUserFullInfo)*/
/*lint -save -esym(1512, CExUsrInfo)*/
CMpu2VmpTestClientApp g_cMpuVmpTestApp;

#ifdef _8KI_
#define _8KI_LEFTCARD      (u8)0
#define _8KI_RIGHTCARD     (u8)1
#endif

/*====================================================================
	����  : CMpu2VmpTestClient
	����  : �������Թ��캯��
	����  : 
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
CMpu2VmpTestClient::CMpu2VmpTestClient()
{
    m_dwMpuTestDstNode = INVALID_NODE;
    m_dwMpuTestDstInst = INVALID_INS;
	m_by8KICurTestCard = 0xFF;
	//TODO:��ʵ��mpu2���ӹ�������ϸ�޸ģ��ȶ���ƵĻ������
	//mpu2������define������5���ƣ���������ȷ��ֻ����������Ҫ�㣬link��alarm
// 	u8 byLedRun; 
//     u8 byLedLINK;
//     u8 byLedALM;
//     u8 byLedLINK0;	
//     u8 byLedLINK1;
	u8 abyLedId[MPU2_LED_NUM] = { LED_SYS_ALARM,LED_SYS_LINK };//Ӧ�ø���ʵ�ʶ���
	memcpy(m_abyLedId,abyLedId,MPU2_LED_NUM);
}




/*====================================================================
	����  : Clear
	����  : ��յ�ǰinstance��״̬
	����  : 
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
	11/12/1		v4.7		zhouyiliang		create
====================================================================*/
void CMpu2VmpTestClient::Clear()
{
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
    for (; byLoop < MAXNUM_MPU2VMP_CHANNEL; byLoop++)
    {
		if ( NULL !=  m_pcMediaSnd[byLoop] ) 
		{
			delete  m_pcMediaSnd[byLoop];
			m_pcMediaSnd[byLoop] = NULL;
		}
        
    }
    byLoop = 0;
    for ( ; byLoop < MAXNUM_MPU2VMP_MEMBER; byLoop++ )
    {
        m_bAddVmpChannel[byLoop] = FALSE;
    }
	m_cHardMulPic.Destory();
	


       
}

/*====================================================================
	����  : Init
	����  : ��ʼ������
	����  : 
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::Init( CMessage *const pMsg )
{
	//1���������� 
    if ( NULL == pMsg )
    {
        ErrorLog( "[CMpu2VmpTestClient::Init] Init params cannot be NULL!\n" );
        return;
    }
    KeyLog("Mpu2Test Initiate!!!\n");
    //2��������Ϣ��TEqpBasicCfg��ֱ�ӱ��浽m_tCfg
	memcpy( &m_tCfg, pMsg->content, sizeof(TEqpBasicCfg) );
	m_tCfg.wRcvStartPort = 40700;//д��40700
	KeyLog("[CMpu2VmpTestClient Init] m_tCfg.wRcvStartPort is: %d\n", m_tCfg.wRcvStartPort);

	//3��ý�ز��ʼ��
	TMultiPicCreate tMPCreate;

	tMPCreate.m_dwDevVersion = (u32)en_MPU2_Board;


	tMPCreate.m_dwMode = VMP_BASIC;
	if ( g_cMpu2VmpApp.m_byWorkMode == TYPE_MPU2VMP_ENHANCED)
	{
		tMPCreate.m_dwMode = VMP_ENHANCED;
	}
	
    s32 nRet = m_cHardMulPic.Create( tMPCreate );

    if ( HARD_MULPIC_OK == nRet )
    {
        KeyLog( "[Init] Create HardMulPic Success!\n" );
		printf("[Init] Create HardMulPic Success!\n");
    }
    else
    {
        ErrorLog( "[Init] Fail to Create. errcode.%d\n", nRet );
		printf("[Init] Fail to Create. errcode.%d\n", nRet );
        return;
    }


	//4��������ʼ��
    u16 wRet = KdvSocketStartup();
    
    if ( MEDIANET_NO_ERROR != wRet )
    {
        ErrorLog( "[Init] KdvSocketStartup failed, error: %d\n", wRet );
        return ;
    }
	
	KeyLog("KdvSocketStartup ok!!!\n");

	//5��Init���ն���
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

	//6��new���Ͷ���
	byLoop =0;
	for (;byLoop < MAXNUM_MPU2VMP_CHANNEL;byLoop++)
	{

        m_pcMediaSnd[byLoop] = new(CKdvMediaSnd); 
        if ( NULL == m_pcMediaSnd[byLoop] )
        {
			Clear();
            ErrorLog( " new(CKdvMediaSnd) fail!\n" );
            return;
        }
    
	}
	//7����ʼ�����Խӷ�����
	ConnectServ();
	

		
    return;
}
/*====================================================================
	����  : ConnectServ
	����  : ���ӷ���������
	����  : 
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
BOOL32 CMpu2VmpTestClient::ConnectServ()
{
	KillTimer(EV_C_CONNECT_TIMER);

	KeyLog("Begin to connect Test Server!\n");

	//1�����Ӳ��Է�����10.1.1.1 @60000
	u32 dwServerIpAddr = inet_addr(g_cMpuVmpTestApp.m_achServerIp);
	m_dwMpuTestDstNode = OspConnectTcpNode(dwServerIpAddr, 60000);
	//2�����Ӳ��Է�����ʧ�ܣ������ӵ�timer
	if( !OspIsValidTcpNode(m_dwMpuTestDstNode) )
	{
		ErrorLog( "[ConnectServ] connect to Server %s failed!\n",g_cMpuVmpTestApp.m_achServerIp);
		SetTimer( EV_C_CONNECT_TIMER, MPU_CONNETC_TIMEOUT );
		return FALSE;
	}
	//3�����Ӳ��Է������ɹ���ע�������Ϣ����ע���timer
	m_dwServerRcvIp = dwServerIpAddr; 
	KeyLog("Connect Server(%d) ok, DstNode is %d!\n", m_dwServerRcvIp, m_dwMpuTestDstNode);
	::OspNodeDiscCBRegQ( m_dwMpuTestDstNode, GetAppID(), GetInsID() );
	SetTimer( EV_C_REGISTER_TIMER, MPU_REGISTER_TIMEOUT);
	return TRUE;
}
/*====================================================================
	����  : RegisterSrv
	����  : ��������ע�ắ��
	����  : 
	���  : ��
	����  : ��
	ע    :
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::RegisterSrv(void)
{

	KillTimer(EV_C_REGISTER_TIMER);

	KeyLog("Enter RegisterSrv!\n");
	//���͵�½��Ϣ: ev_MPUTest_REQ ��Ϣ�壺
	//	u8 ���Ե�ģʽ��VMP/BAS�� + 
	//	u8 *31  ��
	//	+ �û���Ϣ + �豸��Ϣ
    u8 abyTemp[sizeof(u8)*32 + sizeof(CLoginRequest) + sizeof(CDeviceInfo)];
    memset(abyTemp, 0, sizeof(abyTemp));

	abyTemp[0] = g_cMpuVmpTestApp.m_byWorkMode;
	

#ifdef _8KI_ 
	abyTemp[1] = MCU_TYPE_KDV8000I;
#endif
    CLoginRequest cLoginReq;
    cLoginReq.Empty();
    cLoginReq.SetName("admin");
    cLoginReq.SetPassword("admin");
    memcpy(abyTemp + sizeof(u8)*32, &cLoginReq, sizeof(CLoginRequest));
    
    CDeviceInfo cDeviceinfo;
	memset( &cDeviceinfo, 0x0, sizeof( cDeviceinfo ) );
#ifdef _LINUX_
    SetMpuDeviceInfo(cDeviceinfo);
#endif
    memcpy(abyTemp + sizeof(u8)*32 + sizeof(CLoginRequest), &cDeviceinfo, sizeof(CDeviceInfo));


    post( MAKEIID( MPU_SERVER_APPID, 1 ), 
		C_S_MPULOGIN_REQ, 
		abyTemp, 
        sizeof(u8) * 32  + sizeof(CLoginRequest) + sizeof(CDeviceInfo),
        m_dwMpuTestDstNode
		);
	SetTimer( EV_C_REGISTER_TIMER, MPU_REGISTER_TIMEOUT);
}

#ifdef _LINUX_
/*====================================================================
  �� �� ���� SetMpuDeviceInfo
  ��    �ܣ� �����豸��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CDeviceInfo &cDeviceInfo
  �� �� ֵ�� 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::SetMpuDeviceInfo(CDeviceInfo &cDeviceInfo)
{


	s8* pSoftVer = (s8*)VER_MPU2;
	cDeviceInfo.setSoftVersion( pSoftVer );

    //MAC, IP, SubMask
	s8 achMac[VERSION_LEN];
	memset( achMac, 0x0, sizeof(achMac) );
	u8 byEthId = 0;
	u8 byState = 0;
	//�ж���ǰ(��/0)���ڻ��Ǻ�(��/1)���ڣ�Ĭ����������
	for(; byEthId < 2; byEthId ++)
	{
		if( ERROR == BrdGetEthLinkStat(byEthId, &byState) )
		{
			ErrorLog("[SetMpuDeviceInfo] Call BrdGetEthLinkStat(%u) ERROR!\n", byEthId);
			break;
		}
		if( byState == 1 )
		{
			break;
		}
	}
	
	TBrdEthParam tBrdEthParam;
	memset( &tBrdEthParam, 0x0, sizeof(tBrdEthParam) );
    BrdGetEthParam( byEthId, &tBrdEthParam );
    
	u32 dwIp = tBrdEthParam.dwIpAdrs;
    u32 dwMask = tBrdEthParam.dwIpMask; 
	sprintf(achMac,
        "%02X-%02X-%02X-%02X-%02X-%02X",
        tBrdEthParam.byMacAdrs[0],
        tBrdEthParam.byMacAdrs[1],
        tBrdEthParam.byMacAdrs[2],
        tBrdEthParam.byMacAdrs[3],
        tBrdEthParam.byMacAdrs[4],
        tBrdEthParam.byMacAdrs[5]
        ); 
	cDeviceInfo.setMac( achMac );
    cDeviceInfo.setIp( dwIp );
    cDeviceInfo.setSubMask( dwMask ); 
	
	// hw version, serialNo
	TBrdE2PromInfo tBrdE2PromInfo;
	s8 achSerial[12] = { 0 };
	s8 achHWversion[VERSION_LEN];
	
	memset( achHWversion, 0x0, sizeof(achHWversion) );
	memset( &tBrdE2PromInfo, 0x0, sizeof(tBrdE2PromInfo) );
    BrdGetE2PromInfo( &tBrdE2PromInfo );
	
    sprintf( achHWversion, "%d", tBrdE2PromInfo.dwHardwareVersion );
    memcpy( achSerial, tBrdE2PromInfo.chDeviceSerial, sizeof(tBrdE2PromInfo.chDeviceSerial) );
	cDeviceInfo.setHardVersion( achHWversion );
    cDeviceInfo.setSerial( achSerial ); 
	

	return;
}
#endif
/*====================================================================
  �� �� ���� ProcRegAck
  ��    �ܣ� ע��ɹ�������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage
  �� �� ֵ�� 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::ProcRegAck(CMessage* const pMsg)
{
	KillTimer(EV_C_REGISTER_TIMER);
	m_dwMpuTestDstInst = pMsg->srcid;

#ifdef _8KI_ //8ki��mpu2vmp�����������̸ı䣬��½�ɹ���Ҫ�ȷ�������S_C_STARTVMPTEST_REQ�ٳ�ʼ��ý�أ���ʼ�շ�
	return;
#endif
	PrepareToTest();

}

/*====================================================================
  �� �� ���� PrepareToTest
  ��    �ܣ� Ϊ��ʼ������׼������ý���²Σ�׼���ÿ�ʼ�շ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage
  �� �� ֵ�� 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   2012/04/19		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::PrepareToTest( )
{
	//1��--------------׼������/�������-------------------
	
	m_wServerRcvStartPort = 39000;			//�����������Ŀ�Ķ˿�
	//9·��	1080p60HP,1080p60BP,1080p30HP,1080p30BP,720p60HP,720p60BP,4cif 25,cif 25,other 25
// 	u16 wVideoWidth[]		= { 1920,1920,1920,1920, 1280,1280, 704,352, 352 };
// 	u16 wVideoHeight[]		= { 1080,1080,1080,1080,  720, 720, 576,288, 288 };
// 	u8  abyFrameRate[]      = {   60,  60,  30,  30,   60,  60,  25, 25, 25  };
// 	u8  abyProfile[]        = {   1,     0,  1,   0,    1,   0,   0,  0,  0               };
// 	u16 wMaxKeyFrameInterval[]	= {3000,3000,3000,3000,3000, 3000, 1000, 500 ,500};
	
    /*lint -save -e661*/
	//u8 byOutLoop = 0;
	//���ݹ��ֻ���һ·1080p30bp������������ֻ����һ·�����Ϳ�����
	// 	for (;byOutLoop < MAXNUM_MPU2VMP_CHANNEL; byOutLoop ++)
	// 	{
	// 		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_wVideoHeight = wVideoHeight[byOutLoop]; 
	// 		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_wVideoWidth  = wVideoWidth[byOutLoop];
	// 		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_byEncType    = (byOutLoop < 8)?MEDIA_TYPE_H264:MEDIA_TYPE_MP4;
	// 		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_wBitRate     = 2000;
	// 		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_byMaxKeyFrameInterval = wMaxKeyFrameInterval[byOutLoop];
	// 		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_byMaxQuant	= 50;
	// 		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_byMinQuant	= 15;
	// 		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_byFrameRate  = abyFrameRate[byOutLoop];
	// 		m_tMpuVmpCfg.m_adwMaxSendBand[byOutLoop] = (u32)( m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_wBitRate * 2 + 100 );
	// 		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_byProfile = abyProfile[byOutLoop];
	// 		
	// 		KeyLog( "Follows are the params we used to start the vmp:\n\n" );
	// 		KeyLog( "/*----------  video channel.%d  -----------*/\n", byOutLoop );
	// 		m_tMpuVmpCfg.PrintEncParam( byOutLoop );
	// 		KeyLog( "/*----------------------------------------*/\n\n" );
	// 
	// 	}
	
	m_tMpuVmpCfg.m_tVideoEncParam[0].m_wVideoHeight = 1080;//wVideoHeight[3]; 
	m_tMpuVmpCfg.m_tVideoEncParam[0].m_wVideoWidth  = 1920;//wVideoWidth[3];
	m_tMpuVmpCfg.m_tVideoEncParam[0].m_byEncType    = MEDIA_TYPE_H264;
	m_tMpuVmpCfg.m_tVideoEncParam[0].m_wBitRate     = 3072;
	m_tMpuVmpCfg.m_tVideoEncParam[0].m_dwMaxKeyFrameInterval = 3000;//wMaxKeyFrameInterval[3];
	m_tMpuVmpCfg.m_tVideoEncParam[0].m_byMaxQuant	= 50;
	m_tMpuVmpCfg.m_tVideoEncParam[0].m_byMinQuant	= 15;
	m_tMpuVmpCfg.m_tVideoEncParam[0].m_byFrameRate  = 30;//abyFrameRate[3];
	m_tMpuVmpCfg.m_adwMaxSendBand[0] = (u32)( 3072 * 2 + 100 );
	m_tMpuVmpCfg.m_tVideoEncParam[0].m_dwProfile = 0;
	
	KeyLog( "Follows are the params we used to start the vmp:\n\n" );
	KeyLog( "/*----------  video channel.%d  -----------*/\n", 0 );
	m_tMpuVmpCfg.PrintEncParam( 0 );
	KeyLog( "/*----------------------------------------*/\n\n" );
	
	/*lint -restore*/
	//2�����÷��
	//	m_tParam[0].m_byVMPStyle = VMP_STYLE_TWENTY;
	m_byHardStyle = ConvertVcStyle2HardStyle( VMP_STYLE_TWENTYFIVE );
	m_cHardMulPic.SetMulPicType(m_byHardStyle);
	
	
	
    s32 nRet = 0;
	// 3����Ӻϳɳ�Ա
	for (u8 byChnNo = 0; byChnNo < MAXNUM_MPU2VMP_MEMBER; byChnNo++ )
	{
        nRet = m_cHardMulPic.AddChannel( byChnNo ); 
        if ( HARD_MULPIC_OK != nRet )
        {
            ErrorLog( "[MsgStartVidMixProc] add Chn.%d failed, ret=%u!\n", byChnNo, nRet);
        }
        else
        {
            m_bAddVmpChannel[byChnNo] = TRUE;
        }
        KeyLog( "Add channel-%u return %u(%u is correct)!\n", byChnNo, nRet, HARD_MULPIC_OK );
		
	}
	//4��StartMerge
	nRet = m_cHardMulPic.SetVideoEncParam(&m_tMpuVmpCfg.m_tVideoEncParam[0], 
										1);
	nRet = m_cHardMulPic.StartMerge( );
	if ( HARD_MULPIC_OK/*HARD_MULPIC_ERROR*/ != nRet )
	{
		ErrorLog( " startmerge correct, Error code: %u\n", nRet );
		Clear();
		return;
	}
	else
	{
		KeyLog( "StartMerge succeed!\n" );
//		printf("StartMerge Succeed!\n");
	}
	//5����ʼ�շ�
	StartNetRcvSnd() ;
	
	
	//6��֪ͨ���Է�����mpu2׼������
	post( MAKEIID( MPU_SERVER_APPID, 1 ), 
		C_S_MPUREADY_NOTIF, 
		NULL, 0, m_dwMpuTestDstNode
		);
}

/*====================================================================
  �� �� ���� ProDisconnect
  ��    �ܣ� ����������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage
  �� �� ֵ�� 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::ProDisconnect(CMessage* const pMsg)
{
	
	if ( NULL == pMsg )  
    {
        ErrorLog( "[ProDisconnect] message's pointer is Null\n" );
        return;
    }

	u32 dwNode = *(u32*)pMsg->content;

	if( dwNode != m_dwMpuTestDstNode)
	{
		ErrorLog( "[ProDisconnect] the Node.%d isn't equal to m_dwMpuTestDstNode.%d\n",dwNode, m_dwMpuTestDstNode );
		return;
	}

	m_dwMpuTestDstInst = INVALID_INS;	
    
    if(INVALID_NODE == m_dwMpuTestDstNode)
	{
		ErrorLog( "[CMpu2VmpTestClient ProDisconnect]: m_dwHduAutoTestDstNode is invalid!\n" );
        return;   
	}
	//1���Ͽ��ڵ�
    OspDisconnectTcpNode(m_dwMpuTestDstNode);
	
    m_dwMpuTestDstNode = INVALID_NODE;
	//2��stopmerge
	s32 nRet = m_cHardMulPic.StopMerge();
	if (HARD_MULPIC_OK != nRet )
	{
		ErrorLog("[CMpu2VmpTestClient ProDisconnect] stopmerge return failed!\n");
	}

	m_by8KICurTestCard = 0xFF;
	
	memset( m_bAddVmpChannel, 0, sizeof(m_bAddVmpChannel) );
	
	//3������
	SetTimer( EV_C_CONNECT_TIMER, MPU_CONNETC_TIMEOUT );

    return;
}

/*====================================================================
  �� �� ���� ChangeTestMode
  ��    �ܣ� �ı����ģʽ������mpu2ֻ��vmp��Ӧ��û�����ˣ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� byMode��Ҫ���ģʽ
  �� �� ֵ�� 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::ChangeTestMode(u8 byMode)
{
	if( byMode != TYPE_MPUSVMP && byMode != TYPE_MPUBAS)
	{
		ErrorLog("[ChangeTestMode] Unexpected Mode.%d!\n", byMode);
		return;
	}
	KeyLog("Change to Test Mode: %u\n wait for rebooting...\n", byMode);
	
	//תģʽ����
	s8   achProfileName[64] = {0};
    memset((void*)achProfileName, 0x0, sizeof(achProfileName));
    sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
	
	BOOL32 bRet;
    s32  sdwTest = byMode;	
    bRet = SetRegKeyInt( achProfileName, SECTION_BoardDebug, KEY_TestMode, sdwTest );
    if( !bRet )  
	{
		ErrorLog( "[RestoreDefault] Wrong profile while reading %s%s!\n", SECTION_BoardDebug, KEY_TestMode );
		return;
	}
	//reset
#ifndef WIN32
	OspDelay(2000);
	BrdHwReset();
#endif
    return;

}

/*====================================================================
  �� �� ���� RestoreDefault
  ��    �ܣ� ������Ա�־
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::RestoreDefault(void)
{
#ifndef WIN32


	BOOL32 bRet = FALSE;
    bRet = BrdClearE2promTestFlag();	//����Ա�־λ
	u8 bySucessRestore = bRet?0:1;
	s32 sdwRet;
    sdwRet = post(m_dwMpuTestDstInst, C_S_MPURESTORE_NOTIF, &bySucessRestore, sizeof(u8), m_dwMpuTestDstNode);
    if ( sdwRet != OSP_OK )
    {
        ErrorLog( "[RestoreDefault] post failed!\n" );
    }
	else
	{
		KeyLog( "[RestoreDefault] post sucessful!\n" );
	}
	//reset
	OspDelay(2000);
	BrdHwReset();
#endif
    return;
}
/*lint -save -e715*/
/*====================================================================
  �� �� ���� DaemonInstanceEntry
  ��    �ܣ� Daemon��Ϣ�ַ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage����Ϣ�壬CApp:ģ��
  �� �� ֵ�� 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp )
{
	if (NULL == pMsg || NULL == pcApp)
	{
		ErrorLog("[DaemonInstanceEntry]wrong message recived. pcMsg is Null or pApp is Null\n ");
		return;
	}
	switch (pMsg->event)
	{
	case EV_C_CHANGEAUTOTEST_CMD:
		DaemonProcChangeAutoTestCmd( pMsg );
		break;

	default:
		ErrorLog("DeamonInstanceEntry: wrong message type %u!\n", pMsg->event );
		break;
	}
}
/*lint -restore*/

/*====================================================================
  �� �� ���� InstanceEntry
  ��    �ܣ� ��ͨʵ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::InstanceEntry( CMessage* const pMsg )
{
	if ( NULL == pMsg )
    {
		ErrorLog("[InstanceEntry]wrong message recived. pMsg is Null \n");
        return;
    }
	KeyLog("enter InstanceEntry! message type %d\n",pMsg->event );
    switch ( pMsg->event )
    {
		//��ʼ����Ϣ
		case EV_C_INIT:
			Init( pMsg );		
			break;
		//����timer
		case EV_C_CONNECT_TIMER:
			ConnectServ();
			break;
		//ע��timer
		case EV_C_REGISTER_TIMER:
			RegisterSrv();
			break;
		//���Է�����ע��Ack
		case S_C_MPULOGIN_ACK:
			ProcRegAck(pMsg);		
			break;
		//���Է�����ע��NACK
		case S_C_MPULOGIN_NACK:
			break;
		//����������Ա�־λ
		case S_C_MPURESTORE_CMD:
			RestoreDefault();
			break;
		//����������ؼ�֡
		case S_C_FASTUPDATEPIC_CMD:
			MsgFastUpdatePicProc(pMsg);
			break;
		//OSP����
		case OSP_DISCONNECT:
			ProDisconnect(pMsg);
			break;
		//��Ʋ���
		case S_C_TEST_OUS_CMD:
			ProcTestLedCmd();
			break;
		//���timer
		case TIMER_TEST_LED:
			TimerTestLed(pMsg);
		break;

		case  S_C_STARTVMPTEST_REQ:
			Proc8KIStartVmpTestReq(pMsg);
			break;

		
			
		default:
			ErrorLog("wrong message type %u!\n", pMsg->event );
			break;

	}

    return;
}

/*====================================================================
  �� �� ���� DaemonProcChangeAutoTestCmd
  ��    �ܣ� �����������Ա�־
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage
  �� �� ֵ�� void
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::DaemonProcChangeAutoTestCmd( CMessage* const pMsg )
{
	if ( NULL == pMsg)
	{
		ErrorLog( "[DaemonProcChangeAutoTestCmd] pMsg is Null!\n" );
		return;
	}
#ifdef _LINUX_
	

	s32 sdwAutoTest = *(s32*)pMsg->content;
	if( sdwAutoTest != 0)
	{
		BrdSetE2promTestFlag();
		KeyLog( "[DaemonProcChangeAutoTestCmd] Set e2prom test flag 1\n" );
	}
	else 
	{
		BrdClearE2promTestFlag();
		KeyLog( "[DaemonProcChangeAutoTestCmd] Set e2prom test flag 0\n" );
	}
#endif
	
	// 	BrdHwReset();
}
/*====================================================================
  �� �� ���� InitMediaRcv
  ��    �ܣ� ��ʼ�����ն���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� byChnNo:���ն���ͨ����
  �� �� ֵ�� TRUE����ʼ���ɹ���FALSE����ʼ��ʧ��
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
BOOL32 CMpu2VmpTestClient::InitMediaRcv(u8 byChnNo )
{
	//mpu2ֻ��һ��instance����basic*2ģʽ��
    g_cMpu2VmpApp.m_atMpu2VmpRcvCB[0][byChnNo].m_byChnnlId = byChnNo;
    g_cMpu2VmpApp.m_atMpu2VmpRcvCB[0][byChnNo].m_pHardMulPic  = &m_cHardMulPic;

    s32 wRet = m_pcMediaRcv[byChnNo]->Create( MAX_VIDEO_FRAME_SIZE,
			                                  CBMpuSVmpRecvFrame,   
				    		                  (u32)&g_cMpu2VmpApp.m_atMpu2VmpRcvCB[0][byChnNo] );

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
/*====================================================================
  �� �� ���� StartNetRcvSnd
  ��    �ܣ� ��ʼ���ͺͽ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::StartNetRcvSnd()
{
   
	//1. ���뷢�����
    u8 byChnNo = 0;
	//u8 byActPt = 0;

	// media encrypt modify
//	TMediaEncrypt tMediaEncrypt;
// 	s32 nKeyLen = 0;
// 	u8  abyKeyBuf[MAXLEN_KEY] = {0};
	
//     for ( byChnNo = 0; byChnNo < MAXNUM_MPU2VMP_CHANNEL; byChnNo++ )
//     {
//         InitMediaSnd( byChnNo );
//         m_cHardMulPic.SetVidDataCallback( byChnNo, VMPCALLBACK, m_pcMediaSnd[byChnNo] );
//     }
	//��������ֻ��һ·�Ϳ�����
	InitMediaSnd( 0 );
    m_cHardMulPic.SetVidDataCallback( byChnNo, VMPCALLBACK, m_pcMediaSnd[0] );

    //2. ����������
	for (byChnNo = 0; byChnNo < MAXNUM_MPU2VMP_MEMBER; byChnNo ++)
	{
        //MediaRcv ���������������
		//���ö�̬�غ�,����
		m_pcMediaRcv[byChnNo]->SetDecryptKey( NULL, 0, 0 );	
		//2009-7-22 Ϊ��ֹ������̬�غ��л�����meidanet���ң�ͳһ�����غ�Ϊ MEDIA_TYPE_H264
        u16 wRet = m_pcMediaRcv[byChnNo]->SetActivePT( MEDIA_TYPE_H264, MEDIA_TYPE_H264 );
		KeyLog( "Set key to NULL, PT(real): %u, SetActivePt result is:%d \n", MEDIA_TYPE_H264, wRet );
		SetMediaRcvNetParam( byChnNo);

		  if (NULL != m_pcMediaRcv[byChnNo]) 
		  {
			m_pcMediaRcv[byChnNo]->StartRcv();
		  }
	  
	}


}


/*=============================================================================
  �� �� ���� InitMediaSnd
  ��    �ܣ� ��ʼ�����Ͷ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byChnNo        ��ͨ����
  �� �� ֵ�� void 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::InitMediaSnd( u8 byChnNo )
{
    u8 byFrameRate = m_tMpuVmpCfg.m_tVideoEncParam[byChnNo].m_byFrameRate;
    u8 byMediaType = m_tMpuVmpCfg.m_tVideoEncParam[byChnNo].m_byEncType;
    u32 dwNetBand  = m_tMpuVmpCfg.m_adwMaxSendBand[byChnNo];


	//1��������緢�ʹ������
    if ( dwNetBand * 1024 > 8000000 )
    {
        if ( m_tMpuVmpCfg.m_tVideoEncParam[byChnNo].m_wBitRate <= 8128 )
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
	if ( 7 == byChnNo || 8 == byChnNo ) 
	{
		bHDFlag = FALSE;
	}
	//4.����payload
	m_pcMediaSnd[byChnNo]->SetActivePT(MEDIA_TYPE_H264);
	KeyLog("[InitMediaSnd] m_cMediaSnd[%d].SetActivePt:%d",byChnNo,MEDIA_TYPE_H264);
	m_pcMediaSnd[byChnNo]->SetHDFlag( bHDFlag );
	KeyLog( "[InitMediaSnd] m_cMediaSnd[%d].SetHDFlag: %d\n", byChnNo, bHDFlag );

	//4�����÷���ͨ����netparam
    SetMediaSndNetParam(byChnNo);

    return;
}

/*=============================================================================
  �� �� ���� SetMediaSndNetParam
  ��    �ܣ� ���÷��Ͷ�����������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byChnNo        ��ͨ����
  �� �� ֵ�� TRUE:���óɹ���FALSE������ʧ�� 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
BOOL32 CMpu2VmpTestClient::SetMediaSndNetParam(u8 byChnNo)
{
	 TNetSndParam tNetSndPar;
    memset( &tNetSndPar, 0x00, sizeof(tNetSndPar) );
	
    tNetSndPar.m_byNum  = 1;           
    tNetSndPar.m_tLocalNet.m_wRTPPort       = PORT_SNDBIND +  byChnNo * 2; 
    tNetSndPar.m_tLocalNet.m_wRTCPPort      = m_wServerRcvStartPort + byChnNo * PORTSPAN + 1;/*PORT_SNDBIND + byChnNo * 2 + 1*/
    tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr  = m_dwServerRcvIp; //�Ѿ����������ˣ�������ת
    tNetSndPar.m_tRemoteNet[0].m_wRTPPort   = m_wServerRcvStartPort + byChnNo * PORTSPAN;
    tNetSndPar.m_tRemoteNet[0].m_dwRTCPAddr = m_dwServerRcvIp;//�Ѿ����������ˣ�������ת
    tNetSndPar.m_tRemoteNet[0].m_wRTCPPort  = m_wServerRcvStartPort + byChnNo * PORTSPAN + 1;

    KeyLog( "[InitMediaSnd] Snd[%d]: RTPAddr(%s)\n", byChnNo, StrOfIP(m_dwServerRcvIp));

    u16 wRet = m_pcMediaSnd[byChnNo]->SetNetSndParam( tNetSndPar );
    if ( MEDIANET_NO_ERROR != wRet )
	{
    	ErrorLog( "[SetMediaSndNetParam] m_cMediaSnd[%d].SetNetSndParam fail, Error code is: %d \n", byChnNo, wRet );
       	return FALSE;
	}
	return TRUE;
}


/*=============================================================================
  �� �� ���� SetMediaRcvNetParam
  ��    �ܣ� ����Medianet Receive�����������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byChnNo        ��ͨ����
  �� �� ֵ�� TRUE:���óɹ���FALSE������ʧ��
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
BOOL32 CMpu2VmpTestClient::SetMediaRcvNetParam( u8 byChnNo)
{
	TLocalNetParam tlocalNetParm;
    memset(&tlocalNetParm, 0, sizeof(TLocalNetParam));
    tlocalNetParm.m_tLocalNet.m_wRTPPort  = m_tCfg.wRcvStartPort + byChnNo * PORTSPAN;
    tlocalNetParm.m_tLocalNet.m_wRTCPPort = m_tCfg.wRcvStartPort + 1 + byChnNo * PORTSPAN;
    tlocalNetParm.m_dwRtcpBackAddr        = m_dwServerRcvIp;//�Ѿ����������ˣ�������ת
	// xliang [5/6/2009] ����ԭ�� ChnNo		Port(��MPU�󶨵�MP����rtcp�˿�)
	//							  0~7	===> 2~9 (��39002,...,39009)
	//							  8~15	===> 12~19 (��39012,...,39019)
	//16~23	===> 22~29	
	//24~25 ====> 32~33	
	u16 wRtcpBackPort;
	if (byChnNo >= 24)
	{
		wRtcpBackPort = m_wServerRcvStartPort+8+byChnNo;
	}
	else if (byChnNo >= 16)
	{
		wRtcpBackPort = m_wServerRcvStartPort + 6 + byChnNo;
	}
	if (byChnNo >= 8)
	{
		wRtcpBackPort = m_wServerRcvStartPort + 4 + byChnNo;
	}
	else
	{
		wRtcpBackPort = m_wServerRcvStartPort + 2 + byChnNo;
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
  �� �� ���� MsgFastUpdatePicProc
  ��    �ܣ� ����ؼ�֡������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage
  �� �� ֵ�� void
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::MsgFastUpdatePicProc( CMessage * const pMsg )
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

		m_cHardMulPic.SetFastUpdata( byChnnlId );

	
        KeyLog( "[MsgFastUpdatePicProc]m_cHardMulPic.SetFastUpdata(%u)!\n",byChnnlId );
    }

}

/*=============================================================================
  �� �� ���� ProcTestOusCmd
  ��    �ܣ� ����mpu2״̬�ƴ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::ProcTestLedCmd()
{
	//1���������еĵ�	
	for (u8 byLoop =0; byLoop < MPU2_LED_NUM ; byLoop++ )
	{
#ifdef _LINUX_
		s32 nRet = BrdLedStatusSet( m_abyLedId[byLoop],BRD_LED_OFF );
		if ( OK == nRet )
		{
			KeyLog("[ProcTestOusCmd]Set Led:%d OFF success\n",m_abyLedId[byLoop]);
		}
		else
		{
			KeyLog("[ProcTestOusCmd]Set Led:%d OFF failed\n",m_abyLedId[byLoop]);
		}
#endif
	}
	//2����timer�����ε��
	u8 byLedIdIdx = 0;
	SetTimer(TIMER_TEST_LED,TEST_LED_TIMER_INTERVAL,byLedIdIdx);
}


/*=============================================================================
  �� �� ���� TimerTestLed
  ��    �ܣ� ��Ƶ�timer����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage
  �� �� ֵ�� void
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::TimerTestLed(CMessage* const pMsg)
{
	if ( NULL == pMsg ) 
	{
		ErrorLog("[TimerTestLed]Null pointer CMessage\n");
		return;
	}
	KillTimer(TIMER_TEST_LED);
	u8 byLedIdIdx = *(u8 *)pMsg->content;
	if ( byLedIdIdx > MPU2_LED_NUM )
	{
		ErrorLog("[TimerTestLed]Invalid ledIdx:%d\n",byLedIdIdx);	
		return;
	}
	s32 nRet = 0;
	//1�����ǰ��һ�ε����ĵ�
	if ( byLedIdIdx > 0 ) 
	{
#ifdef _LINUX_
		nRet = BrdLedStatusSet( m_abyLedId[byLedIdIdx -1 ],BRD_LED_OFF );
		if ( OK == nRet )
		{
			KeyLog("[TimerTestLed]Set Led:%d OFF success\n",m_abyLedId[byLedIdIdx -1]);
		}
		else
		{
			KeyLog("[TimerTestLed]Set Led:%d OFF failed\n",m_abyLedId[byLedIdIdx -1]);
		}
#endif
	
	}
	if ( byLedIdIdx == MPU2_LED_NUM  ) //���һ����Ҳ��������
	{
		//�������һ���ƣ���ʱ���еƶ�����
		KeyLog("[TimerTestLed] All Leds are off now!!!\n");
		return;
	}
	//2���������Ҫ��ĵ�
#ifdef _LINUX_
	nRet = BrdLedStatusSet( m_abyLedId[byLedIdIdx],BRD_LED_ON );
	if ( OK == nRet )
	{
		KeyLog("[TimerTestLed]Set Led:%d ON success\n",m_abyLedId[byLedIdIdx]);
	}
	else
	{
		KeyLog("[TimerTestLed]Set Led:%d ON failed\n",m_abyLedId[byLedIdIdx]);
	}
#endif
	
	//3����������һյ��
	SetTimer(TIMER_TEST_LED,TEST_LED_TIMER_INTERVAL,byLedIdIdx + 1);

	
}

/*=============================================================================
  �� �� ���� Proc8KIStartVmpTestReq
  ��    �ܣ� ������Է������������Ŀ�ʼvmp��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
   2012/04/19		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::Proc8KIStartVmpTestReq(CMessage* const pMsg)
{
	if ( pMsg == NULL )
	{
		ErrorLog("[Proc8KIStartVmpTestReq]null pointer msg in!\n");
		return;
	}
 #ifdef _8KI_
	//1��������Ϣ�壬��Ϣ���д��˵�ǰҪ���Ե����Ŀ�card
	CServMsg cServMsg( pMsg->content, pMsg->length );
	u8* pContent = cServMsg.GetMsgBody();
	u8 byCardIdx = *pContent;
	pContent += sizeof(u8);
	if (byCardIdx != _8KI_LEFTCARD && byCardIdx != _8KI_RIGHTCARD)
	{
		post( MAKEIID( MPU_SERVER_APPID, 1 ), 
			C_S_STARTVMPTEST_NACK, 
			NULL, 0, m_dwMpuTestDstNode
			);
		ErrorLog("[Proc8KIStartVmpTestReq]Server Passed Invalid cardidx:d\n",byCardIdx);
		return;
	}
	if (m_by8KICurTestCard != 0xFF && byCardIdx == m_by8KICurTestCard )//��Ϊ�Ѿ���ʼ�����ˣ�ֱ�ӻ�ack����������ʼ������
	{
		post( MAKEIID( MPU_SERVER_APPID, 1 ), 
			C_S_STARTVMPTEST_ACK, 
			NULL, 0, m_dwMpuTestDstNode
		);
		return;
	}
	m_cHardMulPic.StopMerge();
	//2��TODO:ý�ز��ʼ������ý�صĽӿ�,��ʼ�����ɹ���Ҫ��nack
	
	post( MAKEIID( MPU_SERVER_APPID, 1 ), 
		C_S_STARTVMPTEST_ACK, 
		NULL, 0, m_dwMpuTestDstNode
		);
	//3��׼���ÿ�ʼ�շ�
	m_by8KICurTestCard = byCardIdx;//���浱ǰ���Եİ忨id
	PrepareToTest();
 #endif
}