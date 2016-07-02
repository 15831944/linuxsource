/*****************************************************************************
   ģ����      : mcu
   �ļ���      : mcuutility.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: MCUҵ���ڲ�ʹ�ýṹ�����ʵ��
   ����        : ������
   �汾        : V3.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/11/7   3.0         ������      ����
   2005/02/19  3.6         ����      �����޸ġ���3.5�汾�ϲ�
******************************************************************************/

#include "mcuvc.h"
#include "evmcu.h"
#include "mcuver.h"
#include "evmcumcs.h"
#include "mcuutility.h"
#include "eqplib.h"
#include "bindmp.h"
#include "bindmtadp.h"
#include "mcupfmlmt.h"
#include "satconst.h"

#include<sys/types.h>   
#include<sys/stat.h>   


SEMHANDLE g_hConfInfoRW = NULL;

//#define SAME_RECVMTPORT_VER
BOOL32 ReadMAPConfig( TMAPParam tMAPParam[], u8 byMapCount );

// Windows 8000B MCU �ӿ�
/*
#ifdef WIN32
#ifdef _MINIMCU_
BOOL32 mixInit (TAudioMixerCfg* tAudioMixerCfg)
{
    return TRUE;
}
BOOL32 vmpinit (TVmpCfg * ptVmpCfg)
{
    return TRUE;
}
BOOL32 basInit (TEqpCfg* ptCfg)
{
    return TRUE;
}

#endif
#endif
*/


/*--------------------------------------------------------------------------*/
/*                                TConfPollParam                            */
/*                               ������ѯ������Ϣ                           */
/*--------------------------------------------------------------------------*/

/*====================================================================
    ������      ��InitPollParam
    ����        ������Ҫ����ѯ���ն��б���������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/04/04    4.0         ����         ����
====================================================================*/
void TConfPollParam::InitPollParam(u8 byMtNum, TMtPollParam* ptParam)
{
    SetPollList(byMtNum, ptParam) ;

    m_byCurPollPos = 0;
    ClearSpecPos();
}

/*====================================================================
    ������      SetPollList
    ����        ������Ҫ����ѯ���ն��б�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/04/04    4.0         ����         ����
====================================================================*/
void TConfPollParam::SetPollList(u8 byMtNum, TMtPollParam* ptParam)
{
    if ( ptParam == NULL && byMtNum != 0)
    {
        return ;
    }
    m_byPollMtNum = byMtNum > MAXNUM_CONF_MT ? MAXNUM_CONF_MT : byMtNum;
    if ( m_byPollMtNum > 0 )
    {
        memcpy( m_atMtPollParam, ptParam, m_byPollMtNum * sizeof(TMtPollParam) );
    }    
}

/*====================================================================
    ������      ��GetPolledMtNum
    ����        ����ȡ��ѯ�б��ն�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/04/04    4.0         ����         ����
====================================================================*/
u8 TConfPollParam::GetPolledMtNum() const
{
    return m_byPollMtNum;
}

/*====================================================================
    ������      ��GetPollMtByIdx
    ����        ����ȡ��ѯ�б���ָ��λ�õ��ն˲���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/04/04    4.0         ����         ����
====================================================================*/
TMtPollParam* const TConfPollParam::GetPollMtByIdx(u8 byIdx)
{
    if ( byIdx < m_byPollMtNum )
    {
        return &m_atMtPollParam[byIdx];
    }
    else
    {
        return NULL;
    }    
}

/*====================================================================
    ������      ��SetCurrentIdx
    ����        �����õ�ǰ��ѯ�ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/04/04    4.0         ����         ����
====================================================================*/
void TConfPollParam::SetCurrentIdx(u8 byIdx)
{
    if ( byIdx < m_byPollMtNum )
    {
        m_byCurPollPos = byIdx;
    }
}


/*====================================================================
    ������      ��GetCurrentIdx
    ����        ����ȡ��ǰ����ѯ�ն�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/04/04    4.0         ����         ����
====================================================================*/
u8 TConfPollParam::GetCurrentIdx() const
{
    return m_byCurPollPos;
}

/*====================================================================
    ������      ��GetCurrentMtPolled
    ����        ����ȡ��ǰ����ѯ�ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/04/04    4.0         ����         ����
====================================================================*/
TMtPollParam* const TConfPollParam::GetCurrentMtPolled()
{
    return &m_atMtPollParam[m_byCurPollPos];
}

/*====================================================================
    ������      ��IsExistMt
    ����        ������ѯ�б��в���ָ���ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/04/04    4.0         ����         ����
====================================================================*/
BOOL32 TConfPollParam::IsExistMt(const TMt &tMt, u8 &byIdx)
{
    byIdx = 0xFF;
    u8 byPos = 0;
    for( ; byPos < m_byPollMtNum; byPos++ )
    {
        // guzh [4/5/2007] ������ô�ж�����Ϊ��ص������������ܲ���ȷ
        if ( tMt.GetMcuId() == m_atMtPollParam[byPos].GetMcuId() && 
             tMt.GetMtId() == m_atMtPollParam[byPos].GetMtId() )
        {
            byIdx = byPos;
            return TRUE;
        }
    }

    return FALSE;
}

/*====================================================================
    ������      ��RemoveMtFromList
    ����        ������ѯ�б����Ƴ�ָ���ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/04/04    4.0         ����         ����
====================================================================*/
void TConfPollParam::RemoveMtFromList(const TMt &tMt)
{
    u8 byPos = 0;
    for( ; byPos < m_byPollMtNum; byPos++ )
    {
        if ( tMt == m_atMtPollParam[byPos] )
        {            
            // �ƶ��б�
            for( ; byPos < m_byPollMtNum-1; byPos++ )
            {
                m_atMtPollParam[byPos] = m_atMtPollParam[byPos+1];
            }
            m_byPollMtNum--;
            break;
        }
    }
}

/*--------------------------------------------------------------------------*/
/*                              TTvWallPollParam                            */
/*                             ����ǽ��ѯ������Ϣ                           */
/*--------------------------------------------------------------------------*/

/*=============================================================================
�� �� ���� GetTvWallEqp
��    �ܣ� ��õ���ǽ�豸
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� TEqp* const 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/4/9  4.0			�ܹ��                  ����
=============================================================================*/
TEqp TTvWallPollParam::GetTvWallEqp( void )
{
	return m_tTvWall;
}

/*=============================================================================
�� �� ���� SetTvWallEqp
��    �ܣ� ���õ���ǽ�豸
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TEqp &tTvWall
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/4/9  4.0			�ܹ��                  ����
=============================================================================*/
void TTvWallPollParam::SetTvWallEqp( const TEqp &tTvWall )
{
	m_tTvWall = tTvWall;
}

/*=============================================================================
�� �� ���� GetTvWallChnnl
��    �ܣ� ��õ���ǽͨ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� u8 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/4/9  4.0			�ܹ��                  ����
=============================================================================*/
u8 TTvWallPollParam::GetTvWallChnnl( void )
{
	return m_byTvWallChnnl;
}

/*=============================================================================
�� �� ���� SetTvWallChnnl
��    �ܣ� ���õ���ǽͨ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byChnIndex
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/4/9  4.0			�ܹ��                  ����
=============================================================================*/
void TTvWallPollParam::SetTvWallChnnl( u8 byChnIndex )
{
	m_byTvWallChnnl = byChnIndex;
}

/*--------------------------------------------------------------------------*/
/*                                TConfMtTable                              */
/*--------------------------------------------------------------------------*/

/*====================================================================
    ������      ��TConfMtTable
    ����        �����캯��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������        ����
  2008/05/06	4.5			�ű���		  ɾ��: g++�������Ϊ��ʵ������������๹���壬�����쳣
====================================================================*/
/*
TConfMtTable::TConfMtTable( void )
{
    m_byMaxNumInUse = 0;
	memset( m_atMtExt, 0, sizeof( m_atMtExt ) );
	memset( m_atMtData, 0, sizeof( m_atMtData ) );	
}*/


/*====================================================================
    ������      ��SetMtLogicChnnl
    ����        ������MT���߼�ͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
				  u8 byMediaType, ý������
				  TLogicalChannel * ptChnnl, �߼�ͨ������
				  BOOL32 bForwardChnnl, ����
    �������˵��:		��
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/04/06    1.0         ���         ����
====================================================================*/
void TConfMtTable::SetMtLogicChnnl( u8 byMtId, u8 byChannel, const TLogicalChannel * ptChnnl, BOOL32 bForwardChnnl )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptChnnl == NULL )
	{
		return;
	}
	
	switch( byChannel )
	{
	case LOGCHL_VIDEO:
		if( bForwardChnnl )
		{
			m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel = *ptChnnl;
		}
		else
		{
			m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel = *ptChnnl;			
		}
		break;

	case LOGCHL_SECVIDEO:
		if( bForwardChnnl )
		{
			m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel = *ptChnnl;
		}
		else
		{
			m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel = *ptChnnl;			
		}
		break;

	case LOGCHL_AUDIO:
		if( bForwardChnnl )
		{
			m_atMtData[byMtId - 1].m_tFwdChannel.m_tAudioChannel = *ptChnnl;
		}
		else
		{
			m_atMtData[byMtId - 1].m_tRvsChannel.m_tAudioChannel = *ptChnnl;			
		}
		break;

	case LOGCHL_T120DATA:
		if( bForwardChnnl )
		{
			m_atMtData[byMtId - 1].m_tFwdChannel.m_tT120DataChannel = *ptChnnl;
		}
		else
		{
			m_atMtData[byMtId - 1].m_tRvsChannel.m_tT120DataChannel = *ptChnnl;			
		}
		break;

	case LOGCHL_H224DATA:
		if( bForwardChnnl )
		{
			m_atMtData[byMtId - 1].m_tFwdChannel.m_tH224DataChannel = *ptChnnl;
		}
		else
		{
			m_atMtData[byMtId - 1].m_tRvsChannel.m_tH224DataChannel = *ptChnnl;			
		}
		break;
	case LOGCHL_MMCUDATA:
		if( bForwardChnnl )
		{
			m_atMtData[byMtId - 1].m_tFwdChannel.m_tMmcuDataChannel = *ptChnnl;
		}
		else
		{
			m_atMtData[byMtId - 1].m_tRvsChannel.m_tMmcuDataChannel = *ptChnnl;			
		}
		break;

	default:
		OspPrintf( TRUE, FALSE, "Exception - TConfMtData::SetMtLogicChnnl(): wrong byChannel %u!\n", byChannel );
		break;
	}

	return;
}

/*====================================================================
    ������      ��GetMtLogicChnnl
    ����        ���õ�MT���߼�ͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
				  u8 byMediaType, ý������
				  TLogicalChannel * ptChnnl, �߼�ͨ������
				  BOOL32 bForwardChnnl, ����
    �������˵��:		��
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
BOOL32 TConfMtTable::GetMtLogicChnnl( u8 byMtId, u8 byChannel, TLogicalChannel * ptChnnl, BOOL32 bForwardChnnl )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptChnnl == NULL )
	{
		return FALSE;
	}
	
	switch( byChannel )
	{
	case LOGCHL_VIDEO:
		if( bForwardChnnl )
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel;
		}
		else
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel;			
		}
		break;

	case LOGCHL_SECVIDEO:
		if( bForwardChnnl )
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel;
		}
		else
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel;			
		}
		break;

	case LOGCHL_AUDIO:
		if( bForwardChnnl )
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tFwdChannel.m_tAudioChannel;
		}
		else
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tRvsChannel.m_tAudioChannel;			
		}
		break;

	case LOGCHL_T120DATA:
		if( bForwardChnnl )
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tFwdChannel.m_tT120DataChannel;
		}
		else
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tRvsChannel.m_tT120DataChannel;			
		}
		break;

	case LOGCHL_H224DATA:
		if( bForwardChnnl )
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tFwdChannel.m_tH224DataChannel;
		}
		else
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tRvsChannel.m_tH224DataChannel;			
		}
		break;

	case LOGCHL_MMCUDATA:
		if( bForwardChnnl )
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tFwdChannel.m_tMmcuDataChannel;
		}
		else
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tRvsChannel.m_tMmcuDataChannel;			
		}
		break;

	default:
		OspPrintf( TRUE, FALSE, "Exception - TConfMtTable::GetMtLogicChnnl(): wrong byChannel %u!\n", byChannel );
		return FALSE;
		break;
	}

	return ptChnnl->IsOpened();
}

/*====================================================================
    ������      ��SetMtSwitchAddr
    ����        �������ն˵Ľ�����ַ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    			  u32 dwRcvIp, ����Ip
    			  u16 wRcvPort, ���ն˿�
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/02    2.0         ������         ����
====================================================================*/
void TConfMtTable::SetMtSwitchAddr(u8 byMtId, u32 dwRcvIp, u16 wRcvPort)
{
    if (byMtId == 0 || dwRcvIp == 0 || wRcvPort == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return;
    }

    m_atMtData[byMtId - 1].m_tSwitchAddr.SetIpAddr(dwRcvIp);
    m_atMtData[byMtId - 1].m_tSwitchAddr.SetPort(wRcvPort);

    return;
}

/*====================================================================
    ������      ��GetMtSwitchAddr
    ����        ���õ��ն˵Ľ�����ַ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    			  u32 &dwRcvIp, ����Ip
    			  u16 &wRcvPort, ���ն˿�
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/02    2.0         ������         ����
====================================================================*/
BOOL32 TConfMtTable::GetMtSwitchAddr(u8 byMtId, u32 &dwRcvIp, u16 &wRcvPort)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return FALSE;
    }

    dwRcvIp  = m_atMtData[byMtId - 1].m_tSwitchAddr.GetIpAddr();
    wRcvPort = m_atMtData[byMtId - 1].m_tSwitchAddr.GetPort();

    return (dwRcvIp == 0 || wRcvPort == 0) ? FALSE : TRUE;
}


/*=============================================================================
�� �� ���� ClearMtSwitchAddr
��    �ܣ� ����ն˽�����ַ��Ϣ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byMtId
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/10/11  4.0			������                  ����
=============================================================================*/
void TConfMtTable::ClearMtSwitchAddr(u8 byMtId)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return;
    }

    m_atMtData[byMtId - 1].m_tSwitchAddr.SetIpAddr(0);
    m_atMtData[byMtId - 1].m_tSwitchAddr.SetPort(0);

    return;
}

/*=============================================================================
  �� �� ���� SetMtDcsAddr
  ��    �ܣ� �����ն˵�DCS��ַ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u8 byMtId
             u32 dwDstIp
             u16 wDstPort
             u8 byMode
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/1/10    4.0			�ű���                  ����
=============================================================================*/
void TConfMtTable::SetMtDcsAddr( u8 byMtId, u32 dwDstIp, u16 wDstPort, u8 byMode )
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return;
    }
	if (MODE_DATA == byMode)
    {
        m_atMtData[byMtId - 1].m_tDcsAddr.SetIpAddr(dwDstIp);
        m_atMtData[byMtId - 1].m_tDcsAddr.SetPort(wDstPort);
    }
	return;
}

/*=============================================================================
  �� �� ���� GetMtDcsAddr
  ��    �ܣ� ����ն˵�DCS��ַ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u8 byMtId
             u32 &dwDstIp
             u16 &wDstPort
             u8 byMode
  �� �� ֵ�� BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/1/10    4.0			�ű���                  ����
=============================================================================*/
BOOL32 TConfMtTable::GetMtDcsAddr( u8 byMtId, u32 &dwDstIp, u16 &wDstPort, u8 byMode )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return FALSE;
    }
	
    if (MODE_DATA == byMode)
    {
        dwDstIp = m_atMtData[byMtId - 1].m_tDcsAddr.GetIpAddr();
        wDstPort = m_atMtData[byMtId - 1].m_tDcsAddr.GetPort();
    }

    return (dwDstIp == 0 || wDstPort == 0) ? FALSE : TRUE;
}

/*=============================================================================
  �� �� ���� SetMtBRBeLowed
  ��    �ܣ� ���õ�ǰ���ն��Ƿ񱻽���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8     byMtId
             BOOL32 bLowed
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2008/1/31     4.0			�ű���                  ����
=============================================================================*/
void TConfMtTable::SetMtBRBeLowed( u8 byMtId, BOOL32 bLowed )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return;
    }
    m_atMtData[byMtId-1].m_bBandWidthLowed = bLowed ? 1 : 0;
    return;
}

/*=============================================================================
  �� �� ���� GetMtBRBeLowed
  ��    �ܣ� ��ȡ��ǰ���ն��Ƿ񱻽���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byMtId
  �� �� ֵ�� BOOL32
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2008/1/31     4.0			�ű���                  ����
=============================================================================*/
BOOL32 TConfMtTable::GetMtBRBeLowed( u8 byMtId )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return FALSE;
    }
    return m_atMtData[byMtId-1].m_bBandWidthLowed == 1 ? TRUE : FALSE;
}


/*=============================================================================
  �� �� ���� SetMtTransE1
  ��    �ܣ� �����ն���·���䷽ʽ�Ƿ�ΪE1����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8     byMtId
             BOOL32 bTransE1
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2008/1/31     4.0			�ű���                  ����
=============================================================================*/
void TConfMtTable::SetMtTransE1( u8 byMtId, BOOL32 bTransE1 )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return;
    }
    m_atMtData[byMtId-1].m_bMtE1 = bTransE1 ? 1 : 0;
    return;
}

/*=============================================================================
  �� �� ���� GetMtTransE1
  ��    �ܣ� ��ȡ�ն˵Ĵ�����·�Ƿ�ΪE1����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byMtId
  �� �� ֵ�� BOOL32
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2008/1/31     4.0			�ű���                  ����
=============================================================================*/
BOOL32 TConfMtTable::GetMtTransE1( u8 byMtId )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return FALSE;
    }
    return m_atMtData[byMtId-1].m_bMtE1 == 1 ? TRUE : FALSE;
}


/*=============================================================================
  �� �� ���� SetLowedRcvBandWidth
  ��    �ܣ� �����ն˽����Ժ���ܴ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8  byMtId
             u16 wLowedBandWidth
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2008/1/31     4.0			�ű���                  ����
=============================================================================*/
void TConfMtTable::SetLowedRcvBandWidth( u8 byMtId, u16 wLowedBandWidth )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return;
    }
    m_atMtData[byMtId-1].m_wLowedRcvBandWidth = wLowedBandWidth;
    return;
}

/*=============================================================================
  �� �� ���� GetMtTransE1
  ��    �ܣ� ��ȡ�ն˽����Ժ���ܴ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byMtId
  �� �� ֵ�� BOOL32
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2008/1/31     4.0			�ű���                  ����
=============================================================================*/
u16 TConfMtTable::GetLowedRcvBandWidth( u8 byMtId )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return 0;
    }
    return m_atMtData[byMtId-1].m_wLowedRcvBandWidth;
}

/*=============================================================================
  �� �� ���� SetRcvBandAdjusted
  ��    �ܣ� ����E1�ն��Ƿ��Ѿ���������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8  byMtId
             u16 bAjusted
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2008/9/9     4.0			�ű���                  ����
=============================================================================*/
void TConfMtTable::SetRcvBandAdjusted(u8 byMtId, BOOL32 bAjusted)
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return;
    }
    m_atMtData[byMtId-1].m_bRcvBandAjusted = bAjusted;
    return;
}

/*=============================================================================
  �� �� ���� GetRcvBandAdjusted
  ��    �ܣ� E1�ն��Ƿ��Ѿ���������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8  byMtId
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2008/9/9     4.0			�ű���                  ����
=============================================================================*/
BOOL32 TConfMtTable::GetRcvBandAdjusted(u8 byMtId)
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return FALSE;
    }
	return m_atMtData[byMtId-1].m_bRcvBandAjusted;
}

void TConfMtTable::SetMtSigned(u8 byMtId, BOOL32 bSigned)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return;
    }
    m_atMtData[byMtId-1].m_bMtSigned = bSigned;
    return;
}

BOOL32 TConfMtTable::IsMtSigned(u8 byMtId)
{
    return m_atMtData[byMtId-1].m_bMtSigned;
}


void TConfMtTable::SetMtCurrUpLoad(u8 byMtId, BOOL32 bUpLoad)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return;
    }
    m_atMtData[byMtId-1].m_bCurrUpLoad = bUpLoad;
    return;
}

BOOL32 TConfMtTable::IsMtCurrUpLoad(u8 byMtId)
{
    return m_atMtData[byMtId-1].m_bCurrUpLoad;
}

/*====================================================================
    ������      ��SetMtRtcpDstAddr
    ����        �������ն˵���ƵRtcpĿ�ĵ�ַ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    			  u32 dwRcvIp, ����Ip
    			  u16 wRcvPort, ���ն˿�
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/10/19    3.0         ������         ����
====================================================================*/
void TConfMtTable::SetMtRtcpDstAddr(u8 byMtId, u32 dwDstIp, u16 wDstPort, u8 byMode)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return;
    }

    if (MODE_VIDEO == byMode)
    {
        m_atMtData[byMtId - 1].m_tRtcpDstAddr.SetIpAddr(dwDstIp);
        m_atMtData[byMtId - 1].m_tRtcpDstAddr.SetPort(wDstPort);
    }
    else if (MODE_AUDIO == byMode)
    {
        m_atMtData[byMtId - 1].m_tAudRtcpDstAddr.SetIpAddr(dwDstIp);
        m_atMtData[byMtId - 1].m_tAudRtcpDstAddr.SetPort(wDstPort);
    }

	return;
}

/*====================================================================
    ������      ��GetMtRtcpDstAddr
    ����        ���õ��ն˵���ƵRtcpĿ�ĵ�ַ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    			  u32 &dwRcvIp, ����Ip
    			  u16 &wRcvPort, ���ն˿�
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
     04/10/19    3.0         ������         ����
====================================================================*/
BOOL32 TConfMtTable::GetMtRtcpDstAddr(u8 byMtId, u32 &dwDstIp, u16 &wDstPort, u8 byMode)
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return FALSE;
    }

    if (MODE_VIDEO == byMode)
    {
        dwDstIp = m_atMtData[byMtId - 1].m_tRtcpDstAddr.GetIpAddr();
        wDstPort = m_atMtData[byMtId - 1].m_tRtcpDstAddr.GetPort();
    }
    else if (MODE_AUDIO == byMode)
    {
        dwDstIp = m_atMtData[byMtId - 1].m_tAudRtcpDstAddr.GetIpAddr();
        wDstPort = m_atMtData[byMtId - 1].m_tAudRtcpDstAddr.GetPort();
    }

    return (dwDstIp == 0 || wDstPort == 0) ? FALSE : TRUE;
}

/*====================================================================
    ������      ��GetSrcSCS
    ����        ���õ��ն˵�Դͬʱ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/18    2.0         ������         ����
====================================================================*/
TSimCapSet TConfMtTable::GetSrcSCS( u8 byMtId )
{
	TSimCapSet tSimCapSet;
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return tSimCapSet;
	}

	if( m_atMtData[byMtId - 1].m_tRvsChannel.m_tAudioChannel.IsOpened() )
	{
		tSimCapSet.SetAudioMediaType( m_atMtData[byMtId - 1].m_tRvsChannel.m_tAudioChannel.GetChannelType() );
	}

	if( m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel.IsOpened() )
	{
		tSimCapSet.SetVideoMediaType( m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel.GetChannelType() );
        tSimCapSet.SetVideoResolution( m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel.GetVideoFormat() );
	}

	return tSimCapSet;
}

/*====================================================================
    ������      ��GetSrcSCS
    ����        ���õ��ն˵�Ŀ��ͬʱ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/18    2.0         ������         ����
====================================================================*/
TSimCapSet TConfMtTable::GetDstSCS( u8 byMtId )
{
	TSimCapSet tSimCapSet;
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return tSimCapSet;
	}

	if( m_atMtData[byMtId - 1].m_tFwdChannel.m_tAudioChannel.IsOpened() )
	{
		tSimCapSet.SetAudioMediaType( m_atMtData[byMtId - 1].m_tFwdChannel.m_tAudioChannel.GetChannelType() );
	}

	if( m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel.IsOpened() )
	{
		tSimCapSet.SetVideoMediaType( m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel.GetChannelType() );
        tSimCapSet.SetVideoResolution( m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel.GetVideoFormat() );
	}

	return tSimCapSet;
}

/*====================================================================
    ������      ��ClearMtLogicChnnl
    ����        �����MT���߼�ͨ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    			  u8 byMediaType, ý������
    			  BOOL32 bForwardChnnl, ����
 
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/06/04    1.0         JQL         ����
====================================================================*/
void TConfMtTable::ClearMtLogicChnnl( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT  )
	{
		return;
	}

	TLogicalChannel tLogicalChannel;
	m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel = tLogicalChannel;
	m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel = tLogicalChannel;			
	m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel = tLogicalChannel;
	m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel = tLogicalChannel;			
	m_atMtData[byMtId - 1].m_tFwdChannel.m_tAudioChannel = tLogicalChannel;
	m_atMtData[byMtId - 1].m_tRvsChannel.m_tAudioChannel = tLogicalChannel;			
	m_atMtData[byMtId - 1].m_tFwdChannel.m_tT120DataChannel = tLogicalChannel;
	m_atMtData[byMtId - 1].m_tRvsChannel.m_tT120DataChannel = tLogicalChannel;			
	m_atMtData[byMtId - 1].m_tFwdChannel.m_tH224DataChannel = tLogicalChannel;
	m_atMtData[byMtId - 1].m_tRvsChannel.m_tH224DataChannel = tLogicalChannel;			

	return;
}

/*====================================================================
    ������      ��IsLogicChnnlOpen
    ����        ���ж��߼�ͨ���Ƿ񱻴�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
				  u8 byChannel, ֪ͨ����
				  BOOL32 bForwardChnnl Դ����Ŀ��
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
BOOL32 TConfMtTable::IsLogicChnnlOpen( u8 byMtId, u8 byChannel, BOOL32 bForwardChnnl )
{
	switch( byChannel )
	{
	case LOGCHL_VIDEO:
		return 	bForwardChnnl ? m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel.IsOpened() 
			                  : m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel.IsOpened();
		break;

	case LOGCHL_SECVIDEO:
		return 	bForwardChnnl ? m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel.IsOpened()
			                  : m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel.IsOpened();
		break;

	case LOGCHL_AUDIO:
		return 	bForwardChnnl ? m_atMtData[byMtId - 1].m_tFwdChannel.m_tAudioChannel.IsOpened()
			                  : m_atMtData[byMtId - 1].m_tRvsChannel.m_tAudioChannel.IsOpened();
		break;

	case LOGCHL_T120DATA:
		return 	bForwardChnnl ? m_atMtData[byMtId - 1].m_tFwdChannel.m_tT120DataChannel.IsOpened()
			                  : m_atMtData[byMtId - 1].m_tRvsChannel.m_tT120DataChannel.IsOpened();
		break;

	case LOGCHL_H224DATA:
		return 	bForwardChnnl ? m_atMtData[byMtId - 1].m_tFwdChannel.m_tH224DataChannel.IsOpened()
			                  : m_atMtData[byMtId - 1].m_tRvsChannel.m_tH224DataChannel.IsOpened();
		break;
		
	default:
		return FALSE;
		break;
	}

	return FALSE;
}

/*====================================================================
    ������      ��SetMtSrc
    ����        ������MT�ķ���/������Ƶ����ƵԴ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˺�
				  TMt * ptSrc �ն�Դ
				  u8 byMode ͨ������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
void TConfMtTable::SetMtSrc( u8 byMtId, const TMt * ptSrc, u8 byMode )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || NULL == ptSrc )
	{
		return;
	}
	
	if( MODE_VIDEO==byMode || MODE_BOTH == byMode )
	{
		m_atMtData[byMtId - 1].m_tSelVSrc = *ptSrc;
		m_atMtData[byMtId - 1].m_tMtStatus.SetVideoMt(*ptSrc);
	}
	
	if( MODE_AUDIO == byMode || MODE_BOTH == byMode )
	{
		m_atMtData[byMtId - 1].m_tSelASrc = *ptSrc;
		m_atMtData[byMtId - 1].m_tMtStatus.SetAudioMt(*ptSrc);
	}
	
	return;
}

/*====================================================================
    ������      ��GetMtSrc
    ����        ���õ�MT�ķ���/������Ƶ����ƵԴ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˺�
				  TMt * ptSrc �ն�Դ
				  u8 byMode ͨ������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
BOOL32 TConfMtTable::GetMtSrc( u8 byMtId, TMt * ptSrc, u8 byMode )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || NULL == ptSrc )
	{
		return FALSE;
	}
	
	if( MODE_VIDEO==byMode )
	{
		*ptSrc = m_atMtData[byMtId - 1].m_tSelVSrc;
	}
	else if( MODE_AUDIO==byMode )
	{
		*ptSrc = m_atMtData[byMtId - 1].m_tSelASrc;
	}
	else
	{
		return FALSE;
	}
	
	return TRUE;
}

/*====================================================================
    ������      ��SetMtStatus
    ����        ������MT״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˺�
				  const TMtStatus * ptStatus �ն�״̬
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
void TConfMtTable::SetMtStatus( u8 byMtId, const TMtStatus * ptStatus )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptStatus == NULL )
	{
		return;
	}
	
	m_atMtData[byMtId - 1].m_tMtStatus = *ptStatus;

	return;	
}

/*====================================================================
    ������      ��GetMtStatus
    ����        ���õ��ն�״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
				  TMtStatus * ptStatus, �ն�״̬
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
BOOL32 TConfMtTable::GetMtStatus( u8 byMtId, TMtStatus * ptStatus )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptStatus == NULL )
	{
		return FALSE;
	}
	
	*ptStatus = m_atMtData[byMtId - 1].m_tMtStatus;
	return TRUE;
}

/*====================================================================
    ������      ��SetMtCapSupport
    ����        �������ն�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
				  const TCapSupport * ptCapSupport, �ն�������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
void TConfMtTable::SetMtCapSupport( u8 byMtId, const TCapSupport * ptCapSupport )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptCapSupport == NULL )
	{
		return ;
	}
    
	m_atMtData[byMtId - 1].m_tCapSupport = *ptCapSupport;

	return;
}


/*====================================================================
    ������      ��GetMtCapSupport
    ����        ���õ��ն˵�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
				  TCapSupport * ptCapSupport, �ն�������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
BOOL32 TConfMtTable::GetMtCapSupport( u8 byMtId, TCapSupport * ptCapSupport )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptCapSupport == NULL )
	{
		return FALSE;
	}
	
	*ptCapSupport = m_atMtData[byMtId - 1].m_tCapSupport;
	return TRUE;
}

/*====================================================================
    ������      ��SetMtCapSupportEx
    ����        ����ȡ�ն˵���չ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
				  TCapSupportEx * ptCapSupport, �ն���չ������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/10/13    4.0         �ű���         ����
====================================================================*/
void TConfMtTable::SetMtCapSupportEx( u8 byMtId, const TCapSupportEx * ptCapSupport )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptCapSupport == NULL )
    {
        return ;
    }
    
    m_atMtData[byMtId - 1].m_tCapSupportEx = *ptCapSupport;
    
    return;
}

/*====================================================================
    ������      ��GetMtCapSupportEx
    ����        ���õ��ն˵���չ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
				  TCapSupportEx * ptCapSupport, �ն���չ������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/10/13    4.0         �ű���         ����
====================================================================*/
BOOL32 TConfMtTable::GetMtCapSupportEx( u8 byMtId, TCapSupportEx * ptCapSupport )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptCapSupport == NULL )
    {
        return FALSE;
    }
    
    *ptCapSupport = m_atMtData[byMtId - 1].m_tCapSupportEx;
    return TRUE;
}
/*====================================================================
    ������      ��SetMtCapSpecByMCS
    ����        �������ն��������Ƿ��ɻ�ظ�Ԥ�趨
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId           �ն˺�
	              u8 bySpecByMCS      TRUE: MCS��Ԥ�ն�������
				                      FALSE:���ն��Լ�ָ��
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/10/30    3.0         ���㻪         ����
====================================================================*/
void TConfMtTable::SetMtCapSpecByMCS( u8 byMtId, u8 bySpecByMCS)
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT)
	{
		return ;
	}
    
	m_atMtData[byMtId - 1].m_byMTCapSpecByMCS = bySpecByMCS;

	return;
}
/*====================================================================
    ������      ��IsMtCapSpecByMCS
    ����        ���ն��������Ƿ��ɻ����ָ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId           �ն˺�
    ����ֵ˵��  ��u8 TRUE: �ɻ��ָ��
	                 FALSE:���ն��Լ�ָ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08/10/30    3.0         ���㻪         ����
====================================================================*/
u8 TConfMtTable::IsMtCapSpecByMCS( u8 byMtId)
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT)
	{
		return FALSE;
	}
    
	return m_atMtData[byMtId - 1].m_byMTCapSpecByMCS;

}
/*====================================================================
    ������      ��SetMtAlias
    ����        �������ն˱���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
				  const TMtAlias * ptAlias, �ն˱���
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
BOOL32 TConfMtTable::SetMtAlias( u8 byMtId, const TMtAlias * ptAlias )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptAlias == NULL )
	{
		return FALSE;
	}

	if( ptAlias->m_AliasType == 0 || ptAlias->m_AliasType > mtAliasTypeOthers)
	{
		return FALSE;
	}

	m_atMtData[byMtId - 1].m_atMtAlias[ptAlias->m_AliasType - 1] = *ptAlias;

	TMtAlias tMtAlias;
	if(!GetMtAlias( byMtId, mtAliasTypeH320Alias, &tMtAlias ))//TMtEx��H320Alias������H320Id
	{
        if(!GetMtAlias( byMtId, mtAliasTypeH320ID, &tMtAlias ))
        {
            if(!GetMtAlias( byMtId, mtAliasTypeH323ID, &tMtAlias ))
            {
                if(!GetMtAlias( byMtId, mtAliasTypeE164, &tMtAlias ))
                {
                    GetMtAlias( byMtId, mtAliasTypeTransportAddress, &tMtAlias );
                }
            }
        }		
	}

	if( tMtAlias.m_AliasType == mtAliasTypeH323ID || 
		tMtAlias.m_AliasType == mtAliasTypeE164 || 
		tMtAlias.m_AliasType == mtAliasTypeH320ID ||
        tMtAlias.m_AliasType == mtAliasTypeH320Alias)
	{
		m_atMtExt[byMtId - 1].SetAlias( tMtAlias.m_achAlias );
	}
	else if( tMtAlias.m_AliasType == mtAliasTypeTransportAddress )
	{
		m_atMtExt[byMtId - 1].SetAlias( StrOfIP( tMtAlias.m_tTransportAddr.GetIpAddr() ) );
	}
	else
	{
		return FALSE;
	}

    if( tMtAlias.m_AliasType == mtAliasTypeH320ID ||
        tMtAlias.m_AliasType == mtAliasTypeH320Alias)
    {
        m_atMtExt[byMtId - 1].SetProtocolType(PROTOCOL_TYPE_H320);
    }
    else
    {
        m_atMtExt[byMtId - 1].SetProtocolType(PROTOCOL_TYPE_H323);
    }
	
	return TRUE;
}

/*====================================================================
    ������      ��GetMtAlias
    ����        ���õ��ն˱���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
				  mtAliasType AliasType, ��������
				  TMtAlias * ptAlias, �ն˱���
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
BOOL32 TConfMtTable::GetMtAlias( u8 byMtId, mtAliasType AliasType, TMtAlias * ptAlias )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptAlias == NULL || 
		AliasType == 0 || AliasType > mtAliasTypeOthers )
	{
		return FALSE;
	}
	
	TMtAlias tMtAlias = m_atMtData[byMtId - 1].m_atMtAlias[ AliasType - 1 ].GetMtAlias();
	if( (tMtAlias.m_AliasType == mtAliasTypeTransportAddress) && 
		(tMtAlias.m_tTransportAddr.GetIpAddr() == 0) )
	{
		return FALSE;
	}

	if( (tMtAlias.m_AliasType != mtAliasTypeTransportAddress) && 
		(tMtAlias.m_achAlias[0] == 0) )
	{
		return FALSE;
	}

	*ptAlias = tMtAlias;
	
	return TRUE;

}


/*====================================================================
    ������      ��GetMtAlias
    ����        ���õ��ն˱���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
				  mtAliasType AliasType, ��������
				  TMtAlias * ptAlias, �ն˱���
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
s8* TConfMtTable::GetMtAliasFromExt( u8 byMtId ) const
{
	if( byMtId <= 0 || byMtId > MAXNUM_CONF_MT )
	{
		return NULL;
	}
	
    return m_atMtExt[byMtId - 1].GetAlias();
}


/*====================================================================
    ������      ��SetPuAlias
    ����        ������PU����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, PU��
				  const TMtAlias * ptAlias, PU����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/02/27    3.0         ���㻪         ����
====================================================================*/
BOOL32 TConfMtTable::SetPuAlias( u8 byMtId, const TMtAlias * ptAlias )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptAlias == NULL )
	{
		return FALSE;
	}

	if( ptAlias->m_AliasType != puAliasTypeIPPlusAlias ) 
	{
		return FALSE;
	}

	// ��pu�ô���Ϣ�����棬�ȴ����к�ˢ��
	// m_atMtData[byMtId - 1].m_atMtAlias[ptAlias->m_AliasType - 1] = *ptAlias;

	// PU�������IP+ALIAS
	s8* pbyAlias = (s8*)(ptAlias->m_achAlias);
	m_atMtExt[byMtId - 1].SetIPAddr(ntohl(*(u32*)pbyAlias));
	pbyAlias += sizeof(u32);
	m_atMtExt[byMtId - 1].SetAlias( pbyAlias );

    m_atMtExt[byMtId - 1].SetProtocolType(PROTOCOL_TYPE_H323);
	
	return TRUE;
}
/*====================================================================
    ������      ��SetNotInvited
    ����        �����ø��ն�Ϊδ�������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˺�
				  BOOL32 bNotInvited �Ƿ�����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/21    3.0         ������         ����
====================================================================*/
void TConfMtTable::SetNotInvited( u8 byMtId, BOOL32 bNotInvited )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return ;
	}

	m_atMtData[byMtId - 1].m_bNotInvited = bNotInvited;

	return;
}


/*=============================================================================
  �� �� ���� SetMtInDataConf
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byMtId
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/1/9    4.0			�ű���                  ����
=============================================================================*/
void TConfMtTable::SetMtInDataConf( u8 byMtId, BOOL32 bInDataConf )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return ;
    }

	m_atMtData[byMtId-1].m_bInDataConf = bInDataConf;
}

/*=============================================================================
  �� �� ���� IsMtInDataConf
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byMtId
  �� �� ֵ�� BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/1/9    4.0			�ű���                  ����
=============================================================================*/
BOOL32 TConfMtTable::IsMtInDataConf( u8 byMtId )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return FALSE;
    }

	return m_atMtData[byMtId-1].m_bInDataConf;
}

/*=============================================================================
  �� �� ���� SetMtCalling
  ��    �ܣ� �����ն��Ƿ��ն˺������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byMtId
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/1/9    4.0			�ű���                  ����
=============================================================================*
void TConfMtTable::SetMtCallingIn( u8 byMtId, BOOL32 bCallingIn )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return ;
    }

	m_atMtData[byMtId-1].m_bCallingIn = bCallingIn;
}*/

/*=============================================================================
  �� �� ���� IsMtCalling
  ��    �ܣ� �ն��Ƿ��ն˺������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byMtId
  �� �� ֵ�� BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/1/9    4.0			�ű���                  ����
=============================================================================*
BOOL32 TConfMtTable::IsMtCallingIn( u8 byMtId )
{	
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return FALSE;
    }

	return m_atMtData[byMtId-1].m_bCallingIn;
}*/

/*====================================================================
    ������      ��IsNotInvited
    ����        ���õ����ն��Ƿ�δ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˺�
    ����ֵ˵��  ��TRUE - δ������ FALSE - ������
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/12/21    3.0         ������         ����
====================================================================*/
BOOL32 TConfMtTable::IsNotInvited(  u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

    return m_atMtData[byMtId - 1].m_bNotInvited;
}

/*====================================================================
    ������      ��SetAddMtMode
    ����        �������ն˱�����ķ�ʽ(1: �ն��Ǳ�MCS����� 2: �ն��Ǳ���ϯ�����)
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId      --- �ն˺�
                  u8 byAddMtMode --- Ҫ���õ��ն˱�����ķ�ʽ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/03/30    3.6          �         ����
====================================================================*/
void TConfMtTable::SetAddMtMode(u8 byMtId, u8 byAddMtMode)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
	{
		return ;
	}

	m_atMtData[byMtId - 1].m_byAddMtMode = byAddMtMode;

	return;
}

/*====================================================================
    ������      ��GetAddMtMode
    ����        ���õ����ն˱�����ķ�ʽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId      --- �ն˺�
    ����ֵ˵��  ��1: �ն��Ǳ�MCS����� 2: �ն��Ǳ���ϯ�����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/03/30    3.6          �         ����
====================================================================*/
u8   TConfMtTable::GetAddMtMode(u8 byMtId)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
	{
		return FALSE;
	}

    return m_atMtData[byMtId - 1].m_byAddMtMode;
}

/*====================================================================
    ������      ��IsMtMulticasting
    ����        ���õ����ն��Ƿ������鲥
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˺�
    ����ֵ˵��  ��TRUE - �鲥 FALSE - δ�鲥
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/27    3.0         ������         ����
====================================================================*/
BOOL32 TConfMtTable::IsMtMulticasting( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

    return m_atMtData[byMtId - 1].m_bMulticasting;
}

/*====================================================================
    ������      ��SetMtMulticasting
    ����        �����ø��ն��Ƿ������鲥
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˺�
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/27    3.0         ������         ����
====================================================================*/
void TConfMtTable::SetMtMulticasting( u8 byMtId, BOOL32 bMulticasting )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return ;
	}

	m_atMtData[byMtId - 1].m_bMulticasting = bMulticasting;

	return;
}

/*====================================================================
    ������      ��AddMonitorMt
    ����        �����Ӽ���ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˺�
				  u8 byMonitorMtId ������ն˺�
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/27    3.0         ������         ����
====================================================================*/
void TConfMtTable::AddMonitorSrcMt( u8 byMtId, u8 byMonitorMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return ;
	}

	//���Ҹ��ն��Ƿ����ڼ���б���
	u8 byMtIndex = 0;
	while( byMtIndex < MAXNUM_MT_CHANNL )
	{
		if( m_atMtData[byMtId - 1].m_byMonitorSrcMt[byMtIndex] == byMonitorMtId )
		{
			break;
		}
		else
		{
			byMtIndex++;
		}
	}

	//����
	if( byMtIndex == MAXNUM_MT_CHANNL )
	{
		byMtIndex = 0;
		while( byMtIndex < MAXNUM_MT_CHANNL )
		{
			if( m_atMtData[byMtId - 1].m_byMonitorSrcMt[byMtIndex] == 0 )
			{
				m_atMtData[byMtId - 1].m_byMonitorSrcMt[byMtIndex] = byMonitorMtId;
				return;
			}
			else
			{
				byMtIndex++;
			}
		}
	}
	
	return;
}

/*====================================================================
    ������      ��RemoveMonitorMt
    ����        ���Ƴ�����ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˺�
				  u8 byMonitorMtId ������ն˺�
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/27    3.0         ������         ����
====================================================================*/
void TConfMtTable::RemoveMonitorSrcMt( u8 byMtId, u8 byMonitorMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return ;
	}

	u8 byMtIndex = 0;
	while( byMtIndex < MAXNUM_MT_CHANNL )
	{
		if( m_atMtData[byMtId - 1].m_byMonitorSrcMt[byMtIndex] == byMonitorMtId )
		{
			m_atMtData[byMtId - 1].m_byMonitorSrcMt[byMtIndex] = 0;
			return;
		}
		else
		{
			byMtIndex++;
		}
	}

	return;
}

/*====================================================================
    ������      ��GetMonitorSrcMtList
    ����        ���õ����Դ�ն��б�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˺�
				  u8 byMtList �ն��б�
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/27    3.0         ������         ����
====================================================================*/
u8   TConfMtTable::GetMonitorSrcMtList( u8 byMtId, u8 byMtList[] )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}

	u8 byMtIndex = 0;
	for( u8 byLoop = 0; byLoop < MAXNUM_MT_CHANNL; byLoop++  )
	{
		if( m_atMtData[byMtId - 1].m_byMonitorSrcMt[byLoop] != 0 )
		{
			byMtList[byMtIndex] = m_atMtData[byMtId - 1].m_byMonitorSrcMt[byLoop];
			byMtIndex++;
		}
	}

	return byMtIndex;
}

/*====================================================================
    ������      ��AddMonitorDstMt
    ����        ���Ƴ�����ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˺�
				  u8 byDstMtId ����ն˺�
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/27    3.0         ������         ����
====================================================================*/
void TConfMtTable::AddMonitorDstMt( u8 byMtId, u8 byDstMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return ;
	}

	m_atMtData[byMtId - 1].m_byMonitorDstMt[(byDstMtId-1) / 8] |= 1 << ( (byDstMtId-1) % 8 );

	return;
}

/*====================================================================
    ������      ��RemoveMonitorDstMt
    ����        ���Ƴ�����ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˺�
				  u8 byMonitorMtId ����ն˺�
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/27    3.0         ������         ����
====================================================================*/
void TConfMtTable::RemoveMonitorDstMt( u8 byMtId, u8 byDstMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return ;
	}

	m_atMtData[byMtId - 1].m_byMonitorDstMt[(byDstMtId-1) / 8] &= ~( 1 << ( (byDstMtId-1) % 8 ) );

	return;
}

/*====================================================================
    ������      ��RemoveMonitorMt
    ����        ���Ƴ�����ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˺�
				  u8 byMonitorMtId ������ն˺�
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/27    3.0         ������         ����
====================================================================*/
BOOL32 TConfMtTable::HasMonitorDstMt( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

	u8 byLoop = 0;
	while( byLoop < MAXNUM_MT_CHANNL )
	{
		if( m_atMtData[byMtId - 1].m_byMonitorDstMt[byLoop] != 0 )
		{
			return TRUE;
		}
		else
		{
			byLoop++;
		}
	}

	return FALSE;
}

/*====================================================================
    ������      ��ClearMonitorSrcAndDst
    ����        ���Ƴ�����ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˺�
				  u8 byMonitorMtId ������ն˺�
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/04/27    3.0         ������         ����
====================================================================*/
void TConfMtTable::ClearMonitorSrcAndDst( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return ;
	}

	memset( m_atMtData[byMtId - 1].m_byMonitorDstMt, 0, sizeof(m_atMtData[byMtId - 1].m_byMonitorDstMt) ); 
	memset( m_atMtData[byMtId - 1].m_byMonitorSrcMt, 0, sizeof(m_atMtData[byMtId - 1].m_byMonitorSrcMt) ); 

	return;
}

/*====================================================================
    ������      ��SetMpId
    ����        �������ն�Mp��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
				  u8 byMpId, �ն�Mp��
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/06/02    3.0         ������         ����
====================================================================*/
void TConfMtTable::SetMpId( u8 byMtId, u8 byMpId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || byMpId > MAXNUM_DRI)
	{
		return;
	}
	m_atMtData[byMtId - 1].m_byMpId = byMpId;

	return;
}

/*====================================================================
    ������      ��GetMpId
    ����        ���õ��ն�MP�� 
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/06/02    3.0         ������         ����
====================================================================*/
u8 TConfMtTable::GetMpId( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
	return m_atMtData[byMtId - 1].m_byMpId;
}

/*====================================================================
    ������      ��SetLastTick
    ����        �������ն˵������FastUpdatePic��Tick��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
				  u32 dwLastTick, Tick��
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/06/02    3.0         ������         ����
====================================================================*/
void TConfMtTable::SetLastTick( u8 byMtId, u32 dwLastTick, BOOL32 bSecVideo )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	if( bSecVideo )
	{
        m_atMtData[byMtId - 1].m_dwSecVideoLastRefreshTick = dwLastTick;
	}
	else
	{
        m_atMtData[byMtId - 1].m_dwLastRefreshTick = dwLastTick;
	}
	
}

/*====================================================================
    ������      ��GetLastTick
    ����        ���õ��ն˵������FastUpdatePic��Tick��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/06/02    3.0         ������         ����
====================================================================*/
u32   TConfMtTable::GetLastTick( u8 byMtId, BOOL32 bSecVideo )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}

	if( bSecVideo )
	{
        return m_atMtData[byMtId - 1].m_dwSecVideoLastRefreshTick;
	}
	else
	{
        return m_atMtData[byMtId - 1].m_dwLastRefreshTick;
	}
		
}

/*====================================================================
    ������      ��SetMtReqBitrate
    ����        �����ø��ն�Ҫ����յ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˺�
	              u16 wBitRate
	              u8 byChnlMode = LOGCHL_VIDEO_BOTH
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/02/12    3.0         ������         ����
====================================================================*/
void TConfMtTable::SetMtReqBitrate( u8 byMtId, u16 wBitRate, u8 byChnlMode )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	if( LOGCHL_VIDEO_BOTH == byChnlMode || LOGCHL_VIDEO == byChnlMode )
	{
		m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel.SetFlowControl( wBitRate );
	}
	if( LOGCHL_VIDEO_BOTH == byChnlMode || LOGCHL_SECVIDEO == byChnlMode )
	{
		m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel.SetFlowControl( wBitRate );
	}

	return;
}

/*====================================================================
    ������      ��GetMtReqBitrate
    ����        ���õ����ն�Ҫ����յ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˺�
	              bPrimary = TRUE
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/02/12    3.0         ������         ����
====================================================================*/
u16  TConfMtTable::GetMtReqBitrate( u8 byMtId, BOOL32 bPrimary )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}

	if( TRUE == bPrimary )
	{
		return m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel.GetFlowControl();
	}
	else
	{
		return m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel.GetFlowControl();
	}
}

void TConfMtTable::SetMtSndBitrate( u8 byMtId, u16 wBitRate )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel.SetFlowControl( wBitRate );

	return;
}

u16  TConfMtTable::GetMtSndBitrate( u8 byMtId, u8 byChannel /*= LOGCHL_VIDEO*/ )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}

    if (LOGCHL_VIDEO == byChannel)
    {
        return m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel.GetFlowControl();
    }
    else if (LOGCHL_SECVIDEO == byChannel)
    {
        return m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel.GetFlowControl();
    }
    else
    {
        OspPrintf(TRUE, FALSE, "[GetMtSndBitrate] get mt sending bitrate error!!!\n");
        return 0;
    }
}

/*====================================================================
    ������      ��SetMcuId
    ����        �������ն˵�Mcu Id
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˺�
				  u8 byMcuId MCU��
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
void TConfMtTable::SetMcuId( u8 byMtId, u8 byMcuId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	m_atMtExt[byMtId - 1].SetMcuId( byMcuId );

	return;
}

/*====================================================================
    ������      ��GetMcuId
    ����        ���õ��ն˵�MCU��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
u8 TConfMtTable::GetMcuId( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}

	return m_atMtExt[byMtId - 1].GetMcuId();
}

/*====================================================================
    ������      ��SetDriId
    ����        ��������ֶDRI��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
				  u8 byDriId, �ն�DRI��
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
void TConfMtTable::SetDriId( u8 byMtId, u8 byDriId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || byDriId > MAXNUM_DRI)
	{
		return;
	}
	m_atMtExt[byMtId - 1].SetDriId( byDriId );

	return;
}

/*====================================================================
    ������      ��GetDriId
    ����        ���õ��ն�DRI��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
u8 TConfMtTable::GetDriId( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
	return m_atMtExt[byMtId - 1].GetDriId();
}

/*====================================================================
    ������      ��SetConfIdx
    ����        �������ն˻���������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
				  u8 byConfIdx, �ն˻���������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
void TConfMtTable::SetConfIdx(u8 byMtId, u8 byConfIdx )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	m_atMtExt[byMtId - 1].SetConfIdx( byConfIdx );

	return;
}

/*====================================================================
    ������      ��GetConfIdx
    ����        ���õ��ն˻���������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
u8 TConfMtTable::GetConfIdx( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
	return m_atMtExt[byMtId - 1].GetConfIdx();
}

/*====================================================================
    ������      ��SetMtType
    ����        �������ն�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
				  u8 byMtType, �ն�����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
void TConfMtTable::SetMtType( u8 byMtId, u8 byMtType )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	m_atMtExt[byMtId - 1].SetMtType( byMtType );

	return;
}

/*====================================================================
    ������      ��GetMtType
    ����        ���õ��ն�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
u8 TConfMtTable::GetMtType( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
	return m_atMtExt[byMtId - 1].GetMtType();
}

/*====================================================================
    ������      ��SetMainType
    ����        �������ն�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
				  u8 byType, �ն�����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/03/20    3.6         κ�α�         ����
====================================================================*/
void TConfMtTable::SetMainType( u8 byMtId, u8 byType )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	m_atMtExt[byMtId - 1].SetType( byType );
	return;
}

/*====================================================================
    ������      ��GetMainType
    ����        ���õ��ն�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         κ�α�         ����
====================================================================*/
u8 TConfMtTable::GetMainType( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
	return m_atMtExt[byMtId - 1].GetType();
}

/*====================================================================
    ������      ��SetManuId
    ����        �������ն����쳧��Id
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
				  u8 byManuId, �ն����쳧��Id
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
void TConfMtTable::SetManuId( u8 byMtId, u8 byManuId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	m_atMtExt[byMtId - 1].SetManuId( byManuId );

	return;
}

/*====================================================================
    ������      ��GetManuId
    ����        ���õ��ն����쳧��Id
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
u8 TConfMtTable::GetManuId( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
	
	//FIXME: �ܺ���ʱ�޸ģ��Ժ���������Э�̴�������
#ifdef _SATELITE_
	return MT_MANU_KDC;
#else
	return m_atMtExt[byMtId - 1].GetManuId();
#endif
}

/*=============================================================================
  �� �� ���� GetProtocolType
  ��    �ܣ� ȡ�ն�Э������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byMtId
  �� �� ֵ�� u8 
 ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    06/02/17    4.0         john           ����
=============================================================================*/
u8 TConfMtTable::GetProtocolType( u8 byMtId )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
    return m_atMtExt[byMtId - 1].GetProtocolType();
}

/*=============================================================================
  �� �� ���� SetProtocolType
  ��    �ܣ� �����ն�Э������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byMtId
             u8 byProtocolType
  �� �� ֵ�� void 
  ----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    06/02/17    4.0         john           ����
=============================================================================*/
void TConfMtTable::SetProtocolType(u8 byMtId, u8 byProtocolType )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return ;
	}
    m_atMtExt[byMtId - 1].SetProtocolType( byProtocolType );
}
/*====================================================================
    ������      ��SetCallMode
    ����        �������ն˺��з�ʽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
				  u8 byCallMode, �ն˺��з�ʽ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
void TConfMtTable::SetCallMode( u8 byMtId, u8 byCallMode )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	m_atMtExt[byMtId - 1].SetCallMode( byCallMode );

	return;
}

/*====================================================================
    ������      ��GetCallMode
    ����        ���õ��ն˺��з�ʽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
u8 TConfMtTable::GetCallMode( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
	return m_atMtExt[byMtId - 1].GetCallMode();
}

/*=============================================================================
    �� �� ���� SetCallLeftTimes
    ��    �ܣ� �����ն�ʣ����д���
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8  byMtId
               u32 dwCallTimes
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/03/18  3.6			����                  ����
=============================================================================*/
void TConfMtTable::SetCallLeftTimes( u8 byMtId, u32 dwCallLeftTimes )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	m_atMtExt[byMtId - 1].SetCallLeftTimes( dwCallLeftTimes );

	return;
}

/*=============================================================================
    �� �� ���� GetCallLeftTimes
    ��    �ܣ� ��ȡ�ն�ʣ����д���
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ����u8 byMtId
    �� �� ֵ��u32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/03/18  3.6			����                  ����
=============================================================================*/
u32 TConfMtTable::GetCallLeftTimes( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
	return m_atMtExt[byMtId - 1].GetCallLeftTimes();
}

/*====================================================================
    ������      ��SetIPAddr
    ����        �������ն�IP��ַ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
				  u32 dwIPAddr, IP��ַ
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
void TConfMtTable::SetIPAddr( u8 byMtId, u32 dwIPAddr )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	m_atMtExt[byMtId - 1].SetIPAddr( dwIPAddr );

	return;
}

/*====================================================================
    ������      ��GetIPAddr
    ����        ���õ��ն�IP��ַ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
u32 TConfMtTable::GetIPAddr( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
//zhangs if IP not present,this cause a exception
	return m_atMtExt[byMtId - 1].GetIPAddr();
}

/*====================================================================
    ������      ��GetMt
    ����        ���õ��ն˽ṹ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
TMt TConfMtTable::GetMt( u8 byMtId )
{
    TMt tMt;
	tMt.SetNull();
	
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return tMt;
	}

    tMt = (TMt)m_atMtExt[byMtId - 1];
	return tMt;
}

/*====================================================================
    ������      ��AddMt
    ����        �������ն�(�����ն�IP)
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u32 dwIpAddr,        �ն�IP��ַ
				  BOOL32  bRepeatFilter  �Ƿ���˱����ظ����������ǿ�����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
u8 TConfMtTable::AddMt( u32 dwIpAddr, BOOL32 bRepeatFilter )
{
    if( dwIpAddr == 0 )return 0;

	u8 byMtId = GetMtIdByIp( dwIpAddr );

	//�Ѵ��� ���� ���˱����ظ�, ���ⷴ�����
	if( byMtId > 0 && bRepeatFilter )
	{
		return 0;
	}

	TMtAlias tMtAlias;
	tMtAlias.m_AliasType = mtAliasTypeTransportAddress;
    tMtAlias.m_tTransportAddr.SetIpAddr( dwIpAddr );
	tMtAlias.m_tTransportAddr.SetPort( DEFAULT_Q931_PORT );
  
	byMtId = 1;
	while( byMtId <= MAXNUM_CONF_MT )
	{
	    if( m_atMtExt[byMtId - 1].GetMtId() == 0 && byMtId != LOCAL_MCUID)//�ҵ���λ
		{
            m_atMtExt[byMtId - 1].SetMtId( byMtId );
			m_atMtExt[byMtId - 1].SetIPAddr( dwIpAddr );
			SetMtAlias( byMtId, &tMtAlias );
            if(byMtId > m_byMaxNumInUse)
			{
				m_byMaxNumInUse = byMtId;
			}
			break;
		}
	     byMtId++;
	}

	if( byMtId == MAXNUM_CONF_MT + 1 )//�������ն���
	{
		return 0;
	}
	return byMtId;
}

/*====================================================================
    ������      ��AddMt
    ����        �������ն�(�����ն˱���)
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMtAlias * ptAlias, �ն˱���
				  BOOL32  bRepeatFilter       �Ƿ���˱����ظ����������ǿ�����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
u8 TConfMtTable::AddMt( TMtAlias * ptAlias, BOOL32 bRepeatFilter )
{
    if( ptAlias == NULL || ptAlias->m_AliasType == 0 || 
		(ptAlias->m_AliasType > mtAliasTypeOthers && 
		 //pu��������
		 ptAlias->m_AliasType != puAliasTypeIPPlusAlias))
	{
		return 0;
	}

	u8 byMtId = GetMtIdByAlias( ptAlias );

	//�Ѵ��� ���� ���˱����ظ�, ���ⷴ�����
	if( byMtId > 0 && bRepeatFilter )
	{
		return 0;
	}
  
	byMtId = 1;
	while( byMtId <= MAXNUM_CONF_MT )
	{
	    if( m_atMtExt[byMtId - 1].GetMtId() == 0 && byMtId != LOCAL_MCUID )        //�ҵ���λ
		{
            m_atMtExt[byMtId - 1].SetMtId( byMtId );
			if( ptAlias->m_AliasType == mtAliasTypeTransportAddress )
			{
				m_atMtExt[byMtId - 1].SetIPAddr( ptAlias->m_tTransportAddr.GetIpAddr() );
				if( 0 == ptAlias->m_tTransportAddr.GetPort() )
				{
					ptAlias->m_tTransportAddr.SetPort( DEFAULT_Q931_PORT );
				}
			}
			
			// pu���ñ���,����дIP
			if ( puAliasTypeIPPlusAlias == ptAlias->m_AliasType )
			{
				SetPuAlias( byMtId, ptAlias );
			}
			else
			{
				SetMtAlias( byMtId, ptAlias );
			}
			if(byMtId > m_byMaxNumInUse)
			{
				m_byMaxNumInUse = byMtId;
			}
			break;
		}
	    byMtId++;
	}

	if( byMtId == MAXNUM_CONF_MT + 1 )//�������ն���
	{
		return 0;
	}

	return byMtId;
}

/*====================================================================
    ������      ��SetDialAlias
    ����        �����ò��ŵı���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMtAlias * ptAlias, �ն˱���
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/03/10    3.6        Jason        ����
====================================================================*/
void TConfMtTable::SetDialAlias( u8 byMtId, const TMtAlias * ptAlias )
{
    if( NULL == ptAlias || 0 == ptAlias->m_AliasType || 
		(ptAlias->m_AliasType > mtAliasTypeOthers &&
		 ptAlias->m_AliasType != puAliasTypeIPPlusAlias)||
		byMtId > MAXNUM_CONF_MT || byMtId <= 0 )
	{
		return;
	}

	if (puAliasTypeIPPlusAlias == ptAlias->m_AliasType)
	{
		// ��������pu����
		TMtAlias tMtAlias;
		s8* pachAlias = (s8*)(ptAlias->m_achAlias);
		tMtAlias.m_tTransportAddr.SetIpAddr(ntohl(*(u32*)(pachAlias)));
 		tMtAlias.m_tTransportAddr.SetPort(MCU_Q931_PORT);
		pachAlias += sizeof(u32);
 		tMtAlias.SetH323Alias(pachAlias);
		tMtAlias.m_AliasType = ptAlias->m_AliasType;
		m_atMtData[byMtId - 1].m_tDialAlias = tMtAlias;
	}
	else
	{
		m_atMtData[byMtId - 1].m_tDialAlias = *ptAlias;
	}
}

/*====================================================================
    ������      ��GetDialAlias
    ����        ���õ����ŵı���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����const TMtAlias * ptAlias, �ն˱���
    ����ֵ˵��  ��true or false
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/03/10    3.6        Jason        ����
====================================================================*/
BOOL32 TConfMtTable::GetDialAlias( u8 byMtId, TMtAlias * ptAlias )
{
    if( NULL == ptAlias ||
		byMtId > MAXNUM_CONF_MT || byMtId <= 0 )
	{
		return FALSE;
	}

    *ptAlias = m_atMtData[byMtId - 1].m_tDialAlias.GetMtAlias();
   
	return TRUE;
}

/*=============================================================================
    �� �� ���� SetDialBitrate
    ��    �ܣ� �����ն˵ĺ�������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8  byMtId
               u16 wDialBitrate ��������
    �� �� ֵ�� void 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/03/17  3.6			����                  ����
=============================================================================*/
void TConfMtTable::SetDialBitrate( u8 byMtId, u16 wDialBitrate )
{
    if( 0 == wDialBitrate ||
		byMtId > MAXNUM_CONF_MT || byMtId <= 0 )
	{
		return;
	}

	m_atMtExt[byMtId - 1].SetDialBitRate( wDialBitrate );
}

/*=============================================================================
    �� �� ���� GetDialBitrate
    ��    �ܣ� ��ȡ�ն˵ĺ�������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8  byMtId
    �� �� ֵ�� u16 wDialBitrate ��������
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/03/17  3.6			����                  ����
=============================================================================*/
u16 TConfMtTable::GetDialBitrate( u8 byMtId )
{
    if( byMtId > MAXNUM_CONF_MT || byMtId <= 0 )
	{
		return 0;
	}

	return m_atMtExt[byMtId - 1].GetDialBitRate();
}


/*=============================================================================
�� �� ���� SetSndBandWidth
��    �ܣ� �ն����д���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u16 wBandWidth
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/3/21  4.0			������                  ����
=============================================================================*/
void TConfMtTable::SetSndBandWidth(u8 byMtId, u16 wBandWidth)
{
    if( byMtId > MAXNUM_CONF_MT || byMtId <= 0 )
    {
        return;
    }

    m_atMtData[byMtId-1].m_wSndBandWidth = wBandWidth;

    return;
}

/*=============================================================================
�� �� ���� GetSndBandWidth
��    �ܣ� �ն����д���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� u16  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/3/21  4.0			������                  ����
=============================================================================*/
u16  TConfMtTable::GetSndBandWidth(u8 byMtId)
{
    if( byMtId > MAXNUM_CONF_MT || byMtId <= 0 )
    {
        return 0;
    }
    
    return m_atMtData[byMtId-1].m_wSndBandWidth;
}


/*=============================================================================
�� �� ���� SetRcvBandWidth
��    �ܣ� �����ն˽��մ�����Ƶ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byMtId
           u16 wBandWidth
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/3/25  4.0			������                  ����
=============================================================================*/
void TConfMtTable::SetRcvBandWidth(u8 byMtId, u16 wBandWidth)
{
    if (byMtId > MAXNUM_CONF_MT || byMtId <= 0)
    {
        return;
    }

    m_atMtData[byMtId-1].m_wRcvBandWidth = wBandWidth;

    return;
}


/*=============================================================================
�� �� ���� GetRcvBandWidth
��    �ܣ� ��ȡ�ն˽��մ�����Ƶ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byMtId
�� �� ֵ�� u16 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/3/25  4.0			������                  ����
=============================================================================*/
u16 TConfMtTable::GetRcvBandWidth(u8 byMtId)
{
    if (byMtId > MAXNUM_CONF_MT || byMtId <= 0)
    {
        return 0;
    }

    return m_atMtData[byMtId-1].m_wRcvBandWidth;
}

/*====================================================================
    ������      ��DelMt
    ����        �����ն��б���ɾ���ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˱��
    ����ֵ˵��  ��TRUE - ɾ���ɹ� FALSE -ɾ��ʧ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
BOOL32 TConfMtTable::DelMt( u8 byMtId )
{
	if( byMtId == 0 || byMtId > m_byMaxNumInUse 
		|| m_atMtExt[byMtId - 1].GetMtId() != byMtId )
	{
		return FALSE;
	}

    memset( &m_atMtExt[byMtId - 1], 0, sizeof(TMtExt) );
	memset( &m_atMtData[byMtId - 1], 0, sizeof(TMtData) ); 
	memset( &m_atMtExt2[byMtId - 1], 0, sizeof(TMtExt2) );
	return TRUE;
}

/*====================================================================
    ������      ��GetMtIdByIp
    ����        ������IP��ַ�����ն�Id
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u32 dwIpAddr, �ն�IP��ַ
    ����ֵ˵��  ��0 - ������IPΪ����ֵ���ն�
	              ����0 - �ն�Id
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
u8 TConfMtTable::GetMtIdByIp( u32 dwIpAddr )
{
	if( dwIpAddr == 0 )return 0;

    u8 byMtId = 1;

	//����
	while( byMtId <= m_byMaxNumInUse )
	{
		if( m_atMtExt[byMtId - 1].GetMtId() != byMtId )
		{
			byMtId++;
		}
		else
		{
            if( m_atMtExt[byMtId - 1].GetIPAddr() == dwIpAddr)
			{
				return byMtId;
			}
			else
			{
				byMtId++;
			}
		}
	}

	return 0;	
}

/*====================================================================
    ������      ��GetMtIdByAlias
    ����        �����ݱ��������ն�Id
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMtAlias tMtAlias, �ն˱���
    ����ֵ˵��  ��0 - ������IPΪ����ֵ���ն�
	              ����0 - �ն�Id
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
u8 TConfMtTable::GetMtIdByAlias( const TMtAlias * ptAlias )
{
    if( ptAlias == NULL || ptAlias->m_AliasType == 0 ||
		(ptAlias->m_AliasType > mtAliasTypeOthers && 
		 ptAlias->m_AliasType != puAliasTypeIPPlusAlias))
	{
		return 0;
	}

    u8 byMtId = 1;

	//����
	while( byMtId <= m_byMaxNumInUse )
	{
		if( m_atMtExt[byMtId - 1].GetMtId() == byMtId )
		{
            // zbq [08/08/2007] �����ն˺���˿ڵĲ�ȷ���ԣ�Ip�������ٱȽ϶˿�
            if ( mtAliasTypeTransportAddress == ptAlias->m_AliasType )
            {
                if ( ptAlias->m_tTransportAddr.GetIpAddr() == 
                     m_atMtData[byMtId-1].m_atMtAlias[ptAlias->m_AliasType-1].GetMtAlias().m_tTransportAddr.GetIpAddr() )
                {
                    return byMtId;
                }
            }
			else if ( puAliasTypeIPPlusAlias == ptAlias->m_AliasType)
			{
				if ( ptAlias->m_tTransportAddr.GetIpAddr() == 
                     m_atMtData[byMtId-1].m_atMtAlias[mtAliasTypeH323ID -1].GetMtAlias().m_tTransportAddr.GetIpAddr() &&
					 strcmp(ptAlias->m_achAlias, m_atMtData[byMtId-1].m_atMtAlias[mtAliasTypeH323ID -1].GetMtAlias().m_achAlias) == 0)
				{
					return byMtId;
				}
			}
            else
            {
                if( *ptAlias == m_atMtData[byMtId-1].m_atMtAlias[ptAlias->m_AliasType-1].GetMtAlias())
                {
                    return byMtId;
                }                
            }
		}
        byMtId++;
	}

	return 0;	
}

/*====================================================================
    ������      : SetMtRecording
    ����        �������ն�¼��״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
				  const TEqp& tRecEqp, ¼������ 
				  const u8& byRecChannel, ¼��ͨ��
    ����ֵ˵��  :��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/09/04    1.0				           ����
====================================================================*/
void TConfMtTable::SetMtRecording( u8 byMtId, const TEqp& tRecEqp, const u8& byRecChannel )
{
	if( byMtId != 0 && byMtId <= MAXNUM_CONF_MT )
	{
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState.SetRecording();
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_byRecChannel = byRecChannel;
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecEqp = tRecEqp;

		//ͬ���ն˵�¼��״̬
		m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState;
	}

	return;
}

/*====================================================================
    ������      : GetMtRecordInfo
    ����        ����ȡ�ն˵�¼�����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 
				  TEqp tRecEqp, ¼������ 
				  u8 byRecChannel, ¼��ͨ�� 
    ����ֵ˵��  :�ɹ�����TRUE�����򷵻�FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/09/04    1.0         JQL           ����
====================================================================*/
BOOL32 TConfMtTable::GetMtRecordInfo( u8 byMtId, TEqp* ptRecEqp, u8* pbyRecChannel )
{
	if( byMtId != 0 && byMtId <= MAXNUM_CONF_MT && ptRecEqp != NULL && pbyRecChannel != NULL )
	{
		*pbyRecChannel = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_byRecChannel;
		*ptRecEqp = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecEqp;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*====================================================================
    ������      : GetMtIdFromRecordInfo
    ����        ����ȡ��¼��ͨ����������ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TEqp tRecEqp, ¼������ 
				  u8 byRecChannel, ¼��ͨ�� 
    ����ֵ˵��  :�ɹ������ն�Id��ʧ�ܷ���0
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/09/04    1.0         JQL           ����
====================================================================*/
u8 TConfMtTable::GetMtIdFromRecordInfo( const TEqp& tRecEqp, const u8& byRecChannel )
{
	for( u8 byMtLoop = 1; byMtLoop <= MAXNUM_CONF_MT; byMtLoop++ )
	{
		if( m_atMtData[ byMtLoop -1 ].m_tMtRecInfo.m_byRecChannel == byRecChannel 
			&& m_atMtData[ byMtLoop -1 ].m_tMtRecInfo.m_tRecEqp == tRecEqp )
			return byMtLoop;
	}
	return 0;
}

/*====================================================================
    ������      : SetMtNoRecording
    ����        �������ն�δ¼��״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 
    ����ֵ˵��  :��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/09/04    1.0         JQL           ����
====================================================================*/
void TConfMtTable::SetMtNoRecording( u8 byMtId )
{
	if( byMtId != 0 && byMtId <= MAXNUM_CONF_MT )
	{	
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState.SetNoRecording();
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_byRecChannel = 0;
		memset( &m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecEqp , 0, sizeof( TEqp ) );

		//ͬ���ն˵�¼��״̬
		m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState;

	}
}

/*====================================================================
    ������      : SetMtRecPause
    ����        �������ն�¼����ͣ״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 
    ����ֵ˵��  :��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/09/04    1.0         JQL           ����
====================================================================*/
void TConfMtTable::SetMtRecPause( u8 byMtId )
{
	if( byMtId != 0 && byMtId <= MAXNUM_CONF_MT )
	{
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState.SetRecPause();

		//ͬ���ն˵�¼��״̬
		m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState;
	}
}

/*====================================================================
    ������      : GetMtRecState
    ����        ����ȡ�ն�¼��״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�  
				  TRecState * ptRecState, ¼���״̬
    ����ֵ˵��  :��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/09/04    1.0         JQL           ����
====================================================================*/
BOOL32 TConfMtTable::GetMtRecState( u8 byMtId, TRecState * ptRecState )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptRecState == NULL )
		return FALSE;

	*ptRecState = m_atMtData[ byMtId -1 ].m_tMtRecInfo.m_tRecState;
	//ͬ���ն˵�¼��״̬
	m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState;

	return TRUE;

}

/*====================================================================
    ������      : IsMtRecording
    ����        ����ѯ�ն�¼���Ƿ�����¼��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 
    ����ֵ˵��  :TRUE/FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/09/04    1.0         JQL           ����
====================================================================*/
BOOL32 TConfMtTable::IsMtRecording( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
		return FALSE;

	return m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecState.IsRecording();

}

/*====================================================================
    ������      : IsMtRecPause
    ����        ����ѯ�ն�¼���Ƿ���ͣ״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 
    ����ֵ˵��  :TRUE/FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/09/04    1.0         JQL           ����
====================================================================*/
BOOL32 TConfMtTable::IsMtRecPause( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
		return FALSE;

	return m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecState.IsRecPause();

}

/*====================================================================
    ������      : IsMtNoRecording
    ����        ����ѯ�ն�¼���Ƿ���ͣ״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 
    ����ֵ˵��  :TRUE/FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/09/04    1.0         JQL           ����
====================================================================*/
BOOL32 TConfMtTable::IsMtNoRecording( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
		return FALSE;

	return m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecState.IsNoRecording();

}

/*====================================================================
    ������      : IsMtRecSkipFrame
    ����        ���ն��Ƿ��ǳ�֡¼��״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 
    ����ֵ˵��  :TRUE/FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/09/04    1.0         JQL           ����
====================================================================*/
BOOL32 TConfMtTable::IsMtRecSkipFrame( u8 byMtId ) const
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
		return FALSE;

	return m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecState.IsRecSkipFrame();

}

/*====================================================================
    ������      : SetMtRecSkipFrame
    ����        �������ն�Ϊ��֡¼��״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 
				  BOOL32 bSkipFrame, �Ƿ��֡¼��
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/09/04    1.0         JQL           ����
====================================================================*/
void TConfMtTable::SetMtRecSkipFrame( u8 byMtId, BOOL32 bSkipFrame )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecState.SetRecSkipFrame( bSkipFrame );
	//ͬ���ն˵�¼��״̬
	m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState;

	return;
}

/*====================================================================
    ������      : SetMtRecProg
    ����        �������ն�¼�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 
				  const TRecProg & tRecProg, ¼�����
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/09/04    1.0         JQL           ����
====================================================================*/
void TConfMtTable::SetMtRecProg( u8 byMtId, const TRecProg & tRecProg )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	
	m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecProg = tRecProg;

	return;
}

/*====================================================================
    ������      : GetMtRecProg
    ����        ���õ��ն�¼�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 
				  const TRecProg & tRecProg, ¼�����
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/09/04    1.0         JQL           ����
====================================================================*/
BOOL32 TConfMtTable::GetMtRecProg( u8 byMtId, TRecProg * ptRecProg )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptRecProg == NULL )
		return FALSE;

	*ptRecProg = m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecProg;
	return TRUE;	
}

/*====================================================================
    ������      : IsMtVideoSending
    ����        ���ն��Ƿ����ڷ�����Ƶ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 		
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/02/24    3.0         ������           ����
====================================================================*/
BOOL32 TConfMtTable::IsMtVideoSending( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

	return m_atMtData[ byMtId - 1].m_tMtStatus.IsSendVideo();
}

/*====================================================================
    ������      : IsMtVideoSending
    ����        ���ն��Ƿ����ڷ�����Ƶ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 		
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/02/24    3.0         ������           ����
====================================================================*/
BOOL32 TConfMtTable::IsMtAudioSending( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

	return m_atMtData[ byMtId - 1].m_tMtStatus.IsSendAudio();
}

/*====================================================================
    ������      : IsMtVideo2Sending
    ����        ���ն��Ƿ����ڷ��͵ڶ���Ƶ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 		
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/02/24    3.0         ������           ����
====================================================================*/
BOOL32 TConfMtTable::IsMtVideo2Sending( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

	return m_atMtData[ byMtId - 1].m_tMtStatus.IsSndVideo2();
}

/*====================================================================
    ������      : IsMtVideo2Recving
    ����        ���ն��Ƿ����ڽ��յڶ���Ƶ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 		
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/02/24    3.0         ������           ����
====================================================================*/
BOOL32 TConfMtTable::IsMtVideo2Recving( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

	return m_atMtData[ byMtId - 1].m_tMtStatus.IsRcvVideo2();
}

/*====================================================================
    ������      : IsMtInMixing
    ����        ���ն��Ƿ����ڻ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 		
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/02/24    3.0         ������           ����
====================================================================*/
BOOL32 TConfMtTable::IsMtInMixing( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

	return m_atMtData[ byMtId - 1].m_tMtStatus.IsInMixing();
}


/*=============================================================================
�� �� ���� SetMtInMixGrp
��    �ܣ� �����ն��Ƿ��ڻ���ͨ����״̬
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  u8 byMtId
           BOOL32 bDiscuss
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/7/29  4.0			������                  ����
=============================================================================*/
void TConfMtTable::SetMtInMixGrp( u8 byMtId, BOOL32 bDiscuss )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	m_atMtData[byMtId - 1].m_tMtStatus.SetIsInMixGrp( bDiscuss );
}

/*=============================================================================
�� �� ���� IsMtInMixGrp
��    �ܣ� �ն��Ƿ��ڻ���ͨ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byMtId
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/7/29  4.0			������                  ����
=============================================================================*/
BOOL32 TConfMtTable::IsMtInMixGrp( u8 byMtId )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

	return m_atMtData[ byMtId - 1].m_tMtStatus.IsInMixGrp();
}

/*====================================================================
    ������      : SetMtVideoSend
    ����        �������ն��Ƿ����ڷ�����Ƶ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 
	              BOOL32 bSending �Ƿ����ڷ���
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/02/24    3.0         ������           ����
====================================================================*/
void TConfMtTable::SetMtVideoSend( u8 byMtId, BOOL32 bSending )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	m_atMtData[byMtId - 1].m_tMtStatus.SetSendVideo( bSending );

	return;
}

/*====================================================================
    ������      : SetMtAudioSend
    ����        �������ն��Ƿ����ڷ�����Ƶ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 
	              BOOL32 bSending �Ƿ����ڷ���
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/02/24    3.0         ������           ����
====================================================================*/
void TConfMtTable::SetMtAudioSend( u8 byMtId, BOOL32 bSending )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	//�ƴ��ն�״̬���ն��ϱ�����������
	/*if( m_atMtExt[byMtId-1].GetManuId() == MT_MANU_KDC )
	{
		return;
	}*/

	m_atMtData[ byMtId - 1].m_tMtStatus.SetSendAudio( bSending );

	return;
}

/*====================================================================
    ������      : SetMtVideoRecv
    ����        �������ն��Ƿ����ڽ�����Ƶ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 
				  BOOL32 bRecving �Ƿ����ڽ���
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/02/24    3.0         ������           ����
====================================================================*/
void TConfMtTable::SetMtVideoRecv( u8 byMtId, BOOL32 bRecving )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	m_atMtData[ byMtId - 1].m_tMtStatus.SetReceiveVideo( bRecving );

	return;
}


/*====================================================================
    ������      : SetMtAudioRecv
    ����        �������ն��Ƿ����ڽ�����Ƶ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 
				  BOOL32 bRecving �Ƿ����ڽ���
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/02/24    3.0         ������           ����
====================================================================*/
void TConfMtTable::SetMtAudioRecv( u8 byMtId, BOOL32 bRecving )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	m_atMtData[ byMtId - 1].m_tMtStatus.SetReceiveAudio( bRecving );

	return;
}

/*====================================================================
    ������      : IsMtVideoSending
    ����        ���ն��Ƿ����ڷ�����Ƶ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 		
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/17    3.6         Jason        ����
====================================================================*/
BOOL32 TConfMtTable::IsMtAudioRecv( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

	return m_atMtData[ byMtId - 1].m_tMtStatus.IsReceiveAudio();
}

/*====================================================================
    ������      : IsMtAudioMute
    ����        ���ն��Ƿ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 		
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/17    3.6         Jason        ����
====================================================================*/
BOOL32 TConfMtTable::IsMtAudioMute( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

	return m_atMtData[ byMtId - 1].m_tMtStatus.IsDecoderMute();
}


/*====================================================================
    ������      : IsMtAudioDumb
    ����        ���ն��Ƿ�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 		
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/17    3.6         Jason        ����
====================================================================*/
BOOL32 TConfMtTable::IsMtAudioDumb( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

	return m_atMtData[ byMtId - 1].m_tMtStatus.IsCaptureMute();
}



/*====================================================================
    ������      : SetMtVideo2Send
    ����        �������ն��Ƿ����ڷ��͵ڶ���Ƶ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 
	              BOOL32 bSending �Ƿ����ڷ���
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/02/24    3.0         ������           ����
====================================================================*/
void TConfMtTable::SetMtVideo2Send( u8 byMtId, BOOL32 bSending )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	m_atMtData[byMtId - 1].m_tMtStatus.SetSndVideo2( bSending );
}

/*====================================================================
    ������      : SetMtVideo2Recv
    ����        �������ն��Ƿ����ڽ��յڶ���Ƶ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 
	              BOOL32 bSending �Ƿ����ڷ���
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/02/24    3.0         ������           ����
====================================================================*/
void TConfMtTable::SetMtVideo2Recv( u8 byMtId, BOOL32 bSending )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	m_atMtData[byMtId - 1].m_tMtStatus.SetRcvVideo2( bSending );
}
/*====================================================================
    ������      : SetMtInMixing
    ����        �������ն��Ƿ����ڻ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 
				  BOOL32 bMtInMixing �Ƿ����ڻ���
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/02/24    3.0         ������           ����
====================================================================*/
void TConfMtTable::SetMtInMixing( u8 byMtId, BOOL32 bMtInMixing )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	//�ƴ��ն�״̬���ն��ϱ�����������
	/*if( m_atMtExt[byMtId-1].GetManuId() == MT_MANU_KDC )
	{
		return;
	}*/

	m_atMtData[ byMtId - 1].m_tMtStatus.SetInMixing( bMtInMixing );

	return;
}

/*====================================================================
    ������      : SetMtInTvWall
    ����        �������ն��Ƿ����ڵ���ǽ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 
				  BOOL32 bMtInTvWall �Ƿ����ڵ���ǽ��
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/08/12    4.0         libo            ����
====================================================================*/
void TConfMtTable::SetMtInTvWall(u8 byMtId, BOOL32 bMtInTvWall)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return;
    }

    m_atMtData[byMtId - 1].m_tMtStatus.SetInTvWall(bMtInTvWall);
}

/*====================================================================
    ������      : IsMtInTvWall
    ����        ���ն��Ƿ����ڵ���ǽ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/08/12    4.0         libo            ����
====================================================================*/
BOOL32 TConfMtTable::IsMtInTvWall(u8 byMtId)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return FALSE;
    }

    return m_atMtData[byMtId - 1].m_tMtStatus.IsInTvWall();
}

/*====================================================================
    ������      : SetMtInHdu
    ����        ���ն��Ƿ����ڵ���ǽ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/08/12    4.0         libo            ����
====================================================================*/
void TConfMtTable::SetMtInHdu(u8 byMtId, BOOL32 bMtInHdu /*= TRUE*/)
{
	if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return;
    }
	
    m_atMtData[byMtId - 1].m_tMtStatus.SetInHdu(bMtInHdu);

	return;
}

/*====================================================================
    ������      : IsMtInHdu
    ����        ���ն��Ƿ����ڵ���ǽ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/08/12    4.0         libo            ����
====================================================================*/
BOOL32 TConfMtTable::IsMtInHdu(u8 byMtId)
{
	if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return FALSE;
    }
	
    return m_atMtData[byMtId - 1].m_tMtStatus.IsInHdu();
}

/*====================================================================
    ������      : SetMtIsMaster
    ����        �������ն��Ƿ����ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 
				  BOOL32 bMaster �Ƿ����ն�
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/14    3.6        Jason           ����
====================================================================*/
void TConfMtTable::SetMtIsMaster( u8 byMtId, BOOL32 bMaster )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	m_atMtData[ byMtId - 1].m_bMaster = bMaster;

	return;
}

/*====================================================================
    ������      : IsMtIsMaster
    ����        ���ն��Ƿ������ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺� 		
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/14    3.6         Jason           ����
====================================================================*/
BOOL32 TConfMtTable::IsMtIsMaster( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

	return m_atMtData[ byMtId - 1].m_bMaster;
}


/*====================================================================
    ������      : SetCurrVidSrcNo
    ����        �����õ�ǰ��ƵԴ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
                  u8 byCurrVidNo, ��ǰ��ƵԴ��
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/04/05    3.6         libo           ����
====================================================================*/
void TConfMtTable::SetCurrVidSrcNo(u8 byMtId, u8 byCurrVidNo)
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	m_atMtData[byMtId - 1].m_byCurrVidNo = byCurrVidNo;
}

/*====================================================================
    ������      : GetCurrVidSrcNo
    ����        ����ȡ��ǰ��ƵԴ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  : ���ն˵ĵ�ǰ��ƵԴ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/04/05    3.6         libo           ����
====================================================================*/
u8 TConfMtTable::GetCurrVidSrcNo(u8 byMtId)
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}

	return m_atMtData[byMtId - 1].m_byCurrVidNo;
}

/*====================================================================
    ������      : SetHWVerID
    ����        �������ն�Ӳ���汾��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  : 
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/12/18    4.5         �ű���           ����
====================================================================*/
void TConfMtTable::SetHWVerID(u8 byMtId, u8 byHWVer)
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return;
    }
    m_atMtExt2[byMtId-1].SetHWVerId(byHWVer);
    return;
}

/*====================================================================
    ������      : GetHWVer
    ����        ����ȡ�ն�Ӳ���汾��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  : u8
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/12/18    4.5         �ű���           ����
====================================================================*/
u8 TConfMtTable::GetHWVerID(u8 byMtId) const
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return 0;
    }
    return m_atMtExt2[byMtId-1].GetHWVerId();
}

/*====================================================================
    ������      : SetSWVerID
    ����        �������ն�����汾��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
                  LPCSTR lpszSWVer
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/12/18    4.5         �ű���           ����
====================================================================*/
void TConfMtTable::SetSWVerID(u8 byMtId, LPCSTR lpszSWVer)
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return;
    }
    m_atMtExt2[byMtId-1].SetSWVerId(lpszSWVer);
    return;
}

/*====================================================================
    ������      : GetSWVerID
    ����        ����ȡ�ն�����汾��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  : LPCSTR
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/12/18    4.5         �ű���           ����
====================================================================*/
LPCSTR TConfMtTable::GetSWVerID(u8 byMtId) const
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return NULL;
    }
    return m_atMtExt2[byMtId-1].GetSWVerId();
}

/*====================================================================
    ������      ��AddSwitchTable
    ����        ������Mp������Ϣ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����byMpId Mp���
	              ptSwitchChannel �����ŵ�
    ����ֵ˵��  ���ɹ�TRUE,ʧ��FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/07    3.0         ������        ����
====================================================================*/
BOOL32 TConfSwitchTable::AddSwitchTable(u8 byMpId,TSwitchChannel *ptSwitchChannel)
{
	if( NULL == ptSwitchChannel )
	{
		return FALSE;
	}

	if (byMpId > MAXNUM_DRI || 0 == byMpId)
	{
		OspPrintf(TRUE, FALSE, "[AddSwitchTable] invalid byMpId<%d> received!!!\n", byMpId);
		return FALSE;
	}
    
#ifdef _MINIMCU_
    if (byMpId > 1)
    {
        byMpId = 2;
    }
#endif

    //��ԭ���˽�����Ϣ�Ƴ�ʧ�ܣ�����ԭ����Ϣ
    BOOL32 bFind = FALSE;
    u16 wTableId = 0;
    while( wTableId < MAX_SWITCH_CHANNEL )
    {
        if(m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableId] == *ptSwitchChannel)
        {
            bFind = TRUE;
            break;
        }        
        wTableId++;
    }
	
    if(!bFind)
    {
        //����±���
        wTableId = 0;
        while( wTableId < MAX_SWITCH_CHANNEL )
        {
            if( m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableId].IsNull() )
                break;
            wTableId++;
        }
    }	

    if( wTableId < MAX_SWITCH_CHANNEL )
	{
		m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableId] = *ptSwitchChannel;
        return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*====================================================================
    ������      ��RemoveSwitchTable
    ����        ���Ƴ�Mp������Ϣ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����byMpId Mp���
	              ptSwitchChannel �����ŵ�
    ����ֵ˵��  ���ɹ�TRUE,ʧ��FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/07    3.0         ������        ����
====================================================================*/
BOOL32 TConfSwitchTable::RemoveSwitchTable(u8 byMpId,TSwitchChannel *ptSwitchChannel)
{
	if( NULL == ptSwitchChannel )
	{
		return FALSE;
	}

	if (byMpId > MAXNUM_DRI || 0 == byMpId)
	{
		OspPrintf(TRUE, FALSE, "[RemoveSwitchTable] invalid byMpId<%d> received!!!\n", byMpId);
		return FALSE;
	}

	//�Ƴ�����
	TSwitchChannel tSwitchChannel = *ptSwitchChannel;
	if( ptSwitchChannel->IsNull() )
	{
		return FALSE;
	}

#ifdef _MINIMCU_
    if (byMpId > 1)
    {
        byMpId = 2;
    }
#endif

	u16 wTableId = 0;
	while(wTableId<MAX_SWITCH_CHANNEL)
	{
		if( m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableId].GetDstIP() == tSwitchChannel.GetDstIP() && 
			m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableId].GetDstPort() == tSwitchChannel.GetDstPort() )
        {
            break;
        }
		else
		{
			wTableId++;
		}
	}
	
	if(wTableId<MAX_SWITCH_CHANNEL)
	{
		ptSwitchChannel->SetRcvPort( m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableId].GetRcvPort() );
		ptSwitchChannel->SetSrcMt( m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableId].GetSrcMt() );
		m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableId].SetNull();
        return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*=============================================================================
�� �� ���� ClearSwitchTable
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byMpId
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/14  4.0			������                  ����
=============================================================================*/
void TConfSwitchTable::ClearSwitchTable(u8 byMpId)
{
    if (0 == byMpId || byMpId > MAXNUM_DRI)
    {
        return;
    }

#ifdef _MINIMCU_
    if (byMpId > 1)
    {
        byMpId = 2;
    }
#endif
    memset(&m_tSwitchTable[byMpId-1], 0, sizeof(TSwitchTable));
}

/*====================================================================
    ������      ��ProcMultiToOneSwitch
    ����        �������㵽һ��Ľ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����ptSwitchChannel �����ŵ�
				  u8 byAct 1-add 2-remove 3-stop
    ����ֵ˵��  ���ɹ�TRUE,ʧ��FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/11/07    3.0         ������        ����
====================================================================*/
void TConfSwitchTable::ProcMultiToOneSwitch( TSwitchChannel *ptSwitchChannel, u8 byAct )
{
	u8 byIdleSlot = 7;
#ifdef _MINIMCU_
    byIdleSlot = 1;
#endif
	switch( byAct ) 
	{
	case 1://����
		{
			u16 wTableId = 0;
			while( wTableId < MAX_SWITCH_CHANNEL )
			{
				if( m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].IsNull() )
				{
					break;
				}
				else
				{
					wTableId++;
				}
			}

			if( wTableId < MAX_SWITCH_CHANNEL )
			{
				m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId] = *ptSwitchChannel;
			}
		}
		break;

	case 2://�Ƴ�
		{
			//�Ƴ�����
			TSwitchChannel tSwitchChannel = *ptSwitchChannel;
			u16 wTableId = 0;
			while(wTableId<MAX_SWITCH_CHANNEL)
			{
				if( m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].GetRcvIP() == tSwitchChannel.GetRcvIP() && 
					m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].GetRcvPort() == tSwitchChannel.GetRcvPort() && 
					m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].GetDstIP() == tSwitchChannel.GetDstIP() && 
					m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].GetDstPort() == tSwitchChannel.GetDstPort() )
				{
					m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].SetNull();
				}

				wTableId++;
			}
		}
		break;

	case 3://ֹͣ
		{
			//�Ƴ�����
			TSwitchChannel tSwitchChannel = *ptSwitchChannel;
			u16 wTableId = 0;
			while(wTableId<MAX_SWITCH_CHANNEL)
			{
				if( m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].GetDstIP() == tSwitchChannel.GetDstIP() && 
					m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].GetDstPort() == tSwitchChannel.GetDstPort() )
				{
					m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].SetNull();
				}
				
				wTableId++;		
			}
		}
		break;
		
	default:
		break;
	}

	return;
}

/*====================================================================
    ������      ��ProcBrdSwitch
    ����        ������㲥����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����byNum ����ͨ������
                  ptSwitchChannel �����ŵ�ָ��
				  u8 byAct 1-add brdsrc  2-remove brdsrc
                           11-add brddst
                  u8 byMpId Ŀ��MpId
    ����ֵ˵��  ���ɹ�TRUE,ʧ��FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/03/27    4.0         ����        ����
====================================================================*/
void TConfSwitchTable::ProcBrdSwitch( u8 byNum, TSwitchChannel *ptSwitchChannel, u8 byAct, u8 byMpId )
{
    // guzh [3/29/2007] Ѱ��һ�����еĽ�������㲥Դ���㲥Ŀ�갴�ձ�׼��������
	u8 byIdleIdx = 0;
#ifndef _MINIMCU_
	byIdleIdx = ( g_cMcuAgent.GetMpcBoardId() == MCU_BOARD_MPC ? MCU_BOARD_MPCD : MCU_BOARD_MPC );  
    byIdleIdx --;
#else
	byIdleIdx = 0;
#endif
    

    TSwitchChannel tSwitchChannel;

    if (0 == byNum || ptSwitchChannel == NULL)
        return;
    if (0 == byMpId)
        return;
    
    switch( byAct ) 
    {
    case 1://���ӹ㲥Դ
        {
            tSwitchChannel = *ptSwitchChannel;

            // �������ӹ㲥Դ��Ϣ�ǹ㲥������ֻ�����MT�����MP�Ļ�Ӧ
            u32 dwMpIp;
            u32 dwSrcIp;
            u16 wMpRecvPort;
            if ( ! g_cMpManager.GetSwitchInfo(tSwitchChannel.GetSrcMt(), dwMpIp, wMpRecvPort, dwSrcIp) )
            {
                return;
            }

            if ( byMpId != g_cMcuVcApp.FindMp( dwMpIp ) )
            {
                break;
            }

            u16 wTableId = 0;
            while( wTableId < MAX_SWITCH_CHANNEL )
            {
                if( m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wTableId].IsNull() )
                {
                    break;
                }
                else
                {
                    wTableId++;
                }
            }
            
            if( wTableId < MAX_SWITCH_CHANNEL )
            {
                // ���Ϊ�㲥Դ                
                tSwitchChannel.SetDstIP(0xffffffff);
                tSwitchChannel.SetDstPort(0x0);
                m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wTableId] = tSwitchChannel;
			}
            else
            {
                OspPrintf(TRUE, FALSE, "[ProcBrdSwitch] Add video broadcast source to switch table failed!\n");
            }
        }
        break;
        
    case 2://�Ƴ��㲥Դ
        {
            tSwitchChannel = *ptSwitchChannel;
            // ����ɾ���㲥Դ��Ϣ�ǹ㲥������ֻ�����MT�����MP�Ļ�Ӧ
            u32 dwMpIp;
            u32 dwSrcIp;
            u16 wMpRecvPort;
            if ( ! g_cMpManager.GetSwitchInfo(tSwitchChannel.GetSrcMt(), dwMpIp, wMpRecvPort, dwSrcIp) )
            {
                return;
            }
            
            if ( byMpId != g_cMcuVcApp.FindMp( dwMpIp ) )
            {
                break;
            }
            
            u16 wTableId = 0;
            BOOL32 bFound = FALSE;
            while(wTableId<MAX_SWITCH_CHANNEL)
            {
                if( m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wTableId].GetSrcIp() == tSwitchChannel.GetSrcIp() && 
                    m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wTableId].GetRcvPort() == tSwitchChannel.GetRcvPort() && 
                    m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wTableId].GetDstIP() == 0xffffffff && 
                    m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wTableId].GetDstPort() == 0x0 )
                {
                    m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wTableId].SetNull();
                    bFound = TRUE;
                    break;
                }
                
                wTableId++;
            }
            if ( !bFound )
            {
                // OspPrintf(TRUE, FALSE, "[ProcBrdSwitch] Remove video broadcast source to switch table failed!\n");
            }
        }
        break;
        
    case 11://���ӹ㲥Ŀ��
        {
#ifdef _MINIMCU_
			if (byMpId > 1)
			{
				byMpId = 2;
			}
#endif
			byIdleIdx =	byMpId - 1;

            u16 wTableId = 0;
            u8 bySwitchLoop = 0;
            u16 wInsertedPos;

            while ( bySwitchLoop < byNum )
            {
                tSwitchChannel = ptSwitchChannel[bySwitchLoop];

                //���Ȳ���
                wTableId = 0;
                wInsertedPos = MAX_SWITCH_CHANNEL;
                while ( wTableId<MAX_SWITCH_CHANNEL )
                {
                    if ( m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wTableId].GetDstIP() == tSwitchChannel.GetDstIP() &&
                         m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wTableId].GetDstPort() == tSwitchChannel.GetDstPort() )
                    {
                        // �ҵ�ԭ���ı��ֱ���滻
                        wInsertedPos = wTableId;
                        break;
                    }
                    else if ( wInsertedPos == MAX_SWITCH_CHANNEL &&
                              m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wTableId].IsNull() )
                    {
                        // ��¼�¿��еĲ����
                        wInsertedPos = wTableId;
                    }
                    wTableId ++;
                }

                if ( wInsertedPos != MAX_SWITCH_CHANNEL )
                {
                    tSwitchChannel.SetSrcIp(0xffffffff);
                    tSwitchChannel.SetRcvPort(0x0);
                    
                    m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wInsertedPos] = tSwitchChannel;                    
                }
                else
                {
                    OspPrintf(TRUE, FALSE, "[ProcBrdSwitch] m_tSwitchTable Mp.%d full, insert failed!\n", byMpId);
                    break;
                }
                bySwitchLoop ++;
            }
        }
        break;
        
    default:
        break;
    }
    
	return;
}

/*====================================================================
    ������      ��IsValidSwitchSrcIp
    ����        ���Ƿ�����Ч�Ľ���ԴIp
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u32 dwSrcIp ԴIp��ַ
                  u16 wRcvPort ת���˿�
                  u8 byMode ����Ƶģʽ
    ����ֵ˵��  ��TRUE-��,FALSE-��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	06/09/27    4.0         ������        ����
====================================================================*/
BOOL32 TConfSwitchTable::IsValidSwitchSrcIp( u32 dwSrcIp, u16 wRcvPort, u8 byMode )
{	
    u8 byMpId;
    for (u8 byLoop = 1; byLoop <= MAXNUM_DRI; byLoop++)
    {
        byMpId = byLoop;
        
        // guzh [3/29/2007] 8/16�ڲ�����������Ľ�����Ŀ
        if ( !g_cMcuVcApp.IsMpConnected(byMpId) &&
             byMpId != MCU_BOARD_MPC &&
             byMpId != MCU_BOARD_MPCD )
            continue;

#ifdef _MINIMCU_
        if (byLoop > 1)
        {
            byMpId = 2;
        }
#endif
        for(u16 wTableIdx = 0; wTableIdx < MAX_SWITCH_CHANNEL; wTableIdx++)
        {
            TSwitchChannel *ptSwitchChnnl = &m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableIdx];
            if (ptSwitchChnnl->IsNull())
                continue;
            
            if( ptSwitchChnnl->GetSrcIp() == dwSrcIp && ptSwitchChnnl->GetRcvPort() == wRcvPort )
            {
                if( (byMode == MODE_VIDEO && ptSwitchChnnl->GetRcvPort() % PORTSPAN == 0 ) ||
                    (byMode == MODE_AUDIO && ptSwitchChnnl->GetRcvPort() % PORTSPAN == 2) )
                {
                    if (ptSwitchChnnl->GetRcvPort() != ptSwitchChnnl->GetDstPort())
                    {
                        return TRUE;
                    }
                    else if (g_cMcuVcApp.FindMp(ptSwitchChnnl->GetDstIP()) == 0)
                    {
                        return TRUE;
                    }
                }
            }
            else if (ptSwitchChnnl->GetDstIP() == 0xffffffff && 
                     ptSwitchChnnl->GetDstPort() == 0 &&
                     byMode == MODE_VIDEO &&
                     //zbq[04/02/2008] �Ƿ�ǰԴ�����µĹ㲥
                     ptSwitchChnnl->GetSrcIp() == dwSrcIp )
            {
                return TRUE;
            }
        }
    }

    return FALSE;	
}

/*=============================================================================
�� �� ���� IsValidCommonSwitchSrcIp
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  u32 dwSrcIp
           u16 wRcvPort
           u8 byMode
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/5/23   4.0		    �ܹ��                  ����
=============================================================================*/
BOOL32 TConfSwitchTable::IsValidCommonSwitchSrcIp( u32 dwSrcIp, u16 wRcvPort, u8 byMode )
{
    u8 byMpId;
    for (u8 byLoop = 1; byLoop <= MAXNUM_DRI; byLoop++)
    {
        byMpId = byLoop;
        
        // guzh [3/29/2007] 8/16�ڲ�����������Ľ�����Ŀ
        if ( !g_cMcuVcApp.IsMpConnected(byMpId) &&
            byMpId != MCU_BOARD_MPC &&
            byMpId != MCU_BOARD_MPCD )
        {
            continue;
        }
        
#ifdef _MINIMCU_
        if (byLoop > 1)
        {
            byMpId = 2;
        }
#endif
        for(u16 wTableIdx = 0; wTableIdx < MAX_SWITCH_CHANNEL; wTableIdx++)
        {
            TSwitchChannel *ptSwitchChnnl = &m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableIdx];
            if (ptSwitchChnnl->IsNull())
            {
                continue;
            }
            
            if ( ptSwitchChnnl->GetSrcIp() != dwSrcIp || ptSwitchChnnl->GetRcvPort() != wRcvPort )
            {
                continue;
            }

            if( (byMode == MODE_VIDEO && ptSwitchChnnl->GetRcvPort() % PORTSPAN == 0 ) ||
                (byMode == MODE_AUDIO && ptSwitchChnnl->GetRcvPort() % PORTSPAN == 2) )
            {
                if (ptSwitchChnnl->GetRcvPort() != ptSwitchChnnl->GetDstPort() && 
                    // �����Ƿ�ֹ�ѹ㲥Դ��MP�Ľ�������Ϊ����ͨ����, zgc, 2008-08-27
                    ptSwitchChnnl->GetDstPort() != 0 )
                {
                    return TRUE;
                }
                else if (g_cMcuVcApp.FindMp(ptSwitchChnnl->GetDstIP()) == 0 &&
                         // �����Ƿ�ֹ�ѹ㲥Դ��MP�Ľ�������Ϊ����ͨ����, zgc, 2008-08-27
                         ptSwitchChnnl->GetDstIP() != 0xffffffff )
                {
                    return TRUE;
                }
            }
        }
    }
    
    return FALSE;	
}

/*=============================================================================
�� �� ���� IsValidBroaddSwitchSrcIp
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  u32 dwSrcIp
           u16 wRcvPort
           u8 byMode
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/5/23   4.0		    �ܹ��                  ����
=============================================================================*/
BOOL32 TConfSwitchTable::IsValidBroadSwitchSrcIp( u32 dwSrcIp, u16 wRcvPort, u8 byMode )
{
    u8 byMpId;
    for (u8 byLoop = 1; byLoop <= MAXNUM_DRI; byLoop++)
    {
        byMpId = byLoop;
        
        // guzh [3/29/2007] 8/16�ڲ�����������Ľ�����Ŀ
        if ( !g_cMcuVcApp.IsMpConnected(byMpId) &&
            byMpId != MCU_BOARD_MPC &&
            byMpId != MCU_BOARD_MPCD )
            continue;
        
#ifdef _MINIMCU_
        if (byLoop > 1)
        {
            byMpId = 2;
        }
#endif
        for(u16 wTableIdx = 0; wTableIdx < MAX_SWITCH_CHANNEL; wTableIdx++)
        {
            TSwitchChannel *ptSwitchChnnl = &m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableIdx];
            if (ptSwitchChnnl->IsNull())
            {
                continue;
            }
            
            if (ptSwitchChnnl->GetDstIP() == 0xffffffff && 
                ptSwitchChnnl->GetDstPort() == 0 &&
                byMode == MODE_VIDEO &&
                ptSwitchChnnl->GetSrcIp() == dwSrcIp )
            {
                return TRUE;
            }
        }
    }
    
    return FALSE;	
}

/*====================================================================
    ������      : McuGetDebugKeyValue
    ����        : ��ȡMCU Debug�ļ�key value
    �㷨ʵ��    :����ѡ�
    ����ȫ�ֱ���: ��
    �������˵��: TMcuDebugVal &tMcuDebugVal: MCU Debug ��Ϣ�ṹ
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/13    3.0         ������       ����
====================================================================*/
void McuGetDebugKeyValue(TMcuDebugVal &tMcuDebugVal)
{
	s32 nKeyVal;
	s8  achFullName[64];

	sprintf(achFullName, "%s/%s", DIR_CONFIG, MCUDEBUGFILENAME);
	GetRegKeyInt( achFullName, "mcuNetWork", "mcuListenPort", MCU_LISTEN_PORT, &nKeyVal );
	tMcuDebugVal.m_wMcuListenPort = (u16)nKeyVal;
	GetRegKeyInt( achFullName, "mcuNetWork", "mcuTelnetPort", MCU_TELNET_PORT, &nKeyVal );
	tMcuDebugVal.m_wMcuTelnetPort = (u16)nKeyVal;
    
	GetRegKeyInt( achFullName, "refreshInterval", "mcsRefreshInterval", 3, &nKeyVal );
	tMcuDebugVal.m_dwMcsRefreshInterval = (u32)nKeyVal;

	GetRegKeyInt( achFullName, "mcuWatchDog", "mcuWatchDogEnable", 1, &nKeyVal );
	tMcuDebugVal.m_bWDEnable = (1 == nKeyVal) ? TRUE : FALSE;

#ifdef WIN32
	tMcuDebugVal.m_bMSDetermineType = FALSE;
#else
	GetRegKeyInt( achFullName, "mcuMSExchangeCap", "mcuMSDetermineType", 1, &nKeyVal );
	tMcuDebugVal.m_bMSDetermineType = (1 == nKeyVal) ? TRUE : FALSE;
#endif
	
	// ��̬ʱ����ͬ��ʱ��, ����Χ��5���ӵ�60����
	GetRegKeyInt( achFullName, "mcuMSExchangeCap", "mcuMSSynTime", MS_SYN_MAX_SPAN_TIMEOUT, &nKeyVal);
	if( nKeyVal < MS_SYN_MAX_SPAN_TIMEOUT || nKeyVal > MS_SYN_MAX_SPAN_TIMEOUT*12 )
	{
		nKeyVal = MS_SYN_MAX_SPAN_TIMEOUT;
	}
	tMcuDebugVal.m_wMsSynTime = (u16)nKeyVal;
    
    GetRegKeyInt(achFullName, "BitrateAdapterParam", "BitrateScale", 0, &nKeyVal);    
    tMcuDebugVal.m_byBitrateScale = nKeyVal;
    
    GetRegKeyInt(achFullName, "cascadeParam", "IsMMcuSpeaker", 0, &nKeyVal);
    tMcuDebugVal.m_byIsMMcuSpeaker = (0 != nKeyVal) ? 1 : 0;

    GetRegKeyInt(achFullName, "cascadeParam", "IsAutoDetectMMcuDupCallIn", 0, &nKeyVal);
    tMcuDebugVal.m_byIsAutoDetectMMcuDupCall = (0 != nKeyVal) ? 1 : 0;

    GetRegKeyInt(achFullName, "cascadeParam", "CascadeAlias", 0, &nKeyVal);
    tMcuDebugVal.m_byCascadeAliasType = (u8)nKeyVal;

    GetRegKeyInt(achFullName, "cascadeParam", "IsShowMMcuMtList", 1, &nKeyVal);
    tMcuDebugVal.m_byShowMMcuMtList = (u8)nKeyVal;
    
    // Ĭ�����Ӽ���SMcu�¼��˿�
    tMcuDebugVal.m_wSMcuCasPort = 3337;

	GetRegKeyInt(achFullName, "mcuGeneralParam", "IsApplyChairToZxMcu", 0, &nKeyVal);
	tMcuDebugVal.m_byIsApplyChairToZxMcu = (0 != nKeyVal) ? 1 : 0;

    // �Ƿ�ת���ն˶���Ϣ
    GetRegKeyInt(achFullName, "mcuGeneralParam", "IsTransmitMtShortMsg", 1, &nKeyVal);
	tMcuDebugVal.m_byIsTransmitMtShortMsg = (0 != nKeyVal) ? 1 : 0;

    //�Ƿ��ն�������ϯ�����˵���ʾ��Ϣ������ϯ�ն�
    GetRegKeyInt(achFullName, "mcuGeneralParam", "IsChairDisplayMtApplyInfo", 1, &nKeyVal);
	tMcuDebugVal.m_byIsChairDisplayMtApplyInfo = (0 != nKeyVal) ? 1 : 0;
    
    //�Ƿ�����˫��ʽ����ѡ��
    GetRegKeyInt(achFullName, "mcuGeneralParam", "IsSelInDoubleMediaConf", 0, &nKeyVal);
    tMcuDebugVal.m_byIsSelInDoubleMediaConf = (0 != nKeyVal) ? 1 : 0;

    // guzh [4/30/2007] ���²�����Debug�ļ��в��ɼ�����������Ҫ��
#ifndef WIN32    
    // �Ƿ�����ն��ͺţ���Ʒ���ͣ����ƽ�������
    GetRegKeyInt(achFullName, "mcuGeneralParam", "IsNotLimitAccessByMtModal", 0, &nKeyVal);
    tMcuDebugVal.m_byIsNotLimitAccessByMtModal = (0 != nKeyVal) ? 1 : 0;
#else
    tMcuDebugVal.m_byIsNotLimitAccessByMtModal = 1;
#endif
    
    //�Ƿ�֧�ֵڶ�˫����������һ˫��ΪH239/264��
    GetRegKeyInt(achFullName, "mcuGeneralParam", "IsSupportSecDSCap", 0, &nKeyVal);
    tMcuDebugVal.m_byIsSupportSecDSCap = (u8)nKeyVal;

    //�Ƿ�����ͬһ�ն˽�HD-VMP���ͨ��
    GetRegKeyInt(achFullName, "mcuGeneralParam", "IsAllowVmpMemRepeated", 0, &nKeyVal);
    tMcuDebugVal.m_byIsAllowVmpMemRepeated = (u8)nKeyVal;

    //�Ƿ����ʹ�õ�ǰ��������Դ
    GetRegKeyInt(achFullName, "mcuGeneralParam", "IsAdpResourceCompact", 0, &nKeyVal);
    tMcuDebugVal.m_byIsAdpResourceCompact = (u8)nKeyVal;
    
    //MPU-SVMP�Ƿ�ǿ�Ʊ���1080i
    GetRegKeyInt(achFullName, "mcuGeneralParam", "IsSVmpOutput1080i", 0, &nKeyVal);
    tMcuDebugVal.m_byIsSVmpOutput1080i = (u8)nKeyVal;

    //�Ƿ�֧���չ��������
    GetRegKeyInt(achFullName, "mcuGeneralParam", "IsConfAdpManually", 1, &nKeyVal);
    tMcuDebugVal.m_byConfAdpManually = (u8)nKeyVal;
	

	//MCU�Ƿ�����MP
#ifndef WIN32
	GetRegKeyInt(achFullName, "mcuAbilityParam", "IsMpcRunMp", 0, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byIsMpcRunMp = (u8)nKeyVal;
#else
    // guzh [4/2/2007] Windows ȱʡ����MP
    GetRegKeyInt(achFullName, "mcuAbilityParam", "IsMpcRunMp", 1, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byIsMpcRunMp = (u8)nKeyVal;
#endif
	
	//MCU�Ƿ�����MtAdp, 8000B/C ��ͨ��debug����
#ifndef WIN32
	GetRegKeyInt(achFullName, "mcuAbilityParam", "IsMpcRunMtAdp", 0, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byIsMpcRunMtAdp = (u8)nKeyVal;
#else//Windows ȱʡ����MtAdp
	GetRegKeyInt(achFullName, "mcuAbilityParam", "IsMpcRunMtAdp", 1, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byIsMpcRunMtAdp = (u8)nKeyVal;
#endif

    //�Ƿ���������ת�����ת������
    GetRegKeyInt(achFullName, "mcuAbilityParam", "IsRestrictFlux", 1, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byIsRestrictFlux = (u8)nKeyVal;    

	//MCU�ϵ�MP���ת������
	GetRegKeyInt(achFullName, "mcuAbilityParam", "mpcMaxMpAbility", MAXLIMIT_MPC_MP, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_wMpcMaxMpAbility = (u16)nKeyVal;
	
	//CRI���ϵ�MP���ת������
	GetRegKeyInt(achFullName, "mcuAbilityParam", "criMaxMpAbility", MAXLIMIT_CRI_MP, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_wCriMaxMpAbility = (u16)nKeyVal;

	//�Ƿ�����MCU�ն��������ն˽�������
	GetRegKeyInt(achFullName, "mcuAbilityParam", "IsRestrictMtNum", 1, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byIsRestrictMtNum = (u8)nKeyVal;

	//MCU���ն���������ն˽�������
	GetRegKeyInt(achFullName, "mcuAbilityParam", "mpcMaxMtAdpConnMtAbility", MAXLIMIT_MPC_MTADP, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byMpcMaxMtAdpConnMtAbility = (u8)nKeyVal;

	//MCU���ն���������¼�MCU��������
	GetRegKeyInt(achFullName, "mcuAbilityParam", "mpcMaxMtAdpConnSMcuAbility", MAXLIMIT_MPC_MTADP_SMCU, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byMpcMaxMtAdpConnSMcuAbility = (u8)nKeyVal;


	//TUI�����ն���������ն˽�������
	GetRegKeyInt(achFullName, "mcuAbilityParam", "tuiMaxMtAdpConnMtAbility", MAXLIMIT_CRI_MTADP, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byTuiMaxMtAdpConnMtAbility = (u8)nKeyVal;

	//TUI�����ն���������¼�MCU��������
	GetRegKeyInt(achFullName, "mcuAbilityParam", "tuiMaxMtAdpConnSMcuAbility", MAXLIMIT_CRI_MTADP_SMCU, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byTuiMaxMtAdpConnSMcuAbility = (u8)nKeyVal;
    
    //HDI�����ն���������ն˽�������
    GetRegKeyInt(achFullName, "mcuAbilityParam", "hdiMaxMtAdpConnMtAbility", MAXLIMIT_HDI_MTADP, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byHdiMaxMtAdpConnMtAbility = (u8)nKeyVal;
    
    //�Ƿ�����polycom�ն˵ı��뷢�ͣ�����HD���飬��ӦHD-VMP�Ľ�����ޣ�
    GetRegKeyInt(achFullName, "mcuAbilityParam", "IsSendFakeCap2Polycom", 0, &nKeyVal);
    tMcuDebugVal.m_byIsSendFakeCap2Polycom = (u8)nKeyVal;

    //�Ƿ�����Taide�ն˵ı��뷢�ͣ�����HD���飬��Ӧtandberg�����ն˵������������ޣ�
    GetRegKeyInt(achFullName, "mcuAbilityParam", "IsSendFakeCap2Taide", 0, &nKeyVal);
    tMcuDebugVal.m_byIsSendFakeCap2Taide = (u8)nKeyVal;
    
    //�Ƿ���ǿTaide�ն˵ı��뷢�ͣ�����HD���飬��Ӧtangberg�����ն˵������������ޣ�
    GetRegKeyInt(achFullName, "mcuAbilityParam", "IsSendFakeCap2TaideHD", 0, &nKeyVal);
    tMcuDebugVal.m_byIsSendFakeCap2TaideHD = (u8)nKeyVal;
	
	//�Ƿ����ִ������ͱ�����ն��ֻ�
	GetRegKeyInt(achFullName, "mcuAbilityParam", "IsDistingtishSDHDMt", 0, &nKeyVal);
	tMcuDebugVal.m_byIsDistingtishSDHDMt = (u8)nKeyVal;

    //���Ȼ�ѡ�� polycom�Ƿ� ������򽵷ֱ���
	GetRegKeyInt(achFullName, "mcuAbilityParam", "IsVidAdjustless4Polycom", 0, &nKeyVal);
	tMcuDebugVal.m_byIsVidAdjustless4Polycom = (u8)nKeyVal;

    //ֱ��ѡ��ʧ�ܣ��Ƿ�ǿ��������ѡ��
    GetRegKeyInt(achFullName, "mcuAbilityParam", "IsSelAccord2Adp", 0, &nKeyVal);
    tMcuDebugVal.m_byIsSelAccord2Adp = (u8)nKeyVal;
    
    //�Ƿ�ΪHD-BAS������Ԥ������λ��Mbps��0����������Ԥ����
    GetRegKeyInt(achFullName, "mcuAbilityParam", "BandWidthReserved4HdBas", 0, &nKeyVal);
    tMcuDebugVal.m_byBandWidthReserved4HdBas = (u8)nKeyVal;

    //�Ƿ�ΪHD-Vmp������Ԥ������λ��Mbps��0����������Ԥ����
    GetRegKeyInt(achFullName, "mcuAbilityParam", "BandWidthReserved4HdVmp", 0, &nKeyVal);
    tMcuDebugVal.m_byBandWidthReserved4HdVmp = (u8)nKeyVal;

	return;
}

/*====================================================================
    ������      : McuGetSateliteConfigValue
    ����        : ��ȡMCU ���ǻ������ļ�key value
    �㷨ʵ��    :����ѡ�
    ����ȫ�ֱ���: ��
    �������˵��: TMcuSatInfo &tSatInfo
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/08/28    4.6         �ű���       ����
====================================================================*/
void McuGetSateliteConfigValue(TMcuSatInfo &tSatInfo)
{
    s32 nKeyVal;
    s8  achFullName[64];
    s8  achReturn[MAX_VALUE_LEN + 1];
    s8  achDefStr[] = "Cannot find the section or key";
    BOOL32 bResult = FALSE;

	sprintf(achFullName, "%s/%s", DIR_CONFIG, MCU_SAT_CFGFILENAME);

    /*�����������IP��ַ*/	
    bResult = GetRegKeyString( achFullName, "mcuSatNetWork", "mcunetApplyIpAddr", 
                               achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetApplyIpAddr" );
    }
    tSatInfo.SetApplyIp( ntohl( inet_addr( achReturn )));

    /*����������Ķ˿�*/
    bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mcunetApplyPort", 
                            0, &nKeyVal );
    if( bResult == FALSE )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetApplyPort" );
    }
    tSatInfo.SetApplyPort( (u16)nKeyVal );
    
    /*���ܻ�Ӧ�Ķ˿�*/
    bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mcunetApplyRcvPort", 0, &nKeyVal );
    if( bResult == FALSE )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetApplyRcvPort" );
    }
	tSatInfo.SetApplyRcvPort( (u16)nKeyVal );

    /*mcu�����ն���������˿�*/
    bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mcuRcvMtSignalPort", 0, &nKeyVal );
    if ( !bResult )
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcuRcvMtSignalPort" );
    }
    tSatInfo.SetRcvMtSignalPort( (u16)nKeyVal );

    /*ʱ���������ĵ�ַ*/
    bResult = GetRegKeyString( achFullName, "mcuSatNetWork", "mcunetTimeIpaddr", 
                               achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetTimeIpaddr" );
        return;
    }
    
    tSatInfo.SetTimeRefreshIpAddr( ntohl( inet_addr( achReturn ) ) );
    
    /*ʱ���������Ķ˿�*/
    bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mcunetTimePort", 
                            0, &nKeyVal );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetTimePort" );
        return;
    }
    tSatInfo.SetTImeRefreshPort( ( u16 )nKeyVal );
	

    /* mcuRcvMtMediaStartPort */
    bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mcuRcvMtMediaStartPort",
                            0, &nKeyVal );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcuRcvMtMediaStartPort" );
        return;
    }
    tSatInfo.SetMcuRcvMtMediaStartPort((u16)nKeyVal);
    
    
    /* mtRcvMcuMediaPort */
    bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mtRcvMcuMediaPort", 
                            0, &nKeyVal );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mtRcvMcuMediaPort" );
        return;
    }
	tSatInfo.SetMtRcvMcuMediaPort((u16)nKeyVal);

    // MCU�鲥����IP��ַ(224.0.0.0��239.255.255.255֮��)
    bResult = GetRegKeyString( achFullName, "mcuSatNetWork", "mcunetMulticastSignalIpAddr", 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetMulticastSignalIpAddr" );
        return;
    }
	tSatInfo.SetMcuMulitcastSignalIpAddr(ntohl( inet_addr( achReturn ) ));
    
    // MCU�鲥����˿�(������4��������)
    bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mcunetMulticastSignalPort", 
        0, &nKeyVal );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetMulticastSignalPort" );
        return;
    }
	tSatInfo.SetMcuMulticastSignalPort((u16)nKeyVal);
    
    // MCU�鲥IP��ַ(224.0.0.0��239.255.255.255֮��) 
    bResult = GetRegKeyString( achFullName, "mcuSatNetWork", "mcunetMulticastIpAddr", 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetMulticastIpAddr" );
        return;
    }
	tSatInfo.SetMcuMulticastDataIpAddr(ntohl( inet_addr( achReturn ) ));

    // MCU�鲥��ʼ�˿�(������4��������) 
    bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mcunetMulticastPort", 
        0, &nKeyVal );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetMulticastPort" );
        return;
    }
	tSatInfo.SetMcuMulticastDataPort((u16)nKeyVal);
	
    
    
    // MCU�鲥��ʼ�˿�(������4��������)
    bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mcunetMulticastPortCount", 
        0, &nKeyVal );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetMulticastPortCount" );
        return;
    }
	tSatInfo.SetMcuMulticastDataPortNum((u16)nKeyVal);
    
    // MCU�鲥TTLֵ
    bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mcunetMulticastTTL", 
        0, &nKeyVal );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetMulticastTTL" );
        return;
    }
	tSatInfo.SetMcuMulticastTTL((u8)nKeyVal);    
    
    // ����ϴ�·��
    bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mcunetMaxUploadCount", 
        0, &nKeyVal );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetMaxUploadCount" );
        return;
    }
	tSatInfo.SetMcuMaxUploadNum((u8)nKeyVal);

	// MCU�����ն�������ʼ�˿�
	bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mcuRcvMtMediaStartPort",
		0, &nKeyVal);
	if (!bResult)
	{
		OspPrintf(TRUE, FALSE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcuRcvMtMediaStartPort");
		return;
	}
	tSatInfo.SetMcuRcvMtMediaStartPort((u16)nKeyVal);

	// �ն˽���MCU������ʼ�˿�
	bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mtRcvMcuMediaPort",
		0, &nKeyVal);
	if (!bResult)
	{
		OspPrintf(TRUE, FALSE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mtRcvMcuMediaPort");
		return;
	}
	tSatInfo.SetMtRcvMcuMediaPort((u16)nKeyVal);

	// MCU�㲥���еĵڶ��鲥��ַ����Ҫ����˫��ƵĿ�Ķ����飬����ƵĿ����Ҫ��һ����չ��
    // (224.0.0.0��239.255.255.255֮��)
    bResult = GetRegKeyString( achFullName, "mcuSatNetWork", "mcunetSecMulticastIpAddr", 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetSecMulticastIpAddr" );
        return;
    }
	tSatInfo.SetMcuSecMulticastIpAddr(ntohl( inet_addr( achReturn ) ));

    return;
}

/*=============================================================================
    �� �� ���� McuGetMtCallInterfaceInfo
    ��    �ܣ� ��ȡMCU Debug�ļ�����ָ���ն˵�Э���������Դ������ת������Դ������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� TMtCallInterface *ptMtCallInterface
	           u32 &dwEntryNum
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/25   3.6			����                  ����
    2006/12/20  4.0-R4      ����                    ����E164�����֧��
=============================================================================*/
BOOL32 McuGetMtCallInterfaceInfo( TMtCallInterface *ptMtCallInterface, u32 &dwEntryNum )
{
	BOOL32  bRet = FALSE;
	
	if( NULL == ptMtCallInterface )
	{
		return bRet;
	}

	char achFullName[64];
    s8   pszE164Start[MAXLEN_E164] = {0};
    s8   pszE164End[MAXLEN_E164] = {0};
	s32  nEntryNum = 0;
	s8   chSeps[] = " \t"; //space or tab as seperators
	s8  *pchToken;
	u32  dwLoop = 0;
	u32  dwValidNum = 0;
	s8 **lpszTable;
	
	sprintf(achFullName, "%s/%s", DIR_CONFIG, MCUDEBUGFILENAME);
    
	//get the number of entry
	bRet = GetRegKeyInt( achFullName, "MtCallInterfaceTable", "EntryNum", 0, &nEntryNum );
	if( FALSE == bRet ) 
	{
		// OspPrintf( TRUE, FALSE, "McuGetMtCallInterfaceInfo Err while reading %s %s!\n", "MtCallInterfaceTable", "EntryNum" );
		return FALSE;
	}
	if( nEntryNum < 0 )
	{
		OspPrintf( TRUE, FALSE, "McuGetMtCallInterfaceInfo Err2 while reading %s %s!\n", "MtCallInterfaceTable", "EntryNum" );
		return FALSE;
	}

	if( nEntryNum > MAXNUM_MCU_MT )
	{
		nEntryNum = MAXNUM_MCU_MT;
	}
    
	if( nEntryNum <= (s32)dwEntryNum )
	{
		dwEntryNum = (u32)nEntryNum;
	}

	//alloc memory
	lpszTable = (s8 **)malloc( dwEntryNum * sizeof( s8* ) );
	for( dwLoop = 0; dwLoop < (u32)dwEntryNum; dwLoop++ )
	{
		lpszTable[dwLoop] = (s8 *)malloc( MAX_VALUE_LEN+1 );
	}
	
	//get the table string
	bRet = GetRegKeyStringTable( achFullName, "MtCallInterfaceTable", "fail", lpszTable, &dwEntryNum, (MAX_VALUE_LEN+1) );
	if( FALSE == bRet ) 
	{
		printf( "McuGetMtCallInterfaceInfo Err while reading %s table!\n", "MtCallInterfaceTable" );
		return bRet;
	}

	for( dwLoop = 0; dwLoop < dwEntryNum; dwLoop++ )
	{
        
        u32 dwStartNetIp = 0;
        u32 dwEndNetIp = 0;
        u8  byAliasType = mtAliasTypeOthers;
		pchToken = strtok( lpszTable[dwValidNum], chSeps );

		//CalledMtMtadpIPAddr		
		if( NULL == pchToken )
		{
			printf( "McuGetMtCallInterfaceInfo Err while reading %s entryID.%d!\n", "CalledMtMtadpIPAddr", dwLoop );
			continue;
		}
		else
		{
			ptMtCallInterface[dwValidNum].m_dwMtadpIpAddr = ntohl( INET_ADDR(pchToken) );
		}

		//CalledMtMPIPAddr
		pchToken = strtok( NULL, chSeps );
		if( NULL == pchToken )
		{
			printf( "McuGetMtCallInterfaceInfo Err while reading %s entryID.%d!\n", "CalledMtMPIPAddr", dwLoop );
			continue;
		}
		else
		{
			ptMtCallInterface[dwValidNum].m_dwMpIpAddr = ntohl( INET_ADDR(pchToken) );
		}

        pchToken = strtok( NULL, chSeps );
        // ��ʼIP������E164����
		if( NULL == pchToken )
		{
			printf( "McuGetMtCallInterfaceInfo Err while reading %s entryID.%d!\n", "StartIP", dwLoop );
			continue;
		}
		else
		{
			u8 byMtAliasType = GetMtAliasTypeFromString( pchToken );

            switch (byMtAliasType)
            {
            case mtAliasTypeTransportAddress:
                dwStartNetIp = INET_ADDR(pchToken); //Ҫ��������
                byAliasType = mtAliasTypeTransportAddress;
                //��������IP������ַ
                pchToken = strtok(NULL, chSeps);
                if( NULL == pchToken)
                {
			        printf("McuGetMtCallInterfaceInfo Err while reading %s entryID.%d!\n", "StartIp", dwLoop );
                    continue;
                }
                else
                {
                    byMtAliasType = GetMtAliasTypeFromString( pchToken );
                    if( mtAliasTypeTransportAddress != byMtAliasType )
                    {
                        printf( "McuGetMtCallInterfaceInfo Err Invalid IP while reading %s entryID.%d!\n", "EndIp", dwLoop );
                        continue;
                    }
                    dwEndNetIp = INET_ADDR(pchToken); //Ҫ��������
                    ptMtCallInterface[dwValidNum].SetIpSeg( dwStartNetIp, dwEndNetIp );
                }
                break;
            case mtAliasTypeE164:
                // ��ȡ�����ļ���E164����
                ptMtCallInterface[dwValidNum].SetE164Alias( pchToken );
                break;
            default:
                printf( "McuGetMtCallInterfaceInfo Err Invalid IP while reading %s entryID.%d!\n", "StartIP", dwLoop );
                continue;
            }
        }
        /* del by wangliang 2006-12-20 for E164 seg support start */
        /*
        if ( byAliasType == mtAliasTypeTransportAddress )
        {
            // ����IP
            pchToken = strtok( NULL, chSeps );
		    if( NULL == pchToken )
		    {
			    printf("McuGetMtCallInterfaceInfo Err while reading %s entryID.%d!\n", "EndIp", dwLoop );
			    continue;
		    }
		    else
		    {
			    u8 byMtAliasType = GetMtAliasTypeFromString( pchToken );
			    
			    if( mtAliasTypeTransportAddress != byMtAliasType )
			    {
				    printf( "McuGetMtCallInterfaceInfo Err Invalid IP while reading %s entryID.%d!\n", "EndIp", dwLoop );
				    continue;
			    }
                dwEndNetIp = INET_ADDR(pchToken); //Ҫ��������
                ptMtCallInterface[dwValidNum].SetIpSeg( dwStartNetIp, dwEndNetIp );
		    }            
        }
        */
        /* del by wangliang 2006-12-20 for E164 seg support end */
        
		dwValidNum++;
	}

	
	//free memory
	for( dwLoop = 0; dwLoop < dwEntryNum; dwLoop++ )
	{
		if( NULL != lpszTable[dwLoop] )
		{
			free( lpszTable[dwLoop] );
		}
	}
	free( lpszTable );
    
	dwEntryNum = dwValidNum;
	bRet = TRUE;
	return bRet;
}

/*=============================================================================
    �� �� ���� GetMtAliasTypeFromString
    ��    �ܣ� �Ӵ��б���ִ��б��ն�����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� s8* pszAliasString ���б���ִ�
    �� �� ֵ�� u8 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/25   3.6			����                  ����
=============================================================================*/
u8 GetMtAliasTypeFromString(s8* pszAliasString)
{
	if(IsIPTypeFromString(pszAliasString))
	{	
		//ip
		return mtAliasTypeTransportAddress;
	}
	else if(IsE164TypeFromString(pszAliasString,"0123456789*,#"))
	{
		//e164
		return mtAliasTypeE164;
	}
	else
	{
		//h323
		return mtAliasTypeH323ID;
	}
	
	return mtAliasTypeOthers;
}

/*=============================================================================
    �� �� ���� IsIPTypeFromString
    ��    �ܣ� �Ӵ��б���ִ��б��Ƿ�Ϊip�ִ�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� s8* pszAliasString ���б���ִ�
    �� �� ֵ�� BOOL32
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/25   3.6			����                  ����
=============================================================================*/
BOOL32 IsIPTypeFromString(s8* pszAliasString)
{
	s32 anValue[4] = {0};
	s32 anLen[4] = {0};
	s32 nDot   = 0;
	s8 *pszTmp = pszAliasString;
	s32 nPos   = 0;
	for(nPos=0; *pszTmp&&nPos<16; nPos++,pszTmp++)
	{
		if( '.' == *pszTmp )
		{
			nDot++;
			//excude 1256.1.1.1.1
			if(nDot > 3)
			{
				return FALSE;
			}
			continue;			
		}
		//excude a.1.1.1
		if( *pszTmp<'0'|| *pszTmp>'9' )
		{
			return FALSE;
		}
		
		anValue[nDot] = anValue[nDot]*10 + (*pszTmp-'0');
		anLen[nDot]++;
	}
	
	//excude 1256.1.1.1234444
	if( nPos >=16 )
	{
		return FALSE;
	}
	
	//excude 0.1.1.1
	if( 0 == anValue[0] )
	{
		return FALSE;
	}
	
	for(nPos=0; nPos<4; nPos++)
	{
		//excude 1256.1.1.1
		if( (0 == anLen[nPos]) || (anLen[nPos] > 3) )
		{
			return FALSE;
		}
		//excude 256.1.1.1
		if(anValue[nPos] > 255)
		{
			return FALSE;
		}
	}

	return TRUE;
}

/*=============================================================================
    �� �� ���� IsIPTypeFromString
    ��    �ܣ� �Ӵ��б���ִ��б��Ƿ�ΪE164�ִ�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� s8* pszAliasString ���б���ִ�
    �� �� ֵ�� BOOL32
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/25   3.6			����                  ����
=============================================================================*/
BOOL32 IsE164TypeFromString(s8 *pszSrc, s8* pszDst)
{
	s8 *pszSrcTmp = pszSrc;
	s8 *pszDstTmp = pszDst;
	while(*pszSrcTmp)
	{
		pszDstTmp = pszDst;
		while(*pszDstTmp)
		{
			if(*pszSrcTmp == *pszDstTmp) 
			{
				break;
			}
			*pszDstTmp++;
		}
		if( 0 == *pszDstTmp )
		{
			return FALSE;
		}
		pszSrcTmp++;
	}

	return TRUE;
}

/*=============================================================================
�� �� ���� IsVidFormatHD
��    �ܣ� �Ƿ�Ϊ����ֱ���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/12/19  4.5			�ű���                  ����
=============================================================================*/
BOOL32 IsVidFormatHD(u8 byVidFormat)
{
    BOOL32 bFormatHD = FALSE;
    switch( byVidFormat )
    {
    case VIDEO_FORMAT_W4CIF:
    case VIDEO_FORMAT_HD720:
    case VIDEO_FORMAT_SXGA:
    case VIDEO_FORMAT_UXGA:
    case VIDEO_FORMAT_HD1080:
        bFormatHD = TRUE;
        break;
    default:
        break;
    }
    return bFormatHD;
}

/*
    ���û�����Ϣ�ļ���д����: һ�������ģ����Ϣ��Ӧһ���ļ���: confinfo_index.dat 
                              (���� index <- [0, MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE-1])
                              ����ͷ��Ϣ��ȱʡ������Ϣ��Ӧһ���������ļ���: confinfo_head.dat
                              (ȱʡ���� index == MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLAT)

    ���ڴ洢������Ϣ���ļ��ṹΪ��
    confinfo_head.dat CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1] + 
	                  TPackConfStore + m_wAliasBufLen�ֽ�m_pbyAliasBuf + TTvWallModule(opt) + TVmpModule(opt);
    confinfo_pos.dat  TPackConfStore + m_wAliasBufLen�ֽ�m_pbyAliasBuf + TTvWallModule(opt) + TVmpModule(opt);

    ʵ����Ч�Ĵ洢������Ϊ (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE)�����ģ�� + һ��ȱʡ����
*/

/*
    guzh [4/3/2007]
    Ϊ�˱����дȱʡ���鵼�»���ͷ��Ϣд��,��ȱʡ������Ϣ���浽confinfo_128(32).dat��,
    ���ٱ����� confinfo_head.dat ��.
*/


/*====================================================================
    ������      : BeginRWConfInfoFile
    ����        : ׼����д�����ļ���Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���: ��
    �������˵��: ��
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/04/03    4.0         ����         ����
====================================================================*/
BOOL32 BeginRWConfInfoFile()
{
    if (OspSemTakeByTime(g_hConfInfoRW, 15000))
    {
        return TRUE;
    }
    else
    {
        printf("[BeginRWConfInfoFile] OspSemTakeByTime failed!\n");
        return FALSE;
    }
}

/*====================================================================
    ������      : EndRWConfInfoFile
    ����        : ������д�����ļ���Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���: ��
    �������˵��: ��
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    07/04/03    4.0         ����         ����
====================================================================*/
void EndRWConfInfoFile()
{
    OspSemGive(g_hConfInfoRW);
}

/*====================================================================
    ������      : CreateConfStoreFile
    ����        : ���޴洢�ļ��������¹�������ļ�(������ֻ�ܵ���һ��)
    �㷨ʵ��    :
    ����ȫ�ֱ���: ��
    �������˵��: ��
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/31    3.0         ������         ����
    07/04/03    4.0         ����         �޸�
====================================================================*/
BOOL32 CreateConfStoreFile(void)
{
	//check data files' path
#if defined(WIN32)
	CreateDirectory(DIR_DATA, NULL);
#elif defined(_VXWORKS_)
	mkdir((LPSTR)DIR_DATA);
#elif defined(_LINUX_)
    mkdir((LPSTR)DIR_DATA, 0777);
    chmod((LPSTR)DIR_DATA, 0777);
#endif

    // �����ļ���д�ź���
    if (NULL != g_hConfInfoRW)
    {
        OspSemDelete( g_hConfInfoRW );
        g_hConfInfoRW = NULL;
    }
    if( !OspSemBCreate( &g_hConfInfoRW ) )
    {
        OspSemDelete( g_hConfInfoRW );
        g_hConfInfoRW = NULL;
        printf("[CreateConfStoreFile] create conf info readwrite lock failed!\n" );
        return FALSE;
    }
	
	FILE *hFile = NULL;
	s8    achFullName[64];
    if ( ! BeginRWConfInfoFile() )
    {
        return FALSE;
    }

	//��ȡ ����ͷ��Ϣ_�洢�ļ�
	sprintf(achFullName, "%s/%s", DIR_DATA, CONFINFOFILENAME_HEADINFO);
	hFile = fopen(achFullName, "rb"); 
	if (NULL == hFile)
	{
		hFile = fopen(achFullName, "wb");
		if (NULL != hFile)
		{
			//д����ͷ �ļ���Ϣ
            CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
            memset(acConfId, 0, sizeof(acConfId));
			fwrite(acConfId, sizeof(acConfId), 1, hFile);
		}
	}
	if (NULL != hFile)
	{
		fclose(hFile);
		hFile = NULL;
	}

    EndRWConfInfoFile();

	//�����ģ����Ϣ_�洢�ļ�������Ԥ������
	/*
	for (s32 nPos = 0; nPos < (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE); nPos++)
	{
		//��ȡ �����ģ����Ϣ_�洢�ļ�
		sprintf(achFullName, "%s/%s%d%s", DIR_DATA, CONFINFOFILENAME_PREFIX, nPos, CONFINFOFILENAME_POSTFIX);
		hFile = fopen(achFullName, "r+b"); 
		if (NULL == hFile)
		{
			hFile = fopen(achFullName, "w+b");
			if (NULL != hFile)
			{
				//д�����ģ�� �ļ���Ϣ
				fwrite(&tNullConfStore, sizeof(tNullConfStore), 1, hFile);
			}
		}
		if (NULL != hFile)
		{
			fclose(hFile);
			hFile = NULL;
		}
	}
	*/

    return TRUE;
}

/*====================================================================
    ������      : AddConfToFile
    ����        : ��ָ���Ļ����¼�洢���ļ��д���
    �㷨ʵ��    :����ѡ�
    ����ȫ�ֱ���: ��
    �������˵��: TConfStore &tConfStore    ���洢�Ļ�����Ϣ(δ����Pack����Ļ�������)
	              BOOL32 bDefaultConf = FALSE �Ƿ��Ϊȱʡ����(����ǣ���ǿ�Ƹ��ǵ�ǰ��ȱʡ����
    ����ֵ˵��  : BOOL32
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/13    3.0         ������          ����
====================================================================*/
BOOL32 AddConfToFile( TConfStore &tConfStore, BOOL32 bDefaultConf )
{
	FILE *hHeadFile = NULL;
	FILE *hConfFile = NULL;

	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
	u8 byConfPos = MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE;
	TConfStore tConfStoreBuf;
	TPackConfStore *ptPackConfStore = (TPackConfStore *)&tConfStoreBuf;
	u16 wPackConfDataLen = 0;
	CConfId cDefConfConfId;
	CConfId cConfConfId;
	memset(acConfId, 0, sizeof(acConfId));
	cDefConfConfId.SetNull();

	//1.��ͷ��Ϣ��¼�� ��ȡ �����ģ������λ��
	GetAllConfHeadFromFile(acConfId, sizeof(acConfId));

	//�������л��飺�˻����ѱ��棬���ǣ�������ȱʡ����λ��
	for (s32 nPos = 0; nPos < (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE); nPos++)
	{
		if (acConfId[nPos] == tConfStore.m_tConfInfo.GetConfId())
		{
			byConfPos = (u8)nPos;
			break;
		}
	}
	//δ�棬��һ����λ
	if (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE == byConfPos)
	{
		byConfPos = 0;
		while (!acConfId[byConfPos].IsNull() && byConfPos<(MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE))
		{
			byConfPos++;
		}
	}
	//����
	if (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE == byConfPos) 
	{
		return FALSE;
	}

	//2.�� δ����Pack����Ļ������� ����Pack����
	if (FALSE == PackConfStore(tConfStore, ptPackConfStore, wPackConfDataLen))
	{
		return FALSE;
	}

	//3.���� �û����ģ����Ϣ ��ָ�������Ĵ洢�ļ�
	cConfConfId = ptPackConfStore->m_tConfInfo.GetConfId();
	if (FALSE == SetConfDataToFile(byConfPos, cConfConfId, (u8*)ptPackConfStore, wPackConfDataLen))
	{
		return FALSE;
	}
	
	//��¼�û����ģ��ͷ��Ϣ
	acConfId[byConfPos] = ptPackConfStore->m_tConfInfo.GetConfId();

	//��¼ȱʡ����ͷ��Ϣ
	if (bDefaultConf || acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE].IsNull())
	{
		if (!bDefaultConf)
		{
            //����ID�����ڣ�����һ��
			cDefConfConfId.CreateConfId(USRGRPID_INVALID);
			cDefConfConfId.SetConfSource(ptPackConfStore->m_tConfInfo.GetConfSource());
			ptPackConfStore->m_tConfInfo.SetConfId(cDefConfConfId);			
		}
		else
		{
            // guzh [8/1/2006] �Ѵ��ڣ����޸�Ϊ���û�Ȩ������
			cDefConfConfId = ptPackConfStore->m_tConfInfo.GetConfId();
            cDefConfConfId.SetUsrGrpId(USRGRPID_INVALID);
            ptPackConfStore->m_tConfInfo.SetConfId(cDefConfConfId);            
		}
        ptPackConfStore->m_tConfInfo.m_tStatus.SetOngoing();  // libo [10/31/2005]
	}
	
	//4.�� �û����ģ����Ϣ��Ϊȱʡ���� ���浽ָ�������Ĵ洢�ļ�
	if (!cDefConfConfId.IsNull())
	{
		//������ȱʡʧ��
		if (SetConfDataToFile((MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE), 
			                  cDefConfConfId, (u8*)ptPackConfStore, wPackConfDataLen))
		{
			acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE] = cDefConfConfId;
		}
	}
	
	//5.���±��� ˢ�º�����л����ģ��ͷ��Ϣ
	if (FALSE == SetAllConfHeadToFile(acConfId, sizeof(acConfId)))
	{
		return FALSE;
	}

	return TRUE;
}

/*====================================================================
    ������      : DeleteConfFromFile
    ����        : ɾ���ļ��д洢�Ļ����¼
    �㷨ʵ��    :����ѡ�
    ����ȫ�ֱ���: ��
    �������˵��: CConfId cConfId
    ����ֵ˵��  : BOOL32
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/13    3.0         ������         ����
    07/04/03    4.0         ����         �޸�
====================================================================*/
BOOL32 DeleteConfFromFile(CConfId cConfId)
{
	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
	u8 byConfPos = MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE;
	memset(acConfId, 0, sizeof(acConfId));
	
	//1. ��ȡ ����ͷ��Ϣ_�洢�ļ�
    if ( !GetAllConfHeadFromFile(acConfId, sizeof(acConfId)) )
    {
        return FALSE;
    }
	
	//�������л��飬������ȱʡ����λ��
	for (s32 nPos = 0; nPos < (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE); nPos++)
	{
		if (acConfId[nPos] == cConfId)
		{
			byConfPos = (u8)nPos;
			break;
		}
	}
	
	//δ����
	if (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE == byConfPos)
	{
		return FALSE;
	}

	//2.����û����ģ����ͷ��Ϣ�ϵļ�¼��Ϣ
	acConfId[byConfPos].SetNull();

    //3.���±���
    if (FALSE == SetAllConfHeadToFile(acConfId, sizeof(acConfId)))
    {
        return FALSE;
	}    
	
	return TRUE;	
}

/*====================================================================
    ������      : GetConfFromFile
    ����        : ���ļ��л�ȡ�洢�Ļ����¼
    �㷨ʵ��    :����ѡ�
    ����ȫ�ֱ���: ��
    �������˵��: [IN]  u8 byIndex, 
	              [IN]  TPackConfStore *ptPackConfStore ��СΪԤ���� sizeof(TConfStore)
    ����ֵ˵��  : BOOL32
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/13    3.0         ������          ����
====================================================================*/
BOOL32 GetConfFromFile(u8 byIndex, TPackConfStore *ptPackConfStore)
{
	//����ȱʡ����λ��
	if (NULL == ptPackConfStore || 
		byIndex > MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE)
	{
		return FALSE;
	}
	CConfId cConfId = GetOneConfHeadFromFile(byIndex);
	if (cConfId.IsNull()) //δ����
	{
		return FALSE;
	}

	u16 wOutBufLen = 0;
	return GetConfDataFromFile(byIndex, cConfId, (u8*)ptPackConfStore, 
		                       sizeof(TConfStore), wOutBufLen);
}

/*=============================================================================
    �� �� ���� GetOneConfHeadFromFile
    ��    �ܣ� ���ļ��л�ȡ�洢��ָ�������Ļ����ģ��ͷ��Ϣ��¼
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8 byIndex
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/29  4.0			����                  ����
=============================================================================*/
CConfId GetOneConfHeadFromFile( u8 byIndex )
{
	CConfId cConfId;
	cConfId.SetNull();

	//����ȱʡ����λ��
	if (byIndex > MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE)
	{
		return cConfId;
	}
	
	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
	memset(acConfId, 0, sizeof(acConfId));
	
	//��ȡ ����ͷ��Ϣ��ȱʡ������Ϣ_�洢�ļ�
    if ( !GetAllConfHeadFromFile(acConfId, sizeof(acConfId)) )
    {
        return cConfId;
    }
    else
    {
        return acConfId[byIndex];
    }	
}

/*=============================================================================
    �� �� ���� GetConfHeadFromFile
    ��    �ܣ� ���ļ��л�ȡ�洢�����л����ģ��ͷ��Ϣ��¼
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CConfId *pacConfId
	           u32 dwInBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/27  4.0			����                  ����
=============================================================================*/
BOOL32 GetAllConfHeadFromFile( CConfId *pacConfId, u32 dwInBufLen )
{
	FILE *hHeadFile = NULL;
	s8    achFullName[64];
	u32   dwOutBufLen = (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1)*sizeof(CConfId);
	if (NULL == pacConfId || dwInBufLen < dwOutBufLen)
	{
		return FALSE;
	}
	memset((s8*)pacConfId, 0, dwOutBufLen);
	
    if (!BeginRWConfInfoFile())
    {
        return FALSE;
    }

	//��ȡ ����ͷ��Ϣ_�洢�ļ�
	sprintf(achFullName, "%s/%s", DIR_DATA, CONFINFOFILENAME_HEADINFO);
	hHeadFile = fopen(achFullName, "rb"); 
	if (NULL != hHeadFile)
	{
        fread((s8*)pacConfId, dwOutBufLen, 1, hHeadFile);				
        fclose(hHeadFile);
	}

    EndRWConfInfoFile();

	return (NULL != hHeadFile);
}

/*=============================================================================
    �� �� ���� SetAllConfHeadToFile
    ��    �ܣ� �����л����ģ��ͷ��¼ �洢��ָ���Ļ����ļ���
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� CConfId *pacConfId
	           u32 dwInBufLen
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/27  4.0			����                  ����
=============================================================================*/
BOOL32 SetAllConfHeadToFile( CConfId *pacConfId, u32 dwInBufLen )
{
	FILE *hHeadFile = NULL;
	s8    achFullName[64];
	u32   dwOutBufLen = (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1)*sizeof(CConfId);
	if (NULL == pacConfId || dwInBufLen < dwOutBufLen)
	{
		return FALSE;
	}

    // guzh [4/3/2007] ���ȳ��Զ�ȡ,�ж��Ƿ���ȫ��ͬ
    CConfId acTmpConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
    if ( GetAllConfHeadFromFile(acTmpConfId, sizeof(acTmpConfId)) )
    {
        if ( 0 == memcmp( acTmpConfId, pacConfId, dwOutBufLen ) )
        {
            // ��ȫ��ͬ,�����ٴα���
            return TRUE;
        }
    }

    sprintf(achFullName, "%s/%s", DIR_DATA, CONFINFOFILENAME_HEADINFO);
    // guzh [4/3/2007] ȱʡ���鵥������,����ֱ�Ӹ��Ǽ���
    if (!BeginRWConfInfoFile())
    {
        return FALSE;
    }
    hHeadFile = fopen(achFullName, "wb");
    if (NULL != hHeadFile)
    {
        fwrite((s8*)pacConfId, dwOutBufLen, 1, hHeadFile);
        fclose(hHeadFile);        
    }

    EndRWConfInfoFile();
	
	return (hHeadFile != NULL);
}

/*=============================================================================
    �� �� ���� GetConfDataFromFile
    ��    �ܣ� ���ļ��л�ȡ�洢�Ļ����¼
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  u8  byIndex
               [IN]  CConfId cConfId
               [IN]  u8 *pbyBuf  �ѽ���Pack����Ļ�������
			         TPackConfStore + m_wAliasBufLen�ֽ�m_pbyAliasBuf + TTvWallModule(opt) + TVmpModule(opt);
               [IN]  u32 dwInBufLen ������
               [OUT] u16 &wOutBufLen ������
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/27  4.0			����                  ����
    07/04/03    4.0         ����                  �޸�
=============================================================================*/
BOOL32 GetConfDataFromFile(u8 byIndex, CConfId cConfId, u8 *pbyBuf, u32 dwInBufLen, u16 &wOutBufLen)
{
	wOutBufLen = 0;

	//�ɻ�ȡȱʡ������Ϣ
	if (byIndex > MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE)
	{
		return FALSE;
	}
	//pbyBuf Ϊ�ѽ���Pack����Ļ������ݣ�����Ӧ��С��sizeof(TPackConfStore)
	if (NULL == pbyBuf || dwInBufLen < sizeof(TPackConfStore))
	{
		return FALSE;
	}

	FILE *hConfFile = NULL;
	s8    achFullName[64];
	s32   nFileLen = 0;
	
    // guzh [4/3/2007] ȱʡ���鵥������
	//��ȡ ָ�������Ļ����ģ����Ϣ �洢�ļ� ��Ϣ
    sprintf(achFullName, "%s/%s%d%s", DIR_DATA, CONFINFOFILENAME_PREFIX, byIndex, CONFINFOFILENAME_POSTFIX);

    if (!BeginRWConfInfoFile())
    {
        return FALSE;
    }

	hConfFile = fopen(achFullName, "rb"); 
	if (NULL == hConfFile)
	{
        EndRWConfInfoFile();
		return FALSE;
	}

	fseek(hConfFile, 0, SEEK_END);
	nFileLen = ftell(hConfFile);
    //�����жϻ��峤�ȵ���Ч��
//    if (nFileLen < sizeof(TPackConfStore) || dwInBufLen < (u32)nFileLen )
//    {
//        EndRWConfInfoFile();
//        return FALSE;
//    }
    
    wOutBufLen = (u16)nFileLen;	

    fseek(hConfFile, 0, SEEK_SET);

	// confinfo size notify, zgc, 20070524
	s8 achConfFileHead[50] = {0};
	u16 wFileTConfInfoSize = 0;
	u32 dwSpace;

	// ���ļ�ͷ, �������°汾�����ϰ汾
	BOOL32 bIsNewVersion = TRUE;
	fread( achConfFileHead, strlen(CONFFILEHEAD), 1, hConfFile );
	if( 0 == memcmp( achConfFileHead, CONFFILEHEAD, strlen(CONFFILEHEAD) ) )
	{
		fread( &wFileTConfInfoSize, sizeof(u16), 1, hConfFile );
		wFileTConfInfoSize = ntohs(wFileTConfInfoSize);
		fread( &dwSpace, sizeof(u32), 1, hConfFile );
		wOutBufLen = wOutBufLen - strlen(CONFFILEHEAD) - sizeof(u16) - sizeof(u32);
	}
	else
	{
		bIsNewVersion = FALSE;
		fseek(hConfFile, 0, SEEK_SET);		
	}

	if ( bIsNewVersion && 0 == wFileTConfInfoSize )
	{
		OspPrintf( TRUE, FALSE, "[GetConfDataFromFile] In new version, FileTConfInfoSize can't be 0!\n");
		fclose(hConfFile);
		EndRWConfInfoFile();
		hConfFile = NULL;
		return FALSE;
	}

	// �ڴ汣��
	if( !bIsNewVersion && dwInBufLen < wOutBufLen )
	{
		OspPrintf( TRUE, FALSE, "[GetConfDataFromFile] The input buf length(%d) is less than output buf(%d)!\n", dwInBufLen, wOutBufLen );
		fclose(hConfFile);
		EndRWConfInfoFile();
		hConfFile = NULL;
        return FALSE;
	}
	// confinfo size notify end
	
	if( !bIsNewVersion )
	{
		wOutBufLen = ( wOutBufLen <= sizeof(TPackConfStore) ) ? wOutBufLen : sizeof(TPackConfStore);
		fread(pbyBuf, wOutBufLen, 1, hConfFile);
		fclose(hConfFile);
		hConfFile = NULL;
		TPackConfStore *ptTemp = (TPackConfStore *)pbyBuf;
		ptTemp->m_byMtNum = 0;
		ptTemp->m_wAliasBufLen = 0;
	}
	// confinfo size notify, zgc, 20070524
	else
	{
		u16 wMutualLen = min( wFileTConfInfoSize, sizeof(TConfInfo) );
		// �ڴ汣��
		if ( dwInBufLen < (u32)( wOutBufLen - wFileTConfInfoSize + wMutualLen ) )
		{
			OspPrintf( TRUE, FALSE, "[GetConfDataFromFile] The buf length is unenough!\n" );
			EndRWConfInfoFile();
			fclose(hConfFile);
			return FALSE;
		}

		u8 *pbyTemp = new u8[wOutBufLen];
		memset( pbyTemp, 0, wOutBufLen );
		fread( pbyTemp, wOutBufLen, 1, hConfFile );
		fclose( hConfFile );
		hConfFile = NULL;
		
		memcpy( pbyBuf, pbyTemp, wMutualLen );
		memcpy( pbyBuf + wMutualLen, pbyTemp + wFileTConfInfoSize, wOutBufLen - wFileTConfInfoSize );
		
		wOutBufLen = wOutBufLen - wFileTConfInfoSize + wMutualLen;

		delete [] pbyTemp;
	}
	// confinfo size notify end

	//У�� �û����ģ�� ͷ��Ϣ
	TPackConfStore *ptPackConfStore = (TPackConfStore *)pbyBuf;

	if (!(cConfId == ptPackConfStore->m_tConfInfo.GetConfId()))
	{
		wOutBufLen = 0;
        EndRWConfInfoFile();
		return FALSE;
	}

    EndRWConfInfoFile();
	return TRUE;
}

/*=============================================================================
    �� �� ���� SetConfDataToFile
    ��    �ܣ� ��ĳһ�����¼ �洢��ָ���Ļ����ļ���
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]   u8  byIndex
               [OUT]  CConfId &cConfId
               [IN]   u8 *pbyBuf �ѽ���Pack����Ļ�������
			          TPackConfStore + m_wAliasBufLen�ֽ�m_pbyAliasBuf + TTvWallModule(opt) + TVmpModule(opt);
               [IN]   u16 wInBufLen ������
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/27  4.0			����                  ����
=============================================================================*/
BOOL32 SetConfDataToFile(u8 byIndex, CConfId &cConfId, u8 *pbyBuf, u16 wInBufLen)
{
	cConfId.SetNull();

	//�ɱ���ȱʡ������Ϣ
	if (byIndex > MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE)
	{
		return FALSE;
	}
	//pbyBuf Ϊ�ѽ���Pack����Ļ������ݣ�����Ӧ��С��sizeof(TPackConfStore)
	if (NULL == pbyBuf || wInBufLen < sizeof(TPackConfStore))
	{
		return FALSE;
	}
	
	//��ȡ��У�� �û����ģ�� ͷ��Ϣ
	TPackConfStore *ptPackConfStore = (TPackConfStore *)pbyBuf;
	cConfId = ptPackConfStore->m_tConfInfo.GetConfId();
	if (cConfId.IsNull())
	{
		return FALSE;
	}

    // guzh [4/3/2007] ���ȳ��Զ�ȡ,��ͬ�򲻱���
    TConfStore tStore;
    u16 wOutLen;
    if (GetConfDataFromFile(byIndex, cConfId, (u8*)&tStore, sizeof(tStore), wOutLen))
    {
        if ( memcmp(&tStore, pbyBuf, wOutLen) == 0)
        {
            // ������ͬ������Ҫ�ٴα���
            return TRUE;
        }
    }

    if ( !BeginRWConfInfoFile() )
    {
        return FALSE;                
    }

    FILE *hConfFile = NULL;
    s8    achFullName[64];	
    sprintf(achFullName, "%s/%s%d%s", DIR_DATA, CONFINFOFILENAME_PREFIX, byIndex, CONFINFOFILENAME_POSTFIX);

	hConfFile = fopen(achFullName, "wb");
	if (NULL != hConfFile)
	{
		// confinfo size notify, zgc, 20070524
		s8 achConfFileHead[50];
		memcpy( achConfFileHead, CONFFILEHEAD, strlen(CONFFILEHEAD) );
		fwrite( achConfFileHead, strlen(CONFFILEHEAD), 1, hConfFile );
		u16 wTConfInfoSize = sizeof(TConfInfo);
		wTConfInfoSize = htons(wTConfInfoSize);
		fwrite( &wTConfInfoSize, sizeof(wTConfInfoSize), 1, hConfFile );
		u32 dwSpace = 0;
		fwrite( &dwSpace, sizeof(dwSpace), 1, hConfFile );
		// confinfo size notify end

        fwrite(pbyBuf, wInBufLen, 1, hConfFile);
	    fclose(hConfFile);
	}	
	
    EndRWConfInfoFile();

	return (hConfFile != NULL);
}
/*=============================================================================
    �� �� ���� GetUnProConfDataToFile
    ��    �ܣ� unprocconfinfo_confindex.dat�ļ��ж�ȡĳһ����ģ�岻��Ҫ���������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]   u8  byIndex
               [IN/OUT]   u8 *pbyBuf mcu����Ҫ����Ļ���ģ�������Ϣ(�������Ϣ)
               [OUT]   u16 wOutBufLen ������
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		������		�߶���    
    08/11/26                ���㻪
=============================================================================*/
BOOL32 GetUnProConfDataToFile(u8 byIndex, s8 *pbyBuf, u16& wOutBufLen)
{
	wOutBufLen = 0;

	//�ɱ���ȱʡ������Ϣ
	if (byIndex > MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE)
	{
		return FALSE;
	}

	if (NULL == pbyBuf)
	{
		return FALSE;
	}
	
    if ( !BeginRWConfInfoFile() )
    {
        return FALSE;                
    }

    FILE *hConfFile = NULL;
    s8    achFullName[64];	
    sprintf(achFullName, "%s/%s%d%s", DIR_DATA, UNPROCCONFINFOFILENAME_PREFIX, byIndex, CONFINFOFILENAME_POSTFIX);

	hConfFile = fopen(achFullName, "rb");
	if (NULL != hConfFile)
	{
		fseek(hConfFile, 0, SEEK_END);
		wOutBufLen = (u16)ftell(hConfFile);
		fseek(hConfFile, 0, SEEK_SET);

		fread(pbyBuf, wOutBufLen, 1, hConfFile);
		u16 dwLen = *(u16*)pbyBuf;
		dwLen = htons(dwLen);
		memcpy(pbyBuf, &dwLen, sizeof(u16));
	    fclose(hConfFile);
	}

    EndRWConfInfoFile();

	return (hConfFile != NULL);
}

/*=============================================================================
    �� �� ���� SetUnProConfDataToFile
    ��    �ܣ� ��ĳһ����ģ�岻��Ҫ���������ֱ�Ӵ洢����Ӧ��unprocconfinfo_confindex.dat�ļ���
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]   u8  byIndex
               [IN]   s8 *pbyBuf mcu����Ҫ����Ļ���ģ�������Ϣ(�������Ϣ)
               [IN]   u16 wInBufLen ������
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		������		�߶���    
    08/11/26                ���㻪
=============================================================================*/
BOOL32 SetUnProConfDataToFile(u8 byIndex, s8 *pbyBuf, u16 wInBufLen)
{
	//�ɱ���ȱʡ������Ϣ
	if (byIndex > MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE)
	{
		return FALSE;
	}

	if (NULL == pbyBuf )
	{
		return FALSE;
	}
	
    if ( !BeginRWConfInfoFile() )
    {
        return FALSE;                
    }

    FILE *hConfFile = NULL;
    s8    achFullName[64];	
    sprintf(achFullName, "%s/%s%d%s", DIR_DATA, UNPROCCONFINFOFILENAME_PREFIX, byIndex, CONFINFOFILENAME_POSTFIX);

	hConfFile = fopen(achFullName, "wb");
	if (NULL != hConfFile)
	{
        fwrite( pbyBuf, wInBufLen, 1, hConfFile);
	    fclose(hConfFile);
	}	
	
    EndRWConfInfoFile();

	return (hConfFile != NULL);
}

/*=============================================================================
    �� �� ���� PackConfStore
    ��    �ܣ� �� δ����Pack����Ļ������� ����Pack����
	           TConfStore -->
	           TPackConfStore + m_wAliasBufLen�ֽ�m_pbyAliasBuf + TTvWallModule(opt) + TVmpModule(opt);
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  TConfStore &tConfStore ��Pack����Ļ�������
               [OUT] TPackConfStore *ptPackConfStore Pack����������
               [OUT] u16 &wPackConfDataLen Pack���������峤�� ������
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/29  4.0			����                  ����
=============================================================================*/
BOOL32 PackConfStore( TConfStore &tConfStore, TPackConfStore *ptPackConfStore, u16 &wPackConfDataLen )
{
	u16 wAliasBufLen = 0;
	u8* pbyAliasBuf  = NULL; //�ն˱�������������
	u8* pbyModuleBuf = NULL; //����ǽ//����ϳ�ģ�建��
	
	wPackConfDataLen = 0;

	//����������Ϣ
	memcpy((s8*)ptPackConfStore, (s8*)&tConfStore, sizeof(TPackConfStore));
	wPackConfDataLen += sizeof(TPackConfStore);
	//�ն˱���������
	pbyAliasBuf = (u8 *)(ptPackConfStore+1);
	PackTMtAliasArray(tConfStore.m_atMtAlias, tConfStore.m_awMtDialBitRate, 
		              tConfStore.m_byMtNum, (s8*)pbyAliasBuf, wAliasBufLen);
	ptPackConfStore->m_wAliasBufLen = wAliasBufLen;
	wPackConfDataLen += wAliasBufLen;
	pbyModuleBuf = pbyAliasBuf+ptPackConfStore->m_wAliasBufLen;
	//����ǽģ�� 
	if (ptPackConfStore->m_tConfInfo.GetConfAttrb().IsHasTvWallModule())
	{
		memcpy(pbyModuleBuf, (s8*)&tConfStore.m_tMultiTvWallModule, sizeof(TMultiTvWallModule));
		wPackConfDataLen += sizeof(TMultiTvWallModule);
		pbyModuleBuf += sizeof(TMultiTvWallModule);
	}
	//����ϳ�ģ�� 
	if (ptPackConfStore->m_tConfInfo.GetConfAttrb().IsHasVmpModule())
	{
		memcpy(pbyModuleBuf, (s8*)&tConfStore.m_atVmpModule, sizeof(TVmpModule));
		wPackConfDataLen += sizeof(TVmpModule);
		pbyModuleBuf += sizeof(TVmpModule);
	}
	//���ΪVCSģ�壬���ӵ���Ϣ
	//                  + 1byte(u8: 0 1  �Ƿ������˸������ǽ)
	//                  +(��ѡ, THDTvWall)
	//                  + 1byte(u8: ��������HDU��ͨ������)
	//                  + (��ѡ, THduModChnlInfo+...)	
	//                  + 1byte(�Ƿ�Ϊ��������)
	//                  +(��ѡ��2byte[u16 ���������ô�����ܳ���]+�¼�mcu����[1byte(�ն�����)+1byte(��������)+xbyte(�����ַ���)+2byte(��������)...)])
	//                  + 1byte(�Ƿ�֧�ַ���)
	//                  + (��ѡ��2byte(u16 ��������Ϣ�ܳ�)+����(1byte(����)+n��[1TVCSGroupInfo��m��TVCSEntryInfo])
	if (VCS_CONF == tConfStore.m_tConfInfo.GetConfSource())
	{
		u8* pbyVCSInfoExBuf = pbyModuleBuf;  //VCS����ģ���������ݻ���
		*pbyVCSInfoExBuf++  = (u8)tConfStore.IsHDTWCfg();
		wPackConfDataLen += sizeof(u8);
		if (tConfStore.IsHDTWCfg())
		{
			memcpy(pbyVCSInfoExBuf, &tConfStore.m_tHDTWInfo, sizeof(THDTvWall));
			pbyVCSInfoExBuf  += sizeof(THDTvWall);
			wPackConfDataLen += sizeof(THDTvWall);
		}

		u8 byHduChnlNum = tConfStore.m_tHduModule.GetHduChnlNum();
		*pbyVCSInfoExBuf++ = byHduChnlNum;
		wPackConfDataLen += sizeof(u8);
		if (byHduChnlNum)
		{
			memcpy(pbyVCSInfoExBuf, tConfStore.m_tHduModule.GetHduModuleInfo(), byHduChnlNum * sizeof(THduModChnlInfo));
			pbyVCSInfoExBuf  += byHduChnlNum *sizeof(THduModChnlInfo);
			wPackConfDataLen += byHduChnlNum *sizeof(THduModChnlInfo);
		}

		*pbyVCSInfoExBuf++  = (u8)tConfStore.IsVCSMCUCfg();
		wPackConfDataLen += sizeof(u8);
		if (tConfStore.IsVCSMCUCfg())
		{
			wAliasBufLen = 0;
			pbyVCSInfoExBuf += sizeof(u16);
			wPackConfDataLen += sizeof(u16);
			PackTMtAliasArray(tConfStore.m_tVCSSMCUCfg.m_atSMCUAlias, tConfStore.m_tVCSSMCUCfg.m_awSMCUDialBitRate, 
							  tConfStore.m_tVCSSMCUCfg.m_wSMCUNum, (s8*)pbyVCSInfoExBuf, wAliasBufLen);
			*(u16*)(pbyVCSInfoExBuf - sizeof(u16)) = wAliasBufLen;
			pbyVCSInfoExBuf += wAliasBufLen;
			wPackConfDataLen += wAliasBufLen;
		}
		*pbyVCSInfoExBuf++ = tConfStore.m_byMTPackExist;
		wPackConfDataLen += sizeof(u8);
	}	


	return TRUE;
}

/*=============================================================================
    �� �� ���� UnPackConfStore
    ��    �ܣ� �� �ѽ���Pack����Ļ������� ����UnPack����
	           TPackConfStore + m_wAliasBufLen�ֽ�m_pbyAliasBuf + TTvWallModule(opt) + TVmpModule(opt) -->
			   TConfStore;
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  TPackConfStore *ptPackConfStore ��UnPack����Ļ�������
               [OUT] TConfStore &tConfStore  UnPack����������
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/29  4.0			����                  ����
=============================================================================*/
BOOL32 UnPackConfStore( TPackConfStore *ptPackConfStore, TConfStore &tConfStore )
{
	u16 wAliasBufLen = 0;
	u8* pbyAliasBuf  = NULL; //�ն˱�������������
	u8* pbyModuleBuf = NULL; //����ǽ//����ϳ�ģ�建��
	
	//����������Ϣ
	memcpy((s8*)&tConfStore, (s8*)ptPackConfStore, sizeof(TPackConfStore));
	//�ն˱���������
	pbyAliasBuf  = (u8 *)(ptPackConfStore+1);
	wAliasBufLen = ptPackConfStore->m_wAliasBufLen;
	pbyModuleBuf = pbyAliasBuf+ptPackConfStore->m_wAliasBufLen;
    u8 byMtNum = tConfStore.m_byMtNum;
	UnPackTMtAliasArray((s8*)pbyAliasBuf, wAliasBufLen, &ptPackConfStore->m_tConfInfo, 
		                tConfStore.m_atMtAlias, tConfStore.m_awMtDialBitRate, byMtNum);
    tConfStore.m_byMtNum = byMtNum; 
	//����ǽģ�� 
	if (ptPackConfStore->m_tConfInfo.GetConfAttrb().IsHasTvWallModule())
	{
		memcpy((s8*)&tConfStore.m_tMultiTvWallModule, pbyModuleBuf, sizeof(TMultiTvWallModule));
		pbyModuleBuf += sizeof(TMultiTvWallModule);
	}
	//����ϳ�ģ�� 
	if (ptPackConfStore->m_tConfInfo.GetConfAttrb().IsHasVmpModule())
	{
		memcpy((s8*)&tConfStore.m_atVmpModule, pbyModuleBuf, sizeof(TVmpModule));
		pbyModuleBuf += sizeof(TVmpModule);
	}
	
	return TRUE;
}

/*=============================================================================
    �� �� ���� UpdateConfStoreModule
    ��    �ܣ� ����ConfStore�е���ǽ�ͻ���ϳ�ģ����ն�������ӳ��
    �㷨ʵ�֣� ������ OldIdx �޸�Ϊ NewIdx
    ȫ�ֱ����� 
    ��    ���� ע�⣬�ú���ֻ�޸�������ӳ��ֵ�������������Լ�ά��
    �� �� ֵ�� BOOL32  �Ƿ��������ݸ���
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2007/07/05  4.0			����                  ����
=============================================================================*/
BOOL32 UpdateConfStoreModule( const TConfStore &tConfStore, 
                            u8 byOldIdx, u8 byNewIdx,
                            TMultiTvWallModule &tOutTvwallModule, 
                            TVmpModule &tOutVmpModule )
{
    BOOL32 bReplace = FALSE;
    if (tConfStore.m_tConfInfo.GetConfAttrb().IsHasTvWallModule())
    {
        for(u8 byModuleLp = 0; byModuleLp < tConfStore.m_tMultiTvWallModule.GetTvModuleNum() ; byModuleLp++)
        {
            for (u8 byTvChlLp = 0; byTvChlLp < MAXNUM_PERIEQP_CHNNL; byTvChlLp++)
            {
                if(byOldIdx == tConfStore.m_tMultiTvWallModule.m_atTvWallModule[byModuleLp].m_abyTvWallMember[byTvChlLp] )
                {
                    tOutTvwallModule.m_atTvWallModule[byModuleLp].m_abyTvWallMember[byTvChlLp] = byNewIdx;
                    bReplace = TRUE;
                }
            }
        }
    }
    if (tConfStore.m_tConfInfo.GetConfAttrb().IsHasVmpModule())
    {
        for (u8 byVmpIdx = 0; byVmpIdx < MAXNUM_MPUSVMP_MEMBER; byVmpIdx++)
        {
            if( byOldIdx == tConfStore.m_atVmpModule.m_abyVmpMember[byVmpIdx] )
            {
                tOutVmpModule.m_abyVmpMember[byVmpIdx] = byNewIdx;
                bReplace = TRUE;
            }
        }
    }            

    return bReplace;
}

/*====================================================================
    ������      : GetMediaStr
    ����        : ��ȡý�������ַ���
    �㷨ʵ��    :����ѡ�
    ����ȫ�ֱ���: ��
    �������˵��: byMediaType ý������
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/13    3.0         ������       ����
====================================================================*/
LPCSTR GetMediaStr( u8 byMediaType )
{
    static char szMediaType[16];
	switch( byMediaType ) 
	{
	case MEDIA_TYPE_H261:
		sprintf( szMediaType, "%s%c", "H.261", 0 );
		break;
	case MEDIA_TYPE_H262:
		sprintf( szMediaType, "%s%c", "H.262", 0 );
		break;
	case MEDIA_TYPE_H263:
		sprintf( szMediaType, "%s%c", "H.263", 0 );
		break;
	case MEDIA_TYPE_H263PLUS:
		sprintf( szMediaType, "%s%c", "H.263+", 0 );
		break;
	case MEDIA_TYPE_H264:
		sprintf( szMediaType, "%s%c", "H.264", 0 );
		break;
	case MEDIA_TYPE_MP4:
		sprintf( szMediaType, "%s%c", "MPEG-4", 0 );
		break;
	case MEDIA_TYPE_PCMA:
		sprintf( szMediaType, "%s%c", "PCMA", 0 );
		break;
	case MEDIA_TYPE_PCMU:
		sprintf( szMediaType, "%s%c", "PCMU", 0 );
		break;
	case MEDIA_TYPE_G722:
		sprintf( szMediaType, "%s%c", "G.722", 0 );
		break;
    case MEDIA_TYPE_G7221C:
        sprintf( szMediaType, "%s%c", "G.7221", 0 );
        break;
    case MEDIA_TYPE_AACLC:
        sprintf( szMediaType, "%s%c", "AAC-LC", 0 );
        break;
    case MEDIA_TYPE_AACLD:
        sprintf( szMediaType, "%s%c", "AAC-LD", 0 );
        break;
	case MEDIA_TYPE_G7231:
		sprintf( szMediaType, "%s%c", "G.7231", 0 );
		break;
	case MEDIA_TYPE_G728:
		sprintf( szMediaType, "%s%c", "G.728", 0 );
		break;
	case MEDIA_TYPE_G729:
		sprintf( szMediaType, "%s%c", "G.729", 0 );
		break;
	case MEDIA_TYPE_MP3:
		sprintf( szMediaType, "%s%c", "MP3", 0 );
		break;
	case MEDIA_TYPE_T120:
		sprintf( szMediaType, "%s%c", "T.120", 0 );
		break;
	case MEDIA_TYPE_H224:
		sprintf( szMediaType, "%s%c", "H.224", 0 );
		break;
	case MEDIA_TYPE_H239:
		sprintf( szMediaType, "%s%c", "H.239", 0 );
		break;
	case MEDIA_TYPE_MMCU:
		sprintf( szMediaType, "%s%c", "MMCU", 0 );
		break;
	default:
		sprintf( szMediaType, "%s(%d)%c", "Unknown", byMediaType, 0 );
		break;
	}
	return szMediaType;
}

/*====================================================================
    ������      : GetResStr
    ����        : ��ȡ�ֱ��������ַ���
    �㷨ʵ��    :����ѡ�
    ����ȫ�ֱ���: ��
    �������˵��: byResolution �ֱ�������
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    08-08-09    5.0         �ܹ��        ����
	3/4/2009    4.6			Ѧ��		  ���ӷǱ�ֱ��� 
====================================================================*/
LPCSTR GetResStr( u8 byResolution )
{
    static char szResType[16];
	
    switch( byResolution )
    {
    //��Ƶ��ʽ����
    case VIDEO_FORMAT_SQCIF:
        sprintf( szResType, "%s%c", "SQCIF", 0 );
        break;
    case VIDEO_FORMAT_QCIF:	
        sprintf( szResType, "%s%c", "QCIF", 0 );
        break;
    case VIDEO_FORMAT_CIF:              
        sprintf( szResType, "%s%c", "CIF", 0 );
        break;
    case VIDEO_FORMAT_2CIF:              
        sprintf( szResType, "%s%c", "2CIF", 0 );
        break;
    case VIDEO_FORMAT_4CIF:         
        sprintf( szResType, "%s%c", "4CIF", 0 );
        break;
    case VIDEO_FORMAT_16CIF:			
        sprintf( szResType, "%s%c", "16CIF", 0 );
        break;
    case VIDEO_FORMAT_AUTO:			
        sprintf( szResType, "%s%c", "AUTO", 0 );
        break;
    
    case VIDEO_FORMAT_SIF:             
        sprintf( szResType, "%s%c", "SIF", 0 );
        break;
    case VIDEO_FORMAT_2SIF:               
        sprintf( szResType, "%s%c", "2SIF", 0 );
        break;
    case VIDEO_FORMAT_4SIF:           
        sprintf( szResType, "%s%c", "4SIF", 0 );
        break;
    
    case VIDEO_FORMAT_VGA:            
        sprintf( szResType, "%s%c", "VGA", 0 );
        break;
    case VIDEO_FORMAT_SVGA:             
        sprintf( szResType, "%s%c", "SVGA", 0 );
        break;
    case VIDEO_FORMAT_XGA:             
        sprintf( szResType, "%s%c", "XGA", 0 );
        break;
    
    //�������ն˷ֱ��ʸı�
    case VIDEO_FORMAT_SQCIF_112x96:     
        sprintf( szResType, "%s%c", "SQCIF112x96", 0 );
        break;
    case VIDEO_FORMAT_SQCIF_96x80:     
        sprintf( szResType, "%s%c", "SQCIF96x80", 0 );
        break;
    
    // ����ֱ���
    case VIDEO_FORMAT_W4CIF:            
        sprintf( szResType, "%s%c", "W4CIF", 0 );
        break;
    case VIDEO_FORMAT_HD720:           
        sprintf( szResType, "%s%c", "HD720", 0 );
        break;
    case VIDEO_FORMAT_SXGA:             
        sprintf( szResType, "%s%c", "SXGA", 0 );
        break;
    case VIDEO_FORMAT_UXGA:             
        sprintf( szResType, "%s%c", "UXGA", 0 );
        break;
    case VIDEO_FORMAT_HD1080:             
        sprintf( szResType, "%s%c", "HD1080", 0 );
        break;

	//�Ǳ�ֱ���
	case VIDEO_FORMAT_1440x816:
		sprintf( szResType, "%s%c", "1440x816",0 );
		break;
	case VIDEO_FORMAT_1280x720:
		sprintf( szResType, "%s%c", "1280x720",0 );
		break;
	case VIDEO_FORMAT_960x544:
		sprintf( szResType, "%s%c", "960x544",0 );
		break;
	case VIDEO_FORMAT_640x368:
		sprintf( szResType, "%s%c", "640x368",0 );
		break;
	case VIDEO_FORMAT_480x272:
		sprintf( szResType, "%s%c", "480x272",0 );
		break;

	case VIDEO_FORMAT_720_960x544:
		sprintf( szResType, "%s%c", "960x544",0 );
		break;
	case VIDEO_FORMAT_720_864x480:
		sprintf( szResType, "%s%c", "864x480",0 );
		break;
	case VIDEO_FORMAT_720_640x368:
		sprintf( szResType, "%s%c", "640x368",0 );
		break;
	case VIDEO_FORMAT_720_432x240:
		sprintf( szResType, "%s%c", "432x240",0 );
		break;
	case VIDEO_FORMAT_720_320x192:
		sprintf( szResType, "%s%c", "320x192",0 );
		break;
    default:
        sprintf( szResType, "%s(%d)%c", "Unknown", byResolution, 0 );
		break;
    }

	return szResType;
}

/*=============================================================================
�� �� ���� IsHDConf
��    �ܣ� 
�㷨ʵ�֣� �������������ϵ�HD���� >= 4CIF ��H264����������HD�� >= HD720p��
ȫ�ֱ����� 
��    ���� const TConfInfo &tConfInfo
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/8/9   4.0		    �ܹ��                  ����
=============================================================================*/
BOOL32 IsHDConf( const TConfInfo &tConfInfo )
{
    if ( tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
         ( IsVidFormatHD( tConfInfo.GetMainVideoFormat() ) ||
           tConfInfo.GetMainVideoFormat() == VIDEO_FORMAT_4CIF ) )
    {
        return TRUE;
    }

    if ( tConfInfo.GetSecVideoMediaType() == MEDIA_TYPE_H264 &&
        ( IsVidFormatHD( tConfInfo.GetSecVideoFormat() ) ||
          tConfInfo.GetSecVideoFormat() == VIDEO_FORMAT_4CIF ) )
    {
        return TRUE;
    }

    return FALSE;
}


/*=============================================================================
�� �� ���� IsConfDualEqMV
��    �ܣ� �жϻ����Ƿ� ˫��ͬ����Ƶ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const TConfInfo &tConfInfo
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/11/17   4.0		�ű���                  ����
=============================================================================*/
BOOL32 IsConfDualEqMV( const TConfInfo &tConfInfo )
{
    if (!tConfInfo.GetCapSupport().IsDStreamSupportH239() &&
        tConfInfo.GetMainVideoMediaType() == tConfInfo.GetDStreamMediaType() &&
        tConfInfo.GetMainVideoFormat() == tConfInfo.GetDoubleVideoFormat() )
    {
        return TRUE;
    }
    return FALSE;
}

/*=============================================================================
�� �� ���� IsConfDualEqMV
��    �ܣ� �жϻ����Ƿ� ����˫������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const TConfInfo &tConfInfo
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/11/17   4.0		�ű���                  ����
=============================================================================*/
BOOL32 IsConfDoubleDual( const TConfInfo &tConfInfo )
{
    if (tConfInfo.GetCapSupport().IsDStreamSupportH239() &&
        MEDIA_TYPE_H264 == tConfInfo.GetDStreamMediaType() &&
        (g_cMcuVcApp.IsSupportSecDSCap() ||
         MEDIA_TYPE_NULL != tConfInfo.GetCapSupportEx().GetSecDSType()))
    {
        return TRUE;
    }
    return FALSE;
}

/*====================================================================
    ������      : GetManufactureStr
    ����        : ��ȡ���쳧���ַ���
    �㷨ʵ��    :����ѡ�
    ����ȫ�ֱ���: ��
    �������˵��: byMediaType ý������
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/13    3.0         ������       ����
====================================================================*/
LPCSTR GetManufactureStr( u8 byManufacture )
{
    static char szManu[16];
	switch( byManufacture ) 
	{
	case MT_MANU_KDC:
		sprintf( szManu, "%s%c", "Keda", 0 );
		break;
	case MT_MANU_NETMEETING:
		sprintf( szManu, "%s%c", "Netmeeting", 0 );
		break;
	case MT_MANU_POLYCOM:
		sprintf( szManu, "%s%c", "Polycom", 0 );
		break;
	case MT_MANU_HUAWEI:
		sprintf( szManu, "%s%c", "Huawei", 0 );
		break;
	case MT_MANU_TAIDE:
		sprintf( szManu, "%s%c", "Tandberg", 0 );
		break;
	case MT_MANU_SONY:
		sprintf( szManu, "%s%c", "Sony", 0 );
		break;
	case MT_MANU_VCON:
		sprintf( szManu, "%s%c", "Vcon", 0 );
		break;
	case MT_MANU_RADVISION:
		sprintf( szManu, "%s%c", "Radvision", 0 );
		break;
	case MT_MANU_KDCMCU:
		sprintf( szManu, "%s%c", "KedaMCU", 0 );
		break;
	case MT_MANU_PENTEVIEW:
		sprintf( szManu, "%s%c", "Penteviw", 0 );
		break;
    case MT_MANU_CODIAN:
        sprintf( szManu, "%s%c", "CodianMCU", 0 );
        break;
	case MT_MANU_OTHER:
		sprintf( szManu, "%s%c", "Unknown", 0 );
		break;
	default: sprintf( szManu, "%s%c", "Unknown", 0);break;
	}
	return szManu;	
}

/*====================================================================
    ������      : AddEventStr
    ����        : �����Ϣ�ַ���
    �㷨ʵ��    : 
    ����ȫ�ֱ���: ��
    �������˵��: 
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/11    3.0         ������       ����
====================================================================*/
void AddEventStr()
{
	#undef OSPEVENT
	#define OSPEVENT( x, y ) OspAddEventDesc( #x, y )
	
#ifdef _EVMCUMCS_H_
	#undef _EVMCUMCS_H_
	#include "evmcumcs.h"
	#define _EVMCUMCS_H_
#else
	#include "evmcumcs.h"
	#undef _EVMCUMCS_H_
#endif

#ifdef _EVMCUVCS_H_
	#undef _EVMCUVCS_H_
	#include "evmcuvcs.h"
	#define _EVMCUVCS_H_
#else
	#include "evmcuvcs.h"
	#undef _EVMCUVCS_H_
#endif

#ifdef _EV_MCUMT_H_
	#undef _EV_MCUMT_H_
	#include "evmcumt.h"
	#define _EV_MCUMT_H_
#else
	#include "evmcumt.h"
	#undef _EV_MCUMT_H_
#endif
	
#ifdef _EV_MCUEQP_H_
	#undef _EV_MCUEQP_H_
	#include "evmcueqp.h"
	#define _EV_MCUEQP_H_
#else
	#include "evmcueqp.h"
	#undef _EV_MCUEQP_H_
#endif

#ifdef _EV_MCU_H_
	#undef _EV_MCU_H_
	#include "evmcu.h"
	#define _EV_MCU_H_
#else
	#include "evmcu.h"
	#undef _EV_MCU_H_
#endif

#ifdef __EVMP_H_
    #undef __EVMP_H_
    #include "evmp.h"
    #define __EVMP_H_
#else
    #include "evmp.h"
    #undef __EVMP_H_
#endif
    
#ifdef _EV_MCUDCS_H_
	#undef _EV_MCUDCS_H_
	#include "evmcudcs.h"
	#define _EV_MCUDCS_H_
#else
	#include "evmcudcs.h"
	#undef _EV_MCUDCS_H_
#endif

#ifdef _EVMCUTEST_H_
	#undef _EVMCUTEST_H_
	#include "evmcutest.h"
	#define _EVMCUTEST_H_
#else
	#include "evmcutest.h"
	#undef _EVMCUTEST_H_
#endif

#ifdef _EV_MODEM_H_
    #undef _EV_MODEM_H_
    #include "evmodem.h"
    #define _EV_MODEM_H_
#else
    #include "evmodem.h"
    #undef _EV_MODEM_H_
#endif
}

/*====================================================================
    ������      : StartLocalMixer
    ����        : �������صĻ�����
    �㷨ʵ��    : 
    ����ȫ�ֱ���: ��
    �������˵��: u8 byEqpId �豸Id
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/11    3.0         ������       ����
====================================================================*/
void StartLocalMixer( u8 byEqpId )
{
#if defined(_MINIMCU_)
	TAudioMixerCfg tAudioMixerCfg;

    tAudioMixerCfg.byEqpId      = byEqpId;
    tAudioMixerCfg.byEqpType    = EQP_TYPE_MIXER;
    tAudioMixerCfg.dwLocalIP    = g_cMcuAgent.GetMpcIp();
    tAudioMixerCfg.dwConnectIP  = g_cMcuAgent.GetMpcIp();
    tAudioMixerCfg.wConnectPort = PORT_MCU;
    tAudioMixerCfg.wMcuId       = LOCAL_MCUID;
    
	TEqpMixerInfo tMixParam;
	g_cMcuAgent.GetEqpMixerCfg( byEqpId, &tMixParam );

	TMAPParam tMAPParam[3];
    ReadMAPConfig( tMAPParam, 3 );

	tAudioMixerCfg.byMaxMixGroupCount  = 1; //ԭ����mcu agent�����1
    tAudioMixerCfg.wRcvStartPort       = tMixParam.GetEqpRecvPort();
	tAudioMixerCfg.byMaxChannelInGroup = tMixParam.GetMaxChnInGrp();
	tAudioMixerCfg.wMAPCount           = tMixParam.GetUsedMapNum();

	for( u8 byMapId = 0; byMapId < tAudioMixerCfg.wMAPCount; byMapId++ )
	{
		tAudioMixerCfg.m_atMap[byMapId].byMapId   = tMixParam.GetUsedMapId( byMapId );
		tAudioMixerCfg.m_atMap[byMapId].dwPort    = tMAPParam[tAudioMixerCfg.m_atMap[byMapId].byMapId].dwPort;
		tAudioMixerCfg.m_atMap[byMapId].dwCoreSpd = tMAPParam[tAudioMixerCfg.m_atMap[byMapId].byMapId].dwCoreSpd;
		tAudioMixerCfg.m_atMap[byMapId].dwMemSize = tMAPParam[tAudioMixerCfg.m_atMap[byMapId].byMapId].dwMemSize;
		tAudioMixerCfg.m_atMap[byMapId].dwMemSpd  = tMAPParam[tAudioMixerCfg.m_atMap[byMapId].byMapId].dwMemSpd;
	}

    memset( tAudioMixerCfg.achAlias, 0, sizeof(tAudioMixerCfg.achAlias));
    memcpy( tAudioMixerCfg.achAlias, tMixParam.GetAlias(), MAXLEN_EQP_ALIAS );
    
	// sprintf( tAudioMixerCfg.achAlias, "Mix%d%c", byEqpId, 0);
    if( mixInit( &tAudioMixerCfg ) )
    {
        OspPrintf( TRUE, FALSE, "Start local mixer success!\n" );
	
    }
    else
    {
        OspPrintf( TRUE, FALSE, "Start local mixer failure!\n");
    }
#endif

	return;
}

#ifdef  _MINIMCU_
//void painthelp(void);
#endif

/*====================================================================
    ������      : StartLocalVmp
    ����        : �������صĻ���ϳ���
    �㷨ʵ��    : 
    ����ȫ�ֱ���: ��
    �������˵��: u8 byEqpId �豸Id
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/11    3.0         ������       ����
====================================================================*/
void StartLocalVmp( u8 byEqpId )
{
#if defined(_MINIMCU_)

	// painthelp();

	TVmpCfg tVmpCfg;
    tVmpCfg.byEqpId      = byEqpId;
    tVmpCfg.byEqpType    = EQP_TYPE_VMP;
    tVmpCfg.dwLocalIP    = g_cMcuAgent.GetMpcIp();
    tVmpCfg.dwConnectIP  = g_cMcuAgent.GetMpcIp();
    tVmpCfg.wConnectPort = PORT_MCU;
    tVmpCfg.wMcuId       = LOCAL_MCUID;

	TMAPParam tMAPParam[3];
    ReadMAPConfig( tMAPParam, 3 );
    
	TEqpVMPInfo tVmpParam;
	g_cMcuAgent.GetEqpVMPCfg( byEqpId, &tVmpParam );

    tVmpCfg.wRcvStartPort = tVmpParam.GetEqpRecvPort();
	tVmpCfg.byDbVid = tVmpParam.GetEncodeNum();

	tVmpCfg.wMAPCount = tVmpParam.GetUsedMapNum();
	for( u8 byMapId = 0; byMapId < tVmpCfg.wMAPCount; byMapId++ )
	{
		tVmpCfg.m_atMap[byMapId].byMapId   = tVmpParam.GetUsedMapId(byMapId);
		tVmpCfg.m_atMap[byMapId].dwPort    = tMAPParam[tVmpCfg.m_atMap[byMapId].byMapId].dwPort;
		tVmpCfg.m_atMap[byMapId].dwCoreSpd = tMAPParam[tVmpCfg.m_atMap[byMapId].byMapId].dwCoreSpd;
		tVmpCfg.m_atMap[byMapId].dwMemSize = tMAPParam[tVmpCfg.m_atMap[byMapId].byMapId].dwMemSize;
		tVmpCfg.m_atMap[byMapId].dwMemSpd  = tMAPParam[tVmpCfg.m_atMap[byMapId].byMapId].dwMemSpd;
	}

    memset( tVmpCfg.achAlias, 0, sizeof(tVmpCfg.achAlias));
    memcpy( tVmpCfg.achAlias, tVmpParam.GetAlias(), MAXLEN_EQP_ALIAS );
    
	// sprintf( tVmpCfg.achAlias, "Vmp%d%c", byEqpId, 0);
    vmpinit( &tVmpCfg );
	
#endif

	return;
}

/*====================================================================
    ������      : GetPrsTimeSpan
    ����        : ��ȡPrsʱ����
    �㷨ʵ��    : 
    ����ȫ�ֱ���: ��
    �������˵��: u8 byEqpId �豸Id
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/11    3.0         ������       ����
====================================================================*/
void GetPrsTimeSpan( TPrsTimeSpan *ptPrsTimeSpan )
{
	TEqpPrsInfo tPrsParam;
	memset( &tPrsParam, 0, sizeof(tPrsParam) );

	u8  byEqpId;
	u32 dwEqpIp;
	u8  byEqpType;
	for( byEqpId=1; byEqpId < MAXNUM_MCU_PERIEQP; byEqpId++ )
	{
		if( SUCCESS_AGENT == g_cMcuAgent.GetPeriInfo( byEqpId, &dwEqpIp, &byEqpType ) )
		{
			if( byEqpType == EQP_TYPE_PRS )
			{
				g_cMcuAgent.GetEqpPrsCfg( byEqpId, tPrsParam );
				ptPrsTimeSpan->m_wFirstTimeSpan  = tPrsParam.GetFirstTimeSpan();
				ptPrsTimeSpan->m_wSecondTimeSpan = tPrsParam.GetSecondTimeSpan();
				ptPrsTimeSpan->m_wThirdTimeSpan  = tPrsParam.GetThirdTimeSpan();
				ptPrsTimeSpan->m_wRejectTimeSpan = tPrsParam.GetRejectTimeSpan();
			}
		}
	}

	return;
}

/*====================================================================
    ������      : StartLocalPrs
    ����        : �������صĶ����ش�
    �㷨ʵ��    : 
    ����ȫ�ֱ���: ��
    �������˵��: u8 byEqpId �豸Id
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/11    3.0         ������       ����
====================================================================*/
void StartLocalPrs( u8 byEqpId )
{
#if defined(_MINIMCU_)
	TPrsCfg tPrsCfg;
	tPrsCfg.byEqpId      = byEqpId;
	tPrsCfg.byEqpType    = EQP_TYPE_PRS;
	tPrsCfg.dwConnectIP  = g_cMcuAgent.GetMpcIp();
	tPrsCfg.dwLocalIP    = g_cMcuAgent.GetMpcIp();;
	tPrsCfg.wConnectPort = PORT_MCU;
	tPrsCfg.wMcuId       = LOCAL_MCUID;
	tPrsCfg.wMAPCount    = 0;
	//sprintf( tPrsCfg.achAlias, "Prs%d%c", byEqpId, 0);

	TEqpPrsInfo tPrsParam;
	g_cMcuAgent.GetEqpPrsCfg( byEqpId, tPrsParam );
	tPrsCfg.m_wFirstTimeSpan  = tPrsParam.GetFirstTimeSpan();
	tPrsCfg.m_wSecondTimeSpan = tPrsParam.GetSecondTimeSpan();
	tPrsCfg.m_wThirdTimeSpan  = tPrsParam.GetThirdTimeSpan();
	tPrsCfg.m_wRejectTimeSpan = tPrsParam.GetRejectTimeSpan();
	tPrsCfg.wRcvStartPort     = tPrsParam.GetEqpRecvPort();
    memset( tPrsCfg.achAlias, 0, sizeof(tPrsCfg.achAlias));
    memcpy( tPrsCfg.achAlias, tPrsParam.GetAlias(), MAXLEN_EQP_ALIAS );


	if( prsinit( &tPrsCfg ) )
	{
		OspPrintf( TRUE, FALSE, "Start local prs success!\n" );
	}
	else
	{
		OspPrintf( TRUE, FALSE, "Start local prs failure!\n" );
	}
#endif

	return;
}

/*====================================================================
    ������      : StartLocalBas
    ����        : �������ص�����������
    �㷨ʵ��    : 
    ����ȫ�ֱ���: ��
    �������˵��: u8 byEqpId �豸Id
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/11    3.0         ������       ����
====================================================================*/
void StartLocalBas( u8 byEqpId )
{
#if defined(_MINIMCU_)

	TEqpCfg	tEqpCfg;
	tEqpCfg.byEqpId = byEqpId;
	tEqpCfg.byEqpType = EQP_TYPE_BAS;
	tEqpCfg.dwConnectIP = g_cMcuAgent.GetMpcIp();
	tEqpCfg.dwLocalIP = g_cMcuAgent.GetMpcIp();
	tEqpCfg.wConnectPort = PORT_MCU;
	tEqpCfg.wMcuId = LOCAL_MCUID;

	TEqpBasInfo tBasParam;
	g_cMcuAgent.GetEqpBasCfg( byEqpId, &tBasParam );
	tEqpCfg.wRcvStartPort = tBasParam.GetEqpRecvPort();

	TMAPParam tMAPParam[3];
    ReadMAPConfig( tMAPParam, 3 );
		
//#ifdef WIN32
//	tEqpCfg.wMAPCount = 2;
//#else    
	tEqpCfg.wMAPCount = tBasParam.GetUsedMapNum();
	for( u8 byMapId = 0; byMapId < tEqpCfg.wMAPCount; byMapId++ )
	{
		tEqpCfg.m_atMap[byMapId].byMapId   = tBasParam.GetUsedMapId(byMapId);
        if (tEqpCfg.m_atMap[byMapId].byMapId == INVALID_MAP_ID)
        {
            // �����255,��ʾ��������Ƶ���䣬�������������
            continue;
        }
        else
        {
		    tEqpCfg.m_atMap[byMapId].dwPort    = tMAPParam[tEqpCfg.m_atMap[byMapId].byMapId].dwPort;
		    tEqpCfg.m_atMap[byMapId].dwCoreSpd = tMAPParam[tEqpCfg.m_atMap[byMapId].byMapId].dwCoreSpd;
		    tEqpCfg.m_atMap[byMapId].dwMemSize = tMAPParam[tEqpCfg.m_atMap[byMapId].byMapId].dwMemSize;
		    tEqpCfg.m_atMap[byMapId].dwMemSpd  = tMAPParam[tEqpCfg.m_atMap[byMapId].byMapId].dwMemSpd;
        }
	}
//#endif
    
    memset( tEqpCfg.achAlias, 0, sizeof(tEqpCfg.achAlias));
    memcpy( tEqpCfg.achAlias, tBasParam.GetAlias(), MAXLEN_EQP_ALIAS );
	// sprintf(tEqpCfg.achAlias, "Bas%d%c", byEqpId, 0);
	
    if (basInit(&tEqpCfg))
    {
        OspPrintf(TRUE, FALSE, "Start local bas success!\n");
    }
    else
    {
        OspPrintf(TRUE, FALSE, "Start local bas failure!\n");
    }

#endif	

	return;
}

/*====================================================================
    ������      : StartLocalEqp
    ����        : �������ص��豸
    �㷨ʵ��    : 
    ����ȫ�ֱ���: ��
    �������˵��: u8 byEqpId �豸Id
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/11    3.0         ������       ����
====================================================================*/
void StartLocalEqp( void )
{
	u8 byEqpId;
	u32 dwEqpIp;
	u8 byEqpType;

    // ����Ƿ����������衣���ڿ��ƻ�������
    u8 byEqpStart = 0;

	for( byEqpId=1 ;byEqpId < MAXNUM_MCU_PERIEQP ;byEqpId++ )
	{
		if( SUCCESS_AGENT == g_cMcuAgent.GetPeriInfo( byEqpId, &dwEqpIp, &byEqpType ) )
		{
			if( dwEqpIp == g_cMcuAgent.GetMpcIp() )
			{
				switch( byEqpType ) 
				{
				case EQP_TYPE_MIXER:
					StartLocalMixer( byEqpId );
                    byEqpStart ++;
					break;
					
				case EQP_TYPE_VMP:
					StartLocalVmp( byEqpId );
                    byEqpStart ++;
					break;

				case EQP_TYPE_BAS:
					StartLocalBas( byEqpId );
                    byEqpStart ++;
					break;
				
				case EQP_TYPE_PRS:
					StartLocalPrs( byEqpId );
                    byEqpStart ++;
					break;

				default:
					break;
				}
			}
		}
	}
#ifdef _VXWORKS
    if (byEqpStart != 0)
    {
        // ����ÿ������Ӧ�ô����Ҫ2M���ڴ�
        // �����Vx����������Ӧ�����裬Ӧ�����ټ�ȥһ����������        
        // g_cMcuVcApp.m_byMaxOngoConfSupported --;
    }
#endif
	
	return;
}

/*====================================================================
    ������      : StartLocalMp
    ����        : �������ص�Mp
    �㷨ʵ��    : 
    ����ȫ�ֱ���: ��
    �������˵��: ��
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/11    3.0         ������       ����
====================================================================*/
void StartLocalMp( void )
{
	u8  byMpcId  = (g_cMcuAgent.GetMpcBoardId() % MCU_BOARD_MPC == 0) ? 
                    MCU_BOARD_MPC : (g_cMcuAgent.GetMpcBoardId()%16);
	u32 dwMcuIp  = g_cMcuAgent.GetMpcIp();
	u16 wMcuPort = g_cMcuVcApp.GetMcuListenPort();

	TMp tMp;
	tMp.SetMcuId( LOCAL_MCUID );
	tMp.SetMpId( byMpcId );
	tMp.SetAttachMode( 1 );
	tMp.SetDoubleLink( 0 );
	tMp.SetIpAddr( ntohl(dwMcuIp) );

	//��������
	if( !mpstart( 0, dwMcuIp, wMcuPort, &tMp ) )
	{
		OspPrintf( TRUE, FALSE, "MCU: Start mp failure!\n" );
		return;
	}
	return;
}

/*====================================================================
    ������      : StartLocalMtadp
    ����        : �������ص�Mtadp
    �㷨ʵ��    : 
    ����ȫ�ֱ���: ��
    �������˵��: ��
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/11    3.0         ������       ����
====================================================================*/
void StartLocalMtadp( void )
{
	u8  byMpcId  = (g_cMcuAgent.GetMpcBoardId()%16 == 0) ? 16 : (g_cMcuAgent.GetMpcBoardId()%16);
	u32 dwMcuIp  = g_cMcuAgent.GetMpcIp();
	u16 wMcuPort = g_cMcuVcApp.GetMcuListenPort();

	if( 0 == dwMcuIp || inet_addr("127.0.0.1") == dwMcuIp ) 
	{
		OspPrintf( TRUE, FALSE, "LocalIP.%x is invalid reset localip start\n", dwMcuIp );

		u32 tdwIPList[5];  //�оٵ��ĵ�ǰ���õ�������Чip
		u16 dwIPNum;       //�оٵ�ip ��Ŀ

		dwIPNum = OspAddrListGet(tdwIPList, 5);
		if( 0 == dwIPNum )
		{
			OspPrintf( TRUE, FALSE, "LocalIP.%x is invalid reset localip err\n", dwMcuIp );
		}
		for(u16 dwAdapter=0; dwAdapter<dwIPNum; dwAdapter++)
		{
			if((inet_addr( "127.0.0.1" ) != tdwIPList[dwAdapter]))
			{
				OspPrintf( TRUE, FALSE, "LocalIP.%x is invalid reset localip.%x ok\n", tdwIPList[dwAdapter] );

				dwMcuIp = tdwIPList[dwAdapter];
				break;
			}
		}
	}

	TMtAdpConnectParam tConnectParam;
	TMtAdp tMtadp;
	
	tConnectParam.m_dwMcuTcpNode = 0;
	tConnectParam.m_wMcuPort = wMcuPort;
	tConnectParam.SetMcuIpAddr( ntohl(dwMcuIp) );
	// zgc, 2008-04-16, WIN32MCU����mtadp֧�ָ���
#ifdef WIN32
	tConnectParam.m_bSupportHD = TRUE;
#endif

	tMtadp.SetMtadpId( byMpcId );
	tMtadp.SetAttachMode( 1 );
	tMtadp.SetIpAddr( ntohl(dwMcuIp) );

	//�ն�����ģ�飨��Ƕ��ʽ������ģ�飩
	if( !MtAdpStart(&tConnectParam, &tMtadp) )
	{
		OspPrintf( TRUE, FALSE, "MCU: Start mtadp failure!\n" );
		return;
	}
	return;
}

/*====================================================================
    ������      : GetVmpChlNumByStyle
    ����        : �õ�����ϳɷ�������Ա��
    �㷨ʵ��    : 
    ����ȫ�ֱ���: ��
    �������˵��: byVMPStyle ����ϳɷ��
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/31    3.0         ������       ����
====================================================================*/
u8   GetVmpChlNumByStyle( u8 byVMPStyle )
{
	u8   byMaxMemNum = 0;

	switch( byVMPStyle ) 
	{
	case VMP_STYLE_ONE:
		byMaxMemNum = 1;
		break;
	case VMP_STYLE_VTWO:
	case VMP_STYLE_HTWO:
		byMaxMemNum = 2;
		break;
	case VMP_STYLE_THREE:
		byMaxMemNum = 3;
		break;
	case VMP_STYLE_FOUR:
		byMaxMemNum = 4;
		break;
	case VMP_STYLE_SIX:
		byMaxMemNum = 6;
		break;
	case VMP_STYLE_EIGHT:
		byMaxMemNum = 8;
		break;
	case VMP_STYLE_NINE:
		byMaxMemNum = 9;
		break;
	case VMP_STYLE_TEN:
		byMaxMemNum = 10;
		break;
	case VMP_STYLE_THIRTEEN:
		byMaxMemNum = 13;
		break;
	case VMP_STYLE_SIXTEEN:
		byMaxMemNum = 16;
		break;
	case VMP_STYLE_SPECFOUR:
		 byMaxMemNum = 4;
		break;
	case VMP_STYLE_SEVEN:
		 byMaxMemNum = 7;
		 break;
	case VMP_STYLE_TWENTY:// xliang [2/19/2009] 
		 byMaxMemNum = 20;
		 break;
	default:
		break;
	}

	return byMaxMemNum;
}


/*====================================================================
    ������      : PackTMtAliasArray
    ����        : ����ն˱������飨��������Ϣ��
    �㷨ʵ��    : 
    ����ȫ�ֱ���: ��
    �������˵��: TMtAlias *ptMtAlias �����������
				  u8 &byMtNum �����������
				  char *pszBuf ���Buf 
	              u16 wBufLen  ���Buf����
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/31    3.0         ������       ����
====================================================================*/
BOOL32 PackTMtAliasArray( TMtAlias *ptMtAlias, u16* pawMtDialRate, u8 byMtNum, 
					      char* const pszBuf, u16 &wBufLen )
{
	TMtAlias *ptMtTempAlias = ptMtAlias;
	char *pszTemp = pszBuf;
	u8 byMtCount = 0;

	if( pszBuf == NULL || ptMtAlias == NULL || byMtNum == 0 || byMtNum > MAXNUM_CONF_MT )
	{
        wBufLen = 0;
		return FALSE;
	}

    while( byMtCount < byMtNum )
	{
		if( !ptMtTempAlias->IsNull() )
		{
			*pszTemp++ = ptMtTempAlias->m_AliasType;
			if( ptMtTempAlias->m_AliasType == mtAliasTypeTransportAddress )
			{
				*pszTemp++ = sizeof(TTransportAddr);
				memcpy( pszTemp, &ptMtTempAlias->m_tTransportAddr, sizeof(TTransportAddr) );
				pszTemp = pszTemp + sizeof(TTransportAddr);
			}
			else
			{
				*pszTemp++ = strlen( ptMtTempAlias->m_achAlias );
				memcpy( pszTemp, ptMtTempAlias->m_achAlias, strlen( ptMtTempAlias->m_achAlias ) );
				pszTemp = pszTemp + strlen( ptMtTempAlias->m_achAlias );
			}

			//���ú�������
			*((u16*)pszTemp) = htons(*pawMtDialRate++);
			pszTemp += sizeof(u16);
		}

		ptMtTempAlias++;
		byMtCount++;
	}

	wBufLen = pszTemp - pszBuf;

	return TRUE;
}

/*====================================================================
    ������      : UnPackTMtAliasArray
    ����        : ����ն˱������飨��������Ϣ��
    �㷨ʵ��    : 
    ����ȫ�ֱ���: ��
    �������˵��: char *pszBuf ����Buf 
	              u16 wBufLen  ����Buf����
				   const TConfInfo *ptConfInfo,  ����Ļ�����Ϣ
				  TMtAlias *ptMtAlias �����������
				  u8*pawMtDialRate    ����ն˺����������飨������
				  u8 &byMtNum �����������
    ����ֵ˵��  : ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/03/31    3.0         ������       ����
====================================================================*/
BOOL32 UnPackTMtAliasArray(const char *pszBuf, u16 wBufLen, const TConfInfo *ptConfInfo, 
						  TMtAlias *ptMtAlias, u16* pawMtDialRate, u8 &byMtNum )
{
	u8 byMtCount = 0;
    const char *pszTemp = pszBuf;
	u8 byAliasLen;

	if( NULL == pszBuf || NULL == ptConfInfo || 
		NULL == ptMtAlias || NULL == pawMtDialRate )
	{
		return FALSE;
	}
 
	while( pszTemp - pszBuf < wBufLen )
	{
		ptMtAlias->m_AliasType = *pszTemp++;//�õ���������
	    if( ptMtAlias->m_AliasType != mtAliasTypeTransportAddress && 
			ptMtAlias->m_AliasType != mtAliasTypeE164 && 
			ptMtAlias->m_AliasType != mtAliasTypeH323ID && 
			ptMtAlias->m_AliasType != mtAliasTypeH320ID &&
			// PU����
			ptMtAlias->m_AliasType != puAliasTypeIPPlusAlias) //mtAliasTypeH320Alias���Ͳ�����������Ϣ�����Բ��ж�
		{
            byMtNum = byMtCount;
			return FALSE;
		}

		byAliasLen = *pszTemp++;//�õ���������
		if( byAliasLen > MAXLEN_ALIAS )
		{
            byMtNum = byMtCount;
			return FALSE;
		}

		if( ptMtAlias->m_AliasType == mtAliasTypeTransportAddress )
		{
			if( byAliasLen != sizeof(TTransportAddr) )
			{
                byMtNum = byMtCount;
				return FALSE;
			}
			memcpy( &ptMtAlias->m_tTransportAddr, pszTemp, byAliasLen );
		}
		else
		{
            memset( ptMtAlias->m_achAlias, 0, sizeof( ptMtAlias->m_achAlias) );
			memcpy( ptMtAlias->m_achAlias, pszTemp, byAliasLen );
		}
		pszTemp = pszTemp + byAliasLen;

		*pawMtDialRate = *((u16*)pszTemp);//�õ�����������Ϣ;
		*pawMtDialRate = ntohs(*pawMtDialRate);
	
		if (*pawMtDialRate == 0 || *pawMtDialRate > ptConfInfo->GetBitRate())
		{
			*pawMtDialRate = ptConfInfo->GetBitRate();			
		}

		pawMtDialRate++;
		pszTemp += sizeof(u16);

		ptMtAlias++;
		byMtCount++;
	}

	byMtNum = byMtCount;
	return TRUE;
}

/*====================================================================
    ������      : GetAudioBitrate
    ����        : �õ���Ƶ�ı��������
    �㷨ʵ��    : 
    ����ȫ�ֱ���: ��
    �������˵��: byAudioType ��Ƶ����
    ����ֵ˵��  : ��Ƶ�ı��������
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/08/06    3.0         ������       ����
====================================================================*/
u16 GetAudioBitrate(u8 byAudioType)
{
	u16 wBitrate = 0;
	switch(byAudioType)
	{
	case MEDIA_TYPE_PCMA:
		wBitrate = AUDIO_BITRATE_G711A;
		break;
	case MEDIA_TYPE_PCMU:
		wBitrate = AUDIO_BITRATE_G711U;
		break;
	case MEDIA_TYPE_G722:
		wBitrate = AUDIO_BITRATE_G722;
		break;
	case MEDIA_TYPE_G7231:
		wBitrate = AUDIO_BITRATE_G7231;
		break;
	case MEDIA_TYPE_G728:
		wBitrate = AUDIO_BITRATE_G728;
		break;
	case MEDIA_TYPE_G729:
		wBitrate = AUDIO_BITRATE_G729;
		break;
	case MEDIA_TYPE_MP3:
		wBitrate = AUDIO_BITRATE_MP3;
		break;
//	case MEDIA_TYPE_G7221:
//		wBitrate = AUDIO_BITRATE_MP3;
//		break;
    case MEDIA_TYPE_G7221C:
        wBitrate = AUDIO_BITRATE_G7221C;
		break;
    case MEDIA_TYPE_AACLC:
    case MEDIA_TYPE_AACLD:
        wBitrate = AUDIO_BITRATE_AAC;
        break;
	default:
		break;
	}
	return wBitrate;
}

/*=============================================================================
    �� �� ���� AddMcInfo
    ��    �ܣ� ��ָ������mcu��Ϣ�������б���ά���������ӵĲ����ظ�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byMcuId
    �� �� ֵ�� TConfMcInfo* 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/02/19  3.6			����                  ����
=============================================================================*/
TConfMcInfo* TConfOtherMcTable::AddMcInfo(u8 byMcuId)
{
	s32 nIdle = MAXNUM_SUB_MCU;
	for(s32 nLoop=0; nLoop<MAXNUM_SUB_MCU; nLoop++)
	{
		if(m_atConfOtherMcInfo[nLoop].m_byMcuId == byMcuId)
		{
			return &(m_atConfOtherMcInfo[nLoop]);
		}
		if(nIdle == MAXNUM_SUB_MCU && m_atConfOtherMcInfo[nLoop].IsNull())
		{
			nIdle = nLoop;
		}
	}
	if(nIdle == MAXNUM_SUB_MCU)
	{
		return NULL;
	}
	m_atConfOtherMcInfo[nIdle].SetNull();
	m_atConfOtherMcInfo[nIdle].m_byMcuId = byMcuId;
	m_atConfOtherMcInfo[nIdle].m_tSpyMt.SetNull();
	m_atConfOtherMcInfo[nIdle].m_dwSpyViewId = OspTickGet();
	m_atConfOtherMcInfo[nIdle].m_dwSpyVideoId = OspTickGet()+10;
	
    ///FIXME: guzh [7/4/2007] Ȩ��֮�ƣ����ѡ���˲��������б��ᵼ��û�и��¼�MCU��״̬
    // ������ȰѸ�MCU�����״̬���úã���ʹ�÷��ԡ�ѡ������ý��Դ�ȼ��������ܹ����Խ��� 
    TMt tMt;
    tMt.SetMcuId(byMcuId);
    tMt.SetMtId(0);
    m_atConfOtherMcInfo[nIdle].m_atMtExt[0].SetMt(tMt);
    m_atConfOtherMcInfo[nIdle].m_atMtStatus[0].SetMt(tMt);
    
	return &(m_atConfOtherMcInfo[nIdle]);
}

/*=============================================================================
    �� �� ���� RemoveMcInfo
    ��    �ܣ� ��ָ������mcu��Ϣ�Ƴ����б������Ӧ��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ����
    �� �� ֵ�� BOOL32
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/02/19  3.6			����                  ����
=============================================================================*/
BOOL32 TConfOtherMcTable::RemoveMcInfo(u8 byMcuId)
{
	for(s32 nLoop=0; nLoop<MAXNUM_SUB_MCU; nLoop++)
	{
		if(m_atConfOtherMcInfo[nLoop].m_byMcuId == byMcuId)
		{
			memset( &m_atConfOtherMcInfo[nLoop], 0, sizeof(TConfMcInfo) );
			return TRUE;
		}
	}

	return FALSE;
}

/*=============================================================================
    �� �� ���� GetMcInfo
    ��    �ܣ� ��ȡָ���ļ���mcu��Ϣ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byMcuId
    �� �� ֵ�� TConfMcInfo* 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/02/19  3.6			����                  ����
=============================================================================*/
TConfMcInfo* TConfOtherMcTable::GetMcInfo(u8 byMcuId)
{
	for(s32 nLoop=0; nLoop<MAXNUM_SUB_MCU; nLoop++)
	{
		if(m_atConfOtherMcInfo[nLoop].m_byMcuId == byMcuId)
		{
			return &(m_atConfOtherMcInfo[nLoop]);
		}
	}
	return NULL;
}

/*=============================================================================
�� �� ���� SetMtInMixing
��    �ܣ� ����ĳ�¼�mcuĳ�ն��ڻ���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TMt tMt
�� �� ֵ�� void  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/7/28  4.0			������                  ����
=============================================================================*/
BOOL32  TConfOtherMcTable::SetMtInMixing(TMt tMt)
{
    TConfMcInfo *ptMcInfo = GetMcInfo(tMt.GetMcuId());
    if (NULL != ptMcInfo)
    {
        TMcMtStatus *ptStatus = ptMcInfo->GetMtStatus(tMt);
        if (NULL != ptStatus)
        {
            ptStatus->SetInMixing(TRUE);
            return TRUE;
        }
    }
    
    return FALSE;
}

/*=============================================================================
�� �� ���� IsMtInMixing
��    �ܣ� ĳ�¼�mcuĳ�ն��Ƿ��ڻ���
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TMt tMt
�� �� ֵ�� BOOL32  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/7/27  4.0			������                  ����
=============================================================================*/
BOOL32  TConfOtherMcTable::IsMtInMixing( TMt tMt)
{       
    TConfMcInfo *ptMcInfo = GetMcInfo(tMt.GetMcuId());
    if (NULL != ptMcInfo)
    {
        TMcMtStatus *ptStatus = ptMcInfo->GetMtStatus(tMt);
        if (NULL != ptStatus && ptStatus->IsInMixing())
        {
            return TRUE;
        }        
    }

    return FALSE;
}

/*=============================================================================
�� �� ���� ClearMtInMixing
��    �ܣ� ���ĳmcu���ն˻���״̬
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TMcu tMcu
�� �� ֵ�� void  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/7/28  4.0			������                  ����
=============================================================================*/
BOOL32   TConfOtherMcTable::ClearMtInMixing(TMcu tMcu)
{
    TMt tMt;
    tMt.SetMcuId(tMcu.GetMcuId());
    TConfMcInfo *ptMcInfo = GetMcInfo(tMcu.GetMcuId());
    if (NULL != ptMcInfo)
    {
        for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
        {
            tMt.SetMtId(byMtId);
            TMcMtStatus *ptStatus = ptMcInfo->GetMtStatus(tMt);
            if (NULL != ptStatus)
            {
                ptStatus->SetInMixing(FALSE);
            }
        } 

        return TRUE;
    }

    return FALSE;
}

/*====================================================================
    ������      ��TableMemoryFree
    ����        ���ͷŶ�̬������ڴ�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/29    3.5         ������         ����
====================================================================*/
void TableMemoryFree( s8 **ppMem, u32 dwEntryNum )
{
    if( ppMem == NULL )
    {
        return;
    }
	
    for(u32 dwLoop = 0; dwLoop < dwEntryNum; dwLoop++ )
    {
        if( ppMem[dwLoop] != NULL )
        {
            free( ppMem[dwLoop] );
        }
    }
	
    free( ppMem );
	
	return;
}

/*====================================================================
    ������      ��GetMAPMemSpeed
    ����        ���õ�map�ڴ���Ƶ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��u32
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/29    3.5         ������         ����
====================================================================*/
u32 GetMAPMemSpeed(u32 dwCoreSpeed)
{
	u16    wLoop=0;
	static u32 adwCoreSpdToMemSpdTable[][2] = 
	{
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
    ������      ��ReadMAPConfig
    ����        ���õ�map������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��BOOL32
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/01/29    3.5         ������         ����
====================================================================*/
BOOL32 ReadMAPConfig( TMAPParam tMAPParam[], u8 byMapCount )
{
    s8    achProfileName[32];
    s8*   *lpszTable;
    s8    chSeps[] = " \t";       
    s8    *pchToken;
    u32   dwLoop;
    BOOL32  bResult = TRUE;
    u32   dwMemEntryNum;
 
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "mapcfg.ini");
   
    bResult = GetRegKeyInt( achProfileName, "MAPTable", STR_ENTRY_NUM, 
                            0, (s32*)&dwMemEntryNum );
	if( bResult == FALSE ) 
	{
		memset( tMAPParam, 0, byMapCount*sizeof(TMAPParam) );
		return FALSE;
	}
	if( dwMemEntryNum != byMapCount )
	{
		memset( tMAPParam, 0, byMapCount*sizeof(TMAPParam) );
		return FALSE;
	}

	//alloc memory
    lpszTable = (s8**)malloc( dwMemEntryNum * sizeof( s8* ) );
    for( dwLoop = 0; dwLoop < dwMemEntryNum; dwLoop++ )
    {
        lpszTable[dwLoop] = (s8*)malloc( MAX_VALUE_LEN + 1 );
    }

	//get the map table
    bResult = GetRegKeyStringTable( achProfileName, "MAPTable",
                                    "fail", lpszTable, &dwMemEntryNum, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )
    {
		memset( tMAPParam, 0, byMapCount*sizeof(TMAPParam) );
		return FALSE;
    }
	if( dwMemEntryNum != byMapCount )
	{
		memset( tMAPParam, 0, byMapCount*sizeof(TMAPParam) );
		return FALSE;
	}

    // analyze entry strings
    for( dwLoop = 0; dwLoop < dwMemEntryNum; dwLoop++ )
    {
        pchToken = strtok( lpszTable[dwLoop], chSeps );
        if( pchToken == NULL )
        {
            bResult = FALSE;
        }
        else
        {
            tMAPParam[dwLoop].byMapId = atoi( pchToken );
        }

        // MAP��������Ƶ
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            bResult = FALSE;
        }
        else
        {
            tMAPParam[dwLoop].dwCoreSpd = atoi( pchToken );
        }

		tMAPParam[dwLoop].dwMemSpd = GetMAPMemSpeed( tMAPParam[dwLoop].dwCoreSpd );

        // MAP�������ڴ�����
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            bResult = FALSE;
        }
        else
        {
            tMAPParam[dwLoop].dwMemSize = atoi( pchToken );
        }
        // MAP��������Ƶ�˿�
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            bResult = FALSE;
        }
        else
        {
            tMAPParam[dwLoop].dwPort = atoi( pchToken );
        }
    }

    // free memory
    TableMemoryFree( lpszTable, dwMemEntryNum );
    
    return bResult;
}


u8 *GetRandomKey()
{
	static u8 abyKey[MAXLEN_KEY];
	u32 dwTemp;
	memset( abyKey, 0, sizeof( abyKey ) );
    srand(time(NULL));
	//rand
	dwTemp = rand();
	memcpy( abyKey, &dwTemp, sizeof( u32 ) );
    //tick
	dwTemp = OspTickGet();
	memcpy(abyKey + 4, &dwTemp, sizeof(u32));
	
	//tick1
	dwTemp = rand();
	memcpy(abyKey + 8, &dwTemp, sizeof(u32));

	//tick2
	dwTemp = rand();
	memcpy(abyKey + 12, &dwTemp, sizeof(u32));

    return abyKey;
}

/*=============================================================================
    �� �� ���� GetActivePayload
    ��    �ܣ� 
    �㷨ʵ�֣� ǰ������ͨ�� ͳһ���� MEDIA_TYPE_FEC,��ǰ������������������߼�
    ȫ�ֱ����� 
    ��    ���� u32 dwIP   ������IP��ַ
    �� �� ֵ�� char * 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2007/10/11  4.0			�ű���                ���� ���ڻ����FEC���ͷ��ض�̬�غ�
=============================================================================*/
u8 GetActivePayload(const TConfInfo &tConfInfo, u8 byRealPayloadType)
{
    u8 byAPayload = MEDIA_TYPE_NULL;
    u8 byFECType = FECTYPE_NONE;

    switch(byRealPayloadType)
    {
    case MEDIA_TYPE_PCMA:
    case MEDIA_TYPE_PCMU:
    case MEDIA_TYPE_G721:
    case MEDIA_TYPE_G722:
    case MEDIA_TYPE_G7231:
    case MEDIA_TYPE_G728:
    case MEDIA_TYPE_G729:
    case MEDIA_TYPE_MP3:
    case MEDIA_TYPE_G7221C:
    case MEDIA_TYPE_G7221:
    case MEDIA_TYPE_ADPCM:
    case MEDIA_TYPE_AACLC:
    case MEDIA_TYPE_AACLD:

        byFECType = tConfInfo.GetCapSupportEx().GetAudioFECType();
    	break;

    case MEDIA_TYPE_H261:
    case MEDIA_TYPE_H262:
    case MEDIA_TYPE_H263:
    case MEDIA_TYPE_MP4:
    case MEDIA_TYPE_H263PLUS:
    case MEDIA_TYPE_H264:

        byFECType = tConfInfo.GetCapSupportEx().GetVideoFECType();
        break;

    case MEDIA_TYPE_H224:
    case MEDIA_TYPE_MMCU:
        break;
        
    default:
        OspPrintf( TRUE, FALSE, "[GetActivePayload] unexpected media type.%d, ignore it\n", byRealPayloadType );
        break;
    }

    if ( FECTYPE_NONE != byFECType && MEDIA_TYPE_H224 != byRealPayloadType )
    {
        byAPayload = MEDIA_TYPE_FEC;
    }
    else
    {
        BOOL32 bNewActivePT = CONF_ENCRYPTMODE_NONE != tConfInfo.GetConfAttrb().GetEncryptMode() ? TRUE : FALSE;
            
        // guzh [9/26/2007] H264/G7221.C(������)/MP4/MP3/ADPCM(������) ��̬�غɺ�ԭ�غ�һ��
	    switch(byRealPayloadType)
	    {
	    case MEDIA_TYPE_PCMA:   byAPayload = ACTIVE_TYPE_PCMA;  break;
	    case MEDIA_TYPE_PCMU:   byAPayload = ACTIVE_TYPE_PCMU;  break;
	    case MEDIA_TYPE_G721:   byAPayload = ACTIVE_TYPE_G721;  break;
	    case MEDIA_TYPE_G722:   byAPayload = ACTIVE_TYPE_G722;  break;
	    case MEDIA_TYPE_G7231:  byAPayload = ACTIVE_TYPE_G7231; break;
	    case MEDIA_TYPE_G728:   byAPayload = ACTIVE_TYPE_G728;  break;
	    case MEDIA_TYPE_G729:   byAPayload = ACTIVE_TYPE_G729;  break;
	    case MEDIA_TYPE_H261:   byAPayload = ACTIVE_TYPE_H261;  break;
	    case MEDIA_TYPE_H262:   byAPayload = ACTIVE_TYPE_H262;  break;
	    case MEDIA_TYPE_H263:   byAPayload = ACTIVE_TYPE_H263;  break;
        case MEDIA_TYPE_ADPCM:  byAPayload = bNewActivePT ? ACTIVE_TYPE_ADPCM : byRealPayloadType;  break;
        case MEDIA_TYPE_G7221C: byAPayload = bNewActivePT ? ACTIVE_TYPE_G7221C : byRealPayloadType; break;
        case MEDIA_TYPE_AACLC:   byAPayload = MEDIA_TYPE_AACLC;  break;
        case MEDIA_TYPE_AACLD:   byAPayload = MEDIA_TYPE_AACLD;  break;
	    default:                byAPayload = byRealPayloadType; break;
	    }
    }

	return byAPayload;
}

// �ж�Դ�˷ֱ����Ƿ����Ŀ�Ķ˷ֱ���,�����򷵻�TRUE,С�ڵ����򷵻�FALSE
BOOL32 IsResG(u8 bySrcRes, u8 byDstRes)
{
    if (VIDEO_FORMAT_AUTO == bySrcRes ||
        VIDEO_FORMAT_AUTO == byDstRes)
    {
        return FALSE;
    }

    if (!IsDSResMatched(bySrcRes, byDstRes))
    {
        return FALSE;
    }

    const s32 s_anRes[] = { VIDEO_FORMAT_SQCIF_96x80,
                            VIDEO_FORMAT_SQCIF_112x96,
                            VIDEO_FORMAT_SIF,
                            VIDEO_FORMAT_2SIF,
                            VIDEO_FORMAT_CIF,
                            VIDEO_FORMAT_2CIF,
                            VIDEO_FORMAT_VGA,
                            VIDEO_FORMAT_4SIF,
                            VIDEO_FORMAT_4CIF,
                            VIDEO_FORMAT_SVGA,
                            VIDEO_FORMAT_XGA,
                            VIDEO_FORMAT_HD720,
                            VIDEO_FORMAT_SXGA, 
                            VIDEO_FORMAT_16CIF,
                            VIDEO_FORMAT_HD1080              
                           };

    u8 bySrcResIdx = 0xff;
    u8 byDstResIdx = 0xff;
    u8 byLop = 0;

    // ��¼�ڷֱ��������е�λ�ã����ڱȽϷֱ��ʴ�С
    for ( byLop = 0; byLop < sizeof(s_anRes)/sizeof(s32); byLop++ )
    {
        if ( s_anRes[byLop] == bySrcRes )
        {
            if ( bySrcResIdx == 0xff )
            {
                bySrcResIdx = byLop;    
            }
        }
        if ( s_anRes[byLop] == byDstRes )
        {
            if ( byDstResIdx == 0xff )
            {
                byDstResIdx = byLop;
            }
        }
        if ( bySrcResIdx != 0xff && byDstResIdx != 0xff )
        {
            break;
        }
    }

    if (bySrcResIdx == 0xff || byDstResIdx == 0xff)
    {
        OspPrintf(TRUE, FALSE, "[IsResGE] unexpected res <src.%d, dst.%d>\n", bySrcRes, byDstRes);
        return FALSE;
    }

    if (bySrcResIdx <= byDstResIdx)
    {
        return FALSE;
    }
    return TRUE;
}
// �ж�Դ�˷ֱ����Ƿ���ڵ���Ŀ�Ķ˷ֱ���,���ڵ����򷵻�TRUE,С���򷵻�FALSE
BOOL32 IsResGE(u8 bySrcRes, u8 byDstRes)
{
	if (IsResG(bySrcRes, byDstRes) || bySrcRes == byDstRes)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL32 IsDSResMatched(u8 bySrcRes, u8 byDstRes)
{
	BOOL32 bMatched = FALSE;
	if (IsResPresentation(bySrcRes) && IsResPresentation(byDstRes))
	{
		bMatched = TRUE;
	}
	else if (IsResLive(bySrcRes) && IsResLive(byDstRes))
	{
		bMatched = TRUE;
	}
	return bMatched;
}

BOOL32 IsResPresentation(u8 byRes)
{
	BOOL32 bPresentation = FALSE;

	switch(byRes)
	{
	case VIDEO_FORMAT_VGA:
	case VIDEO_FORMAT_SVGA:
	case VIDEO_FORMAT_XGA:
	case VIDEO_FORMAT_SXGA:
	case VIDEO_FORMAT_UXGA:
		bPresentation = TRUE;
		break;
	default:
		break;
	}
	return bPresentation;
}

BOOL32 IsResLive(u8 byRes)
{
	BOOL32 bLive = FALSE;
	
	switch(byRes)
	{
	case VIDEO_FORMAT_CIF:
	case VIDEO_FORMAT_2CIF:
	case VIDEO_FORMAT_4CIF:
	case VIDEO_FORMAT_16CIF:
	
	case VIDEO_FORMAT_SIF:
	case VIDEO_FORMAT_2SIF:
	case VIDEO_FORMAT_4SIF:

	case VIDEO_FORMAT_W4CIF:
	case VIDEO_FORMAT_HD720:
	case VIDEO_FORMAT_HD1080:
		bLive = TRUE;
		break;
	//Auto������
	case VIDEO_FORMAT_AUTO:
	//DS������SQCIF
	case VIDEO_FORMAT_SQCIF_112x96:
	case VIDEO_FORMAT_SQCIF_96x80:
		break;
	default:
		break;
	}
	return bLive;
}

u8 GetChnType(const TEqp &tEqp, u8 byChIdx)
{
    u8 byChnType = MAU_CHN_NONE;
    
    if (tEqp.IsNull())
    {
        //OspPrintf(TRUE, FALSE, "[GetChnType] tEqp.IsNull()!\n");
        return byChnType;
    }

    if (!g_cMcuVcApp.IsPeriEqpValid(tEqp.GetEqpId()))
    {
        OspPrintf(TRUE, FALSE, "[GetChnType] Eqp.%d EqpValid = 0!\n", tEqp.GetEqpId());
        return byChnType;
    }
    /*
    if (!g_cMcuVcApp.IsPeriEqpConnected(tEqp.GetEqpId()))
    {
        OspPrintf(TRUE, FALSE, "[GetChnType] Eqp.%d Connected = 0!\n", tEqp.GetEqpId());
        return byChnType;
    }*/
    if (!g_cMcuAgent.IsEqpBasHD(tEqp.GetEqpId()))
    {
        OspPrintf(TRUE, FALSE, "[GetChnType] Eqp.%d IsEqpBasHD = 0!\n", tEqp.GetEqpId());
        return byChnType;
    }
    
    TPeriEqpStatus tStatus;
    if (!g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tStatus))
    {
        OspPrintf(TRUE, FALSE, "[GetChnType] GetPeriEqpStatus failed!\n");
        return byChnType;
    }
    
    u8 byMauType = tStatus.m_tStatus.tHdBas.GetEqpType();
    if (TYPE_MAU_NORMAL == byMauType)
    {
        if (0 == byChIdx)
        {
            byChnType = MAU_CHN_NORMAL;
        }
        else if (1 == byChIdx)
        {
            byChnType = MAU_CHN_VGA;
        }
    }
    else if (TYPE_MAU_H263PLUS == byMauType)
    {
        if (0 == byChIdx)
        {
            byChnType = MAU_CHN_263TO264;
        }
        else if (1 == byChIdx)
        {
            byChnType = MAU_CHN_264TO263;
        }
    }
    else if(TYPE_MPU == byMauType)
    {
        if (byChIdx < 4)
        {
            byChnType = MPU_CHN_NORMAL;
        }
    }

    if (MAU_CHN_NONE == byChnType)
    {
        OspPrintf(TRUE, FALSE, "[GetChnType] failed for eqp<%d, %d>!\n", tEqp.GetEqpId(), byChIdx);
    }
    return byChnType;
}


/*=============================================================================
    �� �� ���� StrOfIP
    ��    �ܣ� ������IP��ַת���ַ���
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u32 dwIP   ������IP��ַ
    �� �� ֵ�� char * 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/02/28  3.6			����                  ����
=============================================================================*/
char * StrOfIP( u32 dwIP )
{
    dwIP = htonl(dwIP);
	static char strIP[17];  
	u8 *p = (u8 *)&dwIP;
	sprintf(strIP,"%d.%d.%d.%d%c",p[0],p[1],p[2],p[3],0);
	return strIP;
}


/*=============================================================================
�� �� ���� GetAddrBookPath
��    �ܣ� ��ȡ��ַ���ļ�·��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� s8 * 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/3/23  4.0			������                  ����
=============================================================================*/
s8 * GetAddrBookPath(void)
{
    static s8 szPath[128];
    sprintf(szPath, "%s/%s", DIR_DATA, MCUADDRFILENAME);
    return szPath;
}

/*=============================================================================
�� �� ���� GetMcuType
��    �ܣ� ��ȡMCU��Ʒ(���)����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� u8
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/03/08  4.0			����                  ����
=============================================================================*/
u8 GetMcuPdtType(void)
{
    // �������µķ�ʽд��Ϊ�˷���WindowsMCUģ����������
#ifdef _MINIMCU_
    #ifdef _MCU8000C_
        return MCU_TYPE_KDV8000C;
    #else
        if ( !g_cMcuAgent.IsMcu8000BHD() )
        {
            return MCU_TYPE_KDV8000B;
        }
        else
        {
            return MCU_TYPE_KDV8000B_HD;
        }        
    #endif
#else
    #ifdef WIN32
        return MCU_TYPE_WIN32;
    #else
        return MCU_TYPE_KDV8000;
    #endif
#endif
}

/*=============================================================================
�� �� ���� GetMtLeftReasonStr
��    �ܣ� ��ȡMCU�ն��뿪��ԭ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� u8
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/03/08  4.0			����                  ����
=============================================================================*/
const s8* GetMtLeftReasonStr(u8 byReason)
{
    static s8* gs_szLeftReason[] = {"Unknown", "Exception", "Normal", "RTD", "DRQ", "TypeUnmatch", "Busy",
                                    "Reject", "Unreachable", "Local", "BusyExt" };

    if ( byReason >= MTLEFT_REASON_EXCEPT && byReason <= MTLEFT_REASON_BUSYEXT )
    {
        return gs_szLeftReason[byReason];
    }
    else
    {
        return gs_szLeftReason[0];
    }  
}

/*=============================================================================
�� �� ���� GetMcuVersion
��    �ܣ� ��ȡMCU��ϵͳ�汾��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/09/03  4.0			����                  ����
=============================================================================*/
const s8* GetMcuVersion()
{
    static s8 gs_VersionBuf[128] = {0};
    if ( strlen(gs_VersionBuf) != 0 )
    {
        return gs_VersionBuf;
    }
        
    strcpy(gs_VersionBuf, KDV_MCU_PREFIX);

    /*
    struct stat buf;
    s32 nResult = stat( APPNAME, &buf );

    if( nResult != 0 )
    {
        return gs_VersionBuf;
    }
    else
    {
        // ��ȡ�ļ�����ʱ�䣬��Ϊ����ʱ��
        TKdvTime tTime;
        tTime.SetTime(&buf.st_ctime);
        s8 achBuf[16] = {0};
        sprintf(achBuf, ".%04d%02d%02d", tTime.GetYear(), tTime.GetMonth(), tTime.GetDay());
        sprintf(gs_VersionBuf, VERSION_MCU_PREFIX, achBuf);        
    } 
    */
    
    s8 achMon[16] = {0};
    u32 byDay = 0;
    u32 byMonth = 0;
    u32 wYear = 0;
    static s8 achFullDate[24] = {0};
    
    s8 achDate[32] = {0};
    sprintf(achDate, "%s", __DATE__);
    StrUpper(achDate);
    
    sscanf(achDate, "%s %d %d", achMon, &byDay, &wYear );
    
    if ( 0 == strcmp( achMon, "JAN") )		 
        byMonth = 1;
    else if ( 0 == strcmp( achMon, "FEB") )
        byMonth = 2;
    else if ( 0 == strcmp( achMon, "MAR") )
        byMonth = 3;
    else if ( 0 == strcmp( achMon, "APR") )		 
        byMonth = 4;
    else if ( 0 == strcmp( achMon, "MAY") )
        byMonth = 5;
    else if ( 0 == strcmp( achMon, "JUN") )
        byMonth = 6;
    else if ( 0 == strcmp( achMon, "JUL") )
        byMonth = 7;
    else if ( 0 == strcmp( achMon, "AUG") )
        byMonth = 8;
    else if ( 0 == strcmp( achMon, "SEP") )		 
        byMonth = 9;
    else if ( 0 == strcmp( achMon, "OCT") )
        byMonth = 10;
    else if ( 0 == strcmp( achMon, "NOV") )
        byMonth = 11;
    else if ( 0 == strcmp( achMon, "DEC") )
        byMonth = 12;
    else
        byMonth = 0;
    
    if ( byMonth != 0 )
    {
        sprintf(achFullDate, "%04d%02d%02d", wYear, byMonth, byDay);
        sprintf(gs_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
    }
    else
    {
        // for debug information
        sprintf(gs_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
    }
    
    return gs_VersionBuf;
}


//mcu dcs���Դ�ӡ�ӿ�
BOOL32 g_bPrtDcsMsg = FALSE;
API void pdcsmsg( void )
{
	g_bPrtDcsMsg = TRUE;
}

API void npdcsmsg( void )
{
	g_bPrtDcsMsg = FALSE;
}

//mcs���Դ�ӡ�ӿ�
BOOL32 g_bPrintMcsMsg = FALSE;
API void pmcsmsg( void )
{
    g_bPrintMcsMsg = TRUE;
}

API void npmcsmsg( void )
{
    g_bPrintMcsMsg = FALSE;
}

//vcs���Դ�ӡ�ӿ�
BOOL32 g_bPrintVcsMsg = FALSE;
API void pvcsmsg( void )
{
    g_bPrintVcsMsg = TRUE;
	//ͬʱ��mcs����Ϣ��Ӧ��
    g_bPrintMcsMsg = TRUE;
}

API void npvcsmsg( void )
{
    g_bPrintVcsMsg = FALSE;
	//ͬʱ��mcs����Ϣ��Ӧ��
    g_bPrintMcsMsg = FALSE;
}

//eqp���Դ�ӡ�ӿ�
BOOL32 g_bPrintEqpMsg = FALSE;
API void peqpmsg( void )
{
    g_bPrintEqpMsg = TRUE;
}

API void npeqpmsg( void )
{
    g_bPrintEqpMsg = FALSE;
}

//mmcu���Խӿ�
BOOL32 g_bpMMcuMsg = FALSE;
API void pmmcumsg(void)
{
    g_bpMMcuMsg = TRUE;
}
API void npmmcumsg(void)
{
    g_bpMMcuMsg = FALSE;
}

//mt
BOOL32 g_bPMt2Msg = FALSE;
API void pmt2msg( void )
{
    g_bPMt2Msg = TRUE;
}

API void npmt2msg( void )
{
    g_bPMt2Msg = FALSE;
}

//mt call
BOOL32 g_bPrintCallMsg = FALSE;
API void pcallmsg( void )
{
    g_bPrintCallMsg = TRUE;
}

API void npcallmsg( void )
{
    g_bPrintCallMsg = FALSE;
}

//mp
BOOL32 g_bpMpMgrMsg = FALSE;
API void pmpmgrmsg(void)
{
    g_bpMpMgrMsg = TRUE;
}
API void npmpmgrmsg(void)
{
    g_bpMpMgrMsg = FALSE;
}

//cfg
BOOL32 g_bPrintCfgMsg = FALSE;
API void pcfgmsg( void )
{
    g_bPrintCfgMsg = TRUE;
}

API void npcfgmsg( void )
{
    g_bPrintCfgMsg = FALSE;
}

//mt
BOOL32 g_bPrintMtMsg = FALSE;
API void pmtmsg( void )
{
    g_bPrintMtMsg = TRUE;
}

API void npmtmsg( void )
{
    g_bPrintMtMsg = FALSE;
}

//guard
BOOL32 g_bPrintGdMsg = FALSE;
API void pgdmsg( void )
{
    g_bPrintGdMsg = TRUE;
}

API void npgdmsg( void )
{
    g_bPrintGdMsg = FALSE;
}

//satelite
BOOL32 g_bPrintSatMsg = FALSE;
API void psatmsg( void )
{
    g_bPrintSatMsg = TRUE;
}

API void npsatmsg( void )
{
    g_bPrintSatMsg = FALSE;
}

BOOL32 g_bPrintNPlusMsg = FALSE;

API void pnplusmsg( void )
{
    g_bPrintNPlusMsg = TRUE;
}

API void npnplusmsg( void )
{
    g_bPrintNPlusMsg = FALSE;
}

API void ppfmmsg( void )
{
    g_bPrintPfmMsg = TRUE;
}

API void nppfmmsg( void )
{
    g_bPrintPfmMsg = FALSE;
}

API void sconftotemp( s8* psConfName )
{
	if ( psConfName == NULL )
	{
		OspPrintf( TRUE, FALSE, "ERROR: The name is null!\n" );
		return;
	}

	CConfId cConfId = g_cMcuVcApp.GetConfIdByName( (LPCSTR)psConfName, FALSE );
	if( cConfId.IsNull() )
	{
		OspPrintf( TRUE, FALSE, "ERROR: The conference %s is not exist! The name maybe error!\n", psConfName );
		return;
	}
	u8 byConfIdx = g_cMcuVcApp.GetConfIdx( cConfId );
	CServMsg cServMsg;
	cServMsg.SetMsgBody( &byConfIdx, sizeof(u8) );
	OspPost(MAKEIID( AID_MCU_VC, CInstance::DAEMON ), MCS_MCU_SAVECONFTOTEMPLATE_REQ, 
				cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	return;
}

API void resetlogin( void )
{
	TLoginInfo tLoginInfo;
	tLoginInfo.SetUser("admin");
	tLoginInfo.SetPwd("admin");
	if ( SUCCESS_AGENT != g_cMcuAgent.SetLoginInfo(&tLoginInfo) )
	{
		OspPrintf( TRUE, FALSE, "Reset login info failed!\n" );
	}
	return;
}

/*=============================================================================
�� �� ���� NPlusLog
��    �ܣ� N+1���ݵ��Դ�ӡ�ӿ�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  s8 * fmt
           ...
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/11/21  4.0			������                  ����
=============================================================================*/
void NPlusLog( s8 * fmt, ... )
{
    s8  achBuf[255];
    va_list argptr;    
    if( g_bPrintNPlusMsg )
    {
        s32 nPrefix = sprintf( achBuf, "[McuNPlus]:" );
        va_start( argptr, fmt );    
        vsprintf( achBuf + nPrefix, fmt, argptr );   
        OspPrintf(TRUE, FALSE, achBuf); 
        va_end(argptr); 
    }
}

/*=============================================================================
�� �� ���� McsLog
��    �ܣ� mcs���Դ�ӡ�ӿ�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  s8 * fmt
           ...
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/3/23  4.0			������                  ����
=============================================================================*/
void McsLog( s8 * fmt, ... )
{
    s8  achBuf[255];
    va_list argptr;    
    if( g_bPrintMcsMsg )
    {
        s32 nPrefix = sprintf( achBuf, "[McuMcs]:" );
        va_start( argptr, fmt );    
        vsprintf( achBuf + nPrefix, fmt, argptr );   
        OspPrintf(TRUE, FALSE, achBuf); 
        va_end(argptr); 
    }
}

/*=============================================================================
  �� �� ���� DcsLog
  ��    �ܣ� dcs��Ϣ��ӡ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� s8 * pszFmt ...
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/13    4.0			�ű���                  ����
=============================================================================*/
void DcsLog( s8 * fmt, ... )
{
    s8  achBuf[255];
    va_list argptr;	
    if( g_bPrtDcsMsg )
    {
        s32 nPrefix = sprintf( achBuf, "[MCUDCS]:" );
        va_start( argptr, fmt );    
        vsprintf( achBuf + nPrefix, fmt, argptr );   
        OspPrintf(TRUE, FALSE, achBuf); 
        va_end(argptr); 
    }
}

/*====================================================================
    ������      ��EqpLog
    ����        �������ӡ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    05/09/05    4.0         libo          ����
====================================================================*/
void EqpLog(s8 * fmt, ...)
{   
	s8 achBuf[255];
    va_list argptr;
    if (g_bPrintEqpMsg)
    {
        s32 nPrefix = sprintf( achBuf, "[Eqp]: " );
        va_start(argptr, fmt);    
        vsprintf(achBuf + nPrefix, fmt, argptr);   
        OspPrintf(TRUE, FALSE, achBuf);
        va_end(argptr); 	
    }
}

/*=============================================================================
    �� �� ���� MMcuLog
    ��    �ܣ� ������Ϣ��ӡ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ����char * fmt
               ...
    �� �� ֵ�� void  
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/4/14   3.6			����                  ����
=============================================================================*/
void  MMcuLog(s8 * fmt,...)
{
	s8 achBuf[255];
    va_list argptr;
	if( g_bpMMcuMsg )
	{
		s32 nPrefix = sprintf( achBuf, "[MMCU]:" );
		va_start(argptr, fmt);    
		vsprintf( achBuf+nPrefix, fmt, argptr );   
		OspPrintf(TRUE, FALSE, achBuf); 
		va_end(argptr); 
	}
}

/*====================================================================
    ������      ��CallLog
    ����        ��������Ϣ��ӡ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/17    1.0         ������        ����
====================================================================*/	
void  CallLog(s8 * fmt,...)
{
	s8 achBuf[255];
    va_list argptr;
	if( g_bPrintCallMsg )
	{
		s32 nPrefix = sprintf( achBuf, "[CALL]:" );
		va_start(argptr, fmt);    
		vsprintf( achBuf+nPrefix, fmt, argptr );   
		OspPrintf(TRUE, FALSE, achBuf); 
		va_end(argptr); 
	}
}

/*====================================================================
    ������      ��Mt2Log
    ����        �����ն�ͨѶ��һЩ������Ϣ��ӡ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/17    1.0         ������        ����
====================================================================*/	
void  Mt2Log(s8 * fmt,...)
{
	s8 achBuf[255];
    va_list argptr;
	if( g_bPMt2Msg )
	{
		s32 nPrefix = sprintf( achBuf, "[MTCOM]:" );
		va_start(argptr, fmt);    
		vsprintf( achBuf+nPrefix, fmt, argptr );   
		OspPrintf(TRUE, FALSE, achBuf); 
		va_end(argptr); 
	}
}

/*=============================================================================
�� �� ���� MtLog
��    �ܣ� �ն������ӡ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� s8 * fmt
           ...
�� �� ֵ�� void  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/3/24  4.0			������                  ����
=============================================================================*/
void  MtLog(s8 * fmt,...)
{
    s8 achBuf[255];
    va_list argptr;
    if( g_bPrintMtMsg )
    {
        s32 nPrefix = sprintf( achBuf, "[McuMtadp]:" );
        va_start(argptr, fmt);    
        vsprintf( achBuf+nPrefix, fmt, argptr );   
        OspPrintf(TRUE, FALSE, achBuf); 
        va_end(argptr); 
    }
}

/*====================================================================
    ������      ��MpManagerLog
    ����        ��Mp��Ϣ��ӡ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/07/17    1.0         ������        ����
====================================================================*/	
void  MpManagerLog(s8 * fmt,...)
{
	s8 achBuf[255];
    va_list argptr;
    if (g_bpMpMgrMsg)
    {
        s32 nPrefix = sprintf( achBuf, "[MpManager]:" );
        va_start(argptr, fmt);
        vsprintf(achBuf+nPrefix,fmt,argptr);          
        OspPrintf(TRUE, FALSE, achBuf);
        va_end(argptr);
    }
}

/*====================================================================
  �� �� ����CfgLog
  ��    �ܣ�������Ϣ��ӡ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void  
====================================================================*/	
void CfgLog(s8 * fmt,...)
{
	s8 szBuf[255];
	va_list argptr;
	if (g_bPrintCfgMsg)
	{
		s32 nPrefix = sprintf( szBuf, "[MCUCFG]:" );
		va_start(argptr, fmt);
		vsprintf( szBuf + nPrefix, fmt, argptr );
		OspPrintf(TRUE, FALSE, szBuf);
		va_end(argptr);
	}
}

/*=============================================================================
�� �� ���� GuardLog
��    �ܣ� mcu������ӡ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� s8 * fmt
           ...
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/3/24  4.0			������                  ����
=============================================================================*/
void GuardLog(s8 * fmt,...)
{
    s8 szBuf[255];
    va_list argptr;
    if (g_bPrintGdMsg)
    {
        s32 nPrefix = sprintf( szBuf, "[MCUGuard]:" );
        va_start(argptr, fmt);
        vsprintf( szBuf + nPrefix, fmt, argptr );
        OspPrintf(TRUE, FALSE, szBuf);
        va_end(argptr);
    }    
}

/*=============================================================================
�� �� ���� VcsLog
��    �ܣ� vcs���Դ�ӡ�ӿ�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  s8 * fmt
           ...
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/11/20  		    ���㻪                  ����
=============================================================================*/
void VcsLog( s8 * fmt, ... )
{
    s8  achBuf[255];
    va_list argptr;    
    if( g_bPrintVcsMsg )
    {
        s32 nPrefix = sprintf( achBuf, "[McuVcs]:" );
        va_start( argptr, fmt );    
        vsprintf( achBuf + nPrefix, fmt, argptr );   
        OspPrintf(TRUE, FALSE, achBuf); 
        va_end(argptr); 
    }
}


/*====================================================================
    ������      ��SatLog
    ����        �����ǻ�����Ϣ��ӡ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	09/08/28    4.6         �ű���        ����
====================================================================*/	
void  SatLog(s8 * fmt,...)
{
	s8 achBuf[255];
    va_list argptr;
	if( g_bPrintSatMsg )
	{
		s32 nPrefix = sprintf( achBuf, "[Sat]: " );
		va_start(argptr, fmt);    
		vsprintf( achBuf+nPrefix, fmt, argptr );   
		OspPrintf(TRUE, FALSE, achBuf); 
		va_end(argptr); 
	}
}


u8 GetResByWH( u16 wWidth, u16 wHeight )
{
    if ( wWidth == 128 && wHeight == 96 )
    {
        return VIDEO_FORMAT_SQCIF;
    }
    if ( wWidth == 176 && wHeight == 144 )
    {
        return VIDEO_FORMAT_QCIF;
    }
    if ( wWidth == 352 && wHeight == 288 )
    {
        return VIDEO_FORMAT_CIF;
    }
    if ( wWidth == 352 && wHeight == 576 )
    {
        return VIDEO_FORMAT_2CIF;
    }
    if ( (wWidth == 704 && wHeight == 576) ||
         (wWidth == 720 && wHeight == 576) ||
         (wWidth == 720 && wHeight == 480))
    {
        return VIDEO_FORMAT_4CIF;
    }
    if ( wWidth == 1408 && wHeight == 1152 )
    {
        return VIDEO_FORMAT_16CIF;
    }
    if ( wWidth == 352 && wHeight == 240 )
    {
        return VIDEO_FORMAT_SIF;
    }
    if ( wWidth == 352 && wHeight == 480 )
    {
        return VIDEO_FORMAT_2SIF;
    }
    if ( wWidth == 704 && wHeight == 480 )
    {
        return VIDEO_FORMAT_4SIF;
    }
    if ( wWidth == 640 && wHeight == 480 )
    {
        return VIDEO_FORMAT_VGA;
    }
    if ((wWidth == 800 && wHeight == 600) ||
        (wWidth == 800 && wHeight == 608))
    {
        return VIDEO_FORMAT_SVGA;
    }
    if ( wWidth == 1024 && wHeight == 768 )
    {
        return VIDEO_FORMAT_XGA;
    }
    if ( wWidth == 112 && wHeight == 96 )
    {
        return VIDEO_FORMAT_SQCIF_112x96;
    }
    if ( wWidth == 96 && wHeight == 80 )
    {
        return VIDEO_FORMAT_SQCIF_96x80;
    }
    if ( wWidth == 1024 && wHeight == 576 )
    {
        return VIDEO_FORMAT_W4CIF;
    }
    if ( wWidth == 1280 && wHeight == 720 )
    {
        return VIDEO_FORMAT_HD720;
    }
    if ( wWidth == 1280 && wHeight == 1024 )
    {
        return VIDEO_FORMAT_SXGA;
    }
    if ( wWidth == 1600 && wHeight == 1200 )
    {
        return VIDEO_FORMAT_UXGA;
    }
    if ( ( wWidth == 1920 && wHeight == 1088 ) ||
         ( wWidth == 1920 && wHeight == 1080 ) ||
         ( wWidth == 1920 && wHeight == 544 ) )
    {
        return VIDEO_FORMAT_HD1080;
    }
    return 0;
}

void GetWHByRes(u8 byRes, u16 &wWidth, u16 &wHeight)
{
    wWidth = 0;
    wHeight = 0;
    
    switch(byRes)
    {
    case VIDEO_FORMAT_SQCIF_112x96:
        wWidth = 112;
        wHeight = 96;
        break;
        
    case VIDEO_FORMAT_SQCIF_96x80:
        wWidth = 96;
        wHeight = 80;
        break;
        
    case VIDEO_FORMAT_SQCIF:
        wWidth = 128;
        wHeight = 96;
        break;
        
    case VIDEO_FORMAT_QCIF:
        wWidth = 176;
        wHeight = 144;
        break;
        
    case VIDEO_FORMAT_CIF:
        wWidth = 352;
        wHeight = 288;
        break;
        
    case VIDEO_FORMAT_2CIF:
        wWidth = 352;
        wHeight = 576;
        break;
        
    case VIDEO_FORMAT_4CIF:
        
        //���ھ����ܲ��ſ��ǣ�����ȡ��ֵ
        wWidth = 704;
        wHeight = 576;
        break;
        
    case VIDEO_FORMAT_16CIF:
        wWidth = 1048;
        wHeight = 1152;
        break;
        
    case VIDEO_FORMAT_AUTO:
		// ����auto�ֱ��ʽ���Ϊcif
        wWidth = 352;
        wHeight = 288;      
        break;
        
    case VIDEO_FORMAT_SIF:
        wWidth = 352;
        wHeight = 240;
        break;
        
    case VIDEO_FORMAT_2SIF:
        wWidth = 352;
        wHeight = 480;
        break;
        
    case VIDEO_FORMAT_4SIF:
        wWidth = 704;
        wHeight = 480;
        break;
        
    case VIDEO_FORMAT_VGA:
        wWidth = 640;
        wHeight = 480;
        break;
        
    case VIDEO_FORMAT_SVGA:
        wWidth = 800;
        wHeight = 608;   //600-->608
        break;
        
    case VIDEO_FORMAT_XGA:
        wWidth = 1024;
        wHeight = 768;
        break;

    case VIDEO_FORMAT_W4CIF:
        wWidth = 1024;
        wHeight = 576;
        break;

    case VIDEO_FORMAT_HD720:
        wWidth = 1280;
        wHeight = 720;
        break;

    case VIDEO_FORMAT_SXGA:
        wWidth = 1280;
        wHeight = 1024;
        break;

    case VIDEO_FORMAT_UXGA:
        wWidth = 1600;
        wHeight = 1200;
        break;

    case VIDEO_FORMAT_HD1080:
        wWidth = 1920;
        wHeight = 1088;
        break;
		//�Ǳ�ֱ��ʣ�1080p��ͼ��
	case VIDEO_FORMAT_1440x816:
		wWidth = 1440;
		wHeight = 816;
        break;

	case VIDEO_FORMAT_1280x720:
		wWidth = 1280;
		wHeight = 720;
		break;
	
	case VIDEO_FORMAT_960x544:
		wWidth = 960;
		wHeight = 544;
		break;

	case VIDEO_FORMAT_640x368:
		wWidth = 640;
		wHeight =368;
		break;

	case VIDEO_FORMAT_480x272:
		wWidth = 480;
		wHeight =272;
		break;
	//�Ǳ�ֱ��ʣ�720p��ͼ��
	case VIDEO_FORMAT_720_960x544:
		wWidth = 960;
		wHeight =544;
		break;

	case VIDEO_FORMAT_720_864x480:
		wWidth = 864;
		wHeight =480;
		break;

	case VIDEO_FORMAT_720_640x368:
		wWidth = 640;
		wHeight =368;
		break;

	case VIDEO_FORMAT_720_432x240:
		wWidth = 432;
		wHeight =240;
		break;

	case VIDEO_FORMAT_720_320x192:
		wWidth = 320;
		wHeight =192;
		break;
    default:
        break;
    }
    if ( 0 == wHeight || 0 == wWidth )
    {
        OspPrintf( TRUE, FALSE, "[GetWHByRes] unexpected res.%d, ignore it\n", byRes );
    }
    return;
}
/*==============================================================================
������    :  IsResCmpNoMeaning
����      :  
�㷨ʵ��  :  vga��ķֱ��ʺͷ�vga��ķֱ��ʱȽϱ���Ϊ������
����˵��  :  u8 bySrcRes	[in]
			 u8 byDstRes	[in]
����ֵ˵��:  BOOL32
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-2-20					Ѧ��							create
==============================================================================*/
BOOL32 IsResCmpNoMeaning (u8 bySrcRes, u8 byDstRes)
{
	if ( ( (bySrcRes == VIDEO_FORMAT_VGA  || bySrcRes == VIDEO_FORMAT_SVGA 
		|| bySrcRes == VIDEO_FORMAT_XGA || bySrcRes == VIDEO_FORMAT_SXGA 
		|| bySrcRes == VIDEO_FORMAT_UXGA)
		&& (byDstRes != VIDEO_FORMAT_VGA  || byDstRes != VIDEO_FORMAT_SVGA 
		|| byDstRes != VIDEO_FORMAT_XGA || byDstRes != VIDEO_FORMAT_SXGA 
		|| byDstRes != VIDEO_FORMAT_UXGA) ) //src ΪVGA�࣬dstΪ��vga��
		
		|| ( (bySrcRes != VIDEO_FORMAT_VGA  || bySrcRes != VIDEO_FORMAT_SVGA 
		|| bySrcRes != VIDEO_FORMAT_XGA || bySrcRes != VIDEO_FORMAT_SXGA 
		|| bySrcRes != VIDEO_FORMAT_UXGA)
		&& (byDstRes == VIDEO_FORMAT_VGA  || byDstRes == VIDEO_FORMAT_SVGA 
		|| byDstRes == VIDEO_FORMAT_XGA || byDstRes == VIDEO_FORMAT_SXGA 
		|| byDstRes == VIDEO_FORMAT_UXGA) ) //src Ϊ��VGA�࣬dstΪvga��
		)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*==============================================================================
������    :  ResWHCmp
����      :  
�㷨ʵ��  :  
����˵��  :  u8 bySrcRes	[in]
			 u8 byDstRes    [in]
����ֵ˵��:  s32 
             ��߾�С����-2����С�ߴ󷵻�-1����߾���ȷ���0������С����1����߾��󷵻�2��
			 ���߱Ƚ������巵��3
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-2-20                  Ѧ��                            ����
==============================================================================*/
s32 ResWHCmp(u8 bySrcRes, u8 byDstRes)
{
	//VGA��ķֱ��ʺͷ�VGA��ķֱ��ʱȽ�û������
	if(IsResCmpNoMeaning(bySrcRes,byDstRes))
	{
		return 3;
	}

	u16 wSrcWidth = 0;
	u16 wSrcHeight = 0;
	u16 wDstWidth = 0;
	u16 wDstHeight = 0;
	GetWHByRes(bySrcRes, wSrcWidth, wSrcHeight);
	GetWHByRes(byDstRes, wDstWidth, wDstHeight);

	//�������
	//VIDEO_FORMAT_480x272 ����>�Ƚ�ʱͳһ���䵽480 x 288
	wSrcHeight = (wSrcHeight == 272) ? 288 : wSrcHeight;
	wDstHeight = (wSrcHeight == 272) ? 288 : wDstHeight;
	
	//ǰ�߾������ں���
	if ( (wSrcWidth == wDstWidth) && (wSrcHeight == wDstHeight) )
	{
		return 0;  //��߾���  (equal in width and height)
	}
	else if( ( wSrcWidth <= wDstWidth ) && (wSrcHeight <= wDstHeight) )
	{
		return -2; //��߾�С����ȸ�С����С�ߵ� (none bigger in both width and height)
	}
	//ǰ��������һ����ں���
	else if ( (wSrcWidth <= wDstWidth) && (wSrcHeight >= wDstHeight) )
	{
		return -1; //��С�ߴ󣬿�ȸߴ�  (only bigger in height)
	}
	else if ( (wSrcWidth >= wDstWidth) && (wSrcHeight <= wDstHeight) )
	{
		return 1;  //����С�����ߵ�  (only bigger in width)
	}
	else 
	{
		return 2; //��߾���			 (bigger both in width and height)
	}
}

/*==============================================================================
������    :  GetVmpOutChnnlByRes
����      :  ��ȡVMP��Ӧĳ�ֱ��ʵ����ͨ��
�㷨ʵ��  :  
����˵��  :  u8 byRes			[in]
			 u8 byVmpId			[in]
			 u8 byMediaType		[in] ý���ʽ��Ĭ��ֵ��MEDIA_TYPE_H264
			 
����ֵ˵��:  u8 VMP���ͨ����
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-5-07                  Ѧ��                            ����
==============================================================================*/
u8 GetVmpOutChnnlByRes(u8 byRes, u8 byVmpId, u8 byMediaType)
{
	u8 byChnnlIdx = ~0;

	TPeriEqpStatus tPeriEqpStatus;	
	g_cMcuVcApp.GetPeriEqpStatus( byVmpId , &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;	
	u8 byBoardVer	= tPeriEqpStatus.m_tStatus.tVmp.m_byBoardVer;

    //zbq[07/27/2007] ������A/B�壬ǿ��1080Iʱ��720p��ȡproximal
    if (g_cMcuVcApp.IsSVmpOutput1080i())
    {
        if (VIDEO_FORMAT_HD720 == byRes)
        {
            byRes = VIDEO_FORMAT_4CIF;
        }
    }

	if (VMP == byVmpSubType)		//����VMPֻ��һ·���
	{
		byChnnlIdx = 0;				
	}
	else if (MPU_SVMP == byVmpSubType)
	{
		if (MPU_BOARD_A128 == byBoardVer)
		{
			switch (byRes)
			{
			case VIDEO_FORMAT_HD1080:
				byChnnlIdx = 0;
				break;
				
			case VIDEO_FORMAT_HD720:
				byChnnlIdx = 0;
				break;
				
			case VIDEO_FORMAT_4CIF:
				byChnnlIdx = 2;
				break;
				
			case VIDEO_FORMAT_CIF:
				byChnnlIdx = 2;
				break;
			default:
				break;
			}
		}
		else if(MPU_BOARD_B256 == byBoardVer)
		{
			if( byMediaType == MEDIA_TYPE_H264)
			{
				switch (byRes)
				{
				case VIDEO_FORMAT_HD1080:
					byChnnlIdx = 0;
					break;
					
				case VIDEO_FORMAT_HD720:
					byChnnlIdx = 1;
					break;
					
				case VIDEO_FORMAT_4CIF:
					byChnnlIdx = 3;
					break;
					
				case VIDEO_FORMAT_CIF:
					byChnnlIdx = 2;
					break;
					
				default:
					break; 
				}
			}
			else
			{
				byChnnlIdx = 2;
			}
		}
		else
		{
			//Do nothing
			OspPrintf(TRUE, FALSE, "[GetVmpOutChnnlByRes] Unexpected mpu board version!\n");
		}
	}
	else if (EVPU_SVMP == byVmpSubType)
	{
		//������չ
	}
	

	return byChnnlIdx;
}

/*==============================================================================
������    :  GetResByVmpOutChnnl
����      :  ��ȡVMP��Ӧĳ�ֱ��ʵ����ͨ��
�㷨ʵ��  :  
����˵��  :  u8			byChnnl		[in]
			 TConfInfo	tConfInfo	[in]
			 u8			bySubVmpType[in]
			 u8			byMediaType	[out]
			 
����ֵ˵��:  u8			byRes
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-5-07                  Ѧ��                            ����
==============================================================================*/
u8 GetResByVmpOutChnnl(u8 &byMediaType, u8 byChnnl, const TConfInfo &tConfInfo, u8 byVmpId)
{
	u8 byRes = 0;

	TPeriEqpStatus tPeriEqpStatus;	
	g_cMcuVcApp.GetPeriEqpStatus( byVmpId , &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;	
	u8 byBoardVer	= tPeriEqpStatus.m_tStatus.tVmp.m_byBoardVer;

	//old vmp
	if( byVmpSubType == VMP)
	{
		byMediaType = tConfInfo.GetMainVideoMediaType();	//ȡ���������ʽ
		return byRes;										//��������£���ֵ���ز��߱��κβο���ֵ
	}

	//MPU
	if (MPU_BOARD_A128 == byBoardVer)
	{
		//MPU 2���汾(A��)  0    1   2
		/*----------------------------
		1080				1080	cif	
		1080/720			720		cif
		1080/4cif			1080	cif
		1080/cif			1080	cif
		720					720		cif
		720/4cif			720		cif
		720/cif				720		cif
		4cif						cif
		4cif/cif					cif
		cif							cif
		xxx/other			*       other
		other				1080	other (HDU ��0·�ģ�Ҳ�ܿ����Ϻõ�Ч��)
		------------------------------*/
		if (MEDIA_TYPE_H264 == tConfInfo.GetMainVideoMediaType())
		{
			byMediaType = MEDIA_TYPE_H264;
			if( 2 == byChnnl )
			{
				if(tConfInfo.GetSecVideoMediaType() == MEDIA_TYPE_H263 || tConfInfo.GetSecVideoMediaType() == MEDIA_TYPE_MP4 )
				{
					byMediaType = tConfInfo.GetSecVideoMediaType();//��other
					byRes =  VIDEO_FORMAT_CIF;
				}
				else
				{
					byRes =  VIDEO_FORMAT_CIF;
				}
			}
			else if( 0 == byChnnl )
			{
				if( VIDEO_FORMAT_HD720 == tConfInfo.GetMainVideoFormat()
					|| ( VIDEO_FORMAT_HD1080 == tConfInfo.GetMainVideoFormat()//1080,720�Ļ����1����720
					&& tConfInfo.GetConfAttrbEx().IsResEx720() )
					)
				{
					byRes = VIDEO_FORMAT_HD720;
				}
				else if( VIDEO_FORMAT_HD1080 == tConfInfo.GetMainVideoFormat() ||
					VIDEO_FORMAT_CIF == tConfInfo.GetMainVideoFormat() )
				{
					byRes = VIDEO_FORMAT_HD1080;
				}
				else
				{
					//do nothing
				}
			}
			else
			{
				//do nothing
			}
		}
		else //h263�����mpeg4����
		{
			byMediaType = ( 0 == byChnnl )? MEDIA_TYPE_H264: tConfInfo.GetMainVideoMediaType();
			byRes = ( 0 == byChnnl )? VIDEO_FORMAT_HD1080 : VIDEO_FORMAT_CIF;  
		}	
	}
	else
	{
		//MPU 4���汾 ��B�壩
		if( 0 == byChnnl )	//0��
		{
			byMediaType = MEDIA_TYPE_H264;
			byRes = VIDEO_FORMAT_HD1080;
		}
		else if (1 == byChnnl)	//1��
		{
			byMediaType = MEDIA_TYPE_H264;
			byRes = VIDEO_FORMAT_HD720;
		}
		else if (2 == byChnnl)	//2��
		{
			if( tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_H263 || tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_MP4 )
			{
				byMediaType = tConfInfo.GetMainVideoMediaType(); 
				byRes =  VIDEO_FORMAT_CIF;
			}
			else
			{
				if(tConfInfo.GetSecVideoMediaType() == MEDIA_TYPE_H263 || tConfInfo.GetSecVideoMediaType() == MEDIA_TYPE_MP4)
				{
					byMediaType = tConfInfo.GetSecVideoMediaType(); //��other
					byRes =  VIDEO_FORMAT_CIF;
				}
				else
				{
					byMediaType = MEDIA_TYPE_H264;
					byRes =  VIDEO_FORMAT_CIF;
				}
			}
		}
		else					//3��
		{
			byMediaType = MEDIA_TYPE_H264;
			byRes = VIDEO_FORMAT_4CIF;
		}
	}
	/*
	if(DEVVER_MPU == 4501)
	{

		if (MEDIA_TYPE_H264 == tConfInfo.GetMainVideoMediaType())
		{
			byMediaType = MEDIA_TYPE_H264;
			if( 2 == byChnnl )
			{
				if(tConfInfo.GetSecVideoMediaType() == MEDIA_TYPE_H263 || tConfInfo.GetSecVideoMediaType() == MEDIA_TYPE_MP4 )
				{
					byMediaType = tConfInfo.GetSecVideoMediaType();//��other
					byRes =  VIDEO_FORMAT_CIF;
				}
				else
				{
					byRes =  VIDEO_FORMAT_CIF;
				}
			}
			else if( 0 == byChnnl )
			{
				if( VIDEO_FORMAT_HD720 == tConfInfo.GetMainVideoFormat()
					|| ( VIDEO_FORMAT_HD1080 == tConfInfo.GetMainVideoFormat()//1080,720�Ļ����1����720
					&& tConfInfo.GetConfAttrbEx().IsResEx720() )
					)
				{
					byRes = VIDEO_FORMAT_HD720;
				}
				else if( VIDEO_FORMAT_HD1080 == tConfInfo.GetMainVideoFormat() ||
					VIDEO_FORMAT_CIF == tConfInfo.GetMainVideoFormat() )
				{
					byRes = VIDEO_FORMAT_HD1080;
				}
				else
				{
					//do nothing
				}
			}
			else
			{
				//do nothing
			}
		}
		else //h263�����mpeg4����
		{
			byMediaType = ( 0 == byChnnl )? MEDIA_TYPE_H264: tConfInfo.GetMainVideoMediaType();
			byRes = ( 0 == byChnnl )? VIDEO_FORMAT_HD1080 : VIDEO_FORMAT_CIF;  
		}
	}
	else if(DEVVER_MPU > 4501)
	{
		//MPU 4���汾 ��B�壩
		if( 0 == byChnnl )	//0��
		{
			byMediaType = MEDIA_TYPE_H264;
			byRes = VIDEO_FORMAT_HD1080;
		}
		else if (1 == byChnnl)	//1��
		{
			byMediaType = MEDIA_TYPE_H264;
			byRes = VIDEO_FORMAT_HD720;
		}
		else if (2 == byChnnl)	//2��
		{
			if( tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_H263 || tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_MP4 )
			{
				byMediaType = tConfInfo.GetMainVideoMediaType(); 
				byRes =  VIDEO_FORMAT_CIF;
			}
			else
			{
				if(tConfInfo.GetSecVideoMediaType() == MEDIA_TYPE_H263 || tConfInfo.GetSecVideoMediaType() == MEDIA_TYPE_MP4)
				{
					byMediaType = tConfInfo.GetSecVideoMediaType(); //��other
					byRes =  VIDEO_FORMAT_CIF;
				}
				else
				{
					byMediaType = MEDIA_TYPE_H264;
					byRes =  VIDEO_FORMAT_CIF;
				}
			}
		}
		else					//3��
		{
			byMediaType = MEDIA_TYPE_H264;
			byRes = VIDEO_FORMAT_4CIF;
		}
	}
	*/
	return byRes;
}



/*====================================================================
    ������      ��topoGetAllTopoSubMcu
    ����        ����ȡ�������˽ṹ�и�MCU�������¼�MCU(��ѡһ������в�)
    �㷨ʵ��    ���ݹ�
    ����ȫ�ֱ�������
    �������˵����WORD wMcuId, �û������McuId
				  u16 wMcuTopo[], ���飬���ڴ�Ų��ҵĽ��
				  u8 bySize, ����Ԫ�ظ���
				  TMcuTopo atMcuTopo[], MCU��������
				  u16 wMcuTopoNum, ȫ����MCU��Ŀ
				  u8 byMode, ���ʲ�����0��ʾ���в㣬1��ʾһ�㣬ȱʡΪ0
    ����ֵ˵��  ��ʵ����Ŀ
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/08    1.0         ����ƽ        ����
    02/09/19    1.0         LI Yi         ��CMcuAgent���ƹ���
    02/10/24    1.0         LI Yi         ��Ӳ���byMode
====================================================================*/
 u16 topoGetAllTopoSubMcu( u16    wMcuId,
                                 u16    wSubMcu[],
                                 u8  bySize, 
							     TMcuTopo atMcuTopo[],
                                 u16    wMcuTopoNum,
                                 u8  byMode )
{
	u16	wLoop;
	u16	wNum = 0;
	u16    wMMcuNum;
	u8 byLayer = 0;

	ASSERT( atMcuTopo != NULL );

	for( wLoop = 0 ; wLoop < wMcuTopoNum && wNum < bySize; wLoop++ )
	{
		if(byMode != 0)
		{	
		    if( wMcuId == atMcuTopo[wLoop].GetSMcuId() && wMcuId != atMcuTopo[wLoop].GetMcuId() )
			{
			    wSubMcu[wNum] = atMcuTopo[wLoop].GetMcuId();
			    wNum++;
			}
		}
		else
		{
            wMMcuNum = wLoop;
			byLayer = 0;
			while(wMMcuNum != atMcuTopo[wMMcuNum].GetSMcuId() && byLayer<3)
			{
                 wMMcuNum = atMcuTopo[wMMcuNum].GetSMcuId();
				 if(wMMcuNum == wMcuId && wMcuId != atMcuTopo[wLoop].GetMcuId())
				 {
                     wSubMcu[wNum] = atMcuTopo[wLoop].GetMcuId();
			         wNum++;
					 break;
				 }
				 byLayer++;
			}
		}
	}
	
    return( wNum );
}

/*====================================================================
    ������      ��topoGetMcuTopo
    ����        ����ȡָ��MCU����TOP MCU�������·��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId, �û������McuId
				  u16 wMcuTopo[], ���飬���ڴ�Ų��ҵĽ������ָ��MCU
						          ��TOP MCU����"3,2,1"�� 0��β
				  u8 bySize, ����Ԫ�ظ�����Ҫ��С��5
				  TMcuTopo atMcuTopo[], MCU��������
				  u16 wMcuTopoNum, ȫ����MCU��Ŀ
    ����ֵ˵��  ���ɹ�����TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/02    1.0         ����ƽ        ����
    02/09/19    1.0         LI Yi         ��CMcuAgent���ƹ���
====================================================================*/
  BOOL topoGetMcuTopo( u16     wMcuId,
                            u16     wMcuTopo[],
                            u8   bySize, 
					        TMcuTopo atMcuTopo[],
                            u16     wMcuTopoNum )
{
	u16 wBufLoop;
	u16 wEntLoop;

	ASSERT( atMcuTopo != NULL );

	memset( wMcuTopo, 0, bySize * sizeof( u16 ) );

    //Set the first element of buffer to wMcuId
	wMcuTopo[0] = wMcuId;

	for( wBufLoop = 0; wBufLoop < bySize - 1; wBufLoop++ )
	{
		for( wEntLoop = 0; wEntLoop < wMcuTopoNum; wEntLoop++ )
		{
			if( wMcuTopo[wBufLoop] == atMcuTopo[wEntLoop].GetMcuId() )
			{
				if( wMcuTopo[wBufLoop] != atMcuTopo[wEntLoop].GetSMcuId() )
				{
                    //find the MCU but it's not topMcu
					wMcuTopo[wBufLoop + 1] = atMcuTopo[wEntLoop].GetSMcuId();
					break;
				}
				else
				{
                    //find the topMCU
					return( TRUE );
				}
			}
		}
		if( wMcuTopo[wBufLoop + 1] == 0 )	//cannot find the MCU
		{
			break;
		}
	}	

	//Cannot find the McuId
	printf( "Cannot find Mcu%d!\n", wMcuId );

    return( FALSE );
}

/*====================================================================
    ������      ��topoGetInterMcu
    ����        �������ԴMCU������Ŀ��MCU����һվֱ��MCU��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wSrcMcu, ԴMCU��
				  u16 wDstMcu, Ŀ��MCU��
				  TMcuTopo atMcuTopo[], MCU��������
				  u16 wMcuTopoNum, ȫ����MCU��Ŀ
    ����ֵ˵��  ������ֱ��MCU�ţ�0xffff��ʾ��һվΪԴMCU���ϼ�MCU��
				  ʧ�ܷ���NULL
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/30    1.0         LI Yi         ����
====================================================================*/
  u16 topoGetInterMcu( u16 wSrcMcu, u16 wDstMcu, TMcuTopo atMcuTopo[], u16 wMcuTopoNum )
{
	u16	awBuf[5];
	u8	byLoop;

	ASSERT( atMcuTopo != NULL );

	if( wDstMcu == wSrcMcu )	//self
		return( wDstMcu );

	if( !topoGetMcuTopo( wDstMcu, awBuf, 5, atMcuTopo, wMcuTopoNum ) )	//fail to find it
	{
		printf( "Cannot find the path to MCU%u!\n", wDstMcu );
		return( NULL );
	}

	for( byLoop = 1; byLoop < 5; byLoop++ )
	{
		if( awBuf[byLoop] == wSrcMcu )
			return( awBuf[byLoop - 1] );
	}

	return( ( u16 )-1 );	//������MCU������0xFFFF
}

/*====================================================================
    ������      ��topoFindMcuPath
    ����        �������ԴMCU������Ŀ��MCU��·
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wSrcMcu, ԴMCU��
				  u16 wDstMcu, Ŀ��MCU��
				  u16 awMcuPath[], ���ص�·��BUFFER����Դ��Ŀ��MCU��
						��"1,2,3"��0��β
				  u8 byBufSize, BUFFER��С
				  TMcuTopo atMcuTopo[], MCU��������
				  u16 wMcuTopoNum, ȫ����MCU��Ŀ
    ����ֵ˵��  ���ɹ�����TRUE��ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/09/19    1.0         LI Yi         ����
====================================================================*/
  BOOL topoFindMcuPath( u16 wSrcMcu, u16 wDstMcu, u16 awMcuPath[], u8 byBufSize, 
					         TMcuTopo atMcuTopo[], u16 wMcuTopoNum )
{
	u16	wNextMcu = wSrcMcu;
	u8	byLoop, byLoop2;

	ASSERT( atMcuTopo != NULL );

	if( byBufSize < 2 )
		return( FALSE );

	byLoop = 0;
	while( wNextMcu != NULL && byLoop < byBufSize - 1 )
	{
		awMcuPath[byLoop++] = wNextMcu;
		
		if( wNextMcu == wDstMcu )	//finished
		{
			awMcuPath[byLoop] = 0;
			return( TRUE );
		}

		wSrcMcu = wNextMcu;
		wNextMcu = topoGetInterMcu( wSrcMcu, wDstMcu, atMcuTopo, wMcuTopoNum );
		if( wNextMcu == 0xffff )	//to superior MCU
		{
			//find its superior MCU ID
			for( byLoop2 = 0; byLoop2 < wMcuTopoNum; byLoop2++ )
			{
				if( atMcuTopo[byLoop2].GetMcuId() == wSrcMcu )
				{
					wNextMcu = atMcuTopo[byLoop2].GetSMcuId();
					break;
				}
			}
			if( byLoop2 == wMcuTopoNum )	//wrong!
				return( FALSE );
		}
	}

	return( FALSE );
}

/*====================================================================
    ������      ��topoIsCollide
    ����        ���ж��û����ڿ��������նˣ������裩�Ƿ����MCU����
					�����ĸı�����·��ͻ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wCurSrcMcuId, �û���ѡ���������ն˻���������MCU��
				  u16 wConnMcuId, �û����ӵ�MCU��
				  u16 wSrcMcuId, ��·�ı��Դ�ն˻���������MCU��
				  u16 wDstMcuId, ��·�ı��Ŀ���ն˻���������MCU��
				  TMcuTopo atMcuTopo[], MCU��������
				  u16 wMcuTopoNum, ȫ����MCU��Ŀ
    ����ֵ˵��  ���г�ͻ����TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/09/19    1.0         LI Yi         ����
====================================================================*/
  BOOL topoIsCollide( u16 wCurSrcMcuId,
                           u16 wConnMcuId,
                           u16 wSrcMcuId, 
					       u16 wDstMcuId,
                           TMcuTopo atMcuTopo[],
                           u16      wMcuTopoNum )
{
	u16	awCurPath[16];		//�û���ǰ·��
	u16	awChangePath[16];	//��·�仯��·��
	u8	byLoop1, byLoop2;

	ASSERT( atMcuTopo != NULL );

	//get two paths
	if( !topoFindMcuPath( wCurSrcMcuId, wConnMcuId, awCurPath, 16, atMcuTopo, wMcuTopoNum ) )
	{
		printf( "wrong path from MCU%d to MCU%d!\n", wCurSrcMcuId, wConnMcuId );
		return( FALSE );
	}
	if( !topoFindMcuPath( wSrcMcuId, wDstMcuId, awChangePath, 16, atMcuTopo, wMcuTopoNum ) )
	{
		printf( "wrong path from MCU%d to MCU%d!\n", wSrcMcuId, wDstMcuId);
		return( FALSE );
	}

	//judge two paths if they collide with each other
	byLoop1 = 0;
	while( awCurPath[byLoop1] != 0 )
	{
		byLoop2 = 0;
		while( awChangePath[byLoop2] != 0 )
		{
			if( awCurPath[byLoop1]   == awChangePath[byLoop2] &&
                awCurPath[byLoop1+1] == awChangePath[byLoop2+1] &&
                awCurPath[byLoop1+1] != 0 )
            {
				return TRUE;
            }

			byLoop2++;
		}

		byLoop1++;
	}

	return( FALSE );
}

/*====================================================================
    ������      ��topoGetMcuInfo
    ����        ���õ���Ҫ��Mcu��TMcuTopo�ṹ��
				  �û����Լ���֤�����MCU����������ʵ�ʴ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId,Ҫ���MCU��
				  TMcuTopo atMcuTopo[]��MCU�������� 
				  u16 wMcuTopoNum��ȫ���е�MCU��Ŀ
    ����ֵ˵��  ���ɹ�����TMcuTopo�����򷵻�һ���ڲ�ȫΪ0��TMcuTopo�ṹ
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/25    1.0         Liaoweijiang  ����
====================================================================*/
  TMcuTopo topoGetMcuInfo( u16 wMcuId,  
					            TMcuTopo atMcuTopo[], u16 wMcuTopoNum )
{
	TMcuTopo	tMcuTopo;

    ASSERT( atMcuTopo != NULL );

	for( u16 wLoop=0; wLoop<wMcuTopoNum; wLoop++ )
	{
		if( atMcuTopo[wLoop].GetMcuId() == wMcuId )
		{
			return atMcuTopo[wLoop];
		}
	}
	
	memset( &tMcuTopo, 0, sizeof(TMcuTopo) );
	return	tMcuTopo;
}

/*====================================================================
    ������      ��topoGetMtInfo
    ����        ���õ���Ҫ��Mt��TMtTopo�ṹ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId��MT���ڵ�MCU��
				  u8 byMtId��MT��MCU�еı��
				  TMcuTopo atMtTopo[]��Mt����������
				  u16 wMtTopoNum��ȫ����Mt����Ŀ
    ����ֵ˵��  ���ɹ�����TMtTopo�����򷵻�һ��Ϊȫ0��TMtTopo�ṹ
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/25    1.0         Liaoweijiang  ����
====================================================================*/
  TMtTopo topoGetMtInfo( u16 wMcuId, u8 byMtId, 
					          TMtTopo atMtTopo[], u16 wMtTopoNum )
{
	TMtTopo	tMtTopo;

    ASSERT( atMtTopo != NULL );

	for(u16 wLoop=0; wLoop<wMtTopoNum; wLoop++)
	{
		if( atMtTopo[wLoop].GetMcuId() == wMcuId && atMtTopo[wLoop].m_byMtId == byMtId )
		{
			return atMtTopo[wLoop];
		}
	}

	memset( &tMtTopo, 0, sizeof(TMtTopo) );
	return	tMtTopo;
}

/*====================================================================
    ������      ��topoIsSecondaryMt
    ����        ���ж�һ��Mt�Ƿ��Ǵμ��ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId��MT���ڵ�MCU��
				  u8 byMtId����������MCU�е�MT��
				  TMtTopo atMtTopo[]��MT����������
				  u16 wMtTopoNum��ȫ��MT����Ŀ
    ����ֵ˵��  ������Ƿ���TRUE, ����FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/25    1.0         Liaoweijiang  ����
====================================================================*/
  BOOL topoIsSecondaryMt( u16 wMcuId, u8 byMtId, 
					           TMtTopo atMtTopo[], u16 wMtTopoNum )
{
    ASSERT( atMtTopo != NULL );

	for(u16 wLoop=0; wLoop<wMtTopoNum; wLoop++)
	{
		if( atMtTopo[wLoop].GetMcuId() == wMcuId && atMtTopo[wLoop].m_byMtId == byMtId )
		{
			if(atMtTopo[wLoop].m_byPrimaryMtId !=0 )
				return TRUE;
			else
				return FALSE;
		}
	}
    
	return	FALSE;
}

/*====================================================================
    ������      ��topoHasSecondaryMt
    ����        ���ж�һ��Mt�Ƿ��дμ��ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId��MT���ڵ�MCU��
				  u8 byMtId��MT�ڴ�MCU�еĺ�
				  TMtTopo atMtTopo[]��ȫ����MT��������
				  u16 wMtTopoNum��ȫ���е�MT��Ŀ
    ����ֵ˵��  ���ɹ�����TRUE, ����FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/25    1.0         Liaoweijiang  ����
====================================================================*/
  BOOL topoHasSecondaryMt( u16 wMcuId, u8 byMtId, 
					            TMtTopo atMtTopo[], u16 wMtTopoNum )
{
    ASSERT( atMtTopo != NULL );

	for(u16 wLoop=0; wLoop<wMtTopoNum; wLoop++)
	{
		if( atMtTopo[wLoop].GetMcuId() == wMcuId && atMtTopo[wLoop].m_byPrimaryMtId == byMtId )
		{
				return TRUE;
		}
	}
    
	return	FALSE;
}
/*====================================================================
    ������      ��topoGetPrimaryMt
    ����        ���õ�һ���ն˵����ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId��MT���ڵ�MCU��
				  u8 byMtId��MT�ڴ�MCU�еĺ�
				  TMtTopo atMtTopo[]��ȫ����MT��������
				  u16 wMtTopoNum��ȫ���е�MT��Ŀ
    ����ֵ˵��  ���ɹ������������ն˺�,����0˵�����������նˣ���Ƿ��ն�
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/25    1.0         Liaoweijiang  ����
====================================================================*/
  u8 topoGetPrimaryMt( u16 wMcuId, u8 byMtId, 
					           TMtTopo atMtTopo[],  u16  wMtTopoNum )
{
    ASSERT( atMtTopo != NULL );

	for(u16 wLoop=0; wLoop < wMtTopoNum; wLoop++)
	{
		if( atMtTopo[wLoop].GetMcuId() == wMcuId && atMtTopo[wLoop].m_byMtId == byMtId )
		{	
			//�ж����Ƿ��Ǵμ��ն�
			return atMtTopo[wLoop].m_byPrimaryMtId;
		}
	}
	return 0;
}

/*====================================================================
    ������      ��topoGetExcludedMt
    ����        ���õ�һ���ն˵����л����ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId��MT���ڵ�MCU��
				  u8 byMtId��MT�ڴ�MCU�еĺ�
				  TMtTopo atMtTopo[]��ȫ����MT��������
				  u16 wMtTopoNum��ȫ���е�MT��Ŀ
				  u8 MtBuffer[]��������׼��װ�����ն˺ŵ�����
				  u16 BufferSize������������Ĵ�С
    ����ֵ˵��  ���ɹ�����ʵ�ʵ��ն���Ŀ, ����NULL
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/25    1.0         Liaoweijiang  ����
====================================================================*/
  u16 topoGetExcludedMt( u16 wMcuId, u8 byMtId, 
					          TMtTopo atMtTopo[], u16 wMtTopoNum,
                              u8 MtBuffer[], u16 wBufferSize)
{
    u16	wLoop, wLoop2=0;
	u8	byTempMtId;
	
	ASSERT( atMtTopo != NULL );
	
	//��������ն�
	if( !topoIsSecondaryMt( wMcuId, byMtId, atMtTopo, wMtTopoNum ) )
	{
		for( wLoop = 0; wLoop < wMtTopoNum; wLoop++)
		{
			if( atMtTopo[wLoop].GetMcuId () == wMcuId && atMtTopo[wLoop].m_byPrimaryMtId == byMtId )
			{
				MtBuffer[wLoop2++] = atMtTopo[wLoop].m_byMtId;
			}
		}
		
		if(wLoop2 >= min( wBufferSize, wMtTopoNum ) )  
			return wLoop2;
	}
	
	//����Ǵμ��նˣ��򻥳��ն˰������������ն˺������Ĵμ��ն�
	else
	{
		//���ȵõ��μ��ն˵����ն�
		for( wLoop = 0; wLoop < wMtTopoNum; wLoop++)
		{
			if( atMtTopo[wLoop].GetMcuId () == wMcuId && atMtTopo[wLoop].m_byMtId == byMtId )
			{
				byTempMtId = atMtTopo[wLoop].m_byPrimaryMtId;
			}
		}
		
		//�õ��˴μ��ն˵Ļ����նˣ��������������ն˺������Ĵμ��ն�
		for( wLoop = 0; wLoop < wMtTopoNum; wLoop++)
		{
			if( ( atMtTopo[wLoop].GetMcuId () == wMcuId && atMtTopo[wLoop].m_byMtId == byTempMtId ) || //���������ն�
				( atMtTopo[wLoop].GetMcuId () == wMcuId && atMtTopo[wLoop].m_byMtId != byMtId
				&& atMtTopo[wLoop].m_byPrimaryMtId == byTempMtId ) ) //ͬһ���ն��µ������Ĵμ��ն�
			{
				MtBuffer[wLoop2++]=atMtTopo[wLoop].m_byMtId;
			}
		}
		
		if(wLoop2 >= min( wBufferSize, wMtTopoNum ) )  
			return wLoop2;
	}
    
	return wLoop2;
}

/*====================================================================
    ������      ��topoGetAllSubMt
    ����        ���õ�һ��Mcu�µ�����Mt
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId : MCU��
				  TMtTopo atMtTopo[] : Mt����������
				  u16 wMtTopoNum��ȫ��MT����Ŀ
				  TMt MtBuffer[], ׼�����õõ�MT��TMt�ṹ������
				  u16 wBufferSize, �����С
				  byMode ����ģʽ
					Ϊ0,��ʾֻ�õ�������MT
					Ϊ1,��ʾ���Եõ���һ��MCU��MT
					Ϊ2,��ʾ�õ����¶���MCU��MT
    ����ֵ˵��  ��ʵ�ʵõ���MT����Ŀ
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/25    1.0         Liaoweijiang  ����
====================================================================*/
  u16 topoGetAllSubMt( u16 wMcuId, TMtTopo atMtTopo[], u16 wMtTopoNum, 
							TMt MtBuffer[], u16 wBufferSize, u8 byMode )
{
	u16	wLoop;
	u16	wNum = 0;

	for( wLoop = 0 ; wLoop < wMtTopoNum; wLoop++ )
	{
		if( atMtTopo[wLoop].GetMcuId() == wMcuId )
		{
			//����MCU�ź�MT�ţ��������Ϊ0����ʾ���ն�����
			MtBuffer[wNum].SetMt( (u8)atMtTopo[wLoop].GetMcuId(), 
				 atMtTopo[wLoop].m_byMtId );

            //FIXME:
			//���ñ���
			//MtBuffer[wNum].SetAlias( atMtTopo[wLoop].GetAlias() );

			if( wNum >= wBufferSize )
				return wNum;

			wNum++;
		}
	}
    return( wNum );
}

/*====================================================================
    ������      ��topoGetTopMcuBetweenTwoMcu
    ����        ���õ�����MCU��������ߵ��MCU
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId1 : MCU��
				  u16 wMcuId2 : MCU��
				  TMtTopo atMtTopo[] : Mcu����������
				  u16 wMtTopoNum��ȫ��Mcu����Ŀ
    ����ֵ˵��  ������MCU��������ߵ��MCU��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/11/07    1.0         Liaoweijiang  ����
====================================================================*/
  u16 topoGetTopMcuBetweenTwoMcu ( u16 wMcuId1, u16 wMcuId2,
										TMcuTopo atMcuTopo[], u16 wMcuTopoNum )
{
	u16 wPath1[5], wPath2[5];

	memset( wPath1, 0, sizeof(wPath1) );
	memset( wPath2, 0, sizeof(wPath2) );
	if( !topoGetMcuTopo( wMcuId1, wPath1, 5, atMcuTopo, wMcuTopoNum ) )
		return 0;
	if( !topoGetMcuTopo( wMcuId2, wPath2, 5, atMcuTopo, wMcuTopoNum ) )
		return 0;

	for( u8 byLoop1=0; byLoop1<5 && wPath1[byLoop1] != 0; byLoop1++)
	{
		for( u8 byLoop2=0; byLoop2<5 && wPath2[byLoop2] != 0; byLoop2++)
		{
			if( wPath1[byLoop1] == wPath2[byLoop2] )
				return wPath1[byLoop1];
		}
	}
	return 0;
}

/*====================================================================
    ������      ��topoGetTopMcu
    ����        ���õ�����MCU��������ߵ��Mcu
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId[] : MCU������
				  u8 byMcuNum : �����С
				  TMtTopo atMtTopo[] : Mcu����������
				  u16 wMtTopoNum��ȫ��Mcu����Ŀ
    ����ֵ˵��  ��������ߵ��Mcu
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/11/07    1.0         Liaoweijiang  ����
====================================================================*/
  u16 topoGetTopMcu( u16 wMcuId[], u8 byMcuNum, 
						  TMcuTopo atMcuTopo[], u16 wMcuTopoNum )
{
	u8 byArrayNum = 0;
	u16  wTopMcuId = 0;
	
	ASSERT( atMcuTopo != NULL );

	byArrayNum = byMcuNum;
	wTopMcuId = wMcuId[ byArrayNum - 1 ];
	//���ô�������������Ԫ�ؿ�ʼ�Ƚϣ����ȽϽ�����뵹���ڶ���Ԫ��λ�ã�
	//��������¿�ʼ��ֱ������ֻʣ�����һ��Ԫ�أ�������
	while( byArrayNum > 1 )
	{
		wTopMcuId = topoGetTopMcuBetweenTwoMcu( wMcuId[byArrayNum - 2],
								   wTopMcuId, atMcuTopo, wMcuTopoNum );
		byArrayNum--;
	}

	return wTopMcuId;
}


/*====================================================================
    ������      ��topoJudgeLegality
    ����        �����˽ṹ�Ϸ����ж�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMcuTopo atMcuTopo[], Mcu����������
				  u16 wMcuTopoNum, ȫ��Mcu����Ŀ
				  TMtTopo atMtTopo[] : Mt����������
				  u16 wMtTopoNum��ȫ��Mt����Ŀ
    ����ֵ˵��  ��������ߵ��Mcu
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/11/13    1.0         Liaoweijiang  ����
====================================================================*/
BOOL topoJudgeLegality( TMcuTopo atMcuTopo[], u16 wMcuTopoNum,
							   TMtTopo atMtTopo[], u16 wMtTopoNum )
{
	u16	wLoop, wLoop2;
	u16	wPath1[5];
	u16	wFlag = 0;
	BOOL	bResult = TRUE;

	ASSERT( atMcuTopo != NULL );

	//һ��MCU���ϼ�MCU�Ƿ���ڵ��ж�
	wFlag = 0;	
	for( wLoop = 0; wLoop < wMcuTopoNum; wLoop++ )
	{
		for( wLoop2 = 0; wLoop2 < wMcuTopoNum; wLoop2++ )
		{
			if( atMcuTopo[wLoop].GetSMcuId() == atMcuTopo[wLoop2].GetMcuId() )
			{
				wFlag++;
				break;
			}
		}
		if( wFlag == 0 )	//��0��ʾ������
		{
			printf( "MCU%d's Super-MCU MCU%d does not exist!\n", atMcuTopo[wLoop].GetMcuId(),
				atMcuTopo[wLoop].GetSMcuId() );
			bResult = FALSE;
		}
		wFlag = 0;
	}

	//���˽ṹ��TOPMCUΨһ���ж�
	wFlag = 0;	
	for( wLoop = 0; wLoop < wMcuTopoNum; wLoop++ )
	{
		if( atMcuTopo[wLoop].GetMcuId() == atMcuTopo[wLoop].GetSMcuId() )
		{
			wFlag++;
		}
	}
	if( wFlag > 1 )		//���
	{
		printf( "The TOPMCU is not only one in topology!\n" );
		bResult = FALSE;
	}
	if( wFlag == 0 )	//��0��ʾ������
	{
		printf( "The TOPMCU does not exist in topology!\n" );
		bResult = FALSE;
	}

	//һ��MT����MCU�Ƿ�����ж�
	wFlag = 0;
	for( wLoop = 0; wLoop < wMtTopoNum; wLoop++ )
	{
		for( wLoop2 = 0; wLoop2 < wMcuTopoNum; wLoop2++ )
		{
			if( atMtTopo[wLoop].GetMcuId() == atMcuTopo[wLoop2].GetMcuId() )
			{
				wFlag++;
				break;
			}
		}
		if( wFlag == 0 )//��0��ʾ������
		{
			printf( "MCU%d-MT%d's Super-MCU MCU%d does not exist!\n", 
					atMtTopo[wLoop].GetMcuId(), atMtTopo[wLoop].m_byMtId, 
					atMtTopo[wLoop].GetMcuId() );
			bResult = FALSE;
		}
		wFlag = 0;
	}

	//MCU���¼����ϵ�Ƿ��ͻ�ж�
	for( wLoop = 0; wLoop < wMcuTopoNum; wLoop++ )
	{
		if( !topoGetMcuTopo( atMcuTopo[wLoop].GetMcuId(), wPath1, 5, atMcuTopo, wMcuTopoNum ) )
		{
			printf( "There could have some mistakes between MCU%u and its superior MCUs!\n", 
				atMcuTopo[wLoop].GetMcuId() );
			continue;
		}
		for( wLoop2 = 1; wLoop2<5 && wPath1[wLoop2] != 0; wLoop2++)
		{
			if( wPath1[wLoop2] == atMcuTopo[wLoop].GetMcuId() )
			{
				printf( "MCU%d Super-MCU or indirect Super-MCU MCU%d is not correct!\n", 
					atMcuTopo[wLoop].GetMcuId(), atMcuTopo[wLoop].GetSMcuId() );
				bResult = FALSE;
				break;
			}
		}
	}

	//MT�����ж�
	for( wLoop = 0; wLoop < wMtTopoNum; wLoop++ )
	{
		if( atMtTopo[wLoop].m_byMtType < MTTYPE_RCVTRS || atMtTopo[wLoop].m_byMtType > MTTYPE_RCVTRSADAPT )
		{
			printf( "MCU%d-MT%d's type %u not exist!\n", 
					atMtTopo[wLoop].GetMcuId(), atMtTopo[wLoop].m_byMtId, atMtTopo[wLoop].m_byMtType );
			bResult = FALSE;
		}
	}

	//MT����Ǵμ��նˣ��������ն˵����ն˱�����0
	for( wLoop = 0; wLoop < wMtTopoNum; wLoop++ )
	{
		if( atMtTopo[wLoop].m_byPrimaryMtId != 0 )
		{
			for( wLoop2 = 0; wLoop2 < wMtTopoNum; wLoop2++ )
			{
				//�ж����������ն˵����ն�
				if( atMtTopo[wLoop2].GetMcuId() == atMtTopo[wLoop].GetMcuId() 
					&& atMtTopo[wLoop2].m_byMtId == atMtTopo[wLoop].m_byPrimaryMtId )
				{
					if( atMtTopo[wLoop2].m_byPrimaryMtId !=0 )
					{	
						printf( "MCU%d-MT%d's Primary-MT MCU%d-MT%d is not right!\n", 
							atMtTopo[wLoop].GetMcuId(), atMtTopo[wLoop].m_byMtId, 
							atMtTopo[wLoop].GetMcuId(), atMtTopo[wLoop].m_byPrimaryMtId );
						bResult = FALSE;
						break;
					}
				}
			}
		}
	}

	return bResult;
}

/*====================================================================
    ������      ��topoIsValidMt
    ����        �������ն˺ţ�IP��ַ�ͼ������кϷ����ж�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId, MCU��
				  u8 byMtId, MT��
				  LPCSTR lpszAlias, �ն˼���
				  TMtTopo atMtTopo[], Mt����������
				  u16 wMtTopoNum, ȫ��Mt����Ŀ
    ����ֵ˵��  ���ɹ�����TRUE��ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/05    1.0         LI Yi         ����
====================================================================*/
BOOL topoIsValidMt( u16 wMcuId, u8 byMtId, LPCSTR lpszAlias, TMtTopo atMtTopo[], u16 wMtTopoNum )
{
	TMtTopo	tMtInfo;

	tMtInfo = topoGetMtInfo( wMcuId, byMtId, atMtTopo, wMtTopoNum );

	//ID
	if( tMtInfo.GetMcuId()!= wMcuId || tMtInfo.m_byMtId != byMtId )
	{
		printf( "Invalid MT%u-%u! Wrong MCU or MT ID!\n", wMcuId, byMtId );
		return( FALSE );
	}

	//Alias
	if( strcmp( tMtInfo.GetAlias(), lpszAlias ) != 0 )
	{
		printf( "Invalid MT%u-%u! Wrong Alias %s!\n", wMcuId, byMtId, lpszAlias );
		return( FALSE );
	}

	return( TRUE );
}

/*====================================================================
    ������      ��topoIsValidMcu
    ����        ������MCU�ţ�IP��ַ�ͼ������кϷ����ж�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId, MCU��
				  u32 dwIpAddr, �ն�IP��ַ��������
				  LPCSTR lpszAlias, �ն˼���
				  TMcuTopo atMcuTopo[], Mcu����������
				  u16 wMcuTopoNum, ȫ��Mcu����Ŀ
    ����ֵ˵��  ���ɹ�����TRUE��ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/05    1.0         LI Yi         ����
====================================================================*/
BOOL topoIsValidMcu( u16 wMcuId, u32 dwIpAddr, LPCSTR lpszAlias, 
						   TMcuTopo atMcuTopo[], u16 wMcuTopoNum )
{
	TMcuTopo	tMcuInfo;

	ASSERT( atMcuTopo != NULL );

	tMcuInfo = topoGetMcuInfo( wMcuId, atMcuTopo, wMcuTopoNum );

	//ID
	if( tMcuInfo.GetMcuId()!= wMcuId )
	{
		printf( "Invalid Mcu%u! Wrong MCU ID!\n", wMcuId );
		return( FALSE );
	}

	//IP
	if( tMcuInfo.GetIpAddr() != dwIpAddr && tMcuInfo.GetGwIpAddr() != dwIpAddr )
	{
		printf( "Invalid Mcu%u! Wrong IP %#.8x!\n", wMcuId, dwIpAddr );
		return( FALSE );
	}

	//Alias
	if( strcmp( tMcuInfo.GetAlias(), lpszAlias ) != 0 )
	{
		printf( "Invalid Mcu%u! Wrong Alias %s!\n", wMcuId, lpszAlias );
		return( FALSE );
	}

	return( TRUE );
}

/*====================================================================
    ������      ��topoFindSubMtByIp
    ����        ������MCU�ţ�IP��ַ�õ��¼��ն˺�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId, MCU��
				  u32 dwIpAddr, �ն�IP��ַ��������
				  TMcuTopo atMtTopo[]��Mt����������
				  u16 wMtTopoNum��ȫ����Mt����Ŀ
    ����ֵ˵��  ���ɹ������ն˺ţ�ʧ�ܷ���0
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/03/12    1.0         LI Yi         ����
====================================================================*/
u8 topoFindSubMtByIp( u16 wMcuId, u32 dwIpAddr, TMtTopo atMtTopo[], u16 wMtTopoNum )
{
    ASSERT( atMtTopo != NULL );

	for( u16 wLoop = 0; wLoop < wMtTopoNum; wLoop++ )
	{
		if( atMtTopo[wLoop].GetMcuId() == wMcuId && atMtTopo[wLoop].GetIpAddr() == dwIpAddr )
		{
			return( atMtTopo[wLoop].m_byMtId );
		}
	}

	return( 0 );
}

/*====================================================================
    ������      ��topoMtNeedAdapt
    ����        ���ж��ն��Ƿ���Ҫ��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId, MCU��
				  u32 dwIpAddr, �ն�IP��ַ��������
				  TMcuTopo atMtTopo[]��Mt����������
				  u16 wMtTopoNum��ȫ����Mt����Ŀ
    ����ֵ˵��  ���ɹ������ն˺ţ�ʧ�ܷ���0
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/03/12    1.0         LI Yi         ����
====================================================================*/
BOOL topoMtNeedAdapt( u16 wMcuId, u8 byMtId, TMtTopo atMtTopo[], u16 wMtTopoNum )
{
    ASSERT( atMtTopo != NULL );

	for( u16 wLoop = 0; wLoop < wMtTopoNum; wLoop++ )
	{
		if( atMtTopo[wLoop].GetMcuId() == wMcuId && atMtTopo[wLoop].m_byMtId == byMtId )
		{
			if( atMtTopo[wLoop].m_byMtType == MTTYPE_RCVTRSADAPT )	//��Ҫ����
				return( TRUE );
			else
				return( FALSE );
		}
	}

	return( FALSE );
}




/*====================================================================
    ������      ��ReadMcuTopoTable
    ����        ������MCU������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�������
    �������˵����LPCSTR lpszProfileName, �ļ�����������·����
                  TMcuTopo atMcuTopoBuf[], MCU��ϢBUFFER
				  u16 wBufSize, BUFFER��С
    ����ֵ˵��  ��������MCUʵ����Ŀ
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	02/09/18    1.1         LI Yi         ����
====================================================================*/
u8 ReadMcuTopoTable( LPCSTR lpszProfileName, TMcuTopo atMcuTopo[], u16 wBufSize )
{
	LPSTR   *lpszTable;
    char    chSeps[] = " \t";       /* space or tab as seperators */
    char    *pchToken;
    u32		dwLoop;
    BOOL    bResult = TRUE;
    u32		dwMemEntryNum;
	u32		dwMcuNum;

    ASSERT( atMcuTopo != NULL );
    
    /* get the number of entry */
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuTopoTable, STR_ENTRY_NUM, 
                0, ( int * )&dwMemEntryNum );

    dwMemEntryNum = min( dwMemEntryNum, wBufSize );
	lpszTable = (LPSTR *)malloc( dwMemEntryNum * sizeof( LPSTR ) );
    for( dwLoop = 0; dwLoop < dwMemEntryNum; dwLoop++ )
    {
		lpszTable[dwLoop] = (char *)malloc( MAX_VALUE_LEN + 1 );
    }
    dwMcuNum = dwMemEntryNum;

	/* get entry strings */
    bResult = GetRegKeyStringTable( lpszProfileName, SECTION_mcuTopoTable,
                   "fail", lpszTable, &dwMcuNum, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )
    {
        dwMcuNum = 0;
    }
    
	/* analyze entry strings */
    for( dwLoop = 0; dwLoop < dwMcuNum && dwLoop < wBufSize; dwLoop++ )
    {
        /* McuId  */
        pchToken = strtok( lpszTable[dwLoop], chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", FILED_McuId );
            bResult = FALSE;
        }
        else
        {
            atMcuTopo[dwLoop].SetMcuId( atoi( pchToken ) );
        }
        /* MCU��IP��ַ */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", FILED_McuIpAddr );
            bResult = FALSE;
        }
        else
        {
           atMcuTopo[dwLoop].SetIpAddr( ntohl(inet_addr( pchToken ) ) );
        }
        /* MCU������IP��ַ */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", FILED_McuGwIpAddr );
            bResult = FALSE;
        }
        else
        {
           atMcuTopo[dwLoop].SetGwIpAddr( ntohl(inet_addr( pchToken )) );
        }
        /* MCU���� */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", FILED_McuAlias );
            bResult = FALSE;
        }
        else
        {
           atMcuTopo[dwLoop].SetAlias( pchToken );
        }
    }
	dwMcuNum = dwLoop;

    /* free memory */
    for( dwLoop = 0; dwLoop < dwMemEntryNum; dwLoop++ )
    {
		free( lpszTable[dwLoop] );
    }
	free( lpszTable );

	return( ( u8 )dwMcuNum );
}

/*====================================================================
    ������      ��ReadMtTopoTable
    ����        ������MT������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�������
    �������˵����LPCSTR lpszProfileName, �ļ�����������·����
                  TMtTopo atMtTopoBuf[], Mt��ϢBUFFER
				  u16 wBufSize, BUFFER��С
    ����ֵ˵��  ��������Mtʵ����Ŀ
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	02/10/24    1.0         liaoweijiang  ����
====================================================================*/
u16 ReadMtTopoTable( LPCSTR lpszProfileName, TMtTopo atMtTopo[], u16 wBufSize )
{
	LPSTR   *lpszTable;
    char    chSeps[] = " \t";       /* space or tab as seperators */
    char    *pchToken;
    u32		dwLoop;
    BOOL    bResult = TRUE;
    u32		dwMemEntryNum;
	u32		dwMtNum;

    ASSERT( atMtTopo != NULL );
    
    /* get the number of entry */
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mtTopoTable, STR_ENTRY_NUM, 
                0, ( int * )&dwMemEntryNum );
    if (!bResult)
    {
        printf("[ReadMtTopoTable] GetRegKeyInt failed with lpszProName.%s!\n", lpszProfileName);
        return 0;
    }
	lpszTable = (LPSTR *)malloc( dwMemEntryNum * sizeof( LPSTR ) );
	dwMemEntryNum = min( dwMemEntryNum, wBufSize );
    for( dwLoop = 0; dwLoop < dwMemEntryNum; dwLoop++ )
    {
		lpszTable[dwLoop] = (char *)malloc( MAX_VALUE_LEN + 1 );
    }
    dwMtNum = dwMemEntryNum;

    printf("[ReadMtTopoTable] mtNum.%d\n", dwMtNum);

	/* get entry strings */
    bResult = GetRegKeyStringTable( lpszProfileName, SECTION_mtTopoTable,
                   "fail", lpszTable, &dwMtNum, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )
    {
        dwMtNum = 0;
    }
    printf("[ReadMtTopoTable] mtNum.%d, getStrRet.%d\n", dwMtNum, bResult);

    
	/* analyze entry strings */
    for( dwLoop = 0; dwLoop < dwMtNum && dwLoop < wBufSize; dwLoop++ )
    {
        /* MtId  */
        pchToken = strtok( lpszTable[dwLoop], chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", FILED_MtId );
            bResult = FALSE;
        }
        else
        {
            atMtTopo[dwLoop].SetMtId( atoi( pchToken ) );
        }
        /* McuId */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", FILED_McuId );
            bResult = FALSE;
        }
        else
        {
           atMtTopo[dwLoop].SetMcuId( atoi( pchToken ) );
        }
        /* Mt��IP��ַ */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", FILED_MtIpAddr );
            bResult = FALSE;
        }
        else
        {
           atMtTopo[dwLoop].SetIpAddr( ntohl(inet_addr( pchToken )) );
        }
        /* Mt������IP��ַ */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", FILED_MtGwIpAddr );
            bResult = FALSE;
        }
        else
        {
           atMtTopo[dwLoop].SetGwIpAddr( ntohl(inet_addr( pchToken )) );
        }
        /* Mt���� */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", FILED_MtAlias );
            bResult = FALSE;
        }
        else
        {
           atMtTopo[dwLoop].SetAlias( pchToken );
        }
         /* �ն����� */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", FILED_MtType );
            bResult = FALSE;
        }
        else
        {
           atMtTopo[dwLoop].SetMtType( atoi(pchToken) );
        }
		
		pchToken = strtok( NULL, chSeps );
		if ( pchToken == NULL )
		{
			printf( "Wrong profile while reading %s\n!", FILED_MtConnected );
            bResult = FALSE;
		}
		else
		{
			atMtTopo[dwLoop].SetConnected( atoi(pchToken) );
		}
		/* Mt��MODEM IP��ַ */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading Modem Ip\n!");
            bResult = FALSE;
        }
        else
        {
			atMtTopo[dwLoop].m_dwMtModemIp= inet_addr( pchToken );
        }
		/* Mt��MODEM �˿� */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading Modem Port\n!" );
            bResult = FALSE;
        }
        else
        {
			atMtTopo[dwLoop].m_wMtModemPort = htons( atoi( pchToken ) );
        }
		
		pchToken = strtok( NULL, chSeps );
		if ( pchToken == NULL )
		{
			printf( "Wrong profile while reading Modem Type\n!" );
            bResult = FALSE;
		}
		else
		{
			atMtTopo[dwLoop].m_byMtModemType = atoi(pchToken);
		}

		/* Mt �Ƿ�����ն�*/
		pchToken = strtok( NULL, chSeps );
		if ( NULL == pchToken)
		{
			printf( "Wrong profile while reading MT high defection\n!" );
			bResult = FALSE;
		}
		else
		{
			BOOL bMtHd = atoi(pchToken) != 0 ? TRUE : FALSE;
			atMtTopo[dwLoop].SetMtHd(bMtHd);
		}

    }
	dwMtNum = dwLoop;

    /* free memory */
    for( dwLoop = 0; dwLoop < dwMemEntryNum; dwLoop++ )
    {
		free( lpszTable[dwLoop] );
    }

	free( lpszTable );

	return( ( u16 )dwMtNum );
}

/*====================================================================
    ������      :ReadMcuModemTable
    ����        ������MCU Modem������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�������
    �������˵����LPCSTR lpszProfileName, �ļ�����������·����
                  TMtTopo atMtTopoBuf[], Mt��ϢBUFFER
				  u16 wBufSize, BUFFER��С
    ����ֵ˵��  ��������Mtʵ����Ŀ
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	02/10/24    1.0         liaoweijiang  ����
====================================================================*/
u8 ReadMcuModemTable( LPCSTR lpszProfileName, TMcuModemTopo atMcuModemTopo[], u16 wBufSize )
{
	LPSTR   *lpszTable;
    char    chSeps[] = " \t";       /* space or tab as seperators */
    char    *pchToken;
    u32		dwLoop;
    BOOL    bResult = TRUE;
    u32		dwMemEntryNum;
	u32		dwMtNum;

    ASSERT( atMcuModemTopo != NULL );
    
    /* get the number of entry */
    bResult = GetRegKeyInt( lpszProfileName, "mcuModemTable", STR_ENTRY_NUM, 
                0, ( int * )&dwMemEntryNum );

	lpszTable = (LPSTR *)malloc( dwMemEntryNum * sizeof( LPSTR ) );
	dwMemEntryNum = min( dwMemEntryNum, wBufSize );
    for( dwLoop = 0; dwLoop < dwMemEntryNum; dwLoop++ )
    {
		lpszTable[dwLoop] = (char *)malloc( MAX_VALUE_LEN + 1 );
    }
    dwMtNum = dwMemEntryNum;

	/* get entry strings */
    bResult = GetRegKeyStringTable( lpszProfileName, "mcuModemTable",
                   "fail", lpszTable, &dwMtNum, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )
    {
        dwMtNum = 0;
    }
    
	/* analyze entry strings */
    for( dwLoop = 0; dwLoop < dwMtNum && dwLoop < wBufSize; dwLoop++ )
    {
        /* ModemId  */
        pchToken = strtok( lpszTable[dwLoop], chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", "Mcu Modem Id" );
            bResult = FALSE;
        }
        else
        {
            atMcuModemTopo[dwLoop].m_byModemId = ( atoi( pchToken ) );
        }
        /* IP*/
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", "Mcu Modem Ip" );
            bResult = FALSE;
        }
        else
        {
           atMcuModemTopo[dwLoop].m_dwMcuModemIp = inet_addr( pchToken );
        }
        /* Port */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", "Mcu Modem Port" );
            bResult = FALSE;
        }
        else
        {
           atMcuModemTopo[dwLoop].m_wMcuModemPort = htons( atoi( pchToken ) );
        }
        /* Modem Type */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", "Mcu Modem type" );
            bResult = FALSE;
        }
        else
        {
           atMcuModemTopo[dwLoop].m_byMcuModemType = atoi( pchToken );
        }
		//S port
		pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
           atMcuModemTopo[dwLoop].m_bySPortNum = (u8)dwLoop+1;
        }
        else
        {
           atMcuModemTopo[dwLoop].m_bySPortNum = atoi( pchToken );
        }
    }
	dwMtNum = dwLoop;

    /* free memory */
    for( dwLoop = 0; dwLoop < dwMemEntryNum; dwLoop++ )
    {
		free( lpszTable[dwLoop] );
    }

	free( lpszTable );

	return( ( u8 )dwMtNum );
}


u8 GetSatMediaType(u8 byType)
{
	u8 bySatType = 0;
	switch (byType)
	{
	case MEDIA_TYPE_PCMU:	bySatType = SAT_AUDIO_G711U;	break;
	case MEDIA_TYPE_PCMA:	bySatType = SAT_AUDIO_G711A;	break;
	case MEDIA_TYPE_G722:	bySatType = SAT_AUDIO_G722;		break;
	case MEDIA_TYPE_G7231:	bySatType = SAT_AUDIO_G7231;		break;
	case MEDIA_TYPE_G728:	bySatType = SAT_AUDIO_G728;		break;
	case MEDIA_TYPE_G729:	bySatType = SAT_AUDIO_G729;		break;
	case MEDIA_TYPE_MP3:	bySatType = SAT_AUDIO_MP3;		break;

	case MEDIA_TYPE_H261:	bySatType = SAT_VIDEO_H261;		break;
	case MEDIA_TYPE_H263:	bySatType = SAT_VIDEO_H263;		break;
	case MEDIA_TYPE_H264:	bySatType = SAT_VIDEO_H264;		break;
	case MEDIA_TYPE_H262:	bySatType = SAT_VIDEO_MPEG2;		break;
	case MEDIA_TYPE_MP4:	bySatType = SAT_VIDEO_MPEG4;		break;
	default:
		OspPrintf(TRUE, FALSE, "[GetSatMediaType] unexpected type.%d, ignore it!\n", byType);
		break;
	}

	return bySatType;
}

u8 GetSatRes(u8 byRes)
{
	u8 bySatRes = TYPE_CIF;
	switch (byRes)
	{
	case VIDEO_FORMAT_CIF:	bySatRes = TYPE_CIF;	break;
	case VIDEO_FORMAT_2CIF:	bySatRes = TYPE_2CIF;	break;
	case VIDEO_FORMAT_4CIF:	bySatRes = TYPE_4CIF;	break;
	case VIDEO_FORMAT_16CIF:	bySatRes = TYPE_16CIF;	break;
	case VIDEO_FORMAT_QCIF:		bySatRes = TYPE_QCIF;	break;
	case VIDEO_FORMAT_SQCIF:	bySatRes = TYPE_SQCIF;	break;
	}

	return bySatRes;
}

//END OF FILE
