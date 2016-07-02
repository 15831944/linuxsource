

#include "cmdtable.h"
#include "cmdprotocol.h"
#include "msgpipe.h"


extern MdcomEnv g_env;

typedef struct
{
    cmd_t id;               //Ψһ�������ʶ
    u8 num;                 //Э���ж�����������
    u8 len;                 //���е����ݶγ���
    u8 mask1;               //Ҫ���õı�־λ(Ŀǰֻ��һ������������)
} ProtlCmdDesp;

ProtlCmdDesp CmdDesps[] = 
{
    { MDCOM_SET_MOD_FREQ,       0x41,   0x24,   0x01},
    { MDCOM_SET_MOD_OUTPUT,     0x41,   0x24,   0x08},
    { MDCOM_SET_MOD_BITRATE,    0x42,   0x14,   0x01},
    { MDCOM_SET_MOD_CLKSRC,     0x42,   0x14,   0x40},
    { MDCOM_SET_DEM_FREQ,       0x81,   0x22,   0x01},
    { MDCOM_SET_DEM_BITRATE,    0x82,   0x1E,   0x01},
    { MDCOM_SET_DEM_CLKSRC,     0x82,   0x1E,   0x40}
};

/* ����У��� */
static int GenChecksum(u8 *data)
{
	int i, len = data[6] + 10 - 2;
    int sum = 0;
	for(i = 1; i < len; i++)
	{
		sum += data[i];
		sum %= 256;
	}
	sum = 256 - sum;
	return sum;
}

static void FillSendPack(u8 *data)
{
    int len = data[6] + 10;
    /* Э���ͷ */
    data[0] = 0xFF;
    data[1] = 0xA5;
    data[2] = 0x01;
    data[3] = 0x01;
    data[5] = 0x01;
    /* Э���β */
    data[len - 3] = 0x96;
    data[len - 1] = 0xFF;
    data[len - 2] = (u8) GenChecksum(data);
};

/* �����������������װһ��Э��� */
int AssemSendPack(cmd_t cmd, u32 val, u8 *data)
{
    u8 idx = cmd & 0x0f;

	if (idx >= MDCOM_SET_MAX)
        return -1;

	/* ���ú�����֤�����㹻�� */
	memset(data, 0, CmdDesps[idx].len + 10);
    data[4] = CmdDesps[idx].num;
    data[7] = CmdDesps[idx].mask1;
    
    /* ����Query�������Ϊ�� */
    if (cmd & 0x0f00)
    {
        data[6] = 0x00;
        FillSendPack(data);
        return 10;
    }
    data[6] = CmdDesps[idx].len;
    
    ////////////////////////////////////////////////////////////
    switch(idx)
    {
    case MDCOM_SET_MOD_FREQ:
        data[15] = (u8)val;                        //freq num
	    data[16] = (u8)(val >> 8);                 //freq num
	    data[17] = (u8)(val >> 16);                //freq num
	    data[18] = (u8)(val >> 24);                //freq num
		break;
    case MDCOM_SET_MOD_OUTPUT:
        data[11] = (u8)val;
		break;
	case MDCOM_SET_MOD_BITRATE:
        data[17] = (u8)val;                        //freq num
	    data[18] = (u8)(val >> 8);                 //freq num
	    data[19] = (u8)(val >> 16);                //freq num
	    data[20] = (u8)(val >> 24);                //freq num
		break;
	case MDCOM_SET_MOD_CLKSRC:
		data[13] = (u8)(val << 4);
		break;
	case MDCOM_SET_DEM_FREQ:
		data[15] = (u8)val;                        //freq num
	    data[16] = (u8)(val >> 8);                 //freq num
	    data[17] = (u8)(val >> 16);                //freq num
	    data[18] = (u8)(val >> 24);                //freq num
		break;
	case MDCOM_SET_DEM_BITRATE:
		data[17] = (u8)val;                        //freq num
	    data[18] = (u8)(val >> 8);                 //freq num
	    data[19] = (u8)(val >> 16);                //freq num
	    data[20] = (u8)(val >> 24);                //freq num
		break;
	case MDCOM_SET_DEM_CLKSRC:
		data[13] = (u8)(val << 4);
		break;
	default:
        debug("Invalid command.\n");
		return -1;
    }
    FillSendPack(data);
    return data[6] + 10;
}

/* ��һ��Э����зֽ��ָ�������ֵ */
int DisassemRecvPack(const u8 *buf, int buflen, u32 ip, u16 port)
{
    int idx, curr, sum;
    u32 val;
    InCmd* item;
    cmd_t cmd;
    int errpack;								/* ָʾ���ڽ����İ��д��� */				
	int succpack_num = 0;						/* �ܹ����������İ��ĸ��� */
    u8 data[PROTLPACK_MAXSIZE];
    int packlen;

    /* �ӻ�������������Ч��Э�������������� */
    curr = 0;
    while (curr < buflen)
    {        
        if ((buflen - curr) < 10)
        {
            break;
        }
        /* Э�����0x5A,01,01��ͷ�������ڵ�7���ֽ� */
        if ((buf[curr]      != 0x5A) ||
            (buf[curr + 1]  != 0x01) ||
            (buf[curr + 2]  != 0x01) ||
            (buf[curr + 6]   > (buflen - curr - 9)))
        {
            curr++;
            continue;
        }
        packlen = buf[curr + 6] + 9;
        /* �����ڶ���Ϊ0x96 */
        if (buf[curr + packlen - 2] != 0x96)
        {
            curr++;
            continue;
        }        
        sum = 0;
        for(idx = 0; idx < packlen; idx++)
        {
            sum += buf[curr + idx];
        }
        
        /* ���У��ͼ������� */
        if ((sum % 256 != 0) ||
            ((buf[curr + 3] != 0x41) &&
            (buf[curr  + 3] != 0x42) &&
            (buf[curr  + 3] != 0x81) &&
            (buf[curr  + 3] != 0x82)))
        {            
            /*��У��ͻ������ֲ��ԣ����������� */
            curr = curr + packlen;
            continue;
        };
		
		/* ͨ����֤ */
		memcpy(data, &buf[curr], packlen);
        curr = curr + packlen;
		succpack_num++;      

        /* ����Э��� */
        for (idx = 0; idx < MDCOM_SET_MAX; idx++)
        {
            if (CmdDesps[idx].num != data[3])
            {
                continue;
            }
			errpack = 0;
            cmd = CmdDesps[idx].id;
            /* ������������������ */
            switch(cmd)
            {
            case MDCOM_SET_MOD_FREQ:
                val = data[18];
                val = (val << 8) | data[17];
                val = (val << 8) | data[16];
                val = (val << 8) | data[15];
                if (val == 0)
                {
                    debug("Warning: SET_MOD_FREQ is zero!\n");
                    errpack = -1;
                }
		        break;
            case MDCOM_SET_MOD_OUTPUT:
                val = data[11] & 0x1;
		        break;
	        case MDCOM_SET_MOD_BITRATE:
                val = data[20];
                val = (val << 8) | data[19];
                val = (val << 8) | data[18];
                val = (val << 8) | data[17];
                if (val == 0)
                {
                    debug("Warning: SET_MOD_BITRATE is zero!\n");
                    errpack = -1;
                }
	    	    break;
	        case MDCOM_SET_MOD_CLKSRC:		
                val = data[13] >> 4;
		        break;
	        case MDCOM_SET_DEM_FREQ:
		        val = data[18];
                val = (val << 8) | data[17];
                val = (val << 8) | data[16];
                val = (val << 8) | data[15];
                if (val == 0)
                {
                    debug("Warning: SET_DEM_FREQ is zero!\n");
                    errpack = -1;
                }
		        break;
	        case MDCOM_SET_DEM_BITRATE:
    		    val = data[20];
                val = (val << 8) | data[19];
                val = (val << 8) | data[18];
                val = (val << 8) | data[17];
                if (val == 0)
                {
                    debug("Warning: SET_DEM_BITRATE is zero!\n");
                    errpack = -1;
                }
    		    break;
	        case MDCOM_SET_DEM_CLKSRC:
    	    	val = data[13] >> 4;
		        break;
    	    default:    
                break;
            }
            /* �����д��������� */
            if (errpack == 0)
            {
                debug("Receive: %d at %d\n", cmd, GetTick());
                item = LookforCmdTable(g_env.table, ip, port, cmd);
				if (item != NULL)
				{
					item->ret = val;
					item->status = ics_receive;
				}
            }
        }
    }    
    return succpack_num;
}


