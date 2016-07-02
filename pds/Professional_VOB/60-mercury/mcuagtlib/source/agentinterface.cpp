/*****************************************************************************
   ģ����      : Mcu Agent
   �ļ���      : AgentInterface.cpp
   ����ļ�    : AgentInterface.h
   �ļ�ʵ�ֹ���: Agent �ṩ���ϲ���õýӿ���
   ����        : liuhuiyun
   �汾        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2005/08/17  1.0         liuhuiyun       ����
******************************************************************************/

#include "agentinterface.h"
#include "configureagent.h"


CAgentInterface::CAgentInterface()
{
	//��ʼ������MCU����ʼ�˿� [12/13/2011 chendaiwei]
	m_wMixerMcuRecvStartPort = MIXER_MCU_STARTPORT;
	m_wRecMcuRecvStartPort = REC_MCU_STARTPORT;
	m_wBasMcuRecvStartPort = BAS_MCU_STARTPORT;
	m_wVmpMcuRecvStartPort = VMP_MCU_STARTPORT;
	
	m_byIsNeedRebootAllMPUBas = 0;
}

CAgentInterface::~CAgentInterface()
{
}

/*=============================================================================
  �� �� ���� ReadTrapTable
  ��    �ܣ� ��Trap��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8* pbyTrapNum
             TTrapInfo** pptTrapTable
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::ReadTrapTable(u8* pbyTrapNum, TTrapInfo* ptTrapTable )
{
     return g_cCfgParse.ReadTrapTable(pbyTrapNum, ptTrapTable);
}

/*=============================================================================
  �� �� ���� WriteTrapTable
  ��    �ܣ� дTrap��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byTrapNum
             TTrapInfo** pptTrapTable
  �� �� ֵ�� u16 
=============================================================================*/
u16 CAgentInterface::WriteTrapTable(u8 byTrapNum, TTrapInfo* ptTrapTable)
{
    return g_cCfgParse.WriteTrapTable(byTrapNum, ptTrapTable);
}

/*=============================================================================
  �� �� ���� ReadBrdTable
  ��    �ܣ� �������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8* pbyBrdNum
             TBoardInfo** pptBoardTable
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::ReadBrdTable(u8* pbyBrdNum, TBoardInfo* ptBoardTable)
{
     return g_cCfgParse.ReadBrdTable(pbyBrdNum, ptBoardTable);
}

/*=============================================================================
  �� �� ���� WriteBrdTable
  ��    �ܣ� д�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byBrdNum
             TBoardInfo** pptBoardTable
  �� �� ֵ�� u16 
=============================================================================*/
u16 CAgentInterface::WriteBrdTable(u8 byBrdNum, TBoardInfo* ptBoardTable)
{
    return g_cCfgParse.WriteBrdTable(byBrdNum, ptBoardTable);
}

/*=============================================================================
  �� �� ���� ReadMixerTable
  ��    �ܣ� ����������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8* pbyMixNum
             TEqpMixerInfo** pptMixerTable
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::ReadMixerTable(u8* pbyMixNum, TEqpMixerInfo* ptMixerTable)
{
     return g_cCfgParse.ReadMixerTable(pbyMixNum, ptMixerTable);
}

/*=============================================================================
  �� �� ���� WriteMixerTable
  ��    �ܣ� д��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byMixNum
             TEqpMixerInfo** pptMixerTable
			 BOOL32 bIsNeedUpdatePortInTable ��ʶ�Ƿ���Ҫ�����ڴ��еĶ˿���Ϣ
  �� �� ֵ�� u16 
=============================================================================*/
u16 CAgentInterface::WriteMixerTable(u8 byMixNum, TEqpMixerInfo* ptMixerTable, BOOL32 bIsNeedUpdatePortInTable)
{
	// ������MCU�����ʼ�˿ڷ��� [12/13/2011 chendaiwei]
	for( u8 byRow = 1; byRow <= byMixNum && ptMixerTable; byRow++ )
    {	
		u8 byMixerType = UNKONW_MIXER;
		if(GetMixerSubTypeByRunBrdId(ptMixerTable[byRow-1].GetRunBrdId(),byMixerType))
		{
			ptMixerTable[byRow-1].SetMcuRecvPort(GetPeriEqpMcuRecvStartPort(EQP_TYPE_MIXER,byMixerType));
		}
	}

    return g_cCfgParse.WriteMixerTable(byMixNum, ptMixerTable,bIsNeedUpdatePortInTable);
}

/*=============================================================================
  �� �� ���� ReadTvTable
  ��    �ܣ� ������ǽ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8* pbyTvNum
             TEqpTVWallInfo** pptTvTable
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::ReadTvTable(u8* pbyTvNum, TEqpTVWallInfo* ptTvTable)
{
     return g_cCfgParse.ReadTvTable(pbyTvNum, ptTvTable);
}

/*=============================================================================
  �� �� ���� WriteTvTable
  ��    �ܣ� д����ǽ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byTvNum
             TEqpTVWallInfo** pptTvTable
  �� �� ֵ�� u16 
=============================================================================*/
u16 CAgentInterface::WriteTvTable(u8 byTvNum, TEqpTVWallInfo* ptTvTable)
{
    return g_cCfgParse.WriteTvTable(byTvNum, ptTvTable);
}

/*=============================================================================
  �� �� ���� ReadRecTable
  ��    �ܣ� ��¼�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8* pbyRecNum
             TEqpRecInfo** pptRecTable
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::ReadRecTable(u8* pbyRecNum, TEqpRecInfo* ptRecTable)
{
     return g_cCfgParse.ReadRecTable(pbyRecNum, ptRecTable);
}

/*=============================================================================
  �� �� ���� WriteRecTable
  ��    �ܣ� д¼�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byRecNum
             TEqpRecInfo** pptRecTable
			 BOOL32 bIsNeedUpdatePortInTable ��ʶ�Ƿ���Ҫ�����ڴ��еĶ˿���Ϣ
  �� �� ֵ�� u16 
=============================================================================*/
u16 CAgentInterface::WriteRecTable(u8 byRecNum, TEqpRecInfo* ptRecTable,BOOL32 bIsNeedUpdatePortInTable)
{
	// ������MCU�����ʼ�˿ڷ��� [12/13/2011 chendaiwei]
	for( u8 byRow = 1; byRow <= byRecNum && ptRecTable; byRow++ )
    {
		ptRecTable[byRow-1].SetMcuRecvPort(GetPeriEqpMcuRecvStartPort(EQP_TYPE_RECORDER));
	}

    return g_cCfgParse.WriteRecTable(byRecNum, ptRecTable,bIsNeedUpdatePortInTable);
}

/*=============================================================================
  �� �� ���� ReadVrsRecTable
  ��    �ܣ� ��Vrs��¼����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8* pbyVrsRecNum
             TEqpVrsRecInfo** pptVrsRecTable
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::ReadVrsRecTable(u8* pbyVrsRecNum, TEqpVrsRecCfgInfo* ptVrsRecTable)
{
     return g_cCfgParse.ReadVrsRecTable(pbyVrsRecNum, ptVrsRecTable);
}

/*=============================================================================
  �� �� ���� WriteVrsRecTable
  ��    �ܣ� дVrs��¼����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byVrsRecNum
             TEqpVrsRecInfo** pptVrsRecTable
  �� �� ֵ�� u16 
=============================================================================*/
u16 CAgentInterface::WriteVrsRecTable(u8 byVrsRecNum, TEqpVrsRecCfgInfo* ptVrsRecTable)
{
    return g_cCfgParse.WriteVrsRecTable(byVrsRecNum, ptVrsRecTable);
}

/*=============================================================================
  �� �� ���� ReadBasTable
  ��    �ܣ� ��Bas��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8* pbyBasNum
             TEqpBasInfo** pptBasTable
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::ReadBasTable(u8* pbyBasNum, TEqpBasInfo* ptBasTable)
{
     return g_cCfgParse.ReadBasTable(pbyBasNum, ptBasTable);
}

/*=============================================================================
  �� �� ���� ReadBasHDTable
  ��    �ܣ� ��BasHD��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8* pbyBasHDNum
             TEqpBasHDInfo** pptBasHDTable
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::ReadBasHDTable(u8* pbyBasHDNum, TEqpBasHDInfo* ptBasHDTable)
{
     return g_cCfgParse.ReadBasHDTable(pbyBasHDNum, ptBasHDTable);
}

/*=============================================================================
  �� �� ���� WriteBasTable
  ��    �ܣ� дBas��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byBasNum
             TEqpBasInfo** pptBasTable
			 BOOL32 bIsNeedUpdatePortInTable ��ʶ�Ƿ���Ҫ�����ڴ��еĶ˿���Ϣ
  �� �� ֵ�� u16 
=============================================================================*/
u16 CAgentInterface::WriteBasTable(u8 byBasNum, TEqpBasInfo* ptBasTable, BOOL32 bIsNeedUpdatePortInTable)
{
	// ������MCU�����ʼ�˿ڷ��� [12/13/2011 chendaiwei]
	for( u8 byRow = 1; byRow <= byBasNum && ptBasTable; byRow++ )
    {
		ptBasTable[byRow-1].SetMcuRecvPort(GetPeriEqpMcuRecvStartPort(EQP_TYPE_BAS));
	}

    return g_cCfgParse.WriteBasTable(byBasNum, ptBasTable,bIsNeedUpdatePortInTable);
}

/*=============================================================================
  �� �� ���� WriteBasHDTable
  ��    �ܣ� дBasHD��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byBasHDNum
             TEqpBasHDInfo** pptBasHDTable
			 BOOL32 bIsNeedUpdatePortInTable ��ʶ�Ƿ���Ҫ�����ڴ��еĶ˿���Ϣ
  �� �� ֵ�� u16 
=============================================================================*/
u16 CAgentInterface::WriteBasHDTable(u8 byBasHDNum, TEqpBasHDInfo* ptBasHDTable, BOOL32 bIsNeedUpdatePortInTable)
{
	// ������MCU�����ʼ�˿ڷ��� [12/13/2011 chendaiwei]
	for( u8 byRow = 1; byRow <= byBasHDNum && ptBasHDTable; byRow++ )
    {
		ptBasHDTable[byRow-1].SetMcuRecvPort(GetPeriEqpMcuRecvStartPort(EQP_TYPE_BAS));
	}

    return g_cCfgParse.WriteBasHDTable(byBasHDNum, ptBasHDTable, bIsNeedUpdatePortInTable);
}

/*=============================================================================
  �� �� ���� ReadVmpTable
  ��    �ܣ� ��Vmp��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u8* pbyVmpNum
             TEqpVMPInfo** pptVmpTabl
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::ReadVmpTable( u8* pbyVmpNum, TEqpVMPInfo* ptVmpTable)
{
     return g_cCfgParse.ReadVmpTable(pbyVmpNum, ptVmpTable);
}

/*=============================================================================
  �� �� ���� WriteVmpTable
  ��    �ܣ� дVmp��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byVmpNum
             TEqpVMPInfo** pptVmpTabl
			 BOOL32 bIsNeedUpdatePortInTable ��ʶ�Ƿ���Ҫ�����ڴ��еĶ˿���Ϣ
  �� �� ֵ�� u16 
=============================================================================*/
u16 CAgentInterface::WriteVmpTable(u8 byVmpNum, TEqpVMPInfo* ptVmpTable, BOOL32 bIsNeedUpdatePortInTable )
{
	// ������MCU�����ʼ�˿ڷ��� [12/13/2011 chendaiwei]
	for( u8 byRow = 1; byRow <= byVmpNum && ptVmpTable; byRow++ )
    {
		ptVmpTable[byRow-1].SetMcuRecvPort(GetPeriEqpMcuRecvStartPort(EQP_TYPE_VMP));
	}

    return g_cCfgParse.WriteVmpTable(byVmpNum, ptVmpTable,bIsNeedUpdatePortInTable);
}

/*=============================================================================
  �� �� ���� ReadMpwTable
  ��    �ܣ� ��Mpw��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8* pbyMpwNum
             TEqpMpwInfo* ptMpwTable
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::ReadMpwTable(u8* pbyMpwNum, TEqpMpwInfo* ptMpwTable)
{
     return g_cCfgParse.ReadMpwTable(pbyMpwNum, ptMpwTable);
}

/*=============================================================================
  �� �� ���� WriteMpwTable
  ��    �ܣ� дMpw��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byMpwNum
             TEqpMpwInfo* ptMpwTable
  �� �� ֵ�� u16 
=============================================================================*/
u16 CAgentInterface::WriteMpwTable(u8 byMpwNum, TEqpMpwInfo* ptMpwTable)
{
    return g_cCfgParse.WriteMpwTable(byMpwNum, ptMpwTable);
}

//4.6�¼������д���ñ� jlb
/*=============================================================================
  �� �� ���� ReadHduTable
  ��    �ܣ� ��ȡ����Hdu����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8* pbyHduNum, TEqpHduInfo* pptHduTable
  �� �� ֵ�� u16 
=============================================================================*/
u16    CAgentInterface::ReadHduTable( u8* pbyHduNum, TEqpHduInfo* ptHduTable )
{
    return g_cCfgParse.ReadHduTable(pbyHduNum, ptHduTable);
}

/*=============================================================================
  �� �� ���� WriteHduTable
  ��    �ܣ� ��������Hdu����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8* pbyHduNum, TEqpHduInfo* pptHduTable
  �� �� ֵ�� u16 
=============================================================================*/
u16    CAgentInterface::WriteHduTable( u8 byHduNum, TEqpHduInfo* ptHduTable )
{
	    return g_cCfgParse.WriteHduTable(byHduNum, ptHduTable);
}  

/*=============================================================================
  �� �� ���� WriteHduTable
  ��    �ܣ� ��ȡ����Svmp����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8* pbySvmpNum, TEqpSvmpInfo* pptSvmpTable
  �� �� ֵ�� u16 
=============================================================================*/
u16    CAgentInterface::ReadSvmpTable( u8* pbySvmpNum, TEqpSvmpInfo* ptSvmpTable )
{
    return g_cCfgParse.ReadSvmpTable(pbySvmpNum, ptSvmpTable);
}

/*=============================================================================
  �� �� ���� WriteSvmpTable
  ��    �ܣ� ��������Svmp����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 bySvmpNum, TEqpSvmpInfo* ptSvmpTable
			 BOOL32 bIsNeedUpdatePortInTable ��ʶ�Ƿ���Ҫ�����ڴ��еĶ˿���Ϣ
  �� �� ֵ�� u16 
=============================================================================*/
u16    CAgentInterface::WriteSvmpTable( u8 bySvmpNum, TEqpSvmpInfo* ptSvmpTable,BOOL32 bIsNeedUpdatePortInTable)
{
	// ������MCU�����ʼ�˿ڷ��� [12/13/2011 chendaiwei]
	for( u8 byRow = 1; byRow <= bySvmpNum && ptSvmpTable; byRow++ )
    {
		ptSvmpTable[byRow-1].SetMcuRecvPort(GetPeriEqpMcuRecvStartPort(EQP_TYPE_VMP,ptSvmpTable[byRow-1].GetVmpType()));
	}

	return g_cCfgParse.WriteSvmpTable(bySvmpNum, ptSvmpTable,bIsNeedUpdatePortInTable);
}

/*=============================================================================
  �� �� ���� ReadDvmpTable
  ��    �ܣ� ��ȡ����Dvmp����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8* pbyDvmpNum, TEqpDvmpBasicInfo* pptDvmpTable
  �� �� ֵ�� u16 
=============================================================================*/ 
// u16    CAgentInterface::ReadDvmpTable( u8* pbyDvmpNum, TEqpDvmpBasicInfo* ptDvmpTable )
// {
//     return g_cCfgParse.ReadDvmpTable(pbyDvmpNum, ptDvmpTable);
// }

/*=============================================================================
  �� �� ���� ReadDvmpTable
  ��    �ܣ� ��������Dvmp����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byDvmpNum, TEqpDvmpBasicInfo* ptDvmpTable
  �� �� ֵ�� u16 
=============================================================================*/ 
// u16    CAgentInterface::WriteDvmpTable( u8 byDvmpNum, TEqpDvmpBasicInfo* ptDvmpTable )
// {
// 	 return g_cCfgParse.WriteDvmpTable(byDvmpNum, ptDvmpTable);
// }

/*=============================================================================
  �� �� ���� ReadMpuBasTable
  ��    �ܣ� ��ȡ����MpuBas����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8* pbyMpuBasNum, TEqpSvmpInfo* pptMpuBasTable
  �� �� ֵ�� u16 
=============================================================================*/
u16    CAgentInterface::ReadMpuBasTable( u8* pbyMpuBasNum, TEqpMpuBasInfo* ptMpuBasTable )
{
    return g_cCfgParse.ReadMpuBasTable(pbyMpuBasNum, ptMpuBasTable);
}

/*=============================================================================
  �� �� ���� WriteMpuBasTable
  ��    �ܣ� ��������MpuBas����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byMpuBasNum, TEqpSvmpInfo* ptMpuBasTable
			 BOOL32 bIsNeedUpdatePortInTable ��ʶ�Ƿ���Ҫ�����ڴ��еĶ˿���Ϣ
  �� �� ֵ�� u16 
=============================================================================*/
u16    CAgentInterface::WriteMpuBasTable( u8 byMpuBasNum, TEqpMpuBasInfo* ptMpuBasTable,BOOL32 bIsNeedUpdatePortInTable )
{
	// ������MCU�����ʼ�˿ڷ��� [12/13/2011 chendaiwei]
	for( u8 byRow = 1; byRow <= byMpuBasNum && ptMpuBasTable; byRow++ )
	{
		ptMpuBasTable[byRow-1].SetMcuRecvPort(GetPeriEqpMcuRecvStartPort(EQP_TYPE_BAS,ptMpuBasTable[byRow-1].GetStartMode()));
	}

	return g_cCfgParse.WriteMpuBasTable(byMpuBasNum, ptMpuBasTable,bIsNeedUpdatePortInTable);
}

/*=============================================================================
  �� �� ���� ReadEbapTable
  ��    �ܣ� ��ȡ����Ebap����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8* pbyEbapNum, TEqpEbapInfo* ptEbapTable
  �� �� ֵ�� u16 
=============================================================================*/
// u16    CAgentInterface::ReadEbapTable( u8* pbyEbapNum, TEqpEbapInfo* ptEbapTable )
// {
//     return g_cCfgParse.ReadEbapTable(pbyEbapNum, ptEbapTable);
// }

/*=============================================================================
  �� �� ���� WriteEbapTable
  ��    �ܣ� ��������Ebap����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byEbapNum, TEqpEbapInfo* ptEbapTable
  �� �� ֵ�� u16 
=============================================================================*/
// u16    CAgentInterface::WriteEbapTable( u8 byEbapNum, TEqpEbapInfo* ptEbapTable )
// {
// 	 return g_cCfgParse.WriteEbapTable(byEbapNum, ptEbapTable);
// }


/*=============================================================================
  �� �� ���� ReadEvpuTable
  ��    �ܣ� ��ȡ����Evpu����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8* pbyEvpuNum, TEqpSvmpInfo* ptEvpuTable
  �� �� ֵ�� u16 
=============================================================================*/
// u16    CAgentInterface::ReadEvpuTable( u8* pbyEvpuNum, TEqpEvpuInfo* ptEvpuTable )
// {
//     return g_cCfgParse.ReadEvpuTable(pbyEvpuNum, ptEvpuTable);
// }

/*=============================================================================
  �� �� ���� ReadEvpuTable
  ��    �ܣ� ��������Evpu����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8* pbyEvpuNum, TEqpSvmpInfo* ptEvpuTable
  �� �� ֵ�� u16 
=============================================================================*/
// u16    CAgentInterface::WriteEvpuTable( u8 byEvpuNum, TEqpEvpuInfo* ptEvpuTable )
// {
// 	 return g_cCfgParse.WriteEvpuTable(byEvpuNum, ptEvpuTable);
// }

/*=============================================================================
  �� �� ���� ReadHduSchemeTable
  ��    �ܣ� ��ȡ����HDUԤ�����ñ���Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8* pbyEvpuNum, TEqpSvmpInfo* ptEvpuTable
  �� �� ֵ�� u16 
=============================================================================*/
u16    CAgentInterface::ReadHduSchemeTable( u8* pbyHduProjectNum, THduStyleInfo* ptHduSchemeTable )
{
    return g_cCfgParse.ReadHduSchemeTable(pbyHduProjectNum, ptHduSchemeTable);
}

/*=============================================================================
  �� �� ���� WriteHduSchemeTable
  ��    �ܣ� ��������HDUԤ�����ñ���Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8* pbyEvpuNum, TEqpSvmpInfo* ptEvpuTable
  �� �� ֵ�� u16 
=============================================================================*/
u16    CAgentInterface::WriteHduSchemeTable( u8 byHduProjectNum, THduStyleInfo* ptHduSchemeTable )
{
	 return g_cCfgParse.WriteHduSchemeTable(byHduProjectNum, ptHduSchemeTable);
} 



/*=============================================================================
  �� �� ���� GetEqpRecorderCfg
  ��    �ܣ� ȡ¼���������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u8 byId
             TMcueqpRecorderEntry * ptRecCfg
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::GetEqpRecorderCfg( u8 byId, TEqpRecInfo * ptRecCfg )
{
     return g_cCfgParse.GetEqpRecCfgById( byId, ptRecCfg);
}

/*=============================================================================
  �� �� ���� SetEqpRecorderCfg
  ��    �ܣ� ����¼���������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u8 byId
             TmcueqpRecorderEntry * ptRecCfg
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::SetEqpRecorderCfg( u8 byId, TEqpRecInfo * ptRecCfg )
{
     return g_cCfgParse.SetEqpRecCfgById( byId, ptRecCfg);
}

/*=============================================================================
  �� �� ���� GetEqpTVWallCfg
  ��    �ܣ� ȡ����ǽ��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u8 byId
             TEqpTVWallInfo * ptTWCfg
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::GetEqpTVWallCfg( u8 byId, TEqpTVWallInfo * ptTWCfg )
{
     return g_cCfgParse.GetEqpTVWallCfgById(byId, ptTWCfg);
}

/*=============================================================================
  �� �� ���� SetEqpTVWallCfg
  ��    �ܣ� ���õ���ǽ��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u8 byId
             TEqpTVWallInfo * ptTWCfg
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::SetEqpTVWallCfg( u8 byId, TEqpTVWallInfo * ptTWCfg )
{
     return g_cCfgParse.SetEqpTVWallCfgById(byId, ptTWCfg);
}

/*=============================================================================
  �� �� ���� GetEqpMixerCfg
  ��    �ܣ� ȡ��������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u8 byId
             TEqpMixerInfo * ptMixerCfg
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::GetEqpMixerCfg( u8 byId, TEqpMixerInfo * ptMixerCfg )
{
     return g_cCfgParse.GetEqpMixerCfgById(byId, ptMixerCfg);
}

/*=============================================================================
  �� �� ���� SetEqpMixerCfg
  ��    �ܣ� ���û�������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u8 byId
             TEqpMixerInfo tMixerCfg
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::SetEqpMixerCfg( u8 byId, TEqpMixerInfo tMixerCfg )
{
     return g_cCfgParse.SetEqpMixerCfgById(byId, tMixerCfg);
}

/*=============================================================================
  �� �� ���� GetEqpBasCfg
  ��    �ܣ� ȡBas��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u8 byId
             TEqpBasInfo * ptBasCfg
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::GetEqpBasCfg( u8 byId, TEqpBasInfo * ptBasCfg )
{
     return g_cCfgParse.GetEqpBasCfgById(byId, ptBasCfg);
}

/*=============================================================================
  �� �� ���� IsEqpBasHD
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byId
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CAgentInterface::IsEqpBasHD( u8 byId )
{
     return g_cCfgParse.IsEqpBasHD(byId);
}

BOOL32 CAgentInterface::IsEqpBasAud (u8 byEqpId )
{
	return g_cCfgParse.IsEqpBasAud(byEqpId);
}

/*=============================================================================
  �� �� ���� IsSVmp
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byId
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CAgentInterface::IsSVmp( u8 byEqpId )
{
    return g_cCfgParse.IsSVmp( byEqpId );
}

/*=============================================================================
�� �� ���� GetVmpEqpVersion
��    �ܣ� ��ȡ��ǰVmp��Ӳ���汾
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byEqpId
�� �� ֵ�� u8 
------------------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
=============================================================================*/
u16 CAgentInterface::GetVmpEqpVersion( u8 byEqpId )
{
	return g_cCfgParse.GetVmpEqpVersion(byEqpId);
}

/*=============================================================================
  �� �� ���� IsSVmp
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byId
  �� �� ֵ�� BOOL32 
=============================================================================*/
// BOOL32 CAgentInterface::IsDVmp( u8 byEqpId )
// {
//     return g_cCfgParse.IsDVmp( byEqpId );
// }

/*=============================================================================
  �� �� ���� IsSVmp
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byId
  �� �� ֵ�� BOOL32 
=============================================================================*/
// BOOL32 CAgentInterface::IsEVpu( u8 byEqpId )
// {
//     return g_cCfgParse.IsEVpu( byEqpId );
// }

/*=============================================================================
  �� �� ���� IsSVmp
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byId
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CAgentInterface::IsMpuBas( u8 byEqpId )
{
    return g_cCfgParse.IsMpuBas( byEqpId );
}

/*=============================================================================
�� �� ���� GetBasEqpVersion
��    �ܣ� ��ȡ��ǰBas��Ӳ���汾
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byEqpId
�� �� ֵ�� u8 
------------------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2011/12/07  4.7         ������          ����
=============================================================================*/
u16 CAgentInterface::GetBasEqpVersion( u8 byEqpId )
{
	return g_cCfgParse.GetBasEqpVersion(byEqpId);
}

/*=============================================================================
�� �� ���� GetHDUEqpVersion
��    �ܣ� ��ȡ��ǰHDU��Ӳ���汾
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byEqpId
�� �� ֵ�� u8 
------------------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    2012/02/20  4.7.2        chendaiwei         ����
=============================================================================*/
u16 CAgentInterface::GetHDUEqpVersion( u8 byEqpId )
{
	return g_cCfgParse.GetHDUEqpVersion(byEqpId);
}

/*=============================================================================
  �� �� ���� IsSVmp
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byId
  �� �� ֵ�� BOOL32 
=============================================================================*/
// BOOL32 CAgentInterface::IsEBap( u8 byEqpId )
// {
//     return g_cCfgParse.IsEBap( byEqpId );
// }

/*=============================================================================
  �� �� ���� GetEqpSvmpCfgById
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byId
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16    CAgentInterface::GetEqpSvmpCfgById( u8 byId, TEqpSvmpInfo * ptSvmpCfg )
{
    return g_cCfgParse.GetEqpSvmpCfgById( byId, ptSvmpCfg );
}

/*=============================================================================
  �� �� ���� GetEqpBasHDCfg
  ��    �ܣ� ȡBasHD��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u8 byId
             TEqpBasHDInfo * ptBasHDCfg
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::GetEqpBasHDCfg( u8 byId, TEqpBasHDInfo * ptBasHDCfg )
{
     return g_cCfgParse.GetEqpBasHDCfgById(byId, ptBasHDCfg);
}

/*=============================================================================
  �� �� ���� GetMpuBasBrdInfo
  ��    �ܣ� ȡMpu MPU2 bas ������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u8 byId
             TEqpMpuBasInfo * ptEqpMpuBasInfo
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::GetMpuBasBrdInfo(u8 byBasEqpId,TEqpMpuBasInfo* ptEqpMpuBasInfo )
{
     return g_cCfgParse.GetMpuBasCfgById(byBasEqpId, ptEqpMpuBasInfo);
}

/*=============================================================================
  �� �� ���� SetEqpBasCfg
  ��    �ܣ� ����Bas��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u8 byId
             TEqpBasInfo * ptBasCfg
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::SetEqpBasCfg( u8 byId, TEqpBasInfo * ptBasCfg )
{
     return g_cCfgParse.SetEqpBasCfgById(byId, ptBasCfg);
}

/*=============================================================================
  �� �� ���� SetEqpHduCfg
  ��    �ܣ� ���õ���Hdu��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u8 byId
             TEqpHduInfo* ptHduCfg
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16    CAgentInterface::SetEqpHduCfg(u8 byHduId, TEqpHduInfo* ptHduCfg)         
{
	return g_cCfgParse.SetEqpHduCfgById( byHduId, ptHduCfg);
}
/*=============================================================================
  �� �� ���� GetEqpHduCfg
  ��    �ܣ� ��ȡ����Hdu��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u8 byId
             TEqpHduInfo* ptHduCfg
  �� �� ֵ�� BOOL32 
=============================================================================*/  
u16    CAgentInterface::GetEqpHduCfg(u8 byHduId, TEqpHduInfo* ptHduCfg)         
{
	return g_cCfgParse.GetEqpHduCfgById(byHduId, ptHduCfg);
}
																				
/*=============================================================================
  �� �� ���� GetEqpVMPCfg
  ��    �ܣ� ȡVmp��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u8 byId
             TEqpVMPInfo * ptVMPCfg
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::GetEqpVMPCfg( u8 byId, TEqpVMPInfo * ptVMPCfg )
{
    if ( IsSVmp(byId) )
    {
        return g_cCfgParse.GetEqpSvmpCfgById( byId, (TEqpSvmpInfo*)(void*)ptVMPCfg );
    }
    else if ( SUCCESS_AGENT != g_cCfgParse.GetEqpVMPCfgById(byId, ptVMPCfg) )
    {
		Agtlog(LOG_INFORM, "[GetEqpVMPCfg] can not find byId(%d) in all VmpTable!\n", byId );
        return ERR_AGENT_EQPNOTEXIST;
    }

    return SUCCESS_AGENT;
}

/*=============================================================================
  �� �� ���� SetEqpVMPCfg
  ��    �ܣ� ����Vmp��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u8 byId
             TEqpVMPInfo * ptVMPCfg
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::SetEqpVMPCfg( u8 byId, TEqpVMPInfo * ptVMPCfg )
{
     return g_cCfgParse.SetEqpVMPCfgById( byId, ptVMPCfg);
}

/*=============================================================================
  �� �� ���� GetEqpMpwCfg
  ��    �ܣ� ȡָ��Mpw��������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byId
             TEqpMpwInfo* ptMpwCfg
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::GetEqpMpwCfg(u8 byId, TEqpMpwInfo* ptMpwCfg)
{
     return g_cCfgParse.GetEqpMpwCfgById(byId, ptMpwCfg);
}

/*=============================================================================
  �� �� ���� SetEqpMpwCfg
  ��    �ܣ� ����ָ��Mpw��������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byId
             TEqpMpwInfo* ptMpwCfg
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::SetEqpMpwCfg(u8 byId, TEqpMpwInfo* ptMpwCfg)
{
     return g_cCfgParse.SetEqpMpwCfgById(byId, ptMpwCfg);
}

/*=============================================================================
�� �� ���� GetEqpSwitchBrdId
��    �ܣ� ��ȡָ������ת������Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byId
�� �� ֵ�� ��Ӧת����ID
=============================================================================*/
u8 CAgentInterface::GetEqpSwitchBrdId(u8 byEqpId)
{
	return g_cCfgParse.GetEqpSwitchBrdId(byEqpId);
}
/*=============================================================================
�� �� ���� GetEqpSwitchBrdId
��    �ܣ� �����ƶ�����ת������Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byEqpId
�� �� ֵ�� ��Ӧת����ID
=============================================================================*/
BOOL32 CAgentInterface::SetEqpSwitchBrdId(u8 byEqpId, u8 bySwitchBrdId)
{
	return g_cCfgParse.SetEqpSwitchBrdId(byEqpId, bySwitchBrdId);
}

/*=============================================================================
  �� �� ���� GetQosInfo
  ��    �ܣ� ȡQos��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TQosInfo*  ptQosInfo
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::GetQosInfo(TQosInfo*  ptQosInfo )
{
     return g_cCfgParse.GetQosInfo(ptQosInfo);
}

/*=============================================================================
  �� �� ���� GetLocalInfo
  ��    �ܣ� ȡ������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TLocalInfo*  ptLocalInfo
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::GetLocalInfo(TLocalInfo*  ptLocalInfo )
{
	u16 wRet = g_cCfgParse.GetLocalInfo(ptLocalInfo);

#ifdef _8KH_
	if( Is800LMcu() && NULL != ptLocalInfo )
	{
		ptLocalInfo->SetMaxMcsOnGoingConfNum( 4 );
		ptLocalInfo->SetAdminLevel( DEF_ADMINLVEL );
		ptLocalInfo->SetConfNameShowType( 1 );
		ptLocalInfo->SetIsHoldDefaultConf( 1 );
		ptLocalInfo->SetIsSaveBand( 1 );
	}
#endif
    return wRet;
}

/*=============================================================================
  �� �� ���� SetLocalInfo
  ��    �ܣ� ���ñ�����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TLocalInfo  tLocalInfo
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::SetLocalInfo(TLocalInfo  tLocalInfo )
{
#ifdef _8KH_
	if( Is800LMcu() )
	{
		tLocalInfo.SetMaxMcsOnGoingConfNum( 4 );
		tLocalInfo.SetAdminLevel( DEF_ADMINLVEL );
		tLocalInfo.SetConfNameShowType( 1 );
		tLocalInfo.SetIsHoldDefaultConf( 1 );
		tLocalInfo.SetIsSaveBand( 1 );
	}
#endif
	 return g_cCfgParse.SetLocalInfo(&tLocalInfo);
}

/*=============================================================================
  �� �� ���� GetNetWorkInfo
  ��    �ܣ� ȡ������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TNetWorkInfo*  ptNetWorkInfo
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::GetNetWorkInfo(TNetWorkInfo*  ptNetWorkInfo )
{
     return g_cCfgParse.GetNetWorkInfo(ptNetWorkInfo);
}

/*=============================================================================
  �� �� ���� SetNetWorkInfo
  ��    �ܣ� ����������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TNetWorkInfo  tNetWorkInfo
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::SetNetWorkInfo(TNetWorkInfo  tNetWorkInfo )
{
     return g_cCfgParse.SetNetWorkInfo(&tNetWorkInfo);
}

/*=============================================================================
�� �� ���� AddRegedMcsIp
��    �ܣ� �������MCS��IP��Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u32 dwMcsIpAddr 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2012/4/24   4.7			liaokang                ����
=============================================================================*/
BOOL32 CAgentInterface::AddRegedMcsIp( u32 dwMcsIpAddr )
{
    return g_cCfgParse.AddRegedMcsIp( dwMcsIpAddr );
}

/*=============================================================================
�� �� ���� DeleteRegedMcsIp
��    �ܣ� ɾ������MCS��IP��Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u32 dwMcsIpAddr 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2012/4/24   4.7			liaokang                ����
=============================================================================*/
BOOL32 CAgentInterface::DeleteRegedMcsIp( u32 dwMcsIpAddr )
{
    return g_cCfgParse.DeleteRegedMcsIp( dwMcsIpAddr );
}

/*=============================================================================
  �� �� ���� IsMcuConfiged
  ��    �ܣ� �ж�MCU�����ñ�ʶ
  �㷨ʵ�֣� 
  ȫ�ֱ����� ��
  ��    ���� ��
  �� �� ֵ�� BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���       �޸�����
  2006/12/21  4.0         �ܹ��       ����
=============================================================================*/
BOOL32 CAgentInterface::IsMcuConfiged()
{
	return g_cCfgParse.IsMcuConfiged();
}

/*=============================================================================
  �� �� ���� SetIsMcuConfiged
  ��    �ܣ� ����MCU�����ñ�ʶ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byIsMcuConfiged
  �� �� ֵ�� u16 
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���       �޸�����
  2006/12/21  4.0         �ܹ��       ����
=============================================================================*/
u16 CAgentInterface::SetIsMcuConfiged(BOOL32 bConfiged)
{
	return g_cCfgParse.SetIsMcuConfiged(bConfiged);
}

/*=============================================================================
  �� �� ���� GetDcsId
  ��    �ܣ� ȡDcsId
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u32  
=============================================================================*/
u32   CAgentInterface::GetDcsIp()
{
    return g_cCfgParse.GetDcsIp();
}

/*=============================================================================
  �� �� ���� SetDcsId
  ��    �ܣ� ����DcsId
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byId
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::SetDcsIp(u32 dwDcsIp)
{
     return g_cCfgParse.SetDcsIp(dwDcsIp);
}

/*=============================================================================
  �� �� ���� ReadPrsTable
  ��    �ܣ� ȡPrs����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8* pbyPrsNum
             TEqpPrsInfo* ptPrsTable
  �� �� ֵ�� u16 
=============================================================================*/
u16 CAgentInterface::ReadPrsTable(u8* pbyPrsNum, TEqpPrsInfo* ptPrsTable)
{
    return g_cCfgParse.ReadPrsTable(pbyPrsNum, ptPrsTable);
}

/*=============================================================================
  �� �� ���� GetEqpPrsCfg
  ��    �ܣ� ȡ����Prs��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byPrsId
             TEqpPrsInfo& tPrsCfg
  �� �� ֵ�� u16 
=============================================================================*/
u16 CAgentInterface::GetEqpPrsCfg(u8 byPrsId, TEqpPrsInfo& tPrsCfg)
{
    return g_cCfgParse.GetEqpPrsCfgById(byPrsId, &tPrsCfg);
}

/*=============================================================================
  �� �� ���� WritePrsTable
  ��    �ܣ� ����Prs��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byPrsNum
             TEqpPrsInfo* ptPrsTable
  �� �� ֵ�� u16 
=============================================================================*/
u16 CAgentInterface::WritePrsTable(u8 byPrsNum, TEqpPrsInfo* ptPrsTable)
{
    return g_cCfgParse.WritePrsTable(byPrsNum, ptPrsTable);
}

/*=============================================================================
  �� �� ���� GetNetSyncInfo
  ��    �ܣ� ȡ��ͬ����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TNetSyncInfo* ptNetSyncInfo
  �� �� ֵ�� void 
=============================================================================*/
u16 CAgentInterface::GetNetSyncInfo( TNetSyncInfo* ptNetSyncInfo )
{
     return g_cCfgParse.GetNetSyncInfo(ptNetSyncInfo);
}

/*=============================================================================
  �� �� ���� SetNetSyncInfo
  ��    �ܣ� ������ͬ����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TNetSyncInfo tNetSyncInfo
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::SetNetSyncInfo(TNetSyncInfo tNetSyncInfo)
{
    return g_cCfgParse.SetNetSyncInfo(&tNetSyncInfo);
}

/*=============================================================================
�� �� ���� SetLoginInfo
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TLoginInfo *ptLoginInfo
�� �� ֵ�� u16 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/10/19   4.0		�ܹ��                  ����
=============================================================================*/
u16 CAgentInterface::SetLoginInfo( TLoginInfo *ptLoginInfo )
{
	return g_cCfgParse.SetLoginInfo( ptLoginInfo );
}

/*=============================================================================
�� �� ���� GetLoginInfo
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TLoginInfo *ptLoginInfo
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/10/19   4.0		�ܹ��                  ����
=============================================================================*/
u16	CAgentInterface::GetLoginInfo( TLoginInfo *ptLoginInfo )
{
	return g_cCfgParse.GetLoginInfo( ptLoginInfo );
}

/*=============================================================================
  �� �� ���� GetVmpAttachCfg
  ��    �ܣ� ȡ����ϳɸ�����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TVmpAttachCfg* ptVmpAttachCfg
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::ReadVmpAttachTable(u8* pbyVmpProjectNum, TVmpAttachCfg* ptVmpAttachTable)
{
     return g_cCfgParse.ReadVmpAttachTable(pbyVmpProjectNum, ptVmpAttachTable);
}
// ������� [pengguofeng 4/16/2013]
/*=============================================================================
�� �� ���� ReadMcuEncodingType
��    �ܣ� ��ȡMcu�ı��뷽ʽ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8
�� �� ֵ�� u16
=============================================================================*/
u16 CAgentInterface::ReadMcuEncodingType(u8 &byEncoding)
{
	return g_cCfgParse.ReadMcuEncodingType(byEncoding);
}

/*=============================================================================
�� �� ���� WriteMcuEncodingType
��    �ܣ� д��Mcu�ı��뷽ʽ��д���ļ���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8
�� �� ֵ�� True/False
=============================================================================*/
BOOL32 CAgentInterface::WriteMcuEncodingType(const u8 &byEncoding)
{
	return g_cCfgParse.WriteMcuEncodingType(&byEncoding);
}

/*=============================================================================
�� �� ���� GetMcuEncodingType
��    �ܣ� ��ȡMcu�ı��뷽ʽ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� ��
�� �� ֵ�� u8
=============================================================================*/
u8 CAgentInterface::GetMcuEncodingType(void)
{
	return g_cCfgParse.GetEncodingType();
}

/*=============================================================================
�� �� ���� SetMcuEncodingType
��    �ܣ� ����Mcu�ı��뷽ʽ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8
�� �� ֵ�� ��
=============================================================================*/
void CAgentInterface::SetMcuEncodingType(const u8 &byEncoding)
{
	g_cCfgParse.SetEncodingType(byEncoding);
}
// end [pengguofeng 4/16/2013]
/*=============================================================================
  �� �� ���� SetVmpAttachCfg
  ��    �ܣ� ���û���ϳɸ�����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TVmpAttachCfg& tVmpAttachCfg
  �� �� ֵ�� u8 
=============================================================================*/
u16 CAgentInterface::WriteVmpAttachTable(u8 byVmpProjectNum, TVmpAttachCfg* ptVmpAttachTable)
{
    return g_cCfgParse.WriteVmpAttachTable(byVmpProjectNum, ptVmpAttachTable);
}
/*=============================================================================
  �� �� ���� GetPriInfo
  ��    �ܣ� ȡ������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byEqpId
             u32* pdwEqpIp:������
             u8* pbyEqpType
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::GetPeriInfo(u8 byEqpId, u32* pdwEqpIp, u8* pbyEqpType)
{
    return g_cCfgParse.GetPriInfo(byEqpId, pdwEqpIp, pbyEqpType);
}

/*=============================================================================
  �� �� ���� GetMpcIp
  ��    �ܣ� ȡMpcIp(������)
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u32 
=============================================================================*/
u32 CAgentInterface::GetMpcIp()
{
    return g_cCfgParse.GetLocalIp();
}

/*=============================================================================
  �� �� ���� SetMpcEnabled 
  ��    �ܣ� ֻ�ṩ�����ýӿڣ�֪ͨMcu������MPC�Ƿ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� BOOL32 bEnabled
  �� �� ֵ�� void 
=============================================================================*/
u16 CAgentInterface::SetMpcEnabled(BOOL32 bEnabled)
{
    g_cCfgParse.SetMpcActive(bEnabled);
    return SUCCESS_AGENT;
}


/*=============================================================================
  �� �� ���� GetMpcDIpAddr
  ��    �ܣ� ȡMpcD��Ip��ַ(������)
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� u32 
=============================================================================*/
u32 CAgentInterface::GetMpcDIpAddr(void)
{
    TMPCInfo tMPCInfo;
    g_cCfgParse.GetMPCInfo( &tMPCInfo );
    return tMPCInfo.GetOtherMpcIp();
}
/*=============================================================================
  �� �� ���� GetMpcDPort
  ��    �ܣ� ȡMpcD��Port
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u16 
=============================================================================*/
u16 CAgentInterface::GetMpcDPort(void)
{
    TMPCInfo tMPCInfo;
    g_cCfgParse.GetMPCInfo( &tMPCInfo );
    return tMPCInfo.GetOtherMpcPort();
}

/*=============================================================================
  �� �� ���� IsHaveOtherMpc
  ��    �ܣ� ��ǰ�Ƿ�����������Mpc��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� BOOL32 TRUE�����������飬FALSE��û������
=============================================================================*/
BOOL32 CAgentInterface::IsHaveOtherMpc(void)
{
    TMPCInfo tMPCInfo;
    g_cCfgParse.GetMPCInfo( &tMPCInfo );
    return tMPCInfo.GetIsHaveOtherMpc();
}

/*=============================================================================
  �� �� ���� GetMcuAlias
  ��    �ܣ� ȡMcu Alias
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� s8* lpszAlias
             u8 byLength
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::GetMcuAlias(s8* pszAlias, u8 byLength)
{
    if( NULL == pszAlias)
    {
        return ERR_AGENT_GETNODEVALUE;
    }
    byLength = byLength > MAX_ALIAS_LENGTH ? MAX_ALIAS_LENGTH: byLength;

    TLocalInfo  tLocalInfo;
    u16 wRet = g_cCfgParse.GetLocalInfo(&tLocalInfo);
    memcpy( pszAlias, tLocalInfo.GetAlias(), byLength);
	
#ifdef _UTF8
	// ������������80�ֽڣ�����ȡ32�������»��в������ĺ��ֲ��� [pengguofeng 11/4/2013]
	CorrectUtf8Str(pszAlias, byLength);
#endif
    return wRet;
}

/*=============================================================================
  �� �� ���� GetIsUseMpcTranData
  ��    �ܣ� ȡ�Ƿ���Mcpת������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u8  
=============================================================================*/
BOOL32 CAgentInterface::GetIsUseMpcTranData()
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return(1 == tNetWorkInfo.GetMpcTransData());
}

/*=============================================================================
  �� �� ���� GetIsUseMpcStack
  ��    �ܣ� ȡ�Ƿ���Mcp����Э��ջ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CAgentInterface::GetIsUseMpcStack()
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return(1 == tNetWorkInfo.GetMpcStack());
}
/*=============================================================================
  �� �� ���� GetCastIpAddr
  ��    �ܣ� ȡ�鲥��ַ(������)
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u32  
=============================================================================*/
u32  CAgentInterface::GetCastIpAddr()
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return tNetWorkInfo.GetCastIp();
}

/*=============================================================================
  �� �� ���� GetCastPort
  ��    �ܣ� ȡ�鲥�˿�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u16  
=============================================================================*/
u16  CAgentInterface::GetCastPort()
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return tNetWorkInfo.GetCastPort();
}
/*=============================================================================
  �� �� ���� GetE164Number
  ��    �ܣ� ȡE164��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� s8* lpszE164Number
             u8 byLength
  �� �� ֵ�� void 
=============================================================================*/
u16 CAgentInterface::GetE164Number(s8* pszE164Number, u8 byLength)
{
    TLocalInfo  tLocalInfo;
    g_cCfgParse.GetLocalInfo(&tLocalInfo);
    memcpy(pszE164Number, tLocalInfo.GetE164Num(), byLength);
    return SUCCESS_AGENT;
}

/*=============================================================================
  �� �� ���� GetGkIpAddr
  ��    �ܣ� ȡGk��ַ(������)
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u32 
=============================================================================*/
u32 CAgentInterface::GetGkIpAddr()
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return htonl(tNetWorkInfo.GetGkIp());
}

/*=============================================================================
  �� �� ���� SetGKIpAddr
  ��    �ܣ� ����Gk��ַ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwGkIp
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::SetGKIpAddr(u32 dwGkIp )
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    tNetWorkInfo.SetGKIp(dwGkIp);
    return g_cCfgParse.SetNetWorkInfo(&tNetWorkInfo);
}

/*=============================================================================
  �� �� ���� GetIsGKCharge
  ��    �ܣ� ȡGK�Ƿ���мƷ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwGkIp
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CAgentInterface::GetIsGKCharge()
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return tNetWorkInfo.GetIsGKCharge();
}

/*=============================================================================
�� �� ���� GetRRQMtadpIp
��    �ܣ� ��ȡ��Э�������ַ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� u32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/3/14  4.0			������                  ����
=============================================================================*/
u32 CAgentInterface::GetRRQMtadpIp()
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return tNetWorkInfo.GetRRQMtadpIp();
}

/*=============================================================================
�� �� ���� GetGkRRQUsername
��    �ܣ� ��ȡGK RRQ �û���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� LPCSTR �û��� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2013/3/19   4.7			�´�ΰ                 ����
=============================================================================*/
u8 CAgentInterface::GetGkRRQUsePwdFlag() const
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return tNetWorkInfo.GetGkRRQUsePwdFlag();
}

/*=============================================================================
�� �� ���� GetGkRRQPassword
��    �ܣ� ��ȡGK RRQ ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� LPCSTR ����
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2013/3/19   4.7.2		�´�ΰ                 ����
=============================================================================*/
LPCSTR CAgentInterface::GetGkRRQPassword ( void )
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return tNetWorkInfo.GetGkRRQPassword();
}

/*=============================================================================
�� �� ���� SetRRQMtadpIp
��    �ܣ� ���� ��Э�������ַ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u32 dwIp ������
�� �� ֵ�� u32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2009/10/17  4.6			�ű���                  ����
=============================================================================*/
void CAgentInterface::SetRRQMtadpIp(u32 dwIp)
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    tNetWorkInfo.SetRRQMtadpIp(dwIp);
    g_cCfgParse.SetNetWorkInfo(&tNetWorkInfo);
	
    return;
}


/*=============================================================================
  �� �� ���� GetRecvStartPort
  ��    �ܣ� ȡMcu��ʼ�˿�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u16 
=============================================================================*/
u16 CAgentInterface::GetRecvStartPort()
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return tNetWorkInfo.GetRecvStartPort();
}

/*=============================================================================
  �� �� ���� SetRebootMsgDst
  ��    �ܣ� ����������Ϣ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u16 wRebootEvent
              u32 dwNode = 0
  �� �� ֵ��  void 
=============================================================================*/
void CAgentInterface::SetRebootMsgDst( u16 wRebootEvent, u32 dwNode = 0 )
{
	g_cCfgParse.SetRebootMsgDst(wRebootEvent, dwNode );
    return;
}

/*=============================================================================
  �� �� ���� SetRunningMsgDst
  ��    �ܣ� ���ôӴ�����������Ϣ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u16 wRunningEvent
              u32 dwNode = 0
  �� �� ֵ��  void 
=============================================================================*/
void CAgentInterface::SetRunningMsgDst( u16 wRunningEvent, u32 dwNode = 0 )
{

#ifndef WIN32
	g_cCfgParse.SetRunningMsgDst(wRunningEvent, dwNode );
#endif
    return;
}

/*=============================================================================
  �� �� ���� SetPowerOffMsgDst
  ��    �ܣ� ���ô����е�������Ϣ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u16 wPowerOffEvent
             u32 dwNode = 0
  �� �� ֵ�� void 
=============================================================================*/
void CAgentInterface::SetPowerOffMsgDst( u16 wPowerOffEvent, u32 dwNode = 0 )
{

#ifndef WIN32
	g_cCfgParse.SetPowerOffMsgDst(wPowerOffEvent, dwNode );
#endif
    return;
}

/*=============================================================================
  �� �� ���� GetPeriEqpType
  ��    �ܣ� ȡ��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byEqpId
  �� �� ֵ�� u8  
=============================================================================*/
u8  CAgentInterface::GetPeriEqpType(u8 byEqpId)
{
    u8  byEqpType = 0;
    u32 dwEqpIp = 0;
    g_cCfgParse.GetPriInfo(byEqpId, &dwEqpIp, &byEqpType);
    return  byEqpType;
}

/*=============================================================================
  �� �� ���� SetSystemTime
  ��    �ܣ� ����ϵͳʱ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� time_t tTime
  �� �� ֵ�� u8 
=============================================================================*/
u16 CAgentInterface::SetSystemTime(time_t tTime)
{
    return g_cCfgParse.SetSystemTime(tTime);
}
/*=============================================================================
  �� �� ���� GetBrdChoice
  ��    �ܣ� ȡ��������ѡ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byLayer
             u8 bySlot
             u8 byType
  �� �� ֵ�� u8 
=============================================================================*/
u8 CAgentInterface::GetBrdChoice(u8 byLayer, u8 bySlot, u8 byType)
{
    TBrdPosition tPos;
    tPos.byBrdLayer = byLayer;
    tPos.byBrdSlot  = bySlot;
    tPos.byBrdID    = byType;

    TBoardInfo tBrdInfo;
    g_cCfgParse.GetBrdCfgById( g_cCfgParse.GetBrdId(tPos), &tBrdInfo );
    return tBrdInfo.GetPortChoice();
}

/*=============================================================================
  �� �� ���� GetCriDriIsCast
  ��    �ܣ� ȡ�����Ƿ��鲥
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byBrdId
  �� �� ֵ�� u8 
=============================================================================*/
BOOL32 CAgentInterface::GetCriDriIsCast(u8 byBrdId)
{
    u8 byRet = g_cCfgParse.GetCriDriIsCast(byBrdId);
    return(1 == byRet);
}

/*=============================================================================
  �� �� ���� GetBrdIpAddr
  ��    �ܣ� ȡ����Ip(������)
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byBrdId:����Id
  �� �� ֵ�� u8  
=============================================================================*/
u32   CAgentInterface::GetBrdIpAddr( u8 byBrdId )
{
    /*
    TBoardInfo tBrdInfo;
    g_cCfgParse.GetBrdCfgById( byBrdId, &tBrdInfo );
    return htonl(tBrdInfo.GetBrdIp());*/

    return g_cCfgParse.GetBrdIpAddr( byBrdId );    
}

/*=============================================================================
�� �� ���� GetBoardIpAddrFromBrdId
��    �ܣ� ȡ����Ip(������)
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byBrdId:�������
�� �� ֵ�� u32  
=============================================================================*/
u32  CAgentInterface::GetBoardIpAddrFromIdx(u8 byBrdIdx)
{
    TBoardInfo tBrdInfo;
    g_cCfgParse.GetBrdCfgById( byBrdIdx, &tBrdInfo );
    return htonl(tBrdInfo.GetBrdIp());
}

/*=============================================================================
  �� �� ���� Get225245StartPort
  ��    �ܣ� ȡ225�˿�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u16  
=============================================================================*/
u16  CAgentInterface::Get225245StartPort()
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return tNetWorkInfo.Get225245StartPort();
}

/*=============================================================================
  �� �� ���� Get225245MtNum
  ��    �ܣ� ȡ�ն���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u16  
=============================================================================*/
u16  CAgentInterface::Get225245MtNum()
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return tNetWorkInfo.Get225245MtNum();
}

/*=============================================================================
  �� �� ���� SetQosInfo
  ��    �ܣ� ����Qos��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TQosInfo tQosInfo
  �� �� ֵ�� BOOL32 
=============================================================================*/
u16 CAgentInterface::SetQosInfo(TQosInfo tQosInfo )
{
    return g_cCfgParse.SetQosInfo(&tQosInfo);
}

/*=============================================================================
  �� �� ���� GetMpcBoardId
  ��    �ܣ� ȡMpc���Id
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� u8 
=============================================================================*/
u8 CAgentInterface::GetMpcBoardId(void)
{
    u8 byMpcId = 0;
    TMPCInfo tMPCInfo;
    g_cCfgParse.GetMPCInfo( &tMPCInfo );

    if(0 == tMPCInfo.GetLocalSlot())
    {
        byMpcId = MCU_BOARD_MPC;
    }
    else if(1 == tMPCInfo.GetLocalSlot())
    {
        byMpcId = MCU_BOARD_MPCD;
    }
    return byMpcId;
}

/*=============================================================================
  �� �� ���� GetBoardInfo
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwBrdIp
             u8* pbyBrdId
             u8* pbyLayer
             u8* pbySlot
   �� �� ֵ�� u8 
 =============================================================================*/
u16 CAgentInterface::GetBoardInfo(u32 dwBrdIp, u8* pbyBrdId, u8* pbyLayer, u8* pbySlot)
{
    u16 wRet = ERR_AGENT_GETPERIINFO;
    if( NULL == pbyBrdId )
    {
        Agtlog( LOG_ERROR, "[GetBoardInfo] param err: pbyBrdId.0x%x !\n", pbyBrdId );
        return wRet;
    }
    for(u8 byBrdId = 1; byBrdId <= MAX_BOARD_NUM; byBrdId ++)
    {
        TBoardInfo tBrdInfo;
        if ( g_cCfgParse.GetBrdCfgById( byBrdId, &tBrdInfo ) ) 
        {
            if ( ntohl(dwBrdIp) == tBrdInfo.GetBrdIp() )
            {
                *pbyBrdId = tBrdInfo.GetBrdId();
                if ( NULL != pbyLayer)
                {
                    *pbyLayer = tBrdInfo.GetLayer();
                }
                if ( NULL != pbySlot )
                {
                    *pbySlot = tBrdInfo.GetSlot();
                }
                wRet = SUCCESS_AGENT;
            }
        }
    }
    return wRet;
}

/*=============================================================================
  �� �� ���� SetMpcIp
  ��    �ܣ� ����Mpc��Ip: ������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwIp
  �� �� ֵ�� u8  
=============================================================================*/
u16   CAgentInterface::SetMpcIp(u32 dwIp, u8 byInterface)
{
    return g_cCfgParse.SetLocalIp(dwIp, byInterface);
} 

/*=============================================================================
  �� �� ���� GetInterface
  ��    �ܣ� ȡǰ������ѡ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� u32 
=============================================================================*/
u8 CAgentInterface::GetInterface(void)
{
    TMPCInfo tMPCInfo;
    g_cCfgParse.GetMPCInfo( &tMPCInfo );
	return tMPCInfo.GetLocalPortChoice();
}

/*=============================================================================
  �� �� ���� SetInterface
  ��    �ܣ� ����ǰ������ѡ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byInterface
  �� �� ֵ�� u16 
=============================================================================*/
u16 CAgentInterface::SetInterface(u8 byInterface)
{
    TMPCInfo tMPCInfo;
    g_cCfgParse.GetMPCInfo( &tMPCInfo );
    tMPCInfo.SetLocalPortChoice( byInterface );
	g_cCfgParse.SetMPCInfo(tMPCInfo);
	return SUCCESS_AGENT;
}
/*=============================================================================
  �� �� ���� GetMaskIp
  ��    �ܣ� ��������(������)
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u32  ������
=============================================================================*/
u32  CAgentInterface::GetMaskIp()
{
    return g_cCfgParse.GetMpcMaskIp();
}

/*=============================================================================
  �� �� ���� SetMaskIp
  ��    �ܣ� ������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwIp
  �� �� ֵ�� u8  
=============================================================================*/
u16   CAgentInterface::SetMaskIp(u32 dwIp, u8 byInterface)
{
    return g_cCfgParse.SetMpcMaskIp(dwIp, byInterface);
}

/*=============================================================================
  �� �� ���� GetGateway
  ��    �ܣ� ȡ����Ip,������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u32  
=============================================================================*/
u32  CAgentInterface::GetGateway()
{
    return g_cCfgParse.GetMpcGateway();
}
/*=============================================================================
  �� �� ���� SetGateway
  ��    �ܣ� ��������Ip(������)
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwIp
  �� �� ֵ�� u8  
=============================================================================*/
u16   CAgentInterface::SetGateway(u32 dwIp)
{
	return g_cCfgParse.SetMpcGateway(dwIp);
}
    
/*=============================================================================
  �� �� ���� GetHWVersion
  ��    �ܣ� ȡӲ���汾�Ų�ѯ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u8  
=============================================================================*/
u8 CAgentInterface::GetHWVersion()
{
    return g_cCfgParse.GetMpcHWVersion();
}

/*=============================================================================
  �� �� ���� GetCpuRate
  ��    �ܣ� ȡCpu ռ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u8  
=============================================================================*/
u8  CAgentInterface::GetCpuRate()
{
    return g_cCfgParse.GetCpuRate();
}

/*=============================================================================
  �� �� ���� GetMemoryRate
  ��    �ܣ� ȡMemoryռ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u8  
=============================================================================*/
u32  CAgentInterface::GetMemoryRate()
{
    return g_cCfgParse.GetMemoryRate();
}

/*=============================================================================
  �� �� ���� RebootBoard
  ��    �ܣ� ����ָ������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TBrdPosition tBrdPos
  �� �� ֵ�� void 
=============================================================================*/
u16 CAgentInterface::RebootBoard(u8 byLayer, u8 bySlot, u8 byType)
{
    return g_cCfgParse.RebootBoard(byLayer, bySlot, byType);
}


/*=============================================================================
  �� �� ���� GetLocalLayer
  ��    �ܣ� �������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8& byLayer
  �� �� ֵ�� u16 
=============================================================================*/
u16 CAgentInterface::GetLocalLayer(u8& byLayer)
{
    TMPCInfo tMPCInfo;
    g_cCfgParse.GetMPCInfo( &tMPCInfo );
    byLayer = tMPCInfo.GetLocalLayer();
    return SUCCESS_AGENT;
}

/*=============================================================================
  �� �� ���� GetLocalSlot
  ��    �ܣ� �����ۺ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8& bySlot
  �� �� ֵ�� u16 
=============================================================================*/
u16 CAgentInterface::GetLocalSlot(u8& bySlot)
{
    TMPCInfo tMPCInfo;
    g_cCfgParse.GetMPCInfo( &tMPCInfo );
    bySlot = tMPCInfo.GetLocalSlot();
    return SUCCESS_AGENT;
}



#ifdef _LINUX_

u32 CAgentInterface::EthIdxSys2Hard( u32 dwSysIdx )
{
	return g_cCfgParse.EthIdxSys2Hard( dwSysIdx );
}

#endif

#ifdef _MINIMCU_
/*=============================================================================
  �� �� ���� Get8000BInfo
  ��    �ܣ� ���8000B��DSC����������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TDSCInfo *ptMcu8000BInfo
  �� �� ֵ�� u16 
=============================================================================*/
u16 CAgentInterface::GetDscInfo( TDSCModuleInfo *ptMcuDscInfo )
{
    return g_cCfgParse.GetDSCInfo( ptMcuDscInfo );
}


/*=============================================================================
�� �� ���� GetLastDscInfo
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TDSCModuleInfo * ptDscInfo
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/2   4.0			�ܹ��                  ����
=============================================================================*/
u16	CAgentInterface::GetLastDscInfo( TDSCModuleInfo * ptDscInfo )
{
	return g_cCfgParse.GetLastDscInfo( ptDscInfo );
}

/*=============================================================================
�� �� ���� SetLastDscInfo
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TDSCModuleInfo * ptDscInfo
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/2   4.0			�ܹ��                  ����
=============================================================================*/
u16	CAgentInterface::SetLastDscInfo( TDSCModuleInfo * ptDscInfo )
{
	return g_cCfgParse.SetLastDscInfo( ptDscInfo );
}

/*=============================================================================
�� �� ���� GetConfigedDscType
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� u8(���������DSC���򷵻�DSC�����ͣ����򷵻�0) 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/2/28   4.0			�ܹ��                  ����
2007/3/05	4.0			�ܹ��					�޸�
=============================================================================*/
u8 CAgentInterface::GetConfigedDscType( void )
{
	return g_cCfgParse.GetConfigedDscType();
}

/*=============================================================================
  �� �� ���� SetDscInfo
  ��    �ܣ� ����8000B��DSC����������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TDSCModuleInfo *ptMcuDscInfo, BOOL32 bToFile
  �� �� ֵ�� u16 
=============================================================================*/
u16 CAgentInterface::SetDscInfo( TDSCModuleInfo *ptMcuDscInfo, BOOL32 bToFile )
{
    return g_cCfgParse.SetDSCInfo( ptMcuDscInfo, bToFile);
}

/*=============================================================================
�� �� ���� SaveDscLocalInfoToNip
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TDSCModuleInfo * ptDscInfo
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/17   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CAgentInterface::SaveDscLocalInfoToNip( TDSCModuleInfo * ptDscInfo )
{
	return g_cCfgParse.SaveDscLocalInfoToNip( ptDscInfo );
}

/*=============================================================================
�� �� ���� SaveRouteToNipByDscInfo
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TDSCModuleInfo *ptDscInfo
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/9/21   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CAgentInterface::SaveRouteToNipByDscInfo( TDSCModuleInfo *ptDscInfo )
{
	return g_cCfgParse.SaveRouteToNipByDscInfo( ptDscInfo );
}

/*=============================================================================
�� �� ���� IsDscReged
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� ��
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/3   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CAgentInterface::IsDscReged(u8 &byDscType)
{
	u8 byDscAgtType = BRD_TYPE_UNKNOW;
	BOOL32 bRet = g_cCfgParse.IsDscReged(byDscAgtType);
	// [pengjie 2010/4/26] �ӿڸı�
	//byDscType = g_cCfgParse.GetBrdType4Mcu(byDscAgtType);
	byDscType = g_cCfgParse.GetSnmpBrdTypeFromHW(byDscAgtType);
	return bRet;
}
#endif

/*=============================================================================
  �� �� ���� GetMpRunBrdTypeByMpIdx
  ��    �ܣ� ͨ����Ĳ�����������ѯ���а�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u8
=============================================================================*/
u8 CAgentInterface::GetRunBrdTypeByIdx( u8 byMpId )
{
    return g_cCfgParse.GetRunBrdTypeByIdx(byMpId);
}

/*=============================================================================
�� �� ���� GetMcuCfgInfoLevel
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� u8 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/25   4.0			�ܹ��                  ����
=============================================================================*/
u8 CAgentInterface::GetMcuCfgInfoLevel(void)
{
	TMcuSystem tSystem;
	g_cCfgParse.GetSystemInfo( &tSystem );
	return tSystem.GetMcuCfgInfoLevel();
}

/*=============================================================================
  �� �� ���� IsMcuPdtBrdMatch
  ��    �ܣ� ���ҵ����MPC�������Ƿ�ƥ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32
=============================================================================*/
BOOL32 CAgentInterface::IsMcuPdtBrdMatch(u8 byPdtType) const
{
    return g_cCfgParse.IsMcuPdtBrdMatch(byPdtType);
}

/*=============================================================================
�� �� ���� IsMcu8000BHD
��    �ܣ� �Ƿ��� MCU-8000B-HD
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� u8 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/03/27  4.5			����                  ����
=============================================================================*/
#ifdef _MINIMCU_
BOOL32 CAgentInterface::IsMcu8000BHD() const
{
    return g_cCfgParse.IsMcu8000BHD();
}
#endif

// VCS
/*=============================================================================
�� �� ���� SetVCSSoftName
��    �ܣ� ����VCS����Զ�������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� u16 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
09/05/11    4.5			���㻪                  ����
=============================================================================*/
u16 CAgentInterface::SetVCSSoftName( s8* pachSoftName )
{
    return g_cCfgParse.SetVCSSoftName( pachSoftName );
}
/*=============================================================================
�� �� ���� GetVCSSoftName
��    �ܣ� ��ȡVCS����Զ�������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� void 
  ----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
09/05/11    4.5			���㻪                  ����
=============================================================================*/
void CAgentInterface::GetVCSSoftName( s8* pachSoftName )
{
    g_cCfgParse.GetVCSSoftName( pachSoftName );
}

/*=============================================================================
�� �� ���� SetEqpExCfgInfo
��    �ܣ� ����������չ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� IN  TEqpExCfgInfo &tEqpExCfgInfo
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
20100108    4.6			pengjie                create
=============================================================================*/
u16 CAgentInterface::SetEqpExCfgInfo( TEqpExCfgInfo &tEqpExCfgInfo )
{
	u16 wRet = SUCCESS_AGENT;
	g_cCfgParse.SetEqpExCfgInfo( tEqpExCfgInfo );
    return wRet;
}

/*=============================================================================
�� �� ���� GetEqpExCfgInfo
��    �ܣ� �õ�������չ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� OUT  TEqpExCfgInfo &tEqpExCfgInfo
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
20100108    4.6			pengjie                create
=============================================================================*/
u16 CAgentInterface::GetEqpExCfgInfo( TEqpExCfgInfo &tEqpExCfgInfo )
{
	u16 wRet = SUCCESS_AGENT;
	g_cCfgParse.GetEqpExCfgInfo( tEqpExCfgInfo );
    return wRet;
}

u16 CAgentInterface::SetMcuCompileTime(s8 *pszCompileTime)
{
	u16 wRet = SUCCESS_AGENT;
	g_cCfgParse.SetMcuCompileTime( pszCompileTime );
    return wRet;
}

u16 CAgentInterface::SetAuthMTNum(u16 wAuthMTNum)
{
	u16 wRet = SUCCESS_AGENT;
	TMcuPfmInfo* ptMcuPfmInfo = g_cCfgParse.GetMcuPfmInfo();
	if (ptMcuPfmInfo == NULL)
	{
		return ~wRet;
	}
	ptMcuPfmInfo->SetAuthMTNum(wAuthMTNum);
	return wRet;
}

u16 CAgentInterface::SetAuthMTTotal(u16 wAuthTotal)
{
	u16 wRet = SUCCESS_AGENT;
	TMcuPfmInfo* ptMcuPfmInfo = g_cCfgParse.GetMcuPfmInfo();
	if (ptMcuPfmInfo == NULL)
	{
		return ~wRet;
	}
	ptMcuPfmInfo->SetAuthMTTotal(wAuthTotal);
	return wRet;
}

u16 CAgentInterface::SetEqpStat(u8 byEqpType, u16 wEqpUsedNum, u16 wEqpTotal)
{
	u16 wRet = SUCCESS_AGENT;
	TMcuPfmInfo* ptMcuPfmInfo = g_cCfgParse.GetMcuPfmInfo();
	if (ptMcuPfmInfo == NULL)
	{
		return ~wRet;
	}
	ptMcuPfmInfo->SetEqpStat(byEqpType, wEqpUsedNum, wEqpTotal);
	return wRet;
}

void CAgentInterface::GetIpFromU32(s8* achDstStr, u32 dwIpAddr)
{
	g_cCfgParse.GetIpFromU32(achDstStr, dwIpAddr);
}

// ����8ke ר��
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
#ifdef _8KI_
u16 CAgentInterface::SetNewNetCfgInfo(const TNewNetCfg &tNewNetcfgInfo, const s8 bySecEthIndx/* = -1*/)
{
	return g_cCfgParse.SetNewNetCfgInfo( tNewNetcfgInfo, bySecEthIndx );
}

u16 CAgentInterface::GetNewNetCfgInfo(TNewNetCfg &tNewNetCfgInfo)
{
	return g_cCfgParse.GetNewNetCfgInfo( tNewNetCfgInfo );
}

u16	CAgentInterface::GetMacByEthIdx( const u8 &byEthIdx,s8* achMac,const u8 &byMacLen )
{	
	if( byMacLen < MAX_MACADDR_STR_LEN )
	{
		return ERR_AGENT_VALUEBESET;
	}

	if( byEthIdx >= MAXNUM_ETH_INTERFACE )
	{
		return ERR_AGENT_VALUEBESET; 
	}
#ifdef _LINUX_	
	TEthInfo tEthInfo[MAXNUM_ETH_INTERFACE];
	u8 byEthNum = 0;

	GetEthInfo(tEthInfo, byEthNum);

	memcpy( achMac, tEthInfo[byEthIdx].GetMacAddrStr(), MAX_MACADDR_STR_LEN );


#endif

	return SUCCESS_AGENT;
}

#endif

u16 CAgentInterface::SetSipRegGkInfo( u32 dwGKIp )
{
	return g_cCfgParse.SetSipRegGkInfo( dwGKIp );
}

u16 CAgentInterface::SetProxyDMZInfo( TProxyDMZInfo &tProxyDMZInfo )
{
	u16 wRet = SUCCESS_AGENT;
	g_cCfgParse.SetProxyDMZInfo( tProxyDMZInfo );
    return wRet;
}

u16 CAgentInterface::GetProxyDMZInfo( TProxyDMZInfo &tProxyDMZInfo )
{
	u16 wRet = SUCCESS_AGENT;
	g_cCfgParse.GetProxyDMZInfo( tProxyDMZInfo );
    return wRet;
}

u16 CAgentInterface::GetProxyDMZInfoFromMcuCfgFile( TProxyDMZInfo &tProxyDMZInfo )
{
	u16 wRet = SUCCESS_AGENT;
	g_cCfgParse.GetProxyDMZInfoFromMcuCfgFile( tProxyDMZInfo );
    return wRet;
}

u16 CAgentInterface::SetGkProxyCfgInfo( TGKProxyCfgInfo &tGKProxyCfgInfo )
{
	u16 wRet = SUCCESS_AGENT;
	g_cCfgParse.SetGkProxyCfgInfo( tGKProxyCfgInfo );
    return wRet;
}

u16 CAgentInterface::GetGkProxyCfgInfo( TGKProxyCfgInfo &tGKProxyCfgInfo )
{
	u16 wRet = SUCCESS_AGENT;
	g_cCfgParse.GetGkProxyCfgInfo( tGKProxyCfgInfo );
    return wRet;
}

u16 CAgentInterface::SetPrsTimeSpanCfgInfo( TPrsTimeSpan &tPrsTimeSpan )
{
	u16 wRet = SUCCESS_AGENT;
	g_cCfgParse.SetPrsTimeSpanCfgInfo( tPrsTimeSpan );
    return wRet;
}

u16 CAgentInterface::GetPrsTimeSpanCfgInfo( TPrsTimeSpan &tPrsTimeSpan )
{
	u16 wRet = SUCCESS_AGENT;
	g_cCfgParse.GetPrsTimeSpanCfgInfo( tPrsTimeSpan );
    return wRet;
}

u16 CAgentInterface::GetMcuEqpCfg( TMcu8KECfg * ptMcuEqpCfg )
{
    return g_cCfgParse.GetMcuEqpCfg( ptMcuEqpCfg );
}

/*=============================================================================
  �� �� ���� SetMcu8KIEqpCfg
  ��    �ܣ� ����8KI�������õ��ڴ��У���������IP���� ���� �������ü�·�����ã�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TMcu8KECfg* ptMcu8KECfg:����������Ϣ
  �� �� ֵ�� u16 
=============================================================================*/
u16 CAgentInterface::SetMcu8KIEqpCfg( TMcu8KECfg * ptMcuEqpCfg )
{
    return g_cCfgParse.SetMcu8KIEqpCfg( ptMcuEqpCfg );
}

u16 CAgentInterface::SetMcuEqpCfg(TNetAdaptInfoAll * ptNetAdaptInfoAll)
{
	return g_cCfgParse.SetMcuEqpCfg(ptNetAdaptInfoAll);
}

u16 CAgentInterface::SetRouteCfg(TMultiNetCfgInfo &tMultiNetCfgInfo)
{
#if defined(_8KE_) || defined(_8KH_)
	return g_cCfgParse.SetRouteCfg(tMultiNetCfgInfo);
#else
	//8000I���ṩ·�����ĸ����ڵ���Ϣ,������Ҫ��������·�����ĸ�����(ͨ����ip���ñȽϻ��)
	TNewNetCfg tNewNetCfg;
	TEthCfg tEthCfg;
	TNetCfg tNetCfg;
	GetNewNetCfgInfo( tNewNetCfg );
	u8 byRouteIdx = 0;
	u8 byRouteEthIdx = 0;
	u8 byRouteEthSubIdx = 0;
	TMultiNetCfgInfo tResultCfgInfo;
	u32 dwRouteIp = 0,dwIp = 0;
	u32 dwRouteMask = 0,dwMask = 0;
	u32 dwRouteGateIp = 0;
	u8 byIpIdx = 0;
	u8 byIsFind = 0;
	u32 adwRouteIp[MAXNUM_ETH_INTERFACE*MCU_MAXNUM_ADAPTER_IP];
	u32 adwRouteMask[MAXNUM_ETH_INTERFACE*MCU_MAXNUM_ADAPTER_IP];
	u32 adwRouteGate[MAXNUM_ETH_INTERFACE*MCU_MAXNUM_ADAPTER_IP];
	u8 byOtherRouteIdx = 0;
	u8 bIsInsert = 0;
	
	for( byRouteIdx = 0;byRouteIdx<MAXNUM_ETH_INTERFACE*MCU_MAXNUM_ADAPTER_IP;++byRouteIdx,++byRouteEthSubIdx )
	{
		byRouteEthIdx    = byRouteIdx / MCU_MAXNUM_ADAPTER_IP;
		byRouteEthSubIdx = byRouteIdx % MCU_MAXNUM_ADAPTER_IP;

		/*if(  0 == byRouteEthSubIdx && byRouteIdx > 0 )
		{
			byIpIdx = 0;
			byRouteEthIdx++;
		}*/

		if( tMultiNetCfgInfo.GetMcuEthCfg(byRouteEthIdx) == NULL )
		{
			continue;
		}

		if( byRouteEthSubIdx >= tMultiNetCfgInfo.GetMcuEthCfg(byRouteEthIdx)->GetIpSecNum() )
		{
			continue;
		}

		dwRouteIp	= tMultiNetCfgInfo.GetMcuEthCfg(byRouteEthIdx)->GetMcuIpAddr(byRouteEthSubIdx);
		dwRouteMask = tMultiNetCfgInfo.GetMcuEthCfg(byRouteEthIdx)->GetMcuSubNetMask(byRouteEthSubIdx);
		dwRouteGateIp = tMultiNetCfgInfo.GetMcuEthCfg(byRouteEthIdx)->GetGWIpAddr(byRouteEthSubIdx);

		if( 0 == dwRouteIp )
		{
			continue;
		}

		byIsFind = 0;
		bIsInsert = 0;
		for( u8 byIdx = 0;byIdx < MAXNUM_ETH_INTERFACE;++byIdx )
		{
			tEthCfg.Clear();
			
			tNewNetCfg.GetEthCfg( byIdx,tEthCfg );
			for( u8 byIpIdx = 0;byIpIdx < MCU_MAXNUM_ADAPTER_IP;++byIpIdx )
			{
				tNetCfg.Clear();
				tEthCfg.GetNetCfg( byIpIdx,tNetCfg );	
				
				dwIp   = tNetCfg.GetIpAddr();
				dwMask = tNetCfg.GetIpMask();

				if( 0 == dwIp )
				{
					continue;
				}
		
				//if( (dwRouteIp & dwRouteMask) == (dwIp & dwMask) )
				if( (dwRouteGateIp & dwMask) == (dwIp & dwMask) )
				{
					if( tResultCfgInfo.GetMcuEthCfg(byIdx) != NULL )
					{
						for( byIpIdx = 0;byIpIdx < MCU_MAXNUM_ADAPTER_IP;++byIpIdx )
						{
							if( tResultCfgInfo.GetMcuEthCfg(byIdx)->GetMcuIpAddr(byIpIdx) == 0 )
							{
								tResultCfgInfo.GetMcuEthCfg(byIdx)->AddIpSection( dwRouteIp,dwRouteMask,dwRouteGateIp);
								//	SetMcuIpAddr( dwRouteIp,byIpIdx );
								//tResultCfgInfo.GetMcuEthCfg(byIdx)->SetMcuSubNetMask( dwRouteMask,byIpIdx );
								//tResultCfgInfo.GetMcuEthCfg(byIdx)->SetGWIpAddr( dwRouteGateIp,byIpIdx );
								bIsInsert = 1;
								break;
							}
						}

					}
					byIsFind = 1;

					break;
				}
			}
			if( 1 == byIsFind )
			{
				break;
			}			
		}
		if( 0 == byIsFind || 0 == bIsInsert )
		{
			adwRouteIp[byOtherRouteIdx]   = dwRouteIp;
			adwRouteMask[byOtherRouteIdx] = dwRouteMask;
			adwRouteGate[byOtherRouteIdx] = dwRouteGateIp;
			++byOtherRouteIdx;
		}
	}

	byRouteEthIdx = 0;
	byIpIdx		  = 0;
	for( byRouteIdx = 0;byRouteIdx<byOtherRouteIdx; )
	{
		for( byIpIdx = 0;byIpIdx < MCU_MAXNUM_ADAPTER_IP;++byIpIdx )
		{		
			if( tResultCfgInfo.GetMcuEthCfg(byRouteEthIdx)->GetMcuIpAddr(byIpIdx) == 0 )
			{				
				tResultCfgInfo.GetMcuEthCfg(byRouteEthIdx)->AddIpSection( adwRouteIp[byRouteIdx],
					adwRouteMask[byRouteIdx],adwRouteGate[byRouteIdx] );
				//	SetMcuIpAddr( adwRouteIp[byRouteIdx],byIpIdx );
				//tResultCfgInfo.GetMcuEthCfg(byRouteEthIdx)->SetMcuSubNetMask( adwRouteMask[byRouteIdx],byIpIdx );
				//tResultCfgInfo.GetMcuEthCfg(byRouteEthIdx)->SetGWIpAddr( adwRouteGate[byRouteIdx],byIpIdx );
				++byRouteIdx;
				//tResultCfgInfo.GetMcuEthCfg(byRouteEthIdx)->ModifyIpSection()
				break;
			}
		}
		if( MCU_MAXNUM_ADAPTER_IP == byIpIdx )
		{
			byRouteEthIdx++;
			if( byRouteEthIdx >= MAXNUM_ETH_INTERFACE )
			{
				break;
			}
		}
	}

	return g_cCfgParse.SetRouteCfg( tResultCfgInfo );
#endif


}


u16 CAgentInterface::SetMultiManuNetCfg( const TMultiManuNetCfg &tMultiManuNetCfg, const TMultiEthManuNetAccess *tMultiEthManuNetAccess, const u8 byEthNum )
{
	return g_cCfgParse.SetMultiManuNetCfg(tMultiManuNetCfg, tMultiEthManuNetAccess, byEthNum);
}

u16 CAgentInterface::GetMultiNetCfgInfo(TMultiNetCfgInfo &tMultiNetCfgInfo)
{
	return g_cCfgParse.GetMultiNetCfgInfo(tMultiNetCfgInfo);
}

u16 CAgentInterface::GetMultiManuNetAccess(TMultiManuNetAccess &tMultiManuNetAccess,BOOL32 bFromMcuCfgFile, TMultiEthManuNetAccess *tMultiEthManuNetAccess, u8 *byEthNum)
{
	return g_cCfgParse.GetMultiManuNetAccess(tMultiManuNetAccess,bFromMcuCfgFile,tMultiEthManuNetAccess, byEthNum);
}

u16 CAgentInterface::WriteMultiManuNetAccess(const TMultiManuNetAccess &tMultiManuNetAccess, const TMultiEthManuNetAccess *tMultiEthManuNetAccess, const u8 byEthNum)
{
	return g_cCfgParse.WriteMultiManuNetAccess(tMultiManuNetAccess, tMultiEthManuNetAccess, byEthNum);
}

BOOL32 CAgentInterface::GetGkProxyCfgInfoFromCfgFile ( TGKProxyCfgInfo &tgkProxyCfgInfo )
{
	s8 achCfgName[256]={0};
	sprintf( achCfgName, "%s/%s", DIR_CONFIG, MCUCFGFILENAME );
	
	BOOL32 bResult = g_cCfgParse.AgentGetGkProxyCfgInfo( achCfgName );
	g_cCfgParse.GetGkProxyCfgInfo(tgkProxyCfgInfo);
	
	return bResult;
}

/*=============================================================================
  �� �� ���� Save8KENetCfgToSys
  ��    �ܣ� ˢ��8KE mcuϵͳ����������,������������IP���� ���� �������ü�·������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TMcu8KECfg* ptMcu8KECfg:����������Ϣ
  �� �� ֵ�� u16 
=============================================================================*/
u16 CAgentInterface::Save8KENetCfgToSys(TMcu8KECfg * ptMcu8KECfg)
{
	return g_cCfgParse.Save8KENetCfgToSys(ptMcu8KECfg);
}
 
void CAgentInterface::SetRouteToSys(void)
{
	g_cCfgParse.SetRouteToSys();
}
/*=============================================================================
  �� �� ���� SearchPortChoice
  ��    �ܣ� ���ҵ�ǰʹ�õ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u8 
=============================================================================*/
u8  CAgentInterface::SearchPortChoice(u32 dwIpAddr /* = 0 */)
{
	return g_cCfgParse.SearchPortChoice(dwIpAddr);
}
#endif
//[2011/02/11 zhushz] mcs�޸�mcu ip
/*=============================================================================
�� �� ���� GetNewNetCfg
��    �ܣ� ȡ������������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� void 
=============================================================================*/
void CAgentInterface::GetNewNetCfg(TMcuNewNetCfg& tMcuNewNetInfo)
{
	g_cCfgParse.GetNewMpcNetCfg(tMcuNewNetInfo);
	return;
}
/*=============================================================================
�� �� ���� GetNewNetCfg
��    �ܣ� ��������������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� void 
=============================================================================*/
void CAgentInterface::SetNewNetCfg(const TMcuNewNetCfg& tMcuNewNetInfo)
{
	g_cCfgParse.SetNewMpcNetCfg(tMcuNewNetInfo);
	return;
}
/*=============================================================================
�� �� ���� GetNewNetCfg
��    �ܣ� �Ƿ���������������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CAgentInterface::IsMpcNetCfgBeModifedByMcs()
{
	return g_cCfgParse.IsMpcNetCfgBeModifedByMcs();
}
/*=============================================================================
�� �� ���� GetNewNetCfg
��    �ܣ� �����Ƿ��������������ñ�־
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CAgentInterface::SetIsNetCfgBeModifed(BOOL32 bNetInfoBeModify)
{
	return g_cCfgParse.SetIsNetCfgBeModifed(bNetInfoBeModify);
}

/*====================================================================
    ������        GetMPU2TypeByVmpEqpId
    ����        ������vmp����Id��ȡ����MPU2�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byVmpEqpId vmp����ID
    ����ֵ˵��  ��u8 ��������.�����ڷ���BRD_TYPE_UNKNOW.
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	11/12/12    4.7         �´�ΰ           ����
====================================================================*/
u8 CAgentInterface::GetMPU2TypeByVmpEqpId(u8 byVmpEqpId)
{
	u8 byBrdType = BRD_TYPE_UNKNOW;
	if(byVmpEqpId <VMPID_MIN || byVmpEqpId > VMPID_MAX)
	{
		Agtlog(LOG_ERROR, "[GetMPU2TypeByVmpEqpId] unexpected Eqp ID!\n");
		
		return byBrdType;
	}

	TEqpVMPInfo tVmpInfo;
	if(GetEqpVMPCfg(byVmpEqpId,&tVmpInfo) == SUCCESS_AGENT)
	{
		TBoardInfo tBrdInfo;
		g_cCfgParse.GetBrdCfgById(tVmpInfo.GetRunBrdId(),&tBrdInfo);

		byBrdType = tBrdInfo.GetType();
	}
	else
	{
		Agtlog(LOG_ERROR, "[GetMPU2TypeByVmpEqpId]can not find byId(%d) in all VmpTable!\n", byVmpEqpId );
	}

	return byBrdType;
}

/*====================================================================
    ������        GetMPU2TypeByVmpEqpId
    ����        ������bas����Id��ȡ����MPU2�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byVmpEqpId vmp����ID
    ����ֵ˵��  ��u8 ��������.�����ڷ���BRD_TYPE_UNKNOW.
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	2012/11/08   4.7         ��־��           ����
====================================================================*/
u8 CAgentInterface::GetMPU2TypeByEqpId(u8 byEqpId,u8 &byBrdId)
{
	u8 byBrdType = BRD_TYPE_UNKNOW;
	TBoardInfo tBrdInfo;
	if(byEqpId >= BASID_MIN && byEqpId <= BASID_MAX)
	{	
		TEqpMpuBasInfo tBasInfo;
		if(GetMpuBasBrdInfo(byEqpId,&tBasInfo) == SUCCESS_AGENT)
		{			
			g_cCfgParse.GetBrdCfgById(tBasInfo.GetRunBrdId(),&tBrdInfo);

			byBrdType = tBrdInfo.GetType();
			byBrdId = tBasInfo.GetRunBrdId();
		}
		return byBrdType;
	}

	if(byEqpId >= VMPID_MIN && byEqpId <= VMPID_MAX)
	{	
		TEqpSvmpInfo tVmpInfo;
		if(GetEqpSvmpCfgById(byEqpId,&tVmpInfo) == SUCCESS_AGENT)
		{			
			g_cCfgParse.GetBrdCfgById(tVmpInfo.GetRunBrdId(),&tBrdInfo);
			byBrdType = tBrdInfo.GetType();
			byBrdId = tVmpInfo.GetRunBrdId();
		}
		return byBrdType;
	}	

	Agtlog(LOG_ERROR, "[GetMPU2TypeByBasEqpId] unexpected Eqp ID!\n");
	return byBrdType;
}

/*====================================================================
    ������        IsLowMpu2EqpId
    ����        ��MPU2 ECARD BASICģʽ�������һ��EQPID
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byVmpEqpId vmp����ID
    ����ֵ˵��  ��u8 ��������.�����ڷ���BRD_TYPE_UNKNOW.
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	2012/11/08   4.7         ��־��           ����
====================================================================*/
BOOL32 CAgentInterface::GetMpu2AnotherEqpId(u8 byEqpId,u8 &byAnotherId)
{
	//У���Ƿ�Ecard��
	u8 byBrdId =0;
	if ( BRD_TYPE_MPU2ECARD != GetMPU2TypeByEqpId(byEqpId,byBrdId) )
	{
		return FALSE;
	}

	//��õ�����Ϣ
	TBoardInfo tBrdInfo;
    u16 wRet = GetBrdCfgById( byBrdId, &tBrdInfo );

	//���ݵ�����Ϣ�ж��Ƿ���ECARD�忨����������basic����

	if ( wRet == SUCCESS_AGENT
		)
	{
		u8 byPeriCount = 0;
		u8 abyPeriId[MAXNUM_BRD_EQP] = {0};
		tBrdInfo.GetPeriId( abyPeriId, &byPeriCount );
		if ( byPeriCount == 2  )
		{
			u8 byMinEqpId = min(abyPeriId[0],abyPeriId[1]);
			if ( byEqpId == byMinEqpId )
			{
				byAnotherId = max(abyPeriId[0],abyPeriId[1]);	//���ظߵ�eqpid	
			}
			else
			{
				byAnotherId =byMinEqpId;
			}
			return TRUE;
		}	
	}

	return FALSE;
}

/*====================================================================
    ������        GetAPU2BasEqpIdByMixerId
    ����        ��apu2 ͨ������������id���������һ����Ƶ����������EQPID
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMixerEqpId apu2�����ϻ�����������id
    ����ֵ˵��  ��u8 &byBasId ͬһAPU2����������apu2��Ƶ��������id
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	2013/05/14   4.7         �´�ΰ           ����
====================================================================*/
BOOL32 CAgentInterface::GetAPU2BasEqpIdByMixerId(u8 byMixerEqpId,u8 &byBasId)
{
	if(byMixerEqpId >= MIXERID_MIN && byMixerEqpId <= MIXERID_MAX)
	{	
		u8 byBrdId =0;
		TEqpMixerInfo tMixerInfo;
		TBoardInfo tBrdInfo;
		if(GetEqpMixerCfg(byMixerEqpId,&tMixerInfo) == SUCCESS_AGENT)
		{			
			g_cCfgParse.GetBrdCfgById(tMixerInfo.GetRunBrdId(),&tBrdInfo);
			
			byBrdId = tMixerInfo.GetRunBrdId();
			
			u8 byPeriCount = 0;
			u8 abyPeriId[MAXNUM_BRD_EQP] = {0};
			tBrdInfo.GetPeriId( abyPeriId, &byPeriCount );
			if(tBrdInfo.GetType() == BRD_TYPE_APU2)
			{
				for( u8 byIdx = 0; byIdx < byPeriCount; byIdx++)
				{
					if(abyPeriId[byIdx] >= BASID_MIN && abyPeriId[byIdx]<= BASID_MAX)
					{
						byBasId = abyPeriId[byIdx];
						
						return TRUE;
					}
				}
			}
		}
	}
	
	return FALSE;
}


/*====================================================================
    ������        GetPeriEqpMcuRecvStartPort
    ����        ��Ϊ�ض������ض���������MCU�������ʼ�˿�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byEqpType ��������
				  u8 bySubEqpType ���������ͣ�����MPU2���basic vmp���ͣ�
    ����ֵ˵��  ��u16 MCU�������ʼ�˿ڣ���ȡʧ�ܷ���0
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	11/12/12    4.7         �´�ΰ           ����
====================================================================*/
u16 CAgentInterface::GetPeriEqpMcuRecvStartPort(u8 byEqpType,u8 bySubEqpType)
{
	u16 wRecvStartPort = 0;

	switch(byEqpType)
	{
	case EQP_TYPE_MIXER:
		if(bySubEqpType == APU_MIXER || bySubEqpType == EAPU_MIXER || bySubEqpType == MIXER_8KH || bySubEqpType == MIXER_8KG )
		{
			wRecvStartPort = m_wMixerMcuRecvStartPort;
			m_wMixerMcuRecvStartPort += MIXER_MCU_PORTSPAN;
		}
		else if (bySubEqpType == APU2_MIXER || bySubEqpType == MIXER_8KI )
		{
			wRecvStartPort = m_wMixerMcuRecvStartPort;
			m_wMixerMcuRecvStartPort += APU2_MIXER_MCU_PORTSPAN;
		}

		break;
	case EQP_TYPE_RECORDER:
		wRecvStartPort = m_wRecMcuRecvStartPort;
		m_wRecMcuRecvStartPort += REC_MCU_PORTSPAN;

		break;

	case EQP_TYPE_BAS:
		wRecvStartPort = m_wBasMcuRecvStartPort;

		if(TYPE_MPU2BAS_BASIC == bySubEqpType )
		{
			m_wBasMcuRecvStartPort += BAS_BASIC_PORTSPAN;
		}
		else if(TYPE_MPU2BAS_ENHANCED == bySubEqpType)
		{
			m_wBasMcuRecvStartPort += BAS_ENHANCED_PORTSPAN;
		}
		else if (TYPE_APU2BAS == bySubEqpType )
		{
			m_wBasMcuRecvStartPort+=BAS_APU2_PORTSPAN;
		}
		else
		{
			m_wBasMcuRecvStartPort += BAS_MCU_PORTSPAN;
		}
		
		break;

	case EQP_TYPE_VMP:
		wRecvStartPort = m_wVmpMcuRecvStartPort;
		if(TYPE_MPU2VMP_BASIC == bySubEqpType || TYPE_MPU2VMP_ENHANCED == bySubEqpType)
		{
			m_wVmpMcuRecvStartPort += VMP_MPU2_PORTSPAN;
		}
		else
		{
			m_wVmpMcuRecvStartPort += VMP_MCU_PORTSPAN;
		}

		break;

	default:
		Agtlog(LOG_WARN, "[GetPeriEqpMcuRecvStartPort] unexpected EqpType!\n" );

		break;
	}

	return wRecvStartPort;
}

/*====================================================================
    ������        ClearAllPeriEqpMcuRecvPort
    ����		  ��ʼ������MCU����ʼ�˿�ֵ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	11/12/12    4.7         �´�ΰ           ����
====================================================================*/
void CAgentInterface::ClearAllPeriEqpMcuRecvPort( void )
{
	//��ʼ������MCU����ʼ�˿� [12/13/2011 chendaiwei]
	m_wMixerMcuRecvStartPort = MIXER_MCU_STARTPORT;
	m_wRecMcuRecvStartPort = REC_MCU_STARTPORT;
	m_wBasMcuRecvStartPort = BAS_MCU_STARTPORT;
	m_wVmpMcuRecvStartPort = VMP_MCU_STARTPORT;
}


/*=============================================================================
  �� �� ���� SetMpcUpateStatus
  ��    �ܣ� ����mpc��������־λ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� s32:0��ʾ�Ѿ�������1��ʾδ����
  �� �� ֵ��  
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	12/02/08    4.7         zhouyiliang           ����
====================================================================*/
void  CAgentInterface::SetMpcUpateStatus(s32 dwStat /*= 1*/ )
{
	 g_cCfgParse.SetMpcUpateStatus(dwStat);
}

/*====================================================================
    ������        GetMixerSubTypeByEqpId
    ����		  ͨ������ID��ȡ��������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	12/02/13    4.7         �´�ΰ           ����
====================================================================*/
BOOL32 CAgentInterface::GetMixerSubTypeByEqpId(u8 byEqpId, u8 & byMixertype)
{
	if (byEqpId < MIXERID_MIN || byEqpId > MIXERID_MAX)
	{
		return FALSE;
	}
	byMixertype = UNKONW_MIXER;
	BOOL32 bResult = TRUE;

#ifdef _8KE_
	byMixertype = MIXER_8KG;
#elif defined(_8KH_)
	byMixertype = MIXER_8KH;
#elif defined(_8KI_)
	byMixertype = MIXER_8KI;
#elif defined(_MINIMCU_)
	byMixertype = APU_MIXER;
#else
	TEqpMixerInfo atMixInfo;
	if(SUCCESS_AGENT != g_cCfgParse.GetEqpMixerCfgById(byEqpId,&atMixInfo))
	{
		bResult = FALSE;
		Agtlog(LOG_ERROR, "[GetMixerSubTypeByEqpId]Failed to get Mixer Subtype!\n");
	}
	else
	{
		bResult = GetMixerSubTypeByRunBrdId(atMixInfo.GetRunBrdId(),byMixertype);
	}
#endif

	return bResult;
}

/*====================================================================
    ������        GetHduSubTypeByEqpId
    ����		  ͨ������ID��ȡHDU��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	12/02/13    4.7         �´�ΰ           ����
====================================================================*/
BOOL32 CAgentInterface::GetHduSubTypeByEqpId(u8 byEqpId, u8 & byHduSubtype)
{
	if (byEqpId < HDUID_MIN || byEqpId > HDUID_MAX)
	{
		return FALSE;
	}
	byHduSubtype = 0;
	BOOL32 bResult = TRUE;

	TEqpHduInfo atHduInfo;
	if(SUCCESS_AGENT != g_cCfgParse.GetEqpHduCfgById(byEqpId,&atHduInfo))
	{
		bResult = FALSE;
		Agtlog(LOG_ERROR, "[GetHduSubTypeByEqpId]Failed to get HDU Subtype!\n");
	}
	else
	{
		byHduSubtype = atHduInfo.GetStartMode();//START_MODE��HDU����������һһ��Ӧ����������ת��
	}

	return bResult;
}

/*====================================================================
    ������        GetMixerSubTypeByRunBrdId
    ����		  ͨ������ID��ȡ�����ϵĻ�������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	12/02/13    4.7         �´�ΰ           ����
====================================================================*/
/*lint -save -esym(715,byBrdId)*/
BOOL32 CAgentInterface::GetMixerSubTypeByRunBrdId(u8 byBrdId, u8 & byMixertype)
{
	byMixertype = UNKONW_MIXER;
	BOOL32 bResult = TRUE;

#ifdef _8KE_
	byMixertype = MIXER_8KG;
#elif defined(_8KH_)
	byMixertype = MIXER_8KH;
#elif defined(_8KI_)
	byMixertype = MIXER_8KI;
#else

	TEqpBrdCfgEntry tBrdCfg;
	g_cCfgParse.GetBrdInfoById(byBrdId,&tBrdCfg);
	
	switch(tBrdCfg.GetType())
	{
	case BRD_TYPE_APU2:
		byMixertype = APU2_MIXER;
		break;

	case BRD_TYPE_APU:
		byMixertype = APU_MIXER;
		break;

	case BRD_TYPE_EAPU:
		byMixertype = EAPU_MIXER;
		break;

	default:
		byMixertype = UNKONW_MIXER;
		bResult = FALSE;
		break;
	}
#endif

	return bResult;
}

/*====================================================================
    ������        IsNeedRebootAllMpuBas
    ����		  �Ƿ���Ҫ�������е�MPU��BAS������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��BOOL32 TRUE��ʾ��Ҫ������FALS��ʾ����Ҫ
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	12/02/13    4.7         �´�ΰ           ����
====================================================================*/
BOOL32 CAgentInterface::IsNeedRebootAllMpuBas( void )
{
	return (1 == m_byIsNeedRebootAllMPUBas);
}

/*====================================================================
    ������        SetRebootAllMpuBasFlag
    ����		  ������������MPU��BAs����־λ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����BOOL32 bNeedReboot True��ʾ��Ҫ������False��ʾ����Ҫ����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	12/02/13    4.7         �´�ΰ           ����
====================================================================*/
void CAgentInterface::SetRebootAllMpuBasFlag( BOOL32 bNeedReboot)
{
	if(bNeedReboot)
	{
		m_byIsNeedRebootAllMPUBas = 1;
	}
	else
	{
		m_byIsNeedRebootAllMPUBas = 0;
	}
}

/*====================================================================
    ������        GetBrdIdbyIpAddr
    ����		  ͨ��IP��ȡ��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u32 IP ������
    ����ֵ˵��  ��u8 ����ID
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	12/02/13    4.7         �´�ΰ           ����
====================================================================*/
u8 CAgentInterface::GetBrdIdbyIpAddr(u32 dwIpAddr)
{
	return g_cCfgParse.GetBrdIdbyIpAddr(dwIpAddr);
}

/*====================================================================
    ������        GetCurEthChoice
    ����		  ��ȡ��ǰ��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��u8 ����idx
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	12/02/13    4.7         �´�ΰ           ����
====================================================================*/
u8 CAgentInterface::GetCurEthChoice ( void )
{
	TMPCInfo tMpcInfo;
	g_cCfgParse.GetMPCInfo(&tMpcInfo);

	return tMpcInfo.GetLocalPortChoice();
}


#ifdef _8KH_
/*====================================================================
    ������        SetMcuType800L
    ����		  ��Ǳ�MCUΪ800L���ͣ������8000I���֣�  
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	12/06/18    4.7         �´�ΰ           ����
====================================================================*/
void CAgentInterface::SetMcuType800L( void )
{
	g_cCfgParse.SetMcuType800L();
}

/*====================================================================
    ������        SetMcuType8000Hm
    ����		  ��Ǳ�MCUΪ8000H-M����  
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	12/06/18    4.7         �´�ΰ           ����
====================================================================*/
void CAgentInterface::SetMcuType8000Hm( void )
{
	g_cCfgParse.SetMcuType8000Hm();
}

/*====================================================================
    ������        Is800LMcu
    ����		  �ж��Ƿ���800L�ͺŵ�MCU  
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��BOOL32 �Ƿ���TRUE ���򷵻�FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	12/06/18    4.7         �´�ΰ           ����
====================================================================*/
BOOL32 CAgentInterface::Is800LMcu( void )
{
	return g_cCfgParse.Is800LMcu();
}

/*====================================================================
    ������        Is8000HMMcu
    ����		  �ж��Ƿ���8000H-M�ͺŵ�MCU  
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��BOOL32 �Ƿ���TRUE ���򷵻�FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	12/06/18    4.7         �´�ΰ           ����
====================================================================*/
BOOL32 CAgentInterface::Is8000HmMcu( void )
{
	return g_cCfgParse.Is8000HmMcu();
}

#endif //defined 8KH

/*====================================================================
    ������        AdjustMcuRasGKPort
    ����		  ����MCU��RasPort����MpcIP��GKIP��Ȼ��߲����
				  ��������� �Լ�����GK��GKCͨ�ŵ�Ĭ�϶˿�Ϊ60001
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	12/06/18    4.7         �´�ΰ           ����
====================================================================*/
void CAgentInterface::AdjustMcuRasPort(u32 dwMpcIp, u32 dwGkIp)
{
	LogPrint(LOG_LVL_DETAIL,MID_MCU_CFG,"[AdjustMcuRasPort] Gk IP: 0x%x,Mcu IP: 0x%x\n",dwGkIp,dwMpcIp);

	s8 achFName[KDV_MAX_PATH] = {0};
	sprintf(achFName, "%s/%s", DIR_CONFIG, FILE_MTADPDEBUG_INI);
	
	FILE *hFile = fopen(achFName,"a");
	{
		if( NULL != hFile )
		{
			fclose(hFile);
		}
		else
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustMcuRasPort] open mtadpdebug.ini failed! could not set correct Ras Port.\n");
		}
	}
	
	s32 dwRasPort = 0;
	s32 dwq931Port = 0;

	//����GK IP��MCU IP��ͬ������£�δ����IP��ͻ����mcu���ý����RASport
	//��Q931 port��Ϊ1919��1920[6/19/2012 chendaiwei]
	if( dwMpcIp == dwGkIp )
	{
		dwRasPort = 1919;
		dwq931Port = 1920;
	}
	else
	{
		dwRasPort = 1719;
		dwq931Port = 1720;
	}
	
	BOOL32 bResult = ::SetRegKeyInt( achFName,"calldata", "rasport",dwRasPort);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustMcuRasPort] SetRegKeyInt [RasPort:%d] failed.\n",dwRasPort);	
	}
	
	bResult = ::SetRegKeyInt( achFName, "calldata", "q931port", dwq931Port);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustMcuRasPort] SetRegKeyInt [q931port:%d] failed.\n",dwq931Port);
	}

	memset(achFName,0,sizeof(achFName));
	sprintf(achFName, "%s/%s", DIR_CONFIG, GKCFG_INI_FILENAME);
	FILE *hGkFile = fopen(achFName,"a");
	{
		if( NULL != hGkFile )
		{
			fclose(hGkFile);
		}
		else
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustMcuRasPort] open gkconfig.ini failed! could not set Gk Port 60001.\n");
		}
	}

	bResult = ::SetRegKeyInt( achFName,"PortInfo", "PortAddTimes",10);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustMcuRasPort] SetRegKeyInt [PortAddTimes:10] failed.\n");	
	}

	bResult = ::SetRegKeyInt( achFName,"PortInfo", "Port_GK",60001);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustMcuRasPort] SetRegKeyInt [Port_GK:60001] failed.\n");	
	}

	return;
}

/*====================================================================
    ������        AdjustNewMcuRasPort
    ����		  ����MCU��RasPort����MpcIP��GKIP��sipip��Ȼ��߲����
				  ��������� �Լ�����GK��GKCͨ�ŵ�Ĭ�϶˿�Ϊ60001
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u32 dwMcuIp, u32 dwGKIp, u32 dwSipIp
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	20130929    8KI         ������           ����
====================================================================*/
void CAgentInterface::AdjustNewMcuRasPort(u32 dwMpcIp, u32 dwGKIp, u32 dwSipIp)
{
	s8 achFName[KDV_MAX_PATH] = {0};
	sprintf(achFName, "%s/%s", DIR_CONFIG, FILE_MTADPDEBUG_INI);
	
	FILE *hFile = fopen(achFName,"a");
	{
		if( NULL != hFile )
		{
			fclose(hFile);
		}
		else
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustNewMcuRasPort] open mtadpdebug.ini failed! could not set correct Ras Port.\n");
		}
	}
	
	s32 dwRasPort = 1719;
	s32 dwq931Port = 1720;

	//����GK IP��MCU IP��ͬ��Sip Ip��MCU IP��ͬ������£�Ϊ����IP��ͻ����mcu���ý����RASport
	//��Q931 port��Ϊ1919��1920
	if( dwMpcIp == dwGKIp || dwMpcIp == dwSipIp)
	{
		dwRasPort = 1919;
		dwq931Port = 1920;
	}
	
	BOOL32 bResult = ::SetRegKeyInt( achFName,"calldata", "rasport",dwRasPort);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustNewMcuRasPort] SetRegKeyInt [RasPort:%d] failed.\n",dwRasPort);	
	}
	
	bResult = ::SetRegKeyInt( achFName, "calldata", "q931port", dwq931Port);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustNewMcuRasPort] SetRegKeyInt [q931port:%d] failed.\n",dwq931Port);
	}

	s32 dwSipPort = 1720;
	//����GK IP��Sip Ip��ͬ������£�Ϊ����IP��ͻ����Sip port��Ϊ1825
	if(dwGKIp == dwSipIp)
	{
		dwSipPort = 1825;
	}

	#ifdef WIN32
	#define DIR_CONFIG_NEW               ( LPCSTR )"./conf"	
	#else
	#define DIR_CONFIG_NEW               ( LPCSTR )"/usr/conf"	
	#endif

	memset(achFName,0,sizeof(achFName));
	sprintf(achFName, "%s/%s", DIR_CONFIG_NEW, "iwCfg.ini");
	
	FILE *hOpen = NULL;
    if(NULL == (hOpen = fopen(achFName, "r")))	// no exist
	{	
		g_cCfgParse.AgentDirCreate(DIR_CONFIG_NEW);
		hOpen = fopen(achFName, "w");			// create configure file
	}

	if(NULL != hOpen)
	{
		fclose(hOpen);
		hOpen = NULL;
	}

	bResult = ::SetRegKeyInt( achFName, "InterworkingSrvInfo", "H225Port", dwSipPort);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustNewMcuRasPort] SetRegKeyInt [Sipport:%d] failed.\n",dwSipPort);
	}

	//дSipIp��ַ
	s8	achSipIpAddr[32] = {0};
	GetIpFromU32(achSipIpAddr, htonl(dwSipIp));
	bResult = ::SetRegKeyString( achFName, "InterworkingSrvInfo", "InterworkingIP", achSipIpAddr);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustNewMcuRasPort] SetRegKeyString [InterworkingIP:%s] failed.\n",achSipIpAddr);
	}

	bResult = ::SetRegKeyString( achFName, "SipRegSrvInfo", "IpAddr", achSipIpAddr);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustNewMcuRasPort] SetRegKeyString [IpAddr:%s] failed.\n",achSipIpAddr);
	}

	memset(achFName,0,sizeof(achFName));
	sprintf(achFName, "%s/%s", DIR_CONFIG_NEW, "regprefix.ini");

    if(NULL == (hOpen = fopen(achFName, "r")))	// no exist
	{	
		g_cCfgParse.AgentDirCreate(DIR_CONFIG_NEW);
		hOpen = fopen(achFName, "w");			// create configure file
	}
	
	if(NULL != hOpen)
	{
		fclose(hOpen);
		hOpen = NULL;
	}

	bResult = ::SetRegKeyString( achFName, "RegserverIP", "IP", achSipIpAddr);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustNewMcuRasPort] SetRegKeyString [IP:%s] failed.\n",achSipIpAddr);
	}

	memset(achFName,0,sizeof(achFName));
	sprintf(achFName, "%s/%s", DIR_CONFIG, GKCFG_INI_FILENAME);
	FILE *hGkFile = fopen(achFName,"a");
	{
		if( NULL != hGkFile )
		{
			fclose(hGkFile);
		}
		else
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustNewMcuRasPort] open gkconfig.ini failed! could not set Gk Port 60001.\n");
		}
	}

	bResult = ::SetRegKeyInt( achFName,"PortInfo", "PortAddTimes",10);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustNewMcuRasPort] SetRegKeyInt [PortAddTimes:10] failed.\n");	
	}

	bResult = ::SetRegKeyInt( achFName,"PortInfo", "Port_GK",60001);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustNewMcuRasPort] SetRegKeyInt [Port_GK:60001] failed.\n");	
	}

	return;
}
/*====================================================================
    ������        RefreshBrdOsType
    ����		  ����ĳ������忨��OSTYPEֵ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��u8 byBrdId, u8 byOsType
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	12/08/23    4.7         ��־��          ����
====================================================================*/
BOOL32 CAgentInterface::RefreshBrdOsType(u8 byBrdId, u8 byOsType)
{
	return g_cCfgParse.RefreshBrdOsType(byBrdId, byOsType);
}

/*====================================================================
    ������        GetBrdCfgById
    ����		  ��ȡ������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	12/10/23    4.7         chendaiwei    ����
====================================================================*/
u16 CAgentInterface::GetBrdCfgById(u8 byBrdId, TBoardInfo* ptBoardInfo)
{
	return g_cCfgParse.GetBrdCfgById(byBrdId,ptBoardInfo);
}
// END OF FILE
