/*****************************************************************************
   ģ����      : ���渴����( Video Multiplexer)
   �ļ���      : McuPrsInst.h
   ����ʱ��    : 2003�� 12�� 4��
   ʵ�ֹ���    : McuPrsInst.cpp
   ����        : zhangsh
   �汾        : 
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   03/12/4	   v1.0	       zhangsh	   create
   06/03/21    4.0         �ű���      �����Ż� 
******************************************************************************/
#ifndef _MCUPRSINST_H_
#define _MCUPRSINST_H_

#include "osp.h"
#include "evmcueqp.h"
#include "mcustruct.h"
#include "mcuconst.h"
#include "netbuf.h"
#include "eqpcfg.h"
#include "mcuprs.h"



#define   PRS_CONNECT_TIMEOUT           (u16)(3*1000)   // connect time out(s)
#define   PRS_REGISTER_TIMEOUT          (u16)(4*1000)   // register time out(s)
#define   FIRST_REGACK                  (u8)1           // ��һ���յ�ע��Ack

struct NetBufInfo
{
    NetBufInfo()
    {
        Init();
    }
	TNetLocalAddr local;
	TNetAddr	  addr;
	void Init()
	{
		memset( &local, 0, sizeof(TNetLocalAddr) );
		memset( &addr,  0, sizeof(TNetAddr) );
	}
	void EmptyLocal()
	{
		memset( &local, 0, sizeof(TNetLocalAddr) );
	}
	void EmptyFeed()
	{
		memset( &addr, 0, sizeof(TNetAddr) );
	}
};
typedef NetBufInfo TNetBufInfo;

class CMcuPrsInst : public CInstance
{
	enum //ʵ��״̬
	{
		IDLE   = 0,
		NORMAL = 1,
	};
public:
	CMcuPrsInst();
	virtual ~CMcuPrsInst();

private:
	TPeriEqpStatus  m_tPrsStatus;    //״̬
	TRSParam        m_tRSParam;      //��������
	TNetBufInfo tNetBufInfo[MAXNUM_PRS_CHNNL];

private:
	//-----------����״̬----------------
	CNetBuf*   m_pNetBuf[MAXNUM_PRS_CHNNL];         //��װ
	u8		   m_sCreated[MAXNUM_PRS_CHNNL];        //�����Ƿ��Ѿ�����
private:
	//-------------��Ϣ��Ӧ----------------
	void InstanceEntry( CMessage *const pcMsg );    //��Ϣ���
	void Init( CMessage *const pcMsg );             //��ʼ��
	void MsgRegAckProc( CMessage* const pcMsg );    //ȷ��
	void MsgRegNackProc( CMessage* const pcMsg );   //�ܾ�
	void MsgSetSource( CMessage* const pcMsg );     //���ü��Դ
	void MsgPrintStatus();   //��ӡͨ��״̬
	void MsgRemvoeAllCh( CMessage* const pcMsg );   //�������ͨ��
	void SendStatusChangeMsg();                     //����״̬

	void ProcReconnectBCmd( CMessage* const pcMsg ); //MCU ֪ͨ�Ự ����MPCB ��Ӧʵ��, zgc, 2007/04/30

	void ProcMainTaskScanReq( CMessage* const pcMsg ); //prs guard������̽��������Ϣ����zgc, 2008-04-07

    BOOL InitNetBuf();
private:
	//-------------����------------------
    BOOL32 ConnectMcu( BOOL32 bConnectA, u32& dwMcuNode ); //��������MCU
    void   Disconnect( CMessage *const pcMsg );            //�������
    void   ProcConnectTimeOut( BOOL32 bConnectA );         //�������ӳ�ʱ
    void   Register( BOOL32 bRegisterA, u32 dwMcuNode );   //������MCUע��
    void   ProcRegisterTimeOut( BOOL32 bRegisterA );       //������MCUע�ᳬʱ
	void   ProcGetMsStatusRsp( CMessage* const pcMsg );    //����ȡ����������Ϣ
    void   ClearAllChannelStatus( void );                  //�������ͨ��״̬
	BOOL32 SendMsgToMcu( u16 wEvent, CServMsg* const pcServMsg ); //������Ϣ��MCU
};

class CPrsCfg
{
public:
    CPrsCfg();
    virtual ~CPrsCfg();
public:    
    u32     m_dwMcuNode;        //���ӵ�MCU.A �ڵ��
    u32     m_dwMcuNodeB;       //���ӵ�MCU.B �ڵ��
    u32     m_dwMcuIId;         //��ʵ��ͨѶ��MCU.A ��ȫ��ID
    u32     m_dwMcuIIdB;        //��ʵ��ͨѶ��MCU.B ��ȫ��ID
    BOOL32  m_bEmbed;           //��Ƕ��MCU.A��
    BOOL32  m_bEmbedB;          //��Ƕ��MCU.B��
    u8      m_byRegAckNum;      //��һ���յ�ע��ACK
	u32     m_dwMpcSSrc;        // guzh [6/12/2007] ҵ���ỰУ��ֵ 
    u16     m_wLocalPort;
    TPrsCfg m_tPrsCfg;          //��������
public:
    void FreeStatusDataA( void );
    void FreeStatusDataB( void );
};

void PrsAPIEnableInLinux();

typedef zTemplate< CMcuPrsInst, 1, CPrsCfg > CMcuPrsApp;
extern CMcuPrsApp g_cMcuPrsApp;

#endif
