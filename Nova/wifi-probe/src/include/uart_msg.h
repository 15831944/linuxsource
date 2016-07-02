#ifndef _UART_MSG_H_
#define _UART_MSG_H_

#include "common_types.h"

/*1000 - frmlen - msgID - crc16.*/
#define MAX_MSG_MTU		1000
#define MAX_BODY_LEN	(MAX_MSG_MTU - sizeof(UINT16) - sizeof(UINT8) - sizeof(UINT16))
#define MAX_MSG_CRC16		2

struct MSG_BODY{
	UINT16 frmLen;
	UINT8 msgID;
	char body[MAX_BODY_LEN];
}__attribute__((packed));

typedef enum{
	
	UART_MSG_HB_REPORT       = 0x01,	/*probe -> host*/
	UART_MSG_PROBE_REPORT    = 0x02, /*probe -> host*/
	
	UART_MSG_ONLINE_REQUEST  = 0x03,	/*probe -> host*/
	UART_MSG_ONLINE_RESPONSE = 0x83, /*host -> probe*/

	UART_MSG_CONFIG_REQUEST  = 0x84,	/*host -> probe*/ 
	UART_MSG_CONFIG_RESPONSE = 0x04, /*probe -> host*/

	UART_MSG_IMAGE_REQUEST   = 0x85,	/*host -> probe*/
	UART_MSG_IMAGE_RESPONSE	 = 0x05,	/*probe -> host*/

	UART_MSG_END
	
}UART_MSG_TYPE;

/****************probe report struct.*******************/
enum{
	DEV_STA = 0,
	DEV_AP
};

typedef enum{
	QQ_ELEMENT_ID = 0x1,
	TB_ELEMENT_ID,
	WB_ELEMENT_ID,
	AP_SSID_ELEMENT_ID,
	AP_AUTH_ELEMENT_ID,
	AP_ENCRY_ELEMENT_ID,
	ELEMENT_END
}PROBE_ELEMENT_ID;

#define ELEMENT_OPT_TYPE(Option)   (*((unsigned char *)Option))                            
#define ELEMENT_OPT_LEN(Option)    (*(((unsigned char *)Option) + 1))                     
#define ELEMENT_OPT_VALUE(Option)  (((unsigned char*)Option) + 2)                          
#define ELEMENT_OPT_NEXT(Option)   (((unsigned char*)Option) + 2 + ELEMENT_OPT_LEN(Option))  

/*
 *	Option struction.
 */
typedef struct _ELEMENT_OPT
{
    UINT8 type;
    UINT8 Length;
    UINT8 Value[1];
}ELEMENT_OPT, *PELEMENT_OPT;


struct msg_probe_report{
	UINT16 msgLen;
	UINT32 timestamp;
	UINT8 devtype;
	INT16 rssi;
	UINT8 mac[6];
	UINT8 chn;
}__attribute__((packed));

#define PROBE_REPORT_MSG_COMM_LEN		(sizeof(struct msg_probe_report))
#define PROBE_OPTIONS_MAX_LEN			(MAX_BODY_LEN - PROBE_REPORT_MSG_COMM_LEN)

/***********************online msg.**********************/
#define MSG_ONLINE_RETRASMIT_TIME	5	/*seconds*/
struct msg_online_request{
	char hwversion[8];
	char swversion[8];
	UINT32 cur_scan_ch_mask;/*��ǰ�豸ɨ���ŵ���MASK*/
	UINT16 ch_switch_time; /*�ŵ��л�ʱ��,��λ=��*/
}__attribute__((packed));

struct msg_online_response{
	UINT32 set_scan_ch_mask;/*����ɨ���ŵ�mask�� ����ɨ��1-4=0x0F��0xFFFFFFFF��ʾ auto����̽���豸�Լ�ѡ��*/
	UINT16 set_ch_swith_time; /*�ŵ��л�ʱ��,��λ=��*/
	UINT16 set_report_time_per_mac;/* �����ڶ೤ʱ������ͬMACֻ���ϱ�һ����0����ʾȫ���������ˡ���λ=�롣*/
}__attribute__((packed));

/************************config msg.**********************************/
struct msg_config_request{
	UINT32 set_scan_ch_mask;/*����ɨ���ŵ�mask�� ����ɨ��1-4=0x0F��0xFFFFFFFF��ʾ auto����̽���豸�Լ�ѡ��*/
	UINT16 set_ch_swith_time;/*�����ŵ�ɨ���л�ʱ��,��λ=��*/
	UINT16 set_report_time_per_mac;/* �����ڶ೤ʱ������ͬMACֻ���ϱ�һ����0����ʾȫ���������ˡ���λ=�롣*/
}__attribute__((packed));

struct msg_config_response{
	UINT32 retcode;	/*0 = OK. 1 = failure.*/
};


/*****************************IMAGE MSG.*******************************************/
#define MAX_IMAGEDATA_PER_MSG		128
#define MAX_IMAGE_RESPONSE_TIMEOUT	1
#define MAX_IMAGE_RETRANSMIT		3

struct msg_image_request{
	UINT32 blockid;
	char content[MAX_IMAGEDATA_PER_MSG];
};

struct msg_image_response{
	UINT32 blockid_ack;
};

typedef struct msg_funcs{
	
	INT32 (*valid_messages)(char *inbuff, int len);
	INT32 (*build_msg_online_request)(struct msg_online_request *in_online_req, char *outbuff, int *outlen);
	INT32 (*parse_msg_online_response)(struct msg_online_response *out_online_resp, char *inbuff, int len);
	
}MSG_FUNCS;

#endif

