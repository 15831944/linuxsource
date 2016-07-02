#include "kdvtype.h"
#include "osp.h"
#include "mcueqpsim.h"
#ifndef _HD_
#include "kdvadapter.h"
#else 
#include "kdvadapter_hd.h"
#endif

#ifndef _HD_
	NULL_CLASS_DEF(CVideoEncoder);
	NULL_CLASS_DEF(CVideoDecoder);
	NULL_CLASS_DEF(CAudioEncoder);
	NULL_CLASS_DEF(CAudioDecoder);
#else
	NULL_CLASS_DEF(CAdapterChannel);
#endif

CKdvAdapterGroup::CKdvAdapterGroup ()
{
}

CKdvAdapterGroup::~ CKdvAdapterGroup ()
{
#ifndef _HD_

#define DEL_TABLE_PTR(pp, v) do { for (int nLoop = 0; pp && nLoop < v; nLoop++) { SAFE_DEL_PTR(pp[nLoop]); } }while(0)


	DEL_TABLE_PTR(m_apVideoEncoder, MAX_VIDEO_ADAPT_CHANNUM);
	DEL_TABLE_PTR(m_apVideoDecoder, MAX_VIDEO_ADAPT_CHANNUM);
	DEL_TABLE_PTR(m_apAudioEncoder, MAX_AUDIO_ADAPT_CHANNUM);
	DEL_TABLE_PTR(m_apAudioDecoder, MAX_AUDIO_ADAPT_CHANNUM);
	
	DEL_TABLE_PTR(m_apTransVideoEncoder, MAX_TRANS_VIDEO_ADAPT_CHANNUM);
	DEL_TABLE_PTR(m_apTransVideoDecoder, MAX_TRANS_VIDEO_ADAPT_CHANNUM);
	DEL_TABLE_PTR(m_apTransAudioEncoder, MAX_TRANS_AUDIO_ADAPT_CHANNUM);
	DEL_TABLE_PTR(m_apTransAudioDecoder, MAX_TRANS_AUDIO_ADAPT_CHANNUM);

#else
	SAFE_DEL_PTR(m_pCodec);
#endif
}

#ifndef _HD_
	u16 CKdvAdapterGroup::CreateGroup(const TAdapterMap *ptAdapterMap, u8 byVideoChnNum, u8 byAudioChnNum)// ����������
	{
		return CODEC_NO_ERROR;
	}
	u16 CKdvAdapterGroup::StartGroup(void)//��ʼ����
	{
		return CODEC_NO_ERROR;
	}
	
   	u16 CKdvAdapterGroup::StopGroup(void)//ֹͣ����	
	{
		return CODEC_NO_ERROR;
	}
	
    u16 CKdvAdapterGroup::DestroyGroup(void)//�˳���
	{
		return CODEC_NO_ERROR;
	}
	
	
	u16 CKdvAdapterGroup::AddVideoChannel(const TAdapterChannel * ptAdpChannel) //���ͼ��ͨ��
	{
		return CODEC_NO_ERROR;
	}
	
	u16 CKdvAdapterGroup::RemoveVideoChannel(u8 byChnNo) //ɾ��ͼ��ͨ��
	{
		return CODEC_NO_ERROR;
	}
	
	u16 CKdvAdapterGroup::AddAudioChannel(const TAdapterChannel * ptAdpChannel) //�������ͨ��
	{
		return CODEC_NO_ERROR;
	}
	
	u16 CKdvAdapterGroup::RemoveAudioChannel(u8 byChnNo) //ɾ������ͨ��
	{
		return CODEC_NO_ERROR;
	}
	
    u16 CKdvAdapterGroup::GetGroupStatus(TAdapterGroupStatus &tAdapterGroupStatus) //�õ����״̬
	{
		return CODEC_NO_ERROR;
	}
	
	u16 CKdvAdapterGroup::GetAudioChannelStatis(u8 byChnNo, TAdapterChannelStatis &tAdapterChannelStatis) //�õ���Ƶͨ����ͳ����Ϣ
	{
		return CODEC_NO_ERROR;
	}
	
	u16 CKdvAdapterGroup::GetVideoChannelStatis(u8 byChnNo, TAdapterChannelStatis &tAdapterChannelStatis) //�õ���Ƶͨ����ͳ����Ϣ
	{
		return CODEC_NO_ERROR;
	}
	
	u16 CKdvAdapterGroup::ChangeVideoEncParam(u8 byChnNo, TVideoEncParam  * ptVidEncParam)     //�ı���Ƶ����������
	{
		return CODEC_NO_ERROR;
	}
	
	u16 CKdvAdapterGroup::ChangeAudioEncParam(u8 byChnNo, u8 byMediaType, TAdapterAudioEncParam * ptAudEncParam)     //�ı���Ƶ���������� 
	{
		return CODEC_NO_ERROR;
	}
		
	u16 CKdvAdapterGroup::ChangeVideoDecParam(u8 byChnNo, TVideoDecParam  * ptVidDecParam)     //�ı���Ƶ����������
	{
		return CODEC_NO_ERROR;
	}
	
	void  CKdvAdapterGroup::ShowChnInfo(u32 chnNo, BOOL32 bVidChn)
	{
	}

	/*����ͼ�����������ش�����*/
	u16  CKdvAdapterGroup::SetNetRecvFeedbackVideoParam(TNetRSParam tNetRSParam, BOOL32 bRepeatSnd)
	{
			return CODEC_NO_ERROR;
	}
	/*����ͼ������緢���ش�����*/
	u16 CKdvAdapterGroup::SetNetSendFeedbackVideoParam(u16 wBufTimeSpan, BOOL32 bRepeatSnd )
	{
		return CODEC_NO_ERROR;
	}

    //������Ƶ�����غ�PTֵ
	u16  CKdvAdapterGroup::SetAudEncryptPT(u32 dwChnNo, u8 byEncryptPT)
	{
		return CODEC_NO_ERROR;
	}
	//������Ƶ�������key�ִ��Լ� ����ģʽ Aes or Des
	u16  CKdvAdapterGroup::SetAudEncryptKey(u32 dwChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode )
	{
		return CODEC_NO_ERROR;
	}

	//������Ƶ�����غ�PTֵ
	u16  CKdvAdapterGroup::SetVidEncryptPT(u32 dwChnNo, u8 byEncryptPT)
	{
		return CODEC_NO_ERROR;
	}
	//������Ƶ�������key�ִ��Լ� ����ģʽ Aes or Des
	u16  CKdvAdapterGroup::SetVidEncryptKey(u32 dwChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode )
	{
		return CODEC_NO_ERROR;
	} 
	
	//���� H.263+/H.264 �ȶ�̬��Ƶ�غɵ� Playloadֵ
	u16  CKdvAdapterGroup::SetVideoActivePT(u32 dwChnNo, u8 byRmtActivePT, u8 byRealPT )
	{
		return CODEC_NO_ERROR;
	}
	//���� ��Ƶ����key�ִ� �Լ� ����ģʽ Aes or Des
    u16  CKdvAdapterGroup::SetVidDecryptKey(u32 dwChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode)
	{
		return CODEC_NO_ERROR;
	}
	
	//���� ��̬��Ƶ�غɵ� Playloadֵ
	u16  CKdvAdapterGroup::SetAudioActivePT(u32 dwChnNo, u8 byRmtActivePT, u8 byRealPT )
	{
		return CODEC_NO_ERROR;
	}
	//���� ��Ƶ����key�ִ� �Լ� ����ģʽ Aes or Des
    u16  CKdvAdapterGroup::SetAudDecryptKey(u32 dwChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode)
	{
		return CODEC_NO_ERROR;
	}
    u16 CKdvAdapterGroup::SetFastUpdata(void)
    {
        return CODEC_NO_ERROR;
    }
    


#ifndef TOSFUNC
#define TOSFUNC
//����ý��TOSֵ nType����0:ȫ�� 1:��Ƶ 2:��Ƶ 3: ����
API int SetMediaTOS(int nTOS, int nType);
API int GetMediaTOS(int nType);
#endif

/*
	//������Ƶ�������key�ִ������ܵ��غ�PTֵ �Լ� ����ģʽ Aes or Des
	u16  CKdvAdapterGroup::SetAudEncryptKey( s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode )
	{
		return CODEC_NO_ERROR;
	}
	//������Ƶ�������key�ִ������ܵ��غ�PTֵ �Լ� ����ģʽ Aes or Des
	u16  CKdvAdapterGroup::SetVidEncryptKey( s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode )
	{
		return CODEC_NO_ERROR;
	}
	
	//���� H.263+/H.264 �ȶ�̬��Ƶ�غɵ� Playloadֵ
	u16  CKdvAdapterGroup::SetVideoActivePT( u8 byRmtActivePT, u8 byRealPT )
	{
		return CODEC_NO_ERROR;
	}
	//���� ��Ƶ����key�ִ� �Լ� ����ģʽ Aes or Des
    u16  CKdvAdapterGroup::SetVidDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode)
	{
		return CODEC_NO_ERROR;
	}
	
	//���� ��̬��Ƶ�غɵ� Playloadֵ
	u16  CKdvAdapterGroup::SetAudioActivePT( u8 byRmtActivePT, u8 byRealPT )
	{
		return CODEC_NO_ERROR;
	}
	//���� ��Ƶ����key�ִ� �Լ� ����ģʽ Aes or Des
    u16  CKdvAdapterGroup::SetAudDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode)
	{
		return CODEC_NO_ERROR;
	}

*/

u16 CKdvAdapterGroup::SetVidFecMode(unsigned long,unsigned char)
{
    return 0;
}

u16 CKdvAdapterGroup::SetVidFecEnable(unsigned long,int)
{
    return 0;
}

#else

u16 CKdvAdapterGroup::CreateGroup(u32 bIsVgaGroup)// ����������
{
    return 0;
}

u16 CKdvAdapterGroup::StartGroup()
{
    return 0;
}

u16 CKdvAdapterGroup::StopGroup()
{
    return 0;
}

u16 CKdvAdapterGroup::DestroyGroup()
{
    return 0;
}

u16 CKdvAdapterGroup::AddVideoChannel(const TAdapterChannel * ptAdpChannel, u8& byChnNo)
{
    return 0;
}

u16 CKdvAdapterGroup::RemoveVideoChannel(u8 byChnNo)
{
    return 0;
}

u16 CKdvAdapterGroup::GetGroupStatus(TAdapterGroupStatus &tAdapterGroupStatus)
{
    return 0;
}

u16 CKdvAdapterGroup::GetVideoChannelStatis(u8 byChnNo, TAdapterChannelStatis &tAdapterChannelStatis)
{
    return 0;
}

u16 CKdvAdapterGroup::GetVideoCodecType(u8 byChnNo, u32 dwID, u32& dwType)
{
    return 0;
}

u16 CKdvAdapterGroup::ChangeVideoEncParam(u8 byChnNo, TVideoEncParam  * ptVidEncParam, u32 dwEncNum)
{
    return 0;
}

u16 CKdvAdapterGroup::ChangeVideoDecParam(u8 byChnNo, TVideoDecParam  * ptVidDecParam)
{
    return 0;
}

u16 CKdvAdapterGroup::SetVidDataCallback(u8 byChnNo, u32 dwID, FRAMECALLBACK fVidData, void* pContext) /*������Ƶ���ݻص���������*/
{
    return 0;
}

u16 CKdvAdapterGroup::SetData(u32 chnNo, const TFrameHeader& tFrameInfo)        /*������Ƶ�������ݣ�������*/
{
    return 0;
}

void CKdvAdapterGroup::ShowChnInfo(u32 chnNo, BOOL32 bVidChn)
{
    return;
}

u16 CKdvAdapterGroup::SetFastUpdata(u32 dwID)
{
    return 0;
}

u16 CKdvAdapterGroup::ClearVideo()
{
    return CODEC_NO_ERROR;
}

u16 CKdvAdapterGroup::SetResizeMode(u32 dwMode)
{
	return 0;
}
#endif //_HD_
