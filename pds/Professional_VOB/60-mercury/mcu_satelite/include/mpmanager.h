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

class CMpManager
{
public:
	CMpManager();
	virtual ~CMpManager();

public:	
    //-------------------------Multicast-------------------------------
	//��ʼ�鲥
	BOOL32 StartMulticast( const TMt & tSrc, u8 bySrcChnnl = 0, u8 byMode = MODE_BOTH, BOOL32 bConf = TRUE );
	//ֹͣ�鲥
	BOOL32 StopMulticast( const TMt & tSrc, u8 bySrcChnnl = 0, u8 byMode = MODE_BOTH, BOOL32 bConf = TRUE );
    
    //-------------------------Distributed Conf Multicast--------------
    //��ʼ��ɢ�����鲥
    BOOL32 StartDistrConfCast( const TMt &tSrc, u8 byMode = MODE_BOTH, u8 bySrcChnnl = 0) ;
    //ֹͣ��ɢ�����鲥
    BOOL32 StopDistrConfCast ( const TMt &tSrc, u8 byMode = MODE_BOTH, u8 bySrcChnnl = 0) ;

	//-------------------------Satelite Conf Mul/Unicast---------------
	//��ʼ���ǻ����鲥�����������С��鲥���У�
	BOOL32 StartSatConfCast( const TMt &tSrc, u8 byCastType, u8 byMode = MODE_BOTH, u8 bySrcChnnl = 0 ) ;
	//ֹͣ���ǻ����鲥�����������С��鲥���У�
	BOOL32 StopSatConfCast ( const TMt &tSrc, u8 byCastType, u8 byMode = MODE_BOTH, u8 bySrcChnnl = 0 ) ;

	//��ʼ���ǻ��鵥�������������С��������У�
	BOOL32 StartSwitchSrc2Dst( const TMt &tSrc, const TMt &tDst, u8 byMode = MODE_BOTH, u8 bySrcChnnl = 0 );
	//ֹͣ���ǻ��鵥�������������У��������У�
	BOOL32 StopSwitchSrc2Dst ( const TMt &tSrc, const TMt &tDst, u8 byMode = MODE_BOTH, u8 bySrcChnnl = 0 );

    //------------------------Switch Bridge----------------------------
	//����������
	BOOL32 SetSwitchBridge( const TMt & tSrc, u8 bySrcChnnl = 0, u8 byMode = MODE_BOTH, BOOL32 bEqpHDBas = FALSE );
	//�Ƴ�������
	BOOL32 RemoveSwitchBridge( const TMt & tSrc, u8 bySrcChnnl = 0, u8 byMode = MODE_BOTH, BOOL32 bEqpHDBas = FALSE );

	//------------------------------Mt----------------------------------
	//��ָ���ն����ݽ������¼��ն�
	BOOL32 StartSwitchToSubMt( const TMt & tSrc,
							   u8 bySrcChnnl,
							   const TMt & tDst,
							   u8 byMode = MODE_BOTH,
							   u8 bySwitchMode = SWITCH_MODE_BROADCAST,
							   BOOL32 bH239Chnnl = FALSE,
                               BOOL32 bStopBeforeStart = TRUE,
							   BOOL32 bSrcHDBas = FALSE);

	//ֹͣ�����ݽ�����ֱ���¼����ն�
	void StopSwitchToSubMt( const TMt & tDst, u8 byMode = MODE_BOTH, BOOL32 bH239Chnnl = FALSE );
	//ֹͣ�����ն�����
    // ���� [4/30/2006] ����������ʹ�á�����ds ��RecvOnly����
	//void StopRecvSubMt( const TMt & tMt, u8 byMode );

    // guzh [3/21/2007] �����㲥����
    BOOL32 StartSwitchToBrd(const TMt &tSrc, u8 bySrcChnnl, BOOL32 bForce);
    BOOL32 StartSwitchFromBrd(const TMt &tSrc, u8 bySrcChnnl, u8 byDstMtNum, const TMt* const ptDst);
    BOOL32 StartSwitchEqpFromBrd(const TMt &tSrc, u8 bySrcChnnl, u8 byEqpId, u16 wDstChnnl, u8 bySwitchMode);
    // guzh [3/23/2007] ɾ���㲥Դ
    BOOL32 StopSwitchToBrd(const TMt &tSrc, u8 bySrcChnnl);
    
	//------------------------------Mc---------------------------------
	//��ָ���ն����ݽ��������
	BOOL32 StartSwitchToMc( const TMt & tSrc, u8 bySrcChnnl, u16 wMcInstId, u8 byDstChnnl = 0, u8 byMode = MODE_BOTH );
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
								 u8 bySrcChnnl,
								 u8 byEqpId,
								 u16 wDstChnnl = 0, 
								 u8 byMode = MODE_BOTH,
								 u8 bySwitchMode = SWITCH_MODE_BROADCAST,
								 BOOL32 bEqpHDBas = FALSE,
								 BOOL32 bStopBeforeStart = TRUE,
								 BOOL32 bVCSConf = FALSE);
	//ֹͣ�����ݽ���������
	void StopSwitchToPeriEqp( u8 byConfIdx, u8 byEqpId, u16 wDstChnnl = 0,  u8 byMode = MODE_BOTH ); 	


	//��Ļ��ӡ����Mp��Ϣ
	void ShowMp();
	//��Ļ��ӡ���н�����
	void ShowBridge();
	//��Ļ��ӡ���н�����Ϣ
	void ShowSwitch( u8 byType );
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
                     BOOL32 bStopBeforeStart = TRUE);
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
                     BOOL32 bStopBeforeStart = TRUE);
	//ֹͣ����
	BOOL32 StopSwitch( u8 byConfIdx, const TLogicalChannel& tFwdChnnl, u16 wDstChnnl = 0, 
		               u8 byIsSwitchRtcp = 1, u8 byMulPortSpace = 1);
	
	//--------------------��㵽һ��Ľ���---------------------------
	//���ӽ���
	BOOL32 AddMultiToOneSwitch( u8 byConfIdx, u32 dwSrcIp, u32 dwDisIp, u32 dwRcvIp, u16 wRcvPort, 
		                      u32 dwDstIp, u16 wDstPort,u32 dwRcvBindIP = 0, u32 dwSndBindIP = 0, 
							  u32 dwMapIpAddr = 0, u16 wMapPort = 0 );
	//�Ƴ�����
	BOOL32 RemoveMultiToOneSwitch( u8 byConfIdx, u32 dwRcvIp, u16 wRcvPort, u32 dwDstIp, u16 wDstPort );
    //ֹͣ����
	BOOL32 StopMultiToOneSwitch( u8 byConfIdx, u32 dwDstIp, u16 wDstPort );

	//--------------------ֻ�ղ����Ľ���-----------------------------
	//��mp����ssrc���ӽ�������
	BOOL32 AddRecvOnlySwitch( u8 byConfIdx, u32 dwRcvIp, u16 wRcvPort );
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

#endif
