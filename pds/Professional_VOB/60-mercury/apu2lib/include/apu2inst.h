/*****************************************************************************
  ģ����      : apu2lib
  �ļ���      : apu2inst.h
  ����ļ�    : 
  �ļ�ʵ�ֹ���: ��Ϣת��
  ����        : �ܼ���
  �汾        : 1.0  
-----------------------------------------------------------------------------*/
#ifndef _APU2INST_H_
#define _APU2INST_H_

#include "osp.h" 
#include "eqpcfg.h"        
#include "mcustruct.h"     
#include "mcuver.h"
#include "evapu2.h"
#include "boardagent.h"
#include "eqpbase.h"
#include "apu2cfg.h"
#include "apu2eqp.h"

/************************************************************************/
/*							CApu2Inst�ඨ��								*/
/************************************************************************/
class CApu2Inst:public CEqpMsgSender, public CInstance
{
public:
	CApu2Inst();
	virtual~CApu2Inst();
public:
	/***********************Daemonʵ������**************************/
	void   DaemonInstanceEntry(CMessage * const pcMsg, CApp* pcApp );
	void   DaemonProcInit(CMessage * const pcMsg);
	void   DaemonProcPrint(CMessage * const pcMsg);
#ifdef _8KI_//��ҪΪ���������MCU��ȡ����������
	//8KI�ϵ紦��
	void   DaemonProcPowerOn();
	//������ʱ
	void   DaemonProcConnetTimerOut();
	//ע�ᳬʱ
	void   DaemonProcRegTimerOut();
	//ע��ɹ�
	void   DaemonProcRegAck(CMessage * const pcMsg);
	//ע��ʧ��
	void   DaemonProcRegNack();
	//��������
	void   DaemonProcOspDisconnect();
#endif
	/***********************��ͨʵ������****************************/
    void   InstanceEntry( CMessage * const pcMsg );	
	//�����ʼ��
	void   ProcInit(CMessage * const pcMsg);
	//�������Գ�ʼ��
	void   ProcTestInit(CMessage * const pcMsg);

	/**************************Э��ӿ�*****************************/
	//��Ϣ����
	void   PostEqpMsg(const u32 dwMcuNode, const u32 dwDstId, CServMsg& cMsg,const BOOL32 bSendHeadInfo = TRUE);	
	//���ö�ʱ
	int    SetTimer(u32 dwTimerId, long nMilliSeconds, u32 dwPara = 0 );	
	//ȡ����ʱ
	int	   KillTimer(u32 dwTimerId);	
	//����ע��
	int    DisConRegister( u32 dwMcuNode );
	//У��Tcp�����Ч��
	BOOL32 IsValidTcpNode(const u32 dwTcpNode);
	//�Ͽ�һ��Tcp�������
	BOOL32 DisconnectTcpNode(const u32 dwTcpNode);
	//����TCP����
	int    ConnectTcpNode(const u32 dwConnectIP, const u16 wConnectPort);
	//��ȡʵ����
	u16    GetInsID();

	/***********************��Ϣת������****************************/
	//mcu������ͨ����Ϣת����EqpBase��Ϣ
	void   TransMcuMsgToEqpBaseMsg(CMessage * const pcMsg);
	//EqpBase��Ϣת����mcu������ͨ����Ϣ
	void   TransEqpBaseMsgToMcuMsg(CServMsg& cMsg);

private:
	CEqpBase* m_pcEqp;
#ifdef _8KI_
	BOOL32    m_bIsGetMixerCfg;//��־�Ƿ��MCU��ȡ������������
	u32       m_dwMcuNode;     //�����MCU��ȡ���ýڵ�ţ���ȡ���ú��ͷŶϿ��ýڵ�
#endif
};

typedef zTemplate< CApu2Inst, MAXNUM_APU2_MIXER> CApu2MixApp;
extern CApu2MixApp g_cApu2MixApp;

#endif


