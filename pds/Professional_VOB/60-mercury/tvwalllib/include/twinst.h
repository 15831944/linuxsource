/*****************************************************************************
   ģ����      : �໭����ʾ
   �ļ���      : twinst.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: �໭����ʾʵ��ͷ�ļ�
   ����        : ����
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
 -----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/11/20     			   zhangsh		MCC
   2006/03/21  4.0         �ű���      �����Ż�
******************************************************************************/
#ifndef _TV_WALL_INST_H_
#define _TV_WALL_INST_H_

#include "osp.h"
#include "mcustruct.h"
#define  MEDIA_TYPE
#include "codeccommon.h"
#include "codeclib.h"
#include "eqpcfg.h"

#define MAXNUM_TVWALL_CHNNL			(u8)5				//���ͨ����
#define TVWALL_MODEL_NEW			(u8)3				//����ǽ����ģʽ
#define FIRST_REGACK				(u8)1				//��һ���յ�ע��Ack
#define TV_CONNETC_TIMEOUT			(u16)(3 * 1000)	    //connect time
#define TV_REGISTER_TIMEOUT			(u16)(6 * 1000)	    //register time
#define TVWALL_PRI                  (u16)60

#define PCHECK( x )  \
    if( NULL == x ) \
    {\
	OspPrintf( TRUE, FALSE, "The pointer cannot be Null\n"  );\
	return;\
    }\

class CTWInst : public CInstance
{
protected:
    enum ESTATE
    {
        eIDLE,
        eNORMAL,
        eERROR
    };
    
    u8            m_byTvNum;	    // ����ǽ������
    TTvWallStatus m_tTWStatus;	    // ����ǽ״̬
    TEqpCfg       m_tCfg;		    // ����ǽ����
    u8            m_byTvModel;	    // ����ǽ���ģʽ
    CKdvDecoder   m_acDecoder[MAXNUM_TVWALL_CHNNL];
    TPrsTimeSpan  m_tPrsTimeSpan;  
public:
    CTWInst();
    ~CTWInst();
	
	//��Ϣ���
	void InstanceEntry( CMessage * const pMsg );

    void ProcConnectTimeOut( BOOL32 bIsConnectA );				// ���������
    void ProcRegisterTimeOut( BOOL32 bIsRegisterA );			// ע�������
	BOOL32  ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode );	// ��������Mcu
	void    Register( BOOL32 bIsRegisterA, u32 dwMcuNode );		// ��MCUע��

	void MsgDisconnectProc( CMessage * const pMsg );			// ��������
    void MsgRegAckProc( CMessage * const pMsg );				// ע��ɹ�
   	void MsgRegNackProc( CMessage * const pMsg );				// ע��ʧ��
    void ProcStartPlayMsg( CMessage * const pcMsg );			// ��ʼ����
	void ProcStopPlayMsg( CMessage * const pcMsg );				// ֹͣ����
	void ProcGetMsStatusMsg( CMessage * const pcMsg );			// ȡ����״̬
    void SendNotify();											// ״̬�ϱ�
	BOOL SendMsgToMcu( u16 wEvent, CServMsg const &cServMsg );

	void ProcReconnectBCmd( CMessage * const pcMsg );	//MCU ֪ͨ�Ự ����MPCB ��Ӧʵ��, zgc, 2007/04/30

	BOOL Init( CMessage * const pMsg );
	BOOL InitDecoder();								// ��ʼ��������
	BOOL Play( s32 nChnnl );						// ָ��ͨ����ʼ����
	BOOL Stop( s32 nChnnl );						// ָֹͣ��ͨ��
	void StopAllChannels( void );					// ֹͣ����ͨ��

	void ProcSetQosInfo( CMessage * const pMsg );	// ����Qos��Ϣ
    void ComplexQos( u8& byValue, u8 byPrior );

	// ������Ƶ�������
    void SetEncryptParam( u8 byChnIdx, TMediaEncrypt *ptMediaEncryptVideo, TDoublePayload *ptDoublePayloadVideo );
	
	// ������Ƶ�������
    BOOL32 SetAudEnctypt( u8 byChnIdx, TMediaEncrypt *ptMediaEncryptVideo, TDoublePayload *ptDoublePayloadVideo );
	void ConfigShow();
	void StatusShow();
};

class CTvWallCfg
{
public:
    CTvWallCfg();
    virtual ~CTvWallCfg();
public:
    u32           m_dwMcuIId ;	 // mcu.Aʵ��ID
    u32			  m_dwMcuIIdB;	 // mcu.Bʵ��ID
    u32           m_dwMcuNode;	 // ��MCU.Aͨ�Žڵ��
    u32			  m_dwMcuNodeB;	 // ��MCU.Bͨ�Žڵ��
    BOOL32        m_bEmbed;      // ��Ƕ��A��
    BOOL32		  m_bEmbedB;     // ��Ƕ��B��
    u8            m_byRegAckNum; // ��һ���յ�ע��

	u32           m_dwMpcSSrc;      // guzh [6/12/2007] ҵ���ỰУ��ֵ

public:
    void FreeStatusDataA();
    void FreeStatusDataB();
	u32 GetCfgWait2BlueTimeSpan();
};

typedef zTemplate< CTWInst, 1, CTvWallCfg > CTvWallApp;
extern CTvWallApp g_cTvWallApp;

#endif //_TV_WALL_INST_H_
