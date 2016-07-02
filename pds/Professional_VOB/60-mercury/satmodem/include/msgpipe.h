/*!
 *\file MsgPipe.h
 *\brief �ܵ�ʵ�ֲ��ֵĶ���
 *
 *��ģ��ʹ����һ���ܵ������ڴ��л�Ҫ���͵����
 */

#ifndef _MSG_PIPE_H_
#define _MSG_PIPE_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "kdvtype.h"
#include "modeminterface.h"
#include "modemservice.h"

#ifdef _LINUX_
#include "unistd.h"
#endif




int  CreateMsgPipe(MsgPipe *pipe);                          /*< �����ܵ�����ر��� */
void CloseMsgPipe(MsgPipe *pipe);                           /*< �رչܵ� */
int  WriteMsgPipe(MsgPipe *pipe, const MdcomCmd *msg);      /*< ���ܵ���д��һ����Ϣ */
int  ReadMsgPipe(MsgPipe *pipe, MdcomCmd *msg);             /*< �ӹܵ��ж���һ����Ϣ */


int  InitSocket(SOCKET *sock);
void CloseSocket(SOCKET *sock);

u32  GetTick(void);
void Waitms(u32 millisecs);


#ifdef __cplusplus
}
#endif

#endif

