/* CodecLib.h - Codec API Header */ 

/* Copyright(C) 2001-2002 KDC, All rights reserved. 
˵��:CODEC�ϲ�API��װ����
����:������ κ�α�
*/
#include "kdvtype.h"
#include "osp.h"
#include "codeclib.h"
#include "mcueqpsim.h"
//ģ��汾��Ϣ�ͱ���ʱ�� �� �����Ŀ�İ汾�ͱ���ʱ��
void kdvencoderver()
{
}
void kdvdecoderver()
{
}

//ģ�������Ϣ �� �����Ŀ�İ�����Ϣ
void kdvencoderhelp()
{
}
void kdvdecoderhelp()
{
}

#ifdef SOFT_CODEC
class CSoftEncoder{};
class TEncoderRef{};
class TEncoderStatus{};
#else
class CVideoEncoder{};
class CAudioEncoder{};
#endif

CKdvEncoder::CKdvEncoder()
{
}
CKdvEncoder::~CKdvEncoder()
{

#ifdef SOFT_CODEC
    SAFE_DEL_PTR(m_pcKdvInterEnc);
    SAFE_DEL_PTR(m_ptRef);
    SAFE_DEL_PTR(m_ptStatus);	
#else
    SAFE_DEL_PTR(pVideoEncoder);
    SAFE_DEL_PTR(pAudioEncoder);
#endif
}

	
#ifdef SOFT_CODEC      
    //��ʼ�����������
u16 CKdvEncoder::Create( HWND previewHwnd, s32 nPriority ,u8 byBindCpuId, u8 byCapType)
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::SetVoiceCheck(BOOL32 bCheck,const TVoiceCheck *ptVoiceCheck)/*�������ʼ��*/	
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StartCap(u8 byCapType)//��ʼ����
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StopCap(u8 byCapType) //ֹͣ����
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::SetPreviewHwnd( HWND hPrevWnd )
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::SelectVideoSource(const char *szFileName)//ѡ����ƵԴ
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::SetVideoIcon( BOOL32 bEnable )//bEnable,TRUEΪ���ã�FALSE���衣
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::SetVideoCapParam(const TVideoCapParam  *ptVideoCapParam )//����ͼ�񲶻����
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::SetAudioCapParam( u8 byMode )
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::GrabPic(CDrawWnd *pDrawWnd)//��pDrawWnd��Чʱ���ص��ӿڣ�����Ϊ��.
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::SaveAsPic(LPCSTR lpszFileName, u8 byEncodeMode)
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::SetAudioCallBack(PAUDIOCALLBACK pAudioCallBack, u32 dwContext)
{
	return CODEC_NO_ERROR;
}

	// ���� ��ȡ������Ƶ���� �Ļص�
u16 CKdvEncoder::SetInputAudioPowerCB(PGETAUDIOPOWER pGetInputAudioPowerCB, u32 dwContext)
{
	return CODEC_NO_ERROR;
}

#else // vxworks
u16 CKdvEncoder::CreateEncoder(const TEncoder *ptVideoEncoder, 
							const TEncoder *ptAudioEncoder)/*��ʼ��������*/
{
	return CODEC_NO_ERROR;
}
	/*��Ƶ���������뵥��ƵԴ˫������*/
u16 CKdvEncoder::AddVidEncToSingleCapGroup(u16  wAddtoChnNum)
{
	return CODEC_NO_ERROR;
}
	
	/*���;�̬ͼƬ*/
u16 CKdvEncoder::SetSendStaticPic(BOOL32  bSendStaticPic)
{
	return CODEC_NO_ERROR;
}
	/*��ȡ����ץ��ͼƬ*/
u16 CKdvEncoder::SetSnapshotMaxSize(u32  dwSnapshotMaxSize)
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::Snapshot()
{
	return CODEC_NO_ERROR;
}
u32 CKdvEncoder::GetSnapshotSaveSize()
{
	return CODEC_NO_ERROR;
}
u32 CKdvEncoder::GetSnapshotSavePicNum()
{
	return CODEC_NO_ERROR;
}

	/*��ȡ������Ƶ����*/
u32 CKdvEncoder::GetInputAudioPower()
{
	return 2;
}

u16 CKdvEncoder::ScaleVideoCap(u8 byType, u8 byValue) /* ��Ƶ�ɼ����� */
{
	return CODEC_NO_ERROR;
}
#endif

u16 CKdvEncoder::GetEncoderStatus(TKdvEncStatus &tKdvEncStatus)/*��ȡ������״̬*/ 
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::GetEncoderStatis(TKdvEncStatis &tKdvEncStatis )/*��ȡ��������ͳ����Ϣ*/
{
	return CODEC_NO_ERROR;
}
//#ifndef WIN32
u16 CKdvEncoder::StartVideoCap( u8 byCapType )/*��ʼ�ɼ�ͼ��*/
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StopVideoCap() /*ֹͣ�ɼ�ͼ��*/
{
	return CODEC_NO_ERROR;
}
//#endif
u16 CKdvEncoder::SetVideoEncParam(const TVideoEncParam *ptVideoEncParam )/*������Ƶ�������*/
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::GetVideoEncParam(TVideoEncParam &tVideoEncParam )       /*�õ���Ƶ�������*/
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StartVideoEnc()/*��ʼѹ��ͼ��*/
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StopVideoEnc() /*ֹͣѹ��ͼ��*/
{
	return CODEC_NO_ERROR;
}
u16	CKdvEncoder::SetMpv4FrmPara(const TMpv4FrmParam *ptMpv4FrmPara)	/*����mpv4֡����*/ 
{
	return CODEC_NO_ERROR;
}
	
//#ifndef WIN32
u16 CKdvEncoder::StartAudioCap(BOOL32 bAinMic)/*��ʼ�����ɼ�*/
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StopAudioCap() /*ֹͣ�����ɼ�*/
{
	return CODEC_NO_ERROR;
}
//#endif
u16 CKdvEncoder::SetAudioEncParam(u8 byAudioEncParam, 
                            u8 byMediaType,
                            u16 wAudioDuration )/*��������ѹ������*/
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::GetAudioEncParam(u8 &byAudioEncParam, 
                            u8 *pbyMediaType,
                            u16 *pwAudioDuration )/*�õ�����ѹ������*/
{
	return CODEC_NO_ERROR;
}

u16 CKdvEncoder::StartAudioEnc()/*��ʼѹ������*/
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StopAudioEnc() /*ֹͣѹ������*/
{
	return CODEC_NO_ERROR;	
}
u16 CKdvEncoder::SetAudioMute( BOOL32 bMute )     //�����Ƿ���
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::SetAudioVolume(u8 byVolume ) /*���òɼ�����*/	
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::GetAudioVolume(u8 &byVolume )/*��ȡ�ɼ�����*/	
{
	return CODEC_NO_ERROR;
}
	
	/*����ͼ��������ش�����*/
u16 CKdvEncoder::SetNetFeedbackVideoParam(u16 wBufTimeSpan, BOOL32 bRepeatSnd)
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::SetNetSndVideoParam(const TNetSndParam *ptNetSndParam)/*����ͼ������紫�Ͳ���*/	
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::SetNetSndAudioParam(const TNetSndParam *ptNetSndParam)/*�������������紫�Ͳ���*/
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StartSendVideo(int  dwSSRC)/*��ʼ����ͼ��*/	
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StopSendVideo() /*ֹͣ����ͼ��*/
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StartSendAudio(int  dwSSRC)/*��ʼ��������*/	
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StopSendAudio() /*ֹͣ��������*/	
{
	return CODEC_NO_ERROR;
}
u16	CKdvEncoder::SetAudioDumb(BOOL32 bDumb) /*�����Ƿ�����*/	 
{
	return CODEC_NO_ERROR;
}
	
u16 CKdvEncoder::StartAec( u8 byType )/*��ʼ��������*/
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StopAec() /*ֹͣ��������*/
{
	return CODEC_NO_ERROR;
}

u16	CKdvEncoder::StartAgcSmp() /*��ʼsmp����*/
{
	return CODEC_NO_ERROR;
}
u16	CKdvEncoder::StopAgcSmp() /*ֹͣsmp����*/
{
	return CODEC_NO_ERROR;
}
u16	CKdvEncoder::StartAgcSin() /*��ʼsin����*/
{
	return CODEC_NO_ERROR;
}
u16	CKdvEncoder::StopAgcSin() /*ֹͣsin����*/
{
	return CODEC_NO_ERROR;
}	
u16 CKdvEncoder::SetFastUpata(BOOL32 bIsNeedProtect)     /*����FastUpdata��MediaCtrl��75֡�ڱ�һ��I֡����*/
{
	return CODEC_NO_ERROR;
}
#ifdef SOFT_CODEC
VOID CKdvEncoder::GetLastError( u32 &dwErrorId )
{
	return 0;
}
#endif

NULL_CLASS_DEF(CFileEncoder);
NULL_CLASS_DEF(CKdvVideoDec);
NULL_CLASS_DEF(CKdvAudioDec);
NULL_CLASS_DEF(TDecoderRef);
NULL_CLASS_DEF(TDecoderStatus);
NULL_CLASS_DEF(CVideoDecoder);
NULL_CLASS_DEF(CAudioDecoder);

CKdvDecoder::CKdvDecoder()
{
}
CKdvDecoder::~CKdvDecoder()
{
	SAFE_DEL_PTR(m_pcFileEncoder);
#ifdef SOFT_CODEC
	SAFE_DEL_PTR(m_pcKdvVideoDec);
	SAFE_DEL_PTR(m_pcKdvAudioDec);
	SAFE_DEL_PTR(m_ptRef);
	SAFE_DEL_PTR(m_ptStatus);
#else
	SAFE_DEL_PTR(pVideoDecoder);
	SAFE_DEL_PTR(pAudioDecoder);
#endif
}
	

u16  CKdvDecoder::CreateDecoder(const TVideoDecoder *ptVideoDecoder,const TAudioDecoder *ptAudioDecoder)/*��ʼ��������*/	
{
	return CODEC_NO_ERROR;
}
u16 CKdvDecoder::GetDecoderStatus(TKdvDecStatus &tKdvDecStatus )/*��ȡ������״̬*/
{
	return CODEC_NO_ERROR;
}
u16    CKdvDecoder::GetDecoderStatis(TKdvDecStatis &tKdvDecStatis )/*��ȡ��������ͳ����Ϣ*/	
{
	return CODEC_NO_ERROR;
}

u16    CKdvDecoder::SetVideoDecType(u8 byType) /*����ͼ�����������, ֻ���ڿ�ʼ����ǰ���ò���Ч*/
{
	return CODEC_NO_ERROR;
}
u16    CKdvDecoder::StartVideoDec()/*��ʼͼ�����*/
{
	return CODEC_NO_ERROR;
}
    u16    CKdvDecoder::StopVideoDec() /*ֹͣͼ�����*/
	{
		return CODEC_NO_ERROR;
	}
	u16	   CKdvDecoder::SetVideoPlayFormat(BOOL32 bFormat) /* ������ʾ��ʽ TRUE: 16:9, FALSE: 4:3 */
	{
		return CODEC_NO_ERROR;
	}
	
	u16    CKdvDecoder::SetAudioDecType(u8 byType) /*������������������, ֻ���ڿ�ʼ����ǰ���ò���Ч*/
	{
		return CODEC_NO_ERROR;
	}
    u16    CKdvDecoder::StartAudioDec()/*��ʼ��������*/	
	{
		return CODEC_NO_ERROR;
	}
    u16    CKdvDecoder::StopAudioDec() /*ֹͣ��������*/	
	{
		return CODEC_NO_ERROR;
	}
    u16    CKdvDecoder::SetAudioVolume(u8 byVolume )/*���������������*/	
	{
		return CODEC_NO_ERROR;
	}
    u16    CKdvDecoder::GetAudioVolume(u8 &pbyVolume )/*�õ������������*/
	{
		return CODEC_NO_ERROR;
	}	
    u16    CKdvDecoder::SetAudioMute(BOOL32 bMute)/*���þ���*/
	{
		return CODEC_NO_ERROR;
		
	}
	
    u16    CKdvDecoder::StartRcvVideo()/*��ʼ�������ͼ��*/
	{
		return CODEC_NO_ERROR;

	}
    u16    CKdvDecoder::StopRcvVideo() /*ֹͣ�������ͼ��*/
	{
		return CODEC_NO_ERROR;

	}
    u16    CKdvDecoder::StartRcvAudio()/*��ʼ�����������*/	
	{
		return CODEC_NO_ERROR;

	}
    u16    CKdvDecoder::StopRcvAudio() /*ֹͣ�����������*/	
	{
		return CODEC_NO_ERROR;
	}		
    u16    CKdvDecoder::SetVideoNetRcvParam(const TLocalNetParam *ptLocalNetParam )/*����ͼ���������ղ���*/	
	{
		return CODEC_NO_ERROR;
	}		
    u16    CKdvDecoder::SetAudioNetRcvParam(const TLocalNetParam *ptLocalNetParam )/*����������������ղ���*/
	{
		return CODEC_NO_ERROR;
		
	}
	
	/*����ͼ��������ش�����*/
	u16    CKdvDecoder::SetNetFeedbackVideoParam(TNetRSParam tNetRSParam, BOOL32 bRepeatSnd)
	{
		return CODEC_NO_ERROR;
		
	}

	/*������������·������һ·����Map����win32������ƣ�һ·��������, ����NULL,��رպ�һ·��������Ӧ�׽����ͷ�*/
    u16    CKdvDecoder::SetVidDecDblSend(const TNetSndParam *ptNetSndVideoParam, u16 wBitRate/* = 4<<10*/)
	{
		return CODEC_NO_ERROR;
	}		
    u16    CKdvDecoder::SetAudDecDblSend(const TNetSndParam *ptNetSndAudioParam)
	{
		return CODEC_NO_ERROR;
		
	}


    u16    CKdvDecoder::ClearWindow() /*����*/
	{
		return CODEC_NO_ERROR;
	}		
    u16   CKdvDecoder::FreezeVideo() /*��Ƶ����*/
	{
		return CODEC_NO_ERROR;
	}		
    u16    CKdvDecoder::UnFreezeVideo()/*ȡ����Ƶ����*/
	{
		return CODEC_NO_ERROR;
		
	}


    u16    CKdvDecoder::SetDisplayMode(u8 byDisplayMode)     /*���û��л���ʾ��ʽ*/
	{
		return CODEC_NO_ERROR;
	}		
    u16    CKdvDecoder::SetDisplaySaturation(u8 bySaturation)/*������ʾ���Ͷ�*/
	{
		return CODEC_NO_ERROR;

	}
    u16    CKdvDecoder::SetDisplayContrast(u8 byContrast)    /*������ʾ�Աȶ�*/
	{
		return CODEC_NO_ERROR;
	}		
    u16    CKdvDecoder::SetDisplayBrightness(u8 byBrightness)/*������ʾ����*/	
	{
		return CODEC_NO_ERROR;
	}		
    u16    CKdvDecoder::SetLittlePicDisplayParam(u16 dwXPos,u16 dwYPos,u16 dwWidth,u16 dwHeight)/*����С������ʾλ����ߴ�*/
	{
		return CODEC_NO_ERROR;
	}		
    u16    CKdvDecoder::SetGetVideoScaleCallBack(TDecodeVideoScaleInfo tDecodeVideoScaleInfo)
	{
		return CODEC_NO_ERROR;
	}		
    u16    CKdvDecoder::GetAlarmInfo(TAlarmInfo &tAlarmInfo)
	{
		return CODEC_NO_ERROR;
		
	}
	u16    CKdvDecoder::SetVideoDecParam(TVideoDecParam *ptVidDecParam)
	{
		return CODEC_NO_ERROR;
		
	}
	
	/*����˫��Ƶ������*/
	u16    CKdvDecoder::SetDoubleVidStreamParam(u32  dwZoomLevel, u32  dwXPos, u32  dwYPos)
	{
		return CODEC_NO_ERROR;
		
	}
	
	/*��ȡ����ץ��ͼƬ*/
	u16    CKdvDecoder::SetSnapshotMaxSize(u32  dwSnapshotMaxSize)
	{
		return CODEC_NO_ERROR;
		
	}
	u16    CKdvDecoder::Snapshot()
	{
		return CODEC_NO_ERROR;
		
	}
	u32    CKdvDecoder::GetSnapshotSaveSize()
	{
		return CODEC_NO_ERROR;
		
	}
	u32    CKdvDecoder::GetSnapshotSavePicNum()
	{
		return CODEC_NO_ERROR;
		
	}
	
	/*��ȡ�����Ƶ����*/
	u32    CKdvDecoder::GetOutputAudioPower()
	{
		return CODEC_NO_ERROR;
		
	}


	//���� Զ�˶�̬��Ƶ�غɵ� Playloadֵ
	u16    CKdvDecoder::SetVideoActivePT( u8 byRmtActivePT, u8 byRealPT )
	{
		return CODEC_NO_ERROR;
		
	}
	//���� ��Ƶ����key�ִ� �Լ� ����ģʽ Aes or Des
    u16    CKdvDecoder::SetVidDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode) 
	{
		return CODEC_NO_ERROR;
		
	}
	//���� Զ�˶�̬��Ƶ�غɵ� Playloadֵ
	u16    CKdvDecoder::SetAudioActivePT( u8 byRmtActivePT, u8 byRealPT )
	{
		return CODEC_NO_ERROR;
		
	}
	//���� ��Ƶ����key�ִ� �Լ� ����ģʽ Aes or Des
    u16    CKdvDecoder::SetAudDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode) 
	{
		return CODEC_NO_ERROR;
		
	}


      /*�ļ�����������*/
    u16    CreateFileEncoder(char * pFielName)
	{
		return CODEC_NO_ERROR;
		
	}

	u16    DestroyFileEncoder()
	{
		return CODEC_NO_ERROR;
		
	}
	u16    StartFileEncoder()
	{
		return CODEC_NO_ERROR;
		
	}
	u16    StopFileEncoder()
	{
		return CODEC_NO_ERROR;
		
	}
	u16  PauseFileEncoder()
	{
		return CODEC_NO_ERROR;
		
	}
	u16    ContinueFileEncoder()
	{		return CODEC_NO_ERROR;
	
	}

	u16 SendVidFrameToMap(u16 dwStreamType, u16 dwFrameSeq, u16 dwWidth,
		                   u16 dwHeight, u8 *pbyData, u16 dwDataLen)
	{
		return CODEC_NO_ERROR;
		
	}
	u16 SendAudFrameToMap(u16 dwStreamType, u16 dwFrameSeq,  u8 byAudioMode, 
		                   u8 *pbyData, u16 dwDataLen)
	{
		return CODEC_NO_ERROR;
		
	}
	

	NULL_CLASS_DEF(CHardCodec);

	COsdMap::COsdMap()
	{
	}
    COsdMap::~COsdMap()
	{
		SAFE_DEL_PTR(m_pcCodecMap);
	}
	

	//��ʼ��
    u16 COsdMap::Initialize(u32 dwMapID)
	{
		return CODEC_NO_ERROR;
	}

	//��ʾOSD���ɹ����ر�Osd��ID�ţ�ʧ�ܷ���OSD_ERROR.
	int COsdMap::OsdShow(u8 *pbyBmp, 
		         u32 dwBmpLen, 
				 u32 dwXPos, 
				 u32 dwYPos, 
				 u32 dwWidth, 
				 u32 dwHeight, 
				 u8  byBgdColor, 
				 u32 dwClarity, 
				 BOOL32  bSlide,
				 u32 dwSlideTimes,
				 u32 dwSlideStride)
	{
		return CODEC_NO_ERROR;
	}

	//�ر�Osd, �ɹ�����OSD_OK; ʧ�ܷ���OSD_ERROR.
    int COsdMap::OsdClose(u32 dwXPos, u32 dwYPos, u32 dwWidth, u32 dwHeight, BOOL32 bSlide)
	{
		return CODEC_NO_ERROR;
	}
  
	//��ʼ̨����������
	int COsdMap::StartAddLogoInEncStream(u8 *pbyBmp, u32 dwBmpLen, u32 dwXPos, u32 dwYPos, u32 dwWidth, 
								u32 dwHeight, TBackBGDColor tBackBGDColor , u32 dwClarity, u32 dwChnNum, u32 dwCapPrt)
	{
		return CODEC_NO_ERROR;
	}
	//ֹ̨ͣ����������
	int COsdMap::StopAddLogoInEncStream(u32 dwChnNum, u32 dwCapPrt)
	{
		return CODEC_NO_ERROR;
	}

	//��ʼ����ͼ�����̨��
	int COsdMap::StartAddIconInLocal(u8 *pbyBmp, u32 dwBmpLen, u32 dwXPos, u32 dwYPos, u32 dwWidth, 
		u32 dwHeight, TBackBGDColor tBackBGDColor , u32 dwClarity, u32 dwChnNum)
	{
		return CODEC_NO_ERROR;
	}
	//ֹͣ����ͼ�����̨��
	int COsdMap::StopAddIconInLocal(u32 dwChnNum)
	{
		return CODEC_NO_ERROR;
	}
	
	//��ʼ������ʾ������Ļ(�������Ϲ�)
	int COsdMap::StartRunCaption(u8 *pbyBmp, u32 dwBmpLen, u32 dwTimes, u32 dwXPos, u32 dwSpeed, u32 dwWidth, 
		u32 dwHeight, u8  byBgdColor, u32 dwClarity, u32 dwChnNum)
	{
		return CODEC_NO_ERROR;
	}

	//ֹͣ������ʾ������Ļ
	int COsdMap::StopRunCaption(u32 dwChnNum)
	{
		return CODEC_NO_ERROR;
	}

	//��ȡOsdBuf��ַ
	u16 COsdMap::GetOsdBufAddr()
	{
		return CODEC_NO_ERROR;
	}
	
 
#ifdef HARD_CODEC


/* Ӳ�����������ʼ������, ������ʹ��Ӳ������������������ǰ����. �ɹ�����CODEC_NO_ERROR, ʧ�ܲμ������� */
int HardCodecInit(int dwCpuId, THardCodecInitParm *ptCodecInitParam, BOOL32  bKdv8010)
{
	return CODEC_NO_ERROR;
}
/* Ӳ����������˳�����������֮���û��ٲ��ܶ���Ӧ������������κβ��� */
void HardCodecQuit(int dwCpuId)
{
	return ;
}
/* �����ʾ���庯�����û��ڵ���HardCodecQuit��������֮ǰ�ȵ��ñ������Խ��reboot����������� */
void ClearPlayBuf()
{
	return ;
}

void codecwrapperver(u32 dwMapId)
{
	return ;
}
/*   ���÷���ɫ�����Ի���ʾɫ������
     dwChnNum -- ����ͨ���ţ�=255ʱ��ʾ���б���ͨ��������ɫ������ͼƬ
     dwTestPicType -- ɫ������ͼƬ�ţ�0--�ֱ��ʲ��ԣ�1--ɫ�Ȳ��ԣ�2--�ҶȲ��ԣ�255--ȡ������*/
BOOL32  SetSendTestPic(u32 dwMapId, u32 dwChnNum, u32 dwTestPicType)
{
	return TRUE;
}
BOOL32  SetPlayTestPic(u32 dwMapId, u32 dwTestPicType)
{
	return TRUE;
}


#endif














