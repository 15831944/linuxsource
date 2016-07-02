/*****************************************************************************
   ģ����      : KDV draw bmp
   �ļ���      : drawinterface.cpp
   ����ļ�    : drawinterface.h
   �ļ�ʵ�ֹ���: ���׻�ͼ�ӿ���
   ����        : 
   �汾        : V0.9  Copyright(C) 2001-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2006/03/29  4.0         john     ����
******************************************************************************/
#include "drawinterface.h"
#include "drawbmp.h"

/*=============================================================================
  �� �� ���� CDrawInterface
  ��    �ܣ� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
=============================================================================*/
CDrawInterface::CDrawInterface()
{
    m_pDrawGdi = NULL;
    m_pDrawGdi = new CKdvGDI;
    m_pDrawGdi->InitGDI();
}

/*=============================================================================
  �� �� ���� ~CDrawInterface
  ��    �ܣ� ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� 
=============================================================================*/
CDrawInterface::~CDrawInterface()
{
    if( NULL == m_pDrawGdi )
    {
        return;
    }
    m_pDrawGdi->QuitGDI();
    delete m_pDrawGdi;
	m_pDrawGdi = NULL;
}

/*=============================================================================
  �� �� ���� DrawBmp
  ��    �ܣ� ����Bmpͼ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� OUT TPic& tPic ͼƬ
             IN TDrawParam& tDrawParam ��ͼ����
  �� �� ֵ�� u16 
=============================================================================*/
BOOL32 CDrawInterface::DrawBmp( OUT TPic& tPic, IN TDrawParam& tDrawParam )
{
	return m_pDrawGdi->DrawBmp( tPic, tDrawParam );
}