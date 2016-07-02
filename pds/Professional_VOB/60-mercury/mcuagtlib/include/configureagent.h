/*****************************************************************************
   ģ����      : Mcu Agent
   �ļ���      : cfgagent.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: �����ļ��Ķ�ȡ
   ����        : liuhuiyun
   �汾        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���          �޸�����
   2005/08/17  1.0         liuhuiyun       ����
   2006/05/20  4.0         liuhuiyun       �Ż�����
   2006/11/02  4.0         �ű���          ���ݶ�ȡ�㷨�Ż�
******************************************************************************/
#ifndef _AGENT_CONFIGURE_READ_
#define _AGENT_CONFIGURE_READ_

#include "osp.h"
#include "brdmanager.h"
#include "mcustruct.h"
#include "mcuagtstruct.h"
#include "agentcommon.h"
#include "agtcomm.h"
#include "snmpadp.h"
#include "evagtsvc.h"
#include "mcuver.h"
#include "brdwrapperdef.h"
#include "mcustruct.h"
#include "mcuinnerstruct.h"
#include "commonlib.h"
#include "brdctrl.h"


#ifdef _VXWORKS_
#include "brddrvlib.h"
#endif

// MPC2 ֧��
#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
        #include "brdwrapperdef.h"
        #include "nipwrapper.h"
        #include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
#endif

#ifdef WIN32
#include "stdlib.h"
#include "winbrdwrapper.h"
#pragma  once
#endif


class CSemOpt;
class CAlarmProc;


#define ENTER( hSem) \
	CCfgSemOpt cSemOpt( hSem );

class CCfgAgent{

public:
    CCfgAgent();
    virtual ~CCfgAgent();

public: 

// [0].������ں���
    BOOL32 AgentInit( u8 byMcuType );						        // ��ʼ��������
    BOOL32 AgentReadConfig( void );							        // �������ļ�
    void   AgentReadDebugInfo( void );                              // ��ȡDebug�ļ�
    void   AgentDirCreate( const s8* pPathName );			        // ����Ŀ¼
    void   SetDefaultConfig( void );							    // ���ó�Ĭ������
    u16    PretreatConfigureFile( s8* achFileName, u8 byMpcType );  // û�������ļ�ʱ����Ĭ�������ļ�
    
// [1].����������ļ��е�ĳ����    
    BOOL32 AgentGetSystemInfo( const s8* lpszProfileName );         // ��ϵͳ��Ϣ
    BOOL32 AgentGetLocalInfo( const s8* lpszProfileName );          // ��������Ϣ
    BOOL32 AgentGetNetworkInfo( const s8* lpszProfileName );        // ��������Ϣ
    BOOL32 AgentGetTrapRcvTable( const s8* lpszProfileName );       // ��Trap��������Ϣ
    BOOL32 AgentGetBrdCfgTable( const s8* lpszProfileName );        // ��������Ϣ
    BOOL32 AgentGetEqpDcs( const s8* lpszProfileName );             // �����ݻ����������Ϣ
    BOOL32 AgentGetPrsInfo( const s8* lpszProfileName );            // ��Prs��Ϣ
    BOOL32 AgentGetNecSync( const s8* lpszProfileName );            // ����ͬ����Ϣ
    BOOL32 AgentGetMixerTable( const s8* lpszProfileName );         // ����������Ϣ
    BOOL32 AgentGetRecorderTable( const s8* lpszProfileName );      // ��¼����Ϣ    
    BOOL32 AgentGetTVWallTable( const s8* lpszProfileName );        // ������ǽ��Ϣ
    BOOL32 AgentGetBasTable( const s8* lpszProfileName );           // ��Bas��Ϣ
    //zw[08/07/2008]
    BOOL32 AgentGetBasHDTable( const s8* lpszProfileName );         // ��BasHD��Ϣ
    //4.6�汾�¼� jlb
    BOOL32 AgentGetHduTable( const s8* lpszProfileName );         // ��Hdu��Ϣ
    BOOL32 AgentGetSvmpTable( const s8* lpszProfileName );         // ��Svmp��Ϣ
//    BOOL32 AgentGetDvmpTable( const s8* lpszProfileName );         // ��Dvmp��Ϣ
    BOOL32 AgentGetMpuBasTable( const s8* lpszProfileName );         // ��MpuBas��Ϣ
//    BOOL32 AgentGetEbapTable( const s8* lpszProfileName );         // ��Ebap��Ϣ
//    BOOL32 AgentGetEvpuTable( const s8* lpszProfileName );         // ��Evpu��Ϣ
    BOOL32 AgentGetHduSchemeTable( const s8* lpszProfileName );     // ��ȡHDUԤ��
    BOOL32 AgentGetVrsRecorderTable( const s8* lpszProfileName );	// ��ȡVrs��¼����Ϣ
    

    BOOL32 AgentGetVMPTable( const s8* lpszProfileName );           // ��Vmp��Ϣ
    BOOL32 AgentGetQosInfo( const s8* lpszProfileName );            // ��Qos��Ϣ	
    BOOL32 AgentGetVmpAttachTable( const s8* lpszProfileName );     // ������ϳɷ��
    BOOL32 AgentGetMpwTable( const s8* lpszProfileName );           // ���໭�����ǽ
#ifdef _MINIMCU_
	BOOL32 AgentGetDSCInfo( const s8* lpszProfileName );            // ��8000B��DSC��������Ϣ
#endif
	BOOL32 AgentGetVCSSoftName( const s8* lpszProfileName );

	BOOL32 AgentGetGkProxyCfgInfo( const s8* lpszProfileName );		
	BOOL32 AgentGetPrsTimeSpanCfgInfo(const s8* lpszProfileName );
#ifdef _8KI_
	BOOL32 AgentGetNewNetCfgInfo( const s8* lpszProfileName );	//��ȡ8KI������Ϣ
	void SetNewNetCfgInfoByDefault();
	void SwitchNewNetCfgToMcuEqpCfg( const TNewNetCfg &tNewNetCfg, TMcu8KECfg &tMcuEqpCfg);	//TNewNetCfgת��ΪtMcuEqpCfg
#endif
// [2].�������û�
    u16    ReadTrapTable( u8* pbyTrapNum, TTrapInfo* ptTrapTable ); // �����TRAP��
    u16    WriteTrapTable( u8 byTrapNum,  TTrapInfo* ptTrapTable ); // ����дTRAP��
    u16    ReadBrdTable( u8* pbyBrdNum, TBoardInfo* ptBoardTable ); // ����������
    u16    WriteBrdTable( u8 byBrdNum,  TBoardInfo* ptBoardTable ); // ����д�����
    u16    WriteBoardTable( u8 byIndex, TBoardInfo* ptBoardTable, BOOL32 bSemTake = FALSE ); // д�����������
	u16    OrganizeBoardTable(u8 byIndex,  TBoardInfo* ptBoardTable,   s8* ptInfoTobeWrited, BOOL32 bSemTake = FALSE ); // �����������
    u16    ReadMixerTable( u8* pbyMixNum, TEqpMixerInfo* ptTable ); // �����Mix��
	//��ʶ�Ƿ���Ҫ�����ڴ��еĶ˿���Ϣ[12/15/2011 chendaiwei]
    u16    WriteMixerTable( u8 byMixNum,  TEqpMixerInfo* ptTable,BOOL32 bIsNeedUpdatePortInTable = TRUE); // ����дMix��
    u16    ReadTvTable( u8* pbyTvNum, TEqpTVWallInfo* ptTvTable );  // �����TVWall��
    u16    WriteTvTable( u8 byTvNum,  TEqpTVWallInfo* ptTvTable );  // ����дTVWall��
    u16    ReadRecTable( u8* pbyRecNum, TEqpRecInfo* ptRecTable );  // �����Rec��
    u16    WriteRecTable( u8 byRecNum,  TEqpRecInfo* ptRecTable,BOOL32 bIsNeedUpdatePortInTable = TRUE );  // ����дRec��
    u16    ReadBasTable( u8* pbyBasNum, TEqpBasInfo* ptBasTable );  // �����Bas��
    u16    WriteBasTable( u8 byBasNum,  TEqpBasInfo* ptBasTable,BOOL32 bIsNeedUpdatePortInTable = TRUE );  // ����дBas��
    //zw[08/06/2008]    
    u16    ReadBasHDTable( u8* pbyBasNum, TEqpBasHDInfo* ptBasHDTable );  // �����BasHD��
    u16    WriteBasHDTable( u8 byBasNum,  TEqpBasHDInfo* ptBasHDTable,BOOL32 bIsNeedUpdatePortInTable = TRUE );  // ����дBasHD��

    u16    ReadVmpTable( u8* pbyVmpNum, TEqpVMPInfo* ptVmpTable );  // �����VMP��
    u16    WriteVmpTable( u8 byVmpNum,  TEqpVMPInfo* ptVmpTable,BOOL32 bIsNeedUpdatePortInTable = TRUE );  // ����дVMP��
    u16    ReadMpwTable( u8* pbyMpwNum, TEqpMpwInfo* ptMpwTable );  // �����MPW��
    u16    WriteMpwTable( u8 byMpwNum,  TEqpMpwInfo* ptMpwTable );  // ����дMPW��
	//4.6�汾 �¼�����
	u16    ReadHduTable( u8* pbyHduNum, TEqpHduInfo* ptHduTable );  // �����Hdu��
    u16    WriteHduTable( u8 byHduNum,  TEqpHduInfo* ptHduTable );  // ����дHdu��
	u16    ReadSvmpTable( u8* pbySvmpNum, TEqpSvmpInfo* ptSvmpTable );  // �����Svmp��
    u16    WriteSvmpTable( u8 bySvmpNum,  TEqpSvmpInfo* ptSvmpTable,BOOL32 bIsNeedUpdatePortInTable = TRUE );  // ����дSvmp��
// 	u16    ReadDvmpTable( u8* pbyDvmpNum, TEqpDvmpBasicInfo* ptDvmpTable );  // �����Dvmp��      u16    WriteDvmpTable( u8 byDvmpNum,  TEqpDvmpBasicInfo* ptDvmpTable );  // ����дDvmp��
 	u16    ReadMpuBasTable( u8* pbyMpuBasNum, TEqpMpuBasInfo* ptMpuBasTable );  // �����MpuBas��
    u16    WriteMpuBasTable( u8 byMpuBasNum,  TEqpMpuBasInfo* ptMpuBasTable,BOOL32 bIsNeedUpdatePortInTable = TRUE );  // ����дMpuBas��
// 	u16    ReadEbapTable( u8* pbyEbapNum, TEqpEbapInfo* ptEbapTable );  // �����Ebap��
//  u16    WriteEbapTable( u8 byEbapNum,  TEqpEbapInfo* ptEbapTable );  // ����дEbap��
//	u16    ReadEvpuTable( u8* pbyEvpuNum, TEqpEvpuInfo* ptEvpuTable );  // �����Evpu��
//  u16    WriteEvpuTable( u8 byEvpuNum,  TEqpEvpuInfo* ptEvpuTable );  // ����дEvpu��
    u16    ReadHduSchemeTable( u8* pbyNum, THduStyleInfo* ptCfg );  // �����HduԤ����
    u16    WriteHduSchemeTable( u8 byNum,  THduStyleInfo* ptCfg );  // ����дHduԤ����
	u16    ReadVrsRecTable( u8* pbyVrsRecNum, TEqpVrsRecCfgInfo* ptVrsRecTable );  // �����Vrs��¼����
	u16    WriteVrsRecTable( u8 byVrsRecNum, TEqpVrsRecCfgInfo* ptVrsRecTable );  // ����дVrs��¼����

    u16    ReadVmpAttachTable( u8* pbyNum, TVmpAttachCfg* ptCfg );  // ���������ϳɷ���
    u16    WriteVmpAttachTable( u8 byNum,  TVmpAttachCfg* ptCfg );  // ����д����ϳɷ���
    u16    ReadPrsTable( u8* pbyPrsNum, TEqpPrsInfo* ptPrsTable );  // �����Prs��
    u16    WritePrsTable( u8 byPrsNum,  TEqpPrsInfo* ptPrsTable );  // ����дPrs��
	BOOL32 RefreshBrdOsType(u8 byBrdId, u8 byOsType);               // ����ĳ������忨��OSTYPEֵ

	// lang����mcu��ǰ���뷽ʽ [pengguofeng 4/16/2013]
	u16    ReadMcuEncodingType(u8 &byEncoding);
	BOOL32 WriteMcuEncodingType(const u8 *pbyEncoding);
	
	// lang [pengguofeng 4/12/2013]
	void SetEncodingType( u8 byEncoding);
	u8	GetEncodingType(void);
	BOOL32	IsUtf8Encoding(void);
	// end [pengguofeng 4/16/2013]

    BOOL32 IsSVmp( u8 byEqpId );
	u16     GetVmpEqpVersion(u8 byEqpId);
	
//    BOOL32 IsDVmp( u8 byEqpId );
//    BOOL32 IsEVpu( u8 byEqpId );
    BOOL32 IsMpuBas( u8 byEqpId );
	u16     GetBasEqpVersion(u8 byEqpId);
	u16		GetHDUEqpVersion(u8 byEqpId);
//    BOOL32 IsEBap( u8 byEqpId );

	BOOL32 GetMpuBasEntry( u8 byEqpId, TEqpMpuBasEntry &tMpuBasEntry);  //��ȡ��Ӧ����id��mpubas��Ϣ
	u8     GetIS22BrdIndexByPos(u8 byLayer, u8 bySlot);
	u8	   GetIdlePRSEqpId();	  // ��ȡ��ǰ��С���п��õ�PRS����ID[9/15/2011 chendaiwei]
	// mcu start optimize, zgc, 2007-02-27
// [3] �����ļ����ݵĵ��뵼��

	//[3.1] �ļ����帴��
	BOOL32 PreReadCfgfile( const s8* lpszCfgfilename );
	BOOL32 CopyCfgSrcfileToDstfile( const s8* lpszSrcfilename, const s8* lpszDstfilename );
	BOOL32 CopyFileBetweenCfgAndBak( const s8* lpszSrcfilename, const s8* lpszDstfilename );
	BOOL32 WriteAllCfgInfoToCfgfile();
	
	//[3.2] Ĭ������
	void SetSystemInfoByDefault();         // ϵͳ��Ϣ
    void SetLocalInfoByDefault();          // ������Ϣ
    void SetNetWorkInfoByDefault();        // ������Ϣ
    void SetTrapRcvTableByDefault();       // Trap��������Ϣ
    void SetBrdCfgTableByDefault();        // ������Ϣ
    void SetEqpDcsByDefault();             // ���ݻ����������Ϣ
    void SetPrsInfoByDefault();            // Prs��Ϣ
    void SetNecSyncByDefault();            // ��ͬ����Ϣ
    void SetMixerTableByDefault();         // ��������Ϣ
    void SetRecorderTableByDefault();      // ¼����Ϣ    
    void SetTVWallTableByDefault();        // ����ǽ��Ϣ
    void SetBasTableByDefault();           // Bas��Ϣ
    //zw[08/07/2008]
    void SetBasHDTableByDefault();           // BasHD��Ϣ
    //4.6�汾�¼�  jlb
	void SetHduTableByDefault();           // Hdu��Ϣ
 	void SetSvmpTableByDefault();           // Svmp��Ϣ
 	void SetDvmpTableByDefault();           // Dvmp��Ϣ
 	void SetMpuBasTableByDefault();           // MpuBas��Ϣ
	void SetEbapTableByDefault();           // Ebap��Ϣ
	void SetEvpuTableByDefault();           // Evpu��Ϣ
    void SetVrsRecorderTableByDefault();    // Vrs��¼����Ϣ


    void SetVMPTableByDefault();           // Vmp��Ϣ
    void SetQosInfoByDefault();            // Qos��Ϣ	
    void SetVmpAttachTableByDefault();     // ����ϳɷ��
    void SetMpwTableByDefault();           // �໭�����ǽ
	void SetEqpExCfgInfoByDefault();      // ������չ������Ϣ
#ifdef _MINIMCU_
	void SetDscInfoByDefault();			   // Dsc��Ϣ
#endif

	void SetGkProxyCfgInfoByDefault();
	void SetPrsTimeSpanCfgInfoByDefault();  
//end, zgc, 2007-02-27

// [4].������Ϣ�Ķ�д
    u16 GetEqpRecCfgById( u8 byId, TEqpRecInfo * ptRecCfg );        // ������IDȡ¼�����Ϣ
    u16 SetEqpRecCfgById( u8 byId, TEqpRecInfo * ptRecCfg );        // ������ID��¼�����Ϣ
    u16 GetEqpTVWallCfgById( u8 byId, TEqpTVWallInfo * ptTWCfg );   // ������IDȡ����ǽ��Ϣ
    u16 SetEqpTVWallCfgById( u8 byId, TEqpTVWallInfo * ptTWCfg );   // ������ID�����ǽ��Ϣ
    u16 GetEqpMixerCfgById( u8 byId, TEqpMixerInfo * ptMixerCfg );  // ������IDȡ��������Ϣ
    u16 SetEqpMixerCfgById( u8 byId, TEqpMixerInfo tMixerCfg );     // ������ID���������Ϣ
    u16 GetEqpBasCfgById( u8 byId, TEqpBasInfo * ptBasCfg );        // ������IDȡBas��Ϣ
    u16 SetEqpBasCfgById( u8 byId, TEqpBasInfo * ptBasCfg );        // ������ID��Bas��Ϣ

    //zw[08/06/2008] 
    BOOL32 IsEqpBasHD( u8 byId );                                   // ������ID�ж��Ƿ����bas
	BOOL32 IsEqpBasAud(u8 byEqpId);									//�ж�BAS�Ƿ�����Ƶ�����BAS

    u16 GetEqpBasHDCfgById( u8 byId, TEqpBasHDInfo * ptBasHDCfg );  // ������IDȡBasHD��Ϣ
    u16 SetEqpBasHDCfgById( u8 byId, TEqpBasHDInfo * ptBasHDCfg );  // ������ID��BasHD��Ϣ
    u16 GetMpuBasCfgById( u8 byId,TEqpMpuBasInfo* ptEqpMpuBasInfo );  // ������IDȡBasHD��Ϣ

    u16 GetEqpVMPCfgById( u8 byId, TEqpVMPInfo * ptVMPCfg );        // ������IDȡVmp��Ϣ
    u16 SetEqpVMPCfgById( u8 byId, TEqpVMPInfo * ptVMPCfg );        // ������ID��Vmp��Ϣ
    u16 GetEqpMpwCfgById( u8 byId, TEqpMpwInfo * ptMpwCfg );        // ������IDȡMpw��Ϣ
    u16 SetEqpMpwCfgById( u8 byId, TEqpMpwInfo * ptMpwCfg );        // ������ID��Mpw��Ϣ
    u16 GetEqpPrsCfgById( u8 byId, TEqpPrsInfo * ptPrsCfg );        // ������IDȡPRS��Ϣ
    u16 SetEqpPrsCfgById( u8 byId, TEqpPrsInfo * ptPrsCfg );        // ������ID��PRS��Ϣ

	//4.6�汾 �¼�����  jlb
    u16 GetEqpHduCfgById( u8 byId, TEqpHduInfo * ptHduCfg );        // ������IDȡHdu��Ϣ
    u16 SetEqpHduCfgById( u8 byId, TEqpHduInfo * ptHduCfg );        // ������ID��Hdu��Ϣ
    u16 GetEqpSvmpCfgById( u8 byId, TEqpSvmpInfo * ptSvmpCfg );     // ������IDȡSvmp��Ϣ
    u16 SetEqpSvmpCfgById( u8 byId, TEqpSvmpInfo * ptSvmpCfg );     // ������ID��Svmp��Ϣ
//  u16 GetEqpDvmpCfgById( u8 byId, TEqpDvmpBasicInfo * ptDvmpCfg );        // ������IDȡDvmp��Ϣ
//  u16 SetEqpDvmpCfgById( u8 byId, TEqpDvmpBasicInfo * ptDvmpCfg );        // ������ID��Dvmp��Ϣ
    u16 GetEqpMpuBasCfgById( u8 byId, TEqpMpuBasInfo * ptMpuBasCfg );       // ������IDȡMpuBas��Ϣ
    u16 SetEqpMpuBasCfgById( u8 byId, TEqpMpuBasInfo * ptMpuBasCfg );   // ������ID��MpuBas��Ϣ
//     u16 GetEqpEbapCfgById( u8 byId, TEqpEbapInfo * ptEbapCfg );        // ������IDȡEbap��Ϣ
//     u16 SetEqpEbapCfgById( u8 byId, TEqpEbapInfo * ptEbapCfg );        // ������ID��Ebap��Ϣ
//     u16 GetEqpEvpuCfgById( u8 byId, TEqpEvpuInfo * ptEvpuCfg );        // ������IDȡEvpu��Ϣ
//     u16 SetEqpEvpuCfgById( u8 byId, TEqpEvpuInfo * ptEvpuCfg );        // ������ID��Evpu��Ϣ

    u16 GetBrdCfgById( u8 byBrdId, TBoardInfo * ptBoardInfo );      // ������IDȡ������Ϣ
    u16 SetBrdCfgById( u8 byBrdId, TBoardInfo * ptBoardInfo );      // ������ID�赥����Ϣ

    u16 GetEqpRecCfgByRow( u8 byRow, TEqpRecInfo * ptRecCfg );      // �������к�ȡ¼�����Ϣ
		//BOOL32 bIsNeedUpdatePortInTable ��ʶ�Ƿ���Ҫ�����ڴ���ж˿�[12/15/2011 chendaiwei]
    u16 SetEqpRecCfgByRow( u8 byRow, TEqpRecInfo * ptRecCfg, BOOL32 bIsNeedUpdatePortInTable = TRUE );      // �������к���¼�����Ϣ
    u16 GetEqpTVWallCfgByRow( u8 byRow, TEqpTVWallInfo * ptTWCfg ); // �������к�ȡ����ǽ��Ϣ
    u16 SetEqpTVWallCfgByRow( u8 byRow, TEqpTVWallInfo * ptTWCfg ); // �������к������ǽ��Ϣ
    u16 GetEqpMixerCfgByRow( u8 byRow, TEqpMixerInfo * ptMixerCfg );// �������к�ȡ��������Ϣ
	//BOOL32 bIsNeedUpdatePortInTable ��ʶ�Ƿ���Ҫ�����ڴ���ж˿�[12/15/2011 chendaiwei]
    u16 SetEqpMixerCfgByRow( u8 byRow, TEqpMixerInfo tMixerCfg, BOOL32 bIsNeedUpdatePortInTable = TRUE );   // �������к����������Ϣ
    u16 GetEqpBasCfgByRow( u8 byRow, TEqpBasInfo * ptBasCfg );      // �������к�ȡBas��Ϣ
	//BOOL32 bIsNeedUpdatePortInTable ��ʶ�Ƿ���Ҫ�����ڴ���ж˿�[12/15/2011 chendaiwei]
    u16 SetEqpBasCfgByRow( u8 byRow, TEqpBasInfo * ptBasCfg,BOOL32 bIsNeedUpdatePortInTable = TRUE );      // �������к���Bas��Ϣ

    //zw[08/07/2008]
    u16 GetEqpBasHDCfgByRow( u8 byRow, TEqpBasHDInfo * ptBasHDCfg );// �������к�ȡBasHD��Ϣ
	//BOOL32 bIsNeedUpdatePortInTable ��ʶ�Ƿ���Ҫ�����ڴ���ж˿�[12/15/2011 chendaiwei]
    u16 SetEqpBasHDCfgByRow( u8 byRow, TEqpBasHDInfo * ptBasHDCfg,BOOL32 bIsNeedUpdatePortInTable = TRUE );// �������к���BasHD��Ϣ

    u16 GetEqpVMPCfgByRow( u8 byRow, TEqpVMPInfo * ptVMPCfg );      // �������к�ȡVmp��Ϣ
	//BOOL32 bIsNeedUpdatePortInTable ��ʶ�Ƿ���Ҫ�����ڴ���ж˿�[12/15/2011 chendaiwei]
    u16 SetEqpVMPCfgByRow( u8 byRow, TEqpVMPInfo * ptVMPCfg, BOOL32 bIsNeedUpdatePortInTable = TRUE  );      // �������к���Vmp��Ϣ
    u16 GetEqpMpwCfgByRow( u8 byRow, TEqpMpwInfo * ptMpwCfg );      // �������к�ȡMpw��Ϣ
    u16 SetEqpMpwCfgByRow( u8 byRow, TEqpMpwInfo * ptMpwCfg );      // �������к���Mpw��Ϣ
    u16 GetEqpPrsCfgByRow( u8 byRow, TEqpPrsInfo * ptPrsCfg );      // �������к�ȡPRS��Ϣ
    u16 SetEqpPrsCfgByRow( u8 byRow, TEqpPrsInfo * ptPrsCfg );      // �������к���PRS��Ϣ  

    //4.6�汾 �¼�����  jlb
    u16 GetEqpHduCfgByRow( u8 byRow, TEqpHduInfo * ptHduCfg );      // �������к�ȡHdu��Ϣ
    u16 SetEqpHduCfgByRow( u8 byRow, TEqpHduInfo * ptHduCfg );      // �������к���Hdu��Ϣ
	u16 GetEqpSvmpCfgByRow( u8 byRow, TEqpSvmpInfo * ptSvmpCfg );      // �������к�ȡSvmp��Ϣ
	//BOOL32 bIsNeedUpdatePortInTable ��ʶ�Ƿ���Ҫ�����ڴ���ж˿�[12/15/2011 chendaiwei]
    u16 SetEqpSvmpCfgByRow( u8 byRow, TEqpSvmpInfo * ptSvmpCfg,BOOL32 bIsNeedUpdatePortInTable = TRUE  );      // �������к���Svmp��Ϣ  
//  u16 GetEqpDvmpCfgByRow( u8 byRow, TEqpDvmpBasicInfo * ptDvmpCfg );      // �������к�ȡDvmp��Ϣ
//  u16 SetEqpDvmpCfgByRow( u8 byRow, TEqpDvmpBasicInfo * ptDvmpCfg );      // �������к���Dvmp��Ϣ  
    u16 GetEqpMpuBasCfgByRow( u8 byRow, TEqpMpuBasInfo * ptMpuBasCfg );     // �������к�ȡMpuBas��Ϣ
	//BOOL32 bIsNeedUpdatePortInTable ��ʶ�Ƿ���Ҫ�����ڴ���ж˿�[12/15/2011 chendaiwei]
    u16 SetEqpMpuBasCfgByRow( u8 byRow, TEqpMpuBasInfo * ptMpuBasCfg,BOOL32 bIsNeedUpdatePortInTable = TRUE  );     // �������к���MpuBas��Ϣ  
//     u16 GetEqpEbapCfgByRow( u8 byRow, TEqpEbapInfo * ptEbapCfg );      // �������к�ȡEbap��Ϣ
//     u16 SetEqpEbapCfgByRow( u8 byRow, TEqpEbapInfo * ptEbapCfg );      // �������к���Ebap��Ϣ
//	u16 GetEqpEvpuCfgByRow( u8 byRow, TEqpEvpuInfo * ptEvpuCfg );      // �������к�ȡEvpu��Ϣ
//  u16 SetEqpEvpuCfgByRow( u8 byRow, TEqpEvpuInfo * ptEvpuCfg );      // �������к���Evpu��Ϣ  
    u16 GetEqpVrsRecCfgByRow( u8 byRow, TEqpVrsRecCfgInfo * ptVrsRecCfg );      // �������к�ȡVrs��¼����Ϣ
    u16 SetEqpVrsRecCfgByRow( u8 byRow, TEqpVrsRecCfgInfo * ptVrsRecCfg );      // �������к���Vrs��¼����Ϣ

    
    u16 GetBrdInfoByRow( u8 byRow, TEqpBrdCfgEntry * ptBrdCfgEnt, BOOL32 bSnmp = FALSE ); // �������к�ȡ������Ϣ
    u16 SetBrdInfoByRow( u8 byRow, TEqpBrdCfgEntry * ptBrdCfgEnt ); // �������к��赥����Ϣ
    u16 GetBrdInfoById( u8 byBrdId, TEqpBrdCfgEntry * ptBrdCfgEnt );// ������IDȡ������Ϣ
    u16 SetBrdInfoById( u8 byBrdId, TEqpBrdCfgEntry * ptBrdCfgEnt );// ������ID�赥����Ϣ
    u16 GetLocalMpcInfo( TEqpBrdCfgEntry * ptBrdCfgEnt);            // ȡ�Զ�mcu��TEqpBrdCfgEntry��Ϣ
	
	u8		GetBrdIdxFrmId(u8 byBrdId);
	u8		GetBrdIdFrmIdx(u8 byBrdIdx);
	u8      GetEqpSwitchBrdId(u8 byEqpId);
	BOOL32  SetEqpSwitchBrdId(u8 byEqpId, u8 bySwitchBrdId);
	
    u16 SetQosInfo( TQosInfo * ptQosInfo ) ;                        // �޸�Qos��Ϣ
    u16 GetQosInfo( TQosInfo * ptQosInfo );                         // ȡQos��Ϣ
    u16 SetLocalInfo( TLocalInfo * ptLocalInfo,u32 dwNodeValue = 0) ;                  // �޸ı�����Ϣ
    u16 GetLocalInfo( TLocalInfo * ptLocalInfo );                   // ȡ������Ϣ
    u16 SetTrapInfo( u8 byRow, TTrapInfo * ptTrapInfo );			// �޸ĵ���Trap��������Ϣ
    u16 GetTrapInfo( u8 byRow, TTrapInfo * ptTrapInfo );			// ȡ����Trap��������Ϣ
    u16 GetTrapInfo( TTrapInfo* ptTrapInfo );                       // ȡ����Trap����Ϣ
    u16 SetNetWorkInfo( TNetWorkInfo * ptNetWorkInfo,u32 dwNodeValue = 0 );             // �޸�����������Ϣ
    u16 GetNetWorkInfo( TNetWorkInfo * ptNetWorkInfo );             // ȡ����������Ϣ
    u16 SetNetSyncInfo( TNetSyncInfo * ptNetSyncInfo );             // �޸���ͬ����Ϣ
    u16 GetNetSyncInfo( TNetSyncInfo * ptNetSyncInfo );             // ȡ��ͬ����Ϣ
	u16 SetLoginInfo( TLoginInfo *ptLoginInfo );					// �޸ĵ�½��Ϣ
	u16 GetLoginInfo( TLoginInfo *ptLoginInfo );					// ȡ��½��Ϣ

    u16 GetSystemInfo( TMcuSystem * ptMcuSysInfo );                 // ȡϵͳ��Ϣ
    u16 SetSystemInfo( TMcuSystem * ptMcuSysInfo );                 // �޸�ϵͳ��Ϣ

    u16 GetMcuPfmInfo( TMcuPfmInfo * ptMcuPfmInfo );                 // ��ȡmcu Performance Info
    //u16 SetMcuPfmInfo( TMcuPfmInfo * ptMcuPfmInfo );                 // ����mcu Performance Info
	TMcuPfmInfo* GetMcuPfmInfo();

	u16 GetMcuUpdateInfo( TMcuUpdateInfo * ptMcuUpdateInfo );
	u16 SetMcuUpdateInfo( TMcuUpdateInfo * ptMcuUpdateInfo );
	TMcuUpdateInfo* GetMcuUpdateInfo();

#ifdef _MINIMCU_
    u16 GetDSCInfo( TDSCModuleInfo * ptDscInfo );					// ȡ8000B��dsc����������Ϣ
	u16 SetDSCInfo( TDSCModuleInfo * ptDscInfo, BOOL32 bToFile );   // �޸�8000B��dsc����������Ϣ
	u16 GetLastDscInfo( TDSCModuleInfo * ptDscInfo );				// ȡ������޸�֮ǰ��DSC����������Ϣ
	u16 SetLastDscInfo( TDSCModuleInfo * ptDscInfo );				// �����޸�֮ǰ��DSC����������Ϣ
	// ��DSC info���õ�MINIMCU MPC����, zgc, 2007-07-17
	BOOL32 SaveDscLocalInfoToNip( TDSCModuleInfo * ptDscInfo );	
		// ����·�ɵ�MINIMCU MPC����, zgc, 2007-09-21
	BOOL32 SaveRouteToNipByDscInfo( TDSCModuleInfo *ptDscInfo );	
    void SetDscInnerIp(u32 dwIp);                                   // ����dsc���������IP
	u8   GetConfigedDscType( void );								// ������ù���Dsc������ͣ���û�����ù����򷵻�0
	BOOL32 IsDscReged(u8 &byDscType);								// �ж�DSC�Ƿ�ע��	
#endif

	u8 GetWatchDogOption();		// ��ȡWATCH DOG������, zgc

	// [1/14/2010 xliang] // ��������·���������
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
#ifdef _8KI_
	u16		SetNewNetCfgInfo(const TNewNetCfg &tNewNetcfgInfo, const s8 bySecEthIndx = -1);	//����8KI֧��3������������ 
	u16		GetNewNetCfgInfo(TNewNetCfg &tNewNetCfgInfo);		//��ȡ8KI֧��3������������ 
	u16		WritePxyMultiNetInfo( void );
	void	SwitchMcuEqpCfgToNewNetCfg( TMcu8KECfg &tMcuEqpCfg, TNewNetCfg &tNewNetCfg);
#endif
	u16		SetMcu8KIEqpCfg( TMcu8KECfg * ptMcuEqpCfg ); //����8KI������� [5/7/2012 chendaiwei]
	u16		SetMcuEqpCfg( TNetAdaptInfoAll * ptNetAdaptInfoAll );		
	u16		GetMcuEqpCfg( TMcu8KECfg * ptMcuEqpCfg );                // ���MCU�豸��Ϣ
	
	void	SetRouteToSys( void );								//�������ļ���ȡ·����Ϣ����·���䵽ϵͳ�� 
	u16		SetRouteCfg(TMultiNetCfgInfo &tMultiNetCfgInfo);	//��·����Ϣ�䵽ϵͳ��
	u16		GetMultiNetCfgInfo(TMultiNetCfgInfo &tMultiNetCfgInfo);
	
	u16		SetMultiManuNetCfg(const TMultiManuNetCfg &tMultiManuNetCfg,
							   const TMultiEthManuNetAccess *tMultiEthManuNetAccess, const u8 byEthNum);
	u16     WriteMultiManuNetAccess(const TMultiManuNetAccess &tMultiManuNetAccess,
									const TMultiEthManuNetAccess *tMultiEthManuNetAccess, const u8 byEthNum);
	u16     GetMultiManuNetAccess(TMultiManuNetAccess &tMultiManuNetAccess,BOOL32 bFromMcuCfgFile = FALSE,
								  TMultiEthManuNetAccess *tMultiEthManuNetAccess = NULL, u8 *byEthNum = NULL);
	//void	Make8KINetCfgEffect( void );
	u16		Save8KENetCfgToSys(TMcu8KECfg * ptMcu8KECfg);
	BOOL32  EthInit( void );

	u16		WriteProxyIp( TNetParam tNtParam );
#endif

#ifdef _LINUX_
	u32 EthIdxSys2Hard( u32 dwSysIdx );
	u32 EthIdxHard2Sys( u32 dwHardIdx );
#endif
#ifdef WIN32
	BOOL32 AdapterDiscript2EthIdx( const s8 * pchAdapterDiscript, u32 &dwEthIdx );
	BOOL32 EthIdx2AdapterDiscript( const u32 dwEthIdx, s8 * pchAdapterDiscript );
#endif


// [5].����ȡ������Ϣ���������
    BOOL32 GetRecCfgToBrd( u8 byId, TEqpRecEntry * ptRecCfg );      // ȡ¼���
    BOOL32 GetTvCfgToBrd( u8 byId, TEqpTVWallEntry * ptTWCfg );     // ȡ����ǽ��Ϣ
    BOOL32 GetMixCfgToBrd( u8 byId, TEqpMixerEntry * ptMixCfg );    // ȡ��������Ϣ
    BOOL32 GetBasCfgToBrd( u8 byId, TEqpBasEntry * ptBasCfg );      // ȡBas��Ϣ
    BOOL32 GetVmpCfgToBrd( u8 byId, TEqpVMPEntry * ptVMPCfg );      // ȡVmp��Ϣ
    BOOL32 GetPrsCfgToBrd( u8 byId, TEqpPrsEntry * ptPrsCfg );      // ȡPrs��Ϣ
    BOOL32 GetMpwCfgToBrd( u8 byId, TEqpMPWEntry * ptMpwCfg );      // ȡMpw��Ϣ

    //4.6�汾�¼� jlb 
    BOOL32 GetHduCfgToBrd( u8 byId, TEqpHduEntry * ptHduCfg );      // ȡHdu��Ϣ
    BOOL32 GetSvmpCfgToBrd( u8 byId, TEqpSvmpEntry * ptSvmpCfg );      // ȡSvmp��Ϣ
//    BOOL32 GetDvmpCfgToBrd( u8 byId, TEqpDvmpBasicEntry * ptDvmpBasicCfg );      // ȡDvmp��Ϣ
    BOOL32 GetMpuBasCfgToBrd( u8 byId, TEqpMpuBasEntry * ptMpuBasCfg );      // ȡMpuBas��Ϣ
//    BOOL32 GetEbapCfgToBrd( u8 byId, TEqpEbapEntry * ptEbapCfg );      // ȡEbap��Ϣ
//    BOOL32 GetEvpuCfgToBrd( u8 byId, TEqpEvpuEntry * ptEvpuCfg );      // ȡEvpu��Ϣ
        
// [6].�������������д

    //[6.1] snmp
    u8     GetTrapServerNum( void );								// ȡTrap��������
    void   GetTrapTarget( u8 byIndex, TTarget& tTrapTarget );		// ȡTrap��Ϣ�ṹ
    BOOL32 GetSnmpParam( TSnmpAdpParam& tParam ) const;             // ȡSNMP����
    BOOL32 HasSnmpNms( void ) const;                                // ȡ�Ƿ�����������

    //[6.2] dcs
    u32    GetDcsIp( void );										// ȡ���ֻ��������Ip
    u16    SetDcsIp( u32 dwDcsIp );								    // �������ֻ��������Ip

    //[6.3] board
	u32    GetBoardNum( void );
    u8     GetBrdId( TBrdPosition tBrdPositon );                    // ȡ����ID
    u32    GetBrdIpAddr( u8 byBrdPosId );							// �ɵ���Id�õ�����Ip
	u32	   GetBrdIpByAgtId( u8 byBrdAgtId );						// �ɵ��������õ�����Ip, zgc, 2007-03-22
    u8     GetCriDriIsCast( u8 byBrdId );                           // �ɵ���ID�õ��Ƿ��鲥 (FIXME: byBrdId �����ʶ)
    void   ChangeBoardStatus( TBrdPosition tPos, u8 byStatus );     // ���ĵ�������״̬
    BOOL32 ChangeBrdRunEqpNum( u8 byBrdId, u8 byEqpId );		    // �޸�ָ���������е�������
    BOOL32 IsConfigedBrd( TBrdPosition tBrdPositon ); 		        // �Ƿ����øõ���
    u16    GetPriInfo( u8 byEqpId, u32* pdwEqpIp, u8* pbyEqpType ); // �õ�������Ϣ
    u8    GetBoardRow( u8 byLayer, u8 bySlot, u8 byType );        // ȡ�����к�
    u16    RebootBoard( u8 byLayer, u8 bySlot, u8 byType );         // ����ָ������
    BOOL32 CheckEqpRow( u8 byType, u8 byRow, BOOL32 bHDFlag = FALSE );// ��������к� zw[08/07/2008]���һ������TUREΪ����
    LPCSTR GetBrdTypeStr( u8 byType );			                    // �õ������ַ���
    void   GetBrdSlotString( u8 bySlot, u8 byType, s8* pszStr );	// �õ����ַ���
    BOOL32 GetSlotThroughName( const s8* lpszName, u8* pbySlot );	// �Ӳ��ַ����õ��ۺ�
    BOOL32 GetTypeThroughName( const s8* lpszName, u8* pbyType );	// �������ַ����õ����ͺ�
    void   UpdateBrdSoftware( TBrdPosition tBrdPostion, CServMsg &cServMsg ); // ���������ļ�

    //[6.4] mpc
    void   GetMPCInfo( TMPCInfo *ptMPCInfo );                       // ȡMPC��Ϣ(������)
    void   SetMPCInfo( TMPCInfo tMPCInfo );                         // ��MPC��Ϣ(������)

    u32    GetLocalIp();                                            // ȡMPC���ص�ַ
    u16    SetLocalIp( u32 dwIp, u8 byInterface );                  // ��MPC���ص�ַ
    u32    GetMpcMaskIp();                                          // ȡMPC��������
    u16    SetMpcMaskIp( u32 dwIp, u8 byInterface );                // ��MPC��������
    u32    GetMpcGateway();                                         // ȡMPC����
    u16    SetMpcGateway( u32 dwIp );                               // ��MPC����
    u8     GetMpcHWVersion();                                       // ȡMPCӲ���汾��
    u8     GetCpuRate( void );										// ��ѯCpu ռ����
	u32	   GetMemoryRate(void);										// ��ѯMemory ռ����
	void   SetMcuCompileTime(s8 *pszCompileTime);					// ����mcu����ʱ��
	s8*	   GetMcuCompileTime(u16 &wLen);							// ��ȡmcu����ʱ��
	void   SetTempPortKind(u8 byPortKind);
	u8	   GetTempPortKind();

	u8   GetMac(s8 *pszBuffer);


    void   SetMpcActive( BOOL32 bEnabled );                         // ҵ������������(TRUE: ACTIVE, FALSE: STANDBY)
    BOOL32 IsMpcActive( void );                                     // �Ƿ�Ϊҵ������(TRUE: ACTIVE, FALSE: STANDBY)
    
#ifdef _MINIMCU_
    BOOL32 IsMcu8000BHD() const;
#endif

    u8     GetMcuLayerCount( void );                                // ȡmcu�ܲ���
    BOOL32 GetImageFileName( u8 byType, s8 *pszFileName );          // �õ�����image�ļ���
	BOOL32 GetBinFileName( u8 byBrdType, s8 *pszFileName );          //�õ�����bin�ļ���
    BOOL32 UpdateMpcSoftware( s8* lpszMsg, u16 wMsgLen, u8 byFileNum, u8 *pbyRet, BOOL32 bNMS = FALSE ); // ����MPC �ļ�
    BOOL32 SmUpdateMpcSoftware();

	BOOL32 Update8KXMpcSoftware( const s8* szFileName,TBrdPosition tMpcPosition,BOOL32 &bIsCheckFileError );

//#ifdef _8KE_
//	u8     Search8KEPortChoice();
//#endif
	u8	   SearchPortChoice( u32 dwIpAddr = 0 );	// zgc, 2008-05-14
	BOOL32   SaveMpcInfo( u8 byLocalMpcIdx, TEqpBrdCfgEntry *ptLocalMpc, u8 byOtherMpcIdx = MAX_BOARD_NUM, TEqpBrdCfgEntry *ptOtherMpc = NULL, BOOL32 bSemTake = FALSE ); // zgc, 2008-05-14

	void SetMpcUpateStatus(s32 dwStat = 1);//zhouyiliang 20120208 ����mpc������־�����mcs����mcu������������mcu����������־��Ϊ1



    //[6.5] system    
    void   SetSystemState( u8 byState );                            // ϵͳ״̬����    
    u16    SetSystemTime( time_t  tTime );					        // ����ϵͳʱ��
    void   SyncSystemTime( const s8* pszTime );	                    // MPC ��ʱ��ͬ��
    u16    SetSystemCfgVer( s8* achVersion );                       // ����ϵͳ���ð汾��
    void   SyncBoardTime( u8* pbyBuf );                             // ���������ʱ��ͬ��
    void   SnmpRebootBoard( TBrdPosition tBrdPostion );             // ������������
    void   BitErrorTest( TBrdPosition tBrdPostion );                // SNMPλ�������
    void   BoardSelfTest( TBrdPosition tBrdPostion );				// SNMP�����Բ�

	//֧��MCU����ʽ����
	u16    SetIsMcuConfiged( BOOL32 bIsConfiged);					// ����Mcu���ñ�ʶ
	BOOL32 IsMcuConfiged();											// �ж�Mcu���ñ�ʶ

	// ������Ϣ�ļ���
	void	SetMcuCfgInfoLevel( u8 byLevel );						// ����MCU������Ϣ����
	u8		GetMcuCfgInfoLevel();									// ���MCU������Ϣ����

    //[6.6] qos
    u8     GetQosIpType( u8* pchIpType);                 // �������ļ��е�QosIp���������ַ���ת��Ϊ��Ӧ��Qos��ֵ
    u8     CombineQosValue( u8 byCombineValue, u8 byType );         // ���Qosֵ
    BOOL32 IsValidQosIpTypeStr( u8* pchIpTypeStr );                 // ���Qos�ַ����Ƿ�Ϸ� 
    BOOL32 GetQosStrFromNum( u8 byType, u8* pchQosIpType, u8 byLen );// �����ֵõ�QosIp�����ַ���

    //[6.7] VCS
	u16    SetVCSSoftName(const s8* pachSoftName);
	void   GetVCSSoftName(s8* pachSoftName);

	u16    SetEqpExCfgInfo( TEqpExCfgInfo &tEqpExCfgInfo );
	u16    GetEqpExCfgInfo( TEqpExCfgInfo &tEqpExCfgInfo );
	BOOL32 AgentGetEqpExCfgInfo( const s8* lpszProfileName );

	//[6.8] GK/Proxy/Prs --exclude for 8000E
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	u16		SetGkProxyCfgInfo( TGKProxyCfgInfo &tGKProxyCfgInfo);
	u16		GetGkProxyCfgInfo( TGKProxyCfgInfo &tGKProxyCfgInfo );

	u16     SetProxyDMZInfo( TProxyDMZInfo &tProxyDMZInfo );
	u16     GetProxyDMZInfo( TProxyDMZInfo &tProxyDMZInfo );
	u16		GetProxyDMZInfoFromMcuCfgFile( TProxyDMZInfo &tProxyDMZInfo );

	u16		SetPrsTimeSpanCfgInfo( TPrsTimeSpan &tPrsTimeSpan );
	u16		GetPrsTimeSpanCfgInfo( TPrsTimeSpan &tPrsTimeSpan );

	u16		SetSipRegGkInfo(u32 dwGKIp);
#endif	
// [7] �����������ܺ���
public:
    BOOL32 CreateSemHandle( SEMHANDLE &hSem );						// �����ź���
    BOOL32 FreeSemHandle( SEMHANDLE &hSem );						// �ͷ��ź���
	BOOL32 FreeAllSemHandle();										// �ͷ������ź���
    void   GetIpFromU32( s8* achDstStr, u32 dwIpAddr );				// �õ�IP�ַ���
    BOOL32 TableMemoryFree( void **ppMem, u32 dwEntryNum );         // �ͷű��ڴ�
    u8     BrdStatusMcuVc2Agt(u8 byStatusIn);                       // ����״ֵ̬ ҵ��ת������
    u8     BrdStatusAgt2McuVc(u8 byStatusIn);                       // ����״ֵ̬ ����ת��ҵ��
    u8     BrdStatusHW2Agt(u8 byStatusIn);                          // ����״ֵ̬ �ײ�ת������(ֻ��������δʵ�ִ���ת���ײ㺯��)
    u8     BrdStatusAgt2NMS(u8 byStatusIn);                         // ����״ֵ̬ ����ת��NMS(ֻ��)
    u8     PanelLedStatusAgt2NMS( TEqpBrdCfgEntry& tBrdCfg, s8* pszLedStr );   // ��״ֵ̬ ����ת��NMS(ֻ��)  [2012/05/09 liaokang]
    u8     EthPortStatusAgt2NMS( TEqpBrdCfgEntry& tBrdCfg, s8* pszEthPortStr );// ����״ֵ̬ ����ת��NMS(ֻ��)[2012/05/09 liaokang]
    u8     GetBrdInstIdByPos( const TBrdPosition tBrdPos );         // ȡ�����Ӧ��ʵ����
    u8     GetRunBrdTypeByIdx (  u8 byBrdIdx );                     // ͨ����Ĳ�����������ѯ���а�����
    BOOL32 IsMcuPdtBrdMatch(u8 byPdtType) const;                    // ���ҵ����MPC�������Ƿ�ƥ��
    u8     McuSubTypeAgt2Snmp(u8 byMcuSubType);                     // MCU������ת��

	u8     GetSnmpBrdTypeFromMcu(u8 byMcuType);							// ��ȡVC����ĵ�������
    u8     GetMcuBrdTypeFromSnmp(u8 byNmsType);                            // ��ȡsnmp����ĵ�������

	u8     GetSnmpBrdTypeFromHW(u8 byHWType);							// ��ȡVC����ĵ�������
    u8     GetHWBrdTypeFromSnmp(u8 bySnmpType);                            // ��ȡsnmp����ĵ�������

	void   PreTreatMpcNIP(void);									// ���MPC��NIP�Ƿ�������IP
    void   GetHdBasCfgFromMpuBas(TEqpBasHDInfo &tHdBasInfo,
                                 TEqpMpuBasInfo &tMpuBasInfo);      // ���ֶ�MCU�ӿڵ�һ����

	// windows������ȡIp, maskIp, Gateway��ַ
    BOOL32 RegGetAdpName( s8* lpszAdpName, u16 wLen );
    BOOL32 RegSetIpAdr( s8* lpszAdpName, s8* pIpAddr, u16 wLen );
/*    BOOL32 RegGetIpAdr( s8* lpszAdpName, s8* pIpAddr, u16 wLen );*/
    BOOL32 RegSetNetGate( s8* lpszAdpName, s8* pNetGate, u16 wLen );
    BOOL32 RegGetNetGate( s8* lpszAdpName, s8* pNetGate, u16 wLen );
    BOOL32 RegSetNetMask( s8* lpszAdpName, s8* pNetMask, u16 wLen );
    BOOL32 RegGetNetMask( s8* lpszAdpName, s8* pNetMask, u16 wLen );
    
    void   GetEntryString( s8* achStr, u8 byIdx );					                                    // �õ�Entry�ַ���
    u16    WriteStringToFile( const s8* plszSectionName, const s8* plszKeyName, const s8* pszValue );   // д�ַ������ļ�
	u16    WriteIntToFile( const s8* plszSectionName, const s8* plszKeyName, const s32 sdwValue );      // д���ֵ��ļ�
    u16    WriteTableEntryNum( s8* achFileName, s8* achTableName, s8* achEntryNumStr, u8 byEntryNum );  // д�����������
    u16    WriteTableEntry( const s8* achFileName, const s8* achTableName, const s8* achEntry, const s8* achConfInfo, BOOL32 bSemTake = TRUE ); // д��������

	// [3/27/2010 xliang] expand interface
	u16		WriteStringToFile( const s8* plszFileName, const s8* plszSectionName, const s8* plszKeyName, const s8* pszValue );
	u16		WriteIntToFile( const s8* plszFileName, const s8* plszSectionName, const s8* plszKeyName, const s32 sdwValue );

	// �Զ�ȡ���ñ����������·�װ, zgc, 2007-03-20
	BOOL32 AgtGetRegKeyStringTable( const s8* lpszProfileName,      /* �ļ�����������·���� */
		                            const s8* lpszSectionName,      /* Profile�еĶ���      */ 
		                            const s8* lpszDefault,          /* ʧ��ʱ���ص�Ĭ��ֵ   */
		                            s8* *const lpszEntryArray,            /* �����ַ�������ָ��   */
		                            u32 *dwEntryNum,                /* �ַ���������Ŀ���ɹ��󷵻��ַ�������ʵ����
                                                                       Ŀ���紫�����Ŀ���������ļ�¼���贫�� */
		                            u32 dwBufSize                   /* �����ִ��ĳ��ȣ��粻����ضϣ���
                                                                       ��һ���ֽ�����'\0'   */
		                            );

    // ����Ϣ���������
    BOOL32 PostMsgToBrdMgr( const TBrdPosition &tBrdPosition, u16 wEvent, const void* pvContent = NULL, u16 wLen = 0 );

	// ͳ��MCU8000G��Cpu���ڴ��ռ���� mqs [2010-11-30]
#ifdef _LINUX_
	BOOL32 Get8KECpuInfo( T8KECpuInfo* pt8KECpuInfo );
	BOOL32 Get8KEMemoryInfo( T8KEMemInfo* pt8KEMemInfo);
#endif

public:
    void   ShowLocalInfo( void );                               // ��ʾ������Ϣ
	void   ShowNetInfo( void );                                 // ��ʾ������Ϣ
	void   ShowNetsyncInfo( void );                             // ��ʾ��ͬ����Ϣ  
    void   ShowBoardInfo( void );                               // ��ʾBoard��Ϣ
    void   ShowMixerInfo( void );                               // ��ʾMix��Ϣ
    void   ShowBasInfo( void );                                 // ��ʾBas��Ϣ
    void   ShowRecInfo( void );                                 // ��ʾRec��Ϣ
	void   ShowTvInfo( void );                                  // ��ʾTvWall��Ϣ
	void   ShowVmpInfo( void );                                 // ��ʾVmp��Ϣ
	void   ShowMpwInfo( void );                                 // ��ʾMpw��Ϣ
    void   ShowPrsInfo( void );                                 // ��ʾPrs��Ϣ
    
    //4.6�汾�¼� jlb 
    void   ShowHduInfo( void );                                 // ��ʾHdu��Ϣ
    void   ShowSvmpInfo( void );                                // ��ʾSvmp��Ϣ
    void   ShowMpuBasInfo( void );                              // ��ʾMpuBas��Ϣ
    void   ShowMauBasInfo( void );                              // ��ʾMauBas��Ϣ



	void   ShowDcsInfo( void );                                 // ��ʾDcs��Ϣ
    void   ShowQosInfo( void );                                 // ��ʾQos��Ϣ
    void   ShowVmpAttachInfo( void );                           // ��ʾVmpAttach��Ϣ
    void   ShowDscInfo( void );                                 // ��ʾ8000B DCS����Ϣ
    void   ShowSemHandleInfo( void );                           // ��ʾ�����ź��������Ϣ

public:
    void   SetPowerOffMsgDst( u16 wEvent, u32 dwDstId );        // ����Mpc�����¼���
    void   SetRebootMsgDst( u16 wEvent, u32 dwDstId );          // ����Mpc�����¼���
    void   SetRunningMsgDst( u16 wEvent, u32 dwDstId );         // ����Mpc�����¼���

	//����mcs��û�ж�dri16��������������£���ʱ�޸�����
	void /*CCfgAgent::*/AdjustConfigedBrdDRI16(TBrdPosition tBrdPosition);

	//[2011/02/11 zhushz] mcs�޸�mcu ip
	void GetNewMpcNetCfg(TMcuNewNetCfg& tMcuNewNetInfo);				//�õ�mcs���õ�����������
	void SetNewMpcNetCfg(const TMcuNewNetCfg& tMcuNewNetInfo);			// ����mcs���õ�����������
	BOOL32 SetIsNetCfgBeModifed(BOOL32 bIsNetCfgModify);				//�����Ƿ�mcs���õ����������ñ�־
	BOOL32 IsMpcNetCfgBeModifedByMcs();									//���������Ƿ�mcs�޸�
	void WriteFailedTimeForOpenMcuCfg(s32 nErrno);
	void ShowEqpInfo( void ); //��ʾ������Ϣ[2/16/2012 chendaiwei]

    // ���� ��������MCS��IP��Ϣ  [04/24/2012 liaokang]
    BOOL32 AddRegedMcsIp( u32 dwMcsIpAddr );                                    // �������MCS��IP��Ϣ
    BOOL32 DeleteRegedMcsIp( u32 dwMcsIpAddr );                                 // ɾ������MCS��IP��Ϣ
    BOOL32 GetRecombinedRegedMcsIpInfo( s8 *pchRegedMcsIpInfo, u16 &wBufLen );  // ��ȡ���������MCS��IP��Ϣ
	u8 GetBrdIdbyIpAddr(u32 dwIpAddr);
#ifdef _8KH_
	void SetMcuType800L( void );
	void SetMcuType8000Hm(void);
	BOOL32 Is800LMcu( void );
	BOOL32 Is8000HmMcu( void );
#endif	

private:
	s32 MoveToSectionStart( FILE *stream, const s8* lpszSectionName, BOOL32 bCreate );
	s32 MoveToSectionEnd( FILE *stream, const s8* lpszSectionName, BOOL32 bCreate );
	s32	DelSection( s8 *pszfilename, const s8* lpszSectionName );
	
private:

    // ������Ϣ����
    TLocalInfo			m_tMcuLocalInfo;                        // ������Ϣ
    TMcuSystem          m_tMcuSystem;                           // ϵͳ��Ϣ
    TMPCInfo            m_tMPCInfo;                             // MPC��Ϣ(������)    
	TEqpExCfgInfo       m_tEqpExCfgInfo;                       // ������չ��Ϣ
	
	TGKProxyCfgInfo		m_tGkProxyCfgInfo;						//gk/��������// [3/16/2010 xliang]
	TProxyDMZInfo       m_tProxyDMZInfo;						//DMZ���������Ϣ
	TPrsTimeSpan		m_tPrsTimeSpan;
    
    TMcu8KECfg          m_tMcuEqpCfg;							//����MPC��������·������// [1/14/2010 xliang] 

    // �豸��ı��� 
    u32					m_dwBasEntryNum;                        // Bas��Ŀ

    //zw[08/06/2008]
    u32                 m_dwBasHDEntryNum;                      // BasHD��Ŀ 

    u32					m_dwMixEntryNum;                        // Mix��Ŀ 
    u32                 m_dwMpwEntryNum;                        // Mpw��Ŀ
    u32                 m_dwPrsEntryNum;                        // Prs��Ŀ
    u32					m_dwVMPEntryNum;                        // Vmp��Ŀ
    u32					m_dwRecEntryNum;                        // Rec��Ŀ
    u32					m_dwTVWallEntryNum;                     // TvWall��Ŀ
    u32					m_dwBrdCfgEntryNum;                     // ������Ŀ
    
    //4.6�¼����� jlb
    u32                 m_dwHduEntryNum;                        // Hdu��Ŀ
    u32                 m_dwEbapEntryNum;                       // Ebap��Ŀ
    u32                 m_dwEvpuEntryNum;                       // Evpu��Ŀ
	u32                 m_dwHduSchemeNum;                       // HduԤ����Ŀ
	u32                 m_dwSvmpEntryNum;                       // Svmp��Ŀ
	u32                 m_dwDvmpEntryNum;                       // MAXNUM = 8  8*2
    u32                 m_dwMpuBasEntryNum;                     //  MpuBas��Ŀ
	u32					m_dwVrsRecEntryNum;						// VRS��¼����Ŀ

    u32                 m_dwVmpAttachNum;					    // VMP�����Ŀ
    TEqpBasEntry		m_atBasTable[MAX_PRIEQP_NUM];	        // Bas��
    
    //zw[08/07/2008]
    TEqpBasHDEntry		m_atBasHDTable[MAX_PRIEQP_NUM];	        // BasHD��

    TEqpMixerEntry	    m_atMixTable[MAX_PRIEQP_NUM];	        // Mix��
    TEqpMPWEntry        m_atMpwTable[MAX_PRIEQP_NUM];           // Mpw��
    TEqpPrsEntry        m_atPrsTable[MAX_PRIEQP_NUM];		    // Prs��
    TEqpVMPEntry		m_atVMPTable[MAX_PRIEQP_NUM];	        // Vmp��
    TEqpRecEntry		m_atRecTable[MAX_PRIEQP_NUM];	        // Rec��
    TEqpTVWallEntry		m_atTVWallTable[MAX_PRIEQP_NUM];        // TvWall��
    TEqpBrdCfgEntry	    m_atBrdCfgTable[MAX_BOARD_NUM];		    // �����
    TVmpAttachCfg       m_atVmpAttachCfg[MAX_VMPSTYLE_NUM];	    // VMP����
	//4.6�¼�����  jlb
	TEqpHduEntry        m_atHduTable[MAX_PERIHDU_NUM];          // Hdu��
 	TEqpSvmpEntry       m_atSvmpTable[MAX_PRIEQP_NUM];          // Svmp��
//	TEqpDvmpBasicEntry  m_atDvmpBasicTable[MAX_PRIEQP_NUM];     // BasicDvmp��
	TEqpMpuBasEntry     m_atMpuBasTable[MAX_PRIEQP_NUM];        // MpuBas��
//	TEqpEbapEntry       m_atEbapTable[MAX_PRIEQP_NUM];          // Ebap��
//	TEqpEvpuEntry       m_atEvpuTable[MAX_PRIEQP_NUM];          // Evpu��
	THduStyleInfo       m_atHduSchemeTable[MAX_HDUSTYLE_NUM];   // HDUԤ����
	TEqpVrsRecEntry		m_atVrsRecTable[MAX_PRIEQP_NUM];		// VRS��¼����

    TEqpDCSEntry		m_tDCS;						            // ���ֻ��������   
        
    // ������ı���  
    u32					m_dwTrapRcvEntryNum;				    // Trap��������Ŀ
    TTrapInfo			m_atTrapRcvTable[MAXNUM_TRAP_LIST];	    // Trap��������
    TNetWorkInfo		m_tMcuNetwork;						    // ����������Ϣ
    TEqpNetSync			m_tNetSync;							    // ��ͬ��   
    TQosInfo            m_tMcuQosInfo;						    // Qos ��Ϣ
    TDSCModuleInfo      m_tMcuDscInfo;                          // MCU8000B����DSC������Ϣ
	TDSCModuleInfo		m_tLastMcuDscInfo;						// MCU8000B�޸�ǰ������DSC������Ϣ
	
	// VCS
	s8                  m_achVCSSoftName[MAX_VCSSOFTNAME_LEN];     // �������

    // MCS   ���� ����MCS��IP��Ϣ  [04/24/2012 liaokang] 
    TRegedMcsIpInfo     m_tRegedMcsIpInfo[MAXNUM_MCU_MC];       // ����MCS��IP��Ϣ

    // ������ʶ����
    u16                 m_wPowerOffEvent;                       // �����¼���
    u32                 m_dwPowerOffDstId;                      // �����¼����ܶ���
    u16                 m_wRunningEvent;                        // �����¼���
    u32                 m_dwRunningDstId;                       // �����¼����ܶ���
    u16                 m_wRebootEvent;                         // �����¼���
    u32                 m_dwRebootDstId;                        // �����¼����ܶ���
	s8                  m_achCfgName[256];					    // �����ļ�����
    SEMHANDLE           m_hMcuAllConfig;                        // ������Ϣд���ź�������
	s8					m_achCfgBakName[256];					// �����ļ���������
	s8					m_achCompileTime[MAX_ALARM_TIME_LEN + 1];	//mcu����ʱ��
	u8					m_byTempPortKind;
	TMcuPfmInfo			m_tMcuPfmInfo;							// Mcu Performance Info
	TMcuUpdateInfo		m_tMcuUpdateInfo;						// Mcu Update Info
	T8KECpuInfo         m_t8KECpuInfo;                          // mqs [2010-11-30]
	T8KEMemInfo         m_t8KEMemInfo;                          // mqs [2010-11-30]
	//[2011/02/11 zhushz] mcs�޸�mcu ip
	TMcuNewNetCfg		m_tMcuNewNetCfg;						//mcs�����µ���������
	BOOL32				m_bIsNetCfgBeModifedByMcs;				//mcs�����µ��������ñ�־
	u8					m_byIsMcu800L;							//�Ƿ���800L MCU
	u8					m_byIsMcu8000H_M;						//�Ƿ���8000H-M MCU
	u8					m_byEncoding;							//���뷽ʽ��0��ʾGBK,1��ʾUTF8������δʹ��
#ifdef _8KI_
	TNewNetCfg			m_tNewNetCfg;
#endif
};

class CCfgSemOpt{
public:
	CCfgSemOpt( SEMHANDLE& hSem )
	{
		m_hSem = hSem;
		if( OspSemTakeByTime( m_hSem, WAIT_SEM_TIMEOUT ) != TRUE )
		{
			OspPrintf( TRUE, FALSE, "[Agent] semTake error accord to handle<0x%x>\n", hSem );
		}
	}
	~CCfgSemOpt( )
	{
		OspSemGive( m_hSem );
	}
private:
	SEMHANDLE m_hSem;
};

void Agtlog( u8 byPrintLvl, s8 * pszFmt, ... );
void SendMsgToMcuCfg( u16 wEvent, const u8 * pcMsg, u16 wLen );
const s8* GetBoardIDStr();
const s8* GetMcuSoftVersion();

extern CCfgAgent		g_cCfgParse;
extern CBrdManagerApp	g_cBrdManagerApp;
extern CAlarmProc		g_cAlarmProc;

extern TMcuAgentDebugInfo g_tAgentDebugInfo;

#endif  // _AGENT_CONFIGURE_READ_
