/*!
*\brief Э����ķ�װ
*/

#ifndef _CMD_PROTOCOL_H_
#define _CMD_PROTOCOL_H_

//#include "osp.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    
    int AssemSendPack(cmd_t cmd, u32 val, u8 *pack);        /*< �����������ֵ��װ��һ�����Ͱ� */
    int DisassemRecvPack(const u8 *pack, int len,           \
                         u32 ip, u16 port);                 /*< ��һ�����հ��зֽ��ָ�������ֵ */
    
#ifdef __cplusplus
}
#endif


#endif

