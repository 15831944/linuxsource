/*****************************************************************************
ģ����      : ������뵥Ԫ
�ļ���      : hdu2test
����ļ�    : 
�ļ�ʵ�ֹ���: hdu2��������
����        : ��־��
�汾        : 4.7  Copyright(C) 2011-2013 KDV, All rights reserved.
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
11/10/31    4.7         ��־��      ����
******************************************************************************/
#ifndef MTHARDWARE_AUTO_TEST_H
#define MTHARDWARE_AUTO_TEST_H

#include "hdu2inst.h"
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
#define CONNECT_TIMER_LENGTH    (3*1000)
#define VERSION_LEN             32
const s8 Host_IpAddr[] = "10.1.1.1";
class CDeviceInfo;
class CHdu2TestClient : public CInstance
{
public:
    CHdu2TestClient();
    virtual ~CHdu2TestClient();
private:

	/* --------- ����ʵ����Ϣ������ -------------*/
	void  DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp );    //����ʵ����Ϣ�����
	void  DaemonProcPowerOn();                  //�ϵ��ʼ��
	void  DaemonProcConnectTimerOut();          //��MCU������Ϣ����
	void  DaemonProcOspDisconnect();            //����������
    void DaemonRestoreDefault(CMessage* const pMsg);                  //����ָ�Ĭ��ֵ������Ϣ

	/* --------- ��ͨʵ����Ϣ������ -------------*/
	void  InstanceEntry( CMessage* const pMsg );                      //��ͨʵ����Ϣ�����
	void  ProcHduChnGrpCreate(CMessage* const pMsg);                  //������ͨ����������Ϣ;
	void  ProcStartSwitchReq( CMessage* const pMsg );                 //��ʼ���뼰��������
	void ProcStopSwitchReq( CMessage* const pMsg );                   //ֹͣ���뼰��������

	/* ---------------- ���ܺ��� ------------------*/
    void SetHduDeviceInfo(CDeviceInfo &cDeviceInfo);                  //�����豸��Ϣ
	BOOL Connect(void);                                               //�����������Է�����
	void LoginTestServer();                                        //���������Է���������ע��
	void StatusShow(void);                                            //��ʾͨ����Ϣ

private:
	CHdu2ChnMgrGrp  m_cHdu2ChnMgrGrp;
	u32  m_dwHduAutoTestDstNode;
	THduModePort    m_tHduModePort;
	enum EHDU2CHNSTATE
	{
		emIDLE,
		emINIT,
		emREADY,
		emRUNNING,
	};
};

class CDeviceInfo
{
private:
    s8  achSerial[MAX_SERIAL_LENGTH];             //���к�
    s8  achMac[MAX_MAC_LENGTH];                   //mac��ַ
    s8  m_achSoftVersion[MAX_VERSION_LENGTH];     //����汾
    s8  m_achHardVersion[MAX_VERSION_LENGTH];     //Ӳ���汾
    u32 m_dwIp;                                   //�豸ip
    u32 m_dwSubMask;                              //��������
public:
    void setSubMask(u32 dwIp)
    {
        m_dwSubMask = dwIp;
    }
    void setSerial(s8 * buf)
    {
        if(buf == NULL) return;
        memset(achSerial,0,MAX_SERIAL_LENGTH);
        int length = (strlen(buf) >= MAX_SERIAL_LENGTH -1 ? MAX_SERIAL_LENGTH - 1: strlen(buf));
        memcpy(achSerial,buf,length);
        achSerial[length] = '\0';
    }
    void setIp(u32 IP)
    {
        m_dwIp = IP;
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
        memset(m_achSoftVersion, 0, MAX_VERSION_LENGTH);
        int length = (strlen(buf) >= MAX_VERSION_LENGTH -1 ? MAX_VERSION_LENGTH - 1 : strlen(buf));
        memcpy(m_achSoftVersion, buf, length);
        m_achSoftVersion[length] = '\0';
    }

    void setHardVersion(s8 * buf)
    {
        if(buf == NULL) return;
        memset(m_achHardVersion, 0, MAX_VERSION_LENGTH);
        int length = (strlen(buf) >= MAX_VERSION_LENGTH -1 ? MAX_VERSION_LENGTH - 1 : strlen(buf));
        memcpy(m_achHardVersion, buf, length);
        m_achHardVersion[length] = '\0';
    }    
};
typedef zTemplate<CHdu2TestClient, MAXNUM_HDU_CHANNEL> CHdu2TestClientApp;
extern CHdu2TestClientApp g_cHdu2TestApp;
#endif
//END OF FILE

