/*****************************************************************************
  ģ����      : Codecwrapper_HD.a
  �ļ���      : codecwrapperdef_hd.h
  ����ļ�    : codecwrapper_hd.h
  �ļ�ʵ�ֹ���: 
  ����        : ������
  �汾        : V4.0  Copyright(C) 2007-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
2007/11/29  4.0         ������      ����
******************************************************************************/
#ifndef _CODECWRAPPER_DEF_HD_H_
#define _CODECWRAPPER_DEF_HD_H_

#ifdef _MEDIACTRL_DM816x_

#include "dm816x/codecwrapperdef_hd.h"

#else
//CPU��ʶ
#define CWTI_CPUMASK_MAIN   0x00000001
#define CWTI_CPUMASK_SUB    0x00000002
#define CWTI_CPUMASK_ALL    0x00000003

//������
#define ERROR_CODE_BASE             30000             
#define CODEC_NO_ERROR              0//�޴�

#include "kdvtype.h"
#include "osp.h"

//��ʼ��ʱָ������������
enum enBoardType
{
	en_H600_Board = 1,
	en_H700_Board,
	en_H900_Board,
	en_HDU2_Board,
	en_MPU2_Board,
	en_APU2_Board,
	en_HDU2_Board_S,
	en_Invalid_Board,
};

enum enCodecErr
{
	Codec_Success = CODEC_NO_ERROR,

	Codec_Error_Base = ERROR_CODE_BASE,
	Codec_Error_Param,
	Codec_Error_CPUID,
	Codec_Error_Codec_NO_Create,
	Codec_Error_Audio_CapInstance,
	Codec_Error_Audio_PlyInstance,
	Codec_Error_CreateThread,
	Codec_Error_FastUpdateTooFast,
	Codec_Error_WriteToDSP,
	Codec_Error_CreateHPIChannel,
	Codec_Error_GetMem,
	Codec_Error_GetBMP
};

#define CODECMAXCHANNELNUM         4
#define MAX_VIDENC_CHANNEL         CODECMAXCHANNELNUM
#define MAX_VIDDEC_CHANNEL         CODECMAXCHANNELNUM
#define MAX_AUDENC_CHANNEL         CODECMAXCHANNELNUM
#define MAX_AUDDEC_CHANNEL         CODECMAXCHANNELNUM

#define MAX_NUMLOGO_NUM        4
#define MAX_NOMALLOGO_NUM      16
#define MAX_BMP_NUM            32
#define MIN_BMP_LOGO_ID        1

//Audio mode
enum
{
	AUDIO_MODE_MP3 = 4,
	AUDIO_MODE_PCMA = 5,
	AUDIO_MODE_PCMU,
	AUDIO_MODE_G723_6,
	AUDIO_MODE_G723_5,
	AUDIO_MODE_G728,
	AUDIO_MODE_G722,
	AUDIO_MODE_G729,
	AUDIO_MODE_G719,
	AUDIO_MODE_G7221,
	AUDIO_MODE_ADPCM,
    AUDIO_MODE_AACLC_32,     //32K ˫����
    AUDIO_MODE_AACLC_32_M,   //32K ������
    AUDIO_MODE_AACLC_48,     //48K ˫����
    AUDIO_MODE_AACLC_48_M,   //48K ������
    AUDIO_MODE_AACLD_32,
    AUDIO_MODE_AACLD_32_M,
    AUDIO_MODE_AACLD_48,
    AUDIO_MODE_AACLD_48_M
};

//��Ƶ����������
typedef enum
{
    SAMPLE_RATE96000 = 0, //������Ϊ96kHz��������0
    SAMPLE_RATE88200,     //������Ϊ88.2kHz��������1
    SAMPLE_RATE64000,     //������Ϊ64kHz��������2
    SAMPLE_RATE48000,     //������Ϊ48kHz��������3
    SAMPLE_RATE44100,     //������Ϊ44.1kHz��������4
    SAMPLE_RATE32000,     //������Ϊ32kHz��������5
    SAMPLE_RATE24000,     //������Ϊ24kHz��������6  
    SAMPLE_RATE22050,     //������Ϊ22.05kHz��������7
    SAMPLE_RATE16000,     //������Ϊ16kHz��������8
    SAMPLE_RATE12000,     //������Ϊ12kHz��������9
    SAMPLE_RATE11025,     //������Ϊ11.025kHz��������10 
    SAMPLE_RATE8000       //������Ϊ8kHz��������11
};

/* �����������*/
enum
{
	VID_DROP_PREFERRED_QUALITY   =   0, 
	VID_DROP_PREFERRED_FAST_SPEED,   
	VID_DROP_PREFERRED_MID_SPEED,
	VID_DROP_PREFERRED_SLOW_SPEED
};

/*��������������λ��*/
enum
{
    VID_CODECTYPE_FPGA = 0,
    VID_CODECTYPE_DSP,
    VID_CODECTYPE_UNKNOW
};

/* ��Ƶ��������ӿں궨�� */
#define VIDIO_HDMI0                 0x00000001
#define VIDIO_HDMO1					0x00000002
#define VIDIO_VGA0                  0x00000010
#define VIDIO_YPbPr0                0x00000100
#define VIDIO_YPbPr1                0x00000200  /* ��KDV7180�ϸýӿں�VIDIN_VGA0��ͻ��ֻ��2ѡ1 */
#define VIDIO_SDI0                  0x00001000
#define VIDIO_C0                    0x00010000


/*ͼ�����*/
enum
{
    VIDCAP_SCALE_BRIGHTNESS = 0, /* �������ȣ�  C | YPbPr */
    VIDCAP_SCALE_CONTRAST,       /* ���öԱȶȣ�C | YPbPr */
    VIDCAP_SCALE_HUE,            /* ����ɫ�ȣ�  C */
    VIDCAP_SCALE_SATURATION      /* ���ñ��Ͷȣ�C | YPbPr */
};

/*SD�źŹ̶������ʽ*/
enum
{
	SDOUTTYPE_576I = 0,     //�̶���576i���
	SDOUTTYPE_720P,         //�̶���720p���
	SDOUTTYPE_1080I         //�̶���1080i���
};

/* ����������豸ID*/
enum
{
	CODECDEV_VIDENC = 0,     //��Ƶ�����豸
	CODECDEV_VIDDEC,         //��Ƶ�����豸
	CODECDEV_AUDDEV          //��Ƶ������豸
};

/* VGA��1080I��1080P���Ƶ��ѡ��*/
#define  VIDPLYFREQ_AUTO             0     //Ĭ���Զ�����
#define  VIDPLYFREQ_24FPS            24    //����1080P
#define  VIDPLYFREQ_25FPS            25    //����1080P
#define  VIDPLYFREQ_30FPS            30    //����1080P
#define  VIDPLYFREQ_50FPS            50    //����1080I
#define  VIDPLYFREQ_60FPS            60	   //����1080I��VGA
#define  VIDPLYFREQ_75FPS            75	   //����VGA
#define  VIDPLYFREQ_1080POUT1080I    0xFF  //����1080P��1080I���

/*������GroupID*/
/*ע:A��Bģʽͨ�����ɽ��洴��*/
enum
{
	ADAPTER_MODEA_NOR = 0,     //Aģʽ����ͨ����ͨ��
	ADAPTER_MODEA_VGA,         //Aģʽ��VGA����ͨ��
	ADAPTER_MODEB_H264ToH263,		   //Bģʽ��H264ToH263ͨ��
	ADAPTER_MODEB_H263ToH264           //Bģʽ��H263ToH264ͨ��
};

//�豸��ʼ��ģʽ
enum enInitMode
{
    INITMODE_All_VMP = 0,           //���嵥VMPģʽ���൱�ڵ��� HardMPUInitDev(1,2,4)
    INITMODE_HALF_VMP,              //����˫VMPģʽ���൱�ڵ��� HardMPUInitDev(2,2,4)
    INITMODE_EVMP,                  //���嵥VMPģʽ���൱�ڵ��� HardMPUInitDev(1,2,4)
    INITMODE_HDBAS,                 //��������ģʽ���൱�ڵ���  HardMPUInitDev(2,2,4)
	INITMODE_HD2BAS,				//���ݸ���2�ն�1080p60��������ģʽ��ͬINITMODE_HDBAS
    INITMODE_EBAS,                  //��������ģʽ���൱�ڵ���  HardMPUInitDev(2,0,4)
    INITMODE_HDU,                   //�������ǽģʽ���൱�ڵ���  HardMPUInitDev(0,1,1)
	INITMODE_HDU_D,					//�������ǽģʽ��HDU_D��
    INITMODE_NUM
};

//HDU����ģʽ��hdu-1 or hdu-2
enum enHduWorkMode
{
    HDU_WORKMODE_INVALID,
    HDU_WORKMODE_1_CHNL,            //ֻ��һ·��/��Ƶ���
    HDU_WORKMODE_2_CHNL                 //��·��/��Ƶ���������ģʽ
};

/*������ʱ����*/
enum enNoStreamBack
{
	NoStrm_PlyBlk = 0,		//��ɫ����(Ĭ��)
	NoStrm_PlyLst = 1,		//������һ֡
	NoStrm_PlyBmp_dft,		//Ĭ��ͼƬ
	NoStrm_PlyBmp_usr,		//�û�����ͼƬ
	NoStrm_Total
};
#define FILE_BMP_DFT_VGA		"/usr/etc/config/dft_vga.bmp"
#define FILE_BMP_DFT_16To9		"/usr/etc/config/dft_16to9.bmp"
#define FILE_BMP_USR_VGA		"/usr/etc/config/usr_vga.bmp"
#define FILE_BMP_USR_16To9		"/usr/etc/config/usr_16to9.bmp"

/* ��Ƶ��������ӿں궨�� */
#define AUDIO_HDMI0                 0x00000001
#define AUDIO_HDMI1					0x00000002
#define AUDIO_LINE0					0x00000010
#define AUDIO_LINE1					0x00000020
#define AUDIO_C0                    0x00010000

/* ����Ϣ�����������ʽ�궨�� */
//������ʽ
#define RUNMODE_STATIC     0    //����Ϣ��֧��
#define RUNMODE_LEFTRIGHT  1    
#define RUNMODE_UPDWON     2    
//�����ٶ�
#define RUNSPEED_FAST      4
#define RUNSPEED_HIGH      3
#define RUNSPEED_NORM      2
#define SPEED_LOW          1

/*�����������Ƶ���VP*/
#define MAKEVPID(nMain,nSub)          (u32)( ((u32)nMain&0x000000FF) | (((u32)(nSub+1)&0x000000FF)<<8) )

//��������������
#define VIDENC_MBSIZE_ALL         (u32)0xFF0FFFFF
#define VIDENC_MBSIZE_P8x8UP      (u32)0xFF0FE0FF
#define VIDENC_MBSIZE_DEFAULT     0  //ĿǰΪ0xFF0FFFFF

#define VIDENC_MBSIZE_ENABLE      VIDENC_MBSIZE_ALL
#define VIDENC_MBSIZE_DISABLE     VIDENC_MBSIZE_P8x8UP
//////////////////////////////////////////////////////////////////////////
//adpter �궨��
#define MAX_VIDEO_ADAPT_CHANNUM       1
#define MAX_AUDIO_ADAPT_CHANNUM       3
#define MAX_ADAPT_CHNNUM              (MAX_VIDEO_ADAPT_CHANNUM+MAX_AUDIO_ADAPT_CHANNUM)

#define MAX_TRANS_VIDEO_ADAPT_CHANNUM       20
#define MAX_TRANS_AUDIO_ADAPT_CHANNUM       20

enum {
	ADAPTER_CODECID_HD1080,
	ADAPTER_CODECID_HD720P,
	ADAPTER_CODECID_SD4CIF,
	ADAPTER_CODECID_SDCIF,
	ADAPTER_CODECID_OTHER1,
	ADAPTER_CODECID_OTHER2
};


#define ADAPTER_CODECID_ENCHD               0
#define ADAPTER_CODECID_ENCSD               1
#define ADAPTER_CODECID_DEC                 10
//////////////////////////////////////////////////////////////////////////

//�ӿڽṹ
typedef struct tagDSPInit
{
	u32     m_dwDspID;          //��ʼ����DSP���
	u32     m_dwLogoNum;        //��(1)��(0)��Ҫ��DSP�ϼ�logo��banner��OSD��
	u32     m_dwNumLogoNum;     //�ݲ�ʹ��
	u32     m_dwAudEncNum;      //��Ƶ����·��
	u32     m_dwAudDecNum;      //��Ƶ����·��
	BOOL32  m_bInitAec;         //�Ƿ�ʹ��AEC
    BOOL32  m_bOutD1;           //�Ƿ�̶����D1
    u32     m_bDspDDRFreq;      //Dsp DDRƵ��(135��162)
}TDSPInit;

typedef struct tagCodecInit
{
	u32     dwDspID;   //Dspid kdv7810(0:videnc, 1:viddec, 2:audio)
	u32     dwChnID;   // 0,1....
	u32     dwVPID;    // 0:��ͨ��DSPֱ�Ӳɼ��� 1:ͨ��DSP���غ�ɼ�
                       /* ��������VP����� ռ�õ�VP�ڣ�һ��chnl���ͬʱ��4��VP�������
                       32λ���Ϊ4��Byte,ÿ��byte��Ӧһ����Ƶ�����:
                       Bit[ 7: 0]Ϊ����Ƶ���: ��Χ0~FPGA_DEC_VP_MAX_NUM-1,�ֱ��ӦVP0/1/2/3��
                       Bit[15: 8]Ϊ����Ƶ���1��
                       Bit[23:16]Ϊ����Ƶ���2��
                       Bit[31:24]Ϊ����Ƶ���3��
                       3������Ƶ����β���[VP��+1]��ָʾ��Ӧ��VP�ţ�
                       0 ��ʾ��Ӧ����Ƶ����رա�
                       ��ʹ��MAKEVPID(nMain,nSub),�����ɣ�nMainΪ��VPID�� nSubΪ��VPID*/

	u32     dwCapPort; //�ɼ��˿�
}
TEncoder, TDecoder;

typedef struct tagVidSrcInfo
{
    u16	    m_wWidth;                     /* ��Ƶ������Ϊ��λ,Ϊ0��ʾ���ź� */
    u16	    m_wHeight;                    /* ��Ƶ�ߣ���Ϊ��λ,Ϊ0��ʾ���ź� */
    BOOL32  m_bProgressive;               /* ���л���У�TRUE=���У�FALSE=���� */
    u32     m_dwFrameRate;                /* ֡�ʣ�����ʱ=��Ƶ������ʱ=��Ƶ/2����60i=30P,Ϊ0��ʾ���ź� */
}
TVidSrcInfo;

typedef struct tagVidFrameInfo
{
	BOOL32    m_bKeyFrame;    //Ƶ֡���ͣ�I or P��
	u16       m_wVideoWidth;  //��Ƶ֡��
	u16       m_wVideoHeight; //��Ƶ֡��
}
TVidFrameInfo;

typedef struct tagFrameHeader
{
	u32     m_dwMediaType;    //��������
	u32     m_dwFrameID;      //֡��ʶ�����ڽ��ն�
	u32     m_dwSSRC;         //ͬ��Դ��Ϣ�����ڽ��ն�
	u8*     m_pData;          //����Ƶ����
	u32     m_dwDataSize;     //���ݳ���
    union
    {
		TVidFrameInfo m_tVideoParam;
        u32           m_dwAudioMode;//��Ƶģʽ
    };
}
TFrameHeader, *PTFrameHeader;

//��Ƶ�������,ֻ��AAC_LC��Ч
typedef struct 
{
    u32 m_dwMediaType;            //��Ƶý������ MEDIA_TYPE_AACLC
    u32 m_dwSamRate;              //��Ƶ������, SAMPLE_RATE32000��
    u32 m_dwChannelNum;           //��Ƶ������, 1��2
}
TAudioDecParam;

typedef struct
{
	u32 dwFrmType;				    //0:���̶�����, 1: half cif, 2: 1 cif, 3: 2 cif,  4: 4 cif,  5: ����ͼ��ϳɱ���
	u32 dwFrameRate;				//֡��
	u32 dwIFrmInterval;				//I֡���
	u32 dwBitRate;					//����
	BOOL32 bFrmRateLittleThanOne;	//����֡��С��1 ?
	BOOL32 bFramRateCanSet;			//����֡���Ƿ����ⲿ�趨? 1:�ǣ�0����
	BOOL32 bAutoDelFrame;			//�������Ƿ��Զ���֡1���ǣ�0����				
}TMpv4FrmParam;


typedef struct 
{
	u32 m_dwReserve;
}
TVideoDecParam;

/*��Ƶ�������*/
typedef struct tagVideoEncParam
{
	u16   m_wVideoWidth;            /*ͼ����(default:1280)*/
	u16	  m_wVideoHeight;           /*ͼ��߶�(default:720)*/
    u16   m_byMaxKeyFrameInterval;  /*I֡�����P֡��Ŀ*/
    u16   m_wBitRate;               /*���������(Kbps)*/

	u8    m_byEncType;              /*ͼ���������*/
    u8    m_byRcMode;               /*ͼ��ѹ�����ʿ��Ʋ���*/    
	u8    m_byMaxQuant;             /*�����������(1-51)*/
    u8    m_byMinQuant;             /*��С��������(1-51)*/

	u8	  m_byFrameRate;            /*֡��(default:60)*/	 
	u8	  m_byFrmRateCanSet;        /*֡���Ƿ�������趨 ?, ��Ч*/
	u8    m_byFrmRateLittleThanOne; /* ֡���Ƿ�С��1 ?, ��Ч*/
	u8    m_byImageQulity;          /*ͼ��ѹ������,0:�ٶ�����;1:��������, ��Ч*/

	u8  m_byFrameFmt;               /*�ֱ���*/ 
	u8  m_byReserved1;              /*����*/
}TVideoEncParam;

/*������״̬*/
typedef struct tagKdvEncStatus
{
	BOOL32 			m_bVideoSignal;			/*�Ƿ�����Ƶ�ź�*/
	BOOL32			m_bAudioCapStart;       /*�Ƿ�ʼ��Ƶ����*/
	BOOL32			m_bEncStart;            /*�Ƿ�ʼ����*/	
}TKdvEncStatus;

/*������ͳ����Ϣ*/
typedef struct tagKdvEncStatis
{
    u32  m_dwFrameRate;  /*֡��*/
    u32  m_dwBitRate;    /*�����ٶ�*/
    u32  m_dwPackLose;   /*��֡��*/
    u32  m_dwPackError;  /*��֡��*/
	u32  m_wAvrBitRate;  /*1��������Ƶ����ƽ������*/
}TKdvEncStatis;



/*������״̬*/
typedef struct tagKdvDecStatus
{
	BOOL32	m_bDecStart;         /*�Ƿ�ʼ����*/	
	u32     m_dwAudioDecType;    /*��������*/
    u32     m_dwHeart;           /*�����߳�����*/
	u32     m_dwPos;             /*�����߳�λ��*/
    u32     m_dwDecType;
}TKdvDecStatus;

/*������ͳ����Ϣ*/
typedef struct tagKdvDecStatis
{
	u16	   m_wFrameRate;           /*����֡��*/
    u16    m_wLoseRatio;           /*��ʧ��,��λ��%*/ 
	u32    m_dwRecvFrame;          /*�յ���֡��*/
	u32    m_dwLoseFrame;          /*��ʧ��֡��*/		
	u32    m_dwPackError;          /*��֡��*/ 
	u32    m_dwIndexLose;          /*��Ŷ�֡*/
	u32    m_dwSizeLose;           /*��С��֡*/
	u32    m_dwFullLose;           /*����֡*/	
	u16	   m_wBitRate;             /*��Ƶ��������*/
	u16    m_wAvrVideoBitRate;     /*1�����ڽ���ƽ������*/
	BOOL32 m_bVidCompellingIFrm;   /*��Ƶǿ������I֡*/								  
	u32    m_dwDecdWidth;	       /*�����Ŀ�*/
	u32    m_dwDecdHeight;         /*�����ĸ�*/
}TKdvDecStatis;

typedef struct tagTranspDColor
{
	u8 RColor;          //R����
	u8 GColor;          //G����
	u8 BColor;          //B����	
	u8 u8Transparency;  //͸����(0��ʾȫ͸����255��ʾ��͸����1~254��ʾ����͸��)
}TTranspColor;

typedef struct BackBGDColor
{
    u8 RColor;    //R����
    u8 GColor;    //G����
    u8 BColor;    //B����	
}TBackBGDColor;       //��Ϊ�ն˼���ʹ��

/* ����������� */
typedef struct
{
	u32 dwXPos;
    u32 dwYPos;//��ʾ�����Y����
    u32 dwWidth;//��ʾ����Ŀ�
    u32 dwHeight;//��ʾ����ĸ�
    u32 dwBMPWight;//BMPͼ���
    u32 dwBMPHeight;//BMPͼ���
    u32 dwBannerCharHeight;//����߶ȣ�����Ҫ�������¹���ʱ��ȷ��ͣ�ٵ�λ��
    TTranspColor tBackBGDColor;//����ɫ 
    u32 dwPicClarity;          //����ͼƬ͸����
    u32 dwRunMode;//����ģʽ������or����or��ֹ���궨�����ϣ�
    u32 dwRunSpeed;//�����ٶ� �ĸ��ȼ����궨�����ϣ�
    u32 dwRunTimes;//�������� 0Ϊ�����ƹ���
 	u32 dwHaltTime;//ͣ��ʱ��(��)�����¹���ʱ��ÿ����һ���ֵ�ͣ�ټ����0ʼ��ͣ�����뾲ֹ��ͬ�� 
}TAddBannerParam;


//////////////////////////////////////////////////////////////////////////
//adapter structs

/* ��Ƶ������� */
typedef struct
{
    u8  byAudioEncMode; /*��������ģʽ*/
    u8  byAudioDuration;/*ʱ��*/
}TAdapterAudioEncParam;

/* ������������� */
typedef union
{
    TVideoEncParam          tVideoEncParam[6];
    TAdapterAudioEncParam   tAudioEncParam;
}TAdapterEncParam;

//����ͨ��
typedef struct
{
    u8 byChnNo;//ͨ����
    u8 byMediaType;//Ŀ����������(����ָ��)
    TAdapterEncParam  tAdapterEncParam; // �������
}TAdapterChannel;

//������״̬
typedef struct
{
    BOOL32     bAdapterStart;      //�����Ƿ�ʼ
    u8     byCurChnNum;  //Ŀǰ���������ͨ����Ŀ
    TAdapterChannel atChannel[6];  //���е�ͨ��
}TAdapterGroupStatus;

//ͨ��ͳ����Ϣ
typedef struct	
{
    u32  dwRecvBitRate;        //��������
    u32  dwRecvPackNum;        //�յ��İ���
    u32  dwRecvLosePackNum;    //������ն�����
    u32  dwSendBitRate;        //��������
    u32  dwSendPackNum;        //���͵İ���
    u32  dwSendLosePackNum;    //���Ͷ�����
    BOOL32 m_bVidCompellingIFrm;	//�Ƿ�Ҫǿ�ƹؼ�֡
#ifdef MAU_VMP
    u32  dwDecChnl;                 //*****��
#endif
}TAdapterChannelStatis;

typedef struct
{
    u32 dwChannelNum;
}TAdapterInit;

////////////////////////////////////////////////////////////////////////////
//

typedef void ( *FRAMECALLBACK)(PTFrameHeader pFrameInfo, void* pContext);
typedef void ( *TDecodeVideoScaleInfo)(u16 wVideoWidth, u16 wVideoHeight);
typedef void ( *TDebugCallBack )(char *pbyBuf, int dwBufLen);
typedef void ( *CHANGECALLBACK)(void* pContext);
typedef void ( *CpuResetNotify)(s32 nCpuType);  //nCpuType��DSP ID��0��1��2 ��
typedef void ( *VIDENCCALLBACK)(TVideoEncParam* pFrameInfo, void* pContext);//gaoden:MainVidEncParam�������֮�ص�(2009-7-8)

#endif
#endif





















