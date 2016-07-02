 /*****************************************************************************
   ģ����      : mpuhardware (vmp/bas) auto test client 
   �ļ���      : mpuvmphwtest.h, 
   ����ļ�    : mpuvmphwtest.cpp, mpubashwtest.cpp
   �ļ�ʵ�ֹ���: MPU�Զ�������
   ����        : 
   �汾        : V4.6.2  Copyright(C) 2009-2015 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2009/05/27  1.0         Ѧ��        ���� 
******************************************************************************/

#ifndef MPU2VMPHWTEST_H
#define MPU2VMPHWTEST_H

#include "mpu2struct.h"
#include "eqpcfg.h"
#include "mpu2inst.h"
#include "mpu2utility.h"
#ifdef _LINUX_
#include "libsswrap.h"

#endif


#define MAXNUM_MPUBAS_CHANNEL        (u8)4

#define MAX_SERIAL_LENGTH	    32
#define MAX_MAC_LENGTH		    32
#define MAX_VERSION_LENGTH	    32

#define MAX_IPSTRING_LEN           64

class CDeviceInfo
{
private:
    s8          achSerial[MAX_SERIAL_LENGTH];           //���к�
    s8          achMac[MAX_MAC_LENGTH];                 //mac��ַ
    s8          achSoftVersion[MAX_VERSION_LENGTH];     //����汾
    s8          achHardVersion[MAX_VERSION_LENGTH];     //Ӳ���汾
    u32         dwIp;                                   //�豸ip
    u32         dwSubMask;                              //��������
public:
    u32 getSubMask()
    {
        return dwSubMask;
    }
	
    void setSubMask(u32 dwMask)
    {
        dwSubMask = dwMask;
    }
	
    void setSerial(s8 * buf)
    {
        if(buf == NULL) return;
        memset(achSerial,0,MAX_SERIAL_LENGTH);
        int length = (strlen(buf) >= MAX_SERIAL_LENGTH -1 ? MAX_SERIAL_LENGTH - 1: strlen(buf));
        memcpy(achSerial,buf,length);
        achSerial[length] = '\0';
    }    
    s8* getSerial()
    {
        return achSerial;
    }
    
    void setIp(u32 IP)
    {
        dwIp = IP;
    }
    
    u32 getIp()
    {
        return dwIp;
    }
	
    s8* getMac()
    {
        return achMac;
    } 
    void setMac(s8* pchbuf)
    {
        if(pchbuf == NULL) return;
        memset(achMac,0,MAX_VERSION_LENGTH);
        int length = (strlen(pchbuf) >= MAX_MAC_LENGTH -1 ? MAX_MAC_LENGTH - 1: strlen(pchbuf));
        memcpy(achMac, pchbuf, length);
    }
	
    void setSoftVersion(s8* buf)
    {
        if(buf == NULL) return;
        memset(achSoftVersion, 0, MAX_VERSION_LENGTH);
        int length = (strlen(buf) >= MAX_VERSION_LENGTH -1 ? MAX_VERSION_LENGTH - 1 : strlen(buf));
        memcpy(achSoftVersion, buf, length);
        achSoftVersion[length] = '\0';
    }
    s8* getSoftVersion()
    {
        return achSoftVersion;
    }
    void setHardVersion(s8 * buf)
    {
        if(buf == NULL) return;
        memset(achHardVersion, 0, MAX_VERSION_LENGTH);
        int length = (strlen(buf) >= MAX_VERSION_LENGTH -1 ? MAX_VERSION_LENGTH - 1 : strlen(buf));
        memcpy(achHardVersion, buf, length);
        achHardVersion[length] = '\0';
    }    
    s8* getHardVersion()
    {
        return achHardVersion;
    }
};

/*==============================================================================
����    :  CMpu2VmpTestClient
����    :  MPU-VMP �Զ����Կͻ�����
��Ҫ�ӿ�:  
��ע    :  
-------------------------------------------------------------------------------
�޸ļ�¼:  
��  ��     �汾          �޸���          �߶���          �޸ļ�¼
2009-7-16				 Ѧ��							 create							
==============================================================================*/
class CMpu2VmpTestClient : public CInstance
{
public:
    CMpu2VmpTestClient();
    virtual ~CMpu2VmpTestClient()
	{
		Clear();
	}

	void DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp );
    void InstanceEntry( CMessage* const pMsg );
	void Init( CMessage * const pcMsg );                             //Ӳ����ʼ��
	void MsgRegAckProc( CMessage * const pcMsg );                    //ȷ��
	void MsgRegNackProc( CMessage * const pcMsg );                   //�ܾ�
    BOOL ConnectServ();												//connect server 
	void RegisterSrv();
	void ProcRegAck( CMessage * const pcMsg ); 
	void ProDisconnect( CMessage * const pcMsg );
	void RestoreDefault(void);
	void ChangeTestMode(u8 byMode);									//תģʽ���ԣ���������
	void DaemonProcChangeAutoTestCmd( CMessage* const pMsg );		//�ı���������flag
	
	void Clear();
	BOOL32 InitMediaRcv(u8 byChnnl);
	void StartNetRcvSnd();
	void InitMediaSnd( u8 byChnNo );
	BOOL32 SetMediaSndNetParam(u8 byChnNo);
	BOOL32 SetMediaRcvNetParam( u8 byChnNo);
	void MsgFastUpdatePicProc( CMessage * const pMsg );
	void ProcTestLedCmd();           //�����յ��������������Ĳ��Ե���Ϣ
	void TimerTestLed(CMessage* const pMsg);
	void Proc8KIStartVmpTestReq(CMessage* const pMsg);
	void PrepareToTest();
private:
#ifdef _LINUX_
    //�����豸��Ϣ
    void SetMpuDeviceInfo(CDeviceInfo &cDeviceInfo);
#endif	
public:
		CHardMulPic   m_cHardMulPic; 

private:
	u32  m_dwMpuTestDstNode;
	u32  m_dwMpuTestDstInst;
	TEqpBasicCfg       m_tCfg;            // ��������

	CKdvMediaRcv* m_pcMediaRcv[MAXNUM_MPU2VMP_MEMBER];   // 25������
	CKdvMediaSnd* m_pcMediaSnd[MAXNUM_MPU2VMP_CHANNEL];  // ���9·���� 
	TMpuVmpCfg    m_tMpuVmpCfg;
	u32           m_dwServerRcvIp;      // Server���յ�ַ
    u16           m_wServerRcvStartPort;// Server������ʼ�˿ں�
	BOOL32        m_bAddVmpChannel[MAXNUM_MPU2VMP_MEMBER];// ��¼��ͨ���Ƿ��Ѿ����
	u8            m_byHardStyle;// ����ϳɷ�񣨵ײ��õģ�
	u32           m_adwLastFUPTick[MAXNUM_MPU2VMP_CHANNEL];   // ��һ���յ��ؼ�֡�����Tick��
	u8            m_abyLedId[MPU2_LED_NUM]; //mpu2��ĵ�ID
	u8            m_by8KICurTestCard;        //8kI�������Ե�ǰ���ڲ�İ���
};


class CMpu2VmpTestCfg:public CMpu2VmpCfg
{
public:
	CMpu2VmpTestCfg() 
	{
		memset(m_achServerIp,0,sizeof(m_achServerIp));
		memcpy(m_achServerIp,achServIpAddr,min(sizeof(achServIpAddr),sizeof(m_achServerIp)) );
	}
	virtual ~CMpu2VmpTestCfg() {}
	
public:
		s8 m_achServerIp[MAX_IPSTRING_LEN];//����serverip���������û�����api����serverip
	
};

// ����app��  
typedef zTemplate<CMpu2VmpTestClient, 1,CMpu2VmpTestCfg> CMpu2VmpTestClientApp;

extern CMpu2VmpTestClientApp g_cMpuVmpTestApp;

//
///*==============================================================================
//����    :  CMpuBasTestClient
//����    :  MPU-BAS �Զ����Կͻ�����
//��Ҫ�ӿ�:  
//��ע    :  
//-------------------------------------------------------------------------------
//�޸ļ�¼:  
//��  ��     �汾          �޸���          �߶���          �޸ļ�¼
//2009-7-16				 Ѧ��								create
//==============================================================================*/
//class CMpuBasTestClient :public CMpuBasInst
//{
//public:
//    CMpuBasTestClient();
//    virtual ~CMpuBasTestClient();
//
//	void DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp );
//    void InstanceEntry( CMessage* const pMsg );
//
//	void DaemonInit( CMessage * const pcMsg );                      //Ӳ����ʼ��
//	void DaemonProcConnectTimeOut(void);							//����
//	void DaemonRegisterSrv();
//	void DaemonProcRegAck( CMessage * const pcMsg ); 
//	void DaemonProcOspDisconnect( CMessage* const pMsg );
//#ifdef _LINUX_	
//	void DaemonProcChangeAutoTestCmd( CMessage* const pMsg );		//�ı���������flag
//#endif 	
//public:
//	BOOL ConnectServ(void);											//connect server 
//	void ProcInitBas(void);
//	void ProcStartAdpReq(void);
//	void RestoreDefault(void);										//�ָ���������ǰĬ��ֵ
//	void ChangeTestMode(u8 byMode);									//תģʽ���ԣ���������
//private:
//	
//    //�����豸��Ϣ
//#ifdef _LINUX_
//    void SetMpuDeviceInfo(CDeviceInfo &cDeviceInfo);
//#endif
//// private:
//// 	u32  m_dwMpuTestDstNode;
//// 	u32  m_dwMpuTestDstInst;
//// 	BOOL m_bConnected;
//	
//};
//
//// ����app��  
//typedef zTemplate<CMpuBasTestClient, MAXNUM_MPUBAS_CHANNEL, CMpuBasCfg> CMpuBasTestClientApp;
//
//extern CMpuBasTestClientApp g_cMpuBasTestApp;

#endif //MPU2VMPHWTEST_H

//END OF FILE