
#ifndef _MDCOM_INTERFACE_H_
#define _MDCOM_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "kdvtype.h"

//LOG_LVL
#define LVL_ERR							0
#define LVL_WARN						1
#define LVL_INFO						2
#define LVL_DETAIL						3
#define LVL_VERBOSE						4

#define MCMD_OK                 0               //�ɹ�
#define MCMD_FAIL               -1              //ʧ��
#define MCMD_UNSURE             -2              //����ִ����

/* �˴��Ķ��������������е�λ�ö�Ӧ */
#define  MDCOM_SET_MOD_FREQ             0       //���÷���Ƶ��
#define  MDCOM_SET_MOD_OUTPUT           1       //���÷����Ƿ�ʼ
#define  MDCOM_SET_MOD_BITRATE          2       //���÷�������
#define  MDCOM_SET_MOD_CLKSRC           3       //���÷���ʱ��Դ

#define  MDCOM_SET_DEM_FREQ             4       //���ý���Ƶ��
#define  MDCOM_SET_DEM_BITRATE          5       //���ý�������
#define  MDCOM_SET_DEM_CLKSRC           6       //���ý���ʱ��Դ


#define  MDCOM_SET_MAX                  7       //��Ч�������
#define  MDCOM_PIPEBUF_SIZE             1024    //�ܵ������п���������Ϣ�ĸ�

/* ��ѯ������Ƕ�Ӧ��SET�������0x100 */
#define  MDCOM_QUERY_MOD_BITRATE                \
            MDCOM_SET_MOD_BITRATE  | 0x100 
#define  MDCOM_QUERY_MOD_FREQ                   \
            MDCOM_SET_MOD_FREQ     | 0x100      
#define  MDCOM_QUERY_MOD_OUTPUT                 \
            MDCOM_SET_MOD_OUTPUT   | 0x100 
#define  MDCOM_QUERY_MOD_CLKSRC                 \
            MDCOM_SET_MOD_CLKSRC   | 0x100 

#define  MDCOM_QUERY_DEM_BITRATE                \
            MDCOM_SET_DEM_BITRATE  | 0x100 
#define  MDCOM_QUERY_DEM_CLKSRC                 \
            MDCOM_SET_DEM_CLKSRC   | 0x100 
#define  MDCOM_QUERY_DEM_FREQ                   \
            MDCOM_SET_DEM_FREQ     | 0x100 

typedef u16 cmd_t;

typedef struct {
	cmd_t cmd;                      /* ����� */
	u32 ip; 
    u16 port;                       /*Ŀ��IP, Port*/
	u32 val;                        /* Ҫ���õ�ֵ */
} MdcomCmd;


int     CommandModem(const MdcomCmd *cmd, u8 trynum);     //��MODEM��������
int     CheckCmdStatus(MdcomCmd *cmd);                    //��ѯ����ִ��״̬
int 	MdcomOutput(u8 byLvl, const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#if defined(_MSC_VER)       // for Microsoft c++
#define API extern "C"  __declspec(dllexport)
#else                       // for gcc
#define API extern "C"
#endif

API int		MdcomInit		(void);                       //ģ���ʼ��
API void	MdcomTerm		(void);
API void	MdcomReport		(void); 
API void	MdcomMsgLog		(void);
API int		MdcomCommand	(const char* cmd, const char* ip, u16 port, u32 value); 
API int		MdcomCheck		(const char* cmd, const char* ip, u16 port);


#endif

