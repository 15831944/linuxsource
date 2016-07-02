/*****************************************************************************
   ģ����      : Recorder
   �ļ���      : chnInst.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: Recorder channel���ඨ��
   ����        : 
   �汾        : V3.5  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/07/04  3.0         ������      ������¼�����ļ��ı���
   2006/03/21  4.0         �ű���      �����Ż�
******************************************************************************/
#ifndef CHNINST_H
#define CHNINST_H

#include "stdafx.h"

#pragma warning( disable : 4706 )
#include "osp.h"
#include "ConfAgent.h"
#include "RPCtrl.h"
#include "eqpcfg.h"
#include "loguserdef.h"
#include "kdvlog.h"
//#import "WMSServerTypeLib.dll" no_namespace named_guids raw_interfaces_only
#include "wmsserver.h"

#include <vector>
 using namespace std;

#pragma once

#define REC_PLAY_RSTIME			    (u16)500        // ����ʱ�ķ��ͻ���ʱ�䳤��ms�����ڶ����ش�
#define REC_CONNECTTOA			    (u8)1	        // connect to MCU.A
#define REC_CONNECTTOB			    (u8)2	        // connect to MCU.B
#define REC_CONNECTTOAB			    (u8)3	        // connect to MCU.A && MCU.B
#define REC_CONNECT_TIMEOUT		    (u16)3 * 1000
#define REC_REGISTER_TIMEOUT	    (u16)4 * 1000

#define RECPROG_PERIOD				(u16)(5*1000)	// ��ʱ֪ͨ¼�����
#define RECSTATUS_NOTIPERIOD		(u32)(5*60*1000)   // ������� 5min ����һ��¼���״̬�ϱ�
#define RECSTATUS_CRINOTIPERIOD		(u16)(5*1000)      // ������� 5sec ����һ��¼���״̬�ϱ�
#define REC_PLAYLIST_TIMER			(u16)(60*1*1000)
#define PLAY_EMPTY_FILE				(u8)254			// ���ſ��ļ�
#define FINISH_PLAY					(u8)100			// �������
#define PLAY_RATE_GRAN              (u8)5           // ����ʱ������(s)
#define RENAME_OK					(u8)0			// �����ɹ�
#define VIDEO_TWO					(u8)2			// ��·��Ƶ
#define FIRST_REGISTERACK			(u8)1			// ��һ���յ�ע����Ϣ
#define CHECK_IFRAME_INTERVAL       (u16)3000       // ���ؼ�֡ʱ����(ms)

#define DISCSPACE_LEVEL1            200             // ���̿ռ�澯
#define DISCSPACE_LEVEL2            50
#define CHECK_PUBLISHPOINT_INTERVAL (u16)3000        // ��ⷢ����ʱ����(ms)

#define MAX_TCNAME_LEN				(u8)(32)		//

extern CDBOperate  g_cTDBOperate;

struct TConnectParam
{
    u32  m_dwIpAddr;			// ����Ip
    u16  m_wPort;				// ���Ӷ˿�
    u16  m_wConnectContext;		// ����������
};

class CChnInst : public CInstance
{
public:
	CChnInst();
	~CChnInst();	
public:
	enum EDAEMONSTATE
    {
		STATE_IDLE,
		STATE_NORMAL
	};   
private:
	u32  m_dwDevice;			    	// �豸���
	u32  m_dwPlayIpAddr;				// ����ͨ������IP��ַ       
    u16  m_wPlayPort;					// ����ͨ��������ʼ�˿ں� 
	u32  m_dwRecIpAddr;					// ¼��ͨ��¼��IP��ַ        
    u16  m_wRecPort;					// ¼��ͨ��¼����ʼ�˿ں�    
	u8   m_byChnIdx ;			        // ͨ��������,¼�񡢷���ͨ���ֱ��0��ʼ
	u16  m_wLocalPort;			        // ��������ڰ󶨵Ķ˿�
	s8   m_achChnInfo[40];              // ͨ����Ϣ
	TMt  m_tSrvTmt;				        // ¼���TMT
	u8   m_byPublishMode;		        // ����¼������� ������������ʽ
	u8   m_byPublishLevel;		        // ����¼������� ��������������

	u8   m_byIsNeedPrs;                 //[������][2010/11/04]
	u8   m_byIsDstream;
	
    TRecChnnlStatus m_tChnnlStatus ;	// ͨ��״��
    CConfId   m_cConfId;    	        // �����

	// �޸ķ���������, zgc, 07/02/14
	TMediaEncrypt m_tVideoEncrypt;		// ��Ƶ���ܲ���
	TMediaEncrypt m_tAudioEncrypt;		// ��Ƶ���ܲ���
	TMediaInfo    m_tMediaInfo;			// ý����Ϣ
    TCapSupportEx m_tCapSupportEx;      // ��չ������Ϣ��Ŀǰֻ����FEC��
	TCapSupport   m_tConfCapSupport;    //�������������Ϣ
	BOOL32 m_bNeedCapChk;               //�����Ƿ�����������Ϣ����,�������Ļ���Ҫ���������Ϣ
	u8      m_byConfPlaySrcSsnId;           //�������ʱ�������SrcSsnId����autoplaynext�ã���֯��Ϣ��
	u8		m_byAudioTrackNum;
	u8		m_byAudioMediaType;
	TAudioTypeDesc m_patAudDesc[MAXNUM_CONF_AUDIOTYPE];
	u8 m_byAudNumber;

	u8		m_byConfVideoMediaType;
	u8		m_byConfSecVidMediaType;

	BOOL32 m_bFileFirstVidHp;
	BOOL32 m_bFileSecVidHp;
	u8 m_byFileAudioTrackNum;

	u8 m_byVideoNum;					// ��Ƶͨ������Ƶ�ź�·��

    s8 m_achFileName[MAX_FILE_NAME_LEN];  // ��ǰ¼�����ļ�������ʽΪ abc_255_384K.asf 

	s8 m_achPlayListName[MAX_FILE_NAME_LEN];  // ��ǰ¼�����ļ�������ʽΪ abc_255_384K.asf 

	s8 m_achOrignalFileName[MAX_FILE_NAME_LEN]; // �ļ�ȫ·��ԭʼ��     //add by jlb 081027
	s8 m_achFullName[MAX_FILE_NAME_LEN];  // �ļ�ȫ�ļ�������ʽΪ E:\vod\mcuname/abc_255_384K.asf 
	s8 m_achOrignalFullName[MAX_FILE_NAME_LEN]; // �ļ�ȫ·��ԭʼ��     //add by jlb 081027
	BOOL32 m_bIsNodeAConnect;
	BOOL32 m_bIsNodeBConnect;

	//[2013/9/5 zhangli]¼����� 
	TMediaEncrypt m_ptMediaEncrypt[MAX_STREAM_NUM];
	TDoublePayload m_ptDoublePayload[MAX_STREAM_NUM];
	u8 m_byAudIndex;
	u8 m_byDsIndex;
	u8 m_byVidIndex;

protected:
	BOOL32	InitRecorder( );
	// [8/25/2010 liuxu] Ϊ��ʼ��¼��ͨ������InitalData�������pMsg��pcApp�Ĳ�����ԭ�Ͳ�������
    BOOL32	InitalData(CMessage * const pMsg = NULL, CApp* pcApp = NULL);
	void	DaemonInstanceEntry( CMessage * const pMsg, CApp* pcApp);
	void	InstanceEntry( CMessage * const pcMsg );
    void	ProcConnectTimeOut(BOOL32 bIsConnectA);
	BOOL32	ConnectMcu( u32 dwMcuIp, u16 wMcuPort, u32& dwMcuNode );
    void	ProcRegisterTimeOut( CMessage* const pMsg, BOOL32 bIsRegisterA );
    void	Register( u32 dwMcuNode, BOOL32 bIsRegisterA );
	void	Disconnect( CMessage * const pcMsg );

	LPCSTR GetCurChnInfoStr();    //��õ�ǰͨ����Ϣ(�ַ�������)

    void MsgReNameFile( CMessage * const pcMsg );

	// << DAEMON ������Ϣ����>>
	void MsgRegAckProc( CMessage* const pMsg, CApp* pcApp );
	void MsgRegNackProc( CMessage * const pcMsg );
	void MsgChnInitProc( CMessage * const pcMsg );
	void MsgListAllRecordProc( CMessage * const pcMsg );
	void MsgGetRecStateProc(CMessage* const pMsg, CApp* pcApp);
	void MsgGetRecChnStatusProc( CMessage* const pMsg, CApp* pcApp );
	void MsgGetPlayChnStatusProc( CMessage* const pMsg, CApp* pcApp );
	void MsgDeleteRecordProc( CMessage * const pcMsg );

	void MsgDaemonPublishFileReq( CMessage * const pcMsg );
	void MsgDaemonCancelPublishFileReq( CMessage * const pcMsg );
    void MsgDaemonGetMsStatusRsp( CMessage * const pcMsg );

	void DaemonProcReconnectBCmd( CMessage * const pcMsg ); //MCU ֪ͨ�Ự ����MPCB ��Ӧʵ��, zgc, 2007/04/30

	//<< ¼��ͨ����Ϣ������ >>
	void MsgStartRecordProc( CMessage * const pcMsg );
	void MsgPauseRecordProc( CMessage * const pcMsg );
	void MsgResumeRecordProc( CMessage * const pcMsg );
	void MsgStopRecordProc( CMessage * const pcMsg );
	void SendAddVidFileNotify(void);
	void MsgChangeRecModeProc( CMessage * const pcMsg );

	//<< ����ͨ����Ϣ������ >>
	void MsgStartPlayProc( CMessage * const pcMsg );
	void MsgPausePlayProc( CMessage * const pcMsg );
	void MsgResumePlayProc( CMessage * const pcMsg );
	void MsgFBPlayProc( CMessage * const pcMsg );
	void MsgFFPlayProc( CMessage * const pcMsg );
	void MsgStopPlayProc( CMessage * const pcMsg );
	void MsgSeekPlayProc( CMessage * const pcMsg );
    void MsgForceStopProc( CMessage * const pcMsg );
	void MsgSwitchStartNotifProc( CMessage * const pcMsg, BOOL32 bSwitchedFile = FALSE );
	void SwitchStartWaittimerOver( CMessage * const pcMsg );
	BOOL AutoPlayNextSwitchFile( void );     //�Զ�������һ���л��ļ� add by jlb 081027
    BOOL GetNextSwitchFile(LPCSTR lpszCurFile, s8 *pszNextFile);   //�õ���һ��Ҫ���ŵ��л��ļ� add by jlb 081027

	//��MCU����һ���ؼ�֡
	void  ProcTimerNeedIFrame( const CMessage *pMsg );                                  //Hdu����Mcu���ؼ�֡

    // <<¼���豸֪ͨ>>
	void MsgDeviceNotifyProc(CMessage *const pMsg);
	void SendChnNotify();
	void SendRecProgNotify(BOOL32 bSetTimer = TRUE);
	u8   SendPlayProgNotify();
    void MsgRefreshCurChnRecFileName(CMessage * const pMsg);
	void MsgResetRtcpParam(CMessage * const pMsg);
	void MsgResetRcvParam(CMessage * const pMsg);

	void   SendRecStatusNotify( CApp* pcApp);
	void   StopAllChannel(BOOL32 bIncludePlayChnl);
	void   ReleaseChannel();
	BOOL32 SendMsgToMcu(u16 wEvent, CServMsg * const pcServMsg);	
	void   StatusShow( CApp* const pcApp);	
	u8     GetActivePayload(u8 byRealPayloadType, u8 byEncrpytMode = CONF_ENCRYPTMODE_NONE, BOOL32 bDStream = FALSE);
	void   MsgCppUnitGetEqpStatus(CMessage * const pMsg, CApp * pcApp);
    void   RestartRecorderProc();
    BOOL32 IsExistFile( s8* pchFileName );                      // �ļ��Ƿ����    

	u16 CheckFileCanPlay(s8 *pFileFullName);
    
    // ����
    u16    GetRecordCount(vector<CString> &vtDirName);    // ͳ�Ƹ�MCU��¼�������

    /* guzh [9/7/2006]
    void   SendRecordList(CServMsg &cServMsg, u16 dwTotalCount, u16 dwStartIdx, TListInfo& tListInfo, 
                          BOOL32 bConfReq = FALSE,
                          LPCSTR lpszConfName = NULL) ;
    */

    void   SendRecordList(CServMsg &cServMsg, vector<CString> &vtFiles) ;

	// void   InitPlayList();	
	// void   DelFileTapeList( s8* DelFileName );
	// BOOL32 ComparePlayList( TListInfo tTapeInfoList );          // �Ƚ�
	// void   MaintenanceList( TRecFileListNotify RecNotify );     // ά��
	// void   MsgMaintainPlayList();                               // ά����Ϣ����
	// void   IndexTapeList( TListInfo& tTapeInfoList );	        // ����
    // void   CancelAllPublicFile();
    // void   PrintPlayList();
    /*void   MsgAjustBackRtcp(CMessage* const pMsg);*/   //����BackRtcp[liu lijiu][20100727]
	BOOL32 IsPublicFile( s8* achFileName );
	
    BOOL32 GetSpaceInfo( s32& nFreeSpace, s32& nTotalSpace );
    void   ProcSetQosInfo( CMessage * const pcMsg );                // ����Qos��Ϣ
    void   ComplexQos( u8& byValue, u8 byPrior );
    u32    GetRPBitRate( u16 wConfBitRate, u16 wPayLoadType );      //��������ת��rp�������   
    void   Reclog( s8* pszFmt, ... );

    // ý����Ϣ�洢�ṹת��, zgc, 2008-08-02
    void   MediaInfoRec2Mcu( TMediaInfo tMediaInfo, TPlayFileMediaInfo &tPlayFileMediaInfo );

	//[2012/4/13 zhangli]�������ݿ�����ӿڣ��������ʧ�ܣ��������ݿ��ٴβ����������Ȼʧ���򷵻�
	//¼�������ʱ�������ݿ⣬��;�п������ݿ����Ӷϵ���¼������DB�඼����֪�����²���ʧ��
	u16 PublishOneFile(s8 *szFullFileName, BOOL32 isWriteFileInfo = TRUE);
	u16 UpdatePublishOneFile(char *szFullFileName);
	u16 AddOneLiveSource();
	u16 DeleteOneLiveSource();

	BOOL32 InitCOMServer();
	BOOL32 CreateBrdPubPoint();
	void DeleteBrdPubPoint();
	void ProcTimerPpFailed(CMessage * const pcMsg);
	void ProcRecorderQuit(CMessage * const pcMsg);

	BOOL32 CreatePlayList();
	void DeletePlayList();
	void AddToPlayList();

public:			// ¼����������ӿ����
	/** 
	/* ���ܣ��жϴ�ʵ���Ƿ��Ѿ����������ն�
	/* ������ ��
	/* ����ֵ�� True�Ѿ������ϣ�Falseû��������
	*/
    BOOL32 IsMtConnect(){return m_bIsMtConnect;}

	/** 
	/* ���ܣ����ô�ʵ���Ƿ��Ѿ����������ն�
	/* ������ bFlag �� True�Ѿ������ϣ�Falseû��������
	/* ����ֵ�� ��
	*/
    void SetMtConnect(BOOL32 bFlag)
	{
		m_bIsMtConnect = bFlag;

		// [9/28/2010 liuxu] �Զ�����
		if (!m_bIsMtConnect)
		{
			SetMTNodeInfo(0, 0);
		}
	}

	/** 
	/* ���ܣ�Daemonʵ������ʼ¼������
	/* ������ CMessage , CApp
	/* ����ֵ�� ��
	*/
    void OnDaemStartRecReq(CMessage* const pMsg, CApp* pcApp);
	
private:		// ¼����������ӿ����
	
				/** 
				/* ���ܣ�Daemonʵ����Ӧ�ն˵�ע������
	*/
    void OnMtRegReq(CMessage * const pMsg, CApp * pcApp);

	/** 
	/* ���ܣ���ͨ��Ӧ�ն˶����Ĵ���
	*/
    void OnMtDisconnect(CMessage * const pMsg);

	/** 
	/* ���ܣ���¼�ն˵Ľڵ���Ϣ
	/* ������ 
	/*			dwNodeId : �ڵ��
	/*			dwInstId ��ȫ�ֺ�
	/* ����ֵ�� ��
	*/
    void SetMTNodeInfo(u32 dwNodeId, u32 dwInstId);

	/** 
	/* ���ܣ�����Ϣ����¼������������նˣ�
	/* ������
	/*        wEvent    : �¼���
	/*        pcServMsg : �¼����ݣ�CServMsg�ṹ
	/* ����ֵ�� ���ͳɹ�������True������false
	*/
	BOOL32 SendMsgToRecSevr(u16 wEvent, CServMsg * const pcServMsg);

	/** 
	/* ���ܣ�����Ϣ����¼������������նˣ���Daemonʵ��
	/* ������
	/*        wEvent    : �¼���
	/*        pcServMsg : �¼����ݣ�CServMsg�ṹ
	/* ����ֵ�� ���ͳɹ�������True������false
	*/
	BOOL32 SendMsgToRecSevrDaem(u16 wEvent, CServMsg * const pcServMsg);
    
private:

    u32 m_dwMtNodeId;							// �ն˵Ľڵ��
    u32 m_dwMtInstId;							// �ն˵�Instance��
    BOOL32 m_bIsMtConnect;						// �Ƿ����������ն�
	BOOL32 m_bIsMtMsg;							// ��Ϣ�ǲ���Ҫ�����ն�

	IWMSPublishingPoint *m_pPubPoint;
	IWMSBroadcastPublishingPoint *m_pBCPubPoint;
};

class CRecCfg
{
public:
    CRecCfg();
    ~CRecCfg();
public:
    u32    m_dwMcuNode;         // MCU.A �Ľڵ����
    u32    m_dwMcuIId;          // MCU.A APPID��Instance ID
    u32    m_dwMcuNodeB;        // MCU.A �Ľڵ����
    u32    m_dwMcuIIdB;         // MCU.A APPID��Instance ID
    u8     m_byRegAckNum;       // ��һ��ע��ɹ�
	u32    m_dwMpcSSrc;         // MPC�ĻỰ����
    u16    m_wRegTimes;         // ע�ᳬʱ��1��reg to MCU.A; 2-reg to MCU.B; 3-both A and B
    u16    m_wDevCount;         // �豸������
    TEqp   m_tEqp;              // �����������Ϣ

    u16    m_dwMcuStartRcvPort; // mcu �࿪ʼ���ܶ˿�
    
    // ����ʹ�õ��ļ���,ÿ��ʵ��������Ӧ����һӦ�������
    // ����ʵ��1���ڲ������ļ���Ϊ m_achUsingFile[1]
    // ����ʵ��Ϊ�����ļ�����Ӧ������Ϊ��
    s8     m_achUsingFile[MAXNUM_RECORDER_CHNNL+4][MAX_FILE_NAME_LEN];    
    
    TPrsTimeSpan       m_PrsParam;       // �ش�ʱ����
    
    s8      m_szMcuAlias[MAX_ALIAS_LEN]; // ���ӵ�Mcu����
    s8      m_szWorkPath[MAX_FILE_PATH_LEN];    // ���εĹ���··������ E:\vod\McuName
    s8      m_achPublishPath[MAX_FILE_PATH_LEN]; // ����·�� mms://usr:pas@KEDACOM/vod

	// [8/31/2010 liuxu] ����ն˵Ĺ���·��.ÿ���ն˶���һ������·��
	s8		m_szMTWorkPath[MAXNUM_RECORDER_CHNNL][MAX_FILE_PATH_LEN];	// �ն˵Ĺ���·��
public:
    // ���ع���·��
    LPCSTR  GetWorkingPath() ; 

	// [8/31/2010 liuxu] �����ն˵Ĺ���·��
	LPCSTR  GetMtWorkingPath(const u32&) ; 
	void FileTimeToTime_t(FILETIME cFileTime, time_t *pcTime);
    // Ԥ�����ļ�����ƴ����VOD·����MCU������Ϣ
    // lpszFullName [out] ��Ҫ�Լ�Ԥ����ռ�
    // ��� conf1 �� a.asf �滻�� e:\vod\mcuname\conf1/a.asf
    
    u16 PreProcFileame(LPSTR lpszFullName, LPSTR lpszOldName, LPSTR lpszDirname) ;
    
    /**
	/*  Ԥ�����ļ�����ƴ����VOD·����MCU������Ϣ
    /*  lpszFullName [out] ��Ҫ�Լ�Ԥ����ռ�
	/*	��� conf1 �� a.asf �滻�� e:\vod\mcuname\conf1/a.asf
	*/
    u16 PreProcMtFileName(LPSTR lpszFullName, LPSTR lpszOldName, LPSTR lpszDirname, u32 dwMtId) ;

    // guzh [7/17/2007] �����RP����Ҫ��Pattern��(����·����
    void    PreProcPatternName(LPSTR lpszPatternName, 
                               LPCSTR lpszOldNameWithoutDotAsf,
                               u8 byOwnerGrp, u16 wBitrate) ;

    // ������ �������ļ����зָ�Ŀ¼���ļ�����Ҫ���� / ����
    // �� e:\vod\mcuname\conf/abc.asf
    // �ָ�Ϊ e:\vod\mcuname\conf/ �� abc.asf
    // �û��Լ���Ҫ���������� memset 0 ���Ŀռ�
    void SplitPath(LPCSTR lpszFull, LPSTR lpszDir, LPSTR lpszName, BOOL32 bWithSlash) ;

    // ������·���и�ɷ���MMSҪ���·��������
    void Path2MMS(LPCSTR lpszFullName, LPSTR lpszMMS) ;
    
    // ���������ļ�����ȡ���ļ������������ʺ���ID
    BOOL32 GetParamFromFilename(LPCSTR lpszFullname, u8 &byGrpId, u16 &wBitrate, CString& cstrPureName);

	// [8/1/2011 liuxu] �û��Զ������Ƽ���
	u16 CheckPureName( const s8* );

	void GetWHByRes(u8 byRes, u16 &wWidth, u16 &wHeight);
	void GetResByWH(u16 wWidth, u16 wHeight, u8 &byRes);
	
	BOOL32 IsConfHD(const TCapSupport &tCap);

	u16 OpenDB();
};

typedef zTemplate< CChnInst, MAXNUM_RECORDER_CHNNL, CRecCfg > CRecApp;
extern CRecApp g_cRecApp;
extern u32 g_adwRecDuration[MAXNUM_RECORDER_CHNNL+1];

extern IWMSServer *g_pServer;
extern IWMSPublishingPoints *g_pPubPoints;
//¼����˳��ź���
extern SEMHANDLE g_hSemQuit;

#endif //!CHNINST_H

