/*****************************************************************************
   ģ����      : license generate
   �ļ���      : licenseinst.h
   ����ļ�    : licenseinst.cpp
   �ļ�ʵ�ֹ���: ͨѶ�࣬ʵ�ָ�����˵���ϢͨѶ
   ����        : john
   �汾        : V0.9  Copyright(C) 2001-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2006/06/23  0.9         john         ����
******************************************************************************/
#ifndef _LICENSE_H
#define _LICENSE_H
#include "osp.h"

#define	AID_LICENSEGEN	  60

class CLicenseGen : public CInstance  
{
public:
	CLicenseGen();
	~CLicenseGen();

// ��Ϣ������
protected:
	
	void InstanceEntry( CMessage * const pcMsg );

	void ProcPowerOn( const CMessage *const pcMsg );
	void ProcGenerateLicenseReq( const CMessage *const pcMsg );
	void ProcGenerateLicenseRsp( const CMessage * pcMsg );

// ���ܺ���
private:
	void ClearInst( void );

private:
	u32  m_dwservernode;
	u32  m_dwserveripaddr;
	u16  m_wserverport;
};

typedef zTemplate< CLicenseGen, 1 > CLicenseGenApp;
extern CLicenseGenApp	g_cLicenseGenApp;	

// û��ѹ����ֻ����Win32
struct TNetAddr
{
	u32 dwAddr;
	u16 wPort;
};

#endif
