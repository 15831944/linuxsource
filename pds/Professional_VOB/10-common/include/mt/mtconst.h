/*******************************************************************************            
 *  ģ����   : MT                                                              *            
 *  �ļ���   : mtconst.h                                                       *            
 *  ����ļ� :                                                                 *            
 *  ʵ�ֹ��� : ��Ƶ����ϵͳ�ն������������ͷ�ļ�                              *            
 *  ����     : ������                                                          *            
 *  �汾     : V3.0  Copyright(c) 1997-2005 KDC, All rights reserved.          *            
 *                                                                             *            
 *    =======================================================================  *            
 *  �޸ļ�¼:                                                                  *            
 *    ��  ��      �汾        �޸���      �޸�����                             *            
 *    2005/8/6    4.0         ������      ����                                 *            
 *                                                                             *            
 *******************************************************************************/           
                                                                                            
                                                                                            
                                                                                            
#ifndef _MT_CONST_H_                                                                        
#define _MT_CONST_H_                                                                        

#include "kdvtype.h"                                                                                            
#include "kdvdef.h"                                                                               
enum EmDataFormat                                                                           
{                                                                                           
	emStr  ,                                                                            
	emHex  ,                                                                            
	emBin  ,                                                                            
//	emBOOL ,                                                                            
	emOct  ,                                                                            
	emIP   ,                                                                            
	emDec  ,                                                                            
	emDW                                                                                
};                                                                                          
                                                                                            
                                                                                            
#define CALLRATE_64_SEL        0x00000001                                                   
#define CALLRATE_128_SEL       0x00000002                                                   
#define CALLRATE_192_SEL       0x00000004                                                   
#define CALLRATE_256_SEL       0x00000008                                                   
#define CALLRATE_320_SEL       0x00000010                                                   
#define CALLRATE_384_SEL       0x00000020                                                   
#define CALLRATE_448_SEL       0x00000040                                                   
#define CALLRATE_512_SEL       0x00000080                                                   
#define CALLRATE_576_SEL       0x00000100                                                   
#define CALLRATE_640_SEL       0x00000200                                                   
#define CALLRATE_704_SEL       0x00000400                                                   
#define CALLRATE_768_SEL       0x00000800                                                   
#define CALLRATE_832_SEL       0x00001000                                                   
#define CALLRATE_896_SEL       0x00002000                                                   
#define CALLRATE_960_SEL       0x00004000                                                   
#define CALLRATE_1024_SEL      0x00008000                                                   
#define CALLRATE_MASK          0x0000FFFF                                                   
                                                                                            
                                                                                             
//
// VOD Limited Condition 
//
#define VOD_DIRECTORY_SEPARATOR ( ( s8* ) "\\" )

const int VOD_MAX_CLIENT = 32; // ��������ӵĿͻ�����
const int VOD_MAX_USER_NAME_LENGTH = 32; // �û�������
const int VOD_MAX_PASSWORD_LENGTH = 32; // ���볤��
const int VOD_MAX_PATH_FILE_NAME_LENGTH = 128;  // ��·�����ļ�������
const int VOD_MAX_DIRECTIRY_NAME_LENGTH = 128; // ·��������
const int VOD_MAX_FILE_NAME_LENGTH = 50; // �ļ������� 
const int VOD_MAX_FILE_NUMBER = 64; // ͬһ�ļ���������ļ���                                                                                        
                                                                                            
                                                                                            
// ���ȶ���                                                                                 
const int    MT_MAX_NAME_LEN            =   32;                                                
const int    MT_MAX_H323ALIAS_LEN       =   32;            // �ն˱�����󳤶�                 
const int    MT_MAX_E164NUM_LEN         =   16;            // �ն�E164��󳤶�                 
//const int    MT_MAX_PORTNAME_LEN        =   16;            // ��Ƶ����˿�����󳤶�           
const int    MT_MAX_AVMATRIX_PORT_NUM   =   64;            // ��Ƶ�������˿ڸ���            
const int    MT_MAX_AVMATRIX_MODEL_NAME =   16; 
const int    MT_MAXLEN_AVMATRIX_SCHEMENAME  = 16;          //���󷽰�������󳤶�
const int    MT_MAX_AVMATRIX_PORTNAME_LEN = 16;            //����˿�����󳤶�
const int    MT_MAX_COMM_LEN            =   32;            // SNMP��ͬ������󳤶�             
const int    MT_MAX_PPPOE_USERNAME_LEN  =   16;            // PPPOE�û�����󳤶�              
const int    MT_MAX_CHANNEL_NAME_LEN    =   16;            // ��ý��Ƶ������󳤶�             
const int    MT_MAX_PASSWORD_LEN        =   16;            // ������󳤶�                     
const int    MT_MAX_FULLFILENAME_LEN    =   128;           // �����ļ�����󳤶�               
const int    MT_MAX_FILEPATH_LEN        =   128;           // ·������󳤶�                   
const int    MT_MAX_CAMERA_NUM          =   6;             // ����ͷ������ 
const int    MT_MAX_CAMERA_EX_NUM       =   64;            // �����չ����ͷ��                  
const int    MT_MAX_CAMERA_NAME_LEN     =   16;            // ����ͷ������󳤶�               
const int    MT_MAX_ETHNET_NUM          =   4;             // �ն���̫�������ֵ               
const int    MT_MAX_E1_NAME_LEN         =   32;            // E1����󳤶�                     
const int    MT_MAX_E1UNIT_NUM          =   4;             // E1ģ�����E1��                   
const int    MT_MAX_ROUTE_NUM           =   16;            // ���·����Ŀ                     
const int    MT_MAX_SERIAL_NUM          =   3 ;            //��മ����Ŀ
const int    MT_MAX_VIDEO_NUM           =   2 ;            //�����Ƶ·��
const int    MT_MAXNUM_VMP_MEMBER       =   16;            //����ϳɳ�Ա��
const int    MT_MAXNUM_SMSDST           =   64;            //����Ϣ����Ŀ�ĳ�Ա���ֵ
const int    MT_MAXLEN_SMSTEXT          =   1024;          //����Ϣ������󳤶�
const int    MT_MAX_IMAGEJUDGEPARAM_NUM =   2;             //������������� 
const int    MT_MAX_POLL_NUM		    =   128;    		//�����ѯ�ն���
const int    MT_MAX_ADDRENTRY_NUM	    =   128;           //��ַ�������Ŀ����
const int    MT_MAX_ADDRMULTISET_NUM    =   16;            //��ַ����������
const int    MT_MAX_NETSND_DEST_NUM     =   5 ;            //˫��������͵�Ŀ�ĵ���
const int    MT_MAX_CALLSITE_TERNINAL   =   16;              //�����Ļ������
const int    MT_MAX_ADDRUSERDEF_LEN     =   1024;          //��ַ���û��Զ����ֶγ���

//const int    MT_MAXNUM_VIDSOURCE        =   10;            //�����ƵԴ��
const int    MT_MAXNUM_CALLADDR_RECORD  =   5;             //�����е�ַ��¼��

const int    MT_PPPoE_MAX_USENAME_LEN     =  255;            //PPPOE����ʱʹ�õ��û�������󳤶�
const int    MT_PPPoE_MAX_PASSWORD_LEN    =  255;            //PPPOE����ʱʹ�õ��������󳤶�
const int    MT_PPPoE_MAX_SERVERNAME_LEN  =  255;            //PPPOE����ʱʹ�õ�ָ��ISP���������ֵ���󳤶�
const int    MT_PPPoE_MAX_VERSION_LEN     =  255;               //PPPOE�汾��Ϣ����󳤶�
const int    MT_MAX_UPDATASERVER_NAME_LEN =  64;              //��������������������󳤶�
const int    MT_MAX_LDAPSERVER_NAME_LEN   =  100;             //LDAP��������������󳤶�
const int    MT_MAX_PROXYSERVER_NAME_LEN  =  255;             //�������ǽ��������󳤶�
const int    MT_MAX_H323GKSERVER_NAME_LEN =  255;              //H323ע��Gk��������󳤶�

//FILESRV: �������������
const int    MT_MAX_FILESRV_BUG_REPORT_LEN   = 512;       //�����汾�е��޸���bug����0
const int    MT_MAX_FILESRV_FILENAME_LEN     = 64;        //�����ļ�������
const int    MT_MAX_FILESRV_DEVFILE_NUM      = 4;         //�����ļ�����
const int    MT_MAX_FILESRV_SOFTWARE_VER_LEN = 32;        //����汾����󳤶�

const int    MT_MAX_PXYIP_NUM                =   5;            // �������pxy֧�ֵ�ip��ַ��   

//WiFi������� 
const int    MT_WIFI_MAX_NAME_LEN            =   32;
const int    MT_WIFI_MAX_PASSWORD_LEN        =   256;
const int    MT_WIFI_MAX_BSSIDCFG_NUM        =   16;  

//HD�ն˵���Ƶ����ڸ���
const int    MT_HD_MAX_AV_OUTPORT_NUM  = 4;    //�����ն˵������Ƶ����ڵĸ���


#define      MT_MAXNUM_MCTERMINAL        6    //�ն�MTC֧�ֺ��е�����ն���
#define      MT_MAXLEN_CAMERA_TYPE_NAME  32    //����ͷ��������󳤶�                                                                                        
//==========================================================================================
//				������س�������                                            
//==========================================================================================
                                                                                            
////////////////////////////// ����ID ////////////////////////////                          
const int    AID_MT_BEGIN        =   AID_MT_BGN;
const int    AID_MT_AGENT	     =   AID_MT_BEGIN + 1;    	//�ն˴���                          
const int    AID_MT_SERVICE      =   AID_MT_BEGIN + 2;        //�ն�ҵ��Ӧ��ID                      
const int    AID_MT_GUARD	     = 	 AID_MT_BEGIN + 3;    	//�ն�����                          
const int    AID_MT_UE		     =	 AID_MT_BEGIN + 4;    	//ҵ��ӿ�ʵ��                      
const int    AID_MT_STACKIN      =	 AID_MT_BEGIN + 5;    	//H323 Э��ջ����                   
const int    AID_MT_STACKOUT	 =	 AID_MT_BEGIN + 6;    	//H323 Э��ջ���                   
const int    AID_MT_RCP		     =	 AID_MT_BEGIN + 7;    	//MT RCP                            
const int    AID_MT_H320STACK    =   AID_MT_BEGIN + 8;        //H320 Э��ջ����Ӧ��                    
const int    AID_MT_RCSIM        =	 AID_MT_BEGIN + 9;        //ң������ģ��ģ��                  
const int    AID_MT_MC		     =	 AID_MT_BEGIN + 10;    	//MC                                
const int    AID_MT_SNMP	     =	 AID_MT_BEGIN + 11;        //SNMP                              
const int    AID_MT_CONFIG       =   AID_MT_BEGIN + 12;        //�����ļ�����Ӧ��ID                  
const int    AID_MT_UI	         =	 AID_MT_BEGIN + 13;        //�ն˽���Ӧ��                
const int    AID_MT_DEVCTRL	     =	 AID_MT_BEGIN + 14;    	//�豸����                          
const int    AID_MT_CODEC	     =	 AID_MT_BEGIN + 15;    	//�����                            
//const int    AID_ADDRBOOK	     =	 AID_MT_BEGIN + 16;    	//��ַ��                            
const int    AID_MTCONSOLE	     =	 AID_MT_BEGIN + 17;    	//�ն˿���̨ 
const int    AID_MT_SYS          =   AID_MT_BEGIN + 18;        //�ն�ϵͳӦ��ID                  
const int    AID_MT_H323SERVICE  =   AID_MT_BEGIN + 20;        //�ն�H323ҵ��Ӧ��ID                  
const int    AID_MT_H320SERVICE  =   AID_MT_BEGIN + 21;        //�ն�H320ҵ��Ӧ��ID                  
const int    AID_MT_SIPSERVICE   =   AID_MT_BEGIN + 22;        //�ն�SIPҵ��Ӧ��ID                                                                                            
const int    AID_MT_DVB          =   AID_MT_BEGIN + 23 ;        //�ն�˫����ҵ��Ӧ��ID
const int    AID_MT_DVBSSN       =   AID_MT_BEGIN + 24;        //˫���лỰӦ��
const int    AID_MT_VODCLIENT    =   AID_MT_BEGIN + 25;         //VOD�ͻ���Ӧ��ID
const int    AID_MT_PCDUALVIDEO  =   AID_MT_BEGIN + 26;         //pc˫�����Ӧ��ID
const int    AID_MT_UPGRADE      =   AID_MT_BEGIN + 27;         //�Զ�����Ӧ��ID
const int    AID_MT_BATCHCONFIG  =   AID_MT_BEGIN + 28;         //�����ļ�������Ӧ��ID
const int    AID_MT_AUTOTEST     =   AID_MT_BEGIN + 29;         //�ն�Ӳ���Զ�������Ӧ��ID   
const int    AID_MT_AUTOTESTCLIENT = AID_MT_BEGIN + 30;         // add by wangliang 2007-03-02
const int    AID_MT_REMOTESIM    =   AID_MT_BEGIN + 31;         // add by wangliang 2007-03-02
const int    AID_MT_THREADDETECT =   AID_MT_BEGIN + 32;         // add by qianshufeng 2008-01-07
//End

#define AID_MT_CTRL     AID_MT_SERVICE

#define AID_ADDRBOOK    46
////////////////////////////// �������ȼ� ////////////////////////////                      
                                                                                            
	                                                                                    
//==========================================================================================
//			     ������س�������                                               
//==========================================================================================
                                                                                            
////////////////////////////// ��Ϣ�ṹ���� ////////////////////////////                    
                                                                                         
                                                                                            
#define     MT_MAX_CONF_ID_LEN 			16        //����ID����                         
#define     MT_MAX_CONF_NAME_LEN		64	      //�������ֳ���                       
#define     MT_MAX_CONF_E164_LEN	 	16	      //������볤��                      
#define     MT_CONF_CAPACITY			192		  //�����ն�����      
#define     MT_MAXNUM_INVITEMT			32        //�����������������ն���
#define		MT_MAX_CONF_NUM				16		  //��������

#define     MT_MAXLEN_ENCRYPTKEY    64 

const int   MT_MAXLEN_MESSAGE       =   1024*16;  //�ն���Ϣ��󳤶�16K                                                                                            

#define     MT_MAXLEN_CONFGUID          16		  //����guid��󳤶�
#define     MT_MAXLEN_PASSWORD          32		  //������󳤶�
#define     MT_NAME_LEN                 16		  //�ն˱�������                      
//#define		CTRL_TERLABEL_UPPER		193			//��Ч���ն˱��    
//#define     CTRL_TERMID_LEN			128			//�����ն�ID����


// ��־����
#define		MT_LOG_ERROR		0
#define		MT_LOG_VITAL		1
#define		MT_LOG_WARNING		10
#define		MT_LOG_HINT			100
#define		MT_LOG_DETAIL		200
#define		MT_LOG_MAX			255

//����mtmp������
//[xujinxing-2006-05-15]

#define   MTMP_NO_ERROR     (u16)0
#define   MTMP_ERROR_BASE   (u16)5000
#define   ERROR_DECODER_SNAPSHOT_FAIL_H261  (u16)(MTMP_ERROR_BASE + 1)
#define   ERROR_ENCODER_SNAPSHOT_FAIL_H261  (u16)(MTMP_ERROR_BASE + 2)

//�����mc_license��صĺ� 
//[xujinxing-2006-10-13]

// License����
#define MT_KEY_FILENAME                "kedalicense.key" // "mt.key"
#define MT_KEDA_AES_KEY				  "kedacom200610101"
#define MT_KEDA_COPYRIGHT			  "Suzhou Keda Technology Co.,ltd. Tel:86-512-68418188, Fax: 86-512-68412699, http://www.kedacom.com"
// �������ݵĳ���
#define MT_LEN_KEYFILE_CONT              (u16)512

//add by xujinxing, telnet authority
#define MT_TEL_USRNAME  "admin"
#define MT_TEL_PWD      "admin"

#define MT_TELNET_PWD_MAX_LEN 32

/************************************************************************/
/* �ն�Ӳ���Զ������� wl 2007/02/06
/************************************************************************/

// �Զ�����������汾��
#define MT_AUTOTEST_VER 0x0100

// ����ʧ��ԭ��
#define AUTOTEST_NACK_REASON_OK          0  //���Խ���ɹ�
#define AUTOTEST_NACK_REASON_HDERR       1  //Ӳ������ʧ��
#define AUTOTEST_NACK_REASON_TESTING     2  //�������ڽ���
#define AUTOTEST_NACK_REASON_UNSUPPORT   3  //Ӳ����֧��
#define AUTOTEST_NACK_REASON_STATUSERROR 4  //�ն�״̬����
#define AUTOTEST_NACK_REASON_VERERROR    5  //�汾�Ŵ���

/* �Զ������Խӿں��������������� */

// VLYNQ���ܺ�������ϲ���: ����Ƶ��+����״̬+���Է���
const u8 AUTOTEST_VLYNQFUNC_FREQ_76 = 0;  // VLYNQ����Ƶ��: 76MHZ
const u8 AUTOTEST_VLYNQFUNC_FREQ_99 = 1;  // VLYNQ����Ƶ��: 99MHZ 

const u8 AUTOTEST_VLYNQFUNC_STATE_0 = 0;  // VLYNQ����״̬: ��ģʽ
const u8 AUTOTEST_VLYNQFUNC_STATE_1 = 1;  // VLYNQ����״̬: ��ģʽ

const u8 AUTOTEST_VLYNQFUNC_MODE_1  = 1;  // VLYNQ���Է���: ��Է�DSP��ַд��������
const u8 AUTOTEST_VLYNQFUNC_MODE_2  = 2;  // VLYNQ���Է���: ��Է�DSP��ַд�ݼ�����
const u8 AUTOTEST_VLYNQFUNC_MODE_3  = 3;  // VLYNQ���Է���: ���Լ�DSP��ַд��������
const u8 AUTOTEST_VLYNQFUNC_MODE_4  = 4;  // VLYNQ���Է���: ���Լ�DSP��ַд�ݼ�����
const u8 AUTOTEST_VLYNQFUNC_MODE_5  = 5;  // VLYNQ���Է���: ���Լ�DSP��ַ����������
const u8 AUTOTEST_VLYNQFUNC_MODE_6  = 6;  // VLYNQ���Է���: ���Լ�DSP��ַ���ݼ�����
const u8 AUTOTEST_VLYNQFUNC_MODE_7  = 7;  // VLYNQ���Է���: ��Է�DSP��ַ����������
const u8 AUTOTEST_VLYNQFUNC_MODE_8  = 8;  // VLYNQ���Է���: ��Է�DSP��ַ���ݼ�����

// EPLD�Ĵ�����д��ϲ���:  epld��������
const u8 AUTOTEST_EPLDFUNC_CMD_WRITECMD = 0;
const u8 AUTOTEST_EPLDFUNC_CMD_READCMD  = 1;

// ��Ƶ�Ի���ϲ���: ������+��Ƶ���ݸ�ʽ
// ������ʹ��ʱ��Ҫʹ��ʵ��ֵ
const u8 AUTOTEST_AUDIOFUNC_SAMPLERATE_8000  = 0; // ������: 8000
const u8 AUTOTEST_AUDIOFUNC_SAMPLERATE_32000 = 1; // ������: 32000
const u8 AUTOTEST_AUDIOFUNC_SAMPLERATE_44100 = 2; // ������: 44100
const u8 AUTOTEST_AUDIOFUNC_SAMPLERATE_48000 = 3; // ������: 48000

const u8 AUTOTEST_AUDIOFUNC_FORMAT_16 = 0; // ��Ƶ���ݸ�ʽ: S16_LE
const u8 AUTOTEST_AUDIOFUNC_FORMAT_32 = 1; // ��Ƶ���ݸ�ʽ: S32_LE

// ��Ƶ�Ի���ϲ���: �����ʽ+OSD��ʾ+���л���ʾ

const u8 AUTOTEST_VIDEOFUNC_OUTPUT_0  = 0;  // �����ʽ: D1
const u8 AUTOTEST_VIDEOFUNC_OUTPUT_1  = 1;  // �����ʽ: CIF 
const u8 AUTOTEST_VIDEOFUNC_OUTPUT_2  = 2;  // �����ʽ: ZOOM 
const u8 AUTOTEST_VIDEOFUNC_OUTPUT_3  = 3;  // �����ʽ: VGA60 
const u8 AUTOTEST_VIDEOFUNC_OUTPUT_4  = 4;  // �����ʽ: VGA75 
const u8 AUTOTEST_VIDEOFUNC_OUTPUT_5  = 5;  // �����ʽ: VGA85
const u8 AUTOTEST_VIDEOFUNC_OUTPUT_6  = 6;  // �����ʽ: SVGA60 
const u8 AUTOTEST_VIDEOFUNC_OUTPUT_7  = 7;  // �����ʽ: SVGA75
const u8 AUTOTEST_VIDEOFUNC_OUTPUT_8  = 8;  // �����ʽ: SVGA85
const u8 AUTOTEST_VIDEOFUNC_OUTPUT_9  = 9;  // �����ʽ: XGA60 
const u8 AUTOTEST_VIDEOFUNC_OUTPUT_10 = 10; // �����ʽ: XGA75

const u8 AUTOTEST_VIDEOFUNC_OSD_0 = 0; // OSD��ʾ: ��֧��
const u8 AUTOTEST_VIDEOFUNC_OSD_1 = 1; // OSD��ʾ: ֧��

const u8 AUTOTEST_VIDEOFUNC_PIP_0 = 0; // ���л���ʾ: ��֧��
const u8 AUTOTEST_VIDEOFUNC_PIP_1 = 1; // ���л���ʾ: ֧��

//////////////////////////////////////////////////////////////////////
/*HD Macro definition BEGIN*/
#define HD_PATCH_VER_A              0
#define HD_PATCH_VER_B              1

#define HD_SCREENMODE_SIMPLE        0
#define HD_SCREENMODE_NORMAL        1
#define HD_SCREENMODE_ADVANCE       2


/*WIFI Macro definition BEGIN*/
#define WIFI_MAX_BSS_NUM            16
#define WIFI_MAX_KEY_LEN            64
#define WIFI_MAX_NAME_LEN           64

#define WIFI_MAX_CFG_NUM            16

#define WIFI_NET_TYPE_INFRA          0
#define WIFI_NET_TYPE_ADHOC          1

#define WIFI_AUTH_TYPE_OPEN          0
#define WIFI_AUTH_TYPE_SHARE         1
#define WIFI_AUTH_TYPE_WPA_PSK       2
#define WIFI_AUTH_TYPE_WPA2_PSK      3

#define WIFI_ENCRYTO_TYPE_WEP        0
#define WIFI_ENCRYTO_TYPE_AES        1
#define WIFI_ENCRYTO_TYPE_TKIP       2
/*WIFI Macro definition END*/
#endif //!_MT_CONST_H_


