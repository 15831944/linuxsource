

#include "osp.h"
#include "modemservice.h"
#include "cmdtable.h"
#include "msgpipe.h"
#include "cmdprotocol.h"
#include "modemcmdproxy.h"


#ifdef _VXWORKS_
#include "taskLib.h"
#include "sysLib.h"
#endif

#ifdef _LINUX_
#include "ctype.h"
#endif

MdcomStat g_stat;
MdcomEnv g_env;                                     /* Ψһ��ȫ�ֱ��� */

static const int udp_maxsize = 1024;
static u8  udp_buffer[udp_maxsize];         		/* ���ڴ��UDP���Ļ��� */


const char* ip2Stringu32(u32 dwIP )
{
    struct in_addr in;
    in.s_addr   = dwIP;

    return ip2String(in);
}

const char* ip2String(struct in_addr in)
{
    static char achIPStr[8][17]={0};
    static char byIdx = 0;    
    
    byIdx++;
    byIdx %=sizeof(achIPStr)/sizeof(achIPStr[0]);
    
    memset( achIPStr[byIdx] ,0,sizeof(achIPStr[0]) );
    
#ifdef _MSC_VER
    strncpy( achIPStr[byIdx] ,inet_ntoa(in),sizeof(achIPStr[0]) );
#endif //_MSC_VER
    
#ifdef _VXWORKS_
    inet_ntoa_b( in ,achIPStr[byIdx] );
#endif //_VXWORKS_
    
    achIPStr[byIdx][16]= 0;
    return achIPStr[byIdx];
    
}


/*�����̣߳�����״̬��Ϣ*/
u32 MdcomRecvTask(void *data)
{
	struct	sockaddr_in   srcaddr;
    int addrlen;
    int recvlen;
    int ret;

	char msgbuf[MAX_LOG_MSG_LEN];
    SOCKET *sock = &g_env.sock;

	while(!g_env.thread[1].terminate)
	{
        addrlen = sizeof(srcaddr);
        memset(&srcaddr, 0, addrlen);

#ifdef WIN32
        recvlen = recvfrom(*sock, (char *)udp_buffer, udp_maxsize, 0, (struct sockaddr*)&srcaddr, &addrlen);
#endif


#ifdef _LINUX_
        socklen_t socklen = socklen_t(addrlen);
        recvlen = recvfrom(*sock, (char *)udp_buffer, udp_maxsize, 0, (struct sockaddr*)&srcaddr, &socklen);
#endif

        if (recvlen == -1/*SOCKET_ERROR*/)
        {
            Waitms(g_env.time.timeout);
            continue;
        }
		strcpy(msgbuf, "\n");
		MdcomOutput(LVL_INFO, "[MdcomRecvTask] Receive package from \" %s : %d \" at %d\n", ip2String(srcaddr.sin_addr), ntohs(srcaddr.sin_port), GetTick());
		DumpMsg(udp_buffer, recvlen, msgbuf);
		MdcomOutput(LVL_DETAIL, "%s", msgbuf);
        ret = DisassemRecvPack(udp_buffer, recvlen, srcaddr.sin_addr.s_addr, ntohs(srcaddr.sin_port));
        if (ret > 0)
        {
            g_stat.recv += ret;
        }
	}
    return 0;
}

/*������������߳�*/
u32 MdcomSendTask(void *data)
{
	int  msglen, packlen;
	int  ret;
    u32  recvlen;
    MdcomCmd msg;
    u8   pack[PROTLPACK_MAXSIZE];
	char msgbuf[MAX_LOG_MSG_LEN];
    struct sockaddr_in addr;
    int  addrlen;
    MsgPipe *msgpipe = &(g_env.pipe);
    SOCKET *sock = &g_env.sock;
	

	while (!g_env.thread[0].terminate)
	{
        msglen = sizeof(MdcomCmd);
        memset(&msg, 0, msglen);
        recvlen = 0;

        /* �Ӷ�����ȡ��һ������ */
        if (*sock == -1/*INVALID_SOCKET*/)
            break;

	    ret = ReadMsgPipe(msgpipe, &msg);
		if(ret != 0 )
        {
            debug("Msg queue is empty!\n");
            Waitms(g_env.time.timeout);
            continue;
        }
        MdcomOutput(LVL_INFO, "[MdcomSendTask] Send msg< %s,\tip.%s, port.%d, val.%d>\tat tick.%d\n", 
								strOfCmd(msg.cmd),
								strOfIp(msg.ip),
								msg.port,
								msg.val,
								GetTick());

        /* ��װ��һ��Э��� */
        packlen = AssemSendPack(msg.cmd, msg.val, pack);

        /* ��������ʧ�ܵ��������ʧ�ܣ���Monitor�߳����ջ���ʱ�ط�*/
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = msg.ip;
#ifdef WIN32
        addr.sin_port = htons(msg.port);
#endif

#ifdef _VXWORKS_
        addr.sin_port = msg.port;
#endif

#ifdef _LINUX_
		addr.sin_port = msg.port;
#endif
        
		memset(addr.sin_zero, 0, sizeof(addr.sin_zero));
        addrlen = sizeof(addr);
        ret = sendto(*sock, (char *)pack, packlen, 0, (struct sockaddr*)&addr, addrlen);         
        
		if (ret == -1)//SOCKET_ERROR)
        {
            MdcomOutput(LVL_ERR, "[MdcomSendTask] Send error: %d, packlen.%d, addr.%s\n", ret, packlen, strOfIp(msg.ip) );
        } 
        else
        {
            strcpy(msgbuf,"\n");
			MdcomOutput(LVL_DETAIL, "Send package to \" %s : %d \" at %d\n", strOfIp(msg.ip), msg.port, GetTick());
			DumpMsg(pack, packlen, msgbuf);
			MdcomOutput(LVL_DETAIL, "%s", msgbuf);
            g_stat.send++;
        }
    }
    return 0;
}

/*������������߳�*/
u32 MdcomMonitorTask(void *data)
{
    int idx, idx2, ready_msg;
    u32 oldest;
    int ipnum;  /* ������ע��IP��������Ϊ�㣬���߳�Ҫ�ȴ�1s */
    CmdTableItem *table = g_env.table;
    InCmd *cmditem;
    MdcomCmd msg;
       
    while (!g_env.thread[2].terminate)
    {
        ipnum = 0;

        /* ����ÿһ�� */
        for (idx = 0; idx < CMDTABLE_ITEM_NUM; idx++)
        {
            if (table[idx].ip == 0)
            {
                continue;
            }
            ipnum++;
            msg.ip = table[idx].ip;
            msg.port = table[idx].port;

            /* ���ÿ�����ÿ��ѭ�����ֻ��һ������ */
            /* ����״̬ */
            oldest    = GetTick();
            ready_msg = 0;

            for (idx2 = 0; idx2 < MDCOM_SET_MAX; idx2++)
            {                
                cmditem = &(table[idx].cmds[idx2]);
                
                /* �յ����µ�MODEMЭ��� */
                if (cmditem->status == ics_receive)
                {
                    if(cmditem->set == cmditem->ret)
                    {
                        cmditem->status = ics_equal;
                    }
					else
					{
                        cmditem->status = ics_unequal;
                    }
                }

                /* �����Ҫ���͵���Ϣ����һ���ȴ���õ� */
                if ((cmditem->status == ics_ready) &&
                    (cmditem->tick != 0) &&
                    (cmditem->tick < oldest))
                {
                    msg.cmd = idx2;					
                    msg.val = cmditem->set;
                    oldest  = cmditem->tick;
                    ready_msg++;
                }
            }

            /* �������͵ķ��������Ҳ��ټ�����������Եġ�*/
            if (ready_msg > 0)
            {
                cmditem = &(table[idx].cmds[msg.cmd]);
				if (cmditem->query)
				{
					msg.cmd |= 0x100;
				}
                WriteMsgPipe(&(g_env.pipe), &msg);

                cmditem->status = ics_send;
                continue;
            }
            
            /* û��Ҫ���͵ģ������Ҫ�������ط��ģ�ÿ����෢��һ�� */
            for (idx2 = 0; idx2 < MDCOM_SET_MAX; idx2++)
            {
				int interval;			// �Ѿ���ȥ��ʱ��
				int inter_fail;			// ��ʱʱ��
				int inter_try;          // ����ʱ��

                cmditem = &(table[idx].cmds[idx2]);
				interval = GetTick() - cmditem->tick;
				msg.cmd = idx2;				
                msg.val = cmditem->set;

                /* �����������ѷ�������δ��Ӧ��������� */
				inter_try = (cmditem->triednum + 1) * g_env.time.interval;
                if ((cmditem->status == ics_send) &&
					(cmditem->trynum > 0) &&
					( interval > inter_try))                    
                {
                    cmditem->trynum--;
                    cmditem->triednum++;
					if (cmditem->query)
					{
						msg.cmd |= 0x100;
					}
                    WriteMsgPipe(&(g_env.pipe), &msg);
                    break;
                }
                
                /* 1000 > trynum * 100 */
				inter_fail = (COMMAND_FAILNUM - cmditem->failnum + 1) * g_env.time.timeout;
				
				/* �������ʱ�����������Ѿ���� */
				if (interval > inter_fail)
				{
					/* ��ΪSET������ҽ������ȣ����߸���û�з��أ����ط� */
					if (((cmditem->query == 0) && (cmditem->status == ics_unequal)) ||
						((cmditem->status == ics_send) && (cmditem->trynum == 0)))
					{
						if (cmditem->failnum > 0)
						{
							cmditem->failnum--;
							if (cmditem->query)
							{
								msg.cmd |= 0x100;
							}
		                    WriteMsgPipe(&(g_env.pipe), &msg);
			                g_stat.fail++;
				            break;
					    }
                        else 
                        {
							/* ������ɣ���Ȼ����ȷ������Ϊʧ�� */
						    cmditem->status = ics_invalid;
						}
					}
				}
            }
        }
        if (ipnum <= 0)
        {
            Waitms(g_env.time.timeout);
        }
        /* VERY IMPORTANT!!! Must delay a while */
        Waitms(g_env.time.interval * MONITOR_INTERVAL);
    } /* while (!share ... */
    return 0;
}


/*******************************************************************/
/***************      ����Modem����ģ��API      ********************/
/*******************************************************************/


/**********  ����Modem����ģ���ʼ��   ***********/
/*����ֵΪ0��ʾģ����������ɹ��� -1��ʾ����ʧ��*/

int MdcomInit(void)
{
    int idx;
    static int ModuleInitialized = 0;
    ThreadData * thread = NULL;

    if (ModuleInitialized)
    {
        debug("Module is running...\n");         
        return 0;               //ģ���Ѿ���ʼ����
    }

    memset(&g_stat, 0, sizeof(g_stat));
	g_env.log = 0;

    /* Socket */
    if (InitSocket(&(g_env.sock)) != 0)
	{
		return -1;
	}

    /* ��������ܵ� */
    if (CreateMsgPipe(&(g_env.pipe)) != 0)
    {
        MdcomTerm();
        return -1;
    }


	if (!::IsOspInitd())
	{
		OspInit( TRUE );
	}
	

#ifdef _LINUX_
	OspRegCommand("pmdmpxymsg",		(void*)pmdmpxymsg,	"pmdmpxymsg");
	OspRegCommand("npmdmpxymsg",	(void*)npmdmpxymsg,	"npmdmpxymsg");
	OspRegCommand("setmdmpxylog",	(void*)setmdmpxylog, "setmdmpxylog");
#endif

    /* ��ʼ������� */
    InitCmdTable(g_env.table);
	
    thread = g_env.thread;
    for (idx = 0; idx < 3; idx++ )
    {
        thread[idx].handle = NULL;
        thread[idx].terminate = 0;
    }
    
#ifdef WIN32
	/* �����߳� */
    g_env.time.interval = 100;
    g_env.time.timeout  = 1000;

	thread[0].handle = CreateThread(NULL, THREAD_STACK_SIZE, 
									(LPTHREAD_START_ROUTINE)MdcomSendTask,
									NULL,
									0,
									NULL);
	if (thread[0].handle == NULL)
	{
		debug("Create ModemServer Send Thread Fail\n");
        MdcomTerm();
		return -1;		
	}

    /* �����߳� */
	thread[1].handle = CreateThread(NULL,
									THREAD_STACK_SIZE, 
									(LPTHREAD_START_ROUTINE)MdcomRecvTask,
									NULL,
									0,
									NULL);
	if (thread[1].handle == NULL)
	{
		debug("Create ModemServer Recv Thread Fail\n");
        MdcomTerm();
		return -1;
	}

    /* check�߳� */
    thread[2].handle = CreateThread(NULL,
									THREAD_STACK_SIZE,
									(LPTHREAD_START_ROUTINE)MdcomMonitorTask,
									NULL,
									0,
									NULL);
	if (thread[2].handle == NULL)
	{
		debug("Create ModemServer Monitor Thread Fail\n");
        MdcomTerm();
		return -1;
	}
#endif

#ifdef _VXWORKS_
    
	/* ����ʱ�� */
    g_env.time.interval = sysClkRateGet() / 10;     /* 100 ms */    
    g_env.time.timeout  = sysClkRateGet();          /* 1s */    

    /* ���������̣߳������գ����� */
	strcpy(thread[0].name, "Send_Mdcom");
	thread[0].handle = taskSpawn(thread[0].name,
								 100,
								 0,
								 THREAD_STACK_SIZE,
								 (FUNCPTR)MdcomSendTask, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	if (thread[0].handle == ERROR)
	{
		debug("Create ModemServer Send Thread Fail\n");
		MdcomTerm();
		return -1;		
	}

	strcpy(thread[1].name, "Receive_Mdcom");
	thread[1].handle = taskSpawn(thread[1].name,
								 100,
								 0,
								 THREAD_STACK_SIZE,
								 (FUNCPTR)MdcomRecvTask, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if (thread[1].handle == ERROR)
	{
		debug("Create ModemServer Rcv Thread Fail\n");
		MdcomTerm();
		return -1;		
	}

	strcpy(thread[2].name, "Monitor_Mdcom");
	thread[2].handle = taskSpawn(thread[2].name,
								 100,
								 0,
								 THREAD_STACK_SIZE,
								 (FUNCPTR)MdcomMonitorTask, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	if (thread[2].handle == ERROR)
	{
		debug("Create ModemServer Monitor Thread Fail\n");
		MdcomTerm();
		return -1;		
	}

#endif


#ifdef _LINUX_

	/* �����߳� */
    g_env.time.interval = 100;
    g_env.time.timeout  = 1000;

	// ���������̣߳������գ����� 
	strcpy(thread[0].name, "Send_Mdcom");
	thread[0].handle = OspTaskCreate((LINUXFUNC)MdcomSendTask,
		                             thread[0].name,
									 100,
									 THREAD_STACK_SIZE,
									 0,
									 0,
									 NULL);
	
	if (thread[0].handle == -1)
	{
		printf("Create ModemServer Send Thread Fail\n");
		debug("Create ModemServer Send Thread Fail\n");
		MdcomTerm();
		return -1;		
	}
	
	strcpy(thread[1].name, "Receive_Mdcom");
	thread[1].handle = OspTaskCreate((LINUXFUNC)MdcomRecvTask,
									 thread[1].name,
									 100,
									 THREAD_STACK_SIZE,
									 0,
									 0,
									 NULL);
	if (thread[1].handle == -1)
	{
		debug("Create ModemServer Rcv Thread Fail\n");
		MdcomTerm();
		return -1;		
	}
	
	strcpy(thread[2].name, "Monitor_Mdcom");
	thread[2].handle = OspTaskCreate((LINUXFUNC)MdcomMonitorTask,
 									 thread[2].name,
									 100,
									 THREAD_STACK_SIZE,
									 0,
									 0,
									 NULL);
	
	if (thread[2].handle == -1)
	{
		debug("Create ModemServer Monitor Thread Fail\n");
		MdcomTerm();
		return -1;		
	}
	
#endif

    debug("interval: %d\ttimeout: %d\n", g_env.time.interval, g_env.time.timeout);
    ModuleInitialized = 1;
	return 0;
}

void MdcomTerm(void)
{
    int idx;
    for (idx = 0; idx < 3; idx++ )
    {
        g_env.thread[idx].terminate = 1;
    }
    CloseSocket(&(g_env.sock));
    CloseMsgPipe(&(g_env.pipe));
}

/* ��MODEM����һ������ */
int CommandModem(const MdcomCmd *cmd, u8 trynum)
{
    int ret;
    InCmd item;

    debug("\n\nCommand(%d) Valus is: %d at tick.%d\n", cmd->cmd, cmd->val, GetTick());

    /* ����������� */
    BuildSingleItem(cmd->cmd, trynum, cmd->val, &item);
    
    /* ���뵽������� */
    ret = InsertCmdTable(g_env.table, cmd->ip, cmd->port, cmd->cmd, &item);
    if (ret < 0)
    {
        debug("Insert command table failed!\n");
        return -1;
    }
    return 0;
}

int CheckCmdStatus(MdcomCmd *cmd)
{
	/*
	printf("[CheckCmdStatus] ");
	return MCMD_UNSURE;
    */

    int ret;
    InCmd *item = NULL;

    /* ��λ */
    item = LookforCmdTable(g_env.table, cmd->ip, cmd->port, cmd->cmd);
	if (item == NULL)
	{
		printf( "[CheckCmdStatud] LookforCmdTable failed!\n");
		return MCMD_FAIL;
	}
    cmd->val = item->ret;
    
	/* ȷ��״̬ */
    ret = MCMD_FAIL;
    if (item->status == ics_equal)
    {
        ret = MCMD_OK;
        debug("Query(%d) Valus is: %d at tick.%d\n", cmd->cmd, cmd->val, GetTick());
    }
    else if (item->status == ics_unequal)
    {        
        /* MODEM���ؾ���ȷ */
        ret = MCMD_OK;
        debug("Cmd %d unequal at %d\n", cmd->cmd, GetTick());
    }
    else if ((item->status == ics_send) ||
             (item->status == ics_receive) ||
             (item->status == ics_ready))
    {
        ret = MCMD_UNSURE;
    }
    else
    {
        ret = MCMD_FAIL;
        debug("Cmd %d invalid at %d\n", cmd->cmd, GetTick());
    }
    /* ͳ��һ�γɹ���ʹ��tick��֤ÿ������ֻ��ͳ��һ��*/
    if (item->tick > 0)
    {
        /* ����QUERY�������OK���� */
        if (ret == MCMD_OK)
        {
            g_stat.succ++;
            item->tick = 0;
        }
    }
    return ret;
}

void MdcomReport(void)
{
    printf("\nMdcom summary:\n");
    printf("Send: %d\t", g_stat.send);
    printf("Receive: %d\n", g_stat.recv);
    printf("Success: %d\t", g_stat.succ);
    printf("Fail: %d\t", g_stat.fail);
    printf("Timeout: %d\n", g_stat.send - g_stat.recv);
}

/* ���ڵ���ʱ�����Ϣ */
int MdcomOutput(u8 byLvl, const char* fmt, ...)
{
	va_list list;
	char msg[MAX_LOG_MSG_LEN];
	int len;

	memset(msg, 0, sizeof(msg));
	if ((u8)(g_env.log) >= byLvl)
	{
		va_start(list, fmt);
		len = vsprintf(msg, fmt, list);
		va_end(list);
		OspPrintf(TRUE, FALSE, "%s", msg);
	}
	return len;
}

/* ��ʮ��������ʾ���ݾ䣨��OSP�и��ƣ� */
int DumpMsg(const u8* msg, int msglen, char* buf)
{
	int i, iChar, actLen;
	const u8 *ptr;
	ptr = msg;

	actLen = 0;
    for(i=0; i < msglen; i++, ptr++)
	{
		/* ����������������һ���ַ�����80��*/
		if (actLen > (MAX_LOG_MSG_LEN - 80))
		{
			break;
		}
		/* ÿ16��Ϊһ�У���ӡ���� */
		if( (i & 0x0F) == 0 )
		{
			actLen += sprintf(buf+actLen, "%4xh: ", i);
		}
		/* ��16���ƴ�ӡ���� */
		actLen += sprintf(buf+actLen, "%.2X ", *ptr);
		/* ��ӡ��Ӧ�Ŀ���ʾ�ַ� */
		if(((i + 1) & 0x0F) == 0) 
		{
			// ÿ���16��u8����ӡ���е��ַ���ʾ
			actLen += sprintf(buf+actLen, ";   "); 
			for(iChar=15; iChar>=0; iChar--)
			{
				// �ɴ�ӡ�ַ�
				if( (*(ptr - iChar) >= 0x21) && (*(ptr-iChar) <= 0x7E)) 
				{
					actLen += sprintf(buf+actLen, "%1c", *(ptr-iChar));
				}
				else  // ���ɴ�ӡ�ַ�
				{
					actLen += sprintf(buf+actLen, ".");
				}    
			}
			actLen += sprintf(buf+actLen, "\n");				
		}

		/* ���һ�У��Ҳ���16���� */
		if( (i == (msglen - 1)) &&  (((i + 1) & 0x0F) != 0))
		{
			u32 iBlank = 16 - ((i + 1) & 0x0F);
			u32 iBCount;
			
			for(iBCount = 0; iBCount < iBlank; iBCount++)
			{
				actLen += sprintf(buf+actLen, "   ");
			}
			actLen += sprintf(buf+actLen, ";   ");
			
			for(iChar=(i&0x0F); iChar>=0; iChar--)
			{
				if(((*(ptr-iChar)) >= 0x21)&&((*(ptr-iChar)) <= 0x7e))
				{
					actLen += sprintf(buf+actLen, "%1c", *(ptr - iChar));
				}
				else
				{
					actLen += sprintf(buf+actLen, ".");
				} 
			}				
		}			   
	} 

	actLen += sprintf(buf+actLen, (i>0) ? "\n\n":"\n");
	return actLen;
}


void	MdcomMsgLog(void)
{
	if (g_env.log)
		g_env.log = 0;
	else
		g_env.log = 1;
}

API void pmdmpxymsg(void)
{
	g_env.log = LVL_INFO;
}

API void npmdmpxymsg(void)
{
	g_env.log = LVL_ERR;
}

API void setmdmpxylog(u8 byLevel)
{
	if (byLevel > LVL_VERBOSE)
	{
		byLevel = LVL_VERBOSE;
	}
	g_env.log = byLevel;
}

/* ���ַ����н�����Ҫ���������ID */
static int  get_command_num(const char * command)
{
	int  ret_val = -1;
	char buffer[20];
	char *substr;
	int  mod_dem = 0;			// 1: mod 2: dem
	int  set_qry = 0;		    // 1: set 2: query
	int  has_err = 0;

	strncpy(buffer, command, 20);
	substr = strtok(buffer, " ");
	while (substr != NULL)
	{
		char *p = substr;

        
		while (*p) 
		{
			*p = tolower(*p);
			p++;
		}
		if (0 == strcmp(substr, "set"))
		{
			set_qry = 1;
		} 
		else if (0 == strcmp(substr, "query"))
		{
			set_qry = 2;
		}
		else if (0 == strcmp(substr, "mod"))
		{
			mod_dem = 1;
		}
		else if (0 == strcmp(substr, "dem"))
		{
			mod_dem = 2;
		}
		else if (0 == strcmp(substr, "bitrate"))
		{
			if (mod_dem == 1)
				ret_val = MDCOM_SET_MOD_BITRATE;
			else if (mod_dem == 2)
				ret_val = MDCOM_SET_DEM_BITRATE;
		}
		else if (0 == strcmp(substr, "freq"))
		{
			if (mod_dem == 1)
				ret_val = MDCOM_SET_MOD_FREQ;
			else if (mod_dem == 2)
				ret_val = MDCOM_SET_DEM_FREQ;
		}
		else if (0 == strcmp(substr, "clksrc"))
		{
			if (mod_dem == 1)
				ret_val = MDCOM_SET_MOD_CLKSRC;
			else if (mod_dem == 2)
				ret_val = MDCOM_SET_DEM_CLKSRC;
		}
		else if (0 == strcmp(substr, "output"))
		{
			if (mod_dem == 1)
				ret_val = MDCOM_SET_MOD_OUTPUT;
			else if (mod_dem == 2)
				has_err = 1;
		}
		substr = strtok(NULL, " ");
	}
	/* ��������� */
	if ((ret_val == -1) || has_err)
	{
		ret_val = -1;
	} else {
		if (set_qry == 2)
		{
			ret_val |= 0x100;
		}
	}
	return ret_val;
}


int MdcomCommand(const char* cmd, const char* ip, u16 port, u32 value)
{
	MdcomCmd item;
	int cmd_num;
	if ((cmd == NULL) || (ip == NULL) || (port == 0) || ((cmd_num =get_command_num(cmd)) == -1))
	{
		//����������ʾ������Ϣ
		printf("\n\nSet or query modem.\n");
		printf("MdcomCommand command, ip, port, value\n");
		printf("\tcommand: set|query mod|dem flag\n");
		printf("\t\tflag can be any one of: bitrate, freq, output, clksrc\n");
		printf("\tip: string\n");
		printf("\tport: integer\n");
		printf("\tvalue: integer\n");
		return -1;
	}

	item.cmd = cmd_num;
	item.ip = inet_addr(ip);
	item.port = port;
	item.val = value;
	
	return CommandModem(&item, 0);
}



int MdcomCheck(const char* cmd, const char* ip, u16 port)
{
	MdcomCmd item;
	int cmd_num;

	if ((cmd == NULL) || (ip == NULL) || (port == 0) || ((cmd_num =get_command_num(cmd)) == -1))
	{
		//����������ʾ������Ϣ
		printf("\n\nCheck the result of last command.\n");
		printf("MdcomCheck command, ip, port\n");
		printf("\tcommand: mod|dem flag\n");
		printf("\t\tflag can be any one of: bitrate, freq, output, clksrc\n");
		printf("\tip: string\n");
		printf("\tport: integer\n");
		return -1;
	}

	item.cmd = cmd_num;
	item.ip = inet_addr(ip);
	item.port = port;
	item.val = 0;

	CheckCmdStatus(&item);
	return item.val;
}

