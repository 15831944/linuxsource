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
#include "mcuerrcode.h"
#include "evmcumt.h"
#include "mtadpssn.h"

CNPlusApp g_cNPlusApp;

//CNPlusInst
//construct
CNPlusInst::CNPlusInst( void ):m_dwMcuNode(INVALID_NODE), 
                               m_dwMcuIId(0), 
                               m_dwMcuIpAddr(0),
                               m_wRtdFailTimes(0),
                               m_byUsrNum(0),
                               m_wRegNackByCapTimes(0)
{
    memset(&m_cUsrGrpsInfo, 0, sizeof(m_cUsrGrpsInfo));
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
        OspPrintf( TRUE, FALSE, "[CNPlusInst::InstanceEntry] invalid msg body!\n" );
        return;
    }

    switch( pcMsg->event )
    {  
    case OSP_DISCONNECT:
        ProcDisconnect( pcMsg );
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
    case MCU_NPLUS_CHAIRUPDATE_REQ:
    case MCU_NPLUS_SPEAKERUPDATE_REQ:
    case MCU_NPLUS_VMPUPDATE_REQ:
    case MCU_NPLUS_RASINFOUPDATE_REQ:
        ProcMcuDataUpdateReq( pcMsg );
        break;

    case MCU_NPLUS_EQPCAP_NOTIF:
        ProcMcuEqpCapNotif( pcMsg );
        break;

    case MCU_NPLUS_RTD_RSP:
        ProcRtdRsp( pcMsg );
        break;

    case MCU_NPLUS_RTD_TIMER:
        ProcRtdTimeOut( pcMsg );
        break;

        //����mcu���͵���mcu����ع���Ϣ
    case VC_NPLUS_MSG_NOTIF:
        ProcConfRollback( pcMsg );
        break;

    case VC_NPLUS_RESET_NOTIF:
        ProcReset( pcMsg );
        break;

    case MCU_NPLUS_CONFROLLBACK_ACK:
    case MCU_NPLUS_CONFROLLBACK_NACK:
    case MCU_NPLUS_USRROLLBACK_ACK:
    case MCU_NPLUS_USRROLLBACK_NACK:
    case MCU_NPLUS_GRPROLLBACK_ACK:
    case MCU_NPLUS_GRPROLLBACK_NACK:
        break;

    default:
        OspPrintf( TRUE, FALSE, "[CNPlusInst::InstanceEntry] invalid event(%d): %s!\n", 
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
    OspPrintf(TRUE, FALSE, "=================NPlusInst: InsID %d================\n", GetInsID());
    OspPrintf(TRUE, FALSE, "McuNode:0x%x McuIId:0x%x McuIp:0x%x\n", m_dwMcuNode, 
              m_dwMcuIId, m_dwMcuIpAddr);
    
    for (byIdx = 0; byIdx < MAXNUM_ONGO_CONF; byIdx++)
    {
        if (m_atConfData[byIdx].IsNull())
        {
            continue;
        }
        m_atConfData[byIdx].m_tConf.Print();

        OspPrintf(TRUE, FALSE, "\nMtInfo in conf:\n");
        for (byLoop = 0; byLoop < m_atConfData[byIdx].m_byMtNum; byLoop++)
        {   
            TMtInfo *ptInfo = &m_atConfData[byIdx].m_atMtInConf[byLoop];            
            OspPrintf(TRUE, FALSE, "%d: 0x%x(%d)  %d(kbs)\n", byLoop,
                      ptInfo->GetMtAddr().GetIpAddr(),
                      ptInfo->GetMtAddr().GetPort(), ptInfo->GetCallBitrate());
        }

        OspPrintf(TRUE, FALSE, "VmpInfo in conf:\n");
        for (byLoop = 0; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++)
        {
            TMtInfo *ptInfo = &m_atConfData[byIdx].m_tVmpInfo.m_atMtInVmp[byLoop];
            if (ptInfo->IsNull())
            {
                continue;
            }
            OspPrintf(TRUE, FALSE, "%d: 0x%x(%d)  %d(kbs) type(%d)\n", byLoop,
                      ptInfo->GetMtAddr().GetIpAddr(),
                      ptInfo->GetMtAddr().GetPort(), ptInfo->GetCallBitrate(), 
                      m_atConfData[byIdx].m_tVmpInfo.m_abyMemberType[byLoop]);
        }
    }

    m_cUsrGrpsInfo.Print();

    OspPrintf(TRUE, FALSE, "user info:\n");
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
    m_dwMcuNode = 0;
    m_dwMcuIId = 0;
    m_dwMcuIpAddr = 0;
    m_byUsrNum = 0;
    m_wRtdFailTimes = 0;
    m_wRegNackByCapTimes = 0;
    memset(m_atConfData, 0, sizeof(m_atConfData));
    memset(&m_cUsrGrpsInfo, 0, sizeof(m_cUsrGrpsInfo));
    memset(m_acUsrInfo, 0, sizeof(m_acUsrInfo));
    memset(&m_tRASInfo, 0, sizeof(m_tRASInfo));
    memset(&m_atChargeInfo, 0, sizeof(m_atChargeInfo));

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
=============================================================================*/
void CNPlusInst::ProcRegNPlusMcuReq( const CMessage * pcMsg )
{
    if (CurState() == STATE_NORMAL)
    {
        OspPrintf(TRUE, FALSE, "[ProcRegNPlusMcuReq] invalid ins state(%d)(id.%d)\n", CurState(), GetInsID());
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);   
    TNPlusMcuRegInfo *ptRegInfo = (TNPlusMcuRegInfo *)cServMsg.GetMsgBody();
    BOOL32 bRollBack = FALSE;
    
    //����Ƿ�Ϊ���ϻָ�mcu �� ������mcu�Ƿ�֧�ֹ��ϻָ�
    if (CurState() == STATE_SWITCHED && g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_SWITCH)
    {
        TLocalInfo tLocalInfo;
        if (SUCCESS_AGENT == g_cMcuAgent.GetLocalInfo(&tLocalInfo) && 
            tLocalInfo.GetIsNPlusRollBack() &&
            m_dwMcuIpAddr == ptRegInfo->GetMcuIpAddr())
        {        
            //������ָ�������������������������ܾ����ȴ����������������ٽ���ģʽ�л�
            TNPlusEqpCap tNeedCap, tTmpCap;
            for (u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF; byLoop++)
            {
                if (!m_atConfData[byLoop].IsNull())
                {
                    tTmpCap = g_cNPlusApp.GetEqpCapFromConf(m_atConfData[byLoop].m_tConf);
                    tNeedCap = tNeedCap + tTmpCap;
                }
            }
            
            tTmpCap = ptRegInfo->GetMcuEqpCap();
            // guzh  [12/13/2006] �������������ͬʱҪ���MCU����Mp��MtAdp
            if ( tTmpCap < tNeedCap || 
                 !tTmpCap.HasMp() || 
                 !tTmpCap.HasMtAdp() )
            {
                OspPrintf(TRUE, FALSE, "[ProcRegNPlusMcuReq] Reg MCU(0x%x) eqp cap is not enough to rollback conf\n", 
                          ptRegInfo->GetMcuIpAddr());

                // guzh [12/13/2006]
                // �����û�е���ܾ��ع�����
                // ��Ͽ����ӣ��˳�������ǿ�ƻع�����ֹ���ݷ������ٳ�
                m_wRegNackByCapTimes ++;
                // guzh [12/13/2006] ���ǵ����ܶԶ˻����������޷���ʼ����ʱ����
                //if ( m_wRegNackByCapTimes  < NPLUS_MAXNUM_REGNACKBYREMOTECAP)
                if (true)
                {                   
                    OspPrintf(TRUE, FALSE, "[ProcRegNPlusMcuReq] Reject m_wRegNackByCapTimes = %d\n", m_wRegNackByCapTimes);
                    OspDisconnectTcpNode(pcMsg->srcnode);
                    return;
                }                
            }
            
            //ģʽ�л� rollback
            bRollBack = TRUE; 
            
            //���ԭ�ȱ���Ļ�������
            ClearInst();
        }
        else
        {
            NPlusLog("[ProcRegNPlusMcuReq] mcu not support N+1 rollback.ignore mcu(0x%x) reg\n", 
                     ptRegInfo->GetMcuIpAddr());
            return;
        }        
    }    

    m_dwMcuIpAddr = ptRegInfo->GetMcuIpAddr();
    m_dwMcuIId = *(u32 *)(cServMsg.GetMsgBody() + sizeof(TNPlusMcuRegInfo));
    m_dwMcuNode = *(u32 *)(cServMsg.GetMsgBody() + sizeof(TNPlusMcuRegInfo) + sizeof(pcMsg->srcid));
    NPlusLog("[ProcRegNPlusMcuReq] McuIpAddr(0x%x) McuIId(%x) McuNode(%d)\n", 
        m_dwMcuIpAddr, m_dwMcuIId, m_dwMcuNode);

    //ack
    u8 byRollBack = bRollBack ? 1 : 0;
    PostReplyBack(pcMsg->event+1, 0, &byRollBack, sizeof(u8) );

    //ע���������ʵ��
    OspNodeDiscCBRegQ(m_dwMcuNode, GetAppID(), GetInsID());

    //����rtd
    SetTimer(MCU_NPLUS_RTD_TIMER, g_cNPlusApp.GetRtdTime()*1000);

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

        // zbq [03/15/2007] �ָ���MCU��ע��E164��
        CServMsg cServMsg;
        cServMsg.SetConfIdx( 0 );
        cServMsg.SetEventId( MCU_MT_RESTORE_MCUE164_NTF );
        g_cMtAdpSsnApp.SendMsgToMtAdpSsn( g_cMcuVcApp.GetRegGKDriId(), 
                                          MCU_MT_RESTORE_MCUE164_NTF, cServMsg );
    }
    
    NEXTSTATE(STATE_NORMAL);

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
                if (sizeof(TNPlusConfData) != cServMsg.GetMsgBodyLen())
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(confdata)\n", cServMsg.GetMsgBodyLen());
                    return;
                }                

                TNPlusConfData *ptConfData = (TNPlusConfData *)cServMsg.GetMsgBody();

                if (!SetConfData(ptConfData))
                {
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] set conf data failed.insid.%d\n", GetInsID());
                    return;
                }                
            }            
            break;

        case MCU_NPLUS_CONFINFOUPDATE_REQ:
            {
                if ((sizeof(u8) + sizeof(TConfInfo)) != cServMsg.GetMsgBodyLen())
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(confinfo)\n", cServMsg.GetMsgBodyLen());
                    return;
                }
                
                u8 byMode = *(u8 *)cServMsg.GetMsgBody();
                TConfInfo *ptConf = (TConfInfo *)(cServMsg.GetMsgBody() + sizeof(u8));
                if (!SetConfInfo(ptConf, NPLUS_CONF_START == byMode))
                {
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] set conf info failed.insid.%d\n", GetInsID());
                    return;
                }               
            }            
            break;

        case MCU_NPLUS_USRGRPUPDATE_REQ:
            {
                if ((sizeof(CUsrGrpsInfo)) != cServMsg.GetMsgBodyLen())
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(usrgrp)\n", cServMsg.GetMsgBodyLen());
                    return;
                }               

                CUsrGrpsInfo *pcGrpInfo = (CUsrGrpsInfo *)cServMsg.GetMsgBody();
                if (!SetUsrGrpInfo(pcGrpInfo))
                {
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] set grp info failed.insid.%d\n", GetInsID());
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
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(usrinfo)\n", cServMsg.GetMsgBodyLen());
                    return;
                }
                            
                u8 byUsrNum = cServMsg.GetMsgBodyLen()/sizeof(CExUsrInfo);
                CExUsrInfo *pcUsrInfo = (CExUsrInfo *)cServMsg.GetMsgBody();
                BOOL32 bAdd = (cServMsg.GetCurPktIdx() == 0) ? FALSE : TRUE;
                if (!SetUsrInfo(pcUsrInfo, byUsrNum, bAdd))
                {
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] set usr info failed.insid.%d\n", GetInsID());
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
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(mtinfo)\n", cServMsg.GetMsgBodyLen());
                    return;
                }
              
                TMtInfo *ptMtInfo = (TMtInfo *)cServMsg.GetMsgBody();
                u8 byMtNum = cServMsg.GetMsgBodyLen()/sizeof(TMtInfo);

                if (!SetConfMtInfo(cConfId, ptMtInfo, byMtNum))
                {
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] set mt info failed. insid.%d\n", GetInsID());
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
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(chair)\n", cServMsg.GetMsgBodyLen());
                    return;
                }

                TMtAlias *ptAlias = (TMtAlias *)cServMsg.GetMsgBody();
                if (!SetChairman(cConfId, ptAlias))
                {
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] set chair info failed.insid.%d\n", GetInsID());
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
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(speaker)\n", cServMsg.GetMsgBodyLen());
                    return;
                }
             
                TMtAlias *ptAlias = (TMtAlias *)cServMsg.GetMsgBody();
                if (!SetSpeaker(cConfId, ptAlias))
                {
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] set speaker info failed.insid.%d\n", GetInsID());
                    return;
                }
            }            
            break;

        case MCU_NPLUS_VMPUPDATE_REQ:
            {
                if (sizeof(TNPlusVmpInfo) + sizeof(TVMPParam) != cServMsg.GetMsgBodyLen())
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(vmpinfo)\n", cServMsg.GetMsgBodyLen());
                    return;
                }

                TNPlusVmpInfo *ptVmpInfo = (TNPlusVmpInfo *)cServMsg.GetMsgBody();
                TVMPParam *ptParam = (TVMPParam *)(cServMsg.GetMsgBody() + sizeof(TNPlusVmpInfo));
                if (!SetConfVmpInfo(cConfId, ptVmpInfo, ptParam))
                {
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] set vmp info failed.insid.%d\n", GetInsID());
                    return;
                }
            }            
            break;

        case MCU_NPLUS_RASINFOUPDATE_REQ:
            {
                if ( sizeof(TRASInfo) != cServMsg.GetMsgBodyLen() ) 
                {
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(rasinfo)\n", cServMsg.GetMsgBodyLen() );
                    return;
                }
                TRASInfo tRASInfo = *(TRASInfo*)cServMsg.GetMsgBody();
                SetRASInfo(&tRASInfo);
                
                if ( tRASInfo.GetEPID()->GetIDlength() == 0 )
                {
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] EPID has set to SLAVE mcu Error!\n");
                }
            }
            break;

        default: 
            OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] invalid msg (%s:%d)\n",
                                        OspEventDesc(pcMsg->event), pcMsg->event);
            return;
            break;
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
    if (sizeof(TNPlusEqpCap) != cServMsg.GetMsgBodyLen())
    {
        OspPrintf(TRUE, FALSE, "[ProcMcuEqpCapNotif] invalid msg body len(%d)\n", cServMsg.GetMsgBodyLen());
        return;
    }

    TNPlusEqpCap tLocalCap = g_cNPlusApp.GetMcuEqpCap();
    TNPlusEqpCap tRegMcuCap = *(TNPlusEqpCap *)cServMsg.GetMsgBody();
    if (tLocalCap < tRegMcuCap)
    {
        OspPrintf(TRUE, FALSE, "[ProcMcuEqpCapNotif] registing mcu eqpcap is larger then local. disconnect node(%d)\n", 
            cServMsg.GetMsgBodyLen(), m_dwMcuNode);
        OspDisconnectTcpNode(m_dwMcuNode);
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
        OspPrintf(TRUE, FALSE, "[ProcConfRollback] invalid msg(%s) received.\n", OspEventDesc(cServMsg.GetEventId()));
        return;
        break;
    }

    PostMsgToNPlusMcuDaemon(wEvent, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
    
    NPlusLog("[ProcConfRollback] send msg %s to master mcu(0x%x).\n", OspEventDesc(wEvent), m_dwMcuIpAddr);  
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
void CNPlusInst::ProcReset( const CMessage * pcMsg )
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
void CNPlusInst::ProcRtdRsp( const CMessage * pcMsg )
{
    m_wRtdFailTimes = 0;    
    SetTimer(MCU_NPLUS_RTD_TIMER, g_cNPlusApp.GetRtdTime()*1000);   
    NPlusLog("[ProcRtdRsp] InsId(%d) mcu(0x%x).\n", GetInsID(), m_dwMcuIpAddr); 
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
void CNPlusInst::ProcRtdTimeOut( const CMessage * pcMsg )
{        
    m_wRtdFailTimes++;

    //��Ҫ����ģʽ�л����ָ���ʵ����Ӧmcu�Ļ�����Ϣ
    if (m_wRtdFailTimes >= g_cNPlusApp.GetRtdNum())
    {
        NPlusLog("[ProcRtdTimeOut] mode switch and RESTORE mcu(0x%x) conf.\n", m_dwMcuIpAddr);       
        RestoreMcuConf();        
    }
    else
    {
        NPlusLog("[ProcRtdTimeOut] rtd time out and retrying mcu(0x%x).\n", m_dwMcuIpAddr); 
        PostMsgToNPlusMcuDaemon(MCU_NPLUS_RTD_REQ);
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
=============================================================================*/
void CNPlusInst::ProcDisconnect( const CMessage * pcMsg )
{
    NPlusLog("[ProcDisconnect] OSP Disconnected MCU(0x%x).\n", m_dwMcuIpAddr);
    return;
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
=============================================================================*/
void CNPlusInst::PostReplyBack( u16 wEvent, u16 wErrorCode, u8 *const pbyMsg, u16 wMsgLen )
{
    CServMsg cServMsg;
    cServMsg.SetEventId(wEvent);
    cServMsg.SetErrorCode(wErrorCode);
    if (wMsgLen > 0 && NULL != pbyMsg)
    {
        cServMsg.SetMsgBody(pbyMsg, wMsgLen);
    }
    post(m_dwMcuIId, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), m_dwMcuNode);
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
        OspPrintf(TRUE, FALSE, "[RestoreMcuConf] invalid NPlusState.%d, restore mcu conf failed.\n",
                  g_cNPlusApp.GetLocalNPlusState());
        
        ClearInst();
        return;
    }

    // ���⴦��1: N+1 ����ע��GK, ����MCUȡ����MCU����������ע����MCU�����е�ʵ
    //            ��ID����������������ͬGK������£�����MCU�ٿ��Ķ�Ӧ���齫����ʵ�������ն� [12/23/2006-zbq]
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
        BOOL32 bVmpMod = ptConfData->m_tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_CTRL ? TRUE : FALSE;
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
            atAlias[byLoop].m_tTransportAddr = ptMtInfo->GetMtAddr();
            awMtDialBitRate[byLoop] = htons(ptMtInfo->GetCallBitrate());

            //����vmpmod���ն�����
            if (bVmpMod)
            {
                for (u8 byChnl = 0; byChnl < MAXNUM_MPUSVMP_MEMBER; byChnl++)
                {
                    if (ptConfData->m_tVmpInfo.IsMtInVmpMem(*ptMtInfo, byChnl))
                    {
                        u8 byType = ptConfData->m_tVmpInfo.GetVmpMemType(*ptMtInfo, byChnl);
                        tVmpMod.SetVmpMember(byChnl, byLoop+1, byType);
                    }    
                }                            
            }
        }

        PackTMtAliasArray(atAlias, awMtDialBitRate, ptConfData->m_byMtNum, achAliasBuf, wAliasBufLen);

        //ȥ��tvwall��Ϣ����Ϊtvwall����������id��أ����ܽ��лָ�
        ptConfData->m_tConf.SetHasTvwallModule(FALSE);
        ptConfData->m_tConf.SetHasVmpModule(bVmpMod);

        ptConfData->m_tConf.m_tStatus.m_tConfMode.SetTakeMode(CONF_TAKEMODE_ONGOING);        
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
    CExUsrInfo cUsrInfo;
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
        OspPrintf( TRUE, FALSE, "[CNPlusInst::DaemonInstanceEntry] invalid msg body!\n" );
        return;
    }

    switch( pcMsg->event )
    {
    case OSP_POWERON:
        DaemonPowerOn( pcMsg, pcApp );
        break;

    case VC_NPLUS_RESET_NOTIF:
    case MCS_MCU_NPLUSRESET_CMD:    // Mcs ����Reset
        DaemonReset( pcMsg, pcApp );
        break;

    case MCS_MCU_NPLUSROLLBACK_REQ:
        DaemonMcsRollBack(pcMsg, pcApp);
        break;
        
    case OSP_DISCONNECT:
        DaemonDisconnect( pcMsg, pcApp );
        break;

    case MCU_NPLUS_REG_REQ:
        DaemonRegNPlusMcuReq( pcMsg, pcApp );
        break;

    case MCU_NPLUS_REG_ACK:
    case MCU_NPLUS_REG_NACK:
        DaemonRegNPlusMcuRsp( pcMsg, pcApp );
        break;

    case MCU_NPLUS_CONNECT_TIMER:
        DaemonConnectNPlusMcuTimeOut( pcMsg );
        break;
    
    case MCU_NPLUS_REG_TIMER:
        DaemonRegNPlusMcuTimeOut( pcMsg );
        break;

        //��mcu�����ϱ���Ϣ
    case VC_NPLUS_MSG_NOTIF:  
        DaemonVcNPlusMsgNotif( pcMsg, pcApp );
        break;

        //����rtd��Ϣ����
    case MCU_NPLUS_RTD_REQ:
        DaemonRtdReq( pcMsg, pcApp );
        break;

    case MCU_NPLUS_CONFINFOUPDATE_ACK:    
    case MCU_NPLUS_USRGRPUPDATE_ACK:
    case MCU_NPLUS_CONFMTUPDATE_ACK:    
    case MCU_NPLUS_CHAIRUPDATE_ACK:    
    case MCU_NPLUS_SPEAKERUPDATE_ACK:    
    case MCU_NPLUS_VMPUPDATE_ACK:    
    case MCU_NPLUS_CONFDATAUPDATE_ACK:    
    case MCU_NPLUS_USRINFOUPDATE_ACK:
    case MCU_NPLUS_RASINFOUPDATE_ACK:
        
    case MCU_NPLUS_CONFINFOUPDATE_NACK:
    case MCU_NPLUS_USRGRPUPDATE_NACK:
    case MCU_NPLUS_CONFMTUPDATE_NACK:
    case MCU_NPLUS_CHAIRUPDATE_NACK:
    case MCU_NPLUS_SPEAKERUPDATE_NACK:
    case MCU_NPLUS_VMPUPDATE_NACK:
    case MCU_NPLUS_CONFDATAUPDATE_NACK:
    case MCU_NPLUS_USRINFOUPDATE_NACK:
    case MCU_NPLUS_RASINFOUPDATE_NACK:
        
        DaemonDataUpdateRsp( pcMsg, pcApp );
        break;

    //����ع�
    case MCU_NPLUS_CONFROLLBACK_REQ:
        DaemonConfRollbackReq( pcMsg, pcApp );
        break;
    //�û��ع�
    case MCU_NPLUS_USRROLLBACK_REQ:
        DaemonUsrRollbackReq( pcMsg, pcApp );
        break;
    //�û���ع�
    case MCU_NPLUS_GRPROLLBACK_REQ:
        DaemonGrpRollbackReq( pcMsg, pcApp );
        break;
        
    default:
        OspPrintf( TRUE, FALSE, "[CNPlusInst::DaemonInstanceEntry] invalid event(%d): %s!\n", 
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
void  CNPlusInst::DaemonPowerOn( const CMessage * pcMsg, CApp* pcApp  )
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
void  CNPlusInst::DaemonReset( const CMessage * pcMsg, CApp* pcApp  )
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
            g_cMcsSsnApp.SendMsgToMcsSsn( cServMsg.GetSrcSsnId(), 
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
void CNPlusInst::DaemonDisconnect( const CMessage * pcMsg, CApp* pcApp  )
{
    OspPrintf(TRUE, FALSE, "[DaemonDisconnect] node(%d) disconnect. connecting...\n", m_dwMcuNode);
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
void CNPlusInst::DaemonDataUpdateRsp( const CMessage * pcMsg, CApp* pcApp )
{
    if ( pcMsg->event == MCU_NPLUS_CONFINFOUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_VMPUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_CONFDATAUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_CONFMTUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_CHAIRUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_SPEAKERUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_USRINFOUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_USRGRPUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_RASINFOUPDATE_ACK )
    {
        g_cNPlusApp.SetNPlusSynOk(TRUE);
    }
    else if ( pcMsg->event == MCU_NPLUS_CONFINFOUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_VMPUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_CONFDATAUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_CONFMTUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_CHAIRUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_SPEAKERUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_USRINFOUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_USRGRPUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_RASINFOUPDATE_NACK )
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
void CNPlusInst::DaemonConfRollbackReq( const CMessage * pcMsg, CApp* pcApp )
{
    if (g_cNPlusApp.GetLocalNPlusState() != MCU_NPLUS_MASTER_CONNECTED)
    {
        OspPrintf(TRUE, FALSE, "[DaemonConfRollbackReq] invalid N+1 mode state(%d), ignore conf rollback msg\n", 
                  g_cNPlusApp.GetLocalNPlusState());
        return;
    }

    //ack
    PostReplyBack(pcMsg->event+1);

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    //�������Ѿ�ͨ�������Զ��ָ��ٿ����Ƚ������ٽ��лָ�
    TNPlusConfData tConfData = *(TNPlusConfData *)cServMsg.GetMsgBody();
    if (tConfData.IsNull())
    {
        OspPrintf(TRUE, FALSE, "[DaemonConfRollbackReq] confid is null, ignore conf rollback msg\n");
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
    BOOL32 bVmpMod = tConfData.m_tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_CTRL ? TRUE : FALSE;
    if (bVmpMod)
    {
        tVmpMod.SetVmpParam(tConfData.m_tConf.m_tStatus.GetVmpParam());

        // ����ϳɳ�Ա��Ϣֻ�ܱ��浽������Ϣ����ṹֻ����������Ϣ [12/14/2006-zbq]
        for( u8 byIndex = 0; byIndex <  MAXNUM_MPUSVMP_MEMBER; byIndex ++ )
        {
            tVmpMod.m_tVMPParam.ClearVmpMember(byIndex);
        }
    }

    for(u8 byLoop = 0; byLoop < tConfData.m_byMtNum; byLoop++)
    {            
        ptMtInfo = &tConfData.m_atMtInConf[byLoop];
        atAlias[byLoop].m_AliasType = mtAliasTypeTransportAddress;
        atAlias[byLoop].m_tTransportAddr = ptMtInfo->GetMtAddr();
        awMtDialBitRate[byLoop] = htons(ptMtInfo->GetCallBitrate());

        //����vmpmod���ն�����
        if (bVmpMod)
        {
            for (u8 byChnl = 0; byChnl < MAXNUM_MPUSVMP_MEMBER; byChnl++)
            {
                if (tConfData.m_tVmpInfo.IsMtInVmpMem(*ptMtInfo, byChnl))
                {
                    u8 byType = tConfData.m_tVmpInfo.GetVmpMemType(*ptMtInfo, byChnl);
                    tVmpMod.SetVmpMember(byChnl, byLoop+1, byType);
                }    
            }               
        }
    }

    PackTMtAliasArray(atAlias, awMtDialBitRate, tConfData.m_byMtNum, achAliasBuf, wAliasBufLen);

    //ȥ��tvwall��Ϣ����Ϊtvwall����������id��أ����ܽ��лָ�
    tConfData.m_tConf.SetHasTvwallModule(FALSE);
    tConfData.m_tConf.SetHasVmpModule(bVmpMod);

    tConfData.m_tConf.m_tStatus.m_tConfMode.SetTakeMode(CONF_TAKEMODE_ONGOING);
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
void CNPlusInst::DaemonUsrRollbackReq( const CMessage * pcMsg, CApp* pcApp )
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
void CNPlusInst::DaemonGrpRollbackReq( const CMessage * pcMsg, CApp* pcApp )
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
void CNPlusInst::DaemonConnectNPlusMcuTimeOut( const CMessage * pcMsg )
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
			    NPlusLog("OspConnectTcpNode MCU(0x%x) Failed!\n", tLocalInfo.GetNPlusMcuIp());
			    SetTimer( MCU_NPLUS_CONNECT_TIMER, NPLUS_CONNECTMCU_TIMEOUT );
			    return;
            }
            OspNodeDiscCBRegQ(m_dwMcuNode, GetAppID(), GetInsID());
        }
        else
        {
            OspPrintf(TRUE, FALSE, "[DaemonConnectNPlusMcu] mcu agent get localinfo failed.\n");
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
void CNPlusInst::DaemonRegNPlusMcuTimeOut( const CMessage * pcMsg )
{
    if (CurState() != STATE_IDLE)
    {
        return;
    }

    TNPlusMcuRegInfo tRegInfo;
    u32 dwIp = ntohl(g_cMcuAgent.GetMpcIp());
    tRegInfo.SetMcuIpAddr( dwIp );
    tRegInfo.SetMcuEqpCap( g_cNPlusApp.GetMcuEqpCap() );

    // N+1 ���ݻ���Լ��: 8000B��������8000������8�ֱ������������ [12/20/2006-zbq]
    tRegInfo.SetMcuType( g_cNPlusApp.GetMcuType() );

    PostMsgToNPlusMcuDaemon( MCU_NPLUS_REG_REQ, (u8 *)&tRegInfo, sizeof(tRegInfo) );

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
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    if (sizeof(TNPlusMcuRegInfo) != cServMsg.GetMsgBodyLen())
    {
        OspPrintf( TRUE, FALSE, "[DaemonRegNPlusMcuReq] cServMsg.GetMsgBodyLen()=%d, sizeof(TNPlusMcuRegInfo)=%d !\n",
                   cServMsg.GetMsgBodyLen(), sizeof(TNPlusMcuRegInfo));
        DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_MSGLEN_ERR );
        return;
    }

    TNPlusMcuRegInfo *ptRegInfo = (TNPlusMcuRegInfo *)(cServMsg.GetMsgBody());

    //��MCU��֧��N+1���� �� N+1�����±������ã��ܾ�����MCU������������ ��ע��ɹ� �� ��������[12/28/2006-zbq]
    if ( MCU_NPLUS_IDLE == g_cNPlusApp.GetLocalNPlusState() ||
         MCU_NPLUS_MASTER_IDLE == g_cNPlusApp.GetLocalNPlusState() ||
         MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState() )
    {
        DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_CONNECT_MASTER );
        return;
    }

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
    TNPlusEqpCap tLocalCap = g_cNPlusApp.GetMcuEqpCap();
    if (tLocalCap < ptRegInfo->GetMcuEqpCap())
    {
        DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_CAP_ERR );
        return;
    }

    u8 byInsId = 1;
    if (g_cNPlusApp.GetMcuSwitchedIp() == ptRegInfo->GetMcuIpAddr())
    {
        byInsId = g_cNPlusApp.GetMcuSwitchedInsId();
    }
    else
    {
        for( ; byInsId <= MAXNUM_NPLUS_MCU; byInsId++ )
        {
            CNPlusInst *pcInst = (CNPlusInst *)pcApp->GetInstance(byInsId);
            if (NULL != pcInst && pcInst->CurState() == STATE_IDLE)
            {
                break;
            }
        }    
    }    

    if (byInsId <= MAXNUM_NPLUS_MCU)
    {
        cServMsg.CatMsgBody((u8 *)&pcMsg->srcid, sizeof(pcMsg->srcid));
        cServMsg.CatMsgBody((u8 *)&pcMsg->srcnode, sizeof(pcMsg->srcnode));
        post(MAKEIID(AID_MCU_NPLUSMANAGER, byInsId), pcMsg->event, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
    }
    else
    {
        DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_OVERLOAD );
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
    DaemonReplyNack( pcMsg, cServMsg );
    OspDisconnectTcpNode(m_dwMcuNode);

    OspPrintf(TRUE, FALSE, "[DaemonRegNPlusMcuReq] NPlus reg failed due to reason<%d>.\n", byReason);
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
void CNPlusInst::DaemonRegNPlusMcuRsp( const CMessage * pcMsg, CApp* pcApp )
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
                
                CServMsg cServMsg;
                cServMsg.SetMsgBody( (u8*)&tRASInfo, sizeof(tRASInfo) );
                cServMsg.SetEventId( MCU_NPLUS_RASINFOUPDATE_REQ );

                PostMsgToNPlusMcuInst( cServMsg );
            }
            else
            {
                NPlusLog("[DaemonRegNPlusMcuRsp] Rasinfo invalid, the RRQ mtadp will notify the backup mcu in NO TIME !\n");
            }
        }
    }
    else
    {
        //�ϱ���ҵ������ע��ʧ�ܵ�ԭ�� [12/18/2006-zbq]
        OspPost(MAKEIID(AID_MCU_VC, CInstance::DAEMON), pcMsg->event, NULL, 0);
        OspPrintf(TRUE, FALSE, "[DaemonRegNPlusMcuRsp] receive register nack due to Reason.%d.\n", cServMsg.GetErrorCode());
    }
    return;
}

/*=============================================================================
�� �� ���� DaemonReplyNack
��    �ܣ� ����mcu Daemonʵ���ķ���nack����mcu
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  const CMessage * pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/22  4.0			������                  ����
=============================================================================*/
void CNPlusInst::DaemonReplyNack( const CMessage * pcMsg, CServMsg &cServMsg )
{
    post(pcMsg->srcid, pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);
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
void CNPlusInst::DaemonVcNPlusMsgNotif( const CMessage * pcMsg, CApp* pcApp )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    if (CurState() != STATE_NORMAL)
    {
//        OspPrintf(TRUE, FALSE, "[DaemonVcNPlusMsgNotif] invalid ins state(%d), ignore it(%s)\n", 
//            CurState(), OspEventDesc(cServMsg.GetEventId()));
        return;
    }

    PostMsgToNPlusMcuInst(cServMsg);
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
void CNPlusInst::DaemonRtdReq( const CMessage * pcMsg, CApp* pcApp )
{
    NPlusLog("[DaemonRtdReq] receive rtd request.\n");
    PostReplyBack(MCU_NPLUS_RTD_RSP);
    return;
}

/*=============================================================================
�� �� ���� PostMsgToNPlusMcuDaemon
��    �ܣ� ��mcu daemon������Ϣ������mcu daemonʵ��
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
=============================================================================*/
void CNPlusInst::PostMsgToNPlusMcuDaemon( u16 wEvent, u8 *const pbyMsg, u16 wMsgLen )
{
    CServMsg cServMsg;
    cServMsg.SetEventId(wEvent);
    if (wMsgLen > 0 && NULL != pbyMsg)
    {
        cServMsg.SetMsgBody(pbyMsg, wMsgLen);
    }    
    post(MAKEIID(AID_MCU_NPLUSMANAGER, CInstance::DAEMON), 
        wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), m_dwMcuNode);
    return;
}

/*=============================================================================
�� �� ���� PostMsgToNPlusMcuInst
��    �ܣ� ��mcu daemon������Ϣ������mcu��Ӧʵ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/22  4.0			������                  ����
=============================================================================*/
void CNPlusInst::PostMsgToNPlusMcuInst( CServMsg &cServMsg )
{    
    post(m_dwMcuIId, cServMsg.GetEventId(), cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), m_dwMcuNode);
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
        OspPrintf(TRUE, FALSE, "[CNPlusInst::SetConfData] get conf index failed by cconfid:");
        ptConfData->m_tConf.GetConfId().Print();
        return FALSE;
    }

    memcpy(&m_atConfData[byIdx], ptConfData, sizeof(TNPlusConfData));
    return TRUE;
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
BOOL32  CNPlusInst::SetConfInfo( TConfInfo *ptConfInfo, BOOL32 bStart )
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
        OspPrintf(TRUE, FALSE, "[CNPlusInst::SetConfInfo] get conf index failed by cconfid:");
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
        m_atConfData[byIdx].Clear();
    }
    
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
        OspPrintf(TRUE, FALSE, "[CNPlusInst::SetConfMtInfo] get conf index failed by cconfid:");
        cConfId.Print();
        return FALSE;
    }

    memcpy(m_atConfData[byIdx].m_atMtInConf, ptMtInfo, byMtNum*sizeof(TMtInfo));
    m_atConfData[byIdx].m_byMtNum = byMtNum;
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
        OspPrintf(TRUE, FALSE, "[CNPlusInst::SetConfVmpInfo] get conf index failed by cconfid:");
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
        OspPrintf(TRUE, FALSE, "[CNPlusInst::SetChairman] get conf index failed by cconfid:");
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
        OspPrintf(TRUE, FALSE, "[CNPlusInst::SetSpeaker] get conf index failed by cconfid:");
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
            OspPrintf(TRUE, FALSE, "[SetUsrInfo] user packet error.\n");
            return FALSE;
        }
        memcpy(&m_acUsrInfo[m_byUsrNum], pcUsrInfo, sizeof(CExUsrInfo)*byNum);
        m_byUsrNum += byNum;
    }
    else
    {
        if (byNum > MAXNUM_USRNUM)
        {
            OspPrintf(TRUE, FALSE, "[SetUsrInfo] user packet error.\n");
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

    if ( 0 == ptRASInfo->GetEPID()->GetIDlength() ) 
    {
        OspPrintf( TRUE, FALSE, "[UnRegAllInfoOfSwitchedMcu] the EPID of MASTER mcu hasn't got yet !\n" );
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
            if (byEqpId <= BASID_MAX)
            {
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
                        tEqpCap.m_byMAUNum++;
                    }
                    else if (TYPE_MAU_H263PLUS == tStatus.m_tStatus.tHdBas.GetEqpType())
                    {
                        tEqpCap.m_byMAUH263pNum ++;
                    }
                    else if (TYPE_MPU == tStatus.m_tStatus.tHdBas.GetEqpType())
                    {
                        tEqpCap.m_byMpuBasNum ++;
                    }
                    else
                    {
                        OspPrintf(TRUE, FALSE, "[CNPlusData::GetMcuEqpCap] unexpected mau type.%d\n",
                                                tStatus.m_tStatus.tHdBas.GetEqpType());
                    }
                }
               
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
                tEqpCap.m_byVmpNum++;

                // VMP�������嵽MAP�� [12/28/2006-zbq]
                tEqpCap.m_abyVMPChnNum[tEqpCap.m_byVmpNum-1] = tStatus.m_tStatus.tVmp.m_byChlNum;
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
TNPlusEqpCap    CNPlusData::GetEqpCapFromConf( TConfInfo &tConf )
{
    TNPlusEqpCap tEqpCap;
    //vmp
	//�Զ�����ϳ���ҪVMP����֧�� [01/13/2006-zbq]
    if ( tConf.GetConfAttrb().IsHasVmpModule() || 
	     CONF_VMPMODE_AUTO == tConf.m_tStatus.GetVMPMode() )
    {
        tEqpCap.m_byVmpNum++;
    }
    //bas
    if (tConf.GetConfAttrb().IsUseAdapter())
    {
        if (!IsHDConf(tConf))
        {
            if (tConf.GetSecAudioMediaType() != MEDIA_TYPE_NULL)
            {
                tEqpCap.m_byBasAudChnlNum++;
            }
            else if (tConf.GetSecVideoMediaType() != MEDIA_TYPE_NULL)
            {
                tEqpCap.m_byBasVidChnlNum++;
            }
            
            if (tConf.GetSecBitRate() != 0)
            {
                tEqpCap.m_byBasVidChnlNum++;
            }
        }
        else
        {
            u8 byNVChn = 0;
            u8 byDSChn = 0;
            u8 byH263pChn = 0;
            u8 byVGAChn = 0;
            
            u8 byMpuNum = 0;
            g_cMcuVcApp.GetMpuNum(byMpuNum);

            //��mpu����Ϊ�Ǵ�mpu���ݣ��ݲ�֧��mpu��mau�Ľ��汸��
            if (byMpuNum > 0)
            {
                CBasMgr cBasMgr;
                cBasMgr.GetNeededMpu(tConf, byNVChn, byDSChn);
                tEqpCap.m_byMpuBasNum += (byNVChn + byDSChn)/MAXNUM_MPU_CHN + 1;
            }
            else
            {
                CBasMgr cBasMgr;
                cBasMgr.GetNeededMau(tConf, byNVChn, byH263pChn, byVGAChn);
                
                tEqpCap.m_byMAUNum += byNVChn;
                tEqpCap.m_byMAUH263pNum += byH263pChn;
            }
        }
    }
    //prs
    if (tConf.GetConfAttrb().IsResendLosePack())
    {
        tEqpCap.m_byPrsChnlNum++;
    }
    //GK Charge [12/18/2006-zbq]
    tEqpCap.SetIsGKCharge(tConf.m_tStatus.IsGkCharge());
    
    return tEqpCap;
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

//END OF FILE
