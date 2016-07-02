  /*****************************************************************************
   ģ����      : mcu_new
   �ļ���      : mcuguard.h
   ����ļ�    : mcuguard.cpp
   �ļ�ʵ�ֹ���: MCU����Ӧ����ͷ�ļ�
   ����        : ����
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2002/07/25  0.9         ����        ����
   2003/03/28  1.1         ���     �޸�
******************************************************************************/

// mcuguard.h: interface for the CMcuGuardInst class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __MCUGUARD_H
#define __MCUGUARD_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//��ʱ�˲���ʱ��
#define TIMESPACE_GUARD_ONGOING_CHECK (u32)60000 //��λ(����)
//��ʱι��
#define TIMESPACE_FEEDDOG             (u32)10000  //��λ(����)

//��ʱ���������ʧ�ܴ���
#define TIMESPACE_MAX_FAILED_TIMES    (u8)3  

//��ʱ�ڴ�����Сʣ���ڴ�ߴ�
#define MEMSPACE_MIN_SIZE             (u32)(3*1024*1024)

//��License������Ϣ
#define READ_LICENSE_SUCCESS					"Read License success!"
#define READ_LICENSE_FILE_ERROR					"Read License file error!"
#define READ_LICENSE_VERSION_ERROR				"Read License version error!"
#define READ_LICENSE_TYPE_ERROR					"Read License type error!"
#define READ_LICENSE_KEY_ERROR					"Read License key error!"
#define READ_LICENSE_MCUDEVICEID_ERROR			"Read MCU device id error!"
#define READ_LICENSE_MCUTYPE_ERROR				"Read MCU type error!"
#define READ_LICENSE_MCUACCESSNUM_ERROR			"Read MCU access num error!"
#define READ_LICENSE_VCSACCESSNUM_ERROR			"Read VCS access num error!"
#define READ_LICENSE_MCUEXPIREDATA_ERROR		"Read MCU expire data error!"
#define READ_LICENSE_ILLEGALEBOARD_ERROR        "Illegal Running Board!"
//[2011/01/28 zhushz]PCmt error add
#define READ_LICENSE_PCMTACCESSNUM_ERROR        "Read PcMt access num error!"
#define READ_LICENSE_UNDEFINED_ERROR			"Undefined error!"

class CMcuGuardInst : public CInstance  
{
	enum
	{
		SHIFT_MCSSN_CHECK = 0,
		SHIFT_PERIEQPSSN_CHECK,
		SHIFT_MCUVC_CHECK,
		SHIFT_MCUCONFIG_CHECK,
		SHIFT_MPSSN_CHECK,
		SHIFT_MTADPSSN_CHECK,
        SHIFT_DCSSSN_CHECK,
        SHIFT_END_CHECK
	};
	//�˲�����ֵ
	enum
	{
		STATUS_NORMAL = 65,
		STATUS_BUSY = 75,
		STATUS_CONGESTION = 85,
		STATUS_ALLFULL = 95
	};
public:
	CMcuGuardInst();
	virtual ~CMcuGuardInst();

protected:
	void InstanceEntry( CMessage * const pcMsg );

	void ProcReoPowerOn(void);					//�ϵ紦����
	void ProcMemCheck(void);					//�ڴ�˲麯��
	void ProcFileSystemCheck(void);				//�ļ�ϵͳ�˲麯��
	void ProcTaskCheck(void);					//APP TASK �˲麯��
	void ProcCpuStatusCheck(void);				//CPU ״̬�˲麯��
	void ProcGuardTest(void);					//GUARDģ����Ժ���
	void ProcPowerOff(void);					//�µ���Ϣ������
	void ProcTaskTestAck( const CMessage * pcMsg );	//����˲�ȷ����Ϣ������
	void ProcSyncSrcStateCheck(void);
	void ProcTimerFeedDog( void );	
	void ProcGetLicenseDataFromFile( void );
private:
	void ProcEachTaskCheck( u8 byShift );
	u8   GuardGetBrdNetSyncLock();
	void PingEveryTask();        //��ÿһ��Ӧ�÷��Ͳ��԰�
	void GetBoardSequenceNum( u8 *pBuf, s8* achSep );
    
	void GenEncodeData( u8 *pOutBuf, u16 wInBufLen, u8 *pInBuf, BOOL32 bDirect = TRUE );

	// zgc [2007/01/23] License�汾����
	BOOL32 GetOldLicenseDataFromFile( s8* pchPath );	//��ȡ�ɰ汾License
	BOOL32 GetCurrentLicenseDataFromFile( s8* pchPath );	//��ȡ��ǰ�汾License

private:
	u32  m_dwGuardTestSeriNo;	 //�˲���Ϣ���
	u8   m_byCurSyncSourceState;
	u8   m_bySyncSourceMode;     //ʱ��Դ��ʽ
	u8   m_byTaskState;          //����Ӧ�õ�״̬��ÿһλ����һ��Ӧ��
        	                     //ƫ����ΪSHIFT_**_CHECK
	u8   m_abyTaskBusyTimes[SHIFT_END_CHECK];
	
	BOOL32 m_bRebootMcu;         //�Ƿ���Ҫ����MCU
	s8     achAppName[32];
};

// zgc [2007/01/26]
class CMcuGuardData
{
public:
	CMcuGuardData();
	virtual ~CMcuGuardData();

public:
	u8 GetLicenseErrorCode();	// ��Licnese������
	void SetLicenseErrorCode( u8 byErrorCode );	// дLicnese������

    void SetLicenseSN(const s8* szSN);
    const s8* GetLicenseSN(void);

private:
	u8	m_byLicenseErrorCode;	// License������
    s8  m_szLicenseSN[64];      // ���к�

};

// [12/4/2011 liuxu] ���ھ������Ź��߳����ܼ�ʱ�յ�������̵߳Ļظ����Ѹ��̹߳Ҷϵ�����
class CGuardCheck
{
public:
	CGuardCheck() { Destroy(); }
	~CGuardCheck(){ Destroy(); }
	
public:
	BOOL32	Init( const u8 byMaxUnRsp );
	void	Destroy();
	
	// ������߳������Լ��ı��λ
	void	ClientSet();
	
	// Guard���
	BOOL32	GuardCheck();
	
	// Guard�ж�Client�Ƿ�����Ӧ
	BOOL32	IsClientUnRsp() const { return m_byUnRspTimes >= m_byMaxUnRspTimes; }
	
	// Guard����RspTimes
	BOOL32	ResetUnRspTimes( ) { m_byUnRspTimes = 0; }
	
private:
	enum EGCFlag
	{
		E_GC_FLAG_CHECK = 0,
			E_GC_FLAG_BUSY
	};
	
private:
	SEMHANDLE		m_hSem;					// ͬ���ź���
	u8				m_byFlag;				// ���
	u8				m_byUnRspTimes;			// ������߳�����Ӧ����
	u8				m_byMaxUnRspTimes;		// �������Ӧ����
};

API void mcuguarddisable(void);
API void mcuguardenable(void);
API void showlicense(void);

typedef zTemplate< CMcuGuardInst, 1, CMcuGuardData > CMcuGuardApp;

extern CMcuGuardApp	g_cMcuGuardApp;	//MCU����Ӧ��ʵ��

#endif //__MCUGUARD_H
