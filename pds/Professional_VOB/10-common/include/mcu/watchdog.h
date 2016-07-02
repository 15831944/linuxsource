#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

#include "kdvtype.h"
#include "kdvdef.h"
#include "osp.h"
#include "eventid.h"

//////////////////////////////////////////////////////////////////////////
//WDCLT 
#define  WD_CLT_APP_ID             (AID_DSC_BGN+2) 
//////
enum TMType
{ 
	em_INVALIDMODULE   = -1,           //δ֪ģ��
	em_TS              = 0,            //TS
	em_GK              = 1,            //GK
	em_PROXY           = 2,            //PROXY
    em_MMP             = 3,            //MMP
	em_MCU             = 4,            //MCU
	em_MP              = 5,            //MP
	em_MTADP           = 6,            //MTADP
	em_PRS             = 7,            //PRS
	em_TUI             = 8,            //TUI
	em_DSS             = 9,            //DSS
	em_MPS			   = 10,			  //MPS
	em_END
};

#define  WD_MODULE_NUM             (u16)em_END

enum TMState
{
	em_STOP            = 0,          //ֹͣ
    em_RUNNING         = 1           //����
};
////////////////////////////////////////////
//�໤���߳�����
enum eMCUAPPType
{
	em_MCU_INVALIDAPP  = 0,
	em_MCU_GUARD_APP   = 1
};
enum eTsAPPType
{
	em_TS_INVALID_APP  = 0,     
	em_TS_OPER_APP     = 1,
	em_TS_TRVLS_APP    = 2,
	em_TS_CALLS_APP    = 3
};
enum eGKAPPType
{
	em_GK_INVALID_APP  = 0, 
	em_GK_OPER_APP     = 1
};
enum ePXYAPPType
{
	em_PXY_INVALID_APP = 0,
};
enum eMMPAPPType
{
	em_MMP_INVALID_APP = 0,
};

//ģ�����
typedef struct tagModuleParam
{
    tagModuleParam() : emType(em_INVALIDMODULE), emState(em_STOP) {}
	
    TMType    emType;     //ģ������
    TMState   emState;    //ģ��״̬
} TModuleParameter, *PTModuleParameter;

typedef struct tagOthersMState
{
	TModuleParameter m_achOtherMdInfo[em_END];
}TOthersMState;

//��ѭ���Ź�����˵��¼���Χ
OSPEVENT(WD_APP_WDC_REG_REQ,                   (EV_WD8000E_BGN + 40 )); //enum
OSPEVENT(WD_WDC_APP_REG_ACK,                   (EV_WD8000E_BGN + 41 )); //nothing
OSPEVENT(WD_WDC_APP_HB_REQ,                    (EV_WD8000E_BGN + 42 )); //nothing
OSPEVENT(WD_APP_WDC_HB_ACK,                    (EV_WD8000E_BGN + 43 )); //enum
OSPEVENT(WD_MCUGD_WDC_REBOOT_CMD,              (EV_WD8000E_BGN + 44 )); //nothing
OSPEVENT(WD_MCUGD_WDC_GETKEY_REQ,              (EV_WD8000E_BGN + 45 )); //nothing
OSPEVENT(WD_WDC_MCUGD_GETKEY_ACK,              (EV_WD8000E_BGN + 46 )); //TLicenseMsg
OSPEVENT(WD_MCUGD_WDC_GETMDINFO_REQ,           (EV_WD8000E_BGN + 47 )); //nothing
OSPEVENT(WD_WDC_MCUGD_GETMDINFO_ACK,           (EV_WD8000E_BGN + 48 )); //nothing
OSPEVENT(WD_WDC_MCUGD_GETMDINFO_NOTIFY,        (EV_WD8000E_BGN + 49 )); //
OSPEVENT(WD_MCUGD_WDC_UPDATE_CMD,              (EV_WD8000E_BGN + 50 )); //nothing
OSPEVENT(WD_MCUGD_WDC_SHUTDOWN_CMD,            (EV_WD8000E_BGN + 51 )); //nothing
OSPEVENT(WD_MCUGD_WDC_SOFTREBOOT_CMD,          (EV_WD8000E_BGN + 52 )); //nothing

/*=============================================================================
  �� �� ���� StartWatchDog
  ��    �ܣ� �������Ź� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  TMType emModuletype 
              BOOL         bIsCarrier
  �� �� ֵ��  BOOL32  ���Ź�Ӧ���Ƿ�ɹ�����
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2009/02/24    1.0         �ź��      ����
=============================================================================*/
BOOL32 WDStart(TMType emModuletype,BOOL bIsCarrier = FALSE);
/*=============================================================================
  �� �� ���� SetModuleState
  ��    �ܣ� ����ģ��״̬ 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  TMState emState           
  �� �� ֵ��  BOOL32  ���Ź�Ӧ���Ƿ�ɹ�����
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2009/02/24    1.0         �ź��      ����
=============================================================================*/
void WDSetModuelState(TMState emState);
/*=============================================================================
  �� �� ���� SetModuWDStopleState
  ��    �ܣ� ����ģ��״̬ 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����                          
  �� �� ֵ��  
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2009/02/24    1.0         �ź��      ����
=============================================================================*/
void WDStop();
#endif
