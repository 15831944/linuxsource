/*****************************************************************************
   ģ����      : Board Manager
   �ļ���      : brdmanager.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ���������ඨ��
   ����        : ����
   �汾        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���        �޸�����
   2002/01/25  0.9         ����          ����
   2002/07/29  1.0         ����          �·�������
   2003/11/20  3.0         jianghy       ����
   2004/11/10  3.6         libo          �½ӿ��޸�
   2004/11/29  3.6         libo          ��ֲ��Linux
   2005/08/17  4.0         liuhuiyun     ����
******************************************************************************/
#ifndef BRDMANAGER_H
#define BRDMANAGER_H

#if _MSC_VER > 1000
#pragma once
#endif

#include "mcuagtstruct.h"
#include "agtcomm.h"
#include "mcuconst.h"
#include "evagtsvc.h"
#include "agentcommon.h"
#include "agentmsgcenter.h"

#ifdef WIN32
#pragma pack( push )
#pragma pack(1)
#endif


#ifdef _VXWORKS_
    #define IS_DSL8000_BRD_MPC
    #include "brdDrvLib.h"
    #include "time.h"
    #include "timers.h"
#endif

// ���嵥������������˿ں�  (������,ʵ����MCU�Ľ����˿ں�)
#define BOARD_MANAGER_LISTEN_PORT	8000

#ifdef WIN32
#include "winbrdwrapper.h"
#endif
// BSP�ж���,WINDOWS��ģ��ʱʹ��
//#ifdef WIN32
//struct TBrdPosition
//{
//public:
//    u8 byBrdID;         // ����ID�� 
//    u8 byBrdLayer;      // �������ڲ��
//    u8 byBrdSlot;       // �������ڲ�λ��
//
//public:
//    TBrdPosition(){ Clear();}
//    void Clear(void) { memset( this, 0, sizeof(TBrdPosition) );  }
//    void SetBrdId(u8 byID ) { byBrdID = byID;  }
//    u8   GetBrdId(void) const { return byBrdID; }
//    void SetBrdLayer(u8 byLayer ) { byBrdLayer = byLayer; }
//    u8   GetBrdLayer(void) const { return byBrdLayer; }
//    void SetBrdSlot(u8 bySlot) { byBrdSlot = bySlot; }
//    u8   GetBrdSlot(void) const { return byBrdSlot; }
//    BOOL32 IsSameBrd( TBrdPosition &tBrdPosition)
//    {
//        if ( tBrdPosition.GetBrdId() == byBrdID &&
//             tBrdPosition.GetBrdLayer() == byBrdLayer &&
//             tBrdPosition.GetBrdSlot() == byBrdSlot ) 
//        {
//            return TRUE;
//        }
//        return FALSE;
//    }
//    
//};
//#endif

// MPC2 ֧��
#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
        #include "brdwrapperdef.h"
        #include "nipwrapper.h"
        #include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
#endif

// ��Ҫ���͸������������Ϣ����ṹ
struct UTEqpConfig
{
public:
    UTEqpConfig()
    {
        memset(this, 0, sizeof(UTEqpConfig));
    }
    u8 byEqpType;
	u8 bySubEqpType;

    // �����޷�ʹ��Union
	TEqpTVWallEntry tTVWallCfg;
	TEqpMixerEntry  tMixerCfg;
	TEqpBasEntry    tBasCfg;
	TEqpVMPEntry tVMPCfg;
    TEqpMPWEntry tMpwCfg;
	TEqpPrsEntry tPrsCfg; 
	//4.6�汾 �¼�   jlb
	TEqpHduEntry tHduCfg;
	TEqpSvmpEntry tSvmpCfg;
	TEqpDvmpBasicEntry tDvmpCfg;
	TEqpMpuBasEntry tMpuBasCfg;
	TEqpEbapEntry tEbapCfg;
	TEqpEvpuEntry tEvpuCfg;
		
};

class CEqpConfigBuf
{
private:
    u8 m_abyBuf[4096];

    u8 m_byUseNetSync;
    u8 m_byNetSyncE1Chnl;

    u8 m_byUseWatchDog;

    u32 m_dwIp;
    
    u8 m_byCfgNum;

    // guzh [1/12/2007] 8000B ֧������Mp MtAdp Gk ��
    TDSCModuleInfo m_tDscModule;
	
	// GK ע���ַ, ��dsc��proxy�޸������ļ���, zgc, 2007-08-15
	u32 m_dwGKIp;

	// TLoginInfo, zgc, 2007-10-12
	TLoginInfo m_tLoginInfo;
	
public:
    UTEqpConfig m_utCfg[MAXNUM_BRD_EQP]; 
public:
    CEqpConfigBuf()
    {
        // guzh [1/12/2007] û���麯�����Բſ���memset
        memset(this, 0, sizeof(CEqpConfigBuf));
    }
    ~CEqpConfigBuf()
    {
    }

	u8 GetEqpCfgNum(void)
	{
		return m_byCfgNum;
	}
	void SetEqpCfgNum(u8 val)
	{
        m_byCfgNum = val;
    }

    void SetUseNetSync(BOOL32 bUse, u8 byE1Chnl = 0xFF)
    {
        m_byUseNetSync = bUse ? 1 : 0;
        if (bUse)
        {
            m_byNetSyncE1Chnl = byE1Chnl;
        }
    }

    void SetUseWatchDog(BOOL32 bUse)
    {
        m_byUseWatchDog = bUse ? 1 : 0;
    }

    void SetIpAddr(u32 dwIp)
    {
        m_dwIp = dwIp;
    }

	void SetGKIp(u32 dwIp)
	{
		m_dwGKIp = htonl(dwIp);
	}

    void AddEqpCfg(TEqpTVWallEntry entry)
    {
        m_utCfg[m_byCfgNum].byEqpType = EQP_TYPE_TVWALL;
        m_utCfg[m_byCfgNum].tTVWallCfg = entry;
        m_byCfgNum ++;
    }
    void AddEqpCfg(TEqpMixerEntry entry)
    {
        m_utCfg[m_byCfgNum].byEqpType = entry.GetType();//tianzhiyong  ����EAPU���ͻ�����
        m_utCfg[m_byCfgNum].tMixerCfg = entry;
        m_byCfgNum ++;
    }
    void AddEqpCfg(TEqpBasEntry entry)
    {
        m_utCfg[m_byCfgNum].byEqpType = EQP_TYPE_BAS;
        m_utCfg[m_byCfgNum].tBasCfg = entry;
        m_byCfgNum ++;
    }
    void AddEqpCfg(TEqpVMPEntry entry)
    {
        m_utCfg[m_byCfgNum].byEqpType = EQP_TYPE_VMP;        
        m_utCfg[m_byCfgNum].tVMPCfg = entry;
        m_byCfgNum ++;
    }
    void AddEqpCfg(TEqpMPWEntry entry)
    {
        m_utCfg[m_byCfgNum].byEqpType = EQP_TYPE_VMPTW;
        m_utCfg[m_byCfgNum].tMpwCfg = entry;
        m_byCfgNum ++;
    }
    void AddEqpCfg(TEqpPrsEntry entry)
    {
        m_utCfg[m_byCfgNum].byEqpType = EQP_TYPE_PRS;
        m_utCfg[m_byCfgNum].tPrsCfg = entry;
        m_byCfgNum ++;
    }

	//Ŀǰֻ��APU2 basʹ��[3/22/2013 chendaiwei]
	void AddEqpCfg(TEqpMpuBasEntry entry)
	{
        m_utCfg[m_byCfgNum].byEqpType = EQP_TYPE_BAS;
        m_utCfg[m_byCfgNum].tMpuBasCfg = entry;
		m_utCfg[m_byCfgNum].bySubEqpType = entry.GetStartMode();
        m_byCfgNum ++;
	}

    //4.6�汾 �¼����� jlb
	void AddEqpCfg(TEqpHduEntry entry)
    {
// 		u8 byEqpType = 0;
// 		if (STARTMODE_HDU_H == entry.GetStartMode())
// 		{
// 			byEqpType = EQP_TYPE_HDU_H;
// 		}
// 		else if (STARTMODE_HDU_M == entry.GetStartMode())
// 		{
// 			byEqpType = EQP_TYPE_HDU;
// 		}
// 		else if(STARTMODE_HDU_L == entry.GetStartMode())
// 		{
// 			byEqpType = EQP_TYPE_HDU_L;
// 		}
// 		else if(STARTMODE_HDU2 == entry.GetStartMode())
// 		{
// 			byEqpType = EQP_TYPE_HDU2;
// 		}		
// 		else if(STARTMODE_HDU2_L == entry.GetStartMode())
// 		{
// 			byEqpType = EQP_TYPE_HDU2_L;
// 		}
// 		else
// 		{
// 			OspPrintf(TRUE, FALSE, "[AddEqpCfg][TEqpHduEntry] unexpected hdu type:%d!\n", entry.GetStartMode());
// 			return;
// 		}
		
        m_utCfg[m_byCfgNum].byEqpType = entry.GetType();
        m_utCfg[m_byCfgNum].tHduCfg = entry;
        m_byCfgNum ++;
    }

    void SetDscModuleInfo(const TDSCModuleInfo& tInfo)
    {
        m_tDscModule = tInfo;
    }

	void SetLoginInfo( const TLoginInfo & tLoginInfo )
	{
		memcpy( &m_tLoginInfo, &tLoginInfo, sizeof(m_tLoginInfo) );
	}
    
    u8* GetBuffer(u16 &wBufLen)
    {
        wBufLen = Generate();
        return m_abyBuf;
    }
    
private:
    // ����������Ϣ����� m_abyBuf�����ذ�����
    u16 Generate() ;

};

typedef struct tagRegboard{

    BOOL32   bReg;          // �Ƿ�ע��
    u32      dwNode;        // ����Ľڵ��
    u32      dwDstInsId;    // ʵ��Id
    TBrdPosition tBrdPos;   // ����λ��

    //�����ļ�״̬, Ŀǰֻ����LINUX�����ܶ��������ļ�����Ĳ�ѯ���ش�Լ����
    //m_abyBrdFileStatus[0]->.image, m_abyBrdFileStatus[1]->.linux, ������ʱδʹ�ã�����չ.
    u8       m_abyBrdFileStatus[MAXNUM_FILE_UPDATE];

}TRegBoard;

class CRegedBoard{
public:
    CRegedBoard();
    virtual ~CRegedBoard();

    BOOL32 IsRegedBoard(TBrdPosition tBrdPos, u32 dwNode = INVALID_NODE);
    u16  AddBoard(TBrdPosition tBrdPos, u32 wInsId, u32 dwNode);
    u16  DelBoard(TBrdPosition tBrdPos);
    void SetBinFileStatus(TBrdPosition tBrdPos, u8 byStatus);
    u8   GetBinFileStatus(TBrdPosition tBrdPos);
    void SetImageFileStatus(TBrdPosition tBrdPos, u8 byStatus);
    u8   GetImageFileStatus(TBrdPosition tBrdPos);
    void SetOSFileStatus(TBrdPosition tBrdPos, u8 byStatus);
    u8   GetOSFileStatus(TBrdPosition tBrdPos);
    void ShowRegedBoard(void);
    void ShowSemHandle(void);

private:
    u16				m_wRegedBrdNum;   // �Ѿ�ע�ᵥ����Ŀ
    TRegBoard		m_tBrdReged[MAX_BOARD_NUM];
	SEMHANDLE		m_hBoardManager;  // �������

    //MPC���ļ�״̬��Ŀǰֻ����LINUX�����ܶ��������ļ�����Ĳ�ѯ���ش�Լ����
    //m_abyMpcFileStatus[0]->.image, m_abyMpcFileStatus[1]->.linux, ������ʱδʹ�ã�����չ.
    u8              m_abyMpcFileStatus[MAXNUM_FILE_UPDATE];
};

// ��������ඨ��
class CBoardManager:public CInstance
{
	enum 
	{ 
		STATE_IDLE,			// ����
        STATE_WAITREG,      // �ȴ�ע��
		STATE_INIT,			// ��ʼ��
		STATE_NORMAL,		// ����
	};

public:
	CBoardManager();
	virtual ~CBoardManager();

protected:
	// ʵ����Ϣ������ں���������override
	void InstanceEntry(CMessage* const pMsg);
	void ProcBoardRegMsg(CMessage* const pcMsg);            // ����ע����Ϣ	
	void ProcBoardGetConfigMsg(CMessage* const pcMsg);      // ����ȡ������Ϣ
	void ProcBoardAlarmSyncAck(CMessage* const pcMsg);      // �澯ͬ��Ӧ����Ϣ
	void ProcBoardGetVersionAck(CMessage* const pcMsg);     // ��ȡ�汾��ϢӦ��
	void ProcBoardGetModuleAck(CMessage* const pcMsg);      // ��ȡģ����ϢӦ��
	void ProcMpcCommand(CMessage* const pcMsg);             // �ϲ�Ӧ�õ��������Ϣ
	void ProcBoardAlarmNotify(CMessage* const pcMsg);       // ����ĸ澯֪ͨ��Ϣ
	void ProcBoardLedNotify(CMessage* const pcMsg);         // ����ĵ�״̬֪ͨ
    void ProcBoardEthPortNotify(CMessage* const pcMsg );    // ���������״̬֪ͨ  [2012/05/04 liaokang]
	//void ProcBoardGetStatisticsAck(CMessage* const pcMsg);  // �����ͳ����ϢӦ����Ϣ
	void ProcBoardDisconnect( CMessage* const pcMsg );      // ����Ͽ���Ϣ����
    void ProcBoardUpdateRsp( CMessage* const pcMsg );       // ������������Ӧ
	void ProcBrdConnectTestOverTimeOut( CMessage* const pcMsg );
	void ProcBoardE1BandWidthNotif( CMessage* const pcMsg );//������E1����ָʾ 

	/*void ProcBoardSoftwareVerNotify(CMessage* const pcMsg); // ��������汾��֪ͨ [12/13/2011 chendaiwei]*/

#ifdef _MINIMCU_
	void ProcBoardSetDscInfoRsp( CMessage* const pcMsg );	// ����DSC info����Ļ�Ӧ������, zgc, 2007-07-17
	void ProcDscStartUpdateSoftwareRsp( CMessage* const pcMsg ); //��ʼ��������Ļ�Ӧ, zgc, 2007-08-25
	void ProcDscMpcUpdateFileRsp( CMessage* const pcMsg );		 //�������ݰ��Ļ�Ӧ, zgc, 2007-08-25
#endif

	void ProcBoardConfigModifyNotif( CMessage* const pcMsg );	//���������޸�, zgc, 2007-10-16

	void ProcMpcBrdDisconnectCmd( void );		// MPC����͵������, zgc, 2007-08-28
	
protected:
	BOOL32 PostMsgToBoard( u16 wEvent, u8 * const pbyMsg, u16 wLen );	
	u8   GetWatchDogOption( void );                         // ��ȡWATCH DOG������

	// ��CEqpConfigBuf�ṹ��������Ϣ���, ����FALSE��CEqpConfigBuf�е�������Ч, zgc, 2007-10-17
	BOOL32 PackEqpConfig(CEqpConfigBuf &cEqpCfgBuf, TBoardInfo tBrdInfo);

    void GetMpuEBapEVpuEqpInfo( TBoardInfo &tBoardInfo, TMpuEBapEVpuConfigAck &tMpuEBapEVpuConfigAck );

#if defined(_8KI_) && defined(_LINUX_) 	
	void ProcDetectEthTimer(CMessage* const pcMsg);		//�������״̬
#endif

protected:
	// ���²�����
	void DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp);
    void DaemonProcBrdRegMsg(CMessage* const pMsg, CApp* pcApp);
    void DaemonProcDaemonPowerOn(CApp* pApp);
	void DaemonDumpInst(CApp* pApp);
    //void DaemonProcBrdDisconectMsg(CMessage* const pMsg);   // DAEMON ���������    
	void ProcDaemonGetConfig( void );        // DAEMON��ȡ���ú���(ʹ��ͬ����Ϣ)
	void ProcDaemonManagerCmd( void );      // ��������ת��

	void ClearInst(void);
protected:
	TBrdPosition	m_tBrdPosition;      //����λ�ã������㡢�ۡ�����
	u32			    m_dwBrdNode;		 //��Ӧ����Ľڵ��
	u32			    m_dwBrdIId;			 //�����IID
    u8              m_byOsType;          //os����
};

typedef zTemplate< CBoardManager, MAX_BOARD_NUM, CRegedBoard, 32 > CBrdManagerApp;
extern CBrdManagerApp	g_cBrdManagerApp;	//�������Ӧ��ʵ��

s32 AgtQueryBrdPosition(TBrdPosition *ptBrdPosition);
s32 AgtGetBrdEthParamAll(u8 byEthId, TBrdEthParamAll *ptBrdEthParamAll);
s32 AgtGetBrdEthParam(u8 byEthId, TBrdEthParam *ptBrdEthParam);
u32 AgtGetMpcGateway( u32 dwIfIndex );




#ifdef WIN32
#pragma pack( pop )
#endif

#endif   /*End BRDMANAGER_H*/

