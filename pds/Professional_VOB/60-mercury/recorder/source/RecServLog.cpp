/*****************************************************************************
   ģ����      : Recorder
   �ļ���      : RecServLog.cpp
   ����ļ�    : RecApp.cpp, RecServInst.cpp, State.cpp, StateBase.cpp��
   �ļ�ʵ�ֹ���: Recorder Serverʵ��
   ����        : 
   �汾        : V1.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2010/08/26  1.0         �� ��        ����
******************************************************************************/

#include "RecServ_Def.h"


static BOOL32 s_bRecServLog = TRUE;							// RecServInst��log���أ�Ĭ��Ϊ��
static u8     s_byLogLevel  = RECSERV_LEVEL_LOG_WARNING;	// ���ȼ�log��ֵ��Ĭ�������warning������


/*=============================================================================
  �� �� ���� recservhelp
  ��    �ܣ� RecServInst��ӡlog�İ�������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� ��
  �� �� ֵ�� void 
=============================================================================*/
API void recservhelp()
{
	OspPrintf(TRUE, FALSE, "==========================================================================\n");
	OspPrintf(TRUE, FALSE, "==               Helps of Recorcer Server                               ==\n");
	OspPrintf(TRUE, FALSE, "precservlog()                 : Print log from recserver\n");
	OspPrintf(TRUE, FALSE, "nprecservlog()                : Don't print log from recserver\n");
	OspPrintf(TRUE, FALSE, "setloglevel(u8 byLevel)       : Set the level of levellog��loglevels were defined below\n");
	OspPrintf(TRUE, FALSE, "     loglevel < 4 : only output error message\n");
	OspPrintf(TRUE, FALSE, "     loglevel < 6 : output error and warning message\n");
	OspPrintf(TRUE, FALSE, "     loglevel < 8 : output error, warning and common message\n");
	OspPrintf(TRUE, FALSE, "     loglevel >= 8: output all messages\n");
	OspPrintf(TRUE, FALSE, "recservstatus(u8 byInstId)    : Show the status of RecServInst\n");
	OspPrintf(TRUE, FALSE, "     byInstId = 0 : Show all recservs' status");
	OspPrintf(TRUE, FALSE, "     byInstId < %u: Show recserv[byInstId]'s status", MAXNUM_RECORDER_CHNNL);
	OspPrintf(TRUE, FALSE, "     byInstId > %u: Input Error", MAXNUM_RECORDER_CHNNL);
	OspPrintf(TRUE, FALSE, "==========================================================================\n");
}


/*=============================================================================
  �� �� ���� precservlog
  ��    �ܣ� ��recserv log�ĺ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
API void precservlog()
{
	s_bRecServLog = TRUE;
	OspPrintf(TRUE, FALSE, "Recserv log opend\n");
}


/*=============================================================================
  �� �� ���� precservlog
  ��    �ܣ� �ر�recserv log�ĺ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void 
=============================================================================*/
API void nprecservlog()
{
	s_bRecServLog = FALSE;
	OspPrintf(TRUE, FALSE, "Recserv log closed\n");
}


/*=============================================================================
  �� �� ���� setloglevel
  ��    �ܣ� �������ȼ�log�����ȼ�ֵ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
			 byLevel : ��Ҫ���óɵ����ȼ�
  �� �� ֵ�� void 
=============================================================================*/
API void setloglevel(const u8 byLevel)
{
	if (byLevel <= RECSERV_LEVEL_LOG_BELOW)
	{
		// ������������ȼ��ߣ������ȼ����óɲ�����ֵ
		s_byLogLevel = byLevel;
	}else
	{
		// ������������ȼ����ͣ������ȼ����ó�������ȼ�
		s_byLogLevel = RECSERV_LEVEL_LOG_BELOW;
	}

	OspPrintf(TRUE, FALSE, "Log level has been set to %d", s_byLogLevel);
}

/*=============================================================================
  �� �� ���� RecServLog
  ��    �ܣ� RecServLog��ӡ���ȼ�log�ĺ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
			 szFormat : ����ַ���
			 ��������ձ�׼c��printf����
  �� �� ֵ�� void 
=============================================================================*/
void RecServLog( s8* pszFmt, ... )
{
    s8 achPrintBuf[KDV_MAX_PATH];
    s32  nBufLen = 0;
    va_list argptr;
    if( s_bRecServLog )
    {		  
        nBufLen = sprintf( achPrintBuf, "[RecServ]: " ); 
        va_start( argptr, pszFmt );
		vsnprintf(achPrintBuf + nBufLen, KDV_MAX_PATH - nBufLen - 1, pszFmt, argptr );
        //nBufLen += vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
        va_end(argptr); 
        OspPrintf( TRUE, FALSE, achPrintBuf ); 
    }
    return;
}


/*=============================================================================
  �� �� ���� RecServLevelLog
  ��    �ܣ� RecServLevelLog��ӡ���ȼ�log�ĺ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
			 byLevel  : log�����ȼ��������log���ȼ�ֵ����
			 szFormat : ����ַ���
			 ��������ձ�׼c��printf����
  �� �� ֵ�� void 
=============================================================================*/
void RecServLevelLog( const u8& byLevel, s8* pszFmt, ... )
{
	// ���ȼ��ȵ�ǰ���ȼ��߲����
	if (byLevel <= s_byLogLevel)
    {
		// log��������Ǵ򿪵�
        if( s_bRecServLog )
        {	
			s8 achPrintBuf[KDV_MAX_PATH];
			s32  nBufLen = 0;

			// ����log��Ϣͷ
			switch (byLevel)
			{
			case RECSERV_LEVEL_LOG_ERROR:
				nBufLen = sprintf( achPrintBuf, "[Error]" );
				break;
				
			case RECSERV_LEVEL_LOG_WARNING:
				nBufLen = sprintf( achPrintBuf, "[Warning]" );
				break;

			default:
				nBufLen = sprintf( achPrintBuf, "[RecServ]" );
				break;
			}

			// ��ȡ����ַ���
	        va_list argptr;
            va_start( argptr, pszFmt );
			vsnprintf(achPrintBuf + nBufLen, KDV_MAX_PATH - nBufLen - 1, pszFmt, argptr );
            //nBufLen += vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
            va_end(argptr); 

			// ���log
            OspPrintf( TRUE, FALSE, achPrintBuf ); 
        }
    }
}

API void recservstatus(u8 byInstId = 0)
{
	if (byInstId == 0)
	{
		OspPost(MAKEIID(AID_REC_SERVER, CInstance::DAEMON), EV_RECSERV_SHOW_STATUS);
	}else
	{
		if (byInstId > MAXNUM_RECORDER_CHNNL)
		{
			OspPrintf(TRUE, FALSE, "The parameter Input is invalid, it must be in [0, %u]", MAXNUM_RECORDER_CHNNL);
			return;
		}

		OspPost(MAKEIID(AID_REC_SERVER, byInstId), EV_RECSERV_SHOW_STATUS);
	}	
}