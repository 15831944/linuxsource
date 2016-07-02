/****************************************************************************
  ģ����   ����Խ����ǽ&NAT����Server��                        
  �ļ���   ��FirewallProxyServerLib.h
  ����ļ� ��FirewallProxyServerLib.cpp
  �ļ����� �����崩Խ����ǽ&NAT
             ��������Lib��ʹ��ͷ�ļ�
  ����	   ��Tony
  �汾	   ��1.0
----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2004/12/09    1.0         
******************************************************************************/
#ifndef FIREWALLPROXYSERVERLIB_H_
#define FIREWALLPROXYSERVERLIB_H_

#include "osp.h"

//#define UNIT_TEST
extern u16     UNIT_TEST_TYPE;

//add by gzj.070815 
#define VER_PXYS (const char *)"PxyServer40.00.01.05.130124(CBB)"

// add by gxb 2011-11-24
#define MAX_VIRTUAL_IP_COUNT  (u16)10

//Proxy Server������Ϣ
struct TProxyServerCfg
{
	BOOL32 m_bStartPxy;
    BOOL32 m_bStartMp;
    u32 m_dwGKIpAddr;        //GK��IP��ַ
	u16 m_wGKRasPort;        //GK��Ras�˿�
	u32 m_dwSrvIpAddr;       //PROXY SERVERʹ�õ�IP��ַ
	u16 m_wSrvRasPort;       //PROXY SERVER������˿�
	u16 m_wSrvH245Port;
	u16 m_wOspTcpPort;
    u16 m_wRtpPort;          //RTP�˿�
    u16 m_wRtcpPort;         //RTCP�˿�
    u16 m_wMediaBasePort;    //������ʼ�˿ڵ�ַ
	u32 m_dwSrvDmzIpAddr;	 //���������DMZӳ���ַ
	BOOL m_bUseDmzAddr;		 //�Ƿ����ô��������DMZӳ��
	BOOL32	m_b323UseSpecialIP; //323ָ��ʹ��m_dwSrvIpAddr�ĵ�ַ
	u16 m_wVirtualIPNum;                         //����IP������
	u32 m_dwVirtualIPAddr[MAX_VIRTUAL_IP_COUNT]; //����IP�ĵ�ַ
	BOOL32 m_bIsForceGKRoute;                    //�Ƿ�ʹ��GK·�� 2013/05/09
};

extern u16 InitPxy();
extern u16 InitPxyServer(TProxyServerCfg &tCfg);
extern void UnInitPxyServer();


#endif

