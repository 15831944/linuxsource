/*****************************************************************************
ģ����      : Watchdog
�ļ���      : Watchdog.h
����ʱ��    : 2009�� 02�� 23��
ʵ�ֹ���    : 
����        : �ź��
�汾        : 
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
2009/02/23  1.0         �ź��        ����
******************************************************************************/
#ifndef _WATCHDOGCOMM_H_
#define _WATCHDOGCOMM_H_

#include "kdvtype.h"
#include "kdvdef.h"
#include "osp.h"
#include "eventid.h"

#ifdef WIN32
#define WM_MY_MESSAGE      (WM_USER+100)
#define WM_MY_MESSAGE_QUIT (WM_USER+101)
#define WM_MY_ICONNOTIF    (WM_USER+102) 
#endif

//////////////////////////////// MDSC/HDSC APP ID ///////////////////////////////
#define  WD_SER_APP_ID             (AID_DSC_BGN+1)
#define  WD_CLT_APP_ID             (AID_DSC_BGN+2) 


//////////////////////////////// �궨�� ///////////////////////////////
//���������� 
#define  WD_SER_IP                 inet_addr( "127.0.0.1" )
#define  WD_SER_TCP_PORT           (u16) 3500
#define  WD_SER_TELNET_PORT        (u16) 3501

//////////////////////////////// ��Ϣ���� ///////////////////////////////

/*EV_CS_MODULECON_REQ
  ��������
  ��Ϣ����: (TModuleParameter ��1)
*/
OSPEVENT(WD_CLI_SER_MODULECON_REQ,            (EV_WD8000E_BGN +1  ));
OSPEVENT(WD_SER_CLI_MODULECON_ACK,            (EV_WD8000E_BGN +2  ));
OSPEVENT(EV_SER_CLI_MODULECON_NACK,           (EV_WD8000E_BGN +3  ));

/*EV_CS_GETOTERSTATE_REQ
  mcu����������������Щģ��
  Ŀǰ������mcuʹ��
  ���ݣ�
*/
OSPEVENT(WD_CLI_SER_GETOTERSTATE_REQ,         (EV_WD8000E_BGN +4  ));
OSPEVENT(WD_SER_CLI_GETOTERSTATE_ACK,         (EV_WD8000E_BGN +5  ));
OSPEVENT(WD_SER_CLI_GETOTERSTATE_NOTFY,       (EV_WD8000E_BGN +6  ));

/*EV_CS_SETMODULESTATE_CMD
  ����״̬
  ���ݣ�(TModuleParameter ��1)
*/	
OSPEVENT(WD_CLI_SER_REPORTCLISTATE_REQ,       (EV_WD8000E_BGN +7  ));
OSPEVENT(WD_SER_CLI_REPORTCLISTATE_ACK,       (EV_WD8000E_BGN +8  ));
OSPEVENT(WD_SER_CLI_REPORTCLISTATE_NACK,      (EV_WD8000E_BGN +9  ));


OSPEVENT(WD_SER_CLI_HB_REQ,                   (EV_WD8000E_BGN +10 ));
OSPEVENT(WD_CLI_SER_HB_ACK,                   (EV_WD8000E_BGN +11 ));

OSPEVENT(EV_SER_HB_TIMER,                     (EV_WD8000E_BGN +12 ));
OSPEVENT(EV_SER_RESTART_TIMER,                (EV_WD8000E_BGN +13 ));

OSPEVENT(WD_CLI_GUARD_QUIT,                   (EV_WD8000E_BGN +14 ));
OSPEVENT(WD_CLI_SERV_APPBLOCK,                (EV_WD8000E_BGN +15 ));

//mcuҪ��guard����Ӳ��
OSPEVENT(WD_MCUCLI_GUARD_REBOOT_REQ,          (EV_WD8000E_BGN +16 ));
OSPEVENT(WD_MCUCLI_GAURD_GETKEY_REQ,          (EV_WD8000E_BGN +17 ));
OSPEVENT(WD_GAURD_MCUCLI_GETKEY_ACK,          (EV_WD8000E_BGN +18 ));
OSPEVENT(WD_MCUCLI_GUARD_UPDATE_REQ,          (EV_WD8000E_BGN +19 ));
OSPEVENT(WD_MCUCLI_GUARD_SHUTDOWN_REQ,        (EV_WD8000E_BGN +20 ));
OSPEVENT(WD_MCUCLI_GUARD_SOFTRESTART_REQ,     (EV_WD8000E_BGN +21 )); 

//�ͻ���Ҫ��guard���·�������Ϣ�壺u32(dstIp,������)+u32(mask,������)+u32(gatewayIp,������)+u8(metric)
OSPEVENT(WD_CLI_SER_ADDROUTE_CMD,             (EV_WD8000E_BGN +22 ));
//�ͻ���Ҫ��guardɾ��·�������Ϣ�壺u32(dstIp,������)+u32(mask,������)+u32(gatewayIp,������)
OSPEVENT(WD_CLI_SER_DELROUTE_CMD,             (EV_WD8000E_BGN +23 ));

//��֤����
OSPEVENT(WD_GLOBALAUTH_CMD,				      (EV_WD8000E_BGN +90 )); 
OSPEVENT(WD_GLOBALAUTH_NOTIF,			      (EV_WD8000E_BGN +91 )); 

//��֤��ʱ��
OSPEVENT(WD_GLOBALAUTH_TIMER,			      (EV_WD8000E_BGN +96 )); 

//����ʧ��ԭ��
enum emConnFailReason
{
	emUnknowModule = 0,             //δ֪��ģ������
	emHadConnected  = 1             //�Ѵ�������״̬

};

#endif