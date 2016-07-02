/*****************************************************************************
   ģ����      : Mcu Agent
   �ļ���      : Agentinterface.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: mcu ���ý��滯�ӿ�
   ����        : 
   �汾        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���        �޸�����
   2005/08/17  4.0         liuhuiyun     ����
   2006/11/02  4.0         �ű���        �����Ż�
******************************************************************************/
#ifndef _AGENT_INTERFACE_H_
#define _AGENT_INTERFACE_H_
#include "agentcommon.h"
#include "mcuagtstruct.h"
#include "mcuinnerstruct.h"

#ifdef WIN32
#pragma  once
#endif

class CCfgAgent;

BOOL32 McuAgentInit( u8 byMcuType, BOOL32 bEnabled = TRUE );        // ��ʼ������
BOOL32 McuAgentQuit( BOOL32 bRebootBrd = TRUE );                    // �˳�����( �������Ƿ��������е���)

class CAgentInterface
{
public:
    CAgentInterface();
    ~CAgentInterface();
    
public:
    u32    GetDcsIp( void );                                        // ��ȡDCS IP
    u16    SetDcsIp( u32 dwDcsIp );                                 // ����DCS IP
    u16    GetQosInfo( TQosInfo*  tQosInfo );                       // ��ȡQOS��Ϣ
    u16    SetQosInfo( TQosInfo tQosInfo );                         // ����QOS��Ϣ
    u16    GetLocalInfo( TLocalInfo*  tLocalInfo );                 // ��ȡ������Ϣ
    u16    SetLocalInfo( TLocalInfo  tLocalInfo );                  // ���ñ�����Ϣ
    u16    GetNetWorkInfo( TNetWorkInfo*  tNetWorkInfo );           // ��ȡ������Ϣ
    u16    SetNetWorkInfo( TNetWorkInfo  tNetWorkInfo );            // ����������Ϣ
    u16    GetNetSyncInfo( TNetSyncInfo* ptNetSyncInfo );           // ��ȡ��ͬ����Ϣ
    u16    SetNetSyncInfo( TNetSyncInfo tNetSyncInfo );             // ������ͬ����Ϣ
    u16    GetDscInfo( TDSCModuleInfo *ptDSCInfo );                 // ��ȡ8000B DSC��������Ϣ
	u16    SetDscInfo( TDSCModuleInfo *ptDSCInfo, BOOL32 bToFile ); // ����8000B DSC��������Ϣ
	u8	   GetConfigedDscType( void );								// ���DSC������ͣ����û�����ù����򷵻�0, zgc, 2007-03-05

	u16    SetLoginInfo( TLoginInfo *ptLoginInfo );					// ���õ�½��Ϣ
	u16	   GetLoginInfo( TLoginInfo *ptLoginInfo );					// ��ȡ��½��Ϣ

	u16	   GetLastDscInfo( TDSCModuleInfo * ptDscInfo );				// ȡ������޸�֮ǰ��DSC����������Ϣ
	u16	   SetLastDscInfo( TDSCModuleInfo * ptDscInfo );				// �����޸�֮ǰ��DSC����������Ϣ

	// ��DSC info���õ�MPC����, zgc, 2007-07-17
	BOOL32 SaveDscLocalInfoToNip( TDSCModuleInfo * ptDscInfo );
	// ����·�ɵ�MINIMCU MPC����, zgc, 2007-09-21
	BOOL32 SaveRouteToNipByDscInfo( TDSCModuleInfo *ptDscInfo );

	// ���MCU�������ļ����, zgc, 2007-07-25
	u8	   GetMcuCfgInfoLevel(void);

    // ���� ��������MCS��IP��Ϣ  [04/24/2012 liaokang]
    BOOL32 AddRegedMcsIp( u32 dwMcsIpAddr );                        // �������MCS��IP��Ϣ
    BOOL32 DeleteRegedMcsIp( u32 dwMcsIpAddr );                     // ɾ������MCS��IP��Ϣ

	BOOL32 IsMcuConfiged();											// �ж�MCU�Ƿ����ù�
	u16    SetIsMcuConfiged(BOOL32 bConfiged);			    		// ����MCU���ñ�ʶ

    u16    ReadTrapTable( u8* pbyTrapNum, TTrapInfo* ptTrapTable ); // ��ȡ����TRAP��Ϣ
    u16    WriteTrapTable( u8 byTrapNum, TTrapInfo* ptTrapTable );  // ��������TRAP��Ϣ
    u16    ReadBrdTable( u8* pbyBrdNum, TBoardInfo* pptBoardTable );// ��ȡ���嵥�����Ϣ
    u16    WriteBrdTable( u8 byBrdNum, TBoardInfo* ptBoardTable );  // �������嵥�����Ϣ
    u16    ReadTvTable( u8* pbyTvNum, TEqpTVWallInfo* ptTvTable );  // ��ȡ����TVWall����Ϣ
    u16    WriteTvTable( u8 byTvNum, TEqpTVWallInfo* ptTvTable );   // ��������TVWall����Ϣ
    u16    ReadRecTable( u8* pbyRecNum, TEqpRecInfo* ptRecTable );  // ��ȡ����Rec����Ϣ
	// �������� bIsNeedUpdatePortInTable��ʶ�Ƿ���Ҫ�����ڴ��еĶ˿���Ϣ [12/15/2011 chendaiwei]
    u16    WriteRecTable( u8 byRecNum, TEqpRecInfo* ptRecTable, BOOL32 bIsNeedUpdatePortInTable = TRUE  );   // ��������Rec����Ϣ
    u16    ReadBasTable( u8* pbyBasNum, TEqpBasInfo* ptBasTable );  // ��ȡ����Bas����Ϣ
	// �������� bIsNeedUpdatePortInTable��ʶ�Ƿ���Ҫ�����ڴ��еĶ˿���Ϣ [12/15/2011 chendaiwei]
    u16    WriteBasTable( u8 byBasNum, TEqpBasInfo* ptBasTable, BOOL32 bIsNeedUpdatePortInTable = TRUE  );   // ��������Bas����Ϣ
    //zw[08/06/2008]
    u16    ReadBasHDTable( u8* pbyBasNum, TEqpBasHDInfo* ptBasHDTable );  // ��ȡ����BasHD����Ϣ
	// �������� bIsNeedUpdatePortInTable��ʶ�Ƿ���Ҫ�����ڴ��еĶ˿���Ϣ [12/15/2011 chendaiwei]
    u16    WriteBasHDTable( u8 byBasNum, TEqpBasHDInfo* ptBasHDTable, BOOL32 bIsNeedUpdatePortInTable = TRUE  );   // ��������BasHD����Ϣ
   
    u16    ReadVmpTable( u8* pbyVmpNum, TEqpVMPInfo* ptVmpTable );  // ��ȡ����VMP����Ϣ
	// �������� bIsNeedUpdatePortInTable��ʶ�Ƿ���Ҫ�����ڴ��еĶ˿���Ϣ [12/15/2011 chendaiwei]
    u16    WriteVmpTable( u8 byVmpNum, TEqpVMPInfo* ptVmpTable, BOOL32 bIsNeedUpdatePortInTable = TRUE   );   // ��������VMP����Ϣ
    u16    ReadMpwTable( u8* pbyMpwNum, TEqpMpwInfo* ptMpwTable );  // ��ȡ����MPW����Ϣ
    u16    WriteMpwTable( u8 byMpwNum, TEqpMpwInfo* ptMpwTable );   // ��������MPW����Ϣ
    u16    ReadPrsTable( u8* pbyPrsNum, TEqpPrsInfo* ptPrsTable );  // ��ȡ����Prs����Ϣ
    u16    WritePrsTable( u8 byPrsNum, TEqpPrsInfo* ptPrsTable );   // ��������Prs����Ϣ
    u16    ReadMixerTable( u8* pbyMixNum, TEqpMixerInfo* pptMixerTable );// ��ȡ����Mix����Ϣ
	// �������� bIsNeedUpdatePortInTable��ʶ�Ƿ���Ҫ�����ڴ��еĶ˿���Ϣ [12/15/2011 chendaiwei]
    u16    WriteMixerTable( u8 byMixNum, TEqpMixerInfo* ptMixerTable, BOOL32 bIsNeedUpdatePortInTable = TRUE );  // ��������Mix����Ϣ
	//4.6�¼������д���ñ� jlb
    u16    ReadHduTable( u8* pbyHduNum, TEqpHduInfo* ptHduTable );// ��ȡ����Hdu����Ϣ
    u16    WriteHduTable( u8 byHduNum, TEqpHduInfo* ptHduTable );  // ��������Hdu����Ϣ
	u16    ReadSvmpTable( u8* pbySvmpNum, TEqpSvmpInfo* ptSvmpTable );// ��ȡ����Svmp����Ϣ
	// �������� bIsNeedUpdatePortInTable��ʶ�Ƿ���Ҫ�����ڴ��еĶ˿���Ϣ [12/15/2011 chendaiwei]
    u16    WriteSvmpTable( u8 bySvmpNum, TEqpSvmpInfo* ptSvmpTable,BOOL32 bIsNeedUpdatePortInTable = TRUE   );  // ��������Svmp����Ϣ
// 	u16    ReadDvmpTable( u8* pbyDvmpNum, TEqpDvmpBasicInfo* ptDvmpTable );// ��ȡ����Dvmp����Ϣ
//  u16    WriteDvmpTable( u8 byDvmpNum, TEqpDvmpBasicInfo* ptDvmpTable );  // ��������Dvmp����Ϣ
	u16    ReadMpuBasTable( u8* pbyMpuBasNum, TEqpMpuBasInfo* ptMpuBasTable );// ��ȡ����MpuBas����Ϣ
	// �������� bIsNeedUpdatePortInTable��ʶ�Ƿ���Ҫ�����ڴ��еĶ˿���Ϣ [12/15/2011 chendaiwei]
    u16    WriteMpuBasTable( u8 byMpuBasNum, TEqpMpuBasInfo* ptMpuBasTable, BOOL32 bIsNeedUpdatePortInTable = TRUE   );  // ��������MpuBas����Ϣ
// 	u16    ReadEbapTable( u8* pbyEbapNum, TEqpEbapInfo* ptEbapTable );// ��ȡ����Ebap����Ϣ
//     u16    WriteEbapTable( u8 byEbapNum, TEqpEbapInfo* ptEbapTable );  // ��������Ebap����Ϣ
// 	u16    ReadEvpuTable( u8* pbyEvpuNum, TEqpEvpuInfo* ptEvpuTable );// ��ȡ����Evpu����Ϣ
//    u16    WriteEvpuTable( u8 byEvpuNum, TEqpEvpuInfo* ptEvpuTable );  // ��������Evpu����Ϣ
    u16    ReadVrsRecTable( u8* pbyVrsRecNum, TEqpVrsRecCfgInfo* ptVrsRecTable );  // ��ȡ����VrsRec����Ϣ
    u16    WriteVrsRecTable( u8 byVrsRecNum, TEqpVrsRecCfgInfo* ptVrsRecTable);   // ��������VrsRec����Ϣ
    u16    ReadHduSchemeTable( u8* pbyHduProjectNum, THduStyleInfo* ptHduSchemeTable );  // ��ȡ����HDUԤ�����ñ���Ϣ
    u16    WriteHduSchemeTable( u8 byHduProjectNum, THduStyleInfo* ptHduSchemeTable );   // ��������HDUԤ�����ñ���Ϣ
    u16    ReadVmpAttachTable( u8* pbyVmpProjectNum, TVmpAttachCfg* ptVmpAttachTable );  // ��ȡ����VMP������Ϣ
    u16    WriteVmpAttachTable( u8 byVmpProjectNum, TVmpAttachCfg* ptVmpAttachTable );   // ��������VMP������Ϣ
	BOOL32 RefreshBrdOsType(u8 byBrdId, u8 byOsType);//����ĳ������忨��OSTYPEֵ
    // �������: [pengguofeng 4/16/2013]
	u16    ReadMcuEncodingType(u8 &byEncoding);
	BOOL32 WriteMcuEncodingType(const u8 &byEncoding);
	u8     GetMcuEncodingType(void);
	void   SetMcuEncodingType(const u8 &byEncoding);
	// end [pengguofeng 4/16/2013]
    
    u16    GetEqpRecorderCfg( u8 byRecId, TEqpRecInfo * ptRecCfg ); // ��ȡ����Rec��Ϣ
    u16    SetEqpRecorderCfg( u8 byRecId, TEqpRecInfo * ptRecCfg ); // ���õ���Rec��Ϣ
    u16    GetEqpTVWallCfg( u8 byTvId, TEqpTVWallInfo* ptTWCfg );   // ��ȡ����TVWall��Ϣ
    u16    SetEqpTVWallCfg( u8 byTvId, TEqpTVWallInfo* ptTWCfg );   // ���õ���TVWall��Ϣ
    u16    GetEqpMixerCfg( u8 byMixId, TEqpMixerInfo* ptMixerCfg ); // ��ȡ����Mix��Ϣ
    u16    SetEqpMixerCfg( u8 byMixId, TEqpMixerInfo tMixerCfg );   // ���õ���Mix��Ϣ
    u16    GetEqpBasCfg( u8 byBasId, TEqpBasInfo* ptBasCfg );       // ��ȡ����Bas��Ϣ
    u16    SetEqpBasCfg( u8 byBasId, TEqpBasInfo* ptBasCfg );       // ���õ���Bas��Ϣ
	// 4.6 jlb 
	u16    SetEqpHduCfg(u8 byHduId, TEqpHduInfo* ptHduCfg);         //��ȡ����Hdu��Ϣ  
	u16    GetEqpHduCfg(u8 byHduId, TEqpHduInfo* ptHduCfg);         //��ȡ����Hdu��Ϣ  

    //zw[08/07/2008]
    BOOL32 IsEqpBasHD(u8 byBasHDId);                                 //�ж��Ƿ����bas
	BOOL32 IsEqpBasAud(u8 byBasId);
    u16    GetEqpBasHDCfg( u8 byBasHDId, TEqpBasHDInfo* ptBasHDCfg );// ��ȡ����BasHD��Ϣ
    u16    SetEqpBasHDCfg( u8 byBasHDId, TEqpBasHDInfo* ptBasHDCfg );// ���õ���BasHD��Ϣ
	u16    GetMpuBasBrdInfo(u8 byBasEqpId,TEqpMpuBasInfo* ptEqpMpuBasInfo);
    BOOL32 IsSVmp( u8 byEqpId );
	u16     GetVmpEqpVersion( u8 byEqpId );
    BOOL32 IsDVmp( u8 byEqpId );
    BOOL32 IsEVpu( u8 byEqpId );
    BOOL32 IsMpuBas( u8 byEqpId );
	u16     GetBasEqpVersion(u8 byEqpId);
	u16		GetHDUEqpVersion( u8 byEqpId );
    BOOL32 IsEBap( u8 byEqpId );    
	u16    GetEqpSvmpCfgById( u8 byId, TEqpSvmpInfo * ptSvmpCfg );

    u16    GetEqpVMPCfg( u8 byVmpId, TEqpVMPInfo* ptVMPCfg );       // ��ȡ����VMP��Ϣ
    u16    SetEqpVMPCfg( u8 byVmpId, TEqpVMPInfo* ptVMPCfg );       // ���õ���VMP��Ϣ
    u16    GetEqpMpwCfg( u8 byMpwId, TEqpMpwInfo* ptMpwCfg );       // ��ȡ����MPW��Ϣ
    u16    SetEqpMpwCfg( u8 byMpwId, TEqpMpwInfo* ptMpwCfg );       // ���õ���MPW��Ϣ
    u16    GetEqpPrsCfg( u8 byPrsId, TEqpPrsInfo& tPrsCfg );        // ��ȡ����Prs��Ϣ
	
	u8     GetEqpSwitchBrdId(u8 byEqpId);                         // ��ȡָ������ת������Ϣ
	BOOL32 SetEqpSwitchBrdId(u8 byEqpId, u8 bySwitchBrdId);       // �����ƶ�����ת������Ϣ
	u16    GetBrdCfgById(u8 byBrdId, TBoardInfo* ptBoardInfo);    // ��ȡ������Ϣ

public:
    u16    GetMcuAlias( s8* pszAlias, u8 byLength );        // ȡMcu����
    u16    GetE164Number( s8* pszE164Num, u8 byLen );       // ȡMcu E164��
    u32    GetMpcIp( void );                                // ȡMPC Ip( ������)
    u16    SetMpcIp( u32 dwIp, u8 byInterface );            // ��MPC Ip( ������)
    u32    GetMaskIp( void );                               // ȡ��������( ������)
    u16    SetMaskIp( u32 dwIp, u8 byInterface );           // ����������( ������)
    u32    GetGateway( void );                              // ȡ����( ������)
    u16    SetGateway( u32 dwIp );                          // ������( ������)
	u8     GetInterface( void );                            // ȡMPC�˿�����
	u16    SetInterface( u8 byInterface );                  // ��MPC�˿�����
    u32    GetCastIpAddr( void );                           // ȡ�鲥��ַ
    u16    GetCastPort( void );                             // ȡ�鲥�˿�

    BOOL32 GetIsGKCharge( void );                           // ȡGK�Ƿ�֧��
    u32    GetGkIpAddr( void );                             // ȡGkIp( ������)
    u16    SetGKIpAddr( u32 dwGkIp );                       // ����Gk Ip( ������)
	u8	   GetGkRRQUsePwdFlag ( void ) const;					// ����GK RRQ�û���
    LPCSTR GetGkRRQPassword ( void );						// ����GK RRQ����

    u32    GetRRQMtadpIp( void );                           // ȡ��Э��������ַ( ������)
    void   SetRRQMtadpIp( u32 dwIp );                       // �����µ��������ַ
    u16    GetRecvStartPort( void );                        // ȡMcu������ʼ�˿�
    u16    Get225245StartPort( void );                      // ȡ225�˿�
    u16    Get225245MtNum( void );                          // ȡ�ն���
    u8     GetMpcBoardId( void );                           // ȡMpc��Id
    BOOL32 GetIsUseMpcStack( void );                        // ȡ�Ƿ�ʹ��Mpc����Э��
    BOOL32 GetIsUseMpcTranData( void );                     // ȡ�Ƿ���Mpcת������(����)
    u16    SetMpcEnabled( BOOL32 bEnabled );                // ��Mpc���Ƿ�������( ҵ�� 1-ACTIVE 0-STANDBY)
    
    u32    GetMpcDIpAddr( void );                           // ȡ��һ��Mpc��Ip( ������)
    u16    GetMpcDPort( void );                             // ȡ��һ��Mpc��Port
    u16    GetLocalLayer( u8& byLayer );                    // �������
    u16    GetLocalSlot( u8& bySlot );                      // �����ۺ�
    u16    SetSystemTime( time_t tTime );                   // ����ϵͳʱ��
    BOOL32 IsHaveOtherMpc( void );                          // �Ƿ�����������Mpc��

    u8     GetPeriEqpType( u8 byEqpId );                    // ȡ��������
    u8     GetBrdChoice( u8 byLayer, u8 bySlot, u8 byType );// ȡ��������ѡ��
    u32    GetBrdIpAddr( u8 byBrdId );                      // ȡ����Ip(������)
    u32    GetBoardIpAddrFromIdx( u8 byBrdId );             // ȡ����Ip(������)
    u16    RebootBoard( u8 byLayer, u8 bySlot, u8 byType ); // ����ָ������
    BOOL32 GetCriDriIsCast( u8 byBrdId );                   // ȡ�����Ƿ��鲥ѡ��
    u16    GetBoardInfo( u32 dwBrdIp, u8* pbyBrdId, u8* pbyLayer = NULL, u8* pbySlot = NULL ); // ȡ������Ϣ
      u16    GetPeriInfo( u8 byEqpId, u32* pdwEqpIp, u8* pbyEqpType );// ȡ������Ϣ
	BOOL32 IsDscReged( u8 &byDscType );		// �ж�DSC�Ƿ�ע��

    void   SetRunningMsgDst( u16 wRunningEvent, u32 dwNode );   // ����Mpc������Ϣ��
    void   SetPowerOffMsgDst( u16 wPowerOffEvent, u32 dwNode ); // ����Mpc�ػ���Ϣ��
    void   SetRebootMsgDst( u16 wRebootEvent, u32 dwNode );     // ����Mpc������Ϣ��
    
    u8     GetRunBrdTypeByIdx( u8 byBrdIdx );                   // ͨ����Ĳ�����������ѯ���а�����

    u8     GetHWVersion( void );                                // ��ѯӲ���汾��
    u8     GetCpuRate( void );									// ��ѯCpu ռ����
	u32	   GetMemoryRate(void);									// ��ѯMemory ռ����

    BOOL32 IsMcuPdtBrdMatch(u8 byPdtType) const;                // ���ҵ����MPC�������Ƿ�ƥ��
	
	void	GetIpFromU32(s8* achDstStr, u32 dwIpAddr);

	// [1/15/2010 xliang] ������ز���
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
#ifdef _8KI_
	u16		SetNewNetCfgInfo(const TNewNetCfg &tNewNetcfgInfo, const s8 bySecEthIndx = -1);	//����8KI֧��3������������ 
	u16		GetNewNetCfgInfo(TNewNetCfg &tNewNetCfgInfo);		//��ȡ8KI֧��3������������ 
	u16	    GetMacByEthIdx( const u8 &byEthIdx,s8* achMac,const u8 &byMacLen );
#endif
	u16	  SetGkProxyCfgInfo( TGKProxyCfgInfo &tGKProxyCfgInfo);
	u16   GetGkProxyCfgInfo( TGKProxyCfgInfo &tGKProxyCfgInfo );

	u16		SetSipRegGkInfo( u32 dwGKIp );							//дע��GK���ļ�
	u16   SetProxyDMZInfo( TProxyDMZInfo &tProxyDMZInfo );
	u16   GetProxyDMZInfo( TProxyDMZInfo &tProxyDMZInfo );
	u16	  GetProxyDMZInfoFromMcuCfgFile( TProxyDMZInfo &tProxyDMZInfo );
	
	u16	  SetPrsTimeSpanCfgInfo( TPrsTimeSpan &tPrsTimeSpan );
	u16	  GetPrsTimeSpanCfgInfo( TPrsTimeSpan &tPrsTimeSpan );	

	u16    GetMcuEqpCfg( TMcu8KECfg * ptMcu8KECfg );                // ���MCU�豸��Ϣ
	u16    SetMcuEqpCfg(TNetAdaptInfoAll * ptNetAdaptInfoAll);
	u16	   SetMcu8KIEqpCfg( TMcu8KECfg * ptMcuEqpCfg );   //����8KI��������[5/7/2012 chendaiwei]	

	u16	   GetMultiNetCfgInfo(TMultiNetCfgInfo &tMultiNetCfgInfo);		// ��ȡ·������
	u16	   GetMultiManuNetAccess(TMultiManuNetAccess &tMultiManuNetAccess,BOOL32 bFromMcuCfgFile = FALSE, 
								 TMultiEthManuNetAccess *tMultiEthManuNetAccess = NULL, u8 *byEthNum = NULL);	//��ȡ����Ӫ����������
	u16	   WriteMultiManuNetAccess(const TMultiManuNetAccess &tMultiManuNetAccess, 
								   const TMultiEthManuNetAccess *tMultiEthManuNetAccess = NULL, 
								   const u8 byEthNum = 0);
	BOOL32 GetGkProxyCfgInfoFromCfgFile ( TGKProxyCfgInfo &tgkProxyCfgInfo );
	u16    SetRouteCfg(TMultiNetCfgInfo &tMultiNetCfgInfo);				// ����·��
	u16 SetMultiManuNetCfg(const TMultiManuNetCfg &tMultiManuNetCfg, 
						   const TMultiEthManuNetAccess *tMultiEthManuNetAccess = NULL,
						   const u8 byEthNum = 0);	// ���ö���Ӫ����������

//	u16   SaveMcuEqpCfgToSys( TMcu8KECfg * ptMcu8KECfg);			//��������Ϣˢ�µ�ϵͳ������
	u16	  Save8KENetCfgToSys( TMcu8KECfg * ptMcu8KECfg);			//��������Ϣˢ�µ�ϵͳ������
	void  SetRouteToSys(void);
	u8	  SearchPortChoice(u32 dwIpAddr = 0);						//���ҵ�ǰʹ�õ�����
#endif

#ifdef _LINUX_
	u32 EthIdxSys2Hard( u32 dwSysIdx );
#endif
#ifdef _MINIMCU_
    BOOL32 IsMcu8000BHD() const;
#endif    
	
	u16   SetVCSSoftName( s8* pachSoftName );
	void  GetVCSSoftName( s8* pachSoftName );

	//  [1/8/2010 pengjie] Modify ������չ����
	u16   SetEqpExCfgInfo( TEqpExCfgInfo &tEqpExCfgInfo );
	u16   GetEqpExCfgInfo( TEqpExCfgInfo &tEqpExCfgInfo );

	//lukp [2/22/2010]������ñ���ʱ��
	u16   SetMcuCompileTime(s8 *pszCompileTime);

	u16 SetAuthMTNum(u16 wAuthMTNum);
	u16 SetAuthMTTotal(u16 wAuthTotal);
	u16 SetEqpStat(u8 byEqpType, u16 wEqpUsedNum, u16 wEqpTotal);
	// End Modify

	//[2011/02/11 zhushz] mcs�޸�mcu ��������	
	void GetNewNetCfg(TMcuNewNetCfg& tMcuNewNetInfo);
	void SetNewNetCfg(const TMcuNewNetCfg& tMcuNewNetInfo);	
	BOOL32 IsMpcNetCfgBeModifedByMcs(void);
	BOOL32 SetIsNetCfgBeModifed(BOOL32 bNetInfoBeModify);

    //[12/13/2011 chendaiwei]����vmp����Id��ȡ����MPU2�������
	u8 GetMPU2TypeByVmpEqpId(u8 byVmpEqpId);

	u8 GetMPU2TypeByEqpId(u8 byEqpId,u8 &byBrdId);
	BOOL32 GetMpu2AnotherEqpId(u8 byEqpId,u8 &byAnotherId);

	BOOL32 GetAPU2BasEqpIdByMixerId(u8 byMixerEqpId,u8 &byBasId);

	// ��ȡΪ�ض�Eqp���������MCU�������ʼ�˿� [12/13/2011 chendaiwei]
	u16	GetPeriEqpMcuRecvStartPort(u8 byEqpType, u8 bySubEqpType = 0);
	// ��ʼ������MCU����ʼ�˿�ֵ [12/13/2011 chendaiwei]
	void ClearAllPeriEqpMcuRecvPort(void);
	//zhouyiliang 20110208 ����������־λ
	void SetMpcUpateStatus(s32 dwStat = 1 );
	
	//��ȡ������������[2/13/2012 chendaiwei]
	BOOL32 GetMixerSubTypeByEqpId(u8 byEqpId, u8 &byMixertype);
	BOOL32 GetHduSubTypeByEqpId(u8 byEqpId, u8 &byHduSubtype);
	BOOL32 GetMixerSubTypeByRunBrdId(u8 byBrdId, u8 & byMixertype);
	BOOL32 IsNeedRebootAllMpuBas( void );
	void   SetRebootAllMpuBasFlag( BOOL32 bNeedReboot);
	u8 GetBrdIdbyIpAddr(u32 dwIpAddr);
	u8 GetCurEthChoice ( void );

#ifdef _8KH_
	void SetMcuType800L( void );
	void SetMcuType8000Hm(void);
	BOOL32 Is800LMcu( void );
	BOOL32 Is8000HmMcu( void );
#endif

    void AdjustMcuRasPort(u32 wMpcIp, u32 wGkIp);

	void AdjustNewMcuRasPort(u32 dwMcuIp, u32 dwGkIp, u32 dwSipIp);
private:
	// ������MCU�������ʼ�˿� [12/13/2011 chendaiwei]
	u16 m_wMixerMcuRecvStartPort;
	u16 m_wRecMcuRecvStartPort;
	u16 m_wBasMcuRecvStartPort;
	u16	m_wVmpMcuRecvStartPort;
	u8  m_byIsNeedRebootAllMPUBas; // �Ƿ���Ҫ��������MPU(bas)���� [5/3/2012 chendaiwei]
};

API void mcuagtver( void );
API void mcuagenthelp( void );
API u32  BrdGetDstMcuNode( void );
API u32  BrdGetDstMcuNodeB( void );

#endif  // _AGENT_INTERFACE_H_

