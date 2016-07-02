/*****************************************************************************
   ģ����      : ������
   �ļ���      : audiomixercpp
   ����ʱ��    : 2003�� 12�� 9��
   ʵ�ֹ���    : 
   ����        : ������
   �汾        : 
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   03/12/09    3.0         zmy           ����
   12/0217     4.7         ��־��        ����
******************************************************************************/
#include "audbasinst.h"
#include "audmixinst.h"
#include "mcuver.h"
#include "evmixer.h"


#ifndef _COMMONCLASS_H_
#include "commonclass.h"
#endif
/*=============================================================================
�� �� ���� mixerhelp
��    �ܣ� ��������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
12/02/17    4.7		    ��־��                 ����
=============================================================================*/
API void mixerhelp()
{
    ::OspPrintf(TRUE, FALSE, "\n\t\t KDV Mixer \n");
    ::OspPrintf(TRUE, FALSE, "Audio Mixer Compile Time : %s, %s\n", __TIME__, __DATE__);
    ::OspPrintf(TRUE, FALSE, "Audio Mixer Verion       : %s\n\n", VER_MIXER);
    ::OspPrintf(TRUE, FALSE, "mixershow                  : Show Mixer Status \n");
}
/*=============================================================================
�� �� ���� mixerver
��    �ܣ� �鿴��ǰ�汾����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
12/02/17    4.7		    ��־��                 ����
=============================================================================*/
API void mixerver()
{
	OspPrintf( TRUE, FALSE, "Mixer: %s  compile time: %s    %s\n", VER_MIXER, __DATE__, __TIME__);
}
/*=============================================================================
�� �� ���� mixershow
��    �ܣ� ��ʾ������״̬����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
12/02/17    4.7		    ��־��                 ����
=============================================================================*/
API void mixershow()
{
    if (::OspPost(MAKEIID(AID_MIXER, CInstance::DAEMON), EV_MIXER_SHOWGROUP) != OSP_OK)
	{
		printf("[mixershow]OspPost To DAEMON EV_MIXER_SHOWGROUP  Failed!\n");
		return;
	}
}
/*=============================================================================
�� �� ���� MixAPIEnableInLinux
��    �ܣ� ע����Խӿ�
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� void
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/8/15  4.0			������                  ����
=============================================================================*/
/*lint -save -e611*/
#ifdef _LINUX_  
static void MixAPIEnableInLinux(void)
{
    OspRegCommand("mixerhelp",        (void*)mixerhelp,         "mixer help");
    OspRegCommand("mixershow",        (void*)mixershow,         "mixer status");
    OspRegCommand("mixerver",       (void*)mixerver,          "mixer ver");
}
#endif
/*lint -restore*/
/*=============================================================================
�� �� ���� mixInit
��    �ܣ� ����������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� T8keAudioMixerCfg * ptMixerCfg
�� �� ֵ�� BOOL 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
12/02/17    4.7		    ��־��                 ����
=============================================================================*/
/*lint -save -e528 -e714 -e765*/
BOOL mixInit(T8keAudioMixerCfg * ptMixerCfg)
{   
#ifdef _LINUX_  
    MixAPIEnableInLinux();
#endif
    
    if (NULL == ptMixerCfg || ( 0 == ptMixerCfg->GetConnectIp() ) )
    {
		printf("[mixInit]NULL == ptMixerCfg || ( 0 == ptMixerCfg->GetConnectIp() ),So Return!\n");
		return FALSE;
    }

    if (g_cMixerApp.CreateApp( "mix", AID_MIXER, APPPRI_MIXER, 512, 200<<10 ) != OSP_OK)// ����MixerӦ��
    {
		printf("[mixInit]CreateApp mix  Failed!\n");
		return FALSE;
    }



	

    #undef OSPEVENT
    #define OSPEVENT(x, y) OspAddEventDesc(#x, y)

    /*lint -save -e537 -e760*/
#ifdef _EVENT_MCU_MIXER_H_
    #undef _EVENT_MCU_MIXER_H_
    #include "evmixer.h"
    #define _EVENT_MCU_MIXER_H_
#else
    #include "evmixer.h"
    #undef _EVENT_MCU_MIXER_H_
#endif

#ifdef _EV_MCUEQP_H_
    #undef _EV_MCUEQP_H_
    #include "evmcueqp.h"
    #define _EV_MCUEQP_H_
#else
    #include "evmcueqp.h"
    #undef  _EV_MCUEQP_H_
    #undef _EV_MCUEQP_H_
#endif
	/*lint -restore*/

    if (::OspPost(MAKEIID(AID_MIXER, CInstance::DAEMON), OSP_POWERON, ptMixerCfg, sizeof(T8keAudioMixerCfg)) == OSP_ERROR)
    {
		printf("[startmixeqp]OspPost To DAEMON OSP_POWERON  Failed!\n");
		return FALSE;
    }

#ifdef _8KI_

	if( g_c8KIAudBasApp.CreateApp( "8kiaudbas",AID_MPU2,APPPRI_MPU,512,1<<20) != OSP_OK )
	{
		printf("[mixInit]CreateApp 8kiaudbas Failed!\n");
		return FALSE;
	}

	if (::OspPost(MAKEIID(AID_MPU2, CInstance::DAEMON), EV_BAS_INI, (T8keEqpCfg*)ptMixerCfg, sizeof(T8keEqpCfg)) == OSP_ERROR)
    {
		printf("[startmixeqp]OspPost To DAEMON EV_BAS_INIT  Failed!\n");
		return FALSE;
    }
#endif

    return TRUE;
}
