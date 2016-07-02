/*****************************************************************************
   ģ����      : ���渴����(Video MultiPlexer)
   �ļ���      : VMPCfg.h
   ����ʱ��    : 2003�� 12�� 4��
   ʵ�ֹ���    : ���渴������������
   ����        : zhangsh
   �汾        : 
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
******************************************************************************/
#ifndef _VIDEO_MULTIPLEXER_CONFIG_H_
#define _VIDEO_MULTIPLEXER_CONFIG_H_

#include "kdvtype.h"
#include "osp.h"
#include "kdvsys.h"
#include "mcuconst.h"
#include "mcustruct.h"
#include "mmpcommon.h"
#if !defined(_8KH_) && !defined(_8KE_)
#include "multpic_gpu.h"
#else
#include "multpic.h"
#endif



#define	MIN_BITRATE_OUTPUT      (u16)128

#define MAXNUM_VMPDEFINE_PIC   MAXNUM_SDVMP_MEMBER
struct TVmpBitrateDebugVal
{
protected:
    BOOL32          m_bEnableBitrateCheat;         // �Ƿ����������������
    u16             m_wOverDealRate;               // ƽ��������ͻ�İٷֱ�
public:
    TVmpBitrateDebugVal():m_bEnableBitrateCheat(0),
                          m_wOverDealRate(0){}
public:
    void   SetEnableBitrateCheat(BOOL32 bEnable)
    {
        m_bEnableBitrateCheat = bEnable;
    }
    BOOL32 IsEnableBitrateCheat() const
    {
        return m_bEnableBitrateCheat;
    }
    void   SetOverDealRate(u16 wPctRate)
    {
        m_wOverDealRate = wPctRate;
    }
    u16    GetOverDealRate(void) const
    {
        return m_wOverDealRate;
    }
};

// ����ϳɳ�Ա���� [7/3/2013 liaokang]
struct TVmpMbAlias
{
public:  
    BOOL32  SetMbAlias( u8 byAliasLen, const s8 *pAlias ) 
    { 
        if( NULL == pAlias )
        {
            return FALSE;
        }
        memset(m_achMbAlias, 0, sizeof(m_achMbAlias));
        memcpy(m_achMbAlias, pAlias, min(byAliasLen, MAXLEN_ALIAS-1));
        m_achMbAlias[MAXLEN_ALIAS-1] = '\0';
        return TRUE; 
    }
    const s8* GetMbAlias(void) { return m_achMbAlias; }
    
protected:
    s8   m_achMbAlias[MAXLEN_ALIAS];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TVmpMemPicRoute
{
private:
	s8 m_szVmpMemAlias[/*VALIDLEN_ALIAS+1*/MAXLEN_ALIAS]; // ���� [7/3/2013 liaokang]
	s8 m_szVmpMemPicRoute[255];
public:
	TVmpMemPicRoute()
	{
		memset(m_szVmpMemAlias, 0, sizeof(m_szVmpMemAlias));
		memset(m_szVmpMemPicRoute, 0, sizeof(m_szVmpMemPicRoute));
	}	
	
	void SetRoute(const s8* pszRoute)
    {
        memset( m_szVmpMemPicRoute, 0, sizeof(m_szVmpMemPicRoute));
        strncpy( m_szVmpMemPicRoute, pszRoute,  sizeof(m_szVmpMemPicRoute));
		m_szVmpMemPicRoute[sizeof(m_szVmpMemPicRoute)-1] = '\0';
    }
	
	const s8* GetRoute() const
    {
        return m_szVmpMemPicRoute;
    }
	
	void SetVmpMemAlias(const s8* pszMemAlias)
    {
        memset( m_szVmpMemAlias, 0, sizeof(m_szVmpMemAlias));
        strncpy( m_szVmpMemAlias, pszMemAlias,  sizeof(m_szVmpMemAlias));
		m_szVmpMemAlias[sizeof(m_szVmpMemAlias)-1] = '\0';
    }
	
	const s8* GetVmpMemAlias() const
    {
        return m_szVmpMemAlias;
    }
};

struct TDebugVal
{
protected:
	u32				m_dwVidResizeMode;	    //ģʽ0:�Ӻڱߣ�1:�ñߣ�2:�ǵȱ����죬Ĭ��0
    u32             m_dwVidEachResizeMode;   // ��Ժϳɷ���и���Сͼ����У��� 0�ڱ�/ 1�ñ�/ 2�ǵȱ�������ȫ����
	TVmpMemPicRoute	m_atVmpMemPic[MAXNUM_VMPDEFINE_PIC];
public:
   

	void	SetVidResizeMode(u32 dwMode)
	{
		m_dwVidResizeMode = htonl(dwMode);
	}
	
	u32		GetVidResizeMode(void)
	{
		return ntohl(m_dwVidResizeMode);
	}

	void	SetVidEachResizeMode(u32 dwMode)
	{
		m_dwVidEachResizeMode = htonl(dwMode);
	}
	
	u32		GetVidEachResizeMode(void)
	{
		return ntohl(m_dwVidEachResizeMode);
	}

	/*=============================================================================
	�� �� ���� TableMemoryFree
	��    �ܣ� �ͷ�ָ������ڴ�
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
	��    ����  void **ppMem
	u32 dwEntryNum
	�� �� ֵ�� BOOL32 
	=============================================================================*/
	BOOL32 TableMemoryFree( void **ppMem, u32 dwEntryNum )
	{
		if( NULL == ppMem)
		{
			return FALSE;
		}
		for( u32 dwLoop = 0; dwLoop < dwEntryNum; dwLoop++ )
		{
			if( NULL != ppMem[dwLoop] )
			{
				delete [] (s8*)ppMem[dwLoop];
				ppMem[dwLoop] = NULL;
			}
		}
		delete [] (s8*)ppMem;
		ppMem = NULL;
		return TRUE;
	}

	void ReadDebugValues()
	{
		s8 CFG_FILE[MAX_PATH] = {0};
		s8  SECTION_VMPDEBUG[]    = "EQPDEBUG";
		s8 SECTION_VMPDEFINE_PIC[] = "VMPDEFINEPIC";
		sprintf(CFG_FILE, "%s/mcueqp.ini", DIR_CONFIG);
		s32 nValue;

		// �Ƿ�Ӻڱ߻�ñ�,Ĭ����0
		::GetRegKeyInt( CFG_FILE, SECTION_VMPDEBUG, "VidSDMode", 0, &nValue );
		if( nValue != 0 && nValue != 1 && nValue != 2)
		{
			nValue = 0;
		}
		SetVidResizeMode((u32)nValue);
		
		// ��Ժϳɷ���и���Сͼ����У��� 0�ڱ�/ 1�ñ�/ 2�ǵȱ�������ȫ����
		::GetRegKeyInt( CFG_FILE, SECTION_VMPDEBUG, "VidEachResizeMode", 2, &nValue );
		if( nValue != 0 && nValue != 1 && nValue != 2)
		{
			nValue = 2;
		}
		SetVidEachResizeMode((u32)nValue);
		s32 nMemEntryNum = 0;
		::GetRegKeyInt( CFG_FILE, SECTION_VMPDEFINE_PIC, "EntryNum", 0, &nMemEntryNum );
		if (0 == nMemEntryNum)
		{
			return;
		}
		
		// alloc memory
		s8** ppszTable = NULL;        
		ppszTable = new s8*[nMemEntryNum];
		if( NULL == ppszTable )
		{
			OspPrintf(1, 0,  "[AgentGetBrdCfgTable] Fail to malloc memory \n");
			return;
		}
		
		u32 dwEntryNum = nMemEntryNum;
		u32 dwLoop = 0;
		for( dwLoop = 0; dwLoop < dwEntryNum; dwLoop++ )
		{
			ppszTable[dwLoop] = new s8[MAX_VALUE_LEN+1];
			if(NULL == ppszTable[dwLoop])
			{
				//�ͷ��ڴ�
				TableMemoryFree( (void**)ppszTable, nMemEntryNum );
				return;
			}
		}
		u32 dwReadEntryNum = dwEntryNum;
		GetRegKeyStringTable( CFG_FILE,    
			SECTION_VMPDEFINE_PIC,      
			"fail",     
			ppszTable, 
			&dwReadEntryNum, 
			MAX_VALUE_LEN + 1);
		
		if (dwReadEntryNum != dwEntryNum)
		{
			//�ͷ��ڴ�
			TableMemoryFree( (void**)ppszTable, nMemEntryNum );
			return;
		}
		
		s8    achSeps[] = "\t";        // �ָ���
		s8    *pchToken = NULL;
		
		for( dwLoop = 0; dwLoop < dwEntryNum; dwLoop++)
		{
			if (dwLoop + 1 == MAXNUM_VMPDEFINE_PIC)
			{
				break;
			}
			//����
			pchToken = strtok( ppszTable[dwLoop], achSeps );
			if (NULL == pchToken)
			{
				continue;
			}
			
			//Vmp��Ա����
			pchToken = strtok( NULL, achSeps );
			if (NULL == pchToken)
			{
				continue;
			}
			SetVmpMemAlias(dwLoop, pchToken);
			
			//Vmp��Ա��Ҫ��ʾ��ͼƬ
			pchToken = strtok( NULL, achSeps );
			if (NULL == pchToken)
			{
				continue;
			}
			SetRoute(dwLoop, pchToken);
		}
		
		TableMemoryFree( (void**)ppszTable, nMemEntryNum );
		
		return;
	}

	void SetRoute(u8 byIndex, const s8* pszRoute)
    {
		if (byIndex >= MAXNUM_VMPDEFINE_PIC || NULL == pszRoute)
		{
			return;
		}
        m_atVmpMemPic[byIndex].SetRoute(pszRoute);
    }
	
	const s8* GetVmpMemRoute(u8 byIndex) const
    {
        return m_atVmpMemPic[byIndex].GetRoute();
    }
	
	void SetVmpMemAlias(u8 byIndex, const s8* pszMemAlias)
    {
		if (byIndex >= MAXNUM_VMPDEFINE_PIC || NULL == pszMemAlias)
		{
			return;
		}
		m_atVmpMemPic[byIndex].SetVmpMemAlias(pszMemAlias);
    }
	
	const s8* GetVmpMemAlias(u8 byIndex) const
    {
        return m_atVmpMemPic[byIndex].GetVmpMemAlias();
    }

	void Print()
	{
		OspPrintf(TRUE,FALSE,"GetVidResizeMode: %d\n",GetVidResizeMode());
		OspPrintf(TRUE,FALSE,"GetVidEachResizeMode: %d\n",GetVidEachResizeMode());
		OspPrintf(TRUE,  FALSE, "VmpPic as follows:\n");
		for (u8 byIndex = 0; byIndex < MAXNUM_VMPDEFINE_PIC; byIndex++)
		{
			OspPrintf(TRUE, FALSE, "Index.%d -> Alias.%s -> Route.%s\n", 
				byIndex,
				m_atVmpMemPic[byIndex].GetVmpMemAlias(),
				m_atVmpMemPic[byIndex].GetRoute());
		}
	}

};

class CCPParam
{      
public:
    //���浱ǰ����״̬
    C8KEVMPParam m_tStatus;

    //TVmpBitrateDebugVal   m_tDebugVal;                // ����������ֵ
public:
    CCPParam();
	~CCPParam();
    void Clear();
	
	void GetDefaultParam(u8 byEncType,TVidEncParam& TEncParam);
};

#endif //_BITRATE_ADAPTER_SERVER_CONFIG_H_

