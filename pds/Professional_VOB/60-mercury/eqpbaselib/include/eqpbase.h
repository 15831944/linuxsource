/*****************************************************************************
   ģ����      : eqpbase
   �ļ���      : eqpbase.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ����������
   ����        :  �ܼ���
   �汾        :  1.0
   ����		   :  2012/02/14
-----------------------------------------------------------------------------
******************************************************************************/

#ifndef _EQP_BASE_H_
#define _EQP_BASE_H_

#include "kdvtype.h"
#include "osp.h"
#include "eqpcfg.h"
#include "mcustruct.h"
#include "eveqpbase.h"
#include "eqpautotest.h"
#include "eqpservertest.h"


#ifdef _LINUX_
#include "nipwrapper.h"
#include "nipwrapperdef.h"
#include "brdwrapperdef.h"
#endif

#ifdef _LINUX12_
#include "brdwrapper.h"
#endif

#define EQP_CONNECT_TIMEOUT            (u16)3*1000     // ���ӳ�ʱֵ3s
#define EQP_REGISTER_TIMEOUT           (u16)5*1000     // ע�ᳬʱֵ5s
#define EQP_GETMSSTATUS_TIMEOUT        (u16)8*1000     // ��ȡ����״̬��ʱֵ8s
#define FIRST_REGACK                   (u8)1           // ��һ���յ�ע��Ack

char * StrOfIP( u32 dwIP );

/*��ǰ������*/
class CEqpBase;

	/*******************************************************************/
	/*				CChnnlStatusBase����ͨ��״̬���ඨ��	    	   */
	/*******************************************************************/
	#define  	EQP_CHNNL_STATUS_INVALID  0xff			// ͨ����ʼ״̬
	#define     INVALID_CHNLIDX           0xffff		

class CChnnlStatusBase
{
public:
	CChnnlStatusBase();
	virtual~CChnnlStatusBase();
public:

	//����ͨ��״̬
	BOOL32 SetState(const u8 byState);
	//��ȡͨ��״̬
	u8     GetState()const;
	//����ý��ģʽ(����Ƶ)
	void   SetMediaMode(const u8 byMediaMode);
	//��ȡý��ģʽ(����Ƶ)
	u8     GetMediaMode()const;
	//����ͨ������
	void   SetChnIdx(const u16 wChnIdx); 	
	//��ȡͨ������
	u16    GetChnIdx()const;

private:
	u8  m_byState;	    //״̬��
	u8  m_byMediaMode;	//��Ƶ����Ƶ
	u16 m_wChnIdx;      //ͨ������

};

	/**********************************************************************/
	/*					CEqpChnnlBase����ͨ�����ඨ��					  */
	/**********************************************************************/

class CEqpChnnlBase
{
public:
	CEqpChnnlBase(const CEqpBase*const pcEqp, CChnnlStatusBase*const pcChnlStatus);
	virtual~CEqpChnnlBase();
public:

	/*******************************��Ϣ���***********************************/
	virtual BOOL32 OnMessage(CMessage* const pcMsg);
	
	/*******************************Э��ӿ�***********************************/
	virtual BOOL32 Init();
	virtual BOOL32 Destroy();
	virtual BOOL32 Start(const u8 byMode = MODE_BOTH);
	virtual BOOL32 Stop(const u8 byMode = MODE_BOTH);

	/******************************���������ӿ�********************************/
	//������Ϣ��mcu
	BOOL32  PostMsgToMcu(CServMsg& cMsg, BOOL32 bMcuA = TRUE, BOOL32 bMcuB = TRUE);
	//������Ϣ��������
	BOOL32  PostMsgToServer(CServMsg& cMsg,const BOOL32 bSendHeadInfo = TRUE);
	//����ͨ������
	void	SetChnIdx(const u16 wChnIdx);
	//��ȡͨ������
	u16     GetChnIdx()const;
	//����ͨ��״̬
	BOOL32  SetState(const u8 byState);
	//��ȡͨ��״̬
	u8      GetState()const;
	//��ȡInsId
	u16     GetInsID()const;
	//���ö�ʱ��
	void    SetTimer(const u32 dwTimerId, const u32 dwMillionSecs, const u32 dwParam = 0 );
	//ȡ����ʱ��
	void	KillTimer(u32 dwTimerId);
private:
	const CEqpBase *const m_pcEqp;         //Eqp�ص�ָ��
	CChnnlStatusBase*const m_pcChnlState ; //ͨ������״̬����ʵ����
};

	/************************************************************************/
	/*						CEqpCfg���������ඨ��						    */
	/************************************************************************/
class CEqpCfg:public TEqpCfg
{
public:
	CEqpCfg();
	virtual~CEqpCfg();
public:
	void Print();
public:
	u32 m_dwMcuNodeA;				//����McuA����
	u32 m_dwMcuIIdA;				//��McuAͨ��IID
	u32 m_dwMcuNodeB;				//����McuB����
	u32 m_dwMcuIIdB;				//��McuBͨ��IID
	u32 m_dwServerNode;             //�������Խ���
    u32 m_dwServerIId;				//��������IID
	u32 m_dwMpcSSrc;				//Ψһ�ỰԴ
	u32 m_dwMcuRcvIp;               //McuΪ��������ת����IP
	u16 m_wMcuRcvStartPort;         //McuΪ��������ת����˿�
	u16 m_wEqpVer;					//����汾
	u8  m_byRegAckNum;				//ע��ɴ���
	TPrsTimeSpan m_tPrsTimeSpan;    //�ش�����
	TMcuQosCfgInfo m_tQosInfo;      
};

	/************************************************************************/
	/*							��Ϣ���Ͷ�����			                */
	/************************************************************************/
class CEqpMsgSender
{
public:
	/*******************************Э��ӿ�*********************************/
	//���ⲿ������Ϣ����
	virtual void   PostEqpMsg( const u32 dwMcuNode, const u32 dwDstId, CServMsg& cMsg,const BOOL32 bSendHeadInfo = TRUE) = 0;	
	// ���ö�ʱ
	virtual int    SetTimer( u32 dwTimerId, long nMilliSeconds, u32 dwPara = 0 ) = 0;	
	// ȡ����ʱ
	virtual int    KillTimer(u32 dwTimerId) = 0;	
	// ����ע��
	virtual int	   DisConRegister( u32 dwMcuNode ) = 0;	
	// Tcp�����Ч��У��
	virtual BOOL32 IsValidTcpNode(const u32 dwTcpNode) = 0;
	// �Ͽ�һ��Tcp���
	virtual BOOL32 DisconnectTcpNode(const u32 dwTcpNode) = 0;
	// ����TCP����
	virtual int    ConnectTcpNode(const u32 dwConnectIP, const u16 wConnectPort) = 0;
	// ��ȡʵ����
	virtual u16    GetInsID() = 0;
};

	/************************************************************************/
	/*							CEqpBase�ඨ��								*/
	/************************************************************************/
class CEqpBase
{
public:
	enum EEqpState{
		E_EQP_INVALID = 0,					// δ��ʼ��
			E_EQP_OFFLINE,					// δע��
			E_EQP_ONLINE					// �Ѿ�ע��
	};

public:
	CEqpBase(CEqpMsgSender*const pcMsgSender, CEqpCfg*const pcCfg);
	virtual~CEqpBase();
public:

	/***********************��Ϣ���************************/
	virtual BOOL32 OnMessage(CMessage* const pcMsg);

	/***********************Э��ӿ�************************/
	//��ʼ��
	virtual BOOL32 Init();
	//����
	virtual BOOL32 Destroy();
	//���Ӵ���
	virtual BOOL32 OnConnectMcuTimer(CMessage* const pcMsg);
	//ע��Mcu����
	virtual BOOL32 OnRegisterMcuTimer(CMessage* const pcMsg);
	//ע��Mcu�ɹ�����
	virtual BOOL32 OnRegisterMcuAck(CMessage* const pcMsg);
	//ע��Mcuʧ�ܴ���
	virtual BOOL32 OnRegisterMcuNack(CMessage* const pcMsg);
	//��������
	virtual BOOL32 OnDisConnect(CMessage* const pcMsg);
	//����״̬��ȡ�ɹ����� /����״̬��ȡ��ʱ����
	virtual BOOL32 OnGetMsStatusAck(CMessage* const pcMsg);
	//�����������ӷ�������ʱ����
	virtual BOOL32 OnConnectServerTimer();
	//��������ע���������ʱ����
	virtual BOOL32 OnRegisterServerTimer();
	//����ע���������Է������ɹ�����
	virtual BOOL32 OnRegisterServerAck(CMessage* const pcMsg);
	//����ע���������Է�����ʧ�ܴ���
	virtual BOOL32 OnRegisterServerNack(CMessage* const pcMsg);
	//����Qos����
	virtual BOOL32 OnSetQosCmd(CMessage *const pcMsg);
	//�޸ķ��͵�ַ
	virtual BOOL32 OnModifyMcuRcvIp(CMessage* const pcMsg);

	/***********************ҵ��ӿ�************************/
	//��mcu����Ϣ
	BOOL32	  PostMsgToMcu( CServMsg& cMsg, const BOOL32 bMcuA = TRUE, const BOOL32 bSndToEqpSSn = FALSE )const;
	//���������Է���������Ϣ
	BOOL32	  PostMsgToServer( CServMsg& cMsg,const BOOL32 bSendHeadInfo = TRUE)const;
	//��Qos����ֵ
	void      ComplexQos(u8& byValue, u8 byPrior);
	//�Ƿ��������
	virtual BOOL32 IsHdEqp( )const;
	//����ͨ������Ϊͨ�������ַ
	BOOL32	  SetChnNum(const u16 wChnNum);
	//��ȡͨ����
	u16       GetChnNum()const;
	//��ȡͨ��
	CEqpChnnlBase* GetChnl(const u16 wIndex);
	//ͨ����ַʵ��������
	BOOL32    InitChn(CEqpChnnlBase* pChn, const u16 wChnIdx);
	//����Eqp״̬
	void      SetEqpState(const EEqpState eState);
	//��ȡEqp״̬
	EEqpState GetEqpState()const;
	//���ö�ʱ
	void      SetTimer(const u32 dwTimerId, const u32 dwMillionSecs, const u32 dwParam = 0 )const;
	//ȡ����ʱ
	void	  KillTimer(u32 dwTimerId) const;
	//��ȡ����
	const CEqpCfg*  GetEqpCfg()const;
	//��ȡʵ����
	u16       GetInsID()const;
	//У��Tcp�����Ч��
	BOOL32    IsValidTcpNode(const u32 dwTcpNode)const;
	//�Ͽ�һ��Tcp�������
	BOOL32    DisconnectTcpNode(const u32 dwTcpNode)const;
	//����TCP����
	int       ConnectTcpNode(const u32 dwConnectIP, const u16 wConnectPort);
	//��ȡ�豸��Ϣ
	void      SetDeviceInfo(CDeviceInfo &cDeviceInfo);
private:
	//����ͨ��
	BOOL32    DestroyChn();
	//��������
    BOOL32    DestroyCfg();

private:
	CEqpMsgSender*const m_pcMsgSender;      //��Ϣ���Ͷ���
	CEqpCfg*			m_pcEqpCfg;			//�������ã���ʵ����
	u16					m_wChnNum;		    //����ͨ����
	CEqpChnnlBase**		m_ppcChnl;			//����ͨ��
	EEqpState		    m_eEqpStat;			//����״̬��
};
#endif