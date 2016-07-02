/*****************************************************************************
ģ����      : Board Agent
�ļ���      : isslavesys.h
����ļ�    : isslavesys.cpp
�ļ�ʵ�ֹ���: IS��ϵͳ��IS2.2 8313��ҵ��
����        : liaokang
�汾        : V4R7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
2012/05/05  1.0         liaokang      ����
******************************************************************************/
#ifndef _ISSLAVESYS_H_
#define _ISSLAVESYS_H_

#include "osp.h"
#include "evagtsvc.h"
#include "mcuconst.h"
#include "kdvsys.h"
#include "kdvlog.h"
#include "kdvtype.h"
#include "mcuver.h"

#ifdef _LINUX_
#ifdef _LINUX12_
    #include "brdwrapper.h"
    #include "brdwrapperdef.h"
    #include "nipwrapper.h"
    #include "nipwrapperdef.h"
#else
    #include "boardwrapper.h"
#endif
#endif

#ifdef WIN32
    #include "winbrdwrapper.h"
#endif

// log��ӡ�ȼ����壬��Ӧkdvlog
#define	LOG_OFF				            (u8)0	// �رմ�ӡ
#define	LOG_ERROR				        (u8)1	// �������д���(�߼���ҵ�����),�������
#define	LOG_WARNING			            (u8)2	// �澯��Ϣ, ������ȷ, Ҳ���ܴ���
#define	LOG_KEYSTATUS			        (u8)3	// �������е�һ���ؼ�״̬ʱ����Ϣ���
#define	LOG_DETAIL				        (u8)4	// ��ͨ��Ϣ, ��ò�Ҫд��log�ļ���

// brdcfg.ini begin
#define SECTION_BoardMasterSystem	    (s8*)"BoardMasterSystem"
#define KEY_BrdMasterSysInnerIp	        (s8*)"BrdMasterSysInnerIp"
#define KEY_BrdMasterSysListenPort	    (s8*)"BrdMasterSysListenPort"
#define DEFVALUE_BrdMasterSysListenPort BRDMASTERSYS_LISTEN_PORT   // ��ϵͳ��IS2.2 8548��Ĭ�ϼ����˿�

#define SECTION_IpConfig			    (s8*)"IpConfig"
#define KEY_IsSupportIpConfig           (s8*)"IsSupportIpConfig"
#define DEFVALUE_IsSupportIpConfig		(u8)0           // Ĭ�� ��֧��
#define KEY_EthChoice				    (s8*)"EthChoice"
#define DEFVALUE_EthChoice			    (u8)1           // Ĭ�� ������
#define KEY_BrdSlaveSysIp	            (s8*)"BrdSlaveSysIp"
#define KEY_BrdSlaveSysInnerIp	        (s8*)"BrdSlaveSysInnerIp"
#define KEY_BrdSlaveSysIPMask	        (s8*)"BrdSlaveSysIPMask"
#define KEY_BrdSlaveSysDefGateway	    (s8*)"BrdSlaveSysDefGateway"
// brdcfg.ini end

// default param
// time out
#define CONNECT_BRDMASTERSYS_TIMEOUT    (u32)2000     //���Ӽ�� 2000ms
#define REGISTER_BRDMASTERSYS_TIMEOUT   (u32)2000     //ע���� 2000ms
#define SCAN_BRDSLAVESYS_STATE_TIMEOUT  (u32)5000     //ɨ���� 5000ms
// heart beat check param
#define HEART_BEAT_TIME                 (u8)10        // osp���������� Time
#define HEART_BEAT_NUM                  (u8)3         // osp���������� Num

// kdvlog config file
#if defined( WIN32 ) || defined(_WIN32)
    #define BRDSLAVESYS_KDVLOG_FILE	    (s8*)"./conf/kdvlog_isslavesys.ini"
#else
    #define BRDSLAVESYS_KDVLOG_FILE	    (s8*)"/usr/etc/config/conf/kdvlog_isslavesys.ini"
#endif

// IS��ϵͳ
class CBrdSlaveSysInst:public CInstance
{
protected:
    enum 
    {
        STATE_IDLE,			//����
        STATE_INIT,			//��ʼ��
        STATE_NORMAL,		//����
	};

public:
	CBrdSlaveSysInst();
	~CBrdSlaveSysInst();

protected:
	// ʵ����Ϣ������ں���������override
	void InstanceEntry(CMessage* const pcMsg);
    // ����ʵ��
    void ClearInst();

    // ����
	void ProcBrdSlaveSysPowerOn( CMessage* const pcMsg ) ;
    // ����IS��ϵͳ��IS2.2 8548��
    void ProcConnectBrdMasterSysTimeOut();
    // ע��IS��ϵͳ��IS2.2 8548����Ӧ
    void ProcRegToBrdMasterSysRsp( CMessage* const pcMsg );
    // ע��IS��ϵͳ��IS2.2 8548��TimeOut
    void ProcRegToBrdMasterSysTimeOut( );
    
    // ��ʱɨ��
    void ProcBrdSlaveSysStateScan(void);

    // ����
    void ProcBrdSlaveSysUpdateSoftwareCmd( CMessage* const pcMsg );
    // ������
    void ProcBrdSlaveSysResetCmd( CMessage* const pcMsg );
    // ��������
    void ProcDisconnect( void );

    // ������Ϣ��IS��ϵͳ��IS2.2 8548��
    void PostMsgToBrdMasterSys( u16 wEvent, void * pMsg = NULL, u16 wLen = 0 );

private:
    TBrdLedState  m_tBrdSlaveSysLedState;    // ��ϵͳ��IS2.2 8313��led״̬   
    SWEthInfo     m_tBrdSlaveSysEthPortState;// ��ϵͳ��IS2.2 8313������״̬
    u32           m_dwBrdMasterSysNode;      // ��ϵͳ��IS2.2 8548��node id
};

class CBrdSlaveSysCfg
{
public:
    CBrdSlaveSysCfg();
    virtual ~CBrdSlaveSysCfg();
public:
    // �������ļ���ʼ������������Ϣ
    BOOL32  ReadConnectConfigInfo();
    // ��ʼ������������Ϣ
    BOOL32  InitLocalCfgInfoByCfgFile();
    // ��ȡ��ϵͳ��IS2.2 8548�� secondary ip
    u32     GetBrdMasterSysInnerIP();
    // ��ȡ��ϵͳ��IS2.2 8548�� Listen Port
    u16     GetBrdMasterSysListenPort();
private:
    u32           m_dwBrdMasterSysInnerIP;    // �ڲ�ͨ�� ip��second ip��
    u16           m_wBrdMasterSysListenPort;  // ��ϵͳ��IS2.2 8548�� Listen Port
};

typedef zTemplate< CBrdSlaveSysInst, 1, CBrdSlaveSysCfg > CBrdSlaveSysApp;
extern CBrdSlaveSysApp	g_cBrdSlaveSysApp;

void IsSlaveSysLog( u8 byPrintLvl, s8* pszFmt, ... );
void staticlog( LPCSTR lpszFmt, ...);
API void isslavesysver(void);     // �汾��ӡ
API void isslavesyshelp( void );   // ������Ϣ
API void isslavesysinit(void);    // IS22Mpc8313��ʼ��
API void quit(void);              // quit
void AddEventStr();            // �����Ϣ�ַ���

#endif /* _ISSLAVESYS_H_ */
/* end of file isslavesys.h */