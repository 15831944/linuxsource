

// SimuNMS.cpp
#include "SimuNMS.h"

CSimuNmsApp g_cSimuNmsApp;

CSimuNMS::CSimuNMS()
{
    m_dwMcuIns = 0;
    m_dwMcuNode = 0;
    m_dwBrdIns = 0;
    m_dwBrdNode = 0;
    m_dwAgentIns = 0;
    m_dwAgentNode = 0;
}
CSimuNMS::~CSimuNMS()
{
}

/*=============================================================================
  �� �� ���� InstanceEntry
  ��    �ܣ� ģ��Nms���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pMsg
  �� �� ֵ�� void 
=============================================================================*/
void CSimuNMS::InstanceEntry( CMessage* const pMsg )
{
    CServMsg    cServMsg(pMsg->content, pMsg->length);
    char achAckBuf[512] = {0};

    switch(pMsg->event)
    {

    case SVC_AGT_MEMORY_STATUS:         // �澯
        ProcMemAlarm();
        break;

    case SVC_AGT_FILESYSTEM_STATUS:      //�ļ�ϵͳ״̬�ı�
        ProcFileAlarm();
        break;

	case BOARD_MODULE_STATUS: 
        ProcModuleAlarm();
        break;

    case BOARD_LED_STATUS:
        ProcLedAlarm();
        break;

    default:
        break;
        
            
    }

    return;

}

/*=============================================================================
  �� �� ���� ProcFileAlarm
  ��    �ܣ� �����ļ��澯
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CSimuNMS::ProcFileAlarm()
{
    post(MAKEIID(APP_SIMUBRDSSN, 1), NMS_REV_FILESYSALARM);
    return;
}

/*=============================================================================
  �� �� ���� ProcLedAlarm
  ��    �ܣ� ����Led�澯
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CSimuNMS::ProcLedAlarm()
{
    post(MAKEIID(APP_SIMUBRDSSN, 1), NMS_REV_LEDALARM);
    return;

}

/*=============================================================================
  �� �� ���� ProcMemAlarm
  ��    �ܣ� �����ڴ�澯
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CSimuNMS::ProcMemAlarm()
{
    post(MAKEIID(APP_SIMUBRDSSN, 1), NMS_REV_MEMALARM);
    return;
}

/*=============================================================================
  �� �� ���� ProcModuleAlarm
  ��    �ܣ� ����ģ��澯
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CSimuNMS::ProcModuleAlarm()
{
    post(MAKEIID(APP_SIMUBRDSSN, 1), NMS_REV_MODULEALARM);
    return;
}