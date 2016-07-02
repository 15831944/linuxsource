#include "apu2inst.h"
#include "apu2basinst.h"


CApu2MixApp g_cApu2MixApp;
CApu2BasApp g_cApu2BasApp;


u32      g_dwSaveNum = 0;
u16      g_wChnIdx   = 0;


/*====================================================================
������      : apu2help
����        ��ҵ�����
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
API void apu2help(void)
{
	StaticLog("\n------------APU2 Print As Follows:------------\n");
	StaticLog("pmixmsg		: OPEN MIX-PRINT\n");
	StaticLog("npmixmsg	: STOP MIX-PRINT\n");
	StaticLog("pbasmsg		: OPEN BAS-PRINT\n");
	StaticLog("npbasmsg	: STOP BAS-PRINT\n");
	StaticLog("apu2ver		: APU2 VERSION\n");
	StaticLog("showmix		: MIX BASEINFO\n");
	StaticLog("showmixchnl	: MIX CHNLINFO\n");
	StaticLog("showstate	: MIX CHNLSTATE\n");
	StaticLog("showbas		: BAS BASEINFO\n");
	StaticLog("showbaschnl	: BAS CHNLINFO\n");
	StaticLog("----------------------------------------------\n");
}


/*====================================================================
������      : pmixmsg
����        ���ſ�����ҵ���ӡ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
API void pmixmsg(void)
{
	logenablemod(MID_MCU_MIXER);
}

/*====================================================================
������      : npmixmsg
����        ���رջ���ҵ���ӡ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
API void npmixmsg(void)
{
	logdisablemod(MID_MCU_MIXER);
}

/*====================================================================
������      ��pbasmsg
����        ���ſ������ӡ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/04/02	2.0			������		  ����
====================================================================*/
API void pbasmsg(void)
{
	logenablemod(MID_MCU_BAS);
}

/*====================================================================
������      ��npbasmsg
����        �����������ӡ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/04/02	2.0			������		  ����
====================================================================*/
API void npbasmsg(void)
{
	logdisablemod(MID_MCU_BAS);
}

/*====================================================================
������      : showmix
����        ����ʾ���л�����̬��ӡ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u8 byInsID  Instanceʵ����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
API void showmix(u8 byInsID)
{
    ::OspPost(MAKEIID(AID_MIXER, CInstance::DAEMON), EV_MIXER_SHOWMIX, &byInsID, sizeof(byInsID));
}

/*====================================================================
������      : showchinfo
����        ����ʾͨ����Ϣ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u8 byInsID  Instanceʵ����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/02  2.0         ������          ����
====================================================================*/
API void showmixchnl(u8 byInsID)
{
	::OspPost(MAKEIID(AID_MIXER, CInstance::DAEMON), EV_MIXER_SHOWCHINFO, &byInsID, sizeof(byInsID));
}

/*====================================================================
������      : showstatus
����        ����ʾ�����������Ϣ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u8 byInsID  Instanceʵ����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/03/02  2.0         ������          ����
====================================================================*/
API void showstate(u8 byInsID)
{
	::OspPost(MAKEIID(AID_MIXER, CInstance::DAEMON), EV_MIXER_SHOWSTATE, &byInsID, sizeof(byInsID));
}

/*====================================================================
������      ��showbas
����        ����ʾ��������Ϣ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u8 byInsID  Instanceʵ����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/04/02  2.0         ������        ����
====================================================================*/
API void showbas(u8 byInsID)
{
	::OspPost(MAKEIID(AID_BAS, CInstance::DAEMON), EV_BAS_SHOWBAS, &byInsID, sizeof(byInsID));
}

/*====================================================================
������      ��showbaschnl
����        ����ʾ������״̬
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u8 byInsID  Instanceʵ����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2013/04/02  2.0         ������        ����
====================================================================*/
API void showbaschnl(u8 byInsID)
{
	::OspPost(MAKEIID(AID_BAS, CInstance::DAEMON), EV_BAS_SHOWCHINFO, &byInsID, sizeof(byInsID));
}

/*====================================================================
������      : apu2ver
����        ���汾��ӡ
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
API void apu2ver(void)
{
	s8 gs_VersionBuf[128] = {0};
    {
        strcpy(gs_VersionBuf, KDV_MCU_PREFIX);
		
        s8 achMon[16] = {0};
        u32 byDay = 0;
        u32 byMonth = 0;
        u32 wYear = 0;
        s8 achFullDate[24] = {0};
		
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
            sprintf(gs_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
        }
    }
    StaticLog("Apu2 Version: %s\n", gs_VersionBuf );    
	StaticLog("Apu2: %s  compile time: %s    %s\n", VER_APU2, __DATE__, __TIME__);
}


/*====================================================================
������      : savefile
����        ����������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����u32 dwChnIdx ����������ͨ������
			  u32 dwNum    �������
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
API void savefile(u16 wChnIdx, u32 dwNum)
{
    g_wChnIdx   = wChnIdx;
	g_dwSaveNum = dwNum;
}


/*====================================================================
������      : eqpsettest
����        �������������Ա�־
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����int nlevel ���Ա�־
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
#ifdef _LINUX_ 
API void eqpsettest(int nlevel)
{
	STATUS wRet = 0;
	if (nlevel == 0)
	{
		wRet = BrdClearE2promTestFlag();
		if ( wRet != 0 )
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER,  "[eqpsettest] BrdClearE2promTestFlag failed!\n" );
		}
		else
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[eqpsettest] BrdClearE2promTestFlag sucessful!\n" );
		}
	}
	else
	{
		wRet = BrdSetE2promTestFlag();
		if ( wRet != 0 )
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[eqpsettest] BrdSetE2promTestFlag failed!\n" );
		}
		else
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[eqpsettest] BrdSetE2promTestFlag sucessful!\n" );
		}
	}
}
#endif
/*====================================================================
������      : Apu2APIEnableInLinux
����        ��apu2ҵ��ע����Խӿ�
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
#ifdef _LINUX_  
static void Apu2APIEnableInLinux(void)
{
	/*lint -save -e611*/
    OspRegCommand("apu2help",        (void*)apu2help,         "apu2help");
    OspRegCommand("pmixmsg",         (void*)pmixmsg,           "pmixmsg");
	OspRegCommand("npmixmsg",        (void*)npmixmsg,         "npmixmsg");
	OspRegCommand("pbasmsg",         (void*)pbasmsg,           "pbasmsg");
	OspRegCommand("npbasmsg",        (void*)npbasmsg,         "npbasmsg");
    OspRegCommand("showmix",         (void*)showmix,           "showmix");
	OspRegCommand("showmixchnl",	 (void*)showmixchnl,   "showmixchnl");
	OspRegCommand("showstate",		 (void*)showstate,		 "showstate");
	OspRegCommand("showbas",		 (void*)showbas,		   "showbas");
	OspRegCommand("showbaschnl",	 (void*)showbaschnl,   "showbaschnl");
    OspRegCommand("apu2ver",         (void*)apu2ver,           "apu2ver");
    OspRegCommand("savefile",        (void*)savefile,         "savefile");
    OspRegCommand("eqpsettest",      (void*)eqpsettest,     "eqpsettest");
	/*lint -restore*/
}
#endif
/*====================================================================
������      : apu2Init
����        ��lib������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����TApu2EqpCfg *ptApu2Cfg ����
����ֵ˵��  ��TRUE OR FALSE
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
2012/02/14  1.0         �ܼ���          ����
====================================================================*/
/*lint -save -e765 -e714*/
BOOL32 apu2Init(TApu2EqpCfg *ptApu2Cfg)
{
	/*lint -save -e537*/
	#undef OSPEVENT
	#define OSPEVENT(x, y) OspAddEventDesc(#x, y)
	#ifdef _EVENT_APU2_H
		#undef _EVENT_APU2_H
		#include "evapu2.h"
		#define _EVENT_APU2_H
	#else
		#include "evapu2.h"
		#undef _EVENT_APU2_H
	#endif
	
	#ifdef _EV_MCUEQP_H_
		#undef _EV_MCUEQP_H_
		#include "evmcueqp.h"
		#define _EV_MCUEQP_H_
	#else
		#include "evmcueqp.h"
		#undef  _EV_MCUEQP_H_
	#endif

	#ifdef _EQP_BASE_EVENT_H
		#undef _EQP_BASE_EVENT_H
		#include "eveqpbase.h"
		#define _EQP_BASE_EVENT_H
	#else
		#include "eveqpbase.h"
		#undef  _EQP_BASE_EVENT_H
	#endif
	/*lint -restore*/

#ifdef _LINUX_ 
	//apu2ҵ��ע����Խӿ�
	Apu2APIEnableInLinux();
#endif
	PTR_NULL_RETURN(ptApu2Cfg, FALSE);
	
	if ( ptApu2Cfg->m_bIsProductTest)
	{
		LogPrint(LOG_LVL_KEYSTATUS,  MID_MCU_MIXER, "[apu2Init]Begin ProductTest!!!!\n");
		::OspSetPrompt("APU2AUTOTEST");
	}
	else
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[eapuInit]Begin NormalTask!!!!\n");
		::OspSetPrompt("APU2");
	}

	//����Mixer�߳�
	g_cApu2MixApp.CreateApp( "APU2MIX", AID_MIXER, APPPRI_MIXER, 512, 1<<20 );
	//����Bas�߳�
	g_cApu2BasApp.CreateApp( "APU2BAS", AID_BAS, APPPRI_BAS, 512, 1<<20);

#ifdef _8KI_
	if (!ptApu2Cfg->m_bIsProductTest) 
	{
		::OspPost(MAKEIID(AID_MIXER, CInstance::DAEMON), OSP_POWERON);
		return TRUE;
	}
#endif

	::OspPost(MAKEIID(AID_MIXER, CInstance::DAEMON), EV_MIXER_INIT, ptApu2Cfg, sizeof(TApu2EqpCfg));
	::OspPost(MAKEIID(AID_BAS, CInstance::DAEMON), EV_BAS_INIT, ptApu2Cfg, sizeof(TApu2EqpCfg));
	
    return TRUE;
}
