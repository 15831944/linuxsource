/*****************************************************************************
ģ����      : 8kiBas����
�ļ���      : bas8ki.h
����ʱ��    : 2012�� 04�� 23��
ʵ�ֹ���    : 
����        : ��־��
�汾        : 
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
2012/04/23  1.0         ��־��        ����
******************************************************************************/
#ifndef _BAS_8KI_H_
#define _BAS_8KI_H_

#include "kdvtype.h"

#define BAS8KI_CONNECT_INI			( LPCSTR )"connect.ini"

#ifdef WIN32
#define BAS8KI_KDVLOG_PATH					( LPCSTR )"./log/bas8ki.ini"
#else
#define BAS8KI_KDVLOG_PATH					( LPCSTR )"/usr/etc/config/conf/bas8ki.ini" 
#endif

void Bas8kiStart();						

#ifdef _LINUX_	
void Bas8kiQuitFunc( int nSignalNum );
#endif

struct TBas8KIConnectInfo 
{
private:
	u32		m_dwMcuIp;			//MCU��IP��ַ  (������)
	u16		m_wMcuPort;			//MCU�ļ����˿�(������)

	u32		m_dwLocalIp;		//������IP��ַ
public:
	TBas8KIConnectInfo(){	memset(this,0,sizeof(TBas8KIConnectInfo)); }
	
	void SetMcuIp(u32 dwMcuIp){ m_dwMcuIp = htonl(dwMcuIp); }
	u32  GetMcuIp(){ return ntohl(m_dwMcuIp); }

	void SetMcuPort(u16  wMcuPort){ m_wMcuPort = htons(wMcuPort); }
	u16  GetMcuPort(){ return ntohs(m_wMcuPort); }	

	void SetLocalIp(u32 dwLocalIp){ m_dwLocalIp = htonl(dwLocalIp); }
	u32  GetLocalIp(){ return ntohl(m_dwLocalIp); }	

};

void CreateBas8kiConnectCfg( void );
void GetBas8kiConnectInfo(TBas8KIConnectInfo &tConnectInfo);


#endif