/*****************************************************************************
ģ����      : h320�ն�����ģ��
�ļ���      : MtAdpcom.cpp
����ļ�    : MtAdpcom.h
�ļ�ʵ�ֹ���: ����ģ��
����        : ������
�汾        : V3.6  Copyright(C) 1997-2003 KDC, All rights reserved.
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
2005/06/07  3.6         ������      ����
******************************************************************************/

#include "mtadpcom.h"
#include "mtadpinst.h"

extern u8  g_byLsdType;
/*=============================================================================
  �� �� ���� CapSetIn2Out
  ��    �ܣ� ���ϲ����������ת��ΪЭ��ջ���������� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TCapSupport &tCapSupport [in] ����������
             u32 dwTransCap           [in] ����������
             TCapSetEx &tCapSet       [out]Э��ջ������
  �� �� ֵ�� void 
=============================================================================*/
void CapSetIn2Out(TCapSupport &tCapSupport, u32 dwTrasnCap, TKdvH320CapSet &tCapSet)
{
    TSimCapSet tSim1, tSim2;
    tSim1 = tCapSupport.GetMainSimCapSet();
    tSim2 = tCapSupport.GetSecondSimCapSet();

    //transrate
    if(transcap_none != dwTrasnCap)
    {
        tCapSet.AddTransCap(dwTrasnCap);
    }

    //audio
    u8 byType = tSim1.GetAudioMediaType();
    u32 dwCap = StackType2Cap(MediaTypeIn2Out(byType));
    if(audcap_none != dwCap)
    {
        //temp for zhongxing mt
        if(audcap_g711u == dwCap)
        {
            tCapSet.AddAudCap(audcap_g711a);
        }

        tCapSet.AddAudCap(dwCap);
    }    

    //video
    byType = tSim1.GetVideoMediaType();    

    dwCap = StackType2Cap(MediaTypeIn2Out(byType), tSim1.GetVideoResolution());
    if(vidcap_none != dwCap)
    {
        tCapSet.AddVidCap(dwCap, vidcap_mpi_12997);
    }
    
    //lsd
    if( /* tCapSupport.GetSupportH224() && */ g_byLsdType != 0)
    {
        if(1 == g_byLsdType)
        {
            tCapSet.AddLsdCap(lsdcap_6400);
        }
        else if(2 == g_byLsdType)
        {
            tCapSet.AddLsdCap(lsdcap_4800);
        }
        tCapSet.m_byH224LsdCap = TRUE;
    }

    if(!tSim2.IsNull())
    {
        //audio
        byType = tSim2.GetAudioMediaType();
        dwCap = StackType2Cap(MediaTypeIn2Out(byType));
        if(audcap_none != dwCap)
        {
            tCapSet.AddAudCap(dwCap);
        }    
        
        //video
        byType = tSim2.GetVideoMediaType();

        dwCap = StackType2Cap(MediaTypeIn2Out(byType), tSim2.GetVideoResolution());
        if(vidcap_none != dwCap)
        {
            tCapSet.AddVidCap(dwCap, vidcap_mpi_12997);
        }
    } 
}

/*=============================================================================
  �� �� ���� StackType2Cap
  ��    �ܣ� ��Э��ջ����ת��Ϊ������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byType      [in] ����
  �� �� ֵ�� u32            [out]������
=============================================================================*/
u32 StackType2Cap(u8 byMediaType, u8 byResolution)
{
    u32 dwCap = 0;
    switch(byMediaType)
    {
        //��Ƶ
    case audtype_g711a_56K:
        dwCap = audcap_g711a;
        break;

    case audtype_g711u_56K:
        dwCap = audcap_g711u;
        break;

    case audtype_g722_56K:
        dwCap = audcap_g722_48;
        break;

    case audtype_g728:
        dwCap = audcap_g728;
        break;

    case audtype_g729:
        dwCap = audcap_g729;
        break;

    case audtype_g7231:
        dwCap = audcap_g7231;
        break;

        //��Ƶ
    case vidtype_h261:
        {
            if(VIDEO_FORMAT_QCIF == byResolution)
            {
                dwCap = vidcap_h261qcif;
            }
            else
            {
                dwCap = vidcap_h261cif;
            }
        }        
        break;

    case vidtype_h263:
        {
            if(VIDEO_FORMAT_QCIF == byResolution || VIDEO_FORMAT_SQCIF == byResolution)
            {
                dwCap = vidcap_h263qcif;
            }
            else if(VIDEO_FORMAT_4CIF == byResolution)
            {
                dwCap = vidcap_h263_4cif;
            }
            else if(VIDEO_FORMAT_16CIF == byResolution)
            {
                dwCap = vidcap_h263_16cif;
            }
            else
            {
                dwCap = vidcap_h263cif;
            }
        }
        break;

    default:
        break;
    }

    return dwCap;
}

/*=============================================================================
  �� �� ���� MediaTypeIn2Out
  ��    �ܣ� ���ϲ����ý������ת��ΪЭ��ջý������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byMediaType [in] ����ý������
  �� �� ֵ�� u8            [out]Э��ջý������
=============================================================================*/
u8  MediaTypeIn2Out(u8 byMediaType)
{
    u8 byType = payloadtype_none;
    switch(byMediaType)
    {
        //��Ƶ
    case MEDIA_TYPE_PCMA:
        byType = audtype_g711a_56K;
        break;

    case MEDIA_TYPE_PCMU:
        byType = audtype_g711u_56K;
        break;
        
    case MEDIA_TYPE_G722:
        byType = audtype_g722_56K;
        break;

    case MEDIA_TYPE_G728:
        byType = audtype_g728;
        break;

    case MEDIA_TYPE_G729:
        byType = audtype_g729;
        break;

    case MEDIA_TYPE_G7231:
        byType = audtype_g7231;
        break;        

        //��Ƶ
    case MEDIA_TYPE_H261:	
        byType = vidtype_h261;
        break;      
        
    case MEDIA_TYPE_H263:
        byType = vidtype_h263;
        break;

    default:
        break;
    }

    return byType;
}

/*=============================================================================
  �� �� ���� MediaTypeIn2Out
  ��    �ܣ� ��Э��ջý������ת��Ϊ�ϲ����ý������ 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byMediaType [in] Э��ջý������
  �� �� ֵ�� u8             [out] ����ý������
=============================================================================*/
u8  MediaTypeOut2In(u8 byMediaType)
{
    u8 byType = MEDIA_TYPE_NULL;
    switch(byMediaType)
    {
        //��Ƶ
    case audtype_g711a_56K:
        byType = MEDIA_TYPE_PCMA;
        break;
        
    case audtype_g711u_56K:
        byType = MEDIA_TYPE_PCMU;
        break;
        
    case audtype_g722_56K:
        byType = MEDIA_TYPE_G722;
        break;
        
    case audtype_g728:
        byType = MEDIA_TYPE_G728;
        break;
        
    case audtype_g729:
        byType = MEDIA_TYPE_G729;
        break;
        
    case audtype_g7231:
        byType = MEDIA_TYPE_G7231;
        break;     
        
        //��Ƶ
    case vidtype_h261:	
        byType = MEDIA_TYPE_H261;
        break;     
        
    case vidtype_h263:
        byType = MEDIA_TYPE_H263;
   
    default:
        break;
    }
    
    return byType;
}

/*=============================================================================
  �� �� ���� FeccActionIn2Out
  ��    �ܣ� ���ϲ�Զң����ת��ΪЭ��ջԶң����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byParam     [in] mcuԶң����
  �� �� ֵ�� u8             [out]Э��ջԶң����
=============================================================================*/
u8  FeccActionIn2Out(u8 byParam)
{
    u8 byAction = feccreq_Invalid;
    switch(byParam)
    {
    case CAMERA_CTRL_UP:
        byAction = feccreq_TiltUp;
        break;

    case CAMERA_CTRL_DOWN:
        byAction = feccreq_TiltDown;
        break;

    case CAMERA_CTRL_LEFT:
        byAction = feccreq_PanLeft;
        break;

    case CAMERA_CTRL_RIGHT:
        byAction = feccreq_PanRight;
        break;

    case CAMERA_CTRL_ZOOMIN:
        byAction = feccreq_ZoomIn;
        break;

    case CAMERA_CTRL_ZOOMOUT:
        byAction = feccreq_ZoomOut;
        break;

    case CAMERA_CTRL_FOCUSIN:
        byAction = feccreq_FocusIn;
        break;

    case CAMERA_CTRL_FOCUSOUT:
        byAction = feccreq_FocusOut;
        break;

    default:
        break;
    }

    return byAction;
}

/*=============================================================================
  �� �� ���� FeccActionOut2In
  ��    �ܣ� ��Э��ջԶң����ת��Ϊ�ϲ�Զң����
  ��    ���� u8 byAction     [in] Э��ջԶң����
  �� �� ֵ�� u8             [out] mcuԶң����
=============================================================================*/
u8  FeccActionOut2In(u8 byAction)
{
    u8 byParam = 0;
    switch(byAction)
    {
    case feccreq_TiltUp:
        byParam = CAMERA_CTRL_UP;
        break;
        
    case feccreq_TiltDown:
        byParam = CAMERA_CTRL_DOWN;
        break;
        
    case feccreq_PanLeft:
        byParam = CAMERA_CTRL_LEFT;
        break;
        
    case feccreq_PanRight:
        byParam = CAMERA_CTRL_RIGHT;
        break;
        
    case feccreq_ZoomIn:
        byParam = CAMERA_CTRL_ZOOMIN;
        break;
        
    case feccreq_ZoomOut:
        byParam = CAMERA_CTRL_ZOOMOUT;
        break;
        
    case feccreq_FocusIn:
        byParam = CAMERA_CTRL_FOCUSIN;
        break;
        
    case feccreq_FocusOut:
        byParam = CAMERA_CTRL_FOCUSOUT;
        break;
        
    default:
        break;
    }
    
    return byParam;
}

/*=============================================================================
  �� �� ���� MuxMode2Cap
  ��    �ܣ� �ɸ���ģʽת��Ϊ�ϲ���������ʽ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TKDVH320MuxMode &tMuxMode      [in] ����ģʽ
             TCapSupport &tCap              [out]������
  �� �� ֵ�� void 
=============================================================================*/
void MuxMode2Cap(TKDVH320MuxMode &tMuxMode, TKdvH320CapSet &tLocalCapSet, TCapSupport &tCap)
{
    TSimCapSet tSimCap;
    
    //cascade
    tCap.SetSupportMMcu(1);

    //audio
    switch(tMuxMode.m_byAudType)
    {
    case audtype_g711a_56K:
        tSimCap.SetAudioMediaType(MEDIA_TYPE_PCMA);
        break;

    case audtype_g711u_56K:
        tSimCap.SetAudioMediaType(MEDIA_TYPE_PCMU);
        break;

    case audtype_g722_56K:
        tSimCap.SetAudioMediaType(MEDIA_TYPE_G722);
        break;

    case audtype_g728:
        tSimCap.SetAudioMediaType(MEDIA_TYPE_G728);
        break;

    case audtype_g7231:
        tSimCap.SetAudioMediaType(MEDIA_TYPE_G7231);
        break;

    case audtype_g729:
        tSimCap.SetAudioMediaType(MEDIA_TYPE_G729);
        break;
    default:
        tSimCap.SetAudioMediaType(MEDIA_TYPE_NULL);
        break;
    }

    //video
    switch(tMuxMode.m_byVidType)
    {
    case vidtype_h261:
        tSimCap.SetVideoMediaType(MEDIA_TYPE_H261);
        if(tLocalCapSet.IsExistCap(vidcap_h261qcif, CAPTYPE_VIDEO))
        {
            tSimCap.SetVideoResolution(VIDEO_FORMAT_QCIF);
        }
        else
        {
            tSimCap.SetVideoResolution(VIDEO_FORMAT_CIF);
        }
        break;

    case vidtype_h263:
        tSimCap.SetVideoMediaType(MEDIA_TYPE_H263);
        if(tLocalCapSet.IsExistCap(vidcap_h263qcif, CAPTYPE_VIDEO))
        {
            tSimCap.SetVideoResolution(VIDEO_FORMAT_QCIF);
        }
        else if(tLocalCapSet.IsExistCap(vidcap_h263_4cif, CAPTYPE_VIDEO))
        {
            tSimCap.SetVideoResolution(VIDEO_FORMAT_4CIF);
        }
        else if(tLocalCapSet.IsExistCap(vidcap_h263_16cif, CAPTYPE_VIDEO))
        {
            tSimCap.SetVideoResolution(VIDEO_FORMAT_16CIF);
        }
        else
        {
            tSimCap.SetVideoResolution(VIDEO_FORMAT_CIF);
        }
        break;

    default:
        tSimCap.SetVideoMediaType(MEDIA_TYPE_NULL);
        break;
    }

    //lsd
    switch(tMuxMode.m_byLSDType)
    {
    case lsdtype_6400:
    case lsdtype_14_4K:
        tCap.SetSupportH224(1);
        break;

    default:
        break;
    }

    tCap.SetMainSimCapSet(tSimCap);
    tCap.SetEncryptMode(CONF_ENCRYPTMODE_NONE);
}

/*=============================================================================
  �� �� ���� IsEqualMuxMode
  ��    �ܣ� �ж�������ģʽ�Ƿ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TKDVH320MuxMode &tMuxMode1     [in]
             TKDVH320MuxMode &tMuxMode2     [in]
  �� �� ֵ�� BOOL32     (true - ��ȣ�false - ����)
=============================================================================*/
BOOL32 IsEqualMuxMode(const TKDVH320MuxMode &tMuxMode1, const TKDVH320MuxMode &tMuxMode2)
{
    if(tMuxMode1.m_byAudType == tMuxMode2.m_byAudType &&
       tMuxMode1.m_byVidType == tMuxMode2.m_byVidType &&
       tMuxMode1.m_byLSDType == tMuxMode2.m_byLSDType &&
       tMuxMode1.m_byHSDType == tMuxMode2.m_byHSDType)
    {
        return TRUE;
    }

    return FALSE;
}

/*=============================================================================
  �� �� ���� GetAudioBitrate
  ��    �ܣ� �õ���Ƶ�ı��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byAudioType     [in] ��Ƶ����
  �� �� ֵ�� u16                [out]��Ƶ�ı��������
=============================================================================*/
u16 GetAudioBitrate(u8 byAudioType)
{
    u16 wBitrate = 0;
    switch(byAudioType)
    {
    case MEDIA_TYPE_PCMA:
        wBitrate = AUDIO_BITRATE_G711A;
        break;
    case MEDIA_TYPE_PCMU:
        wBitrate = AUDIO_BITRATE_G711U;
        break;
    case MEDIA_TYPE_G722:
        wBitrate = AUDIO_BITRATE_G722;
        break;
    case MEDIA_TYPE_G7231:
        wBitrate = AUDIO_BITRATE_G7231;
        break;
    case MEDIA_TYPE_G728:
        wBitrate = AUDIO_BITRATE_G728;
        break;
    case MEDIA_TYPE_G729:
        wBitrate = AUDIO_BITRATE_G729;
        break;
    case MEDIA_TYPE_MP3:
        wBitrate = AUDIO_BITRATE_MP3;
        break;
    case MEDIA_TYPE_G7221C:
        wBitrate = AUDIO_BITRATE_G7221C;
        break;
    case MEDIA_TYPE_AACLC:
    case MEDIA_TYPE_AACLD:
        wBitrate = AUDIO_BITRATE_AAC;
        break;
    default:
        break;
    }
    return wBitrate;
}

/*=============================================================================
  �� �� ���� GetTsMaskAndTransFromBitrate
  ��    �ܣ� �ӻ������ʻ��ʱ϶����ʹ�����
  �㷨ʵ�֣� Ѱ����ӽ���������������
  ȫ�ֱ����� 
  ��    ���� u16 wBitrate       [in] ��������
             u32 &dwTsMask      [out]ʱ϶����
             u32 &dwTransCap    [out]������
  �� �� ֵ�� BOOL32         (TRUE-�ɹ���FALSE-ʧ��)
=============================================================================*/
BOOL32 GetTsMaskAndTransFromBitrate(u16 &wBitrate, u32 &dwTsMask, u32 &dwTransCap)
{
    if(wBitrate > 1920)
    {
        return FALSE;
    }
    else if(wBitrate > 1536)    //h12 (30Ts)
    {
        dwTsMask = tsmode_1920K;
        dwTransCap = transcap_h12;
        wBitrate = 1920;
    }
    else if(wBitrate > 1472)    //h11 (24Ts)
    {
        dwTsMask = tsmode_1536K;
        dwTransCap = transcap_h11;
        wBitrate = 1536;
    }
    else if(wBitrate > 1152)    //(23Ts)
    {
        dwTsMask = tsmode_1472K;
        dwTransCap = transcap_1472K;
        wBitrate = 1472;
    }
    else if(wBitrate > 768)     //(18Ts)
    {
        dwTsMask = tsmode_1152K;
        dwTransCap = transcap_1152K;
        wBitrate = 1152;
    }
    else if(wBitrate > 512)     //(12Ts)
    {
        dwTsMask = tsmode_768K;
        dwTransCap = transcap_768K;
        wBitrate = 768;
    }
    else if(wBitrate > 384)     //(8Ts)
    {
        dwTsMask = tsmode_512K;
        dwTransCap = transcap_512K;
        wBitrate = 512;
    }
    else if(wBitrate > 320)     //h0 (6Ts)
    {  
        dwTsMask = tsmode_384K;
        dwTransCap = transcap_h0;
        wBitrate = 384;
    }
    else if(wBitrate > 256)     //(5Ts)
    {
        dwTsMask = tsmode_320K;
        dwTransCap = transcap_320K;
        wBitrate = 320;
    }
    else if(wBitrate > 192)     //(4Ts)
    {
        dwTsMask = tsmode_256K;
        dwTransCap = transcap_256K;
        wBitrate = 256;
    }
    else if(wBitrate > 128)     //(3Ts)
    {
        dwTsMask = tsmode_192K;
        dwTransCap = transcap_192K;
        wBitrate = 192;
    }
    else if(wBitrate > 64)      //2Ts
    {
        dwTsMask = tsmode_128K;
        dwTransCap = transcap_128K;
        wBitrate = 128;
    }
    else                        //1Ts
    {
        dwTsMask = tsmode_64K;
        dwTransCap = transcap_1b;
        wBitrate = 64;
    }

    return TRUE;
}

/*=============================================================================
  �� �� ���� StrIPv4
  ��    �ܣ� ��ip��ַת��Ϊ�ַ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwIP
  �� �� ֵ�� s8 * 
=============================================================================*/
s8 * StrIPv4(u32 dwIP)
{
    static s8 szIP[18];
    u8 *p = (u8 *)&dwIP;
    sprintf(szIP,"%d.%d.%d.%d%c",p[0],p[1],p[2],p[3],0);
	return szIP;
}
