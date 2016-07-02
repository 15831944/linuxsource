#ifndef SM_PUBLIC_MACRO_H_
#define SM_PUBLIC_MACRO_H_

#include "kdvtype.h"
#include "kdvdef.h"
#include "osp.h"
#include "mtsnmp.h"
#include "vccommon.h"
#include "errorid.h"

/////////////////////////////////////////////////////Linux ���쳣������
#ifdef _LINUX_

#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <string.h>
#include <sys/types.h>
#endif

#ifdef WIN32
#include "time.h"
#endif


//Session ģ��App ID����
#define CSESSIONMANAGER_APPID       AID_SVR_BGN
#define CCMAPP_APPID                (AID_SVR_BGN + 1)		
#define CFMAPP_APPID                (AID_SVR_BGN + 2)

#define CRITICAL        1
#define IMPORTANT       2
#define APPLICATION     196
#define USERDEBUG       255

//ǰ��̨��·״��
#define	LINK_DOWN       0
#define LINK_ACTIVE     1
#define LINK_UNKNOWN    255

#define INVALID_IPADDR            (u32)0xffffffff

//���ݿ���
const u8 DB_NAME_MAX_LEN = 16;

//�û�������ĳ���
const u8 USER_LEN = 17;

const u8 PWD_LEN = USER_LEN;
//ָʾ��״̬����
//const u8 LED_STATUS_LEN = 24;

//ʱ���ַ����ĳ���
const u8 TIME_LEN = 20;

//IP��ַ�ַ����ĳ���
const u8 IPADDR_LEN = 16;

//�豸�汾����
const u8 DEVICE_VER_LEN = 20;

//DB���ӳ�ʱ���룩
const u8 MAX_DBCONNECT_TIME = 30;

//����汾����
const u8 MAX_SOFTWARE_VER_LEN = 36;

//����ʱ�䳤��
const u8 MAX_COMPILE_TIME_LEN = 36;

//�Ƶ�״̬�̶�
const u8 MAX_LED_STATUS_LEN = 64;

//����������
const u8 MAX_CONFNAME_LEN = 32;

const u8 MAX_CONFID_LEN = MAX_CONFNAME_LEN;

//�豸�������Ƴ���
const u8 SM_DEVICE_TYPENAME_LENGTH = 16;

//mcu���������Ĳ�λ��
const u8 SM_MCU_LAYER_SLOT_MAX = 64;

//���mcu����
const u8 SM_MCU_MANA_MAX = 200;

//����buffer���ֵ
const u16 SM_BOARD_BUFFER_MAX = SM_MCU_MANA_MAX * SM_MCU_LAYER_SLOT_MAX;

//�����������
const u8 SM_MAX_PROC_NAME_LEN = 32;

//�豸�����ַ�������
const u8 SM_DEVTYPE_NAME_LEN  = 31;

//��������
const u8 SM_MAX_ALIAS_LEN  = MAXLEN_ALIAS;

//164�ų���
const u8 SM_MAX_E164_LEN  = MAXLEN_E164;

//����ʹ����ͳ��buffer����
const u8 SM_MAX_EQPNODEBUFF_LEN  = 128;

//lukp[02/08/2010] ���һ�������������ֵ���Ժ�����������裬��ֵ��Ҫ����
const u16 SM_MAX_EQP_TYPE = 12; //�����������ֵ

const u16 SM_MAX_UPDATE_LEN = 4 * 1024;

const u16 SM_MAX_UPDATE_FILENUM = 2;

const u16 SM_MAX_UPDATE_BUFFER = 100;

const u16 SM_MAX_UPDATEBOARD_BUFFER = 1000;

const u16 SM_MAX_SUFF_LEN = 10;

const u8 PERI_STATE_LEN         = 4;    //MCU�����豸״̬����

//ʵ���������
const u8 SM_MAX_INS_NAME_LEN = SM_MAX_PROC_NAME_LEN;

//�ļ�������ʱ��ѯ���
#define SM_UPDATE_QUERY_TIME       (u32)20 * 1000

//������
#define ERROR_CMD_SUCCESS               0	// ���׳ɹ�
#define ERROR_DEFAULT                   ERR_SVR_BGN + 1     //ȱʡ����
#define ERROR_SVR_RESTARTING            ERR_SVR_BGN + 2     //����������������
#define ERROR_DB_UNKNOWD                ERR_SVR_BGN + 3     //���ݿ����
#define ERROR_CMD_IN_RUNNING            ERR_SVR_BGN + 4     //�������ڽ�����

#define	ERROR_NEED_MORE_PARAM           ERR_SVR_BGN + 5     //������ȫ
#define ERROR_SM_BUSY					ERR_SVR_BGN + 6     //snmpmanageræ

#define ERROR_CMD_TIMEOUT               ERR_SVR_BGN + 10    //SessionManager��ʱ
#define ERROR_CMD_TYPE_MISMATCHED       ERR_SVR_BGN + 11    //�������Ͳ�ƥ��

#define	ERROR_NO_FREEINSTANCE           ERR_SVR_BGN + 20    //CM/FMӦ��û�п��õ�ʵ��
#define	ERROR_CLIENT_MISMATCHED         ERR_SVR_BGN + 21    //�ͻ��˲�ƥ��

#define	ERROR_FM_GETALARMSTAMP_FAIL     ERR_SVR_BGN + 30    //��ȡ�澯��Ϣʧ��
#define	ERROR_FM_GETALARMSTAMP_TIMEOUT  ERR_SVR_BGN + 31    //��ȡ�澯��Ϣ��ʱ

#define	ERROR_FM_GETALARM_FAIL          ERR_SVR_BGN + 32    //��ȡ�澯ʧ��
#define	ERROR_FM_GETALARM_TIMEOUT       ERR_SVR_BGN + 33    //��ȡ�澯��ʱ

#define ERROR_CM_RESTART_UPDATING       ERR_SVR_BGN + 34    //�豸�ļ������У�����������

#define ERROR_SNMP_SEND_FAIL            ERR_SVR_BGN + 40    //����SNMP����ʧ��
#define ERROR_SNMP_RCV_FAIL             ERR_SVR_BGN + 41    //����SNMP����ʧ��

#define ERROR_FTP_RUNNING               ERR_SVR_BGN + 60    //Ftp��������ִ��
#define ERROR_FTP_DELETEFILE_FAIL       ERR_SVR_BGN + 61    //FTPɾ���ļ�ʧ��
#define ERROR_FTP_PUTFILE_FAIL          ERR_SVR_BGN + 62    //FTP�����ļ�ʧ��
#define ERROR_FTP_GETFILE_FAIL          ERR_SVR_BGN + 63    //FTP��ȡ�ļ�ʧ��

#define ERROR_EQUIPMENT_MISMATCHED      ERR_SVR_BGN + 70    //�豸���Ͳ�ƥ��

#define ERROR_LOGIN                     ERR_SVR_BGN + 80    //��¼ʧ��
#define ERROR_LOGIN_NO_USER             ERR_SVR_BGN + 81    //û������û�
#define ERROR_LOGIN_ERR_PWD             ERR_SVR_BGN + 82    //�û��������
#define ERROR_LOGIN_NO_ROLE             ERR_SVR_BGN + 83    //�û��������ڽ�ɫ
#define ERROR_CHANGE_PWD                ERR_SVR_BGN + 84    //�޸�����ʧ��
#define ERROR_PWD_MISMATCHED            ERR_SVR_BGN + 85    //���벻����
#define ERROR_USER_HAS_LOGON            ERR_SVR_BGN + 86    //�û��Ѿ���¼				

//����
#define ERROR_SET_BOARD_STATUS_FAILED       ERR_SVR_BGN + 100   //���õ���״̬ʧ��
#define ERROR_SYNC_BOARD_TIME_FAILED        ERR_SVR_BGN + 101   //ͬ������ʱ��ʧ��
#define ERROR_BOARD_BITERROR_TEST_FAILED    ERR_SVR_BGN + 102   //�������ʧ��
#define ERROR_BOARD_SELF_TEST_FAILED        ERR_SVR_BGN + 103   //�Բ�ʧ��
#define ERROR_BOARD_E1LINK_CFG_FAILED       ERR_SVR_BGN + 104   //E1��·����ʧ��



#define	DEVICE_LINK_DOWN            (u16)999
#define DEVICE_POWEROFF             (u16)998
#define DEVICE_RESTART              (u16)1000

//��������
#define OBJECT_TYPE_UNKNOWN     (u8)0   // δ֪���豸
#define OBJECT_TYPE_MCU         (u8)0x1 // MCU�豸
#define OBJECT_TYPE_MT          (u8)0x2 // �ն��豸
#define OBJECT_TYPE_SM			(u8)0x3	// Snmp ManagerҲ��Ϊһ���豸
#define OBJECT_TYPE_PERI        (u8)0x4 // �����豸

//Mcu ����
#define MCU_CATALOG_UNKNOWN     0x0 //δ֪Mcu
#define MCU_CATALOG_8000        0x1 //Mcu8000
#define MCU_CATALOG_8000B       0x2 //Mcu8000B
#define MCU_CATALOG_8000C       0x3 //MCU 8000C

// �ն˷���
#define MT_CATALOG_UNKNOWN				MTSYSHARDWARETYPE_UNKNOWNMTMODEL	// δ֪�ն�
#define MT_CATALOG_PC					MTSYSHARDWARETYPE_PCMT				// �����ն�
#define MT_CATALOG_EMBED_MT8010			MTSYSHARDWARETYPE_8010				// MT8010�ն�
#define MT_CATALOG_EMBED_MT8010C		MTSYSHARDWARETYPE_8010C				// MT8010C�ն�
#define MT_CATALOG_EMBED_MT8010C1		MTSYSHARDWARETYPE_8010C1			// MT8010C�ն�
#define MT_CATALOG_EMBED_IMT			MTSYSHARDWARETYPE_IMT				// Ƕ���ն�IMT
#define MT_CATALOG_EMBED_MT8010A		MTSYSHARDWARETYPE_8010A				// MT8010A�ն�
#define MT_CATALOG_EMBED_MT8010APLUS    MTSYSHARDWARETYPE_8010APLUS			// MT8010A+�ն�
#define MT_CATALOG_EMBED_MT6610			MTSYSHARDWARETYPE_6610				// TS6610
#define MT_CATALOG_EMBED_MT6610E		MTSYSHARDWARETYPE_6610E				// TS6610E
#define MT_CATALOG_EMBED_MT6210			MTSYSHARDWARETYPE_6210				// TS6210
#define MT_CATALOG_EMBED_MT5210			MTSYSHARDWARETYPE_5210				// TS5210
#define MT_CATALOG_EMBED_MT3210			MTSYSHARDWARETYPE_3210				// TS3210
#define MT_CATALOG_EMBED_MTV5			MTSYSHARDWARETYPE_V5				// TS V5
#define MT_CATALOG_EMBED_MT8010A_2		MTSYSHARDWARETYPE_8010A_2			// 8010A ���2M
#define MT_CATALOG_EMBED_MT8010A_4		MTSYSHARDWARETYPE_8010A_4			// 8010A ���4M
#define MT_CATALOG_EMBED_MT8010A_8		MTSYSHARDWARETYPE_8010A_8			// 8010A ���8M
#define MT_CATALOG_EMBED_MT7210			MTSYSHARDWARETYPE_7210				// TS7210
#define MT_CATALOG_EMBED_MT7610			MTSYSHARDWARETYPE_7610				// TS7610
#define MT_CATALOG_EMBED_MT7620_B		MTSYSHARDWARETYPE_7620_2			// TS7620_B
#define MT_CATALOG_EMBED_MT7620_A		MTSYSHARDWARETYPE_7620_4			// TS7620_A
#define MT_CATALOG_EMBED_MT5610         MTSYSHARDWARETYPE_5610              // MT5610�ն�
#define MT_CATALOG_EMBED_MT7810			MTSYSHARDWARETYPE_7810				// TS7810
#define MT_CATALOG_EMBED_MT7910			MTSYSHARDWARETYPE_7910				// TS7910
#define MT_CATALOG_EMBED_MT7820_A		MTSYSHARDWARETYPE_7820_A			// MT7820_A
#define MT_CATALOG_EMBED_MT7820_B		MTSYSHARDWARETYPE_7820_B			// MT7820_B
#define MT_CATALOG_EMBED_MT7920_A		MTSYSHARDWARETYPE_7920_A			// MT7920_A
#define MT_CATALOG_EMBED_MT7920_B		MTSYSHARDWARETYPE_7920_B			// MT7920_B


#endif
