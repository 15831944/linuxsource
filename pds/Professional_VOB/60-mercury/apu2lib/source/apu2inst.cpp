#include "apu2inst.h"

/*====================================================================
������      : CApu2Inst
����        ��CApu2Inst����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
CApu2Inst::CApu2Inst():m_pcEqp(NULL)
{
#ifdef _8KI_
	m_bIsGetMixerCfg = FALSE;
#endif
}

/*====================================================================
������      : ~CApu2Inst
����        ��CApu2Inst����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
/*lint -save -e1551*/
CApu2Inst::~CApu2Inst()
{
	SAFE_DELETE(m_pcEqp);
}
/*lint -restore*/
/*====================================================================
������      : DaemonInstanceEntry
����        ��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage * const pcMsg
		      CApp*            pcApp
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
void CApu2Inst::DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp )
{
	PTR_NULL_VOID(pcMsg);
	PTR_NULL_VOID(pcApp);

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "MIXER (CApu2InstDaemon) Received msg %u(%s).\n", 
							pcMsg->event, ::OspEventDesc(pcMsg->event));

	switch(pcMsg->event)
	{
		case EV_MIXER_INIT:			 
			DaemonProcInit(pcMsg);
			 break;

		case EV_MIXER_SHOWMIX:
		case EV_MIXER_SHOWCHINFO:
		case EV_MIXER_SHOWSTATE:
			DaemonProcPrint(pcMsg);
			break;

#ifdef _8KI_
		case OSP_POWERON:
			DaemonProcPowerOn();
			break;
		case EV_EQP_CONNECT_TIMER:
			DaemonProcConnetTimerOut();
			break;
		case EV_EQP_REGISTER_TIMER:
			DaemonProcRegTimerOut();
			break;
		case MCU_MIXER_REG_ACK:
			DaemonProcRegAck(pcMsg);
			break;
		case MCU_MIXER_REG_NACK:
			DaemonProcRegNack();
			break;
		case OSP_DISCONNECT:
			DaemonProcOspDisconnect();
			break;
#endif

		default:
			break;
	}
}

/*====================================================================
������      : DaemonProcInit
����        ��Daemonʵ����������Ͷ�ݸ�CommonInst��������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage * const pcMsg 
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
void CApu2Inst::DaemonProcInit(CMessage * const pcMsg)
{
	PTR_NULL_VOID(pcMsg);
	
	TApu2EqpCfg* ptApu2Cfg = (TApu2EqpCfg*)pcMsg->content;

	if (ptApu2Cfg->m_bIsProductTest)
	{
		post(MAKEIID(GetAppID(), 1), EV_TEST_INIT, (u8*)&(ptApu2Cfg->m_acMixerCfg[0]),  sizeof(CApu2MixerCfg));
		return;
	}
	
	CServMsg cMsg;
	u16 wChnNum = 0;
	u8 bySndOff = 0;
	u8 byRcvOff = 0;
	for ( u8 byIdx = 0 ;byIdx < ptApu2Cfg->m_byMixerNum ; byIdx++ )
    {
		bySndOff = 0;
		byRcvOff = 0;
		cMsg.SetMsgBody();

        if ( (wChnNum += ptApu2Cfg->m_acMixerCfg[byIdx].m_byMemberNum) > MAXNUM_MIXER_CHNNL)
        {
            LogPrint( LOG_LVL_ERROR, MID_MCU_EQP, "[DaemonProcInit] Mixer.%d ChnNum.%d AllChnNum.%d > %d!\n",
									 byIdx, ptApu2Cfg->m_acMixerCfg[byIdx].m_byMemberNum, 
									 wChnNum, MAXNUM_MIXER_CHNNL);
            return;
        }
        else
        {
            if ( ptApu2Cfg->m_acMixerCfg[byIdx].m_byMemberNum >= MIXER_APU2_CHN_MINNUM)
            {
				
				for (u8 byLoop = 0; byLoop < byIdx; byLoop++)
				{
					bySndOff = bySndOff + ptApu2Cfg->m_acMixerCfg[byLoop].m_byMemberNum + 4;
					byRcvOff = byRcvOff + ptApu2Cfg->m_acMixerCfg[byLoop].m_byMemberNum;
				}
				ptApu2Cfg->m_acMixerCfg[byIdx].m_bySndOff = bySndOff;
				ptApu2Cfg->m_acMixerCfg[byIdx].m_byRcvOff = byRcvOff;
                post(MAKEIID(GetAppID(),byIdx+1), EV_MIXER_INIT, (u8*)&(ptApu2Cfg->m_acMixerCfg[byIdx]), sizeof(CApu2MixerCfg));
              
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[DaemonProcInit] Mixer.%d  ChnNum.%d SndOff.%d!\n", 
											byIdx, ptApu2Cfg->m_acMixerCfg[byIdx].m_byMemberNum, bySndOff);
            }
        }    
    }
}

/*====================================================================
������      : InstanceEntry
����        ��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage * const pcMsg
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
void CApu2Inst::InstanceEntry( CMessage * const pcMsg )
{
	PTR_NULL_VOID(pcMsg);

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "MIXER.%d (CApu2Inst) Recv msg [%u](%s).\n", 
								GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event));

	switch (pcMsg->event)
	{
	case EV_MIXER_INIT:
		ProcInit(pcMsg);
		 break;

	case EV_TEST_INIT:
		ProcTestInit(pcMsg);
	 	 break;
	default:
		{
			TransMcuMsgToEqpBaseMsg(pcMsg);
			PTR_NULL_VOID(m_pcEqp);
			m_pcEqp->OnMessage(pcMsg);
		}
		break;
	}
}

/*====================================================================
������      : ProcInit
����        ���������ʼ��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage * const pcMsg 
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
void CApu2Inst::ProcInit(CMessage * const pcMsg)
{
	PTR_NULL_VOID(pcMsg);

	//��������������
	CApu2MixerCfg* pApu2MixerCfg = (CApu2MixerCfg*)(pcMsg->content);
	PTR_NULL_VOID(pApu2MixerCfg);

	//ʵ����Apu2����
	if (NULL == m_pcEqp)
	{	
		/*lint -save -esym(429, pcCfg)*/
		/*lint -save -e438*/
		//�������ÿռ�
		CApu2MixerCfg* pcCfg = new CApu2MixerCfg();
		
		PTR_NULL_VOID(pcCfg);

		//��������
		pcCfg->Copy(pApu2MixerCfg);

		pcCfg->m_wEqpVer = pApu2MixerCfg->m_byIsSimuApu? pApu2MixerCfg->m_wSimApuVer : DEVVER_APU2;

		m_pcEqp = new CApu2Eqp((CEqpMsgSender*)this, pcCfg, MAXNUM_MIXERCHN);

		if (NULL == m_pcEqp)
		{
			SAFE_DELETE(pcCfg);
			LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcInit] Mixer.%d CreatEqp failed!\n", GetInsID());
			return;
		}
		/*lint -restore*/
		/*lint -restore*/
	}

	//�����ʼ��
	m_pcEqp->Init();

	return;
}

/*====================================================================
������      : ProcTestInit
����        ���������Գ�ʼ��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage * const pcMsg
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
void CApu2Inst::ProcTestInit(CMessage * const pcMsg)
{	
	PTR_NULL_VOID(pcMsg);

	//��������������
	CApu2MixerCfg* pApu2MixerCfg = (CApu2MixerCfg*)(pcMsg->content);
	PTR_NULL_VOID(pApu2MixerCfg);

	//ʵ����Apu2����
	if (NULL == m_pcEqp)
	{	
		/*lint -save -esym(429, pcCfg)*/
		/*lint -save -e438*/
		//�������ÿռ�
		CApu2MixerCfg* pcCfg = new CApu2MixerCfg();
		PTR_NULL_VOID(pcCfg);

		pcCfg->Copy(pApu2MixerCfg);	

		pcCfg->m_wEqpVer = DEVVER_APU2;
		
		m_pcEqp = new CTestEqp((CEqpMsgSender*)this, pcCfg, MAXNUM_MIXERCHN);

		if (NULL == m_pcEqp)
		{
			SAFE_DELETE(pcCfg);
			LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcTestInit] CreatEqp failed!\n");
			return;
		}
		/*lint -restore*/
		/*lint -restore*/
	}
	
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
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
void CApu2Inst::PostEqpMsg( const u32 dwMcuNode, const u32 dwDstIId, CServMsg& cMsg,const BOOL32 bIsSendHeadInfo /*= TRUE*/ )
{
	TransEqpBaseMsgToMcuMsg(cMsg);

	if (bIsSendHeadInfo) 
	{
		post(dwDstIId, cMsg.GetEventId(), cMsg.GetServMsg(), cMsg.GetServMsgLen(), dwMcuNode);
	}
	else
	{
		post(dwDstIId, cMsg.GetEventId(), cMsg.GetMsgBody(), cMsg.GetMsgBodyLen(), dwMcuNode);
	}

	LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER, "[CApu2Inst::PostEqpMsg] Mixer.%d Snd msg [%u](%s)(%d).\n", 
							CInstance::GetInsID(), cMsg.GetEventId(), ::OspEventDesc(cMsg.GetEventId()),bIsSendHeadInfo);
}	

/*====================================================================
������      : SetTimer
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
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
int CApu2Inst::SetTimer( u32 dwTimerId, long nMilliSeconds, u32 dwPara /* = 0 */ )
{
	return CInstance::SetTimer(dwTimerId, nMilliSeconds, dwPara);
}

/*====================================================================
������      : KillTimer
����        ��Ospֹͣ��ʱ��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u32 dwTimerId �¼���
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0          �ܼ���          ����
====================================================================*/
int CApu2Inst::KillTimer(u32 dwTimerId)
{
	return CInstance::KillTimer(dwTimerId);
}

/*====================================================================
������      : DisConRegister
����        ��ע�����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u32 dwMcuNode  Tcp���
����ֵ˵��  ��int
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
int CApu2Inst::DisConRegister( u32 dwMcuNode )
{
	return ::OspNodeDiscCBRegQ(dwMcuNode, CInstance::GetAppID(), CInstance::GetInsID());
}

/*====================================================================
������      : IsValidTcpNode
����        ��У��Tcp�����Ч��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const u32 dwTcpNode Tcp���
����ֵ˵��  ��TRUE OR FALSE
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
BOOL32 CApu2Inst::IsValidTcpNode(const u32 dwTcpNode)
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
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
BOOL32 CApu2Inst::DisconnectTcpNode(const u32 dwTcpNode)
{
	return OspDisconnectTcpNode(dwTcpNode);
}

/*====================================================================
������      : ConnectTcpNode
����        ������TCP����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const u32 dwConnectIP  ����Զ��IP
			  const u16 wConnectPort ����Զ�˶˿�
����ֵ˵��  ��int
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
int CApu2Inst::ConnectTcpNode(const u32 dwConnectIP, const u16 wConnectPort)
{
	return OspConnectTcpNode(dwConnectIP, wConnectPort);
}

/*====================================================================
������      : GetInsID
����        ��Osp��ȡʵ����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��u16  Instanceʵ����
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
u16 CApu2Inst::GetInsID()
{
	return CInstance::GetInsID();
}

/*====================================================================
������      : TransMcuMsgToEqpBaseMsg
����        ����mcu��������Ϣת����EqpBase��Ϣ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage * const pcMsg
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
void CApu2Inst::TransMcuMsgToEqpBaseMsg(CMessage * const pcMsg)
{
	PTR_NULL_VOID(pcMsg);

	switch (pcMsg->event)
	{
	case MCU_MIXER_REG_ACK:
		pcMsg->event = MCU_EQP_REG_ACK;
		 break;

	case MCU_MIXER_REG_NACK:
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

	case TEST_S_C_LOGIN_ACK:
		pcMsg->event = SERVER_EQP_REGISTER_ACK;
		 break;

	case TEST_S_C_LOGIN_NACK:
		pcMsg->event = SERVER_EQP_REGISTER_NACK;
		 break;

	default:
		break;
	}
	return;
}

/*====================================================================
������      : TransEqpBaseMsgToMcuMsg
����        ��EqpBase��Ϣת����mcu������ͨ����Ϣ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage * const pcMsg
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
void CApu2Inst::TransEqpBaseMsgToMcuMsg(CServMsg& cMsg)
{
	switch(cMsg.GetEventId())
	{
	case EQP_MCU_REG_REQ:
		{
			cMsg.SetEventId(MIXER_MCU_REG_REQ);
			break;
		}
	case EQP_MCU_GETMSSTAT_REQ:
		{
			cMsg.SetEventId(EQP_MCU_GETMSSTATUS_REQ);
			break;
		}
	case EQP_SERVER_REGISTER_REQ:
		{
			cMsg.SetEventId(TEST_C_S_LOGIN_REG);
			break;		
		}
	default:
		break;
	}
	return;
}

/*====================================================================
������      : DaemonProcShowMix
����        ����ʾ������״̬
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage * const pcMsg
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
void CApu2Inst::DaemonProcPrint(CMessage * const pcMsg)
{
	PTR_NULL_VOID(pcMsg);

	u8 byInsID = *(u8*)(pcMsg->content);
	if (byInsID > MAXNUM_APU2_MIXER)
	{
		return;
	}
	u8 byPrtIns = (byInsID == 0)? MAXNUM_APU2_MIXER : byInsID;
	u8 byIns = (byInsID == 0)? 1 : byInsID;
	for (byIns; byIns <= byPrtIns; byIns++)
	{
		post(MAKEIID(GetAppID(), byIns), pcMsg->event);
	}
}

#ifdef _8KI_
/*====================================================================
������      : DaemonProcConnetTimerOut
����        ���ϵ���Ϣ����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/04/25  1.0         ��־��          ����
====================================================================*/
void CApu2Inst::DaemonProcPowerOn()
{
	if (OSP_OK != SetTimer(EV_EQP_CONNECT_TIMER,10) )
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[DaemonInstanceEntry]SetTimer(EV_EQP_CONNECT_TIMER,10) Failed.\n");
	}
}
/*====================================================================
������      : DaemonProcConnetTimerOut
����        ��������ʱ����ʱ����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/04/25  1.0         ��־��          ����
====================================================================*/
void CApu2Inst::DaemonProcConnetTimerOut()
{
	m_dwMcuNode = ConnectTcpNode(inet_addr("127.0.0.1"),MCU_LISTEN_PORT);
	if (m_dwMcuNode == INVALID_NODE || !IsValidTcpNode(m_dwMcuNode) 
		|| OSP_OK != OspNodeDiscCBRegQ(m_dwMcuNode,CInstance::GetAppID(),CInstance::DAEMON))
	{
		SetTimer(EV_EQP_CONNECT_TIMER,EQP_CONNECT_TIMEOUT);
	}
	else
	{
		SetTimer(EV_EQP_REGISTER_TIMER,10);
	}
}
/*====================================================================
������      : DaemonProcRegTimerOut
����        ��ע�ᶨʱ����ʱ����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/04/25  1.0         ��־��          ����
====================================================================*/
void CApu2Inst::DaemonProcRegTimerOut()
{
	CServMsg cSvrMsg;
	TEqpRegReq tReg;
	tReg.SetEqpType( EQP_TYPE_MIXER );
	tReg.SetVersion( DEVVER_MIXER );
	cSvrMsg.SetMsgBody((u8*)&tReg, sizeof(tReg));
	post( MAKEIID(AID_MCU_PERIEQPSSN, CInstance::DAEMON), MIXER_MCU_REG_REQ,
		cSvrMsg.GetServMsg(), cSvrMsg.GetServMsgLen(), m_dwMcuNode );
	SetTimer(EV_EQP_REGISTER_TIMER,EQP_CONNECT_TIMEOUT);
}
/*====================================================================
������      : DaemonProcRegAck
����        ��ע��ɹ���Ϣ����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CMessage * const pcMsg
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/04/25  1.0         ��־��          ����
====================================================================*/
void CApu2Inst::DaemonProcRegAck(CMessage * const pcMsg)
{
	KillTimer(EV_EQP_REGISTER_TIMER);
	m_bIsGetMixerCfg = true;
	OspDisconnectTcpNode(m_dwMcuNode);
	TApu2EqpCfg tApu2cfg;
	u32 tdwIPList[5];  //�оٵ��ĵ�ǰ���õ�������Чip
	u16 dwIPNum;       //�оٵ�ip ��Ŀ
	
	u32 dwLocalIp = 0;
	dwIPNum = OspAddrListGet(tdwIPList, 5);
	if( 0 == dwIPNum )
	{
		LogPrint( LOG_LVL_DETAIL, MID_MCU_MIXER,"[GetConnectInfo] LocalIP is invalid.\n");
		dwLocalIp = INET_ADDR("127.0.0.1");
	}
	else
	{
		dwLocalIp = tdwIPList[0];
	}
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	u8 byMixMemNum = 0;
	char achAlias [ MAXLEN_ALIAS + 1 ] = {0};
	u16 wEqpRcvStartPort = 0;
	u8 byMixerIdx = 0;
	u8 byMixerNum = cServMsg.GetMsgBodyLen() / (sizeof(u8) + sizeof(u8) + sizeof(u16) + sizeof(achAlias));
	tApu2cfg.m_byMixerNum = byMixerNum;
	u8 *pMsg = (u8 *)cServMsg.GetMsgBody();
	for(byMixerIdx = 0 ;byMixerIdx < byMixerNum;byMixerIdx++)
	{
		tApu2cfg.m_acMixerCfg[byMixerIdx].dwConnectIP = ntohl(inet_addr("127.0.0.1"));
		tApu2cfg.m_acMixerCfg[byMixerIdx].wConnectPort = MCU_LISTEN_PORT;
		tApu2cfg.m_acMixerCfg[byMixerIdx].wMcuId  = LOCAL_MCUID;
		tApu2cfg.m_acMixerCfg[byMixerIdx].byEqpType  = EQP_TYPE_MIXER;
		tApu2cfg.m_acMixerCfg[byMixerIdx].dwLocalIP = dwLocalIp;
		tApu2cfg.m_acMixerCfg[byMixerIdx].byEqpId  = *(u8*)pMsg;
		pMsg += sizeof(u8);
		tApu2cfg.m_acMixerCfg[byMixerIdx].m_byMemberNum  = *(u8*)pMsg;
		pMsg += sizeof(u8);
		tApu2cfg.m_acMixerCfg[byMixerIdx].wRcvStartPort  =  *(u16*)pMsg;
		pMsg += sizeof(u16);
		strcpy(tApu2cfg.m_acMixerCfg[byMixerIdx].achAlias, (char *)pMsg);
		pMsg += sizeof(achAlias);
		LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MIXER,"[DaemonProcEqpMcuRegMsg]MixerId(%d) MixMemNum(%d) RcvStartPort(%d) Alias(%s)!\n", 
			tApu2cfg.m_acMixerCfg[byMixerIdx].byEqpId,tApu2cfg.m_acMixerCfg[byMixerIdx].m_byMemberNum,
			tApu2cfg.m_acMixerCfg[byMixerIdx].wRcvStartPort,tApu2cfg.m_acMixerCfg[byMixerIdx].achAlias);
	}
	::OspPost(MAKEIID(AID_MIXER, CInstance::DAEMON), EV_MIXER_INIT, &tApu2cfg, sizeof(TApu2EqpCfg));
}
/*====================================================================
������      : DaemonProcRegNack
����        ��ע��ʧ����Ϣ����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/04/25  1.0         ��־��          ����
====================================================================*/
void CApu2Inst::DaemonProcRegNack()
{
	SetTimer(EV_EQP_CONNECT_TIMER,EQP_CONNECT_TIMEOUT);
}
/*====================================================================
������      : DaemonProcOspDisconnect
����        ��������Ϣ����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/04/25  1.0         ��־��          ����
====================================================================*/
void CApu2Inst::DaemonProcOspDisconnect()
{
	m_dwMcuNode = INVALID_NODE;
	if (!m_bIsGetMixerCfg) 
	{
		SetTimer(EV_EQP_CONNECT_TIMER,EQP_CONNECT_TIMEOUT);
	}
}
#endif


