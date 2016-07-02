/*****************************************************************************
   ģ����      : Board Agent Basic
   �ļ���      : boardconfigbasic.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ������ú�������
   ����        : �ܹ��
   �汾        : V4.0  Copyright(C) 2001-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2007/08/20  4.0         �ܹ��       ����
******************************************************************************/
#ifndef BOARDCONFIGBASIC_H
#define BOARDCONFIGBASIC_H

#include "mcuconst.h"
#include "vccommon.h"
#include "mcuagtstruct.h"
#include "eqpcfg.h"

#ifdef _VXWORKS_
#include "brdDrvLib.h"
#endif

// [pengjie 2010/3/9] CRI2/MPC2 ֧��
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

// brdcfg.ini
#define SECTION_BoardSystem		(s8*)"BoardSystem"
#define KEY_MpcIpAddr			(s8*)"MpcIpAddr"
#define KEY_MpcIpAddrB			(s8*)"MpcIpAddrB"
#define KEY_MpcPort				(s8*)"MpcPort"
#define KEY_MpcPortB			(s8*)"MpcPortB"
#define KEY_HeartBeatTime		(s8*)"HeartBeatTime"
#define KEY_HeartBeatNum		(s8*)"HeartBeatNum"

#define SECTION_IsFront			(s8*)"IsFront"
#define KEY_Flag				(s8*)"Flag"
#define DEFVALUE_Flag			(u8)0

#define SECTION_Eqp 			(s8*)"Eqp"
#define KEY_Id			    	(s8*)"Id"

#define KEY_IsSimuMpu               (s8*)"IsSimuMpu"

// brdcfgdebug.ini   
#define SECTION_BoardConfig			(s8*)"BoardConfig"
#define SECTION_BoardDebug			(s8*)"BoardDebug"
// mcueqp.ini
#define SECTION_EqpMixer            (s8*)"EQPMIXER"
#define KEY_IsSimuApu			    (s8*)"IsSimuApu"
#define KEY_SimApuVer			    (s8*)"SimApuVer"
#define KEY_QualityLvl              (s8*)"QualityLvl"

#define KEY_Layer					(s8*)"Layer"
#define KEY_Slot					(s8*)"Slot"
#define KEY_Type					(s8*)"Type"
#define KEY_BoardIpAddr				(s8*)"BoardIpAddr"
#define KEY_IsAutoTest              (s8*)"IsAutoTest"
#define KEY_TestMode				(s8*)"TestMode"
#define KEY_ServerIpAddr			(s8*)"ServerIpAddr"
#define KEY_BrdType					(s8*)"BrdType"

#define SECTION_IpConfig			(s8*)"IpConfig"
#define ENTRYPART_ETHERNET			(s8*)"Eth"
#define ENTRYPART_IPADDR			(s8*)"IpAddr"
#define ENTRYPART_IPNUM				(s8*)"IpNum"
#define	ENTRYPART_MAC				(s8*)"Mac"

#define SECTION_IpRoute				(s8*)"IpRoute"
#define KEY_IpRouteNum				(s8*)"IpRouteNum"
#define ENTRYPART_IpRoute			(s8*)"IpRoute"


#define SUCCESS_FIRST_CONNECT   (u8)1  // first to connect the mcu
// ���嵥������������ʵ����Ŀ
#define BOARD_AGENT_MAX_INSTANCE	1
// ��������MPC�Ϲ������Ķ�ʱ��ʱ��(2��)
#define POWEN_ON_CONNECT            200  // 0.2s
#define CONNECT_MANAGER_TIMEOUT		2000
// ע��ȴ���ʱ��ʱ��(3��)
#define REGISTER_TIMEOUT		3000
// ȡ������Ϣ�ȴ���ʱ��ʱ��(2��)
#define GET_CONFIG_TIMEOUT		2000
// ��ʱɨ�赥��״̬��ʱ��ʱ��(5S)
#define SCAN_STATE_TIME_OUT			5000
// ����澯�����󳤶�
#define	MAXNUM_BOARD_ALARM		80


class CBBoardConfig
{
public:
	CBBoardConfig();
	virtual ~CBBoardConfig();
	friend class CBBoardAgent;

public:
	u32 GetMpcIpA();
	u16 GetMpcPortA();
	u32 GetMpcIpB();
	u16 GetMpcPortB();

	void SetMpcIpA(u32 dwMpcIp);
	void SetMpcPortA(u16 wMpcPort);
	void SetMpcIpB(u32 dwMpcIp);
	void SetMpcPortB(u16 wMpcPort);

	u8  GetBoardId();
	void SetBrdId(u8 byBrdId);
	void SetBrdId(u8 byBrdLayer, u8 byBrdSlot);

	u32 GetBrdIpAddr();
	void SetBrdIpAddr(u32 dwBrdIp);

	u8 GetBrdEthChoice();
	void SetBrdEthChoice(u8 byEthChoice);
	
	TBrdPos GetBrdPosition();

	// [pengjie 2010/3/9] CRI2/MPC2 ֧��
	BOOL32 GetBrdPosByConfig(TBrdPos &tBrdPos);

	void SetBrdPosition( TBrdPos tBrdPosition );

    BOOL IsBrdMtHDSerial() const;

	BOOL32 ReadConnectMcuInfo();
	BOOL32 WriteBoardInfoToDetecteeFile();
	BOOL32 GetBoardInfo();   // WIN32ģ����    

	void NetAToNetB(void); //A����ת��B��A��0
	void NetBToNetA(void); //B����ת��A��B��0
	void SwitchNetAB(void); //A��B���õ���

	void brdlog(char * fmt, va_list argptr);  //��ӡ����

	u8 TransPid2BrdType(u32 dwPID);

public:
	u32 m_dwDstMcuNodeA;
	u32 m_dwDstMcuNodeB;

	u32	m_dwDstMcuIIdA;
	u32	m_dwDstMcuIIdB;

	u16 m_wDiscHeartBeatTime;
	u8  m_byDiscHeartBeatNum;

	BOOL32 m_bPrintBrdLog;
	BOOL32 m_bUpdateSoftBoard;

private:
	u32 m_dwMpcIpAddrA;	//������
	u32 m_dwMpcIpAddrB;	//������

	u16 m_wMpcPortA;	//������
	u16 m_wMpcPortB;	//������

	u8  m_byBrdId;
	u32 m_dwBrdIpAddr;	//������
	TBrdPos m_tBoardPosition;

	u8  m_byBrdEthChoice;

	//������
	BOOL32    m_bIsTest;
	u16 	m_wLastEvent;
	u8	m_abyLastEventPara[20];
};
#endif
