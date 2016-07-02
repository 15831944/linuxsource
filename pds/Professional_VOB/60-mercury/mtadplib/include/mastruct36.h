/*****************************************************************************
   ģ����      : MtAdp
   �ļ���      : mastruct36.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: 3.6�汾���ݽṹ���� (ͬ����12��21�յ�3.6�汾)
   ����        : ������
   �汾        : V4.0  Copyright(C) 2005-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2005/12     4.0         ������          ����(Ŀǰֻͬ���ն�״̬�������Ϣ�ṹ)
******************************************************************************/

#ifndef _MASTRUCT36_H
#define _MASTRUCT36_H

#include "kdvtype.h"

#ifdef WIN32
#pragma pack( push )
#pragma pack( 1 )
#define PACKED 
#else
#define PACKED __attribute__((__packed__))	
#endif

//ý�����Ͷ���
#define   MEDIA_TYPE_NULL_VER36                    255  //ý������Ϊ��

//��Ƶ
#define	  MEDIA_TYPE_MP3_VER36	                    96  //mp3 mode 0-4
#define	  MEDIA_TYPE_MP2_VER36	                    98  //mp3 mode 0-4
#define   MEDIA_TYPE_PCMA_VER36						8   //G.711 Alaw  mode 5
#define   MEDIA_TYPE_PCMU_VER36						0   //G.711 ulaw  mode 6
#define   MEDIA_TYPE_G721_VER36						2   //G.721
#define   MEDIA_TYPE_G722_VER36						9   //G.722
#define	  MEDIA_TYPE_G7231_VER36		            4   //G.7231
#define	  MEDIA_TYPE_G728_VER36						15  //G.728
#define	  MEDIA_TYPE_G729_VER36						18  //G.729
#define   MEDIA_TYPE_G7221_VER36					13  //G.7221

//��Ƶ
#define   MEDIA_TYPE_MP4_VER36	                    97  //MPEG-4
#define   MEDIA_TYPE_H261_VER36						31  //H.261
#define   MEDIA_TYPE_H262_VER36						33  //H.262 (MPEG-2)
#define   MEDIA_TYPE_H263_VER36						34  //H.263
#define   MEDIA_TYPE_H263PLUS_VER36					101 //H.263+
#define   MEDIA_TYPE_H264_VER36						106 //H.264

//����
#define   MEDIA_TYPE_H224_VER36						100  //H.224 Payload �ݶ�100
#define   MEDIA_TYPE_T120_VER36                     200  //T.120ý������
#define   MEDIA_TYPE_H239_VER36                     239  //H.239�������� 
#define   MEDIA_TYPE_MMCU_VER36                     120  //��������ͨ�� 

//kdvԼ���ı��ط���ʱʹ�õĻý������ 
#define   ACTIVE_TYPE_PCMA_VER36		            110   //G.711 Alaw  mode 5
#define   ACTIVE_TYPE_PCMU_VER36		            111   //G.711 ulaw  mode 6
#define   ACTIVE_TYPE_G721_VER36		            112   //G.721
#define   ACTIVE_TYPE_G722_VER36		            113   //G.722
#define	  ACTIVE_TYPE_G7231_VER36		            114   //G.7231
#define	  ACTIVE_TYPE_G728_VER36		            115   //G.728
#define	  ACTIVE_TYPE_G729_VER36		            116   //G.729
#define   ACTIVE_TYPE_G7221_VER36                   117   //G.7221
#define   ACTIVE_TYPE_H261_VER36	                118   //H.261
#define   ACTIVE_TYPE_H262_VER36	                119   //H.262 (MPEG-2)
#define   ACTIVE_TYPE_H263_VER36	                120   //H.263

//�豸���ඨ��
#define		TYPE_MCU_VER36							1   //MCU
#define		TYPE_MCUPERI_VER36						2   //MCU����
#define		TYPE_MT_VER36							3   //�ն�

//Զң
#define		CAMERA_MAX_NUM_VER36					6	 //����������Ŀ

//����id����
#define     MAXLEN_CONFID_VER36						16

//����ͼ����������ģʽ
#define		MODE_NONE_VER36			                0	 //����Ƶ������
#define		MODE_VIDEO_VER36					    1	 //ֻ����Ƶ
#define		MODE_AUDIO_VER36					    2	 //ֻ����Ƶ
#define		MODE_BOTH_VER36					        3   //����Ƶ����
#define     MODE_DATA_VER36                         4   //���� 
#define     MODE_SECVIDEO_VER36                     5   //�ڶ�·��Ƶ

//��Ƶ��ʽ����
#define		VIDEO_FORMAT_SQCIF_VER36			    1   //SQCIF(128*96)
#define		VIDEO_FORMAT_QCIF_VER36				    2   //SCIF(176*144)
#define		VIDEO_FORMAT_CIF_VER36                  3   //CIF(352*288)
#define		VIDEO_FORMAT_4CIF_VER36                 4   //4CIF(704*576)
#define		VIDEO_FORMAT_16CIF_VER36			    5   //16CIF(1408*1152)
#define		VIDEO_FORMAT_AUTO_VER36				    6   //����Ӧ��������MPEG4
#define		VIDEO_FORMAT_2CIF_VER36				    7   //2CIF(352*576)

//��Ƶ˫�����Ͷ���
#define		VIDEO_DSTREAM_H263PLUS_VER36		    0   //H263��
#define		VIDEO_DSTREAM_MAIN_VER36			    1   //������Ƶ��ʽһ��
#define		VIDEO_DSTREAM_H263PLUS_H239_VER36       2   //H263����H239
#define		VIDEO_DSTREAM_H263_H239_VER36           3   //H263��H239
#define		VIDEO_DSTREAM_H264_H239_VER36           4   //H264��H239

//���鿪�ŷ�ʽ����
#define		CONF_OPENMODE_CLOSED_VER36              0  //������,�ܾ��б�������ն�
#define		CONF_OPENMODE_NEEDPWD_VER36             1  //�����������
#define		CONF_OPENMODE_OPEN_VER36                2  //��ȫ���� 

//������ܷ�ʽ����
#define     CONF_ENCRYPTMODE_NONE_VER36             0 //������
#define     CONF_ENCRYPTMODE_DES_VER36              1 //des����
#define     CONF_ENCRYPTMODE_AES_VER36              2 //aes����

//��������ת��ʱ�Ƿ�֧�ֹ�һ������ʽ����, �붪���ش���ʽ�Լ����ܷ�ʽ����
#define     CONF_UNIFORMMODE_NONE_VER36             0 //����һ������
#define     CONF_UNIFORMMODE_VALID_VER36            1 //  ��һ������
#define     SWITCHCHANNEL_UNIFORMMODE_NONE_VER36    0 //����һ������
#define     SWITCHCHANNEL_UNIFORMMODE_VALID_VER36   1 //  ��һ������

//���ݻ��鷽ʽ����
#define		CONF_DATAMODE_VAONLY_VER36              0  //���������ݵ�����Ƶ����
#define		CONF_DATAMODE_VAANDDATA_VER36           1  //�������ݵ�����Ƶ����
#define		CONF_DATAMODE_DATAONLY_VER36            2  //����������Ƶ�����ݻ���

//���������ʽ����
#define     CONF_RELEASEMODE_NONE_VER36             0  //�����Զ�����
#define     CONF_RELEASEMODE_NOMT_VER36             1  //���ն�ʱ�Զ�����

//������з�ʽ
#define     CONF_TAKEMODE_SCHEDULED_VER36           0  //ԤԼ����
#define     CONF_TAKEMODE_ONGOING_VER36             1  //��ʱ����
#define     CONF_TAKEMODE_TEMPLATE_VER36            2  //����ģ��

//���鱣����ʽ
#define		CONF_LOCKMODE_NONE_VER36                0  //δ����
#define		CONF_LOCKMODE_NEEDPWD_VER36             1  //�����������
#define		CONF_LOCKMODE_LOCK_VER36                2  //���� 

//�����˵�Դ�Ķ���
#define		CONF_SPEAKERSRC_SELF_VER36              0  //���Լ�
#define		CONF_SPEAKERSRC_CHAIR_VER36             1  //����ϯ
#define		CONF_SPEAKERSRC_LAST_VER36              2  //����һ�η�����

//�����ն˷�ʽ����
#define		CONF_CALLMODE_NONE_VER36                0  //������
#define		CONF_CALLMODE_ONCE_VER36                1  //����һ��
#define		CONF_CALLMODE_TIMER_VER36               2  //��ʱ����

//��ʱ�����ն�ʱ��������
#define		DEFAULT_CONF_CALLINTERVAL_VER36         20 //ȱʡ�Ķ�ʱ���м�����룩
#define		MIN_CONF_CALLINTERVAL_VER36             10 //��С�Ķ�ʱ���м�����룩

//��ʱ�����ն˺��д�������
#define		DEFAULT_CONF_CALLTIMES_VER36            0  //����ζ�ʱ���д���
#define		MIN_CONF_CALLTIMES_VER36                2  //��С�Ķ�ʱ���д���

//����ϳɷ�ʽ����
#define		CONF_VMPMODE_NONE_VER36                 0  //�����л���ϳ�
#define		CONF_VMPMODE_CTRL_VER36                 1  //��ػ���ϯѡ���Ա�ϳ�
#define		CONF_VMPMODE_AUTO_VER36                 2  //MCU�Զ�ѡ���Ա�ϳ�

//����¼��ʽ���� 
#define		CONF_RECMODE_NONE_VER36                 0  //��¼��
#define		CONF_RECMODE_REC_VER36                  1  //¼��
#define		CONF_RECMODE_PAUSE_VER36                2  //��ͣ

//�������ʽ����
#define		CONF_PLAYMODE_NONE_VER36                0  //������
#define		CONF_PLAYMODE_PLAY_VER36                1  //����
#define		CONF_PLAYMODE_PAUSE_VER36               2  //��ͣ
#define		CONF_PLAYMODE_FF_VER36                  3  //���
#define		CONF_PLAYMODE_FB_VER36                  4  //����

//������ѯ��ʽ����
#define		CONF_POLLMODE_NONE_VER36                0  //����ѯ�㲥 
#define		CONF_POLLMODE_VIDEO_VER36               1  //��ͼ����ѯ�㲥
#define		CONF_POLLMODE_SPEAKER_VER36             2  //��������

//�������䷽ʽ����
#define		CONF_BASMODE_NONE_VER36                 0  //�������������� 
#define		CONF_BASMODE_RTP_VER36                  1  //RTPͷ��������
#define		CONF_BASMODE_BR_VER36                   2  //��������
#define		CONF_BASMODE_MDTP_VER36                 3	//ý����������

//����ϳɳ�Ա���Ͷ���
#define		VMP_MEMBERTYPE_MCSSPEC_VER36            1  //���ָ�� 
#define		VMP_MEMBERTYPE_SPEAKER_VER36            2  //�����˸���
#define		VMP_MEMBERTYPE_CHAIRMAN_VER36           3  //��ϯ����
#define		VMP_MEMBERTYPE_POLL_VER36               4  //��ѯ��Ƶ����
#define		VMP_MEMBERTYPE_VAC_VER36                5	//��������(��ز�Ҫ�ô�����)

//����ϳɷ����
#define		VMP_STYLE_DYNAMIC_VER36                 0   //��̬����(���Զ��ϳ�ʱ��Ч)
#define		VMP_STYLE_ONE_VER36                     1   //һ����
#define		VMP_STYLE_VTWO_VER36                    2   //�����棺���ҷ� 
#define		VMP_STYLE_HTWO_VER36                    3   //������: һ��һС
#define		VMP_STYLE_THREE_VER36                   4   //������
#define		VMP_STYLE_FOUR_VER36                    5   //�Ļ���
#define		VMP_STYLE_SIX_VER36                     6   //������ 
#define		VMP_STYLE_EIGHT_VER36                   7   //�˻���
#define		VMP_STYLE_NINE_VER36                    8   //�Ż���
#define		VMP_STYLE_TEN_VER36                     9   //ʮ����
#define		VMP_STYLE_THIRTEEN_VER36                10  //ʮ������
#define		VMP_STYLE_SIXTEEN_VER36                 11  //ʮ������
#define		VMP_STYLE_SPECFOUR_VER36                12  //�����Ļ��� 
#define		VMP_STYLE_SEVEN_VER36                   13  //�߻���

//MCU���ú�ҵ��������
#define		MAXLEN_ALIAS_VER36			            128  //MCU���ն˼������127����ĸ��63������
#define     VALIDLEN_ALIAS							16   //��Ч��������
#define		MAXLEN_CONFNAME_VER36					64   //��������󳤶�
#define		MAXLEN_PWD_VER36						32   //������볤��  
 
#define		MAXLEN_EQP_ALIAS						16   //���������󳤶� 
#define		MAXLEN_E164_VER36						16   //���E164���볤�� 
#define     MAXLEN_KEY_VER36						16   //���ļ���key����        

#define     LEN_DES_VER36							8	 //byte
#define     LEN_AES_VER36							16	 //byte
#define     LOCAL_MCUID_VER36						192  //����MCU ID

//���賣������
#define     MAXNUM_MIXER_DEPTH_VER36				10  //���������������
#define     MAXNUM_VMP_MEMBER_VER36					16  //�����ϳɳ�Ա��
#define     DEFAULT_MIXER_VOLUME_VER36			    250	//ȱʡ�Ļ�������

//�ն�״̬
struct _TTERSTATUS_VER36
{
    ///////����״̬///////
    u8		bOnline;					//�Ƿ��ڻ�����	1 yes 0 no
    u8		byConfMode;					//����ģʽ		1 ���� 2 ���		
    u8		byPollStat;					//��ѯ״̬		1 ��ʼ 2 ��ͣ 3 ֹͣ
    u8		byCallMode;					//���з�ʽ		1 GK·�� 0 ֱ�Ӻ���
    u8		bRegGkStat;					//GKע����	1 �ɹ� 0 ʧ��
    u8		byEncryptConf;				//����״̬		1 ���� 0 ������(ֻ�Ǳ�ʾ���˷����Ƿ����)
    
    ///////�豸״̬///////
    u8		byMtBoardType;				//�ն˰忨����	0: WIN,1:8010,2:8010a,3:8018,4:IMT,5:8010C
    u8		bMatrixStatus;				//����״̬		(1 ok 0 err)
    u8		byCamCtrlSrc;				//��ǰ��������ͷԴ����(0:����,1:Զ��)
    u8		byLocalCamCtrlNo;			//��ǰ���Ƶı�������ͷ��(1-6������ͷ)
    u8		byRemoteCamCtrlNo;			//��ǰ���Ƶ�Զ������ͷ��(1-6������ͷ)
    u8		byCamNum;					//����ͷ����	(0~6)
    u8		bCamStatus[CAMERA_MAX_NUM_VER36];	//����ͷ״̬ (1 ok 0 err)
    u8		byVideoSrc;					//��ǰ������ƵԴ(0:S����,1-6:C����)
    u8		bRemoteCtrl;				//�Ƿ�����Զң	1 yes 0 no
    
    //////�����״̬////////
    u8		byAEncFormat;				//��Ƶ�����ʽ(AUDIO_G711A,AUDIO_MP3...)
    u8		byADecFormat;				//��Ƶ�����ʽ(AUDIO_G711A,AUDIO_MP3...)
    
    u8		abyVEncFormat[2];			//��Ƶ�����ʽ(VIDEO_H261,VIDEO_H263...)
    u8		abyVEncRes[2];				//��Ƶ����ֱ���(VIDEO_QCIF,VIDEO_CIF...)
    u16		awSendBitRate[2];			//��Ƶ��������(��λ:kbps)
    u16		awSendAVBitRate[2];			//ƽ����������(��λ:kbps)
    
    u8		abyVDecFormat[2];			//��Ƶ�����ʽ(VIDEO_H261,VIDEO_H263...)
    u8		abyVDecRes[2];				//��Ƶ����ֱ���(VIDEO_QCIF,VIDEO_CIF...)
    u16		awRecvBitRate[2];			//��Ƶ��������(��λ:kbps)
    u16		awRecvAVBitRate[2];			//ƽ����������(��λ:kbps)
    
    u8      abyEncryptMode[2];			//����ʹ�õļ����㷨(ENCRYPT_AES/ENCRYPT_DES/ENCRYPT_NONE)
    u8		abyDecryptMode[2];			//����ʹ�õļ����㷨(ENCRYPT_AES/ENCRYPT_DES/ENCRYPT_NONE)
    
    u8		byEncVol;					//��ǰ��������(��λ:�ȼ�)
    u8		bMute;						//�Ƿ�����			1 yes 0 no
    u8		byDecVol;					//��ǰ��������(��λ:�ȼ�)
    u8		bQuiet;						//�Ƿ���			1 yes 0 no
    u8		bLocalLoop;					//�Ƿ��Ի�			1 yes 0 no
    u8		byAudioPower;				//�Ƿ��ڲ�����Ƶ	1 yes 0 no
    u8		byLocalImageTest;			//�Ƿ񱾵�ͼ�����1 yes 0 no
    u8		byRmtImageTest;				//�Ƿ�Զ��ͼ����� 1 yes 0 no
    u16		wSysSleep;					//����ʱ��(0xFFFF��ʾ������)
    
    //////����״̬///////
    
}PACKED;

//������鷽ʽ�ṹ(�����л�ı�)
struct TConfModeVer36
{
protected:
	u8     m_byTakeMode;        //������з�ʽ: 0-ԤԼ 1-��ʱ 2-����ģ�� 
	u8     m_byLockMode;        //����������ʽ: 0-������,���л�ؿɼ��ɲ��� 1-������������˻��� 2-ĳ���������̨����
    u8     m_byCallMode;        //�����ն˷�ʽ: 0-�������նˣ��ֶ����� 1-����һ�� 2-��ʱ����δ����ն�
	u8     m_byVACMode;         //����������ʽ: 0-�����������������Ʒ���(���ݿ��Ʒ���) 1-���������������Ʒ���
	u8     m_byAudioMixMode;    //������ʽ:     0-������ 1-���ܻ���
	u8     m_byVMPMode;         //ͼ�񸴺Ϸ�ʽ: 0-��ͼ�񸴺� 1-��ػ���ϯ����ͼ�񸴺� 2-�Զ�ͼ�񸴺�(��̬���������ó�Ա)
	u8     m_byRecordMode;      //����¼��ʽ: (BIT:0-6)0-��¼�� 1-¼�� 2-¼����ͣ; BIT7 = 0 ʵʱ 1��֡¼��
	u8     m_byPlayMode;        //�������ʽ: 0-������ 1-���� 2-������ͣ 3-��� 4-����
	u8     m_byBasMode;         //�������䷽ʽ: BIT0-�Ƿ�RTPͷ���� BIT1-�Ƿ��������� BIT2-�Ƿ���������
	u8     m_byPollMode;        //������ѯ��ʽ: 0-����ѯ�㲥 1-��ͼ����ѯ�㲥 2-��������
	u8     m_byDiscussMode;     //��������ģʽ: 0-������ģʽ(�ݽ�ģʽ) 1-����ģʽ
	u8     m_byForceRcvSpeaker; //ǿ���տ�����: 0-��ǿ���տ������� 1-ǿ���տ�������
	u8     m_byNoChairMode;     //������ϯģʽ: 0-��������ϯ��ʽ 1-����ϯ��ʽ
	u8     m_byRegToGK;         //����ע��GK���: 0-����δ��GK��ע�� 1-������GK�ϳɹ�ע��
	u8     m_byMixSpecMt;		//�Ƿ�ָ���ն˻���: 0-����ָ���ն˻���(�����ն˲μӻ���) 1-ָ���ն˻���
	u32    m_dwCallInterval;    //��ʱ����ʱ�ĺ��м����(��λ���룬��СΪ5��)��������
	u32    m_dwCallTimes;       //��ʱ����ʱ�ĺ��д�����������
public:
	TConfModeVer36( void ){ memset( this, 0, sizeof(TConfModeVer36) ); }
    void   SetTakeMode(u8 byTakeMode){ m_byTakeMode = byTakeMode;} 
    u8     GetTakeMode( void ) const { return m_byTakeMode; }
    void   SetLockMode(u8   byLockMode){ m_byLockMode = byLockMode;} 
    u8     GetLockMode( void ) const { return m_byLockMode; }
    void   SetCallMode(u8   byCallMode){ m_byCallMode = byCallMode;} 
    u8     GetCallMode( void ) const { return m_byCallMode; }
	void   SetCallInterval(u32 dwCallInterval){ m_dwCallInterval = htonl(dwCallInterval);} 
    u32    GetCallInterval( void ) const { return ntohl(m_dwCallInterval); }
	void   SetCallTimes(u32 dwCallTimes){ m_dwCallTimes = htonl(dwCallTimes);} 
    u32    GetCallTimes( void ) const { return ntohl(m_dwCallTimes); }
    void   SetVACMode(BOOL32 bVACMode){ m_byVACMode = bVACMode;} 
    BOOL32 IsVACMode( void ) const { return m_byVACMode == 0 ? FALSE : TRUE; }
    void   SetAudioMixMode(BOOL32 bAudioMixMode){ m_byAudioMixMode = bAudioMixMode;} 
    BOOL32 IsAudioMixMode( void ) const { return m_byAudioMixMode == 0 ? FALSE : TRUE; }
    void   SetVMPMode(u8   byVMPMode){ m_byVMPMode = byVMPMode;} 
    u8     GetVMPMode( void ) const { return m_byVMPMode; }
	void   SetRegToGK( BOOL32 bRegToGK ){ m_byRegToGK = bRegToGK; }
	BOOL32 IsRegToGK( void ){ return m_byRegToGK == 0 ? FALSE : TRUE; }
    void   SetRecordMode(u8   byRecordMode)
	{   byRecordMode   &= 0x7f ;
		m_byRecordMode &= 0x80 ;
		m_byRecordMode |= byRecordMode;
	} 
    u8     GetRecordMode( void ) const { return m_byRecordMode & 0x7f; }
	
	//�жϵ�ǰ�Ƿ��ڳ�֡¼�����Ƿ���TRUE���򷵻�FALSE
	BOOL32   IsRecSkipFrame(){ return m_byRecordMode & 0x80 ? TRUE : FALSE;}
	//bSkipFrame = TRUE ���õ�ǰΪ��֡¼��,����ʵʱ¼��
	void   SetRecSkipFrame( BOOL32 bSkipFrame )
	{
		if(bSkipFrame)
		{
			m_byRecordMode |=0x80;
		}
		else 
		{
			m_byRecordMode &=0x7f;
		}
	}
    void   SetPlayMode( u8  byPlayMode ){ m_byPlayMode = byPlayMode;} 
    u8     GetPlayMode( void ) const { return m_byPlayMode; }
    void   SetBasMode( u8  byBasMode, BOOL32 bHasBasMode )
	{ 
		if( bHasBasMode )
		{
			m_byBasMode |= (1<<(byBasMode-1)) ;
		}
		else
		{
			m_byBasMode &= ~(1<<(byBasMode-1));
		}
	} 
    BOOL32   GetBasMode( u8  byBasMode ) const 
	{ 
		return m_byBasMode & (1<<(byBasMode-1));
	}
    void   SetPollMode(u8   byPollMode){ m_byPollMode = byPollMode;} 
    u8     GetPollMode( void ) const { return m_byPollMode; }
    void   SetDiscussMode(u8   byDiscussMode){ m_byDiscussMode = byDiscussMode;} 
    BOOL32 IsDiscussMode( void ) const { return m_byDiscussMode == 0 ? FALSE : TRUE; }  
    void   SetForceRcvSpeaker(u8   byForceRcvSpeaker){ m_byForceRcvSpeaker = byForceRcvSpeaker;} 
    BOOL32 IsForceRcvSpeaker( void ) const { return m_byForceRcvSpeaker == 0 ? FALSE : TRUE; }
    void   SetNoChairMode(u8   byNoChairMode){ m_byNoChairMode = byNoChairMode;} 
    BOOL32 IsNoChairMode( void ) const { return m_byNoChairMode == 0 ? FALSE : TRUE; }
    void   SetMixSpecMt( BOOL32 bMixSpecMt ){ m_byMixSpecMt = bMixSpecMt;} 
    BOOL32 IsMixSpecMt( void ) const { return m_byMixSpecMt == 0 ? FALSE : TRUE; }
	void   Print( void )
	{
		StaticLog("\nConfMode:\n" );
		StaticLog("m_byTakeMode: %d\n", m_byTakeMode);
		StaticLog("m_byLockMode: %d\n", m_byLockMode);
		StaticLog("m_byCallMode: %d\n", m_byCallMode);
		StaticLog("m_dwCallInterval: %d\n", m_dwCallInterval);
		StaticLog("m_dwCallTimes: %d\n", m_dwCallTimes);
		StaticLog("m_byVACMode: %d\n", m_byVACMode);
		StaticLog("m_byAudioMixMode: %d\n", m_byAudioMixMode);
		StaticLog("m_byVMPMode: %d\n", m_byVMPMode);
		StaticLog("m_byRecordMode: %d\n", m_byRecordMode);
		StaticLog("m_byPlayMode: %d\n", m_byPlayMode);
		StaticLog("m_byBasMode: %d\n", m_byBasMode);
		StaticLog("m_byPollMode: %d\n", m_byPollMode);
		StaticLog("m_byDiscussMode: %d\n", m_byDiscussMode);
		StaticLog("m_byForceRcvSpeaker: %d\n", m_byForceRcvSpeaker);
		StaticLog("m_byNoChairMode: %d\n", m_byNoChairMode);
		StaticLog("m_byRegToGK: %d\n", m_byRegToGK);
		StaticLog("m_byMixSpecMt: %d\n", m_byMixSpecMt);
	}
}PACKED;

//��¼¼���������(��λ:��)
struct  TRecProgVer36
{
protected:
    u32   		m_dwCurProg;	//��ǰ����
    u32   		m_dwTotalTime;	//�ܳ��ȣ����ڷ���ʱ��Ч
    
public:
    //���õ�ǰ����λ�ã���λ���룩
    void SetCurProg( u32    dwCurProg )	{ m_dwCurProg = htonl( dwCurProg ); }
    //��ȡ��ǰ����λ�ã���λ���룩
    u32    GetCurProg( void ) const	{ return ntohl( m_dwCurProg ); }
    //�����ܳ��ȣ����Է�����Ч����λ���룩
    void SetTotalTime( u32    dwTotalTime )	{ m_dwTotalTime = htonl( dwTotalTime ); }
    //��ȡ�ܳ��ȣ����Է�����Ч����λ���룩
    u32    GetTotalTime( void ) const	{ return ntohl( m_dwTotalTime ); }

}PACKED;

//�����MCU���ն˻�����ṹ (len:6)
struct TMtVer36
{    
protected:
    u8          m_byMainType;   //�豸����
    u8  		m_bySubType;	//�豸����
    u8  		m_byMcuId;		//MCU��
    u8  		m_byEqpId;	    //�豸��	
    u8          m_byConfDriId;  //DRI���
    u8          m_byConfIdx;    //���������ţ�������ʵ���� 1 - MAXNUM_MCU_CONF
    
public:
    //�����������ͺ���
    void   SetType( u8   byType ){ m_byMainType = byType; }         //���ýṹ����
    u8     GetType( void ) const { return m_byMainType; }           //�õ��ṹ����
    void   SetMcuId( u8   byMcuId ){ m_byMcuId = byMcuId; }         //����McuId
    u8     GetMcuId( void ) const { return m_byMcuId; }             //�õ�McuId
    BOOL32   operator ==( const TMtVer36 & tObj ) const;                  //�ж��Ƿ����
    
    //����MCU����
    void   SetMcu( u8   byMcuId );                         //����MCU
    void   SetMcuType( u8  byMcuType ){ m_byMainType = TYPE_MCU_VER36; m_bySubType = byMcuType; }
    u8     GetMcuType( void ) const { return m_bySubType; }
    
    //�������躯��
    void   SetMcuEqp( u8   byMcuId, u8   byEqpId, u8   byEqpType );            //����MCU����
    void   SetEqpType( u8   byEqpType ){ m_byMainType = TYPE_MCUPERI_VER36; m_bySubType = byEqpType; }                                       //����MCU��������
    u8     GetEqpType( void ) const { return m_bySubType; }
    void   SetEqpId( u8   byEqpId ){ m_byMainType = TYPE_MCUPERI_VER36; m_byEqpId = byEqpId; }
    u8     GetEqpId( void ) const { return m_byEqpId; }
    
    //�����ն˺���
    void   SetMt( u8   byMcuId, u8   byMtId, u8   m_byDriId = 0, u8   m_byConfIdx = 0);
    void   SetMt( TMtVer36 tMt);
    void   SetMtId( u8   byMtId ){ m_byMainType = TYPE_MT_VER36; m_byEqpId = byMtId; }
    u8     GetMtId( void ) const { return m_byEqpId; }//�����ն�
    void   SetMtType( u8   byMtType ) { m_byMainType = TYPE_MT_VER36; m_bySubType = byMtType; }
    u8     GetMtType( void ) const { return m_bySubType; }  	
    
	void   SetDriId(u8   byDriId){ m_byConfDriId = byDriId; }
    u8     GetDriId( void ) const { return m_byConfDriId; }
    void   SetConfIdx(u8   byConfIdx){ m_byConfIdx = byConfIdx; } 
    u8     GetConfIdx( void ) const { return m_byConfIdx; }
    void   SetNull( void ){ m_byMcuId = 0; m_byEqpId = 0; m_byConfDriId = 0; }
    BOOL32 IsNull( void ){ return m_byMcuId == 0 && m_byEqpId == 0; }
    BOOL32 IsLocal() const {	return (m_byMcuId ==LOCAL_MCUID_VER36 );	}

}PACKED;

//�ն���ѯ���� (len:8)
struct TMtPollParamVer36: public TMtVer36
{
protected:
    u16   m_wKeepTime;        //�ն���ѯ�ı���ʱ�� ��λ:��(s)
public:
    void  SetKeepTime(u16  wKeepTime){ m_wKeepTime = htons(wKeepTime);} 
    u16   GetKeepTime( void ) const { return ntohs(m_wKeepTime); }
    void  SetTMt( TMtVer36 tMt ){ memcpy( this, &tMt, sizeof(tMt) ); } 
	TMtVer36 GetTMt( void ) const { return *(TMtVer36 *)this; }
}PACKED;

//��ѯ��Ϣ
struct TPollInfoVer36
{
protected:
	u8   m_byMediaMode;          //��ѯģʽ MODE_VIDEO ͼ��  MODE_BOTH ����ͼ��
    u8   m_byPollState;          //��ѯ״̬���μ���ѯ״̬����
	TMtPollParamVer36 m_tMtPollParam; //��ǰ����ѯ�㲥���ն˼������
public:
    void   SetMediaMode(u8   byMediaMode){ m_byMediaMode = byMediaMode;} 
    u8     GetMediaMode(){ return m_byMediaMode; }
	void   SetPollState(u8   byPollState){ m_byPollState = byPollState;} 
    u8     GetPollState( void ) const { return m_byPollState; }
    void   SetMtPollParam(TMtPollParamVer36 tMtPollParam){ m_tMtPollParam = tMtPollParam;} 
    TMtPollParamVer36  GetMtPollParam(){ return m_tMtPollParam; }
	void Print( void )
	{
		StaticLog( "\nPollInfo:\n" );
		StaticLog( "m_byMediaMode: %d\n", m_byMediaMode );
		StaticLog( "m_byPollState: %d\n", m_byPollState );
		StaticLog( "CurPollMtId: %d\n", m_tMtPollParam.GetMtId() );
	}
}PACKED;

//�����ն˱���ö������
typedef enum
{
	mtAliasTypeBegin_ver36 = 0,
	mtAliasTypeTransportAddress_ver36 = 1,  //�ն˱�������: �����ַ
	mtAliasTypeE164_ver36,					//�ն˱�������: E164����
	mtAliasTypeH323ID_ver36,				//�ն˱�������: H323 ID
	mtAliasTypeH320ID_ver36,				//�ն˱�������: H320 ID����
	mtAliasTypeOthers_ver36					//�ն˱�������: ����
} mtAliasType_ver36;

//���崫���ַ�ṹ(len:6)
struct TTransportAddrVer36
{
public:
    u32   		m_dwIpAddr;		//IP��ַ, ������
    u16 		m_wPort;		//�˿ں�, ������
public:
    void   SetNetSeqIpAddr(u32    dwIpAddr){ m_dwIpAddr = dwIpAddr; } 
    u32    GetNetSeqIpAddr( void ) const { return m_dwIpAddr; }
    void   SetIpAddr(u32    dwIpAddr){ m_dwIpAddr = htonl(dwIpAddr); } 
    u32    GetIpAddr( void ) const { return ntohl(m_dwIpAddr); }
    void   SetPort(u16  wPort){ m_wPort = htons(wPort); } 
    u16    GetPort( void ) const { return ntohs(m_wPort); }

}PACKED;

//�����ն˱����ṹ(len:135)
struct TMtAliasVer36
{
public:
    u8				    m_AliasType;					//��������
    s8			        m_achAlias[MAXLEN_ALIAS_VER36];	//�����ַ���
    TTransportAddrVer36	m_tTransportAddr;				//�����ַ
public:
	TMtAliasVer36( void ){ memset( this, 0, sizeof(TMtAliasVer36) ); }
	BOOL32 operator ==( const TMtAliasVer36 & tObj ) const;
	BOOL32 IsNull( void ){ if( m_AliasType == 0 )return TRUE; return FALSE; }
	void   SetNull( void ){ memset(this, 0, sizeof(TMtAliasVer36)); }
	void   SetE164Alias( LPCSTR lpzAlias )
	{
		if( lpzAlias != NULL )
		{
			m_AliasType = mtAliasTypeE164_ver36;
			memcpy( m_achAlias, lpzAlias, MAXLEN_ALIAS_VER36 );
			m_achAlias[MAXLEN_ALIAS_VER36-1] = '\0';
		}
	}
	void SetH323Alias( LPCSTR lpzAlias )
	{
		if( lpzAlias != NULL )
		{
			m_AliasType = mtAliasTypeH323ID_ver36;
			memcpy( m_achAlias, lpzAlias, MAXLEN_ALIAS_VER36 );
			m_achAlias[MAXLEN_ALIAS_VER36-1] = '\0';
		}
	}
	void SetH320Alias( u8 byLayer, u8 bySlot, u8 byChannel )
	{
		m_AliasType = mtAliasTypeH320ID_ver36;
		sprintf( m_achAlias, "��%d-��%d-ͨ��%d%c", byLayer, bySlot, byChannel, 0 );
	}
	BOOL32 GetH320Alias( u8 &byLayer, u8 &bySlot, u8 &byChannel )
	{
		BOOL32 bRet = FALSE;
		byLayer   = 0;
		bySlot    = 0; 
		byChannel = 0; 
		if( mtAliasTypeH320ID_ver36 != m_AliasType )
		{
			return bRet;
		}

		s8 *pachLayerPrefix   = "��";
		s8 *pachSlotPrefix    = "-��";
		s8 *pachChannelPrefix = "-ͨ��";
		s8 *pachAlias = m_achAlias;

		if( 0 == memcmp( pachAlias, pachLayerPrefix, strlen("��") ) )
		{
			s32  nPos = 0;
			s32  nMaxCHLen = 3; //һ�ֽڵ�����ִ�����
			s8 achLayer[4] = {0};

			//��ȡ���
			pachAlias += strlen("��");
			for( nPos = 0; nPos < nMaxCHLen; nPos++ )
			{
				if( '-' == pachAlias[nPos] )
				{
					break;
				}
				achLayer[nPos] = pachAlias[nPos];
			}
			if( 0 == nPos || nPos >= nMaxCHLen )
			{
				return bRet;
			}
			achLayer[nPos+1] = '\0';
			byLayer = atoi(achLayer);

			//��ȡ�ۺ�
			pachAlias += nPos;
			if( 0 == memcmp( pachAlias, pachSlotPrefix, strlen("-��") ) )
			{
				pachAlias += strlen("-��");
				for( nPos = 0; nPos < nMaxCHLen; nPos++ )
				{
					if( '-' == pachAlias[nPos] )
					{
						break;
					}
					achLayer[nPos] = pachAlias[nPos];
				}
				if( 0 == nPos || nPos >= nMaxCHLen )
				{
					return bRet;
				}
				achLayer[nPos+1] = '\0';
				bySlot = atoi(achLayer);
			
				//��ȡͨ����
				pachAlias += nPos;
				if( 0 == memcmp( pachAlias, pachChannelPrefix, strlen("-ͨ��") ) )
				{
					pachAlias += strlen("-ͨ��");
					memcpy( achLayer, pachAlias, nMaxCHLen );
					achLayer[nMaxCHLen] = '\0';
					byChannel = atoi(achLayer);
					bRet = TRUE;
				}
			}
		}
		return bRet;
	}
	void Print( void )
	{
		if( m_AliasType == 0 )
		{
			StaticLog( "null aliase!" );
		}
		else if( m_AliasType == mtAliasTypeTransportAddress_ver36 )
		{
			StaticLog( "IP:%u.%u.%u.%u, port:%d", 
				     QUADADDR(m_tTransportAddr.GetIpAddr()), m_tTransportAddr.GetPort() );
		}		
		else if( m_AliasType == mtAliasTypeE164_ver36 )
		{
			StaticLog( "E164: %s", m_achAlias ); 
		}
		else if( m_AliasType == mtAliasTypeH323ID_ver36 )
		{
			StaticLog( "H323ID: %s", m_achAlias ); 
		}
		else if( m_AliasType == mtAliasTypeH320ID_ver36 )
		{
			StaticLog( "H320ID: %s", m_achAlias ); 
		}
		else
		{
			StaticLog( "Other type aliase!" ); 
		}
		StaticLog( "\n" );
	}
}PACKED;

//�������ģʽ
typedef enum
{
    mcuNoMix_VER36 = 0,
    mcuWholeMix_VER36,
    mcuPartMix_VER36

} emMcuMixMode_VER36;

//�������۲����ṹ
struct TDiscussParamVer36
{
public:
    u8        m_byMixMode;       //����ģʽ�����ƻ�����ȫ�������������
    u8        m_byMemberNum;     //�μ����۵ĳ�Ա����(�������)
    TMtVer36  m_atMtMember[MAXNUM_MIXER_DEPTH_VER36];  //���۳�Ա
public:
    TDiscussParamVer36( void )
    { 
        memset( this, 0, sizeof(TDiscussParamVer36) );
    }
    BOOL32    IsMtInMember( TMtVer36 tMt )
    {
        u8 byLoop = 0;
        u8 byLoopCtrl = 0;
        while( byLoop < m_byMemberNum && byLoopCtrl < MAXNUM_MIXER_DEPTH_VER36 )
        {
            if( tMt == m_atMtMember[byLoop] )
            {
                return TRUE;
            }
            
            byLoop++;
            byLoopCtrl++;
        }
        
        return FALSE;
    }
}PACKED;

//���廭��ϳɳ�Ա�ṹ (len:7)
struct TVMPMemberVer36: public TMtVer36
{
protected:
    u8   m_byMemberType;     //����ϳɳ�Ա����,�μ�mcuconst.h�л���ϳɳ�Ա���Ͷ���
public:
    void  SetMemberType(u8   byMemberType){ m_byMemberType = byMemberType;} 
    u8    GetMemberType( void ) const { return m_byMemberType; }
    void  SetMemberTMt( TMtVer36 tMt ){ memcpy( this, &tMt, sizeof(TMtVer36) ); }
    
}PACKED;

//���廭��ϳɲ����ṹ (len:121)
struct TVMPParamVer36
{
protected:
	u8      m_byVMPAuto;      //�Ƿ����Զ�����ϳ� 0-�� 1-��
	u8      m_byVMPBrdst;     //�ϳ�ͼ���Ƿ����ն˹㲥 0-�� 1-�� 
	u8      m_byVMPStyle;     //����ϳɷ��,�μ�mcuconst.h�л���ϳɷ����
	TVMPMemberVer36  m_atVMPMember[MAXNUM_VMP_MEMBER_VER36]; //����ϳɳ�Ա
public:
    void   SetVMPAuto(u8   byVMPAuto){ m_byVMPAuto = byVMPAuto;} 
    BOOL32 IsVMPAuto( void ) const { return m_byVMPAuto == 0 ? FALSE : TRUE; }
    void   SetVMPBrdst(u8   byVMPBrdst){ m_byVMPBrdst = byVMPBrdst;} 
    BOOL32 IsVMPBrdst( void ) const { return m_byVMPBrdst == 0 ? FALSE : TRUE; }
    void   SetVMPStyle(u8   byVMPStyle){ m_byVMPStyle = byVMPStyle;} 
    u8     GetVMPStyle( void ) const { return m_byVMPStyle; }
    u8     GetMaxMemberNum( void ) const 
	{
		u8   byMaxMemNum = 1;

		switch( m_byVMPStyle ) 
		{
		case VMP_STYLE_ONE_VER36:
			byMaxMemNum = 1;
			break;
		case VMP_STYLE_VTWO_VER36:
		case VMP_STYLE_HTWO_VER36:
			byMaxMemNum = 2;
			break;
		case VMP_STYLE_THREE_VER36:
			byMaxMemNum = 3;
			break;
		case VMP_STYLE_FOUR_VER36:
			byMaxMemNum = 4;
			break;
		case VMP_STYLE_SIX_VER36:
			byMaxMemNum = 6;
			break;
		case VMP_STYLE_EIGHT_VER36:
			byMaxMemNum = 8;
			break;
		case VMP_STYLE_NINE_VER36:
			byMaxMemNum = 9;
			break;
		case VMP_STYLE_TEN_VER36:
			byMaxMemNum = 10;
			break;
		case VMP_STYLE_THIRTEEN_VER36:
			byMaxMemNum = 13;
			break;
		case VMP_STYLE_SIXTEEN_VER36:
			byMaxMemNum = 16;
			break;
		case VMP_STYLE_SPECFOUR_VER36:
			byMaxMemNum = 4;
			break;
		case VMP_STYLE_SEVEN_VER36:
			byMaxMemNum = 7;
			break;
		default:
			byMaxMemNum = 1;
			break;
		}
		return byMaxMemNum;
	}
	void   SetVmpMember( u8   byMemberId, TVMPMemberVer36 tVMPMember )
	{
		if( byMemberId >= MAXNUM_VMP_MEMBER_VER36 )return;
		m_atVMPMember[byMemberId] = tVMPMember;
	}
	void   ClearVmpMember( u8 byMemberId )
	{
		if( byMemberId >= MAXNUM_VMP_MEMBER_VER36 )return;
		m_atVMPMember[byMemberId].SetNull();
	}
    TVMPMemberVer36 *GetVmpMember( u8   byMemberId )
	{
		if( byMemberId >= MAXNUM_VMP_MEMBER_VER36 )return NULL;
		return &m_atVMPMember[byMemberId];
	}
	BOOL32 IsMtInMember( TMtVer36 tMt )
	{
		u8 byLoop = 0;
		u8 byLoopCtrl = 0;
		while( byLoop < GetMaxMemberNum() && byLoopCtrl < MAXNUM_VMP_MEMBER_VER36 )
		{
			if( tMt.GetMtId() == m_atVMPMember[byLoop].GetMtId() && 
				tMt.GetMcuId() == m_atVMPMember[byLoop].GetMcuId() )
			{
				return TRUE;
			}

			byLoop++;
			byLoopCtrl++;
		}

		return FALSE;
	}
	u8 GetChlOfMtInMember( TMtVer36 tMt )
	{
		u8 byLoop = 0;
		while( byLoop < GetMaxMemberNum() )
		{
			if( tMt.GetMtId() == m_atVMPMember[byLoop].GetMtId() )
			{
				return byLoop;
			}

			byLoop++;
		}

		return MAXNUM_VMP_MEMBER_VER36;
	}
	void   Print( void )
	{
		StaticLog( "\nVMPParam:\n" );
		StaticLog( "m_byVMPAuto: %d\n", m_byVMPAuto);
		StaticLog( "m_byVMPBrdst: %d\n", m_byVMPBrdst);
		StaticLog( "m_byVMPStyle: %d\n", m_byVMPStyle);
		for( s32 i=0;i<GetMaxMemberNum();i++ )
		{
			StaticLog( "VMP member %d: MtId-%d, Type-%d\n", 
				i,  m_atVMPMember[i].GetMtId(), m_atVMPMember[i].GetMemberType() );
		}
	}
}PACKED;

//����Žṹ (len:16)
struct CConfIdVer36
{
protected:
	u8  	m_abyConfId[16];

public:
	u8		CConfIdVer36::GetConfId( u8   * pbyConfId, u8   byBufLen ) const; //��û��������
	void	CConfIdVer36::CreateConfId( void );							//���������
	void	GetConfIdString( LPSTR lpszConfId, u8   byBufLen ) const;	//��û�����ַ���
	void	SetConfId( const CConfIdVer36 & cConfId );						//���û����
	u8		SetConfId( const u8   * pbyConfId, u8   byBufLen );			//���û��������
	u8		SetConfId( LPCSTR lpszConfId );								//���û��������	
	BOOL32	IsNull( void ) const;										//�жϻ�����Ƿ�Ϊ��
	void	SetNull( void );											//���û����Ϊ��
	BOOL32	operator == ( const CConfIdVer36 & cConfId ) const;			//����������
	void	Print( void )
	{
		s8 achBuf[33];
		achBuf[32] = 0;
		GetConfIdString( achBuf, 33 );
		StaticLog( "%s", achBuf);
	}
}PACKED;

//�������״̬�ṹ
struct TConfStatusVer36
{
public:
	TMtVer36 			m_tChairman;	    //��ϯ�նˣ�MCU��Ϊ0��ʾ����ϯ
	TMtVer36		    m_tSpeaker;			//�����նˣ�MCU��Ϊ0��ʾ�޷�����
	TConfModeVer36		m_tConfMode;        //���鷽ʽ
	TRecProgVer36		m_tRecProg;			//��ǰ¼�����,��¼��״̬ʱ��Ч
	TRecProgVer36		m_tPlayProg;	    //��ǰ�������,������״̬ʱ��Ч
	TPollInfoVer36		m_tPollInfo;        //������ѯ����,����ѯʱ�н�
	TDiscussParamVer36	m_tDiscussParam;    //��ǰ����������������ʱ��Ч
	TVMPParamVer36		m_tVMPParam;        //��ǰ��Ƶ���ϲ���������Ƶ����ʱ��Ч
	u8					m_byPrs;            //��ǰ���ش�״̬
	
public:
    BOOL32   HasChairman( void ) const{ return m_tChairman.GetMtId()==0 ? FALSE : TRUE; }	
	TMtVer36 GetChairman( void ) const{ return m_tChairman; }
	void	 SetNoChairman( void ){ memset( &m_tChairman, 0, sizeof( TMtVer36 ) ); }
	void	 SetChairman( TMtVer36 tChairman ){ m_tChairman = tChairman; }
	BOOL32   HasSpeaker( void ) const{ return m_tSpeaker.GetMtId()==0 ? FALSE : TRUE; }
    TMtVer36 GetSpeaker( void ) const{ return m_tSpeaker; }
	void	 SetNoSpeaker( void ){ memset( &m_tSpeaker, 0, sizeof( TMtVer36 ) ); }
	void	 SetSpeaker( TMtVer36 tSpeaker ){ m_tSpeaker = tSpeaker; }
    void           SetConfMode(TConfModeVer36 tConfMode){ m_tConfMode = tConfMode;} 
    TConfModeVer36 GetConfMode( void ) const { return m_tConfMode; }
    void           SetRecProg(TRecProgVer36 tRecProg){ m_tRecProg = tRecProg;} 
    TRecProgVer36  GetRecProg( void ) const { return m_tRecProg; }
    void           SetPlayProg(TRecProgVer36 tPlayProg){ m_tPlayProg = tPlayProg;} 
    TRecProgVer36  GetPlayProg( void ) const { return m_tPlayProg; }
	void           SetDiscussMember(TDiscussParamVer36 tAudMixParam)
	{ memcpy( m_tDiscussParam.m_atMtMember, tAudMixParam.m_atMtMember, sizeof(tAudMixParam.m_atMtMember) ); } 
    void			   SetDiscussParam(TDiscussParamVer36 tAudMixParam){ m_tDiscussParam = tAudMixParam;} 
    TDiscussParamVer36 GetDiscussParam( void ) const { return m_tDiscussParam; }
    void		   SetVmpParam(TVMPParamVer36 tVMPParam){ m_tVMPParam = tVMPParam;} 
    TVMPParamVer36 GetVmpParam( void ) const { return m_tVMPParam; } 
	//�������״̬
	BOOL32	IsScheduled( void ) const { return m_tConfMode.GetTakeMode() == CONF_TAKEMODE_SCHEDULED_VER36 ? TRUE : FALSE; }
	void	SetScheduled( void ){ m_tConfMode.SetTakeMode( CONF_TAKEMODE_SCHEDULED_VER36 ); }
	BOOL32	IsOngoing( void ) const { return m_tConfMode.GetTakeMode() == CONF_TAKEMODE_ONGOING_VER36 ? TRUE : FALSE; }
	void	SetOngoing( void ){ m_tConfMode.SetTakeMode( CONF_TAKEMODE_ONGOING_VER36 ); }
	BOOL32	IsTemplate( void ) const { return m_tConfMode.GetTakeMode() == CONF_TAKEMODE_TEMPLATE_VER36 ? TRUE : FALSE; }
	void	SetTemplate( void ){ m_tConfMode.SetTakeMode( CONF_TAKEMODE_TEMPLATE_VER36 ); }
	u8		GetTakeMode( void ){ return m_tConfMode.GetTakeMode();	}
	void	SetTakeMode( u8 byTakeMode ){ m_tConfMode.SetTakeMode( byTakeMode ); }

	//���鱣��״̬
	u8   GetProtectMode( void ) const { return m_tConfMode.GetLockMode(); }
    void SetProtectMode( u8   byProtectMode ) { m_tConfMode.SetLockMode( byProtectMode ); }
	
	//������в���
	u8   GetCallMode( void ) const { return m_tConfMode.GetCallMode(); }
	void SetCallMode( u8   byCallMode ){ m_tConfMode.SetCallMode( byCallMode ); }
	void SetCallInterval(u32 dwCallInterval){ m_tConfMode.SetCallInterval( dwCallInterval ); } 
    u32  GetCallInterval( void ) const { return m_tConfMode.GetCallInterval( ); }
	void SetCallTimes(u32 dwCallTimes){ m_tConfMode.SetCallTimes( dwCallTimes ); } 
    u32  GetCallTimes( void ) const { return m_tConfMode.GetCallTimes( ); }

	//����������ʽ
	BOOL32	IsVACMode( void ) const { return m_tConfMode.IsVACMode(); }
	void	SetVACMode( BOOL32 bVACMode = TRUE ) { m_tConfMode.SetVACMode( bVACMode ); }

	//�������״̬
	BOOL32	IsNoMixing( void ) const	{ return !m_tConfMode.IsAudioMixMode(); }
	void	SetNoMixing( void )	{ m_tConfMode.SetAudioMixMode( 0 ); }
	BOOL32	IsMixing( void ) const	{ return m_tConfMode.IsAudioMixMode(); }
	void	SetMixing( void )	{ m_tConfMode.SetAudioMixMode( 1 ); }

	//�������۷�ʽ
	BOOL32	IsDiscussMode( void ) const { return m_tConfMode.IsDiscussMode(); }
	void	SetDiscussMode( BOOL32 bDiscussMode = TRUE ) { m_tConfMode.SetDiscussMode( bDiscussMode ); }

    //����ϳɷ�ʽ
	void	SetVMPMode(u8   byVMPMode){ m_tConfMode.SetVMPMode( byVMPMode ); } 
    u8		GetVMPMode( void ) const { return m_tConfMode.GetVMPMode(); }
	BOOL32  IsBrdstVMP( void ) const { return m_tConfMode.GetVMPMode() != CONF_VMPMODE_NONE_VER36 && m_tVMPParam.IsVMPBrdst(); }
	void	SetVmpBrdst( BOOL32 bBrdst ){ m_tVMPParam.SetVMPBrdst( bBrdst ); }
	u8		GetVmpStyle( void ){ return m_tVMPParam.GetVMPStyle(); }
	void	SetVmpStyle( u8 byVMPStyle ){ m_tVMPParam.SetVMPStyle( byVMPStyle ); }

	//ǿ�ƹ㲥������
	BOOL32	IsMustSeeSpeaker( void ) const { return m_tConfMode.IsForceRcvSpeaker(); }
	void	SetMustSeeSpeaker( BOOL32 bMustSeeSpeaker ) { m_tConfMode.SetForceRcvSpeaker( bMustSeeSpeaker ); }

	//��������״̬
	BOOL32	IsRtpAdapting( void ) const { return m_tConfMode.GetBasMode( CONF_BASMODE_RTP_VER36 ); }
	BOOL32	IsBrAdapting( void ) const { return m_tConfMode.GetBasMode( CONF_BASMODE_BR_VER36 ); }
	BOOL32	IsMdtpAdapting( void ) const { return m_tConfMode.GetBasMode( CONF_BASMODE_MDTP_VER36 ); }
	void SetAdaptMode( u8 byBasMode, BOOL32 bAdapt ) 
	{ 
		m_tConfMode.SetBasMode( byBasMode, bAdapt );
	}

	//������ϯ״̬
	BOOL32 IsNoChairMode( void ) const { return m_tConfMode.IsNoChairMode(); }
    void   SetNoChairMode( BOOL32 bNoChairMode ) { m_tConfMode.SetNoChairMode( bNoChairMode ); }

	//��ѯ��ʽ
	void   SetPollMode(u8   byPollMode){ m_tConfMode.SetPollMode( byPollMode ); } 
    u8     GetPollMode(){ return m_tConfMode.GetPollMode(); }
	
	//��ѯ����
    void   SetPollInfo(TPollInfoVer36 tPollInfo){ m_tPollInfo = tPollInfo;} 
    TPollInfoVer36  *GetPollInfo( void ) { return &m_tPollInfo; }
	void   SetPollMedia(u8   byMediaMode){ m_tPollInfo.SetMediaMode( byMediaMode ); } 
    u8     GetPollMedia( void ){ return m_tPollInfo.GetMediaMode(); }
	void   SetPollState(u8   byPollState){ m_tPollInfo.SetPollState( byPollState );} 
    u8     GetPollState( void ) const { return m_tPollInfo.GetPollState(); }
    void   SetMtPollParam(TMtPollParamVer36 tMtPollParam){ m_tPollInfo.SetMtPollParam( tMtPollParam ); } 
    TMtPollParamVer36  GetMtPollParam(){ return m_tPollInfo.GetMtPollParam(); }

	//����¼��״̬	
	BOOL32 IsNoRecording( void ) const{ return m_tConfMode.GetRecordMode() == CONF_RECMODE_NONE_VER36 ? TRUE : FALSE; }
	void   SetNoRecording( void ){ m_tConfMode.SetRecordMode( CONF_RECMODE_NONE_VER36 ); }
	BOOL32 IsRecording( void ) const{ return m_tConfMode.GetRecordMode() == CONF_RECMODE_REC_VER36 ? TRUE : FALSE; }
	void   SetRecording( void ){ m_tConfMode.SetRecordMode( CONF_RECMODE_REC_VER36 ); }
	BOOL32 IsRecPause( void ) const{ return m_tConfMode.GetRecordMode() == CONF_RECMODE_PAUSE_VER36 ? TRUE : FALSE; }
	void   SetRecPause( void ){ m_tConfMode.SetRecordMode( CONF_RECMODE_PAUSE_VER36 ); }

	//�������״̬
	BOOL32 IsNoPlaying( void ) const{ return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_NONE_VER36 ? TRUE : FALSE; }
	void   SetNoPlaying( void ){ m_tConfMode.SetPlayMode( CONF_PLAYMODE_NONE_VER36 ); }
	BOOL32 IsPlaying( void ) const { return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_PLAY_VER36 ? TRUE : FALSE; }
	void   SetPlaying( void ){ m_tConfMode.SetPlayMode( CONF_PLAYMODE_PLAY_VER36 ); }
	BOOL32 IsPlayPause( void )const { return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_PAUSE_VER36 ? TRUE : FALSE; }
	void   SetPlayPause( void ){ m_tConfMode.SetPlayMode( CONF_PLAYMODE_PAUSE_VER36 ); }
	BOOL32 IsPlayFF( void )const { return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_FF_VER36 ? TRUE : FALSE; }
	void   SetPlayFF( void ) { m_tConfMode.SetPlayMode( CONF_PLAYMODE_FF_VER36 ); }
	BOOL32 IsPlayFB( void )const { return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_FB_VER36 ? TRUE : FALSE; }
	void   SetPlayFB( void ){ m_tConfMode.SetPlayMode( CONF_PLAYMODE_FB_VER36 ); }

	//���ش�״̬
	void   SetPrsing( BOOL32 bPrs ){ m_byPrs = bPrs; }
	BOOL32 IsPrsing( void ){ return m_byPrs; }

	//ע��GK���
	void   SetRegToGK( BOOL32 bRegToGK ){ m_tConfMode.SetRegToGK( bRegToGK ); }
	BOOL32 IsRegToGK( void ){ return m_tConfMode.IsRegToGK(); }

	//������ʽ 
    BOOL32 IsMixSpecMt( void ) const { return m_tConfMode.IsMixSpecMt(); }
	void   SetMixSpecMt( BOOL32 bMixSpecMt ){ m_tConfMode.SetMixSpecMt(bMixSpecMt); }

	void Print( void )
	{
		StaticLog( "\nConfStatus:\n" );
		StaticLog( "Chairman: Mcu%dMt%d\n", m_tChairman.GetMcuId(), m_tChairman.GetMtId() );
		StaticLog( "Speaker: Mcu%dMt%d\n", m_tSpeaker.GetMcuId(), m_tSpeaker.GetMtId() );
	}
}PACKED;

//ͬʱ�������ṹ(len:4)
struct TSimCapSetVer36
{
protected:
	u8  m_byVideoMediaType;
	u8  m_byAudioMediaType;
	u8  m_byDataMediaType1;
	u8  m_byDataMediaType2;
public:
	TSimCapSetVer36( void )
	{
		m_byVideoMediaType = MEDIA_TYPE_NULL_VER36;
		m_byAudioMediaType = MEDIA_TYPE_NULL_VER36;
		m_byDataMediaType1 = MEDIA_TYPE_NULL_VER36;
		m_byDataMediaType2 = MEDIA_TYPE_NULL_VER36;
	}

    void SetVideoMediaType( u8 byVideoMediaType ){ m_byVideoMediaType = byVideoMediaType;} 
    u8   GetVideoMediaType( void ) const { return m_byVideoMediaType; }
    void SetAudioMediaType( u8 byAudioMediaType ){ m_byAudioMediaType = byAudioMediaType;} 
    u8   GetAudioMediaType( void ) const { return m_byAudioMediaType; }
    void SetDataMediaType1( u8 byDataMediaType1 ){ m_byDataMediaType1 = byDataMediaType1;} 
    u8   GetDataMediaType1( void ) const { return m_byDataMediaType1; }
    void SetDataMediaType2( u8 byDataMediaType2 ){ m_byDataMediaType2 = byDataMediaType2;} 
    u8   GetDataMediaType2( void ) const { return m_byDataMediaType2; }

	BOOL32   operator ==( const TSimCapSetVer36 & tSimCapSet ) const  //�ж��Ƿ����(ֻ�ж�����ͼ��)
	{
		if( m_byVideoMediaType == tSimCapSet.GetVideoMediaType() && 
			m_byAudioMediaType == tSimCapSet.GetAudioMediaType() && 
			m_byVideoMediaType != MEDIA_TYPE_NULL_VER36 && 
			m_byAudioMediaType != MEDIA_TYPE_NULL_VER36 )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	BOOL32   IsLike(  const TSimCapSetVer36 & tSimCapSet )  const  //�ж��Ƿ�����(ֻ�ж�����ͼ��)
	{
		if( *this == tSimCapSet )
		{
			return TRUE;
		}
		if( ( m_byVideoMediaType == MEDIA_TYPE_NULL_VER36 && m_byAudioMediaType != MEDIA_TYPE_NULL_VER36 && 
			  m_byAudioMediaType == tSimCapSet.GetAudioMediaType() ) || 
			( m_byAudioMediaType == MEDIA_TYPE_NULL_VER36 && m_byVideoMediaType != MEDIA_TYPE_NULL_VER36 && 
			  m_byVideoMediaType == tSimCapSet.GetVideoMediaType() ) || 
			( tSimCapSet.GetAudioMediaType() == MEDIA_TYPE_NULL_VER36 && tSimCapSet.GetVideoMediaType() != MEDIA_TYPE_NULL_VER36 && 
			  tSimCapSet.GetVideoMediaType() == m_byVideoMediaType ) || 
			( tSimCapSet.GetVideoMediaType() == MEDIA_TYPE_NULL_VER36 && tSimCapSet.GetAudioMediaType() != MEDIA_TYPE_NULL_VER36 && 
			  tSimCapSet.GetAudioMediaType() == m_byAudioMediaType ) )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	BOOL32   IsNull( void )
	{
		if(	m_byVideoMediaType == MEDIA_TYPE_NULL_VER36 && m_byAudioMediaType == MEDIA_TYPE_NULL_VER36 )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	void  SetNull( void )
	{
		m_byVideoMediaType = MEDIA_TYPE_NULL_VER36;
		m_byAudioMediaType = MEDIA_TYPE_NULL_VER36;
		m_byDataMediaType1 = MEDIA_TYPE_NULL_VER36;
		m_byDataMediaType2 = MEDIA_TYPE_NULL_VER36;
	}
}PACKED;

//�������ṹ(len:27)
struct TCapSupportVer36
{
protected:
	//��Ƶ
	u8   m_bySupportG711A;	//�Ƿ�֧��g711alaw
	u8   m_bySupportG711U;	//�Ƿ�֧��g911ulaw
	u8   m_bySupportG722;   //�Ƿ�֧��g722
	u8   m_bySupportG7231;	//�Ƿ�֧��g7231
	u8   m_bySupportG728;	//�Ƿ�֧��g728
	u8   m_bySupportG729;   //�Ƿ�֧��g729
	u8   m_bySupportMP3;	//�Ƿ�֧��mp3
	//��Ƶ
	u8   m_bySupportH261;	//�Ƿ�֧��h261
	u8   m_bySupportH262;	//�Ƿ�֧��h262
	u8   m_bySupportH263;	//�Ƿ�֧��h263
	u8   m_bySupportH263Plus;	//�Ƿ�֧��h263+ ˫��
	u8   m_bySupportH239;	    //�Ƿ�֧��h239  ˫��
	u8   m_bySupportH264;	//�Ƿ�֧��h264
	u8   m_bySupportMPEG4;	//�Ƿ�֧��mpeg4
	//����
	u8   m_bySupportT120;	//�Ƿ�֧��t120
	u8   m_bySupportH224;	//�Ƿ�֧��H224

	u8   m_bySupportMMcu;   //�Ƿ�֧�ֺϲ�����

	//ͬʱ������
	//����ģʽCONF_ENCRYPTMODE_NONE_VER36, CONF_ENCRYPTMODE_DES_VER36, CONF_ENCRYPTMODE_AES_VER36
	u8 m_byMainEncryptMode;		  //����ͬʱ����������ģʽ
	u8 m_bySecondEncryptMode;	  //����ͬʱ����������ģʽ
	TSimCapSetVer36 tMainSimCapSet;    //����ͬʱ������
	TSimCapSetVer36 tSecondSimCapSet;  //����ͬʱ������

public:
	//���캯��
	TCapSupportVer36( void ) { memset( this, 0, sizeof( TCapSupportVer36 ) ); tMainSimCapSet.SetNull(); tSecondSimCapSet.SetNull(); }
    void   SetSupportG711A(u8   bySupportG711A){ m_bySupportG711A = bySupportG711A;} 
    u8     GetSupportG711A( void ) const { return m_bySupportG711A; }
    void   SetSupportG711U(u8   bySupportG711U){ m_bySupportG711U = bySupportG711U;} 
    u8     GetSupportG711U( void ) const { return m_bySupportG711U; }
    void   SetSupportG722(u8   bySupportG722){ m_bySupportG722 = bySupportG722;} 
    u8     GetSupportG722( void ) const { return m_bySupportG722; }
    void   SetSupportG7231(u8   bySupportG7231){ m_bySupportG7231 = bySupportG7231;} 
    u8     GetSupportG7231( void ) const { return m_bySupportG7231; }
    void   SetSupportG728(u8   bySupportG728){ m_bySupportG728 = bySupportG728;} 
    u8     GetSupportG728( void ) const { return m_bySupportG728; }
    void   SetSupportG729(u8   bySupportG729){ m_bySupportG729 = bySupportG729;} 
    u8     GetSupportG729( void ) const { return m_bySupportG729; }
    void   SetSupportMP3(u8   bySupportMP3){ m_bySupportMP3 = bySupportMP3;} 
    u8     GetSupportMP3( void ) const { return m_bySupportMP3; } 
    void   SetSupportH261(u8   bySupportH261){ m_bySupportH261 = bySupportH261;} 
    u8     GetSupportH261( void ) const { return m_bySupportH261; }
    void   SetSupportH262(u8   bySupportH262){ m_bySupportH262 = bySupportH262;} 
    u8     GetSupportH262( void ) const { return m_bySupportH262; }
    void   SetSupportH263(u8   bySupportH263){ m_bySupportH263 = bySupportH263;} 
    u8     GetSupportH263( void ) const { return m_bySupportH263; }
    void   SetSupportH263Plus(u8   bySupportH263Plus){ m_bySupportH263Plus = bySupportH263Plus;} 
    u8     GetSupportH263Plus( void ) const { return m_bySupportH263Plus; }
    void   SetSupportH239(u8 bySupportH239){ m_bySupportH239 = bySupportH239;} 
    u8     GetSupportH239( void ) const { return m_bySupportH239; }	
    void   SetSupportH264(u8   bySupportH264){ m_bySupportH264 = bySupportH264;} 
    u8     GetSupportH264( void ) const { return m_bySupportH264; }
    void   SetSupportMPEG4(u8   bySupportMPEG4){ m_bySupportMPEG4 = bySupportMPEG4;} 
    u8     GetSupportMPEG4( void ) const { return m_bySupportMPEG4; }
    void   SetSupportT120(u8   bySupportT120){ m_bySupportT120 = bySupportT120;} 
    u8     GetSupportT120( void ) const { return m_bySupportT120; }
    void   SetSupportH224(u8   bySupportH224){ m_bySupportH224 = bySupportH224;} 
    u8     GetSupportH224( void ) const { return m_bySupportH224; }
	void   SetSupportMMcu( BOOL32 bMMcu) { m_bySupportMMcu = (bMMcu==TRUE?1:0); }
	BOOL32 IsSupportMMcu() const { return (m_bySupportMMcu != 0 ); }

	BOOL32 IsSupportMediaType( u8 byMediaType ); //�Ƿ�֧��ĳ��ý������
	void   SetVideoType( u8   byVideoType );//������Ƶ����
	void   SetAudioType( u8   byAudioType );//������Ƶ����
	u8     GetPriVideoType( void );//�õ����ȵ���Ƶ����
	u8     GetPriAudioType( void );//�õ����ȵ���Ƶ����

	void SetMainEncryptMode(u8 byEncryptMode) {m_byMainEncryptMode = byEncryptMode;}
    u8   GetMainEncryptMode(){return m_byMainEncryptMode;}
	void SetSecondEncryptMode(u8 byEncryptMode) {m_bySecondEncryptMode = byEncryptMode;}
    u8   GetSecondEncryptMode(){return m_bySecondEncryptMode;}
	TSimCapSetVer36 GetMainSimCapSet( void ){ return tMainSimCapSet; }
	TSimCapSetVer36 GetSecondSimCapSet( void ){ return tSecondSimCapSet; }
	void SetMainSimCapSet( TSimCapSetVer36 tSimCapSet )
	{ 
		tMainSimCapSet = tSimCapSet; 
		SetVideoType( tMainSimCapSet.GetVideoMediaType() );
		SetAudioType( tMainSimCapSet.GetAudioMediaType() );
		if( MEDIA_TYPE_T120_VER36 == tMainSimCapSet.GetDataMediaType1() )
		{
			SetSupportT120( TRUE );
		}
		if( MEDIA_TYPE_H224_VER36 == tMainSimCapSet.GetDataMediaType1() )
		{
			SetSupportH224( TRUE );
		}
		if( MEDIA_TYPE_T120_VER36 == tMainSimCapSet.GetDataMediaType2() )
		{
			SetSupportT120( TRUE );
		}
		if( MEDIA_TYPE_H224_VER36 == tMainSimCapSet.GetDataMediaType2() )
		{
			SetSupportH224( TRUE );
		}
	}
	void SetSecondSimCapSet( TSimCapSetVer36 tSimCapSet )
	{ 
		tSecondSimCapSet = tSimCapSet; 
		SetVideoType( tSecondSimCapSet.GetVideoMediaType() );
		SetAudioType( tSecondSimCapSet.GetAudioMediaType() );
		if( MEDIA_TYPE_T120_VER36 == tSecondSimCapSet.GetDataMediaType1() )
		{
			SetSupportT120( TRUE );
		}
		if( MEDIA_TYPE_H224_VER36 == tSecondSimCapSet.GetDataMediaType1() )
		{
			SetSupportH224( TRUE );
		}
		if( MEDIA_TYPE_T120_VER36 == tSecondSimCapSet.GetDataMediaType2() )
		{
			SetSupportT120( TRUE );
		}
		if( MEDIA_TYPE_H224_VER36 == tSecondSimCapSet.GetDataMediaType2() )
		{
			SetSupportH224( TRUE );
		}
	}

	void Clear( void )
	{
		memset( this, 0, sizeof( TCapSupportVer36 ) );
		m_byMainEncryptMode   = CONF_ENCRYPTMODE_NONE_VER36;
		m_bySecondEncryptMode = CONF_ENCRYPTMODE_NONE_VER36;
		tMainSimCapSet.SetNull();
		tSecondSimCapSet.SetNull();
	}
	
	void Print( void )
	{
		StaticLog( "\nvideo support: ");
		if( m_bySupportH261 )StaticLog( "H.261 " );
		if( m_bySupportH262 )StaticLog( "H.262 " );
		if( m_bySupportH263 )StaticLog( "H.263 " );
		if( m_bySupportH264 )StaticLog( "H.264 " );
		if( m_bySupportMPEG4 )StaticLog( "MPEG-4 " );
		StaticLog( "\naudio support: ");
		if( m_bySupportG711A )StaticLog( "G711A " );
		if( m_bySupportG711U )StaticLog( "G711U " );
		if( m_bySupportG722  )StaticLog( "G722 " );
		if( m_bySupportG7231 )StaticLog( "G7231 " );
		if( m_bySupportG728 )StaticLog( "G728 " );
		if( m_bySupportG729 )StaticLog( "G729 " );
		if( m_bySupportMP3  )StaticLog( "MP3 " );
		StaticLog( "\nmain encrypt mode:%d, second encrypt mode:%d", m_byMainEncryptMode, m_bySecondEncryptMode);
		StaticLog( "\nmain simul support:");
		StaticLog( "\n %d ", tMainSimCapSet.GetVideoMediaType() );
		StaticLog( "\n %d ", tMainSimCapSet.GetAudioMediaType() );
		StaticLog( "\nsecond simul support:");
		StaticLog( "\n %d ", tSecondSimCapSet.GetVideoMediaType() );
		StaticLog( "\n %d ", tSecondSimCapSet.GetAudioMediaType() );
	}
}PACKED;

//����������Խṹ(�����в���ı�)
struct TConfAttrbVer36
{
protected:
    u8     m_byMeetingRoom;     //�Ƿ������:   0-���ǻ����� 1-������
    u8     m_byOpenMode;        //���鿪�ŷ�ʽ: 0-������,�ܾ��б�������ն� 1-����������� 2-��ȫ����
    u8     m_byEncryptMode;     //�������ģʽ: 0-������, 1-des����,2-aes����
    u8     m_byMulticastMode;   //�鲥���鷽ʽ: 0-�����鲥���� 1-�鲥����
    u8     m_byDataMode;        //���ݻ��鷽ʽ: 0-���������ݵ�����Ƶ���� 1-�������ݵ�����Ƶ���� 2-ֻ�����ݵĻ���
    u8     m_byReleaseMode;     //���������ʽ: 0-�����Զ����� 1-���ն�ʱ�Զ�����
    u8     m_byVideoMode;       //������Ƶģʽ: 0-�ٶ����� 1-��������
    u8     m_byCascadeMode;     //���鼶����ʽ: 0-��֧�ֺϲ�����, 1-֧�ֺϲ�����
    u8     m_byPrsMode;         //�����ش���ʽ: 0-���ش� 1-��
    u8     m_byHasTvWallModule; //����ǽģ��:   0-�޵���ǽģ��  1-�е���ǽģ��
    u8     m_byHasVmpModule;    //����ϳ�ģ��: 0-�޻���ϳ�ģ��  1-�л���ϳ�ģ��
    u8     m_byDiscussConf;     //�Ƿ����ۻ���: 0-�������ۻ���(�ݽ�����) 1-���ۻ��� (��һ�ֶν��������ƻ��鿪ʼ���Ƿ���������)
    u8     m_byUseAdapter;		//�Ƿ���������: 0-�������κ����� 2-����Ҫ����(����������)���� 
    u8     m_bySpeakerSrc;		//�����˵�Դ:   0-���Լ� 1-����ϯ 2-����һ�η����� 
    u8     m_bySpeakerSrcMode;  //�����˵�Դ�ķ�ʽ: 0-��Чֵ 1-����Ƶ 2-����Ƶ 3-����Ƶ
    u8     m_byMulcastLowStream;//�Ƿ��鲥��������: 0-�����鲥��������(�鲥��������) 1-�鲥��������
    u8     m_byAllInitDumb;     //�ն������Ƿ��ʼ���� 0-������ 1-����
    u8     m_byUniformMode;     //����ת��ʱ�Ƿ�֧�ֹ�һ������ʽ: CONF_UNIFORMMODE_NONE-������ CONF_UNIFORMMODE_valid-������
    //�������ʽ�붪���ش���ʽ����
    u8     m_byDStreamType;     //˫����ʽ��Ŀǰ֧���������ͣ�VIDEO_DSTREAM_MAIN->������Ƶ��ʽһ�¡�
    //VIDEO_DSTREAM_H263PLUS->H263����
    //VIDEO_DSTREAM_H239->H239
public:
    TConfAttrbVer36( void )
    { 
        memset( this, 0, sizeof(TConfAttrbVer36) );
        m_byOpenMode = 2;
    }
    void   SetMeetingRoom(BOOL32 bMeetingRoom){ m_byMeetingRoom = bMeetingRoom; } 
    BOOL32 IsMeetingRoom( void ) const { return m_byMeetingRoom == 0 ? FALSE : TRUE; }
    void   SetOpenMode(u8   byOpenMode){ m_byOpenMode = byOpenMode;} 
    u8     GetOpenMode( void ) const { return m_byOpenMode; }
    void   SetEncryptMode(u8   byEncryptMode){ m_byEncryptMode = byEncryptMode;} 
    u8     GetEncryptMode( void ) const { return m_byEncryptMode; }
    void   SetMulticastMode(BOOL32 bMulticastMode){ m_byMulticastMode = bMulticastMode;} 
    BOOL32 IsMulticastMode( void ) const { return m_byMulticastMode == 0 ? FALSE : TRUE; }
    void   SetDataMode(u8   byDataMode){ m_byDataMode = byDataMode;} 
    u8     GetDataMode( void ) const { return m_byDataMode; }
    void   SetReleaseMode(BOOL32 bReleaseMode){ m_byReleaseMode = bReleaseMode;} 
    BOOL32 IsReleaseNoMt( void ) const { return m_byReleaseMode == 0 ? FALSE : TRUE; }
    void   SetQualityPri(BOOL32 bQualityPri){ m_byVideoMode = bQualityPri;} 
    BOOL32 IsQualityPri( void ) const { return m_byVideoMode == 0 ? FALSE : TRUE; }
    void   SetSupportCascade(BOOL32 bCascadeMode){ m_byCascadeMode = (bCascadeMode==TRUE?1:0); }
    BOOL32 IsSupportCascade( void ) const { return (m_byCascadeMode != 0 );}
    void   SetPrsMode( BOOL32 bResendPack ){ m_byPrsMode = bResendPack; }
    BOOL32 IsResendLosePack( void ){ return m_byPrsMode == 0 ? FALSE : TRUE; }
    void   SetHasTvWallModule( BOOL32 bHasTvWallModule ){ m_byHasTvWallModule = bHasTvWallModule; }
    BOOL32 IsHasTvWallModule( void ){ return m_byHasTvWallModule == 0 ? FALSE : TRUE; }
    void   SetHasVmpModule( BOOL32 bHasVmpModule ){ m_byHasVmpModule = bHasVmpModule; }
    BOOL32 IsHasVmpModule( void ){ return m_byHasVmpModule == 0 ? FALSE : TRUE; }
    void   SetDiscussConf( BOOL32 bDiscussConf ){ m_byDiscussConf = bDiscussConf; }
    BOOL32 IsDiscussConf( void ){ return m_byDiscussConf == 0 ? FALSE : TRUE; }
    void   SetUseAdapter( BOOL32 bUseAdapter ){ m_byUseAdapter = bUseAdapter; } 
    BOOL32 IsUseAdapter( void ) const { return m_byUseAdapter == 0 ? FALSE : TRUE; }
    void   SetSpeakerSrc( u8 bySpeakerSrc ){ m_bySpeakerSrc = bySpeakerSrc;} 
    u8     GetSpeakerSrc( void ) const { return m_bySpeakerSrc; }
    void   SetSpeakerSrcMode( u8 bySpeakerSrcMode ){ m_bySpeakerSrcMode = bySpeakerSrcMode;} 
    u8     GetSpeakerSrcMode( void ) const { return m_bySpeakerSrcMode; }
    void   SetMulcastLowStream( BOOL32 bMulcastLowStream ){ m_byMulcastLowStream = bMulcastLowStream;} 
    BOOL32 IsMulcastLowStream( void ) const { return m_byMulcastLowStream == 0 ? FALSE : TRUE; }
    void   SetAllInitDumb( BOOL32 bAllInitDumb ){ m_byAllInitDumb = bAllInitDumb;} 
    BOOL32 IsAllInitDumb( void ) const { return m_byAllInitDumb == 0 ? FALSE : TRUE; }
    void   SetUniformMode( BOOL32 bUniformMode ){ m_byUniformMode = bUniformMode; }
    u8     IsAdjustUniformPack( void ){ return m_byUniformMode; }
    void   SetDStreamType( u8 byDStreamType ){ m_byDStreamType = byDStreamType; }
    u8     GetDStreamType( void ){ return m_byDStreamType; }
    
    void   Print( void )
    {
        StaticLog( "\nConfAttrb:\n" );
        StaticLog( "m_byMeetingRoom: %d\n", m_byMeetingRoom);
        StaticLog( "m_byOpenMode: %d\n", m_byOpenMode);
        StaticLog( "m_byEncryptMode: %d\n", m_byEncryptMode);
        StaticLog( "m_byMulticastMode: %d\n", m_byMulticastMode);
        StaticLog( "m_byDataMode: %d\n", m_byDataMode);
        StaticLog( "m_byReleaseMode: %d\n", m_byReleaseMode);
        StaticLog( "m_byVideoMode: %d\n", m_byVideoMode);
        StaticLog( "m_byCascadeMode: %d\n", m_byCascadeMode);
        StaticLog( "m_byPrsMode: %d\n", m_byPrsMode);
        StaticLog( "m_byHasTvWallModule: %d\n", m_byHasTvWallModule);
        StaticLog( "m_byHasVmpModule: %d\n", m_byHasVmpModule);	
        StaticLog( "m_byDiscussConf: %d\n", m_byDiscussConf);
        StaticLog( "m_byUseAdapter: %d\n", m_byUseAdapter);
        StaticLog( "m_bySpeakerSrc: %d\n", m_bySpeakerSrc);
        StaticLog( "m_bySpeakerSrcMode: %d\n", m_bySpeakerSrcMode);
        StaticLog( "m_byMulcastLowStream: %d\n", m_byMulcastLowStream);
        StaticLog( "m_byAllInitDumb: %d\n", m_byAllInitDumb);
        StaticLog( "m_byUniformMode: %d\n", m_byUniformMode);
        StaticLog( "m_byDStreamType: %d\n", m_byDStreamType);
    }
}PACKED;

//(len:22)
struct TMediaEncryptVer36
{
protected:
    u8  m_byEncryptMode;    //����ģʽ:CONF_ENCRYPTMODE_NONE_VER36,CONF_ENCRYPTMODE_DES_VER36, CONF_ENCRYPT_AES_VER36
    s32 m_nKeyLen;          //����key�ĳ���
    u8  m_abyEncKey[MAXLEN_KEY_VER36]; //����key
    u8  m_byReserve;//����
public:
    TMediaEncryptVer36()
    {
        Reset();
    }
    void Reset()
    {
        m_byEncryptMode = CONF_ENCRYPTMODE_NONE_VER36;
        m_nKeyLen = 0;
    }
    
    void SetEncryptMode(u8 byEncMode)
    {
        m_byEncryptMode = byEncMode;
    }
    u8  GetEncryptMode()
    {
        return m_byEncryptMode;
    }
    void SetEncryptKey(u8 *pbyKey, s32 nLen)
    {
        m_nKeyLen = (nLen > MAXLEN_KEY_VER36 ? MAXLEN_KEY_VER36 : nLen);
        if(m_nKeyLen > 0)
            memcpy(m_abyEncKey, pbyKey, m_nKeyLen); 
        m_nKeyLen = htonl(m_nKeyLen);
    }
    
    void GetEncryptKey(u8 *pbyKey, s32* pnLen)
    {
        //if(pnLen != NULL)
			*pnLen = ntohl(m_nKeyLen);
        //if(pbyKey != NULL) 
			memcpy(pbyKey, m_abyEncKey, ntohl(m_nKeyLen));
    }
}PACKED;

//ʱ��ṹ���¶���
struct TKdvTimeVer36
{
protected:
    u16 		m_wYear;		//��
    u8  		m_byMonth;		//��
    u8  		m_byMDay;		//��
    u8  		m_byHour;		//ʱ
    u8  		m_byMinute;		//��
    u8  		m_bySecond;		//��
    
public:
    void SetTime( const time_t * ptTime );//����ʱ��
    void GetTime( time_t & tTime ) const;//�õ�ʱ��ṹ
    BOOL32 operator == ( const TKdvTimeVer36 & tTime );//����
    
    u16  GetYear( void ) const	{ return( ntohs( m_wYear ) ); }
    u8   GetMonth( void ) const	{ return( m_byMonth ); }
    u8   GetDay( void ) const	{ return( m_byMDay ); }
    u8   GetHour( void ) const	{ return( m_byHour ); }
    u8   GetMinute( void ) const	{ return( m_byMinute ); }
    u8   GetSecond( void ) const	{ return( m_bySecond ); }
    void SetYear( u16  wYear )	{ m_wYear = htons( wYear ); }
    void SetMonth( u8   byMonth )	{ m_byMonth = byMonth; }
    void SetDay( u8   byDay )	{ m_byMDay = byDay; }
    void SetHour( u8   byHour )	{ m_byHour = byHour; }
    void SetMinute( u8   byMinute )	{ m_byMinute = byMinute; }
    void SetSecond( u8   bySecond )	{ m_bySecond = bySecond; }
    void Print( void )
    {
        StaticLog( "%d-%d-%d %d:%d:%d", ntohs(m_wYear),m_byMonth,m_byMDay,m_byHour,m_byMinute,m_bySecond );
    }
}PACKED;

//���������Ϣ�ṹ,�ýṹ�����˻��������Ϣ��״̬ (len:725)
struct TConfInfoVer36
{
protected:
	CConfIdVer36	m_cConfId;			//����ţ�ȫ��Ψһ
	TKdvTimeVer36	m_tStartTime;		//��ʼʱ�䣬����̨��0Ϊ������ʼ
	u16 			m_wDuration;	    //����ʱ��(����)��0��ʾ���Զ�ֹͣ
    u16				m_wBitRate;         //��������(��λ:Kbps,1K=1024)
	u16				m_wSecBitRate;      //˫�ٻ���ĵ�2����(��λ:Kbps,Ϊ0��ʾ�ǵ��ٻ���)
	u16				m_wReserved;        //����
	u8				m_byVideoFormat;    //��Ƶ��ʽ,�μ�mcuconst.h����Ƶ��ʽ����
	u8				m_byTalkHoldTime;   //��С���Գ���ʱ��(��λ:��)
    TCapSupportVer36 m_tCapSupport;		//����֧�ֵ�ý��
	s8      m_achConfPwd[MAXLEN_PWD_VER36+1];		//��������
	s8		m_achConfName[MAXLEN_CONFNAME_VER36+1];	//������
    s8      m_achConfE164[MAXLEN_E164_VER36+1];		//�����E164����
    TMtAliasVer36	m_tChairAlias;      //��������ϯ�ı���
	TMtAliasVer36	m_tSpeakerAlias;    //�����з����˵ı���
    TConfAttrbVer36 m_tConfAttrb;		//��������

	TMediaEncryptVer36  m_tMediaKey;   //��һ�汾������Կһ������֧�ָ���
		
public:
	TConfStatusVer36	m_tStatus;	//����״̬

public:	
	TConfInfoVer36( void ){ memset( this, 0, sizeof( TConfInfoVer36 ) ); m_tCapSupport.Clear(); }
	CConfIdVer36 GetConfId( void ) const { return m_cConfId; }
	void		 SetConfId( CConfIdVer36 cConfId ){ m_cConfId = cConfId; }
	TKdvTimeVer36 GetKdvStartTime( void ) const { return( m_tStartTime ); }
	void		  SetKdvStartTime( TKdvTimeVer36 tStartTime ){ m_tStartTime = tStartTime; }
    void   SetDuration(u16  wDuration){ m_wDuration = htons(wDuration);} 
    u16    GetDuration( void ) const { return ntohs(m_wDuration); }
    void   SetBitRate(u16  wBitRate){ m_wBitRate = htons(wBitRate);} 
    u16    GetBitRate( void ) const { return ntohs(m_wBitRate); }
    void   SetSecBitRate(u16 wSecBitRate){ m_wSecBitRate = htons(wSecBitRate);} 
    u16    GetSecBitRate( void ) const { return ntohs(m_wSecBitRate); }
    void   SetVideoFormat(u8   byVideoFormat){ m_byVideoFormat = byVideoFormat;} 
    u8     GetVideoFormat( void ) const { return m_byVideoFormat; }
    void   SetTalkHoldTime(u8   byTalkHoldTime){ m_byTalkHoldTime = byTalkHoldTime;} 
    u8     GetTalkHoldTime( void ) const { return m_byTalkHoldTime; }
    void   SetCapSupport(TCapSupportVer36 tCapSupport){ m_tCapSupport = tCapSupport;} 
    TCapSupportVer36 GetCapSupport( void ) const { return m_tCapSupport; }
	void   SetConfPwd( LPCSTR lpszConfPwd );
    LPCSTR GetConfPwd( void ) const { return m_achConfPwd; }
	void   SetConfName( LPCSTR lpszConfName );
    LPCSTR GetConfName( void ) const { return m_achConfName; }
	void   SetConfE164( LPCSTR lpszConfE164 );
	LPCSTR GetConfE164( void ) const { return m_achConfE164; }
    void	       SetChairAlias(TMtAliasVer36 tChairAlias){ m_tChairAlias = tChairAlias;} 
    TMtAliasVer36  GetChairAlias( void ) const { return m_tChairAlias; }
    void		   SetSpeakerAlias(TMtAliasVer36 tSpeakerAlias){ m_tSpeakerAlias = tSpeakerAlias;} 
    TMtAliasVer36  GetSpeakerAlias( void ) const { return m_tSpeakerAlias; }
    void		   SetConfAttrb(TConfAttrbVer36 tConfAttrb){ m_tConfAttrb = tConfAttrb;} 
    TConfAttrbVer36 GetConfAttrb( void ) const { return m_tConfAttrb; }
    void			 SetStatus(TConfStatusVer36 tStatus){ m_tStatus = tStatus;} 
    TConfStatusVer36 GetStatus( void ) const { return m_tStatus; }	

	TMediaEncryptVer36& GetMediaKey(void) { return m_tMediaKey; };
	void SetMediaKey(TMediaEncryptVer36& tMediaEncrypt){ memcpy(&m_tMediaKey, &tMediaEncrypt, sizeof(tMediaEncrypt));}

	void     GetVideoScale( u16 &wVideoWidth, u16 &wVideoHeight );
	BOOL32   HasChairman( void ) const{ if(m_tStatus.m_tChairman.GetMtId()==0)return FALSE; return TRUE; }	
	TMtVer36 GetChairman( void ) const{ return m_tStatus.m_tChairman; }
	void     SetNoChairman( void ){ memset( &m_tStatus.m_tChairman, 0, sizeof( TMtVer36 ) ); }
	void     SetChairman( TMtVer36 tChairman ){ m_tStatus.m_tChairman = tChairman; }
	BOOL32   HasSpeaker( void ) const{ if(m_tStatus.m_tSpeaker.GetMtId()==0)return FALSE; return TRUE; }
    TMtVer36 GetSpeaker( void ) const{ return m_tStatus.m_tSpeaker; }
	void   SetNoSpeaker( void ){ memset( &m_tStatus.m_tSpeaker, 0, sizeof( TMtVer36 ) ); }
	void   SetSpeaker( TMtVer36 tSpeaker ){ m_tStatus.m_tSpeaker = tSpeaker; }
	time_t GetStartTime( void ) const;	
	void   SetStartTime( time_t dwStartTime );

	void Print( void )
	{
		StaticLog( "\nConfInfo:\n" );
		StaticLog( "m_cConfId: " );
		m_cConfId.Print();
		StaticLog( "\nm_tStartTime: " );
        m_tStartTime.Print();
		StaticLog( "\nm_wDuration: %d\n", ntohs(m_wDuration) );
		StaticLog( "m_wBitRate: %d\n", ntohs(m_wBitRate) );
		StaticLog( "m_byVideoFormat: %d\n", m_byVideoFormat );
		StaticLog( "m_byTalkHoldTime: %d\n", m_byTalkHoldTime );
		StaticLog( "m_tCapSupport:" );
	    m_tCapSupport.Print();
		StaticLog( "\nm_achConfPwd: %s\n", m_achConfPwd );
		StaticLog( "m_achConfName: %s\n", m_achConfName );
		StaticLog( "m_achConfE164: %s\n", m_achConfE164 );
		StaticLog( "\n" );
	}
}PACKED;


// ----------   TMtVer36    ---------- 

/*====================================================================
    ������      ��SetMt
    ����        �������ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMcuId, MCU��
				  u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2002/07/26  1.0         LI Yi         ����
    2003/10/28  3.0         ������        �޸�
	2005/01/06	4.0			�ű���		  ��3.6��ֲ����
====================================================================*/
inline void TMtVer36::SetMt( u8 byMcuId, u8 byMtId, u8 byDriId, u8 byConfIdx )
{
	m_byMainType = TYPE_MT;
	m_bySubType = 0;
	m_byMcuId = byMcuId;
	m_byEqpId = byMtId;	
	m_byConfDriId = byDriId;  
    m_byConfIdx = byConfIdx; 
}

/*====================================================================
    ������      ��SetMt
    ����        �������ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMt tMt 
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2005/01/24  3.6         κ�α�        ����
	2005/01/06	4.0			�ű���		  ��3.6��ֲ����
====================================================================*/
inline void TMtVer36::SetMt( TMtVer36 tMt )
{
	SetMt(tMt.GetMcuId(), tMt.GetMtId(), tMt.GetDriId(), tMt.GetConfIdx());
}

/*====================================================================
    ������      ��SetMcu
    ����        ������MCU
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMcuId, MCU��
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2002/07/26  1.0         LI Yi         ����
    2003/10/28  3.0         ������        �޸� 
	2005/01/06	4.0			�ű���		  ��3.6��ֲ����
====================================================================*/
inline void TMtVer36::SetMcu( u8 byMcuId )
{
	m_byMainType = TYPE_MCU;
	m_bySubType = 0;
	m_byMcuId = byMcuId;
	m_byEqpId = 0;	
	m_byConfDriId = 0;  
    m_byConfIdx = 0;    
}

/*====================================================================
    ������      ��SetMcuEqp
    ����        ������MCU����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMcuId, MCU��
				  u8 byEqpId, �����
				  u8 byEqpType, ��������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    2002/07/26  1.0         LI Yi         ����
    2003/06/06  1.0         LI Yi         �����������Ͳ���
    2003/10/28  3.0         ������        �޸�  
	2005/01/06	4.0			�ű���		  ��3.6��ֲ����
====================================================================*/
inline void TMtVer36::SetMcuEqp( u8 byMcuId, u8 byEqpId, u8 byEqpType )
{
	m_byMainType = TYPE_MCUPERI;
	m_bySubType = byEqpType;
	m_byMcuId = byMcuId;
	m_byEqpId = byEqpId;	
	m_byConfDriId = 0;  
    m_byConfIdx = 0; 
}


// ----------   TConfInfoVer36    ---------- 

/*====================================================================
    ������      ��SetConfPwd
    ����        �����ñ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����LPCSTR lpszConfPwd, ����
    ����ֵ˵��  ���ַ���ָ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/26    1.0         LI Yi         ����
	2005/01/06	4.0			�ű���		  ��3.6��ֲ����
====================================================================*/
inline void TConfInfoVer36::SetConfPwd( LPCSTR lpszConfPwd )
{
	if( lpszConfPwd != NULL )
	{
		strncpy( m_achConfPwd, lpszConfPwd, sizeof( m_achConfPwd ) );
		m_achConfPwd[sizeof( m_achConfPwd ) - 1] = '\0';
	}
	else
	{
		memset( m_achConfPwd, 0, sizeof( m_achConfPwd ) );
	}
}

/*====================================================================
    ������      ��SetConfName
    ����        �����ñ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����LPCSTR lpszConfName, ����
    ����ֵ˵��  ���ַ���ָ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/26    1.0         LI Yi         ����
	2005/01/06	4.0			�ű���		  ��3.6��ֲ����
====================================================================*/
inline void TConfInfoVer36::SetConfName( LPCSTR lpszConfName )
{
	if( lpszConfName != NULL )
	{
		strncpy( m_achConfName, lpszConfName, sizeof( m_achConfName ) );
		m_achConfName[sizeof( m_achConfName ) - 1] = '\0';
	}
	else
	{
		memset( m_achConfName, 0, sizeof( m_achConfName ) );
	}
}

/*====================================================================
    ������      ��SetConfE164
    ����        �����ñ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����LPCSTR lpszConfE164, ����
    ����ֵ˵��  ���ַ���ָ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/26    1.0         LI Yi         ����
	2005/01/06	4.0			�ű���		  ��3.6��ֲ����
====================================================================*/
inline void TConfInfoVer36::SetConfE164( LPCSTR lpszConfE164 )
{
	if( lpszConfE164 != NULL )
	{
		strncpy( m_achConfE164, lpszConfE164, sizeof( m_achConfE164 ) );
		m_achConfE164[sizeof( m_achConfE164 ) - 1] = '\0';
	}
	else
	{
		memset( m_achConfE164, 0, sizeof( m_achConfE164 ) );
	}
}


// ----------   TCapSupportVer36    ---------- 

/*====================================================================
    ������      ��SetVideoType
    ����        ��������Ƶ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8   byVideoType ��Ƶ����       
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/05    3.0         ������        ����
	2005/01/06	4.0			�ű���		  ��3.6��ֲ����
====================================================================*/
inline void TCapSupportVer36::SetVideoType( u8   byVideoType )
{
    switch( byVideoType ) 
	{
    case MEDIA_TYPE_H261:
       SetSupportH261( TRUE );
    	break;

    case MEDIA_TYPE_H262:
       SetSupportH262( TRUE );
    	break;

	case MEDIA_TYPE_H263:
       SetSupportH263( TRUE );
		break;

	case MEDIA_TYPE_H263PLUS:
		SetSupportH263Plus( TRUE );
		break;

	case MEDIA_TYPE_H264:
       SetSupportH264( TRUE );
		break;

	case MEDIA_TYPE_MP4:
       SetSupportMPEG4( TRUE );
		break;

    default:
		break;
    }
}

/*====================================================================
    ������      ��SetAudioType
    ����        ��������Ƶ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8   byAudioType ��Ƶ����            
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/05    3.0         ������        ���� 
	2005/01/06	4.0			�ű���		  ��3.6��ֲ����
====================================================================*/
inline void TCapSupportVer36::SetAudioType( u8   byAudioType )
{
    switch( byAudioType ) 
	{
    case MEDIA_TYPE_PCMA:
       SetSupportG711A( TRUE );
    	break;

    case MEDIA_TYPE_PCMU:
       SetSupportG711U( TRUE );
    	break;

    case MEDIA_TYPE_G722:
       SetSupportG722( TRUE );
    	break;

    case MEDIA_TYPE_G7231:
       SetSupportG7231( TRUE );
    	break;

	case MEDIA_TYPE_G728:
       SetSupportG728( TRUE );
		break;

	case MEDIA_TYPE_G729:
       SetSupportG729( TRUE );
		break;

	case MEDIA_TYPE_MP3:
       SetSupportMP3( TRUE );
		break;

    default:
		break;
    }
}


// ----------   CConfId    ----------

/*====================================================================
    ������      ��GetConfId
    ����        ����û��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 * pbyConfId, �����BUFFER����ָ�룬�������ػ����
			      u8 byBufLen, BUFFER��С
    ����ֵ˵��  ��ʵ�ʷ��������С
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/31    1.0         LI Yi         ����
	2005/01/09	4.0			�ű���		  ��3.6��ֲ����
====================================================================*/
inline u8 CConfIdVer36::GetConfId( u8 * pbyConfId, u8 byBufLen ) const
{
	u8	byLen = min( sizeof( m_abyConfId ), byBufLen );
	memcpy( pbyConfId, m_abyConfId, byLen );

	return( byLen );
}

/*====================================================================
    ������      ��GetConfIdString
    ����        ����û�����ַ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����LPSTR lpszConfId, ������ַ���BUFFER����ָ�룬����
	                   ����0��β������ַ���
			      u8 byBufLen, BUFFER��С
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/31    1.0         LI Yi         ����
	2005/01/06	4.0			�ű���		  ��3.6��ֲ����
====================================================================*/
inline void CConfIdVer36::GetConfIdString( LPSTR lpszConfId, u8 byBufLen ) const
{
	u8	byLoop;

	for( byLoop = 0; byLoop < sizeof( m_abyConfId ) && byLoop < ( byBufLen - 1 ) / 2; byLoop++ )
	{
		sprintf( lpszConfId + byLoop * 2, "%.2x", m_abyConfId[byLoop] );
	}
	lpszConfId[byLoop * 2] = '\0';
}

/*====================================================================
    ������      ��SetConfId
    ����        �����û��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const u8 * pbyConfId, ���������ָ��
			      u8 byBufLen, BUFFER��С
    ����ֵ˵��  ��ʵ�����������С
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/31    1.0         LI Yi         ����
	2005/01/06	4.0			�ű���		  ��3.6��ֲ����
====================================================================*/
inline u8 CConfIdVer36::SetConfId( const u8 * pbyConfId, u8 byBufLen )
{
	u8	bySize = min( byBufLen, sizeof( m_abyConfId ) );
	
	memset( m_abyConfId, 0, sizeof( m_abyConfId ) );
	memcpy( m_abyConfId, pbyConfId, bySize );

	return( bySize );
}

	
/*====================================================================
    ������      ��SetConfId
    ����        �����û��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����LPCSTR lpszConfId, ����Ļ�����ַ���
    ����ֵ˵��  ��ʵ�����������С
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/31    1.0         LI Yi         ����
	2005/01/06	4.0			�ű���		  ��3.6��ֲ����
====================================================================*/
inline u8 CConfIdVer36::SetConfId( LPCSTR lpszConfId )
{
	LPCSTR	lpszTemp = lpszConfId;
	char	achTemp[3], *lpszStop;
	u8	byCount = 0;

	memset( m_abyConfId, 0, sizeof( m_abyConfId ) );
	while( lpszTemp != NULL && lpszTemp + 1 != NULL && byCount < sizeof( m_abyConfId ) )
	{
		memcpy( achTemp, lpszTemp, 2 );
		achTemp[2] = '\0';
		m_abyConfId[byCount] = ( u8 )strtoul( achTemp, &lpszStop, 16 );
		byCount++;
		lpszTemp += 2;
	}

	return( byCount );
}
#ifdef WIN32
#pragma pack( pop )
#endif

#endif  // _MASTRUCT36_H_
