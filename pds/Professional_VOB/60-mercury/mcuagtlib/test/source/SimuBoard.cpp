
// simuBoard.cpp
#include "SimuBoard.h"


CSimuBoardApp g_cSimuBrdApp;

CSimuBoard::CSimuBoard()
{
    m_dwTestEvent = 0;
}

CSimuBoard::~CSimuBoard()
{
}

/*=============================================================================
  �� �� ���� InstanceEntry
  ��    �ܣ� ģ�ⵥ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
=============================================================================*/
void CSimuBoard::InstanceEntry( CMessage* const pMsg )
{
    CServMsg    cServMsg(pMsg->content, pMsg->length);
    BOOL bFlag = FALSE;
    char achAckBuf[512] = {0};

    switch(pMsg->event)
    {
    case BOARD_POWERON:
        {
            bFlag = InitBoard();
            if( bFlag == FALSE)
            {
                OspPrintf(TRUE, FALSE, "[SimuBrd]Fail to inital the board\n");
                return;
            }
        }
        break;

    case GETINFO_FROM_BRD:
        memcpy(achAckBuf, &m_dwLastEvent, sizeof(u32));
        SetSyncAck( achAckBuf, sizeof(u32) );
        break;
        
    case BOARD_MPC_BIT_ERROR_TEST_CMD:
    case BOARD_MPC_TIME_SYNC_CMD:
    case BOARD_MPC_E1_LOOP_CMD:
        m_dwTestEvent = pMsg->event;
        break;

    case BRDTO_ERROR_TEST:
        ProcErrorTest(pMsg->event);
        break;

    case BRDTO_TIMESYNC_TEST:
        ProcTimeSync(pMsg->event);
        break;

    case BRDTO_E1LOOP_TEST:
        ProcE1Loop(pMsg->event);
        break; 
        
    case BOARD_MPC_REG:
    case BOARD_MPC_GET_CONFIG:
        post( MAKEIID(AID_MCU_BRDMGR,1), pMsg->event, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        break;

    case SVC_AGT_MEMORY_STATUS:         // �澯
    case SVC_AGT_FILESYSTEM_STATUS:      //�ļ�ϵͳ״̬�ı�
	case BOARD_MODULE_STATUS: 
    case BOARD_LED_STATUS:
        post( MAKEIID(AID_MCU_BRDMGR,1), BOARD_MPC_ALARM_NOTIFY, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()); // ֪ͨ�������
        post( MAKEIID(APP_SIMUNMSSSN, 1), pMsg->event);     // ֪ͨģ��NMS
        break;

    case NMS_REV_MEMALARM:            // guard report
    case NMS_REV_FILESYSALARM:      // guard report
    case NMS_REV_MODULEALARM:       // guard report
    case NMS_REV_LEDALARM:          // guard report
        m_dwTestEvent = pMsg->event;
        OspSemGive( g_SimuSem );
        break;
        
    case BOARD_MPC_REG_ACK:
    case BOARD_MPC_REG_NACK:
    case BOARD_MPC_GET_CONFIG_ACK:
    case BOARD_MPC_GET_CONFIG_NACK:
        m_dwTestEvent = pMsg->event;
        OspSemGive( g_SimuSem );
        break;

    default:
        break;
    }
    return ;
}

/*=============================================================================
  �� �� ���� ProcErrorTest
  ��    �ܣ� ����Error��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 wEvent
  �� �� ֵ�� void 
=============================================================================*/
void CSimuBoard::ProcErrorTest(u32 wEvent)
{
    if(m_dwTestEvent == BOARD_MPC_BIT_ERROR_TEST_CMD)
    {
        post(MAKEIID(APP_SIMUMCUSSN, 1), wEvent+1);
    }
    else
    {
        post(MAKEIID(APP_SIMUMCUSSN, 1), wEvent+2);

    }
    return;
}

/*=============================================================================
  �� �� ���� ProcTimeSync
  ��    �ܣ� ����ʱ��ͬ����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 wEvent
  �� �� ֵ�� void 
=============================================================================*/
void CSimuBoard::ProcTimeSync(u32 wEvent)
{
    if(m_dwTestEvent == BOARD_MPC_TIME_SYNC_CMD)
    {
        post(MAKEIID(APP_SIMUMCUSSN, 1), wEvent+1);
    }
    else
    {
        post(MAKEIID(APP_SIMUMCUSSN, 1), wEvent+2);
    }
    return;
}

/*=============================================================================
  �� �� ���� ProcE1Loop
  ��    �ܣ� ����E1�Ի���Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 wEvent
  �� �� ֵ�� void 
=============================================================================*/
void CSimuBoard::ProcE1Loop(u32 wEvent)
{
    if(m_dwTestEvent == BOARD_MPC_E1_LOOP_CMD)
    {
        post(MAKEIID(APP_SIMUMCUSSN, 1), wEvent+1);
    }
    else
    {
        post(MAKEIID(APP_SIMUMCUSSN, 1), wEvent+2);
    }
    return;
}

/*=============================================================================
  �� �� ���� InitBoard
  ��    �ܣ� ��ʼ������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL 
=============================================================================*/
BOOL CSimuBoard::InitBoard()
{
	CServMsg cReportMsg;
    u32 m_dwBrdIp = 0x01010101;  // Ip: 1.1.1.1
    u8  m_byChoice = 1;

    TBrdPosition    m_tBoardPosition;
    m_tBoardPosition.byBrdID = 6;
    m_tBoardPosition.byBrdLayer = 0;
    m_tBoardPosition.byBrdSlot  = 3;

    cReportMsg.SetMsgBody((u8*)&m_tBoardPosition, sizeof(TBrdPosition));
    cReportMsg.CatMsgBody((u8*)&m_dwBrdIp, sizeof(u32));
    cReportMsg.CatMsgBody(&m_byChoice, sizeof(u8));

    post( MAKEIID(AID_MCU_BRDMGR,1), BOARD_MPC_REG, cReportMsg.GetServMsg(), cReportMsg.GetServMsgLen());

    return TRUE;
}