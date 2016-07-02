/*****************************************************************************
   ģ����      : eqpbase
   �ļ���      : eqpbase.h
   ����ļ�    : eqpbase.cpp
   �ļ�ʵ�ֹ���: �ṩ������Ĺ�ͨ���ܽӿ�
   ����        : ����
   �汾        : V0.9  Copyright(C) 2001-2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2011/06/01  4.6         ����	       ����
******************************************************************************/

#ifndef MCU_PERIEQP_BASE_H
#define MCU_PERIEQP_BASE_H
 
#include "kdvtype.h"

struct TMt;

extern BOOL32 IsRecEqp(const TMt&);


// class CSwitchBase
// {
// public:
// 	virtual BOOL32 StartSwitchTo( const TSwitchInfo& );
// 	virtual BOOL32 StartSwitchFrom( const TSwitchInfo& );
// 	virtual BOOL32 ChangeSwitch( const TSwitchInfo& );
// 	virtual BOOL32 StopSwitch( const BOOL32 );
// };
// 
// typedef TMt CKdvObj;
// 
// class CNetObj : CKdvObj
// {
// public:
// 	/** 
// 	* @ ��    ��@ : ����ͨ���Ż�ȡǰ���ַ(���յ�ַ)
// 	*/ 
// 	virtual TTransportAddr GetFwdAddr( const u16 wChnnlIdx ) const = 0;
// 
// 	/** 
// 	* @ ��    ��@ : ����ͨ���Ż�ȡ�����ַ(���͵�ַ), һ������
// 	*/ 
// 	virtual TTransportAddr GetRearAddr( const u16 wChnnlIdx ) const = 0;
// 
// 	/** 
// 	* @ ��    ��@ : ��ȡǰ������ͨ������
// 	*/ 
// 	virtual u16 GetChnnlNum ( const BOOL32 bFwd = TRUE ) const { return 0; }
// 	
// };
// 
// template< u16 wFwdChnnlNum, u16 wRearChnnlNum >
// class CMediaObj : CNetObj
// {
// public:
// 	/** 
// 	* @ ��    ��@ : ����ͨ���Ż�ȡǰ���߼�ͨ��(����ͨ��)
// 	*/ 
// 	virtual TLogicalChannel GetFwdAddr( const u16 wChnnlIdx ) const = 0;
// 
// 	/** 
// 	* @ ��    ��@ : ����ͨ���Ż�ȡ�����߼�ͨ��(����ͨ��), һ������
// 	*/ 
// 	virtual TLogicalChannel GetRearAddr( const u16 wChnnlIdx ) const = 0;	
// 
// private:
// 
// 	u16				m_wFwdChnnNum;							// ǰ��ͨ������
// 	u16				m_wRearChnnlNum;						// ����ͨ������
// };

#endif