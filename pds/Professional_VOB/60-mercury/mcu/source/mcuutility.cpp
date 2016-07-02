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

//#include "evmcumcs.h"
//#include "mcuutility.h"
#include "eqplib.h"
#include "bindmp.h"
#include "bindmtadp.h"
#include "mcupfmlmt.h"


#include<sys/types.h>   
#include<sys/stat.h>   

#define F_WHNONELARGER			-2	// none larger in both
#define F_HLARGER				-1	// only larger in height
#define WHEQUAL					0	// equal
#define F_WLARGER				1	// only larger in width
#define F_WHLARGER				2	// larger in both
//#define F_NOMATCH				3	// no match

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

/*lint -save -e1536*/

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
/*
void TConfPollParam::InitPollParam(u8 byMtNum, TMtPollParam* ptParam)
{
    SetPollList(byMtNum, ptParam) ;

    m_byCurPollPos = 0;
    ClearSpecPos();
}
*/
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
/*
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
*/
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
/*
u8 TConfPollParam::GetPolledMtNum() const
{
    return m_byPollMtNum;
}
*/
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
/*
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
*/
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
/*
void TConfPollParam::SetCurrentIdx(u8 byIdx)
{
    if ( byIdx < m_byPollMtNum )
    {
        m_byCurPollPos = byIdx;
    }
}
*/

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
/*
u8 TConfPollParam::GetCurrentIdx() const
{
    return m_byCurPollPos;
}
*/
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
/*
TMtPollParam* const TConfPollParam::GetCurrentMtPolled()
{
    return &m_atMtPollParam[m_byCurPollPos];
}
*/
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
/*
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
*/
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
/*
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
*/
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
/*
TEqp TTvWallPollParam::GetTvWallEqp( void )
{
	return m_tTvWall;
}
*/
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
/*
void TTvWallPollParam::SetTvWallEqp( const TEqp &tTvWall )
{
	m_tTvWall = tTvWall;
}
*/
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
/*
u8 TTvWallPollParam::GetTvWallChnnl( void )
{
	return m_byTvWallChnnl;
}
*/
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
/*
void TTvWallPollParam::SetTvWallChnnl( u8 byChnIndex )
{
	m_byTvWallChnnl = byChnIndex;
}
*/
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
		LogPrint( LOG_LVL_ERROR, MID_MCU_MT, "Exception - TConfMtData::SetMtLogicChnnl(): wrong byChannel %u!\n", byChannel );
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
    10/01/18	4,6			Ѧ��			create
====================================================================*/
/*BOOL32 TConfMtTable::GetMtLogicChnnl( const TMt &tMt, u8 byChannel, TLogicalChannel * ptChnnl, BOOL32 bForwardChnnl )
{
	if( tMt.IsNull() )
	{
		return FALSE;
	}

	//zjj20100720 ����������getmcuid�޷���ʾmcuid�ˣ�����Ҫ��֤�����������Ǳ����ն�
	if( !tMt.IsLocal() )
	{
		return FALSE;
	}

	u8 byMtOrMcuId =  tMt.GetMtId() ;
	return GetMtLogicChnnl( byMtOrMcuId, byChannel, ptChnnl, bForwardChnnl);

	return FALSE;
}*/
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
		LogPrint( LOG_LVL_ERROR, MID_MCU_MT, "Exception - TConfMtTable::GetMtLogicChnnl(): wrong byChannel %u!\n", byChannel );
		return FALSE;
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

	dwDstIp = 0;
	wDstPort = 0;

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
		tSimCapSet.SetVideoMaxBitRate( m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel.GetFlowControl() );
		if (MEDIA_TYPE_H264 == tSimCapSet.GetVideoMediaType())
		{
			tSimCapSet.SetVideoProfileType(m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel.GetProfileAttrb());
			tSimCapSet.SetUserDefFrameRate(m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel.GetChanVidFPS());
		}
		else
		{
			tSimCapSet.SetVideoFrameRate(m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel.GetChanVidFPS());
		}
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
		tSimCapSet.SetVideoMaxBitRate( m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel.GetFlowControl() );
		if (MEDIA_TYPE_H264 == tSimCapSet.GetVideoMediaType())
		{
			tSimCapSet.SetVideoProfileType( m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel.GetProfileAttrb() );
			tSimCapSet.SetUserDefFrameRate(m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel.GetChanVidFPS());
		}
		else
		{
			tSimCapSet.SetVideoFrameRate(m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel.GetChanVidFPS());
		}
	}

	return tSimCapSet;
}

/*====================================================================
    ������       GetSrcDSSCS
    ����        ���õ��ն�˫����������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    20110408    4.6         �ܼ���         ����
====================================================================*/
TSimCapSet TConfMtTable::GetSrcDSSCS( u8 byMtId )
{
	TSimCapSet tSimCapSet;
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return tSimCapSet;
	}

	if( m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel.IsOpened() )
	{
		tSimCapSet.SetVideoMediaType( m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel.GetChannelType() );
        tSimCapSet.SetVideoResolution( m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel.GetVideoFormat() );
		tSimCapSet.SetVideoMaxBitRate( m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel.GetFlowControl() );
		if (MEDIA_TYPE_H264 == tSimCapSet.GetVideoMediaType())
		{
			tSimCapSet.SetVideoProfileType(m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel.GetProfileAttrb());
			tSimCapSet.SetUserDefFrameRate(m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel.GetChanVidFPS());
		}
		else
		{
			tSimCapSet.SetVideoFrameRate(m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel.GetChanVidFPS());
		}
	}

	return tSimCapSet;
}
/*====================================================================
    ������         GetDSSCS
    ����        ���õ�Ŀ���ն˵�˫����������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��    TSimCapSet
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    20110408    4.6         �ܼ���         ����
====================================================================*/
TSimCapSet TConfMtTable::GetDstDSSCS(u8 byMtId)
{
	TSimCapSet tSimCapSet;
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return tSimCapSet;
	}
	
	if( m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel.IsOpened() )
	{
		tSimCapSet.SetVideoMediaType( m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel.GetChannelType() );
        tSimCapSet.SetVideoResolution( m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel.GetVideoFormat() );
		tSimCapSet.SetVideoMaxBitRate( m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel.GetFlowControl() );
		if (MEDIA_TYPE_H264 == tSimCapSet.GetVideoMediaType())
		{
			tSimCapSet.SetVideoProfileType(m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel.GetProfileAttrb());
			tSimCapSet.SetUserDefFrameRate(m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel.GetChanVidFPS());
		}
		else
		{
			tSimCapSet.SetVideoFrameRate(m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel.GetChanVidFPS());
		}
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

	case LOGCHL_SECVIDEO:
		return 	bForwardChnnl ? m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel.IsOpened()
			                  : m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel.IsOpened();

	case LOGCHL_AUDIO:
		return 	bForwardChnnl ? m_atMtData[byMtId - 1].m_tFwdChannel.m_tAudioChannel.IsOpened()
			                  : m_atMtData[byMtId - 1].m_tRvsChannel.m_tAudioChannel.IsOpened();

	case LOGCHL_T120DATA:
		return 	bForwardChnnl ? m_atMtData[byMtId - 1].m_tFwdChannel.m_tT120DataChannel.IsOpened()
			                  : m_atMtData[byMtId - 1].m_tRvsChannel.m_tT120DataChannel.IsOpened();

	case LOGCHL_H224DATA:
		return 	bForwardChnnl ? m_atMtData[byMtId - 1].m_tFwdChannel.m_tH224DataChannel.IsOpened()
			                  : m_atMtData[byMtId - 1].m_tRvsChannel.m_tH224DataChannel.IsOpened();
		
	default:
		return FALSE;
	}
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
    
//  	m_atMtData[byMtId - 1].m_tCapSupport = *ptCapSupport;
    memcpy(&m_atMtData[byMtId - 1].m_tCapSupport, ptCapSupport, sizeof(TCapSupport));

	return;
}

/*====================================================================
    ������      ��SetMtMultiCapSupport
    ����        �������ն˶���������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
				  const TMultiCapSupport * ptCapSupport, �ն˶���������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    06/14/11    3.0         ��ʤ��         ����
====================================================================*/
void TConfMtTable::SetMtMultiCapSupport( u8 byMtId, const TMultiCapSupport * ptMultiCapSupport )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptMultiCapSupport == NULL )
	{
		return ;
	}
    
 	m_atMtData[byMtId - 1].m_tCapSupport = *ptMultiCapSupport;

	return;
}
/*====================================================================
    ������      ��GetMtMultiCapSupport
    ����        ������ն˶���������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
				  TMultiCapSupport * ptCapSupport, �ն˶���������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    06/18/11    3.0         ��ʤ��         ����
====================================================================*/
BOOL32 TConfMtTable::GetMtMultiCapSupport(u8 byMtId, TMultiCapSupport * ptMultiCapSupport)
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptMultiCapSupport == NULL )
    {
        return FALSE;
	}

    *ptMultiCapSupport = m_atMtData[byMtId - 1].m_tCapSupport;

    return TRUE;
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
	
// 	*ptCapSupport = m_atMtData[byMtId - 1].m_tCapSupport;
    memcpy(ptCapSupport, &m_atMtData[byMtId - 1].m_tCapSupport, sizeof(TCapSupport));
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
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
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

	TMtAlias tPuAlias;

	// PU�������IP+ALIAS
	s8* pbyAlias = (s8*)(ptAlias->m_achAlias);
	m_atMtExt[byMtId - 1].SetIPAddr(ntohl(*(u32*)pbyAlias));
	tPuAlias.m_tTransportAddr.SetIpAddr(ntohl(*(u32*)pbyAlias));

	pbyAlias += sizeof(u32);
	m_atMtExt[byMtId - 1].SetAlias( pbyAlias );
    m_atMtExt[byMtId - 1].SetProtocolType(PROTOCOL_TYPE_H323);
	tPuAlias.SetH323Alias(pbyAlias);
		
	m_atMtData[byMtId - 1].m_atMtAlias[tPuAlias.m_AliasType - 1] = tPuAlias;
	
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

// ����pclint�Ĵ���:Warning -- Expected unsigned type
/*lint -save -e502*/

	m_atMtData[byMtId - 1].m_byMonitorDstMt[(byDstMtId-1) / 8] &= ~( 1 << ( (byDstMtId-1) % 8 ) );
/*lint -restore*/

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
        LogPrint( LOG_LVL_ERROR, MID_MCU_MT, "[GetMtSndBitrate] get mt sending bitrate error!!!\n");
        return 0;
    }
}

/*====================================================================
    ������      : GetDisconnetReason
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  : LPCSTR
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/09/12    4.5         �ű���           ����
====================================================================*/
u8 TConfMtTable::GetDisconnectReason(u8 byMtId)
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return 0;
    }
    return m_atMtData[byMtId-1].m_byDisconnetReason;    
}

/*====================================================================
    ������      : SetDisconnectReason
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  : LPCSTR
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/09/12    4.5         �ű���           ����
====================================================================*/
void TConfMtTable::SetDisconnectReason(u8 byMtId,u8 byReason)
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return;
    }
    m_atMtData[byMtId-1].m_byDisconnetReason = byReason;    
}

/*====================================================================
    ������      ��SetRecordName
    ����        ������vrs¼���ļ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˺�
				  LPCSTR lpszName
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    13/10/10    4.7         �־         ����
====================================================================*/
void TConfMtTable::SetRecordName( u8 byMtId, LPCSTR lpszName )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || NULL == lpszName)
	{
		return;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		m_atVrsRecChlStatus[byVrsIdx].SetRecordName( lpszName );
	}
	return;
}

/*====================================================================
    ������      ��GetRecordName
    ����        ���õ�vrs¼���ļ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    13/10/10    4.7         �־         ����
====================================================================*/
BOOL32 TConfMtTable::GetRecordName( u8 byMtId, char *szAlias, u16 wBufLen )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		m_atVrsRecChlStatus[byVrsIdx].GetRecordName(szAlias, wBufLen);
		return TRUE;
	}
	return FALSE;
}

/*====================================================================
    ������      ��SetRecChlType
    ����        ������vrs¼��ͨ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˺�
				  u8 byRecChlType
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    13/10/10    4.7         �־         ����
====================================================================*/
void TConfMtTable::SetRecChlType( u8 byMtId, u8 byRecChlType )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		m_atVrsRecChlStatus[byVrsIdx].m_byType = byRecChlType;
	}
	return;
}

/*====================================================================
    ������      ��GetRecChlType
    ����        ���õ�vrs¼��ͨ������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    13/10/10    4.7         �־         ����
====================================================================*/
u8 TConfMtTable::GetRecChlType( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		return m_atVrsRecChlStatus[byVrsIdx].m_byType;
	}
	return 0;
}

/*====================================================================
    ������      ��SetRecChlState
    ����        ������vrs¼��ͨ��״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˺�
				  u8 byRecChlState
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    13/10/10    4.7         �־         ����
====================================================================*/
void TConfMtTable::SetRecChlState( u8 byMtId, u8 byRecChlState )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		m_atVrsRecChlStatus[byVrsIdx].m_byState = byRecChlState;
	}
	return;
}

/*====================================================================
    ������      ��GetRecChlState
    ����        ���õ�vrs¼��ͨ��״̬
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    13/10/10    4.7         �־         ����
====================================================================*/
u8 TConfMtTable::GetRecChlState( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		return m_atVrsRecChlStatus[byVrsIdx].m_byState;
	}
	return 0;
}

/*====================================================================
    ������      ��SetRecMode
    ����        ������vrs¼��ʽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˺�
				  u8 byRecMode
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    13/10/10    4.7         �־         ����
====================================================================*/
void TConfMtTable::SetRecMode( u8 byMtId, u8 byRecMode )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		m_atVrsRecChlStatus[byVrsIdx].m_byRecMode = byRecMode;
	}
	return;
}

/*====================================================================
    ������      ��GetRecMode
    ����        ���õ�vrs¼��ʽ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    13/10/10    4.7         �־         ����
====================================================================*/
u8 TConfMtTable::GetRecMode( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		return m_atVrsRecChlStatus[byVrsIdx].m_byRecMode;
	}
	return 0;
}

/*====================================================================
    ������      ��SetRecProg
    ����        ������vrs¼��ǰ¼���������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˺�
				  TRecProg tRecProg
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    13/10/10    4.7         �־         ����
====================================================================*/
void TConfMtTable::SetRecProg( u8 byMtId, TRecProg tRecProg )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		m_atVrsRecChlStatus[byVrsIdx].m_tProg = tRecProg;
	}
	return;
}

/*====================================================================
    ������      ��GetRecProg
    ����        ���õ�vrs¼��ǰ¼���������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    13/10/10    4.7         �־         ����
====================================================================*/
TRecProg TConfMtTable::GetRecProg( u8 byMtId )
{
	TRecProg tProg;
	memset(&tProg, 0, sizeof(tProg));
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return tProg;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		return m_atVrsRecChlStatus[byVrsIdx].m_tProg;
	}
	return tProg;
}

/*====================================================================
    ������      ��SetRecSrc
    ����        ������vrs¼��Դ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն˺�
				  TMtNoConstruct tSrc
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    13/10/10    4.7         �־         ����
====================================================================*/
void TConfMtTable::SetRecSrc( u8 byMtId, TMtNoConstruct tSrc )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		m_atVrsRecChlStatus[byVrsIdx].m_tSrc = tSrc;
	}
	return;
}

/*====================================================================
    ������      ��GetRecSrc
    ����        ���õ�vrs¼��Դ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    13/10/10    4.7         �־         ����
====================================================================*/
TMtNoConstruct TConfMtTable::GetRecSrc( u8 byMtId )
{
	TMtNoConstruct tMt;
	tMt.SetNull();
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return tMt;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		return m_atVrsRecChlStatus[byVrsIdx].m_tSrc;
	}

	return tMt;
}

/*====================================================================
    ������      ��GetVrsChlIdxbyMtId
    ����        ���õ�vrs¼��ͨ��ȫ��״̬��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ��u8 ������
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    13/10/10    4.7         �־         ����
====================================================================*/
u8 TConfMtTable::GetVrsChlIdxbyMtId( u8 byMtId )
{
	u8 byRet = 0XFF;
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return byRet;
	}
	for (u8 byIdx=0; byIdx<MAX_PRIEQP_NUM; byIdx++)
	{
		if (m_atVrsRecChlStatus[byIdx].GetVrsMtId() == byMtId)
		{
			byRet = byIdx;
			break;
		}
	}

	return byRet;
}

/*====================================================================
    ������      ��AddVrsRecChlbyMtId
    ����        ������һ���µ�vrsͨ����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ��BOOL32
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    13/10/10    4.7         �־         ����
====================================================================*/
BOOL32 TConfMtTable::AddVrsRecChlbyMtId( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}
	if (GetVrsChlIdxbyMtId(byMtId) != 0XFF)
	{
		// �Ѵ��ڣ�����TRUE
		return TRUE;
	}
	
	// ����һ������ͨ��
	for (u8 byIdx=0; byIdx<MAX_PRIEQP_NUM; byIdx++)
	{
		if (m_atVrsRecChlStatus[byIdx].GetVrsMtId() == 0)
		{
			m_atVrsRecChlStatus[byIdx].SetVrsMtId(byMtId);
			return TRUE;
		}
	}
	
	return FALSE;
}

/*====================================================================
    ������      ��GetRecChnnlStatus
    ����        ���õ�vrs¼��ͨ��ȫ��״̬��Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    13/10/10    4.7         �־         ����
====================================================================*/
TRecChnnlStatus TConfMtTable::GetRecChnnlStatus( u8 byMtId )
{
	TRecChnnlStatus tStatus;
	memset(&tStatus, 0, sizeof(tStatus));
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return tStatus;
	}

	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		return m_atVrsRecChlStatus[byVrsIdx];
	}

	return tStatus;
}

/*====================================================================
    ������      ��ResetRecChnnlStatus
    ����        ������vrs��ӦrecChnnlStatusͨ����Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    13/10/10    4.7         �־         ����
====================================================================*/
void TConfMtTable::ResetRecChnnlStatus( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return ;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		m_atVrsRecChlStatus[byVrsIdx].Clear();
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
void TConfMtTable::SetMcuId( u8 byMtId, u16 wMcuId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	m_atMtExt[byMtId - 1].SetMcuIdx( wMcuId );

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
u16 TConfMtTable::GetMcuId( u8 byMtId )
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
	return m_atMtExt[byMtId - 1].GetManuId();
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
	     
		++byMtId;
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

	TMtAlias tTempAlias;
	if(ptAlias->m_AliasType == puAliasTypeIPPlusAlias)
	{
		// PU�������IP+ALIAS
		tTempAlias.m_AliasType = puAliasTypeIPPlusAlias;
		
		s8* pbyAlias = (s8*)(ptAlias->m_achAlias);
		tTempAlias.m_tTransportAddr.SetIpAddr(ntohl(*(u32*)pbyAlias));
		
		pbyAlias += sizeof(u32);

		u32 dwAliasActualSize = strlen(pbyAlias);
		//u32 dwMaxAliasSize = sizeof(tTempAlias.m_achAlias);
		if( dwAliasActualSize > sizeof(tTempAlias.m_achAlias) )
		{
			dwAliasActualSize = sizeof(tTempAlias.m_achAlias);
		}
		memcpy(tTempAlias.m_achAlias,pbyAlias,dwAliasActualSize);
	}
	else
	{
		tTempAlias = *ptAlias;
	}
	
	u8 byMtId = GetMtIdByAlias( &tTempAlias );

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
		byMtId > MAXNUM_CONF_MT || byMtId == 0 )
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
		byMtId > MAXNUM_CONF_MT || byMtId == 0 )
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
		byMtId > MAXNUM_CONF_MT || byMtId == 0 )
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
    if( byMtId > MAXNUM_CONF_MT || byMtId == 0 )
	{
		return 0;
	}

	u16 wBitRate = 0;
	if( m_atMtData[byMtId - 1].m_tRvsChannel.m_tAudioChannel.IsOpened() )
	{
		wBitRate = m_atMtExt[byMtId - 1].GetDialBitRate() - GetAudioBitrate(m_atMtData[byMtId - 1].m_tRvsChannel.m_tAudioChannel.GetChannelType()) ;
	}
	else
	{
		wBitRate = m_atMtExt[byMtId - 1].GetDialBitRate();
	}

	return wBitRate;
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
    if( byMtId > MAXNUM_CONF_MT || byMtId == 0 )
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
    if( byMtId > MAXNUM_CONF_MT || byMtId == 0 )
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
    if (byMtId > MAXNUM_CONF_MT || byMtId == 0)
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
    if (byMtId > MAXNUM_CONF_MT || byMtId == 0)
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
    ������      ��GetVrsRecMtIdByAlias
    ����        �����ݱ�������Ϣ����vrs��¼��ʵ��Id
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMtAlias tMtAlias, vrs����������
				  u8 byType ¼����ģʽ
				  TMt tSrc  Դ�ն�
    ����ֵ˵��  ��0 - ������IPΪ����ֵ���ն�
	              ����0 - �ն�Id
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/05    3.0         ������         ����
====================================================================*/
u8 TConfMtTable::GetVrsRecMtIdByAlias( const TMtAlias * ptAlias , u8 byType, TMt tSrc)
{
    if( ptAlias == NULL || ptAlias->m_AliasType == 0 ||
		(ptAlias->m_AliasType > mtAliasTypeOthers && 
		ptAlias->m_AliasType != puAliasTypeIPPlusAlias))
	{
		return 0;
	}
#ifdef _8KI_
    u8 byMtId = 1;
	
	//����
	while( byMtId <= m_byMaxNumInUse )
	{
		if( m_atMtExt[byMtId - 1].GetMtId() == byMtId )
		{
            // �����ն˺���˿ڵĲ�ȷ���ԣ�Ip�������ٱȽ϶˿�
            if ( mtAliasTypeTransportAddress == ptAlias->m_AliasType )
            {
                if ( ptAlias->m_tTransportAddr.GetIpAddr() == 
					m_atMtData[byMtId-1].m_atMtAlias[ptAlias->m_AliasType-1].GetMtAlias().m_tTransportAddr.GetIpAddr() )
                {
					if ( m_atVrsRecChlStatus[GetVrsChlIdxbyMtId(byMtId)].m_byType == TRecChnnlStatus::TYPE_UNUSE)
					{
						// δʹ��ͨ����ƥ��
						return byMtId;
					}
					else if (m_atVrsRecChlStatus[GetVrsChlIdxbyMtId(byMtId)].m_byType == byType)
					{
						if (byType == TRecChnnlStatus::TYPE_PLAY)
						{
							// ƥ��������
							return byMtId;
						}
						else if (byType == TRecChnnlStatus::TYPE_RECORD)
						{
							if ( m_atVrsRecChlStatus[GetVrsChlIdxbyMtId(byMtId)].m_tSrc.IsNull() && tSrc.IsNull())
							{
								//ƥ�����¼��
								return byMtId;
							}
							else if (m_atVrsRecChlStatus[GetVrsChlIdxbyMtId(byMtId)].m_tSrc == tSrc)
							{
								//��ʹ��ͨ���ϸ�ƥ��¼����ģʽ��Դ�ն�
								return byMtId;
							}
						}
					}
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
#endif
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
void TConfMtTable::SetMtRecording( u8 byMtId )
{
	if( byMtId != 0 && byMtId <= MAXNUM_CONF_MT )
	{
		//lukunpeng 2010/07/14 ɾ��¼�����Ϣ����������ն�״̬��ͬ����
		/*
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState.SetRecording();
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_byRecChannel = byRecChannel;
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecEqp = tRecEqp;

		//ͬ���ն˵�¼��״̬
		m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState;
		*/

		m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState.SetRecording();
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

//lukunpeng 2010/07/14 ɾ��¼�����Ϣ����������ն�״̬��ͬ����
/*
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
*/

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

//lukunpeng 2010/07/14 ɾ��¼�����Ϣ����������ն�״̬��ͬ����
/*
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
*/
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
		//lukunpeng 2010/07/14 ɾ��¼�����Ϣ����������ն�״̬��ͬ����
		/*
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState.SetNoRecording();
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_byRecChannel = 0;
		memset( &m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecEqp , 0, sizeof( TEqp ) );
		//ͬ���ն˵�¼��״̬
		m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState;
		*/
		m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState.SetNoRecording();

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
		//lukunpeng 2010/07/14 ɾ��¼�����Ϣ����������ն�״̬��ͬ����
		/*
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState.SetRecPause();

		//ͬ���ն˵�¼��״̬
		m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState;
		*/
		m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState.SetRecPause();
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
	
	//lukunpeng 2010/07/14 ɾ��¼�����Ϣ����������ն�״̬��ͬ����
	//*ptRecState = m_atMtData[ byMtId -1 ].m_tMtRecInfo.m_tRecState;
	//ͬ���ն˵�¼��״̬
	//m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState;
	*ptRecState = m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState;
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

	//lukunpeng 2010/07/14 ɾ��¼�����Ϣ����������ն�״̬��ͬ����
	//return m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecState.IsRecording();

	return m_atMtData[ byMtId - 1].m_tMtStatus.m_tRecState.IsRecording();
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

	//lukunpeng 2010/07/14 ɾ��¼�����Ϣ����������ն�״̬��ͬ����
	//return m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecState.IsRecPause();
	return m_atMtData[ byMtId - 1].m_tMtStatus.m_tRecState.IsRecPause();


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

	//lukunpeng 2010/07/14 ɾ��¼�����Ϣ����������ն�״̬��ͬ����
	//return m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecState.IsNoRecording();
	return m_atMtData[ byMtId - 1].m_tMtStatus.m_tRecState.IsNoRecording();
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

	//lukunpeng 2010/07/14 ɾ��¼�����Ϣ����������ն�״̬��ͬ����
	//return m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecState.IsRecSkipFrame();
	return m_atMtData[ byMtId - 1].m_tMtStatus.m_tRecState.IsRecSkipFrame();
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

	//lukunpeng 2010/07/14 ɾ��¼�����Ϣ����������ն�״̬��ͬ����
	/*
	m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecState.SetRecSkipFrame( bSkipFrame );
	//ͬ���ն˵�¼��״̬
	m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState;
	*/
	m_atMtData[ byMtId - 1].m_tMtStatus.m_tRecState.SetRecSkipFrame( bSkipFrame );
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

//lukunpeng 2010/07/14 ɾ��¼�����Ϣ����������ն�״̬��ͬ����
/*
void TConfMtTable::SetMtRecProg( u8 byMtId, const TRecProg & tRecProg )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	
	m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecProg = tRecProg;

	return;
}
*/

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
//lukunpeng 2010/07/14 ɾ��¼�����Ϣ����������ն�״̬��ͬ����
/*
BOOL32 TConfMtTable::GetMtRecProg( u8 byMtId, TRecProg * ptRecProg )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptRecProg == NULL )
		return FALSE;

	*ptRecProg = m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecProg;
	return TRUE;	
}
*/

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

BOOL32 TConfMtTable::IsMtAutoInSpec( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}
	
	return (m_atMtData[byMtId - 1].m_bySpecInMix == 0) ? TRUE : FALSE;
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
void TConfMtTable::SetMtVideoRecv( u8 byMtId, BOOL32 bRecving, u16 wBitRate/* = 0*/ )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	m_atMtData[ byMtId - 1].m_tMtStatus.SetReceiveVideo( bRecving );

	if( bRecving )
	{
		m_atMtData[ byMtId - 1].m_tMtStatus.SetRecvBitRate( wBitRate );
	}
	else
	{
		m_atMtData[ byMtId - 1].m_tMtStatus.SetRecvBitRate( 0 );
	}
	
	
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
void TConfMtTable::SetMtInMixing( u8 byMtId, BOOL32 bMtInMixing, BOOL32 bAutoInSpec )
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

	//lukp ����ʱʹ��
	//LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "SetMtInMixing McuId: %d, MtId%d, bMix:%d, bAutoInSpec: %d\n", m_atMtData[ byMtId - 1].m_tMtStatus.GetMcuId(), byMtId, bMtInMixing, bAutoInSpec);

	//�����ն˽����ƻ���״̬ʱ��Ҫ�ж��ն���Ƶ�߼�ͨ���Ƿ�򿪣����û�д򿪣�����������
	//Bug00105911_tzy ȡ��������״̬ʱ����Ҫ�ж��ն��߼�ͨ���Ƿ�򿪣���Ϊ���ܸ��쳣�ն˵����ˣ���ʱ�߼�ͨ���Ѿ��رգ�
	//ͣ��������ʱҲ��Ҫ������ն˽�����״̬��ʶ�ֶ�
	if (!m_atMtData[byMtId - 1].m_tRvsChannel.m_tAudioChannel.IsOpened() && bMtInMixing)
	{
		return;
	}
	m_atMtData[byMtId - 1].m_tMtStatus.SetInMixing( bMtInMixing );

	m_atMtData[byMtId - 1].m_bySpecInMix = bAutoInSpec ? 0 : 1;
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
    ������      : GetProductId
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  : LPCSTR
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/09/12    4.5         �ű���           ����
====================================================================*/
LPCSTR TConfMtTable::GetProductId(u8 byMtId) const
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return NULL;
    }
    return m_atMtExt2[byMtId-1].GetProductId();
}

/*====================================================================
    ������      : SetProductId
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  : LPCSTR
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/09/12    4.5         �ű���           ����
====================================================================*/
void TConfMtTable::SetProductId(u8 byMtId, LPCSTR lpszProductId)
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return;
    }
    m_atMtExt2[byMtId-1].SetProductId(lpszProductId);
    return;
}

/*====================================================================
    ������      : GetVersionId
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  : LPCSTR
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/09/12    4.5         �ű���           ����
====================================================================*/
LPCSTR TConfMtTable::GetVersionId(u8 byMtId) const
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return NULL;
    }
    return m_atMtExt2[byMtId-1].GetVersionId();
}

/*====================================================================
    ������      : SetVersionId
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId, �ն˺�
    ����ֵ˵��  : LPCSTR
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    09/09/12    4.5         �ű���           ����
====================================================================*/
void TConfMtTable::SetVersionId(u8 byMtId, LPCSTR lpszVersionId)
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return;
    }
    m_atMtExt2[byMtId-1].SetVersionId(lpszVersionId);
    return;
}

/*====================================================================
    ������      : SetDisconnectDRI
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId
				  u8 byDRIId
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
====================================================================*/
void TConfMtTable::SetDisconnectDRI(u8 byMtId, u8 byDRIId)
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return;
    }
    m_atMtData[byMtId-1].m_byDisconnectDRI = byDRIId;
    return;
}

/*====================================================================
    ������      : GetDisconnectDRI
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId
    ����ֵ˵��  : u8 
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
====================================================================*/
u8 TConfMtTable::GetDisconnectDRI(u8 byMtId)
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return 0;
    }
    return m_atMtData[byMtId-1].m_byDisconnectDRI;
}

/*====================================================================
    ������      ��SetMtVidAlias
    ����        �������ն�һ����ƵԴ�ı���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն�Id
	              u8 byVidNum ��ƵԴ����
	              const TVidSrcAliasInfo *ptVidAliasInfo ��ƵԴ������Ϣ
    ����ֵ˵��  ���ɹ�TRUE,ʧ��FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	20110412    3.0         ���           create
====================================================================*/
BOOL32 TConfMtTable::SetMtVidAlias( u8 byMtId, u8 byVidNum, const TVidSrcAliasInfo *ptVidAliasInfo )
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return FALSE;
    }

	for( u8 byLoop = 0; byLoop < MT_MAXNUM_VIDSOURCE; byLoop++ )
	{
		m_atMtData[byMtId-1].m_tVidAliasInfo[byLoop].Clean();
	}

	memcpy( m_atMtData[byMtId-1].m_tVidAliasInfo, ptVidAliasInfo, \
		min(byVidNum, MT_MAXNUM_VIDSOURCE) * sizeof(TVidSrcAliasInfo) );
	return TRUE;
}

/*====================================================================
    ������      :InitMtVidAlias
    ����        ����ʼ���ն���ƵԴ������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն�Id
    ����ֵ˵��  ��void
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	20110412    3.0         ���           create
====================================================================*/
void TConfMtTable::InitMtVidAlias( u8 byMtId )
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return;
    }

	for( u8 byLoop = 0; byLoop < MT_MAXNUM_VIDSOURCE; byLoop++ )
	{
		m_atMtData[byMtId-1].m_tVidAliasInfo[byLoop].Clean();
	}
}

/*====================================================================
    ������      ��GetMtSelMtByMode
    ����        ������modeȡ�ն˵�ѡ��Դ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն�Id
				  u8 byMode ģʽ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	2012/09/13  4.7         zhangli       create
====================================================================*/
TMt TConfMtTable::GetMtSelMtByMode(u8 byMtId, u8 byMode)
{
	TMt tMt;
	if(byMtId == 0 || byMtId > MAXNUM_CONF_MT)
	{
		return tMt;
	}
	if (MODE_AUDIO != byMode && MODE_VIDEO != byMode && MODE_VIDEO2SECOND != byMode)
	{
		return tMt;
	}
	
	if (MODE_VIDEO2SECOND == byMode)
	{
		tMt = m_atMtData[byMtId-1].m_tSecSelSrc;
	}
	else if (MODE_AUDIO == byMode || MODE_VIDEO == byMode)
	{
		TMtStatus tMtStatus;
		if (GetMtStatus(byMtId, &tMtStatus))
		{
			tMt = tMtStatus.GetSelectMt(byMode);
		}
	}
	return tMt;
}

/*====================================================================
    ������      ��SetMtSelMtByMode
    ����        ������mode�����ն˵�ѡ��Դ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն�Id
				  u8 byMode ģʽ
				  u8 bySrcId Դ�ն�ID
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	2012/09/13  4.7         zhangli       create
====================================================================*/
void TConfMtTable::SetMtSelMtByMode(u8 byMtId, u8 byMode, TMt &tSrcMt)
{
	if(byMtId == 0 || byMtId > MAXNUM_CONF_MT)
	{
		return;
	}
	if (MODE_AUDIO != byMode && MODE_VIDEO != byMode && MODE_VIDEO2SECOND != byMode && MODE_BOTH != byMode)
	{
		return;
	}
	
	TMtStatus tMtStatus;
	if (!GetMtStatus(byMtId, &tMtStatus))
	{
		return;
	}
	
	if (MODE_VIDEO2SECOND == byMode)
	{
		m_atMtData[byMtId-1].m_tSecSelSrc = tSrcMt;
	}
	else
	{	
		tMtStatus.SetSelectMt(tSrcMt, byMode);
	}
	
	SetMtStatus(byMtId, &tMtStatus);
}

/*====================================================================
    ������      ��RemoveMtSelMtByMode
    ����        ������mode����ն˵�ѡ��Դ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն�Id
				  u8 byMode ģʽ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	2012/09/13  4.7         zhangli       create
====================================================================*/
void TConfMtTable::RemoveMtSelMtByMode(u8 byMtId, u8 byMode)
{
	if(byMtId == 0 || byMtId > MAXNUM_CONF_MT)
	{
		return;
	}
	if (MODE_AUDIO != byMode && MODE_VIDEO != byMode && MODE_VIDEO2SECOND != byMode && MODE_BOTH != byMode)
	{
		return;
	}
	
	TMtStatus tMtStatus;
	if (!GetMtStatus(byMtId, &tMtStatus))
	{
		return;
	}
	
	tMtStatus.RemoveSelByMcsMode(byMode);

	TMt tNullMt;
	
	if (MODE_VIDEO2SECOND == byMode)
	{
		m_atMtData[byMtId-1].m_tSecSelSrc = tNullMt;
	}
	else
	{	
		tMtStatus.SetSelectMt(tNullMt, byMode);
	}
	
	SetMtStatus(byMtId, &tMtStatus);
}
/*====================================================================
    ������      ��GetMtVidPortNum
    ����        ���õ��ն˵���ƵԴ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն�Id
    ����ֵ˵��  ���ն���ƵԴ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	20110412    3.0         ���           create
====================================================================*/
u8 TConfMtTable::GetMtVidPortNum( u8 byMtId )
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return 0;
    }

	u8 byMtVidPortNum = 0;
	for( u8 byLoop = 0; byLoop < MT_MAXNUM_VIDSOURCE; byLoop++ )
	{
		if( m_atMtData[byMtId-1].m_tVidAliasInfo[byLoop].byVidPortIdx != INVALID_MTVIDSRCPORTIDX )
		{
			byMtVidPortNum++;
		}
	}
	
	return byMtVidPortNum;
}

/*====================================================================
    ������      ��GetMtVidAliasbyVidIdx
    ����        ���õ��ն˵�һ����ƵԴ������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u8 byMtId �ն�Id
    ����ֵ˵��  ���ն���ƵԴ������Ϣ
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	20110412    3.0         ���           create
====================================================================*/
TVidSrcAliasInfo TConfMtTable::GetMtVidAliasbyVidIdx( u8 byMtId, u8 byIdx )
{
	if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT || byIdx >= MT_MAXNUM_VIDSOURCE )
    {
		TVidSrcAliasInfo tVidSrcAliasInfo;
        return tVidSrcAliasInfo;
    }

	return m_atMtData[byMtId-1].m_tVidAliasInfo[byIdx];
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
		LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[AddSwitchTable] invalid byMpId<%d> received!!!\n", byMpId);
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
		LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[RemoveSwitchTable] invalid byMpId<%d> received!!!\n", byMpId);
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

/*=============================================================================
�� �� ���� ClearRtcpSwitchTable
��    �ܣ� ���byMpIdת�����ϵ�RTCP����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byMpId
			u32 dwMpIp
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2011/08/30  4.6			zhangli                  ����
=============================================================================*/
void TConfSwitchTable::ClearRtcpSwitchTable(u8 byMpId, u32 dwMpIp)
{
	
	if (0 == byMpId || byMpId > MAXNUM_DRI || dwMpIp == 0)
    {
        return;
    }
	
	LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ClearSwitchTable]mp ip:%s \n", StrOfIP(dwMpIp));

	u8 byIdleSlot = 7;
	for (u16 wTableId = 0; wTableId < MAX_SWITCH_CHANNEL; ++wTableId)
	{
		if (m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].IsNull())
		{
			continue;
		}

		if(m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].GetRcvIP() == dwMpIp
			|| m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].GetSrcIp() == dwMpIp
			/*|| m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].GetDstIP() == dwMpIp
			|| m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].GetDisIp() == dwMpIp*/)
		{
			m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].SetNull();
		}
	}
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
    �������˵����byNum             ������ͨ������
                  ptSwitchChannel   �������ŵ�ָ��
				  u8 byAct          ��1-add brdsrc��2-remove brdsrc��11-add brddst
                  u8 byMpId         ��Ŀ��MpId
                  BOOL32 bForcely   ���Ƿ�ǿ�����ӻ�ɾ�����������Ӧ��ת���Ƿ����ߣ�һ������Ӧ�԰忨���ߵ��µ�ʱ������
    ����ֵ˵��  ���ɹ�TRUE,ʧ��FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	03/03/27    4.0         ����        ����
====================================================================*/
void TConfSwitchTable::ProcBrdSwitch( u8 byNum, TSwitchChannel *ptSwitchChannel, u8 byAct, u8 byMpId, BOOL32 bForcely )
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
				//zjj20100311 ��һ�����ӹ㲥Դ��ack�д��Ĺ㲥Դ�˿��ǶԵ�
				//����Ҫ���Ӷ�ش�(����Ϊ�ǵ��㲥),����ֻ����һ���㲥Դ�˿�,
				//�ں�������ӹ㲥Ŀ��˿�����mplib�е�bug�ع����Ķ˿ڲ���,
				//���������ȼ�һ�£��ں������ӹ㲥Ŀ��ack����ʱ�����������
				SetBrdSrcRcvPort( tSwitchChannel.GetRcvPort() );
                m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wTableId] = tSwitchChannel;
			}
            else
            {
                LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[ProcBrdSwitch] Add video broadcast source to switch table failed!\n");
            }
        }
        break;
        
    case 2://�Ƴ��㲥Դ
        {
            tSwitchChannel = *ptSwitchChannel;
            // ����ɾ���㲥Դ��Ϣ�ǹ㲥������ֻ�����MT�����MP�Ļ�Ӧ
			// �����ն˽��������в�������Ӱ������Ĵ������ [pengguofeng 1/19/2013]
//             u32 dwMpIp;
//             u32 dwSrcIp;
//             u16 wMpRecvPort;
//             if ( ! g_cMpManager.GetSwitchInfo(tSwitchChannel.GetSrcMt(), dwMpIp, wMpRecvPort, dwSrcIp) )
//             {
//                 return;
//             }
//             
//             if ( byMpId != g_cMcuVcApp.FindMp( dwMpIp ) &&
//                  !bForcely )
//             {
//                 break;
//             }
            
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
                // LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[ProcBrdSwitch] Remove video broadcast source to switch table failed!\n");
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
                    LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[ProcBrdSwitch] m_tSwitchTable Mp.%d full, insert failed!\n", byMpId);
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
	LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[IsValidSwitchSrcIp] dwSrcIp.%0x, Port.%d, Mode.%d!\n",
											 dwSrcIp, wRcvPort, byMode);

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
					LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[IsValidSwitchSrcIp] Table<MpId.%d, Idx.%d> Exists Dst<Ip.%0x, Port.%d>\n",
															 byMpId,
															 wTableIdx,
															 ptSwitchChnnl->GetDstIP(),
															 ptSwitchChnnl->GetDstPort());

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
                     ptSwitchChnnl->GetSrcIp() == dwSrcIp &&
					 ptSwitchChnnl->GetRcvPort() == wRcvPort
					 )
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
            
			//Դ��ת���˿ڲ�ͬ�����
            if ( ptSwitchChnnl->GetSrcIp() != dwSrcIp || ptSwitchChnnl->GetRcvPort() != wRcvPort )
            {
                continue;
            }

            if( (byMode == MODE_VIDEO && ptSwitchChnnl->GetRcvPort() % PORTSPAN == 0 ) ||
                (byMode == MODE_AUDIO && ptSwitchChnnl->GetRcvPort() % PORTSPAN == 2) )
            {
				//�ַ�ip��ת��ip���ǿ� + Ŀ�Ķ˿ڷǿ� + ���Ž��� ��ʶ�����Ž������������ܲ���
				if (ptSwitchChnnl->GetDisIp() != 0 &&
					ptSwitchChnnl->GetRcvIP() != 0 &&
					ptSwitchChnnl->GetDstPort() != 0 && 
					ptSwitchChnnl->GetRcvPort() != ptSwitchChnnl->GetDstPort())
				{
					LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[IsValidCommonSwitchSrcIp] Exists Bridge Switch!\n");
					return 	TRUE;
				}


//                 if (ptSwitchChnnl->GetRcvPort() != ptSwitchChnnl->GetDstPort() && 
//                     // �����Ƿ�ֹ�ѹ㲥Դ��MP�Ľ�������Ϊ����ͨ����, zgc, 2008-08-27
//                     ptSwitchChnnl->GetDstPort() != 0 )
//                 {
//                     return TRUE;
//                 }
//                 else if (g_cMcuVcApp.FindMp(ptSwitchChnnl->GetDstIP()) == 0 &&
//                          // �����Ƿ�ֹ�ѹ㲥Դ��MP�Ľ�������Ϊ����ͨ����, zgc, 2008-08-27
//                          ptSwitchChnnl->GetDstIP() != 0xffffffff )
//                 {
//                     return TRUE;
//                 }
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
// BOOL32 TConfSwitchTable::IsValidBroadSwitchSrcIp( u32 dwSrcIp, u16 wRcvPort, u8 byMode )
// {
//     u8 byMpId;
//     for (u8 byLoop = 1; byLoop <= MAXNUM_DRI; byLoop++)
//     {
//         byMpId = byLoop;
//         
//         // guzh [3/29/2007] 8/16�ڲ�����������Ľ�����Ŀ
//         if ( !g_cMcuVcApp.IsMpConnected(byMpId) &&
//             byMpId != MCU_BOARD_MPC &&
//             byMpId != MCU_BOARD_MPCD )
//             continue;
//         
// #ifdef _MINIMCU_
//         if (byLoop > 1)
//         {
//             byMpId = 2;
//         }
// #endif
//         for(u16 wTableIdx = 0; wTableIdx < MAX_SWITCH_CHANNEL; wTableIdx++)
//         {
//             TSwitchChannel *ptSwitchChnnl = &m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableIdx];
//             if (ptSwitchChnnl->IsNull())
//             {
//                 continue;
//             }
//             
//             if (ptSwitchChnnl->GetDstIP() == 0xffffffff && 
//                 ptSwitchChnnl->GetDstPort() == 0 &&
//                 byMode == MODE_VIDEO &&
//                 ptSwitchChnnl->GetSrcIp() == dwSrcIp )
//             {
//                 return TRUE;
//             }
//         }
//     }
//     
//     return FALSE;	
// }

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
    
	
	//�Ƿ�������telnet
	GetRegKeyInt(achFullName, "mcuNetWork", "EnableTelnet", 1, &nKeyVal);
	tMcuDebugVal.m_byTelnetEn = (u8)nKeyVal;
	
	if( !GetRegKeyInt( achFullName, "refreshInterval", "mcsRefreshInterval", 1, &nKeyVal ))
	{
		SetRegKeyInt( achFullName, "refreshInterval", "mcsRefreshInterval", nKeyVal );
	}

	// [9/28/2011 liuxu] ��ֹΪ0, С�ڵ���0����Ϊ1
	// ����Ϊ��λ
	tMcuDebugVal.m_dwMcsRefreshInterval = nKeyVal > 0 ? nKeyVal : 1;
	// ��ԭʼֵ��һ��, �ٴ�д���ļ�
	if (tMcuDebugVal.m_dwMcsRefreshInterval != (u32)nKeyVal)
	{
		SetRegKeyInt( achFullName, "refreshInterval", "mcsRefreshInterval", tMcuDebugVal.m_dwMcsRefreshInterval );
	}
	
	// [12/19/2011 liuxu] תΪ����
	tMcuDebugVal.m_dwMcsRefreshInterval *= 1000;
	
	// [11/23/2011 liuxu] �����������������ÿ���
	if( !GetRegKeyInt( achFullName, "refreshInterval", "EnableMcuMcsMtInfoBuf", 0, &nKeyVal ) )
	{
		SetRegKeyInt( achFullName, "refreshInterval", "EnableMcuMcsMtInfoBuf", 0 );
	}
	tMcuDebugVal.m_byEnableMcuMcsMtInfoBuf = nKeyVal > 0 ? 1 : 0;
	
	// �Ƿ�EnableMcuMcsMtStatusBuf
	if( !GetRegKeyInt( achFullName, "refreshInterval", "EnableMcuMcsMtStatusBuf", 1, &nKeyVal ) )
	{
		SetRegKeyInt( achFullName, "refreshInterval", "EnableMcuMcsMtStatusBuf", 1 );
	}
	tMcuDebugVal.m_byEnableMcuMcsMtStatusBuf = nKeyVal > 0 ? 1 : 0;
	
	// �Ƿ�EnableMcuMcuMtStatusBuf
	if( !GetRegKeyInt( achFullName, "refreshInterval", "EnableMcuMcuMtStatusBuf", 1, &nKeyVal ) )
	{
		SetRegKeyInt( achFullName, "refreshInterval", "EnableMcuMcuMtStatusBuf", 1 );
	}
	tMcuDebugVal.m_byEnableMcuMcuMtStatusBuf = nKeyVal > 0 ? 1 : 0;
	
	// �Ƿ�EnableMtAdpSMcuListBuf
	if( !GetRegKeyInt( achFullName, "refreshInterval", "EnableMtAdpSMcuListBuf", 0, &nKeyVal ) )
	{
		SetRegKeyInt( achFullName, "refreshInterval", "EnableMtAdpSMcuListBuf", 0 );
	}
	tMcuDebugVal.m_byEnableMtAdpSMcuListBuf = nKeyVal > 0 ? 1 : 0;
	
	
	if( !GetRegKeyInt( achFullName, "refreshInterval", "mtInfoRefreshInterval", 1000, &nKeyVal ))
	{
		SetRegKeyInt( achFullName, "refreshInterval", "mtInfoRefreshInterval", 1000 );
	}
	tMcuDebugVal.m_dwRefressMtInfoInterval = nKeyVal > 0 ? nKeyVal : 1000;
	
	if( !GetRegKeyInt( achFullName, "refreshInterval", "smcuMtRefreshInterval", 1000, &nKeyVal ))
	{
		SetRegKeyInt( achFullName, "refreshInterval", "smcuMtRefreshInterval", 1000 );
	}
	tMcuDebugVal.m_dwRefreshSMcuMtInterval = nKeyVal > 0 ? nKeyVal : 1000;	
	
	if( !GetRegKeyInt( achFullName, "refreshInterval", "mtAdpMtListRefreshInterval", 500, &nKeyVal ))
	{
		SetRegKeyInt( achFullName, "refreshInterval", "mtAdpMtListRefreshInterval", 500 );
	}
	tMcuDebugVal.m_dwRefreshMtAdpSMcuList = nKeyVal > 0 ? nKeyVal : 500;
	
	
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
	if( nKeyVal < (s32)MS_SYN_MAX_SPAN_TIMEOUT || nKeyVal > (s32)MS_SYN_MAX_SPAN_TIMEOUT*12 )
	{
		nKeyVal = (s32)MS_SYN_MAX_SPAN_TIMEOUT;
	}
	tMcuDebugVal.m_wMsSynTime = (u16)nKeyVal;
    
    GetRegKeyInt(achFullName, "BitrateAdapterParam", "BitrateScale", 0, &nKeyVal);    
    tMcuDebugVal.m_byBitrateScale = nKeyVal;
    
	//zjj20100205 ȡ�����Ƶ�mcucfg.ini��
    //GetRegKeyInt(achFullName, "cascadeParam", "IsMMcuSpeaker", 0, &nKeyVal);
    //tMcuDebugVal.m_byIsMMcuSpeaker = (0 != nKeyVal) ? 1 : 0;

    GetRegKeyInt(achFullName, "cascadeParam", "IsAutoDetectMMcuDupCallIn", 0, &nKeyVal);
    tMcuDebugVal.m_byIsAutoDetectMMcuDupCall = (0 != nKeyVal) ? 1 : 0;

    GetRegKeyInt(achFullName, "cascadeParam", "CascadeAlias", 0, &nKeyVal);
    tMcuDebugVal.m_byCascadeAliasType = (u8)nKeyVal;

	//[nizhijun 2011/02/14]ȡ�����Ƿ���ʾ�ϼ�MCU�����ն��б���mcucfg.ini��Ϊ׼
    //GetRegKeyInt(achFullName, "cascadeParam", "IsShowMMcuMtList", 1, &nKeyVal);
    //tMcuDebugVal.m_byShowMMcuMtList = (u8)nKeyVal;
    
	//[03/04/2010] zjl modify (�侯�������ϲ�)
	GetRegKeyInt(achFullName, "cascadeParam", "IsStartSwtichToMMcuWithOutCaseBas", 0, &nKeyVal); 
    tMcuDebugVal.m_byIsStartSwitchToMMcuWithoutCasBas = (u8)nKeyVal;

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
    
	//����֡������
	GetRegKeyInt(achFullName, "mcuGeneralParam", "ConfFPS", 0, &nKeyVal);
    tMcuDebugVal.m_byConfFPS = (u8)nKeyVal;
	
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


	//zhouyiliang 20100702 vcs����ĺ��г�ʱʱ�䣬��λ��,Ĭ��10s
	GetRegKeyInt(achFullName, "mcuGeneralParam", "VcsMtOverTimeInterval", DEFAULT_VCS_MTOVERTIME_INTERVAL, &nKeyVal);
    tMcuDebugVal.m_byVcsMtOverTimeInterval = (u8)nKeyVal;
	if( tMcuDebugVal.m_byVcsMtOverTimeInterval < DEFAULT_VCS_MTOVERTIME_INTERVAL )
    {
        tMcuDebugVal.m_byVcsMtOverTimeInterval = DEFAULT_VCS_MTOVERTIME_INTERVAL;
    }
	
	// [1/19/2011 xliang] ������ͨ����ʱ���
	if (GetRegKeyInt(achFullName, "mcuGeneralParam", "MmcuOLCTimerOut", TIMESPACE_WAIT_CASCADE_CHECK, &nKeyVal))
	{
		tMcuDebugVal.m_wMmcuOlcTimerOut = (u16)nKeyVal;
	}
	else
	{
		tMcuDebugVal.m_wMmcuOlcTimerOut = TIMESPACE_WAIT_CASCADE_CHECK;
	}

	//�Ƿ�֧�ִ���ͬ��
    GetRegKeyInt(achFullName, "mcuGeneralParam", "IsVASimultaneous ", 0, &nKeyVal);
    tMcuDebugVal.m_byIsVASimultaneous = (u8)nKeyVal;

    //���õ���������(Ĭ����4��,���1��)
	//zjj20100108������÷ϳ����Ƶ�mcucfg.ini
    /*GetRegKeyInt(achFullName, "mcuGeneralParam", "McuAdminLevel", 4, &nKeyVal);
    if (nKeyVal < 1)
    {
        nKeyVal = 1;
    }
    tMcuDebugVal.m_byMcuAdminLevel = nKeyVal;
	*/

	//MCU�Ƿ�����MP
#if defined(_MINIMCU_) || defined(WIN32)
    // guzh [4/2/2007] Windows ȱʡ����MP
    GetRegKeyInt(achFullName, "mcuAbilityParam", "IsMpcRunMp", 1, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byIsMpcRunMp = (u8)nKeyVal;	
#else
	GetRegKeyInt(achFullName, "mcuAbilityParam", "IsMpcRunMp", 0, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byIsMpcRunMp = (u8)nKeyVal;
#endif
	
	//MCU�Ƿ�����MtAdp, 8000B/C ��ͨ��debug����
#ifndef WIN32
	GetRegKeyInt(achFullName, "mcuAbilityParam", "IsMpcRunMtAdp", 0, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byIsMpcRunMtAdp = (u8)nKeyVal;
	LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[McuGetDebugKeyValue] tMcuDebugVal.m_tPerfLimit.m_byIsMpcRunMtAdp = %d\n",
		nKeyVal); // [12/30/2009 xliang] add print
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
    GetRegKeyInt(achFullName, "mcuAbilityParam", "IsSendFakeCap2Taide", 1, &nKeyVal);
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
	
    //�Ƿ�Լ����Ϊ�ı���MCU��2CIF����Ƶ������CIF
    //GetRegKeyInt(achFullName, "mcuAbilityParam", "IsSendFakeCap2HuaweiSDMcu", 0, &nKeyVal);
    //tMcuDebugVal.m_bIsSendFakeCap2HuaweiSDMcu = (u8)nKeyVal;

    // guzh [4/17/2007] ���ʱ��������
	//zjj20100108������÷ϳ�����Ϊmcs�������ʱ���������Ƶ�mcucfg.ini
    /*GetRegKeyInt( achFullName, "mcuMaxConnCap", "mcuMaxOngoingConfNum", MAXNUM_ONGO_CONF, &nKeyVal );

    tMcuDebugVal.m_byMaxOngoingConf = nKeyVal;
    if ( 0 == tMcuDebugVal.m_byMaxOngoingConf )
    {
        tMcuDebugVal.m_byMaxOngoingConf = MAXNUM_ONGO_CONF;
    }
    if ( tMcuDebugVal.m_byMaxOngoingConf > MAXNUM_ONGO_CONF )
    {
        tMcuDebugVal.m_byMaxOngoingConf = MAXNUM_ONGO_CONF;
    }*/

	GetRegKeyInt(achFullName, "mcuAbilityParam", "IsCanConfCapabilityCheat", 0, &nKeyVal);
	tMcuDebugVal.m_byIsCanConfCapabilityCheat = nKeyVal;


	//�Ƿ�������ն˻���������ƭ
	GetRegKeyInt(achFullName, "mcuAbilityParam", "IsCanConfCapabilityCheat", 0, &nKeyVal);
	tMcuDebugVal.m_byIsCanConfCapabilityCheat = nKeyVal;

	//����������ƭ���ն�������
	GetRegKeyInt(achFullName, "mcuAbilityParam", "MTBoardTypeNum ", 0, &nKeyVal);
	tMcuDebugVal.m_byConfCapCheatMtBoardTypeNum = nKeyVal;
	if( tMcuDebugVal.m_byConfCapCheatMtBoardTypeNum > MAXNUM_CONFCAPCHEATMTBOARDNUM )
	{
		tMcuDebugVal.m_byConfCapCheatMtBoardTypeNum = MAXNUM_CONFCAPCHEATMTBOARDNUM;
	}
 
	//���Ի���������ƭ���ն��ͺ�
	s8  achKeyName[20];
	u8 byBoardTypeIndex = 0;
	u8 byIndex = 0;
	for( byIndex = 0;byIndex < tMcuDebugVal.m_byConfCapCheatMtBoardTypeNum;byIndex++ )
	{
		sprintf(achKeyName, "MTBoardType%d", byIndex + 1 );
		GetRegKeyInt(achFullName, "mcuAbilityParam", achKeyName, MT_BOARD_UNKNOW, &nKeyVal);
		GetRegKeyString( achFullName,"mcuAbilityParam",achKeyName,"MT_BOARD_UNKNOW",(char*)&tMcuDebugVal.m_pachCheatMtBoardType[byBoardTypeIndex][0],MAXLEN_PRODUCTID);
		if( 0 != strcmp( "MT_BOARD_UNKNOW",(char*)tMcuDebugVal.m_pachCheatMtBoardType[byBoardTypeIndex] ) )
		{
			//strncpy( (char*),achKeyName,MAXLEN_PRODUCTID );
			tMcuDebugVal.m_pachCheatMtBoardType[byBoardTypeIndex][MAXLEN_PRODUCTID-1] = '\0';
		}		
		byBoardTypeIndex++;
	}

	//�����ն��ͺ�����
	GetRegKeyInt(achFullName, "mcuAbilityParam", "HDMTBoardTypeNum ", 0, &nKeyVal);
	tMcuDebugVal.m_byConfHDMtBoardTypeNum = nKeyVal;
	if( tMcuDebugVal.m_byConfHDMtBoardTypeNum > MAXNUM_CONFHDMTBOARDNUM )
	{
		tMcuDebugVal.m_byConfHDMtBoardTypeNum = MAXNUM_CONFHDMTBOARDNUM;
	}

	//�����ն��ͺŶ�ȡ
	byBoardTypeIndex = 0;
	for( byIndex = 0;byIndex < tMcuDebugVal.m_byConfHDMtBoardTypeNum;byIndex++ )
	{
		sprintf(achKeyName, "HDMTBoardType%d", byIndex + 1 );
		GetRegKeyInt(achFullName, "mcuAbilityParam", achKeyName, MT_BOARD_UNKNOW, &nKeyVal);
		GetRegKeyString( achFullName,"mcuAbilityParam",achKeyName,"MT_BOARD_UNKNOW",(char*)&tMcuDebugVal.m_pachHDMtBoardType[byBoardTypeIndex][0],MAXLEN_PRODUCTID);
		if( 0 != strcmp( "MT_BOARD_UNKNOW",(char*)tMcuDebugVal.m_pachHDMtBoardType[byBoardTypeIndex] ) )
		{
			//strncpy( (char*),achKeyName,MAXLEN_PRODUCTID );
			tMcuDebugVal.m_pachHDMtBoardType[byBoardTypeIndex][MAXLEN_PRODUCTID-1] = '\0';
		}		
		byBoardTypeIndex++;
	}
	
	GetRegKeyInt(achFullName, "mcuAbilityParam", "LowBrCalledMtChlOpenMode", 0, &nKeyVal);
	tMcuDebugVal.m_byLowBrCalledMtChlOpenMode = nKeyVal;

	//�Ƿ�֧�ָ����ն˺������ʽ��ն��߼�ͨ���ֱ���
    GetRegKeyInt(achFullName, "mcuAbilityParam", "IsSupportChgLGCResAcdBR", 0, &nKeyVal);
    tMcuDebugVal.m_byIsSupportChgLGCResAcdBR = (u8)nKeyVal;

	//1080P/60/50FPS Bp�����ʷֽ��debug֧�֣���λKbps��Ĭ��3072Kbps
    GetRegKeyInt(achFullName, "mcuAbilityParam", "1080P6050FPSBpLeastBitRate", 3072, &nKeyVal);
    tMcuDebugVal.m_w1080p6050FPSBpLeastBR = (u16)nKeyVal;
	
	//1080P/30/25FPS Bp�����ʷֽ��debug֧�֣���λKbps��Ĭ��2048Kbps
    GetRegKeyInt(achFullName, "mcuAbilityParam", "1080P3025FPSBpLeastBitRate", 2560, &nKeyVal);
    tMcuDebugVal.m_w1080p3025FPSBpLeastBR = (u16)nKeyVal;
	
	//720P BP�����ʷֽ��debug֧�֣���λKbps��Ĭ��1536Kbps
    GetRegKeyInt(achFullName, "mcuAbilityParam", "720P6050FpsBpLeastBitRate", 2560, &nKeyVal);
    tMcuDebugVal.m_w720P6050FpsBpLeastBR = (u16)nKeyVal;

	//720P 30/25 BP�����ʷֽ��debug֧�֣���λKbps��Ĭ��1024Kbps
    GetRegKeyInt(achFullName, "mcuAbilityParam", "720P3025FpsBpLeastBitRate", 1536, &nKeyVal);
    tMcuDebugVal.m_w720P3025FpsBpLeastBR = (u16)nKeyVal;

	//Cif �����ʷֽ��debug֧�֣���λKbps��Ĭ��384Kbps
    GetRegKeyInt(achFullName, "mcuAbilityParam", "CifLeastBitRate", 384, &nKeyVal);
    tMcuDebugVal.m_wCifLeastBR = (u16)nKeyVal;
	
    //4Cif �����ʷֽ��debug֧�֣���λKbps��Ĭ��1024Kbps
    GetRegKeyInt(achFullName, "mcuAbilityParam", "4CifLeastBitRate", 1024, &nKeyVal);
    tMcuDebugVal.m_w4CifLeastBR = (u16)nKeyVal;

	//1080P/60/50FPS Hp�����ʷֽ��debug֧�֣���λKbps��Ĭ��1536Kbps
    GetRegKeyInt(achFullName, "mcuAbilityParam", "1080P6050FPSHpLeastBitRate", 1536, &nKeyVal);
    tMcuDebugVal.m_w1080p6050FPSHpLeastBR = (u16)nKeyVal;
	
	//1080P/30/25FPS Hp�����ʷֽ��debug֧�֣���λKbps��Ĭ��1024Kbps
    GetRegKeyInt(achFullName, "mcuAbilityParam", "1080P3025FPSHpLeastBitRate", 1024, &nKeyVal);
    tMcuDebugVal.m_w1080p3025FPSHpLeastBR = (u16)nKeyVal;
	
	//720P/60/50Fps HP�����ʷֽ��debug֧�֣���λKbps��Ĭ��832Kbps
    GetRegKeyInt(achFullName, "mcuAbilityParam", "720P6050FpsHpLeastBitRate", 832, &nKeyVal);
    tMcuDebugVal.m_w720P6050FpsHpLeastBR = (u16)nKeyVal;

	//720P/30/25Fps HP�����ʷֽ��debug֧�֣���λKbps��Ĭ��512Kbps
    GetRegKeyInt(achFullName, "mcuAbilityParam", "720P3025FpsHpLeastBitRate", 512, &nKeyVal);
    tMcuDebugVal.m_w720P3025FpsHpLeastBR = (u16)nKeyVal;

	//�Ƿ�֧��������ϯ�Զ��������ƻ���
    GetRegKeyInt(achFullName, "mcuGeneralParam", "IsChairAutoSpecMix", 0, &nKeyVal);
    tMcuDebugVal.m_byIsChairAutoSpecMix = (0 != nKeyVal) ? 1 : 0; 

	//��ȡ��������ʱ�������
	GetRegKeyInt( achFullName, "mcuAbilityParam", "MixerWaitRspTimeout", 6,&nKeyVal );
	tMcuDebugVal.m_byWaitMixerRspTimeout = nKeyVal;

	//��ȡvcs�����쳣���������ӳٽ�����ʱ��
	GetRegKeyInt( achFullName, "mcuGeneralParam", "DelayReleaseConfTime", 30, &nKeyVal );
	tMcuDebugVal.m_byDelayReleaseConfTime = nKeyVal;

    GetRegKeyInt(achFullName, "mcuAbilityParam", "FpsAdp", 0, &nKeyVal);
    tMcuDebugVal.m_byFpsAdp = nKeyVal;

	GetRegKeyInt( achFullName, "mcuAbilityParam", "MtFastUpdateInterval", DEFAULT_MT_MCU_FASTUPDATE_INTERVAL,&nKeyVal );
	tMcuDebugVal.m_byMtFastUpdateInterval = nKeyVal;

	GetRegKeyInt( achFullName, "mcuAbilityParam", "MtFastUpdateNeglectNum", 0,&nKeyVal );
	tMcuDebugVal.m_byMtFastUpdateNeglectNum = nKeyVal;
	
	//��ȡMCU����������ؼ�֡������1-4�Σ�[4/6/2012 chendaiwei]
	if(!GetRegKeyInt( achFullName, "mcuAbilityParam", "FastUpdateToSpeakerNum", 4,&nKeyVal ) || nKeyVal <1 || nKeyVal >4)
	{
		nKeyVal = 4;
	}

	tMcuDebugVal.m_byFastUpdateToSpeakerNum = nKeyVal;

	GetRegKeyInt( achFullName, "mcuAbilityParam", "IsPolyComMtSendDoubleStream", 1,&nKeyVal );
	tMcuDebugVal.m_byIsPolyComMtSendDoubleStream = nKeyVal;
	
	GetRegKeyInt( achFullName, "mcuAbilityParam", "IsChaoRanMtAllowInVmpChnnl", 0,&nKeyVal );
	tMcuDebugVal.m_byIsChaoRanMtAllowInVmpChnnl = nKeyVal;

	GetRegKeyInt( achFullName, "mcuAbilityParam", "IsSendStaticText", 1,&nKeyVal );
	tMcuDebugVal.m_byIsSendStaticText = nKeyVal;
	
	
	//  ���׵�������[pengguofeng 7/24/2012]
	GetRegKeyInt(achFullName, "mcuCheatCap", "EntryNum", 0, &nKeyVal);
	if ( nKeyVal > MAXNUM_CHEATMTVIDEOCAP + MAXNUM_CHEATMTAUDIOCAP)//����20����Ƶ��100����Ƶ
	{
		nKeyVal = MAXNUM_CHEATMTVIDEOCAP + MAXNUM_CHEATMTAUDIOCAP;
	}
	tMcuDebugVal.m_byCheatCapNum = nKeyVal;
	tMcuDebugVal.m_byCheatVideoCapNum = 0;
	u8 byAudioCapIndx = 0;
	u8 abyCapIdxToRealIdx[MAXNUM_CHEATMTVIDEOCAP + MAXNUM_CHEATMTAUDIOCAP]; //capid�����������׵�����Ƶ���idx��Ӧ��ϵ
	memset(abyCapIdxToRealIdx,0xFF,sizeof(abyCapIdxToRealIdx));
	for ( byIndex = 0; byIndex < tMcuDebugVal.m_byCheatCapNum; byIndex++)
	{
		sprintf(achKeyName,"Entry%d",byIndex);
		//         achKeyName[5] = '0'+ byIndex;
		s8 achCapContent[100];               
		memset(achCapContent, 0, sizeof(achCapContent));
		
		if ( !GetRegKeyString(achFullName, "mcuCheatCap", achKeyName, 0, achCapContent, sizeof(achCapContent)) )
		{
			continue;
		}
		
		s8 achStore[6][6];
		memset(achStore, 0, sizeof(achStore));
		
		//�ָ�
		sscanf(achCapContent, "%s %s %s %s %s %s\n", achStore[0], achStore[1], achStore[2], achStore[3],achStore[4],achStore[5]);
		printf("[mcuCheatCap]Key:[%s]MediaType:%s RES:%s FPS:%s BR:%s HP:%s,IsSelectH239:%s\n", achKeyName,
			achStore[0], achStore[1], achStore[2], achStore[3],achStore[4],achStore[5]);
		
		u8 byMedaiType = atoi(achStore[0]);
		//����byMediaType�ֱ����videoCapSet����AudioCapSet����
		if ( tMcuDebugVal.IsAudioType(byMedaiType) )
		{
			if ( byAudioCapIndx >= MAXNUM_CHEATMTAUDIOCAP )//��Ƶ����20��������
			{
				continue;
			}
			u8 byAudioTrackNum = atoi(achStore[1]);
			if ( byAudioTrackNum == 0 ) //��������Ĭ�ϵ�����
			{
				byAudioTrackNum = 1;
			}
			
			tMcuDebugVal.m_tCheatAudioCapSet[byAudioCapIndx].SetAudioMediaType(byMedaiType);
			tMcuDebugVal.m_tCheatAudioCapSet[byAudioCapIndx].SetAudioTrackNum(byAudioTrackNum);
			abyCapIdxToRealIdx[byIndex] = byAudioCapIndx;
			byAudioCapIndx++;

		}
		else  //��Ƶ���߿յ�������
		{
			u8 byRes = atoi(achStore[1]);
			u8 byFps = atoi(achStore[2]);
			u16 wMaxBR = atoi(achStore[3]);
			emProfileAttrb eAttrb = (emProfileAttrb)atoi(achStore[4]);
			u8 bySelectH239 = atoi(achStore[5]);
			if ( byRes == 0 || byFps == 0  || tMcuDebugVal.m_byCheatVideoCapNum >= MAXNUM_CHEATMTVIDEOCAP )//��Ƶ����100��������
			{
				continue;
			}
		
			//����
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetMediaType(byMedaiType);
			if( MEDIA_TYPE_H264 == byMedaiType )
			{
				
				tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetUserDefFrameRate(byFps);
			}
			else
			{
				tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetFrameRate(byFps);
			}
			
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetResolution(byRes);
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetMaxBitRate(wMaxBR);
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetH264ProfileAttrb(eAttrb);
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetSupportH239( (BOOL32)bySelectH239 );
			abyCapIdxToRealIdx[byIndex] = tMcuDebugVal.m_byCheatVideoCapNum;
			tMcuDebugVal.m_byCheatVideoCapNum++;
		}

	
 	}
	// ���׵�IP��ַ�ͳ��� [pengguofeng 7/24/2012]
	GetRegKeyInt(achFullName, "mcuCheatMt", "EntryNum", 0, &nKeyVal);
	if ( nKeyVal > 100)
	{
		nKeyVal = 100;
	}
	tMcuDebugVal.m_byCheatMtNum = nKeyVal;
	
	//     s8 achKeyName[7];                    // ��ʱ�����Entry192, ��Cap192
	memset(achKeyName, 0, sizeof(achKeyName));
	for ( byIndex = 0; byIndex < tMcuDebugVal.m_byCheatMtNum; byIndex++ )
	{
		sprintf(achKeyName,"Entry%d",byIndex);
		s8 achCheatIpContent[200];                // ������󳤶�: 15λIP + tab + 3 + tab + 3 + \0
		memset(achCheatIpContent, 0, sizeof(achCheatIpContent));
		
		if ( !GetRegKeyString(achFullName, "mcuCheatMt", achKeyName, 0, achCheatIpContent, sizeof(achCheatIpContent)) )
		{
			continue;
		}
		
		s8 achStore[5][MAXLEN_PRODUCTID];
		memset(achStore, 0, sizeof(achStore));
		
		//�ָ�
		sscanf(achCheatIpContent, "%s %s %s %s %s\n", achStore[0], achStore[1], achStore[2], achStore[3],achStore[4]);
		printf("[mcuCheatIp]key:[%s]MtType:%s MtAlias:%s MainCapId:%s DualCapId:%s AudioCapId:%s\n", achKeyName,
			achStore[0], achStore[1], achStore[2], achStore[3],achStore[4]);
		
		u8 byMtType = atoi(achStore[0]);
		s8 *pszMtAlias = achStore[1];
		u8 byMainCapId = atoi(achStore[2]);  //ע�⣺��Ҫʹ����ĸ����ĸ����Ҳ��0;��Чֵ����255
		u8 byDualCapId = atoi(achStore[3]);  //ͬ��,���������-1��ʾ,ʵ�ʳ����Ӧ255
		u8 byAudioCapId = atoi(achStore[4]);
		if (  byMtType == 0 || (byMainCapId == 255 && byDualCapId == 255 && byAudioCapId == 255)) //�Ƿ�����2:  ����������˫��������ž�Ϊ255
		{
			continue;
		}
		
		//����
		tMcuDebugVal.m_tCheatMtParam[byIndex].m_byMtIdType = byMtType;
		//�����˶�Ӧ�������������Ƕ�Ӧ�������������⣨null��,������ָ���������m_tCheatVideoCapSet[MAXNUM_CHEATMTVIDEOCAP+1]
		//��m_tCheatAudioCapSet[MAXNUM_CHEATMTAUDIOCAP+1]�����һ�������һ������ֵ���϶�Ϊ��
		if (  byMainCapId != 255 && abyCapIdxToRealIdx[byMainCapId] == 255 )
		{
			abyCapIdxToRealIdx[byMainCapId] = MAXNUM_CHEATMTVIDEOCAP;
		}
		if (  byDualCapId != 255 && abyCapIdxToRealIdx[byDualCapId] == 255 )
		{
			abyCapIdxToRealIdx[byDualCapId] = MAXNUM_CHEATMTVIDEOCAP;
		}
		if (  byAudioCapId != 255 && abyCapIdxToRealIdx[byAudioCapId] == 255 )
		{
			abyCapIdxToRealIdx[byAudioCapId] = MAXNUM_CHEATMTAUDIOCAP;
		}
		if( byMainCapId < MAXNUM_CHEATMTVIDEOCAP + MAXNUM_CHEATMTAUDIOCAP )
		{
			tMcuDebugVal.m_tCheatMtParam[byIndex].m_byMainCapId = abyCapIdxToRealIdx[byMainCapId];
		}
		if( byDualCapId < MAXNUM_CHEATMTVIDEOCAP + MAXNUM_CHEATMTAUDIOCAP )
		{
			tMcuDebugVal.m_tCheatMtParam[byIndex].m_byDualCapId = abyCapIdxToRealIdx[byDualCapId];
		}
		if( byAudioCapId < MAXNUM_CHEATMTVIDEOCAP + MAXNUM_CHEATMTAUDIOCAP )
		{
			tMcuDebugVal.m_tCheatMtParam[byIndex].m_byAudioCapId = abyCapIdxToRealIdx[byAudioCapId];
		}		
		
		memcpy(tMcuDebugVal.m_tCheatMtParam[byIndex].m_aszMtAlias,pszMtAlias,sizeof(achStore[1]));
	}

	//����sp1_v4r6b2[8/21/2012 chendaiwei]
	GetRegKeyInt(achFullName, "mcuCheatIp", "EntryNum", 0, &nKeyVal);
	if ( nKeyVal > MAXNUM_CONF_MT)
	{
		nKeyVal = MAXNUM_CONF_MT;
	}

	u8 byCheatIpNum = (u8)nKeyVal;
	
	memset(achKeyName, 0, sizeof(achKeyName));
	for ( byIndex = 0; byIndex < tMcuDebugVal.m_byCheatMtNum; byIndex++ )
	{
		sprintf(achKeyName,"Entry%d",byIndex);
		s8 achCheatIpContent[100];			
		memset(achCheatIpContent, 0, sizeof(achCheatIpContent));
		
		if ( !GetRegKeyString(achFullName, "mcuCheatIp", achKeyName, 0, achCheatIpContent, sizeof(achCheatIpContent)) )
		{
			continue;
		}
		
		s8 achStore[3][MAXLEN_PRODUCTID];
		memset(achStore, 0, sizeof(achStore));
		sscanf(achCheatIpContent, "%s %s %s\n", achStore[0], achStore[1], achStore[2]);

		u8 byIdx = 0;
		for( ;byIdx < tMcuDebugVal.m_byCheatMtNum; byIdx++ )
		{
			if( tMcuDebugVal.m_tCheatMtParam[byIdx].m_byMtIdType == 1
				&& INET_ADDR(tMcuDebugVal.m_tCheatMtParam[byIdx].m_aszMtAlias)==  INET_ADDR(achStore[0]))
			{
				break;
			}
		}

		//Cheat�б���������ͬIP������[8/21/2012 chendaiwei]
		u8 byCapIdx = atoi(achStore[1]);
		if( byIdx != tMcuDebugVal.m_byCheatMtNum || byCapIdx == 'N' || tMcuDebugVal.m_byCheatVideoCapNum >= MAXNUM_CHEATMTVIDEOCAP)
		{
			continue;
		}
		else
		{
			memset(achKeyName, 0, sizeof(achKeyName));
			sprintf(achKeyName,"Cap%d",byCapIdx);
			s8 achCheatCapContent[100];			
			memset(achCheatCapContent, 0, sizeof(achCheatCapContent));
				
			if ( !GetRegKeyString(achFullName, "NonKedaCap", achKeyName, 0, achCheatCapContent, sizeof(achCheatCapContent)) )
			{
				continue;
			}

			tMcuDebugVal.m_tCheatMtParam[tMcuDebugVal.m_byCheatMtNum].m_byMtIdType = 1;
			tMcuDebugVal.m_tCheatMtParam[tMcuDebugVal.m_byCheatMtNum].m_byMainCapId = tMcuDebugVal.m_byCheatVideoCapNum;
			tMcuDebugVal.m_tCheatMtParam[tMcuDebugVal.m_byCheatMtNum].m_byDualCapId = 0xFF;
			memcpy(tMcuDebugVal.m_tCheatMtParam[tMcuDebugVal.m_byCheatMtNum].m_aszMtAlias,achStore[0],sizeof(achStore[0]));
			tMcuDebugVal.m_byCheatMtNum ++;

			s8 achCapStore[4][MAXLEN_PRODUCTID];
			memset(achCapStore, 0, sizeof(achCapStore));
			sscanf(achCheatCapContent, "%s %s %s %s\n", achCapStore[0], achCapStore[1], achCapStore[2],achCapStore[3]);
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetMediaType(atoi(achCapStore[0]));
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetResolution(atoi(achCapStore[1]));
			
			if( atoi(achCapStore[0]) == MEDIA_TYPE_H264 )
			{
				tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetUserDefFrameRate(atoi(achCapStore[2]));
			}
			else
			{
				tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetFrameRate(atoi(achCapStore[2]));
			}
			
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetMaxBitRate(atoi(achCapStore[3]));
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetH264ProfileAttrb(emBpAttrb);
			tMcuDebugVal.m_byCheatVideoCapNum++;//��Ƶ����+1
			tMcuDebugVal.m_byCheatCapNum++;
		}
	}

	//����Rlease_v4r6b2[8/21/2012 chendaiwei]
	GetRegKeyInt( achFullName, "mcuNoneKedaMtAbilityParam", "MtNum", 0,&nKeyVal );
	tMcuDebugVal.m_byNoneKedaMtAbilityCheatNum = nKeyVal;
	if( tMcuDebugVal.m_byNoneKedaMtAbilityCheatNum > MAXNUM_CONF_MT )
	{
		tMcuDebugVal.m_byNoneKedaMtAbilityCheatNum = MAXNUM_CONF_MT;
	}

	s8  achContent[16];
	for( byIndex = 0;byIndex < tMcuDebugVal.m_byNoneKedaMtAbilityCheatNum ;++byIndex )
	{
		memset( achContent,0,sizeof(achContent) );
		sprintf(achKeyName, "ip%d", byIndex + 1 );
		GetRegKeyString( achFullName,"mcuNoneKedaMtAbilityParam",achKeyName,"0",(char*)&achContent[0],16);
		achContent[sizeof(achContent)-1] = '\0';
		u8 byIdx = 0;
		for( ;byIdx < tMcuDebugVal.m_byCheatMtNum; byIdx++ )
		{
			if( tMcuDebugVal.m_tCheatMtParam[byIdx].m_byMtIdType == 1
				&& INET_ADDR(tMcuDebugVal.m_tCheatMtParam[byIdx].m_aszMtAlias)==  INET_ADDR(&achContent[0]))
			{
				break;
			}
		}

		//Cheat�б���������ͬIP������[8/21/2012 chendaiwei]
		if( byIdx != tMcuDebugVal.m_byCheatMtNum || tMcuDebugVal.m_byCheatVideoCapNum >= MAXNUM_CHEATMTVIDEOCAP )//��Ƶ����100��������
		{
			continue;
		}
		else
		{
			tMcuDebugVal.m_tCheatMtParam[tMcuDebugVal.m_byCheatMtNum].m_byMtIdType = 1;
			tMcuDebugVal.m_tCheatMtParam[tMcuDebugVal.m_byCheatMtNum].m_byMainCapId = tMcuDebugVal.m_byCheatVideoCapNum;
			tMcuDebugVal.m_tCheatMtParam[tMcuDebugVal.m_byCheatMtNum].m_byDualCapId = 0xFF;
			memcpy(tMcuDebugVal.m_tCheatMtParam[tMcuDebugVal.m_byCheatMtNum].m_aszMtAlias,achContent,sizeof(achContent));
			tMcuDebugVal.m_byCheatMtNum ++;

			//  [8/10/2011 chendaiwei]ȡ�ֱ���
			memset( achContent,0,sizeof(achContent) );
			sprintf(achKeyName, "resolution%d", byIndex + 1 );
			GetRegKeyString( achFullName,"mcuNoneKedaMtAbilityParam",achKeyName,"cif",(char*)&achContent[0],16);
			achContent[sizeof(achContent)-1] = '\0';
			u8 byResolution = VIDEO_FORMAT_INVALID;
			if( strcmp(achContent,"cif") == 0 )
			{
				byResolution = VIDEO_FORMAT_CIF;
			}
			else if( strcmp(achContent,"4cif") == 0 )
			{
				byResolution= VIDEO_FORMAT_4CIF;
			}
			else if( strcmp(achContent,"720") == 0 )
			{
				byResolution = VIDEO_FORMAT_HD720;
			}
			else if( strcmp(achContent,"1080") == 0 )
			{
				byResolution = VIDEO_FORMAT_HD1080;
			}

			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetResolution(byResolution);
			
			//  [8/10/2011 chendaiwei]ȡý���ʽ
			memset( achContent,0,sizeof(achContent) );
			sprintf(achKeyName, "mediatype%d", byIndex + 1 );
			GetRegKeyString( achFullName,"mcuNoneKedaMtAbilityParam",achKeyName,"H264",(char*)&achContent[0],16);
			achContent[sizeof(achContent)-1] = '\0';
			u8 byMediaType = MEDIA_TYPE_NULL;
			if( strcmp(achContent,"H264") == 0 )
			{
				byMediaType = MEDIA_TYPE_H264;
			}
			else if( strcmp(achContent,"H263") == 0 )
			{
				byMediaType = MEDIA_TYPE_H263;
			}
			else if( strcmp(achContent,"H261") == 0 )
			{
				byMediaType = MEDIA_TYPE_H261;
			}
			else if( strcmp(achContent,"MPEG4") == 0 )
			{
				byMediaType = MEDIA_TYPE_MP4;
			}
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetMediaType(byMediaType);
			
			//  [8/10/2011 chendaiwei]ȡ֡��
			sprintf(achKeyName, "fps%d", byIndex + 1 );
			GetRegKeyInt(achFullName, "mcuNoneKedaMtAbilityParam",achKeyName , 25, &nKeyVal);		

			if( byMediaType == MEDIA_TYPE_H264 )
			{
				tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetUserDefFrameRate((u8)nKeyVal);
			}
			else
			{
				tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetFrameRate((u8)nKeyVal);
			}
			
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetMaxBitRate(0);
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetH264ProfileAttrb(emBpAttrb);
			tMcuDebugVal.m_byCheatVideoCapNum++;
			tMcuDebugVal.m_byCheatCapNum++;
		}
	}

	//20120907 zhouyiliang ��Ҫ����Դip�Ͷ˿ڵ�Ŀ��ip,���100��
	GetRegKeyInt( achFullName, "mcuNeedMapIp", "EntryNum", 0,&nKeyVal );
	tMcuDebugVal.m_byNeedMapIpNum = min(nKeyVal,100);	
	memset(achKeyName, 0, sizeof(achKeyName));
	for (byIndex = 0 ; byIndex< tMcuDebugVal.m_byNeedMapIpNum;byIndex++)
	{
	
		sprintf(achKeyName,"Entry%d",byIndex);
		s8 achMapIpAddr[20];			
		memset(achMapIpAddr, 0, sizeof(achMapIpAddr));
		
		if ( !GetRegKeyString(achFullName, "mcuNeedMapIp", achKeyName, 0, achMapIpAddr, sizeof(achMapIpAddr)) )
		{
			continue;
		}
		tMcuDebugVal.m_adwNeedMappedIp[byIndex] = INET_ADDR(achMapIpAddr);
	}
	


    //�����鲥�����ն��б�
    GetRegKeyInt( achFullName, "mcuMultiCastMtTable", "EntryNum", 0, &nKeyVal );
    nKeyVal = min( nKeyVal, MAXNUM_MCU_MT );
    nKeyVal = max( nKeyVal, 0 );
    memset(achKeyName, 0, sizeof(achKeyName));
    for ( byIndex = 0; byIndex< nKeyVal; byIndex++ )
    {
        sprintf(achKeyName, "Entry%d", byIndex);
        s8 achMtIp[20] = {0};
        if( !GetRegKeyString( achFullName, "mcuMultiCastMtTable", achKeyName, "0.0.0.0", achMtIp, sizeof(achMtIp)) ||
            INET_ADDR(achMtIp) == 0)
        {
            continue;
        }
        
        tMcuDebugVal.m_adwMultiCastMtTable[tMcuDebugVal.m_byMultiCastMtTableSize] = INET_ADDR(achMtIp);
        tMcuDebugVal.m_byMultiCastMtTableSize++;
        
    }

    //��ȡ��ʱ����
    GetRegKeyInt( achFullName, "PinHoldConfig", "PinHoldInterval", 5, &nKeyVal );
    tMcuDebugVal.m_dwPinHoleInterval = nKeyVal;

	//�ǿƴﳧ������������[6/17/2013 chendaiwei]
	nKeyVal = 0;
	GetRegKeyInt( achFullName, "NoKedaMutePackConfig", "EntryNum", 0,&nKeyVal );

	tMcuDebugVal.m_byMutePackManuEntryNum = min(nKeyVal,20);	
	memset(achKeyName, 0, sizeof(achKeyName));
	for (byIndex = 0 ; byIndex< tMcuDebugVal.m_byMutePackManuEntryNum;byIndex++)
	{
		sprintf(achKeyName,"Entry%d",byIndex);
		s8 achSndSelfMutePackMt[140] = {0};	
		if ( !GetRegKeyString(achFullName, "NoKedaMutePackConfig", achKeyName, 0, achSndSelfMutePackMt, sizeof(achSndSelfMutePackMt)) )
		{
			continue;
		}
		s8 achSendSelfManuId[4] = {0};
		//productid�����м���пո������ļ�����|����ʾproductid�Ŀ�ʼ
		s8 achdelimChar[] = "|";
		s8 * pTok = strtok(achSndSelfMutePackMt,achdelimChar);
		if (NULL == pTok)
		{
			continue;
		}
		u8 byLen = strlen(pTok);
		memcpy(achSendSelfManuId,pTok,min(byLen,3));
		achSendSelfManuId[3] = '\0';
		pTok = strtok(NULL,achdelimChar);
		if (NULL == pTok)
		{
			continue;
		}
		
		tMcuDebugVal.m_atSndSelfMutePackMt[byIndex].m_byManuId = atoi(achSendSelfManuId);
		byLen = strlen(pTok);
		memcpy(tMcuDebugVal.m_atSndSelfMutePackMt[byIndex].m_achProductId ,pTok,min(byLen,MAXLEN_PRODUCTID));
	}

	nKeyVal = 0;
	GetRegKeyInt( achFullName, "NoKedaMutePackConfig", "MutePackSendInteral", 1,&nKeyVal );
	tMcuDebugVal.m_byMutePackSendInterval = (u8)nKeyVal;

	nKeyVal = 0;
	GetRegKeyInt( achFullName, "NoKedaMutePackConfig", "MutePackNum", 5,&nKeyVal );
	tMcuDebugVal.m_byMutePackNum = (u8)nKeyVal;
	
	//20131114 g7221.c��Ƶ��ʽ����������ż�Ի��ն�manuid��productid
	GetRegKeyInt( achFullName, "NoKedaReverseG7221cConfig", "EntryNum", 0,&nKeyVal );
	tMcuDebugVal.m_byReverseG7221cNum = min(nKeyVal,5);	
	memset(achKeyName, 0, sizeof(achKeyName));
	for (byIndex = 0 ; byIndex< tMcuDebugVal.m_byReverseG7221cNum;byIndex++)
	{
		sprintf(achKeyName,"Entry%d",byIndex);
		s8 achReverseG7221cMt[140] = {0};	
		if ( !GetRegKeyString(achFullName, "NoKedaReverseG7221cConfig", achKeyName, 0, achReverseG7221cMt, sizeof(achReverseG7221cMt)) )
		{
			continue;
		}
		s8 achReverseG7221cManuId[4] = {0};
		//productid�����м���пո������ļ�����|����ʾproductid�Ŀ�ʼ
		s8 achdelimChar[] = "|";
		s8 * pTok = strtok(achReverseG7221cMt,achdelimChar);
		if (NULL == pTok)
		{
			continue;
		}
		u8 byLen = strlen(pTok);
		memcpy(achReverseG7221cManuId,pTok,min(byLen,3));
		achReverseG7221cManuId[3] = '\0';
		pTok = strtok(NULL,achdelimChar);
		if (NULL == pTok)
		{
			continue;
		}
		
		tMcuDebugVal.m_atReverseG7221cMt[byIndex].m_byManuId = atoi(achReverseG7221cManuId);
		byLen = strlen(pTok);
		memcpy(tMcuDebugVal.m_atReverseG7221cMt[byIndex].m_achProductId ,pTok,min(byLen,MAXLEN_PRODUCTID));
	}

	//20130723 ��Ҫ�����ⷽʽ����Ĳ�Ʒ�ն�manuid��productid��medianet��Ի�Ϊ���� ���ñ�־λ�ӿ�
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	GetRegKeyInt( achFullName, "NoKedaRcvH264DependInMarkConfig", "EntryNum", 0,&nKeyVal );
	tMcuDebugVal.m_byRcvH264DependInMarkNum = min(nKeyVal,5);	
	memset(achKeyName, 0, sizeof(achKeyName));
	for (byIndex = 0 ; byIndex< tMcuDebugVal.m_byRcvH264DependInMarkNum;byIndex++)
	{
		sprintf(achKeyName,"Entry%d",byIndex);
		s8 achRcvH264DependInMarkMt[140] = {0};	
		if ( !GetRegKeyString(achFullName, "NoKedaRcvH264DependInMarkConfig", achKeyName, 0, achRcvH264DependInMarkMt, sizeof(achRcvH264DependInMarkMt)) )
		{
			continue;
		}
		s8 achRcvH264DependInMarkManuId[4] = {0};
		//productid�����м���пո������ļ�����|����ʾproductid�Ŀ�ʼ
		s8 achdelimChar[] = "|";
		s8 * pTok = strtok(achRcvH264DependInMarkMt,achdelimChar);
		if (NULL == pTok)
		{
			continue;
		}
		u8 byLen = strlen(pTok);
		memcpy(achRcvH264DependInMarkManuId,pTok,min(byLen,3));
		achRcvH264DependInMarkManuId[3] = '\0';
		pTok = strtok(NULL,achdelimChar);
		if (NULL == pTok)
		{
			continue;
		}
		
		tMcuDebugVal.m_atRcvH264DependInMarkMt[byIndex].m_byManuId = atoi(achRcvH264DependInMarkManuId);
		byLen = strlen(pTok);
		memcpy(tMcuDebugVal.m_atRcvH264DependInMarkMt[byIndex].m_achProductId ,pTok,min(byLen,MAXLEN_PRODUCTID));
	}
#endif

	return;
}

#define INNER_FREE_MEM(p, n)				\
	{									\
		for( u32 dwInnerLoop = 0; dwInnerLoop < n; dwInnerLoop++ )			\
		{															\
			if( p && NULL != p[dwInnerLoop] )			\
			{															\
				free( p[dwInnerLoop] );							\
				p[dwInnerLoop] = NULL;							\
			}															\
		}															\
		free( p );											\
		p = NULL;											\
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
    //s8   pszE164Start[MAXLEN_E164] = {0};
    //s8   pszE164End[MAXLEN_E164] = {0};
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
		// LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "McuGetMtCallInterfaceInfo Err while reading %s %s!\n", "MtCallInterfaceTable", "EntryNum" );
		return FALSE;
	}
	if( nEntryNum < 0 )
	{
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "McuGetMtCallInterfaceInfo Err2 while reading %s %s!\n", "MtCallInterfaceTable", "EntryNum" );
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
	
	// [7/25/2011 liuxu]
	if ( NULL == lpszTable )
	{
		printf("McuGetMtCallInterfaceInfo Err while malloc for lpszTable\n");
		return FALSE;
	}

	for( dwLoop = 0; dwLoop < (u32)dwEntryNum; dwLoop++ )
	{
		lpszTable[dwLoop] = (s8 *)malloc( MAX_VALUE_LEN+1 );
	}
	
	//get the table string
	bRet = GetRegKeyStringTable( achFullName, "MtCallInterfaceTable", "fail", lpszTable, &dwEntryNum, (MAX_VALUE_LEN+1) );
	if( FALSE == bRet ) 
	{
		printf( "McuGetMtCallInterfaceInfo Err while reading %s table!\n", "MtCallInterfaceTable" );

		// [7/25/2011 liuxu] �ͷ��ڴ�
		INNER_FREE_MEM(lpszTable, dwEntryNum);
		return bRet;
	}

	for( dwLoop = 0; dwLoop < dwEntryNum; dwLoop++ )
	{
        
        u32 dwStartNetIp = 0;
        u32 dwEndNetIp = 0;
        //u8  byAliasType = mtAliasTypeOthers;

		pchToken = NULL;
		if (lpszTable && lpszTable[dwValidNum])
		{
			pchToken = strtok( lpszTable[dwValidNum], chSeps );
		}		

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
                //byAliasType = mtAliasTypeTransportAddress;
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

	INNER_FREE_MEM(lpszTable, dwEntryNum);
    
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
	
	//return mtAliasTypeOthers;
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
	// [7/25/2011 liuxu]
	if ( !pszSrc || !pszDst)
	{
		return FALSE;
	}

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
			pszDstTmp++;
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

	//����ͷ��Ϣ_�洢�ļ��ı����ļ�������,������״δ���
	sprintf(achFullName, "%s/%s", DIR_DATA, CONFHEADINFO_BACKUP);
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
BOOL32 AddConfToFile( TConfStore &tConfStore, BOOL32 bDefaultConf,TConfStore *ptOldConfStore )
{
	//FILE *hHeadFile = NULL;
	//FILE *hConfFile = NULL;

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
	if (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE== byConfPos)
	{
		byConfPos = 0;
		while (!acConfId[byConfPos].IsNull() && byConfPos<(MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE))
		{
			byConfPos++;
		}
	}

	//����
	if (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE== byConfPos) 
	{
		return FALSE;
	}

	//2.1zjj20091102�Ƚ������������(��Щ��̬���������ݣ�����ֻ���vcs����ǽԤ��)�ó���
	u16 wExtraDataLen = 0;
	u8 *pbyExtraDataBuf = NULL; //Ԥ������[8/16/2012 chendaiwei]

	u16 wExInfoLen = 0;
	u8 *pbyExInfoBuf = NULL;//��չ��Ϣ����[8/16/2012 chendaiwei]

	if( NULL != ptOldConfStore )//�޸�ģ��
	{
		GetConfExtraDataLenFromFile( *ptOldConfStore,wExtraDataLen,wExInfoLen );							
	}
	else
	{
		GetConfExtraDataLenFromFile( tConfStore,wExtraDataLen,wExInfoLen );	
	}

	//Ԥ�����ݳ����п���Ϊ0�����������ݳ��ȵ�u16.
	//��չ�������ݳ�����СΪu16����Ϊ�������ݳ��ȵ�u16[8/16/2012 chendaiwei]

	pbyExtraDataBuf = new u8[wExtraDataLen+1];
	pbyExInfoBuf = new u8[wExInfoLen+1];

	BOOL32 bHasFix2PlanData = FALSE;
	if( NULL != pbyExtraDataBuf && NULL != pbyExInfoBuf )
	{
		if( NULL != ptOldConfStore )
		{
			GetConfExtraDataFromFile( *ptOldConfStore,pbyExtraDataBuf,wExtraDataLen,pbyExInfoBuf,wExInfoLen,bHasFix2PlanData );

		}
		else
		{
			GetConfExtraDataFromFile( tConfStore,pbyExtraDataBuf,wExtraDataLen,pbyExInfoBuf,wExInfoLen,bHasFix2PlanData );
		}				
	}

	u8* pbyActualExtraDataBuf = NULL;
	u16 wActualExtradataLen = 0;
	if(bHasFix2PlanData)
	{
		wActualExtradataLen = GetPlanDataLenByV4R6B2PlanData(pbyExtraDataBuf,wExtraDataLen);
		if( wActualExtradataLen == 0 )
		{
			SAFE_DEL_ARRAY(pbyExtraDataBuf);
			SAFE_DEL_ARRAY(pbyExInfoBuf);
			SAFE_DEL_ARRAY(pbyActualExtraDataBuf);
			LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_CFG,"[AddConfToFile]Conf.%s wActualExtradataLen==0, return!\n",tConfStore.m_tConfInfo.GetConfName());

			return FALSE;
		}

		pbyActualExtraDataBuf = new u8[ wActualExtradataLen ];
		if (!pbyActualExtraDataBuf)
		{
			SAFE_DEL_ARRAY(pbyExtraDataBuf);
			SAFE_DEL_ARRAY(pbyExInfoBuf);
			SAFE_DEL_ARRAY(pbyActualExtraDataBuf);

			return FALSE;
		}
		
		TransferV4R6B2planDataToV4R7(pbyExtraDataBuf,wExtraDataLen,pbyActualExtraDataBuf,wActualExtradataLen);
		
		LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_CFG,"[AddConfToFile]Conf.%s v4r6b2 plan Data transfer from len.%d to len.%d\n",
			tConfStore.m_tConfInfo.GetConfName(),wExtraDataLen,wActualExtradataLen);

	}
	else
	{
		pbyActualExtraDataBuf = new u8[ wExtraDataLen+1 ];
		if (!pbyActualExtraDataBuf)
		{
			SAFE_DEL_ARRAY(pbyExtraDataBuf);
			SAFE_DEL_ARRAY(pbyExInfoBuf);
			SAFE_DEL_ARRAY(pbyActualExtraDataBuf);

			return FALSE;
		}
		
		if(pbyActualExtraDataBuf!=NULL && pbyExtraDataBuf!=NULL)
		{
			memcpy(pbyActualExtraDataBuf,pbyExtraDataBuf,wExtraDataLen);
			wActualExtradataLen = wExtraDataLen;
		}
	}
				
	//////////////////////////////////////////////////////////////////////////
	

	//2.2�� δ����Pack����Ļ������� ����Pack����
	if (FALSE == PackConfStore(tConfStore, ptPackConfStore, wPackConfDataLen))
	{
		SAFE_DEL_ARRAY(pbyExtraDataBuf);
		SAFE_DEL_ARRAY(pbyExInfoBuf);
		SAFE_DEL_ARRAY(pbyActualExtraDataBuf);

		return FALSE;
	}

	//3.1���� �û����ģ����Ϣ ��ָ�������Ĵ洢�ļ�
	cConfConfId = ptPackConfStore->m_tConfInfo.GetConfId();
	if (FALSE == SetConfDataToFile(byConfPos, cConfConfId, (u8*)ptPackConfStore, wPackConfDataLen))
	{
		SAFE_DEL_ARRAY(pbyExtraDataBuf);
		SAFE_DEL_ARRAY(pbyExInfoBuf);
		SAFE_DEL_ARRAY(pbyActualExtraDataBuf);

		return FALSE;
	}


	//////////////////////////////////////////////////////////////////////////
	
	
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
		//ȱʡ�����¼��־λ[7/17/2012 chendaiwei]
		TConfAttrbEx tConfAttrbEx = ptPackConfStore->m_tConfInfo.GetConfAttrbEx();
		tConfAttrbEx.SetDefaultConfFlag(TRUE);
		ptPackConfStore->m_tConfInfo.SetConfAttrbEx(tConfAttrbEx);

		//������ȱʡʧ��
		if (SetConfDataToFile((MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE), 
			                  cDefConfConfId, (u8*)ptPackConfStore, wPackConfDataLen))
		{
			acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE] = cDefConfConfId;
		}
		else
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"save default conf failed!\n");
		}
	}
	
	//5.���±��� ˢ�º�����л����ģ��ͷ��Ϣ
	if (FALSE == SetAllConfHeadToFile(acConfId, sizeof(acConfId)))
	{
		SAFE_DEL_ARRAY(pbyExtraDataBuf);
		SAFE_DEL_ARRAY(pbyExInfoBuf);
		SAFE_DEL_ARRAY(pbyActualExtraDataBuf);

		return FALSE;
	}

	//6.2zjj20091102 �������Ķ�����Ϣ(��Щ��̬���������ݣ�����ֻ���vcs����ǽԤ��)
	if( NULL != pbyActualExtraDataBuf && NULL != pbyExInfoBuf)
	{
		SaveConfExtraPlanDataToFile( tConfStore, pbyActualExtraDataBuf,wActualExtradataLen );

		if (!cDefConfConfId.IsNull())
		{
			//ȱʡ���鱣��Ԥ���͹�ѡ��Ϣ[5/28/2013 chendaiwei]
			SaveConfExtraPlanDataToFile( tConfStore, pbyActualExtraDataBuf,wActualExtradataLen,TRUE );
		}
	}

	SAFE_DEL_ARRAY(pbyExtraDataBuf);
	SAFE_DEL_ARRAY(pbyExInfoBuf);
	SAFE_DEL_ARRAY(pbyActualExtraDataBuf);
	
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
	BOOL32 bRet = FALSE;
	if (NULL != hHeadFile)
	{
		bRet = TRUE;
        fread((s8*)pacConfId, dwOutBufLen, 1, hHeadFile);				
        fclose(hHeadFile);
	}

    EndRWConfInfoFile();

	return bRet;
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
	BOOL32 bRet = FALSE;
    if (NULL != hHeadFile)
    {
		bRet = TRUE;
        fwrite((s8*)pacConfId, dwOutBufLen, 1, hHeadFile);
        fclose(hHeadFile);        
    }
	
    EndRWConfInfoFile();
	
	return bRet;
}

/*=============================================================================
  �� �� ���� BackConfHeadInfo
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void BackConfHeadInfo( void )
{
	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
	if (GetAllConfHeadFromFile(acConfId, sizeof(acConfId)))
	{
		FILE *hHeadFile = NULL;
		s8    achFullName[64];

		sprintf(achFullName, "%s/%s", DIR_DATA, CONFHEADINFO_BACKUP);
		// guzh [4/3/2007] ȱʡ���鵥������,����ֱ�Ӹ��Ǽ���
		if (!BeginRWConfInfoFile())
		{
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[BackConfHeadInfo] BeginRWConfInfoFile fail\n");
			return;
		}
		hHeadFile = fopen(achFullName, "wb");
		if (NULL != hHeadFile)
		{
			fwrite((s8*)acConfId, sizeof(acConfId), 1, hHeadFile);
			fclose(hHeadFile);        
		}
		else
		{
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[BackConfHeadInfo] open confback fail\n");
		}
		EndRWConfInfoFile();
		return;
	}

	LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[BackConfHeadInfo]GetAllConfHeadFromFile fail\n");
	return;	

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
			   [BOOL32] bReadAllData TRUE��ȡ�������ݣ�����Ԥ�������Է����������ݵ�����FALSE��ȡ��Ԥ�����������
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/12/27  4.0			����                  ����
    07/04/03    4.0         ����                  �޸�
=============================================================================*/
BOOL32 GetConfDataFromFile(u8 byIndex, CConfId cConfId, u8 *pbyBuf, u32 dwInBufLen, u16 &wOutBufLen, BOOL32 bReadAllData)
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
	u32 dwSpace = 0;

	fread( achConfFileHead, strlen(CONFFILEHEAD), 1, hConfFile );
	//1)V4R6�ļ�ͷ  [11/19/2011 chendaiwei]
	if( 0 == memcmp( achConfFileHead, CONFFILEHEAD, strlen(CONFFILEHEAD) ) )
	{
		fread( &wFileTConfInfoSize, sizeof(u16), 1, hConfFile );
		wFileTConfInfoSize = ntohs(wFileTConfInfoSize);
		fread( &dwSpace, sizeof(u32), 1, hConfFile );
		wOutBufLen = wOutBufLen - strlen(CONFFILEHEAD) - sizeof(u16) - sizeof(u32);

		// �ڴ汣��
		if ( dwInBufLen < (u32)wOutBufLen || 0 == wFileTConfInfoSize)
		{
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetConfDataFromFile] The buf length is unenough!\n" );
			EndRWConfInfoFile();
			fclose(hConfFile);
			return FALSE;
		}

		if( bReadAllData )
		{
			fread(pbyBuf,wOutBufLen,1,hConfFile);
		}
		else
		{

			//���� ��ͨ + Ԥ�� + ��չ������Ԥ�����֣���ȡ������Ϣ ��ͨ+��չ [8/16/2012 chendaiwei]
			TConfStore tTempConfStore;
			TConfStore tUnPackConfstore;
			u16 wRemainFileLenth = (u16)nFileLen;	
			
			u16 wTempBufLen = min(wOutBufLen,sizeof(TConfStore));
			fread((u8*)&tTempConfStore,wTempBufLen,1,hConfFile);
			
			TPackConfStore *ptTempPackConfstore = (TPackConfStore *)&tTempConfStore;
			u16 wUnPackConfDataLen = 0;
			UnPackConfStore(ptTempPackConfstore,tUnPackConfstore,wUnPackConfDataLen,FALSE);
			//wUnPackConfDataLen -= 1;
			fseek(hConfFile,strlen(CONFFILEHEAD) + sizeof(u16) + sizeof(u32),SEEK_SET);
			wRemainFileLenth = wRemainFileLenth - strlen(CONFFILEHEAD) - sizeof(u16) - sizeof(u32);

			BOOL32 bIsFix2or8000HTemplate = FALSE;
			u8 byVcAutoModeExtraLen = 0;
			if(IsV4R6B2VcsTemplate(hConfFile,(u16)nFileLen))
			{
				bIsFix2or8000HTemplate= TRUE;
				byVcAutoModeExtraLen = 1;//������pbyBufӦ�ð���m_byVCAutoMode��Ϣ[11/13/2012 chendaiwei]
				wUnPackConfDataLen--;
			}
			
			if( wRemainFileLenth >= wUnPackConfDataLen )
			{
				fread(pbyBuf,wUnPackConfDataLen,1,hConfFile);
				wRemainFileLenth -= wUnPackConfDataLen;

				if(bIsFix2or8000HTemplate)
				{
					pbyBuf[wUnPackConfDataLen] = 0;
				}
			}
			else
			{
				LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetConfDataFromFile] conf.%s wUnPackConfDataLen.%d >wRemainFileLenth.%d,error!\n",tTempConfStore.m_tConfInfo.GetConfName(),wUnPackConfDataLen,wRemainFileLenth );
				EndRWConfInfoFile();
				fclose(hConfFile);
				return FALSE;
			}


			BOOL32 bIgnoreUnpack = FALSE; //��ʶ�Ƿ���������[8/17/2012 chendaiwei]
			u16 wPlanLen = 0;
			if( wRemainFileLenth == 0)
			{
				LogPrint( LOG_LVL_WARNING, MID_PUB_ALWAYS, "[GetConfDataFromFile]conf.%s wRemainFileLenth == 0, it must v4r6 template without plan!\n",tTempConfStore.m_tConfInfo.GetConfName());
				*(u16*)(pbyBuf+wUnPackConfDataLen+byVcAutoModeExtraLen) = 0;

				bIgnoreUnpack = TRUE;
			}
			else if( wRemainFileLenth >= sizeof(u16))
			{
				fread(&wPlanLen,sizeof(u16),1,hConfFile);
				wRemainFileLenth -= sizeof(u16);
			}
			else
			{
				LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetConfDataFromFile] conf.%s wRemainFileLenth.%d < sizeof(u16) plan length,error!\n",tTempConfStore.m_tConfInfo.GetConfName(),wRemainFileLenth );
				EndRWConfInfoFile();
				fclose(hConfFile);
				return FALSE;
			}

			if (!bIgnoreUnpack)
			{
				wPlanLen = ntohs(wPlanLen);
				if(wRemainFileLenth >= wPlanLen)
				{
					fseek(hConfFile,wPlanLen,SEEK_CUR);
					wRemainFileLenth -= wPlanLen;
				}
				else
				{
					LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetConfDataFromFile] conf.%s wPlanLen.%d > wRemainFileLenth.%d,error!\n",tTempConfStore.m_tConfInfo.GetConfName(),wPlanLen,wRemainFileLenth );
					EndRWConfInfoFile();
					fclose(hConfFile);
					return FALSE;
				}

				u16 wExInfoLen = 0;
				if( wRemainFileLenth == 0)
				{
					LogPrint( LOG_LVL_WARNING, MID_PUB_ALWAYS, "[GetConfDataFromFile]conf.%s wRemainFileLenth == 0, it must v4r6 template which has plan without Ex-info!\n",tTempConfStore.m_tConfInfo.GetConfName());
				}
				else if(wRemainFileLenth >= sizeof(u16))
				{
					fread(&wExInfoLen,sizeof(u16),1,hConfFile);
					wRemainFileLenth -= sizeof(u16);
				}
				else
				{
					LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetConfDataFromFile]cons.%s wRemainFileLenth.%d < sizeof(u16) ex-info len,error!\n",tTempConfStore.m_tConfInfo.GetConfName(),wRemainFileLenth );
					EndRWConfInfoFile();
					fclose(hConfFile);
					return FALSE;
				}
				
				*(u16*)(pbyBuf+wUnPackConfDataLen+byVcAutoModeExtraLen) = wExInfoLen;
				wExInfoLen = ntohs(wExInfoLen);
				
				if(wRemainFileLenth >= wExInfoLen)
				{
					fread(pbyBuf+wUnPackConfDataLen+sizeof(u16),wExInfoLen,1,hConfFile);
					wRemainFileLenth -= wExInfoLen;
				}
				else
				{
					LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetConfDataFromFile]conf.%s wRemainFileLenth.%d < wExInfoLen.%d,error!\n",tTempConfStore.m_tConfInfo.GetConfName(),wRemainFileLenth,wExInfoLen );
					EndRWConfInfoFile();
					fclose(hConfFile);
					return FALSE;
				}
			}
		}

		fclose(hConfFile);
		hConfFile = NULL;
	}
	//2)�ϰ汾
	else
	{
		fseek(hConfFile, 0, SEEK_SET);
		
		// �ڴ汣��
		if(dwInBufLen < wOutBufLen )
		{
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetConfDataFromFile] The input buf length(%d) is less than output buf(%d)!\n", dwInBufLen, wOutBufLen );
			fclose(hConfFile);
			EndRWConfInfoFile();
			hConfFile = NULL;
			return FALSE;
		}

		//��������Ϣ
		wOutBufLen = ( wOutBufLen <= sizeof(TPackConfStore) ) ? wOutBufLen : sizeof(TPackConfStore);
		fread(pbyBuf, wOutBufLen, 1, hConfFile);
		fclose(hConfFile);
		hConfFile = NULL;
		TPackConfStore *ptTemp = (TPackConfStore *)pbyBuf;
		ptTemp->m_byMtNum = 0;
		ptTemp->m_wAliasBufLen = 0;
	}

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
BOOL32 SetConfDataToFile(u8 byIndex, CConfId &cConfId, u8 *pbyBuf, u16 wInBufLen,BOOL32 bWriteAllData)
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
    if (GetConfDataFromFile(byIndex, cConfId, (u8*)&tStore, sizeof(tStore), wOutLen,bWriteAllData))
    {
		u16 wcompareLen = max(wInBufLen,wOutLen);
        if ( memcmp(&tStore, pbyBuf, wcompareLen) == 0)
        {
            // ������ͬ������Ҫ�ٴα���
            return TRUE;
        }
		else
		{
			LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[GetConfDataFromFile tick.%d] diff from file,so need to restore\n", OspTickGet());
		}
    }

    if ( !BeginRWConfInfoFile() )
    {
        return FALSE;                
    }

    FILE *hConfFile = NULL;
    s8    achFullName[64];	
    sprintf(achFullName, "%s/%s%d%s", DIR_DATA, CONFINFOFILENAME_PREFIX, byIndex, CONFINFOFILENAME_POSTFIX);

	LogPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[SetConfDataToFile tick.%d] save conf to file:%s\n", OspTickGet(), achFullName);

	hConfFile = fopen(achFullName, "wb");
	BOOL32 bRet = FALSE;
	if (NULL != hConfFile)
	{
		bRet = TRUE;
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

	return bRet;
}
/*=============================================================================
    �� �� ���� GetUnProConfDataToFileLen
    ��    �ܣ� unprocconfinfo_confindex.dat�ļ��ж�ȡĳһ����ģ�岻��Ҫ����������ܳ�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]   u8  byIndex
    �� �� ֵ�� u32
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		������		�߶���    
    08/11/26                ���㻪
=============================================================================*/
u32 GetUnProConfDataToFileLen(u8 byIndex)
{
	//�ɱ���ȱʡ������Ϣ
	if (byIndex > MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE)
	{
		return 0;
	}
	
    if ( !BeginRWConfInfoFile() )
    {
        return 0;                
    }

	u32 dwFileLen = 0;

    FILE *hConfFile = NULL;
    s8    achFullName[64];	
    sprintf(achFullName, "%s/%s%d%s", DIR_DATA, UNPROCCONFINFOFILENAME_PREFIX, byIndex, CONFINFOFILENAME_POSTFIX);

	hConfFile = fopen(achFullName, "rb");
	if (NULL != hConfFile)
	{
		fseek(hConfFile, 0, SEEK_END);
		dwFileLen = ftell(hConfFile);
	    fclose(hConfFile);
	}
	EndRWConfInfoFile();

	return dwFileLen;
}
/*=============================================================================
    �� �� ���� GetUnProConfDataToFile
    ��    �ܣ� unprocconfinfo_confindex.dat�ļ��ж�ȡĳһ����ģ�岻��Ҫ���������
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]   u8  byIndex
               [IN/OUT]   u8 *pbyBuf mcu����Ҫ����Ļ���ģ�������Ϣ(�������Ϣ)
               [IN/OUT]   u32 dwOutBufLen ������,������Ҫ��ȡ�ĳ���;����ʵ�ʶ�ȡ�ĳ���
			   [IN]       u32 dwBeginPos  ��ȡ����ʼλ

    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		������		�߶���    
    08/11/26                ���㻪
=============================================================================*/
BOOL32 GetUnProConfDataToFile(u8 byIndex, s8 *pbyBuf, u32& dwOutBufLen, u32 dwBeginPos)
{
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
	BOOL32 bRet = FALSE;
	if (NULL != hConfFile)
	{
		bRet = TRUE;
		fseek(hConfFile, 0, SEEK_END);
		u32 dwTotalLen = ftell(hConfFile);
		fseek(hConfFile, dwBeginPos, SEEK_SET);

		if (dwBeginPos > dwTotalLen)
		{
			dwOutBufLen = 0;
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetUnProConfDataToFile]beginpos(%d) bigger than totallen(%d)\n",
				      dwBeginPos, dwTotalLen);
		}
		else
		{
			dwOutBufLen = min(dwOutBufLen, dwTotalLen - dwBeginPos);
		}

		fread(pbyBuf, dwOutBufLen, 1, hConfFile);

// 		if (!wBeginPos)
// 		{
// 			u16 dwLen = *(u16*)pbyBuf;
// 			dwLen = htons(dwLen);
// 			memcpy(pbyBuf, &dwLen, sizeof(u16));
// 		}
	    fclose(hConfFile);
	}

    EndRWConfInfoFile();

	return bRet;
}

/*=============================================================================
    �� �� ���� SetUnProConfDataToFile
    ��    �ܣ� ��ĳһ����ģ�岻��Ҫ���������ֱ�Ӵ洢����Ӧ��unprocconfinfo_confindex.dat�ļ���
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]   u8  byIndex
               [IN]   s8 *pbyBuf mcu����Ҫ����Ļ���ģ�������Ϣ(�������Ϣ)
               [IN]   u32 dwInBufLen ������
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		������		�߶���    
    08/11/26                ���㻪
=============================================================================*/
BOOL32 SetUnProConfDataToFile(u8 byIndex, s8 *pbyBuf, u32 dwInBufLen, u8 byOverWrite)
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

	if (byOverWrite)
	{
		hConfFile = fopen(achFullName, "wb");
	}
	else
	{
		hConfFile = fopen(achFullName, "a+b");
	}

	BOOL32 bRet = FALSE;

	if (NULL != hConfFile)
	{
		bRet = TRUE;
        fwrite( pbyBuf, dwInBufLen, 1, hConfFile);
	    fclose(hConfFile);
	}	
	
    EndRWConfInfoFile();

	return bRet;
}

/*=============================================================================
    �� �� ���� UnPackConfInfoEx
    ��    �ܣ� Un pack���ݣ��洢��TConfInfoEx��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN][OUT]  TConfInfoEx &tConfInfoEx ����unpack�������
               [IN] const u8 *pbyBuf ��unpack������
               [IN][OUT] u16 &wUnPackConfExLen unPack������ݻ��峤�� ������
			   [IN][OUT] bool32 &bExistUnknowInfo ���ڱ�MCU�޷���������Ϣ����ΪTRUE
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2011/12/29  4.0			chendaiwei                  ����
=============================================================================*/
void   UnPackConfInfoEx(TConfInfoEx & tConfInfoEx, const u8 *pbyInBuf,
						u16 &wUnPackConfExLen, BOOL32 &bExistUnknowInfo,
						THduVmpModuleOrTHDTvWall *ptHduVmpModule ,
						TVmpModuleInfo *ptVmpModule,
						u8 *pbyEncoding)
{
	//��չ��Ϣ����  u16��չ��Ϣ�ܳ���+ u8��Ϣ����A + u16��Ϣ����A�ֽ��� + ��Ϣ����A���ݣ�ǰ���ʶ�ֽ�����С��+ u16��Ϣ����B......[12/29/2011 chendaiwei]
	u16 wConfExSize = ntohs( *(u16*)pbyInBuf);
	wUnPackConfExLen = wConfExSize + sizeof(u16);
	pbyInBuf = pbyInBuf + sizeof(u16);
	bExistUnknowInfo = FALSE;

	s32 nConfExSize = wConfExSize;

	THduVmpChannlInfo m_atHduVmpChnInfo[(HDUID_MAX-HDUID_MIN+1)*MAXNUM_HDU_CHANNEL];
	memset(&m_atHduVmpChnInfo[0],0,sizeof(m_atHduVmpChnInfo));
	u8 byHduVmpChnlCount = 0;

	// lang [pengguofeng 4/12/2013]
	if ( pbyEncoding != NULL )
	{
		*pbyEncoding = emenCoding_GBK; //Ĭ����GBK����
	}

	while( nConfExSize > 0)
	{
		u16 wLength = 0;
		emCommunicateType etype = (emCommunicateType)(*pbyInBuf);
		pbyInBuf ++;
		
		switch(etype)
		{
		case emEncodingType:
			// ����Ϊ�գ�����������������Ҫ����ƫ�� [pengguofeng 5/21/2013]
// 			if ( pbyEncoding != NULL )
			{
				wLength = ntohs(*(u16*)pbyInBuf); //����
				pbyInBuf = pbyInBuf + sizeof(u16);
				LogPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[UnPackConfInfoEx]emEncording length:%d\n", wLength);

				if ( pbyEncoding != NULL ) //����
				{
					memcpy(pbyEncoding, pbyInBuf, wLength);
				}
				pbyInBuf = pbyInBuf + wLength;
			}			
			break;
		case emMainStreamCapEx:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			memcpy(&tConfInfoEx,pbyInBuf,wLength);
			pbyInBuf = pbyInBuf + wLength;
			
			break;
		case emDoubleStreamCapEx:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tConfInfoEx.SetDoubleStreamCapEx((TVideoStreamCap *)pbyInBuf,wLength/sizeof(TVideoStreamCap));
			pbyInBuf = pbyInBuf +  wLength;
			
			break;
		case emMainAudioTypeDesc:
			wLength = ntohs(*(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tConfInfoEx.SetAudioTypeDesc((TAudioTypeDesc*)pbyInBuf,wLength/sizeof(TAudioTypeDesc));
			pbyInBuf = pbyInBuf + wLength;
			
			break;
		case emScheduleDurationDateEx:
			wLength = ntohs(*(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tConfInfoEx.SetDurationDate((TDurationDate*)pbyInBuf);
			pbyInBuf = pbyInBuf + wLength;
			
			break;
		case emScheduleNextStartTimeEx:
			wLength = ntohs(*(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tConfInfoEx.SetNextStartTime((TKdvTime*)pbyInBuf);
			pbyInBuf = pbyInBuf + wLength;
			
			break;

		case emVmpChnnlMember:
			{
				wLength = ntohs( *(u16*)pbyInBuf);
				pbyInBuf = pbyInBuf + sizeof(u16);
				
				TVmpChnnlMember aTmpChnlMember[MAXNUM_MPU2VMP_MEMBER - MAXNUM_MPUSVMP_MEMBER];
				memset(aTmpChnlMember,0,sizeof(aTmpChnlMember));
				
				if( wLength/sizeof(TVmpChnnlMember) <= (MAXNUM_MPU2VMP_MEMBER - MAXNUM_MPUSVMP_MEMBER)
					&& ptVmpModule != NULL )
				{
					memcpy(aTmpChnlMember,(TVmpChnnlMember *)pbyInBuf,wLength);
					
					ptVmpModule->SetVmpChnnlMember(&aTmpChnlMember[0],wLength/sizeof(TVmpChnnlMember));
				}
				
				pbyInBuf = pbyInBuf + wLength;
			}

			break;

		case emHduVmpChnnl:
			{
				wLength = ntohs(*(u16*)pbyInBuf);
				pbyInBuf = pbyInBuf + sizeof(u16);
				THduVmpChnnl tVmpChnnl[(HDUID_MAX-HDUID_MIN+1)*MAXNUM_HDU_CHANNEL];
				u8 byVmpChnlNum = wLength/sizeof(THduVmpChnnl);
				if( byVmpChnlNum <= (HDUID_MAX-HDUID_MIN+1)*MAXNUM_HDU_CHANNEL)
				{
					u8 byIdx = 0;
					memcpy(&tVmpChnnl[0],pbyInBuf,wLength);
					for(byIdx = 0; byIdx < byVmpChnlNum; byIdx++)
					{
						if( byHduVmpChnlCount <(HDUID_MAX-HDUID_MIN+1)*MAXNUM_HDU_CHANNEL)
						{
							m_atHduVmpChnInfo[byHduVmpChnlCount].m_byHduEqpId = tVmpChnnl[byIdx].m_byHduEqpId;
							m_atHduVmpChnInfo[byHduVmpChnlCount].m_byChIdx = tVmpChnnl[byIdx].m_byChnnlIdx;
							m_atHduVmpChnInfo[byHduVmpChnlCount].m_byVmpStyle = tVmpChnnl[byIdx].m_byVmpStyle;
							
							byHduVmpChnlCount++;
						}
					}
					
					pbyInBuf = pbyInBuf + wLength;
					
					s32 nTmpConfExSize = nConfExSize - (s32)sizeof(u8) - (s32)sizeof(u16) - (s32)wLength;
					
					if(nTmpConfExSize > 0)
					{
						emCommunicateType eSubtype = (emCommunicateType)(*pbyInBuf);
						if (eSubtype == emHduVmpSubChnnl)
						{
							pbyInBuf ++;

							wLength = ntohs(*(u16*)pbyInBuf);
							pbyInBuf = pbyInBuf + sizeof(u16);
							
							THduVmpSubChnnl atHduVmpSubChnnl[HDU_MODEFOUR_MAX_SUBCHNNUM];
							u8 byVmpSubChnlNum = wLength/sizeof(THduVmpSubChnnl);
							if(byVmpSubChnlNum <= HDU_MODEFOUR_MAX_SUBCHNNUM && byHduVmpChnlCount >=1)
							{
								memcpy(&atHduVmpSubChnnl[0],pbyInBuf,wLength);
								for( byIdx = 0; byIdx < byVmpSubChnlNum; byIdx++)
								{
									m_atHduVmpChnInfo[byHduVmpChnlCount-1].m_abyMemberType[atHduVmpSubChnnl[byIdx].m_bySubChnnlIdx] = atHduVmpSubChnnl[byIdx].m_byMemberType;
									m_atHduVmpChnInfo[byHduVmpChnlCount-1].m_abyTvWallMember[atHduVmpSubChnnl[byIdx].m_bySubChnnlIdx] = atHduVmpSubChnnl[byIdx].m_byMember;
								}
							}

							pbyInBuf = pbyInBuf + wLength;

							nConfExSize = nTmpConfExSize;
						}
					}
				}
			}

			break;

		default:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16) +wLength;
			bExistUnknowInfo = TRUE;
			
			break;
		}
		
		nConfExSize = nConfExSize - (s32)sizeof(u8) - (s32)sizeof(u16) - (s32)wLength;
	}

	if(byHduVmpChnlCount != 0 && ptHduVmpModule != NULL)
	{
		ptHduVmpModule->SetHduVmpChnnlInfo(&m_atHduVmpChnInfo[0],byHduVmpChnlCount);
	}

	return;
}

/*=============================================================================
    �� �� ���� PackConfInfoEx
    ��    �ܣ� Pack���ݣ��洢��pbybuf��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� [IN]  TConfInfoEx &tConfInfoEx ��pack����
               [IN][OUT]u8 *pbyBuf pack�����ݴ洢λ��
               [IN][OUT] u16 &wPackConfDataLen Pack������ݳ��� ������
    �� �� ֵ�� BOOL32 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2011/12/29  4.0			chendaiwei                  ����
=============================================================================*/
void   PackConfInfoEx(const TConfInfoEx &tConfInfoEx,u8 * pbyOutbuf, u16 &wPackConfDataLen,
					  THduVmpModuleOrTHDTvWall *ptHduVmpModule,
					  TVmpModuleInfo *ptVmpModule,
					  u8 *pbyEncoding)
{
	//��չ��Ϣ����  u16��չ��Ϣ�ܳ���+ u8��Ϣ����A + u16��Ϣ����A�ֽ��� + ��Ϣ����A���ݣ�ǰ���ʶ�ֽ�����С��+ u16��Ϣ����B......[12/29/2011 chendaiwei]
	wPackConfDataLen = 0;
	u8 *pbyConfExSizeBuf = pbyOutbuf;
	u8 *pbyConfExBuf = pbyConfExSizeBuf + 2;

	// �������:����4���ֽ� [pengguofeng 4/12/2013]
	if ( pbyEncoding != NULL )
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[PackConfInfoEx Tick.%d]Add Mcu EncodingType to pbyOutbuf\n", OspTickGet());
		*pbyConfExBuf = (u8)emEncodingType;
		pbyConfExBuf++;
		wPackConfDataLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(u8));//���ȣ��������:net order
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackConfDataLen = wPackConfDataLen + sizeof(u16);
		
		memcpy(pbyConfExBuf, pbyEncoding, sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u8);
		wPackConfDataLen = wPackConfDataLen + sizeof(u8);
	}
	else
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CONF, "[PackConfInfoEx Tick.%d]param pbyEncoding is NULL\n", OspTickGet());
	}

	//������ѡ
	TVideoStreamCap tMSCap[MAX_CONF_CAP_EX_NUM];
	u8 byActualNum = MAX_CONF_CAP_EX_NUM;
	tConfInfoEx.GetMainStreamCapEx(tMSCap,byActualNum);

	if(byActualNum !=0)
	{
		*pbyConfExBuf = (u8)emMainStreamCapEx;
		pbyConfExBuf++;
		wPackConfDataLen++;
		
		*(u16*)pbyConfExBuf = htons(byActualNum*sizeof(TVideoStreamCap));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackConfDataLen = wPackConfDataLen + sizeof(u16);
		
		memcpy(pbyConfExBuf,tMSCap,byActualNum*sizeof(TVideoStreamCap));
		pbyConfExBuf = pbyConfExBuf + byActualNum*sizeof(TVideoStreamCap);
		wPackConfDataLen = wPackConfDataLen + byActualNum*sizeof(TVideoStreamCap);
	}

	//˫����ѡ
	TVideoStreamCap tDSCap[MAX_CONF_CAP_EX_NUM];
	u8 byActualDSNum = MAX_CONF_CAP_EX_NUM;
	tConfInfoEx.GetDoubleStreamCapEx(tDSCap,byActualDSNum);

	if(byActualDSNum !=0)
	{
		*pbyConfExBuf = (u8)emDoubleStreamCapEx;
		pbyConfExBuf++;
		wPackConfDataLen++;
		
		*(u16*)pbyConfExBuf = htons(byActualDSNum*sizeof(TVideoStreamCap));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackConfDataLen = wPackConfDataLen + sizeof(u16);
		
		memcpy(pbyConfExBuf,tDSCap,byActualDSNum*sizeof(TVideoStreamCap));
		pbyConfExBuf = pbyConfExBuf + byActualDSNum*sizeof(TVideoStreamCap);
		wPackConfDataLen = wPackConfDataLen + byActualDSNum*sizeof(TVideoStreamCap);
	}
	
	//��Ƶ��������
	TAudioTypeDesc  atAudioCap[MAXNUM_CONF_AUDIOTYPE];
	u8 byActualAudioNum = tConfInfoEx.GetAudioTypeDesc(atAudioCap);
	if( byActualAudioNum > MAXNUM_CONF_AUDIOTYPE )
	{
		byActualAudioNum = MAXNUM_CONF_AUDIOTYPE;
	}	 
	if(byActualAudioNum > 0)
	{
		*pbyConfExBuf = (u8)emMainAudioTypeDesc;
		pbyConfExBuf++;
		wPackConfDataLen++;
		
		*(u16*)pbyConfExBuf = htons(byActualAudioNum*sizeof(TAudioTypeDesc));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackConfDataLen = wPackConfDataLen + sizeof(u16);
		
		memcpy(pbyConfExBuf,atAudioCap,byActualAudioNum*sizeof(TAudioTypeDesc));
		pbyConfExBuf = pbyConfExBuf + byActualAudioNum*sizeof(TAudioTypeDesc);
		wPackConfDataLen = wPackConfDataLen + byActualAudioNum*sizeof(TAudioTypeDesc);
	}
	//TDuraDate�����òŴ���������ã�TConfInfoExĬ�Ϲ����TDuraDate���ǿ�
	TDurationDate tDuraDate = tConfInfoEx.GetDurationDate();
	if ( !tDuraDate.IsDuraStartTimeNull() || !tDuraDate.IsDuraEndTimeNull() )
	{
		*pbyConfExBuf = (u8)emScheduleDurationDateEx;
		pbyConfExBuf++;
		wPackConfDataLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(TDurationDate));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackConfDataLen = wPackConfDataLen + sizeof(u16);
		
		memcpy(pbyConfExBuf,&tDuraDate,sizeof(TDurationDate));
		pbyConfExBuf = pbyConfExBuf + sizeof(TDurationDate);
		wPackConfDataLen = wPackConfDataLen + sizeof(TDurationDate);
	}
	//TKdvTime�����òŴ���������ã�TConfInfoExĬ�Ϲ����TKdvTime���ǿ�
	TKdvTime tNextStartTime = tConfInfoEx.GetNextStartTime();
	if ( !tConfInfoEx.IsNextStartTimeNull() )
	{
		*pbyConfExBuf = (u8)emScheduleNextStartTimeEx;
		pbyConfExBuf++;
		wPackConfDataLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(TKdvTime));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackConfDataLen = wPackConfDataLen + sizeof(u16);
		
		memcpy(pbyConfExBuf,&tNextStartTime,sizeof(TKdvTime));
		pbyConfExBuf = pbyConfExBuf + sizeof(TKdvTime);
		wPackConfDataLen = wPackConfDataLen + sizeof(TKdvTime);
	}

	if(ptHduVmpModule != NULL)
	{
		if(ptHduVmpModule->IsUnionStoreHduVmpInfo())
		{
			for( u8 byLoopChIdx = 0; byLoopChIdx < (HDUID_MAX-HDUID_MIN+1)*MAXNUM_HDU_CHANNEL;byLoopChIdx++)
			{
				if(ptHduVmpModule->m_union.m_atHduVmpChnInfo[byLoopChIdx].IsValid())
				{
					*pbyConfExBuf = (u8)emHduVmpChnnl;
					pbyConfExBuf++;
					wPackConfDataLen++;
					
					*(u16*)pbyConfExBuf = htons(sizeof(THduVmpChnnl));
					pbyConfExBuf = pbyConfExBuf + sizeof(u16);
					wPackConfDataLen = wPackConfDataLen + sizeof(u16);
					
					THduVmpChnnl tHduVmpChnl;
					tHduVmpChnl.m_byHduEqpId = ptHduVmpModule->m_union.m_atHduVmpChnInfo[byLoopChIdx].m_byHduEqpId;
					tHduVmpChnl.m_byVmpStyle = ptHduVmpModule->m_union.m_atHduVmpChnInfo[byLoopChIdx].m_byVmpStyle;
					tHduVmpChnl.m_byChnnlIdx = ptHduVmpModule->m_union.m_atHduVmpChnInfo[byLoopChIdx].m_byChIdx;
					memcpy(pbyConfExBuf,&tHduVmpChnl,sizeof(THduVmpChnnl));
					pbyConfExBuf = pbyConfExBuf + sizeof(THduVmpChnnl);
					wPackConfDataLen = wPackConfDataLen + sizeof(THduVmpChnnl);
					
					THduVmpSubChnnl atSubChnnl[HDU_MODEFOUR_MAX_SUBCHNNUM];
					memset(&atSubChnnl[0],0,sizeof(atSubChnnl));
					u8 byCount = 0;
					for( u8 byLoopSubIdx = 0; byLoopSubIdx <HDU_MODEFOUR_MAX_SUBCHNNUM; byLoopSubIdx++)
					{
						u8 byMemberType = ptHduVmpModule->m_union.m_atHduVmpChnInfo[byLoopChIdx].m_abyMemberType[byLoopSubIdx];
						u8 byMember = ptHduVmpModule->m_union.m_atHduVmpChnInfo[byLoopChIdx].m_abyTvWallMember[byLoopSubIdx];
						//�кϷ��ն�ID�͸�������
						if(byMemberType != TW_MEMBERTYPE_NULL || byMember != 0)
						{
							atSubChnnl[byCount].m_bySubChnnlIdx = byLoopSubIdx;
							atSubChnnl[byCount].m_byMember = byMember;
							atSubChnnl[byCount].m_byMemberType = byMemberType;
							
							byCount++;
						}
					}
					
					if(byCount != 0)
					{
						*pbyConfExBuf = (u8)emHduVmpSubChnnl;
						pbyConfExBuf++;
						wPackConfDataLen++;
						
						*(u16*)pbyConfExBuf = htons(sizeof(THduVmpSubChnnl)*byCount);
						pbyConfExBuf = pbyConfExBuf + sizeof(u16);
						wPackConfDataLen = wPackConfDataLen + sizeof(u16);
						
						memcpy(pbyConfExBuf,&atSubChnnl[0],sizeof(THduVmpSubChnnl)*byCount);
						pbyConfExBuf = pbyConfExBuf + byCount*sizeof(THduVmpSubChnnl);
						wPackConfDataLen = wPackConfDataLen + byCount*sizeof(THduVmpSubChnnl);
					}
				}
			}
		}
	}

	//VMPģ��20���֮��ĺ�ͨ����Ϣ
	if(ptVmpModule != NULL)
	{
		TVmpChnnlMember atVmpChnl[MAXNUM_MPU2VMP_MEMBER-MAXNUM_MPUSVMP_MEMBER];
		memset(atVmpChnl,0,sizeof(atVmpChnl));
		u8 byVmpChnlCount = 0;
		for( u8 byIdx = MAXNUM_MPUSVMP_MEMBER; byIdx < MAXNUM_MPU2VMP_MEMBER;byIdx++)
		{
			if(ptVmpModule->m_abyMemberType[byIdx] != VMP_MEMBERTYPE_NULL
				|| ptVmpModule->m_abyVmpMember[byIdx] != 0)
			{
				atVmpChnl[byVmpChnlCount].m_byChnIdx = byIdx;
				atVmpChnl[byVmpChnlCount].m_byMemberType = ptVmpModule->m_abyMemberType[byIdx];
				atVmpChnl[byVmpChnlCount].m_byVmpMember = ptVmpModule->m_abyVmpMember[byIdx];
				byVmpChnlCount++;
			}
		}
		
		if ( byVmpChnlCount > 0 )
		{
			*pbyConfExBuf = (u8)emVmpChnnlMember;
			pbyConfExBuf++;
			wPackConfDataLen++;
			
			*(u16*)pbyConfExBuf = htons(byVmpChnlCount*sizeof(TVmpChnnlMember));
			pbyConfExBuf = pbyConfExBuf + sizeof(u16);
			wPackConfDataLen = wPackConfDataLen + sizeof(u16);
			
			memcpy(pbyConfExBuf,atVmpChnl,byVmpChnlCount*sizeof(TVmpChnnlMember));
			pbyConfExBuf = pbyConfExBuf + byVmpChnlCount*sizeof(TVmpChnnlMember);
			wPackConfDataLen = wPackConfDataLen + byVmpChnlCount*sizeof(TVmpChnnlMember);
		}
	}


	//Buffer�ܴ�С
	*(u16*)pbyConfExSizeBuf = htons(wPackConfDataLen);
	wPackConfDataLen = wPackConfDataLen + sizeof(u16); //�����ײ������ֽ�

	return;
}

/*====================================================================
������       GetConfInfoExFromConfAttrb
����        ���ӻ���������Ϣ�л�ȡ��չ�ֱ��ʹ�ѡ��Ϣ��������µĽṹ��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����[IN] TConfInfo ������Ϣ�ṹ���� 
����ֵ˵��  ��[OUT]TConfInfoEx ��չ��������ѡ��Ϣ
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
12/06/07    4.7         �´�ΰ          ����
====================================================================*/
TConfInfoEx GetConfInfoExFromConfAttrb(const TConfInfo& tConfInfo)
{
	TConfInfoEx tConfInfoEx;

	TConfAttrbEx tTConfAttrbEx = tConfInfo.GetConfAttrbEx();
	if(!tTConfAttrbEx.IsImaxModeConf())
	{
		TVideoStreamCap tCapEx[4];
		u8 byIdx = 0;
		
		//8000H��ѡ1080/30/25֡���[2/14/2012 chendaiwei]
		if(tTConfAttrbEx.IsResEx1080())
		{
			u8 byfps = tConfInfo.GetMainVidUsrDefFPS();
			if(byfps > 30 )
			{
				byfps = tConfInfo.GetMainVidUsrDefFPS()/2;
			}
			TVideoStreamCap tTempCap(MEDIA_TYPE_H264,VIDEO_FORMAT_HD1080,byfps,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate());
			tCapEx[byIdx] = tTempCap;
			byIdx++;
		}

		if(tTConfAttrbEx.IsResEx720())
		{
			TVideoStreamCap tTempCap(MEDIA_TYPE_H264,VIDEO_FORMAT_HD720,tConfInfo.GetMainVidUsrDefFPS(),emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate());
			tCapEx[byIdx] = tTempCap;
			byIdx++;
		}
		
		if(tTConfAttrbEx.IsResEx4Cif())
		{	
			u8 byfps = tConfInfo.GetMainVidUsrDefFPS();
			if(byfps >= 30 )
			{
				byfps = 25;
			}
			
			TVideoStreamCap tTempCap(MEDIA_TYPE_H264,VIDEO_FORMAT_4CIF,byfps,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate());
			tCapEx[byIdx] = tTempCap;
			byIdx++;
		}
		
		if(tTConfAttrbEx.IsResExCif())
		{
			u8 byfps = tConfInfo.GetMainVidUsrDefFPS();
			if(byfps >= 30 )
			{
				byfps = 25;
			}
			
			TVideoStreamCap tTempCap(MEDIA_TYPE_H264,VIDEO_FORMAT_CIF,byfps,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate());
			tCapEx[byIdx] = tTempCap;
			byIdx++;
		}
		
		tConfInfoEx.SetMainStreamCapEx(&tCapEx[0],byIdx);

	}
	//IMax����
	else
	{
		//����
		TVideoStreamCap tImaxCapEx[5];
		u8 byIdx = 0;

		TVideoStreamCap t1080Cap(MEDIA_TYPE_H264,VIDEO_FORMAT_HD1080,30,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate());
		tImaxCapEx[byIdx] = t1080Cap;
		byIdx++;

		TVideoStreamCap t720Cap(MEDIA_TYPE_H264,VIDEO_FORMAT_HD720,60,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate());
		tImaxCapEx[byIdx] = t720Cap;
		byIdx++;

		TVideoStreamCap t72030Cap(MEDIA_TYPE_H264,VIDEO_FORMAT_HD720,30,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate());
		tImaxCapEx[byIdx] = t72030Cap;
		byIdx++;

		TVideoStreamCap t4cifCap(MEDIA_TYPE_H264,VIDEO_FORMAT_4CIF,25,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate());
		tImaxCapEx[byIdx] = t4cifCap;
		byIdx++;
		
		TVideoStreamCap tcifCap(MEDIA_TYPE_H264,VIDEO_FORMAT_CIF,25,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate());
		tImaxCapEx[byIdx] = tcifCap;
		byIdx++;

		tConfInfoEx.SetMainStreamCapEx(&tImaxCapEx[0],byIdx);

		//˫��
		TVideoStreamCap tImaxDSCapEx[4];
		byIdx = 0;
		
		TVideoStreamCap t720DSCap(MEDIA_TYPE_H264,VIDEO_FORMAT_HD720,30,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate()*tConfInfo.GetDStreamScale()/100);
		tImaxDSCapEx[byIdx] = t720DSCap;
		byIdx++;
		
		TVideoStreamCap tXGADSCap(MEDIA_TYPE_H264,VIDEO_FORMAT_XGA,5,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate()*tConfInfo.GetDStreamScale()/100);
		tImaxDSCapEx[byIdx] = tXGADSCap;
		byIdx++;
		
		TVideoStreamCap t4cifDSCap(MEDIA_TYPE_H264,VIDEO_FORMAT_4CIF,25,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate()*tConfInfo.GetDStreamScale()/100);
		tImaxDSCapEx[byIdx] = t4cifDSCap;
		byIdx++;
		
		TVideoStreamCap tcifDSCap(MEDIA_TYPE_H264,VIDEO_FORMAT_CIF,25,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate()*tConfInfo.GetDStreamScale()/100);
		tImaxDSCapEx[byIdx] = tcifDSCap;
		byIdx++;

		tConfInfoEx.SetDoubleStreamCapEx(&tImaxDSCapEx[0],byIdx);
	}


	//������ģ��ֻ��һ����Ƶ���ͣ�����ֻ�е�����
	TAudioTypeDesc tMainAudioType(tConfInfo.GetMainAudioMediaType(),1);
	tConfInfoEx.SetAudioTypeDesc(&tMainAudioType,1);

	return tConfInfoEx;
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

	TConfAttrb tConfAttrb = ptPackConfStore->m_tConfInfo.GetConfAttrb();
	//����ǽģ�� 
	if (tConfAttrb.IsHasTvWallModule())
	{
		memcpy(pbyModuleBuf, (s8*)&tConfStore.m_tMultiTvWallModule, sizeof(TMultiTvWallModule));
		wPackConfDataLen += sizeof(TMultiTvWallModule);
		pbyModuleBuf += sizeof(TMultiTvWallModule);
	}
	//����ϳ�ģ�� 
	if (tConfAttrb.IsHasVmpModule())
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
	//                  + 1 TMtAlias (�����ն˱���)
	u8* pbyVCSInfoExBuf = pbyModuleBuf;  //VCS����ģ���������ݻ���

	if (VCS_CONF == tConfStore.m_tConfInfo.GetConfSource())
	{

		*pbyVCSInfoExBuf++  = (u8)tConfStore.IsHDTWCfg();
		wPackConfDataLen += sizeof(u8);
		if (tConfStore.IsHDTWCfg())
		{
			memcpy(pbyVCSInfoExBuf, &tConfStore.m_tHDTWInfo, sizeof(THDTvWall));
			pbyVCSInfoExBuf  += sizeof(THDTvWall);
			wPackConfDataLen += sizeof(THDTvWall);
		}

		u8 byHduChnlNum = tConfStore.m_tHduModule.GetHduChnlNum( FALSE );
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
		
		//zhouyiliang 20100820 VCS 1+1 ���ݱ��汾�ر����ն�
		memcpy( pbyVCSInfoExBuf,&tConfStore.m_tVCSBackupChairMan,sizeof(TMtAlias) );
		wPackConfDataLen += sizeof(TMtAlias);
		
		// [4/20/2011 xliang] VCS VCAutoMode
		pbyVCSInfoExBuf += sizeof(TMtAlias);
		*pbyVCSInfoExBuf = tConfStore.m_byVCAutoMode;
		wPackConfDataLen += sizeof(u8);
		pbyVCSInfoExBuf += sizeof(u8);
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
BOOL32 UnPackConfStore( TPackConfStore *ptPackConfStore, TConfStore &tConfStore, u16& wPackConfDataLen, BOOL32 bUnPackExInfo )
{
	u16 wAliasBufLen = 0;
	u8* pbyAliasBuf  = NULL; //�ն˱�������������
	u8* pbyModuleBuf = NULL; //����ǽ//����ϳ�ģ�建��
	
	wPackConfDataLen = 0;
	u8 *pbyInitPos = (u8*)ptPackConfStore; //��¼ָ���ʼλ��[8/16/2012 chendaiwei]

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
	TConfAttrb tConfAttrb = ptPackConfStore->m_tConfInfo.GetConfAttrb();
	if (tConfAttrb.IsHasTvWallModule())
	{
		memcpy((s8*)&tConfStore.m_tMultiTvWallModule, pbyModuleBuf, sizeof(TMultiTvWallModule));
		pbyModuleBuf += sizeof(TMultiTvWallModule);
	}
	//����ϳ�ģ�� 
	if (tConfAttrb.IsHasVmpModule())
	{
		memcpy((s8*)&tConfStore.m_atVmpModule, pbyModuleBuf, sizeof(TVmpModule));
		pbyModuleBuf += sizeof(TVmpModule);
	}

	//VCS��Ϣ[12/27/2011 chendaiwei]
	u8 * pbyVCSInfoBuf = pbyModuleBuf;
	if (VCS_CONF == ptPackConfStore->m_tConfInfo.GetConfSource())
	{	
		u8 byIsSupportHDTW = *pbyVCSInfoBuf++;
		if (byIsSupportHDTW)
		{
			pbyVCSInfoBuf += sizeof(THDTvWall);
		}
		
		u8 byHduNum = *pbyVCSInfoBuf++;
		if (byHduNum)
		{
			pbyVCSInfoBuf += byHduNum * sizeof(THduModChnlInfo);
		}
		
		u8 bySMCUExist = *pbyVCSInfoBuf++;
		if (bySMCUExist)
		{
			wAliasBufLen = htons(*(u16*)pbyVCSInfoBuf);
			pbyVCSInfoBuf += sizeof(u16);
			wAliasBufLen = ntohs(wAliasBufLen);
			pbyVCSInfoBuf += wAliasBufLen;					
		}
		
		// ����ģ���в�����������Ϣ
		pbyVCSInfoBuf++;

		//zhouyiliang 20100820 ���ӱ����ն�
		pbyVCSInfoBuf += sizeof(TMtAlias);
		
		// [4/20/2011 xliang] VCAutoMode
		pbyVCSInfoBuf ++;
	}

	if(bUnPackExInfo)
	{
		u16 wConfInfoExLength = ntohs(*(u16*)pbyVCSInfoBuf) + sizeof(u16);
		if(wConfInfoExLength <= CONFINFO_EX_BUFFER_LENGTH)
		{
			memcpy(tConfStore.m_byConInfoExBuf,pbyVCSInfoBuf,wConfInfoExLength);
			pbyVCSInfoBuf += wConfInfoExLength;
		}
		else
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_CONF,"[UnPackConfStore] %s buffer is out of memory, error!\n",tConfStore.m_tConfInfo.GetConfName());

			return FALSE;
		}
	}

	wPackConfDataLen = pbyVCSInfoBuf - pbyInitPos;

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
                            TVmpModule &tOutVmpModule,
							THduVmpModuleOrTHDTvWall &tHduVmpModule,
							TVmpModuleInfo &tVmpModuleEx)
{
    BOOL32 bReplace = FALSE;
	TConfAttrb tConfattrb = tConfStore.m_tConfInfo.GetConfAttrb();
		
	TConfInfoEx tConfInfoEx;
	u16 wPackConfExInfoLength = 0;
	BOOL32 bUnkownConfInfo = FALSE;
	THduVmpModuleOrTHDTvWall tOrgiHduVmpModule;
	TVmpModuleInfo tOrigVmpModuleEx25;
	UnPackConfInfoEx(tConfInfoEx,tConfStore.m_byConInfoExBuf, wPackConfExInfoLength,
						bUnkownConfInfo, &tOrgiHduVmpModule, &tOrigVmpModuleEx25);

    if (tConfattrb.IsHasTvWallModule())
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
    if (tConfattrb.IsHasVmpModule())
    {
        for (u8 byVmpIdx = 0; byVmpIdx < MAXNUM_MPUSVMP_MEMBER; byVmpIdx++)
        {
            if( byOldIdx == tConfStore.m_atVmpModule.m_abyVmpMember[byVmpIdx] )
            {
                tOutVmpModule.m_abyVmpMember[byVmpIdx] = byNewIdx;
                bReplace = TRUE;
            }
        }

		for ( u8 byExVmpIdx = MAXNUM_MPUSVMP_MEMBER; byExVmpIdx < MAXNUM_MPU2VMP_MEMBER;byExVmpIdx ++ )
		{
			if( byOldIdx == tOrigVmpModuleEx25.m_abyVmpMember[byExVmpIdx])
			{
				tVmpModuleEx.m_abyVmpMember[byExVmpIdx] = byNewIdx;
				bReplace = TRUE;
			}
		}
    }

	if (tOrgiHduVmpModule.IsUnionStoreHduVmpInfo())
	{
		for(u8 byLoopChnIdx = 0; byLoopChnIdx < (HDUID_MAX-HDUID_MIN+1)*MAXNUM_HDU_CHANNEL; byLoopChnIdx++)
		{
			if(tOrgiHduVmpModule.m_union.m_atHduVmpChnInfo[byLoopChnIdx].IsValid())
			{
				for (u8 bySubChnlIdx = 0; bySubChnlIdx < HDU_MODEFOUR_MAX_SUBCHNNUM; bySubChnlIdx++)
				{
					u8 byMtIndex =  tOrgiHduVmpModule.m_union.m_atHduVmpChnInfo[byLoopChnIdx].m_abyTvWallMember[bySubChnlIdx];
					u8 byMemberType = tOrgiHduVmpModule.m_union.m_atHduVmpChnInfo[byLoopChnIdx].m_abyMemberType[bySubChnlIdx];
					
					if( byMtIndex == byOldIdx )
					{
						tHduVmpModule.m_union.m_atHduVmpChnInfo[byLoopChnIdx].m_abyTvWallMember[bySubChnlIdx] = byNewIdx;

						bReplace = TRUE;
					}
				}
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
	case MEDIA_TYPE_G719:
		sprintf( szMediaType, "%s%c", "G.719", 0 );
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
	case VIDEO_FORMAT_384x272:
		sprintf( szResType, "%s%c", "384x272",0 );
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
	case VIDEO_FORMAT_640x544:
		sprintf( szResType, "%s%c", "640x544",0 );
		break;
	case VIDEO_FORMAT_320x272:
		sprintf( szResType, "%s%c", "320x272",0 );
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
	if (tConfInfo.GetCapSupport().GetDStreamType() == VIDEO_DSTREAM_MAIN ||
		tConfInfo.GetCapSupport().GetDStreamType() == VIDEO_DSTREAM_MAIN_H239 )
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
/*==============================================================================
������    :  IsConfFrm50OR60
����      :  �жϻ���֡���Ƿ���50 or 60
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
20100510                   �ܼ���                            ����
==============================================================================*/
BOOL32 IsConfFR50OR60(const TConfInfo &tConfInfo)
{
	u8 byConfFrm = 0;
	if(MEDIA_TYPE_H264 == tConfInfo.GetMainVideoMediaType())
	{
		byConfFrm = tConfInfo.GetMainVidUsrDefFPS();
	}
	else
	{
		byConfFrm = tConfInfo.GetMainVidFrameRate();
	}
	
	LogPrint( LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[IsConfFrm50OR60] ConfFrm is %d!\n", byConfFrm);
	
	if (byConfFrm == 50 || byConfFrm == 60)
	{
		return TRUE;	
	}
	return FALSE;
}


/*==============================================================================
������    :  IsDSFR50OR60
����      :  �ж�˫��֡���Ƿ��֡��
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
20121009                   ��־��                           ����
==============================================================================*/
BOOL32 IsDSFR50OR60(const TConfInfo &tConfInfo)
{
	u8 byConfFrm = tConfInfo.GetDStreamUsrDefFPS();
	
	LogPrint( LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[IsDSFR50OR60] DSFrm is %d!\n", byConfFrm);
	
	if (byConfFrm == 50 || byConfFrm == 60)
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
SEMHANDLE	g_hGetManuStr = NULL;						// GetManufactureStrר��

LPCSTR GetManufactureStr( u8 byManufacture )
{
	if (NULL == g_hGetManuStr)
	{
		if( !OspSemBCreate(&g_hGetManuStr))
		{
			OspSemDelete( g_hGetManuStr );
			g_hGetManuStr = NULL;
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetManufactureStr] create g_hGetManuStr failed!\n" );
		}
	}

	if (NULL != g_hGetManuStr)
	{
		OspSemTake(g_hGetManuStr);
	}

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
	case MT_MANU_AETHRA:
		sprintf( szManu, "%s%c", "Aethra", 0 );
        break;
	case MT_MANU_CHAORAN:
		sprintf( szManu, "%s%c", "ChaoRan", 0 );
        break;
	case MT_MANU_OTHER:
		sprintf( szManu, "%s%c", "Unknown", 0 );
		break;
	default: sprintf( szManu, "%s%c", "Unknown", 0);break;
	}

	if (NULL != g_hGetManuStr)
	{
		OspSemGive(g_hGetManuStr);
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
	/*lint -save -esym(529, EV_MCUMT_SAT_END)*/
	/*lint -save -e537*/

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

	/*lint -restore*/
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
        LogPrint( LOG_LVL_ERROR, MID_MCU_MIXER, "Start local mixer success!\n" );
	
    }
    else
    {
        LogPrint( LOG_LVL_ERROR, MID_MCU_MIXER, "Start local mixer failure!\n");
    }
#endif

	LogPrint(LOG_LVL_DETAIL,MID_MCU_MIXER,"[StartLocalMixer] byEqpId.%d\n",byEqpId);

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

	LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[StartLocalVmp] byEqpId.%d\n",byEqpId);
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

	// [9/26/2010 liuxu] �˴���Ҫ�Ż���PRS id��PRSID_MIN��PRSID_MAX֮��
	// ���⣬���û��PRS�豸����ȥ��Ӧ���и���ǣ��˴�����б�ǣ���ʡ������forѭ��

	for( byEqpId=1; byEqpId <= MAXNUM_MCU_PERIEQP; byEqpId++ )
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
	tPrsCfg.dwLocalIP    = g_cMcuAgent.GetMpcIp();

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
	g_cMcuAgent.WritePrsTable(1, &tPrsParam);


	if( prsinit( &tPrsCfg ) )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_PRS, "Start local prs success!\n" );
	}
	else
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_PRS, "Start local prs failure!\n" );
	}
#endif
	LogPrint(LOG_LVL_DETAIL,MID_MCU_PRS,"[StartLocalPrs] byEqpId.%d\n",byEqpId);
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
        LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, "Start local bas success!\n");
    }
    else
    {
        LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, "Start local bas failure!\n");
    }

#endif	
	LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[StartLocalBas] byEqpId.%d\n",byEqpId);
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
	// [7/25/2011 liuxu] �������ĸ��ӿ�����8000a�¶��ǿյ�,�ᵼ��pclint��522�Ĵ���,���������˴���
	/*lint -save -esym(522, StartLocalMixer, StartLocalVmp, StartLocalBas, StartLocalPrs)*/

#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	//[nizhijun 2011/02/17] 8ke prs���û�
	//StartLocalPrs(PRSID_MIN);
#else	
	u8 byEqpId;
	u32 dwEqpIp;
	u8 byEqpType;

    // ����Ƿ����������衣���ڿ��ƻ�������
   // u8 byEqpStart = 0;

	for( byEqpId=1 ;byEqpId <= MAXNUM_MCU_PERIEQP ;byEqpId++ )
	{
		if( SUCCESS_AGENT == g_cMcuAgent.GetPeriInfo( byEqpId, &dwEqpIp, &byEqpType ) )
		{
			if( dwEqpIp == g_cMcuAgent.GetMpcIp() )
			{
				switch( byEqpType ) 
				{
				case EQP_TYPE_MIXER:
					{
						StartLocalMixer( byEqpId );
						//++byEqpStart;
					}
					break;
					
				case EQP_TYPE_VMP:
					{
						StartLocalVmp( byEqpId );
					//	++byEqpStart;
					}
					break;

				case EQP_TYPE_BAS:
					{
						StartLocalBas( byEqpId );
						//++byEqpStart;
					}
					break;
				
				case EQP_TYPE_PRS:
					{
						StartLocalPrs( byEqpId );
						//++byEqpStart;
					}
					break;

				default:
					break;
				}
			}
		}
	}
#endif

#ifdef _VXWORKS
   // if (byEqpStart != 0)
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
    04/03/11    3.0         ������        ����
	10/03/01	4.6			Ѧ��		  ����8000E	
====================================================================*/
void StartLocalMp( void )
{
	/*
#ifdef _8KE_
	// [3/1/2010 xliang]  FIXME: ������
	TMcu8KECfg tMcu8KECfg;
	g_cMcuAgent.GetMcuEqpCfg(&tMcu8KECfg);
	
	
	TNetAdaptInfo * ptNetAdapt = tMcu8KECfg.m_tLinkNetAdap.GetValue(0);	
	if( ptNetAdapt == NULL)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "GetNetAdaptInfo error! Local Mp start failed!\n");
		return;
	}
	TNetParam *ptNetParam = ptNetAdapt->m_tLinkIpAddr.GetValue(0);			//host ip, mask
	
	u16 wMcuPort = g_cMcuVcApp.GetMcuListenPort();
	u32 dwMcuIp = 0;
	TMp tMp;
	
	if( ptNetParam->IsValid() )
	{
		dwMcuIp = ptNetParam->GetIpAddr();
		
		tMp.SetMcuId( LOCAL_MCUID );
		tMp.SetMpId( MCU_BOARD_MPC );
		tMp.SetAttachMode( 1 );
		tMp.SetDoubleLink( 0 );
		tMp.SetIpAddr( dwMcuIp );
	}

#else
  */
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
//#endif

	//��������
	if( !mpstart(dwMcuIp, wMcuPort, &tMp ) )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "MCU: Start mp failure!\n" );
		return;
	}
	return;
}

void StopLocalMp( void )
{
	mpstop();
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
	10/03/01	4.6			Ѧ��		  ����8000E	
====================================================================*/
void StartLocalMtadp( void )
{
	/*
#ifdef _8KE_
	// [3/1/2010 xliang] FIXME: ������
	TMcu8KECfg tMcu8KECfg;
	g_cMcuAgent.GetMcuEqpCfg(&tMcu8KECfg);	
	TNetAdaptInfo * ptNetAdapt = tMcu8KECfg.m_tLinkNetAdap.GetValue(0);		
	// [3/30/2010 xliang] FIXME: ����һֱ��0��
	TNetAdaptInfo * ptNetAdapt = tMcu8KECfg.m_tLinkNetAdap.GetValue(0);	
	
	if( ptNetAdapt == NULL)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, "GetNetAdaptInfo error! Local Mtadp start failed!\n");
		printf("GetNetAdaptInfo error! Local Mtadp start failed!\n");
		return;
	}
	TNetParam *ptNetParam = ptNetAdapt->m_tLinkIpAddr.GetValue(0);			//host ip, mask
	
	u16 wMcuPort = g_cMcuVcApp.GetMcuListenPort();
	u32 dwMcuIp = 0;
	TMtAdp tMtadp;
	
	if( ptNetParam->IsValid() )
	{
		dwMcuIp = ptNetParam->GetIpAddr();
		
		tMtadp.SetMtadpId( MCU_BOARD_MPC );
		tMtadp.SetAttachMode( 1 );
		tMtadp.SetIpAddr( dwMcuIp );
	}
	
	
	TMtAdpConnectParam tConnectParam;
	
	tConnectParam.m_dwMcuTcpNode = 0;
	tConnectParam.m_wMcuPort = wMcuPort;
	tConnectParam.SetMcuIpAddr( dwMcuIp );
	
	tConnectParam.m_bSupportHD = TRUE;//����mtadp֧�ָ���

#else
*/

	//u8  byMpcId  = (g_cMcuAgent.GetMpcBoardId()%16 == 0) ? 16 : (g_cMcuAgent.GetMpcBoardId()%16);
	u8  byMpcId  = g_cMcuAgent.GetMpcBoardId();
	u32 dwMcuIp  = g_cMcuAgent.GetMpcIp();
	u16 wMcuPort = g_cMcuVcApp.GetMcuListenPort();

	if( 0 == dwMcuIp || inet_addr("127.0.0.1") == dwMcuIp ) 
	{
		LogPrint( LOG_LVL_ERROR, MID_MCULIB_MTADP, "LocalIP.%x is invalid reset localip start\n", dwMcuIp );

		u32 tdwIPList[5];  //�оٵ��ĵ�ǰ���õ�������Чip
		u16 dwIPNum;       //�оٵ�ip ��Ŀ

		dwIPNum = OspAddrListGet(tdwIPList, 5);
		if( 0 == dwIPNum )
		{
			LogPrint( LOG_LVL_ERROR, MID_MCULIB_MTADP, "LocalIP.%x is invalid reset localip err\n", dwMcuIp );
		}
		for(u16 dwAdapter=0; dwAdapter<dwIPNum; dwAdapter++)
		{
			if((inet_addr( "127.0.0.1" ) != tdwIPList[dwAdapter]))
			{
				LogPrint( LOG_LVL_ERROR, MID_MCULIB_MTADP, "LocalIP.%x is invalid reset localip.%x ok\n", dwMcuIp, tdwIPList[dwAdapter] );

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

//#endif	//!8KE 

	//�ն�����ģ�飨��Ƕ��ʽ������ģ�飩
	if( !MtAdpStart(&tConnectParam, &tMtadp) )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCULIB_MTADP, "MCU: Start mtadp failure!\n" );
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
	case VMP_STYLE_TEN_M:
		byMaxMemNum = 10;
		break;
	case VMP_STYLE_THIRTEEN:
	case VMP_STYLE_THIRTEEN_M:
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
	case VMP_STYLE_FIFTEEN:
		 byMaxMemNum = 15;
		 break;
	case VMP_STYLE_TWENTYFIVE:
		byMaxMemNum = 25;
		break;
	default:
		break;
	}

	return byMaxMemNum;
}

/*====================================================================
    ������      : IsValidVmpId
    ����        : �ж��Ƿ�����Чvmpid
    �㷨ʵ��    : 
    ����ȫ�ֱ���: ��
    �������˵��: byVmpId 
    ����ֵ˵��  : BOOL32
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    13/04/12    4.7       yanghuaizhi       ����
====================================================================*/
BOOL32 IsValidVmpId(u8 byVmpId)
{
	if (byVmpId >= VMPID_MIN &&
		byVmpId <= VMPID_MAX)
	{
		return TRUE;
	}
	return FALSE;
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
				if( ptMtTempAlias->m_AliasType == puAliasTypeIPPlusAlias )
				{
					*pszTemp++ = sizeof(u32) + strlen( &ptMtTempAlias->m_achAlias[sizeof(u32)] );
					memcpy( pszTemp, ptMtTempAlias->m_achAlias, sizeof(u32) + strlen( &ptMtTempAlias->m_achAlias[sizeof(u32)] ) );
					pszTemp = pszTemp + sizeof(u32) + strlen( &ptMtTempAlias->m_achAlias[sizeof(u32)] );
				}
				else
				{
					*pszTemp++ = strlen( ptMtTempAlias->m_achAlias );
					memcpy( pszTemp, ptMtTempAlias->m_achAlias, strlen( ptMtTempAlias->m_achAlias ) );
					pszTemp = pszTemp + strlen( ptMtTempAlias->m_achAlias );
				}
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
 
	while( (pszTemp - pszBuf < wBufLen) &&
		    // fxh����MAXNUM_CONF_MT���ն���Ϣ���ԣ�����Խ��
		    byMtCount < MAXNUM_CONF_MT )
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
	case MEDIA_TYPE_G719:
		wBitrate = AUDIO_BITRATE_G719;
		break;
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
�� �� ���� GetMaxAudioBiterate
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����const TConfInfoEx &tConInfoEx 
�� �� ֵ��u16
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
2013/04/09  4.7			��־��                 ����
=============================================================================*/
u16 GetMaxAudioBiterate(const TConfInfoEx &tConInfoEx)
{
	TAudioTypeDesc tAudDesc[MAXNUM_CONF_AUDIOTYPE];
	u8 byAudNum = tConInfoEx.GetAudioTypeDesc(tAudDesc);
	u16 wMaxAudBiterate = 0;

	for ( u8 byIdx =0 ; byIdx<byAudNum; byIdx++ )
	{
		if ( GetAudioBitrate( tAudDesc[byIdx].GetAudioMediaType()) > wMaxAudBiterate )
		{
			wMaxAudBiterate = GetAudioBitrate( tAudDesc[byIdx].GetAudioMediaType());
		}
	}
	
	return wMaxAudBiterate;
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
	2010/12/31  4.6			 ����                  �޸�
=============================================================================*/
TConfMcInfo* TConfOtherMcTable::AddMcInfo(const u16 wMcuIdx)
{
	// 192������local mcuid
	if (!IsValidSubMcuId(wMcuIdx))
	{
		return NULL;
	}

	if( m_atConfOtherMcInfo[wMcuIdx].GetMcuIdx() == wMcuIdx )
	{
		return &(m_atConfOtherMcInfo[wMcuIdx]);
	}

	m_atConfOtherMcInfo[wMcuIdx].SetNull();
	m_atConfOtherMcInfo[wMcuIdx].SetMcuIdx( wMcuIdx );//.m_wMcuIdx = wMcuIdx;
	m_atConfOtherMcInfo[wMcuIdx].m_tSpyMt.SetNull();
	m_atConfOtherMcInfo[wMcuIdx].m_dwSpyViewId = OspTickGet();
	m_atConfOtherMcInfo[wMcuIdx].m_dwSpyVideoId = OspTickGet()+10;
	
    ///FIXME: guzh [7/4/2007] Ȩ��֮�ƣ����ѡ���˲��������б��ᵼ��û�и��¼�MCU��״̬
    // ������ȰѸ�MCU�����״̬���úã���ʹ�÷��ԡ�ѡ������ý��Դ�ȼ��������ܹ����Խ��� 
    TMt tMt;
    tMt.SetMcuIdx(wMcuIdx);
    tMt.SetMtId(0);
    m_atConfOtherMcInfo[wMcuIdx].m_atMtExt[0].SetMt(tMt);
    m_atConfOtherMcInfo[wMcuIdx].m_atMtStatus[0].SetMt(tMt);
    
	return &(m_atConfOtherMcInfo[wMcuIdx]);
}


/*=============================================================================
    �� �� ���� SetMcInfo
    ��    �ܣ� ����McuIdx��������TConfMcInfo
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� wMcuIdx, Id��; 
	           tMcInfo, TConfMcInfo
    �� �� ֵ�� BOOL32
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2010/12/31  4.6			liuxu                   ����
=============================================================================*/
BOOL32 TConfOtherMcTable::SetMcInfo(const u16 wMcuIdx, const TConfMcInfo& tMcInfo)
{
	// �Ƿ�����mcu id
	if (!IsValidSubMcuId(wMcuIdx))
	{
		return FALSE;
	}

	// ���ۺ�ʱ��ǿ�����
	memcpy(&m_atConfOtherMcInfo[wMcuIdx], &tMcInfo, sizeof(TConfMcInfo));

	return TRUE;
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
    2010/12/31  4.6			liuxu                   �޸�
=============================================================================*/
BOOL32 TConfOtherMcTable::RemoveMcInfo( const u16 wMcuIdx )
{
	if (!IsValidSubMcuId(wMcuIdx))
	{
		return FALSE;
	}

	if(m_atConfOtherMcInfo[wMcuIdx].GetMcuIdx() == wMcuIdx)
	{
		memset( &m_atConfOtherMcInfo[wMcuIdx], 0, sizeof(TConfMcInfo) );
		m_atConfOtherMcInfo[wMcuIdx].SetNull();
		return TRUE;
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
	2010/12/31  4.6			 ����                   ����
=============================================================================*/
TConfMcInfo* TConfOtherMcTable::GetMcInfo(const u16 wMcuIdx)
{
	if (!IsValidSubMcuId(wMcuIdx))
	{
		// ֻ��ӡ��192�ķǷ�idx
		if (!IsLocalMcuId(wMcuIdx))
		{
			LogPrint( LOG_LVL_WARNING, MID_MCU_MMCU, "[TConfOtherMcTable::GetMcInfo] wMcuIdx:%d is not a invalid sub mcu id!\n", wMcuIdx);
		}

		return NULL;
	}

	if(m_atConfOtherMcInfo[wMcuIdx].GetMcuIdx() == wMcuIdx)
	{
		return &(m_atConfOtherMcInfo[wMcuIdx]);
	}
	else
	{
		return NULL;
	}
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
BOOL32  TConfOtherMcTable::SetMtInMixing(const TMt& tMt)
{
    TConfMcInfo *ptMcInfo = GetMcInfo(tMt.GetMcuIdx());
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
BOOL32  TConfOtherMcTable::IsMtInMixing( const TMt& tMt)  
{       
    TConfMcInfo *ptMcInfo = GetMcInfo(tMt.GetMcuIdx());
    if (NULL != ptMcInfo)
    {
        const TMcMtStatus *ptStatus = ptMcInfo->GetMtStatus(tMt);
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
BOOL32   TConfOtherMcTable::ClearMtInMixing(const TMcu& tMcu)
{
    TMt tMt;
    tMt.SetMcuIdx(tMcu.GetMcuIdx());
    TConfMcInfo *ptMcInfo = GetMcInfo(tMcu.GetMcuIdx());
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

/*=============================================================================
�� �� ���� GetIsMcuSupMultSpy
��    �ܣ� �ж�ĳ���¼�mcu�Ƿ�֧�ֶ�ش�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byMcuId 
�� �� ֵ�� void  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
20100106  4.0			pengjie                create
=============================================================================*/
BOOL32  TConfOtherMcTable::GetIsMcuSupMultSpy( const u16 wMcuIdx)
{
	if( IsValidSubMcuId(wMcuIdx) )
	{
		if( wMcuIdx == m_atConfOtherMcInfo[wMcuIdx].GetMcuIdx() )
		{
			return m_atConfOtherMcInfo[wMcuIdx].m_byIsSupMultSpy;
		}
	}

	return FALSE;	
}

BOOL32  TConfOtherMcTable::GetMultiSpyBW( const u16 wMcuIdx, u32 &dwMaxSpyBW, s32 &nRemainSpyBW)
{
	if( IsValidSubMcuId(wMcuIdx) )
	{
		if( wMcuIdx == m_atConfOtherMcInfo[wMcuIdx].GetMcuIdx() )
		{
			if (m_atConfOtherMcInfo[wMcuIdx].m_byIsSupMultSpy == 1)
			{
				dwMaxSpyBW = m_atConfOtherMcInfo[wMcuIdx].m_dwMaxSpyBW;
				nRemainSpyBW = m_atConfOtherMcInfo[wMcuIdx].m_nRemainSpyBW;
				return TRUE;
			}
			else
			{
				return FALSE;
			}
				
		}
	}

	return FALSE;	
}

BOOL32  TConfOtherMcTable::SetMcuSupMultSpyMaxBW( const u16 wMcuIdx, const u32 dwMaxSpyBW)
{
	if( IsValidSubMcuId(wMcuIdx) )
	{
		if( wMcuIdx == m_atConfOtherMcInfo[wMcuIdx].GetMcuIdx() )
		{
			if (m_atConfOtherMcInfo[wMcuIdx].m_byIsSupMultSpy == 1)
			{
				m_atConfOtherMcInfo[wMcuIdx].m_dwMaxSpyBW = dwMaxSpyBW;
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}

	return FALSE;
}

BOOL32  TConfOtherMcTable::SetMcuSupMultSpyRemainBW( const u16 wMcuIdx, const s32 nRemainSpyBW)
{
	if( IsValidSubMcuId(wMcuIdx) )
	{
		if( wMcuIdx == m_atConfOtherMcInfo[wMcuIdx].GetMcuIdx() )
		{
			if (m_atConfOtherMcInfo[wMcuIdx].m_byIsSupMultSpy == 1)
			{
				m_atConfOtherMcInfo[wMcuIdx].m_nRemainSpyBW = nRemainSpyBW;
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}

	return FALSE;
}

/*=============================================================================
�� �� ���� SetMcuSupMultSpy
��    �ܣ� ��־ĳ���¼���mcu֧�ֶ�ش�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byMcuId 
�� �� ֵ�� void  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
20100106  4.0			pengjie                create
=============================================================================*/
BOOL32  TConfOtherMcTable::SetMcuSupMultSpy( const u16 wMcuIdx )
{
	if( IsValidSubMcuId(wMcuIdx) )
	{
		if( wMcuIdx == m_atConfOtherMcInfo[wMcuIdx].GetMcuIdx() )
		{
			m_atConfOtherMcInfo[wMcuIdx].m_byIsSupMultSpy = TRUE;
			return TRUE;
		}
	}

	return FALSE;	
}

/*=============================================================================
�� �� ���� IsMcuSupMultCas
��    �ܣ� �ж�ĳ���¼�mcu�Ƿ�֧�ֶ༶��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u16 wMcuIdx 
�� �� ֵ�� BOOL32  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2010/07/09  4.6			xueliang                create
=============================================================================*/
BOOL32 TConfOtherMcTable::IsMcuSupMultCas( const u16 wMcuIdx )
{
	if( IsValidSubMcuId(wMcuIdx) )
	{
		if( wMcuIdx == m_atConfOtherMcInfo[wMcuIdx].GetMcuIdx() )
		{
			return  ( m_atConfOtherMcInfo[wMcuIdx].m_byIsSupMultCas == 1);
		}
	}

	return FALSE;
}

/*=============================================================================
�� �� ���� SetMcuSupMultCas
��    �ܣ� ��־ĳ���¼���mcu֧�ֶ༶��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u16 wMcuIdx 
�� �� ֵ�� BOOL32  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2010/07/09  4.6			xueliang                create
=============================================================================*/
BOOL32  TConfOtherMcTable::SetMcuSupMultCas( const u16 wMcuIdx )
{
	if( IsValidSubMcuId(wMcuIdx) )
	{
		if( wMcuIdx == m_atConfOtherMcInfo[wMcuIdx].GetMcuIdx() )
		{
			m_atConfOtherMcInfo[wMcuIdx].m_byIsSupMultCas = 1;
			return TRUE;
		}
		
		return FALSE;
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
	if (!lpszTable)
	{
		memset( tMAPParam, 0, byMapCount*sizeof(TMAPParam) );
		return FALSE;
	}

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
		INNER_FREE_MEM(lpszTable, dwMemEntryNum);
		return FALSE;
    }
	if( dwMemEntryNum != byMapCount )
	{
		memset( tMAPParam, 0, byMapCount*sizeof(TMAPParam) );
		INNER_FREE_MEM(lpszTable, dwMemEntryNum);
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
	INNER_FREE_MEM(lpszTable, dwMemEntryNum);
    
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
	case MEDIA_TYPE_G719:

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
        LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetActivePayload] unexpected media type.%d, ignore it\n", byRealPayloadType );
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
	    case MEDIA_TYPE_PCMA:   byAPayload = bNewActivePT ? ACTIVE_TYPE_PCMA : byRealPayloadType;  break;
	    case MEDIA_TYPE_PCMU:   byAPayload = bNewActivePT ? ACTIVE_TYPE_PCMU : byRealPayloadType;  break;
	    case MEDIA_TYPE_G721:   byAPayload = ACTIVE_TYPE_G721;  break;
	    case MEDIA_TYPE_G722:   byAPayload = bNewActivePT ? ACTIVE_TYPE_G722 : byRealPayloadType;  break;
	    case MEDIA_TYPE_G7231:  byAPayload = ACTIVE_TYPE_G7231; break;
	    case MEDIA_TYPE_G728:   byAPayload = bNewActivePT ? ACTIVE_TYPE_G728 : byRealPayloadType;  break;
	    case MEDIA_TYPE_G729:   byAPayload = bNewActivePT ? ACTIVE_TYPE_G729 : byRealPayloadType;  break;
	    case MEDIA_TYPE_G719:   byAPayload = bNewActivePT ? ACTIVE_TYPE_G719 : byRealPayloadType;  break;
		// Bug00121515 h261��ʽ���뻹�ǲ���ԭ�����غ�
		//zjj20131120 �ٴ��޸ļ���ʹ��ACTIVE_TYPE_H261
		case MEDIA_TYPE_H261:   byAPayload = bNewActivePT ? ACTIVE_TYPE_H261 : byRealPayloadType;  break;	//
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

/*=============================================================================
�� �� ���� GetActivePL
��    �ܣ� ��������� ��ȡ��̬�غɷ�װ
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
08/11/18    4.6         �ܼ���        ����
=============================================================================*/
u8 GetActivePL(const TConfInfo &tConfInfo, u8 byMediaType)
{
	// [7/25/2011 liuxu] ����,ɾ��
    // u8 byConfIdx = g_cMcuVcApp.GetConfIdx(tConfInfo.GetConfId());

    return GetActivePayload(tConfInfo, byMediaType);
}

// �ж�Դ�˷ֱ����Ƿ����Ŀ�Ķ˷ֱ���,�����򷵻�TRUE,С�ڵ����򷵻�FALSE
BOOL32 IsResG(u8 bySrcRes, u8 byDstRes)
{
    if (VIDEO_FORMAT_AUTO == bySrcRes ||
        VIDEO_FORMAT_AUTO == byDstRes)
    {
        return FALSE;
    }

	//20110503 zjl ˫�������ֶ���̬
//     if (!IsDSResMatched(bySrcRes, byDstRes))
//     {
//         return FALSE;
//     }

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
							VIDEO_FORMAT_UXGA,
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
        LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[IsResGE] unexpected res <src.%d, dst.%d>\n", bySrcRes, byDstRes);
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
	//Auto������
	case VIDEO_FORMAT_AUTO:
		bLive = TRUE;
		break;
	//DS������SQCIF
	case VIDEO_FORMAT_SQCIF_112x96:
	case VIDEO_FORMAT_SQCIF_96x80:
		break;
	default:
		break;
	}
	return bLive;
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
    2011/01/21  4.6			 liuxu                  ���lock
=============================================================================*/
SEMHANDLE	g_hStrOfIP = NULL;						// StrOfIPר��

char * StrOfIP( u32 dwIP )
{
	if (NULL == g_hStrOfIP)
	{
		if( !OspSemBCreate(&g_hStrOfIP))
		{
			OspSemDelete( g_hStrOfIP );
			g_hStrOfIP = NULL;
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[StrOfIP] create g_hStrOfIP failed!\n" );
		}
	}

	if (NULL != g_hStrOfIP)
	{
		OspSemTake(g_hStrOfIP);
	}	

    dwIP = htonl(dwIP);
	static char strIP[17];  
	u8 *p = (u8 *)&dwIP;
	sprintf(strIP,"%d.%d.%d.%d%c",p[0],p[1],p[2],p[3],0);

	if (NULL != g_hStrOfIP)
	{
		OspSemGive(g_hStrOfIP);
	}

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
#elif defined(_8KE_) 
	return MCU_TYPE_KDV8000E;	// [3/1/2010 xliang] ����8000E�ͺ�
#elif defined(_8KH_)	
	#ifdef _800L_
		return MCU_TYPE_KDV800L;
	#elif defined(_8KH_M_)
		return MCU_TYPE_KDV8000H_M;
	#else
		return MCU_TYPE_KDV8000H;
	#endif

#elif defined(_8KI_)
	return MCU_TYPE_KDV8000I;
#else
//     #ifdef WIN32
//         return MCU_TYPE_WIN32;
//     #else
//         return MCU_TYPE_KDV8000;
//     #endif
	return MCU_TYPE_KDV8000;
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
	// [pengjie 2010/6/3] �������ԭ��
    static s8* gs_szLeftReason[] = {"Unknown", "Exception", "Normal", "RTD", "DRQ", "TypeUnmatch", "Busy",
                                    "Reject", "Unreachable", "Local", "BusyExt", "remotereconnect", "confholding",
	                                "hascascaded"};

    if ( byReason >= MTLEFT_REASON_EXCEPT && byReason <= MTLEFT_REASON_REMOTEHASCASCADED )
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
	logenablemod(MID_MCU_DCS);
	//g_bPrtDcsMsg = TRUE;
}

API void npdcsmsg( void )
{
	logdisablemod(MID_MCU_DCS);
	//g_bPrtDcsMsg = FALSE;
}

//mcs���Դ�ӡ�ӿ�
BOOL32 g_bPrintMcsMsg = FALSE;
API void pmcsmsg( void )
{
	logenablemod(MID_MCU_MCS);
    g_bPrintMcsMsg = TRUE;
}

API void npmcsmsg( void )
{
	logdisablemod(MID_MCU_MCS);
    g_bPrintMcsMsg = FALSE;
}

//vcs���Դ�ӡ�ӿ�
BOOL32 g_bPrintVcsMsg = FALSE;
API void pvcsmsg( void )
{
	logenablemod(MID_MCU_MCS);
	logenablemod(MID_MCU_VCS);

    g_bPrintVcsMsg = TRUE;
	//ͬʱ��mcs����Ϣ��Ӧ��
    g_bPrintMcsMsg = TRUE;
}

API void npvcsmsg( void )
{
	logdisablemod(MID_MCU_MCS);
	logdisablemod(MID_MCU_VCS);
    g_bPrintVcsMsg = FALSE;
	//ͬʱ��mcs����Ϣ��Ӧ��
    g_bPrintMcsMsg = FALSE;
}

//eqp���Դ�ӡ�ӿ�
BOOL32 g_bPrintEqpMsg = FALSE;
API void peqpmsg( void )
{
	logenablemod(MID_MCU_EQP);
	logenablemod(MID_MCU_REC);
	logenablemod(MID_MCU_HDU);
	
	logenablemod(MID_MCU_VMP);
	logenablemod(MID_MCU_BAS);
	logenablemod(MID_MCU_PRS);

	logenablemod(MID_MCU_MIXER);
	
    g_bPrintEqpMsg = TRUE;
}

API void npeqpmsg( void )
{
	logdisablemod(MID_MCU_EQP);
	logdisablemod(MID_MCU_REC);
	logdisablemod(MID_MCU_HDU);

	logdisablemod(MID_MCU_VMP);
	logdisablemod(MID_MCU_BAS);
	logdisablemod(MID_MCU_PRS);
	
	logdisablemod(MID_MCU_MIXER);

    g_bPrintEqpMsg = FALSE;
}

//mmcu���Խӿ�
BOOL32 g_bpMMcuMsg = FALSE;
API void pmmcumsg(void)
{
	logenablemod(MID_MCU_MMCU);
	logenablemod(MID_MCU_SPY);
    g_bpMMcuMsg = TRUE;
}
API void npmmcumsg(void)
{
	logdisablemod(MID_MCU_MMCU);
	logdisablemod(MID_MCU_SPY);
    g_bpMMcuMsg = FALSE;
}

//mt
BOOL32 g_bPMt2Msg = FALSE;
API void pmt2msg( void )
{
	logenablemod(MID_MCU_MT2);
    g_bPMt2Msg = TRUE;
}

API void npmt2msg( void )
{
	logdisablemod(MID_MCU_MT2);
    g_bPMt2Msg = FALSE;
}

//mt call
BOOL32 g_bPrintCallMsg = FALSE;
API void pcallmsg( void )
{
	logenablemod(MID_MCU_CALL);
    g_bPrintCallMsg = TRUE;
}

API void npcallmsg( void )
{
	logdisablemod(MID_MCU_CALL);
    g_bPrintCallMsg = FALSE;
}

//mp
BOOL32 g_bpMpMgrMsg = FALSE;
API void pmpmgrmsg(void)
{
	logenablemod(MID_MCU_MPMGR);
    g_bpMpMgrMsg = TRUE;
}
API void npmpmgrmsg(void)
{
	logdisablemod(MID_MCU_MPMGR);
    g_bpMpMgrMsg = FALSE;
}

//cfg
BOOL32 g_bPrintCfgMsg = FALSE;
API void pcfgmsg( void )
{
	logenablemod(MID_MCU_CFG);
    g_bPrintCfgMsg = TRUE;
}

API void npcfgmsg( void )
{
	logdisablemod(MID_MCU_CFG);
    g_bPrintCfgMsg = FALSE;
}

//mt
BOOL32 g_bPrintMtMsg = FALSE;
API void pmtmsg( void )
{
	logenablemod(MID_MCU_MT);
    g_bPrintMtMsg = TRUE;
}

API void npmtmsg( void )
{
	logdisablemod(MID_MCU_MT);
    g_bPrintMtMsg = FALSE;
}

//guard
BOOL32 g_bPrintGdMsg = FALSE;
API void pgdmsg( void )
{
	logenablemod(MID_MCU_GUARD);
    g_bPrintGdMsg = TRUE;
}

API void npgdmsg( void )
{
	logdisablemod(MID_MCU_GUARD);
    g_bPrintGdMsg = FALSE;
}

BOOL32 g_bPrintNPlusMsg = FALSE;

API void pnplusmsg( void )
{
	logenablemod(MID_MCU_NPLUS);
    g_bPrintNPlusMsg = TRUE;
}

API void npnplusmsg( void )
{
	logdisablemod(MID_MCU_NPLUS);
    g_bPrintNPlusMsg = FALSE;
}

API void ppfmmsg( void )
{
	logenablemod(MID_MCU_PFM);
    g_bPrintPfmMsg = TRUE;
}

API void nppfmmsg( void )
{
	logdisablemod(MID_MCU_PFM);
    g_bPrintPfmMsg = FALSE;
}

API void sconftotemp( s8* psConfName )
{
	if ( psConfName == NULL )
	{
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "ERROR: The name is null!\n" );
		return;
	}

	CConfId cConfId = g_cMcuVcApp.GetConfIdByName( (LPCSTR)psConfName, FALSE,TRUE );
	if( cConfId.IsNull() )
	{
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "ERROR: The conference %s is not exist! The name maybe error!\n", psConfName );
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
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "Reset login info failed!\n" );
	}
	return;
}

//��ش����Դ�ӡ
BOOL32 g_bPrintCascadeSpyMsg = 0;

API void pspymsg( void )
{
	logenablemod(MID_MCU_SPY);
	g_bPrintCascadeSpyMsg = LOG_LVL_DETAIL+1;
}

API void npspymsg( void )
{
	logdisablemod(MID_MCU_SPY);
	g_bPrintCascadeSpyMsg = 0;
}

u8 FrameRateMac2Real(u8 byFrameRate)
{
	switch (byFrameRate)
	{
	case VIDEO_FPS_2997_1:	return 30;
	case VIDEO_FPS_25:		return 25;
	case VIDEO_FPS_2997_2:	return 15;
	case VIDEO_FPS_2997_3:	return 10;
	case VIDEO_FPS_2997_4:	return 8;
	case VIDEO_FPS_2997_5:	return 6;
	case VIDEO_FPS_2997_6:	return 5;
	case VIDEO_FPS_2997_30: return 1;
	case VIDEO_FPS_2997_7P5:return 4;
	case VIDEO_FPS_2997_10:	return 3;
	case VIDEO_FPS_2997_15:	return 2;

	default:				return 1;
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

	if ( wWidth == 1440 && wHeight == 816 )
	{
		return VIDEO_FORMAT_1440x816;
	}
	
	if ( wWidth == 960 && wHeight == 544 )
	{
		return VIDEO_FORMAT_960x544;
	}
	
	if ( wWidth == 640 && wHeight == 368 )
	{
		return VIDEO_FORMAT_640x368;
	}
	
	if ( wWidth == 480 && wHeight == 272 )
	{
		return VIDEO_FORMAT_480x272;
	}
	
	if ( wWidth == 384 && wHeight == 272 )
	{
		return VIDEO_FORMAT_384x272;
	}
	
	//�Ǳ�ֱ��ʣ�720p��ͼ��
	if ( wWidth == 864 && wHeight == 480 )
	{
		return VIDEO_FORMAT_720_864x480;
	}
	
	if( wWidth == 432 && wHeight == 240 )
	{
		return VIDEO_FORMAT_720_432x240;
	}
	
	if( wWidth == 320 && wHeight == 192 )
	{
		return VIDEO_FORMAT_720_320x192;
	}

	if( wWidth == 640 && wHeight == 544 )
	{
		return VIDEO_FORMAT_640x544;
	}

	if( wWidth == 320 && wHeight == 272 )
	{
		return VIDEO_FORMAT_320x272;
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
		
	case VIDEO_FORMAT_384x272:
		wWidth = 384;
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
	case VIDEO_FORMAT_640x544:
		wWidth = 640;
		wHeight =544;
		break;
	case VIDEO_FORMAT_320x272:
		wWidth = 320;
		wHeight =272;
		break;
    default:
        break;
    }
    if ( 0 == wHeight || 0 == wWidth )
    {
        LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetWHByRes] unexpected res.%d, ignore it\n", byRes );
    }
    return;
}

/*====================================================================
������           GetFormatByBitrate
����        ���������ʵõ��ֱ���
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u8 byResԭ�ֱ���
			  u16 wBitrate ����
����ֵ˵��  ��u8 ��Ӧ�ֱ���
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2011/06/30    4.0         zhangli        ����
====================================================================*/
u8 GetAutoResByBitrate(u8 byRes, u16 wBitrate)
{
	if (byRes != VIDEO_FORMAT_AUTO)
	{
		return VIDEO_FORMAT_INVALID;
	}
	if (wBitrate > 3072)
	{
		return VIDEO_FORMAT_4CIF;
	}
	else if (wBitrate > 1536)
	{
		return VIDEO_FORMAT_2CIF;
	}
	else
	{
		return VIDEO_FORMAT_CIF;
	}
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
������    :  WHCmp
����      :  ĳ�߿�ֱ�ͬ��һ�߿�Ƚ�
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  s32 
             ��߾�С���� F_WHNONELARGER
			 ��С�ߴ󷵻� F_HLARGER
			 ��߾���ȷ��� WHEQUAL
			 ����С���� F_WLARGER
			 ��߾��󷵻� F_WHLARGER
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-10-20                  Ѧ��                            ����
==============================================================================*/
s32 WHCmp(u16 wSrcWidth, u16 wSrcHeight, u16 wDstWidth, u16 wDstHeight)
{
	//�������
	//VIDEO_FORMAT_480x272 ����>�Ƚ�ʱͳһ���䵽480 x 288
	wSrcHeight = (wSrcHeight == 272) ? 288 : wSrcHeight;
	wDstHeight = (wDstHeight == 272) ? 288 : wDstHeight;
	
	//ǰ�߾������ں���
	if ( (wSrcWidth == wDstWidth) && (wSrcHeight == wDstHeight) )
	{
		return WHEQUAL;  //��߾���  (equal in width and height)
	}
	else if( ( wSrcWidth <= wDstWidth ) && (wSrcHeight <= wDstHeight) )
	{
		return F_WHNONELARGER; //��߾�С����ȸ�С����С�ߵ� (none bigger in both width and height)
	}
	//ǰ��������һ����ں���
	else if ( (wSrcWidth <= wDstWidth) && (wSrcHeight >= wDstHeight) )
	{
		return F_HLARGER; //��С�ߴ󣬿�ȸߴ�  (only bigger in height)
	}
	else if ( (wSrcWidth >= wDstWidth) && (wSrcHeight <= wDstHeight) )
	{
		return F_WLARGER;  //����С�����ߵ�  (only bigger in width)
	}
	else 
	{
		return F_WHLARGER; //��߾���			 (bigger both in width and height)
	}
}
/*==============================================================================
������    :  ResWHCmp
����      :  
�㷨ʵ��  :  
����˵��  :  u8 bySrcRes	[in]
			 u8 byDstRes    [in]
����ֵ˵��: s32 
			��߾�С���� F_WHNONELARGER
			��С�ߴ󷵻� F_HLARGER
			��߾���ȷ��� WHEQUAL
			����С���� F_WLARGER
			��߾��󷵻� F_WHLARGER
			���߱Ƚ������巵�� F_NOMATCH
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-2-20                  Ѧ��                            ����
==============================================================================*/
s32 ResWHCmp(u8 bySrcRes, u8 byDstRes)
{
	// vmp֧��˫������,˫���ֱ���ͬ����Ҫ�Ƚ�
	/*VGA��ķֱ��ʺͷ�VGA��ķֱ��ʱȽ�û������
	if(IsResCmpNoMeaning(bySrcRes,byDstRes))
	{
// 		return 3;
		return F_NOMATCH;
	}*/
	
	u16 wSrcWidth = 0;
	u16 wSrcHeight = 0;
	u16 wDstWidth = 0;
	u16 wDstHeight = 0;
	GetWHByRes(bySrcRes, wSrcWidth, wSrcHeight);
	GetWHByRes(byDstRes, wDstWidth, wDstHeight);
	
	return WHCmp(wSrcWidth, wSrcHeight, wDstWidth, wDstHeight);
}

/*==============================================================================
������    :  IsSrcResThanDst
����      :  
�㷨ʵ��  :  
����˵��  :  u8 bySrcRes	[in]
			 u8 byDstRes    [in]
����ֵ˵��: BOOL32 TRUE:Դ����Ŀ�ģ�FALSE��ԴС��Ŀ��
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
20130419                  chenbing                            ����
==============================================================================*/
BOOL32 IsSrcResThanDst(u8 bySrcRes, u8 byDstRes)
{
	if (  -1 == ResWHCmp(bySrcRes, byDstRes)
		|| 1 == ResWHCmp(bySrcRes, byDstRes)
		|| 2 == ResWHCmp(bySrcRes, byDstRes)
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
������    :  GetSuitableRes
����      :  
�㷨ʵ��  :  
����˵��  :  u8 byLmtRes	[i]	��Ҫ�ķֱ�������
u8 byOrgRes	[i] ԭʼ�ֱ���
����ֵ˵��:  u8					���ʵķֱ���
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-10-20                   Ѧ��
==============================================================================*/
u8 GetSuitableRes(u8 byLmtRes, u8 byOrgRes)
{
	//1.
	u16 wLmtWidth = 0;
	u16 wLmtHeight = 0;
	u16 wOrgWidth = 0;
	u16 wOrgHeight = 0;
	GetWHByRes(byLmtRes, wLmtWidth, wLmtHeight);
	GetWHByRes(byOrgRes, wOrgWidth, wOrgHeight);
	
	//4.
	return GetSuitableResByWH(wLmtWidth, wLmtHeight, wOrgWidth, wOrgHeight);	
}

/*==============================================================================
������    :  GetSuitableResByWH
����      :  
�㷨ʵ��  :  
����˵��  :  u8 byLmtRes	[i]	��Ҫ�ķֱ�������
u8 byOrgRes	[i] ԭʼ�ֱ���
����ֵ˵��:  u8					���ʵķֱ���
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2012-09-13                 yanghuaizhi
==============================================================================*/
u8 GetSuitableResByWH(u16 wLmtWidth, u16 wLmtHeight, u16 wOrgWidth, u16 wOrgHeight)
{
	u16 wMinWidth  = min(wLmtWidth, wOrgWidth);
	u16 wMinHeight = min(wLmtHeight,wOrgHeight);
	
	return GetMcuSupportedRes(wMinWidth, wMinHeight);
}

/*==============================================================================
������    :  GetDecAbility
����      :  ����ͬʱ��������ȡ��֮��Ӧ�Ľ�������
�㷨ʵ��  :  
����˵��  :  [in]TSimCapSet, �ն˻������ͬʱ������
			 
����ֵ˵��:  TBasChnData::DecAbility
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010/11/12                 liuxu                            ����
2011/02/09   4.7		   ��־��							�޸�
==============================================================================*/
const s16 GetDecAbility(const TSimCapSet& tSimCapSet)
{
	if ( tSimCapSet.GetVideoProfileType() == emHpAttrb )
	{
		return CBasChn::emDecHp;
	}
	else if(tSimCapSet.GetUserDefFrameRate() >= 50)
	{
		return CBasChn::emDecHighFps;
	}
	else if (MEDIA_TYPE_H264 == tSimCapSet.GetVideoMediaType())
	{
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
		if(IsResG(tSimCapSet.GetVideoResolution(), VIDEO_FORMAT_4CIF))
#else
		if(IsResGE(tSimCapSet.GetVideoResolution(), VIDEO_FORMAT_4CIF))
#endif
		{
			return CBasChn::emDecHD;
		}
		else
		{
			return CBasChn::emDecSD;
		}		
	}
	else
	{
		return CBasChn::emDecSD;
	}
}

/*==============================================================================
������    :  GetSwitchSndBindIp
����      :  ȡ�鲥�ķ��͵�ַ���������DataSwitch
ע��	  :  ֻ��8000H-M����Ч
�㷨ʵ��  :  ����route -n ���UGȡĬ��·�ɵ�IP��ַ
����˵��  :  ��
����ֵ˵��:  0: ��Ӱ����ǰ���
		   ��0: ��ǰ�ķ��� ������IP��ַ
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2013/06/5                  �����                            ����
==============================================================================*/
u32 GetSwitchSndBindIp()
{
	//1. ��8000H-M��һ�ɷ���0����ԭ��һ��
#ifdef _8KH_
	if ( !g_cMcuAgent.Is8000HmMcu() )
	{
		LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS, "[sorry it it not 8000H-M]\n");
		return 0;
	}

	// ֻ��ʾ��һ��Ĭ��·��[pengguofeng 6/5/2013]
#ifndef _LINUX_
#define __func__    "GetSwitchSndBindIp"
#define popen		_popen
#define pclose		_pclose
#endif
	s8 *pCmd = "ifconfig `route -n | grep UG | awk '{print $8}' | sed -n 1p` | grep \"inet addr\" | cut -d':' -f2 | cut -d' ' -f1";
	LogPrint(LOG_LVL_DETAIL, MID_PUB_ALWAYS, "[%s] run cmd is [%s]\n", __func__, pCmd);
	FILE *fp = popen(pCmd, "r");
	if ( fp)
	{
		s8 achIp[16] = {0}; //IP�ĳ���
		while(fgets(achIp, 16, fp) )
		{
			u8 byLen = strlen(achIp);
			if ( byLen == 0 )
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[%s]get ip content is Null\n", __func__);
				continue;
			}
			if ( achIp[byLen-1] == '\n')
			{
				achIp[byLen-1] = 0;
			}
			LogPrint(LOG_LVL_DETAIL, MID_PUB_ALWAYS, "[%s] get fist UG ip: [%s]\n", __func__, achIp);
			break;
		}
		pclose(fp);
		return ntohl(INET_ADDR(achIp));
	}
	else
	{
		LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS, "[%s]seach IP failed\n", __func__);
		return 0;
	}
#else
	return 0; //��8000Hƽ̨���ÿ��ǣ�Ҳ����0
#endif
}

/*==============================================================================
������    :  GetMtPosInMtArray
����      :  ���ն��б�ptMtList�л�ȡ�����ն�tSpecMt������λ��
�㷨ʵ��  :  
����˵��  :  [in]tSpecMt  -- ��Ѱ�ҵ��ն�
			 [in]ptMtList -- �ն��б�
			 [in]byMtNum  -- �ն��б���Ŀ
����ֵ˵��:  �ն�tSpecMt����ptMtList�е�����λ��[0��byMtNum), û�ҵ�����-1
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2011/06/13                 liuxu                            ����
==============================================================================*/
const s16 GetMtPosInMtArray( const TMtAlias& tSpecMt, const TMtAlias* ptMtList, const u8 byMtNum )
{
	s16 swRet = -1;									// ԭʼ����ֵ
	
	if ( !ptMtList || byMtNum == 0 )
	{
		return swRet;
	}

	for ( u8 byLoop = 0; byLoop < byMtNum; ++byLoop)
	{
		if (tSpecMt == ptMtList[byLoop])
		{
			swRet = byLoop;
			break;
		}
	}

	return swRet;
}

/*==============================================================================
������    :  GetConfExtraDataLenFromFile
����      :  ��û����ļ��������ݴ�С������ֻ�����vcs�����еĵ���ǽԤ��
�㷨ʵ��  :  
����˵��  :  u8			byChnnl		[in]
			 TConfInfo	tConfInfo	[in]
			 u8			bySubVmpType[in]
			 u8			byMediaType	[out]
			 
����ֵ˵��:  u8			byRes
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-11-02                  �ܾ���                            ����
==============================================================================*/
BOOL32 GetConfExtraDataLenFromFile( TConfStore tConfStore, u16 &wPlanDataLen,  u16 &wExInfoLen, BOOL32 bIsDefaultConf )
{
	FILE *hConfFile = NULL;
	s8    achFullName[64];
	s32   nFileLen = 0;
	u16	  wOutBufLen = 0;
	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];


	u8 byConfPos = MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE;
	//ȱʡ����ֱ��ȡMAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE��ΪIdx[7/17/2012 chendaiwei]
	if(!bIsDefaultConf)
	{
		//1.��ͷ��Ϣ��¼�� ��ȡ �����ģ������λ��
		GetAllConfHeadFromFile(acConfId, sizeof(acConfId));

		//�������л��飺�˻����ѱ��棬���ǣ�������ȱʡ����λ��
		// modify ����ȱʡ����λ�� [7/16/2012 chendaiwei]
		for (s32 nPos = 0; nPos < (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE); nPos++)
		{
			if (acConfId[nPos] == tConfStore.m_tConfInfo.GetConfId())
			{
				byConfPos = (u8)nPos;
				break;
			}
		}

		if( (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE) == byConfPos)
		{
			return FALSE;
		}
	}

	//��ȡ ָ�������Ļ����ģ����Ϣ �洢�ļ� ��Ϣ
    sprintf(achFullName, "%s/%s%d%s", DIR_DATA, CONFINFOFILENAME_PREFIX, byConfPos, CONFINFOFILENAME_POSTFIX);

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
		//wFileTConfInfoSize = ntohs(wFileTConfInfoSize);
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
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetConfDataFromFile] In new version, FileTConfInfoSize can't be 0!\n");
		fclose(hConfFile);
		EndRWConfInfoFile();
		hConfFile = NULL;
		return FALSE;
	}

	u8 *pbyBuf = NULL;

	if( !bIsNewVersion )
	{		
		fclose(hConfFile);
		hConfFile = NULL;		
		wPlanDataLen = 0;
		wExInfoLen = 0;
		EndRWConfInfoFile();
		return FALSE;
	}
	// confinfo size notify, zgc, 20070524
	else
	{
		
		TConfStore tBufConfStore;
		u16 wPackConfDataLen = 0;
		if (FALSE == PackConfStore(tConfStore, (TPackConfStore*)&tBufConfStore, wPackConfDataLen))
		{
			fclose(hConfFile);
			hConfFile = NULL;		
			wPlanDataLen = 0;
			wExInfoLen = 0;
			EndRWConfInfoFile();
			return FALSE;
		}

		if(IsV4R6B2VcsTemplate(hConfFile,(u16)nFileLen))
		{
			// fix2ģ�����ٸ�u8 [11/13/2012 chendaiwei]
			wPackConfDataLen--;
		}

		if( wOutBufLen <= wPackConfDataLen )
		{
			fclose(hConfFile);
			hConfFile = NULL;		
			wPlanDataLen = 0;
			wExInfoLen = 0;
			EndRWConfInfoFile();
			return FALSE;
		}

		pbyBuf = new u8[ sizeof(TPackConfStore) ];
		memset( pbyBuf,0,sizeof( pbyBuf ) );

		u16 wRemainFileLen = wOutBufLen;

		if( wRemainFileLen >= wPackConfDataLen && wPackConfDataLen >= sizeof(TPackConfStore))
		{
			fread( pbyBuf,sizeof(TPackConfStore),1,hConfFile );
			fseek(hConfFile,wPackConfDataLen - sizeof(TPackConfStore),SEEK_CUR);
			wRemainFileLen -= wPackConfDataLen;
		}
		else
		{
			fclose(hConfFile);
			hConfFile = NULL;		
			wPlanDataLen = 0;
			wExInfoLen = 0;
			EndRWConfInfoFile();
			SAFE_DEL_ARRAY(pbyBuf);

			return FALSE;
		}

		BOOL32 bIgnoreUnpack = FALSE;
		if( wRemainFileLen == 0)
		{
			LogPrint( LOG_LVL_WARNING, MID_PUB_ALWAYS, "[GetConfExtraDataLenFromFile] wRemainFileLen == 0, v4r6 template(%s) without plan!\n",tConfStore.m_tConfInfo.GetConfName());

			bIgnoreUnpack = TRUE;
		}
		else if( wRemainFileLen >= sizeof(u16) )
		{
			fread(&wPlanDataLen,sizeof(u16),1,hConfFile);
			wRemainFileLen-= sizeof(u16);
		}
		else
		{
			fclose(hConfFile);
			hConfFile = NULL;		
			wPlanDataLen = 0;
			wExInfoLen = 0;
			EndRWConfInfoFile();
			SAFE_DEL_ARRAY(pbyBuf);

			return FALSE;
		}

		if( !bIgnoreUnpack )
		{
			wPlanDataLen = ntohs(wPlanDataLen);
			if( wRemainFileLen >= wPlanDataLen )
			{
				fseek(hConfFile,wPlanDataLen,SEEK_CUR);
				wRemainFileLen-= wPlanDataLen;
			}
			else
			{
				fclose(hConfFile);
				hConfFile = NULL;		
				wPlanDataLen = 0;
				wExInfoLen = 0;
				EndRWConfInfoFile();
				SAFE_DEL_ARRAY(pbyBuf);

				return FALSE;
			}
			
			if( wRemainFileLen == 0)
			{
				LogPrint( LOG_LVL_WARNING, MID_PUB_ALWAYS, "[GetConfExtraDataLenFromFile] wRemainFileLen == 0, v4r6 template(%s) has plan without ex-info!\n",tConfStore.m_tConfInfo.GetConfName());
				
				bIgnoreUnpack = TRUE;
			}
			else if( wRemainFileLen >= sizeof(u16) )
			{
				fread(&wExInfoLen,sizeof(u16),1,hConfFile);
				wRemainFileLen -= sizeof(u16);
			}
			else
			{
				fclose(hConfFile);
				hConfFile = NULL;		
				wExInfoLen = 0;
				EndRWConfInfoFile();
				SAFE_DEL_ARRAY(pbyBuf);

				return FALSE;
			}
			
			if( !bIgnoreUnpack )
			{
				wExInfoLen = ntohs(wExInfoLen);
				if(wRemainFileLen >= wExInfoLen )
				{
					wExInfoLen = wExInfoLen + sizeof(u16);
				}
				else
				{
					fclose(hConfFile);
					hConfFile = NULL;		
					wExInfoLen = 0;
					EndRWConfInfoFile();
					SAFE_DEL_ARRAY(pbyBuf);

					return FALSE;
				}
			}

		}
		
		fclose( hConfFile );
		hConfFile = NULL;			
	}
	// confinfo size notify end

	//У�� �û����ģ�� ͷ��Ϣ
	TPackConfStore *ptPackConfStore = (TPackConfStore *)pbyBuf;

	if (!(tConfStore.m_tConfInfo.GetConfId() == ptPackConfStore->m_tConfInfo.GetConfId()))
	{
		SAFE_DEL_ARRAY(pbyBuf);	
		wPlanDataLen = 0;
		wExInfoLen = 0;
        EndRWConfInfoFile();
		return FALSE;
	}

	SAFE_DEL_ARRAY(pbyBuf);
    EndRWConfInfoFile();

	return TRUE;
}
/*==============================================================================
������    :  GetConfExtraDataFromFile
����      :  ��û����ļ��������ݣ�����ֻ�����vcs�����еĵ���ǽԤ��
�㷨ʵ��  :  
����˵��  :  u8			byChnnl		[in]
			 TConfInfo	tConfInfo	[in]
			 u8			bySubVmpType[in]
			 u8			byMediaType	[out]
			 
����ֵ˵��:  u8			byRes
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-11-02                  �ܾ���                            ����
==============================================================================*/
BOOL32 GetConfExtraDataFromFile( TConfStore tConfStore, u8 *pbyBuf,u16 wBufInLen, u8 *pbyExInfoBuf, u16 wExInfoBufLen,BOOL32 &bHasFix2PlanData,BOOL32 bIsDefaultConf)
{
/*lint -save -esym(429, pbyBuf)*/
/*lint -save -esym(429, pbyExInfoBuf)*/
	
	FILE *hConfFile = NULL;
	s8    achFullName[64];
	s32   nFileLen = 0;
	u16	  wOutBufLen = 0;
	u16   wDataLen = 0;
	u16   wExInfoLen = 0; //��չ��Ϣ���� [8/16/2012 chendaiwei]

	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];

	//ȱʡ����ֱ��ȡMAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE��ΪIdx[7/17/2012 chendaiwei]
	u8 byConfPos = MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE;
	if( !bIsDefaultConf )
	{
		//1.��ͷ��Ϣ��¼�� ��ȡ �����ģ������λ��
		GetAllConfHeadFromFile(acConfId, sizeof(acConfId));

		//�������л��飺�˻����ѱ��棬���ǣ�������ȱʡ����λ��
		for (s32 nPos = 0; nPos <(MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE); nPos++)
		{
			if (acConfId[nPos] == tConfStore.m_tConfInfo.GetConfId())
			{
				byConfPos = (u8)nPos;
				break;
			}
		}

		if( (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE) == byConfPos)
		{
			return FALSE;
		}
	}

	//��ȡ ָ�������Ļ����ģ����Ϣ �洢�ļ� ��Ϣ
    sprintf(achFullName, "%s/%s%d%s", DIR_DATA, CONFINFOFILENAME_PREFIX, byConfPos, CONFINFOFILENAME_POSTFIX);

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
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetConfDataFromFile] In new version, FileTConfInfoSize can't be 0!\n");
		fclose(hConfFile);
		EndRWConfInfoFile();
		hConfFile = NULL;
		wDataLen = 0;
		return FALSE;
	}


	// confinfo size notify end
	u8 *pbyConfStoreBuf = NULL;
	if( !bIsNewVersion )
	{		
		fclose(hConfFile);
		hConfFile = NULL;		
		wDataLen = 0;
		EndRWConfInfoFile();
		return FALSE;
	}
	// confinfo size notify, zgc, 20070524
	else
	{
		TConfStore tBufConfStore;
		u16 wPackConfDataLen = 0;
		if (FALSE == PackConfStore(tConfStore, (TPackConfStore*)&tBufConfStore, wPackConfDataLen))
		{
			fclose(hConfFile);
			hConfFile = NULL;		
			wDataLen = 0;
			EndRWConfInfoFile();
			return FALSE;
		}

		BOOL32 bIsFix2Or8000HTemplate = FALSE;
		if(IsV4R6B2VcsTemplate(hConfFile,(u16)nFileLen))
		{
			// fix2ģ�����ٸ�u8 [11/13/2012 chendaiwei]
			wPackConfDataLen--;
			bIsFix2Or8000HTemplate = TRUE;
		}

		//û�ж�������
		if( wOutBufLen <= wPackConfDataLen )
		{
			fclose(hConfFile);
			hConfFile = NULL;		
			wDataLen = 0;
			EndRWConfInfoFile();
			return FALSE;
		}

		//��ȥԤ������ͷ��2�ֽڵ�buf��С
		//wDataLen = wOutBufLen - wPackConfDataLen - sizeof(u16);

		pbyConfStoreBuf = new u8[ sizeof(TPackConfStore) ];
		memset( pbyConfStoreBuf,0,sizeof( pbyConfStoreBuf ) );
		fread( pbyConfStoreBuf,sizeof(TPackConfStore),1,hConfFile );

		if( 0 != fseek( hConfFile,wPackConfDataLen - sizeof(TPackConfStore),SEEK_CUR ) )
		{
			SAFE_DEL_ARRAY(pbyConfStoreBuf);
			fclose(hConfFile);
			hConfFile = NULL;		
			wDataLen = 0;
			EndRWConfInfoFile();
			return FALSE;
		}

		u16 wRemainFileLen = 0;
		if( wOutBufLen >= wPackConfDataLen )
		{
			wRemainFileLen = wOutBufLen - wPackConfDataLen;
		}
		else
		{
			SAFE_DEL_ARRAY(pbyConfStoreBuf);
			fclose(hConfFile);
			hConfFile = NULL;		
			wDataLen = 0;
			EndRWConfInfoFile();
			return FALSE;
		}

		BOOL32 bUnPackData = FALSE;
		if( wRemainFileLen == 0 )
		{
			bUnPackData = TRUE;
			LogPrint( LOG_LVL_WARNING, MID_PUB_ALWAYS, "[GetConfExtraDataFromFile] wRemainFileLen == 0, v4r6 template(%s) without plan!\n",tConfStore.m_tConfInfo.GetConfName());
		}
		else if( wRemainFileLen >= sizeof(u16))
		{
			fread(&wDataLen,sizeof(u16),1,hConfFile);
			wDataLen = ntohs(wDataLen);
			wRemainFileLen -= sizeof(u16);
		}
		else
		{
			SAFE_DEL_ARRAY(pbyConfStoreBuf);
			fclose(hConfFile);
			hConfFile = NULL;		
			wDataLen = 0;
			EndRWConfInfoFile();
			return FALSE;
		}
		
		//����
		if( wDataLen >  wBufInLen)
		{
			SAFE_DEL_ARRAY(pbyConfStoreBuf);
			fclose(hConfFile);
			hConfFile = NULL;		
			wDataLen = 0;
			EndRWConfInfoFile();
			return FALSE;
		}
		
		if( wRemainFileLen >= wDataLen)
		{
			fread(pbyBuf,wDataLen,1,hConfFile);
			wRemainFileLen -= wDataLen;
			//��ʶ�Ƿ���Fix2ģ���Ԥ������[11/14/2012 chendaiwei]
			bHasFix2PlanData = bIsFix2Or8000HTemplate;
		}
		else
		{
			SAFE_DEL_ARRAY(pbyConfStoreBuf);
			fclose(hConfFile);
			hConfFile = NULL;		
			wDataLen = 0;
			EndRWConfInfoFile();
			return FALSE;
		}

		if( !bUnPackData )
		{
			if ( wRemainFileLen == 0 )
			{
				bUnPackData = TRUE;
				LogPrint( LOG_LVL_WARNING, MID_PUB_ALWAYS, "[GetConfExtraDataFromFile] wRemainFileLen == 0, v4r6 template(%s) has plan without ex-info!\n",tConfStore.m_tConfInfo.GetConfName());
			}
			else if( wRemainFileLen >= sizeof(u16))
			{
				fread(&wExInfoLen,sizeof(u16),1,hConfFile);
				wRemainFileLen -= sizeof(u16);
				wExInfoLen = ntohs(wExInfoLen);
			}
			else
			{
				SAFE_DEL_ARRAY(pbyConfStoreBuf);
				fclose(hConfFile);
				hConfFile = NULL;		
				wDataLen = 0;
				EndRWConfInfoFile();
				return FALSE;
			}
			
			//����
			if(  wExInfoLen!= 0 && wExInfoLen + sizeof(u16) >  wExInfoBufLen )
			{
				SAFE_DEL_ARRAY(pbyConfStoreBuf);
				fclose(hConfFile);
				hConfFile = NULL;		
				EndRWConfInfoFile();
				return FALSE;
			}
			
			if( !bUnPackData )
			{
				if(wRemainFileLen >= wExInfoLen)
				{
					memcpy(pbyExInfoBuf,&wExInfoLen,sizeof(u16));
					fread(pbyExInfoBuf+sizeof(u16),wExInfoLen,1,hConfFile);
				}
				else
				{
					SAFE_DEL_ARRAY(pbyConfStoreBuf);
					fclose(hConfFile);
					hConfFile = NULL;		
					wDataLen = 0;
					EndRWConfInfoFile();
					return FALSE;
				}
			}
		}

		fclose( hConfFile );
		hConfFile = NULL;
		
	}
	// confinfo size notify end

	//У�� �û����ģ�� ͷ��Ϣ
	TPackConfStore *ptPackConfStore = (TPackConfStore *)pbyConfStoreBuf;

	if (!(tConfStore.m_tConfInfo.GetConfId() == ptPackConfStore->m_tConfInfo.GetConfId()))
	{
		SAFE_DEL_ARRAY(pbyConfStoreBuf);
		wOutBufLen = 0;
        EndRWConfInfoFile();
		return FALSE;
	}

	SAFE_DEL_ARRAY(pbyConfStoreBuf);
	
    EndRWConfInfoFile();

	return TRUE;

/*lint -restore*/
/*lint -restore*/
}

/*==============================================================================
������    :  SaveConfExtraTvWallPlanDataToFile
����      :  ��û����ļ��������ݣ�����ֻ�����vcs�����еĵ���ǽԤ��
�㷨ʵ��  :  
����˵��  :  u8			byChnnl		[in]
			 TConfInfo	tConfInfo	[in]
			 u8			bySubVmpType[in]
			 u8			byMediaType	[out]
			 
����ֵ˵��:  u8			byRes
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-11-02                  �ܾ���                            ����
==============================================================================*/
BOOL32 SaveConfExtraPlanDataToFile( TConfStore &tConfStore, u8 *pbyBuf,u16 wDataLen, BOOL32 bIsDefaultConf )
{
/*lint -save -esym(593, pbyBuf)*/

	FILE *hConfFile = NULL;
	s8    achFullName[64];
	s32   nFileLen = 0;
	u16	  wOutBufLen = 0;
//	u16	  wExtraDataLen = 0;

	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];


	u8 byConfPos = MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE;
	if( !bIsDefaultConf )
	{
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

		if( (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE) == byConfPos)
		{
			return FALSE;
		}
	}

	//��ȡ ָ�������Ļ����ģ����Ϣ �洢�ļ� ��Ϣ
    sprintf(achFullName, "%s/%s%d%s", DIR_DATA, CONFINFOFILENAME_PREFIX, byConfPos, CONFINFOFILENAME_POSTFIX);

	LogPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[SaveConfExtraPlanDataToFile tick.%d] save conf to file:%s\n", OspTickGet(), achFullName);

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
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetConfDataFromFile] In new version, FileTConfInfoSize can't be 0!\n");
		fclose(hConfFile);
		EndRWConfInfoFile();
		hConfFile = NULL;
		return FALSE;
	}

	
	// confinfo size notify end
	//u8 *pbyConfStoreBuf = NULL;
	if( !bIsNewVersion )
	{		
		fclose(hConfFile);
		hConfFile = NULL;		
		EndRWConfInfoFile();
		return FALSE;
	}
	// confinfo size notify, zgc, 20070524
	else
	{

		TConfStore tBufConfStore;
		u16 wPackConfDataLen = 0;

		if (FALSE == PackConfStore(tConfStore, (TPackConfStore*)&tBufConfStore, wPackConfDataLen))
		{
			fclose(hConfFile);
			hConfFile = NULL;		
			EndRWConfInfoFile();
			return FALSE;
		}


// 		if( wOutBufLen > wPackConfDataLen )
// 		{
// 			wExtraDataLen = wOutBufLen - wPackConfDataLen;
// 		}		


		u16 wPlanBufLen = wDataLen;

		{
			if( 0 != fseek(hConfFile, 0, SEEK_SET) )
			{
				//delete []pbyConfStoreBuf;
				fclose( hConfFile );
				hConfFile = NULL;
				return FALSE;
			}

			u16 byOrginalDataLen = wPackConfDataLen+strlen(CONFFILEHEAD) + sizeof(u16) + sizeof(u32);

			u8 *pbyOrginalBuf = new u8[byOrginalDataLen];
			fread( pbyOrginalBuf,byOrginalDataLen,1,hConfFile );
			fclose( hConfFile );
			
			hConfFile = fopen(achFullName, "wb"); 
			if (NULL == hConfFile)
			{
				//delete []pbyConfStoreBuf;
				delete[] pbyOrginalBuf;
				wOutBufLen = 0;
				EndRWConfInfoFile();
				return FALSE;
			}
			fclose( hConfFile );

			hConfFile = fopen(achFullName, "ab");
			if (NULL == hConfFile)
			{
				//delete []pbyConfStoreBuf;
				delete[] pbyOrginalBuf;
				wOutBufLen = 0;
				EndRWConfInfoFile();
				return FALSE;
			}

			fwrite( pbyOrginalBuf,byOrginalDataLen,1,hConfFile );

			delete []pbyOrginalBuf;

			//ʼ��д��Ԥ������u16��������0��[8/16/2012 chendaiwei]
			wPlanBufLen = htons( wPlanBufLen );
			fwrite( &wPlanBufLen,sizeof(wPlanBufLen),1,hConfFile );
			if( wDataLen > 0  && NULL != pbyBuf )
			{
				fwrite( pbyBuf,wDataLen,1,hConfFile );
			}

			//д����չ��Ϣ[8/16/2012 chendaiwei]
			u16 wExInfoLen = ntohs(*(u16*)&tConfStore.m_byConInfoExBuf[0]);
			if( wExInfoLen + sizeof(u16) <= CONFINFO_EX_BUFFER_LENGTH)
			{
				fwrite( &tConfStore.m_byConInfoExBuf[0],wExInfoLen+sizeof(u16),1,hConfFile);
			}
			else
			{
				fclose( hConfFile );
				EndRWConfInfoFile();
				return FALSE;
			}
		}
		
		fclose( hConfFile );
		hConfFile = NULL;
		
	}
	// confinfo size notify end

	//У�� �û����ģ�� ͷ��Ϣ
	/*TPackConfStore *ptPackConfStore = (TPackConfStore *)pbyConfStoreBuf;

	if (!(tConfStore.m_tConfInfo.GetConfId() == ptPackConfStore->m_tConfInfo.GetConfId()))
	{
		delete []pbyConfStoreBuf;
		wOutBufLen = 0;
        EndRWConfInfoFile();
		return FALSE;
	}
	*/

	//delete []pbyConfStoreBuf;
	
    EndRWConfInfoFile();

	return TRUE;

/*lint -restore*/
}

/*==============================================================================
������    : GetMtVidLmtForHd 
����      : �õ��ն˲���VMP�ֱ����������ֵ�����ڸ����նˣ� 
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-2-20	4.6				Ѧ��							create
==============================================================================*/
BOOL32 CVmpMemVidLmt::GetMtVidLmtForHd ( u8 byStyle, u8 byMemPos, u8 &byMtRes, u16 &wMtBandWidth)
{
	
	if( byStyle >= VMPSTYLE_NUMBER || byMemPos >= MAXNUM_VMP_MEMBER)
	{
		return FALSE;
	}

	BOOL32 bRet = TRUE;
	byMtRes = m_atMtVidLmtHd[byStyle][byMemPos].GetMtVidRes();
	wMtBandWidth = m_atMtVidLmtHd[byStyle][byMemPos].GetMtVidBw();
	if(byMtRes == 0 || wMtBandWidth == 0)
	{
		bRet = FALSE;
	}
	return bRet;
}

/*==============================================================================
������    : GetMtVidLmtForStd 
����      : �õ��ն˲���VMP�ֱ����������ֵ�����ڱ����նˣ�
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  BOOL32
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-2-20	4.6				Ѧ��							create
==============================================================================*/
BOOL32 CVmpMemVidLmt::GetMtVidLmtForStd ( u8 byStyle, u8 byMemPos, u8 &byMtRes, u16 &wMtBandWidth)
{

	if( byStyle >= VMPSTYLE_NUMBER || byMemPos >= MAXNUM_VMP_MEMBER)
	{
		return FALSE;
	}

	BOOL32 bRet = TRUE;
	byMtRes = m_atMtVidLmtStd[byStyle][byMemPos].GetMtVidRes();
	wMtBandWidth = m_atMtVidLmtStd[byStyle][byMemPos].GetMtVidBw();
	if(byMtRes == 0 || wMtBandWidth == 0)
	{
		bRet = FALSE;
	}
	return bRet;
}

/*==============================================================================
����    :  CSmsControl
����    :  ����Ϣ���Ϳ���
��Ҫ�ӿ�:  
��ע    :  
-------------------------------------------------------------------------------
�޸ļ�¼:  
��  ��     �汾          �޸���          �߶���          �޸ļ�¼
2011-1-19                Ѧ��							 create
==============================================================================*/
CSmsControl::CSmsControl()
{
	Init();
}

void CSmsControl::Init()
{
	m_cServMsg.Init();
	m_wTimerSpan = 200;// 200ms
	m_wPos = 0;
	m_byState = IDLE;
}

void CSmsControl::SetMtPos(u16 wPos )
{
	m_wPos = wPos;
}

u16	CSmsControl::GetMtPos(void) const
{
	return m_wPos;
}

void CSmsControl::SetState(u8 byState)
{
	m_byState = byState;
}

BOOL32 CSmsControl::IsStateIdle(void) const
{
	return (m_byState == IDLE);
}

void CSmsControl::SetServMsg(const CServMsg &cServMsg)
{
	memcpy(&m_cServMsg, &cServMsg, sizeof(cServMsg));
}

CServMsg * CSmsControl::GetServMsg(void)
{
	return &m_cServMsg;
}

void CSmsControl::SetTimerSpan(u16 wTimerSpan)
{
	m_wTimerSpan = wTimerSpan;
}

u16 CSmsControl::GetTimerSpan(void) const
{
	return m_wTimerSpan;
}

/*==============================================================================
����    :  CVmpMemVidLmt
����    :  ����ϳɳ�Ա��Ƶ�ֱ��ʵ���������
��Ҫ�ӿ�:  
��ע    :  
-------------------------------------------------------------------------------
�޸ļ�¼:  
��  ��     �汾          �޸���          �߶���          �޸ļ�¼
2009-2-26
==============================================================================*/
void CVmpMemVidLmt::Init( void )
{
	u8 byStyle = 0;
	u8 byLoop = 0;
	memset( m_atMtVidLmtHd, 0, sizeof( m_atMtVidLmtHd ) );
	memset( m_atMtVidLmtStd, 0, sizeof( m_atMtVidLmtStd ) );
	const u16 wBW_8M = 8192;
	const u16 wBW_3M = 3072;
	const u16 wBW_1d5M = 1536;

#if !defined(_8KE_)
	byStyle = VMP_STYLE_ONE;	//1����
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_HD1080);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);
	
	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4SIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);
	
	byStyle = VMP_STYLE_VTWO;	//����2����
	for(byLoop = 0; byLoop < 2;byLoop ++)
	{
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_960x544);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_8M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_4SIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_8M);
	}
	
	
	byStyle = VMP_STYLE_HTWO;	//һ��һС2����
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_HD1080);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);
	m_atMtVidLmtHd[byStyle][1].SetMtVidRes(VIDEO_FORMAT_640x368);
	m_atMtVidLmtHd[byStyle][1].SetMtVidBW(wBW_3M);
	
	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4SIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);
	m_atMtVidLmtStd[byStyle][1].SetMtVidRes(VIDEO_FORMAT_CIF);
	m_atMtVidLmtStd[byStyle][1].SetMtVidBW(wBW_1d5M);
	
	byStyle = VMP_STYLE_THREE;	//3����
	for(byLoop = 0; byLoop < 3; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_960x544);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_8M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_4SIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_8M);
	}
	
	
	byStyle = VMP_STYLE_FOUR;	//4����
	for(byLoop = 0; byLoop < 4; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_960x544);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_8M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_4SIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_8M);
	}
	
	
	byStyle = VMP_STYLE_SIX;	//6����
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_HD720);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);
	
	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4SIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);
	
	for(byLoop = 1; byLoop < 6; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_640x368);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_3M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	
	
	byStyle = VMP_STYLE_EIGHT;	//8����
	// ֮ǰ��mpu���ܽ�8����0ͨ���ֱ��ʸ�Ϊ720��Ӱ�쵽�����ϳ����裬���ڳ�ʼ��ʱ���˴���
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_1440x816);
	//m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_HD720);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);
	
	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4SIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);
	
	for(byLoop = 1; byLoop < 8; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_480x272);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	
	
	byStyle = VMP_STYLE_NINE;	//9����
	for(byLoop = 0; byLoop < 9; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_640x368);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_3M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	
				
	byStyle = VMP_STYLE_TEN;	//10����
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_960x544);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);
	m_atMtVidLmtHd[byStyle][5].SetMtVidRes(VIDEO_FORMAT_960x544);
	m_atMtVidLmtHd[byStyle][5].SetMtVidBW(wBW_8M);
	
	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4SIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);
	m_atMtVidLmtStd[byStyle][5].SetMtVidRes(VIDEO_FORMAT_4SIF);
	m_atMtVidLmtStd[byStyle][5].SetMtVidBW(wBW_8M);
	
	for(byLoop = 1; byLoop < 5; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_480x272);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	for(byLoop = 6; byLoop < 10; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_480x272);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	
	byStyle = VMP_STYLE_THIRTEEN;//13����
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_960x544);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);
	
	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4SIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);
	
	for(byLoop = 1; byLoop < 13; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_480x272);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	
				
	byStyle = VMP_STYLE_SIXTEEN;//16����
	for(byLoop = 0; byLoop < 16; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_480x272);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	
	
	byStyle = VMP_STYLE_SPECFOUR;//����4����
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_HD720);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);
	
	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4SIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);
	
	for(byLoop = 1; byLoop < 4; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_640x368);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_3M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	
	
	byStyle = VMP_STYLE_SEVEN;	//7����
	for(byLoop = 0; byLoop < 3; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_960x544);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_8M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_4SIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_8M);
	}
	for(byLoop = 3; byLoop < 7; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_480x272);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	
	
	byStyle = VMP_STYLE_TWENTY; //20����
	for(byLoop = 0; byLoop < 20; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_480x272);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}

	byStyle = VMP_STYLE_FIFTEEN;//15����
	for(byLoop = 0; byLoop < 3; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_640x544);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_3M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_640x544);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_3M);
	}

	for(byLoop = 3; byLoop < 15; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_320x272);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_320x272);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	
	byStyle = VMP_STYLE_TEN_M; //10����(�м�����)
	for(byLoop = 0; byLoop < 2; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_960x544);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_8M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_4SIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_8M);
	}
	for(byLoop = 2; byLoop < 10; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_480x272);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	
	byStyle = VMP_STYLE_THIRTEEN_M; //13����(һ�����м�)
	for(byLoop = 0; byLoop < 1; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_960x544);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_8M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_4SIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_8M);
	}
	for(byLoop = 1; byLoop < 13; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_480x272);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}

	byStyle = VMP_STYLE_TWENTYFIVE; //25����
	for(byLoop = 0; byLoop < 25; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}


#else
	byStyle = VMP_STYLE_ONE;	//1����
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_HD720);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);
	
	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4CIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);

	byStyle = VMP_STYLE_VTWO;	//����2����
	for(byLoop = 0; byLoop < 2;byLoop ++)
	{
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_640x368);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_8M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_4CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_8M);
	}

	
	byStyle = VMP_STYLE_HTWO;	//һ��һС2����
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_HD720);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);
	m_atMtVidLmtHd[byStyle][1].SetMtVidRes(VIDEO_FORMAT_720_432x240);
	m_atMtVidLmtHd[byStyle][1].SetMtVidBW(wBW_3M);
	
	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4CIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);
	m_atMtVidLmtStd[byStyle][1].SetMtVidRes(VIDEO_FORMAT_4CIF);
	m_atMtVidLmtStd[byStyle][1].SetMtVidBW(wBW_1d5M);

	byStyle = VMP_STYLE_THREE;	//3����
	for(byLoop = 0; byLoop < 3; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_640x368);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_8M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_4CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_8M);
	}
	
	
	byStyle = VMP_STYLE_FOUR;	//4����
	for(byLoop = 0; byLoop < 4; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_640x368);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_8M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_4CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_8M);
	}

	
	byStyle = VMP_STYLE_SIX;	//6����
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_720_864x480);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);

	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4CIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);

	for(byLoop = 1; byLoop < 6; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_432x240);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_3M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}

	
	byStyle = VMP_STYLE_EIGHT;	//8����
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_720_960x544);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);

	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4CIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);

	for(byLoop = 1; byLoop < 8; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_320x192);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);

 		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
 		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}

	
	byStyle = VMP_STYLE_NINE;	//9����
	for(byLoop = 0; byLoop < 9; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_432x240);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_3M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}

			
	byStyle = VMP_STYLE_TEN;	//10����
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_720_640x368);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);
	m_atMtVidLmtHd[byStyle][5].SetMtVidRes(VIDEO_FORMAT_720_640x368);
	m_atMtVidLmtHd[byStyle][5].SetMtVidBW(wBW_8M);

	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4CIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);
	m_atMtVidLmtStd[byStyle][5].SetMtVidRes(VIDEO_FORMAT_4CIF);
	m_atMtVidLmtStd[byStyle][5].SetMtVidBW(wBW_8M);

	for(byLoop = 1; byLoop < 5; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_320x192);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	for(byLoop = 6; byLoop < 10; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_320x192);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}

    	
	byStyle = VMP_STYLE_THIRTEEN;//13����
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_720_640x368);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);

	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4CIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);

	for(byLoop = 1; byLoop < 13; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_320x192);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}

			
	byStyle = VMP_STYLE_SIXTEEN;//16����
	for(byLoop = 0; byLoop < 16; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_320x192);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}

	
	byStyle = VMP_STYLE_SPECFOUR;//����4����
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_720_864x480);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);

	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4CIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);

	for(byLoop = 1; byLoop < 4; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_432x240);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_3M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_4CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}


	byStyle = VMP_STYLE_SEVEN;	//7����
	for(byLoop = 0; byLoop < 3; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_640x368);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_8M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_4CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_8M);
	}
	for(byLoop = 3; byLoop < 7; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_320x192);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}

#endif
	
}

/*==============================================================================
������    :  GetMacStrSegmentedbySep
����      :  ��ָ���ָ���õ�mac��ַ
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
20100601                   Ѧ��                            ����
==============================================================================*/
void GetMacStrSegmentedbySep(u8 *pbyBufIn, s8 *pchBufOut, s8 *pchSep)
{
	sprintf(pchBufOut, "%02X%s%02X%s%02X%s%02X%s%02X%s%02X", 
		pbyBufIn[0], pchSep,
		pbyBufIn[1], pchSep,
		pbyBufIn[2], pchSep,
		pbyBufIn[3], pchSep,
		pbyBufIn[4], pchSep,
		pbyBufIn[5]
		);
	
	return;
	
}


/************************************************************************/
/*                                                                      */
/*                            TApplySpeakQue                            */
/*                                                                      */
/************************************************************************/

//����
TApplySpeakQue::TApplySpeakQue() : 
                       m_byLen(MAXNUM_CONF_MT),
                       m_nHead(0),
                       m_nTail(0),
                       m_bPrtChgInfo(FALSE)
{
						   memset(m_atMtList, 0, sizeof(m_atMtList));
}

//����
TApplySpeakQue::~TApplySpeakQue()
{
/*lint -save -e1551*/
    Quit();
/*lint -restore*/
}

/*=============================================================================
  �� �� ���� Init
  ��    �ܣ� ���г�ʼ�ڴ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byQueueLen
  �� �� ֵ�� void
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/05    4.0			�ű���                  ����
=============================================================================*/
void TApplySpeakQue::Init( void )
{
	/*
    if ( 0 == byQueueLen ) 
    {
        LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[TApplySpeakQue] err: Len.%d, Init failed !\n", byQueueLen );
        return;
    }*/

    //m_ptMtList = new TMt[byQueueLen];
	
	memset(m_atMtList, 0, sizeof(m_atMtList));
	m_byLen = MAXNUM_CONF_MT;
	/*
    if ( NULL != m_ptMtList )
    {
        m_byLen = byQueueLen;
        memset(m_ptMtList, 0, sizeof(TMt) * byQueueLen);
    }
    else
    {
        LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[TApplySpeakQue] Init failed !\n" );
    }*/
    return;    
}

/*=============================================================================
  �� �� ���� Quit
  ��    �ܣ� �����ڴ��ͷ�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� void
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/05    4.0			�ű���                  ����
=============================================================================*/
void TApplySpeakQue::Quit()
{
	/*
    if ( NULL != m_ptMtList ) 
    {
        delete [] m_ptMtList;
        m_ptMtList = NULL;
    }*/
	memset(m_atMtList, 0, sizeof(m_atMtList));
    m_byLen = 0;
    m_nHead = 0;
    m_nTail = 0;
    m_bPrtChgInfo = FALSE;
}

/*=============================================================================
  �� �� ���� ProcQueueInfo
  ��    �ܣ� �������
  �㷨ʵ�֣� ������ �� ��ѭ�ϸ�� FIFO ����
  ȫ�ֱ����� 
  ��    ���� TMt &tMt:
             BOOL32      bInc      : TRUE ���У�    FALSE ������� �� ��������
             BOOL32      bDel      : TRUE ������У�FALSE ��������
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/05    4.0			�ű���                  ����
=============================================================================*/
BOOL32 TApplySpeakQue::ProcQueueInfo( TMt &tMt, BOOL32 &bIsSendToChairman, BOOL32 bInc, BOOL32 bDel )
{
	bIsSendToChairman = FALSE;
	u8 byCount = 0;
    if ( bInc )
    {
        if ( IsQueueFull() )
        {
			QueueLog("[ProcQueueInfo] Buffer Full!\n");
            return FALSE;
        }
        if ( tMt.IsNull() ) 
        {
			QueueLog("[ProcQueueInfo] tMt Null!\n");
            return FALSE;
        }
        if ( IsMtInQueue(tMt) )
        {
			QueueLog("[ProcQueueInfo] Mt<%d, %d> exist already!\n",
                      tMt.GetMcuId(), tMt.GetMtId());


            return FALSE;
        }

        memcpy(&m_atMtList[m_nTail], &tMt, sizeof(tMt));
        m_nTail = (m_nTail + 1) % m_byLen;

		byCount = m_nTail > m_nHead ? m_nTail - m_nHead  : ( m_byLen - m_nHead) + m_nTail ;
		if( byCount <= MAXNUM_ANSWERINSTANTLY_MT )
		{
			bIsSendToChairman = TRUE;
		}
		
        QueueLog("[ProcQueueInfo] Mt <%d, %d> INC queue !\n",
                  tMt.GetMcuId(), tMt.GetMtId());

        if ( m_bPrtChgInfo ) 
        {
            ShowQueue();
        }

        return TRUE;
    }
    else
    {
        if ( IsQueueNull() )
        {
			QueueLog("[ProcQueueInfo] Buffer NULL!\n");
            return FALSE;
        }

        if ( bDel ) 
        {
            if ( tMt.IsNull() ) 
            {
                QueueLog("[ProcQueueInfo] tCallInfo Null<Del>!\n");
                return FALSE;
            }
            
            u8 byQue = 0;
            if ( !IsMtInQueue(tMt, &byQue) )
            {

                QueueLog( "[ProcQueueInfo] Mt <%d, %d> to be DE unexist !\n",
                           tMt.GetMcuId(), tMt.GetMtId());

                return FALSE;
            }
            m_atMtList[byQue].SetNull();
            
            // ɾ����MT���ڶ���ͷ���������
            if ( m_nHead != byQue )
            {
                s32 nPos = 0;
                s32 nAdjustPos = byQue > m_nHead ? byQue : byQue + m_byLen;
				s32 nQueNew = 0;
				s32 nQueNewFr = 0;
				
                
                for( nPos = nAdjustPos; nPos > m_nHead; nPos-- )
                {
                    nQueNew   = nPos;
                    nQueNewFr = nQueNew - 1;
                    nQueNew   = nQueNew % m_byLen;
                    nQueNewFr = nQueNewFr % m_byLen;
                    m_atMtList[nQueNew] = m_atMtList[nQueNewFr];
					++byCount;
                }
				if( byCount < MAXNUM_ANSWERINSTANTLY_MT )
				{
					bIsSendToChairman = TRUE;
				}
                m_atMtList[m_nHead].SetNull();
            }
			else
			{
				bIsSendToChairman = TRUE;
			}
            m_nHead = (m_nHead + 1) % m_byLen;
        }
        else
        {
            if ( NULL != m_atMtList &&
				!m_atMtList[m_nHead].IsNull() )
            {
                tMt = m_atMtList[m_nHead];
                m_atMtList[m_nHead].SetNull();
                m_nHead = (m_nHead + 1) % m_byLen;
				bIsSendToChairman = TRUE;
            }
            else
            {
                QueueLog("[ProcQueueInfo] None member in queue !\n");
            }
        }


        QueueLog("[ProcQueueInfo] Mt <%d, %d> DE queue !\n",
                  tMt.GetMcuId(), tMt.GetMtId());

        
        if ( m_bPrtChgInfo ) 
        {
            ShowQueue();
        }

        return TRUE;
    }
}

/*=============================================================================
  �� �� ���� IsQueueNull
  ��    �ܣ� ����ͷ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2010/06/01    4.0			�ܾ���                  ����
=============================================================================*/
BOOL32 TApplySpeakQue::PopQueueHead( TMt &tMt )
{
	if ( IsQueueNull() )
    {
		QueueLog("[PopQueueHead] Buffer NULL!\n");
		tMt.SetNull();
        return FALSE;
    }

	tMt = m_atMtList[m_nHead];	
	m_atMtList[m_nHead].SetNull();
	m_nHead = (m_nHead + 1) % m_byLen;

	return TRUE;	
}

/*=============================================================================
  �� �� ���� PopQueue
  ��    �ܣ� ���������е�����һ���ն�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2010/06/01    4.0			�ܾ���                  ����
=============================================================================*/
BOOL32 TApplySpeakQue::GetQueueHead( TMt &tMt )
{
	if ( IsQueueNull() )
    {
		QueueLog("[PopQueueHead] Buffer NULL!\n");
		tMt.SetNull();
        return FALSE;
    }

	tMt = m_atMtList[m_nHead];		

	return TRUE;	
}

/*=============================================================================
  �� �� ���� GetQueueNextMt
  ��    �ܣ� ���������е�ĳ���ն˺��һ���ն�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2010/06/01    4.0			�ܾ���                  ����
=============================================================================*/
BOOL32 TApplySpeakQue::GetQueueNextMt( const TMt tCurMt,TMt &tNextMt )
{
	if ( IsQueueNull() ) 
    {
        return FALSE;
    }

	u8 byPos = 0;
	if( IsMtInQueue( tCurMt,&byPos ) )
	{
		byPos += 1;		
		byPos = byPos % m_byLen;		
		tNextMt = m_atMtList[byPos];
		if( !tNextMt.IsNull() )
		{
			return TRUE;
		}		
	}
	return FALSE;
}

/*=============================================================================
  �� �� ���� RemoveMtByMcuIdx
  ��    �ܣ� ɾ��ĳ��mcu�µ��ն�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� wMcuIdx mcu������
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/05    4.0			�ű���                  ����
=============================================================================*/
BOOL32 TApplySpeakQue::RemoveMtByMcuIdx( u16 wMcuIdx,BOOL32 &bIsSendToChairman )
{
	TMt tMt;
	u8 byPos = 0;
	u8 byNum = 0;
	bIsSendToChairman = FALSE;
	BOOL32 bSend = FALSE;

	BOOL32 bLoopFlag = TRUE;
	while( bLoopFlag )
	{
		tMt.SetMcuId( wMcuIdx ); 
		if( !IsMcuInQueue( tMt,&byPos ) )
		{
			break;
		}
		tMt = m_atMtList[byPos];
		ProcQueueInfo( tMt,bSend,FALSE );
		bIsSendToChairman = bSend ? TRUE:bIsSendToChairman;
		++byNum;
	}
	return ( byNum > 0 );
}

/*=============================================================================
  �� �� ���� RemoveMtByMcuIdx
  ��    �ܣ� ɾ��ĳ��mcu�µ��ն�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� wMcuIdx mcu������
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/05    4.0			�ű���                  ����
=============================================================================*/
BOOL32 TApplySpeakQue::IsMcuInQueue( TMt &tMt, u8* pbyPos )
{
	if ( IsQueueNull() ) 
    {
        return FALSE;
    }
    if ( INVALID_MCUIDX == tMt.GetMcuId() ) 
    {
        return FALSE;
    }

    s32 nQue  = m_nHead;
    s32 nTail = m_nTail > m_nHead ? m_nTail : m_nTail + m_byLen;

    for( ; nQue < nTail; nQue++ )
    {
        s32 nPos = nQue;
        nPos = nPos % m_byLen;

        if ( tMt.GetMcuId() == m_atMtList[nPos].GetMcuId() )
        {
            if ( NULL != pbyPos )
            {
                *pbyPos = (u8)nPos;
            }
            return TRUE;
        }
    }
    return FALSE;
}
/*=============================================================================
  �� �� ���� IsQueueNull
  ��    �ܣ� �����Ƿ�Ϊ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/05    4.0			�ű���                  ����
=============================================================================*/
BOOL32 TApplySpeakQue::IsQueueNull()
{
	/*
	if( NULL == m_atMtList )
	{
		return TRUE;
	}
	*/
    if ( m_atMtList[m_nHead].IsNull() )
    {
        return TRUE;
    }
    return FALSE;
}

/*=============================================================================
  �� �� ���� IsQueueFull
  ��    �ܣ� �����Ƿ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/05    4.0			�ű���                  ����
=============================================================================*/
BOOL32 TApplySpeakQue::IsQueueFull()
{
	/*
	if( NULL == m_ptMtList )
	{
		return FALSE;
	}
	*/
    if ( !m_atMtList[m_nTail].IsNull() )
    {
        return TRUE;
    }
    return FALSE;
}

/*=============================================================================
  �� �� ���� IsMtInQueue
  ��    �ܣ� ��ѯ�Ƿ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfIdx
             u8 byMtId
             u8 byPos   :����λ��
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/05    4.0			�ű���                  ����
=============================================================================*/
BOOL32 TApplySpeakQue::IsMtInQueue( const TMt &tMt, u8* pbyPos )
{
    if ( IsQueueNull() ) 
    {
        return FALSE;
    }
    if ( tMt.IsNull() ) 
    {
        return FALSE;
    }

    s32 nQue  = m_nHead;
    s32 nTail = m_nTail > m_nHead ? m_nTail : m_nTail + m_byLen;

    for( ; nQue < nTail; nQue++ )
    {
        s32 nPos = nQue;
        nPos = nPos % m_byLen;

        if ( tMt.GetMcuId() == m_atMtList[nPos].GetMcuId() &&
               tMt.GetMtId()  == m_atMtList[nPos].GetMtId())
        {
            if ( NULL != pbyPos )
            {
                *pbyPos = (u8)nPos;
            }
            return TRUE;
        }
    }
    return FALSE;
}

/*=============================================================================
  �� �� ���� ClearQueue
  ��    �ܣ� ��ն��г�Ա
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/05/22    4.0			�ű���                  ����
=============================================================================*/
void TApplySpeakQue::ClearQueue()
{
    memset( m_atMtList, 0, sizeof(m_atMtList) );
    m_nHead = 0;
    m_nTail = 0;
    m_bPrtChgInfo = FALSE;

    return;
}

/*=============================================================================
  �� �� ���� ShowQueue
  ��    �ܣ� ��ʾ��ǰ�ĺ��ж��к͵ȴ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL32 
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/05    4.0			�ű���                  ����
=============================================================================*/
void TApplySpeakQue::ShowQueue()
{
    // zbq [06/12/2007] �쳣����
    if ( m_byLen == 0 )
    {
        return;
    }
    
    BOOL32 bNull  = IsQueueNull();

    s32 nPos        = 0;
    s32 nAdjustTail = 0;

    if ( !bNull ) 
    {
        StaticLog("\n--------------------- Apply Queue ----------------------\n" );
        
        StaticLog("Head.%d, Tail.%d, Len.%d \n", m_nHead, m_nTail,
                                 m_nTail>m_nHead ? m_nTail-m_nHead : m_nTail+m_byLen-m_nHead );

        nAdjustTail = m_nHead < m_nTail ? m_nTail : m_nTail + m_byLen;

        for( nPos = m_nHead; nPos < nAdjustTail; nPos++ )
        {
            s32 nRealPos = nPos;
            nRealPos = nRealPos % m_byLen;

            StaticLog( "   queue[%d]: < McuId.%d, MtId.%d> \n",
                                        nRealPos,
                                        m_atMtList[nRealPos].GetMcuId(),
                                        m_atMtList[nRealPos].GetMtId());
                        
        }
        StaticLog("--------------------- Apply Queue End ------------------\n\n" );
    }
    else
    {
        StaticLog("\n********************* Apply Queue NULL *****************\n\n" );
    }
    return;
}

/*=============================================================================
  �� �� ���� PrtQueueInfo
  ��    �ܣ� ʵʱ��ӡ���ж��к͵ȴ����еĳ�������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
 -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/06    4.0			�ű���                  ����
=============================================================================*/
void TApplySpeakQue::PrtQueueInfo( BOOL32 bPrt /* = TRUE */)
{
    m_bPrtChgInfo = bPrt;
}


/*=============================================================================
  �� �� ���� QueueLog
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  s8* pszStr...
  �� �� ֵ�� void 
----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/4/17  4.0			�ܹ��                  ����
=============================================================================*/
void TApplySpeakQue::QueueLog( s8* pszStr, ... )
{
	s8 achBuf[1024];
    va_list argptr;
    if ( m_bPrtChgInfo )
    {
        s32 nPrefix = sprintf( achBuf, "[MtQueue]:" );
        va_start( argptr, pszStr );
		vsnprintf(achBuf + nPrefix, 1024 - nPrefix - 1, pszStr, argptr );
        //vsprintf( achBuf + nPrefix, pszStr, argptr );
        LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, achBuf );
        va_end( argptr );
    }
}

/*=============================================================================
  �� �� ���� GetQueueList
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2010/5/13     4.6			�ű���                  ����
=============================================================================*/
void TApplySpeakQue::GetQueueList(IN OUT TMt *ptMtList, IN OUT u8 &byLen)
{
    if (IsQueueNull())
    {
        byLen = 0;
        return;
    }

    u8 byListLen = 0;
    if (m_nTail > m_nHead)
    {
        byListLen = m_nTail - m_nHead;
    }
    else
    {
        byListLen = m_nTail + m_byLen - m_nHead;
    }

    if (byListLen > byLen)
    {
		byLen = 0;
        LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[GetQueueList] Non enough memery<ListLen.%d, MemLen.%d> applyed, ignore it!\n", byListLen, byLen);
        return;
    }

    s32 nQue  = m_nHead;
    s32 nTail = m_nTail > m_nHead ? m_nTail : m_nTail + m_byLen;

    byLen = 0;

    for( ; nQue < nTail; nQue++ )
    {
        s32 nPos = nQue;
        nPos = nPos % m_byLen;
        ptMtList[byLen] = m_atMtList[nPos];
        byLen ++;
    }

    return;
}

/*====================================================================
������      : IsConfSupportMainCap
����        : �����Ƿ�����Ӧ�Ĺ�ѡ֧������ʽ����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const TConfInfoEx& tConfinfo ���鹴ѡ��Ϣ
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2011/01/09  4.7         ��־��         ����
====================================================================*/
BOOL32 IsConfSupportMainCap(const TConfInfoEx& tConfInfoEx, const TSimCapSet &tMainSimCapSet)
{
	//���ģ�年ѡ
	TVideoStreamCap tVideoCap[MAX_CONF_CAP_EX_NUM];
	u8 byCapNUm = MAX_CONF_CAP_EX_NUM;
	if ( tConfInfoEx.GetMainStreamCapEx(tVideoCap,byCapNUm) )
	{
		if ( byCapNUm == 0 )
		{
			return FALSE;
		}

		for ( u8 byIdx = 0; byIdx<byCapNUm; byIdx++)
		{
			if ( tVideoCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL )
			{
				continue;
			}
			
			//ý������һ��
			if ( tMainSimCapSet.GetVideoMediaType() != tVideoCap[byIdx].GetMediaType()  )
			{
				continue;
			}

			//profile����һ��
			if ( tMainSimCapSet.GetVideoProfileType() != tVideoCap[byIdx].GetH264ProfileAttrb() )
			{
				continue;
			}

			//��ѡ�ֱ���С������ʽ�ֱ���
			if ( tMainSimCapSet.GetVideoResolution() < tVideoCap[byIdx].GetResolution() )
			{
				continue;
			}

			//��ѡ��֡��С������ʽ֡��
			if ( tMainSimCapSet.GetUserDefFrameRate() < tVideoCap[byIdx].GetUserDefFrameRate() )
			{
				continue;
			}

			return TRUE;
		}
	}

	return FALSE;
}

/*====================================================================
������      : IsConfSupportDSCap
����        : �����Ƿ�����Ӧ�Ĺ�ѡ֧������С��ĳ������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const TConfInfoEx& tConfinfo ���鹴ѡ��Ϣ
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2011/01/09  4.7         ��־��         ����
====================================================================*/
BOOL32 IsConfSupportDSCap(const TConfInfoEx& tConfInfoEx, const TDStreamCap &tSimCapSet)
{
	//���ģ�年ѡ
	TVideoStreamCap tVideoCap[MAX_CONF_CAP_EX_NUM];
	u8 byCapNUm = MAX_CONF_CAP_EX_NUM;
	if ( tConfInfoEx.GetDoubleStreamCapEx(tVideoCap,byCapNUm) )
	{
		if ( byCapNUm == 0 )
		{
			return FALSE;
		}

		for ( u8 byIdx = 0; byIdx<byCapNUm; byIdx++)
		{
			if ( tVideoCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL )
			{
				continue;
			}
			
			//ý������
			if ( tSimCapSet.GetMediaType() != tVideoCap[byIdx].GetMediaType()  )
			{
				continue;
			}

			//profile����
			if ( tSimCapSet.GetH264ProfileAttrb() != tVideoCap[byIdx].GetH264ProfileAttrb() )
			{
				continue;
			}

			//�ֱ���
			if ( tSimCapSet.GetResolution() < tVideoCap[byIdx].GetResolution() )
			{
				continue;
			}

			//֡��
			if ( tSimCapSet.GetUserDefFrameRate() < tVideoCap[byIdx].GetUserDefFrameRate() )
			{
				continue;
			}

			return TRUE;
		}
	}

	return FALSE;
}

/*====================================================================
������      : IsConfCanCompactAdapt
����        : �жϻ����Ƿ��ܽ�������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const TConfInfo& tConfinfo ������Ϣ
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
03/01/2010              �ܼ���         ����
====================================================================*/
BOOL32 IsConfCanCompactAdapt(const TConfInfo& tConfinfo, const TConfInfoEx &tConfInfoEx)
{
	if (tConfinfo.GetMainVideoMediaType() != MEDIA_TYPE_H264)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[IsConfCanCompactAdapt] conf's Main Media is not h264!\n");
		return FALSE;
	}
	
	//˫��
	if ( 0 == tConfinfo.GetSecBitRate())
	{
		return FALSE;
	}
	//����ʽ��ֱ���ʱ��������
	return !tConfInfoEx.IsMainHasCapEx();
}

u8 TransFRtoReal(u8 byMediaType, u8 byFrame)
{
	if (byMediaType == 0 || byMediaType == MEDIA_TYPE_H264)
	{
		return 0;
	}

	u8 byFrameRate = byFrame;

	if (byMediaType != MEDIA_TYPE_H264)
	{		
		switch(byFrameRate)
		{
		case VIDEO_FPS_2997_1:
			byFrameRate = 30;
			break;
		case VIDEO_FPS_25:
			byFrameRate = 25;
			break;
		case VIDEO_FPS_2997_2:
			byFrameRate = 15;
			break;
		case VIDEO_FPS_2997_3:
			byFrameRate = 10;
			break;
		case VIDEO_FPS_2997_4:
		case VIDEO_FPS_2997_5:
			byFrameRate = 6;
			break;
		case VIDEO_FPS_2997_6:
			byFrameRate = 5;
			break;
		case VIDEO_FPS_2997_30:
			byFrameRate = 1;
			break;
		case VIDEO_FPS_2997_7P5:
			byFrameRate = 4;
			break;
		case VIDEO_FPS_2997_10:
			byFrameRate = 3;
			break;
		case VIDEO_FPS_2997_15:
			byFrameRate = 2;
			break;
		default:
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[TransFRtoReal] undef non264 framerate%d\n", byFrameRate);
			break;
		}
	}
	return byFrameRate;
}

/*====================================================================
������      : GetDownStandRes
����        : ��ȡ�ȵ�ǰ�ֱ���С��һ����׼�ֱ���
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u8 byRes ��ǰ�ֱ���
����ֵ˵��  ������ƥ�䵽�ı�׼�ֱ���
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
05/21/2010              �ܼ���         ����
====================================================================*/
u8 GetDownStandRes(u8 byRes)
{
	if (VIDEO_FORMAT_INVALID == byRes || 0 == byRes)
	{
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetDownStandRes] byRes is unexpected %d!\n", byRes) ;
		return VIDEO_FORMAT_INVALID;
	}
	
	u16 wOrgWidth  = 0;
	u16 wOrgHeight = 0;
	GetWHByRes(byRes, wOrgWidth, wOrgHeight);
	
	if (wOrgWidth == 0 || wOrgHeight == 0)
	{
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetDownStandRes]wOrgWidth or wOrgHeight = 0!\n");
		return VIDEO_FORMAT_INVALID;
	}
	
    u8 abyStandRes[4] = {VIDEO_FORMAT_HD1080, 
		VIDEO_FORMAT_HD720,
		VIDEO_FORMAT_4CIF,
		VIDEO_FORMAT_CIF};
	
	u16 wStandWidth  = 0;
	u16 wStandHeight = 0;
	for(u8 byId = 0; byId < sizeof(abyStandRes)/sizeof(u8); byId++)
	{
		wStandWidth  = 0;
		wStandHeight = 0;
		GetWHByRes(abyStandRes[byId], wStandWidth, wStandHeight);
		if ((wOrgWidth * wOrgHeight) > (wStandWidth * wStandHeight))
		{
			return abyStandRes[byId];
		}
		
		//С��cif�ķǱ�ֱ��ʶ���cif����
		if (byId == ((sizeof(abyStandRes)/sizeof(u8)) -1))
		{
			if ((wOrgWidth * wOrgHeight) < (wStandWidth * wStandHeight))
			{
				return abyStandRes[byId];
			}			
		}
	}
	return VIDEO_FORMAT_INVALID;
}

/*====================================================================
������      : GetUpStandRes
����        : ��ȡ�ȵ�ǰ�ֱ��ʴ��һ����׼�ֱ���
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u8 byRes ��ǰ�ֱ���
����ֵ˵��  ������ƥ�䵽�ı�׼�ֱ���
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2011/08/23  4.6         zhangli         ����
====================================================================*/
u8 GetUpStandRes(u8 byRes)
{
	if (VIDEO_FORMAT_INVALID == byRes || 0 == byRes)
	{
		return VIDEO_FORMAT_INVALID;
	}
	
	u16 wOrgWidth  = 0;
	u16 wOrgHeight = 0;
	GetWHByRes(byRes, wOrgWidth, wOrgHeight);
	
	if (wOrgWidth == 0 || wOrgHeight == 0)
	{
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetUpStandRes]wOrgWidth or wOrgHeight = 0!\n");
		return VIDEO_FORMAT_INVALID;
	}
	
	u8 abyStandRes[4] = {VIDEO_FORMAT_CIF, VIDEO_FORMAT_4CIF, VIDEO_FORMAT_HD720, VIDEO_FORMAT_HD1080};
	
	u16 wStandWidth  = 0;
	u16 wStandHeight = 0;
	for(u8 byId = 0; byId < sizeof(abyStandRes)/sizeof(u8); byId++)
	{
		wStandWidth  = 0;
		wStandHeight = 0;
		GetWHByRes(abyStandRes[byId], wStandWidth, wStandHeight);
		if ((wOrgWidth * wOrgHeight) < (wStandWidth * wStandHeight))
		{
			return abyStandRes[byId];
		}
	}
	return VIDEO_FORMAT_INVALID;
}

/*====================================================================
������      : GetMinResAcdWHProduct
����        : ����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u8 byRes ��ǰ�ֱ���
����ֵ˵��  ������ƥ�䵽�ı�׼�ֱ���
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
05/21/2010              �ܼ���         ����
====================================================================*/
u8 GetMinResAcdWHProduct(u8 bySrcRes, u8 byDstRes)
{
	if (VIDEO_FORMAT_INVALID == bySrcRes || 0 == bySrcRes ||
		VIDEO_FORMAT_INVALID == byDstRes || 0 == byDstRes)
	{
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetMinResAcdWHProduct] bySrcRes %d or byDstRes %d is unexpected!\n",
			bySrcRes, byDstRes) ;
		return VIDEO_FORMAT_INVALID;
	}
	
	u16 wSrcWidth  = 0;
	u16 wSrcHeight = 0;
	u16 wDstWidth  = 0;
	u16 wDstHeight = 0;
	
	//��ȡԴ�ֱ���
	GetWHByRes(bySrcRes, wSrcWidth, wSrcHeight);
	if (wSrcWidth == 0 || wSrcHeight == 0)
	{
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetMinResAcdWHProduct]wSrcWidth or wSrcHeight = 0!\n");
		return VIDEO_FORMAT_INVALID;
	}
	
	//��ȡĿ�ķֱ���
	GetWHByRes(byDstRes, wDstWidth, wDstHeight);
	if (wDstWidth == 0 || wDstHeight == 0)
	{
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetMinResAcdWHProduct]wDstWidth or wDstHeight = 0!\n");
		return VIDEO_FORMAT_INVALID;
	}
	
	if ((wSrcWidth * wSrcHeight) >= (wDstWidth * wDstHeight))
	{
		return byDstRes;
	}
	else
	{
		return bySrcRes;
	} 
}

/*====================================================================
������      : GetNormalRes
����        : ���ݿ��,���һ��ͨ�õķֱ���,���ֱ�����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u16 wResW ��,u16 wResH ��
����ֵ˵��  ��v4r7ǰmcu֧�ֵķֱ���
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
09/13/2012              yanghuaizhi     ����
====================================================================*/
u8 GetNormalRes(u16 wResW, u16 wResH)
{
	// ֧�ֵ��ֱ��ʵ�ͨ�÷ֱ����б�,��mcu֧��
	u8 abyAcceptableRes[MAXNUM_ACCEPTABLE_RES] = 
	{
		VIDEO_FORMAT_HD1080,
		VIDEO_FORMAT_1440x816,
		VIDEO_FORMAT_HD720,
		VIDEO_FORMAT_960x544,
		VIDEO_FORMAT_720_864x480,	//TS Vmp ����Ҫ�÷ֱ���
		VIDEO_FORMAT_4CIF,
		VIDEO_FORMAT_4SIF,
		VIDEO_FORMAT_640x368,
		VIDEO_FORMAT_480x272,
		VIDEO_FORMAT_CIF,
		VIDEO_FORMAT_720_432x240,	// need by 8000G vmp 
		VIDEO_FORMAT_720_320x192,	// need by 8000G vmp
		VIDEO_FORMAT_QCIF,			//old vmp ���ܻ���ҪQCIF
		VIDEO_FORMAT_2CIF
	}; 

	u8 byExistRes = VIDEO_FORMAT_INVALID;	//�Ѵ��ڷֱ���
	u8 bySuitableRes = VIDEO_FORMAT_INVALID;	//��ӽ��ķֱ���,��߳˻�С�ڵ��ڴ˷ֱ��ʵ�����
	u16 wWidth = 0;
	u16 wHeight = 0;
	for(u8 byLoop = 0; byLoop < MAXNUM_ACCEPTABLE_RES; byLoop ++)
	{
		if( abyAcceptableRes[byLoop] == 0 )
		{
			break;
		}
		GetWHByRes( abyAcceptableRes[byLoop], wWidth, wHeight);
		// ���б����ж���,ֱ��ʹ��
		if (WHEQUAL == WHCmp (wWidth, wHeight, wResW, wResH))
		{
			byExistRes = abyAcceptableRes[byLoop];
			break;
		}
		// ��߳ɼ�С�ڴ˷ֱ���,������ӽ����Ǹ�����
		if (wWidth * wHeight < wResW * wResH)
		{
			if (VIDEO_FORMAT_INVALID == bySuitableRes)
			{
				bySuitableRes = abyAcceptableRes[byLoop];
			}
		}
	}

	// �д��ڵķֱ���,���ش��ڵ�,����,���غ��ʵ�
	if (VIDEO_FORMAT_INVALID != byExistRes)
	{
		return byExistRes;
	}
	else
	{
		return bySuitableRes;
	}
}


/*====================================================================
������      : GetSutiableRes
����        : ���ݿ��,�����Ӧ�ֱ��ʡ�����ÿ��MCU��֧�֣����»����ӽ��ķֱ���
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u16 wResW ��,u16 wResH ��
����ֵ˵��  ��mcu֧�ֵķֱ���
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
09/17/2012              chendaiwei     ����
====================================================================*/
u8 GetMcuSupportedRes(u16 wResW, u16 wResH)
{
	u8 abyAcceptableRes[MAXNUM_ACCEPTABLE_RES] = 
	{
		VIDEO_FORMAT_HD1080,
		VIDEO_FORMAT_1440x816,
		VIDEO_FORMAT_HD720,
		VIDEO_FORMAT_960x544,
		VIDEO_FORMAT_720_864x480,	//TS Vmp ����Ҫ�÷ֱ���
		VIDEO_FORMAT_4CIF,
		VIDEO_FORMAT_4SIF,
		VIDEO_FORMAT_640x544,
		VIDEO_FORMAT_640x368,
		VIDEO_FORMAT_480x272,
		VIDEO_FORMAT_384x272,		//mpu2 20������Ҫ384x272
		VIDEO_FORMAT_CIF,
		VIDEO_FORMAT_720_432x240,	// need by 8000G vmp 
		VIDEO_FORMAT_720_320x192,	// need by 8000G vmp
		VIDEO_FORMAT_320x272,
		VIDEO_FORMAT_QCIF,			//old vmp ���ܻ���ҪQCIF
		VIDEO_FORMAT_2CIF
	}; 

	u8 byExistRes = VIDEO_FORMAT_INVALID;	//�Ѵ��ڷֱ���
	u8 bySuitableRes = VIDEO_FORMAT_INVALID;	//��ӽ��ķֱ���,��߳˻�С�ڵ��ڴ˷ֱ��ʵ�����
	u16 wWidth = 0;
	u16 wHeight = 0;
	for(u8 byLoop = 0; byLoop < MAXNUM_ACCEPTABLE_RES; byLoop ++)
	{
		if( abyAcceptableRes[byLoop] == 0 )
		{
			break;
		}
		GetWHByRes( abyAcceptableRes[byLoop], wWidth, wHeight);
		// ���б����ж���,ֱ��ʹ��
		if (WHEQUAL == WHCmp (wWidth, wHeight, wResW, wResH))
		{
			byExistRes = abyAcceptableRes[byLoop];
			break;
		}
		// ��߳ɼ�С�ڴ˷ֱ���,������ӽ����Ǹ�����
		if (wWidth * wHeight < wResW * wResH)
		{
			if (VIDEO_FORMAT_INVALID == bySuitableRes)
			{
				bySuitableRes = abyAcceptableRes[byLoop];
			}
		}
	}

	// �д��ڵķֱ���,���ش��ڵ�,����,���غ��ʵ�
	if (VIDEO_FORMAT_INVALID != byExistRes)
	{
		return byExistRes;
	}
	else
	{
		return bySuitableRes;
	}
}



/*====================================================================
������      : GetBasNeedResDataByCap
����        : ͨ��������ñ������������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const TSimCapSet &tSimCapSet  ������
			  TNeedBasResData &tBasResData  ����BAS��Դ
����ֵ˵��  ���ɹ�����TRUE
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2011/12/07  4.7         ��־��         ����
====================================================================*/
BOOL32	GetBasNeedResDataByCap(const TSimCapSet &tSrcCap,const TSimCapSet &tDstCap, u8 byMediaMode, TNeedVidAdaptData &tBasResData)
{
	if (MODE_AUDIO == byMediaMode)
	{
		tBasResData.m_byMediaMode = MODE_AUDIO;
		tBasResData.m_byRealEncNum = 1;
		tBasResData.m_byDecNeedRes = (u8)1 <<CBasChn::emDecAud;
	}
	else if (MODE_VIDEO == byMediaMode || MODE_SECVIDEO == byMediaMode)
	{
		tBasResData.m_byMediaMode = byMediaMode;
		tBasResData.m_byRealEncNum = 1;
		
		//��ý�������
		if ( tSrcCap.GetVideoMediaType() == MEDIA_TYPE_H261 )
		{
			tBasResData.m_byDecNeedRes = (u8)1 << CBasChn::emDecH261;
		}
		else if ( tSrcCap.GetVideoCap().IsSupportHP() )
		{
			tBasResData.m_byDecNeedRes = (u8)1 << CBasChn::emDecHp;
		}
		else
		{
			u16 wSrcWidth  = 0;
			u16 wSrcHeight = 0;
			GetWHByRes(tSrcCap.GetVideoResolution(), wSrcWidth, wSrcHeight);
			u16 w720Width  = 0;
			u16 w720Height = 0;
			GetWHByRes(VIDEO_FORMAT_HD720, w720Width, w720Height);
			BOOL32 bDecHighFps = tSrcCap.GetUserDefFrameRate() >= 50 &&
				((wSrcWidth * wSrcHeight) > (w720Width * w720Height));
			//�����Ƿ�������
			BOOL32 bDecHDRes   = MEDIA_TYPE_H264 == tSrcCap.GetVideoMediaType()
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
				&& IsResG(tSrcCap.GetVideoResolution(), VIDEO_FORMAT_4CIF);
#else
			&& IsResGE(tSrcCap.GetVideoResolution(), VIDEO_FORMAT_4CIF);
#endif
			if (bDecHighFps)
			{
				tBasResData.m_byDecNeedRes  = (u8)1 << CBasChn::emDecHighFps;
			}
			else if(bDecHDRes)
			{
				tBasResData.m_byDecNeedRes  = (u8)1 << CBasChn::emDecHD;
			}
			else
			{
				tBasResData.m_byDecNeedRes  = (u8)1 << CBasChn::emDecSD;
			}
		}
		
		//��ñ�������
		tBasResData.m_atSimCapSet[0] = tDstCap.GetVideoCap();
	}
	else
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[GetBasNeedResDataByCap]byMediaMode:%d is illegal!\n");
		return FALSE;
	}
	return TRUE;
}

/*====================================================================
������      : GetResourceUnitAcd2Cap
����        : ���ݸ�ʽ���ֱ��ʡ�֡�ʻ�ñ�����Ƕ�Ӧռ�õ���Դ��λ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u8 byMediaType  ��ʽ
			  u8 byRes		  �ֱ���
			  u8 byFrameRate  ֡��
����ֵ˵��  ������������Դ��λ
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2011/12/07  4.7            ��־��         ����
====================================================================*/
u16 GetResourceUnitAcd2Cap(u8 byMediaType, u8 byRes, u8 byFrameRate)
{
	u16 wRetVal = 0;
	if ( MEDIA_TYPE_H264 != byMediaType ||
		 VIDEO_FORMAT_INVALID == byRes  ||
		 0 == byFrameRate
		)
	{
		LogPrint( LOG_LVL_DETAIL, MID_PUB_ALWAYS, "[GetResourceUnitAcd2Cap]byMediaType:%d,byRes:%d,byFrameRate:%d is Error!\n",
				 byMediaType, byRes, byFrameRate
				);
		return INVALID_RESOUCEUNIT_VALUE;
	}

	switch (byRes)
	{
	case VIDEO_FORMAT_HD1080:
		{
			if ( byFrameRate == 60 || byFrameRate == 50 )
			{
				wRetVal = 1000;
			} 
			else if (byFrameRate ==30  || byFrameRate ==25 )
			{
				wRetVal = 500;
			}
			else
			{
				LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetResourceUnitAcd2Cap] byRes:%d,byFrameRate:%d is Error!\n",
						  byRes, byFrameRate
						);
				wRetVal = INVALID_RESOUCEUNIT_VALUE;
			}
		}
		break;
	case VIDEO_FORMAT_HD720:
		{
			if ( byFrameRate == 60 || byFrameRate == 50 )
			{
				wRetVal = 444;
			} 
			else if (byFrameRate ==30  || byFrameRate ==25 )
			{
				wRetVal = 222;
			}
			else
			{
				LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetResourceUnitAcd2Cap] byRes:%d,byFrameRate:%d is Error!\n",
					byRes, byFrameRate
					);
				wRetVal = INVALID_RESOUCEUNIT_VALUE;
			}
		}
		break;
	case VIDEO_FORMAT_4CIF:
		{
			wRetVal = 81;
		}
		break;
	case VIDEO_FORMAT_CIF:
		{
			wRetVal = 2;
		}
		break;
	case VIDEO_FORMAT_UXGA:
		{
			if ( byFrameRate == 60  )
			{
				wRetVal = 926;
			} 
			else if ( byFrameRate == 30 )
			{
				wRetVal = 463;
			}
			else if ( byFrameRate == 20 )
			{
				wRetVal = 309;
			}
			else if ( byFrameRate == 15 )
			{
				wRetVal = 232;
			}
			else if ( byFrameRate == 10 )
			{
				wRetVal = 154;
			}
			else if ( byFrameRate == 5 )
			{
				wRetVal = 77;
			}
			else
			{
				LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetResourceUnitAcd2Cap] byRes:%d,byFrameRate:%d is Error!\n",
					byRes, byFrameRate
					);
				wRetVal = INVALID_RESOUCEUNIT_VALUE;
			}
		}
		break;
	case VIDEO_FORMAT_SXGA:
		{
			if ( byFrameRate == 60  )
			{
				wRetVal = 632;
			} 
			else if ( byFrameRate== 30)
			{
				wRetVal = 316;
			}
			else if ( byFrameRate == 20)
			{
				wRetVal = 211;
			}
			else if ( byFrameRate == 15)
			{
				wRetVal = 158;
			}
			else if ( byFrameRate == 10)
			{
				wRetVal = 106;
			}
			else if ( byFrameRate ==5 )
			{
				wRetVal = 53;
			}
			else
			{
				LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetResourceUnitAcd2Cap] byRes:%d,byFrameRate:%d is Error!\n",
					byRes, byFrameRate
					);
				wRetVal = INVALID_RESOUCEUNIT_VALUE;
			}
		}
		break;
// 	case VIDEO_FORMAT_XGA:
// 		{
// 			if ( byFrameRate == 5  )
// 			{
// 				wRetVal = 32;
// 			} 
// 			else
// 			{
// 				LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetResourceUnitAcd2Cap] byRes:%d,byFrameRate:%d is Error!\n"
// 						 byRes, byFrameRate
// 						);
// 				wRetVal = INVALID_RESOUCEUNIT_VALUE;
// 			}
// 		}
// 		break;
	default:
		{
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetResourceUnitAcd2Cap] byRes:%d,byFrameRate:%d is Error!\n",
					byRes, byFrameRate
					);
			wRetVal = INVALID_RESOUCEUNIT_VALUE;
		}
		break;
	}

	return wRetVal;
}

/*====================================================================
������      : GetResourceUnitAcd2Cap
����        : ����TVideoStreamCap��ñ�����Ƕ�Ӧռ�õ���Դ��λ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const TVideoStreamCap &tSimCapSet ������
����ֵ˵��  ������������Դ��λ
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2011/12/07  4.7            ��־��         ����
====================================================================*/
u16 GetResourceUnitAcd2Cap(const TVideoStreamCap &tSimCapSet)
{
	u8 byMediaType = tSimCapSet.GetMediaType();
	u8 byRes	   = tSimCapSet.GetResolution();
	u8 byFrameRate = tSimCapSet.GetUserDefFrameRate();

	return GetResourceUnitAcd2Cap(byMediaType, byRes, byFrameRate);
}

/*====================================================================
������      : GetResourceUnitAcd2Cap
����        : ���ݻ��鹴ѡidx��ö�Ӧ��Դ��λ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u8 *pbyIdx ���鹴ѡidx����ָ��
			  u8 byNum	 �������
����ֵ˵��  ������������Դ��λ��0XFFFF��ʾ��������ʱ�����ܻ�ö�Ӧ������Դ��λ
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2011/12/07  4.7            ��־��         ����
====================================================================*/
u16 GetResourceUnitAcd2CapArray(TSimCapSet *tSimCapSet,u8 byNum)
{
	u16  wResUnit = 0;

	for ( u8 byIdx= 0; byIdx< byNum; byIdx++)
	{
		u16 wTempResUnit =  GetResourceUnitAcd2Cap( tSimCapSet[byIdx].GetVideoCap() );
		if ( wTempResUnit != INVALID_RESOUCEUNIT_VALUE )
		{
			wResUnit += wTempResUnit;
		}
	}

	return wResUnit;
}

/*====================================================================
������      : IsNeedAdjustCapBySrc
����        : ���Դ����С��Ŀ����������ô����Դ��������Ŀ������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����const TVideoStreamCap &tVidSrcCap Դ����
			  TVideoStreamCap &tVidDstCap Ŀ�������������Ҫ����Դ������ôͨ��
			  �ñ��������µ�����
			  u8 byMediaMode ģʽ
����ֵ˵��  ������������Դ��λ
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/04/23  4.7            ��־��         ����
====================================================================*/
BOOL32 IsNeedAdjustCapBySrc(const TVideoStreamCap &tVidSrcCap,TVideoStreamCap &tVidDstCap)
{
	BOOL32 bIsAdjustBySrc = FALSE;
	//�Ƿ���Ҫ��֡�ʣ�Ŀǰ��֡��ֻ��Ŀ����H264�����
	if ( tVidDstCap.GetMediaType() == MEDIA_TYPE_H264 )
	{
		u8 byDstFrameRate = tVidDstCap.GetUserDefFrameRate();
		u8 bySrcFramRate =0;
		if ( tVidSrcCap.GetMediaType() != MEDIA_TYPE_H264 )
		{
			bySrcFramRate= FrameRateMac2Real(tVidSrcCap.GetFrameRate());
		}
		else
		{
			bySrcFramRate = tVidSrcCap.GetUserDefFrameRate();
		}

		if ( bySrcFramRate < byDstFrameRate )
		{
			tVidDstCap.SetUserDefFrameRate(bySrcFramRate);
			bIsAdjustBySrc= TRUE;
		}
	}

	//�Ƿ���Ҫ���ֱ���
	u16 wSrcWidth=0;
	u16 wSrcHeight=0;
	if ( tVidSrcCap.GetMediaType() == MEDIA_TYPE_MP4 && tVidSrcCap.GetResolution() == VIDEO_FORMAT_AUTO )
	{
		GetWHByRes( GetAutoResByBitrate( tVidSrcCap.GetResolution(),tVidSrcCap.GetMaxBitRate() ),wSrcWidth,wSrcHeight );
		u8 byTempMpe4Res= GetResByWH(wSrcWidth,wSrcHeight);
		//���Դ��MPE4 2CIF��������CIF���Ƚ�
		if ( byTempMpe4Res == VIDEO_FORMAT_2CIF )
		{
			GetWHByRes( VIDEO_FORMAT_CIF, wSrcWidth,wSrcHeight);
		}
	}
	else
	{
		GetWHByRes( tVidSrcCap.GetResolution(), wSrcWidth,wSrcHeight);
	}

	u16 wDstWidth=0;
	u16 wDstHeight=0;
	if ( tVidDstCap.GetMediaType() == MEDIA_TYPE_MP4 && tVidDstCap.GetResolution() == VIDEO_FORMAT_AUTO )
	{
		GetWHByRes( GetAutoResByBitrate( tVidDstCap.GetResolution(),tVidDstCap.GetMaxBitRate() ),wDstWidth,wDstHeight );
		u8 byTempMpe4Res= GetResByWH(wDstWidth,wDstHeight);
		//���Դ��MPE4 2CIF��������CIF���Ƚ�
		if ( byTempMpe4Res == VIDEO_FORMAT_2CIF )
		{
			GetWHByRes( VIDEO_FORMAT_CIF, wDstWidth,wDstHeight);
		}
	}
	else
	{
		GetWHByRes( tVidDstCap.GetResolution(), wDstWidth,wDstHeight);
	}

	if ( wSrcWidth*wSrcHeight < wDstWidth*wDstHeight )
	{
		u8 byNewRes=GetResByWH(wSrcWidth,wSrcHeight);
		tVidDstCap.SetResolution(byNewRes);
		bIsAdjustBySrc = TRUE;
	}
	
	return bIsAdjustBySrc;
}

/*====================================================================
������      : ConstructMsgToVmp
����        : ��֯������ϳ����Ĳ���
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CServMsg &cServMsg	[o]
����ֵ˵��  ��void
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
02/16/2011              Ѧ��          ����
====================================================================*/
void CParamToOldVmp::ConstructMsgToVmp(CServMsg &cServMsg)
{
	CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(m_byConfIdx);
	if ( NULL == pVcInst )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ConstructMsgToVmp]ConfIdx.%d doesn't exist!\n", m_byConfIdx );
		return;
    }

	TConfInfo tConfInfo = pVcInst->m_tConf;
	cServMsg.SetConfId(tConfInfo.GetConfId());


	CKDVVMPParam cKDVVMPParam;
	memset(&cKDVVMPParam, 0, sizeof(cKDVVMPParam));

	//style
	cKDVVMPParam.m_byVMPStyle = m_tVmpCommonAttrb.m_byVmpStyle;
	
	//enctype
	cKDVVMPParam.m_byEncType = tConfInfo.GetMainVideoMediaType();

	//member number
	cKDVVMPParam.m_byMemberNum  = m_tVmpCommonAttrb.m_byMemberNum;

	//payload and encypt key
	memcpy(cKDVVMPParam.m_atMtMember, m_tVmpCommonAttrb.m_atMtMember, sizeof(cKDVVMPParam.m_atMtMember));
	memcpy(cKDVVMPParam.m_tVideoEncrypt, m_tVmpCommonAttrb.m_tVideoEncrypt, sizeof(cKDVVMPParam.m_tVideoEncrypt));
	memcpy(cKDVVMPParam.m_tDoublePayload, m_tVmpCommonAttrb.m_tDoublePayload, sizeof(cKDVVMPParam.m_tDoublePayload));

	//fps: no

	//resolution, BR info for 1st Encode Channel-----------------------
	u16 wMinMtReqBitrate = 0;
    if (0 != tConfInfo.GetSecBitRate() && 
        MEDIA_TYPE_NULL == tConfInfo.GetSecVideoMediaType())
    {
		//���ж�˫�ٵ���ʽ������������һͨ��Ϊ�������ʣ��ڶ�ͨ��Ϊ�ǻ������ʵ�������ʣ�>=���鸨�����ʣ�
        wMinMtReqBitrate = tConfInfo.GetBitRate();
    }
    else
    {
        wMinMtReqBitrate = pVcInst->GetLeastMtReqBitrate(TRUE, cKDVVMPParam.m_byEncType);
    }
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( m_byVmpId );
	g_cMcuVcApp.SetVMPOutChlBitrate(tVmpEqp, 0, wMinMtReqBitrate);
    
    cKDVVMPParam.m_wBitRate = htons(wMinMtReqBitrate);
	
    u16 wWidth = 0;
    u16 wHeight = 0;
    tConfInfo.GetVideoScale(cKDVVMPParam.m_byEncType,
		wWidth,
		wHeight);
    cKDVVMPParam.m_wVideoWidth = wWidth;
    cKDVVMPParam.m_wVideoHeight = wHeight;
	
    //����Լ��16CIF����2CIF,������AUTO����, xsl [8/11/2006] mpeg2 4cif��2cif����
	u8 byRes = tConfInfo.GetVideoFormat(cKDVVMPParam.m_byEncType);
    if( VIDEO_FORMAT_16CIF == byRes ||
        (MEDIA_TYPE_H262 == cKDVVMPParam.m_byEncType && VIDEO_FORMAT_4CIF == byRes))
    {
        cKDVVMPParam.m_wVideoWidth  = 352;
        cKDVVMPParam.m_wVideoHeight = 576;
    }
    
	cKDVVMPParam.m_wVideoWidth  = htons(cKDVVMPParam.m_wVideoWidth);
	cKDVVMPParam.m_wVideoHeight = htons(cKDVVMPParam.m_wVideoHeight);

	cServMsg.SetMsgBody((u8*)&cKDVVMPParam, sizeof(cKDVVMPParam));

	// resolution, BR info for 2nd Encode Channel if any-----------------------
    if (0 != tConfInfo.GetSecBitRate() && 
        MEDIA_TYPE_NULL == tConfInfo.GetSecVideoMediaType())
    {
		//˫�ٵ���ʽ����ʱ�ڶ�ͨ������ý���ʽ
        cKDVVMPParam.m_byEncType = tConfInfo.GetMainVideoMediaType();
    }
    else
    {
        cKDVVMPParam.m_byEncType = tConfInfo.GetSecVideoMediaType();
        
        //xsl [8/11/2006] mpeg2 4cif��2cif����
        tConfInfo.GetVideoScale(cKDVVMPParam.m_byEncType, wWidth, wHeight);
        cKDVVMPParam.m_wVideoWidth = wWidth;
        cKDVVMPParam.m_wVideoHeight = wHeight;
        byRes = tConfInfo.GetVideoFormat(cKDVVMPParam.m_byEncType);
        if( VIDEO_FORMAT_16CIF == byRes ||
            (MEDIA_TYPE_H262 == cKDVVMPParam.m_byEncType && VIDEO_FORMAT_4CIF == byRes))
        {
            cKDVVMPParam.m_wVideoWidth  = 352;
            cKDVVMPParam.m_wVideoHeight = 576;
        }        
        cKDVVMPParam.m_wVideoWidth  = htons(cKDVVMPParam.m_wVideoWidth);
        cKDVVMPParam.m_wVideoHeight = htons(cKDVVMPParam.m_wVideoHeight);
    }
    
    //modify bas 2
    // ���ٵ���ʽ���飬������0, zgc, 2008-03-19
    if ( MEDIA_TYPE_NULL == cKDVVMPParam.m_byEncType )
    {
        wMinMtReqBitrate = 0;
    }
    else
    {
        wMinMtReqBitrate = pVcInst->GetLeastMtReqBitrate(TRUE, cKDVVMPParam.m_byEncType);
//         if (wMinMtReqBitrate < m_wBasBitrate || 0 == m_wBasBitrate)
//         {
//             m_wBasBitrate = wMinMtReqBitrate;
//         }
    }
    cKDVVMPParam.m_wBitRate = htons(wMinMtReqBitrate);
	
	cServMsg.CatMsgBody((u8 *)&cKDVVMPParam, sizeof(cKDVVMPParam));


	//need PRS
	u8 byNeedPrs = m_tVmpCommonAttrb.m_byNeedPrs;
	cServMsg.CatMsgBody((u8 *)&byNeedPrs, sizeof(byNeedPrs));

	//����ϳɷ��
    TVmpStyleCfgInfo tMcuVmpStyle = m_tVmpCommonAttrb.m_tVmpStyleCfgInfo;    
    cServMsg.CatMsgBody((u8*)&tMcuVmpStyle, sizeof(tMcuVmpStyle));

}

/*====================================================================
������      : ConstructMsgToVmp
����        : ��֯������ϳ����Ĳ���
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CServMsg &cServMsg	[o]
����ֵ˵��  ��void
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2011-12-1              yanghuaizhi       ����
====================================================================*/
void CParamToKDVVmp::ConstructMsgToVmp(CServMsg &cServMsg)
{
	CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(m_byConfIdx);
	if ( NULL == pVcInst )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ConstructMsgToVmp]ConfIdx.%d doesn't exist!\n", m_byConfIdx );
		return;
    }
	
	TConfInfo tConfInfo = pVcInst->m_tConf;
	cServMsg.SetConfId(tConfInfo.GetConfId());
	// ���û���ϳɲ���ͨ�ò���
	// ����mpu2��ʽ�洢ͨ�ò���
	TMPU2CommonAttrb tMpu2CommonAttrb;
	tMpu2CommonAttrb.m_byVmpStyle	= m_tVmpCommonAttrb.m_byVmpStyle;	//�ϳɷ��
	tMpu2CommonAttrb.m_byMemberNum	= m_tVmpCommonAttrb.m_byMemberNum;	//��Ա����
	tMpu2CommonAttrb.m_byNeedPrs	= m_tVmpCommonAttrb.m_byNeedPrs;	//�Ƿ���ҪPRS
	memcpy(tMpu2CommonAttrb.m_atMtMember, m_tVmpCommonAttrb.m_atMtMember, sizeof(TVMPMemberEx)*MAXNUM_MPU2VMP_MEMBER);
	memcpy(tMpu2CommonAttrb.m_tVideoEncrypt, m_tVmpCommonAttrb.m_tVideoEncrypt, sizeof(TMediaEncrypt)*MAXNUM_MPU2VMP_MEMBER);
	memcpy(tMpu2CommonAttrb.m_tDoublePayload, m_tVmpCommonAttrb.m_tDoublePayload, sizeof(TDoublePayload)*MAXNUM_MPU2VMP_MEMBER);
	tMpu2CommonAttrb.m_tVmpStyleCfgInfo = m_tVmpCommonAttrb.m_tVmpStyleCfgInfo;		//������Ϣ
	tMpu2CommonAttrb.m_tVmpExCfgInfo = m_tVmpCommonAttrb.m_tVmpExCfgInfo;		//��չ������Ϣ
	// ����ͨ����Ϣ
	cServMsg.SetMsgBody((u8*)&tMpu2CommonAttrb, sizeof(tMpu2CommonAttrb));

	CKDVVMPOutMember cVMPOutMember;
	u8 byChnnlRes  = 0;
	u16 wWidth		= 0;	//��
	u16 wHeight		= 0;	//��
	u16 wMinMtReqBitrate = 0;
	u8 byFrameRate = 0;
	TVideoStreamCap tStrCap;
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( m_byVmpId );
	for (u8 byChnIdx=0; byChnIdx<MAXNUM_MPU2_OUTCHNNL; byChnIdx++)
	{
		cVMPOutMember.Clear();
		// �����Ƶ����
		tStrCap = m_tVMPOutParam.GetVmpOutCapIdx(byChnIdx);
		if (MEDIA_TYPE_NULL != tStrCap.GetMediaType())
		{
			//���Idx��Ӧ������
			cVMPOutMember.SetEncType(tStrCap.GetMediaType());
			byChnnlRes = tStrCap.GetResolution();
			wMinMtReqBitrate = pVcInst->GetMinMtRcvBitByVmpChn(m_byVmpId, TRUE,byChnIdx);
			//mp4_auto �������ʱ��Ӧ�ֱ���
			if ( MEDIA_TYPE_MP4 == tStrCap.GetMediaType()
				&& VIDEO_FORMAT_AUTO == byChnnlRes )
			{
				if (wMinMtReqBitrate > 3072)
				{
					byChnnlRes = VIDEO_FORMAT_4CIF;
				} 
				else if(wMinMtReqBitrate > 1536)
				{
					byChnnlRes = VIDEO_FORMAT_2CIF;
				}
				else
				{
					byChnnlRes = VIDEO_FORMAT_CIF;
				}
			}
			// mp4 16cif �²�Ϊmpeg4 4cif
			else if (MEDIA_TYPE_MP4 == tStrCap.GetMediaType()
				&& VIDEO_FORMAT_16CIF == byChnnlRes )
			{
				byChnnlRes = VIDEO_FORMAT_4CIF;
			}
			// h264 auto �²�Ϊcif
			else if (MEDIA_TYPE_H264 == tStrCap.GetMediaType()
				&& VIDEO_FORMAT_AUTO == byChnnlRes )
			{
				byChnnlRes = VIDEO_FORMAT_CIF;
			}
			//������������÷ֱ���
			GetWHByRes(byChnnlRes , wWidth, wHeight);
			// ��MP4��4cif �ϸ����ָ����
			if( MEDIA_TYPE_MP4 == tStrCap.GetMediaType() && VIDEO_FORMAT_4CIF == tStrCap.GetResolution())
			{
				wWidth  = 720;
				wHeight = 576;
			}

			//fps:
			byFrameRate = tStrCap.GetUserDefFrameRate();
			if( tStrCap.GetMediaType() == MEDIA_TYPE_H264)
			{
				//h264: the value is real, so maintain the value
			}
			else
			{
				byFrameRate = FrameRateMac2Real(byFrameRate);
			}

			cVMPOutMember.SetVideoWidth(wWidth);
			cVMPOutMember.SetVideoHeight(wHeight);
			cVMPOutMember.SetProfileType(tStrCap.GetH264ProfileAttrb());
			cVMPOutMember.SetFrameRate(byFrameRate);
			g_cMcuVcApp.SetVMPOutChlBitrate(tVmpEqp, byChnIdx, wMinMtReqBitrate);
			cVMPOutMember.SetBitRate(wMinMtReqBitrate);
		}
		// ����VMP���������
		cServMsg.CatMsgBody((u8*)&cVMPOutMember, sizeof(cVMPOutMember));
	}

    // ׷����Ϣ [7/4/2013 liaokang]
    // �ṹ��u8 ��type num��+ 
    //       u8 ��type��    +  u16 ��type length��net order�� + content + ��
    u8  byTypeNum = 1;
    cServMsg.CatMsgBody(&byTypeNum, sizeof(byTypeNum));

    // ׷��Vmp��Ա���� [7/4/2013 liaokang] type + type length + ��  ChnNum + ( ChnNo + Len + content + ��)  ��
    s8  achMbAlias[1 + 2 + 1 + (1 + 1 + MAXLEN_ALIAS)*MAXNUM_MPU2VMP_MEMBER] = {0};
    u8 *pchMbAlias = (u8*)achMbAlias;
    u8 byChnNum = 0;
    u16 wTypeLen = (1 + 2 + 1);
    pchMbAlias += wTypeLen; 
    s8  achAlias[MAXLEN_ALIAS] = {0}; 
    u8 byAliasLen = 0;
    for(u8 byChnNo=0; byChnNo<m_tVmpCommonAttrb.m_byMemberNum; byChnNo++)
    {
        if(m_tVmpCommonAttrb.m_atMtMember[byChnNo].IsNull())
        {
            continue;
        }

        memset(achAlias,0,sizeof(achAlias));
        if( !( pVcInst->GetMtAliasToVmp(m_tVmpCommonAttrb.m_atMtMember[byChnNo],achAlias) ) )
        {
            continue;
        }
        byChnNum++;
        byAliasLen = strlen(achAlias);
        *pchMbAlias = byChnNo;
        pchMbAlias++;
        *pchMbAlias = byAliasLen;
        pchMbAlias++;
        memcpy(pchMbAlias,achAlias,byAliasLen);
        pchMbAlias += byAliasLen;
        wTypeLen += (2 + byAliasLen);
    }
    pchMbAlias -= wTypeLen;
    *pchMbAlias = (u8)emVmpMbAlias;
    pchMbAlias++;
    *(u16*)pchMbAlias = htons(wTypeLen - 1 - 2);
    pchMbAlias += 2;
    *pchMbAlias = byChnNum;
    cServMsg.CatMsgBody((u8*)&achMbAlias[0], wTypeLen);
}

/*====================================================================
������      : ConstructMsgToVmp
����        : ��֯������ϳ����Ĳ���
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CServMsg &cServMsg	[o]
����ֵ˵��  ��void
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
02/16/2011              Ѧ��          ����
====================================================================*/
void CParamToMpuVmp::ConstructMsgToVmp(CServMsg &cServMsg)
{
	CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(m_byConfIdx);
	if ( NULL == pVcInst )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ConstructMsgToVmp]ConfIdx.%d doesn't exist!\n", m_byConfIdx );
		return;
    }

	TConfInfo tConfInfo = pVcInst->m_tConf;
	cServMsg.SetConfId(tConfInfo.GetConfId());


	CKDVNewVMPParam cKDVVMPParam;
	memset(&cKDVVMPParam, 0, sizeof(cKDVVMPParam));

	//style
	cKDVVMPParam.m_byVMPStyle = m_tVmpCommonAttrb.m_byVmpStyle;
	
	//enctype
	cKDVVMPParam.m_byEncType = tConfInfo.GetMainVideoMediaType();

	//member number
	cKDVVMPParam.m_byMemberNum  = m_tVmpCommonAttrb.m_byMemberNum;

	//���ϳɳ�Ա���µ�TPeriEqpStatus��//is there need

	memcpy(cKDVVMPParam.m_atMtMember, m_tVmpCommonAttrb.m_atMtMember, sizeof(cKDVVMPParam.m_atMtMember));

	//payload and encypt key
	memcpy(cKDVVMPParam.m_tVideoEncrypt, m_tVmpCommonAttrb.m_tVideoEncrypt, sizeof(cKDVVMPParam.m_tVideoEncrypt));
	memcpy(cKDVVMPParam.m_tDoublePayload, m_tVmpCommonAttrb.m_tDoublePayload, sizeof(cKDVVMPParam.m_tDoublePayload));

	//fps:
	u8 byFrameRate = tConfInfo.GetMainVidUsrDefFPS();
	if( tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_H264)
	{
		//h264: the value is real, so maintain the value
	}
	else
	{
		byFrameRate = FrameRateMac2Real(byFrameRate);
	}
	cKDVVMPParam.SetFrameRate(byFrameRate);


	//resolution, BR info
	u16 wMinMtReqBitrate = 0;
	u16 wWidth = 0;
	u16 wHeight = 0;
	u8 byChnnlRes  = 0;
	u8 byMediaType = 0;
	u8 byVmpOutChnnl = 0;
	u8 byFRate = 0;
	u8 byAttrb = emBpAttrb;		//��ʱ����,�����ڴ���
	TPeriEqpStatus tVmpStatus;	//��ʱ����,�����ڴ���
	g_cMcuVcApp.GetPeriEqpStatus(m_byVmpId, &tVmpStatus);
    //u8 byVmpSubType = tVmpStatus.m_tStatus.tVmp.m_bySubType;	
	u8 byBoardVer	= tVmpStatus.m_tStatus.tVmp.m_byBoardVer;
	
	for (byVmpOutChnnl = 0; byVmpOutChnnl < MAXNUM_MPU_OUTCHNNL; byVmpOutChnnl++)
	{
		
		if (byBoardVer == MPU_BOARD_A128 && byVmpOutChnnl == 3) //A��ֻ��3·
		{
			break;
		}
		byChnnlRes = pVcInst->GetResByVmpOutChnnl(m_byVmpId, byMediaType, byFRate, byAttrb, byVmpOutChnnl);
		
		if (VIDEO_FORMAT_INVALID == byChnnlRes)
		{
			cKDVVMPParam.m_byEncType = MEDIA_TYPE_NULL;
			cKDVVMPParam.SetBitRate(0);
			cKDVVMPParam.SetVideoWidth(0);
			cKDVVMPParam.SetVideoHeight(0);
		}
		else
		{
			wMinMtReqBitrate = pVcInst->GetMinMtRcvBitByVmpChn(m_byVmpId, TRUE,byVmpOutChnnl);//songkun,20110530
			//mp4_auto �������ʱ��Ӧ�ֱ���
			if ( MEDIA_TYPE_MP4 == byMediaType
				&& VIDEO_FORMAT_AUTO == byChnnlRes )
			{
				if (wMinMtReqBitrate > 3072)
				{
					byChnnlRes = VIDEO_FORMAT_4CIF;
				} 
				else if(wMinMtReqBitrate > 1536)
				{
					byChnnlRes = VIDEO_FORMAT_2CIF;
				}
				else
				{
					byChnnlRes = VIDEO_FORMAT_CIF;
				}
			}
			GetWHByRes(byChnnlRes, wWidth, wHeight);
			
			// [pengjie 2010/1/20] Modify ��MP4��4cif �ϸ����ָ����
			if( VIDEO_FORMAT_4CIF == byChnnlRes && MEDIA_TYPE_MP4 == byMediaType)
			{
				wWidth  = 720;
				wHeight = 576;
			}
			// End Modify	
			
			cKDVVMPParam.SetVideoWidth(wWidth);
			cKDVVMPParam.SetVideoHeight(wHeight);
			cKDVVMPParam.m_byEncType = byMediaType;
			//	wMinMtReqBitrate = pVcInst->GetLstRcvMediaResMtBr(TRUE, byMediaType, byChnnlRes);
			g_cMcuVcApp.SetVMPOutChlBitrate((TEqp)tVmpStatus, byVmpOutChnnl, wMinMtReqBitrate);
			cKDVVMPParam.SetBitRate(wMinMtReqBitrate);
			
			//zbq[07/27/2009] �Ƿ�ǿ�Ʊ������1080i, ����720p
			if (g_cMcuVcApp.IsSVmpOutput1080i())
			{
				if (VIDEO_FORMAT_HD1080 == byChnnlRes)
				{
					cKDVVMPParam.SetVideoHeight(544);
				}
				else if (VIDEO_FORMAT_HD720 == byChnnlRes)
				{
					cKDVVMPParam.SetBitRate(0);
				}
			}
			
			// xliang [7/28/2009] �������û�и������������SVMP��EVPU��
			if( VIDEO_FORMAT_HD1080 != tConfInfo.GetMainVideoFormat() 
				&& VIDEO_FORMAT_HD720 != tConfInfo.GetMainVideoFormat() )
			{
				if( VIDEO_FORMAT_HD1080 == byChnnlRes || VIDEO_FORMAT_HD720 == byChnnlRes )
				{
					cKDVVMPParam.SetBitRate(0);
				}
			}
		}
		LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[ConstructMsgToVmp]set BR.%d for chn.%d\n", 
			cKDVVMPParam.GetBitRate(), byVmpOutChnnl);
		cServMsg.CatMsgBody((u8 *)&cKDVVMPParam, sizeof(cKDVVMPParam));
	}

	//need PRS
	u8 byNeedPrs = m_tVmpCommonAttrb.m_byNeedPrs;
	cServMsg.CatMsgBody((u8 *)&byNeedPrs, sizeof(byNeedPrs));

	//����ϳɷ��
    TVmpStyleCfgInfo tMcuVmpStyle = m_tVmpCommonAttrb.m_tVmpStyleCfgInfo;    
    cServMsg.CatMsgBody((u8*)&tMcuVmpStyle, sizeof(tMcuVmpStyle));

	//���û���ϳɿ���ͨ���ı���
	TEqpExCfgInfo tEqpExCfgInfo;
	if( SUCCESS_AGENT != g_cMcuAgent.GetEqpExCfgInfo( tEqpExCfgInfo ) )
	{
		tEqpExCfgInfo.Init();
	}
	
	TVMPExCfgInfo tVMPExCfgInfo;
	tVMPExCfgInfo = tEqpExCfgInfo.m_tVMPExCfgInfo;
    LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP,  "Set Vmp IdleChlShowMode: %d \n", tVMPExCfgInfo.m_byIdleChlShowMode );	
	cServMsg.CatMsgBody( (u8 *)&tVMPExCfgInfo, sizeof(TVMPExCfgInfo) );

}


/*====================================================================
������      : ConstructMsgToVmp
����        : ��֯������ϳ����Ĳ���
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CServMsg &cServMsg	[o]
����ֵ˵��  ��void
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
02/16/2011              Ѧ��          ����
====================================================================*/
void CParamTo8kg8khVmp::ConstructMsgToVmp(CServMsg &cServMsg)
{
	CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(m_byConfIdx);
	if ( NULL == pVcInst )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ConstructMsgToVmp]ConfIdx.%d doesn't exist!\n", m_byConfIdx );
		return;
    }

	TConfInfo tConfInfo = pVcInst->m_tConf;
	cServMsg.SetConfId(tConfInfo.GetConfId());
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( m_byVmpId, &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;

	C8KEVMPParam cKDVVMPParam;
	memset(&cKDVVMPParam, 0, sizeof(cKDVVMPParam));

	//style
	cKDVVMPParam.m_byVMPStyle = m_tVmpCommonAttrb.m_byVmpStyle;
	
	//enctype
	cKDVVMPParam.m_byEncType = tConfInfo.GetMainVideoMediaType();

	//member number
	cKDVVMPParam.m_byMemberNum  = m_tVmpCommonAttrb.m_byMemberNum;

	memcpy(cKDVVMPParam.m_atMtMember, m_tVmpCommonAttrb.m_atMtMember, sizeof(cKDVVMPParam.m_atMtMember));

	//payload and encypt key
	memcpy(cKDVVMPParam.m_tVideoEncrypt, m_tVmpCommonAttrb.m_tVideoEncrypt, sizeof(cKDVVMPParam.m_tVideoEncrypt));
	memcpy(cKDVVMPParam.m_tDoublePayload, m_tVmpCommonAttrb.m_tDoublePayload, sizeof(cKDVVMPParam.m_tDoublePayload));

	//��ȡ����ϳɿ���ͨ���ı���
	TEqpExCfgInfo tEqpExCfgInfo;
	if( SUCCESS_AGENT != g_cMcuAgent.GetEqpExCfgInfo( tEqpExCfgInfo ) )
	{
		tEqpExCfgInfo.Init();
	}
	
	TVMPExCfgInfo tVMPExCfgInfo;
	tVMPExCfgInfo = tEqpExCfgInfo.m_tVMPExCfgInfo;
	
	//���úϳ�ͼ���Ƿ���Ӻϳɳ�Ա����
    cKDVVMPParam.m_byIsDisplayMmbAlias = tVMPExCfgInfo.m_byIsDisplayMmbAlias;	
	
	LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP,  "[ConstructMsgToVmp]Vmp IdleChlShowMode: %d, Vmp IsDisplayMmbAlias: %d \n", 
		tVMPExCfgInfo.m_byIdleChlShowMode,
		tVMPExCfgInfo.m_byIsDisplayMmbAlias);

	//fps: no by now

	//resolution, BR info
	u16 wMinMtReqBitrate = 0;
	u16 wWidth = 0;
	u16 wHeight = 0;
	u8 byChnnlRes  = 0;
	u8 byMediaType = 0;
	u8 byVmpOutChnnl = 0;
	TPeriEqpStatus tVmpStatus;
	g_cMcuVcApp.GetPeriEqpStatus(m_byVmpId, &tVmpStatus);
    //u8 byVmpSubType = tVmpStatus.m_tStatus.tVmp.m_bySubType;	
	//u8 byBoardVer	= tVmpStatus.m_tStatus.tVmp.m_byBoardVer;
	
	u8 byMaxOutputChnnlNum = MAXNUM_MPU_OUTCHNNL;
	u8 abyChnlResEx[MAXNUM_MPU_OUTCHNNL] = { 0 };	//ͨ����Ӧ�ķֱ����Ƿ�ѡ��
	u8 abyOutChnlActive[MAXNUM_MPU_OUTCHNNL];		//���ͨ���Ƿ�Active,1:Active,0:��Active
	u8 abyOutChnlAttrb[MAXNUM_MPU_OUTCHNNL];		//8kh���ͨ����������(HP/BP),emBpAttrb/emHpAttrb
	memset(abyOutChnlActive,emVmpOutChnlNotChange,sizeof(abyOutChnlActive));
	memset(abyOutChnlAttrb, emBpAttrb, sizeof(abyOutChnlAttrb));//Ĭ��emBpAttrb
	if (VMP_8KH == byVmpSubType)
	{
		if ( pVcInst->IsMSSupportCapEx(VIDEO_FORMAT_HD720) )
		{
			abyChnlResEx[0] = 1;//1080
			abyChnlResEx[1] = 1;//720
		}
		if ( pVcInst->IsMSSupportCapEx(VIDEO_FORMAT_4CIF) )
		{
			abyChnlResEx[2] = 1;//4cif
		}
		if ( pVcInst->IsMSSupportCapEx(VIDEO_FORMAT_CIF) )
		{
			abyChnlResEx[3] = 1;//cif
		}
		if( MEDIA_TYPE_H264 == tConfInfo.GetMainVideoMediaType() ) 
		{
			if (VIDEO_FORMAT_HD1080 == tConfInfo.GetMainVideoFormat() ) //����ʽ1080
			{
				abyChnlResEx[0] = 1;//1080
				// 1080p60fps����ʱ,��1·���ܳ�720p
				if (tConfInfo.GetMainVidUsrDefFPS() >= 50)
				{
					abyChnlResEx[1] = 1;//720
				}
			}
			if (VIDEO_FORMAT_HD720 == tConfInfo.GetMainVideoFormat() ) //����ʽ720
			{
				abyChnlResEx[1] = 1;//720
				// 720HP����ʱ,��0·��HP
				if (emHpAttrb == tConfInfo.GetProfileAttrb())
				{
					abyChnlResEx[0] = 1;//720 HP
				}
			}
			if (VIDEO_FORMAT_4CIF == tConfInfo.GetMainVideoFormat() ) //����ʽ4cif
			{
				abyChnlResEx[2] = 1;//4cif
			}
			if (VIDEO_FORMAT_CIF == tConfInfo.GetMainVideoFormat() ) //����ʽcif
			{
				abyChnlResEx[3] = 1;//cif
			}
		}
		else if( MEDIA_TYPE_H264 == tConfInfo.GetSecVideoMediaType() ) //����ʽ��h264
		{
			
			if (VIDEO_FORMAT_HD1080 == tConfInfo.GetSecVideoFormat() ) //����ʽ1080
			{
				abyChnlResEx[0] = 1;//1080
			}
			if (VIDEO_FORMAT_HD720 == tConfInfo.GetSecVideoFormat() ) //����ʽ720
			{
				abyChnlResEx[1] = 1;//720
			}
			if (VIDEO_FORMAT_4CIF == tConfInfo.GetSecVideoFormat() ) //����ʽ4cif
			{
				abyChnlResEx[2] = 1;//4cif
			}
			if (VIDEO_FORMAT_CIF == tConfInfo.GetSecVideoFormat() ) //����ʽcif
			{
				abyChnlResEx[3] = 1;//cif
			}		
		}
		abyChnlResEx[MAXNUM_MPU_OUTCHNNL-1] = 1;//other�϶���

		// 1080p30fps�������vmp�ǹ㲥ʱ,vmp������1080p30Ҫ��Active
		if (pVcInst->IsConfExcludeDDSUseAdapt() && !tPeriEqpStatus.m_tStatus.tVmp.GetVmpParam().IsVMPBrdst())
		{
			//�ж�����ʽ
			if (tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
				tConfInfo.GetMainVideoFormat() == VIDEO_FORMAT_HD1080 &&
				tConfInfo.GetMainVidUsrDefFPS() >= 25 &&
				tConfInfo.GetMainVidUsrDefFPS() <= 30)
			{
				//vmp�������0·1080p30Ҫ��Active
				abyOutChnlActive[0] = emVmpOutChnlInactive;
			}
			if (tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
				tConfInfo.GetMainVideoFormat() == VIDEO_FORMAT_HD720 &&
				tConfInfo.GetMainVidUsrDefFPS() >= 25 &&
				tConfInfo.GetMainVidUsrDefFPS() <= 30 &&
				tConfInfo.GetProfileAttrb() == emHpAttrb)
			{
				//vmp�������0·720p30HPҪ��Active
				abyOutChnlActive[0] = emVmpOutChnlInactive;
			}
			//�жϸ���ʽ
			if (tConfInfo.GetSecVideoMediaType() == MEDIA_TYPE_H264 &&
				tConfInfo.GetSecVideoFormat() == VIDEO_FORMAT_HD1080 &&
				tConfInfo.GetSecVidUsrDefFPS() >= 25 &&
				tConfInfo.GetSecVidUsrDefFPS() <= 30)
			{
				//vmp�������0·1080p30Ҫ��Active
				abyOutChnlActive[0] = emVmpOutChnlInactive;
			}
		}
	}

	u8 byFrameRate = 0;
	for (byVmpOutChnnl = 0; byVmpOutChnnl < byMaxOutputChnnlNum; byVmpOutChnnl ++)
	{
		
		byChnnlRes = pVcInst->GetResByVmpOutChnnl(m_byVmpId, byMediaType, byFrameRate, abyOutChnlAttrb[byVmpOutChnnl], byVmpOutChnnl);
		//zhouyiliang20110228 H264 auto����������MPEG4 16cif��������,��ʱ�²�Ӧ����mpeg4 4cif
// 		if (MEDIA_TYPE_MP4 == byMediaType && VIDEO_FORMAT_16CIF == byChnnlRes ) 
// 		{
// 			byChnnlRes = VIDEO_FORMAT_4CIF;
// 		}
		if(VMP_8KH == byVmpSubType)
		{
			//����֡��
			//byFrameRate = tConfInfo.GetMainVidUsrDefFPS();
			if( byMediaType == MEDIA_TYPE_H264)
			{
				//h264: the value is real, so maintain the value
			}
			else
			{
				byFrameRate = FrameRateMac2Real(byFrameRate);
			}
			
			//����ʽ���� 50/60�Ķ�Ҫ����
			if (MEDIA_TYPE_H264 == tConfInfo.GetMainVideoMediaType()  && byFrameRate >= 50 ) 
			{
				byFrameRate = byFrameRate/2;
			}
			cKDVVMPParam.m_byFrameRate = byFrameRate;
			LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"Set outchnl:%d Vmp frame rate to %d\n", byVmpOutChnnl,byFrameRate);
			
			if ( !pVcInst->IsConfExcludeDDSUseAdapt() )
			{
				//8000H����ʽ����ֻ��һ·,������·�¿յĲ���
				if (byMediaType != tConfInfo.GetMainVideoMediaType() || byChnnlRes != tConfInfo.GetMainVideoFormat() )
				{
					if (emHpAttrb == tConfInfo.GetProfileAttrb() && 0 == byVmpOutChnnl)
					{
						// ����ʽHP����,��0·��720p/HP,�ֱ��ʲ�һ��,���¿�
					}
					else if (VIDEO_FORMAT_16CIF == tConfInfo.GetMainVideoFormat() && 4 == byVmpOutChnnl)
					{
						// �侯MPEG4 16CIF����,��4·��MPEG4/4CIF,�ֱ��ʲ�һ��,���¿�
					} 
					else
					{
						byChnnlRes = VIDEO_FORMAT_INVALID;
					}
				}
			}
			else//�������720 50/60���鲻��1080 ��һ·�������������ʲô�ͳ���·��˫other����·
			{
				if ( MEDIA_TYPE_H264 == byMediaType && 1 != abyChnlResEx[byVmpOutChnnl])
				{
					byChnnlRes = VIDEO_FORMAT_INVALID;
				}
				
				if (MEDIA_TYPE_H264 != tConfInfo.GetMainVideoMediaType() &&  MEDIA_TYPE_NULL != tConfInfo.GetMainVideoMediaType() 
					&&  MEDIA_TYPE_H264 != tConfInfo.GetSecVideoMediaType() && MEDIA_TYPE_NULL != tConfInfo.GetSecVideoMediaType()) //˫other
				{
					if (byMediaType != tConfInfo.GetMainVideoMediaType() && byMediaType != tConfInfo.GetSecVideoMediaType())
					{
						byChnnlRes = VIDEO_FORMAT_INVALID;
					}
				}
				
			}
		}
		if( byChnnlRes == VIDEO_FORMAT_INVALID)
		{
			cKDVVMPParam.m_byEncType = MEDIA_TYPE_NULL;
			cKDVVMPParam.m_wBitRate = 0;
			cKDVVMPParam.m_wVideoHeight = 0;
			cKDVVMPParam.m_wVideoWidth = 0;
		}
		else
		{
			//wMinMtReqBitrate = pVcInst->GetLstRcvMediaResMtBr(TRUE, byMediaType, byChnnlRes);
            wMinMtReqBitrate = pVcInst->GetMinMtRcvBitByVmpChn(m_byVmpId, TRUE,byVmpOutChnnl);//songkun,20110530
			//mp4_auto �������ʱ��Ӧ�ֱ���
			if ( MEDIA_TYPE_MP4 == byMediaType
				&& VIDEO_FORMAT_AUTO == byChnnlRes )
			{
				if (wMinMtReqBitrate > 3072)
				{
					byChnnlRes = VIDEO_FORMAT_4CIF;
				} 
				else if(wMinMtReqBitrate > 1536)
				{
					byChnnlRes = VIDEO_FORMAT_2CIF;
				}
				else
				{
					byChnnlRes = VIDEO_FORMAT_CIF;
				}
			}
			GetWHByRes(byChnnlRes, wWidth, wHeight);
			// ��MP4��4cif �ϸ����ָ����
			if( VIDEO_FORMAT_4CIF == byChnnlRes && MEDIA_TYPE_MP4 == byMediaType)
			{
				wWidth  = 720;
				wHeight = 576;
			}

			cKDVVMPParam.SetVideoWidth(wWidth);
			cKDVVMPParam.SetVideoHeight(wHeight);
			cKDVVMPParam.m_byEncType = byMediaType;
			g_cMcuVcApp.SetVMPOutChlBitrate((TEqp)tVmpStatus, byVmpOutChnnl, wMinMtReqBitrate);
			cKDVVMPParam.SetBitRate(wMinMtReqBitrate);
		}
		
		cServMsg.CatMsgBody((u8 *)&cKDVVMPParam, sizeof(cKDVVMPParam));
	}

	//need PRS
	u8 byNeedPrs = m_tVmpCommonAttrb.m_byNeedPrs;
	cServMsg.CatMsgBody((u8 *)&byNeedPrs, sizeof(byNeedPrs));

	//����ϳɷ��
    TVmpStyleCfgInfo tMcuVmpStyle = m_tVmpCommonAttrb.m_tVmpStyleCfgInfo;    
    cServMsg.CatMsgBody((u8*)&tMcuVmpStyle, sizeof(tMcuVmpStyle));

	//���û���ϳɿ���ͨ���ı���
	cServMsg.CatMsgBody( (u8 *)&tVMPExCfgInfo, sizeof(TVMPExCfgInfo) );
	
	// ׷�ӽ���ͨ��RcvH264DependInMark����,8kg/8kh/8ki��֧��
	for (u8 byIdx=0; byIdx<MAXNUM_SDVMP_MEMBER; byIdx++)
	{
		LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"VMP chnl[%d] RcvH264DependInMark: %d.\n", 
			byIdx, m_tVmpCommonAttrb.m_abyRcvH264DependInMark[byIdx]);
		cServMsg.CatMsgBody( (u8 *)&m_tVmpCommonAttrb.m_abyRcvH264DependInMark[byIdx],sizeof(u8));
	}

    // ׷��Vmp��Ա���� [7/4/2013 liaokang]
    // �ṹ��(u8) ChnNum + ( (u8)ChnNo + (u8)Len + (s8*)content + ��)
    s8  achMbAlias[1 + (1 + 1 + MAXLEN_ALIAS)*MAXNUM_SDVMP_MEMBER] = {0};
    u8 *pchMbAlias = (u8*)achMbAlias;
    u8  byChnNum = 0;
    u16 wTypeLen = 1;
    pchMbAlias += wTypeLen; 
    s8  achAlias[MAXLEN_ALIAS] = {0}; 
    u8  byAliasLen = 0;
    for(u8 byChnNo=0; byChnNo<m_tVmpCommonAttrb.m_byMemberNum; byChnNo++)
    {
        if(m_tVmpCommonAttrb.m_atMtMember[byChnNo].IsNull())
        {
            continue;
        }
        
        memset(achAlias,0,sizeof(achAlias));
        if( !( pVcInst->GetMtAliasToVmp(m_tVmpCommonAttrb.m_atMtMember[byChnNo],achAlias) ) )
        {
            continue;
        }
        byChnNum++;
        byAliasLen = strlen(achAlias);
        *pchMbAlias = byChnNo;
        pchMbAlias++;
        *pchMbAlias = byAliasLen;
        pchMbAlias++;
        memcpy(pchMbAlias,achAlias,byAliasLen);
        pchMbAlias += byAliasLen;
        wTypeLen += (2 + byAliasLen);
    }
    pchMbAlias -= wTypeLen;
    *pchMbAlias = byChnNum;
    cServMsg.CatMsgBody((u8*)&achMbAlias[0], wTypeLen);

	if (VMP_8KH == byVmpSubType)
	{
		for (u8 byIdx=0; byIdx<sizeof(abyOutChnlActive); byIdx++)
		{
			LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"VMP Outchnl[%d] Active: %d, ProfileAttrb: %d.\n", 
				byIdx, abyOutChnlActive[byIdx], abyOutChnlAttrb[byIdx]);
		}
		//8kh����ϳɴ���ͨ���Ƿ�Active����
		cServMsg.CatMsgBody( abyOutChnlActive,sizeof(abyOutChnlActive));
		//8kh����ϳɴ���ͨ��HB/BP����
		cServMsg.CatMsgBody( abyOutChnlAttrb,sizeof(abyOutChnlAttrb));
	}
}

/*====================================================================
������      : ConstructMsgToVmp
����        : ��֯������ϳ����Ĳ���(�²��ع�׼��)
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CServMsg &cServMsg	[o]
����ֵ˵��  ��void
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
08/05/2013            yanghuaizhi       ����
====================================================================*/
/*void CParamTo8kiVmp::ConstructMsgToVmp(CServMsg &cServMsg)
{
	CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(m_byConfIdx);
	if ( NULL == pVcInst )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ConstructMsgToVmp]ConfIdx.%d doesn't exist!\n", m_byConfIdx );
		return;
    }

	TConfInfo tConfInfo = pVcInst->m_tConf;
	cServMsg.SetConfId(tConfInfo.GetConfId());
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( m_byVmpId, &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;

	T8KEVmpCommonAttrb t8keVmpCommonAttrb;

	//style
	t8keVmpCommonAttrb.m_byVmpStyle = m_tVmpCommonAttrb.m_byVmpStyle;
	
	//member number
	t8keVmpCommonAttrb.m_byMemberNum  = m_tVmpCommonAttrb.m_byMemberNum;

	u8 byIdx;
    s8  achAlias[MAXLEN_ALIAS] = {0}; 
    u8  byAliasLen = 0;
	for (byIdx = 0; byIdx < MAXNUM_SDVMP_MEMBER; byIdx++)
	{
		// ���ó�Ա��Ϣ
		t8keVmpCommonAttrb.m_atMtMember[byIdx].SetMember(m_tVmpCommonAttrb.m_atMtMember[byIdx]);
		// ���ó�Ա������128����
		memset(achAlias, 0, sizeof(achAlias));
        if( pVcInst->GetMtAliasToVmp(m_tVmpCommonAttrb.m_atMtMember[byIdx],achAlias))
        {
            t8keVmpCommonAttrb.m_atMtMember[byIdx].SetMbAlias(strlen(achAlias), achAlias);
        }
		//payload and encypt key
		t8keVmpCommonAttrb.m_tVideoEncrypt[byIdx] = m_tVmpCommonAttrb.m_tVideoEncrypt[byIdx];
		t8keVmpCommonAttrb.m_tDoublePayload[byIdx] = m_tVmpCommonAttrb.m_tDoublePayload[byIdx];
		t8keVmpCommonAttrb.m_abyRcvH264DependInMark[byIdx] = m_tVmpCommonAttrb.m_abyRcvH264DependInMark[byIdx];
	}
	t8keVmpCommonAttrb.m_tVmpStyleCfgInfo = m_tVmpCommonAttrb.m_tVmpStyleCfgInfo;		//������Ϣ
	t8keVmpCommonAttrb.m_tVmpExCfgInfo = m_tVmpCommonAttrb.m_tVmpExCfgInfo;		//��չ������Ϣ
	// ����ͨ����Ϣ
	cServMsg.SetMsgBody((u8*)&t8keVmpCommonAttrb, sizeof(t8keVmpCommonAttrb));

	CKDVVMPOutMember cVMPOutMember;
	u8 byChnnlRes  = 0;
	u16 wWidth		= 0;	//��
	u16 wHeight		= 0;	//��
	u16 wMinMtReqBitrate = 0;
	u8 byFrameRate = 0;
	TVideoStreamCap tStrCap;
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( m_byVmpId );
	u8 byMaxOutputChnnlNum = MAXNUM_MPU_OUTCHNNL;
	u8 abyOutChnlActive[MAXNUM_MPU_OUTCHNNL];		//���ͨ���Ƿ�Active,1:Active,0:��Active
	memset(abyOutChnlActive,emVmpOutChnlNotChange,sizeof(abyOutChnlActive));
	for (u8 byVmpOutChnnl = 0; byVmpOutChnnl < byMaxOutputChnnlNum; byVmpOutChnnl ++)
	{
		cVMPOutMember.Clear();
		// �����Ƶ����
		tStrCap = m_tVMPOutParam.GetVmpOutCapIdx(byVmpOutChnnl);
		
		if (MEDIA_TYPE_NULL != tStrCap.GetMediaType())
		{
			//���Idx��Ӧ������
			cVMPOutMember.SetEncType(tStrCap.GetMediaType());
			byChnnlRes = tStrCap.GetResolution();
			wMinMtReqBitrate = pVcInst->GetMinMtRcvBitByVmpChn(m_byVmpId, TRUE,byVmpOutChnnl);
			//mp4_auto �������ʱ��Ӧ�ֱ���
			if ( MEDIA_TYPE_MP4 == tStrCap.GetMediaType()
				&& VIDEO_FORMAT_AUTO == byChnnlRes )
			{
				if (wMinMtReqBitrate > 3072)
				{
					byChnnlRes = VIDEO_FORMAT_4CIF;
				} 
				else if(wMinMtReqBitrate > 1536)
				{
					byChnnlRes = VIDEO_FORMAT_2CIF;
				}
				else
				{
					byChnnlRes = VIDEO_FORMAT_CIF;
				}
			}
			// mp4 16cif �²�Ϊmpeg4 4cif
			else if (MEDIA_TYPE_MP4 == tStrCap.GetMediaType()
				&& VIDEO_FORMAT_16CIF == byChnnlRes )
			{
				byChnnlRes = VIDEO_FORMAT_4CIF;
			}
			// h264 auto �²�Ϊcif
			else if (MEDIA_TYPE_H264 == tStrCap.GetMediaType()
				&& VIDEO_FORMAT_AUTO == byChnnlRes )
			{
				byChnnlRes = VIDEO_FORMAT_CIF;
			}
			//������������÷ֱ���
			GetWHByRes(byChnnlRes , wWidth, wHeight);
			// ��MP4��4cif �ϸ����ָ����
			if( MEDIA_TYPE_MP4 == tStrCap.GetMediaType() && VIDEO_FORMAT_4CIF == tStrCap.GetResolution())
			{
				wWidth  = 720;
				wHeight = 576;
			}
			
			//fps:
			byFrameRate = tStrCap.GetUserDefFrameRate();
			if( tStrCap.GetMediaType() == MEDIA_TYPE_H264)
			{
				//h264: the value is real, so maintain the value
			}
			else
			{
				byFrameRate = FrameRateMac2Real(byFrameRate);
			}

			cVMPOutMember.SetVideoWidth(wWidth);
			cVMPOutMember.SetVideoHeight(wHeight);
			cVMPOutMember.SetProfileType(tStrCap.GetH264ProfileAttrb());
			cVMPOutMember.SetFrameRate(byFrameRate);
			if (tStrCap.GetMaxBitRate() > 0)
			{
				// ���ʲ�Ϊ0ʱ�Ÿ��¶�Ӧͨ�����ʣ�8kivmp�ǹ㲥����ֹͣĳ·����
				g_cMcuVcApp.SetVMPOutChlBitrate((TEqp)tPeriEqpStatus, byVmpOutChnnl, wMinMtReqBitrate);
			}
			else
			{
				// ������Ϊ0����·������
				abyOutChnlActive[byVmpOutChnnl] = emVmpOutChnlInactive;
			}
			cVMPOutMember.SetBitRate(wMinMtReqBitrate);//�ճ��²�
		}
		else
		{
			cVMPOutMember.SetEncType(MEDIA_TYPE_NULL);
		}
		
		cServMsg.CatMsgBody((u8 *)&cVMPOutMember, sizeof(cVMPOutMember));
	}

}*/

/*====================================================================
������      : ConstructMsgToVmp
����        : ��֯������ϳ����Ĳ���
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����CServMsg &cServMsg	[o]
����ֵ˵��  ��void
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
08/05/2013            yanghuaizhi       ����
====================================================================*/
void CParamTo8kiVmp::ConstructMsgToVmp(CServMsg &cServMsg)
{
	CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(m_byConfIdx);
	if ( NULL == pVcInst )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ConstructMsgToVmp]ConfIdx.%d doesn't exist!\n", m_byConfIdx );
		return;
    }

	TConfInfo tConfInfo = pVcInst->m_tConf;
	cServMsg.SetConfId(tConfInfo.GetConfId());
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( m_byVmpId, &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;

	C8KEVMPParam cKDVVMPParam;
	memset(&cKDVVMPParam, 0, sizeof(cKDVVMPParam));

	//style
	cKDVVMPParam.m_byVMPStyle = m_tVmpCommonAttrb.m_byVmpStyle;
	
	//enctype
	cKDVVMPParam.m_byEncType = tConfInfo.GetMainVideoMediaType();

	//member number
	cKDVVMPParam.m_byMemberNum  = m_tVmpCommonAttrb.m_byMemberNum;

	memcpy(cKDVVMPParam.m_atMtMember, m_tVmpCommonAttrb.m_atMtMember, sizeof(cKDVVMPParam.m_atMtMember));

	//payload and encypt key
	memcpy(cKDVVMPParam.m_tVideoEncrypt, m_tVmpCommonAttrb.m_tVideoEncrypt, sizeof(cKDVVMPParam.m_tVideoEncrypt));
	memcpy(cKDVVMPParam.m_tDoublePayload, m_tVmpCommonAttrb.m_tDoublePayload, sizeof(cKDVVMPParam.m_tDoublePayload));

	//��ȡ����ϳɿ���ͨ���ı���
	TEqpExCfgInfo tEqpExCfgInfo;
	if( SUCCESS_AGENT != g_cMcuAgent.GetEqpExCfgInfo( tEqpExCfgInfo ) )
	{
		tEqpExCfgInfo.Init();
	}
	
	TVMPExCfgInfo tVMPExCfgInfo;
	tVMPExCfgInfo = tEqpExCfgInfo.m_tVMPExCfgInfo;
	
	//���úϳ�ͼ���Ƿ���Ӻϳɳ�Ա����
    cKDVVMPParam.m_byIsDisplayMmbAlias = tVMPExCfgInfo.m_byIsDisplayMmbAlias;	
	
	LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP,  "[ConstructMsgToVmp]Vmp IdleChlShowMode: %d, Vmp IsDisplayMmbAlias: %d \n", 
		tVMPExCfgInfo.m_byIdleChlShowMode,
		tVMPExCfgInfo.m_byIsDisplayMmbAlias);

	//fps: no by now

	//resolution, BR info
	u16 wMinMtReqBitrate = 0;
	u16 wWidth = 0;
	u16 wHeight = 0;
	u8 byChnnlRes  = 0;
	u8 byVmpOutChnnl = 0;
	TPeriEqpStatus tVmpStatus;
	g_cMcuVcApp.GetPeriEqpStatus(m_byVmpId, &tVmpStatus);
	
	u8 byMaxOutputChnnlNum = MAXNUM_MPU_OUTCHNNL;
	//u8 abyChnlResEx[MAXNUM_MPU_OUTCHNNL] = { 0 };	//ͨ����Ӧ�ķֱ����Ƿ�ѡ��
	u8 abyOutChnlActive[MAXNUM_MPU_OUTCHNNL];		//���ͨ���Ƿ�Active,1:Active,0:��Active
	u8 abyOutChnlAttrb[MAXNUM_MPU_OUTCHNNL];		//8kh���ͨ����������(HP/BP),emBpAttrb/emHpAttrb
	memset(abyOutChnlActive,emVmpOutChnlNotChange,sizeof(abyOutChnlActive));
	memset(abyOutChnlAttrb, emBpAttrb, sizeof(abyOutChnlAttrb));//Ĭ��emBpAttrb
	
	u8 byFrameRate = 0;
	TVideoStreamCap tStrCap;
	for (byVmpOutChnnl = 0; byVmpOutChnnl < byMaxOutputChnnlNum; byVmpOutChnnl ++)
	{
		// �����Ƶ����
		tStrCap = m_tVMPOutParam.GetVmpOutCapIdx(byVmpOutChnnl);

		if (MEDIA_TYPE_NULL != tStrCap.GetMediaType())
		{
			//���Idx��Ӧ������
			byChnnlRes = tStrCap.GetResolution();
			wMinMtReqBitrate = pVcInst->GetMinMtRcvBitByVmpChn(m_byVmpId, TRUE,byVmpOutChnnl);
			//mp4_auto �������ʱ��Ӧ�ֱ���
			if ( MEDIA_TYPE_MP4 == tStrCap.GetMediaType()
				&& VIDEO_FORMAT_AUTO == byChnnlRes )
			{
				if (wMinMtReqBitrate > 3072)
				{
					byChnnlRes = VIDEO_FORMAT_4CIF;
				} 
				else if(wMinMtReqBitrate > 1536)
				{
					byChnnlRes = VIDEO_FORMAT_2CIF;
				}
				else
				{
					byChnnlRes = VIDEO_FORMAT_CIF;
				}
			}
			// mp4 16cif �²�Ϊmpeg4 4cif
			else if (MEDIA_TYPE_MP4 == tStrCap.GetMediaType()
				&& VIDEO_FORMAT_16CIF == byChnnlRes )
			{
				byChnnlRes = VIDEO_FORMAT_4CIF;
			}
			// h264 auto �²�Ϊcif
			else if (MEDIA_TYPE_H264 == tStrCap.GetMediaType()
				&& VIDEO_FORMAT_AUTO == byChnnlRes )
			{
				byChnnlRes = VIDEO_FORMAT_CIF;
			}
			//������������÷ֱ���
			GetWHByRes(byChnnlRes , wWidth, wHeight);
			// ��MP4��4cif �ϸ����ָ����
			if( MEDIA_TYPE_MP4 == tStrCap.GetMediaType() && VIDEO_FORMAT_4CIF == tStrCap.GetResolution())
			{
				wWidth  = 720;
				wHeight = 576;
			}
			
			//fps:
			byFrameRate = tStrCap.GetUserDefFrameRate();
			if( tStrCap.GetMediaType() == MEDIA_TYPE_H264)
			{
				//h264: the value is real, so maintain the value
			}
			else
			{
				byFrameRate = FrameRateMac2Real(byFrameRate);
			}

			cKDVVMPParam.SetVideoWidth(wWidth);
			cKDVVMPParam.SetVideoHeight(wHeight);
			cKDVVMPParam.m_byEncType = tStrCap.GetMediaType();
			if (tStrCap.GetMaxBitRate() > 0)
			{
				// ���ʲ�Ϊ0ʱ�Ÿ��¶�Ӧͨ�����ʣ�8kivmp�ǹ㲥����ֹͣĳ·����
				g_cMcuVcApp.SetVMPOutChlBitrate((TEqp)tVmpStatus, byVmpOutChnnl, wMinMtReqBitrate);
			}
			else
			{
				// ������Ϊ0����·������
				abyOutChnlActive[byVmpOutChnnl] = emVmpOutChnlInactive;
			}
			cKDVVMPParam.SetBitRate(wMinMtReqBitrate);//�ճ��²�
			abyOutChnlAttrb[byVmpOutChnnl] = tStrCap.GetH264ProfileAttrb();//��¼HP/BP����
			cKDVVMPParam.m_byFrameRate = byFrameRate;
		}
		else
		{
			cKDVVMPParam.m_byEncType = MEDIA_TYPE_NULL;
			cKDVVMPParam.m_wBitRate = 0;
			cKDVVMPParam.m_wVideoHeight = 0;
			cKDVVMPParam.m_wVideoWidth = 0;
			cKDVVMPParam.m_byFrameRate = 0;
		}
		
		cServMsg.CatMsgBody((u8 *)&cKDVVMPParam, sizeof(cKDVVMPParam));
	}

	//need PRS
	u8 byNeedPrs = m_tVmpCommonAttrb.m_byNeedPrs;
	cServMsg.CatMsgBody((u8 *)&byNeedPrs, sizeof(byNeedPrs));

	//����ϳɷ��
    TVmpStyleCfgInfo tMcuVmpStyle = m_tVmpCommonAttrb.m_tVmpStyleCfgInfo;    
    cServMsg.CatMsgBody((u8*)&tMcuVmpStyle, sizeof(tMcuVmpStyle));

	//���û���ϳɿ���ͨ���ı���
	cServMsg.CatMsgBody( (u8 *)&tVMPExCfgInfo, sizeof(TVMPExCfgInfo) );
	
	// ׷�ӽ���ͨ��RcvH264DependInMark����,8kg/8kh/8ki��֧��
	for (u8 byIdx=0; byIdx<MAXNUM_SDVMP_MEMBER; byIdx++)
	{
		LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"VMP chnl[%d] RcvH264DependInMark: %d.\n", 
			byIdx, m_tVmpCommonAttrb.m_abyRcvH264DependInMark[byIdx]);
		cServMsg.CatMsgBody( (u8 *)&m_tVmpCommonAttrb.m_abyRcvH264DependInMark[byIdx],sizeof(u8));
	}

    // ׷��Vmp��Ա���� [7/4/2013 liaokang]
    // �ṹ��(u8) ChnNum + ( (u8)ChnNo + (u8)Len + (s8*)content + ��)
    s8  achMbAlias[1 + (1 + 1 + MAXLEN_ALIAS)*MAXNUM_SDVMP_MEMBER] = {0};
    u8 *pchMbAlias = (u8*)achMbAlias;
    u8  byChnNum = 0;
    u16 wTypeLen = 1;
    pchMbAlias += wTypeLen; 
    s8  achAlias[MAXLEN_ALIAS] = {0}; 
    u8  byAliasLen = 0;
    for(u8 byChnNo=0; byChnNo<m_tVmpCommonAttrb.m_byMemberNum; byChnNo++)
    {
        if(m_tVmpCommonAttrb.m_atMtMember[byChnNo].IsNull())
        {
            continue;
        }
        
        memset(achAlias,0,sizeof(achAlias));
        if( !( pVcInst->GetMtAliasToVmp(m_tVmpCommonAttrb.m_atMtMember[byChnNo],achAlias) ) )
        {
            continue;
        }
        byChnNum++;
        byAliasLen = strlen(achAlias);
        *pchMbAlias = byChnNo;
        pchMbAlias++;
        *pchMbAlias = byAliasLen;
        pchMbAlias++;
        memcpy(pchMbAlias,achAlias,byAliasLen);
        pchMbAlias += byAliasLen;
        wTypeLen += (2 + byAliasLen);
    }
    pchMbAlias -= wTypeLen;
    *pchMbAlias = byChnNum;
    cServMsg.CatMsgBody((u8*)&achMbAlias[0], wTypeLen);
	
	//8ki����ϳɴ���ͨ���Ƿ�Active����
	cServMsg.CatMsgBody( abyOutChnlActive,sizeof(abyOutChnlActive));
	//8ki����ϳɴ���ͨ��HB/BP����
	cServMsg.CatMsgBody( abyOutChnlAttrb,sizeof(abyOutChnlAttrb));

}

/*====================================================================
������      : IsConfAdpOptimization
����        : ��ǰ�����Ƿ�������Դ�������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
11/04/25                �ܼ���          ����
====================================================================*/
BOOL32 IsConfAdpOptimization(const TConfInfo& tConfinfo)
{
	TConfAttrbEx tConfAttrbEx = tConfinfo.GetConfAttrbEx();
	return tConfAttrbEx.IsImaxModeConf();
}

/*====================================================================
������      : IsConfNeedReserveMainCap
����        : ��ǰ�����Ƿ���ҪԤ����������ʽ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/01/31              ��־��           ����
====================================================================*/
BOOL32	IsConfNeedReserveMainCap(const TConfInfo& tConfinfo, const TConfInfoEx &tConfInfoEx)
{
#if defined(_8KH_) || defined(_8KI_)
	if ( tConfinfo.GetProfileAttrb() == emHpAttrb )
	{
		//1080HP,��ѡ1080BP����Ԥ��HP
		if ( tConfinfo.GetMainVideoFormat() == VIDEO_FORMAT_HD1080 &&
			 tConfInfoEx.IsMSSupportCapEx(VIDEO_FORMAT_HD1080) 
			)
		{
			return FALSE;
		}

		//���٣��޹�ѡ���޸���ʽ���飬��Ԥ�����������ȫ��Ԥ�� 720 30 HP
		//˫�ٽ���Ҳ����Ԥ��
		if (  tConfinfo.GetSecBitRate() ==0 && 
			  !tConfInfoEx.IsMainHasCapEx() &&
			  tConfinfo.GetSecVideoMediaType() == MEDIA_TYPE_NULL
			)
		{
			return FALSE;
		}

		
#ifndef _8KI_
		return TRUE;
#endif

	}
	else
	{
#ifndef _8KI_
		//8000H,����ʽ�����720 60/50 BP�����ҹ�ѡ720 30/25 BP ֻԤ��һ·,�˴���Ԥ��
		if ( tConfInfoEx.IsMainHasCapEx() &&
			tConfinfo.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
			tConfinfo.GetMainVideoFormat() == VIDEO_FORMAT_HD720 &&
			(tConfinfo.GetMainVidUsrDefFPS() == 60 || tConfinfo.GetMainVidUsrDefFPS() == 50) &&
			( tConfInfoEx.IsMSSupportCapEx(VIDEO_FORMAT_HD720,tConfinfo.GetMainVidUsrDefFPS()/2,emBpAttrb) == TRUE )
			)
		{
			return FALSE;
		}
#endif
		
		//8000H,����ʽ�����1080 60�����ҹ�ѡ1080 30/25 ֻԤ��һ·
		if ( tConfInfoEx.IsMainHasCapEx() &&
			tConfinfo.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
			tConfinfo.GetMainVideoFormat() == VIDEO_FORMAT_HD1080 &&
			(tConfinfo.GetMainVidUsrDefFPS() == 60 || tConfinfo.GetMainVidUsrDefFPS() == 50) &&
			( tConfInfoEx.IsMSSupportCapEx(VIDEO_FORMAT_HD1080) == TRUE )
			)
		{
			return FALSE;
		}
	}
#endif

	//8ke ��֧��1080����
#ifdef _8KE_
	if ( 	tConfinfo.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
			tConfinfo.GetMainVideoFormat() == VIDEO_FORMAT_HD1080  
		)
	{
		return FALSE;
	}

#endif

#ifdef _MINIMCU_
	//������ʽ��H264��other����������ʽ�޹�ѡ������ҪԤ��,8000B,�Ƚ����⣬ֻ��1������H264 CIF��Ԥ��
	if (  (!tConfInfoEx.IsMainHasCapEx() &&
		tConfinfo.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
		tConfinfo.GetMainVideoFormat() != VIDEO_FORMAT_CIF &&
		tConfinfo.GetSecVideoMediaType() != MEDIA_TYPE_NULL &&
		tConfinfo.GetSecVideoMediaType() != MEDIA_TYPE_H264) 
		||
		(!tConfInfoEx.IsMainHasCapEx() &&
		tConfinfo.GetMainVideoMediaType() != MEDIA_TYPE_H264 &&
		tConfinfo.GetMainVideoMediaType() != MEDIA_TYPE_NULL &&
		tConfinfo.GetSecVideoMediaType()  == MEDIA_TYPE_H264&&
		tConfinfo.GetSecVideoFormat() != VIDEO_FORMAT_CIF 
		)
	   )
	{
		return TRUE;
	}
#else
	//������ʽ��H264��other����������ʽ�޹�ѡ������ҪԤ��
	if (  (!tConfInfoEx.IsMainHasCapEx() &&
		tConfinfo.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
		tConfinfo.GetSecVideoMediaType() != MEDIA_TYPE_NULL &&
		tConfinfo.GetSecVideoMediaType() != MEDIA_TYPE_H264) 
		||
		(!tConfInfoEx.IsMainHasCapEx() &&
		tConfinfo.GetMainVideoMediaType() != MEDIA_TYPE_H264 &&
		tConfinfo.GetMainVideoMediaType() != MEDIA_TYPE_NULL &&
		tConfinfo.GetSecVideoMediaType()  == MEDIA_TYPE_H264)
		)
	{
		return TRUE;
	}
#endif

	//��˫�ٲ�Ԥ��
	if ( 0 == tConfinfo.GetSecBitRate())
	{
		return FALSE;
	}

	//����ʱ�����ж��ʽ���Ų�Ԥ��
	if ( g_cMcuVcApp.IsAdpResourceCompact() && tConfInfoEx.IsMainHasCapEx() )
	{
		return FALSE;
	}

	return TRUE;
}

/*====================================================================
������      : Is8KINeedDsAdaptH264SXGA
����        : 8ki�����Ƿ�ҪԤ��264sxga20����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/08/14              �ܾ���          ����
====================================================================*/
BOOL32 Is8KINeedDsAdaptH264SXGA( const TConfInfo& tConfinfo )
{
	if( (tConfinfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetResolution() > VIDEO_FORMAT_SXGA &&
			tConfinfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetUserDefFrameRate() >= 20) ||
			(tConfinfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetResolution() == VIDEO_FORMAT_SXGA &&
			tConfinfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetUserDefFrameRate() > 20)
		)
	{
		return TRUE;
	}

	return FALSE;
}

/*====================================================================
������      : IsConfHasBpCapOrBpExCap
����        : ��ǰ���������Ƿ���bp�������й�ѡbp����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/09/02              �ܾ���          ����
====================================================================*/
BOOL32 IsConfHasBpCapOrBpExCap( const TConfInfo& tConfinfo,const TConfInfoEx& tConfEx )
{
	TCapSupport tCapSupport = tConfinfo.GetCapSupport();
	if( tCapSupport.GetMainVideoType() == MEDIA_TYPE_H264 )
	{
		if( tCapSupport.GetMainStreamProfileAttrb() == emHpAttrb )
		{
			TVideoStreamCap atMStreamCapEx[MAX_CONF_CAP_EX_NUM];
			u8 byCapExNum = MAX_CONF_CAP_EX_NUM;
			tConfEx.GetMainStreamCapEx(atMStreamCapEx,byCapExNum);

			for( u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++ )
			{
				if( atMStreamCapEx[byIdx].GetMediaType() == MEDIA_TYPE_H264 
					&& atMStreamCapEx[byIdx].GetH264ProfileAttrb() == emBpAttrb)
				{
					return TRUE;					
				}
			}
		}
		else
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*====================================================================
������      : IsConfNeedReserveDSMainCap
����        : ��ǰ�����Ƿ���ҪԤ��˫������ʽ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/01/31              ��־��          ����
====================================================================*/
BOOL32 IsConfNeedReserveDSMainCap(const TConfInfo& tConfinfo, const TConfInfoEx &tConfInfoEx)
{
// 	if ( tConfinfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetMediaType() != MEDIA_TYPE_H264)
// 	{
// 		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[IsNeedReserveMainCap] conf's Main Media is not h264!\n");
// 		return FALSE;
// 	}

	//˫������ʽΪ��Ҳ����ҪԤ��
	if ( tConfinfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetMediaType() == MEDIA_TYPE_NULL ||
		tConfinfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetResolution() == VIDEO_FORMAT_INVALID	
		)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[IsNeedReserveMainCap] MediaType() or Resolution is invalid!\n");
		return FALSE;
	}


	if ( IsConfDoubleDual(tConfinfo)  && 
		!tConfInfoEx.IsDoubleHasCapEx()&&
		tConfinfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetMediaType() == MEDIA_TYPE_H264  &&
		tConfinfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetResolution() != VIDEO_FORMAT_XGA
		)
	{
		return TRUE;
	}

	//����ʽUXA����£������ѡ1080������ҪԤ������ʽ����ΪUXGA�ķֱ���С��1080
	if ( tConfinfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetMediaType() == MEDIA_TYPE_H264  &&
		tConfinfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetResolution() == VIDEO_FORMAT_UXGA &&
		tConfInfoEx.IsDSSupportCapEx(VIDEO_FORMAT_HD1080)
		)
	{
		return TRUE;
	}

	//8000E,8000H����֯��ѡ���ʶ����й�ѡ����ֱ��Ԥ��H264 XGA
#if	defined(_8KE_) || 	defined(_8KH_) || defined(_8KI_)
	if ( tConfInfoEx.IsDoubleHasCapEx() )
	{
		return TRUE;
	}
#endif

	if ( 0 == tConfinfo.GetSecBitRate())
	{
		return FALSE;
	}
	
	//����ʱ�����ж��ʽ
	if ( g_cMcuVcApp.IsAdpResourceCompact() && tConfInfoEx.IsDoubleHasCapEx() )
	{
		return FALSE;
	}
	
	return TRUE;
}

/*====================================================================
������      : GetAACChnlTypeByAudioTrackNum
����        : ͨ����������ȡAACLD/AACLC��Ӧͨ������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/10/10              chendaiwei    ����
====================================================================*/
u8 GetAACChnlTypeByAudioTrackNum( u8 byAudioTrackNum )
{
	u8 byChnlType = 0;
	switch ( byAudioTrackNum )
	{
	//������
	case 1:
		byChnlType = AAC_CHNL_TYPE_SINGLE;
		break;
	//˫����
	case 2:
		byChnlType = AAC_CHNL_TYPE_DOUBLE;
		break;
	default:
		byChnlType = byAudioTrackNum;
		LogPrint(LOG_LVL_ERROR,MID_MCU_CONF,"[GetAACChnlTypeByAudioTrackNum] AudioTrackNum.%d, not support,error!\n");
		break;
	}

	return byChnlType;
}

/*====================================================================
������      : IsV4R6B2VcsTemplate
����        : �ж��Ƿ���fix2����8000H��հ汾��VCSģ��
�㷨ʵ��    ��fix2, 8000h�Ľṹ��TConfStore����m_byVCAutoMode�������ļ�
			  ����ʱ��Ҫ���⴦���������ֶ�
����ȫ�ֱ�����
�������˵����File *hConfFile �ļ�
			  u16 wFileLenth �ļ�����
����ֵ˵��   �Ƿ���TRUE
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/11/13              chendaiwei    ����
====================================================================*/
BOOL32 IsV4R6B2VcsTemplate ( FILE * hConfFile, u16 wFileLenth)
{
	BOOL32 bRet = FALSE;

	if(hConfFile == NULL || wFileLenth == 0)
	{
		return bRet;
	}

	//���� ��ͨ + Ԥ�� + ��չ������Ԥ�����֣���ȡ������Ϣ ��ͨ+��չ [8/16/2012 chendaiwei]
	TConfStore tTempConfStore;
	TConfStore tUnPackConfstore;

	u16 wRemainFileLenth = wFileLenth - (strlen(CONFFILEHEAD) + sizeof(u16) + sizeof(u32));
	u16 wTempBufLen = min(wRemainFileLenth,sizeof(TConfStore));

	fseek(hConfFile,strlen(CONFFILEHEAD) + sizeof(u16) + sizeof(u32),SEEK_SET);
	fread((u8*)&tTempConfStore,wTempBufLen,1,hConfFile);
	u16 wPackConfDataLen = 0;
	TPackConfStore *ptTempPackConfstore = (TPackConfStore *)&tTempConfStore;
	UnPackConfStore(ptTempPackConfstore,tUnPackConfstore,wPackConfDataLen,FALSE);

	//����fix2��8000H�汾TConfStore�ṹ������m_byVCAutoMode������һ��U8�����˴����⴦��
	u8 byVcAutoModeExtraLen = 0;
	BOOL32 bIsFix2or8000HTemplate = FALSE;
	if( tTempConfStore.m_tConfInfo.GetConfSource() == VCS_CONF )
	{
		u16 wOldTemplateUnPackConfDataLen = wPackConfDataLen - 1;
		u16 wOldTEmplateRemainFileLenth = wRemainFileLenth - wOldTemplateUnPackConfDataLen;
		u16 wOldwPlanLen = 0;
		if( wOldTEmplateRemainFileLenth >= sizeof(u16))
		{
			fseek(hConfFile,strlen(CONFFILEHEAD) + sizeof(u16) + sizeof(u32) + wOldTemplateUnPackConfDataLen ,SEEK_SET);
			fread(&wOldwPlanLen,sizeof(u16),1,hConfFile);
			wOldwPlanLen = htons(wOldwPlanLen);
			wOldTEmplateRemainFileLenth -= sizeof(u16);
		}

		if( wOldwPlanLen == wOldTEmplateRemainFileLenth )
		{
			wPackConfDataLen--;
			bRet = TRUE;
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetConfDataFromFile] conf.%s is Fix2 or 8000H template without m_byVCAutoMode\n",tTempConfStore.m_tConfInfo.GetConfName());
		}
	}

	fseek(hConfFile,strlen(CONFFILEHEAD) + sizeof(u16) + sizeof(u32),SEEK_SET);

	return bRet;
}

/*====================================================================
������      : GetPlanDataLenByV4R6B2PlanData
����        : ͨ��fix2��Ԥ�����ݽ����������ر�ʾԤ������ʵ�ʳ���
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u8 *pbyV4R6B2PlanData fix2Ԥ�����ݳ���
			  u16 wV4R6B2Length Ԥ�����ݳ���
			 
����ֵ˵��   u16 ����Ԥ�����ݳ���
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/11/13              chendaiwei    ����
====================================================================*/
u16 GetPlanDataLenByV4R6B2PlanData( u8*pbyV4R6B2PlanData, u16 wV4R6B2Length)
{
	/*lint -save -esym(429, pbyV4R6B2PlanData)*/
	//����
	if( wV4R6B2Length < 3 || pbyV4R6B2PlanData == NULL)
	{
		return 0;
	}
	
	u16 bRetLen = 0;
	u8 *pbyBuf = pbyV4R6B2PlanData;
	
	//��һ���ֽ���Ԥ������
	u8 byPlanNum = *pbyBuf;
	pbyBuf++;
	bRetLen++;
	
	//Ԥ������0 ����
	if( 0 == byPlanNum )
	{
		return 0;
	}
	
	u8 byTemp = 0;
	for( u8 idx = 0;idx < byPlanNum;idx++ )
	{
		//����ֻ֧�ֵ�8������ǽԤ��
		if( idx >= VCS_MAXNUM_PLAN )
		{
			byPlanNum = VCS_MAXNUM_PLAN;
			break;
		}
		//Ԥ�����ֳ���u8��������\0
		byTemp = *pbyBuf;
		pbyBuf++;
		bRetLen++;

		//Ԥ����������[11/14/2012 chendaiwei]
		pbyBuf += byTemp;
		bRetLen += byTemp;
		
		//�ն˱�������u8
		byTemp = *pbyBuf;
		pbyBuf++;
		bRetLen++;

		pbyBuf += sizeof( TMtVCSPlanAliasV4R6B2 ) * byTemp;
		
		bRetLen += sizeof( TMtVCSPlanAlias ) * byTemp;
		
		if( ( wV4R6B2Length + pbyV4R6B2PlanData ) <= pbyBuf )
		{
			byPlanNum = idx + 1;
			break;
		}
	}

	return bRetLen;
	/*lint -restore*/
}

/*====================================================================
������      : TransferV4R6B2planDataToV4R7
����        : v4r6b2��8000H��Ԥ������ת����v4r7Ԥ������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u8 *pbyV4R6B2PlanData fix2Ԥ������
			  u16 wV4R6B2Length fix2Ԥ�����ݳ���
			  u8 *pbyV4R7PlanData v4r7Ԥ�������׵�ַ
			  u16 wV4R7DataLen v4r7Ԥ�����ݳ���
����ֵ˵��   �Ƿ���TRUE
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/11/13              chendaiwei    ����
====================================================================*/
BOOL32 TransferV4R6B2planDataToV4R7(u8*pbyV4R6B2PlanData,u16 wV4R6B2DataLen,u8*pbyV4R7PlanData,u16 wV4R7DataLen )
{
	/*lint -save -esym(429, pbyV4R6B2PlanData)*/
	/*lint -save -esym(429, pbyV4R7PlanData)*/

	if( wV4R6B2DataLen == 0 || wV4R7DataLen == 0 || pbyV4R7PlanData == NULL || pbyV4R6B2PlanData == NULL)
	{
		return FALSE;
	}
	
	u16 bRetLen = 0;
	u8 *pbyBuf = pbyV4R6B2PlanData;
	
	//��һ���ֽ���Ԥ������
	u8 byPlanNum = *pbyBuf;
	*pbyV4R7PlanData = *pbyBuf;
	pbyBuf++;
	pbyV4R7PlanData++;

	//Ԥ������0 ����
	if( 0 == byPlanNum )
	{
		return FALSE;
	}
	
	u8 byTemp = 0;
	for( u8 idx = 0;idx < byPlanNum;idx++ )
	{
		//����ֻ֧�ֵ�8������ǽԤ��
		if( idx >= VCS_MAXNUM_PLAN )
		{
			byPlanNum = VCS_MAXNUM_PLAN;
			break;
		}
		//Ԥ�����ֳ���u8��������\0
		byTemp = *pbyBuf;
		*pbyV4R7PlanData = byTemp;
		pbyBuf++;
		pbyV4R7PlanData++;
		
		//Ԥ����������[11/14/2012 chendaiwei]
		memcpy(pbyV4R7PlanData,pbyBuf,byTemp);
		pbyBuf += byTemp;
		pbyV4R7PlanData += byTemp;
		
		//�ն˱�������u8
		byTemp = *pbyBuf;
		*pbyV4R7PlanData = byTemp;
		pbyBuf++;
		pbyV4R7PlanData++;

		u8 byV4R6AlaisHeadLen = 33;
		u8 byV4R6AliasTrailLen = sizeof(TTransportAddr)+sizeof(u8);
		for( u8 byIdx = 0; byIdx < byTemp; byIdx++)
		{
			memcpy(pbyV4R7PlanData,pbyBuf,byV4R6AlaisHeadLen);
			pbyV4R7PlanData += byV4R6AlaisHeadLen;
			pbyBuf += byV4R6AlaisHeadLen;

			memset(pbyV4R7PlanData,0,VCS_MAXLEN_ALIAS-32);
			pbyV4R7PlanData += (VCS_MAXLEN_ALIAS-32);

			memcpy(pbyV4R7PlanData,pbyBuf,byV4R6AliasTrailLen);
			pbyBuf+= byV4R6AliasTrailLen;
			pbyV4R7PlanData+= byV4R6AliasTrailLen;
		}
		
		if( ( wV4R6B2DataLen + pbyV4R6B2PlanData ) <= pbyBuf )
		{
			break;
		}
	}
	
	return TRUE;
	/*lint -restore*/
	/*lint -restore*/
}

//END OF FILE
