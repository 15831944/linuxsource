/*****************************************************************************
   ģ����      : Mcu Agent
   �ļ���      : ProcAlarm.h
   ����ļ�    : ProcAlarm.cpp
   �ļ�ʵ�ֹ���: �澯���ά��
   ����        : liuhuiyun
   �汾        : V0.9  Copyright(C) 2001-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2005/08/17  1.0         liuhuiyun       ����
******************************************************************************/
#ifndef  _AGENT_ALARMPROC_H
#define  _AGENT_ALARMPROC_H
#include "agentcommon.h"
#include "mcuagtstruct.h"
#include "agtcomm.h"

#ifdef WIN32
#pragma  once
#endif

class  CAlarmProc 
{
public:
    CAlarmProc();
    virtual ~CAlarmProc();

public:
    
    BOOL32 AddAlarm( u32 dwAlarmCode, u8 byObjType, u8 abyObject[], TMcupfmAlarmEntry * ptData );
    BOOL32 FindAlarm( u32 dwAlarmCode, u8 byObjType, u8 abyObject[], TMcupfmAlarmEntry * ptData );
    BOOL32 DeleteAlarm( u32 dwSerialNo );
    BOOL32 GetAlarmStamp(u16& wAlarmStamp);
    BOOL32 GetAlarmTableIndex(u16 wTableIndex, TMcupfmAlarmEntry& tAlarmEntry);
	void   ProcReadError( u8 byErrorId );   //���������Ϣ
	BOOL32 GetErrorString(s8* pszErrStr, u16 wLen);
	void   ShowErrorString(void);
	void   ShowCurAlarm( void );		    //��ʾ��ǰ�澯
    void   ShowSemHandle( void );           //��ʾ�ź���ֵ
	void   InitAlarmTable(void);		    //��ʼ���澯��
	void   Quit(void);
private:

	TMcupfmAlarmTable	m_atAlarmTable[MAXNUM_MCU_ALARM];	
	TMcuPerformance		m_tMcuPfm;
    u32                 m_dwNowAlarmNo;		// ��ǰ�澯��
	u16                 m_wErrorNum;		// �����ô�����
	SEMHANDLE	        m_hMcuAlarmTable;   // �澯������ź���
	s8                  m_achCfgErr[MAX_ERROR_NUM][MAX_ERROR_STR_LEN];		// �����ַ���
};

#endif // _AGENT_ALARMPROC_H
