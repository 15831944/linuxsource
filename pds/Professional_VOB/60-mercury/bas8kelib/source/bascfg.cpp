/*****************************************************************************
   ģ����      : ���渴��������
   �ļ���      : VMPCfg.cpp
   ����ʱ��    : 2003�� 12�� 4��
   ʵ�ֹ���    : 
   ����        : zhangsh
   �汾        : 
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
******************************************************************************/
#include "bascfg.h"



void CCPParam::GetDefaultParam(u8 byEncType, TVidEncParam& tEncParam)
{
    GetDefaultEncParam(byEncType, tEncParam);
}


void CCPParam::Clear()
{
    memset( &m_tStatus, 0, sizeof(m_tStatus) );
    m_tStatus.m_byVMPStyle = 0xFF;  //invalid
}


CCPParam::CCPParam()
{
    Clear();    
}

CCPParam::~CCPParam()
{
}
