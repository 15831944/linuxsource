
#include "modemservice.h"
#include "cmdtable.h"
#include "msgpipe.h"

//#include "osp.h"

void InitCmdTable(CmdTableItem* table)
{
    memset(table, 0, sizeof(CmdTableItem) * CMDTABLE_ITEM_NUM);
}

/* ����IP�Ͷ˿ڣ��ҵ�����λ�ã�����λ�ã�����ԭ��λ�� */
int LocationCmdTable(const CmdTableItem* table, u32 ip, u16 port)
{
    int begin, curr, idx;
    int first = -1;

#ifdef WIN32
    begin = ip >> 24;
#endif

#ifdef _VXWORKS_
    begin = ip & 0xff;
#endif

#ifdef _LINUX_
	begin = ip & 0xff;
#endif

    for( idx = 0; idx < CMDTABLE_ITEM_NUM; idx++)
    {
        curr = (idx + begin) % ((int)CMDTABLE_ITEM_NUM);
        if (table[curr].ip == ip)
        {
            return curr;
        }
        else if ((table[curr].ip == 0) && (first == -1))
        {
            first = curr;
        }
    }
    /* �Ҳ�����Ӧ��IP�����ң�û�п���λ�� */
    if (first == -1)
	{
        return -1;
	}
    else
	{
        return first;
	}
}

int InsertCmdTable(CmdTableItem* table, u32 ip, u16 port, 
                   cmd_t cmd, const InCmd* item)
{
    int pos, index;
	InCmd *t_item;
    pos = LocationCmdTable(table, ip, port);
    if (pos == -1)
    {
        debug("Command table is full.\n");
        return -1;
    }
    table[pos].ip = ip;
    table[pos].port = port;
    
	/* �������λ���Ƕ�Ӧ�� */
    index = cmd & 0xf;
	if (index >= MDCOM_SET_MAX)
	{
		return -1;
    }

	/* �����������ã�ret���� */
	t_item = &(table[pos].cmds[index]);
	t_item->failnum = item->failnum;
	t_item->query   = item->query;
	t_item->set     = item->set;
	t_item->status  = item->status;
	t_item->tick    = item->tick;
	t_item->triednum= item->triednum;
	t_item->trynum  = item->trynum;

    return pos;
}

InCmd* LookforCmdTable(CmdTableItem* table, u32 ip, u16 port, cmd_t cmd)
{
    int pos, index;
    pos = LocationCmdTable(table, ip, port);
    if (pos == -1)
    {
        debug("Command table is full.\n");
        return NULL;
    }

    index = cmd & 0xf;
	if (index >= MDCOM_SET_MAX)
	{
		return NULL;
	}
    return &(table[pos].cmds[index]);
}

void BuildSingleItem(cmd_t cmd, u8 trynum, u32 value, InCmd *item)
{
    /* ȷ����Set������Query */
    if (cmd & 0x0f00)
    {
		item->query = 1;
        item->failnum = 0;                       /* Queryִֻ��һ�� */
    }
	else
	{
		item->query = 0;
        item->failnum = COMMAND_FAILNUM;         /* Set������� */
    }
    
	/* �������λ�� */
    item->ret = 0;
    item->set = value;
    item->status = ics_ready;
    item->trynum = trynum;
    item->triednum = 0;
    item->tick = GetTick();

	return;
}

