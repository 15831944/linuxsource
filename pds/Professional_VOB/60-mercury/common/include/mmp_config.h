/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : mmpconfig.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ������ú�������
   ����        : jianghy
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/08/25  1.0         jianghy       ����
   2004/11/11  3.5         �� ��         �½ӿڵ��޸�
******************************************************************************/
#ifndef MMPCONFIG_H
#define MMPCONFIG_H

#include "mcuagtstruct.h"
#include "mediabrdconfig.h"

class CMmpConfig : public CMediaBrdConfig
{
	friend class CBoardAgent;

public:
	CMmpConfig();
	virtual ~CMmpConfig();
};

#endif    /* MMPCONFIG_H */
