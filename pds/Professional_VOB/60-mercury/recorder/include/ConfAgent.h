/*****************************************************************************
   ģ����      : Recorder
   �ļ���      : ConfAgent.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: TCfgAgent�ඨ��
   ����        : 
   �汾        : V3.5  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
******************************************************************************/
#ifndef CONFAGENT_H
#define CONFAGENT_H

#include "osp.h"
#include "kdvtype.h"
#include "kdvsys.h"
#include "mcuconst.h"
#include "mcustruct.h"
#include "afxtempl.h"
#include "rpctrl.h"
#include "dboperate.h"

//�����ļ���غ궨��
#define RECCFG_FILE						( LPCSTR )".\\conf\\reccfg.ini"
#define RECDBGCFG_FILE					( LPCSTR )".\\conf\\recDebugCfg.ini"

#define SECTION_RECSYS   				( LPCSTR )"RecorderSystem"
#define KEY_RECSYS_PERIEQPID			( LPCSTR )"Id"
#define KEY_RECSYS_PERIEQPTYPE		    ( LPCSTR )"Type"
#define KEY_RECSYS_PERIEQPALIAS		    ( LPCSTR )"Alias"
#define KEY_RECSYS_IPADDR			    ( LPCSTR )"IpAddr"
#define KEY_RECSYS_RCVPORT		        ( LPCSTR )"RecorderRcvStartPort"
#define KEY_RECSYS_PLAYPORT		        ( LPCSTR )"RecorderPlayStartPort"
#define KEY_RECSYS_RECFILEPATH		    ( LPCSTR )"RecFilePath"
#define KEY_RECSYS_MCUID				( LPCSTR )"McuId"
#define KEY_RECSYS_MCUIPADDR			( LPCSTR )"McuIpAddr"
#define KEY_RECSYS_MCUCONNPORT		    ( LPCSTR )"McuConnPort"
#define KEY_RECSYS_MCUIDB				( LPCSTR )"McuIdB"
#define KEY_RECSYS_MCUIPADDRB			( LPCSTR )"McuIpAddrB"
#define KEY_RECSYS_MCUCONNPORTB		    ( LPCSTR )"McuConnPortB"
#define KEY_RECSYS_RECCHNUM             ( LPCSTR )"RecordChannels"
#define KEY_RECSYS_PLAYCHNUM            ( LPCSTR )"PlayChannels"
#define KEY_RECSYS_TELNETPORT           ( LPCSTR )"RecorderTelnetPort"
#define KEY_RECSYS_MACHINENAME          ( LPCSTR )"MachineName"

#define KEY_RECSYS_ISPUBLISH            ( LPCSTR )"IsPublish"
#define KEY_RECSYS_DBHOSTIP             ( LPCSTR )"DBHostIP"
#define KEY_RECSYS_USERNAME             ( LPCSTR )"DBUserName"
#define KEY_RECSYS_PASSWORD             ( LPCSTR )"DBPassWord"
#define KEY_RECSYS_VODMMSPATH           ( LPCSTR )"VodMMSPath"
#define KEY_RECSYS_MMSUSERNAME          ( LPCSTR )"MMSUserName"
#define KEY_RECSYS_MMSPASSWORD          ( LPCSTR )"MMSPassWord"

#define KEY_RECSYS_MULTIINSTANCE        ( LPCSTR )"MultiInstance"
#define	SECTION_RECORDERDEBUGINFO		(LPCSTR)"RecorderDebugInfo"

enum EMNotifyType
{
	emRECORDEROK		= 1,            // ¼��״̬�ı�
	emRECORDER_FULLDISK	= 2,            // ��������50M��
    emRECORDER_SMALL_FREESPACE  = 3,    // ���пռ䲻����200M��
	emRECORDER_WRITE_FILE_ERR = 4,		// rpctrlд�ļ�����, ¼��ֹͣ

	emPLAYEROK       	= 1000,	        // ����״̬�ı� 
	emPLAYEREMPTYFILE	= 1001,         // ���ļ�, �쳣
	emPLAYFINISHTASK	= 1002,         // �������
	emPLAYERPROCESS  	= 1003,         // �������
};

extern u32 g_adwDevice[MAXNUM_RECORDER_CHNNL+1];

#define NOT_FIND_RECID			(u32)(-1)			// û���ҵ��豸��
#define LANGUAGE_CHINESE        MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED)

typedef struct tagNotify{
    u16 wChnIndex;  
    u16 wRecId;
    u16 wAlarmType;
    u32 dwPlayTime;
    tagNotify()
    {
        wAlarmType = 0;
        wChnIndex  = 0;
        wRecId = 0;
        dwPlayTime = 0;
    };
}TNotifyInfo;

u16 GetRecChnIndex( u32 dwRecId );

class TCfgAgent
{
public:
    TCfgAgent()
    {
        Reset();
    }
    void Reset()
    {
        memset(this, 0, sizeof(TCfgAgent));
    }

public:
	u32  m_dwRecIpAddr;                  //¼��ͨ��IP��ַ
	s8   m_achLocalIP[32];
	u16  m_wRecStartPort;                //¼��ͨ����ʼ�˿ں�
	u16  m_wPlayStartPort;               //����ͨ����ʼ�˿ں� //add play start port
	u8   m_byEqpId;                      //����ID
	u8   m_byEqpType;                    //��������
	
	u32  m_dwMcuIpAddr;                  //MCU IP��ַ
	u16  m_wMcuConnPort;                 //MCU ͨ�Ŷ˿ں�
	u16  m_wMcuId;                       //MCU ID��

    u32  m_dwMcuIpAddrB;                  // Mcu Ip
    u16  m_wMcuConnPortB;                 // Mcu port
    u16  m_wMcuIdB;                       // mcu id

	u8   m_byPlayChnNum;                 //����ͨ������
	u8   m_byRecChnNum;                  //¼��ͨ������
	u8   m_byTotalChnNum;                //��ͨ����
	s8   m_szAlias[MAXLEN_ALIAS];        //����
	s8   m_szRecordPath[MAX_FILE_PATH_LEN]; //¼����ļ�����·��
	s8   m_szFtpPath[MAX_FILE_PATH_LEN];    //�ļ�����FTP·��
    s8   m_szMachineName[32];            // ������
    
	BOOL m_bCppUnitTest;                 //�Ƿ����
	u32  m_dwCppUnitIpAddr;              //����IP��ַ
	u16  m_wCppUnitPort;                 //���Զ˿�
	u16  m_wTelnetPort;                  //Telnet ���ԶϿ�

	BOOL m_bMediaServer;                 //�Ƿ�����ý�������
	s8   m_achMSInfo[256];               //ý���������Ϣ    
	
	u32  m_dwDBHostIP;                   //�������ݿ�IP��ַ
	s8   m_achDBHostIP[32];
	
	s8   m_achDBUsername[32];
	s8   m_achDBPassword[32];

	s8   m_achVodFtpPatch[128];
	
	s8   m_achFTPUsername[32];
	s8   m_achFTPPassword[32];

    BOOL m_bMultiInst;                   //�Ƿ����������
    
    BOOL32 m_bInited;
    s8     m_szCfgFilename[KDV_MAX_PATH];
	
	// ����Ӣ�İ������ļ�, zgc, 2007-03-23
	LANGID  m_tLangId;  // language id
    BOOL    m_bOSEn;  // �Ƿ�Ӣ�İ�
    
    BOOL    m_bOpenDBSucceed;           // ���ݿ��Ƿ�򿪳ɹ�

	// [pengjie 2010/9/26] ����¼��������¼���ļ���С������������ļ���
	u32 m_dwMaxRecFileLen;

	//2012/[6/11 zhangli] ��֡ʱ�Ƿ�����ؼ�֡����֡ʱ�Ƿ�д�ļ� 
	BOOL32 m_bIsWriteAsfWhenLoseFrame;
	BOOL32 m_bIsNeedKeyFrameWhenLoseFrame;
	u8		m_byAudioBuffSize;	//��Ƶbuff��С(Ĭ��Ϊ16֡��ÿ֡2048�ֽ�)
	u8		m_byVideoBuffSize;	//��Ƶbuff��С(Ĭ��Ϊ25֡��ÿ֡524288�ֽ�)
	u8		m_byDsBuffSize;		//˫��buff��С(Ĭ��Ϊ16֡��ÿ֡524288�ֽ�)
	u8		m_byLeftBuffSize;	//buff���ʣ��֡��(��ζ��ÿ�ζ�buffʱ�����ʣ��֡������10���򷴸�����ֱ��ʣ��Ϊ10֡)
	
	//��¼�����ļ���֡���ݴ�С
	TFrameSizeInfo	m_atFrameSize[MAX_AUDIO_TYPE_NUM];

	//ֱ���ļ�����С��С������һ��¼��һ��ֱ�����ļ���Ĭ��512K
	u16 m_wMinSizeForBroadcast;
	// End

public:
    BOOL32 IsInited()
    {
        return m_bInited;
    }

    void SetCfgFilename(LPCSTR lpszName)
    {
        memset( m_szCfgFilename, 0, sizeof(m_szCfgFilename));
        strncpy( m_szCfgFilename, lpszName, KDV_MAX_PATH - 1 );
    }
	BOOL ReadConfigFromFile();
    BOOL WriteConfigIntoFile();
    BOOL32 IsLegalityPort(u16 wPort1, u16 wPort2);
	BOOL IsLegality( s32 nNum );	// �жϲ����Ƿ�Ϸ�
	BOOL IsLegality( s8* aChar);
	void ErrorMsg(int nNum);		// ������Ϣ nNum: ������
	void RecLog(char * fmt,...);

	//����֡��������Ϣ
	BOOL32 SetFrameSizeInfo(TFrameSizeInfo &tFrameSizeInfo);
	//ȡ֡��������Ϣ
	TFrameSizeInfo* GetFrameSizeInfo()
	{
		return m_atFrameSize;
	};
};

extern TCfgAgent g_tCfg;

#endif//! CONFAGENT_H
