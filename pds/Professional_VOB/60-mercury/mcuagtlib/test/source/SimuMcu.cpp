

// SimuMcu.cpp
#include "simumcu.h"

CSimuMcuApp g_cSimuMcuApp;

CSimuMcu::CSimuMcu()
{
    m_dwAgentNode = 0;
    m_dwAgentIns  = 0;
    m_dwBrdIns    = 0;
    m_dwBrdNode   = 0;
    m_dwLastEvent = 0;
}

CSimuMcu::~CSimuMcu()
{
    m_dwAgentNode = 0;
    m_dwAgentIns  = 0;
    m_dwBrdIns    = 0;
    m_dwBrdNode   = 0;

}

/*=============================================================================
  �� �� ���� InstanceEntry
  ��    �ܣ� ģ��Mcu���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
=============================================================================*/
void CSimuMcu::InstanceEntry(CMessage* const pMsg )
{
    CServMsg    cServMsg(pMsg->content, pMsg->length);
    char achAckBuf[512] = {0};

    switch(pMsg->event)
    {
    case EV_REGISTER_VPU:
        memcpy(&m_tBoardPosition, pMsg->content, sizeof(TBrdPosition));
        break;

    case AGENT_MCU_REGISTER_NOTIFY:
        ProcBrdRegInfo(cServMsg);
        break;
    
    case GETINFO_FROM_MCU:
        memcpy(achAckBuf, &m_dwLastEvent, sizeof(u32));
        SetSyncAck( achAckBuf, sizeof(u32) );
        break;

    case BRDTO_ERROR_TEST:
        post(MAKEIID(AID_MCU_BRDMGR, 1), BOARD_MPC_BIT_ERROR_TEST_CMD);  // �����������
        post(MAKEIID(APP_SIMUBRDSSN, 1), pMsg->event);  // ����ģ�ⵥ��
        break;

    case BRDTO_TIMESYNC_TEST:
        post(MAKEIID(AID_MCU_BRDMGR, 1), BOARD_MPC_TIME_SYNC_CMD);  // �����������
        post(MAKEIID(APP_SIMUBRDSSN, 1), pMsg->event);  // ����ģ�ⵥ��
        break;

    case BRDTO_E1LOOP_TEST:
        post(MAKEIID(AID_MCU_BRDMGR, 1), BOARD_MPC_E1_LOOP_CMD);  // �����������
        post(MAKEIID(APP_SIMUBRDSSN, 1), pMsg->event);  // ����ģ�ⵥ��
        break;

    case BRDTO_ERROR_TESTACK:
    case BRDTO_ERROR_TESTNACK:
    case BRDTO_TIMESYNC_TESTACK:
    case BRDTO_TIMESYNC_TESTNACK:
    case BRDTO_E1LOOP_TESTACK:
    case BRDTO_E1LOOP_TESTNACK:
        m_dwLastEvent = pMsg->event;
        OspSemGive( g_SimuSem );
        break;

    case MCU_NMS_SETRECCFG:
    case MCU_NMS_SETBASCFG:
    case MCU_NMS_SETMIXCFG:
    case MCU_NMS_SETPRSCFG:
    case MCU_NMS_SETTVWALLCFG:
    case MCU_NMS_SETVMPCFG:

        post(MAKEIID(APP_SIMUNMSSSN, 1), pMsg->event);
        break;
        
    // �Ժ���չ��
    case MCU_NMS_SETLOCALCFG:
	case MCU_NMS_SETNETWORK:
	case MCU_NMS_SETBRDINFO:
	case MCU_NMS_SETTRAPINFO:
	case MCU_NMS_SETQOSCFG: 
        break;

    default:
        break;
    }
    return;
    
}

/*=============================================================================
  �� �� ���� ProcBrdRegInfo
  ��    �ܣ� ������ע��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CServMsg cServMsg
  �� �� ֵ�� void 
=============================================================================*/
void CSimuMcu::ProcBrdRegInfo(CServMsg cServMsg)
{
    BOOL bFlag = FALSE;
    TBrdPosition tBrdPosition;
    memcpy(&tBrdPosition, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
    
//    if(tBrdPosition.byBrdID == m_tBoardPosition.byBrdID && tBrdPosition.byBrdLayer == m_tBoardPosition.byBrdLayer 
//                             && tBrdPosition.byBrdSlot == m_tBoardPosition.byBrdSlot)
//    {
//        bFlag = TRUE;
//    }
    
//    CPPUNIT_ASSERT(TRUE == bFlag);
	return;
}


