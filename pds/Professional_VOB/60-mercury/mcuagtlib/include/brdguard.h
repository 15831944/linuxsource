/*****************************************************************************
   ģ����      : KDVMCU
   �ļ���      : brdguard.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: �����������ඨ��
   ����        : ����
   �汾        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾     �޸���          �޸�����
   2002/01/25  0.9      ����            ����
   2002/07/29  1.0      ����            �·�������
   2003/07/15  1.1      jianghy         ��������
   2003/11/11  3.0      jianghy         3.0ʵ��
   2004/11/10  3.6      libo            �½ӿ��޸�
   2004/11/29  3.6      libo            ��ֲ��Linux
   2005/08/17  4.0      liuhuiyun       ����
******************************************************************************/
#ifndef BRDGUARD_H
#define BRDGUARD_H

#if _MSC_VER > 1000
#pragma once
#endif

#include "osp.h"
#include "kdvsys.h"
#include "mcuconst.h"

#ifdef _VXWORKS_
#include "brddrvlib.h"
#endif

#ifdef WIN32
#include "winbrdwrapper.h"
#endif

// MPC2 ֧��
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

// ��ʱɨ�赥��״̬��ʱ��ʱ��
#define SCAN_STATE_TIME_OUT			5000

class CCfgAgent;

class CBoardGuard : public CInstance  
{
public:
	CBoardGuard();
	~CBoardGuard();
	
protected:
	void InstanceEntry( CMessage * const pcMsg );   // ��Ϣ��ں�������������
	void ProcGuardPowerOn( void );   // GUARDģ��������Ϣ
	void ProcGuardStateScan( void ); // ����״̬��ʱɨ��
    void ProcReboot(CMessage* const pMsg);          // ����

protected:
	u8 m_byPowerStatus;
	u8 m_byPowerFanStatus;
	u8 m_byBoxFanStatus;
	u8 m_byMPCCpuStatus;
	u8 m_byMPC2TempStatus;
	u8 m_byPowerTempStatus;
	u8 m_byMPCMemoryStatus;
	u8 m_byNetSyncStatus;
	u32 m_dwSDHStatus;
	u8  m_byOldRemoteMpcState; // �Զ�Mpc����һ��״̬
//#ifndef WIN32   // vx, linux
    TBrdLedState m_tBrdLedState;
//#endif
    BOOL32 m_b8KASlamOn;     //8ka��ǰ�ĸ澯���Ƿ��Ѿ�������
#ifdef _8KI_
	u32	m_dwCpuFan;			//cpu����ת��
	u8 m_byCpuTemp;			//cpu�¶�
#endif
};

typedef zTemplate< CBoardGuard, 1 > CBoardGuardApp;
extern CBoardGuardApp	g_cBrdGuardApp;	

#endif /*End BRDGUARD_H*/
