#include "apu2baseqp.h"

/*====================================================================
������      ��CApu2BasEqp
����        ��CApu2BasEqp����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CEqpMsgSender* const pcMsgSender  ��Ϣ���Ͷ����
			  CEqpCfg* const       pcCfg        ���ð�
			  const u16            wChnNum      ����ͨ����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
CApu2BasEqp::CApu2BasEqp(CEqpMsgSender* const pcMsgSender, CEqpCfg* const pcCfg, const u16 wChnNum)
	:CEqpBase(pcMsgSender, pcCfg), m_bInited(FALSE)
{
	//Ϊͨ��ָ������ڴ�
	if(!CEqpBase::SetChnNum(wChnNum))
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "BAS: [CApu2BasEqp][error] Create BasChannelPointer Failed!\n");
		return;
	}

	CApu2BasChnnl* pcApu2BasChnnl = NULL;

	/*lint -save -e429*/
	for (u16 wLoop = 0; wLoop < wChnNum; wLoop++)
	{
		//Ϊͨ�������ڴ�
		pcApu2BasChnnl = new CApu2BasChnnl(this, pcCfg, wLoop);
		PTR_NULL_VOID(pcApu2BasChnnl);
		
		//ͨ��ָ��ָ��ͨ��
		if(!CEqpBase::InitChn( pcApu2BasChnnl, wLoop))
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "BAS: [CApu2BasEqp][error] Init CApu2BasChnnl.%d Failed\n", wLoop);
		}
	}
	/*lint -restore*/
	
	return;
}

/*====================================================================
������      ��~CApu2BasEqp
����        ��CApu2BasEqp����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
CApu2BasEqp::~CApu2BasEqp()
{
	CApu2BasEqp::Destroy();
}

/*====================================================================
������      ��Init
����        ��Apu2�����ʼ��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��TRUE OR FALSE
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
BOOL32 CApu2BasEqp::Init()
{
	if (m_bInited)
	{
		return TRUE;
	}
	
	//��ʼ��socket
	u16 wRet = KdvSocketStartup();
    if ( MEDIANET_NO_ERROR != wRet )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "BAS: [CApu2BasEqp::Init][error] KdvSocketStartup Failed!\n");
        return FALSE;
    }

	//��������ʼ��
	if(!CEqpBase::Init())
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "BAS: [CApu2BasEqp::Init][error] Init CEqpBase Failed!\n");
		return FALSE;
	}
	
	const CEqpCfg* pcEqpCfg = CEqpBase::GetEqpCfg();
	PTR_NULL_RETURN(pcEqpCfg, FALSE);

	// ��ʱ����
    if( 0 != pcEqpCfg->dwConnectIP )
    {
        SetTimer(EV_EQP_CONNECT_TIMER,  BAS_CONNETC_TIMEOUT, 0 );
		LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "BAS: [CApu2BasEqp::Init] Connecting To MCUA!\n");
    }

	// ��ʱ����
    if( 0 != pcEqpCfg->dwConnectIpB )
    {
        SetTimer(EV_EQP_CONNECTB_TIMER, BAS_CONNETC_TIMEOUT, 1 );
		LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "BAS: [CApu2BasEqp::Init] Connecting to MCUB!\n");
    }

	m_bInited = TRUE;
	return TRUE;
}

/*====================================================================
������      ��Destroy
����        ����������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��TRUE OR FALSE
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
BOOL32 CApu2BasEqp::Destroy()
{
	//�ر�socket
	KdvSocketCleanup();

	//��������
	CEqpBase::Destroy();

	m_bInited = FALSE;
	return TRUE;
}

/*====================================================================
������      ��OnMessage
����        ��Apu2������Ϣ����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage* const pcMsg
����ֵ˵��  ��TRUE OR FALSE
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
BOOL32 CApu2BasEqp::OnMessage(CMessage* const pcMsg)
{
	PTR_NULL_RETURN(pcMsg, FALSE);
	
	LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS, 
		"BAS: [CApu2BasEqp::OnMessage] BAS.%d Received MSG [%u](%s).\n", GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event));

	switch (pcMsg->event)
	{
	case MCU_EQP_REG_ACK:
		OnRegisterMcuAck(pcMsg);
	  	 break;

	case MCU_EQP_REG_NACK:
	    OnRegisterMcuNack(pcMsg);
	 	 break;

	case EV_BAS_SHOWBAS:
	case EV_BAS_SHOWCHINFO:
		OnBasPrint(pcMsg->event);
		break;
		
	case MCU_EQP_MODIFYADDR_CMD:
		OnModifyMcuRcvIp(pcMsg);
		break;

	default:
		{
			if (pcMsg->event != EV_EQP_CONNECT_TIMER &&
				pcMsg->event != EV_EQP_CONNECTB_TIMER &&
				pcMsg->event != EV_EQP_REGISTER_TIMER &&
				pcMsg->event != EV_EQP_REGISTERB_TIMER &&
				pcMsg->event != OSP_DISCONNECT)
			{
				CServMsg cServMsg(pcMsg->content, pcMsg->length);
				if (pcMsg->event == MCU_BAS_STARTADAPT_REQ ||
					pcMsg->event == MCU_BAS_CHANGEAUDPARAM_REQ ||
					pcMsg->event == MCU_BAS_STOPADAPT_REQ)
				{
					//��Ϣ�����׷��ͨ����(u8)
					u8 byChnIdx = cServMsg.GetChnIndex();
					cServMsg.CatMsgBody((u8*)&byChnIdx, sizeof(u8));
					pcMsg->length += sizeof(u8);
				}
				cServMsg.SetChnIndex(0);
				pcMsg->content = cServMsg.GetServMsg();
			}
			CEqpBase::OnMessage(pcMsg);
		}
		break;
	}
	return TRUE;
}

/*====================================================================
������      ��OnRegisterMcuAck
����        ��ע��Mcu�ɹ�����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage *const pcMsg
����ֵ˵��  ��TRUE OR FALSE
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
BOOL32 CApu2BasEqp::OnRegisterMcuAck(CMessage *const pcMsg)
{
	PTR_NULL_RETURN(pcMsg, FALSE);
	
	if (!CEqpBase::OnRegisterMcuAck(pcMsg))
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "BAS: [CApu2BasEqp::OnRegisterMcuAck][error] CEqpBase::OnRegisterMcuAck Failed!\n");
		return FALSE;
	}
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "BAS: [CApu2BasEqp::OnRegisterMcuAck] BAS.%d Register to MCU Success!\n", GetInsID());

	CApu2BasCfg* pBasCfg = (CApu2BasCfg*)(GetEqpCfg());
	PTR_NULL_RETURN(pBasCfg, FALSE);
	BOOL32 bIsMainBrd = TRUE;
	if (pBasCfg->m_dwMcuNodeA == pcMsg->srcnode)
	{
		bIsMainBrd = TRUE;
	}
	else if (pBasCfg->m_dwMcuNodeB == pcMsg->srcnode)
	{
		bIsMainBrd = FALSE;
	}

	u16 wChnNum = CEqpBase::GetChnNum();
	CApu2BasChnnl* pcChnnl = NULL;
	for (u16 wIdx = 0; wIdx < wChnNum; wIdx++)
	{
		pcChnnl = GetChnnl(wIdx);
		PTR_NULL_RETURN(pcChnnl, FALSE);

		if (pcChnnl->GetState() == (u8)CApu2BasChnStatus::IDLE)
		{
			pcChnnl->SetState((u8)CApu2BasChnStatus::READY);
		}
	}
	//BAS״̬ͨ��
	SendEqpStusNotify();

	//BASͨ��״̬����
	for (u8 byChIdx = 0; byChIdx < MAXNUM_APU2_BASCHN; byChIdx++)
	{
		u8 byChnlState = (pcChnnl->GetChnlOccupy(byChIdx) == TRUE) ? CApu2BasChnStatus::RUNNING : CApu2BasChnStatus::READY;
		SendChnlNotify(byChnlState, byChIdx, bIsMainBrd);
	}
	
	return TRUE;
}

/*====================================================================
������      ��OnRegisterMcuNack
����        ��ע��Mcuʧ�ܴ���
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage *const pcMsg
����ֵ˵��  ��TRUE OR FALSE
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
BOOL32 CApu2BasEqp::OnRegisterMcuNack(CMessage *const pcMsg)
{
	PTR_NULL_RETURN(pcMsg, FALSE);

	return CEqpBase::OnRegisterMcuNack(pcMsg);
}
/*====================================================================
������      ��OnModifyMcuRcvIp
����        ���޸ķ��͵�ַ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage *const pcMsg
����ֵ˵��  ��TRUE OR FALSE
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/08  2.0         ������          ����
====================================================================*/
BOOL32 CApu2BasEqp::OnModifyMcuRcvIp(CMessage* const pcMsg)
{
	PTR_NULL_RETURN(pcMsg, FALSE);

	//�޸ı���ķ���IP��ַ
	CEqpBase::OnModifyMcuRcvIp(pcMsg);
	CApu2BasChnnl* pBasChn = NULL;
	//����ȡ����ֵ
	CApu2BasCfg* pBasCfg = (CApu2BasCfg*)(GetEqpCfg());
	for (u16 wIndex = 0; wIndex < GetChnNum(); wIndex++)
	{
		pBasChn = GetChnnl(wIndex);
		PTR_NULL_CONTINUE(pBasChn);

		//�޸ķ��Ͷ���
		switch(pBasChn->GetState())
		{
		case CApu2BasChnStatus::RUNNING:
			{
				for (u8 byChIdx = 0; byChIdx < MAXNUM_APU2_BASCHN; byChIdx++)
				{
					if (FALSE == pBasChn->GetChnlOccupy(byChIdx))
					{
						continue;
					}
					for (u8 byLoop = 0; byLoop < MAXNUM_APU2BAS_OUTCHN; byLoop++)
					{
						tAudBasInfo *ptAudBasInfo = pBasChn->GetChnlInfo(byChIdx);
						if (MEDIA_TYPE_NULL == ptAudBasInfo->abyEncAudType[byLoop])
						{
							continue;
						}
						pBasChn->SetSndObjectPara(pBasCfg->m_dwMcuRcvIp, 
							pBasCfg->m_wMcuRcvStartPort + (byChIdx * MAXNUM_APU2BAS_OUTCHN + byLoop) * PORTSPAN + 2, 
							byChIdx * MAXNUM_APU2BAS_OUTCHN + byLoop);
					}
				}
			}
			break;
		default:
			break;
		}
	}
	return TRUE;
}

/*====================================================================
������      ��GetChnnl
����        : ��ȡͨ��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const u16 wChnIdx ͨ������
����ֵ˵��  ��CApu2Chnnl*       ʵ����ͨ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
CApu2BasChnnl* CApu2BasEqp::GetChnnl( const u16 wChnIdx ) 
{
	return (CApu2BasChnnl*)GetChnl(wChnIdx); 
}

/*====================================================================
������      ��SendChnlNotify
����        ������ͨ��״̬
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const u16 wChnIdx ͨ������
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
void CApu2BasEqp::SendChnlNotify(const u8 byState, const u16 wChIdx, BOOL32 bIsMainBrd)
{
	CApu2BasCfg* pcCfg = (CApu2BasCfg*)GetEqpCfg();
	PTR_NULL_VOID(pcCfg);

	CApu2BasChnnl* pcBasChnl = GetChnnl(0);

	//ͨ��״̬
	CServMsg cServMsg;
	TAudBasChnStatus tAudBasChnlStatus;
	TEqp tBas;
	tBas.SetMcuEqp(LOCAL_MCUID, pcCfg->byEqpId, pcCfg->byEqpType);
	tAudBasChnlStatus.SetEqp(tBas);
	tAudBasChnlStatus.SetChnIdx(wChIdx);
	tAudBasChnlStatus.SetStatus(byState);
	tAudBasChnlStatus.ClrOutputAudParam();
	u8 byEqpType = TYPE_APU2_BAS;
	cServMsg.SetChnIndex(wChIdx);
	cServMsg.SetConfId(pcBasChnl->GetConfId(wChIdx));
	cServMsg.SetEventId(AUDBAS_MCU_CHNNLSTATUS_NOTIF);
	cServMsg.SetMsgBody((u8 *) &byEqpType, sizeof(u8));
	cServMsg.CatMsgBody((u8 *) &tAudBasChnlStatus, sizeof(TAudBasChnStatus));

	if (bIsMainBrd == TRUE)
	{
		PostMsgToMcu(cServMsg);
	}
	else
	{
		PostMsgToMcu(cServMsg, FALSE);
	}
}

/*====================================================================
������      ��SendEqpStusNotify
����        ����������״̬
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const u16 wChnIdx ͨ������
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
void CApu2BasEqp::SendEqpStusNotify(void)
{
	CApu2BasCfg* pcCfg = (CApu2BasCfg*)GetEqpCfg();
	PTR_NULL_VOID(pcCfg);

	//�豸״̬
	CServMsg cMsg;
	TPeriEqpStatus tEqpStatus;
    tEqpStatus.SetMcuEqp(LOCAL_MCUID, pcCfg->byEqpId, pcCfg->byEqpType);
    tEqpStatus.m_byOnline = (u8)1;
	tEqpStatus.m_tStatus.tAudBas.SetEqpType(TYPE_APU2_BAS);
    tEqpStatus.SetAlias(pcCfg->achAlias);
    cMsg.SetMsgBody((u8*)&tEqpStatus, sizeof(tEqpStatus));
	cMsg.SetEventId(BAS_MCU_BASSTATUS_NOTIF);

	//��������
	PostMsgToMcu(cMsg);
	PostMsgToMcu(cMsg, FALSE);
}

/*====================================================================
������      ��OnBasPrint
����        ����ʾ��ӡ��Ϣ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��TRUE OR FALSE
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/29	2.0			������		  ����
====================================================================*/
BOOL32 CApu2BasEqp::OnBasPrint(u16 wMsgID)
{
	CApu2BasChnnl* pBasChnl = NULL;
	for (u16 wIndex = 0; wIndex < GetChnNum(); wIndex++)
	{
		pBasChnl = GetChnnl(wIndex);
		PTR_NULL_CONTINUE(pBasChnl);
		CApu2BasCfg* pBasCfg = (CApu2BasCfg*)(GetEqpCfg());
		PTR_NULL_RETURN(pBasCfg, FALSE);
		
		switch (wMsgID)
		{
		case EV_BAS_SHOWCHINFO:
			StaticLog("\n******************* BAS.[%d]'s ChnlNetInfo *******************\n", pBasChnl->GetInsID());
			pBasChnl->PrintChnlInfo();
			break;
		case EV_BAS_SHOWBAS:
			StaticLog("\n******************* BAS.[%d]'s ConfigInfo *******************\n", pBasChnl->GetInsID());
			pBasCfg->Print();
			StaticLog("\n******************* BAS.[%d]'s WorkParam ********************\n", pBasChnl->GetInsID());
			pBasChnl->Print();
		default:
			break;
		}
		StaticLog("\n*************************************************************\n");
	}
	return TRUE;
}