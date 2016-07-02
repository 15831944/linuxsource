
#include "kdvtype.h"
#include "osp.h"
#include "kdvmixer.h"




CKdvMixerGroup::CKdvMixerGroup()
{
}
CKdvMixerGroup:: ~CKdvMixerGroup()
{
}
u16 CKdvMixerGroup::StartMixerGroup(u8 bAllMix)
{
	return MIXER_OPERATE_SUCCESS;
}

u16    CKdvMixerGroup::SetMixChnStatus(u8  bChnStatus[192])  //�����ֹ��������ģʽ�£���ͨ���Ƿ�������
{
	return MIXER_OPERATE_SUCCESS;
}

	u16 CKdvMixerGroup::CreateMixerGroup(const TMixerMap* tMixerMap,u8 byChannelNum)//����������
	{
		return MIXER_OPERATE_SUCCESS;
	}
	u16 CKdvMixerGroup::DestroyMixerGroup( )//ɾ��������
	{
		return MIXER_OPERATE_SUCCESS;
	}
	u16 CKdvMixerGroup::StopMixerGroup()//ֹͣ������
	{
		return MIXER_OPERATE_SUCCESS;
	}
	u16 CKdvMixerGroup::AddMixerChannel(const TMixerChannel* tMixerChanel)//������Ļ���ͨ��
	{
		return MIXER_OPERATE_SUCCESS;
	}
	u16 CKdvMixerGroup::DeleteMixerChannel(u8 byChannelIndex)//ɾ����Ļ���ͨ��
	{
		return MIXER_OPERATE_SUCCESS;
	}
    
	u16 CKdvMixerGroup::SetNModeOutput(const TNetAddress* tLocalAddr, const TNetAddress* tDstNetAddr)//��ʼNģʽ�����
	{
		return MIXER_OPERATE_SUCCESS;
	}

    u16 CKdvMixerGroup::SetNModeNetFeedbackParam(BOOL32 bRepeatSnd, u16 wBufTimeSpan)
    {
        return MIXER_OPERATE_SUCCESS;
    }
    u16 CKdvMixerGroup::SetMixerDelay(u32 dwTime)
    {
        return CODEC_NO_ERROR;
    }
	u16 CKdvMixerGroup::SetMixerNetFeedbackParam(u8 byChnIndex,  BOOL32 bFeedBackSnd, TNetRSParam tNetRSParam, BOOL32 bRepeatSnd, u16 wBufTimeSpan)
    {
        return MIXER_OPERATE_SUCCESS;
    }

    u16 CKdvMixerGroup::SetForceAudMix(u8 byChnNo) // ǿ�ƻ���
	{
		return MIXER_OPERATE_SUCCESS;
	}
    u16 CKdvMixerGroup::CancelForceAudMix() // ȡ��ǿ��
	{
		return MIXER_OPERATE_SUCCESS;
	}
    u16 CKdvMixerGroup::SetMixerDepth(u8 byDepth)//���û������
	{
		return MIXER_OPERATE_SUCCESS;
	}
    u16 CKdvMixerGroup::SetMixerCallBack(TMixerCallBack tMixerCallBack, int dwContext)//���������ص�
	{
		return MIXER_OPERATE_SUCCESS;
	}
    u16 CKdvMixerGroup::GetActiveChns(u8 abyActiveChnsArr[], u8 byArrNum, u8 *pbyActiveNum) //��õ�ǰ���������ͨ��
	{
		return MIXER_OPERATE_SUCCESS;
	}
	
	u16 CKdvMixerGroup::SetMixerChannelVolume(u8 byChannelIndex,u8 byVolume)//����ͨ������
	{
		return MIXER_OPERATE_SUCCESS;
	}
	u16 CKdvMixerGroup::GetMixerGroupStatus(TMixerGroupStatus &tMixerGroupStatus)//�õ����״̬
	{
		return MIXER_OPERATE_SUCCESS;
	}
	u16 CKdvMixerGroup::GetMixerChannelStatis(u8 byChannelIndex,TMixerChannelStatis &tMixerChannelStatis)//�õ�ͨ����ͳ����Ϣ
	{
		return MIXER_OPERATE_SUCCESS;
	}
	
	u16 CKdvMixerGroup::SetMixerParam(TMixerParam  *ptMixPara)//�������ܻ���������
	{
		return MIXER_OPERATE_SUCCESS;
	}

	u16 CKdvMixerGroup::SetMixerRcvBufs(u32 dwStartRcvBufs, u32 dwFastRcvBufs)
	{
		return MIXER_OPERATE_SUCCESS;
	}
	
	u16 CKdvMixerGroup::StopMixerSnd(BOOL32 bStopSnd)	//TRUE: ֹͣ���з���, FALSE: ��ʼ���з���
	{
		return MIXER_OPERATE_SUCCESS;
	}
	u16 CKdvMixerGroup::SetExciteKeepTime(u32 dwKeepTime)  //����������������ʱ����, ��λ����
	{
		return MIXER_OPERATE_SUCCESS;
	}
	
	void CKdvMixerGroup::MixerCallBackProc(u8 *pbyMediaMsg, u32 dwMediaMsgLen)
	{
	}
	
	u16 CKdvMixerGroup::SendMsgToMap(u32 dwCciQueNo, u8 *pbyMsgHead, u32 dwMsgHeadLen, u8 *pbyMsgBody,
		u32 dwMsgBodyLen)		//������
	{
		return MIXER_OPERATE_SUCCESS;
	}
	
	u16 CKdvMixerGroup::H2MCmdSend(u32 type, u32 event, u32 channel, u8 *msg, u32 msgLen) //������
	{
		return MIXER_OPERATE_SUCCESS;
	}
    //������Ƶ�����غ�PTֵ
	u16  CKdvMixerGroup::SetAudEncryptPT(u8 byChnNo, u8 byEncryptPT)
	{
		return MIXER_OPERATE_SUCCESS;
	}
	//������Ƶ�������key�ִ��Լ� ����ģʽ Aes or Des
	u16  CKdvMixerGroup::SetAudEncryptKey(u8 byChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode )
	{
		return MIXER_OPERATE_SUCCESS;
	}
	
	
	//���� ��̬��Ƶ�غɵ� Playloadֵ
	u16  CKdvMixerGroup::SetAudioActivePT(u8 byChnNo, u8 byRmtActivePT, u8 byRealPT )
	{
		return MIXER_OPERATE_SUCCESS;
	}
	//���� ��Ƶ����key�ִ� �Լ� ����ģʽ Aes or Des
    u16  CKdvMixerGroup::SetAudDecryptKey(u8 byChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode)
	{
		return MIXER_OPERATE_SUCCESS;
	}
/*
	//������Ƶ�������key�ִ������ܵ��غ�PTֵ �Լ� ����ģʽ Aes or Des
	u16  CKdvMixerGroup::SetAudEncryptKey( s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptPT, u8 byEncryptMode )
	{
		return MIXER_OPERATE_SUCCESS;
	}
	//���� ��̬��Ƶ�غɵ� Playloadֵ
	u16  CKdvMixerGroup::SetAudioActivePT( u8 byRmtActivePT, u8 byRealPT )
	{
		return MIXER_OPERATE_SUCCESS;
	}
	//���� ��Ƶ����key�ִ� �Լ� ����ģʽ Aes or Des
    u16  CKdvMixerGroup::SetAudDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode)
	{
		return MIXER_OPERATE_SUCCESS;
	}
*/

    u16 CKdvMixerGroup::SetNetAudioResend(u8 byChnNo, s32 nSendTimes, s32 nSendSpan)
    {
        return MIXER_OPERATE_SUCCESS;         
    }