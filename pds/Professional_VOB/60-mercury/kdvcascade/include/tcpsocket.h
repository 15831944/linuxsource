#ifndef _TCPSOCKET_H_
#define _TCPSOCKET_H_
#include "osp.h"

//���̵߳���

typedef u32    HNODE;
typedef u32    HAPPNODE; 
#define INVALID_NODE 0
#define MAX_TCPMSG_LEN    (s32)8192//����TCP������
#define MAX_SOCKETBUFF_LEN (s32)(128*1024)//����TCP������
#define LOCAL_DEFAULT_PORT (u16)5670 //��������Ĭ�϶˿�
#define MAX_PORT_SPAN      (u8)30	 //����5670 - 5699�˿�

typedef void (*PTCPNEWCONN)(HNODE hListenNode, HNODE hNode, HAPPNODE* phAppNode);

//nLen <= 0 ��ʾnode �Ͽ���
typedef void (*PTCPDataCallBack)(HNODE hNode, HAPPNODE hAppNode, u8 *pBuf, s32 nLen);

typedef struct tagTCPEvent
{
    PTCPNEWCONN			pNewConnEvent;
    PTCPDataCallBack	pDataCallBack;
}TTCPEvent;

BOOL32 SocketBindToCascIns(HNODE hSocket, HAPPNODE hCascIns);
BOOL32 TcpSocketInit( BOOL32 bSockInit = FALSE );
BOOL32 TcpSocketClean( BOOL32 bSockClean = FALSE );
BOOL32 TcpSetCallBack( TTCPEvent &tTCPEvent );
HNODE  TcpCreateNode( u32 dwIp, u16 wPort );
void   TcpDestroyNode(HNODE hListenNode);
HNODE  TcpConnect( u32 dwIp, u16 wPort, HAPPNODE hAppNode ); 
s32    TcpSend( HNODE hNode, u8 *pbyBuf, s32 nLen);  
BOOL32 TcpDisconnect( HNODE hNode);

#endif

