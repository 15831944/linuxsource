/*****************************************************************************
   ģ����      : MP
   �ļ���      : mpmanage.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: MP��MCU�ӿڶ���
   ����        : ������
   �汾        : V0.1  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/07/10  0.1         ������        ����
******************************************************************************/
#ifndef __MPMANAGER_H_
#define __MPMANAGER_H_

#define DEFAULT_MAPPORT 29990 //��Ҫ���ٵ�Ŀ��ip��Ӧ���ն�ȡ��������ͨ��ip��˿�ʱ�õĹ̶��˿�

// ��������Ϣ
struct TSwitchGrp
{
private:
	u8  m_bySrcChnl;
	u8  m_byDstMtNum;
	TMt *m_pDstMt;
	
public:
	TSwitchGrp() 
	{
		Init();
	}
	
	void Init()
	{
		m_bySrcChnl = 255;
		m_byDstMtNum = 0;
		m_pDstMt = NULL;
	}
	
	BOOL32 IsNull()
	{
		return m_bySrcChnl == 255;
	}
	
	void SetSrcChnl( u8 bySrcChnl )
	{
		m_bySrcChnl = bySrcChnl;
	}
	u8 GetSrcChnl()
	{
		return m_bySrcChnl;
	}
	
	void SetDstMtNum( u8 byDstMtNum )
	{
		m_byDstMtNum = byDstMtNum;
	}
	u8 GetDstMtNum()
	{
		return m_byDstMtNum;
	}
	
	void SetDstMt( TMt *pDstMt )
	{
		m_pDstMt = pDstMt;
	}
	TMt *GetDstMt()
	{
		return m_pDstMt;
	}
};

//[8/5/2011 liuxu]
// ���ܼ�� : ����TSwitchGrp��m_pDstMt��һ��ָ���ⲿ��ָ��,�����m_pDstMt�����޸�,����Ӱ���ⲿʹ�����ĵط�
// CSwitchGrpAdpt��Ϊ������������Ƶ�. CSwitchGrpAdptͨ����TSwitchGrp��m_pDstMt�����Լ��Ļ�����,Ȼ���滻
// ����, �Ӷ�ʹ����TSwitchGrp��m_pDstMt�Ĵ��벻Ӱ����
class CSwitchGrpAdpt
{
public:
	CSwitchGrpAdpt( ) : m_nGrpNum(0), m_ppMtList(NULL){ }
	~CSwitchGrpAdpt( ) { Reset(); }

public:
	void   Reset();
	
	// ��m_ppMtList����ptSwitchGrp������mtlist
	BOOL32 Convert( const s32 nGrpNum, TSwitchGrp* ptSwitchGrp );

private:
	s32			m_nGrpNum;							// TSwitchGrp����Ŀ
	TMt			**m_ppMtList;						// ��ʱ�洢TSwitchGrp��mtlist
};



struct TSwInfo
{
	u8  m_byMpId;
	u32 m_dwRcvIp;
	u16 m_wRcvPort;
	u32 m_dwDstIp;
	u16 m_wDstPort;
};

struct TAllMpSwInfo
{
	TSwInfo m_tSwInfo[MAXNUM_DRI];
	u8		m_UsedMPNum;

	TAllMpSwInfo()
	{
		memset(this, 0, sizeof(TAllMpSwInfo));
	}
};

class CMpManager
{
public:
	CMpManager();
	virtual ~CMpManager();

public:	
    //-------------------------Multicast-------------------------------
	//��ʼ�鲥
	BOOL32 StartMulticast( const TMt & tSrc, u16 wSrcChnnl = 0/*u8 bySrcChnnl = 0*/, u8 byMode = MODE_BOTH, BOOL32 bConf = TRUE );
	//ֹͣ�鲥
	BOOL32 StopMulticast( const TMt & tSrc, u8 bySrcChnnl = 0, u8 byMode = MODE_BOTH, BOOL32 bConf = TRUE );
    
    //-------------------------Distributed Conf Multicast-------------------------------
    //��ʼ��ɢ�����鲥
    BOOL32 StartDistrConfCast( const TMt &tSrc, u8 byMode = MODE_BOTH, u8 bySrcChnnl = 0) ;
    //ֹͣ��ɢ�����鲥
    BOOL32 StopDistrConfCast (const TMt &tSrc, u8 byMode = MODE_BOTH, u8 bySrcChnnl = 0) ;

    //------------------------Switch Bridge----------------------------
	//����������
	BOOL32 SetSwitchBridge( const TMt & tSrc, u8 bySrcChnnl = 0, u8 byMode = MODE_BOTH, BOOL32 bEqpHDBas = FALSE,u16 wSpyStartPort = SPY_CHANNL_NULL );
	//�Ƴ�������
	BOOL32 RemoveSwitchBridge( const TMt & tSrc, u8 bySrcChnnl = 0, u8 byMode = MODE_BOTH, BOOL32 bEqpHDBas = FALSE,u16 wSpyStartPort = SPY_CHANNL_NULL );

	//------------------------------Mt----------------------------------
	//��ָ���ն����ݽ������¼��ն�
// 	BOOL32 StartSwitchToSubMt( const TMt & tSrc,
// 							   u8 bySrcChnnl,
// 							   const TMt & tDst,
// 							   u8 byMode = MODE_BOTH,
// 							   u8 bySwitchMode = SWITCH_MODE_BROADCAST,
// 							   BOOL32 bH239Chnnl = FALSE,
//                                BOOL32 bStopBeforeStart = TRUE,
// 							   BOOL32 bSrcHDBas = FALSE,
// 							   u16 wSpyPort = SPY_CHANNL_NULL);

	//zjj20100201
	//  [11/10/2009 pengjie] ������ش�֧��
	BOOL32 StartSwitchToMmcu( const TMt & tSrc,
		u16 wSrcChnnl,
		const TMt & tDst,
		u8 byMode = MODE_BOTH,
		u8 bySwitchMode = SWITCH_MODE_BROADCAST,
		BOOL32 bH239Chnnl = FALSE,
		BOOL32 bStopBeforeStart = TRUE,
		BOOL32 bSrcHDBas = FALSE,
		u16 wSpyPort = SPY_CHANNL_NULL);

	//ֹͣ�����ݽ�����ֱ���¼����ն�
	//void StopSwitchToSubMt( const TMt & tDst, u8 byMode = MODE_BOTH, BOOL32 bH239Chnnl = FALSE, u16 wSpyStartPort = SPY_CHANNL_NULL );
	void StopSwitchToSubMt( u8 byConfIdx, 
							u8 byMtNum,
							const TMt *ptDst, 
							u8 byMode, 
							u16 wSpyPort = SPY_CHANNL_NULL );

	//ֹͣ�����ն�����
    // ���� [4/30/2006] ����������ʹ�á�����ds ��RecvOnly����
	//void StopRecvSubMt( const TMt & tMt, u8 byMode );

    // guzh [3/21/2007] �����㲥����
    BOOL32 StartSwitchToBrd(const TMt &tSrc, u8 bySrcChnnl, BOOL32 bForce, u16 wSpyStartPort = SPY_CHANNL_NULL,u8 byMpId = 0);
    BOOL32 StartSwitchFromBrd(const TMt &tSrc, u8 bySrcChnnl, u8 byDstMtNum, const TMt* const ptDst, u16 wSpyStartPort = SPY_CHANNL_NULL);
    BOOL32 StartSwitchEqpFromBrd(const TMt &tSrc, u16 bySrcChnnl, u8 byEqpId, u16 wDstChnnl);
    // guzh [3/23/2007] ɾ���㲥Դ
    BOOL32 StopSwitchToBrd(const TMt &tSrc, u8 bySrcChnnl, u16 wSpyStartPort = SPY_CHANNL_NULL);
    
	
	BOOL32 StartSwitchToDst(TSwitchDstInfo &tSwitchDstInfo, u16 wSpyStartPort = SPY_CHANNL_NULL);
	void   StopSwitchToDst(TSwitchDstInfo &tSwitchDstInfo);	

	void AddSendSelfMutePack(const u8& byMpId,const TSendMutePack& tSendMutePack);
	void DelSendSelfMutePack(const u8 & byMpId,const TMt & tMt);
	//------------------------------Mc---------------------------------
	//��ָ���ն����ݽ��������
	BOOL32 StartSwitchToMc( const TMt & tSrc, u8 bySrcChnnl, u16 wMcInstId, u8 byDstChnnl = 0, u8 byMode = MODE_BOTH, u16 wSpyPort = SPY_CHANNL_NULL );
	//ֹͣ�����ݽ��������
	void StopSwitchToMc( u8 byConfIdx, u16 wMcInstId,  u8 byDstChnnl = 0, u8 byMode = MODE_BOTH );

	//----------------------------PeriEqp----------------------------------	 
	//����¼��������ݽ���IP��ַ��˿�
	BOOL32 GetRecorderSwitchAddr(u8 byRecorderId, u32 &dwSwitchIpAddr,u16 &wSwitchPort); 
	//��������������ݽ���IP��ַ��˿�
	BOOL32 GetMixerSwitchAddr(u8 byMixerId, u32 &dwSwitchIpAddr,u16 &wSwitchPort); 
	//�������������������ݽ���IP��ַ��˿�
	BOOL32 GetBasSwitchAddr(u8 byBasId, u32 &dwSwitchIpAddr,u16 &wSwitchPort);
	//���仭��ϳ��������ݽ���IP��ַ��˿�
	BOOL32 GetVmpSwitchAddr(u8 byVmpId, u32 &dwSwitchIpAddr,u16 &wSwitchPort);
    //���临�ϵ���ǽ�����ݽ���IP��ַ��˿�
    BOOL32 GetVmpTwSwitchAddr(u8 byVmpTwId, u32 &dwSwitchIpAddr, u16 &wSwitchPort);
	//���䶪���ش��������ݽ���IP��ַ��˿�
	BOOL32 GetPrsSwitchAddr(u8 byPrsId, u32 &dwSwitchIpAddr,u16 &wSwitchPort );

	//��ָ���ն����ݽ���������
	BOOL32 StartSwitchToPeriEqp( const TMt & tSrc,
								 u16 wSrcChnnl,
								 u8 byEqpId,
								 u16 wDstChnnl = 0, 
								 u8 byMode = MODE_BOTH,
								 u8 bySwitchMode = SWITCH_MODE_BROADCAST,
								 BOOL32 bEqpHDBas = FALSE,
								 BOOL32 bStopBeforeStart = TRUE,
								 u8 byHduSubChnId = 0);
	//ֹͣ�����ݽ���������
	void StopSwitchToPeriEqp( u8 byConfIdx, u8 byEqpId, u16 wDstChnnl = 0,  u8 byMode = MODE_BOTH, u8 byHduSubChnId = 0 ); 	


	//��Ļ��ӡ����Mp��Ϣ
	void ShowMp();
	//��Ļ��ӡ���н�����
	void ShowBridge();
	//��Ļ��ӡ���н�����Ϣ
	void ShowSwitch( u8 byType );
	//get switchinfo according to DstAddr
	BOOL32 ShowSwitchRouteToDst(u8 byConfIdx, u32 dwDstIp, u16 wDstPort, BOOL32 bDetail = TRUE);
	//��Ļ��ӡ����MtAdp��Ϣ
	void ShowMtAdp();
	// ��ȡָ��mp���ϵĽ�����Ϣ
	void GetMPSwitchInfo( u8 byMpId, TSwitchTable &tSwitchTable );


	//��������Mp����Ϣ
	void ProcMpToMcuMessage(const CMessage * pcMsg);

	//��������Mtadp����Ϣ
	void ProcMtAdpToMcuMessage(const CMessage * pcMsg);

	//-----------------------������غ���----------------------------
	//��ʼ����
	BOOL32 StartSwitch(const TMt & tSrcMt, u8 byConfIdx,
                     u32 dwSrcIp, u32 dwDisIp,
                     u32 dwRcvIp, u16 wRcvPort,
                     u32 dwDstIp, u16 wDstPort,
                     u32 dwRcvBindIP = 0,u32 dwSndBindIP = 0,
                     u8 bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_NONE,
                     u8 byUniformPayload = INVALID_PAYLOAD,
                     u8 byIsSwitchRtcp = 1,
                     BOOL32 bStopBeforeStart = TRUE,u8 byDstMtId = 0);
	//ֹͣ����
	BOOL32 StopSwitch( u8 byConfIdx, u32 dwDstIp, u16 wDstPort, u8 byIsSwitchRtcp = 1);
    BOOL32 StartStopSwitch(u8 byConfIdx, u32 dwDstIp, u16 wDstPort, u8 byMpId = 0);

	//��ʼ����
	BOOL32 StartSwitch(const TMt & tSrcMt, u8 byConfIdx,
                     u32 dwSrcIp, u32 dwDisIp,
                     u32 dwRcvIp, u16 wRcvPort,
                     const TLogicalChannel& tFwdChnnl,
                     u16 wDstChnnl = 0,
                     u8 bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_NONE,
                     u8 byUniformPayload = INVALID_PAYLOAD,
                     u8 byIsSwitchRtcp = 1,
                     BOOL32 bStopBeforeStart = TRUE,u8 byDstMtId = 0);
	//ֹͣ����
	BOOL32 StopSwitch( u8 byConfIdx, const TLogicalChannel& tFwdChnnl, u16 wDstChnnl = 0, 
		               u8 byIsSwitchRtcp = 1, u8 byMulPortSpace = 1);

	BOOL32 StartSwitchToAll( const TMt &tSrc,
							const u8 bySwitchGrpNum,
							TSwitchGrp *pSwitchGrp,
							u8 byMode,
							u8 bySwitchMode = SWITCH_MODE_BROADCAST,
							BOOL32 bStopBeforeStart = TRUE,
							u16 wSpyStartPort = SPY_CHANNL_NULL );

	//--------------------��㵽һ��Ľ���---------------------------
	//���ӽ���
	BOOL32 AddMultiToOneSwitch( u8 byConfIdx, u32 dwSrcIp, u32 dwDisIp, u32 dwRcvIp, u16 wRcvPort, 
		                      u32 dwDstIp, u16 wDstPort,u32 dwRcvBindIP = 0, u32 dwSndBindIP = 0, 
							  u32 dwMapIpAddr = 0, u16 wMapPort = 0 );
	//�Ƴ�����
	BOOL32 RemoveMultiToOneSwitch( u8 byConfIdx, u32 dwRcvIp, u16 wRcvPort, u32 dwDstIp, u16 wDstPort );

	//zjl20100414 rtcp���������
	BOOL32 AddMultiToOneSwitch(u8 byConfIdx, u8 byDstNum, TSwitchChannelExt *ptSwitchChannelExt);
	//zjl20100414 rtcp����𽻻�
	BOOL32 RemoveMultiToOneSwitch(u8 byConfIdx, u8 byDstNum, TSwitchChannelExt *ptSwitchChannelExt);

    //ֹͣ����
	BOOL32 StopMultiToOneSwitch( u8 byConfIdx, u32 dwDstIp, u16 wDstPort );

	//--------------------ֻ�ղ����Ľ���-----------------------------
	//��mp����ssrc���ӽ�������
	BOOL32 AddRecvOnlySwitch( u8 byConfIdx, u32 dwRcvIp, u16 wRcvPort ,u32 dwRtcpBackIp = 0,u16 wRtcpBackPort = 0, BOOL32 bReverseG7221c = FALSE);
	//��mp����ssrc�Ƴ���������
    BOOL32 RemoveRecvOnlySwitch( u8 byConfIdx, u32 dwRcvIp, u16 wRcvPort );
	//--------------------��ͣ�����ն�����---------------------------
	//��ʼ�����ն�����
	BOOL32 StartRecvMt( const TMt &tMt, u8 byMode );
	//ֹͣ�����ն�����
	BOOL32 StopRecvMt( const TMt &tMt, u8 byMode );
	
	//------------------���ö�������SSRC���Ķ�-------------------------
    //zbq[07/18/2007]�±��漰��SSRC�߼����ɹ�һ����ͳһ����ע��֮
	//���ö�������SSRC���Ķ�
	//BOOL32 ResetRecvMtSSRC( const TMt &tMt, u8 byMode );
	//ȡ����������SSRC���Ķ�
	//BOOL32 RestoreRecvMtSSRC( const TMt &tMt, u8 byMode );
	//��mp����ssrc�������
	//BOOL32 SetRecvMtSSRCValue( u8 byConfIdx, u32 dwRcvIp, u16 wRcvPort, BOOL32 bReset );
	
    //�õ�������Ϣ
	BOOL32 GetSwitchInfo( const TMt & tSrc, u32 &dwSwitchIpAddr, u16 &wSwitchPort, u32 &dwSrcIpAddr );
	
	//�������������ʼ�˿ں�MCU������ʼ�˿�
    BOOL32 GetSwitchInfo( u8 byEqpId, u32 &dwSwitchIpAddr, u16 &wMcuStartPort, u16 &wEqpStartPort );

    void UpdateRRQInfoAfterURQ( u8 byURQConfIdx );


protected:
    
    BOOL32 StartBridgeSwitch(u8 byMode, TMt tSrcMt, u32 dwSrcIp, u32 dwRcvIp, u16 wRcvPort, u32 dwDstIp, BOOL32 bEqpHDBas = FALSE);

	//-----------------------��Ϣ������------------------------
	//����Mpע����Ϣ
	void ProcMpRegReq(const CMessage * pcMsg);	
	//����Mp������Ϣ
	void ProcMpDisconnect(const CMessage * pcMsg);
	//����Mtadpע����Ϣ
	void ProcMtAdpRegReq(const CMessage * pcMsg);	
	//����Mtadp������Ϣ
	void ProcMtAdpDisconnect(const CMessage * pcMsg);

};

API void showmp(void);
API void showmtadp(void);
API void showbridge(void);
API void ssw( u8 byType = 0 );
API void sswi(const s8* pchDstIp = NULL, u16 wDstPort = 0, u8 byConfIdx = 0, BOOL32 bDetail = TRUE);

#endif
