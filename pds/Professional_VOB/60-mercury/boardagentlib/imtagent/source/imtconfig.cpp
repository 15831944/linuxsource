/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : imtconfig.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ��������ṩ���ϲ�Ӧ�õĽӿ�ʵ��
   ����        : jianghy
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/09/12  1.0         jianghy       ����
   2004/12/05  3.5         �� ��        �½ӿڵ��޸�
******************************************************************************/
#include "osp.h"
#include "imtconfig.h"
#include "mcuconst.h"
#include "imtagent.h"

/*
*���캯��
*/
CImtConfig::CImtConfig()
{
	byBrdId = 0;
	bIsRunMixer = FALSE;    //�Ƿ�����MIXER
	bIsRunTVWall = FALSE;	//�Ƿ�����TVWall
	bIsRunBas = FALSE;		//�Ƿ�����Bas
	bIsRunVMP = FALSE;		//�Ƿ�����VMP
	bIsRunPrs = FALSE;
	byMcuId = 0;
	dwMpcIpAddr = 0;
	wMpcPort = 0;

    m_wDiscHeartBeatTime = DEF_OSPDISC_HBTIME;
    m_byDiscHeartBeatNum = DEF_OSPDISC_HBNUM;   

	memset(&m_tMixerCfg, 0, sizeof(m_tMixerCfg));
	memset(&m_tTVWallCfg, 0, sizeof(m_tTVWallCfg));
	memset(&m_tBasCfg, 0, sizeof(m_tBasCfg));
	memset(&m_tVMPCfg, 0, sizeof(m_tVMPCfg));
	memset(&m_tPrsCfg, 0, sizeof(m_tPrsCfg));
#ifndef WIN32
#ifdef IMT
	/*Ҫ���ݱ�������ȷ����IMT����APU��������*/
	TBrdPosition tBoardPosition;
	BrdQueryPosition( &tBoardPosition );

	if( tBoardPosition.byBrdID == BRD_TYPE_IMT/*DSL8000_BRD_IMT*/ )
	{
		byMAPCount=3;
		memset(m_tMAPCfg, 0, sizeof(m_tMAPCfg));
		for(u16 wLoop=0; wLoop<byMAPCount; wLoop++)
		{
			m_tMAPCfg[wLoop].byMAPId = (u8)wLoop;
			m_tMAPCfg[wLoop].dwMAPCoreSpeed = 392;
			m_tMAPCfg[wLoop].dwMAPMemSpeed = 131;
			m_tMAPCfg[wLoop].dwMAPMemSize = 67108864;
		}
	}
	else
	{
		byMAPCount=1;
		memset(m_tMAPCfg, 0, sizeof(m_tMAPCfg));
		for(u16 wLoop=0; wLoop<byMAPCount; wLoop++)
		{
			m_tMAPCfg[wLoop].byMAPId = (u8)wLoop;
			m_tMAPCfg[wLoop].dwMAPCoreSpeed = 392;
			m_tMAPCfg[wLoop].dwMAPMemSpeed = 131;
			m_tMAPCfg[wLoop].dwMAPMemSize = 67108864;
		}
	}
#else
	byMAPCount=3;
	memset(m_tMAPCfg, 0, sizeof(m_tMAPCfg));
	for(u16 wLoop=0; wLoop<byMAPCount; wLoop++)
	{
		m_tMAPCfg[wLoop].byMAPId = (u8)wLoop;
		m_tMAPCfg[wLoop].dwMAPCoreSpeed = 392;
		m_tMAPCfg[wLoop].dwMAPMemSpeed = 131;
		m_tMAPCfg[wLoop].dwMAPMemSize = 67108864;
	}
#endif

#else
	byMAPCount=0;
	memset(m_tMAPCfg, 0, sizeof(m_tMAPCfg));
#endif
}

/*
*��������
*/
CImtConfig::~CImtConfig()
{
}

/*====================================================================
    ������      ��GetBoardId
    ����        �����ȡ����Ĳ�λ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������ 
    ����ֵ˵��  ��IDֵ��0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
u8	CImtConfig::GetBoardId()
{
	return byBrdId;
}

/*====================================================================
    ������      ��GetConnectMcuId
    ����        ����ȡҪ���ӵ�MCU��ID
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������ 
    ����ֵ˵��  ��MCU��ID��0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
u8 CImtConfig::GetConnectMcuId()
{
	return byMcuId;
}


/*====================================================================
    ������      ��GetConnectMcuIpAddr
    ����        ����ȡҪ���ӵ�MCU��IP��ַ(������)
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������ 
    ����ֵ˵��  ��MCU��IP��ַ(������)��0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
u32 CImtConfig::GetConnectMcuIpAddr()
{
	return htonl(dwMpcIpAddr);
}

/*====================================================================
    ������      ��GetConnectMcuPort
    ����        ����ȡ���ӵ�MCU�Ķ˿ں�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������ 
    ����ֵ˵��  ��MCU�Ķ˿ںţ�0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
u16 CImtConfig::GetConnectMcuPort()
{
	return wMpcPort;
}

BOOL32 CImtConfig::SetMpcIpAddrA(u32 dwIpAddr)
{
    dwMpcIpAddr = dwIpAddr;
    return TRUE;
}

BOOL32 CImtConfig::SetMpcPortA(u16 wPort)
{
    wMpcPort = wPort;
    return TRUE;
}

u32 CImtConfig::GetMpcIpAddrA(void)
{
    return dwMpcIpAddr;
}

u16 CImtConfig::GetMpcPortA(void)
{
    return wMpcPort;
}

/*=============================================================================
  �� �� ���� SetMpcIpAddrB
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwIpAddr
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CImtConfig::SetMpcIpAddrB(u32 dwIpAddr)
{
    dwMpcIpAddrB = dwIpAddr;
    return TRUE;
}

BOOL32 CImtConfig::SetMpcPortB(u16 wPort)
{
    wMpcPortB = wPort;
    return TRUE;
}

u32 CImtConfig::GetMpcIpAddrB(void)
{
    return dwMpcIpAddrB;
}

u16 CImtConfig::GetMpcPortB(void)
{
    return wMpcPortB;
}

/*====================================================================
    ������      ��IsRunMixer
    ����        ���Ƿ����л�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������ 
    ����ֵ˵��  �����з���TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
BOOL CImtConfig::IsRunMixer()
{
	return bIsRunMixer;
}

/*=============================================================================
  �� �� ���� SetMpwConfig
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TMcueqpMPWEntry* ptMpwCfg
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CImtConfig::SetMpwConfig(TEqpMPWEntry* ptMpwCfg)
{
    if(NULL == ptMpwCfg)
    {
        return FALSE;
    }

    m_tMpwCfg = *ptMpwCfg;
    bIsRunMpw = TRUE;
    byMcuId = ptMpwCfg->GetMcuId();

    return TRUE;
}

/*=============================================================================
  �� �� ���� GetMpwId
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u8 
=============================================================================*/
u8 CImtConfig::GetMpwId()
{
    if(!bIsRunMpw)
    {
        return FALSE;
    }
    return m_tMpwCfg.GetEqpId();
}

/*=============================================================================
  �� �� ���� GetMpwAlias
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� s8* lpstrBuf
             u16 wLen
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CImtConfig::GetMpwAlias(s8* lpstrBuf, u16 wLen)
{
    if(!bIsRunMpw || NULL == lpstrBuf)
    {
        return FALSE;
    }
    
    strncpy(lpstrBuf, m_tMpwCfg.GetAlias(), wLen);
    lpstrBuf[wLen-1] = '\0';
    return TRUE;
}

/*=============================================================================
  �� �� ���� GetMpwIpAddr
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u32 
=============================================================================*/
u32 CImtConfig::GetMpwIpAddr()
{
    if(!bIsRunMpw)
    {
        return FALSE;
    }
    return htonl(m_tMpwCfg.GetIpAddr());
}

/*=============================================================================
  �� �� ���� GetMpwMAPId
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u8* pbyIdBuf
             u8 byBufLen
  �� �� ֵ�� u8  
=============================================================================*/
u8  CImtConfig::GetMpwMAPId( u8* pbyIdBuf, u8 byBufLen )
{
    u8 byLoop = 0;

	if( !bIsRunMpw )
	{
		return FALSE;
    }

	for( byLoop = 0; byLoop < m_tMpwCfg.GetUsedMapNum() && byLoop < byBufLen; byLoop++ )
	{
		pbyIdBuf[byLoop] = m_tMpwCfg.GetUsedMapId(byLoop);
	}

	return byLoop;
}

/*=============================================================================
  �� �� ���� GetMpwRecvStartPort
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u16 
=============================================================================*/
u16 CImtConfig::GetMpwRecvStartPort()
{
    if(!bIsRunMpw)
    {
        return FALSE;
    }
    return m_tMpwCfg.GetEqpRecvPort();
}

/*=============================================================================
  �� �� ���� GetMpwType
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� u8 
=============================================================================*/
u8 CImtConfig::GetMpwType()
{
    if( !bIsRunMpw )
	{
		return FALSE;
    }

	return m_tMpwCfg.GetType();
}

/*=============================================================================
  �� �� ���� IsRunMpw
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CImtConfig::IsRunMpw()
{
    return bIsRunMpw;
}
/*====================================================================
    ������      ��IsRunBas
    ����        ���Ƿ�����BAS
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������ 
    ����ֵ˵��  �����з���TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
BOOL CImtConfig::IsRunBas()
{
	return bIsRunBas;
}

/*====================================================================
    ������      ��IsRunVMP
    ����        ���Ƿ�����VMP
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������ 
    ����ֵ˵��  �����з���TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
BOOL CImtConfig::IsRunVMP()
{
	return bIsRunVMP;
}


/*====================================================================
    ������      ��IsRunPrs
    ����        ���Ƿ�����prs
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������ 
    ����ֵ˵��  �����з���TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
BOOL CImtConfig::IsRunPrs()
{
	return bIsRunPrs;
}


/*====================================================================
    ������      ��IsRunTVWALL
    ����        ���Ƿ�����TVWALL
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������ 
    ����ֵ˵��  �����з���TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
BOOL CImtConfig::IsRunTVWall()
{
	return bIsRunTVWall;
}

/*====================================================================
    ������      ��GetMixerMAPId
    ����        ����ȡMixer��MAP����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8* pbyIdBuf: ���MAP��ŵ�����
                  u8 byBufLen: ���鳤��
    ����ֵ˵��  ��MAP����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
u8 CImtConfig::GetMixerMAPId( u8* pbyIdBuf, u8 byBufLen )
{
	u8 byLoop = 0;

	if( !bIsRunMixer )
    {
		return FALSE;
    }

	for( byLoop = 0; byLoop < m_tMixerCfg.GetUsedMapNum() && byLoop < byBufLen; byLoop++ )
	{
		pbyIdBuf[byLoop] = m_tMixerCfg.GetUsedMapId(byLoop);
	}

	return byLoop;
}


/*====================================================================
    ������      ��GetTVWallMAPId
    ����        ����ȡTVWall��MAP����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8* pbyIdBuf: ���MAP��ŵ����� 
                  u8 byBufLen: ���鳤�� 
    ����ֵ˵��  ��MAP����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
u8 CImtConfig::GetTVWallMAPId( u8* pbyIdBuf, u8 byBufLen )
{
	u8 byLoop = 0;

	if( !bIsRunTVWall )
	{
		return FALSE;
    }

	for( byLoop = 0; byLoop < m_tTVWallCfg.GetUsedMapNum() && byLoop < byBufLen; byLoop++ )
	{
		pbyIdBuf[byLoop] = m_tTVWallCfg.GetUsedMapId(byLoop);
	}

	return byLoop;
}

/*====================================================================
    ������      ��GetBasMAPId
    ����        ����ȡBas��MAP����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8* pbyIdBuf: ���MAP��ŵ����� 
                  u8 byBufLen: ���鳤�� 
    ����ֵ˵��  ��MAP����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
u8 CImtConfig::GetBasMAPId( u8* pbyIdBuf, u8 byBufLen )
{
	u8 byLoop = 0;

	if( !bIsRunBas )
	{
		return FALSE;
    }

	for( byLoop = 0; byLoop < m_tBasCfg.GetUsedMapNum() && byLoop < byBufLen; byLoop++ )
	{
		pbyIdBuf[byLoop] = m_tBasCfg.GetUsedMapId(byLoop);
	}

	return byLoop;
}

/*====================================================================
    ������      ��GetVMPMAPId
    ����        ����ȡVMP��MAP����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8* pbyIdBuf: ���MAP��ŵ����� 
                  u8 byBufLen: ���鳤��
    ����ֵ˵��  ��MAP����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
u8 CImtConfig::GetVMPMAPId( u8* pbyIdBuf, u8 byBufLen )
{
	u8 byLoop = 0;

	if( !bIsRunVMP )
	{
		return FALSE;
    }

	for( byLoop = 0; byLoop < m_tVMPCfg.GetUsedMapNum() && byLoop < byBufLen; byLoop++ )
	{
		pbyIdBuf[byLoop] = m_tVMPCfg.GetUsedMapId(byLoop);
	}

	return byLoop;
}


/*====================================================================
    ������      ��GetMAPInfo
    ����        ����ȡMAP��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byId: MAP��
                  u32 *pdwCoreSpeed: MAP��Ƶ
                  u32 *pdwMemSpeed: MAP���ڴ���Ƶ
                  u32 *pdwPort: MAP�Ķ˿ں�
    ����ֵ˵��  ���ɹ�����TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
BOOL CImtConfig::GetMAPInfo(u8 byId, u32 *pdwCoreSpeed, u32 *pdwMemSpeed, u32 *pdwMemSize, u32 *pdwPort)
{
	if( pdwCoreSpeed == NULL || pdwMemSpeed == NULL || pdwMemSize == NULL || pdwPort == NULL )
	{
		return FALSE;
	}

	if( byId >= byMAPCount )
	{
		return FALSE;
	}

	*pdwCoreSpeed = m_tMAPCfg[byId].dwMAPCoreSpeed;
	*pdwMemSpeed = m_tMAPCfg[byId].dwMAPMemSpeed;
	*pdwMemSize = m_tMAPCfg[byId].dwMAPMemSize;
	*pdwPort = m_tMAPCfg[byId].byMAPPort;

	return TRUE;
}

/*====================================================================
    ������      ��GetMixerId
    ����        ����ȡMixer��ID
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��Mixer��ID��0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
u8 CImtConfig::GetMixerId()
{
	if( !bIsRunMixer )
	{
		return FALSE;
    }

	return m_tMixerCfg.GetEqpId();
}

/*====================================================================
    ������      ��GetMixerType
    ����        ����ȡMixer������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��Mixer�����ͣ�0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
u8 CImtConfig::GetMixerType()
{
	if( !bIsRunMixer )
	{
		return FALSE;
    }

	return m_tMixerCfg.GetType();
}

/*====================================================================
    ������      ��GetMixerAlias
    ����        ����ȡMixer�ļ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����LPSTR lpstrBuf, Alias�ַ���ָ�룬�ڴ����û�����
                  u16 wLen, Alias�ַ�����������С��
                  ��С��ʵ�ʳ��ȣ������ַ������Խض�
    ����ֵ˵��  ���ɹ�����TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
BOOL CImtConfig::GetMixerAlias(s8* lpstrBuf, u16 wLen)
{
	if( !bIsRunMixer )
	{
		return FALSE;
    }

	if( lpstrBuf == NULL )
	{
		return FALSE;
    }

	strncpy( lpstrBuf, m_tMixerCfg.GetAlias(), wLen );
	lpstrBuf[wLen-1] = '\0';
	return TRUE;
}

/*====================================================================
    ������      ��GetMixerIpAddr
    ����        ����ȡMixer��IP(������)
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��Mixer��IP(������)��0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u32 CImtConfig::GetMixerIpAddr()
{
	if( !bIsRunMixer )
	{
		return FALSE;
    }

	return htonl(m_tMixerCfg.GetIpAddr());
}

/*====================================================================
    ������      ��GetMixerRecvStartPort
    ����        ����ȡMixer����ʼ���ն˿ں�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����ʼ���ն˿ںţ�0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u16 CImtConfig::GetMixerRecvStartPort()
{
	if( !bIsRunMixer )
	{
		return FALSE;
    }

	return m_tMixerCfg.GetEqpRecvPort();
}


/*====================================================================
    ������      ��GetMixerMaxMixGroupNum
    ����        ����ȡ������֧������������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��������������0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u8 CImtConfig::GetMixerMaxMixGroupNum()
{
	if( !bIsRunMixer )
	{
		return FALSE;
    }

	return m_tMixerCfg.GetMaxMixGrpNum();
}

/*====================================================================
    ������      ��GetMixerMaxChannelInGrp
    ����        ����ȡÿ�����������ͨ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  �����ͨ������0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u8 CImtConfig::GetMixerMaxChannelInGrp()
{
	if( !bIsRunMixer )
	{
		return FALSE;
    }

	return m_tMixerCfg.GetMaxChnInGrp();
}


/*====================================================================
    ������      ��GetTWId
    ����        ����ȡTVWall��ID
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��TVWall��ID��0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u8 CImtConfig::GetTWId()
{
	if( !bIsRunTVWall )
	{
		return FALSE;
    }

	return m_tTVWallCfg.GetEqpId();
}

/*====================================================================
    ������      ��GetTWType
    ����        ����ȡTVWall������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��TVWall�����ͣ�0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u8 CImtConfig::GetTWType()
{
	if( !bIsRunTVWall )
	{
		return FALSE;
    }

	return m_tTVWallCfg.GetType();
}


/*====================================================================
    ������      ��GetTWAlias
    ����        ����ȡTVWall�ļ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����LPSTR lpstrBuf, Alias�ַ���ָ�룬�ڴ����û�����
                  u16 wLen, Alias�ַ�����������С��
                  ��С��ʵ�ʳ��ȣ������ַ������Խض�
    ����ֵ˵��  ���ɹ�����TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
BOOL CImtConfig::GetTWAlias(s8* lpstrBuf, u16 wLen)
{
	if( !bIsRunTVWall )
	{
		return FALSE;
    }

	if( lpstrBuf == NULL )
	{
		return FALSE;
    }

	strncpy( lpstrBuf, m_tTVWallCfg.GetAlias(), wLen );
	lpstrBuf[wLen-1] = '\0';
	return TRUE;
}

/*====================================================================
    ������      ��GetTWIpAddr
    ����        ����ȡTVWall��IP(������)
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��TVWall��IP(������)��0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u32 CImtConfig::GetTWIpAddr()
{
	if( !bIsRunTVWall )
	{
		return FALSE;
    }

	return htonl(m_tTVWallCfg.GetIpAddr());
}

/*====================================================================
    ������      ��GetTWRecvStartPort
    ����        ����ȡTVWall��Ƶ��ʼ���ն˿ں�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����ʼ���ն˿ںţ�0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u16 CImtConfig::GetTWRecvStartPort()
{
	if( !bIsRunTVWall )
	{
		return FALSE;
    }

	return m_tTVWallCfg.GetEqpRecvPort();
}

/*====================================================================
    ������      ��GetTWDivStyle
    ����        ����ȡ����ǽ�ķָʽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ���ָʽ��0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u8 CImtConfig::GetTWDivStyle()
{
	if( !bIsRunTVWall )
	{
		return FALSE;
    }

	// return m_tTVWallCfg.mcueqpTVWallEntDivStyle;
    return 1;
}

/*====================================================================
    ������      ��GetTWDivNum
    ����        ����ȡ����ǽ�ķָ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ���ָ������0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u8 CImtConfig::GetTWDivNum()
{
	if( !bIsRunTVWall )
	{
		return FALSE;
    }

	// return m_tTVWallCfg.mcueqpTVWallEntDivNum;
    return 1;
}

/*====================================================================
    ������      ��GetBasId
    ����        ����ȡBas��ID
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��Bas��ID��0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u8 CImtConfig::GetBasId()
{
	if( !bIsRunBas )
	{
		return FALSE;
    }

	return m_tBasCfg.GetEqpId();
}


/*====================================================================
    ������      ��GetBasType
    ����        ����ȡBas������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��Bas�����ͣ�0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u8 CImtConfig::GetBasType()
{
	if( !bIsRunBas )
	{
		return FALSE;
    }

	return m_tBasCfg.GetType();
}

/*====================================================================
    ������      ��GetBasAlias
    ����        ����ȡBas�ļ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����LPSTR lpstrBuf, Alias�ַ���ָ�룬�ڴ����û�����
                  u16 wLen, Alias�ַ�����������С��
                  ��С��ʵ�ʳ��ȣ������ַ������Խض�
    ����ֵ˵��  ���ɹ�����TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
BOOL CImtConfig::GetBasAlias(s8* lpstrBuf, u16 wLen)
{
	if( !bIsRunBas )
	{
		return FALSE;
    }

	if( lpstrBuf == NULL )
	{
		return FALSE;
    }

	strncpy( lpstrBuf, m_tBasCfg.GetAlias(), wLen );
	lpstrBuf[wLen-1] = '\0';
	return TRUE;
}

/*====================================================================
    ������      ��GetBasIpAddr
    ����        ����ȡBas��IP(������)
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��Bas��IP(������)��0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u32 CImtConfig::GetBasIpAddr()
{
	if( !bIsRunBas )
	{
		return FALSE;
    }

	return htonl(m_tBasCfg.GetIpAddr());
}

/*====================================================================
    ������      ��GetBasRecvStartPort
    ����        ����ȡBas����ʼ���ն˿ں�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����ʼ���ն˿ںţ�0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u16 CImtConfig::GetBasRecvStartPort()
{
	if( !bIsRunBas )
	{
		return FALSE;
    }

	return m_tBasCfg.GetEqpRecvPort();
}


/*====================================================================
    ������      ��GetBasMaxAdpChannel
    ����        ����ȡBas��ÿ��MAP���������ͨ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ���������ͨ������0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u8 CImtConfig::GetBasMaxAdpChannel()
{
	if( !bIsRunBas )
	{
		return FALSE;
    }

	return m_tBasCfg.GetAdpChnNum();
}

/*====================================================================
    ������      ��GetVMPId
    ����        ����ȡVMP��ID
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵���� 
    ����ֵ˵��  ��VMP��ID��0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u8 CImtConfig::GetVMPId()
{
	if( !bIsRunVMP )
	{
		return FALSE;
    }

	return m_tVMPCfg.GetEqpId();
}

/*====================================================================
    ������      ��GetVMPType
    ����        ����ȡVMP������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��VMP�����ͣ�0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u8 CImtConfig::GetVMPType()
{
	if( !bIsRunVMP )
	{
		return FALSE;
    }

	return m_tVMPCfg.GetType();
}


/*====================================================================
    ������      ��GetVMPAlias
    ����        ����ȡVMP�ļ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����LPSTR lpstrBuf, Alias�ַ���ָ�룬�ڴ����û�����
                  u16 wLen, Alias�ַ�����������С��
                  ��С��ʵ�ʳ��ȣ������ַ������Խض�
    ����ֵ˵��  ���ɹ�����TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
BOOL CImtConfig::GetVMPAlias(s8* lpstrBuf, u16 wLen)
{
	if( !bIsRunVMP )
	{
		return FALSE;
    }

	if( lpstrBuf == NULL )
	{
		return FALSE;
    }

	strncpy( lpstrBuf, m_tVMPCfg.GetAlias(), wLen );
	lpstrBuf[wLen-1] = '\0';
	return TRUE;
}


/*====================================================================
    ������      ��GetVMPIpAddr
    ����        ����ȡVMP��IP(������)
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��VMP��IP(������)��0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u32 CImtConfig::GetVMPIpAddr()
{
	if( !bIsRunVMP )
	{
		return FALSE;
    }

	return htonl(m_tVMPCfg.GetIpAddr());
}
	

/*====================================================================
    ������      ��GetVMPRecvStartPort
    ����        ����ȡVMP����ʼ���ն˿ں�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����ʼ���ն˿ںţ�0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u16 CImtConfig::GetVMPRecvStartPort()
{
	if( !bIsRunVMP )
	{
		return FALSE;
    }

	return m_tVMPCfg.GetEqpRecvPort();
}


/*====================================================================
    ������      ��GetVMPDecodeNumber
    ����        ����ȡVMPͬʱ��Ƶ����·��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ������·����0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u8  CImtConfig::GetVMPDecodeNumber()
{
	if( !bIsRunVMP )
	{
		return FALSE;
    }

	return m_tVMPCfg.GetEncodeNum();
}

/*====================================================================
    ������      ��GetPrsId
    ����        ����ȡPrs��ID
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��Prs��ID��0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u8 CImtConfig::GetPrsId()
{
	if( !bIsRunPrs )
	{
		return FALSE;
    }

	return m_tPrsCfg.GetEqpId();
}

/*====================================================================
    ������      ��GetPrsType
    ����        ����ȡPrs������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵���� 
    ����ֵ˵��  ��Prs�����ͣ�0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u8 CImtConfig::GetPrsType()
{
	if( !bIsRunPrs )
	{
		return FALSE;
    }

	return m_tPrsCfg.GetType();
}

/*====================================================================
    ������      ��GetPrsAlias
    ����        ����ȡPrs�ļ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����LPSTR lpstrBuf, Alias�ַ���ָ�룬�ڴ����û�����
                  u16 wLen, Alias�ַ�����������С��
                  ��С��ʵ�ʳ��ȣ������ַ������Խض�
    ����ֵ˵��  ���ɹ�����TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
BOOL CImtConfig::GetPrsAlias(s8* lpstrBuf, u16 wLen)
{
	if( !bIsRunPrs )
	{
		return FALSE;
    }

	if( lpstrBuf == NULL )
	{
		return FALSE;
    }

	strncpy( lpstrBuf, m_tPrsCfg.GetAlias(), wLen );
	lpstrBuf[wLen-1] = '\0';
	return TRUE;
}

/*====================================================================
    ������      ��GetPrsIpAddr
    ����        ����ȡPrs��IP(������)
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��Prs��IP(������)��0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u32 CImtConfig::GetPrsIpAddr()
{
	if( !bIsRunPrs )
	{
		return FALSE;
    }

	return htonl(m_tPrsCfg.GetIpAddr());
}

/*====================================================================
    ������      ��GetPrsRecvStartPort
    ����        ����ȡPrs����ʼ���ն˿ں�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����ʼ���ն˿ںţ�0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u16 CImtConfig::GetPrsRecvStartPort()
{
	if( !bIsRunPrs )
	{
		return FALSE;
    }

	return m_tPrsCfg.GetEqpRecvPort();
}

/*====================================================================
    ������      ��GetPrsRetransPara
    ����        ����ȡPrs�ش�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16* pwFistTimeSpan ��һ���ش�����
                  u16 *pwSecondTimeSpan �ڶ����ش�����
                  u16* pwThirdTimeSpan �������ش�����
                  u16* pwRejectTimeSpan ���ڶ�����ʱ���� 
    ����ֵ˵��  ������·����0��ʾʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
BOOL  CImtConfig::GetPrsRetransPara(u16* pwFistTimeSpan, u16 *pwSecondTimeSpan, 
									u16* pwThirdTimeSpan, u16* pwRejectTimeSpan )
{
	if( !bIsRunPrs )
    {
		return FALSE;
    }

	if( pwFistTimeSpan == NULL || pwSecondTimeSpan == NULL 
		|| pwThirdTimeSpan == NULL || pwRejectTimeSpan == NULL )
	{
		return FALSE;
    }

	*pwFistTimeSpan = m_tPrsCfg.GetFirstTimeSpan();
	*pwSecondTimeSpan = m_tPrsCfg.GetSecondTimeSpan();
	*pwThirdTimeSpan = m_tPrsCfg.GetThirdTimeSpan();
	*pwRejectTimeSpan = m_tPrsCfg.GetRejectTimeSpan();

	return TRUE;
}


/*====================================================================
    ������      ��SetMixerConfig
    ����        ������MIXER��������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����MIXER������Ϣ�ṹ 
    ����ֵ˵��  ���ɹ�����TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
BOOL CImtConfig::SetMixerConfig(TEqpMixerEntry *ptMixerCfg)
{
	if( ptMixerCfg == NULL )
	{
		return FALSE;
    }
	m_tMixerCfg = *ptMixerCfg;
	bIsRunMixer = TRUE;
    byMcuId = ptMixerCfg->GetMcuId();

	return TRUE;
}

/*====================================================================
    ������      ��GetTVWallMAPId
    ����        ����ȡTVWall��MAP����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TVWall������Ϣ�ṹ
    ����ֵ˵��  ��MAP����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
BOOL CImtConfig::SetTVWallConfig(TEqpTVWallEntry *ptTVWallCfg)
{
	if( ptTVWallCfg == NULL )
	{
		return FALSE;
    }
	m_tTVWallCfg = *ptTVWallCfg;
	bIsRunTVWall = TRUE;
    byMcuId = ptTVWallCfg->GetMcuId();

	return TRUE;
}

/*====================================================================
    ������      ��SetBasConfig
    ����        ������BAS��������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����BAS������Ϣ�ṹ 
    ����ֵ˵��  ���ɹ�����TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
BOOL CImtConfig::SetBasConfig(TEqpBasEntry *ptBasCfg)
{
	if( ptBasCfg == NULL )
	{
		return FALSE;
    }
	m_tBasCfg = *ptBasCfg;
	bIsRunBas= TRUE;
    byMcuId = ptBasCfg->GetMcuId();

	return TRUE;
}

/*====================================================================
    ������      ��SetPrsConfig
    ����        ������prs��������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����prs������Ϣ�ṹ
    ����ֵ˵��  ���ɹ�����TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
BOOL CImtConfig::SetPrsConfig(TEqpPrsEntry *ptPrsCfg)
{
	if( ptPrsCfg == NULL )
	{
		return FALSE;
    }
	m_tPrsCfg = *ptPrsCfg;
	bIsRunPrs= TRUE;
    byMcuId = ptPrsCfg->GetMcuId();

	return TRUE;
}

/*====================================================================
    ������      ��SetVMPConfig
    ����        ������VMP��������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����STRUCT_mcueqpVMPEntry_mem *ptVMPCfg  VMP������Ϣ�ṹ��ָ��
    ����ֵ˵��  ���ɹ�����TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
BOOL CImtConfig::SetVMPConfig(TEqpVMPEntry *ptVMPCfg)
{
	if( ptVMPCfg == NULL )
	{
		return FALSE;
    }
	m_tVMPCfg = *ptVMPCfg;
	bIsRunVMP = TRUE;
    byMcuId = ptVMPCfg->GetMcuId();

	return TRUE;
}

/*====================================================================
    ������      ��ReadConfig
    ����        ����MMP��������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ���ɹ�����TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
BOOL CImtConfig::ReadConfig()
{
	BOOL bResult;

	//��������
	bResult = ReadBoardInfo();
	if(bResult == FALSE)
	{
		return FALSE;
    }

	//MAP����, ����û��
	ReadMAPConfig();

	return TRUE;
}

/*====================================================================
    ������      ��ReadBoardInfo
    ����        ����MMP�ĵ���������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ���ɹ�����TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
BOOL CImtConfig::ReadBoardInfo()
{
    s8   achProfileName[32];
    BOOL bResult;
    s8   achDefStr[] = "Cannot find the section or key";
    s8   achReturn[MAX_VALUE_LEN + 1];
    s32  nValue;

	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);

    bResult = GetRegKeyString( achProfileName, "BoardSystem", "MpcIpAddr", 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( FALSE == bResult)  
	{
		printf("[Imt] Wrong profile while reading %s!\n", "MpcIpAddr" );
		dwMpcIpAddr = 0;
	}
    else
    {
        dwMpcIpAddr = ntohl( INET_ADDR( achReturn ) );
		if( 0xffffffff == dwMpcIpAddr )
		{
			dwMpcIpAddr = 0;
		}
    }

	bResult = GetRegKeyInt( achProfileName, "BoardSystem", "MpcPort", 0, &nValue );
	if( FALSE == bResult)  
	{
		printf("[Imt] Wrong profile while reading %s!\n", "MpcPort" );
		nValue = MCU_LISTEN_PORT;
	}
	else
    {
        wMpcPort = ( u16 )nValue;
    }

    memset(achReturn, '\0', sizeof(achReturn));
    bResult = GetRegKeyString( achProfileName, "BoardSystem", "MpcIpAddrB", 
                                            achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( FALSE == bResult )  
	{
		printf("[Imt] Wrong profile while reading %s!\n", "MpcIpAddrB" );
        dwMpcIpAddrB = 0;
	}
    else
    {
		dwMpcIpAddrB = ntohl( INET_ADDR( achReturn ) );
		if( 0xffffffff == dwMpcIpAddrB )
		{
			dwMpcIpAddrB = 0;
		}
    }

    bResult = GetRegKeyInt(achProfileName, "BoardSystem", "MpcPortB", 0, &nValue);
    if(FALSE == bResult)
    {
        OspPrintf(TRUE, FALSE, "[Imt] Wrong profile while reading %s\n", "MpcPortB");
        nValue = MCU_LISTEN_PORT;
    }
    wMpcPortB = (u16)nValue;

    if(dwMpcIpAddr == dwMpcIpAddrB)
    {
        dwMpcIpAddrB = 0;
        wMpcPortB = 0;
    }
	
    bResult = GetRegKeyInt(achProfileName, "IsFront", "Flag", 1, &nValue);
	if( bResult == FALSE )  
	{
		printf("[Imt] Wrong profile while reading %s!\n", "IsFront" );
        nValue = 1;
	}
    m_byChoice = (u8)nValue;

#ifdef WIN32 
    memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "brdcfgdebug.ini");
	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Layer", 0, &nValue );
	if( bResult == FALSE )  
	{
		printf("[Agent] Wrong profile while reading %s!\n", "Layer" );
		return( FALSE );
	}
	u8 byBrdLayer = (u8)nValue;

	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Slot", 0, &nValue );
	if( bResult == FALSE )  
	{
		printf("[Agent] Wrong profile while reading %s!\n", "Slot" );
		return( FALSE );
	}
	byBrdId = (byBrdLayer<<4) | ((u8)nValue+1);
#else
#ifdef IMT
	TBrdPosition tBoardPosition;
	BrdQueryPosition( &tBoardPosition );
	byBrdId = (tBoardPosition.byBrdLayer<<4) | (tBoardPosition.byBrdSlot+1);
#else
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "brdcfgdebug.ini");
	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Layer", 0, &nValue );
	if( bResult == FALSE )  
	{
		printf("[Agent] Wrong profile while reading %s!\n", "Layer" );
		return( FALSE );
	}
	u8 byBrdLayer = (u8)nValue;

	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Slot", 0, &nValue );
	if( bResult == FALSE )  
	{
		printf("[Agent] Wrong profile while reading %s!\n", "Slot" );
		return( FALSE );
	}
	byBrdId = (byBrdLayer<<4) | ((u8)nValue+1);
#endif
#endif

    if(0 == dwMpcIpAddr && 0 == dwMpcIpAddrB)
    {
        printf("[Imt] The A and B Mpc's Ip are all 0.\n");
        return FALSE;
    }

    //��ȡosp����������
    GetRegKeyInt( achProfileName, "BoardSystem", "HeartBeatTime", DEF_OSPDISC_HBTIME, &nValue );
    if (nValue >= 3)
    {
        m_wDiscHeartBeatTime = (u16)nValue;
    }
    
    GetRegKeyInt( achProfileName, "BoardSystem", "HeartBeatNum", DEF_OSPDISC_HBNUM, &nValue );
    if (nValue >= 1)
    {
        m_byDiscHeartBeatNum = (u8)nValue;
    }    

	return( TRUE );
}

/*====================================================================
    ������      ��GetMAPMemSpeed
    ����        ��ͨ��MAP��CPU��Ƶ�õ����ڴ���Ƶ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����DWORD dwCoreSpeed:MAP��CPU��Ƶ
    ����ֵ˵��  ��MAP���ڴ���Ƶ��0��ʾû�в鵽
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
u32 CImtConfig::GetMAPMemSpeed(u32 dwCoreSpeed)
{
	u16    wLoop=0;
	static u32 adwCoreSpdToMemSpdTable[][2] = {
		{68,68},  {74,74},  {81,81},  {88,88},  {95,95},  {101,101},{108,108}, {115,115},{122,122},
		{128,128},{135,68}, {149,74}, {162,81}, {176,88}, {189,95}, {203,101}, {216,108},{230,115},
		{243,122},{257,128},{270,135},{284,142},{297,99}, {311,104},{324,108}, {338,113},{351,117},
		{365,122},{378,126},{392,131},{405,135},{419,140},{432,144},{446,149}, {459,153},{473,158},
		{486,162},{500,167},{0,0}
	};

	while( adwCoreSpdToMemSpdTable[wLoop][0] != 0)
	{
		if( adwCoreSpdToMemSpdTable[wLoop][0] == dwCoreSpeed )
        {
			return adwCoreSpdToMemSpdTable[wLoop][1];
        }
		wLoop++;
	}
	return 0;
}

/*====================================================================
    ������      ��TableMemoryFree
    ����        ���ͷŵ���̬���루malloc���ı�ṹ�ڴ�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����void **ppMem, ָ������ָ��
                  u32 dwEntryNum, ָ�������С
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
void CImtConfig::TableMemoryFree( void **ppMem, u32 dwEntryNum )
{
    u32    dwLoop;

    if( ppMem == NULL )
    {
        return;
    }

    for( dwLoop = 0; dwLoop < dwEntryNum; dwLoop++ )
    {
        if( ppMem[dwLoop] != NULL )
        {
            free( ppMem[dwLoop] );
        }
    }

    free( ppMem );
}

/*====================================================================
    ������      ��GetTVWallMAPId
    ����        ����MMP��MAP������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ���ɹ�����TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����

====================================================================*/
BOOL CImtConfig::ReadMAPConfig()
{
    char    achProfileName[32];
    s8*   *lpszTable;
    char    chSeps[] = " \t";       /* space or tab as seperators */
    char    *pchToken;
    u32   dwLoop;
    BOOL    bResult = TRUE;
    u32   dwMemEntryNum;
 
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "mapcfg.ini");
   
    /* get the number of entry */
    bResult = GetRegKeyInt( achProfileName, "MAPTable", STR_ENTRY_NUM, 
                0, (s32*)&dwMemEntryNum );
	if( bResult == FALSE ) 
	{
        printf("[ReadMAPConfig] GetRegKeyInt Ent Num failed !\n");
		return( FALSE );
	}
	if( dwMemEntryNum > 3 ) /*���3��MAP*/
	{
        printf("[ReadMAPConfig] Ent Num larger than 3, failed !\n");
		return( FALSE );
	}
	byMAPCount = (u8)dwMemEntryNum;

	/*alloc memory*/
    lpszTable = (char**)malloc( dwMemEntryNum * sizeof( s8* ) );
    for( dwLoop = 0; dwLoop < dwMemEntryNum; dwLoop++ )
    {
        lpszTable[dwLoop] = (char*)malloc( MAX_VALUE_LEN + 1 );
    }

	/*get the map table*/
    bResult = GetRegKeyStringTable( achProfileName, "MAPTable",
                   "fail", lpszTable, &dwMemEntryNum, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )
    {
        printf("[ReadMAPConfig] GetRegKeyStringTable failed !\n");
		return( FALSE );
    }
	if( dwMemEntryNum > 5 ) /*���5��MAP*/
	{
        printf("[ReadMAPConfig] Map Num larger than 5, failed !\n");
		return( FALSE );
	}

    /* analyze entry strings */
    for( dwLoop = 0; dwLoop < dwMemEntryNum; dwLoop++ )
    {
        /* MAP��������� */
        pchToken = strtok( lpszTable[dwLoop], chSeps );
        if( pchToken == NULL )
        {
            printf("[ReadMAPConfig] 1.pchToken == NULL \n");
            bResult = FALSE;
        }
        else
        {
            m_tMAPCfg[dwLoop].byMAPId = atoi( pchToken );
        }

        /* MAP��������Ƶ */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf("[ReadMAPConfig] 2.pchToken == NULL \n");
            bResult = FALSE;
        }
        else
        {
            m_tMAPCfg[dwLoop].dwMAPCoreSpeed = atoi( pchToken );
        }

		m_tMAPCfg[dwLoop].dwMAPMemSpeed = GetMAPMemSpeed( m_tMAPCfg[dwLoop].dwMAPCoreSpeed );


        /* MAP�������ڴ����� */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf("[ReadMAPConfig] 3.pchToken == NULL \n");
            bResult = FALSE;
        }
        else
        {
            m_tMAPCfg[dwLoop].dwMAPMemSize = atoi( pchToken );
        }
        /* MAP��������Ƶ�˿� */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf("[ReadMAPConfig] 4.pchToken == NULL \n");
            bResult = FALSE;
        }
        else
        {
            m_tMAPCfg[dwLoop].byMAPPort = atoi( pchToken );
        }
    }
    /* free memory */
    TableMemoryFree( ( void ** )lpszTable, dwMemEntryNum );
    
    return( bResult );
}