/*****************************************************************************
   ģ����      : Recorder
   �ļ���      : RecServ_Def.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: Recorder��Server������ݶ���
   ����        : 
   �汾        : V1.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2010/08/26  1.0         �� ��         ����
******************************************************************************/

#ifndef RECSERVDEF_H
#define RECSERVDEF_H

#include "mcustruct.h"
#include "evmcueqp.h"
#include "evrec.h"
#include "RecError.h"

#ifdef WIN32
#pragma comment( lib, "ws2_32.lib" ) 
#pragma pack( push )
#pragma pack( 1 )
#define window( x )	x
#else
#include <netinet/in.h>
#define window( x )
#endif


// ���񷵻�ֵ����
typedef u32 RET_TYPE;				// ���񷵻�ֵ���Ͷ���
const RET_TYPE RET_PASSED = 0;		// û�н��ܴ�����,pass��
const RET_TYPE RET_DONE   = 1;		// ���ܴ����񲢴������
const RET_TYPE RET_FAILED = 2;		// ���ܴ�����,������ʧ��


// RecServer��AppId��MTͨ����AppId���������������
#define     AID_REC_SERVER                AID_MCU_BGN + 49    // APP ID of Rec Server

// �����ID��¼���������֪ͨMT������æ
#define ERR_REC_SERVER_BUSY					ERR_REC_BGN + 30			  //�����ļ�������
// �����ID���ն�����Ƿ�
#define ERR_REC_SERVER_INVALID				ERR_REC_BGN + 31			  //�ն�����Ƿ�
// �����ID�������쳣
#define ERR_REC_SERVER_INVALID_PARAM		ERR_REC_BGN + 32			  //��������
// �����ID�������쳣
#define ERR_REC_SERVER_ERR					ERR_REC_BGN + 33			  //�����쳣

//�ն���¼�������������ע������MT->REC SERVER����Ϣ��ΪTMtRegReq�ṹ
OSPEVENT( MT_RECSERV_REG_REQ,			    EV_MCUREC_BGN  + 109);
//¼�������������Ӧ��REC SERVER->MT����Ϣ��Ϊ��Ϣ��ΪTMtRegAck
OSPEVENT( RECSERV_MT_REG_ACK,				EV_MCUREC_BGN + 110 );
//¼����������ܾ�Ӧ��REC SERVER->MT������Ϣ��
OSPEVENT( RECSERV_MT_REG_NACK,				EV_MCUREC_BGN + 111 );

//[��ʱ����]�ն���¼�������������ע����MT->REC SERVER����Ϣ����
OSPEVENT( MT_RECSERV_UNREG_CMD,			    EV_MCUREC_BGN  + 112);
//[��ʱ����]¼��������������Ͽ����ն˵�����
OSPEVENT( RECSERV_MT_EXIT_CMD,				EV_MCUREC_BGN + 115 );

//MTָ��ʱ����û��Ӧ, �����Ͽ�
OSPEVENT( RECSERV_REC_MT_DISC,				EV_MCUREC_BGN + 116 );
//���ն�֮��ʧȥ����
OSPEVENT( EV_MT_DISCONNECTED,		    	EV_MCUREC_BGN + 117 );

//��ӡ¼�����������״̬��Ϣ
OSPEVENT( EV_RECSERV_SHOW_STATUS,	    	EV_MCUREC_BGN + 131 );

// ����ظ��ͻ��˵Ľṹ��
typedef TPeriEqpRegReq TMtRegAck;
// ����ͻ�������ע��Ľṹ��
typedef TPeriEqpRegAck TMtRegReq;


// ����ڵ���Ϣ
typedef struct tagNodeInfo
{
    u32 m_dwNodeId;									// �ڵ��
    u32 m_dwAppId;									// Instance�ţ���MAKEID(appid��instanceid)����
}TNodeInfo, *PTNodeInfo;


// �ն�����ע�����Ϣ
struct TMTRegInfo
{
    TMtRegReq m_tMtRegReq;
    s8        m_aliase[MAXLEN_RECORD_NAME + 1];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// ¼���ļ��ļ�������󳤶ȣ�����·����
#define MAX_REC_FILE_NAME 128
// �ն˱�������󳤶�
#define MAX_H323ALIASNAME_LEN 32

// �����ն˵�Ĭ�ϱ���
#define MT_DEFAULT_NAME  "localMt"

// �ն�����¼�����������ʼ¼���msg��content������
typedef struct tagTRecStartParam
{
public:
    //TEqp			TEqp;
    // �¼ӵ�
    TRecRtcpBack    tRecRtcpBack;						// �ش���Ϣ
    // �¼ӵ�
    u8              byNeedPrs;							//�Ƿ��ش�
    TRecStartPara	tRecStartPara;
    TMediaEncrypt	tPriVideoEncrypt;
    TMediaEncrypt	tSecVideoEncrypt;
    TMediaEncrypt	tAudVideoEncrypt;
    TDoublePayload	tPriPayload;
    TDoublePayload	tSecPayload;
    TDoublePayload	tAudPayload;
    s8				achFileName[MAX_REC_FILE_NAME+1];
    TCapSupportEx	tCapSupportEx;
  	s8				achAliase[MAX_H323ALIASNAME_LEN+1];
public:
    tagTRecStartParam()
    {
        memset( this->achFileName, 0, MAX_REC_FILE_NAME+1 );
        memset( this->achAliase, 0, MAX_H323ALIASNAME_LEN+1 );
    };
}TRecStartParam
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// ����Nackʱ֪ͨ�ն˵Ĵ�����ṹ
typedef struct tagErrInfo
{
    u16 m_wErrCode;									// �����
}TErrorInfo
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// Recservģ���log�������
extern void RecServLog( s8* pszFmt, ... );

// Recservģ������ȼ�log�������
extern void RecServLevelLog( const u8& byLevel, s8* pszFmt, ... );

// log���ȼ����塣ֵԽС�����ȼ�Խ��
const u8 RECSERV_LEVEL_LOG_ERROR    = 0;			// ������Ϣ��������ʱ���
const u8 RECSERV_LEVEL_LOG_WARNING  = 4;			// ������Ϣ�����淢��ʱ���
const u8 RECSERV_LEVEL_LOG_COMMON   = 6;			// ��ͨ��Ϣ�����һ���״̬��Ϣ
const u8 RECSERV_LEVEL_LOG_BELOW    = 8;			// �ͼ���Ϣ��Ϊ������ԣ����һЩ�ǳ��ͼ�����Ϣ����msg���ݵ�


#endif