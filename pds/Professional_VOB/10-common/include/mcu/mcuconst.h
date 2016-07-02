/*****************************************************************************
   ģ����      : mcu
   �ļ���      : mcuconst.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: MCU��������
   ����        : ������
   �汾        : V3.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/10/13  3.0         ������       ����
   2005/02/19  3.6         ����       �����޸�
******************************************************************************/
#ifndef __MCUCONST_H_
#define __MCUCONST_H_

#ifndef _EXCLUDE_VCCOMMON_H
#include "vccommon.h"
#endif

//OS����
#define     OS_TYPE_VXWORKS                 1   //Vx
#define     OS_TYPE_LINUX                   2   //Linux
#define     OS_TYPE_WIN32                   3   //Windows


// [11/25/2010 liuxu][���]
#define     KDV_NAME_MAX_LENGTH             (u8)128	// �����ļ������ȣ�������·����

//֧�ֵ�IP����
#define IP_NONE 0
#define IP_V4	1
#define IP_V6	2
#define IP_V4V6	3

//[pengguofeng 5/11/2012]  IPV6�ַ�������
#define		IPV6_STR_LEN					46

//[4/8/2013 liaokang] ���뷽ʽ
enum emenCodingForm
{
    emenCoding_Start = 0,
    emenCoding_Utf8,
    emenCoding_GBK
};

//�豸���ඨ��---MCU���Ͷ���
// ����vccommon.h�еĶ���
// MCU_TYPE_KDV8000
// MCU_TYPE_KDV8000B
// MCU_TYPE_KDV8000C
// MCU_TYPE_WIN32

//�豸���ඨ��---�������Ͷ���
#define		EQP_TYPE_MIXER                   1   //������
#define		EQP_TYPE_VMP                     2   //ͼ��ϳ���
#define		EQP_TYPE_RECORDER                3   //¼���
#define		EQP_TYPE_BAS                     4   //����������
#define		EQP_TYPE_TVWALL                  5   //����ǽ
#define		EQP_TYPE_DCS                     6   //���ݷ�����
#define     EQP_TYPE_PRS                     7   //���ش�
#define     EQP_TYPE_FILEENC                 8   //�ļ�������
#define		EQP_TYPE_VMPTW                   9   //ͼ��ϳɵ���ǽ
//4.6�汾 �¼� jlb
#define		EQP_TYPE_HDU					10	 //�������ֵ���ǽ// xliang [12/5/2008]
#define     EQP_TYPE_HDU_SCHEME             11   //hduԤ��ģ����������//zjl[20091208]
//#define		EQP_TYPE_EMIXER					12	 //tianzhiyong 20100309 ��ǿ�Ի�����
#define     EQP_TYPE_HDU_H					13   //zjl 20100426 ����hdu��ͨ������
#define     EQP_TYPE_HDU_L					14   //zjl 20100426 ����hdu����������

// 4.7�汾 [1/31/2012 chendaiwei]
#define     EQP_TYPE_HDU2					15	 //�������ֵ���ǽ2��
//#define     EQP_TYPE_APU2_MIXER				16	 //APU2�Ļ�����
#define     EQP_TYPE_HDU2_L					12  //�������ֵ���ǽ2���Ͷ�

//tianzhiyong 20100309 ��ǿ�Ի���������ģʽ
#define     EMIXER_MODE1                    (u8)0   //EAPU������ģʽ1
#define     EMIXER_MODE2                    (u8)1   //EAPU������ģʽ2
#define     EMIXER_MODE3                    (u8)2   //EAPU������ģʽ3
//tianzhiyong 2010/03/21 ����������ģʽ���ֶ����Զ�
enum EmStartMixerMode
{
	emStartMixerAutoMode = 0,
	emStartMixerSelectMode
};

//�������������Ͷ���[2/14/2012 chendaiwei]
#define		UNKONW_MIXER	0
#define		APU2_MIXER		1
#define		APU_MIXER		2
#define		EAPU_MIXER		3
#define		MIXER_8KG		4
#define		MIXER_8KH		5
#define     MIXER_8KI		6

// ���������������ͣ��ڲ�ʹ��
#define		TYPE_MPUSVMP				11	 //MPU-��VMP
#define		TYPE_MPUDVMP				12   //MPU-VMP
#define		TYPE_MPUBAS					13	 //MPU-BAS
#define		TYPE_EVPU					14	 //EVMP
#define		TYPE_EBAP					15   //EBAP
#define     TYPE_MPUBAS_H				16   //MPU������ģʽ
#define     TYPE_MPU2BAS_ENHANCED		17   //MPU2 Bas(Enhanced)����ģʽ
#define     TYPE_MPU2BAS_BASIC			18   //MPU2 Bas(Basic)����ģʽ
#define     TYPE_MPU2VMP_ENHANCED		19   //MPU2 VMP(Enhanced)����ģʽ
#define     TYPE_MPU2VMP_BASIC			20   //MPU2 VMP(Basic)����ģʽ
#define		TYPE_APU2BAS			    21   //APU2 BAS

// VMP�����Ͷ���---------xliang [12/5/2008] ----------
#define		VMP								(u8)0	//��vpu (Ĭ��ֵ0)
#define		EVPU_SVMP						(u8)1	//EVPU-��VMP
#define		EVPU_DVMP						(u8)2	//EVPU-˫VMP
#define		MPU_SVMP						(u8)3	//MPU-��VMP
#define		MPU_DVMP					    (u8)4	//MPU-˫VMP
#define		MPU_BAS							(u8)5	//mpu-bas
#define		MPU_EBAP						(u8)6	//EBAP
#define		VMP_8KE							(u8)7	//8000E-VMP
#define     MPU2_VMP_BASIC					(u8)8	//MPU2 vmp-basic
#define		MPU2_VMP_ENHACED				(u8)9	//MPU2 vmp-enhanced
#define		VMP_8KH							(u8)10	//8000H-VMP
#define		VMP_8KI							(u8)11	//8000H-VMP

//MPU board version
#define		MPU_BOARD_A128					(u8)0	//MPU A ��
#define		MPU_BOARD_B256					(u8)1	//MPU B ��
#define     MPU2_BOARD                      (u8)2   //mpu2�����ӿ���
#define     MPU2_BOARD_ECARD                (u8)3   //mpu2 ���ӿ���

//VMP���ͨ����Ŀ
#define		MAXNUM_MPU2_OUTCHNNL			(u8)9	//MPU2 9��
#ifdef _8KI_
#define		MAXNUM_MPU_OUTCHNNL				(u8)6	//8KIVMP  6��
#elif defined(_8KH_)
#define		MAXNUM_MPU_OUTCHNNL				(u8)5	//8KHVMP  5��
#else
#define		MAXNUM_MPU_OUTCHNNL				(u8)4	//MPU  4��
#endif
#define		MAXNUM_EVPU_OUTCHNNL			(u8)2	//EVPU 2��

//MPU����ǰ����ͨ����Ŀ
#define		MAXNUM_SVMP_HDCHNNL				(u8)3	//SVMPǰ����ͨ������3   
#define		MAXNUM_DVMP_HDCHNNL				(u8)1	//DVMPǰ����ͨ������1
#define		MAXNUM_SVMPB_HDCHNNL			(u8)3	//SVMP B��ǰ����ͨ����: 3
#define		MAXNUM_MPU2VMP_BS_HDCHNNL		(u8)2	//MPU2 VMP(Basic)ǰ����ͨ����: 2
#define		MAXNUM_MPU2VMP_EH_HDCHNNL		(u8)8	//MPU2 VMP(Enhanced)ǰ����ͨ����: 8
#define		MAXNUM_MPU2VMP_E20_HDCHNNL		(u8)20	//MPU2 ���ǰ����ͨ����: 20

//8KGVMP����ǰ����ͨ����Ŀ
#define		MAXNUM_8KEVMP_HDCHNNL			(u8)0	//���� 1080p60/50fps����
//8KHVMP����ǰ����ͨ����Ŀ
#define		MAXNUM_8KHVMP_ADPCONF1080P60_HDCHNNL	(u8)0       //���� 1080p60/50fps����
#define		MAXNUM_8KHVMP_ADPCONF1080P30_HDCHNNL	(u8)1       //���� 1080p30/25fps����
#define		MAXNUM_8KHVMP_ADPCONF_HDCHNNL			(u8)3       //�������720��720���»���
#define		MAXNUM_8KHVMP_CONF1080P60_HDCHNNL		(u8)1       //������ 1080p60/50fps����
#define		MAXNUM_8KHVMP_CONF1080P30_HDCHNNL		(u8)3       //������ 1080p30/25fps����
#define		MAXNUM_8KHVMP_CONF720P_HDCHNNL			(u8)9       //������ 720����
#define		MAXNUM_8KHVMP_CONF_HDCHNNL				(u8)16      //������720���»���,��16ǰ����
//yanghuaizhi 2012/02/16 ����ϳ������ͨ��״̬,�Ƿ�Active
enum EmVmpOutChnlStatus
{
	emVmpOutChnlInactive = 0,	//��Ч
	emVmpOutChnlActive,			//����
	emVmpOutChnlNotChange		//����ԭ��״̬
};

//���������������·������Ӧ��Ƶ��������Ƶ��˫����MAX_CONF_CAP_EX_NUM����ѡ��BAS���仹��Ҫ��������˫��Ԥ��1·����˫��H263+ XGA�� H264 XGA����
#define		MAX_CONF_BAS_ADAPT_NUM			MAX_CONF_CAP_EX_NUM+2	

//MPU���ܷ�h264 CIF����
#define		MAXNUM_SVMP_NON264CIFCHNNL		(u8)20	// xliang [09/11/3] ����

//MPU2�������vicp��Դ����
#define		MAXNUM_MPU2_VICP_RES			(u16)3600

//Resolutions that can be acceptable
#define		MAXNUM_ACCEPTABLE_RES			(u8)57

//�豸���ඨ��---�ն����Ͷ���
#define		MT_TYPE_NONE                     0   //������Ҳ������
#define     MT_TYPE_MT                       3
#define		MT_TYPE_MMCU	                 4   //�ϼ�MCU
#define     MT_TYPE_SMCU                     5   //�¼�MCU
#define     MT_TYPE_VRSREC                   6   //vrs��¼���豸

// [10/29/2010 liuxu] ������MPEG���ָ�ʽ��ߵ����ʶ���, ��λKB
#define		MPEG4_MAX_BT_CIF				u16(1536)	//MP4 CIf������ʶ���
#define		MPEG4_MAX_BT_2CIF				u16(3072)	//MP4 2CIF������ʶ���
														//MP4�����ʱ�3072���߾���4CIF
// [10/29/2010 liuxu]over


//���̱�Ŷ���
#define		MT_MANU_KDC                      1   //�ƴ�
#define		MT_MANU_NETMEETING               2   //Netmeeting
#define		MT_MANU_POLYCOM                  3   //PolyCom
#define		MT_MANU_HUAWEI                   4   //��Ϊ
#define		MT_MANU_TAIDE                    5   //̩��
#define		MT_MANU_SONY                     6   //SONY
#define		MT_MANU_VCON                     7   //VCON
#define		MT_MANU_RADVISION                8   //Radvision
#define     MT_MANU_KDCMCU                   9   //kedamcu
#define		MT_MANU_PENTEVIEW                10  //��̩
#define		MT_MANU_VTEL                     11  //VTEL
#define     MT_MANU_ZTE                      12  //�����ն�
#define     MT_MANU_ZTEMCU                   13  //����MCU
#define     MT_MANU_CODIAN                   14  //CodianMCU
#define		MT_MANU_AETHRA					 15	 //Aethra
#define		MT_MANU_OTHER                    16  //����
#define     MT_MANU_CHAORAN                  17  //��Ȼ

//TvWallOutputMode
#define     TW_OUTPUTMODE_NONE               0   //�����ģʽ
#define     TW_OUTPUTMODE_AUDIO              1   //��Ƶ���ģʽ
#define     TW_OUTPUTMODE_VIDEO              2   //��Ƶ���ģʽ
#define     TW_OUTPUTMODE_BOTH               3   //����Ƶ���ģʽ

//HduOutputMode   jlb
#define     HDU_OUTPUTMODE_NONE               0   //�����ģʽ
#define     HDU_OUTPUTMODE_AUDIO              1   //��Ƶ���ģʽ
#define     HDU_OUTPUTMODE_VIDEO              2   //��Ƶ���ģʽ
#define     HDU_OUTPUTMODE_BOTH               3   //����Ƶ���ģʽ
//�ն�����ԭ��
//zjj20100329 ����û���쳣�Ķ���
#define		MTLEFT_REASON_NONE				 0	 //û���쳣
#define     MTLEFT_REASON_EXCEPT             1   //�쳣�����������ת��
#define     MTLEFT_REASON_NORMAL             2   //�����Ҷ�
#define     MTLEFT_REASON_RTD                3   //RTD��ʱ
#define     MTLEFT_REASON_DRQ                4   //DRQ
#define     MTLEFT_REASON_UNMATCHTYPE        5   //���Ͳ�ƥ��
#define		MTLEFT_REASON_DELETE			 6   //����ɾ��
//�ն�δ����ԭ��
#define     MTLEFT_REASON_BUSY               6   //�ն�æ
#define     MTLEFT_REASON_REJECTED           7   //�ն������Ҷ�
#define     MTLEFT_REASON_UNREACHABLE        8   //�ն˲��ɴ�
#define     MTLEFT_REASON_LOCAL              9   //
#define     MTLEFT_REASON_BUSYEXT            10  //�ն�æ,���ն�Ŀǰ���ڻ���ļ��𼰻�������
#define     MTLEFT_REASON_REMOTERECONNECT    11  //������������ͣ���Զ���Զ���������
// [pengjie 2010/6/2] �������ԭ�� Э��ջ->mcu
#define     MTLEFT_REASON_REMOTECONFHOLDING  12  // �����¼�MCUʧ�ܣ���MCU�����ٿ���������
#define     MTLEFT_REASON_REMOTEHASCASCADED  13  // �����¼�MCUʧ�ܣ���MCU�Ѿ��������߼���MCU����
// End
#define     MTLEFT_REASON_UNKNOW             0xff   //δ֪����


//�������ֱ��ʾܾ���ԭ��
#define		CASCADE_ADJRESNACK_REASON_NONEKEDA	1		//�ǿƴ�(�����µ�)
#define		CASCADE_ADJRESNACK_REASON_INTVWALL	2		//��DEC5�У������ϵ������ɿ��ǻ�ack��
#define		CASCADE_ADJRESNACK_REASON_INVMP		3		//��VMP��
#define		CASCADE_ADJRESNACK_REASON_UNKNOWN	0		//δ֪ԭ��

// [pengjie 2010/4/23] ������֡�ʾܾ�ԭ��
#define		CASCADE_ADJFPSNACK_REASON_NONEKEDA	1		//�ǿƴ�(�����µ�)
#define		CASCADE_ADJFPSNACK_REASON_INTVWALL	2		//��DEC5�У������ϵ���
#define		CASCADE_ADJFPSNACK_REASON_INVMP		3		//��VMP��
#define		CASCADE_ADJFPSNACK_REASON_DISCON	4		//�ն˲�����
#define		CASCADE_ADJFPSNACK_REASON_CANT  	5		//�ն˲�֧�ֵ�
#define		CASCADE_ADJFPSNACK_REASON_UNKNOWN	0		//δ֪ԭ��

//Э�����Ͷ���
#define		PROTOCOL_TYPE_H323				1  //H323
#define		PROTOCOL_TYPE_H320              2  //H320
#define		PROTOCOL_TYPE_SIP               3  //SIP

//MCU���ú�ҵ��������
#define     MAXNUM_H225H245_MT              192 //�ײ�Э��ջ����������
#define     MAXNUM_CONNTAUD_MT				192  //������������ն���
#define		MAXNUM_CONF_MT					192  //�������������MT��,ǣ�浽�ӿڣ�8000B��8000ά�ֲ��� 
#define		MAXNUM_CALL_CHANNEL             12   //ÿ�������е����ͨ����

#define     MAXNUM_MCU_VC                   16   //MCU֧��ͬʱ���ӵ����VCS��
#define		MAXNUM_MCU_MC			        16   //MCU���ӻ������̨�����
#define		MAXNUM_MCU_PERIEQP				182    //MCU�����������, ���� 6*7 ��HDU, ����λ��16������12��λ��
#define		MAXNUM_OLDMCU_PERIEQP			140    //MCU�����������, ����28��HDU, ����λ��16������12��λ��
//#define		MAXNUM_MCU_PERIEQP				(128+12)    //MCU�����������, ����28��HDU, ����λ��16������12��λ��

#define     MAXNUM_MONITOR_NUM				(u16)32	// ����豸�������Ŀ

#define		MAXNUM_CONF_SPY					128	 //һ���������spy��
#define     SPY_CHANNL_NULL                 65535 // ��ʾ�ü�����ش�ͨ��δ����
#define     DEFAULT_MAX_SENDSPY_BW          100000
#define     DEFAULT_MAX_RECVSPY_BW          100000

// ����MTU��С, zgc, 2007-04-02
#define		MAXNUM_MTU_SIZE					1468  //MTU��󳤶�
#define		MINNUM_MTU_SIZE					1308  //MTU��С����
#define		DEFAULT_MTU_SIZE				MAXNUM_MTU_SIZE  //MTUĬ�ϳ���

//USBKEY��ص����ݣ�[liu lijiu][20101028]            
#define    	MAXIANUM_OF_PAIRKEYVALUE           30  //һ��USBKEY�м�ֵ�Ե������Ŀ

#define     INVALID_MTVIDSRCPORTIDX          255 //��Ч���ն���ƵԴ�ӿ�����

// ȱʡ��DSC��MCU8000�ⲿIP���ڲ�ͨ��IP
#define DEFAULT_MCU_IP                          "192.168.2.1"
#define DEFAULT_DSC_IP                          "192.168.2.2"
#define DEFAULT_IPMASK        					"255.255.255.0"

#define DEFAULT_MCU_INNERIP                     "10.253.253.250"
#define DEFAULT_DSC_INNERIP                     "10.253.253.249"
#define DEFAULT_INNERIPMASK						"255.255.255.252"

#define ETH_IP_MAXNUM						    16
#define MAXNUM_MCSACCESSADDR					2

// MINIMCU���ڵ���������
#define	NETTYPE_INVALID						0
#define NETTYPE_LAN							1
#define NETTYPE_WAN							2
#define NETTYPE_MIXING_ALLPROXY				3
#define NETTYPE_MIXING_NOTALLPROXY			4

#ifdef _MINIMCU_    // 8000B ����

// �������ڲ����ݽṹ��������
#define     MAXNUM_MCU_MT                   (u16)128  //һ��MCU֧������ն�����ͬʱ���ߵ��նˣ� 
#define		MAXNUM_MCU_CONF					64        //һ��MCU�ڲ����õ�������������Χ
#define     MAXNUM_MCU_TEMPLATE             32        //���ģ�����(����16���ӵ�32������16���ָ�VCS)
#define     MAXNUM_MCU_VCSTEMPLATE          16        //VCS�ɴ�����ģ����
#define     MAXNUM_MCU_MCSTEMPLATE          (MAXNUM_MCU_TEMPLATE - MAXNUM_MCU_VCSTEMPLATE)
#define		MAXNUM_DCS_CONF					16
#define		MAXNUM_DCS_CONFMT				16

#define     MAXNUM_MCU_VCCONF              16        //һ��MCU��������ٿ��ĵ��Ȼ����� 

#define		MAXNUM_DRI                      16        //DRI������ 
#define		MAXNUM_DRI_MT                   192       //DRI���ϵ������������ն���

// ��������ҵ��������
#define     MAXLIMIT_MPC_MP                 (u16)32   // MPC ����MPת������, MB
#define     MAXLIMIT_CRI_MP_MDSC            (u16)128  // CRI MP(MDSC) ת������, MB (32*2M*2direction)
#define     MAXLIMIT_CRI_MP_HDSC            (u16)512  // CRI MP(HDSC) ת������, MB (128*2M*2direction)
#define     MAXLIMIT_CRI_MP                 MAXLIMIT_CRI_MP_HDSC

#define     MAXLIMIT_MPC_MP_PMNUM           (u16)16   // MPC����MPת��������PM(�����ʺ��ն����ĳ˻����㣬mcuҵ���MPת�������������������ơ���λPM��һ��1M���ն�Ϊһ��PM)
#define     MAXLIMIT_CRI_MP_MDSC_PMNUM      (u16)64   // CRI MP(MDSC) ת������, PM
#define     MAXLIMIT_CRI_MP_HDSC_PMNUM      (u16)256  // CRI MP(HDSC) ת������, PM
#define     MAXLIMIT_CRI_MP_PMNUM           MAXLIMIT_CRI_MP_HDSC_PMNUM

#define		MAXHDLIMIT_MPC_MTADP			(u8)2	  //MPC ����MtAdp����HD MT����
#define		MAXNUM_HDI_MT					(u16)48  //HDI�ϵ������������ն���

#ifdef WIN32
#define     MAXLIMIT_MPC_MTADP              (u8)112   // MCU ����MtAdp��������
#define     MAXLIMIT_MPC_MTADP_SMCU         (u8)28    // MCU ����MtAdp SMCU��������	
#else
#define     MAXLIMIT_MPC_MTADP              (u8)8     // MCU ����MtAdp��������
#define     MAXLIMIT_MPC_MTADP_SMCU         (u8)4     // MCU ����MtAdp SMCU��������
#endif
#define     MAXLIMIT_CRI_MTADP              (u8)128   // HDSC/MDU         (u8)16    // HDSC/MDSC SMCU��������
#define     MAXLIMIT_CRI_MTADP_SMCU         (u8)16    // HDSC/MDSC SMCU��������
#define     MAXLIMIT_HDI_MTADP              (u8)112     // HDI ��������(û�����壬���������)

#define     MAXLIMIT_MCU_CASCONF            (u8)4     // ��?            (u8)4     // ����������� SMCU
#define     MAXLIMIT_CONF_SMCU              (u8)4     // ����������� SMCU

#elif defined(_8KE_) || defined(_8KH_)	|| defined(_8KI_)// 8000E // [3/10/2010 xliang] FIXME����Щ��ҵ�������ƴ���

// �������ڲ����ݽṹ��������
#define     MAXNUM_MCU_MT                   (u16)64  //MCU֧������ն��� // [3/11/2010 xliang] 
#define		MAXNUM_MCU_CONF					64        //һ��MCU�ڲ����õ�������������Χ
#define     MAXNUM_MCU_TEMPLATE             64        //���ģ�����
#define     MAXNUM_MCU_VCSTEMPLATE          16        //VCS�ɴ�����ģ����
#define     MAXNUM_MCU_MCSTEMPLATE          (MAXNUM_MCU_TEMPLATE - MAXNUM_MCU_VCSTEMPLATE)
#define		MAXNUM_DCS_CONF					32
#define		MAXNUM_DCS_CONFMT				64

#define     MAXNUM_MCU_VCCONF              16        //һ��MCU��������ٿ��ĵ��Ȼ����� 

#define		MAXNUM_DRI                      64        //DRI������ 
#define		MAXNUM_DRI_MT                   192       //DRI���ϵ������������ն���

// ��������ҵ��������
#define     MAXLIMIT_MPC_MP                 (u16)(16*64)   // MPC ����MPת������, MB
#define     MAXLIMIT_CRI_MP                 (u16)120  // CRI MP ת������, MB(�����ʼ��㣬mplib������ͳ��ʹ��)


#define     MAXLIMIT_MPC_MP_PMNUM           (u16)(8*64)   // MPC ����MPת������, PM(�����ʺ��ն����ĳ˻����㣬mcuҵ���MPת�������������������ơ���λPM��һ��1M���ն�Ϊһ��PM)
#define     MAXLIMIT_CRI_MP_PMNUM           (u16)60   // CRI MP ת������


#define		MAXHDLIMIT_MPC_MTADP			MAXNUM_MCU_MT	  //����HD MT���� // [3/11/2010 xliang] 
#define		MAXNUM_HDI_MT					(u16)48  //HDI�ϵ������������ն���

#ifdef WIN32
#define     MAXLIMIT_MPC_MTADP              (u8)112   // MPC ����MtAdp��������
#define     MAXLIMIT_MPC_MTADP_SMCU         (u8)28    // MPC ����MtAdp SMCU��������
#else
#define     MAXLIMIT_MPC_MTADP              (u8)64    // MPC ����MtAdp�������� // ��������������ʱ��24
#define     MAXLIMIT_MPC_MTADP_SMCU         (u8)12    // MPC ����MtAdp SMCU��������
#endif
#define     MAXLIMIT_CRI_MTADP              (u8)112   // CRI ��������
#define     MAXLIMIT_CRI_MTADP_SMCU         (u8)28    // CRI SMCU��������
#define     MAXLIMIT_HDI_MTADP              (u8)112     // HDI ��������

#ifdef WIN32
#define     MAXLIMIT_MCU_CASCONF            (u8)2     // ��༶���������
#define     MAXLIMIT_CONF_SMCU              (u8)4     // ����������� SMCU
#else
#define     MAXLIMIT_MCU_CASCONF            (u8)16    // ��༶���������
#define     MAXLIMIT_CONF_SMCU              (u8)56    // ����������� SMCU
#endif

#else               // 8000 A ����

// �������ڲ����ݽṹ��������
#define     MAXNUM_MCU_MT                   (u16)448  //MCU֧������ն��� 
#define		MAXNUM_MCU_CONF					64        //һ��MCU�ڲ����õ�������������Χ
#define     MAXNUM_MCU_TEMPLATE             64        //���ģ�����
#define     MAXNUM_MCU_VCSTEMPLATE          16        //VCS�ɴ�����ģ����
#define     MAXNUM_MCU_MCSTEMPLATE          (MAXNUM_MCU_TEMPLATE - MAXNUM_MCU_VCSTEMPLATE)
#define		MAXNUM_DCS_CONF					32
#define		MAXNUM_DCS_CONFMT				64

#define     MAXNUM_MCU_VCCONF              16        //һ��MCU��������ٿ��ĵ��Ȼ����� 

#define		MAXNUM_DRI                      64        //DRI������ 
#define		MAXNUM_DRI_MT                   192       //DRI���ϵ������������ն���

// ��������ҵ��������
#define     MAXLIMIT_MPC_MP                 (u16)60   // MPC ����MPת������, MB
#define     MAXLIMIT_CRI_MP                 (u16)120  // CRI MP ת������, MB(�����ʼ��㣬mplib������ͳ��ʹ��)
//zhouyiliang 20101208 mp������
#define     MAXLIMIT_CRI_MP_EX              (u16)400

#define     MAXLIMIT_MPC_MP_PMNUM           (u16)30   // MPC ����MPת������, PM(�����ʺ��ն����ĳ˻����㣬mcuҵ���MPת�������������������ơ���λPM��һ��1M���ն�Ϊһ��PM)
#define     MAXLIMIT_CRI_MP_PMNUM           (u16)60   // CRI MP ת������
//zhouyiliang 20101208 mp������
#define     MAXLIMIT_CRI_MP_PMNUM_EX       (u16)200

#define		MAXHDLIMIT_MPC_MTADP			(u8)2	  //MPC ����MtAdp����HD MT����
#define		MAXNUM_HDI_MT					(u16)MAXNUM_H225H245_MT  //HDI������ն���

#ifdef WIN32
#define     MAXLIMIT_MPC_MTADP              (u8)112   // MPC ����MtAdp��������
#define     MAXLIMIT_MPC_MTADP_SMCU         (u8)28    // MPC ����MtAdp SMCU��������
#else
#define     MAXLIMIT_MPC_MTADP              (u8)24    // MPC ����MtAdp��������
#define     MAXLIMIT_MPC_MTADP_SMCU         (u8)12    // MPC ����MtAdp SMCU��������
#endif
#define     MAXLIMIT_CRI_MTADP              (u8)112   // CRI ��������
#define     MAXLIMIT_CRI_MTADP_SMCU         (u8)28    // CRI SMCU��������
#define     MAXLIMIT_HDI_MTADP              (u8)112     // HDI ��������

#ifdef WIN32
#define     MAXLIMIT_MCU_CASCONF            (u8)2     // ��༶���������
#define     MAXLIMIT_CONF_SMCU              (u8)4     // ����������� SMCU
#else
#define     MAXLIMIT_MCU_CASCONF            (u8)16    // ��༶���������
#define     MAXLIMIT_CONF_SMCU              (u8)56    // ����������� SMCU
#endif
#endif		//8000A

#define     MAXNUM_MCU_MODEM                (u16)16     //���ǻ������modem��

#define		INVALID_RESOUCEUNIT_VALUE		(u16)0XFFFF	//��Ч������Դֵ

//���������������
#define     CHN_ADPMODE_NONE               (u8)0   //��
#define     CHN_ADPMODE_MVBRD              (u8)1   //�����㲥����
#define     CHN_ADPMODE_DSBRD              (u8)2   //˫���㲥����
#define     CHN_ADPMODE_MVSEL              (u8)3   //����ѡ������
#define     CHN_ADPMODE_DSSEL              (u8)4   //˫��ѡ������

//��������ͨ�����Ͷ���
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
#define     TYPE_MAU_NORMAL                 (u8)0   //��ͨMAU
#else
#define     TYPE_MAU_NORMAL                 (u8)0xff   //��ͨMAU
#endif
#define     TYPE_MAU_H263PLUS               (u8)1   //ΪH263plus���Ƶ�MAU
#define     TYPE_MPU                        (u8)2   //MPU
#define     TYPE_8KE_BAS					(u8)3   //[03/01/2010] zjl add
#define     TYPE_MPU_H						(u8)4   //MPU_H
#define		TYPE_MPU2_ENHANCED				(u8)5	//MPU2-Enhanced
#define		TYPE_MPU2_BASIC					(u8)6	//MPU2-Basic
#define		TYPE_8KH_BAS					(u8)7	//[03/31/2011] nzj add
#define		TYPE_APU2_BAS					(u8)8	//APU2 BAS����
#define		TYPE_8KI_VID_BAS				(u8)9   //8KI ��ƵBAS�������Ͷ���
#define		TYPE_8KI_AUD_BAS				(u8)10  //8KI ��ƵBAS�������Ͷ���

//MAU����궨��
#define		MAXNUM_VOUTPUT 					(u8)2   // һ·��������������·��
#define		MAXNUM_MAU_VCHN				    (u8)1   // һ��MAU�����������ͨ����
#define		MAXNUM_MAU_DVCHN				(u8)1   // һ��MAU���˫������ͨ����
#define		MAXNUM_MAU_VDCHN			    (u8)2   // һ��MAU���ͨ����
#define		MAXNUM_VOUTPUT_TMP 				(u8)1   // һ·��������������·��// [12/23/2009 xliang]  modify to 1
#define     MAXNUM_8KEBAS_VOUTPUT			(u8)4	// һ��8kebas������·��  [03/01/2010] zjl add
#define     MAXNUM_8KHBAS_VOUTPUT			(u8)5	// һ��8khbas������·��  [03/31/2011] nzj add
#define     MAXNUM_8KHBAS_DS_VOUTPUT		(u8)2	// һ��8khbas˫��������·��  [03/31/2011] nzj add
#define     MAXNUM_8KHBAS_SEL_VOUTPUT		(u8)1	// һ��8khbasѡ��ͨ��������·��  [03/31/2011] nzj add
#define		MAXNUM_APU2BAS_OUTCHN			(u8)3	 //apu2 ��Ƶbas������ͨ����
#define		MAXNUM_8KIAUDBAS_OUTPUT			(u8)2  //8KI��Ƶͨ���������·��

#define		MAXNUM_8KIVIDBAS_CHN0_OUTPUT	(u8)5
#define		MAXNUM_8KIVIDBAS_CHN1_OUTPUT	(u8)2
#define		MAXNUM_8KIVIDBAS_CHN2_OUTPUT	(u8)1

//MPU����궨��
#define     MAXNUM_MPU_VCHN                 (u8)4   // һ��MPU�������ͨ����, ����������˫��
#define     MAXNUM_8KEBAS_VCHN              (u8)1   // һ��8kebas�������ͨ���� [03/01/2010] zjl add 
#define     MAXNUM_8KHBAS_VCHN              (u8)1   // һ��8khbas�������ͨ���� [03/31/2011] nzj add 

//��������ͨ���궨��
#define     HDBAS_CHNTYPE_MAU_MV            (u8)0
#define     HDBAS_CHNTYPE_MAU_DS            (u8)1
#define     HDBAS_CHNTYPE_MPU               (u8)2

//����궨��
#define     MAXNUM_MPU_VOUTPUT			    (u8)2       //MPU������ͨ����
#define     MAXNUM_MPU_H_VOUTPUT		    (u8)6	    //mpu_hģʽ������ͨ����
#define     MAXNUM_VPU_VAOUTPUT				(u8)1		//vpuһ·������ͨ����
#define     MAXNUM_VPU_OUTPUT				(u8)5		//vpu������ͨ����
#define     MAXNUM_MAU_VOUTPUT				(u8)2		//mau������ͨ����
#define     MAXNUM_MAU_H263P_VOUTPUT		(u8)1	    //mau������ͨ����
#define     MAXNUM_BASOUTCHN				(u8)8	    //bas������ͨ����
#define     MAXNUM_8KEBAS_INPUT			    (u8)1       //8kebas�������ͨ����
#define     MAXNUM_8KHBAS_INPUT			    (u8)1       //8khbas�������ͨ����
#define     MAXNUM_8KIBAS_INPUT			    (u8)1       //8kibas�������ͨ����
#define     MAXNUM_8KIAUDBAS_INPUT			(u8)4       //8kibas�������ͨ����

#define     MAXNUM_MPU2_BASIC_VOUTPUT		(u8)6       //MPU2 basicģʽ������ͨ����
#define     MAXNUM_MPU2_EHANCED_VOUTPUT		(u8)8	    //MPU2 ehancedģʽ������ͨ����

#define		MPU2_BAS_BASIC_MVOUTPUT			(u8)6		
#define		MPU2_BAS_BASIC_DSOUTPUT			(u8)6
#define		MPU2_BAS_BASIC_SELOUTPUT		(u8)5
#define		MPU2_BAS_BASIC_MV_OTHERNUM		(u8)1
#define		MPU2_BAS_BASIC_DS_OTHERNUM		(u8)2
#define		MPU2_BAS_BASIC_SEL_OTHERNUM		(u8)1

#define		MPU2_BAS_ENHACNED_MVOUTPUT		(u8)7
#define		MPU2_BAS_ENHACNED_DSOUTPUT		(u8)8
#define		MPU2_BAS_ENHACNED_SELOUTPUT		(u8)5
#define		MPU2_BAS_ENHACNED_MV_OTHERNUM	(u8)1
#define		MPU2_BAS_ENHACNED_DS_OTHERNUM	(u8)2
#define		MPU2_BAS_ENHACNED_SEL_OTHERNUM	(u8)1

#define		MAX_OTHER_ENC_NUM				(u8)2	//���other����·�����ֱ��Ӧ������˫������Ƶ

#define     CHN_ADPMODE_BRD				    (u8)1  //�㲥����
#define		CHN_ADPMODE_SEL					(u8)2  //ѡ������

//HDU����ģʽ

#define STARTMODE_HDU_M						(u8)0      //hduԭ��ģʽ
#define STARTMODE_HDU_H						(u8)1      //hdu��ͨ��ģʽ
#define STARTMODE_HDU_L						(u8)2	   //hdu������ģʽ
#define STARTMODE_HDU2						(u8)3	   //hdu2����ģʽ
#define STARTMODE_HDU2_L					(u8)4	   //hdu2-L����ģʽ
#define STARTMODE_HDU2_S					(u8)5	   //hdu2-S����ģʽ

//HDU����������(�����������ģʽһһ��Ӧ)
#define HDU_SUBTYPE_HDU_M						(u8)0      //hdu��˫ͨ��ģʽ����
#define HDU_SUBTYPE_HDU_H						(u8)1      //hdu�嵥ͨ��ģʽ����
#define HDU_SUBTYPE_HDU_L						(u8)2	   //hdu_L������
#define HDU_SUBTYPE_HDU2						(u8)3	   //hdu2������
#define HDU_SUBTYPE_HDU2_L						(u8)4	   //hdu2-L������
#define HDU_SUBTYPE_HDU2_S						(u8)5	   //hdu2-S������

//HDU2��ʾ������������
#define HDU2_SHOWMODE_NONGEOMETRIC          (u8)1      //���ȱ�����
#define HDU2_SHOWMODE_CUTEDGEGEOME          (u8)2      //�ñߵȱ�����
#define HDU2_SHOWMODE_BLACKEDGEGEOME        (u8)3      //�Ӻڱߵȱ�����

//HDU֧�ֻ���ϳɷ����غ궨��
#define HDUCHN_MODE_MAXCHNNUM				(u8)20  //ÿ���������ǽͨ��֧�ֵ������ͨ�����������Ժ���չ
#define HDUCHN_MODE_ONE						(u8)0   //һ������ģʽ
#define HDUCHN_MODE_FOUR					(u8)1   //�Ļ�����ģʽ
#define HDU_MODEFOUR_MAX_SUBCHNNUM			(u8)4   //�ķ��֧�ֵ������ͨ����

#define MAXNUM_CONF_MVCHN                   (u8)3       //�������������ͨ����
#define MAXNUM_CONF_DSCHN                   (u8)3       //�������˫������ͨ����
    
#define MAXNUM_CONF_DUALMVCHN               (u8)1
#define MAXNUM_CONF_DUALDSCHN               (u8)2

#define MAXNUM_CONF_MAU                     (u8)4       //�������MAU��
#define MAXNUM_CONF_MPU                     (u8)MAXNUM_MPU_CHN       //�������MPUͨ����

#define MAU_CHN_NONE                        (u8)0
#define MAU_CHN_NORMAL                      (u8)1       //HDBAS/MAU ͨ������ͨ��
#define MAU_CHN_VGA                         (u8)2       //HDBAS/MAU ��VGA�����ͨ��
#define MAU_CHN_263TO264                    (u8)3       //HDBAS/MAU ��263��264�����ͨ��
#define MAU_CHN_264TO263                    (u8)4       //HDBAS/MAU ��264��263�����ͨ��
#define MPU_CHN_NORMAL                      (u8)5       //HDBAS/MPU ͨ������ͨ����ͨ����ǿ��MAU_CHN_NORMAL
#define BAS_8KECHN_MV						(u8)6       //HDBAS/8KEBAS [03/01/2010] zjl add
#define BAS_8KECHN_DS						(u8)7       //HDBAS/8KEBAS [03/01/2010] zjl add
#define BAS_8KECHN_SEL						(u8)8       //HDBAS/8KEBAS [03/01/2010] zjl add
#define MPU2_BAS_CHN_MV						(u8)9       //HDBAS MPU2 ����ͨ��
#define MPU2_BAS_CHN_DS						(u8)10      //HDBAS MPU2 ˫��ͨ��
#define MPU2_BAS_CHN_SEL					(u8)11      //HDBAS MPU2 ѡ��ͨ��
#define BAS_8KHCHN_MV						(u8)12      //HDBAS/8KHBAS [03/31/2011] nzj add
#define BAS_8KHCHN_DS						(u8)13      //HDBAS/8KHBAS [03/31/2011] nzj add
#define BAS_8KHCHN_SEL						(u8)14      //HDBAS/8KHBAS [03/31/2011] nzj add

#define BAS_8KICHN0							(u8)15      //8KI chnnl0
#define BAS_8KICHN1							(u8)16      //8KI chnnl1
#define BAS_8KICHN2							(u8)17      //8KI chnnl2

#define BAS_8KIAUDCHN						(u8)18		//8KI audbas

#define MAXNUM_MPU_CHN                      (u8)4
#define MAXNUM_MPU_H_CHN					(u8)2       //mpu_h�������ͨ����
#define MAXNUM_MPU2_BASIC_CHN               (u8)4		//mpu2 basic�������ͨ����
#define MAXNUM_MPU2_EHANCED_CHN				(u8)7       //mpu2 enhanced�������ͨ����
#define MAXNUM_APU2_BASCHN					(u8)7		//apu2����֧����������ͨ����
#define MAXNUM_8KI_AUD_BASCHN				(u8)4		//8kI aud bas����֧����������ͨ����

// ���ʱ������(ʵ������)
#ifdef _VXWORKS_
    #ifdef _MINIMCU_
    #define     MAXNUM_ONGO_CONF            4   //20100927zhouyiliang �޸����ʱ��������mcs��vcs����
    #define     MAXNUM_ONGO_VCSCONF         4 // ����ͬʱ�ٿ���VCS������   
    #else
    #define     MAXNUM_ONGO_CONF            4  //mtadp �� mp ��������ʱ//20100927zhouyiliang �޸����ʱ��������mcs��vcs����
    #define     MAXNUM_ONGO_VCSCONF         4 // ����ͬʱ�ٿ���VCS������ 
    #endif 
#elif defined(_LINUX_)
    #ifdef _MINIMCU_
        #define     MAXNUM_ONGO_CONF            4 // 16������û���⣬�����Ƿ���������ҵ��//20100927zhouyiliang �޸����ʱ��������mcs��vcs����
        #define     MAXNUM_ONGO_VCSCONF         4 // ����ͬʱ�ٿ���VCS������ 
    #else
        #if defined(_LINUX12_) || defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
            #define     MAXNUM_ONGO_CONF        16
            #define     MAXNUM_ONGO_VCSCONF     16
        #else
            #define     MAXNUM_ONGO_CONF            4  //����ֵ36(��128M,MCU����ռ18M,Ԥ��20M)//20100927zhouyiliang �޸����ʱ��������mcs��vcs����
            #define     MAXNUM_ONGO_VCSCONF         4 // ����ͬʱ�ٿ���VCS������ 
        #endif 
    #endif
#else   // WIN32
#define     MAXNUM_ONGO_CONF                4 //20100927zhouyiliang �޸����ʱ��������mcs��vcs����
#define     MAXNUM_ONGO_VCSCONF             4 // ����ͬʱ�ٿ���VCS������ 
#endif

// �����������, 8000/8000B: 8M, 8000C: 4M
#ifdef _MINIMCU_
    #ifndef _MCU8000C_
        #define MAX_CONFBITRATE             8128
    #else
        #define MAX_CONFBITRATE             4096
    #endif
#elif defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	#define MAX_CONFBITRATE                 8192
#else
    #define MAX_CONFBITRATE                 8128
#endif

#define     MAXNUM_VIEWINCONF				5    //ÿ������������ͼ��
#define     MAXNUM_GEOINVIEW				5    //ÿ����ͼ����󼸺β�����
#define		MAXNUM_SUBFRAMEINGEO            16   //ÿ�����β��ֵ�����Ա��
#define     MAXNUM_VSCHEMEINVIEW			4    //ÿ����ͼ�����Ƶ������
#define     MAXNUM_MIXERINCONF              16   //ÿ�������е���������  
#define     MAXNUM_ADDMT                    10   //һ�����ӵ����MT��   

#define     MAXNUM_NPLUS_MCU                16
 
//�������(ģ��)��confidxΪ����map��������+1
#define MIN_CONFIDX                         1
#define MAX_CONFIDX                         (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE)

//���֧�ֵ������  
#define     MAXNUM_HDUBRD                   70   // ���Hdu������Ŀ   4.6   jlb
#define     MAXNUM_CONF_HDUBRD				28	 //Ϊ����ģ��ʹ�ã�һ�����������28��HDU����56��HDUͨ��
#define     MAXNUM_OTHERBRD                 64   // ��󵥰��ϵ���Ŀ
#define     MAXNUM_BOARD       (MAXNUM_HDUBRD + MAXNUM_OTHERBRD)   // 64 + 28(hdu)

//ͬһ������������
#define     MAXNUM_PERIEQP                  16   //����HDU
#define     MAXNUM_PERIHDU                  70   //10��tvs,ÿ��7��HDU 
#define		MAXNUM_PERIDVMP					8	 //8��mpu-dvmp // xliang [12/10/2008] 	
#define		MAXLEN_EQP_ALIAS                16   //���������󳤶�

//��󵥰��š��ۺ�
#define     MAXNUM_LAYER                    4   //���
#define     MAXNUM_HDU_LAYER                10   //���
#define     MAXNUM_SLOT                     14  //�ۺ�  
#define     MAXNUM_TVSSLOT                  7   // TVS�Ĳ�λ  jlb 4.6 
#define     MAXLEN_SLOT_ALIAS               16  //�ۺ���󳤶�

 // �Ƿ���MapId
#define INVALID_MAP_ID	        		    (u8)255        

//������
#define     SLOTTYPE_MC                     1   
#define     SLOTTYPE_EX                     2
#define     SLOTTYPE_TVSEX                  3 //TVS�Ĳ����� 4.6 jlb 
#define     SLOTTYPE_IS                     4

#define     LEN_DES                         8	 //byte
#define     LEN_AES                         16	 //byte

//��ַ����������(�ο���ַ��ͷ�ļ�)
#define     MAXLEN_ADDR_ENTRYNAME           32
#define     MAXLEN_ADDR_323ALIAS            32
#define     MAXLEN_ADDR_E164NUM             16
#define     MAXLEN_ADDR_320ID               48
#define		MAXLEN_ADDR_ENTRYNAME_V1		48	//  [6/17/2013 guodawei]
#define		MAXLEN_ADDR_323ALIAS_V1			48	//  [6/17/2013 guodawei]

#define     MAXNUM_ADDRGROUP                64
#define     MAXNUM_ADDRENTRY                1024

//������س�������
#define     MAXNUM_SUB_MCU                  24   //����¼�MCU����
#define		MAXNUM_ACCOMMODATE_SPY			(u16)512	 //MMCU�����������SMCU��spy��
#define		MAXNUM_CONFSUB_MCU				( MAXNUM_SUB_MCU * MAXNUM_SUB_MCU + MAXNUM_SUB_MCU )	//����һ������ӵ�е��¼�MCU����
#define		MAX_CASCADEDEPTH				3 //mcu�������
#define		MAX_CASCADELEVEL				(MAX_CASCADEDEPTH - 2)


// ���mcu�ĸ���, �¼�mcu�������ϱ���mcu
// #define			MAXNUM_CONF_MCU				(u16)(MAXNUM_CONFSUB_MCU + 1) 
// mcu������������
#define			MAX_SUBMCU_DEPTH			(u8)(MAX_CASCADEDEPTH-1)

#define		MCU_Q931_PORT					1720	//ȱʡQ931�˿ں�
#define		MCU_RAS_PORT					1719	//ȱʡRAS�˿ں�
#define		MCU_LISTEN_PORT					60000	//ȱʡ����/���TCP�����˿ں�(TCP,OSP)
#define		MCU_H225245_STARTPORT	        60002	//ȱʡMCU��MT��������ʼ�˿ں�(TCP) -- 60386(2*192)
//#define		MCU_RCVH224_PORT				2330	//�����ն�H224���ݶ˿ں�

//�����б��͸���ϯ�ն˵�����ն���
#define		MAXNUM_ANSWERINSTANTLY_MT		5
//��־������
#define     LOGLVL_EXCEPTION				0	//�쳣
#define     LOGLVL_IMPORTANT				1	//��Ҫ��־
#define     LOGLVL_DEBUG1					254 //һ��������Ϣ
#define     LOGLVL_DEBUG2					255 //����������Ϣ

//����ҵ��APP�Ŷ��壨����kdvdef.h�з�Χ����ͳһ���壩
#define     AID_MCONSOLE					AID_MCU_BGN	        //�������̨
#define     AID_MCU_AGENT					AID_MCU_BGN + 5 	//MCU����
#define     AID_MCU_BRDMGR					AID_MCU_BGN + 6 	//�������
#define		AID_MCU_BRDAGENT				AID_MCU_BGN + 7 	//�������
#define		AID_MCU_BRDGUARD				AID_MCU_BGN + 8	    //��������
#define     AID_MCU_MTADP					AID_MCU_BGN + 9	    //MTADP
#define     AID_VMPTW                       AID_MCU_BGN + 10    //�໭�渴�ϵ���ǽ
#define     AID_MIXER						AID_MCU_BGN + 11	//������
#define     AID_TVWALL						AID_MCU_BGN + 12	//����ǽ
#define     AID_RECORDER					AID_MCU_BGN + 13	//¼���
#define     AID_DCS							AID_MCU_BGN + 14	//���ݻ��������
#define     AID_BAS							AID_MCU_BGN + 15	//�������������
#define     AID_MP							AID_MCU_BGN + 16	//MP
#define	    AID_VMP							AID_MCU_BGN + 17	//����ϳ���
#define     AID_PRS							AID_MCU_BGN + 18    //���ط�
#define		AID_PRS_GUARD					AID_MCU_BGN + 19	//prs����
#define     AID_HDU                         AID_MCU_BGN + 20    //HDU
#define		AID_MPU							AID_MCU_BGN + 21	//MPU

#define     AID_MCU_VC						AID_MCU_BGN + 22	//MCUҵ��
#define     AID_MCU_GUARD					AID_MCU_BGN + 23	//MCU����
#define     AID_MCU_MCSSN					AID_MCU_BGN + 24	//MCU�������̨�Ự
#define     AID_MCU_CONFIG					AID_MCU_BGN + 25	//MCU���ûỰ
#define     AID_MCU_MTADPSSN				AID_MCU_BGN + 26	//MCU��MTADP�Ự
#define     AID_MCU_PERIEQPSSN				AID_MCU_BGN + 27	//MCU����Ự
#define     AID_MCU_MPSSN					AID_MCU_BGN + 28	//MCU��MP�Ự
#define		AID_MCU_DCSSSN					AID_MCU_BGN + 29	//MCU��DCS�Ự
#define     AID_MCU_MSMANAGERSSN			AID_MCU_BGN + 30	//�ȱ��ݻỰģ��
#define     AID_MCU_NPLUSMANAGER            AID_MCU_BGN + 31    //N+1���ݹ���ģ��
#define     AID_MCU_VCSSN					AID_MCU_BGN + 32	//VCS�������̨�Ự

#define     AID_MCU_MTSSN                   AID_MCU_BGN + 33    //����ƽ̨Э������Ự
#define     AID_MCU_MODEMSSN                AID_MCU_BGN + 34    //����ƽ̨Modem�Ự
#define     AID_UDPDISPATCH                 AID_MCU_BGN + 35    //����ƽ̨�ն���Ϣ��udp�Ự
#define     AID_UDPNMS                      22//AID_MCU_BGN + 36    //����ƽ̨NMS������udp�Ự

#define		AID_MPU2							AID_MCU_BGN + 37	//MPU2

//CPPUNIT��������
#define     AID_TEST_MCAGT					AID_MCU_BGN + 40	//MC���ɲ���
#define     AID_UNIT_TEST_MCAGT				AID_MCU_BGN + 41	//MC��Ԫ����
#define     AID_TEST_MTCAGT					AID_MCU_BGN + 42	//MTC���ɲ���
#define     AID_UNIT_TEST_MTCAGT			AID_MCU_BGN + 43	//MTC��Ԫ����
#define     AID_TEST_NMCAGT					AID_MCU_BGN + 44	//NMC
#define     AID_UNIT_TEST_MCUAGT			AID_MCU_BGN + 45	//MCU��Ԫ����
#define     AID_UNIT_TEST_MTAGT				AID_MCU_BGN + 46	//MT��Ԫ����
#define     AID_UNIT_TEST_SIMUMT            AID_MCU_BGN + 47
#define     AID_HDUAUTO_TEST                AID_MCU_BGN + 48    //hdu�Զ����

// ������ҪӦ�õ����ȼ�����(�ر���ͬһ�������ж��)
#define     APPPRI_MIXER                    80
#define     APPPRI_BAS                      80
#define     APPPRI_PRS                      80
#define     APPPRI_TVWALL                   80
#define     APPPRI_VMP                      80
#define     APPPRI_MPW                      80
#define     APPPRI_REC                      80
#define     APPPRI_HDU                      80
#define		APPPRI_MPU						80	
 
#define     APPPRI_HDUAUTO                  80   // HDU�Զ�������ȼ�

#define     APPPRI_PRS_GUARD	            75

#define     APPPRI_MTADP                    80
#define     APPPRI_MP                       90

#define     APPPRI_BRDGUARD                 90
#define     APPPRI_BRDAGENT                 100

#define     APPPRI_MCUGUARD                 75
#define     APPPRI_MCUVC                    80
#define     APPPRI_MCUMSMGR                 80
#define     APPPRI_MCUAGT                   80
#define     APPPRI_MSCONFIG                 80
#define     APPPRI_MCUSSN                   90
#define     APPPRI_MCUCONFIG                90
#define     APPPRI_MCUBRDGUARD              90
#define     APPPRI_MCUBRDMANAGER            100
#define     APPPRI_MCUNPLUS                 80

//MCU E1Ӳ������
#define      MAXNUM_MCU_DTI					32  //MCU���DTI��Ŀ
#define      MAXNUM_MCU_E1_PER_DTI			8	//ÿ��DTI�����E1��
#define      MAXNUM_MCU_MT64K				128 //MCU����MT���ʱ϶��:8M
#define      MAXNUM_MCU_SMCU64K				512 //�����¼�MCU���ʱ϶��:4*8M
 
//���賣������
#define		MAXNUM_BASGRP					5   //���������������������RTPͷ��
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
    #define     MAXNUM_PRS_CHNNL				30	//���ط���������
#else
	#define     MAXNUM_PRS_CHNNL				16	//���ط���������
#endif

#define     MAXNUM_BAS_CHNNL				32  //�������������ͨ���� 
#define     MAXNUM_MIXER_GROUP				5	//�������������
#define		MAXNUM_MIXER_GROUP_8KE			1	// ������������� [1/5/2010 xliang] modify from 5 to 1			
#define     MAXNUM_MIXER_GRPCHNNL			192 //���������Ա�����

//tianzhiyong  2010/03/21  APU��EAPU�˿ڷ�ΧͳһΪ940
// APU2��APU,EAPU2�л�����ռ�ö˿������ֵΪ790��APU2��[1/31/2012 chendaiwei]
#define     MAXPORTSPAN_MIXER_GROUP			790 //������˿ڷ�Χ(Ӧ����>=���ͨ����*PORTSPAN)
#define     MIXER_INVALID_GRPID				255 //��Ч�������
#define     MIXER_INVALID_CHNNLID			255 //��Ч�����ŵ���
#define     MAXNUM_RECORDER_CHNNL			32	//¼������ͨ������(��¼��ͷ���ͨ��)
#define     MAX_VOD_FILES_LEN				1024//VOD�ļ����б���󳤶�
#define     MAX_VOD_FNAME_LEN				155	//VOD�ļ�����󳤶ȣ������ݿ�һ��
#define     MAX_VODFILEPATH_LEN				100	//VOD�ļ�Ŀ¼��󳤶ȣ������ݿ�һ��
#define     MAXLEN_RECORD_NAME              64  //�ϱ�����״̬ʱ¼���ļ����ĳ���
#define     DEFAULT_MIXER_VOLUME			250	//ȱʡ�Ļ�������
#define		MAXLEN_TRAP_COMMUNITY			33	//trap community ��󳤶�(��Чλ32)
#define		MAXNUM_TRAP_LIST				16	//trap����󳤶�
#define		MAXNUM_HDU_CHANNEL              2   //һ��HDU�����ͨ����
#define		MAXNUM_HDU_H_CHANNEL            1   //һ��HDU_H�����ͨ����
//���HDU������ͨ����ͨ����   jlb
#define     MAXNUM_HDUCFG_CHNLNUM           56  //��������HDU������ͨ����

#define     MAXNUM_EAPU_MIXER               4   //һ��EAPU��������õĻ���������   //tianzhiyong  20100309
#define     MAXNUM_APU2_MIXER               2   //һ��APU2��������õĻ���������   
#define     MAXNUM_MIXER_AUDTYPE            15  //���ʽ������֧�ֵ������Ƶ������
#define     MAXNUM_EMIXER_DEPTH             10  //EAPU�������֧�ֵ����������   //tianzhiyong  20100309

// HDUԤ����������󳤶�    jlb
#define		MAX_HDUSTYLE_ALIASLEN           32

//Mcu��DCS��������
#define		MAXNUM_MCU_DCS					1	//MCU����DCS������, Ŀǰֻ֧��һ��

//������������������
#define		MAXNUM_BRD_EQP					5
#define		MAXNUM_HDUBRD_EQP					1

//����Ӫ�̱�����󳤶�
#define		MAXLEN_MULTINETALIAS			32

//ÿ��map������ͨ����
#ifdef _MINIMCU_
#define     MAXNUM_MIXER_CHNNL              32
#else
#define     MAXNUM_MIXER_CHNNL              MAXNUM_MIXING_MEMBER
#endif
//���Ƿ�ɢ����������ƻ�������
#define     MAXNUM_MIXERCHNNL_SATDCONF      4
//Ĭ���ش�����ʱ��
#define     DEF_FIRSTTIMESPAN_PRS           40
#define     DEF_SECONDTIMESPAN_PRS          120
#define     DEF_THIRDTIMESPAN_PRS           240
#define     DEF_LOSETIMESPAN_PRS            480
//Ĭ��trap�˿�
#define     DEF_READWRITE_TRAPPORT          161
#define     DEF_SEND_TRAPPORT               162
//Ĭ���ն˶������ʱ��ʹ���
#define     DEF_DISCCHECKTIME               10
#define     DEF_DISCCHECKTIMES              3
//�ն���С�������ʱ��ʹ���
#define     MIN_DISCCHECKTIME               5   //5s 
#define     MIN_DISCCHECKTIMES              1   //1��
//����ʱ�ն��б�ˢ��ʱ����
#define     PARTLIST_REFRESHTIME            (u16)600 //(��λ����)
//����ʱ��Ƶ��Ϣ����ʱ����
#define     AUDINFO_REFRESHTIME             (u16)600
//����ʱ��Ƶ��Ϣ����ʱ����
#define     VIDINFO_REFRESHTIME             (u16)600
//������Ϣ��������ʱ����(12hour)
#define     CASCADEINFO_MAX_REFRESHTIME     (u16)43200
//������Ϣ�������Сʱ����(30s)
#define     CASCADEINFO_MIN_REFRESHTIME     (u16)30

// �������������Զ�ʱͬ��ʱ���� 300��
#define MS_SYN_MAX_SPAN_TIMEOUT             (u32)300

// �������osp�������Ĭ����������
#define     DEF_OSPDISC_HBTIME              (u16)10
// �������osp�������Ĭ����������
#define     DEF_OSPDISC_HBNUM               (u8)3

//N+1����RTDĬ����������
#define     DEF_NPLUSRTD_TIME               5

//N+1����RTDĬ����������
#define     DEF_NPLUSRTD_NUM                3

//mcuĬ����������
#define		DEF_ADMINLVEL					4

//�����������
#define		MAX_ADMINLEVEL					1

//mcs�������ٿ��ļ�ʱ������
#define		DEF_MCSONGOINGCONFNUM_MAX		16

//Qos type
#define     QOSTYPE_DIFFERENCE_SERVICE      1   //���ַ���
#define     QOSTYPE_IP_PRIORITY             2   //ip����
//qos �ȼ�
#define     QOS_IP_MAXLEVEL                 7   //ip ���ȷ���ȼ���0��7��
#define     QOS_DIFF_MAXLEVEL               63  //���ַ���ȼ���0��63��
//ip ��������
#define     IPSERVICETYPE_NONE              0   //ȫ����ѡ
#define     IPSERVICETYPE_LOW_DELAY         1   //��С�ӳ�
#define     IPSERVICETYPE_HIGH_THROUGHPUT   2   //���������
#define     IPSERVICETYPE_HIGH_RELIABILITY  4   //��߿ɿ���
#define     IPSERVICETYPE_LOW_EXPENSE       8   //��ͷ���

//�����ѯ����״̬��ʱʱ��
#define     WAITING_MSSTATUS_TIMEOUT        (8*1000) // guzh [9/12/2006] �޸�Ϊ8s

//����ϳɷ��Ĭ��RGBֵ
#define     VMPSTYLE_DEFCOLOR               (u32)(246|(246<<8)|(246<<16))// r|(g<<8)|(b<<16)
//����ϳ���󷽰�����
#define     MAX_VMPSTYLE_NUM                5
//HDUԤ����󷽰�����
#define   MAX_HDUSTYLE_NUM                  16
// ����ϳɷ���������󳤶�
#define  MAX_VMPSTYLE_ALIASLEN              32   
// ����ϳ��������
// ����
#define FONT_HEI    (u8)01 // ����
#define FONT_SONG   (u8)02 // ���� (Ĭ��)
#define FONT_KAI    (u8)03 // ����
// �ֺ�
#define FONT_SIZE_24  24	// 24*24 (Ĭ��)
#define FONT_SIZE_32  32	// 32*32
// ���뷽ʽ
#define LEFT_ALIGN    (u8)1  // ��
#define MIDDLE_ALIGN  (u8)2  // ��
#define RIGHT_ALIGN   (u8)3  // ��

//  [1/8/2010 pengjie] Modify
// HDU �ӻ������ʱ��ʾģʽ
// HDU����ͨ����ʾΪ��ɫ
#define HDU_SHOW_BLACK_MODE           0
// HDU����ͨ����ʾ���һ֡
#define HDU_SHOW_LASTFRAME_MODE       1 
// HDU����ͨ����ʾĬ��ͼƬ��1280*720
#define HDU_SHOW_DEFPIC_MODE          2
// HDU����ͨ����ʾ�û��Զ���ͼƬ
#define HDU_SHOW_USERDEFPIC_MODE      3

// VMP����ͨ����ʾģʽ
// VMP����ͨ����ʾΪ��ɫ
#define VMP_SHOW_BLACK_MODE           0
// VMP����ͨ����ʾ���һ֡
#define VMP_SHOW_LASTFRAME_MODE       1
// VMP����ͨ����ʾĬ��ͼƬ��480*272
#define VMP_SHOW_DEFPIC_MODE          2 
// VMP����ͨ����ʾ�û��Զ���ͼƬ
#define VMP_SHOW_USERDEFPIC_MODE      3
// End

// ����ϳ�����Ĭ��������ɫ
#define DEFAULT_TEXT_COLOR  (u32)(44|(94<<8)|(180<<16))  // blue
// ����ϳ�����Ĭ�ϱ���ɫ
#define DEFAULT_BACKGROUND_COLOR (u32)(255|(255<<8)|(255<<16)) // white
// ����ϳ�����Ĭ��͸����
#define DEFAULT_DIAPHANEITY   0 // 
// ����ϳ�����Ĭ���ı�����
#define DEFAULT_TEXT_LEN     (u8)32 
 // ����ϳ�����Ĭ�ϸ߶�
#define wDEFAULT_BMP_HEIGHT  (u16)50 
// BMP�ļ������С
#define  MAX_BMPFILE_SIZE   (u32)800*600  
// ���BMP�ļ����ݴ�С
#define  MAX_BMPDATA_BUFFER (u32)720*576  

// mcucfg.ini �����ļ�����󳤶�
#define    MAXLEN_MCUCFG_INI                12800
// mcuunproccfg.ini�ļ�����󳤶�
#define    MAXLEN_MCUUNPROCCFG_INI          8192

// �����ļ�ȫ·������󳤶�
#define	   MAXLEN_MCU_FILEPATH			128

//Read and save /conf file
#ifdef _VXWORKS_
#define DIR_CONFIG               ( LPCSTR )"/conf"
#define DIR_DATA                 ( LPCSTR )"/data"
#define DIR_FTP                  ( LPCSTR )"/ftp"
#define DIR_WEB                  ( LPCSTR )"/webfiles/doc"
#define DIR_LOG					 ( LPCSTR )"/log"
#endif

#ifdef _LINUX_
#ifdef _X86_  // Redhat
//#define DIR_CONFIG               ( LPCSTR )"/usr/etc/conf" // [3/6/2010 xliang] modify for 8000E 
#define DIR_CONFIG               ( LPCSTR )"/opt/mcu/conf" 
#define DIR_DATA                 ( LPCSTR )"/opt/mcu/data"	
#define DIR_FTP                  ( LPCSTR )"/opt/mcu/ftp"	
#define DIR_WEB                  ( LPCSTR )"/opt/mcu/webroot"
#define DIR_LOG					 ( LPCSTR )"/opt/mcu/log"
#define MMPLOGFILENAME           ( LPCSTR )"/opt/mcu/log/mmplog.log" 
#define DIR_EXCLOG               ( LPCSTR )"/opt/mcu/log/exc.log"
#define APACHE_CONFIG_SCRIPT	 ( LPCSTR )"/opt/mcu/apachesvrcfg"
#endif
#if defined(_PPC_) || defined(_ARM_)
#define DIR_CONFIG               ( LPCSTR )"/usr/etc/config/conf"	
#define DIR_DATA                 ( LPCSTR )"/usr/etc/data"	
#define DIR_FTP                  ( LPCSTR )"/usr/ftp"	
#define DIR_WEB                  ( LPCSTR )"/usr/webroot"
#define DIR_LOG					 ( LPCSTR )"/usr/log"
#define DIR_BIN                  ( LPCSTR )"/usr/bin"
#endif
#endif

#ifdef WIN32
#define DIR_CONFIG               ( LPCSTR )"./conf"	
#define DIR_DATA                 ( LPCSTR )"./data"	
#define DIR_FTP                  ( LPCSTR )"./ftp"	
#define DIR_WEB                  ( LPCSTR )"./webfiles/doc"	
#define DIR_LOG					 ( LPCSTR )"./log"
#define MMPLOGFILENAME           ( LPCSTR )"./log/mmplog.log"
#endif

#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
#define DIR_EXCLOG               ( LPCSTR )"/usr/log/exc.log"
#endif

#define FILE_BRDCFG_INI          ( LPCSTR )"brdcfg.ini"
#define FILE_BRDCFGDEBUG_INI     ( LPCSTR )"brdcfgdebug.ini"    //4.6  jlb
#define MCUCFGFILENAME           ( LPCSTR )"mcucfg.ini"
#define MCUDEBUGFILENAME         ( LPCSTR )"mcudebug.ini"
#define MCUCFGBAKFILENAME		 ( LPCSTR )"mcucfg.bak"
#define MCU_VCSUSER_FILENAME     ( LPCSTR )"vcslogin.usr"
#define MCUUNPROCCFGFILENAME     ( LPCSTR )"mcuunproccfg.ini"         
#define MCU_SAT_CFGFILENAME      ( LPCSTR )"mcusatcfg.ini"
#define GKCFG_INI_FILENAME		 ( LPCSTR )"gkconfig.ini"
#define PXYCFG_INI_FILENAME		 ( LPCSTR )"pxysrvCfg.ini"
#define MULTINETCFG_INI_FILENAME ( LPCSTR )"multinetcfg.ini"
#define MULTIMANUNETCFG_FILENAME ( LPCSTR )"multimanucfg.ini"
#define FILE_MODCFG_INI          ( LPCSTR )"modcfg.ini"
#define FILE_MTADPDEBUG_INI      ( LPCSTR )"mtadpdebug.ini"	        


// [3/6/2010 xliang] 8000E ����״̬����ļ�
#define SECTION_RUNSTATUS			(LPCSTR)"RunStatus"
#if defined(_8KE_)
	#define RUNSTATUS_8KE_CHKFILENAME	(LPCSTR)"runstatus_8ke.stchk"	
	#define KEY_MCU8KE					(LPCSTR)"mcu8ke"
#endif	
#if defined(_8KH_)
	#define RUNSTATUS_8KE_CHKFILENAME	(LPCSTR)"runstatus_8kh.stchk"
	#define KEY_MCU8KE					(LPCSTR)"mcu8kh"
#endif	
#if defined(_8KI_)
	#define RUNSTATUS_8KE_CHKFILENAME	(LPCSTR)"runstatus_8ki.stchk"
	#define KEY_MCU8KE					(LPCSTR)"mcu8ki"
#endif

#define KDV8KE_UPDATE_FILENAME		(LPCSTR)"kdv8000g.bin"
#define KDV8KE_FTPSETUP_SCRIPT      (LPCSTR)"ftpsetup.sh"

#define KDV8KI_UPDATE_FILENAME		(LPCSTR)"kdv8000i.bin"
#define	KDV8KI_UPDATE_MAGICNUMBER	(LPCSTR)"#8kiupdatefile"

//linux�°汾���´���ļ���׺
#define LINUXAPP_POSTFIX         ( LPCSTR )"image"
#define LINUXAPP_POSTFIX_BIN     ( LPCSTR )"bin"
//linux�°汾OS�ļ���׺
#define LINUXOS_POSTFIX          ( LPCSTR )"linux"

// License����
#define KEY_FILENAME                  "kedalicense.key"

//#define KEDA_AES_KEY				  "kedacom200606231"
//#define KEDA_COPYRIGHT			      "Suzhou Keda Technology Co.,ltd. Tel:86-512-68418188, Fax: 86-512-68412699, http://www.kedacom.com"

// �������ݵĳ���
//#define LEN_KEYFILE_CONT              (u16)512

//MCU������Ϣ����, zgc, 2007-03-16
#define		MCUCFGINFO_LEVEL_NEWEST			0	//��ǰ���µ������ļ���Ϣ
#define		MCUCFGINFO_LEVEL_LAST			1	//���һ�γɹ���ȡ�������ļ���Ϣ
#define		MCUCFGINFO_LEVEL_DEFAULT		2	//Ĭ��������Ϣ
#define		MCUCFGINFO_LEVEL_PARTNEWEST		3	//ֻ�в��������µ������ļ���Ϣ��
												//���в��ֱ��޸�ΪĬ�ϻ����һ�γɹ���ȡ��������Ϣ

#define     MAX_FILESTREAM_NUM                  3   //ý���ļ����ý������, zgc, 2008-08-02

//��ͬ������
#define     NETSYNTYPE_VIBRATION            1   //������
#define     NETSYNTYPE_TRACE                2   //����
#define     NETSYN_MAX_DTNUM                14  //���DT���
#define     NETSYN_MAX_E1NUM                7   //���E1��

//���鳣������ 
#define     MAXNUM_MC_CHANNL   				16	//�������̨���ͨ����
#define     MAXNUM_MT_CHANNL   				16	//�ն˼�����ͨ����

#define     EQP_CHANNO_INVALID              0xFF //��Ч����ͨ����

//�����������Ͷ���
#define     ADAPT_TYPE_NONE                 0   //������������
#define     ADAPT_TYPE_AUD                  1   //��Ƶ��������
#define     ADAPT_TYPE_VID                  2   //��Ƶ��������
#define     ADAPT_TYPE_BR                   3   //��������
#define     ADAPT_TYPE_CASDAUD              4   //��Ƶ��������
#define     ADAPT_TYPE_CASDVID              5   //��Ƶ��������
#define     ADAPT_TYPE_MULTISPYCASDVID      6   //��ش���Ƶ��������
#define     ADAPT_TYPE_MULTISPYCASDAUD      7   //��ش���Ƶ��������

//��������ͨ�����Ͷ���
#define     BAS_CHAN_AUDIO                  1   //��Ƶ����ͨ��
#define     BAS_CHAN_VIDEO                  2   //��Ƶ����ͨ��

//cppunit���Խ��
#define		CPPTEST_OK						0	//�ɹ�

//����������Դ��������, zgc, 2008-04-14
#define		SPEAKER_SRC_NOTSEL				(u8)0		// �����˷�ѡ��
#define		SPEAKER_SRC_MCSDRAGSEL			(u8)1		// �������ѡ��
#define		SPEAKER_SRC_CONFSEL				(u8)2		// �����ڲ��߼�ѡ��

enum emChangeSpeakerSrcReason
{
	emReasonUnkown = 0,
	emReasonChangeSpeaker,
	emReasonChangeChairman,
	emReasonChangeBrdSrc
};


//����ͷ���Ʋ�������
#define     CAMERA_CTRL_UP					1   //��
#define     CAMERA_CTRL_DOWN				2   //��
#define     CAMERA_CTRL_LEFT				3   //��
#define     CAMERA_CTRL_RIGHT				4   //��
#define     CAMERA_CTRL_UPLEFT				5   //����
#define     CAMERA_CTRL_UPRIGHT				6   //����
#define     CAMERA_CTRL_DOWNLEFT			7   //����
#define     CAMERA_CTRL_DOWNRIGHT			8   //����
#define     CAMERA_CTRL_ZOOMIN				9   //��ҰС
#define     CAMERA_CTRL_ZOOMOUT				10  //��Ұ��
#define     CAMERA_CTRL_FOCUSIN				11  //������
#define     CAMERA_CTRL_FOCUSOUT			12  //������
#define     CAMERA_CTRL_BRIGHTUP			13  //���ȼ�
#define     CAMERA_CTRL_BRIGHTDOWN			14  //���ȼ�
#define     CAMERA_CTRL_AUTOFOCUS			15  //�Զ�����

// ��ƵԴ����
#define      MAX_OUTPUTNUM					7
#define      MATRIX_MAXPORT					16	

// ʹ����ƵԴ���
#define      VIDEO_OUTPUT_CLOSE				0xfb
#define      VIDEO_OUTPUT_OPEN				0xfc

//MP ��س���
#ifdef _MINIMCU_
#define      MAX_SWITCH_CHANNEL				500
#else
#define      MAX_SWITCH_CHANNEL				500
#endif
#define      MAX_IP_NUM						2

//������ʽ
// guzh [2/13/2007] ˵�������ڽ�����ʽ��4.0R3��ǰ��Ҫ��������
// һ�� H.320 ������������
// ���� ҵ�����ֽ�������ʱ�Ƿ�Ҫ���ţ������ SWITCH_MODE_SELECT ��������
// 
// R4�Ż��󣬵�һ�����ñ���
// �ڶ�������Ŀǰ�߼�ʼ�ն��ն˿�MP�������ţ����Ա��ֶβ���������
// ��������ת����˵�������ɱ�����MP�������ŵ��߼�
// ͬʱ���Ӹ�Ϊ��ʶ֪ͨMP�Ľ�����Ϣ���ͣ�������ȫ�ı䣩
// MP���ݱ�������������ͨ���ڲ���ת�ڵ㣨ÿ����������Ƶ��һ·��ת��ת������ֱ��ת������
#define      SWITCH_MODE_NONE               0   //��Ч��ѡ����ʽ
#define      SWITCH_MODE_BROADCAST			1	//�㲥������ʽ
#define      SWITCH_MODE_SELECT				2	//ѡ��������ʽ


//ȱʡ���ж˿�
#define      DEFAULT_Q931_PORT				MCU_Q931_PORT

//��Ϣ�������
#define      SERV_MSG_LEN					0x7000  //��Ϣ����
#define      SERV_MSGHEAD_LEN				0x30    //��Ϣͷ����


//��Ƶ��������
#define      MATRIXTYPE_INNER				1	//�ڲ���Ƶ����
#define      MATRIXTYPE_VAS					2	//VAS��
#define      MATRIXTYPE_EXTRON				3	//Extron��Ƶ��������

#define      MATRIXPORT_IN					1	//����˿�
#define      MATRIXPORT_OUT					2	//����˿�

#define      MATRIXPORT_SVIDEO				0	//S����
#define      MATRIXPORT_ENCODER				1	//�������˿�
#define      MATRIXPORT_DECODER				1	//�������˿�

#define      PRSCHANMODE_FIRST              1   //��һ��Ƶͨ��
#define      PRSCHANMODE_SECOND             2   //�ڶ���Ƶͨ��
#define		 PRSCHANMODE_BOTH               3   //������Ƶͨ��
#define      PRSCHANMODE_AUDIO              4   //��Ƶͨ��
#define      PRSCHANMODE_AUDBAS             5   //���������Ƶ�ش�ͨ��
#define      PRSCHANMODE_VIDBAS             6   //��ý���������Ƶ�ش�ͨ��
#define      PRSCHANMODE_BRBAS              7   //�������������Ƶ�ش�ͨ��
#define      PRSCHANMODE_VMP2               8   //VMP�ڶ�·�����Ķ����ش�ͨ��

// ���Ӹ���PRSͨ��, zgc, 2008-08-13
#define      PRSCHANMODE_HDBAS_VID          9   //����BAS����Ƶ�Ķ����ش�ͨ��
#define      PRSCHANMODE_HDBAS_DVID         10  //����BAS˫���Ķ����ش�ͨ��

// xliang [4/30/2009]  ��·�����Ĺ㲥Դ(��MPU) PRSͨ��
#define      PRSCHANMODE_VMPOUT1			11	//1080��
#define      PRSCHANMODE_VMPOUT2				12  //720��
#define		 PRSCHANMODE_VMPOUT3			13	//4CIF
#define		 PRSCHANMODE_VMPOUT4		14	//CIF/other

#define      AP_MIN                         96  //��̬�غ���Сֵ
#define      AP_MAX                         126 //��̬�غ���Сֵ

/*����MPC�ĵ����λ��*/
#define		 MCU_BOARD_MPC					16
// [pengjie 2010/12/9] 0��7�� boardId 8 ΪIS2���λ�ã�MPCD����ʹ��boardId 32
//#define      MCU_BOARD_MPCD                 8
#define      MCU_BOARD_MPCD                 32
// End

#define      BOARD_INLINE                   (u8)1     // ��������
#define      BOARD_OUTLINE                  (u8)2     // ���岻����
#define      BOARD_UNKNOW                   (u8)3     // ����δ֪
#define      BOARD_CFGCONFLICT              (u8)4     // �������ó�ͻ(��ʱδʹ��)

//�������Ͷ���
#define		BRD_TYPE_MPC					0
#define		BRD_TYPE_DTI					1
#define		BRD_TYPE_DIC					2
#define		BRD_TYPE_CRI					3
#define		BRD_TYPE_VAS					4
#define		BRD_TYPE_MMP					5
#define		BRD_TYPE_DRI					6
#define		BRD_TYPE_IMT					7
#define		BRD_TYPE_APU					8
#define		BRD_TYPE_DSI					9
#define		BRD_TYPE_VPU					10
#define		BRD_TYPE_DEC5					11

//  [1/21/2011 chendaiwei]CRI2֧��
#define		BRD_TYPE_CRI2					12

#define     BRD_TYPE_DSC                    13

#define     BRD_TYPE_DRI2                   14
//  [1/21/2011 chendaiwei]DRI2֧��
#define     BRD_TYPE_DRI16					15


#define     BRD_TYPE_MDSC                   0x10
#define     BRD_TYPE_16E1                   0x11
#define     BRD_TYPE_HDSC                   0x14
#define     BRD_TYPE_IS21                   0x22
#define     BRD_TYPE_IS22                   0x23
//  [1/21/2011 chendaiwei]MPC2֧��
#define		BRD_TYPE_MPC2					0x24
//4.6�汾���������� jlb
#define     BRD_TYPE_HDU                    0x46
#define     BRD_TYPE_MPU                    0x16
#define     BRD_TYPE_EBAP                   0x17
#define     BRD_TYPE_EVPU                   0x18
//tianzhiyong 100211  EAPU
#define		BRD_TYPE_EAPU					0x19
#define     BRD_TYPE_HDU_L					0x20

// V4R7������������֧�� [11/29/2011 chendaiwei]
#define     BRD_TYPE_HDU2                   0x25
#define     BRD_TYPE_MPU2					0x26
#define     BRD_TYPE_MPU2ECARD				0x27
#define		BRD_TYPE_APU2					0x28
#define     BRD_TYPE_HDU2_L                 0x29
#define     BRD_TYPE_HDU2_S			        0x2a

#define		BRD_TYPE_UNKNOW					0xff

#define     BRD_STATUS_NORMAL	            0  //����״̬����
#define		BRD_STATUS_ABNORMAL             1  //����״̬�쳣
#define     BRD_CPU_THRESHOLD               85 //����CPUռ���ʷ�ֵ����������Ϊ�쳣
#define     BRD_TEMPERATURE_THRESHOLD       80 //�����¶ȷ�ֵ����������Ϊ�쳣

//��������������
// DRI ������Ϊ: DRI / TUI
// VPU ������Ϊ: VPU / BAP
// CRI ������Ϊ: CRI / PRS / TUI / HDI
#define     BRD_ALIAS_MPC                   "MPC"
#define     BRD_ALIAS_CRI                   "CRI"
#define     BRD_ALIAS_IS21                  "IS21"
#define     BRD_ALIAS_IS22                  "IS22"
#define     BRD_ALIAS_MPC2                  "MPC2"
//  [1/21/2011 chendaiwei] ֧��CRI2,DRI2
#define     BRD_ALIAS_DRI2                  "DRI2"
#define     BRD_ALIAS_CRI2                  "CRI2"
#define     BRD_ALIAS_DRI                   "DRI"
#define     BRD_ALIAS_APU                   "APU"
#define     BRD_ALIAS_EAPU                  "EAPU"
#define     BRD_ALIAS_VPU                   "VPU"
#define     BRD_ALIAS_DEC5                  "DEC5"
#define     BRD_ALIAS_DSC                   "DSC"
#define     BRD_ALIAS_MDSC                  "MDSC"
#define     BRD_ALIAS_HDSC                  "HDSC"
#define     BRD_ALIAS_TUI                   "TUI"
#define     BRD_ALIAS_HDI                   "HDI"
#define     BRD_ALIAS_PRS                   "PRS"
#define     BRD_ALIAS_BAP                   "BAP"
#define     BRD_ALIAS_MMP                   "MMP"
//4.6�汾������������������ jlb
#define     BRD_ALIAS_MPU                   "MPU"
#define     BRD_ALIAS_HDU                   "HDU"
#define     BRD_ALIAS_EBAP                  "EBAP"
#define     BRD_ALIAS_EVPU                  "EVPU"
#define     BRD_ALIAS_HDU_L                 "HDU-L"
#define     BRD_ALIAS_HDU2                  "HDU2"
#define		BRD_ALIAS_MPU2					"MPU2"
#define     BRD_ALIAS_MPU2_ECARD			"MPU2-E"
#define     BRD_ALIAS_APU2                  "APU2"
#define     BRD_ALIAS_HDU2_L                "HDU2-L"
#define     BRD_ALIAS_HDU2_S                "HDU2-S"

#define     MAXLEN_BRDALIAS                 8   // ��Ч����7

#define      DISABLE						0	//��Ч
#define      ENABLE							1	//��Ч

#define		ISTRUE(b)						((b)!=0) 
#define		GETBBYTE(b)						((b)?1:0)
#define		kmax(a,b)						(((a) > (b)) ? (a) : (b))
#define		kmin(a,b)						(((a) < (b)) ? (a) : (b))
#define		astrncpy(a, b, al, bl)  { s32 l = min(al, bl); strncpy((s8 *)a, b, l); a[l-1] = 0;}

#define GK_RRQ_NAME_LEN  64 //GK RRQ�ʺ�
#define GK_RRQ_PWD_LEN   64 //GK RRQ����  

enum emDisconnectReason
{
	emDisconnectReasonDRQ = 1,      // DRQ from GK
	emDisconnectReasonRtd,          // roundtripdelay timeout
	emDisconnectReasonBusy,
	emDisconnectReasonNormal,
	emDisconnectReasonRejected,
	emDisconnectReasonUnreachable,
	emDisconnectReasonLocal,
    emDisconnectReasonUnmatched,    // ��ԭ�����ҵ���壬��Э��ջ�޹ء������䡣
	emDisconnectReasonUnknown,
	emDisconnectReasonBysyExt,
	emDisconnectReasonRemoteReconnect,
	emDisconnectReasonRemoteConfHolding,
	emDisconnectReasonRemoteHasCascaded
};

#define  ADDMTMODE_MCS                  1   //�ն��Ǳ�MCS�����
#define  ADDMTMODE_CHAIRMAN             2   //�ն��Ǳ���ϯ�����
//#define  ADDMTMODE_MTSELF               3   //�ն����Լ������

#define  MIXERID_MIN                    1
#define  MIXERID_MAX                    16
#define  RECORDERID_MIN                 17
#define  RECORDERID_MAX                 32
#define  TVWALLID_MIN                   33
#define  TVWALLID_MAX                   48
#define  BASID_MIN                      49
#define  BASID_MAX                      64
#define  VMPID_MIN                      65
#define  VMPID_MAX                      80
#define  VMPTWID_MIN                    81
#define  VMPTWID_MAX                    96
#define  PRSID_MIN                      97
#define  PRSID_MAX                      112
#define  HDUID_MIN                      113
#define  HDUID_MAX                      182

#define  DEFAULT_PRS_CHANNELS           (u8)3       // Ĭ����Ҫ��Prsͨ����
#define  MAX_TIMES_CREATE_CONF			(u8)5       // ����Դ������
#define  LEN_WAIT_TIME					(u32)5000   // �ȴ�ʱ�䲽��
#define  MAXNUM_CHECK_CAS_TIMES         (u8)3

#define  PORTSPAN                       (u16)10     // �˿ڵļ��
#define  HDU2_PORTSPAN					(u16)20     // HDU2(0ͨ����1ͨ���˿ڵļ��)
#define  COMPOSECHAN(a, b)				((a)*HDU2_PORTSPAN + b)
#define	 RESTORECHAN(a, b)				((a) - (b))

//UDP �˿ڷ��䣬ʹ�÷�Χ��39000--65500

#define CASCADE_SPY_STARTPORT			(u16)30000	// 30000��37000 �ϼ����ɻش�Դ�˿ڣ�10 * 512 = 5120; ���� 1880 
#define CASCADE_BRD_STARTPORT			(u16)38000	// 37000��39000	�¼������ϼ��㲥�˿ڣ�10 * 128 = 1280; ���� 720

//����mcu��Ĭ����ʼ�˿� (VMP)
#define VMP_MCU_STARTPORT               (u16)39000  // 39000��39900 �����VMP: 30  * 16 = 480;    ���ж˿�: 420

//����Ĭ����ʼ�˿�
//����BAS/VMP/MIX/PRS��������MCU8000B�ϣ��ʲ��ܺ�������˿ڳ�ͻ
//tianzhiyong 20100323  APU��EAPU�������ʼ�˿�ͳһ��ԭ���ķ�Χ��������ǰռ����100
#define MIXER_EQP_STARTPORT             (u16)39900  // 39900��40600 �����һ��MIX: ���ж˿�: 60
#define BAS_EQP_STARTPORT               (u16)40600  // 40600��40700 �����һ��BAS: ���ж˿�: 50
#define VMP_EQP_STARTPORT               (u16)40700  // 40700��40900 �����һ��VMP: ���ж˿�: 40
#define VMP_RCVRTCP_START_PORT          (u16)20000  //vmp�����rtcp����ʼ�˿�
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	#define PRS_EQP_STARTPORT               (u16)41200  // 41200 ��41490 �����PRS 300���˿�
#else
	#define PRS_EQP_STARTPORT               (u16)40900  // 40900��41000 �����һ��PRS: ���ж˿�: 40
#endif
#define TVWALL_EQP_STARTPORT            (u16)41000  // 41000��41500 �����һ��TW:  ���ж˿�: xxx
#define MTW_EQP_STARTPORT               (u16)41500  // �����Ǻ�MCU���������ʼ�˿ڳ�ͻ
#define HDU_EQP_STARTPORT				(u16)42000	// 42000��42100 �����һ��HDU: ���ж˿�: 80
#define REC_EQP_STARTPORT               (u16)7200   // ���ǵ�������ǽ�������¼��������ã�

//����mcu��Ĭ����ʼ�˿�
#define REC_MCU_STARTPORT               (u16)41500  // 41500��46700 �����REC: 320 * 16 = 5120;   ���ж˿�: 80
// xliang [12/22/2008]  vmp����ǰ��
// 48000��53000 Ϊ����ǽ����ռ�ö˿�(������xDSC�ϣ�������PRS/MT����˿ڳ�ͻ);
#define MIXER_MCU_STARTPORT             (u16)10000  // 50000��58000 �����MIX: 570 * 14 = 8980;   ���ж˿�: 20
#define BAS_8KE_LOCALSND_PORT			(u16)55700  // 55700��55900 �����8kebas: 120���˿�		  ���ж˿ڣ�80
#define VMP_8KEOR8KH_LOCALSND_PORT	    (u16)55900  // 55900��56000	�����8kevmp: 40���˿�        ���ж˿ڣ�60
#define BAS_MCU_STARTPORT               (u16)53000  // 53000��58000 �����BAS: 120 * 16 = 1920;    ���ж˿�: 70 
#define PRS_MCU_STARTPORT               (u16)58000  // 58000��59000 �����PRS: 60  * 16 = 960;    ���ж˿�: 40
#define RESV_MCU_STARTPORT              (u16)59000  // 59000��61000 ΪMCU�ڲ��㲥Ĭ�϶˿�: 10 * 128 = 1280;   ���ж˿�: 720
													// 61000��65530 ������ն�: 10 * 448 = 4480;  ���ж˿�: 50
//�ն�mcu��Ĭ����ʼ�˿�
#define MT_MCU_STARTPORT                (u16)61000

//����mcu��˿ڷ�����
#define BAS_8KE_PORTSPAN               (u16)(4 * PORTSPAN)//8KEBAS����˿ڼ��
#define BAS_8KH_PORTSPAN               (u16)(5 * PORTSPAN)//8KHBAS����˿ڼ��
#define BAS_MCU_PORTSPAN               (u16)(12 * PORTSPAN)/*(5 * PORTSPAN)*/
#define VMP_MCU_PORTSPAN               (u16)(4 * PORTSPAN)/*(2 * PORTSPAN)*/
#define MPW_MCU_PORTSPAN               (u16)(2 * PORTSPAN)
#define PRS_MCU_PORTSPAN               (u16)(6 * PORTSPAN)   
#define MIXER_MCU_PORTSPAN             (u16)(65 * PORTSPAN) //apu eqpu  [1/31/2012 chendaiwei]
#define APU2_MIXER_MCU_PORTSPAN        (u16)((64+15) * PORTSPAN) //apu2 64·N-1 15·Nģʽ  [1/31/2012 chendaiwei]
#define REC_MCU_PORTSPAN               (u16)(32 * PORTSPAN)
#define HDU_MCU_PORTSPAN               (u16)(2 * PORTSPAN)

#define BAS_BASIC_PORTSPAN			   (u16)( (MPU2_BAS_BASIC_MVOUTPUT+MPU2_BAS_BASIC_DSOUTPUT+MPU2_BAS_BASIC_SELOUTPUT*2)* PORTSPAN )	//MPU2 bas basic eqp
#define BAS_ENHANCED_PORTSPAN		   (u16)( (MPU2_BAS_ENHACNED_MVOUTPUT+MPU2_BAS_ENHACNED_DSOUTPUT+MPU2_BAS_ENHACNED_SELOUTPUT*5)* PORTSPAN ) //MPU2 bas enhanced eqp
#define BAS_APU2_PORTSPAN			   (u16)(MAXNUM_APU2BAS_OUTCHN*MAXNUM_APU2_BASCHN*PORTSPAN)
#define VMP_MPU2_PORTSPAN			   (u16)90  //MPU2 vmp eqp

// �鲥��ַ����ʼ/����IP��
#define MULTICAST_IP_START              ntohl(inet_addr("225.0.0.0"))
#define MULTICAST_IP_END                ntohl(inet_addr("239.255.255.255"))
// �鲥��ַ����Ҫ������IP��
#define MULTICAST_IP_RESV_START         ntohl(inet_addr("231.255.255.255"))
#define MULTICAST_IP_RESV_END           ntohl(inet_addr("233.0.0.0"))
// Ĭ���鲥�˿�
#define MULTICAST_DEFPORT               (u16)60100

#define MAXBITRATE_MTSEND_INVMP         (u16)4032   //����ϳ�ʱ�ն����������(4096-64)

//����telnet�˿ڶ���
#define BRD_TELNET_PORT                 2500
#define BRDMASTERSYS_LISTEN_PORT        6682  // [2012/06/01 liaokang] ֧��IS2.2�ڲ�ͨ��

//mcu telnet �˿ڶ���
#define MCU_TELNET_PORT                 2500

// ����Linux��inet_addrʱ���� "0"��Ϊ�Ǵ����ַ��������Ҫ����һ��
// ��������Linux��صĵ�ַת��ʹ�ñ��ꡣ
// ����Ҫ���ַ�����\0��β
#define INET_ADDR( szAddr )   ( strncmp(szAddr, "0", strlen(szAddr)) == 0 ? 0 : inet_addr(szAddr) )

//�û�����չ
#define MAXNUM_USRGRP                   32
#define MAXNUM_GRPUSRNUM                MAX_USERNUM
#define MAXNUM_IPSEG                    3

//����û�����
#define MAXNUM_USRNUM                   MAX_USERNUM

// �Ϸ�����ID�� 1 - MAXNUM_USRGRP���Լ� USRGRPID_SADMIN
#define USRGRPID_INVALID                (u8)0
#define USRGRPID_SADMIN                 (u8)0xFF

//MCU����Ӧ��OSP��ʼ��Ȩ����
#define MCU_TEL_USRNAME                 "admin"
#define MCU_TEL_PWD                     "admin"


//NPlus��Mcs����ĺ꣬Ϊ�˴�����ֵ
#define NPLUS_NONE                      (u8)200    //��NPlusģʽ

#define NPLUS_BAKCLIENT_IDLE            (u8)201     //N�� - ����״̬
#define NPLUS_BAKCLIENT_MSGLEN_ERR      (u8)202     //N�� - ���ӱ��ݶ�ʧ�ܣ���Ϣ�������ʾΪδ֪����
#define NPLUS_BAKCLIENT_CAP_ERR         (u8)203     //N�� - ���ӱ��ݶ�ʧ�ܣ����ݶ���������֧��
#define NPLUS_BAKCLIENT_OVERLOAD        (u8)204     //N�� - ���ӱ��ݶ�ʧ�ܣ����ݶ˱�����������
#define NPLUS_BAKCLIENT_ENV_UNMATCHED   (u8)205     //N�� - ���ӱ��ݶ�ʧ�ܣ����ݶ�Ϊ8000B����֧�ֶԱ���KDV8000�ı���; �� ���ݶ�Ϊ8000C �� ����Ϊ8000C �� ���߾�Ϊ8000C�����ܹ��ɱ���
#define NPLUS_BAKCLIENT_CONNECT_MASTER  (u8)206     //N�� - ���ӱ��ݶ�ʧ�ܣ����ݶ�Ϊ����MCU �� ��N+1ģʽ��MCU
#define NPLUS_BAKCLIENT_CHECKENCODING_ERR (u8)207   //N�� - ���ӱ��ݶ�ʧ�ܣ����ݶ�У����뷽ʽ��ͨ��
#define NPLUS_BAKCLIENT_CONNECT         (u8)210     //N�� - �����ӷ�����

#define NPLUS_BAKSERVER_IDLE            (u8)211     //���ݶ� - ����״̬
#define NPLUS_BAKSERVER_SWITCHED        (u8)212     //���ݶ� - ���л�
#define NPLUS_BAKSERVER_SWITCHERR       (u8)213     //���ݶ� - �л�ʧ��

//8000B�����������⴦�� �궨��
#define MAXNUM_MIX_SINGLE_MP3           (u8)32      //MP3����(���ƺ�����)��������
#define MAXNUM_MIX_MULTI_MP3            (u8)8       //MP3����(���ƺ�����)������(VMP��BAS)�쿪����
#define MAXNUM_MIX_SINGLE_G729          (u8)10      //G729����(���ƺ�����)��������
#define MAXNUM_MIX_MULTI_G729           (u8)4       //G729����(���ƺ�����)������(VMP��BAS)�쿪����
#define MAXNUM_VAC_SINGLE_MP3           (u8)32      //MP3��VAC��������
#define MAXNUM_VAC_MULTI_MP3            (u8)16      //MP3��VAC������(BAS)�쿪����
#define MAXNUM_VAC_SINGLE_G729          (u8)16      //G729��VAC��������
#define MAXNUM_VAC_MULTI_G729           (u8)4       //G729��VAC������(BAS)�쿪����

//mcu��mcs�Ự�汾���û��������ʱ��֤���޸����ݽṹӰ��mcs��mcu����ʱ��ֵ��1��45����4.0R5�汾��01����ÿ���޸İ汾��
#define MCUMCS_VER                      (u16)46009  //temperary

// zw [06/27/2008] AAC LC����
#define AAC_SAMPLE_FRQ_8                (u8)1
#define AAC_SAMPLE_FRQ_11               (u8)2
#define AAC_SAMPLE_FRQ_12               (u8)3
#define AAC_SAMPLE_FRQ_16               (u8)4
#define AAC_SAMPLE_FRQ_22               (u8)5
#define AAC_SAMPLE_FRQ_24               (u8)6
#define AAC_SAMPLE_FRQ_32               (u8)7
#define AAC_SAMPLE_FRQ_44               (u8)8
#define AAC_SAMPLE_FRQ_48               (u8)9
#define AAC_SAMPLE_FRQ_64               (u8)10
#define AAC_SAMPLE_FRQ_88               (u8)11
#define AAC_SAMPLE_FRQ_96               (u8)12

#define AAC_CHNL_TYPE_CUST              (u8)11
#define AAC_CHNL_TYPE_SINGLE            (u8)1
#define AAC_CHNL_TYPE_DOUBLE            (u8)2
#define AAC_CHNL_TYPE_3                 (u8)3
#define AAC_CHNL_TYPE_4                 (u8)4
#define AAC_CHNL_TYPE_5                 (u8)5
#define AAC_CHNL_TYPE_5P1               (u8)51
#define AAC_CHNL_TYPE_7P1               (u8)71

#define AAC_MAX_FRM_NUM                 1


// xliang [11/28/2008] vmpͨ����Ŀ FIXME
#define MAX_SVMP_HDCHNNLNUM				(u8)3		//
#define MAX_SVMP_4CIFCHNNLNUM			(u8)16
#define MAX_SVMP_CIFCHNNLNUM			(u8)12

// xliang [11/28/2008] vmp ��Ա�漰���ȼ������� -�ݲ���
#define VMPMEMBER_NORMAL				(u8)1	//��ͨMT
#define VMPMEMBER_BESELECTED			(u8)2	//��ѡ��
#define VMPMEMBER_NONEKEDA				(u8)3	//�ǿƴ�
#define VMPMEMBER_SPEAKER				(u8)4	//������

// xliang [12/23/2008] vmp������ѯʱ��T������
#define VMP_BATCHPOLL_TMAX				(u32)(300*1000)
#define VMP_BATCHPOLL_TMIN				(u32)(60*1000)

//4.6�¼Ӱ汾  jlb
//HDU ����ӿ�
#define HDU_OUTPUT_YPbPr   (u8)0    
#define HDU_OUTPUT_VGA     (u8)1 
#define HDU_OUTPUT_DVI     (u8)2
#define HDU_OUTPUT_HDMI    (u8)3
#define HDU_OUTPUT_C       (u8)4           //C��������ӿ�����
#define HDU_OUTPUT_SDI     (u8)5           //SDI��������ӿ�����

#define HDU_VOLUME_DEFAULT (u8)24

//���ű���
#define HDU_ZOOMRATE_4_3   (u8)0  
#define HDU_ZOOMRATE_16_9  (u8)1

//HDU �����ʽ
#define HDU_YPbPr_1080P_24fps         (u8)0
#define HDU_YPbPr_1080P_25fps         (u8)1
#define HDU_YPbPr_1080P_30fps         (u8)2
#define HDU_YPbPr_1080P_50fps         (u8)3
#define HDU_YPbPr_1080P_60fps         (u8)4
#define HDU_YPbPr_1080i_50HZ          (u8)5    //(Ĭ��)
#define HDU_YPbPr_1080i_60HZ          (u8)6
#define HDU_YPbPr_720P_60fps          (u8)7
#define HDU_YPbPr_576i_50HZ           (u8)8
#define HDU_YPbPr_480i_60HZ           (u8)9


#define HDU_VGA_SXGA_60HZ            (u8)10
#define HDU_VGA_XGA_60HZ             (u8)11     //(Ĭ��)
#define HDU_VGA_XGA_75HZ             (u8)12
#define HDU_VGA_SVGA_60HZ            (u8)13
#define HDU_VGA_SVGA_75HZ            (u8)14
#define HDU_VGA_VGA_60HZ             (u8)15
#define HDU_VGA_VGA_75HZ             (u8)16
#define HDU_VGA_SXGA_75HZ            (u8)17
#define HDU_YPbPr_720P_50fps         (u8)18  //zjl[20091216]�¼�hdu�����ʽ
#define HDU_VGA_WXGA_1280_768_60HZ   (u8)19
#define HDU_VGA_WXGA_1280_800_60HZ   (u8)20
#define HDU_VGA_WSXGA_60HZ           (u8)21
#define HDU_VGA_SXGAPlus_60HZ        (u8)22
#define HDU_VGA_UXGA_60HZ            (u8)23
#define HDU_YPbPr_1080P_29970HZ      (u8)24
#define HDU_YPbPr_1080P_59940HZ      (u8)25
#define HDU_WXGA_1280_800_75HZ       (u8)26
#define HDU_C_576i_50HZ              (u8)27     //C����570i50Hz�����ʽ
#define HDU_C_480i_60HZ              (u8)28     //C����480i60Hz�����ʽ

//�鲥������ʽ
#define CAST_UNI					 (u8)1
#define CAST_FST				     (u8)2
#define CAST_SEC				     (u8)3
#define CAST_DST				     (u8)4
#define MAXNUM_SAT_MODEM			 (u8)16

//VCS֧�����õ�����û���
#define MAXNUM_VCSUSERNUM               MAX_USERNUM

//VCS�����ն˳�ʱʱ��
#define DEFAULT_VCS_MTOVERTIME_INTERVAL      (u8)10

//�ն˹ؼ�֡�����ֻ�ʱ��(��)
#define DEFAULT_MT_MCU_FASTUPDATE_INTERVAL	 (u8)10


//��������������ƭ���ն��ͺ���
#define MAXNUM_CONFCAPCHEATMTBOARDNUM	 (u8)20
//�����ն�������
#define MAXNUM_CONFHDMTBOARDNUM	 (u8)10

#define		arraynum(a)						(sizeof(a)/sizeof(a[0]))

// ������������Ϣ��ȡ // [1/13/2010 xliang]  
#define MCU_MAX_ADAPTER_DESCRIPTION_LENGTH  128 
#define MCU_MAX_ADAPTER_NAME_LENGTH         256 
#define MCU_MAX_ADAPTER_ADDRESS_LENGTH      8   
#define MCU_DEFAULT_MINIMUM_ENTITIES        32  
#define MCU_MAX_HOSTNAME_LEN                128 
#define MCU_MAX_DOMAIN_NAME_LEN             128 
#define MCU_MAX_SCOPE_ID_LEN                256 
#define MCU_MAXNUM_ADAPTER_IP               (u32)16
#define MCU_MAXNUM_ADAPTER_GW               (u32)16
#define MCU_MAXNUM_ADAPTER_ROUTE            (u32)16
#define MCU_MAXNUM_ADAPTER                  (u32)16
#define MCU_MAXNUM_MULTINET					(u32)(MCU_MAXNUM_ADAPTER_IP-2)

#define MCU_MIB_IF_TYPE_OTHER               1
#define MCU_MIB_IF_TYPE_ETHERNET            6
#define MCU_MIB_IF_TYPE_TOKENRING           9
#define MCU_MIB_IF_TYPE_FDDI                15
#define MCU_MIB_IF_TYPE_PPP                 23
#define MCU_MIB_IF_TYPE_LOOPBACK            24
#define MCU_MIB_IF_TYPE_SLIP                28

#define MEDIA_TYPE_G7221C_SUBTYPE_24K       24
#define MEDIA_TYPE_G7221C_SUBTYPE_32K       32
#define MEDIA_TYPE_G7221C_SUBTYPE_48K       48

// 8000EӲ���������������Ӧ������
#define G_ETH1      (u8)1
#define G_ETH2      (u8)2
#define ETH         (u8)0

#define MAXNUM_ETH_INTERFACE			(u8)3
#define MAXNUM_E1PORT                   (u8)8

//�ñ�ע��UTF8��ʽ��ÿ���ַ�ռ3���ֽڣ��ܹ�18���ַ���ĩβ + '\0'
#define MAXLEN_NOTES 					(u8)55		//���ע����

//�����ն˱���ö������
enum emDebugMode
{
	emDebugModeNone,
	//emDebugModeftp,
	emDebugModeDebug
};

//WD �����
#ifdef WIN32
#define MD_NAME_8000E_GUARD      (LPCSTR) "guard.exe"
#define MD_NAME_8000E_MCU        (LPCSTR) "mcu_8000e.exe"
#define MD_NAME_8000E_MMP        (LPCSTR) "winmmp.exe"
#define MD_NAME_8000E_TS         (LPCSTR) "kdvts_8000e.exe"
#define MD_NAME_8000E_GK         (LPCSTR) "kdvgk_8000e.exe"
#define MD_NAME_8000E_PXY        (LPCSTR) "pxysrvapp_8000e.exe"
#define MD_NAME_8000E_DSSERVER   (LPCSTR) "dsserverapp.exe"
#define UP_NAME_MCU_TOOL         (LPCSTR) "updatetool.exe"
#define UP_NAME_MCU_FILE         (LPCSTR) "mcuupdate.upd"
#define MD_NAME_KDVE_MPS		 (LPCSTR) "mps_kdve.exe"
#define MD_NAME_KDVE_MMP		 (LPCSTR) "mmp_kdve.exe"
#define MD_NAME_KDVE_MPU		 (LPCSTR) "mpu_kdve.exe"
#define MD_NAME_KDVE_MTADP		 (LPCSTR) "mtadp_kdve.exe"
#define MD_NAME_KDVE_MP			 (LPCSTR) "mp_kdve.exe"
#define MD_NAME_KDVE_PRS		 (LPCSTR) "prs_kdve.exe"
#define MD_NAME_KDVE_TUI		 (LPCSTR) "tui_kdve.exe"
//---------------��ӪMCU ģ����-------------------------//
#define MD_NAME_CARRIER_GUARD    (LPCSTR) "kdvpguard.exe"
#define MD_NAME_CARRIER_MCU      (LPCSTR) "mcu_kdvp.exe"
#define MD_NAME_CARRIER_MPS      (LPCSTR) "mps_kdvp.exe"
#define MD_NAME_CARRIER_MMP      (LPCSTR) "mmp_kdvp.exe"
#define MD_NAME_CARRIER_MPU      (LPCSTR) "mpu_kdvp.exe"
#define MD_NAME_CARRIER_MTADP    (LPCSTR) "mtadp_kdvp.exe"
#define MD_NAME_CARRIER_MP       (LPCSTR) "mp_kdvp.exe"
#define MD_NAME_CARRIER_PRS      (LPCSTR) "prs_kdvp.exe"
#define MD_NAME_CARRIER_TUI      (LPCSTR) "tui_kdvp.exe"
#define MD_NAME_CARRIER_KDVTS    (LPCSTR) "kdvpts.exe"
#define MD_NAME_CARRIER_DSSERVER (LPCSTR) "dsserverapp.exe"
#define UP_NAME_TUI_TOOL		 (LPCSTR) "updatetool.exe"
#define UP_NAME_TUI_FILE		 (LPCSTR) "tuiupdate.upd"
#define UP_NAME_MPS_TOOL         (LPCSTR) "updatetool.exe"
#define UP_NAME_MPS_FILE         (LPCSTR) "mmpupdate.upd"
//------------------------------------------------------//
#else
#define MD_NAME_8000E_GUARD      (LPCSTR) "guard"
#define MD_NAME_8000E_MCU        (LPCSTR) "mcu_8000e"
#define MD_NAME_8000E_MMP        (LPCSTR) "mmp_8000e"
#define MD_NAME_8000E_TS         (LPCSTR) "kdvts_8000e"
#define MD_NAME_8000E_GK         (LPCSTR) "kdvgk_8000e"
#define MD_NAME_8000E_PXY        (LPCSTR) "pxyserverapp_8000e"
#define MD_NAME_8000E_DSSERVER   (LPCSTR) "dsserverapp"
#define UP_NAME_MCU_TOOL         (LPCSTR) "./mcuupdate.bin"
#define UP_NAME_MCU_FILE         (LPCSTR) "mcuupdate.bin"
#define MD_NAME_KDVE_MPS		 (LPCSTR) "mps_kdve"
#define MD_NAME_KDVE_MMP		 (LPCSTR) "mmp_kdve"
#define MD_NAME_KDVE_MPU		 (LPCSTR) "mpu_kdve"
#define MD_NAME_KDVE_MTADP		 (LPCSTR) "mtadp_kdve"
#define MD_NAME_KDVE_MP			 (LPCSTR) "mp_kdve"
#define MD_NAME_KDVE_PRS		 (LPCSTR) "prs_kdve"
#define MD_NAME_KDVE_TUI		 (LPCSTR) "tui_kdve"
//---------------��ӪMCU ģ����-------------------------//
#define MD_NAME_CARRIER_GUARD    (LPCSTR) "kdvpguard"
#define MD_NAME_CARRIER_MCU      (LPCSTR) "mcu_kdvp"
#define MD_NAME_CARRIER_MPS      (LPCSTR) "mps_kdvp"
#define MD_NAME_CARRIER_MMP      (LPCSTR) "mmp_kdvp"
#define MD_NAME_CARRIER_MPU      (LPCSTR) "mpu_kdvp"
#define MD_NAME_CARRIER_MTADP    (LPCSTR) "mtadp_kdvp"
#define MD_NAME_CARRIER_MP       (LPCSTR) "mp_kdvp"
#define MD_NAME_CARRIER_PRS      (LPCSTR) "prs_kdvp"
#define MD_NAME_CARRIER_TUI      (LPCSTR) "tui_kdvp"
#define MD_NAME_CARRIER_KDVTS    (LPCSTR) "kdvpts"
#define MD_NAME_CARRIER_DSSERVER (LPCSTR) "dsserverapp"
#define UP_NAME_TUI_TOOL         (LPCSTR) "./tuiupdate.bin"
#define UP_NAME_TUI_FILE         (LPCSTR) "tuiupdate.bin"
#define UP_NAME_MPS_TOOL         (LPCSTR) "./mmpupdate.bin"
#define UP_NAME_MPS_FILE         (LPCSTR) "mmpupdate.bin"
//------------------------------------------------------//
#endif


#endif


