/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : imtconfig.h
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
#ifndef IMTCONFIG_H
#define IMTCONFIG_H

#include "mcuagtstruct.h"
#include "mediabrdconfig.h"

class CImtConfig:public CMediaBrdConfig
{
	friend class CBoardAgent;

public:
	CImtConfig();
	~CImtConfig();

};

#endif    /* IMTCONFIG_H */
