/*****************************************************************************
   ģ����      : mcu
   �ļ���      : evmcuvcs.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: MCU��VCS�ӿ���Ϣ����
   ����        : ���㻪
   �汾        : 
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2008/11/20              ���㻪      ����
******************************************************************************/
#ifndef _EVMCUVCS_H_
#define _EVMCUVCS_H_

#include "osp.h"
#include "eventid.h"

/*------------------------------------------------------------------------------
 MCU��VCS�ӿ���Ϣ
------------------------------------------------------------------------------*/

//1 �û������ȡMCS��MCU��������Ϣ, ����Ϣ��ΪCVCSUsrInfo
//2 �û�������ȡMCS��MCU��������Ϣ����Ϣ��


//3 ����ģ������ȡMCS��MCU��������Ϣ������Ϣ������
//VCS����ģ�壬     //��Ϣ��: TConfInfo + 2 byte(������,TMtAlias����������,�������n)
//                  + n ��(TMtAlias������,����Ϊ 1byte(��������)+1byte(��������)+xbyte(�����ַ���)+2byte(��������)) 
//                  + TMultiTvWallModule(��ѡ�ֶΣ��ɻ��������е��Ƿ��е���ǽģ�����) 
//                  + TVmpModule(��ѡ�ֶΣ��ɻ��������е��Ƿ��л���ϳ�ǽģ�����)
//                  + 1byte(u8: 0 1  �Ƿ������˸������ǽ)
//                  + (��ѡ, THDTvWall)
//                  + 1byte(u8: ��������HDU��ͨ������)
//                  + (��ѡ, THduModChnlInfo+...)
//                  + 1byte(�Ƿ�Ϊ��������)��(��ѡ��2byte[u16 ���������ô�����ܳ���]+�¼�mcu[1byte(��������)+1byte(��������)+xbyte(�����ַ���)+2byte(��������)...)])
//                  + 1byte(�Ƿ�֧�ַ���)��(��ѡ��2byte(u16 ��������Ϣ�ܳ�)+����(1byte(����)+n��[1TVCSGroupInfo��m��TVCSEntryInfo])
//                  + 1�� TMtAlias �����ݱ����նˣ�����ʵ�ֱ����ն�1+1���ݣ�

//4 ��������
   //VCS��MCU�ϰ�����ģ�崴��һ����ʱ����, ����Ϣ��                
    OSPEVENT( VCS_MCU_CREATECONF_BYTEMPLATE_REQ,		EV_VCSVC_BGN + 1 );
    //����ģ��ɹ���ת��ɼ�ʱ����, ����Ϣ�� 
    OSPEVENT( MCU_VCS_CREATECONF_BYTEMPLATE_ACK,		EV_VCSVC_BGN + 2 );
    //����ģ��ת��ɼ�ʱ����ʧ��, ����Ϣ��
    OSPEVENT( MCU_VCS_CREATECONF_BYTEMPLATE_NACK,       EV_VCSVC_BGN + 3 );

	//VCSΪָ���Ļ���ģ�����÷�����Ϣ,      ��Ϣ�壺��������
	OSPEVENT( VCS_MCU_SENDPACKINFO_REQ,                     EV_VCSVC_BGN + 4 );
	//����VCSΪָ���Ļ���ģ�����÷�����Ϣ,   ��Ϣ�壺��
	OSPEVENT( MCU_VCS_SENDPCAKINFO_ACK,                     EV_VCSVC_BGN + 5 );
	//������VCSΪָ���Ļ���ģ�����÷�����Ϣ, ��Ϣ�壺��(��������)
	OSPEVENT( MCU_VCS_SENDPCAKINFO_NACK,                    EV_VCSVC_BGN + 6 );
	//MCU��VCS�Ļ���ģ�������Ϣͨ��,        ��Ϣ�壺��������
	OSPEVENT( MCU_VCS_PACKINFO_NOTIFY,                      EV_VCSVC_BGN + 7 );

// 5 ���Ȳ�������������Ϣ
	// VCS�����ĳ����ϯ��ʼ����       ��Ϣ�壺��(������Ϣͷ��CConfId)
	OSPEVENT( VCS_MCU_OPRCONF_REQ,              EV_VCSVC_BGN + 10 );
	// VCS�����ĳ����ϯ��ʼ������Ӧ�� ��Ϣ�壺��
	OSPEVENT( MCU_VCS_OPRCONF_ACK,              EV_VCSVC_BGN + 11 );
	// VCS�����ĳ����ϯ��ʼ������Ӧ�� ��Ϣ�壺��
	OSPEVENT( MCU_VCS_OPRCONF_NACK,             EV_VCSVC_BGN + 12 );

	// VCS�����˳�����ϯ               ��Ϣ�壺��(������Ϣͷ��CConfId)
	OSPEVENT( VCS_MCU_QUITCONF_REQ,             EV_VCSVC_BGN + 13 );
	// VCS�����˳�����ϯ��Ӧ��         ��Ϣ�壺��
	OSPEVENT( MCU_VCS_QUITCONF_ACK,             EV_VCSVC_BGN + 14 );
	// VCS�����˳�����ϯ��Ӧ��         ��Ϣ�壺��
	OSPEVENT( MCU_VCS_QUITCONF_NACK,            EV_VCSVC_BGN + 15 );

	//VCS�������ĵ���ģʽ,    ��Ϣ�壺��
	OSPEVENT( VCS_MCU_VCMODE_REQ,               EV_VCSVC_BGN + 20 );
	//VCS�������ĵ���ģʽӦ��,��Ϣ�壺u8(VCS_SINGLE_MODE��VCS_MULTW_MODE��VCS_MULVMP_MODE)
	OSPEVENT( MCU_VCS_VCMODE_ACK,               EV_VCSVC_BGN + 21 );
	//VCS�������ĵ���ģʽӦ��,��Ϣ�壺��
	OSPEVENT( MCU_VCS_VCMODE_NACK,              EV_VCSVC_BGN + 22 );

	//VCS���Ļ���ĵ���ģʽ��   ��Ϣ�壺u8(VCS_SINGLE_MODE��VCS_MULTW_MODE��VCS_MULVMP_MODE)
    OSPEVENT( VCS_MCU_CHGVCMODE_REQ,            EV_VCSVC_BGN + 23 );
	//VCS���Ļ���ĵ���ģʽӦ��
	OSPEVENT( MCU_VCS_CHGVCMODE_ACK,            EV_VCSVC_BGN + 24 );
	//VCS���Ļ���ĵ���ģʽӦ��,��Ϣ�壺��
	OSPEVENT( MCU_VCS_CHGVCMODE_NACK,           EV_VCSVC_BGN + 25 );
	//VCS���Ļ���ĵ���ģʽͨ��,��Ϣ�壺u8
	OSPEVENT( MCU_VCS_CHGVCMODE_NOTIF,          EV_VCSVC_BGN + 26 );
	
	//VCS����ָ�����ն�,         ��Ϣ�壺u8(�����ն��������ͣ�VCS_DTYPE_MTID, VCS_DTYPE_MTALIAS) +
	//                                   TMt(�����ն˵���Ϣ)����u8(��������)+u8(��������)+xbyte(�����ַ���)+u16(��������) +
	//                                   u8(��������:VCS_VCTYPE_DEFAULT, VCS_VCTYPE_DROPMT)
	//                                   u8(ǿ������:VCS_FORCECALL_REQ, VCS_FORCECALL_CMD)
	OSPEVENT( VCS_MCU_VCMT_REQ,                 EV_VCSVC_BGN + 27 );
	//VCS����ָ�����ն˵�Ӧ��,   ��Ϣ�壺TMt(�����ն˵���Ϣ) + u8(��������)
	//                                   u8(ǿ������:VCS_FORCECALL_REQ, VCS_FORCECALL_CMD)
	OSPEVENT( MCU_VCS_VCMT_ACK,                 EV_VCSVC_BGN + 28 );
	//VCS����ָ�����ն�Ӧ��,     ��Ϣ�壺��ERR_MCU_VCS_VCMTING����Ϣ��TMt(��һ�������ն˵���Ϣ)
	OSPEVENT( MCU_VCS_VCMT_NACK,                EV_VCSVC_BGN + 29 );
	//VCS���Ƚ��ͨ�棬          ��Ϣ�壺��ERR_MCU_VCS_VCMTOVERTIME����Ϣ��TMt(��һ�������ն˵���Ϣ)(��������)
	OSPEVENT( MCU_VCS_VCMT_NOTIF,               EV_VCSVC_BGN + 30 );

	//VCS����״̬ͨ��            ��Ϣ�壺CBasicVCCStatus
	OSPEVENT( MCU_VCS_CONFSTATUS_NOTIF,         EV_VCSVC_BGN + 31 );

	//VCS����������/����       ��Ϣ�壺u8(����ʱ�ĵ���ģʽ)+u8(����(1)/�ر�(0))
	//                                  +u8(VCS_OPR_LOCAL/VCS_OPR_REMOTE)+u8(VCS_AUDPROC_MUTE/VCS_AUDPROC_SILENCE)
	OSPEVENT( VCS_MCU_MUTE_REQ,                 EV_VCSVC_BGN + 40 );
	//VCS����������/����Ӧ��   ��Ϣ�壺 ��
	OSPEVENT( MCU_VCS_MUTE_ACK,                 EV_VCSVC_BGN + 41 );
	//VCS����������/����Ӧ��   ��Ϣ�壺 ��(�������룬��ģʽ���л�������ȡ��)
	OSPEVENT( MCU_VCS_MUTE_NACK,                EV_VCSVC_BGN + 42 );

// 6 
	//��������Զ��� 
	OSPEVENT( VCS_MCU_GETSOFTNAME_REQ  ,        EV_VCSVC_BGN + 60 );
	OSPEVENT( MCU_VCS_GETSOFTNAME_ACK  ,        EV_VCSVC_BGN + 61 );
	OSPEVENT( MCU_VCS_GETSOFTNAME_NACK,         EV_VCSVC_BGN + 62 );

	OSPEVENT( VCS_MCU_CHGSOFTNAME_REQ  ,        EV_VCSVC_BGN + 63 );
	OSPEVENT( MCU_VCS_CHGSOFTNAME_ACK  ,        EV_VCSVC_BGN + 64 );
    OSPEVENT( MCU_VCS_CHGSOFTNAME_NACK ,        EV_VCSVC_BGN + 65 );

	OSPEVENT( VCS_MCU_SOFTNAME_NOTIF   ,        EV_VCSVC_BGN + 66 );


// �������, ������
	//VCS����MCU�������Ӳ�����ն�,   ��Ϣ�壺  u8(����)+u8(��������)+s8�ַ���(����)+u8(�ն���)+TMt����+
	//                                          ...
	OSPEVENT( VCS_MCU_GROUPCALLMT_REQ,            EV_VCSVC_BGN + 70 );
	//MCUͬ���������Ӳ�����ն�,      ��Ϣ�壺  ��
	OSPEVENT( MCU_VCS_GROUPCALLMT_ACK,            EV_VCSVC_BGN + 71 );
	//MCU�ܾ��������Ӳ�����ն�,      ��Ϣ�壺  ��
	OSPEVENT( MCU_VCS_GROUPCALLMT_NACK,           EV_VCSVC_BGN + 72 );
	//ͨ��                            ��Ϣ��:   ����(��ʧ�ܴ�����)
	OSPEVENT( MCU_VCS_GROUPCALLMT_NOTIF ,           EV_VCSVC_BGN + 73 );

	//VCS����MCU�����Ҷ�����ն�,     ��Ϣ�壺 u8(����)+u8(��������)+s8�ַ���(����)+u8(�ն���)+TMt����+
	//                                          ...
	OSPEVENT( VCS_MCU_GROUPDROPMT_REQ,            EV_VCSVC_BGN + 75 );
	//MCUͬ�������Ҷ�����ն�,        ��Ϣ�壺  ��
	OSPEVENT( MCU_VCS_GROUPDROPMT_ACK,            EV_VCSVC_BGN + 76 );
	//MCU�ܾ������ҶϺ��ն�,          ��Ϣ�壺  ��
	OSPEVENT( MCU_VCS_GROUPDROPMT_NACK,           EV_VCSVC_BGN + 77 );

	//VCS����MCU��ʱ�����ն�,    ��Ϣ�壺  TAddMtInfo
	OSPEVENT( VCS_MCU_ADDMT_REQ,                EV_VCSVC_BGN + 80 );
	//MCUͬ�������ն�,           ��Ϣ�壺  ��
	OSPEVENT( MCU_VCS_ADDMT_ACK,                EV_VCSVC_BGN + 81 );
	//MCU�ܾ������ն�,           ��Ϣ�壺  ��
	OSPEVENT( MCU_VCS_ADDMT_NACK,               EV_VCSVC_BGN + 82 );

	//VCS����MCUɾ���ն�,        ��Ϣ�壺  TMt
	//VCS��Ҫ��֤����ɾ�����ն˷ǵ���ģʽ�еĿɵ�����Դ
	OSPEVENT( VCS_MCU_DELMT_REQ,                EV_VCSVC_BGN + 85 );
	//MCUͬ�������ն�,           ��Ϣ�壺  ��
	OSPEVENT( MCU_VCS_DELMT_ACK,                EV_VCSVC_BGN + 86 );
	//MCU�ܾ������ն�,           ��Ϣ�壺  ��
	OSPEVENT( MCU_VCS_DELMT_NACK,               EV_VCSVC_BGN + 87 );

	//VCS������ϯ��ѯ,               ��Ϣ�壺  u8(��ѯ���)
	OSPEVENT( VCS_MCU_STARTCHAIRMANPOLL_REQ,     EV_VCSVC_BGN + 90 );
	//MCUͬ�⿪����ϯ��ѯ,           ��Ϣ�壺  ��
	OSPEVENT( MCU_VCS_STARTCHAIRMANPOLL_ACK,      EV_VCSVC_BGN + 91 );
	//MCU�ܾ�������ϯ��ѯ,           ��Ϣ�壺  ��(��������)
	OSPEVENT( MCU_VCS_STARTCHAIRMANPOLL_NACK,     EV_VCSVC_BGN + 92 );

	//VCSֹͣ��ϯ��ѯ,               ��Ϣ�壺  ��
	OSPEVENT( VCS_MCU_STOPCHAIRMANPOLL_REQ,     EV_VCSVC_BGN + 95 );
	//MCUͬ��ֹͣ��ϯ��ѯ,           ��Ϣ�壺  ��
	OSPEVENT( MCU_VCS_STOPCHAIRMANPOLL_ACK,      EV_VCSVC_BGN + 96 );
	//MCU�ܾ�ֹͣ��ϯ��ѯ,           ��Ϣ�壺  ��
	OSPEVENT( MCU_VCS_STOPCHAIRMANPOLL_NACK,     EV_VCSVC_BGN + 97 );
	
// ����
    // ͬ��ǿ������                   ��Ϣ��:   TMt(�����ͷŵ��ն�) + s8[](������ռ�Ļ�������)
	OSPEVENT( MCU_VCS_RELEASEMT_REQ,             EV_VCSVC_BGN + 100 );
    // ͬ��ǿ��                       ��Ϣ��:   TMt(�����ͷŵ��ն�) + s8[](������ռ�Ļ�������)
	OSPEVENT( VCS_MCU_RELEASEMT_ACK,             EV_VCSVC_BGN + 101 );
	// �ܾ�ǿ��                       ��Ϣ��:�� TMt(�����ͷŵ��ն�) + s8[](������ռ�Ļ�������)��
	OSPEVENT( VCS_MCU_RELEASEMT_NACK,            EV_VCSVC_BGN + 102 );
	// ǿ��ִ��ͨ��                   ��Ϣ��:   TMt(�����ͷŵ��ն�) + s8[](������ռ�Ļ�������)
	OSPEVENT( MCU_VCS_RELEASEMT_NOTIF,           EV_VCSVC_BGN + 103 );
 
// 7 Ϊ���汣��mcu��������Ϣ
    //VCS�����ȡ��������Ϣ��        ��Ϣ�壺  ��
    OSPEVENT( VCS_MCU_GETUNPROCFGINFO_REQ,       EV_VCSVC_BGN + 150 );
    //�����ȡ��������Ϣ�ɹ�,        ��Ϣ�壺  s8[] ʵ������
    OSPEVENT( MCU_VCS_GETUNPROCFGINFO_ACK,       EV_VCSVC_BGN + 151 );
    //�����ȡ��������Ϣʧ��,        ��Ϣ�壺  ��
    OSPEVENT( MCU_VCS_GETUNPROCFGINFO_NACK ,     EV_VCSVC_BGN + 152 );

    //VCS����mcu���治������Ϣ��     ��Ϣ�壺  s8[] ʵ������
    OSPEVENT( VCS_MCU_SETUNPROCFGINFO_REQ,       EV_VCSVC_BGN + 153 );
    //���󱣴�mcu��������Ϣ�ɹ�,     ��Ϣ�壺  ��
    OSPEVENT( MCU_VCS_SETUNPROCFGINFO_ACK,       EV_VCSVC_BGN + 154 );
    //���󱣴�mcu��������Ϣʧ��,     ��Ϣ�壺  ��  
    OSPEVENT( MCU_VCS_SETUNPROCFGINFO_NACK,      EV_VCSVC_BGN + 155 );
    //mcu��������Ϣͨ��,             ��Ϣ�壺  s8[] ʵ������
    OSPEVENT( MCU_VCS_SETUNPROCFGINFO_NOTIF,     EV_VCSVC_BGN + 156 );

	//�����������������ӵ���Ϣ
	//zjj20091102��Ϊֻ�����ֶ�ģʽ
	//VCS�����ն˽������ǽ�ĸ�ͨ��(ֻ�����ֶ�ģʽ��Ԥ��ģʽ)	  ��Ϣ�壺TSwitchInfo����(u8)+
	//    u8(�����ն��������ͣ�VCS_DTYPE_MTID, VCS_DTYPE_MTALIAS ��VCS_DTYPE_MTALIAS+u8(��������)+u8(��������)+xbyte(�����ַ���)+u16(��������))
	//    +TSwitchInfo
	//    +...
	OSPEVENT( VCS_MCU_SETMTINTVWALL_REQ,		EV_VCSVC_BGN  + 157	);
	//MCU�����ն˽������ǽ�ĸ�ͨ���ɹ�(ֻ�����ֶ�ģʽ��Ԥ��ģʽ) ��Ϣ��: ��
	OSPEVENT( MCU_VCS_SETMTINTVWALL_ACK,		EV_VCSVC_BGN  + 158 );
	//MCU�����ն˽������ǽ�ĸ�ͨ��ʧ��(ֻ�����ֶ�ģʽ��Ԥ��ģʽ) ��Ϣ��: ��
	OSPEVENT( MCU_VCS_SETMTINTVWALL_NACK,		EV_VCSVC_BGN  + 159 );

	//zjj20091102
	//VCS��Ӻͱ���Ԥ��					��Ϣ��:		u8[]	Ԥ�����Ƴ���(u8)+Ԥ����+ + TMtVCSPlanAlias����
	//��� �ն˸���num(u8)Ϊ0,��Ϊ���Ԥ�����������0���ͱ�ʾ�Ǳ���Ԥ���ն˱�������
	OSPEVENT( VCS_MCU_ADDPLANNAME_REQ,			EV_VCSVC_BGN  + 160 );
	//MCU��Ӧ���Ԥ���ɹ�			��Ϣ��:��
	OSPEVENT( MCU_VCS_ADDPLANNAME_ACK,			EV_VCSVC_BGN  + 161 );
	//MCU��Ӧ���Ԥ��ʧ��			��Ϣ��:��
	OSPEVENT( MCU_VCS_ADDPLANNAME_NACK,			EV_VCSVC_BGN  + 162 );

	//VCSɾ��Ԥ��					��Ϣ��:		u8[]		Ԥ������(��\0)
	OSPEVENT( VCS_MCU_DELPLANNAME_REQ,			EV_VCSVC_BGN  + 163 );
	//MCU��Ӧɾ��Ԥ���ɹ�			��Ϣ��:��
	OSPEVENT( MCU_VCS_DELPLANNAME_ACK,			EV_VCSVC_BGN  + 164 );
	//MCU��Ӧɾ��Ԥ��ʧ��			��Ϣ��:��
	OSPEVENT( MCU_VCS_DELPLANNAME_NACK,			EV_VCSVC_BGN  + 165 );

	//VCS�޸�Ԥ������				��Ϣ��:		u8[]Ҫ�޸ĵ�Ԥ��������(u8)+Ҫ�޸ĵ�Ԥ����+��Ԥ��������(u8)+��Ԥ����
	OSPEVENT( VCS_MCU_MODIFYPLANNAME_REQ,		EV_VCSVC_BGN  + 166 );
	//MCU��Ӧ�޸�Ԥ�����Ƴɹ�			��Ϣ��:��
	OSPEVENT( MCU_VCS_MODIFYPLANNAME_ACK,		EV_VCSVC_BGN  + 167 );
	//MCU��Ӧ�޸�Ԥ������ʧ��			��Ϣ��:��
	OSPEVENT( MCU_VCS_MODIFYPLANNAME_NACK,		EV_VCSVC_BGN  + 168 );

	//MCU֪ͨvcs���еĻ���Ԥ������		��Ϣ��:TMtVcsPlanNames
	OSPEVENT( MCU_VCS_ALLPLANNAME_NOTIFY,		EV_VCSVC_BGN  + 169 );

	/*//VCS���󱣴�ĳ��Ԥ��(��ӻ�ɾ��Ԥ���е��ն˱���) ��Ϣ��: Ԥ�����Ƴ���(u8)+Ԥ����+�ն˸���num(u8) + TMtVCSPlanAlias����
	OSPEVENT( VCS_MCU_SAVEPLANDATA_REQ,			EV_VCSVC_BGN  + 170 );
	//MCU��Ӧ����ĳ��Ԥ���ɹ�(��ӻ�ɾ��Ԥ���е��ն˱���) ��Ϣ��:��
	OSPEVENT( MCU_VCS_SAVEPLANDATA_ACK,			EV_VCSVC_BGN  + 171 );
	//MCU��Ӧ����ĳ��Ԥ��ʧ��(��ӻ�ɾ��Ԥ���е��ն˱���) ��Ϣ��:��
	OSPEVENT( MCU_VCS_SAVEPLANDATA_NACK,		EV_VCSVC_BGN  + 172 );
	*/

	//MCU֪ͨvcsĳ��Ԥ�������ƺͱ�������		��Ϣ��:Ԥ�����Ƴ���(u8)+Ԥ����+�ն˸���num(u8) + TMtVcsPlanNames����
	OSPEVENT( MCU_VCS_ONEPLANDATA_NOTIFY,		EV_VCSVC_BGN  + 170 );

	//VCS����������Ԥ��������Ϣ				��Ϣ��:ConfId�ṹ
	OSPEVENT( VCS_MCU_GETALLPLANDATA_REQ,		EV_VCSVC_BGN  + 171 );
	//mcu��Ӧvcs����������Ԥ�����ݳɹ�			��Ϣ��:��
	OSPEVENT( MCU_VCS_GETALLPLANDATA_ACK,		EV_VCSVC_BGN  + 172 );
	//mcu��Ӧvcs����������Ԥ������ʧ��			��Ϣ��:��
	OSPEVENT( MCU_VCS_GETALLPLANDATA_NACK,		EV_VCSVC_BGN  + 173 );

	//MCU֪ͨvcs���е����е�����(��Ϊ��������һ��������������ʵ�ʷ���ʱ��Ҳ��һ�η�һ��Ԥ������)
	//��Ϊ������������ӦVCS_MCU_GETALLPLANDATA_REQ��Ϣ�ģ�������Ȼ��Ϣ���MCU_VCS_ONEPLANDATA_NOTIFY��Ϣ����Ϣ��һ�������������¶���һ��
	//��Ϣ��: Ԥ�����Ƴ���(u8)+Ԥ����+�ն˱�������num(u8) + TMtVCSPlanAlias����
	OSPEVENT( MCU_VCS_ALLPLANDATA_SINGLE_NOTIFY,EV_VCSVC_BGN  + 174 );


	//vcs����mcu�ı����ǽģʽ		��Ϣ��:ģʽֵ(u8)VCS_TVWALLMANAGE_MANUAL_MODE��
	OSPEVENT( VCS_MCU_CHGTVWALLMODE_REQ,		EV_VCSVC_BGN  + 175 );
	//mcu��Ӧvcs�ı����ǽģʽ�ɹ�	��Ϣ�壺��
	OSPEVENT( MCU_VCS_CHGTVWALLMODE_ACK,		EV_VCSVC_BGN  + 176 );
	//mcu��Ӧvcs�ı����ǽģʽʧ��	��Ϣ�壺��
	OSPEVENT( MCU_VCS_CHGTVWALLMODE_NACK,		EV_VCSVC_BGN  + 177 );

	//MCU֪ͨvcs���ڵĵ���ǽģʽ	��Ϣ�壺ģʽֵ(u8)VCS_TVWALLMANAGE_MANUAL_MODE��
	//OSPEVENT( MCU_VCS_TVWALLMODE_NOTIFY,		EV_VCSVC_BGN  + 181 );

	/*//VCS֪ͨmcu�ն˽������ǽͨ��(ֻ������Ԥ��ģʽ��),��Ϣ�壺TSwitchInfo����(u8)+TSwitchInfo+TSwitchInfo+...
	OSPEVENT( VCS_MCU_REVIEWMODEMTINTVWALL_REQ,	EV_VCSVC_BGN  + 182 );
	//mcu��Ӧvcs�ն˽������ǽͨ���ɹ�(ֻ������Ԥ��ģʽ��),��Ϣ�壺��
	OSPEVENT( MCU_VCS_REVIEWMODEMTINTVWALL_ACK,	EV_VCSVC_BGN  + 183 );
	//mcu��Ӧvcs�ն˽������ǽͨ��ʧ��(ֻ������Ԥ��ģʽ��),��Ϣ�壺��
	OSPEVENT( MCU_VCS_REVIEWMODEMTINTVWALL_NACK,EV_VCSVC_BGN  + 184 );
	*/

	//VCS֪ͨmcu���ĳ������ǽͨ����Ϣ(ֻ�����ֶ�ģʽ),��Ϣ��:eqpid(u8)+channelindex(u8)
	OSPEVENT( VCS_MCU_CLEARONETVWALLCHANNEL_CMD,EV_VCSVC_BGN  + 178  );
	//VCS֪ͨmcu���ȫ������ǽͨ����Ϣ(ֻ�����ֶ�ģʽ),��Ϣ�壺��
	OSPEVENT( VCS_MCU_CLEARALLTVWALLCHANNEL_CMD,EV_VCSVC_BGN  + 179  );

	//vcs֪ͨmcu���õ���ǽͨ���ն˻���״̬ ��Ϣ��:�Ƿ����(u8)0-ȡ������,1-��ʼ����
	OSPEVENT( VCS_MCU_TVWALLSTARMIXING_CMD,		EV_VCSVC_BGN  + 180  );
		
	//mcu֪ͨvcs�Ѿ��޸ĳɹ���Ԥ������ ��Ϣ��:		u8[]Ҫ�޸ĵ�Ԥ��������(u8)+Ҫ�޸ĵ�Ԥ����+��Ԥ��������(u8)+��Ԥ����
	OSPEVENT( MCU_VCS_MODIFYPLANNAME_NOTIFY,	EV_VCSVC_BGN  + 181 );

	//��ش��ã�ʵ��vcs���ᷢ������ֻ�ڶ�ش���Ӧ�����ش�������ʶѡ�������� ��Ϣ��:��
	OSPEVENT( VCS_MCU_CONFSELMT_CMD,			EV_VCSVC_BGN  + 182 );
	/*//vcs�������������ĳ��Ԥ��						��Ϣ��:Ԥ��������(u8)+Ԥ����+���������(u8)(0-���� 1-����)
	OSPEVENT( VCS_MCU_LOCKPLAN_REQ,				EV_VCSVC_BGN  + 182 );
	//vcs�������������ĳ��Ԥ���ɹ�					��Ϣ��:Ԥ��������(u8)+Ԥ����+���������(u8)(0-���� 1-����)
	OSPEVENT( MCU_VCS_LOCKPLAN_ACK,				EV_VCSVC_BGN  + 183 );
	//vcs�������������ĳ��Ԥ��ʧ��					��Ϣ��:Ԥ��������(u8)+Ԥ����+���������(u8)(0-���� 1-����)
	OSPEVENT( MCU_VCS_LOCKPLAN_NACK,			EV_VCSVC_BGN  + 184 );

	//vcs�������������vcs������Ԥ��(��vcs������vcs�˳�����ϯʱ��mcu�Լ�����������)//��Ϣ��:��	
	OSPEVENT( VCS_MCU_VCSUNLOCKPLAN_CMD,		EV_VCSVC_BGN  + 185 );
	*/

    //[11/18/2011 zhushengze]֪ͨVCS�¼��ն˿�ʼ��˫��������VCS���������� ��Ϣ�壺TMt:˫��Դ
    OSPEVENT( MCU_VCS_STARTDOUBLESTREAM_NOTIFY,   EV_VCSVC_BGN  + 184  );
    //VCS�л������˫��״̬: ��Ϣ�� u8��CONF_DUALSTATUS_DISABLE/CONF_DUALSTATUS_ENABLE
    OSPEVENT( VCS_MCU_CHGDUALSTATUS_REQ,          EV_VCSVC_BGN + 185 );
    OSPEVENT( MCU_VCS_CHGDUALSTATUS_ACK,          EV_VCSVC_BGN + 186 );
    OSPEVENT( MCU_VCS_CHGDUALSTATUS_NACK,         EV_VCSVC_BGN + 187 );

	// ������� ��ʼ���  ��Ϣ�壺TSwitchDstInfo 
    OSPEVENT( VCS_MCU_START_MONITOR_UNION_REQ,          EV_VCSVC_BGN + 188 );
    OSPEVENT( MCU_VCS_START_MONITOR_UNION_ACK,          EV_VCSVC_BGN + 189 );
    OSPEVENT( MCU_VCS_START_MONITOR_UNION_NACK,         EV_VCSVC_BGN + 190 );
	
	// MCU��֪������Կ�ʼ��ء���Ϣ�壺
	// TSwitchDstInfo + TMediaEncrypt(��Ƶ������Ϣ) + TDoublePayload(��Ƶ�غ�) +
	// TMediaEncrypt(��Ƶ������Ϣ) +  TDoublePayload(��Ƶ�غ�) + TAudAACCap(�����Ƶ��AACLC,AACLD��׷��)
	// + TTransportAddr(video rtcp addr) + TTransportAddr(audio rtcp addr) 
	OSPEVENT( MCU_VCS_START_MONITOR_NOTIF,				EV_VCSVC_BGN + 191 );
	
	// ������� ֹͣ���  ��Ϣ�壺TSwitchDstInfo
    OSPEVENT( VCS_MCU_STOP_MONITOR_UNION_REQ,          EV_VCSVC_BGN + 192 );
    OSPEVENT( MCU_VCS_STOP_MONITOR_UNION_ACK,          EV_VCSVC_BGN + 193 );
    OSPEVENT( MCU_VCS_STOP_MONITOR_UNION_NACK,         EV_VCSVC_BGN + 194 );

		

	//�ն������������  ��Ϣ�壺byEncrypt(1-����,0-������)+bySrcDriId(u8)+TMtAlias(tFstAlias)+TMtAlias(tMtH323Alias)+TMtAlias(tMtE164Alias)+TMtAlias(tMtAddr)+U16(wCallRate)+u8(bySrcSsnId)
    OSPEVENT( MCU_VCS_MTJOINCONF_REQ,          EV_VCSVC_BGN + 195 );
    OSPEVENT( VCS_MCU_MTJOINCONF_ACK,          EV_VCSVC_BGN + 196 );
    OSPEVENT( VCS_MCU_MTJOINCONF_NACK,         EV_VCSVC_BGN + 197 );

	OSPEVENT( VCS_MCU_STOP_ALL_MONITORUNION_CMD,  EV_VCSVC_BGN + 198 );
    
    //[5/19/2011 zhushengze]vcs֪ͨmcu��������ն������������ ��Ϣ��:(u8)0-����,1-����
	OSPEVENT( VCS_MCU_GROUPMODELOCK_CMD,		EV_VCSVC_BGN  + 199  );
	
#endif /*_EVMCUVCS_H_*/


