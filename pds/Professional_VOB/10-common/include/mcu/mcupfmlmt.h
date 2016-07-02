/*****************************************************************************
   ģ����      : mcu
   �ļ���      : mcupfmlmt.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: MCU ҵ��MCS ʹ�ã�����MCU��������ͨ�õ�ʵ��
   ����        : �ű��� ���� 
   �汾        : V4.0  Copyright(C) 2001-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   ʹ��˵����
        MCS������ͷ�ļ�ǰ����Ԥ����� MCS_PFMLMT.�������ֻ����ͷ���ļ�һ��
         
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2007/01/29  4.0         ����      �������ļ�
   2008/08/06  4.5         ����      H.264 HD �޸�
******************************************************************************/

#ifndef __MCUPFMLMT_H_
#define __MCUPFMLMT_H_

#include "mcustruct.h"
#include "mcuerrcode.h"

TMcu8kbPfmFilter CMcuPfmLmt::m_tFilter;

// �ڲ��ӿ�
// ���ʴ��� x
#define IS_BITRATE_GR(tConf, br)  ( tConf.GetBitRate() > (br)  )
// ���ʴ��ڵ��� x
#define IS_BITRATE_GE(tConf, br)  ( tConf.GetBitRate() >= (br) )
// ����С�� x
#define IS_BITRATE_LO(tConf, br)  ( tConf.GetBitRate() < (br)  )
// ����С�ڵ��� x
#define IS_BITRATE_LE(tConf, br)  ( tConf.GetBitRate() <= (br) )

// �Ƿ�H264���飬���˫��ʽ���飬һ��Ҫ��H.264���ó�����ʽ
inline BOOL32 IsConfH264(const TConfInfo &tConfInfo)
{
    return ( MEDIA_TYPE_H264 == tConfInfo.GetMainVideoMediaType() );
}

// �Ƿ�G729����
inline BOOL32 IsConfG729(const TConfInfo &tConfInfo)
{
    return ( MEDIA_TYPE_G729 == tConfInfo.GetMainAudioMediaType() );
}

// �����Ƿ�������������
inline BOOL32 IsConfVacing(const TConfInfo &tConfInfo)
{
    return ( tConfInfo.m_tStatus.IsVACing() );
}

// �����Ƿ����ڻ���(�������ơ����ܡ�VAC�������)
inline BOOL32 IsConfMixing(const TConfInfo &tConfInfo)
{
    return ( tConfInfo.m_tStatus.IsMixing() || tConfInfo.m_tStatus.IsVACing() );
}

// �����Ƿ����ڻ���(�������ƺ�����)
inline BOOL32 IsConfMixingNoVac(const TConfInfo &tConfInfo)
{
    return ( tConfInfo.m_tStatus.IsMixing() );
}

// �����Ƿ������κ����͵�����
inline BOOL32 IsConfBasing(const TConfInfo &tConfInfo)
{
    return tConfInfo.m_tStatus.IsConfAdapting();
}

// �����Ƿ��ڻ���ϳ�
inline BOOL32 IsConfVmping(const TConfInfo &tConfInfo)
{
    return ( CONF_VMPMODE_NONE != tConfInfo.m_tStatus.GetVMPMode() );
}


BOOL32 g_bPrintPfmMsg = false;
void pfmlog(s8 * fmt, ... )
{
    s8  achBuf[255];
    va_list argptr;	
    if( g_bPrintPfmMsg )
    {
        s32 nPrefix = sprintf( achBuf, "[PFMLimit]:" );
        va_start( argptr, fmt );    
        vsprintf( achBuf + nPrefix, fmt, argptr );   
        OspPrintf(TRUE, FALSE, achBuf); 
        va_end(argptr); 
    }    
}

/*=============================================================================
    �� �� ���� GetMaxVmpStyle
    ��    �ܣ� �жϵ�ǰVMP���֧�ֵĻ���ϳ�·��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TConfInfo &tConfInfo         [in]  ���������Ϣ
               u8 byMtNum                   [in]  �����ն�����
               u8 byMixingMtNum             [in]  ���ڲ������(���ܻ���)���ն�����
               u16 &wError                  [out] ���صĴ�����
    �� �� ֵ�� BOOL32
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2007/01/29  4.0			����                  ����
=============================================================================*/
u8 CMcuPfmLmt::GetMaxOprVmpStyle( const TConfInfo &tConfInfo, u8 byMtNum, u8 byMixingMtNum, u16 &wError )
{
	// xliang [2/25/2009] Ŀǰ�ýӿ�ֻ��8000b����£�mcs���
    wError = 0;
    u8 byVmpStyle = VMP_STYLE_NONE;


#if ( (!defined(_MINIMCU_)) && (!defined(MCS_PFMLMT)) )
    return VMP_STYLE_TWENTY/*VMP_STYLE_SIXTEEN*/;   // ���ݵ�ǰ������ҵ���ж��߼�
#else
    if (!m_tFilter.IsEnable())
    {
        // guzh [2/12/2007] FIXME ���ݵ�ǰ������ҵ���ж��߼�������ִ���
        return VMP_STYLE_TWENTY/*VMP_STYLE_SIXTEEN*/;   
    }
        
    BOOL32 bConfVideoH264  = IsConfH264(tConfInfo);     //������Ƶ��ʽH264
    BOOL32 bConfAudioG729  = IsConfG729(tConfInfo);     //������Ƶ��ʽG729

    // VMP��VAC���⡣�Ѿ���VAC��֧��Vmp
    if ( IsConfVacing(tConfInfo) )
    {
        wError = ERR_MCU_VACNOVMP;
        return VMP_STYLE_NONE;
    }

    // û������ģʽ�Ļ��������ݵ�ǰ������ҵ���ж��߼�
    if ( !IsConfMixing(tConfInfo) )
    {
        // 264-Auto֧��1��2��3��4��9��16�������ʽֻ֧��1��2��3��4
		if( !bConfVideoH264 && IS_BITRATE_GR(tConfInfo, 2048) )
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_PFM, "[GetMaxOprVmpStyle] MP4 conf's BR should not be over 2048 !\n" );
			wError = ERR_MCU_VMPNOSUPPORT;
			return VMP_STYLE_NONE;
		}
		else if ( bConfVideoH264 && IS_BITRATE_GR(tConfInfo, 4096) )
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_PFM, "[GetMaxOprVmpStyle] H264 conf's BR should not be over 4096 !\n" );
			wError = ERR_MCU_VMPNOSUPPORT;
			return VMP_STYLE_NONE;
		}
        else 
		{
            // [zw] [09/08/2008] �������VMP_STYLE_NONE�Ͳ�ֱ��return����������return����������Ϳ�����ҵ����չ�ĺ��������ˡ�
            byVmpStyle = ( bConfVideoH264 ? ( !tConfInfo.IsVmpAutoAdapt() ? VMP_STYLE_NONE : VMP_STYLE_SIXTEEN) : VMP_STYLE_FOUR );
		}
    }
	else
	{
		// �ڻ���       
		if ( bConfVideoH264 )
		{
			if ( !tConfInfo.IsVmpAutoAdapt() )
			{
				wError = ERR_MCU_VMPNOSUPPORT;
				return VMP_STYLE_NONE;
			}

			// 4M����
			if ( IS_BITRATE_GR(tConfInfo, 4096) )
			{
				wError = ERR_MCU_VMPNOSUPPORT;
				LogPrint(LOG_LVL_ERROR, MID_MCU_PFM, "[GetMaxOprVmpStyle] H264 conf's BR should not be over 4096 !\n" );
				return  VMP_STYLE_NONE;
			}
        
			// H264-G729
			if ( bConfAudioG729 )
			{
				if ( byMixingMtNum > m_tFilter.byMaxNumMixMultiG729 ) 
				{
					wError = ERR_MCU_VMPNOSUPPORTONMIX;
					LogPrint(LOG_LVL_ERROR, MID_MCU_PFM,  "[GetMaxOprVmpStyle] Mix mem is over.%d, support no VMP in H264-G729!\n", m_tFilter.byMaxNumMixMultiG729 );
					return VMP_STYLE_NONE;
				}
				else 
				{
					// 768k �� 4M
					if ( IS_BITRATE_GR(tConfInfo, 768) )
					{
						wError = ERR_MCU_VMPNOSUPPORTONMIX;
						LogPrint(LOG_LVL_ERROR, MID_MCU_PFM,  "[GetMaxOprVmpStyle] Mix member is Not zero, support no VMP in H264-G729 at Br. %d!\n", 768 );
						return VMP_STYLE_NONE;
					}
					// 768K ����
					else
					{
						byVmpStyle = VMP_STYLE_NINE;
					}
				}
			}
			// H264-MP3
			else
			{
				if ( byMixingMtNum > m_tFilter.byMaxNumMixMultiMp3 ) 
				{
					wError = ERR_MCU_VMPNOSUPPORTONMIX;
					LogPrint(LOG_LVL_ERROR, MID_MCU_PFM,  "[GetMaxOprVmpStyle] Mix mem is over.%d, support no VMP in H264-MP3!\n", m_tFilter.byMaxNumMixMultiMp3 );
					return VMP_STYLE_NONE;
				}
				else
				{
					// 1M �� 4M
					if ( IS_BITRATE_GR(tConfInfo, 1024) )
					{
						byVmpStyle = VMP_STYLE_FOUR;
					}
					// 1M ����
					else
					{
						byVmpStyle = VMP_STYLE_NINE;
					}
				}
			}
		}
		else // MPEG4
		{
			if ( IS_BITRATE_GR(tConfInfo, 2048) )
			{
				wError = ERR_MCU_VMPNOSUPPORT;
				LogPrint(LOG_LVL_ERROR, MID_MCU_PFM,  "[GetMaxOprVmpStyle] MPEG4 conf's BR should not be over 2048 !\n" );
				return  VMP_STYLE_NONE;
			}

			// MPEG4-G729
			if ( bConfAudioG729 )
			{
				if ( byMixingMtNum > m_tFilter.byMaxNumMixMultiG729 ) 
				{
					wError = ERR_MCU_VMPNOSUPPORTONMIX;
					LogPrint(LOG_LVL_ERROR, MID_MCU_PFM,  "[GetMaxOprVmpStyle] Mix mem is over.%d, support no VMP in MP4-G729!\n", m_tFilter.byMaxNumMixMultiG729 );
					return VMP_STYLE_NONE;
				}
				else
				{
					// 768k �� 2M
					if ( IS_BITRATE_GR(tConfInfo, 768) )
					{
						wError = ERR_MCU_VMPNOSUPPORTONMIX;
						LogPrint(LOG_LVL_ERROR, MID_MCU_PFM,  "[GetMaxOprVmpStyle] Mix member is Not zero, support no VMP in H264-G729 at Br. %d!\n", 768 );
						return VMP_STYLE_NONE;
					}
					// 768K ����
					else
					{
						byVmpStyle = VMP_STYLE_FOUR;
					}
				}
			}
			// MPEG4-MP3
			else
			{
				if ( byMixingMtNum > m_tFilter.byMaxNumMixMultiMp3 ) 
				{
					wError = ERR_MCU_VMPNOSUPPORTONMIX;
					LogPrint(LOG_LVL_ERROR, MID_MCU_PFM, "[GetMaxOprVmpStyle] Mix mem is over.%d, support no VMP in MP4-MP3!\n", m_tFilter.byMaxNumMixMultiMp3 );
					return VMP_STYLE_NONE;
				}
				else
				{
					byVmpStyle = VMP_STYLE_FOUR;
				}
			}
		}
	}



    // [zw] [09/08/2008] ҵ����չ��������8000B����������֧���Ļ���
/*
#ifdef _MINIMCU_
    if ( IsConfFormatHD(tConfInfo) )
    {
        if ( (VMP_STYLE_DYNAMIC  != byVmpStyle)       
          && (VMP_STYLE_SPECFOUR != byVmpStyle) 
          && (VMP_STYLE_NONE     != byVmpStyle) 
          && (VMP_STYLE_ONE      != byVmpStyle) 
          && (VMP_STYLE_VTWO     != byVmpStyle) 
          && (VMP_STYLE_HTWO     != byVmpStyle)
          && (VMP_STYLE_THREE    != byVmpStyle)
          && (VMP_STYLE_FOUR     != byVmpStyle) )
        {
            byVmpStyle = VMP_STYLE_FOUR;
        }
    }
#endif
*/	

    return byVmpStyle;

#endif
}

/*=============================================================================
    �� �� ���� GetMaxDynVmpStyle
    ��    �ܣ� ��ȡ��ǰ��֧�ֵĶ�̬�ϳɷ�񡣲�֧��MCS���ʡ�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TConfInfo &tConfInfo         [in]  ���������Ϣ
               u8 byMtNum                   [in]  �����ն�����
               u8 byMixingMtNum             [in]  ���ڲ������(���ܻ���)���ն�����
               u16 &wError                  [out] ���صĴ�����
    �� �� ֵ�� BOOL32
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2007/01/31  4.0			����                  ����
=============================================================================*/    
u8 CMcuPfmLmt::GetMaxDynVmpStyle( const TConfInfo &tConfInfo, u8 byMtNum, u8 byMixingMtNum, u16 &wError )
{
#if (!defined(_MINIMCU_)) 
    return VMP_STYLE_SIXTEEN;
#else

    u8 byExpVmpStyleByMt;   // ����MT����������������
    u8 byResultStyle = VMP_STYLE_NONE;
    BOOL32 bConfH264 = IsConfH264(tConfInfo);

    if (bConfH264 && !tConfInfo.IsVmpAutoAdapt())
    {
        wError = ERR_MCU_VMPNOSUPPORT;
        return VMP_STYLE_NONE;
    }

    if (byMtNum <= 2)
    {
        byExpVmpStyleByMt = VMP_STYLE_VTWO;
    }
    else if (byMtNum <= 3)
    {
        byExpVmpStyleByMt = VMP_STYLE_THREE;
    }
    else if (byMtNum <= 4)
    {
        byExpVmpStyleByMt = VMP_STYLE_FOUR;
    }
    else if (byMtNum <= 9)
    {
        byExpVmpStyleByMt = (bConfH264 ? VMP_STYLE_NINE : VMP_STYLE_FOUR);        
    }
    else
    {
        byExpVmpStyleByMt = (bConfH264 ? VMP_STYLE_SIXTEEN : VMP_STYLE_FOUR);        
    }
    
    if (!m_tFilter.IsEnable())
    {
        byResultStyle = byExpVmpStyleByMt;
    }
    else
    {
        // ��ǰʣ�������ķ��
        u8 byCurCapStyle = GetMaxOprVmpStyle(tConfInfo, byMtNum, byMixingMtNum, wError);
        switch ( byCurCapStyle )
        {
        case VMP_STYLE_NONE:
            byResultStyle = VMP_STYLE_NONE;
            break;
        case VMP_STYLE_FOUR:
            switch (byExpVmpStyleByMt)
            {
            case VMP_STYLE_NINE:
            case VMP_STYLE_SIXTEEN:
                byResultStyle = VMP_STYLE_FOUR;
                break;
            default:
                byResultStyle = byExpVmpStyleByMt;
            }
            break;
        case VMP_STYLE_NINE:
            switch (byExpVmpStyleByMt)
            {
            case VMP_STYLE_SIXTEEN:
                byResultStyle = VMP_STYLE_NINE;
                break;
            default:
                byResultStyle = byExpVmpStyleByMt;
            }
			break;
        case VMP_STYLE_SIXTEEN:
        default:
            byResultStyle = byExpVmpStyleByMt;
            break;
        }
    }
    return byResultStyle;
#endif
}

/*=============================================================================
�� �� ���� IsVmpOprSupported
��    �ܣ� �жϵ�ǰ��VMP�����ܷ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TConfInfo &tConfInfo         [in]  ���������Ϣ
               TVMPParam &tNewVmpParam      [in]  �µ�VMP�������ﵽ��Param
               u8 byMtNum                   [in]  �����ն�����
               u8 byMixingMtNum             [in]  ���ڲ������(���ܻ���)���ն�����
               u16 &wError                  [out] ���صĴ�����
    �� �� ֵ�� BOOL32
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2007/01/25  4.0			�ű���                  ����
=============================================================================*/
BOOL32 CMcuPfmLmt::IsVmpOprSupported( const TConfInfo &tConfInfo, u8 byNewStyle, u8 byMtNum, u8 byMixingMtNum, u16 &wError )
{
#if ( (!defined(_MINIMCU_)) && (!defined(MCS_PFMLMT)) )
    return TRUE;
#else
    if (!m_tFilter.IsEnable())
        return TRUE;

    u8 byCapVmpStyle = GetMaxOprVmpStyle( tConfInfo, byMixingMtNum, byMixingMtNum, wError );

    // ����
    BOOL32 bSupportVMPOpr = TRUE;

    // ֧�ֵ�����
    switch( byCapVmpStyle )
    {
    case VMP_STYLE_FOUR:

        switch( byNewStyle )
        {
        case VMP_STYLE_DYNAMIC:
        case VMP_STYLE_ONE:
        case VMP_STYLE_VTWO:
        case VMP_STYLE_THREE:
        case VMP_STYLE_FOUR:
            break;
        default:
			wError = ERR_MCU_VMPNOSUPPORTONMIX;
            bSupportVMPOpr = FALSE;
            return FALSE;
        }
        break;

    case VMP_STYLE_NINE:

        switch( byNewStyle )
        {
        case VMP_STYLE_DYNAMIC:
        case VMP_STYLE_ONE:
        case VMP_STYLE_VTWO:
        case VMP_STYLE_THREE:
        case VMP_STYLE_FOUR:
        case VMP_STYLE_NINE:
            break;
        default:
			wError = ERR_MCU_VMPNOSUPPORTONMIX;
            bSupportVMPOpr = FALSE;
            break;
        }
        break;

    case VMP_STYLE_SIXTEEN:
        switch(byNewStyle)
        {
        case VMP_STYLE_DYNAMIC:
        case VMP_STYLE_ONE:
        case VMP_STYLE_VTWO:
        case VMP_STYLE_THREE:
        case VMP_STYLE_FOUR:
        case VMP_STYLE_NINE:
        case VMP_STYLE_SIXTEEN:
            break;
        default:
			wError = ERR_MCU_VMPNOSUPPORTONMIX;
            bSupportVMPOpr = FALSE;
            break;
        }
		break;

    default:
        bSupportVMPOpr = FALSE;
        break;
    }

    if ( !bSupportVMPOpr )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_PFM,  "[IsVmpOprSupported] Result: failed due to style.%d needed while style.%d support only !\n",
                byNewStyle, byCapVmpStyle );
        return FALSE;
    }

    return TRUE;
#endif // 8000B / MCS
}


/*=============================================================================
    �� �� ���� GetMaxOprMixNum
    ��    �ܣ� �жϵ�ǰ��������õĻ���(�������ƺ�����)�ն���
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TConfInfo &tConfInfo     [in]  ������Ϣ
    �� �� ֵ�� BOOL32
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2007/01/29  4.0			����                  ����
=============================================================================*/
u8 CMcuPfmLmt::GetMaxOprMixNum( const TConfInfo &tConfInfo, u16 &wError )
{
#if ( (!defined(_MINIMCU_)) && (!defined(MCS_PFMLMT)) )
    return MAXNUM_MIXER_CHNNL;
#else
    
    if (!m_tFilter.IsEnable())
        return MAXNUM_MIXER_CHNNL;

	//������Ƶ��ʽH264
	BOOL32 bVideoTypeH264 = IsConfH264(tConfInfo);
    //������Ƶ��ʽG729
    BOOL32 bAudioTypeG729 = IsConfG729(tConfInfo);
	//�������ʳ�4M, zgc, 2007-11-02
	BOOL32 bConfBrLE4M	= IS_BITRATE_LE(tConfInfo, 4096);	
    //�������ʳ�2M
    BOOL32 bConfBrLE2M    = IS_BITRATE_LE(tConfInfo, 2048);  
    //�������ʳ�768K
    BOOL32 bConfBrLE768K  = IS_BITRATE_LE(tConfInfo, 768);
	//�������ʳ�1024K
    BOOL32 bConfBrLE1024K  = IS_BITRATE_LE(tConfInfo, 1024);
	//�������ʳ�1536K
    BOOL32 bConfBrLE1536K  = IS_BITRATE_LE(tConfInfo, 1536);

	TConfStatus tConfStatus = tConfInfo.m_tStatus;
	u8 byVmpStyle = tConfStatus.GetVmpStyle();

	u8 byMediaTypeComb = 0;
	byMediaTypeComb |= bAudioTypeG729 ? 0x01 : 0x00 ;
	byMediaTypeComb |= bVideoTypeH264 ? 0x02 : 0x00 ;
    
    //��ǰ����֧�ֵ���MIX��Ա����
    u8 byMaxNum = 0;
    
    // ������
    if ( IsConfBasing(tConfInfo) )
    {
        wError = ERR_MCU_MIXNOSUPPORTONBAS;
        if ( bConfBrLE2M )
        {
            byMaxNum = ( bAudioTypeG729 ?  m_tFilter.byMaxNumMixMultiG729 : m_tFilter.byMaxNumMixMultiMp3 );
        }
        else
        {            
            byMaxNum = 0;
        }
    }
    else if ( IsConfVmping(tConfInfo) ) // �ڻ���ϳ�
    {
        wError = ERR_MCU_MIXNOSUPPORTONVMP;
		if ( bConfBrLE4M )
		{
			switch( byMediaTypeComb )
			{
			case 0x02://H264-MP3
				if( bConfBrLE1024K )
				{
					switch(byVmpStyle) {
					case VMP_STYLE_ONE:
					case VMP_STYLE_VTWO:
					case VMP_STYLE_HTWO:
					case VMP_STYLE_THREE:
					case VMP_STYLE_FOUR:
					case VMP_STYLE_NINE:
						byMaxNum =  m_tFilter.byMaxNumMixMultiMp3;
						break;
					default:
						byMaxNum = 0;
						break;
					}
				}
				else
				{
					switch(byVmpStyle) {
					case VMP_STYLE_ONE:
					case VMP_STYLE_VTWO:
					case VMP_STYLE_HTWO:
					case VMP_STYLE_THREE:
					case VMP_STYLE_FOUR:
						byMaxNum =  bConfBrLE1536K ? m_tFilter.byMaxNumMixMultiMp3 : m_tFilter.byMaxNumMixMultiMp3/*4,��ʱ�ŵ�8*/;
						break;
					default:
						byMaxNum = 0;
						break;
					}
				}
				break;
			case 0x03://H264-G729
				if( bConfBrLE768K )
				{
					switch(byVmpStyle) {
						case VMP_STYLE_ONE:
						case VMP_STYLE_VTWO:
						case VMP_STYLE_HTWO:
						case VMP_STYLE_THREE:
						case VMP_STYLE_FOUR:
						case VMP_STYLE_NINE:
							byMaxNum = m_tFilter.byMaxNumMixMultiG729;
							break;
						default:
							byMaxNum = 0;
							break;
					}				
				}
				else
				{
					byMaxNum = 0;
				}
				break;
			default:
				if ( bConfBrLE2M )
				{
					switch( byMediaTypeComb )
					{
					case 0x00://MPEG4-MP3
						switch(byVmpStyle) {
						case VMP_STYLE_ONE:
						case VMP_STYLE_VTWO:
						case VMP_STYLE_HTWO:
						case VMP_STYLE_THREE:
						case VMP_STYLE_FOUR:
							byMaxNum =  bConfBrLE1024K ? m_tFilter.byMaxNumMixMultiMp3 : m_tFilter.byMaxNumMixMultiMp3/*4,��ʱ�ŵ�8*/;
							break;
						default:
							byMaxNum = 0;
							break;
						}				
						break;
					case 0x01://MPEG4-G729
						switch(byVmpStyle) {
						case VMP_STYLE_ONE:
						case VMP_STYLE_VTWO:
						case VMP_STYLE_HTWO:
						case VMP_STYLE_THREE:
						case VMP_STYLE_FOUR:
							byMaxNum =  bConfBrLE768K ? m_tFilter.byMaxNumMixMultiG729 : 0;
							break;
						default:
							byMaxNum = 0;
							break;
						}	
						break;
					default:
						byMaxNum = 0;
						break;
					}
				}
				else
				{
					byMaxNum = 0;
				}
				break;
			}
		}
		else
		{
			byMaxNum = 0;
		}

//		if( bConfBrLE2M )
//		{
//			switch( byMediaTypeComb ) {
//			case 0x00://MPEG4-MP3
//				switch(byVmpStyle) {
//				case VMP_STYLE_ONE:
//				case VMP_STYLE_VTWO:
//				case VMP_STYLE_HTWO:
//				case VMP_STYLE_THREE:
//				case VMP_STYLE_FOUR:
//					byMaxNum =  bConfBrLE1024K ? m_tFilter.byMaxNumMixMultiMp3 : m_tFilter.byMaxNumMixMultiMp3/*4,��ʱ�ŵ�8*/;
//					break;
//				default:
//					byMaxNum = 0;
//					break;
//				}				
//				break;
//			case 0x01://MPEG4-G729
//				switch(byVmpStyle) {
//				case VMP_STYLE_ONE:
//				case VMP_STYLE_VTWO:
//				case VMP_STYLE_HTWO:
//				case VMP_STYLE_THREE:
//				case VMP_STYLE_FOUR:
//					byMaxNum =  bConfBrLE768K ? m_tFilter.byMaxNumMixMultiG729 : 0;
//					break;
//				default:
//					byMaxNum = 0;
//					break;
//				}	
//				break;
//			case 0x02://H264-MP3
//				if( bConfBrLE1024K )
//				{
//					switch(byVmpStyle) {
//					case VMP_STYLE_ONE:
//					case VMP_STYLE_VTWO:
//					case VMP_STYLE_HTWO:
//					case VMP_STYLE_THREE:
//					case VMP_STYLE_FOUR:
//					case VMP_STYLE_NINE:
//						byMaxNum =  m_tFilter.byMaxNumMixMultiMp3;
//						break;
//					default:
//						byMaxNum = 0;
//						break;
//					}
//				}
//				else
//				{
//					switch(byVmpStyle) {
//					case VMP_STYLE_ONE:
//					case VMP_STYLE_VTWO:
//					case VMP_STYLE_HTWO:
//					case VMP_STYLE_THREE:
//					case VMP_STYLE_FOUR:
//						byMaxNum =  bConfBrLE1536K ? m_tFilter.byMaxNumMixMultiMp3 : m_tFilter.byMaxNumMixMultiMp3/*4,��ʱ�ŵ�8*/;
//						break;
//					default:
//						byMaxNum = 0;
//						break;
//					}
//				}
//				break;
//			case 0x03://H264-G729
//				if( bConfBrLE768K )
//				{
//					switch(byVmpStyle) {
//						case VMP_STYLE_ONE:
//						case VMP_STYLE_VTWO:
//						case VMP_STYLE_HTWO:
//						case VMP_STYLE_THREE:
//						case VMP_STYLE_FOUR:
//						case VMP_STYLE_NINE:
//							byMaxNum = m_tFilter.byMaxNumMixMultiG729;
//							break;
//						default:
//							byMaxNum = 0;
//							break;
//					}				
//				}
//				else
//				{
//					byMaxNum = 0;
//				}
//				break;
//			default:
//				pfmlog("[GetMaxOprMixNum]MediaType is error!\n");
//				byMaxNum = 0;
//				break;
//			}
//		}
//		else
//		{
//			byMaxNum = 0;
//		}
    }
    else    // ����
    {
        wError = ERR_MCU_MIXNOSUPPORT;
        byMaxNum = ( bAudioTypeG729 ? m_tFilter.byMaxNumMixSingleG729 : m_tFilter.byMaxNumMixSingleMp3);
    }
    return byMaxNum;
#endif
}

/*=============================================================================
    �� �� ���� IsMixOprSupported
    ��    �ܣ� �жϵ�ǰ�Ķ��ƻ��������ܷ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TConfInfo &tConfInfo         [in]  ������Ϣ
               u8 byMixingMtNum             [in]  ���ڲ������(���ܻ���)���ն�����
               u8 byAddNum                  [in]  Ҫ��ӽ��������ն�����
               u16 &wError                  [out] ���صĴ�����
    �� �� ֵ�� BOOL32
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2007/01/25  4.0			�ű���                  ����
=============================================================================*/
BOOL32 CMcuPfmLmt::IsMixOprSupported( const TConfInfo &tConfInfo, u8 byMixingNum, u8 byAddNum, u16 &wError )
{
    wError = 0;
#if ( (!defined(_MINIMCU_)) && (!defined(MCS_PFMLMT)) )
    return TRUE;
#else

    if (!m_tFilter.IsEnable())
        return TRUE;

    if ( byAddNum == 0 )
    {
        return TRUE;
    }

	if( IsConfG729(tConfInfo) )
	{
		if( byMixingNum + byAddNum > m_tFilter.byMaxNumMixSingleG729 )
		{
			wError = ERR_MCU_MIXNOSUPPORT;
			return FALSE;
		}
	}
	else
	{
		if( byMixingNum + byAddNum > m_tFilter.byMaxNumMixSingleMp3 )
		{
			wError = ERR_MCU_MIXNOSUPPORT;
			return FALSE;
		}
	}

    u8 byMaxNum = GetMaxOprMixNum(tConfInfo, wError);
    if ( byMaxNum == 0 )
    {
		LogPrint(LOG_LVL_ERROR, MID_MCU_PFM, "[IsMixOprSupported] GetMaxOprMixNum return 0 !\n");
        return FALSE;
    }
    if ( byMaxNum < byMixingNum + byAddNum )
    {
		LogPrint(LOG_LVL_ERROR, MID_MCU_PFM, "[IsMixOprSupported] return failed due to max num.%d while CurNum.%d and AddNum.%d !\n",
			      byMaxNum, byMixingNum, byAddNum );

        return FALSE;
    }

    return TRUE;       
#endif
}

/*=============================================================================
    �� �� ���� IsVacOprSupported
    ��    �ܣ� �жϿ���VAC�Ƿ��ܱ�����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TConfInfo &tConfInfo   [in]  ������Ϣ
               u8        byMtNum      [in]  ���鵱ǰ�����������ն���
               u8        byMixNum     [in]  ���ڻ���(���ܡ�����)�ն���
               u16 &wError            [out] ���صĴ�����
    �� �� ֵ�� BOOL32
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2007/01/30  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuPfmLmt::IsVacOprSupported( const TConfInfo &tConfInfo, u8 byMtNum, u16 &wError )
{
    wError = 0;
#if ( (!defined(_MINIMCU_)) && (!defined(MCS_PFMLMT)) )
    return TRUE;
#else

    if (!m_tFilter.IsEnable())
        return TRUE;

    BOOL32 bConfVMP = IsConfVmping(tConfInfo);
    BOOL32 bConfBAS = IsConfBasing(tConfInfo);

    BOOL32 bConfAudioG729 = IsConfG729(tConfInfo);

    BOOL32 bAck = TRUE;
    u8 byRef = 0;
    // ��VAC
    if ( !bConfBAS & !bConfVMP )
    {
        wError = ERR_MCU_VACNOSUPPORT;
        byRef = bConfAudioG729 ? m_tFilter.byMaxNumVacSingleG729 : m_tFilter.byMaxNumVacSingleMp3 ;
        bAck = (byMtNum <= byRef);
    }
    else if ( bConfVMP ) // VMP + VAC
    {        
        wError = ERR_MCU_VMPNOVAC;
        bAck = FALSE;
    }
    else if ( bConfBAS )    // VAC + BAS
    {        
        wError = ERR_MCU_VACNOSUPPORTONBAS;
        BOOL32 bConfBRLE2M = IS_BITRATE_LE(tConfInfo, 2048);
        if ( bConfBRLE2M )
        {
            byRef = bConfAudioG729 ? m_tFilter.byMaxNumVacMultiG729 : m_tFilter.byMaxNumVacMultiMp3;
            bAck = (byMtNum <= byRef);
        }
        else 
        {
            bAck = FALSE;
        }        
    }
    
    if (!bAck)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_PFM, "[IsVacOprSupported] Return Nack(%d) because mtnum.%d is over max.%d at BR.%d\n", wError , byMtNum, byRef, tConfInfo.GetBitRate());
    }
    
    return bAck;
#endif

}

/*=============================================================================
    �� �� ���� IsBasOprSupported
    ��    �ܣ� �жϿ���BAS�Ƿ��ܱ�����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TConfInfo &tConfInfo   [in]  ������Ϣ
               u8        byMixNum     [in]  ���ڻ���(���ܡ�����)�ն���
               u16 &wError            [out] ���صĴ�����
    �� �� ֵ�� BOOL32
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2007/01/30  4.0			����                  ����
=============================================================================*/
BOOL32 CMcuPfmLmt::IsBasOprSupported( const TConfInfo &tConfInfo, u8 byMixNum, u16 &wError )
{
    wError = 0;
#if ( (!defined(_MINIMCU_)) && (!defined(MCS_PFMLMT)) )
    return TRUE;
#else

    if (!m_tFilter.IsEnable())
        return TRUE;

    BOOL32 bConfVAC = IsConfVacing(tConfInfo);
    BOOL32 bConfMIX = IsConfMixingNoVac(tConfInfo);
    
    BOOL32 bConfAudioG729 = IsConfG729(tConfInfo);
    BOOL32 bConfBRLE2M = IS_BITRATE_LE(tConfInfo, 2048);
    
    BOOL32 bAck = TRUE;
    u8 byRef = 0;
    
    // ��BAS
    if ( !bConfVAC & !bConfMIX )
    {
        return TRUE;
    }

    if ( bConfVAC ) // BAS + VAC
    {
        wError = ERR_MCU_BASNOSUPPORTONVAC;
        if ( bConfBRLE2M )
        {
            byRef = bConfAudioG729 ? m_tFilter.byMaxNumVacMultiG729 : m_tFilter.byMaxNumVacMultiMp3;
            bAck =  (byMixNum <= byRef);
        }
        else 
        {
            bAck = FALSE;
        }  
    }
    else if ( bConfMIX )    // BAS + MIX
    {                
        wError = ERR_MCU_BASNOSUPPORTONMIX;
        if ( bConfBRLE2M )
        {
            byRef = bConfAudioG729 ? m_tFilter.byMaxNumMixMultiG729 : m_tFilter.byMaxNumMixMultiMp3;
            bAck =  (byMixNum <= byRef);
        }
        else 
        {
            bAck = FALSE;
        }        
    }

    if (!bAck)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_PFM, "[IsBasOprSupported] Return Nack(%d) because byMixNum.%d is over max. %d at BR %d\n", wError , byMixNum, byRef, tConfInfo.GetBitRate());
    }

    return bAck;
#endif    
}

/*=============================================================================
    �� �� ���� IsMtOprSupported
    ��    �ܣ� �жϵ�ǰ�������ն��Ƿ��ܱ�����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TConfInfo &tConfInfo   [in]  ������Ϣ
               u8        byMtNum      [in]  ���鵱ǰ�����������ն���
               u8        byMixNum     [in]  ���ڻ���(���ܡ�����)�ն���
               u16 &wError            [out] ���صĴ�����
    �� �� ֵ�� BOOL32
    ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2007/01/26  4.0			�ű���                  ����
=============================================================================*/
BOOL32 CMcuPfmLmt::IsMtOprSupported( const TConfInfo &tConfInfo, u8 byMtNum, u8 byMixNum, u16 &wError )
{
    wError = 0;
#if ( (!defined(_MINIMCU_)) && (!defined(MCS_PFMLMT)) )
    return TRUE;
#else

    if (!m_tFilter.IsEnable())
        return TRUE;

    // ��ǰ�������ԵĹؼ�ֵ ��ʶ
    BOOL32 bConfVAC = IsConfVacing(tConfInfo);
    BOOL32 bConfVMP = IsConfVmping(tConfInfo);
    BOOL32 bConfMIX = IsConfMixingNoVac(tConfInfo);
    BOOL32 bConfBAS = IsConfBasing(tConfInfo);

    BOOL32 bConfAudioG729 = IsConfG729(tConfInfo);
    BOOL32 bConfVideoH264 = IsConfH264(tConfInfo);

    BOOL32 bConfBRLE768K   = IS_BITRATE_LE(tConfInfo, 768);
    BOOL32 bConfBRLE1M     = IS_BITRATE_LE(tConfInfo, 1024);
    BOOL32 bConfBRLE2M     = IS_BITRATE_LE(tConfInfo, 2048);

    // [1] nothing
    if ( !bConfVMP && !bConfMIX && !bConfBAS && !bConfVAC )
    {
        return TRUE;
    }
    
	// zgc, 2008-04-24, BUG10556, ��ʼ���ۻ���ʱ��ض�ε�����ʾ�ӻ��������л������ƻ�������ʾ
	// ���´���ͳһ�޸�: (1)����ʹ�� byMixNum ����������ʹ�� byMtNum; (2)'>'��'>='�滻

    // [2] Audio only
    if ( !bConfVMP && !bConfBAS ) 
    {
        if ( bConfMIX )
        {
            u8 byMax = bConfAudioG729 ? m_tFilter.byMaxNumMixSingleG729 : m_tFilter.byMaxNumMixSingleMp3;
            //�ն������Ѿ�����
            if ( byMixNum >= byMax ) 
            {
                wError = ERR_MCU_MIXNOSUPPORT;
                LogPrint(LOG_LVL_ERROR, MID_MCU_PFM,  "[IsMtOprSupported] cur mt Num.%d is over the load.%d MIX!\n",
                        byMtNum, byMax );
                return FALSE;
            }
        }
        else if ( bConfVAC ) 
        {
            u8 byMax = bConfAudioG729 ? m_tFilter.byMaxNumVacSingleG729 : m_tFilter.byMaxNumVacSingleMp3;
            //�ն������Ѿ�����
            if ( byMtNum >= byMax ) 
            {
                wError = ERR_MCU_VACNOSUPPORT;
                LogPrint(LOG_LVL_ERROR, MID_MCU_PFM,  "[IsMtOprSupported] cur mt Num.%d is over the load.%d VAC!\n",
                    byMtNum, byMax );
                return FALSE;
            }
        }
        return TRUE;
    }

    // [3] Video and Audio    
    // [3.1] VMP + Mix  
    BOOL32 bOverload = FALSE;
    if ( bConfVMP ) 
    {
        // VMP + VAC ��ͬʱ֧�֣�������
        if ( !bConfMIX )
        {
            return TRUE;
        }
        
        bOverload =  ( bConfAudioG729 ? (byMixNum /*>*/>= m_tFilter.byMaxNumMixMultiG729) : (byMixNum /*>*/>= m_tFilter.byMaxNumMixMultiMp3) );
        if ( bOverload ) 
        {
            wError = ERR_MCU_MIXNOSUPPORTONVMP;
            LogPrint(LOG_LVL_ERROR, MID_MCU_PFM,  "[IsMtOprSupported] cur mt Num.%d is over the load.%d VMP+MIX!\n", byMtNum );
            return FALSE;
        }

        return TRUE;
    }

    if ( bConfBAS )
    {
        // ֻ�� BAS
        if ( !bConfMIX && !bConfVAC )
        {
            return TRUE;
        }

        // [3.2] BAS + MIX
        if (bConfMIX)
        {            
            if ( bConfBRLE2M )
            {
                bOverload =  ( bConfAudioG729 ? (byMixNum >= m_tFilter.byMaxNumMixMultiG729) : (byMixNum >= m_tFilter.byMaxNumMixMultiMp3) );
            }
            else 
            {
                bOverload = TRUE;
            }
            if ( bOverload ) 
            {
                wError = ERR_MCU_MIXNOSUPPORTONBAS;
                LogPrint(LOG_LVL_ERROR, MID_MCU_PFM,  "[IsMtOprSupported] cur mt Num.%d is over the load.%d BAS+MIX!\n", byMtNum);
                return FALSE;
            }
            return TRUE;
        }
        // [3.3] BAS + VAC
        else
        {
            bOverload = FALSE;
            if ( bConfBRLE2M )
            {
                bOverload =  ( bConfAudioG729 ? (byMtNum >= m_tFilter.byMaxNumVacMultiG729) : (byMtNum >= m_tFilter.byMaxNumVacMultiMp3) );
            }
            else 
            {
                bOverload = TRUE;
            }
            if ( bOverload ) 
            {
                wError = ERR_MCU_VACNOSUPPORTONBAS;
                LogPrint(LOG_LVL_ERROR, MID_MCU_PFM,  "[IsMtOprSupported] cur mt Num.%d is over the load.%d BAS+VAC!\n", byMtNum);
                return FALSE;
            }
            return TRUE;
        }
    }

    return TRUE;
#endif 
}

/* MCU 8000A �������ƴ��룬���ܹ�����Լ�� */

/*=============================================================================
�� �� ���� IsVmpStyleSupport
��    �ܣ� �ж�ĳ������ϳɣ������໭�����ǽ��������Ŀ�����Ƿ���֧��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const TConfInfo &tConfInfo   [in] ������Ϣ
		   u8 byConfIdx                 [in] ����ConfIdx��Ϣ
		   TPeriEqpStatus *ptVmpStatus  [in] ����״̬����
		   u16 wLen                     [in] ����״̬���鳤��
		   u8 byTargetStyle             [in] ָ����Ŀ����
		   u8 &bySelVmpId               [out] ���ط���������Vmp��VmpTw
�� �� ֵ�� static BOOL32 �Ƿ���֧��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/13   4.0			�ܹ��      ����      ����
=============================================================================*/
BOOL32 CMcuPfmLmt::IsVmpStyleSupport( const TConfInfo &tConfInfo, u8 byConfIdx, 
                                      TPeriEqpStatus *ptVmpStatus, u16 wLen, 
								      u8 byTargetStyle, u8 &bySelVmpId )
{
    // FIXME: 
    // ��������1������û�ѡ����һ��Ҫ��ܵ͵�VMP��񣬵���ȴ�����һ�������ܸߵ�VMP
    // ��������2��Mcu�������� IsMcuSupport, IsVmpSupport������MCSû�����֣�
    //            ����ChangeʱMCS���һЩ����Vmp֧�ֵ������г�������MCU�ܾ�

	u8 byVmpCapChnnlNum = 0;
	bySelVmpId = 0;
	if( !GetMaxCapVMP( tConfInfo, byConfIdx,
                       ptVmpStatus, wLen, 
                       byVmpCapChnnlNum, 
                       bySelVmpId,
					   byTargetStyle) )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_PFM,  "[IsVmpStyleSupport] The GetMaxCapVmp() return no vmp avaiable or suitable!\n" );
		return FALSE;
	}
/*
	u8 byLop = 0;
	u8 byVmpMaxChnnlNum = 0;
	u8 byEqpType = 0;		//ȡ����TPeriEqpStatus�е�EqpType��status notif��Ϣ������
							//vmp,mpu-svmp,mpu-dvmp,evpu����EQP_TYPE_VMP
	u8 byVmpSubtype = 0;	//VMP ������
	for ( byLop = 0; byLop < wLen; byLop++ )
	{
		if ( ptVmpStatus[byLop].GetEqpId() == bySelVmpId )
		{
			byVmpMaxChnnlNum = ptVmpStatus[byLop].m_tStatus.tVmp.m_byChlNum;
			byEqpType = ptVmpStatus[byLop].GetEqpType();
			byVmpSubtype = ptVmpStatus[byLop].m_tStatus.tVmp.m_bySubType;
			break;
		}
	}

	u8 abyStyleArray[32];
    u8 bySize = 32;
	GetSupportedVmpStyles( tConfInfo, byEqpType, byVmpSubtype, byVmpCapChnnlNum, byVmpMaxChnnlNum, 
                           abyStyleArray, bySize );
	for ( byLop = 0; byLop < bySize; byLop++ )
	{
		if ( abyStyleArray[byLop] == byTargetStyle )
		{
			return TRUE;
		}
	}
		
	return FALSE;
	*/
	return TRUE;
}

/*=============================================================================
�� �� ���� GetSupportedVmpStylesFor8KE
��    �ܣ� ����ָ�������ܹ�֧�ֵ����л���ϳɷ�����8000E,����mcs���ã�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����   const TConfInfo &tConfInfo   [in] ������Ϣ
             u8 byConfIdx                 [in] ����ConfIdx��Ϣ
             TPeriEqpStatus *ptVmpStatus  [in] ����״̬����
             u16 wLen                     [in] ����״̬���鳤��
             u8 *pabStyle                 [out] ���صķ������
             u8 &byStyleSize              [I/O] ��������Ĵ�С��ͬʱ����ʵ�ʴ�С��
                                                ���ʵ�ʷ���ֵ���ڴ���ֵ����˵���ռ䲻����ͬʱ���������û�н������
�� �� ֵ�� void
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2010/03/01   4.6		Ѧ��		          ����
=============================================================================*/
void CMcuPfmLmt::GetSupportedVmpStylesFor8KE( const TConfInfo &tConfInfo, u8 byConfIdx, 
												TPeriEqpStatus *ptVmpStatus, u16 wLen, 
												u8 *pabyStyle, u8 &byArraySize )
{

	//���ر�˵���� �ú���Ŀǰ����mcs��

	u8 byWholeSelNum = 0;
	u8 byLop = 0;
	if (0 == wLen)
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_PFM,  "[GetSupportedVmpStyles] The wLen is 0, return!\n" );
		byArraySize = 0;
		return;
	}
	else if (1 == wLen)
	{
		// ����ָ������ʱ��ֱ���д������Ƿ���ʣ���������ݴ�������֧�ֵ����з��
		BOOL32 bIsOK = TRUE;
		u8 byIdx = 0;
		if (ptVmpStatus[byIdx].IsNull() || !ISTRUE(ptVmpStatus[byIdx].m_byOnline))
		{
			bIsOK = FALSE;
		}
		// ��ʹ�õ�VMP�Ǳ�������ռ�ã�������
		if (ISTRUE(ptVmpStatus[byIdx].m_tStatus.tVmp.m_byUseState) && 
			byConfIdx != ptVmpStatus[byIdx].GetConfIdx())
		{
			bIsOK = FALSE;
		}
		if (bIsOK)
		{
			byLop = 0;
			byWholeSelNum = 1;
		}
		else
		{
			// �����費����
			LogPrint(LOG_LVL_WARNING, MID_MCU_PFM,  "[GetSupportedVmpStyles] No vmp avaiable!\n" );
			byArraySize = 0;
			return;
		}
	}
	else
	{
		BOOL32 byIsVMPNone = FALSE;
		if ( CONF_VMPMODE_NONE == tConfInfo.m_tStatus.GetVMPMode() )
		{
			byIsVMPNone = TRUE;
		}
  
		// 8kg/8kh��֧��һ��vmp���裬�ҵ���break
		if ( byIsVMPNone )
		{
			for (; byLop < wLen; byLop++ )
			{
				if ( ptVmpStatus[byLop].IsNull() )
				{
					continue;
				}
				if ( !ISTRUE(ptVmpStatus[byLop].m_byOnline) )
				{
					continue;
				}
				if ( ISTRUE(ptVmpStatus[byLop].m_tStatus.tVmp.m_byUseState) )
				{
					// �ѱ�ʹ�õ�VMP����������δ����VMP�ϳɵĻ���
					continue;
				}
				byWholeSelNum ++;
				break;
			}
		}
		else
		{
			for (; byLop < wLen; byLop++ )
			{
				if ( ptVmpStatus[byLop].IsNull() )
				{
					continue;
				}
				if ( !ISTRUE(ptVmpStatus[byLop].m_byOnline) )
				{
					continue;
				}
				if ( ISTRUE(ptVmpStatus[byLop].m_tStatus.tVmp.m_byUseState) && 
					byConfIdx != ptVmpStatus[byLop].GetConfIdx() )
				{
					// ��ʹ�õ�VMP�Ǳ�������ռ�ã�������
					continue;
				}
				if ( !ISTRUE(ptVmpStatus[byLop].m_tStatus.tVmp.m_byUseState) )
				{
					LogPrint(LOG_LVL_DETAIL, MID_MCU_PFM, "[GetSupportedVmpStyles] encounter unexpected VMP UseState!\n");
				}
				else
				{
					// һ������ֻ�ܿ�һ��VMP
					// ��������Ѿ��ڿ�VMP����ֻ���жϸ�VMP��������Ŀǰ��֧��VMP֮����л�
					//nUsedIdx = byLop;
					byWholeSelNum = 1;
					break;
				}
			}
		}
	}
	
	if( byWholeSelNum == 0)
	{
		byArraySize = 0;
		return;
	}
	u8 byVmpSubtype = ptVmpStatus[byLop].m_tStatus.tVmp.m_bySubType;
	u8 byVmpMaxChnnlNum = ptVmpStatus[byLop].m_tStatus.tVmp.m_byChlNum;

	//  [1/6/2010 pengjie] Modify ���鿪�����󣬸��ݻ�����ѡVMP�����Լ��������Եó����Ļ���ϳɷ��
	u8 byVmpCapChnnlNum = MAXNUM_SDVMP_MEMBER;
	u8 byMaxChnlNumByConf = GetMaxCapVMPByConfInfo(tConfInfo);
	if( byMaxChnlNumByConf < byVmpCapChnnlNum )
	{
		byVmpCapChnnlNum = byMaxChnlNumByConf;
	}
	// End Modify

	GetSupportedVmpStyles( tConfInfo, EQP_TYPE_VMP, byVmpSubtype, byVmpCapChnnlNum, byVmpMaxChnnlNum, pabyStyle, byArraySize );

}

/*=============================================================================
�� �� ���� GetSupportedVmpStyles
��    �ܣ� ����ָ�������ܹ�֧�ֵ����л���ϳɷ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����   const TConfInfo &tConfInfo   [in] ������Ϣ
             u8 byConfIdx                 [in] ����ConfIdx��Ϣ
             TPeriEqpStatus *ptVmpStatus  [in] ����״̬����
             u16 wLen                     [in] ����״̬���鳤��
             u8 *pabStyle                 [out] ���صķ������
             u8 &byStyleSize              [I/O] ��������Ĵ�С��ͬʱ����ʵ�ʴ�С��
                                                ���ʵ�ʷ���ֵ���ڴ���ֵ����˵���ռ䲻����ͬʱ���������û�н������
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/13   4.0			�ܹ��      ����      ����
=============================================================================*/
void CMcuPfmLmt::GetSupportedVmpStyles( const TConfInfo &tConfInfo, u8 byConfIdx, 
                                        TPeriEqpStatus *ptVmpStatus, u16 wLen, 
								        u8 *pabyStyle, u8 &byArraySize )
{
	
	// [3/1/2010 xliang] �ú����ᱻmcsֱ�ӵ��������޷��ñ����������֡����Զ���8000E��mcs�����һ�ӿڡ�
	u8 byVmpCapChnnlNum = 0;
	u8 bySelVmpId = 0;
	if (0 == wLen)
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_PFM,  "[GetSupportedVmpStyles] The wLen is 0, return!\n" );
		byArraySize = 0;
		return;
	}
	else if (1 == wLen)
	{
		// ����ָ������ʱ��ֱ���д������Ƿ���ʣ���������ݴ�������֧�ֵ����з��
		BOOL32 bIsOK = TRUE;
		u8 byIdx = 0;
		if (ptVmpStatus[byIdx].IsNull() || !ISTRUE(ptVmpStatus[byIdx].m_byOnline))
		{
			bIsOK = FALSE;
		}
		// ��ʹ�õ�VMP�Ǳ�������ռ�ã�������
		if (ISTRUE(ptVmpStatus[byIdx].m_tStatus.tVmp.m_byUseState) && 
			byConfIdx != ptVmpStatus[byIdx].GetConfIdx())
		{
			bIsOK = FALSE;
		}
		if (bIsOK)
		{
			bySelVmpId = ptVmpStatus[byIdx].GetEqpId();
			byVmpCapChnnlNum = ptVmpStatus[byIdx].m_tStatus.tVmp.m_byChlNum;
		}
		else
		{
			// �����費����
			LogPrint(LOG_LVL_WARNING, MID_MCU_PFM,  "[GetSupportedVmpStyles] No vmp avaiable!\n" );
			byArraySize = 0;
			return;
		}
	}
	else
	{
		// ����������״̬ʱ�����б�����������ǿ��
		if( !GetMaxCapVMP( tConfInfo, byConfIdx, 
			ptVmpStatus, wLen, 
			byVmpCapChnnlNum, 
			bySelVmpId ) )
		{
			// û���ҵ����õ�Vmp/VmpTw������0
			LogPrint(LOG_LVL_WARNING, MID_MCU_PFM,  "[GetSupportedVmpStyles] The GetMaxCapVmp() return no vmp avaiable!\n" );
			byArraySize = 0;
			return;
		}
	}
    
	u8 byLop = 0;
	u8 byVmpMaxChnnlNum = 0;
	u8 byEqpType = 0;
	u8 byVmpSubType = 0;
	for ( byLop = 0; byLop < wLen; byLop++ )
	{
		if ( ptVmpStatus[byLop].GetEqpId() == bySelVmpId )
		{
			byVmpMaxChnnlNum = ptVmpStatus[byLop].m_tStatus.tVmp.m_byChlNum;
			byEqpType = ptVmpStatus[byLop].GetEqpType();
			byVmpSubType = ptVmpStatus[byLop].m_tStatus.tVmp.m_bySubType;
			break;
		}
	}

	//  [1/6/2010 pengjie] Modify ���鿪�����󣬸��ݻ�����ѡVMP�����Լ��������Եó����Ļ���ϳɷ��
	u8 byMaxChnlNumByConf = GetMaxCapVMPByConfInfo(tConfInfo);
	if( byMaxChnlNumByConf < byVmpCapChnnlNum )
	{
		byVmpCapChnnlNum = byMaxChnlNumByConf;
	}
	// End Modify

	GetSupportedVmpStyles( tConfInfo, byEqpType, byVmpSubType, byVmpCapChnnlNum, byVmpMaxChnnlNum, pabyStyle, byArraySize );

}

/*=============================================================================
�� �� ���� GetMaxCapVMPByConfInfo
��    �ܣ� ���ݻ����ʽ�������ʻ�ȡ��ǰ����vmp���֧�ֵ�ͨ������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� u8  ͨ����
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/3/31  4.0			������                  ����
=============================================================================*/
u8 CMcuPfmLmt::GetMaxCapVMPByConfInfo(const TConfInfo &tConfInfo)  
{
    u8 byCapChnnlNum = 0;   
    
    u16 wBitrate   = tConfInfo.GetBitRate();
    u8 byVidFormat = tConfInfo.GetMainVideoFormat();
	u8 byVidType   = tConfInfo.GetMainVideoMediaType();

    // ֻҪ��VmpAutoAdapt,��֧��25·����VmpAutoAdapt���������
    if ( tConfInfo.IsVmpAutoAdapt() )
    {   
	    byCapChnnlNum = MAXNUM_VMP_MEMBER/*25*/;
    }    
    else
    {
        if ( MEDIA_TYPE_H264 == byVidType )
        {
            // guzh [2008/08/06]
            if ( VIDEO_FORMAT_CIF == byVidFormat && wBitrate <= 4096)
            {
                byCapChnnlNum = 4;
            }
            else
            {
                byCapChnnlNum = 0;
            }
        }
        else if ( byVidType   == MEDIA_TYPE_H262 && // MPEG2,2CIF ��������
                  byVidFormat == VIDEO_FORMAT_2CIF )
        {
            if ( wBitrate <= 3072 ) // <=2048 && 2048-3072
            {
                byCapChnnlNum = 6;
            }
            else if ( wBitrate <= 8128 )    // 3072 -- 8128
            {
                byCapChnnlNum = 4;
            }
            else
            {
                byCapChnnlNum = 0;
            }                
        }
        // �����շֱ��ʴ���
        else
        {
            switch ( byVidFormat )
            {
            case VIDEO_FORMAT_CIF:     //  CIF
                if ( wBitrate <= 1024 )
                {
                    byCapChnnlNum = 16;
                }
                else if ( wBitrate <= 1536 )
                {
                    byCapChnnlNum = 13;
                }
                else if ( wBitrate <= 2048 )
                {
                    byCapChnnlNum = 10;
                }
                else if ( wBitrate <= 3072 )
                {
                    byCapChnnlNum = 7;
                }
                else if ( wBitrate <= 4096 )
                {
                    byCapChnnlNum = 6;
                }
                else if ( wBitrate <= 8128 )
                {
                    byCapChnnlNum = 4;
                }
                else
                {
                    byCapChnnlNum = 0;
                }
                break;
            case VIDEO_FORMAT_4CIF:     //  4CIF
                if (wBitrate > 8128)
                {
                    byCapChnnlNum = 0;
                }
                else
                {
                    byCapChnnlNum = 4;
                }            
                break;
            default:                   //  ���ఴauto����
                if ( wBitrate <= 1024 )
                {
                    byCapChnnlNum = 16;
                }
                else if ( wBitrate <= 1536 )
                {
                    byCapChnnlNum = 13;
                }
                else if ( wBitrate <= 2048 )
                {
                    byCapChnnlNum = 8;
                }
                else if ( wBitrate <= 3072 )
                {
                    byCapChnnlNum = 6;
                }
                else if ( wBitrate <= 8128 )
                {
                    byCapChnnlNum = 4;
                }
                else
                {
                    byCapChnnlNum = 0;
                }
                break;
            }
        }
    }

	//  [1/5/2010 pengjie] Modify AES���ܻ��飬VMP����ϳ���������������ʹ�ϵ��Ӳ������ԭ��
	u16 wConfBitRate = tConfInfo.GetBitRate();	
	TConfInfo tConfInfoTemp = tConfInfo;

	if( ( IsConfFormatHD(tConfInfoTemp) || (tConfInfo.GetMainVideoFormat() == VIDEO_FORMAT_4CIF
		&& tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_H264))
		&& tConfInfoTemp.GetMediaKey().GetEncryptMode() == CONF_ENCRYPTMODE_AES )
	{
		if( wConfBitRate > 3584 )
		{
			if( byCapChnnlNum > 6 )
			{
				byCapChnnlNum = 6;
			}
		}
		else if( wConfBitRate > 2560 && wConfBitRate <= 3584 )
		{
			if( byCapChnnlNum > 8 )
			{
				byCapChnnlNum = 8;
			}
		}
		else if( wConfBitRate > 2048 && wConfBitRate <= 2560 )
		{
			if( byCapChnnlNum > 9 )
			{
				byCapChnnlNum = 9;
			}	
		}
		else if( wConfBitRate > 1536 && wConfBitRate <= 2048 )
		{
			if( byCapChnnlNum > 13 )
			{
				byCapChnnlNum = 13;
			}
		}
		else if( wConfBitRate > 1024 && wConfBitRate <= 1536 )
		{
			if( byCapChnnlNum > 16 )
			{
				byCapChnnlNum = 16;
			}
		}
		else
		{
			// NONE;
		}
	} 
    
    LogPrint(LOG_LVL_DETAIL, MID_MCU_PFM, "[GetMaxCapVMPByConfInfo] CapChnnlNum=%d, IsAutoAdapt:%d, Video Type:%d, Format:%d, Bitrate:%d\n",
            byCapChnnlNum, tConfInfo.IsVmpAutoAdapt(), byVidType, byVidFormat, wBitrate);
    
    return byCapChnnlNum;
}

/*=============================================================================
	�� �� ���� GetMaxCapVMP
	��    �ܣ� ���㵱ǰMCU�Ŀ���VMP�����֧�ֵĻ���ϳ�·��������Լ���Ӧ���豸ID			   
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
	��    ���� const TConfInfo &tConfInfo :
			   u8 byConfIdx :
			   TPeriEqpStatus *ptStatus : VMP״̬����
			   u16 wlen : VMP״̬���鳤��
			   u8 &byVmpCapStyle [OUT] : ���ĺϳɷ��
			   u8 &byVmpCapChnnlNum [OUT] : ���ĺϳ�ͨ����
			   u8 &bySelVmpId[OUT] : ���п��м���������ʹ�õ�VMP��������ǿ��VMP���豸ID
	�� �� ֵ�� BOOL32
		----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��		�汾		�޸���		�߶���    �޸�����
	2007/6/20   4.0			�ܹ��                  ����
	09/04/21    4.6         Ѧ��                  ��ͬ����VMPѡ���������
=============================================================================*/
BOOL32 CMcuPfmLmt::GetMaxCapVMP( const TConfInfo &tConfInfo, u8 byConfIdx, 
								 TPeriEqpStatus *ptStatus, u16 wLen, 
								 u8 &byVmpCapChnnlNum, u8 &bySelVmpId,
								 u8 byTargetStyle)
{
	/* �����ж� */
	byVmpCapChnnlNum = 0;
	bySelVmpId = 0;
	if( NULL == ptStatus )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_PFM,  "[GetMaxCapVMP] pointer is NULL !\n" );
		return FALSE;
	}
	if( 0 == wLen || wLen > MAXNUM_PERIEQP )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_PFM,  "[GetMaxCapVMP] vmp status array length is %d, error!\n", wLen );
		return FALSE;
	}

	// [12/26/2009 xliang] 
	// MCS���ӵ�GetMaxCapVMP��GetVmpChlNumByStyle���䲢���ɼ�����GetMaxChlNumByStyle���
	// ������ϳɳ�Ա��
	u8 bychnlNumForStyle = GetMaxChlNumByStyle(byTargetStyle);

	// ��������Ϣ�������֧�ֺϳ�����
    u8 byMaxChnlNumByConf = GetMaxCapVMPByConfInfo(tConfInfo);

	// ����֧������Ϊ0������FALSE, zgc, 2008-03-26
	if ( 0 == byMaxChnlNumByConf )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_PFM,  "[GetMaxCapVMP] Conf don't support vmpopr!\n" );
		return FALSE;
	}
    
    // ��������״̬������������VMP����ӽ�������Ҫ�ģ�
    // ��ӽ������������VMP���޷�������������򷵻����������
    // ���򷵻ظ�����������
	// ��4��vmp:MPU2_VMP_ENHACED, MPU2_VMP_BASIC, MPU_SVMP, VMP
	const u8 byVmpTypeNum = 10;
	u8 bySeizeTypeNum = 4;		//����ռ���͸���
	u8	abySelIdx[byVmpTypeNum][MAXNUM_PERIEQP] = {0};	//4��vmp��Ӧ��id
	u8	abySelNum[byVmpTypeNum] = {0};					//4��vmp�ֱ��Ӧ����Ŀ
	u16 byWholeSelNum = 0;					//vmp����Ŀ
	u8  byVmpSubType = 0;					//vmp������
	s32 nUsedIdx = -1;						//������ǰ�����Ѿ�����ĳ��Vmp ID��-1��û����
	u8 byLop = 0;

	// zgc, 2008-04-23, BUG10524�޸ļ���������
    // zw 20081014 ����VMPTW
    BOOL32 byIsVMPNone = FALSE;
    if ( EQP_TYPE_VMP == ptStatus->GetEqpType() )
    {
        //if ( CONF_VMPMODE_NONE == tConfInfo.m_tStatus.GetVMPMode() )֧�ֶ໭��ϳ�ʱ����Զ���µĿ���vmp
        {
            byIsVMPNone = TRUE;
        }
    }
    else // EQP_TYPE_VMPTW == (u8*)ptStatus
    {
        if ( CONF_VMPTWMODE_NONE == tConfInfo.m_tStatus.GetVmpTwMode() )
        {
            byIsVMPNone = TRUE;
        }
    }

	if ( byIsVMPNone )
	{
		for (byLop = 0; byLop < wLen; byLop++ )
		{
			if ( ptStatus[byLop].IsNull() )
			{
				continue;
			}
			if ( !ISTRUE(ptStatus[byLop].m_byOnline) )
			{
				continue;
			}
			if ( ISTRUE(ptStatus[byLop].m_tStatus.tVmp.m_byUseState) )
			{
				// �ѱ�ʹ�õ�VMP����������δ����VMP�ϳɵĻ���
				continue;
			}
			/*�������ʴ���4Mʱ����֧����ռVmp(Basic*2)�ĺϳ���
			if (IS_BITRATE_GR(tConfInfo, 4096) &&
				ptStatus[byLop].m_tStatus.tVmp.m_bySubType == MPU2_VMP_BASIC &&
				ptStatus[byLop].m_tStatus.tVmp.m_byBoardVer == MPU2_BOARD_ECARD)
			{
				continue;
			}*/
			// ���¿��ܱ�ѡ�е�VMP�����
			// xliang [12/8/2008] vmp����ͳ��.
			/*			0	//��vmp
						1	//MPU_SVMP
					    2	//MPU2_VMP_BASIC
						3	//MPU2_VMP_ENHACED */
			byVmpSubType = ptStatus[byLop].m_tStatus.tVmp.m_bySubType;
			abySelIdx[byVmpSubType][ abySelNum[byVmpSubType] ] = byLop;
			abySelNum[byVmpSubType]++;
			byWholeSelNum ++;
		}
	}
	else
	{
    	for (byLop = 0; byLop < wLen; byLop++ )
		{
			if ( ptStatus[byLop].IsNull() )
			{
				continue;
			}
			if ( !ISTRUE(ptStatus[byLop].m_byOnline) )
			{
				continue;
			}
			if ( ISTRUE(ptStatus[byLop].m_tStatus.tVmp.m_byUseState) && 
				byConfIdx != ptStatus[byLop].GetConfIdx() )
			{
				// ��ʹ�õ�VMP�Ǳ�������ռ�ã�������
				continue;
			}
			if ( !ISTRUE(ptStatus[byLop].m_tStatus.tVmp.m_byUseState) )
			{
				// xliang [12/8/2008] 
				byVmpSubType = ptStatus[byLop].m_tStatus.tVmp.m_bySubType;
				abySelIdx[byVmpSubType][abySelNum[byVmpSubType]] = byLop;
				abySelNum[byVmpSubType]++;
				byWholeSelNum++;
			}
			else
			{
				// һ������ֻ�ܿ�һ��VMP
				// ��������Ѿ��ڿ�VMP����ֻ���жϸ�VMP��������Ŀǰ��֧��VMP֮����л�
				nUsedIdx = byLop;
				// xliang [12/8/2008] 
 				byWholeSelNum = 1;
				break;
			}
		}
	}

	// xliang [12/8/2008] 
	if (0 == byWholeSelNum )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_PFM,  "[GetMaxCapVMP] No vmp can be used!\n" );
		return FALSE;
	}

	if(nUsedIdx >= 0) //������ǰ�����Ѿ�����ĳ��vmp��
	{
		byVmpCapChnnlNum = ptStatus[nUsedIdx].m_tStatus.tVmp.m_byChlNum;
		bySelVmpId = ptStatus[nUsedIdx].GetEqpId();
		return TRUE;
	}

	// xliang [12/8/2008] ����4��vmp��Ӧ���Ի����ʽ�����ȼ��ֶα���
	u8 byConfMediaType = tConfInfo.GetMainVideoMediaType();
	u8 byConfVidFormat = tConfInfo.GetMainVideoFormat();
	u8 abyVmpTypePri[byVmpTypeNum] = {0};

	// �ϳ�������ʱ��ռ
	//�Ⱥ�������VMP��vmp(Enhance) > vmp(Basic) > mpu > vmp
	u8 abyVmpPri[] = { MPU2_VMP_ENHACED, MPU2_VMP_BASIC, MPU_SVMP, VMP };
	memcpy(abyVmpTypePri,abyVmpPri,sizeof(abyVmpTypePri));

	u8 byLoop = 0;
 	//print All Selectable VMPs info
	LogPrint(LOG_LVL_DETAIL, MID_MCU_PFM, "[GetMaxCapVMP] All Selectable VMPs info:\n==================================\n");
	for(byLoop =0; byLoop<bySeizeTypeNum; byLoop++)
	{
		u8 bySelVmpType = abyVmpTypePri[byLoop];
		u8 bySelNum		= abySelNum[bySelVmpType];
		LogPrint(LOG_LVL_DETAIL, MID_MCU_PFM, "VMP type %u, Num is %u\n---------------\n",bySelVmpType,bySelNum);
		for(byLop = 0; byLop < bySelNum; byLop++)
		{
			u8 byIdx =  abySelIdx[bySelVmpType][byLop];
			LogPrint(LOG_LVL_DETAIL, MID_MCU_PFM,  "Vmp Id.%u\n",ptStatus[byIdx].GetEqpId() );
		}
	}
	
	//�ֶα���ѡȡ���ʵ�vmp
	u8 abySelVmpIdx[MAXNUM_PERIEQP];
	for(byLoop =0; byLoop < bySeizeTypeNum; byLoop++)
	{
		u8 bySelNum = abySelNum[abyVmpTypePri[byLoop]];
		if(bySelNum ==0)
		{
			continue;
		}
		//  [12/22/2009 pengjie] Modify ������飬����ʹ��VPU��VMP�������л���ϳ�
		if( MEDIA_TYPE_H264 == byConfMediaType && (VIDEO_FORMAT_HD720 == byConfVidFormat || VIDEO_FORMAT_HD1080 == byConfVidFormat) )
		{
			if(abyVmpTypePri[byLoop] == VMP)
			{
				continue;
			}
			// ����ʽ��ѡHPʱ,����ʹ��MPU
			if (emHpAttrb == tConfInfo.GetProfileAttrb() && abyVmpTypePri[byLoop] == MPU_SVMP)
			{
				continue;
			}
		}
		// End
		
		//���ڻ�������ʽ��mp2,h261,������mpu
		if( (MEDIA_TYPE_H262 == byConfMediaType || MEDIA_TYPE_H261 == byConfMediaType)
			&& abyVmpTypePri[byLoop] == MPU_SVMP)
		{
			continue;
		}

		//��vmp�������ڻ���ϳ�������ѯ
		if (tConfInfo.m_tStatus.m_tVMPParam.IsVMPBatchPoll() &&
			abyVmpTypePri[byLoop] == VMP)
		{
			continue;
		}
		
		memcpy(abySelVmpIdx,abySelIdx[abyVmpTypePri[byLoop]],sizeof(u8)*bySelNum);
	
		if(GetSelVmpCap(abySelVmpIdx, ptStatus, bySelNum,
			byMaxChnlNumByConf,byVmpCapChnnlNum, bySelVmpId))
		{
			// xliang [2/12/2009] ��ѡ����VMP ������С���û���ѡ�ķ����������ѡ����VMPҲ�ǲ����ʵ�
			if ( bychnlNumForStyle > byVmpCapChnnlNum)  
			{
				bySelVmpId = 0; //��û�к��ʵ�VMP����
			}
			else
			{
				break;
			}
		}
	}
	LogPrint(LOG_LVL_DETAIL, MID_MCU_PFM, "[GetMaxCapVMP] byVmpCapChnnlNum=%d, bychnlNumForStyle = %d, bySelVmpId=%d \n",
		byVmpCapChnnlNum, bychnlNumForStyle, bySelVmpId);

	if(bySelVmpId == 0)
	{
		return FALSE;
	}
	return TRUE;

}

/*====================================================================
    ������      : GetMaxChlNumByStyle
    ����        : �õ�����ϳɷ�������Ա��
    �㷨ʵ��    : 
    ����ȫ�ֱ���: ��
    �������˵��: byVMPStyle ����ϳɷ��
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/31    3.0         ������       ����
====================================================================*/
u8 CMcuPfmLmt::GetMaxChlNumByStyle( u8 byVMPStyle )
{
	u8   byMaxMemNum = 0;

	switch( byVMPStyle ) 
	{
	case VMP_STYLE_ONE:
		byMaxMemNum = 1;
		break;
	case VMP_STYLE_VTWO:
	case VMP_STYLE_HTWO:
		byMaxMemNum = 2;
		break;
	case VMP_STYLE_THREE:
		byMaxMemNum = 3;
		break;
	case VMP_STYLE_FOUR:
		byMaxMemNum = 4;
		break;
	case VMP_STYLE_SIX:
		byMaxMemNum = 6;
		break;
	case VMP_STYLE_EIGHT:
		byMaxMemNum = 8;
		break;
	case VMP_STYLE_NINE:
		byMaxMemNum = 9;
		break;
	case VMP_STYLE_TEN:
	case VMP_STYLE_TEN_M:
		byMaxMemNum = 10;
		break;
	case VMP_STYLE_THIRTEEN:
	case VMP_STYLE_THIRTEEN_M:
		byMaxMemNum = 13;
		break;
	case VMP_STYLE_FIFTEEN:
		byMaxMemNum = 15;
		break;
	case VMP_STYLE_SIXTEEN:
		byMaxMemNum = 16;
		break;
	case VMP_STYLE_SPECFOUR:
		 byMaxMemNum = 4;
		break;
	case VMP_STYLE_SEVEN:
		 byMaxMemNum = 7;
		 break;
	case VMP_STYLE_TWENTY:// xliang [2/19/2009] 
		 byMaxMemNum = 20;
		 break;
	case VMP_STYLE_TWENTYFIVE:
		byMaxMemNum = 25;
		break;
	default:
		break;
	}

	return byMaxMemNum;
}

/*=============================================================================
�� �� ���� GetSupportedVmpStyles
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const TConfInfo &tConfInfo
		   u8 byEqpType
		   u8 bySubType
		   u8 byCapChnnlNum             
           u8 byVmpMaxChnnlNum
           u8 *pabyStyle                [out] ���صķ������
           u8 &byArraySize              [I/O] ��������Ĵ�С��ͬʱ����ʵ�ʴ�С��
                                              ���ʵ�ʷ���ֵ���ڴ���ֵ����˵���ռ䲻����ͬʱ���������û�н������
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/13   4.0			�ܹ��                  ����
2009/2/18	4.6			Ѧ��					����������VMP���ࣻ����20���
=============================================================================*/
void CMcuPfmLmt::GetSupportedVmpStyles( const TConfInfo &tConfInfo, u8 byEqpType, u8 bySubType, u8 byCapChnnlNum, u8 byVmpMaxChnnlNum, 
                                        u8 *pabyStyle, u8 &byArraySize )
{
	if ( pabyStyle == NULL )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_PFM,  "[GetSupportedVmpStyles] style array point is NULL!\n");
        byArraySize = 0;
		return;
	}
    
    u8 abyStyles[MAXNUM_MCU_PERIEQP] = {VMP_STYLE_NONE};

    u8 byRetSize = 0;
    if ( byEqpType == EQP_TYPE_VMP )
    {
        abyStyles[byRetSize++] = VMP_STYLE_DYNAMIC;
    }

	// [3/1/2010 xliang]  VMP��8000E-VMP���16���MPU��EVPU���20���
	BOOL32 bEqpChnlFull = FALSE;
	// mpu2֧��25���
	if (bySubType == MPU2_VMP_BASIC || bySubType == MPU2_VMP_ENHACED)
	{
		bEqpChnlFull = (byVmpMaxChnnlNum == MAXNUM_MPU2VMP_MEMBER);
	}
	else if( bySubType == MPU_SVMP )
	{
		bEqpChnlFull = (byVmpMaxChnnlNum == MAXNUM_MPUSVMP_MEMBER);
	}
	else
	{	
		bEqpChnlFull = (byVmpMaxChnnlNum == MAXNUM_SDVMP_MEMBER);
	}	
	
    //BOOL32 bEqpChnlFull = (byVmpMaxChnnlNum == 16);
    byCapChnnlNum = min(byCapChnnlNum, byVmpMaxChnnlNum);
    
	switch( byCapChnnlNum )
	{
		case 1:
			if ( /*tConfInfo.GetMainVideoFormat() != MEDIA_TYPE_H264 ||*/ bEqpChnlFull )
			{
				abyStyles[byRetSize++] = VMP_STYLE_ONE; 
			}
			break;
		case 2:
			if ( /*tConfInfo.GetMainVideoFormat() != MEDIA_TYPE_H264 ||*/ bEqpChnlFull )
			{
				abyStyles[byRetSize++] = VMP_STYLE_ONE;
			}
            if (bEqpChnlFull)
            {
                abyStyles[byRetSize++] = VMP_STYLE_HTWO;
            }
            abyStyles[byRetSize++] = VMP_STYLE_VTWO;
			break;
		case 3:
            if ( /*tConfInfo.GetMainVideoFormat() != MEDIA_TYPE_H264 ||*/ bEqpChnlFull )
			{
				abyStyles[byRetSize++] = VMP_STYLE_ONE;
			}
            if (bEqpChnlFull)
            {
                abyStyles[byRetSize++] = VMP_STYLE_HTWO;
			}
            abyStyles[byRetSize++] = VMP_STYLE_VTWO;
            abyStyles[byRetSize++] = VMP_STYLE_THREE;
			break;
		case 4:
        case 5:
            if ( /*tConfInfo.GetMainVideoFormat() != MEDIA_TYPE_H264 ||*/ bEqpChnlFull )
			{
				abyStyles[byRetSize++] = VMP_STYLE_ONE;
			}
            if (bEqpChnlFull)
            {
                abyStyles[byRetSize++] = VMP_STYLE_HTWO;
            }
            abyStyles[byRetSize++] = VMP_STYLE_VTWO;
            abyStyles[byRetSize++] = VMP_STYLE_THREE;
            abyStyles[byRetSize++] = VMP_STYLE_SPECFOUR;	
            abyStyles[byRetSize++] = VMP_STYLE_FOUR;            		
			break;        
		case 6:
            if ( /*tConfInfo.GetMainVideoFormat() != MEDIA_TYPE_H264 ||*/ bEqpChnlFull )
			{
				abyStyles[byRetSize++] = VMP_STYLE_ONE;
			}
            if (bEqpChnlFull)
            {
                abyStyles[byRetSize++] = VMP_STYLE_HTWO;
            }
            abyStyles[byRetSize++] = VMP_STYLE_VTWO;
            abyStyles[byRetSize++] = VMP_STYLE_THREE;
            abyStyles[byRetSize++] = VMP_STYLE_SPECFOUR;
            abyStyles[byRetSize++] = VMP_STYLE_FOUR;            
            abyStyles[byRetSize++] = VMP_STYLE_SIX;
			break;
		case 7:
            if ( /*tConfInfo.GetMainVideoFormat() != MEDIA_TYPE_H264 ||*/ bEqpChnlFull )
			{
				abyStyles[byRetSize++] = VMP_STYLE_ONE;
			}
            if (bEqpChnlFull)
            {
                abyStyles[byRetSize++] = VMP_STYLE_HTWO;
            }
            abyStyles[byRetSize++] = VMP_STYLE_VTWO;
            abyStyles[byRetSize++] = VMP_STYLE_THREE;
            abyStyles[byRetSize++] = VMP_STYLE_SPECFOUR;
            abyStyles[byRetSize++] = VMP_STYLE_FOUR;            
            abyStyles[byRetSize++] = VMP_STYLE_SIX;
            if (bEqpChnlFull)
            {
                abyStyles[byRetSize++] = VMP_STYLE_SEVEN;
            }		
			break;
		case 8:
            if ( /*tConfInfo.GetMainVideoFormat() != MEDIA_TYPE_H264 ||*/ bEqpChnlFull )
			{
				abyStyles[byRetSize++] = VMP_STYLE_ONE;
			}
            if (bEqpChnlFull)
            {
                abyStyles[byRetSize++] = VMP_STYLE_HTWO;
            }
            abyStyles[byRetSize++] = VMP_STYLE_VTWO;
            abyStyles[byRetSize++] = VMP_STYLE_THREE;
            abyStyles[byRetSize++] = VMP_STYLE_SPECFOUR;
            abyStyles[byRetSize++] = VMP_STYLE_FOUR;  
            abyStyles[byRetSize++] = VMP_STYLE_SIX;
            if (bEqpChnlFull)
            {
                abyStyles[byRetSize++] = VMP_STYLE_SEVEN;
                abyStyles[byRetSize++] = VMP_STYLE_EIGHT;
            }	
			break;
		case 9:
            if ( /*tConfInfo.GetMainVideoFormat() != MEDIA_TYPE_H264 ||*/ bEqpChnlFull )
			{
				abyStyles[byRetSize++] = VMP_STYLE_ONE;
			}
            if (bEqpChnlFull)
            {
                abyStyles[byRetSize++] = VMP_STYLE_ONE;
            }
            abyStyles[byRetSize++] = VMP_STYLE_VTWO;
            abyStyles[byRetSize++] = VMP_STYLE_THREE;
            abyStyles[byRetSize++] = VMP_STYLE_SPECFOUR;
            abyStyles[byRetSize++] = VMP_STYLE_FOUR;  
            abyStyles[byRetSize++] = VMP_STYLE_SIX;
            if (bEqpChnlFull)
            {
                abyStyles[byRetSize++] = VMP_STYLE_SEVEN;
                abyStyles[byRetSize++] = VMP_STYLE_EIGHT;
            }
            abyStyles[byRetSize++] = VMP_STYLE_NINE;		
			break;
		case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
            if ( /*tConfInfo.GetMainVideoFormat() != MEDIA_TYPE_H264 ||*/ bEqpChnlFull )
			{
				abyStyles[byRetSize++] = VMP_STYLE_ONE;
			}
            abyStyles[byRetSize++] = VMP_STYLE_HTWO;
            abyStyles[byRetSize++] = VMP_STYLE_VTWO;            
            abyStyles[byRetSize++] = VMP_STYLE_THREE;
            abyStyles[byRetSize++] = VMP_STYLE_SPECFOUR;
            abyStyles[byRetSize++] = VMP_STYLE_FOUR;            
            abyStyles[byRetSize++] = VMP_STYLE_SIX;
            abyStyles[byRetSize++] = VMP_STYLE_SEVEN;
            abyStyles[byRetSize++] = VMP_STYLE_EIGHT;
            abyStyles[byRetSize++] = VMP_STYLE_NINE;	
            abyStyles[byRetSize++] = VMP_STYLE_TEN;
			// 8KH 8KI��MPU2֧���µ�ʮ���
			if (VMP_8KH == bySubType || 
				VMP_8KI == bySubType || 
				MPU2_VMP_BASIC == bySubType || 
				MPU2_VMP_ENHACED == bySubType)
			{
				abyStyles[byRetSize++] = VMP_STYLE_TEN_M;
			}
            if (byCapChnnlNum >= 13)
            {
                abyStyles[byRetSize++] = VMP_STYLE_THIRTEEN;
				// 8KH 8KI��MPU2֧���µ�ʮ�����
				if (VMP_8KH == bySubType || 
					VMP_8KI == bySubType || 
					MPU2_VMP_BASIC == bySubType || 
					MPU2_VMP_ENHACED == bySubType)
				{
					abyStyles[byRetSize++] = VMP_STYLE_THIRTEEN_M;
				}  
            }
            if (byCapChnnlNum >= 15 && 
				(VMP_8KH == bySubType || VMP_8KI == bySubType))// 8kh 8kivmp����֧��15���
            {
                abyStyles[byRetSize++] = VMP_STYLE_FIFTEEN;  
            }
            if (byCapChnnlNum >= 16)
            {
                abyStyles[byRetSize++] = VMP_STYLE_SIXTEEN;
            }
            break;
		case 20:
		case 25:
			abyStyles[byRetSize++] = VMP_STYLE_ONE;
			abyStyles[byRetSize++] = VMP_STYLE_HTWO;
            abyStyles[byRetSize++] = VMP_STYLE_VTWO;            
            abyStyles[byRetSize++] = VMP_STYLE_THREE;
            abyStyles[byRetSize++] = VMP_STYLE_SPECFOUR;
            abyStyles[byRetSize++] = VMP_STYLE_FOUR;            
            abyStyles[byRetSize++] = VMP_STYLE_SIX;
            abyStyles[byRetSize++] = VMP_STYLE_SEVEN;
            abyStyles[byRetSize++] = VMP_STYLE_EIGHT;
            abyStyles[byRetSize++] = VMP_STYLE_NINE;	
            abyStyles[byRetSize++] = VMP_STYLE_TEN;
			// MPU2֧���µ�ʮ���
			if (MPU2_VMP_BASIC == bySubType || MPU2_VMP_ENHACED == bySubType)
			{
				abyStyles[byRetSize++] = VMP_STYLE_TEN_M;
			}
			abyStyles[byRetSize++] = VMP_STYLE_THIRTEEN;
			// MPU2֧���µ�ʮ�����
			if (MPU2_VMP_BASIC == bySubType || MPU2_VMP_ENHACED == bySubType)
			{
				abyStyles[byRetSize++] = VMP_STYLE_THIRTEEN_M;
			}
			abyStyles[byRetSize++] = VMP_STYLE_SIXTEEN;
			abyStyles[byRetSize++] = VMP_STYLE_TWENTY;
			if (byCapChnnlNum >= 25)
			{
				// MPU2֧���µ�25���
				if (MPU2_VMP_BASIC == bySubType || MPU2_VMP_ENHACED == bySubType)
				{
					abyStyles[byRetSize++] = VMP_STYLE_TWENTYFIVE;
				}
			}
			break;
		default:
			break;
	}

    if (byArraySize >= byRetSize)
    {
        memcpy(pabyStyle, abyStyles, byRetSize);
    }
    byArraySize = byRetSize;
    return;

}

/*=============================================================================
�� �� ���� IsConfFormatHD
��    �ܣ� �������Ƿ�Ϊ�������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const TConfInfo &tConfInfo   [in] ������Ϣ
�� �� ֵ�� BOOL32
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/12/19  4.0			�ű���                  ����
=============================================================================*/
BOOL32 CMcuPfmLmt::IsConfFormatHD(const TConfInfo &tConfInfo)
{
    BOOL32 bFormatHD = FALSE;
    u8 byVideoType = tConfInfo.GetMainVideoMediaType();
    if (MEDIA_TYPE_H264 != byVideoType)
    {
        return bFormatHD;
    }
    
    u8 byConfFormat = tConfInfo.GetMainVideoFormat();
    switch( byConfFormat )
    {
    case VIDEO_FORMAT_W4CIF:
    case VIDEO_FORMAT_HD720:
    case VIDEO_FORMAT_SXGA:
    case VIDEO_FORMAT_UXGA:
    case VIDEO_FORMAT_HD1080:
        bFormatHD = TRUE;
        break;
    default:
        break;
    }
    return bFormatHD;
}

/*=============================================================================
�� �� ���� IsConfFormatOverCif
��    �ܣ� �������Ƿ�ΪD1���߸�����飬����H.264��Ч(��������Ƶ)
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const TConfInfo &tConfInfo   [in] ������Ϣ
�� �� ֵ�� BOOL32
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/03/11  4.5			����                  ����
=============================================================================*/
BOOL32 CMcuPfmLmt::IsConfFormatOverCif( const TConfInfo &tConfInfo )
{
    u8 byVideoType = tConfInfo.GetMainVideoMediaType();
    if (MEDIA_TYPE_H264 != byVideoType)
    {
        return FALSE;
    }

    u8 byConfFormat = tConfInfo.GetMainVideoFormat();
    if ( byConfFormat == VIDEO_FORMAT_4CIF ||
         IsConfFormatHD(tConfInfo) )
    {
         return TRUE;
    }
    return FALSE;
}

/*=============================================================================
�� �� ���� IsConfPermitVmp
��    �ܣ� �������Ƿ�������ϳ�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const TConfInfo &tConfInfo
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/4/2   4.0		�ܹ��                  ����
=============================================================================*/
BOOL32 CMcuPfmLmt::IsConfPermitVmp( const TConfInfo &tConfInfo )
{
	BOOL32 bSupport = TRUE;
    /*
	if ( IsConfFormatHD(tConfInfo) )
	{
		bSupport = FALSE;
	}
    */
	return bSupport;
}

/*=============================================================================
 �� �� ���� IsRollCallSupported
 ��    �ܣ� �������Ƿ�֧�ֻ������
 �㷨ʵ�֣� 
 ȫ�ֱ����� 
 ��    ���� const TConfInfo &tConfInfo   [in] ������Ϣ
 �� �� ֵ�� BOOL32
 ----------------------------------------------------------------------
 �޸ļ�¼    ��
 ��  ��		 �汾		�޸���		�߶���    �޸�����
 2007/12/19  4.0		�ű���                  ����
=============================================================================*/
BOOL32 CMcuPfmLmt::IsRollCallSupported( const TConfInfo &tConfInfo )
{
    BOOL32 bSupported = TRUE;
    /*
    if ( IsConfFormatHD(tConfInfo) )
    {
        bSupported = FALSE;
    }
    */
    return bSupported;
}

/*=============================================================================
 �� �� ���� IsMPWSupported
 ��    �ܣ� �������Ƿ�֧�ֶ໭�����ǽ
 �㷨ʵ�֣� 
 ȫ�ֱ����� 
 ��    ���� const TConfInfo &tConfInfo   [in] ������Ϣ
 �� �� ֵ�� BOOL32
 ----------------------------------------------------------------------
 �޸ļ�¼    ��
 ��  ��		 �汾		�޸���		�߶���    �޸�����
 2007/12/19  4.0		�ű���                  ����
=============================================================================*/
BOOL32 CMcuPfmLmt::IsMPWSupported( const TConfInfo &tConfInfo )
{
    BOOL32 bSupported = FALSE;
    /*
    if ( IsConfPermitVmp(tConfInfo) )
    {
        bSupported = TRUE;
    }
    */
    return bSupported;
}

/*=============================================================================
�� �� ���� IsVmpSupported
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const TConfInfo &tConfInfo
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/4/2   4.0		�ܹ��                  ����
=============================================================================*/
BOOL32 CMcuPfmLmt::IsVmpSupported( const TConfInfo &tConfInfo )
{
	BOOL32 bSupported = FALSE;
    if ( IsConfPermitVmp(tConfInfo) )
    {
        bSupported = TRUE;
    }
    return bSupported;
}

/*=============================================================================
 �� �� ���� IsTVWallSupported
 ��    �ܣ� �������Ƿ�֧�ֵ���ǽ
 �㷨ʵ�֣� 
 ȫ�ֱ����� 
 ��    ���� const TConfInfo &tConfInfo   [in] ������Ϣ
 �� �� ֵ�� BOOL32
 ----------------------------------------------------------------------
 �޸ļ�¼    ��
 ��  ��		 �汾		�޸���		�߶���    �޸�����
 2007/12/19  4.0		�ű���                  ����
=============================================================================*/
BOOL32 CMcuPfmLmt::IsTVWallSupported( const TConfInfo &tConfInfo )
{
    BOOL32 bSupported = TRUE;
    if ( IsConfFormatOverCif(tConfInfo) )
    {
        bSupported = FALSE;
    }
    return bSupported;
}

/*=============================================================================
 �� �� ���� IsMultiVidStreamSupported
 ��    �ܣ� �������Ƿ�֧����Ƶ���ٶ��ʽ����Ҫ�ж���·��Ƶ��ʽ
 �㷨ʵ�֣� 
 ȫ�ֱ����� 
 ��    ���� const TConfInfo &tConfInfo   [in] ������Ϣ
            u8 byMode = MODE_VIDEO       [in] ��Ƶ����Ƶ,��֧��MODE_BOTH
 �� �� ֵ�� BOOL32
 ----------------------------------------------------------------------
 �޸ļ�¼    ��
 ��  ��		�汾		�޸���		�߶���    �޸�����
 2007/12/19  4.0		�ű���                  ����
 2008/03/11  4.5        ����
=============================================================================*/
BOOL32 CMcuPfmLmt::IsMultiStreamSupported( const TConfInfo &tConfInfo, u8 byMode )
{
    BOOL32 bSupported = TRUE;
    
    /*
    if ( MODE_VIDEO != byMode && MODE_AUDIO != byMode )
    {
        OspPrintf( TRUE, FALSE, "[MultiStreamSup] unexpected mode.%d\n", byMode );
        return FALSE;
    }
    if ( byMode == MODE_VIDEO )
    {
        // ����ʽ��Ƶ
        if ( IsConfFormatOverCif(tConfInfo) )
        {
            return FALSE;
        }

        // ����ʽ��Ƶ
        u8 byVideoType = tConfInfo.GetSecVideoMediaType();
        if (MEDIA_TYPE_H264 == byVideoType)
        {
            u8 byConfFormat = tConfInfo.GetSecVideoFormat();
            if ( byConfFormat != VIDEO_FORMAT_CIF )
            {
                return FALSE;
            }
        }          
    }
    */

    return bSupported;
}
/*=============================================================================
�� �� ���� GetSelVmpCap
��    �ܣ� ��ȡ��ӽ�����������vmp id��������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 abySelVmpIdx[]				[in] 
TPeriEqpStatus *ptStatus		[in] 
u8 bySelNum						[in]
u8 byMaxChnlNumByConf			[in]
u8 &byVmpCapChnnlNum			[out]
u8 &bySelVmpId					[out]
�� �� ֵ�� BOOL32
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
12/9/2008				Ѧ��					
=============================================================================*/
BOOL32 CMcuPfmLmt::GetSelVmpCap(u8 abySelVmpIdx[], TPeriEqpStatus *ptStatus, u8 bySelNum, 
								u8 byMaxChnlNumByConf,u8 &byVmpCapChnnlNum, u8 &bySelVmpId)
{
	if( bySelNum == 0 )
	{
		return FALSE;
	}

	u8 byMaxEqpChnlNum = 0;
	u8 byMaxEqpChnlNumId = 0;
	u8 byMinNearConfChnlNum = MAXNUM_MPU2VMP_MEMBER +1; /*16+1;*/ // xliang [2/12/2009] 
	u8 byMinNearConfChnlNumId = 0;
	
	u8 bySelIdx = 0;
	u8 byCurEqpChnnlNum = 0;
	for(u8 byLoop = 0; byLoop < bySelNum; byLoop++)
	{
		bySelIdx = abySelVmpIdx[byLoop];
		byCurEqpChnnlNum = ptStatus[bySelIdx].m_tStatus.tVmp.m_byChlNum;
		
		if ( byCurEqpChnnlNum > byMaxEqpChnlNum )
		{
			// ͨ���������Ҳ���¼�����豸�е��������ֵ
			byMaxEqpChnlNum = byCurEqpChnnlNum;
			byMaxEqpChnlNumId = ptStatus[bySelIdx].GetEqpId();
		}
		if ( byCurEqpChnnlNum >= byMaxChnlNumByConf )
		{
			// �����������������������¼�����е���Сֵ
			if ( byCurEqpChnnlNum < byMinNearConfChnlNum )
			{
				byMinNearConfChnlNum = byCurEqpChnnlNum;
				byMinNearConfChnlNumId = ptStatus[bySelIdx].GetEqpId();
			}
		}
	}

	if (byMinNearConfChnlNumId >= VMPID_MIN &&
		byMinNearConfChnlNumId <= VMPID_MAX)
	{
		byVmpCapChnnlNum = byMaxChnlNumByConf;
		bySelVmpId = byMinNearConfChnlNumId;
	}
	else if (byMaxEqpChnlNumId >= VMPID_MIN && 
		     byMaxEqpChnlNumId <= VMPID_MAX)
	{
		byVmpCapChnnlNum = byMaxEqpChnlNum;
		bySelVmpId = byMaxEqpChnlNumId;
	}
    else if (byMinNearConfChnlNumId >= VMPTWID_MIN &&
             byMinNearConfChnlNumId <= VMPTWID_MAX)
    {
        byVmpCapChnnlNum = byMaxChnlNumByConf;
        bySelVmpId = byMinNearConfChnlNumId;
    }
    else if (byMaxEqpChnlNumId >= VMPTWID_MIN && 
             byMaxEqpChnlNumId <= VMPTWID_MAX)
    {
        byVmpCapChnnlNum = byMaxEqpChnlNum;
        bySelVmpId = byMaxEqpChnlNumId;
	}
	else
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_PFM, "[GetSelVmpCap] no find any vmp\n");
		return FALSE;
	}
// 	if (byMaxEqpChnlNum >= byMaxChnlNumByConf)
// 	{
// 		byVmpCapChnnlNum = byMaxChnlNumByConf;
// 		bySelVmpId = byMinNearConfChnlNumId;
// 	}
// 	else
// 	{
// 		byVmpCapChnnlNum = byMaxEqpChnlNum;
// 		bySelVmpId = byMaxEqpChnlNumId;
// 	}
	return TRUE;
}

#endif  //__MCUPFMLMT_H_



