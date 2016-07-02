

// BoardAction.cpp
//#pragma once

#include "boardaction.h"
extern GetInfo(u16 wAppNum, u16& wEvent);

/*=============================================================================
  �� �� ���� GetInfoFromBrd
  ��    �ܣ� �ӵ���ȡ��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 &wEvent
  �� �� ֵ�� BOOL 
=============================================================================*/
BOOL CBoardActTest::GetInfoFromBrd(u16 &wEvent)
{
    char ackbuf[4096];
	u16 ackbuflen = sizeof(ackbuf);

	if( OSP_OK != OspSend(MAKEIID(APP_SIMUBRDSSN, 1), GETINFO_FROM_BRD, 0, 0, 0, 
		MAKEIID(INVALID_APP, INVALID_INS), INVALID_NODE,
		ackbuf, ackbuflen))
	{
		return FALSE;
	}
		
	wEvent = *(u16 *)ackbuf;
	return TRUE;
}

/*=============================================================================
  �� �� ���� PostToBrd
  ��    �ܣ� ����Ϣ������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wEvent
             CServMsg cReportMsg
  �� �� ֵ�� void 
=============================================================================*/
void CBoardActTest::PostToBrd(u16 wEvent, CServMsg cReportMsg)
{
    OspPost(MAKEIID(APP_SIMUBRDSSN, 1), wEvent, cReportMsg.GetMsgBody(), cReportMsg.GetServMsgLen());
    return;
}

/*=============================================================================
  �� �� ���� TestBrdReg
  ��    �ܣ� ���Ե���ע��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CBoardActTest::TestBrdReg()
{
    CServMsg cReportMsg;
    u32 m_dwBrdIp = 0x02020202;  // Ip: 2.2.2.2
    u8  m_byChoice = 1;
    u16 wEvent = 0;

    TBrdPosition    m_tBoardPosition;
    m_tBoardPosition.byBrdID = 5;
    m_tBoardPosition.byBrdLayer = 0;
    m_tBoardPosition.byBrdSlot  = 4;

    cReportMsg.SetMsgBody((u8*)&m_tBoardPosition, sizeof(TBrdPosition));
    cReportMsg.CatMsgBody((u8*)&m_dwBrdIp, sizeof(u32));
    cReportMsg.CatMsgBody(&m_byChoice, sizeof(u8));
    PostToBrd(BOARD_MPC_REG, cReportMsg);
//    OspDelay(1000);
    OspSemTake( g_SimuSem );

    GetInfo(APP_SIMUBRDSSN, wEvent);

    CPPUNIT_ASSERT( wEvent == BOARD_MPC_REG_ACK); // 
    return;
}

/*=============================================================================
  �� �� ���� TestBrdGetReg
  ��    �ܣ� ���Ե���ȡ������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CBoardActTest::TestBrdGetReg()
{
    CServMsg cReportMsg;
    u16 wEvent = 0;
    TBrdPosition    m_tBoardPosition;
    m_tBoardPosition.byBrdID = 5;
    m_tBoardPosition.byBrdLayer = 0;
    m_tBoardPosition.byBrdSlot  = 4;

    cReportMsg.SetMsgBody((u8*)&m_tBoardPosition, sizeof(TBrdPosition));
    PostToBrd(BOARD_MPC_GET_CONFIG, cReportMsg);
//    OspDelay(1000);
    OspSemTake( g_SimuSem );

    GetInfo(APP_SIMUBRDSSN, wEvent);
	()
    CPPUNIT_ASSERT(wEvent == BOARD_MPC_GET_CONFIG+1);
    return;
}

/*=============================================================================
  �� �� ���� TestBrdMemAlarm
  ��    �ܣ� �����ڴ�澯
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CBoardActTest::TestBrdMemAlarm()
{
    TBoardAlarmInfo tAlarmData;
	u8 byAlarmObj[2];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];
    CServMsg cReportMsg;
    u16 wEvent = 0;

    TBrdPosition    m_tBoardPosition;
    m_tBoardPosition.byBrdID = 5;
    m_tBoardPosition.byBrdLayer = 0;
    m_tBoardPosition.byBrdSlot  = 4;

	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	tAlarmMsg.wEventId = SVC_AGT_MEMORY_STATUS;
	tAlarmMsg.abyAlarmContent[0] = m_tBoardPosition.byBrdLayer;
	tAlarmMsg.abyAlarmContent[1] = m_tBoardPosition.byBrdSlot;
	tAlarmMsg.abyAlarmContent[2] = m_tBoardPosition.byBrdID;
	memcpy((void*)tAlarmMsg.abyAlarmContent[3], "Error", sizeof("Error"));
			
	memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );
				
	memcpy(abyAlarmBuf+sizeof(m_tBoardPosition)+sizeof(u16), &tAlarmMsg, sizeof(tAlarmMsg) );

	*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(1); 
    
    cReportMsg.SetMsgBody((u8*)abyAlarmBuf, sizeof(sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg)));

	PostToBrd( SVC_AGT_MEMORY_STATUS, cReportMsg);
//    OspDelay(1000);
    OspSemTake( g_SimuSem );

    GetInfo(APP_SIMUBRDSSN, wEvent);
    CPPUNIT_ASSERT( wEvent == NMS_REV_MEMALARM);
    return;				
}

/*=============================================================================
  �� �� ���� TestBrdFileAlarm
  ��    �ܣ� �����ļ�ϵͳ�澯
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CBoardActTest::TestBrdFileAlarm()
{
    TBoardAlarmInfo tAlarmData;
	u8 byAlarmObj[2];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];
    u16 wEvent;
    CServMsg cReportMsg;

    TBrdPosition    m_tBoardPosition;
    m_tBoardPosition.byBrdID = 5;
    m_tBoardPosition.byBrdLayer = 0;
    m_tBoardPosition.byBrdSlot  = 4;

	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	tAlarmMsg.wEventId = SVC_AGT_FILESYSTEM_STATUS;
	tAlarmMsg.abyAlarmContent[0] = m_tBoardPosition.byBrdLayer;
	tAlarmMsg.abyAlarmContent[1] = m_tBoardPosition.byBrdSlot;
	tAlarmMsg.abyAlarmContent[2] = m_tBoardPosition.byBrdID;
	memcpy((void*)tAlarmMsg.abyAlarmContent[3], "Error", sizeof("Error"));
			
	memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );
				
	memcpy(abyAlarmBuf+sizeof(m_tBoardPosition)+sizeof(u16), &tAlarmMsg, sizeof(tAlarmMsg) );

	*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(1); 

    cReportMsg.SetMsgBody((u8*)abyAlarmBuf, sizeof(sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg)));
	PostToBrd( SVC_AGT_FILESYSTEM_STATUS, cReportMsg);
    OspSemTake( g_SimuSem );

    GetInfo(APP_SIMUBRDSSN, wEvent);
    CPPUNIT_ASSERT( wEvent == NMS_REV_FILESYSALARM);
    return;
}

/*=============================================================================
  �� �� ���� TestBrdModuleAlarm
  ��    �ܣ� ����ģ��澯
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CBoardActTest::TestBrdModuleAlarm()
{
    TBoardAlarmInfo tAlarmData;
	u8 byAlarmObj[2];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];
    u16 wEvent;
    CServMsg cReportMsg;

    TBrdPosition    m_tBoardPosition;
    m_tBoardPosition.byBrdID = 5;
    m_tBoardPosition.byBrdLayer = 0;
    m_tBoardPosition.byBrdSlot  = 4;


	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	tAlarmMsg.wEventId = BOARD_MODULE_STATUS;
	tAlarmMsg.abyAlarmContent[0] = m_tBoardPosition.byBrdLayer;
	tAlarmMsg.abyAlarmContent[1] = m_tBoardPosition.byBrdSlot;
	tAlarmMsg.abyAlarmContent[2] = m_tBoardPosition.byBrdID;
	memcpy((void*)tAlarmMsg.abyAlarmContent[3], "Error", sizeof("Error"));
			
	memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );
				
	memcpy(abyAlarmBuf+sizeof(m_tBoardPosition)+sizeof(u16), &tAlarmMsg, sizeof(tAlarmMsg) );

	*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(1); 

    cReportMsg.SetMsgBody((u8*)abyAlarmBuf, sizeof(sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg)));
	PostToBrd( BOARD_MODULE_STATUS, cReportMsg);
    OspSemTake( g_SimuSem );
    
    GetInfo(APP_SIMUBRDSSN, wEvent);
    CPPUNIT_ASSERT( wEvent == NMS_REV_MODULEALARM);
    return;
}

/*=============================================================================
  �� �� ���� TestBrdLedAlarm
  ��    �ܣ� ����Led�澯
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CBoardActTest::TestBrdLedAlarm()
{
    TBoardAlarmInfo tAlarmData;
	u8 byAlarmObj[2];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];
    u16 wEvent;
    CServMsg cReportMsg;

    TBrdPosition    m_tBoardPosition;
    m_tBoardPosition.byBrdID = 5;
    m_tBoardPosition.byBrdLayer = 0;
    m_tBoardPosition.byBrdSlot  = 4;


	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	tAlarmMsg.wEventId = BOARD_LED_STATUS;
	tAlarmMsg.abyAlarmContent[0] = m_tBoardPosition.byBrdLayer;
	tAlarmMsg.abyAlarmContent[1] = m_tBoardPosition.byBrdSlot;
	tAlarmMsg.abyAlarmContent[2] = m_tBoardPosition.byBrdID;
	memcpy((void*)tAlarmMsg.abyAlarmContent[3], "Error", sizeof("Error"));
			
	memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );
				
	memcpy(abyAlarmBuf+sizeof(m_tBoardPosition)+sizeof(u16), &tAlarmMsg, sizeof(tAlarmMsg) );

	*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(1); 

    cReportMsg.SetMsgBody((u8*)abyAlarmBuf, sizeof(sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg)));
	PostToBrd( BOARD_LED_STATUS, cReportMsg);
    OspDelay(1000);

    GetInfo(APP_SIMUBRDSSN, wEvent);
    CPPUNIT_ASSERT( wEvent == NMS_REV_LEDALARM);
    return;
}