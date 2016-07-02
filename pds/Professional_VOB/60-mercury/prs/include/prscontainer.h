/*****************************************************************************
ģ����      : prs����
�ļ���      : prscontainer.h
����ʱ��    : 2011�� 01�� 25��
ʵ�ֹ���    : 
����        : ��־��
�汾        : 
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
2011/01/25  1.0         ��־��        ����
******************************************************************************/
#ifndef _PRS_CONTAINER_H_
#define _PRS_CONTAINER_H_

#include "kdvtype.h"

#define PRS8KE_CONNECT_INI			( LPCSTR )"connect.ini"
#define PRS8KE_CONFIG_INI			( LPCSTR )"config.ini"

#ifdef WIN32
#define PRS8KELOGFILENAME           ( LPCSTR )"./log/prs8kelog.log"
#else
#define PRS8KELOGFILENAME           ( LPCSTR )"/opt/mcu/log/prs8kelog.log" 
#endif

#ifdef WIN32
#define PRS_KDVLOG_PATH					( LPCSTR )"./log/prslog.log"
#else
#define PRS_KDVLOG_PATH					( LPCSTR )"/opt/mcu/log/prslog.log" 
#endif

void Prs8keStart();						
void Prs8keFileLog( const s8 * pchLogFile, s8 * pszFmt, ... );

#ifdef _LINUX_	
void Prs8keQuitFunc( int nSignalNum );
#endif

struct TPrs8keConnectInfo 
{
private:
	u32		m_dwMcuIp;			//MCU��IP��ַ  (������)
	u16		m_wMcuPort;			//MCU�ļ����˿�(������)

	u32		m_dwLocalIp;		//������IP��ַ
public:
	TPrs8keConnectInfo(){	memset(this,0,sizeof(TPrs8keConnectInfo)); }
	
	void SetMcuIp(u32 dwMcuIp){ m_dwMcuIp = htonl(dwMcuIp); }
	u32  GetMcuIp(){ return ntohl(m_dwMcuIp); }

	void SetMcuPort(u16  wMcuPort){ m_wMcuPort = htons(wMcuPort); }
	u16  GetMcuPort(){ return ntohs(m_wMcuPort); }	

	void SetLocalIp(u32 dwLocalIp){ m_dwLocalIp = htonl(dwLocalIp); }
	u32  GetLocalIp(){ return ntohl(m_dwLocalIp); }	

};

void CreatePrs8keConnectCfg( void );
void GetPrs8keConnectInfo(TPrs8keConnectInfo &tConnectInfo);


#endif