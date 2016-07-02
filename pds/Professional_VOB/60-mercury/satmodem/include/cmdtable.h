
/*!
 *\file CmdTable.h
 *\brief �����������в����Ķ��壻
 */

#ifndef _CMD_TABLE_H_
#define _CMD_TABLE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "modeminterface.h"
#include "modemservice.h"

#ifdef _LINUX_
	#include "string.h"
#endif

void        InitCmdTable(CmdTableItem* table);

int         InsertCmdTable(CmdTableItem* table,
                           u32 ip,
                           u16 port, 
                           cmd_t cmd,
                           const InCmd* item);      /*< ��ʼ�������� */

InCmd*      LookforCmdTable(CmdTableItem* table,
                            u32 ip,
                            u16 port,
                            cmd_t cmd);             /*< ����ָ������ */

void        BuildSingleItem(cmd_t cmd,
                            u8 trynum,
                            u32 value,
                            InCmd *item);           /* �����������������һ�������� */

#ifdef __cplusplus
}
#endif


#endif

