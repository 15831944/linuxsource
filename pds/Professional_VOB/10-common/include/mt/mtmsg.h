/*******************************************************************************
 *  ģ����   : �ն�ҵ��ϵͳӦ�ýӿ�                                            *
 *  �ļ���   : mtmsg.h                                                         *
 *  ����ļ� :                                                                 *
 *  ʵ�ֹ��� : ��Ϣ����                                                        *
 *  ����     : ������                                                          *
 *  �汾     : V3.0  Copyright(c) 1997-2005 KDC, All rights reserved.          *
 *                                                                             *
 *    =======================================================================  *
 *  �޸ļ�¼:                                                                  *
 *    ��  ��      �汾        �޸���      �޸�����                             *
 *    2005/8/17   4.0         ������      ����                                 *
 *                                                                             *
 *******************************************************************************/

#include "osp.h"
#include "mtconst.h"
#include "mttype.h"
#include "mtstruct.h"


#ifndef _MT_MESSAGE_H_
#define _MT_MESSAGE_H_




  

#define node_ui    0x001
#define node_mtc   0x002
#define node_peer  0x004  
#define node_pcdualvideo 0x008
#define node_batchconfig 0x010 
#define node_mtautotest  0x012
  

/*


  MTMsg���ݽṹ

  


*/
struct TMtMsg
{
protected:
	u16 m_wEvent;
	u16 m_wBodyLen;
	u32 m_dwSrcIId;
	u32 m_dwSrcNode;

	u8  m_abyMsg[MT_MAXLEN_MESSAGE+4];

	TMtMsg(){memset(this,0,sizeof(TMtMsg));}
};

class CMtMsg : public TMtMsg
{
protected:
	enum EmMsgCodeIndex
	{
		CODEC_FLAG        = 0 ,//���ֽ� = 0��ʾδ���� = 1��ʾ��������
		CODEC_HOMEPLACE   = 1,
		CODEC_User        = 2,
		CODEC_RESERVER3   = 3,
		CODEC_BODY = 4,

	};
	enum EmMsgDataType
	{
		emRawData     = 0 ,//δ��������ԭʼ����
		emCodeData         //������������
	};
		//   ���¼�����Ϊ  wEvent �� buf�����ݽ���ΪCMtMsg
	BOOL Decode(u16 wEvent ,u8* buf ,int len);
	//��Ϣ�Ƿ񾭹�����
	BOOL IsMsgCoder()
	{
		return m_abyMsg[0] ? TRUE:FALSE;
	}
	//   ��CMtMsg�����ݰ���wEvent�¼����ͱ���Ϊ���洢��buf��
	int Code(u16 wEvent ,u8* buf ,int len );

public:
	CMtMsg(){};
	CMtMsg(const CMessage* ptMsg )
	{
		m_dwSrcIId  = ptMsg->srcid;
		m_dwSrcNode = ptMsg->srcnode;
		Decode(ptMsg->event ,(u8*)ptMsg->content ,ptMsg->length );
	}

	CMtMsg(u16 wEvent ,void* buf ,int len )
	{
		Decode( wEvent ,(u8*)buf ,len );
	}
	

	

	
	
	
	//   ��CMessage��Ϣ����ΪCMtMsg
//	BOOL Load(const CMessage* ptMsg )
//	{ 
//		m_dwSrcIId = ptMsg->srcid;
//		m_dwSrcNode = ptMsg->srcnode;
//		return Decode(ptMsg->event ,ptMsg->content ,ptMsg->length );
//	}	
//
//	//  ��buf�е����ݼ��ص�CMtMsg��
//	BOOL Load(u16 wEvent ,void* buf ,int len )
/*	{ return Decode( wEvent ,buf ,len );}*/	
	//������Ϣ
	int Post(u32 dstid,
		     u32 dstnode =0,	
			 u32 srciid  = MAKEIID(INVALID_APP, INVALID_INS), 
			 u32 srcnode = INVALID_NODE );


	//�����Ϣ��ָ��
	u8*	GetBody(){	return m_abyMsg+CODEC_BODY;}
	
	//�����Ϣ�峤��
	u16	GetBodyLen() {	return m_wBodyLen; 	}

	//�����Ϣָ��
	u8*	GetMsg(){	return m_abyMsg;}
	
	//�����Ϣ����
	u16	GetMsgLen() {	return m_wBodyLen+4; 	}

	//������Ϣ������
	BOOL SetBody(const void* buf ,u16 len )
	{ 
		if( len > MT_MAXLEN_MESSAGE )
			return FALSE;
		if( NULL == buf || 0 == len )
		{
			m_wBodyLen = 0;
			return TRUE;
		}
		memcpy(m_abyMsg+CODEC_BODY ,buf ,len);
		m_wBodyLen = len;
		return TRUE;
	}

	//�����Ϣ������
	BOOL CatBody(const void* buf ,u16 len )
	{
		if( len + m_wBodyLen > MT_MAXLEN_MESSAGE )
			return FALSE;

		memcpy( m_abyMsg +CODEC_BODY +m_wBodyLen ,buf ,len);
		m_wBodyLen += len;
		return TRUE;

	}

	void SetEvent(u16 event ) { m_wEvent = event ; }
	u16  GetEvent() { return m_wEvent; }


	u32 GetSrcIId(){ return m_dwSrcIId; }
	u32 GetSrcNode(){ return m_dwSrcNode; }

	//��ȡ��Ϣ�ķ�����
	u8   GetHomeplace()
	{
		return m_abyMsg[CODEC_HOMEPLACE];
	}

	void SetHomeplace( u8 byNode )
	{
		m_abyMsg[CODEC_HOMEPLACE] = byNode;
	}

	u8   GetUserData()
	{
		return m_abyMsg[CODEC_User];
	}

	void   SetUserData(u8 byData)
	{
		m_abyMsg[CODEC_User] = byData;
	}
protected:
    void  DisplayMsgStream(u32 srcid , u32 dstid);
};

#endif//!_MT_MESSAGE_H_

