/*****************************************************************************
   ģ����      : MCU�����ȱ��ݵ�ͨѶ��ʵ�ֻỰģ��
   �ļ���      : msmanagerssn.h
   ����ļ�    : msmanagerssn.cpp
   �ļ�ʵ�ֹ���: MCU�����ȱ��ݵ�ͨѶ��ʵ�ֻỰģ�鶨��
   ����        : ����
   �汾        : V4.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2005/08/18  4.0         ����        ����
******************************************************************************/

#ifndef __MSMANAGERSSN_H_
#define __MSMANAGERSSN_H_

#include "osp.h"
#include "agentinterface.h"
#include "kdvlog.h"


//...............................................................................................

enum emMCUMSType
{ 
	MCU_MSTYPE_INVALID = 0,   //��Ч��
	MCU_MSTYPE_SLAVE,         //��λ��    mc0	
	MCU_MSTYPE_MASTER,        //��λ��    mc1
};

enum emMCUMSState
{ 
	MCU_MSSTATE_OFFLINE = 0,  //����
	MCU_MSSTATE_STANDBY,      //����״̬
	MCU_MSSTATE_ACTIVE,       //����״̬
};

#define CONNECT_MASTERMCU_TIMEOUT   (u32)2000     //���Ӽ�� 2��
#define REGISTER_MASTERMCU_TIMEOUT  (u32)2000     //ע���� 2��

#define MS_CEHCK_MSSTATE_TIMEOUT    (u32)200      //vxworks��ɨ��Ӳ��������״̬�ļ���� 0.2��

#define MS_CHECK_MSSTATE_TIMES      (u8)3         //vxworks��ɨ��Ӳ��������״̬�ĳ���״̬�������,3�����Ͻ����л�

#define MS_INIT_LOCK_SPAN_TIMEOUT   (u32)60000    //����ģ���ʼ��������ʱ��� 60��

#define MS_SYN_LOCK_SPAN_TIMEOUT    (u32)60000    //�������ݵ���ʱ ����ģ����ҵ��ģ��ľ���������ʱ��� 60��

#define MS_WAITFOTRSP_TIMEOUT       (u32)5000     //�ȴ��Զ�����Ӧ��ʱ��� 5��

#define MS_SYN_NOW_SPAN_TIMEOUT     (u32)100      //������������Э�̺󼴿̿�ʼͬ��ʱ���� 0.1��
#define MS_SYN_MIN_SPAN_TIMEOUT     (u32)10000    //���������Զ��������ȫͬ�����ָ��Զ�ʱͬ��ʱ���� 10��

#define MS_DEBUG_CRITICAL           (u8)LOG_LVL_ERROR
#define MS_DEBUG_ERROR              (u8)LOG_LVL_ERROR
#define MS_DEBUG_WARNING            (u8)LOG_LVL_WARNING
#define MS_DEBUG_INFO               (u8)LOG_LVL_KEYSTATUS
#define MS_DEBUG_VERBOSE            (u8)LOG_LVL_DETAIL

//...............................................................................................

//�������ݵ������ͨѶЭ������ݽṹ
#ifdef WIN32
#pragma pack( push )
#pragma pack( 1 )
#else
#endif


#define MS_MAX_PACKET_LEN    (u32)(20*1024)     //��������ʱ��֡�а���ĵ���������Ϣ�峤�ȣ������˳���������зִ��� 20 Kbytes
#define MS_MAX_FRAME_LEN     (u32)(3600*1024)   //��������ʱ��֡�а�ǰ�������Ϣ�峤�� 3600 Kbytes
#define MS_MAX_PACKET_NUM    (u8)(MS_MAX_FRAME_LEN/MS_MAX_PACKET_LEN)


#define MS_MAX_STRING_LEN    255


//����ģ����������
struct TMSStartParam
{
	u8     m_byMcuType;
	u16    m_wMcuListenPort;
	BOOL32 m_bMSDetermineType; //vxworks��Ӳ��ƽ̨֧�������ù��ܣ���ֱ����os��ȡ�����ý����Ĭ�ϲ�֧��
};


//����ʵ������״̬
struct TMSVcPeriEqpState
{
	u8 m_abyOnline[MAXNUM_MCU_PERIEQP]; //����ż�1Ϊ�����±�, 1-online
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

//MCʵ������״̬
struct TMSVcMCState
{
	u8 m_abyOnline[MAXNUM_MCU_MC + MAXNUM_MCU_VC]; //���ʵ���ż�1Ϊ�����±�, 1-online
	TMcsRegInfo m_tMcsRegInfo[MAXNUM_MCU_MC + MAXNUM_MCU_VC];
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

//MPʵ������״̬
struct TMSVcMpState
{
	u8 m_abyOnline[MAXNUM_DRI]; //�豸�ż�1Ϊ�����±�, 1-online
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

//MTADPʵ������״̬
struct TMSVcMtAdpState
{
	u8 m_abyOnline[MAXNUM_DRI]; //�豸�ż�1Ϊ�����±�, 1-online
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

struct TMSVcDcsState
{
    u8 m_abyOnline[MAXNUM_MCU_DCS];
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

//ͬ��ǰ�ı����ⲿģ��ʵ������״̬
typedef struct tagMSSynEnvState
{
	TMSVcPeriEqpState m_tEqpState;
	TMSVcMCState      m_tMCState;
	TMSVcMpState      m_tMpState;
	TMSVcMtAdpState   m_tMtAdpState;
    // guzh [9/14/2006] DCS
    TMSVcDcsState     m_tDcsState;
}TMSSynEnvState
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//ͬ������ʱ����Ϣ�壺���˻���״̬��Э���Ƿ���ͬһ��̬��
typedef struct tagMSSynInfoReq
{
	u8 m_byInited;                //ͬ��ǰ�ı�������ģ���ʼ��״̬ 1-Inited
	TMSSynEnvState m_tEnvState;   //ͬ��ǰ�ı����ⲿģ��״̬
}TMSSynInfoReq
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//ͬ�������Ӧʱ����Ϣ�壺
typedef struct tagMSSynInfoRsp
{
	u8 m_byInited;                //ͬ��ǰ�ı�������ģ���ʼ��״̬ 1-Inited
}TMSSynInfoRsp
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//��ͬ������������ - ��Ӧ TMSReqHead::m_byMsgID
enum emgMSSynDataType
{
	emSynDataStart = 0,           // ����ͬ����ʼ��
	//emConfigDataStart,          // �����ļ�����ͬ����
	emSysTime,
	emSysCritData,                // guzh [6/12/2007] ϵͳ��Ҫ����
	emCfgFile,
	emDebugFile,
    emAddrbookFile_Utf8,
	emAddrbookFile,
	emConfinfoFile,
	emUnProcConfinfoFile,         // fxh VCS������Ϣ
	emLoguserFile,
	emVCSLoguserFile,             // fxh VCS�û���Ϣ
	emVCSUserTaskFile,            // fxh VCS�û����������Ϣ
    emUserExFile,                 // ��չ�û�����Ϣ
	//emConfigDataEnd,

	//emVcDataStart,              //ҵ������ͬ����
	emVcDeamonPeriEqpData,        //ҵ�������ڴ�����ͬ����
	emVcDeamonMCData,
	emVcDeamonMpData,
	emVcDeamonMtadpData,          //
	emVcDeamonTemplateData,
	emVcDeamonOtherData,
	emAllVcInstState,
	emOneVcInstConfMtTableData,   //ҵ��ָ��ʵ�����ڴ�����ͬ����
	emOneVcInstConfSwitchTableData,
	emOneVcInstConfOtherMcuTableData,
	emOneVcInstOtherData,
	//emVcDataEnd
	emSynDataEnd                  // ����ͬ��������
};

//response��Ϣ�ķ���ֵ�ֶ�
enum emMSReturnValue
{
    emMSReturnValue_Ok = 0,   //request��Ϣ�ѵõ���ȷ�Ĵ���
	emMSReturnValue_Error,    //һ���Դ��󣬴���ԭ����
	emMSReturnValue_Invalid   //request��Ч,���ᱻ����
};

typedef struct tagMSFrmHead
{
	u32 m_dwFrmLen;   //��֡��С��������
	u16 m_wFrmSN;     //����֡��ˮ�ţ�������
	u8  m_byFrmType;  //��ͬ������������ - ��Ӧ TMSReqHead::m_byMsgID
	u8 *m_pbyFrmBuf;  //��֡����ָ�룬���ͻ��彫��ǰ��Ԥ��sizeof(TMSReqHead)
}TMSFrmHead
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//request: ͬ����������ʱ���а���ĵ�����Ϣͷ��
typedef struct tagMSSynDataReqHead
{
	u32 m_dwFrmLen;   //����֡��С��������
	u16 m_wFrmSN;     //����֡��ˮ�ţ�������
	u8  m_byMsgID;    //�Զ�����Ϣ/�������� ID
	u8  m_byPackIdx;  //��������֡������ (��0��ʼ)
	u8  m_byMark;     //�����Ƿ�Ϊ������
}TMSSynDataReqHead
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//��Ϣͷ
//response: ͬ��������֡������Ļ�Ӧʱ����Ϣ�壺
typedef struct tagMSSynDataRsp
{
	u32 m_dwFrmLen;   //����֡��С��������
	u16 m_wFrmSN;     //����֡��ˮ�ţ�������
	u8  m_byMsgID;    //�Զ�����Ϣ/�������� ID
	u8  m_byRetVal;   //request��Ϣ������
}TMSSynDataRsp
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//��ͬ����ЧMC��Ϣ�壺
struct TMSVcMCStateHead
{
	u8 m_abyValid[MAXNUM_MCU_MC + MAXNUM_MCU_VC]; //���ʵ���ż�1Ϊ�����±�, 0,1 1-Valid
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

//��ͬ����ЧEQP��Ϣ�壺
struct TMSVcPeriEqpStateHead
{
	u8 m_abyValid[MAXNUM_MCU_PERIEQP]; //����ż�1Ϊ�����±�, 0,1 1-Valid ������������
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

struct TMSVcConfState
{
	u8  m_abyValid[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];      //0,1 1-Valid	
	u32 m_awConfDataLen[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1]; //�������ݳ��ȣ�������
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMSVcTemplateState
{
	u8 m_abyValid[MAXNUM_MCU_TEMPLATE]; //0,1 1-Valid
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMSVcInstState
{
	u8 m_abyState[MAXNUM_MCU_CONF]; //STATE_IDLE STATE_SCHEDULED STATE_ONGOING
	u8 m_abyConfIdx[MAXNUM_MCU_CONF];//�����m_byConfIdx
	CConfId m_acConfId[MAXNUM_MCU_CONF];//�����confid
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMSVcInstDataHead
{
	u8 m_byInstID;       //����ʵ��ID [1, MAXNUM_MCU_CONF]
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMSVcOtherMcuState
{
	u8 m_abyOnline[MAXNUM_CONFSUB_MCU]; //1-online
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMSCurSynState
{
	u8 m_byInstID;                //0����ʾ�ļ��������������ݣ�[1, MAXNUM_MCU_CONF]����ʾĳ����ʵ��������
	emgMSSynDataType m_emSynType; //��ͬ������������
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

#ifdef WIN32
#pragma pack( pop )
#endif

//...............................................................................................

//ms-mcu mutex
typedef struct tagMCULock
{
	tagMCULock()
    {
        m_bRet = FALSE;
		m_hMutex = NULL;
    }
	
    BOOL32 SetLock( SEMHANDLE hMutex )
	{		
		if( NULL == hMutex )
		{
			m_bRet = FALSE;
			return m_bRet;
		}	
        m_bRet = TRUE;
		m_hMutex = hMutex;
		m_bRet = OspSemTakeByTime(m_hMutex, 3000);

		return m_bRet;
	}
	
	~tagMCULock()
	{
        if( TRUE == m_bRet )
        {
            if( NULL != m_hMutex )
			{
				OspSemGive(m_hMutex);
			}
        }
	}

protected:
	BOOL32 m_bRet;
	SEMHANDLE m_hMutex;
	
}TMCULock;

//...............................................................................................


class CMSManagerSsnInst : public CInstance  
{
	enum 
	{ 
		MS_STATE_IDLE = 0,    //����
		MS_STATE_INIT,        //��ʼ��
		MS_STATE_NORMAL,      //����
	};
		
public:
	CMSManagerSsnInst();
	virtual ~CMSManagerSsnInst();
		
protected:
	//��ͨʵ�����
	void InstanceEntry( CMessage * const pcMsg );
	
	void ProcPowerOnCmd( void );
	void ClearInst( void );
	
	//����һ��MCU������Ϣ����Ϣ�岻�з�ֱ��Ͷ�ݣ�
	BOOL32 PostMsgToOtherMcuDaemon(u16 wEvent, u8 *const pbyMsg, u16 wLen);
	BOOL32 PostMsgToOtherMcuInst(u16 wEvent, u8 *const pbyMsg, u16 wLen);

	//MCU֮�� ����֪ͨ
	void ProcMcuMcuDisconnectNtf( void );	

	//��λ��MCU����λ��MCU ��ע������
	void ProcMcuMcuRegisterReq( CMessage * const pcMsg );
	void ProcMcuMcuRegisterRsp( CMessage * const pcMsg );
	
	//MCU֮�� �Զ�MCU������λ���ͼ���ǰ������ʹ��״̬֪ͨ������������Э��
	void ProcMcuMcuMSDetermine( CMessage * const pcMsg );
	//MCU֮�� ������Э�̽��֪ͨ
	void ProcMcuMcuMSResult( CMessage * const pcMsg );

	//���������ð�MCU֮�� �����ݵ�����ʱ��
	void ProcMcuMcuSynTimeOut( void );

	//���������ð�MCU֮�� �ȴ��Զ˵�����Ӧ��ʱ��ʱ��
	void ProcMcuMcuWaitforRspTimeOut( CMessage * const pcMsg );

	//���������ð�MCU֮�� ��ʼ���ݵ��� ����
	void ProcMcuMcuStartSynReq( CMessage * const pcMsg );
	void ProcMcuMcuStartSynRsp( CMessage * const pcMsg );
	//�������ð�MCU���ð�MCU �������ݵ��� ֪ͨ
	void ProcMcuMcuEndSynNtf( void );

	//���������ð�MCU֮�� ʵ���а���������ݵ��� ����
	void ProcMcuMcuSynningDataReq( CMessage * const pcMsg );
	void ProcMcuMcuSynningDataRsp( CMessage * const pcMsg );
    
    void ProcMcuMcuProbeReq( void );
    void ProcMcuMcuProbeAck( void );

    void PrintMSVerbose( s8*  pszFmt, ... );
	void PrintMSInfo( s8*  pszFmt, ... );
	void PrintMSWarning( s8*  pszFmt, ... );
	void PrintMSException( s8*  pszFmt, ... );
	void PrintMSCritical( s8*  pszFmt, ... );
	void MSManagerPrint( u8 byLevel, s8 *pszPrintStr );
	
protected:
	void ProcMcuMcuConnectTimeOut( void );
	void ProcMcuMcuRegisterTimeOut( void );
	void ProcMcuMcuCheckMSStateTimeOut( void );

	//��λ��MCU������λ��mcu����
	BOOL32 ConnectMasterMcu( void );

protected:
	//����һ��MCU-Inst������Ϣ����Ϣ���з�Ͷ�ݣ�
	BOOL32 SendCustomMsgToOtherMcu(TMSFrmHead *ptSndFrmHead);

	//������һ��MCU-Inst����Ϣ����Ϣ���������飩
	TMSFrmHead *RecvCustomMsgFromOtherMcu( CMessage * const pcMsg );
	
	//�������ݵ���ʱ�� һ������������Զ�������ͬ����Ϣ
	BOOL32 DealOneCustomSynMsg( TMSFrmHead *ptMSFrmHead );
	
protected:
	//MCU֮�� ���ݵ���
	BOOL32 SendSynData( BOOL32 bStart = FALSE );
	BOOL32 SendSynData( TMSCurSynState tMSCurSynState );
	
	BOOL32 SendSynDataOfSysTime( void );
	BOOL32 SendSynDataOfSysCritData( void );
	BOOL32 SendSynDataOfCfgFile( void );
	BOOL32 SendSynDataOfDebugFile( void );
    BOOL32 SendSynDataOfAddrbookFileUtf8( void );
	BOOL32 SendSynDataOfAddrbookFile( void );
	BOOL32 SendSynDataOfConfinfoFile( void );
	BOOL32 SendSynDataOfLoguserFile( void ); //
    BOOL32 SendSynDataOfUserExFile( void ); // ��չ�û�����Ϣ
	BOOL32 SendSynDataOfUnProcConfinfoFile( void );
    BOOL32 SendSynDataOfVCSLoguserFile( void );
    BOOL32 SendSynDataOfVCSUserTaskFile( void );
	BOOL32 SendSynDataOfVcDeamonPeriEqpData( void );
	BOOL32 SendSynDataOfVcDeamonMCData( void );
	BOOL32 SendSynDataOfVcDeamonMpData( void );
	BOOL32 SendSynDataOfVcDeamonMtadpData( void );
	BOOL32 SendSynDataOfVcDeamonTemplateData( void );
	BOOL32 SendSynDataOfVcDeamonOtherData( void );
	BOOL32 SendSynDataOfAllVcInstState( void );//
	BOOL32 SendSynDataOfOneVcInstConfMtTableData( u8 byInstID );
	BOOL32 SendSynDataOfOneVcInstConfSwitchTableData( u8 byInstID );
	BOOL32 SendSynDataOfOneVcInstConfOtherMcTableData( u8 byInstID );
	BOOL32 SendSynDataOfOneVcInstOtherData( u8 byInstID );
		
	BOOL32 SaveSynDataOfSysTime( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfSysCritData( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfCfgFile( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfDebugFile( u8 *pbyBuf, u32 dwBufLen );
    BOOL32 SaveSynDataOfAddrbookFileUtf8( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfAddrbookFile( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfConfinfoFile( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfLoguserFile( u8 *pbyBuf, u32 dwBufLen ); //
    BOOL32 SaveSynDataOfUserExFile( u8 *pbyBuf, u32 dwBufLen ); // ��չ�û�����Ϣ
	BOOL32 SaveSynDataOfVcDeamonPeriEqpData( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfVcDeamonMCData( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfVcDeamonMpData( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfVcDeamonMtadpData( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfVcDeamonTemplateData( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfVcDeamonOtherData( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfAllVcInstState( u8 *pbyBuf, u32 dwBufLen );//
	BOOL32 SaveSynDataOfOneVcInstConfMtTableData( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfOneVcInstConfSwitchTableData( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfOneVcInstConfOtherMcTableData( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfOneVcInstOtherData( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfUnProcConfinfoFile( u8 *pbyBuf, u32 dwBufLen );
    BOOL32 SaveSynDataOfVCSLoguserFile( u8 *pbyBuf, u32 dwBufLen );
    BOOL32 SaveSynDataOfVCSUserTaskFile( u8 *pbyBuf, u32 dwBufLen );
	
private:	
	u32    m_dwMcuNode;         //��һ��Mcu��Ӧ�ڵ��
	u32    m_dwMcuAppIId;       //ע���Mcu���Mcuʵ��APP��

	u32    m_dwRcvFrmLen;       //���ڽ��յ�����֡�ѽ��ճ���
	TMSFrmHead m_tSndFrmHead;   //�����͵����ݻ�����
	TMSFrmHead m_tRcvFrmHead;   //���ڽ��յ����ݻ�����

	TMSCurSynState m_tMSCurSynState; //��ǰ���ͳ�ȥ��ͬ�����ݵ�״̬ SnedReq ��ȴ���ӦAck
	                                 //������ʱ�����ƣ������ش�2�Σ���ʧ�ܵȴ��´�ͬ��
		
	BOOL32 m_bRemoteInited;          //�Զ��Ƿ��Ѿ���ɳ�ʼ��
	u8     m_byCheckTimes;           //vxworks��ɨ��Ӳ��������״̬�ĳ���״̬�������,3�����Ͻ����л�
};


//..............................................................
//�������ݵ������ͨѶЭ������ݴ���
class CMSManagerConfig
{
	/*lint -save -sem(CMSManagerConfig::FreeMSConfig,cleanup)*/
public:
	CMSManagerConfig();
	virtual ~CMSManagerConfig();

public:
	BOOL32 InitMSConfig(TMSStartParam tMSStartParam);
	void   FreeMSConfig(void);

    void   SetRemoteMpcConnected(BOOL32 bConnect);
    BOOL32 IsRemoteMpcConnected() const;

	BOOL32 InitLocalCfgInfo(u8 byLocalMcuType);

    void   SetMpcOusLed(BOOL32 bOpen);

	u32    GetLocalMcuIP(void);
	void   SetLocalMcuIP(u32 dwLocalMcuIP);
	u32    GetAnotherMcuIP(void);
	void   SetAnotherMcuIP(u32 dwAnotherMcuIP);
	u16    GetMcuConnectPort(void);
	void   SetMcuConnectPort(u16 wConnectPort);
	u8     GetLocalMcuType(void);
	void   SetLocalMcuType(u8 byLocalMcuType);
	BOOL32 IsDoubleLink(void);
	void   SetDoubleLink(BOOL32 bDoubleLink);	
	emMCUMSType  GetLocalMSType(void);
	void   SetLocalMSType(emMCUMSType emLocalMSType);
	emMCUMSState GetCurMSState(void);
	void   SetCurMSState(emMCUMSState emCurMSState);
    TMSSynState  GetCurMSSynState(void);
    void   SetCurMSSynState(TMSSynState &tMSSynState);

	BOOL32 DetermineMSlave(emMCUMSType emLocalMSType, emMCUMSState &emLocalMSState, 
		                   emMCUMSType emRemoteMSType, emMCUMSState &emRemoteMSState);

	BOOL32 IsMSConfigInited(void);

	void   SetMSSynOKFlag(BOOL32 bSynOK);
	BOOL32 IsMSSynOK(void);

    void   SetMsSwitchOK(BOOL32 bSwitchOK);
    BOOL32 IsMsSwitchOK(void);

	BOOL32 JudgeSndMsgPass(void);
	BOOL32 JudgeRcvMsgPass(void);

	BOOL32 EnterMSInitLock(void);
	void   LeaveMSInitLock(void);
	
	void   EnterMSSynLock( u16 wAppId );
	void   LeaveMSSynLock( u16 wAppId );

	u8     GetDebugLevel(void);
	void   SetDebugLevel(u8 byDebugLevel);

	BOOL32 GetMSDetermineType(void);
	void   SetMSDetermineType(BOOL32 bMSDetermineType);

    void   IncDitheringTimes(void);
    u32    GetDitheringTimes(void);

	void   RebootMCU(void);

	// guzh [6/12/2007] ����ϵͳ�ỰУ��ֵ
    void   SetMSSsrc(u32 dwSSrc);   // �ڲ����ã��ⲿ����ʹ��
    u32    GetMSSsrc(void) const; 

	//��¼����������״̬, zgc, 2008-03-25
	void   SetMSVcMCState( const TMSVcMCState &tMSVcMCState )
	{
		memcpy( &m_tMSVcMCState, &tMSVcMCState, sizeof(TMSVcMCState) );
	}
	void   GetMSVcMCState( TMSVcMCState &tMSVcMCState ) const
	{
		memcpy( &tMSVcMCState, &m_tMSVcMCState, sizeof(TMSVcMCState) );
	}

public:
	BOOL32 SetLocalMcuTime(time_t tTime);
	BOOL32 IsActiveBoard(void);

private:
	BOOL32 GetSlotThroughName(const s8* pszName, u8* pbySlot);
	BOOL32 GetTypeThroughName(const s8* pszName, u8* pbyType);
	
private:
	u32     m_dwLocalMcuIP;         //����mcu��ip ������
	u32     m_dwRemoteMcuIP;        //��һ��mcu��ip ������
	u16     m_wMcuListenPort;       //��һ��mcu��port ������
	
	u8      m_byLocalMcuType;       //��ǰMCU���� -- 8000A��8000B��WIN32
	BOOL32  m_bDoubleLink;          //Mcu Linking mode ( TRUE-DoubleLink, FALSE-SingleLink )
	BOOL32  m_bInited;              //�Ƿ��Ѿ���ɳ�ʼ��
    BOOL32  m_bRemoteConnected;     //�Զ��Ƿ��Ѿ�����(RegistAck)
	
	SEMHANDLE m_hSemInitLock;       //����ģ���ʼ��ͬ�������Ļ����ź���
	
	//�������ݵ���ʱ ����ģ����ҵ��ģ��ľ�������
	SEMHANDLE m_hSemSynLock;        //����ģ����ҵ��ģ��ľ��� ����ģ��Ļ����ź���
	u16       m_wLockedAppId;       //��ǰ������AppӦ��ID

	//���±�����д��ͬ������
	emMCUMSType  m_emLocalMSType;   //����mcu������λ���� -- ��λ�塢��λ�塢��Ч��
	emMCUMSState m_emCurMSState;    //��ǰMCU������״̬ -- ���á����á�����
                                    //���ð������Ӧ�Ķ���ҵ�񣬱��ð�ֻ������Ϣ����������Ӧ
	BOOL32       m_bSynOK;          //�Ƿ������ͬ������ FALSE-���ð�δ���ͬ���������ܵ�����Ϣ����ֹ����

    BOOL32       m_bMsSwitchOK;     //�����л��Ƿ�ɹ����������л�ʱ���ݻ�û���ü�ͬ���꣬�������л�ʧ�ܣ�

	//SEMHANDLE m_hSemDataLock;     //����ģ��App��ҵ��ģ�����ݾ���ͬ�������Ļ����ź���
	//BOOL32    m_bDataLocked;      //�Ƿ��Ѿ��������ݾ���ͬ������

	u8          m_byDebugLevel;     //���Դ�ӡ�ȼ�

	BOOL32      m_bMSDetermineType; //TRUE�����ö�ʱ����ȡӲ��������״̬��FALSE������Osp��ϢЭ��������״̬
    
    u32         m_dwDitheringTimes; //ͳ�������л�ʱӲ����������
    
    TMSSynState m_tMSSynState;      //����ʧ��״̬��¼

	u32     m_dwSysSSrc;            //��������ϵͳ��SSRCֵ���ṩ����������Ự
	TMSVcMCState m_tMSVcMCState;	//��¼����������״̬, zgc, 2008-03-25
};


typedef zTemplate< CMSManagerSsnInst, 1, CMSManagerConfig, sizeof( u8 ) > CMSManagerSsnApp;

/*=============================================================================
    �� �� ���� MSGBODY_LEN_EQ
    ��    �ܣ� check if body len of CServMsg is equal to len
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CServMsg &cServMsg
               u16 wLen
    �� �� ֵ�� inline BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/08/18  4.0			����                  ����
=============================================================================*/
inline BOOL32 MSGBODY_LEN_EQ( CServMsg &cServMsg, u16 wLen )
{
	if(cServMsg.GetMsgBodyLen() != wLen)
	{
		const s8* pszMsgStr = OspEventDesc(cServMsg.GetEventId());
		return FALSE;
	}
	
	return TRUE;
}

/*=============================================================================
    �� �� ���� MSGBODY_LEN_GE
    ��    �ܣ� check if the body len of CServMsg >= len
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CServMsg &cServMsg
               u16 wLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/08/18  4.0			����                  ����
=============================================================================*/
inline BOOL32 MSGBODY_LEN_GE(CServMsg &cServMsg, u16 wLen)
{
	if(cServMsg.GetMsgBodyLen() < wLen)
	{
		const s8* pszMsgStr = OspEventDesc(cServMsg.GetEventId());
		return FALSE;
	}
	
	return TRUE;
}

/*====================================================================
    ������      : CompareStringElements
    ����        : �Ƚ��ַ���
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: 
    ����ֵ˵��  :
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���      �޸�����
    2013/05/09              liaokang      ����
====================================================================*/
inline BOOL32 CompareStringElements(const s8* pchStr1, const s8* pchStr2)
{    
    if( NULL == pchStr1 || NULL == pchStr2 )
    {
        return FALSE;
    }

    if( strlen(pchStr1) != strlen(pchStr2) ||
        0 != strncmp( pchStr1, pchStr2, strlen(pchStr1) ) )
    {
        return FALSE;
    }
    return TRUE;
}

API void msdebug( s32 nDbgLvl );
API void setmschecktime(u32 dwTime);
API void showmsstate();

#endif //__MSMANAGERSSN_H_

//END OF FILE
