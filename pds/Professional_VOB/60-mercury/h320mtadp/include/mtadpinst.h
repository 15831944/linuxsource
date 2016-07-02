/*****************************************************************************
ģ����      : h320�ն�����ģ��
�ļ���      : MtAdpInst.h
����ļ�    : MtAdpInst.cpp
�ļ�ʵ�ֹ���: ʵ��ģ��
����        : ������
�汾        : V3.6  Copyright(C) 1997-2003 KDC, All rights reserved.
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
2005/06/07  3.6         ������      ����
******************************************************************************/

#ifndef __MTADPINST_H
#define __MTADPINST_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mtadpcom.h"

class CMtAdpNet;
class CMtAdpInst : public CInstance 
{
    enum 
    {
        STATE_IDLE = 0,                     //����
        STATE_CALLING,                      //�ն��������
        STATE_NORMAL                        //�ն˳ɹ����
    };

public:
	CMtAdpInst();
	virtual ~CMtAdpInst();

//////////////////////////////////////////////////////////////////////////
    //ʵ�����
    void InstanceEntry(CMessage *const pcMsg);

    //mcu�����ն������������
    void OnMcuInviteMtReq(const CMessage *pcMsg);    

    //mcu�����ն���ᳬʱ������
    void OnInviteMtTimeout(const CMessage *pcMsg);

    //Э��ջ״̬����
    void OnH320StackState(const CMessage *pcMsg);

    //Э��ջԶң����
    void OnH320StackFecc(const CMessage *pcMsg);

    //Э��ջcmd����
    void OnH320StackCmd(const CMessage *pcMsg); 

    //�յ����˸���ģʽ����
    void OnLocalMuxMode(const TKDVH320MuxMode *ptMuxMode);

    //mcu ���߼��ŵ�������Ӧ
    void OnMcuMtOpenLogicChanRsp(const CMessage *pcMsg);

    //mcu ���߼��ŵ�����
    void OnMcuMtOpenLogicChanReq(const CMessage *pcMsg);

    //mcu �����ŵ�����
    void OnMcuMtChanOnCmd(const CMessage *pcMsg);

    //mcu ֹͣ�ŵ�����
    void OnMcuMtChanOffCmd(const CMessage *pcMsg);

    //mcu �ر��ŵ�����
    void OnMcuMtCloseChanCmd(const CMessage *pcMsg);

    //flow control
    void OnFlowCtrlCmd(const CMessage *pcMsg);

    //��֪ѡ���ն�Դ
    void OnYouAreSeeingNotif(const CMessage *pcMsg);

    void OnGetBitrateInfo(const CMessage *pcMsg);

    //ǿ�ƹؼ�֡
    void OnMcuMtFastUpdatePicCmd(const CMessage *pcMsg);

    //ָ��/ȡ����ϯ
    void OnMcuMtChairNotif(const CMessage *pcMsg);
    
    //ָ��/ȡ������
    void OnMcuMtSpeakerNotif(const CMessage *pcMsg);

    //ָ��/ȡ��ѡ��
    void OnMcuMtSelNotif(const CMessage *pcMsg);
    //ѡ����Ӧ����
    void OnMcuMtSelRsp(const CMessage *pcMsg);

    //����/����
    void OnMcuMtMuteDumbCmd(const CMessage *pcMsg);

    //�ն��б���Ӧ
    void OnMcuMtMtListRsp(const CMessage *pcMsg);

    //�ն˱�����Ӧ
    void OnMcuMtGetMtAliasRsp(const CMessage *pcMsg);

    //�ն�������֪ͨ
    void OnMcuMtOnlineChangeNotif(const CMessage *pcMsg);

    void OnConfInfoNotif(const CMessage *pcMsg);

    //Զң����
    void OnMcuMtFeccCmd(const CMessage *pcMsg);

    //sel or speaker
    void OnSetInReq(const CMessage *pcMsg);

    //assign tia
    void AssignTia(TMt &tMt);

    u8  GetRemoteMcuId();
    u8  GetLocalMcuId();    
   
    //send iis
    void OnSendCmdIIS();

    //release lsd token
    void OnReleaseLsdToken();

    //������Ϣ����
    void OnMcuMtPrivateMsg(const CMessage *pcMsg);

    //������Ϣ��ӡ
    void OnPrintSwitchInfo(const CMessage *pcMsg);
   
//////////////////////////////////////////////////////////////////////////    
    //Daemon ʵ�����
    void DaemonInstanceEntry(CMessage *const pcMsg, CApp *pcApp);

    //�ϵ紦��
    void OnDaemonPowerOn();
    //����ģ����mcuע�ἰ��Ӧ������
    void OnDaemonRegisterMcuReq(const CMessage *pcMsg, BOOL32 bMcuA);
    void OnDaemonRegisterMcuRsp(const CMessage *pcMsg);

    //��������
    void OnDaemonOspDisconnect(const CMessage *pcMsg, CApp *pcApp);

    //��������״̬��Ӧ
    void DaemonProcGetMsStatusRsp(const CMessage *pcMsg, CApp *pcApp);

    //mcu�����ն�Daemon����
    void OnDaemonMcuInviteMtReq(const CMessage *pcMsg);

    void OnDaemonDelMtCmd(const CMessage *pcMsg, CApp *pcApp);

    //����mcu��mt������Ϣ��ʵ���ַ�
    void OnDaemonMcuMtGeneral(const CMessage *pcMsg, CApp *pcApp);

    //�ַ���Ϣ����������ն�
    void OnDaemonBroadMsg2AllMt(const CMessage *pcMsg);

//////////////////////////////////////////////////////////////////////////
    //���ʵ��
    void ClearInst();

    //����Ϣ��mcu
    void SendMsg2Mcu(CServMsg &cServMsg);

    //Ϊ��ʵ����Ӧ���ն˷��������շ���ʼ�˿�
    u16 AssignSwitchPort();    

    //���������ļ���Ϣ
    void LoadConfig();

    //������Ϣ��ӡ���
    void MtAdpLog(u8 byLevel, s8 * pInfo, ...);

//////////////////////////////////////////////////////////////////////////
    //H320 cmd dealing
    void OnCmdCIS();
    void OnCmdCCA();
    void OnCmdIIS(u8 *pbyParam, u16 wParamLen);
    void OnCmdTIA(u8 *pbyParam, u16 wParamLen);
    void OnCmdMCC(u8 *pbyParam, u16 wParamLen);
    void OnCmdTIN(u8 *pbyParam, u16 wParamLen);
    void OnCmdTID(u8 *pbyParam, u16 wParamLen);
    void OnCmdTIL(u8 *pbyParam, u16 wParamLen);
    void OnCmdTIP(u8 *pbyParam, u16 wParamLen);
    void OnCmdTCP(u8 *pbyParam, u16 wParamLen);
    void OnCmdTIR(u8 *pbyParam, u16 wParamLen);
    void OnCmdVCB(u8 *pbyParam, u16 wParamLen);
    void OnCmdCancelVCB();
    void OnCmdVCS(u8 *pbyParam, u16 wParamLen);
    void OnCmdCancelVCS();
    void OnCmdDCAL();
    void OnCmdDISL();
    void OnCmdVCU();
    void OnCmdVIN(u8 *pbyParam, u16 wParamLen);
    void OnCmdTCU();

public:

    CKdvH320          * m_pcH320Stack;                  //h320 stack
    CMtAdpNet         * m_pcMtAdpNet;                   //net rcv & snd

    TMtAlias            m_tMtAlias;                     //mt alias
    u8                  m_byMtId;                       //mt id
    u8                  m_byConfIdx;                    //conf idx
    CConfId             m_cConfId;                      //conf id
    TMtAlias            m_tConfName;                    //conf name

    TKDVH320MuxMode     m_tLocalMuxMode;                //���˸���ģʽ
    TKDVH320MuxMode     m_tRemoteMuxMode;               //Զ�˸���ģʽ
    TKdvH320CapSet      m_tLocalCapSet;                 //����������

    u16                 m_wVideoRate;                   //��Ƶ����(kbps)
    u16                 m_wConfRate;                    //ʱ϶����

    //AUD_RCV_INDEX-��Ƶ�գ�VID_RCV_INDEX-��Ƶ��
    //AUD_SND_INDEX-��Ƶ����VID_SND_INDEX-��Ƶ��
    TChannel            m_atChannel[MAXNUM_CHAN];       //�߼��ŵ�

    u8                  m_byLocalMcuId;                 //master mcu �����slave mcu��mcu id
    BOOL32              m_bRemoteTypeMcu;               //�Զ��Ƿ�mcu(0-mt,1-mcu)
    //master or slave (load from config file) 0-slave, 1-master, only used when remote is mcu
    BOOL32              m_bMasterMcu;
    BOOL32              m_bChairMt;                     //�Ƿ���ϯ�ն�
    BOOL32              m_bAssignMtId;                  //�Ƿ��Ѿ������ն�id
    TMt                 m_tSpySrcMt;                    //��ǰ�ش����ն�Դ
    TMt                 m_tVinMt;                       //vin mt    
    TConfMtInfo         m_tSMcuMtList;                  //�Զ˴�mcu���ն��б�
    TH320TIR            m_tConfTir;                     //lsd, hsd, chair
    BOOL32              m_bHasLsdToken;                 //�Ƿ�ӵ��lsd����
    u16                 m_wLsdTokenHoldTime;         //Զң�����ͷų�ʱʱ��
    BOOL32              m_bReceivedIIS;                 //�Ƿ��Ѿ��յ�IIS

    BOOL32              m_bBchVidLoopback;

};

class CMtAdpData
{
public:
    CMtAdpData();
    ~CMtAdpData();

public:
    u32     m_dwMcuANode;                        //mcu A �ڵ��
    u32     m_dwMcuAIId;                         //mcu A ʵ��id
    u8      m_byMcuAId;                          //mcu A Id

    // ���� [6/9/2006] ˫��
    u32     m_dwMcuBNode;                        //mcu B �ڵ��
    u32     m_dwMcuBIId;                         //mcu B ʵ��id
    u8      m_byMcuBId;                          //mcu B Id

    BOOL32  m_bDoubleLink;                       // Mcu Linking mode ( 1-DoubleLink )

    u32     m_dwLocalIP;                        //����IP��ַ(net order)
    u8      m_byDriId;                          //Dri��id
    s8      m_achMtAdpAlias[32];                //����

    //ʵ���ţ�ͨ�������նˣ�confid��mtid��ӳ���ϵ
    u8      m_abyMt2InsMap[MAXNUM_MCU_CONF+1][MAXNUM_CONF_MT+1];
    
    u8      m_byLogLevel;                       //������Ϣ��ӡ����

};

typedef zTemplate< CMtAdpInst, MAXNUM_CHAN, CMtAdpData > CMtAdpApp;

extern  CMtAdpApp   g_cMtAdpApp;

#endif // __MTADPINST_H
