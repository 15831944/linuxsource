              
/*****************************************************************************
   ģ����      : �·���ҵ��
   �ļ���      : evmcumt.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: �·���MCU���ն�֮��ӿ���Ϣ����
                 *_REQ: ��Ӧ������
				 *_ACK, *_NACK: ����֮��
				 *_CMD: ����Ӧ������
				 *_NOTIF: ״̬֪ͨ

-----------------------------------------------------------------------------
   �޸ļ�¼:

ע��Ϊ��֤������ȷ���У����гɹ���ʧ��Ӧ���ԭ��Ϣ�ŷֱ��1��2

******************************************************************************/
#ifndef _EV_MCUMT_H_
#define _EV_MCUMT_H_

#include "osp.h"
#include "eventid.h"

const u16 EV_MCUMT_STD_INTERVAL  = 400;
const u16 EV_MCUMCU_STD_INTERVAL = 100;
const u16 EV_MCUMT_KDV_INTERVAL  = 400;
const u16 EV_MTADP_INTERVAL      = 50;
const u16 EV_MCUMCU_KDV_INTERVAL = 20;  //  pengjie[9/28/2009] �Զ��弶����ش���Ϣ���
const u16 EV_MCUMT_SAT_INTERVAL  = 30;

const u16 EV_MCUMT_STD_BGN = EV_MCUMT_BGN + 1;
const u16 EV_MCUMT_STD_END = EV_MCUMT_BGN + EV_MCUMT_STD_INTERVAL;

const u16 EV_MCUMCU_STD_BGN = EV_MCUMT_STD_END  + 1;
const u16 EV_MCUMCU_STD_END = EV_MCUMCU_STD_BGN + EV_MCUMCU_STD_INTERVAL;

const u16 EV_MCUMT_KDV_BGN = EV_MCUMCU_STD_END + 1;
const u16 EV_MCUMT_KDV_END = EV_MCUMCU_STD_END + EV_MCUMT_KDV_INTERVAL;

const u16 EV_MTADP_BGN = EV_MCUMT_KDV_END + 1; // �Զ��� MTADP�ڲ�ά��, ���䳤��150   
const u16 EV_MTADP_END = EV_MCUMT_KDV_END + EV_MTADP_INTERVAL; // �Զ��� MTADP�ڲ�ά��, ���䳤��150 

//  pengjie[9/28/2009] �Զ��弶����ش���Ϣ ����20
const u16 EV_MCUMCU_KDV_BGN = EV_MTADP_END + 1;
const u16 EV_MCUMCU_KDV_END = EV_MTADP_END + EV_MCUMCU_KDV_INTERVAL;

const u16 EV_MCUMT_SAT_BGN = EV_MCUMCU_KDV_END + 1;
const u16 EV_MCUMT_SAT_END = EV_MCUMT_SAT_BGN + EV_MCUMT_SAT_INTERVAL;  // MCU��mtadp����Э��Ự���������䳤��30

/************************************************************************/
/* ��׼ ��������������                                                  */
/************************************************************************/

//�ն˷���MCU�Ĵ�����������MT->MCU��
//TMtAlias(�ն�H323Id)+TMTAlias(�ն�e164Id)+TMtAlias(�ն�ip)+TMtAlias(��MCU�Ļ�����)+
//+byType(type_MT, type_MCU)+byEncrypt(1-����,0-������)+TConfInfo
OSPEVENT( MT_MCU_CREATECONF_REQ,		EV_MCUMT_STD_BGN + 1 );
//MCU����MT�Ĵ�������Ӧ��MCU->MT ��Ϣ��:byEncrypt(1-����,0-������)+TCapSupport(��������)+TMtAlias(��MCU�Ļ�����)
OSPEVENT( MCU_MT_CREATECONF_ACK,		EV_MCUMT_STD_BGN + 2 );
//MCU�ܾ�MT�Ĵ���������������Ϣ��
OSPEVENT( MCU_MT_CREATECONF_NACK,		EV_MCUMT_STD_BGN + 3 );

// guzh [6/19/2007] 
//�¼�MCU��������MCU�ĺ�������֪ͨ��MT->MCU��
//TMtAlias(�ն�H323Id)+TMTAlias(�ն�e164Id)+TMtAlias(�ն�ip)+TMtAlias(��MCU�Ļ�����)+
//+byType(type_MT, type_MCU)+byEncrypt(1-����,0-������)+TConfInfo
OSPEVENT( MT_MCU_LOWERCALLEDIN_NTF,		EV_MCUMT_STD_BGN + 5);

//��ϯ�ն˷���MCU����ֹ��������MT->MCU����Ϣ��NULL
OSPEVENT( MT_MCU_DROPCONF_CMD,			EV_MCUMT_STD_BGN + 8 );
/************************************************************************/
/* ��׼ ��ᡢ���                                                      */
/************************************************************************/

//�ն˷���MCU�ļ�����������MT->MCU����Ϣ��Ϊ�ṹTMtAlias(�ն˱���) + TMtAlias�����������
OSPEVENT( MT_MCU_MTJOINCONF_REQ,		EV_MCUMT_STD_BGN + 10 );
//�ն˷���MCU�ļ�����������ɹ�Ӧ��MT->MCU������Ϣ��
OSPEVENT( MCU_MT_MTJOINCONF_ACK,		EV_MCUMT_STD_BGN + 11 );
//�ն˷���MCU�ļ�����������ʧ��Ӧ��MT->MCU������Ϣ��
OSPEVENT( MCU_MT_MTJOINCONF_NACK,	    EV_MCUMT_STD_BGN + 12 );



//MCU�����ն˼���ָ����������MCU->MT (MCU->SMCU)��
//��Ϣ��Ϊ�ṹTMt+TMtAlias(�ն˱���)+TMtAlias(������)+byEncrypt(1-����,0-������)
//u16(������,���д���kbps)+TCapSupport(��������)+u16(���ֽ�:���ȼ���(u8)+���ֽ�:��������(1-������ 0-������))
OSPEVENT( MCU_MT_INVITEMT_REQ,		    EV_MCUMT_STD_BGN + 18 );
//MT����Ӧ��MT->MCU����Ϣ��Ϊ�ṹTMtAlias(H323Id) + TMtAlias(ip) + 1byte (�ں�Զ��ʵ������,Ϊ: TYPE_MCU��TYPE_MT)+
//byEncrypt(1-����,0-������)
OSPEVENT( MT_MCU_INVITEMT_ACK,		    EV_MCUMT_STD_BGN + 19 );
//MT�ܾ�Ӧ��MT->MCU����Ϣ��Ϊ�ṹTMtAlias + 1byte (VCԭ��)
//                                 (��ԭ��ΪMTLEFT_REASON_BYSYEXT + u8(���ڻ���ĵ��ȼ���) + �����323ID)   ԭ��μ�������
OSPEVENT( MT_MCU_INVITEMT_NACK,		    EV_MCUMT_STD_BGN + 20 );


//MCUǿ���ն��˳��������MCU->MT (MCU->SMCU)������Ϣ��
OSPEVENT( MCU_MT_DELMT_CMD,			    EV_MCUMT_STD_BGN + 21 );

//��ϯ�ն˷���MCU��ǿ���ն��˳�����MT->MCU����Ϣ��Ϊ�ṹTMt
OSPEVENT( MT_MCU_DELMT_REQ,				EV_MCUMT_STD_BGN + 25 );
//MCU�صĳɹ�Ӧ��MCU->MT������Ϣ��
OSPEVENT( MCU_MT_DELMT_ACK,				EV_MCUMT_STD_BGN + 26 );
//MCU�ص�ʧ��Ӧ��MCU->MT������Ϣ��
OSPEVENT( MCU_MT_DELMT_NACK,			EV_MCUMT_STD_BGN + 27 );

//�ն˼������֪ͨ��MCU->MT (MCU->SMCU)����Ϣ��Ϊ�ṹTMt
OSPEVENT( MCU_MT_MTJOINED_NOTIF,		EV_MCUMT_STD_BGN + 30 );
//�ն��뿪����֪ͨ��MCU->MT (MCU->SMCU)����Ϣ��Ϊ�ṹTMt
OSPEVENT( MCU_MT_MTLEFT_NOTIF,			EV_MCUMT_STD_BGN + 32 );
//�������������ն�֪ͨ��MCU->MT����Ϣ��Ϊ�ṹTMt
/************************************************************************/
/* ��׼ ��֤����                                                        */
/************************************************************************/
//�����ն��������룬��Ϣ��ΪNULL
OSPEVENT(MCU_MT_ENTERPASSWORD_REQ,      EV_MCUMT_STD_BGN + 35 ); //enterH243Password
//�ն˻�Ӧ����, u8 ����,��ʾʵ�����������(���Ȳ���)
OSPEVENT(MT_MCU_ENTERPASSWORD_ACK,      EV_MCUMT_STD_BGN + 36 );
OSPEVENT(MT_MCU_ENTERPASSWORD_NACK,     EV_MCUMT_STD_BGN + 37 );

//MCUͬ���ն������û���Ļ������룬���ںϲ������������֤У�飬��Ϣ��Ϊ�����������飬����MAXLEN_PWD
OSPEVENT(MCU_MT_CONFPWD_NTF,		    EV_MCUMT_STD_BGN + 39 );

/************************************************************************/
/* ��׼ ��ϯ����                                                        */
/************************************************************************/

//�ն˷���MCU��������ϯ����MT->MCU����Ϣ��ΪNULL
OSPEVENT( MT_MCU_APPLYCHAIRMAN_REQ,		EV_MCUMT_STD_BGN + 40 );
//�ն˷���MCU��������ϯ����ɹ�Ӧ��MCU->MT������Ϣ��
OSPEVENT( MCU_MT_APPLYCHAIRMAN_ACK,		EV_MCUMT_STD_BGN + 41 ); 
//�ն˷���MCU��������ϯ����ʧ��Ӧ��MCU->MT������Ϣ��
OSPEVENT( MCU_MT_APPLYCHAIRMAN_NACK,	EV_MCUMT_STD_BGN + 42 );

//MCU�����ն˵�ȡ����ϯ����MCU->MT������Ϣ��
OSPEVENT( MCU_MT_CANCELCHAIRMAN_NOTIF,	EV_MCUMT_STD_BGN + 45 );

//��ϯ�ն˷��͸�MCU��ȡ����ϯ������MT->MCU, ��Ϣ��NULL
OSPEVENT( MT_MCU_CANCELCHAIRMAN_REQ,	EV_MCUMT_STD_BGN + 50 );//cancelMakeMeChair
//MCU->MT, ��Ϣ��NULL
OSPEVENT( MCU_MT_CANCELCHAIRMAN_ACK,	EV_MCUMT_STD_BGN + 51 );//withdrawChairToken
OSPEVENT( MCU_MT_CANCELCHAIRMAN_NACK,	EV_MCUMT_STD_BGN + 52 );


//��ѯ������ϯȨ�޵��ն�ID����Ϣ��NULL
OSPEVENT( MT_MCU_GETCHAIRMAN_REQ,       EV_MCUMT_STD_BGN + 55 );
//��ѯ��ϯIDӦ����Ϣ��TMt + TMtAlias
OSPEVENT( MCU_MT_GETCHAIRMAN_ACK,       EV_MCUMT_STD_BGN + 56 );
OSPEVENT( MCU_MT_GETCHAIRMAN_NACK,      EV_MCUMT_STD_BGN + 57 );

/************************************************************************/
/* ��׼ ���Բ���                                                        */
/************************************************************************/

//��ϯ�ն˷���MCU��ָ������������MT->MCU����Ϣ��Ϊ��
OSPEVENT( MT_MCU_SPECSPEAKER_REQ,		EV_MCUMT_STD_BGN + 60 );//makeTerminalBroadcaster
//MCU�صĳɹ�Ӧ��MCU->MT������Ϣ��
OSPEVENT( MCU_MT_SPECSPEAKER_ACK,		EV_MCUMT_STD_BGN + 61 );
//MCU�ص�ʧ��Ӧ��MCU->MT������Ϣ��
OSPEVENT( MCU_MT_SPECSPEAKER_NACK,		EV_MCUMT_STD_BGN + 62 );

//MCU�����ն˵�ָ������֪ͨ��MCU->MT����Ϣ��Ϊ�ṹTMt����������
OSPEVENT( MCU_MT_SPECSPEAKER_NOTIF,	    EV_MCUMT_STD_BGN + 65 ); //seenByAll
//MCU�����ն˵�ȡ����������MCU->MT����Ϣ��Ϊ�ṹTMt
OSPEVENT( MCU_MT_CANCELSPEAKER_NOTIF,	EV_MCUMT_STD_BGN + 67 ); //cancelSeenByAll
//��ϯ�ն�ָ�������ˣ�MT->MCU��TMt
OSPEVENT( MT_MCU_SPECSPEAKER_CMD,		EV_MCUMT_STD_BGN + 69 );//makeTerminalBroadcaster
//��ϯ�ն�ȡ�������ˣ�MT->MCU��NULL
OSPEVENT( MT_MCU_CANCELSPEAKER_CMD,		EV_MCUMT_STD_BGN + 71 );//cancelMakeTerminalBroadcaster NULL

//�ն˷���MCU�����뷢������MT->MCU����Ϣ��NULL
//OSPEVENT( MT_MCU_APPLYSPEAKER_CMD,		EV_MCUMT_STD_BGN + 136 ); 
OSPEVENT( MT_MCU_APPLYSPEAKER_NOTIF,	EV_MCUMT_STD_BGN + 75 ); //requestForFloor ind
//֪ͨ��ϯ�ն����뷢������MCU->MT����Ϣ��Ϊ�ṹTMt
OSPEVENT( MCU_MT_APPLYSPEAKER_NOTIF,	EV_MCUMT_STD_BGN + 77 ); // floorRequested

/************************************************************************/
/* ��׼ ѡ���㲥                                                        */
/************************************************************************/

//��ϯ�ն˷���MCU��ѡ���ն����MT->MCU����Ϣ��Ϊ�ṹTSwitchInfo
OSPEVENT( MT_MCU_STARTSELMT_CMD,		EV_MCUMT_STD_BGN + 90 );//sendThisSource
//��ϯ�ն˷���MCU��ֹͣѡ���ն����MT->MCU����Ϣ��Ϊ�ṹTMt, ѡ���ն�
OSPEVENT( MT_MCU_STOPSELMT_CMD,			EV_MCUMT_STD_BGN + 92 );//cancelSendThisSource

//��ϯ�ն˷���MCU��ѡ���ն�����MT->MCU����Ϣ��Ϊ�ṹTSwitchInfo
OSPEVENT( MT_MCU_STARTSELMT_REQ,		EV_MCUMT_STD_BGN + 94 ); //sendThisSource
//��Ϣ��NULL
OSPEVENT( MCU_MT_STARTSELMT_ACK,		EV_MCUMT_STD_BGN + 95 );
OSPEVENT( MCU_MT_STARTSELMT_NACK,		EV_MCUMT_STD_BGN + 96 );

//MCU��֪�ն˵�ǰ��ƵԴ�� ��Ϣ��TMt
OSPEVENT( MCU_MT_YOUARESEEING_NOTIF,	EV_MCUMT_STD_BGN + 97 );

//MCU֪ͨ�ն˱�ѡ���� ��Ϣ��TMt
OSPEVENT( MCU_MT_SEENBYOTHER_NOTIF,		EV_MCUMT_STD_BGN + 98 );
//MCU֪ͨ�ն˱�ȡ��ѡ���� ��Ϣ��TMt
OSPEVENT( MCU_MT_CANCELSEENBYOTHER_NOTIF,		EV_MCUMT_STD_BGN + 99 );
/************************************************************************/
/* ��׼ �߼��ŵ�                                                        */
/************************************************************************/
//MCU����MT�Ĵ��߼�ͨ������MCU->MT (MCU->SMCU)����Ϣ��TLogicalChannel
OSPEVENT( MCU_MT_OPENLOGICCHNNL_REQ,	EV_MCUMT_STD_BGN + 100 );
//�ն˷���MCU�ĳɹ�Ӧ��MT->MCU����Ϣ��Ϊ�ṹTLogicalChannel
OSPEVENT( MT_MCU_OPENLOGICCHNNL_ACK,	EV_MCUMT_STD_BGN + 101 );
//�ն˷���MCU��ʧ��Ӧ��MT->MCU����Ϣ��ΪTLogicalChannel
OSPEVENT( MT_MCU_OPENLOGICCHNNL_NACK,	EV_MCUMT_STD_BGN + 102 );

//MT����MCU�Ĵ��߼�ͨ������MT->MCU����Ϣ��Ϊ�ṹTLogicalChannel
OSPEVENT( MT_MCU_OPENLOGICCHNNL_REQ,	EV_MCUMT_STD_BGN + 105 );
//MCU�����ն˵ĳɹ�Ӧ��MCU->MT (MCU->SMCU)����Ϣ��Ϊ�ṹTLogicalChannel
OSPEVENT( MCU_MT_OPENLOGICCHNNL_ACK,	EV_MCUMT_STD_BGN + 106 );
//MCU�����ն˵�ʧ��Ӧ��MCU->MT (MCU->SMCU)����Ϣ��Ϊ�ṹTLogicalChannel
OSPEVENT( MCU_MT_OPENLOGICCHNNL_NACK,	EV_MCUMT_STD_BGN + 107 );
//Э��ջ�ص���MtAdpת��MCU���߼�ͨ���ɹ�������֪ͨ��MT(stack)->MCU(SMCU->MCU)����Ϣ��Ϊ�ṹTLogicChannel
OSPEVENT( MT_MCU_LOGICCHNNLOPENED_NTF,  EV_MCUMT_STD_BGN + 104 );

//MCU�����ն˵�ChannelOn��Ϣ,��Ϣ����
OSPEVENT( MCU_MT_CHANNELON_CMD,	        EV_MCUMT_STD_BGN + 108 );
//MCU�����ն˵�ChannelOff��Ϣ,��Ϣ����
OSPEVENT( MCU_MT_CHANNELOFF_CMD,        EV_MCUMT_STD_BGN + 109 );

//�ն˿�ʼ�������룬��Ϣ�壺TLogicalChannel
OSPEVENT( MT_MCU_MEDIALOOPON_REQ,		EV_MCUMT_STD_BGN + 111 );
//��Ϣ�壺TLogicalChannel
OSPEVENT( MCU_MT_MEDIALOOPON_ACK,		EV_MCUMT_STD_BGN + 112 );
//��Ϣ�壺TLogicalChannel
OSPEVENT( MCU_MT_MEDIALOOPON_NACK,		EV_MCUMT_STD_BGN + 113 );
//�ն˽������������Ϣ�壺TLogicalChannel
OSPEVENT( MT_MCU_MEDIALOOPOFF_CMD,		EV_MCUMT_STD_BGN + 115 );

//MCU�����ն˶���ͼ�����MCU->MT������Ϣ��
OSPEVENT( MCU_MT_FREEZEPIC_CMD,			EV_MCUMT_STD_BGN + 116 );
OSPEVENT( MT_MCU_FREEZEPIC_CMD,			EV_MCUMT_STD_BGN + 118 );
//MCU�����ն˿��ٸ���ͼ�����MCU->MT (MCU->SMCU)����Ϣ��u8 byMode(MODE_VIDEO,MODE_SECVIDEO );
OSPEVENT( MCU_MT_FASTUPDATEPIC_CMD,		EV_MCUMT_STD_BGN + 120 );
OSPEVENT( MT_MCU_FASTUPDATEPIC_CMD,		EV_MCUMT_STD_BGN + 122 );

//������������, TLogicChannel
OSPEVENT( MT_MCU_FLOWCONTROL_CMD,		EV_MCUMT_STD_BGN + 124 );
OSPEVENT( MCU_MT_FLOWCONTROL_CMD,		EV_MCUMT_STD_BGN + 126 );

//MCU����MT�Ĺر��߼�ͨ������MCU->MT (MCU->SMCU)����Ϣ��Ϊ�ṹTLogicalChannel
OSPEVENT( MCU_MT_CLOSELOGICCHNNL_CMD,   EV_MCUMT_STD_BGN + 128 );

//MT����MCU�Ĺر��߼�ͨ������MT->MCU��
//��Ϣ��Ϊ�ṹTLogicalChannel+(u8)byOut�Ƿ����
OSPEVENT( MT_MCU_CLOSELOGICCHNNL_NOTIF, EV_MCUMT_STD_BGN + 130 );

//��������ָʾ, TLogicChannel
OSPEVENT( MT_MCU_FLOWCONTROLINDICATION_NOTIF,	EV_MCUMT_STD_BGN + 132 );
OSPEVENT( MCU_MT_FLOWCONTROLINDICATION_NOTIF,	EV_MCUMT_STD_BGN + 134 );

/************************************************************************/
/* ��׼ ��·ά��                                                        */
/************************************************************************/

//MCU����MT��RoundTripDelay����MCU->MT (MCU->SMCU)������Ϣ��
OSPEVENT( MCU_MT_ROUNDTRIPDELAY_REQ,	EV_MCUMT_STD_BGN + 135 );
//�ն˷���MCU�ĳɹ�Ӧ��MT->MCU������Ϣ��
OSPEVENT( MT_MCU_ROUNDTRIPDELAY_ACK,	EV_MCUMT_STD_BGN + 136 );
//MT����MCU��RoundTripDelay����MCU->MT������Ϣ��
OSPEVENT( MT_MCU_ROUNDTRIPDELAY_REQ,	EV_MCUMT_STD_BGN + 139 );
//MCU�����ն˵ĳɹ�Ӧ��MT->MCU������Ϣ��
OSPEVENT( MCU_MT_ROUNDTRIPDELAY_ACK,	EV_MCUMT_STD_BGN + 140 );


/************************************************************************/
/* ��׼ ����                                                            */
/************************************************************************/

//MCU�����ն˵�������֪ͨ��MCU->MT (MCU->SMCU)����Ϣ��Ϊ�ṹTCapSupport+u8(bySupportH263+)+u8(bySendMSDetermine)+TCapSupportEx
OSPEVENT( MCU_MT_CAPBILITYSET_NOTIF,	EV_MCUMT_STD_BGN + 142 );
//�ն˷���MCU��������֪ͨ��MT->MCU����Ϣ��Ϊ�ṹTMultiCapSupport
OSPEVENT( MT_MCU_CAPBILITYSET_NOTIF,	EV_MCUMT_STD_BGN + 144 );
//�ն����䷢��ҵ�������ȷ�������֪ͨ����Ϣ��u8(ȷ�����), 1Ϊmaster������Ϊslave
OSPEVENT( MT_MCU_MSD_NOTIF,				EV_MCUMT_STD_BGN + 146 );


//�ն˷���MCU�Ļ�������ն��б��ѯ����MT->MCU������Ϣ��
OSPEVENT( MT_MCU_JOINEDMTLIST_REQ,		EV_MCUMT_STD_BGN + 150 );
//MCU���سɹ������MCU->MT����Ϣ��ΪTMt����
OSPEVENT( MCU_MT_JOINEDMTLIST_ACK,		EV_MCUMT_STD_BGN + 151 );
//MCU����ʧ�ܽ����MCU->MT����Ϣ��
OSPEVENT( MCU_MT_JOINEDMTLIST_NACK,		EV_MCUMT_STD_BGN + 152 );

//�ն˷���MCU�Ļ�������ն��б���ID��ѯ����MT->MCU������Ϣ��
OSPEVENT( MT_MCU_JOINEDMTLISTID_REQ,	EV_MCUMT_STD_BGN + 160 );
//MCU���سɹ������MCU->MT����Ϣ��Ϊ(TMt + TMtAlias)�����ѹ����ʽ
OSPEVENT( MCU_MT_JOINEDMTLISTID_ACK,	EV_MCUMT_STD_BGN + 161 );
OSPEVENT( MCU_MT_JOINEDMTLISTID_NACK,	EV_MCUMT_STD_BGN + 162 );

//��ѯ�ض��ն˱�������Ϣ��TMt
OSPEVENT( MT_MCU_GETMTALIAS_REQ,        EV_MCUMT_STD_BGN + 165 );
//��ѯ�ض��ն˱���Ӧ����Ϣ��TMt+TMtAlias
OSPEVENT( MCU_MT_GETMTALIAS_ACK,        EV_MCUMT_STD_BGN + 166 );
OSPEVENT( MCU_MT_GETMTALIAS_NACK,       EV_MCUMT_STD_BGN + 167 );
//�ն˻㱨����, TMtAlias
OSPEVENT( MT_MCU_MTALIAS_NOTIF,         EV_MCUMT_STD_BGN + 170 );


/************************************************************************/
/* ��׼ ���������                                                      */
/************************************************************************/

//MCU�����ն˵ľ�ͷ��������

//�����ն��������ͷ�˶����MCU->MT (MCU->SMCU)����Ϣ��Ϊ�ṹTMt+1�ֽڣ�
OSPEVENT( MCU_MT_MTCAMERA_CTRL_CMD,		    EV_MCUMT_STD_BGN + 175 );
//�����ն��������ͷֹͣ�˶����MCU->MT (MCU->SMCU)����Ϣ��Ϊ�ṹTMt+1�ֽڣ�
OSPEVENT( MCU_MT_MTCAMERA_CTRL_STOP,	    EV_MCUMT_STD_BGN + 177 );
//�����ն������ң����ʹ�����MCU->MT (MCU->SMCU)����Ϣ��Ϊ�ṹTMt+1�ֽڣ�
OSPEVENT( MCU_MT_MTCAMERA_RCENABLE_CMD,	    EV_MCUMT_STD_BGN + 179 );
//�����ն������Ҫ�󽫵�ǰλ����Ϣ����ָ��λ�ã�MCU->MT����Ϣ��Ϊ�ṹTMt+1�ֽڣ�
//1��4��Ԥ��λ�úš�
OSPEVENT( MCU_MT_MTCAMERA_SAVETOPOS_CMD,	EV_MCUMT_STD_BGN + 181 );
//�����ն������Ҫ�������ָ��λ�ã�MCU->MT (MCU->SMCU)����Ϣ��Ϊ�ṹTMt+1�ֽڣ�
//1��4��Ԥ��λ�úš�
OSPEVENT( MCU_MT_MTCAMERA_MOVETOPOS_CMD,	EV_MCUMT_STD_BGN + 183 );
//�����ն˵��л���ƵԴ���MCU->MT (MCU->SMCU)����Ϣ��Ϊ�ṹTMt+1�ֽڣ�
OSPEVENT( MCU_MT_VIDEOSOURCESWITCHED_CMD,	EV_MCUMT_STD_BGN + 184 );
//�����ն˵�ѡ����ƵԴ���MCU->MT (MCU->SMCU)����Ϣ��Ϊ�ṹTMt+1�ֽڣ�
OSPEVENT( MCU_MT_SETMTVIDSRC_CMD,	EV_MCUMT_STD_BGN + 182 );


//��ϯ�ն˷����ն˾�ͷ��������

//�����ն��������ͷ�˶����MT->MCU����Ϣ��Ϊ�ṹTEqp+1�ֽ�(�μ�����ͷ���Ʋ�������)
OSPEVENT( MT_MCU_MTCAMERA_CTRL_CMD,			EV_MCUMT_STD_BGN + 185 );
//�����ն��������ͷֹͣ�˶��˶����MT->MCU����Ϣ��Ϊ�ṹTEqp+1�ֽ�(�μ�����ͷ���Ʋ�������)
OSPEVENT( MT_MCU_MTCAMERA_CTRL_STOP,		EV_MCUMT_STD_BGN + 187 );
//�����ն������ң����ʹ�����MT->MCU����Ϣ��Ϊ�ṹTEqp+1�ֽڣ�
OSPEVENT( MT_MCU_MTCAMERA_RCENABLE_CMD,		EV_MCUMT_STD_BGN + 189 );
//�����ն������Ҫ�󽫵�ǰλ����Ϣ����ָ��λ�ã�MT->MCU����Ϣ��Ϊ�ṹTEqp+1�ֽڣ�
//1��4��Ԥ��λ�úš�
OSPEVENT( MT_MCU_MTCAMERA_SAVETOPOS_CMD,	EV_MCUMT_STD_BGN + 191 );
//�����ն������Ҫ�������ָ��λ�ã�MT->MCU����Ϣ��Ϊ�ṹTEqp+1�ֽڣ�
//1��4��Ԥ��λ�úš�
OSPEVENT( MT_MCU_MTCAMERA_MOVETOPOS_CMD,	EV_MCUMT_STD_BGN + 193 );

OSPEVENT( MT_MCU_VIDEOSOURCESWITCHED_CMD,	EV_MCUMT_STD_BGN + 194 );
OSPEVENT( MT_MCU_SELECTVIDEOSOURCE_CMD,	    EV_MCUMT_STD_BGN + 195 );


/************************************************************************/
/* ��׼ ���ز���                                                        */
/************************************************************************/

//ע������,��Ϣ��: TMtAlias+TMtAlias(��Ϊmcuע�ᣬ��Ϊ323id������null)+TTransportAddr[](Э��������ַ)
OSPEVENT( MCU_MT_REGISTERGK_REQ,		    EV_MCUMT_STD_BGN + 200 );
//ע��ȷ��, NULL
OSPEVENT( MT_MCU_REGISTERGK_ACK,		    EV_MCUMT_STD_BGN + 201 );
//ע��ܾ�, NULL
OSPEVENT( MT_MCU_REGISTERGK_NACK,		    EV_MCUMT_STD_BGN + 202 );


//ע������, ��Ϣ��: TMtAlias
OSPEVENT( MCU_MT_UNREGISTERGK_REQ,		    EV_MCUMT_STD_BGN + 210 );
//ע��ȷ��, NULL
OSPEVENT( MT_MCU_UNREGISTERGK_ACK,	  	    EV_MCUMT_STD_BGN + 211 );
//ע���ܾ�, NULL
OSPEVENT( MT_MCU_UNREGISTERGK_NACK,		    EV_MCUMT_STD_BGN + 212 );

//��������������MCU������ʵ���GKע����Ϣ��֪ͨ, ��Ϣ��: TMtAlias+u8(byConfIdx)+u8(byRegState)
OSPEVENT( MCU_MT_UPDATE_REGGKSTATUS_NTF,    EV_MCUMT_STD_BGN + 213 );

//��������������GK��ע���GatekeeperID/EndpointID��Ϣ��֪ͨ, ��Ϣ��: TH323EPGKIDAlias(m_tGKID)+TH323EPGKIDAlias(m_tEPID)
OSPEVENT( MT_MCU_UPDATE_GKANDEPID_NTF,	    EV_MCUMT_STD_BGN + 215 );

//��������������GK��ע���GatekeeperID/EndpointID��Ϣ��֪ͨ, ��Ϣ��: TH323EPGKIDAlias(m_tGKID)+TH323EPGKIDAlias(m_tEPID)
OSPEVENT( MCU_MT_UPDATE_GKANDEPID_NTF,	    EV_MCUMT_STD_BGN + 216 );

//ǿ��ע�� ��MCU������ע��GK�� ʵ�壬N+1�����á���Ϣ�壺TRASInfo
OSPEVENT( MCU_MT_UNREGGK_NPLUS_CMD,         EV_MCUMT_STD_BGN + 217 );

//֪ͨ ����MCU �ָ��䱾����E164�š���Ϣ�壺NULL
OSPEVENT( MCU_MT_RESTORE_MCUE164_NTF,       EV_MCUMT_STD_BGN + 218 );

/************************************************************************/
/* H239 ���Ʋ���                                                        */
/************************************************************************/

//�ն˸�MCU�� ��ȡH239���� Ȩ������
OSPEVENT( MT_MCU_GETH239TOKEN_REQ,	  	    EV_MCUMT_STD_BGN + 220 );
//MCU���ն˵� ��ȡH239����Ȩ������� ��Ӧ֪ͨ
OSPEVENT( MCU_MT_GETH239TOKEN_ACK,	        EV_MCUMT_STD_BGN + 221 );
//MCU���ն˵� ��ȡH239����Ȩ������� ��Ӧ֪ͨ
OSPEVENT( MCU_MT_GETH239TOKEN_NACK,	        EV_MCUMT_STD_BGN + 222 );

//�ն˸�MCU�� ӵ�� H239���� Ȩ��֪ͨ
OSPEVENT( MT_MCU_OWNH239TOKEN_NOTIF,	    EV_MCUMT_STD_BGN + 223 );
//�ն˸�MCU�� �ͷ� H239���� Ȩ��֪ͨ
OSPEVENT( MT_MCU_RELEASEH239TOKEN_NOTIF,	EV_MCUMT_STD_BGN + 224 );

//MCU���ն˵� �ͷ�H239���� Ȩ������ - 
//ʵ��Ϊ���������һ�������ͷŶԷ�����,һ����ΪĬ�ϳɹ��Ҳ���response��Ӧ����
OSPEVENT( MCU_MT_RELEASEH239TOKEN_CMD,	  	EV_MCUMT_STD_BGN + 225 );
//MCU���ն˵� ӵ�� H239���� Ȩ��֪ͨ
OSPEVENT( MCU_MT_OWNH239TOKEN_NOTIF,	    EV_MCUMT_STD_BGN + 226 );

//MCU���������ն˻�mcu��������ϯ����(Ϊ��ӦԶң��֧��)��MCU->ZTE_MT, ��Ϣ��: NULL
OSPEVENT( MCU_MT_APPLYCHAIRMAN_CMD,			EV_MCUMT_STD_BGN + 228 );
//mcu����ȡ����ϯ������Ϣ�壺��
OSPEVENT( MCU_MT_CANCELCHAIRMAN_CMD,		EV_MCUMT_STD_BGN + 229 );


/************************************************************************/
/* ����ƷѲ���                                                         */
/************************************************************************/
//��������GK������Ϣ(�ڲ���)
OSPEVENT( MTADP_GK_CONNECT,                 EV_MCUMT_STD_BGN + 230 );

//��������GKע����Ϣ (�ڲ���)
OSPEVENT( MTADP_GK_REGISTER,                EV_MCUMT_STD_BGN + 231 );

//�Ʒ���·��������ϱ�MCU����Ϣ�� NULL
OSPEVENT( MT_MCU_CHARGE_REGGK_NOTIF,        EV_MCUMT_STD_BGN + 232 );

//MCU����������Ŀ�ʼ�Ʒ�����, ��Ϣ�� TConfChargeInfo
OSPEVENT( MCU_MT_CONF_STARTCHARGE_REQ,      EV_MCUMT_STD_BGN + 234 );
//�����䷵�ظ�MCU�Ŀ�ʼ�Ʒѵ�ͬ��Ӧ��, ��Ϣ�� TAcctSessionId
OSPEVENT( MT_MCU_CONF_STARTCHARGE_ACK,      EV_MCUMT_STD_BGN + 235 );
//�����䷵�ظ�MCU�Ŀ�ʼ�Ʒѵľܾ�Ӧ��, ��Ϣ�� NULL 
OSPEVENT( MT_MCU_CONF_STARTCHARGE_NACK,     EV_MCUMT_STD_BGN + 236 );

//MCU����������Ľ����Ʒ�����, ��Ϣ�� NULL
OSPEVENT( MCU_MT_CONF_STOPCHARGE_REQ,       EV_MCUMT_STD_BGN + 237 );
//�����䷵�ظ�MCU�Ľ����Ʒѵ�ͬ��Ӧ��, ��Ϣ�� NULL
OSPEVENT( MT_MCU_CONF_STOPCHARGE_ACK,       EV_MCUMT_STD_BGN + 238 );
//�����䷵�ظ�MCU�Ľ����Ʒѵľܾ�Ӧ��, ��Ϣ�� NULL
OSPEVENT( MT_MCU_CONF_STOPCHARGE_NACK,      EV_MCUMT_STD_BGN + 239 );

//�����䷢��MCU�Ļ���Ʒ��쳣֪ͨ����Ϣ�� NULL
OSPEVENT( MT_MCU_CONF_CHARGEEXP_NOTIF,		EV_MCUMT_STD_BGN + 240 );

//�����䷢��MCU��ֹͣ�Ʒ������Ϣ��: TConfChargeInfo + TAcctSessionId
OSPEVENT( MCU_MT_CONF_STOPCHARGE_CMD,       EV_MCUMT_STD_BGN + 241  );

/*  ���Polycom��MCU��˫��ͨ������ H239 ����  */

//KEDAMCU��Polycom��MCU����H239��������
OSPEVENT( MCU_POLY_GETH239TOKEN_REQ,			EV_MCUMT_STD_BGN + 245 );
//PolyMCU ��ȡ���ƵĻ�Ӧ
OSPEVENT( POLY_MCU_GETH239TOKEN_ACK,			EV_MCUMT_STD_BGN + 246 );
//PolyMCU ��ȡ���ƵĻ�Ӧ
OSPEVENT( POLY_MCU_GETH239TOKEN_NACK,			EV_MCUMT_STD_BGN + 247 );

//������Ϣ ���polymcu���¼�ʱ����δʹ��
//KEDAMCU ��PolyMCU ��ӵ��H239���Ƶ�Ȩ��֪ͨ
OSPEVENT( MCU_POLY_OWNH239TOKEN_NOTIF,			EV_MCUMT_STD_BGN + 248 );
//PolyMCU ��KEDAMCU ���ͷ�H239���Ƶ�Ȩ��֪ͨ
OSPEVENT( POLY_MCU_RELEASEH239TOKEN_NOTIF,		EV_MCUMT_STD_BGN + 249 );
//KEDAMCU ��PolyMCU �ͷ�H239����Ȩ�޵�����
OSPEVENT( MCU_POLY_RELEASEH239TOKEN_CMD,		EV_MCUMT_STD_BGN + 252 );

//PolyMCU��KEDAMCU���ͷ�H239���� ����
OSPEVENT( POLY_MCU_RELEASEH239TOKEN_CMD,		EV_MCUMT_STD_BGN + 250 );
//PolyMCU��KEDAMCU�� ӵ��H239���Ƶ� Ȩ��֪ͨ
OSPEVENT( POLY_MCU_OWNH239TOKEN_NOTIF,			EV_MCUMT_STD_BGN + 251 );

/************************************************************************/
/* MCU <=> MMCU    (��׼���Զ���)                                       */
/************************************************************************/

//mmcu asks password
OSPEVENT(MMCU_MCU_ENTERPASSWORD_REQ,        EV_MCUMCU_STD_BGN + 1 );
OSPEVENT(MCU_MMCU_ENTERPASSWORD_ACK,        EV_MCUMCU_STD_BGN + 2 );
OSPEVENT(MCU_MMCU_ENTERPASSWORD_NACK,       EV_MCUMCU_STD_BGN + 3 );

//mcu ask another mcu for mtlist
// ͷ����ӱ�ʾ�Ƿ�Ҫ����MMcu [pengguofeng 7/10/2013]
//content: bySend2MMcu + byMcuNum + byMcuNum * byMcuMtId
OSPEVENT( MCU_MTADP_GETMTLIST_CMD,          EV_MCUMCU_STD_BGN + 4);
//content: bySend2MMcu + byMcuMtId + byMtNum + [ byMtId + wNameLen + pName ] * byMtNum + byMcuMtId_x + byMtNum_x + ...
OSPEVENT( MTADP_MCU_GETMTLIST_NOTIF,        EV_MCUMCU_STD_BGN + 7);
// mcu����mcu��ȡ�ն˱�����Ϣ��Ŀǰ�����¼��ն���vmp�б�����ʾ(��������)��
//content:byEqpid + byMcuId + byMtNum + byMtNum*[ byMtId + byChlNum + byChlNum*[byChlIdx]]
OSPEVENT( MCU_MTADP_GETSMCUMTALIAS_CMD,     EV_MCUMCU_STD_BGN + 8);
//content:byEqpid + byMcuId + byMtNum + byMtNum*[ byMtId + byChlNum + byChlNum*[byChlIdx] + byNameLen + pName]
OSPEVENT( MTADP_MCU_GETSMCUMTALIAS_NOTIF,   EV_MCUMCU_STD_BGN + 9);


/************************************************************************/
/* MCU <=> MMCU  3.6�汾�ı�׼������Ϣ                                  */
/************************************************************************/


////////////////�����ն˲���//////////////////////////

//����mcu���¼�mcu�������նˣ���Ϣ��ΪTMcuMcuReq+TMCU+TAddMtInfo����
OSPEVENT(MCU_MCU_INVITEMT_REQ,			    EV_MCUMCU_STD_BGN + 10 );
OSPEVENT(MCU_MCU_INVITEMT_ACK,			    EV_MCUMCU_STD_BGN + 11 );
OSPEVENT(MCU_MCU_INVITEMT_NACK,			    EV_MCUMCU_STD_BGN + 12 );
//�Է�MCU�������ն�֪ͨ, ��Ϣ��TMcuMcuMtInfo
OSPEVENT(MCU_MCU_NEWMT_NOTIF,			    EV_MCUMCU_STD_BGN + 13 );

//����mcu���¼�mcu�Ϻ����նˣ���Ϣ��ΪTMcuMcuReq+TMt
OSPEVENT(MCU_MCU_REINVITEMT_REQ,			EV_MCUMCU_STD_BGN + 14 );
OSPEVENT(MCU_MCU_REINVITEMT_ACK,			EV_MCUMCU_STD_BGN + 15 );
OSPEVENT(MCU_MCU_REINVITEMT_NACK,			EV_MCUMCU_STD_BGN + 16 );

//mcu<->mcu�Ҷ��ն�������Ϣ��ΪTMcuMcuReq+TMt
OSPEVENT(MCU_MCU_DROPMT_REQ,				EV_MCUMCU_STD_BGN + 17 );
OSPEVENT(MCU_MCU_DROPMT_ACK,				EV_MCUMCU_STD_BGN + 18 );
OSPEVENT(MCU_MCU_DROPMT_NACK,				EV_MCUMCU_STD_BGN + 19 );

//mcu<->mcu�Ҷ��ն�֪ͨ����Ϣ��ΪTMt
OSPEVENT(MCU_MCU_DROPMT_NOTIF,				EV_MCUMCU_STD_BGN + 20 );

//���ں���ͨ��, ��Ϣ��ΪTMt+TMtAlias
OSPEVENT(MCU_MCU_CALLALERTING_NOTIF, EV_MCUMCU_STD_BGN + 21 );

//mcu<->mcuɾ���ն�������Ϣ��ΪTMcuMcuReq+TMt
OSPEVENT(MCU_MCU_DELMT_REQ,					EV_MCUMCU_STD_BGN + 22 );
OSPEVENT(MCU_MCU_DELMT_ACK,					EV_MCUMCU_STD_BGN + 23 );
OSPEVENT(MCU_MCU_DELMT_NACK,				EV_MCUMCU_STD_BGN + 24 );

//mcu<->mcuɾ���ն�֪ͨ����Ϣ��ΪTMt
OSPEVENT(MCU_MCU_DELMT_NOTIF,				EV_MCUMCU_STD_BGN + 25 );

//mcu-mcu��������ý��ͨ������, ��Ϣ��:TMcuMcuReq+TMtMediaChanStatus
OSPEVENT(MCU_MCU_SETMTCHAN_REQ,				EV_MCUMCU_STD_BGN + 26 );
OSPEVENT(MCU_MCU_SETMTCHAN_ACK,				EV_MCUMCU_STD_BGN + 27 );
OSPEVENT(MCU_MCU_SETMTCHAN_NACK,			EV_MCUMCU_STD_BGN + 28 );

//mcu-mcuý��ͨ������״̬֪ͨ, ��Ϣ��:TMtMediaChanStatus
OSPEVENT(MCU_MCU_SETMTCHAN_NOTIF,			EV_MCUMCU_STD_BGN + 29 );

//mcu-mcuָ���ն�ͼ�����view,��Ϣ��:TMcuMcuReq+TSetInParam+  Optional-H320(bySwitchMode) 
OSPEVENT(MCU_MCU_SETIN_REQ,					EV_MCUMCU_STD_BGN + 30 );
OSPEVENT(MCU_MCU_SETIN_ACK,					EV_MCUMCU_STD_BGN + 31 );
OSPEVENT(MCU_MCU_SETIN_NACK,				EV_MCUMCU_STD_BGN + 32 );

//mcu-mcuָ��view���뵽�ն�, ��Ϣ��:TMcuMcuReq+TSetOutParam
OSPEVENT(MCU_MCU_SETOUT_REQ,				EV_MCUMCU_STD_BGN + 33 );
OSPEVENT(MCU_MCU_SETOUT_ACK,				EV_MCUMCU_STD_BGN + 34 );
OSPEVENT(MCU_MCU_SETOUT_NACK,				EV_MCUMCU_STD_BGN + 35 );

//mcu-mcuָ��view���뵽�ն�֪ͨ, ��Ϣ��:TSetOutParam
OSPEVENT(MCU_MCU_SETOUT_NOTIF,				EV_MCUMCU_STD_BGN + 36 );

//RadMcu->KedaMcu �Զ���ѯ������Ϣ�壺TRadMMcuAutoSwitchReq
OSPEVENT(MCU_MCU_AUTOSWITCH_REQ,            EV_MCUMCU_STD_BGN + 37 );
//KedaMcu->RadMcu �Զ���ѯӦ����Ϣ�壺NULL
OSPEVENT(MCU_MCU_AUTOSWITCH_ACK,            EV_MCUMCU_STD_BGN + 38 );
OSPEVENT(MCU_MCU_AUTOSWITCH_NACK,           EV_MCUMCU_STD_BGN + 39 );


////////////////����ͷԶң����///////////////////////

//��MCU��������ͷԶң, ��Ϣ��TMt + 1byte��Զң�������� + 1 byte: Զң����
OSPEVENT( MCU_MCU_FECC_CMD,                 EV_MCUMCU_STD_BGN + 40 );


//���ն˶���Ϣ֪ͨ��MCU->MT ��Ϣ�壺TMt(�ն˺�Ϊ0�㲥�������ն�)+TROLLMSG(��Ч����)
OSPEVENT( MCU_MCU_SENDMSG_NOTIF,			EV_MCUMCU_STD_BGN + 42 );


//MCU����ͨ���򿪺��ע����Ϣ,��Ϣ��:��
OSPEVENT(MCU_MCU_REGISTER_NOTIF,			EV_MCUMCU_STD_BGN + 44 );

//mcu�ϲ������򿪺�Ļ�����֪ͨ,��Ϣ��TConfMtInfo
OSPEVENT(MCU_MCU_ROSTER_NOTIF,			    EV_MCUMCU_STD_BGN + 45 );

//lukunpeng 2010/07/23 ����61 - 63�Ѿ��ڹ��������б�MCU_MCU_APPLYSPEAKER_REQռ�ã��˴���Ϣ��Ҫ����
// [pengjie 2010/4/23] �������¼��ն�֡�ʣ���Ϣ��:Tmt(Ҫ�����ն�) + u8(�ǻָ����ǵ���) + u8(�ŵ�����) + u8(������֡��ֵ)
OSPEVENT(MCU_MCU_ADJMTFPS_REQ,	            EV_MCUMCU_STD_BGN + 46 );
OSPEVENT(MCU_MCU_ADJMTFPS_ACK,	            EV_MCUMCU_STD_BGN + 47 );
OSPEVENT(MCU_MCU_ADJMTFPS_NACK,	            EV_MCUMCU_STD_BGN + 48 );


//����Է�MCU���ն��б�,��Ϣ��TMcuMcuReq
OSPEVENT(MCU_MCU_MTLIST_REQ,			    EV_MCUMCU_STD_BGN + 50 );
//MCU���ն��б�����ɹ���Ӧ,��Ϣ��u8(1-���һ��,0-�м��)+TMcuMcuMtInfo[]
OSPEVENT(MCU_MCU_MTLIST_ACK,			    EV_MCUMCU_STD_BGN + 51 );
//MCU���ն��б�����ʧ����Ӧ,��Ϣ����
OSPEVENT(MCU_MCU_MTLIST_NACK,			    EV_MCUMCU_STD_BGN + 52 );

//����Է�MCU����Ƶ��Ϣ, ��Ϣ��TMcuMcuReq
OSPEVENT(MCU_MCU_VIDEOINFO_REQ,			    EV_MCUMCU_STD_BGN + 55 );
//����Է�MCU����Ƶ��Ϣ�ĳɹ�Ӧ��,��Ϣ�壺TCConfViewInfo
OSPEVENT(MCU_MCU_VIDEOINFO_ACK,			    EV_MCUMCU_STD_BGN + 56 );
//����Է�MCU����Ƶ��Ϣ��ʧ��Ӧ��,��Ϣ����
OSPEVENT(MCU_MCU_VIDEOINFO_NACK,			EV_MCUMCU_STD_BGN + 57 );

//mcu<->������ͼ�����仯,��Ϣ��:TCConfViewChangeNtf
OSPEVENT(MCU_MCU_CONFVIEWCHG_NOTIF,	        EV_MCUMCU_STD_BGN + 60 );

//���������Ϊ�ϼ��ķ����ˣ���Ϣ�壺TMMcuReqInfo��TMt
OSPEVENT(MCU_MCU_APPLYSPEAKER_REQ,          EV_MCUMCU_STD_BGN + 61 );
OSPEVENT(MCU_MCU_APPLYSPEAKER_ACK,          EV_MCUMCU_STD_BGN + 62 );
OSPEVENT(MCU_MCU_APPLYSPEAKER_NACK,         EV_MCUMCU_STD_BGN + 63 );

//���������Ϊ�ϼ��ķ����ˣ���Ϣ�壺TMt
OSPEVENT(MCU_MCU_APPLYSPEAKER_NOTIF,        EV_MCUMCU_STD_BGN + 64 );

//����Է�MCU����Ƶ��Ϣ
OSPEVENT(MCU_MCU_AUDIOINFO_REQ,			    EV_MCUMCU_STD_BGN + 65 );
//����Է�MCU����Ƶ��Ϣ�ĳɹ�Ӧ��, ��Ϣ��:TCConfAudioInfo
OSPEVENT(MCU_MCU_AUDIOINFO_ACK,			    EV_MCUMCU_STD_BGN + 66 );
//����Է�MCU����Ƶ��Ϣ��ʧ��Ӧ��,��Ϣ����
OSPEVENT(MCU_MCU_AUDIOINFO_NACK,			EV_MCUMCU_STD_BGN + 67 );

//�����������缶����״̬֪ͨ����Ϣ�壺TMt �� u8 byMode ( emAgreed )
OSPEVENT( MCU_MCU_SPEAKSTATUS_NTF,			EV_MCUMCU_STD_BGN + 68 );
OSPEVENT( MCU_MCU_SPEAKERMODE_NOTIFY,		EV_MCUMCU_STD_BGN + 69 );


/*******************************************************************/
/*nostandard message                                               */
/*******************************************************************/
//����Է�MCU��ʼ����, ��Ϣ��:TMcu+1byte(ͬʱ�μ�����(����)�ĳ�Ա����)+TMt����(ָ����Ա����ʱ����)
OSPEVENT(MCU_MCU_STARTMIXER_CMD,			EV_MCUMCU_STD_BGN + 70 );
//����Է�MCU��ʼ������ʧ��Ӧ��,��Ϣ�壺TMcu+1byte(ͬʱ�μ�����(����)�ĳ�Ա����)
OSPEVENT(MCU_MCU_STARTMIXER_NOTIF,			EV_MCUMCU_STD_BGN + 73 );

//����Է�MCUֹͣ����, ��Ϣ��:TMcu
//OSPEVENT(MCU_MCU_STOPMIXER_REQ,			EV_MCUMCU_STD_BGN + 75 );
//����Է�MCU��ʼ�����ĳɹ�Ӧ��, ��Ϣ��:TMcu
//OSPEVENT(MCU_MCU_STOPMIXER_ACK,			EV_MCUMCU_STD_BGN + 76 );
//����Է�MCU��ʼ������ʧ��Ӧ��,��Ϣ��TMcu
//OSPEVENT(MCU_MCU_STOPMIXER_NACK,			EV_MCUMCU_STD_BGN + 77 );

//����Է�MCUֹͣ����, ��Ϣ��:TMcu
OSPEVENT(MCU_MCU_STOPMIXER_CMD,				EV_MCUMCU_STD_BGN + 75 );
//����Է�MCU��ʼ������ʧ��Ӧ��,��Ϣ��TMcu
OSPEVENT(MCU_MCU_STOPMIXER_NOTIF,			EV_MCUMCU_STD_BGN + 78 );

//����Է�MCU��������, ��Ϣ��:TMcu
OSPEVENT(MCU_MCU_GETMIXERPARAM_REQ,			EV_MCUMCU_STD_BGN + 80 );
//����Է�MCU���������ĳɹ�Ӧ��, ��Ϣ��TMcu+TDiscussParam
OSPEVENT(MCU_MCU_GETMIXERPARAM_ACK,			EV_MCUMCU_STD_BGN + 81 );
//����Է�MCU����������ʧ��Ӧ��,��Ϣ��TMcu
OSPEVENT(MCU_MCU_GETMIXERPARAM_NACK,		EV_MCUMCU_STD_BGN + 82 );
//����Է�MCU��������֪ͨ,��Ϣ��TMcu+TDiscussParam
OSPEVENT(MCU_MCU_MIXERPARAM_NOTIF,			EV_MCUMCU_STD_BGN + 83 );

//���ӻ�����Ա, ��Ϣ�壺TMcu+TMt����
OSPEVENT( MCU_MCU_ADDMIXMEMBER_CMD,		    EV_MCUMCU_STD_BGN + 85 );
//�Ƴ�������Ա, ��Ϣ�壺TMcu+TMt����
OSPEVENT( MCU_MCU_REMOVEMIXMEMBER_CMD,	    EV_MCUMCU_STD_BGN + 87 );

//���������Է�MCU, ��Ϣ��:TMcu+byLocdMode(CONF_LOCKMODE_NONE,CONF_LOCKMODE_LOCK..)
OSPEVENT(MCU_MCU_LOCK_REQ,			        EV_MCUMCU_STD_BGN + 89 );
//���������Է�MCU�ɹ�Ӧ��, ��Ϣ��TMcu
OSPEVENT(MCU_MCU_LOCK_ACK,			        EV_MCUMCU_STD_BGN + 90 );
//���������Է�MCUʧ��Ӧ��,��Ϣ��TMcu
OSPEVENT(MCU_MCU_LOCK_NACK,			        EV_MCUMCU_STD_BGN + 91 );

//����Է�MCUĳһ�ն�״̬, ��Ϣ��:TMcu
OSPEVENT(MCU_MCU_MTSTATUS_CMD,				EV_MCUMCU_STD_BGN + 92 );
//֪ͨ�Է�MCU�Լ�ĳһ����ֱ���ն�״̬,��Ϣ��TMcu
OSPEVENT(MCU_MCU_MTSTATUS_NOTIF,			EV_MCUMCU_STD_BGN + 95 );

//����Է�MCU��������ֱ���ն˵���Ƶ�ֱ��ʣ���Ϣ��:
OSPEVENT(MCU_MCU_ADJMTRES_REQ,				EV_MCUMCU_STD_BGN + 96 );
OSPEVENT(MCU_MCU_ADJMTRES_ACK,				EV_MCUMCU_STD_BGN + 97 );
OSPEVENT(MCU_MCU_ADJMTRES_NACK,				EV_MCUMCU_STD_BGN + 98 );

////[5/4/2011 zhushengze]VCS���Ʒ����˷�˫��
OSPEVENT(MCU_MCU_CHANGEMTSECVIDSEND_CMD,    EV_MCUMCU_STD_BGN + 99);
//[2/23/2012 zhushengze]���桢�ն���Ϣ͸��
OSPEVENT(MCU_MCU_TRANSPARENTMSG_NOTIFY,     EV_MCUMCU_STD_BGN + 100);

/************************************************************************/
/* �Զ��� ������ش���Ϣ                                                */
/************************************************************************/
//mcu�Ƿ�֧�ֶ�ش�������̽��Ϣ,u32 �ش�����
OSPEVENT(MCU_MCU_MULTSPYCAP_NOTIF,	    	 EV_MCUMCU_KDV_BGN + 1 );
  
//�ϼ�mcu�����¼�mcu��Ҫ���м�����ش���setin ǰ��׼������
//�¼�mcu���ݴ��ж��Լ��Ƿ��ܻش����������¼��ܴ����ն˻ش����ʣ�
//��ϢӦ������Ҫ�ش����նˣ��ش���mode(AUDIO��VIDEO��BOTH)���ش���ý���ʽ�����ʡ�֡��
OSPEVENT(MCU_MCU_PRESETIN_REQ,	    	     EV_MCUMCU_KDV_BGN + 2 );
//�ϼ�������SMCU����ACK/NACK, �ж����ܷ����㹻����������һ��spy,��Ϣ��
//�����¼�mcu��������ش�ͨ����Ҫ�Ĵ����¼��ն��Ƿ�֧�ֵ��ֱ��ʵı�־
OSPEVENT(MCU_MCU_PRESETIN_ACK,	    	     EV_MCUMCU_KDV_BGN + 3 );
OSPEVENT(MCU_MCU_PRESETIN_NACK,	    	     EV_MCUMCU_KDV_BGN + 4 );

//֪ͨ�¼�mcu���ϼ����Խ�����ش������Ĵ���
OSPEVENT(MCU_MCU_SPYCHNNL_NOTIF,	    	 EV_MCUMCU_KDV_BGN + 5 );

//֪ͨ�¼�mcu���ϼ�mcu�����㣬���ܽ��м�����ش�
OSPEVENT(MCU_MCU_REJECTSPY_NOTIF,	    	 EV_MCUMCU_KDV_BGN + 6 );

//�ϼ�mcu�����¼����ͷ�һ·������Ƶ�ش�ͨ����CMD�����ջش�ͨ��
OSPEVENT(MCU_MCU_BANISHSPY_CMD,	    	     EV_MCUMCU_KDV_BGN + 7 );

// �¼�mcu�����ϼ����ͷ�һ·������Ƶ�ش�ͨ����֪ͨ���ϼ��յ����ͷ��Լ���Ӧ��������Ƶ�ش�ͨ����Դ
//lukunpeng 2010/06/10 �������������ϼ������¼�����ռ��������ʲ���Ҫ��ͨ���¼�֪ͨ�ϼ�
//OSPEVENT(MCU_MCU_BANISHSPY_NOTIF,	    	 EV_MCUMCU_KDV_BGN + 8 );

// ������ش��ؼ�֡������Ϣ����Ϣ��Ϊ��u8 byMode + Tmt  (����Ƶģʽ + ����ƵԴ)
OSPEVENT(MCU_MCU_SPYFASTUPDATEPIC_CMD,	     EV_MCUMCU_KDV_BGN + 9 );

//�Ǳ���Ϣ��û�пռ��ˣ��ڼ�����ش������
//�缶ȡ�������˵ķǱ���Ϣ����Ϣ�壺TMt
OSPEVENT(MCU_MCU_CANCELMESPEAKER_REQ,       EV_MCUMCU_KDV_BGN + 10 );
OSPEVENT(MCU_MCU_CANCELMESPEAKER_ACK,       EV_MCUMCU_KDV_BGN + 11 );
OSPEVENT(MCU_MCU_CANCELMESPEAKER_NACK,      EV_MCUMCU_KDV_BGN + 12 );

//�������������Ǳ���Ϣ����Ϣ�壺TMt + u8(byMuteOpenFlag �������ã�1 ȡ����0) + u8(byMuteType ������1 ������0)
OSPEVENT(MCU_MCU_MTAUDMUTE_REQ,       EV_MCUMCU_KDV_BGN + 13 );
OSPEVENT(MCU_MCU_MTAUDMUTE_ACK,       EV_MCUMCU_KDV_BGN + 14 );
OSPEVENT(MCU_MCU_MTAUDMUTE_NACK,      EV_MCUMCU_KDV_BGN + 15 );

//���������ش�RTCP��Ϣ��֪��Ϣ����Ϣ�壺
OSPEVENT(MCU_MCU_MTEXTINFO_NOTIF,     EV_MCUMCU_KDV_BGN+16);

//�����¼����������ϼ�MCUʧ�ܵĸ�֪��Ϣ����Ϣ�壺
OSPEVENT(MCU_MCU_SWITCHTOMCUFAIL_NOTIF,     EV_MCUMCU_KDV_BGN+17);

//�����¼������������Ϣ�壺TMsgHeadMsg + TMt + u8 bIsRecover + u16 wBitRate
OSPEVENT(MCU_MCU_ADJMTBITRATE_CMD,     EV_MCUMCU_KDV_BGN + 18);
// End

/************************************************************************/
/* �Զ��� ģ��ע����Ϣ                                                  */
/************************************************************************/

//�ն�����ģ�鷢��MCU��OSP����
OSPEVENT(MTADP_MCU_CONNECT,			        EV_MCUMT_KDV_BGN + 1 );

//�ն�����ģ���ڲ�����mcu�Ķ�ʱ��ID �� Ϊ��ֹevent id��ͻ��ͳһ�����ڴ˴�
OSPEVENT(MTADP_MCU_CONNECT2,		        EV_MCUMT_KDV_BGN + 2 );
//�ն�����ģ���ڲ�ע��mcu�Ķ�ʱ��ID�� Ϊ��ֹevent id��ͻ��ͳһ�����ڴ˴�
OSPEVENT(MTADP_MCU_REGISTER_REQ2,	        EV_MCUMT_KDV_BGN + 4 );

// MCU ֪ͨ�Ự ����MPCB ��Ӧʵ��, zgc, 2007/04/30 
OSPEVENT(MCU_MTADP_RECONNECTB_CMD,			EV_MCUMT_KDV_BGN + 3 );

//�ն�����ģ�鷢��MCU��ע������
OSPEVENT(MTADP_MCU_REGISTER_REQ,	        EV_MCUMT_KDV_BGN + 5 );
//MCU�����ն�����ģ���ע����Ӧ
OSPEVENT(MCU_MTADP_REGISTER_ACK,	        EV_MCUMT_KDV_BGN + 6 );
OSPEVENT(MCU_MTADP_REGISTER_NACK,	        EV_MCUMT_KDV_BGN + 7 );

//�ն˺���ʧ��֪ͨ
OSPEVENT(MT_MCU_CALLMTFAILURE_NOTIFY,	    EV_MCUMT_KDV_BGN + 8 );

/************************************************************************/
/* �Զ��� MCU���߸���GK��ַ����		                                            */
/************************************************************************/
//MCU���߸���GK��ַ, ��Ϣ��: ��
OSPEVENT( MCU_MTADP_GKADDR_UPDATE_CMD,		EV_MCUMT_KDV_BGN + 9 );

//��ϯ�ն�ǿ�ƹ㲥����Ϣ�壺1����ʼ��0��ֹͣ
OSPEVENT( MT_MCU_VIEWBRAODCASTINGSRC_CMD,   EV_MCUMT_KDV_BGN + 10 );

/************************************************************************/
/* �Զ��� �����ӳ������ͨ��                                            */
/************************************************************************/
//��Ϣ��Ϊu16 ����
OSPEVENT( MT_MCU_DELAYCONF_REQ,			    EV_MCUMT_KDV_BGN + 12 );
OSPEVENT( MCU_MT_DELAYCONF_ACK,			    EV_MCUMT_KDV_BGN + 13 );
OSPEVENT( MCU_MT_DELAYCONF_NACK,		    EV_MCUMT_KDV_BGN + 14 );
//��Ϣ��Ϊu16 ����
OSPEVENT( MCU_MT_DELAYCONF_NOTIF,		    EV_MCUMT_KDV_BGN + 15 );
OSPEVENT( MCU_MT_CONFWILLEND_NOTIF, 	    EV_MCUMT_KDV_BGN + 17 );

//�����ѯmcu״̬����Ϣ�壺��
OSPEVENT( MTADP_MCU_GETMSSTATUS_REQ,        EV_MCUMT_KDV_BGN + 18 );
//��Ϣ�壺TMcuMsStatus
OSPEVENT( MCU_MTADP_GETMSSTATUS_ACK,        EV_MCUMT_KDV_BGN + 19 );

/************************************************************************/
/* �Զ��� ��ѯͨ��                                                      */
/************************************************************************/

//�ն˷���MCU�Ĳ�ѯ������ĳ���ն�״̬����MT->MCU����Ϣ��Ϊ�ṹTMt
OSPEVENT( MT_MCU_GETMTSTATUS_REQ,		    EV_MCUMT_KDV_BGN + 20 );
//MCU�صĳɹ�Ӧ��MCU->MT����Ϣ�壺TMtStatus
OSPEVENT( MCU_MT_GETMTSTATUS_ACK,		    EV_MCUMT_KDV_BGN + 21 );
//MCU�ص�ʧ��Ӧ��MCU->MT����Ϣ��Ϊ�ṹTMt
OSPEVENT( MCU_MT_GETMTSTATUS_NACK,		    EV_MCUMT_KDV_BGN + 22 );

//MCU�����ն˲�ѯ״̬����MCU->MT������Ϣ��
OSPEVENT( MCU_MT_GETMTSTATUS_REQ,	        EV_MCUMT_KDV_BGN + 25 );
//MT����Ӧ��MT->MCU����Ϣ��Ϊ�ṹTMtStatus
OSPEVENT( MT_MCU_GETMTSTATUS_ACK,	        EV_MCUMT_KDV_BGN + 26 );
//MT�ܾ�Ӧ��MT->MCU����Ϣ��Ϊ�ṹTMt
OSPEVENT( MT_MCU_GETMTSTATUS_NACK,	        EV_MCUMT_KDV_BGN + 27 );
//MT״̬�ı�ʱ��������MCU״̬��Ϣ����Ϣ��Ϊ�ṹTMtStatus
OSPEVENT( MT_MCU_MTSTATUS_NOTIF,	        EV_MCUMT_KDV_BGN + 30 );


//MCU�����ն˲�ѯ�ն˰汾��Ϣ����MCU->MT,����Ϣ��
OSPEVENT( MCU_MT_GETMTVERID_REQ,            EV_MCUMT_KDV_BGN + 31 );
//MT����Ӧ��MT->MCU����Ϣ�� TMtExt2
OSPEVENT( MT_MCU_GETMTVERID_ACK,            EV_MCUMT_KDV_BGN + 32 );
//MT�ܾ�����Ӧ��MT->MCU, NULL
OSPEVENT( MT_MCU_GETMTVERID_NACK,           EV_MCUMT_KDV_BGN + 33 );

//�ն˷���MCU�Ĳ�ѯ����������Ϣ����MT->MCU
OSPEVENT( MT_MCU_GETCONFINFO_REQ,		    EV_MCUMT_KDV_BGN + 35 );
//MCU�صĳɹ�Ӧ��MCU->MT����Ϣ��Ϊ�ṹTConfFullInfo
OSPEVENT( MCU_MT_GETCONFINFO_ACK,		    EV_MCUMT_KDV_BGN + 36 );
//MCU�ص�ʧ��Ӧ��MCU->MT����Ϣ��Ϊ�ṹTMt
OSPEVENT( MCU_MT_GETCONFINFO_NACK,		    EV_MCUMT_KDV_BGN + 37 );

//������Ϣ֪ͨ��MCU->MT����Ϣ��Ϊ�ṹTConfFullInfo
OSPEVENT( MCU_MT_CONF_NOTIF,			    EV_MCUMT_KDV_BGN + 40 );

//��֪ͨ�ն�ʹ�õĻ�����Ϣ֪ͨ��MCU->MT����Ϣ��Ϊ�ṹTSimConfInfo 2005-10-11
OSPEVENT( MCU_MT_SIMPLECONF_NOTIF,	        EV_MCUMT_KDV_BGN + 41 );

//���ն˽������ӳɹ�֪ͨ��1 BYTE�� ���̱�� + u8 + s8*(versionId) + u8 + s8*(ProductId)
OSPEVENT( MT_MCU_MTCONNECTED_NOTIF,	        EV_MCUMT_KDV_BGN + 42 );
//�ն˳ɹ����֪ͨ, 1byte(1-master,0-slave)
OSPEVENT( MT_MCU_MTJOINCONF_NOTIF,	        EV_MCUMT_KDV_BGN + 44 );

//�ն�����֪ͨ��320����ʱ�Ķ���֪ͨ��, 1byte(MT_TYPE_MT��TYPE_MCU)
OSPEVENT( MT_MCU_MTTYPE_NOTIF,	            EV_MCUMT_KDV_BGN + 45 );

//���ն����ӶϿ�֪ͨ��1 Byte. ����ԭ��
//param: emDisconnectReason
OSPEVENT( MT_MCU_MTDISCONNECTED_NOTIF,	    EV_MCUMT_KDV_BGN + 46 );

/************************************************************************/
/* �Զ��� 	��ϯ����                                                    */
/************************************************************************/
//MCU֪ͨ�ն˷����˸ı�,��Ϣ��: TMt(�·�����)
OSPEVENT( MCU_MT_CHANGESPEAKER_NOTIF,		EV_MCUMT_KDV_BGN + 48 );
//MCU֪ͨ�ն���ϯ�ı�,��Ϣ��: TMt(����ϯ)
OSPEVENT( MCU_MT_CHANGECHAIRMAN_NOTIF,		EV_MCUMT_KDV_BGN + 50 );

//��ϯ�ն˷���MCU��ָ����ϯ����MT->MCU����Ϣ��Ϊ�ṹTMt����������ϯ
OSPEVENT( MT_MCU_SPECCHAIRMAN_REQ,		    EV_MCUMT_KDV_BGN + 55 );
//MCU�صĳɹ�Ӧ��MCU->MT������Ϣ��
OSPEVENT( MCU_MT_SPECCHAIRMAN_ACK,		    EV_MCUMT_KDV_BGN + 56 );
//MCU�ص�ʧ��Ӧ��MCU->MT������Ϣ��
OSPEVENT( MCU_MT_SPECCHAIRMAN_NACK,		    EV_MCUMT_KDV_BGN + 57 );

//֪ͨ��ϯ�ն�������ϯ����MCU->MT����Ϣ��Ϊ�ṹTMt
OSPEVENT( MCU_MT_APPLYCHAIRMAN_NOTIF,	    EV_MCUMT_KDV_BGN + 59 );

//MCU�����ն˵�ָ����ϯ֪ͨ��MCU->MT������Ϣ��
OSPEVENT( MCU_MT_SPECCHAIRMAN_NOTIF,	    EV_MCUMT_KDV_BGN + 61 );
/************************************************************************/
/* �Զ��� �ն�ѡ�����                                                  */
/************************************************************************/

//�ն˷���MCU�������ն�ѡ���Լ�������MT->MCU����Ϣ��Ϊ�ṹTSwitchInfo
OSPEVENT( MT_MCU_STARTMTSELME_REQ,		    EV_MCUMT_KDV_BGN + 65 );
//MCU�����ն˵������ն�ѡ���Լ�������Ӧ��MCU->MT����Ϣ��Ϊ�ṹTSwitchInfo
OSPEVENT( MCU_MT_STARTMTSELME_ACK,		    EV_MCUMT_KDV_BGN + 66 );
//MCU�����ն˵������ն�ѡ���Լ��ľܾ���MCU->MT����Ϣ��Ϊ�ṹTSwitchInfo
OSPEVENT( MCU_MT_STARTMTSELME_NACK,		    EV_MCUMT_KDV_BGN + 67 );
//�ն˷���MCU��ֹͣ�����ն�ѡ���Լ���֪ͨ��MT->MCU����Ϣ��Ϊ�ṹTSwitchInfo
OSPEVENT( MT_MCU_STOPMTSELME_CMD,	        EV_MCUMT_KDV_BGN + 70 );

//�ն˷���MCU������MCU�鲥�����ն˵�����MT->MCU����Ϣ��Ϊ�ṹTMt(���鲥���ն�)
OSPEVENT( MT_MCU_STARTBROADCASTMT_REQ,	    EV_MCUMT_KDV_BGN + 75 );

//MCU�����ն˵��鲥�����ն˵���Ӧ��MCU->Mt����Ϣ��Ϊ
//TMt(���鲥���ն�)+TTransportAddr(��Ƶ���鲥��ַ)+TTransportAddr(��Ƶ���鲥��ַ)
//				   +TMediaEncrypt(��Ƶ�����ִ�)   +TDoublePayload(��Ƶ��̬�غ�)
//				   +TMediaEncrypt(��Ƶ�����ִ�)   +TDoublePayload(��Ƶ��̬�غ�)
OSPEVENT( MCU_MT_STARTBROADCASTMT_ACK,	    EV_MCUMT_KDV_BGN + 76 );

//MCU�����ն˵��鲥�����ն˵���Ӧ��MCU->Mt, ��Ϣ��ΪTMt(���鲥���ն�)
OSPEVENT( MCU_MT_STARTBROADCASTMT_NACK,	    EV_MCUMT_KDV_BGN + 77 );

//�ն˷���MCU��ֹͣMCU�鲥�����ն˵����MT->MCU����Ϣ��Ϊ�ṹTMt(�����ն�)
OSPEVENT( MT_MCU_STOPBROADCASTMT_CMD,	    EV_MCUMT_KDV_BGN + 80 );

//�ն˷���MCU�Ĳ�ѯ�����ն�ѡ�����������MT->MCU����Ϣ��Ϊ�ṹTMt(�����ն�)
OSPEVENT( MT_MCU_GETMTSELSTUTS_REQ,		    EV_MCUMT_KDV_BGN + 85 );
//MCU�����ն˵Ĳ�ѯ�����ն�ѡ�������Ӧ��MCU->MT����Ϣ��Ϊ�ṹTMt(�����ն�)+TMt(ѡ���ն�)
//TMt(ѡ���ն�) ��ȫ0��ʾδѡ��
OSPEVENT( MCU_MT_GETMTSELSTUTS_ACK,		    EV_MCUMT_KDV_BGN + 86 );
//MCU�����ն˵Ĳ�ѯ�����ն�ѡ������ĵľܾ���MCU->MT����Ϣ��Ϊ�ṹTMt(�����ն�)+TMt(ȫ0)
OSPEVENT( MCU_MT_GETMTSELSTUTS_NACK,	    EV_MCUMT_KDV_BGN + 87 );
//�ն˺���ʧ��֪ͨ(��HDI��������������)
OSPEVENT(MT_MCU_CALLFAIL_HDIFULL_NOTIF,		EV_MCUMT_KDV_BGN + 88 );

/************************************************************************/
/* �Զ��� �ն���ƵԴ������ʾ                                            */
/************************************************************************/
// ��ȡ�ն���ƵԴ��������Ϣ��: ��
OSPEVENT( MCU_MT_GETMTVIDEOALIAS_CMD,		EV_MCUMT_KDV_BGN + 89 );
// �ϱ��ն���ƵԴ��������Ϣ��: s8 achVidALias[MT_MAXNUM_VIDSOURCE][MT_MAX_PORTNAME_LEN]
OSPEVENT( MT_MCU_MTVIDEOALIAS_NOTIF,		EV_MCUMT_KDV_BGN + 90 );


/************************************************************************/
/* �Զ��� ���þ�����Զ�˾�����                                          */
/************************************************************************/

//��ϯ�ն�����MCU�����ն˾������ã�MT->MCU����Ϣ��Ϊ�ṹTMt+1�ֽڣ�
//1��MUTE��0��NOTMUTE
OSPEVENT( MT_MCU_MTMUTE_CMD,			    EV_MCUMT_KDV_BGN + 100 );

//MCU�����ն�ĳһ����ʵ�����ն˾������ã�MCU->MT����Ϣ��Ϊ�ṹTMt+1�ֽڣ�
//1��MUTE��0��NOTMUTE
OSPEVENT( MCU_MT_MTMUTE_CMD,			    EV_MCUMT_KDV_BGN + 105 );

/************************************************************************/
/* �Զ��� �������������˾�����                                          */
/************************************************************************/

//��ϯ�ն�����MCU�����ն��������ã�MT->MCU����Ϣ��Ϊ�ṹTMt+1�ֽڣ�
//1��MUTE��2��NOTMUTE
OSPEVENT( MT_MCU_MTDUMB_CMD,			    EV_MCUMT_KDV_BGN + 110 );

//MCU�����ն�ĳһ����ʵ�����ն˽����������ã�MCU->MT����Ϣ��Ϊ�ṹTMt+1�ֽڣ�
//1��MUTE��2��NOTMUTE
OSPEVENT( MCU_MT_MTDUMB_CMD,			    EV_MCUMT_KDV_BGN + 115 );

//�����ն˵�ѡ����ƵԴ���MCU->MT (MCU->SMCU)����Ϣ��Ϊ�ṹTMt+1�ֽڣ�
//OSPEVENT( MCU_MT_SETMTVIDSRC_CMD,	EV_MCUMT_KDV_BGN + 120 );

/************************************************************************/
/*  �Զ��� ������������                                                 */
/************************************************************************/

//���������������Ʒ���

   //��ϯ����MCU��ʼ�����������Ʒ���, ��Ϣ�壺  ��
   OSPEVENT( MT_MCU_STARTVAC_REQ ,          EV_MCUMT_KDV_BGN + 130 );
   //MCUͬ����ϯ��ʼ�����������Ʒ��Ե�����, ��Ϣ�壺  ��
   OSPEVENT( MCU_MT_STARTVAC_ACK,           EV_MCUMT_KDV_BGN + 131 );
   //MCU�ܾ���ϯ��ʼ�����������Ʒ��Ե�����, ��Ϣ�壺  ��
   OSPEVENT( MCU_MT_STARTVAC_NACK,          EV_MCUMT_KDV_BGN + 132 );
   //MCU����ϯ��ʼ�����������Ʒ��Ե�֪ͨ, ��Ϣ�壺  ��
   OSPEVENT( MCU_MT_STARTVAC_NOTIF,         EV_MCUMT_KDV_BGN + 135 );

//ȡ�������������Ʒ���
   
   //��ϯ����MCUֹͣ�����������Ʒ���, ��Ϣ�壺  ��
   OSPEVENT( MT_MCU_STOPVAC_REQ ,           EV_MCUMT_KDV_BGN + 140 );
   //MCUͬ����ϯֹͣ�����������Ʒ��Ե�����, ��Ϣ�壺  ��
   OSPEVENT( MCU_MT_STOPVAC_ACK,            EV_MCUMT_KDV_BGN + 141 );
   //MCU�ܾ���ϯֹͣ�����������Ʒ��Ե�����, ��Ϣ�壺  ��
   OSPEVENT( MCU_MT_STOPVAC_NACK,           EV_MCUMT_KDV_BGN + 142 );
   //MCU����ϯֹͣ�����������Ʒ��Ե�֪ͨ, ��Ϣ�壺  ��
   OSPEVENT( MCU_MT_STOPVAC_NOTIF,          EV_MCUMT_KDV_BGN + 150 );

/************************************************************************/
/*  �Զ��� ��������                                                     */
/************************************************************************/

//��ʼ����
   
    //��ʼ������������, ��Ϣ�壺  1byte(ͬʱ�μ�����(����)�ĳ�Ա����) + ����������ƻ�����׷��TMt����
	OSPEVENT( MT_MCU_STARTDISCUSS_REQ,		EV_MCUMT_KDV_BGN + 160 );
	//MCUͬ�⿪ʼ��������, ��Ϣ�壺  1byte(ͬʱ�μ�����(����)�ĳ�Ա����)
	OSPEVENT( MCU_MT_STARTDISCUSS_ACK,		EV_MCUMT_KDV_BGN + 161 );
	//MCU�ܾ���ʼ��������, ��Ϣ�壺  1byte(ͬʱ�μ�����(����)�ĳ�Ա����)
	OSPEVENT( MCU_MT_STARTDISCUSS_NACK,		EV_MCUMT_KDV_BGN + 162 );
	//MCU��ʼ��������֪ͨ, ��Ϣ�壺  1byte(ͬʱ�μ�����(����)�ĳ�Ա����)
	OSPEVENT( MCU_MT_STARTDISCUSS_NOTIF,	EV_MCUMT_KDV_BGN + 165 );

//��������

	//��ϯ����MCU������������, ��Ϣ�壺  ��
	OSPEVENT( MT_MCU_STOPDISCUSS_REQ,       EV_MCUMT_KDV_BGN + 167 );
	//MCUͬ����ϯ�����������۵�����, ��Ϣ�壺 ��
	OSPEVENT( MCU_MT_STOPDISCUSS_ACK,       EV_MCUMT_KDV_BGN + 168 );
	//MCU�ܾ���ϯ�����������۵�����, ��Ϣ�壺  ��
	OSPEVENT( MCU_MT_STOPDISCUSS_NACK,      EV_MCUMT_KDV_BGN + 169 );
	//MCU����ϯ�����������۵�֪ͨ, ��Ϣ�壺  ��
	OSPEVENT( MCU_MT_STOPDISCUSS_NOTIF,	    EV_MCUMT_KDV_BGN + 175 );

/************************************************************************/
/*  �Զ��� ��Ƶ����                                                     */
/************************************************************************/
//��ʼ��Ƶ����

	//��ʼ��Ƶ��������, ��Ϣ�壺  TVMPParam
	OSPEVENT( MT_MCU_STARTVMP_REQ,          EV_MCUMT_KDV_BGN + 180 );
	//MCUͬ����Ƶ��������, ��Ϣ�壺  TVMPParam
	OSPEVENT( MCU_MT_STARTVMP_ACK,          EV_MCUMT_KDV_BGN + 181 );
	//MCU��ͬ����Ƶ��������, ��Ϣ�壺  ��
	OSPEVENT( MCU_MT_STARTVMP_NACK,         EV_MCUMT_KDV_BGN + 182 );
	//MCU��ʼ��Ƶ���ϵ�֪ͨ, ��Ϣ�壺  TVMPParam
	OSPEVENT( MCU_MT_STARTVMP_NOTIF,        EV_MCUMT_KDV_BGN + 185 );

//ֹͣ��Ƶ����	

	//������Ƶ��������, ��Ϣ�壺  
	OSPEVENT( MT_MCU_STOPVMP_REQ,		    EV_MCUMT_KDV_BGN + 187 );
	//MCUͬ����Ƶ������������, ��Ϣ�壺  ��
	OSPEVENT( MCU_MT_STOPVMP_ACK,		    EV_MCUMT_KDV_BGN + 188 );
	//MCU��ͬ�������Ƶ��������, ��Ϣ�壺  ��
	OSPEVENT( MCU_MT_STOPVMP_NACK,		    EV_MCUMT_KDV_BGN +  189 );
	//MCU������Ƶ���ϵ�֪ͨ, ��Ϣ�壺  ��
	OSPEVENT( MCU_MT_STOPVMP_NOTIF,		    EV_MCUMT_KDV_BGN +  191 );
	
//�ı���Ƶ���ϲ���

	//��ϯ����MCU�ı���Ƶ���ϲ���, ��Ϣ�壺  TVMPParam
	OSPEVENT( MT_MCU_CHANGEVMPPARAM_REQ,	EV_MCUMT_KDV_BGN + 193 );
	//MCUͬ����ϯ�ĸı���Ƶ���ϲ�������, ��Ϣ�壺 TVMPParam
	OSPEVENT( MCU_MT_CHANGEVMPPARAM_ACK,	EV_MCUMT_KDV_BGN + 194 );
	//MCU�ܾ���ϯ�ĸı���Ƶ���ϲ�������, ��Ϣ�壺  ��
	OSPEVENT( MCU_MT_CHANGEVMPPARAM_NACK,	EV_MCUMT_KDV_BGN + 195 );

//��ѯ��Ƶ���ϲ���	

	//��ѯ��Ƶ���ϳ�Ա����, ��Ϣ�壺  �� 
	OSPEVENT( MT_MCU_GETVMPPARAM_REQ,	    EV_MCUMT_KDV_BGN + 197 );
	//MCUͬ���ѯ��Ƶ���ϳ�Ա, ��Ϣ�壺  TVMPParam
	OSPEVENT( MCU_MT_GETVMPPARAM_ACK,	    EV_MCUMT_KDV_BGN + 198 );
	//MCU��ͬ���ѯ��Ƶ���ϳ�Ա, ��Ϣ�壺  ��
	OSPEVENT( MCU_MT_GETVMPPARAM_NACK,	    EV_MCUMT_KDV_BGN + 199 );
	//MCU����ϯ����Ƶ���ϲ���֪ͨ, ��Ϣ�壺  TVMPParam
	OSPEVENT( MCU_MT_VMPPARAM_NOTIF,	    EV_MCUMT_KDV_BGN + 201 );

//��ʼ�㲥������Ƶ��
	//��ϯ����MCU�㲥��Ƶ��������, ��Ϣ����  
	OSPEVENT( MT_MCU_STARTVMPBRDST_REQ,		EV_MCUMT_KDV_BGN + 210 );
	//MCUͬ��㲥��Ƶ��������, ��Ϣ����
	OSPEVENT( MCU_MT_STARTVMPBRDST_ACK,		EV_MCUMT_KDV_BGN + 211 );
	//MCU�ܾ��㲥��Ƶ��������, ��Ϣ����
	OSPEVENT( MCU_MT_STARTVMPBRDST_NACK,	EV_MCUMT_KDV_BGN + 212 );

//ֹͣ�㲥������Ƶ��
	//��ϯ����MCUֹͣ�㲥��Ƶ��������, ��Ϣ����  
	OSPEVENT( MT_MCU_STOPVMPBRDST_REQ,		EV_MCUMT_KDV_BGN + 215 );
	//MCUͬ��ֹͣ�㲥��Ƶ��������, ��Ϣ����
	OSPEVENT( MCU_MT_STOPVMPBRDST_ACK,		EV_MCUMT_KDV_BGN + 216 );
	//MCU�ܾ�ֹͣ�㲥��Ƶ��������, ��Ϣ����
	OSPEVENT( MCU_MT_STOPVMPBRDST_NACK,	    EV_MCUMT_KDV_BGN + 217 );
	//��ϯѡ��VMP ���� ��Ϣ�壺 ��
	OSPEVENT( MT_MCU_STARTSWITCHVMPMT_REQ,	EV_MCUMT_KDV_BGN + 218 );
	//��ϯѡ��VMPӦ��, ��Ϣ�壺 ��
	OSPEVENT( MCU_MT_STARTSWITCHVMPMT_ACK,  EV_MCUMT_KDV_BGN + 219 );
	OSPEVENT( MCU_MT_STARTSWITCHVMPMT_NACK, EV_MCUMT_KDV_BGN + 220 );

/************************************************************************/
/*  �Զ���  �ն���ѯѡ��                                                */
/************************************************************************/

//�ն���ѯѡ������

   //MCU������ն˿�ʼ��ѯѡ��, ��Ϣ�壺  TMt +TPollInfo + TMtPollParam����
   OSPEVENT( MCU_MT_STARTMTPOLL_CMD,        EV_MCUMT_KDV_BGN + 221 ); 
   //MCU������ն�ֹͣ��ѯѡ��, ��Ϣ�壺  TMt
   OSPEVENT( MCU_MT_STOPMTPOLL_CMD,         EV_MCUMT_KDV_BGN + 223 );
   //MCU������ն���ͣ��ѯѡ��, ��Ϣ�壺  TMt
   OSPEVENT( MCU_MT_PAUSEMTPOLL_CMD,        EV_MCUMT_KDV_BGN + 225 );
   //MCU������ն˼�����ѯѡ��, ��Ϣ�壺  TMt
   OSPEVENT( MCU_MT_RESUMEMTPOLL_CMD,       EV_MCUMT_KDV_BGN + 227 );

//��ѯ�ն���ѯѡ������

   //MCU���ն˲�ѯ��ѯ����, ��Ϣ�壺  TMt 
   OSPEVENT( MCU_MT_GETMTPOLLPARAM_REQ,     EV_MCUMT_KDV_BGN + 230 );
   //MtӦ��MCU��ѯ�ն���ѯ����, ��Ϣ�壺  TMt + TPollInfo + TMtPollParam����
   OSPEVENT( MT_MCU_GETMTPOLLPARAM_ACK,     EV_MCUMT_KDV_BGN + 231 ); 
   //MT�ܾ�MCU��ѯ�ն���ѯ����, ��Ϣ�壺  TMt
   OSPEVENT( MT_MCU_GETMTPOLLPARAM_NACK,    EV_MCUMT_KDV_BGN + 232 );
   //MT֪ͨMCU��ǰ���ն���ѯ״̬, ��Ϣ�壺  TMt + TPollInfo + TMtPollParam����
   OSPEVENT( MT_MCU_POLLMTSTATE_NOTIF,      EV_MCUMT_KDV_BGN + 235 );


/************************************************************************/
/* �Զ��� ���þ���(��Ƶ)                                                    */
/************************************************************************/
  
//���о��󷽰�����  MC->ter, ��Ϣ�壺��
OSPEVENT( MCU_MT_MATRIX_GETALLSCHEMES_CMD,  EV_MCUMT_KDV_BGN + 237 );
//���о��󷽰���Ϣ֪ͨ����Ϣ��Ϊ��u8( ������ ) + ITInnerMatrixScheme[]
OSPEVENT( MT_MCU_MATRIX_ALLSCHEMES_NOTIF,   EV_MCUMT_KDV_BGN + 240 );

//�������󷽰�����  MC->ter, ��Ϣ�壺u8 ( 0 ΪĬ�Ϸ��� )       ����������( 0 - 6 )
OSPEVENT( MCU_MT_MATRIX_GETONESCHEME_CMD,   EV_MCUMT_KDV_BGN + 241 );
//��Ϣ�壺ITInnerMatrixScheme
OSPEVENT( MT_MCU_MATRIX_ONESCHEME_NOTIF,    EV_MCUMT_KDV_BGN + 245 );

//������󷽰�����,��Ϣ��:ITInnerMatrixScheme
OSPEVENT( MCU_MT_MATRIX_SAVESCHEME_CMD,		EV_MCUMT_KDV_BGN + 246 );
//��Ϣ�壺ITInnerMatrixScheme
OSPEVENT( MT_MCU_MATRIX_SAVESCHEME_NOTIF,   EV_MCUMT_KDV_BGN + 249 );

//ָ����ǰ����,��Ϣ�壺u8  ��������
OSPEVENT( MCU_MT_MATRIX_SETCURSCHEME_CMD,   EV_MCUMT_KDV_BGN + 250 );
//��Ϣ�壺u8  ��������
OSPEVENT( MT_MCU_MATRIX_SETCURSCHEME_NOTIF, EV_MCUMT_KDV_BGN + 253 );

//��ȡ��ǰ����,��Ϣ�壺��
OSPEVENT( MCU_MT_MATRIX_GETCURSCHEME_CMD,   EV_MCUMT_KDV_BGN + 254 );
//��Ϣ�壺u8  ��������
OSPEVENT( MT_MCU_MATRIX_CURSCHEME_NOTIF,    EV_MCUMT_KDV_BGN + 257 );

/************************************************************************/
/* ���þ���                                                             */
/************************************************************************/

//��ȡ�ն����þ������ͣ���Ϣ�壺��
OSPEVENT( MCU_MT_EXMATRIX_GETINFO_CMD,      EV_MCUMT_KDV_BGN + 260 );
//�ն����þ�������ָʾ, ��Ϣ�壺u8 ����������˿��� 0��ʾû�о���
OSPEVENT( MT_MCU_EXMATRIXINFO_NOTIFY,       EV_MCUMT_KDV_BGN + 263 );

//�������þ������Ӷ˿ں�,��Ϣ�壺u8  (1-64 )
OSPEVENT( MCU_MT_EXMATRIX_SETPORT_CMD,      EV_MCUMT_KDV_BGN + 264 );

//�����ȡ���þ������Ӷ˿�,��Ϣ�壺��
OSPEVENT( MCU_MT_EXMATRIX_GETPORT_REQ,      EV_MCUMT_KDV_BGN + 268 );
//��Ϣ�壺��
OSPEVENT( MT_MCU_EXMATRIX_GETPORT_ACK,      EV_MCUMT_KDV_BGN + 269 );
//��Ϣ�壺��
OSPEVENT( MT_MCU_EXMATRIX_GETPORT_NACK,     EV_MCUMT_KDV_BGN + 270 );
//��Ϣ�壺u8  (1-64 )
OSPEVENT( MT_MCU_EXMATRIX_GETPORT_NOTIF,    EV_MCUMT_KDV_BGN + 271 );

//�������þ������Ӷ˿���,��Ϣ�壺u8  (1-64 )+s8[MAXLEN_MATRIXPORTNAME] 0��β���ַ��� 
OSPEVENT( MCU_MT_EXMATRIX_SETPORTNAME_CMD,  EV_MCUMT_KDV_BGN + 272 );
//���þ���˿���ָʾ, ��Ϣ�壺u8  (1-64 )+s8[MAXLEN_MATRIXPORTNAME] 0��β���ַ��� 
OSPEVENT( MT_MCU_EXMATRIX_PORTNAME_NOTIF,   EV_MCUMT_KDV_BGN + 275 );

//�����ȡ���þ�������ж˿���,��Ϣ�壺��
OSPEVENT( MCU_MT_EXMATRIX_GETALLPORTNAME_CMD,   EV_MCUMT_KDV_BGN + 276 );
//���þ�������ж˿���ָʾ����Ϣ�壺s8[][MAXLEN_MATRIXPORTNAME] ����0 ��ʾ����˿��� ����Ϊ����˿ڣ����ַ������飩
OSPEVENT( MT_MCU_EXMATRIX_ALLPORTNAME_NOTIF,    EV_MCUMT_KDV_BGN + 279 );


/************************************************************************/
/* �Զ��� ������Ϣ                                                      */
/************************************************************************/
//��ϯ�ն˷���MCU�������ն�����MT->MCU����Ϣ��Ϊ�ṹTMcu+TAddMtInfo

OSPEVENT( MT_MCU_ADDMT_REQ,				    EV_MCUMT_KDV_BGN + 280 );
//MCU�صĳɹ�Ӧ��MCU->MT����Ϣ��Ϊ�ṹTMtStatus
OSPEVENT( MCU_MT_ADDMT_ACK,				    EV_MCUMT_KDV_BGN + 281 );
//MCU�ص�ʧ��Ӧ��MCU->MT������Ϣ��
OSPEVENT( MCU_MT_ADDMT_NACK,			    EV_MCUMT_KDV_BGN + 282 );

//���ն˶���Ϣ֪ͨ��MCU->MT ��Ϣ�壺TMt(�ն˺�Ϊ0�㲥�������ն�)+TROLLMSG(��Ч����)
OSPEVENT( MCU_MT_SENDMSG_NOTIF,			    EV_MCUMT_KDV_BGN + 285 );

//�ն����ն˶���Ϣ���MT->MCU
//��Ϣ�壺2 byte(�ն���,������,ֵΪN,0Ϊ�㲥�������ն�) + N��TMt +CRollMsg(��Ч����)
OSPEVENT( MT_MCU_SENDMSG_CMD,			    EV_MCUMT_KDV_BGN + 287 );

//�ն����������̨����Ϣ���MT->MCU����Ϣ��Ϊ�ṹTMt+CRollMsg����Ч���ȣ�
OSPEVENT( MT_MCU_SENDMCMSG_CMD,			    EV_MCUMT_KDV_BGN + 289 );

//�ն���ƵԴ����ָʾ    MC->ter,��Ϣ�����ַ�����'\0'����, ��󳤶�1024�ֽ�
OSPEVENT(MCU_MT_MTSOURCENAME_NOTIF,	        EV_MCUMT_KDV_BGN + 292 );
//��ϯ�Ƴ������ն�  MT->MCU, TMt
OSPEVENT(MT_MCU_REMOVEMIXMEMBER_CMD,		EV_MCUMT_KDV_BGN + 295 );
//��ϯ���ӻ����նˣ�MT->MCU��TMt
OSPEVENT( MT_MCU_ADDMIXMEMBER_CMD,		    EV_MCUMT_KDV_BGN + 300 );
//�ն˷���MCU������μӻ�������MT->MCU����Ϣ��NULL
OSPEVENT( MT_MCU_APPLYMIX_NOTIF,	        EV_MCUMT_KDV_BGN + 305 ); 
//֪ͨ��ϯ�ն������ն�����μӻ�������MCU->MT����Ϣ��Ϊ�ṹTMt
OSPEVENT( MCU_MT_MTAPPLYMIX_NOTIF,	        EV_MCUMT_KDV_BGN + 310 ); 
//֪ͨ�ն��ѳɹ��μӻ�����MCU->MT����Ϣ��: 0-���ڲ廰 1-�ڲ廰
OSPEVENT( MCU_MT_MTADDMIX_NOTIF,	        EV_MCUMT_KDV_BGN + 315 );
//֪ͨ�ն˶��ƻ���״̬, MCU->MT, ��Ϣ�壺TMixParam	// [11/25/2010 xliang] add for T3, R3-full mt
OSPEVENT( MCU_MT_DISCUSSPARAM_NOTIF,        EV_MCUMT_KDV_BGN + 316 );
//����qos����Ϣ�壺TMcuQosCfgInfo
OSPEVENT(MCU_MT_SETQOS_CMD,                 EV_MCUMT_KDV_BGN + 320 );

//����Э�������������Ϣ,��Ϣ�壺TMtAdpCfg
OSPEVENT(MCU_MT_MTADPCFG_CMD,               EV_MCUMT_KDV_BGN + 325 );

//��ȡ�ն�����
OSPEVENT(MCU_MT_GETBITRATEINFO_REQ,         EV_MCUMT_KDV_BGN + 330 );
//��Ϣ�壺��
OSPEVENT(MT_MCU_GETBITRATEINFO_ACK,         EV_MCUMT_KDV_BGN + 331 );
OSPEVENT(MT_MCU_GETBITRATEINFO_NACK,        EV_MCUMT_KDV_BGN + 332 );
//��Ϣ�壺TMtBitrate
OSPEVENT(MT_MCU_GETBITRATEINFO_NOTIF,       EV_MCUMT_KDV_BGN + 333 );

//�ն˽��մ���֪ͨ, ��Ϣ�壺u16���մ��� + u16���ʹ���������Ƶ����Ƶ��
OSPEVENT(MT_MCU_BANDWIDTH_NOTIF,            EV_MCUMT_KDV_BGN + 334 );

//�ն���չ��ƵԴ
//��ȡ�ն���ƵԴ��Ϣ,��Ϣ�壺��
OSPEVENT( MCU_MT_GETVIDEOSOURCEINFO_CMD,    EV_MCUMT_KDV_BGN + 340 );
//�ն�ȫ����ƵԴָʾ����Ϣ�壺tagITVideoSourceInfo[]
OSPEVENT( MT_MCU_ALLVIDEOSOURCEINFO_NOTIF,  EV_MCUMT_KDV_BGN + 343 );

//�����ն���ƵԴ��Ϣ����Ϣ�壺tagITVideoSourceInfo
OSPEVENT( MCU_MT_SETVIDEOSOURCEINFO_CMD,    EV_MCUMT_KDV_BGN + 344 );
//�ն���ƵԴ��Ϣָʾ,��Ϣ�壺tagITVideoSourceInfo
OSPEVENT( MT_MCU_VIDEOSOURCEINFO_NOTIF,     EV_MCUMT_KDV_BGN + 347 );

//�����ն˵�ѡ����չ��ƵԴ����, ��Ϣ�壺u8
OSPEVENT( MCU_MT_SELECTEXVIDEOSRC_CMD,      EV_MCUMT_KDV_BGN + 348 );

//�����ն˵���Ƶ�ֱ��ʸı������(H264����VMPʱ����)����Ϣ�壺u8(��Ƶ�ֱ���) + u8 ͨ������(LOGCHL_VIDEO)
//OSPEVENT( MCU_MT_VIDEOFORMATCHANGE_CMD,     EV_MCUMT_KDV_BGN + 349 );
//xliang [080731] ������Ϣ�������¸���
//�����ն˵���Ƶ�ֱ��ʸı������,��Ϣ�壺u8 ͨ������ + u8 �ŵ�����(H264,MPEG4��) + u8 �ֱ������� + u8 ֡��
OSPEVENT( MCU_MT_VIDEOPARAMCHANGE_CMD,     EV_MCUMT_KDV_BGN + 349 );
// xliang [1/6/2009] �����ն˵Ļָ���Ƶ�ֱ�������,��Ϣ�壺��
OSPEVENT( MCU_MT_VIDEOPARAMRECOVER_CMD,		EV_MCUMT_KDV_BGN + 350); 
// [pengjie 2010/4/22] �����ն˵���Ƶ֡�ʸı������,��Ϣ�壺u8 ͨ������  + u8 ֡��
OSPEVENT( MCU_MT_VIDEOFPSCHANGE_CMD,		EV_MCUMT_KDV_BGN + 351);

//[5/4/2011 zhushengze]VCS���Ʒ����˷�˫��
OSPEVENT(MCU_MT_CHANGEMTSECVIDSEND_CMD,         EV_MCUMT_KDV_BGN + 352 );

//[2/23/2012 zhushengze]���桢�ն���Ϣ͸��
OSPEVENT(MCU_MT_TRANSPARENTMSG_NOTIFY,          EV_MCUMT_KDV_BGN + 353 );
OSPEVENT(MT_MCU_TRANSPARENTMSG_NOTIFY,          EV_MCUMT_KDV_BGN + 354 );

//���ǻ����ն˽��յ�ַ�仯�����Ϣ�壺u8(LOGCHL_VIDEO,...) + TTransportAddr(�ն˽���������ַ)
OSPEVENT( MCU_MT_SATDCONFCHGADDR_CMD,       EV_MCUMT_KDV_BGN + 356 );

//MUC->MT���ǻ����鲥��ַ֪ͨ����Ϣ�壺u32 ��һ·��Ƶ��ַ +   u16   ��һ·��Ƶ�˿�
//                                     u32 ��һ·��Ƶ��ַ +   u16   ��һ·��Ƶ�˿�
//                                     u32 �ڶ�·��Ƶ��ַ +   u16   �ڶ�·��Ƶ�˿�
// ������ַ�˿ھ�Ϊ������
//OSPEVENT( MCU_MT_SATDCONFADDR_NOTIF,        EV_MCUMT_KDV_BGN + 363 );

//�����ն˵ĵ����ն������������Ϣ�壺u8(�������� VOLUME_TYPE_IN ...) + u8(�����������)
OSPEVENT( MCU_MT_SETMTVOLUME_CMD,           EV_MCUMT_KDV_BGN + 357 );

//�����ն��Զ�����Ϣ ͨ��֪ͨ����Ϣ�壺Mcu2MtNtfMsgType
OSPEVENT( MCU_MT_SOMEOFFERRING_NOTIF,       EV_MCUMT_KDV_BGN + 358 );

// ��ռ�����Ƿ���ͷŸ��ն�    ��Ϣ��: s8[]������ռ�Ļ�������
OSPEVENT( MT_MCU_RELEASEMT_REQ,             EV_MCUMT_KDV_BGN + 359 );
OSPEVENT( MCU_MT_RELEASEMT_ACK,             EV_MCUMT_KDV_BGN + 360 );
OSPEVENT( MCU_MT_RELEASEMT_NACK,            EV_MCUMT_KDV_BGN + 361 );

//�ϼ�mcu�Ļ����������¼�����ʾ����(mcu����+�������ƻ��߻�������) ��Ϣ��:u8(��ʾ����)
OSPEVENT( MCU_MT_MMCUCONFNAMESHOWTYPE_CMD,	EV_MCUMT_KDV_BGN + 362 );

//mcu���������� ��Ϣ��:u8(��������)
OSPEVENT( MCU_MT_CHANGEADMINLEVEL_CMD,	EV_MCUMT_KDV_BGN + 363 );

// ֪ͨ�ն˵�ǰ�ķ���״̬����Ϣ�壺u32 / emMtSpeakerStatus
OSPEVENT( MCU_MT_MTSPEAKSTATUS_NTF,         EV_MCUMT_KDV_BGN + 364 );

// ֪ͨ�����ն˻��鵱ǰ�ķ���״̬����Ϣ�壺u32 / emConfSpeakMode
OSPEVENT( MCU_MT_CONFSPEAKMODE_NTF,         EV_MCUMT_KDV_BGN + 365 );

// �ն�ȡ���Լ����������룬��Ϣ�壺NULL
OSPEVENT( MT_MCU_APPLYCANCELSPEAKER_REQ,       EV_MCUMT_KDV_BGN + 367 );
OSPEVENT( MCU_MT_APPLYCANCELSPEAKER_ACK,       EV_MCUMT_KDV_BGN + 368 );
OSPEVENT( MCU_MT_APPLYCANCELSPEAKER_NACK,      EV_MCUMT_KDV_BGN + 369 );

/************************************************************************/
/* mt������Ϣ	                                                        */
/************************************************************************/
OSPEVENT( MCU_MT_DBG_FLOWCTRLCMD,			   EV_MCUMT_KDV_BGN + 380 );


/************************************************************************/
/* Э��ջ�ص���Ϣ                                                       */
/************************************************************************/
OSPEVENT( EV_RAD_RAS_NOTIF,             EV_MTADP_BGN );
OSPEVENT( EV_RAD_NEWCALL_NOTIF,         EV_MTADP_BGN + 1 );
OSPEVENT( EV_RAD_NEWCHAN_NOTIF,         EV_MTADP_BGN + 2 );
OSPEVENT( EV_RAD_CALLCTRL_NOTIF,        EV_MTADP_BGN + 3 );
OSPEVENT( EV_RAD_CHANCTRL_NOTIF,        EV_MTADP_BGN + 4 );
OSPEVENT( EV_RAD_CONFCTRL_NOTIF,        EV_MTADP_BGN + 5 );
OSPEVENT( EV_RAD_FECCCTRL_NOTIF,        EV_MTADP_BGN + 6 ); 
OSPEVENT( EV_RAD_MMCUCTRL_NOTIF,        EV_MTADP_BGN + 7 ); 

/************************************************************************/
/* MTADP�ڲ���Ϣ                                                        */
/************************************************************************/
OSPEVENT( WAIT_PASSWORD_TIMEOUT,		EV_MTADP_BGN + 9 ); //wait for password response
OSPEVENT( WAIT_RESPONSE_TIMEOUT,		EV_MTADP_BGN + 10 ); //wait for response of calling or message

OSPEVENT( TIMER_ROUNDTRIPDELAY_REQ,	    EV_MTADP_BGN + 11 );  //��·ά������ʱ��
//OSPEVENT( WAIT_ROUNDTRIPDELAY_RSP_TIMEOUT,     EV_MTADP_BGN + 12 ); //link maintenance timeout
OSPEVENT( TIMER_REGGK_REQ,			    EV_MTADP_BGN + 13 ); //ע��GK��ʱ��
OSPEVENT( TIMER_POLL_WATCHDOG,          EV_MTADP_BGN + 14 ); 
OSPEVENT( TIMER_IRR,					EV_MTADP_BGN + 15 ); //IRR��ʱ��
OSPEVENT( TIMER_VIDEOINFO,			    EV_MTADP_BGN + 16 ); //mcu����ʱ��������Ƶ��Ϣ��ʱ��
OSPEVENT( TIMER_AUDIOINFO,              EV_MTADP_BGN + 17 ); //mcu����ʱ��������Ƶ��Ϣ��ʱ��
OSPEVENT( TIMER_MTLIST,                 EV_MTADP_BGN + 18 ); //mcu����ʱ�������ն��б�ʱ��
OSPEVENT( CLEAR_INSTANCE,			    EV_MTADP_BGN + 19 ); //���ʵ������
OSPEVENT( TIMER_ROUNDTRIPDELAY_START,   EV_MTADP_BGN + 20 );
OSPEVENT( TIMER_GETMSSTATUS_REQ,        EV_MTADP_BGN + 21 ); //�ȴ���ȡ����״̬��ʱ��
OSPEVENT( TIMER_H323_POLL_EVENT,        EV_MTADP_BGN + 22 );
OSPEVENT( TIMER_CONFCHARGE_STATUS_NTF,	EV_MTADP_BGN + 23 ); //������ע��GK��, ����Ʒ�״̬���ʱ��
OSPEVENT( CALLOUT_NEXT_NOTIFY,          EV_MTADP_BGN + 24 ); //�������в��Ժ�����һ��MT֪ͨ

//h320 MTAPD �ڲ���Ϣ
OSPEVENT( EV_H320STACK_STATE_NOTIF,     EV_MTADP_BGN + 25 ); //H320Э��ջ״̬�ص�֪ͨ
OSPEVENT( EV_H320STACK_CMD_NOTIF,       EV_MTADP_BGN + 26 ); //h320Э��ջcmd�ص�֪ͨ
OSPEVENT( EV_H320STACK_FECC_NOTIF,      EV_MTADP_BGN + 27 ); //h320Э��ջԶң�ص�֪ͨ
OSPEVENT( EV_MTADP_SHOWSWITCH,          EV_MTADP_BGN + 28 ); //show switch info
OSPEVENT( EV_MTADP_SETSTACKLOG,         EV_MTADP_BGN + 29 ); //set stack log print level
OSPEVENT( EV_MTADP_VIDLOOPBACK,         EV_MTADP_BGN + 30 ); //video loop back no bch
OSPEVENT( EV_MTADP_BCHVIDLOOPBACK,      EV_MTADP_BGN + 31 ); //video loop back bch
OSPEVENT( TIMER_INVITEMT,               EV_MTADP_BGN + 32 ); //�����ն˳�ʱ��ʱ��
OSPEVENT( TIMER_LSDTOKEN_RELEASE,       EV_MTADP_BGN + 33 ); //�Ƿ��ͷ�Զң���ƶ�ʱ��

//H323 MTADP �ڲ���Ϣ(����)
OSPEVENT( TIMER_URQ_RSP,                EV_MTADP_BGN + 34 ); //URQ��������Ӧ�ȴ���ʱ
//RAS��Ϣ��ʱУ�飨��ֹGK���ݵĵ��µ�״̬�ȴ��������ϲ���ƣ�
OSPEVENT( TIMER_RAS_RSP,                EV_MTADP_BGN + 35 );
// [11/21/2011 liuxu] ��ʱ���¼���MtList����McuVc
OSPEVENT( TIMER_SEND_SMCU_MTLIST,       EV_MTADP_BGN + 36 );

//H225�����������ø�TIMER����Timer��ʱǰH245��������Ӧ�ÿ�ʼ����(Mtadp�յ�MCU_MT_CAPBILITYSET_NOTIF����).��ʱClearInst.
//��TIMER��Ҫ�������������������ն˽����У�������������ʱ��δ������������H225�������ɺ󣬱�������Mtadp���������
//H245�����Ĺ��̡������ն�δ���ߡ�����Mtadp�ڲ��������ý������е�H225��ɣ���δ������н�������ʱ��������������ٺ��и��ն�
//Mtadp���������ն��Ѿ�������ֻ��δ��ɽ������������ն˷���Ϣ�������ն���Զ�޷����ߡ�[9/6/2012 chendaiwei]
//OSPEVENT( TIMER_H245_CAPBILITY_NOTIFY_OVERTIME, EV_MTADP_BGN + 37 );

// vrs֧�֣�����¼������ȡ�б�����Ҫ��Timer�ȴ��ظ�������ʱ������Ҷ��ն�
OSPEVENT( TIMER_VRS_REC_RSP,       EV_MTADP_BGN + 38 );


/************************************************************************/
/*  ˽����Ϣ�����ڲ��ԣ�                                                */
/************************************************************************/

OSPEVENT(MCU_MTADP_PRIVATE_MSG,			EV_MTADP_BGN + 40 ); 

#define EXIT_INSTANCE                   0  //body: null
#define ENABLE_OUTPUT_REDIRECTED        1  //body: u8, debug level
#define DISABLE_OUTPUT_REDIRECTED       2  //body: null
#define CLEAR_ALL				        3  //body: null


OSPEVENT( MTADP_MCU_PRIVATE_MSG,		EV_MTADP_BGN + 41 ); 

#define MTADP_OUTPUT                    10
#define CALL_STATISTICS_NOTIF           11  //u32[8]

//����h320MtAdp���Ե���Ϣ
OSPEVENT( MCU_H320MTADP_PRIVATE_MSG,    EV_MTADP_BGN + 42 );
OSPEVENT( H320MTADP_MCU_PRIVATE_MSG,    EV_MTADP_BGN + 43 );
OSPEVENT( TIMER_STARTAFTERSTOPSMIX,     EV_MTADP_BGN + 44 );
#define CHAN_SEND_STATE                 20 // ͨ����������״̬(body��u8)
#define CHAN_OPENCLOSE_STATE            21 // ͨ���Ƿ�ر�״̬(body��u8)



/************************************************************************/
/* MCU���ǻ�����Ϣ����                                                  */
/************************************************************************/

OSPEVENT( MT_MCU_REG_REQ,               EV_MCUMT_SAT_BGN + 1);
OSPEVENT( MCU_MT_REG_ACK,               EV_MCUMT_SAT_BGN + 2);
OSPEVENT( MCU_MT_REG_NACK,              EV_MCUMT_SAT_BGN + 3);
OSPEVENT( MT_MCU_READY_NOTIF,           EV_MCUMT_SAT_BGN + 4);

OSPEVENT( MCU_MTCONNECTED_NOTIF,        EV_MCUMT_SAT_BGN + 6);

OSPEVENT( MT_MCU_APPLYJOINCONF_REQ,     EV_MCUMT_SAT_BGN + 8);
OSPEVENT( MCU_MT_APPLYJOINCONF_ACK,     EV_MCUMT_SAT_BGN + 9);
OSPEVENT( MCU_MT_APPLYJOINCONF_NACK,    EV_MCUMT_SAT_BGN + 10);

OSPEVENT( MCU_MT_MTTOPO_NOTIF,          EV_MCUMT_SAT_BGN + 12);

OSPEVENT( MCU_MTDISCONNECTED_NOTIF,     EV_MCUMT_SAT_BGN + 14);

#endif /* _EV_MCUMT_H_ */
