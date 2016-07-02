/*****************************************************************************
    ģ����      : EqpAgt
    �ļ���      : eqpagtscan.h
    ����ļ�    : 
    �ļ�ʵ�ֹ���: ɨ���豸trap��Ϣ
    ����        : liaokang
    �汾        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    �޸ļ�¼:
    ��  ��      �汾        �޸���          �޸�����
    2012/06/18  4.7         liaokang        ����
******************************************************************************/
#ifndef _EQPAGT_SCAN_H_
#define _EQPAGT_SCAN_H_
#include "eqpagtutility.h"

// EqpAgtScan�߳������Ϣ
// ���������ţ��漰�����㣬����EV_AGT_BGN����û�������Ҵ������ȼ������⣩
#define  EQPAGT_SCAN_POWERON        (EV_AGT_BGN)        // ����
#define  EQPAGT_TRAPINFO            (EV_AGT_BGN + 1)    // Trap��Ϣ�����ڵ��ԣ�
#define  EQPAGT_SCAN_TIMER          (EV_AGT_BGN + 2)    // ���涨ʱ��

// ��ʱɨ���豸״̬��ʱ��ʱ��
#define  SCAN_STATE_TIME_OUT		5000

class CEqpAgtScan : public CInstance  
{
public:
	CEqpAgtScan();
	~CEqpAgtScan();
	
protected:
	void InstanceEntry( CMessage * const pcMsg );   // ��Ϣ��ں�������������
	void ProcScanPowerOn( void );                   // ģ��������Ϣ
    void ProcScanTimer( void );                     // ���涨ʱ��
    void ProcTrapInfoScan( CMessage* const pcMsg ); // ��ҵ��ģ��Trap��Ϣ��ʱɨ��
    void ScanAndSendTrap( TEqpAgtTrapFunc pfTrapFunc ); // Trap��Ϣɨ�貢����
private:
    u32  m_dwMaxTimerIdx;                           // ��ǰ���ʱ�����
};

typedef zTemplate< CEqpAgtScan, 1 > CEqpAgtScanApp;
extern CEqpAgtScanApp	g_cEqpAgtScanApp;	

#endif /*End _EQPAGT_SCAN_H_ */
