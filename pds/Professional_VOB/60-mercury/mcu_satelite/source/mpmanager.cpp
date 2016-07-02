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
#include "mpmanager.h"
#include "mtadpssn.h"
#include "mpssn.h"

#if defined(_LINUX_)
#include "boardwrapper.h"
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
void ssw( u8 byType )
{
	pcMpManager->ShowSwitch( byType );

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
BOOL32 CMpManager::StartMulticast( const TMt & tSrc, u8 bySrcChnnl, u8 byMode, BOOL32 bConf )
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
		 MpManagerLog("StartMulticast() failure because of can't get switch info!\n");
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
	
	//�����ŵ��ż���ƫ����	
	wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;

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
                MpManagerLog("\n[StartMulticast]byIsMulticast = %d  wMpId+1 = %d\n\n", byIsMulticast, wMpId+1);
                if (1 != byIsMulticast)
                {
                    continue;
                }

                byMulticastFlag++;

                dwRcvIp = g_cMcuVcApp.GetMpIpAddr(wMpId+1);
                MpManagerLog("\n[StartMulticast]dwRcvIp = 0x%x\n\n", dwRcvIp);
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
                    MpManagerLog("Switch video failure!\n");
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
                    MpManagerLog("Switch audio failure!\n");
                }
            }
            
            //�ڶ�·��Ƶ
            if (byMode == MODE_SECVIDEO)
            {
                bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, (wRcvPort+4),
                                       dwMultiCastAddr, (dwMultiCastPort+4), dwRcvIp, dwRcvIp);
                if (!bResult1)
                {
                    MpManagerLog("Switch snd video failure!\n");
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
		 MpManagerLog("StopMulticast() failure because of can't get switch info!\n");
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
		 MpManagerLog("StartDistrConfCast() failure because of can't get switch info!\n");
		 return FALSE ;
	}

    CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(tSrc.GetConfIdx());
    if ( NULL == pVcInst )
    {
        // Fatal error!
        MpManagerLog("Cannot get VC Instance(conf idx: %d) while StartDistrConfCast!\n", 
                            tSrc.GetConfIdx());
        return FALSE;
    }

    dwMultiCastAddr = pVcInst->m_tConf.GetConfAttrb().GetSatDCastIp();
    wMultiCastPort = pVcInst->m_tConf.GetConfAttrb().GetSatDCastPort();
	
	// xsl [8/19/2006] ���Ƿ�ɢ���������Nģʽ�������鲥��ַ
    if (tSrc.GetType() == TYPE_MCUPERI && tSrc.GetEqpType() == EQP_TYPE_MIXER)
    {
        //wRcvPort = wRcvPort + MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId;    //���ﲻ���Ƕ������������������ʱ�ٿ���
    }
    else //�����ŵ��ż���ƫ����
    {
        wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;
    }	

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
                MpManagerLog("\n[StartDistrConfCast()]byIsMulticast = %d  wMpId+1 = %d\n\n", byIsMulticast, wMpId+1);
                if (1 != byIsMulticast)
                {
                    continue;
                }

                byMulticastFlag++;

                dwRcvIp = g_cMcuVcApp.GetMpIpAddr(wMpId+1);
                MpManagerLog("\n[StartDistrConfCast()]dwRcvIp = 0x%x\n\n", dwRcvIp);
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
                MpManagerLog("StartDistrConfCast() Switch video\n");
                bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort,
                                       dwMultiCastAddr, wMultiCastPort, dwRcvIp, dwRcvIp,
                                       SWITCHCHANNEL_UNIFORMMODE_NONE,
                                       INVALID_PAYLOAD,
                                       // ����RTCP����
                                       1);
                if (!bResult1)
                {
                    MpManagerLog("StartDistrConfCast() Switch video failure!\n");
                }
            }

            //����
            if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
            {
                //��������
                MpManagerLog("StartDistrConfCast() Switch audio\n");
                bResult2 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, (wRcvPort+2),
                                       dwMultiCastAddr, (wMultiCastPort+2), dwRcvIp, dwRcvIp,
                                       SWITCHCHANNEL_UNIFORMMODE_NONE,
                                       INVALID_PAYLOAD,
                                       // ����RTCP����
                                       1);
                if (!bResult2)
                {
                    MpManagerLog("StartDistrConfCast() Switch audio failure!\n");
                }
            }
            
            //�ڶ�·��Ƶ
            if (byMode == MODE_SECVIDEO)
            {
                MpManagerLog("StartDistrConfCast() Switch snd video\n");
                bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, (wRcvPort+4),
                                       dwMultiCastAddr, (wMultiCastPort+4), dwRcvIp, dwRcvIp,
                                       SWITCHCHANNEL_UNIFORMMODE_NONE,
                                       INVALID_PAYLOAD,
                                       // ����RTCP����
                                       1);
                if (!bResult1)
                {
                    MpManagerLog("StartDistrConfCast() Switch snd video failure!\n");
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
		 MpManagerLog("StopDistrConfCast() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//�õ�MCU���鲥��ַ
    CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(tSrc.GetConfIdx());
    if ( NULL == pVcInst )
    {
        // Fatal error!
        MpManagerLog("Cannot get VC Instance(conf idx: %d) while StopDistrConfCast!\n", 
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
        MpManagerLog("StopDistrConfCast() Switch video\n");
		StopSwitch(tSrc.GetConfIdx(), dwMultiCastAddr, wMultiCastPort, 1);
	}
		
	//����
	if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
	{
		//�Ƴ�����
        MpManagerLog("StopDistrConfCast() Switch audio\n");
		StopSwitch(tSrc.GetConfIdx(), dwMultiCastAddr, (wMultiCastPort+2), 1);
	}
    
    //�ڶ�·��Ƶ
    if (byMode == MODE_SECVIDEO)
    {
        MpManagerLog("StopDistrConfCast() Switch snd video\n");
        StopSwitch(tSrc.GetConfIdx(), dwMultiCastAddr, (wMultiCastPort+4), 1);
    }

	return TRUE;
}

/*====================================================================
    ������      ��StartSatConfCast
    ����        ����ʼ���ǻ����鲥����������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tSrc Դ�ն�
	              bySrcChnnl ͨ����
				  byMode ����ģʽ,ȱʡΪMODE_BOTH
    ����ֵ˵��  ��TRUE��������ܿ�ʼ�㲥����FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	11/11/09    4.6         �ű���        ����
====================================================================*/
BOOL32 CMpManager::StartSatConfCast(const TMt &tSrc, u8 byCastType, u8 byMode, u8 bySrcChnnl)
{
	if (MODE_BOTH == byMode)
	{
		return StartSatConfCast(tSrc, byCastType, MODE_AUDIO, bySrcChnnl) &
			   StartSatConfCast(tSrc, byCastType, MODE_VIDEO, bySrcChnnl);
	}

    u32  dwSrcIp  = 0;
	u32  dwRcvIp  = 0;
	u16  wRcvPort = 0;
	
	
	//�õ�����Դ��ַ
	if (!GetSwitchInfo(tSrc, dwRcvIp, wRcvPort, dwSrcIp))
	{
		MpManagerLog("StartSatConfCast() failure because of can't get switch info!\n");
		return FALSE ;
	}

	u16 wPortAdj = 0;
	
	switch (byMode)
	{
	case MODE_AUDIO:
		wPortAdj += 2;
		break;
	case MODE_VIDEO:
		break;
	case MODE_SECVIDEO:
		wPortAdj += 4;
		break;
	}

	u16 wMultiCastPort = g_cMcuVcApp.GetMcuMulticastDataPort() + wPortAdj;

	u32 dwMultiCastAddr = 0;
	if (CAST_FST == byCastType)
	{
		//��һ�鲥��
		dwMultiCastAddr = g_cMcuVcApp.GetMcuMulticastDataIpAddr();
	}
	else if (CAST_SEC == byCastType)
	{
		//�ڶ��鲥��
		dwMultiCastAddr = g_cMcuVcApp.GetMcuSecMulticastIpAddr();

		//�����polycom��֧��˫�����˴������һ·��Ƶ�鲥��ַ��ͣ˫��ʱ�ָ�;
		if (MODE_SECVIDEO == byMode)
		{
			wMultiCastPort -= wPortAdj;
		}
		if (MODE_SECVIDEO == byMode &&
			tSrc.GetType() == TYPE_MCUPERI &&
			tSrc.GetEqpType() == EQP_TYPE_BAS)
		{
			wPortAdj -= 4;
		}
	}
	
	wRcvPort = wRcvPort + bySrcChnnl * PORTSPAN + wPortAdj;

	BOOL32 bRet = StartSwitch(tSrc,
								tSrc.GetConfIdx(),
								dwSrcIp,
								0,
								dwRcvIp,
								wRcvPort,
								dwMultiCastAddr, wMultiCastPort,
								dwRcvIp, dwRcvIp,
								SWITCHCHANNEL_UNIFORMMODE_NONE,
								INVALID_PAYLOAD,
								// ����RTCP����
								1);
	
	if (!bRet)
	{
		OspPrintf( TRUE, FALSE, "[StartSatConfCast] startSwitch <0x%x@%d>-><0x%x@%d> failed!\n",
								 dwRcvIp,
								 wRcvPort,
								 dwMultiCastAddr,
								 wMultiCastPort);
	}
	return bRet;
}

/*====================================================================
    ������      ��StopSatConfCast
    ����        ��ֹͣ���ǻ����鲥����������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tSrc Դ�ն�
	              bySrcChnnl ͨ����
				  byMode ����ģʽ,ȱʡΪMODE_BOTH
    ����ֵ˵��  ��TRUE��������ܿ�ʼ�㲥����FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	11/11/09    4.6         �ű���        ����
====================================================================*/
BOOL32 CMpManager::StopSatConfCast(const TMt &tSrc, u8 byCastType, u8 byMode, u8 bySrcChnnl)
{

	if (MODE_BOTH == byMode)
	{
		return StopSatConfCast(tSrc, byCastType, MODE_AUDIO, bySrcChnnl) &
			   StopSatConfCast(tSrc, byCastType, MODE_VIDEO, bySrcChnnl);
	}

	u32  dwSrcIp  = 0;
	u32  dwRcvIp  = 0;
	u16  wRcvPort = 0;

	//�õ�����Դ��ַ
	if (!GetSwitchInfo(tSrc, dwRcvIp, wRcvPort, dwSrcIp))
	{
		MpManagerLog("StopSatConfCast() failure because of can't get switch info!\n");
		return FALSE ;
	}
	
	BOOL32 bRet = FALSE;

	u16 wMultiCastPort = g_cMcuVcApp.GetMcuMulticastDataPort();

	u16 wUniPort = g_cMcuVcApp.GetMcuRcvMtMediaStartPort() + (tSrc.GetMtId()-1) * PORTSPAN;
	
	u16 wPortAdj = 0;
	
	switch (byMode)
	{
	case MODE_AUDIO:
		wPortAdj += 2;
		break;
	case MODE_VIDEO:
		break;
	case MODE_SECVIDEO:
		wPortAdj += 4;
		break;
	}
	
	wUniPort += wPortAdj;

	if (CAST_UNI == byCastType)
	{
		//[1] ��������ֻȡIP���˿�ȡ�����ļ�

		//FIXME: �鲥�ش��͹�һ�����ļ���
		bRet = StopSwitch(tSrc.GetConfIdx(),
							dwRcvIp,
							wUniPort
							);
		
		if (!bRet)
		{
			OspPrintf( TRUE, FALSE, "[StartSatConfCast] stopSwitch <0x%x@%d> failed!\n",
									dwRcvIp,
									wUniPort);
			return bRet;
		}
	}
	
	wMultiCastPort += wPortAdj;

	if (CAST_FST == byCastType)
	{
		//[2] ȡ�����鲥���
		u32 dwMultiCastAddr = g_cMcuVcApp.GetMcuMulticastDataIpAddr();
		
		bRet = StopSwitch( tSrc.GetConfIdx(), dwMultiCastAddr, wMultiCastPort);
		
		if (!bRet)
		{
			OspPrintf( TRUE, FALSE, "[StartSatConfCast] stopSwitch <0x%x@%d> failed!\n",
									dwMultiCastAddr,
									wMultiCastPort);
		}
	}
	
	if (CAST_SEC == byCastType)
	{
		//[3] ͣ���ڶ��鲥���
		u32 dwSecMultiCastIpAddr = g_cMcuVcApp.GetMcuSecMulticastIpAddr();
	
		
		//�����polycom��֧��˫�����˴������һ·��Ƶ�鲥��ַ��ͣ˫��ʱ�ָ�;
		if (MODE_SECVIDEO == byMode)
		{
			wMultiCastPort -= wPortAdj;
		}

		bRet = StopSwitch( tSrc.GetConfIdx(),
						   dwSecMultiCastIpAddr, wMultiCastPort);
		
		if (!bRet)
		{
			OspPrintf( TRUE, FALSE, "[StopSatConfCast] startSwitch secMulSwitch <0x%x@%d> failed!\n",
									dwSecMultiCastIpAddr,
									wMultiCastPort);
		}
	}

	return bRet;
}


BOOL32 CMpManager::StartSwitchSrc2Dst(const TMt &tSrc,
									  const TMt &tDst,
									  u8		byMode,
									  u8		bySrcChnnl)
{
	if (MODE_BOTH == byMode)
	{
		return StartSwitchSrc2Dst(tSrc, tDst, MODE_AUDIO, bySrcChnnl) &
			   StartSwitchSrc2Dst(tSrc, tDst, MODE_VIDEO, bySrcChnnl);
	}

	u32  dwSrcIp  = 0;
	u32  dwRcvIp  = 0;
	u16  wRcvPort = 0;
	
	//�õ�����Դ��ַ
	if (!GetSwitchInfo(tSrc, dwRcvIp, wRcvPort, dwSrcIp))
	{
		MpManagerLog("StartSwitchSrc2Dst() failure because of can't get switch info!\n");
		return FALSE ;
	}

	//[1] ��������ֻȡIP���˿�ȡ�����ļ�
	u16 wUniPort = g_cMcuVcApp.GetMcuRcvMtMediaStartPort() + (tSrc.GetMtId()-1) * PORTSPAN;
	
	u16 wPortAdj = 0;
	
	switch (byMode)
	{
	case MODE_AUDIO:
		wPortAdj += 2;
		break;
	case MODE_VIDEO:
		break;
	case MODE_SECVIDEO:
		wPortAdj += 4;
		break;
	}
	
	wUniPort += wPortAdj;
	wRcvPort += wPortAdj;
	
	/*
	//FIXME: �鲥�ش��͹�һ�����ļ���
	BOOL32 bRet = StartSwitch(tSrc,
								tSrc.GetConfIdx(),
								dwSrcIp,
								0,
								dwRcvIp,
								wRcvPort,
								dwRcvIp,
								wUniPort,
								0,
								0,
								SWITCHCHANNEL_UNIFORMMODE_NONE
								);
	
	if (!bRet)
	{
		OspPrintf( TRUE, FALSE, "[StartSwitchSrc2Dst] startSwitch <0x%x@%d>-><0x%x@%d> failed!\n",
								dwRcvIp,
								wRcvPort,
								dwRcvIp,
								wUniPort);
		return FALSE;
	}
*/
	//[2] �������У�ֱ�ӽ�����Ŀ��

	//�õ�����Ŀ�ĵ�ַ
	u32 dwDstRcvIp = 0;
	u16 wDstRcvPort = 0;
	u32 dwDstSrcIp = 0;
	if (!GetSwitchInfo(tDst, dwDstRcvIp, wDstRcvPort, dwDstSrcIp))
	{
		MpManagerLog("StartSwitchSrc2Dst() failure because of can't get switch info!\n");
		return FALSE ;
	}

	u32 dwDstIp = dwDstSrcIp;
	u16 wDstPort = g_cMcuVcApp.GetMtRcvMcuMediaPort();
	
	wDstPort += wPortAdj;

	BOOL32 bRet = StartSwitch(tSrc,
								tSrc.GetConfIdx(),
								dwSrcIp,
								0,
								dwRcvIp,
								wUniPort,
								dwDstIp,
								wDstPort,
								0,
								0,
								SWITCHCHANNEL_UNIFORMMODE_NONE
								);

	if (!bRet)
	{
		OspPrintf( TRUE, FALSE, "[StartSwitchSrc2Dst] startSwitch <0x%x@%d>-><0x%x@%d> failed!\n",
								 dwRcvIp,
								 wUniPort,
								 dwDstRcvIp,
								 wDstRcvPort);
		return FALSE;
	}
	return TRUE;
}

BOOL32 CMpManager::StopSwitchSrc2Dst(const TMt &tSrc,
									 const TMt &tDst,
									 u8			byMode, 
									 u8			bySrcChnnl)
{
	if (MODE_BOTH == byMode)
	{
		return StopSwitchSrc2Dst(tSrc, tDst, MODE_AUDIO, bySrcChnnl) &
			   StopSwitchSrc2Dst(tSrc, tDst, MODE_VIDEO, bySrcChnnl);
	}
	u32  dwSrcIp  = 0;
	u32  dwRcvIp  = 0;
	u16  wRcvPort = 0;
	
	//�õ�����Դ��ַ
	if (!GetSwitchInfo(tSrc, dwRcvIp, wRcvPort, dwSrcIp))
	{
		MpManagerLog("StopSwitchSrc2Dst() failure because of can't get switch info!\n");
		return FALSE ;
	}


	//[1] ��������ֻȡIP���˿�ȡ�����ļ�
	u16 wUniPort = g_cMcuVcApp.GetMcuRcvMtMediaStartPort() + (tSrc.GetMtId()-1) * PORTSPAN;
	
	u16 wPortAdj = 0;
	
	switch (byMode)
	{
	case MODE_AUDIO:
		wPortAdj += 2;
		break;
	case MODE_VIDEO:
		break;
	case MODE_SECVIDEO:
		wPortAdj += 4;
		break;
	}
	
	wUniPort += wPortAdj;
	
	/*
	//FIXME: �鲥�ش��͹�һ�����ļ���
	BOOL32 bRet = StopSwitch(tSrc.GetConfIdx(),
							 dwRcvIp,
							 wUniPort
							 );
	
	if (!bRet)
	{
		OspPrintf( TRUE, FALSE, "[StopSwitchSrc2Dst] stopSwitch <0x%x@%d> failed!\n",
								 dwRcvIp,
								 wUniPort);
		return bRet;
	}
	*/

	//[2] ȡ��������Ŀ�ĵ��
	
	//�õ�����Ŀ�ĵ�ַ
	u32 dwDstRcvIp = 0;
	u16 wDstRcvPort = 0;
	u32 dwDstSrcIp = 0;
	if (!GetSwitchInfo(tDst, dwDstRcvIp, wDstRcvPort, dwDstSrcIp))
	{
		MpManagerLog("StopSwitchSrc2Dst() failure because of can't get switch info!\n");
		return FALSE ;
	}

	u32 dwDstIp = dwDstSrcIp;
	u16 wDstPort = g_cMcuVcApp.GetMtRcvMcuMediaPort();
	
	wDstPort += wPortAdj;
	
	BOOL32 bRet = StopSwitch(tSrc.GetConfIdx(), dwDstIp, wDstPort);
	
	if (!bRet)
	{
		OspPrintf( TRUE, FALSE, "[StartSwitchSrc2Dst] stopSwitch <0x%x@%d> failed!\n",
			dwDstIp,
			wDstPort);
		return FALSE;
	}

	return bRet;

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
====================================================================*/
BOOL32 CMpManager::GetSwitchInfo(const TMt & tSrc, u32 &dwSwitchIpAddr, u16 &wSwitchPort, u32 &dwSrcIpAddr)
{
	u32  dwSrcIp  = 0;
	u32  dwRcvIp  = 0;
	u16  wRcvPort = 0;

	u16  byMcuId  = tSrc.GetMcuId();
	u8   byMtId   = tSrc.GetMtId();
	u8   byEqpId  = tSrc.GetEqpId();
	u8   byPeriType;

    BOOL32 bHDVmp = FALSE;

	//�õ�����Դ�ĵ�ַ��˿�
	switch(tSrc.GetType()) 
	{

	case TYPE_MT://�ն�
		{
			TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable(tSrc.GetConfIdx());
			if (ptConfMtTable == NULL)
			{
				MpManagerLog("GetSwitchInfo() failure, Type: %d SubType: %d Id: %d ConfIdx: %d!\n", 
					   tSrc.GetType(), tSrc.GetEqpType(), tSrc.GetEqpId(),tSrc.GetConfIdx());
				return FALSE;
			}

			//GetSubMtSwitchAddr(tSrc,dwRcvIp,wRcvPort);
			ptConfMtTable->GetMtSwitchAddr(tSrc.GetMtId(), dwRcvIp, wRcvPort);
			dwSrcIp = ptConfMtTable->GetIPAddr(byMtId);
		}
		break;

	case TYPE_MCUPERI://����

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
		}
		g_cMcuAgent.GetPeriInfo(byEqpId, &dwSrcIp, &byPeriType);
		dwSrcIp = ntohl(dwSrcIp);
		break;
	}

	//�жϽ���Դ�ĵ�ַ��˿��Ƿ�Ϸ�
	if (dwRcvIp == 0 || wRcvPort == 0)
	{
        if (!(tSrc.GetType() == TYPE_MCUPERI  && tSrc.GetEqpType() == EQP_TYPE_TVWALL))
        {
		    MpManagerLog("GetSwitchInfo() failure, Type: %d SubType: %d Id: %d, dwRcvIp: %d wRcvPort: %d!\n", 
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
		    MpManagerLog("GetSwitchInfo() cann't get switch src IP, dwSrcIp: 0, byEqpId: %d!\n", byEqpId);
        }
        else
            MpManagerLog("GetSwitchInfo() cann't get switch src IP, dwSrcIp: 0, byMtId: %d!\n", byMtId);
		return FALSE;
	}

	dwSrcIpAddr    = dwSrcIp;
	dwSwitchIpAddr = dwRcvIp;
	wSwitchPort    = wRcvPort;

	return TRUE;
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
BOOL32 CMpManager::SetSwitchBridge(const TMt & tSrc, u8 bySrcChnnl, u8 byMode, BOOL32 bSrcHDBas)
{
	u32  dwSrcIp = 0;
	u32  dwRcvIp = 0;
	u16  wRcvPort = 0;

	TMt tSrcMt = tSrc;

	//�õ�����Դ��ַ
	if (!GetSwitchInfo(tSrcMt, dwRcvIp, wRcvPort, dwSrcIp))
	{
        MpManagerLog("SetSwitchBridge() failure because of can't get switch info!\n");
		return FALSE;
	}

	//����IP��ַ���ҽ���MP���
	u8 byMpId = g_cMcuVcApp.FindMp(dwRcvIp);
	 
    wRcvPort = wRcvPort +PORTSPAN*bySrcChnnl;

	//����������
	u32 dwDstIp = 0;
	BOOL32 bResult;
	BOOL32 bResultAnd = TRUE;
    dwRcvIp = 0;
    u8  byFirstMpId = 0;
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
               continue;
            }            

            //��һ��mp                   
            dwDstIp = g_cMcuVcApp.GetMpIpAddr(nMpId+1);

			bResult = StartBridgeSwitch(byMode, tSrcMt, dwSrcIp, dwRcvIp, wRcvPort, dwDstIp, bSrcHDBas);

			bResultAnd = bResultAnd && bResult;

            //����mp��Ŀ�ĵ�ַ��Ϊ��һ��mp�Ľ��յ�ַ
            dwRcvIp = dwDstIp;
	   } 
	}

    //��Դ���ǵ�һ��mp����Ҫ����β����ͷ�����Ž����������γ������Ž���
    if ( byMpId > 1 && byMpId != byFirstMpId && g_cMcuVcApp.IsMpConnected(byFirstMpId) )
    {
        dwDstIp = g_cMcuVcApp.GetMpIpAddr(byFirstMpId);

        bResult = StartBridgeSwitch(byMode, tSrcMt, dwSrcIp, dwRcvIp, wRcvPort, dwDstIp, bSrcHDBas);

		bResultAnd = bResultAnd && bResult;
    }

    return bResultAnd; 
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
BOOL32 CMpManager::RemoveSwitchBridge(const TMt & tSrc,u8 bySrcChnnl,u8 byMode, BOOL32 bEqpHDBas )
{
	u32  dwSrcIp = 0;
	u32  dwRcvIp = 0;
	u16  wRcvPort = 0;
 
	//�õ�����Դ��ַ
	if( !GetSwitchInfo( tSrc, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
        MpManagerLog("RemoveSwitchBridge() failure because of can't get switch info!");
		return FALSE;
	}

	//����IP��ַ���ҽ���MP���
	u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	
	wRcvPort = wRcvPort + PORTSPAN*bySrcChnnl;
  
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
        bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort, dwDstIp, wRcvPort, 0, 0, SWITCHCHANNEL_UNIFORMMODE_VALID);
        break;
    case MODE_AUDIO:
        bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort+2, dwDstIp, wRcvPort+2, 0, 0, SWITCHCHANNEL_UNIFORMMODE_VALID);
        break;
    case MODE_BOTH:
        bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort, dwDstIp, wRcvPort, 0, 0, SWITCHCHANNEL_UNIFORMMODE_VALID);
        bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort+2, dwDstIp, wRcvPort+2, 0, 0, SWITCHCHANNEL_UNIFORMMODE_VALID);
        break;
	case MODE_SECVIDEO:
		//zbq[09/05/2008] ���������˫���Ž���������4����
		if (bSrcHDBas)
		{
			bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort, dwDstIp, wRcvPort, 0, 0, SWITCHCHANNEL_UNIFORMMODE_VALID);
		}
		else
		{
			bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort+4, dwDstIp, wRcvPort+4, 0, 0, SWITCHCHANNEL_UNIFORMMODE_VALID);
		}
		break;
    default:
        break;
    }

    return bResult;
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
BOOL32 CMpManager::StartSwitchToMc( const TMt & tSrc, u8 bySrcChnnl, u16 wMcInstId, u8 byDstChnnl, u8 byMode )
{
	u32  dwSrcIp = 0;
	u32	dwRcvIp;
	u16	wRcvPort;
	u8  byChannelNum;
	TLogicalChannel tLogicalChannel;
	BOOL32 bResult1 = TRUE;
	BOOL32 bResult2 = TRUE;

	//�õ�����Դ��ַ
	if( !GetSwitchInfo( tSrc, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
		MpManagerLog("StartSwitchToMc() failure because of can't get switch info!");
		return FALSE ;
	}

	//�����ŵ��ż���ƫ����	
	wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;

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
			MpManagerLog( "Cannot get video send address to Meeting Console%u!\n", wMcInstId );
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
			MpManagerLog("Cannot get audio send address to Meeting Console%u!\n", wMcInstId );
		}
	}
	
    return bResult1&&bResult2; 
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
}


/*====================================================================
    ������      ��StartSwitchToBrd
    ����        ����ָ���ն˵���Ƶ��Ϊ�㲥Դ
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
BOOL32 CMpManager::StartSwitchToBrd( const TMt &tSrc, u8 bySrcChnnl, BOOL32 bForce)
{
	u32  dwSrcIp = 0;
	u32  dwRcvIp = 0;
	u16  wRcvPort = 0;
	BOOL32 bResult = TRUE;

	//�õ�����Դ��ַ
	if( !GetSwitchInfo( tSrc, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
		 MpManagerLog("StartSwitchToBrd() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//�����ŵ��ż���ƫ����
	wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl; 

    //��������

    //����IP��ַ���ҽ���MP���
    u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
    if( 0 == byMpId )
    {
        MpManagerLog("No MP's Addrs is 0x%x in StartSwitchToBrd(),please check switch recv IP!\n",dwRcvIp);
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
    cServMsg.SetMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));
    u8 byForce = bForce ? 1 : 0;
    cServMsg.CatMsgBody(&byForce, sizeof(byForce));
    
    g_cMpSsnApp.BroadcastToAllMpSsn(MCU_MP_ADDBRDSRCSWITCH_REQ, 
                                    cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
    
    MpManagerLog("Send MCU_MP_ADDBRDSRCSWITCH_REQ from RecvMp %s:%d to all Mp.\n",
                 StrOfIP(tSwitchChannel.GetRcvIP()), tSwitchChannel.GetRcvPort());
    
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
BOOL32 CMpManager::StartSwitchFromBrd(const TMt &tSrc, u8 bySrcChnnl, u8 byDstMtNum, const TMt* const ptDstBase)
{
    CMcuVcInst * pcVcInst = g_cMcuVcApp.GetConfInstHandle(tSrc.GetConfIdx());
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

    if( ptConfMtTable == NULL || pcVcInst == NULL )
    {
        MpManagerLog("StartSwitchToSubMt() param err. ptMtTable.0x%x, ptVcInst.0x%x!\n", ptConfMtTable, pcVcInst);
        return FALSE;
    }

    //�õ�����Դ��ַ
    if( !GetSwitchInfo( tSrc, dwSrcMpIp, wRcvPort, dwSrcIp ) )
    {
        MpManagerLog("StartSwitchToSubMt() failure because of can't get switch info!\n");
        return FALSE ;
	}
    //�����ŵ��ż���ƫ����
    wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;

    CServMsg cServMsg;
    cServMsg.SetConfIdx(tSrc.GetConfIdx());    
    TSwitchChannel tSwitchChannel;
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
            // ÿ�δ���ǰMP������ն�
            byMpId = ptConfMtTable->GetMpId( ptDstBase[byMtLoop].GetMtId() );
            if ( byMpId != byMpLoop )
            {
                continue;
            }

            // zbq [06/29/2007] �ش�Դ�Ĺ㲥���������滻����ͨ�ش�����
            if (!pcVcInst->m_tCascadeMMCU.IsNull()  &&
                 pcVcInst->m_ptConfOtherMcTable->GetMcInfo(pcVcInst->m_tCascadeMMCU.GetMtId())->m_tSpyMt == tSrc &&
                 pcVcInst->m_tCascadeMMCU == ptDstBase[byMtLoop] )
            {
                MpManagerLog( "[StartSwitchFromBrd] tSrc.%d create no brd switch to MMcu.%d\n", 
                               tSrc.GetMtId(), pcVcInst->m_tCascadeMMCU.GetMtId() );
                continue;
            }

            byManuID = ptConfMtTable->GetManuId(ptDstBase[byMtLoop].GetMtId());     
            ptConfMtTable->GetMtLogicChnnl(ptDstBase[byMtLoop].GetMtId(), LOGCHL_VIDEO, &tLogicChannel, TRUE);
            
            if (( TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_VMP == tSrc.GetEqpType() && MT_TYPE_MT != ptDstBase[byMtLoop].GetMtType() &&
                ptConfInfo->GetConfAttrb().IsResendLosePack()) ||
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
            tSwitchChannel.SetSrcMt( tSrc );
            tSwitchChannel.SetSrcIp( dwSrcIp );
            tSwitchChannel.SetRcvIP( dwRcvIp );
            tSwitchChannel.SetRcvPort( wRcvPort );
            tSwitchChannel.SetDisIp( dwDisIp );
            tSwitchChannel.SetRcvBindIP( dwRcvIp );
            tSwitchChannel.SetDstIP( tLogicChannel.m_tRcvMediaChannel.GetIpAddr() );
        	tSwitchChannel.SetDstPort( tLogicChannel.m_tRcvMediaChannel.GetPort() );
            
            // ׼������ǰMP����Ϣ��
            cServMsg.CatMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));                  
            cServMsg.CatMsgBody((u8 *)&bySwitchChannelMode, sizeof(u8));
            cServMsg.CatMsgBody((u8 *)&byUniformPayload, sizeof(u8));            
            cServMsg.CatMsgBody((u8 *)&byIsSwitchRtcp, sizeof(u8));

            MpManagerLog("Send MCU_MP_ADDBRDDSTSWITCH_REQ for Mt.%d(%s:%d) to Mp.%d \n",
                ptDstBase[byMtLoop].GetMtId(),
                StrOfIP(tSwitchChannel.GetDstIP()), tSwitchChannel.GetDstPort(), byMpLoop);
        }
        if ( cServMsg.GetMsgBodyLen() > 0 )
        {
            g_cMpSsnApp.SendMsgToMpSsn( byMpLoop, 
                                        MCU_MP_ADDBRDDSTSWITCH_REQ, 
                                        cServMsg.GetServMsg(), 
                                        cServMsg.GetServMsgLen());        
        }
    }

    return TRUE;
}

/*====================================================================
    ������      ��StartSwitchEqpFromBrd
    ����        �����㲥Դ����������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byEqpId, ����Ŀ��
    ����ֵ˵��  ��TRUE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/08/07    4.0         �ű���        ����
====================================================================*/
BOOL32 CMpManager::StartSwitchEqpFromBrd( const TMt &tSrc, u8 bySrcChnnl, u8 byEqpId, u16 wDstChnnl, u8 bySwitchMode )
{
    u8  byChannelNum = 0;
    u32 dwDisIp = 0;
    u32 dwSrcIp = 0;
    u32 dwRcvIp = 0;
    u16 wRcvPort = 0;
    u32 dwDstIp = 0;
    u16 wDstPort = 0;
    TLogicalChannel	tLogicalChannel;

        //�õ�����Դ��ַ
    if (!GetSwitchInfo(tSrc, dwRcvIp, wRcvPort, dwSrcIp))
    {
        MpManagerLog("StartSwitchEqpFromBrd() failure because of can't get switch info!");
        return FALSE;
    }

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
    }

    // ��Ҫת��
    if (SWITCH_MODE_BROADCAST == bySwitchMode || EQP_TYPE_RECORDER == byEqpType
        /* || 
        // zbq [06/14/2007] VMP�������Ե���
        EQP_TYPE_VMP == byEqpType*/ )
    {
        if (dwMpIP != dwRcvIp)
        {
            dwDisIp = dwRcvIp;
            dwRcvIp = dwMpIP;
        }
    }
    
    wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;

    if (g_cMcuVcApp.GetPeriEqpLogicChnnl(byEqpId, MODE_VIDEO, &byChannelNum, &tLogicalChannel, TRUE))
    {
        CServMsg cServMsg;
        cServMsg.SetConfIdx( tSrc.GetConfIdx() );
        
        dwDstIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
        wDstPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();
        
        TSwitchChannel tSwitchChannel;
        u8 bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_NONE;
        u8 byUniformPayload = INVALID_PAYLOAD;
        u8 byIsSwitchRtcp = 0;
        
        
        //���콻���ŵ�
        tSwitchChannel.SetSrcMt( tSrc );
        tSwitchChannel.SetSrcIp( dwSrcIp );
        tSwitchChannel.SetRcvIP( dwRcvIp );
        tSwitchChannel.SetRcvPort( wRcvPort );
        tSwitchChannel.SetDisIp( dwDisIp );
        tSwitchChannel.SetRcvBindIP( dwRcvIp );
        tSwitchChannel.SetDstIP( dwDstIp );
        tSwitchChannel.SetDstPort( wDstPort + PORTSPAN * wDstChnnl );
        
        // ׼������ǰMP����Ϣ��
        cServMsg.CatMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));                  
        cServMsg.CatMsgBody((u8 *)&bySwitchChannelMode, sizeof(u8));
        cServMsg.CatMsgBody((u8 *)&byUniformPayload, sizeof(u8));            
        cServMsg.CatMsgBody((u8 *)&byIsSwitchRtcp, sizeof(u8));
        
        
        MpManagerLog("Send MCU_MP_ADDBRDDSTSWITCH_REQ for Eqp.%d(%s:%d)\n",
                      byEqpId,
                      StrOfIP(tSwitchChannel.GetDstIP()), tSwitchChannel.GetDstPort());
        
        BOOL32 bDstMpExist = FALSE;
        u8 byDstMpId = 0;
        for( u8 byMpId = 1; byMpId <= MAXNUM_DRI; byMpId ++ )
        {
            if ( g_cMcuVcApp.GetMpIpAddr(byMpId) == dwMpIP )
            {
                bDstMpExist = TRUE;
                byDstMpId = byMpId;
                break;
            }
        }
        if ( !bDstMpExist )
        {
            OspPrintf( TRUE, FALSE, "[StartSwitchEqpFromBrd] Dst Mp.0x%x Unexist\n", dwMpIP );
        }
        
        g_cMpSsnApp.SendMsgToMpSsn( byDstMpId, 
                                    MCU_MP_ADDBRDDSTSWITCH_REQ, 
                                    cServMsg.GetServMsg(), 
                                    cServMsg.GetServMsgLen());
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
BOOL32 CMpManager::StopSwitchToBrd( const TMt &tSrc, u8 bySrcChnnl )
{
    CServMsg cServMsg;
    TSwitchChannel tSwitchChannel;
    
    //�õ�����Դ��ַ
    u32 dwSrcMpIp;
    u16 wRcvPort;
    u32 dwSrcIp;
    if( !GetSwitchInfo( tSrc, dwSrcMpIp, wRcvPort, dwSrcIp ) )
    {
        MpManagerLog("StopSwitchToBrd() failure because of can't get switch info!\n");
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

    MpManagerLog("Send MCU_MP_REMOVEBRDSRCSWITCH_REQ from RecvMp %s:%d to all Mp.\n",
                 StrOfIP(tSwitchChannel.GetRcvIP()), tSwitchChannel.GetRcvPort());

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
BOOL32 CMpManager::StartSwitchToSubMt( const TMt & tSrc, 
                                     u8        bySrcChnnl, 
                                     const TMt & tDst, 
                                     u8        byMode,
                                     u8        bySwitchMode, 
                                     BOOL32 bH239Chnnl,
									 BOOL32 bStopBeforeStart,
									 BOOL32 bSrcHDBas)
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
		 MpManagerLog("StartSwitchToSubMt() failure because of can't get switch info!\n");
		 return FALSE ;
	}
    
    TConfInfo * ptConfInfo = &g_cMcuVcApp.GetConfInstHandle(tSrc.GetConfIdx())->m_tConf;
    u8 byIsSwitchRtcp;
    if (( TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_VMP == tSrc.GetEqpType() && MT_TYPE_MT != tDst.GetMtType() &&
        ptConfInfo->GetConfAttrb().IsResendLosePack()) ||
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
        MpManagerLog("StartSwitchToSubMt() tDstMt is NULL! It's send-only MT?\n");        
		return TRUE;
	}

	//�õ������ն˱�ָ��
	TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable( tSrc.GetConfIdx() );
	if( ptConfMtTable == NULL )
	{
		MpManagerLog("StartSwitchToSubMt() failure because invalid ConfMtTable Pointer!\n");
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
            u8 byChannelType;
            TLogicalChannel  tChannel;
            memset(&tChannel, 0, sizeof(TLogicalChannel));
            //Get channal info
            if (0 == bySrcChnnl)
            {
                byChannelType = LOGCHL_VIDEO;
            }
            else if (1 == bySrcChnnl)
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
	//if(bySwitchMode==SWITCH_MODE_BROADCAST)
	{
		//�õ�Ŀ��Mt�Ľ�����Ϣ
		u32  dwMpIP = 0;
		dwMpIP = g_cMcuVcApp.GetMpIpAddr( ptConfMtTable->GetMpId( tDst.GetMtId() ) );
		if( dwMpIP != dwRcvIp )//��Ҫת��
		{
			dwDisIp = dwRcvIp;
			dwRcvIp = dwMpIP;
		}
	}

	//�����ŵ��ż���ƫ����
	wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;    //˫��Ҫ����

	//ͼ��
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		//��������
		if( ptConfMtTable->GetMtLogicChnnl( tDst.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, TRUE ) )	//���Խ���
		{
            bResult1 = StartSwitch( tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort, tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp, bStopBeforeStart);
			if(!bResult1)MpManagerLog("Switch video failure!\n");

			/*
#ifdef  _SATELITE_
			//���ǻ���ͬ�������ǹ㲥��
			//Ŀǰ���������ߣ��������ߴ˴�
			if (SWITCH_MODE_BROADCAST == bySwitchMode)
			{
				if ( bySrcChnnl != 0 )
				{
					StartSatConfCast(tSrc, MODE_VIDEO, bySrcChnnl);
				}
			}
			else
			{
				if ( bySrcChnnl != 0 )
				{
					StartSwitchSrc2Dst(tSrc, tDst, MODE_VIDEO, bySrcChnnl);
				}
			}
#endif
			*/
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
										   tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp);
					/*
			#ifdef  _SATELITE_
					//���ǻ���ͬ�������ǹ㲥��
					//Ŀǰ���������ߣ��������ߴ˴�
					if (SWITCH_MODE_BROADCAST == bySwitchMode)
					{
						StartSatConfCast(tSrc, MODE_SECVIDEO, bySrcChnnl);
					}
			#endif
			*/
				}
				else
				{
					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort+4,
										   tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp);					
				}
                if (!bResult1)
                {
                    MpManagerLog("Switch video h.239 failure!\n");
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
			bResult2 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, (wRcvPort+2),
                                   tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp);
			if(!bResult2)MpManagerLog("Switch audio failure!\n");
		}
	}	

	return  bResult1 && bResult2 ;
}

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
void CMpManager::StopSwitchToSubMt( const TMt & tDst, u8 byMode, BOOL32 bH239Chnnl )
{
	u8   byDstMtId = tDst.GetMtId();
	TLogicalChannel	tLogicalChannel;

	TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable( tDst.GetConfIdx() );
	if( ptConfMtTable == NULL )
	{
		MpManagerLog("StopSwitchToSubMt() failure because invalid ConfMtTable Pointer!\n");
		return ;
	}

    TConfInfo * ptConfInfo = &g_cMcuVcApp.GetConfInstHandle(tDst.GetConfIdx())->m_tConf;
    u8 byIsSwitchRtcp;
    if (MT_TYPE_MT != tDst.GetMtType() && ptConfInfo->GetConfAttrb().IsResendLosePack())
    {
        byIsSwitchRtcp = 0;
    }
    else
    {
        byIsSwitchRtcp = 1;
    }

    //ͼ��
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		if( ptConfMtTable->GetMtLogicChnnl( byDstMtId, LOGCHL_VIDEO, &tLogicalChannel, TRUE ) )	//�ѵǼ�
		{
			StopSwitch(tDst.GetConfIdx(), tLogicalChannel, 0, byIsSwitchRtcp);

			/*
#ifdef  _SATELITE_
			//���ǻ���ͬ��ֹͣ���ǹ㲥
			//Ŀǰ��������ֹͣ���������ߴ˴�
			if ( bySrcChnnl != 0 )
			{
				StopSatConfCast(tSrc, MODE_VIDEO, bySrcChnnl);
			}
			if ( bySrcChnnl != 0 )
			{
				StopSwitchSrc2Dst(tSrc, tDst, MODE_VIDEO, bySrcChnnl);
			}
#endif
			*/
		}
		
	}

    if (MODE_SECVIDEO == byMode)
	{
		//H.239��Ƶ�߼�ͨ��
		if (bH239Chnnl)
		{
			if (ptConfMtTable->GetMtLogicChnnl(tDst.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, TRUE))
			{
				StopSwitch(tDst.GetConfIdx(), tLogicalChannel, 0, byIsSwitchRtcp);
			/*
#ifdef  _SATELITE_
				//���ǻ���ͬ��ֹͣ���ǹ㲥��
				if (SWITCH_MODE_BROADCAST == bySwitchMode)
				{
					StopSatConfCast(tSrc, MODE_SECVIDEO, bySrcChnnl);
				}
#endif
			*/
			}
			return;
		}
	}

    //����
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		if( ptConfMtTable->GetMtLogicChnnl( byDstMtId, LOGCHL_AUDIO, &tLogicalChannel, TRUE ) )	//�ѵǼ�
		{
			StopSwitch(tDst.GetConfIdx(), tLogicalChannel, 0, byIsSwitchRtcp);
		}
	}
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
        dwSwitchIpAddr = ntohl( g_cMcuAgent.GetBoardIpAddrFromIdx(tRecInfo.GetSwitchBrdId()) );
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
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����tSrc, Դ
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
====================================================================*/
BOOL32 CMpManager::StartSwitchToPeriEqp(const TMt & tSrc,
										u8	bySrcChnnl,
										u8	byEqpId,
										u16 wDstChnnl,
										u8	byMode,
										u8	bySwitchMode,
										BOOL32 bDstHDBas,
										BOOL32 bStopBeforeStart,
										BOOL32 bVCSConf)
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
        MpManagerLog("StartSwitchToPeriEqp() failure because of can't get switch info!\n");
        return FALSE;
    }
	
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
    }

    // ��Ҫת��
    if (SWITCH_MODE_BROADCAST == bySwitchMode || EQP_TYPE_RECORDER == byEqpType)
    {
        if (dwMpIP != dwRcvIp)
        {
            dwDisIp = dwRcvIp;
            dwRcvIp = dwMpIP;
        }
    }

    wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;

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

    MpManagerLog("[StartSwitchToPeriEqp] bModify2Inner.%d, mcueqp recv ip: 0x%x\n", bModified2InnerIp, dwEqpIp);
#endif


    //ͼ��
    if (MODE_VIDEO == byMode || MODE_BOTH == byMode)
    {
        //��baphd����ͬ��
        if (EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId) &&
            g_cMcuAgent.IsEqpBasHD(byEqpId))
        {
            wDstChnnl = wDstChnnl * 2;
        }

#ifndef _SATELITE_
        // zbq [08/04/2007] BAS����ͬ�㲥Ŀ�꽻������Ϊͬһ�߼�
        if ( EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId) && bySwitchMode == SWITCH_MODE_BROADCAST &&
			 !bVCSConf )
        {
            StartSwitchEqpFromBrd( tSrc, bySrcChnnl, byEqpId, wDstChnnl, bySwitchMode );
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
#else
		StartSatConfCast(tSrc, CAST_UNI, MODE_VIDEO, bySrcChnnl);
		//StartSatConfCast(tSrc, CAST_DST, MODE_VIDEO, bySrcChnnl);

		if (g_cMcuVcApp.GetPeriEqpLogicChnnl(byEqpId, MODE_VIDEO, &byChannelNum, &tLogicalChannel, TRUE))
		{
			dwDstIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
			wDstPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();

			wRcvPort =  g_cMcuVcApp.GetMcuRcvMtMediaStartPort() + (tSrc.GetMtId()-1) * PORTSPAN;

			bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort,
									dwDstIp, (wDstPort + PORTSPAN * wDstChnnl), 0, 0, 
									SWITCHCHANNEL_UNIFORMMODE_NONE, INVALID_PAYLOAD, 1, bStopBeforeStart
									);// xliang [4/13/2009] modify for 4 last param
            }
#endif
    }

    //����
    if (MODE_AUDIO == byMode || MODE_BOTH == byMode)
    {
        //��baphd����ͬ��
        if (EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId) &&
            g_cMcuAgent.IsEqpBasHD(byEqpId))
        {
            wDstChnnl = wDstChnnl * 2;
        }

#ifndef _SATELITE_
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
#else
		StartSatConfCast(tSrc, CAST_UNI, MODE_AUDIO, bySrcChnnl);
		//StartSatConfCast(tSrc, CAST_DST, MODE_AUDIO, bySrcChnnl);

		if (g_cMcuVcApp.GetPeriEqpLogicChnnl(byEqpId, MODE_AUDIO, &byChannelNum, &tLogicalChannel, TRUE))
        {
            dwDstIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
            wDstPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();

			wRcvPort =  g_cMcuVcApp.GetMcuRcvMtMediaStartPort() + (tSrc.GetMtId()-1) * PORTSPAN;
			
            bResult2 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, (wRcvPort + 2),
				dwDstIp, (wDstPort + PORTSPAN * wDstChnnl), 0, 0);
        }
#endif
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

            if (EQP_TYPE_PRS == byEqpType)
            {
                bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, (wRcvPort + 4),
                                       dwDstIp, (wDstPort + PORTSPAN * wDstChnnl), 0, 0);
            }
            else
            {
				//zbq[09/03/2008] ����HD Bas��˫�������� �˿ڻ�ַ+PORTSPAN ��ʼ������
				if (!bDstHDBas)
				{
					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, (wRcvPort + 4),
										   dwDstIp, (wDstPort + PORTSPAN * wDstChnnl + 4), 0, 0);					
				}
				else
				{
					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, (wRcvPort + 4),
										   dwDstIp, (wDstPort + PORTSPAN * (wDstChnnl * 2)), 0, 0);
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
void CMpManager::StopSwitchToPeriEqp( u8 byConfIdx, u8 byEqpId, u16 wDstChnnl,  u8 byMode )
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
    
    MpManagerLog("[StopSwitchToPeriEqp] bModify2Inner.%d, mcueqp recv ip: 0x%x\n", bModified2InnerIp, dwEqpIp);
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
			// Ŀǰ������basͨ���������10*2
			u8 byMulPortSpace = 1;
			if (EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId) &&
				g_cMcuAgent.IsEqpBasHD(byEqpId))
			{
				byMulPortSpace = 2;
			}
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
            // Ŀǰ������basͨ���������10*2
            if (EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId) &&
                g_cMcuAgent.IsEqpBasHD(byEqpId))
            {
                wDstChnnl = 2 * wDstChnnl;
			}
			StopSwitch( byConfIdx, tLogicalChannel, wDstChnnl );
		}
	}
    
    // ˫��
    if ( byMode == MODE_SECVIDEO )
    {
        u8 byEqpType = g_cMcuAgent.GetPeriEqpType(byEqpId);
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
            else
            {
                //zbq[01/07/2009] ˫�����佻��
                if (!g_cMcuAgent.IsEqpBasHD(byEqpId))
                {
                    StopSwitch( byConfIdx, 
                        tLogicalChannel.m_tRcvMediaChannel.GetIpAddr(),
                        tLogicalChannel.m_tRcvMediaChannel.GetPort() + PORTSPAN * wDstChnnl + 4, 
                        TRUE);
                }
                else
                {
                    StopSwitch( byConfIdx, 
                        tLogicalChannel.m_tRcvMediaChannel.GetIpAddr(),
                        tLogicalChannel.m_tRcvMediaChannel.GetPort() + PORTSPAN * wDstChnnl * 2, 
                        TRUE);
                }
            }   

		}        
    }
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
                             u8 byIsSwitchRtcp, BOOL32 bStopBeforeStart)
{
	//����IP��ַ���ҽ���MP���
	u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	//û�ҵ�
	if( 0 == byMpId )
	{
        MpManagerLog("No MP's Addrs is 0x%x in StartSwitch(),please check switch recv IP!\n",dwRcvIp);
		return FALSE;
	}

	//���콻���ŵ� 
	TSwitchChannel tSwitchChannel;
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
    MpManagerLog("Send Message: MCU_MP_ADDSWITCH_REQ for %s:%d --> %s:%d to Mp.\n",
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

    MpManagerLog("Send Message: MCU_MP_REMOVESWITCH_REQ for %s:%d to Mp in StopSwitch().\n",
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

    MpManagerLog("Send Message: MCU_MP_REMOVESWITCH_REQ for %s:%d to Mp in StartStopSwitch() .\n",
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
                              u8 byIsSwitchRtcp, BOOL32 bStopBeforeStart)
{
	return StartSwitch(tSrcMt, byConfIdx, dwSrcIp, dwDisIp, dwRcvIp, wRcvPort, 
		               tFwdChnnl.m_tRcvMediaChannel.GetIpAddr(),
		               (tFwdChnnl.m_tRcvMediaChannel.GetPort() + PORTSPAN*wDstChnnl),
                       0, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp, bStopBeforeStart);
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
	
	//û�ҵ�
	if(byMpId==0)
	{
        MpManagerLog("No MP's Addrs is 0x%x in AddMultiToOneSwitch(),please check switch recv IP!",dwRcvIp);
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
        MpManagerLog("No MP's Addrs is 0x%x in RemoveMultiToOneSwitch(),please check switch recv IP!\n",dwRcvIp);
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
====================================================================*/
BOOL32 CMpManager::AddRecvOnlySwitch( u8 byConfIdx, u32 dwRcvIp, u16 wRcvPort )
{
	//����IP��ַ���ҽ���MP���
	u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	
	//û�ҵ�
	if(byMpId==0)
	{
		MpManagerLog("No MP's Addrs is 0x%x in AddRecvOnlySwitch(),please check switch recv IP!\n",dwRcvIp);
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
		MpManagerLog("No MP's Addrs is 0x%x in RemoveRecvOnlySwitch(),please check switch recv IP!\n",dwRcvIp);
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
		 MpManagerLog("StartRecvMt() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//ͼ��
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		//����dump����
		bResult1 = AddRecvOnlySwitch( tMt.GetConfIdx(), dwRcvIp, wRcvPort );
		if(!bResult1)
		{
			MpManagerLog("Switch video dump failure!\n");
		}
	}
		
	//����
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		//����dump����
		bResult2 = AddRecvOnlySwitch( tMt.GetConfIdx(), dwRcvIp, wRcvPort+2 );
		if(!bResult2)
		{
			MpManagerLog("Switch audio dump failure!\n");
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
		 MpManagerLog("StopRecvMt() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//ͼ��
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		//���dump����
		bResult1 = RemoveRecvOnlySwitch( tMt.GetConfIdx(), dwRcvIp, wRcvPort );
		if(!bResult1)
		{
			MpManagerLog("Stop switch video dump failure!\n");
		}
	}
		
	//����
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		//���dump����
		bResult2 = RemoveRecvOnlySwitch( tMt.GetConfIdx(), dwRcvIp, wRcvPort+2 );
		if(!bResult2)
		{
			MpManagerLog("Stop switch audio dump failure!\n");
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
        MpManagerLog("No MP's Addrs is 0x%x in SetRecvMtSSRCValue(),please check switch recv IP!\n",dwRcvIp);
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
		 MpManagerLog("ResetRecvMtSSRC() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//ͼ��
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		bResult1 = SetRecvMtSSRCValue( tMt.GetConfIdx(), dwRcvIp, wRcvPort, TRUE );
		if(!bResult1)
		{
			MpManagerLog("ResetRecvMtSSRC video switch failure!\n");
		}
	}
	//����
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		bResult2 = SetRecvMtSSRCValue( tMt.GetConfIdx(), dwRcvIp, wRcvPort+2, TRUE );
		if(!bResult2)
		{
			MpManagerLog("ResetRecvMtSSRC audio switch failure!\n");
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
		 MpManagerLog("RestoreRecvMtSSRC() failure because of can't get switch info!\n");
		 return FALSE;
	}

	//ͼ��
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		bResult1 = SetRecvMtSSRCValue( tMt.GetConfIdx(), dwRcvIp, wRcvPort, FALSE );
		if(!bResult1)
		{
			MpManagerLog("RestoreRecvMtSSRC video switch failure!\n");
		}
	}
		
	//����
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		bResult2 = SetRecvMtSSRCValue( tMt.GetConfIdx(), dwRcvIp, wRcvPort+2, FALSE );
		if(!bResult2)
		{
			MpManagerLog("RestoreRecvMtSSRC audio switch failure!\n");
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

    //multicast
//    u8 byNetNo = g_cMcuAgent.GetCriDriNetChoice(tMp.GetMpId());
    u8 byIsMulticast = g_cMcuAgent.GetCriDriIsCast(tMp.GetMpId()) ? 1:0;
//    tMp.SetNetNo(byNetNo);
    tMp.SetMulticast(byIsMulticast);

    g_cMcuVcApp.AddMp(tMp);

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
                if( NULL == g_cMcuVcApp.GetConfInstHandle( byConfIdx ) )
                {
                    continue;
                }
                else
                {
                    ptConfInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConf;
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
void CMpManager::ProcMtAdpDisconnect(const CMessage * pcMsg)
{
	CServMsg cServMsg( pcMsg->content ,pcMsg->length );	
	TMtAdpReg tMtAdpReg =  *( TMtAdpReg* )cServMsg.GetMsgBody();
	u8 byRRQReset = *( cServMsg.GetMsgBody() + sizeof(TMtAdpReg) );
	g_cMcuVcApp.RemoveMtAdp( tMtAdpReg.GetDriId() );
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
                if( NULL == g_cMcuVcApp.GetConfInstHandle( byConfIdx ) )
                {
                    continue;
                }
                else
                {
                    ptConfInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConf;

                    // ����GK�Ʒѻ����ر���, zgc, 2008-09-26
                    // �ȴ��������ע�����������¿�ʼע��Ʒѻ���
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

		//���GKID/EPID��Ϣ
		memset((void*)&g_cMcuVcApp.m_tGKID, 0, sizeof(TH323EPGKIDAlias));
		memset((void*)&g_cMcuVcApp.m_tEPID, 0, sizeof(TH323EPGKIDAlias));
			
		//����ע�������ģ�������������ע��Ǩ�ƣ�����ѡ������ģ��� ע��MCU������ʵ��
//		cServMsg.SetDstDriId( byMinDri );
//		if( 0 != byMinDri && byMinDri <= MAXNUM_DRI )
//		{
//			cServMsg.SetMsgBody( (u8*)&byMinDri, sizeof( byMinDri ) );
//			g_cMtAdpSsnApp.SendMsgToMtAdpSsn( byMinDri, MCU_MT_REREGISTERGK_REQ, cServMsg );
//		}
	}
	
	return;
}

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
                if( NULL == g_cMcuVcApp.GetConfInstHandle( byConfIdx ) )
                {
                    continue;
                }
                else
                {
                    ptConfInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConf;
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
    OspPrintf(TRUE,FALSE,"\n------------------MCU %s register MP list----------------", achMcuAlias );
	for( byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		if(g_cMcuVcApp.m_atMpData[byLoop].m_bConnected)
		{
            OspPrintf(TRUE,FALSE, "\nMp%d: %s (Band: %d/%d[Total/Real] P.K, NetTraffic: %d/%d/%d[Total/Real/Reserved] Mbps)", 
                                  byLoop+1, 
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
	OspPrintf(TRUE,FALSE,"\nTotal Register MP num: %d \n",nMpSum);
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
    OspPrintf(TRUE,FALSE,"\n------------------MCU %s register MtAdp list----------------", achMcuAlias );
	for( byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		if( g_cMcuVcApp.m_atMtAdpData[byLoop].m_bConnected )
		{
            byMtNum = 0;
            bySMcuNum = 0;
            g_cMcuVcApp.GetMtNumOnDri(byLoop+1, TRUE, byMtNum, bySMcuNum);
            OspPrintf( TRUE,FALSE, "\nMtAdp%d: %s (MaxMT/SMcu||OnlineMt/SMcu/All: %d/%d||%d/%d/%d)", byLoop+1, 
				       StrOfIP( g_cMcuVcApp.GetMtAdpIpAddr( byLoop+1 ) ),
                       g_cMcuVcApp.m_atMtAdpData[byLoop].m_byMaxMtNum,
                       g_cMcuVcApp.m_atMtAdpData[byLoop].m_byMaxSMcuNum,
                       byMtNum,
                       bySMcuNum,
                       g_cMcuVcApp.m_atMtAdpData[byLoop].m_wMtNum);
            
            if ( ISTRUE(g_cMcuVcApp.m_atMtAdpData[byLoop].m_byIsSupportHD) )
            {
                OspPrintf( TRUE,FALSE," (HD)" );
            }

			nMtAdpSum++;
		}		
	}
	OspPrintf(TRUE,FALSE,"\nTotal Register MtAdp num: %d", nMtAdpSum);
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
        if(NULL == g_cMcuVcApp.GetConfInstHandle( byConfIdx ))
        {
            continue;
        }

        ptConfFullInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConf;
		if( ptConfFullInfo->m_tStatus.IsOngoing() )
		{   
			OspPrintf( TRUE, FALSE, "\n--------Conf: %s bridge info--------\n", ptConfFullInfo->GetConfName() );
		    ptSwitchTable = g_cMcuVcApp.GetConfSwitchTable( byConfIdx );
			for( byLoop1 = 1; byLoop1 <= MAXNUM_DRI; byLoop1++ )
			{
				if( g_cMcuVcApp.IsMpConnected( byLoop1 ) )
				{
					OspPrintf( TRUE, FALSE, "\nMp%d switch info: ", byLoop1 );
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

								OspPrintf( TRUE, FALSE, achTemp );
							}
						}
					}
				}
			}
		}
	}
}

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
	TConfSwitchTable *ptSwitchTable;
	u8  byMpId, byLoop1;
    u16 wLoop2;
	u32 dwSrcIp, dwDisIp, dwRcvIp, dwDstIp;
	u16 wRcvPort, wDstPort;
	TSwitchChannel *ptSwitchChannel;
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

		ptConfFullInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConf;
		if( ptConfFullInfo->m_tStatus.IsOngoing() )
		{   
            // �����Ƶ�㲥Դ, zgc, 2008-07-23
            tVidBrdSrc = g_cMcuVcApp.GetConfInstHandle( byConfIdx )->GetVidBrdSrc();
            if ( !tVidBrdSrc.IsNull() )
            {
                GetSwitchInfo( tVidBrdSrc, dwSwitchIp, wBrdSrcRcvPort, dwSwitchSrcIp );
            }

			OspPrintf( TRUE, FALSE, "\n--------Conf: %s data switch info--------\n", ptConfFullInfo->GetConfName() );
		    ptSwitchTable = g_cMcuVcApp.GetConfSwitchTable( byConfIdx );
			for( byLoop1 = 1; byLoop1 <= MAXNUM_DRI; byLoop1++ )
			{				
                // guzh [3/29/2007] ֧�ִ�ӡ���⽻��������Ϊ1����2���ʹ�ӡ������Ϣ(�㲥Դ/Rtcp)
				if( g_cMcuVcApp.IsMpConnected( byLoop1 ) ||
                    ( ( byType == 1 || byType == 2) &&
					  ( (bIsMiniMcu && byLoop1 == 1 && !g_cMcuVcApp.IsMpConnected( byLoop1 )) ||
                      ( ( byLoop1 == MCU_BOARD_MPC || byLoop1 == MCU_BOARD_MPCD ) && 
						( byLoop1 != byMpcBrdId && !bIsMiniMcu ) ) ) 
					)
                   )
				{
					OspPrintf( TRUE, FALSE, "\nMp%d(%s) switch info: ", byLoop1 , StrOfIP(g_cMcuVcApp.GetMpIpAddr(byLoop1)));
					for( wLoop2 = 0; wLoop2 < MAX_SWITCH_CHANNEL; wLoop2++ )
					{
						ptSwitchChannel = ptSwitchTable->GetSwitchChannel( byLoop1, wLoop2 );
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
                                wRcvPort = wBrdSrcRcvPort;
                            }

							byMpId = g_cMcuVcApp.FindMp( dwSrcIp );
							if( byMpId > 0 )
							{
								nLen = sprintf( achTemp, "\n[%d]  %s:%d(mp%d)", wLoop2, StrOfIP( dwSrcIp ), wRcvPort, byMpId );
							}
							else
							{
								nLen = sprintf( achTemp, "\n[%d]  %s:%d(src)", wLoop2, StrOfIP( dwSrcIp ), wRcvPort );
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
								OspPrintf( TRUE, FALSE, "%s\n", achTemp );
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
                                wDstPort = wBrdSrcRcvPort;
                            }

							if( byMpId > 0 )
							{
								nLen += sprintf( achTemp+nLen, "-->%s:%d(mp%d)", StrOfIP( dwDstIp ), wDstPort, byMpId );
							}
							else
							{
								nLen += sprintf( achTemp+nLen, "-->%s:%d(dst)", StrOfIP( dwDstIp ), wDstPort );
							}

							OspPrintf( TRUE, FALSE, achTemp );
						
						}
					}
					OspPrintf( TRUE, FALSE, "\n" );
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
		OspPrintf( TRUE, FALSE, "[GetMPSwitchInfo] Err: byMpId(%d) is invalid \n", byMpId );
		return;
	}
	if( FALSE == g_cMcuVcApp.IsMpConnected( byMpId ) )
	{
		OspPrintf( TRUE, FALSE, "[GetMPSwitchInfo] Err: byMpId(%d) is not connect \n", byMpId );
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
        
        ptConfFullInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConf;        
		if( ptConfFullInfo->m_tStatus.IsOngoing() )
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

// END OF FILE
