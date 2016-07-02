/*****************************************************************************
   ģ����      : tvwall
   �ļ���      : tvwall.cpp
   ����ļ�    : tvwall.h
   �ļ�ʵ�ֹ���: ʵ����tvwall(����hdu��tvwall�豸)�Ļ�������
   ����        : ����
   �汾        : V0.9  Copyright(C) 2001-2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2011/06/01  4.6         ����	       ����
******************************************************************************/

#include "tvwall.h"

#include "mcuvc.h"
extern  CMcuVcApp	        g_cMcuVcApp;	//MCUҵ��Ӧ��ʵ��
extern CAgentInterface		g_cMcuAgent;   

/*====================================================================
    ������      ��IsValidHduEqp
    ����        ���ж�tEqp�Ƿ��ǺϷ���Hdu�豸
    �㷨ʵ��    ������������,�������Լ�EqpId�����ж�
    ����ȫ�ֱ�����
    �������˵����[in]tEqp
    ����ֵ˵��  ��TRUE, �Ϸ�; FALSE, ���Ϸ�
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	11/05/26    4.6          ����          ����
====================================================================*/
BOOL32 IsValidHduEqp(const TEqp& tEqp) 
{
	// �������ж�
	if (TYPE_MCUPERI != tEqp.GetType())
	{
		return FALSE;
	}

	// Hdu�����ж�
	/*if (EQP_TYPE_HDU != tEqp.GetEqpType()
			&& EQP_TYPE_HDU_H != tEqp.GetEqpType() 
			&& EQP_TYPE_HDU_L != tEqp.GetEqpType()
			&& EQP_TYPE_HDU2 != tEqp.GetEqpType()
			&& EQP_TYPE_HDU2_L != tEqp.GetEqpType())
	{
		return FALSE;
	}*/

	// Hdu��Id�ж�
	if(HDUID_MIN > tEqp.GetEqpId() || HDUID_MAX < tEqp.GetEqpId())
	{
		return FALSE;
	}
	
	return TRUE;
}

/*====================================================================
    ������      ��IsValidHduMode
    ����        ���ж�Hduģʽ�Ƿ�Ϸ�
    �㷨ʵ��    ������������,�������Լ�EqpId�����ж�
    ����ȫ�ֱ�����
    �������˵����[in]tEqp
    ����ֵ˵��  ��TRUE, �Ϸ�; FALSE, ���Ϸ�
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	2013/03/11   4.7.2       �±�         �޸�(HDU�໭��֧��)
====================================================================*/
BOOL32 IsValidHduMode(const u8 byHduChnMode) 
{
	// �ķ���ж�
	if ( HDUCHN_MODE_FOUR == byHduChnMode)
	{
		return TRUE;
	}
	else if ( HDUCHN_MODE_ONE == byHduChnMode)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*====================================================================
    ������      IsValidHduChn
    ����        ���ж�byHduId�Ƿ��ǺϷ���Hdu�豸��Id
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[in]byHduId, hdu�豸Id
				  [in]byChnnlIdx, hduͨ��
    ����ֵ˵��  ��TRUE, �Ϸ�; FALSE, ���Ϸ�
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	11/05/26    4.6          ����          ����
====================================================================*/
BOOL32 IsValidHduChn(const u8 byHduId, const u8 byChnnlIdx, const u8 bySubChnId) 
{
	TEqp tHduEqp = g_cMcuVcApp.GetEqp( byHduId );
	if (!IsValidHduEqp(tHduEqp))
	{
		return FALSE;
	}

	//hdu ͨ������У��
	u8 byHduChnNum = 0;
	u8 byHduSubType = 0;
	if(g_cMcuAgent.GetHduSubTypeByEqpId(byHduId,byHduSubType))
	{
		switch(byHduSubType)
		{
		case HDU_SUBTYPE_HDU_M:
		case HDU_SUBTYPE_HDU_L:
		case HDU_SUBTYPE_HDU2:
		case HDU_SUBTYPE_HDU2_L:
		case HDU_SUBTYPE_HDU2_S:
			byHduChnNum = MAXNUM_HDU_CHANNEL;
			break;
		case HDU_SUBTYPE_HDU_H:
			byHduChnNum = MAXNUM_HDU_H_CHANNEL;
			break;
		default:
			byHduChnNum = 0;
			LogPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[GetHduChnNumAcd2Type] unexpected hdu subtype :%d!\n", byHduSubType);
			break;
		}
	}

	// [2013/05/08 chenbing] 
	// ��ͨ����Ч��������ж���ͨ���Ƿ���Ч����ͨ����Ч,��ͨ������������ȷ������ٽ�����ͨ���ж� 
	if ( byChnnlIdx >= byHduChnNum )
	{
		if (bySubChnId < HDU_MODEFOUR_MAX_SUBCHNNUM)
		{
			u8 byHduChnId = RESTORECHAN(byChnnlIdx, bySubChnId);
			if ( 0 != byHduChnId && HDU2_PORTSPAN != byHduChnId )
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}


/*====================================================================
    ������      ��IsValidTvwEqp
    ����        ���ж�tEqp�Ƿ��ǺϷ���Tvw�豸
    �㷨ʵ��    ������������,�������Լ�EqpId�����ж�
    ����ȫ�ֱ�����
    �������˵����[in]tEqp
    ����ֵ˵��  ��TRUE, �Ϸ�; FALSE, ���Ϸ�
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	11/05/26    4.6          ����          ����
====================================================================*/
BOOL32 IsValidTvwEqp(const TEqp& tEqp) 
{
	// Hdu�豸Ҳ��Tv wall����
	if (IsValidHduEqp(tEqp))
	{
		return TRUE;
	}

	// �������ж��ǲ�����ͨ��Tv Wall

	// �������ж�
	if (TYPE_MCUPERI != tEqp.GetType())
	{
		return FALSE;
	}

	// Tv Wall�����ж�
	if (EQP_TYPE_TVWALL != tEqp.GetEqpType())
	{
		return FALSE;
	}

	// Tv Wall��Id�ж�
	if(TVWALLID_MIN > tEqp.GetEqpId() || TVWALLID_MAX < tEqp.GetEqpId())
	{
		return FALSE;
	}
	
	return TRUE;
}

/*====================================================================
    ������      : ClearHduChnnlStatus
    ����        �����Hdu�����ĳ��ͨ��״̬��������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[in]byEqpId, Hdu�豸��
				  [in]byChnnlIdx, Hdu�豸ͨ����, ���ֵΪ0xFF, ����ո��豸����ͨ��
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	11/05/25    4.6         liuxu           ����
====================================================================*/
void ClearHduChnnlStatus( const u8 byEqpId, const u8 byChnnlIdx /*= 0xFF*/ )
{
	TPeriEqpStatus tStatus;
	if (!g_cMcuVcApp.GetPeriEqpStatus( byEqpId, &tStatus) )
	{
		return;
	}

	if ( byChnnlIdx != 0xFF ) 
	{
		if (!IsValidHduChn(byEqpId, byChnnlIdx))
		{
			return;
		}
		tStatus.m_tStatus.tHdu.atVideoMt[byChnnlIdx].SetNull();
		tStatus.m_tStatus.tHdu.atVideoMt[byChnnlIdx].SetConfIdx(0);
		tStatus.m_tStatus.tHdu.atHduChnStatus[byChnnlIdx].SetNull();
	}
	else
	{
		for (u8 byLoop = 0; byLoop < MAXNUM_HDU_CHANNEL; ++byLoop)
		{
			tStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
			tStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
			tStatus.m_tStatus.tHdu.atHduChnStatus[byLoop].SetNull();
		}
	}

	g_cMcuVcApp.SetPeriEqpStatus( byEqpId, &tStatus);
}

/*====================================================================
    ������      : ClearTvwChnnlStatus
    ����        �����Tvw�����ĳ��ͨ��״̬��������Ϣ(����Hdu)
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[in]byEqpId, Tvw�豸��
				  [in]byChnnlIdx, Tvw�豸ͨ����, ���ֵΪ0xFF, ����ո��豸����ͨ��
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	11/05/25    4.6         liuxu           ����
====================================================================*/
void ClearTvwChnnlStatus( const u8 byEqpId, const u8 byChnnlIdx /*= 0xFF*/ )
{
	// �����HDU, �����HDUͨ��
	if (IsValidHduChn(byEqpId, byChnnlIdx))
	{
		ClearHduChnnlStatus( byEqpId, byChnnlIdx);
		return;
	}

	// ��ͨ����ǽͨ��
	TPeriEqpStatus tStatus;
	if (!g_cMcuVcApp.GetPeriEqpStatus( byEqpId, &tStatus) )
	{
		return;
	}
	
	if ( byChnnlIdx != 0xFF ) 
	{
		if (!IsValidTvw(byEqpId, byChnnlIdx))
		{
			return;
		}
		
		tStatus.m_tStatus.tTvWall.atVideoMt[byChnnlIdx].byMemberType = 0;
		tStatus.m_tStatus.tTvWall.atVideoMt[byChnnlIdx].SetNull();
		tStatus.m_tStatus.tTvWall.atVideoMt[byChnnlIdx].SetConfIdx( (u8)0 );

	}
	else
	{
		for (u8 byLoop = 0; byLoop < MAXNUM_PERIEQP_CHNNL; ++byLoop)
		{
			tStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = 0;
			tStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
			tStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx( (u8)0 );
		}
	}
	
	g_cMcuVcApp.SetPeriEqpStatus( byEqpId, &tStatus);
}

/*====================================================================
    ������      ��IsValidTvw
    ����        ���ж�byTvwId��byChnnlIdx�Ƿ��ǺϷ���Tvw�豸��Id��ͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[in]byHduId, Tvw�豸Id
				  [in]byChnnlIdx, Tvwͨ��
    ����ֵ˵��  ��TRUE, �Ϸ�; FALSE, ���Ϸ�
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	11/05/26    4.6          ����          ����
====================================================================*/
BOOL32 IsValidTvw(const u8 byTvwId, const u8 byChnnlIdx ) 
{
	// Hdu�豸Ҳ��һ��Tv wall
	if (IsValidHduChn(byTvwId, byChnnlIdx))
	{
		return TRUE;
	}else
	{
		TEqp tTvwEqp = g_cMcuVcApp.GetEqp( byTvwId );
		if (!IsValidTvwEqp(tTvwEqp))
		{
			return FALSE;
		}

		if (!IsValidTvwChnnl(byChnnlIdx))
		{
			return FALSE;
		}

		return TRUE;
	}
}

/*====================================================================
    ������      ��IsValidTvw
    ����        ���ж�byTvwId��byChnnlIdx�Ƿ��ǺϷ���Tvw�豸��Id��ͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����[in]byHduId, Tvw�豸Id
				  [in]byChnnlIdx, Tvwͨ��
    ����ֵ˵��  ��TRUE, �Ϸ�; FALSE, ���Ϸ�
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	11/05/26    4.6          ����          ����
====================================================================*/
const char* const GetHduTypeStr( const TEqp& tHdu )
{
	const char* const HDU_STR = "hdu";
	const char* const HDU_L_STR = "hdu_l";
	const char* const HDU_H_STR = "hdu_h";
	const char* const HDU2_STR = "hdu2";
	const char* const HDU2_L_STR = "hdu2_l";
	const char* const HDU2_S_STR = "hdu2_s";
	
	if (!IsValidHduEqp(tHdu))
	{
		return NULL;
	}

	u8 byHduSubType = 0;
	if(g_cMcuAgent.GetHduSubTypeByEqpId(tHdu.GetEqpId(),byHduSubType))
	{
		switch (byHduSubType)
		{
		case HDU_SUBTYPE_HDU_H:
			return HDU_H_STR;
			
		case HDU_SUBTYPE_HDU_L:
			return HDU_L_STR;
			
		case HDU_SUBTYPE_HDU2:
			return HDU2_STR;
			
		case HDU_SUBTYPE_HDU2_L:
			return HDU2_L_STR;

		case HDU_SUBTYPE_HDU2_S:
			return HDU2_S_STR;

		default:
			return HDU_STR;
		}
	}
	else
	{
		return NULL;
	}
}

/*====================================================================
������      ��Clear
����        �����Tvwall
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2011/05/25  4.0         ��  ��         ����
====================================================================*/
void CTvwChnnl::Clear()
{
	memset(this, 0, sizeof(CTvwChnnl)); 
	m_tMember.SetNull();
}

/*====================================================================
������      ��GetTvwEqp
����        ����ȡͨ�����ڵ�TEqp����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2011/05/25  4.0         ��  ��         ����
====================================================================*/
TEqp CTvwChnnl::GetTvwEqp()
{
	TEqp tRet;
	tRet.SetNull();

	if ( !IsValidTvw( GetEqpId(), GetChnnlIdx() ))
	{
		return tRet;
	}

	return g_cMcuVcApp.GetEqp( GetEqpId() ); 
}

/*====================================================================
������      ��operator==
����        �����ƺ���
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2011/05/25  4.0         ��  ��         ����
====================================================================*/
const CTvwChnnl& CTvwChnnl::operator== ( const CTvwChnnl& cOtherTvw )
{
	// ��ֹ���Ҹ���
	if ( this == &cOtherTvw )
	{
		return *this;
	}

	memcpy(this, &cOtherTvw, sizeof(CTvwChnnl));
	return *this;
}

/*====================================================================
������      ��operator==
����        �����ƺ���
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2011/05/25  4.0         ��  ��         ����
====================================================================*/
const CConfTvwChnnl& CConfTvwChnnl::operator== ( const CConfTvwChnnl& cOtherTvw)
{
	// ��ֹ���Ҹ���
	if ( this == &cOtherTvw )
	{
		return *this;
	}
	
	memcpy(this, &cOtherTvw, sizeof(CConfTvwChnnl));
	return *this;
}

/************************************************************************/
/* CTvwEqp                                                              */
/************************************************************************/


/*====================================================================
������      ��CTvwEqp
����        �����캯��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2011/05/25  4.0         ��  ��         ����
====================================================================*/
CTvwEqp::CTvwEqp()
{
	// ��ȫ����0
	memset(this, 0, sizeof(CTvwEqp));
	
	// ������������
	this->SetType( TYPE_MCUPERI );
}

/*====================================================================
������      ��GetChnnlNum
����        ����ȡͨ������
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2011/05/25  4.0         ��  ��         ����
====================================================================*/
// u8 CTvwEqp::GetChnnlNum()
// {
// 	return 
// }