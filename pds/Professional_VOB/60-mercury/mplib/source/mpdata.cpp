/*****************************************************************************
   ģ����      : MP
   �ļ���      : mpdata.cpp
   ����ļ�    : mp.h
   �ļ�ʵ�ֹ���: MP��ȫ������ʵ��
   ����        : ������
   �汾        : V4.0  Copyright(C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��       �汾        �޸���          �޸�����
   2007/09/24   4.0         �ű���          ����
******************************************************************************/
#include "mp.h"

CMpApp g_cMpApp;

// ����
CMpData::CMpData() : m_nInitUdpHandle(0),
					 m_dwMcuNode(0),
					 m_dwMcuIId(0),
                     m_dwMcuNodeB(0),           
                     m_dwMcuIIdB(0),
                     m_dwMcuIpAddr(0),
					 m_wMcuPort(0),
                     m_dwMcuIpAddrB(0),                    
                     m_wMcuPortB(0),
                     m_bEmbedA(FALSE),
                     m_bEmbedB(FALSE),
					 m_dwMpcSSrc(0),
                     m_byAttachMode(0),
                     m_byMpId(0),
                     m_byMcuId(0),
                     m_dwIpAddr(0),
                     m_byRegAckNum(0),
                     m_byConnectNum(0),
                     m_bIsRestrictFlux(FALSE),
                     m_bIsSendOverrunNtf(TRUE),
                     m_bIsWaitingIFrame(FALSE),
					 m_byWIFrameSeqInt(SWITCH_SRC_SEQ_INTERVAL),
                     m_bCancelWaitIFrameNoUni(FALSE),
					 m_bySeqNumRangeLow(MIN_SEQNUM_INTERVAL),
					 m_bySeqNumRangeUp(MAX_SEQNUM_INTERVAL),
					 m_bySensSeqNum(SWITCH_SRC_SEQ_INTERVAL),
					 m_byRtcpBackTimerInterval(RTCPBACK_INTERVAL),
                     m_dwPinHoleInterval(5),
					 m_byMutePackNum(5),
					 m_byMutePackSendInterval(1)
					 
                     
{
    memset(m_abyConfMode, 0, sizeof(m_abyConfMode));
    memset(m_byMtNumNeedT, 0, sizeof(m_byMtNumNeedT));
    memset(m_abyMpAlias, 0, sizeof(m_abyMpAlias));
    memset(m_awChgDelayTick, 0, sizeof(m_awChgDelayTick));
    memset(m_abyPtState, 0, sizeof(m_abyPtState));
    memset(m_atPinHoleTable, 0, sizeof(m_atPinHoleTable));
}

// ����
CMpData::~CMpData()
{
    if(NULL != m_pptFilterParam)
    {
        delete[] m_pptFilterParam;
        m_pptFilterParam = NULL;
    }

    if(NULL != m_pptFilterData)
    {
        delete[] m_pptFilterData;
        m_pptFilterData = NULL;
    }

    if(NULL != m_pptWaitIFrmData)
    {
        delete[] m_pptWaitIFrmData;
        m_pptWaitIFrmData = NULL;
    }

    if( NULL != m_pptSwitchChannel )
    {
        delete[] m_pptSwitchChannel;
        m_pptSwitchChannel = NULL;
    }

    if( NULL != m_hPtInfo )
    {
        OspSemDelete( m_hPtInfo );
        m_hPtInfo = NULL;
    }

    if( NULL != m_hPtTick )
    {
        OspSemDelete( m_hPtTick );
        m_hPtTick = NULL;
    }
    
    if( NULL != m_hMtNum )
    {
        OspSemDelete( m_hMtNum );
        m_hMtNum = NULL;
    }

	if ( NULL != m_ptSendMutePackTable )
	{
		delete[] m_ptSendMutePackTable;
		m_ptSendMutePackTable = NULL;
	}
}

/*=============================================================================
  �� �� ���� Init
  ��    �ܣ� ��һ����������Ϣ������ڴ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/18  4.0			�ű���                ���Ӷ���ת����ź�������
=============================================================================*/
BOOL32 CMpData::Init()
{
    m_pptSwitchChannel = new TSwitchChannel[MAX_CONFIDX][MAX_SWITCH_CHANNEL];
    if( NULL == m_pptSwitchChannel )  
    {
        MpLog( LOG_CRIT, "[Init] allocate memory for SwitchChnnl failed !\n");
        return FALSE;
    }
    ClearSwitchTable( TRUE );

    m_pptFilterParam = new TSendFilterParam[MAX_CONFIDX][MAX_SWITCH_CHANNEL];
    if( NULL == m_pptFilterParam )  return FALSE;
    
    m_pptFilterData = new TSendFilterAppData[MAX_CONFIDX][MAX_SWITCH_CHANNEL];
    if( NULL == m_pptFilterData ) return FALSE;

    m_pptWaitIFrmData = new TWaitIFrameData[MAX_CONFIDX][MAXNUM_CONF_MT+POINT_NUM];
    if( NULL == m_pptWaitIFrmData ) return FALSE;


    BOOL32 bRet = TRUE;
    bRet = OspSemBCreate( &m_hPtInfo );
    if ( !bRet ) 
    {
        OspSemDelete( m_hPtInfo );
        m_hPtInfo = NULL;
        MpLog( LOG_CRIT, "[Init] create semhandle for PtInfo failed !\n" );
        return FALSE;
    }

    bRet = OspSemBCreate( &m_hPtTick );
    if ( !bRet ) 
    {
        OspSemDelete( m_hPtTick );
        m_hPtTick = NULL;
        MpLog( LOG_CRIT, "[Init] create semhandle for PtTick failed !\n" );
        return FALSE;
    }

    bRet = OspSemBCreate( &m_hMtNum );
    if ( !bRet ) 
    {
        OspSemDelete( m_hMtNum );
        m_hMtNum = NULL;
        MpLog( LOG_CRIT, "[Init] create semhandle for MtNum failed !\n" );
        return FALSE;
    }

	m_ptSendMutePackTable = new TSendMutePack[MAXNUM_CONF_MT];
	if ( NULL == m_ptSendMutePackTable )
	{
		return FALSE;
	}

	memset(m_wReverseG7221cPort, 0, sizeof(m_wReverseG7221cPort));
    
    return TRUE;
}

/*=============================================================================
  �� �� ���� FreeStatusA
  ��    �ܣ� ���A��״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CMpData::FreeStatusA(void)
{
    m_dwMcuNode = INVALID_NODE;
    m_dwMcuIId  = INVALID_INS;
    return;
}

/*=============================================================================
  �� �� ���� FreeStatusB
  ��    �ܣ� ���B��״̬
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CMpData::FreeStatusB(void)
{
    m_dwMcuNodeB = INVALID_NODE;
    m_dwMcuIIdB  = INVALID_INS;
    return;
}

/*=============================================================================
  �� �� ���� CreateDS
  ��    �ܣ� ��ʼ��DataSwitch
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMpData::CreateDS()
{
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	if( DSOK != dsSetCapacity( 512,512 ) )
	{
		printf("[CreateDS] dsSetCapacity 512 failed !\n");
	}
#endif
	
    u32 adwIpAddr[] = { htonl(m_dwIpAddr), 0 };
    m_nInitUdpHandle = dsCreate(NUM_IPADDR, adwIpAddr);
    

#ifndef WIN32    
    if(INVALID_DSID == m_nInitUdpHandle)
    {
        MpLog( LOG_CRIT, "[CreateDS] Call dsCreate Failure !\n");
        return FALSE;
    }
#endif
    return TRUE;
}

/*=============================================================================
  �� �� ���� DestroyDS
  ��    �ܣ� ����dataswitch
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CMpData::DestroyDS()
{
    dsDestroy(m_nInitUdpHandle);
    return;
}


/*=============================================================================
  �� �� ���� MpSendFilterParamInit
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/18    4.0			�ű���                ������ֲ
=============================================================================*/
void CMpData::MpSendFilterParamInit(void)
{
    if( NULL == m_pptFilterParam   || 
        NULL == m_pptFilterData ||
        NULL == m_pptWaitIFrmData )
    {
        MpLog( LOG_CRIT, "[MpSendFilterParamInit] memory allocate failed !\n");
        return;
    }

    u8 byConfIdx = 0;
    for( ; byConfIdx < MAX_CONFIDX; byConfIdx++ )
    {
        MpResetSendFilterParam( byConfIdx );
    }
    return;
}

/*=============================================================================
  �� �� ���� MpResetSendFilterParam
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfNo
  �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/18    4.0			�ű���                ������ֲ
=============================================================================*/
void CMpData::MpResetSendFilterParam(u8 byConfNo)
{
    if( NULL == m_pptFilterParam   || 
        NULL == m_pptFilterData ||
        NULL == m_pptWaitIFrmData )
    {
        MpLog( LOG_CRIT, "[MpResetSendFilterParam] memory allocate failed !\n");
        return;
    }
    
    for(u16 wChannelIdx = 0; wChannelIdx < MAX_SWITCH_CHANNEL; wChannelIdx++)
    {
        m_pptFilterParam[byConfNo][wChannelIdx].nSeqNumInterval     = 0;
        m_pptFilterParam[byConfNo][wChannelIdx].nTimeStampInterval  = 0;
        m_pptFilterParam[byConfNo][wChannelIdx].wModifySeqNum       = 0;
        m_pptFilterParam[byConfNo][wChannelIdx].dwModifyTimeStamp   = 0;
        m_pptFilterParam[byConfNo][wChannelIdx].dwModifySSRC        = 0;
        m_pptFilterParam[byConfNo][wChannelIdx].wLastSeqNum         = 0;
        m_pptFilterParam[byConfNo][wChannelIdx].dwLastTimeStamp     = 0;
        m_pptFilterParam[byConfNo][wChannelIdx].dwLastSSRC          = 0;

        m_pptFilterData[byConfNo][wChannelIdx].byConfNo   = 0xFF;
        m_pptFilterData[byConfNo][wChannelIdx].wChannelNo = 0xFFFF;
        m_pptFilterData[byConfNo][wChannelIdx].dwDstIp    = 0;
        m_pptFilterData[byConfNo][wChannelIdx].wDstPort   = 0;
        m_pptFilterData[byConfNo][wChannelIdx].dwAddTime  = 0;
        m_pptFilterData[byConfNo][wChannelIdx].bRemoved   = FALSE;
        m_pptFilterData[byConfNo][wChannelIdx].bUniform   = FALSE;
        m_pptFilterData[byConfNo][wChannelIdx].byPayload  = INVALID_PAYLOAD;
    }

    for(u16 wMtIdx = 0; wMtIdx < MAXNUM_CONF_MT+POINT_NUM; wMtIdx++)
    {
        m_pptWaitIFrmData[byConfNo][wMtIdx].byConfNo = 0xff;
        m_pptWaitIFrmData[byConfNo][wMtIdx].byMtId   = 0xff;
        m_pptWaitIFrmData[byConfNo][wMtIdx].dwSrcIp  = 0;
        m_pptWaitIFrmData[byConfNo][wMtIdx].wSrcPort = 0;
        m_pptWaitIFrmData[byConfNo][wMtIdx].bWaitingIFrame = FALSE;
        m_pptWaitIFrmData[byConfNo][wMtIdx].dwFstTimeStamp = 0;
    }
}


/*=============================================================================
  �� �� ���� AddSwitchTable
  ��    �ܣ� ���ӽ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8              byConfIdx
             TSwitchChannel* ptSwitchChannel
             u8              byType��1.Add 1 to 1 switch, 2.Add Many to 1 switch
  �� �� ֵ�� BOOL32 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/20    4.0			�ű���                ������ֲ
  2007/07/19    4.0         �ű���                �������Ӳ��� ����
=============================================================================*/
BOOL32 CMpData::AddSwitchTable( u8 byConfIdx, TSwitchChannel* ptSwitchChannel, u8 byType )
{
    u16 wTableIdx = 0;
    TSwitchChannel tSwitchChannel = *ptSwitchChannel;

    u32 dwDstIp  = tSwitchChannel.GetDstIP();
    u16 wDstPort = tSwitchChannel.GetDstPort();
    u32 dwRcvIp  = tSwitchChannel.GetRcvIP();
    u16 wRcvPort = tSwitchChannel.GetRcvPort();

    u16 wIdx = 0;
    u16 wFirstNullIdx = INVALID_SWITCH_CHANNEL;

    if( byConfIdx >= MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[AddSwitchTable] ConfIdx.%d error !\n", byConfIdx + 1 );
        return FALSE;
    }

    if( NULL == m_pptSwitchChannel )
    {
        MpLog( LOG_CRIT, "[AddSwitchTable] err: NULL == m_pptSwitchChannel\n" );
        return FALSE;
    }

    BOOL32 bIsExist = FALSE;
    if( SWITCH_ONE2ONE == byType )
    {
        while( wTableIdx < MAX_SWITCH_CHANNEL )
        {
            if( m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstIP()   == dwDstIp && 
                m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstPort() == wDstPort )
            {
                wIdx = wTableIdx;
                bIsExist = TRUE;
                break;
            }
            if( 0 == m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstIP() )
            {
                // zbq [07/19/2007] One2One�Ľ�����249�������£��Ŵ�ͷ
                wFirstNullIdx = wTableIdx;
            }
            wTableIdx++;
        }
    }
    else
    {
        while( wTableIdx < MAX_SWITCH_CHANNEL )
        {
            //zbq [07/19/2007] Mul2One����������Ҳ��Ҫ�����ظ�
            if ( m_pptSwitchChannel[byConfIdx][wTableIdx].GetRcvIP()  == dwRcvIp   &&
                 m_pptSwitchChannel[byConfIdx][wTableIdx].GetRcvPort() == wRcvPort &&
                 m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstIP()   == dwDstIp  && 
                 m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstPort() == wDstPort )
            {
                wIdx = wTableIdx;
                bIsExist = TRUE;
                break;                
            }
            if( 0 == m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstIP()) // ��һ����λ��
            {
                //zbq [07/19/2007] Mul2One��RTP����Ҳ�Ŵ�ͷ��RTCP������Сͷ
                if ( 0 == wRcvPort%2 )
                {
                    wFirstNullIdx = wTableIdx;
                }
                else
                {
                    if ( INVALID_SWITCH_CHANNEL == wFirstNullIdx )
                    {
                        wFirstNullIdx = wTableIdx;
                    }
                }
            }
            wTableIdx++;
        }
    }

    if( !bIsExist ) // ������
    {
        wIdx = wFirstNullIdx;
    }

    if( wIdx < MAX_SWITCH_CHANNEL )
    {
        m_pptSwitchChannel[byConfIdx][wIdx] = tSwitchChannel;

        s8 achRcvIp[16];
        s8 achDstIp[16];
        strcpy( achRcvIp, strofip(dwRcvIp) );
        strcpy( achDstIp, strofip(dwDstIp) );

        MpLog( LOG_INFO, "[AddSwitchTable] Add <%s@%d>-><%s@%d>!\n", 
                          achRcvIp, wRcvPort, achDstIp, wDstPort );
        return TRUE;
    }
    MpLog( LOG_CRIT, "[AddSwitchTable] Add Fail, exceeded.%d !\n!", wIdx );

    return FALSE;
}


/*=============================================================================
  �� �� ���� RemoveSwitchTable
  ��    �ܣ� �Ƴ�������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8     byConfIdx
             TSwitchChannel *ptSwitchChannel
             u8     byType
             BOOL32 bStop
  �� �� ֵ�� BOOL32 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/18    4.0			�ű���                ������ֲ
  2007/07/19    4.0         �ű���                ɾ��������� ����
=============================================================================*/
BOOL32 CMpData::RemoveSwitchTable(u8 byConfIdx, TSwitchChannel *ptSwitchChannel, u8 byType, BOOL32 bStop)
{
    if ( NULL == ptSwitchChannel || NULL == m_pptSwitchChannel ) 
    {
        MpLog( LOG_CRIT, "[RemoveSwitchTable] param err: ptSwitchChannel.0x%x, m_pptSwitchChannel.0x%x !\n",
                         ptSwitchChannel, m_pptSwitchChannel );
        return FALSE;
    }

    u8 byFstIdx   = INVALID_SWITCH_CHANNEL;
    u16 wTableIdx = 0;
    BOOL32 bFlag = TRUE;

    TSwitchChannel tSwitchChannel = *ptSwitchChannel;
    u32 dwDstIp  = tSwitchChannel.GetDstIP();
    u16 wDstPort = tSwitchChannel.GetDstPort();
    u32 dwRcvIp  = tSwitchChannel.GetRcvIP();
    u16 wRcvPort = tSwitchChannel.GetRcvPort();

    if( byConfIdx >= MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[RemoveSwitchTable] ConfIdx:%d error !\n", byConfIdx+1 );
        return FALSE;
    }

    while(wTableIdx < MAX_SWITCH_CHANNEL)
    {
        // zbq [07/19/2007] Mul2One�Ľ���ҪRcv��Dstһ���ʶ; ��ɾ�����У���������ɾ��֮
        if ( SWITCH_ONE2ONE == byType )
        {
            if( m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstIP()   == dwDstIp &&
                m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstPort() == wDstPort )
            {
                break;
            }
        }
        else
        {
            if ( !bStop )
            {
                if( m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstIP()   == dwDstIp  &&
                    m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstPort() == wDstPort &&
                    m_pptSwitchChannel[byConfIdx][wTableIdx].GetRcvIP()   == dwRcvIp  &&
                    m_pptSwitchChannel[byConfIdx][wTableIdx].GetRcvPort() == wRcvPort )
                {
                    break;
                }
            }
            else
            {
                if( m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstIP()   == dwDstIp &&
                    m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstPort() == wDstPort )
                {
                    if ( INVALID_SWITCH_CHANNEL == byFstIdx )
                    {
                        byFstIdx = wTableIdx;
                    }

                    u16 wPort = m_pptSwitchChannel[byConfIdx][wTableIdx].GetRcvPort();
                    u32 dwIp  = m_pptSwitchChannel[byConfIdx][wTableIdx].GetRcvIP();

                    s8 achRcvIp[16];
                    s8 achDstIp[16];
                    strcpy( achDstIp, strofip(dwDstIp) );
                    strcpy( achRcvIp, strofip(dwIp) );

                    MpLog( LOG_INFO, "[RemoveSwitchTable] Remove <%s@%d>-><%s@%d>, bStop \n",
                                      achRcvIp, wPort, achDstIp, wDstPort );
                    
                    memset(&m_pptSwitchChannel[byConfIdx][wTableIdx], 0, sizeof(TSwitchChannel));
                }
            }
        }
        wTableIdx++;
    }

    if ( !bStop )
    {
        if( wTableIdx < MAX_SWITCH_CHANNEL )
        {
            memset(&m_pptSwitchChannel[byConfIdx][wTableIdx], 0, sizeof(TSwitchChannel));
            
            s8 achRcvIp[16];
            s8 achDstIp[16];
            strcpy( achDstIp, strofip(dwDstIp) );
            strcpy( achRcvIp, strofip(dwRcvIp) );
            
            MpLog( LOG_INFO, "[RemoveSwitchTable] Remove <%s@%d>-><%s@%d> \n",
                              achRcvIp, wRcvPort, achDstIp, wDstPort );
        }
        else
        {
            bFlag = FALSE;
            MpLog( LOG_ERR, "[RemoveSwitchTable] Entry Failure. Entry = %s:%d.\n", strofip(dwDstIp), wDstPort);
        }        
    }
    else
    {
        if ( INVALID_SWITCH_CHANNEL == byFstIdx )
        {
            bFlag = FALSE;
            MpLog( LOG_ERR, "[RemoveSwitchTable] Entry Failure. Entry = %s:%d. bStop\n", strofip(dwDstIp), wDstPort);
        }
    }
    return bFlag;
}

/*=============================================================================
  �� �� ���� RemoveSwitchTableSrc
  ��    �ܣ� �Ƴ�������: ����Դ��ַ�Ͷ˿�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfIdx
             TSwitchChannel *ptSwitchChannel
  �� �� ֵ�� BOOL32 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/20    4.0			�ű���                ������ֲ
=============================================================================*/
BOOL32 CMpData::RemoveSwitchTableSrc( u8 byConfIdx, TSwitchChannel *ptSwitchChannel )
{
    if ( NULL == ptSwitchChannel || NULL == m_pptSwitchChannel ) 
    {
        MpLog( LOG_CRIT, "[RemoveSwitchTableSrc] param err: ptSwitchChannel.0x%x, m_pptSwitchChannel.0x%x !\n",
                          ptSwitchChannel, m_pptSwitchChannel );
        return FALSE;
    }

    u16 wTableIdx = 0;
    BOOL32 bFlag = FALSE;
    
    TSwitchChannel tSwitchChannel = *ptSwitchChannel;
    u32 dwSrcIp  = tSwitchChannel.GetSrcIp();
    u16 wSrcPort = tSwitchChannel.GetRcvPort();
    u16 wRcvPort = tSwitchChannel.GetRcvPort();
    
    if( byConfIdx >= MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[RemoveSwitchTableSrc] ConfIdx:%d error !\n", byConfIdx+1 );
        return FALSE;
    }
    
    while(wTableIdx < MAX_SWITCH_CHANNEL)
    {
        if( m_pptSwitchChannel[byConfIdx][wTableIdx].GetSrcIp()   == dwSrcIp &&
            m_pptSwitchChannel[byConfIdx][wTableIdx].GetRcvPort() == wSrcPort )
        {
            // zbq [05/25/2007] �ϸ�ƥ��Դ����
            if ( g_cMpApp.IsPointSrc(m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstPort()) )
            {
                break;
            }
        }
        wTableIdx++;
    }
    
    if(wTableIdx < MAX_SWITCH_CHANNEL)
    {
        s8 achSrcIp[16];
        s8 achRcvIp[16];
        s8 achDstIp[16];
        strcpy( achSrcIp, strofip(tSwitchChannel.GetSrcIp()) );
        strcpy( achRcvIp, strofip(tSwitchChannel.GetRcvIP()) );
        strcpy( achDstIp, strofip(m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstIP()) );
        
        MpLog( LOG_INFO, "[RemoveSwitchTable] Remove <%s@%d>-><%s@%d>-><%s@%d> by SRC\n",
                          achSrcIp, wRcvPort, achRcvIp, wRcvPort, achDstIp,
                          m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstPort());

        memset(&m_pptSwitchChannel[byConfIdx][wTableIdx], 0, sizeof(TSwitchChannel));
        bFlag = TRUE;
    }
    else
    {
        MpLog( LOG_CRIT, "[RemoveSwitchTableSrc] Entry Failure. Entry = %s:%d by Src\n", strofip(dwSrcIp), wSrcPort);
        bFlag = FALSE;
    }
    return bFlag;
}

/*=============================================================================
  �� �� ���� ClearSwitchTable
  ��    �ܣ� ��ս�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� BOOL32 bClearAll
             u8 byConfIdx = 0
  �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/20    4.0			�ű���                ������ֲ
=============================================================================*/
void CMpData::ClearSwitchTable( BOOL32 bClearAll, u8 byConfIdx )
{    
    if( NULL == m_pptSwitchChannel )
    {
        MpLog( LOG_CRIT, "[ClearSwitchTable] NULL == m_pptSwitchChannel\n");
        return;
    }

    if ( bClearAll ) 
    {
        for(u8 byConfID = 0; byConfID < MAX_CONFIDX; byConfID++)
        {
            u16 wTableIdx = 0;
            while(wTableIdx < MAX_SWITCH_CHANNEL)
            {
                memset(&m_pptSwitchChannel[byConfID][wTableIdx], 0, sizeof(TSwitchChannel));
                wTableIdx++;
            }

			DelSendMutePackByConfIdx(byConfID);
        }
    }
    else
    {
        u16 wTableIdx = 0;
        while(wTableIdx < MAX_SWITCH_CHANNEL)
        {
            memset(&m_pptSwitchChannel[byConfIdx][wTableIdx], 0, sizeof(TSwitchChannel));
            wTableIdx++;
        }

		DelSendMutePackByConfIdx(byConfIdx + 1);
    }

    return;
}

/*=============================================================================
  �� �� ���� SearchSwitchTable
  ��    �ܣ� ���ҽ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfIdx
             u32 dwDstIp
             u16 wDstPort
             TSwitchChannel &tSwitchChannel
  �� �� ֵ�� BOOL32 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/20    4.0			�ű���                ������ֲ
=============================================================================*/
BOOL32 CMpData::SearchSwitchTable(u8 byConfIdx, u32 dwDstIp, u16 wDstPort, TSwitchChannel &tSwitchChannel )
{    
    if( byConfIdx > MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[SearchSwitchTable] ConfIdx:%d error !\n", byConfIdx + 1 );
        return FALSE;
    }

    if( NULL == m_pptSwitchChannel )
    {
        MpLog( LOG_CRIT, "[SearchSwitchTable] NULL == m_pptSwitchChannel\n");
        return FALSE;
    }

    BOOL32 bFlag = FALSE;
    u16 wTableIdx = 0;

    while(wTableIdx < MAX_SWITCH_CHANNEL)
    {
        if( m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstIP()   == dwDstIp &&
            m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstPort() == wDstPort )
        {
            tSwitchChannel = m_pptSwitchChannel[byConfIdx][wTableIdx];
            bFlag = TRUE;
            break;
        }
        wTableIdx++;
    }

    return bFlag;
}

/*=============================================================================
  �� �� ���� SearchBridgeSwitch
  ��    �ܣ� ���ҽ����� ����Ž���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfIdx
             u16 wRcvDstPort
             TSwitchChannel &tSwitchChannel
  �� �� ֵ�� BOOL32 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/29    4.0			�ű���                ����
=============================================================================*/
BOOL32 CMpData::SearchBridgeSwitch( u8 byConfIdx, u16 wRcvDstPort, TSwitchChannel &tSwitchChannel )
{
    if( byConfIdx > MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[SearchSwitchTable] ConfIdx:%d error !\n", byConfIdx + 1 );
        return FALSE;
    }
    
    if( NULL == m_pptSwitchChannel )
    {
        MpLog( LOG_CRIT, "[SearchSwitchTable] NULL == m_pptSwitchChannel\n");
        return FALSE;
    }
    
    BOOL32 bFlag = FALSE;
    u16 wTableIdx = 0;
    
    while(wTableIdx < MAX_SWITCH_CHANNEL)
    {
        if( m_pptSwitchChannel[byConfIdx][wTableIdx].GetRcvPort() == wRcvDstPort &&
            m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstPort() == wRcvDstPort )
        {
            tSwitchChannel = m_pptSwitchChannel[byConfIdx][wTableIdx];
            bFlag = TRUE;
            break;
        }
        wTableIdx++;
    }
    
    return bFlag;
}

/*=============================================================================
  �� �� ���� IsSrcSwitchExist
  ��    �ܣ� ���ҽ����� ���Դ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfIdx
             TSwitchChannel *ptSrcSwitch
  �� �� ֵ�� BOOL32 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/06/13    4.0			�ű���                ����
=============================================================================*/
BOOL32 CMpData::IsSrcSwitchExist( u8 byConfIdx, TSwitchChannel *ptSrcSwitch )
{
    if( byConfIdx > MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[IsSrcSwitchExist] ConfIdx:%d error !\n", byConfIdx + 1 );
        return FALSE;
    }
    
    if( NULL == m_pptSwitchChannel || NULL == ptSrcSwitch )
    {
        MpLog( LOG_CRIT, "[IsSrcSwitchExist] m_pptSwitch.0x%x, ptSrcSwitch.0x%x\n", m_pptSwitchChannel, ptSrcSwitch);
        return FALSE;
    }

    BOOL32 bGotSrcNow = FALSE;
    u8 bySwitchedPt = 0;
    if ( PS_SWITCHED == g_cMpApp.GetPtState(byConfIdx, 1) )
    {
        bySwitchedPt = 1;
    }
    else if ( PS_SWITCHED == g_cMpApp.GetPtState(byConfIdx, 2) )
    {
        bySwitchedPt = 2;
    }

    if ( bySwitchedPt != 0 )
    {
        bGotSrcNow = TRUE;
    }

    //u32 dwDstIp  = g_cMpApp.m_dwIpAddr;
    u16 wDstPort = byConfIdx * PORTSPAN + CONFBRD_STARTPORT + bySwitchedPt * POINT_NUM;

    BOOL32 bExist = FALSE;
    u16 wTableIdx = 0;

    if ( bGotSrcNow )
    {
        wTableIdx = 0;
        while (wTableIdx < MAX_SWITCH_CHANNEL)
        {
            if( m_pptSwitchChannel[byConfIdx][wTableIdx].GetSrcIp() == ptSrcSwitch->GetSrcIp() &&
                m_pptSwitchChannel[byConfIdx][wTableIdx].GetRcvIP() == ptSrcSwitch->GetRcvIP() &&
                m_pptSwitchChannel[byConfIdx][wTableIdx].GetRcvPort() == ptSrcSwitch->GetRcvPort() &&
                m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstPort() == wDstPort )
            {
                bExist = TRUE;
                break;
            }
            wTableIdx++;
        }
    }

    return bExist;
}

/*=============================================================================
  �� �� ���� GetSwitchChan
  ��    �ܣ� ��ý�����������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfIdx:
             u16 wIndex  : ������λ��
  �� �� ֵ�� BOOL32 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/20    4.0			�ű���                ����
=============================================================================*/
BOOL32 CMpData::GetSwitchChan( u8 byConfIdx, u16 wIndex, TSwitchChannel *ptSwitchChannel )
{
    if ( NULL == ptSwitchChannel || byConfIdx >= MAX_CONFIDX || wIndex >= MAX_SWITCH_CHANNEL ) 
    {
        MpLog( LOG_CRIT, "[GetSwitchTable] param err !\n" );
        return FALSE;
    }
    *ptSwitchChannel = m_pptSwitchChannel[byConfIdx][wIndex];
    return TRUE;
}


/*=============================================================================
  �� �� ���� ProceedSrcChange
  ��    �ܣ� Դ�л�֪ͨ��ɾ����һ��Դ��ؽ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfIdx
             u8 byPID
             BOOL32 bAnotherPt
  �� �� ֵ�� void 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  07/04/11    4.0         �ű���        ����
=============================================================================*/
void CMpData::ProceedSrcChange( u8 byConfIdx, u8 bySrcPID, BOOL32 bAnotherPt )
{
    if ( byConfIdx >= MAX_CONFIDX || bySrcPID > POINT_NUM || bySrcPID == 0 )
    {
        MpLog( LOG_CRIT, "[ProceedSrcChange] param err: ConfIdx.%d, byPID.%d \n", 
                                                           byConfIdx, bySrcPID );
        return;
    }

    u8  byDealPt = bySrcPID;
    if ( bAnotherPt )
    {
        byDealPt = bySrcPID == 1 ? 2 : 1; 
    }
    u32 dwDstIp  = g_cMpApp.m_dwIpAddr;
    u16 wDstPort = CONFBRD_STARTPORT + byConfIdx * PORTSPAN + byDealPt * POINT_NUM;

    TSwitchChannel tSwitchM2Src;
    if ( g_cMpApp.SearchSwitchTable(byConfIdx, dwDstIp, wDstPort, tSwitchM2Src) )
    {
        u32 dwRet1 = DSOK;
        u32 dwRet2 = DSOK;

        // 1. �� MT->M->S1/S2 �Ľ���
        dwRet1 = ::dsRemove( g_cMpApp.m_nInitUdpHandle, htonl(dwDstIp), wDstPort );
        if ( DSOK == dwRet1 ) 
        {
            // ֪ͨMCUɾ���ɵ�Դ����
            NtfMcuSrcSwitchChg( byConfIdx, tSwitchM2Src );

            if ( !g_cMpApp.RemoveSwitchTable(byConfIdx, &tSwitchM2Src, SWITCH_ONE2ONE) ) 
            {
                MpLog( LOG_CRIT, "[ProceedSrcChange] remove <0x%x@%d> from table failed!\n", 
                                                                        dwDstIp, wDstPort );
            }
        }
        else
        {
            MpLog( LOG_CRIT, "[ProceedSrcChange] dsRemove <%s@%d> failed, Ret=%d!\n", 
                                                strofip(dwDstIp), wDstPort, dwRet1 );
        }

        
        // 2. �Ƴ� M��dump
        dwRet2 = ::dsRemoveDump( g_cMpApp.m_nInitUdpHandle, 
                                 htonl(tSwitchM2Src.GetRcvIP()), tSwitchM2Src.GetRcvPort() );
        if ( DSOK != dwRet2 )
        {
            // FIXME: ��������Ž�����dumpΪʲô���Ƴ�ʧ��
			// to FIXME: ipӦ���������
            MpLog( LOG_ERR, "[ProceedSrcChange] dsRemoveDump <0x%x@%d> failed !\n", 
                              tSwitchM2Src.GetRcvIP(), tSwitchM2Src.GetRcvPort() );
        }

        // zbq[06/15/2007] �˴����ܻ������Ȼ��ʹ�õ��� �������в�����������MCU��ǿ�жϴ���
        /*
        // 3. �Ƴ� M���Ž�������������. �˱���MCU��MP���� zbq [04/29/2007]
        TSwitchChannel tBridgeSwitch;
        u16 wRcvDstPort = tSwitchM2Src.GetRcvPort();
        if ( g_cMpApp.SearchBridgeSwitch( byConfIdx, wRcvDstPort, tBridgeSwitch ) )
        {
            u32 dwRet3 = DSOK;
            dwRet3 = ::dsRemove( g_cMpApp.m_nInitUdpHandle,
                                 tBridgeSwitch.GetDstIP(), wRcvDstPort );
            if ( DSOK == dwRet3 )
            {
                if ( !g_cMpApp.RemoveSwitchTable( byConfIdx, &tBridgeSwitch, SWITCH_ONE2ONE ) )
                {
                    MpLog( LOG_CRIT, "[ProceedSrcChange] remove brg<0x%x@%d>-><0x%x@%d> frome table failed !\n",
                                      tBridgeSwitch.GetRcvIP(), wRcvDstPort, tSwitchM2Src.GetDstIP(), wRcvDstPort );
                }
            }
            else
            {
                MpLog( LOG_CRIT, "[ProceedSrcChange] dsRemove brg<0x%x@%d>-><0x%x@%d> failed !\n",
                                  tBridgeSwitch.GetRcvIP(), wRcvDstPort, tSwitchM2Src.GetDstIP(), wRcvDstPort );
            }
        }
		*/
    }
    else
    {
        MpLog( LOG_CRIT, "[ProceedSrcChange] search for dst<%d: 0x%x@%d> failed!\n",
                                                     byConfIdx, dwDstIp, wDstPort );
    }

    // 4. �� M->S1/S2->T �Ľ���
    TSwitchChannel tSwitchSrc2T;
    tSwitchSrc2T.SetRcvIP(dwDstIp);
    tSwitchSrc2T.SetRcvPort(wDstPort);
    tSwitchSrc2T.SetDstIP(dwDstIp);
    tSwitchSrc2T.SetDstPort(CONFBRD_STARTPORT + byConfIdx * PORTSPAN);

    u32 dwRet3 = DSOK;
    dwRet3 = ::dsRemoveManyToOne( g_cMpApp.m_nInitUdpHandle,
                                  htonl(tSwitchSrc2T.GetRcvIP()), tSwitchSrc2T.GetRcvPort(),
                                  htonl(tSwitchSrc2T.GetDstIP()), tSwitchSrc2T.GetDstPort() );
    if ( DSOK == dwRet3 )
    {
        if ( !g_cMpApp.RemoveSwitchTable(byConfIdx, &tSwitchSrc2T, SWITCH_MUL2ONE) ) 
        {
            MpLog( LOG_CRIT, "[ProceedSrcChange] remove <0x%x@%d>-><0x%x@%d> from table failed!\n", 
                              dwDstIp, wDstPort, dwDstIp, tSwitchSrc2T.GetDstPort() );
        }
    }
    else
    {
        MpLog( LOG_CRIT, "[ProceedSrcChange] RemoveM2One <0x%x@%d>-><0x%x@%d> failed, Ret=%d!\n", 
                                 dwDstIp, wDstPort, dwDstIp, tSwitchSrc2T.GetDstPort(), dwRet3 );
    }

    return;
}


/*=============================================================================
  �� �� ���� NtfMcuSrcSwitchChg
  ��    �ܣ� ֪ͨMCU����Դ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8              byConfIdx:
             TSwitchChannel  &tSwitch :
  �� �� ֵ�� BOOL32 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��      �汾        �޸���        �޸�����
  07/05/08    4.0         �ű���        ����
=============================================================================*/
void CMpData::NtfMcuSrcSwitchChg( u8 byConfIdx, TSwitchChannel &tSwitchSrc )
{
    if ( byConfIdx >= MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[NtfMcuSrcSwitchChg] param err: confIdx.%d !\n", byConfIdx );
        return;
    }

    CServMsg cServMsg;
    cServMsg.SetConfIdx( byConfIdx + 1 );
    cServMsg.SetSrcDriId( g_cMpApp.m_byMpId );
    cServMsg.SetEventId( MP_MCU_BRDSRCSWITCHCHANGE_NTF );
    cServMsg.SetMsgBody( (u8*)&tSwitchSrc, sizeof(TMt) );
	u16 wRcvPort = htons(tSwitchSrc.GetRcvPort());
	cServMsg.CatMsgBody((u8*)&wRcvPort,sizeof(u16));

    SendMsg2Mcu( cServMsg.GetEventId(), cServMsg );

    return;
}

/*=============================================================================
  �� �� ���� ShowWholeSwitch
  ��    �ܣ� ��ʾ���н�����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/06/13    4.0			�ű���                ����
=============================================================================*/
void CMpData::ShowWholeSwitch()
{
    u32 dwSrcIp, dwDisIp, dwRevIp, dwDstIp;
    u16 wRevPort, wDstPort;
    TSwitchChannel tSwitchChannel;
    
    OspPrintf(TRUE, FALSE, "\n---------------- Total switch channel info --------------------\n");
    
    if(NULL == m_pptSwitchChannel)
    {
        MpLog( LOG_CRIT, "[ShowSwitch] NULL == m_pptSwitchChannel\n");
        return;
    }
    
    for(int nLp = 0; nLp < MAX_CONFIDX; nLp++)
    {
        for(int nLp1 = 0; nLp1 < MAX_SWITCH_CHANNEL; nLp1++)
        {
            tSwitchChannel = m_pptSwitchChannel[nLp][nLp1];
            if(!tSwitchChannel.IsNull() || !tSwitchChannel.IsSrcNull() || !tSwitchChannel.IsRcvNull())
            {
                u8 byConfIdx = 0;
                u8 byPointId = 0;
                
                dwSrcIp = tSwitchChannel.GetSrcIp();
                dwDisIp = tSwitchChannel.GetDisIp();
                dwRevIp = tSwitchChannel.GetRcvIP();
                dwDstIp = tSwitchChannel.GetDstIP();
                
                wRevPort = tSwitchChannel.GetRcvPort();
                wDstPort = tSwitchChannel.GetDstPort();
                
                u16 wSrcPort  = 0;
                
                if ( g_cMpApp.IsPointT(wRevPort) )
                {
                    byConfIdx = (wRevPort - CONFBRD_STARTPORT) / PORTSPAN;
                    
                    for( u8 byPId = 1; byPId <= POINT_NUM; byPId ++ )
                    {
                        if ( PS_SWITCHED == g_cMpApp.GetPtState(byConfIdx, byPId) ) 
                        {
                            wSrcPort = CONFBRD_STARTPORT + byConfIdx * PORTSPAN + byPId * POINT_NUM;
                            break;
                        }                        
                    }
                    OspPrintf(TRUE, FALSE, "\nConf%d %d  %s:%d", nLp+1, nLp1, strofip(dwSrcIp), wSrcPort);
                }
                else if ( g_cMpApp.IsPointSrc(wRevPort, &byConfIdx, &byPointId) )
                {
                    TSwitchChannel tSwitchSrc;
                    g_cMpApp.SearchSwitchTable( byConfIdx, dwDstIp, wRevPort, tSwitchSrc );
                    wSrcPort = tSwitchSrc.GetRcvPort();
                    
                    OspPrintf(TRUE, FALSE, "\nConf%d %d  %s:%d", nLp+1, nLp1, strofip(dwSrcIp), wSrcPort);
                }
                else
                {
                    OspPrintf(TRUE, FALSE, "\nConf%d %d  %s:%d", nLp+1, nLp1, strofip(dwSrcIp), wRevPort);
                }
                
                if(dwDisIp > 0)
                {
                    OspPrintf(TRUE, FALSE, "--> %s:%d", strofip( dwDisIp ), wRevPort);
                }
                OspPrintf(TRUE, FALSE, "--> %s:%d", strofip( dwRevIp ), wRevPort);
                OspPrintf(TRUE, FALSE, "--> %s:%d", strofip( dwDstIp ), wDstPort);
            }
        }
    }
    OspPrintf( TRUE, FALSE, "\n" );
    
    return;
}

/*=============================================================================
  �� �� ���� ShowSwitchUniformInfo
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/20    4.0			�ű���                ����������ֲ
=============================================================================*/
void CMpData::ShowSwitchUniformInfo()
{
	u32 dwDstIp;
	u16 wDstPort;
	u16 wLoop1, wLoop2;
    TSendFilterAppData * ptSendAppData;

    OspPrintf(TRUE,FALSE,"\n----------------------switch channel uniform info--------------------\n");
    OspPrintf(TRUE,FALSE,"\nConfId ChnlNo         destaddr        IsUniform(0:no uniform 1:uniform)\n");

    if(NULL == g_cMpApp.m_pptFilterData)
    {
        return;
    }

    for (wLoop1=0; wLoop1 < MAX_CONFIDX; wLoop1++)
	{
        if ( CONF_UNIFORMMODE_VALID == g_cMpApp.GetConfUniMode((u8)wLoop1) )
        {
		    for (wLoop2=0; wLoop2<MAX_SWITCH_CHANNEL; wLoop2++)
		    {
			    ptSendAppData = &g_cMpApp.m_pptFilterData[wLoop1][wLoop2];
			    if (ptSendAppData->byConfNo != 0xFF && ptSendAppData->wChannelNo != 0xFFFF)
			    {
				    dwDstIp  = ptSendAppData->dwDstIp;
				    wDstPort = ptSendAppData->wDstPort;
                    if (ptSendAppData->bUniform)
                    {
                    }

				    OspPrintf(TRUE, FALSE, "   %d      %d       %s:%d          %d\n",
                              wLoop1+1, wLoop2, strofip(dwDstIp), wDstPort, ptSendAppData->bUniform);
			    }
		    }
        }
        else if ( CONF_UNIFORMMODE_NONE == g_cMpApp.GetConfUniMode((u8)wLoop1) )
        {
            OspPrintf(TRUE, FALSE, "   %d      Encrypt conf isn't uniform packet\n", wLoop1+1);
        }
	}
    OspPrintf(TRUE,FALSE,"\n----------------------switch channel uniform end---------------------\n");
}

/*=============================================================================
  �� �� ���� IsPointSrc
  ��    �ܣ� �ж�ĳ�˿��Ƿ�Ϊ�ն˽ڵ� S1��S2
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wPort
             u8 *pbyConfIdx
             u8 *pbyPointId
  �� �� ֵ�� BOOL32
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/10    4.0			�ű���                ����
=============================================================================*/
BOOL32 CMpData::IsPointSrc( u16 wPort, u8 *pbyConfIdx, u8 *pbyPointId )
{
    if ( wPort > CONFBRD_STARTPORT &&
         wPort < CONFBRD_STARTPORT + MAX_CONFIDX * PORTSPAN ) 
    {
        //ֻ�� S1��S2
        if ( 0 != (wPort - CONFBRD_STARTPORT) % PORTSPAN ) 
        {
            // �����������ǻ����
            u8 byConfIdx = (wPort - CONFBRD_STARTPORT) / PORTSPAN;
            u8 byPointId = (wPort - CONFBRD_STARTPORT - PORTSPAN * byConfIdx) / POINT_NUM; 
			
            if ( 1 == byPointId || 2 == byPointId ) 
            {
                if ( NULL != pbyConfIdx ) 
                {
                    *pbyConfIdx = byConfIdx;
                }
                if ( NULL != pbyPointId ) 
                {
                    *pbyPointId = byPointId;
                }
                return TRUE;
            }
        }
    }
    return FALSE;
}

/*=============================================================================
  �� �� ���� IsPointMt
  ��    �ܣ� �ж�ĳ�˿��Ƿ�Ϊ�ն˶˿�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wPort
  �� �� ֵ�� BOOL32
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/10    4.0			�ű���                ����
=============================================================================*/
BOOL32 CMpData::IsPointMt( u16 wPort )
{
    BOOL32 bRet = FALSE;
    if ( wPort >= MT_MCU_STARTPORT &&
         wPort <= MT_MCU_STARTPORT + MAXNUM_MCU_MT * PORTSPAN ) 
    {
        bRet = 0 == wPort % PORTSPAN ? TRUE : FALSE;
    }
    return bRet;
}

/*=============================================================================
  �� �� ���� IsPointT
  ��    �ܣ� �ж�ĳ�˿��Ƿ�ΪT�ڵ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wPort
  �� �� ֵ�� BOOL32
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/10    4.0			�ű���                ����
=============================================================================*/
BOOL32 CMpData::IsPointT( u16 wPort )
{
    BOOL32 bRet = FALSE;
    if ( wPort >= CONFBRD_STARTPORT &&
         wPort <= CONFBRD_STARTPORT + MAX_CONFIDX * PORTSPAN ) 
    {
        bRet = 0 == wPort % PORTSPAN ? TRUE : FALSE;
    }
    return bRet;
}

/*=============================================================================
    �� �� ���� GetWaitIFrameDataBySrc
    ��    �ܣ� ����Դ��Ϣ��ȡ�ȴ��ؼ�֡��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u32 dwSrcIp
               u16 wSrcPort
               TWaitIFrameData *ptWaitData
    �� �� ֵ�� BOOL 
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/5/11  4.0			������                  ����
=============================================================================*/
BOOL CMpData::GetWaitIFrameDataBySrc(u32 dwSrcIp, u16 wSrcPort, TWaitIFrameData *ptWaitData)
{
    for(u8 byConfNo = 0; byConfNo < MAXNUM_MCU_CONF; byConfNo++)
    {
        for(u8 byMtIdx = 0; byMtIdx < MAXNUM_CONF_MT+POINT_NUM; byMtIdx++)
        {            
            if (m_pptWaitIFrmData[byConfNo][byMtIdx].dwSrcIp  == dwSrcIp && 
                m_pptWaitIFrmData[byConfNo][byMtIdx].wSrcPort == wSrcPort)
            {
                *ptWaitData = m_pptWaitIFrmData[byConfNo][byMtIdx];
                return TRUE;
            }
        }
    }
    
    return FALSE;
}

/*=============================================================================
    �� �� ���� SetConfUniMode
    ��    �ܣ� �����Ƿ��һ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byConfIdx, u8 byMode
    �� �� ֵ�� BOOL 
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/5/11  4.0			������                  ����
=============================================================================*/
void CMpData::SetConfUniMode( u8 byConfIdx, u8 byMode )
{
    m_abyConfMode[byConfIdx] = byMode;
    return;
}

/*=============================================================================
    �� �� ���� GetConfUniMode
    ��    �ܣ� �����Ƿ��һ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byConfIdx
    �� �� ֵ�� BOOL 
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/5/11  4.0			������                  ����
=============================================================================*/
u8 CMpData::GetConfUniMode( u8 byConfIdx )
{
    return m_abyConfMode[byConfIdx];
}

/*=============================================================================
    �� �� ���� GetPtIdle
    ��    �ܣ� ��ÿ��е���ƵԴ��ת��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byConfIdx
    �� �� ֵ�� u8 : ���� 1 �� 2���ɹ�
                    ���� 0     ��ʧ��
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2007/03/29  4.0			�ű���                  ����
=============================================================================*/
u8 CMpData::GetPtIdle( u8 byConfIdx )
{
    u8 byPoint = 1;
    for( ; byPoint <= POINT_NUM; byPoint ++ )
    {
        if ( PS_IDLE == m_abyPtState[byConfIdx][byPoint-1] )
        {
            return byPoint;
        }
    }
    MpLog( LOG_ERR, "[GetPtIdle] ConfIdx.%d get Idle point failed! CurStatus<%d, %d>\n",
                      byConfIdx, m_abyPtState[byConfIdx][0], m_abyPtState[byConfIdx][1]);
    return 0;
}

/*=============================================================================
    �� �� ���� GetPtState
    ��    �ܣ� ��ȡ ĳ��ת�ڵ� ĳ��Ƶ�˿� ��ǰ״̬
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� u8 ʧ�ܣ�PS_UNEXIST
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2007/03/29  4.0			�ű���                  ����
=============================================================================*/
u8 CMpData::GetPtState( u8 byConfIdx, u8 byPoint, BOOL32 bAnother )
{
    if ( byConfIdx >= MAX_CONFIDX || 0 == byPoint ) 
    {
        MpLog( LOG_CRIT, "[GetPtState] param err: ConfIdx.%d, byPoint.%d !\n",
                          byConfIdx, byPoint );
        return PS_UNEXIST;
    }
    if ( !bAnother )
    {
        return m_abyPtState[byConfIdx][byPoint-1];
    }
    else
    {
        u8 byAnotherPt = 0;
        if ( byPoint == 1 )
        {
            byAnotherPt = 2;
        }
        else if ( byPoint == 2 )
        {
            byAnotherPt = 1;
        }
        else
        {
            MpLog( LOG_CRIT, "[GetPtState] Cur Point.%d unexist!\n", byPoint );
            return PS_UNEXIST;
        }

        return m_abyPtState[byConfIdx][byAnotherPt-1];
    }
}

/*=============================================================================
    �� �� ���� SetPtState
    ��    �ܣ� ��ע ĳ��ת�ڵ� ĳ��Ƶ�˿� ��ǰ״̬
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� void
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2007/03/29  4.0			�ű���                  ����
=============================================================================*/
void CMpData::SetPtState( u8 byConfIdx, u8 byPoint, u8 byState, BOOL32 bAnother )
{
	/*lint -save -e1788*/
    ENTER( m_hPtInfo )
	
    if ( !bAnother )
    {
        if ( PS_IDLE != m_abyPtState[byConfIdx][byPoint-1] )
        {
            MpLog( LOG_INFO, "[SetPtState] ConfIdx.%d: byPoint.%d state change from.%d to %d !\n", 
                              byConfIdx, byPoint, m_abyPtState[byConfIdx][byPoint-1], byState );
        }
        m_abyPtState[byConfIdx][byPoint-1] = byState;
    }
    else
    {
        u8 byAnotherPt = 0;
        if ( byPoint == 1 ) 
        {
            byAnotherPt = 2;
        }
        else if ( byPoint == 2 )
        {
            byAnotherPt = 1;
        }
        else
        {
            MpLog( LOG_CRIT, "[SetPtState] cur point.%d unexist !\n", byPoint );
            return;
        }
        if ( PS_IDLE != m_abyPtState[byConfIdx][byAnotherPt-1] )
        {
            MpLog( LOG_INFO, "[SetPtState] ConfIdx.%d: byPoint.%d state change from.%d to %d !\n", 
                        byConfIdx, byAnotherPt, m_abyPtState[byConfIdx][byAnotherPt-1], byState );
        }
        m_abyPtState[byConfIdx][byAnotherPt-1] = byState;
    }
    return;
	/*lint -restore*/
}

/*=============================================================================
    �� �� ���� SetChgDelay
    ��    �ܣ� ���� ĳ�����Ƿ�ǿ���л��ĳ�ʱʱ�� ��λ tick
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8  byConfIdx
               u16 wDelayTime
    �� �� ֵ�� void
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2007/04/13  4.0			�ű���                  ����
=============================================================================*/
void CMpData::SetChgDelay( u8 byConfIdx, u16 wDelayTick )
{
	/*lint -save -e1788*/
    ENTER( m_hPtTick )
    m_awChgDelayTick[byConfIdx] = wDelayTick;
    return;
	/*lint -restore*/
}

/*=============================================================================
    �� �� ���� GetChgDelay
    ��    �ܣ� ��ȡ ĳ�����Ƿ�ǿ���л��ĳ�ʱʱ�� ��λ tick
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byConfIdx 
    �� �� ֵ�� u16
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2007/04/13  4.0			�ű���                  ����
=============================================================================*/
u16 CMpData::GetChgDelay( u8 byConfIdx )
{
    return m_awChgDelayTick[byConfIdx];
}

/*=============================================================================
    �� �� ���� ClearChgDelay
    ��    �ܣ� ���� ĳ�����Ƿ�ǿ���л��ĳ�ʱʱ�� ��λ tick
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8  byConfIdx
    �� �� ֵ�� void
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2007/04/19  4.0			�ű���                  ����
=============================================================================*/
void CMpData::ClearChgDelay( u8 byConfIdx )
{
	/*lint -save -e1788*/
    ENTER( m_hPtTick )
    m_awChgDelayTick[byConfIdx] = 0;
    return;
	/*lint -restore*/
}

/*=============================================================================
  �� �� ���� GetChanNoByDst
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8  byConfIdx:
             u32 dwDstIp  :
             u16 wDstPort :
             u8  byGetType: 0-set, 1-romove
             u16 wRcvPort : Ŀǰֻ��� M->S1/S2->T �����ִ���
  �� �� ֵ�� u16 
=============================================================================*/
u16  CMpData::GetChanNoByDst(u8 byConfIdx, u32 dwDstIp, u16 wDstPort, u8 byGetType, u16 wRcvPort)
{
    u16 wChanNo = 0;

    if( NULL == g_cMpApp.m_pptFilterData )
    {
        MpLog( LOG_CRIT, "[GetChanNoByDst] err: NULL == m_pptFilterData\n");
        return wChanNo;
    }

    // 
    while ( wChanNo < MAX_SWITCH_CHANNEL )
    {
        if (m_pptFilterData[byConfIdx][wChanNo].dwDstIp  == dwDstIp &&
            m_pptFilterData[byConfIdx][wChanNo].wDstPort == wDstPort)
        {
            if ( 0 != wRcvPort )
            {
                // ���� S1->T �� S2->T
                if ( IsPortSrcToT(byConfIdx, wRcvPort) &&
                     m_pptFilterData[byConfIdx][wChanNo].wRcvPort == wRcvPort )
                {
                    return wChanNo;
                }
            }
            else
            {
                return wChanNo;
            }
        }
		wChanNo++;
    }

    if (wChanNo >= MAX_SWITCH_CHANNEL && byGetType == 1)
    {
        return MAX_SWITCH_CHANNEL;
    }

    wChanNo = 0;
    while (wChanNo < MAX_SWITCH_CHANNEL) // ��һ���յ�λ��
    {
        if (m_pptFilterData[byConfIdx][wChanNo].dwDstIp == 0)
        {
            u32 dwNow = time(NULL);
            m_pptFilterData[byConfIdx][wChanNo].dwDstIp  = dwDstIp;
            m_pptFilterData[byConfIdx][wChanNo].wDstPort = wDstPort;

            if ( IsPortSrcToT(byConfIdx, wRcvPort) )
            {
                m_pptFilterData[byConfIdx][wChanNo].wRcvPort = wRcvPort;
            }           

            srand(dwNow);
            m_pptFilterData[byConfIdx][wChanNo].wSeqNum     = rand();
            m_pptFilterData[byConfIdx][wChanNo].dwTimeStamp = rand();
            m_pptFilterData[byConfIdx][wChanNo].dwSSRC      = rand();
            
            m_pptFilterData[byConfIdx][wChanNo].dwAddTime   = dwNow;
            m_pptFilterData[byConfIdx][wChanNo].bRemoved    = FALSE;
            return wChanNo;
        }
		
		wChanNo++;
    }

    wChanNo = 0;
    u16 wTempChannelNo = 1;
    u16 wSelectChannelNo = MAX_SWITCH_CHANNEL;

    while (wChanNo < MAX_SWITCH_CHANNEL) // Ѱ�Һ���λ��(�Ѿ��Ƴ��˵�)
    {
        if (m_pptFilterData[byConfIdx][wChanNo].bRemoved)
        {
            wTempChannelNo = wChanNo + 1;
            while (wTempChannelNo < MAX_SWITCH_CHANNEL) // Ѱ����������λ��
            {
                if (m_pptFilterData[byConfIdx][wTempChannelNo].bRemoved)
                {
                    if (m_pptFilterData[byConfIdx][wChanNo].dwAddTime >
                        m_pptFilterData[byConfIdx][wTempChannelNo].dwAddTime)
                    {
                        wSelectChannelNo = wTempChannelNo;
                        wChanNo = wTempChannelNo;
                    }
                }
                wTempChannelNo++;
            }
            wSelectChannelNo = wChanNo;
            break;
        }
        wChanNo++;
    }

    if (wSelectChannelNo < MAX_SWITCH_CHANNEL)
    {
        u32 dwNow = time(NULL);
        m_pptFilterData[byConfIdx][wSelectChannelNo].dwDstIp  = dwDstIp;
        m_pptFilterData[byConfIdx][wSelectChannelNo].wDstPort = wDstPort;
        
        if ( IsPortSrcToT(byConfIdx, wRcvPort) )
        {
            m_pptFilterData[byConfIdx][wSelectChannelNo].wRcvPort = wRcvPort;
        } 

        srand(dwNow);
        m_pptFilterData[byConfIdx][wSelectChannelNo].wSeqNum     = rand();
        m_pptFilterData[byConfIdx][wSelectChannelNo].dwTimeStamp = rand();
        m_pptFilterData[byConfIdx][wSelectChannelNo].dwSSRC      = rand();
    
        m_pptFilterData[byConfIdx][wSelectChannelNo].dwAddTime   = dwNow;
        m_pptFilterData[byConfIdx][wSelectChannelNo].bRemoved    = FALSE;
    }

    return wSelectChannelNo;
}

/*=============================================================================
  �� �� ���� IsPortSrcToT
  ��    �ܣ� �Ƿ�Ϊ��ת�ڵ�ĺϷ�PORT
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wRcvPort
  �� �� ֵ�� BOOL32
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��	  �汾		    �޸���		�߶���    �޸�����
  2007/04/04  4.0			�ű���                  ����
=============================================================================*/
BOOL32 CMpData::IsPortSrcToT( u8 byConfIdx, u16 wRcvPort )
{
    if ( wRcvPort >= CONFBRD_STARTPORT ||
         wRcvPort <= CONFBRD_STARTPORT + MAX_CONFIDX * PORTSPAN )
    {
        u16 wPortRem = CONFBRD_STARTPORT + byConfIdx * PORTSPAN;
        if ( ( 0 == (wRcvPort - wPortRem) % POINT_NUM) &&
              ( 1 == (wRcvPort - wPortRem) / POINT_NUM ||
                2 == (wRcvPort - wPortRem) / POINT_NUM ) )
        {
            MpLog( LOG_INFO, "[IsPortSrcToT] Legal port.%d, ConfIdx.%d !\n", wRcvPort, byConfIdx );
            return TRUE;
        }
    }
    MpLog( LOG_ERR, "[IsPortSrcToT] illegal port.%d, ConfIdx.%d !\n", wRcvPort, byConfIdx );
    return FALSE;
}

/*=============================================================================
  �� �� ���� IsPortNeedChkIFrm
  ��    �ܣ� �Ƿ�Ϊ��Ҫ���ؼ�֡�Ľ��ն˿�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wRcvPort
  �� �� ֵ�� BOOL32
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/20    4.0			�ű���                  ����
=============================================================================*/
BOOL32 CMpData::IsPortNeedChkIFrm( u16 wRcvPort )
{
    return IsPointSrc(wRcvPort) || IsPointT(wRcvPort) || IsPointMt(wRcvPort);
}

/*=============================================================================
  �� �� ���� IsTInUseStill
  ��    �ܣ� ���˱�ͨ���⣬�������Ƿ��������ն˽���T�˿ڵ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfIdx
  �� �� ֵ�� BOOL32
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/27    4.0			�ű���                ����
=============================================================================*/
BOOL32 CMpData::IsTInUseStill( u8 byConfIdx )
{
    BOOL32 bRet = FALSE;

    if ( byConfIdx >= MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[IsTInUseStill] param err: byConfIdx=%d !\n", byConfIdx );
        return FALSE;
    }

    // ��Ҫ��T��ȡ���ݵ��ն���
    u8 byMtDataFromT = 0;
    
    for( u16 wChnIdx = 0; wChnIdx < MAX_SWITCH_CHANNEL; wChnIdx ++ )
    {
        if ( 0 != m_pptSwitchChannel[byConfIdx][wChnIdx].GetDstIP() &&
             0 != m_pptSwitchChannel[byConfIdx][wChnIdx].GetDstPort() )
        {
            if ( g_cMpApp.m_dwIpAddr == m_pptSwitchChannel[byConfIdx][wChnIdx].GetRcvIP() &&
                 CONFBRD_STARTPORT+byConfIdx*PORTSPAN == m_pptSwitchChannel[byConfIdx][wChnIdx].GetRcvPort() )
            {
                byMtDataFromT ++;

                MpLog( LOG_INFO, "[IsTInUseStill] Conf.%d: Rcv.0x%x@%d --> Dst.0x%x@%d !\n",
                                  byConfIdx,
                                  m_pptSwitchChannel[byConfIdx][wChnIdx].GetRcvIP(),
                                  m_pptSwitchChannel[byConfIdx][wChnIdx].GetRcvPort(),
                                  m_pptSwitchChannel[byConfIdx][wChnIdx].GetDstIP(),
                                  m_pptSwitchChannel[byConfIdx][wChnIdx].GetDstPort());
            }
        }
    }

    if ( 0 != byMtDataFromT )
    {
        bRet = TRUE;
        MpLog( LOG_INFO, "[IsTInUseStill] switchNum.%d in use of T !\n", byMtDataFromT );
    }

    return bRet;
}

/*=============================================================================
  �� �� ���� AddReverseG7221cPort
  ��    �ܣ� ������Ҫ��g7221.c��Ƶ��ʽ����������ż�Ի��Ķ˿�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wRcvPort
  �� �� ֵ�� void
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2013/11/14    4.0			�־                  ����
=============================================================================*/
void CMpData::AddReverseG7221cPort( u16 wRcvPort )
{
	u16 wReverseIdx = MAXnum_ReverseG7221cPort;
	for (u16 wIdx=0; wIdx<MAXnum_ReverseG7221cPort; wIdx++)
	{
		if (m_wReverseG7221cPort[wIdx] == 0)
		{
			m_wReverseG7221cPort[wIdx] = wRcvPort;
			return;
		}

		if (m_wReverseG7221cPort[wIdx] == wRcvPort)
		{
			// �Ѵ��ڣ�ֱ�ӷ���
			return;
		}
	}

    MpLog( LOG_INFO, "[AddReverseG7221cPort] m_wReverseG7221cPort is full, wRcvPort.%d!\n", wRcvPort );
	return;
}

/*=============================================================================
  �� �� ���� IsPortNeedReverseG7221cData
  ��    �ܣ� �ö˿��Ƿ���Ҫ��g7221.c��Ƶ��ʽ����������ż�Ի�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wRcvPort
  �� �� ֵ�� BOOL32
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2013/11/14    4.0			�־                  ����
=============================================================================*/
BOOL32 CMpData::IsPortNeedReverseG7221cData( u16 wRcvPort )
{
	for (u16 wIdx=0; wIdx<MAXnum_ReverseG7221cPort; wIdx++)
	{
		if (m_wReverseG7221cPort[wIdx] == 0)
		{
			// ����������FALSE
			return FALSE;
		}
		if (m_wReverseG7221cPort[wIdx] == wRcvPort)
		{
			// ���ڣ�����TRUE
			return TRUE;
		}
	}

	return FALSE;
}

/*=============================================================================
  �� �� ���� RemoveReverseG7221cPort
  ��    �ܣ� �����Ҫ��g7221.c��Ƶ��ʽ����������ż�Ի��Ķ˿�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wRcvPort
  �� �� ֵ�� void
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2013/11/14    4.0			�־                  ����
=============================================================================*/
void CMpData::RemoveReverseG7221cPort( u16 wRcvPort )
{
	u16 wRemovePos = MAXnum_ReverseG7221cPort;
	u16 wLastPos = MAXnum_ReverseG7221cPort-1;
	for (u16 wIdx=0; wIdx<MAXnum_ReverseG7221cPort; wIdx++)
	{
		if (m_wReverseG7221cPort[wIdx] == 0)
		{
			// ���ҽ���
			wLastPos = wIdx - 1;
			break;
		}

		if (m_wReverseG7221cPort[wIdx] == wRcvPort)
		{
			wRemovePos = wIdx;
		}
	}

	// �ҵ�Ҫ�����λ�ã����ڴ�ǰ��
	if (wRemovePos < MAXnum_ReverseG7221cPort)
	{
		u16 wTmpBuf[MAXnum_ReverseG7221cPort];
		memset(wTmpBuf, 0, sizeof(wTmpBuf));
		memcpy(wTmpBuf, m_wReverseG7221cPort, wRemovePos);//����ǰ�벿��
		u16 wCopyLen = MAXnum_ReverseG7221cPort - wRemovePos - 1;
		memcpy(&wTmpBuf[wRemovePos], &m_wReverseG7221cPort[wRemovePos+1], wCopyLen);//���ƺ�벿��
		memcpy(m_wReverseG7221cPort, wTmpBuf, sizeof(m_wReverseG7221cPort));
	}
}

/*=============================================================================
  �� �� ���� GetMtNumNeedT
  ��    �ܣ� ��ȡ�Կ��ܴ�T�ڵ�ת���������ն���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfIdx
  �� �� ֵ�� u8
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/05/13    4.0			�ű���                ����
=============================================================================*/
u8 CMpData::GetMtNumNeedT( u8 byConfIdx )
{
    if ( byConfIdx >= MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[GetMtNumNeedT] param err: confIdx.%d \n", byConfIdx );
        return 0;
    }
    return m_byMtNumNeedT[byConfIdx];
}

/*=============================================================================
  �� �� ���� SetMtNumNeedT
  ��    �ܣ� ���� �Կ��ܴ�T�ڵ�ת���������ն���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfIdx
             u8 byMtNum
  �� �� ֵ�� void
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/05/13    4.0			�ű���                ����
=============================================================================*/
void CMpData::SetMtNumNeedT( u8 byConfIdx, u8 byMtNum )
{
	/*lint -save -e1788*/
    ENTER( m_hMtNum )

    if ( byConfIdx >= MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[SetMtNumNeedT] param err: confIdx.%d \n", byConfIdx );
        return;
    }
    m_byMtNumNeedT[byConfIdx] = byMtNum;

    return;
	/*lint -restore*/
}

/*=============================================================================
  �� �� ���� SendMsg2Mcu
  ��    �ܣ� ����Ϣ��Mcu
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16      wEvent
             CServMsg &cServMsg
  �� �� ֵ�� void 
=============================================================================*/
void CMpData::SendMsg2Mcu( u16 wEvent, CServMsg &cServMsg )
{
    if( OspIsValidTcpNode(m_dwMcuNode) || m_bEmbedA )
    {
        OspPost( m_dwMcuIId, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), m_dwMcuNode );
    }
    
    if( OspIsValidTcpNode(m_dwMcuNodeB)  || m_bEmbedB )
    {
        OspPost( m_dwMcuIIdB, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), m_dwMcuNodeB );
    }
    
    return;
}

/*=============================================================================
  �� �� ���� IsPortSrcToT
  ��    �ܣ� �Ƿ�Ϊ��ת�ڵ�ĺϷ�PORT
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wRcvPort
  �� �� ֵ�� BOOL32
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/18    4.0			�ű���                  ����
=============================================================================*/
void CMpData::ShowHandle()
{
    OspPrintf( TRUE, FALSE, "PtStateHandle.0x%x \n", m_hPtInfo );
    OspPrintf( TRUE, FALSE, "PtTickHandle.0x%x \n",  m_hPtTick );
    OspPrintf( TRUE, FALSE, "PtMtNumHandle.0x%x \n", m_hMtNum );
}

/*=============================================================================
�� �� ���� AddSendMutePack
��    �ܣ� ���һ����Ҫ���Լ����������ı���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TSendMutePack
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2012/09/26    4.7			zhouyiliang                  ����
=============================================================================*/
void CMpData::AddSendMutePack(const TSendMutePack& tSendMutePack)
{
	if ( m_wNeedSendMutePackNum == MAXNUM_CONF_MT  || NULL == m_ptSendMutePackTable )
	{
		MpLog(LOG_CRIT,"[AddSendMutePack]SendMutePackTable is full or m_ptSendMutePackTable is null!\n");
		return;
	}
	m_ptSendMutePackTable[m_wNeedSendMutePackNum++] = tSendMutePack; 
}


/*lint -save -e850*/
/*=============================================================================
�� �� ���� DelSendMutePackByConfIdx
��    �ܣ� ɾ����������mtid��Ӧ���mtidΪ0ɾ�����������������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byConfIdx:��������confidx��Ϊ��ʵ��idx����-1��ģ�
u8 byMtid = 0��Ҫɾ��Ӧ�ĸ�mtid����
�� �� ֵ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2012/09/26    4.7			zhouyiliang                  ����
=============================================================================*/
void CMpData::DelSendMutePackByConfIdx(u8 byConfIdx,u8 byMtid )
{
	if( m_wNeedSendMutePackNum == 0 )
	{
		MpLog(LOG_INFO,"[DelSendMutePackByConfIdx]SendMutePackTable current no content!\n");
		return;
	}
	for(u16 wLoop = 0 ; wLoop < m_wNeedSendMutePackNum ; wLoop++)
	{
		if (NULL != m_ptSendMutePackTable && m_ptSendMutePackTable[wLoop].m_byConfIdx  == byConfIdx  )
		{
			if (0 == byMtid )//ɾ����������������ն˶�Ӧ����
			{
				//�����һ���Ƶ���ǰ�ͬʱ�´�ѭ���ӵ�ǰ�ʼ
				//�Ȳ������
				//::dsRemove(g_cMpApp.m_nInitUdpHandle,htonl(m_ptSendMutePackTable[wLoop].GetDstIp()),m_ptSendMutePackTable[wLoop].GetDstPort());
				TSendMutePack tLastSendMutePack = m_ptSendMutePackTable[m_wNeedSendMutePackNum - 1];
				m_ptSendMutePackTable[wLoop] = tLastSendMutePack;
				
				m_ptSendMutePackTable[m_wNeedSendMutePackNum - 1].Clear();
				wLoop--;
				m_wNeedSendMutePackNum--;
			}
			else if ( m_ptSendMutePackTable[wLoop].m_byMtId == byMtid )
			{
				//::dsRemove(g_cMpApp.m_nInitUdpHandle,htonl(m_ptSendMutePackTable[wLoop].GetDstIp()),m_ptSendMutePackTable[wLoop].GetDstPort());
				//�����һ���Ƶ���ǰ�ͬʱ�´�ѭ���ӵ�ǰ�ʼ
				TSendMutePack tLastSendMutePack = m_ptSendMutePackTable[m_wNeedSendMutePackNum - 1];
				m_ptSendMutePackTable[wLoop] = tLastSendMutePack;
				m_ptSendMutePackTable[m_wNeedSendMutePackNum - 1].Clear();
				wLoop--;
				m_wNeedSendMutePackNum--;
				break;
			}
			
		}
	}
}
/*lint -restore*/

/*=============================================================================
�� �� ���� IsAudioType
��    �ܣ� �ж�ý�������Ƿ�Ϊ��Ƶ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byMediaType:ý������

  �� �� ֵ��
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2012/09/26    4.7			zhouyiliang                  ����
=============================================================================*/
BOOL32 CMpData::IsAudioType(u8 byMediaType )
{
	if( (byMediaType == MEDIA_TYPE_PCMU) ||	(byMediaType == MEDIA_TYPE_PCMA) ||
		(byMediaType == MEDIA_TYPE_G728) || (byMediaType == MEDIA_TYPE_G722) ||
		(byMediaType == MEDIA_TYPE_G729) || (byMediaType == MEDIA_TYPE_G7231) ||
		(byMediaType == MEDIA_TYPE_ADPCM) || (byMediaType == MEDIA_TYPE_G7221C) ||
		(byMediaType == MEDIA_TYPE_MP3) || (byMediaType == MEDIA_TYPE_AACLC) ||
		(byMediaType == MEDIA_TYPE_AACLD) || (byMediaType == MEDIA_TYPE_G719) || 
		(byMediaType == MEDIA_TYPE_G721) || (byMediaType == MEDIA_TYPE_G7221)
		)
	{
		return TRUE;
	}
	return FALSE;
}

/*=============================================================================
�� �� ���� PrintSendMuteTable
��    �ܣ� ��ӡ������������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 

  �� �� ֵ��
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2012/09/26    4.7			zhouyiliang                  ����
=============================================================================*/
void CMpData::PrintSendMuteTable( )
{
	for (u16 wLoop = 0 ;wLoop < m_wNeedSendMutePackNum ; wLoop++)
	{
		OspPrintf(TRUE,FALSE,"m_atSendMutePackTable[%d]:\n",wLoop);
		if (NULL != m_ptSendMutePackTable)
		{
			m_ptSendMutePackTable[wLoop].OspPrint();
		}
		OspPrintf(TRUE,FALSE,"\n");
	}
}

//END OF FILE
