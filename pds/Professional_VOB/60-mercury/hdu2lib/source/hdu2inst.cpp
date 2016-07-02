/*****************************************************************************
   ģ����      : ������뿨
   �ļ���      : hdu2inst.cpp
   ����ļ�    : hdu2inst.h
   �ļ�ʵ�ֹ���: HDU2��ʵ��
   ����        : ��־��
   �汾        : V1.0  Copyright(C) 2011-2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2011/11/28  4.7         ��־��       ����
******************************************************************************/
#include "hdu2eventid.h"
#include "hdu2inst.h"

/*lint -save -e843*/
CHdu2App g_cHdu2App;
BOOL32 g_bPrintFrameInfo = FALSE;
extern u8     g_byHduPrintLevel;
/*=============================================================================
	����  : hdulog
	����  : ��ӡ
	����  : 
	���  : ��
	����  : ��
	ע    : 
  -----------------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���      �޸�����
	2011/11/28  4.7         ��־��        ����
=============================================================================*/
/*lint -save -e530 */
/*lint -save -esym(438, argptr)*/
/*lint -save -esym(526, __builtin_va_start)*/
/*lint -save -esym(628, __builtin_va_start)*/
void hdu2log( const u8 byLevel, const u16 wModule, const s8* pszFmt, ...)
{
	s8 achBuf[1024] = {0};
    va_list argptr;		      
    va_start( argptr, pszFmt );
	vsnprintf(achBuf , 1024, pszFmt, argptr);
	LogPrint( byLevel, wModule, achBuf );
	va_end(argptr); 
    return;
}


SEMHANDLE	g_hIpToStr = NULL;						// StrOfIPר��
/*=============================================================================
	����  : IpToStr
	����  : ת��IpΪ�ַ���
	����  : 
	���  : ��
	����  : ��
	ע    : 
  -----------------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���      �޸�����
	2013/09/03				�±�        ����
=============================================================================*/
s8 *IpToStr( u32 dwIP )
{
	if (NULL == g_hIpToStr)
	{
		if( !OspSemBCreate(&g_hIpToStr))
		{
			OspSemDelete( g_hIpToStr );
			g_hIpToStr = NULL;
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[StrOfIP] create g_hStrOfIP failed!\n" );
		}
	}
	
	if (NULL != g_hIpToStr)
	{
		OspSemTake(g_hIpToStr);
	}	
	
    dwIP = htonl(dwIP);
	static char strIP[17];  
	u8 *p = (u8 *)&dwIP;
	sprintf(strIP,"%d.%d.%d.%d%c",p[0],p[1],p[2],p[3],0);
	
	if (NULL != g_hIpToStr)
	{
		OspSemGive(g_hIpToStr);
	}
	
	return strIP;
}


/*=============================================================================
	����  : hdulog
	����  : ��ӡ
	����  : 
	���  : ��
	����  : ��
	ע    : 
  -----------------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���      �޸�����
	2013/03/11				�±�        ����
=============================================================================*/
void Hdu2Print( const u8 byLevel, const s8* pszFmt, ...)
{
	s8 achBuf[1024] = { 0 };
    va_list argptr;		      
    va_start( argptr, pszFmt );
	vsnprintf(achBuf, 1024, pszFmt, argptr);
    va_end(argptr); 
	
	if (g_byHduPrintLevel >= byLevel)
	{
		if ( byLevel <= LOG_LVL_DETAIL)
		{
			LogPrint(byLevel, MID_PUB_ALWAYS, achBuf);
		}
		else
		{
			OspPrintf(TRUE, FALSE, achBuf);
		}
	}

    return;
}
/*lint -restore*/

/*=============================================================================
  �� �� ���� CBVidRecvFrame    
  ��    �ܣ� �����յ�����Ƶ֡����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� PFRAMEHDR pFrmHdr, u32 dwContext
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2011/11/28  4.7         ��־��       ����
=============================================================================*/
static void CBVidRecvFrame(PFRAMEHDR pFrmHdr, u32 dwContext)
{
	//CHdu2ChnMgrGrp *pHdu2ChnMgrGrp = (CHdu2ChnMgrGrp*)dwContext;
	CRcvData *pcRcvData = (CRcvData *)dwContext;

    if ( NULL == pFrmHdr || NULL == pcRcvData )
    {
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[CBVidRecvFrame] PTFrameHeader 0x%x, dwContext 0x%x\n", pFrmHdr, dwContext);
        return ;
    }

	u8 byHduSubChnId = pcRcvData->m_byHduSubChnId;

	TFrameHeader tFrameHdr;
	memset(&tFrameHdr, 0x00, sizeof(tFrameHdr));
	tFrameHdr.m_dwMediaType = pFrmHdr->m_byMediaType;
	tFrameHdr.m_dwFrameID   = pFrmHdr->m_dwFrameID;
    tFrameHdr.m_dwSSRC      = pFrmHdr->m_dwSSRC;
	tFrameHdr.m_pData       = pFrmHdr->m_pData + pFrmHdr->m_dwPreBufSize; 
	tFrameHdr.m_dwDataSize  = pFrmHdr->m_dwDataSize;
	tFrameHdr.m_tVideoParam.m_bKeyFrame    = pFrmHdr->m_tVideoParam.m_bKeyFrame;
	tFrameHdr.m_tVideoParam.m_wVideoHeight = pFrmHdr->m_tVideoParam.m_wVideoHeight;
	tFrameHdr.m_tVideoParam.m_wVideoWidth  = pFrmHdr->m_tVideoParam.m_wVideoWidth;
	//pHdu2ChnMgrGrp->SetData(MODE_VIDEO, tFrameHdr);
	pcRcvData->m_pcHdu2ChnMgrGrp->SetData(MODE_VIDEO, tFrameHdr, byHduSubChnId);

	if (g_bPrintFrameInfo)
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CBVidRecvFrame] MediaType:%u, FrameID:%u, KF:%d, W*H:%d*%d, SSRC:%u, DataSize:%u\n",
			tFrameHdr.m_dwMediaType,tFrameHdr.m_dwFrameID,tFrameHdr.m_tVideoParam.m_bKeyFrame,tFrameHdr.m_tVideoParam.m_wVideoWidth,
			tFrameHdr.m_tVideoParam.m_wVideoHeight,tFrameHdr.m_dwSSRC,tFrameHdr.m_dwDataSize);
	}
	return;
}
/*=============================================================================
  �� �� ���� CBAudRecvFrame    
  ��    �ܣ� �����յ�����Ƶ֡����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� PFRAMEHDR pFrmHdr, u32 dwContext
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2011/11/28  4.7         ��־��       ����
=============================================================================*/
static void CBAudRecvFrame(PFRAMEHDR pFrmHdr, u32 dwContext)
{
	CHdu2ChnMgrGrp *pHdu2ChnMgrGrp = (CHdu2ChnMgrGrp*)dwContext;
    if ( NULL == pFrmHdr || NULL == pHdu2ChnMgrGrp )
    {
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[CBAudRecvFrame] PTFrameHeader 0x%x, dwContext 0x%x\n", pFrmHdr, dwContext);
        return ;
    }
	TFrameHeader tFrameHdr;
	memset(&tFrameHdr, 0x00, sizeof(tFrameHdr));
	tFrameHdr.m_dwMediaType = pFrmHdr->m_byMediaType;
	tFrameHdr.m_dwFrameID   = pFrmHdr->m_dwFrameID;
    tFrameHdr.m_dwSSRC      = pFrmHdr->m_dwSSRC;
	tFrameHdr.m_pData       = pFrmHdr->m_pData + pFrmHdr->m_dwPreBufSize; 
	tFrameHdr.m_dwDataSize  = pFrmHdr->m_dwDataSize;
    tFrameHdr.m_dwAudioMode = pFrmHdr->m_byAudioMode;
	pHdu2ChnMgrGrp->SetData(MODE_AUDIO,tFrameHdr);
	if (g_bPrintFrameInfo)
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CBAudRecvFrame] MediaType:%u, FrameID:%u, AudioMode:%u, SSRC:%u, DataSize:%u\n",
			tFrameHdr.m_dwMediaType,tFrameHdr.m_dwFrameID,tFrameHdr.m_dwAudioMode,tFrameHdr.m_dwSSRC,tFrameHdr.m_dwDataSize);
	}
	return;
}
/*=============================================================================
  �� �� ���� GetVidPlyPortTypeByChnAndType    
  ��    �ܣ� ��Ƶ���������ת���ӿ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wChnIdx
  �� �� ֵ�� u32
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2011/11/28  4.7         ��־��       ����
=============================================================================*/
u32 GetVidPlyPortTypeByChnAndType(u16 wChnIdx,u32 dwType)
{
	u32 dwVidPlyPortType = 0;
	switch(dwType) {
	case HDU_OUTPUT_YPbPr:
		dwVidPlyPortType = (wChnIdx == 0 ? VIDIO_YPbPr0 : VIDIO_YPbPr1);
		break;
	case HDU_OUTPUT_VGA:
		dwVidPlyPortType = (wChnIdx == 0 ? VIDIO_VGA0 : VIDIO_VGA1);
		break;
	case HDU_OUTPUT_DVI:
		dwVidPlyPortType = (wChnIdx == 0 ? VIDIO_DVI0 : VIDIO_DVI1);
		break;
	case HDU_OUTPUT_HDMI:
		dwVidPlyPortType = (wChnIdx == 0 ? VIDIO_HDMI0 : VIDIO_HDMI1);
		break;
	case HDU_OUTPUT_SDI:
		dwVidPlyPortType = (wChnIdx == 0 ? VIDIO_SDI0 : VIDIO_SDI1);
		break;
	case HDU_OUTPUT_C:
		dwVidPlyPortType = (wChnIdx == 0 ? VIDIO_C0 : VIDIO_C1);
		break;
	default:
		Hdu2Print(HDU_LVL_WARNING,  "[ProcChangeChnCfg]Unknown PortType!\n");
		return 0;
	}
	return dwVidPlyPortType;
}
/*=============================================================================
  �� �� ���� CheckChnCfgParam    
  ��    �ܣ� �������Ƿ�Ϸ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� THduModePort &tHdu2Cfg BOOL32 bIsHdu2_l
  �� �� ֵ�� BOOL32
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2013/02/01  4.7         ��־��       ����
=============================================================================*/
BOOL32 CheckChnCfgParam(THduModePort &tChnCfg,BOOL32 bIsHdu2_l)
{
	u8 byOutPortType = tChnCfg.GetOutPortType();
	u8 byOutModeType = tChnCfg.GetOutModeType();
	if (bIsHdu2_l)//HDU2_L
	{
		if (byOutPortType != HDU_OUTPUT_C)
		{
			tChnCfg.SetOutPortType(HDU_OUTPUT_C);
			tChnCfg.SetOutModeType(HDU_C_576i_50HZ);
			tChnCfg.SetZoomRate(HDU_ZOOMRATE_4_3);
			tChnCfg.SetScalingMode(HDU2_SHOWMODE_NONGEOMETRIC);
			Hdu2Print(HDU_LVL_WARNING,  "[CheckChnCfgParam]HDU2_L Only Support HDU_OUTPUT_C(%d) HDU_C_576i_50HZ(%d) OR HDU_C_480i_60HZ(%d)!!",
				HDU_OUTPUT_C,HDU_C_576i_50HZ,HDU_C_480i_60HZ);
			return FALSE;
		}
		if (tChnCfg.GetOutModeType() != HDU_C_576i_50HZ && tChnCfg.GetOutModeType() != HDU_C_480i_60HZ)
		{
			tChnCfg.SetOutModeType(HDU_C_576i_50HZ);
			tChnCfg.SetZoomRate(HDU_ZOOMRATE_4_3);
			tChnCfg.SetScalingMode(HDU2_SHOWMODE_NONGEOMETRIC);
			Hdu2Print(HDU_LVL_WARNING,  "[CheckChnCfgParam]HDU2_L Only Support HDU_OUTPUT_C(%d) HDU_C_576i_50HZ(%d) OR HDU_C_480i_60HZ(%d)!!",
				HDU_OUTPUT_C,HDU_C_576i_50HZ,HDU_C_480i_60HZ);
			return FALSE;
		}
	}
	else//HDU2
	{
		if (byOutPortType != HDU_OUTPUT_YPbPr && byOutPortType != HDU_OUTPUT_DVI &&
			byOutPortType != HDU_OUTPUT_HDMI && byOutPortType != HDU_OUTPUT_VGA &&
			byOutPortType != HDU_OUTPUT_SDI && byOutPortType != HDU_OUTPUT_C)
		{
			tChnCfg.SetOutPortType(HDU_OUTPUT_YPbPr);
			tChnCfg.SetOutModeType(HDU_YPbPr_1080i_50HZ);
			tChnCfg.SetZoomRate(HDU_ZOOMRATE_16_9);
			tChnCfg.SetScalingMode(HDU2_SHOWMODE_NONGEOMETRIC);
			Hdu2Print(HDU_LVL_WARNING,  "[CheckChnCfgParam]HDU2 Not support(%d)(%d)!!",tChnCfg.GetOutPortType(),byOutModeType);
			return FALSE;
		}
		if ((byOutPortType == HDU_OUTPUT_YPbPr || byOutPortType == HDU_OUTPUT_DVI ||
			byOutPortType == HDU_OUTPUT_HDMI || byOutPortType == HDU_OUTPUT_SDI) && 
			byOutModeType != HDU_YPbPr_1080P_24fps && byOutModeType != HDU_YPbPr_1080P_25fps && 
			byOutModeType != HDU_YPbPr_1080P_29970HZ && byOutModeType != HDU_YPbPr_1080P_30fps && 
			byOutModeType != HDU_YPbPr_1080P_50fps && byOutModeType != HDU_YPbPr_1080P_59940HZ && 
			byOutModeType != HDU_YPbPr_1080P_60fps&& byOutModeType != HDU_YPbPr_1080i_50HZ && 
			byOutModeType != HDU_YPbPr_1080i_60HZ&& byOutModeType != HDU_YPbPr_720P_50fps && 
			byOutModeType != HDU_YPbPr_720P_60fps && byOutModeType != HDU_YPbPr_576i_50HZ && 
			byOutModeType != HDU_YPbPr_480i_60HZ)
		{
			tChnCfg.SetOutModeType(HDU_YPbPr_1080i_50HZ);
			tChnCfg.SetZoomRate(HDU_ZOOMRATE_16_9);
			tChnCfg.SetScalingMode(HDU2_SHOWMODE_NONGEOMETRIC);
			Hdu2Print(HDU_LVL_WARNING,  "[CheckChnCfgParam]HDU2 Not support(%d)(%d)!!",tChnCfg.GetOutPortType(),byOutModeType);
			return FALSE;
		}
		if (byOutPortType == HDU_OUTPUT_VGA &&
			byOutModeType != HDU_VGA_SXGA_60HZ && byOutModeType != HDU_VGA_XGA_60HZ  &&
			byOutModeType != HDU_VGA_XGA_75HZ  && byOutModeType != HDU_VGA_SVGA_60HZ &&
			byOutModeType != HDU_VGA_SVGA_75HZ && byOutModeType != HDU_VGA_VGA_60HZ  &&
			byOutModeType != HDU_VGA_VGA_75HZ  && byOutModeType != HDU_VGA_SXGA_75HZ &&
			byOutModeType != HDU_VGA_WXGA_1280_768_60HZ  &&
			byOutModeType != HDU_VGA_WXGA_1280_800_60HZ  && 
			byOutModeType != HDU_VGA_WSXGA_60HZ          &&
			byOutModeType != HDU_VGA_SXGAPlus_60HZ       && 
			byOutModeType != HDU_VGA_UXGA_60HZ           &&
			byOutModeType != HDU_WXGA_1280_800_75HZ)
		{
			tChnCfg.SetOutModeType(HDU_VGA_XGA_60HZ);
			tChnCfg.SetZoomRate(HDU_ZOOMRATE_4_3);
			tChnCfg.SetScalingMode(HDU2_SHOWMODE_NONGEOMETRIC);
			Hdu2Print(HDU_LVL_WARNING,  "[CheckChnCfgParam]HDU2 Not support(%d)(%d)!!",tChnCfg.GetOutPortType(),byOutModeType);
			return FALSE;
		}
		//֧��HDU2-S
		if (byOutPortType == HDU_OUTPUT_C && 
			tChnCfg.GetOutModeType() != HDU_C_576i_50HZ && 
			tChnCfg.GetOutModeType() != HDU_C_480i_60HZ)
		{
			tChnCfg.SetOutModeType(HDU_C_576i_50HZ);
			tChnCfg.SetZoomRate(HDU_ZOOMRATE_4_3);
			tChnCfg.SetScalingMode(HDU2_SHOWMODE_NONGEOMETRIC);
			Hdu2Print(HDU_LVL_WARNING,  "[CheckChnCfgParam]HDU2-S Not support(%d)(%d)!!",tChnCfg.GetOutPortType(),byOutModeType);
			return FALSE;
		}
	}
	return TRUE;
}
/* --------------------CHdu2ChnMgrGrp���ʵ��  start-----------------------*/
/*====================================================================
����  : Create
����  : ����KdvMediaRcv,KdvVidDec,KdvAudDec
����  : u16 wChnId
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��       ����
2013/03/11   4.7.2       �±�		  �޸�(HDU֧�ֶ໭��)
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::Create(u16 wChnId,BOOL32 bIsHdu2_s)
{
	u16 wRet = 0;
	TVidDecHD3RD tVidDecInit = {0};
	tVidDecInit.dwChnID = wChnId;
	// [2013/03/11 chenbing] 
	tVidDecInit.dwReserved = HDU2_VMP;
	tVidDecInit.dwPlyStd = VID_OUTTYPE_1080P60;
	if (wChnId == 0)
	{
		tVidDecInit.dwPlyPort = VIDIO_HDMI0;
	}
	else if (wChnId == 1) 
	{
		tVidDecInit.dwPlyPort = VIDIO_HDMI1;
	}
	else
	{
		Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::Create]Unknown  PlayPortType!\n");
		return FALSE;
	}

	// [2013/03/11 chenbing] ����һ����Ƶ������ 
	wRet = m_cVidDecoder.CreateHD3(&tVidDecInit);
	if((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::Create]VDecoder:CreateDecoder Failed!Param(ChnId:%d),error:%d.\n",wChnId,wRet);
		return FALSE;
	}
	wRet = m_cVidDecoder.SetVidPlayPolicy(5, (u32)EN_PLAY_BMP);
	if((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::Create]VDecoder:SetVidPlayPolicy Failed!So Return!!\n");
		return FALSE;
	}
	TDecoder tDecoder;
	memset(&tDecoder, 0, sizeof(TDecoder));
	tDecoder.dwChnID  = wChnId;

	// [2013/03/11 chenbing] ����һ����Ƶ������ 
	if( bIsHdu2_s )
	{
		wRet = m_cAudDecoder.CreateDecoder(&tDecoder, (u32)en_HDU2_Board_S);
	}
	else
	{
		wRet = m_cAudDecoder.CreateDecoder(&tDecoder, (u32)en_HDU2_Board);
	}
	if((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::Create]ADecoder:CreateDecoder Failed!Param(ChnId:%d),error:%d.\n",wChnId,wRet);
		return FALSE;
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::Create]ADecoder:CreateDecoder Successed!\n");
	}

	// [2013/03/11 chenbing]  
	for (u8 byIndex=0; byIndex<HDU_MODEFOUR_MAX_SUBCHNNUM; byIndex++)
	{
		m_acRcvCB[byIndex].m_byHduSubChnId	 = byIndex;
		m_acRcvCB[byIndex].m_pcHdu2ChnMgrGrp = this;
		// �����ĸ���Ƶ���ն��� 
		wRet = m_acVidMediaRcv[byIndex].Create(MAX_VIDEO_FRAME_SIZE, CBVidRecvFrame, (u32)&m_acRcvCB[byIndex]);
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::Create]VidMediaRcv:Create Failed!error:%d.\n", wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::Create]VidMediaRcv[%d]:Create Successed!\n", byIndex);
		}

		m_acVidMediaRcv[byIndex].SetHDFlag(TRUE);
	} //for u8 byIndex=0 over	
	
	// ����һ����Ƶ���ն��� 
 	wRet = m_cAudMediaRcv.Create(MAX_AUDIO_FRAME_SIZE, CBAudRecvFrame, (u32)this);
	if(MEDIANET_NO_ERROR != wRet)
	{
		Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::Create]AudMediaRcv:Create Failed!error:%d.\n", wRet);
		return FALSE;
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::Create]AudMediaRcv:Create Successed!\n");
	}

	return TRUE;
}
/*====================================================================
����  : StartDecode
����  : ��������Ƶ������
����  : u8 byMode        //ģʽ
���  : ��
����  : BOOL32
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::StartDecode(u8 byMode/* = MODE_BOTH */)
{
	if (MODE_NONE == byMode)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::StartDecode]byMode == MODE_NONE\n");
		return FALSE;
	}

	u8 byOldMode = MODE_NONE;
	for (u8 bySubChnId = 0; bySubChnId < HDU_MODEFOUR_MAX_SUBCHNNUM; bySubChnId++)
	{
		byOldMode = GetMode(bySubChnId);
		if (MODE_NONE != byOldMode)
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StartDecode] SubChnId(%d) OldMode(%d)\n", bySubChnId, byOldMode);
			break;
		}
	}

	u16 wRet = (u16)Codec_Success;
	Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StartDecode] OldMode(%d)\n", GetMode());
//	if ((MODE_NONE == GetMode() || MODE_VIDEO == GetMode() ) && (MODE_AUDIO == byMode || MODE_BOTH == byMode))
	if( MODE_AUDIO == byMode || MODE_BOTH == byMode )
	{
		// ������Ƶǰ��Ҫ��ֹͣ��Ƶ���룬��ֹ������ʱ������� 
		wRet = m_cAudDecoder.StopDec();
		if((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::StopDecode]m_cAudDecoder.StaopDec() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StopDecode]m_cAudDecoder.StopDec() Successed!\n");
		}

		if (m_tAudDp.GetRealPayLoad() == MEDIA_TYPE_AACLC || m_tAudDp.GetRealPayLoad() == MEDIA_TYPE_AACLD)
		{
			TAudioDecParam tAudParam = {0};
			tAudParam.m_dwSamRate = (u32)SAMPLE_RATE32000;
			tAudParam.m_dwMediaType = m_tAudDp.GetRealPayLoad();
			tAudParam.m_dwChannelNum = GetAudChnNum();
			m_cAudDecoder.SetAudioDecParam( tAudParam );
			if((u16)Codec_Success != wRet)
			{
				Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::StartDecode]m_cAudDecoder.SetAudioDecParam() Failed,Error:%d\n",wRet);
			}
			else
			{
				Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StartDecode]m_cAudDecoder.SetAudioDecParam(MediaType: %d AudChnNum: %d) Successed!\n",
					tAudParam.m_dwMediaType, tAudParam.m_dwChannelNum);
			}
		}

		wRet = m_cAudDecoder.StartDec();
		if((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::StartDecode]m_cAudDecoder.StartDec() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StartDecode]m_cAudDecoder.StartDec() Successed!\n");
		}
	}

	if ((MODE_NONE == byOldMode || MODE_AUDIO == byOldMode ) && (MODE_VIDEO == byMode || MODE_BOTH == byMode))
	{
		wRet = m_cVidDecoder.StartDec();
		if((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::StartDecode]m_cVidDecoder.StartDec() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StartDecode]m_cVidDecoder.StartDec() Successed!\n");
		}
	}

	return TRUE;
}
/*====================================================================
����  : StopDecode
����  : ֹͣ����Ƶ������
����  : u8 byMode        //ģʽ
���  : ��
����  : BOOL32
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::StopDecode(u8 byMode/* = MODE_BOTH */ )
{
	if (byMode == MODE_NONE)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::StopDecode]buCurMode == MODE_NONE\n");
		return FALSE;
	}

	u16 wRet = (u16)Codec_Success;
	if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
	{
		wRet = m_cAudDecoder.StopDec();
		if((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::StopDecode]m_cAudDecoder.StopDec() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StopDecode]m_cAudDecoder.StopDec() Successed!\n");
		}
	}
	if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
	{
		wRet = m_cVidDecoder.StopDec();
		if((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::StopDecode]m_cVidDecoder.StopDec() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StopDecode]m_cVidDecoder.StopDec() Successed!\n");
		}
	}

	return TRUE;
}
/*====================================================================
����  : StartNetRecv
����  : ֹͣ����Ƶ����
����  : u8 byMode        //ģʽ
���  : ��
����  : BOOL32
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
2013/03/11   4.7.2       �±�		   �޸�(HDU֧�ֶ໭��)
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::StartNetRecv(u8 byMode/* = MODE_BOTH */, u8 byHduSubChnId)
{
	u16 wRet = MEDIANET_NO_ERROR;
	u8 byOldMode = GetMode(byHduSubChnId);
	if ((MODE_NONE == byOldMode || MODE_VIDEO == byOldMode) && (MODE_AUDIO == byMode || MODE_BOTH == byMode) )
	{
		wRet = m_cAudMediaRcv.StartRcv();
		if((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::StartNetRecv]m_cAudMediaRcv.StartRcv() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StartNetRecv]m_cAudMediaRcv.StartRcv() Successed!\n");
		}
	}

	if ((MODE_NONE == byOldMode || MODE_AUDIO == byOldMode) && (MODE_VIDEO == byMode || MODE_BOTH == byMode) )
	{
		wRet = m_acVidMediaRcv[byHduSubChnId].StartRcv();
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::StartNetRecv]m_acVidMediaRcv.StartRcv() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StartNetRecv]m_acVidMediaRcv.StartRcv() Successed!\n");
		}
	}
	return TRUE;
}
/*====================================================================
����  : StopDecode
����  : ֹͣ����Ƶ����
����  : u8 byMode        //ģʽ
���  : ��
����  : BOOL
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
2013/03/11   4.7.2       �±�		   �޸�(HDU֧�ֶ໭��)
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::StopNetRecv(u8 byMode/* = MODE_BOTH */, u8 byHduSubChnId)
{
	u16 wRet = MEDIANET_NO_ERROR;

	if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
	{
		wRet = m_cAudMediaRcv.StopRcv();
		if((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::StopNetRecv]m_cAudMediaRcv.StopRcv() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StopNetRecv]m_cAudMediaRcv.StopRcv() Successed!\n");
		}
	}

	if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
	{
		wRet = m_acVidMediaRcv[byHduSubChnId].StopRcv();
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::StopNetRecv]m_acVidMediaRcv.StopRcv() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StopNetRecv]m_acVidMediaRcv.StopRcv() Successed!\n");
		}
	}
	return TRUE;
}
/*====================================================================
����  : SetVidLocalNetParam
����  : ������Ƶ���ղ�����Ϣ�������ø���Ӧ��Ƶ��������Ϊ�գ������
����  : TLocalNetParam *ptVidLocalNetParam  �����������
���  : ��
����  : BOOL32
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
2013/03/11   4.7.2       �±�		   �޸�(HDU֧�ֶ໭��)
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetVidLocalNetParam(TLocalNetParam *ptVidLocalNetParam, u8 byHduSubChnId)
{
	u16 wRet = MEDIANET_NO_ERROR;
	if (ptVidLocalNetParam == NULL)
	{
		memset( &m_atVidLocalNetParam[byHduSubChnId], 0, sizeof(TLocalNetParam) );
		wRet = m_acVidMediaRcv[byHduSubChnId].RemoveNetRcvLocalParam();
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetVidLocalNetParam]m_acVidMediaRcv.RemoveNetRcvLocalParam() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_KEYSTATUS,  "[CHdu2ChnMgrGrp::SetVidLocalNetParam]m_acVidMediaRcv.RemoveNetRcvLocalParam() Successed!\n");
		}
	}
	else
	{
		memcpy( &m_atVidLocalNetParam[byHduSubChnId], ptVidLocalNetParam, sizeof(TLocalNetParam) );
		wRet = m_acVidMediaRcv[byHduSubChnId].SetNetRcvLocalParam(m_atVidLocalNetParam[byHduSubChnId]);
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::SetVidLocalNetParam]m_acVidMediaRcv[byHduSubChnId].SetNetRcvLocalParam() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_KEYSTATUS,  "[CHdu2ChnMgrGrp::SetVidLocalNetParam]m_acVidMediaRcv[byHduSubChnId].SetNetRcvLocalParam() Successed!\n");
		}
	}
	return TRUE;
}
/*====================================================================
����  : SetAudLocalNetParam
����  : ������Ƶ���ղ�����Ϣ�������ø���Ӧ��Ƶ��������Ϊ�գ������
����  : TLocalNetParam *ptVidLocalNetParam  �����������
���  : ��
����  : BOOL32
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetAudLocalNetParam(TLocalNetParam *ptAudLocalNetParam)
{
	u16 wRet = MEDIANET_NO_ERROR;	
	if (ptAudLocalNetParam == NULL)
	{
		memset( &m_tAudLocalNetParam, 0, sizeof(TLocalNetParam) );
		wRet = m_cAudMediaRcv.RemoveNetRcvLocalParam();
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetAudLocalNetParam]m_cAudMediaRcv.RemoveNetRcvLocalParam() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_KEYSTATUS,  "[CHdu2ChnMgrGrp::SetAudLocalNetParam]m_cAudMediaRcv.RemoveNetRcvLocalParam() Successed!\n");
		}
	}
	else
	{
		memcpy( &m_tAudLocalNetParam, ptAudLocalNetParam, sizeof(TLocalNetParam) );
		wRet = m_cAudMediaRcv.SetNetRcvLocalParam(m_tAudLocalNetParam);
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetAudLocalNetParam]m_cAudMediaRcv.SetNetRcvLocalParam() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_KEYSTATUS,  "[CHdu2ChnMgrGrp::SetAudLocalNetParam]m_cAudMediaRcv.SetNetRcvLocalParam() Successed!\n");
		}
	}
	return TRUE;
}
/*====================================================================
����  : SetNetRecvRsParam
����  : ���ö����ش�����
����  : u8 byMode  ģʽ
		BOOL32 bRepeatSnd �Ƿ��������ش�����
���  : ��
����  : BOOL32
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
2013/03/11   4.7.2       �±�		   �޸�(HDU֧�ֶ໭��)
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetNetRecvRsParam(u8 byMode, BOOL32 bRepeatSnd, u8 byHduSubChnId)
{
	u16 wRet = MEDIANET_NO_ERROR;
	TRSParam tRsParam;
	memset(&tRsParam, 0x0, sizeof(tRsParam));
	tRsParam.m_wFirstTimeSpan  = htons(g_cHdu2App.GetPrsTimeSpan().m_wFirstTimeSpan); 
	tRsParam.m_wSecondTimeSpan = htons(g_cHdu2App.GetPrsTimeSpan().m_wSecondTimeSpan); 
	tRsParam.m_wThirdTimeSpan  = htons(g_cHdu2App.GetPrsTimeSpan().m_wThirdTimeSpan); 
	tRsParam.m_wRejectTimeSpan = htons(g_cHdu2App.GetPrsTimeSpan().m_wRejectTimeSpan); 
	if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
	{
		wRet = m_cAudMediaRcv.ResetRSFlag(tRsParam, bRepeatSnd);
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetNetRecvRsParam]m_cAudMediaRcv.ResetRSFlag() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetNetRecvRsParam]m_cAudMediaRcv.ResetRSFlag() Successed!\n");
		}
	}
	if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
	{
		wRet = m_acVidMediaRcv[byHduSubChnId].ResetRSFlag(tRsParam, bRepeatSnd);
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetNetRecvRsParam]m_acVidMediaRcv.ResetRSFlag() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetNetRecvRsParam]m_acVidMediaRcv.ResetRSFlag() Successed!\n");
		}
	}
	return TRUE;
}
/*====================================================================
����  : SetActivePT
����  : ���ö�̬�غ�
����  : u8 byMode  ģʽ
        u8 byHduSubChnId:��ͨ��
���  : ��
����  : BOOL32
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
2013/03/11   4.7.2       �±�		   �޸�(HDU֧�ֶ໭��)
====================================================================*/
BOOL32  CHdu2ChnMgrGrp::SetActivePT(u8 byMode,BOOL32 bIsVidAutoAjust /*= TRUE */, u8 byHduSubChnId)
{
	u16 wRet = MEDIANET_NO_ERROR;
	if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
	{
		wRet = m_cAudMediaRcv.SetActivePT(m_tAudDp.GetActivePayload(), m_tAudDp.GetRealPayLoad());
		if((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetActivePT]m_cAudMediaRcv.SetActivePT() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetActivePT]m_cAudMediaRcv.SetActivePT() Successed!\n");
		}
	}
	if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
	{
		if (bIsVidAutoAjust)
		{
			//���ö�̬�غɲ���Ϊ����Ӧ������Ƶ��ʽ�����仯���������Ӧ����
			wRet = m_acVidMediaRcv[byHduSubChnId].SetActivePT(MEDIA_TYPE_H264, MEDIA_TYPE_H264);
		}
		else
		{
			wRet = m_acVidMediaRcv[byHduSubChnId].SetActivePT(m_atVidDp[byHduSubChnId].GetActivePayload(), m_atVidDp[byHduSubChnId].GetRealPayLoad());
		}
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetActivePT]m_acVidMediaRcv.SetActivePT() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetActivePT]m_acVidMediaRcv.SetActivePT() Successed!\n");
		}
	}
	return TRUE;
}
/*====================================================================
����  : SetDecryptKey
����  : ���ý��ս��ܲ���
����  : u8 byMode  ģʽ
		u8 byHduSubChnId:��ͨ��
���  : ��
����  : BOOL32
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
2013/03/11   4.7.2       �±�		   �޸�(HDU֧�ֶ໭��)
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetDecryptKey(u8 byMode, u8 byHduSubChnId)
{
	u16 wRet = 0;
	u8  byDecryptMode = m_tMediaDec.GetEncryptMode();
	u8  abyKeyBuf[MAXLEN_KEY];
	s32 nKeySize = 0;

	if (byDecryptMode == CONF_ENCRYPTMODE_NONE)
	{
		if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
		{
			wRet = m_cAudMediaRcv.SetDecryptKey((s8*)NULL, (u16)0, byDecryptMode);
			if(MEDIANET_NO_ERROR != wRet)
			{
				Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetDecryptKey]m_cAudMediaRcv.SetDecryptKey() Failed,Error:%d\n",wRet);
				return FALSE;
			}
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetDecryptKey]m_cAudMediaRcv.SetDecryptKey() Successed!\n");
		}
		if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
		{
			wRet = m_acVidMediaRcv[byHduSubChnId].SetDecryptKey((s8*)NULL, (u16)0, byDecryptMode);
			if(MEDIANET_NO_ERROR != wRet)
			{
				Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetDecryptKey]m_acVidMediaRcv.SetDecryptKey() Failed,Error:%d\n",wRet);
				return FALSE;
			}
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetDecryptKey]m_acVidMediaRcv.SetDecryptKey() Successed!\n");
		}
		return TRUE;
	}
	//�ϲ����²�ı����ģʽ��һ��
	if (byDecryptMode == CONF_ENCRYPTMODE_DES)
	{
		byDecryptMode = DES_ENCRYPT_MODE;
	}
	else if (byDecryptMode == CONF_ENCRYPTMODE_AES)
	{
		byDecryptMode = AES_ENCRYPT_MODE;
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetDecryptKey]Unknown DecryptMode(%d),So return!\n",byDecryptMode);
		return FALSE;
	}
	m_tMediaDec.GetEncryptKey(abyKeyBuf,&nKeySize);
	if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
	{
		wRet = m_cAudMediaRcv.SetDecryptKey((s8*)abyKeyBuf, nKeySize, byDecryptMode);
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetDecryptKey]m_cAudMediaRcv.SetDecryptKey() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetDecryptKey]m_cAudMediaRcv.SetDecryptKey() Successed!\n");
		}
	}
	if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
	{
		wRet = m_acVidMediaRcv[byHduSubChnId].SetDecryptKey((s8*)abyKeyBuf, nKeySize, byDecryptMode);
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetDecryptKey]m_acVidMediaRcv.SetDecryptKey() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetDecryptKey]m_acVidMediaRcv.SetDecryptKey() Successed!\n");
		}
	}
	return TRUE;
}
/*====================================================================
����  : SetVidPlyPortType
����  : ������Ƶ���Ŷ˿�����
����  : u32 dwType   ���Ŷ˿�����
���  : ��
����  : BOOL32
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetVidPlyPortType(u32 dwType)
{
    u16 wRet = m_cVidDecoder.SetVidPlyPortType(dwType);
	if ((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetVidPlyPortType]m_cVidDecoder.SetVidPlyPortType() Failed,Error:%d\n",wRet);
		return FALSE;
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetVidPlyPortType]m_cVidDecoder.SetVidPlyPortType() Successed!\n");
	}
	return TRUE;
}
/*====================================================================
����  : SetVideoPlyInfo
����  : ������ƵԴ��Ϣ
����  : TVidSrcInfo* ptInfo ��ƵԴ��Ϣ
���  : ��
����  : BOOL32
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetVideoPlyInfo(TVidSrcInfo* ptInfo)
{
    u16 wRet = m_cVidDecoder.SetVideoPlyInfo(ptInfo);
	if ((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetVideoPlyInfo]m_cVidDecoder.SetVideoPlyInfo() Failed,Error:%d\n",wRet);
		return FALSE;
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetVideoPlyInfo]m_cVidDecoder.SetVideoPlyInfo() Successed!\n");
	}
	return TRUE;
}

/*====================================================================
����  : SetVidPIPParam
����  : HDUͨ��ģʽ�л�
����  : 
���  : ��
����  : BOOL32
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2013/03/11	 4.7.2		 �±�		  ����(HDU�໭��֧��)
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetVidPIPParam(const u8 byHduVmpMode)
{
	enVideoPIPIndex enIndex[3];
	memset(enIndex, 0, sizeof(enIndex));

	if ( byHduVmpMode == HDUCHN_MODE_FOUR )
	{
		u16 wRet = 0;
		m_cVidDecoder.SetVidPIPParam(VIDEO_PIPMODE_FOUR, enIndex);
		if ((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetVidPIPParam]m_cVidDecoder.SetVidPIPParam(%d) Failed, Error:%d\n", byHduVmpMode, wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetVidPIPParam]m_cVidDecoder.SetVidPIPParam(%d) Successed!\n", byHduVmpMode);
		}
	}
	else if (byHduVmpMode == HDUCHN_MODE_ONE)
	{
		u16 wRet = 0;
		m_cVidDecoder.SetVidPIPParam(VIDEO_PIPMODE_ONE, enIndex);
		if ((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetVidPIPParam]m_cVidDecoder.SetVidPIPParam(%d) Failed, Error:%d\n", byHduVmpMode, wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetVidPIPParam]m_cVidDecoder.SetVidPIPParam(%d) Successed!\n", byHduVmpMode);
		}
	}
	else
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetVidPIPParam]m_cVidDecoder.SetVidPIPParam(%d) byHduVmpMode Failed, Error\n", byHduVmpMode);
		return FALSE;
	}

	return TRUE;
}

/*====================================================================
����  : SetPlayScales
����  : ������Ƶ���ű���
����  : u16 wWidth  ��
		u16 wHeigh  ��
���  : ��
����  : BOOL32
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetPlayScales( u16 wWidth, u16 wHeigh )
{
    u16 wRet = m_cVidDecoder.SetPlayScale(wWidth, wHeigh);
	if ((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetPlayScales]m_cVidDecoder.SetPlayScales() Failed,Error:%d\n",wRet);
		return FALSE;
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetPlayScales]m_cVidDecoder.SetPlayScales() Successed!\n");
	}
	return TRUE;
}
/*====================================================================
����  : SetVidDecResizeMode
����  : ������ʾ����
����  : u16 nMode ��ʾ����
���  : ��
����  : BOOL32
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetVidDecResizeMode(u16 nMode)
{
	u8 byDecReszeMode = 0;
	switch(nMode) 
	{
		case HDU2_SHOWMODE_NONGEOMETRIC:
			byDecReszeMode = (u8)EN_ZOOM_SCALE;
			break;
		case HDU2_SHOWMODE_CUTEDGEGEOME:
			byDecReszeMode = (u8)EN_ZOOM_CUT;
			break;
		case HDU2_SHOWMODE_BLACKEDGEGEOME:
			byDecReszeMode = (u8)EN_ZOOM_FILLBLACK;
			break;
		default:
			return FALSE;
	}
    u16 wRet = m_cVidDecoder.SetVidDecResizeMode(byDecReszeMode);
	if ((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetVidDecResizeMode]m_cVidDecoder.SetVidDecResizeMode(%d) Failed,Error:%d\n",byDecReszeMode,wRet);
		return FALSE;
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetVidDecResizeMode]m_cVidDecoder.SetVidDecResizeMode(%d) Successed!\n",byDecReszeMode);
	}
	return TRUE;
}
/*====================================================================
����  : GetVidDecoderStatus
����  : ��ȡ��Ƶ������״̬
����  : u8 byMode
		TKdvDecStatus &tKdvDecStatus
���  : ��Ƶ������״̬
����  : BOOL32
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2011/11/28  4.7         ��־��        ����
20130529	4.7         �±�		  ����
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::GetDecoderStatus(u8 byMode ,TKdvDecStatus &tKdvDecStatus, u8 byHduSubChnId)
{
// 	if (byMode != MODE_AUDIO && byMode != MODE_VIDEO)
// 	{
// 		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::GetDecoderStatus]Mode(%d) Is Invalide\n",byMode);
// 		return FALSE;
// 	}
// 	u16 wRet = 0;
// 	if (byMode == MODE_AUDIO)
// 	{
// 		wRet = m_cAudDecoder.GetDecoderStatus(tKdvDecStatus);
// 		if ((u16)Codec_Success != wRet)
// 		{
// 			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::GetDecoderStatus]m_cAudDecoder.GetDecoderStatus() Failed,Error:%d\n",wRet);
// 			return FALSE;
// 		}
// 		else
// 		{
// 			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::GetDecoderStatus]m_cAudDecoder.GetDecoderStatus() Successed!\n");
// 		}
// 	}
// 	if (byMode == MODE_VIDEO)
// 	{
// 		wRet = m_cVidDecoder.GetDecoderStatus(tKdvDecStatus, byHduSubChnId);
// 		if ((u16)Codec_Success != wRet)
// 		{
// 			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::GetDecoderStatus]m_cVidDecoder.GetDecoderStatus() Failed,Error:%d\n",wRet);
// 			return FALSE;
// 		}
// 		else
// 		{
// 			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::GetDecoderStatis]m_cVidDecoder.GetDecoderStatus() Successed!\n");
// 		}
// 	}
	return TRUE;
}
/*====================================================================
����  : GetVidDecoderStatis
����  : ��ȡ��Ƶ��������ͳ����Ϣ
����  : u8 byMode
		TKdvDecStatis &tKdvDecStatis
���  : ��Ƶ��������ͳ����Ϣ
����  : BOOL32
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2011/11/28  4.7         ��־��        ����
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::GetDecoderStatis(u8 byMode ,TKdvDecStatis &tKdvDecStatis, u8 byHduSubChnId)
{
	if (byMode != MODE_AUDIO && byMode != MODE_VIDEO)
	{
		Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::GetDecoderStatis]Mode(%d) Is Invalide\n",byMode);
		return FALSE;
	}
	u16 wRet = 0;
	if (byMode == MODE_AUDIO)
	{
		wRet = m_cAudDecoder.GetDecoderStatis(tKdvDecStatis);
		if ((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::GetDecoderStatis]m_cAudDecoder.GetDecoderStatis() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_GENERAL,  "[CHdu2ChnMgrGrp::GetDecoderStatis]m_cAudDecoder.GetDecoderStatis() Successed!\n");
		}
	}
	if (byMode == MODE_VIDEO)
	{
		wRet = m_cVidDecoder.GetDecoderStatis(tKdvDecStatis, byHduSubChnId);
		if ((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::GetDecoderStatis]m_cVidDecoder.GetDecoderStatis() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
		  Hdu2Print(HDU_LVL_GENERAL,  "[CHdu2ChnMgrGrp::GetDecoderStatis]m_cVidDecoder.GetDecoderStatis() Successed!\n");
		}
	}
	return TRUE;
}
/*====================================================================
����  : SetData
����  : Ϊ��������������
����  : u8 byMode
		TFrameHeader tFrameHdr
���  : 
����  : BOOL32
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2011/11/28  4.7         ��־��        ����
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetData(u8 byMode, TFrameHeader tFrameHdr, u8 byHduSubChnId)
{
	if (byMode != MODE_AUDIO && byMode != MODE_VIDEO)
	{
		Hdu2Print(HDU_LVL_GENERAL,  "[CHdu2ChnMgrGrp::SetData]Mode(%d) Is Invalide\n",byMode);
		return FALSE;
	}
	u16 wRet = 0;
	if (byMode == MODE_AUDIO)
	{
		wRet = m_cAudDecoder.SetData(tFrameHdr);
		if ((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::SetData]m_cAudDecoder.SetData() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_GENERAL,  "[CHdu2ChnMgrGrp::SetData]m_cAudDecoder.SetData() Successed!\n");
		}
	}
	if (byMode == MODE_VIDEO)
	{
		wRet = m_cVidDecoder.SetData(tFrameHdr, byHduSubChnId);
		if ((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::SetData]m_cVidDecoder.SetData[SubChnId](%d) Failed,Error:%d\n",byHduSubChnId, wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_GENERAL,  "[CHdu2ChnMgrGrp::SetData]m_cVidDecoder.SetData[SubChnId](%d) Successed!\n", byHduSubChnId);
		}
	}
	return TRUE;
}
/*====================================================================
����  : SetAudOutPort
����  : ������Ƶ���Ŷ˿����� 
����  : u32 dwType
���  : ��
����  : BOOL32
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2011/11/28  4.7         ��־��        ����
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetAudOutPort(u32 dwAudPort)
{
    u16 wRet = (u16)Codec_Success;
	wRet = m_cAudDecoder.SetAudOutPort(dwAudPort);
	if ((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetAudOutPort]m_cAudDecoder.SetAudOutPort() Failed,Error:%d\n",wRet);
		return FALSE;
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetAudOutPort]m_cAudDecoder.SetAudOutPort() Successed!\n");
	}
	return TRUE;
}
/*====================================================================
����  : SetVolume
����  : ���������������
����  : u8 byVolume
���  : ��
����  : BOOL32
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2011/11/28  4.7         ��־��        ����
====================================================================*/
BOOL32  CHdu2ChnMgrGrp::SetVolume(u8 byVolume )
{
    u16 wRet = (u16)Codec_Success;
	wRet = m_cAudDecoder.SetAudioVolume(byVolume);
	if ((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetVolume]m_cAudDecoder.SetAudioVolume() Failed,Error:%d\n",wRet);
		return FALSE;
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetVolume]m_cAudDecoder.SetAudioVolume() Successed!\n");
	}
	m_byVolume = byVolume;
	return TRUE;
} 
/*====================================================================
����  : GetVolume
����  : �õ������������
����  : u8 byVolume
���  : ��
����  : u8
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2011/11/28  4.7         ��־��        ����
====================================================================*/
u8  CHdu2ChnMgrGrp::GetVolume()
{
	u8 byVolume = 0;
    u16 wRet = m_cAudDecoder.GetAudioVolume(byVolume);
	if ((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::GetVolume]m_cAudDecoder.GetAudioVolume() Failed,Error:%d\n",wRet);
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::GetVolume]m_cAudDecoder.GetAudioVolume() Successed!\n");
	}
	if (byVolume != m_byVolume)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::GetVolume]m_cAudDecoder.GetAudioVolume() m_byVolume(%d) byVolume(%d)\n",m_byVolume,byVolume);
	}
	return m_byVolume;
}
/*====================================================================
����  : SetIsMute
����  : ���þ���
����  : BOOL32 bMute
���  : ��
����  : u16
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2011/11/28  4.7         ��־��        ����
====================================================================*/
BOOL32  CHdu2ChnMgrGrp::SetIsMute(BOOL32 bMute)
{
    u16 wRet = (u16)Codec_Success;
	wRet = m_cAudDecoder.SetAudioMute(bMute);
	if ((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetIsMute]m_cAudDecoder.SetAudioMute() Failed,Error:%d\n",wRet);
		return FALSE;
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetIsMute]m_cAudDecoder.SetAudioMute() Successed!\n");
	}
	m_bIsMute = bMute;
	return TRUE;
}
/*====================================================================
����  : GetIsMute
����  : ��ȡͨ���Ƿ���
����  : 
���  : ��
����  : u16
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2011/11/28  4.7         ��־��        ����
====================================================================*/
BOOL32  CHdu2ChnMgrGrp::GetIsMute()
{
	return m_bIsMute;
}
/*====================================================================
����  : SetVidPlayPolicy
����  : ���ÿ���ͨ��������ʾ����
����  : u8 byMode
		TKdvDecStatis &tKdvDecStatis
���  : 
����  : BOOL32
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2011/11/28  4.7         ��־��        ����
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetVidPlayPolicy(u8 byShowMode)
{
	enplayMode eIdleChnShowMode = EN_PLAY_BMP;
	switch(byShowMode) 
	{
		case HDU_SHOW_BLACK_MODE:
			eIdleChnShowMode = EN_PLAY_BLACK;
			break;
		case HDU_SHOW_LASTFRAME_MODE:
			eIdleChnShowMode = EN_PLAY_LAST;
			break;
		case HDU_SHOW_DEFPIC_MODE:
			eIdleChnShowMode = EN_PLAY_BMP;
			break;
		case HDU_SHOW_USERDEFPIC_MODE:
			eIdleChnShowMode = EN_PLAY_BMP_USR;
			break;
		default:
			eIdleChnShowMode = EN_PLAY_ERROR;
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetVidPlayPolicy]Unknown IdleChnShowMode(%d)\n",byShowMode);
			break;
	}
	m_tIdleChnBackGround = byShowMode;
	u16 wRet = m_cVidDecoder.SetVidPlayPolicy(5,u32(eIdleChnShowMode));
	if ((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetVidPlayPolicy]m_cVidDecoder.SetVidPlayPolicy() Failed,Error:%d\n",wRet);
		return FALSE;
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetVidPlayPolicy]m_cVidDecoder.SetVidPlayPolicy() Successed!\n");
	}
	return TRUE;
}
/* ==================================================================
����  : GetVidSrcInfoByTypeAndMode
����  : ����HDU����ӿں������ʽ�õ���ƵԴ��Ϣ
����  : ��
���  : ��
����  : void
ע    :
---------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2011/11/28  4.7         ��־��        ����
===================================================================== */
BOOL32 CHdu2ChnMgrGrp::GetVidSrcInfoByTypeAndMode(u8 byOutPortType, u8 byOutModeType, TVidSrcInfo *ptVidSrcInfo,BOOL32 bIsHdu2 = TRUE)
{
    if (NULL == ptVidSrcInfo)
    {
		Hdu2Print(HDU_LVL_WARNING,  "[GetVidSrcInfoByTypeAndMode]NULL == ptVidSrcInfo\n");
        return FALSE;
	}
	switch (byOutPortType)
	{
		case HDU_OUTPUT_DVI:
        case HDU_OUTPUT_HDMI:
		case HDU_OUTPUT_YPbPr:
		case HDU_OUTPUT_SDI:
			switch ( byOutModeType )
			{
			case HDU_YPbPr_1080P_24fps:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 24;
				break;

			case HDU_YPbPr_1080P_25fps:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 25;
				break;

			case HDU_YPbPr_1080P_29970HZ:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 29970;
				break;

			case HDU_YPbPr_1080P_30fps:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 30;
				break;
			
			case HDU_YPbPr_1080P_50fps:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 50;
				break;

			case HDU_YPbPr_1080P_59940HZ:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 59940;
				break;

			case HDU_YPbPr_1080P_60fps:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;

			case HDU_YPbPr_1080i_50HZ:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 0;
				ptVidSrcInfo->m_dwFrameRate = 25;
				break;

			case HDU_YPbPr_1080i_60HZ:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 0;
				ptVidSrcInfo->m_dwFrameRate = 30;
				break;

			case HDU_YPbPr_720P_50fps:
				ptVidSrcInfo->m_wWidth = 1280;
				ptVidSrcInfo->m_wHeight = 720;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 50;
				break;

			case HDU_YPbPr_720P_60fps:
				ptVidSrcInfo->m_wWidth = 1280;
				ptVidSrcInfo->m_wHeight = 720;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;
			
			case HDU_YPbPr_576i_50HZ:
				ptVidSrcInfo->m_wWidth = 720;
				ptVidSrcInfo->m_wHeight = 576;
				ptVidSrcInfo->m_bProgressive = 0;
				ptVidSrcInfo->m_dwFrameRate = 25;
				break;
			
			case HDU_YPbPr_480i_60HZ:    
				ptVidSrcInfo->m_wWidth = 720;
				ptVidSrcInfo->m_wHeight = 480;
				ptVidSrcInfo->m_bProgressive = 0;
				ptVidSrcInfo->m_dwFrameRate = 30;
				break;
			
			default:
				if (bIsHdu2)
				{
					ptVidSrcInfo->m_wWidth = 1920;
					ptVidSrcInfo->m_wHeight = 1080;
					ptVidSrcInfo->m_bProgressive = 1;
					ptVidSrcInfo->m_dwFrameRate = 60;
				}
				else
				{
					ptVidSrcInfo->m_wWidth = 1920;
					ptVidSrcInfo->m_wHeight = 1080;
					ptVidSrcInfo->m_bProgressive = 0;
					ptVidSrcInfo->m_dwFrameRate = 25;
				}
				break;
			}
			break;
		
		case HDU_OUTPUT_VGA:
			switch (byOutModeType)
			{
			case HDU_VGA_SXGA_60HZ:
				ptVidSrcInfo->m_wWidth = 1280;
				ptVidSrcInfo->m_wHeight = 1024;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;

			case HDU_VGA_SXGA_75HZ:
				ptVidSrcInfo->m_wWidth = 1280;
				ptVidSrcInfo->m_wHeight = 1024;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 75;
				break;

			case HDU_VGA_XGA_60HZ:
				ptVidSrcInfo->m_wWidth = 1024;
				ptVidSrcInfo->m_wHeight = 768;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;
		
			case HDU_VGA_XGA_75HZ:
				ptVidSrcInfo->m_wWidth = 1024;
				ptVidSrcInfo->m_wHeight = 768;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 75;
				break;
        
			case HDU_VGA_SVGA_60HZ:
				ptVidSrcInfo->m_wWidth = 800;
				ptVidSrcInfo->m_wHeight = 600;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;
			
			case HDU_VGA_SVGA_75HZ:
				ptVidSrcInfo->m_wWidth = 800;
				ptVidSrcInfo->m_wHeight = 600;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 75;
				break;
			
			case HDU_VGA_VGA_60HZ:
				ptVidSrcInfo->m_wWidth = 640;
				ptVidSrcInfo->m_wHeight = 480;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;
			
			case HDU_VGA_VGA_75HZ:
				ptVidSrcInfo->m_wWidth = 640;
				ptVidSrcInfo->m_wHeight = 480;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 75;
				break;

			case HDU_VGA_WXGA_1280_768_60HZ:
				ptVidSrcInfo->m_wWidth = 1280;
				ptVidSrcInfo->m_wHeight = 768;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;

			case HDU_VGA_WXGA_1280_800_60HZ:
				ptVidSrcInfo->m_wWidth = 1280;
				ptVidSrcInfo->m_wHeight = 800;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;

			case HDU_VGA_WSXGA_60HZ:
				ptVidSrcInfo->m_wWidth = 1440;
				ptVidSrcInfo->m_wHeight = 900;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;

			case HDU_VGA_SXGAPlus_60HZ:
				ptVidSrcInfo->m_wWidth = 1680;
				ptVidSrcInfo->m_wHeight = 1050;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;

			case HDU_VGA_UXGA_60HZ:
				ptVidSrcInfo->m_wWidth = 1600;
				ptVidSrcInfo->m_wHeight = 1200;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;

			case HDU_WXGA_1280_800_75HZ:
				ptVidSrcInfo->m_wWidth = 1280;
				ptVidSrcInfo->m_wHeight = 800;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 75;
				break;

			default:
				if (bIsHdu2)
				{
					ptVidSrcInfo->m_wWidth = 1280;
					ptVidSrcInfo->m_wHeight = 1024;
					ptVidSrcInfo->m_bProgressive = 1;
					ptVidSrcInfo->m_dwFrameRate = 60;
				}
				else
				{
					ptVidSrcInfo->m_wWidth = 1024;
					ptVidSrcInfo->m_wHeight = 768;
					ptVidSrcInfo->m_bProgressive = 1;
					ptVidSrcInfo->m_dwFrameRate = 60;
				}
				break;
			}
			break;
	case HDU_OUTPUT_C:
		switch (byOutModeType)
		{
		case HDU_C_576i_50HZ:
			ptVidSrcInfo->m_wWidth = 720;
			ptVidSrcInfo->m_wHeight = 576;
			ptVidSrcInfo->m_bProgressive = 0;
			ptVidSrcInfo->m_dwFrameRate = 25;
			break;
		case HDU_C_480i_60HZ:    
			ptVidSrcInfo->m_wWidth = 720;
			ptVidSrcInfo->m_wHeight = 480;
			ptVidSrcInfo->m_bProgressive = 0;
			ptVidSrcInfo->m_dwFrameRate = 30;
			break;
		default:
			ptVidSrcInfo->m_wWidth = 720;
			ptVidSrcInfo->m_wHeight = 576;
			ptVidSrcInfo->m_bProgressive = 0;
			ptVidSrcInfo->m_dwFrameRate = 25;
			break;
		}
	    break;
	default:
	    Hdu2Print(HDU_LVL_WARNING,  "[GetVidSrcInfoByTypeAndMode]byOutModeType is not exit!\n");
		return FALSE;
	}
    return TRUE;
}

/*====================================================================
����  : StatusShow
����  : ״̬��ʾ����ʾ���Ͻ�����״̬
����  : void
���  : ��
����  : void
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2011/11/28  4.7         ��־��        ����
====================================================================*/
void   CHdu2Instance::StatusShow(void)                                    
{
	static char status[4][10] = {"IDLE", "INIT","READY","RUNNING"};
	u16 wChnId = GetInsID();
	THduCfg tHduCfg = g_cHdu2App.GetHduCfg();
	if (wChnId == CInstance::DAEMON)
	{
		u32 dwMcuNodeA = g_cHdu2App.GetMcuNode();
		u32 dwMcuNodeB = g_cHdu2App.GetMcuNodeB();
		Hdu2Print(HDU_LVL_KEYSTATUS, "\n**********HDU2(EQPID:%d,EQPTYPE:%d) INFO**********\n",
			tHduCfg.byEqpId,tHduCfg.GetEqpType());
		Hdu2Print(HDU_LVL_KEYSTATUS, "Status:[%s]   Alias:[%s]   RcvStartPort:[%d]   NodeA:[%d] NodeB:[%d]\n",
			status[CurState()],tHduCfg.achAlias,tHduCfg.wRcvStartPort,dwMcuNodeA,dwMcuNodeB);
		Hdu2Print(HDU_LVL_KEYSTATUS, "ChnOutPortTypeInCfg(0)[%d] ChnOutPortTypeInCfg(1)[%d]\n",
			g_cBrdAgentApp.GetOutPortTypeInCfg(0),g_cBrdAgentApp.GetOutPortTypeInCfg(1));
	}
	else if ( wChnId <= MAXNUM_HDU_CHANNEL )
    {
		Hdu2Print(HDU_LVL_KEYSTATUS, "**********CHN(%d) INFO**********\n",wChnId);
		Hdu2Print(HDU_LVL_KEYSTATUS, "Conf Info:");
		m_cChnConfId.Print();
		THduModePort tHduModePort;
		tHduCfg.GetHduModePort( (u8)wChnId - 1, tHduModePort );
		Hdu2Print(HDU_LVL_KEYSTATUS, "Status:%s Param(OutType:%d(%s),OutMode:%d(%s),ZoomRate:%d(%s),ScalingMode:%d(%s))\n",
			status[CurState()],tHduModePort.GetOutPortType(),tHduModePort.GetOutTypeStr(),tHduModePort.GetOutModeType(),tHduModePort.GetOutModeStr(),
			tHduModePort.GetZoomRate(),tHduModePort.GetZoomRateStr(),tHduModePort.GetScalingMode(),tHduModePort.GetScalingModeStr());
        m_cHdu2ChnMgrGrp.ShowInfo(wChnId - 1);
    }
	else
	{
		Hdu2Print(HDU_LVL_ERROR, "------------------INVALID CHN(%d)----------------\n",wChnId);
	}
	return;
}
/*====================================================================
����  : ShowInfo
����  : ��ʾ��Ϣ
����  : ��
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2011/11/28  4.7         ��־��        ����
====================================================================*/
void  CHdu2ChnMgrGrp::ShowInfo(u8 byChnId)
{
	u8 abyDecKey[64];
	s32 iLen = 0;
	m_tMediaDec.GetEncryptKey(abyDecKey, &iLen);
	abyDecKey[iLen] = 0;
	TKdvRcvStatus tKdvRcvStatus;
	u8 byHduChnCount = 0;

	if ( g_cHdu2App.GetHduChnMode(byChnId) == HDUCHN_MODE_FOUR )
	{
		byHduChnCount = HDU_MODEFOUR_MAX_SUBCHNNUM;
	}
	else if(g_cHdu2App.GetHduChnMode(byChnId) == HDUCHN_MODE_ONE)
	{
		byHduChnCount = 1;
	}

	for (u8 byIndex=0; byIndex<byHduChnCount; byIndex++)
	{
		m_acVidMediaRcv[byIndex].GetStatus ( tKdvRcvStatus );
		BOOL32 bIsVidStartRcv = tKdvRcvStatus.m_bRcvStart;
		u16 wVidLocalRcvPort = tKdvRcvStatus.m_tRcvAddr.m_tLocalNet.m_wRTPPort;
		m_cAudMediaRcv.GetStatus ( tKdvRcvStatus );
		BOOL32 bIsAudStartRcv = tKdvRcvStatus.m_bRcvStart;
		u16 wAudLocalRcvPort = tKdvRcvStatus.m_tRcvAddr.m_tLocalNet.m_wRTPPort;
		TKdvDecStatus tKdvDecStatus;
		m_cVidDecoder.GetDecoderStatus(tKdvDecStatus, byIndex);
		BOOL32 bIsVidDecing = tKdvDecStatus.m_bDecStart;
		m_cAudDecoder.GetDecoderStatus(tKdvDecStatus);
		BOOL32 bIsAudDecing = tKdvDecStatus.m_bDecStart;

		Hdu2Print(HDU_LVL_ERROR, "1.HduVmpMode Info    :(HduMode: %d)\n", g_cHdu2App.GetHduChnMode(byChnId));
		Hdu2Print(HDU_LVL_ERROR, "2.ChnMgrGroup Info   :(Mode:%d,IsNeedRS:%d,IdleChnBackGround:%d)\n",m_abyMode[byIndex],m_bIsNeedRs,m_tIdleChnBackGround);
		Hdu2Print(HDU_LVL_ERROR, "3.DECParam Info      :(Mode:%u,key len:%d,key:%s)\n",m_tMediaDec.GetEncryptMode(),iLen,abyDecKey);
		Hdu2Print(HDU_LVL_ERROR, "4.Vid Info           :(IsDecing:%d,IsRecving:%d,RecvPort:%d,PayLoad(Real:%d,Act:%d))\n",
			bIsVidDecing,bIsVidStartRcv,wVidLocalRcvPort,m_atVidDp[byIndex].GetRealPayLoad(),m_atVidDp[byIndex].GetActivePayload());
		Hdu2Print(HDU_LVL_ERROR, "5.Aud Info           :(IsDecing:%d,IsRecving:%d,RecvPort:%d,PayLoad(Real:%d,Act:%d,),Volume(%d)\n",
			bIsAudDecing,bIsAudStartRcv,wAudLocalRcvPort,m_tAudDp.GetRealPayLoad(),m_tAudDp.GetActivePayload(),m_byAudChnNum);
	} //for u8 byIndex=0 over
	return;
}
/* --------------------CHdu2ChnMgrGrp���ʵ��  end-----------------------*/


/* ------------------CHdu2Instance���ʵ��  start----------------------*/
/*====================================================================
����  : CHdu2Instance
����  : ���캯��
����  : 
���  : 
����  : 
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
CHdu2Instance::CHdu2Instance()
{
}
/*====================================================================
����  : ~CHdu2Instance
����  : ��������
����  : 
���  : 
����  : 
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
CHdu2Instance::~CHdu2Instance()
{
}
/*====================================================================
����  : DaemonInstanceEntry
����  : ��ͨʵ����Ϣ���
����  : CMessage* const, CApp*
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
void CHdu2Instance::DaemonInstanceEntry( CMessage* const pMsg, CApp* pcApp )
{
	if ( NULL == pMsg || NULL == pcApp )
	{
		Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonInstanceEntry] pMsg or pcApp is Null!\n");
        return;
	}
    Hdu2Print(HDU_LVL_GENERAL,  "[DaemonInstanceEntry]Receive msg.%d<%s>\n", pMsg->event, ::OspEventDesc(pMsg->event) );
	if ( pMsg->event == EV_HDU_STATUSSHOW )
    {
		StatusShow();
		return;
	}
 	switch (CurState())
 	{
    case emIDLE:
		switch (pMsg->event)
		{
			//�ϵ��ʼ��
			case OSP_POWERON:
				DaemonProcPowerOn(pMsg);
				break;
 			default:
 				Hdu2Print(HDU_LVL_WARNING,  "[DaemonInstanceEntry]wrong message %d<%s> in eIDLE!\n", 
					pMsg->event, OspEventDesc(pMsg->event));
 		}
 		break;
 
 	case emINIT:
 		switch (pMsg->event)
 		{
			// ����
			case EV_HDU_CONNECT_TIMER:
				DaemonProcConnectTimeOut(TRUE);
				break;
				
			// ����
			case EV_HDU_CONNECTB_TIMER:
				DaemonProcConnectTimeOut(FALSE);
				break;

			// ע��
			case EV_HDU_REGISTER_TIMER:  	
				DaemonProcRegisterTimeOut(TRUE);
				break;
			// ע��
			case EV_HDU_REGISTERB_TIMER:  	
				DaemonProcRegisterTimeOut(FALSE);
				break;
				
			// MCU ע��Ӧ����Ϣ
			case MCU_HDU_REG_ACK:
				DaemonProcMcuRegAck(pMsg);
				break;
				
			// MCU �ܾ�ע��
			case MCU_HDU_REG_NACK:
				DaemonProcMcuRegNack(pMsg);
				break;
				
			default: 
				Hdu2Print(HDU_LVL_WARNING,  "[DaemonInstanceEntry] wrong message %d<%s> in eINIT\n",
					pMsg->event, OspEventDesc(pMsg->event));
		}
		break;
	case emREADY:
		switch (pMsg->event)
 		{
			// ����
			case EV_HDU_CONNECT_TIMER:
				DaemonProcConnectTimeOut(TRUE);
				break;
				
			// ����
			case EV_HDU_CONNECTB_TIMER:
				DaemonProcConnectTimeOut(FALSE);
				break;
			// ע��
			case EV_HDU_REGISTER_TIMER:  	
				DaemonProcRegisterTimeOut(TRUE);
				break;
			// ע��
			case EV_HDU_REGISTERB_TIMER:  	
				DaemonProcRegisterTimeOut(FALSE);
				break;
				
			// MCU ע��Ӧ����Ϣ
			case MCU_HDU_REG_ACK:
				DaemonProcMcuRegAck(pMsg);
				break;
				
			// MCU �ܾ�ע��
			case MCU_HDU_REG_NACK:
				DaemonProcMcuRegNack(pMsg);
				break;
				
			// OSP ������Ϣ
			case OSP_DISCONNECT:
				DaemonProcOspDisconnect(pMsg);
				break;

			// ��ʼ����
			case MCU_HDU_START_PLAY_REQ:         
				DaemonProcHduStartPlay(pMsg);
				break;
			
			// �л�HDUͨ�����
			case MCU_HDU_CHGHDUVMPMODE_REQ:
				DaemonProcHduChgVmpMode(pMsg);
				break;

			// ֹͣ����
			case MCU_HDU_STOP_PLAY_REQ:            
				DaemonProcHduStopPlay( pMsg );
				break;
			
			// ������Ƶ�غ�
			case MCU_HDU_FRESHAUDIO_PAYLOAD_CMD:
				DaemonProcHduSetAudioPayLoad(pMsg);
				break;

			// ������������
			case MCU_HDU_CHANGEVOLUME_CMD:
				DaemonProcHduChangeVolumeReq( pMsg );
				break;

			//ȡ��������״̬
			case EV_HDU_GETMSSTATUS_TIMER:
			case MCU_EQP_GETMSSTATUS_ACK:
				DeamonProcGetMsStatusRsp(pMsg);
				break;
			case MCU_HDU_CHANGEMODEPORT_NOTIF:
				DaemonProcModePortChangeNotif(pMsg);
				break;
			case MCU_HDU_CHANGEMODE_CMD:
				DaemonProcChangeModeCmd(pMsg);
				break;
			case MCU_HDU_CHANGEPLAYPOLICY_NOTIF:
				DaemonProcChangePlayPolicy(pMsg);
				break;
			default: 
				Hdu2Print(HDU_LVL_WARNING,  "[DaemonInstanceEntry] wrong message %d<%s> in eREADY\n",
				pMsg->event, OspEventDesc(pMsg->event));
		}
		break;

 	default:
         Hdu2Print(HDU_LVL_WARNING,  "[DaemonInstanceEntry]wrong status.%d for msg.%d<%s>!\n", 
             CurState(), pMsg->event, OspEventDesc(pMsg->event));
		 break;
 	}
}
/*====================================================================
����  : InstanceEntry
����  : ��ͨʵ����Ϣ���
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
void CHdu2Instance::InstanceEntry( CMessage* const pMsg )
{
	if (NULL == pMsg)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[InstanceEntry]: The pMsg point can not be Null!\n");
		return;
	}

	CServMsg cServMsg(pMsg->content, pMsg->length);
    cServMsg.SetEventId(pMsg->event);
    if ( pMsg->event != EV_HDU_NEEDIFRAME_TIMER )
    {
        Hdu2Print(HDU_LVL_FRAME,  "[InstanceEntry]Chn(%d) Receive msg.%d<%s>\n",GetInsID() - 1, pMsg->event, ::OspEventDesc(pMsg->event) );
    }
	if ( pMsg->event == EV_HDU_STATUSSHOW )
    {
		StatusShow();
		return;
	}

	switch (CurState())
	{
	case emIDLE:
		switch (pMsg->event)
		{
			//ͨ����ʼ��
			case EV_HDUCHNMGRGRP_CREATE:
				ProcHduChnGrpCreate();
				break;

			default:
				Hdu2Print(HDU_LVL_WARNING,  "[InstanceEntry]CHN(%d) wrong message %d<%s> in eIDLE\n",GetInsID() - 1, pMsg->event, OspEventDesc(pMsg->event));
				break;
		}
		break;
	case emINIT:
		switch (pMsg->event)
		{
			case EV_HDU_INITCHNCFG:
				ProcInitChnCfg();
				break;

			default:
				Hdu2Print(HDU_LVL_WARNING,  "[InstanceEntry]CHN(%d) wrong message %d<%s> in emINIT\n",GetInsID() - 1, pMsg->event, OspEventDesc(pMsg->event));
				break;
		}
		break;
	case emREADY:
		switch (pMsg->event)
        { 
			//��ʼ����
			case MCU_HDU_START_PLAY_REQ:
				ProcStartPlayReq( cServMsg );
				break;

			// [2013/03/11 chenbing] 
			// �޸�HDU�໭��ģʽ
			case MCU_HDU_CHGHDUVMPMODE_REQ:
				ProcChangeHduVmpMode( cServMsg );
				break;

			// ������Ƶ�غ�
			case MCU_HDU_FRESHAUDIO_PAYLOAD_CMD:
				ProcHduSetAudioPayLoad(cServMsg);
				break;

			//�޸�ͨ������
			case MCU_HDU_CHANGEMODEPORT_NOTIF:
				ProcChangeChnCfg(pMsg);
				break;

			// ����hdu��������
			case MCU_HDU_CHANGEVOLUME_CMD:
				ProcHduChangeVolumeReq( cServMsg );
				break;

			case MCU_HDU_CHANGEPLAYPOLICY_NOTIF:
				ProcChangePlayPolicy(pMsg);
				break;
			//��Ѱ��ǰģʽ
			case EVENT_HDU_SHOWMODWE:
				ProcShowMode();
				break;

			//�����ǰģʽ
			case EVENT_HDU_CLEARMODWE:
				ProcClearMode();
				break;

				//��������
			case OSP_DISCONNECT:
				ProcDisConnect();
				break;

			default: 
				Hdu2Print(HDU_LVL_WARNING,  "[InstanceEntry]CHN(%d) wrong message %d<%s> in emREADY\n",GetInsID() - 1, pMsg->event, OspEventDesc(pMsg->event));
				break;
        }
		break;
	case emRUNNING:
		switch (pMsg->event)
		{
			//��ʼ����
			case MCU_HDU_START_PLAY_REQ:
				//Hdu2ChnStopPlay( MODE_BOTH );
				ProcStartPlayReq( cServMsg );
				break;

			// [2013/03/11 chenbing] 
			// �޸�HDU�໭��ģʽ
			case MCU_HDU_CHGHDUVMPMODE_REQ:
				ProcChangeHduVmpMode( cServMsg );
				break;

			// ������Ƶ�غ�
			case MCU_HDU_FRESHAUDIO_PAYLOAD_CMD:
				ProcHduSetAudioPayLoad(cServMsg);
				break;

			//ֹͣ����
			case MCU_HDU_STOP_PLAY_REQ:
				ProcStopPlayReq( cServMsg );
				break;

			//��������
			case OSP_DISCONNECT:
				ProcDisConnect();
				break;

			//Hdu����Mcu���ؼ�֡
			case EV_HDU_NEEDIFRAME_TIMER:
				ProcTimerNeedIFrame();
				break;

			//�޸�ͨ������
			case MCU_HDU_CHANGEMODEPORT_NOTIF:
				ProcChangeChnCfg(pMsg);
				break;
				
			//����hdu2ͨ����������
			case MCU_HDU_CHANGEVOLUME_CMD:
				ProcHduChangeVolumeReq( cServMsg );
				break;

			case MCU_HDU_CHANGEMODE_CMD:
				ProcChangeModeCmd();
				break;

			case MCU_HDU_CHANGEPLAYPOLICY_NOTIF:
				ProcChangePlayPolicy(pMsg);
				break;

			//��Ѱ��ǰģʽ
			case EVENT_HDU_SHOWMODWE:
				ProcShowMode();
				break;
				
			//�����ǰģʽ
			case EVENT_HDU_CLEARMODWE:
				ProcClearMode();
				break;

			default:
				Hdu2Print(HDU_LVL_WARNING,  "[InstanceEntry]CHN(%d) wrong message %d<%s> in emRUNNING\n",GetInsID() - 1, pMsg->event, OspEventDesc(pMsg->event));
				break;
		}
		break;
	default:
        Hdu2Print(HDU_LVL_WARNING,  "[InstanceEntry]wrong state %d for msg.%d<%s> \n",
            CurState(), pMsg->event, OspEventDesc(pMsg->event));
        break;
	}

	return;
}
/*====================================================================
����  : DaemonProcPowerOn
����  : ��ʼ��
����  : CMessage* const��CApp*
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
void  CHdu2Instance::DaemonProcPowerOn( CMessage* const pMsg)      
{
	printf( "recv power on\n" );
	THduCfg tHduCfg = g_cHdu2App.GetHduCfg();
	if ( pMsg->length == sizeof(THduCfg) )
    {
        memcpy( (u8*)&tHduCfg, (u8*)pMsg->content, sizeof(THduCfg));
		g_cHdu2App.SetHduCfg(tHduCfg);
    }
	else
	{
		Hdu2Print(HDU_LVL_ERROR,  "[DaemonProcPowerOn]pMsg->length(%d) == sizeof(THduCfg)(%d),So Return!\n",pMsg->length,sizeof(THduCfg));
		return;
	}
	tHduCfg = g_cHdu2App.GetHduCfg();
	s32 nRet = 0;
	//�����£����ײ�ý�س�ʼ���ӿ�Ԥ��
	printf( "start HardMPUInit subtype.%d\n",tHduCfg.GetHduSubType() );
	
	if( tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2_S )
	{
		nRet = HardMPUInit((u32)en_HDU2_Board_S);
	}
	else
	{
		nRet = HardMPUInit((u32)en_HDU2_Board);
	}	
	if ((u16)CODEC_NO_ERROR != nRet )
	{
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcPowerOn]HardMPUInit error:%d\n", nRet);
		return;
	}

	printf( "HardMPUInit success!!!\n" );


    u16 wRet = KdvSocketStartup();
    if ( MEDIANET_NO_ERROR != wRet)
    {
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcPowerOn]KdvSocketStartup Faileded, error: %d\n", wRet);
        return ;
    }
	printf( "KdvSocketStartup success!!!\n" );
	for (u8 byLoop = 1; byLoop <= MAXNUM_HDU_CHANNEL; byLoop++ )
	{
		post(MAKEIID(GetAppID(), byLoop), EV_HDUCHNMGRGRP_CREATE);
	}
    if( 0 != tHduCfg.dwConnectIP )
    {
        SetTimer(EV_HDU_CONNECT_TIMER, HDU_CONNETC_TIMEOUT );
    }
    if( 0 != tHduCfg.dwConnectIpB )
    {
        SetTimer(EV_HDU_CONNECTB_TIMER, HDU_CONNETC_TIMEOUT );
    }
    NextState((u32)emINIT);
	printf( "[DaemonProcPowerOn]PowerOn Successed!So DAEMONINST State Change: IDLE--->INIT!\n");
	Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcPowerOn]PowerOn Successed!So DAEMONINST State Change: IDLE--->INIT!\n");
    return;
}
/*====================================================================
����  : DaemonProcHduStartPlay
����  : ����ʵ����������Ϣ�ַ�����Ӧͨ��
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
void  CHdu2Instance::DaemonProcHduStartPlay( CMessage* const pMsg )
{
	CServMsg cServMsg(pMsg->content, pMsg->length);
    post(MAKEIID(GetAppID(), cServMsg.GetChnIndex()+1), MCU_HDU_START_PLAY_REQ, pMsg->content,pMsg->length);
}

/*====================================================================
����  : DaemonProcHduChgVmpMode
����  : HDUͨ���л�ͨ��ģʽ(�໭���ͨ��ģʽ)
����  : CMessage* const
���  : ��
����  : ��
ע    : HDU�໭��ӿ�
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾         �޸���      �޸�����
2013/03/11	 4.7.2		  �±�		   ����(HDU�໭��֧��)
====================================================================*/
void  CHdu2Instance::DaemonProcHduChgVmpMode( CMessage* const pMsg )
{
	CServMsg cServMsg(pMsg->content, pMsg->length);
    post(MAKEIID(GetAppID(), cServMsg.GetChnIndex()+1), MCU_HDU_CHGHDUVMPMODE_REQ, pMsg->content,pMsg->length);
}

/*====================================================================
����  : DaemonProcHduSetAudioPayLoad
����  : ������Ƶ�غ�
����  : CMessage* const
���  : ��
����  : ��
ע    : HDU�໭��ӿ�
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾         �޸���      �޸�����
2013/03/11	 4.7.2		  �±�		   ����(HDU�໭��֧��)
====================================================================*/
void  CHdu2Instance::DaemonProcHduSetAudioPayLoad(CMessage* const pMsg)
{
	CServMsg cServMsg(pMsg->content, pMsg->length);
    post(MAKEIID(GetAppID(), cServMsg.GetChnIndex()+1), MCU_HDU_FRESHAUDIO_PAYLOAD_CMD, pMsg->content,pMsg->length);
}

/*====================================================================
����  : DaemonProcHduStopPlay
����  : ����ʵ����ֹͣ��Ϣ�ַ�����Ӧͨ��
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
void  CHdu2Instance::DaemonProcHduStopPlay( CMessage* const pMsg )
{
	CServMsg cServMsg(pMsg->content, pMsg->length);
    post(MAKEIID(GetAppID(), cServMsg.GetChnIndex()+1), MCU_HDU_STOP_PLAY_REQ, pMsg->content,pMsg->length);
	return;

}
/*====================================================================
����  : DaemonProcOspDisconnect
����  : ��������
����  : CMessage* const��CApp*
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
void  CHdu2Instance::DaemonProcOspDisconnect( CMessage* const pMsg) 
{
	if ( NULL == pMsg )  
    {
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcOspDisconnect] message's pointer is Null\n");
        return;
    }

    u32 dwNode = *(u32*)pMsg->content;
	u32 dwMcuNodeA = g_cHdu2App.GetMcuNode();
	u32 dwMcuNodeB = g_cHdu2App.GetMcuNodeB();
    if (INVALID_NODE != dwNode)
    {
        OspDisconnectTcpNode(dwNode);
    } 
    if(dwNode == dwMcuNodeA)
    {
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcOspDisconnect] McuNode.A disconnect\n");
        g_cHdu2App.FreeStatusDataA();
        SetTimer(EV_HDU_CONNECT_TIMER, HDU_CONNETC_TIMEOUT);
    }
    else if(dwNode == dwMcuNodeB)
    {
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcOspDisconnect] McuNode.B disconnect\n");
        g_cHdu2App.FreeStatusDataB();
        SetTimer(EV_HDU_CONNECTB_TIMER, HDU_CONNETC_TIMEOUT);
    }
	dwMcuNodeA = g_cHdu2App.GetMcuNode();
	dwMcuNodeB = g_cHdu2App.GetMcuNodeB();
	if (INVALID_NODE != dwMcuNodeA || INVALID_NODE != dwMcuNodeB)
	{
		SetTimer(EV_HDU_GETMSSTATUS_TIMER, 100);
		return;
	}

	if (INVALID_NODE == dwMcuNodeA && INVALID_NODE == dwMcuNodeB)
	{
		for (u8 byLoop = 1; byLoop <= MAXNUM_HDU_CHANNEL; byLoop++ )
		{
			post(MAKEIID(GetAppID(), byLoop), OSP_DISCONNECT);
		}
		g_cHdu2App.SetMcuSSrc(0);
		NextState((u32)emINIT);
		Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcOspDisconnect]DAEMONINST State Change: READY--->INIT!\n");
	}
    return;
}
/*====================================================================
����  : DaemonProcConnectTimeOut
����  : ��MCU����
����  : BOOL32 ��MCUA or MCUB
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
void  CHdu2Instance::DaemonProcConnectTimeOut( BOOL32 bIsConnectA )   
{
	u32 dwConnectNode = INVALID_NODE;
	if (bIsConnectA)
	{
		dwConnectNode = BrdGetDstMcuNode();
	}
	else
	{
		dwConnectNode = BrdGetDstMcuNodeB();
	}
	if (!::OspIsValidTcpNode(dwConnectNode) ||
		OSP_OK != ::OspNodeDiscCBRegQ(dwConnectNode, GetAppID(), CHdu2Instance::DAEMON))
	{
		Hdu2Print(HDU_LVL_WARNING,  "[DaemonProcConnectTimeOut]Connect Mcu %s failed, node: %d!\n", 
				bIsConnectA ? "A" : "B", dwConnectNode);
		if (bIsConnectA)
		{
			SetTimer(EV_HDU_CONNECT_TIMER, HDU_CONNETC_TIMEOUT);
		}
		else
		{
			SetTimer(EV_HDU_CONNECTB_TIMER, HDU_CONNETC_TIMEOUT);
		}
		return;
	}
	if (bIsConnectA)
	{
		KillTimer(EV_HDU_CONNECT_TIMER);
		SetTimer(EV_HDU_REGISTER_TIMER, 10 );
		g_cHdu2App.SetMcuNode(dwConnectNode);
	}
	else
	{
		KillTimer(EV_HDU_CONNECTB_TIMER);
		SetTimer(EV_HDU_REGISTERB_TIMER, 10 );
		g_cHdu2App.SetMcuNodeB(dwConnectNode);
	}
}
/*====================================================================
����  : DaemonProcRegisterTimeOut
����  : ��MCUע��
����  : BOOL32 ע�� MCUA or MCUB
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
void  CHdu2Instance::DaemonProcRegisterTimeOut( BOOL32 bIsRegiterA )       
{
	CServMsg       cSvrMsg;
    TPeriEqpRegReq tReg;
	THduCfg tHduCfg = g_cHdu2App.GetHduCfg();

	// ����4.7.2HDU2ģ��HDUע�ᵽ�ϰ汾MCU[6/3/2013 chendaiwei]
	u8 byEqpType = GetOldVersionEqpTypeBySubType(tHduCfg.GetHduSubType());
	
    tReg.SetMcuEqp((u8)tHduCfg.wMcuId, tHduCfg.byEqpId, byEqpType);
    tReg.SetPeriEqpIpAddr(htonl(tHduCfg.dwLocalIP));
	tReg.SetChnnlNum(MAXNUM_HDU_CHANNEL);
	if(tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2)
	{
		tReg.SetVersion(DEVVER_HDU2);
	}
	else if (tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2_S)
	{
		tReg.SetVersion(DEVVER_HDU2_S);
	}
	else if (tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2_L)
	{
		tReg.SetVersion(DEVVER_HDU2_L);
	}
	else
	{
		tReg.SetVersion(DEVVER_HDU);
	}
	tReg.SetStartPort(HDU_EQP_STARTPORT);
    tReg.SetHDEqp(TRUE);
    cSvrMsg.SetMsgBody((u8*)&tReg, sizeof(tReg));
    if(bIsRegiterA)
    {
		if (!::OspIsValidTcpNode(g_cHdu2App.GetMcuNode()))
		{
			SetTimer(EV_HDU_CONNECT_TIMER,HDU_CONNETC_TIMEOUT);
			Hdu2Print(HDU_LVL_WARNING,  "[DaemonProcRegisterTimeOut]OspIsValidTcpNode(%d),So Reconnect To Mcu(A)!\n", 
				g_cHdu2App.GetMcuNode());
			return;
		}
		post(MAKEIID(AID_MCU_PERIEQPSSN, tHduCfg.byEqpId),HDU_MCU_REG_REQ,
		cSvrMsg.GetServMsg(),cSvrMsg.GetServMsgLen(),g_cHdu2App.GetMcuNode());
        SetTimer(EV_HDU_REGISTER_TIMER, HDU_REGISTER_TIMEOUT);
    }
    else
    {
		if (!::OspIsValidTcpNode(g_cHdu2App.GetMcuNodeB()))
		{
			SetTimer(EV_HDU_CONNECTB_TIMER,HDU_CONNETC_TIMEOUT);
			Hdu2Print(HDU_LVL_WARNING,  "[DaemonProcRegisterTimeOut]OspIsValidTcpNode(%d),So Reconnect To Mcu(B)!\n", 
				g_cHdu2App.GetMcuNodeB());
			return;
		}
		post(MAKEIID(AID_MCU_PERIEQPSSN, tHduCfg.byEqpId),HDU_MCU_REG_REQ,
		cSvrMsg.GetServMsg(),cSvrMsg.GetServMsgLen(),g_cHdu2App.GetMcuNodeB());
		SetTimer(EV_HDU_REGISTERB_TIMER, HDU_REGISTER_TIMEOUT);
    }
    return;
}
/*====================================================================
����  : DaemonProcMcuRegAck
����  : ����ע��ACK��Ӧ
����  : CMessage* const, CApp*
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
void  CHdu2Instance::DaemonProcMcuRegAck(CMessage* const pMsg)      
{
    if(NULL == pMsg)
    {
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcMcuRegAck]Recv Reg Ack Msg, but the msg's pointer is Null\n");
        return;
    }
	u32 dwMcuNodeA = g_cHdu2App.GetMcuNode();
	u32 dwMcuNodeB = g_cHdu2App.GetMcuNodeB();
	if(pMsg->srcnode == dwMcuNodeA)     
    {
		g_cHdu2App.SetMcuIId(pMsg->srcid);
        KillTimer(EV_HDU_REGISTER_TIMER);                
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcMcuRegAck] Regist success to mcu A\n");
    }
    else if(pMsg->srcnode == dwMcuNodeB)
    {
        g_cHdu2App.SetMcuIIdB(pMsg->srcid);
        KillTimer(EV_HDU_REGISTERB_TIMER);
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcMcuRegAck] Regist success to mcu B\n");       
    }
	CServMsg cServMsg(pMsg->content, pMsg->length);
	TPeriEqpRegAck *ptRegAck = NULL;
	ptRegAck = (TPeriEqpRegAck*)cServMsg.GetMsgBody();
	if (NULL == ptRegAck)
	{
		Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcMcuRegAck] ptRegAck is null!\n");
		return;
	}
    if ( g_cHdu2App.GetMcuSSrc() == 0 )
    {
        g_cHdu2App.SetMcuSSrc(ptRegAck->GetMSSsrc());
    }
    else
    {
        if ( g_cHdu2App.GetMcuSSrc()  != ptRegAck->GetMSSsrc() )
        {
            Hdu2Print(HDU_LVL_ERROR,  "[DaemonProcMcuRegAck] MPC SSRC ERROR(%u<-->%u), Disconnect Both Nodes!\n", 
				g_cHdu2App.GetMcuSSrc(), ptRegAck->GetMSSsrc());
            if ( OspIsValidTcpNode(dwMcuNodeA) )
            {
                OspDisconnectTcpNode(dwMcuNodeA);
            }
            if ( OspIsValidTcpNode(dwMcuNodeB) )
            {
                OspDisconnectTcpNode(dwMcuNodeB);
            }      
        }
		else
		{
			StatusNotify();
		}
		return;
    }
	
	TEqpHduCfgInfo *ptCfgInfo = NULL;
	
	ptCfgInfo = (TEqpHduCfgInfo*)(cServMsg.GetMsgBody() + sizeof(TPrsTimeSpan) + sizeof(TPeriEqpRegAck));
    if (NULL == ptCfgInfo)
    {
		Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcMcuRegAck]ptCfgInfo is Null!\n");
		return;
    }
	THduCfg tHduCfg = g_cHdu2App.GetHduCfg();
	if( tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU_M ||
		tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU_L ||
		tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU_H)
	{
		THduChnlModePort tHduChnModePort;
		THduModePort tModePort;
		for (u8 byChnIdx = 0 ;byChnIdx < MAXNUM_HDU_CHANNEL;byChnIdx++)
		{
			memset(&tModePort, 0x0, sizeof(tModePort));
			memset(&tHduChnModePort, 0x0, sizeof(tHduChnModePort));
			ptCfgInfo->GetHduChnlModePort(byChnIdx, tHduChnModePort);
			tModePort.SetOutModeType(tHduChnModePort.GetOutModeType());
			tModePort.SetOutPortType(tHduChnModePort.GetOutPortType());
			tModePort.SetZoomRate(tHduChnModePort.GetZoomRate());
			s8	HDUCFG_FILE[KDV_MAX_PATH] = {0};
			sprintf(HDUCFG_FILE, "%s/mcueqp.ini", DIR_CONFIG);
			s32 nValue;
			::GetRegKeyInt( HDUCFG_FILE, "EQPDEBUG", "DecResizeMode", 0, &nValue );
			tModePort.SetScalingMode(nValue);
			tHduCfg.SetHduModePort(byChnIdx, tModePort);
		}
	}
    
	tHduCfg.wRcvStartPort = ptCfgInfo->GetEqpStartPort();
    memcpy(tHduCfg.achAlias, ptCfgInfo->GetAlias(), MAXLEN_EQP_ALIAS);
	g_cHdu2App.SetHduCfg(tHduCfg);

	for (u8 byLoop = 1; byLoop <= MAXNUM_HDU_CHANNEL; byLoop++ )
	{
		post(MAKEIID(GetAppID(), byLoop), EV_HDU_INITCHNCFG);
		if (tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU_H)
		{
			Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcMcuRegAck]EQP_TYPE_HDU_H Only Has One Chn,So break!\n");
			printf("[DaemonProcMcuRegAck]EQP_TYPE_HDU_H Only Has One Chn,So break!\n");
			break;
		}
		
	}
	NEXTSTATE((u32)emREADY);
	TPrsTimeSpan *ptPrsTime = NULL;
	ptPrsTime = (TPrsTimeSpan*)(cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck));
	g_cHdu2App.SetPrsTimeSpan(*ptPrsTime);
	StatusNotify();
	Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcMcuRegAck]RegAck Successed!DAEMONINST State Change: INIT--->READY!\n");
	return;
}
/*====================================================================
����  : DaemonProcHduChangeVolumeReq
����  : ��ʽ�ı�֪ͨ
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
void  CHdu2Instance::DaemonProcHduChangeVolumeReq( CMessage* const pMsg )
{
	CServMsg cServMsg(pMsg->content, pMsg->length);
	THduVolumeInfo *ptHduVolumeInfo = (THduVolumeInfo*)cServMsg.GetMsgBody();
	post(MAKEIID(GetAppID(), ptHduVolumeInfo->GetChnlIdx()+1), pMsg->event, pMsg->content,pMsg->length);
}
/*====================================================================
����  : DaemonProcMcuRegNack
����  : ��ʽ�ı�֪ͨ
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
void  CHdu2Instance::DaemonProcModePortChangeNotif( CMessage* const pMsg )  
{
	if ( NULL == pMsg)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[DaemonProcModePortChangeNotif]NULL == pMsg,So Return!\n");
		return;
	}
	THduChnlModePort *ptHduChnModePort = (THduChnlModePort*)pMsg->content;
    THduModePort tModePort;
	THduCfg tHduCfg = g_cHdu2App.GetHduCfg();
    for (u8 byLoop=0; byLoop<MAXNUM_HDU_CHANNEL; byLoop++)
    {
		tHduCfg.GetHduModePort(byLoop, tModePort);
		tModePort.SetOutModeType(ptHduChnModePort[byLoop].GetOutModeType());
		tModePort.SetOutPortType(ptHduChnModePort[byLoop].GetOutPortType());
		tModePort.SetZoomRate(ptHduChnModePort[byLoop].GetZoomRate());
		tModePort.SetScalingMode(ptHduChnModePort[byLoop].GetScalingMode());
        tHduCfg.SetHduModePort(byLoop, tModePort);
		post(MAKEIID(GetAppID(), byLoop+1), MCU_HDU_CHANGEMODEPORT_NOTIF, &tModePort,sizeof(THduModePort));
		Hdu2Print(HDU_LVL_WARNING,  "[DaemonProcModePortChangeNotif]hdu chn%d's config changed!\n", byLoop);

		if (tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU_H)
		{
			Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcModePortChangeNotif]EQP_TYPE_HDU_H Only Has One Chn,So break!\n");
			printf("[DaemonProcModePortChangeNotif]EQP_TYPE_HDU_H Only Has One Chn,So break!\n");
			break;
		}
    }
}      
/*====================================================================
����  : DaemonProcMcuRegNack
����  : ����ע��NACK��Ӧ
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
void  CHdu2Instance::DaemonProcMcuRegNack(CMessage* const pMsg)               
{
	if( pMsg->srcnode == g_cHdu2App.GetMcuNode())    
    {
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcMcuRegNack]Hdu registe be refused by A.\n");
		SetTimer(EV_HDU_REGISTER_TIMER, HDU_REGISTER_TIMEOUT);
    }
    if( pMsg->srcnode == g_cHdu2App.GetMcuNodeB())    
    {
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcMcuRegNack]Hdu registe be refused by B.\n");
		SetTimer(EV_HDU_REGISTERB_TIMER, HDU_REGISTER_TIMEOUT);
    }
}
/*====================================================================
����  : DeamonProcGetMsStatusRsp
����  : ȡ��������״̬
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
void  CHdu2Instance::DeamonProcGetMsStatusRsp( CMessage* const pMsg )         
{
	BOOL bSwitchOk = FALSE;
	CServMsg cServMsg(pMsg->content, pMsg->length);
	if( MCU_EQP_GETMSSTATUS_ACK == pMsg->event )
	{
		TMcuMsStatus *ptMsStatus = (TMcuMsStatus *)cServMsg.GetMsgBody();
        KillTimer(EV_HDU_GETMSSTATUS_TIMER);
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[DeamonProcGetMsStatusRsp]receive msg MCU_EQP_GETMSSTATUS_ACK. IsMsSwitchOK :%d\n",ptMsStatus->IsMsSwitchOK());
        if(ptMsStatus->IsMsSwitchOK()) //�����ɹ�
        {
            bSwitchOk = TRUE;
        }
	}	
	if( !bSwitchOk )//����ʧ�ܻ��߳�ʱ
	{
		u32 dwMcuNodeA = g_cHdu2App.GetMcuNode();
		u32 dwMcuNodeB = g_cHdu2App.GetMcuNodeB();
		if (OspIsValidTcpNode(dwMcuNodeA))
		{
			post( g_cHdu2App.GetMcuIId(), EQP_MCU_GETMSSTATUS_REQ, NULL, 0, dwMcuNodeA );           
			Hdu2Print(HDU_LVL_KEYSTATUS,  "[DeamonProcGetMsStatusRsp]post EQP_MCU_GETMSSTATUS_REQ to McuNode.A\n");
		}
		if (OspIsValidTcpNode(dwMcuNodeB))
		{
			post( g_cHdu2App.GetMcuIIdB(), EQP_MCU_GETMSSTATUS_REQ, NULL, 0, dwMcuNodeB );        
			Hdu2Print(HDU_LVL_KEYSTATUS,  "[DeamonProcGetMsStatusRsp]post EQP_MCU_GETMSSTATUS_REQ to McuNode.B\n");
		}
		SetTimer(EV_HDU_GETMSSTATUS_TIMER, WAITING_MSSTATUS_TIMEOUT);
	}
}
/*====================================================================
����  : DaemonProcChangeModeCmd
����  : �ı�ģʽ����
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2012/02/24   4.7         ��־��        ����
====================================================================*/
void  CHdu2Instance::DaemonProcChangeModeCmd( CMessage* const pMsg )
{
	CServMsg cServMsg(pMsg->content, pMsg->length);
	post(MAKEIID(GetAppID(), cServMsg.GetChnIndex()+1), pMsg->event, pMsg->content,pMsg->length);
}
/*====================================================================
����  : ProcHduChnGrpCreate
����  : ��ʼ��HDU
����  : void
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
void  CHdu2Instance::ProcHduChnGrpCreate(void)
{
	u16 wChnIdx = GetInsID() - 1;
	
	printf( "[ProcHduChnGrpCreate] create.%d\n",g_cHdu2App.GetHduCfg().GetHduSubType() );

	BOOL32 bRet = m_cHdu2ChnMgrGrp.Create(wChnIdx,g_cHdu2App.GetHduCfg().GetHduSubType() == HDU_SUBTYPE_HDU2_S);
	
	if (!bRet)
	{
		Hdu2Print(HDU_LVL_ERROR, "[ProcHduChnGrpCreate]Chn(%d) Create Failed!So return\n",wChnIdx);
		return;
	}
	NextState((u32)emINIT);
	Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcHduChnGrpCreate]Chn(%d) Create Successed!So Chn State Change: IDLE--->INIT!\n",wChnIdx);
}
/*====================================================================
����  : ProcInitChn
����  : ��ʼ��HDU2ͨ��
����  : void
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
void  CHdu2Instance::ProcInitChnCfg(void)
{
	u16 wChnIdx = GetInsID() - 1;
	THduModePort tChnCfg;
	TVidSrcInfo tVidSrcInfo;
	memset( &tVidSrcInfo, 0x0, sizeof(tVidSrcInfo) );
	memset(&tChnCfg, 0x0, sizeof(THduModePort));
	THduCfg tHduCfg = g_cHdu2App.GetHduCfg();
	tHduCfg.GetHduModePort(wChnIdx, tChnCfg);
	u8 byChnOutPortType = g_cBrdAgentApp.GetOutPortTypeInCfg((u8)wChnIdx);
	if (g_cBrdAgentApp.GetBrdPosition().byBrdID == BRD_TYPE_HDU && byChnOutPortType !=0 )//ģ��HDU���ȿ������ļ������õ�����ӿ�����
	{
		tChnCfg.SetOutPortType(byChnOutPortType - 1);
	}
	BOOL32 bIsNotHdu2Brd = g_cBrdAgentApp.GetBrdPosition().byBrdID == BRD_TYPE_HDU2_L ? TRUE : FALSE;
	CheckChnCfgParam(tChnCfg, bIsNotHdu2Brd);
	tHduCfg.SetHduModePort(wChnIdx,tChnCfg);
	g_cHdu2App.SetHduCfg(tHduCfg);

	Hdu2Print(HDU_LVL_WARNING, "[ProcInitChnCfg]new cfg param(%s,%s,%s,%s)\n",
		tChnCfg.GetOutTypeStr(),tChnCfg.GetOutModeStr(),tChnCfg.GetZoomRateStr(),tChnCfg.GetScalingModeStr());

	g_cHdu2App.SetHduCfg(tHduCfg);
	if (!m_cHdu2ChnMgrGrp.SetVidPlyPortType(GetVidPlyPortTypeByChnAndType(wChnIdx,tChnCfg.GetOutPortType())))
	{
		Hdu2Print(HDU_LVL_WARNING, "[ProcInitChnCfg]SetVidPlyPortType Failed!\n");
	}
	BOOL32 bIsHdu2 = (tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2 || tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2_S);
	if (!m_cHdu2ChnMgrGrp.GetVidSrcInfoByTypeAndMode(tChnCfg.GetOutPortType(),tChnCfg.GetOutModeType(), 
			&tVidSrcInfo,bIsHdu2))
	{
		Hdu2Print(HDU_LVL_WARNING, "[ProcInitChnCfg]Chn(%d) GetVidSrcInfoByTypeAndMode() Failed!\n",wChnIdx);
	}
	if (!m_cHdu2ChnMgrGrp.SetVideoPlyInfo(&tVidSrcInfo))
	{
		Hdu2Print(HDU_LVL_WARNING, "[ProcInitChnCfg]Chn(%d) SetVideoPlyInfo() Failed!\n",wChnIdx);
	}
	u16 wWidth, wHeigh;
	switch (tChnCfg.GetZoomRate())
	{
	case HDU_ZOOMRATE_4_3:
		wWidth = 4;
		wHeigh = 3;
		break;
		
	case HDU_ZOOMRATE_16_9:
		wWidth = 16;
		wHeigh = 9;
		break;
		
	default:
		wWidth = 0;
		wHeigh = 0;
	}
	if (!m_cHdu2ChnMgrGrp.SetPlayScales(wWidth, wHeigh))
	{
		Hdu2Print(HDU_LVL_WARNING, "[ProcInitChnCfg]Chn(%d) SetPlayScales() Failed!\n",wChnIdx);
	}
	if (!m_cHdu2ChnMgrGrp.SetVidDecResizeMode(tChnCfg.GetScalingMode()))
	{
		Hdu2Print(HDU_LVL_WARNING, "[ProcInitChnCfg]Chn(%d) SetVidDecResizeMode() Failed!\n",wChnIdx);
	}
	if (!m_cHdu2ChnMgrGrp.SetVolume( HDU_VOLUME_DEFAULT ))
	{
		Hdu2Print(HDU_LVL_WARNING, "[ProcInitChnCfg]Chn(%d) SetAudioVolume() Failed!\n",wChnIdx);
	}
	if (!m_cHdu2ChnMgrGrp.SetIsMute( FALSE ))
	{
		Hdu2Print(HDU_LVL_WARNING, "[ProcInitChnCfg]Chn(%d) SetIsMute() Failed!\n",wChnIdx);
	}

	g_cHdu2App.SetHduChnMode(wChnIdx, HDUCHN_MODE_ONE );

	NextState((u32)emREADY);
	SendChnNotify(TRUE);
	Hdu2Print(HDU_LVL_KEYSTATUS,  "[ProcInitChnCfg]Chn(%d) Create Successed!So Chn State Change: INIT--->READY!\n",wChnIdx);
}
/*====================================================================
����  : ProcHduChangeVolumeReq
����  : ������������
����  : CServMsg &cServMsg
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
void  CHdu2Instance::ProcHduChangeVolumeReq( CServMsg &cServMsg )
{
    THduVolumeInfo tHduVolumeInfo = *( THduVolumeInfo* )cServMsg.GetMsgBody();
	BOOL32 bRet;
	bRet = m_cHdu2ChnMgrGrp.SetVolume( tHduVolumeInfo.GetVolume() );
	if (!bRet)
	{
		Hdu2Print(HDU_LVL_WARNING, "[ProcHduChangeVolumeReq]SetAudioVolume Failed!\n" );
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL, "[ProcHduChangeVolumeReq]SetAudioVolume sucessful!\n" );
	}
	bRet = m_cHdu2ChnMgrGrp.SetIsMute( tHduVolumeInfo.GetIsMute() );
	if (!bRet)
	{
		Hdu2Print(HDU_LVL_WARNING, "[ProcHduChangeVolumeReq]SetAudioMute Failed!\n" );
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[ProcHduChangeVolumeReq]SetAudioMute sucessful!\n" );
	}
	SendChnNotify();
}
/*====================================================================
����  : ProcChangeModeCmd
����  : ����ģʽ
����  : CServMsg &cServMsg
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
2013/03/11	 4.7.2		  �±�		   �޸�(HDU�໭��֧��)
====================================================================*/
void  CHdu2Instance::ProcChangeModeCmd()
{
// 	u8 byIsAdd = *(u8*)cServMsg.GetMsgBody();
// 	u8 byChgMode = *(u8*)(cServMsg.GetMsgBody() + sizeof(u8));
// 
// 	if (byIsAdd >= 1)
// 	{
// 		if (g_cHdu2App.GetHduChnMode(GetInsID() - 1) == HDUCHN_MODE_FOUR)
// 		{
// 			if (!Hdu2ChnStartPlay( byChgMode, 0, TRUE ))
// 			{
// 				Hdu2Print(HDU_LVL_WARNING,  "[ProcChangeModeCmd]Hdu2ChnStartPlay(%d) HduMode:(%d) Failed!\n", byChgMode, HDUCHN_MODE_FOUR);
// 			}
// 		}
// 		else if (g_cHdu2App.GetHduChnMode(GetInsID() - 1) == HDUCHN_MODE_ONE)
// 		{
// 			if (!Hdu2ChnStartPlay( byChgMode ))
// 			{
// 				Hdu2Print(HDU_LVL_WARNING,  "[ProcChangeModeCmd]Hdu2ChnStartPlay(%d)HduMode:(%d) Failed!\n", byChgMode, HDUCHN_MODE_ONE);
// 			}
// 		}
// 		else
// 		{
// 			//
// 		}
// 	} 
// 	else
// 	{
// 		if (g_cHdu2App.GetHduChnMode(GetInsID() - 1) == HDUCHN_MODE_FOUR)
// 		{
// 			if (!Hdu2ChnStopPlay( byChgMode, 0, TRUE ))
// 			{
// 				Hdu2Print(HDU_LVL_WARNING,  "[ProcChangeModeCmd]Hdu2ChnStopPlay(%d) HduMode:(%d) Failed!\n", byChgMode, HDUCHN_MODE_FOUR);
// 			}
// 		}
// 		else if (g_cHdu2App.GetHduChnMode(GetInsID() - 1) == HDUCHN_MODE_ONE)
// 		{
// 			if (!Hdu2ChnStopPlay( byChgMode ))
// 			{
// 				Hdu2Print(HDU_LVL_WARNING,  "[ProcChangeModeCmd]Hdu2ChnStopPlay(%d) HduMode:(%d) Failed!\n", byChgMode, HDUCHN_MODE_ONE);
// 			}
// 		}
// 		else
// 		{
// 			//
// 		}
// 	}
// 	SendChnNotify();
 }
/*====================================================================
����  : ProcChangeChnCfg
����  : �޸�ͨ������
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
void CHdu2Instance::ProcChangeChnCfg( CMessage* const pMsg )
{
	u16 wChnIdx = GetInsID() - 1;
	THduModePort tHdu2ChnCfg;
	TVidSrcInfo tVidSrcInfo;
    u16 wWidth;
	u16 wHeigh;
	THduCfg tHduCfg = g_cHdu2App.GetHduCfg();
	tHdu2ChnCfg = *(THduModePort*)pMsg->content;
	u8 byChnOutPortType = g_cBrdAgentApp.GetOutPortTypeInCfg((u8)wChnIdx);
	if (g_cBrdAgentApp.GetBrdPosition().byBrdID == BRD_TYPE_HDU && byChnOutPortType !=0 )//ģ��HDU���ȿ������ļ������õ�����ӿ�����
	{
		tHdu2ChnCfg.SetOutPortType(byChnOutPortType - 1);
	}
	BOOL32 bIsNotHdu2Brd = g_cBrdAgentApp.GetBrdPosition().byBrdID == BRD_TYPE_HDU2_L ? TRUE : FALSE;
	CheckChnCfgParam(tHdu2ChnCfg, bIsNotHdu2Brd);
	tHduCfg.SetHduModePort(wChnIdx,tHdu2ChnCfg);
	g_cHdu2App.SetHduCfg(tHduCfg);
	Hdu2Print(HDU_LVL_WARNING,  "[ProcInitChnCfg]new cfg param(%s,%s,%s,%s)\n",
		tHdu2ChnCfg.GetOutTypeStr(),tHdu2ChnCfg.GetOutModeStr(),tHdu2ChnCfg.GetZoomRateStr(),tHdu2ChnCfg.GetScalingModeStr());
	if (!m_cHdu2ChnMgrGrp.SetVidPlyPortType(GetVidPlyPortTypeByChnAndType(wChnIdx,tHdu2ChnCfg.GetOutPortType())))
	{
		Hdu2Print(HDU_LVL_WARNING,  "[ProcChangeChnCfg]SetVidPlyPortType Failed!\n");
	}
	BOOL32 bIsHdu2 =  (tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2 || tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2_S);
	if (m_cHdu2ChnMgrGrp.GetVidSrcInfoByTypeAndMode(tHdu2ChnCfg.GetOutPortType(),tHdu2ChnCfg.GetOutModeType(), 
		&tVidSrcInfo,bIsHdu2))
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[ProcChangeChnCfg]GetVidSrcInfoByTypeAndMode suceeded!\n");
	}
	else
	{
		Hdu2Print(HDU_LVL_WARNING,  "[ProcChangeChnCfg]GetVidSrcInfoByTypeAndMode Failed\n");
	}
	if (!m_cHdu2ChnMgrGrp.SetVideoPlyInfo(&tVidSrcInfo))
	{
		Hdu2Print(HDU_LVL_WARNING,  "[ProcChangeChnCfg]SetVideoPlyInfo Failed!\n");
	}
	switch (tHdu2ChnCfg.GetZoomRate())
	{
		case HDU_ZOOMRATE_4_3:
			wWidth = 4;
			wHeigh = 3;
			break;
		case HDU_ZOOMRATE_16_9:
			wWidth = 16;
			wHeigh = 9;
			break;
		default:
			wWidth = 0;
			wHeigh = 0;
	}
	if (!m_cHdu2ChnMgrGrp.SetPlayScales( wWidth, wHeigh ))
	{
		Hdu2Print(HDU_LVL_WARNING,  "[ProcChangeChnCfg]SetPlayScales Failed!\n");
	}
	if (!m_cHdu2ChnMgrGrp.SetVidDecResizeMode(tHdu2ChnCfg.GetScalingMode()))
	{
		Hdu2Print(HDU_LVL_WARNING,  "[ProcChangeChnCfg]Chn(%d) SetVidDecResizeMode() Failed!\n",wChnIdx);
	}
}
/*====================================================================
����  : ProcStartPlayReq
����  : ��ʼ����
����  : CServMsg&
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
2013/03/11	 4.7.2		  �±�		   �޸�(HDU�໭��֧��)
=====================================================================*/
void  CHdu2Instance::ProcStartPlayReq( CServMsg &cServMsg )
{
    u8 byChnIdx = (u8)GetInsID() - 1; ////HDUͨ��Id
	u16 wIndex = 0;

    THduStartPlay tHduStartPlay = *(THduStartPlay*)cServMsg.GetMsgBody();
	wIndex += sizeof(THduStartPlay);
	Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Revc tHduStartPlay.GetMode(%d)!!!\n", tHduStartPlay.GetMode());

	TDoublePayload  tDoubleVidPayload = *(TDoublePayload*)(cServMsg.GetMsgBody() + wIndex);
	wIndex += sizeof(TDoublePayload);
	Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Revc VidRealPayLoad(%d) VidActivePayload(%d)!!!\n", 
		tDoubleVidPayload.GetRealPayLoad(), tDoubleVidPayload.GetActivePayload());

	TDoublePayload  tDoubleAudPayload = *(TDoublePayload*)(cServMsg.GetMsgBody() + wIndex);
	wIndex += sizeof(TDoublePayload);
	Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Revc AudRealPayLoad(%d) AudActivePayload(%d)!!!\n", 
		tDoubleAudPayload.GetRealPayLoad(), tDoubleAudPayload.GetActivePayload());

	THDUExCfgInfo tHDUExCfgInfo;
	tHDUExCfgInfo.m_byIdleChlShowMode = HDU_SHOW_DEFPIC_MODE;// ��ʾĬ��ͼƬ
	if (cServMsg.GetMsgBodyLen() > wIndex)
	{
		tHDUExCfgInfo = *(THDUExCfgInfo *)(cServMsg.GetMsgBody() + wIndex);
		wIndex += sizeof(THDUExCfgInfo);
		Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Revc tHDUExCfgInfo.m_byIdleChlShowMode(%d)!!!\n", tHDUExCfgInfo.m_byIdleChlShowMode);
	}

	TTransportAddr tAudRemoteAddr;
	memset(&tAudRemoteAddr,0,sizeof(TTransportAddr));
	tAudRemoteAddr = *(TTransportAddr *)(cServMsg.GetMsgBody() + wIndex);
	wIndex += sizeof(TTransportAddr);
	Hdu2Print(HDU_LVL_KEYSTATUS,  "[ProcStartPlayReq] Recv AudRemoteAddr<%x, %d>!\n", tAudRemoteAddr.GetIpAddr(), tAudRemoteAddr.GetPort());
	
	u8 byAudChnNum = *(u8 *)(cServMsg.GetMsgBody() + wIndex);
	m_cHdu2ChnMgrGrp.SetAudChnNum(byAudChnNum);
	wIndex += sizeof(u8);
	Hdu2Print(HDU_LVL_KEYSTATUS,  "[ProcStartPlayReq] Recv byAudChnNum<%d>!\n", byAudChnNum);

	u8 byHduMode = 0;		//HDUͨ��ģʽ
	u8 byHduSubChnId = 0;	//HDU��ͨ��Id
	if(cServMsg.GetMsgBodyLen() > wIndex)
	{
		// [2013/03/11 chenbing] ����HDU��ͨ��Id��ͨ��ģʽ
		byHduSubChnId = *(u8 *)(cServMsg.GetMsgBody() + wIndex);
		wIndex += sizeof(u8);
		byHduMode = *(u8 *)(cServMsg.GetMsgBody() + wIndex);
		wIndex += sizeof(u8);
		// У��Hduģʽ�Ƿ�Ϸ�
		if ( g_cHdu2App.IsValidHduVmpMode(byHduMode) )
		{
			if (byHduMode == HDUCHN_MODE_ONE)
			{
				byHduSubChnId = 0;
			}
		}
		else
		{
			SendMsgToMcu(HDU_MCU_START_PLAY_NACK, cServMsg);
			return;
		}

		Hdu2Print(HDU_LVL_KEYSTATUS,  
				"[ProcStartPlayReq] Revc HduChnId(%d) HduMode(%d) HduSubChnId(%d) MediaMode(%d)!!!\n",
				 byChnIdx, byHduMode, byHduSubChnId, tHduStartPlay.GetMode());

		Hdu2Print(HDU_LVL_WARNING,  
				"[ProcStartPlayReq] NewHduVmpMode(%d) OldHduVmpMode(%d) !!!\n", byHduMode, g_cHdu2App.GetHduChnMode(byChnIdx));
		
		//�л���ͨ�����
		if ( !ChangeHduVmpMode(byChnIdx, byHduMode) )
		{
			Hdu2Print(HDU_LVL_WARNING,  
				"[ProcStartPlayReq] ChangeHduVmpMode Error !!!\n");
			return;
		}
	}

	u8 byMode = MODE_NONE;
	if (tHduStartPlay.GetMode() == HDU_OUTPUTMODE_AUDIO)
	{
		byMode = MODE_AUDIO;
	}
	else if (tHduStartPlay.GetMode() == HDU_OUTPUTMODE_VIDEO)
	{
		byMode = MODE_VIDEO;
	}
	else if (tHduStartPlay.GetMode() == HDU_OUTPUTMODE_BOTH)
	{
		byMode = MODE_BOTH;
	}
	else
	{
		Hdu2Print(HDU_LVL_WARNING,  "[ProcStartPlayReq]byMode(%d) error!!\n",byMode);
		return;
	}

	if ( MODE_VIDEO == byMode || MODE_BOTH == byMode )
	{
		m_cHdu2ChnMgrGrp.SetVidDoublePayload( tDoubleVidPayload, byHduSubChnId);
	}
    
    if( MODE_AUDIO == byMode || MODE_BOTH == byMode )
    {
		m_cHdu2ChnMgrGrp.SetAudDoublePayload(tDoubleAudPayload);
    }

	m_cHdu2ChnMgrGrp.SetVidPlayPolicy(tHDUExCfgInfo.m_byIdleChlShowMode);
	
	if (m_cHdu2ChnMgrGrp.GetAudChnNum() == 0)
	{
		m_cHdu2ChnMgrGrp.SetAudChnNum(1);
	}

	Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Revc IsNeedByPrs(%d) !!!\n", tHduStartPlay.IsNeedByPrs());

	TMediaEncrypt   tMediaEncrypt;
	u8 abyDecKey[64] = {0};
	s32 iLen = 0;
	tMediaEncrypt = tHduStartPlay.GetVideoEncrypt();
	tMediaEncrypt.GetEncryptKey(abyDecKey, &iLen);
	Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Revc EncryptMode(%d) EncKeyLen(%d) EncKey(%s)!!!\n",
	     	tMediaEncrypt.GetEncryptMode(), iLen, abyDecKey);

	m_cHdu2ChnMgrGrp.SetMediaEncrypt(&tMediaEncrypt);
    // ���ü��ܲ���
    m_cHdu2ChnMgrGrp.SetDecryptKey(byMode, byHduSubChnId);
	m_cHdu2ChnMgrGrp.SetActivePT(byMode, TRUE, byHduSubChnId);
	//�趪���ش�����
	m_cHdu2ChnMgrGrp.SetNetRecvRsParam(byMode,tHduStartPlay.IsNeedByPrs(), byHduSubChnId);

	TLocalNetParam tNetParm;
	THduCfg tHduCfg = g_cHdu2App.GetHduCfg();


	u8 byDstChnnl = 0;
	Hdu2Print(HDU_LVL_KEYSTATUS, "tHduCfg.GetHduSubType(%d)\n", tHduCfg.GetHduSubType());
	if(tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2 || tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2_S)
	{
		// HDU2֧�ֶ໭��1ͨ���Ľӿڶ˿���ʼΪ��42200
		byDstChnnl = (COMPOSECHAN(byChnIdx, byHduSubChnId))*PORTSPAN;
	}
	else
	{
		// ��֧���Ļ����1ͨ���ӿڶ˿���ʼΪ��42010
		byDstChnnl = byChnIdx*PORTSPAN;
	}

    //��Ƶ
    if( MODE_BOTH  == byMode || MODE_VIDEO == byMode )
    {
		u32 dwRtcpBackIp;
		u16 wRtcpBackPort;
		tHduStartPlay.GetVidRtcpBackAddr(dwRtcpBackIp, wRtcpBackPort);
        memset(&tNetParm, 0, sizeof(tNetParm));

        tNetParm.m_tLocalNet.m_dwRTPAddr  = htonl(tHduCfg.dwLocalIP);
        tNetParm.m_tLocalNet.m_wRTPPort   = tHduCfg.wRcvStartPort + byDstChnnl;
        tNetParm.m_tLocalNet.m_dwRTCPAddr = htonl(tHduCfg.dwLocalIP);
        tNetParm.m_tLocalNet.m_wRTCPPort  = tHduCfg.wRcvStartPort + 1 + byDstChnnl;
        tNetParm.m_dwRtcpBackAddr = htonl(dwRtcpBackIp);
        tNetParm.m_wRtcpBackPort  = wRtcpBackPort;

		Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Video RTPAddr(%s) RTPPort(%d)\n",
			IpToStr(htonl(tNetParm.m_tLocalNet.m_dwRTPAddr)), tNetParm.m_tLocalNet.m_wRTPPort);
		Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Video RTCPAddr(%s) RTCPPort(%d)\n",
			IpToStr(htonl(tNetParm.m_tLocalNet.m_dwRTCPAddr)),	tNetParm.m_tLocalNet.m_wRTCPPort);
		Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Video RtcpBackAddr(%s) RtcpBackPort(%d)!!!\n",
			IpToStr(htonl(tNetParm.m_dwRtcpBackAddr)), tNetParm.m_wRtcpBackPort);

		m_cHdu2ChnMgrGrp.SetVidLocalNetParam(&tNetParm, byHduSubChnId);
    }

    // ��Ƶ
    if( MODE_BOTH  == byMode || MODE_AUDIO == byMode )
    {
        memset(&tNetParm, 0, sizeof(tNetParm));
        tNetParm.m_tLocalNet.m_dwRTPAddr  = htonl(tHduCfg.dwLocalIP);
        tNetParm.m_tLocalNet.m_wRTPPort   = tHduCfg.wRcvStartPort + 2 + byDstChnnl;
        tNetParm.m_tLocalNet.m_dwRTCPAddr = htonl(tHduCfg.dwLocalIP);
        tNetParm.m_tLocalNet.m_wRTCPPort  = tHduCfg.wRcvStartPort + 3 + byDstChnnl;
        tNetParm.m_dwRtcpBackAddr = htonl(tAudRemoteAddr.GetIpAddr());
        tNetParm.m_wRtcpBackPort  = tAudRemoteAddr.GetPort();

		Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Audio RTPAddr(%s) RTPPort(%d)\n",
			IpToStr(htonl(tNetParm.m_tLocalNet.m_dwRTPAddr)), tNetParm.m_tLocalNet.m_wRTPPort);
		Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Audio RTCPAddr(%s) RTCPPort(%d)\n",
			IpToStr(htonl(tNetParm.m_tLocalNet.m_dwRTCPAddr)),	tNetParm.m_tLocalNet.m_wRTCPPort);
		Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Audio RtcpBackAddr(%s) RtcpBackPort(%d)!!!\n",
			IpToStr(htonl(tNetParm.m_dwRtcpBackAddr)), tNetParm.m_wRtcpBackPort);

		m_cHdu2ChnMgrGrp.SetAudLocalNetParam(&tNetParm);
    }

    if (Hdu2ChnStartPlay( byMode, byHduSubChnId ))
    {
		// [2013/03/11 chenbing]  
        SendMsgToMcu(HDU_MCU_START_PLAY_ACK, cServMsg);
		SendChnNotify(FALSE, byHduSubChnId);
		m_cChnConfId = cServMsg.GetConfId();
		Hdu2Print(HDU_LVL_DETAIL,  "[ProcStartPlayReq] HduChnId(%d) HduSubChnId(%d) Start Succes!!!\n", byChnIdx, byHduSubChnId);
    }
    else
    {
        SendMsgToMcu(HDU_MCU_START_PLAY_NACK, cServMsg);
		Hdu2Print(HDU_LVL_ERROR,  "[ProcStartPlayReq] HduChnId(%d) HduSubChnId(%d) Start Faild!!!\n", byChnIdx, byHduSubChnId);
    }

	Hdu2Print(HDU_LVL_GENERAL, "Mcu_Hdu2 MsgLen: %d\n", wIndex);
}

/*====================================================================
����  : ProcChangeHduVmpMode
����  : �л�HDUͨ��ģʽ
����  : CServMsg&
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���       �޸�����
2013/03/11	 4.7.2		  �±�		   ����(HDU�໭��֧��)
=====================================================================*/
void CHdu2Instance::ProcChangeHduVmpMode( CServMsg &cServMsg )
{
	u8 byHduEqpId = cServMsg.GetEqpId();
	u8 byHduChnId = cServMsg.GetChnIndex();
	u8 byHduVmpMode = *(u8 *)cServMsg.GetMsgBody();

	Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcChangeHduVmpMode] Revc HduMode(%d) HduId(%d) HduChnId(%d)!!!\n",
				byHduVmpMode, byHduEqpId, byHduChnId);

	if ( ChangeHduVmpMode(byHduChnId, byHduVmpMode) )
	{
		SendMsgToMcu(HDU_MCU_CHGHDUVMPMODE_ACK, cServMsg);
	}
	else
	{
		SendMsgToMcu(HDU_MCU_CHGHDUVMPMODE_NACK, cServMsg);
	}

	u8 bySubId = 0;
	for (; bySubId < HDU_MODEFOUR_MAX_SUBCHNNUM; bySubId++)
	{
		if (MODE_NONE != m_cHdu2ChnMgrGrp.GetMode(bySubId))
		{
			break;
		}
	}
	
	if (bySubId >= HDU_MODEFOUR_MAX_SUBCHNNUM)
	{
		//�ָ���ͨ���ĳ�ʼ̬
		NextState((u32)emREADY);
	}
}

/*====================================================================
����  : ChangeHduVmpMode
����  : �л�HDUͨ��ģʽ
����  : 
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���       �޸�����
2013/05/23	 4.7.2		  �±�		   ����(HDU�໭��֧��)
=====================================================================*/
BOOL32 CHdu2Instance::ChangeHduVmpMode(const u8 byHduChnId, const u8 byHduVmpMode)
{
	// У��Hduģʽ�Ƿ�Ϸ�
	if ( g_cHdu2App.IsValidHduVmpMode(byHduVmpMode) )
	{
		//��Ҫ�л���ͨ���뵱ǰͨ��ģʽ��ͬ
		if ( byHduVmpMode == g_cHdu2App.GetHduChnMode(byHduChnId) )
		{
			Hdu2Print(HDU_LVL_WARNING,  
				"[ChangeHduVmpMode] NewHduVmpMode and OldHduVmpMode is same!!!\n");
			return TRUE;
		}
		else
		{
			//��һͨ���Ѿ����ķ��
			if ( HDUCHN_MODE_FOUR == g_cHdu2App.GetHduChnMode(byHduChnId, TRUE)
				&& HDUCHN_MODE_FOUR ==  byHduVmpMode)
			{
				Hdu2Print(HDU_LVL_WARNING,  
					"[ChangeHduVmpMode] Other HduChnnl already is HDUCHN_MODE_FOUR!!!\n");
				return FALSE;
			}
			else
			{
				// ��һ����л����ķ�� �� ���ķ���л���һ���
				if (  (HDUCHN_MODE_ONE == g_cHdu2App.GetHduChnMode(byHduChnId) && byHduVmpMode == HDUCHN_MODE_FOUR)
					|| (HDUCHN_MODE_FOUR == g_cHdu2App.GetHduChnMode(byHduChnId)&& byHduVmpMode == HDUCHN_MODE_ONE)
					)
				{
					// �л�ģʽ, ������Ƶ���л�
					if( m_cHdu2ChnMgrGrp.SetVidPIPParam( byHduVmpMode ) )
					{
						g_cHdu2App.SetHduChnMode( byHduChnId, byHduVmpMode );
					}
					else
					{
						Hdu2Print(HDU_LVL_WARNING,  
							"[ChangeHduVmpMode]HDUVMPMODE change Failed!!!\n");
						return FALSE;
					}
				}
			}
		}
	}
	else
	{
		Hdu2Print(HDU_LVL_ERROR,  
			"[ChangeHduVmpMode]byHduVmpMode or HduChnId IsValid!!!\n");
		return FALSE;
	}
	return TRUE;
}

/*====================================================================
����  : ProcStopPlayReq
����  : ֹͣ����
����  : CServMsg&
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼ ��
��  ��       �汾        �޸���      �޸�����
2013/03/11	 4.7.2		  �±�		   ����(HDU�໭��֧��)
====================================================================*/
void  CHdu2Instance::ProcStopPlayReq( CServMsg& cServMsg)   		  
{
	u8 byHduChnId = cServMsg.GetChnIndex();
	u8 byHduSubChnId = 0;
	u8 byMode = MODE_BOTH;


	// [2013/03/12 chenbing]
	// ������ѯʱ�����ȿ���HDUͬʱ�л����
	// ��ֹͣ��ͨ������ʱ�������Ѿ��л������ԣ������жϷ��
	if ( cServMsg.GetMsgBodyLen() == (sizeof(TEqp) + sizeof(u8) + sizeof(u8)) )
	{
		Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStopPlayReq] Revc SerMsgBodyLen(%d) !!!\n", cServMsg.GetMsgBodyLen());
		byHduSubChnId = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TEqp));
		byMode = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TEqp) + sizeof(u8));
	}

	Hdu2Print(HDU_LVL_KEYSTATUS,  
		"[ProcStopPlayReq] Revc OldMediaMode(%d) NewMediaMode(%d) HduChnId(%d) HduSubChnId(%d) HduMode(%d)!!!\n",
				m_cHdu2ChnMgrGrp.GetMode(byHduSubChnId), byMode, byHduChnId, byHduSubChnId, g_cHdu2App.GetHduChnMode(byHduChnId));

	if (Hdu2ChnStopPlay(byMode, byHduSubChnId))
	{
		SendMsgToMcu(HDU_MCU_STOP_PLAY_ACK, cServMsg);
		SendChnNotify(FALSE, byHduSubChnId);
		m_cChnConfId.SetNull();
		Hdu2Print(HDU_LVL_DETAIL,  "[ProcStopPlayReq] HduChnId(%d) HduSubChnId(%d) Stop Success!!!\n", byHduChnId, byHduSubChnId);
	}
	else
	{
		SendMsgToMcu(HDU_MCU_STOP_PLAY_NACK, cServMsg);
		Hdu2Print(HDU_LVL_ERROR,  "[ProcStopPlayReq] HduChnId(%d) HduSubChnId(%d) Stop Faild!!!\n", byHduChnId, byHduSubChnId);
	}
}

/*====================================================================
����  : ProcHduSetAudioPayLoad
����  : ������Ƶ�غ�
����  : CServMsg&
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼ ��
��  ��       �汾        �޸���      �޸�����
2013/03/11	 4.7.2		  �±�		   ����
====================================================================*/
void CHdu2Instance::ProcHduSetAudioPayLoad(CServMsg& cServMsg)
{
	u8 byAudioNum = *(u8*)(cServMsg.GetMsgBody());
	TDoublePayload  tDoubleAudPayload = *(TDoublePayload*)(cServMsg.GetMsgBody() + sizeof(u8));

	Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcHduSetAudioPayLoad] ChnId(%d) CurrMediaMode(%d) AudioNum(%d) ActivePayload(%d) RealPayLoad(%d)\n",
	GetInsID() - 1,	m_cHdu2ChnMgrGrp.GetMode(), byAudioNum, tDoubleAudPayload.GetActivePayload(), tDoubleAudPayload.GetRealPayLoad());

	if ( MODE_BOTH == m_cHdu2ChnMgrGrp.GetMode() 
		|| MODE_AUDIO == m_cHdu2ChnMgrGrp.GetMode())
	{
		Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcHduSetAudioPayLoad] Rfresh AudioPayLoad Success!!!\n");

		Hdu2ChnStopPlay( MODE_AUDIO );

		m_cHdu2ChnMgrGrp.SetAudDoublePayload(tDoubleAudPayload);
	    m_cHdu2ChnMgrGrp.SetActivePT(MODE_AUDIO);
		m_cHdu2ChnMgrGrp.SetAudChnNum(byAudioNum);
		Hdu2ChnStartPlay( MODE_AUDIO );
	}
}

/*====================================================================
����  : ProcOspDisconnect
����  : Hdu��������
����  : void
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2013/03/11   4.7.2       �±�        �޸�(HDU�໭��)
====================================================================*/
void CHdu2Instance::ProcDisConnect(void)
{
	u8 byHduChnId = GetInsID() - 1;
	
	if ( HDUCHN_MODE_FOUR == g_cHdu2App.GetHduChnMode(byHduChnId) )
	{
		for (u8 bySubChnId=0; bySubChnId<HDU_MODEFOUR_MAX_SUBCHNNUM; bySubChnId++)
		{
			Hdu2ChnStopPlay( MODE_BOTH, bySubChnId );
		}

		// Mcu����,�л�ģʽ, ������Ƶ���л�Ϊһ���
		if( m_cHdu2ChnMgrGrp.SetVidPIPParam( HDUCHN_MODE_ONE ) )
		{
			g_cHdu2App.SetHduChnMode( byHduChnId, HDUCHN_MODE_ONE );
			Hdu2Print(HDU_LVL_KEYSTATUS, "ChangeHduVmpMode Success!!!\n");
		}
		else
		{
			Hdu2Print(HDU_LVL_KEYSTATUS, "ChangeHduVmpMode Falid!!!\n");
		}
	}
	else if ( HDUCHN_MODE_ONE == g_cHdu2App.GetHduChnMode(byHduChnId) )
	{
		Hdu2ChnStopPlay( MODE_BOTH );
	}

	NextState((u32)emINIT);
	Hdu2Print(HDU_LVL_KEYSTATUS,  "[InstanceEntry]OSP_DISCONNECT Chn(%d) State Change: RUNNING--->READY!\n",GetInsID() - 1);
}

/*====================================================================
����  : ProcTimerNeedIFrame
����  : Hdu����Mcu���ؼ�֡
����  : void
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��      ����
2013/03/11   4.7.2       �±�        �޸�(HDU�໭��)
====================================================================*/
void  CHdu2Instance::ProcTimerNeedIFrame( )                  
{
	TKdvDecStatis tDecChannelStatis;
	u8 byMaxChnNum = 1;

	// �ķ��
	if ( HDUCHN_MODE_FOUR == g_cHdu2App.GetHduChnMode( GetInsID() - 1) )
	{
		byMaxChnNum = HDU_MODEFOUR_MAX_SUBCHNNUM;
	}

	Hdu2Print(HDU_LVL_GENERAL,  
		"[ProcTimerNeedIFrame] Revc HduMode(%d) MaxChnNum(%d)!!!\n",
				g_cHdu2App.GetHduChnMode( GetInsID() - 1), byMaxChnNum);

	// [2013/03/11 chenbing] ѭ����������ͨ���Ƿ���Ҫ�ؼ�֡
	for (u8 byHduSubChnId = 0; byHduSubChnId < byMaxChnNum; byHduSubChnId++)
	{
		if( m_cHdu2ChnMgrGrp.GetMode(byHduSubChnId) == MODE_VIDEO || m_cHdu2ChnMgrGrp.GetMode(byHduSubChnId) == MODE_BOTH)
		{ 
			memset( &tDecChannelStatis, 0, sizeof(TKdvDecStatis) );
			if (!m_cHdu2ChnMgrGrp.GetDecoderStatis(MODE_VIDEO, tDecChannelStatis, byHduSubChnId))
			{
				Hdu2Print(HDU_LVL_FRAME,  "[ProcTimerNeedIFrame] GetVidDecoderStatis Failed!\n");
			}
			if ( tDecChannelStatis.m_bVidCompellingIFrm )
			{	
				THduCfg tHduCfg = g_cHdu2App.GetHduCfg();
				CServMsg cServMsg;
				cServMsg.SetConfId( m_cChnConfId );
				cServMsg.SetChnIndex( GetInsID() - 1 );
				cServMsg.SetSrcSsnId( tHduCfg.byEqpId );
				
				// �ķ���׷����ͨ�� 
				if ( HDU_MODEFOUR_MAX_SUBCHNNUM == byMaxChnNum )
				{
					u8 bySubChnId = byHduSubChnId;
					cServMsg.SetMsgBody((u8*)&bySubChnId, sizeof(u8));
				}
				
				SendMsgToMcu( HDU_MCU_NEEDIFRAME_CMD, cServMsg );
 				Hdu2Print(HDU_LVL_FRAME,  "[ProcTimerNeedIFrame]HduChannel(%d) HduSubChnId(%d) request iframe!!\n",
 					GetInsID() - 1, byHduSubChnId);
			}
		}
	}

	SetTimer( EV_HDU_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL );	
}

/*====================================================================
����  : SendMsgToMcu
����  : ��MCU������Ϣͨ�ú���
����  : u16 -->��Ϣ��, CServMsg&
���  : ��
����  : BOOL32
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��      ����
====================================================================*/
void CHdu2Instance::SendMsgToMcu( u16 wEvent, CServMsg& cServMsg ) 
{
	if (GetInsID() != CInstance::DAEMON)
    {
        cServMsg.SetChnIndex((u8)GetInsID() - 1);
    }
	u32 dwMcuNodeA = g_cHdu2App.GetMcuNode();
	u32 dwMcuNodeB = g_cHdu2App.GetMcuNodeB();
    if (OspIsValidTcpNode(dwMcuNodeA)) 
	{
		post(g_cHdu2App.GetMcuIId(), wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), dwMcuNodeA);
		Hdu2Print(HDU_LVL_GENERAL,  "[SendMsgToMcu]Send Message %u(%s) to Mcu A\n",wEvent, ::OspEventDesc(wEvent));
	}
	else
	{
		Hdu2Print(HDU_LVL_GENERAL,  "[SendMsgToMcu]Send Message %u(%s) to Mcu A(Node:%d) Failed\n",wEvent, ::OspEventDesc(wEvent),dwMcuNodeA);
	}
    if(OspIsValidTcpNode(dwMcuNodeB))
    {
		post(g_cHdu2App.GetMcuIIdB(), wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), dwMcuNodeB);
		Hdu2Print(HDU_LVL_GENERAL,  "[SendMsgToMcu]Send Message %u(%s) to Mcu B\n",wEvent, ::OspEventDesc(wEvent));
    }
    else
    {
		Hdu2Print(HDU_LVL_GENERAL,  "[SendMsgToMcu]Send Message %u(%s) to Mcu B(Node:%d) Failed\n",wEvent, ::OspEventDesc(wEvent),dwMcuNodeB);
    }
}
/*====================================================================
����  : SendChnNotif
����  : ״̬֪ͨ
����  : void
���  : ��
����  : void
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
2013/03/11   4.7.2       �±�          �޸�(HDU�໭��֧��)
====================================================================*/
void   CHdu2Instance::SendChnNotify( BOOL32 bFirstNotif/* = FALSE*/, u8 byHduSubChnId /* = 0*/)                                       
{
	 // [2013/03/11 chenbing] 
	 // Ŀǰ4.7.2���ز��Ѿ����Դ�Notif����
	 // ��Ϊ�˼����ϵ�Mcu,�˴���Ϣ���豣��
	 u8 byChnIdx = (u8)GetInsID() - 1;
	 CServMsg cServMsg;
	 cServMsg.SetConfId(m_cChnConfId);
	 TEqp tEqp;
	 THduCfg tHduCfg = g_cHdu2App.GetHduCfg();

	// ����4.7.2HDU2ģ��HDUע�ᵽ�ϰ汾MCU[6/3/2013 chendaiwei]
	 u8 byEqpType = GetOldVersionEqpTypeBySubType(tHduCfg.GetHduSubType());
	 
	 tEqp.SetEqpType( byEqpType);
	 tEqp.SetEqpId( tHduCfg.GetEqpId() );
	 tEqp.SetMcuId( tHduCfg.GetMcuId() );
	 THduChnStatus tHduChnStatus;
	 tHduChnStatus.SetEqp( tEqp );
	 tHduChnStatus.SetChnIdx(byChnIdx);
	 tHduChnStatus.SetStatus((u8)CurState());  
	 tHduChnStatus.SetVolume( m_cHdu2ChnMgrGrp.GetVolume() );
	 tHduChnStatus.SetIsMute( m_cHdu2ChnMgrGrp.GetIsMute() );
	 cServMsg.SetMsgBody( (u8*)&tHduChnStatus, sizeof(THduChnStatus)  );
	 u8 bFirstNotifTmp = bFirstNotif ? 1:0;
	 cServMsg.CatMsgBody( (u8*)&bFirstNotifTmp, sizeof(bFirstNotifTmp));
	 cServMsg.CatMsgBody( (u8*)&byHduSubChnId, sizeof(u8));
	 

	 SendMsgToMcu(HDU_MCU_CHNNLSTATUS_NOTIF, cServMsg);
	 Hdu2Print(HDU_LVL_DETAIL,  
		 "[SendChnNotif]Send HDU_MCU_CHNNLSTATUS_NOTIF To Mcu HduSubChnId(%d) CutState(%d)\n", byHduSubChnId, (u8)CurState());
 	 return;
}

/*====================================================================
����  : StatusNotify
����  : HDU״̬֪ͨ
����  : void
���  : ��
����  : void
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��        ����
====================================================================*/
void   CHdu2Instance::StatusNotify(void)
{
	TPeriEqpStatus tEqpStatus;
	memset( &tEqpStatus, 0, sizeof(tEqpStatus) );
	THduCfg tHduCfg = g_cHdu2App.GetHduCfg();

	// ����4.7.2HDU2ģ��HDUע�ᵽ�ϰ汾MCU[6/3/2013 chendaiwei]
	u8 byEqpType = GetOldVersionEqpTypeBySubType(tHduCfg.GetHduSubType());

	tEqpStatus.SetMcuEqp((u8)tHduCfg.wMcuId, tHduCfg.byEqpId, byEqpType);	
	tEqpStatus.m_byOnline = (CurState() >= (u32)emREADY) ? 1 : 0;
	tEqpStatus.SetAlias(tHduCfg.achAlias);
	if (g_cBrdAgentApp.GetBrdPosition().byBrdID == BRD_TYPE_HDU)
	{
		tEqpStatus.m_tStatus.tHdu.byOutputMode = HDU_OUTPUTMODE_BOTH;
	}
	
	for ( u8 byChnlIdx=0; byChnlIdx<MAXNUM_HDU_CHANNEL; byChnlIdx++ )
	{
		// [2013/03/11 chenbing] ���õ�ǰͨ��֧���ķ�� 
		if(tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2 || tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2_S)
		{
			tEqpStatus.m_tStatus.tHdu.SetChnMaxVmpMode(byChnlIdx, HDU_MODEFOUR_MAX_SUBCHNNUM);
			Hdu2Print(HDU_LVL_DETAIL,  "[StatusNotify]MaxSubChnNum(%d)\n", HDU_MODEFOUR_MAX_SUBCHNNUM);
		}
		tEqpStatus.m_tStatus.tHdu.atHduChnStatus[byChnlIdx].SetVolume( HDU_VOLUME_DEFAULT );
	}

	CServMsg cServMsg;
	cServMsg.SetMsgBody((u8*)&tEqpStatus, sizeof(tEqpStatus));
 	SendMsgToMcu(HDU_MCU_STATUS_NOTIF, cServMsg);
	Hdu2Print(HDU_LVL_DETAIL,  "[StatusNotify]Send HDU_MCU_STATUS_NOTIF To Mcu\n");
	return;
}
/* ==================================================================
����  : Hdu2ChnPlay
����  : ͨ������
����  : ��
���  : ��
����  : void
ע    :
---------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���      �޸�����
2011/11/28  4.7         ��־��        ����
2013/03/11	4.7.2		�±�		  �޸�(HDU�໭��֧��)
===================================================================== */
BOOL32 CHdu2Instance::Hdu2ChnStartPlay( u8 byMode /* = MODE_BOTH */, u8 byHduSubChnId)
{
	// [2013/03/11 chenbing] ������ͨ���Ƿ����ģʽ
	// ��Ҫ��ǰ�ж�
	u8 bySubChnId = 0;
	for (bySubChnId=0; bySubChnId<HDU_MODEFOUR_MAX_SUBCHNNUM; bySubChnId++)
	{
		Hdu2Print(HDU_LVL_DETAIL,  
			"[ProcStartPlayReq] Revc MediaMode(%d) HduSubChnId(%d)!!!\n",
				byMode, bySubChnId);
		if( MODE_VIDEO == m_cHdu2ChnMgrGrp.GetMode(bySubChnId)
		 ||	MODE_BOTH == m_cHdu2ChnMgrGrp.GetMode(bySubChnId)
		  )
		{
			break;
		}
	}

	if (MODE_NONE == byMode )
	{
		Hdu2Print(HDU_LVL_ERROR,  "[Hdu2ChnStartPlay]GetMode() == MODE_NONE(%d)\n",byMode);
		return FALSE;
	}

    if(!m_cHdu2ChnMgrGrp.StartNetRecv(byMode, byHduSubChnId))
    {
        Hdu2Print(HDU_LVL_ERROR, "[Hdu2ChnStartPlay]StartNetRecv Faileded MediaMode(%d) HduSubChnId(%d)!!!\n",
					byMode, byHduSubChnId);
        return FALSE;
    }

	if(!m_cHdu2ChnMgrGrp.StartDecode(byMode))
	{
		Hdu2Print(HDU_LVL_ERROR,  "[Hdu2ChnStartPlay]StartDecode Faileded MediaMode(%d) HduSubChnId(%d)!!!\n",
					byMode, byHduSubChnId);
		return FALSE;
	}

	//��ǰ��ͨ����û����Ƶ, ��ǰ����Ϊ��Ƶ������Ƶ, ���ùؼ�֡��ʱ��
	if ( bySubChnId >= HDU_MODEFOUR_MAX_SUBCHNNUM
		&& (MODE_VIDEO == byMode || MODE_BOTH == byMode ))
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[Hdu2ChnStartPlay]All SubChnl is Null, SetTimer EV_HDU_NEEDIFRAME_TIMER!!!\n");
		SetTimer(EV_HDU_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL);
		NextState((u32)emRUNNING);
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[Hdu2ChnStartPlay]SubChnl(%d) OldMode(%d) NewMode(%d) !!!\n",
			bySubChnId, m_cHdu2ChnMgrGrp.GetMode(bySubChnId),  byMode);
	}

	u8 byOldMode = m_cHdu2ChnMgrGrp.GetMode(byHduSubChnId);
	u8 byCurMode = MODE_NONE;
	if (MODE_VIDEO == byOldMode && (MODE_BOTH == byMode || MODE_AUDIO == byMode))
	{
		byCurMode = MODE_BOTH;
	}
	else if (MODE_AUDIO == byOldMode && (MODE_BOTH == byMode || MODE_VIDEO == byMode))
	{
		byCurMode = MODE_BOTH;
	}
	else 
	{
		byCurMode = byMode;
	}

	m_cHdu2ChnMgrGrp.SetMode(byCurMode, byHduSubChnId);
    return TRUE;
}

/* ==================================================================
����  : Hdu2ChnStopPlay
����  : ͨ��ֹͣ
����  : ��
���  : ��
����  : void
ע    : ���u8 byIsClearAllSubChn, �ж��Ƿ������ͨ��
---------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2011/11/28   4.7         ��־��      ����
2013/03/11   4.7.2       �±�        �޸�(HDU֧�ֶ໭��)
===================================================================== */
BOOL32 CHdu2Instance::Hdu2ChnStopPlay( u8 byMode /* = MODE_BOTH */, u8 byHduSubChnId)						                            
{
	if (byMode == MODE_NONE)
	{
		Hdu2Print(HDU_LVL_ERROR,  "[Hdu2ChnStopPlay] byMode(%d)\n",byMode);
		return FALSE;
	}
	
	u8 byOldMode = m_cHdu2ChnMgrGrp.GetMode(byHduSubChnId);
	u8 byCurMode = MODE_NONE;

	if (MODE_BOTH == byOldMode && MODE_VIDEO == byMode)
	{
		byCurMode = MODE_AUDIO;
	}
	else if (MODE_BOTH == byOldMode && MODE_AUDIO == byMode)
	{
		byCurMode = MODE_VIDEO;
	}
	else
	{
		byCurMode = MODE_NONE;
	}

	m_cHdu2ChnMgrGrp.SetMode(byCurMode, byHduSubChnId);
	Hdu2Print(HDU_LVL_DETAIL, "[Hdu2ChnStopPlay] OldMediaMode: %d  StopMediaMode:%d  AfterStopMediaMode(%d) HduSubChnId: %d\n",
		byOldMode, byMode, byCurMode, byHduSubChnId);

	
	if(!m_cHdu2ChnMgrGrp.StopNetRecv(byMode, byHduSubChnId))
	{
		Hdu2Print(HDU_LVL_ERROR,  "[Hdu2ChnStopPlay]StopNetRecv Faileded MediaMode(%d) HduSubChnId(%d)!!!\n",
			byMode, byHduSubChnId);
		return FALSE;
	}
	
	if (MODE_BOTH == byMode || MODE_VIDEO == byMode )
	{
		// [2013/03/11 chenbing] ������ͨ��StopDecode�ὫbyMode���õ�m_cHdu2ChnMgrGrp��
		u8 bySubChnId = 0;
		for (bySubChnId=0; bySubChnId<HDU_MODEFOUR_MAX_SUBCHNNUM; bySubChnId++)
		{
			//��ǰģʽ������Ƶ,������Ƶ,����ɾ������ؼ�֡��ʱ��
			if(MODE_VIDEO == m_cHdu2ChnMgrGrp.GetMode(bySubChnId)
				|| MODE_BOTH == m_cHdu2ChnMgrGrp.GetMode(bySubChnId)
				)
			{
				break;
			}
		}
		//��ͨ����û����Ƶ, ȡ������ؼ�֡��ʱ��,�رս�����
		if ( bySubChnId >= HDU_MODEFOUR_MAX_SUBCHNNUM )
		{
			if(!m_cHdu2ChnMgrGrp.StopDecode(byMode))
			{
				Hdu2Print(HDU_LVL_ERROR,  "[Hdu2ChnStopPlay]StopDecode Faileded MediaMode(%d) HduSubChnId(%d)!!!\n",
					byMode, byHduSubChnId);
				return FALSE;
			}
			Hdu2Print(HDU_LVL_DETAIL,  "[Hdu2ChnStopPlay]All SubChnl MediaMode is Null, KillTimer EV_HDU_NEEDIFRAME_TIMER!!!\n");
			KillTimer(EV_HDU_NEEDIFRAME_TIMER);
			NextState((u32)emREADY);
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[Hdu2ChnStopPlay]SubChnl(%d) OldMode(%d) NewMode(%d) !!!\n",
				bySubChnId, m_cHdu2ChnMgrGrp.GetMode(bySubChnId), byMode);
		}
	}

	if (MODE_AUDIO == byMode)
	{
		if(!m_cHdu2ChnMgrGrp.StopDecode(byMode))
		{
			Hdu2Print(HDU_LVL_ERROR,  "[Hdu2ChnStopPlay]StopDecode Faileded MediaMode(%d) HduSubChnId(%d)!!!\n",
				byMode, byHduSubChnId);
			return FALSE;
		}
	}

    return TRUE;
}

/*====================================================================
����  : DaemonProcChangePlayPolicy
����  : ����ͨ����ʾ���Ըı䴦��
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2012/09/18   4.7         ��־��        ����
====================================================================*/
void  CHdu2Instance::DaemonProcChangePlayPolicy( CMessage* const pMsg ) 
{
	if ( NULL == pMsg)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[DaemonProcChangePlayPolicy]NULL == pMsg,So Return!\n");
		return;
	}
	u8 byIdleChnPlayPolicy = *(u8*)pMsg->content;
    for (u8 byLoop=0; byLoop<MAXNUM_HDU_CHANNEL; byLoop++)
    {
		post(MAKEIID(GetAppID(), byLoop+1), MCU_HDU_CHANGEPLAYPOLICY_NOTIF, &byIdleChnPlayPolicy,sizeof(u8));
	}
}

/*====================================================================
����  : ProcShowMode
����  : ��Ѱ��ǰͨ��ģʽ
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2013/03/11   4.7.2       �±�        �޸�(HDU�໭��֧��)
====================================================================*/
void CHdu2Instance::ProcShowMode()
{
	OspPrintf(TRUE, FALSE, "\n^^^^^^^^^^^^^^Current HduChnnl (%d)^^^^^^^^^^^^^^\n", GetInsID()-1);
	OspPrintf(TRUE, FALSE, "SubChnnl     Mode\n");
	for (u8 bySubChnId=0; bySubChnId<HDU_MODEFOUR_MAX_SUBCHNNUM; bySubChnId++)
	{
		OspPrintf(TRUE, FALSE, "%4d %11d\n", bySubChnId,  m_cHdu2ChnMgrGrp.GetMode(bySubChnId)	);
	}
}

/*====================================================================
����  : ProcCLearMode
����  : �����ǰͨ��ģʽ
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2013/03/11   4.7.2       �±�        �޸�(HDU�໭��֧��)
====================================================================*/
void CHdu2Instance::ProcClearMode()
{
	for (u8 bySubChnId=0; bySubChnId<HDU_MODEFOUR_MAX_SUBCHNNUM; bySubChnId++)
	{
		m_cHdu2ChnMgrGrp.SetMode(MODE_NONE, bySubChnId);
	}
}

/*====================================================================
����  : ProcChangePlayPolicy
����  : ����ͨ����ʾ���Ըı䴦��
����  : CMessage* const
���  : ��
����  : ��
ע    :
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��       �汾        �޸���      �޸�����
2012/09/18   4.7         ��־��        ����
====================================================================*/
void  CHdu2Instance::ProcChangePlayPolicy( CMessage* const pMsg ) 
{
	u8 byIdleChnPlayPolicy = *(u8*)pMsg->content;
	m_cHdu2ChnMgrGrp.SetVidPlayPolicy(byIdleChnPlayPolicy);
	Hdu2Print(HDU_LVL_KEYSTATUS,  "[ProcChangePlayPolicy]Chn(%d)  m_cVidDecoder.SetVidPlayPolicy(%d) Successed!\n",GetInsID(),byIdleChnPlayPolicy);
}

u8 CHdu2Instance::GetOldVersionEqpTypeBySubType ( u8 byHduEqpSubType )
{
	// ����4.7.2HDU2ģ��HDUע�ᵽ�ϰ汾MCU[6/3/2013 chendaiwei]
	u8 byEqpType = EQP_TYPE_HDU;
	if(byHduEqpSubType == HDU_SUBTYPE_HDU_H)
	{
		byEqpType = 13;//ԭEQP_TYPE_HDU_H
	}
	else if ( byHduEqpSubType == HDU_SUBTYPE_HDU_L)
	{
		byEqpType = 14;//ԭEQP_TYPE_HDU_L
	}
	else if ( byHduEqpSubType == HDU_SUBTYPE_HDU2 )
	{
		byEqpType = 15;//ԭ���壬�����ϰ汾HDU�汾��#define EQP_TYPE_HDU2 15
	}
	else if ( byHduEqpSubType == HDU_SUBTYPE_HDU2_L )
	{
		byEqpType = 12;//ԭ���壬�����ϰ汾HDU�汾��#define EQP_TYPE_HDU2_L 12
	}

	return byEqpType;
}

/* -------------------CHdu2Instance���ʵ��  end---------------------*/
/* ---------------------CHdu2Data���ʵ��  start---------------------*/

CHdu2Data::CHdu2Data()
{
	memset(this, 0, sizeof(CHdu2Data));
}
CHdu2Data::~CHdu2Data()
{
}
/* ---------------------CHdu2Data���ʵ��  end-----------------------*/
//END OF FILE

