
#ifndef MODEM_SERVICE_H
#define MODEM_SERVICE_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>


#ifdef _VXWORKS_
    #include "tickLib.h"
    #include "iolib.h"
    #include "types.h"
    //#include <mbuf.h>
    #include "socket.h"
    #include "in.h"
    #include "socklib.h"
    #include "msgQLib.h"
    #include "netinet\tcp.h"
    #include "ctype.h"
    #include "inetlib.h"
    //#include <socketvar.h>
#endif

#ifdef WIN32
    #include "winsock2.h"
    #include "Windows.h"
#endif

#ifdef _LINUX_

#endif

#define MAXNUM_CMD		(u8)100

#include "kdvtype.h"
#include "modeminterface.h"



#define THREAD_STACK_SIZE   400 << 10       /*< �̶߳�ջ�Ĵ�С */
#define CMDTABLE_ITEM_NUM   1 * 256         /*< ����Ϊ256����������ӳ��һ�����ε�IP */
#define PROTLPACK_MAXSIZE   150             /*< Э�������󳤶� */

#define MAX_LOG_MSG_LEN		1000			/*  ������Ϣ����������󳤶� */


#define MONITOR_INTERVAL    2               /* Monitor�̵߳�ɨ��ʱ������100ms�ı��� */
#define COMMAND_FAILNUM     2               /* ���ִ�������Դ�������������һ�η��� */


#define debug               //printf


typedef struct MsgPipe 
{
#ifdef WIN32
    HANDLE read;                            /*< �ܵ��Ķ���� */
    HANDLE write;                           /*< �ܵ���д��� */
    CRITICAL_SECTION readCS;                /*< �����ܵ���д */
#endif

#ifdef _VXWORKS_
    MSG_Q_ID pipe;
#endif

#ifdef _LINUX_
    //int anPipe[2];
	MdcomCmd		cmdQueue[MAXNUM_CMD];
	s32             m_nHead;
    s32             m_nTail;
#endif

}MsgPipe;


/* �����������Ķ��� */
typedef enum InCmdStatus
{
    ics_invalid = 0,                        /*< ������Ч */
    ics_ready,                              /*< ����ȴ����� */
    ics_send,                               /*< �����Ѿ����� */
    ics_receive,                            /*< �����ѽ��� */
    ics_equal,                              /*< ���һ�� */
    ics_unequal                             /*< �����һ�� */
} InCmdStatus;

typedef struct InCmd
{
    u32 set;                                /*< Ҫ���õ���������� */
    u32 ret;                                /*< ��Ӧ��������� */
    u32 tick;                               /*< ���������ʱ�� */
    InCmdStatus  status;                    /*< ����ִ�е�״̬ */
    u8  failnum;                            /*< ִ������ʧ�ܴ��� */
    u8  trynum;                             /*< �������� */
    u8  triednum;                           /*< �Ѿ��������� */
	u8  query;								/*< 0: set 1: query */
} InCmd;

typedef struct CmdTableItem
{
    u32 ip;                                 /*< ��ӦMODEM��IP */
    u16 port;                               /*< �˿���ʱ���� */
    InCmd cmds[MDCOM_SET_MAX];              /*< ÿ��MODEM�ϵ�7�����λ�þ�������� */
} CmdTableItem;


#ifndef WIN32
    typedef int                   HANDLE;
    typedef int                   SOCKET;
    #define INVALID_SOCKET        ERROR
    #define SOCKET_ERROR          ERROR
#endif

typedef struct ThreadData
{
    HANDLE handle;                                  /* �̵߳ľ�� */
    int terminate;                                  /* ֪ͨ�߳��˳� */
#ifdef _VXWORKS_
    char name[20];
#endif
#ifdef _LINUX_
	char name[20];
#endif
} ThreadData;

/* ����TICK��صĳ�����ʹ�ô˽ṹ����ΪVXWORKS�ϵĺ������ǻ�仯�ģ�
 * û�취ʹ�ó�������ʱ�� 
 */
typedef struct MdcomTime
{
    u16     interval;                               /* ������ʱ����*/
    u16     timeout;                                /* ���η��͵ĳ�ʱʱ�� */
} MdcomTime;

typedef struct MdcomEnv
{
    SOCKET  sock;                                   /* ����UDP��SOCKET */
    MsgPipe pipe;                                   /* ����ݹܵ� */
    ThreadData thread[3];                           /* ���������߳� */
    CmdTableItem table[CMDTABLE_ITEM_NUM];          /* ����� */
    MdcomTime time;                                 /* ʱ����صĳ��� */
	int     log;								    /* �Ƿ���Ҫ������Ϣ��0������Ҫ */
} MdcomEnv;

typedef struct MdcomStat
{
	int send;
	int recv;
	int fail;
	int succ;	
}MdcomStat;


u32		MdcomSendTask(void *data);
u32		MdcomRevTask(void *data);
u32		MdcomMonitorTask(void *data);
int		DumpMsg(const u8* msg, int msglen, char* buf);

const char* ip2String(struct in_addr addr);
const char* ip2String_u32(u32 dwIP );

API void pmdmpxymsg( void );
API void npmdmpxymsg( void );
API void setmdmpxylog( u8 byLevel );

#ifdef __cplusplus
}
#endif


#endif



