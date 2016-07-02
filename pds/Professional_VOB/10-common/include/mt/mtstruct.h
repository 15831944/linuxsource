/*******************************************************************************
 *  ģ����   : MT                                                              *
 *  �ļ���   : mtstruct.h                                                      *
 *  ����ļ� :                                                                 *
 *  ʵ�ֹ��� : �ն˳������ݽṹ����                                            *
 *  ����     : ������                                                          *
 *  �汾     : V3.0  Copyright(c) 1997-2005 KDC, All rights reserved.          *
 *                                                                             *
 *  ˵��:                                                                      *
 *      ���ļ����岻ͬ�ڵ㡢Ӧ�ü�ͨ���õ����ݽṹ����ṹ���������һ��ģʽ   *
 *    type struct tag�ṹ�� {                                                  *
 *        ��������  ������;                                                    *
 *        ��������  ������;                                                    *
 *     }�ṹ��;                                                                *
 *                                                                             *
 *    =======================================================================  *
 *  �޸ļ�¼:                                                                  *
 *    ��  ��      �汾        �޸���      �޸�����                             *
 *  2005/8/6      4.0         ������      ����                                 *
 *                                                                             *
 *******************************************************************************/
#ifndef _MT_STRUCT_H_
#define _MT_STRUCT_H_

#include "kdvtype.h"
#include "mtmacro.h"
#include "mttype.h"
#include "osp.h"
#include "vccommon.h"




/************************************************************************/
/* ���������Ϣ���ݽṹ���忪ʼ                                         */
/************************************************************************/


/************************************************************************/
/* H323��Ϣ���ݽṹ����                                                 */
/************************************************************************/
typedef struct tagTH323Cfg
{
    s8   achMtAlias[MT_MAX_H323ALIAS_LEN+1]; //����
    s8   achE164[MT_MAX_E164NUM_LEN+1];      //E164����
    BOOL bIsUseGk;                  // �Ƿ�ʹ��GK
	s8   achGKPwd[MT_MAX_PASSWORD_LEN+1];//GK����
    u32_ip  dwGkIp;                     // GK ��ַ
    s8   achGkIpName[MT_MAX_H323GKSERVER_NAME_LEN]; //Gk������ַ
    BOOL bIsH239Enable;                  // H239��ʽ
    BOOL bIsEnctyptEnable;          // �Ƿ�ʹ�ü���
    EmEncryptArithmetic emEncrptMode;      //���ܷ�ʽ
    u16  wRoundTrip;                //��·���ʱ��
public:
	tagTH323Cfg(){memset(this , 0 ,sizeof( struct tagTH323Cfg) );	}
}TH323Cfg,*PTH323Cfg;
    

/************************************************************************/
/* ������Ϣ���ݽṹ����                                               */
/************************************************************************/
typedef struct tagTGuideCfg
{
    EmLanguage  emLanguage;         //����ѡ��
    BOOL  bIsDHCPEnable;            //�Ƿ�ʹ��DHCP
    u32_ip  dwIp;                      //�ն�IP��ַ    
    u32_ip  dwMask;                     //��������
	u32_ip  dwGateWay;                  //���ص�ַ
    BOOL bH323Enable;              //�Ƿ�ʹ��H323
	BOOL bH320Enable;              //�Ƿ�ʹ��H320
	BOOL bSIPEnable;                //�Ƿ�ʹ��SIP
    TH323Cfg tH323Info;             //H323����
    BOOL bUserCfgPwdEnable;         //�Ƿ�������������
    s8  achUserCfgPwd[MT_MAX_PASSWORD_LEN+1]; //�Ƿ�������������
    BOOL bNetCfgPwdEnable;         //�Ƿ�����������������
    s8  achNetCfgPwd[MT_MAX_PASSWORD_LEN+1]; //������������
}TGuideCfg, *PTGuideCfg;

// TsymboPoint̨��λ�ýṹ
typedef struct tagTSymboPoint
{
	u16 X;
	u16 Y;
public:
	tagTSymboPoint(){ memset( this ,0 ,sizeof( struct  tagTSymboPoint ) );}
}TSymboPoint;

/************************************************************************/
/* ��ʾ������Ϣ���ݽṹ����                                               */
/************************************************************************/
typedef struct tagTOsdCfg
{
    BOOL  bIsAutoPip;            //�Ƿ������Զ����л�
    BOOL  bIsShowConfLapse;      //�Ƿ���ʾ����ʱ��
    BOOL  bIsShowSysTime;        //�Ƿ���ʾϵͳʱ��
    BOOL  bIsShowState;          //�Ƿ���ʾ״̬��־
    EmLabelType  emLabelType;    //̨������
	TSymboPoint  tLableCoordinate; // ̨������
    EmDualVideoShowMode emDualMode;//˫����ʾ��ʽ
	EmDisplayRatio  emDisplayRatio;//��ʾ����ģʽ
}TOsdCfg, *PTOsdCfg;

//ʱ��ṹ���¶���
typedef struct tagTMtKdvTime
{
    u16 		m_wYear;//��
    u8  		m_byMonth;//��
    u8  		m_byMDay;//��
    u8  		m_byHour;//ʱ
	u8  		m_byMinute;//��
	u8  		m_bySecond;//��
public:
    tagTMtKdvTime(){ memset ( this ,0 ,sizeof( struct tagTMtKdvTime) );}
}TMtKdvTime, *PTMtKdvTime;

/************************************************************************/
/* �û�������Ϣ���ݽṹ����                                               */
/************************************************************************/
typedef struct tagTUserCfg
{
    EmTripMode emAnswerMode;   //Ӧ��ʽ
    BOOL  bIsAutoCallEnable;    //�Ƿ������Զ�����
    s8  achTerminal[MT_MAX_H323ALIAS_LEN+1]; //�Զ����е��ն���
    u16  wCallRate;               //�Զ���������
    BOOL  bIsRmtCtrlEnbale;      //�Ƿ�����Զ�˿���
    BOOL  bIsSleepEnable;        //�Ƿ�����������
    u16  wSleepTime;            //����ʱ��
	BOOL  bDisableTelephone;       // �Ƿ���õ绰����
}TUserCfg, *PTUserCfg;

/************************************************************************/
/*8010c����vga�������                                               */
/************************************************************************/
//[xujinxing]
typedef struct tagTVgaOutCfg
{
	BOOL bVgaOut; //8010c�Ƿ���vga�����
	u16  wRefreshRate; //vga��ˢ����
}TVgaOutCfg, *PTVgaOutCfg;

//[xujinxing-2006-05-19]
typedef struct tagTMtEquipmentCapset
{
    BOOL bMicAdjustSupport;
	BOOL bMcSupport;
}TMtEquipmentCapset, *PTMtEquipmentCapset;


/************************************************************************/
/*��Ƕmc����                                                            */
/************************************************************************/
//[xujinxing-2006-10-17]
typedef struct tagTInnerMcCfg
{
	BOOL bUseMc;
	EmMCMode emMode;
	BOOL bAutoVMP;
	tagTInnerMcCfg()
	{
		bUseMc = FALSE;
		emMode = emMcModeEnd;
		bAutoVMP = FALSE;
	}
}TInnerMcCfg,*PTInnerMcCfg;



/************************************************************************/
/* ��̫����Ϣ���ݽṹ����                                               */
/************************************************************************/
typedef struct tagTEthnetInfo
{
    BOOL bEnable;
    u32_ip  dwIP;
    u32_ip  dwMask;
    BOOL bIsUserAssignBandWidth;
    u32  dwUpBandWidth;
    u32  dwDownBandWidth;
}TEthnetInfo, *PTEthnetInfo;



typedef struct tagTE1Unit                                                                                     
{                                                                                                              
    BOOL bUsed;        //ʹ�ñ�־,��1ʹ�ã�0��ʹ��  
    u32   dwE1TSMask;        // E1ʱ϶����˵�� 
    EmDLProtocol emProtocol; //�ӿ�Э���װ����,����ǵ�E1���ӿ���ָ��PPP/HDLC��
    //����Ƕ�E1�������ӱ�����PPPЭ��
    u32 dwEchoInterval; // ��Ӧserialͬ���ӿڵ�echo����ʱ��������Ϊ��λ����Ч��Χ0-256��Ĭ����2
    u32 dwEchoMaxRetry;// ��Ӧserialͬ���ӿڵ�echo����ط���������Ч��Χ0-256��Ĭ����2 
}TE1Unit,*PTE1Unit;

/************************************************************************/
/* E1��Ϣ���ݽṹ����                                               */
/************************************************************************/
typedef struct tagTE1Config
{                                                                                                            
// Single & Multi Chan param.                                                                                  

	u8  byUnitNum;                 //ʹ��E1��Ԫ����
    BOOL  bIsBind;                  //  �Ƿ��                                                                    
    u32_ip  dwIP;                      //  ip��ַ                                                                          
    u32_ip  dwIPMask;                  //  ���룬                                                                          
	// ��ͨ������
    EmAuthenticationType emAuthenticationType;//PPP���ӵ���֤����PAP/CHAP,Ĭ����emPAP 
    u32  dwFragMinPackageLen;       //  ��С��Ƭ�������ֽ�Ϊ��λ����Χ20~1500��Ĭ����20                                 
    s8   achSvrUsrName[MT_MAX_E1_NAME_LEN+1]; 	/* ������û����ƣ�������֤�Զ˵����� */                                       
    s8   achSvrUsrPwd[MT_MAX_E1_NAME_LEN+1];   	/* ������û����룬������֤�Զ˵����� */                               
    s8   achSentUsrName[MT_MAX_E1_NAME_LEN+1];	/* �ͻ����û����ƣ��������Զ���֤ */                                           
    s8   achSentUsrPwd[MT_MAX_E1_NAME_LEN+1]; 	/* �ͻ����û����룬�������Զ���֤ */                                           
	// E1 Module Param.                                                                                            
    TE1Unit atE1Unit[ MT_MAX_E1UNIT_NUM ];//E1_SINGLE_LINK_CHAN_MT_MAX_NUM];/* ÿ���������serialͬ���ӿڲ��� */
	
}TE1Config, *PTE1Config;



/************************************************************************/
/* ������Ϣ���ݽṹ����                                               */
/************************************************************************/
typedef struct tagTSerialCfg
{
    u32  dwBaudRate; //�����ʵ�λkbps
    u8   byByteSize;   //����λ���� 5,6,7,8
    EmParityCheck emCheck; //У���㷨
    EmStopBits emStopBits; //ֹͣλ
}TSerialCfg, *PTSerialCfg;


/************************************************************************/
/* ·����Ϣ���ݽṹ����                                               */
/************************************************************************/
typedef struct tagTRouteCfg
{
    BOOL bUsed;
    u32_ip dwDstNet; //Ŀ������
    u32_ip dwDstNetMask; //Ŀ����������
    u32_ip dwNextIP; //��һ��IP��ַ  
    u32 dwPri;  //·�����ȼ�
}TRouteCfg, *PTRouteCfg;

/************************************************************************/
/* PPPOE��Ϣ���ݽṹ����                                               */
/************************************************************************/

/*��ʼ��PPPOEģ��ʱʹ�õĽṹ��*/
typedef struct  tagTPPPOECfg
{
	BOOL    bUsed;   //�Ƿ�ʹ��PPPoE
    s8      abyUserName [MT_PPPoE_MAX_USENAME_LEN];/*����ʱʹ�õ��û���,������*/
    s8      abyPassword [MT_PPPoE_MAX_PASSWORD_LEN]; /*����ʱʹ�õ�����,������*/
    s8      abyServerName[MT_PPPoE_MAX_SERVERNAME_LEN];/*ָ�����ŷ����̵�����*/
    BOOL  bAutoDialEnable;/*�Ƿ�����������Զ�����,Ĭ�ϲ��Զ�����*/
    BOOL  bDefaultRouteEnable;/*�Ƿ������Զ˵ĵ�ַ��ΪĬ�����أ�Ĭ�ϲ���Ϊ����*/
    BOOL  bDebug;/*�Ƿ���debugģʽ����pppoe��Ĭ�ϲ�������debugģʽ*/
    u32     dwAutoDialInterval;/*�������ٴ��Զ�������Ҫ�ȴ���ʱ�䣨�������ó�0s��*/
    u16     wDialRetryTimes;/*���ų��ԵĴ���������ﵽ�ô�����û����Ӧ�򲦺�ʧ��*/
    u16     wLcpEchoSendInterval;/*���ӽ����󣬷���LCP-ECHO����ʱ����*/
    u16     wLcpEchoRetryTimes;/* ���ͼ���LCP-ECHO��û�еõ���Ӧ����Ϊ���ӶϿ�*/
    u8      byEthID;/*PPPOE������Ҫ�󶨵���̫����ID�ţ���0��ʼ��*/
}TPPPOECfg, *PTPPPOECfg;


/*�й�PPPOEģ�����ͳ����Ϣ�Ľṹ��*/
typedef struct tagTPPPoEStates
{
    BOOL    bPppoeRunning;/*PPPOE�Ƿ����У�TRUR�����У�FALSE��û������*/
    u32     dwLocalIpAddr;/*����IP��ַ,������*/
    u32     dwPeerIpAddr;/*�Զ�IP��ַ,������*/
    s8      abyServerName[MT_PPPoE_MAX_SERVERNAME_LEN];/*ISP Server������*/
    u32     dwLinkKeepTimes;/*��·������ʱ��*/
    u32     dwLinkDownTimes;/*��·�����Ĵ���ͳ��*/
    u32     dwPktsSend;/*�ܹ����͵����ݰ���--��������Э����Լ��ϲ�ҵ���������ݰ�*/
    u32     dwPktsRecv;/*�ܹ����յ����ݰ���*/
    u32     dwBytesSend;/*�ܹ����͵��ֽ���*/
    u32     dwBytesRecv;/*�ܹ����յ��ֽ���*/
}TPPPoEStates,*PTPPPoEStates;

/*�����ϲ�ҵ�����ע��Ļص�����������֤ʧ��ʱ��Ϣ�����ݽṹ��*/
typedef struct tagTPPPoEAuthFailed
{
    s8      abyUserName [MT_PPPoE_MAX_USENAME_LEN];/*ҵ�������Ĳ���ʱʹ�õ��û���*/
    s8      abyPassword [MT_PPPoE_MAX_PASSWORD_LEN]; /*ҵ�������Ĳ���ʱʹ�õ����� */
}TPPPoEAuthFailed,*PTPPPoEAuthFailed;

/*�����ϲ�ҵ�����ע��Ļص��������ͽ���֪ͨ��Ϣ�Ľṹ��*/
typedef struct tagTPPPoEConnNotify
{
    u32     dwOurIpAddr;/*���ϻ�õ�IP��ַ,������*/
    u32     dwPeerIpAddr;/*�Զ˵�IP��ַ*/
    u32     dwDnsServer1;/*DNS Server1,������*/
    u32     dwDnsServer2;/*DNS Server1,������*/    
}TPPPoEConnNotify,*PTPPPoEConnNotify;

/*֪ͨҵ������Ҳ���ָ����ISP Server����Ϣ�ṹ��*/
typedef struct tagTPPPoENoServer
{
    s8      abyServerName[MT_PPPoE_MAX_SERVERNAME_LEN];/*ISP Server������*/
}TPPPoENoServer,*PTPPPoENoServer;

/*�洢PPPOE�汾��Ϣ�Ľṹ��*/
typedef struct tagTPPPoEVersion
{
    s8      abyVersion[MT_PPPoE_MAX_VERSION_LEN];
}TPPPoEVersion,*PTPPPoEVersion;

/************************************************************************/
/* SNMP��Ϣ���ݽṹ����                                               */
/************************************************************************/
typedef struct tagTSNMPCfg
{
    s8    achUserName[MT_MAX_COMM_LEN+1];//��ͬ����
    u32_ip   dwTrapServerIp;//�澯������ַ
}TSNMPCfg, *PTSNMPCfg;


/************************************************************************/
/* QOS��Ϣ���ݽṹ����                                               */
/************************************************************************/
typedef struct tagTIPQoS
{
	//QoS��������
    EmQoS    emType;
	//����QOSֵ ��Χ
    u8       abySignalling[2];
	//�����ŵ�����������ͷԶҡ��QOSֵ ��Χ
    u8       abyData[2];
	//����QOSֵ ��Χ
    u8       abyAudio[2];
	//��ƵQOSֵ ��Χ
    u8       abyVideo[2];
	//������֤����
    EmTOS    emTOS;
}TIPQoS, *PTIPQoS;




/************************************************************************/
/* ��ý����Ϣ���ݽṹ����                                               */
/************************************************************************/
typedef struct tagTStreamMedia
{
    BOOL bUse;
    BOOL bForwardLocalVideo; //�Ƿ�ת��������Ƶ
    u32_ip  dwMCIP;    //�鲥IP��ַ
    u16  wPort;     //�˿ں�
	u8   byTTL;     //TTLֵ
    s8   achChannelName[MT_MAX_CHANNEL_NAME_LEN+1];//Ƶ������
    s8   achPassword[MT_MAX_PASSWORD_LEN+1];//����
}TStreamMedia, *PTStreamMedia;


/************************************************************************/
/*��Ƶ��������Ϣ���ݽṹ����                                               */
/************************************************************************/
typedef struct tagTVideoParam 
{
    BOOL  bUseFixedFrame;    //ʹ�ù̶�֡ 
    u8 byFrameRate;	///֡��
    EmFrameUnitType  emFrameUnit;	//֡�ʵ�λ
    u8 byQualMaxValue;	//��С��������
    u8 byQualMinValue;	//�����������
    u16 wIKeyRate;	//�ؼ�֡���
    EmEncodeMode emEncodeMode;     //���뷽ʽ
    EmLostPackageRestore emRestoreType; //�����ָ���ʽ
    u16 wH264IKeyRate;		//H264�ؼ�֡���
    u8  byH264QualMaxValue;	//H264�����������
    u8  byH264QualMinValue;	//H264��С��������
//���²���ֻ����windows�²�����
//��Ӳɼ�ͼ����
	u16	wWidth; //��Ƶ����ͼ���� ��Windows��Ч(default:640)
	u16	wHeight;//��Ƶ����ͼ��߶� ��Windows��Ч(default:480)
	EmMediaType   emCapType;    //�ɼ�����   (default:emMediaAV)
	EmPCCapFormat emPCCapFormat;//��Ƶ����֡��ʽ ��Windows��Ч(default:emCapPCFrameBMP)
//���²�������Ը����ն�, 
	u8 by720pFrameRate; //720p�ֱ�����֡��
	u8 byVgaFrameRate;  //˫��vga��֡��
	//xjx_080612
	u8 byD1FrameRate; //D1�ֱ�����֡��
	
}TVideoParam, *PTVideoParam;


/************************************************************************/
/*��ѡЭ����Ϣ���ݽṹ����                                               */
/************************************************************************/
typedef struct tagTAVPriorStrategy 
{
    EmVideoFormat  emVideoFormat;  //��Ƶ��ѡЭ��
    EmAudioFormat  emAudioFormat;    //��Ƶ��ѡЭ��
    EmVideoResolution emVideoResolution; //�ֱ�����ѡЭ��
}TAVPriorStrategy, *PTAVPriorStrategy;
/************************************************************************/
/* �����ش���Ϣ���ݽṹ����                                               */
/************************************************************************/
typedef struct tagTLostPackResend
{
	BOOL bUse;             //�Ƿ�����
	EmNetType emNetType;   //��������
	u8   byResendLevel;    //�ش��ȼ� 0<��,�ش�һ��> 1<��,�ش�����> 2<��,�ش�����>
	u16  wFirstTimeSpan;   //��һ���ش�����   default 40
	u16  wSecondTimeSpan;  //�ڶ����ش�����   default 80
	u16  wThirdTimeSpan;   //�������ش�����   default 160
	u16  wRejectTimeSpan;  //���ڶ�����ʱ���� default 200
	u16  wSendBufTimeSpan; //���ͻ���           default 1000
	BOOL bUseSmoothSend;   //����ƽ������

}TLostPackResend,*PTLostPackResend;


/************************************************************************/
/*�������Ϣ���ݽṹ����                                               */
/************************************************************************/


//����ͷ��������
typedef struct tagTCameraTypeInfo
{
	s8 achName[MT_MAXLEN_CAMERA_TYPE_NAME];
	u8 byMaxAddr; //����ͷ����ֵַ
	u8 byMaxSpeedLevel;//����ͷ����ٶȼ��� 

}TCameraTypeInfo ,*PTCameraTypeInfo;


//����ͷ����
typedef struct tagTCameraCfg
{
    BOOL  bUse;
    s8  achCamera[MT_MAX_CAMERA_NAME_LEN+1];//����ͷ��
    u8  byAddr;  //����ͷ��ַ
    u8  bySpeedLevel; //����ͷ�����ٶ�
    EmSerialType emSerialType;
    u32_ip dwSerialServerIP; //���ڷ�����IP ,����ֵΪ0ʱ��ʾʹ�ñ��ش���
    u16 wSerialServerPort; //���ڷ������˿ں�    
	
}TCameraCfg, *PTCameraCfg;

typedef struct tagTVideoStandard
{
	EmVideoType     emVideoType;
	BOOL            bIsInPort;    //�Ƿ�Ϊ����˿�
	EmVideoStandard emStandard;
}TVideoStandard ,*PTVideoStandard;

/************************************************************************/
/*��ƵԴ��Ϣ���ݽṹ����                                               */
/************************************************************************/
typedef struct tagTVideoSource
{
    EmVideoInterface emMainCam;
    u8 byCPortIndex ;//C��������ʱ��������˿ںţ�0��ַ
    EmVideoInterface emVideoOut;
    u8  byFrequency; //VGAʱΪˢ����
}TVideoSource, *PTVideoSource;


//������Ϣ
typedef struct tagTSnapInfo
{
	BOOL bResult;		// ��ȡ���(�ɹ�TRUE /ʧ��FALSE)
	u32  dwMaxSize;		// ���ռ��С(�ֽ�)
	u32  dwUsedSize;		// ���ÿռ��С(�ֽ�)
	u32	 dwPicNum;	// ����ͼƬ����(��)
} TSnapInfo,*PTSnapInfo;

typedef struct tagTBannerInfo
{
	BOOL        bEnable;			// ʹ��(MT_ENABLE/MT_DISABLE)
	u32		    dwTextClr;			// ������ɫ
	u32		    dwBkgClr;			// ����ɫ
	EmTextAlign	emTextAlign;	// ���ֶ���ģʽ(TEXT_ALIGN_LEFT/TEXT_ALIGN_CENTER/TEXT_ALIGN_RIGHT)
	EmRollMode	emRollMode;		// ����ģʽ(ROLL_NOT_ROLL/ROLL_RIGHT_LEFT/ROLL_DOWN_UP)
	u8 	        byTransParent;		// ͸����
	EmRollSpeed	emRollSpeed;		// �����ٶ�(ROLL_SPEED_SLOW/ROLL_SPEED_NORMAL/ROLL_SPEED_FAST/ROLL_SPEED_FASTER)
	u8		    byRollNum;			// ��������(0:��ʾ�����ƹ���,1~255:��ʾ�û�ָ���Ĺ�������)
	u8		    byStayTime;			// ͣ��ʱ��(0:��ʾ��ͣ��,1~255:��ʾ�û�ָ����ʱ��,��λ��)
	u32		    dwStartXPos;		// ��Ļ��ʾ����ʼX����
	u32		    dwStartYPos;		// ��Ļ��ʾ����ʼY����
	u32		    dwDisplayWidth;		// ��Ļ��ʾ����Ŀ�(������Ϊ��λ)
	u32		    dwDisplayHeight;	// ��Ļ��ʾ����ĸ�(ָ�и�,������Ϊ��λ)
	s8	        achWinInfo[ 256 ];	// ���ڴ��Windows�������Ϣ(��\0����)
} TBannerInfo, *PTBannerInfo;


// codec����ͳ����Ϣ
typedef struct tagTCodecStatistic
{
	u16 awVideoBitRate[2];		// ��Ƶ��������
	u16 awAudioBitRate[2];		// ��Ƶ��������
	u32 adwVideoRecvFrm[2];		// �յ�����Ƶ֡��
	u32 adwAudioRecvFrm[2];		// �յ�����Ƶ֡��
	u16 awVideoLoseRatio[2]; 	// ��Ƶ������
	u16 awAudioLoseRatio[2];	    // ��Ƶ������
	u32 adwVideoLoseFrm[2];		// ��Ƶ�ܶ�����
	u32 adwAudioLoseFrm[2];		// ��Ƶ�ܶ�����
} TCodecStatistic, *PTCodecStatistic;

// ��Ƶ����
typedef struct tagTAudioPower
{
	u8	byDecoderId;
	u32 dwOutputPower;
	u32 dwInputPower;
} TAudioPower, *PTAudioPower;

/************************************************************************/
/*     ��������                                                         */
/************************************************************************/

/************************************************************************/
/* ���������Ϣ���ݽṹ�������                                         */
/************************************************************************/



/************************************************************************/
/* ���������Ϣ���ݽṹ���忪ʼ                                         */
/************************************************************************/

//�ն˱��
typedef struct tagTMtId
{
	u8 byMcuNo;
	u8 byTerNo;
	
public:
	tagTMtId(){ memset( this ,0 ,sizeof( struct tagTMtId ) );	}
	
}TMtId ,*PTMtId;

//����������Ϣ
typedef struct tagTConfBaseInfo
{
	s8	 achConfId[MT_MAX_CONF_ID_LEN + 1 ];			// ����ID
	s8	 achConfName[MT_MAX_CONF_NAME_LEN + 1 ];		// ������
	s8	 achConfNumber[ MT_MAX_CONF_E164_LEN + 1 ];	    // �������
	s8	 achConfPwd[ MT_MAXLEN_PASSWORD + 1 ];			// ��������
	BOOL bNeedPassword;                         //�Ƿ���Ҫ����
	u16  wConfDuration;							// �������ʱ��		
	EmVideoFormat   emVideoFormat;							// ������Ƶ��ʽ(VIDEO_H261��)
	EmAudioFormat   emAudioFormat;							// ������Ƶ��ʽ(AUDIO_MP3��)
	EmVideoResolution  emResolution;							// ������Ƶ�ֱ���(VIDEO_CIF��)
	BOOL  bIsAutoVMP;								// �Ƿ��Զ�����ϳ�
	BOOL  bIsMix;	    							// �Ƿ����

public:
	tagTConfBaseInfo(){ memset( this ,0 ,sizeof( struct tagTConfBaseInfo ) );	}
}TConfBaseInfo ,*PTConfBaseInfo;

//�����б���Ϣ
typedef struct tagTConfListInfo
{
	u8			  byConfNum;
	TConfBaseInfo atConfInfo[MT_MAX_CONF_NUM];
public:
	tagTConfListInfo(){ memset( this ,0 ,sizeof( struct tagTConfListInfo ) );	}
}TConfListInfo ,*PTConfListInfo;

//�ն���Ϣ
typedef struct tagTMtInfo
{
	TMtId tLabel;
	s8     achAlias[MT_MAX_NAME_LEN+1];

public:
	tagTMtInfo(){memset( this ,0 ,sizeof( struct tagTMtInfo) );}
}TMtInfo ,*PTMtInfo;

//��Ƶ�����״̬
typedef struct tagTMtVideoCodecStatus
{
	BOOL                bRuning;           //�Ƿ��ڹ���
	EmVideoFormat       emFormat;          //��Ƶ�����ʽ
	EmVideoResolution   emRes;             //��Ƶ�����ʽ
	u16                 wBitrate;          //��Ƶ��������(��λ:kbps)
	u16                 wAverageBitrate;   //����Ƶ��ƽ��������(��λ:kbps)
	BOOL                IsEncrypt;         //����Ƶ�����Ƿ����
	EmEncryptArithmetic emArithmetic;	   //���ӱ���ʹ�õļ����㷨	

public:
	tagTMtVideoCodecStatus()
	{ 
		memset ( this ,0 ,sizeof( struct tagTMtVideoCodecStatus) );
	}
}TMtVideoCodecStatus ,*PTMtVideoCodecStatus;

//�ն�״̬
typedef struct tagTTerStatus
{
	EmMtModel  emMtModel;				    //�ն��ͺ�
	EmFileSys  emFileSys;                   //�ļ�ϵͳ����

	u8		byEncVol;					//��ǰ��������(��λ:�ȼ�)
	u8		byDecVol;					//��ǰ��������(��λ:�ȼ�)
	BOOL	bIsMute;					//�Ƿ�����	
	BOOL	bIsQuiet;					//�Ƿ���			
	BOOL	bIsLoopBack;				//�Ƿ��Ի�
	BOOL    bIsInMixing;                //�Ƿ�μӻ���
	BOOL    bRegGkStat;			    	//GKע����	1 �ɹ� 0 ʧ��
	EmAction   emPollStat;				//��ѯ״̬		emStart ,emPause ,emStop /��ʼ/��ͣ/ֹͣ
	EmSite	emCamCtrlSrc;				    //��ǰ��������ͷԴ���� emLocal, emRemote
	u8		byLocalCamCtrlNo;			    //��ǰ���Ƶı�������ͷ��(1-6������ͷ)
	u8		byRemoteCamCtrlNo;			    //��ǰ���Ƶ�Զ������ͷ��(1-6������ͷ)
	u8      byE1UnitNum ;                   //�ն�E1ģ��ĵ�E1����
	BOOL	bFECCEnalbe;				//�Ƿ�����Զң
	BOOL    bLocalIsVAG;                //���صڶ�·����������Ƿ�VGA
	BOOL    bPVHasVideo;                //��һ·�Ƿ�����ƵԴ
	BOOL    bSVHasVideo;                //�ڶ�·�Ƿ�����ƵԴ
//һ�²��ֲ�����ʹ��
	BOOL	bIsAudioPowerTest;			//�Ƿ��ڲ�����Ƶ	
	BOOL	bIsLocalImageTest;			//�Ƿ񱾵�ͼ�����
	BOOL	bIsRemoteImageTest;			//�Ƿ�Զ��ͼ����� 
//	///////����״̬///////
	BOOL       bIsInConf;				//�Ƿ��ڻ�����	
	EmConfMode emConfMode;				//����ģʽ		1 ���� 2 ���		
    BOOL       bCallByGK;				//�Ƿ�ͨ��GK���� TRUE GK·�� FALSEֱ�Ӻ���
//	
//	///////�豸״̬///////
	BOOL	bMatrixStatus;				    //���þ���״̬		(TRUE ok FALSE err)
	u8		byCamNum;					    //����ͷ����	(0~6)
	BOOL	bCamStatus[MT_MAX_CAMERA_NUM];	//����ͷ״̬ (TRUE ok FALSE err)
	EmMtVideoPort emVideoSrc;					//��ǰ������ƵԴ(0:S����,1-6:C����)

	BOOL    bIsEncrypt;
//	//�������״̬
	EmAudioFormat	    emADecFormat;	//��Ƶ�����ʽ
	EmAudioFormat	    emAEncFormat;	//��Ƶ�����ʽ
	TMtVideoCodecStatus  tPVEncStatus;   //����Ƶ������
	TMtVideoCodecStatus  tSVEncStatus;   //����Ƶ������
	TMtVideoCodecStatus  tPVDecStatus;   //����Ƶ������
	TMtVideoCodecStatus  tSVDecStatus;   //����Ƶ������
//	
	u16		wSysSleep;					//����ʱ��(0x0��ʾ������)
//VOD states
	BOOL bIsInVOD;
	EmUIPosion   byVodUser;
	//////����״̬///////
public:
   tagTTerStatus(){ memset ( this ,0 ,sizeof( struct tagTTerStatus) );}
}TTerStatus ,*PTTerStatus;



//����ϳɲ���
typedef struct tagTMtVMPParam
{
	BOOL    bIsCustomVMP;  //�Ƿ��Զ��廭��ϳ�
	BOOL    bIsAutoVMP;    //�Ƿ��Զ�����ϳ�
	BOOL    bIsBroadcast;
	EmVMPStyle emStyle;       //����ϳɷ��
	TMtId   atMt[MT_MAXNUM_VMP_MEMBER]; //����ϳɳ�Ա
	EmVMPMmbType atemMmbType[MT_MAXNUM_VMP_MEMBER];//����Ա������

public:
   tagTMtVMPParam(){ memset ( this ,0 ,sizeof( struct tagTMtVMPParam) );}
}TMtVMPParam ,*PTMtVMPParam;
// ��չ����ϳɲ���
typedef struct tagTMtVmpItem
{
	TMtId        tMt;
	EmVMPMmbType emMmbType;
	u32          adwReserved[8];
public:
   tagTMtVmpItem(){ memset ( this ,0 ,sizeof( struct tagTMtVmpItem) );}
}TMtVmpItem, *PTMtVmpItem;



//��ѯ��Ϣ
typedef struct tagTMtPollInfo
{
	EmMediaType emMode;      //��ѯģʽ emMediaVide /emMediaAV 
	EmAction    emStat;      //    emStart/  emStop/  emPause ��ѯ״̬
	u16         wKeepTime;   //�ն���ѯ�ı���ʱ�� ��λ:��(s)
	u8			byMtNum;			// �μ���ѯ���ն˸���,0��ʾ��������ն�(��λ��,���CTRL_POLL_MAXNUM)
	TMtInfo	    atMtInfo[ MT_MAX_POLL_NUM ]; // �μ���ѯ���ն�<m,t>
public:
	tagTMtPollInfo(){ memset ( this ,0 ,sizeof( struct tagTMtPollInfo) );}
}TMtPollInfo ,*PTMtPollInfo;

 
typedef struct tagTMtSimpConfInfo 
{
	TMtId tSpeaker;
	TMtId tChairMan;
	BOOL  bIsVAC;
	BOOL  bIsDisc;
	BOOL  bIsAutoVMP;
	BOOL  bIsCustomVMP;
public:
	tagTMtSimpConfInfo(){ memset( this, 0, sizeof( struct tagTMtSimpConfInfo ) ); }
}TMtSimpConfInfo, *PTMtSimpConfInfo;
  
//���������Ϣ
typedef struct tagTMtConfInfo
{
	s8          chConfId[MT_MAXLEN_CONFGUID+1];
	TMtKdvTime    tStartTime;//��ʼʱ�䣬����̨��0Ϊ������ʼ
	u16           wDuration;   //����ʱ��(����)��0��ʾ���Զ�ֹͣ
    u16           wBitRate;        //��������(��λ:Kbps,1K=1024)
    u16           wSecBitRate;     //˫�ٻ���ĵ�2����(��λ:Kbps,Ϊ0��ʾ�ǵ��ٻ���)
    EmVideoResolution emMainVideoResolution;  //����Ƶ��ʽ
    EmVideoResolution emSecondVideoResolution; //����Ƶ��ʽ
    EmVideoResolution emDoubleVideoResolution;   //�ڶ�·��Ƶ��ʽ,
    u8            byTalkHoldTime;                 //��С���Գ���ʱ��(��λ:��)

	s8            achConfPwd[MT_MAXLEN_PASSWORD+1];    //��������
    s8            achConfName[MT_MAX_CONF_NAME_LEN+1]; //������
    s8            achConfE164[MT_MAX_CONF_E164_LEN+1]; //�����E164����
	
	BOOL          bIsAudioPowerSel;   //�Ƿ���������
	BOOL          bIsDiscussMode;     //�Ƿ�����ģʽ
    BOOL          bIsAutoVMP;            //�Ƿ��Զ��໭��ϳ�
	BOOL          bIsCustomVMP;        //�Ƿ��Զ���໭��ϳ�
	BOOL          bIsForceBroadcast;//ǿ�ƹ㲥

    TMtId 	      tChairman;	   //��ϯ�նˣ�MCU��Ϊ0��ʾ����ϯ
    TMtId		  tSpeaker;		  //�����նˣ�MCU��Ϊ0��ʾ�޷�����
    TMtPollInfo   tPollInfo;        //������ѯ����,����ѯʱ�н�
    TMtVMPParam   tVMPParam;        //��ǰ��Ƶ���ϲ���������Ƶ����ʱ��Ч
  
public:
	tagTMtConfInfo(){ memset ( this ,0 ,sizeof( struct tagTMtConfInfo) );}
}TMtConfInfo ,*PTMtConfInfo;





// �����ͳ����Ϣ
typedef struct tagTMtCodecStat
{
	BOOL bWorking;               //�Ƿ�ʼ����
	u16 wBitrate;
	u32 dwTransPacket;            //�շ�����
	u32 dwLostPackets;            //������ 
	u16 wLostRatio;               //������
	EmAudioFormat  emAudioFormat; //��ʽ
	EmVideoFormat  emVideoFormat;
	EmVideoResolution emVideoResolution; //��Ƶ�ֱ���
	BOOL      bIsEncrypt;   //�Ƿ����
public:
	tagTMtCodecStat(){ memset( this ,0 ,sizeof( struct tagTMtCodecStat));}


}TMtCodecStat ,*PTMtCodecStat;

//IP�����ַ
typedef struct tagTIPTransAddr
{
	u32_ip dwIP ;
	u16 wPort;
public:
	tagTIPTransAddr(){ memset( this, 0, sizeof( struct tagTIPTransAddr));}
}TIPTransAddr,*PTIPTransAddr;

//�ն˵�ַ
typedef struct tagTMtAddr
{
	EmMtAddrType emType;							//��ַ����
	u32_ip			 dwIP;						//�ն�IP��ַ
	s8			 achAlias[MT_MAX_H323ALIAS_LEN+1];		//(����)
public:
	tagTMtAddr(){memset( this ,0 ,sizeof( struct tagTMtAddr));	}
}TMtAddr,*PTMtAddr;




//�����ʶ
typedef struct tagTConfId
{
	u8  byGuid[MT_MAXLEN_CONFGUID];//����Id
	TMtAddr   tAlias;    //�������
public:
	tagTConfId(){ memset( this ,0 ,sizeof( struct tagTConfId)) ;	}
}TConfId ,PTConfId ;

//���в���
typedef struct tagTDialParam
{
	EmCallMode   emCallType;			// ��������:JOIN|CREATE|INVITE
	EmConfProtocol emProtocol;          // ͨ��Э��Э��
	u16			 wCallRate;			    // ��������(kbps)
	TMtAddr	     tCallingAddr;		    // ���е�ַ(����ʱ�ɲ���)
	TMtAddr	     tCalledAddr;			// ���е�ַ

	//////////////////////////////////////////////////////////////////////////
	//���½����ڴ�������
	BOOL		bCreateConf;		//�Ƿ��Ǵ�������
	TConfBaseInfo   tConfInfo;			//������Ϣ
	u8			byTerNum;			//�����ն˸���
	TMtAddr 	atList[MT_MAXNUM_INVITEMT];	//�����ն��б�
public:
	tagTDialParam(){ memset( this ,0 ,sizeof( struct tagTDialParam ) );}
}TDialParam,*PTDialParam;

// ������·״̬�ṹ
typedef struct tagTLinkState
{
	EmCallState emCallState; //�Ự״̬
	u32_ip			dwIpAddr;					  // �Զ�IP��ַ(������)
	s8  		achAlias[ MT_MAX_NAME_LEN + 1 ];  // �Զ˱���
	BOOL        bCalling;                         // TRUE = ����  FALSE=����
	EmCallDisconnectReason	emReason;   // ���йҶ�ԭ��
	BOOL        bGetChairToken;                   //�����ϯ����
	BOOL        bSeenByAll;                       //���㲥
	u16         wCallRate;                        //��������
	EmMtModel   emPeerMtModel;                    //add by xujinxing,07/07/26,�Զ��ͺ�
public:
	tagTLinkState(){ memset( this ,0 ,sizeof( struct tagTLinkState ) );}
}TLinkState,*PTLinkState;


//TEmbedFwNatProxy ��Ƕ����ǽ�������ýṹ
typedef struct tagTEmbedFwNatProxy
{
    BOOL   bUsed;
    u32_ip dwFwNatProxyServIP;
	s8     achPrxySrvName[MT_MAX_PROXYSERVER_NAME_LEN];
    u16    wFwNatProxyServListenPort;
    u16    wStreamBasePort;//ý����������ʼ�˿ں�
public:
	tagTEmbedFwNatProxy(){ memset( this ,0 ,sizeof( struct tagTEmbedFwNatProxy ) );}
}TEmbedFwNatProxy,*PTEmbedFwNatProxy;


//��̬NATӳ���ַ
typedef struct tagTNATMapAddr
{
	BOOL bUsed; //�Ƿ����þ�̬ӳ���ַ
	u32_ip  dwIP;  //ӳ���ַ
public:
	tagTNATMapAddr(){ memset( this ,0 ,sizeof( struct  tagTNATMapAddr ) );}
}TNATMapAddr,*PTNATMapAddr;

//GK����
typedef struct tagTGKCfg
{
	BOOL bUsed; //�Ƿ�ʹ��GK
	u32_ip  dwGKIP; //GK IP��ַ
    s8   achGkIpName[MT_MAX_H323GKSERVER_NAME_LEN]; //Gk������ַ
	s8   achGKPasswd[MT_MAX_PASSWORD_LEN + 1]; //Gk����
public:
	tagTGKCfg(){ memset( this ,0 ,sizeof( struct  tagTGKCfg ) );}
}TGKCfg,*PTGKCfg;

/************************************************************************/
/* ����GK��Ϣ���ݽṹ����                                               */
/************************************************************************/
typedef struct tagTInnerGKCfg
{
    BOOL bGKUsed; //�Ƿ���������GK
    u32_ip dwIp; //ip��ַ
    u32_ip dwMask; //��������
    u32_ip dwGateWay; //����
public:
    tagTInnerGKCfg(){ memset( this, 0, sizeof(struct tagTInnerGKCfg ) ); }   
}TInnerGKCfg, *PTInnerGKCfg;

/************************************************************************/
/* ����Proxy��Ϣ���ݽṹ����                                               */
/************************************************************************/
typedef struct tagTInnerProxyCfg
{
    BOOL bInnerProxyUsed; //�Ƿ����ô��������
    u32_ip dwIp; //ip��ַ
    u32_ip dwMask; //��������
    u32_ip dwGateWay; //����
public:
    tagTInnerProxyCfg(){ memset( this, 0, sizeof(struct tagTInnerProxyCfg ) ); }  
}TInnerProxyCfg, *PTInnerProxyCfg;

/************************************************************************/
/* WiFi����������Ϣ���ݽṹ����                                         */
/************************************************************************/
typedef struct tagTWiFiNetCfg
{
    BOOL   bWiFiUsed;  //�Ƿ�����WiFi 
    BOOL   bDHCP;      //�Ƿ�����DHCP
    u32_ip dwIp;       //ip��ַ
    u32_ip dwMask;     //��������
    u32_ip dwGateWay;  //����
public:
    tagTWiFiNetCfg(){ memset( this, 0, sizeof(struct tagTWiFiNetCfg ) ); }  
}TWiFiNetCfg, *PTWiFiNetCfg;

// TMonitorService������л�ҵ�����ݽṹ�� 
typedef struct tagTMonitorService
{
	BOOL bSwitchEnable;
	//����ʱ�л������ҵ���ʱ����
	u32   dwSwitchTimeout;
public:
	tagTMonitorService(){ memset( this ,0 ,sizeof( struct  tagTMonitorService ) );}
}TMonitorService,*PTMonitorService;

//���νṹ����
typedef struct  tagTRect
{
    u16  wLeft;
    u16  wTop;
    u16  wWidth;
    u16  wHeight;

}TRect;

//��
typedef struct tagTPoint
{
	u16 wX;
	u16 wY;
}TPoint;

//˫��
typedef struct TDualStream
{
    EmDualSrcType	emDualSrcType;  // ˫��Դ����
	EmAction        emAction;		// [REQ/RSP]	����(MT_START/MT_STOP)
	BOOL            bResult;		// [RSP]		������� TRUE= �ɹ� FALSE=ʧ��
	EmMtVideoPort   emVideoSrc;     // ��ƵԴ
	TMtAddr         tRecvAddr;		// [RSP]		�ն˽��յ�ַ(����PCԴ)
	u8		        byReason;			// [RSP]		˫������ʧ�ܵ�ԭ��
}TDualStream,*PTDualStream;


//˫�����ʱ�������
typedef struct tagTDualRation
{
    BOOL bSecondSendRateType;   //TRUE auto
    BOOL bSecondRecvRateType;   //������������
    u8 bySecondSendRate;  //��������Զ�ʱ˫���������ʵı���ֵ
    u8 bySecondRecvRate;  //˫���������ʵı���ֵ
    u8 byPrimaryRecvRate; //�����������ʵı���ֵ
    tagTDualRation(){ memset( this ,0 ,sizeof( struct  tagTDualRation ) );}
}TDualRation, *PTDualRation;

//	ͼ���������
typedef struct tagTImageAdjustParam
{	 
	u8  wBrightness;//����  	 
	u8  wContrast;   //�Աȶ�  	
	u8  wSaturation; //ɫ�ʱ��Ͷ� 
public:
	tagTImageAdjustParam(){ memset( this ,0 ,sizeof( struct  tagTImageAdjustParam ) );}
	
}TImageAdjustParam,*PTImageAdjustParam;


//��Ƶ�������
typedef struct tagTVideoEncodeParameter    
{
	EmVideoFormat      emVideoFormat; 
	u16                byMaxKeyFrameInterval; // I֡�����P֡��Ŀ
    u8                 byMaxQuant;  //�����������
	u8                 byMinQuant;  //��С��������
    BOOL               bUseFixedFrame; //�Ƿ�ʹ�ù̶�֡��
	u8	               byFrameRate;  //֡��
	BOOL               bFramePerSecond;  //֡�ʵ�λ TRUE�� ֡/��  FALSE����/֡ 

    EmEncodeMode emEncodeMode;     //���뷽ʽ
	EmVideoResolution  emVideoResolution;
	u32                wChanMaxBitrate; // ��λ Kbps
//��������ֻ����windowsƽ̨������
//��Ӳɼ�ͼ����
	u16	m_wWidth; //��Ƶ����ͼ���� ��Windows��Ч(default:640)
	u16	m_wHeight;//��Ƶ����ͼ��߶� ��Windows��Ч(default:480)
	EmMediaType   emCapType;    //�ɼ�����   (default:emMediaAV)
	EmPCCapFormat emPCCapFormat;//��Ƶ����֡��ʽ ��Windows��Ч(default:emCapPCFrameBMP)
public:
	tagTVideoEncodeParameter(){ memset( this ,0 ,sizeof( struct  tagTVideoEncodeParameter ) );}	
}TVideoEncodeParameter ,*PTVideoEncodeParameter;

typedef struct tagTEncryptKey
{
	u8 byLen;   //��Կ����
	u8 byKey[MT_MAXLEN_ENCRYPTKEY]; //��Կ����
public:
	tagTEncryptKey(){ memset( this ,0 ,sizeof( struct  tagTEncryptKey ) );}	
}TEncryptKey,*PTEncryptKey;

//��Ƶͨ������
typedef struct tagTVideoChanParam
{
	EmVideoFormat     emVideoFormat;
	EmVideoResolution emVideoResolution;
	u32                wChanMaxBitrate; // ��λ Kbps
	u8                 byPayload;       //��̬�غ�
	TEncryptKey        tKey;            //��tKey.byLen=0 ��ʾ������
	BOOL               bIsH239;
	u8                 byFrameRate;          //֡��
 // 	u8                 byFrameRate;     2005-12-19 ������ί���ŷ����
public:
	tagTVideoChanParam()
	{ 
		emVideoFormat = emVEnd;
		emVideoResolution = emVResEnd;
		wChanMaxBitrate   = 0;
		byPayload         = 0;
		memset( &tKey ,0 ,sizeof(tKey) );
		bIsH239  = FALSE;
		byFrameRate = 25;
	}	
}TVideoChanParam ,*PTVideoChanParam;

//��Ƶͨ������
typedef struct tagTAudioChanParam
{
	EmAudioFormat   emAudioFormat;
  
	u8               byPayload;     //��̬�غ�
	TEncryptKey      tKey;          //��tKey.byLen=0 ��ʾ������
public:
	tagTAudioChanParam()
	{
		emAudioFormat = emAEnd;
		byPayload     = 0;
		memset( &tKey ,0 ,sizeof(tKey));
	}
}TAudioChanParam ,*PtagTAudioChanParam;

//***************************** VOD *************************************
//
//��½VOD�������û���Ϣ
//
typedef struct tagTVODUserInfo
{
	TIPTransAddr m_tIp;  //VOD��������ַ
	s8   m_achUserName[VOD_MAX_USER_NAME_LENGTH + 1];//�û���
	s8   m_achUserPass[VOD_MAX_PASSWORD_LENGTH + 1];//�û�����
	BOOL m_bRecordNamePass; //�´ε�¼�Ƿ���ʾ�û���������
public:
	tagTVODUserInfo()
	{
		memset(this,0,sizeof(tagTVODUserInfo));
	}
}TVODUserInfo,*PTVODUserInfo;


//
// vod �ļ���Ϣ ���ṹȡ�� TVODFileNameInfo+TVODFileInfo 
//
typedef struct tagTVODFile
{
	s8   achName[ MT_MAX_NAME_LEN + 1 ]; // �ļ���
    BOOL bIsFile;     // TRUE =file FALSE= folder
    u32  dwFileSize;  // �ļ�����
	//-- �ļ���Ϣ--
	BOOL bIsInfoValid;       //�ļ���Ϣ�Ƿ���Ч
	u32 dwRecordTime;        //¼��ʱ�䣬
    u32 dwDuration;          //������ʱ��������
    EmAudioFormat emAudio;          //��Ƶ�� emAEnd��ʾû�и�·���� 
    EmVideoFormat emPrimoVideo;     //��Ƶ1��emVEnd��ʾû�и�·���� 
    EmVideoFormat emSecondVideo;    //��Ƶ2��emVEnd��ʾû�и�·����

}TVODFile;
//
// Files from VODServer Directory 
//
typedef struct tagTVODFileNameInfo
{
    s8  m_achFileName[ MT_MAX_NAME_LEN + 1 ]; // �ļ���
    u8  m_byType; // 0 Ŀ¼  1 �ļ� 
    u32 m_dwFileLen; // �ļ�����
} TVODFileNameInfo, *PTVODFileNameInfo;
 

//
// File Info from VODServer
//
typedef struct tagTVODFileInfo
{
    u32 m_dwRecordTime;        //¼��ʱ�䣬
    u32 m_dwDuration;          //������ʱ��������
    EmAudioFormat m_emAudio;          //��Ƶ�� emAEnd��ʾû�и�·���� // MEDIA_TYPE_PCMA ==> emAG711a( mtmp )
    EmVideoFormat m_emPrimoVideo;         //��Ƶ1��emVEnd��ʾû�и�·���� // kdvdef.h
    EmVideoFormat m_emSecondVideo;         //��Ƶ2��emVEnd��ʾû�и�·����
} TVODFileInfo, *PTVODFileInfo;

//
// Request Play Info from UI
//
typedef struct tagTRequestPalyInfo {
	s8  m_achFileName[ MT_MAX_NAME_LEN + 1 ]; // �ļ���
	u8  m_bStart; // ( TRUE / ��, FALSE ) �Ƿ�������ʼ����
    EmAudioFormat m_emAudio;          //��Ƶ�� emAEnd��ʾû�и�·���� // MEDIA_TYPE_PCMA ==> emAG711a( mtmp )
    EmVideoFormat m_emPrimoVideo;         //��Ƶ1��emVEnd��ʾû�и�·���� // kdvdef.h
    EmVideoFormat m_emSecondVideo;         //��Ƶ2��emVEnd��ʾû�и�·����
} TRequestPalyInfo, *PTRequestPalyInfo;
//****************************************** VOD ********************************************

typedef struct tagTVideoSourceInfo
{
	EmMtVideoPort  emVideoPort;
	s8  achPortName[MT_MAX_PORTNAME_LEN +1];
}TVideoSourceInfo,*PTVideoSourceInfo;


//�ⲿ������Ϣ
typedef struct tagTExternalMatrixInfo 
{
	s8 TypeName[ MT_MAX_AVMATRIX_MODEL_NAME ]; //�����ͺ�
	u8 OutPort; //ָ��������Ͽں�
	u8 InPortTotal; //����˿����� 
public:
	tagTExternalMatrixInfo( ) { memset( this, 0, sizeof ( *this ) );	}

} TExternalMatrixInfo, *PTExternalMatrixInfo;


//�ڲ����󷽰��ṹ
typedef struct tagTAVInnerMatrixScheme
{
	  EmMatrixType  emType; //��������
		s8 achName[ MT_MAXLEN_AVMATRIX_SCHEMENAME + 1 ]; 
		u8 byIndex; 
		//
		// ����˿ں�( 1 - 6 )
		// ��������˿����ӵ�����˿�
		//
		u8 abyOutPort2InPort[ 6 ]; 
		//
		// �˿�( 1- 6 )����
		//
		s8 achOutPortName[6][ MT_MAXLEN_AVMATRIX_SCHEMENAME + 1 ];
		s8 achInPortName [6][ MT_MAXLEN_AVMATRIX_SCHEMENAME + 1 ];
public:
	tagTAVInnerMatrixScheme( )
	{
		memset( this, 0, sizeof ( struct tagTAVInnerMatrixScheme ) );
	}
}TAVInnerMatrixScheme,*PTAVInnerMatrixScheme;


//����Ϣ
typedef struct tagTMtSMS
{
	s8    abyText[MT_MAXLEN_SMSTEXT+1];
	EmSMSType emType;
	TMtId tMtSrc;
	u8    byDstNum;
	TMtId atMtDst[MT_MAXNUM_SMSDST];
	u8    bySpeed; //�ٶ� (1-5)
	u8    byRollTimes; //��������
//
public:
	tagTMtSMS(){memset(this ,0 ,sizeof( struct tagTMtSMS));	}
}TMtSMS ,*PTMtSMS;



//
//    ����֪ͨ���ݽṹ
//

//�û�����
typedef struct tagTUserCfgNotify
{
	EmLanguage  emLanguage;   //!����ָʾ 
    TOsdCfg     tOsdCfg;      //!����ָʾ 
    TUserCfg    tUserCfg;     //!�û�����
public:
	tagTUserCfgNotify(){memset(this ,0 ,sizeof( struct tagTUserCfgNotify));	}
}TUserCfgNotify ,*PTUserCfgNotify;



//��������ָʾ
typedef struct tagTNetConfigNotify
{
	TEthnetInfo  tEth0;    //!��̫��0ָʾ
	TEthnetInfo  tEth1;    //!��̫��1ָʾ
    TPPPOECfg    tPPOE;    //!PPPOE����
    TSerialCfg   tRS232;   //!232��������ָʾ
	TSerialCfg   tRS422;   //!422��������ָʾ 
	TSerialCfg   tRS485;   //!485��������ָʾ
    TE1Config    tE1;      //!E1ָʾ
    TRouteCfg    atRouteCfg[MT_MAX_ROUTE_NUM];  //!·��
    TEmbedFwNatProxy tEmbedPrxy;        //!����ǽָʾ
    TNATMapAddr  tNatMapAddr;           //!��̬NATӳ���ַָʾ
    //TRetransCfg                //!�����ش�
    TSNMPCfg     tSnmp;             //!��������
    TIPQoS       tQos;              //!Qosָʾ 
public:
	tagTNetConfigNotify(){memset(this ,0 ,sizeof( tagTNetConfigNotify));	}
}TNetConfigNotify ,*PTNetConfigNotify;


//����Ƶ����
typedef struct tagTAVConfigNotify
{
	TAVPriorStrategy  tAVPriorStrategy;        //!������ѡЭ��ָʾ
	TVideoEncodeParameter tPVEncParam;  //!����Ƶ�������
	TVideoEncodeParameter tSVEncParam;  //!����Ƶ�������
	EmMtVideoPort         emVideoSrc;    //!��ƵԴָʾ
	TImageAdjustParam     tImgAdjustParam;   //!��Ƶ��������
	EmVideoStandard    emPriomVideoStandard;       //!��Ƶ��ʽָʾ  MTCTRL->UI
	EmVideoStandard    emSecondVideoStandard;      //!��Ƶ��ʽָʾ  MTCTRL->UI
	BOOL      bIsAEC;                //!AECָʾ     MTCTRL->UI 
	BOOL      bIsAGC;                //!AGCָʾ
	u8        abyInVolume;                //!��������ָʾ
	u8        abyOutVolume;                      //!�������ָʾ
	TStreamMedia     tStreamMedia;         //!��ý��

public:
	tagTAVConfigNotify(){memset(this ,0 ,sizeof( struct tagTAVConfigNotify));	}

}TAVConfigNotify ,*PTAVConfigNotify;
/************************************************************************/
/* ���������Ϣ���ݽṹ�������                                         */
/************************************************************************/

//////////////////////////////////////////////////////////////////////////
//����ʱ�Զ�������������
typedef struct tagTBitRateAdjust
{
	BOOL bEnable;   //�Ƿ�����
	u16  wInterval;  //ʱ����
	u16  wLostTimes; //��������
	u16  wDescRatio; //���ٱ���
	u16  wLowDescRatio; //���ٽ�������
	u16  wDelayInterval;  //�ӳ�ʱ��
}TBitRateAdjust,*PTBitRateAdjust;

//////////////////////////////////////////
//������ͳ����Ϣ
typedef struct tagTCodecPackStat
{
	u16    wBitrate;       //ʵʱ����
	u16    wAvrBitrate;    //ƽ������
	u32    dwLostPacks;    //������
	u32    dwTotalPacks;   //�շ�����
	u32    dwFrames;       //�շ�֡��
	u8     byLostFrameRate;//��֡��(2��)%d     
}TCodecPackStat,*PTCodecPackStat;

//��Ƶͳ����Ϣ
typedef struct tagTVideoStatistics
{
	BOOL             bH239;	
	EmVideoFormat     emFormat;      //��ʽ
	EmVideoResolution emRes;
	TCodecPackStat    tPack;
}TVideoStatistics ,*PTVideoStatistics;

//��Ƶͳ����Ϣ
typedef struct tagTAudioStatistics
{
	EmAudioFormat     emFormat;      //��ʽ
	TCodecPackStat    tPack;
}TAudioStatistics ,*PTAudioStatistics;

/************************************************************************/
/* PCMT��ƵԴ��Ϣ Add by FangTao                                                       */
/************************************************************************/
typedef struct tagTCapType
{
	s8 m_achCapFileName[MT_MAX_FULLFILENAME_LEN+1];
	EmCapType emCapType;
public:
	tagTCapType(){ memset(this ,0 ,sizeof( tagTCapType)); }
}TCapType ,*PTCapType;

//����ͳ����Ϣ
typedef struct tagTCallInfoStatistics
{
	u16               wCallBitrate;//��������
	EmEncryptArithmetic emRecvEncrypt; //������ռ�������
	EmEncryptArithmetic emSendEncrypt; //���鷢�ͼ�������
	TAudioStatistics  tRecvAudio;//��Ƶ���ո�ʽ
	TAudioStatistics  tSendAudio;//��Ƶ���͸�ʽ

	TVideoStatistics tPrimoRecvVideo;  //����Ƶ����ͳ����Ϣ 
	TVideoStatistics tPrimoSendVideo;  //����Ƶ����ͳ����Ϣ 
	TVideoStatistics tSencodRecvVideo; //�ڶ�·��Ƶ����ͳ����Ϣ 
	TVideoStatistics tSecondSendVideo; //�ڶ�·��Ƶ����ͳ����Ϣ 
public:
	tagTCallInfoStatistics()
	{
		wCallBitrate = 0;
		emRecvEncrypt = emEncryptNone;
		emSendEncrypt = emEncryptNone;
		tRecvAudio.emFormat = emAEnd;
		memset(&tRecvAudio.tPack,0,sizeof(tRecvAudio.tPack));
		memcpy(&tSendAudio,&tRecvAudio,sizeof(tSendAudio));

		tPrimoRecvVideo.bH239 = FALSE;
		tPrimoRecvVideo.emFormat = emVEnd;
		tPrimoRecvVideo.emRes = emVResEnd;
		memset(&tPrimoRecvVideo.tPack,0,sizeof(tPrimoRecvVideo.tPack));

		memcpy(&tPrimoSendVideo,&tPrimoRecvVideo,sizeof(tPrimoRecvVideo));
		memcpy(&tSencodRecvVideo,&tPrimoRecvVideo,sizeof(tPrimoRecvVideo));
		memcpy(&tSecondSendVideo,&tPrimoRecvVideo,sizeof(tPrimoRecvVideo));
	}

}TCallInfoStatistics,*PTCallInfoStatistics;


/************************************************************************/
/* ˫������Ϣ                                                           */
/************************************************************************/
typedef struct tagTDualStreamBoxInfo
{
	BOOL   m_bUsed;
	u32_ip m_dwIp;
	u16    m_wPort;
}TDualStreamBoxInfo,*PTDualStreamBoxInfo;


/************************************************************************/
/* ������ͳ����Ϣ                                                       */
/************************************************************************/



/************************************************************************/
/* �ļ�����ģ�鿪ʼ
/************************************************************************/

// �汾��
#define		MTC_MT_FILE_VER		0x0100		// Ver1.0
#define     MTB_MT_FILE_VER     0x0100      // Ver1.0 add wangliang 2007/01/08

// �ļ�Ƭ��ţ�ѭ��ʹ��
#define		MTC_MT_MSG_BGN		0			// ����ʼ��Ϣ��Э����
#define     MTB_MT_MSG_BGN      0           // ����ʼ��Ϣ��Э���� add wangliang 2007/01/08


// �ļ���������
#define		MT_OSD_LABEL		0			// �ն�̨��
#define		MT_OSD_RUNMSG		1			// �ն˹�����Ϣ
#define		MT_OSD_TITLE		2			// �ն���Ļ

// �ܾ�ԭ��
#define		NACK_REASON_OK			0		// ����ɹ�
#define		NACK_REASON_LOSTPACK	1		// ����
#define		NACK_REASON_TIMEOUT		2		// ��ʱ
#define		NACK_REASON_TYPEERR		3		// ���Ͳ�ƥ��
#define		NACK_REASON_LENERR		4		// ��Ϣͷ���ȳ���
#define		NACK_REASON_VERERR		5		// �汾�Ų�ƥ��
#define		NACK_REASON_PROTOCOL	6		// Э���ų���
#define		NACK_REASON_RECVING		7		// ���ڴ���
#define		NACK_REASON_FILEERR		8		// �ļ�����ȷ
#define     NACK_REASON_UNKNOWTYPE  9       // δ�����ļ�����
#define     NACK_REASON_SAVEFILEERROR 10    //�����ļ�����
#define     NACK_REASON_MTSTATUSERROR 11    //�ն�״̬����ȷ



typedef  struct tagTFileTransfer 
{
public:
	//�汾��		0x0100
	u16 wProtocolIdentifer;

	// �ļ����䵱ǰ��Ƭ��ţ�0��ʼ��ţ�ѭ�����
	u16 wFileSliceNo;


	//�ļ�����		̨��,������Ϣ,��Ļ
	u8 byType;

	//ԭ��
	u8 byRspInfo;
	
	//�ļ���ʶ��
	u16 wFileIdentifer;

	//��Ϣ����󳤶�
	u16 wPakageMaxLen;

	//�û����ݳ��ȣ�ֵ=��Ϣͷ+�ļ���Ƭ����
	u16 wDataLen;

}TFileTransfer,*PTFileTransfer;

/************************************************************************/
/* ˫���нṹ����
/************************************************************************/
typedef struct tagTDVBNetSession
{
	 //[xujinxing]
	 u32_ip   m_dwRTPAddr; /*RTP��ַ*/
	 u16    m_wRTPPort;  /*RTP�˿�*/
	 u32_ip   m_dwRTCPAddr;/*RTCP��ַ*/
	 u16    m_wRTCPPort; /*RTCP�˿�*/
}TDVBNetSession,*PTDVBNetSession;

typedef struct tagTDVBNetSndParam
{
	 u8 m_byNum;
	 TDVBNetSession m_tLocalNet;
	 TDVBNetSession m_tRemoteNet[MT_MAX_NETSND_DEST_NUM];
}TDVBNetSndParam,*PTDVBNetSndParam;

typedef struct tagTDVBLocalNetParam
{
	 TDVBNetSession  m_tLocalNet;
	 u32_ip        m_dwRtcpBackAddr;/*RTCP�ط���ַ*/
	 u16         m_wRtcpBackPort; /*RTCP�ط��˿�*/        
}TDVBLocalNetParam,*PTDVBLocalNetParam;

typedef struct tagTDVBVideoEncParam
{
	u8  m_byEncType;   /*ͼ���������*/
    u8  m_byRcMode;    /*ͼ��ѹ�����ʿ��Ʋ���*/
    u16  m_byMaxKeyFrameInterval;/*I֡�����P֡��Ŀ*/
    u8  m_byMaxQuant;  /*�����������(1-31)*/
    u8  m_byMinQuant;  /*��С��������(1-31)*/
    u8  m_byReserved1; /*����*/
    u8  m_byReserved2; /*����*/
    u8  m_byReserved3; /*����*/
    u16  m_wBitRate;    /*���������(Kbps)*/
    u16  m_wReserved4;  /*����*/		
	u32 m_dwSndNetBand;/*���緢�ʹ���(��λ:Kbps,1K=1024)*/   
    u32 m_dwReserved5; /*����*/

	u8    m_byPalNtsc;    /*ͼ����ʽ(PAL��NTSC)*/	
	u8    m_byCapPort;    /*�ɼ��˿ں�: 1~7��Ч����KDV8010�ϣ�S���Ӷ˿ں�Ϊ7��C���Ӷ˿ں�ȱʡΪ2*/
	u8	m_byFrameRate;  /*֡��(default:25)*/	
	u8    m_byImageQulity;/*ͼ��ѹ������,0:�ٶ�����;1:��������*/
	u8    m_byReserved6;  /*����*/
	u16	m_wVideoWidth;  /*ͼ����(default:640)*/
	u16	m_wVideoHeight; /*ͼ��߶�(default:480)*/


	u8  m_byFrameFmt; // �ֱ��� 0:���̶�����, 1: half cif, 2: 1 cif, 3: 2 cif,  4: 4 cif,  5: ����ͼ��ϳɱ���  
	u8	m_byFrmRateCanSet; //֡���Ƿ�������趨 ?
	u8  m_byFrmRateLittleThanOne; // ֡���Ƿ�С��1 ?
}TDVBVideoEncParam,*PTDVBVideoEncParam;

typedef struct tagTSitecallInformation
{
	BOOL m_bIsCustomConf;
	s8 m_achConfName[MT_MAX_CONF_NAME_LEN+1];
	s8 m_achConfPwd[MT_MAXLEN_PASSWORD+1];
	BOOL m_bLocalPay;	
	s8 m_achConfCard[MT_MAX_CONF_NAME_LEN+1];
	s8 m_achConfCardPwd[MT_MAXLEN_PASSWORD+1];
	u8 m_byMultiPicCount;
	EmCallRate        m_emRate;
	EmVideoFormat     m_emVideoType;
	EmVideoResolution m_emVideoFormat;
	EmAudioFormat     m_emAudioFormat;
	u8   m_byTerninalCount;
	TMtAddr m_atTerminalE164Info[MT_MAX_CALLSITE_TERNINAL];
	TMtAddr m_atTerminalH323Info[MT_MAX_CALLSITE_TERNINAL];
public:
	tagTSitecallInformation()
	{
		memset(this,0,sizeof(tagTSitecallInformation));
		m_emRate = emRaten64;
		m_emVideoType = emVH263;
		m_emVideoFormat = emVCIF;
		m_emAudioFormat = emAG711a;
		m_bLocalPay = TRUE;
	}
}TSitecallInformation,*PTSitecallInformation;

typedef struct tagTTransparentSerial
{
    EmSerialType  m_emComType;
	u32           m_nDataLen;
    s8            m_achData[128];
}TTransparentSerial,*PTTransparentSerial;

typedef struct tagTPeerCapabilityInfo
{
    BOOL m_bMix;                        //����
    BOOL m_bVAC;                        //��������
    BOOL m_bCustomVMP;                  //���ƻ���ϳ�
    BOOL m_bAutoVMP;                    //�Զ�����ϳ�
    BOOL m_bEndConf;                    //��������
    BOOL m_bInvateMt;                   //�����ն�
    BOOL m_bDropMt;                     //ɾ���ն�
    BOOL m_bSelSpeaker;                 //ָ��/ȡ��������
    BOOL m_bSelChair;                   //ָ��/������ϯ
    BOOL m_bSelSource;                  //ѡ���ն�
    BOOL m_bFECC;                       //Զ������ͷң��
    BOOL m_bQuiet;                      //Զ�˾���
    BOOL m_bMute;                       //Զ������
    BOOL m_bConfReq;                    //�������봦��
    BOOL m_bOnlineList;                 //�����ն��б�
    BOOL m_bOfflineList;                //�����ն��б�
	BOOL m_bPicSwitch;                  //�����л�
	BOOL m_bSelSpeakingUser;            //���� 
	BOOL m_bForceBroadcast;             //ǿ�ƹ㲥
}TPeerCapabilityInfo,*PTPeerCapabilityInfo;


/************************************************************************/
/*              ��ݼ��ṹ��                                            */
/************************************************************************/
typedef struct tagTRapidKey 
{
	EmMtVideoPort emMainVidSrcBtn;
	EmHotKeyType emRedBtn;
	EmHotKeyType emYellowBtn;
	EmHotKeyType emGreenBtn;
}TRapidKey, *PTRapidKey;

/************************************************************************/
/*              �ն����������ṹ��                                      */
/************************************************************************/
//�����汾�е��ļ���Ϣ
typedef struct tagTVerFileInfo
{
    EmMtOSType   m_emFileType;       //�ļ�����(Linux,Vxworks)
    u32          m_dwFileSize;       //�ļ���С(���ֽڼ���)
    s8           m_szFileName[MT_MAX_FILESRV_FILENAME_LEN];//�ļ���
}TVerFileInfo, *PTVerFileInfo;

//�����汾����Ϣ
typedef struct tagTVerInfo
{ 
    u8 m_byFileNum;
    s8 m_achBugReport[MT_MAX_FILESRV_BUG_REPORT_LEN];
    s8 m_achSoftVer[MT_MAX_FILESRV_SOFTWARE_VER_LEN];
    TVerFileInfo m_atVerFile[MT_MAX_FILESRV_DEVFILE_NUM];
}TVerInfo,*PTVerInfo;


// FEC������Ϣ�ṹ
typedef struct tagTFecInfo
{
	BOOL   m_bEnableFec;   //��Ƶ�Ƿ�ʹ��ǰ�����
	u8     m_byAlgorithm;  //����fec�㷨 0:NONE 1:RAID5 2:RAID6
	u16    m_wPackLen;     //����fec���а�����
	BOOL   m_bIntraFrame;  //�Ƿ�֡��fec
	u32    m_dwDataPackNum; //����fec��x�����ݰ�
	u32    m_dwCrcPackNum;  //����fec��y�������
public:
	tagTFecInfo(){ memset( this, 0, sizeof( struct tagTFecInfo ) ); }
}TFecInfo,*PTFecInfo;


//xjx_071221, ��������þ���ı���ṹ
//�ú�����������ڵĸ����ɡ�
//�ڲ����󷽰��ṹ
typedef struct tagTHDAVInnerMatrixScheme
{
	    EmMatrixType  emType; //��������
		s8 achName[ MT_MAXLEN_AVMATRIX_SCHEMENAME + 1 ]; 
		u8 byIndex; 
		BOOL bUseVgaIn;   //YPrPb2��vga����˿ڸ��ã�ʹ����vga
		BOOL bUseVgaOut;  //YPrPb2��vga����˿ڸ��ã��Ƿ�ʹ��vga
		// ����˿ں�( 1 - 4 ),	��������˿������ӵ�����˿�
		u8 abyOutPort2InPort[ MT_HD_MAX_AV_OUTPORT_NUM ]; 
		// �˿�( 1-4 )����, ��Ӧ�����ϵ�����
		s8 achOutPortName[MT_HD_MAX_AV_OUTPORT_NUM][ MT_MAXLEN_AVMATRIX_SCHEMENAME + 1 ];
		s8 achInPortName [MT_HD_MAX_AV_OUTPORT_NUM][ MT_MAXLEN_AVMATRIX_SCHEMENAME + 1 ];
public:
	tagTHDAVInnerMatrixScheme( )
	{
		memset( this, 0, sizeof ( struct tagTHDAVInnerMatrixScheme ) );
	}
}THDAVInnerMatrixScheme,*PTHDAVInnerMatrixScheme;



template < class T >
T SAFE_CAST( T &t, u8 *p )
{
	if (NULL == p) return t;
	memcpy( &t, p, sizeof( T ) );
	return t;
}



char* GetStructVersion();

/************************************************************************/
/* �ն�Ӳ���Զ������� wl 2007/02/06
/************************************************************************/


// ������Ϣͷ�ṹ
typedef struct tagTMtAutoTestMsg
{	
    // �汾�� 0x0100
    u16  m_wProtocolIdentifer;
    // �������
    EmAutoTestType m_emAutoTestType;
    // ��ʼ���Ա�־λ��TRUE����ʼ���ԣ�FALSE��ֹͣ����
    BOOL m_bAutoTestStart;
    // ��Ƶ����
    EmVideoType m_emVideoType;
    // ��Ƶ�˿�
    EmMtVideoPort m_emVideoPort;
    // ���Խ��: 0��ʧ��; 1���ɹ���
    BOOL m_bAutoTestReult; 
    // ����ʧ��ԭ��:1, Ӳ�����Խ��ʧ��; 2, �������ڽ���;  
    // 3, ��֧�ָ������; 4,�ն�״̬����;5, ��������汾�Ŵ���; 
    // �������ӷ�����Ϣ
    u8 m_byErrorReason;
    //�绰����
    s8 m_achPhoneNumber[32+1]; 
    //�ֻ�����
    s8 m_achSubPhoneNumber[32+1];
    //E1 ������ַ
    u32_ip m_dwLocalIp;
    //E1 ping��ַ
    u32_ip m_dwDstIp;
}TMtAutoTestMsg, *PTMtAutoTestMsg;

// ��ַ����Ŀ��Ϣ
typedef struct tagTAddrEntry
{
    s8      m_achEntryName[ MT_MAX_NAME_LEN + 1 ];		// ��Ŀ����
    s8      m_achIpaddr[ MT_MAX_H323ALIAS_LEN + 1 ];	// ���е�ַ
    u32     m_dwCallTime;                               // ����ʱ��
    u16	    m_wCallRate;								// ��������
	BOOL    m_bTeleCall;
}TAddrEntry,*PTAddrEntry;


/************************************************************************/
/* ���ô��������ν��������                                           */
/************************************************************************/
typedef struct tagTPxyIPCfg
{
    BOOL bUsed;
    u32_ip dwIP;         //IP��ַ
    u32_ip dwMask;       //��������
    u32_ip dwGateway;    //���ص�ַ
}TPxyIPCfg, *PTPxyIPCfg;

typedef struct tagTWifiLinkInfo
{
    s8       achSSID[MT_MAX_NAME_LEN];            /* WiFi network name */
	s8       achBssMacAddr[MT_MAX_NAME_LEN];      /* Bss Mac Address*/
    u32      dwSignalLevel;                         /*Signal level : 0-100*/
    u32      dwLinkRate;				 	        /* Rate, unit: mpbs */
    u32      dwChannel;                             /* Wireless channel number*/
    u8       byAuthType;                            /*Authentication method*/
    u8       byEncrytoType;                         /*Encryto method*/
	u8       byNetType;                             /*ad-hoc or infra*/
}TWifiLinkInfo, *PTWifiLinkInfo;

typedef struct tagTWifiScanResult
{
    u32 dwBssNum;   /*Number of Link info, 0 -- WIFI_MAX_BSS_NUM*/
    TWifiLinkInfo tLinkInfo[WIFI_MAX_BSS_NUM];
} TWifiScanResult, *PTWifiScanResult;

typedef struct tagTWifiLinkCfgItem
{
	u32  dwId;
    s8   achCfgName[WIFI_MAX_NAME_LEN]; /*Configure name, shall not be empty when need save */
    TWifiLinkInfo tWifiLinkInfo;           /* Wifi link basic info*/

    s8   achWepKey1[WIFI_MAX_KEY_LEN];     /*Web key, shall not be null when byAuthType is Web*/
    s8   achWepKey2[WIFI_MAX_KEY_LEN];
    s8   achWepKey3[WIFI_MAX_KEY_LEN];
    s8   achWepKey4[WIFI_MAX_KEY_LEN];
    s8   achWpaPasswd[WIFI_MAX_KEY_LEN];  /*Wpa key, shall not be null when byAuthType is wpa or wpa2*/
}TWifiLinkCfgItem, *PTWifiLinkCfgItem;


typedef struct tagTWifiCfgInfo
{
    u32 dwCfgNum;
    TWifiLinkCfgItem tCfgInfo[WIFI_MAX_CFG_NUM];
} TWifiCfgInfo, * PTWifiCfgInfo;

typedef struct tagTWifiLinkStat
{
    EmWiFiLinkStat emState;
    TWifiLinkCfgItem tItem;  /*the state corresponds to this link*/
} TWifiLinkStat, *PTWifiLinkStat;

//ɨ��SSID��Ϣ
typedef struct tagTWiFiBSSIDInfo
{
    u8                      byPSKOFF;                              //PSK or enterprise
    EmWiFiNetType           emNetType;                             //��������
    EmWiFiEncryptType       emEncryptType;                         //��������
    EmWiFiEncryptArithmetic emArithType;                           //�����㷨
    BOOL                    bEncrypt;                              //�Ƿ����
    u32                     dwChannel;                             //Wireless channel number
    u32                     dwBeaconInterval;                      //BCN��ʡ�������
    u32                     dwLinkRate;				 	           //Rate, unit: mpbs 
    u32                     dwSignalLevel;                         //�ź�ǿ��
    s8                      achBSSMacAddr[MT_WIFI_MAX_NAME_LEN];   // Bss Mac Address
    s8                      achSSID[MT_WIFI_MAX_NAME_LEN];         //WiFi network name
    s8                      achPasswd[MT_WIFI_MAX_PASSWORD_LEN];   //��������
public:
    tagTWiFiBSSIDInfo()
    {
        memset( this, 0, sizeof( struct tagTWiFiBSSIDInfo ) );
    }
}TWiFiBSSIDInfo, *PTWiFiBSSIDInfo;

//����SSID��Ϣ,�Ƿ���Ҫ����������������Ϣ��ʡ��ģʽ,��ȷ��?
typedef struct tagTWiFiBSSIDCfg
{
    u8                      byPSKOFF;                              //PSK or enterprise
    EmWiFiNetType           emNetType;                             //��������
    EmWiFiEncryptType       emEncryptType;                         //��������
    EmWiFiEncryptArithmetic emArithType;                           //�����㷨
    BOOL                    bEncrypt;                              //�Ƿ����
	u32                     dwCfgPri;                              //�����������ȼ�
    u32                     dwChannel;                             //Wireless channel number
    s8                      achSSID[MT_WIFI_MAX_NAME_LEN];           //WiFi network name
    s8                      achPassword[MT_WIFI_MAX_PASSWORD_LEN];   //��������
    //u8                      byCfgType;                             //ɨ���������
    //s8                      achBSSMacAddr[MT_WIFI_MAX_NAME_LEN];   // Bss Mac Address
    //u32                     dwBeaconInterval;                      //BCN��ʡ�������
    //u32                     dwLinkRate;				 	           //Rate, unit: mpbs
    //u32                     dwSignalLevel;                         //�ź�ǿ��
public:
    tagTWiFiBSSIDCfg()
    {
        memset( this, 0, sizeof( struct tagTWiFiBSSIDCfg ) );
    }
}TWiFiBSSIDCfg, *PTWiFiBSSIDCfg;

typedef struct tagTGkRegInfo
{
	BOOL bRegisted;
	EmGKRegFailedReason emReason;
}TGkRegInfo, *PTGkRegInfo;

typedef struct tagTSiteCallResult
{
	BOOL bRegisted;
	EmGKRegFailedReason emReason;
}TSiteCallResult, *PTSiteCallResult;


typedef struct tagTHdAudioPort
{
	EmHDAudPort emHdAudPortIn;
	EmHDAudPort emHdAudPortOut;
}THdAudioPort, *PTHdAudioPort;

typedef struct tagTMiscCfg
{
	BOOL bUseSlice; //�����а�����
}TMiscCfg, *PTMiscCfg;

typedef struct tagTVideoDisplayFrm
{
    u32 m_dw1080PFrm;
    u32 m_dw1080IFrm;
    u32 m_dwVGAFrm;
    u32 m_dw720PFrm;
    u32 m_dwSDFrm;
public:
		tagTVideoDisplayFrm()
		{
			memset( this, 0, sizeof( struct tagTVideoDisplayFrm ) );
    }
}TVideoDisplayFrm,*PTVideoDisplayFrm;

#endif //!_MT_STRUCT_H_
