#include "apu2basinst.h"

/*====================================================================
������      ��CApu2BasInst
����        ��CApu2BasInst����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013.03/28	2.0			������		  ����
====================================================================*/
CApu2BasInst::CApu2BasInst():m_pcEqp(NULL)
{
	
}

/*====================================================================
������      ��~CApu2BasInst
����        ��CApu2BasInst����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/28  2.0         ������        ����
====================================================================*/
CApu2BasInst::~CApu2BasInst()
{
	SAFE_DELETE(m_pcEqp);
}

/*====================================================================
������      ��DaemonInstanceEntry
����        ��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage * const pcMsg
		      CApp*            pcApp
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/28	2.0			������		  ����
====================================================================*/
void CApu2BasInst::DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp )
{
	PTR_NULL_VOID(pcMsg);
	PTR_NULL_VOID(pcApp);

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"BAS: [CApu2BasInst::DaemonInstanceEntry] Received MSG %u(%s).\n", pcMsg->event, ::OspEventDesc(pcMsg->event));

	switch(pcMsg->event)
	{
	case EV_BAS_INIT:
		DaemonProcInit(pcMsg);
		break;

	case EV_BAS_SHOWBAS:
	case EV_BAS_SHOWCHINFO:
		DaemonProcPrint(pcMsg);
		break;

	default:
		break;
	}
}

/*====================================================================
������      ��DaemonProcInit
����        ��Daemonʵ����������Ͷ�ݸ�CommonInst��������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage * const pcMsg 
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/28	2.0			������		  ����
====================================================================*/
void CApu2BasInst::DaemonProcInit(CMessage * const pcMsg)
{
	PTR_NULL_VOID(pcMsg);
	
	TApu2EqpCfg* ptApu2Cfg = (TApu2EqpCfg*)pcMsg->content;
	
	for (u8 byIdx = 0; byIdx < ptApu2Cfg->m_byBasNum ; byIdx++ )
    {
		post(MAKEIID(GetAppID(),byIdx+1), EV_BAS_INIT, (u8*)&(ptApu2Cfg->m_acBasCfg[byIdx]), sizeof(CApu2BasCfg));
    }
}

/*====================================================================
������      ��InstanceEntry
����        ��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage * const pcMsg
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/28  2.0         ������        ����
====================================================================*/
void CApu2BasInst::InstanceEntry( CMessage * const pcMsg )
{
	PTR_NULL_VOID(pcMsg);
	
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"BAS: [CApu2BasInst::InstanceEntry] BAS.%d Received MSG [%u](%s).\n", GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event));

	if (EV_BAS_INIT == pcMsg->event)
	{
		ProcInit(pcMsg);
	}
	else
	{
		//��-->��
		TransMcuMsgToEqpBaseMsg(pcMsg);

		PTR_NULL_VOID(m_pcEqp);
		m_pcEqp->OnMessage(pcMsg);
	}
}

/*====================================================================
������      ��ProcInit
����        ���������ʼ��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage * const pcMsg 
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
void CApu2BasInst::ProcInit(CMessage * const pcMsg)
{
	PTR_NULL_VOID(pcMsg);
	
	//����������
	CApu2BasCfg* pApu2BasCfg = (CApu2BasCfg*)(pcMsg->content);
	PTR_NULL_VOID(pApu2BasCfg);

	//ʵ����Apu2����
	/*lint -save -e429*/
	if (NULL == m_pcEqp)
	{	
		CApu2BasCfg* pcCfg = new CApu2BasCfg();
		PTR_NULL_VOID(pcCfg);

		//��������
		pcCfg->Copy(pApu2BasCfg);
		pcCfg->m_wEqpVer = DEVVER_APU2;

		//��ʼ��1��ͨ������
		m_pcEqp = new CApu2BasEqp((CEqpMsgSender*)this, pcCfg, MAXNUM_BASCHN);

		if (NULL == m_pcEqp)
		{
			SAFE_DELETE(pcCfg);
			LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "BAS: [CApu2BasInst::ProcInit][error] CreatEqp failed!\n");
			return;
		}
	}
	/*lint -restore*/

	//�����ʼ��
	m_pcEqp->Init();

	return;
}

/*====================================================================
������      : PostEqpMsg
����        ����Ŀ�ķ���Ϣ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const u32 dwMcuNode    Tcp���
			  const u32 dwDstIId     Ŀ��IID
			  CServMsg& cMsg	     ��Ϣ��
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
void CApu2BasInst::PostEqpMsg( const u32 dwMcuNode, const u32 dwDstIId, CServMsg& cMsg,const BOOL32 bIsSendHeadInfo /*= TRUE*/ )
{
	//��-->��
	TransEqpBaseMsgToMcuMsg(cMsg);

	if (bIsSendHeadInfo) 
	{
		post(dwDstIId, cMsg.GetEventId(), cMsg.GetServMsg(), cMsg.GetServMsgLen(), dwMcuNode);
	}
	else
	{
		post(dwDstIId, cMsg.GetEventId(), cMsg.GetMsgBody(), cMsg.GetMsgBodyLen(), dwMcuNode);
	}
	
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "BAS: [CApu2BasInst::PostEqpMsg] BAS.%d Post MSG [%u](%s)(%d).\n", 
							CInstance::GetInsID(), cMsg.GetEventId(), ::OspEventDesc(cMsg.GetEventId()),bIsSendHeadInfo);
}	

/*====================================================================
������      ��SetTimer
����        ��Osp���ö�ʱ��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u32  dwTimerId       �¼���
              long nMilliSeconds   ʱ����
			  u32  dwPara		   Я������
����ֵ˵��  ��int
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
int CApu2BasInst::SetTimer( u32 dwTimerId, long nMilliSeconds, u32 dwPara )
{
	return CInstance::SetTimer(dwTimerId, nMilliSeconds, dwPara);
}

/*====================================================================
������      ��KillTimer
����        ��Ospֹͣ��ʱ��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u32 dwTimerId �¼���
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
int CApu2BasInst::KillTimer(u32 dwTimerId)
{
	return CInstance::KillTimer(dwTimerId);
}

/*====================================================================
������      ��DisConRegister
����        ��ע�����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u32 dwMcuNode  Tcp���
����ֵ˵��  ��int
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
int CApu2BasInst::DisConRegister( u32 dwMcuNode )
{
	return ::OspNodeDiscCBRegQ(dwMcuNode, CInstance::GetAppID(), CInstance::GetInsID());
}

/*====================================================================
������      ��IsValidTcpNode
����        ��У��Tcp�����Ч��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const u32 dwTcpNode Tcp���
����ֵ˵��  ��TRUE OR FALSE
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
BOOL32 CApu2BasInst::IsValidTcpNode(const u32 dwTcpNode)
{
	return OspIsValidTcpNode(dwTcpNode);
}

/*====================================================================
������      : DisconnectTcpNode
����        ���Ͽ�һ��Tcp�������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const u32 dwTcpNode Tcp���
����ֵ˵��  ��TRUE OR FALSE
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
BOOL32 CApu2BasInst::DisconnectTcpNode(const u32 dwTcpNode)
{
	return OspDisconnectTcpNode(dwTcpNode);
}

/*====================================================================
������      ��ConnectTcpNode
����        ������TCP����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const u32 dwConnectIP  ����Զ��IP
			  const u16 wConnectPort ����Զ�˶˿�
����ֵ˵��  ��int
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
int CApu2BasInst::ConnectTcpNode(const u32 dwConnectIP, const u16 wConnectPort)
{
	return OspConnectTcpNode(dwConnectIP, wConnectPort);
}

/*====================================================================
������      ��GetInsID
����        ��Osp��ȡʵ����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��u16  Instanceʵ����
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
u16 CApu2BasInst::GetInsID()
{
	return CInstance::GetInsID();
}

/*====================================================================
������      ��TransMcuMsgToEqpBaseMsg
����        ����mcu��������Ϣת����EqpBase��Ϣ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage * const pcMsg
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  �޸�
====================================================================*/
void CApu2BasInst::TransMcuMsgToEqpBaseMsg(CMessage * const pcMsg)
{
	PTR_NULL_VOID(pcMsg);
	
	switch (pcMsg->event)
	{
	case MCU_BAS_REG_ACK:
		pcMsg->event = MCU_EQP_REG_ACK;
		 break;

	case MCU_BAS_REG_NACK:
		pcMsg->event = MCU_EQP_REG_NACK;
	 	 break;
		 
	case MCU_EQP_GETMSSTATUS_ACK:
		pcMsg->event = MCU_EQP_GETMSSTAT_ACK;
		 break;

	case MCU_EQP_GETMSSTATUS_NACK:
		pcMsg->event = MCU_EQP_GETMSSTAT_NACK;
	 	 break;

	case MCU_EQP_MODSENDADDR_CMD:
		pcMsg->event = MCU_EQP_MODIFYADDR_CMD;
		break;

	default:
		break;
	}
	return;
}

/*====================================================================
������      ��TransEqpBaseMsgToMcuMsg
����        ��EqpBase��Ϣת����mcu������ͨ����Ϣ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage * const pcMsg
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
void CApu2BasInst::TransEqpBaseMsgToMcuMsg(CServMsg& cMsg)
{
	switch(cMsg.GetEventId())
	{
	case EQP_MCU_REG_REQ:
		{
			cMsg.SetEventId(BAS_MCU_REG_REQ);
			break;
		}
	case EQP_MCU_GETMSSTAT_REQ:
		{
			cMsg.SetEventId(EQP_MCU_GETMSSTATUS_REQ);
			break;
		}

	default:
		break;
	}
	return;
}

/*====================================================================
������      ��DaemonProcPrint
����        ��ͨ�ô�ӡ����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage * const pcMsg
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
void CApu2BasInst::DaemonProcPrint(CMessage * const pcMsg)
{
	PTR_NULL_VOID(pcMsg);

	u8 byInsID = *(u8*)(pcMsg->content);
	if (byInsID > MAXNUM_APU2_BAS)
	{
		return;
	}
	u8 byPrtIns = (byInsID == 0)? MAXNUM_APU2_BAS : byInsID;
	u8 byIns = (byInsID == 0)? 1 : byInsID;

	for (byIns; byIns <= byPrtIns; byIns++)
	{
		post(MAKEIID(GetAppID(), byIns), pcMsg->event);
	}
}
