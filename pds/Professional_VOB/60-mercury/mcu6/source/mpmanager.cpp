 /*****************************************************************************
   ģ����      : MP
   �ļ���      : mpmanage.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: MP��MCU�ӿ�ʵ��
   ����        : ������
   �汾        : V0.1  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/07/10  0.1         ������      ����
   2005/02/19  3.6         ����      �����޸ġ���3.5�汾�ϲ�
******************************************************************************/

#include "evmcumt.h"
#include "evmcu.h"
#include "evmp.h"
#include "mcuvc.h"
//#include "mpmanager.h"
#include "mtadpssn.h"
#include "mpssn.h"
#include "mcuver.h"	//[pengguofeng 5/10/2012]IPV6 

/*lint -save -esym(666, min, max, kmin, kmax)*/

// MPC2 ֧��
#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
        #include "brdwrapperdef.h"
        #include "nipwrapper.h"
        //#include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
#else
    #include "brddrvlib.h"
#endif

CMpManager *pcMpManager = NULL;

CMpManager::CMpManager()
{
	//��ʼ��
	pcMpManager = this;
}

CMpManager::~CMpManager()
{

}

/*====================================================================
    ������      ��showmp
    ����        ����Ļ��ӡ����Mp��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/17    1.0         ������        ����
====================================================================*/
API void showmp(void)
{
	pcMpManager->ShowMp();

	return;
}

/*====================================================================
    ������      ��showmtadp
    ����        ����Ļ��ӡ����MtAdp��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/06/01    3.0         ������        ����
====================================================================*/
API void showmtadp(void)
{
	pcMpManager->ShowMtAdp();

	return;
}

/*====================================================================
    ������      : showbridge
    ����        ����Ļ��ӡ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/17    1.0         ������        ����
====================================================================*/
API void showbridge(void)
{
	pcMpManager->ShowBridge();

	return;
}

/*====================================================================
    ������      ��ssw
    ����        ����Ļ��ӡ������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/17    1.0         ������        ����
====================================================================*/	
API void ssw( u8 byType )
{
	pcMpManager->ShowSwitch( byType );

	return;
}

API void sswi(const s8* pchDstIp, u16 wDstPort, u8 byConfIdx, BOOL32 bDetail)
{
	BOOL32 bPrintUsage = FALSE;
	do 
	{
		if (0 == byConfIdx && 0 == wDstPort && NULL == pchDstIp)
		{
			bPrintUsage = TRUE;
			break;
		}

		if (0 == byConfIdx || MAXNUM_MCU_CONF < byConfIdx || 0 == wDstPort || NULL == pchDstIp)
		{
			StaticLog("Invlid argument! See the usage below:\n");
			bPrintUsage = TRUE;
			break;
		}
		
		//check if the user don't enwrap the DstIp with quotation marks
		u32 dwPValue = (u32)pchDstIp;
		if ((dwPValue & (~0x000000FF)) == 0)
		{
			StaticLog("the DstIp should be enwrapped with \"\"\n");
			bPrintUsage = TRUE;
			break;
		}

		if (strlen(pchDstIp) > 15 || strlen(pchDstIp) < 7)
		{
			StaticLog( "Invlid Ip! See the usage below:\n");
			bPrintUsage = TRUE;
			break;
		}
		if (0 == INET_ADDR(pchDstIp))
		{
			StaticLog( "Invlid Ip! See the usage below:\n");
			bPrintUsage = TRUE;
			break;
		}

	} while (0);

	if (bPrintUsage)
	{
		s8 achFormat[]	= "usage: sswi \"DstIp\" DstPort confIdx";
		s8 achExample[] = "Example: sswi \"172.16.230.202\" 60040 3";
		s8 achNote[]	= "Note that the DstIp must be enwrapped with \"\" ";
		u8 byLen = max( (strlen(achFormat)), (strlen(achNote)) );
		byLen = max(byLen, strlen(achExample));
		
		u8 byLoop =0;
		StaticLog( "+");
		for(; byLoop< ( byLen + 4 ); byLoop ++)
		{
			StaticLog( "-");
		}
		StaticLog( "+\n");
		
		StaticLog( "|  %-*s  |\n", byLen,achFormat);
		StaticLog( "|  %-*s  |\n", byLen, achExample); 
		StaticLog( "|  %-*s  |\n", byLen, achNote); 
		
		StaticLog( "+");
		for(byLoop =0; byLoop< byLen + 4; byLoop ++)
		{
			StaticLog( "-");
		}
		StaticLog( "+\n");
		
	}
	else
	{
		if(pchDstIp != NULL)
		{
			u32 dwDstIp = ntohl(INET_ADDR(pchDstIp));
			if (!pcMpManager->ShowSwitchRouteToDst(byConfIdx, dwDstIp, wDstPort, bDetail))
			{
				StaticLog( "no match info found!\n");
			}
		}
	}

	return;
	
}


/*====================================================================
    ������      ��StartMulticast
    ����        ����ʼMt�鲥
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tSrc Դ�ն�
	              bySrcChnnl ͨ����
				  byMode ����ģʽ,ȱʡΪMODE_BOTH
    ����ֵ˵��  ��TRUE��������ܿ�ʼ�㲥����FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/08/20    2.0         ������        ����
====================================================================*/
BOOL32 CMpManager::StartMulticast( const TMt & tSrc, u16 wSrcChnnl, u8 byMode, BOOL32 bConf )
{
	u32  dwSrcIp = 0;
	u32  dwRcvIp = 0;
	u16  wRcvPort = 0;
	BOOL32 bResult1 = TRUE;
	BOOL32 bResult2 = TRUE;
	u32  dwMultiCastAddr;
	u16  dwMultiCastPort;
	 
	//�õ�����Դ��ַ
	if( !GetSwitchInfo(tSrc, dwRcvIp, wRcvPort, dwSrcIp))
	{
		 LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartMulticast() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//�õ�MCU���鲥��ַ
    //dwMultiCastAddr = g_cMcuAgent.GetCastIpAddr();
    //dwMultiCastPort = g_cMcuAgent.GetCastPort();
    // ����@2006.4.6 ����Ҫ�ӻ�������ȡ
    dwMultiCastAddr = g_cMcuVcApp.AssignMulticastIp(tSrc.GetConfIdx());
	if (bConf)
	{
		dwMultiCastPort = g_cMcuVcApp.AssignMulticastPort(tSrc.GetConfIdx(), 0);
	}
	else
	{
		dwMultiCastPort = g_cMcuVcApp.AssignMulticastPort(tSrc.GetConfIdx(), tSrc.GetMtId());
	}
	
	//multispy port
	if( wSrcChnnl >= CASCADE_SPY_STARTPORT && wSrcChnnl < MT_MCU_STARTPORT ) //�ö�ش���ͨ��
	{
		wRcvPort = wSrcChnnl;
	}
	else
	{
		//�����ŵ��ż���ƫ����
		wRcvPort = wRcvPort + PORTSPAN * wSrcChnnl;
	}

	//�����ŵ��ż���ƫ����	
	//wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;

    //multicasting
    u8 byIsMulticast;
    u8 byMulticastFlag = 0;
    u8 byFirstMpId = 0;
    for(u16 wMpId = 0; wMpId < MAXNUM_DRI + 1; wMpId++)
    {
        if (g_cMcuVcApp.IsMpConnected(wMpId+1) || (MAXNUM_DRI == wMpId))
        {

            if (MAXNUM_DRI > wMpId)
            {
                if (0 == byFirstMpId)
                {
                    byFirstMpId = wMpId+1;
                }

                byIsMulticast = g_cMcuVcApp.GetMpMulticast(wMpId+1);
                LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "\n[StartMulticast]byIsMulticast = %d  wMpId+1 = %d\n\n", byIsMulticast, wMpId+1);
                if (1 != byIsMulticast)
                {
                    continue;
                }

                byMulticastFlag++;

                dwRcvIp = g_cMcuVcApp.GetMpIpAddr(wMpId+1);
                LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "\n[StartMulticast]dwRcvIp = 0x%x\n\n", dwRcvIp);
            }
            else
            {
                if (0 < byMulticastFlag)
                {
                    continue;
                }
                dwRcvIp = g_cMcuVcApp.GetMpIpAddr(byFirstMpId);
            }

            //ͼ��
            if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
            {
                //��������
                bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort,
                                       dwMultiCastAddr, dwMultiCastPort, dwRcvIp, dwRcvIp);
                if (!bResult1)
                {
                    LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Switch video failure!\n");
                }
            }

            //����
            if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
            {
                //��������
                bResult2 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, (wRcvPort+2),
                                       dwMultiCastAddr, (dwMultiCastPort+2), dwRcvIp, dwRcvIp);
                if (!bResult2)
                {
                    LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Switch audio failure!\n");
                }
            }
            
            //�ڶ�·��Ƶ
            if (byMode == MODE_SECVIDEO)
            {
                bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, (wRcvPort+4),
                                       dwMultiCastAddr, (dwMultiCastPort+4), dwRcvIp, dwRcvIp);
                if (!bResult1)
                {
                    LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Switch snd video failure!\n");
                }
            }
        }
    }

	return  bResult1 && bResult2;
}

/*====================================================================
    ������      ��StopMulticast
    ����        ��ֹͣMt�鲥
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tSrc Դ�ն�
	              bySrcChnnl ͨ����
				  byMode ����ģʽ,ȱʡΪMODE_BOTH
    ����ֵ˵��  ��TRUE��������ܿ�ʼ�㲥����FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/08/20    2.0         ������        ����
====================================================================*/
BOOL32 CMpManager::StopMulticast(const TMt & tSrc, u8 bySrcChnnl, u8 byMode, BOOL32 bConf)
{
	u32  dwSrcIp  = 0;
	u32  dwRcvIp  = 0;
	u16  wRcvPort = 0;

	u32  dwMultiCastAddr;
	u16  dwMultiCastPort;
		
	//�õ�����Դ��ַ
	if (!GetSwitchInfo(tSrc, dwRcvIp, wRcvPort, dwSrcIp))
	{
		 LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StopMulticast() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//�õ�MCU���鲥��ַ
    // dwMultiCastAddr = g_cMcuAgent.GetCastIpAddr();
    // ����@2006.4.6 ����Ҫ�ӻ�������ȡ
    dwMultiCastAddr = g_cMcuVcApp.AssignMulticastIp(tSrc.GetConfIdx());    
    // dwMultiCastPort = g_cMcuAgent.GetCastPort();
	if (bConf)
	{
		dwMultiCastPort = g_cMcuVcApp.AssignMulticastPort(tSrc.GetConfIdx(), 0);
	}
	else
	{
		dwMultiCastPort = g_cMcuVcApp.AssignMulticastPort(tSrc.GetConfIdx(), tSrc.GetMtId());
	}

	//�����ŵ��ż���ƫ����	
	wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;

	//ͼ��
	if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
	{
		//�Ƴ�����
		StopSwitch(tSrc.GetConfIdx(), dwMultiCastAddr, dwMultiCastPort);
	}
		
	//����
	if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
	{
		//�Ƴ�����
		StopSwitch(tSrc.GetConfIdx(), dwMultiCastAddr, (dwMultiCastPort+2));
	}
    
    //�ڶ�·��Ƶ
    if (byMode == MODE_SECVIDEO)
    {
        StopSwitch(tSrc.GetConfIdx(), dwMultiCastAddr, (dwMultiCastPort+4));
    }

	return TRUE;
}


/*====================================================================
    ������      ��StartDistrConfCast
    ����        ����ʼ��ɢ�����鲥
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tSrc Դ�ն�
	              bySrcChnnl ͨ����
				  byMode ����ģʽ,ȱʡΪMODE_BOTH
    ����ֵ˵��  ��TRUE��������ܿ�ʼ�㲥����FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/04/13    4.0         ����        ����
====================================================================*/
BOOL32 CMpManager::StartDistrConfCast( const TMt &tSrc, u8 byMode, u8 bySrcChnnl)
{
	u32  dwSrcIp = 0;
	u32  dwRcvIp = 0;
	u16  wRcvPort = 0;
	BOOL32 bResult1 = TRUE;
	BOOL32 bResult2 = TRUE;
	u32  dwMultiCastAddr;
	u16  wMultiCastPort;
	 
	//�õ�����Դ��ַ
	if( !GetSwitchInfo(tSrc, dwRcvIp, wRcvPort, dwSrcIp))
	{
		 LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartDistrConfCast() failure because of can't get switch info!\n");
		 return FALSE ;
	}

    CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(tSrc.GetConfIdx());
    if ( NULL == pVcInst )
    {
        // Fatal error!
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Cannot get VC Instance(conf idx: %d) while StartDistrConfCast!\n", 
                            tSrc.GetConfIdx());
        return FALSE;
    }
	
	//  [3/2/2012 pengguofeng]ȡ�����ն�
	TConfMtTable *ptConfMtTab = g_cMcuVcApp.GetConfMtTable(tSrc.GetConfIdx());
	if ( ptConfMtTab == NULL)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "[StartDistrConfCast]Get conf:%d mt table failed\n", tSrc.GetConfIdx());
		return FALSE;
	}

	//  [3/2/2012 pengguofeng]ȡ������Ϣ
	TConfAllMtInfo *ptConfAllMtInfo = g_cMcuVcApp.GetConfAllMtInfo(tSrc.GetConfIdx());
	if ( ptConfMtTab == NULL)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "[StartDistrConfCast]Get conf:%d mt info failed\n", tSrc.GetConfIdx());
		return FALSE;
	}

	TLogicalChannel tLogCnnl;

// 	TMtStatus tStatus;
// 	TMt tSelMt;
	//  [3/2/2012 pengguofeng]��������ƵԴ��Ŀǰֻ����ƵԴ������ȷ�Ż���ȷ������ؼ�֡��
	/*if ( tSrc.GetType() == TYPE_MT && (byMode == MODE_AUDIO || byMode == MODE_VIDEO) )
	{
		for ( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
		{
			//  [3/2/2012 pengguofeng]���+��Ƶ����ͨ����
			if ( ptConfAllMtInfo->MtJoinedConf(byMtId)
				&& !(ptConfMtTab->GetMt(byMtId) == tSrc)
				&& ptConfMtTab->GetMtLogicChnnl(byMtId, byMode, &tLogCnnl, TRUE))
			{
				ptConfMtTab->SetMtSrc(byMtId, &tSrc, byMode);
			}
		}
	}*/

    dwMultiCastAddr = pVcInst->m_tConf.GetConfAttrb().GetSatDCastIp();
    wMultiCastPort = pVcInst->m_tConf.GetConfAttrb().GetSatDCastPort();
	
	// xsl [8/19/2006] ���Ƿ�ɢ���������Nģʽ�������鲥��ַ
	//tianzhiyong  2010/03/21 ֧��EMIXER
    /*if (tSrc.GetType() == TYPE_MCUPERI && tSrc.GetEqpType() == EQP_TYPE_MIXER )
    {
        //wRcvPort = wRcvPort + MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId;    //���ﲻ���Ƕ������������������ʱ�ٿ���
    }
    else //�����ŵ��ż���ƫ����*/
    {
        wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;
    }

    u8 byIsMulticast;
    u8 byMulticastFlag = 0;
    u8 byFirstMpId = 0;
	u32 dwSndBindIp = 0; // �鲥���͵�ַ [pengguofeng 6/6/2013]
#if defined(_8KH_) || defined(_8KI_)
	if ( g_cMcuAgent.Is8000HmMcu() )
	{
		// 8000H-M��Ҫ����dwSndBindIP���漰���鲥��ַ [pengguofeng 6/5/2013]
		dwSndBindIp = GetSwitchSndBindIp(); //ͨ�ýӿڣ�����8000H-M���ⷵ�أ�����������0
	}
#endif
    for(u16 wMpId = 0; wMpId < MAXNUM_DRI + 1; wMpId++)
    {
        if (g_cMcuVcApp.IsMpConnected(wMpId+1) || (MAXNUM_DRI == wMpId))
        {

            if (MAXNUM_DRI > wMpId)
            {
                if (0 == byFirstMpId)
                {
                    byFirstMpId = wMpId+1;
                }

                byIsMulticast = g_cMcuVcApp.GetMpMulticast(wMpId+1);
                LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "\n[StartDistrConfCast()]byIsMulticast = %d  wMpId+1 = %d\n\n", byIsMulticast, wMpId+1);
                if (1 != byIsMulticast)
                {
                    continue;
                }

                byMulticastFlag++;

                dwRcvIp = g_cMcuVcApp.GetMpIpAddr(wMpId+1);
                LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "\n[StartDistrConfCast()]dwRcvIp = 0x%x\n\n", dwRcvIp);
            }
            else
            {
                if (0 < byMulticastFlag)
                {
                    continue;
                }
                dwRcvIp = g_cMcuVcApp.GetMpIpAddr(byFirstMpId);
            }

			// 8000H��Mȡ��������ʵ�ֵ�ַ [pengguofeng 6/6/2013]
			if ( dwSndBindIp == 0 )
			{
				dwSndBindIp = dwRcvIp;
			}
			
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[StartMulticast]dwSndBindIp:%x\n", dwSndBindIp);
            //ͼ��
            if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
            {
                //��������
                LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StartDistrConfCast() Switch video\n");
                bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort,
                                       dwMultiCastAddr, wMultiCastPort, dwRcvIp, dwSndBindIp,
                                       SWITCHCHANNEL_UNIFORMMODE_NONE,
                                       INVALID_PAYLOAD,
                                       // ����RTCP����
                                       1);
                if (!bResult1)
                {
                    LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartDistrConfCast() Switch video failure!\n");
                }
            }

            //����
            if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
            {
                //��������
                LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StartDistrConfCast() Switch audio\n");
                bResult2 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, (wRcvPort+2),
                                       dwMultiCastAddr, (wMultiCastPort+2), dwRcvIp, dwSndBindIp,
                                       SWITCHCHANNEL_UNIFORMMODE_NONE,
                                       INVALID_PAYLOAD,
                                       // ����RTCP����
                                       1);
                if (!bResult2)
                {
                    LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartDistrConfCast() Switch audio failure!\n");
                }
            }
            
            //�ڶ�·��Ƶ
            if (byMode == MODE_SECVIDEO)
            {
                LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StartDistrConfCast() Switch snd video\n");
                bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, (wRcvPort+4),
                                       dwMultiCastAddr, (wMultiCastPort+4), dwRcvIp, dwSndBindIp,
                                       SWITCHCHANNEL_UNIFORMMODE_NONE,
                                       INVALID_PAYLOAD,
                                       // ����RTCP����
                                       1);
                if (!bResult1)
                {
                    LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartDistrConfCast() Switch snd video failure!\n");
                }
            }
        }
    }

	return  bResult1 && bResult2;
}

/*====================================================================
    ������      ��StopDistrConfCast
    ����        ��ֹͣ��ɢ�����鲥
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tSrc Դ�ն�
	              bySrcChnnl ͨ����
				  byMode ����ģʽ,ȱʡΪMODE_BOTH
    ����ֵ˵��  ��TRUE��������ܿ�ʼ�㲥����FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/04/13    4.0         ����        ����
====================================================================*/
BOOL32 CMpManager::StopDistrConfCast(const TMt &tSrc, u8 byMode, u8 bySrcChnnl)
{
    u32  dwSrcIp  = 0;
	u32  dwRcvIp  = 0;
	u16  wRcvPort = 0;

	u32  dwMultiCastAddr;
	u16  wMultiCastPort;
		
	//�õ�����Դ��ַ
	if (!GetSwitchInfo(tSrc, dwRcvIp, wRcvPort, dwSrcIp))
	{
		 LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StopDistrConfCast() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//�õ�MCU���鲥��ַ
    CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(tSrc.GetConfIdx());
    if ( NULL == pVcInst )
    {
        // Fatal error!
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Cannot get VC Instance(conf idx: %d) while StopDistrConfCast!\n", 
                            tSrc.GetConfIdx());
        return FALSE;
    }

    dwMultiCastAddr = pVcInst->m_tConf.GetConfAttrb().GetSatDCastIp();
    wMultiCastPort = pVcInst->m_tConf.GetConfAttrb().GetSatDCastPort();


	//�����ŵ��ż���ƫ����	
	wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;

	//ͼ��
	if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
	{
		//�Ƴ�����
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StopDistrConfCast() Switch video\n");
		StopSwitch(tSrc.GetConfIdx(), dwMultiCastAddr, wMultiCastPort, 1);
	}
		
	//����
	if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
	{
		//�Ƴ�����
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StopDistrConfCast() Switch audio\n");
		StopSwitch(tSrc.GetConfIdx(), dwMultiCastAddr, (wMultiCastPort+2), 1);
	}
    
    //�ڶ�·��Ƶ
    if (byMode == MODE_SECVIDEO)
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StopDistrConfCast() Switch snd video\n");
        StopSwitch(tSrc.GetConfIdx(), dwMultiCastAddr, (wMultiCastPort+4), 1);
    }

	return TRUE;
}


/*====================================================================
    ������      ��GetSwitchInfo
    ����        ���õ�������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tSrc Դ�ն�
	              dwSwitchIpAddr ������IP��ַ
				  wSwitchPort �����˿�
    ����ֵ˵��  ��TRUE��������ܵõ�����FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/16    1.0         ������        ����
	10/02/25				xl			  modify method for eqp
====================================================================*/
BOOL32 CMpManager::GetSwitchInfo(const TMt & tSrc, u32 &dwSwitchIpAddr, u16 &wSwitchPort, u32 &dwSrcIpAddr)
{
	// [11/29/2011 liuxu] ���ԴΪ��, ���ֱ�ӷ���
	if (tSrc.IsNull())
	{
		dwSwitchIpAddr = 0;
		wSwitchPort = 0;
		dwSrcIpAddr = 0;
		return FALSE;
	}

	u32  dwSrcIp  = 0;
	u32  dwRcvIp  = 0;
	u16  wRcvPort = 0;

	//u16  byMcuId  = tSrc.GetMcuId();
	u8   byMtId   = tSrc.GetMtId();
	u8   byEqpId  = tSrc.GetEqpId();
	u8   byPeriType;

    BOOL32 bHDVmp = FALSE;

	//�õ�����Դ�ĵ�ַ��˿�
	switch(tSrc.GetType()) 
	{

	case TYPE_MT://�ն�
		{
			CMcuVcInst * pcVcInst = g_cMcuVcApp.GetConfInstHandle(tSrc.GetConfIdx());
			if(NULL == pcVcInst)
			{
				LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "[GetSwitchInfo]pcVcInst(ConfIdx.%d) is null!\n", tSrc.GetConfIdx());
				return FALSE;
			}
			TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable(tSrc.GetConfIdx());
			if (ptConfMtTable == NULL)
			{
				LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "GetSwitchInfo() failure, Type: %d SubType: %d Id: %d ConfIdx: %d!\n", 
					   tSrc.GetType(), tSrc.GetEqpType(), tSrc.GetEqpId(),tSrc.GetConfIdx());
				return FALSE;
			}
			TMt tLocalMt;
			if( !tSrc.IsLocal() )
			{
				tLocalMt = ptConfMtTable->GetMt(pcVcInst->GetFstMcuIdFromMcuIdx(tSrc.GetMcuId()));
				byMtId = tLocalMt.GetMtId();
			}

			//GetSubMtSwitchAddr(tSrc,dwRcvIp,wRcvPort);
			ptConfMtTable->GetMtSwitchAddr(byMtId, dwRcvIp, wRcvPort);
			dwSrcIp = ptConfMtTable->GetIPAddr(byMtId);
		}
		break;

	case TYPE_MCUPERI://����
		{
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
			if( tSrc.GetEqpType() == EQP_TYPE_TVWALL)
			{
				dwRcvIp = g_cMcuVcApp.GetAnyValidMp();
				g_cMcuAgent.GetPeriInfo(byEqpId, &dwSrcIp, &byPeriType);
				dwSrcIp = ntohl(dwSrcIp);
			}
			else
			{	
				u16 wEqpStartPort = 0;
				GetSwitchInfo( byEqpId, dwRcvIp, wRcvPort, wEqpStartPort);

				//[2011/10/12/zhangli]hud��recorder��Ҫ�������ļ���eqpip
				if (tSrc.GetEqpType() == EQP_TYPE_RECORDER)
				{
					g_cMcuAgent.GetPeriInfo(byEqpId, &dwSrcIp, &byPeriType);
					dwSrcIp = ntohl(dwSrcIp);
				}
				else
				{
					dwSrcIp = g_cMcuVcApp.GetEqpIpAddr(byEqpId);
				}
			}

#else
			switch(tSrc.GetEqpType()) 
			{
			case EQP_TYPE_MIXER://������
				GetMixerSwitchAddr(byEqpId,dwRcvIp,wRcvPort);
				break;
			case EQP_TYPE_RECORDER://��¼���
				GetRecorderSwitchAddr(byEqpId,dwRcvIp,wRcvPort);
				break;
			case EQP_TYPE_BAS://����������
				GetBasSwitchAddr(byEqpId,dwRcvIp,wRcvPort);
				break;
			case EQP_TYPE_VMP://����ϳ���
				GetVmpSwitchAddr(byEqpId,dwRcvIp,wRcvPort);
				break;
			case EQP_TYPE_PRS://�����ش���
				GetPrsSwitchAddr(byEqpId,dwRcvIp,wRcvPort);
				break;
			case EQP_TYPE_TVWALL://����ǽ
				dwRcvIp = g_cMcuVcApp.GetAnyValidMp();
				break;
			default:
				break;
			}
			g_cMcuAgent.GetPeriInfo(byEqpId, &dwSrcIp, &byPeriType);
			dwSrcIp = ntohl(dwSrcIp);

#endif
			break;
		}
	default:
		break;
	}

	//�жϽ���Դ�ĵ�ַ��˿��Ƿ�Ϸ�
	if (dwRcvIp == 0 || wRcvPort == 0)
	{
        if (!(tSrc.GetType() == TYPE_MCUPERI  && tSrc.GetEqpType() == EQP_TYPE_TVWALL))
        {
		    LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "GetSwitchInfo() failure, Type: %d SubType: %d Id: %d, dwRcvIp: %d wRcvPort: %d!\n", 
			       tSrc.GetType(), tSrc.GetEqpType(), tSrc.GetEqpId(), dwRcvIp, wRcvPort);
		    return FALSE;
        }
	}

	//�жϽ���Դ�ĵ�ַ��˿��Ƿ�Ϸ�
	if (dwSrcIp == 0)
	{
        if ( tSrc.GetType() == TYPE_MCUPERI )
        {
            if (bHDVmp)
            {
                dwSrcIp = ntohl(g_cMcuVcApp.GetEqpIp(byEqpId));

                dwSrcIpAddr    = dwSrcIp;
                dwSwitchIpAddr = dwRcvIp;
	            wSwitchPort    = wRcvPort;

                return TRUE;
            }
		    LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "GetSwitchInfo() cann't get switch src IP, dwSrcIp: 0, byEqpId: %d!\n", byEqpId);
        }
        else
            LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "GetSwitchInfo() cann't get switch src IP, dwSrcIp: 0, byMtId: %d!\n", byMtId);
		return FALSE;
	}

	dwSrcIpAddr    = dwSrcIp;
	dwSwitchIpAddr = dwRcvIp;
	wSwitchPort    = wRcvPort;

	return TRUE;
}

/*=============================================================================
�� �� ���� GetSwitchInfo
��    �ܣ� �˺���ר��8000e ʹ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  u8 byEqpId
			u32 &dwSwitchIpAddr      MP�������ַ(������)
			u16 &wMcuStartPort       MCU���ն˿ں�(������)
			u16 &wEqpStartPort       ������ն˿ں�(������)
									 ���ز�����Ϊ������
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2009/2/16   4.0		�ܹ��                  ����
=============================================================================*/
BOOL32 CMpManager::GetSwitchInfo( u8 byEqpId, u32 &dwSwitchIpAddr, u16 &wMcuStartPort, u16 &wEqpStartPort )
{
    CApp * pcApp = (CApp *)&g_cMpSsnApp;
    CMpSsnInst * pcMpSsnInst = NULL;
    u16 wInstId = 1;
    for ( wInstId = 1; wInstId <= MAXNUM_DRI; wInstId++ )
    {
        pcMpSsnInst = (CMpSsnInst *)pcApp->GetInstance(wInstId);
        if ( pcMpSsnInst->GetMpIp() != 0 )
        {
            dwSwitchIpAddr = pcMpSsnInst->GetMpIp();    //�ҵ���һ��Ϊֹ
			break;
        }
    }
    if ( wInstId > MAXNUM_DRI )
    {
        dwSwitchIpAddr = 0;
        wMcuStartPort = 0;
        wEqpStartPort = 0;
		
        LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[GetSwitchInfo] byEqpId.%d failed for no mp found\n", byEqpId );
        return FALSE;
    }
	
    if( byEqpId >= MIXERID_MIN && byEqpId <= MIXERID_MAX  )
    {
		TEqpMixerInfo tTempMixerInfo;
		if (SUCCESS_AGENT != g_cMcuAgent.GetEqpMixerCfg( byEqpId , &tTempMixerInfo ))
		{
			LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR,"[GetSwitchInfo] GetEqpMixerCfg(%d) failed!\n", byEqpId);
			return FALSE;	
		}
		wEqpStartPort = tTempMixerInfo.GetEqpRecvPort();
		wMcuStartPort = tTempMixerInfo.GetMcuRecvPort();
		LogPrint( LOG_LVL_DETAIL, MID_MCU_MPMGR,"[GetSwitchInfo]m_byEqpId:%d MixMem:%dGetSwitchInfo--wMcuStartPort: %d, wEqpStartPort: %d\n", 
			byEqpId,tTempMixerInfo.GetMaxChnInGrp(),wMcuStartPort, wEqpStartPort);
        return TRUE;
    }
    
    if( byEqpId >= BASID_MIN && byEqpId <= BASID_MAX )
    {
        wMcuStartPort = BAS_MCU_STARTPORT + (byEqpId-BASID_MIN)*BAS_MCU_PORTSPAN;
		wEqpStartPort = BAS_EQP_STARTPORT + (byEqpId-BASID_MIN) * PORTSPAN;
/*#if	defined(_8KE_)
		wEqpStartPort = BAS_EQP_STARTPORT + (byEqpId-BASID_MIN)*BAS_8KE_PORTSPAN;
#elif defined(_8KH_)
		wEqpStartPort = BAS_EQP_STARTPORT + (byEqpId-BASID_MIN)*BAS_8KH_PORTSPAN;
#else
        wEqpStartPort = BAS_EQP_STARTPORT +  (byEqpId-BASID_MIN)*BAS_8KE_PORTSPAN;//(byEqpId-BASID_MIN)*BAS_MCU_PORTSPAN; 
#endif*/
        return TRUE;
    }
    
    if( byEqpId >= VMPID_MIN && byEqpId <= VMPID_MAX )
    {
        wMcuStartPort = VMP_MCU_STARTPORT + (byEqpId-VMPID_MIN)*VMP_MCU_PORTSPAN;
        wEqpStartPort = VMP_EQP_STARTPORT; 
        return TRUE;
    }
    
    if( byEqpId >= PRSID_MIN && byEqpId <= PRSID_MAX )
    {
        wMcuStartPort = PRS_MCU_STARTPORT + (byEqpId-PRSID_MIN)*PRS_MCU_PORTSPAN;
        wEqpStartPort = PRS_EQP_STARTPORT; 
        return TRUE;
    }
    
    if ( byEqpId >= RECORDERID_MIN && byEqpId <= RECORDERID_MAX )
    {
        wMcuStartPort = REC_MCU_STARTPORT + (byEqpId-RECORDERID_MIN)*REC_MCU_PORTSPAN;
        wEqpStartPort = REC_EQP_STARTPORT; 
        return TRUE;
    }
	
    wMcuStartPort = 0;
    wEqpStartPort = 0;
    LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[GetSwitchInfo] byEqpId.%d failed\n", byEqpId );
    return FALSE;
}

/*====================================================================
    ������      ��SetSwitchBridge
    ����        �����ý�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tSrc Դ�ն�
	              bySrcChnnl ͨ����
				  byMode ����ģʽ,ȱʡΪMODE_BOTH
    ����ֵ˵��  ��TRUE��������ܿ�ʼ�㲥����FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/16    1.0         ������        ����
====================================================================*/
BOOL32 CMpManager::SetSwitchBridge(const TMt & tSrc, u8 bySrcChnnl, u8 byMode, BOOL32 bSrcHDBas,u16 wSpyStartPort)
{
	u32  dwSrcIp = 0;
	u32  dwRcvIp = 0;
	u16  wRcvPort = 0;

	TMt tSrcMt = tSrc;

	//�õ�����Դ��ַ
	if (!GetSwitchInfo(tSrcMt, dwRcvIp, wRcvPort, dwSrcIp))
	{
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "SetSwitchBridge() failure because of can't get switch info!\n");
		return FALSE;
	}

	//����IP��ַ���ҽ���MP���
	u8 byMpId = g_cMcuVcApp.FindMp(dwRcvIp);
	 
    wRcvPort = wRcvPort +PORTSPAN*bySrcChnnl;
	LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "Get Mt<%d> Ip:%x AssignMpId:%d Mp IP:%x Port:%d Mode:%d\n",
		tSrcMt.GetMtId(), dwSrcIp, byMpId, dwRcvIp, wRcvPort, byMode);

	//zjj20100201
	//  [11/26/2009 pengjie] Modify ������ش�֧��
	if(wSpyStartPort != SPY_CHANNL_NULL)
	{
		wRcvPort = wSpyStartPort;
	}
	// End

	//����������
	u32 dwDstIp = 0;
	BOOL32 bResult = TRUE;
//	BOOL32 bResultAnd = TRUE;
    dwRcvIp = 0;
    u8  byFirstMpId = 0;
	u8 byTmpLastMpId = 0; // ��¼��һ�����ߵ�MP ID [pengguofeng 5/23/2012]
	u8 byLastMpId = 0;
    for(s32  nMpId = 0; nMpId < MAXNUM_DRI; nMpId++)
	{	   
	   if (g_cMcuVcApp.IsMpConnected(nMpId+1))
	   {
            //��һ��mp ����һ��Ϊ�����ն�mp
            if ( 0 == dwRcvIp || byMpId == nMpId + 1 )
            {
                if ( 0 == dwRcvIp )
                {
                    byFirstMpId = nMpId + 1;
                }
               dwRcvIp = g_cMcuVcApp.GetMpIpAddr(nMpId+1);
				byTmpLastMpId = nMpId + 1;
               continue;
            }            

            //��һ��mp                   
            dwDstIp = g_cMcuVcApp.GetMpIpAddr(nMpId+1);

			if ( byMpId != nMpId + 1
				&& byTmpLastMpId != byFirstMpId )
			{
				dwSrcIp = g_cMcuVcApp.GetMpIpAddr( byTmpLastMpId);
			}

			LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "set bridge from %x[which to send] to %x, last src:%x\n",
				dwRcvIp, dwDstIp, dwSrcIp);
			bResult = bResult && StartBridgeSwitch(byMode, tSrcMt, dwSrcIp, dwRcvIp, wRcvPort, dwDstIp, bSrcHDBas);

			//bResultAnd = bResultAnd && bResult;
			dwSrcIp = dwRcvIp;

           //����mp��Ŀ�ĵ�ַ��Ϊ��һ��mp�Ľ��յ�ַ
            dwRcvIp = dwDstIp;
	   } 
	}

    //��Դ���ǵ�һ��mp����Ҫ����β����ͷ�����Ž����������γ������Ž���
    if ( byMpId > 1 && byMpId != byFirstMpId && g_cMcuVcApp.IsMpConnected(byFirstMpId) )
    {
        dwDstIp = g_cMcuVcApp.GetMpIpAddr(byFirstMpId);

		LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "set bridge from %x[which to send] to %x, last src:%x\n",
			dwRcvIp, dwDstIp, dwSrcIp);

       bResult = StartBridgeSwitch(byMode, tSrcMt, dwSrcIp, dwRcvIp, wRcvPort, dwDstIp, bSrcHDBas);

//		bResultAnd = bResultAnd && bResult;
    }

//    return bResultAnd; 
	return bResult;
}

/*====================================================================
    ������      ��RemoveSwitchBridge
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tSrc ����Դ
	              bySrcChnnl Դͨ����
				  byMode ����ģʽ,ȱʡΪMODE_BOTH
    ����ֵ˵��  ��TRUE���������ֹͣ�㲥����FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/16    1.0         ������        ����
====================================================================*/
BOOL32 CMpManager::RemoveSwitchBridge(const TMt & tSrc,u8 bySrcChnnl,u8 byMode, BOOL32 bEqpHDBas,u16 wSpyStartPort )
{
	u32  dwSrcIp = 0;
	u32  dwRcvIp = 0;
	u16  wRcvPort = 0;
 
	//�õ�����Դ��ַ
	if( !GetSwitchInfo( tSrc, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "RemoveSwitchBridge() failure because of can't get switch info!");
		return FALSE;
	}

	//����IP��ַ���ҽ���MP���
	u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	
	wRcvPort = wRcvPort + PORTSPAN*bySrcChnnl;
	//zjj20100201
	//  [11/26/2009 pengjie] Modify ������ش�֧��
	if(wSpyStartPort != SPY_CHANNL_NULL)
	{
		wRcvPort = wSpyStartPort;
	}
	// End
  
	//���������
	u32 dwDstIp;
    for(s32  nMpId = 0; nMpId < MAXNUM_DRI; nMpId++)
	{	   
	   if( g_cMcuVcApp.IsMpConnected( nMpId+1 ) && (byMpId != nMpId + 1))
	   {
		    dwDstIp = g_cMcuVcApp.GetMpIpAddr(nMpId+1);
			switch(byMode)
			{
			case MODE_VIDEO:
			    StopSwitch(tSrc.GetConfIdx(), dwDstIp,wRcvPort);
				break;
			case MODE_AUDIO:
				StopSwitch(tSrc.GetConfIdx(), dwDstIp,wRcvPort+2);
				break;
			case MODE_BOTH:
				StopSwitch(tSrc.GetConfIdx(), dwDstIp,wRcvPort);
                StopSwitch(tSrc.GetConfIdx(), dwDstIp,wRcvPort+2);
				break;
			case MODE_SECVIDEO:
                if (bEqpHDBas)
                {
                    StopSwitch(tSrc.GetConfIdx(), dwDstIp,wRcvPort);
                }
                else
                {
			        StopSwitch(tSrc.GetConfIdx(), dwDstIp,wRcvPort + 4);
                }
				break;
			default:
				break;
			}
	   } 
	}

    return TRUE; 
}

/*=============================================================================
�� �� ���� StartBridgeSwitch
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byMode
           TMt tSrcMt
           u32 dwSrcIp
           u32 dwRcvIp
           u16 wRcvPort
           u32 dwDstIp
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/9/14  4.0			������                  ����
=============================================================================*/
BOOL32 CMpManager::StartBridgeSwitch(u8		byMode, 
									 TMt	tSrcMt,
									 u32	dwSrcIp,
									 u32	dwRcvIp,
									 u16	wRcvPort,
									 u32	dwDstIp, BOOL32 bSrcHDBas)
{
    BOOL32 bResult;
    switch(byMode)
    {
    case MODE_VIDEO:
        bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort, dwDstIp, wRcvPort, 0, 0, SWITCHCHANNEL_UNIFORMMODE_NONE);
        break;
    case MODE_AUDIO:
        bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort+2, dwDstIp, wRcvPort+2, 0, 0, SWITCHCHANNEL_UNIFORMMODE_NONE);
        break;
    case MODE_BOTH:
        bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort, dwDstIp, wRcvPort, 0, 0, SWITCHCHANNEL_UNIFORMMODE_NONE);
        bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort+2, dwDstIp, wRcvPort+2, 0, 0, SWITCHCHANNEL_UNIFORMMODE_NONE);
        break;
	case MODE_SECVIDEO:
		//zbq[09/05/2008] ���������˫���Ž���������4����
		if (bSrcHDBas)
		{
			bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort, dwDstIp, wRcvPort, 0, 0, SWITCHCHANNEL_UNIFORMMODE_NONE);
		}
		else
		{
			bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort+4, dwDstIp, wRcvPort+4, 0, 0, SWITCHCHANNEL_UNIFORMMODE_NONE);
		}
		break;
    default:
		bResult = FALSE;
        break;
    }

    return bResult;
}


/*====================================================================
    ������      ��StartSwitchToDst
    ����        ����ָ��Դ���ݽ�����Ŀ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��TRUE�����������ֱ�ӷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    10/08/18    4.6         Ѧ��	        ����
====================================================================*/
BOOL32 CMpManager::StartSwitchToDst(TSwitchDstInfo &tSwitchDstInfo, u16 wSpyStartPort /* = SPY_CHANNL_NULL */)
{
	
	LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StartSwitchToDst()--param is: mode: %u, srcChannel: %u, VidIp: 0x%x, VidPort: %d\n\tAudIp:0x%x, AudPort:%d, srcmt(%d,%d)\n", 
		tSwitchDstInfo.m_byMode, 
		tSwitchDstInfo.m_bySrcChnnl, 
		tSwitchDstInfo.m_tDstVidAddr.GetIpAddr(),
		tSwitchDstInfo.m_tDstVidAddr.GetPort(),
		tSwitchDstInfo.m_tDstAudAddr.GetIpAddr(),
		tSwitchDstInfo.m_tDstAudAddr.GetPort(),
		tSwitchDstInfo.m_tSrcMt.GetMcuId(),
		tSwitchDstInfo.m_tSrcMt.GetMtId()
		);

	TMt tSrc = tSwitchDstInfo.m_tSrcMt;
	u8	byMode = tSwitchDstInfo.m_byMode;
	u8  bySrcChnnl = tSwitchDstInfo.m_bySrcChnnl;

	u32 dwSrcIp = 0;
	u32	dwRcvIp = 0;
	u16	wRcvPort = 0;

	BOOL32 bResult1 = TRUE;
	BOOL32 bResult2 = TRUE;
	
	// [9/30/2010 xliang] local conversion if tSrc is TYPE_MT
	if( tSrc.GetType() == TYPE_MT )
	{
		TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable(tSrc.GetConfIdx());
		if (ptConfMtTable == NULL)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchToDst() failure, Type: %d SubType: %d Id: %d ConfIdx: %d!\n", 
				tSrc.GetType(), tSrc.GetEqpType(), tSrc.GetEqpId(),tSrc.GetConfIdx());
			return FALSE;
		}
		
		CMcuVcInst *pcIns = g_cMcuVcApp.GetConfInstHandle(tSrc.GetConfIdx()) ;
		if( pcIns == NULL  || !pcIns->m_tConf.m_tStatus.IsOngoing())
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchToDst() failure for finding invalid VcInst, Type: %d SubType: %d Id: %d ConfIdx: %d!\n", 
				tSrc.GetType(), tSrc.GetEqpType(), tSrc.GetEqpId(),tSrc.GetConfIdx());
			return FALSE;
		}
		
		
		if( !tSrc.IsLocal() )
		{
			u8 byMcuId = pcIns->GetFstMcuIdFromMcuIdx(tSrc.GetMcuId());
			tSrc = ptConfMtTable->GetMt(byMcuId);
			LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[StartSwitchToDst] tSrc change to local is (%d,%d)\n", tSrc.GetMcuId(), tSrc.GetMtId());
		}
		
	}
	
	//�õ�����Դ��ַ
	if( !GetSwitchInfo( tSrc, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchToDst() failure because of can't get switch info!");
		return FALSE ;
	}
	
	//�����ŵ��ż���ƫ����	
	wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;
	
	//������ش�֧��
	if( wSpyStartPort != SPY_CHANNL_NULL )
	{
		wRcvPort = wSpyStartPort;
	}

	//Ŀ�ĵ�ַ
	u32 dwDstIp		= 0;//tSwitchDstInfo.m_tDstVidAddr.GetIpAddr();
	u16 wVidDstPort	= tSwitchDstInfo.m_tDstVidAddr.GetPort();
	u16 wAudDstPort = tSwitchDstInfo.m_tDstAudAddr.GetPort();

	//ͼ��
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		dwDstIp		= tSwitchDstInfo.m_tDstVidAddr.GetIpAddr();
		bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort, dwDstIp, wVidDstPort );		
	}
	
	//����
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		dwDstIp		= tSwitchDstInfo.m_tDstAudAddr.GetIpAddr();
		bResult2 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, (wRcvPort+2), dwDstIp, wAudDstPort );
	}
	
    return bResult1&&bResult2; 
	
}

/*====================================================================
    ������      ��StopSwitchToDst
    ����        ��ֹͣ������Ŀ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    10/08/18    4.6         Ѧ��	        ����
====================================================================*/
void CMpManager::StopSwitchToDst(TSwitchDstInfo &tSwitchDstInfo)
{
	u8 byMode = tSwitchDstInfo.m_byMode;
	TMt tSrcMt = tSwitchDstInfo.m_tSrcMt;
	u8 byConfIdx = tSrcMt.GetConfIdx();

	TTransportAddr tAddr = ( MODE_VIDEO == byMode)? tSwitchDstInfo.m_tDstVidAddr: tSwitchDstInfo.m_tDstAudAddr;
	u32 dwDstIp = tAddr.GetIpAddr();
	u16 wVidDstPort = tSwitchDstInfo.m_tDstVidAddr.GetPort();
	u16 wAudDstPort = tSwitchDstInfo.m_tDstAudAddr.GetPort();
	
	//ͼ��	
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		StopSwitch( byConfIdx, dwDstIp, wVidDstPort );
	}
	
	//����
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		StopSwitch( byConfIdx, dwDstIp, wAudDstPort );
	}
}


/*====================================================================
    ������      ��StartSwitchToMc
    ����        ����ָ���ն����ݽ��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tSrc, Դ
				  bySrcChnnl, Դ���ŵ���
				  wMcInstId, �������̨ʵ����
				  byDstChnnl, Ŀ���ŵ������ţ�ȱʡΪ0
   				  byMode, ����ģʽ��ȱʡΪMODE_BOTH
   ����ֵ˵��  ��TRUE�����������ֱ�ӷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/18    1.0         LI Yi         ����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
BOOL32 CMpManager::StartSwitchToMc( const TMt & tSrc, u8 bySrcChnnl, u16 wMcInstId, u8 byDstChnnl, u8 byMode, u16 wSpyPort )
{
	u32  dwSrcIp = 0;
	u32	dwRcvIp;
	u16	wRcvPort;
	u8  byChannelNum;
	TLogicalChannel tLogicalChannel;
	BOOL32 bResult1 = TRUE;
	BOOL32 bResult2 = TRUE;
	BOOL32 bResult3 = TRUE;

	//�õ�����Դ��ַ
	if( !GetSwitchInfo( tSrc, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchToMc(%d,%d) get switch info fail!\n", tSrc.GetMcuId(), tSrc.GetMtId());
		return FALSE ;
	}

	//�����ŵ��ż���ƫ����	
	wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;

	//  [11/27/2009 pengjie] Modify ������ش�֧��
	if( wSpyPort != SPY_CHANNL_NULL )
	{
		wRcvPort = wSpyPort;
	}
	// End

    //ͼ��
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		if( g_cMcuVcApp.GetMcLogicChnnl( wMcInstId, MODE_VIDEO, &byChannelNum, &tLogicalChannel ) && 
			( byChannelNum > byDstChnnl ) )
		{
			bResult1 = StartSwitch( tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort, tLogicalChannel, byDstChnnl );			
		}
		else
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR,  "Cannot get video send address to Meeting Console%u!\n", wMcInstId );
		}

	}
	
	//����
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		if( g_cMcuVcApp.GetMcLogicChnnl( wMcInstId, MODE_AUDIO, &byChannelNum, &tLogicalChannel ) && 
			( byChannelNum > byDstChnnl ) )
		{
			bResult2 = StartSwitch( tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, (wRcvPort+2), tLogicalChannel, byDstChnnl );			
		}
		else
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "Cannot get audio send address to Meeting Console%u!\n", wMcInstId );
		}
	}
	
	//˫��
	if ( byMode == MODE_SECVIDEO )
	{
		if( g_cMcuVcApp.GetMcLogicChnnl( wMcInstId, MODE_SECVIDEO, &byChannelNum, &tLogicalChannel ) && 
			( byChannelNum > byDstChnnl ) )
		{
			bResult3 = StartSwitch( tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, (wRcvPort+4), tLogicalChannel, byDstChnnl );			
		}
		else
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "Cannot get SecVideo send address to Meeting Console%u!\n", wMcInstId );
		}
	}
    return bResult1&&bResult2&&bResult3; 
}

/*====================================================================
    ������      ��StopSwitchToMc
    ����        ��ֹͣ�����ݽ��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����wMcInstId,  ʵ����
				  byDstChnnl, �ŵ������ţ�ȱʡΪ0
				  byMode, ����ģʽ��ȱʡΪMODE_BOTH
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/18    1.0         LI Yi         ����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
void CMpManager::StopSwitchToMc( u8 byConfIdx, u16 wMcInstId,  u8 byDstChnnl, u8 byMode )
{
    u8  byChannelNum;
	TLogicalChannel tLogicalChannel;
	
	//ͼ��	
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		if( g_cMcuVcApp.GetMcLogicChnnl( wMcInstId, MODE_VIDEO, &byChannelNum, &tLogicalChannel ) 
			&& ( byChannelNum > byDstChnnl ) )
		{
			StopSwitch( byConfIdx, tLogicalChannel, byDstChnnl );
		}
	}
	
	//����
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		if( g_cMcuVcApp.GetMcLogicChnnl( wMcInstId, MODE_AUDIO, &byChannelNum, &tLogicalChannel ) 
			&& ( byChannelNum > byDstChnnl ) )
		{
			StopSwitch( byConfIdx, tLogicalChannel, byDstChnnl );
		}
	}

	//˫��
	if( byMode == MODE_SECVIDEO )
	{
		if( g_cMcuVcApp.GetMcLogicChnnl( wMcInstId, MODE_SECVIDEO, &byChannelNum, &tLogicalChannel ) 
			&& ( byChannelNum > byDstChnnl ) )
		{
			StopSwitch( byConfIdx, tLogicalChannel, byDstChnnl );
		}
	}
}


/*====================================================================
    ������      ��StartSwitchToBrd
    ����        ����ָ���ն˵���Ƶ��Ϊ�㲥Դ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tSrc, Դ
				  bySrcChnnl,  Դ���ŵ���
				  u8 bySpecMpId[IN] ���������ת����ID��ֵΪ0��ʾ��������ת���壬ֵ��0��ʾ���������ת����
    ����ֵ˵��  ��TRUE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/03/21    4.0         ����        ����
	12/04/12	4.7         chendaiei	  modify
====================================================================*/
BOOL32 CMpManager::StartSwitchToBrd( const TMt &tSrc, u8 bySrcChnnl, BOOL32 bForce, u16 wSpyStartPort,u8 bySpecMpId)
{
	u32  dwSrcIp = 0;
	u32  dwRcvIp = 0;
	u16  wRcvPort = 0;
	BOOL32 bResult = TRUE;

	//�õ�����Դ��ַ
	if( !GetSwitchInfo( tSrc, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
		 LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchToBrd() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//�����ŵ��ż���ƫ����
	wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl; 

	//  [11/26/2009 pengjie] Modify ������ش�֧��
	if( wSpyStartPort != SPY_CHANNL_NULL)
	{
		wRcvPort = wSpyStartPort;
	}
	// End

    //��������

    //����IP��ַ���ҽ���MP���
    u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
    if( 0 == byMpId )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "No MP's Addrs is 0x%x in StartSwitchToBrd(),please check switch recv IP!\n",dwRcvIp);
        return FALSE;
    }
    
    //���콻���ŵ� 
    TSwitchChannel tSwitchChannel;
    tSwitchChannel.SetSrcMt( tSrc );
    tSwitchChannel.SetSrcIp( dwSrcIp );
    tSwitchChannel.SetRcvIP( dwRcvIp );
    tSwitchChannel.SetRcvPort( wRcvPort );
    
    // �����ӹ㲥Դ��Ϣ������MP
    CServMsg cServMsg;
    cServMsg.SetConfIdx(tSrc.GetConfIdx());
//     cServMsg.SetMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));
//     u8 byForce = bForce ? 1 : 0;
//     cServMsg.CatMsgBody(&byForce, sizeof(byForce));
    
	if( bySpecMpId != 0 )
	{
	    cServMsg.SetMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));
	    u8 byForce = bForce ? 1 : 0;
	    cServMsg.CatMsgBody(&byForce, sizeof(byForce));
		g_cMpSsnApp.SendMsgToMpSsn( bySpecMpId, 
			MCU_MP_ADDBRDSRCSWITCH_REQ, 
			cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "Send MCU_MP_ADDBRDSRCSWITCH_REQ from RecvMp %s:%d to Mp%d.\n",
                 StrOfIP(tSwitchChannel.GetRcvIP()), tSwitchChannel.GetRcvPort(),bySpecMpId);
	}
    else
	{
	// for loop [pengguofeng 5/16/2012]
// 		g_cMpSsnApp.BroadcastToAllMpSsn(MCU_MP_ADDBRDSRCSWITCH_REQ, 
//                                     cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		TSwitchChannel tTmpChannel;
		for ( u8 byMpIdLoop = 1; byMpIdLoop <= MAXNUM_DRI; byMpIdLoop++)
		{
			if ( g_cMcuVcApp.IsMpConnected(byMpIdLoop) )
			{
				tTmpChannel = tSwitchChannel;
				if ( byMpIdLoop != byMpId)
				{
					// ����Դ��IP����ֹMP���д� [pengguofeng 5/16/2012]
					tTmpChannel.SetSrcIp(0);
				}
			    cServMsg.SetMsgBody((u8 *)&tTmpChannel, sizeof(tTmpChannel));				
			    u8 byForce = bForce ? 1 : 0;
			    cServMsg.CatMsgBody(&byForce, sizeof(byForce));
				g_cMpSsnApp.SendMsgToMpSsn( byMpIdLoop, 
					MCU_MP_ADDBRDSRCSWITCH_REQ, 
					cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "Send MCU_MP_ADDBRDSRCSWITCH_REQ from RecvMp %s:%d srcIp:%x to Mp%d.\n",
			        StrOfIP(tSwitchChannel.GetRcvIP()), tSwitchChannel.GetRcvPort(), tTmpChannel.GetSrcIp(), byMpIdLoop);
			}
		}
    
	}
    
    return TRUE;     
}

/*====================================================================
    ������      ��StartSwitchFromBrd
    ����        �����㲥Դ������������ָ���ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tDst, ����Ŀ��
    ����ֵ˵��  ��TRUE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/03/21    4.0         ����        ����
====================================================================*/
BOOL32 CMpManager::StartSwitchFromBrd(const TMt &tSrc, u8 bySrcChnnl, u8 byDstMtNum, const TMt* const ptDstBase, u16 wSpyStartPort)
{
    CMcuVcInst * pcVcInst = g_cMcuVcApp.GetConfInstHandle(tSrc.GetConfIdx());
	if (!pcVcInst)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchFromBrd Get pcVcint failed!\n");
        return FALSE;
	}

    TConfInfo  * ptConfInfo = &pcVcInst->m_tConf;
    TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable( tSrc.GetConfIdx() );
    u8 bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_NONE;
    u8 byUniformPayload = INVALID_PAYLOAD;
    u8 byIsSwitchRtcp = 0;
    u32 dwSrcMpIp = 0;
    u32 dwSrcIp = 0;
    u32 dwMpIP = 0;
    u32 dwDisIp = 0;
    u32 dwRcvIp = 0;
    u8 byManuID = 0;    
    u16 wRcvPort = 0;
    TLogicalChannel tLogicChannel;

    if( ptConfMtTable == NULL || pcVcInst == NULL || ptConfInfo == NULL)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchFromBrd() param err. ptMtTable.0x%x, ptVcInst.0x%x!\n", ptConfMtTable, pcVcInst);
        return FALSE;
    }

    //�õ�����Դ��ַ
    if( !GetSwitchInfo( tSrc, dwSrcMpIp, wRcvPort, dwSrcIp ) )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchFromBrd() failure because of can't get switch info!\n");
        return FALSE ;
	}
    //�����ŵ��ż���ƫ����
    wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;

	//  [11/26/2009 pengjie] Modify �����໥��֧��
	if( wSpyStartPort != SPY_CHANNL_NULL )
	{
		wRcvPort = wSpyStartPort;
	}
	// End
	u16 wMMcuIdx = INVALID_MCUIDX;
	if( !pcVcInst->m_tCascadeMMCU.IsNull() )
	{
		u8 byId = pcVcInst->m_tCascadeMMCU.GetMtId();
		pcVcInst->m_tConfAllMcuInfo.GetIdxByMcuId( &byId,1,&wMMcuIdx );
	}

    CServMsg cServMsg;
    cServMsg.SetConfIdx(tSrc.GetConfIdx());    
    TSwitchChannelExt tSwitchChannel;

	// liuxu, ��ȡ����mc table
	TConfOtherMcTable * pConfOtherMc = pcVcInst->GetConfOtherMc();
	BOOL32 bSrcIsMMcu = FALSE;
	
	if (NULL != pConfOtherMc)
	{
		if (NULL != pConfOtherMc->GetMcInfo(wMMcuIdx)
			&& pConfOtherMc->GetMcInfo(wMMcuIdx)->m_tSpyMt == tSrc)
		{
			bSrcIsMMcu = TRUE;
		}			
	}

    for (u8 byMpLoop = 1; byMpLoop <= MAXNUM_DRI; byMpLoop ++)
    {
        if ( !g_cMcuVcApp.IsMpConnected(byMpLoop) )
        {
            continue;
        }

        cServMsg.SetDstDriId( byMpLoop );
        cServMsg.SetMsgBody();
        
        dwMpIP = g_cMcuVcApp.GetMpIpAddr( byMpLoop );

        u8 byMpId = 0;
		
        for ( u8 byMtLoop = 0; byMtLoop < byDstMtNum; byMtLoop ++ )
        {
			if( ptDstBase[byMtLoop].IsNull() )
			{
				continue;
			}

			// ֪ͨ���ǽ��յ�ַ [pengguofeng 2/21/2013]
			TMt tDstMt = ptDstBase[byMtLoop];
			if ( pcVcInst->IsMultiCastMt(tDstMt.GetMtId()) )
			{
				pcVcInst->ChangeSatDConfMtRcvAddr(tDstMt.GetMtId(), LOGCHL_VIDEO, TRUE);
				continue;
			}			

            // ÿ�δ���ǰMP������ն�
            byMpId = ptConfMtTable->GetMpId( ptDstBase[byMtLoop].GetMtId() );
            if ( byMpId != byMpLoop )
            {
                continue;
            }

            // zbq [06/29/2007] �ش�Դ�Ĺ㲥���������滻����ͨ�ش�����
            if (!pcVcInst->m_tCascadeMMCU.IsNull()  &&
				bSrcIsMMcu &&
                pcVcInst->m_tCascadeMMCU == ptDstBase[byMtLoop] )
            {
                LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MPMGR,  "[StartSwitchFromBrd] tSrc.%d create no brd switch to MMcu.%d\n", 
                               tSrc.GetMtId(), pcVcInst->m_tCascadeMMCU.GetMtId() );
                continue;
            }

            byManuID = ptConfMtTable->GetManuId(ptDstBase[byMtLoop].GetMtId());     
            ptConfMtTable->GetMtLogicChnnl(ptDstBase[byMtLoop].GetMtId(), LOGCHL_VIDEO, &tLogicChannel, TRUE);
            
			TConfAttrb tConfAtrrb = ptConfInfo->GetConfAttrb();
            if (( TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_VMP == tSrc.GetEqpType() && MT_TYPE_MT != ptDstBase[byMtLoop].GetMtType() &&
                tConfAtrrb.IsResendLosePack()) ||
                tSrc == ptDstBase[byMtLoop]) //  xsl [1/17/2006] (�⳧��mt��mcu)����Ƶ�����������Լ�ʱ������rtcp����
            {
                byIsSwitchRtcp = 0;
            }
            else
            {
                byIsSwitchRtcp = 1;
            }
            
            if (byManuID == MT_MANU_KDC || byManuID == MT_MANU_KDCMCU)
            {
                bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_NONE;
            }
            else
            {
                bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_VALID;
                if (TYPE_MT == tSrc.GetType() && TYPE_MT == ptDstBase[byMtLoop].GetType())
                {
                    if (tLogicChannel.GetChannelType() >= AP_MIN && tLogicChannel.GetChannelType() <= AP_MAX)
                    {
                        byUniformPayload = tLogicChannel.GetChannelType();
                    }
                }
            }
            
            //�õ�Ŀ��Mt�Ľ�����Ϣ                    
            if( dwMpIP != dwSrcMpIp )//��Ҫת��
            {
                dwDisIp = dwSrcMpIp;
            }
            else
            {
                dwDisIp = 0;                
            }                        
            dwRcvIp = dwMpIP;
            //��������
            
            //���콻���ŵ�             
            tSwitchChannel.Clear();
            tSwitchChannel.SetSrcMt( tSrc );
            tSwitchChannel.SetSrcIp( dwSrcIp );
            tSwitchChannel.SetRcvIP( dwRcvIp );
            tSwitchChannel.SetRcvPort( wRcvPort );
            tSwitchChannel.SetDisIp( dwDisIp );
            tSwitchChannel.SetRcvBindIP( dwRcvIp );
            tSwitchChannel.SetDstIP( tLogicChannel.m_tRcvMediaChannel.GetIpAddr() );			
        	tSwitchChannel.SetDstPort( tLogicChannel.m_tRcvMediaChannel.GetPort() );
			//���������ļ���ȡ�Ƿ�Ҫ���ٵ�ip/port����dstip��port�Ƿ���Ҫ���ٵ�
			if ( g_cMcuVcApp.IsNeedMapDstIpAddr( htonl(tSwitchChannel.GetDstIP()) ) )
			{
				//���ٵ�ipΪĿ���ն�����ת����ip���˿�Ϊ����ͨ���Ķ˿�
				u32 dwMappedIp = 0;
				u16 wMappedPort = 0 ;
				u32 dwDstSrcIp = 0;
				GetSwitchInfo( ptDstBase[byMtLoop],dwMappedIp,wMappedPort,dwDstSrcIp);
				if ( dwMappedIp == 0 || wMappedPort == 0 )//��ȡʧ�ܣ����ù̶���ip�Ͷ˿�
				{
					StaticLog("[StartSwitchFromBrd]Get dstmt(mtid:%d) switchip and port failed\n",ptDstBase[byMtLoop].GetMtId());
					//����Ϊrecvip��port
					dwMappedIp = dwRcvIp;
					wMappedPort = DEFAULT_MAPPORT;

				}
				//�ɹ�����ȡ������ip�Ͷ˿�
				tSwitchChannel.SetMapIp( dwMappedIp );
				tSwitchChannel.SetMapPort(wMappedPort);//��Ƶ�Ķ˿ڲ���Ҫ+2֮���
				
				
			}
            
            // ׼������ǰMP����Ϣ��
            cServMsg.CatMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));                  
            cServMsg.CatMsgBody((u8 *)&bySwitchChannelMode, sizeof(u8));
            cServMsg.CatMsgBody((u8 *)&byUniformPayload, sizeof(u8));            
            cServMsg.CatMsgBody((u8 *)&byIsSwitchRtcp, sizeof(u8));

            LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "Send MCU_MP_ADDBRDDSTSWITCH_REQ for Mt.%d(%s:%d) to Mp.%d\n",
                ptDstBase[byMtLoop].GetMtId(),
                StrOfIP(tSwitchChannel.GetDstIP()), tSwitchChannel.GetDstPort(), byMpLoop);
            LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "dwSrcIp:%s dwRcvIP:%s,dwDisIP:%s\n",StrOfIP(dwSrcIp),StrOfIP(dwRcvIp),StrOfIP(dwDisIp));
        }
        if ( cServMsg.GetMsgBodyLen() > 0 )
        {
            g_cMpSsnApp.SendMsgToMpSsn( byMpLoop, 
                                        MCU_MP_ADDBRDDSTSWITCH_REQ, 
                                        cServMsg.GetServMsg(), 
                                        cServMsg.GetServMsgLen());
			LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, " Send MCU_MP_ADDBRDDSTSWITCH_REQ to Mp.%d !!!!!!!!!!!!!!!!! \n", byMpLoop );
        }
    }

    return TRUE;
}

/*====================================================================
    ������      ��StartSwitchEqpFromBrd
    ����        �����㲥Դ����������������
    �㷨ʵ��    ��Ŀǰ�㲥Դ������Ľ��������õ�ת����Ϊ�㲥Դ���ڵ�ת���壬
	              ����Ϊ�����������mcu����������ת���壬��Ϊ������ת�����
				  ��ʱmcu���Զ����·���
    ����ȫ�ֱ�����
    �������˵����const TMt &tSrc:����Դ
				  u8 bySrcChnnl:  Դͨ��
				  u8 byEqpId:     Ŀ������ID
				  u16 wDstChnnl:  Ŀ������ͨ����
    ����ֵ˵��  ��TRUE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/08/07    4.0         �ű���        ����
	10/02/25	4.6			Ѧ��		  ����8000E
====================================================================*/
BOOL32 CMpManager::StartSwitchEqpFromBrd( const TMt &tSrc, u16 bySrcChnnl, u8 byEqpId, u16 wDstChnnl)
{
    u8  byChannelNum = 0;
    u32 dwSrcIp = 0;
    u32 dwRcvIp = 0;
    u16 wRcvPort = 0;
    u32 dwDstIp = 0;
    u16 wDstPort = 0;
    TLogicalChannel	tLogicalChannel;

    //�õ�����Դ��ַ
    if (!GetSwitchInfo(tSrc, dwRcvIp, wRcvPort, dwSrcIp))
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchEqpFromBrd() failure because of can't get switch info!");
        return FALSE;
    }
	u8 byDstMpId = 0;
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
    u8 byEqpType = g_cMcuAgent.GetPeriEqpType(byEqpId);

    //�㲥������ʽ
    u32  dwMpIP = 0;
    u16  wMpPort = 0;

    //�õ�Ŀ��Mt�Ľ�����Ϣ
    switch (byEqpType)
    {
    case EQP_TYPE_BAS://����������
        GetBasSwitchAddr(byEqpId, dwMpIP, wMpPort);
        break;
    case EQP_TYPE_MIXER://������
        GetMixerSwitchAddr(byEqpId, dwMpIP, wMpPort);
        break;
    case EQP_TYPE_RECORDER://��¼���
        GetRecorderSwitchAddr(byEqpId, dwMpIP, wMpPort);
        break;
    case EQP_TYPE_VMP://����ϳ���
        GetVmpSwitchAddr(byEqpId, dwMpIP, wMpPort);
        break;
    case EQP_TYPE_PRS://�����ش���
        GetPrsSwitchAddr(byEqpId, dwMpIP, wMpPort);
        break;
	default:
		break;
    }
#else //8ke/8kh/8ki����������㲥��������ʱ��ֱ���÷Ǵ����59000����
	byDstMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	dwRcvIp = g_cMcuVcApp.GetMpIpAddr(byDstMpId);
#endif

    wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;

    if (g_cMcuVcApp.GetPeriEqpLogicChnnl(byEqpId, MODE_VIDEO, &byChannelNum, &tLogicalChannel, TRUE))
    {
        CServMsg cServMsg;
        cServMsg.SetConfIdx( tSrc.GetConfIdx() );
        
        dwDstIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
        wDstPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();
        
        TSwitchChannelExt tSwitchChannel;
        u8 bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_NONE;
        u8 byUniformPayload = INVALID_PAYLOAD;
        u8 byIsSwitchRtcp = 0;
        
        
        //���콻���ŵ�
        tSwitchChannel.SetSrcMt( tSrc );
        tSwitchChannel.SetSrcIp( dwSrcIp );
        tSwitchChannel.SetRcvIP( dwRcvIp );
        tSwitchChannel.SetRcvPort( wRcvPort );
        tSwitchChannel.SetRcvBindIP( dwRcvIp );
        tSwitchChannel.SetDstIP( dwDstIp );
        tSwitchChannel.SetDstPort( wDstPort + PORTSPAN * wDstChnnl );
        
        // ׼������ǰMP����Ϣ��
        cServMsg.CatMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));                  
        cServMsg.CatMsgBody((u8 *)&bySwitchChannelMode, sizeof(u8));
        cServMsg.CatMsgBody((u8 *)&byUniformPayload, sizeof(u8));            
        cServMsg.CatMsgBody((u8 *)&byIsSwitchRtcp, sizeof(u8));
        
        
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "Send MCU_MP_ADDBRDDSTSWITCH_REQ for Eqp.%d(%s:%d)\n",
                      byEqpId,
                      StrOfIP(tSwitchChannel.GetDstIP()), tSwitchChannel.GetDstPort());
        
        BOOL32 bDstMpExist = FALSE;
       

//         for( u8 byMpId = 1; byMpId <= MAXNUM_DRI; byMpId ++ )
//         {
//             if ( g_cMcuVcApp.GetMpIpAddr(byMpId) == dwMpIP )
//             {
//                 bDstMpExist = TRUE;
//                 byDstMpId = byMpId;
//                 break;
//             }
//         }
// 
//         if ( !bDstMpExist )
//         {
//             LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[StartSwitchEqpFromBrd] Dst Mp.0x%x Unexist\n", dwMpIP );
//         }
    
		byDstMpId = g_cMcuVcApp.FindMp( dwRcvIp );
		
        bDstMpExist = g_cMcuVcApp.IsMpConnected(byDstMpId);

		if ( !bDstMpExist )
        {
            LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[StartSwitchEqpFromBrd] Dst Mp.0x%x Unexist\n", dwRcvIp );
        }
	    else
		{
			g_cMpSsnApp.SendMsgToMpSsn( byDstMpId, 
										MCU_MP_ADDBRDDSTSWITCH_REQ, 
										cServMsg.GetServMsg(), 
										cServMsg.GetServMsgLen());
		}
    }

    return TRUE;
}

/*====================================================================
    ������      ��StopSwitchToBrd
    ����        ��ֹͣ��ָ���ն˵���Ƶ��Ϊ�㲥Դ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tSrc, Դ
				  bySrcChnnl,  Դ���ŵ���
    ����ֵ˵��  ��TRUE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/03/21    4.0         ����        ����
====================================================================*/
BOOL32 CMpManager::StopSwitchToBrd( const TMt &tSrc, u8 bySrcChnnl, u16 wSpyStartPort )
{
    /*CServMsg cServMsg;
    TSwitchChannel tSwitchChannel;
    
    //�õ�����Դ��ַ
    u32 dwSrcMpIp;
    u16 wRcvPort;
    u32 dwSrcIp;
    if( !GetSwitchInfo( tSrc, dwSrcMpIp, wRcvPort, dwSrcIp ) )
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StopSwitchToBrd() failure because of can't get switch info!\n");
        return FALSE ;
	}

    tSwitchChannel.SetSrcMt(tSrc);
    tSwitchChannel.SetSrcIp(dwSrcIp);
    tSwitchChannel.SetRcvIP(dwSrcMpIp);
    tSwitchChannel.SetRcvPort(wRcvPort);

    cServMsg.SetConfIdx(tSrc.GetConfIdx());
    cServMsg.SetMsgBody((u8*)&tSwitchChannel, sizeof(TSwitchChannel));

    g_cMpSsnApp.BroadcastToAllMpSsn(MCU_MP_REMOVEBRDSRCSWITCH_REQ, 
                                    cServMsg.GetServMsg(), cServMsg.GetServMsgLen());  

    LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "Send MCU_MP_REMOVEBRDSRCSWITCH_REQ from RecvMp %s:%d to all Mp.\n",
                 StrOfIP(tSwitchChannel.GetRcvIP()), tSwitchChannel.GetRcvPort());

    return TRUE;
	*/
	CServMsg cServMsg;
    TSwitchChannel tSwitchChannel;
    
    //�õ�����Դ��ַ
    u32 dwSrcMpIp;
    u16 wRcvPort;
    u32 dwSrcIp;
    if( !GetSwitchInfo( tSrc, dwSrcMpIp, wRcvPort, dwSrcIp ) )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StopSwitchToBrd() failure because of can't get switch info!\n");
        return FALSE ;
	}

    //  [11/26/2009 pengjie] Modify ������ش�֧��
	if( wSpyStartPort != SPY_CHANNL_NULL)
	{
		wRcvPort = wSpyStartPort;
	}
	//  End

    tSwitchChannel.SetSrcMt(tSrc);
    tSwitchChannel.SetSrcIp(dwSrcIp);
    tSwitchChannel.SetRcvIP(dwSrcMpIp);
    tSwitchChannel.SetRcvPort(wRcvPort);

    cServMsg.SetConfIdx(tSrc.GetConfIdx());
    cServMsg.SetMsgBody((u8*)&tSwitchChannel, sizeof(TSwitchChannel));

    g_cMpSsnApp.BroadcastToAllMpSsn(MCU_MP_REMOVEBRDSRCSWITCH_REQ, 
                                    cServMsg.GetServMsg(), cServMsg.GetServMsgLen());  

    LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "Send MCU_MP_REMOVEBRDSRCSWITCH_REQ from RecvMp %s:%d to all Mp. bySrcChnnl<%d>\n",
                 StrOfIP(tSwitchChannel.GetRcvIP()), tSwitchChannel.GetRcvPort(),bySrcChnnl);

    return TRUE;
}

/*====================================================================
    ������      ��StartSwitchToSubMt
    ����        ����ָ���ն����ݽ������¼�ֱ���Ǵμ��ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tSrc, Դ
				  bySrcChnnl,  Դ���ŵ���
				  byDstMtId,   Ŀ���ն�
				  byMode,      ����ģʽ��ȱʡΪMODE_BOTH
				  bySwitchMode ������ʽ
                  bH239Channel
                  bStopBeforeStart ���½���ǰ�Ƿ�ͣ����
    ����ֵ˵��  ��TRUE�����������ֱ�ӷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/12    1.0         LI Yi         ����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
// BOOL32 CMpManager::StartSwitchToSubMt( const TMt & tSrc, 
//                                      u8        bySrcChnnl, 
//                                      const TMt & tDst, 
//                                      u8        byMode,
//                                      u8        bySwitchMode, 
//                                      BOOL32 bH239Chnnl,
// 									 BOOL32 bStopBeforeStart,
// 									 BOOL32 bSrcHDBas,
// 									 u16 wSpyPort)
// {
// 	u32  dwSrcIp = 0;
// 	u32  dwDisIp = 0;
// 	u32  dwRcvIp = 0;
// 	u16  wRcvPort = 0;
// 	TLogicalChannel	tLogicalChannel;
// 	BOOL32 bResult1 = TRUE;
// 	BOOL32 bResult2 = TRUE;
// 
// 	//�õ�����Դ��ַ
// 	if( !GetSwitchInfo( tSrc, dwRcvIp, wRcvPort, dwSrcIp ) )
// 	{
// 		 LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchToSubMt() failure because of can't get switch info!\n");
// 		 return FALSE ;
// 	}
//     
//     TConfInfo * ptConfInfo = &g_cMcuVcApp.GetConfInstHandle(tSrc.GetConfIdx())->m_tConf;
//     u8 byIsSwitchRtcp;
//     if (( TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_VMP == tSrc.GetEqpType() && MT_TYPE_MT != tDst.GetMtType() &&
//         ptConfInfo->GetConfAttrb().IsResendLosePack()) ||
//         tSrc == tDst) //  xsl [1/17/2006] (�⳧��mt��mcu)����Ƶ�����������Լ�ʱ������rtcp����
//     {
//         byIsSwitchRtcp = 0;
//     }
//     else
//     {
//         byIsSwitchRtcp = 1;
//     }
// 
// 	//ֻ�����ݲ���
// 	TMt tDstMt = tDst;
// 	if( tDstMt.IsNull() )
// 	{
//         // ���� [4/30/2006] ֻ�ղ����Ĵ���������,���߼�ͨ���򿪺��תDump 
//         LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "StartSwitchToSubMt() tDstMt is NULL! It's send-only MT?\n");        
// 		return TRUE;
// 	}
// 
// 	//�õ������ն˱�ָ��
// 	TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable( tSrc.GetConfIdx() );
// 	if( ptConfMtTable == NULL )
// 	{
// 		LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchToSubMt() failure because invalid ConfMtTable Pointer!\n");
// 		return FALSE;
// 	}
// 
//     u8 bySwitchChannelMode;
//     u8 byUniformPayload = INVALID_PAYLOAD;
//     u8 byManuID = ptConfMtTable->GetManuId(tDst.GetMtId());
// 
//     if (byManuID == MT_MANU_KDC || byManuID == MT_MANU_KDCMCU)
// 	{
// 		bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_NONE;
// 	}
//     else
//     {
//         bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_VALID;
//         //Active Payload Uniform
//         if (TYPE_MT == tSrc.GetType() && TYPE_MT == tDst.GetType())
//         {
//             u8 byChannelType;
//             TLogicalChannel  tChannel;
//             memset(&tChannel, 0, sizeof(TLogicalChannel));
//             //Get channal info
//             if (0 == bySrcChnnl)
//             {
//                 byChannelType = LOGCHL_VIDEO;
//             }
//             else if (1 == bySrcChnnl)
//             {
//                 byChannelType = LOGCHL_SECVIDEO;
//             }
// 
//             if (ptConfMtTable->GetMtLogicChnnl(tDst.GetMtId(), byChannelType, &tChannel, TRUE))
//             {
//                 if (tChannel.GetChannelType() >= AP_MIN && tChannel.GetChannelType() <= AP_MAX)
//                 {
//                     byUniformPayload = tChannel.GetChannelType();
//                 }
//             }
//         }
//     }
// 
// 	//�㲥������ʽ
// 	//if(bySwitchMode==SWITCH_MODE_BROADCAST)
// 	// [pengjie 2010/9/26] ����ѡ�����䲻����
// 	if( ( TYPE_MT == tSrc.GetType() || (TYPE_MCUPERI == tSrc.GetType() && tSrc.GetEqpType() == EQP_TYPE_BAS)) &&
// 		SWITCH_MODE_SELECT == bySwitchMode )
// 	{
// 		//zjj20091023ѡ���ն˲����ţ�����ֱ�Ӵ�Դ������ַ����������Ŀ�ĵ�ַ
// 		//ѡ�����裬����vmp��Ȼѡ����������Ϊ�㲥Դ��Ҫ���ţ����Ի���Ҫ����
// 		//ȱ��:������������Ƿ��в���Ϊ�㲥Դ����Ҫ��ѡ���Ļ�����Ͳ������ˣ���Ҫ��if�м��������͵��ж�
// 		LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[StartSwitchToSubMt] tSrc's type is mt and bySwitchMode is SWITCH_MODE_SELECT so not use brige  \n");
// 	}
// 	else
// 	{
// 		//�õ�Ŀ��Mt�Ľ�����Ϣ
// 		u32  dwMpIP = 0;
// 		dwMpIP = g_cMcuVcApp.GetMpIpAddr( ptConfMtTable->GetMpId( tDst.GetMtId() ) );
// 		if( dwMpIP != dwRcvIp )//��Ҫת��
// 		{
// 			dwDisIp = dwRcvIp;
// 			dwRcvIp = dwMpIP;
// 		}
// 	}
// 
// 	//�����ŵ��ż���ƫ����
// 	wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;    //˫��Ҫ����
// 
// 	//ͼ��
// 	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
// 	{
// 		//��������
// 		if( ptConfMtTable->GetMtLogicChnnl( tDst.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, TRUE ) )	//���Խ���
// 		{
// 			//  [11/9/2009 pengjie] ������ش�֧��
// 			if( wSpyPort != SPY_CHANNL_NULL )
// 			{
// 				wRcvPort = wSpyPort;
// 			}
//             bResult1 = StartSwitch( tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort, tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp, bStopBeforeStart);
// 			if(!bResult1)LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Switch video failure!\n");
// 		}
// 	}
// 
//     if (MODE_SECVIDEO == byMode)
//     {
//         //H.239��Ƶ�߼�ͨ��
//         if (bH239Chnnl)
//         {
//             if (ptConfMtTable->GetMtLogicChnnl(tDst.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, TRUE))
//             {
// 				//zbq[09/03/2008] HD Bas��Դ��˫�� �Ӷ˿ڻ�ַ+PORTSPAN ������
// 				if (bSrcHDBas)
// 				{
// 					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort/* + PORTSPAN*/,
// 										   tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp);					
// 				}
// 				else
// 				{
// 					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort+4,
// 										   tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp);					
// 				}
//                 if (!bResult1)
//                 {
//                     LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Switch video h.239 failure!\n");
//                 }
//             }
//             return bResult1;
//         }
//     }
// 
// 	//����
// 	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
// 	{
// 		//��������
// 		if( ptConfMtTable->GetMtLogicChnnl(tDst.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, TRUE ) )	//���Խ���
// 		{
// 			//  [11/9/2009 pengjie] ������ش�֧��
// 			if( wSpyPort != SPY_CHANNL_NULL )
// 			{
// 				wRcvPort = wSpyPort;
// 			}
// 			bResult2 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, (wRcvPort+2),
//                                    tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp);
// 			if(!bResult2)LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Switch audio failure!\n");
// 		}
// 	}	
// 
// 	return  bResult1 && bResult2 ;
// }

/*====================================================================
    ������      ��StopSwitchToSubMt
    ����        ��ֹͣ�����ݽ�����ֱ���¼��Ǵμ��ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����byDstMtId, Ŀ���ն˺�
				  byMode, ����ģʽ��ȱʡΪMODE_BOTH
				  bySwitchMode ������ʽ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/11    1.0         LI Yi         ����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
// void CMpManager::StopSwitchToSubMt( const TMt & tDst, u8 byMode, BOOL32 bH239Chnnl, u16 wSpyStartPort )
// {
// 	u8   byDstMtId = tDst.GetMtId();
// 	TLogicalChannel	tLogicalChannel;
// 
// 	TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable( tDst.GetConfIdx() );
// 	if( ptConfMtTable == NULL )
// 	{
// 		LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StopSwitchToSubMt() failure because invalid ConfMtTable Pointer!\n");
// 		return ;
// 	}
// 
//     TConfInfo * ptConfInfo = &g_cMcuVcApp.GetConfInstHandle(tDst.GetConfIdx())->m_tConf;
//     u8 byIsSwitchRtcp;
//     if (MT_TYPE_MT != tDst.GetMtType() && ptConfInfo->GetConfAttrb().IsResendLosePack())
//     {
//         byIsSwitchRtcp = 0;
//     }
//     else
//     {
//         byIsSwitchRtcp = 1;
//     }
// 
//     //ͼ��
// 	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
// 	{
// 		if( ptConfMtTable->GetMtLogicChnnl( byDstMtId, LOGCHL_VIDEO, &tLogicalChannel, TRUE ) )	//�ѵǼ�
// 		{
// 			//zjj20100201
// 			//  [12/7/2009 pengjie] ������ش�֧��
// 			if( wSpyStartPort != SPY_CHANNL_NULL )
// 			{
// 				tLogicalChannel.m_tRcvMediaChannel.SetPort( wSpyStartPort );
// 			}
// 			// End
// 			
// 			StopSwitch(tDst.GetConfIdx(), tLogicalChannel, 0, byIsSwitchRtcp);
// 		}
// 		
// 	}
// 
//     if (MODE_SECVIDEO == byMode)
// 	{
// 		//H.239��Ƶ�߼�ͨ��
// 		if (bH239Chnnl)
// 		{
// 			if (ptConfMtTable->GetMtLogicChnnl(tDst.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, TRUE))
// 			{
// 				//zjj20100201
// 				//  [12/7/2009 pengjie] ������ش�֧��
// 				if( wSpyStartPort != SPY_CHANNL_NULL )
// 				{
// 					tLogicalChannel.m_tRcvMediaChannel.SetPort( wSpyStartPort + 4 );
// 				}
// 				// End
// 				
// 				StopSwitch(tDst.GetConfIdx(), tLogicalChannel, 0, byIsSwitchRtcp);
// 			}
// 			return;
// 		}
// 	}
// 
//     //����
// 	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
// 	{
// 		if( ptConfMtTable->GetMtLogicChnnl( byDstMtId, LOGCHL_AUDIO, &tLogicalChannel, TRUE ) )	//�ѵǼ�
// 		{
// 			//zjj20100201
// 			//  [12/7/2009 pengjie] ������ش�֧��
// 			if( wSpyStartPort != SPY_CHANNL_NULL )
// 			{
// 				tLogicalChannel.m_tRcvMediaChannel.SetPort( wSpyStartPort + 2 );
// 			}
// 			// End
// 			StopSwitch(tDst.GetConfIdx(), tLogicalChannel, 0, byIsSwitchRtcp);
// 		}
// 	}
// }

/*====================================================================
    ������      ��StopSwitchToSubMt
    ����        ��ֹͣ�����ݽ�����ֱ���¼��Ǵμ��ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx ����Id
	              u8 byMtNum �ն���
				  const TMt *ptDst �ն���Ϣ
				  byMode, ����ģʽ��ȱʡΪMODE_BOTH
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/11    1.0         LI Yi         ����
	03/07/15    1.0         ������        ��ҵ������
	20110505    4.6         pengjie       ����������֧��
====================================================================*/
void CMpManager::StopSwitchToSubMt( u8 byConfIdx, u8 byMtNum, const TMt *ptDst, u8 byMode, u16 wSpyPort )
{
	if( byMtNum == 0 || ptDst == NULL )
	{
		return;
	}

	TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable( byConfIdx );
	if( ptConfMtTable == NULL )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "StopSwitchToSubMt failure because invalid ConfMtTable Pointer!\n");
		return ;
	}

	CMcuVcInst* pcInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
	TConfInfo *ptConfInfo = pcInst != NULL ? &pcInst->m_tConf : NULL;

	if (NULL == ptConfInfo)
	{
		return;
	}

	if( ptConfMtTable == NULL )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "StopSwitchToSubMt() failure because invalid TConfInfo Pointer!\n");
		return ;
	}

	if( NULL == pcInst )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "StopSwitchToSubMt() failure because invalid pcInst Pointer!\n");
		return;
	}

	u8 byIsSwitchRtcp = 0;
	CServMsg cServMsg;
	cServMsg.SetConfIdx(byConfIdx);
	//u32 dwMpIP = 0;
	TSwitchChannel tSwitchChannel;
	for (u8 byMpLoop = 1; byMpLoop <= MAXNUM_DRI; byMpLoop ++)
    {
		if ( !g_cMcuVcApp.IsMpConnected(byMpLoop) )
        {
            continue;
        }
		
        cServMsg.SetDstDriId( byMpLoop );
        cServMsg.SetMsgBody();
        
        //dwMpIP = g_cMcuVcApp.GetMpIpAddr( byMpLoop );

		u8 byMpId = 0;
		u8   byDstMtId = 0;
		for( u8 byLoop = 0; byLoop < byMtNum; byLoop++ )
		{
			// ֪ͨ���յ�ַ [pengguofeng 2/21/2013]
			byDstMtId = ptDst[byLoop].GetMtId();
			if( pcInst->IsMultiCastMt(byDstMtId) )
			{
				if (  byMode == MODE_VIDEO)
				{
					//pcInst->NotifyMtReceive(ptDst[byLoop], byDstMtId); //֪ͨ���Լ�					
					pcInst->ChangeSatDConfMtRcvAddr(byDstMtId, MODE_VIDEO, FALSE);		
				}
				
				//���ڷ��͵�˫��Դ
				if ( byMode == MODE_SECVIDEO )
				{					
					pcInst->ChangeSatDConfMtRcvAddr(byDstMtId, MODE_SECVIDEO, FALSE);					
				}
				
				//��Ƶ�㲥Դ
				if ( byMode == MODE_AUDIO )
				{					
					pcInst->ChangeSatDConfMtRcvAddr(byDstMtId, MODE_AUDIO, FALSE);				
				}
// 				continue;//�����ն˲��������𽻻�
			}
			
			// ÿ�δ���ǰMP������ն�
			byMpId = ptConfMtTable->GetMpId( ptDst[byLoop].GetMtId() );
			if ( byMpId != byMpLoop )
			{
				continue;
			}
			
			TLogicalChannel	tLogicalChannel;
			TConfAttrb tConfAttrb = ptConfInfo->GetConfAttrb();
			if (MT_TYPE_MT != ptDst[byLoop].GetMtType() && tConfAttrb.IsResendLosePack())
			{
				byIsSwitchRtcp = 0;
			}
			else
			{
				byIsSwitchRtcp = 1;
			}
			
			if( byMode == MODE_VIDEO )
			{
				if( !ptConfMtTable->GetMtLogicChnnl( byDstMtId, LOGCHL_VIDEO, &tLogicalChannel, TRUE ) )
				{				
					//StopSwitch(byConfIdx, tLogicalChannel, 0, byIsSwitchRtcp);
					continue;
				}
				
			}
			else if(byMode == MODE_SECVIDEO || byMode == MODE_VIDEO2SECOND)
			{
				if( !ptConfMtTable->GetMtLogicChnnl( byDstMtId, LOGCHL_SECVIDEO, &tLogicalChannel, TRUE ) )
				{				
					continue;
				}
			}
			else if( byMode == MODE_AUDIO )
			{
				if( !ptConfMtTable->GetMtLogicChnnl( byDstMtId, LOGCHL_AUDIO, &tLogicalChannel, TRUE ) )
				{				
					continue;
				}
			}
			else
			{
				LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[StopSwitchToSubMt] Failed Mt.%d, mode.%d !\n", byDstMtId, byMode );
				continue;
			}
			
			tSwitchChannel.SetDstIP(tLogicalChannel.m_tRcvMediaChannel.GetIpAddr());
			if (wSpyPort != SPY_CHANNL_NULL)
			{
				if (MODE_VIDEO == byMode)
				{
					//wSpyPort����
				}
				else if(MODE_AUDIO == byMode)
				{
					wSpyPort += 2;
				}	
				else if (MODE_VIDEO2SECOND == byMode || MODE_SECVIDEO == byMode)
				{
					wSpyPort += 4;
				}
				else
					
				{
					continue;
				}
				tSwitchChannel.SetDstPort(wSpyPort);
			}
			else
			{
				tSwitchChannel.SetDstPort(tLogicalChannel.m_tRcvMediaChannel.GetPort());
			}	
		
			cServMsg.CatMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));
			cServMsg.CatMsgBody((u8 *)&byIsSwitchRtcp, sizeof(u8));
			
			LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "remove Mt.%d(%s:%d)  \n",
													 	ptDst[byLoop].GetMtId(),
														StrOfIP(tSwitchChannel.GetDstIP()), tSwitchChannel.GetDstPort());

			LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[StopSwitchToSubMt] Add Mt.%d,MpId.%d in remove Msg !\n", ptDst[byLoop].GetMtId(), byMpLoop );
		}

		if ( cServMsg.GetMsgBodyLen() > 0 )
		{
			g_cMpSsnApp.BroadcastToAllMpSsn(MCU_MP_REMOVESWITCH_REQ, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());  
		}
	}

	return;
}

/*====================================================================
    ������      ��StopRecvSubMt
    ����        ��ֹͣ�����ն�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt & tMt ֹͣ����
				  u8 byMode
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/11    1.0         LI Yi         ����
	03/07/15    1.0         ������        ��ҵ������
====================================================================
void CMpManager::StopRecvSubMt( const TMt & tMt, u8 byMode )
{
	u32 dwDstIp = 0x7f000001;
	u16 wDstPort;
	if( byMode == MODE_VIDEO || byMode == MODE_AUDIO )
	{			
		wDstPort = RECV_MT_START_PORT - 512 + 4*tMt.GetMtId() + 2*(byMode -1);//???Temp
		StopSwitch( tMt.GetConfIdx(), dwDstIp, wDstPort );
	}
	else
	{
		wDstPort = RECV_MT_START_PORT - 512 + 4*tMt.GetMtId() + 2*(MODE_VIDEO -1);
		StopSwitch( tMt.GetConfIdx(), dwDstIp, wDstPort );
		wDstPort = RECV_MT_START_PORT - 512 + 4*tMt.GetMtId() + 2*(MODE_AUDIO -1);
		StopSwitch( tMt.GetConfIdx(), dwDstIp, wDstPort );
	}
}
*/

/*====================================================================
    ������      : GetRecorderSwitchAddr
    ����        ���õ�¼��������ݽ���IP��ַ��˿�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����byRecorderId �����
	              dwSwitchIpAddr �������ݵ�IP��ַ
                  wSwitchPort  �������ݵĶ˿�
    ����ֵ˵��  ��TRUE����������ڷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/15    1.0         ������        ����
====================================================================*/
BOOL32 CMpManager::GetRecorderSwitchAddr(u8 byRecorderId, u32 &dwSwitchIpAddr,u16 &wSwitchPort)
{
	TEqpRecInfo tRecInfo;
	if(SUCCESS_AGENT == g_cMcuAgent.GetEqpRecorderCfg( byRecorderId, &tRecInfo))
	{
        wSwitchPort = tRecInfo.GetMcuRecvPort();
//        dwSwitchIpAddr = ntohl( g_cMcuAgent.GetBoardIpAddrFromIdx(tRecInfo.GetSwitchBrdId()) );
// [pengjie 2010/3/30] ���������8000e���Ͳ�����GetSwitchBrdId()�Լ�GetBoardIpAddrFromIdx(),���õ�SwitchIp��
//                     ��Ϊ8000e��һ��mcu��IP.
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
		u16 wMcuStartPort = 0;
		u16 wEqpStartPort = 0;
		GetSwitchInfo( byRecorderId, dwSwitchIpAddr, wSwitchPort, wEqpStartPort );
		//wSwitchPort = wMcuStartPort;
#else
        dwSwitchIpAddr = ntohl( g_cMcuAgent.GetBoardIpAddrFromIdx(tRecInfo.GetSwitchBrdId()) );
#endif

		if( dwSwitchIpAddr > 0 )
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*====================================================================
    ������      : GetMixerSwitchAddr
    ����        ���õ������������ݽ���IP��ַ��˿�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����byMixerId �����
	              dwSwitchIpAddr �������ݵ�IP��ַ
                  wSwitchPort  �������ݵĶ˿�
    ����ֵ˵��  ��TRUE����������ڷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/15    1.0         ������        ����
====================================================================*/
BOOL32 CMpManager::GetMixerSwitchAddr(u8 byMixerId, u32 &dwSwitchIpAddr,u16 &wSwitchPort)
{
	TEqpMixerInfo tMixerInfo;
	if(SUCCESS_AGENT == g_cMcuAgent.GetEqpMixerCfg( byMixerId, &tMixerInfo))
	{
        wSwitchPort = tMixerInfo.GetMcuRecvPort();
        dwSwitchIpAddr = ntohl( g_cMcuAgent.GetBoardIpAddrFromIdx(tMixerInfo.GetSwitchBrdId()) );
		if( dwSwitchIpAddr > 0 )
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*====================================================================
    ������      : GetBasSwitchAddr
    ����        ���õ����������������ݽ���IP��ַ��˿�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����byBasId �����
	              dwSwitchIpAddr �������ݵ�IP��ַ
                  wSwitchPort  �������ݵĶ˿�
    ����ֵ˵��  ��TRUE����������ڷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/15    1.0         ������        ����
====================================================================*/
BOOL32 CMpManager::GetBasSwitchAddr(u8 byBasId, u32 &dwSwitchIpAddr,u16 &wSwitchPort)
{
    TEqpBasInfo tBasInfo;
	if(SUCCESS_AGENT == g_cMcuAgent.GetEqpBasCfg( byBasId, &tBasInfo))
	{
        wSwitchPort = tBasInfo.GetMcuRecvPort();
		dwSwitchIpAddr = ntohl( g_cMcuAgent.GetBoardIpAddrFromIdx(tBasInfo.GetSwitchBrdId()) );
        
		if( dwSwitchIpAddr > 0 )
		{
			return TRUE;
		}
	}

    TEqpBasHDInfo tBasHdInfo;
	if(SUCCESS_AGENT == g_cMcuAgent.GetEqpBasHDCfg( byBasId, &tBasHdInfo))
	{
        wSwitchPort = tBasHdInfo.GetMcuRecvPort();
		//dwSwitchIpAddr = tBasHdInfo.GetIpAddr();
        dwSwitchIpAddr = ntohl( g_cMcuAgent.GetBoardIpAddrFromIdx(tBasHdInfo.GetSwitchBrdId()) );
        
		if( dwSwitchIpAddr > 0 )
		{
			return TRUE;
		}
	}    

	return FALSE;
}

/*====================================================================
    ������      : GetVmpSwitchAddr
    ����        ���õ�����ϳ��������ݽ���IP��ַ��˿�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����byVmpId �����
	              dwSwitchIpAddr �������ݵ�IP��ַ
                  wSwitchPort  �������ݵĶ˿�
    ����ֵ˵��  ��TRUE����������ڷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/15    1.0         ������        ����
====================================================================*/
BOOL32 CMpManager::GetVmpSwitchAddr(u8 byVmpId, u32 &dwSwitchIpAddr,u16 &wSwitchPort)
{
    TEqpVMPInfo tVmpInfo;

    if(SUCCESS_AGENT == g_cMcuAgent.GetEqpVMPCfg( byVmpId, &tVmpInfo))
    {
        wSwitchPort = tVmpInfo.GetMcuRecvPort();
        dwSwitchIpAddr = ntohl( g_cMcuAgent.GetBoardIpAddrFromIdx(tVmpInfo.GetSwitchBrdId()) );
        if( dwSwitchIpAddr > 0 )
        {
            return TRUE;
        }
    }
	return FALSE;
}

/*====================================================================
    ������      : GetVmpTwSwitchAddr
    ����        ���õ����ϵ���ǽ�����ݽ���IP��ַ��˿�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����byVmpTwId �����
	              dwSwitchIpAddr �������ݵ�IP��ַ
                  wSwitchPort  �������ݵĶ˿�
    ����ֵ˵��  ��TRUE����������ڷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/15    1.0         ������        ����
====================================================================*/
//BOOL32 CMpManager::GetVmpTwSwitchAddr(u8 byVmpTwId, u32 &dwSwitchIpAddr, u16 &wSwitchPort)
//{
//    TEqpMpwInfo tVmpTwInfo;
//
//    if(g_cMcuAgent.GetEqpMpwCfg(byVmpTwId, &tVmpTwInfo))
//    {
//        wSwitchPort = tVmpTwInfo.wMcueqpMpwEntPort;
//        dwSwitchIpAddr = ntohl(g_cMcuAgent.GetBoardIpAddrFromIdx(tVmpTwInfo.byMcueqpVMPEntSwitchBrdId));
//        if (dwSwitchIpAddr > 0)
//        {
//            return TRUE;
//        }
//    }
//
//    return FALSE;
//}

/*====================================================================
    ������      : GetPrsSwitchAddr
    ����        ���õ������ش��������ݽ���IP��ַ��˿�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����byPrsId �����
	              dwSwitchIpAddr �������ݵ�IP��ַ
                  wSwitchPort  �������ݵĶ˿�
    ����ֵ˵��  ��TRUE����������ڷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/15    1.0         ������        ����
====================================================================*/
BOOL32 CMpManager::GetPrsSwitchAddr( u8 byPrsId, u32 &dwSwitchIpAddr,u16 &wSwitchPort )
{
    TEqpPrsInfo tPrsInfo;
	if(SUCCESS_AGENT == g_cMcuAgent.GetEqpPrsCfg( byPrsId,  tPrsInfo))
	{
        wSwitchPort = tPrsInfo.GetMcuRecvPort();
        dwSwitchIpAddr = ntohl( g_cMcuAgent.GetBoardIpAddrFromIdx(tPrsInfo.GetSwitchBrdId()) );
		if( dwSwitchIpAddr > 0 )
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*====================================================================
    ������      ��StartSwitchToPeriEqp
    ����        ����ָ���ն����ݽ�����ֱ������
    �㷨ʵ��    ��Ŀǰ�㲥Դ������Ľ��������õ�ת����Ϊ�㲥Դ���ڵ�ת���壬
				  ����Ϊ�����������mcu����������ת���壬��Ϊ������ת�����
				  ��ʱmcu���Զ����·���
    ����ȫ�ֱ�����
    �������˵����tSrc, Դ, Ϊ���ػ�����ն�
				  bySrcChnnl, Դ���ŵ���
				  byEqpId, �����
				  byDstChnnl, Ŀ���ŵ������ţ�ȱʡΪ0
   				  byMode, ����ģʽ��ȱʡΪMODE_BOTH
				  bySwitchMode ������ʽ
   ����ֵ˵��  ��TRUE�����������ֱ�ӷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/20    1.0         LI Yi         ����
	03/07/15    1.0         ������        ��ҵ������
	10/02/25	4.6			Ѧ��		  ����8000E
====================================================================*/
BOOL32 CMpManager::StartSwitchToPeriEqp(const TMt & tSrc,
										u16	wSrcChnnl,
										u8	byEqpId,
										u16 wDstChnnl,
										u8	byMode,
										u8	bySwitchMode,
										BOOL32 bDstHDBas,
										BOOL32 bStopBeforeStart,
										u8 byHduSubChnId)
{
    u8  byChannelNum = 0;
    u32 dwDisIp = 0;
    u32 dwSrcIp = 0;
    u32 dwRcvIp = 0;
    u16 wRcvPort = 0;
    u32 dwDstIp = 0;
    u16 wDstPort = 0;
    TLogicalChannel	tLogicalChannel;
    BOOL32 bResult1 = TRUE;
    BOOL32 bResult2 = TRUE;

    //�õ�����Դ��ַ
    if (!GetSwitchInfo(tSrc, dwRcvIp, wRcvPort, dwSrcIp))
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[StartSwitchToPeriEqp]failure because of can't get switch info for mt()!\n");
        return FALSE;
    }

	//zjj20100306 ��ش��޸�
    //wRcvPort = wRcvPort + PORTSPAN * wSrcChnnl;

	if( wSrcChnnl >= CASCADE_SPY_STARTPORT && wSrcChnnl < MT_MCU_STARTPORT ) //�ö�ش���ͨ��
	{
		wRcvPort = wSrcChnnl;
	}
	else
	{
		wRcvPort = wRcvPort + PORTSPAN * wSrcChnnl;
	}

	// [2/25/2010 xliang] ȡ��������.
	//ע��8000A��8000Eʵ�ֲ�ͬ����mcuagtlib��mcuagt8000elib�����֣��˴��ӿ�ͳһ
    u8 byEqpType = g_cMcuAgent.GetPeriEqpType(byEqpId);
    if (0 == byEqpType)
    {
        if (byEqpId >= VMPID_MIN && byEqpId <= VMPID_MAX)
        {
            byEqpType = EQP_TYPE_VMP;
        }
    }


    //�㲥������ʽ
    u32  dwMpIP = 0;
    u16  wMpPort = 0;

    //�õ�Ŀ��Mt�Ľ�����Ϣ
	// [2/25/2010 xliang] ����ӿڵĵ�����Ҫ��Ϊ�˵õ������Ӧ��ת����ip����ȷ��disIp�ĸ�ֵ��
	// ����8000E���˲�û�б�Ҫ��������ӿڶ�8000EҲ�����á�
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
    switch (byEqpType)
    {
    case EQP_TYPE_MIXER://������
        GetMixerSwitchAddr(byEqpId, dwMpIP, wMpPort);
        break;
    case EQP_TYPE_RECORDER://��¼���
        GetRecorderSwitchAddr(byEqpId, dwMpIP, wMpPort);
        break;
    case EQP_TYPE_BAS://����������
        GetBasSwitchAddr(byEqpId, dwMpIP, wMpPort);
        break;
    case EQP_TYPE_VMP://����ϳ���
        GetVmpSwitchAddr(byEqpId, dwMpIP, wMpPort);
        break;
    case EQP_TYPE_PRS://�����ش���
        GetPrsSwitchAddr(byEqpId, dwMpIP, wMpPort);
        break;
	default:
		break;
    }

    // ��Ҫת��
	// [pengjie 2010/9/27] �ն�¼�����ţ�ֱ�Ӵ�Դ��¼���
//     if (SWITCH_MODE_BROADCAST == bySwitchMode /*|| EQP_TYPE_RECORDER == byEqpType*/)
//     {
//         if (dwMpIP != dwRcvIp)
//         {
//             dwDisIp = dwRcvIp;
//             dwRcvIp = dwMpIP;
//         }
//     }
#endif


    // guzh [5/12/2007] �����8000B ��������MPC�ϣ����������ڲ�IP�� ������ڲ�ͨ��IP
#ifdef _MINIMCU_
    u32 dwEqpIp = 0;
    BOOL32 bModified2InnerIp = FALSE;
    g_cMcuAgent.GetPeriInfo(byEqpId, &dwEqpIp, &byEqpType);

	TMcuStatus tMcuStatus;
	g_cMcuVcApp.GetMcuCurStatus( tMcuStatus );
    // FIXME: ����û�п���MPC��DSC����MP�����, zgc, 2007-09-03
	

    if ( dwEqpIp == g_cMcuAgent.GetMpcIp() && tMcuStatus.IsExistDSC() )
    {
        TDSCModuleInfo tDscInfo;
        g_cMcuAgent.GetDscInfo(&tDscInfo);

		if ( htonl(tDscInfo.GetMcuInnerIp()) != g_cMcuAgent.GetMpcIp())
		{
			dwEqpIp = htonl(tDscInfo.GetMcuInnerIp());
			bModified2InnerIp = TRUE;
		}
    }

    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[StartSwitchToPeriEqp] bModify2Inner.%d, mcueqp recv ip: 0x%x\n", bModified2InnerIp, dwEqpIp);
#endif


    //ͼ��
    if (MODE_VIDEO == byMode || MODE_BOTH == byMode)
    {
//         //��baphd����ͬ��
//         if (EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId))
//         {
// 			u8 byOutNum = g_cMcuVcApp.GetBasOutPutChnNum(g_cMcuVcApp.GetEqp(byEqpId));
//             wDstChnnl = wDstChnnl * byOutNum;
//         }
        // zbq [08/04/2007] BAS����ͬ�㲥Ŀ�꽻������Ϊͬһ�߼�
        if ( EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId) && bySwitchMode == SWITCH_MODE_BROADCAST)
        {
            StartSwitchEqpFromBrd( tSrc, wSrcChnnl, byEqpId, wDstChnnl);
        }
        else
        {
            if (g_cMcuVcApp.GetPeriEqpLogicChnnl(byEqpId, MODE_VIDEO, &byChannelNum, &tLogicalChannel, TRUE))
            {
                dwDstIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
                wDstPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();
            #ifdef _MINIMCU_
                if (bModified2InnerIp)
                {
                    dwDstIp = dwEqpIp;
                }
            #endif
                bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort,
                                       dwDstIp, (wDstPort + PORTSPAN * wDstChnnl), 0, 0, 
									   SWITCHCHANNEL_UNIFORMMODE_NONE, INVALID_PAYLOAD, 1, bStopBeforeStart
									   );// xliang [4/13/2009] modify for 4 last param
            }
        }
    }

    //����
    if (MODE_AUDIO == byMode || MODE_BOTH == byMode)
    {
//         //��baphd����ͬ��
//         if (EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId))
//         {
//             u8 byOutNum = g_cMcuVcApp.GetBasOutPutChnNum(g_cMcuVcApp.GetEqp(byEqpId));
//             wDstChnnl = wDstChnnl * byOutNum;
//         }

        if (g_cMcuVcApp.GetPeriEqpLogicChnnl(byEqpId, MODE_AUDIO, &byChannelNum, &tLogicalChannel, TRUE))
        {
            dwDstIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
            wDstPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();

#ifdef _MINIMCU_
            if (bModified2InnerIp)
            {
                dwDstIp = dwEqpIp;
            }
#endif

            bResult2 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, (wRcvPort + 2),
                                   dwDstIp, (wDstPort + PORTSPAN * wDstChnnl), 0, 0);
        }	
    }

    //˫��ͼ��
    if (MODE_SECVIDEO == byMode)
    {
        if (g_cMcuVcApp.GetPeriEqpLogicChnnl(byEqpId, MODE_VIDEO, &byChannelNum, &tLogicalChannel, TRUE))
        {
            dwDstIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
            wDstPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();

#ifdef _MINIMCU_
            if (bModified2InnerIp)
            {
                dwDstIp = dwEqpIp;
            }
#endif
			TEqp tTempEqp = g_cMcuVcApp.GetEqp( byEqpId );
            if (EQP_TYPE_PRS ==  g_cMcuVcApp.GetEqpType(byEqpId) )
            {
				//[nizhijun 2010/12/15] ��Ҫ�ж�maintpye�Ƿ�������
				//[liu lijiu][2010/11/03]strc��basʱ���˿ڲ���4
				if (TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_BAS == tSrc.GetEqpType())
				{
					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort,
                                       dwDstIp, (wDstPort + PORTSPAN * wDstChnnl), 0, 0);
				}
				else
				{
					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, (wRcvPort + 4),
                                       dwDstIp, (wDstPort + PORTSPAN * wDstChnnl), 0, 0);
				}
            }
			else if (EQP_TYPE_VMP ==  g_cMcuVcApp.GetEqpType(byEqpId) )
            {
				bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, (wRcvPort + 4),
                                       dwDstIp, (wDstPort + PORTSPAN * wDstChnnl), 0, 0);
			}
			else if ( IsValidHduEqp(tTempEqp) )
			{
				if ( IsValidHduChn(byEqpId, (u8)wDstChnnl, byHduSubChnId) )
				{
					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, (wRcvPort + 4),
                                       dwDstIp, (wDstPort + PORTSPAN * wDstChnnl), 0, 0);
				}
			}
            else
            {
				//����Դ��BAS��Ŀ����¼����Ļ���˫��ģʽ�£�ԴBAS�Ķ˿ڲ�ƫ��+4
				if ( TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_BAS == tSrc.GetEqpType() &&
				 	 EQP_TYPE_RECORDER ==  g_cMcuVcApp.GetEqpType(byEqpId)
					)
				{
					
				}
				else
				{
					wRcvPort+=4;
				}

				//zbq[09/03/2008] ����HD Bas��˫�������� �˿ڻ�ַ+PORTSPAN ��ʼ������
				if (!bDstHDBas)
				{
					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort,
										   dwDstIp, (wDstPort + PORTSPAN * wDstChnnl + 4), 0, 0);					
				}
				else
				{
// 					// [2/25/2010 xliang] FIXME: 8000E-VMPBAS ����
// 					u8 byOutNum = g_cMcuVcApp.GetBasOutPutChnNum(g_cMcuVcApp.GetEqp(byEqpId));
// 					wDstChnnl = wDstChnnl * byOutNum;
					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort,
										   dwDstIp, (wDstPort + PORTSPAN * wDstChnnl), 0, 0);
				}
            }            
        }
    }

    return bResult1 && bResult2;
}

/*====================================================================
    ������      ��StopSwitchToPeriEqp
    ����        ��ֹͣ�����ݽ���������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����byEqpId,  �����
				  byDstChnnl, �ŵ������ţ�ȱʡΪ0	
				  byMode, ����ģʽ��ȱʡΪMODE_BOTH
				  bySwitchMode ������ʽ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/11    1.0         LI Yi         ����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
void CMpManager::StopSwitchToPeriEqp( u8 byConfIdx, u8 byEqpId, u16 wDstChnnl,  u8 byMode, u8 byHduSubChnId )
{
	u8 byChannelNum = 0;
    TLogicalChannel tLogicalChannel;

    // guzh [5/12/2007] �����8000B ��������MPC�ϣ����������ڲ�IP�� ������ڲ�ͨ��IP
#ifdef _MINIMCU_
    u32 dwEqpIp = 0;
    u8 byEqpType = 0;
    BOOL32 bModified2InnerIp = FALSE;
    g_cMcuAgent.GetPeriInfo(byEqpId, &dwEqpIp, &byEqpType); // ����IP Ϊ������

	TMcuStatus tMcuStatus;
	g_cMcuVcApp.GetMcuCurStatus( tMcuStatus );
    // FIXME: ����û�п���MPC��DSC����MP�����, zgc, 2007-09-03
    if (dwEqpIp == g_cMcuAgent.GetMpcIp() && tMcuStatus.IsExistDSC() )
    {
        TDSCModuleInfo tDscInfo;
        g_cMcuAgent.GetDscInfo(&tDscInfo);
		if ( htonl(tDscInfo.GetMcuInnerIp()) != g_cMcuAgent.GetMpcIp())
		{
			dwEqpIp = htonl(tDscInfo.GetMcuInnerIp());
			bModified2InnerIp = TRUE;
		}
    }   
    
    LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[StopSwitchToPeriEqp] bModify2Inner.%d, mcueqp recv ip: 0x%x\n", bModified2InnerIp, dwEqpIp);
#endif
    
    //ͼ��
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		if( g_cMcuVcApp.GetPeriEqpLogicChnnl( byEqpId, MODE_VIDEO, &byChannelNum, &tLogicalChannel, TRUE ) )
		{
#ifdef _MINIMCU_
            if (bModified2InnerIp)
            {
                tLogicalChannel.m_tRcvMediaChannel.SetIpAddr( dwEqpIp );
            }            
#endif
			u8 byMulPortSpace = 1;
// 			if (EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId))
// 			{
// 				u8 byOutNum = g_cMcuVcApp.GetBasOutPutChnNum(g_cMcuVcApp.GetEqp(byEqpId));
// 				byMulPortSpace = byOutNum;
// 			}
			StopSwitch( byConfIdx, tLogicalChannel, wDstChnnl, 1, byMulPortSpace);
		}
	}

	//����
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		if( g_cMcuVcApp.GetPeriEqpLogicChnnl( byEqpId, MODE_AUDIO, &byChannelNum, &tLogicalChannel, TRUE ) )
		{
#ifdef _MINIMCU_
            if (bModified2InnerIp)
            {
                tLogicalChannel.m_tRcvMediaChannel.SetIpAddr( dwEqpIp );
            }            
#endif
//             // Ŀǰ������basͨ���������10*2
//             if (EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId) &&
//                 g_cMcuAgent.IsEqpBasHD(byEqpId))
//             {
//                 wDstChnnl = 2 * wDstChnnl;
// 			}
			StopSwitch( byConfIdx, tLogicalChannel, wDstChnnl );
		}
	}
    
    // ˫��
    if ( byMode == MODE_SECVIDEO )
    {
        u8 byEqpType = g_cMcuAgent.GetPeriEqpType(byEqpId);
		TEqp tEqp = g_cMcuVcApp.GetEqp( byEqpId );
		if( g_cMcuVcApp.GetPeriEqpLogicChnnl( byEqpId, MODE_VIDEO, &byChannelNum, &tLogicalChannel, TRUE ) )
		{
#ifdef _MINIMCU_
            if (bModified2InnerIp)
            {
                tLogicalChannel.m_tRcvMediaChannel.SetIpAddr( dwEqpIp );
            }            
#endif
            if (EQP_TYPE_PRS == byEqpType)
            {
                StopSwitch( byConfIdx, 
                            tLogicalChannel.m_tRcvMediaChannel.GetIpAddr(),
		                    tLogicalChannel.m_tRcvMediaChannel.GetPort() + PORTSPAN * wDstChnnl, 
                            TRUE);
            }
			else if ( IsValidHduEqp( tEqp ) && IsValidHduChn(byEqpId, (u8)wDstChnnl, byHduSubChnId) )
			{
				//��˫������
				StopSwitch( byConfIdx, 
					tLogicalChannel.m_tRcvMediaChannel.GetIpAddr(),
					tLogicalChannel.m_tRcvMediaChannel.GetPort() + PORTSPAN * wDstChnnl, 
                            TRUE);
				//����hdu����Ƶ����
				TLogicalChannel tAudLGC;
				if( g_cMcuVcApp.GetPeriEqpLogicChnnl( byEqpId, MODE_AUDIO, &byChannelNum, &tAudLGC, TRUE ) )
				{
#ifdef _MINIMCU_
					if (bModified2InnerIp)
					{
						tAudLGC.m_tRcvMediaChannel.SetIpAddr( dwEqpIp );
					}            
#endif
					StopSwitch( byConfIdx, tAudLGC, wDstChnnl );
				}
			}
			else if ( EQP_TYPE_RECORDER == byEqpType )
			{
				StopSwitch( byConfIdx, 
						tLogicalChannel.m_tRcvMediaChannel.GetIpAddr(),
						tLogicalChannel.m_tRcvMediaChannel.GetPort() + PORTSPAN * wDstChnnl + 4, 
                        TRUE);
			
			}
			else if ( EQP_TYPE_VMP == byEqpType )
			{
				//��˫������
				StopSwitch( byConfIdx, 
					tLogicalChannel.m_tRcvMediaChannel.GetIpAddr(),
					tLogicalChannel.m_tRcvMediaChannel.GetPort() + PORTSPAN * wDstChnnl, 
					TRUE);
				
			}
            else 
            {
                //zbq[01/07/2009] ˫�����佻��
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
				if (g_cMcuAgent.IsEqpBasHD(byEqpId))
#endif          
                {
	//				u8 byOutNum = g_cMcuVcApp.GetBasOutPutChnNum(g_cMcuVcApp.GetEqp(byEqpId));
                    StopSwitch( byConfIdx, 
                        tLogicalChannel.m_tRcvMediaChannel.GetIpAddr(),
                        tLogicalChannel.m_tRcvMediaChannel.GetPort() + PORTSPAN * wDstChnnl /** byOutNum*/, 
                        TRUE);
                }
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
                else
                {
					StopSwitch( byConfIdx, 
                        tLogicalChannel.m_tRcvMediaChannel.GetIpAddr(),
                        tLogicalChannel.m_tRcvMediaChannel.GetPort() + PORTSPAN * wDstChnnl + 4, 
                        TRUE);
                }
#endif
            }  
		

		}        
    }
}

/*====================================================================
    ������      : StartSwitchToAll
    ����        ����ʼ����(����)
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMt &tSrc Դ�ն�/����
	              const u8 bySwitchGrpNum ��Դ�м��������飨bas�����ж����
				  TSwitchGrp *pSwitchGrp ����������
				  u8    byMode // ��/��Ƶģʽ
				  BOOL32 bStopBeforeStart // ��֮ǰ�Ƿ�Ҫ�Ȳ�֮ǰ�Ľ���
				  u16 wSpyStartPort // ��ش��˿ڣ��ش�Դ
    ����ֵ˵��  ����������ɹ�������ֵΪ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/07    1.0         JQL           ����
	03/07/15    1.0         ������        ��ҵ������
	20110505    4.6         pengjie	      ����������֧��
====================================================================*/
BOOL32 CMpManager::StartSwitchToAll( const TMt &tSrc,
									 const u8 bySwitchGrpNum,
									 TSwitchGrp *pSwitchGrp,
									 u8    byMode,
									 u8 bySwitchMode,
									 BOOL32 bStopBeforeStart,
									 u16 wSpyStartPort)
{
    CMcuVcInst * pcVcInst = g_cMcuVcApp.GetConfInstHandle(tSrc.GetConfIdx());
    TConfInfo  * ptConfInfo = pcVcInst != NULL ? &pcVcInst->m_tConf : NULL;
    TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable( tSrc.GetConfIdx() );

    u8 bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_NONE;
    u8 byUniformPayload = INVALID_PAYLOAD;
    u8 byIsSwitchRtcp = 0;
    u32 dwSrcMpIp = 0;
    u32 dwSrcIp = 0;
    u32 dwMpIP = 0;
    u32 dwDisIp = 0;
    u32 dwRcvIp = 0;
    u8 byManuID = 0;    
    u16 wRcvPort = 0;
    TLogicalChannel tLogicChannel;
	BOOL32 bRst = FALSE;

	if( bySwitchGrpNum == 0 || pSwitchGrp == NULL)
	{
		return FALSE;
	}

    if( ptConfMtTable == NULL || pcVcInst == NULL )
    {
        LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitch param err ptMtTable.0x%x ptVcInst.0x%x\n", ptConfMtTable, pcVcInst);
        return FALSE;
    }

	//��Դ����ת����Id����Ϣ
	BOOL32 bSndMsgBySrcMp = FALSE;
	if( (TYPE_MT == tSrc.GetType() ||
		   (TYPE_MCUPERI == tSrc.GetType() 
			&& tSrc.GetEqpType() == EQP_TYPE_BAS) 
		  )&&
		SWITCH_MODE_SELECT == bySwitchMode)
	{
		bSndMsgBySrcMp = TRUE;
	}

	BOOL32 bMixNCutSwitch = FALSE;
	if ( TYPE_MCUPERI == tSrc.GetType() && 
		(EQP_TYPE_MIXER == tSrc.GetEqpType()) &&
		SWITCH_MODE_SELECT == bySwitchMode )
	{
		OspPrintf(TRUE, FALSE, "[StartSwitchToAll] The switch mode is Mixer's N-1 Switch!\n");
		bMixNCutSwitch = TRUE;
	}

    //�õ�����Դ��ַ
    if( !GetSwitchInfo( tSrc, dwSrcMpIp, wRcvPort, dwSrcIp ) )
    {
        LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitch get switch info failed!\n");
        return FALSE ;
	}
    
	// src mp is Ipv4 or Ipv6? [pengguofeng 5/29/2012]
	BOOL32 bySrcMpIpType = g_cMcuVcApp.GetMpIpType(g_cMcuVcApp.FindMp(dwSrcMpIp));
	for ( u8 byMtLop1 = 0; byMtLop1 < bySwitchGrpNum; byMtLop1++ )
	{
		BOOL32 bFind = FALSE;
		u8 byDstMtNum = pSwitchGrp[byMtLop1].GetDstMtNum();
		if ( byDstMtNum == 0)
		{
			continue;
		}
		for ( u8 byMtLop2 = 0; byMtLop2 < byDstMtNum; byMtLop2++ )
		{
			TMt *pDstMt = pSwitchGrp[byMtLop1].GetDstMt();
			if ( pDstMt == NULL || pDstMt->IsNull())
			{
				continue;
			}

			if ( bySrcMpIpType == IP_V4
				&& pcVcInst->IsMtIpV6(pDstMt->GetMtId()) )
			{
				bSndMsgBySrcMp = FALSE;
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[StartSwitchToAll]src mp(%d) is Ipv4, and dst Mt(%d) is Ipv6, bSndMsgBySrcMp:%d!\n",
					g_cMcuVcApp.FindMp(dwSrcMpIp), pDstMt->GetMtId(), bSndMsgBySrcMp);
				bFind = TRUE;
				break;
			}
		}
		if ( bFind)
		{
			break;
		}
	}

	if( wSpyStartPort != SPY_CHANNL_NULL)
	{
		wRcvPort = wSpyStartPort;
	}
    u16 wTempRcvPort = wRcvPort;
    CServMsg cServMsg;
    cServMsg.SetConfIdx(tSrc.GetConfIdx());    
    TSwitchChannelExt tSwitchChannel;

    //[5/9/2011 zhushengze] GetMcInfo()��������
    u16 wMMcuIdx = INVALID_MCUIDX;
    if ( !pcVcInst->m_tCascadeMMCU.IsNull() )
    {
        wMMcuIdx = pcVcInst->GetMcuIdxFromMcuId( pcVcInst->m_tCascadeMMCU.GetMtId() );
	}

	// 1�� ��ͣ
	if( bStopBeforeStart )
	{
		CServMsg cServMsgToStop;
		cServMsgToStop.SetMsgBody();
		cServMsgToStop.SetConfIdx(tSrc.GetConfIdx());
		TSwitchChannel tSwitchChannelToStop;
		u8 byRemoveRtcp = 0;
		
		for( u8 bySrcChnl = 0; bySrcChnl < bySwitchGrpNum; bySrcChnl++ )
		{
			u8 byDstMtNum = pSwitchGrp[bySrcChnl].GetDstMtNum();
			TMt *pDstMt = pSwitchGrp[bySrcChnl].GetDstMt();
			if( pDstMt == NULL )
			{
				LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[StartSwitchToAll] pDstMt == NULL !\n" );
				continue;
			}
			
			for ( u8 byMtLoop = 0; byMtLoop < byDstMtNum; byMtLoop++ )
			{
				if (SWITCH_MODE_BROADCAST == bySwitchMode &&
					MODE_SECVIDEO != byMode &&
					!pcVcInst->m_tCascadeMMCU.IsNull()  &&
					pcVcInst->GetConfOtherMc()&&
					
					pcVcInst->GetConfOtherMc()->GetMcInfo(wMMcuIdx)&&
					pcVcInst->GetConfOtherMc()->GetMcInfo(wMMcuIdx)->m_tSpyMt == tSrc &&
					pcVcInst->m_tCascadeMMCU == pDstMt[byMtLoop] )
				{
					LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[StartSwitchToAll] tSrc.%d create no brd switch to MMcu.%d\n", 
						tSrc.GetMtId(), pcVcInst->m_tCascadeMMCU.GetMtId() );
					continue;
				}
				
				/*�����㲥��Ƶ����ʱ������ƽ�����ǣ�ָ�����¹��򣬽�����ǰ���𽻻�
				 *1.��ǰĿ���ն���һ��Դ�͵�ǰ��Դ�����ն�
				 *2.��ǰĿ���ն���һ��Դ�͵�ǰ��Դ��ͬ������TMT��Դͨ����
				 */
				if (SWITCH_MODE_BROADCAST == bySwitchMode && MODE_VIDEO == byMode)
				{
					TMt tOldSrc;
					u8  byOldSrcChn = 0;
					if (pcVcInst->GetMtCurVidSrc(pDstMt[byMtLoop].GetMtId(), tOldSrc, byOldSrcChn))
					{
						
						if ((TYPE_MT == tOldSrc.GetType() && TYPE_MT == tSrc.GetType()) ||
							(tOldSrc == tSrc && byOldSrcChn == pSwitchGrp[bySrcChnl].GetSrcChnl()))
						{	
							LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[StartSwitchToAll] No need to stopswitch Mt.%d before start because its lastsrc<type.%d, EqpType.%d, McuId.%d, MtId.%d, SrcChn.%d>\n",
										pDstMt[byMtLoop].GetMtId(), tOldSrc.GetType(), tOldSrc.GetEqpType(), tOldSrc.GetMcuId(), tOldSrc.GetMtId(), byOldSrcChn);
							continue;
						}					
					}
				}
				byManuID = ptConfMtTable->GetManuId(pDstMt[byMtLoop].GetMtId()); 
				if (pDstMt[byMtLoop].IsNull())
				{
					continue;
				}
				
				if( byMode == MODE_VIDEO )
				{
					if( !ptConfMtTable->GetMtLogicChnnl(pDstMt[byMtLoop].GetMtId(), LOGCHL_VIDEO, &tLogicChannel, TRUE) )
					{
						continue;
					}
				}
				else if(byMode == MODE_SECVIDEO || byMode == MODE_VIDEO2SECOND)
				{
					if( !ptConfMtTable->GetMtLogicChnnl(pDstMt[byMtLoop].GetMtId(), LOGCHL_SECVIDEO, &tLogicChannel, TRUE) )
					{
						continue;
					}
				}
				else if( byMode == MODE_AUDIO)
				{
					if( !ptConfMtTable->GetMtLogicChnnl(pDstMt[byMtLoop].GetMtId(), LOGCHL_AUDIO, &tLogicChannel, TRUE) )
					{
						continue;
					}
				}
				else
				{
					LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[StartSwitchToAll] byMode is Invalid !!!\n" );
					return FALSE;
				}
				
				tSwitchChannelToStop.SetDstIP(tLogicChannel.m_tRcvMediaChannel.GetIpAddr());
				tSwitchChannelToStop.SetDstPort(tLogicChannel.m_tRcvMediaChannel.GetPort());
				
				cServMsgToStop.CatMsgBody( (u8 *)&tSwitchChannelToStop, sizeof(tSwitchChannelToStop) );
				cServMsgToStop.CatMsgBody( (u8 *)&byRemoveRtcp, sizeof(byRemoveRtcp) );
				
				LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "remove before start Mt.%d(%s:%d)  \n",
											 pDstMt[byMtLoop].GetMtId(),
											 StrOfIP(tSwitchChannelToStop.GetDstIP()), 
											 tSwitchChannelToStop.GetDstPort());
			}
		}
		
		if( cServMsgToStop.GetMsgBodyLen() > 0 )
		{
			g_cMpSsnApp.BroadcastToAllMpSsn(MCU_MP_REMOVESWITCH_REQ, cServMsgToStop.GetServMsg(), cServMsgToStop.GetServMsgLen());
		}
	}
	
	
	// 2�������½�
    for (u8 byMpLoop = 1; byMpLoop <= MAXNUM_DRI; byMpLoop ++)
    {
        if ( !g_cMcuVcApp.IsMpConnected(byMpLoop) )
        {
            continue;
        }
		
        cServMsg.SetDstDriId( byMpLoop );
        cServMsg.SetMsgBody();
        
        dwMpIP = g_cMcuVcApp.GetMpIpAddr( byMpLoop );
		
		// 8KH/G/I����Ӫ�̿��ܵ���src Mp Ip��dwMpIP��һ�£���ʵ����ͬһ��ת���� [5/30/2012 chendaiwei]
#if !defined(_8KI_) && !defined(_8KH_) && !defined(_8KE_)
		//zjl 20100510 ѡ�������ţ����ܰ���Ŀ���ն�����ת����Ϊ���ݷ���Ϣ����Ҫ����Դ����ת���巢��Ϣ
		if (bSndMsgBySrcMp &&
			dwMpIP != dwSrcMpIp)
		{
			continue;
		}
#endif
		BOOL32 bMulti = FALSE;
		for( u8 bySrcChnl = 0; bySrcChnl < bySwitchGrpNum; bySrcChnl++ )
		{
			u8 byDstMtNum = pSwitchGrp[bySrcChnl].GetDstMtNum();
			TMt *pDstMt = pSwitchGrp[bySrcChnl].GetDstMt();
			if( pDstMt == NULL )
			{
				LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "StartSwitch pDstMt is NULL\n" );
				continue;
			}
			wRcvPort = wTempRcvPort;
			wRcvPort = wRcvPort + PORTSPAN * pSwitchGrp[bySrcChnl].GetSrcChnl();
			
			if( byMode == MODE_AUDIO )
			{
				wRcvPort += 2;
			}
			else if( byMode == MODE_SECVIDEO )
			{

				if( !( (tSrc.GetType() == TYPE_MCUPERI && tSrc.GetEqpType() == EQP_TYPE_BAS) 
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
					&& g_cMcuAgent.IsEqpBasHD(tSrc.GetEqpId())
#endif						
					) )
				{
					wRcvPort += 4;
				}

			}
			else
			{
				// vide Port don't need add
			}
			u8 byMpId = 0;
			for ( u8 byMtLoop = 0; byMtLoop < byDstMtNum; byMtLoop ++ )
			{
				// ֪ͨ���ǽ��յ�ַ [pengguofeng 2/21/2013]
				TMt tDstMt = pDstMt[byMtLoop];
				if ( bySwitchMode == SWITCH_MODE_BROADCAST )
				{
					bMulti = TRUE;
				}
				else if ( bySwitchMode == SWITCH_MODE_SELECT 
					&& tSrc == pcVcInst->GetVmpEqp() )
				{
					bMulti = TRUE; //vmp��������ʱ��ģʽ��ľ���Select�����Դ˵ر�֤һ��
				}
				
				if ( pcVcInst->IsMultiCastMt(tDstMt.GetMtId()) )
				{
					LogPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "MP:[StartSwitchToAll]SatMt:%d Mode:%d SwitchMode:%d\n",
						tDstMt.GetMtId(), byMode, bySwitchMode);
					if ( byMode & MODE_VIDEO )
					{
						pcVcInst->ChangeSatDConfMtRcvAddr(tDstMt.GetMtId(), LOGCHL_VIDEO, bMulti);
					}
					if ( byMode & MODE_AUDIO )
					{
						pcVcInst->ChangeSatDConfMtRcvAddr(tDstMt.GetMtId(), LOGCHL_AUDIO, bMulti);
					}
					if ( byMode == MODE_SECVIDEO )
					{
						pcVcInst->ChangeSatDConfMtRcvAddr(tDstMt.GetMtId(), LOGCHL_SECVIDEO, bMulti);
					}
					
					if ( bMulti )
					{
						bRst = TRUE; //Ϊ����McuVcInst�ĸýӿ�������
						continue;//���㲥����VMP�㲥ʱ�������ն˲�������������
					}
				}

				// ÿ�δ���ǰMP������ն�
				byMpId = ptConfMtTable->GetMpId( pDstMt[byMtLoop].GetMtId() );
				if (!bSndMsgBySrcMp && 
					byMpId != byMpLoop )
				{
					continue;
				}
				
				// zbq [06/29/2007] �ش�Դ�Ĺ㲥���������滻����ͨ�ش�����
				if (SWITCH_MODE_BROADCAST == bySwitchMode &&
					MODE_SECVIDEO != byMode &&
					!pcVcInst->m_tCascadeMMCU.IsNull()  &&
					pcVcInst->GetConfOtherMc()->GetMcInfo(wMMcuIdx)&&
					pcVcInst->GetConfOtherMc()->GetMcInfo(wMMcuIdx)->m_tSpyMt == tSrc &&
					pcVcInst->m_tCascadeMMCU == pDstMt[byMtLoop] )
				{
					LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "StartSwitch tSrc.%d create no brd switch to MMcu.%d\n", 
						tSrc.GetMtId(), pcVcInst->m_tCascadeMMCU.GetMtId() );
					continue;
				}
				
				byManuID = ptConfMtTable->GetManuId(pDstMt[byMtLoop].GetMtId()); 
				
				if( byMode == MODE_VIDEO )
				{
					if( !ptConfMtTable->GetMtLogicChnnl(pDstMt[byMtLoop].GetMtId(), LOGCHL_VIDEO, &tLogicChannel, TRUE) )
					{
						continue;
					}
				}
				else if(byMode == MODE_SECVIDEO || byMode == MODE_VIDEO2SECOND)
				{
					if( !ptConfMtTable->GetMtLogicChnnl(pDstMt[byMtLoop].GetMtId(), LOGCHL_SECVIDEO, &tLogicChannel, TRUE) )
					{
						continue;
					}
				}
				else if( byMode == MODE_AUDIO)
				{
					if( !ptConfMtTable->GetMtLogicChnnl(pDstMt[byMtLoop].GetMtId(), LOGCHL_AUDIO, &tLogicChannel, TRUE) )
					{
						continue;
					}
				}
				else
				{
					LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitch byMode is Invalid\n" );
					return FALSE;
				}
				
				TConfAttrb tConfAttrb;
				if( ptConfInfo != NULL )
				{
					tConfAttrb = ptConfInfo->GetConfAttrb();
				}

				if (( TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_VMP == tSrc.GetEqpType() && MT_TYPE_MT != pDstMt[byMtLoop].GetMtType()
					  && ptConfInfo
					  && tConfAttrb.IsResendLosePack()) 
					|| tSrc == pDstMt[byMtLoop]) //  xsl [1/17/2006] (�⳧��mt��mcu)����Ƶ�����������Լ�ʱ������rtcp����
				{
					byIsSwitchRtcp = 0;
				}
				else
				{
					byIsSwitchRtcp = 1;
				}
				
				if (byManuID == MT_MANU_KDC || byManuID == MT_MANU_KDCMCU)
				{
					bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_NONE;
				}
				else
				{
					bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_VALID;
					if (TYPE_MT == tSrc.GetType() && TYPE_MT == pDstMt[byMtLoop].GetType())
					{
						if (tLogicChannel.GetChannelType() >= AP_MIN && tLogicChannel.GetChannelType() <= AP_MAX)
						{
							byUniformPayload = tLogicChannel.GetChannelType();
						}
					}
				}
				

				//8000H/8000G/8000Iֱ��ȡԴMP��IP [5/25/2012 chendaiwei]
#if defined(_8KH_) || defined(_8KE_) || defined(_8KI_)
				dwRcvIp = dwSrcMpIp;

				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_MPMGR,"[startswitchToAll] dwRcvIP = dwSrcMpIp == %s\n",StrOfIP(dwSrcMpIp));	
#else
				//���ѡ��������IPֱ��ָ��ΪԴ����ת����ip,�ַ�ipΪ0
				if (bSndMsgBySrcMp)
				{
					dwRcvIp = dwSrcMpIp;
				}
				else
				{	
					//�õ�Ŀ��Mt�Ľ�����Ϣ                    
					if( dwMpIP != dwSrcMpIp )//��Ҫת��
					{
						dwDisIp = dwSrcMpIp;
					}
					else
					{
						dwDisIp = 0;                
					}   
					dwRcvIp = dwMpIP;
				}
#endif				
				
				//��������
				
				//���콻���ŵ�             
				tSwitchChannel.Clear();
				tSwitchChannel.SetSrcMt( tSrc );
				tSwitchChannel.SetSrcIp( dwSrcIp );
				tSwitchChannel.SetRcvIP( dwRcvIp );
				tSwitchChannel.SetRcvPort( wRcvPort );
				
				// ������N-1ģʽ������������ֱ�Ӵ����������Ա����ת����
				if(bMixNCutSwitch)
				{
					//[pengguofeng 5/11/2012]��Ȼ�˴�Ŀǰ����Ҫ�ģ���MP�������жϣ��������հ汾����Ҫ�ĵ� 
					tSwitchChannel.SetDisIp( 0 );
				}
				else
				{
					tSwitchChannel.SetDisIp( dwDisIp );
				}

				tSwitchChannel.SetRcvBindIP( dwRcvIp );
				tSwitchChannel.SetSndBindIP( dwRcvIp );
				tSwitchChannel.SetDstIP( tLogicChannel.m_tRcvMediaChannel.GetIpAddr() );
				tSwitchChannel.SetDstPort( tLogicChannel.m_tRcvMediaChannel.GetPort() );
				//���������ļ���ȡ�Ƿ�Ҫ���ٵ�ip/port����dstip��port�Ƿ���Ҫ���ٵ�
				if ( g_cMcuVcApp.IsNeedMapDstIpAddr( htonl(tSwitchChannel.GetDstIP()) ) )
				{
					//���ٵ�ipΪĿ���ն�����ת����ip���˿�Ϊ����ͨ���Ķ˿�
					u32 dwMappedIp = 0;
					u16 wMappedPort = 0 ;
					u32 dwDstSrcIp = 0;
					GetSwitchInfo(pDstMt[byMtLoop],dwMappedIp,wMappedPort,dwDstSrcIp);
					if ( dwMappedIp == 0 || wMappedPort == 0 )//��ȡʧ�ܣ����ù̶���ip�Ͷ˿�
					{
						StaticLog("[StartSwitchToAll]Get dstmt(mtid:%d) switchip and port failed\n",pDstMt[byMtLoop].GetMtId());
						//����Ϊrecvip��port
						dwMappedIp = dwRcvIp;
						wMappedPort = DEFAULT_MAPPORT;

					}
					//�ɹ�����ȡ������ip�Ͷ˿�
					tSwitchChannel.SetMapIp( dwMappedIp );
					if ( MODE_AUDIO == byMode )
					{
						wMappedPort += 2;
					}
					else if (MODE_SECVIDEO == byMode)
					{
						wMappedPort += 4;
					}
					tSwitchChannel.SetMapPort(wMappedPort);
				}
			
				// ׼������ǰMP����Ϣ��
				cServMsg.CatMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));                  
				cServMsg.CatMsgBody((u8 *)&bySwitchChannelMode, sizeof(u8));
				cServMsg.CatMsgBody((u8 *)&byUniformPayload, sizeof(u8));            
				cServMsg.CatMsgBody((u8 *)&byIsSwitchRtcp, sizeof(u8));
				
				LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[StartSwitchToAll]Add Mt.%d(%s:%d) From %s(dis:%s)in Msg to Mp.%d \n",
					pDstMt[byMtLoop].GetMtId(),
					StrOfIP(tSwitchChannel.GetDstIP()), tSwitchChannel.GetDstPort(), StrOfIP(dwSrcIp), StrOfIP(dwDisIp),byMpLoop);
			}
		}

        if ( cServMsg.GetMsgBodyLen() > 0 )
        {
			LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "Send MCU_MP_ADDSWITCH_REQ to Mp.%d \n", byMpLoop );
            g_cMpSsnApp.SendMsgToMpSsn( byMpLoop, 
                                        MCU_MP_ADDSWITCH_REQ, 
                                        cServMsg.GetServMsg(), 
                                        cServMsg.GetServMsgLen());
			bRst = TRUE;
        }
    }

    return bRst;
}

/*====================================================================
    ������      : StartSwitch
    ����        ����ʼ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����dwRcvIp     Դ��ַ
				  wRcvPort    Դ�˿ں�
				  dwDstIp     Ŀ�ĵ�ַ
				  wDstPort    Ŀ�Ķ˿ں�
    ����ֵ˵��  ����������ɹ�������ֵΪ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/07    1.0         JQL           ����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
BOOL32 CMpManager::StartSwitch(const TMt & tSrcMt, u8 byConfIdx, 
                             u32 dwSrcIp, u32 dwDisIp, 
                             u32 dwRcvIp, u16 wRcvPort, 
                             u32 dwDstIp, u16 wDstPort, 
                             u32 dwRcvBindIP, u32 dwSndBindIP,
                             u8 bySwitchChannelMode,
                             u8 byUniformPayload,
                             u8 byIsSwitchRtcp, BOOL32 bStopBeforeStart,u8 byDstMtId)
{
	//����IP��ַ���ҽ���MP���
	u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	//û�ҵ�
	if( 0 == byMpId )
	{
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "No MP's Addrs is 0x%x in StartSwitch(),please check switch recv IP!\n",dwRcvIp);
		return FALSE;
	}

	//���콻���ŵ� 
	TSwitchChannelExt tSwitchChannel;
	tSwitchChannel.SetSrcMt( tSrcMt );
	tSwitchChannel.SetDisIp( dwDisIp );
	tSwitchChannel.SetSrcIp( dwSrcIp );
    tSwitchChannel.SetRcvIP( dwRcvIp );
	tSwitchChannel.SetRcvPort( wRcvPort );
	if( 0 == dwRcvBindIP )
	{
		tSwitchChannel.SetRcvBindIP( dwRcvIp );
	}
	else
	{
		tSwitchChannel.SetRcvBindIP( dwRcvBindIP );
	}
	tSwitchChannel.SetDstIP( dwDstIp );
	tSwitchChannel.SetDstPort( wDstPort );
	if( 0 == dwSndBindIP )
	{
		if( 0 == dwRcvBindIP )
		{
			tSwitchChannel.SetSndBindIP(dwRcvIp);
		}
		else
		{
			tSwitchChannel.SetSndBindIP(dwRcvBindIP);
		}
	}
	else
	{
		tSwitchChannel.SetSndBindIP(dwSndBindIP);
	}
	//���������ļ���ȡ�Ƿ�Ҫ���ٵ�ip/port����dstip��port�Ƿ���Ҫ���ٵ�
	if ( byDstMtId != 0 && g_cMcuVcApp.IsNeedMapDstIpAddr( htonl(dwDstIp) ) )
	{
		//���ٵ�ipΪĿ���ն�����ת����ip���˿�Ϊ����ͨ���Ķ˿�
		u32 dwMappedIp = 0;
		u16 wMappedPort = 0 ;
		u32 dwDstSrcIp = 0;
		TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable( byConfIdx );
		if( ptConfMtTable == NULL )
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitch() failure because invalid ConfMtTable Pointer!\n");
			return FALSE;
		}
		GetSwitchInfo( ptConfMtTable->GetMt(byDstMtId),dwMappedIp,wMappedPort,dwDstSrcIp);
		if ( dwMappedIp == 0 || wMappedPort == 0 )//ʧ������rcvip�͹̶�port
		{
			StaticLog("[StartSwitchToAll]Get dstmt(mtid:%d) switchip and port failed\n",byDstMtId);
			dwMappedIp = dwRcvIp;
			wMappedPort = DEFAULT_MAPPORT;

		}
		
		tSwitchChannel.SetMapIp( dwMappedIp );
		if ( wDstPort%PORTSPAN == 2 )
		{
			wMappedPort += 2;
		}
		else if (wDstPort%PORTSPAN == 4)
		{
			wMappedPort += 4;
		}
		tSwitchChannel.SetMapPort(wMappedPort);
			
	}

	//ֹͣ�����豸����һ��ַ��������
    // guzh [6/6/2007] ����MP����
    // zbq [06/15/2007] ��ʱ������MP����
    // zbq [06/29/2007] ���⴦���ȷ����Ҫ�ص��ؿ��Ľ���
    if ( bStopBeforeStart )
    {
		StartStopSwitch(byConfIdx, dwDstIp, wDstPort, byMpId);
    }

	//�����ӽ�����Ϣ��MP
	CServMsg cServMsg;
	cServMsg.SetConfIdx(byConfIdx);
	cServMsg.SetDstDriId(byMpId);
	cServMsg.SetMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));

    // libo [5/13/2005]
    cServMsg.CatMsgBody((u8 *)&bySwitchChannelMode, sizeof(u8));
    // libo [5/13/2005]end

    cServMsg.CatMsgBody((u8 *)&byUniformPayload, sizeof(u8));

    cServMsg.CatMsgBody((u8 *)&byIsSwitchRtcp, sizeof(u8));

	g_cMpSsnApp.SendMsgToMpSsn(byMpId, MCU_MP_ADDSWITCH_REQ, 
                               cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

	s8 achRcvIp[17] = {0};
	memcpy(achRcvIp, StrOfIP(tSwitchChannel.GetRcvIP()), sizeof(achRcvIp));
    LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "Send Message: MCU_MP_ADDSWITCH_REQ for %s:%d --> %s:%d to Mp.\n",
                 achRcvIp, tSwitchChannel.GetRcvPort(),
                 StrOfIP(tSwitchChannel.GetDstIP()), tSwitchChannel.GetDstPort() );

    return TRUE; 
}

/*====================================================================
    ������      : StopSwitch
    ����        ��ֹͣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����dwDstIp Ŀ�ĵ�ַ
				  wDstPort Ŀ�Ķ˿ں�
    ����ֵ˵��  �����ֹͣ�����ɹ�������ֵΪ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/07    1.0         JQL           ����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
BOOL32 CMpManager::StopSwitch(u8 byConfIdx, u32 dwDstIp, u16 wDstPort, u8 byIsSwitchRtcp)
{
    //���콻���ŵ� 
	TSwitchChannel tSwitchChannel;
	tSwitchChannel.SetDstIP(dwDstIp);
	tSwitchChannel.SetDstPort(wDstPort);

	//���Ƴ�������Ϣ��MP
	CServMsg cServMsg;
	cServMsg.SetConfIdx(byConfIdx);

	cServMsg.SetMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));

    cServMsg.CatMsgBody((u8 *)&byIsSwitchRtcp, sizeof(u8));

    g_cMpSsnApp.BroadcastToAllMpSsn(MCU_MP_REMOVESWITCH_REQ, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

    LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "Send Message: MCU_MP_REMOVESWITCH_REQ for %s:%d to Mp in StopSwitch().\n",
                 StrOfIP(tSwitchChannel.GetDstIP()), tSwitchChannel.GetDstPort() );

    return TRUE;
}

/*=============================================================================
  �� �� ���� StartStopSwitch
  ��    �ܣ� ��ʼ�µĽ���ǰ���ԭ���Ľ������˴�û���Ƴ�RTCP������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfIdx
             u32 dwDstIp
             u16 wDstPort
             u8 byMpId
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CMpManager::StartStopSwitch(u8 byConfIdx, u32 dwDstIp, u16 wDstPort, u8 byMpId)
{
    //���콻���ŵ� 
	TSwitchChannel tSwitchChannel;
	tSwitchChannel.SetDstIP(dwDstIp);
	tSwitchChannel.SetDstPort(wDstPort);
	
	//���Ƴ�������Ϣ��MP
    u8 byRemoveRtcp = 0;
	CServMsg cServMsg;
	cServMsg.SetConfIdx( byConfIdx );
	cServMsg.SetMsgBody((u8 *)&tSwitchChannel,sizeof(tSwitchChannel));
    cServMsg.CatMsgBody( &byRemoveRtcp, sizeof(byRemoveRtcp) );

    //u32 dwIpAddr = g_cMcuAgent.GetCastIpAddr();   
    u32 dwIpAddr = g_cMcuVcApp.AssignMulticastIp(byConfIdx);   
    if (dwIpAddr == htonl(dwDstIp))
    {
        g_cMpSsnApp.SendMsgToMpSsn(byMpId, MCU_MP_REMOVESWITCH_REQ,
                                   cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
    }
    else
    {
        g_cMpSsnApp.BroadcastToAllMpSsn(MCU_MP_REMOVESWITCH_REQ, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
    }

    LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "Send Message: MCU_MP_REMOVESWITCH_REQ for %s:%d to Mp in StartStopSwitch() .\n",
                 StrOfIP(tSwitchChannel.GetDstIP()), tSwitchChannel.GetDstPort());

    return TRUE;
}

/*====================================================================
    ������      ��StartSwitch
    ����        ����ָ���˿����ݽ�����ǰ���߼��ŵ�ָ���Ķ˿�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����dwRcvIp,    Դ��ַ
				  wRcvPort,   Դ�˿ں�
				  TFwdChnnl,  ǰ���߼��ŵ�
				  byDstChnnl, �ŵ�������ȱʡΪ0
                  bStopBeforeStart ���½���ǰ�Ƿ�ͣ����
    ����ֵ˵��  ����������ɹ�������ֵΪ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/07    1.0         JQL           ����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
BOOL32 CMpManager::StartSwitch( const TMt & tSrcMt, u8 byConfIdx, 
							  u32 dwSrcIp, u32 dwDisIp, 
							  u32 dwRcvIp, u16 wRcvPort, 
							  const TLogicalChannel& tFwdChnnl, u16 wDstChnnl,
                              u8 bySwitchChannelMode,
                              u8 byUniformPayload,
                              u8 byIsSwitchRtcp, BOOL32 bStopBeforeStart,u8 byDstMtId)
{
	return StartSwitch(tSrcMt, byConfIdx, dwSrcIp, dwDisIp, dwRcvIp, wRcvPort, 
		               tFwdChnnl.m_tRcvMediaChannel.GetIpAddr(),
		               (tFwdChnnl.m_tRcvMediaChannel.GetPort() + PORTSPAN*wDstChnnl),
                       0, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp, bStopBeforeStart,byDstMtId);
}

/*====================================================================
    ������      ��StopSwitch
    ����        ��ֹͣ��ǰ���߼��ŵ�ָ���Ķ˿ڽ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tFwdChnnl, Ŀ���߼��ŵ�
				  byDstChnnl, �ŵ�������ȱʡΪ0
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/07    1.0         JQL           ����
	03/07/15    1.0         ������        ��ҵ������
====================================================================*/
BOOL32 CMpManager::StopSwitch(u8 byConfIdx, const TLogicalChannel& tFwdChnnl, u16 wDstChnnl, u8 byIsSwitchRtcp, u8 byMulPortSpace)
{
	return StopSwitch( byConfIdx, tFwdChnnl.m_tRcvMediaChannel.GetIpAddr(),
		              tFwdChnnl.m_tRcvMediaChannel.GetPort() + PORTSPAN * wDstChnnl * byMulPortSpace, byIsSwitchRtcp);
}

/*====================================================================
    ������      ��AddMultiToOneSwitch
    ����        �����Ӷ�㵽һ��Ľ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx, ����������������
				  u32 dwSrcIp, ԴIP
				  u32 dwDisIp, �ַ�IP
                  u32 dwRcvIp, ����IP
				  u16 wRcvPort, ���ն˿�
				  u32 dwDstIp, Ŀ��IP
				  u16 wDstPort, Ŀ�Ķ˿�
				  u32 dwRcvBindIP, ���հ�IP
				  u32 dwSndBindIP, ���ͰﶨIP
				  u32 dwMapIpAddr
				  u16 wMapPort
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/05/12    3.0         ������        ����
====================================================================*/
BOOL32 CMpManager::AddMultiToOneSwitch( u8 byConfIdx, u32 dwSrcIp, u32 dwDisIp, u32 dwRcvIp, u16 wRcvPort, 
						              u32 dwDstIp, u16 wDstPort, u32 dwRcvBindIP, u32 dwSndBindIP, 
									  u32 dwMapIpAddr, u16 wMapPort )
{
	//����IP��ַ���ҽ���MP���
	u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	
	LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "[AddMultiToOneSwitch] MP %s, port %d!",StrOfIP(dwRcvIp), wRcvPort);

	//û�ҵ�
	if(byMpId==0)
	{
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "No MP's Addrs is 0x%x in AddMultiToOneSwitch(),please check switch recv IP!",dwRcvIp);
		return FALSE;
	}

	//���콻���ŵ� 
	TSwitchChannel tSwitchChannel;
	tSwitchChannel.SetDisIp( dwDisIp );
	tSwitchChannel.SetSrcIp( dwSrcIp );
	tSwitchChannel.SetRcvIP( dwRcvIp );
	tSwitchChannel.SetRcvPort( wRcvPort );
	if( dwRcvBindIP == 0 )
	{
		tSwitchChannel.SetRcvBindIP( dwRcvIp );
	}
	else
	{
		tSwitchChannel.SetRcvBindIP( dwRcvBindIP );	
	}
	tSwitchChannel.SetDstIP( dwDstIp );
	tSwitchChannel.SetDstPort( wDstPort );
	if(dwSndBindIP==0)
	{
		if(dwRcvBindIP==0)tSwitchChannel.SetSndBindIP(dwRcvIp);
		else tSwitchChannel.SetSndBindIP(dwRcvBindIP);
	}
	else
	{
		tSwitchChannel.SetSndBindIP(dwSndBindIP);
	}
	wMapPort    = htons(wMapPort);
	dwMapIpAddr = htonl(dwMapIpAddr);

	//�����ӽ�����Ϣ��MP
	CServMsg cServMsg;
	cServMsg.SetConfIdx( byConfIdx );
	cServMsg.SetDstDriId( byMpId );
	cServMsg.SetMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));
	cServMsg.CatMsgBody((u8 *)&dwMapIpAddr, sizeof(dwMapIpAddr));
	cServMsg.CatMsgBody((u8 *)&wMapPort, sizeof(wMapPort));
	g_cMpSsnApp.SendMsgToMpSsn( byMpId, MCU_MP_ADDMULTITOONESWITCH_REQ, 
		                        cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

    return TRUE; 

}
/*====================================================================
    ������      ��AddMultiToOneSwitch
    ����        �����Ӷ�㵽һ��Ľ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx,							����������������
				  u8 byDstNum							Ŀ����
				  TSwitchChannelExt *ptSwitchChannelExt ������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	11/05/06    4.0         �ܼ���          ����
====================================================================*/
BOOL32 CMpManager::AddMultiToOneSwitch(u8 byConfIdx, 
									   u8 byDstNum, 
									   TSwitchChannelExt *ptSwitchChannelExt)
{
	if (0 == byDstNum || NULL == ptSwitchChannelExt)
	{
		return FALSE;
	}

	CServMsg cServMsg;
	cServMsg.SetConfIdx( byConfIdx );
	for (u8 byMpLoop = 1; byMpLoop <= MAXNUM_DRI; byMpLoop ++)
    {
        if ( !g_cMcuVcApp.IsMpConnected(byMpLoop) )
        {
            continue;
        }
		
		cServMsg.SetDstDriId(byMpLoop);
		cServMsg.SetMsgBody();
		for (u8 byIdx = 0; byIdx < byDstNum; byIdx++)
		{
			//����IP��ַ���ҽ���MP���
			u32 dwRcvIp = ptSwitchChannelExt[byIdx].GetRcvIP();
			u8   byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
			
			//û�ҵ�
			if(byMpId==0)
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_MSMGR, "No MP's Addrs is 0x%x in AddMultiToOneSwitch(),please check switch recv IP!",dwRcvIp);
				continue;
			}
			
			if (byMpId != byMpLoop)
			{
				continue;
			}

			//���콻���ŵ� 
			TSwitchChannel tSwitchChannel;
			tSwitchChannel.SetDisIp(ptSwitchChannelExt[byIdx].GetDisIp());
			tSwitchChannel.SetSrcIp(ptSwitchChannelExt[byIdx].GetSrcIp());

			tSwitchChannel.SetRcvIP(ptSwitchChannelExt[byIdx].GetRcvIP());
			tSwitchChannel.SetRcvPort(ptSwitchChannelExt[byIdx].GetRcvPort());

			tSwitchChannel.SetRcvBindIP(ptSwitchChannelExt[byIdx].GetRcvBindIP());
			tSwitchChannel.SetSndBindIP(ptSwitchChannelExt[byIdx].GetSndBindIP());

			tSwitchChannel.SetDstIP(ptSwitchChannelExt[byIdx].GetDstIP());
			tSwitchChannel.SetDstPort(ptSwitchChannelExt[byIdx].GetDstPort());

			u16 wMapPort    = htons(ptSwitchChannelExt[byIdx].GetMapPort());
			u32 dwMapIpAddr = htonl(ptSwitchChannelExt[byIdx].GetMapIp());
			
			//�����ӽ�����Ϣ��MP
			cServMsg.CatMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));
			cServMsg.CatMsgBody((u8 *)&dwMapIpAddr, sizeof(dwMapIpAddr));
			cServMsg.CatMsgBody((u8 *)&wMapPort, sizeof(wMapPort));
		}
		if (cServMsg.GetMsgBodyLen() > 0)
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_MSMGR, "Send MCU_MP_ADDMULTITOONESWITCH_REQ to Mp.%d !!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", byMpLoop );
			g_cMpSsnApp.SendMsgToMpSsn( byMpLoop, MCU_MP_ADDMULTITOONESWITCH_REQ, 
				cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		}
	}
    return TRUE; 	
}

/*====================================================================
    ������      RemoveMultiToOneSwitch
    ����        ���Ƴ���㵽һ��Ľ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx,							����������������
				  u8 byDstNum							Ŀ����
				  TSwitchChannelExt *ptSwitchChannelExt ������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	11/05/06    4.0         �ܼ���          ����
====================================================================*/
BOOL32 CMpManager::RemoveMultiToOneSwitch(u8 byConfIdx, u8 byDstNum, TSwitchChannelExt *ptSwitchChannelExt)
{
	if (0 == byDstNum || NULL == ptSwitchChannelExt)
	{
		return FALSE;
	}

	CServMsg cServMsg;
	cServMsg.SetConfIdx( byConfIdx );

	for (u8 byMpLoop = 1; byMpLoop <= MAXNUM_DRI; byMpLoop ++)
    {
        if ( !g_cMcuVcApp.IsMpConnected(byMpLoop) )
        {
            continue;
        }

		cServMsg.SetDstDriId(byMpLoop);
		cServMsg.SetMsgBody();

		for (u8 byIdx = 0; byIdx < byDstNum; byIdx++)
		{
			u32 dwRcvIp = ptSwitchChannelExt[byIdx].GetRcvIP();
	   	   //����IP��ַ���ҽ���MP���
			u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	
	       //û�ҵ�
	    	if(byMpId==0)
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_MSMGR,"No MP's Addrs is 0x%x in RemoveMultiToOneSwitch(),please check switch recv IP!\n",dwRcvIp);
				continue;
			}
			
			if (byMpId != byMpLoop)
			{
				continue;
			}
			//���콻���ŵ� 
			TSwitchChannel tSwitchChannel;
			tSwitchChannel.SetRcvIP(ptSwitchChannelExt[byIdx].GetRcvIP());
			tSwitchChannel.SetRcvPort(ptSwitchChannelExt[byIdx].GetRcvPort());
			tSwitchChannel.SetDstIP(ptSwitchChannelExt[byIdx].GetDstIP());
			tSwitchChannel.SetDstPort(ptSwitchChannelExt[byIdx].GetDstPort());
			//�����ӽ�����Ϣ��MP
			cServMsg.CatMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));
		}
		if (cServMsg.GetMsgBodyLen() > 0)
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_MSMGR, "Send MCU_MP_REMOVEMULTITOONESWITCH_REQ to Mp.%d !!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", byMpLoop );
			g_cMpSsnApp.SendMsgToMpSsn( byMpLoop, MCU_MP_REMOVEMULTITOONESWITCH_REQ, 
								cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		}
	}	
    return TRUE;
}
/*====================================================================
    ������      ��RemoveMultiToOneSwitch
    ����        ���Ƴ���㵽һ��Ľ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx, ����������������
                  u32 dwRcvIp, ����IP
				  u16 wRcvPort, ���ն˿�
				  u32 dwDstIp, Ŀ��IP
				  u16 wDstPort, Ŀ�Ķ˿�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/05/12    3.0         ������        ����
====================================================================*/
BOOL32 CMpManager::RemoveMultiToOneSwitch( u8 byConfIdx, u32 dwRcvIp, u16 wRcvPort, u32 dwDstIp, u16 wDstPort )
{
	//����IP��ַ���ҽ���MP���
	u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	
	//û�ҵ�
	if(byMpId==0)
	{
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "No MP's Addrs is 0x%x in RemoveMultiToOneSwitch(),please check switch recv IP!\n",dwRcvIp);
		return FALSE;
	}

	//���콻���ŵ� 
	TSwitchChannel tSwitchChannel;
    tSwitchChannel.SetRcvIP( dwRcvIp );
	tSwitchChannel.SetRcvPort( wRcvPort );
	tSwitchChannel.SetDstIP( dwDstIp );
	tSwitchChannel.SetDstPort( wDstPort );

	//�����ӽ�����Ϣ��MP
	CServMsg cServMsg;
	cServMsg.SetConfIdx( byConfIdx );
	cServMsg.SetDstDriId( byMpId );
	cServMsg.SetMsgBody((u8 *)&tSwitchChannel,sizeof(tSwitchChannel));
	g_cMpSsnApp.SendMsgToMpSsn( byMpId, MCU_MP_REMOVEMULTITOONESWITCH_REQ, 
		                        cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

    return TRUE;
}

/*====================================================================
    ������      ��StopMultiToOneSwitch
    ����        ��ֹͣ��㵽һ��Ľ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx, ����������������
				  u32 dwDstIp, Ŀ��IP
				  u16 wDstPort, Ŀ�Ķ˿�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/05/12    3.0         ������        ����
====================================================================*/
BOOL32 CMpManager::StopMultiToOneSwitch( u8 byConfIdx, u32 dwDstIp, u16 wDstPort )
{
    //���콻���ŵ� 
	TSwitchChannel tSwitchChannel;
	tSwitchChannel.SetDstIP(dwDstIp);
	tSwitchChannel.SetDstPort(wDstPort);
	
	//���Ƴ�������Ϣ��MP
	CServMsg cServMsg;
	cServMsg.SetConfIdx( byConfIdx );
	cServMsg.SetMsgBody((u8 *)&tSwitchChannel,sizeof(tSwitchChannel));
	//g_cMpSsnApp.BroadcastToAllMpSsn( MCU_MP_REMOVEMULTITOONESWITCH_REQ,
	//	                             cServMsg.GetServMsg(),cServMsg.GetServMsgLen());

    //MCU_MP_STOPMULTITOONESWITCH_REQ
    g_cMpSsnApp.BroadcastToAllMpSsn(MCU_MP_STOPMULTITOONESWITCH_REQ,
		                            cServMsg.GetServMsg(),cServMsg.GetServMsgLen());

    return TRUE;
}

/*====================================================================
    ������      ��AddOnlyRecvSwitch
    ����        ����mp����ssrc����ֻ���ս�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx, ����������������
				  u32 dwRcvIp, ����IP
				  u16 wRcvPort, ���ն˿�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/05/12    3.0         ������        ����
	20120/5/14	4.7			pgf				modify
====================================================================*/
BOOL32 CMpManager::AddRecvOnlySwitch( u8 byConfIdx, u32 dwSrcIp,u32 dwRcvIp, u16 wRcvPort,u32 dwRtcpBackIp,u16 wRtcpBackPort )
{
	//����IP��ַ���ҽ���MP���
	u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	
	//û�ҵ�
	if(byMpId==0)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "No MP's Addrs is 0x%x in AddRecvOnlySwitch(),please check switch recv IP!\n",dwRcvIp);
		return FALSE;
	}

	//���콻���ŵ� 
	TSwitchChannel tSwitchChannel; 
	tSwitchChannel.SetSrcIp(dwSrcIp);
    tSwitchChannel.SetRcvIP( dwRcvIp );
	tSwitchChannel.SetRcvPort( wRcvPort );
	tSwitchChannel.SetRcvBindIP( dwRcvIp );
	tSwitchChannel.SetDstIP(dwRtcpBackIp);
	tSwitchChannel.SetDstPort(wRtcpBackPort);

	//�����ӽ�����Ϣ��MP
	CServMsg cServMsg;
	cServMsg.SetConfIdx( byConfIdx );
	cServMsg.SetDstDriId( byMpId );
	cServMsg.SetMsgBody((u8   *)&tSwitchChannel,sizeof(tSwitchChannel));
	StaticLog("Send MCU_MP_ADDRECVONLYSWITCH_REQ to mp!!\n");
	g_cMpSsnApp.SendMsgToMpSsn( byMpId, MCU_MP_ADDRECVONLYSWITCH_REQ, 
		                        cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

    return TRUE; 
}
	

/*====================================================================
    ������      ��RemoveOnlyRecvSwitch
    ����        ����mp����ssrc�����Ƴ�ֻ���ս�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byConfIdx, ����������������
				  u32 dwRcvIp, ����IP
				  u16 wRcvPort, ���ն˿�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/05/12    3.0         ������        ����
====================================================================*/
BOOL32 CMpManager::RemoveRecvOnlySwitch( u8 byConfIdx, u32 dwRcvIp, u16 wRcvPort )
{
	//����IP��ַ���ҽ���MP���
	u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	
	//û�ҵ�
	if(byMpId==0)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "No MP's Addrs is 0x%x in RemoveRecvOnlySwitch(),please check switch recv IP!\n",dwRcvIp);
		return FALSE;
	}

	//���콻���ŵ� 
	TSwitchChannel tSwitchChannel;
    tSwitchChannel.SetRcvIP( dwRcvIp );
	tSwitchChannel.SetRcvPort( wRcvPort );
	tSwitchChannel.SetRcvBindIP( dwRcvIp );

	//�����ӽ�����Ϣ��MP
	CServMsg cServMsg;
	cServMsg.SetConfIdx( byConfIdx );
	cServMsg.SetDstDriId( byMpId );
	cServMsg.SetMsgBody((u8   *)&tSwitchChannel,sizeof(tSwitchChannel));
	g_cMpSsnApp.SendMsgToMpSsn( byMpId, MCU_MP_REMOVERECVONLYSWITCH_REQ, 
		                        cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

    return TRUE; 
}

/*=============================================================================
    �� �� ���� StartRecvMt
    ��    �ܣ� ��ʼ�����ն����ݣ�����ֻ���ս���������dump��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const TMt & tMt, Ҫ���յ��ն�
	           u8 byMode, ����ģʽ��ȱʡΪMODE_BOTH
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/03/02  3.6			����                  ����
=============================================================================*/
BOOL32 CMpManager::StartRecvMt( const TMt &tMt, u8 byMode )
{
	u32  dwSrcIp = 0;
	u32  dwRcvIp = 0;
	u16  wRcvPort = 0;
	BOOL32 bResult1 = TRUE;
	BOOL32 bResult2 = TRUE;
		
	//�õ�����Դ��ַ
	if( !GetSwitchInfo( tMt, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
		 LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartRecvMt() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//ͼ��
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		//����dump����
		bResult1 = AddRecvOnlySwitch( tMt.GetConfIdx(),dwSrcIp, dwRcvIp, wRcvPort );
		if(!bResult1)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Switch video dump failure!\n");
		}
	}
		
	//����
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		//����dump����,�����⳧���նˣ���Ҫ��rtcp��ip��port��֪mp
		CMcuVcInst * pcVcInst = g_cMcuVcApp.GetConfInstHandle(tMt.GetConfIdx());
		if(NULL == pcVcInst)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "[StartRecvMt]pcVcInst(ConfIdx.%d) is null!\n", tMt.GetConfIdx());
			return FALSE;
		}
		TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable(tMt.GetConfIdx());
		if (ptConfMtTable == NULL)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartRecvMt() GetConfMtTable failure, Type: %d SubType: %d Id: %d ConfIdx: %d!\n", 
				tMt.GetType(), tMt.GetEqpType(), tMt.GetEqpId(),tMt.GetConfIdx());
			return FALSE;
		}
		//tMt��localmt�����ÿ��Ƿ�local
		u32 dwRtcpBackIp = 0;
		u16 wRtcpBackPort = 0;
		if ( MT_MANU_KDC !=  ptConfMtTable->GetManuId(tMt.GetMtId()) && 
			MT_MANU_KDCMCU != ptConfMtTable->GetManuId(tMt.GetMtId()) )
		{
			//�⳧���ն�
			TLogicalChannel tLogicChannel;
			if (ptConfMtTable->GetMtLogicChnnl(tMt.GetMtId(),LOGCHL_AUDIO,&tLogicChannel,FALSE))
			{
				dwRtcpBackIp = tLogicChannel.GetSndMediaCtrlChannel().GetIpAddr();
				wRtcpBackPort = tLogicChannel.GetSndMediaCtrlChannel().GetPort();
			}
			
			
		}
		StaticLog("dwRtcpBackIp:%d,wRtcpBackPort:%d\n",dwRtcpBackIp,wRtcpBackPort);
		bResult2 = AddRecvOnlySwitch( tMt.GetConfIdx(), dwSrcIp,dwRcvIp, wRcvPort+2 ,dwRtcpBackIp,wRtcpBackPort);
		if(!bResult2)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Switch audio dump failure!\n");
		}
	}	
	
	return bResult1&&bResult2;
}

/*=============================================================================
    �� �� ���� StopRecvMt
    ��    �ܣ� ֹͣ�����ն����ݣ����ֻ���ս�����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const TMt & tMt, Ҫ���յ��ն�
	           u8 byMode, ����ģʽ��ȱʡΪMODE_BOTH
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/03/02  3.6			����                  ����
=============================================================================*/
BOOL32 CMpManager::StopRecvMt( const TMt & tMt, u8 byMode )
{
	u32  dwSrcIp = 0;
	u32  dwRcvIp = 0;
	u16  wRcvPort = 0;
	BOOL32 bResult1 = TRUE;
	BOOL32 bResult2 = TRUE;
		
	//�õ�����Դ��ַ
	if( !GetSwitchInfo( tMt, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
		 LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StopRecvMt() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//ͼ��
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		//���dump����
		bResult1 = RemoveRecvOnlySwitch( tMt.GetConfIdx(), dwRcvIp, wRcvPort );
		if(!bResult1)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Stop switch video dump failure!\n");
		}
	}
		
	//����
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		//���dump����
		bResult2 = RemoveRecvOnlySwitch( tMt.GetConfIdx(), dwRcvIp, wRcvPort+2 );
		if(!bResult2)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Stop switch audio dump failure!\n");
		}
	}

	return bResult1&&bResult2;
}



/*=============================================================================
    �� �� ���� SetRecvMtSSRCValue
    ��    �ܣ� ��mp����ssrc�������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8   byConfIdx ����������������
               u32  dwRcvIp   ����IP
               u16  wRcvPort  ���ն˿�
			   BOOL32 bReset    �Ƿ����SSRC�����FALSE-�ָ�ԭ��ֵ
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/03/06  3.6			����                  ����
=============================================================================*/
/*
//zbq[07/18/2007] SSRC ���޸��ɹ�һ����ͳһ����
BOOL32 CMpManager::SetRecvMtSSRCValue( u8 byConfIdx, u32 dwRcvIp, u16 wRcvPort, BOOL32 bReset )
{
	//����IP��ַ���ҽ���MP���
	u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	
	//û�ҵ�
	if(byMpId==0)
	{
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "No MP's Addrs is 0x%x in SetRecvMtSSRCValue(),please check switch recv IP!\n",dwRcvIp);
		return FALSE;
	}

	//����SSRC���ֵ 1�����������ָ�ԭ��SSRC Value
	u8 byChangeSSRC = 0;
	if( TRUE == bReset )
	{
		byChangeSSRC = 1;
	}

	//���콻���ŵ� 
	TSwitchChannel tSwitchChannel;
    tSwitchChannel.SetRcvIP( dwRcvIp );
	tSwitchChannel.SetRcvPort( wRcvPort );
	tSwitchChannel.SetRcvBindIP( dwRcvIp );

	//�����ӽ�����Ϣ��MP
	CServMsg cServMsg;
	cServMsg.SetConfIdx( byConfIdx );
	cServMsg.SetDstDriId( byMpId );
	cServMsg.SetMsgBody( (u8 *)&tSwitchChannel, sizeof(tSwitchChannel) );
	cServMsg.CatMsgBody( &byChangeSSRC, sizeof(u8) );
	g_cMpSsnApp.SendMsgToMpSsn( byMpId, MCU_MP_SETRECVSWITCHSSRC_REQ, 
		                        cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

    return TRUE; 
}
*/

/*=============================================================================
    �� �� ���� ResetRecvMtSSRC
    ��    �ܣ� ���ö�ĳ·����������SSRC���Ķ�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const TMt & tMt, Ҫ���յ��ն�
	           u8 byMode, ����ģʽ��ȱʡΪMODE_BOTH
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/03/06  3.6			����                  ����
=============================================================================*/
/*
//zbq[07/18/2007] SSRC ���޸��ɹ�һ����ͳһ����
BOOL32 CMpManager::ResetRecvMtSSRC( const TMt &tMt, u8 byMode )
{
	u32  dwSrcIp  = 0;
	u32  dwRcvIp  = 0;
	u16  wRcvPort = 0;
	BOOL32 bResult1 = TRUE;
	BOOL32 bResult2 = TRUE;
		
	//�õ�����Դ��ַ
	if( !GetSwitchInfo( tMt, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
		 LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "ResetRecvMtSSRC() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//ͼ��
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		bResult1 = SetRecvMtSSRCValue( tMt.GetConfIdx(), dwRcvIp, wRcvPort, TRUE );
		if(!bResult1)
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "ResetRecvMtSSRC video switch failure!\n");
		}
	}
	//����
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		bResult2 = SetRecvMtSSRCValue( tMt.GetConfIdx(), dwRcvIp, wRcvPort+2, TRUE );
		if(!bResult2)
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "ResetRecvMtSSRC audio switch failure!\n");
		}
	}	
	return bResult1&&bResult2;
}*/

/*=============================================================================
    �� �� ���� RestoreRecvMtSSRC
    ��    �ܣ� ȡ����������SSRC���Ķ�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const TMt & tMt, Ҫ���յ��ն�
	           u8 byMode, ����ģʽ��ȱʡΪMODE_BOTH
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/03/06  3.6			����                  ����
=============================================================================*/
/*
//zbq[07/18/2007]
BOOL32 CMpManager::RestoreRecvMtSSRC( const TMt & tMt, u8 byMode )
{
	u32  dwSrcIp  = 0;
	u32  dwRcvIp  = 0;
	u16  wRcvPort = 0;
	BOOL32 bResult1 = TRUE;
	BOOL32 bResult2 = TRUE;
		
	//�õ�����Դ��ַ
	if( !GetSwitchInfo( tMt, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
		 LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "RestoreRecvMtSSRC() failure because of can't get switch info!\n");
		 return FALSE;
	}

	//ͼ��
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		bResult1 = SetRecvMtSSRCValue( tMt.GetConfIdx(), dwRcvIp, wRcvPort, FALSE );
		if(!bResult1)
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "RestoreRecvMtSSRC video switch failure!\n");
		}
	}
		
	//����
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		bResult2 = SetRecvMtSSRCValue( tMt.GetConfIdx(), dwRcvIp, wRcvPort+2, FALSE );
		if(!bResult2)
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "RestoreRecvMtSSRC audio switch failure!\n");
		}
	}

	return bResult1&&bResult2;
}*/

/*====================================================================
    ������      ��ProcMpToMcuMessage
    ����        ����������Mp����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����pcMsg ��Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/16    1.0         ������        ����
====================================================================*/
void CMpManager::ProcMpToMcuMessage(const CMessage * pcMsg)
{
    switch( pcMsg->event ) 
	{
	  case MP_MCU_REG_REQ:
		  ProcMpRegReq(pcMsg);
		  break;
	  case MCU_MP_DISCONNECTED_NOTIFY:
		  ProcMpDisconnect(pcMsg);
          break;
	  default:
		  break;
	}  
}

/*====================================================================
    ������      ��ProcMpRegReq
    ����        ������Mpע����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����pcMsg ��Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/17    1.0         ������        ����
====================================================================*/
void CMpManager::ProcMpRegReq(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);	
	TMp tMp = *(TMp*)cServMsg.GetMsgBody();

	//[pengguofeng 5/10/2012]IPV6 
	u8 byIpType = IP_NONE;
	if ( tMp.GetVersion() == DEVVER_MPV6 )
	{
		u8 abyIpV6[IPV6_STR_LEN] = {0};
		u8 abyIpV6Null[IPV6_STR_LEN];
		memset(abyIpV6Null, 0, sizeof(abyIpV6Null));
		//��ȡ��ô�����ݣ�����Ϊ��Ӧ��ʱ��У������ݳ�����
		memcpy(abyIpV6, cServMsg.GetMsgBody() + sizeof(TMp), sizeof(abyIpV6));
		
		if ( memcmp(abyIpV6, abyIpV6Null, sizeof(abyIpV6)) != 0 )
		{
			if ( tMp.GetIpAddr() != 0)
			{
				byIpType = IP_V4V6;
			}
			else
				byIpType = IP_V6;
		}
		else if ( tMp.GetIpAddr() != 0)
		{
			byIpType = IP_V4;
		}
	}

	//[pengguofeng 5/12/2012]�ϰ汾ֱ�����V4
	if ( tMp.GetVersion() == DEVVER_MP )
	{
		byIpType = IP_V4;
	}

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[MpMcuReg]version:%d iptype:%d\n", tMp.GetVersion(), byIpType);
    //multicast
//    u8 byNetNo = g_cMcuAgent.GetCriDriNetChoice(tMp.GetMpId());
    u8 byIsMulticast = g_cMcuAgent.GetCriDriIsCast(tMp.GetMpId()) ? 1:0;
//    tMp.SetNetNo(byNetNo);
    tMp.SetMulticast(byIsMulticast);

    g_cMcuVcApp.AddMp(tMp);

	g_cMcuVcApp.SetMpIpType(tMp.GetMpId(), byIpType);

	g_cMcuVcApp.BroadcastToAllConf(MP_MCU_REG_REQ, pcMsg->content, pcMsg->length);
}

/*====================================================================
    ������      ��ProcMpDisconnect
    ����        ������Mp������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����pcMsg ��Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/17    1.0         ������        ����
====================================================================*/
void CMpManager::ProcMpDisconnect(const CMessage * pcMsg)
{
    CServMsg cServMsg( pcMsg->content ,pcMsg->length );	
	TMp tMp = *(TMp*)cServMsg.GetMsgBody();
	g_cMcuVcApp.RemoveMp( tMp.GetMpId() );
	
	//zhouyiliang 20130403 ����ת���屸��
//	if( !g_cMSSsnApp.IsDoubleLink() )
//	{
	g_cMcuVcApp.AssignNewMpToReplace(tMp.GetMpId());
//	}
	g_cMcuVcApp.BroadcastToAllConf( MCU_MP_DISCONNECTED_NOTIFY, pcMsg->content, pcMsg->length );


}

/*====================================================================
    ������      ��ProcMtAdpToMcuMessage
    ����        ����������Mtadp����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����pcMsg ��Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/16    3.0         ������        ����
====================================================================*/
void CMpManager::ProcMtAdpToMcuMessage(const CMessage * pcMsg)
{
    switch( pcMsg->event ) 
	{
	  case MTADP_MCU_REGISTER_REQ:
		  ProcMtAdpRegReq(pcMsg);
		  break;
	  case MCU_MTADP_DISCONNECTED_NOTIFY:
		  ProcMtAdpDisconnect(pcMsg);
		  break;
	  default:
		  break;
	}  
}


/*====================================================================
    ������      ��ProcMtAdpRegReq
    ����        ������MtAdpע����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����pcMsg ��Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/17    1.0         ������        ����
====================================================================*/
void CMpManager::ProcMtAdpRegReq(const CMessage * pcMsg)
{
	CServMsg cServMsg( pcMsg->content ,pcMsg->length );	
	TMtAdpReg tMtAdpReg =  *( TMtAdpReg* )cServMsg.GetMsgBody();

	for( u8 byLoop = MIN_CONFIDX; byLoop <= MAX_CONFIDX; byLoop++ )
	{			
		TConfMapData tTempConfMapData = g_cMcuVcApp.GetConfMapData(byLoop);
		if( !tTempConfMapData.IsValidConf() )
		{
			continue;
		}

		CMcuVcInst* pcVcInst = NULL;
		pcVcInst = g_cMcuVcApp.GetConfInstHandle( byLoop );
		if( pcVcInst == NULL )
		{
			continue;
		}

		for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
		{
			//���������ն�
			if ( !pcVcInst->m_tConfAllMtInfo.MtInConf(byMtId) )
			{
				continue;
			}
			if (NULL == pcVcInst->m_ptMtTable)
			{
				continue;
			}
			if (pcVcInst->m_ptMtTable->GetDisconnectDRI(byMtId) == tMtAdpReg.GetDriId())
			{
				pcVcInst->m_ptMtTable->SetDisconnectDRI(byMtId, 0);
			}
		}
	}

	//ͬ����ʱ��ע��MCU������ʵ�� ע����Ϣ ���������
	if( g_cMcuAgent.GetGkIpAddr()  && 0 != g_cMcuVcApp.GetRegGKDriId() && 
		tMtAdpReg.GetDriId() != g_cMcuVcApp.GetRegGKDriId() && 
		PROTOCOL_TYPE_H323 == tMtAdpReg.GetProtocolType() )
	{
		u8 byConfIdx  = 0; //0��mcu��1��MAXNUM_MCU_CONF ����
		u8 byRegState = 0; //0��δע�ᣬ1���ɹ�ע��
		TMtAlias tMtAlias;
		char achMcuAlias[MAXLEN_ALIAS];
		g_cMcuAgent.GetE164Number( achMcuAlias, MAXLEN_ALIAS );

		byRegState = g_cMcuVcApp.GetConfRegState( byConfIdx );	
		tMtAlias.SetE164Alias( achMcuAlias );
		cServMsg.SetMcuId( LOCAL_MCUID );
		//cServMsg.SetConfIdx( 0 );
		cServMsg.SetMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
		cServMsg.CatMsgBody( (u8*)&byConfIdx, sizeof( byConfIdx ) );
		cServMsg.CatMsgBody( (u8*)&byRegState, sizeof( byRegState ) );

		for( byConfIdx=MIN_CONFIDX; byConfIdx<=MAX_CONFIDX; byConfIdx++ )
		{			
            TConfInfo* ptConfInfo = NULL;
            TTemplateInfo tTemInfo;
            
            TConfMapData tMapData = g_cMcuVcApp.GetConfMapData(byConfIdx);
            if(tMapData.IsValidConf())
            {
				CMcuVcInst* pcVcInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
	
                if( NULL == pcVcInst )
                {
                    continue;
                }
                else
                {
                    ptConfInfo = &pcVcInst->m_tConf;
                }
            }
            else if(tMapData.IsTemUsed())
            {
                if(!g_cMcuVcApp.GetTemplate(byConfIdx, tTemInfo))
                {
                    continue;
                }
                else
                {
                    ptConfInfo = &tTemInfo.m_tConfInfo;
                }
            }
            else
            {
                continue;
            }
				
			byRegState = g_cMcuVcApp.GetConfRegState( byConfIdx );
			tMtAlias.SetE164Alias( ptConfInfo->GetConfE164() );
			cServMsg.SetConfId( ptConfInfo->GetConfId() );
			//cServMsg.SetConfIdx( byConfIdx );
			cServMsg.CatMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
			cServMsg.CatMsgBody( (u8*)&byConfIdx, sizeof( byConfIdx ) );
			cServMsg.CatMsgBody( (u8*)&byRegState, sizeof( byRegState ) );
		}
	
		u8 byNewDri = tMtAdpReg.GetDriId();
		cServMsg.SetDstDriId( byNewDri );
		g_cMtAdpSsnApp.SendMsgToMtAdpSsn( byNewDri, MCU_MT_UPDATE_REGGKSTATUS_NTF, cServMsg );

		cServMsg.SetMsgBody( (u8*)g_cMcuVcApp.GetH323GKIDAlias(), sizeof( g_cMcuVcApp.m_tGKID ) );
		cServMsg.CatMsgBody( (u8*)g_cMcuVcApp.GetH323EPIDAlias(), sizeof( g_cMcuVcApp.m_tEPID ) );
		g_cMtAdpSsnApp.SendMsgToMtAdpSsn( byNewDri, MCU_MT_UPDATE_GKANDEPID_NTF, cServMsg );
	}

	return;
}
/*lint -restore*/
/*====================================================================
    ������      ��ProcMpDisconnect
    ����        ��mtadpssn osp������Ϣ�����е��õĴ���MtAdp��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����pcMsg ��Ϣ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/17    1.0         ������        ����
====================================================================*/
/*lint -save -e850*/
void CMpManager::ProcMtAdpDisconnect(const CMessage * pcMsg)
{
	CServMsg cServMsg( pcMsg->content ,pcMsg->length );	
	TMtAdpReg tMtAdpReg =  *( TMtAdpReg* )cServMsg.GetMsgBody();
	u8 byRRQReset = *( cServMsg.GetMsgBody() + sizeof(TMtAdpReg) );
	g_cMcuVcApp.BroadcastToAllConf( MCU_MTADP_DISCONNECTED_NOTIFY, pcMsg->content, pcMsg->length );

	if( 1 == byRRQReset )
	{
		u8 byMinDri   = 0;
		u8 byConfIdx  = 0; //0��mcu��1��MAXNUM_MCU_CONF ����
		u8 byRegState = 0; //0��δע�ᣬ1���ɹ�ע��
		TMtAlias tMtAlias;
		char achMcuAlias[MAXLEN_ALIAS];
		g_cMcuAgent.GetE164Number( achMcuAlias, MAXLEN_ALIAS );

		//��ջ���ע��״̬
		g_cMcuVcApp.SetConfRegState( byConfIdx, byRegState );
		
		tMtAlias.SetE164Alias( achMcuAlias );
		cServMsg.SetMcuId( LOCAL_MCUID );
		//cServMsg.SetConfIdx( 0 );
		cServMsg.SetMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
		cServMsg.CatMsgBody( (u8*)&byConfIdx, sizeof( byConfIdx ) );
		cServMsg.CatMsgBody( (u8*)&byRegState, sizeof( byRegState ) );
        
        for( byConfIdx=MIN_CONFIDX; byConfIdx<=MAX_CONFIDX; byConfIdx++ )
        {			
            TConfInfo* ptConfInfo = NULL;
            TTemplateInfo tTemInfo;

            g_cMcuVcApp.SetConfRegState( byConfIdx, byRegState );
            
            TConfMapData tMapData = g_cMcuVcApp.GetConfMapData(byConfIdx);
            if(tMapData.IsValidConf())
            {
				CMcuVcInst* pcVcInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
                if( NULL == pcVcInst )
                {
                    continue;
                }
                else
                {
                    ptConfInfo = &pcVcInst->m_tConf;

                    // ����GK�Ʒѻ����ر���, zgc, 2008-09-26
                    // �ȴ��������ע�����������¿�ʼע��Ʒѻ���
					if(ptConfInfo)
						ptConfInfo->m_tStatus.SetGkCharge( FALSE );
                }
            }
            else if(tMapData.IsTemUsed())
            {
                if(!g_cMcuVcApp.GetTemplate(byConfIdx, tTemInfo))
                {
                    continue;
                }
                else
                {
                    ptConfInfo = &tTemInfo.m_tConfInfo;
                }
            }
            else
            {
                continue;
            }

            tMtAlias.SetE164Alias( ptConfInfo->GetConfE164() );
            cServMsg.SetConfId( ptConfInfo->GetConfId() );
            //cServMsg.SetConfIdx( byConfIdx );
            cServMsg.CatMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
            cServMsg.CatMsgBody( (u8*)&byConfIdx, sizeof( byConfIdx ) );
            cServMsg.CatMsgBody( (u8*)&byRegState, sizeof( byRegState ) );
        }	

		//�����������ģ���� ע��MCU������ʵ�� ע����Ϣ
		for( u8 byLoop = 1; byLoop <= MAXNUM_DRI; byLoop++)
		{
			if( g_cMcuVcApp.IsMtAdpConnected( byLoop ) && 
				PROTOCOL_TYPE_H323 == g_cMcuVcApp.GetMtAdpProtocalType(byLoop) )
			{
				cServMsg.SetDstDriId( byLoop );
				g_cMtAdpSsnApp.SendMsgToMtAdpSsn( byLoop, MCU_MT_UPDATE_REGGKSTATUS_NTF, cServMsg );

				if( 0 == byMinDri )
				{
					byMinDri = byLoop;
				}
			}
		}
		//  [12/18/2009 pengjie] Modify
		if( 0 == byMinDri)
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[ProcMtAdpDisconnect] Don't Find another Idle Mtadp!\n");
			return;
		}
		// End Modify
		
		//  [12/26/2009 pengjie] αװע����ʱע��
		/*
		//����ע�������ģ�������������ע��Ǩ�ƣ�����ѡ������ģ��� ע��MCU������ʵ��
		cServMsg.SetDstDriId( byMinDri );
		if( 0 != byMinDri && byMinDri <= MAXNUM_DRI )
		{
            //1��ˢ���������ַ��ע��mtadp��״̬ˢ�·�����һ����αװע������ɣ�
            u32 dwOrgRRQIp = g_cMcuAgent.GetRRQMtadpIp();
            u32 dwReplaceRRQIp = g_cMcuVcApp.GetMtAdpIpAddr(byMinDri);
            g_cMcuAgent.SetRRQMtadpIp(dwReplaceRRQIp);
            g_cMcuVcApp.SetRegGKDriId(byMinDri);
			
            //2��αװע��
            s8 achMcuE164[MAXLEN_ALIAS];
            memset(achMcuE164, 0, MAXLEN_ALIAS);
            g_cMcuAgent.GetE164Number(achMcuE164, MAXLEN_ALIAS);
			
            TRASInfo tRASInfo;
            tRASInfo.SetEPID(&g_cMcuVcApp.m_tEPID);
            tRASInfo.SetGKID(&g_cMcuVcApp.m_tGKID);
            tRASInfo.SetGKIp(ntohl(g_cMcuAgent.GetGkIpAddr()));
            tRASInfo.SetMcuE164(achMcuE164);
            tRASInfo.SetRRQIp(dwOrgRRQIp);            
			
            CServMsg cServMsg;
            cServMsg.SetConfIdx( 0 );
            cServMsg.SetEventId( MCU_MT_UNREGGK_NPLUS_CMD );
            cServMsg.SetMsgBody( (u8*)&tRASInfo, sizeof(TRASInfo) );
            
            g_cMtAdpSsnApp.SendMsgToMtAdpSsn( byMinDri, 
				MCU_MT_UNREGGK_NPLUS_CMD, cServMsg );
			
            //3������ע�ᣨע������ע���ڡ�αװע�����ڽ�������ɣ�
            
            //3.1 ��GK��ע��MCU
            g_cMcuVcApp.SetRegGKDriId( byMinDri );
            g_cMcuVcApp.RegisterConfToGK( 0, byMinDri );
            
            //3.2 ��GK��ע�����ʵ��(���������ģ��)
            for( u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++ )
            {
                //ע��ģ��
                TConfMapData tConfMapData = g_cMcuVcApp.GetConfMapData( byConfIdx );
                if ( tConfMapData.IsTemUsed() )
                {
                    g_cMcuVcApp.RegisterConfToGK( byConfIdx, byMinDri, TRUE, FALSE );
                }
                //ע�����
                else if ( tConfMapData.IsValidConf() )
                {
                    g_cMcuVcApp.RegisterConfToGK( byConfIdx, byMinDri, FALSE, FALSE );
                }
            }
			
            //4. �ñ�־λ �����غ��ն�
            g_cMcuVcApp.SetRRQDriTransed( TRUE );
		}
		*/
		//���GKID/EPID��Ϣ
        memset((void*)&g_cMcuVcApp.m_tGKID, 0, sizeof(TH323EPGKIDAlias));
        memset((void*)&g_cMcuVcApp.m_tEPID, 0, sizeof(TH323EPGKIDAlias));
	}
	else
	{
		if (g_cMcuVcApp.GetRegGKDriId() != 0)
		{
			g_cMcuVcApp.RegisterConfToGK( 0, g_cMcuVcApp.GetRegGKDriId() );
        }
	}
	
	return;
}
/*lint -restore*/

/*=============================================================================
    �� �� ���� UpdateRRQInfoAfterURQ
    ��    �ܣ� ע���Ļ���ע���¼�� ͬ�� ��������ģ���� ע��MCU������ʵ�� ע����Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byURQConfIdx
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/16   3.6			����                  ����
=============================================================================*/
/*lint -save -e850*/
void CMpManager::UpdateRRQInfoAfterURQ( u8 byURQConfIdx )
{
	if( g_cMcuAgent.GetGkIpAddr() && 0 != g_cMcuVcApp.GetRegGKDriId() )
	{
		CServMsg cServMsg;
		TMtAlias tMtAlias;
		u8 byConfIdx  = 0; //0��mcu��1��MAXNUM_MCU_CONF ����
		u8 byRegState = 0; //0��δע�ᣬ1���ɹ�ע��
		char achMcuAlias[MAXLEN_ALIAS];
		g_cMcuAgent.GetE164Number( achMcuAlias, MAXLEN_ALIAS );

		byRegState = g_cMcuVcApp.GetConfRegState( byConfIdx );
		tMtAlias.SetNull();
		tMtAlias.SetE164Alias( achMcuAlias );
		cServMsg.SetMcuId( LOCAL_MCUID );
		//cServMsg.SetConfIdx( 0 );
		cServMsg.SetMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
		cServMsg.CatMsgBody( (u8*)&byConfIdx, sizeof( byConfIdx ) );
		cServMsg.CatMsgBody( (u8*)&byRegState, sizeof( byRegState ) );

        for( byConfIdx=MIN_CONFIDX; byConfIdx<=MAX_CONFIDX; byConfIdx++ )
        {			
            TConfInfo* ptConfInfo = NULL;
            TTemplateInfo tTemInfo;
            
            TConfMapData tMapData = g_cMcuVcApp.GetConfMapData(byConfIdx);
            if(tMapData.IsValidConf())
            {
				CMcuVcInst* pcVcInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
                if( NULL == pcVcInst )
                {
                    continue;
                }
                else
                {
                    ptConfInfo = &pcVcInst->m_tConf;
                }
            }
            else if(tMapData.IsTemUsed())
            {
                if(!g_cMcuVcApp.GetTemplate(byConfIdx, tTemInfo))
                {
                    continue;
                }
                else
                {
                    ptConfInfo = &tTemInfo.m_tConfInfo;
                }
            }
            else
            {
                continue;
            }

            //���ע���Ļ���ע���¼
            if( byURQConfIdx == byConfIdx )
            {
                g_cMcuVcApp.SetConfRegState( byConfIdx, 0 );
                byRegState = 0;
                tMtAlias.SetNull();
            }
            else
            {
                byRegState = g_cMcuVcApp.GetConfRegState( byConfIdx );
                tMtAlias.SetE164Alias( ptConfInfo->GetConfE164() );
            }
            cServMsg.SetConfId( ptConfInfo->GetConfId() );
            //cServMsg.SetConfIdx( byConfIdx );
            cServMsg.CatMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
            cServMsg.CatMsgBody( (u8*)&byConfIdx, sizeof( byConfIdx ) );
            cServMsg.CatMsgBody( (u8*)&byRegState, sizeof( byRegState ) );
        }		
		
		//ע���Ļ���ע���¼�� ͬ�� ��������ģ���� ע��MCU������ʵ�� ע����Ϣ
		for( u8 byLoop = 1; byLoop <= MAXNUM_DRI; byLoop++)
		{
			if( byLoop != g_cMcuVcApp.GetRegGKDriId() &&
				g_cMcuVcApp.IsMtAdpConnected( byLoop ) && 
				PROTOCOL_TYPE_H323 == g_cMcuVcApp.GetMtAdpProtocalType(byLoop) )
			{
				cServMsg.SetDstDriId( byLoop );
				g_cMtAdpSsnApp.SendMsgToMtAdpSsn( byLoop, MCU_MT_UPDATE_REGGKSTATUS_NTF, cServMsg );
			}
		}
	}

	return;
}
/*lint -restore*/

/*====================================================================
    ������      ��ShowMp
    ����        ����Ļ��ӡ����Mp��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/17    1.0         ������        ����
====================================================================*/	
void CMpManager::ShowMp()
{
	s32  nMpSum = 0;
	u8 byLoop;
	s8 achMcuAlias[32];
	g_cMcuAgent.GetMcuAlias( achMcuAlias, 32 );
    StaticLog("\n------------------MCU %s register MP list----------------", achMcuAlias );
	for( byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		if(g_cMcuVcApp.m_atMpData[byLoop].m_bConnected)
		{
            StaticLog( "\nMp%d:[IpType:%d] %s (Band: %d/%d[Total/Real] P.K, NetTraffic: %d/%d/%d[Total/Real/Reserved] Mbps)", 
                                  byLoop+1, g_cMcuVcApp.m_atMpData[byLoop].GetIpType(),
				                  StrOfIP( g_cMcuVcApp.m_atMpData[byLoop].m_tMp.GetIpAddr( ) ) ,
                                  g_cMcuVcApp.m_atMpData[byLoop].m_dwPkNumAllowed,
                                  g_cMcuVcApp.m_atMpData[byLoop].m_dwPkNumReal,
                                  g_cMcuVcApp.m_atMpData[byLoop].m_wNetBandAllowed,
                                  g_cMcuVcApp.m_atMpData[byLoop].m_wNetBandReal,
                                  g_cMcuVcApp.m_atMpData[byLoop].m_wNetBandReserved
                      );
			nMpSum++;
		}		
	}
	StaticLog("\nTotal Register MP num: %d \n",nMpSum);
}

/*====================================================================
    ������      ��ShowMtAdp
    ����        ����Ļ��ӡ����MtAdp��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	04/06/01    3.0         ������        ����
====================================================================*/	
void CMpManager::ShowMtAdp()
{
	s32  nMtAdpSum = 0;
	u8 byLoop;
	s8 achMcuAlias[32];
	g_cMcuAgent.GetMcuAlias( achMcuAlias, 32 );
    u8 byMtNum = 0;
    u8 bySMcuNum = 0;
    StaticLog("\n------------------MCU %s register MtAdp list----------------", achMcuAlias );
	for( byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		if( g_cMcuVcApp.m_atMtAdpData[byLoop].m_bConnected )
		{
            byMtNum = 0;
            bySMcuNum = 0;
            g_cMcuVcApp.GetMtNumOnDri(byLoop+1, TRUE, byMtNum, bySMcuNum);
            StaticLog( "\nMtAdp%d:[IpType:%d] %s (MaxMT/SMcu/HDMt/AUDMt ||OnlineMt/SMcu/All: %d/%d/%d/%d||%d/%d/%d)", byLoop+1,
						g_cMcuVcApp.m_atMtAdpData[byLoop].m_byIpType,
				       StrOfIP( g_cMcuVcApp.GetMtAdpIpAddr( byLoop+1 ) ),
                       g_cMcuVcApp.m_atMtAdpData[byLoop].m_byMaxMtNum,
                       g_cMcuVcApp.m_atMtAdpData[byLoop].m_byMaxSMcuNum,
					   g_cMcuVcApp.m_atMtAdpData[byLoop].m_byMaxHDMtNum,
					   g_cMcuVcApp.m_atMtAdpData[byLoop].m_wMaxAudMtNum,
                       byMtNum,
                       bySMcuNum,
                       g_cMcuVcApp.m_atMtAdpData[byLoop].m_wMtNum);
            
			nMtAdpSum++;
		}		
	}
	StaticLog( "\n Current HD Access Point Num : %d \n Total HD Access Point Num :%d \n Total Register MtAdp num: %d",
		g_cMcuVcApp.m_wCurrentHDMtNum, g_cMcuVcApp.GetMpcHDAccessNum(), nMtAdpSum);

	StaticLog( "\n Current Aud Access Point Num : %d \n Total Aud Access Point Num :%d!\n", 
				g_cMcuVcApp.m_wCurrentAudMtNum, g_cMcuVcApp.GetMpcCurrentAudMtAdpNum());
}


/*====================================================================
    ������      ��ShowBridge
    ����        ����Ļ��ӡ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/17    1.0         ������        ����
====================================================================*/	
void CMpManager::ShowBridge()
{
	TConfInfo  *ptConfFullInfo = NULL;
	TConfSwitchTable *ptSwitchTable;
	u8  byMpId, byDstMpId, byLoop1;
    u16 wLoop2;
	u32 dwSrcIp, dwDisIp, dwRcvIp, dwDstIp;
	u16 wRcvPort, wDstPort;
	TSwitchChannel *ptSwitchChannel;
	s8  achTemp[255];
	s32 nLen;

	for( u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++ )
	{ 
		//���η���ÿ�������״̬֪ͨ
		CMcuVcInst* pcVcInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
        if(NULL == pcVcInst)
        {
            continue;
        }

        ptConfFullInfo = &pcVcInst->m_tConf;
		if( ptConfFullInfo && ptConfFullInfo->m_tStatus.IsOngoing() )
		{   
			StaticLog("\n--------Conf: %s bridge info--------\n", ptConfFullInfo->GetConfName() );
		    ptSwitchTable = g_cMcuVcApp.GetConfSwitchTable( byConfIdx );
			for( byLoop1 = 1; byLoop1 <= MAXNUM_DRI; byLoop1++ )
			{
				if( g_cMcuVcApp.IsMpConnected( byLoop1 ) )
				{
					StaticLog("\nMp%d switch info: ", byLoop1 );
					for( wLoop2 = 0; wLoop2 < MAX_SWITCH_CHANNEL; wLoop2++ )
					{
						ptSwitchChannel = ptSwitchTable->GetSwitchChannel( byLoop1, wLoop2 );
						if( !ptSwitchChannel->IsNull() && ptSwitchChannel->GetRcvPort() == ptSwitchChannel->GetDstPort() )
						{
							dwDstIp = ptSwitchChannel->GetDstIP();
							byDstMpId = g_cMcuVcApp.FindMp( dwDstIp );
                            if( byDstMpId > 0 )
							{							
								dwSrcIp = ptSwitchChannel->GetSrcIp();
								dwDisIp = ptSwitchChannel->GetDisIp();
								dwRcvIp = ptSwitchChannel->GetRcvIP();
								wRcvPort = ptSwitchChannel->GetRcvPort();
								wDstPort = ptSwitchChannel->GetDstPort();

								memset( achTemp, 0, 255 );
							    nLen = 0;

								//SrcIp
								byMpId = g_cMcuVcApp.FindMp( dwSrcIp );
								if( byMpId > 0 )
								{
									nLen = sprintf( achTemp, "\n%d  %s:%d(mp%d)", wLoop2, StrOfIP( dwSrcIp ), wRcvPort, byMpId );
								}
								else
								{
									nLen = sprintf( achTemp, "\n%d  %s:%d(src)", wLoop2, StrOfIP( dwSrcIp ), wRcvPort );
								}					
								
								//DisIp
								if( dwDisIp > 0 )
								{
									byMpId = g_cMcuVcApp.FindMp( dwDisIp );
									if( byMpId > 0 )
									{
										nLen += sprintf( achTemp+nLen, "-->%s:%d(mp%d)", StrOfIP( dwDisIp ), wRcvPort, byMpId );
									}
									else
									{
										nLen += sprintf( achTemp+nLen, "-->%s:%d(src)", StrOfIP( dwDisIp ), wRcvPort );
									}
								}

								//RcvIp
								byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
								if( byMpId > 0 )
								{
									nLen += sprintf( achTemp+nLen, "-->%s:%d(mp%d)", StrOfIP( dwRcvIp ), wRcvPort, byMpId );
								}
								else
								{
									nLen += sprintf( achTemp+nLen, "-->%s:%d(dst)", StrOfIP( dwRcvIp ), wRcvPort );
								}
								
								//DstIp
								byMpId = g_cMcuVcApp.FindMp( dwDstIp );
								if( byMpId > 0 )
								{
									nLen += sprintf( achTemp+nLen, "-->%s:%d(mp%d)", StrOfIP( dwDstIp ), wDstPort, byMpId );
								}
								else
								{
									nLen += sprintf( achTemp+nLen, "-->%s:%d(dst)", StrOfIP( dwDstIp ), wDstPort );
								}	

								StaticLog(achTemp );
							}
						}
					}
				}
			}
		}
	}
}

/*====================================================================
    ������      ��ShowSwitchRouteToDst
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��BOOL32
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	2011/07/7	4.6			Ѧ��		  create
====================================================================*/
/*lint -save -e850*/
BOOL32 CMpManager::ShowSwitchRouteToDst(u8 byConfIdx, u32 dwDstIp, u16 wDstPort, BOOL32 bDetail)
{
	CMcuVcInst* pcVcInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
	if(NULL == pcVcInst)
	{
		//print
		return FALSE;
	}
	
	TConfInfo  *ptConfFullInfo = &pcVcInst->m_tConf;
	if(NULL == ptConfFullInfo)
	{
		//print
		return FALSE;
	}
	if( !ptConfFullInfo->m_tStatus.IsOngoing() )
	{
		//print;
		return FALSE;
	}
		
	TConfSwitchTable *ptSwitchTable = g_cMcuVcApp.GetConfSwitchTable( byConfIdx );
	if (NULL == ptSwitchTable)
	{
		//print
		return FALSE;
	}
	
	TAllMpSwInfo tAllMpSwinfo;
	u8 byLp = 0;
	BOOL32 bFound = FALSE;
	u32 dwDestIp = dwDstIp;
	u16 wDestPort = wDstPort;
	u32 dwSrcIp = 0;
	for (u8 byLoop1 = 1; byLoop1 <= MAXNUM_DRI; byLoop1++ )
	{
		if (bFound)
		{
			break;
		}

		if (!g_cMcuVcApp.IsMpConnected( byLoop1 ))
		{
			continue;
		}
		
		TSwitchChannel *ptSwitchChannel = NULL;
		for (u16 wLoop2 = 0; wLoop2 < MAX_SWITCH_CHANNEL; wLoop2++ )
		{
			ptSwitchChannel = ptSwitchTable->GetSwitchChannel( byLoop1, wLoop2 );
			if( NULL == ptSwitchChannel )
			{
				continue;
			}
			
			if (dwDestIp != ptSwitchChannel->GetDstIP() || wDestPort != ptSwitchChannel->GetDstPort())
			{
				continue;
			}
			
			bFound = TRUE;
			dwSrcIp = ptSwitchChannel->GetSrcIp();
			u32 dwDisIp = ptSwitchChannel->GetDisIp();
			u32 dwRcvIp = ptSwitchChannel->GetRcvIP();
			u16 wRcvPort = ptSwitchChannel->GetRcvPort();
			if ( dwSrcIp == 0xffffffff )
			{
				wRcvPort = ptSwitchTable->GetBrdSrcRcvPort();
            }
			
			tAllMpSwinfo.m_tSwInfo[byLp].m_byMpId = byLoop1;
			tAllMpSwinfo.m_tSwInfo[byLp].m_dwRcvIp = dwRcvIp;
			tAllMpSwinfo.m_tSwInfo[byLp].m_wRcvPort = wRcvPort;
			tAllMpSwinfo.m_tSwInfo[byLp].m_dwDstIp = dwDestIp;
			tAllMpSwinfo.m_tSwInfo[byLp].m_wDstPort = wDestPort;
			tAllMpSwinfo.m_UsedMPNum ++;
			byLp ++;

			//DisIp is 0 means that the switch not include bridge.
			if (0 == dwDisIp)
			{
				break;
			}
			
			u8 byMidMpId = 0;
			byMidMpId = g_cMcuVcApp.FindMp( dwDisIp );
			if (0 < byMidMpId)
			{
				byLoop1 = byMidMpId - 1;
				dwDestIp = dwRcvIp;
				wDestPort = wRcvPort;
				bFound = FALSE;
				break;
			}
			else
			{
				StaticLog("unexpected result looking for midMP(%s)!\n", StrOfIP(dwDisIp));
				break;
			}
		}	
	}

	if (byLp == 0)
	{
		return FALSE;
	}

	//print the whole path from src to dst
	StaticLog("\n");
	StaticLog("\t+     +  ++++\n");
	StaticLog("\t++   ++  +  +\n");
	StaticLog("\t+ + + +  ++++\n");
	StaticLog("\t+  +  +  +\n");
	StaticLog("\t+  +  +  +   route to %s:%d\n", StrOfIP(dwDstIp), wDstPort);
	StaticLog("\n------------------------------\n");
	s8 achPrntBuf[1024] = {0};
	u16 wBufLen = 0;
	wBufLen = sprintf(achPrntBuf, "   [src] %-15s", StrOfIP(dwSrcIp));
	s16 wIdx = (s16)(byLp-1);
	for (; wIdx >= 0; wIdx --)
	{
		wBufLen += sprintf(achPrntBuf + wBufLen, "\n-->[mp%2u]%-15s:%d", 
			tAllMpSwinfo.m_tSwInfo[wIdx].m_byMpId,
			StrOfIP(tAllMpSwinfo.m_tSwInfo[wIdx].m_dwRcvIp),
			tAllMpSwinfo.m_tSwInfo[wIdx].m_wRcvPort);
	}
	wBufLen += sprintf(achPrntBuf + wBufLen, "\n==>[dst] %-15s:%d\n", 
		StrOfIP(dwDstIp), wDstPort);
	StaticLog( achPrntBuf );

	StaticLog("------------------------------\nThe packet statistic info of mps:\n");

	// send msg to each mps
	if (!bDetail)
	{
		return TRUE;
	}
	CServMsg cServMsg;
	for (wIdx = (s16)(byLp-1); wIdx >= 0; wIdx --)
	{
		u8 byMpId = tAllMpSwinfo.m_tSwInfo[wIdx].m_byMpId;
		if( g_cMcuVcApp.IsMpConnected( byMpId ) )
		{
			TTransportAddr tRcvAddr;
			tRcvAddr.SetIpAddr(tAllMpSwinfo.m_tSwInfo[wIdx].m_dwRcvIp);
			tRcvAddr.SetPort(tAllMpSwinfo.m_tSwInfo[wIdx].m_wRcvPort);

			TTransportAddr tDstAddr;
			tDstAddr.SetIpAddr(dwDstIp);
			tDstAddr.SetPort(wDstPort);

			cServMsg.SetMsgBody();
			cServMsg.SetMsgBody((u8*)&tRcvAddr, sizeof(tRcvAddr));
			cServMsg.CatMsgBody((u8*)&tDstAddr, sizeof(tDstAddr));
			cServMsg.SetDstDriId(byMpId);
			
			g_cMpSsnApp.SendMsgToMpSsn(byMpId, MCU_MP_GETDSINFO_REQ, 
				cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		}
	}

	return TRUE;

}
/*lint -restore*/

/*====================================================================
    ������      ��ShowSwitch
    ����        ����Ļ��ӡ������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byType:           ֧�ֵĴ�ӡ����
                  0. ��ӡ�ţ�����ӡRTCP�ȣ�Ĭ�ϣ�
                  1. ����ӡ�ţ���ӡRTCP��
                  2. ��ӡ�ţ���ӡRTCP
                  3  ����ӡ�ţ�����ӡRTCP��      
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/17    1.0         ������        ����
====================================================================*/	
void CMpManager::ShowSwitch( u8 byType )
{
	TConfInfo  *ptConfFullInfo = NULL;
	TConfSwitchTable *ptSwitchTable = NULL;
	u8  byMpId, byLoop1;
    u16 wLoop2;
	u32 dwSrcIp, dwDisIp, dwRcvIp, dwDstIp;
	u16 wRcvPort, wDstPort;
	TSwitchChannel *ptSwitchChannel = NULL;
	s8  achTemp[255];
	s32 nLen, nLen0;

    u8 byMpcBrdId = g_cMcuAgent.GetMpcBoardId();

	BOOL32 bIsMiniMcu = FALSE;
#ifdef _MINIMCU_
	bIsMiniMcu = TRUE;
#endif
	
    // ��Ƶ�㲥Դ���ն˿�, zgc, 2008-07-23
    u16 wBrdSrcRcvPort = 0;
    u32 dwSwitchIp = 0;
    u32 dwSwitchSrcIp = 0;
    TMt tVidBrdSrc;
	for( u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++ )
	{ 
        if(NULL == g_cMcuVcApp.GetConfInstHandle( byConfIdx ))
        {
            continue;
        }

		CMcuVcInst* pcVcInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
		ptConfFullInfo = pcVcInst != NULL ? &pcVcInst->m_tConf : NULL;
		if(NULL == ptConfFullInfo || pcVcInst == NULL)
		{
			continue;
		}
		if( ptConfFullInfo->m_tStatus.IsOngoing() )
		{   
            // �����Ƶ�㲥Դ, zgc, 2008-07-23
            tVidBrdSrc = pcVcInst->GetVidBrdSrc();
            if ( !tVidBrdSrc.IsNull() )
            {
                GetSwitchInfo( tVidBrdSrc, dwSwitchIp, wBrdSrcRcvPort, dwSwitchSrcIp );
            }

			StaticLog("\n--------Conf: %s data switch info--------\n", ptConfFullInfo->GetConfName() );
		    ptSwitchTable = g_cMcuVcApp.GetConfSwitchTable( byConfIdx );
			if(NULL == ptSwitchTable)
			{
				continue;
			}
			for( byLoop1 = 1; byLoop1 <= MAXNUM_DRI; byLoop1++ )
			{				
                // guzh [3/29/2007] ֧�ִ�ӡ���⽻��������Ϊ1����2���ʹ�ӡ������Ϣ(�㲥Դ/Rtcp)
				if( g_cMcuVcApp.IsMpConnected( byLoop1 ) ||
                    ( ( byType == 1 || byType == 2)
						&& ( (bIsMiniMcu && byLoop1 == 1 && !g_cMcuVcApp.IsMpConnected( byLoop1 ))
						      ||
							 ( ( byLoop1 == MCU_BOARD_MPC || byLoop1 == MCU_BOARD_MPCD || byLoop1 == 8) && 
						/*( byLoop1 != byMpcBrdId && */!bIsMiniMcu /*)*/ ) ) 
					)
                   )
				{
					StaticLog("\nMp%d(%s) switch info: ", byLoop1 , StrOfIP(g_cMcuVcApp.GetMpIpAddr(byLoop1)));
					for( wLoop2 = 0; wLoop2 < MAX_SWITCH_CHANNEL; wLoop2++ )
					{
						ptSwitchChannel = ptSwitchTable->GetSwitchChannel( byLoop1, wLoop2 );
						if( NULL == ptSwitchChannel )
						{
							continue;
						}
						if( !ptSwitchChannel->IsNull() )
						{
                            // xsl [9/15/2006] ����Ϊ1������ӡ����Ϣ
                            if ( (byType == 1 || byType == 3) 
                                && ptSwitchChannel->GetRcvPort() == ptSwitchChannel->GetDstPort()
                                && g_cMcuVcApp.FindMp(ptSwitchChannel->GetDstIP()) > 0)
                            {
                                continue;
                            }

							dwSrcIp = ptSwitchChannel->GetSrcIp();
							dwDisIp = ptSwitchChannel->GetDisIp();
							dwRcvIp = ptSwitchChannel->GetRcvIP();
							dwDstIp = ptSwitchChannel->GetDstIP();
                            wRcvPort = ptSwitchChannel->GetRcvPort();
							wDstPort = ptSwitchChannel->GetDstPort();

							memset( achTemp, ' ', 255 );
							nLen = 0;

							//SrcIp

                            // BUG4123: ��Ƶ������δ��ʾ������ĵ�ǰʵ��ʹ�õĶ˿�
                            // zgc, 2008-07-23
                            if ( dwSrcIp == 0xffffffff )
                            {
                                wRcvPort = ptSwitchTable->GetBrdSrcRcvPort();//wBrdSrcRcvPort;
                            }

							byMpId = g_cMcuVcApp.FindMp( dwSrcIp );
							if( byMpId > 0 )
							{
								nLen = sprintf( achTemp, "\n%d  %s:%d(mp%d)", wLoop2, StrOfIP( dwSrcIp ), wRcvPort, byMpId );
							}
							else
							{
								nLen = sprintf( achTemp, "\n%d  %s:%d(src)", wLoop2, StrOfIP( dwSrcIp ), wRcvPort );
							}
							
							nLen0 = nLen;
							
							//DisIp
							memset( achTemp+nLen, ' ', 255-nLen );
							if( dwDisIp > 0 )
							{
								byMpId = g_cMcuVcApp.FindMp( dwDisIp );
								if( byMpId > 0 )
								{
									nLen += sprintf( achTemp+nLen, "-->%s:%d(mp%d)", StrOfIP( dwDisIp ), wRcvPort, byMpId );
								}
								else
								{
									nLen += sprintf( achTemp+nLen, "-->%s:%d(src)", StrOfIP( dwDisIp ), wRcvPort );
								}
								StaticLog("%s\n", achTemp );
								nLen = nLen0-1;
								memset( achTemp, ' ', 255 );
							}

							//RcvIp
							memset( achTemp+nLen, ' ', 255-nLen );
							byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
							if( byMpId > 0 )
							{
								nLen += sprintf( achTemp+nLen, "-->%s:%d(mp%d)", StrOfIP( dwRcvIp ), wRcvPort, byMpId );
							}
							else
							{
								nLen += sprintf( achTemp+nLen, "-->%s:%d(dst)", StrOfIP( dwRcvIp ), wRcvPort );
							}
							
							//DstIp
							memset( achTemp+nLen, ' ', 255-nLen );
							byMpId = g_cMcuVcApp.FindMp( dwDstIp );

                            // BUG4123: ��Ƶ������δ��ʾ������ĵ�ǰʵ��ʹ�õĶ˿�
                            // zgc, 2008-07-17
                            if ( dwDstIp == 0xffffffff )
                            {
                                wDstPort = ptSwitchTable->GetBrdSrcRcvPort();//wBrdSrcRcvPort;
                            }

							if( byMpId > 0 )
							{
								nLen += sprintf( achTemp+nLen, "-->%s:%d(mp%d)", StrOfIP( dwDstIp ), wDstPort, byMpId );
							}
							else
							{
								nLen += sprintf( achTemp+nLen, "-->%s:%d(dst)", StrOfIP( dwDstIp ), wDstPort );
							}

							StaticLog( achTemp );
						
						}
					}
					StaticLog( "\n" );
				}
			}
			
		}
	}
	
}

/*=============================================================================
    �� �� ���� GetMPSwitchInfo
    ��    �ܣ� ��ȡָ��mp���ϵĽ�����Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byMpId
               TSwitchTable &tSwitchTable
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/01/25  3.5			����                  ����
=============================================================================*/
void CMpManager::GetMPSwitchInfo( u8 byMpId, TSwitchTable &tSwitchTable )
{
	if( 0 == byMpId || byMpId > MAXNUM_DRI )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[GetMPSwitchInfo] Err: byMpId(%d) is invalid \n", byMpId );
		return;
	}
	if( FALSE == g_cMcuVcApp.IsMpConnected( byMpId ) )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[GetMPSwitchInfo] Err: byMpId(%d) is not connect \n", byMpId );
		return;
	}

	u16	wSwChIdx = 0;
	TConfInfo  *ptConfFullInfo = NULL;
	TSwitchChannel *ptSwitchChannel = NULL;
	TConfSwitchTable *ptSwitchTable = NULL;

	memset(&tSwitchTable, 0, sizeof(tSwitchTable));
	for( u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++ )
	{ 
        if(NULL == g_cMcuVcApp.GetConfInstHandle( byConfIdx ))
        {
            continue;
        }
        
		CMcuVcInst* pcVcInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
		ptConfFullInfo = pcVcInst != NULL ? &pcVcInst->m_tConf : NULL;
        
		if( ptConfFullInfo && ptConfFullInfo->m_tStatus.IsOngoing() )
		{   
		    ptSwitchTable = g_cMcuVcApp.GetConfSwitchTable( byConfIdx );

			for( u16 wChannelIdx = 0; wChannelIdx < MAX_SWITCH_CHANNEL; wChannelIdx++ )
			{
				ptSwitchChannel = ptSwitchTable->GetSwitchChannel( byMpId, wChannelIdx );
				if( FALSE == ptSwitchChannel->IsNull() )
				{
					tSwitchTable.m_atSwitchChannel[wSwChIdx] = *ptSwitchChannel;
					wSwChIdx++;
					if( wSwChIdx >= MAX_SWITCH_CHANNEL )
					{
						break;
					}
				}
			}
		}
	}

	return;
}

///////////////////////////////////////////////////////
//// ��ָ���ն����ݽ������ϼ�mcu��������ش�֧�֣� ////
///////////////////////////////////////////////////////
BOOL32 CMpManager::StartSwitchToMmcu( const TMt & tSrc, 
                                     u16        wSrcChnnl, 
                                     const TMt & tDst, 
                                     u8        byMode,
                                     u8        bySwitchMode, 
                                     BOOL32 bH239Chnnl,
									 BOOL32 bStopBeforeStart,
									 BOOL32 bSrcHDBas,
									 u16 wSpyPort)
{
	u32  dwSrcIp = 0;
	u32  dwDisIp = 0;
	u32  dwRcvIp = 0;
	u16  wRcvPort = 0;

	TLogicalChannel	tLogicalChannel;
	BOOL32 bResult1 = TRUE;
	BOOL32 bResult2 = TRUE;

	//�õ�����Դ��ַ
	if( !GetSwitchInfo( tSrc, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
		 LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchToSubMt() failure because of can't get switch info!\n");
		 return FALSE ;
	}
    
	CMcuVcInst* pcVcInst = g_cMcuVcApp.GetConfInstHandle( tSrc.GetConfIdx() );
	TConfInfo * ptConfInfo = pcVcInst != NULL ? &pcVcInst->m_tConf : NULL;
    
	u8 byIsSwitchRtcp;
	TConfAttrb tTempConfAttrb;
	if( ptConfInfo != NULL )
	{
		tTempConfAttrb = ptConfInfo->GetConfAttrb();
	}
		 
    if (  ( TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_VMP == tSrc.GetEqpType() && MT_TYPE_MT != tDst.GetMtType() &&
        ptConfInfo && tTempConfAttrb.IsResendLosePack()) ||
        tSrc == tDst) //  xsl [1/17/2006] (�⳧��mt��mcu)����Ƶ�����������Լ�ʱ������rtcp����
    {
        byIsSwitchRtcp = 0;
    }
    else
    {
        byIsSwitchRtcp = 1;
    }

	//ֻ�����ݲ���
	TMt tDstMt = tDst;
	if( tDstMt.IsNull() )
	{
        // ���� [4/30/2006] ֻ�ղ����Ĵ���������,���߼�ͨ���򿪺��תDump 
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StartSwitchToSubMt() tDstMt is NULL! It's send-only MT?\n");        
		return TRUE;
	}

	//�õ������ն˱�ָ��
	TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable( tSrc.GetConfIdx() );
	if( ptConfMtTable == NULL )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchToSubMt() failure because invalid ConfMtTable Pointer!\n");
		return FALSE;
	}

    u8 bySwitchChannelMode;
    u8 byUniformPayload = INVALID_PAYLOAD;
    u8 byManuID = ptConfMtTable->GetManuId(tDst.GetMtId());

    if (byManuID == MT_MANU_KDC || byManuID == MT_MANU_KDCMCU)
	{
		bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_NONE;
	}
    else
    {
        bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_VALID;
        //Active Payload Uniform
        if (TYPE_MT == tSrc.GetType() && TYPE_MT == tDst.GetType())
        {
            u8 byChannelType = 0;
            TLogicalChannel  tChannel;
            memset(&tChannel, 0, sizeof(TLogicalChannel));
            //Get channal info
            if (0 == wSrcChnnl)
            {
                byChannelType = LOGCHL_VIDEO;
            }
            else if (1 == wSrcChnnl)
            {
                byChannelType = LOGCHL_SECVIDEO;
            }

            if (ptConfMtTable->GetMtLogicChnnl(tDst.GetMtId(), byChannelType, &tChannel, TRUE))
            {
                if (tChannel.GetChannelType() >= AP_MIN && tChannel.GetChannelType() <= AP_MAX)
                {
                    byUniformPayload = tChannel.GetChannelType();
                }
            }
        }
    }

	//�㲥������ʽ
	if(bySwitchMode==SWITCH_MODE_BROADCAST)
	{
		//�õ�Ŀ��Mt�Ľ�����Ϣ
		u32  dwMpIP = 0;
		dwMpIP = g_cMcuVcApp.GetMpIpAddr( ptConfMtTable->GetMpId( tDst.GetMtId() ) );
#if !defined(_8KH_) && !defined(_8KE_) && !defined(_8KI_)
		if( dwMpIP != dwRcvIp )//��Ҫת��
		{
			dwDisIp = dwRcvIp;
			dwRcvIp = dwMpIP;
		}
#endif
	}

	//�����ŵ��ż���ƫ����
	//wRcvPort = wRcvPort + PORTSPAN * wSrcChnnl;    //˫��Ҫ����
	if( wSrcChnnl >= CASCADE_SPY_STARTPORT && wSrcChnnl < MT_MCU_STARTPORT ) //�ö�ش���ͨ��
	{
		wRcvPort = wSrcChnnl;
	}
	else
	{
		wRcvPort = wRcvPort + PORTSPAN * wSrcChnnl;
	}

	//ͼ��
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		//��������
		if( ptConfMtTable->GetMtLogicChnnl( tDst.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, TRUE ) )	//���Խ���
		{
			//  [11/9/2009 pengjie] ������ش�֧��
			if( wSpyPort != SPY_CHANNL_NULL )
			{

				tLogicalChannel.m_tRcvMediaChannel.SetPort( wSpyPort );
			}
			//  End

            bResult1 = StartSwitch( tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort, tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp, bStopBeforeStart,tDst.GetMtId());
			if(!bResult1)LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Switch video failure!\n");
		}
	}

    if (MODE_SECVIDEO == byMode)
    {
        //H.239��Ƶ�߼�ͨ��
        if (bH239Chnnl)
        {
            if (ptConfMtTable->GetMtLogicChnnl(tDst.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, TRUE))
            {
				//zbq[09/03/2008] HD Bas��Դ��˫�� �Ӷ˿ڻ�ַ+PORTSPAN ������
				if (bSrcHDBas)
				{
					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort/* + PORTSPAN*/,
										   tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp,bStopBeforeStart,tDst.GetMtId());					
				}
				else
				{
					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort+4,
										   tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp,bStopBeforeStart,tDst.GetMtId());					
				}
                if (!bResult1)
                {
                    LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Switch video h.239 failure!\n");
                }
            }
            return bResult1;
        }
    }

	//����
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		//��������
		if( ptConfMtTable->GetMtLogicChnnl(tDst.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, TRUE ) )	//���Խ���
		{
			//  [11/9/2009 pengjie] ������ش�֧��
			if( wSpyPort != SPY_CHANNL_NULL )
			{
				tLogicalChannel.m_tRcvMediaChannel.SetPort( wSpyPort + 2 );
			}
			// End

			bResult2 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, (wRcvPort+2),
                                   tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp,bStopBeforeStart,tDst.GetMtId());
			if(!bResult2)LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Switch audio failure!\n");
		}
	}	

	return  bResult1 && bResult2 ;
}

void CSwitchGrpAdpt::Reset()
{
	if ( 0 == m_nGrpNum || NULL == m_ppMtList)
	{
		return;
	}

	for (int nLoop = 0; nLoop < m_nGrpNum; nLoop++)
	{
		if (m_ppMtList[nLoop])
		{
			delete [] m_ppMtList[nLoop];
			m_ppMtList[nLoop] = NULL;
		}
	}

	delete []m_ppMtList;
	m_ppMtList = NULL;
}

BOOL32 CSwitchGrpAdpt::Convert( const s32 nGrpNum, TSwitchGrp* ptSwitchGrp )
{
	if ( 0 >= nGrpNum || ptSwitchGrp == NULL )
	{
		return FALSE;
	}

	Reset();

	m_nGrpNum = nGrpNum;
	if ( NULL == (m_ppMtList = new TMt*[(u32)m_nGrpNum] )) 
	{
		return FALSE;
	}

	s32 nLoop = 0;
	for ( nLoop = 0; nLoop < m_nGrpNum; nLoop++ )
	{
		if ( NULL == (m_ppMtList[nLoop] = new TMt[MAXNUM_CONF_MT] ) )
		{
			Reset();
			return FALSE;
		}
	}

	TMt* ptMtList = NULL;
	u8 byDstMtNum = 0;
	for ( nLoop = 0; nLoop < m_nGrpNum; nLoop++)
	{
		ptMtList = ptSwitchGrp[nLoop].GetDstMt();
		byDstMtNum = ptSwitchGrp[nLoop].GetDstMtNum();
		byDstMtNum = min(MAXNUM_CONF_MT, byDstMtNum);
		if (!ptMtList || 0 == byDstMtNum )
		{
			StaticLog("[CSwitchGrpAdpt] Err, ptMtList is null or byDstMtNum.%d\n", byDstMtNum);
			continue;
		}

		memcpy(m_ppMtList[nLoop], ptMtList, sizeof(TMt) * byDstMtNum);
		ptSwitchGrp[nLoop].SetDstMt(m_ppMtList[nLoop]);
	}

	return TRUE;
}

// END OF FILE
