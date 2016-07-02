 /*****************************************************************************
   ģ����      : hduhardwate auto test client
   �ļ���      : hduhwautotest.h
   ����ļ�    : hduhwautotest.cpp
   �ļ�ʵ�ֹ���: HDUӲ���Զ�������
   ����        : 
   �汾        : V4.6.2  Copyright(C) 2009-2015 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2009/04/21  1.0         ���ֱ�        ���� 
******************************************************************************/

#ifndef MTHARDWARE_AUTO_TEST_H
#define MTHARDWARE_AUTO_TEST_H

#include "hduinst.h"
#include "osp.h"
#include "kdvtype.h"

#ifdef _LINUX_ 


#include "nipwrapper.h"
#include "nipwrapperdef.h"
#include "brdwrapperdef.h"
#endif

#ifdef _LINUX12_
#include "brdwrapper.h"
#endif

#define MAX_SERIAL_LENGTH	    32
#define MAX_MAC_LENGTH		    32
#define MAX_VERSION_LENGTH	    32


class CDeviceInfo;

class CHduAutoTestClient : public CHduInstance
{
public:
    CHduAutoTestClient();
    virtual ~CHduAutoTestClient();

	void DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp );
    void InstanceEntry( CMessage* const pMsg );
	// ��ʼ��
	void DaemonInit();
    // ����
    void DaemonProcConnectServerCmd();
    // ע���Ӧack
	void DaemonProcRegAckRsp();
	// ע���Ӧnack
	void DaemonProcRegNackRsp();
    // ��������
	void DaemonDisconnect();
    // ��ʼ����
	void DaemonProcStartSwitchReq( CMessage* const pMsg );
	// ֹͣ����
	void DaemonProcStopSwitchReq( CMessage* const pMsg );
	// ����ָ�Ĭ��ֵ������Ϣ
#ifndef WIN32
    void DaemonRestoreDefault(CMessage* const pMsg);
	// �ı��Զ����Ա�־
	void DaemonProcChangeAutoTestCmd( CMessage* const pMsg );
#endif
    // ��ʼ����
	void ProcStartSwitchReq( CMessage* const pMsg );
	// ֹͣ����
	void ProcStopSwitchReq();
	// ��ʼ��hduͨ��
	void ProcInitChnl();

 
private:

    //�����豸��Ϣ
    void SetHduDeviceInfo(CDeviceInfo &cDeviceInfo);
	BOOL ConnectAndLoginTestServer(void);

private:
	u32  m_dwHduAutoTestDstNode;
	u32  m_dwHduAutoTestDstInst;
	BOOL m_bConnected;

};

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

    void setSubMask(u32 dwIp)
    {
        dwSubMask = dwIp;
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

// ����app��  
typedef zTemplate<CHduAutoTestClient, MAXNUM_HDU_CHANNEL> CHduAutoTestClientApp;

extern CHduAutoTestClientApp g_cHduAutoTestApp;

//autotest ���Խӿ�
void AutoTestLog(s8 *, ...);

extern BOOL b_gPAutoTestMsg;

#endif

//END OF FILE

