/*=============================================================================
  �� �� ���� SimuSnmpLib.cpp
  ��    �ܣ� ģ��SnmpLib
  �㷨ʵ�֣� 
  ȫ�ֱ����� 

=============================================================================*/
// SimuSnmpLib.cpp
#include "testevent.h"
#include "evagtsvc.h"
#include "simuSnmplib.h"

TAgentCallBack g_AgentCallBack;


/*=============================================================================
  �� �� ���� Initialize
  ��    �ܣ� ģ��Snmp��ʼ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TSnmpAdpParam * ptSnmpAdpParam /* = NULL */
  �� �� ֵ�� void
=============================================================================*/
void CAgentAdapter::Initialize(TAgentCallBack tSnmpCallBack, TSnmpAdpParam * ptSnmpAdpParam /* = NULL */)
{
    return;
}

/*=============================================================================
  �� �� ���� SetCommunity
  ��    �ܣ� ���ù�ͬ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� s8 * pchCommunity
             u8 byCommunityType
  �� �� ֵ�� void 
=============================================================================*/
void CAgentAdapter::SetCommunity(s8 * pchCommunity, u8 byCommunityType)
{
    return;
}

/*=============================================================================
  �� �� ���� SetAgentCallBack
  ��    �ܣ� ���ûص�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TAgentCallBack tSnmpCallBack
  �� �� ֵ�� void 
=============================================================================*/
void CAgentAdapter::SetAgentCallBack(TAgentCallBack tSnmpCallBack)
{
    
    g_AgentCallBack = tSnmpCallBack;
    m_tAgentCallBack = tSnmpCallBack;
    return;
    
}

/*=============================================================================
  �� �� ���� SendTrap
  ��    �ܣ� ��Trap
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TTarget tTrapRcvAddr
             CNodes & cNodes
  �� �� ֵ�� void 
=============================================================================*/
void CAgentAdapter::SendTrap(TTarget tTrapRcvAddr, CNodes & cNodes)
{
    u32  dwNodes = 0;
    
    u16  wTrapType = GetTrapType(cNodes);
    switch(wTrapType)
    {

    case BOARD_LED_STATUS:
    case BOARD_MODULE_STATUS:
    case SVC_AGT_FILESYSTEM_STATUS:
    case SVC_AGT_MEMORY_STATUS:
        // �澯ת����ģ��Nms
        post(MAKEIID(APP_SIMUNMSSSN,1), wTrapType);
        break;

    default:
        break;
    }
}

u16 CAgentAdapter::GetTrapType(CNodes cNodes)
{
    return cNodes.GetTrapType();
}