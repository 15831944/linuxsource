/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : criagent.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ������ú�������
   ����        : jianghy
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/08/25  1.0         jianghy       ����
   2004/11/11  3.5         �� ��         �½ӿڵ��޸�
******************************************************************************/
#ifndef CRICONFIG_H
#define CRICONFIG_H

#include "boardconfigbasic.h"

// add to brdcfg.ini [2012/05/05 liaokang]
// ֧��ͨ�������ļ����ü����˿�
#define SECTION_BoardMasterSystem	    (s8*)"BoardMasterSystem"
#define KEY_BrdMasterSysListenPort	    (s8*)"BrdMasterSysListenPort"
#define DEFVALUE_BrdMasterSysListenPort BRDMASTERSYS_LISTEN_PORT   // ��ϵͳ��IS2.2 8548��Ĭ�ϼ����˿�
// ֧�� ϵͳ���������ļ�����IP
#define SECTION_IpConfig			    (s8*)"IpConfig"
#define KEY_IsSupportIpConfig           (s8*)"IsSupportIpConfig"
#define DEFVALUE_IsSupportIpConfig		(u8)0           // Ĭ�� ��֧��
#define KEY_EthChoice				    (s8*)"EthChoice"
#define DEFVALUE_EthChoice			    (u8)1           // Ĭ�� ������
#define KEY_BrdMasterSysIp	            (s8*)"BrdMasterSysIp"
#define KEY_BrdMasterSysInnerIp	        (s8*)"BrdMasterSysInnerIp"
#define KEY_BrdMasterSysIPMask	        (s8*)"BrdMasterSysIPMask"
#define KEY_BrdMasterSysDefGateway	    (s8*)"BrdMasterSysDefGateway"
// brdcfg.ini end

class CCriConfig : public CBBoardConfig
{
public:
	CCriConfig();
	~CCriConfig();
    friend class CBoardAgent;
	/*====================================================================
	���ܣ��Ƿ�����Prs
	��������
	����ֵ�����з���TRUE����֮FALSE
	====================================================================*/
	BOOL IsRunPrs();

	/*====================================================================
	���ܣ���ȡprs������Ϣ
	��������
	����ֵ��MCU�Ķ˿ںţ�0��ʾʧ��
	====================================================================*/
	BOOL GetPrsCfg( TPrsCfg* ptCfg );

	/*====================================================================
	���ܣ�����PRS��������Ϣ
	��������
	����ֵ��TRUE/FALSE
	====================================================================*/
	BOOL SetPrsConfig( TEqpPrsEntry *ptPrsCfg );

	BOOL32 IsPrsCfgEqual( const TPrsCfg &tPrsCfg);

	void GetBoardSequenceNum( u8 *pBuf, s8* achSep ); //��ȡ�������к�

    // �������ļ���ʼ������������Ϣ
    BOOL32 InitLocalCfgInfoByCfgFile(void);     // [2012/05/05 liaokang] ֧�� ϵͳ���������ļ�����IP
    // ���õ�����ϵͳ�ļ����˿�
    BOOL32 SetBrdMasterSysListenPort( u16 wListenPort );
    // ��ȡ������ϵͳ�ļ����˿�
    u16    GetBrdMasterSysListenPort(void);

private:
	BOOL    bIsRunPrs;      //�Ƿ�����PRS
	TPrsCfg m_prsCfg;
    u16     m_wBrdMasterSysListenPort;  // ��ϵͳ��IS2.2 8548�� Listen Port
};

#endif /* CRICONFIG_H */
