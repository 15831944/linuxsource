/*****************************************************************************
ģ����      : vmp8ki
�ļ���      : vmp8ki.h
����ʱ��    : 2012�� 05�� 02��
ʵ�ֹ���    : 
����        : ������
�汾        : 
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
2012/05/02  1.0         ������        ����
******************************************************************************/
#ifndef _VMP8KI_H
#define _VMP8KI_H

#include "kdvtype.h"
#include "kdvlog.h"
#include "loguserdef.h"


#define LOGFILE_VMP8KI (s8*)"/usr/etc/config/conf/kdvlog_vmp8ki.ini"


void MmpFileLog( const s8 * pchLogFile, s8 * pszFmt, ... );

#endif //!_VMP8KI_H


