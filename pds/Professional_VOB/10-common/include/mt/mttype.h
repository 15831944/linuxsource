

#ifndef _MT_TYPE_H_
#define _MT_TYPE_H_
#include "osp.h"


enum EmWiFiLinkStat
{
	emWiFiIdle = 0,
	emWiFiConnecting,
    emWiFiConnectFailed,
    emWiFiConnected,
    emWiFiDisconnecting,
    emWiFiDisconnected
};


// enum EmWiFiLinkStatus
// {
// 	emWiFiIdle = 0,
//     emWiFiConnected,
//     emWiFiConnecting,
//     emWiFiConnectFailed,
//     emWiFiDisconnected,
//     emWiFiDisconnectFailed,
//     emWiFiDevicePullIn,
//     emWiFiDevicePullOut
// };

enum EmWiFiSelType
{
    emWiFiCfgSel = 0,
    emWiFiScanSel
};

enum EmWiFiNetType
{
    emWiFiAdHoc = 0,
    emWiFiInfra
};

enum EmWiFiEncryptType
{
    emWiFiEncryptNone = 0,
    emWiFiEncryptWep,
    emWiFiEncryptWpa,
    emWiFiEncryptWpa2
};

enum EmWiFiEncryptArithmetic
{
	emWiFiArithNone = 0,
	emWiFiArithWep40,
	emWiFiArithTkip ,
	emWiFiArithWrap,
	emWiFiArithAes,						/* aes */
	emWiFiArithWep104	
};

// VOD REJ Code 
//------------------------��Ϣ��REJ��-------------------------------------------
enum EmVODRejCode 
{	
	emVODNoReason = 0, // δ֪����
	emVODFileNotFound = 1, // �ļ�δ�ҵ�
	emVODPalyNotStarted = 2, //����δ��ʼ
	emVODMaxUser = 3, // �����û��Ѵﵽ�����������
	emVODUserNotExist = 4, // �û���������	
	emVODUserLogined = 5, // ���û��Ѿ���¼
	emVODInvalidedPassword = 6, // �������
	emVODMEDIAVODInvalidedFileNameType = 7, // Client <-- Server ,�ļ���Ϊ�ջ���ASF�ļ�
	emVODPlayBusy = 9, // Client <-- Server ,���ڷ�����һ�ε���������
	emConnectFailed = 10,
	emDisconnect = 11 ,// �����ж�
	emVODNoError      //�����ɹ�
};

//VOD ״̬
enum EmVODState
{
	emVODIdle         ,//����
	emVODLogin        ,//���ڵ�½
	emVODFileListQuery,//���ڻ�ȡ�ļ��б�
	emVODFileInfoQuery ,//���ڲ�ѯ�����ļ���Ϣ
	emVODPlay  ,//���ڲ���
	emVODPause    ,//��ͣ
	emVODStop  ,   //ֹͣ
	emVODSeek  ,   //ָ��λ�ò���
	emVODActiveEnd
};

//���
enum EmTextAlign
{
	emAlignLeft = 0,//����
	emAlignCenter,//����
	emAlignRight//����
};

enum EmRollMode
{
	emStatic = 0,
	emRight2Left,
	emDown2Up
};

enum EmRollSpeed
{
	emSlower = 0,
	emSlow,
	emNormal,
	emFast,
	emFaster
};

//����ֹͣλ����
enum EmStopBits
{
    em1StopBit =0,
    em1HalfStopBits=1,
    em2StopBits = 2
};



//����У��λ����
enum EmParityCheck
{
    emNoCheck = 0 ,//��У��
    emOddCheck =1 ,//��У��
    emEvenCheck =2,//żУ��
};

//��������
enum EmSerialType
{
    emRS232 = 0,
    emRS422 = 1,
    emRS485 = 2,
	emSerialTcpip =3 
};

//��������
enum EmLanguage
{
    emEnglish  = 0,
    emChineseSB = 1,
	emLangeEnd
};




//Э������
enum EmConfProtocol
{
	emH323  = 0, 
	emH320  = 1,
	emSIP   = 2,
	emInvalidConfProtocel 
};

//���뷽ʽ
enum EmEncodeMode
{
    emQualityFirst  = 0, 
    emSpeedFirst    = 1,
    
};

//�����ָ�����
enum EmLostPackageRestore
{
    emLPQualityFirst  = 0, 
    emLPSpeedSlow     = 1,
    emLPSpeedNormal   = 2,
    emLPSpeedFast     = 3,
};

//��������
enum EmMatrixType
{
	emMatrixInner   = 0, 
	emMatrixOuter   = 1,
	emMatrixVas     = 2,
};

//˫����ʾģʽ
enum EmDualVideoShowMode
{
    emDualVideoOnSigleScreen  =0,//����˫��
    emEachVideoOwnScreen      =1//˫��˫��
};

//��ʾ����
enum EmDisplayRatio
{
	emDR4to3      = 0, //4:3
	emDR16to9     = 1, //16:9
};

//��Ƶ����
enum EmVideoType
{
   emPriomVideo = 0 ,//����Ƶ
   emSecondVideo = 1//�ڶ�·��Ƶ
};


//��Ƶ��ʽ����
enum EmVideoStandard
{
    emPAL  =0 ,
    emNTSC = 1 ,
//    emSECAM = 2
};

//��ƵԴ����
enum EmVideoInterface
{
    emVGA    = 0, 
    emSVideo = 1,
    emCVideo = 2,
};

//�ն���Ƶ�˿�
enum EmMtVideoPort
{

	emMtVGA   =0 , //VGA 
	emMtSVid    , //S ����
	emMtPC      , //PC
	emMtC1Vid   ,  //C1����
	emMtC2Vid   ,  //C2����
	emMtC3Vid   ,  //C3����
	emMtC4Vid   ,  //C4����
	emMtC5Vid   ,  //C5����
	emMtC6Vid   ,    //C6����
	emMtExternalVid = 10,//���þ���ӳ���ʼֵ
	emMtVideoMaxCount = 74, //���֧����ƵԴ����
	emMtVidInvalid = 255 
};

//Ӧ��ʽ����
enum EmTripMode
{
    emTripModeAuto      =0,  //�Զ�
    emTripModeManu      =1,  //�ֶ� 
	emTripModeNegative  =2,   //�ܾ����ر�
};

//̨������
enum EmLabelType
{
    emLabelAuto    =0,   //�Զ�
    emLabelUserDef =1,  //�Զ���
    emLabelOff     =2  //�ر�
};

//E1��·�������
enum EmDLProtocol
{
    emPPP   = 0, 
    emHDLC  = 1,
    emPPPOE = 2,
    emMP    = 3
};

//E1��֤����
enum EmAuthenticationType
{
    emPAP  = 0,
    emCHAP = 1
};

//֡�ʵ�λ����
enum EmFrameUnitType
{
    emFrameSecond =0,
    emSecondFrame =1
};

// ý������
enum EmMediaType
{
	emMediaVideo = 1, //��Ƶ
	emMediaAudio = 2, //��Ƶ
	emMediaAV    = 3, //��Ƶ����Ƶ
};


//��ƵЭ������
enum EmVideoFormat
{
    emVH261     = 0,
    emVH262     = 1,//MPEG2
    emVH263     = 2,
    emVH263plus = 3,
    emVH264     = 4,
    emVMPEG4    = 5,

	emVEnd		
};

//��ƵЭ������
enum EmAudioFormat
{
    emAG711a  = 0,
    emAG711u  = 1,
    emAG722   = 2,
    emAG7231  = 3,
    emAG728   = 4,
    emAG729   = 5,
    emAMP3    = 6,
	emAG721   = 7,
	emAG7221  = 8,

	emAMpegAACLC =9, 	//xjx_080315, ����֧�ֵ���Ƶ
	emAMpegAACLD = 10,
	emAG719   = 11, //ruiyigen 20091012

	emAEnd	 
};


//�ֱ�������
enum EmVideoResolution
{
    emVResolutionAuto = 0,//�Զ�
    emVSQCIF = 1,
	emVQCIF  = 2,
    emVCIF   = 3,
    emV2CIF  = 4,
    emV4CIF  = 5,
    emV16CIF = 6,
		
	emVGA352x240,
	emVGA704x480,
	emVGA640x480,
	emVGA800x600,
	emVGA1024x768,
	emVGA1280x1024,   //�¼�, SXGA
	emVGA1600x1200,   //UXGA
	
	emVSQCIF112x96,
	emVSQCIF96x80,
	
	emVHD720p1280x720,  //����
	emVHD1080p1920x1080, 
	emVHD1080i1920x1080,
	
	//xjx_080412, �¼�
	emVHD480i720x480,
	emVHD480p720x480,
	emVHD576i720x576,
	emVHD576p720x576,	

	emVResEnd,

	//sfqian_081216 �Ǳ�
	emV320x192,
	emV432x240,
	emV480x272,
	emV640x368,
	emV864x480,
	emV960x544,
	emV1440x816,
};

//QOS���ͷ���
enum EmQoS
{
   emDiffServ  = 0,      //���ַ���
   emIPPrecedence  = 1  //IP����
};

//TOS ����
enum EmTOS
{
	emTOSNone        = 0,//��ʹ��
	emTOSMinCost     = 1,//��С����
	emTOSMaxReliable = 2,//��߿ɿ���
	emTOSMaxThruput  = 3,//���������
	emTOSMinDelay    = 4,//��С�ӳ�
};


//λ��
enum EmSite
{
    emLocal = 0,  //����
    emRemote    //Զ��
};

//����10��ͨ��
enum EmChanType
{
	emChanSendAudio,
	emChanSendPrimoVideo,
	emChanSendSecondVideo,
	emChanSendFecc,
	emChanSendT120,

	//����ͨ��
	emChanRecvAudio,
	emChanRecvPrimoVideo,
	emChanRecvSecondVideo,
	emChanRecvFecc,
	emChanRecvT120,

	emChanTypeEnd	
};

//���������������
enum EmCodecComponent
{
	emPriomVideoEncoder  =0,
	emPriomVideoDecoder  ,

	emSecondVideoEncoder ,
	emSecondVideoDecoder ,

	emAudioEncoder ,
	emAudioDecoder ,
	emCodecComponentEnd,
};

//�ն��ͺ�
enum EmMtModel
{
	emUnknownMtModel=0,
	emPCMT  =1    ,//�����ն�
	em8010      ,
	em8010A     ,
	em8010Aplus ,//8010A+
	em8010C     ,
	em8010C1    ,//8010C1
	emIMT       ,
	em8220A     ,
	em8220B     ,
	em8220C     ,
	em8620A     ,
	emTS6610E   ,
	emTS6210    ,
	em8010A_2   ,
	em8010A_4   ,
	em8010A_8   ,
	em7210      ,
	em7610      ,
	emTS5610    ,
	emTS6610    ,
	em7810      ,
	em7910      ,
	em7620_A    ,
	em7620_B    ,
	em7820_A	,
	em7820_B	,
	em7920_A	,
	em7920_B	

};
// << �ļ�ϵͳ���� >>
enum EmFileSys
{
	emRAWFS=1,//raw file system [ramdisk + rawblock + tffs]
	emTFFS  //tffs
};
//�����㷨
enum EmEncryptArithmetic
{
	emEncryptNone = 0,
	emDES         = 1,
	emAES         = 2,
	emEncryptAuto = 3,

	emEncryptEnd
	
};

//����
enum EmAction
{
    emStart,
    emStop,
    emPause,
    emResume,
};

//�ն�ʹ�ܲ���
enum EmOptRet
{
	emEnable,
	emDisable,
};


//����ģʽ
enum EmConfMode
{
	emP2PConf = 0,//��Ե����
	emMCCConf = 1 //������
};


//��ַ����
enum EmMtAddrType
{
	emIPAddr =0,
	emE164   ,
	emH323id ,
	emDialNum,
	emSipAddr	
};

//����ģʽ
enum EmCallMode
{
	emJoin,
	emCreate,
	emInvite
};


//���Ʒ���
enum EmDirection
{
	emUP   =0,
	emDown   ,
	emLeft   ,
	emRight 
};

//����״̬
enum EmCallState
{
	emCS_Idle  ,
	emCS_Calling ,      //���ڷ������
	emCS_P2P ,      //��Ե����
	emCS_MCC ,      //������
	emCS_Hanup        //�Ҷ�
};


//����״̬
enum EmCallSiteState
{
	emCSS_IDLE,
	emCSS_Init ,
	emCSS_Waiting ,      //���ڷ������
	emCSS_Connected ,      //��Ե����
	emCSS_Failed
};

//����ģʽ
enum EmTransMode
{
	emUnicast,  //����
	emBroadcast, //�㲥
	emMulticast //�鲥
};

//˫��Դ����
enum EmDualSrcType
{
    emDualVGA = 0,
	emDualVideo = 1,
	emDualPC = 2
};



enum EmMtInstType
{
	emMtService = 0 ,
	emMtUI          ,
	emMtConsole     ,
	emMtH323Service ,
	emMtH320Service ,
	emMtSipService  ,
	emMtH323StackIn ,
	emMtH323StackOut,
	emMtMP          ,
	emMtDevice      ,
	emMtAgent       
};

//����ϳɷ����
enum EmVMPStyle
{
    emVMPDynamic       =   0 ,  //��̬����(���Զ��ϳ�ʱ��Ч)
    emVMPOne         =   1 ,  //һ����
    emVMPVTwo        =   2 ,  //�����棺���ҷ� 
    emVMPHTwo        =   3 ,  //������: һ��һС
    emVMPThree       =   4 ,  //������
    emVMPFour        =   5 ,  //�Ļ���
    emVMPSix         =   6 , //������ 
    emVMPEight       =   7 ,  //�˻���
    emVMPNine        =   8 ,  //�Ż���
    emVMPTen         =   9 ,  //ʮ����
    emVMPThirteen    =   10,  //ʮ������
    emVMPSixteen     =   11,  //ʮ������
    emVMPSpecFour    =   12,  //�����Ļ��� 
    emVMPSeven       =   13,  //�߻���
	emVMPSpecThirteen  =   14,  //����ʮ�����棨���ڻ�ΪMCU��
	emVMPTwenty      =   15,  //����MPU�Ķ�ʮ���棨ֻ���ڸ���MPU��
};


//����ϳɳ�Ա���Ͷ���
enum EmVMPMmbType
{
    emVMPMmbMCSspec    = 1,  //���ָ�� 
    emVMPMmbSpeaker    = 2,  //�����˸���
    emVMPMmbChairman   = 3,  //��ϯ����
    emVMPMmbPoll       = 4,  //��ѯ��Ƶ����
    emVMPMmbVAC        = 5,	//��������(��ز�Ҫ�ô�����)
};

//����Ϣҵ������
enum EmSMSType//MS
{
	emSMSSingleLine  = 0,  //����Ϣ
	emSMSPageTitle   = 1,  // ��ҳ��Ļ
	emSMSRollTitle   = 2,  // ������Ļ
	emSMSStaticTitle = 3,  // ��̬��Ļ
};


enum EmMtFile
{
    emMtFileBegin  = 0,//�ն��ļ����Ϳ�ʼ
	emMtConfigFile   =1,//�����ļ�
	emMtOldConfigBakFile     ,//�������ļ�����
	emMtDebugConfiFile, //���������ļ�
	emMtCameraCommandFile,//����ͷ�����ļ�
	emMtCameraPresetPostionFile,//����ͷԤ��λ�����ļ�
	emMtExternMaxtrixCommandFile,//���þ����������ļ�
	emMtExternMaxtrixPortNameFile,//���þ���˿����ļ�
	emMtInnerMaxtrixSchemeFile,//���þ��󷽰��ļ�
	emMtStreamMediaHtmlFile,//��ý����ҳ�ļ�
	emMtResourceFile,//�ն���Դ�ļ�
	emMtZipResourceFile,//ѹ���ն���Դ�ļ�
	emMtSymbolFile,//�ն�̨��ͼƬ�ļ�
	emMtBannerFile,//�ն˺���ļ�
	emMtStaticPicFile,//�ն˾�̬ͼƬ�ļ�
	emMtResInBin,//Bin�ļ��е���Դ�ļ�
	emMtAddressBookEx,//��չ��ַ���ļ�
	emMtUpdateSysFile,//�汾�����ļ�
	emMtMinitorFile, //�����ļ�
	emMtZipFile,//Ӧ�ó���
	emMtHintFile,//������ʾ��Դ�ļ�
	emMtKeyFile, //mclicense�ļ�
    emMtInnerPxyFile,//���ô�������������ļ�
	emMtFileEnd //�ն��ļ����ͽ���

};
typedef u32   u32_ip;

enum EmCallRate
{
	emRaten64 = 0,							/*(0)64kbps*/
	emRaten2m64,							/*(1)2��64kbps*/
	emRaten3m64,							/*(2)3��64kbps*/
	emRaten4m64,							/*(3)4��64kbps*/
	emRaten5m64,							/*(4)5��64kbps*/
	emRaten6m64,							/*(5)6��64kbps*/
	emRater384,							/*(6)384kbps*/
	emRater1536,							/*(7)1536kbps*/
	emRater1920,							/*(8)1920kbps*/
	emRater128,							/*(9)128kbps*/
	emRater192,							/*(10)192kbps*/
	emRater256,							/*(11)256kbps*/
	emRater320,							/*(12)320kbps*/
	emRater512,							/*(13)512kbps*/
	emRater768,							/*(14)768kbps*/
	emRater1152,							/*(15)1152kbps*/
	emRater1472,							/*(16)1472kbps*/
	/*(ע��V2.0�汾������3M��4M��6M��8M����)*/
	emRater3M,							/*(17)3M(2880kbps)*/
	emRater4M,							/*(18)4M(3840kbps)*/
	emRater6M,							/*(19)6M(5760kbps)*/
	emRater8M,							/*(20)8M(7680kbps)*/
	emRaterLastOne
};


//PC windows �ɼ���ʽ
enum EmPCCapFormat
{
	emCapPCFrameBMP  = 0,//RGB24λͼ;
	emCapPCFrameUYUY = 1,//yuv4:2:2��ʽ
	emCapPCFrameI420 = 2,//yuv4:2:0��ʽ
};

//���л���ʾģʽ
enum EmPiPMode
{
	emPiPClose   = 0,//���л��ر�
	emPiPRightBottom ,//���л�С������ʾ�����½�
	emPiPLeftBottom  ,//���л�С������ʾ�����½�
	emPiPLeftTop     ,//���л�С������ʾ�����Ͻ�
	emPiPRightTop    ,//���л�С������ʾ�����Ͻ�
};
// DVB Component
enum EmDVBComponent
{
	emDVBEncoder = 0,
	emDVBPriomVideoDecoder,
	emDVBSecondVideoDecoder
};
enum EmUIPosion
{
	emMT = 0,
	emMTC = 1
};
//���йҶ�ԭ��
enum EmCallDisconnectReason
{
	emDisconnect_Busy = 1   ,//�Զ�æ
	emDisconnect_Normal     ,//�����Ҷ�
	emDisconnect_Rejected   ,//�Զ˾ܾ�
	emDisconnect_Unreachable ,//�Զ˲��ɴ�
	emDisconnect_Local       ,//����ԭ��
	emDisconnect_Nnknown      //δ֪ԭ��
};


//MC ��ģʽ
enum EmMCMode
{
	emMcSpeech  ,//�ݽ�ģʽ
	emMcDiscuss ,//����ģʽ
	emMcModeEnd
};


//��Ƕmc�Ļ���ϳɷ��
enum EmInnerVMPStyle
{
    emInnerVMPAuto = 0,
	emInnerVMPOne,
	emInnerVMPTwo,
	emInnerVMPThree,
	emInnerVMPFour,
	emInnerVMPFive,
	emInnerVMPSix,
	emInnerVMPTwoBigLittle,
	emInnerVMPThreeBigLittle,
	emInnerVMPFourBigLittle,
	emInnerVMPEnd
};

//��Ƕmc�Ľ����������
enum EmVMPComponent
{
	emVMPDecNull = 0,
	emVMPDec1th,
	emVMPDec2th,
	emVMPDec3th,
	emVMPDec4th,
	emVMPDec5th,
	emVMPDecLocal,
	emVMPEnd
};


//pcmt��ƵԴ����
enum EmCapType
{
   emAudioOnly = 1, //
   emCameraOnly,
   emACBoth,
   emDesktopOnly,
   emADBoth,
   emFileAVBoth  
};
//pppoe״̬
enum EmPPPoEState
{
	emFree,
	emLinking,
	emLinkup,
	emLinkdown,
	emUserError,
	emTimeOut,
	emAgentError
};

enum EmDhcpState
{
	emDhcpFree,
	emDhcpLinkup,
	emDhcpLinkdown,
};

enum EmAddrGroupOptType
{
    emCallIn,
	emCallOut,
	emUserDef,
	emTemplate,
	emSiteCall,
	emMissed,
	emUserDefExt0,
	emUserDefExt1,
	emUserDefExt2,
	emUserDefExt3,
	emUserDefExt4,
	emUserDefExt5,
	emInvalid
};

enum EmHotKeyType
{
	emApplySpeak,
	emApplyChair,
	emPrevPage,
	emNextPage,
	emBack,
	emLoop,
	emHotkeyEnd,
	emAEC,
	emDial,
};

enum EmMtOSType
{
	emWindows,
    emVxworks,
	emLinux
};

enum EmFxoState
{
    emFxoIdle,
    emFxoCalling, 
    emFxoConnect,
};

// ������������С����
enum EmMtLoudspeakerVal
{
	emLoudspeakerLow = 0,
	emLoudspeakerMiddle,
	emLoudspeakerHigh,
};

// �ش�ʱ��ѡ�����������
enum EmNetType
{
	emInternet = 0,
	emSpecial,
	emVPN,
};

//T2�ն�ϵͳ����״̬��־
enum EmSysRunSuccessFlag
{	
	emSysNoUpdateOperation = 0,
	emSysUpdateVersionRollBack,
	emSysUpdateVersionSuccess,
	emSysSetUpdateFlagFailed,
};

//mcu֪ͨ�ն˵���Ϣ����  
enum EmMcuNtfMsgType
{
	emMsgNoneNtf =0,
	emMsgBePolledNextNtf,
	
};




struct TDATAINFO
{
	//��������
	int type ;
	//�������ʹ�С
	int size ;
	//���ݸ���
	int arraysize;
	//���ݵ�ַƫ����
	int offset;
	//��Ա��������
	char* member;
};

struct TENUMINFO
{
	char* descrip;
	int   val;
};

// �ն�Ӳ���Զ����������� add by wangliang 2007/02/05
enum EmAutoTestType 
{
    emAutoTestTypeBegin,            //�������Ϳ�ʼ
        
    emAutoTestFXOFunc,              //�绰�ӿڲ��ԣ�FXO��·����
    emAutoTestSerialFunc,           //���ڹ��ܲ���
    emAutoTestEthFunc,              //���ڹ��ܲ���
    emAutoTestUSBFunc,              //USB���ܲ���
    emAutoTestAudioLoopBackFunc,    //��Ƶ���ع��ܲ���
    emAutoTestVideoLoopBackFunc,    //��Ƶ���ع��ܲ��� - C����  
    emAutoTestLoudspeakerFunc,      //���������ܲ���
    emAutoTestMICFunc,              //MIC���ܲ���
    emAutoTestCameraRotaFunc,       //����ͷת������
    emAutoTestRTCFunc,              //RTC���ܲ���
    emAutoTestLightFunc,            //ָʾ�Ʋ���
    emAutoTestRemoteCtrlFunc,       //����ӿڹ��ܲ���
    emAutoTestOtherFunc,            //�������ܲ���
    emAutoTestVideoLoopBackVgaFunc, //��Ƶ���ع��ܲ��� - VGA
    emAutoTestSetDefault,           //�ָ����Ա�־Ĭ��ֵ
    emAutoTestLoadFile,             //��Ƭ���ļ�����
    
    emAutoTestE1Func,               //E1���ܲ���

    emAutoTestSDIInSDIOutFunc,      //��������������ԣ�SDI���룬SDI���
    emAutoTestSDIInHDMIOutFunc,     //��������������ԣ�SDI���룬HDMI���
    emAutoTestSDIInYprPb1OutFunc,   //��������������ԣ�SDI���룬YprPb1���
    emAutoTestSDIInYprPb2OutFunc,   //��������������ԣ�SDI���룬YprPb2���
    
    emAutoTestHDMIInSDIOutFunc,      //��������������ԣ�HDMI���룬SDI���
    emAutoTestHDMIInHDMIOutFunc,     //��������������ԣ�HDMI���룬HDMI���
    emAutoTestHDMIInYprPb1OutFunc,   //��������������ԣ�HDMI���룬YprPb1���
    emAutoTestHDMIInYprPb2OutFunc,   //��������������ԣ�HDMI���룬YprPb2���

    emAutoTestYprPb1InSDIOutFunc,    //��������������ԣ�YprPb1���룬SDI���
    emAutoTestYprPb1InHDMIOutFunc,   //��������������ԣ�YprPb1���룬HDMI���
    emAutoTestYprPb1InYprPb1OutFunc, //��������������ԣ�YprPb1���룬YprPb1���
    emAutoTestYprPb1InYprPb2OutFunc, //��������������ԣ�YprPb1���룬YprPb2���
    
    emAutoTestYprPb2InSDIOutFunc,    //��������������ԣ�YprPb2���룬SDI���
    emAutoTestYprPb2InHDMIOutFunc,   //��������������ԣ�YprPb2���룬HDMI���
    emAutoTestYprPb2InYprPb1OutFunc, //��������������ԣ�YprPb2���룬YprPb1���
    emAutoTestYprPb2InYprPb2OutFunc, //��������������ԣ�YprPb2���룬YprPb2���
    
    emAutoTestVGAInVGAOutFunc,       //VGA���ܲ���

    emAutoTestAudioCPortInCPortOut, //������Ƶ����������ܲ���:c��������,c�������
    emAutoTestAudioCPortInHDMIOut,  //������Ƶ����������ܲ���:c��������,HDMI���
    emAutoTestAudioHDMIInCPortOut,  //������Ƶ����������ܲ���:HDMI����,c�������
    emAutoTestAudioHDMIInHDMIOut,   //������Ƶ����������ܲ���:HDMI����,HDMI���
    emAutoTestTypeEnd               //�������ͽ���
};

enum EmGKRegFailedReason
{
	emNone,
	emGKUnReachable,
	emInvalidAliase,
	emDupAlias,
	emInvalidCallAddress,
	emResourceUnavailable,
	emUnknown
};

enum HD_PORT_MASK
{
	//PORT_HDMI =  0x01,
	//PORT_SDI =   0x02,
	PORT_SDI =  0x01,
	PORT_HDMI =   0x02,	
	PORT_YPrPb1 = 0x04,
	PORT_YPrPb2 = 0x08,	
	PORT_VGA = 0x10,
	PORT_C0 = 0x20,
};

enum EmHDAudPort
{
	emAudHDMI0 = 0,
	emAudC0
};

//////////////////////////////////////////////////////////////////////////

#endif

