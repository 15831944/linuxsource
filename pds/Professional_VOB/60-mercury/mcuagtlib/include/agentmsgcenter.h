/*****************************************************************************
   ģ����      : Mcu Agent
   �ļ���      : Agentmsgcenter.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ��Ϣ��ת
   ����        : 
   �汾        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���        �޸�����
   2005/08/17  4.0         liuhuiyun     ����
******************************************************************************/
#ifndef _MCUAGENT_MSGDEAL_H
#define _MCUAGENT_MSGDEAL_H

#include "evagtsvc.h"
#include "osp.h"
#include "kdvsys.h"
#include "mcuconst.h"
#include "mcuagtstruct.h"
#include "agtcomm.h"
#include "agentsnmp.h"
#include "snmpadp.h"
#include "procalarm.h"

#include "ftpc_api.h"

#ifdef WIN32
#pragma  once
#endif

// ����ͳ�ƶ�ʱ�ϱ�ʱ�� ��ƹ�����Ϊ600s(10����) [miaoqingsong 20111219 modify]
#define UPDATE_PFMINFO_TIME_OUT			600000

// ��ʱɨ����ûص���ʱ��ʱ�� mqs 20101118 add
#define OPENORCLOSE_MPCLED_TIME_OUT     35000

class CMsgCenterInst : public CInstance
{
	/*lint -save -esym(1551, CMsgCenterInst::~CMsgCenterInst)*/
	/*lint -save -sem(CMsgCenterInst::Quit,cleanup)*/

public:
    CMsgCenterInst();
    virtual ~CMsgCenterInst();
	void InstanceEntry(CMessage* const pcMsg);

protected: 
    BOOL32 Initialize(void);
    BOOL32 Quit(void);
    BOOL32 SetBoardLedState( u8 byLayer, u8 bySlot, u8 byType, s8 * pchLedState );

protected:
	void ProcSDHStatusChange( CMessage * const pcMsg );         // ����SDH״̬
	void ProcBoardStatusChange( CMessage * const pcMsg );       // ������״̬
	void ProcLinkStatusChange( CMessage * const pcMsg );        // ������·״̬
	void ProcNetSyncStatusChange( CMessage * const pcMsg );     // ������ͬ��״̬
	void ProcSoftwareStatusChange( CMessage * const pcMsg );    // ����������
	void ProcPowerOnOffStatusChange( CMessage * const pcMsg );  // �������
	void ProcPowerStatusChange( CMessage * const pcMsg );       // �����ϵ�
	void ProcModuleStatusChange( CMessage * const pcMsg );      // ����ģ��״̬
	void ProcBoardQueryAlarm( CMessage * const pcMsg );         // �������ѯ�澯
	void ProcPowerFanStatusChange( CMessage * const pcMsg );    // �����Դ����״̬
	void ProcBoxFanStatusChange( CMessage * const pcMsg );      // ����������״̬, mqs, 2010/12/14
	void ProcMPCCpuStatusChange( CMessage * const pcMsg );      // ����MPC��Cpu״̬, mqs, 2010/12/15
	void ProcMPCMemoryStatusChange( CMessage * const pcMsg);    // ����MPC��Memory״̬, mqs, 2010/12/15
	void ProcPowerTempStatusChange( CMessage * const pcMsg);    // �����Դ���¶��쳣״̬��mqs, 2010/12/16
	void ProcMpc2TempStatusChange( CMessage * const pcMsg);     // ����MPC2���¶��쳣״̬��mqs,2011/01/13
	void ProcBrdFanStatusChange( CMessage * const pcMsg );      // ���������
	void ProcConfStatusChange( CMessage * const pcMsg );        // �������״̬, mqs, 2010/12/22, �ϼ�MCU����
    void ProcLedStatusChange( CMessage * const pcMsg );         // ����Led״̬��MPC�壩
    /*void ProcLedAlarm( CMessage * const pcMsg );*/                
    void ProcBoardStatusAlarm( CMessage * const pcMsg );        // �������赥��澯��Led/����״̬�� [2012/05/04 liaokang]
    void ProcMcuLedAlarm( CMessage * const pcMsg );             // ������mcu��Led�澯
	void ProcCpuFanStatusChange( CMessage * const pcMsg );		// ����cpu���ȸ澯
	void ProcCpuTempStatusChange( CMessage * const pcMsg );		// ����cpu�¶ȸ澯

    void ProcMcsUpdateBrdCmd( CMessage * const pcMsg );         // mcu֪ͨ������µ���汾
#ifdef _MINIMCU_
	void ProcMcsUpdateDscMsg( CMessage * const pcMsg );			// mcu����DSC�汾ͳһ������, zgc, 20070820
	void ProcSetDscInfoReq( CMessage * const pcMsg );			// ����DSC info, zgc, 2007-07-17
	void ProcSetDscInfoRsp( CMessage * const pcMsg );			// ����DSC info�Ļ�Ӧ����, zgc, 2007-07-17
	void ProcDscRegSucceedNotif( CMessage * const pcMsg );			// ֪ͨMCU DSC���Ѿ�ע��ɹ�, zgc, 2007-07-31
#endif	
	
    void ProcNMSUpdateBrdCmd( CMessage * const pcMsg );         // �������ܸ���MPC�汾
	void ProcSmUpdateBrdCmd( void );          // ����sm����MPC�汾
    void ProcGetConfigError();                                  // ������ȡ���ô���
    void ProcPowerOffAlarm();                                   // �ػ��澯
    void ProcColdStartdAlarm();                                 // ����������
    void ProcSnmpInfo();                                        // ȡSnmp��Ϣ
	void ProcMsgToBrdManager( CMessage * const pcMsg );				// ת��MCU��������������Ϣ, zgc, 2007-08-28

    u8   GetBoardLedStatus(u8 byBrdType, TEqpBrdCfgEntry& tBrdCfg, u8 * pszLedStr );         // ȡ��Ӧ����ĵ�״̬
    u16  SendPrmAlarmTrap( CNodes * pcNodes, TMcupfmAlarmEntry tAlarmEntry, u8 byTrapType ); // ����Ӧ�ĸ澯trap
	void ProcUpdateMcuPfmInfoTimer( void );
	void ProcOpenorcloseMpcLedTimer( void );    // ����MPC��NMS��״̬��mqs 20101118 add
	// �����¶�״̬�ı䣬Ŀǰ֧��DRI2�� [10/25/2011 chendaiwei]
	void ProcBoardTempStatusChange( CMessage * const pcMsg );
	//  [10/26/2011 chendaiwei] ����CPU״̬�ı䣬Ŀǰ֧��DRI��DRI2��
	void ProcBoardCpuStatusChange(CMessage * const pcMsg);
	/*void ProcBoardSoftwareVersionNotif(CMessage * const pcMsg );*/

private:
    CNodes*			m_pcSnmpNodes;
    CAgentSnmp*		m_pcAgentSnmp;
   
};

//����MCUϵͳ��Ϣ��SNMP�ص�
u16 ProcCallBackMcuSystem(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuLocal (u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);

//����MCU������Ϣ��SNMP�ص�
u16 ProcCallBackMcuNet(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuNetQos(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuNetSnmpCfg(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuNetBoardCfg(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);

//����MCU������Ϣ��SNMP�ص�
u16 ProcCallBackMcuEqpBas(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuEqpVMP(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuEqpPrs(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuEqpDcs(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuEqpMixer(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuEqpTVWall(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuEqpNetSync(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuEqpRecorder(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuPfmInfo(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);

//����MCU�澯��֪ͨSNMP�ص�
u16 ProcCallBackMcuPfmAlarm(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuNotifications(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);

u16 McuMgtGetUpdateFiles(void *pBuf, u16 *pwBufLen);
u16 McuMgtSetUpdateFiles(void *pBuf, u16 *pwBufLen);

u16 McuSmGetUpdateFiles(void *pBuf, u16 *pwBufLen);
u16 McuSmSetUpdateFiles( void *pBuf, u16 *pwBufLen );

BOOL32 ChangeNumBufToStrBuf(u8 * pNumBuf, u8 byBufLen, u8 * pStrBuf);

void ProcCbFtpcCallbackFun(TFtpcCallback *PTParam, void *pCbd);

//[5/16/2013 liaokang] ���ڴ�,�ȴ�����snmpadp֧����ȥ
// Snmp�Ƿ�ΪUTF8����
void   SetSnmpIsUtf8(BOOL32 bIsUtf8);
BOOL32 GetSnmpIsUtf8();
//end
BOOL32 TransEncodingForNmsData(const s8 *pSrc, s8 *pDst, u32 dwLen, BOOL32 bSet = FALSE);

typedef zTemplate<CMsgCenterInst, 1 > CMsgCenterApp;
extern CMsgCenterApp g_cMsgProcApp;

#endif // _MCUAGENT_MSGDEAL_H

