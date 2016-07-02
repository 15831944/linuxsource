/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : mediabrdconfig.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ������ú�������
   ����        : jianghy
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/08/25  1.0         jianghy       ����
   2004/12/05  3.5         �� ��        �½ӿڵ��޸�
******************************************************************************/
#ifndef MEDIABRDCONFIG_H
#define MEDIABRDCONFIG_H

#define MEMSIZE_1K			(u32)(1024)
#define MEMSIZE_BYTE_1K		(u32)(1*MEMSIZE_1K)
#define MEMSIZE_BYTE_1M		(u32)(MEMSIZE_BYTE_1K*MEMSIZE_1K)

#include "osp.h"
#include "mcuconst.h"
#include "boardconfigbasic.h"
#include "mcuagtstruct.h"

typedef struct
{
	u8		byMAPId;		/*MAP���������*/
	u32		dwMAPCoreSpeed;	/*MAP��������Ƶ*/
	u32		dwMAPMemSpeed;	/*MAP�������ڴ���Ƶ*/
	u32		dwMAPMemSize;	/*MAP�������ڴ�����*/
	u8		byMAPPort;		/*MAP��������Ƶ�˿�*/
}TMAPInfo;

class CMediaBrdConfig:public CBBoardConfig
{
public:
	/*���캯��*/
	CMediaBrdConfig()
	{
		m_bIsRunMixer = FALSE;		//�Ƿ�����MIXER
		m_bIsRunTVWall = FALSE;		//�Ƿ�����TVWall
		m_bIsRunBas = FALSE;		//�Ƿ�����Bas
		m_bIsRunVMP = FALSE;		//�Ƿ�����VMP
		m_bIsRunPrs = FALSE;		//�Ƿ�����PRS
		m_bIsRunMpw = FALSE;		//�Ƿ�����Mpw

		memset(&m_tMixerCfg, 0, sizeof(m_tMixerCfg));
		memset(&m_tTVWallCfg, 0, sizeof(m_tTVWallCfg));
		memset(&m_tBasCfg, 0, sizeof(m_tBasCfg));
		memset(&m_tVMPCfg, 0, sizeof(m_tVMPCfg));
		memset(&m_tPrsCfg, 0, sizeof(m_tPrsCfg));
		memset(&m_tMpwCfg, 0, sizeof(m_tMpwCfg));

	#ifdef IMT
		//Ҫ���ݱ�������ȷ����IMT����APU
		TBrdPosition tBoardPosition;
		BrdQueryPosition( &tBoardPosition );   //�˴�����תΪTBrdPos����û�д洢ת����jlb_081120

		if( tBoardPosition.byBrdID == BRD_TYPE_IMT/*DSL8000_BRD_IMT*/ )
		{
			m_byMAPCount=3;
			memset(m_tMAPCfg, 0, sizeof(m_tMAPCfg));
			for(u16 wLoop=0; wLoop<m_byMAPCount; wLoop++)
			{
				m_tMAPCfg[wLoop].byMAPId = (u8)wLoop;
				m_tMAPCfg[wLoop].dwMAPCoreSpeed = 392;
				m_tMAPCfg[wLoop].dwMAPMemSpeed = 131;
				m_tMAPCfg[wLoop].dwMAPMemSize = 64*MEMSIZE_BYTE_1M;
			}
		}
		else
		{
			m_byMAPCount=1;
			memset(m_tMAPCfg, 0, sizeof(m_tMAPCfg));
			for(u16 wLoop=0; wLoop<m_byMAPCount; wLoop++)
			{
				m_tMAPCfg[wLoop].byMAPId = (u8)wLoop;
				m_tMAPCfg[wLoop].dwMAPCoreSpeed = 392;
				m_tMAPCfg[wLoop].dwMAPMemSpeed = 131;
				m_tMAPCfg[wLoop].dwMAPMemSize = 64*MEMSIZE_BYTE_1M;
			}
		}
	#endif

	#ifdef MMP
		m_byMAPCount=5;
		memset(m_tMAPCfg, 0, sizeof(m_tMAPCfg));
		for(u16 wLoop=0; wLoop<m_byMAPCount; wLoop++)
		{
			m_tMAPCfg[wLoop].byMAPId = (u8)wLoop;
			m_tMAPCfg[wLoop].dwMAPCoreSpeed = 392;
			m_tMAPCfg[wLoop].dwMAPMemSpeed = 131;
			m_tMAPCfg[wLoop].dwMAPMemSize = 64*MEMSIZE_BYTE_1M;
		}
	#endif
	}

	
	//��������
	
	virtual CMediaBrdConfig::~CMediaBrdConfig()
	{
	}

	BOOL32 ReadConfig()
	{
		BOOL32 bResult;

		//��������
		bResult = ReadConnectMcuInfo();
		if(bResult == FALSE)
		{
			printf("[ReadConfig] ReadBoardInfo failed !\n");
			return FALSE;
		}

		//MAP����, ����û��
		ReadMAPConfig();

		return TRUE;
	}

public:
	u8 GetMcuId()
	{
		return m_byMcuId;
	}
	void SetMcuId(u8 byMcuId)
	{
		m_byMcuId = byMcuId;
	}

	TEapuCfg GetEapuCfg() 
	{  
		return m_tEapuCfg;
	}
	/*====================================================================
	���ܣ��Ƿ����л�����
	��������
	����ֵ�����з���TRUE����֮FALSE
	====================================================================*/
	BOOL IsRunMixer()
	{
		return m_bIsRunMixer;
	}

	/*====================================================================
	���ܣ��Ƿ�����BAS
	��������
	����ֵ�����з���TRUE����֮FALSE
	====================================================================*/
	BOOL32 IsRunBas()
	{
		return m_bIsRunBas;
	}

	/*====================================================================
	���ܣ��Ƿ�����VMP
	��������
	����ֵ�����з���TRUE����֮FALSE
	====================================================================*/
	BOOL32 IsRunVMP()
	{
		return m_bIsRunVMP;
	}

	/*====================================================================
	���ܣ��Ƿ�����Prs
	��������
	����ֵ�����з���TRUE����֮FALSE
	====================================================================*/
	BOOL32 IsRunPrs()
	{
		return m_bIsRunPrs;
	}

	/*====================================================================
	���ܣ��Ƿ�����TVWALL
	��������
	����ֵ�����з���TRUE����֮FALSE
	====================================================================*/
	BOOL32 IsRunTVWall()
	{
		return m_bIsRunTVWall;
	}

	/*====================================================================
	���ܣ���ȡMixer��MAP����
	������u8* pbyIdBuf: ���MAP��ŵ����� 
	      u8 byBufLen: ���鳤��
	����ֵ��MAP����
	====================================================================*/
	u8 GetMixerMAPId( u8* pbyIdBuf, u8 byBufLen )
	{
		u8 byLoop = 0;

		if( !m_bIsRunMixer )
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
	���ܣ���ȡTVWall��MAP����
	������u8* pbyIdBuf: ���MAP��ŵ����� 
	      u8 byBufLen: ���鳤��
	����ֵ��MAP����
	====================================================================*/
	u8 GetTVWallMAPId( u8* pbyIdBuf, u8 byBufLen )
	{
		u8 byLoop = 0;

		if( !m_bIsRunTVWall )
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
	���ܣ���ȡBas��MAP����
	������u8* pbyIdBuf: ���MAP��ŵ����� 
	      u8 byBufLen: ���鳤��
	����ֵ��MAP����
	====================================================================*/
	u8 GetBasMAPId( u8* pbyIdBuf, u8 byBufLen )
	{
		u8 byLoop = 0;

		if( !m_bIsRunBas )
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
	���ܣ���ȡVMP��MAP����
	������u8* pbyIdBuf: ���MAP��ŵ����� 
	      u8 byBufLen: ���鳤��
	����ֵ��MAP����
	====================================================================*/
	u8 GetVMPMAPId( u8* pbyIdBuf, u8 byBufLen )
	{
		u8 byLoop = 0;

		if( !m_bIsRunVMP )
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
	���ܣ���ȡMAP��Ϣ
	������u8 byId: MAP��
	      u32 *pdwCoreSpeed: MAP��Ƶ
		  u32 *pdwMemSpeed: MAP���ڴ���Ƶ
		  u32 *pdwPort: MAP�Ķ˿ں�
	����ֵ���ɹ�����TRUE����֮FALSE
	====================================================================*/
	BOOL32 GetMAPInfo(u8 byId, u32 *pdwCoreSpeed, u32 *pdwMemSpeed, u32 *pdwMemSize, u32 *pdwPort)
	{
		if( pdwCoreSpeed == NULL || pdwMemSpeed == NULL || pdwMemSize == NULL || pdwPort == NULL )
		{
			return FALSE;
		}

		if( byId >= m_byMAPCount )
		{
			return FALSE;
		}

		*pdwCoreSpeed = m_tMAPCfg[byId].dwMAPCoreSpeed;
		*pdwMemSpeed = m_tMAPCfg[byId].dwMAPMemSpeed;
		*pdwMemSize = m_tMAPCfg[byId].dwMAPMemSize;
		*pdwPort = m_tMAPCfg[byId].byMAPPort;

		return TRUE;
	}


	/*Mixer��������Ϣ�ӿ�*/
	/*====================================================================
	���ܣ���ȡMixer��TAudioMixerCfg
	������TAudioMixerCfg &tAudioMixerCfg: ���Mixer��TAudioMixerCfg
	����ֵ��BOOL32
	====================================================================*/
	BOOL32 GetMixerCfg( TAudioMixerCfg &tAudioMixerCfg )
	{
		if( !m_bIsRunMixer )
		{
			return FALSE;
		}

		u8 buf[MAXNUM_MAP];
		s32 nNum = GetMixerMAPId(buf, MAXNUM_MAP);
        tAudioMixerCfg.wMAPCount = (u16)nNum;
        for(s32 nLp=0;nLp<nNum; nLp++)
        {
            tAudioMixerCfg.m_atMap[nLp].byMapId = buf[nLp];
            GetMAPInfo(buf[nLp],
                                     (u32*)&tAudioMixerCfg.m_atMap[nLp].dwCoreSpd,
                                     (u32*)&tAudioMixerCfg.m_atMap[nLp].dwMemSpd,
                                     (u32*)&tAudioMixerCfg.m_atMap[nLp].dwMemSize,
                                     (u32*)&tAudioMixerCfg.m_atMap[nLp].dwPort);
        }
        tAudioMixerCfg.dwConnectIP = GetMpcIpA();
        tAudioMixerCfg.wConnectPort = GetMpcPortA();
        tAudioMixerCfg.dwConnectIpB = GetMpcIpB();
        tAudioMixerCfg.wConnectPortB = GetMpcPortB();
        
        tAudioMixerCfg.byEqpType = GetMixerType();
        tAudioMixerCfg.byEqpId = GetMixerId();
        tAudioMixerCfg.dwLocalIP = GetMixerIpAddr();
        tAudioMixerCfg.wRcvStartPort = GetMixerRecvStartPort();
        tAudioMixerCfg.wMcuId = GetMcuId();
        tAudioMixerCfg.byMaxMixGroupCount = (u8)nNum;
        tAudioMixerCfg.byMaxChannelInGroup = GetMixerMaxChannelInGrp();
        GetMixerAlias(tAudioMixerCfg.achAlias,MAXLEN_ALIAS );
        tAudioMixerCfg.achAlias[MAXLEN_ALIAS] = '\0';
		return TRUE;
	}

	/*====================================================================
	���ܣ���ȡMixer��ID
	��������
	����ֵ��Mixer��ID��0��ʾʧ��
	====================================================================*/
	u8 GetMixerId()
	{
		if( !m_bIsRunMixer )
		{
			return FALSE;
		}

		return m_tMixerCfg.GetEqpId();
	}

	/*====================================================================
	���ܣ���ȡMixer������
	��������
	����ֵ��Mixer�����ͣ�0��ʾʧ��
	====================================================================*/
	u8 GetMixerType()
	{
		if( !m_bIsRunMixer )
		{
			return FALSE;
		}

		return m_tMixerCfg.GetType();
	}

	/*====================================================================
	���ܣ���ȡMixer�ļ���
	������s8* lpstrBuf, Alias�ַ���ָ�룬�ڴ����û�����
          u16  wLen, Alias�ַ�����������С����С��ʵ�ʳ��ȣ������ַ������Խض�
	����ֵ���ɹ�����TRUE����֮FALSE
	====================================================================*/
	BOOL32 GetMixerAlias(s8* lpstrBuf, u16  wLen)
	{
		if( !m_bIsRunMixer )
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
	���ܣ���ȡMixer��IP(������)
	��������
	����ֵ��Mixer��IP(������)��0��ʾʧ��
	====================================================================*/
	u32 GetMixerIpAddr()
	{
		if( !m_bIsRunMixer )
		{
			return FALSE;
		}

		return htonl(m_tMixerCfg.GetIpAddr());
	}
	
	/*====================================================================
	���ܣ���ȡMixer����ʼ���ն˿ں�
	��������
	����ֵ����ʼ���ն˿ںţ�0��ʾʧ��
	====================================================================*/
	u16  GetMixerRecvStartPort()
	{
		if( !m_bIsRunMixer )
		{
			return FALSE;
		}

		return m_tMixerCfg.GetEqpRecvPort();
	}

	/*====================================================================
	���ܣ���ȡ������ÿ��MAP��֧������������
	��������
	����ֵ��������������0��ʾʧ��
	====================================================================*/
	u8 GetMixerMaxMixGroupNum()
	{
		if( !m_bIsRunMixer )
		{
			return FALSE;
		}

		return m_tMixerCfg.GetMaxMixGrpNum();
	}
	
	/*====================================================================
	���ܣ���ȡÿ�����������ͨ����
	��������
	����ֵ�����ͨ������0��ʾʧ��
	====================================================================*/
	u8 GetMixerMaxChannelInGrp()
	{
		if( !m_bIsRunMixer )
		{
			return FALSE;
		}

		return m_tMixerCfg.GetMaxChnInGrp();
	}


	/*TVWALL��������Ϣ�ӿ�*/
	/*====================================================================
	���ܣ���ȡTVWall��TEqpCfg
	������TEqpCfg &tTvWallCfg: ���TVWall��TEqpCfg
	����ֵ��BOOL32
	====================================================================*/
	BOOL32 GetTVWallCfg( TEqpCfg &tTvWallCfg )
	{
		if ( !m_bIsRunTVWall )
		{
			return FALSE;
		}

		u8 buf[MAXNUM_MAP];
		s32 nNum = GetTVWallMAPId(buf, MAXNUM_MAP);
        for(s32 nLp=0;nLp<nNum;nLp++)
        {
            tTvWallCfg.m_atMap[nLp].byMapId = buf[nLp];
            GetMAPInfo(buf[nLp],
                       (u32*)&tTvWallCfg.m_atMap[nLp].dwCoreSpd,
                       (u32*)&tTvWallCfg.m_atMap[nLp].dwMemSpd,
                       (u32*)&tTvWallCfg.m_atMap[nLp].dwMemSize,
                       (u32*)&tTvWallCfg.m_atMap[nLp].dwPort);
        }
        tTvWallCfg.wMAPCount     = (u8)nNum;
        tTvWallCfg.dwConnectIP   = GetMpcIpA();
        tTvWallCfg.wConnectPort  = GetMpcPortA();
        tTvWallCfg.dwConnectIpB   = GetMpcIpB();
        tTvWallCfg.wConnectPortB  = GetMpcPortB();

        tTvWallCfg.byEqpType     = GetTWType();
        tTvWallCfg.byEqpId       = GetTWId();
        tTvWallCfg.dwLocalIP     = GetTWIpAddr();
        tTvWallCfg.wRcvStartPort = GetTWRecvStartPort();
        tTvWallCfg.wMcuId        = GetMcuId();
        GetTWAlias(tTvWallCfg.achAlias,MAXLEN_ALIAS );
        tTvWallCfg.achAlias[MAXLEN_ALIAS] = '\0';
		return TRUE;
	}

	/*====================================================================
	���ܣ���ȡTVWall��ID
	��������
	����ֵ��TVWall��ID��0��ʾʧ��
	====================================================================*/
	u8 GetTWId()
	{
		if( !m_bIsRunTVWall )
		{
			return FALSE;
		}

		return m_tTVWallCfg.GetEqpId();
	}

	/*====================================================================
	���ܣ���ȡTVWall������
	��������
	����ֵ��TVWall�����ͣ�0��ʾʧ��
	====================================================================*/
	u8 GetTWType()
	{
		if( !m_bIsRunTVWall )
		{
			return FALSE;
		}

		return m_tTVWallCfg.GetType();
	}

	/*====================================================================
	���ܣ���ȡTVWall�ļ���
	������s8* lpstrBuf, Alias�ַ���ָ�룬�ڴ����û�����
          u16  wLen, Alias�ַ�����������С����С��ʵ�ʳ��ȣ������ַ������Խض�
	����ֵ���ɹ�����TRUE����֮FALSE
	====================================================================*/
	BOOL32 GetTWAlias(s8* lpstrBuf, u16  wLen)
	{
		if( !m_bIsRunTVWall )
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
	���ܣ���ȡTVWall��IP(������)
	��������
	����ֵ��TVWall��IP(������)��0��ʾʧ��
	====================================================================*/
	u32 GetTWIpAddr()
	{
		if( !m_bIsRunTVWall )
		{
			return FALSE;
		}

		return htonl(m_tTVWallCfg.GetIpAddr());
	}

	/*====================================================================
	���ܣ���ȡTVWall��Ƶ��ʼ���ն˿ں�
	��������
	����ֵ����ʼ���ն˿ںţ�0��ʾʧ��
	====================================================================*/
	u16  GetTWRecvStartPort()
	{
		if( !m_bIsRunTVWall )
		{
			return FALSE;
		}

		return m_tTVWallCfg.GetEqpRecvPort();
	}

	/*====================================================================
	���ܣ���ȡ����ǽ�ķָʽ
	��������
	����ֵ���ָʽ��0��ʾʧ��
	====================================================================*/
	u8 GetTWDivStyle()
	{
		if( !m_bIsRunTVWall )
		{
			return FALSE;
		}

		// return m_tTVWallCfg.mcueqpTVWallEntDivStyle;
		return 1;
	}

	/*====================================================================
	���ܣ���ȡ����ǽ�ķָ����
	��������
	����ֵ���ָ������0��ʾʧ��
	====================================================================*/
	u8 GetTWDivNum()
	{
		if( !m_bIsRunTVWall )
		{
			return FALSE;
		}

		// return m_tTVWallCfg.mcueqpTVWallEntDivNum;
		return 1;
	}


	/*BAS��������Ϣ�ӿ�*/
	/*====================================================================
	���ܣ���ȡBas��TEqpCfg
	������TEqpCfg &tBasEqpCfg: ���Bas��TEqpCfg
	����ֵ��BOOL32
	====================================================================*/
	BOOL32 GetBasCfg( TEqpCfg &tBasEqpCfg )
	{
		if ( !m_bIsRunBas )
		{
			return FALSE;
		}

		u8 buf[MAXNUM_MAP];
		s32 nNum = GetBasMAPId(buf,MAXNUM_MAP);
        for(s32 nLp=0; nLp<MAXNUM_MAP; nLp++)
        {
            tBasEqpCfg.m_atMap[nLp].byMapId = buf[nLp];
            GetMAPInfo(buf[nLp],
                      (u32*)&tBasEqpCfg.m_atMap[nLp].dwCoreSpd,
                      (u32*)&tBasEqpCfg.m_atMap[nLp].dwMemSpd,
                      (u32*)&tBasEqpCfg.m_atMap[nLp].dwMemSize,
                      (u32*)&tBasEqpCfg.m_atMap[nLp].dwPort);
        }
        tBasEqpCfg.dwConnectIP = GetMpcIpA();
        tBasEqpCfg.wConnectPort = GetMpcPortA();
        tBasEqpCfg.dwConnectIpB = GetMpcIpB();
        tBasEqpCfg.wConnectPortB = GetMpcPortB();

        tBasEqpCfg.byEqpType = GetBasType();
        tBasEqpCfg.byEqpId = GetBasId();
        tBasEqpCfg.dwLocalIP = GetBasIpAddr();
        tBasEqpCfg.wRcvStartPort = GetBasRecvStartPort();
        tBasEqpCfg.wMcuId = GetMcuId();
        tBasEqpCfg.wMAPCount = (u8)nNum;
        GetBasAlias(tBasEqpCfg.achAlias,MAXLEN_ALIAS );
        tBasEqpCfg.achAlias[MAXLEN_ALIAS] = '\0';
		return TRUE;
	}

	/*====================================================================
	���ܣ���ȡBas��ID
	��������
	����ֵ��Bas��ID��0��ʾʧ��
	====================================================================*/
	u8 GetBasId()
	{
		if( !m_bIsRunBas )
		{
			return FALSE;
		}

		return m_tBasCfg.GetEqpId();
	}

	/*====================================================================
	���ܣ���ȡBas������
	��������
	����ֵ��Bas�����ͣ�0��ʾʧ��
	====================================================================*/
	u8 GetBasType()
	{
		if( !m_bIsRunBas )
		{
			return FALSE;
		}

		return m_tBasCfg.GetType();
	}

	/*====================================================================
	���ܣ���ȡBas�ļ���
	������s8* lpstrBuf, Alias�ַ���ָ�룬�ڴ����û�����
          u16  wLen, Alias�ַ�����������С����С��ʵ�ʳ��ȣ������ַ������Խض�
	����ֵ���ɹ�����TRUE����֮FALSE
	====================================================================*/
	BOOL32 GetBasAlias(s8* lpstrBuf, u16  wLen)
	{
		if( !m_bIsRunBas )
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
	���ܣ���ȡBas��IP(������)
	��������
	����ֵ��Bas��IP(������)��0��ʾʧ��
	====================================================================*/
	u32 GetBasIpAddr()
	{
		if( !m_bIsRunBas )
		{
			return FALSE;
		}

		return htonl(m_tBasCfg.GetIpAddr());
	}
	
	/*====================================================================
	���ܣ���ȡBas����ʼ���ն˿ں�
	��������
	����ֵ����ʼ���ն˿ںţ�0��ʾʧ��
	====================================================================*/
	u16  GetBasRecvStartPort()
	{
		if( !m_bIsRunBas )
		{
			return FALSE;
		}

		return m_tBasCfg.GetEqpRecvPort();
	}

	/*====================================================================
	���ܣ���ȡBas��ÿ��MAP���������ͨ����
	��������
	����ֵ���������ͨ������0��ʾʧ��
	====================================================================*/
	u8 GetBasMaxAdpChannel()
	{
		if( !m_bIsRunBas )
		{
			return FALSE;
		}

		return m_tBasCfg.GetAdpChnNum();
	}
	
	/*VMP��������Ϣ�ӿ�*/
	/*====================================================================
	���ܣ���ȡVmp��TVmpCfg
	������TVmpCfg &tVmpCfg: ���Vmp��TVmpCfg
	����ֵ��BOOL32
	====================================================================*/
	BOOL32 GetVmpCfg( TVmpCfg &tVmpCfg )
	{
		if ( !m_bIsRunVMP )
		{
			return FALSE;
		}

		u8 buf[MAXNUM_MAP];
		s32 nNum = GetVMPMAPId(buf, MAXNUM_MAP);
        GetMAPInfo(buf[0],
                   (u32*)&tVmpCfg.m_atMap[0].dwCoreSpd,
                   (u32*)&tVmpCfg.m_atMap[0].dwMemSpd,
                   (u32*)&tVmpCfg.m_atMap[0].dwMemSize,
                   (u32*)&tVmpCfg.m_atMap[0].dwPort);

        tVmpCfg.wMAPCount     = nNum;
        tVmpCfg.dwConnectIP   = GetMpcIpA();
        tVmpCfg.wConnectPort  = GetMpcPortA();
        tVmpCfg.dwConnectIpB  = GetMpcIpB();
        tVmpCfg.wConnectPortB = GetMpcPortB();
        
        tVmpCfg.byEqpType     = GetVMPType();
        tVmpCfg.byEqpId       = GetVMPId();
        tVmpCfg.dwLocalIP     = GetVMPIpAddr();
        tVmpCfg.wRcvStartPort = GetVMPRecvStartPort();
        tVmpCfg.wMcuId        = GetMcuId();
        tVmpCfg.byDbVid       = GetVMPDecodeNumber();
        GetVMPAlias(tVmpCfg.achAlias, MAXLEN_ALIAS);
        tVmpCfg.achAlias[MAXLEN_ALIAS] = '\0';
		return TRUE;
	}
	
	/*====================================================================
	���ܣ���ȡVMP��ID
	��������
	����ֵ��VMP��ID��0��ʾʧ��
	====================================================================*/
	u8 GetVMPId()
	{
		if( !m_bIsRunVMP )
		{
			return FALSE;
		}

		return m_tVMPCfg.GetEqpId();
	}

	/*====================================================================
	���ܣ���ȡVMP������
	��������
	����ֵ��VMP�����ͣ�0��ʾʧ��
	====================================================================*/
	u8 GetVMPType()
	{
		if( !m_bIsRunVMP )
		{
			return FALSE;
		}

		return m_tVMPCfg.GetType();
	}

	/*====================================================================
	���ܣ���ȡVMP�ļ���
	������s8* lpstrBuf, Alias�ַ���ָ�룬�ڴ����û�����
          u16  wLen, Alias�ַ�����������С����С��ʵ�ʳ��ȣ������ַ������Խض�
	����ֵ���ɹ�����TRUE����֮FALSE
	====================================================================*/
	BOOL32 GetVMPAlias(s8* lpstrBuf, u16  wLen)
	{
		if( !m_bIsRunVMP )
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
	���ܣ���ȡVMP��IP(������)
	��������
	����ֵ��VMP��IP(������)��0��ʾʧ��
	====================================================================*/
	u32 GetVMPIpAddr()
	{
		if( !m_bIsRunVMP )
		{
			return FALSE;
		}

		return htonl(m_tVMPCfg.GetIpAddr());
	}
	
	/*====================================================================
	���ܣ���ȡVMP����ʼ���ն˿ں�
	��������
	����ֵ����ʼ���ն˿ںţ�0��ʾʧ��
	====================================================================*/
	u16  GetVMPRecvStartPort()
	{
		if( !m_bIsRunVMP )
		{
			return FALSE;
		}

		return m_tVMPCfg.GetEqpRecvPort();
	}

	/*====================================================================
	���ܣ���ȡVMPͬʱ��Ƶ����·��
	��������
	����ֵ������·����0��ʾʧ��
	====================================================================*/
	u8  GetVMPDecodeNumber()
	{
		if( !m_bIsRunVMP )
		{
			return FALSE;
		}

		return m_tVMPCfg.GetEncodeNum();
	}


	/*MPW��������Ϣ�ӿ�*/
	/*====================================================================
	���ܣ���ȡMpw��TVmpCfg
	������TVmpCfg &tMpwCfg: ���Vmp��TVmpCfg
	����ֵ��BOOL32
	====================================================================*/
	BOOL32 GetMpwCfg( TVmpCfg &tMpwCfg )
	{
		if ( !m_bIsRunMpw )
		{
			return FALSE;
		}

		u8 buf[MAXNUM_MAP];
		s32 nNum = GetMpwMAPId(buf, MAXNUM_MAP);
        GetMAPInfo(buf[0],
                   (u32*)&tMpwCfg.m_atMap[0].dwCoreSpd,
                   (u32*)&tMpwCfg.m_atMap[0].dwMemSpd,
                   (u32*)&tMpwCfg.m_atMap[0].dwMemSize,
                   (u32*)&tMpwCfg.m_atMap[0].dwPort);

        tMpwCfg.wMAPCount     = nNum;
        tMpwCfg.dwConnectIP   = GetMpcIpA();
        tMpwCfg.wConnectPort  = GetMpcPortA();
        tMpwCfg.dwConnectIpB  = GetMpcIpB();
        tMpwCfg.wConnectPortB = GetMpcPortB();
        
        tMpwCfg.byEqpType     = GetMpwType();
        tMpwCfg.byEqpId       = GetMpwId();
        tMpwCfg.dwLocalIP     = GetMpwIpAddr();
        tMpwCfg.wRcvStartPort = GetMpwRecvStartPort();
        tMpwCfg.wMcuId        = GetMcuId();
        GetMpwAlias(tMpwCfg.achAlias, MAXLEN_ALIAS);
        tMpwCfg.achAlias[MAXLEN_ALIAS] = '\0';
		return TRUE;
	}
	
    u8  GetMpwId()
	{
		if(!m_bIsRunMpw)
		{
			return FALSE;
		}
		return m_tMpwCfg.GetEqpId();
	}

    u8 GetMpwType()
	{
		if( !m_bIsRunMpw )
		{
			return FALSE;
		}

		return m_tMpwCfg.GetType();
	}

    BOOL32 GetMpwAlias(s8* lpstrBuf, u16  wLen)
	{
		if(!m_bIsRunMpw || NULL == lpstrBuf)
		{
			return FALSE;
		}
    
		strncpy(lpstrBuf, m_tMpwCfg.GetAlias(), wLen);
		lpstrBuf[wLen-1] = '\0';
		return TRUE;
	}

    u32 GetMpwIpAddr()
	{
		if(!m_bIsRunMpw)
		{
			return FALSE;
		}
		return htonl(m_tMpwCfg.GetIpAddr());
	}

    u16  GetMpwRecvStartPort()
	{
		if(!m_bIsRunMpw)
		{
			return FALSE;
		}
		return m_tMpwCfg.GetEqpRecvPort();
	}

    BOOL32 IsRunMpw()
	{
		return m_bIsRunMpw;
	}

    u8  GetMpwMAPId( u8* pbyIdBuf, u8 byBufLen )
	{
		u8 byLoop = 0;

		if( !m_bIsRunMpw )
		{
			return FALSE;
		}

		for( byLoop = 0; byLoop < m_tMpwCfg.GetUsedMapNum() && byLoop < byBufLen; byLoop++ )
		{
			pbyIdBuf[byLoop] = m_tMpwCfg.GetUsedMapId(byLoop);
		}

		return byLoop;
	}

    //����Mpw��������Ϣ
    BOOL32 SetMpwConfig(TEqpMPWEntry* ptMpwCfg)
	{
		if(NULL == ptMpwCfg)
		{
			return FALSE;
		}

		m_tMpwCfg = *ptMpwCfg;
		m_bIsRunMpw = TRUE;
		m_byMcuId = ptMpwCfg->GetMcuId();

		return TRUE;
	}
	/*Prs��������Ϣ�ӿ�*/
	/*====================================================================
	���ܣ���ȡPrs��TPrsCfg
	������TPrsCfg &tPrsCfg: ���Prs��TPrsCfg
	����ֵ��BOOL32
	====================================================================*/
	BOOL32 GetPrsCfg( TPrsCfg &tPrsCfg )
	{
		if ( !m_bIsRunPrs )
		{
			return FALSE;
		}

		tPrsCfg.wMcuId        = GetMcuId();
        tPrsCfg.byEqpId       = GetPrsId();
        tPrsCfg.byEqpType     = GetPrsType();
        tPrsCfg.dwLocalIP     = GetPrsIpAddr();
        tPrsCfg.wRcvStartPort = GetPrsRecvStartPort();
        tPrsCfg.dwConnectIP   = GetMpcIpA();
        tPrsCfg.wConnectPort  = GetMpcPortA();
        tPrsCfg.dwConnectIpB  = GetMpcIpB();
        tPrsCfg.wConnectPortB = GetMpcPortB();
        
        GetPrsAlias(tPrsCfg.achAlias, MAXLEN_ALIAS);
        tPrsCfg.achAlias[MAXLEN_ALIAS] = '\0';
        GetPrsRetransPara(&tPrsCfg.m_wFirstTimeSpan,
                          &tPrsCfg.m_wSecondTimeSpan,
                          &tPrsCfg.m_wThirdTimeSpan,
                          &tPrsCfg.m_wRejectTimeSpan);
		return TRUE;
	}

	/*====================================================================
	���ܣ���ȡPrs��ID
	��������
	����ֵ��Prs��ID��0��ʾʧ��
	====================================================================*/
	u8 GetPrsId()
	{
		if( !m_bIsRunPrs )
		{
			return FALSE;
		}

		return m_tPrsCfg.GetEqpId();
	}

	/*====================================================================
	���ܣ���ȡPrs������
	��������
	����ֵ��Prs�����ͣ�0��ʾʧ��
	====================================================================*/
	u8 GetPrsType()
	{
		if( !m_bIsRunPrs )
		{
			return FALSE;
		}

		return m_tPrsCfg.GetType();
	}

	/*====================================================================
	���ܣ���ȡPrs�ļ���
	������s8* lpstrBuf, Alias�ַ���ָ�룬�ڴ����û�����
          u16  wLen, Alias�ַ�����������С����С��ʵ�ʳ��ȣ������ַ������Խض�
	����ֵ���ɹ�����TRUE����֮FALSE
	====================================================================*/
	BOOL32 GetPrsAlias(s8* lpstrBuf, u16  wLen)
	{
		if( !m_bIsRunPrs )
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
	���ܣ���ȡPrs��IP(������)
	��������
	����ֵ��Prs��IP(������)��0��ʾʧ��
	====================================================================*/
	u32 GetPrsIpAddr()
	{
		if( !m_bIsRunPrs )
		{
			return FALSE;
		}

		return htonl(m_tPrsCfg.GetIpAddr());
	}
	
	/*====================================================================
	���ܣ���ȡPrs����ʼ���ն˿ں�
	��������
	����ֵ����ʼ���ն˿ںţ�0��ʾʧ��
	====================================================================*/
	u16  GetPrsRecvStartPort()
	{
		if( !m_bIsRunPrs )
		{
			return FALSE;
		}

		return m_tPrsCfg.GetEqpRecvPort();
	}

	/*====================================================================
	���ܣ���ȡPrs�ش�����
	������u16* pwFistTimeSpan ��һ���ش�����
	      u16 *pwSecondTimeSpan �ڶ����ش�����
		  u16* pwThirdTimeSpan �������ش�����
		  u16* pwRejectTimeSpan ���ڶ�����ʱ����
	����ֵ������·����0��ʾʧ��
	====================================================================*/
	BOOL32  GetPrsRetransPara(u16* pwFistTimeSpan, u16 *pwSecondTimeSpan, 
		u16* pwThirdTimeSpan, u16* pwRejectTimeSpan )
	{
		if( !m_bIsRunPrs )
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
public:
    u8   m_byTvWallModel; // ����ǽģʽ��0���ޣ�1����Ƶ��2����Ƶ��3������Ƶ

protected:
	BOOL32 ReadMAPConfig()
	//�������ļ���MAP��������Ϣ
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
		m_byMAPCount = (u8)dwMemEntryNum;

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

	u32 GetMAPMemSpeed(u32 dwCoreSpeed)
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

	void TableMemoryFree( void **ppMem, u32 dwEntryNum )
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

protected:
	//����MIXER��������Ϣ
	BOOL32 SetMixerConfig(TEqpMixerEntry *ptMixerCfg)
	{
		if( ptMixerCfg == NULL )
		{
			return FALSE;
		}
		m_tMixerCfg = *ptMixerCfg;
		m_bIsRunMixer = TRUE;
		m_byMcuId = ptMixerCfg->GetMcuId();

		return TRUE;
	}

	//����TVWALL��������Ϣ
	BOOL32 SetTVWallConfig(TEqpTVWallEntry *ptTVWallCfg)
	{
		if( ptTVWallCfg == NULL )
		{
			return FALSE;
		}
		m_tTVWallCfg = *ptTVWallCfg;
		m_bIsRunTVWall = TRUE;
		m_byMcuId = ptTVWallCfg->GetMcuId();

		return TRUE;
	}

	//����BAS��������Ϣ
	BOOL32 SetBasConfig(TEqpBasEntry *ptBasCfg)
	{
		if( ptBasCfg == NULL )
		{
			return FALSE;
		}
		m_tBasCfg = *ptBasCfg;
		m_bIsRunBas= TRUE;
		m_byMcuId = ptBasCfg->GetMcuId();

		return TRUE;
	}

	//����VMP��������Ϣ
	BOOL32 SetVMPConfig(TEqpVMPEntry *ptVMPCfg)
	{
		if( ptVMPCfg == NULL )
		{
			return FALSE;
		}
		m_tVMPCfg = *ptVMPCfg;
		m_bIsRunVMP = TRUE;
		m_byMcuId = ptVMPCfg->GetMcuId();

		return TRUE;
	}

	//����Prs��������Ϣ
	BOOL32 SetPrsConfig(TEqpPrsEntry *ptPrsCfg)
	{
		if( ptPrsCfg == NULL )
		{
			return FALSE;
		}
		m_tPrsCfg = *ptPrsCfg;
		m_bIsRunPrs= TRUE;
		m_byMcuId = ptPrsCfg->GetMcuId();

		return TRUE;
	}
	//����EMIXER��������Ϣ
	/*
	BOOL32 SetEqpEMixerEntry(u8 byMixIdx,TEqpMixerEntry tEqpMixerEntry)
	{
		m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].byEqpId       = tEqpMixerEntry.GetEqpId();
		m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].byEqpType     = EQP_TYPE_EMIXER;
		m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].wMcuId        = LOCAL_MCUID;
		m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].wRcvStartPort = tEqpMixerEntry.GetEqpRecvPort();
		strcpy(m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].achAlias, tEqpMixerEntry.GetAlias());
		m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].achAlias[MAXLEN_EQP_ALIAS-1] = '\0';
		m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].m_byMixerMemberNum = tEqpMixerEntry.GetMaxChnInGrp();
		m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].m_bIsMultiFormat = FALSE;
		m_bIsRunMixer = TRUE;
		m_byMcuId = tEqpMixerEntry.GetMcuId();

		return TRUE;
	}
	void SetEMixerNum(u8 byMixNum)
	{
		m_tEapuCfg.m_byMixerNum = min(MAXNUM_EAPU_MIXER,byMixNum);
	}*/
protected:
	BOOL32	m_bIsRunMixer;    //�Ƿ�����MIXER
	BOOL32	m_bIsRunTVWall;	//�Ƿ�����TVWall
	BOOL32	m_bIsRunBas;		//�Ƿ�����Bas
	BOOL32	m_bIsRunVMP;		//�Ƿ�����VMP
	BOOL32   m_bIsRunPrs;      //�Ƿ�����PRS
	BOOL32  m_bIsRunMpw;      //�Ƿ�����MPw

	u8		m_byMcuId;		//MCU ID

	TEqpMixerEntry	m_tMixerCfg;	//MIXER��������Ϣ
	TEqpTVWallEntry	m_tTVWallCfg;	//TVWALL��������Ϣ
	TEqpBasEntry	m_tBasCfg;		//BAS��������Ϣ
	TEqpVMPEntry	m_tVMPCfg;		//VMP��������Ϣ
	TEqpPrsEntry	m_tPrsCfg;		//Prs��������Ϣ
	TEqpMPWEntry    m_tMpwCfg;
	u8				m_byMAPCount;
	TMAPInfo		m_tMAPCfg[5];	//MAP��������Ϣ
private:
    TEapuCfg        m_tEapuCfg;     //EAPU���ͻ���������
};

#endif    /* MEDIABRDCONFIG_H */
