#include "kdvtype.h"
#include "osp.h"
#include "kdvdef.h"
#include "codecwrapper_hd.h"
#include "kdvdef.h"
#include "mcueqpsim.h"

NULL_CLASS_DEF(CVideoDecoder);

CKdvVidDec::CKdvVidDec()
{

}
CKdvVidDec::~CKdvVidDec()
{
	SAFE_DEL_PTR(m_pCodec);
}

u16	   CKdvVidDec::CreateDecoder(TDecoder *pDecoer)
{
	return CODEC_NO_ERROR;
}

u16    CKdvVidDec::StartDec()/*��ʼͼ�����*/
{
	return CODEC_NO_ERROR;
}
u16    CKdvVidDec::StopDec() /*ֹͣͼ�����*/
{
	return CODEC_NO_ERROR;
}

u16    CKdvVidDec::GetDecoderStatus(TKdvDecStatus &tKdvDecStatus)/*��ȡ������״̬*/
{
	return CODEC_NO_ERROR;
}

u16    CKdvVidDec::GetDecoderStatis(TKdvDecStatis &tKdvDecStatis)/*��ȡ��������ͳ����Ϣ*/
{
	return CODEC_NO_ERROR;
}
	
u16    CKdvVidDec::GetCodecType(u32& dwType)  /*��������������λ�� VID_CODECTYPE_FPGA VID_CODECTYPE_DSP*/
{
	return CODEC_NO_ERROR;
}
	
u16    CKdvVidDec::SetPlayScale(u16 wWidth, u16 wHeigh)
{
	return CODEC_NO_ERROR;
}

u16    CKdvVidDec::SetData(const TFrameHeader& tFrameInfo)        /*������Ƶ��������*/
{
	return CODEC_NO_ERROR;
}
	
	/*������Ƶ�����������*/
u16    CKdvVidDec::SetVidDropPolicy(u8 byDropPolicy)
{
	return CODEC_NO_ERROR;
}
	
u16    CKdvVidDec::SetDisplayType(u8 byDisplayType)/*���û�����ʾ����(VIDEO VGA)*/
{
	return CODEC_NO_ERROR;
}    

u16    CKdvVidDec::SetGetVideoScaleCallBack(TDecodeVideoScaleInfo tDecodeVideoScaleInfo)
{
	return CODEC_NO_ERROR;
}

u16    CKdvVidDec::SetVideoDecParam(TVideoDecParam *ptVidDecParam)
{
	return CODEC_NO_ERROR;
}
    
u16    CKdvVidDec::SetVidPlyPortType(u32 dwType)
{
	return CODEC_NO_ERROR;
}

u16    CKdvVidDec::SetVideoPlyInfo(TVidSrcInfo* ptInfo)  /*������Ƶ����ź���ʽ��NULL��ʾ�Զ�������*/
{
	return CODEC_NO_ERROR;
}

// [6/29/2011 liuxu] Add
/*����hdu����ģʽ��ָhdu-1��hdu-2ģʽ��hdu��hdu-d��������hdu����ģʽ*/
u32    CKdvVidDec::SetWorkMode(u32 dwWorkMode)
{
	return 0;
}

u16    CKdvVidDec::GetVideoSrcInfo(TVidSrcInfo& tInfo)   /*��ȡ�����Ƶ�ź���ʽ*/
{
	return CODEC_NO_ERROR;
}
    //u16    SetVidPlyDefault(BOOL32 bNtsc = FALSE); /*�Ƿ�Ĭ�ϲ��� 30/60����*/
    //u16    SetVidPlyVgaDefault(u32 dwFreq = 60); /*VGA����Ƶ��Ĭ��60Hz������75,85��*/
    //u16    SetAutoCheckPlayInfo(BOOL32 bAuto);    /*�Ƿ��Զ���������֡��*/
	
u16    CKdvVidDec::ClearWindow()  /*����*/
{
	return CODEC_NO_ERROR;
}

u16    CKdvVidDec::FreezeVideo()  /*��Ƶ����*/
{
	return CODEC_NO_ERROR;
}

u16    CKdvVidDec::UnFreezeVideo()/*ȡ����Ƶ����*/
{
    return CODEC_NO_ERROR;
}	

u16    CKdvVidDec::SetVidDecResizeMode(s32 nMode)
{
	return CODEC_NO_ERROR;
}

u16		CKdvVidDec::SetNoStreamBak(unsigned long,unsigned char *,unsigned long)
{
	return CODEC_NO_ERROR;
}

NULL_CLASS_DEF(CAudioDecoder);
CKdvAudDec::CKdvAudDec()
{

}
CKdvAudDec::~CKdvAudDec()
{
	SAFE_DEL_PTR(m_pCodec);
}

u16	   CKdvAudDec::CreateDecoder(TDecoder *pDecoer)
{
	return CODEC_NO_ERROR;
}

u16    CKdvAudDec::GetDecoderStatus(TKdvDecStatus &tKdvDecStatus)/*��ȡ������״̬*/
{
	return CODEC_NO_ERROR;
}

u16    CKdvAudDec::GetDecoderStatis(TKdvDecStatis &tKdvDecStatis)/*��ȡ��������ͳ����Ϣ*/
{
	return CODEC_NO_ERROR;
}	
	
u16    CKdvAudDec::SetData(const TFrameHeader& tFrameInfo) /*������Ƶ��������*/
{
	return CODEC_NO_ERROR;
}

u16    CKdvAudDec::SetAudioDecParam(const TAudioDecParam& tAudParam)/*��Ƶ���������AAC_LC��Ч*/
{
	return CODEC_NO_ERROR;
} 
    
u16    CKdvAudDec::SetAudOutPort(u32 dwAudPort)  /*������Ƶ���Ŷ˿� HDMI or C*/
{
	return CODEC_NO_ERROR;
}
	
u16    CKdvAudDec::StartDec()/*��ʼ��������*/
{
	return CODEC_NO_ERROR;
}	

u16    CKdvAudDec::StopDec() /*ֹͣ��������*/
{
	return CODEC_NO_ERROR;
}	

u16    CKdvAudDec::SetAudioVolume(u8 byVolume )  /*���������������*/
{
	return CODEC_NO_ERROR;
}	

u16    CKdvAudDec::GetAudioVolume(u8 &pbyVolume )/*�õ������������*/
{
	return CODEC_NO_ERROR;
}	

u16    CKdvAudDec::SetAudioMute(BOOL32 bMute)/*���þ���*/
{
	return CODEC_NO_ERROR;
}
	
	/*��ȡ�����Ƶ����*/
u32    CKdvAudDec::GetOutputAudioPower()
{
	return CODEC_NO_ERROR;
}
	
	//�������� ������
u16   CKdvAudDec::PlayRing(s8* pData /*= NULL*/)
{
	return CODEC_NO_ERROR;
}
