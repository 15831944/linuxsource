/*==============================================================================
    �ļ�����abnormitytest.cpp
    
===============================================================================*/
// AbnormityTest.cpp 
#include "Testevent.h"
#include "AbnormityTest.h"
#include "boardaction.h"

/*=============================================================================
  �� �� ���� UndefinedAlarmTest
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CAbnormityTest::UndefinedAlarmTest()
{

}

/*=============================================================================
  �� �� ���� PostToBrd
  ��    �ܣ� ����Ϣ��ģ�ⵥ�� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wEvent
             CServMsg cReportMsg
  �� �� ֵ�� void 
=============================================================================*/
void CAbnormityTest::PostToBrd(u16 wEvent, CServMsg cReportMsg)
{
    OspPost(MAKEIID(APP_SIMUBRDSSN, 1), wEvent, cReportMsg.GetMsgBody(), cReportMsg.GetServMsgLen());
    return;
}

/*=============================================================================
  �� �� ���� GetInfoFromBrd
  ��    �ܣ� �ӵ���ȡ��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 &wEvent
  �� �� ֵ�� BOOL 
=============================================================================*/
BOOL CAbnormityTest::GetInfoFromBrd(u16 &wEvent)
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
  �� �� ���� HiFrequencyTest
  ��    �ܣ� ����Ƶ�����澯��Agent
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CAbnormityTest::HiFrequencyTest()
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

    cReportMsg.SetMsgBody((u8*)abyAlarmBuf, sizeof(sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg)));  ()
	
    for(u32 dwLp = 0; dwLp < 100; dwLp++)   
    {
        PostToBrd( SVC_AGT_FILESYSTEM_STATUS, cReportMsg);
        OspDelay(1000);

        GetInfoFromBrd(wEvent);
        CPPUNIT_ASSERT( wEvent == NMS_REV_FILESYSALARM);
    }

    return;

}



