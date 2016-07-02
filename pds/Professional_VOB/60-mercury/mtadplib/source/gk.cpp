/*****************************************************************************
   ģ����      : mtadp3.0
   �ļ���      : gk.cpp
   ����ļ�    : mtadp.h
   �ļ�ʵ�ֹ���: MCU GK
   ����        : tanguang
   �汾        : V1.0  Copyright(C) 2001-2003 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2004/04     1.0         tanguang      ����
******************************************************************************/

#include "osp.h"
#include "cmsize.h"
#include "seli.h"
#include "mtadp.h"
#include "mtadputils.h"
#include "evmcumt.h"
//#include "mcuconst.h"
//#include "h323adapter.h"

#include "stkutils.h"
#include "rasdef.h"

// [pengjie 2010/3/9] CRI2/MPC2 ֧��
#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
//        #include "brdwrapperdef.h"
        #include "nipwrapper.h"
//        #include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
#endif

#ifdef _VXWORKS_
#include "brddrvlib.h"
#endif


#ifdef MULTI_THREADS
//helper class for stack global lock
struct RvMutexHelper
{
	BOOL32 result;
	RvMutexHelper():result(FALSE)
	{
		result = RadEnterCritic();
	}

	~RvMutexHelper()
	{
		result = RadExitCritic();
	}
};

#define LOCK_RV_STACK(ret) \
	do{ RvMutexHelper rvLock; if(!rvLock.result) return ret; } while( 0 )

//#else
//#define LOCK_RV_STACK(ret)

#endif


/*=============================================================================
  �� �� ���� ProcRasEvMcuMt
  ��    �ܣ� �����mcu��gk����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg ��Ϣ��ָ̬��
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2004/8        1.0			tanguang              ����
=============================================================================*/
// void CMtAdpInst::ProcRasEvMcuMt(CMessage * const pcMsg)
// {
// }

/*=============================================================================
  �� �� ���� ProcSendARQ
  ��    �ܣ� ����ARQ����ͨ��GK��Ȩ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TMtAlias &tSrcAddress, Դ��ַ�����ṹ
             TMtAlias &tDstAddress, Ŀ�ĵ�ַ�����ṹ
             u16 wBandwidth       , �������
  �� �� ֵ�� BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2004/4        1.0			tanguang              ����
=============================================================================*/
BOOL32 CMtAdpInst::ProcSendARQ( TMtAlias &tSrcAddress, TMtAlias &tDstAddress, u16 wBandwidth )
{
	s32 nCallRate = wBandwidth;
	s32 nARQRet   = 0;
	
	if( !g_cMtAdpApp.m_bGkAddrSet && g_cMtAdpApp.m_tGKAddr.ip )
	{
		SetGKRasAddress(g_cMtAdpApp.m_tGKAddr);
	}

	if( !m_hsCall )
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Invalid hsCall in ARQ!\n" );
        return FALSE;
    }		

	//prepare src & dest info in form of cmAlias
	cmAlias tSrcAlias, tDestAlias;
    s8  achSrcName[256] = {0}, achDestName[256] = {0};
	memset((void*)&tSrcAlias,  0, sizeof( tSrcAlias ));
	memset((void*)&tDestAlias, 0, sizeof( tDestAlias));

	tSrcAlias.string  = achSrcName;
	tDestAlias.string = achDestName;
	 
	CMtAdpUtils::MtAliasIn2Out( tSrcAddress, tSrcAlias );
	CMtAdpUtils::MtAliasIn2Out( tDstAddress, tDestAlias);

	//zbq[06/20/2008] RASʹ����ʽGK��ַ, ���������ڿ�������ʱ�������Э��ջpvtNode��ѯ
	nARQRet = cmRASStartTransaction( g_cMtAdpApp.m_hApp, NULL, &m_hsRas, cmRASAdmission, &g_cMtAdpApp.m_tGKAddr, m_hsCall );
	
	if( nARQRet < 0 )
	{	
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "cmRASStartTransaction failed in arq! Ret = %d g_hApp=0x%x hsCall=0x%x g_bGKAddrsSet = %d!\n", 
			           nARQRet, g_cMtAdpApp.m_hApp, m_hsCall, g_cMtAdpApp.m_bGkAddrSet );	
        DaemonProcGetStackResFailed();
		return FALSE;
	}
	
	//��������������RRQ(����Ϊ����)
	if( FALSE == g_cMtAdpApp.m_bMasterMtAdp )
	{
		s32 nRet = 0;
		
		//Set GatekeeperID
		cmAlias tGKID;
		tGKID.type   = ( cmAliasType ) g_cMtAdpApp.m_tGKID.GetIDtype();
		tGKID.length = ( UINT16 ) g_cMtAdpApp.m_tGKID.GetIDlength();
		tGKID.pnType = ( cmPartyNumberType ) g_cMtAdpApp.m_tGKID.GetIDpnType();
		tGKID.string = g_cMtAdpApp.m_tGKID.GetIDAlias();
		memcpy( (void*)&tGKID.transport, (void*)g_cMtAdpApp.m_tGKID.GetIDtransport(), sizeof(TH323TransportAddress) );		

		nRet = cmRASSetParam( m_hsRas, cmRASTrStageRequest, cmRASParamGatekeeperID, 0,
						      sizeof(tGKID),(s8*)&(tGKID) );
		if( nRet < 0 )
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcSendARQ] -- set optional cmRASParamGatekeeperID error nRet = %d!\n", nRet);
			cmRASClose( m_hsRas );
			return FALSE;
		}

		//Set EndpointID
		cmAlias tEPID;
		tEPID.type   = ( cmAliasType ) g_cMtAdpApp.m_tEPID.GetIDtype();
		tEPID.length = ( UINT16 ) g_cMtAdpApp.m_tEPID.GetIDlength();
		tEPID.pnType = ( cmPartyNumberType ) g_cMtAdpApp.m_tEPID.GetIDpnType();
		tEPID.string = g_cMtAdpApp.m_tEPID.GetIDAlias();
		memcpy( (void*)&tEPID.transport, (void*)g_cMtAdpApp.m_tEPID.GetIDtransport(), sizeof(TH323TransportAddress));
		
		nRet = cmRASSetParam( m_hsRas, cmRASTrStageRequest, cmRASParamEndpointID, 0,
					          sizeof(tEPID), (s8*)&(tEPID) );
		if( nRet < 0 )
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcSendARQ] -- set optional cmRASParamEndpointID error nRet=%d!\n", nRet);
			cmRASClose( m_hsRas );
			return FALSE;
		}
	}

	//set source address
	if( tSrcAlias.type == cmAliasTypeTransportAddress )
	{
		if( cmRASSetParam( m_hsRas, cmRASTrStageRequest, cmRASParamSrcCallSignalAddress, 0,
						   sizeof(tSrcAlias.transport), (s8*)&tSrcAlias.transport) < 0 )
		{
			MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "Sending ARQ: Set cmRASParamSrcCallSignalAddress failed Src ip=%u.%u.%u.%u port=%d\n", 
						  QUADADDR(tSrcAlias.transport.ip), tSrcAlias.transport.port );
			cmRASClose( m_hsRas );
			return FALSE;
		}
	}
	else
	{	
		if( cmRASSetParam( m_hsRas, cmRASTrStageRequest, cmRASParamSrcInfo, 0,
				sizeof(tSrcAlias), (s8*)&tSrcAlias) < 0 )
		{
			MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "Sending ARQ: Set cmRASParamSrcInfo failed\n" );
			cmRASClose( m_hsRas );
			return FALSE;
		}	
	}
	
	//set dst address
	if( tDestAlias.type == cmAliasTypeTransportAddress )
	{		
		if( cmRASSetParam( m_hsRas, cmRASTrStageRequest, cmRASParamDestCallSignalAddress, 0,
						   sizeof(tDestAlias.transport), (s8*)&tDestAlias.transport) < 0 )
		{
			MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "Sending ARQ: Set cmRASParamDestCallSignalAddress failed Dst ip=%u.%u.%u.%u port=%d\n", 
						  QUADADDR(tDestAlias.transport.ip), tDestAlias.transport.port );
			cmRASClose( m_hsRas );
			return FALSE;
		}
	}
	else
	{
		if( cmRASSetParam( m_hsRas, cmRASTrStageRequest, cmRASParamDestInfo, 0,
						   sizeof(tDestAlias), (s8*)&tDestAlias) < 0 )
		{
			MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "Sending ARQ: Set cmRASParamDestInfo failed\n" );
			cmRASClose( m_hsRas );
			return FALSE;
		}
	}
	
	if( cmRASSetParam( m_hsRas, cmRASTrStageRequest, cmRASParamCallModel, 0, 
					   (s32)cmCallModelTypeDirect, NULL ) < 0 )
	{
		MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "Sending ARQ: Set cmRASParamCallModel failed\n" );
		cmRASClose( m_hsRas );
		return FALSE;	
	}

	//convert call rate in units of bps
	//and double it (forward and reverse channels)
	nCallRate *= (1000 * 2);

	if( nCallRate && 
		cmRASSetParam( m_hsRas, cmRASTrStageRequest, cmRASParamBandwidth, 0, nCallRate, NULL) < 0)
	{
		MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "Sending ARQ: Set cmRASParamBandwidth failed nCallRate.%d \n", nCallRate);
		cmRASClose(m_hsRas);
		return FALSE;
	}

	if( cmRASRequest ( m_hsRas ) < 0 )
	{
		MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "Sending ARQ: cmRASRequest failed\n" );
		cmRASClose( m_hsRas );
		return FALSE;
	}
	
	return TRUE;
}


/*=============================================================================
  �� �� ���� DaemonProcSendRRQ
  ��    �ܣ� ע��MCU���ǻ������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2004/4        1.0			tanguang                ����
=============================================================================*/
BOOL32 CMtAdpInst::DaemonProcSendRRQ(CMessage * const pcMsg)
{
	u8 byConfIdx = 0;
	if( pcMsg )	
	{
		//�������
		CServMsg cServMsg( pcMsg->content, pcMsg->length );
		byConfIdx = cServMsg.GetConfIdx();
		
		//ֱ��ע��
		TMtAlias tAliasToReg;
		TMtAlias tH323Alias;
		if( byConfIdx < MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1 )
		{
			if (cServMsg.GetMsgBodyLen() < (2 * sizeof(TMtAlias) + sizeof(TTransportAddr)) ||
                (cServMsg.GetMsgBodyLen() - 2*sizeof(TMtAlias))%sizeof(TTransportAddr) != 0)
            {
                MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[DaemonProcSendRRQ] invalid msg body len :%d!\n", cServMsg.GetMsgBodyLen() );
                return FALSE;
            }

			if (0 != byConfIdx)
            {
				tAliasToReg = *(TMtAlias *)cServMsg.GetMsgBody();
            }
			else if (0 == byConfIdx)
            {
				tAliasToReg = *(TMtAlias *)cServMsg.GetMsgBody();
                tH323Alias  = *(TMtAlias *)(cServMsg.GetMsgBody() + sizeof(TMtAlias));
                g_cMtAdpApp.m_tMcuH323Id = tH323Alias;
                strncpy(g_cMtAdpApp.m_achMcuAlias, tH323Alias.m_achAlias, sizeof(g_cMtAdpApp.m_achMcuAlias)-1);
                g_cMtAdpApp.m_achMcuAlias[MAXLEN_ALIAS-1] = 0;
            }			
            
            //  xsl [6/7/2006] Э�������ַ
            memset( g_cMtAdpApp.m_atMtadpAddr, 0, sizeof(g_cMtAdpApp.m_atMtadpAddr) );
            memcpy( g_cMtAdpApp.m_atMtadpAddr, cServMsg.GetMsgBody() + 2*sizeof(TMtAlias), 
                    cServMsg.GetMsgBodyLen() - 2*sizeof(TMtAlias) );

			g_cMtAdpApp.m_atMcuAlias[byConfIdx] = tAliasToReg;
			
			//��״̬�����������:
			//1. ������ע�����뱾�жϣ�ʹע��״̬����Ϊ STATE_REG_INPROGRESS
			//2. ֻ�е�MCU��������ʱ���������һ��ע��GK��ʱ��Զ����ģ��ͻ���
            //	 �ָ���ʱ�䳤�ȣ���ͬE164�ŵĻ����ģ�岢����֪���Է���ע�������
			//	 ��ʱ���߾ͻᷢ����ռע�ᡣ������������Ĵ���ע�������ע����ƣ�
			//   ʹ��ͬE164�����ǣ����߾ͻ���ǰ�ߵ�GKע��״̬�����������E164
			//   �����նˣ��ܿ��ܾͻ�����GKע��״̬���Զ�����ʧ�ܡ���������ʹ��
			//	 ����������µ�GKע��״̬ʼ��ΪSTATE_REG_OK��ʵ���������롣[2006-03-13 zbq]
			if (STATE_REG_OK != g_cMtAdpApp.m_gkConfRegState[byConfIdx])
			{
				g_cMtAdpApp.m_gkConfRegState[byConfIdx] = STATE_REG_INPROGRESS;
			}
		}
		g_cMtAdpApp.ProcEnqueueRRQ(byConfIdx);
	}

    //��״̬Լ��������£�
    //1. ������ֻ�� N+1ģʽ����MCU�Ľ�����ϲ������壺ʹ���ڱ���MCU��������彫
    //   ��MCU��GK��ע���ʵ��ɹ�ע��֮ǰ������MCU���ɵ����е���MCU�Ļ����RRQ
    //   ����ȫ��ѹ�����������ȴ���
    //2. ֱ������MCU αװ����ע�� ��UCF�������ٽ�������Ļ���˳����RRQ��[12/21/2006-zbq]
    if ( g_cMtAdpApp.m_bURQForNPlus )
    {
        return FALSE;
    }
    
    //1. ��Լ��������ʵ���Ե����á�ֻ��Լ����RRQ�ļ����㡣�п�����Ҫ��������ע�����ȼ���
    //2. ���MCUע��ʧ�ܣ���û�б�Լ��������£������ע��Ҳ����������ע�ᡣ
    //   ��Լ��ʹ�ø������ֻ��ˢ��ע����������ע�ᡣ[03/16/2007-zbq]
    if ( 0 != byConfIdx &&
         MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 2 != byConfIdx &&
         STATE_REG_OK != g_cMtAdpApp.m_gkConfRegState[0] )
    {
        return FALSE;
    }
    
	if( g_cMtAdpApp.m_gkRegState == STATE_REG_INPROGRESS )
	{
		return FALSE;
	}
	if( g_cMtAdpApp.ProcDequeueRRQ(&byConfIdx) == FALSE )
	{
		return FALSE;
	}
	BOOL32 bKeepAliveRegistration  = FALSE;
	BOOL32 bRegisterAfterURQFromGK = FALSE; 	
	HRAS hsRas   = NULL;
	s32  nRRQRet = 0;

	if( g_cMtAdpApp.m_tGKAddr.ip == 0 )
	{
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[DaemonProcSendRRQ] RRQ failed due to GKIP(%u.%u.%u.%u)!\n", QUADADDR(g_cMtAdpApp.m_tGKAddr.ip) );
		return FALSE;
	}
	if( !g_cMtAdpApp.m_bGkAddrSet )
	{			
		if( !SetGKRasAddress(g_cMtAdpApp.m_tGKAddr) )		
		{			
			return FALSE;
		}	
	}
	
	//check register type:
	//confIdx == MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1: to update current registration;
	//confIdx == MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 2: re-register after URQ or RRJ of MCU from GK
	//confIdx <= MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE    : add a new registration	
	if( byConfIdx == MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1 )
	{
		bKeepAliveRegistration = TRUE;
	}
	else if( byConfIdx == MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 2 )
	{
		bRegisterAfterURQFromGK = TRUE;

		//���ԭ��GKID/EPID��Ϣ���Ա����»�ȡ
		g_cMtAdpApp.m_bAlreadyNtfGKEPID  = FALSE;
	}
	else if( byConfIdx < MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1 )
	{
		bKeepAliveRegistration = FALSE;
	}
	else	
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Invalid byConfIdx received in rrq, confidx :%d\n", byConfIdx );
        return FALSE;
    }		
		
	//start registration...

	//[1] get ras handle for the register

	//zbq[06/20/2008] RASʹ����ʽGK��ַ, ���������ڿ�������ʱ�������Э��ջpvtNode��ѯ
	nRRQRet = cmRASStartTransaction( g_cMtAdpApp.m_hApp, NULL, &hsRas, cmRASRegistration, &g_cMtAdpApp.m_tGKAddr, NULL );
	if( nRRQRet < 0 )
	{
        if( !bKeepAliveRegistration && !bRegisterAfterURQFromGK )
        {
			BuildAndSendMsgToMcu( MT_MCU_REGISTERGK_NACK );
        }
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "cmRASStartTransaction ConfIdx.%d failed in rrq! Ret=%d g_hApp=0x%x g_bGKAddrsSet=%d!\n", 
			            byConfIdx, nRRQRet, g_cMtAdpApp.m_hApp, g_cMtAdpApp.m_bGkAddrSet );	

        DaemonProcGetStackResFailed();
        
        //��ȡЭ��ջ��Դʧ�ܣ������屾������
        /*
        if( byConfIdx != 0 && byConfIdx < MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1 )
        {
			MtAdpInfo( "removed conf alias :%s success in rrq\n", g_cMtAdpApp.m_atMcuAlias[byConfIdx].m_achAlias );
			g_cMtAdpApp.m_gkConfRegState[byConfIdx] = STATE_NULL;
			memset( &(g_cMtAdpApp.m_atMcuAlias[byConfIdx]), 0, sizeof(g_cMtAdpApp.m_atMcuAlias[byConfIdx]) );            
        }*/              

        return FALSE;
	}
	
	//keep the handle for lookup in callback
	g_cMtAdpApp.m_hsRas[byConfIdx] = hsRas;

	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Now to register alias (%s) : \n", bKeepAliveRegistration ? "KeepAlive" : "Normal");

	u32 dwAliasIdx = 0;

    // zbq [10/20/2007] ������ע��ʱ���Ƿ��е���ʵ����Ҫ���·���ע��
    BOOL32 bRegLostConfIdx = FALSE;
	
    //[2.1] set h323id alias of MCU
    BOOL32 bBindMcu = TRUE;
    if ( bKeepAliveRegistration )
    {
        // zbq [10/20/2007] MCUע�᲻�ɹ�������MCU����������ע��
        if ( STATE_REG_OK != g_cMtAdpApp.m_gkConfRegState[0] )
        {
            bBindMcu = FALSE;
        }
    }
    if (bBindMcu)
	{
		cmAlias tAlias;
        s8 achNameBuf[256] = {0};
		memset((void*)&tAlias, 0, sizeof(tAlias));
		tAlias.string = achNameBuf;
		CMtAdpUtils::MtAliasIn2Out(g_cMtAdpApp.m_tMcuH323Id, tAlias);

		//coder restriction of MCU [byConfIdx == 0]
		if (tAlias.length >= 1 && tAlias.length <= 128)
		{
			if (cmRASSetParam(hsRas, cmRASTrStageRequest, cmRASParamTerminalAlias,
				dwAliasIdx, sizeof(tAlias), (s8*)&tAlias) < 0)
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "cmRASSetParam(cmRASParamTerminalAlias) ConfIdx.%d failed in DaemonProcSendRRQ\n", byConfIdx);
				cmRASClose(hsRas);
				g_cMtAdpApp.m_hsRas[byConfIdx] = NULL;
				if (!bKeepAliveRegistration && !bRegisterAfterURQFromGK)
				{
					BuildAndSendMsgToMcu(MT_MCU_REGISTERGK_NACK);
				}
				return FALSE;
			}
			else
			{
				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "\t<%d> %s\n", dwAliasIdx + 1, g_cMtAdpApp.m_tMcuH323Id.m_achAlias);
			}
			dwAliasIdx++;
		}
		else
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Invalid alias<len.%d> found when RRQing. Ignore it.\n", tAlias.length);
		}     
	}

	//[2.2] set e164 aliases of MCU and conf ( template )
	for( u32 dwConfNum = 0 ; dwConfNum < MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1; dwConfNum ++ )
	{
        // zbq [11/20/2007]�����MCUע�᲻�ɹ�������֮����������ע��
		if( (bKeepAliveRegistration && g_cMtAdpApp.m_gkConfRegState[dwConfNum] == STATE_REG_OK && byConfIdx < MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1 && dwConfNum == byConfIdx ) || 
            bRegisterAfterURQFromGK ||
            g_cMtAdpApp.m_gkConfRegState[dwConfNum] == STATE_REG_OK ||
            // ��Լ��ʵ��ע��ʵ��� ˳�򵥸� �󶨣�Ϊ������ ˳�򵥸� ע��ʧ�ܺ����������»���
		    (g_cMtAdpApp.m_gkConfRegState[dwConfNum] == STATE_REG_INPROGRESS && byConfIdx < MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1 && dwConfNum == byConfIdx) )
		{            
			if( (u8)mtAliasTypeBegin == g_cMtAdpApp.m_atMcuAlias[dwConfNum].m_AliasType )
				continue;
			
			cmAlias tAlias;
            s8 achNameBuf[256] = {0};
			memset( (void*)&tAlias, 0, sizeof(tAlias) );
			tAlias.string = achNameBuf;
			CMtAdpUtils::MtAliasIn2Out( g_cMtAdpApp.m_atMcuAlias[dwConfNum], tAlias );
			
			//PER coder's restriction: [1..128]
			if( tAlias.length < 1 || tAlias.length > 128 )
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Invalid alias found when RRQing. Ignore it.\n" );
				continue;
			}

			if( cmRASSetParam( hsRas, cmRASTrStageRequest, cmRASParamTerminalAlias,
							   dwAliasIdx, sizeof(tAlias), (s8*)&tAlias) < 0 )
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "cmRASSetParam(cmRASParamTerminalAlias) ConfIdx.%d failed in DaemonProcSendRRQ\n", byConfIdx );
				cmRASClose( hsRas );
				g_cMtAdpApp.m_hsRas[byConfIdx] = NULL;
				if( !bKeepAliveRegistration && !bRegisterAfterURQFromGK )
                {
                    BuildAndSendMsgToMcu( MT_MCU_REGISTERGK_NACK );
                }
				
                if( byConfIdx != 0 && byConfIdx < MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1 )
                {
                    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "removed conf alias :%s success in rrq\n", g_cMtAdpApp.m_atMcuAlias[byConfIdx].m_achAlias);
                    g_cMtAdpApp.m_gkConfRegState[byConfIdx] = STATE_NULL;
                    memset( &(g_cMtAdpApp.m_atMcuAlias[byConfIdx] ), 0, sizeof(g_cMtAdpApp.m_atMcuAlias[byConfIdx]));                    
                }             
				return FALSE;
			}
			else
			{
				MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "\t<%d> %s\n", dwAliasIdx + 1, tAlias.string );
			}
			dwAliasIdx ++ ;
		}

        // zbq[11/20/2007] ���Ѿ�ע��ʧ�ܵ�ʵ��������°�ע��
        // FIXME: START_REG_INPROGRESS ��ô����
        if ( bKeepAliveRegistration && STATE_NULL == g_cMtAdpApp.m_gkConfRegState[dwConfNum] )
        {
            if ( 0 == dwConfNum )
            {
                if (!g_cMtAdpApp.m_atMcuAlias[dwConfNum].IsNull() &&
                    !g_cMtAdpApp.m_tMcuH323Id.IsNull() &&
                    !g_cMtAdpApp.IsConfInQueue((u8)dwConfNum))
                {
                    g_cMtAdpApp.ProcEnqueueRRQ((u8)dwConfNum);
                    bRegLostConfIdx = TRUE;
                }
            }
            else
            {
                if (!g_cMtAdpApp.m_atMcuAlias[dwConfNum].IsNull()&&
                    !g_cMtAdpApp.IsConfInQueue((u8)dwConfNum))
                {
                    g_cMtAdpApp.ProcEnqueueRRQ((u8)dwConfNum);
                    bRegLostConfIdx = TRUE;
                }
            }
        }
	}

    if ( bRegLostConfIdx )
    {
        SetTimer( TIMER_REGGK_REQ, 3 * 1000 );
    }

    
    //zbq[01/09/2008] ��Լ���Ѿ�û������.��������������ϸԼ���»ᵼ��ֹͣע��.
    /*
	if( 0 == dwAliasIdx )
	{
		cmRASClose( hsRas );
		g_cMtAdpApp.m_hsRas[byConfIdx] = NULL;
        
		return FALSE;
	}*/

	//RAS��ַ����е�ַ�����ý���ʱ�ĵ�ַ����ֹǰ���������䣬���GK��Ӧ��Ϣ�޷��յ�
	//[3] set call ras address <ip, port>
	cmTransportAddress tRasAddr;
	tRasAddr.ip   = g_cMtAdpApp.m_dwMtAdpIpAddr;
	tRasAddr.port = g_cMtAdpApp.m_wRasPort;
	tRasAddr.type = cmTransportTypeIP;
	tRasAddr.distribution = cmDistributionUnicast;

	if( cmRASSetParam( hsRas, cmRASTrStageRequest, cmRASParamRASAddress, 
					   0, sizeof(tRasAddr), (s8*)&tRasAddr ) < 0 )
	{
		cmRASClose( hsRas );
		g_cMtAdpApp.m_hsRas[byConfIdx] = NULL;
		if( !bKeepAliveRegistration && !bRegisterAfterURQFromGK )
        {
            BuildAndSendMsgToMcu( MT_MCU_REGISTERGK_NACK );
        }
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "cmRASSetParam(cmRASParamRASAddress) ConfIdx.%d failed in DaemonProcSendRRQ\n", byConfIdx );
        
        if( byConfIdx != 0 && byConfIdx < MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1 )
        {
            MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "removed conf alias :%s success in rrq\n", g_cMtAdpApp.m_atMcuAlias[byConfIdx].m_achAlias );
            g_cMtAdpApp.m_gkConfRegState[byConfIdx] = STATE_NULL;
            memset( &(g_cMtAdpApp.m_atMcuAlias[byConfIdx]), 0, sizeof(g_cMtAdpApp.m_atMcuAlias[byConfIdx]) );            
        }               
			
		return FALSE;
	}

	//[4] set call signalling address <ip, port>
    for (u8 byIdx = 0; byIdx < MAXNUM_DRI; byIdx++)
    {
        if (g_cMtAdpApp.m_atMtadpAddr[byIdx].GetNetSeqIpAddr() == 0)
        {
            break;
        }

        cmTransportAddress tCallAddr;
        tCallAddr.ip   = g_cMtAdpApp.m_atMtadpAddr[byIdx].GetNetSeqIpAddr();
        tCallAddr.port = g_cMtAdpApp.m_atMtadpAddr[byIdx].GetPort();
        
        tCallAddr.type = cmTransportTypeIP;
        tCallAddr.distribution = cmDistributionUnicast;
        u32 dwCallIp = ntohl(tCallAddr.ip);
        if( cmRASSetParam( hsRas, cmRASTrStageRequest, cmRASParamCallSignalAddress,
            byIdx, sizeof(tCallAddr), (s8*)&tCallAddr ) < 0 )
        {
            cmRASClose(hsRas);
            g_cMtAdpApp.m_hsRas[byConfIdx] = NULL;
            if( !bKeepAliveRegistration && !bRegisterAfterURQFromGK )
            {
                BuildAndSendMsgToMcu( MT_MCU_REGISTERGK_NACK );
            }
            MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "cmRASSetParam(cmRASParamCallSignalAddress) ConfIdx.%d byIdx.%d MtAdpAddr:%u.%u.%u.%u:%d failed in DaemonProcSendRRQ\n",
                            byConfIdx, byIdx, QUADADDR(dwCallIp), tCallAddr.port );
            
            if( byConfIdx != 0 && byConfIdx < MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1 )
            {
                MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "removed conf alias :%s success in rrq\n", g_cMtAdpApp.m_atMcuAlias[byConfIdx].m_achAlias );
                g_cMtAdpApp.m_gkConfRegState[byConfIdx] = STATE_NULL;
                memset( &(g_cMtAdpApp.m_atMcuAlias[byConfIdx]), 0, sizeof(g_cMtAdpApp.m_atMcuAlias[byConfIdx]) );            
            }               
            
            return FALSE;
        }
        else
        {
            MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[DaemonProcSendRRQ] cmRASParamCallSignalAddress byIdx:%d, MtAdpAddr:%u.%u.%u.%u:%d success.\n",
                      byIdx, QUADADDR(dwCallIp), tCallAddr.port);
        }
    }	

	if( bKeepAliveRegistration )
	{
		//lightweight registration
		cmRASSetParam( hsRas, cmRASTrStageRequest, cmRASParamKeepAlive, 0,	TRUE, NULL );
	}
    
	// ��ʱֵ�趨Ϊ2����
	cmRASSetParam( hsRas, cmRASTrStageRequest, cmRASParamTimeToLive, 0, 120, NULL );

    //֧�ִ�Խ 
	if( cmRASSetParam( hsRas, cmRASTrStageRequest, cmRASParamFeatureSignalling, 0, 18, NULL ) < 0 )
    {
        MAPrint( LOG_LVL_ERROR, MID_MCULIB_MTADP, "[DaemonProcSendRRQ]set cmRASParamFeatureSignalling error\n");
    }

	if ( g_cMtAdpApp.m_byIsGkUseRRQPwd == 1)
	{
		cmRegKeyParam cRegParam;
		memcpy(&cRegParam.userName[0],g_cMtAdpApp.m_atMcuAlias[0].m_achAlias,sizeof(g_cMtAdpApp.m_atMcuAlias[0].m_achAlias));
		memcpy(&cRegParam.passWord[0],g_cMtAdpApp.m_achRRQPassword,sizeof(g_cMtAdpApp.m_achRRQPassword));
		cmRASSetRegKeyParam(hsRas, cmGetValTree( g_cMtAdpApp.m_hApp ),cRegParam);
	}

	//[5] judge the quality of handle, send RRQ, wait for ACF, the end.
	if( cmRASRequest(hsRas) < 0 )
	{
		cmRASClose( hsRas );
		g_cMtAdpApp.m_hsRas[byConfIdx] = NULL;
		if( !bKeepAliveRegistration && !bRegisterAfterURQFromGK )
        {
            BuildAndSendMsgToMcu( MT_MCU_REGISTERGK_NACK );
        }
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "cmRASRequest ConfIdx.%d failed in DaemonProcSendRRQ\n", byConfIdx );

        if( byConfIdx != 0 && byConfIdx < MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1 )
        {
            MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "removed conf alias :%s success in rrq\n", g_cMtAdpApp.m_atMcuAlias[byConfIdx].m_achAlias );
            g_cMtAdpApp.m_gkConfRegState[byConfIdx] = STATE_NULL;
            memset( &(g_cMtAdpApp.m_atMcuAlias[byConfIdx]), 0, sizeof(g_cMtAdpApp.m_atMcuAlias[byConfIdx]) );            
        }		
		return FALSE;
	}

	if(g_cMtAdpApp.m_hsCurrentRRQRas== NULL)
	{
		g_cMtAdpApp.m_hsCurrentRRQRas = hsRas;
		g_cMtAdpApp.m_byCurrentRRQConIdx = byConfIdx;
		
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP,"Record current RRQ ras<0x%x>,confidx:%d\n",g_cMtAdpApp.m_hsCurrentRRQRas,byConfIdx);
	}

	g_cMtAdpApp.m_gkRegState = STATE_REG_INPROGRESS;
	SetTimer(TIMER_RAS_RSP, RAS_RSP_TIMEOUT); 

	return TRUE;
}	

/*=============================================================================
  �� �� ���� DaemonProcSendURQ
  ��    �ܣ� ע��mcu��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg����Ϣ��ľ�ָ̬�� 
  �� �� ֵ�� BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2004/4        1.0			tanguang                  ����
=============================================================================*/
BOOL32 CMtAdpInst::DaemonProcSendURQ( CMessage * const pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8  byUnregItemNum = cServMsg.GetMsgBodyLen() / sizeof(TMtAlias);
	u8  byConfIdx      = cServMsg.GetConfIdx();	
	s32 nRet = 0;	

	HRAS hsRas = NULL;

    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "DaemonProcSendURQ confidx :%d, GkIP :%u.%u.%u.%u, bRegistered: %d \n",
               byConfIdx, QUADADDR(g_cMtAdpApp.m_tGKAddr.ip), g_cMtAdpApp.m_bGkRegistered );

	if( byConfIdx > MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1 || !g_cMtAdpApp.m_tGKAddr.ip )
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "invalid confidx received or GkIP = 0 in urq! confidx :%d, GkIP :%u.%u.%u.%u\n",
						byConfIdx, QUADADDR(g_cMtAdpApp.m_tGKAddr.ip));
		return FALSE;
	}		

	if( !g_cMtAdpApp.m_bGkAddrSet )
	{		
		SetGKRasAddress( g_cMtAdpApp.m_tGKAddr );
	}
	
	if( byConfIdx == 0 )
	{
		if( byUnregItemNum != 0 ) //unregister mcu alias, should not happen
		{
			for(s32 nIdx = 1; nIdx <= MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE; nIdx ++)
			{
				if(g_cMtAdpApp.m_atMcuAlias[nIdx].m_AliasType != 0)
				{
					MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "alias nIdx.%d 0 cannot not be unregistered.\n", nIdx);
					BuildAndSendMsgToMcu( MT_MCU_UNREGISTERGK_NACK );
					return FALSE;
				}
			}
		}	
	}
	else if( byConfIdx <= MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE )
	{		
		if( g_cMtAdpApp.m_atMcuAlias[ byConfIdx ].m_AliasType == 0 )
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "alias to unregister not found.\n");
			BuildAndSendMsgToMcu( MT_MCU_UNREGISTERGK_NACK );
			return FALSE;
		}
		else //nullify the alias
		{			
            MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "removed conf alias :%s success in urq\n", g_cMtAdpApp.m_atMcuAlias[byConfIdx].m_achAlias);
			memset( &g_cMtAdpApp.m_atMcuAlias[byConfIdx], 0, sizeof(TMtAlias));		
			g_cMtAdpApp.m_gkConfRegState[byConfIdx] = STATE_NULL;		            
		}
	}
	
	//start unregister ...

	//[1] get RAS handle of unregister
	
	//byConfIdx == 0				    means to unregister the last alias: mcu alias
	//byConfIdx == MAXNUM_MCU_CONF + 1  means to unregister the whole alias list
	if( byConfIdx == 0 || byConfIdx == MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1 )
	{
		//zbq[06/20/2008] RASʹ����ʽGK��ַ, ���������ڿ�������ʱ�������Э��ջpvtNode��ѯ
		nRet = cmRASStartTransaction(g_cMtAdpApp.m_hApp, NULL, &hsRas, cmRASUnregistration, &g_cMtAdpApp.m_tGKAddr, NULL);
	}
	else
	{
		//zbq[06/20/2008] RASʹ����ʽGK��ַ, ���������ڿ�������ʱ�������Э��ջpvtNode��ѯ
		nRet = cmRASStartTransaction(g_cMtAdpApp.m_hApp, NULL, &hsRas, cmRASRegistration, &g_cMtAdpApp.m_tGKAddr, NULL);
	}

	if( nRet < 0 )
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "cmRASStartTransaction failed in urq! err ret :%d\n", nRet );
		BuildAndSendMsgToMcu( MT_MCU_UNREGISTERGK_NACK );

        //��ȡЭ��ջ��Դʧ�ܣ����ӻָ�����
        DaemonProcGetStackResFailed();

		//����URQע��ʧ��ʱ��Ҳ֪ͨ�������½���
		if( (MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1) == byConfIdx )
		{
			g_cMtAdpApp.ClearMtAdpData();
			OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), MTADP_MCU_CONNECT, NULL, 0 );
			if (g_cMtAdpApp.m_bDoubleLink )
			{
				OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), MTADP_MCU_CONNECT2, NULL, 0 );
			}
		}
		return FALSE;
	}
	else //keep the handle for lookup in callback	
	{
		g_cMtAdpApp.m_hsRas[byConfIdx] = hsRas;
	}

	u32 dwAliasIdx = 0;

	//[2.1] set alias of MCU to be unregistered
	{
		cmAlias tAlias;
		s8   pchNameBuf[256] = { 0 };			
		memset( (void*)&tAlias, 0, sizeof(tAlias) );
		tAlias.string = pchNameBuf;
		CMtAdpUtils::MtAliasIn2Out( g_cMtAdpApp.m_tMcuH323Id, tAlias );
    
		//coder restriction of MCU
		if( tAlias.length >= 1 || tAlias.length <= 128 )
		{
			if( cmRASSetParam( hsRas, cmRASTrStageRequest, cmRASParamTerminalAlias,
				dwAliasIdx, sizeof(tAlias), (s8*)&tAlias) < 0 )
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "cmRASSetParam(cmRASParamTerminalAlias) failed in DaemonProcSendURQ\n");					
				cmRASClose( hsRas );
				g_cMtAdpApp.m_hsRas[byConfIdx] = NULL;					
				BuildAndSendMsgToMcu( MT_MCU_UNREGISTERGK_NACK );
            
				if( (MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1) == byConfIdx )
				{
					g_cMtAdpApp.ClearMtAdpData();
					OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), MTADP_MCU_CONNECT, NULL, 0 );
					if (g_cMtAdpApp.m_bDoubleLink )
					{
						OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), MTADP_MCU_CONNECT2, NULL, 0 );
					}
				}
				return FALSE;
			}
			dwAliasIdx++;
		}
		else
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Invalid MCU alias found when URQing. Ignore it.\n");
		}
	}

	//[2.2] set alias list to be unregistered
	for( u32 dwIdx = 0; dwIdx < MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1; dwIdx ++ )
	{
		if( g_cMtAdpApp.m_gkConfRegState[dwIdx] != STATE_NULL && 
			g_cMtAdpApp.m_atMcuAlias[dwIdx].m_AliasType != 0 )				
		{			
			cmAlias tAlias;
			s8   pchNameBuf[256] = { 0 };			
			memset( (void*)&tAlias, 0, sizeof(tAlias) );
			tAlias.string = pchNameBuf;
			CMtAdpUtils::MtAliasIn2Out( g_cMtAdpApp.m_atMcuAlias[dwIdx], tAlias );
			
			//PER coder's restriction: [1..128]
			if( tAlias.length < 1 || tAlias.length > 128 )
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Invalid alias found when URQing. Ignore it.\n");
				continue;
			}
			
			if( cmRASSetParam( hsRas, cmRASTrStageRequest, cmRASParamTerminalAlias,
				dwAliasIdx, sizeof(tAlias), (s8*)&tAlias) < 0 )
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "cmRASSetParam(cmRASParamTerminalAlias) failed in DaemonProcSendURQ\n");					
				cmRASClose( hsRas );
				g_cMtAdpApp.m_hsRas[byConfIdx] = NULL;					
				BuildAndSendMsgToMcu( MT_MCU_UNREGISTERGK_NACK );

				if( (MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1) == byConfIdx )
				{
					g_cMtAdpApp.ClearMtAdpData();
					OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), MTADP_MCU_CONNECT, NULL, 0 );
					if (g_cMtAdpApp.m_bDoubleLink )
					{
						OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), MTADP_MCU_CONNECT2, NULL, 0 );
					}
				}
				return FALSE;
			}
			dwAliasIdx++;
		}
	}		

	//URQ ��������ras addr, RRQ������ras addr 2005-8-24
	if( 0 != byConfIdx && byConfIdx <= MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE )
	{
		//RAS��ַ����е�ַ�����ý���ʱ�ĵ�ַ����ֹǰ���������䣬���GK��Ӧ��Ϣ�޷��յ�
		//[3] set call ras address <ip, port> for single URQ
		cmTransportAddress tRasAddr;
		tRasAddr.ip   = g_cMtAdpApp.m_dwMtAdpIpAddr;
		tRasAddr.port = g_cMtAdpApp.m_wRasPort;
		tRasAddr.type = cmTransportTypeIP;
		tRasAddr.distribution = cmDistributionUnicast;

		if( cmRASSetParam( hsRas, cmRASTrStageRequest, cmRASParamRASAddress, 
						   0, sizeof(tRasAddr), (s8*)&tRasAddr ) < 0 )
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "cmRASSetParam(cmRASParamRASAddress) failed in DaemonProcSendURQ\n");					
			g_cMtAdpApp.m_gkConfRegState[byConfIdx] = STATE_NULL;
			cmRASClose( hsRas );
			BuildAndSendMsgToMcu( MT_MCU_UNREGISTERGK_NACK );

			if( (MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1) == byConfIdx )
			{
				g_cMtAdpApp.ClearMtAdpData();
				OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), MTADP_MCU_CONNECT, NULL, 0 );
				if (g_cMtAdpApp.m_bDoubleLink )
				{
					OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), MTADP_MCU_CONNECT2, NULL, 0 );
				}
			}
			return FALSE;
		}
	}
	
	//[3'] set call signalling address <ip, port>
    for (u8 byIdx = 0; byIdx < MAXNUM_DRI; byIdx++)
    {
        if (g_cMtAdpApp.m_atMtadpAddr[byIdx].GetNetSeqIpAddr() == 0)
        {
            break;
        }
        
        cmTransportAddress tCallAddr;
        tCallAddr.ip   = g_cMtAdpApp.m_atMtadpAddr[byIdx].GetNetSeqIpAddr();
        tCallAddr.port = g_cMtAdpApp.m_atMtadpAddr[byIdx].GetPort();
        tCallAddr.type = cmTransportTypeIP;
        tCallAddr.distribution = cmDistributionUnicast;
        u32 dwCappIp = ntohl(tCallAddr.ip);
        if( cmRASSetParam( hsRas, cmRASTrStageRequest, cmRASParamCallSignalAddress,
            0, sizeof(tCallAddr), (s8*)&tCallAddr ) < 0 )
        {
            MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "cmRASSetParam(cmRASParamCallSignalAddress) failed in DaemonProcSendURQ\n");					
            g_cMtAdpApp.m_gkConfRegState[byConfIdx] = STATE_NULL;
            cmRASClose( hsRas );
            BuildAndSendMsgToMcu( MT_MCU_UNREGISTERGK_NACK );
            
            if( (MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1) == byConfIdx )
            {
                g_cMtAdpApp.ClearMtAdpData();
                OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), MTADP_MCU_CONNECT, NULL, 0 );
                if (g_cMtAdpApp.m_bDoubleLink )
                {
                    OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), MTADP_MCU_CONNECT2, NULL, 0 );
                }
            }
            return FALSE;
        }
        else
        {
            MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[DaemonProcSendURQ] cmRASParamCallSignalAddress byIdx:%d, MtAdpAddr:%u.%u.%u.%u:%d success.\n",
                byIdx, QUADADDR(dwCappIp), tCallAddr.port);
        }
    }

	//[4] send out request
	if( cmRASRequest( hsRas ) < 0)
	{		
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "cmRASRequest failed in DaemonProcSendURQ\n" );					
		cmRASClose( hsRas );
		BuildAndSendMsgToMcu( MT_MCU_UNREGISTERGK_NACK );

		if( (MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1) == byConfIdx )
		{
			g_cMtAdpApp.ClearMtAdpData();
			OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), MTADP_MCU_CONNECT, NULL, 0 );
			if (g_cMtAdpApp.m_bDoubleLink )
			{
				OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), MTADP_MCU_CONNECT2, NULL, 0 );
			}
		}
		return FALSE;
	}
	
	g_cMtAdpApp.m_gkRegState = STATE_REG_INPROGRESS;
	return TRUE;
}

/*=============================================================================
    �� �� ���� DaemonProcResendRRQReq
    ��    �ܣ� ���½���RRQע�������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CMessage * const pcMsg
    �� �� ֵ�� BOOL32
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/12   3.6			����                  ����
=============================================================================*/
/*BOOL32 CMtAdpInst::DaemonProcResendRRQReq(CMessage * const pcMsg)
{	
	g_cMtAdpApp.m_gkRegState = STATE_NULL;
	g_cMtAdpApp.m_bGkRegistered = FALSE; 	
	g_cMtAdpApp.m_bGotRRJOrURQFromGK = TRUE;
			
	CServMsg cServMsg;
	cServMsg.SetConfIdx( MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 2 ); // re-registration
	cServMsg.SetEventId( MCU_MT_REGISTERGK_REQ );
	
	post( MAKEIID( AID_MCU_MTADP, CInstance::DAEMON ), 
		  MCU_MT_REGISTERGK_REQ, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	MtAdpInfo( "DaemonProcResendRRQReq \n" );

	return TRUE;
}*/

/*=============================================================================
    �� �� ���� DaemonProcUpdateRRQNtf
    ��    �ܣ� ����������MCU������ʵ���GKע����Ϣ��֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CMessage * const pcMsg
    �� �� ֵ�� BOOL32
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/12   3.6			����                  ����
=============================================================================*/
BOOL32 CMtAdpInst::DaemonProcUpdateRRQNtf(CMessage * const pcMsg)
{
	if( NULL == pcMsg )	
	{
		return FALSE;
	}

	CServMsg cServMsg( pcMsg->content, pcMsg->length );
		
	s32 nAliasNum = cServMsg.GetMsgBodyLen()/(sizeof(TMtAlias)+sizeof(u8)+sizeof(u8));
	TMtAlias *ptAliasToReg = (TMtAlias*)cServMsg.GetMsgBody();
	u8 *pbyConfIdx  = (u8*)( ptAliasToReg + 1 );
	u8 *pbyRegState = (u8*)( pbyConfIdx + 1 );

	g_cMtAdpApp.m_bMasterMtAdp = FALSE;

    u8 byOffSet = sizeof(TMtAlias) + sizeof(u8) + sizeof(u8);

	for( u8 byNum = 0; byNum < nAliasNum; byNum ++ )
	{
		if( *pbyConfIdx < MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1 )
		{
			g_cMtAdpApp.m_atMcuAlias[*pbyConfIdx] = *ptAliasToReg;
			if( 1 == *pbyRegState ) //0��δע�ᣬ1���ɹ�ע��
			{
				g_cMtAdpApp.m_gkConfRegState[*pbyConfIdx] = STATE_REG_OK;
			}
			else
			{
				g_cMtAdpApp.m_gkConfRegState[*pbyConfIdx] = STATE_NULL;
			}
		}

		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[DaemonProcUpdateRRQNtf] ConfIdx.%d RegState.%d\n", *pbyConfIdx, *pbyRegState );

		ptAliasToReg = (TMtAlias*)( (u8*)ptAliasToReg + byOffSet );
		pbyConfIdx   = pbyConfIdx + byOffSet;
		pbyRegState  = pbyRegState + byOffSet;
	}

    // zbq [05/22/2007] ����ע��״̬��ͬ��������MCU��ע��״̬����һ��
	if( STATE_REG_OK == g_cMtAdpApp.m_gkConfRegState[0] )
	{
		g_cMtAdpApp.m_bGkRegistered = TRUE;
		g_cMtAdpApp.m_gkRegState    = STATE_REG_OK;
	}
	else
	{
        g_cMtAdpApp.m_bGkRegistered = FALSE;
        g_cMtAdpApp.m_gkRegState    = STATE_NULL;
	}

	return TRUE;
}

/*=============================================================================
    �� �� ���� DaemonProcUpdateGKANDEPIDNtf
    ��    �ܣ� ����������GatekeeperID/EndpointID��Ϣ��֪ͨ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CMessage * const pcMsg
    �� �� ֵ�� BOOL32
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/12   3.6			����                  ����
=============================================================================*/
BOOL32 CMtAdpInst::DaemonProcUpdateGKANDEPIDNtf(CMessage * const pcMsg)
{
	if( NULL == pcMsg )	
	{
		return FALSE;
	}
	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[DaemonProcUpdateGKANDEPIDNtf] bMasterMtAdp.%d\n", g_cMtAdpApp.m_bMasterMtAdp );

	CServMsg cServMsg( pcMsg->content, pcMsg->length );
		
	//Save GatekeeperID/EndpointID Info
	TH323EPGKIDAlias *ptEPGKIDAlias = (TH323EPGKIDAlias*)cServMsg.GetMsgBody();
	memcpy( (void*)&g_cMtAdpApp.m_tGKID, (void*)ptEPGKIDAlias, sizeof(TH323EPGKIDAlias));
	ptEPGKIDAlias++;
	memcpy( (void*)&g_cMtAdpApp.m_tEPID, (void*)ptEPGKIDAlias, sizeof(TH323EPGKIDAlias));

	g_cMtAdpApp.m_bMasterMtAdp = FALSE;

	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[DaemonProcUpdateGKANDEPIDNtf] GKID.m_ntype.%d GKID.m_wlength.%d EPID.m_ntype.%d EPID.m_wlength.%d \n", 
				g_cMtAdpApp.m_tGKID.GetIDtype(), g_cMtAdpApp.m_tGKID.GetIDlength(), 
				g_cMtAdpApp.m_tEPID.GetIDtype(), g_cMtAdpApp.m_tEPID.GetIDlength() );
	return TRUE;
}

/*=============================================================================
  �� �� ���� ProcSendBRQ
  ��    �ܣ� �����������ı�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� s32 nRate, �ı�������
             u8 byDirection�����е���? ����ת�� ?
  �� �� ֵ�� BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2004/4        1.0			tanguang                  ����
=============================================================================*/
BOOL32 CMtAdpInst::ProcSendBRQ( s32 nRate, u8 byDirection )
{
	if( !g_cMtAdpApp.m_tH323Config.IsManualRAS() || 
		!g_cMtAdpApp.m_bGkAddrSet || !m_hsCall || !nRate )
	{		
		return FALSE;
	}
	HRAS hsRas;

	//zbq[06/20/2008] RASʹ����ʽGK��ַ, ���������ڿ�������ʱ�������Э��ջpvtNode��ѯ
	if( cmRASStartTransaction( g_cMtAdpApp.m_hApp, NULL, &hsRas, cmRASBandwidth, &g_cMtAdpApp.m_tGKAddr, m_hsCall ) < 0 )
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcSendBRQ] -- cmRASStartTransaction error!\n" );

        //��ȡЭ��ջ��Դʧ�ܣ����ӻָ�����
        DaemonProcGetStackResFailed();

		return FALSE;
	}
	//convert call rate in units of bps
	//and double it (forward and reverse channels)
	//���������ɹ���Ҫ��˫���϶�����Ӧ�����ʵĵ���
	nRate *= (1000 * 2);

	cmRASSetParam( hsRas, cmRASTrStageRequest, cmRASParamBandwidth, 0, nRate, NULL );

	if( FALSE == g_cMtAdpApp.m_bMasterMtAdp )
	{
		s32 nRet = 0;
/*
		//Set GatekeeperID
		cmAlias tGKID;
		tGKID.type   = (cmAliasType)g_cMtAdpApp.m_tGKID.GetIDtype();
		tGKID.length = (UINT16)g_cMtAdpApp.m_tGKID.GetIDlength();
		tGKID.pnType = (cmPartyNumberType)g_cMtAdpApp.m_tGKID.GetIDpnType();
		tGKID.string = g_cMtAdpApp.m_tGKID.GetIDAlias();
		memcpy( (void*)&tGKID.transport, (void*)g_cMtAdpApp.m_tGKID.GetIDtransport(), sizeof(TH323TransportAddress));		

		nRet = cmRASSetParam( hsRas, cmRASTrStageRequest, cmRASParamGatekeeperID, 0,
						      sizeof(tGKID),(s8*)&(tGKID) );
		if( nRet < 0 )
		{
			MtAdpException( "[ProcSendBRQ] -- set optional cmRASParamGatekeeperID error nRet=%d!\n", nRet);
			cmRASClose(m_hsRas);
			return FALSE;
		}
*/
		//Set EndpointID
		cmAlias tEPID;
		tEPID.type   = (cmAliasType)g_cMtAdpApp.m_tEPID.GetIDtype();
		tEPID.length = (UINT16)g_cMtAdpApp.m_tEPID.GetIDlength();
		tEPID.pnType = (cmPartyNumberType)g_cMtAdpApp.m_tEPID.GetIDpnType();
		tEPID.string = g_cMtAdpApp.m_tEPID.GetIDAlias();
		memcpy((void*)&tEPID.transport, (void*)g_cMtAdpApp.m_tEPID.GetIDtransport(), sizeof(TH323TransportAddress));
		
		nRet = cmRASSetParam( hsRas, cmRASTrStageRequest, cmRASParamEndpointID, 0,
					          sizeof(tEPID), (s8*)&(tEPID) );
		if( nRet < 0 )
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcSendBRQ] -- set optional cmRASParamEndpointID error nRet=%d!\n", nRet);
			cmRASClose( m_hsRas );
			return FALSE;
		}
	}

	if( byDirection == (u8)CALL_INCOMING )
	{
		cmRASSetParam( hsRas, cmRASTrStageRequest, cmRASParamAnsweredCall, 0, TRUE, NULL ); 
	}
	else
	{
		cmRASSetParam( hsRas, cmRASTrStageRequest, cmRASParamAnsweredCall, 0, FALSE, NULL ); 
	}
	
	if( cmRASRequest( hsRas ) < 0)
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcSendBRQ] -- cmRASRequest error!\n");
		cmRASClose( hsRas );
		return FALSE;
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� DaemonProcDisengageOnGK
  ��    �ܣ� ��GK��ע��һ�����У��ͷ�����ռ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2004/4        1.0			tanguang              ����
=============================================================================*/
BOOL32 CMtAdpInst::DaemonProcDisengageOnGK()
{
	if( !g_cMtAdpApp.m_tH323Config.IsManualRAS() || 
		!g_cMtAdpApp.m_bGkAddrSet || !m_hsCall )
	{
		return FALSE;
	}

	HRAS hsRas;
	
	//zbq[06/20/2008] RASʹ����ʽGK��ַ, ���������ڿ�������ʱ�������Э��ջpvtNode��ѯ
	//start a DRQ transaction associated with m_hsCall
	if( cmRASStartTransaction( g_cMtAdpApp.m_hApp, NULL, &hsRas, cmRASDisengage, &g_cMtAdpApp.m_tGKAddr, m_hsCall ) < 0 )
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[DaemonProcDisengageOnGK] -- cmRASStartTransaction error!\n" );
        
        //��ȡЭ��ջ��Դʧ�ܣ����ӻָ�����
        DaemonProcGetStackResFailed();
		
        return FALSE;
	}

	if( FALSE == g_cMtAdpApp.m_bMasterMtAdp )
	{
		s32 nRet = 0;	
/*
		//Set GatekeeperID
		cmAlias tGKID;
		tGKID.type   = (cmAliasType)g_cMtAdpApp.m_tGKID.GetIDtype();
		tGKID.length = (UINT16)g_cMtAdpApp.m_tGKID.GetIDlength();
		tGKID.pnType = (cmPartyNumberType)g_cMtAdpApp.m_tGKID.GetIDpnType();
		tGKID.string = g_cMtAdpApp.m_tGKID.GetIDAlias();
		memcpy( (void*)&tGKID.transport, (void*)g_cMtAdpApp.m_tGKID.GetIDtransport(), sizeof(TH323TransportAddress));		

		nRet = cmRASSetParam( hsRas, cmRASTrStageRequest, cmRASParamGatekeeperID, 0,
						      sizeof(tGKID),(s8*)&(tGKID) );
		if( nRet < 0 )
		{
			MtAdpException( "[DaemonProcDisengageOnGK] -- set optional cmRASParamGatekeeperID error nRet=%d!\n", nRet);
			cmRASClose(m_hsRas);
			return FALSE;
		}
*/
		//Set EndpointID
		cmAlias tEPID;
		tEPID.type   = (cmAliasType)g_cMtAdpApp.m_tEPID.GetIDtype();
		tEPID.length = (UINT16)g_cMtAdpApp.m_tEPID.GetIDlength();
		tEPID.pnType = (cmPartyNumberType)g_cMtAdpApp.m_tEPID.GetIDpnType();
		tEPID.string = g_cMtAdpApp.m_tEPID.GetIDAlias();
		memcpy( (void*)&tEPID.transport, (void*)g_cMtAdpApp.m_tEPID.GetIDtransport(), sizeof(TH323TransportAddress));
		
		nRet = cmRASSetParam( hsRas, cmRASTrStageRequest, cmRASParamEndpointID, 0, sizeof(tEPID), (s8*)&(tEPID) );
		if( nRet < 0 )
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[DaemonProcDisengageOnGK] -- set optional cmRASParamEndpointID error nRet=%d!\n", nRet );
			cmRASClose( m_hsRas );
			return FALSE;
		}
	}

	if( cmRASRequest(hsRas) < 0 )
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[DaemonProcDisengageOnGK] -- cmRASRequest error!\n");
		cmRASClose( hsRas );
		return FALSE;
	}

	return TRUE;
}

/*=============================================================================
  �� �� ���� ProcSendIRR
  ��    �ܣ� ��ʱ����IRR
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2004/4        1.0			tanguang                  ����
=============================================================================*/
BOOL32 CMtAdpInst::ProcSendIRR()
{
	if( !g_cMtAdpApp.m_tH323Config.IsManualRAS() || 
		!g_cMtAdpApp.m_bGkAddrSet || 
		!m_hsCall ) 

		return FALSE;

	HRAS hsRas;

	//zbq[06/20/2008] RASʹ����ʽGK��ַ, ���������ڿ�������ʱ�������Э��ջpvtNode��ѯ
	//start a IRR transaction associated with m_hsCall
	if( cmRASStartTransaction( g_cMtAdpApp.m_hApp, NULL, &hsRas, cmRASUnsolicitedIRR, &g_cMtAdpApp.m_tGKAddr, m_hsCall) < 0 )
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcSendIRR] -- cmRASStartTransaction error!\n" );

        //��ȡЭ��ջ��Դʧ�ܣ����ӻָ�����
        DaemonProcGetStackResFailed();
        // xsl [7/30/2005] 
        SetTimer( TIMER_IRR, m_nIrrFrequency * 1000 );
        
		return FALSE;
	}
	
	if( FALSE == g_cMtAdpApp.m_bMasterMtAdp )
	{
		s32 nRet = 0;
/*		
		//Set GatekeeperID
		cmAlias tGKID;
		tGKID.type   = (cmAliasType)g_cMtAdpApp.m_tGKID.GetIDtype();
		tGKID.length = (UINT16)g_cMtAdpApp.m_tGKID.GetIDlength();
		tGKID.pnType = (cmPartyNumberType)g_cMtAdpApp.m_tGKID.GetIDpnType();
		tGKID.string = g_cMtAdpApp.m_tGKID.GetIDAlias();
		memcpy( (void*)&tGKID.transport, (void*)g_cMtAdpApp.m_tGKID.GetIDtransport(), sizeof(TH323TransportAddress));		

		HRAS as = m_hsRas;
		nRet = cmRASSetParam( hsRas, cmRASTrStageRequest, cmRASParamGatekeeperID, 0,
						      sizeof(tGKID),(s8*)&(tGKID) );
		if( nRet < 0 )
		{
			MtAdpException( "[ProcSendIRR] -- set optional cmRASParamGatekeeperID error nRet=%d!\n", nRet);
			cmRASClose(hsRas);
			return FALSE;
		}
*/
		//Set EndpointID
		cmAlias tEPID;
		tEPID.type   = (cmAliasType)g_cMtAdpApp.m_tEPID.GetIDtype();
		tEPID.length = (UINT16)g_cMtAdpApp.m_tEPID.GetIDlength();
		tEPID.pnType = (cmPartyNumberType)g_cMtAdpApp.m_tEPID.GetIDpnType();
		tEPID.string = g_cMtAdpApp.m_tEPID.GetIDAlias();
		memcpy( (void*)&tEPID.transport, (void*)g_cMtAdpApp.m_tEPID.GetIDtransport(), sizeof(TH323TransportAddress));
		
		nRet = cmRASSetParam( hsRas, cmRASTrStageRequest, cmRASParamEndpointID, 0,
					          sizeof(tEPID),(s8*)&(tEPID) );
		if( nRet < 0 )
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcSendIRR] -- set optional cmRASParamEndpointID error nRet=%d!\n", nRet );
			cmRASClose( hsRas );
			
            // xsl [7/30/2005] 
            SetTimer( TIMER_IRR, m_nIrrFrequency * 1000);

			return FALSE;
		}
	}

	if( cmRASRequest(hsRas) < 0 )
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcSendIRR] -- cmRASRequest error!\n" );
		cmRASClose( hsRas );
		
        // xsl [7/30/2005] 
        SetTimer( TIMER_IRR, m_nIrrFrequency * 1000 );

		return FALSE;
	}

	//close the transaction here for we do not want response
	cmRASClose( hsRas );

	SetTimer( TIMER_IRR, m_nIrrFrequency * 1000); 
	return TRUE;
}


/*=============================================================================
  �� �� ���� ProcOnRecvDRQ
  ��    �ܣ� ����Э��ջ�����ĹҶ�����( h_ras_DRQ )
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TDRQINFO *ptDRQINF
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2004/4        1.0			tanguang                  ����
=============================================================================*/
void CMtAdpInst::ProcOnRecvDRQ( TDRQINFO *ptDRQINF )
{
    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "receive drq from stack.\n");
	
// 	//091210ע��codian��gk���յ�DRQ�������ն˹Ҷ�
// 	return;
	//  [12/11/2009 pengjie] Modify ͨ�������ļ������Ƿ���ӦDRQ
	if( !g_cMtAdpApp.m_bIsRespDRQFromGK )
	{
		StaticLog( "[MtAdplib][ProcOnRecvDRQ] Recv DRQ From GK, But DebugFile Set Don't Respond DRQ \n" );
		return;
	}
	// End

	u16 wEvent = MT_MCU_MTDISCONNECTED_NOTIF;

	CServMsg cServMsg;

	if( ptDRQINF->GetDRQReason() == cmRASDisengageReasonForcedDrop )
	{
        cServMsg.SetErrorCode( (u16)ERR_MCU_RAS_GK_REJECT );
	}
	else if( ptDRQINF->GetDRQReason() == cmRASDisengageReasonNormalDrop )
	{
		// ���ڸ�ԭ��Ϊ��Ӧ�ն˾ܾ�,��֪GK����
		// ����ȴ��ն˵ĶϿ���Ϣ,֪ͨ����ܾ�ԭ����������
		cServMsg.SetErrorCode( (u16)ERR_MCU_RAS_NORMALDROP );
		return;
	}
	else if( ptDRQINF->GetDRQReason() == cmRASDisengageReasonUndefinedReason )
	{
        cServMsg.SetErrorCode( (u16)ERR_MCU_RAS_UNDEFINED );
	}

	cServMsg.SetEventId( MT_MCU_CALLMTFAILURE_NOTIFY );
	cServMsg.SetMsgBody( NULL, 0 );
	SendMsgToMcu( cServMsg );
	
	m_bIsRecvDRQFromGK = TRUE;

	//���������ı��ж˹Ҷ���ͨ�� h225��releaseComplete������ͬʱ���յ�DRQ���������Ҷ�
	//�ն˶ϵ���������ֻ�нϳ�ʱ��GK��ʱ(KDCGKΪ2����)���DRQ֪ͨ����ʱһ��roundtrip��ʱ�����쳣�Ҷ�
	//������ڿ��ܵ�����...............................
	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcOnRecvDRQ] ClearInst.\n");
	if( m_byTimeoutTimes >= 2 )
	{
		ClearInst( wEvent, (u8)emDisconnectReasonDRQ );
	}
	else
	{
		ClearInst( wEvent, (u8)emDisconnectReasonNormal );	
	}

	return;
}


/*=============================================================================
  �� �� ���� DaemonProcGkMcuGeneral
  ��    �ܣ� ��������ؾ���Э��ջ��mcu�Ļ�����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  CMessage * const  pcMsg
             CApp* pcApp
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2004/4        1.0  		tanguang                  ����
=============================================================================*/
void CMtAdpInst::DaemonProcGkMcuGeneral( CMessage * const  pcMsg, CApp* pcApp )
{	
	if( !pcMsg || !pcApp )
	{
		return;
	}
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	//HAPPRAS haRas  = *(HAPPRAS*)cServMsg.GetMsgBody();
	HRAS hsRas     = *(HRAS*)( cServMsg.GetMsgBody() + sizeof(HAPPRAS) );
	u16  wMsgType  = *(u16*)( cServMsg.GetMsgBody() + sizeof(HAPPRAS) + sizeof(HRAS) );
	u8  *pbyMsg    =  cServMsg.GetMsgBody() + sizeof(HAPPRAS) + sizeof(HRAS) + sizeof(u16);
	u8   byConfIdx = 0;
	
	HRAS hsLastRRQRas = g_cMtAdpApp.m_hsCurrentRRQRas;
	u8 byLastRRQConIdx = g_cMtAdpApp.m_byCurrentRRQConIdx;
	if( (wMsgType == (u16)h_ras_RCF || wMsgType == (u16)h_ras_RRJ )
		&& (hsRas == hsLastRRQRas && hsRas!=NULL))
	{
		//�ϴ�RRQ�����յ���Ӧ�������¼��RAS���[12/26/2012 chendaiwei]
		g_cMtAdpApp.m_hsCurrentRRQRas = NULL;
		g_cMtAdpApp.m_byCurrentRRQConIdx = 0xFF;
	}

	if( wMsgType == (u16)h_ras_RCF || 
		wMsgType == (u16)h_ras_RRJ || 
		wMsgType == (u16)h_ras_UCF || 
		wMsgType == (u16)h_ras_URJ  )
	{
		for( byConfIdx = 0; byConfIdx < MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 3; byConfIdx ++ )
		{
			if( g_cMtAdpApp.m_hsRas[byConfIdx] == hsRas )
			{
				g_cMtAdpApp.m_hsRas[byConfIdx] = NULL;
				break;
			}
		}
		if( byConfIdx == MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 3 )
		{
			if(wMsgType == (u16)h_ras_RRJ && hsRas == hsLastRRQRas && hsRas!=NULL)
			{
				u16 wSize = cServMsg.GetMsgBodyLen()-sizeof(HAPPRAS)-sizeof(HRAS)-sizeof(u16);
				s32 nReason = -1;
				if( wSize == sizeof(cmRASReason))
				{
					nReason = *(s32*)pbyMsg;
				}
				
				byConfIdx = byLastRRQConIdx;
				MAPrint(LOG_LVL_KEYSTATUS,MID_MCULIB_MTADP,"Receive RRJ<Ras:0x%x,ConfIdx:%d> possibly due to GK is down!Reason.%d!\n",hsLastRRQRas,byConfIdx,nReason);
			}
			else
			{
				return;	
			}
		}
	}
	
	CServMsg cReplyMsg;
	cReplyMsg.SetConfIdx( byConfIdx );
	cReplyMsg.SetSrcDriId( g_cMtAdpApp.m_byDriId );
	
	cmAlias tGKID;
	cmAlias tEPID;
	s8      abyAlias[256] = { 0 };
	s32     nAliasLen     = 0;
	BOOL32  bNtfEPGKID    = FALSE;
	BOOL32  bGetEPGKIDOK  = TRUE;
    BOOL32  bMcuRcfAfterUrq = FALSE;
	s32     nRet = 0;

	switch( wMsgType ) 
	{
	case h_ras_RCF:

		KillTimer( TIMER_RAS_RSP );
		if( byConfIdx <= MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE &&
			(u8)mtAliasTypeBegin == g_cMtAdpApp.m_atMcuAlias[byConfIdx].m_AliasType )
		{
			cReplyMsg.SetConfIdx( byConfIdx );
			cReplyMsg.SetEventId( MT_MCU_UNREGISTERGK_ACK );
		}
		else
		{	
			switch( byConfIdx ) 
			{
			case MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 2: //all available aliases registered successfully
				{
					for( u32 dwConfID = 0; dwConfID < MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1; dwConfID ++ )
					{
						if( (u8)mtAliasTypeBegin != g_cMtAdpApp.m_atMcuAlias[dwConfID].m_AliasType )
						{
							g_cMtAdpApp.m_gkConfRegState[dwConfID] = STATE_REG_OK;
							cReplyMsg.SetConfIdx( byConfIdx );// xsl [3/18/2005] re-registered succeed, report to mcu
							cReplyMsg.SetEventId( MT_MCU_REGISTERGK_ACK );

							if (SendMsgToMcuInst(cReplyMsg))
							{
								MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "mtadp -- > vc : %u: %s Conf.%d\n", cReplyMsg.GetEventId(), 
										   OspEventDesc(cReplyMsg.GetEventId()), dwConfID );
							}
							else
							{
								MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Send %u(%s) Conf.%d to Mcu failed!\n", cServMsg.GetEventId(),
												OspEventDesc(cServMsg.GetEventId()), dwConfID );
							}
						}
						else
						{
							g_cMtAdpApp.m_gkConfRegState[dwConfID] = STATE_NULL;
						}
					}
					cReplyMsg.SetConfIdx( 0 );// xsl [3/18/2005] re-registered succeed, report to mcu
					cReplyMsg.SetEventId( MT_MCU_REGISTERGK_ACK );

				}                
				break;
				
			case MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1: //lightweight registration succeeded
				cReplyMsg.SetEventId(0);
				break;

			default:
				if( byConfIdx <= MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE )
				{
					g_cMtAdpApp.m_gkConfRegState[byConfIdx] = STATE_REG_OK;
					cReplyMsg.SetEventId( MT_MCU_REGISTERGK_ACK );
				}
				break;
			}
		}

        // zbq [11/20/2007] �����MCUע��ʧ�ܺ��ע��ɹ�������������ע��һ��
        if ( 0 == byConfIdx && g_cMtAdpApp.m_bGotRRJOrURQFromGK )
        {
            bMcuRcfAfterUrq = TRUE;
        }
				        
        //������������£�
        //  1. m_bGotRRJOrURQFromGK ��ʶ�����rrj����urq�����·�������ע�����λ��
        //                �� lightweight ע��ظ��� rcf û��ֱ�ӹ�ϵ���ʻرܴ˸�λ��
        //  2. �� GK æ������£�һ�� lw ע��� rcf �ظ����ӳٵ�����������һ������
        //     ע���ĳʵ��ĵ���ע��Ķ�ʱ�ȴ��ڼ�Ѹ��ֶθ�λ�����³�MCU�������
        //     ʵ��û��ʵ������ע�ᣬ�������� lw ά���׶Σ����º����ն�ʧ�ܵȵ�һϵ�����⡣[2006-10-12 zbq]
        if ( byConfIdx != MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1 )
        {
            g_cMtAdpApp.m_bGotRRJOrURQFromGK = FALSE;
        }
        g_cMtAdpApp.m_gkRegState    = STATE_REG_OK;
        g_cMtAdpApp.m_bGkRegistered = TRUE;

		DaemonProcSendRRQ(NULL);

        
		if( TRUE == g_cMtAdpApp.m_bMasterMtAdp )
		{
            if ( !bMcuRcfAfterUrq )
            {
                SetTimer( TIMER_REGGK_REQ, INTERVAL_REG_GK * 1000 );
            }
            else
            {
                SetTimer( TIMER_REGGK_REQ, 10 );
            }
		}

		//Get GatekeeperID
		tGKID.string = abyAlias;
		tGKID.length = LEN_256;
		nRet = cmRASGetParam( hsRas, cmRASTrStageConfirm, cmRASParamGatekeeperID,0, &nAliasLen, (s8*)&tGKID );
		if( nRet < 0 || tGKID.length > LEN_256 )
		{
			bGetEPGKIDOK = FALSE;
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[GetRRQInfo] -- get optional cmRASParamGatekeeperID error wGKIDLen=%d nRet=%d!\n", tGKID.length, nRet );
		}
		else
		{
			if( g_cMtAdpApp.m_tGKID.GetIDtype() != (s32)tGKID.type )
			{
				g_cMtAdpApp.m_tGKID.SetIDtype((s32)tGKID.type);
				bNtfEPGKID = TRUE;
			}
			if( g_cMtAdpApp.m_tGKID.GetIDlength() != (u16)tGKID.length )
			{
				g_cMtAdpApp.m_tGKID.SetIDlength((u16)tGKID.type);
				bNtfEPGKID = TRUE;
			}
			if( g_cMtAdpApp.m_tGKID.GetIDpnType() != (s32)tGKID.pnType )
			{
				g_cMtAdpApp.m_tGKID.SetIDpnType((s32)tGKID.pnType);
				bNtfEPGKID = TRUE;
			}
			if( 0 != memcmp( (void*)g_cMtAdpApp.m_tGKID.GetIDtransport(), 
					         (void*)&tGKID.transport, sizeof(TH323TransportAddress) ) )
			{

				TH323TransportAddress tTransportAddress;
				memcpy(&tTransportAddress,&tGKID.transport,sizeof(TH323TransportAddress));
				g_cMtAdpApp.m_tGKID.SetIDtransport(&tTransportAddress);
				bNtfEPGKID = TRUE;
			}
			if( 0 != memcmp( g_cMtAdpApp.m_tGKID.GetIDAlias(), abyAlias, LEN_256 ) )
			{
				g_cMtAdpApp.m_tGKID.SetIDAlias(abyAlias);
				bNtfEPGKID = TRUE;
			}	
		}

		//Get EndpointID
		memset( abyAlias, 0, sizeof(abyAlias) );
		tEPID.string = abyAlias;
		tEPID.length = LEN_256;
		nRet = cmRASGetParam( hsRas, cmRASTrStageConfirm, cmRASParamEndpointID,0, &nAliasLen, (s8*)&tEPID );
		if( nRet < 0 || tEPID.length > LEN_256 )
		{
			bGetEPGKIDOK = FALSE;
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[GetRRQInfo] -- get optional cmRASParamEndpointID error wEPIDLen=%d nRet=%d!\n", tEPID.length, nRet );
		}
		else
		{
			if( g_cMtAdpApp.m_tEPID.GetIDtype() != (s32)tEPID.type )
			{
				g_cMtAdpApp.m_tEPID.SetIDtype((s32)tEPID.type);
				bNtfEPGKID = TRUE;
			}
			if( g_cMtAdpApp.m_tEPID.GetIDlength() != (u16)tEPID.length )
			{
				g_cMtAdpApp.m_tEPID.SetIDlength((u16)tEPID.length);
				bNtfEPGKID = TRUE;
			}
			if( g_cMtAdpApp.m_tEPID.GetIDpnType() != (s32)tEPID.pnType )
			{
				g_cMtAdpApp.m_tEPID.SetIDpnType((s32)tEPID.pnType);
				bNtfEPGKID = TRUE;
			}
			if( 0 != memcmp( (void*)g_cMtAdpApp.m_tEPID.GetIDtransport(), 
					         (void*)&tEPID.transport, sizeof(TH323TransportAddress) ) )
			{
				TH323TransportAddress tTransportAddress;
				memcpy(&tTransportAddress,&tEPID.transport,sizeof(TH323TransportAddress));

				g_cMtAdpApp.m_tEPID.SetIDtransport(&tTransportAddress);
				bNtfEPGKID = TRUE;
			}
			if( 0 != memcmp( g_cMtAdpApp.m_tEPID.GetIDAlias(), abyAlias, LEN_256 ) )
			{
				g_cMtAdpApp.m_tEPID.SetIDAlias(abyAlias);
				bNtfEPGKID = TRUE;
			}
		}
		//֪ͨ�ϲ� GatekeeperID/EndpointID
		if( (TRUE == bGetEPGKIDOK) && 
			(TRUE == bNtfEPGKID || FALSE == g_cMtAdpApp.m_bAlreadyNtfGKEPID) )
		{			
			g_cMtAdpApp.m_bAlreadyNtfGKEPID = TRUE;

			CServMsg cNtfEPGKIDMsg;
			cNtfEPGKIDMsg.SetEventId( MT_MCU_UPDATE_GKANDEPID_NTF );
			cNtfEPGKIDMsg.SetConfIdx( byConfIdx );
			cNtfEPGKIDMsg.SetMsgBody( (u8*)&g_cMtAdpApp.m_tGKID, sizeof(g_cMtAdpApp.m_tGKID) );
			cNtfEPGKIDMsg.CatMsgBody( (u8*)&g_cMtAdpApp.m_tEPID, sizeof(g_cMtAdpApp.m_tEPID) );
			
			if (SendMsgToMcuInst(cNtfEPGKIDMsg))
			{
				MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[GetRRQInfo] mtadp:%s GKID.m_ntype.%d GKID.m_wlength.%d EPID.m_ntype.%d EPID.m_wlength.%d \n", OspEventDesc(cReplyMsg.GetEventId()), 
							g_cMtAdpApp.m_tGKID.GetIDtype(), g_cMtAdpApp.m_tGKID.GetIDlength(), 
							g_cMtAdpApp.m_tEPID.GetIDtype(), g_cMtAdpApp.m_tEPID.GetIDlength() );
			}
			else
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[GetRRQInfo] Send %u(%s) to Mcu failed!\n", cNtfEPGKIDMsg.GetEventId(),
								 OspEventDesc(cServMsg.GetEventId()) );
			}
		}
		
		break;
		  
	case h_ras_RRJ:
		{
			KillTimer( TIMER_RAS_RSP );
			cmRASReason emFailReason;
			s32  nReason = -1;
			s8   achReasonBuf[128];

			if( cmRASGetParam(hsRas, cmRASTrStageReject, cmRASParamRejectReason,
				0, (s32*)&emFailReason, NULL) >= 0 )
			{
				nReason = (s32)emFailReason;
			}

			switch(nReason) 
			{
			case cmRASReasonDuplicateAlias: 
				cReplyMsg.SetErrorCode( (u16)ERR_MCU_RAS_DUPLICATE_ALIAS );
				break;

			case cmRASReasonUndefined:
				cReplyMsg.SetErrorCode( (u16)ERR_MCU_RAS_UNDEFINED );
				break;	
 
   			case -1:
				cReplyMsg.SetErrorCode( (u16)ERR_MCU_RAS_TIMEOUT );				
				break;
				
			default:
				cReplyMsg.SetErrorCode( (u16)ERR_MCU_RAS_UNDEFINED );				
			}

			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "RRJ reason: %s, byConfIdx: %d\n", 
				CMtAdpUtils::PszGetRASReasonName( nReason, achReasonBuf, sizeof(achReasonBuf) ), byConfIdx );
			
			//lightweight registration failed. Probably due to UDP 
			//packets lost, the GK rebooted, or something else 
			//wrong happened... just prepare for another try.
			if( byConfIdx == MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1 )
			{
				for( u32 dwConfNum = 0; dwConfNum < MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1; dwConfNum ++ )
				{
					g_cMtAdpApp.m_gkConfRegState[dwConfNum] = STATE_NULL;
				}

				g_cMtAdpApp.m_gkRegState = STATE_REG_OK;
				g_cMtAdpApp.m_bGkRegistered = FALSE;
				g_cMtAdpApp.m_bGotRRJOrURQFromGK = TRUE;//tell timer to issue a new RRQ

				if( TRUE == g_cMtAdpApp.m_bMasterMtAdp )
				{
					SetTimer( TIMER_REGGK_REQ, 3 * 1000 );
				}

				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Lightweight registration rejected by GK.\n");				
				
				// xsl [3/18/2005] ��ΪЭ��ջ��ʱֵ����keeplive��ʱֵ��
				// �����ڵ�һ��keeplive rrq��ʱ����ǰ��keeplive rrq��ѻ���
				// ���²��ܷ���������ע�ᣬmcu״̬Ҳ�ò�������
				cReplyMsg.SetConfIdx( 0 );
				cReplyMsg.SetEventId( MT_MCU_REGISTERGK_NACK );
			}
			//regular registration/unregistration or re-registration
			else// confIdx < MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1 || confIdx == MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 2
			{
				if( byConfIdx < MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1 )
				{
					cReplyMsg.SetConfIdx( byConfIdx );
					if(g_cMtAdpApp.m_atMcuAlias[byConfIdx].m_AliasType != 0)
						cReplyMsg.SetEventId( MT_MCU_REGISTERGK_NACK );
					else
						cReplyMsg.SetEventId( MT_MCU_UNREGISTERGK_NACK );
				}
				else // xsl [3/18/2005] re-register failed, report to mcu
				{
					cReplyMsg.SetConfIdx( 0 );
					cReplyMsg.SetEventId( MT_MCU_REGISTERGK_NACK );
				}					
				
				//the 0th alias should never be cleared, since the
				//mcu vc will not re-issue it in later conference RRQ's

                if ( -1 != nReason )
                {
                    //MCU ע��ʧ�ܣ������MCU����������ע��
                    if ( byConfIdx == 0 )
                    {
                        MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "mcu alias :%s register failed, retry ...\n", g_cMtAdpApp.m_atMcuAlias[0].m_achAlias);
                        g_cMtAdpApp.m_gkConfRegState[0] = STATE_NULL;
                    }
                    //����ע��ʧ��
                    else if ( byConfIdx < MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1  )
                    {
                        //���MCUע��Ҳʧ�ܣ��Ȳ���ջ���������ȴ�MCUע��ɹ�, ���鱾��ע��ʧ�ܣ���ջ������
                        if ( g_cMtAdpApp.m_gkConfRegState[0] != STATE_NULL ) 
                        {
                            MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "removed conf alias :%s success in rrj, byConfIdx: %d !\n", g_cMtAdpApp.m_atMcuAlias[byConfIdx].m_achAlias, byConfIdx);
                            memset( &(g_cMtAdpApp.m_atMcuAlias[byConfIdx]), 0, sizeof(g_cMtAdpApp.m_atMcuAlias[byConfIdx]));                            
                        }
                        g_cMtAdpApp.m_gkConfRegState[byConfIdx] = STATE_NULL;
                    }
                }
                else
                {
                    //�������ע�ᳬʱ���򲻽�����գ���������ע��
                }

				g_cMtAdpApp.m_gkRegState = STATE_NULL;						
				g_cMtAdpApp.m_bGotRRJOrURQFromGK = TRUE; //tell timer to issue a new RRQ
				
				//  [12/15/2009 pengjie] Modify, Bug00024657 �ж�������������GK��RRQ
				if( TRUE == g_cMtAdpApp.m_bMasterMtAdp )
				{
					MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Registration rejected by GK.\n" );
					SetTimer( TIMER_REGGK_REQ, 3 * 1000 );
					
					//MCUע��ʧ�ܣ�����������ע�ᶼ�������ȴ�MCUע��ɹ�
					if ( g_cMtAdpApp.m_gkConfRegState[0] == STATE_REG_OK )
					{
						DaemonProcSendRRQ( NULL );
					}
				}
				// End Modify
			}		
		}
		break;
		
	case h_ras_UCF:
    case h_ras_URJ:	
		{
			g_cMtAdpApp.m_bMasterMtAdp = TRUE; //  [12/18/2009 pengjie] ���յ�UCF��MTadp��������

            if ( g_cMtAdpApp.m_bWaitingURQRsp )
            {
                KillTimer(TIMER_URQ_RSP);
                MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[GeneralProc] normal clear URQRspTimer\n" );
                g_cMtAdpApp.m_bWaitingURQRsp = FALSE;

                //FIXME: û����N+1��N+1��urq�ͱ�urq����ϸ��ʺ�С����ʹ��ϰ���urq�߼�ͳһ����.
                
                //��·�Ѿ����õȴ�
                if ( OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNode) &&
                     OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNodeB) )
                {
                    //������ע��
                    ProcMtAdpReRegisterMcu(/* pcMsg, */pcApp );
                    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[GeneralProc] normal clear URQRspTimer and start register directly\n" );
                }
                else
                {
                    //FIXME�������Ƿ�Ҫ���������ע���������ݲ�����.

                    //�ǳ���������²�ʹ��ֻ��һ����·��������ɾ�����������. ֻ����΢�Բ�סmp��prs��.
                    //���߸ɴ඼û���ü����������ˣ�������������.
                    BOOL32 bConnectMcuA = FALSE;
                    BOOL32 bConnectMcuB = FALSE;
                    if ( OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNode))
                    {
                        bConnectMcuA = TRUE;
                        OspDisconnectTcpNode(g_cMtAdpApp.m_dwMcuNode);
                    }
                    if ( OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNodeB))
                    {
                        bConnectMcuB = TRUE;
                        OspDisconnectTcpNode(g_cMtAdpApp.m_dwMcuNodeB);
                    }
                    
                    g_cMtAdpApp.ClearMtAdpData();
                    OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), MTADP_MCU_CONNECT, NULL, 0 );
                    if (g_cMtAdpApp.m_bDoubleLink )
                    {
                        OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), MTADP_MCU_CONNECT2, NULL, 0 );
                    }
                    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[GeneralProc] normal clear URQRspTimer and restart connect for LinkA.%d, LinkB.%d ...\n", bConnectMcuA, bConnectMcuB );
                }
            }
            else
            {
                // N+1 ģʽ��αװע����MCU������ʵ��ɹ����ڱ��˲�������ע�ᣬֻ�ͷ�Լ��������RRQ��[12/21/2006-zbq]
                if ( g_cMtAdpApp.m_bURQForNPlus )
                {
                    g_cMtAdpApp.m_bURQForNPlus = FALSE;
                    DaemonProcSendRRQ( NULL );
                }
                else
                {
                    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[GeneralProc] %s might be delayed\n", PszRasCtrlTypeName(wMsgType) );
                }
            }
		}
		break;	
		
	case h_ras_ACF:
		{
			TACFINFO *ptAcfInfo;
			HCALL hsCall = *(HCALL*)pbyMsg;		
			ptAcfInfo = (TACFINFO*)(pbyMsg + sizeof(hsCall));
			cmTransportAddress tTransportAddr;
			tTransportAddr.ip = ptAcfInfo->m_tCalledAddr.GetIPAddr();
			tTransportAddr.port = ptAcfInfo->m_tCalledAddr.GetIPPort();
			s32 nIrrFrequency = ptAcfInfo->m_nIRRFrequency;

			s32 nApprovedBandwidth = ptAcfInfo->m_nBandWidth;

			HAPPCALL haCall = NULL;
			kdvCallGetHandle( hsCall, &haCall );
            /*lint -save -e507*/
			u16 dstInst = (u16)haCall;
            /*lint -restore*/
			if( pcApp->GetInstance(dstInst) != NULL )
			{
				((CMtAdpInst*)pcApp->GetInstance(dstInst))->CallProceed(&tTransportAddr, nApprovedBandwidth, nIrrFrequency);				
			}
			else
			{
				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Failed to find the dest inst (%u) for stack event\n", dstInst );
			}
		}
		break;
		
	case h_ras_ARJ:
		{
			cmRASReason emFailReason = cmRASReasonCalledPartyNotRegistered;
			s32 nReason = -1;
			s8  achReasonBuf[128];

			if( cmRASGetParam( hsRas, cmRASTrStageReject, cmRASParamRejectReason, 
							   0, (s32*)&emFailReason, NULL ) >= 0 )
			{
				nReason = (s32)emFailReason;
			}
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP,"ARJ reason = %s\n", CMtAdpUtils::PszGetRASReasonName( nReason, achReasonBuf, sizeof(achReasonBuf) ));
			
			HCALL	 hsCall = *(HCALL*)pbyMsg;				
			HAPPCALL haCall = NULL;
			kdvCallGetHandle( hsCall, &haCall );			
            /*lint -save -e507*/
            u16 dstInst = (u16)haCall;
            /*lint -restore*/
			if( pcApp->GetInstance(dstInst) != NULL )
			{
				( (CMtAdpInst*)pcApp->GetInstance(dstInst) )->CallProceed( NULL, 0 );
			}
			else
			{
				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Failed to find the dest inst (%u) for stack event\n", dstInst);
			}
		}
		break; 

	case  h_ras_BCF:
	case  h_ras_BRJ:
		{
			HCALL hsCall = *(HCALL*)pbyMsg;
			u32 dwBitRate = 0;
			
			if(!hsCall)
				break;

			if( wMsgType == (u16)h_ras_BCF)
			{
				dwBitRate = *(u32*)(pbyMsg + sizeof(HCALL));
				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "BCF: %d Kbps\n", dwBitRate);
			}
			else
			{
				cmRASReason emFailReason;
				s32 nReason = -1;
				s8  achReasonBuf[128];
				
				if(cmRASGetParam( hsRas, cmRASTrStageReject, cmRASParamRejectReason,
								  0, (s32*)&emFailReason, NULL) >= 0 )
				{
					nReason = (s32)emFailReason;
				}
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP,"BRJ reason = %s\n",	CMtAdpUtils::PszGetRASReasonName( nReason, achReasonBuf, sizeof(achReasonBuf) ));
			}

			HAPPCALL haCall = NULL;
			kdvCallGetHandle( hsCall, &haCall );
            
            /*lint -save -e507*/
			u16 dstInst = (u16)haCall;
            /*lint -restore*/			
			if( pcApp->GetInstance(dstInst) != NULL)
			{
				((CMtAdpInst*)pcApp->GetInstance(dstInst))->FlowControlProceed(dwBitRate);				
			}
			else
			{
				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Failed to find the dest inst (%u) for stack event BCF\n", dstInst);
			}
		}
		break;

	case  h_ras_DRQ:
		{
			HCALL    hsCall = *(HCALL*)pbyMsg;				
			HAPPCALL haCall = NULL;
			kdvCallGetHandle( hsCall, &haCall );
            /*lint -save -e507*/
            u8 dstInst = (u8)haCall;
            /*lint -restore*/
			TDRQINFO *ptDRQINF = (TDRQINFO *)(pbyMsg + sizeof(hsCall));
			if(pcApp->GetInstance(dstInst) != NULL)
			{
				((CMtAdpInst*)pcApp->GetInstance(dstInst))->ProcOnRecvDRQ( ptDRQINF );
			}
			else
			{
				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Failed to find the dest inst (%u) for stack event\n", dstInst);
			}
		}
		break;
	
	case h_ras_URQ:
		{
			//���˿��ܵ�GK���߲�ѯ
			if( TRUE == g_cMtAdpApp.m_bMasterMtAdp )
			{
				//clear the states, but keep the mtAlias array unchanged for re-registration
				for( u32 dwConfNum = 0; dwConfNum < MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1; dwConfNum ++ )
				{
					g_cMtAdpApp.m_gkConfRegState[dwConfNum] = STATE_NULL;
				}

				g_cMtAdpApp.m_gkRegState    = STATE_NULL;
				g_cMtAdpApp.m_bGkRegistered = FALSE; 
				
				//tell timer to issue a new RRQ. 
				g_cMtAdpApp.m_bGotRRJOrURQFromGK = TRUE; 

				//�ظ�GK, ADAPTER��ʱ�ر���ؾ��
				kdvSendRasCtrlMsg( NULL, hsRas, (u16)h_ras_UCF, NULL, 0 );
				
				SetTimer( TIMER_REGGK_REQ, 5 * 1000 );

				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Unregistration from GK.\n");	
			}
			else
			{
				StaticLog( "h_ras_URQ bMasterMtAdp.%d return\n", g_cMtAdpApp.m_bMasterMtAdp );	
			}
		}
		break;

	default:
		break;
	}

	if( cReplyMsg.GetEventId() != 0 && 
		( wMsgType == (u16)h_ras_RCF || wMsgType == (u16)h_ras_RRJ || 
		  wMsgType == (u16)h_ras_UCF || wMsgType == (u16)h_ras_URJ ) )
	{
		if (SendMsgToMcuInst(cReplyMsg))
		{
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "mtadp -- > vc : %u: %s\n", cReplyMsg.GetEventId(), 
						OspEventDesc(cReplyMsg.GetEventId()));
		}
		else
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Send %u(%s) to Mcu failed!\n", cServMsg.GetEventId(),
							OspEventDesc(cServMsg.GetEventId()));
		}
	}
	
	//����ras����رղ��ԣ�2005-11-02
	//����Щras��������Ϣ���ϲ����ras����رգ�adapter��������ras��Ϣ�����Զ��رա�
	if ( wMsgType == (u16)h_ras_ARQ || wMsgType == (u16)h_ras_BRQ || wMsgType == (u16)h_ras_DRQ || 
		 wMsgType == (u16)h_ras_LRQ || wMsgType == (u16)h_ras_RRQ || wMsgType == (u16)h_ras_URQ )
	{
		cmRASClose(hsRas);
	}

	return;
}

/*=============================================================================
  �� �� ���� DaemonProcGkAddrUpdateCmd
  ��    �ܣ� GK��ַ�����������Ӧ			
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/2/9      4.0			�ű���                  ����
=============================================================================*/
BOOL32 CMtAdpInst::DaemonProcUpdateGkAddrCmd( /*CMessage * const pcMsg,*/ CApp *pcApp )
{	
	//��Ϊ���������ԭ���Ѿ��ɹ�ע��gk����ע��gk
	if ( g_cMtAdpApp.m_bMasterMtAdp && 0 != g_cMtAdpApp.m_tGKAddr.ip && g_cMtAdpApp.m_bGkRegistered )
	{
		//ע������ԭ�е���Ϣ
		CServMsg cSerMsg;
		cSerMsg.SetConfIdx(0);
		cSerMsg.SetEventId( MCU_MT_UNREGISTERGK_REQ );
		OspPost( MAKEIID( AID_MCU_MTADP, CInstance::DAEMON ), MCU_MT_UNREGISTERGK_REQ, 
									  cSerMsg.GetServMsg(), cSerMsg.GetServMsgLen() );        
	}
    else
    {
        //��mcu������ע�ᣬ��ack������Ƿ���������gk��ַ����Ϣ
        ProcMtAdpReRegisterMcu( /*pcMsg,*/ pcApp );
    }
		
	return TRUE;
}

/*=============================================================================
  �� �� ���� ProcMtAdpReRegisterMcu
  ��    �ܣ� ����������MCU���·�����ע��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/2/9      4.0			�ű���                  ����
=============================================================================*/
void CMtAdpInst::ProcMtAdpReRegisterMcu( /*CMessage * const pcMsg,*/ CApp* pcApp )
{
    //�������ʵ��״̬
    for ( s32 nInst = 1; nInst <= MAXNUM_DRI_MT; nInst ++ )
    {
        CMtAdpInst *pcMtAdpInst;
        pcMtAdpInst = (CMtAdpInst*)pcApp->GetInstance(nInst);
        pcMtAdpInst->ClearInst(MT_MCU_MTDISCONNECTED_NOTIF, (u8)emDisconnectReasonDRQ);
    }

    KillTimer(TIMER_REGGK_REQ);

    //����ͨѶ��������
    u32 dwMcuNodeA = g_cMtAdpApp.m_dwMcuNode;
    u32 dwMcuNodeB = g_cMtAdpApp.m_dwMcuNodeB;
    u32 dwMcuIIdA  = g_cMtAdpApp.m_dwMcuIId;
    u32 dwMcuIIdB  = g_cMtAdpApp.m_dwMcuIIdB;
    
    //��չ���������
    g_cMtAdpApp.ClearMtAdpData();
    
    //�ָ�ͨѶ��������
    g_cMtAdpApp.m_dwMcuNode = dwMcuNodeA;
    g_cMtAdpApp.m_dwMcuNodeB = dwMcuNodeB;
    g_cMtAdpApp.m_dwMcuIId = dwMcuIIdA;
    g_cMtAdpApp.m_dwMcuIIdB = dwMcuIIdB;

	//������ע��
	OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), MTADP_MCU_REGISTER_REQ, NULL, 0 );
	if (g_cMtAdpApp.m_bDoubleLink )
	{
		OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), MTADP_MCU_REGISTER_REQ2, NULL, 0 );
	}
    
	return;
}

/*=============================================================================
  �� �� ���� DaemonProcConnectToGK
  ��    �ܣ� ��GK����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/11/09    4.0			�ű���                  ����
=============================================================================*/
void CMtAdpInst::DaemonProcConnectToGK()
{
    if ( 0 == g_cMtAdpApp.m_tGKAddr.ip ) 
    {
        return;
    }

	for( u16 wActualGkPort = PORT_GK+1; wActualGkPort>=PORT_GK; wActualGkPort--)
	{
		u32 dwNode = (u32)OspConnectTcpNode( g_cMtAdpApp.m_tGKAddr.ip, wActualGkPort );
		if ( OspIsValidTcpNode(dwNode) )
		{
			g_cMtAdpApp.m_dwGKNode = dwNode;
			OspNodeDiscCBRegQ(dwNode, GetAppID(), GetInsID());
			OspPost( MAKEIID( AID_MCU_MTADP, CInstance::DAEMON ), MTADP_GK_REGISTER );
			
			StaticLog("[MtAdp] Connect to GK %u.%u.%u.%u:%u succeed, start register\n", 
				QUADADDR(g_cMtAdpApp.m_tGKAddr.ip), wActualGkPort );

			return;
		}
	}

	SetTimer( MTADP_GK_CONNECT, CONNECTING_GK_INTERVAL );
	StaticLog("[MtAdp] Connecting to GK %u.%u.%u.%u:%u \n", QUADADDR(g_cMtAdpApp.m_tGKAddr.ip), PORT_GK );

    return;
}

/*=============================================================================
  �� �� ���� DaemonProcRegToGK
  ��    �ܣ� ��GKע��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/11/10    4.0			�ű���                  ����
=============================================================================*/
void CMtAdpInst::DaemonProcRegToGK( /*CMessage * const pcMsg*/ )
{
    if ( !OspIsValidTcpNode(g_cMtAdpApp.m_dwGKNode) ) 
    {
        SetTimer( MTADP_GK_CONNECT, CONNECTING_GK_INTERVAL );
        return;
    }

#ifdef _UTF8
    //[4/15/2013 liaokang] ���뷽ʽ
    u8 byMcuEncoding = g_cMtAdpApp.GetMcuEncoding();
    s32 nRet = post( MAKEIID( AID_GK_RADIUS_CLIENT, CInstance::DAEMON ), 
		EV_MG_CONF_ACCTING_REG, &byMcuEncoding, sizeof(u8), g_cMtAdpApp.m_dwGKNode );
#else
	s32 nRet = post( MAKEIID( AID_GK_RADIUS_CLIENT, CInstance::DAEMON ), 
		EV_MG_CONF_ACCTING_REG, NULL, 0, g_cMtAdpApp.m_dwGKNode );
#endif
	
    if ( OSP_OK != nRet )
    {
        StaticLog( "[MtAdp] send reg msg to GK(%u.%u.%u.%u(node.%d)) failed !\n", 
                                   QUADADDR(g_cMtAdpApp.m_tGKAddr.ip), g_cMtAdpApp.m_dwGKNode );
    }
    else
    {
        StaticLog( "[MtAdp] registering to GK, waiting ... !\n" );
    }
    SetTimer( MTADP_GK_REGISTER, REGISTERING_GK_INTERVAL );

    return;
}

/*=============================================================================
  �� �� ���� DaemonProcRegGKRsp
  ��    �ܣ� ��GKע���Ӧ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/11/10    4.0			�ű���                  ����
=============================================================================*/
void CMtAdpInst::DaemonProcRegGKRsp( CMessage * const pcMsg )
{
    KillTimer( MTADP_GK_REGISTER );
    g_cMtAdpApp.m_dwGKIId = pcMsg->srcid;
    g_cMtAdpApp.m_bGKReged = TRUE;

    //[4/15/2013 liaokang] ���뷽ʽĬ��ΪGBK
    g_cMtAdpApp.SetGkEncoding(emenCoding_GBK);
#ifdef _UTF8
    if(  NULL != pcMsg
		&& pcMsg->content != NULL
		&& pcMsg->length > 0 )
    {
        u8 byGKEncoding = *(u8*)pcMsg->content;
        g_cMtAdpApp.SetGkEncoding((emenCodingForm)byGKEncoding);
    }
#endif

    // zbq [03/13/2007] ���ݹ���������������GK������ע��ɹ����Ƿ���ԭ�����ڼƷѵĻ��顣�����¼Ʒѣ����� �ۼӻ�����
    for(u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++)
    {
		TConfChargeInfo * ptChargeInfo = &g_cMtAdpApp.m_atChargeInfo[byConfIdx-1];
        if ( !ptChargeInfo->IsNull() && g_cMtAdpApp.m_bMasterMtAdp )
        {
            GkChargeStart( *ptChargeInfo, byConfIdx);
        }
    }
	
	// �����Ƿ��п�ʼ�ƷѵĻ��飬��������Ʒ�״̬������Ϣ��ʱ
	SetTimer( TIMER_CONFCHARGE_STATUS_NTF, CONF_CHANGE_NTF_INTERVAL );

    // zbq [03/27/2007] GKע��ɹ���״̬�ϱ�MCU
    CServMsg cRegRsp;
    cRegRsp.SetEventId( MT_MCU_CHARGE_REGGK_NOTIF );
    SendMsgToMcu( cRegRsp );

    MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[DaemonProcRegGKRsp] registering to GK succeed !\n" );
    return;
}

/*=============================================================================
  �� �� ���� DaemonDisconnectFromGK
  ��    �ܣ� ����GK�Ʒ���·��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/11/09    4.0			�ű���                  ����
=============================================================================*/
void CMtAdpInst::DaemonDisconnectFromGK()
{
    g_cMtAdpApp.m_dwGKNode = INVALID_NODE;
    g_cMtAdpApp.m_dwGKIId  = 0;
    g_cMtAdpApp.m_bGKReged = FALSE;
    //g_cMtAdpApp.m_bIsGKCharge = FALSE;
    
    /*
    //֪ͨ�ϲ�ҵ���Ƴ����ڸ�GK�ϼƷѵĻ�����ն�
    TConfChargeInfo *ptChargeInfo = &g_cMtAdpApp.m_atChargeInfo[0];
    for( u8 byConfIdx = 1; byConfIdx <= MAX_CONFIDX; byConfIdx ++ )
    {
        if ( !ptChargeInfo[byConfIdx-1].IsNull() ) 
        {
            CServMsg cServMsg;
            m_byConfIdx = byConfIdx;
            cServMsg.SetEventId( MT_MCU_CONF_CHARGEEXP_NOTIF );
            cServMsg.SetErrorCode( ERR_MCU_GK_DISCONNECT );
            SendMsgToMcu( cServMsg );
        }
    }

    // ���б���Ļ���Ʒѵ�������ȫ���������������GK�ļƷ���·���½������ϲ�ҵ
    // �������·���ʼ����Ʒѵ����󡣱�֤�����ٿ������Raduis���ݿ���Ϣ��һ�¡�[11/20/2006-zbq]
    ClearChargeData();
    */

    // zbq [03/13/2007] ���ݹ���������������GK�������˴���ֹͣ���ڼƷѵĻ��顣
    //           ֻͣ��ʱˢ�¡����µ�ע����ɺ󣬷����µļƷ��������� �ۼӻ�����
    KillTimer( TIMER_CONFCHARGE_STATUS_NTF );

    MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[DaemonDisconnectFromGK] disconnect from GK !\n" );    
    return;
}

/*=============================================================================
  �� �� ���� DaemonProcMcuMtChargeStartReq
  ��    �ܣ� ����MCU���鿪ʼ�Ʒ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/11/09    4.0			�ű���                  ����
=============================================================================*/
void CMtAdpInst::DaemonProcMcuMtChargeStartReq( CMessage * const pcMsg )
{
    if ( !g_cMtAdpApp.m_bGKReged || !g_cMtAdpApp.m_bMasterMtAdp )
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ChargeStartReq] State err: bRegGK.%d, bMaster.%d \n",
                         g_cMtAdpApp.m_bGKReged, g_cMtAdpApp.m_bMasterMtAdp );
        return;
    }
    CServMsg cServMsg( pcMsg->content, pcMsg->length );
    u8 byConfIdx = cServMsg.GetConfIdx();    

    TConfChargeInfo tChargeInfo;
    tChargeInfo = *(TConfChargeInfo*)cServMsg.GetMsgBody();

    if ( byConfIdx <= MAX_CONFIDX )
    {
        m_byConfIdx = byConfIdx;

        if ( !g_cMtAdpApp.m_bMasterMtAdp )
        {
            MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ChargeStartReq] this dri not master mtadp, nack\n", 
                                                   g_cMtAdpApp.m_bMasterMtAdp );
            cServMsg.SetErrorCode((u16)ERR_MCU_GK_UNKNOW);
            cServMsg.SetEventId(MT_MCU_CONF_STARTCHARGE_NACK);
            SendMsgToMcu(cServMsg);
            return;
        }

        if ( !g_cMtAdpApp.m_bGKReged )
        {
            MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "[ChargeStartReq] not connect or register gk success, waiting...\n");
            cServMsg.SetErrorCode((u16)ERR_MCU_GK_REGFAILED);
            cServMsg.SetEventId(MT_MCU_CONF_STARTCHARGE_NACK);
            SendMsgToMcu(cServMsg);
            return;
        }
        // ���ر���
        memcpy( &g_cMtAdpApp.m_atChargeInfo[byConfIdx-1], &tChargeInfo, sizeof(tChargeInfo) );

        GkChargeStart(tChargeInfo, byConfIdx);
    }
    else
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ChargeStartReq] confIdx.%d illegal, ignore!\n", byConfIdx );
    }
    return;
}

/*=============================================================================
�� �� ���� GkChargeStart
��    �ܣ� ��ʼ GK�Ʒ�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TConfChargeInfo &tChargeInfo
           u8 byConfIdx
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/17  4.0			������                  ����
=============================================================================*/
void CMtAdpInst::GkChargeStart( TConfChargeInfo &tChargeInfo, u8 byConfIdx )
{
    TConfAcctStartReq tStartReq;
    tStartReq.SetSeqNum( byConfIdx );

    //[4/15/2013 liaokang] ���뷽ʽ
    /*
    *	��Ϊ�ϵĽ�����GK�Ʒѵı��뷽ʽ��ΪGBK��Ϊ�˼����ϵ��豸�������µ��豸�����GBK�����Ա������κ�ת����   
    *	��������豸��UTF-8,Ҫ���ݶԶ˱������ͽ���ת�����������������:
    *	(1)����Զ���UTF-8,���ͺͽ��ն�����Ҫת������
    *	(2)����Զ���GBK�������ڷ���Ϣǰ��UTF-8תΪGBK,���˽��յ���Ϣʱ��GKBתΪUTF-8
    */
    if( emenCoding_Utf8 == g_cMtAdpApp.GetMcuEncoding() &&
        emenCoding_GBK == g_cMtAdpApp.GetGkEncoding() )
    {
        s8 achGBKText[MAXLEN_CONFNAME+1] = {0};  // ����ȡ���ֵ
        s8 achUTF8Text[MAXLEN_CONFNAME+1] = {0};

        memcpy(achUTF8Text, tChargeInfo.GetGKUsrName(), strlen(tChargeInfo.GetGKUsrName()));
        utf8_to_gb2312(achUTF8Text,achGBKText, MAXLEN_CONFNAME);
        tStartReq.SetAcctNum( (u8*)achGBKText, strlen(achGBKText) );
		
        memset(achGBKText, 0, sizeof(achGBKText));
        memset(achUTF8Text, 0, sizeof(achUTF8Text));
        memcpy(achUTF8Text, tChargeInfo.GetConfName(), strlen(tChargeInfo.GetConfName()));
        utf8_to_gb2312(achUTF8Text,achGBKText, MAXLEN_CONFNAME);
        tStartReq.SetConfName( (u8*)achGBKText, strlen(achGBKText) );
    }
    else
    {
        tStartReq.SetAcctNum( (u8*)tChargeInfo.GetGKUsrName(), strlen(tChargeInfo.GetGKUsrName()) );
        tStartReq.SetConfName( (u8*)tChargeInfo.GetConfName(), strlen(tChargeInfo.GetConfName()) );
    }

	tStartReq.SetAcctPwd( (u8*)tChargeInfo.GetGKPwd(), strlen(tChargeInfo.GetGKPwd()) );
    tStartReq.SetConfId( (u8*)tChargeInfo.GetConfE164(), strlen(tChargeInfo.GetConfE164()) );
    tStartReq.SetSiteRate( tChargeInfo.GetBitRate() );
    tStartReq.SetTermNum( tChargeInfo.GetTerNum() );
    tStartReq.SetMulVoiceNum( tChargeInfo.GetMixerNum() );
    tStartReq.SetMulPicNum( tChargeInfo.GetVMPNum() );
    tStartReq.SetRateAdapNum( tChargeInfo.GetBasNum() );
    tStartReq.SetUseCascade( tChargeInfo.GetIsUseCascade() );
    tStartReq.SetUseDataConf( tChargeInfo.GetIsUseDataConf() );
    tStartReq.SetUseDualVideo( tChargeInfo.GetIsUseDualVideo() );
    tStartReq.SetUseEncrypt( tChargeInfo.GetIsUseEncrypt() );
    tStartReq.SetUseStream( tChargeInfo.GetIsUseStreamBroadCast() );
    
    SendMsgToGK( EV_MG_CONF_ACCTING_START, (u8*)&tStartReq, sizeof(tStartReq) );
    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[GkChargeStart] conf(idx.%d) %s charge start request to gk.\n", byConfIdx, tChargeInfo.GetConfName());
    return;
}

/*=============================================================================
  �� �� ���� DaemonProcMcuMtChargeStopReq
  ��    �ܣ� ����MCU����ֹͣ�Ʒ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/11/09    4.0			�ű���                  ����
=============================================================================*/
void CMtAdpInst::DaemonProcMcuMtChargeStopReq( CMessage * const pcMsg )
{
    if ( !g_cMtAdpApp.m_bGKReged || !g_cMtAdpApp.m_bMasterMtAdp )
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ChargeStopReq] State err: bRegGK.%d, bMaster.%d \n",
                        g_cMtAdpApp.m_bGKReged, g_cMtAdpApp.m_bMasterMtAdp );
        return;
    }
    CServMsg cServMsg( pcMsg->content, pcMsg->length );
    u8 byConfIdx = cServMsg.GetConfIdx();
    
    if ( byConfIdx <= MAX_CONFIDX )
    {
        TConfChargeInfo *ptInfo = &g_cMtAdpApp.m_atChargeInfo[byConfIdx-1];
        TAcctSessionId *ptChargeSsnId = &g_cMtAdpApp.m_atChargeSsnId[byConfIdx-1];
                
        u8  abySsnId[LEN_RADIUS_SESSION_ID] = { 0 };
        u8  bySsnIdLen = sizeof(abySsnId);
        u16 wLenOut = 0;
        if ( ptChargeSsnId->IsSessionIdNull() )
        {
            MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ChargeStopReq] unexpected situation: SsnId is NULL, confIdx.%d \n", byConfIdx);
        }
        ptChargeSsnId->GetAcctSessionId( (u8*)abySsnId, bySsnIdLen, wLenOut );
        
        TConfAcctStopReq tStopReq;
        tStopReq.SetSeqNum( byConfIdx );
        tStopReq.SetSessionId( abySsnId, wLenOut );
        tStopReq.SetAcctNum( (u8*)ptInfo->GetGKUsrName(), strlen(ptInfo->GetGKUsrName()) );
        tStopReq.SetAcctPwd( (u8*)ptInfo->GetGKPwd(), strlen(ptInfo->GetGKPwd()) );
        
        SendMsgToGK( EV_MG_CONF_ACCTING_STOP, (u8*)&tStopReq, sizeof(tStopReq) );
        
        // ����������Ϣ
        ClearChargeData( byConfIdx );   
    }
    else
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ChargeStopReq] confIdx.%d illegal, ignore!\n", byConfIdx );
    }
    return;
}

/*=============================================================================
  �� �� ���� DaemonProcGKMcuChargeStartRsp
  ��    �ܣ� ����GK��ʼ�Ʒ���Ӧ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/11/09    4.0			�ű���                  ����
=============================================================================*/
void CMtAdpInst::DaemonProcGKMcuChargeStartRsp( CMessage * const pcMsg )
{
    if ( !g_cMtAdpApp.m_bGKReged || !g_cMtAdpApp.m_bMasterMtAdp )
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ChargeStartRsp] State err: bRegGK.%d, bMaster.%d \n",
                         g_cMtAdpApp.m_bGKReged, g_cMtAdpApp.m_bMasterMtAdp );
        return;
    }
    TConfAcctRsp tChargeRsp = *(TConfAcctRsp*)pcMsg->content;
    
    if ( emRadReqType_START != tChargeRsp.GetRequsetType() ) 
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ChargeStartRsp] wrong rsp type from GK, ignore it !\n" );
        return;
    } 
    
    m_byConfIdx  = (u8)tChargeRsp.GetSeqNum();
    
    CServMsg cServMsg;
    u16 wEventId = 0;
    u32 dwResult = tChargeRsp.GetResponseValue();
    if ( dwResult != RADIUS_INTER_ACCTING_SUCCESS ) 
    {
        wEventId = MT_MCU_CONF_STARTCHARGE_NACK;
        cServMsg.SetErrorCode( CMtAdpUtils::GetGKErrCode((u16)dwResult) );
        MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ChargeStartRsp] nack by GK, reason: %d !\n", dwResult );

        if ( g_cMtAdpApp.m_abyRestart[m_byConfIdx-1] == 0 )
        {
            //����������Ϣ
            ClearChargeData( m_byConfIdx );
        }
        else
        {
            //zbq [03/13/2007] �������Ľ�������Ʒ���������û���ļƷ���Ϣ
        }
    }
    else
    {
        wEventId = MT_MCU_CONF_STARTCHARGE_ACK;

        //����session id
        TAcctSessionId tChargeSsnId = tChargeRsp.GetSessionId();
		TAcctSessionId * ptSsnId = &g_cMtAdpApp.m_atChargeSsnId[m_byConfIdx-1];
        memcpy( ptSsnId, &tChargeSsnId,  sizeof(TAcctSessionId) );
        
        cServMsg.SetMsgBody( (u8*)&tChargeSsnId, sizeof(tChargeSsnId) );
    }
    
    // zbq [03/13/2007] ���ݹ��������󡣼Ʒ��쳣���µ����¼Ʒ�ʧ���ϲ���齫����֪��
    if ( g_cMtAdpApp.m_abyRestart[m_byConfIdx-1] == 1 &&
         MT_MCU_CONF_STARTCHARGE_NACK == wEventId )
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ChargeStartRsp] Conf.%d start charge failed<%d> but no sensity to MCU due to Restart charge !\n",
                        m_byConfIdx, CMtAdpUtils::GetGKErrCode((u16)dwResult) );
    }
    else
    {
        cServMsg.SetEventId( wEventId );
        SendMsgToMcu( cServMsg );
    }
    
    return;
}

/*=============================================================================
  �� �� ���� DaemonProcGKMcuChargeStopRsp
  ��    �ܣ� ����GKֹͣ�Ʒ���Ӧ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/11/09    4.0			�ű���                  ����
=============================================================================*/
void CMtAdpInst::DaemonProcGKMcuChargeStopRsp( CMessage * const pcMsg )
{
    if ( !g_cMtAdpApp.m_bGKReged || !g_cMtAdpApp.m_bMasterMtAdp )
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ChargeStopRsp] State err: bRegGK.%d, bMaster.%d \n",
                        g_cMtAdpApp.m_bGKReged, g_cMtAdpApp.m_bMasterMtAdp );        
        return;
    } 
    TConfAcctRsp tChargeRsp = *(TConfAcctRsp*)pcMsg->content;
    
    if ( emRadReqype_STOP != tChargeRsp.GetRequsetType() ) 
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ChargeStopRsp] wrong rsp type from GK, ignore it !\n" );
        return;
    } 
    
    CServMsg cServMsg;
    m_byConfIdx = (u8)tChargeRsp.GetSeqNum();

    if ( m_byConfIdx == 0 ) 
    {
        u32 dwResult = tChargeRsp.GetResponseValue();
        if ( dwResult != RADIUS_INTER_ACCTING_SUCCESS ) 
        {
            MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ChargeStopRsp] NPLUS nack by GK, reason: %d !\n", dwResult );
        }
        else
        {
            MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ChargeStopRsp] this rsp might be for the NPLUS, ignore it !\n" );
        }
        return;
    }
    
    u16 wEventId = 0;
    u32 dwResult = tChargeRsp.GetResponseValue();
    if ( dwResult != RADIUS_INTER_ACCTING_SUCCESS ) 
    {
        wEventId = MT_MCU_CONF_STOPCHARGE_NACK;
        if ( dwResult == RADIUS_INTER_TIME_OUT )
        {
            cServMsg.SetErrorCode( (u16)ERR_MCU_GK_STOPCHARGE_TIMEOUT );
        }
        else
        {
            cServMsg.SetErrorCode( CMtAdpUtils::GetGKErrCode((u16)dwResult) );
        }
        MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ChargeStopRsp] nack by GK, reason: %d !\n", dwResult );
    }
    else
    {
		wEventId = MT_MCU_CONF_STOPCHARGE_ACK;
    }

    cServMsg.SetEventId( wEventId );
    SendMsgToMcu( cServMsg );
    
    return;
}

/*=============================================================================
  �� �� ���� DaemonProcMcuGKConfChargeStatusNtf
  ��    �ܣ� ���������䶨ʱ����GK�Ʒѻ���֪ͨ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/11/17    4.0			�ű���                  ����
=============================================================================*/
void CMtAdpInst::DaemonProcMcuGKConfChargeStatusNtf( /*CMessage * const pcMsg*/ )
{
    if ( !g_cMtAdpApp.m_bGKReged || !g_cMtAdpApp.m_bMasterMtAdp )
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[MG_ChargeStatusNtf] State err: bRegGK.%d, bMaster.%d \n",
                             g_cMtAdpApp.m_bGKReged, g_cMtAdpApp.m_bMasterMtAdp );        
        return;
    } 
	u8 byChargeConfNum = 0;
	u8 abyBuf[sizeof(TAcctSessionId)*MAXNUM_MCU_CONF+1];
	memset( abyBuf, 0, sizeof(abyBuf) );

	u16  wBufLen = 1;	//��һ���ֶ�Ϊ��ǰ�Ʒѻ�����ܸ���
	
	for( u8 byConfIdx = 1; byConfIdx <= MAX_CONFIDX; byConfIdx ++ )
	{
		TAcctSessionId * ptSsnId = &g_cMtAdpApp.m_atChargeSsnId[byConfIdx-1];
		TConfChargeInfo * ptInfo = &g_cMtAdpApp.m_atChargeInfo[byConfIdx-1];

		if ( !ptSsnId->IsSessionIdNull() && !ptInfo->IsNull() ) 
		{
			byChargeConfNum ++;
			memcpy( &abyBuf[wBufLen], ptSsnId, sizeof(TAcctSessionId) );
			wBufLen += sizeof(TAcctSessionId);
		}
	}
	abyBuf[0] = byChargeConfNum;
	SendMsgToGK( EV_MG_CONF_ACCTING_STATUS_NTF, (u8*)abyBuf, wBufLen );
	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[MG_ChargeStatusNtf] send EV_MG_CONF_ACCTING_STATUS_NTF to GK !\n");

	SetTimer( TIMER_CONFCHARGE_STATUS_NTF, CONF_CHANGE_NTF_INTERVAL );
	return;
}

/*=============================================================================
  �� �� ���� DaemonProcGKMcuConfChargeStatusNtf
  ��    �ܣ� ����GK����������Ļ���Ʒ��쳣֪ͨ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/11/17    4.0			�ű���                  ����
=============================================================================*/
void CMtAdpInst::DaemonProcGKMcuConfChargeStatusNtf( CMessage * const pcMsg )
{
    if ( !g_cMtAdpApp.m_bGKReged || !g_cMtAdpApp.m_bMasterMtAdp )
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[GM_ChargeStatusNtf] State err: bRegGK.%d, bMaster.%d \n",
                             g_cMtAdpApp.m_bGKReged, g_cMtAdpApp.m_bMasterMtAdp );
        return;
    } 
	u8 * pbyMsgBody = (u8*)pcMsg->content;

	// ��ȡ�Ʒ��쳣�Ļ������
	u8 byExpConfNum = *(u8*)pbyMsgBody;
	pbyMsgBody += sizeof(u8);
    if ( pcMsg->length < byExpConfNum * sizeof(TAcctSessionId) + 1 + sizeof(u32) )
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ChargeStatusNtf] invalid msg body from GK, ignore it!\n" );
        return;
    }
	MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ChargeStatusNtf] there are %d conf got charge exception!\n", 
		                                                           byExpConfNum );

	for( u8 byIndex = 0; byIndex < byExpConfNum; byIndex ++ )
	{
		BOOL32 bExist = FALSE;
		TAcctSessionId tSsnId = *(TAcctSessionId*)pbyMsgBody;
		
		for( u8 byConfIdx = 1; byConfIdx <= MAX_CONFIDX; byConfIdx ++ )
		{
			TAcctSessionId * ptSsnId = &g_cMtAdpApp.m_atChargeSsnId[byConfIdx-1];

			if ( 0 == memcmp( &tSsnId, ptSsnId, sizeof(TAcctSessionId) ) )
			{
                // zbq [03/13/2007] ���ݹ��������������� ��·��������ݿ�����
                //   �ȵȵ��µ�Raduis�Ʒ��쳣�����鲻��֪�����ݴ����������´���
                /*
				CServMsg cServMsg;
				m_byConfIdx = byConfIdx;
				cServMsg.SetEventId( MT_MCU_CONF_CHARGEEXP_NOTIF );
				cServMsg.SetErrorCode( ERR_MCU_GK_CONFCHARGE_EXCEPTION );
				SendMsgToMcu(  cServMsg );
                */
                
                u32 dwErrCode = *(u32*)(pbyMsgBody + sizeof(TAcctSessionId)*byExpConfNum);
                switch( ntohl(dwErrCode) )
                {
                // ���·���Ʒ����� �ۼӻ���
                case RADIUS_INTER_CONF_NOT_EXIST:
                    g_cMtAdpApp.m_abyRestart[byConfIdx-1] = 1;
                    GkChargeStart(g_cMtAdpApp.m_atChargeInfo[byConfIdx-1], byConfIdx);
                    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[GM_ChargeStatusNtf] err code.%d, restart charge !\n", ntohl(dwErrCode));
                	break;
                // ����Ʒ���Ϣ�ȴ���������������������
                case RADIUS_INTER_TIME_OUT:
                case RADIUS_INTER_DB_FAIL:
                case RADIUS_INTER_GK_NOT_SUPPORT_ACCT:
                    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[GM_ChargeStatusNtf] err code.%d, do nothing to conf !\n", ntohl(dwErrCode));
                	break;
                default:
                    MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "unexpected err code.%d received !\n", ntohl(dwErrCode));
                    break;
                }

				// ����üƷ��쳣����ļƷ�����
                // zbq [03/13/2007] �Ʒ��쳣��������Ʒ���Ϣ��
				// ClearChargeData( byConfIdx );
				
				bExist = TRUE;
                break;
			}
		}
		if ( !bExist )
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ChargeStatusNtf] SsnId is unexist, ignore it !\n" );
		}
		pbyMsgBody += sizeof(TAcctSessionId);
	}
	
	return;
}

/*=============================================================================
  �� �� ���� ClearChargeData
  ��    �ܣ� �������Ʒ���Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfIdx��0 ����������л���Ʒ����ݣ����������Ӧ�ľ������
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/11/18    4.0			�ű���                  ����
=============================================================================*/
void CMtAdpInst::ClearChargeData( u8 byConfIdx )
{
	if ( byConfIdx > MAX_CONFIDX )
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ClearChargeData] ilegal confidx<%d> to clear, ignore it !\n",
                                                                          byConfIdx );
		return;
	}
	if ( 0 == byConfIdx ) 
	{
		memset( g_cMtAdpApp.m_atChargeSsnId, 0, sizeof(g_cMtAdpApp.m_atChargeSsnId) );
		memset( g_cMtAdpApp.m_atChargeInfo, 0, sizeof(g_cMtAdpApp.m_atChargeInfo) );
        memset( g_cMtAdpApp.m_abyRestart, 0, sizeof(g_cMtAdpApp.m_abyRestart) );
	}
	else
	{
		memset( &g_cMtAdpApp.m_atChargeInfo[byConfIdx-1], 0, sizeof(TConfChargeInfo) );
		memset( &g_cMtAdpApp.m_atChargeSsnId[byConfIdx-1], 0, sizeof(TAcctSessionId) );
        g_cMtAdpApp.m_abyRestart[byConfIdx-1] = 0;
	}
	return;
}

/*=============================================================================
  �� �� ���� DaemonProcNPlusUnregGKCmd
  ��    �ܣ� N+1���ݣ�����MCUαװע������ע�������ʵ����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/12/18    4.0			�ű���                  ����
=============================================================================*/
void CMtAdpInst::DaemonProcNPlusUnregGKCmd( CMessage * const pcMsg )
{
//     if ( !g_cMtAdpApp.m_bMasterMtAdp ) 
//     {
//         return;
//     }

	// ������Ϣע���϶�����mtadp����ǿ������һ��
    g_cMtAdpApp.m_bMasterMtAdp = TRUE;
    
    CServMsg cServMsg( pcMsg->content, pcMsg->length );
    TRASInfo tRASInfo = *(TRASInfo*)cServMsg.GetMsgBody();

    if ( g_cMtAdpApp.m_tGKAddr.ip != htonl(tRASInfo.GetGKIp()) ) 
    {
        MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[UnregGKCmd] this two N+1 MCU hasn't the same GK, there is no need to Unregister !\n" );
        return;
    }
    // αװ����ע�����(�����ɹ���ʧ��)֮ǰ���е�RRQ�����еȴ� [12/21/2006-zbq]
    g_cMtAdpApp.m_bURQForNPlus = TRUE;

    if( !g_cMtAdpApp.m_bGkAddrSet )
    {		
        SetGKRasAddress( g_cMtAdpApp.m_tGKAddr );
    }
    
    s32 nRet = 0;	
	HRAS hsRas = NULL;

    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "DaemonProcNPlusUnregGKCmd to be unregister MASTER mcu here !\n" );
	
	//start unregister ...

	//zbq[06/20/2008] RASʹ����ʽGK��ַ, ���������ڿ�������ʱ�������Э��ջpvtNode��ѯ

	//[1] get RAS handle of unregister
	nRet = cmRASStartTransaction(g_cMtAdpApp.m_hApp, NULL, &hsRas, cmRASUnregistration, &g_cMtAdpApp.m_tGKAddr, NULL);
	if( nRet < 0 )
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "cmRASStartTransaction failed in urq! err ret :%d\n", nRet );
        
        //��ȡЭ��ջ��Դʧ�ܣ����ӻָ�����
        DaemonProcGetStackResFailed();
        g_cMtAdpApp.m_bURQForNPlus = FALSE;
		return;
	}
    else //keep the handle for lookup in callback	
	{
		g_cMtAdpApp.m_hsRas[0] = hsRas;
	}

	//[2.1] set alias of MCU to be unregistered ��to be add here if need
	//[2.2] set alias list to be unregistered   ��to be add here if need

	//[3.1] set call signalling address <ip, port> which is not in use actually, but just to be extended.
    cmTransportAddress tCallAddr;
    tCallAddr.ip   = htonl(tRASInfo.GetRRQIp());
    tCallAddr.port = g_cMtAdpApp.m_wQ931Port;
    tCallAddr.type = cmTransportTypeIP;
    tCallAddr.distribution = cmDistributionUnicast;
    
    nRet = cmRASSetParam( hsRas, cmRASTrStageRequest, cmRASParamCallSignalAddress,
                                          0, sizeof(tCallAddr), (s8*)&tCallAddr );
    if ( nRet < 0 )
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[NPlusUnregGKCmd] set cmRASParamCallSignalAddress err nRet=%d\n", nRet);
        g_cMtAdpApp.m_bURQForNPlus = FALSE;
        cmRASClose( hsRas );
        return;
    }

    //[3.2] set the fake EPID here which is the most important element for this URQ
    if ( 0 != tRASInfo.GetEPID()->GetIDlength() ) 
    {
        cmAlias tEPID;
        tEPID.type   = ( cmAliasType ) tRASInfo.GetEPID()->GetIDtype();
        tEPID.length = ( UINT16 ) tRASInfo.GetEPID()->GetIDlength();
        tEPID.pnType = ( cmPartyNumberType ) tRASInfo.GetEPID()->GetIDpnType();
        tEPID.string = tRASInfo.GetEPID()->GetIDAlias();
        memcpy( (void*)&tEPID.transport, (void*)tRASInfo.GetEPID()->GetIDtransport(), 
                                                     sizeof(TH323TransportAddress) );
        
        nRet = cmRASSetParam( hsRas, cmRASTrStageRequest, cmRASParamEndpointID, 0,
                                                   sizeof(tEPID), (s8*)&(tEPID) );
        if ( nRet < 0 )
        {
            MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[NPlusUnregGKCmd] -- set cmRASParamEndpointID error nRet=%d!\n", nRet);
            g_cMtAdpApp.m_bURQForNPlus = FALSE;
            cmRASClose( hsRas );
            return;
        }
    }
    else
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[NPlusUnregGKCmd] fake epid length.%d, and URQ failed !\n", 
                                      ( UINT16 )tRASInfo.GetEPID()->GetIDlength() );
        g_cMtAdpApp.m_bURQForNPlus = FALSE;
        return;
    }

	//[4] send out request
	if( cmRASRequest( hsRas ) < 0 )
	{		
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "cmRASRequest failed in DaemonProcNPlusUnregGKCmd\n" );
        g_cMtAdpApp.m_bURQForNPlus = FALSE;
		cmRASClose( hsRas );
	}

    //[5] zbq [03/15/2007] normal URQ send out, start RRQ a fake E164 from 
    //      N-mpc which will be reserved after all the conference restored.
    SendRRQUpdateMcuE164(&tRASInfo);

    return;
}

/*=============================================================================
  �� �� ���� DaemonProcNPlusStopChargeCmd
  ��    �ܣ� N+1���ݣ�����MCUαװע��ͣ����MCU�����мƷ���Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/12/25    4.0			�ű���                  ����
=============================================================================*/
void CMtAdpInst::DaemonProcNPlusStopChargeCmd( CMessage * const pcMsg )
{
    if ( !g_cMtAdpApp.m_bGKReged || !g_cMtAdpApp.m_bMasterMtAdp )
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ChargeStopCmd] State err: bRegGK.%d, bMaster.%d \n",
                        g_cMtAdpApp.m_bGKReged, g_cMtAdpApp.m_bMasterMtAdp );
        return;
    }
    CServMsg cServMsg( pcMsg->content, pcMsg->length );
    TConfChargeInfo tInfo  = *(TConfChargeInfo*)cServMsg.GetMsgBody();
    TAcctSessionId  tSsnId = *(TAcctSessionId*)(cServMsg.GetMsgBody() + sizeof(TConfChargeInfo));

    // ��ʶN+1ģʽ�µ�ֹͣ�ƷѲ���
    u8 byConfIdx = 0;
    
    TConfAcctStopReq tStopReq;
    tStopReq.SetSeqNum( byConfIdx );
    tStopReq.SetSessionId( tSsnId.GetAcctSessionId(), tSsnId.GetAcctSessionIdLen() );
    tStopReq.SetAcctNum( (u8*)tInfo.GetGKUsrName(), strlen(tInfo.GetGKUsrName()) );
    tStopReq.SetAcctPwd( (u8*)tInfo.GetGKPwd(), strlen(tInfo.GetGKPwd()) );
    
    SendMsgToGK( EV_MG_CONF_ACCTING_STOP, (u8*)&tStopReq, sizeof(tStopReq) );

    return;
}

/*=============================================================================
  �� �� ���� SendRRQUpdateMcuE164
  ��    �ܣ� N+1���ݣ�����RRQ����MCU��E164
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TRASInfo * ptRASInfo: ��Ϊ�ָ� == NULL; ��Ϊ���ݣ���ȡptRASInfo��ļ�E164
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/03/15    4.0			�ű���                  ����
=============================================================================*/
void CMtAdpInst::SendRRQUpdateMcuE164( TRASInfo * ptRASInfo )
{
    CServMsg cSerMsg;
    TMtAlias tMcuE164;
    TMtAlias tMcuH323Id;
    if ( NULL != ptRASInfo )
    {
        tMcuE164.SetE164Alias(ptRASInfo->GetMcuE164());
        g_cMtAdpApp.m_tMcuAliasBak.SetE164Alias(g_cMtAdpApp.m_atMcuAlias[0].m_achAlias);
        MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[SendRRQUpdateMcuE164] ptRASInfo.E164<%s> !\n", ptRASInfo->GetMcuE164() );
    }
    else
    {
        tMcuE164.SetE164Alias(g_cMtAdpApp.m_tMcuAliasBak.m_achAlias);
        g_cMtAdpApp.m_atMcuAlias[0].SetE164Alias(g_cMtAdpApp.m_tMcuAliasBak.m_achAlias);
        MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[SendRRQUpdateMcuE164] g_cMtAdpApp.m_atMcuAlias[0].E164<%s> !\n", g_cMtAdpApp.m_atMcuAlias[0].m_achAlias );
    }
    
    tMcuH323Id.SetH323Alias(g_cMtAdpApp.m_tMcuH323Id.m_achAlias);
    cSerMsg.SetMcuId(LOCAL_MCUID);
    cSerMsg.SetConfIdx(0);
    cSerMsg.SetDstDriId(g_cMtAdpApp.m_byDriId);
    cSerMsg.SetMsgBody((u8*)&tMcuE164, sizeof(TMtAlias));
    cSerMsg.CatMsgBody((u8*)&tMcuH323Id, sizeof(TMtAlias));
    
    // Э������ĵ�ַ������ֻд�뱾����壬���������ĵ�ַ����������ͨ������ͬ����
    TTransportAddr tMtAdpAddr;
    tMtAdpAddr.SetIpAddr(ntohl(g_cMtAdpApp.m_dwMtAdpIpAddr));
    tMtAdpAddr.SetPort(g_cMtAdpApp.m_wQ931Port);
    cSerMsg.CatMsgBody((u8*)&tMtAdpAddr, sizeof(TTransportAddr));
    
    CMessage cMsg;
    cMsg.content = cSerMsg.GetServMsg();
    cMsg.length  = cSerMsg.GetServMsgLen();
    DaemonProcSendRRQ( &cMsg );

    return;
}

/*=============================================================================
  �� �� ���� DaemonProcMcuE164RestoreNtf
  ��    �ܣ� N+1���ݣ�����MCUע�������������� ��MCU.E164����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/03/15    4.0			�ű���                  ����
=============================================================================*/
void CMtAdpInst::DaemonProcMcuE164RestoreNtf( CMessage * const pcMsg )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );
    if ( 0 != cServMsg.GetConfIdx() )
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[McuE164RestoreNtf] confIdx.%d != 0, ignore it !\n",
                        cServMsg.GetConfIdx() );
        return;
    }
    SendRRQUpdateMcuE164();

    return;
}

/*=============================================================================
  �� �� ���� DaemonProcURQTimeOut
  ��    �ܣ� URQ��ʱ������Ϊ���Ƿǳ����ص����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/03/15    4.0			�ű���                  ����
=============================================================================*/
void CMtAdpInst::DaemonProcURQTimeOut( void )
{
    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[DaemonProcURQTimeOut] MSLinkStatus.%d change to normal\n", g_cMtAdpApp.m_bWaitingURQRsp );

    if ( g_cMtAdpApp.m_bWaitingURQRsp )
    {
        g_cMtAdpApp.m_bWaitingURQRsp = FALSE;

        BOOL32 bConnectMcuA = FALSE;
        BOOL32 bConnectMcuB = FALSE;
        if ( OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNode))
        {
            bConnectMcuA = TRUE;
            OspDisconnectTcpNode(g_cMtAdpApp.m_dwMcuNode);
        }
        if ( OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNodeB))
        {
            bConnectMcuB = TRUE;
            OspDisconnectTcpNode(g_cMtAdpApp.m_dwMcuNodeB);
        }
        
        g_cMtAdpApp.ClearMtAdpData();
        OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), MTADP_MCU_CONNECT, NULL, 0 );
        if (g_cMtAdpApp.m_bDoubleLink )
        {
            OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), MTADP_MCU_CONNECT2, NULL, 0 );
        }
        MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[DaemonProcURQTimeOut] clear URQRspTimer timeout and restart connect for LinkA.%d, LinkB.%d ...\n", bConnectMcuA, bConnectMcuB );
    }
    return;    
}

/*=============================================================================
�� �� ���� DaemonPronRRQTimeOut
��    �ܣ� RRQ��ʱ������GK���ݵ�˲���п��ܷ���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage * const pcMsg
�� �� ֵ�� void 
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
2009/10/17    4.6			�ű���                  ����
=============================================================================*/
void CMtAdpInst::DaemonPronRRQTimeOut( void )
{
    //Ŀǰֻ��������״̬
    g_cMtAdpApp.m_gkRegState = STATE_NULL;
    
    MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[DaemonPronRRQTimeOut] might due to GK transfer, restore immediately!\n");
	
    DaemonProcSendRRQ(NULL);
	
    return;
}

/*=============================================================================
  �� �� ���� DaemonProcGetStackResFailed
  ��    �ܣ� ��ȡЭ��ջ��Դʧ�ܣ���Ϊ���Ƿǳ����ص��������������ָ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage * const pcMsg
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/03/17    4.0			�ű���                  ����
=============================================================================*/
void CMtAdpInst::DaemonProcGetStackResFailed( void )
{
    g_cMtAdpApp.m_byGetStackResFailedTimes ++;

    MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[DaemonProcGetStackResFailed] failed time(s).%d\n", 
                              g_cMtAdpApp.m_byGetStackResFailedTimes );

    //����10��ʧ��(һ����RRQ����ʧ�ܣ���500s)
    if ( g_cMtAdpApp.m_byGetStackResFailedTimes > 10 )
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[DaemonProcGetStackResFailed] failed time(s).%d overflow, reset\n", 
                                                  g_cMtAdpApp.m_byGetStackResFailedTimes );
        
    #ifndef WIN32 
        
        #ifdef _LINUX_
            //д���ļ�
            char achInfo[255];
            time_t tiCurTime = ::time(NULL);             
            int nLen = sprintf(achInfo, "\nsytem time %s get stack res failed over times.\n", ctime(&tiCurTime));
        
            FILE *hLogFile = fopen(DIR_EXCLOG, "r+b");
            if (NULL == hLogFile)
            {
                printf("exc.log not exist and create it\n");
                hLogFile = fopen(DIR_EXCLOG, "w+b");
                if (NULL == hLogFile)
                {
                    printf("create exc.log failed for %s\n", strerror(errno));     
                    return;
                }
            }
        
            fseek(hLogFile, 0, SEEK_END);
            nLen = fwrite(achInfo, nLen, 1, hLogFile);
            if (0 == nLen)
            {
                printf("write to exc.log failed\n");
                return;
            }
        
            fclose(hLogFile);
        #endif
#ifndef _8KH_
        OspQuit();
#endif
        BrdHwReset();
        
        return;
    #endif
        
    }

    return;
}


// END OF FILE
