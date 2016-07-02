/*****************************************************************************
   ģ����      : N+1���ݹ���ģ��
   �ļ���      : nplusmanager.cpp
   ����ļ�    : nplusmanager.h
   �ļ�ʵ�ֹ���: N+1���ݹ���
   ����        : ������
   �汾        : V4.0  Copyright(C) 2006-2009 KDCOM, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2006/11/21  4.0         ������      ����
******************************************************************************/

#include "evmcu.h"
#include "evmcumcs.h"
#include "mcuvc.h"
#include "nplusmanager.h"
#include "mcsssn.h"
//#include "mcuerrcode.h"
#include "evmcumt.h"
#include "mtadpssn.h"

CNPlusApp g_cNPlusApp;

//CNPlusInst
//construct
CNPlusInst::CNPlusInst( void ):m_dwMcuNode(INVALID_NODE), 
                               m_dwMcuIId(0), 
                               m_dwMcuIpAddr(0),
                               m_dwMcuNodeB(INVALID_NODE), 
                               m_dwMcuIIdB(0), 
                               m_dwMcuIpAddrB(0),
                               m_byUsrNum(0),
                               m_wRtdFailTimes(0),
                               m_wRegNackByCapTimes(0)
{
    memset(&m_cUsrGrpsInfo, 0, sizeof(m_cUsrGrpsInfo));
	memset(&m_atConfExData[0], 0,sizeof(m_atConfExData));
	for( u8 byIdx = 0; byIdx <sizeof(m_atNplusParam)/sizeof(m_atNplusParam[0]);byIdx++ )
	{
		m_atNplusParam[byIdx].Clear();
	}
}

//destruct
CNPlusInst::~CNPlusInst( void )
{
}

/*=============================================================================
�� �� ���� InstanceEntry
��    �ܣ� ����Serverģʽ��ÿ��ʵ����Ӧһ��mcu
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage * const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/21  4.0			������                  ����
=============================================================================*/
void CNPlusInst::InstanceEntry( CMessage * const pcMsg )
{
    if ( NULL == pcMsg )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::InstanceEntry] invalid msg body!\n" );
        return;
    }

    switch( pcMsg->event )
    {  
    case OSP_DISCONNECT:
        ProcDisconnect(pcMsg);
        break;    

    case MCU_NPLUS_REG_REQ:
        ProcRegNPlusMcuReq( pcMsg );
        break;

        //������mcu���͹����Ļ�������
    case MCU_NPLUS_CONFDATAUPDATE_REQ:
    case MCU_NPLUS_CONFINFOUPDATE_REQ:
    case MCU_NPLUS_USRGRPUPDATE_REQ:
    case MCU_NPLUS_USRINFOUPDATE_REQ:
    case MCU_NPLUS_CONFMTUPDATE_REQ:
	case MCU_NPLUS_SMCUINFOUPDATE_REQ:
    case MCU_NPLUS_CHAIRUPDATE_REQ:
    case MCU_NPLUS_SPEAKERUPDATE_REQ:
    case MCU_NPLUS_VMPUPDATE_REQ:
    case MCU_NPLUS_RASINFOUPDATE_REQ:
	case MCU_NPLUS_AUTOMIXUPDATE_REQ:
        ProcMcuDataUpdateReq( pcMsg );
        break;

    case MCU_NPLUS_EQPCAP_NOTIF:
        ProcMcuEqpCapNotif( pcMsg );
        break;

    case MCU_NPLUS_RTD_RSP:
        ProcRtdRsp();
        break;

    case MCU_NPLUS_RTD_TIMER:
        ProcRtdTimeOut();
        break;

        //����mcu���͵���mcu����ع���Ϣ
    case VC_NPLUS_MSG_NOTIF:
        ProcConfRollback( pcMsg );
        break;

    case VC_NPLUS_RESET_NOTIF:
        ProcReset( );
        break;

    case MCU_NPLUS_CONFROLLBACK_ACK:
    case MCU_NPLUS_CONFROLLBACK_NACK:
    case MCU_NPLUS_USRROLLBACK_ACK:
    case MCU_NPLUS_USRROLLBACK_NACK:
    case MCU_NPLUS_GRPROLLBACK_ACK:
    case MCU_NPLUS_GRPROLLBACK_NACK:
        break;

    default:
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::InstanceEntry] invalid event(%d): %s!\n", 
            pcMsg->event, OspEventDesc(pcMsg->event));
        break;
    }

    return;
}

/*=============================================================================
�� �� ���� InstanceDump
��    �ܣ� print
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u32 dwParam = 0
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/24  4.0			������                  ����
=============================================================================*/
void CNPlusInst::InstanceDump( u32 dwParam )
{
    u8 byIdx = 0;
    u8 byLoop = 0;
    StaticLog("=================NPlusInst: InsID %d================\n", GetInsID());
    StaticLog("McuNode:0x%x McuIId:0x%x McuIp:0x%x dwParam:0x%x\n", m_dwMcuNode, 
              m_dwMcuIId, m_dwMcuIpAddr,dwParam);
    StaticLog("McuNodeB:0x%x McuIIdB:0x%x McuIpB:0x%x dwParam:0x%x\n", m_dwMcuNodeB, 
              m_dwMcuIIdB, m_dwMcuIpAddrB,dwParam);
    
    for (byIdx = 0; byIdx < MAXNUM_ONGO_CONF; byIdx++)
    {
        if (m_atConfData[byIdx].IsNull())
        {
            continue;
        }
        m_atConfData[byIdx].m_tConf.Print();
		m_atConfData[byIdx].m_tConf.GetConfAttrb().Print();
		m_atConfData[byIdx].m_tConf.m_tStatus.Print();
		m_atConfData[byIdx].m_tConf.m_tStatus.GetConfMode().Print();

        StaticLog("\nMtInfo in conf:\n");
        for (byLoop = 0; byLoop < m_atConfData[byIdx].m_byMtNum; byLoop++)
        {   
            TMtInfo *ptInfo = &m_atConfData[byIdx].m_atMtInConf[byLoop];            
            StaticLog("%d: 0x%x(%d)  %d(kbs)\n", byLoop,
                      ptInfo->GetMtAddr().GetIpAddr(),
                      ptInfo->GetMtAddr().GetPort(), ptInfo->GetCallBitrate());
        }

		StaticLog("\nSmcuInfo in conf:\n");
        for (byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++)
        {   
            TSmcuCallnfo tScmuInfo = m_atConfExData[byIdx].m_atSmcuCallInfo[byLoop];
            if(tScmuInfo.m_dwMtAddr.IsNull())
			{
				break;
			}
			else
			{
				StaticLog("%d: 0x%x(%d) E164:%s\n", byLoop,
				tScmuInfo.m_dwMtAddr.GetIpAddr(),
				tScmuInfo.m_dwMtAddr.GetPort(),tScmuInfo.m_achAlias);
			}
        }

        StaticLog("single VmpInfo in conf:\n");
        for (byLoop = 0; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++)
        {

			if(m_atConfData[byIdx].m_tVmpInfo.m_abyMemberType[byLoop]!= VMP_MEMBERTYPE_NULL
				&& m_atConfData[byIdx].m_tVmpInfo.m_abyMemberType[byLoop]!= VMP_MEMBERTYPE_MCSSPEC )
			{
				StaticLog("%d: Membertype(%d)\n", byLoop,
                      m_atConfData[byIdx].m_tVmpInfo.m_abyMemberType[byLoop]);

				continue;
			}

            TMtInfo *ptInfo = &m_atConfData[byIdx].m_tVmpInfo.m_atMtInVmp[byLoop];
            if (ptInfo->IsNull())
            {
                continue;
            }
            StaticLog("%d: 0x%x(%d)  %d(kbs) type(%d)\n", byLoop,
                      ptInfo->GetMtAddr().GetIpAddr(),
                      ptInfo->GetMtAddr().GetPort(), ptInfo->GetCallBitrate(), 
                      m_atConfData[byIdx].m_tVmpInfo.m_abyMemberType[byLoop]);
        }
    }

	for(byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx ++ )
	{
		if(!m_atNplusParam[byIdx].IsNull())
		{
			StaticLog("===multi--VmpInfo in conf[%d]=============\n",byIdx);
			m_atNplusParam[byIdx].m_tVmpBaiscParam.Print();

			StaticLog("confIdx:%d\n",m_atNplusParam[byIdx].m_byConfIdx);
			for (byLoop = 0; byLoop < MAXNUM_VMP_MEMBER; byLoop++)
			{
				if(m_atNplusParam[byIdx].m_tVmpMemer.m_tVmpChnnlInfo[byLoop].m_byMemberType!= VMP_MEMBERTYPE_NULL
					&& m_atNplusParam[byIdx].m_tVmpMemer.m_tVmpChnnlInfo[byLoop].m_byMemberType!= VMP_MEMBERTYPE_MCSSPEC )
				{
					StaticLog("%d: Membertype(%d)\n", byLoop,
						m_atNplusParam[byIdx].m_tVmpMemer.m_tVmpChnnlInfo[byLoop].m_byMemberType);
					
					continue;
				}

				TMtInfo *ptInfo = &m_atNplusParam[byIdx].m_tVmpMemer.m_tVmpChnnlInfo[byLoop].m_tMtInVmp;
				if (ptInfo->IsNull())
				{
					continue;
				}
				StaticLog("%d: 0x%x(%d)  %d(kbs) type(%d)\n", byLoop,
					ptInfo->GetMtAddr().GetIpAddr(),
					ptInfo->GetMtAddr().GetPort(), ptInfo->GetCallBitrate(), 
					m_atNplusParam[byIdx].m_tVmpMemer.m_tVmpChnnlInfo[byLoop].m_byMemberType);
			}
		}
	}

    m_cUsrGrpsInfo.Print();

    StaticLog("user info:\n");
    for(byIdx = 0; byIdx < m_byUsrNum; byIdx++)
    {
        m_acUsrInfo[byIdx].Print();
    }
    
    return;
}

/*=============================================================================
�� �� ���� ClearInst
��    �ܣ� ���ʵ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/30  4.0			������                  ����
=============================================================================*/
void CNPlusInst::ClearInst( void )
{
    m_dwMcuNode = INVALID_NODE;
    m_dwMcuIId = 0;
    m_dwMcuIpAddr = 0;
    m_dwMcuNodeB = INVALID_NODE;
    m_dwMcuIIdB = 0;
    m_dwMcuIpAddrB = 0;
    m_byUsrNum = 0;
    m_wRtdFailTimes = 0;
    m_wRegNackByCapTimes = 0;
    memset(m_atConfData, 0, sizeof(m_atConfData));
    memset(&m_cUsrGrpsInfo, 0, sizeof(m_cUsrGrpsInfo));
    memset(m_acUsrInfo, 0, sizeof(m_acUsrInfo));
    memset(&m_tRASInfo, 0, sizeof(m_tRASInfo));
    memset(m_atChargeInfo, 0, sizeof(m_atChargeInfo));
	memset(m_atConfExData, 0, sizeof(m_atConfExData));

	for( u8 byIdx = 0; byIdx < sizeof(m_atNplusParam)/sizeof(m_atNplusParam[0]); byIdx++ )
	{
		m_atNplusParam[byIdx].Clear();
	}

    NEXTSTATE(STATE_IDLE);
    return;
}

/*=============================================================================
�� �� ���� ProcRegNPlusMcuReq
��    �ܣ� ��mcuע�ᴦ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/22  4.0			������                  ����
2013/03/18              liaokang              ֧������N+1��˫����
=============================================================================*/
void CNPlusInst::ProcRegNPlusMcuReq( const CMessage * pcMsg )
{
    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcRegNPlusMcuReq]Enter inst.%d's ProcRegNPlusMcuReq!\n",GetInsID());
    
    CServMsg cServMsg(pcMsg->content, pcMsg->length);   
    TNPlusMcuRegInfo *ptRegInfo = (TNPlusMcuRegInfo *)cServMsg.GetMsgBody();
    TNPlusEqpCapEx tActualCapEx;
    TNPlusMcuInfoEx tRemoteMcuInfoEx;
    tRemoteMcuInfoEx.SetMcuEncodingForm(emenCoding_GBK); //Ĭ�϶Զ�ΪGBK����
    if(cServMsg.GetMsgBodyLen() >sizeof(TNPlusMcuRegInfo) + sizeof(u32) + sizeof(u32))
    {
        u16 wUnpackLen = 0;
        BOOL32 bUnkonwInfo = FALSE;
        g_cNPlusApp.UnPackNplusExInfo(tActualCapEx,cServMsg.GetMsgBody()+sizeof(TNPlusMcuRegInfo),wUnpackLen,bUnkonwInfo,&tRemoteMcuInfoEx);
    }
    
    u32 dwMcuIpAddr = ptRegInfo->GetMcuIpAddr();
    u32 dwMcuIId = *(u32 *)(cServMsg.GetMsgBody() + cServMsg.GetMsgBodyLen() - sizeof(u32) - sizeof(u32));
    u32 dwMcuNode = *(u32 *)(cServMsg.GetMsgBody() + cServMsg.GetMsgBodyLen() - sizeof(u32));
    LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[ProcRegNPlusMcuReq] McuIpAddr(0x%x) McuIId(%x) McuNode(%d)\n", dwMcuIpAddr, dwMcuIId, dwMcuNode);

    if ( CurState() == STATE_NORMAL )
    {
        // ��ֹ����ע��
        if( dwMcuNode == m_dwMcuNode || dwMcuNode == m_dwMcuNodeB )
        {
            LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcRegNPlusMcuReq] invalid ins state(%d)(id.%d)\n", CurState(), GetInsID());
            return;
        }    
    }

    BOOL32 bRollBack = FALSE;
    //����Ƿ�Ϊ���ϻָ�mcu �� ������mcu�Ƿ�֧�ֹ��ϻָ�
    if (CurState() == STATE_SWITCHED && g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_SWITCH)
    {
        TLocalInfo tLocalInfo;
        if (SUCCESS_AGENT == g_cMcuAgent.GetLocalInfo(&tLocalInfo) && 
            tLocalInfo.GetIsNPlusRollBack() &&
            (m_dwMcuIpAddr == ptRegInfo->GetMcuIpAddr() || m_dwMcuIpAddrB == ptRegInfo->GetMcuIpAddr()))
        {        
            //������ָ�������������������������ܾ����ȴ����������������ٽ���ģʽ�л�
            TNPlusEqpCap tNeedCap,tActualCap;
			TNPlusEqpCapEx tNeedCapEx/*,tActualCapEx*/;
			TBasChnAbility atBasChnAbility[NPLUS_MAX_BASCHN_NUM];
			u8 byBasChnNum = 0;

            for (u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF; byLoop++)
            {
                if (!m_atConfData[byLoop].IsNull())
                {
					TBasChnAbility atTempBasChn[NPLUS_MAX_BASCHN_NUM];
					TNPlusEqpCap tTmpCap;
					TNPlusEqpCapEx tTmpCapEx;

					u8 byConfBasChnNum = 0;
                    g_cNPlusApp.GetEqpCapFromConf(m_atConfData[byLoop].m_tConf,tTmpCap,tTmpCapEx,&atTempBasChn[0],byConfBasChnNum);
                    tNeedCap = tNeedCap + tTmpCap;
					tNeedCapEx = tNeedCapEx + tTmpCapEx;
					memcpy((u8*)(atBasChnAbility+byBasChnNum),(u8*)atTempBasChn,byConfBasChnNum*sizeof(TBasChnAbility));
					byBasChnNum += byConfBasChnNum;
                }
            }
            
            tActualCap = ptRegInfo->GetMcuEqpCap();

            // guzh  [12/13/2006] �������������ͬʱҪ���MCU����Mp��MtAdp
			TBasChnAbility atActualBasChnAbility[NPLUS_MAX_BASCHN_NUM];
			u8 byActualBasChnNum = 0;
			GetAllBasChnAbilityArray(tActualCap,tActualCapEx,atActualBasChnAbility,byActualBasChnNum);
			
			u8 byActualVmpNum = tActualCap.m_byVmpNum+tActualCapEx.m_byMPU2BasicVmpNum+tActualCapEx.m_byMPU2EcardBasicVmpNum+tActualCapEx.m_byMPU2EnhancedVmpNum;
			u8 byNeedVmpNum = tNeedCap.m_byVmpNum+tNeedCapEx.m_byMPU2BasicVmpNum+tNeedCapEx.m_byMPU2EcardBasicVmpNum+tNeedCapEx.m_byMPU2EnhancedVmpNum;

			//�ع� vmpֻ�ȸ���
			if(tActualCap < tNeedCap ||
				tActualCapEx < tNeedCapEx ||
				byActualVmpNum < byNeedVmpNum||
				!IsBasSupportRollBack(atActualBasChnAbility,byActualBasChnNum) ||
                 !tActualCap.HasMp() || 
                 !tActualCap.HasMtAdp() )
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcRegNPlusMcuReq] Reg MCU(0x%x) eqp cap is not enough to rollback conf\n", 
                          ptRegInfo->GetMcuIpAddr());

                // guzh [12/13/2006]
                // �����û�е���ܾ��ع�����
                // ��Ͽ����ӣ��˳�������ǿ�ƻع�����ֹ���ݷ������ٳ�
                m_wRegNackByCapTimes ++;
                // guzh [12/13/2006] ���ǵ����ܶԶ˻����������޷���ʼ����ʱ����
                //if ( m_wRegNackByCapTimes  < NPLUS_MAXNUM_REGNACKBYREMOTECAP)
                LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcRegNPlusMcuReq] Reject m_wRegNackByCapTimes = %d\n", m_wRegNackByCapTimes);
                OspDisconnectTcpNode(pcMsg->srcnode);
                return;           
            }
            
            //ģʽ�л� rollback
            bRollBack = TRUE; 
            
            //���ԭ�ȱ���Ļ�������
            ClearInst();
        }
        else
        {
            LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS, "[ProcRegNPlusMcuReq] mcu not support N+1 rollback.ignore mcu(0x%x) reg\n", 
                     ptRegInfo->GetMcuIpAddr());
            return;
        }        
    } 
  
    if( ( 0 == m_dwMcuIpAddr && 0 == m_dwMcuIpAddrB ) ||   // Idle��inst
          dwMcuIpAddr == m_dwMcuIpAddr)                    // normal��switch��inst
    {
        m_dwMcuIpAddr = dwMcuIpAddr;
        m_dwMcuIId = dwMcuIId;
        m_dwMcuNode = dwMcuNode;
        m_dwMcuIpAddrB = tRemoteMcuInfoEx.GetAnotherMpcIp();
    }    
    else /*if( dwMcuIpAddr == m_dwMcuIpAddrB )*/          // normal��switch��inst
    {
        m_dwMcuIpAddrB = dwMcuIpAddr;
        m_dwMcuIIdB = dwMcuIId;
        m_dwMcuNodeB = dwMcuNode;
        m_dwMcuIpAddr = tRemoteMcuInfoEx.GetAnotherMpcIp();
    }

    m_emMcuEncodingForm = tRemoteMcuInfoEx.GetMcuEncodingForm();

    //ack
    u8 byRollBack = bRollBack ? 1 : 0;

		u8 abyBuf[5];
    	abyBuf[0] = byRollBack;
	u16 wRtdTime = htons(g_cNPlusApp.GetRtdTime());
	u16 wRtdNum  = htons(g_cNPlusApp.GetRtdNum());
	memcpy( &abyBuf[1],&wRtdTime,sizeof(wRtdTime) );
	memcpy( &abyBuf[3],&wRtdNum,sizeof(wRtdNum) );
	
    PostReplyBack(pcMsg->event+1, 0, &abyBuf[0], sizeof(abyBuf), dwMcuIId, dwMcuNode );
    
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS,"[ProcRegNPlusMcuReq] TIME.%d num%d\n",g_cNPlusApp.GetRtdTime(),
		g_cNPlusApp.GetRtdNum() );
    

    //ע���������ʵ��
    OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), GetInsID());

    if( STATE_NORMAL != CurState() )
    {
        SetTimer(MCU_NPLUS_RTD_TIMER, g_cNPlusApp.GetRtdTime()*1000);
    }
    
	//  [1/13/2011 chendaiwei]״̬�����л��ɺ���β���ᵽ�˴���Ϊ�˱�֤RTD_TIMER��������ʱ��,״̬ʼ�մ���STATE_NORMAL��
	//  �����������״̬���Ͳ�Ӧ���ٽ���RTD��⡣
	NEXTSTATE(STATE_NORMAL);


	// �ָ�����e164��ע��gk, ������mcu����duplicate alias���޷������ն�[11/8/2012 chendaiwei]
	CServMsg cMsg;
	cServMsg.SetConfIdx( 0 );
	cServMsg.SetEventId( MCU_MT_RESTORE_MCUE164_NTF );
	g_cMtAdpSsnApp.SendMsgToMtAdpSsn( g_cMcuVcApp.GetRegGKDriId(), 
                                          MCU_MT_RESTORE_MCUE164_NTF, cMsg );

	//Delay 5 sȷ�����ָ�����E164��ע���RRQ�ɹ�[11/12/2012 chendaiwei]
	if(bRollBack && 
		g_cMcuAgent.GetGkIpAddr() != 0 && 0 != g_cMcuVcApp.GetRegGKDriId())
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS,"[ProcRegNPlusMcuReq] OspDelay 5 second for restore 1 Mode RRQ suceess\n");
		OspDelay(5000);
	}

    //�ع�
    if (bRollBack)
    {
        //�ָ��û�����û���Ϣ
        OspPost(MAKEIID(AID_MCU_MCSSN, CInstance::DAEMON), NPLUS_VC_DATAUPDATE_NOTIF, NULL, 0, 0,
                MAKEIID(AID_MCU_NPLUSMANAGER, GetInsID())); 

        //�ָ�����������Ϣ
        OspPost(MAKEIID(AID_MCU_VC, CInstance::EACH), NPLUS_VC_DATAUPDATE_NOTIF, NULL, 0, 0,
                MAKEIID(AID_MCU_NPLUSMANAGER, GetInsID()));            
    
        //״̬����ת
        g_cNPlusApp.SetLocalNPlusState(MCU_NPLUS_SLAVE_IDLE);

		//  [1/13/2011 chendaiwei]�ڻع�������,���SwitchedMcu��IP��ַ��InstId�������ٴ�ע���ʱ������
		g_cNPlusApp.SetMcuSwitchedInsId(0,0);

    }
	//xliang [8/27/2009] ��mcu��mcuע��2�ֳ���
    //1, ����down��,��mcu�ӹܣ�֮����mcu�ٴ�up����ʱ��mcu����rollback, ���ָ�����e164��ע��gk
	//2, ����mcu�Ⱥ�����(ֻ�в�����Ա�������ֲ���) 
	//   ����mcu�Һ󣬱�mcu�ӹܺ�mcuҲ�ҵ���֮����/��mcu�ٴ��Ⱥ�����(��ʵ������С�����¼�)����ʱ��mcuҲҪ
	//  �ָ�����e164��ע��gk, ������mcu����duplicate alias���޷������նˡ�
    //NEXTSTATE(STATE_NORMAL);

    return;
}

/*=============================================================================
�� �� ���� ProcMcuDataUpdateReq
��    �ܣ� ��mcu��N+1 mcu����ͬ��������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/23  4.0			������                  ����
=============================================================================*/
void CNPlusInst::ProcMcuDataUpdateReq( const CMessage * pcMsg )
{    
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    CConfId cConfId = cServMsg.GetConfId();
    
    switch (pcMsg->event)
    {
        case MCU_NPLUS_CONFDATAUPDATE_REQ:
            {         
                if (sizeof(TNPlusConfData) > cServMsg.GetMsgBodyLen())
				{
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(confdata)\n", cServMsg.GetMsgBodyLen());
                    return;
				}
				else
				{
					TNPlusConfData *ptConfData = (TNPlusConfData *)cServMsg.GetMsgBody();
					
					if (!SetConfData(ptConfData))
					{
						PostReplyBack(pcMsg->event+2);
						LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set conf data failed.insid.%d\n", GetInsID());
						return;
					}

					u16 wUnpackLen = 0;
					if(ptConfData->m_tConf.HasConfExInfo())
					{
						TConfInfoEx tConfEx;

						BOOL32 bExistUnkonwType = FALSE;
						UnPackConfInfoEx(tConfEx,cServMsg.GetMsgBody()+sizeof(TNPlusConfData),wUnpackLen,bExistUnkonwType);
						
						if(bExistUnkonwType)
						{
							PostReplyBack(pcMsg->event+2);
							LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] unknown conf data.insid.%d\n", GetInsID());

							return;
						}
						else
						{
							if(!SetConfExData(ptConfData->m_tConf.GetConfId(),cServMsg.GetMsgBody()+sizeof(TNPlusConfData),wUnpackLen))
							{
								PostReplyBack(pcMsg->event+2);
								LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] setConfExData failed.insid.%d\n", GetInsID());
								
								return;
							}
						}
					}

					if(cServMsg.GetMsgBodyLen()>sizeof(TNPlusConfData)+wUnpackLen)
					{
						CConfId cTmpConId = ptConfData->m_tConf.GetConfId();
						if(!SetConfSmcuCallInfo(cTmpConId,(TSmcuCallnfo*)(cServMsg.GetMsgBody()+sizeof(TNPlusConfData)+wUnpackLen),(u8)MAXNUM_SUB_MCU))
						{
							PostReplyBack(pcMsg->event+2);
							LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] setConfExData<smcuinfo> failed.insid.%d\n", GetInsID());
							
							return;
						}

						if(cServMsg.GetMsgBodyLen() > sizeof(TNPlusConfData)+wUnpackLen +sizeof(TSmcuCallnfo)*MAXNUM_SUB_MCU)
						{
							u16 wPackVmpLen = 0;
							if(!SetConfMultiVmpParam(cTmpConId,(cServMsg.GetMsgBody()+sizeof(TNPlusConfData)+wUnpackLen+sizeof(TSmcuCallnfo)*MAXNUM_SUB_MCU),wPackVmpLen))
							{
								PostReplyBack(pcMsg->event+2);
								LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] SetConfMultiVmpParam failed.insid.%d\n", GetInsID());
								
								return;
							}
						}
					}


				}
            }            
            break;

        case MCU_NPLUS_CONFINFOUPDATE_REQ:
            {
                if (sizeof(u8) + sizeof(TConfInfo) > cServMsg.GetMsgBodyLen())
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(confinfo)\n", cServMsg.GetMsgBodyLen());
                    return;
                }
                
                u8 byMode = *(u8 *)cServMsg.GetMsgBody();
                TConfInfo *ptConf = (TConfInfo *)(cServMsg.GetMsgBody() + sizeof(u8));
				u8 *ptConfInfoEx = NULL;
				if(cServMsg.GetMsgBodyLen()> sizeof(u8)+sizeof(TConfInfo))
				{
					ptConfInfoEx = cServMsg.GetMsgBody() + sizeof(u8) + sizeof(TConfInfo);
				}
				
				BOOL32 bIsStart = FALSE;
				if( NPLUS_CONF_START == byMode )
				{
					bIsStart = TRUE;
				}

                if (!SetConfInfo(ptConf,  bIsStart))
                {
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set conf info failed.insid.%d\n", GetInsID());
                    return;
                }
			
				u16 wUnpackLen = 0;
				if(ptConf->HasConfExInfo() && bIsStart)
				{
					TConfInfoEx tConfEx;

					BOOL32 bExistUnkonwType = FALSE;
					
					UnPackConfInfoEx(tConfEx,ptConfInfoEx,wUnpackLen,bExistUnkonwType);
					
					if(bExistUnkonwType)
					{
						PostReplyBack(pcMsg->event+2);
						LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] unknown conf info.insid.%d\n", GetInsID());
						
						return;
					}
					else
					{
						if(!SetConfExData(ptConf->GetConfId(),ptConfInfoEx,wUnpackLen))
						{
							PostReplyBack(pcMsg->event+2);
							LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] setConfExData failed.insid.%d\n", GetInsID());
							
							return;
						}
					}
					
					if( ptConfInfoEx != NULL)
					{
						ptConfInfoEx+= wUnpackLen;
					}
				}
            }            
            break;

        case MCU_NPLUS_USRGRPUPDATE_REQ:
            {
                if ((sizeof(CUsrGrpsInfo)) != cServMsg.GetMsgBodyLen())
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(usrgrp)\n", cServMsg.GetMsgBodyLen());
                    return;
                }               

                CUsrGrpsInfo *pcGrpInfo = (CUsrGrpsInfo *)cServMsg.GetMsgBody();
                if (!SetUsrGrpInfo(pcGrpInfo))
                {
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set grp info failed.insid.%d\n", GetInsID());
                    return;
                }
            }            
            break;

        case MCU_NPLUS_USRINFOUPDATE_REQ:
            {
                if (0 != cServMsg.GetMsgBodyLen()%sizeof(CExUsrInfo))
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(usrinfo)\n", cServMsg.GetMsgBodyLen());
                    return;
                }
                            
                u8 byUsrNum = cServMsg.GetMsgBodyLen()/sizeof(CExUsrInfo);
                CExUsrInfo *pcUsrInfo = (CExUsrInfo *)cServMsg.GetMsgBody();
                BOOL32 bAdd = (cServMsg.GetCurPktIdx() == 0) ? FALSE : TRUE;
                if (!SetUsrInfo(pcUsrInfo, byUsrNum, bAdd))
                {
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set usr info failed.insid.%d\n", GetInsID());
                    return;
                }
            }            
            break;

        case MCU_NPLUS_CONFMTUPDATE_REQ:
            {
                if (0 != cServMsg.GetMsgBodyLen()%sizeof(TMtInfo))
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(mtinfo)\n", cServMsg.GetMsgBodyLen());
                    return;
                }
              
                TMtInfo *ptMtInfo = (TMtInfo *)cServMsg.GetMsgBody();
                u8 byMtNum = cServMsg.GetMsgBodyLen()/sizeof(TMtInfo);

                if (!SetConfMtInfo(cConfId, ptMtInfo, byMtNum))
                {
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set mt info failed. insid.%d\n", GetInsID());
                    return;
                }
            }            
            break;

        case MCU_NPLUS_SMCUINFOUPDATE_REQ:
            {
                if (0 != cServMsg.GetMsgBodyLen()%sizeof(TSmcuCallnfo))
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(smcuinfo)\n", cServMsg.GetMsgBodyLen());
                    return;
                }
				
                TSmcuCallnfo *ptSmcuCallInfo = (TSmcuCallnfo *)cServMsg.GetMsgBody();
                u8 bySmcuNum = cServMsg.GetMsgBodyLen()/sizeof(TSmcuCallnfo);
				
                if (!SetConfSmcuCallInfo(cConfId, ptSmcuCallInfo, bySmcuNum))
                {
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set smcu call info failed. insid.%d\n", GetInsID());
                    return;
                }
            }            
            break;

        case MCU_NPLUS_CHAIRUPDATE_REQ:
            {
                if (sizeof(TMtAlias) != cServMsg.GetMsgBodyLen())
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(chair)\n", cServMsg.GetMsgBodyLen());
                    return;
                }

                TMtAlias *ptAlias = (TMtAlias *)cServMsg.GetMsgBody();
                if (!SetChairman(cConfId, ptAlias))
                {
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set chair info failed.insid.%d\n", GetInsID());
                    return;
                }
            }            
            break;

        case MCU_NPLUS_SPEAKERUPDATE_REQ:
            {
                if (sizeof(TMtAlias) != cServMsg.GetMsgBodyLen())
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(speaker)\n", cServMsg.GetMsgBodyLen());
                    return;
                }
             
                TMtAlias *ptAlias = (TMtAlias *)cServMsg.GetMsgBody();
                if (!SetSpeaker(cConfId, ptAlias))
                {
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set speaker info failed.insid.%d\n", GetInsID());
                    return;
                }
            }            
            break;

        case MCU_NPLUS_VMPUPDATE_REQ:
            {
                if (sizeof(TNPlusVmpInfo) + sizeof(TVMPParam) > cServMsg.GetMsgBodyLen())
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(vmpinfo)\n", cServMsg.GetMsgBodyLen());
                    return;
                }

                TNPlusVmpInfo *ptVmpInfo = (TNPlusVmpInfo *)cServMsg.GetMsgBody();
                TVMPParam *ptParam = (TVMPParam *)(cServMsg.GetMsgBody() + sizeof(TNPlusVmpInfo));
                if (!SetConfVmpInfo(cConfId, ptVmpInfo, ptParam))
                {
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set vmp info failed.insid.%d\n", GetInsID());
                    return;
                }

				if (sizeof(TNPlusVmpInfo) + sizeof(TVMPParam) < cServMsg.GetMsgBodyLen())
				{
					u16 wPackVmpLen = 0;
					if(!SetConfMultiVmpParam(cConfId,cServMsg.GetMsgBody()+ sizeof(TNPlusVmpInfo)+sizeof(TVMPParam),wPackVmpLen))
					{
						PostReplyBack(pcMsg->event+2);
						LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set multi vmp info failed.insid.%d\n", GetInsID());
						
						return;
					}
				}
            }            
            break;

		//[chendaiwei 2010/09/27]N+1���ݴ����������������ܻ���״̬����
		case MCU_NPLUS_AUTOMIXUPDATE_REQ:
            {
                if (sizeof(BOOL32) != cServMsg.GetMsgBodyLen())
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(automix)\n", cServMsg.GetMsgBodyLen());
                    return;
                }
                BOOL32 bStartAutoMix = *(BOOL32 *)cServMsg.GetMsgBody();
				if ( !bStartAutoMix )
				{
					LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] stop NPlus auto mixing.");
				}
                if (!SetConfAutoMix(cConfId,bStartAutoMix))
                {
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set auto mix failed.insid.%d\n", GetInsID());
                    return;
                }
            }            
            break;
        case MCU_NPLUS_RASINFOUPDATE_REQ:
            {
                if ( sizeof(TRASInfo) != cServMsg.GetMsgBodyLen() ) 
                {
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(rasinfo)\n", cServMsg.GetMsgBodyLen() );
                    return;
                }
                TRASInfo tRASInfo = *(TRASInfo*)cServMsg.GetMsgBody();
                SetRASInfo(&tRASInfo);
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set RasInfo. Ras EPID Length[%u], EPId Alias[%s], MCUE164[%s]!\n",tRASInfo.GetEPID()->GetIDlength(),tRASInfo.GetEPID()->GetIDAlias(),tRASInfo.GetMcuE164());               

                if ( tRASInfo.GetEPID()->GetIDlength() == 0 )
                {
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] EPID has set to SLAVE mcu Error!\n");
                }
            }
            break;

        default: 
            LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg (%s:%d)\n",
                                        OspEventDesc(pcMsg->event), pcMsg->event);
            return;

    }
    //ack
    PostReplyBack(pcMsg->event+1);
    return;
}

/*=============================================================================
�� �� ���� ProcMcuEqpCapNotif
��    �ܣ� mcu��������������飨Ŀǰֻ�������������飩
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/29  4.0			������                  ����
=============================================================================*/
void CNPlusInst::ProcMcuEqpCapNotif( const CMessage * pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    if (sizeof(TNPlusEqpCap) > cServMsg.GetMsgBodyLen())
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuEqpCapNotif] invalid msg body len(%d)\n", cServMsg.GetMsgBodyLen());
        return;
    }

    TNPlusEqpCap tLocalCap = g_cNPlusApp.GetMcuEqpCap();
    TNPlusEqpCap tRegMcuCap = *(TNPlusEqpCap *)cServMsg.GetMsgBody();

	TNPlusEqpCapEx tLocalCapEx = g_cNPlusApp.GetMcuEqpCapEx();
	
	TNPlusEqpCapEx tRegCapEx;
	if(cServMsg.GetMsgBodyLen()>sizeof(TNPlusEqpCap))
	{
		BOOL32 bUnkownInfo = FALSE;
		u16 wUnpackLen = 0;
		g_cNPlusApp.UnPackNplusExInfo(tRegCapEx,cServMsg.GetMsgBody()+sizeof(TNPlusEqpCap),wUnpackLen,bUnkownInfo);
		
		if(bUnkownInfo)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuEqpCapNotif] registing mcu eqp type couldnot be distinguished . disconnect node!\n", 
				cServMsg.GetMsgBodyLen());
			OspDisconnectTcpNode(m_dwMcuNode);
			OspDisconnectTcpNode(m_dwMcuNodeB);
			//  [2/21/2011 chendaiwei]��MCU�����������ڱ�MCU����Ϊ��MCU���߱�������MCU��������ȡ��RTD��⣬�Ա���
			//  ������MCU�ָ���������̡�
			KillTimer(MCU_NPLUS_RTD_TIMER);
			ClearInst();
			
			return;
		}
	}
	
	if(IsLocalEqpCapLowerThanRemote(tLocalCap,tLocalCapEx,tRegMcuCap,tRegCapEx))
	{
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuEqpCapNotif] registing mcu eqpcap is larger then local. disconnect node!\n", 
            cServMsg.GetMsgBodyLen());
        OspDisconnectTcpNode(m_dwMcuNode);
		OspDisconnectTcpNode(m_dwMcuNodeB);
		//  [2/21/2011 chendaiwei]��MCU�����������ڱ�MCU����Ϊ��MCU���߱�������MCU��������ȡ��RTD��⣬�Ա���
		//  ������MCU�ָ���������̡�
		KillTimer(MCU_NPLUS_RTD_TIMER);
		ClearInst();
		
        return;
	}
}

/*=============================================================================
�� �� ���� ProcConfRollback
��    �ܣ� ���ͻ������ݸ���mcu�ָ����飬��ģʽ�л�����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/30  4.0			������                  ����
=============================================================================*/
void CNPlusInst::ProcConfRollback( const CMessage * pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u16 wEvent;

    switch (cServMsg.GetEventId())
    {
    case MCU_NPLUS_CONFDATAUPDATE_REQ:
        wEvent = MCU_NPLUS_CONFROLLBACK_REQ;
        break;

    case MCU_NPLUS_USRINFOUPDATE_REQ:
        wEvent = MCU_NPLUS_USRROLLBACK_REQ;
        break;

    case MCU_NPLUS_USRGRPUPDATE_REQ:
        wEvent = MCU_NPLUS_GRPROLLBACK_REQ;
        break;

    default: 
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcConfRollback] invalid msg(%s) received.\n", OspEventDesc(cServMsg.GetEventId()));
        return;

    }

    CServMsg cMsg;
    cMsg.SetEventId(wEvent);
    cMsg.SetMsgBody(cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
    SndMsg2NplusMcu(cMsg);

    return;
}

/*=============================================================================
�� �� ���� ProcReset
��    �ܣ� ǿ��ֹͣ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/30  4.0			����                  ����
=============================================================================*/
void CNPlusInst::ProcReset( void )
{
    if ( OspIsValidTcpNode( m_dwMcuNode ) )
    {
        OspDisconnectTcpNode(m_dwMcuNode);
    }

    ClearInst();
}

/*=============================================================================
�� �� ���� ProcRtdRsp
��    �ܣ� ��mcu ���ص�rtd��Ӧ������ 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/23  4.0			������                  ����
=============================================================================*/
void CNPlusInst::ProcRtdRsp( void )
{
    m_wRtdFailTimes = 0;    
    SetTimer(MCU_NPLUS_RTD_TIMER, g_cNPlusApp.GetRtdTime()*1000);   
    LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[ProcRtdRsp] InsId(%d) mcu(0x%x)/(0x%x).\n", GetInsID(), m_dwMcuIpAddr, m_dwMcuIpAddrB); 
    return;
}

/*=============================================================================
�� �� ���� ProcRtdTimeOut
��    �ܣ� �����mcu �Ƿ����������Ķ�ʱ��������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/23  4.0			������                  ����
=============================================================================*/
void CNPlusInst::ProcRtdTimeOut( void )
{
	if( CurState() == STATE_IDLE )
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[ProcRtdTimeOut] invalid ins state(%d)(id.%d)\n", CurState(), GetInsID());
		return;
	}

    LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[ProcRtdTimeOut] Rtd failed %d times!\n", m_wRtdFailTimes);

	m_wRtdFailTimes++;
	
    //��Ҫ����ģʽ�л����ָ���ʵ����Ӧmcu�Ļ�����Ϣ
    if (m_wRtdFailTimes > (g_cNPlusApp.GetRtdNum()) )
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[ProcRtdTimeOut] mode switch and RESTORE mcu(0x%x)/(0x%x) conf.\n", m_dwMcuIpAddr, m_dwMcuIpAddrB);       
        RestoreMcuConf();
        m_dwMcuIId = 0;
        m_dwMcuNode = INVALID_NODE;
        m_dwMcuIIdB = 0;
        m_dwMcuNodeB = INVALID_NODE;
    }
    else
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[ProcRtdTimeOut] rtd time out and retrying mcu(0x%x)/(0x%x).\n", m_dwMcuIpAddr, m_dwMcuIpAddrB); 
        CServMsg cMsg;
        cMsg.SetEventId(MCU_NPLUS_RTD_REQ);
        SndMsg2NplusMcu(cMsg);
        SetTimer(MCU_NPLUS_RTD_TIMER, g_cNPlusApp.GetRtdTime()*1000);
    }    
    return;
}

/*=============================================================================
�� �� ���� ProcDisconnect
��    �ܣ� osp��������������ʱ������ģʽ�л�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/23  4.0			������                  ����
2013/03/19              liaokang              ֧������N+1��˫����
=============================================================================*/
void CNPlusInst::ProcDisconnect( const CMessage * pcMsg )
{
    u32 dwNode = *(u32*)pcMsg->content;
    if( dwNode == m_dwMcuNode )
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[ProcDisconnect] MCU(0x%x) Node.%d Disconnected.\n", m_dwMcuIpAddr, m_dwMcuNode);
        m_dwMcuIId = 0;
        m_dwMcuNode = INVALID_NODE;
    } 
    else
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[ProcDisconnect] MCU(0x%x) Node.%d Disconnected.\n", m_dwMcuIpAddrB, m_dwMcuNodeB);
        m_dwMcuIIdB = 0;
        m_dwMcuNodeB = INVALID_NODE;
    }

    return;
}

/*====================================================================
    ������      : MsgSndPassCheck
    ����        : ��Ϣ���͹���
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    ����ֵ˵��  :
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2013/03/15              liaokang      ����
====================================================================*/
BOOL32 CNPlusInst::MsgSndPassCheck(u16 wEvent)
{
    BOOL32 bRet = TRUE;
    if( !g_cMSSsnApp.JudgeSndMsgPass() &&
        ( MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState() || MCU_NPLUS_MASTER_IDLE == g_cNPlusApp.GetLocalNPlusState() ) )
    {        
        switch(wEvent)
        {            
        case MCU_NPLUS_REG_REQ:
        case MCU_NPLUS_RTD_RSP:
            break;
        default:
            bRet = FALSE;
            break;
        }
    }
    return bRet;
}

/*=============================================================================
�� �� ���� PostReplyBack
��    �ܣ� ����mcuӦ����Ϣ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  u16 wEvent
           u8 *const pbyMsg
           u16 wMsgLen
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/22  4.0			������                  ����
2013/03/18              liaokang              �޸����
=============================================================================*/
void CNPlusInst::PostReplyBack(u16 wEvent, u16 wErrorCode/* = 0*/, u8 *const pbyMsg/* = NULL*/, 
                               u16 wMsgLen/* = 0*/,u32 dwMcuIId/* = 0*/,u32 dwMcuNode/* = 0*/)
{
    CServMsg cServMsg;
    cServMsg.SetEventId(wEvent);
    cServMsg.SetErrorCode(wErrorCode);
    if (wMsgLen > 0 && NULL != pbyMsg)
    {
        cServMsg.SetMsgBody(pbyMsg, wMsgLen);
    }
    SndMsg2NplusMcu(cServMsg,dwMcuIId,dwMcuNode);
    return;
}

/*====================================================================
    ������      : SndMsg2NplusMcu
    ����        : Nplus��Ϣ����
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: CServMsg &cServMsg
                  u32 dwMcuIId
                  u32 dwMcuNode
    ����ֵ˵��  :
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2013/03/15              liaokang      ����
====================================================================*/
void CNPlusInst::SndMsg2NplusMcu(CServMsg &cServMsg,u32 dwMcuIId/* = INVALID_NODE*/,u32 dwMcuNode/* = INVALID_NODE*/)
{   
    if( (INVALID_NODE == dwMcuIId && INVALID_NODE != dwMcuNode ) || 
        (INVALID_NODE != dwMcuIId && INVALID_NODE == dwMcuNode)  )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[SndMsg2NplusMcu] invalid param!\n" );
        return;
    } 

    if( !MsgSndPassCheck(cServMsg.GetEventId()) )
    {
        LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "[SndMsg2NplusMcu] Event.%d<%s> MsgSndPassCheck failed!\n",
            cServMsg.GetEventId(), ::OspEventDesc(cServMsg.GetEventId()));
        return;
    } 

    // ָ��Ŀ��
    if( INVALID_NODE != dwMcuIId && 
        INVALID_NODE != dwMcuNode )
    {
        post(dwMcuIId, cServMsg.GetEventId(), cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), dwMcuNode);
        LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[SndMsg2NplusMcu] send msg.%d<%s> !\n", cServMsg.GetEventId(), ::OspEventDesc(cServMsg.GetEventId()));  
        return;
    } 

    // ��ָ��Ŀ�ģ�double link
    if ( INVALID_NODE != m_dwMcuNode && 
         OspIsValidTcpNode(m_dwMcuNode) ) 
    {
        post(m_dwMcuIId, cServMsg.GetEventId(), cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), m_dwMcuNode);
        LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[SndMsg2NplusMcu] send msg.%d<%s> to Node.%d!\n", cServMsg.GetEventId(), ::OspEventDesc(cServMsg.GetEventId()), m_dwMcuNode);  
    }
    
    if ( INVALID_NODE != m_dwMcuNodeB && 
         OspIsValidTcpNode(m_dwMcuNodeB) )
    {
        post(m_dwMcuIIdB, cServMsg.GetEventId(), cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), m_dwMcuNodeB);
        LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[SndMsg2NplusMcu] send msg.%d<%s> to Node.%d!\n", cServMsg.GetEventId(), ::OspEventDesc(cServMsg.GetEventId()), m_dwMcuNodeB);  
    }

    return;
}

/*=============================================================================
�� �� ���� RestoreMcuConf
��    �ܣ� ģʽ�л����ָ���ʵ����Ӧmcu�Ļ�����Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/23  4.0			������                  ����
=============================================================================*/
void CNPlusInst::RestoreMcuConf( void )
{
    //���״̬��
    if (MCU_NPLUS_SLAVE_IDLE != g_cNPlusApp.GetLocalNPlusState())
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[RestoreMcuConf] invalid NPlusState.%d, restore mcu conf failed.\n",
                  g_cNPlusApp.GetLocalNPlusState());
        
        ClearInst();
        return;
    }

    // ת��
    TranslateEncodingForm();

    // ���⴦��1: N+1 ����ע��GK, ����MCUȡ����MCU����������ע����MCU�����е�ʵ
    //            ��ID����������������ͬGK������£�����MCU�ٿ��Ķ�Ӧ���齫����ʵ�������ն� [12/23/2006-zbq]
	LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"GkIp:%d, GkDriId: %d\n", g_cMcuAgent.GetGkIpAddr(), g_cMcuVcApp.GetRegGKDriId());
    if( g_cMcuAgent.GetGkIpAddr() != 0 && 0 != g_cMcuVcApp.GetRegGKDriId() )
    {
		
        UnRegAllInfoOfSwitchedMcu();
    }

    // ���⴦��2: N+1 ��������GK�Ʒѣ����� MCUȡ����MCU���������Ƚ�����ǰ���ڼ�
    //            �ѵĻ���ļƷѣ������ɻ���������MCU�ָ�����������µĺ�������[12/25/2006-zbq]
    if( g_cMcuAgent.GetGkIpAddr() != 0 && 0 != g_cMcuVcApp.GetRegGKDriId() )
    {
        StopConfChargeOfSwitchedMcu();
    }

    //�л�״̬�����ع���Żָ�״̬��
    g_cNPlusApp.SetLocalNPlusState(MCU_NPLUS_SLAVE_SWITCH);
    
    //��¼��ʵ��id
    g_cNPlusApp.SetMcuSwitchedInsId( (u8)GetInsID(), m_dwMcuIpAddr );
    
    NEXTSTATE(STATE_SWITCHED);

    u8 byIdx;

    //����ǰ�û�����admin��Ͽ���أ���Ϊadmin��֪ͨmcs�����û�����û���Ϣ
    for (byIdx = 1; byIdx <= MAXNUM_MCU_MC; byIdx++)
    {
        if (USRGRPID_SADMIN != CMcsSsn::GetUserGroup(byIdx))
        {
            post(MAKEIID(AID_MCU_MCSSN, byIdx), OSP_DISCONNECT);
        }
        else
        {
            post(MAKEIID(AID_MCU_MCSSN, byIdx), MCS_MCU_GETUSERGRP_REQ);
            post(MAKEIID(AID_MCU_MCSSN, byIdx), MCS_MCU_GETUSERLIST_REQ);
        }
    }
        
    //�ָ�������Ϣ
    TMtAlias        atAlias[MAXNUM_CONF_MT];
    u16             awMtDialBitRate[MAXNUM_CONF_MT];
    s8              achAliasBuf[SERV_MSG_LEN];
    u16             wAliasBufLen = 0;
    TNPlusConfData  *ptConfData;
    TVmpModule      tVmpMod;
    TMtInfo         *ptMtInfo;

    CServMsg cServMsg;
    cServMsg.SetSrcMtId(CONF_CREATE_NPLUS);
    for(byIdx = 0; byIdx < MAXNUM_ONGO_CONF; byIdx++)
    {
        ptConfData = &m_atConfData[byIdx];

        if (ptConfData->IsNull())
        {
            continue;
        }

		// �Զ��ϳ�ģʽ�²�����Ա��������ᵼ���Զ�����ϳɻ���Ļָ��ͻع�ʧ�� [01/12/2007-zbq]
        BOOL32 bVmpMod = ptConfData->m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE ? TRUE : FALSE;
        if (bVmpMod)
        {
            tVmpMod.SetVmpParam(ptConfData->m_tConf.m_tStatus.GetVmpParam());

            // ����ϳɳ�Ա��Ϣֻ�ܱ��浽������Ϣ����ṹֻ����������Ϣ [12/14/2006-zbq]
            for( u8 byIndex = 0; byIndex <  MAXNUM_MPUSVMP_MEMBER; byIndex ++ )
            {
                tVmpMod.m_tVMPParam.ClearVmpMember(byIndex);
            }
        }
		
        for(u8 byLoop = 0; byLoop < ptConfData->m_byMtNum; byLoop++)
        {            
            ptMtInfo = &ptConfData->m_atMtInConf[byLoop];
            atAlias[byLoop].m_AliasType = mtAliasTypeTransportAddress;

			for( u8 bySmcuIdx = 0; bySmcuIdx < MAXNUM_SUB_MCU; bySmcuIdx++ )
			{
				if( m_atConfExData[byIdx].m_atSmcuCallInfo[bySmcuIdx].m_dwMtAddr == ptMtInfo->GetMtAddr())
				{
					atAlias[byLoop].m_AliasType = mtAliasTypeE164;
					atAlias[byLoop].SetE164Alias(m_atConfExData[byIdx].m_atSmcuCallInfo[bySmcuIdx].m_achAlias);
					break;
				}
			}

            atAlias[byLoop].m_tTransportAddr = ptMtInfo->GetMtAddr();
            awMtDialBitRate[byLoop] = /*htons(*/ptMtInfo->GetCallBitrate()/*)*/;

            //����vmpmod���ն�����
            if (bVmpMod && ptConfData->m_tConf.m_tStatus.GetVMPMode() ==  CONF_VMPMODE_CTRL)
            {
                for (u8 byChnl = 0; byChnl < MAXNUM_MPUSVMP_MEMBER; byChnl++)
                {
					u8 byType = ptConfData->m_tVmpInfo.m_abyMemberType[byChnl];
					//���ָ�� ���ó�Ա
					if( VMP_MEMBERTYPE_MCSSPEC == byType && ptConfData->m_tVmpInfo.IsMtInVmpMem(*ptMtInfo, byChnl))
					{
						tVmpMod.SetVmpMember(byChnl, byLoop+1, byType);
					}
					//���� ���ó�Ա193������
					else if( byType != VMP_MEMBERTYPE_NULL && byType != VMP_MEMBERTYPE_MCSSPEC)
					{
						tVmpMod.SetVmpMember(byChnl, MAXNUM_CONF_MT+1, byType);
					}
					else
					{
						//noting to do
					}
                } 
            }
        }

		//��ջ���ϳɳ�Ա״̬ [7/11/2012 chendaiwei]
		TVMPParam tNullVmp;
		memset((void*)&tNullVmp,0,sizeof(tNullVmp));
		ptConfData->m_tConf.m_tStatus.SetVmpParam(tNullVmp);

        PackTMtAliasArray(atAlias, awMtDialBitRate, ptConfData->m_byMtNum, achAliasBuf, wAliasBufLen);

        //ȥ��tvwall��Ϣ����Ϊtvwall����������id��أ����ܽ��лָ�
        ptConfData->m_tConf.SetHasTvwallModule(FALSE);
        ptConfData->m_tConf.SetHasVmpModule(bVmpMod);

        ptConfData->m_tConf.m_tStatus.m_tConfMode.SetTakeMode(CONF_TAKEMODE_ONGOING);   
		
		//���¼������ѯ����ѵ������¼����,HDU��ѯ״̬ [4/27/2013 chendaiwei]
		ptConfData->m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_NONE);
		ptConfData->m_tConf.m_tStatus.SetRollCallMode(ROLLCALL_MODE_NONE);
		ptConfData->m_tConf.m_tStatus.SetNoRecording();
		ptConfData->m_tConf.m_tStatus.SetNoPlaying();
		THduPollInfo tNullInfo;
		ptConfData->m_tConf.m_tStatus.SetHduPollInfo(tNullInfo);

		//��ջ����Զ�¼������[8/6/2013 chendaiwei]
		TConfAutoRecAttrb tNullRecAttrb;
		ptConfData->m_tConf.SetAutoRecAttrb(tNullRecAttrb);

        wAliasBufLen = htons(wAliasBufLen);
        //confinfo
        cServMsg.SetMsgBody((u8*)&ptConfData->m_tConf, sizeof(TConfInfo));

        //mt alias
        cServMsg.CatMsgBody((u8 *)&wAliasBufLen, sizeof(wAliasBufLen));
		cServMsg.CatMsgBody((u8 *)achAliasBuf, ntohs(wAliasBufLen));        

        //vmp
        if (bVmpMod)
        {
            cServMsg.CatMsgBody((u8 *)&tVmpMod, sizeof(tVmpMod));
        }

		//��չ��������ѡ��Ϣ[1/4/2012 chendaiwei]
		u8 *pTConfExInfo = (u8*)&m_atConfExData[byIdx];
		u16 wBuffLen = ntohs(*(u16*)pTConfExInfo);
		if(wBuffLen != 0)
		{
			cServMsg.CatMsgBody(pTConfExInfo,wBuffLen+sizeof(u16));
		}

		cServMsg.CatMsgBody((u8*)&m_atConfExData[byIdx].m_atSmcuCallInfo[0],sizeof(m_atConfExData[byIdx].m_atSmcuCallInfo));

		TNPlusVmpParam atVmpParam[MAXNUM_PERIEQP];
		u8 byVmpNum = 0;
		GetConfMultiVmpParam(byIdx,atVmpParam,byVmpNum);
		cServMsg.CatMsgBody((u8*)&byVmpNum,sizeof(byVmpNum));
		cServMsg.CatMsgBody((u8*)&atVmpParam[0],sizeof(atVmpParam[0])*byVmpNum);

        //����Ϣ��������        
        post(MAKEIID( AID_MCU_VC, CInstance::DAEMON ), MCU_CREATECONF_NPLUS, 
                          cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );			
    }            
    
    return;
}

/*=============================================================================
�� �� ���� GetGrpUsrCount
��    �ܣ� ��ȡ�û������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  u8 byGrpId
           u8 &byMaxNum
           u8 &byNum
�� �� ֵ�� BOOL32  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/24  4.0			������                  ����
=============================================================================*/
BOOL32  CNPlusInst::GetGrpUsrCount( u8 byGrpId, u8 &byMaxNum, u8 &byNum )
{
    // ����ǳ�������Ա����ϵͳֵ
    if (byGrpId == USRGRPID_SADMIN)
    {        
        byMaxNum = MAXNUM_GRPUSRNUM;
        byNum = m_byUsrNum;
        return TRUE;
    }

    TUsrGrpInfo tGrpInfo;
    if (!m_cUsrGrpsInfo.GetGrpById( byGrpId, tGrpInfo ))
    {
        return FALSE;
    }

    byNum = 0;
    byMaxNum = tGrpInfo.GetMaxUsrNum();  
    for (u8 byLoop = 0; byLoop < m_byUsrNum; byLoop++)
    {        
        if ( byGrpId == m_acUsrInfo[byLoop].GetUsrGrpId() )
        {
            byNum ++;
        }
    }

    return TRUE;
}

/*=============================================================================
�� �� ���� GetGrpUserList
��    �ܣ� ��ȡ�û����û��б�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  u8 byGrpId
           u8 *pbyBuf
           u8 &byUsrItr
           u8 &byUserNumInPack
�� �� ֵ�� BOOL32  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/24  4.0			������                  ����
=============================================================================*/
BOOL32  CNPlusInst::GetGrpUserList( u8 byGrpId, u8 *pbyBuf, u8 &byUsrItr, u8 &byUserNumInPack )
{
	if (NULL == pbyBuf)
	{
		return FALSE;
	}

    byUserNumInPack = 0;
	for (; byUserNumInPack < USERNUM_PERPKT && byUsrItr < m_byUsrNum; byUsrItr ++ )
	{
        if ( byGrpId == USRGRPID_SADMIN ||
             byGrpId == m_acUsrInfo[byUsrItr].GetUsrGrpId()  )
        {
			memcpy(pbyBuf + byUserNumInPack * sizeof(CExUsrInfo), &m_acUsrInfo[byUsrItr], sizeof(CExUsrInfo));
			byUserNumInPack++;
        }
	}

	if (0 == byUserNumInPack)
	{
		return FALSE;
	}
    
	return TRUE;
}
    
CExUsrInfo* CNPlusInst::GetUserPtr()
{
    return m_acUsrInfo;
}

CUsrGrpsInfo* CNPlusInst::GetUsrGrpInfo()
{
    return &m_cUsrGrpsInfo;
}

/*=============================================================================
�� �� ���� DaemonInstanceEntry
��    �ܣ� ����Serverģʽ�¸���ʵ����Ϣ�ķַ�������Clientģʽ��demonʵ����Ӧ����mcu
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage * const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/21  4.0			������                  ����
=============================================================================*/
void CNPlusInst::DaemonInstanceEntry( CMessage* const pcMsg, CApp* pcApp )
{
    if ( NULL == pcMsg )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::DaemonInstanceEntry] invalid msg body!\n" );
        return;
    }

    switch( pcMsg->event )
    {
    case OSP_POWERON:
        DaemonPowerOn();
        break;

    case VC_NPLUS_RESET_NOTIF:
    case MCS_MCU_NPLUSRESET_CMD:    // Mcs ����Reset
        DaemonReset();
        break;

    case MCS_MCU_NPLUSROLLBACK_REQ:
        DaemonMcsRollBack(pcMsg, pcApp);
        break;
        
    case OSP_DISCONNECT:
        DaemonDisconnect();
        break;

    case MCU_NPLUS_REG_REQ:
        DaemonRegNPlusMcuReq( pcMsg, pcApp );
        break;

    case MCU_NPLUS_REG_ACK:
    case MCU_NPLUS_REG_NACK:
        DaemonRegNPlusMcuRsp( pcMsg );
        break;

    case MCU_NPLUS_CONNECT_TIMER:
        DaemonConnectNPlusMcuTimeOut(  );
        break;
    
    case MCU_NPLUS_REG_TIMER:
        DaemonRegNPlusMcuTimeOut();
        break;

        //��mcu�����ϱ���Ϣ
    case VC_NPLUS_MSG_NOTIF:  
        DaemonVcNPlusMsgNotif( pcMsg );
        break;

        //����rtd��Ϣ����
    case MCU_NPLUS_RTD_REQ:
        DaemonRtdReq();
        break;

    case MCU_NPLUS_CONFINFOUPDATE_ACK:    
    case MCU_NPLUS_USRGRPUPDATE_ACK:
    case MCU_NPLUS_CONFMTUPDATE_ACK:
	case MCU_NPLUS_SMCUINFOUPDATE_ACK:
    case MCU_NPLUS_CHAIRUPDATE_ACK:    
    case MCU_NPLUS_SPEAKERUPDATE_ACK:    
    case MCU_NPLUS_VMPUPDATE_ACK:    
	case MCU_NPLUS_AUTOMIXUPDATE_ACK:
    case MCU_NPLUS_CONFDATAUPDATE_ACK:    
    case MCU_NPLUS_USRINFOUPDATE_ACK:
    case MCU_NPLUS_RASINFOUPDATE_ACK:
        
    case MCU_NPLUS_CONFINFOUPDATE_NACK:
    case MCU_NPLUS_USRGRPUPDATE_NACK:
    case MCU_NPLUS_CONFMTUPDATE_NACK:
	case MCU_NPLUS_SMCUINFOUPDATE_NACK:
    case MCU_NPLUS_CHAIRUPDATE_NACK:
    case MCU_NPLUS_SPEAKERUPDATE_NACK:
    case MCU_NPLUS_VMPUPDATE_NACK:
    case MCU_NPLUS_CONFDATAUPDATE_NACK:
    case MCU_NPLUS_USRINFOUPDATE_NACK:
    case MCU_NPLUS_RASINFOUPDATE_NACK:
        
        DaemonDataUpdateRsp( pcMsg );
        break;

    //����ع�
    case MCU_NPLUS_CONFROLLBACK_REQ:
        DaemonConfRollbackReq( pcMsg );
        break;
    //�û��ع�
    case MCU_NPLUS_USRROLLBACK_REQ:
        DaemonUsrRollbackReq();
        break;
    //�û���ع�
    case MCU_NPLUS_GRPROLLBACK_REQ:
        DaemonGrpRollbackReq();
        break;
        
    default:
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::DaemonInstanceEntry] invalid event(%d): %s!\n", 
            pcMsg->event, OspEventDesc(pcMsg->event));
        break;
    }

    return;
}   

/*=============================================================================
�� �� ���� DaemonPowerOn
��    �ܣ� �ϵ��ʼ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage * pcMsg
�� �� ֵ�� void  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/21  4.0			������                  ����
=============================================================================*/
void  CNPlusInst::DaemonPowerOn( void )
{    
    //��ΪN+1������������ģʽ������Ҫ�򱸷�ģʽmcu����
    if ( MCU_NPLUS_MASTER_IDLE == g_cNPlusApp.GetLocalNPlusState() )
    {
        SetTimer(MCU_NPLUS_CONNECT_TIMER, 10);  //�������г��Խ���
    }
    return;
}

/*=============================================================================
�� �� ���� DaemonReset
��    �ܣ� ֪ͨN+1���ݷ��������ã�ֹͣ���ݷ��񣬵����Ͽ����ӵĿͻ���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage * pcMsg
�� �� ֵ�� void  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/12/13  4.0			����                  ����
=============================================================================*/
void  CNPlusInst::DaemonReset( void )
{    
    // ��ΪN+1���ݷ�����ģʽ��
    if ( MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState() )
    {
        // ֪ͨ���ڹ�����ʵ��ֹͣ����
        u8 byInsId = g_cNPlusApp.GetMcuSwitchedInsId();
        post(MAKEIID(AID_MCU_NPLUSMANAGER, byInsId), VC_NPLUS_RESET_NOTIF);

        //�������л���
        OspPost(MAKEIID(AID_MCU_VC, CInstance::EACH), MCS_MCU_RELEASECONF_REQ, NULL, 0, 0,
                MAKEIID(AID_MCU_NPLUSMANAGER, GetInsID()));            
    
        //״̬����ת
        g_cNPlusApp.SetLocalNPlusState(MCU_NPLUS_SLAVE_IDLE);        

    }

    return;
}

/*=============================================================================
�� �� ���� DaemonMcsRollBack
��    �ܣ� �û�����ع�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage * pcMsg
�� �� ֵ�� void  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/12/15  4.0			����                  ����
=============================================================================*/
void  CNPlusInst::DaemonMcsRollBack( const CMessage * pcMsg, CApp* pcApp  )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->event );
    // ��ΪN+1���ݷ�����ģʽ��
    if ( MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState() )
    {
        // �Ƿ��Զ��ع�
        TLocalInfo tLocalInfo;
        g_cMcuAgent.GetLocalInfo(&tLocalInfo);
        if ( tLocalInfo.GetIsNPlusRollBack() )
        {
            // Ĭ���Զ��ع���NACK
            cServMsg.SetErrorCode( ERR_MCU_NPLUS_AUTOROLLBACK );
            CMcsSsn::SendMsgToMcsSsn( cServMsg.GetSrcSsnId(), 
                                          pcMsg->event + 2, 
                                          cServMsg.GetServMsg(),
                                          cServMsg.GetServMsgLen() );
            return;
        }

        // ���MCU�Ƿ��Ѿ���������        
        u8 byInsId = g_cNPlusApp.GetMcuSwitchedInsId();
        CNPlusInst *pcInst = (CNPlusInst *)pcApp->GetInstance(byInsId);
        if ( pcInst->CurState() == STATE_NORMAL )
        {
            //�ָ��û�����û���Ϣ
            OspPost(MAKEIID(AID_MCU_MCSSN, CInstance::DAEMON), NPLUS_VC_DATAUPDATE_NOTIF, NULL, 0, 0,
                    MAKEIID(AID_MCU_NPLUSMANAGER, byInsId)); 

            //�ָ�����������Ϣ
            OspPost(MAKEIID(AID_MCU_VC, CInstance::EACH), NPLUS_VC_DATAUPDATE_NOTIF, NULL, 0, 0,
                    MAKEIID(AID_MCU_NPLUSMANAGER, byInsId));            
    
            //״̬����ת
            g_cNPlusApp.SetLocalNPlusState(MCU_NPLUS_SLAVE_IDLE);    

            pcInst->ClearInst();
        }
        else
        {
            // ����MCUδ���ӣ�NACK
            cServMsg.SetErrorCode( ERR_MCU_NPLUS_BAKCLIENT_NOTREG );
            CMcsSsn::SendMsgToMcsSsn( cServMsg.GetSrcSsnId(), 
                                          pcMsg->event + 2, 
                                          cServMsg.GetServMsg(),
                                          cServMsg.GetServMsgLen() );
            return;            
        }    
    }
}

/*=============================================================================
�� �� ���� DaemonDisconnect
��    �ܣ� ����mcu��������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  const CMessage * pcMsg
           CApp* pcApp
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/30  4.0			������                  ����
=============================================================================*/
void CNPlusInst::DaemonDisconnect( void  )
{
    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonDisconnect] node(%d) disconnect. connecting...\n", m_dwMcuNode);
    KillTimer( MCU_NPLUS_REG_TIMER );    
    ClearInst();
    g_cNPlusApp.SetLocalNPlusState(MCU_NPLUS_MASTER_IDLE);

    SetTimer( MCU_NPLUS_CONNECT_TIMER, NPLUS_CONNECTMCU_TIMEOUT );
    return;
}

/*=============================================================================
�� �� ���� DaemonDataUpdateRsp
��    �ܣ� ���ݸ�����Ϣ��Ӧ������ 
�㷨ʵ�֣� Ŀǰֻ�����������ò��㵼�µ�ͬ��ʧ��
ȫ�ֱ����� 
��    ����  const CMessage * pcMsg
           CApp* pcApp
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/28  4.0			������                  ����
=============================================================================*/
void CNPlusInst::DaemonDataUpdateRsp( const CMessage * pcMsg )
{
    if ( pcMsg->event == MCU_NPLUS_CONFINFOUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_VMPUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_CONFDATAUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_CONFMTUPDATE_ACK ||
		 pcMsg->event == MCU_NPLUS_SMCUINFOUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_CHAIRUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_SPEAKERUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_USRINFOUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_USRGRPUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_RASINFOUPDATE_ACK ||
		 pcMsg->event == MCU_NPLUS_AUTOMIXUPDATE_ACK)
    {
        g_cNPlusApp.SetNPlusSynOk(TRUE);
    }
    else if ( pcMsg->event == MCU_NPLUS_CONFINFOUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_VMPUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_CONFDATAUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_CONFMTUPDATE_NACK ||
			  pcMsg->event == MCU_NPLUS_SMCUINFOUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_CHAIRUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_SPEAKERUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_USRINFOUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_USRGRPUPDATE_NACK ||
			  pcMsg->event == MCU_NPLUS_AUTOMIXUPDATE_NACK)
    {
        g_cNPlusApp.SetNPlusSynOk(FALSE);
    }
    return;
}

/*=============================================================================
�� �� ���� DaemonConfRollbackReq
��    �ܣ� ������Ϣ�ع�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  const CMessage * pcMsg
           CApp* pcApp
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/30  4.0			������                  ����
=============================================================================*/
void CNPlusInst::DaemonConfRollbackReq( const CMessage * pcMsg )
{
    if (g_cNPlusApp.GetLocalNPlusState() != MCU_NPLUS_MASTER_CONNECTED)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonConfRollbackReq] invalid N+1 mode state(%d), ignore conf rollback msg\n", 
                  g_cNPlusApp.GetLocalNPlusState());
        return;
    }

    //ack
    PostReplyBack(pcMsg->event+1);

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    //�������Ѿ�ͨ�������Զ��ָ��ٿ����Ƚ������ٽ��лָ�

	u8 *pbyMsgBuf = cServMsg.GetMsgBody();
    TNPlusConfData tConfData = *(TNPlusConfData *)pbyMsgBuf;
	pbyMsgBuf += sizeof(tConfData);

	u8 abyBuffer[CONFINFO_EX_BUFFER_LENGTH] = {0};
	u16 wBufferLen = 0; //��չ���ݳ���[11/16/2012 chendaiwei]
	
	TNPlusConfExData tConfExData;
	if(tConfData.m_tConf.HasConfExInfo())
	{
		wBufferLen = ntohs(*(u16*)(pbyMsgBuf))+sizeof(u16);
		memcpy(tConfExData.m_byConInfoExBuf,pbyMsgBuf,wBufferLen);

		pbyMsgBuf += wBufferLen;
	}

	if(cServMsg.GetMsgBodyLen()>sizeof(TNPlusConfData)+wBufferLen)
	{
		memcpy(tConfExData.m_atSmcuCallInfo,pbyMsgBuf,sizeof(tConfExData.m_atSmcuCallInfo));

		pbyMsgBuf += sizeof(tConfExData.m_atSmcuCallInfo);
	}
	
	TNPlusVmpParam atVmpParam[MAXNUM_PERIEQP];
	u8 byVmpNum = 0;

	if(cServMsg.GetMsgBodyLen() > sizeof(TNPlusConfData)+wBufferLen +sizeof(TSmcuCallnfo)*MAXNUM_SUB_MCU)
	{
		u16 wPackLen = 0;
		if(!UnPackVmpBufToVmpParam(pbyMsgBuf,atVmpParam,byVmpNum,wPackLen))
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonConfRollbackReq] UnPackVmpBufToVmpParam failed\n");

			return;
		}
	}

    if (tConfData.IsNull())
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonConfRollbackReq] confid is null, ignore conf rollback msg\n");
        return;
    }

    //�ָ�������Ϣ
    TMtAlias        atAlias[MAXNUM_CONF_MT];
    u16             awMtDialBitRate[MAXNUM_CONF_MT];
    s8              achAliasBuf[SERV_MSG_LEN];
    u16             wAliasBufLen = 0;
    TVmpModule      tVmpMod;
    TMtInfo         *ptMtInfo;

    cServMsg.SetSrcMtId(CONF_CREATE_NPLUS);    

	// �Զ��ϳ�ģʽ�²�����Ա��������ᵼ���Զ�����ϳɻ���Ļָ��ͻع�ʧ�� [01/12/2007-zbq]
    BOOL32 bVmpMod = tConfData.m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE ? TRUE : FALSE;
    if (bVmpMod)
    {
        tVmpMod.SetVmpParam(tConfData.m_tConf.m_tStatus.GetVmpParam());

        // ����ϳɳ�Ա��Ϣֻ�ܱ��浽������Ϣ����ṹֻ����������Ϣ [12/14/2006-zbq]
        for( u8 byIndex = 0; byIndex <  MAXNUM_MPUSVMP_MEMBER; byIndex ++ )
        {
            tVmpMod.m_tVMPParam.ClearVmpMember(byIndex);
        }
    }

	TVmpModuleInfo tVmpModuleInfo;

    for(u8 byLoop = 0; byLoop < tConfData.m_byMtNum; byLoop++)
    {            
        ptMtInfo = &tConfData.m_atMtInConf[byLoop];

        atAlias[byLoop].m_AliasType = mtAliasTypeTransportAddress;
		for( u8 bySmcuIdx = 0; bySmcuIdx < MAXNUM_SUB_MCU; bySmcuIdx++ )
		{
			if( tConfExData.m_atSmcuCallInfo[bySmcuIdx].m_dwMtAddr == ptMtInfo->GetMtAddr())
			{
				atAlias[byLoop].m_AliasType = mtAliasTypeE164;
				atAlias[byLoop].SetE164Alias(tConfExData.m_atSmcuCallInfo[bySmcuIdx].m_achAlias);
				break;
			}
		}

        atAlias[byLoop].m_tTransportAddr = ptMtInfo->GetMtAddr();
        awMtDialBitRate[byLoop] = ptMtInfo->GetCallBitrate();

        //����vmpmod���ն�����
        if (bVmpMod && tConfData.m_tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_CTRL)
        {
            for (u8 byChnl = 0; byChnl < MAXNUM_MPUSVMP_MEMBER; byChnl++)
            {
				u8 byType = tConfData.m_tVmpInfo.m_abyMemberType[byChnl];

				//���ָ�� ���ó�Ա
				if( VMP_MEMBERTYPE_MCSSPEC == byType && tConfData.m_tVmpInfo.IsMtInVmpMem(*ptMtInfo, byChnl))
				{
					tVmpMod.SetVmpMember(byChnl, byLoop+1, byType);
				}
				//���� ���ó�Ա193������
				else if( byType != VMP_MEMBERTYPE_NULL && byType != VMP_MEMBERTYPE_MCSSPEC)
				{
					tVmpMod.SetVmpMember(byChnl, MAXNUM_CONF_MT+1, byType);
				}
				else
				{
					//noting to do
				}
			} 
        }
    }

	//��ջ���ϳɳ�Ա״̬ [7/11/2012 chendaiwei]
	TVMPParam tNullVmp;
	memset((void*)&tNullVmp,0,sizeof(tNullVmp));
	tConfData.m_tConf.m_tStatus.SetVmpParam(tNullVmp);

    PackTMtAliasArray(atAlias, awMtDialBitRate, tConfData.m_byMtNum, achAliasBuf, wAliasBufLen);

    //ȥ��tvwall��Ϣ����Ϊtvwall����������id��أ����ܽ��лָ�
    tConfData.m_tConf.SetHasTvwallModule(FALSE);
    tConfData.m_tConf.SetHasVmpModule(bVmpMod);

    tConfData.m_tConf.m_tStatus.m_tConfMode.SetTakeMode(CONF_TAKEMODE_ONGOING);
	
	//���¼������ѯ����ѵ������¼����,HDU��ѯ״̬ [4/27/2013 chendaiwei]
	tConfData.m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_NONE);
	tConfData.m_tConf.m_tStatus.SetRollCallMode(ROLLCALL_MODE_NONE);
    tConfData.m_tConf.m_tStatus.SetNoRecording();
    tConfData.m_tConf.m_tStatus.SetNoPlaying();
	THduPollInfo tNullInfo;
	tConfData.m_tConf.m_tStatus.SetHduPollInfo(tNullInfo);

	//��ջ����Զ�¼������[8/6/2013 chendaiwei]
	TConfAutoRecAttrb tNullRecAttrb;
	tConfData.m_tConf.SetAutoRecAttrb(tNullRecAttrb);

    wAliasBufLen = htons(wAliasBufLen);
    //confinfo
    cServMsg.SetMsgBody((u8*)&tConfData.m_tConf, sizeof(TConfInfo));
	
    //mt alias
    cServMsg.CatMsgBody((u8 *)&wAliasBufLen, sizeof(wAliasBufLen));
	cServMsg.CatMsgBody((u8 *)achAliasBuf, ntohs(wAliasBufLen));        

    //vmp
    if (bVmpMod)
    {
        cServMsg.CatMsgBody((u8 *)&tVmpMod, sizeof(tVmpMod));
    }

	if(tConfData.m_tConf.HasConfExInfo() && wBufferLen > 0)
	{		
		cServMsg.CatMsgBody((u8*)&tConfExData.m_byConInfoExBuf[0],wBufferLen);
	}

	cServMsg.CatMsgBody((u8*)&tConfExData.m_atSmcuCallInfo[0],sizeof(tConfExData.m_atSmcuCallInfo));

	//TODO:��ɾ��
	for(u8 byIdx = 0; byIdx < byVmpNum; byIdx++ )
	{
		atVmpParam[byIdx].m_tVmpBaiscParam.Print();
	}

	cServMsg.CatMsgBody((u8*)&byVmpNum,sizeof(byVmpNum));
	cServMsg.CatMsgBody((u8*)&atVmpParam[0],sizeof(atVmpParam[0])*byVmpNum);

    //����Ϣ��������        
    post(MAKEIID( AID_MCU_VC, CInstance::DAEMON ), MCU_CREATECONF_NPLUS, 
                    cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        
    return;
}

/*=============================================================================
�� �� ���� DaemonUsrRollbackReq
��    �ܣ� �û���Ϣ�ع�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  const CMessage * pcMsg
           CApp* pcApp
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/30  4.0			������                  ����
=============================================================================*/
void CNPlusInst::DaemonUsrRollbackReq( void )
{
    //����Ҫ������
    return;
}

/*=============================================================================
�� �� ���� DaemonGrpRollbackReq
��    �ܣ� �û�����Ϣ�ع�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  const CMessage * pcMsg
           CApp* pcApp
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/30  4.0			������                  ����
=============================================================================*/
void CNPlusInst::DaemonGrpRollbackReq( void )
{
    //����Ҫ������
    return;
}

/*=============================================================================
�� �� ���� DaemonConnectNPlusMcu
��    �ܣ� �����򱸷�mcu����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/22  4.0			������                  ����
=============================================================================*/
void CNPlusInst::DaemonConnectNPlusMcuTimeOut( void )
{
    if ( !OspIsValidTcpNode(m_dwMcuNode) )
    {
        TLocalInfo tLocalInfo;
        if (SUCCESS_AGENT == g_cMcuAgent.GetLocalInfo(&tLocalInfo))
        {
            m_dwMcuNode = OspConnectTcpNode( htonl(tLocalInfo.GetNPlusMcuIp()), MCU_LISTEN_PORT );
            if ( !OspIsValidTcpNode(m_dwMcuNode) )
            {
                m_dwMcuNode = INVALID_NODE;
			    LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"OspConnectTcpNode MCU(0x%x) Failed!\n", tLocalInfo.GetNPlusMcuIp());
			    SetTimer( MCU_NPLUS_CONNECT_TIMER, NPLUS_CONNECTMCU_TIMEOUT );
			    return;
            }
            OspNodeDiscCBRegQ(m_dwMcuNode, GetAppID(), GetInsID());
        }
        else
        {
            LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonConnectNPlusMcu] mcu agent get localinfo failed.\n");
            return;
        }
    }

    //����ע������
    SetTimer( MCU_NPLUS_REG_TIMER, 10 );
    return;
}

/*=============================================================================
�� �� ���� DaemonRegNPlusMcu
��    �ܣ� �򱸷�mcu����ע������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/22  4.0			������                  ����
=============================================================================*/
void CNPlusInst::DaemonRegNPlusMcuTimeOut()
{
    if (CurState() != STATE_IDLE)
    {
        return;
    }

    TNPlusMcuRegInfo tRegInfo;
    tRegInfo.SetMcuIpAddr( ntohl(g_cMcuAgent.GetMpcIp()));
    tRegInfo.SetMcuEqpCap( g_cNPlusApp.GetMcuEqpCap() );
    tRegInfo.SetMcuType( g_cNPlusApp.GetMcuType() );

    TNPlusMcuInfoEx tRegInfoEx;
    //[5/7/2013 liaokang] ���ӱ��뷽ʽ
#ifdef _UTF8
    tRegInfoEx.SetMcuEncodingForm(emenCoding_Utf8);
#else
    tRegInfoEx.SetMcuEncodingForm(emenCoding_GBK); 
#endif    
    tRegInfoEx.SetCurMSState(g_cMSSsnApp.GetCurMSState());
    // ������˫��
    if( g_cMSSsnApp.IsDoubleLink() )
    {        
        tRegInfoEx.SetAnotherMpcIp(g_cMSSsnApp.GetAnotherMcuIP());
    }
    
    u8 abyBuffer[NPLUS_PACK_EXINFO_BUF_LEN] = {0};
	u16 wLen = 0;
    g_cNPlusApp.PackNplusExInfo(g_cNPlusApp.GetMcuEqpCapEx(),abyBuffer,wLen,&tRegInfoEx);
    CServMsg cMsg;
    cMsg.SetEventId(MCU_NPLUS_REG_REQ);
	cMsg.SetMsgBody((u8*)&tRegInfo,sizeof(tRegInfo));
	cMsg.CatMsgBody((u8*)&abyBuffer[0],wLen);
    SndMsg2NplusMcu(cMsg,MAKEIID(AID_MCU_NPLUSMANAGER, CInstance::DAEMON),m_dwMcuNode);
    SetTimer( MCU_NPLUS_REG_TIMER, NPLUS_REGMCU_TIMEOUT );
    return;
}

/*=============================================================================
�� �� ���� DaemonRegNPlusMcuReq
��    �ܣ� ��mcu��������mcu��ע����Ϣ������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/22  4.0			������                  ����
=============================================================================*/
void CNPlusInst::DaemonRegNPlusMcuReq( const CMessage * pcMsg, CApp* pcApp )
{
    //��MCU��֧��N+1���� �� N+1�����±������ã��ܾ�����MCU������������ ��ע��ɹ� �� ��������[12/28/2006-zbq]
    if ( MCU_NPLUS_IDLE == g_cNPlusApp.GetLocalNPlusState() ||
        MCU_NPLUS_MASTER_IDLE == g_cNPlusApp.GetLocalNPlusState() ||
        MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState() )
    {
        DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_CONNECT_MASTER );
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    if (sizeof(TNPlusMcuRegInfo) > cServMsg.GetMsgBodyLen())
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonRegNPlusMcuReq] cServMsg.GetMsgBodyLen()=%d, sizeof(TNPlusMcuRegInfo)=%d !\n",
                   cServMsg.GetMsgBodyLen(), sizeof(TNPlusMcuRegInfo));
        DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_MSGLEN_ERR );
        return;
    }

    TNPlusMcuRegInfo *ptRegInfo = (TNPlusMcuRegInfo *)(cServMsg.GetMsgBody());

    //��鱸�ݻ�����������8000B����8000 [12/20/2006-zbq]
    if ( ( MCU_TYPE_KDV8000B == g_cNPlusApp.GetMcuType() && 
           MCU_TYPE_KDV8000  == ptRegInfo->GetMcuType() ) ||
         // guzh [3/8/2007] Ҳ������8000C����8000/8000B
         ( MCU_TYPE_KDV8000C == g_cNPlusApp.GetMcuType() &&
           ( MCU_TYPE_KDV8000 == ptRegInfo->GetMcuType() ||
             MCU_TYPE_KDV8000B == ptRegInfo->GetMcuType() ) ) ||
         // guzh [3/8/2007] ������8000/8000B ���� 8000C
         ( MCU_TYPE_KDV8000C == ptRegInfo->GetMcuType() &&
           ( MCU_TYPE_KDV8000 == g_cNPlusApp.GetMcuType() ||
             MCU_TYPE_KDV8000B == g_cNPlusApp.GetMcuType() ) ) )
    {
        DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_ENV_UNMATCHED );
        return;
    }

    //�����������
	TNPlusEqpCapEx tRemoteCapEx;
    TNPlusMcuInfoEx tRemoteMcuInfoEx;
    tRemoteMcuInfoEx.SetCurMSState(MCU_MSSTATE_ACTIVE);  //Ĭ�϶Զ�Ϊ����
    tRemoteMcuInfoEx.SetMcuEncodingForm(emenCoding_GBK); //Ĭ�϶Զ�ΪGBK����
	if(cServMsg.GetMsgBodyLen() > sizeof(TNPlusMcuRegInfo))
	{
		BOOL32 bUnkownInfo = FALSE;
		u16 wUnpackLen = 0;
        g_cNPlusApp.UnPackNplusExInfo(tRemoteCapEx,cServMsg.GetMsgBody() + sizeof(TNPlusMcuRegInfo),wUnpackLen,bUnkownInfo,&tRemoteMcuInfoEx);
		if(bUnkownInfo)
		{
			DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_CAP_ERR );
			return;
		}
	}

    // ��֧��gbk�����mcu����utf8�����mcu
#ifndef _UTF8
    if( emenCoding_Utf8 == tRemoteMcuInfoEx.GetMcuEncodingForm() )
    {         
        DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_CHECKENCODING_ERR );
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonRegNPlusMcuReq] Do not support GBK backup Utf8, Nack!\n");
        return;
    }
#endif

    TNPlusEqpCap tLocalCap = g_cNPlusApp.GetMcuEqpCap();
	TNPlusEqpCapEx tLocalCapEx = g_cNPlusApp.GetMcuEqpCapEx();	
	TNPlusEqpCap tRemoteCap = ptRegInfo->GetMcuEqpCap();	
	if(IsLocalEqpCapLowerThanRemote(tLocalCap,tLocalCapEx,tRemoteCap,tRemoteCapEx))
	{
        DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_CAP_ERR );
        return;
	}

    BOOL32 bFind = FALSE;
    // �Ȳ����Ƿ�Ϊswitch��inst
    CNPlusInst *pcInst = NULL;
    u8 byInsId = g_cNPlusApp.GetMcuSwitchedInsId();
    if( 0 != byInsId )
    {
        pcInst = (CNPlusInst *)pcApp->GetInstance(byInsId);
        if (NULL != pcInst)
        { 

            if ( (pcInst->GetMcuIpAddr() == ptRegInfo->GetMcuIpAddr() && pcInst->GetMcuIpAddrB() == tRemoteMcuInfoEx.GetAnotherMpcIp() ) || 
                (pcInst->GetMcuIpAddrB() == ptRegInfo->GetMcuIpAddr() && pcInst->GetMcuIpAddr() == tRemoteMcuInfoEx.GetAnotherMpcIp() ) )
            {
                if( MCU_MSSTATE_STANDBY == tRemoteMcuInfoEx.GetCurMSState() )
                {
                    // ���ð岻�ܴ����ع�,�ȶ���
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonRegNPlusMcuReq] standby board must register after active board in switch state, disconnect node.%d!\n",pcMsg->srcnode);
                    OspDisconnectTcpNode(pcMsg->srcnode);
                    return;
                } 
                else
                {
                    bFind = TRUE;
                }
            } 

        }
    }

    // �ٲ����Ƿ�ΪNormal��inst
    if( FALSE == bFind )
    {
        for( byInsId = 1; byInsId <= MAXNUM_NPLUS_MCU; byInsId++ )
        {
            pcInst = (CNPlusInst *)pcApp->GetInstance(byInsId);
            if (NULL == pcInst || STATE_NORMAL != pcInst->CurState())
            {
                continue;
            }            

            if ( (pcInst->GetMcuIpAddr() == ptRegInfo->GetMcuIpAddr() && pcInst->GetMcuIpAddrB() == tRemoteMcuInfoEx.GetAnotherMpcIp() ) || 
                 (pcInst->GetMcuIpAddrB() == ptRegInfo->GetMcuIpAddr() && pcInst->GetMcuIpAddr() == tRemoteMcuInfoEx.GetAnotherMpcIp() ) )
            {
                if( pcInst->GetMcuEncodingForm() != tRemoteMcuInfoEx.GetMcuEncodingForm() )
                {
                    // ǰ����뷽ʽ��һ�£�NACK
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonRegNPlusMcuReq] N mode Mcu Encoding Form is Inconsistent, Nack!\n");
                    DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_CHECKENCODING_ERR );
                    return;
                }

                bFind = TRUE;
                break;
            }
        } 
    }

    // ����Idle��inst
    if( FALSE == bFind )
    {
        for( byInsId = 1; byInsId <= MAXNUM_NPLUS_MCU; byInsId++ )
        {
            pcInst = (CNPlusInst *)pcApp->GetInstance(byInsId);
            if (NULL != pcInst && STATE_IDLE == pcInst->CurState())
            {
                bFind = TRUE;
                break;
            }
        } 
    }

    if( TRUE == bFind )
    {
        cServMsg.CatMsgBody((u8 *)&pcMsg->srcid, sizeof(pcMsg->srcid));
        cServMsg.CatMsgBody((u8 *)&pcMsg->srcnode, sizeof(pcMsg->srcnode));
        post(MAKEIID(AID_MCU_NPLUSMANAGER, byInsId), pcMsg->event, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[DaemonRegNPlusMcuReq] reg to inst %d!\n", byInsId);        
    }
    else
    {
        DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_OVERLOAD );
        LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[DaemonRegNPlusMcuReq] no enough idle inst !\n");
    }

    return;
}

/*=============================================================================
�� �� ���� DaemonRegNack
��    �ܣ� ��mcu��ע��ܾ�ͳһ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/12/21  4.0			�ű���                  ����
=============================================================================*/
void CNPlusInst::DaemonRegNack( const CMessage * pcMsg, u8 byReason )
{
    CServMsg cServMsg;
    cServMsg.SetEventId( pcMsg->event + 2 );
    cServMsg.SetErrorCode( byReason );
    SndMsg2NplusMcu(cServMsg,pcMsg->srcid,pcMsg->srcnode);
    OspDisconnectTcpNode(pcMsg->srcnode);

    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonRegNPlusMcuReq] NPlus reg failed due to reason<%d>, disconnect!\n", byReason);
    return;
}

/*=============================================================================
�� �� ���� DaemonRegNPlusMcuRsp
��    �ܣ� ����mcu��ע����Ӧ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/22  4.0			������                  ����
=============================================================================*/
void CNPlusInst::DaemonRegNPlusMcuRsp( const CMessage * pcMsg )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );

    if ( MCU_NPLUS_REG_ACK == pcMsg->event )
    {
        //ע��ɹ���Ļع��������������������л��顣��ֹ�û����ڱ����Ѿ�����
        //�������ڻع��󱾵� ��Ȼ���� �ļ��෢����ֻ���������绷����ʱ���á�[01/09/2006-zbq]
        u8 byRollBack = *(u8*)cServMsg.GetMsgBody();
        if ( 1 == byRollBack )
        {
            OspPost(MAKEIID(AID_MCU_VC, CInstance::EACH), MCS_MCU_RELEASECONF_REQ,
                           NULL, 0, 0, MAKEIID(AID_MCU_NPLUSMANAGER, GetInsID()));    
        }

		//zjj20130918 ����ʹ�ñ����������������Դﵽ����ͬʱ��⣬��ֹ���������������״̬��һ�����
		if( cServMsg.GetMsgBodyLen() > sizeof(u8) )
		{		
			u16 wRtdTime = *(u16*)(cServMsg.GetMsgBody() + sizeof(u8) );
			u16 wRtdNum  = *(u16*)(cServMsg.GetMsgBody() + + sizeof(u8) + sizeof(u16) );
			wRtdTime     = ntohs(wRtdTime);
			wRtdNum		 = ntohs(wRtdNum);
			OspSetHBParam( m_dwMcuNode, wRtdTime, (u8)wRtdNum );
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "[DaemonRegNPlusMcuRsp] disconnect set %d.%d\n",
					wRtdTime,wRtdNum );
		}

        KillTimer( MCU_NPLUS_REG_TIMER );
        //�����id�����漴����ֱ���뱸��mcu����Ӧʵ��ͨ��
        m_dwMcuIId = pcMsg->srcid;

        g_cNPlusApp.SetLocalNPlusState(MCU_NPLUS_MASTER_CONNECTED);

        NEXTSTATE( STATE_NORMAL);

        //ͬ������������Ϣ
        OspPost(MAKEIID(AID_MCU_VC, CInstance::EACH), NPLUS_VC_DATAUPDATE_NOTIF, NULL, 0, 0,
                MAKEIID(AID_MCU_NPLUSMANAGER, CInstance::DAEMON));

        //ͬ���û�����û���Ϣ
        OspPost(MAKEIID(AID_MCU_MCSSN, CInstance::DAEMON), NPLUS_VC_DATAUPDATE_NOTIF, NULL, 0, 0,
                MAKEIID(AID_MCU_NPLUSMANAGER, CInstance::DAEMON));

        //ͬ��RAS��Ϣ����ʹ�ñ��嵹�����ٴ�ע��ǰ��ע�������ɹ� [12/18/2006-zbq]
        if ( 0 != g_cMcuAgent.GetGkIpAddr() )
        {
            // �������MCU�����ڱ���MCU��������ܿ��ܱ���MCU��RAS��Ϣ���б��������ȴ�������RCF����������
            if ( 0 != g_cMcuVcApp.GetH323EPIDAlias()->GetIDlength() )
            {
                TRASInfo tRASInfo;
                tRASInfo.SetEPID( g_cMcuVcApp.GetH323EPIDAlias() );
                tRASInfo.SetGKID( g_cMcuVcApp.GetH323GKIDAlias() );
                tRASInfo.SetGKIp( ntohl(g_cMcuAgent.GetGkIpAddr()) );
                tRASInfo.SetRRQIp( g_cMcuAgent.GetRRQMtadpIp() );

                // zbq [03/15/2007] ͬ��RAS��Ϣ������MCU��E164һ��
                TLocalInfo tLocalInfo;
                g_cMcuAgent.GetLocalInfo( &tLocalInfo );
                tRASInfo.SetMcuE164( tLocalInfo.GetE164Num() );
                
                CServMsg cSendServMsg;
                cSendServMsg.SetMsgBody( (u8*)&tRASInfo, sizeof(tRASInfo) );
                cSendServMsg.SetEventId( MCU_NPLUS_RASINFOUPDATE_REQ );

                SndMsg2NplusMcu(cSendServMsg);
            }
            else
            {
                LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[DaemonRegNPlusMcuRsp] Rasinfo invalid, the RRQ mtadp will notify the backup mcu in NO TIME !\n");
            }
        }
    }
    else
    {
        //�ϱ���ҵ������ע��ʧ�ܵ�ԭ�� [12/18/2006-zbq]
        OspPost(MAKEIID(AID_MCU_VC, CInstance::DAEMON), pcMsg->event, NULL, 0);
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonRegNPlusMcuRsp] receive register nack due to Reason.%d.\n", cServMsg.GetErrorCode());
    }
    return;
}

/*=============================================================================
�� �� ���� DaemonVcNPlusMsgNotif
��    �ܣ� ��������֪ͨ����mcu
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  const CMessage * pcMsg
           CApp* pcApp
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/22  4.0			������                  ����
=============================================================================*/
void CNPlusInst::DaemonVcNPlusMsgNotif( const CMessage * pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    if (CurState() != STATE_NORMAL)
    {
//        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonVcNPlusMsgNotif] invalid ins state(%d), ignore it(%s)\n", 
//            CurState(), OspEventDesc(cServMsg.GetEventId()));
        return;
    }

    SndMsg2NplusMcu(cServMsg);
    return;
}

/*=============================================================================
�� �� ���� DaemonRtdReq
��    �ܣ� ��mcu rtd��������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  const CMessage * pcMsg
           CApp* pcApp
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/12/5  4.0			������                  ����
=============================================================================*/
void CNPlusInst::DaemonRtdReq( void )
{
    LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[DaemonRtdReq] receive rtd request.\n");
    PostReplyBack(MCU_NPLUS_RTD_RSP);
    return;
}

/*=============================================================================
�� �� ���� SetConfData
��    �ܣ� �������������Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TNPlusConfData *ptConfData
�� �� ֵ�� BOOL32  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/24  4.0			������                  ����
=============================================================================*/
BOOL32  CNPlusInst::SetConfData( TNPlusConfData *ptConfData )
{
    if (NULL == ptConfData)
    {
        return FALSE;
    }

    u8 byIdx = GetConfIdxByConfId(ptConfData->m_tConf.GetConfId());
    if (MAXNUM_ONGO_CONF == byIdx)//modify
    {
        byIdx = GetEmptyConfIdx();//new
    }    

    if (MAXNUM_ONGO_CONF == byIdx)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::SetConfData] get conf index failed by cconfid:");
        ptConfData->m_tConf.GetConfId().Print();
        return FALSE;
    }

    memcpy(&m_atConfData[byIdx], ptConfData, sizeof(TNPlusConfData));
    return TRUE;
}

/*=============================================================================
�� �� ���� SetConfExData
��    �ܣ� ���������չ��Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� [IN]CConfId cConfId
		   [IN]u8 *pbybuff
		   [IN]u16 wBufLen
�� �� ֵ�� BOOL32  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2012/1/10   4.0			�´�ΰ                  ����
=============================================================================*/
BOOL32  CNPlusInst::SetConfExData(CConfId cConfId, u8 *pbybuff,u16 wBufLen )
{
    if (NULL == pbybuff || wBufLen> CONFINFO_EX_BUFFER_LENGTH)
    {
        return FALSE;
    }

    u8 byIdx = GetConfIdxByConfId(cConfId);
    if (MAXNUM_ONGO_CONF == byIdx)
    {
        return FALSE;
    }
    else
	{
		memcpy((u8*)&m_atConfExData[byIdx],pbybuff,wBufLen);
		return TRUE;
	}
}

/*=============================================================================
�� �� ���� SetConfInfo
��    �ܣ� ���������Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  TConfInfo *ptConfInfo
�� �� ֵ�� BOOL32  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/24  4.0			������                  ����
=============================================================================*/
BOOL32  CNPlusInst::SetConfInfo( TConfInfo *ptConfInfo,BOOL32 bStart )
{
    if (NULL == ptConfInfo)
    {
        return FALSE;
    }

    u8 byIdx = GetConfIdxByConfId(ptConfInfo->GetConfId());
    if (MAXNUM_ONGO_CONF == byIdx)
    {
        byIdx = GetEmptyConfIdx();//new     
    }

    if (MAXNUM_ONGO_CONF == byIdx)
    {        
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::SetConfInfo] get conf index failed by cconfid:");
        ptConfInfo->GetConfId().Print();
        return FALSE;
    }

    if(bStart)
    {
        TConfInfo *ptConf = &m_atConfData[byIdx].m_tConf;
        TMtAlias tChair = ptConf->GetChairAlias();
        TMtAlias tSpeaker = ptConf->GetSpeakerAlias();
        memcpy(ptConf, ptConfInfo, sizeof(TConfInfo));
        ptConf->SetChairAlias(tChair);
        ptConf->SetSpeakerAlias(tSpeaker);
    }
    else
    {
		//1.��ն�vmp����
		ClearVmpParamByConfIdx(byIdx);
		
		//2.���ex����
		m_atConfExData[byIdx].Clear();

        m_atConfData[byIdx].Clear();
    }
    
    return TRUE;
}

/*=============================================================================
�� �� ���� SetConfAutoMix
��    �ܣ� ���û����Ƿ�֧�����ܻ���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CConfId  tConfId����Žṹ  BOOL32 bStart TRUE��ʾ�������ܻ��� FALSE��ʾ
		   ֹͣ���ܻ��� 
�� �� ֵ�� BOOL32  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2010/09/26  			�´�ΰ                  ����
=============================================================================*/
BOOL32  CNPlusInst::SetConfAutoMix( CConfId &cConfId, BOOL32 bStart )
{
    u8 byIdx = GetConfIdxByConfId(cConfId);

    if (MAXNUM_ONGO_CONF == byIdx)
    {        
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::SetConfAutoMix] get conf index failed by cconfid:");
        cConfId.Print();
        return FALSE;
    }

	m_atConfData[byIdx].m_tConf.m_tStatus.SetAutoMixing(bStart);
    
    return TRUE;
}
/*=============================================================================
�� �� ���� SetConfMtInfo
��    �ܣ� ��������ն���Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  CConfId *pcConfId
           TMtInfo *ptMtInfo
           u8 byMtNum
�� �� ֵ�� BOOL32  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/24  4.0			������                  ����
=============================================================================*/
BOOL32  CNPlusInst::SetConfMtInfo( CConfId &cConfId, TMtInfo *ptMtInfo, u8 byMtNum )
{
    if (NULL == ptMtInfo)
    {
        return FALSE;
    }

    u8 byIdx = GetConfIdxByConfId(cConfId);
    if (MAXNUM_ONGO_CONF == byIdx)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::SetConfMtInfo] get conf index failed by cconfid:");
        cConfId.Print();
        return FALSE;
    }

    memcpy(m_atConfData[byIdx].m_atMtInConf, ptMtInfo, byMtNum*sizeof(TMtInfo));
    m_atConfData[byIdx].m_byMtNum = byMtNum;
    return TRUE;
}

/*=============================================================================
�� �� ���� SetConfSmcuCallInfo
��    �ܣ� �����¼�MCU���������Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  CConfId *pcConfId
           TSmcuCallnfo *ptSmcuCallInfo
           u8 bySmcuNum
�� �� ֵ�� BOOL32  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2012/11/16  4.7			chendaiwei            create
=============================================================================*/
BOOL32  CNPlusInst::SetConfSmcuCallInfo( CConfId &cConfId, TSmcuCallnfo *ptSmcuCallInfo, u8 bySmcuNum )
{
    if (NULL == ptSmcuCallInfo)
    {
        return FALSE;
    }

    u8 byIdx = GetConfIdxByConfId(cConfId);
    if (MAXNUM_ONGO_CONF == byIdx)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::SetConfSmcuCallInfo] get conf index failed by cconfid:");
        cConfId.Print();
        return FALSE;
    }

	memset((u8*)&m_atConfExData[byIdx].m_atSmcuCallInfo[0],0,sizeof(m_atConfExData[byIdx].m_atSmcuCallInfo));
    memcpy(m_atConfExData[byIdx].m_atSmcuCallInfo, ptSmcuCallInfo, bySmcuNum*sizeof(TSmcuCallnfo));
    return TRUE;
}

BOOL32 CNPlusInst::UnPackVmpBufToVmpParam( u8 *ptNplusVmpInfoBuf, TNPlusVmpParam *ptVmpParam, u8 &byVmpNum, u16 &wPackbufLen)
{
	if( ptNplusVmpInfoBuf == NULL || ptVmpParam == NULL )
	{
		return FALSE;
	}

	byVmpNum = *ptNplusVmpInfoBuf;
	wPackbufLen = 0;

	ptNplusVmpInfoBuf++;
	wPackbufLen++;
	
	for( u8 byVmpNumIdx = 0; byVmpNumIdx < byVmpNum; byVmpNumIdx++ )
	{
		memcpy(&ptVmpParam[byVmpNumIdx].m_tVmpBaiscParam,(TVmpBasicParam*)ptNplusVmpInfoBuf,sizeof(TVmpBasicParam));
		ptNplusVmpInfoBuf += sizeof(TVmpBasicParam);
		wPackbufLen += sizeof(TVmpBasicParam);
				
		u8 byChnlNum = *ptNplusVmpInfoBuf;
		ptNplusVmpInfoBuf++;
		wPackbufLen++;
		
		memcpy(&ptVmpParam[byVmpNumIdx].m_tVmpMemer.m_tVmpChnnlInfo[0],ptNplusVmpInfoBuf,sizeof(TNPlusVmpChnlMember)*byChnlNum);
		ptNplusVmpInfoBuf += sizeof(TNPlusVmpChnlMember)*byChnlNum;
		wPackbufLen += sizeof(TNPlusVmpChnlMember)*byChnlNum;
	}
	
	return TRUE;
}

void CNPlusInst::ClearVmpParamByConfIdx ( u8 byConIdx )
{
	for( u8 byEqpidx = 0; byEqpidx < MAXNUM_PERIEQP; byEqpidx++ )
	{
		if(m_atNplusParam[byEqpidx].m_byConfIdx == byConIdx)
		{
			m_atNplusParam[byEqpidx].Clear();
		}
	}
}

/*=============================================================================
�� �� ���� SetConfMultiVmpParam
��    �ܣ� ��������vmp������Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  CConfId *pcConfId
           u8 *ptNplusVmpInfoBuf��vmp�����ڴ��׵�ַ
		   u16 &wPackBufLen buffer����
�� �� ֵ�� BOOL32  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2013/03/16  4.7			chendaiwei            create
=============================================================================*/
BOOL32  CNPlusInst::SetConfMultiVmpParam( CConfId &cConfId, u8 *ptNplusVmpInfoBuf, u16 &wPackbufLen)
{
    if (NULL == ptNplusVmpInfoBuf)
    {
        return FALSE;
    }

    u8 byIdx = GetConfIdxByConfId(cConfId);
    if (MAXNUM_ONGO_CONF == byIdx)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::SetConfVmpParam] get conf index failed by cconfid:");
        cConfId.Print();
        return FALSE;
    }

	TNPlusVmpParam atVmpParam[MAXNUM_PERIEQP];
	wPackbufLen = 0;
	u8 byVmpNum = 0;
	u8 byVmpNumIdx = 0;

	if(!UnPackVmpBufToVmpParam(ptNplusVmpInfoBuf,atVmpParam,byVmpNum,wPackbufLen))
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::UnPackVmpBufToVmpParam] failed!\n");

		return FALSE;
	}

	ClearVmpParamByConfIdx(byIdx);

	for( u8 byEqpIdx = 0; byEqpIdx < MAXNUM_PERIEQP; byEqpIdx++ )
	{
		if(m_atNplusParam[byEqpIdx].IsNull() && byVmpNumIdx < byVmpNum)
		{
			m_atNplusParam[byEqpIdx] = atVmpParam[byVmpNumIdx];
			m_atNplusParam[byEqpIdx].m_byConfIdx = byIdx;
			byVmpNumIdx++;
		}
	}
	
    return TRUE;
}


BOOL32 CNPlusInst::GetConfMultiVmpParam( u8 byConfIdx, TNPlusVmpParam *ptParam, u8 &byVmpNum)
{
    if (NULL == ptParam)
    {
        return FALSE;
    }
	
    if (MAXNUM_ONGO_CONF == byConfIdx)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::GetConfMultiVmpParam] conf index %d invalid\n",byConfIdx);

        return FALSE;
    }

	byVmpNum = 0;

	for( u8 byEqpidx = 0; byEqpidx < MAXNUM_PERIEQP; byEqpidx++ )
	{
		if(m_atNplusParam[byEqpidx].m_byConfIdx == byConfIdx)
		{
			ptParam[byVmpNum] = m_atNplusParam[byEqpidx];
			byVmpNum++;
		}
	}

	return TRUE;
}

/*=============================================================================
�� �� ���� SetConfVmpInfo
��    �ܣ� �������vmp��Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  CConfId *pcConfId
           TMtInfo *ptMtInfo
           u8 byMtNum
�� �� ֵ�� BOOL32  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/24  4.0			������                  ����
=============================================================================*/
BOOL32  CNPlusInst::SetConfVmpInfo( CConfId &cConfId, TNPlusVmpInfo *ptVmpInfo, TVMPParam *ptParam )
{
    if (NULL == ptVmpInfo || NULL == ptParam)
    {
        return FALSE;
    }

    u8 byIdx = GetConfIdxByConfId(cConfId);
    if (MAXNUM_ONGO_CONF == byIdx)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::SetConfVmpInfo] get conf index failed by cconfid:");
        cConfId.Print();
        return FALSE;
    }   

    memcpy(&m_atConfData[byIdx].m_tVmpInfo, ptVmpInfo, sizeof(TNPlusVmpInfo));

    m_atConfData[byIdx].m_tConf.m_tStatus.SetVmpParam(*ptParam);
    return TRUE;
}

/*=============================================================================
�� �� ���� SetChairman
��    �ܣ� ���������ϯ��Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  CConfId *pcConfId
           TMtInfo *ptMtAlias
�� �� ֵ�� BOOL32  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/24  4.0			������                  ����
=============================================================================*/
BOOL32  CNPlusInst::SetChairman( CConfId &cConfId, TMtAlias *ptMtAlias )
{
    if (NULL == ptMtAlias)
    {
        return FALSE;
    }

    u8 byIdx = GetConfIdxByConfId(cConfId);
    if (MAXNUM_ONGO_CONF == byIdx)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::SetChairman] get conf index failed by cconfid:");
        cConfId.Print();
        return FALSE;
    }

    m_atConfData[byIdx].m_tConf.SetChairAlias(*ptMtAlias);
    return TRUE;
}

/*=============================================================================
�� �� ���� SetSpeaker
��    �ܣ� ������鷢������Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  CConfId *pcConfId
           TMtInfo *ptMtAlias
�� �� ֵ�� BOOL32  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/24  4.0			������                  ����
=============================================================================*/
BOOL32  CNPlusInst::SetSpeaker( CConfId &cConfId, TMtAlias *ptMtAlias )
{
    if (NULL == ptMtAlias)
    {
        return FALSE;
    }

    u8 byIdx = GetConfIdxByConfId(cConfId);
    if (MAXNUM_ONGO_CONF == byIdx)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::SetSpeaker] get conf index failed by cconfid:");
        cConfId.Print();
        return FALSE;
    }

    m_atConfData[byIdx].m_tConf.SetSpeakerAlias(*ptMtAlias);
    return TRUE;
}

/*=============================================================================
�� �� ���� SetUsrGrpInfo
��    �ܣ� �����û�����Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CUsrGrpsInfo *pcUsrGrpInfo
�� �� ֵ�� BOOL32  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/24  4.0			������                  ����
=============================================================================*/
BOOL32  CNPlusInst::SetUsrGrpInfo( CUsrGrpsInfo *pcUsrGrpInfo )
{
    if (NULL == pcUsrGrpInfo)
    {
        return FALSE;
    }
    memcpy(&m_cUsrGrpsInfo, pcUsrGrpInfo, sizeof(CUsrGrpsInfo));
    return TRUE;
}

/*=============================================================================
�� �� ���� SetUsrInfo
��    �ܣ� �����û���Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  CExUsrInfo *pcUsrInfo
           u8 byNum
�� �� ֵ�� BOOL32  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/24  4.0			������                  ����
=============================================================================*/
BOOL32  CNPlusInst::SetUsrInfo( CExUsrInfo *pcUsrInfo, u8 byNum, BOOL32 bAdd )
{
    if (NULL == pcUsrInfo)
    {
        return FALSE;
    }
    
    if (bAdd)
    {
        if (byNum + m_byUsrNum > MAXNUM_USRNUM)
        {
            LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[SetUsrInfo] user packet error.\n");
            return FALSE;
        }
        memcpy(&m_acUsrInfo[m_byUsrNum], pcUsrInfo, sizeof(CExUsrInfo)*byNum);
        m_byUsrNum += byNum;
    }
    else
    {
        if (byNum > MAXNUM_USRNUM)
        {
            LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[SetUsrInfo] user packet error.\n");
            return FALSE;
        }
        memcpy(m_acUsrInfo, pcUsrInfo, sizeof(CExUsrInfo)*byNum);
        m_byUsrNum = byNum;
    }    
    return TRUE;
}

/*=============================================================================
�� �� ���� GetConfIdxByConfId
��    �ܣ� ����CConfId��ȡ������Ϣ����(0 - MAXNUM_ONGO_CONF-1)
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CConfId *pcConfId
�� �� ֵ�� u8  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/24  4.0			������                  ����
=============================================================================*/
u8 CNPlusInst::GetConfIdxByConfId( const CConfId &cConfId )
{    
    if (cConfId.IsNull())
    {
        return MAXNUM_ONGO_CONF;
    }

    for (u8 byIdx = 0; byIdx < MAXNUM_ONGO_CONF; byIdx++)
    {
        if (m_atConfData[byIdx].m_tConf.GetConfId() == cConfId)
        {
            return byIdx;
        }
    }
    
    return MAXNUM_ONGO_CONF;
}

/*=============================================================================
�� �� ���� GetEmptyConfIdx
��    �ܣ� ��ȡ������Ϣ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� u8  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/12/5  4.0			������                  ����
=============================================================================*/
u8  CNPlusInst::GetEmptyConfIdx( void )
{
    for (u8 byIdx = 0; byIdx < MAXNUM_ONGO_CONF; byIdx++)
    {
        if (m_atConfData[byIdx].m_tConf.GetConfId().IsNull())
        {
            return byIdx;
        }
    }

    return MAXNUM_ONGO_CONF;
}

/*=============================================================================
    �� �� ���� UnRegAllInfoOfSwitchedMcu
    ��    �ܣ� ����ñ���MCU�����˺���MCU��ͬ��GK����ñ���MCU��������彫αװ
               ����MCU��������� �� ��MCU������ע�ᵽ GK�ϵ���Ϣ ����ע����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/12/18  4.0			�ű���                  ����
=============================================================================*/
void CNPlusInst::UnRegAllInfoOfSwitchedMcu()
{
    TRASInfo *ptRASInfo = GetRASInfo();
	if(ptRASInfo != NULL)
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "[UnRegAllInfoOfSwitchedMcu] Get RasInfo. Ras EPID Length[%u], EPId Alias[%s], MCUE164[%s]!\n",ptRASInfo->GetEPID()->GetIDlength(),ptRASInfo->GetEPID()->GetIDAlias(),ptRASInfo->GetMcuE164());               
	}

    if (ptRASInfo == NULL || 0 == ptRASInfo->GetEPID()->GetIDlength() ) 
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[UnRegAllInfoOfSwitchedMcu] the EPID of MASTER mcu hasn't got yet !\n" );
        return;
    }
    CServMsg cServMsg;
    cServMsg.SetConfIdx( 0 );
    cServMsg.SetEventId( MCU_MT_UNREGGK_NPLUS_CMD );
    cServMsg.SetMsgBody( (u8*)ptRASInfo, sizeof(TRASInfo) );

    g_cMtAdpSsnApp.SendMsgToMtAdpSsn( g_cMcuVcApp.GetRegGKDriId(), 
                                      MCU_MT_UNREGGK_NPLUS_CMD, cServMsg );

    return;
}

/*=============================================================================
    �� �� ���� StopConfChargeOfSwitchedMcu
    ��    �ܣ� ����ñ���MCU�����˺���MCU��ͬ��GK����ñ���MCU��������彫αװ
               ����MCU��������� �� ��MCU��������GK�ϼƷѵĻ������ȡ���ƷѲ������Է����µļƷ�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� 
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2006/12/25  4.0			�ű���                  ����
=============================================================================*/
void CNPlusInst::StopConfChargeOfSwitchedMcu()
{
    for( u8 byConfIdx = 0; byConfIdx < MAXNUM_ONGO_CONF; byConfIdx  ++ )
    {
        if ( !m_atConfData[byConfIdx].IsNull() &&
             m_atConfData[byConfIdx].m_tConf.IsSupportGkCharge() )
        {
            TConfChargeInfo tChargeInfo;
            tChargeInfo.SetGKUsrName(m_atConfData[byConfIdx].m_tConf.GetConfAttrbEx().GetGKUsrName());
            tChargeInfo.SetGKPwd(m_atConfData[byConfIdx].m_tConf.GetConfAttrbEx().GetGKPwd());

            CServMsg cServMsg;
            cServMsg.SetEventId( MCU_MT_CONF_STOPCHARGE_CMD );
            cServMsg.SetMsgBody( (u8*)&tChargeInfo, sizeof(TConfChargeInfo) );
            cServMsg.CatMsgBody( (u8*)&m_atConfData[byConfIdx].m_tSsnId, sizeof(TAcctSessionId) );
            
            g_cMtAdpSsnApp.SendMsgToMtAdpSsn( g_cMcuVcApp.GetRegGKDriId(), 
                                              MCU_MT_CONF_STOPCHARGE_CMD, cServMsg );
        }
    }
}

/*=============================================================================
    �� �� ���� IsLocalEqpCapLowerThanRemote
    ��    �ܣ� �Ƚϱ������������Ƿ�С��Զ������������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]TNPlusEqpCap &tLocalCap��������������
               [IN] TNPlusEqpCapEx &tlocalCapEx����������չ������
               [IN]TNPlusEqpCapEx &tRemoteCapԶ������������
			   [IN]TNPlusEqpCapEx &tRemoteCapExԶ��������չ������
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2011/12/29  4.0			chendaiwei                  ����
=============================================================================*/
BOOL32 CNPlusInst::IsLocalEqpCapLowerThanRemote(TNPlusEqpCap &tLocalCap, TNPlusEqpCapEx &tlocalCapEx, TNPlusEqpCap &tRemoteCap, TNPlusEqpCapEx &tRemoteCapEx)
{	
	//1.���������Ƚ�
	if( tLocalCap < tRemoteCap)
	{
		return TRUE;
	}

	//2.��չ�����Ƚ�
	if(tlocalCapEx < tRemoteCapEx)
	{
		return TRUE;
	}

	//3.vmp�����Ƚ�
	if( IsVmpLowerThanOther(tLocalCap,tRemoteCap,tlocalCapEx,tRemoteCapEx))
	{
		return TRUE;
	}
	
	//4. BAS�����Ƚ�
	TBasChnAbility tlocalBasChn[NPLUS_MAX_BASCHN_NUM];
	TBasChnAbility tRemoteBasChn[NPLUS_MAX_BASCHN_NUM];
	TBasChnAbility *ptLocalBasChn = tlocalBasChn;
	TBasChnAbility *ptRemoteBasChn = tRemoteBasChn;
	u8 byLocalBasChnNum = 0;
	u8 byRemoteBasChnNum = 0;

	GetVideoBasChnAbilityArray(tLocalCap,tlocalCapEx,ptLocalBasChn,byLocalBasChnNum);
	GetVideoBasChnAbilityArray(tRemoteCap,tRemoteCapEx,ptRemoteBasChn,byRemoteBasChnNum);
	
	//��ƵBASͨ���������Ƚ�
	if(IsBasChnLowerThanOther(ptLocalBasChn,ptRemoteBasChn,byLocalBasChnNum,byRemoteBasChnNum))
	{
		return TRUE;
	}

	memset(tlocalBasChn,0,sizeof(tlocalBasChn));
	memset(tRemoteBasChn,0,sizeof(tRemoteBasChn));
	byLocalBasChnNum = 0;
	byRemoteBasChnNum = 0;
	GetAudioBasChnAbilityArray(tLocalCap,tlocalCapEx,ptLocalBasChn,byLocalBasChnNum);
	GetAudioBasChnAbilityArray(tRemoteCap,tRemoteCapEx,ptRemoteBasChn,byRemoteBasChnNum);

	//��ƵBASͨ�������Ƚ�
	if(IsBasChnLowerThanOther(ptLocalBasChn,ptRemoteBasChn,byLocalBasChnNum,byRemoteBasChnNum,FALSE))
	{
		return TRUE;
	}

	return FALSE;
}

BOOL32 CNPlusInst::IsVmpLowerThanOther( TNPlusEqpCap tLocalCap, TNPlusEqpCap tOtherCap, TNPlusEqpCapEx tLocalCapEx, TNPlusEqpCapEx tOtherCapEx)
{

	if(tLocalCapEx.m_byMPU2EnhancedVmpNum < tOtherCapEx.m_byMPU2EnhancedVmpNum ||
		(tLocalCapEx.m_byMPU2BasicVmpNum + tLocalCapEx.m_byMPU2EcardBasicVmpNum + tLocalCapEx.m_byMPU2EnhancedVmpNum) < (tOtherCapEx.m_byMPU2BasicVmpNum + tOtherCapEx.m_byMPU2EcardBasicVmpNum + tOtherCapEx.m_byMPU2EnhancedVmpNum) ||
			(tLocalCapEx.m_byMPU2BasicVmpNum + tLocalCapEx.m_byMPU2EnhancedVmpNum) < (tOtherCapEx.m_byMPU2BasicVmpNum + tOtherCapEx.m_byMPU2EnhancedVmpNum) 	)
	{
		return TRUE;
	}
	
	//���㱾�ض���mpu2 vmp
	u8 byMPU2VmpNum = tLocalCapEx.m_byMPU2BasicVmpNum + tLocalCapEx.m_byMPU2EcardBasicVmpNum + tLocalCapEx.m_byMPU2EnhancedVmpNum -(tOtherCapEx.m_byMPU2BasicVmpNum + tOtherCapEx.m_byMPU2EcardBasicVmpNum + tOtherCapEx.m_byMPU2EnhancedVmpNum);
	for( u8 byIdx = 0; byIdx < byMPU2VmpNum; byIdx++ )
	{
		tLocalCap.m_byVmpNum ++;
		tLocalCap.m_abyVMPChnNum[tLocalCap.m_byVmpNum-1] = MAXNUM_MPU2VMP_MEMBER;
	}

	if(tLocalCap.m_byVmpNum < tOtherCap.m_byVmpNum
		|| tLocalCap.IsLocalVMPCapInferior(tOtherCap))
	{
		return TRUE;
	}

	return FALSE;
}

/*=============================================================================
    �� �� ���� IsBasChnLowerThanOther
    ��    �ܣ� �Ƚ�����Basͨ����������С
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]TBasChnAbility *ptBasChn Basͨ������������ָ��
			   [IN]TBasChnAbility *ptOtherBasChn ��һ��Basͨ������������ָ��
               [IN] u16 wChNum Basͨ����
               [IN]u16 wOtherChnNum ��һ��BAsͨ����
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2011/12/29  4.0			chendaiwei                  ����
=============================================================================*/
BOOL32 CNPlusInst::IsBasChnLowerThanOther(TBasChnAbility *ptBasChn,TBasChnAbility *ptOtherBasChn,u8 byChNum, u8 byOtherChnNum, BOOL32 bVideoBas)
{
	if(ptBasChn == NULL || ptOtherBasChn == NULL)
	{
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[IsBasChnLowerThanOther] ptLocalBasChn == NULL || ptRemoteBasChn == NULL.\n");
		
		return TRUE;
	}

	if( bVideoBas )
	{
		//1. BAsͨ�������Ƚ�
		if(byChNum < byOtherChnNum)
		{
			LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS, "[IsBasChnLowerThanOther]BAs Chnnl Num <local:%d,remote:%d>",byChNum,byOtherChnNum);

			return TRUE;
		}
		
		//2. BAS���������Ƚ�
		u8 byCanBeUsedChnNum = 0;

		u8 byOtherDecHPChnNum = 0;
		u8 byOtherDecHD60ChnNum = 0;
		u8 byOtherDecHD30ChnNum  = 0;
		u8 byOtherDecSDChnNum = 0;
		for(u16 byIndex = 0; byIndex <byOtherChnNum; byIndex++)
		{
			switch (ptOtherBasChn[byIndex].m_byDecAbility)
			{
			case emDecHP:
				byOtherDecHPChnNum++;
				break;
			case emDecHD60:
				byOtherDecHD60ChnNum++;
				break;
			case emDecHD30:
				byOtherDecHD30ChnNum++;
				break;
			case emDecSD:
				byOtherDecSDChnNum ++;
				break;
			default:
				break;
			}
		}

		u8 byDecHPChnNum = 0;
		u8 byDecHD60ChnNum = 0;
		u8 byDecHD30ChnNum  = 0;
		u8 byDecSDChnNum = 0;
		for(u16 wIdx = 0; wIdx <byChNum; wIdx++)
		{
			switch (ptBasChn[wIdx].m_byDecAbility)
			{
			case emDecHP:
				byDecHPChnNum++;
				break;
			case emDecHD60:
				byDecHD60ChnNum++;
				break;
			case emDecHD30:
				byDecHD30ChnNum++;
				break;
			case emDecSD:
				byDecSDChnNum ++;
				break;
			default:
				break;
			}
		}

		//��HP
		if( byDecHPChnNum < byOtherDecHPChnNum )
		{
			LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS, "[IsBasChnLowerThanOther]Dec HP Ability<local:%d,remote:%d>",byDecHPChnNum,byOtherDecHPChnNum);
			return TRUE;
		}
		else
		{
			byCanBeUsedChnNum = byDecHPChnNum - byOtherDecHPChnNum;
		}
		
		//�����60֡
		if(byDecHD60ChnNum < byOtherDecHD60ChnNum)
		{
			if( byDecHD60ChnNum + byCanBeUsedChnNum < byOtherDecHD60ChnNum )
			{
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS, "[IsBasChnLowerThanOther]Dec HD 60Fps Ability<local:%d,remote:%d>",byDecHD60ChnNum,byOtherDecHD60ChnNum);
				
				return TRUE;
			}
			else
			{
				byCanBeUsedChnNum = byCanBeUsedChnNum - (byOtherDecHD60ChnNum - byDecHD60ChnNum);
			}
		}
		else
		{
			byCanBeUsedChnNum = byCanBeUsedChnNum + (byDecHD60ChnNum - byOtherDecHD60ChnNum);
		}
		
		//�����30֡
		if(byDecHD30ChnNum < byOtherDecHD30ChnNum)
		{
			if( byDecHD30ChnNum + byCanBeUsedChnNum < byOtherDecHD30ChnNum )
			{
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS, "[IsBasChnLowerThanOther]Dec HD 30Fps Ability<local:%d,remote:%d>",byDecHD30ChnNum,byOtherDecHD30ChnNum);
				
				return TRUE;
			}
			else
			{
				byCanBeUsedChnNum = byCanBeUsedChnNum - (byOtherDecHD30ChnNum - byDecHD30ChnNum);
			}
		}
		else
		{
			byCanBeUsedChnNum = byCanBeUsedChnNum + (byDecHD30ChnNum - byOtherDecHD30ChnNum);
		}
		
		//�����
		if(byDecSDChnNum < byOtherDecSDChnNum)
		{

			if( byDecSDChnNum + byCanBeUsedChnNum < byOtherDecSDChnNum )
			{
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS, "[IsBasChnLowerThanOther]Dec SD Ability<local:%d,remote:%d>",byDecSDChnNum,byOtherDecSDChnNum);
				
				return TRUE;
			}
		}	

		//��������
		for( u8 bySortIdx = 0; bySortIdx < byChNum - 1; bySortIdx++ )
		{
			for(u8 byInnerIdx = 0; byInnerIdx <byChNum - bySortIdx -1; byInnerIdx ++)
			{
				if(ptBasChn[byInnerIdx].m_emChnType > ptBasChn[byInnerIdx+1].m_emChnType)
				{
					TBasChnAbility tTemp = ptBasChn[byInnerIdx];
					ptBasChn[byInnerIdx] = ptBasChn[byInnerIdx+1];
					ptBasChn[byInnerIdx+1] = tTemp;
				}
			}
		}

		for( u8 byIdex = 0; byIdex < byOtherChnNum - 1; byIdex++ )
		{
			for(u8 byInnerIdx = 0; byInnerIdx <byOtherChnNum - byIdex -1; byInnerIdx ++)
			{
				if(ptOtherBasChn[byInnerIdx].m_emChnType > ptOtherBasChn[byInnerIdx+1].m_emChnType)
				{
					TBasChnAbility tTemp = ptOtherBasChn[byInnerIdx];
					ptOtherBasChn[byInnerIdx] = ptOtherBasChn[byInnerIdx+1];
					ptOtherBasChn[byInnerIdx+1] = tTemp;
				}
			}
		}

		//3.���������Ƚ�
		u16 byOtherIdx = 0;
		u16 byIdx = 0;

		
		while( byOtherIdx < byOtherChnNum && byIdx < byChNum)
		{
			//localĳͨ�����������㹻
			if(ptOtherBasChn[byOtherIdx].m_byDecAbility <= ptBasChn[byIdx].m_byDecAbility && !ptBasChn[byIdx].m_byIsOccupy)
			{
				//Remoteͨ����60֡�ı���������localͨ����mpu_h,��������local ��mpu_hͨ�������ܳ�60֡����
				if(ptOtherBasChn[byOtherIdx].Has60FpsEncAbility() && ptBasChn[byIdx].m_emChnType == emMPU_HChn)
				{
					ptBasChn[byIdx] = g_cNPlusApp.GetBasChnAbilityByBasChnType(emMPU_H60Chn);
				}
				
				u32 dwOtherBasEncAbility[6] = {0};
				u8 byOtherEncChnIdx = 0;
				u8 byEncChnIdx = 0;

				memcpy((u8*)dwOtherBasEncAbility,(u8*)&ptOtherBasChn[byOtherIdx].m_dwAbility[0],ptOtherBasChn[byOtherIdx].m_byEncNum*sizeof(ptOtherBasChn[byOtherIdx].m_dwAbility[0]));
				
				//�����Ƚ�Remoteĳͨ����N�������Ҵ��ڵ���N��������һ������localͨ��
				while(byOtherEncChnIdx < ptOtherBasChn[byOtherIdx].m_byEncNum && byEncChnIdx < ptBasChn[byIdx].m_byEncNum)
				{
					dwOtherBasEncAbility[byOtherEncChnIdx] = ~(ptBasChn[byIdx].m_dwAbility[byEncChnIdx])&dwOtherBasEncAbility[byOtherEncChnIdx];
					
					//localĳ���ܱ�Remoteĳ��
					if(dwOtherBasEncAbility[byOtherEncChnIdx] == 0)
					{
						byEncChnIdx++;
						byOtherEncChnIdx++;
					}
					//localĳ�����ܱ�Remoteĳ��������local��һ��
					else
					{
						byEncChnIdx++;
					}

					//Remoteͨ��ptRemoteBasChn[byRemoteIdx]�ܹ�����������ѭ��
					if(byOtherEncChnIdx == ptOtherBasChn[byOtherIdx].m_byEncNum)
					{
						byOtherIdx++;
						ptBasChn[byIdx].m_byIsOccupy = 1; //ռ��
						byIdx ++;

						break;
					}
					//��localͨ���ѱ�ռ�ã���Ȼ����Remote ĳBasͨ��������������һ��local bas ͨ��
					else if(byEncChnIdx == ptBasChn[byIdx].m_byEncNum)
					{
						ptBasChn[byIdx].m_byIsOccupy = 1; //ռ��
						byIdx ++;
						byEncChnIdx = 0;
					}
				}
			}
			else
			{
				byIdx++; //local�����������㣬����һ��
			}
			
			//localͨ����ռ���꣬Remote����local�޷����ݵ�ͨ����local Bas Chn Ability < Remote Bas Chn Ability
			if(byIdx == byChNum && byOtherIdx != byOtherChnNum)
			{
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS, "[IsBasChnLowerThanOther]BAs Chnnl Dec ability is not enough!\n");
				return TRUE;
			}
		}

		return FALSE;
	}
	else
	{
		//Ŀǰ��ƵBASֻ��APU2������ƵBAS�����Ƚϲ���[3/30/2013 chendaiwei]
		if(byChNum < byOtherChnNum)
		{
			LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS, "[IsBasChnLowerThanOther]Audio BAs Chnnl Num <local:%d,remote:%d>",byChNum,byOtherChnNum);

			return TRUE;
		}

		//��������
		for( u8 bySortIdx = 0; bySortIdx < byChNum - 1; bySortIdx++ )
		{
			for(u8 byInnerIdx = 0; byInnerIdx <byChNum - bySortIdx -1; byInnerIdx ++)
			{
				if(ptBasChn[byInnerIdx].m_emChnType > ptBasChn[byInnerIdx+1].m_emChnType)
				{
					TBasChnAbility tTemp = ptBasChn[byInnerIdx];
					ptBasChn[byInnerIdx] = ptBasChn[byInnerIdx+1];
					ptBasChn[byInnerIdx+1] = tTemp;
				}
			}
		}

		for( u8 byIdex = 0; byIdex < byOtherChnNum - 1; byIdex++ )
		{
			for(u8 byInnerIdx = 0; byInnerIdx <byOtherChnNum - byIdex -1; byInnerIdx ++)
			{
				if(ptOtherBasChn[byInnerIdx].m_emChnType > ptOtherBasChn[byInnerIdx+1].m_emChnType)
				{
					TBasChnAbility tTemp = ptOtherBasChn[byInnerIdx];
					ptOtherBasChn[byInnerIdx] = ptOtherBasChn[byInnerIdx+1];
					ptOtherBasChn[byInnerIdx+1] = tTemp;
				}
			}
		}

		u8 byAudioBasIdx = 0;
		u8 byOthterAudioBasIdx = 0;
		while (byOthterAudioBasIdx < byOtherChnNum )
		{
			if(ptBasChn[byAudioBasIdx].m_byEncNum < ptOtherBasChn[byOthterAudioBasIdx].m_byEncNum)
			{
				byAudioBasIdx++;
				byOthterAudioBasIdx++;

				if( byAudioBasIdx == byChNum)
				{
					LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS, "[IsBasChnLowerThanOther]Audio BAs Chnnl capbility local<other\n");
					return TRUE;
				}
			}
			else
			{
				byOthterAudioBasIdx++;
			}
		}

		return FALSE;
	}
}

/*=============================================================================
    �� �� ���� GetAllBasChnAbilityArray
    ��    �ܣ� ��ȡ���غ�Զ��basͨ������������������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]TNPlusEqpCap tCap ��������������
			   [IN]TNPlusEqpCapEx tCapEx ������չ������
			   [OUT]TBasChnAbility *ptBasChn ͨ���������׵�ַ
			   [OUT]u16 &wBasChnNum BAsͨ����
    �� �� ֵ�� void
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2011/12/29  4.0			chendaiwei                  ����
=============================================================================*/
void CNPlusInst::GetAllBasChnAbilityArray(TNPlusEqpCap tCap, TNPlusEqpCapEx tCapEx,TBasChnAbility *ptBasChn, u8 &byBasChnNum)
{
	byBasChnNum = 0;
	u8 byAudioBasNum = 0;
	u8 byVideoBasNum = 0;

	if(ptBasChn == NULL)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[GetAllBasChnAbilityArray] || ptLocalBasChn == NULL, error!\n");
		return;
	}

	GetVideoBasChnAbilityArray(tCap,tCapEx,ptBasChn,byVideoBasNum);
	GetAudioBasChnAbilityArray(tCap,tCapEx,(ptBasChn+byVideoBasNum),byAudioBasNum);

	byBasChnNum = byVideoBasNum + byAudioBasNum;
	
	return;
}


void CNPlusInst::GetVideoBasChnAbilityArray(TNPlusEqpCap tCap, TNPlusEqpCapEx tCapEx,TBasChnAbility *ptBasChn, u8 &byBasChnNum)
{
	byBasChnNum = 0;
	
	if(ptBasChn == NULL)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[GetAllBasChnAbilityArray] || ptLocalBasChn == NULL, error!\n");
		return;
	}
	
	//��ȡlocal BAS Chnnel
	u8 byChnNUm = 0;
	
	TBasChnAbility tVpuBasChn = g_cNPlusApp.GetBasChnAbilityByBasChnType(emVpuChn);
	for(u8 byIdx = 0; byIdx < tCap.m_byBasVidChnlNum; byIdx++)
	{
		*ptBasChn = tVpuBasChn;
		ptBasChn++;
		byBasChnNum++;
	}
	
	GetBasChnAbilityArrayByEqpType(TYPE_MAU_H263PLUS,tCap.m_byMAUH263pNum,ptBasChn,byChnNUm);
	ptBasChn = ptBasChn + byChnNUm;
	byBasChnNum = byBasChnNum + byChnNUm;
	
	GetBasChnAbilityArrayByEqpType(TYPE_MAU_NORMAL,tCap.m_byMAUNum,ptBasChn,byChnNUm);
	ptBasChn = ptBasChn + byChnNUm;
	byBasChnNum = byBasChnNum + byChnNUm;
	
	GetBasChnAbilityArrayByEqpType(TYPE_MPU,tCap.m_byMpuBasNum,ptBasChn,byChnNUm);
	ptBasChn = ptBasChn + byChnNUm;
	byBasChnNum = byBasChnNum + byChnNUm;
	
	GetBasChnAbilityArrayByEqpType(TYPE_8KE_BAS,tCapEx.m_by8000GBasNum,ptBasChn,byChnNUm);
	ptBasChn = ptBasChn + byChnNUm;
	byBasChnNum = byBasChnNum + byChnNUm;
	
	GetBasChnAbilityArrayByEqpType(TYPE_8KH_BAS,tCapEx.m_by8000HBasNum,ptBasChn,byChnNUm);
	ptBasChn = ptBasChn + byChnNUm;
	byBasChnNum = byBasChnNum + byChnNUm;
	
	GetBasChnAbilityArrayByEqpType(TYPE_MPU_H,tCap.m_byBap2BasNum,ptBasChn,byChnNUm);
	ptBasChn = ptBasChn + byChnNUm;
	byBasChnNum = byBasChnNum + byChnNUm;
	
	GetBasChnAbilityArrayByEqpType(TYPE_MPU2_ENHANCED,tCapEx.m_byMPU2EnhancedBasNum,ptBasChn,byChnNUm);
	ptBasChn = ptBasChn + byChnNUm;
	byBasChnNum = byBasChnNum + byChnNUm;
	
	GetBasChnAbilityArrayByEqpType(TYPE_MPU2_BASIC,tCapEx.m_byMPU2BasicBasNum,ptBasChn,byChnNUm);
	ptBasChn = ptBasChn + byChnNUm;
	byBasChnNum = byBasChnNum + byChnNUm;

	GetBasChnAbilityArrayByEqpType(TYPE_8KI_VID_BAS,tCapEx.m_by8000IVidBasNum,ptBasChn,byChnNUm);
	ptBasChn = ptBasChn + byChnNUm;
	byBasChnNum = byBasChnNum + byChnNUm;
	
	return;
}

void CNPlusInst::GetAudioBasChnAbilityArray(TNPlusEqpCap tCap, TNPlusEqpCapEx tCapEx,TBasChnAbility *ptBasChn, u8 &byBasChnNum)
{
	byBasChnNum = 0;
	
	if(ptBasChn == NULL)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[GetAllBasChnAbilityArray] || ptLocalBasChn == NULL, error!\n");
		return;
	}
	
	//��ȡlocal BAS Chnnel
	u8 byChnNUm = 0;
	
	GetBasChnAbilityArrayByEqpType(TYPE_APU2_BAS,tCapEx.m_byAPU2BasNum,ptBasChn,byChnNUm);
	ptBasChn = ptBasChn + byChnNUm;
	byBasChnNum = byBasChnNum + byChnNUm;

	GetBasChnAbilityArrayByEqpType(TYPE_8KI_AUD_BAS,tCapEx.m_by8000IAudBasNum,ptBasChn,byChnNUm);
	ptBasChn = ptBasChn + byChnNUm;
	byBasChnNum = byBasChnNum + byChnNUm;

	return;
}


/*=============================================================================
    �� �� ���� GetBasChnAbilityArrayByEqpType
    ��    �ܣ� ͨ��BAS�������ͺ͸�����ȡBas������������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]u8 byEqpType Bas��������
               [IN]u8 byEqpNum  Bas�������
			   [IN][OUT]TBasChnAbility *ptBasChnnlArry Bas�������������׵�ַ
			   [IN][OUT]u8 &byArraySize Bas�������������С
    �� �� ֵ�� void
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2011/12/29  4.0			chendaiwei                  ����
=============================================================================*/
void CNPlusInst::GetBasChnAbilityArrayByEqpType( u8 byEqpType, u8 byEqpNum, TBasChnAbility *ptBasChnnlArry, u8 &byArraySize)
{	
	if( ptBasChnnlArry == NULL)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[CNPlusInst::GetBasChnAbilityArrayByEqpType] ptBasChnnlArry == NULL!\n");

		return;
	}
	
	byArraySize = 0;

	switch(byEqpType)
	{
	case TYPE_MAU_NORMAL:
		{
			TBasChnAbility tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(emMAUNormal);
			byArraySize = byEqpNum;
			for(u8 byCount = 0; byCount < byEqpNum; byCount++)
			{
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
			}
		}
		break;

	case TYPE_MAU_H263PLUS:
		{
			TBasChnAbility tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(emMAUH263PlusChn);
			byArraySize = byEqpNum;
			for(u8 byCount = 0; byCount < byEqpNum; byCount++)
			{
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
			}
		}
		break;

	case TYPE_MPU:
		{	
			TBasChnAbility tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(emMPUChn);
			byArraySize = byEqpNum*4; //4ͨ��
			for(u8 byCount = 0; byCount < byArraySize; byCount++)
			{
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
			}
		}
		break;

	case TYPE_8KE_BAS:
		{
			for(u8 byCount = 0; byCount < byEqpNum/3; byCount ++)
			{
				TBasChnAbility tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(em8KGBrdChn);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
				
				tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(em8KGSelChn);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
				
				tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(em8KGDSChn);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
			}
			byArraySize = byEqpNum/3*3;
		}
		break;

	case TYPE_8KH_BAS:
		{
			for(u8 byCount = 0; byCount < byEqpNum/3; byCount ++)
			{
				TBasChnAbility tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(em8KHBrdChn);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
				
				tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(em8KHSelChn);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
				
				tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(em8KHDSChn);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
			}
			byArraySize = byEqpNum/3*3;
		}
		break;
	case TYPE_MPU_H:
		{
			TBasChnAbility tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(emMPU_HChn);
			byArraySize = byEqpNum*2; //4ͨ��
			for(u8 byCount = 0; byCount < byArraySize; byCount++)
			{
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
			}
		}
		break;
	case TYPE_MPU2_BASIC:
		{
			for(u8 byCount = 0; byCount < byEqpNum; byCount ++)
			{
				TBasChnAbility tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(emMPU2BasicBrdChn);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;

				tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(emMPU2BasicDsChn);		
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
				
				tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(emMPU2BasicSelChn);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
			}
			byArraySize = byEqpNum*4;
		}

		break;
	case TYPE_MPU2_ENHANCED:
		{
			for(u8 byCount = 0; byCount < byEqpNum; byCount ++)
			{
				TBasChnAbility tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(emMPU2EnhancedBrdChn);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;

				tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(emMPU2EnhancedDsChn);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
				
				tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(emMPU2EnhancedSelChn);
				for( u8 byInnerCount = 0; byInnerCount < 5 ; byInnerCount++ )
				{
					memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
					ptBasChnnlArry ++;
				}
			}
			byArraySize = byEqpNum*7;
		}

		break;

	case TYPE_APU2_BAS:
		{
			TBasChnAbility tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(emAPU2BasChn);
			byArraySize = byEqpNum*MAXNUM_APU2_BASCHN;
			for(u8 byCount = 0; byCount < byArraySize; byCount++)
			{
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
			}
		}

		break;

	case TYPE_8KI_VID_BAS:
		{
			for(u8 byCount = 0; byCount < byEqpNum/3; byCount ++)
			{
				TBasChnAbility tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(em8KIVidBasChn0);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
				
				tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(em8KIVidBasChn1);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
				
				tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(em8KIVidBasChn2);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
			}

			byArraySize = byEqpNum/3*3;
		}

		break;

	case TYPE_8KI_AUD_BAS:
		{
			TBasChnAbility tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(em8KIAudBasChn);
			byArraySize = byEqpNum*MAXNUM_8KI_AUD_BASCHN;
			for(u8 byCount = 0; byCount < byArraySize; byCount++)
			{
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
			}
		}
		
		break;
		
	default:
		LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[CNPlusInst::GetBasChnAbilityArrayByEqpType] unexpected eqp type!\n");
		break;
	}

	return;
}


//CNPlusData
//construct
CNPlusData::CNPlusData( void ) : m_emLocalNPlusState(MCU_NPLUS_IDLE),
                                 m_byNPlusSwitchInsId(0),
                                 m_dwNPlusSwitchMcuIp(0),
                                 m_wRtdTime(0),
                                 m_wRtdNum(0),
                                 m_byNPlusSynOk(FALSE),
                                 m_byLocalMcuType(0)
{
}

//destruct
CNPlusData::~CNPlusData( void )
{
}

/*=============================================================================
�� �� ���� GetNPlusSwitchInsId
��    �ܣ� ��ȡ�����л���ʵ��id
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� u8 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/24  4.0			������                  ����
=============================================================================*/
u8 CNPlusData::GetMcuSwitchedInsId( void )
{
    return m_byNPlusSwitchInsId;
}

/*=============================================================================
�� �� ���� SetNPlusSwitchInsId
��    �ܣ� ��¼�����л���ʵ��id
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byInsId
�� �� ֵ�� u8 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/24  4.0			������                  ����
=============================================================================*/
void CNPlusData::SetMcuSwitchedInsId( u8 byInsId, u32 dwIpAddr )
{
    m_byNPlusSwitchInsId = byInsId;
    m_dwNPlusSwitchMcuIp = dwIpAddr;
}

/*=============================================================================
�� �� ���� GetMcuSwitchedIp
��    �ܣ� ��ȡ�����л���mcu ip
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� u32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/28  4.0			������                  ����
=============================================================================*/
u32 CNPlusData::GetMcuSwitchedIp( void )
{
    return m_dwNPlusSwitchMcuIp;
}

/*=============================================================================
�� �� ���� GetLocalNPlusState
��    �ܣ� ��ȡ����mcu N+1ģʽ��״̬��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� ENPlusState 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/21  4.0			������                  ����
=============================================================================*/
ENPlusState CNPlusData::GetLocalNPlusState( void )
{
    return m_emLocalNPlusState;    
}

/*=============================================================================
�� �� ���� SetLocalNPlusState
��    �ܣ� ���ñ��� mcu N+1״̬��(Ŀǰ������ģʽ�л�ʱ״̬���ı�)
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� ENPlusState emState
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/21  4.0			������                  ����
=============================================================================*/
void CNPlusData::SetLocalNPlusState( ENPlusState emState )
{
    m_emLocalNPlusState = emState;
}

/*=============================================================================
�� �� ���� GetRtdTime
��    �ܣ� rtd ʱ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� u16 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/22  4.0			������                  ����
=============================================================================*/
u16 CNPlusData::GetRtdTime( void )
{
    return m_wRtdTime;
}

/*=============================================================================
�� �� ���� GetRtdNum
��    �ܣ� rtd ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� u16 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/22  4.0			������                  ����
=============================================================================*/
u16 CNPlusData::GetRtdNum( void )
{
    return m_wRtdNum;
}

/*=============================================================================
�� �� ���� InitNPlusState
��    �ܣ� ��ʼ������mcu����״̬��(��Ҫ��mcu�����ʼ����ʹ��)
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/21  4.0			������                  ����
=============================================================================*/
BOOL32 CNPlusData::InitNPlusState( void )
{
    TLocalInfo tLocalInfo;
    BOOL32 bNPlusMode = FALSE;
    if (SUCCESS_AGENT == g_cMcuAgent.GetLocalInfo(&tLocalInfo))
    {
        m_wRtdTime = tLocalInfo.GetNPlusRtdTime();
        m_wRtdNum = tLocalInfo.GetNPlusRtdNum();
        if (tLocalInfo.IsNPlusMode())
        {
            bNPlusMode = TRUE;
            if (tLocalInfo.IsNPlusBackupMode())
            {
                m_emLocalNPlusState = MCU_NPLUS_SLAVE_IDLE;

                // m_byNPlusSynOk�ֶζԱ�MCUû�����壬�˴�ʼ����֮ΪTRUE [12/20/2006-zbq]
                m_byNPlusSynOk = TRUE;
            }
            else if (0 != tLocalInfo.GetNPlusMcuIp())
            {
                m_emLocalNPlusState = MCU_NPLUS_MASTER_IDLE;
            }
        }

        m_byLocalMcuType = GetMcuPdtType();
    }
    return bNPlusMode;
}

/*=============================================================================
�� �� ���� GetMcuEqpCap
��    �ܣ� ��ȡ����mcu��������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� TNPlusEqpCap  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/29  4.0			������                  ����
2006/12/13  4.0         ����                  ����Mp��MtAdp
=============================================================================*/
TNPlusEqpCap    CNPlusData::GetMcuEqpCap( void )
{
    TNPlusEqpCap tEqpCap;
    TPeriEqpStatus tStatus;
    for (u8 byEqpId = 1; byEqpId <= MAXNUM_MCU_PERIEQP; byEqpId++)
    {
        if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus) && 1 == tStatus.m_byOnline)
        {
			//[chendaiwei 2010/09/27]���ӻ������������ж�
			if (byEqpId <= MIXERID_MAX)
			{
				tEqpCap.m_byMixerNum ++;
			}
            else if (byEqpId <= BASID_MAX)
            {
/*
#ifdef _8KE_
			 if (TYPE_8KE_BAS == tStatus.m_tStatus.tHdBas.GetEqpType())
			 {
				tEqpCap.m_byBasVidChnlNum++;
				LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[GetMcuEqpCap]m_byBasVidChnlNum is: %d\n", tEqpCap.m_byBasVidChnlNum);
			 }			 
#endif*/

#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)	
                if (!g_cMcuAgent.IsEqpBasHD(byEqpId))
                {
                    for (u8 byLoop = 0; byLoop < tStatus.m_tStatus.tBas.byChnNum && byLoop < MAXNUM_BAS_CHNNL; byLoop++)
                    {
                        if (BAS_CHAN_AUDIO == tStatus.m_tStatus.tBas.tChnnl[byLoop].GetChannelType())
                        {
                            tEqpCap.m_byBasAudChnlNum++;
                        }
                        else if (BAS_CHAN_VIDEO == tStatus.m_tStatus.tBas.tChnnl[byLoop].GetChannelType())
                        {
                            tEqpCap.m_byBasVidChnlNum++;
                        }
                    } 
                }
                else
                {
                    if (TYPE_MAU_NORMAL == tStatus.m_tStatus.tHdBas.GetEqpType())
                    {
						//  [1/13/2011 chendaiwei]��Ҫͨ��tStatus.m_byOnline��EqpType�����������ж��Ƿ�����
						//  ������Ҫ��1��TYPE_MAU_NORMALĬ��ֵ��0��Ҳ����˵ʵ����EqpType��û�б�����,����ʼ��ֵ
						//  ����0���������С����Դ˴�����һ��״̬����Ϊ�������ж�����
						if(tStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus()->GetStatus() != TBasBaseChnStatus::IDLE)
						{
							tEqpCap.m_byMAUNum++;
						}

						LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS,"m_byMAUNum == %u\n",tEqpCap.m_byMAUNum);
                    }
                    else if (TYPE_MAU_H263PLUS == tStatus.m_tStatus.tHdBas.GetEqpType())
                    {
                        tEqpCap.m_byMAUH263pNum ++;
                    }
                    else if (TYPE_MPU == tStatus.m_tStatus.tHdBas.GetEqpType())
                    {
                        tEqpCap.m_byMpuBasNum ++;
                    }
					else if(TYPE_MPU_H == tStatus.m_tStatus.tHdBas.GetEqpType())
					{
						tEqpCap.m_byBap2BasNum ++;
					}
                    else if( TYPE_MPU2_ENHANCED != tStatus.m_tStatus.tHdBas.GetEqpType() && TYPE_MPU2_BASIC != tStatus.m_tStatus.tHdBas.GetEqpType())
                    {
                        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusData::GetMcuEqpCap] unexpected mau type.%d\n",
                                                tStatus.m_tStatus.tHdBas.GetEqpType());
                    }
                }
#endif
            }
            else if (byEqpId <= VMPID_MAX)
            {
                // 8000B ������Ϊ4����MAP��Ϊ2������£��Ծ���16ͨ���ĺϳ����� [12/29/2006-zbq]
                if ( MCU_TYPE_KDV8000B == GetMcuType() ||
                    MCU_TYPE_KDV8000C == GetMcuType() )
                {
                    if ( 4 == tStatus.m_tStatus.tVmp.m_byChlNum )
                    {
                        tStatus.m_tStatus.tVmp.m_byChlNum = 16;
                    }
                }
				
				if(tStatus.m_tStatus.tVmp.m_bySubType !=MPU2_VMP_BASIC && tStatus.m_tStatus.tVmp.m_bySubType != MPU2_VMP_ENHACED)
				{
					tEqpCap.m_byVmpNum++;
					// VMP�������嵽MAP�� [12/28/2006-zbq]
					tEqpCap.m_abyVMPChnNum[tEqpCap.m_byVmpNum-1] = tStatus.m_tStatus.tVmp.m_byChlNum;
				}
            }            
            else if (byEqpId <= PRSID_MAX)
            {
                tEqpCap.m_byPrsChnlNum += MAXNUM_PRS_CHNNL;
            }
        }
    }

    // guzh [12/13/2006] ��ȡMp��MtAdp
    tEqpCap.m_byMtAdpNum = g_cMcuVcApp.GetMtAdpNum(PROTOCOL_TYPE_H323);
    tEqpCap.m_byMpNum = g_cMcuVcApp.GetMpNum();

    //GK Charge [12/18/2006-zbq]
    tEqpCap.SetIsGKCharge(g_cMcuAgent.GetIsGKCharge());
    
    //DCS [12/20/2006-zbq]
    TPeriDcsStatus tDcsStatus;
    for( u8 byDcsId = 1; byDcsId <= MAXNUM_MCU_DCS; byDcsId ++ )
    {
        if ( g_cMcuVcApp.GetPeriDcsStatus(byDcsId, &tDcsStatus) && 1 == tDcsStatus.m_byOnline )
        {
            tEqpCap.m_byDCSNum ++;
        }
    }

    return tEqpCap;
}

/*=============================================================================
�� �� ���� GetMcuEqpCapEX
��    �ܣ� ��ȡ����mcu������չ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� TNPlusEqpCapEx  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/29  4.0			������                  ����
2006/12/13  4.0         ����                  ����Mp��MtAdp
=============================================================================*/
TNPlusEqpCapEx    CNPlusData::GetMcuEqpCapEx( void )
{
    TNPlusEqpCapEx tEqpCapEx;
    TPeriEqpStatus tStatus;
    for (u8 byEqpId = 1; byEqpId <= MAXNUM_MCU_PERIEQP; byEqpId++)
    {
        if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus) && 1 == tStatus.m_byOnline)
        {
			if (byEqpId <= MIXERID_MAX)
			{	
				u8 byMixerType = UNKONW_MIXER;
				if(g_cMcuAgent.GetMixerSubTypeByEqpId(byEqpId,byMixerType))
				{
					if(byMixerType == APU2_MIXER)
					{
						tEqpCapEx.m_byAPU2MixerNum++;
					}
					else if ( byMixerType  == MIXER_8KI)
					{
						tEqpCapEx.m_by8000IMixerNum++;
					}
				}
			}
            else if (byEqpId <= BASID_MAX)
            {
#ifdef _8KE_
				if (TYPE_8KE_BAS == tStatus.m_tStatus.tHdBas.GetEqpType())
				{
					tEqpCapEx.m_by8000GBasNum++;
				}			 
#endif

#ifdef _8KH_
				//8000H�ĺ���������ʹ�����[1/10/2012 chendaiwei]
				if (TYPE_8KH_BAS == tStatus.m_tStatus.tHdBas.GetEqpType())
				{
					tEqpCapEx.m_by8000HBasNum++;
				}			 
#endif

#ifdef _8KI_
				if (TYPE_8KI_VID_BAS == tStatus.m_tStatus.tHdBas.GetEqpType())
				{
					tEqpCapEx.m_by8000IVidBasNum++;
				}	
				if ( TYPE_8KI_AUD_BAS == tStatus.m_tStatus.tAudBas.GetEqpType())
				{
					tEqpCapEx.m_by8000IAudBasNum++;
				}
#endif
				if (g_cMcuAgent.IsEqpBasHD(byEqpId))
				{
					if(TYPE_MPU2_BASIC == tStatus.m_tStatus.tHdBas.GetEqpType())
					{
						tEqpCapEx.m_byMPU2BasicBasNum ++;
					}
					else if(TYPE_MPU2_ENHANCED == tStatus.m_tStatus.tHdBas.GetEqpType())
					{
						tEqpCapEx.m_byMPU2EnhancedBasNum++;
					}
				} 

				//TODO:IsEqpBasAud���� TYPE_8KI_AUD_BAS [7/29/2013 chendaiwei]
				if(g_cMcuAgent.IsEqpBasAud(byEqpId))
				{
					if( TYPE_APU2_BAS == tStatus.m_tStatus.tAudBas.GetEqpType())
					{	
						tEqpCapEx.m_byAPU2BasNum++;
					}
					 
				}
			}
            else if (byEqpId <= VMPID_MAX)
            {
				if(tStatus.m_tStatus.tVmp.m_bySubType == MPU2_VMP_BASIC)
				{
					if(g_cMcuAgent.GetMPU2TypeByVmpEqpId(byEqpId) == BRD_TYPE_MPU2)
					{
						tEqpCapEx.m_byMPU2BasicVmpNum++;
					}
					else if(g_cMcuAgent.GetMPU2TypeByVmpEqpId(byEqpId) == BRD_TYPE_MPU2ECARD)
					{
						tEqpCapEx.m_byMPU2EcardBasicVmpNum++;
					}
				}
				else if(tStatus.m_tStatus.tVmp.m_bySubType == MPU2_VMP_ENHACED)
				{
					tEqpCapEx.m_byMPU2EnhancedVmpNum++;
				}
            }            
        }
    }

    return tEqpCapEx;
}

/*=============================================================================
    �� �� ���� PackNplusExInfo
    ��    �ܣ� Pack���ݣ��洢��pbyOutbuf��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  TNPlusEqpCapEx &tEqpCapEx ��pack����
               [OUT] u8 *pbyOutbuf pack�����ݴ洢λ��
               [OUT] u16 &wPackExLen Pack������ݳ��� ������
               [IN]  TNPlusMcuInfoEx *ptMcuInfoEx ��pack����
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2011/12/29  4.0			chendaiwei              ����
    2013/03/13              liaokang                ֧��TNPlusMcuInfoEx
=============================================================================*/
void CNPlusData::PackNplusExInfo(const TNPlusEqpCapEx &tEqpCapEx,                                    
                                 u8 * pbyOutbuf, 
                                 u16 &wPackExLen,
                                 const TNPlusMcuInfoEx *ptMcuInfoEx/* = NULL*/)
{
	//��չ��Ϣ����  u16��չ��Ϣ�ܳ��� + 
    //              ��Ϣ����A��u8�� + ��Ϣ����A�ֽ�������ʶA���ݴ�С,u16�� + ��Ϣ����A���� + 
    //              ��Ϣ����B��u8��......
    wPackExLen = 0;
	u8 *pbyConfExSizeBuf = pbyOutbuf;
	u8 *pbyConfExBuf = pbyConfExSizeBuf + 2;

	//APU2
	if(tEqpCapEx.m_byAPU2MixerNum !=0)
	{
		*pbyConfExBuf = (u8)emAPU2MixerNum;
		pbyConfExBuf++;
		wPackExLen++;

		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);

		*pbyConfExBuf = tEqpCapEx.m_byAPU2MixerNum;
		pbyConfExBuf++;
		wPackExLen++;
	}

	//MPU2 BAS BAsic
	if(tEqpCapEx.m_byMPU2BasicBasNum !=0)
	{
		*pbyConfExBuf = (u8)emMPU2BasicBasNum;
		pbyConfExBuf++;
		wPackExLen++;

		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);

		*pbyConfExBuf = tEqpCapEx.m_byMPU2BasicBasNum;
		pbyConfExBuf++;
		wPackExLen++;
	}

	//MPU2 Ecard Bas enhanced
	if(tEqpCapEx.m_byMPU2EnhancedBasNum !=0)
	{
		*pbyConfExBuf = (u8)emMPU2EnhancedBasNum;
		pbyConfExBuf++;
		wPackExLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);

		*pbyConfExBuf = tEqpCapEx.m_byMPU2EnhancedBasNum;
		pbyConfExBuf++;
		wPackExLen++;
	}

	//MPU2 vmp basic
	if(tEqpCapEx.m_byMPU2BasicVmpNum !=0)
	{
		*pbyConfExBuf = (u8)emMPU2BasicVmpNum;
		pbyConfExBuf++;
		wPackExLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);

		*pbyConfExBuf = tEqpCapEx.m_byMPU2BasicVmpNum;
		pbyConfExBuf++;
		wPackExLen++;
	}

	//MPU2ECard vmp basic
	if(tEqpCapEx.m_byMPU2EcardBasicVmpNum !=0)
	{
		*pbyConfExBuf = (u8)emMPU2EcardBasicVmpNum;
		pbyConfExBuf++;
		wPackExLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);

		*pbyConfExBuf = tEqpCapEx.m_byMPU2EcardBasicVmpNum;
		pbyConfExBuf++;
		wPackExLen++;
	}

	//MPU2ECard vmp enhanced
	if(tEqpCapEx.m_byMPU2EnhancedVmpNum !=0)
	{
		*pbyConfExBuf = (u8)emMPU2EnhancedVmpNum;
		pbyConfExBuf++;
		wPackExLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);

		*pbyConfExBuf = tEqpCapEx.m_byMPU2EnhancedVmpNum;
		pbyConfExBuf++;
		wPackExLen++;
	}

	//8000G bas
	if(tEqpCapEx.m_by8000GBasNum !=0)
	{
		*pbyConfExBuf = (u8)em8000GBasNum;
		pbyConfExBuf++;
		wPackExLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);
		
		*pbyConfExBuf = tEqpCapEx.m_by8000GBasNum;
		pbyConfExBuf++;
		wPackExLen++;
	}

	//8000H bas
	if(tEqpCapEx.m_by8000HBasNum !=0)
	{
		*pbyConfExBuf = (u8)em8000HBasNum;
		pbyConfExBuf++;
		wPackExLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);
		
		*pbyConfExBuf = tEqpCapEx.m_by8000HBasNum;
		pbyConfExBuf++;
		wPackExLen++;
	}

	//apu2 bas
	if(tEqpCapEx.m_byAPU2BasNum !=0)
	{
		*pbyConfExBuf = (u8)emAPU2BasNum;
		pbyConfExBuf++;
		wPackExLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);
		
		*pbyConfExBuf = tEqpCapEx.m_byAPU2BasNum;
		pbyConfExBuf++;
		wPackExLen++;
	}

	//8000i vid bas
	if(tEqpCapEx.m_by8000IVidBasNum !=0)
	{
		*pbyConfExBuf = (u8)em8000IVidBasNum;
		pbyConfExBuf++;
		wPackExLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);
		
		*pbyConfExBuf = tEqpCapEx.m_by8000IVidBasNum;
		pbyConfExBuf++;
		wPackExLen++;
	}

	//8000i aud bas
	if(tEqpCapEx.m_by8000IAudBasNum !=0)
	{
		*pbyConfExBuf = (u8)em8000IAudBasNum;
		pbyConfExBuf++;
		wPackExLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);
		
		*pbyConfExBuf = tEqpCapEx.m_by8000IAudBasNum;
		pbyConfExBuf++;
		wPackExLen++;
	}

	//8000i mixer
	if(tEqpCapEx.m_by8000IMixerNum !=0)
	{
		*pbyConfExBuf = (u8)em8000IMixerNum;
		pbyConfExBuf++;
		wPackExLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);
		
		*pbyConfExBuf = tEqpCapEx.m_by8000IMixerNum;
		pbyConfExBuf++;
		wPackExLen++;
	}


    //MCU��Ϣ��չ
    if( NULL != ptMcuInfoEx /*&& !ptMcuInfoEx->IsNull()*/)
    {
        *pbyConfExBuf = (u8)emNplusMcuInfoEx;
        pbyConfExBuf++;
        wPackExLen++;
        
        *(u16*)pbyConfExBuf = htons(sizeof(TNPlusMcuInfoEx));
        pbyConfExBuf = pbyConfExBuf + sizeof(u16);
        wPackExLen = wPackExLen + sizeof(u16);
        
        memcpy(pbyConfExBuf,ptMcuInfoEx,sizeof(TNPlusMcuInfoEx));
        pbyConfExBuf = pbyConfExBuf + sizeof(TNPlusMcuInfoEx);
        wPackExLen = wPackExLen + sizeof(TNPlusMcuInfoEx);
	}

	*(u16*)pbyConfExSizeBuf = htons(wPackExLen);
	
	wPackExLen += sizeof(u16);
	
	return;
}

/*=============================================================================
    �� �� ���� UnPackNplusExInfo
    ��    �ܣ� Unpack����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [OUT]  TNPlusEqpCapEx &tNplusEqpCap ����unpack�������
               [IN]   const u8 *pbyBuf ��unpack������
               [OUT]  u16 &wUnPackExLen unPack������ݻ��峤�� ������
			   [OUT]  BOOL32 &bExistUnknowInfo ���ڱ�MCU�޷���������Ϣ����ΪTRUE
               [OUT]  TNPlusMcuInfoEx *ptMcuInfoEx ����unpack�������
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2011/12/29  4.0			chendaiwei              ����
    2013/03/13              liaokang                ֧��TNPlusMcuInfoEx
=============================================================================*/
void CNPlusData::UnPackNplusExInfo(TNPlusEqpCapEx & tNplusEqpCap, 
                                   const u8 *pbyInBuf, 
                                   u16 &wUnPackExLen,
                                   BOOL32 &bExistUnknowInfo, 
                                   TNPlusMcuInfoEx *ptMcuInfoEx/* = NULL*/)
{
    //��չ��Ϣ����  u16��չ��Ϣ�ܳ��� + 
    //              ��Ϣ����A��u8�� + ��Ϣ����A�ֽ�������ʶA���ݴ�С,u16�� + ��Ϣ����A���� + 
    //              ��Ϣ����B��u8��......
    u16 wNplusInfoExSize = ntohs( *(u16*)pbyInBuf);
	wUnPackExLen = wNplusInfoExSize + sizeof(u16);
	pbyInBuf = pbyInBuf + sizeof(u16);
	bExistUnknowInfo = FALSE;

	s32 nNplusInfoExSize = wNplusInfoExSize;

	while( nNplusInfoExSize > 0)
	{
		u16 wLength = 0;
		emNPlusCommunicateType etype = (emNPlusCommunicateType)(*pbyInBuf);
		pbyInBuf ++;
		
		switch(etype)
		{
		case emAPU2MixerNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_byAPU2MixerNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;
		case emMPU2BasicBasNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_byMPU2BasicBasNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;
		case emMPU2EnhancedBasNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_byMPU2EnhancedBasNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;
		case emMPU2BasicVmpNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_byMPU2BasicVmpNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;
		case emMPU2EcardBasicVmpNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_byMPU2EcardBasicVmpNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;
		case emMPU2EnhancedVmpNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_byMPU2EnhancedVmpNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;
		case em8000GBasNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_by8000GBasNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;
		case em8000HBasNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_by8000HBasNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;
		case emAPU2BasNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_byAPU2BasNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;

		case em8000IAudBasNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_by8000IAudBasNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;

		case em8000IVidBasNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_by8000IVidBasNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;

		case em8000IMixerNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_by8000IMixerNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;

        case emNplusMcuInfoEx:
            if( NULL != ptMcuInfoEx )
            {
                wLength = ntohs( *(u16*)pbyInBuf);
                pbyInBuf = pbyInBuf + sizeof(u16);
                memcpy(ptMcuInfoEx,pbyInBuf,min(wLength, sizeof(TNPlusMcuInfoEx)));
                pbyInBuf = pbyInBuf + wLength;
            }
            else
            {
                wLength = ntohs( *(u16*)pbyInBuf);
                pbyInBuf = pbyInBuf + sizeof(u16) +wLength;
                bExistUnknowInfo = TRUE;
                LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[UnPackNplusExInfo] ptMcuInfoEx is Null, wrong!\n");
            }
            
            break;
		default:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16) +wLength;
			bExistUnknowInfo = TRUE;
			
			break;
		}
		
		nNplusInfoExSize = nNplusInfoExSize - (s32)sizeof(u8) - (s32)sizeof(u16) - (s32)wLength;
	}

	return;
}

/*=============================================================================
�� �� ���� GetEqpCapFromConf
��    �ܣ� �ӻ�����Ϣ��ȡ����mcu����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TConfInfo &tConf
�� �� ֵ�� TNPlusEqpCap  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/30  4.0			������                  ����
=============================================================================*/
void CNPlusData::GetEqpCapFromConf( const TConfInfo &tConf, TNPlusEqpCap &tEqpCap, TNPlusEqpCapEx &tCapEx, TBasChnAbility *ptBasAbility, u8& byBasChnNum)
{

	//��ʼ��
	byBasChnNum = 0;

	//[chendaiwei 2010/09/28]���ӻ���������
	if ( tConf.m_tStatus.IsMixing())
	{
		tEqpCap.m_byMixerNum ++;
	}

	//TBD [1/13/2012 chendaiwei] ���apu2�Ĵ����߼�.������ʹ��APU2����APU2���Ǳ���ģ���tCapEx.m_byAPU2Num������

	u8 byConfIdx = MIN_CONFIDX;
    //vmp
	for ( byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++)
	{
		CMcuVcInst * pcVcInst = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
		if (NULL == pcVcInst)
		{
			continue;
		}
		
		if (0 == memcmp(pcVcInst->m_tConf.GetConfE164(), tConf.GetConfE164(), MAXLEN_E164))
		{
			TPeriEqpStatus tPeriEqpStatus; 
			u8 byVmpSubType;
			for (u8 byVmpId = VMPID_MIN; byVmpId <= VMPID_MAX; byVmpId++)
			{
				g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
				// �����Ǹû����vmp����δ����������vmp
				if (tPeriEqpStatus.GetConfIdx() != byConfIdx || TVmpStatus::START != tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
				{
					continue;
				}
				byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
				if(byVmpSubType == MPU2_VMP_BASIC)
				{
					if(g_cMcuAgent.GetMPU2TypeByVmpEqpId(byVmpId) == BRD_TYPE_MPU2)
					{
						tCapEx.m_byMPU2BasicVmpNum++;
					}
					else if(g_cMcuAgent.GetMPU2TypeByVmpEqpId(byVmpId) == BRD_TYPE_MPU2ECARD)
					{
						tCapEx.m_byMPU2EcardBasicVmpNum++;
					}
				}
				else if(byVmpSubType == MPU2_VMP_ENHACED)
				{
					tCapEx.m_byMPU2EnhancedVmpNum++;
				}
				else
				{
					tEqpCap.m_byVmpNum++;
				}
			}
		}
	}

    //bas
    if (tConf.GetConfAttrb().IsUseAdapter())
    {
		//����vc���л��飬ȡConfId��ȵĳ��������й㲥Basռ�������ͳ��
		for ( byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++)
		{
			CMcuVcInst * pcVcInst = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
			if (NULL == pcVcInst)
			{
				continue;
			}
			if (0 == memcmp(pcVcInst->m_tConf.GetConfE164(), tConf.GetConfE164(), MAXLEN_E164))
			{
				CBasChn *apcBrdBasChn[MAXNUM_PERIEQP]={NULL};
				TPeriEqpStatus tBasStatus;
				u8 byChnNum = 0;
				if(pcVcInst->GetBasAllBrdChn(byChnNum, apcBrdBasChn) && byChnNum > 0)
				{
					for (u8 byIdx = 0; byIdx < byChnNum; byIdx++)
					{
						if ( apcBrdBasChn[byIdx] == NULL )
						{
							continue;
						}

						if(g_cMcuVcApp.GetPeriEqpStatus(apcBrdBasChn[byIdx]->GetBas().GetEqpId(), &tBasStatus))
						{
#ifdef _8KE_
							if (TYPE_8KE_BAS == tBasStatus.m_tStatus.tHdBas.GetEqpType())
							{
								if(tBasStatus.m_tStatus.tHdBas.tStatus.t8keBas.GetVidChnStatus(0)->GetType() == BAS_8KECHN_MV)
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(em8KGBrdChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								else if(tBasStatus.m_tStatus.tHdBas.tStatus.t8keBas.GetVidChnStatus(0)->GetType() == BAS_8KECHN_DS)
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(em8KGDSChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								else if(tBasStatus.m_tStatus.tHdBas.tStatus.t8keBas.GetVidChnStatus(0)->GetType() == BAS_8KECHN_SEL)
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(em8KGSelChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								
							}						
#elif defined(_8KH_)
							if (TYPE_8KH_BAS == tBasStatus.m_tStatus.tHdBas.GetEqpType())
							{
								if(tBasStatus.m_tStatus.tHdBas.tStatus.t8khBas.GetVidChnStatus(0)->GetType() == BAS_8KHCHN_MV)
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(em8KHBrdChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								else if(tBasStatus.m_tStatus.tHdBas.tStatus.t8khBas.GetVidChnStatus(0)->GetType() == BAS_8KHCHN_DS)
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(em8KHDSChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								else if(tBasStatus.m_tStatus.tHdBas.tStatus.t8khBas.GetVidChnStatus(0)->GetType() == BAS_8KHCHN_SEL)
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(em8KHSelChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								
							}
#elif defined(_8KI_)
							if (TYPE_8KI_VID_BAS == tBasStatus.m_tStatus.tHdBas.GetEqpType())
							{
								if(tBasStatus.m_tStatus.tHdBas.tStatus.t8kiBas.GetVidChnStatus(0)->GetType() == BAS_8KICHN0)
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(em8KIVidBasChn0);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								else if(tBasStatus.m_tStatus.tHdBas.tStatus.t8kiBas.GetVidChnStatus(0)->GetType() == BAS_8KICHN1)
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(em8KIVidBasChn1);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								else if(tBasStatus.m_tStatus.tHdBas.tStatus.t8kiBas.GetVidChnStatus(0)->GetType() == BAS_8KICHN2)
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(em8KIVidBasChn2);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								
							}

							//TODO:IsEqpBasAud���޸�
							//if( g_cMcuAgent.IsEqpBasAud(apcBrdBasChn[byIdx]->GetBas().GetEqpId()))
							{
								if ( TYPE_8KI_AUD_BAS == tBasStatus.m_tStatus.tAudBas.GetEqpType())
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(em8KIAudBasChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
							}
#else
							if( g_cMcuAgent.IsEqpBasAud(apcBrdBasChn[byIdx]->GetBas().GetEqpId()))
							{
								if ( TYPE_APU2_BAS == tBasStatus.m_tStatus.tAudBas.GetEqpType())
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(emAPU2BasChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
							}
							else if (!g_cMcuAgent.IsEqpBasHD(apcBrdBasChn[byIdx]->GetBas().GetEqpId()))
							{
								if (BAS_CHAN_AUDIO == tBasStatus.m_tStatus.tBas.tChnnl[apcBrdBasChn[byIdx]->GetChnId()].GetChannelType())
								{
									tEqpCap.m_byBasAudChnlNum++;
								}
								else if (BAS_CHAN_VIDEO == tBasStatus.m_tStatus.tBas.tChnnl[apcBrdBasChn[byIdx]->GetChnId()].GetChannelType())
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(emVpuChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
							}
							else
							{
								if (TYPE_MAU_NORMAL == tBasStatus.m_tStatus.tHdBas.GetEqpType())
								{			
									*ptBasAbility = GetBasChnAbilityByBasChnType(emMAUNormal);
									ptBasAbility ++;
									byBasChnNum ++;								
								}
								else if (TYPE_MAU_H263PLUS == tBasStatus.m_tStatus.tHdBas.GetEqpType())
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(emMAUH263PlusChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								else if (TYPE_MPU == tBasStatus.m_tStatus.tHdBas.GetEqpType())
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(emMPUChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								else if (TYPE_MPU_H == tBasStatus.m_tStatus.tHdBas.GetEqpType())
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(emMPU_HChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								else if(TYPE_MPU2_BASIC == tBasStatus.m_tStatus.tHdBas.GetEqpType())
								{
									//BRD chnnl
									if(apcBrdBasChn[byIdx]->GetChnId() == 0 || apcBrdBasChn[byIdx]->GetChnId() == 1)
									{
										*ptBasAbility = GetBasChnAbilityByBasChnType(emMPU2BasicBrdChn);
									}
									//Sel chnnl
									else
									{
										*ptBasAbility = GetBasChnAbilityByBasChnType(emMPU2BasicSelChn);
									}
		
									ptBasAbility ++;
									byBasChnNum ++;
								}
								else if(TYPE_MPU2_ENHANCED == tBasStatus.m_tStatus.tHdBas.GetEqpType())
								{
									//BRD chnnl
									if(apcBrdBasChn[byIdx]->GetChnId() == 0 || apcBrdBasChn[byIdx]->GetChnId() == 1)
									{
										*ptBasAbility = GetBasChnAbilityByBasChnType(emMPU2EnhancedBrdChn);
									}
									//Sel chnnl
									else
									{
										*ptBasAbility = GetBasChnAbilityByBasChnType(emMPU2EnhancedSelChn);
									}
									
									ptBasAbility ++;
									byBasChnNum ++;
								}
								else
								{
									LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[GetEqpCapFromConf] ConIdx.%d unexpected  type.%d\n",
															pcVcInst->m_byConfIdx, tBasStatus.m_tStatus.tHdBas.GetEqpType());
								}
							}
#endif
						}
						else
						{
							LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[GetEqpCapFromConf] ConfIdx.%d GetPeriEqpStatus BasId.%d failed!\n",
													pcVcInst->m_byConfIdx, apcBrdBasChn[byIdx]->GetBas().GetEqpId());
						}				
					}
					LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "[GetEqpCapFromConf] ConfIdx.%d Ocuppy %d BasChn<VpuVid.%d, VpuAud.%d, Mau.%d, MauH263.%d, Mpu-4.%d, Mpu-2.%d>!\n", 
												pcVcInst->m_byConfIdx,
												byChnNum,
												tEqpCap.m_byBasVidChnlNum, 
												tEqpCap.m_byBasAudChnlNum,
												tEqpCap.m_byMAUNum,
												tEqpCap.m_byMAUH263pNum,
												tEqpCap.m_byMpuBasNum,
												tEqpCap.m_byBap2BasNum);

				}
				else
				{
					LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[GetEqpCapFromConf] ConfIdx.%d GetBasAllBrdChn failed!\n", pcVcInst->m_byConfIdx);
				}
				break;
			}
		}
// #ifdef _8KE_
// 		u8 byNVChn = 0;
// 		u8 byDSChn = 0;
// 
// 		tEqpCap.m_byBasVidChnlNum = GetBasChnNumNeededByConf(tConf, byNVChn, byDSChn);
// #else
//         if (!IsHDConf(tConf))
//         {
//             if (tConf.GetSecAudioMediaType() != MEDIA_TYPE_NULL)
//             {
//                 tEqpCap.m_byBasAudChnlNum++;
//             }
//             else if (tConf.GetSecVideoMediaType() != MEDIA_TYPE_NULL)
//             {
//                 tEqpCap.m_byBasVidChnlNum++;
//             }
//             
//             if (tConf.GetSecBitRate() != 0)
//             {
//                 tEqpCap.m_byBasVidChnlNum++;
//             }
//         }
//         else
//         {
//             u8 byNVChn = 0;
//             u8 byDSChn = 0;
//             u8 byH263pChn = 0;
//             u8 byVGAChn = 0;
// 
//             u8 byMpuNum = 0;
//             g_cMcuVcApp.GetMpuNum(byMpuNum);
// 
//             //��mpu����Ϊ�Ǵ�mpu���ݣ��ݲ�֧��mpu��mau�Ľ��汸��
//             if (byMpuNum > 0)
//             {
//                 //CBasMgr cBasMgr;
//                 //cBasMgr.GetNeededMpu(tConf, byNVChn, byDSChn);
// 				GetMpuChnNumNeededByConf(tConf, byNVChn, byDSChn);
//                 tEqpCap.m_byMpuBasNum += (byNVChn + byDSChn)/MAXNUM_MPU_CHN + ((byNVChn + byDSChn)%MAXNUM_MPU_CHN ? 1 : 0);
//             }
//             else
//             {
//                 //CBasMgr cBasMgr;
//                 //cBasMgr.GetNeededMau(tConf, byNVChn, byH263pChn, byVGAChn);
//                 GetMauChnNumNeededByConf(tConf, byNVChn, byH263pChn, byVGAChn);
//                 tEqpCap.m_byMAUNum += byNVChn;
//                 tEqpCap.m_byMAUH263pNum += byH263pChn;
//             }
//         }
// #endif
    }

	TConfAttrb tTempConfAttrb = tConf.GetConfAttrb();
    //prs
    if (tTempConfAttrb.IsResendLosePack())
    {
        tEqpCap.m_byPrsChnlNum++;
    }
    //GK Charge [12/18/2006-zbq]
    tEqpCap.SetIsGKCharge(tConf.m_tStatus.IsGkCharge());
    
    return;
}

/*=============================================================================
�� �� ���� SetNPlusSynOk
��    �ܣ� �Ƿ����ݱ��ݳɹ�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� BOOL32 bSynOk
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/28  4.0			������                  ����
=============================================================================*/
void CNPlusData::SetNPlusSynOk( BOOL32 bSynOk )
{
    m_byNPlusSynOk = bSynOk ? 1 : 0;
}

/*=============================================================================
�� �� ���� GetNPlusSynOk
��    �ܣ� �Ƿ����ݱ��ݳɹ�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/28  4.0			������                  ����
=============================================================================*/
BOOL32 CNPlusData::GetNPlusSynOk( void )
{
    return (1 == m_byNPlusSynOk ? TRUE : FALSE);
}

/*=============================================================================
�� �� ���� SetMcuType
��    �ܣ� ���ñ���MCU������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byType
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/12/20  4.0			�ű���                  ����
=============================================================================*/
void CNPlusData::SetMcuType( u8 byType )
{
    m_byLocalMcuType = byType;
    return;
}

/*=============================================================================
�� �� ���� GetMcuType
��    �ܣ� ��ȡ����MCU������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� u8 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/12/20  4.0			�ű���                  ����
=============================================================================*/
u8 CNPlusData::GetMcuType( void )
{
    return m_byLocalMcuType;
}

/*=============================================================================
�� �� ���� PostMsgToNPlusDaemon
��    �ܣ� Mcu ҵ������Ӧ�÷�����Ϣ��N+1 Deamon
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CServMsg &cServMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/22  4.0			������                  ����
=============================================================================*/
void CNPlusData::PostMsgToNPlusDaemon( u16 wEvent, u8 *const pbyMsg, u16 wMsgLen )
{
    if ( m_emLocalNPlusState != MCU_NPLUS_IDLE )
    {
        OspPost(MAKEIID(AID_MCU_NPLUSMANAGER, CInstance::DAEMON), wEvent, pbyMsg, wMsgLen);
    }    
}

/*=============================================================================
    �� �� ���� GetBasChnAbilityByBasChnType
    ��    �ܣ� ͨ��BASͨ�����ͻ�ȡBas��������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]u8 byBasChnType Basͨ������
    �� �� ֵ�� TBasChnAbility ͨ����������
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2011/12/29  4.0			chendaiwei                  ����
=============================================================================*/
TBasChnAbility CNPlusData::GetBasChnAbilityByBasChnType( emBasChnType byBasChnType)
{	
	TBasChnAbility tBasChn;
	
	switch(byBasChnType)
	{
	case emVpuChn:
		tBasChn.m_emChnType = emVpuChn;
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,emOther);
		tBasChn.SetDecAbility(emDecSD);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 1;
		break;

	case em8KHSelChn:
		tBasChn.m_emChnType = em8KHSelChn;
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,emOther);
		tBasChn.SetDecAbility(emDecSD);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 1;
		break;

	case emMAUH263PlusChn:
		tBasChn.m_emChnType = emMAUH263PlusChn;
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,emOther);
		tBasChn.SetEncAbility(0,emh263Plus);
		tBasChn.SetDecAbility(emDecHD30);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 1;
		break;

	case em8KHDSChn:
		tBasChn.m_emChnType = em8KHDSChn;
		tBasChn.SetEncAbility(0,emXgaBP);
		tBasChn.SetEncAbility(0,emh263Plus);
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(1,emXgaBP);
		tBasChn.SetEncAbility(1,emh263Plus);
		tBasChn.SetDecAbility(emDecHD30);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 2;
		break;

	case em8KGDSChn:
		tBasChn.m_emChnType = em8KGDSChn;
		tBasChn.SetEncAbility(0,emXgaBP);
		tBasChn.SetEncAbility(0,emh263Plus);
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(1,emXgaBP);
		tBasChn.SetEncAbility(1,emh263Plus);
		tBasChn.SetDecAbility(emDecHD30);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 2;
		break;

	case emMAUNormal:
		tBasChn.m_emChnType = emMAUNormal;
		tBasChn.SetEncAbility(0,emXgaBP);
		tBasChn.SetEncAbility(0,emSxga30BP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,em10802530BP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(1,emcifBP);
		tBasChn.SetEncAbility(1,emOther);
		tBasChn.SetEncAbility(1,emh263Plus);
		tBasChn.SetDecAbility(emDecHD30);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 2;
		break;

	case emMPUChn:
		tBasChn.m_emChnType = emMPUChn;
		tBasChn.SetEncAbility(0,emXgaBP);
		tBasChn.SetEncAbility(0,emSxga30BP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,em10802530BP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(1,emcifBP);
		tBasChn.SetEncAbility(1,emOther);
		tBasChn.SetEncAbility(1,emh263Plus);
		tBasChn.SetDecAbility(emDecHD30);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 2;
		break;

	case emMPU2BasicSelChn:
		tBasChn.m_emChnType = emMPU2BasicSelChn;
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(0,em7202530HP);
		tBasChn.SetEncAbility(0,em7205060BP);
		tBasChn.SetEncAbility(0,em7205060HP);
		tBasChn.SetEncAbility(0,emXgaBP);
		tBasChn.SetEncAbility(0,emXgaHP);
		tBasChn.SetEncAbility(0,emSxga30BP);
		tBasChn.SetEncAbility(0,emSxga30HP);
		tBasChn.SetEncAbility(0,emUxga60BP);
		tBasChn.SetEncAbility(0,emUxga60HP);
		tBasChn.SetEncAbility(0,em10802530BP);
		tBasChn.SetEncAbility(0,em10802530HP);
		tBasChn.SetEncAbility(0,em10805060BP);
		tBasChn.SetEncAbility(1,emOther);
		tBasChn.SetEncAbility(1,emh263Plus);
		tBasChn.SetEncAbility(2,emOther);
		tBasChn.SetEncAbility(2,emh263Plus);
		tBasChn.SetDecAbility(emDecHP);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 3;
		break;
		
	case emMPU2BasicDsChn:
		tBasChn.m_emChnType = emMPU2BasicDsChn;
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(0,em7202530HP);
		tBasChn.SetEncAbility(0,em7205060BP);
		tBasChn.SetEncAbility(0,em7205060HP);
		tBasChn.SetEncAbility(0,em10802530BP);
		tBasChn.SetEncAbility(0,em10802530HP);
		tBasChn.SetEncAbility(0,em10805060BP);
		tBasChn.SetEncAbility(0,em10805060HP);

		tBasChn.SetEncAbility(1,emcifBP);
		tBasChn.SetEncAbility(1,em4cifBP);
		tBasChn.SetEncAbility(1,em7202530BP);
		tBasChn.SetEncAbility(1,em7202530HP);
		tBasChn.SetEncAbility(1,em7205060BP);
		tBasChn.SetEncAbility(1,em7205060HP);
		tBasChn.SetEncAbility(1,em10802530BP);
		tBasChn.SetEncAbility(1,em10802530HP);

		tBasChn.SetEncAbility(2,emcifBP);
		tBasChn.SetEncAbility(2,em4cifBP);
		tBasChn.SetEncAbility(2,em7202530BP);
		tBasChn.SetEncAbility(2,em7202530HP);
		
		tBasChn.SetEncAbility(3,emcifBP);
		tBasChn.SetEncAbility(3,em4cifBP);
		
		tBasChn.SetEncAbility(4,emcifBP);
		tBasChn.SetEncAbility(4,emOther);
		tBasChn.SetEncAbility(4,emh263Plus);
		
		tBasChn.SetEncAbility(5,emcifBP);
		tBasChn.SetEncAbility(5,emOther);
		tBasChn.SetEncAbility(5,emh263Plus);
		
		tBasChn.SetDecAbility(emDecHP);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 6;
		break;

	case emMPU2EnhancedSelChn:
		tBasChn.m_emChnType = emMPU2EnhancedSelChn;
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(0,em7202530HP);
		tBasChn.SetEncAbility(0,em7205060BP);
		tBasChn.SetEncAbility(0,em7205060HP);
		tBasChn.SetEncAbility(0,emXgaBP);
		tBasChn.SetEncAbility(0,emXgaHP);
		tBasChn.SetEncAbility(0,emSxga30BP);
		tBasChn.SetEncAbility(0,emSxga30HP);
		tBasChn.SetEncAbility(0,emUxga60BP);
		tBasChn.SetEncAbility(0,emUxga60HP);
		tBasChn.SetEncAbility(0,em10802530BP);
		tBasChn.SetEncAbility(0,em10802530HP);
		tBasChn.SetEncAbility(0,em10805060BP);
		tBasChn.SetEncAbility(0,em10805060HP);
		tBasChn.SetEncAbility(1,emOther);
		tBasChn.SetEncAbility(1,emh263Plus);
		tBasChn.SetEncAbility(2,emOther);
		tBasChn.SetEncAbility(2,emh263Plus);
		tBasChn.SetDecAbility(emDecHP);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 3;
		break;
		
	case em8KGSelChn:
		tBasChn.m_emChnType = em8KGSelChn;
		tBasChn.SetEncAbility(0,emOther);
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetDecAbility(emDecSD);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 1;
		break;

	case em8KGBrdChn:
		tBasChn.m_emChnType = em8KGBrdChn;
		tBasChn.SetEncAbility(0,emOther);
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(1,emOther);
		tBasChn.SetEncAbility(1,emcifBP);
		tBasChn.SetEncAbility(1,em4cifBP);
		tBasChn.SetEncAbility(2,emOther);
		tBasChn.SetEncAbility(2,emcifBP);
		tBasChn.SetEncAbility(3,emOther);
		tBasChn.SetDecAbility(emDecHD30);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 4;
		break;
	
	case em8KHBrdChn:
		tBasChn.m_emChnType = em8KHBrdChn;
		tBasChn.SetEncAbility(0,emOther);
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(0,em10802530BP);
		tBasChn.SetEncAbility(1,emOther);
		tBasChn.SetEncAbility(1,emcifBP);
		tBasChn.SetEncAbility(1,em4cifBP);
		tBasChn.SetEncAbility(1,em7202530BP);
		tBasChn.SetEncAbility(2,emOther);
		tBasChn.SetEncAbility(2,emcifBP);
		tBasChn.SetEncAbility(2,em4cifBP);
		tBasChn.SetEncAbility(3,emOther);
		tBasChn.SetEncAbility(3,emcifBP);
		tBasChn.SetEncAbility(4,emOther);
		tBasChn.SetDecAbility(emDecHD60);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 5;
		break;

	case emMPU_HChn:
		tBasChn.m_emChnType = emMPU_HChn;
		tBasChn.SetEncAbility(0,emXgaBP);
		tBasChn.SetEncAbility(0,emSxga30BP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(0,em10802530BP);
		
		tBasChn.SetEncAbility(1,em7202530BP);
		tBasChn.SetEncAbility(2,em4cifBP);
		tBasChn.SetEncAbility(3,emcifBP);
		
		tBasChn.SetEncAbility(4,emcifBP);
		tBasChn.SetEncAbility(4,em4cifBP);
		tBasChn.SetEncAbility(4,emOther);
		tBasChn.SetEncAbility(4,emh263Plus);
		
		tBasChn.SetEncAbility(5,emcifBP);
		tBasChn.SetEncAbility(5,em4cifBP);
		tBasChn.SetEncAbility(5,emOther);
		tBasChn.SetEncAbility(5,emh263Plus);
		tBasChn.SetDecAbility(emDecHD60);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 6;
		break;

	case emMPU_H60Chn:
		tBasChn.m_emChnType = emMPU_H60Chn;
		tBasChn.SetEncAbility(0,em7205060BP);
		tBasChn.SetEncAbility(0,em10805060BP);
		
		tBasChn.SetEncAbility(1,emcifBP);
		tBasChn.SetEncAbility(1,em4cifBP);
		tBasChn.SetEncAbility(1,emOther);
		tBasChn.SetEncAbility(1,emh263Plus);
		
		tBasChn.SetEncAbility(2,emcifBP);
		tBasChn.SetEncAbility(2,em4cifBP);
		tBasChn.SetEncAbility(2,emOther);
		tBasChn.SetEncAbility(2,emh263Plus);
		tBasChn.SetDecAbility(emDecHD60);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 3;
		break;

	case emMPU2BasicBrdChn:
		tBasChn.m_emChnType = emMPU2BasicBrdChn;
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(0,em7202530HP);
		tBasChn.SetEncAbility(0,em7205060BP);
		tBasChn.SetEncAbility(0,em7205060HP);
		tBasChn.SetEncAbility(0,em10802530BP);
		tBasChn.SetEncAbility(0,em10802530HP);
		tBasChn.SetEncAbility(0,em10805060BP);
		tBasChn.SetEncAbility(0,em10805060HP);

		tBasChn.SetEncAbility(1,emcifBP);
		tBasChn.SetEncAbility(1,em4cifBP);
		tBasChn.SetEncAbility(1,em7202530BP);
		tBasChn.SetEncAbility(1,em7202530HP);
		tBasChn.SetEncAbility(1,em7205060BP);
		tBasChn.SetEncAbility(1,em7205060HP);
		tBasChn.SetEncAbility(1,em10802530BP);
		tBasChn.SetEncAbility(1,em10802530HP);

		tBasChn.SetEncAbility(2,emcifBP);
		tBasChn.SetEncAbility(2,em4cifBP);
		tBasChn.SetEncAbility(2,em7202530BP);
		tBasChn.SetEncAbility(2,em7202530HP);

		tBasChn.SetEncAbility(3,emcifBP);
		tBasChn.SetEncAbility(3,em4cifBP);

		tBasChn.SetEncAbility(4,emcifBP);
		tBasChn.SetEncAbility(4,emOther);
		tBasChn.SetEncAbility(4,emh263Plus);

		tBasChn.SetEncAbility(5,emcifBP);
		tBasChn.SetEncAbility(5,emOther);
		tBasChn.SetEncAbility(5,emh263Plus);

		tBasChn.SetDecAbility(emDecHP);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 6;
		break;

	case emMPU2EnhancedBrdChn:
		tBasChn.m_emChnType = emMPU2EnhancedBrdChn;
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(0,em7202530HP);
		tBasChn.SetEncAbility(0,em7205060BP);
		tBasChn.SetEncAbility(0,em7205060HP);
		tBasChn.SetEncAbility(0,em10802530BP);
		tBasChn.SetEncAbility(0,em10802530HP);
		tBasChn.SetEncAbility(0,em10805060BP);
		tBasChn.SetEncAbility(0,em10805060HP);
		
		tBasChn.SetEncAbility(1,emcifBP);
		tBasChn.SetEncAbility(1,em4cifBP);
		tBasChn.SetEncAbility(1,em7202530BP);
		tBasChn.SetEncAbility(1,em7202530HP);
		tBasChn.SetEncAbility(1,em7205060BP);
		tBasChn.SetEncAbility(1,em7205060HP);
		tBasChn.SetEncAbility(1,em10802530BP);
		tBasChn.SetEncAbility(1,em10802530HP);
		tBasChn.SetEncAbility(1,em10805060BP);
		tBasChn.SetEncAbility(1,em10805060HP);

		tBasChn.SetEncAbility(2,emcifBP);
		tBasChn.SetEncAbility(2,em4cifBP);
		tBasChn.SetEncAbility(2,em7202530BP);
		tBasChn.SetEncAbility(2,em7202530HP);
		
		tBasChn.SetEncAbility(3,emcifBP);
		tBasChn.SetEncAbility(3,em4cifBP);
		
		tBasChn.SetEncAbility(4,emcifBP);
		tBasChn.SetEncAbility(4,emOther);
		tBasChn.SetEncAbility(4,emh263Plus);
		
		tBasChn.SetEncAbility(5,emcifBP);
		tBasChn.SetEncAbility(5,emOther);
		tBasChn.SetEncAbility(5,emh263Plus);
		
		tBasChn.SetDecAbility(emDecHP);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 6;
		break;
	case emMPU2EnhancedDsChn:
		tBasChn.m_emChnType = emMPU2EnhancedBrdChn;
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(0,em7202530HP);
		tBasChn.SetEncAbility(0,em7205060BP);
		tBasChn.SetEncAbility(0,em7205060HP);
		tBasChn.SetEncAbility(0,em10802530BP);
		tBasChn.SetEncAbility(0,em10802530HP);
		tBasChn.SetEncAbility(0,em10805060BP);
		tBasChn.SetEncAbility(0,em10805060HP);
		
		tBasChn.SetEncAbility(1,emcifBP);
		tBasChn.SetEncAbility(1,em4cifBP);
		tBasChn.SetEncAbility(1,em7202530BP);
		tBasChn.SetEncAbility(1,em7202530HP);
		tBasChn.SetEncAbility(1,em7205060BP);
		tBasChn.SetEncAbility(1,em7205060HP);
		tBasChn.SetEncAbility(1,em10802530BP);
		tBasChn.SetEncAbility(1,em10802530HP);
		tBasChn.SetEncAbility(1,em10805060BP);
		tBasChn.SetEncAbility(1,em10805060HP);
		
		tBasChn.SetEncAbility(2,emcifBP);
		tBasChn.SetEncAbility(2,em4cifBP);
		tBasChn.SetEncAbility(2,em7202530BP);
		tBasChn.SetEncAbility(2,em7202530HP);
		
		tBasChn.SetEncAbility(3,emcifBP);
		tBasChn.SetEncAbility(3,em4cifBP);
		
		tBasChn.SetEncAbility(4,emcifBP);
		tBasChn.SetEncAbility(4,emOther);
		tBasChn.SetEncAbility(4,emh263Plus);
		
		tBasChn.SetEncAbility(5,emcifBP);
		tBasChn.SetEncAbility(5,emOther);
		tBasChn.SetEncAbility(5,emh263Plus);
		
		tBasChn.SetDecAbility(emDecHP);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 6;
		break;

	case emAPU2BasChn:
		tBasChn.m_emChnType = emAPU2BasChn;
		tBasChn.m_byBasMediaType = MODE_AUDIO;
		tBasChn.m_byEncNum = MAXNUM_APU2BAS_OUTCHN;
		break;

	//TODO:����������[7/29/2013 chendaiwei]
	case em8KIVidBasChn0:
		tBasChn.m_emChnType = em8KIVidBasChn0;
		tBasChn.SetEncAbility(0,emOther);
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(0,em7202530HP);
		tBasChn.SetEncAbility(0,em7205060BP);
		tBasChn.SetEncAbility(0,em7205060HP);
		tBasChn.SetEncAbility(0,em10802530BP);
		tBasChn.SetEncAbility(0,em10802530HP);
		
		tBasChn.SetEncAbility(1,emOther);
		tBasChn.SetEncAbility(1,emcifBP);
		tBasChn.SetEncAbility(1,em4cifBP);
		tBasChn.SetEncAbility(1,em7202530BP);
		tBasChn.SetEncAbility(1,em7202530HP);		

		tBasChn.SetEncAbility(2,emOther);	
		tBasChn.SetEncAbility(2,emcifBP);
		tBasChn.SetEncAbility(2,em4cifBP);

		tBasChn.SetEncAbility(3,emcifBP);
		tBasChn.SetEncAbility(3,emOther);
		
		tBasChn.SetEncAbility(4,emOther);
		
		tBasChn.SetDecAbility(emDecHP);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 5;

		break;

	case em8KIVidBasChn1:
		tBasChn.m_emChnType = em8KIVidBasChn1;
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,emXgaBP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(0,emSxga20BP);

		//tBasChn.SetEncAbility(1,emcifBP);
		//tBasChn.SetEncAbility(1,emXgaBP);
		
		tBasChn.SetEncAbility(1,emXgaBP);
		tBasChn.SetEncAbility(1,emh263Plus);	
		//tBasChn.SetEncAbility(2,emXgaBP);
		
		tBasChn.SetDecAbility(emDecHP);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 2;
		break;

	case em8KIVidBasChn2:
		tBasChn.m_emChnType = em8KIVidBasChn2;
		tBasChn.SetEncAbility(0,emOther);
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);		

		tBasChn.SetDecAbility(emDecHP);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 1;
		break;

	case em8KIAudBasChn:
		tBasChn.m_emChnType = em8KIAudBasChn;
		tBasChn.m_byBasMediaType = MODE_AUDIO;
		tBasChn.m_byEncNum = MAXNUM_8KIAUDBAS_OUTPUT;
		break;

	default:
		LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::GetBasChnAbilityByBasChnType]unexpected Bas Chn type:%d",byBasChnType);
		break;
	}

	return tBasChn;
}

/*=============================================================================
�� �� ���� IsSupportRollBack
��    �ܣ�  �Ƿ�֧�ֻع�����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8  *pabyChnType           ��¼����ͨ�������͵�����
		   u8 byChnNum				  ͨ������			
		   TConfInfoEx *aptConfInfoEx ��¼���л��������Ϣ������
		   u8 byConfNum				   �������
�� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
2012/04/18  4.7			��־��                 ����
=============================================================================*/
//TODO:Ԥռ�������޸�
BOOL32 CNPlusInst::IsBasSupportRollBack(TBasChnAbility *patBasChnAbility,u8 byChnNum)
{
	if ( NULL == patBasChnAbility )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[IsBasSupportRollBack]pabyChnType is NULL!\n");
		return FALSE;
	}

	if ( byChnNum > MAXNUM_MCU_BAS_CHN )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[IsBasSupportRollBack]byChnNum=%d more than maxvalue!\n",byChnNum);
		return FALSE;
	}
	
	//1.���BASͨ��
	CNPlusBasChnListMgr cChnListMgr;
	COldBasChn	acOldBasChn[MAXNUM_MCU_BAS_CHN];
	CMpu2BasChn acMpu2BasChn[MAXNUM_MCU_BAS_CHN];
	CApu2BasChn acApu2BasChn[MAXNUM_MCU_BAS_CHN];
	C8KIAudBasChn  ac8KIAudBasChn[MAXNUM_MCU_BAS_CHN];
	u8		byRealChnNum = 0;
	BOOL32	bIsChnFrom8KH = FALSE;
	BOOL32  bIsChnFrom8KG = FALSE;
	BOOL32  bIsChnFrom8KI = FALSE;
	for ( u8 byIdx = 0; byIdx<byChnNum; byIdx++ )
	{
		switch (patBasChnAbility[byIdx].m_emChnType)
		{
		case emVpuChn:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(ADAPT_TYPE_AUD,TRUE,TRUE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emVpuChn\n");
			}
			break;
		case emMAUNormal:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(TYPE_MAU_NORMAL,TRUE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emMAUNormal\n");
			}
			break;
		case emMAUH263PlusChn:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(TYPE_MAU_H263PLUS,TRUE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emMAUH263PlusChn\n");
			}
			break;
		case em8KGBrdChn:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(BAS_8KECHN_MV,FALSE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				bIsChnFrom8KG = TRUE;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:em8KGBrdChn\n");
			}
			break;
		case em8KGDSChn:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(BAS_8KECHN_DS,FALSE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				bIsChnFrom8KG = TRUE;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:em8KGDSChn\n");
			}
			break;
		case em8KGSelChn:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(BAS_8KECHN_SEL,FALSE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				bIsChnFrom8KG = TRUE;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:em8KGSelChn\n");
			}
			break;
		case em8KHBrdChn:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(BAS_8KHCHN_MV,FALSE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				bIsChnFrom8KH = TRUE;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:em8KHBrdChn\n");
			}
			break;
		case em8KHDSChn:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(BAS_8KHCHN_DS,FALSE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				bIsChnFrom8KH = TRUE;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:em8KHDSChn\n");
			}
			break;
		case em8KHSelChn:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(BAS_8KHCHN_SEL,FALSE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;	
				bIsChnFrom8KH = TRUE;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:em8KHSelChn\n");
			}
			break;
		case em8KIVidBasChn2:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(BAS_8KICHN2,FALSE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;	
				bIsChnFrom8KI = TRUE;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:em8KIVidBasChn2\n");
			}
			break;
		case em8KIVidBasChn1:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(BAS_8KICHN1,FALSE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;	
				bIsChnFrom8KI = TRUE;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:em8KIVidBasChn1\n");
			}
			break;
		case em8KIVidBasChn0:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(BAS_8KICHN0,FALSE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;	
				bIsChnFrom8KI = TRUE;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:em8KIVidBasChn0\n");
			}
			break;
		case emMPU_H60Chn:
		case emMPU_HChn:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(TYPE_MPU_H,TRUE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emMPU_H60Chn/emMPU_HChn\n");
			}
			break;
		case emMPUChn:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(TYPE_MPU,TRUE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emMPUChn\n");
			}
			break;
		case emMPU2BasicBrdChn:
			{
				acMpu2BasChn[byRealChnNum].SetMpu2BasAttr(0,TYPE_MPU2_BASIC);
				acMpu2BasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acMpu2BasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emMPU2BasicBrdChn\n");
			}
			break;
		case emMPU2BasicDsChn:
			{
				acMpu2BasChn[byRealChnNum].SetMpu2BasAttr(1,TYPE_MPU2_BASIC);
				acMpu2BasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acMpu2BasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emMPU2BasicDsChn\n");
			}
			break;
		case emMPU2BasicSelChn:
			{
				acMpu2BasChn[byRealChnNum].SetMpu2BasAttr(2,TYPE_MPU2_BASIC);
				acMpu2BasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acMpu2BasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emMPU2BasicSelChn\n");
			}
			break;
		case emMPU2EnhancedBrdChn:
			{
				acMpu2BasChn[byRealChnNum].SetMpu2BasAttr(0,TYPE_MPU2_ENHANCED);
				acMpu2BasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acMpu2BasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emMPU2EnhancedBrdChn\n");
			}
			break;
		case emMPU2EnhancedDsChn:
			{
				acMpu2BasChn[byRealChnNum].SetMpu2BasAttr(1,TYPE_MPU2_ENHANCED);
				acMpu2BasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acMpu2BasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emMPU2EnhancedDsChn\n");
			}
			break;
		case emMPU2EnhancedSelChn:
			{
				acMpu2BasChn[byRealChnNum].SetMpu2BasAttr(2,TYPE_MPU2_ENHANCED);
				acMpu2BasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acMpu2BasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;	
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emMPU2EnhancedSelChn\n");
			}
			break;
		case emAPU2BasChn:
			{
				acApu2BasChn[byRealChnNum].SetApu2BasAttr();
				acApu2BasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acApu2BasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emApu2BasChn\n");
			}
			break;
		case em8KIAudBasChn:
			{
				ac8KIAudBasChn[byRealChnNum].Set8KIAudBasAttr();
				ac8KIAudBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&ac8KIAudBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:em8KIAudBasChn\n");
			}
			break;

		default:
			{
				LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[IsBasSupportRollBack]pabyChnType[byIdx]:%d-%d is illegal!\n",patBasChnAbility[byIdx].m_emChnType,byIdx);
				return FALSE;
			}
		}
	}
	
	//ͨ������
	cChnListMgr.SetRealChnNum(byRealChnNum);
	cChnListMgr.SortBasChn();
	LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]byRealChnNum:%d\n",byRealChnNum);

	//2.��ʼԤռ
	TNeedVidAdaptData atReqResource[emMODE_END];	//������Ƶ��˫��
	TNeedAudAdaptData tReqAudResource;				//��Ƶ��������
	TConfInfoEx tTempConfEx;
	u16 wLength = 0;
	BOOL32 bExistUnknownType = FALSE;
	for ( u8 byConfIdx = 0; byConfIdx < MAXNUM_ONGO_CONF; byConfIdx++ )
	{
		if ( m_atConfData[byConfIdx].IsNull() )
		{
			continue;
		}

		if( !m_atConfData[byConfIdx].m_tConf.GetConfAttrb().IsUseAdapter() )
		{
			continue;
		}

		TConfInfoEx tConfAttrbExInfo = GetConfInfoExFromConfAttrb(m_atConfData[byConfIdx].m_tConf);

		if(!m_atConfData[byConfIdx].m_tConf.HasConfExInfo())
		{
			tTempConfEx = tConfAttrbExInfo;
		}
		else
		{
			UnPackConfInfoEx(tTempConfEx,m_atConfExData[byConfIdx].m_byConInfoExBuf,wLength,bExistUnknownType);
			TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];
			u8   byAudioCapNum = tTempConfEx.GetAudioTypeDesc(atAudioTypeDesc);
			if (byAudioCapNum > 0)//������չ��Ϣ������Ƶ������Ϣ����Ҫ����չ��Ϣ��ȥ����
			{
				tConfAttrbExInfo.SetAudioTypeDesc(atAudioTypeDesc,byAudioCapNum);
			}
			tTempConfEx.AddCapExInfo(tConfAttrbExInfo);
		}
		
		memset( &atReqResource,0,sizeof(atReqResource) );
		memset( &tReqAudResource,0,sizeof(tReqAudResource) );

		if ( !GetBasCapConfNeedForNPlus(m_atConfData[byConfIdx].m_tConf, tTempConfEx, atReqResource,&tReqAudResource, bIsChnFrom8KH,bIsChnFrom8KG,bIsChnFrom8KI) )
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[IsBasSupportRollBack]GetBasCapConfNeed failed\n");
			return FALSE;
		}

		if ( !cChnListMgr.CheckBasEnoughForReq(atReqResource, &tReqAudResource) )
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[IsBasSupportRollBack]left baschns don't support conf:%s\n",m_atConfData[byConfIdx].m_tConf.GetConfName());
			return FALSE;
		}
	}

	return TRUE;
}

/*====================================================================
    ������      : TranslateEncodingForm
    ����        : NPlus�ڲ�ת��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    ����ֵ˵��  :
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2013/05/08              liaokang      ����
====================================================================*/
void CNPlusInst::TranslateEncodingForm( void )
{
#ifdef _UTF8
    if( emenCoding_GBK != m_emMcuEncodingForm )
    {
        return;
    }

    u16 wLoop = 0;
    //s8 achTemp[2*MAX_CHARLENGTH] = {0};
    //1��ת�û�����Ϣ m_cUsrGrpsInfo
    TransEncodingOfUsrGrps(m_cUsrGrpsInfo, TRANSENCODING_GBK_UTF8);

    //2��ת�û���Ϣ m_acUsrInfo[MAXNUM_USRNUM]
    for ( wLoop = 0; wLoop < MAXNUM_USRNUM; wLoop++)
    {
        TransEncodingOfUsrInfo(m_acUsrInfo[wLoop], TRANSENCODING_GBK_UTF8);
    }

    for ( wLoop = 0; wLoop < MAXNUM_ONGO_CONF; wLoop++)
    {  
        //3��תmcu�ϻ�����Ϣm_atConfData[MAXNUM_ONGO_CONF]
        TransEncodingOfNPlusConfData(m_atConfData[wLoop], TRANSENCODING_GBK_UTF8);

        //4��������չ��Ϣm_atConfExData[MAXNUM_ONGO_CONF]����Ҫת��
        //4.1 m_byConInfoExBuf����Ҫת��
        //4.2 m_atSmcuCallInfo.m_achAliasΪ164����������Ҫת��
    }

    //5��m_tRASInfo����Ҫת�룬�������ͨ��Э��ӿ�ȡ���ģ�����ucs2
    //6��m_atChargeInfo[MAXNUM_ONGO_CONF]����Ҫת�룬û���õ��������Ϣ�ڻ�����Ϣ����
    //7��m_atNplusParam[MAX_PRIEQP_NUM]����Ҫת��

#endif
}

/*=============================================================================
�� �� ���� InsertBasChn
��    �ܣ� ���ͨ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CBasChn *pBasChn,
		   u8 byIdx
�� �� ֵ�� void 
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
2012/04/18  4.7			��־��                 ����
=============================================================================*/
void CNPlusBasChnListMgr::InsertBasChn(CBasChn *pBasChn,u8 byIdx)
{
	if ( NULL == pBasChn )
	{
		return;
	}
	m_apcBasChn[byIdx] = pBasChn; 	
}

/*=============================================================================
�� �� ���� SortBasChn
��    �ܣ� ��ͨ����������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����
�� �� ֵ�� void 
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
2012/04/18  4.7			��־��                 ����
=============================================================================*/
void CNPlusBasChnListMgr::SortBasChn()
{
	//��BASͨ������������С������������
	//��������
	CBasChn *pcTempBasChn;
	for( u8 bySortIdx = 0; bySortIdx < m_byRealBasChNum - 1; bySortIdx++ )
	{
		for(u8 byInnerIdx = 0; byInnerIdx <m_byRealBasChNum - bySortIdx -1; byInnerIdx ++)
		{
			if( m_apcBasChn[byInnerIdx]->GetMaxDecAbility() > m_apcBasChn[byInnerIdx+1]->GetMaxDecAbility() )
			{	
				pcTempBasChn= m_apcBasChn[byInnerIdx];
				m_apcBasChn[byInnerIdx] = m_apcBasChn[byInnerIdx+1];
				m_apcBasChn[byInnerIdx+1] = pcTempBasChn;
			}
			else if ( m_apcBasChn[byInnerIdx]->GetMaxDecAbility() == m_apcBasChn[byInnerIdx+1]->GetMaxDecAbility() )
			{
				if ( m_apcBasChn[byInnerIdx]->GetMaxEncAbility() > m_apcBasChn[byInnerIdx+1]->GetMaxEncAbility() )
				{
					pcTempBasChn= m_apcBasChn[byInnerIdx];
					m_apcBasChn[byInnerIdx] = m_apcBasChn[byInnerIdx+1];
					m_apcBasChn[byInnerIdx+1] = pcTempBasChn;
				}
			}
		}
	}
	
}

/*=============================================================================
�� �� ���� GetOneIdleSuitableBasChn
��    �ܣ� ���һ���ʺϵ�ͨ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����TNeedBasResData &tLeftReqResData ʣ���������

�� �� ֵ�� CBasChn* ���ػ��ͨ�� 
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
2012/04/18  4.7			��־��                 ����
=============================================================================*/
CBasChn* CNPlusBasChnListMgr::GetOneIdleSuitableBasChn(TNeedVidAdaptData &tLeftReqResData)
{	
	CBasChn *pSubSuitableChn = NULL;		//��¼���ŵ�BASͨ��
	TNeedVidAdaptData tTempLeftData;				//��¼����ͨ��ʹ�ú�ʣ������
	TNeedVidAdaptData tTempNeedData;				//��ʱ����BAS������Դ����
	memcpy( &tTempNeedData, &tLeftReqResData, sizeof(TNeedVidAdaptData) );
	
	CBasChn *pTmpBasChn = NULL;
	TVideoStreamCap atSimCap[MAX_CONF_BAS_ADAPT_NUM]; 
	u8 byOccupiedNum=0;
	for ( u8 byIdx = 0; byIdx < m_byRealBasChNum; byIdx++ )
	{
		pTmpBasChn = m_apcBasChn[byIdx];
		if ( NULL == pTmpBasChn )
		{
			continue;
		}

		if ( pTmpBasChn->IsBasChnSupportNeeds(tLeftReqResData,atSimCap) )
		{	
			//������ռȥ�ı���·������ptOccpiedResData�����Ԫ�ؽ��г�ʼ��
			byOccupiedNum = tTempNeedData.m_byRealEncNum - tLeftReqResData.m_byRealEncNum;
			if ( byOccupiedNum>0 && byOccupiedNum < MAX_CONF_BAS_ADAPT_NUM )
			{
				for ( u8 byLoop =byOccupiedNum; byLoop<MAX_CONF_BAS_ADAPT_NUM; byLoop++ )
				{
					atSimCap[byLoop].Clear();
				}
			}
			//���������ŵ�ͨ�����Ҳ����ȼ�¼��ͨ�����Ա��Ҳ���ʱ����ʹ�ô���ͨ��
			if (tLeftReqResData.m_byRealEncNum ==0 )
			{
				pTmpBasChn->SetIsReserved(TRUE);
				pTmpBasChn->SetMediaMode(tLeftReqResData.m_byMediaMode);
				return pTmpBasChn;		//��������ͨ��
			}
			else
			{
				pSubSuitableChn = pTmpBasChn;	//��¼����ͨ��
				memcpy( &tTempLeftData,&tLeftReqResData,   sizeof(TNeedVidAdaptData) );	//��¼����ʣ������
				memcpy( &tLeftReqResData, &tTempNeedData, sizeof(TNeedVidAdaptData) );	//�ָ����ݼ�������
			}
			
		}
	}

	//�ж��Ƿ��д���ͨ����ʹ��
	if ( pSubSuitableChn != NULL)
	{
		pSubSuitableChn->SetIsReserved(TRUE);		   //Ԥռ
		pSubSuitableChn->SetMediaMode(tLeftReqResData.m_byMediaMode);
		memcpy( &tLeftReqResData, &tTempLeftData, sizeof(TNeedVidAdaptData) );	//����ʣ������
		return pSubSuitableChn;			//����ͨ��
	}
	
	memcpy( &tLeftReqResData, &tTempNeedData, sizeof(TNeedVidAdaptData) );	//�ָ�����
	return NULL;
}

/*=============================================================================
�� �� ���� GetOneIdelSuitableAudBasChn
��    �ܣ� ���һ���ʺϵ�ͨ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����TNeedAudAdaptData &tLeftReqResData ʣ���������

�� �� ֵ�� CBasChn* ���ػ��ͨ�� 
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
2013/03/29  4.7			��־��                 ����
=============================================================================*/
CBasChn* CNPlusBasChnListMgr::GetOneIdelSuitableAudBasChn(TNeedAudAdaptData &tLeftReqResData)
{	
	CBasChn *pSubSuitableChn = NULL;		//��¼���ŵ�BASͨ��
	TNeedAudAdaptData tTempLeftData;				//��¼����ͨ��ʹ�ú�ʣ������
	TNeedAudAdaptData tTempNeedData;				//��ʱ����BAS������Դ����
	memcpy( &tTempNeedData, &tLeftReqResData, sizeof(TNeedAudAdaptData) );
	
	CBasChn *pTmpBasChn = NULL;
	TAudioTypeDesc atSimCap[MAXNUM_CONF_AUDIOTYPE];  
	for ( u8 byIdx = 0; byIdx < m_byRealBasChNum; byIdx++ )
	{
		pTmpBasChn = m_apcBasChn[byIdx];
		if ( NULL == pTmpBasChn )
		{
			continue;
		}

		if ( pTmpBasChn->IsBasChnSupportAudNeeds(tLeftReqResData,atSimCap,FALSE) )
		{	
			//���������ŵ�ͨ�����Ҳ����ȼ�¼��ͨ�����Ա��Ҳ���ʱ����ʹ�ô���ͨ��
			if (tLeftReqResData.m_byNeedAudEncNum ==0 )
			{
				pTmpBasChn->SetIsReserved(TRUE);
				pTmpBasChn->SetMediaMode(MODE_AUDIO);
				return pTmpBasChn;		//��������ͨ��
			}
			else
			{
				pSubSuitableChn = pTmpBasChn;	//��¼����ͨ��
				memcpy( &tTempLeftData,&tLeftReqResData,   sizeof(TNeedAudAdaptData) );	//��¼����ʣ������
				memcpy( &tLeftReqResData, &tTempNeedData, sizeof(TNeedAudAdaptData) );	//�ָ����ݼ�������
			}
			
		}
	}

	//�ж��Ƿ��д���ͨ����ʹ��
	if ( pSubSuitableChn != NULL)
	{
		pSubSuitableChn->SetIsReserved(TRUE);		   //Ԥռ
		pSubSuitableChn->SetMediaMode(MODE_AUDIO);
		memcpy( &tLeftReqResData, &tTempLeftData, sizeof(TNeedAudAdaptData) );	//����ʣ������
		return pSubSuitableChn;			//����ͨ��
	}
	
	memcpy( &tLeftReqResData, &tTempNeedData, sizeof(TNeedAudAdaptData) );	//�ָ�����
	return NULL;
}

/*=============================================================================
�� �� ���� GetBasChnsForBrd
��    �ܣ� ���ͨ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����TNeedBasResData &tLeftReqResData ������������������,��Ƶ����Ƶ��˫��

�� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
2012/04/18  4.7			��־��                 ����
=============================================================================*/
BOOL32 CNPlusBasChnListMgr::GetBasChnsForBrd(TNeedVidAdaptData *ptReqResData)
{
	//����·��Ϊ0��˵������Ҫ����
	if ( ptReqResData->m_byRealEncNum == 0 )
	{
		return TRUE;
	}
	
	if ( NULL == ptReqResData )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[CNPlusBasChnListMgr::GetBasChnsForBrd]ptReqResData is NULL\n");
		return FALSE;
	}
	
	TNeedVidAdaptData	tTempLeftNeedData;
	memcpy( &tTempLeftNeedData, ptReqResData, sizeof(TNeedVidAdaptData) );
	CBasChn *pCBasChn = NULL;	
	while(tTempLeftNeedData.m_byRealEncNum)
	{
		pCBasChn = GetOneIdleSuitableBasChn(tTempLeftNeedData);
		if ( NULL == pCBasChn )
		{
			return FALSE;
		}
	}
	return TRUE;
}

/*=============================================================================
�� �� ���� GetAudBasChnsForBrd
��    �ܣ� ���ͨ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����TNeedAudAdaptData &ptReqAudResource ������������������,��Ƶ

�� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
2013/03/29  4.7			��־��                 ����
=============================================================================*/
BOOL32 CNPlusBasChnListMgr::GetAudBasChnsForBrd(TNeedAudAdaptData *ptReqAudResource)
{
	if ( NULL == ptReqAudResource )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[CNPlusBasChnListMgr::GetBasChnsForBrd]ptReqAudResource is NULL\n");
		return FALSE;
	}

	//����·��Ϊ0��˵������Ҫ����
	if ( ptReqAudResource->m_byNeedAudEncNum == 0 )
	{
		return TRUE;
	}
	

	
	TNeedAudAdaptData	tTempLeftNeedData;
	memcpy( &tTempLeftNeedData, ptReqAudResource, sizeof(TNeedAudAdaptData) );
	CBasChn *pCBasChn = NULL;	
	while(tTempLeftNeedData.m_byNeedAudEncNum)
	{
		pCBasChn = GetOneIdelSuitableAudBasChn(tTempLeftNeedData);
		if ( NULL == pCBasChn )
		{
			return FALSE;
		}
	}
	return TRUE;
}

/*=============================================================================
�� �� ���� CheckBasEnoughForReq
��    �ܣ� ����Ƿ����㹻��ͨ���ٿ�����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����TNeedBasResData &tLeftReqResData ������������������,��Ƶ����Ƶ��˫��

�� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
2012/04/18  4.7			��־��                 ����
=============================================================================*/
BOOL32 CNPlusBasChnListMgr::CheckBasEnoughForReq(TNeedVidAdaptData *ptReqResData,TNeedAudAdaptData *ptReqAudResource)
{
	if ( NULL == ptReqResData || NULL == ptReqAudResource)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[CNPlusBasChnListMgr::CheckBasEnoughForReq]ptReqResData or pReqAudResource is NULL\n");
		return FALSE;
	}

	BOOL32	bIsOccupyOk = FALSE;
	//��Ԥռ��Ƶ
	if ( GetAudBasChnsForBrd(ptReqAudResource) )
	{
		bIsOccupyOk = TRUE;
	}
	else
	{
		return FALSE;
	}

	//��Ԥռ��Ƶ
	u8		byOccupyNum = 0;		//��¼�Ѿ�Ԥռ��ͨ������
	for ( u8 byIdx = 0; byIdx < emMODE_END; byIdx++ )
	{
		u8 byTempNum = 0;
		if ( GetBasChnsForBrd( &ptReqResData[byIdx] ) 
			)
		{ 
			bIsOccupyOk = TRUE;
		}
		else
		{
			bIsOccupyOk = FALSE;
			break;
		}
	}
	
	
	//Ԥռ���ɹ�
	if (!bIsOccupyOk)
	{
		return FALSE;
	}

	return TRUE;
}

/*====================================================================
������        GetBasCapConfNeed
����        ����ȡ��������������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����[in]const TConfInfo &tConfInfo		  ������Ϣ
			  [in]const TConfInfoEx &tConfInfoEx,     ���鹴ѡ������Ϣ
			  [out]TNeedBasResData *ptMVReqResource   ���ر������Ϣ
			  [in] u8  byMediaNum					  ý�����͸���							
			  
����ֵ˵��  ���ɹ���ñ������Ϣ����TRUE,ʧ�ܷ���FALSE
					
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2011/12/26  4.7			��־��		   ����
====================================================================*/
BOOL32 GetBasCapConfNeedForNPlus(const TConfInfo &tConfInfo,const TConfInfoEx &tConfInfoEx, 
									 TNeedVidAdaptData *ptReqResource, TNeedAudAdaptData *pReqAudResource, 
									 BOOL32 bIsChn8KH/* = FALSE*/,BOOL32 bIsChn8KE/* = FALSE*/,
									 BOOL32 bIsChn8KI/* = FALSE*/)
{	
	if ( ptReqResource == NULL || pReqAudResource == NULL )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[GetBasCapConfNeedForNPlus]ptReqResource or pReqAudResource is NULL!\n");
		return FALSE;
	}

	//1.��û����������(���������Ƶ��˫������������������Ƶ�������������)
	u8 byIdx = 0;
	for ( byIdx = emMODE_VIDEO; byIdx < (emMODE_END); byIdx++ )
	{
		if ( byIdx == emMODE_VIDEO && tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_H261 )
		{
			ptReqResource[byIdx].m_byDecNeedRes = (u8)1 << CBasChn::emDecH261;
		}
		else if (tConfInfo.GetProfileAttrb() == emHpAttrb)
		{
			//8kh˫��ͨ��֧�ֽ�HP������֧�ֽ��֡�ʣ������������HP�����⴦����¼��������emDecHighFps
			if ( bIsChn8KH )
			{
				if ( byIdx == emMODE_SECVIDEO && IsDSFR50OR60(tConfInfo))
				{
					ptReqResource[byIdx].m_byDecNeedRes = (u8)1 <<CBasChn::emDecHighFps;
				}
				else
				{
					ptReqResource[byIdx].m_byDecNeedRes = (u8)1 <<CBasChn::emDecHp;
				}
			}
			else
			{
				ptReqResource[byIdx].m_byDecNeedRes = (u8)1 <<CBasChn::emDecHp;
			}
		}
		else if ( tConfInfo.GetProfileAttrb() == emBpAttrb )
		{
			BOOL32 bHighFps = FALSE;
			if ( byIdx == emMODE_VIDEO )
			{
				bHighFps= IsConfFR50OR60(tConfInfo);
			}
			else if ( byIdx == emMODE_SECVIDEO )
			{
				bHighFps= IsDSFR50OR60(tConfInfo);
			}

			BOOL32 bMainMediaH264   =  tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_H264;
			BOOL32 bSecMediaH264	=  tConfInfo.GetSecVideoMediaType() != MEDIA_TYPE_NULL &&
									   tConfInfo.GetSecVideoMediaType() == MEDIA_TYPE_H264;
			if(bHighFps)
			{
				//˫���Ľ��������������⴦��Ϊ�˼�����ǰ�Ĺ��
				//��ǰ����֡�ʻ��� ˫��ͬ����Ƶ ����Ϊ��Ҫhighfps����������ͨ���������������ֻռhd���������ļ���
				//���ڣ�����û��ͬ����Ƶ�����䣬����ΪֻҪ˫������ʽ����50/60�Ļ��������������޸ĳ�hd
				if ( byIdx ==emMODE_SECVIDEO &&
					tConfInfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetUserDefFrameRate() <= 30)
				{
					ptReqResource[byIdx].m_byDecNeedRes= (u8)1 <<CBasChn::emDecHD;
				}
				else
				{
					ptReqResource[byIdx].m_byDecNeedRes= (u8)1 <<CBasChn::emDecHighFps;
				}
			}
			else if((IsResGE(tConfInfo.GetMainVideoFormat(), VIDEO_FORMAT_4CIF) && bMainMediaH264) ||
				(IsResGE(tConfInfo.GetSecVideoFormat(), VIDEO_FORMAT_4CIF) && bSecMediaH264))
			{
				ptReqResource[byIdx].m_byDecNeedRes = (u8)1 <<CBasChn::emDecHD;
			}
			else
			{
				ptReqResource[byIdx].m_byDecNeedRes = (u8)1 <<CBasChn::emDecSD;
			}
		}
		else
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[GetBasCapConfNeedForNPlus]tConfInfo.GetProfileAttrb() failed!\n");
			return FALSE;
		}
	}

	//2. ���������Ƶ�����������
	u8	byStartIdx = 0;		//���ñ���idx����ʼλ�ü�¼
	ptReqResource[emMODE_VIDEO].m_byMediaMode = MODE_VIDEO;

	//Ԥ������ʽ
	if ( IsConfNeedReserveMainCap(tConfInfo,tConfInfoEx) )
	{
		//8000H ����ʽ1080 60�������������ƣ���Ҫ��֡�ʼ�������
		TVideoStreamCap tTempVideoCap = tConfInfo.GetMainSimCapSet().GetVideoCap();
		if (bIsChn8KH)
		{
			if ( tConfInfo.GetProfileAttrb() == emHpAttrb )
			{	
				// HP�Ļ������� 720 30 HP
				tTempVideoCap.SetResolution(VIDEO_FORMAT_HD720);
				tTempVideoCap.SetH264ProfileAttrb(emHpAttrb);
				if (  tTempVideoCap.GetUserDefFrameRate() == 60 ||
					tTempVideoCap.GetUserDefFrameRate() == 50
					)
				{
					tTempVideoCap.SetUserDefFrameRate( tTempVideoCap.GetUserDefFrameRate()/2 );
				}				
			}
			ptReqResource[emMODE_VIDEO].SetVieoSimCapSet( byStartIdx, tTempVideoCap );
		}
		else if( bIsChn8KI )
		{
			if ( tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
				tConfInfo.GetMainVideoFormat() == VIDEO_FORMAT_HD1080 &&
				(tConfInfo.GetMainVidUsrDefFPS() > 30 ) 
				)
			{
				tTempVideoCap.SetUserDefFrameRate( tTempVideoCap.GetUserDefFrameRate()/2 );
			
			}			
			ptReqResource[emMODE_VIDEO].SetVieoSimCapSet( byStartIdx, tTempVideoCap );						
		}
		else
		{
			ptReqResource[emMODE_VIDEO].SetVieoSimCapSet( byStartIdx, tTempVideoCap );
		}	
		byStartIdx++;
		LogPrint(LOG_LVL_DETAIL,MID_MCU_NPLUS,"[GetBasCapConfNeedForNPlus]reserve MainVideoCap:mediatype:%d-res:%d-profile:%d-userframe:%d-biterate:%d !\n",
				tTempVideoCap.GetMediaType(), 
				tTempVideoCap.GetResolution(),
				tTempVideoCap.GetH264ProfileAttrb(),
				tTempVideoCap.GetUserDefFrameRate(),
				tTempVideoCap.GetMaxBitRate()
					);	
	}

	//�������ģ�年ѡ
	TVideoStreamCap tVideoCap[MAX_CONF_CAP_EX_NUM];
	u8 byCapNUm = MAX_CONF_CAP_EX_NUM;
	if ( tConfInfoEx.GetMainStreamCapEx(tVideoCap,byCapNUm) )
	{
		for ( byIdx = 0; byIdx<byCapNUm; byIdx++)
		{
			if ( tVideoCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL )
			{
				continue;
			}
			//8000H�����������ƣ������ѡ����720 60�Ļ�����Ҫ��֡�ʼ���
			if (bIsChn8KH)
			{
				//��ѡ��HP���˵�����Ϊ����ʽԤ���߼����Ѿ�ǿ��Ԥ��720 30 HP,�˴�����Ԥ��
				if ( tVideoCap[byIdx].GetH264ProfileAttrb() == emHpAttrb )
				{
					continue;
				}
			}
			else if( bIsChn8KI )
			{
				if ( tVideoCap[byIdx].GetMediaType()== MEDIA_TYPE_H264 &&
					( tVideoCap[byIdx].GetResolution() == VIDEO_FORMAT_HD1080 || 
						tVideoCap[byIdx].GetResolution() == VIDEO_FORMAT_HD720 ) &&
					tVideoCap[byIdx].GetUserDefFrameRate() > 30 
					)
				{
					tVideoCap[byIdx].SetUserDefFrameRate( tVideoCap[byIdx].GetUserDefFrameRate()/2 );					
				}
				
				ptReqResource[emMODE_VIDEO].SetVieoSimCapSet( byStartIdx,tVideoCap[byIdx] );				
			}			
			else
			{
				//���ù�ѡ����
				ptReqResource[emMODE_VIDEO].SetVieoSimCapSet(byStartIdx,tVideoCap[byIdx]);
			}
			byStartIdx++;	
			LogPrint(LOG_LVL_DETAIL,MID_MCU_NPLUS,"[GetBasCapConfNeedForNPlus]reserve video cap:mediatype:%d-res:%d-profile:%d-userframe:%d-biterate:%d !\n",
				tVideoCap[byIdx].GetMediaType(), 
				tVideoCap[byIdx].GetResolution(),
				tVideoCap[byIdx].GetH264ProfileAttrb(),
				tVideoCap[byIdx].GetUserDefFrameRate(),
				tVideoCap[byIdx].GetMaxBitRate()
					);	
		}
	}
	else
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_BAS,"[GetBasCapConfNeedForNPlus]tConfInfoEx.GetMainStreamCapEx failed!\n");
		return FALSE;
	}

	//��¼����·��
	ptReqResource[emMODE_VIDEO].m_byRealEncNum = byStartIdx;

	//���Ԥ������ʽ	
	if ( tConfInfo.GetSecVideoMediaType()!= MEDIA_TYPE_NULL )
	{
		ptReqResource[emMODE_VIDEO].SetVieoSimCapSet( byStartIdx, tConfInfo.GetSecSimCapSet().GetVideoCap() );
		byStartIdx++;
		ptReqResource[emMODE_VIDEO].m_byRealEncNum = byStartIdx;
		LogPrint(LOG_LVL_DETAIL,MID_MCU_NPLUS,"[GetBasCapConfNeedForNPlus]reserve SecVideoCap:mediatype:%d-res:%d-profile:%d-userframe:%d-biterate:%d !\n",
			tConfInfo.GetSecSimCapSet().GetVideoMediaType(), 
			tConfInfo.GetSecSimCapSet().GetVideoResolution(),
			tConfInfo.GetSecSimCapSet().GetVideoProfileType(),
			tConfInfo.GetSecSimCapSet().GetUserDefFrameRate(),
			tConfInfo.GetSecSimCapSet().GetVideoMaxBitRate()
			);	
	}

	//���ͬʱ����720 30fps ��720 60fps ��ȥ����·720 60fps��idx
	if( !bIsChn8KI )
	{
		if ( bIsChn8KH )
		{
			ptReqResource[emMODE_VIDEO].Filter720P60FPSOnlyFor8KH();
		}
		else
		{
			ptReqResource[emMODE_VIDEO].Filter720P60FPS();
		}
	}	
	
	//3.���˫�������������
	byStartIdx = 0;
	ptReqResource[emMODE_SECVIDEO].m_byMediaMode = MODE_SECVIDEO;
	//Ԥ������ʽ
	if ( IsConfNeedReserveDSMainCap(tConfInfo,tConfInfoEx) )
	{
		if ( IsConfDualEqMV(tConfInfo) )//ͬ����Ƶ˫�ٻ���Ԥ����������ʽ
		{
			ptReqResource[emMODE_SECVIDEO].SetVieoSimCapSet(byStartIdx, tConfInfo.GetMainSimCapSet().GetVideoCap());	
			if ( bIsChn8KE || bIsChn8KH  )
			{
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetResolution(VIDEO_FORMAT_CIF);
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetUserDefFrameRate(25);
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetH264ProfileAttrb(emBpAttrb);
			}
			
			if( bIsChn8KI )
			{
				if( ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetResolution() >= VIDEO_FORMAT_HD720 )
				{
					u8 byFrameRate = ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetUserDefFrameRate();
					if( byFrameRate >= 50 )
					{
						byFrameRate = byFrameRate / 2;
					}
					ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetResolution(VIDEO_FORMAT_HD720);
					ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetUserDefFrameRate(byFrameRate);
					ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetH264ProfileAttrb(emBpAttrb);
				}				
			}

			LogPrint(LOG_LVL_DETAIL,MID_MCU_BAS,"[GetBasCapConfNeed]reserve DSDualMainCap:mediatype:%d-res:%d-profile:%d-userframe:%d-biterate:%d !\n",
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetMediaType(),
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetResolution(),
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetH264ProfileAttrb(),
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetUserDefFrameRate(),
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetMaxBitRate()
				);
			byStartIdx++;
		}
		else
		{
			ptReqResource[emMODE_SECVIDEO].SetVieoSimCapSet(byStartIdx, tConfInfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap());
			if ( ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetResolution() == VIDEO_FORMAT_VGA ||
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetResolution() == VIDEO_FORMAT_SVGA
				)
			{
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetResolution(VIDEO_FORMAT_XGA);
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetUserDefFrameRate(5);
			}
			
			if ( bIsChn8KE || bIsChn8KH )
			{
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetResolution(VIDEO_FORMAT_XGA);
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetUserDefFrameRate(5);
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetH264ProfileAttrb(emBpAttrb);
			}

			if ( bIsChn8KI )
			{
				if( ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetMediaType() == MEDIA_TYPE_H264 )
				{
					emProfileAttrb emProfile = emHpAttrb;
					if( IsConfHasBpCapOrBpExCap(tConfInfo,tConfInfoEx) )
					{
						emProfile = emBpAttrb;
					}
					
					if( Is8KINeedDsAdaptH264SXGA(tConfInfo) )
					{
						ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetResolution(VIDEO_FORMAT_SXGA);
						ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetUserDefFrameRate(20);					
						ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetH264ProfileAttrb(emProfile);						
					}
					else
					{
						if( tConfInfoEx.IsDSSupportCapEx( VIDEO_FORMAT_XGA,5,emBpAttrb ) )
						{						
							ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetResolution(VIDEO_FORMAT_XGA);
							ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetUserDefFrameRate(5);
							ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetH264ProfileAttrb(emBpAttrb);						
						}
						else if(  tConfInfoEx.IsDSSupportCapEx( VIDEO_FORMAT_XGA,5,emHpAttrb ) )
						{
							ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetResolution(VIDEO_FORMAT_XGA);
							ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetUserDefFrameRate(5);
							ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetH264ProfileAttrb(emHpAttrb);
						}
					}		
				}				
			}

			LogPrint(LOG_LVL_DETAIL,MID_MCU_BAS,"[GetBasCapConfNeed]reserve DSMainCap:mediatype:%d-res:%d-profile:%d-userframe:%d-biterate:%d !\n",
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetMediaType(),
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetResolution(),
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetH264ProfileAttrb(),
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetUserDefFrameRate(),
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetMaxBitRate()
				);	
			byStartIdx++;
		}
	}

	//���˫��ģ�年ѡ
	if ( !bIsChn8KE && !bIsChn8KH && !bIsChn8KI )
	{
		TVideoStreamCap tSecVideoCap[MAX_CONF_CAP_EX_NUM];
		u8 byDSCapNum = MAX_CONF_CAP_EX_NUM;
		if ( tConfInfoEx.GetDoubleStreamCapEx(tSecVideoCap,byDSCapNum) )
		{
			for ( byIdx = 0; byIdx<byDSCapNum; byIdx++)
			{
				if ( tSecVideoCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL )
				{
					continue;
				}
				//���ù�ѡ����
				ptReqResource[emMODE_SECVIDEO].SetVieoSimCapSet(byStartIdx,tSecVideoCap[byIdx]);
				byStartIdx++;
				
				LogPrint(LOG_LVL_DETAIL,MID_MCU_NPLUS,"[GetBasCapConfNeedForNPlus]reserve ds video cap:mediatype:%d-res:%d-profile:%d-userframe:%d-biterate:%d !\n",
					tSecVideoCap[byIdx].GetMediaType(), 
					tSecVideoCap[byIdx].GetResolution(),
					tSecVideoCap[byIdx].GetH264ProfileAttrb(),
					tSecVideoCap[byIdx].GetUserDefFrameRate(),
					tSecVideoCap[byIdx].GetMaxBitRate()
					);	
			}
		}
		else
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[GetBasCapConfNeedForNPlus]tConfInfoEx.GetDoubleStreamCapEx failed!\n");
			return FALSE;
		}		
		//��¼����·��
		ptReqResource[emMODE_SECVIDEO].m_byRealEncNum = byStartIdx;
	}

	//˫˫��Ԥ�� ����ʽ
	BOOL32  bDoubleDual= IsConfDoubleDual(tConfInfo);
	if ( bDoubleDual && tConfInfo.GetDStreamUsrDefFPS() >= 5 )
	{
		//����ʽ˫��Ԥ��
		ptReqResource[emMODE_SECVIDEO].SetVieoSimCapSet(byStartIdx,tConfInfo.GetCapSupportEx().GetSecDSVideoCap());
		//H263+ʹ�ú궨�壬��BAS���账����ݺ������ʵ��֡�ʣ�Ŀǰ����5֡,
		ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetFrameRate(VIDEO_FPS_2997_6);
		//H263+ �ֱ��ʴ˴�������XGA
		ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetResolution(VIDEO_FORMAT_XGA);
		byStartIdx++;
		ptReqResource[emMODE_SECVIDEO].m_byRealEncNum = byStartIdx;
		LogPrint(LOG_LVL_DETAIL,MID_MCU_NPLUS,"[GetBasCapConfNeedForNPlus]reserve DoubleDual\n");
	}
	else if( bIsChn8KI )
	{
		if( Is8KINeedDsAdaptH264SXGA(tConfInfo) )
		{
			if( tConfInfoEx.IsDSSupportCapEx( VIDEO_FORMAT_XGA,5,emBpAttrb ) )
			{
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetMediaType( MEDIA_TYPE_H264 );
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetResolution(VIDEO_FORMAT_XGA);
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetUserDefFrameRate(5);
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetH264ProfileAttrb(emBpAttrb);
				byStartIdx++;
				ptReqResource[emMODE_SECVIDEO].m_byRealEncNum = byStartIdx;
			}
			else if( tConfInfoEx.IsDSSupportCapEx( VIDEO_FORMAT_XGA,5,emHpAttrb ) )
			{
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetMediaType( MEDIA_TYPE_H264 );
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetResolution(VIDEO_FORMAT_XGA);
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetUserDefFrameRate(5);
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetH264ProfileAttrb(emHpAttrb);
				byStartIdx++;
				ptReqResource[emMODE_SECVIDEO].m_byRealEncNum = byStartIdx;
			}
		}
	}

	//���ͬʱ����720 30fps ��720 60fps ��ȥ����·720 60fps��idx
	ptReqResource[emMODE_SECVIDEO].Filter720P60FPS();
	
	//4.��Ƶ������
	u8 byAudTypeNum = 0;
	TAudioTypeDesc atAudTypeDesc[MAXNUM_CONF_AUDIOTYPE];
	byAudTypeNum = tConfInfoEx.GetAudioTypeDesc(atAudTypeDesc);
	if ( byAudTypeNum > 1 ) //��Ƶ��������������1��˵����Ҫ��Ƶ����
	{
		memcpy(pReqAudResource->m_atAudTypeDesc,atAudTypeDesc,sizeof(atAudTypeDesc));
		pReqAudResource->m_byNeedAudEncNum = byAudTypeNum-1;		
		for ( u8 byLoop=0 ; byLoop<byAudTypeNum; byLoop++ )
		{
			LogPrint(LOG_LVL_DETAIL,MID_MCU_BAS,"[GetBasCapConfNeedForNPlus]conf all audiotype:mediatype-%d-tracknum-%d\n",atAudTypeDesc[byLoop].GetAudioMediaType(), atAudTypeDesc[byLoop].GetAudioTrackNum() );	
		}
	}
	else
	{
		pReqAudResource->m_byNeedAudEncNum = 0;
	}

	return TRUE;
}

/*====================================================================
    ������      : TransEncodingOfUsrGrps
    ����        : �û��� ת��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    ����ֵ˵��  :
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2013/05/09              liaokang      ����
====================================================================*/
inline void TransEncodingOfUsrGrps(CUsrGrpsInfo &cUsrGrpsInfo, u8 byTransEncodingType)
{
    u16 wLoop = 0;
    s8 achTmp[2*MAX_CHARLENGTH] = {0};     
    switch( byTransEncodingType )
    {
    case TRANSENCODING_UTF8_GBK:
        {
            for ( wLoop = 0; wLoop < MAXNUM_USRGRP; wLoop++)
            {
                if ( cUsrGrpsInfo.m_atInfo[wLoop].IsFree() )
                {
                    continue;
                }
                
                memset(achTmp, 0, sizeof(achTmp));
                utf8_to_gb2312(cUsrGrpsInfo.m_atInfo[wLoop].GetUsrGrpName(), achTmp, MAX_CHARLENGTH-1);
                cUsrGrpsInfo.m_atInfo[wLoop].SetUsrGrpName(achTmp);        
                
                memset(achTmp, 0, sizeof(achTmp));
                utf8_to_gb2312(cUsrGrpsInfo.m_atInfo[wLoop].GetUsrGrpDesc(), achTmp, 2*MAX_CHARLENGTH-1);
                cUsrGrpsInfo.m_atInfo[wLoop].SetUsrGrpDesc(achTmp);
            }
        }
        break;
    case TRANSENCODING_GBK_UTF8:
        {
            for ( wLoop = 0; wLoop < MAXNUM_USRGRP; wLoop++)
            {
                if ( cUsrGrpsInfo.m_atInfo[wLoop].IsFree() )
                {
                    continue;
                }
                
                memset(achTmp, 0, sizeof(achTmp));
                gb2312_to_utf8(cUsrGrpsInfo.m_atInfo[wLoop].GetUsrGrpName(), achTmp, MAX_CHARLENGTH-1);
                cUsrGrpsInfo.m_atInfo[wLoop].SetUsrGrpName(achTmp);        
                
                memset(achTmp, 0, sizeof(achTmp));
                gb2312_to_utf8(cUsrGrpsInfo.m_atInfo[wLoop].GetUsrGrpDesc(), achTmp, 2*MAX_CHARLENGTH-1);
                cUsrGrpsInfo.m_atInfo[wLoop].SetUsrGrpDesc(achTmp);
            }
        }
        break;
    default :
        break;
    } 
}

/*====================================================================
    ������      : TransEncodingOfUsrInfo
    ����        : �û���Ϣ ת��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    ����ֵ˵��  :
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2013/05/09              liaokang      ����
====================================================================*/
inline void TransEncodingOfUsrInfo(CExUsrInfo &cExUsrInfo, u8 byTransEncodingType)
{
    if ( cExUsrInfo.IsEqualName("admin"))
    {
        return; // Mcu��ʹ�õ�ʱ����discription�����ֽڵ� ��ID �ã� [pengguofeng 4/25/2013]
        //����0~127֮���û���⣬���Ƕ���255�����ľͻᱻ���ɷǷ���ĸ����ǿ��ת��
    }

    s8 achTmp[2*MAX_CHARLENGTH] = {0};     
    switch( byTransEncodingType )
    {
    case TRANSENCODING_UTF8_GBK:
        {
            memset(achTmp, 0, sizeof(achTmp));
            utf8_to_gb2312(cExUsrInfo.name, achTmp, MAX_CHARLENGTH-1);
            cExUsrInfo.SetName(achTmp);
            
            memset(achTmp, 0, sizeof(achTmp));
            utf8_to_gb2312(cExUsrInfo.fullname, achTmp, MAX_CHARLENGTH-1);
            cExUsrInfo.SetFullName(achTmp);
            
            memset(achTmp, 0, sizeof(achTmp));
            utf8_to_gb2312(cExUsrInfo.discription, achTmp, 2*MAX_CHARLENGTH-1);
            cExUsrInfo.SetDiscription(achTmp);            
        } 
        break;
    case TRANSENCODING_GBK_UTF8:
        {
            memset(achTmp, 0, sizeof(achTmp));
            gb2312_to_utf8(cExUsrInfo.name, achTmp, MAX_CHARLENGTH-1);
            cExUsrInfo.SetName(achTmp);
            
            memset(achTmp, 0, sizeof(achTmp));
            gb2312_to_utf8(cExUsrInfo.fullname, achTmp, MAX_CHARLENGTH-1);
            cExUsrInfo.SetFullName(achTmp);
            
            memset(achTmp, 0, sizeof(achTmp));
            gb2312_to_utf8(cExUsrInfo.discription, achTmp, 2*MAX_CHARLENGTH-1);
            cExUsrInfo.SetDiscription(achTmp);
        } 
        break;
    default :
        break;
    }
}
/*====================================================================
    ������      : TransEncodingOfNPlusConfData
    ����        : NPlusConfDataת��
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    ����ֵ˵��  :
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2013/05/09              liaokang      ����
====================================================================*/
inline void TransEncodingOfNPlusConfData(TNPlusConfData &tConfData, u8 byTransEncodingType)
{
    if ( tConfData.IsNull() )
    {
        return;
    } 

    u16 wLoop = 0;
    s8 achTmp[MAXLEN_ALIAS+1] = {0};     
    switch( byTransEncodingType )
    {
    case TRANSENCODING_UTF8_GBK:
        {
            //תmcu�ϻ�����Ϣ
            //m_tConf        
            //������m_achConfName[MAXLEN_CONFNAME+1]
            memset(achTmp, 0, sizeof(achTmp));
            utf8_to_gb2312(tConfData.m_tConf.GetConfName(), achTmp, MAXLEN_CONFNAME);
            tConfData.m_tConf.SetConfName(achTmp);        
            //��������m_achConfPwd[MAXLEN_PWD+1]
            memset(achTmp, 0, sizeof(achTmp));
            utf8_to_gb2312(tConfData.m_tConf.GetConfPwd(), achTmp, MAXLEN_PWD);
            tConfData.m_tConf.SetConfPwd(achTmp);
            //��������ϯ�ı���m_tChairAlias
            memset(achTmp, 0, sizeof(achTmp));
            TMtAlias tMtAlias;
            tMtAlias = tConfData.m_tConf.GetChairAlias();
            utf8_to_gb2312(tMtAlias.m_achAlias, achTmp, MAXLEN_ALIAS-1);
            memcpy(tMtAlias.m_achAlias, achTmp, MAXLEN_ALIAS-1);
            tConfData.m_tConf.SetChairAlias(tMtAlias);
            //�����з����˵ı���m_tSpeakerAlias
            memset(achTmp, 0, sizeof(achTmp));
            tMtAlias.SetNull();
            tMtAlias = tConfData.m_tConf.GetSpeakerAlias();
            utf8_to_gb2312(tMtAlias.m_achAlias, achTmp, MAXLEN_ALIAS-1);
            memcpy(tMtAlias.m_achAlias, achTmp, MAXLEN_ALIAS-1);
            tConfData.m_tConf.SetSpeakerAlias(tMtAlias);
            //m_tMediaKey
            memset(achTmp, 0, sizeof(achTmp));
            TMediaEncrypt tMediaKey = tConfData.m_tConf.GetMediaKey();
            s8  abyEncKey[MAXLEN_KEY] = {0};
            s32 nKey = 0;
            u32 dwKey = 0;
            //DES����
            if( CONF_ENCRYPTMODE_DES == tMediaKey.GetEncryptMode() )
            {
                nKey = LEN_DES;
                tMediaKey.GetEncryptKey((u8*)abyEncKey, &nKey);
                dwKey = utf8_to_gb2312(abyEncKey, achTmp, MAXLEN_KEY-1);
                tMediaKey.SetEncryptKey((u8*)achTmp, dwKey);
                tConfData.m_tConf.SetMediaKey(tMediaKey); 
            }
            //AES����
            else if( CONF_ENCRYPTMODE_AES == tMediaKey.GetEncryptMode() )
            {
                nKey = LEN_AES;
                tMediaKey.GetEncryptKey((u8*)abyEncKey, &nKey);
                dwKey = utf8_to_gb2312(abyEncKey, achTmp, MAXLEN_KEY-1);
                tMediaKey.SetEncryptKey((u8*)achTmp, dwKey);
                tConfData.m_tConf.SetMediaKey(tMediaKey); 
            }
        }
        break;
    case TRANSENCODING_GBK_UTF8:
        {
            //תmcu�ϻ�����Ϣ
            //m_tConf        
            //������m_achConfName[MAXLEN_CONFNAME+1]
            memset(achTmp, 0, sizeof(achTmp));
            gb2312_to_utf8(tConfData.m_tConf.GetConfName(), achTmp, MAXLEN_CONFNAME);
            tConfData.m_tConf.SetConfName(achTmp);        
            //��������m_achConfPwd[MAXLEN_PWD+1]
            memset(achTmp, 0, sizeof(achTmp));
            gb2312_to_utf8(tConfData.m_tConf.GetConfPwd(), achTmp, MAXLEN_PWD);
            tConfData.m_tConf.SetConfPwd(achTmp);
            //��������ϯ�ı���m_tChairAlias
            memset(achTmp, 0, sizeof(achTmp));
            TMtAlias tMtAlias;
            tMtAlias = tConfData.m_tConf.GetChairAlias();
            gb2312_to_utf8(tMtAlias.m_achAlias, achTmp, MAXLEN_ALIAS-1);
            memcpy(tMtAlias.m_achAlias, achTmp, MAXLEN_ALIAS-1);
            tConfData.m_tConf.SetChairAlias(tMtAlias);
            //�����з����˵ı���m_tSpeakerAlias
            memset(achTmp, 0, sizeof(achTmp));
            tMtAlias.SetNull();
            tMtAlias = tConfData.m_tConf.GetSpeakerAlias();
            gb2312_to_utf8(tMtAlias.m_achAlias, achTmp, MAXLEN_ALIAS-1);
            memcpy(tMtAlias.m_achAlias, achTmp, MAXLEN_ALIAS-1);
            tConfData.m_tConf.SetSpeakerAlias(tMtAlias);
            //m_tMediaKey
            memset(achTmp, 0, sizeof(achTmp));
            TMediaEncrypt tMediaKey = tConfData.m_tConf.GetMediaKey();
            s8  abyEncKey[MAXLEN_KEY] = {0};
            s32 nKey = 0;
            u32 dwKey = 0;
            //DES����
            if( CONF_ENCRYPTMODE_DES == tMediaKey.GetEncryptMode() )
            {
                nKey = LEN_DES;
                tMediaKey.GetEncryptKey((u8*)abyEncKey, &nKey);
                dwKey = gb2312_to_utf8(abyEncKey, achTmp, MAXLEN_KEY-1);
                tMediaKey.SetEncryptKey((u8*)achTmp, dwKey);
                tConfData.m_tConf.SetMediaKey(tMediaKey); 
            }
            //AES����
            else if( CONF_ENCRYPTMODE_AES == tMediaKey.GetEncryptMode() )
            {
                nKey = LEN_AES;
                tMediaKey.GetEncryptKey((u8*)abyEncKey, &nKey);
                dwKey = gb2312_to_utf8(abyEncKey, achTmp, MAXLEN_KEY-1);
                tMediaKey.SetEncryptKey((u8*)achTmp, dwKey);
                tConfData.m_tConf.SetMediaKey(tMediaKey); 
            }
        } 
        break;
    default :
        break;
    }    
}

//END OF FILE
