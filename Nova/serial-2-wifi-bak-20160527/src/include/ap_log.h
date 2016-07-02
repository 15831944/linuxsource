/******************************************************************************

                  ��Ȩ���� (C), 2012-2016, ���ߵ��ӣ��Ϻ������޹�˾

 ******************************************************************************
  �� �� ��   : ap_log.h
  �� �� ��   : ����
  ��    ��   : kevin
  ��������   : 2015��12��4��
  ����޸�   :
  ��������   : �û�̬��log��־���
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2015��12��4��
    ��    ��   : kevin
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef __AP_LOG_H__
#define __AP_LOG_H__

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ⲿ����˵��                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ⲿ����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ȫ�ֱ���                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/
#define LOG_MOUDLES_CORE	0x0001
#define LOG_MOUDLES_NETWORK	0x0002
#define LOG_MOUDLES_SERIAL  0x0004
#define	LOG_MOUDLES_OPS     0x0008
#define LOG_MOUDLES_OSAL	0x0010

#define DEFAULT_LOG_MOUDLES_MASK 0xFFFF

#define LOG_MOUDLES_CORE_NAME		"core"
#define LOG_MOUDLES_NETWORK_NAME	"network"
#define LOG_MOUDLES_SERIAL_NAME		"serial"
#define LOG_MOUDLES_OPS_NAME		"ops"
#define LOG_MOUDLES_OSAL_NAME		"osal"
#define LOG_MOUDLES_UNKNOW_NAME 	"unknow"

/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/
typedef enum
{
	AP_LOG_EMERG = 0, /* system is unusable */
	AP_LOG_ALERT,	  /* action must be taken immediately */
	AP_LOG_CRIT,	  /* critical conditions */
	AP_LOG_ERR, 	  /* error conditions */
	AP_LOG_WARNING,   /* warning conditions */
	AP_LOG_NOTICE,	  /* normal but significant condition */
	AP_LOG_INFO,	  /* informational */
	AP_LOG_DEBUG,	  /* debug-level messages */
}AP_LOG_LEVEL_E;

extern AP_LOG_LEVEL_E g_enDebugLevel;

extern unsigned int g_uiLogMoudleMask;

extern char *log_moudle_name(unsigned int iLogChanMask);

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define	DEBUG_USE_PRINTF 1
//#define	DEBUG_USE_PRINTF 0

#define AP_LOG(level, module, fmt, args...)          \
	do{											     \
		if (level <= g_enDebugLevel)				 \
		{											 \
			if(module & g_uiLogMoudleMask)			 \
			{										 \
				if(1 == DEBUG_USE_PRINTF)				 \
				{										 \
					printf("[%s]%s(%d): "fmt, log_moudle_name(module), __FUNCTION__, __LINE__, ##args); \
				}										 \
				else									 \
				{										 \
					syslog(level,"[%s]%s(%d): "fmt, log_moudle_name(module), __FUNCTION__, __LINE__, ##args); \
				}									 \
			}                                        \
		}											 \
	}while(0)



#endif /* __AP_LOG_H__ */

