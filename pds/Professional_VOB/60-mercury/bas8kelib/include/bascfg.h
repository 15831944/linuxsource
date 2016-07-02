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

