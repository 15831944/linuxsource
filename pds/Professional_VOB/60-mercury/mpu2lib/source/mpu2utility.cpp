/*****************************************************************************
   ģ����      : mpu2lib
   �ļ���      : mpu2utility.cpp
   ����ļ�    : mpu2utility.h
   �ļ�ʵ�ֹ���: ����/ͨ�� ��/���� ʵ��
   ����        : ������
   �汾        : V4.7  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��       �汾        �޸���      �޸�����
   2011/12/07    4.7         ������      ����
******************************************************************************/
#include "mpu2utility.h"
#include "mpu2struct.h"
#include "mpu2inst.h"


s32 g_nmpubaslog = 1;

u16 g_wKdvlogMid = MID_PUB_ALWAYS;

/*=============================================================================
    �� �� ���� StrOfIP
    ��    �ܣ� ������IP��ַת���ַ���
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u32 dwIP   ������IP��ַ
    �� �� ֵ�� char * 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/02/28  3.6			����                  ����
    2009/03/14  4.6         �ű���                  ��mcu��ֲ����
=============================================================================*/
s8 * StrOfIP( u32 dwIP )
{
    dwIP = htonl(dwIP);
	static s8 strIP[17];  
	u8 *p = (u8 *)&dwIP;
	sprintf(strIP,"%d.%d.%d.%d%c",p[0],p[1],p[2],p[3],0);
	return strIP;
}


/*====================================================================
	����  : ipStr
	����  : ȡ��IP��ַ�ַ���
	����  : dwIp: IP��ַ
	���  : ��
	����  : IP��ַ��Ӧ���ַ���
	ע    : 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
====================================================================*/
s8* ipStr(u32 dwIP)
{
	static s8 achIPStr[4][32]={0};
	static u8 byIdx = 0;
    struct in_addr in;

	byIdx++;
	byIdx %= sizeof(achIPStr)/sizeof(achIPStr[0]);
    in.s_addr = dwIP;
	memset(achIPStr[byIdx], 0, sizeof(achIPStr[0]));
	
#ifdef _MSC_VER
	strncpy(achIPStr[byIdx],inet_ntoa(in),sizeof(achIPStr[0]));
#endif //_MSC_VER

#ifdef _LINUX_
	strncpy(achIPStr[byIdx],inet_ntoa(in),sizeof(achIPStr[0]));
#endif //_LINUX_
	
#ifdef _VXWORKS_
	inet_ntoa_b(in, achIPStr[byIdx]);
#endif //_VXWORKS_
	
	achIPStr[byIdx][31] = '\0'; // �ַ���������־
	return achIPStr[byIdx];
}

/*lint -save -esym(526, __builtin_va_start)*/
/*lint -save -esym(628, __builtin_va_start)*/
/*====================================================================
	����  : ErrorLog
	����  : ��ӡ�ȼ�Ϊerror��kdvlog
	����  : pszFmt: Ҫ��ӡ���ַ���
	���  : ��
	����  : ��
	ע    : 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��         �汾        �޸���        �޸�����
 	2011/12/07     v4.7			������		create    
====================================================================*/
void ErrorLog(s8* pszFmt,...)
{
	s8 achBuf[1024] = { 0 };
    va_list argptr ;
	memset(&argptr,0,sizeof(va_list));
    va_start( argptr, pszFmt );
    vsprintf( achBuf, pszFmt, argptr );
	LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS,achBuf);
    va_end( argptr );
	
	
}

/*====================================================================
	����  : KeyLog
	����  : ��ӡ�ȼ�Ϊkeystatus��kdvlog
	����  : pszFmt: Ҫ��ӡ���ַ���
	���  : ��
	����  : ��
	ע    : 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��         �汾        �޸���        �޸�����
 	2011/12/07     v4.7			������		create    
====================================================================*/
void KeyLog(s8* pszFmt, ...)
{
	//setmpulog ��log�ȼ�������
	s8 achBuf[1024] = { 0 };
    va_list argptr ;
	memset(&argptr,0,sizeof(va_list));
	
    va_start( argptr, pszFmt ); 
    vsprintf( achBuf, pszFmt, argptr );
	LogPrint(LOG_LVL_KEYSTATUS,g_wKdvlogMid,achBuf);
    va_end( argptr );
}

/*====================================================================
	����  : WarningLog
	����  : ��ӡ�ȼ�Ϊwarning��kdvlog
	����  : pszFmt: Ҫ��ӡ���ַ���
	���  : ��
	����  : ��
	ע    : 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��         �汾        �޸���        �޸�����
 	2011/12/07     v4.7			������		create    
====================================================================*/
void WarningLog(s8* pszFmt,...)
{
	s8 achBuf[1024] = { 0 };
    va_list argptr;
	memset(&argptr,0,sizeof(va_list));
	
    va_start( argptr, pszFmt );
    vsprintf( achBuf, pszFmt, argptr );
	LogPrint(LOG_LVL_WARNING,MID_PUB_ALWAYS,achBuf);
    va_end( argptr );
}

/*====================================================================
	����  : DetailLog
	����  : ��ӡ�ȼ�Ϊdetail��kdvlog
	����  : pszFmt: Ҫ��ӡ���ַ���
	���  : ��
	����  : ��
	ע    : 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��         �汾        �޸���        �޸�����
 	2011/12/07     v4.7			������		create    
====================================================================*/
void DetailLog(s8* pszFmt,...)
{
	s8 achBuf[1024] = { 0 };
    va_list argptr;
	memset(&argptr,0,sizeof(va_list));
	
    va_start( argptr, pszFmt );
    vsprintf( achBuf, pszFmt, argptr );
	LogPrint(LOG_LVL_DETAIL,g_wKdvlogMid,achBuf);
    va_end( argptr );
}



/*====================================================================
	����  : mpulog
	����  : mpulog
	����  : u8 byLevel
	���  : ��
	����  : 
	ע    : 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2009/03/14  4.6         �ű���        ��ӡ����֧��
====================================================================*/
/*lint -save -e438 -e530*/
void mpulog( u8 byLevel, s8* pszFmt, ... )
{
    s8 * pchLevelName[] = {(s8 *)"[CRIT]", (s8 *)"[WARN]", (s8 *)"[INFO]", "[DETAIL]", ""};

    s8 achPrintBuf[512];
    s32  nBufLen = 0;
    va_list argptr;
    if ( g_nmpubaslog >= byLevel )
    {		  
        nBufLen = sprintf( achPrintBuf, "[Mpu] %s: ", pchLevelName[byLevel] );         
        va_start( argptr, pszFmt );
        vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
        va_end( argptr ); 
        OspPrintf( TRUE, FALSE, achPrintBuf ); 
    }
}
/*lint -restore*/
/*====================================================================
	����  : mpulog
	����  : mpulog
	����  : 
	���  : ��
	����  : 
	ע    : 
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2009/03/14  4.6         �ű���        ��ӡ����֧��
====================================================================*/
/*lint -save -e438 -e530*/
void mpulogall( s8* pszFmt, ... )
{
    if( g_nmpubaslog >= MPU_DETAIL )
    {
        s8 achPrintBuf[512];
        s32  nBufLen = 0;
        va_list argptr;

        nBufLen = sprintf( achPrintBuf, "[MpuDbg]: " );         
        va_start( argptr, pszFmt );
        vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
        va_end( argptr ); 
        OspPrintf( TRUE, FALSE, achPrintBuf ); 
    }
    return;
}
/*lint -restore*/
/*==============================================================================
������    :  ConvertVmpStyle2ChnnlNum
����      :  VMP style -> ͨ����Ŀ
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  u8
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-5-9	4.6				Ѧ��							create
==============================================================================*/
u8 ConvertVmpStyle2ChnnlNum(u8 byVmpStyle)
{
	switch (byVmpStyle)
	{
	case VMP_STYLE_ONE:
		return 1;

	case VMP_STYLE_VTWO:
	case VMP_STYLE_HTWO:
		return 2;

	case VMP_STYLE_THREE:
		return 3;

	case VMP_STYLE_FOUR:
	case VMP_STYLE_SPECFOUR:
		return 4;

	case VMP_STYLE_SIX:
		return 6;

	case VMP_STYLE_SEVEN:
		return 7;

	case VMP_STYLE_EIGHT:
		return 8;

	case VMP_STYLE_NINE:
		return 9;

	case VMP_STYLE_TEN:
	case VMP_STYLE_TEN_M:
		return 10;

	case VMP_STYLE_THIRTEEN:
	case VMP_STYLE_THIRTEEN_M:
		return 13;

	case VMP_STYLE_SIXTEEN:
		return 16;

	case VMP_STYLE_TWENTY:
		return 20;
	
	case VMP_STYLE_THREE_2BOTTOMRIGHT:
	case VMP_STYLE_THREE_2BOTTOM2SIDE:
	case VMP_STYLE_THREE_2BOTTOMLEFT:
	case VMP_STYLE_THREE_VERTICAL:
		return 3;

	case VMP_STYLE_FOUR_3BOTTOMMIDDLE:
		return 4;

	case VMP_STYLE_TWENTYFIVE:
		return 25;

	default:
		return 0;
	}
}


/*=============================================================================
  �� �� ���� ConvertVcStyle2HardStyle
  ��    �ܣ� �ɻ�ط��õ��ײ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 oldstyle
  �� �� ֵ�� u8 
=============================================================================*/
u8 ConvertVcStyle2HardStyle( u8 byOldstyle )
{
    u8 byStyle;
    switch ( byOldstyle )
    {
    case VMP_STYLE_ONE:  // һ����
        byStyle = (u8)MULPIC_TYPE_ONE;  // ������
        break;

    case VMP_STYLE_VTWO: // �����棺���ҷ�
        byStyle = (u8)MULPIC_TYPE_VTWO;  // �����棺���ҷ�
        break;

    case VMP_STYLE_HTWO:  // ������: һ��һС
        byStyle = (u8)MULPIC_TYPE_ITWO; // �����棺 һ��һС������ֳ�9�ݷ��ͣ�
        break;

    case VMP_STYLE_THREE: // ������
        byStyle = (u8)MULPIC_TYPE_THREE; // ������
        break;

    case VMP_STYLE_FOUR: // �Ļ���
        byStyle = (u8)MULPIC_TYPE_FOUR;  // �Ļ���
        break;

    case VMP_STYLE_SIX: //������
        byStyle = (u8)MULPIC_TYPE_SIX; //������
        break;

    case VMP_STYLE_EIGHT: //�˻���
        byStyle = (u8)MULPIC_TYPE_EIGHT; //�˻���
        break;
    case VMP_STYLE_NINE: //�Ż���
        byStyle = (u8)MULPIC_TYPE_NINE; //�Ż���
        break;

    case VMP_STYLE_TEN: //ʮ����
        byStyle = (u8)MULPIC_TYPE_TEN; //ʮ���棺�����С������ֳ�4�ݷ��ͣ�
        break;

    case VMP_STYLE_THIRTEEN:  //ʮ������
        byStyle = (u8)MULPIC_TYPE_THIRTEEN; //ʮ������
        break;

    case VMP_STYLE_SIXTEEN: //ʮ������
        byStyle = (u8)MULPIC_TYPE_SIXTEEN; //ʮ������
        break;

    case VMP_STYLE_TWENTY: //��ʮ����
        byStyle = (u8)MULPIC_TYPE_TWENTY;
        break;
        
    case VMP_STYLE_SPECFOUR://�����Ļ��� 
        byStyle = (u8)MULPIC_TYPE_SFOUR; //�����Ļ��棺һ����С������ֳ�4�ݷ��ͣ�
        break;

    case VMP_STYLE_SEVEN: //�߻���
        byStyle = (u8)MULPIC_TYPE_SEVEN; //�߻��棺������С������ֳ�4�ݷ��ͣ�
        break;

	case VMP_STYLE_THREE_2BOTTOMRIGHT:	   //�����棬2��С���ڵײ�����
		byStyle = (u8)MULPIC_TYPE_THREE_2IBR; 
		break;

	case VMP_STYLE_THREE_2BOTTOM2SIDE:	   //�����棬2��С���ڵײ�����
		byStyle = (u8)MULPIC_TYPE_THREE_2IB; 
		break;

	case VMP_STYLE_THREE_2BOTTOMLEFT:	   //�����棬2��С���ڵײ�����
		byStyle = (u8)MULPIC_TYPE_THREE_2IBL; 
		break;

	case VMP_STYLE_THREE_VERTICAL:	   //��ֱ�ȴ�3����
		byStyle = (u8)MULPIC_TYPE_VTHREE; 
		break;

	case VMP_STYLE_FOUR_3BOTTOMMIDDLE:	   //�Ļ��棬3���ڵײ�����
		byStyle = (u8)MULPIC_TYPE_FOUR_3IB; 
		break;
		
	case VMP_STYLE_TEN_M   :	  //ʮ���棬�м����������¸�4��
		byStyle = (u8)MULPIC_TYPE_TEN_MID_LR; 
		break;
		
	case VMP_STYLE_THIRTEEN_M  :	  //ʮ�����棬�м�һ����ģ������м���
		byStyle = (u8)MULPIC_TYPE_THIRTEEN_MID; 
		break;

	case VMP_STYLE_TWENTYFIVE  :	  //��ʮ�廭��
		byStyle = (u8)MULPIC_TYPE_TWENTYFIVE; 
		break;
    default:
        byStyle = (u8)MULPIC_TYPE_VTWO;    // Ĭ�������棺���ҷ�
        break;
    }
    return byStyle;
}


/*==============================================================================
������    :  AdjustPrsMaxSpan
����      :  �������prsSpan
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-5-9					Ѧ��							create
==============================================================================*/
u16 AdjustPrsMaxSpan( u8 byVmpStyle )
{
	u16 wPrsMaxSpan = 480;
	u8 byRealChnnlNum = ConvertVmpStyle2ChnnlNum(byVmpStyle);
	if( byRealChnnlNum == 0)
	{
		OspPrintf(TRUE, FALSE, "[AdjustPrsMaxSpan] unexpected chnnl num:%u!\n", byRealChnnlNum);
		return 0;
	}
	if( byRealChnnlNum <= 4)
	{
		wPrsMaxSpan = 960;
	}
	else if ( byRealChnnlNum <= 9 )
	{
		wPrsMaxSpan = 738;
	}
	else
	{
		wPrsMaxSpan = 480;
	}
// 	if(byVmpStyle != 0)
// 	{
// 		wPrsMaxSpan = wPrsMaxSpan * 20 /byRealChnnlNum;
// 		if(wPrsMaxSpan > 738)
// 		{
// 			wPrsMaxSpan = 738;
// 		}
// 	}
	return wPrsMaxSpan;
	
}

void GetRSParamByPos(u8 byPos, TRSParam &tRSParam)
{
    memset(&tRSParam, 0, sizeof(tRSParam));
    
    if (byPos >= MAXNUM_MPU2VMP_MEMBER)
    {
        OspPrintf(TRUE, FALSE, "[GetRSParamByPos] unexpected pos.%d, ignore it!\n", byPos);
        return;
    }

#ifndef WIN32
	
#ifndef _MPUB_256_
    if (byPos < 4)
    {
        tRSParam.m_wFirstTimeSpan = 40;
        tRSParam.m_wSecondTimeSpan = 360;
        tRSParam.m_wThirdTimeSpan = 960;
        tRSParam.m_wRejectTimeSpan = 1920;
    }
//     else if (byPos < 9)
//     {
//         tRSParam.m_wFirstTimeSpan = 40;
//         tRSParam.m_wSecondTimeSpan = 240;
//         tRSParam.m_wThirdTimeSpan = 480;
//         tRSParam.m_wRejectTimeSpan = 960;
//     }
    else
    {
        tRSParam.m_wFirstTimeSpan = 40;
        tRSParam.m_wSecondTimeSpan = 100;
        tRSParam.m_wThirdTimeSpan = 200;
        tRSParam.m_wRejectTimeSpan = 400;
    }
#else
	//B ��
	if (byPos < 4)
    {
        tRSParam.m_wFirstTimeSpan = 40;
        tRSParam.m_wSecondTimeSpan = 360;
        tRSParam.m_wThirdTimeSpan = 960;
        tRSParam.m_wRejectTimeSpan = 1920;
    }
	else if (byPos < 9)
    {
         tRSParam.m_wFirstTimeSpan = 40;
         tRSParam.m_wSecondTimeSpan = 240;
         tRSParam.m_wThirdTimeSpan = 480;
         tRSParam.m_wRejectTimeSpan = 960;
    }
    else
    {
        tRSParam.m_wFirstTimeSpan = 40;
        tRSParam.m_wSecondTimeSpan = 120;
        tRSParam.m_wThirdTimeSpan = 240;
        tRSParam.m_wRejectTimeSpan = 480;
    }


#endif

#endif

    return;
}

void ConvertToRealFR(THDAdaptParam& tAdptParm)
{
	if (tAdptParm.GetVidType() != MEDIA_TYPE_H264)
	{
		u8 byFrameRate = tAdptParm.GetFrameRate();
		switch(byFrameRate)
		{
		case VIDEO_FPS_2997_1:
			byFrameRate = 30;
			break;
		case VIDEO_FPS_25:
			byFrameRate = 25;
			break;
		case VIDEO_FPS_2997_2:
			byFrameRate = 15;
		    break;
		case VIDEO_FPS_2997_3:
			byFrameRate = 10;
		    break;
		case VIDEO_FPS_2997_4:
		case VIDEO_FPS_2997_5:
			byFrameRate = 6;
			break;
		case VIDEO_FPS_2997_6:
			byFrameRate = 5;
			break;
		case VIDEO_FPS_2997_30:
			byFrameRate = 1;
			break;
		case VIDEO_FPS_2997_7P5:
			byFrameRate = 4;
			break;
		case VIDEO_FPS_2997_10:
			byFrameRate = 3;
			break;
		case VIDEO_FPS_2997_15:
			byFrameRate = 2;
			break;
		default:
			OspPrintf(TRUE, FALSE, "[ConvertToRealFR] undef non264 framerate%d\n", byFrameRate);
		    break;
		}

		tAdptParm.SetFrameRate(byFrameRate);
	}
}

void ConverToRealParam(u8 byBasIdx, u8 byChnId,u8 byOutIdx,THDAdaptParam& tBasParam)
{
	THDAdaptParam tTempParam = tBasParam;
	u16 	   wWidth = tBasParam.GetWidth(); 
    u16 	   wHeight= tBasParam.GetHeight();
    u8		   byFrameRate= tBasParam.GetFrameRate();
	if ( g_tResBrLimitTable.GetRealResFrameRatebyBitrate(tBasParam.GetVidType(),wWidth,wHeight,byFrameRate,tBasParam.GetBitrate(),tBasParam.GetProfileType()) )
	{
		tBasParam.SetResolution(wWidth,wHeight);
		tBasParam.SetFrameRate(byFrameRate);
		KeyLog("[ConverToRealParam]BasIdx:%d,chnid:%d,outIdx:%d convert width:%d-height:%d framerate:%d to width:%d-height:%d framerate:%d\n",
			   byBasIdx,byChnId, byOutIdx, tTempParam.GetWidth(),tTempParam.GetHeight(),tTempParam.GetFrameRate(),wWidth,wHeight,byFrameRate
			  );
	}
}

//END OF FILE

