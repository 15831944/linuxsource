

// NmsActTest.cpp
#include "NmsActTest.h"

TAgentCallBack g_AgentCallBack;

#define MCUID_TEST        192
#define MCUALIAS_TEST     "MCU16"
#define MCUE164_TEST     "88888"
#define CASTPORT_TEST     60100
#define RECVPORT_TEST     61000
#define H245PORT_TEST     60002
#define MCU_MTNUM         190
#define MCU_TRAPIPADDR    0x64010101    // 100.1.1.1
#define TRAPWCOMMUNITY    "private"
#define TRAPRCOMMUNITY    "public"

/*=============================================================================
  �� �� ���� ActMcuId
  ��    �ܣ� ����Mcu Id
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActMcuId(void)
{
    u16  wSrcId = 0;
    u16  wDstId = 193;
    u32  dwNode = NODE_MCUID;
    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcId == MCUID_TEST);

    g_AgentCallBack(dwNode, 1, wDstId, sizeof(wDstId));

    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    CPPUNIT_ASSERT(wSrcId == wDstId);
    return;
}

/*=============================================================================
  �� �� ���� ActMcuAlias
  ��    �ܣ� ����mcu ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActMcuAlias(void)
{
    u8 achSrcAlias[32];
    u8 achDstAlias[32];
    memset(achSrcAlias, '\0', sizeof(achSrcAlias));
    memset(achDstAlias, '\0', sizeof(achDstAlias));
    
    u32 dwNode = NODE_MCUALIAS;
    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    OspDelay(500);
    CPPUNIT_ASSERT(0 == strcmp(achSrcAlias, MCUALIAS_TEST));
    
    memcpy(achDstAlias, "MCU15", sizeof("MCU15"));
    g_AgentCallBack(dwNode, 1, achDstAlias, sizeof(achDstAlias));

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    CPPUNIT_ASSERT(0 == strcmp(achDstAlias, achSrcAlias));
    return;
    
} 
   
/*=============================================================================
  �� �� ���� ActMcuE164
  ��    �ܣ� ����E164��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActMcuE164(void)
{
    u8 achSrcAlias[32];
    u8 achDstAlias[32];
    memset(achSrcAlias, '\0', sizeof(achSrcAlias));
    memset(achDstAlias, '\0', sizeof(achDstAlias));
    
    u32 dwNode = NODE_MCUE164NUMBER;
    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    OspDelay(500);
    CPPUNIT_ASSERT(0 == strcmp(achSrcAlias, MCUE164_TEST));
    
    memcpy(achDstAlias, "99999", sizeof("99999"));
    g_AgentCallBack(dwNode, 1, achDstAlias, sizeof(achDstAlias));

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    CPPUNIT_ASSERT(0 == strcmp(achDstAlias, achSrcAlias));
    return;
    
}

void CNmsActTest::ActIsNeedBas(void)
{
    
}
 
// get/set network info
/*=============================================================================
  �� �� ���� ActGk
  ��    �ܣ� ����Gk��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActGk(void)
{
    u32 dwSrcIp = 0;
    u32 dwDstIp = 0x01010101; // 1.1.1.1
    u32 dwNode = NODE_MCUNETGKIPADDR;

    g_AgentCallBack(dwNode, 0, &dwSrcIp, sizeof(dwSrcIp));
    CPPUNIT_ASSERT(dwSrcIp == 0);

    g_AgentCallBack(dwNode, 1, dwDstIp, sizeof(dwDstIp));
    OspDelay(1000);
    g_AgentCallBack(dwNode, 0, &dwSrcIp, sizeof(dwSrcIp));
    CPPUNIT_ASSERT(dwSrcIp == dwDstIp);

    return;

}
/*=============================================================================
  �� �� ���� ActCast
  ��    �ܣ� �����鲥Ip�Ͷ˿�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActCast()
{
    u32 dwNode1 =  NODE_MCUNETMULTICASTIPADDR;
    u32 dwNode2 = NODE_MCUNETMULTICASTPORT;
    u32 dwSrcIp = 0;
    u32 dwDstIp = 0xff0a0a0a;  // 255.10.10.10
    
    u16 wSrcPort = 0;
    u16 wDstPort = CASTPORT_TEST + 10;
    
    // cast ip
    g_AgentCallBack(dwNode1, 0, &dwSrcIp, sizeof(dwSrcIp));
    CPPUNIT_ASSERT(dwSrcIp == 0);

    g_AgentCallBack(dwNode1, 1, dwDstIp, sizeof(dwDstIp));
    OspDelay(1000);
    g_AgentCallBack(dwNode1, 0, &dwSrcIp, sizeof(dwSrcIp));
    CPPUNIT_ASSERT(dwSrcIp == dwDstIp);

    // cast port
    g_AgentCallBack(dwNode2, 0, &wSrcPort, sizeof(wSrcPort));
    CPPUNIT_ASSERT(wSrcPort == CASTPORT_TEST);

    g_AgentCallBack(dwNode2, 1, wDstPort, sizeof(wDstPort));
    OspDelay(1000);
    g_AgentCallBack(dwNode2, 0, &wSrcPort, sizeof(wSrcPort));
    CPPUNIT_ASSERT(wDstPort == wSrcPort);
    return;

}

/*=============================================================================
  �� �� ���� ActRecPort
  ��    �ܣ� �������ܶ˿�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActRecPort()
{
    u32 dwNode =  NODE_MCUNETSTARTRECVPORT;
    u16 wSrcPort = 0;
    u16 wDstPort = RECVPORT_TEST + 10;

    g_AgentCallBack(dwNode, 0, &wSrcPort, sizeof(wSrcPort));
    CPPUNIT_ASSERT(wSrcPort == CASTPORT_TEST);

    g_AgentCallBack(dwNode, 1, wDstPort, sizeof(wDstPort));
    OspDelay(1000);
    g_AgentCallBack(dwNode, 0, &wSrcPort, sizeof(wSrcPort));
    CPPUNIT_ASSERT(wDstPort == wSrcPort);
    return;

}

/*=============================================================================
  �� �� ���� ActH225
  ��    �ܣ� ����H224��245�˿�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActH225()
{
    u32 dwNode =  NODE_MCUNET225245STARTPORT;
    u16 wSrcPort = 0;
    u16 wDstPort = H245PORT_TEST + 10;

    g_AgentCallBack(dwNode, 0, &wSrcPort, sizeof(wSrcPort));
    CPPUNIT_ASSERT(wSrcPort == CASTPORT_TEST);

    g_AgentCallBack(dwNode, 1, wDstPort, sizeof(wDstPort));
    OspDelay(1000);
    g_AgentCallBack(dwNode, 0, &wSrcPort, sizeof(wSrcPort));
    CPPUNIT_ASSERT(wDstPort == wSrcPort);
    return;

}
/*=============================================================================
  �� �� ���� ActMtNum
  ��    �ܣ� �����ն���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActMtNum()
{
    u32 dwNode =  NODE_MCUNET225245MAXMTNUM;
    u16 wSrcValue = 0;
    u16 wDstValue = MCU_MTNUM;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 192);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof( wDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}

/*=============================================================================
  �� �� ���� ActIsUseMpcTranData
  ��    �ܣ� ����IsUseMpcTranData�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActIsUseMpcTranData()
{
    u16 wSrcValue = 0;
    u16 wDstValue = 1;
    u32 dwNode = NODE_MCUNETUSEMPCTRANSDATA;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 0);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof( wDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}
/*=============================================================================
  �� �� ���� ActIsUseMpcStack
  ��    �ܣ� ����IsUseMpcStack�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActIsUseMpcStack()
{
    u16 wSrcValue = 0;
    u16 wDstValue = 0;
    u32 wNode = NODE_MCUNETUSEMPCSTACK;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 1);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof( wDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}

// trap 


//  �� �� ���� ActTrapNum
//  ��    �ܣ� ����
void CNmsActTest::ActTrapNum()
{
    
}

/*=============================================================================
  �� �� ���� ActTrapIp
  ��    �ܣ� ����TrapIp�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActTrapIp()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 0x01010101;   // 1.1.1.1
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUNETSNMPCFGIPADDR);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == MCU_TRAPIPADDR);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;
    
}

/*=============================================================================
  �� �� ���� ActTrapCommunity
  ��    �ܣ� ������ͬ���ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActTrapCommunity()
{
    u8 achSrcValue[32];
    memset(achSrcValue, '\0', sizeof(achSrcValue));
    u8 achDstValue[32];
    memset(achDstValue, '\0', sizeof(achDstValue));
    u8  byRow = 1;

    // ����ͬ��
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUNETSNMPCFGREADCOMMUNITY);
    g_AgentCallBack(wNode, 0, achSrcValue, sizeof(achSrcValue));
    CPPUNIT_ASSERT(0 == strcmp(achSrcValue, TRAPRCOMMUNITY));

    memcpy(achDstValue, "public1", sizeof("public1"));
    g_AgentCallBack(wNode, 1, achDstValue, sizeof(achDstValue));
    OspDelay(1000);
    g_AgentCallBack(wNode, 0, achSrcValue, sizeof(achSrcValue));
    CPPUNIT_ASSERT( 0 == strcmp(achSrcValue, achDstValue));
    
    // д��ͬ��
    wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUNETSNMPCFGWRITECOMMUNITY);
    g_AgentCallBack(wNode, 0, achSrcValue, sizeof(achSrcValue));
    CPPUNIT_ASSERT(0 == strcmp(achSrcValue, TRAPWCOMMUNITY));

    memcpy(achDstValue, "private1", sizeof("private1"));
    g_AgentCallBack(wNode, 1, achDstValue, sizeof(achDstValue));
    OspDelay(1000);
    g_AgentCallBack(wNode, 0, achSrcValue, sizeof(achSrcValue));
    CPPUNIT_ASSERT( 0 == strcmp(achSrcValue, achDstValue));

}
/*=============================================================================
  �� �� ���� ActTrapPort
  ��    �ܣ� �����˿��ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActTrapPort()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 160;   // 1.1.1.1
    u8  byRow = 1;

    // snmp get/set port
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUNETSNMPCFGGETSETPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 161);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );

    // snmp send trap port
    wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUNETSNMPCFGTRAPPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 162);
    
    dwDstValue = 163;
    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;

}

// board 
/*=============================================================================
  �� �� ���� ActBrdId
  ��    �ܣ� ����BrdId�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActBrdId()
{
    u32 dwSrcValue = 0;
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPBRDCFGID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 1);

    return;  
}

void CNmsActTest::ActBrdNum()
{
    
}

/*=============================================================================
  �� �� ���� ActBrdIp
  ��    �ܣ� ����BrdIp�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActBrdIp()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 0x02020202;   // 1.1.1.1
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPBRDCFGIPADDR);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 0x01010101);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;
    
}
/*=============================================================================
  �� �� ���� ActBrdType
  ��    �ܣ� ����BrdType�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActBrdType()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 3;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPBRDCFGTYPE);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 0);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return; 
}
/*=============================================================================
  �� �� ���� ActBrdLayer
  ��    �ܣ� ����BrdLayer�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActBrdLayer()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 3;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPBRDCFGLAYER);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 1);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return; 

}

/*=============================================================================
  �� �� ���� ActBrdSlot
  ��    �ܣ� ����BrdSlot�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActBrdSlot()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 10;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPBRDCFGSLOT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 8);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return; 

}
/*=============================================================================
  �� �� ���� ActBrdChoosePort
  ��    �ܣ� ����BrdchoosePort�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActBrdChoosePort()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 0;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPBRDCFGSLOT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 1);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return; 

}
  
// mixer

/*=============================================================================
  �� �� ���� ActMixId
  ��    �ܣ� ����Mixid�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActMixId()
{
    u32 dwSrcValue = 0;
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPMIXERID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 1);

    return;  
}

void CNmsActTest::ActMixNum()
{
    
}

/*=============================================================================
  �� �� ���� ActMixToMcuPort
  ��    �ܣ� ����MixToMcuPort�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActMixToMcuPort()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 10020;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPMIXERPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 10000);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return; 

}
/*=============================================================================
  �� �� ���� ActMixSwitchNo
  ��    �ܣ� ����MixSwitchNo�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActMixSwitchNo()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 3;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPMIXERSWITCHBRDID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 2);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  

}

/*=============================================================================
  �� �� ���� ActMixAlias
  ��    �ܣ� ����MixAlias�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActMixAlias()
{
    u8 achSrcAlias[32];
    u8 achDstAlias[32];
    u8 byRow = 1;
    memset(achSrcAlias, '\0', sizeof(achSrcAlias));
    memset(achDstAlias, '\0', sizeof(achDstAlias));
    
    u32 dwNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPMIXERALIAS);

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    OspDelay(500);
    CPPUNIT_ASSERT(0 == strcmp(achSrcAlias, "Mixer1"));
    
    memcpy(achDstAlias, "Mix15", sizeof("Mix15"));
    g_AgentCallBack(dwNode, 1, achDstAlias, sizeof(achDstAlias));
    OspDelay(500);

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    CPPUNIT_ASSERT(0 == strcmp(achDstAlias, achSrcAlias));
    return;
    
}
/*=============================================================================
  �� �� ���� ActMixRunIndex
  ��    �ܣ� ����MixRunIdex�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActMixRunIndex()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 4;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPMIXERRUNNINGBRDID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 2);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  

}

/*=============================================================================
  �� �� ���� ActMixStartPort
  ��    �ܣ� ����MixStartPort�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActMixStartPort()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 45010;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPMIXERMIXSTARTPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 45000);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  

}
/*=============================================================================
  �� �� ���� ActMixChannel
  ��    �ܣ� ����MixChannel�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActMixChannel()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 8;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPMIXERMAXMIXGROUPNUM);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 10);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  

}
// ����
void CNmsActTest::ActMixUseMap()
{
    
}

// recorder
/*=============================================================================
  �� �� ���� ActRecId
  ��    �ܣ� ����RecId�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActRecId()
{
    u32 dwSrcValue = 0;
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPRECORDERID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 17);

    return;  

}

void CNmsActTest::ActRecNum()
{
    
}
/*=============================================================================
  �� �� ���� ActRecToMcuPort
  ��    �ܣ� ����RecToMcuPort�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActRecToMcuPort()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 60000;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPRECORDERPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 60000);
    return; 

}
/*=============================================================================
  �� �� ���� ActRecSwitchNo
  ��    �ܣ� ����RecSwitchNo�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActRecSwitchNo()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 3;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPRECORDERSWITCHBRDID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 2);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  

}
/*=============================================================================
  �� �� ���� ActRecAlias
  ��    �ܣ� ����RecAlias�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActRecAlias()
{
    u8 achSrcAlias[32];
    u8 achDstAlias[32];
    u8 byRow = 1;
    memset(achSrcAlias, '\0', sizeof(achSrcAlias));
    memset(achDstAlias, '\0', sizeof(achDstAlias));
    
    u32 dwNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPRECORDERALIAS);

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    OspDelay(500);
    CPPUNIT_ASSERT(0 == strcmp(achSrcAlias, "Recorder1"));
    
    memcpy(achDstAlias, "Rec15", sizeof("Rec15"));
    g_AgentCallBack(dwNode, 1, achDstAlias, sizeof(achDstAlias));
    OspDelay(500);

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    CPPUNIT_ASSERT(0 == strcmp(achDstAlias, achSrcAlias));
    return;

}
/*=============================================================================
  �� �� ���� ActRecStartPort
  ��    �ܣ� ����RecStartPort�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActRecStartPort()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 60000;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPRECORDERPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 60000);
    return; 

}
/*=============================================================================
  �� �� ���� ActRecIp
  ��    �ܣ� ����RecIp�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActRecIp()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 0x02020202;   // 2.2.2.2
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPRECORDERIPADDR);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 0x01010101);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;
    
}

// tvWall
/*=============================================================================
  �� �� ���� ActTvId
  ��    �ܣ� ����TvId�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActTvId()
{
    u32 dwSrcValue = 0;
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPTVWALLID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 33);

    return;  

}

void CNmsActTest::ActTvNum()
{
    
}

/*=============================================================================
  �� �� ���� ActTvAlias
  ��    �ܣ� ����TvAlias�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActTvAlias()
{
    u8 achSrcAlias[32];
    u8 achDstAlias[32];
    u8 byRow = 1;
    memset(achSrcAlias, '\0', sizeof(achSrcAlias));
    memset(achDstAlias, '\0', sizeof(achDstAlias));
    
    u32 dwNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPTVWALLALIAS);

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    OspDelay(500);
    CPPUNIT_ASSERT(0 == strcmp(achSrcAlias, "TVWall1"));
    
    memcpy(achDstAlias, "Tv15", sizeof("Tv15"));
    g_AgentCallBack(dwNode, 1, achDstAlias, sizeof(achDstAlias));
    OspDelay(500);

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    CPPUNIT_ASSERT(0 == strcmp(achDstAlias, achSrcAlias));
    return;

}

/*=============================================================================
  �� �� ���� ActTvRunIndex
  ��    �ܣ� ����TvRunIndex�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActTvRunIndex()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 5;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPTVWALLRUNNINGBRDID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 4);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  
 
}

/*=============================================================================
  �� �� ���� ActTvStartPort
  ��    �ܣ� ����TvStartPort�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActTvStartPort()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 45010;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPTVWALLSTARTPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 45000);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  
  
}

// bas
/*=============================================================================
  �� �� ���� ActBasId
  ��    �ܣ� ����BasId�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActBasId()
{
    u32 dwSrcValue = 0;
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPBASID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 49);

    return;  

}

void CNmsActTest::ActBasNum()
{
    
}

/*=============================================================================
  �� �� ���� ActBasToMcuPort
  ��    �ܣ� ����BasToMcuPort�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActBasToMcuPort()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 13020;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPBASPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 13000);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return; 

}

/*=============================================================================
  �� �� ���� ActBasSwitchBrdIdx
  ��    �ܣ� ����BasSwitchBrdIdx�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActBasSwitchBrdIdx()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 4;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPBASSWITCHBRDID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 2);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return; 
    
}
/*=============================================================================
  �� �� ���� ActBasAlias
  ��    �ܣ� ����BasAlias�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActBasAlias()
{
    u8 achSrcAlias[32];
    u8 achDstAlias[32];
    u8 byRow = 1;
    memset(achSrcAlias, '\0', sizeof(achSrcAlias));
    memset(achDstAlias, '\0', sizeof(achDstAlias));
    
    u32 dwNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPTVWALLALIAS);

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    OspDelay(500);
    CPPUNIT_ASSERT(0 == strcmp(achSrcAlias, "Bas1"));
    
    memcpy(achDstAlias, "Bas15", sizeof("Bas15"));
    g_AgentCallBack(dwNode, 1, achDstAlias, sizeof(achDstAlias));
    OspDelay(500);

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    CPPUNIT_ASSERT(0 == strcmp(achDstAlias, achSrcAlias));
    return;
    
}
/*=============================================================================
  �� �� ���� ActBasRunBrd
  ��    �ܣ� ����BasRunBrd�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActBasRunBrd()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 5;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPBASRUNNINGBRDID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 6);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  
 
}
/*=============================================================================
  �� �� ���� ActBasStartPort
  ��    �ܣ� ����BasStartPort�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActBasStartPort()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 45010;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPBASSTARTRECVPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 45000);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  
  
}

// vmp
/*=============================================================================
  �� �� ���� ActVmpId
  ��    �ܣ� ����VmpId�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActVmpId()
{
    u32 dwSrcValue = 0;
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPVMPID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 65);

    return;  
 
}
void CNmsActTest::ActVmpNum()
{
    
}
/*=============================================================================
  �� �� ���� ActVmpToMcuPort
  ��    �ܣ� ����VmpToMcuPort�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActVmpToMcuPort()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 17010;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPVMPPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 17000);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return; 

}

/*=============================================================================
  �� �� ���� ActVmpSwitchBrdIdx
  ��    �ܣ� ����VmpSwitchBrdIdx�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActVmpSwitchBrdIdx()
{
        u32 dwSrcValue = 0;
    u32 dwDstValue = 3;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPVMPSWITCHBRDID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 2);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return; 
    
}

/*=============================================================================
  �� �� ���� ActVmpAlias
  ��    �ܣ� ����VmpAlias�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActVmpAlias()
{
    u8 achSrcAlias[32];
    u8 achDstAlias[32];
    u8 byRow = 1;
    memset(achSrcAlias, '\0', sizeof(achSrcAlias));
    memset(achDstAlias, '\0', sizeof(achDstAlias));
    
    u32 dwNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPVMPALIAS);

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    OspDelay(500);
    CPPUNIT_ASSERT(0 == strcmp(achSrcAlias, "VMP1"));
    
    memcpy(achDstAlias, "Vmp15", sizeof("Vmp15"));
    g_AgentCallBack(dwNode, 1, achDstAlias, sizeof(achDstAlias));
    OspDelay(500);

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    CPPUNIT_ASSERT(0 == strcmp(achDstAlias, achSrcAlias));
    return;

}

/*=============================================================================
  �� �� ���� ActVmpRunBrd
  ��    �ܣ� ����VmpRunBrd�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActVmpRunBrd()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 5;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPVMPRUNNINGBRDID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 2);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  
  
}
/*=============================================================================
  �� �� ���� ActVmpStartPort
  ��    �ܣ� ����VmpStartPort�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActVmpStartPort()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 43010;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPVMPSTARTRECVPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 43000);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  
   
}
/*=============================================================================
  �� �� ���� ActVmpCodeNum
  ��    �ܣ� ����VmpCodeNum�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActVmpCodeNum()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 2;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPVMPSTARTPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 1);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  
  
}
void CNmsActTest::ActVmpRunIp()
{
    
}

// data conference Server
/*=============================================================================
  �� �� ���� ActDscId
  ��    �ܣ� ����DscId�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActDscId()
{
    u16  wSrcId = 0;
    u16  wDstId = 1;
    u32  dwNode = NODE_MCUEQPDCSID;

    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcId == 0);

    g_AgentCallBack(dwNode, 1, wDstId, sizeof(wDstId));

    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    CPPUNIT_ASSERT(wSrcId == wDstId);
    return;

}

// Prs
/*=============================================================================
  �� �� ���� ActPrsId
  ��    �ܣ� ����PrsId�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActPrsId()
{
    u16  wSrcId = 0;
    u16  wDstId = 91;
    u32  dwNode = NODE_MCUEQPPRSID;

    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcId == 0);

    g_AgentCallBack(dwNode, 1, wDstId, sizeof(wDstId));

    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    CPPUNIT_ASSERT(wSrcId == wDstId);
    return;

}
/*=============================================================================
  �� �� ���� ActPrsToMcuPort
  ��    �ܣ� ����PrsToMcuPort�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActPrsToMcuPort()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 18010;
    u32  dwNode = NODE_MCUEQPPRSMCUSTARTPORT;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcId == 18000);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}
/*=============================================================================
  �� �� ���� ActPrsSwitchBrdIdx
  ��    �ܣ� ����PrsSwitchBrdIdx�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActPrsSwitchBrdIdx()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 3;
    u32  dwNode = NODE_MCUEQPPRSSWITCHBRDID;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcId == 2);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}
/*=============================================================================
  �� �� ���� ActPrsAlias
  ��    �ܣ� ����PrsAlias�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActPrsAlias()
{
    u8 achSrcAlias[32];
    u8 achDstAlias[32];
    memset(achSrcAlias, '\0', sizeof(achSrcAlias));
    memset(achDstAlias, '\0', sizeof(achDstAlias));
    
    u32 dwNode = NODE_MCUEQPPRSALIAS;
    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    OspDelay(500);
    CPPUNIT_ASSERT(0 == strcmp(achSrcAlias, "Prs1"));
    
    memcpy(achDstAlias, "Prs15", sizeof("Prs15"));
    g_AgentCallBack(dwNode, 1, achDstAlias, sizeof(achDstAlias));

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    CPPUNIT_ASSERT(0 == strcmp(achDstAlias, achSrcAlias));
    return;
    
}
/*=============================================================================
  �� �� ���� ActPrsRunBrdIdx
  ��    �ܣ� ����PrsRunBrdIdx�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActPrsRunBrdIdx()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 3;
    u32  dwNode = NODE_MCUEQPPRSRUNNINGBRDID;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 2);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}
/*=============================================================================
  �� �� ���� ActPrsStartPort
  ��    �ܣ� ����PrsStartPort�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActPrsStartPort()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 44010;
    u32  dwNode = NODE_MCUEQPPRSSTARTPORT;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 44000);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}
/*=============================================================================
  �� �� ���� ActPrsFirstTime
  ��    �ܣ� ����PrsFirstTime�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActPrsFirstTime()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 60;
    u32  dwNode = NODE_MCUEQPPRSFIRSTTIMESPAN;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 40);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}
/*=============================================================================
  �� �� ���� ActPrsSecTime
  ��    �ܣ� ����PrsSecTime�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActPrsSecTime()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 100;
    u32  dwNode = NODE_MCUEQPPRSSECONDTIMESPAN;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 80);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;
}
/*=============================================================================
  �� �� ���� ActPrsThdTime
  ��    �ܣ� ����PrsThdTime�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActPrsThdTime()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 140;
    u32  dwNode = NODE_MCUEQPPRSTHIRDTIMESPAN;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 120);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}
/*=============================================================================
  �� �� ���� ActPrsRejectTime
  ��    �ܣ� ����PrsRejectTime�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActPrsRejectTime()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 180;
    u32  dwNode = NODE_MCUEQPPRSREJECTTIMESPAN;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 160);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}

// net sync
/*=============================================================================
  �� �� ���� ActNetSyncMode
  ��    �ܣ� ����NetSyncMode�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActNetSyncMode()
{
    u16  wSrcId = 0;
    u16  wDstId = 2;
    u32  dwNode = NODE_MCUEQPNETSYNCMODE;

    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcId == 1);

    g_AgentCallBack(dwNode, 1, wDstId, sizeof(wDstId));

    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    CPPUNIT_ASSERT(wSrcId == wDstId);
    return;
  
}

/*=============================================================================
  �� �� ���� ActNetSyncDTSlot
  ��    �ܣ� ����NetSyncDTSlot�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActNetSyncDTSlot()
{
    u16  wSrcId = 0;
    u16  wDstId = 14;
    u32  dwNode = NODE_MCUEQPNETSYNCDTSLOT;

    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcId == 10);

    g_AgentCallBack(dwNode, 1, wDstId, sizeof(wDstId));

    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    CPPUNIT_ASSERT(wSrcId == wDstId);
    return;

}

/*=============================================================================
  �� �� ���� ActNetSyncE1Idx
  ��    �ܣ� ����NetSyncE1Idx�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActNetSyncE1Idx()
{
    u16  wSrcId = 0;
    u16  wDstId = 7;
    u32  dwNode = NODE_MCUEQPNETSYNCE1INDEX;

    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcId == 0);

    g_AgentCallBack(dwNode, 1, wDstId, sizeof(wDstId));

    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    CPPUNIT_ASSERT(wSrcId == wDstId);
    return;
 
}

// Qos
/*=============================================================================
  �� �� ���� ActQosType
  ��    �ܣ� ����QosType�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActQosType()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 1;
    u32  dwNode = NODE_MCUNETQOSTYPE;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 2);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;
}
/*=============================================================================
  �� �� ���� ActAudioLevel
  ��    �ܣ� ����AudioLevel�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActAudioLevel()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 3;
    u32  dwNode = NODE_MCUNETQOSAUDIOLEVEL;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 4);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}

/*=============================================================================
  �� �� ���� ActVideoLevel
  ��    �ܣ� ����VideoLevel�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActVideoLevel()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 4;
    u32  dwNode = NODE_MCUNETQOSVIDEOLEVEL;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 3);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;
}

/*=============================================================================
  �� �� ���� ActDataLevel
  ��    �ܣ� ����DataLevel�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActDataLevel()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 1;
    u32  dwNode = NODE_MCUNETQOSDATALEVEL;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 0);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}

/*=============================================================================
  �� �� ���� ActSignalLevel
  ��    �ܣ� ����SignalLevel�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActSignalLevel()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 6;
    u32  dwNode = NODE_MCUNETQOSSIGNALLEVEL;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 7);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return; 

}

/*=============================================================================
  �� �� ���� ActIpServiceType
  ��    �ܣ� ����IpServiceType�ֶ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
void CNmsActTest::ActIpServiceType()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 0;
    u32  dwNode = NODE_MCUNETQOSIPSERVICETYPE;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 1);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;
}