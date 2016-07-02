#ifndef _EVMCUTEST_H_
#define _EVMCUTEST_H_

#include "osp.h"
#include "eventid.h"

/*
 *	���ɲ�����Ϣ
 */
//��ȡ�ض��������Ϣ,��Ϣ��CConfId+TConfFullInfo
OSPEVENT( TEST_MCU_GETCONFINFO_CMD,			    EV_TEST_MCU_BGN + 2 );
//��ȡMCU�����л����б�,��Ϣ��void*( ������ָ��)+u32����
OSPEVENT( TEST_MCU_GETCONFIDLIST_CMD,		    EV_TEST_MCU_BGN + 3 );
// �õ�Mcu�ϵ�ǰ������Ŀ,��Ϣ�� u32( ��ǰ������Ŀ)
OSPEVENT( TEST_MCU_GETCONFCOUNT_CMD,		    EV_TEST_MCU_BGN + 4 );
// MCU�豸����
OSPEVENT( TEST_MCU_REBOOT_CMD,				    EV_TEST_MCU_BGN + 5 );
// �õ�Mp������Ϣ�б�
OSPEVENT( TEST_MCU_GETMPSWITCHINFO_CMD,		    EV_TEST_MCU_BGN + 6 );

//��ȡ�ض��������������״̬,��Ϣ��CConfId+TEqp( VMP)+TEqp( MIX)+u8( ������)
//    +TEqp( BAS)+u8( BASͨ����)+TEqp( REC)+u8( ¼��ͨ����)
//    +TEqp( PLAY)+u8( ����ͨ����)
OSPEVENT( TEST_MCU_GETCONFEQP_CMD,			    EV_TEST_MCU_BGN + 11 );
//��ȡ��������,��Ϣ��u8( ����ID)+TPeriEqpData
OSPEVENT( TEST_MCU_GETEQPSTATUS_CMD,		    EV_TEST_MCU_BGN + 12 );
//��ȡ���б�,��Ϣ��TEqp *( �����б�ָ��)+u32( ���峤��)
OSPEVENT( TEST_MCU_GETMCUEQPLIST_CMD,		    EV_TEST_MCU_BGN + 13 );
//�Ͽ���������,��Ϣ��u8( ����ID��)
OSPEVENT( TEST_MCU_DISCONNECTEQP_CMD,		    EV_TEST_MCU_BGN + 14 );
//�õ�����Ĺ�������,��Ϣ��CConfId+u8 byEqpId+TVMPParam& tVMPParam
OSPEVENT( TEST_MCU_GETEQPVMPDATA_CMD,		    EV_TEST_MCU_BGN + 15 );
//�õ�����Ĺ�������,��Ϣ��CConfId cConfId+u8 byEqpId+TDiscussParam& tDiscussParam
OSPEVENT( TEST_MCU_GETEQPMIXERDATA_CMD,		    EV_TEST_MCU_BGN + 16 );
//��ȡ�ض��������ض��ն�����,��Ϣ��CConfId cConfId+u8 byMtId+TMtData &tMtData
OSPEVENT( TEST_MCU_GETCONFSPECMTDATA_CMD,	    EV_TEST_MCU_BGN + 21 );
//��ȡ�ض��������ն��б�,��Ϣ��CConfId cConfId+TMtExt *ptMtList+u32 byBufSize
OSPEVENT( TEST_MCU_GETCONFMTLIST_CMD,		    EV_TEST_MCU_BGN + 22 );
//��ȡ�ض������н�����Ϣ�б�,��Ϣ��CConfId cConfId+TConfSwitchTable &tConfSwitchTable
OSPEVENT( TEST_MCU_GETCONFMPSWITCHINFO_CMD,	    EV_TEST_MCU_BGN + 30 );
//��ȡ�ض������н�����Ϣ�б�,��Ϣ��CConfId cConfId+TConfSwitchTable &tConfSwitchTable
OSPEVENT( TEST_MCU_GETCONFSWITCHINFO_CMD,	    EV_TEST_MCU_BGN + 31 );

// �õ�˫�ٹ��ܻ�����Ϣ,��Ϣ��CConfId, ack tTSpeedParam
OSPEVENT( TEST_MCU_GETCONFTSPEEDDATA_CMD,       EV_TEST_MCU_BGN + 32 );
// �õ����������Ϣ,��Ϣ��CConfId, ack tEncryptParam
OSPEVENT( TEST_MCU_GETCONFENCRYPTDATA_CMD,      EV_TEST_MCU_BGN + 33 );
// ��ѯ������ѯ״̬,��Ϣ��CConfId, ack tPollParam
OSPEVENT( TEST_MCU_GETCONFPOLLDATA_CMD,		    EV_TEST_MCU_BGN + 34 );
// �õ��������۲���,��Ϣ��CConfId, ack tDiscussParam
OSPEVENT( TEST_MCU_GETCONFMIXERDATA_CMD,        EV_TEST_MCU_BGN + 35 );
// �õ��������Ƶ���ϲ���,��Ϣ��CConfId, ack tVMPParam
OSPEVENT( TEST_MCU_GETCONFVMPDATA_CMD,		    EV_TEST_MCU_BGN + 36 );

//��ȡ��MP���б�,��Ϣ��TMp *ptMpList+u32 byBufSize
OSPEVENT( TEST_MCU_GETMPLIST_CMD,			    EV_TEST_MCU_BGN + 41 );
//��ȡ��MCU��ʱ��,��Ϣ��TKdvTime *ptKdvTime
OSPEVENT( TEST_MCU_GETMCUCURTIME_CMD,		    EV_TEST_MCU_BGN + 51 );
//����������,��Ϣ��Ϊ��
OSPEVENT( TEST_MCU_RELEASEALLCONF_CMD,		    EV_TEST_MCU_BGN + 61 );
//�õ�����MCU��Ϣ
OSPEVENT( TEST_MCU_GETCASCADEMCU_CMD,		    EV_TEST_MCU_BGN + 71 );
//�õ������ն���Ϣ
OSPEVENT( TEST_MCU_GETALLMTINFO_CMD,		    EV_TEST_MCU_BGN + 81 );
//�õ�MCU��Ϣ
OSPEVENT( TEST_MCU_GETMCUINFO_CMD,			    EV_TEST_MCU_BGN + 82 );
//�õ�����MT( ����MCU)��Ϣ
OSPEVENT( TEST_MCU_GETALLMCUMTINFO_CMD,		    EV_TEST_MCU_BGN + 83 );

OSPEVENT( TEST_MCU_TESTUSERINFO_CMD,		    EV_TEST_MCU_BGN + 84 );

// ��ȡ��ؼ����Ϣ
OSPEVENT(  TEST_MCU_GETMCCHANNEL_CMD,		    EV_TEST_MCU_BGN + 85 );




/*======================================================================
						��Ԫ������Ϣ����( 53001--53501)
 ======================================================================*/

#define EV_UNITTEST_MCU_BGN					    EV_TEST_MCU_BGN + 500

/*---------------------------------------------------------------------------
 MCU������Ϣ��52501-53500��
----------------------------------------------------------------------------*/

OSPEVENT( EV_TEST_MT_CONNECT_REQ,				EV_UNITTEST_MCU_BGN + 1 );
OSPEVENT( EV_MT_TEST_CONNECT_ACK,				EV_UNITTEST_MCU_BGN + 2 );
OSPEVENT( EV_MT_TEST_CONNECT_NACK,				EV_UNITTEST_MCU_BGN + 3 );

//-----------      MCU�������ݻ�����Խӿ�(52511-52600)    ---------------

//�õ�ĳ�����Ƿ�����ݹ����ٿ�, ��Ϣ�� byConfIdx
OSPEVENT( EV_TEST_CONF_ONGOING_INDCS_REQ,		EV_UNITTEST_MCU_BGN + 11 );
//ĳ��������ݹ����ٿ�Ӧ��
OSPEVENT( EV_TEST_CONF_ONGOING_INDCS_ACK,		EV_UNITTEST_MCU_BGN + 12 );
//ĳ����û�д����ݹ����ٿ�Ӧ��
OSPEVENT( EV_TEST_CONF_ONGOING_INDCS_NACK,		EV_UNITTEST_MCU_BGN + 13 );

//�õ�ĳ���ݻ����Ƿ����, ��Ϣ�� byConfIdx
OSPEVENT( EV_TEST_CONF_END_INDCS_REQ,			EV_UNITTEST_MCU_BGN + 14 );
//ĳ���ݻ������Ӧ��
OSPEVENT( EV_TEST_CONF_END_INDCS_ACK,			EV_UNITTEST_MCU_BGN + 15 );
//ĳ���ݻ���δ����Ӧ��
OSPEVENT( EV_TEST_CONF_END_INDCS_NACK,			EV_UNITTEST_MCU_BGN + 16 );

//�õ�ĳ�ն��Ƿ������ݻ�����, ��Ϣ��TMt, ��ȷ���ն�
OSPEVENT( EV_TEST_MT_INDATACONF_REQ,			EV_UNITTEST_MCU_BGN + 17 );
//ĳ�ն������ݻ�����Ӧ��
OSPEVENT( EV_TEST_MT_INDATACONF_ACK,			EV_UNITTEST_MCU_BGN + 18 );
//ĳ�ն˲������ݻ�����Ӧ��
OSPEVENT( EV_TEST_MT_INDATACONF_NACK,			EV_UNITTEST_MCU_BGN + 19 );

//�õ�ĳ�ն����Ƿ������ݻ���, ��Ϣ��TMt, ��ȷ���ն�
OSPEVENT( EV_TEST_MT_OFFDATACONF_REQ,			EV_UNITTEST_MCU_BGN + 20 );
//ĳ�ն������ݻ���Ӧ��
OSPEVENT( EV_TEST_MT_OFFDATACONF_ACK,			EV_UNITTEST_MCU_BGN + 21 );
//ĳ�ն�δ�����ݻ���Ӧ��
OSPEVENT( EV_TEST_MT_OFFDATACONF_NACK,			EV_UNITTEST_MCU_BGN + 22 );

//----------      MCU�������ݻ�����Խӿ����(52511-52600)    --------------

//-------------      MCU�����ȱ��ݲ��Խӿ�(52601-52700)    -----------------

//�õ������ģ����Ϣ, ��Ϣ�� CConfId
OSPEVENT( EV_TEST_TEMPLATEINFO_GET_REQ,			EV_UNITTEST_MCU_BGN + 101 );
//�õ������ģ����Ϣ�ɹ�Ӧ��, ��Ϣ�� byConfIdx + TConfInfo
OSPEVENT( EV_TEST_TEMPLATEINFO_GET_ACK,			EV_UNITTEST_MCU_BGN + 102 );
//�õ������ģ����Ϣʧ��Ӧ��, ��Ϣ�� byConfIdx
OSPEVENT( EV_TEST_TEMPLATEINFO_GET_NACK,		EV_UNITTEST_MCU_BGN + 103 );

//�õ�����Ļ�����Ϣ, ��Ϣ�� CConfId
OSPEVENT( EV_TEST_CONFINFO_GET_REQ,				EV_UNITTEST_MCU_BGN + 104 );
//�õ�����Ļ�����Ϣ�ɹ�Ӧ��, ��Ϣ�� byConfIdx + TConfInfo
OSPEVENT( EV_TEST_CONFINFO_GET_ACK,				EV_UNITTEST_MCU_BGN + 105 );
//�õ�����Ļ�����Ϣʧ��Ӧ��, ��Ϣ�� byConfIdx
OSPEVENT( EV_TEST_CONFINFO_GET_NACK,			EV_UNITTEST_MCU_BGN + 106 );

//�õ�������ն��б���Ϣ, ��Ϣ�� CConfId
OSPEVENT( EV_TEST_CONFMTLIST_GET_REQ,			EV_UNITTEST_MCU_BGN + 107 );
//�õ�������ն��б���Ϣ�ɹ�Ӧ��, ��Ϣ�� byConfIdx + TMt[]
OSPEVENT( EV_TEST_CONFMTLIST_GET_ACK,			EV_UNITTEST_MCU_BGN + 108 );
//�õ�������ն��б���Ϣʧ��Ӧ��, ��Ϣ�� byConfIdx
OSPEVENT( EV_TEST_CONFMTLIST_GET_NACK,			EV_UNITTEST_MCU_BGN + 109 );

//�õ�����ĵ�ַ����Ϣ, ��Ϣ�� CConfId
OSPEVENT( EV_TEST_ADDRBOOK_GET_REQ,				EV_UNITTEST_MCU_BGN + 110 );
//�õ�����ĵ�ַ����Ϣ�ɹ�Ӧ��, ��Ϣ�� byConfIdx +
OSPEVENT( EV_TEST_ADDRBOOK_GET_ACK,				EV_UNITTEST_MCU_BGN + 111 );
//�õ�����ĵ�ַ����Ϣʧ��Ӧ��, ��Ϣ�� byConfIdx
OSPEVENT( EV_TEST_ADDRBOOK_GET_NACK,			EV_UNITTEST_MCU_BGN + 112 );

//�õ�MCU��������Ϣ, ��Ϣ�� CConfId
OSPEVENT( EV_TEST_MCUCONFIG_GET_REQ,			EV_UNITTEST_MCU_BGN + 113 );
//�õ�MCU��������Ϣ�ɹ�Ӧ��, ��Ϣ�� byConfIdx +
OSPEVENT( EV_TEST_MCUCONFIG_GET_ACK,			EV_UNITTEST_MCU_BGN + 114 );
//�õ�MCU��������Ϣʧ��Ӧ��, ��Ϣ�� byConfIdx
OSPEVENT( EV_TEST_MCUCONFIG_GET_NACK,			EV_UNITTEST_MCU_BGN + 115 );

//------------      MCU�����ȱ��ݲ��Խӿ����(52601-52700)    --------------

#endif  //!_EVMCUTEST_H_
