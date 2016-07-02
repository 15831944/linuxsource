#ifndef __PARSE_PUBLIC_H__
#define __PARSE_PUBLIC_H__

#include "util.h"
#include "pktbuff.h"
/* Define if you enable IPv6 support */
#define INET6 1

#define PARSE_MOUDLE "parse"

#define	PARSE_PACKET_NAME_MAX	64

/* �豸���ͱ�־ */
#define DEV_TYPE_STA	0
#define DEV_TYPE_AP		1

/* ��ӱ�־λ */
#define ENQUEUE_FLAG_ENABLE  1
#define ENQUEUE_FLAG_DISABLE 0

/* ���������� */
#define DATA_STREAM_DIR_UP   	0
#define DATA_STREAM_DIR_DOWN	1

/* ����ͳ�Ƴɹ���־ */
#define DATA_STAT_SUCCESS 1

/* ���ķ���ͳ�� */
typedef struct __packet_statistics{
	u32 uiTimeStamp;
	u8	ucEnqueueFlag;
	u8  ucType;	/*AP or STA*/
	u8  ucMac[6];
	u8  ucRssi;
	u8  ucChannel;
	u8  ucRealChannel;
	u8  ucGot_qq;
	u8  ucQq[PARSE_PACKET_NAME_MAX];
	u8  ucGot_tb;
	u8  ucTb[PARSE_PACKET_NAME_MAX];
	u8  ucGot_wb;
	u8  ucWb[PARSE_PACKET_NAME_MAX];
	/*�����AP��������Ϣ������*/
	u8  ucSsid[PARSE_PACKET_NAME_MAX];
	//u8  ucEncry[PARSE_PACKET_NAME_MAX];
	//u8  ucAuth[PARSE_PACKET_NAME_MAX];
	u32	uiAuth;
	u32 uiEncry;

	PPKT_HDR pktHdr;
}PACKET_STATISTICS_T;



#endif /* __PARSE_PUBLIC_H__ */
