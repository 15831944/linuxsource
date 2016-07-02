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

#ifndef MPUVMPHWTEST_H
#define MPUVMPHWTEST_H

#include "mpustruct.h"
#include "eqpcfg.h"
#include "mpuutility.h"
#include "mpuinst.h"
#ifdef _LINUX_
#include "libsswrap.h"

#endif


#define MAXNUM_MPUBAS_CHANNEL        (u8)4

#define MAX_SERIAL_LENGTH	    32
#define MAX_MAC_LENGTH		    32
#define MAX_VERSION_LENGTH	    32


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
����    :  CMpuVmpTestClient
����    :  MPU-VMP �Զ����Կͻ�����
��Ҫ�ӿ�:  
��ע    :  
-------------------------------------------------------------------------------
�޸ļ�¼:  
��  ��     �汾          �޸���          �߶���          �޸ļ�¼
2009-7-16				 Ѧ��							 create							
==============================================================================*/
class CMpuVmpTestClient :public CMpuSVmpInst
{
public:
    CMpuVmpTestClient();
    virtual ~CMpuVmpTestClient();

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
private:
#ifdef _LINUX_
    //�����豸��Ϣ
    void SetMpuDeviceInfo(CDeviceInfo &cDeviceInfo);
#endif	
private:
	u32  m_dwMpuTestDstNode;
	u32  m_dwMpuTestDstInst;
	BOOL m_bConnected;

};

// ����app��  
typedef zTemplate<CMpuVmpTestClient, 1> CMpuVmpTestClientApp;

extern CMpuVmpTestClientApp g_cMpuVmpTestApp;


/*==============================================================================
����    :  CMpuBasTestClient
����    :  MPU-BAS �Զ����Կͻ�����
��Ҫ�ӿ�:  
��ע    :  
-------------------------------------------------------------------------------
�޸ļ�¼:  
��  ��     �汾          �޸���          �߶���          �޸ļ�¼
2009-7-16				 Ѧ��								create
==============================================================================*/
class CMpuBasTestClient :public CMpuBasInst
{
public:
    CMpuBasTestClient();
    virtual ~CMpuBasTestClient();

	void DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp );
    void InstanceEntry( CMessage* const pMsg );

	void DaemonInit( CMessage * const pcMsg );                      //Ӳ����ʼ��
	void DaemonProcConnectTimeOut(void);							//����
	void DaemonRegisterSrv();
	void DaemonProcRegAck( CMessage * const pcMsg ); 
	void DaemonProcOspDisconnect( CMessage* const pMsg );
#ifdef _LINUX_	
	void DaemonProcChangeAutoTestCmd( CMessage* const pMsg );		//�ı���������flag
#endif 	
public:
	BOOL ConnectServ(void);											//connect server 
	void ProcInitBas(void);
	void ProcStartAdpReq(void);
	void RestoreDefault(void);										//�ָ���������ǰĬ��ֵ
	void ChangeTestMode(u8 byMode);									//תģʽ���ԣ���������
private:
	
    //�����豸��Ϣ
#ifdef _LINUX_
    void SetMpuDeviceInfo(CDeviceInfo &cDeviceInfo);
#endif
// private:
// 	u32  m_dwMpuTestDstNode;
// 	u32  m_dwMpuTestDstInst;
// 	BOOL m_bConnected;
	
};

// ����app��  
typedef zTemplate<CMpuBasTestClient, MAXNUM_MPUBAS_CHANNEL, CMpuBasCfg> CMpuBasTestClientApp;

extern CMpuBasTestClientApp g_cMpuBasTestApp;

#endif

//END OF FILE