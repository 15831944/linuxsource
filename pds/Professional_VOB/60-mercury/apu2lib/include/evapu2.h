/*****************************************************************************
ģ����      : apu2lib
�ļ���      : evapu2.h
����ʱ��    : 2012�� 02�� 14��
ʵ�ֹ���    : apu2�ڲ��¼�����
����        : �ܼ���
�汾        : 1.0
******************************************************************************/

#ifndef _EVENT_APU2_H
#define _EVENT_APU2_H
#include "eventid.h"
#include "evmcueqp.h"
#include "eapuautotest.h"

/***************************�������ڲ���Ϣ**************************/

//��������ʼ��,��Ϣ��: TApu2EqpCfg
OSPEVENT(EV_MIXER_INIT,                 EV_MIXER_BGN + 1 ); 
//��ʾ����������
OSPEVENT(EV_MIXER_SHOWMIX,              EV_MIXER_BGN + 2 );
//��ʾͨ����Ϣ
OSPEVENT(EV_MIXER_SHOWCHINFO,			EV_MIXER_BGN + 3 );
//��ʾ�����������Ϣ
OSPEVENT(EV_MIXER_SHOWSTATE,			EV_MIXER_BGN + 4 );

//��������ʼ��,��Ϣ��: TApu2EqpCfg
OSPEVENT(EV_BAS_INIT,					EV_BAS_BGN + 1 );
//��ʾ����������
OSPEVENT(EV_BAS_SHOWBAS,				EV_BAS_BGN + 2 );
//��ʾͨ����Ϣ
OSPEVENT(EV_BAS_SHOWCHINFO,				EV_BAS_BGN + 3 );

/**************************���������ڲ���Ϣ*************************/
//�������Գ�ʼ��
OSPEVENT(EV_TEST_INIT,                  EV_MIXER_BGN + 10 ); 
//�������Ե��(����6���ƣ�Ԥ��6����Ϣ)
OSPEVENT(TIMER_TEST_LED,                EV_MIXER_BGN + 11 ); 

/*BAS����*/
#define  MAX_AUDIO_FRAME_SIZE		(u32)8 * 1024			//���մ�С
#define  MAXNUM_BASCHN				(u8)1					//BAS�����һ��ͨ��
#define  MAXNUM_NMODECHN			(u8)4					//Nģʽͨ�����֧��
#define  MAXNUM_MIXERCHN			(u8)1					//Apu2�����һ��ͨ��
#define  MAXVALUE_APU2_VOLUME		(u8)31					//Apu2�������������ֵ
#define  MAX_AUDIO_FRAME_SIZE		(u32)8 * 1024			//���մ�С
#define  APU2_TEST_LEDNUM			(u8)6					//Apu2�������Ե����
#define  TEST_LED_TIMER_INTERVAL	(u32)2 * 1000			//��Ƶȴ����

/*������*/
#define ERR_BAS_NOERROR				( ERR_BAS_BGN + 0 )		//�޴���						
#define ERR_BAS_CHNOTREAD           ( ERR_BAS_BGN + 1 )		//��ǰͨ�����ھ���״̬
#define ERR_BAS_CHRUNING            ( ERR_BAS_BGN + 2 )		//��ǰͨ���������� 
#define ERR_BAS_CHREADY             ( ERR_BAS_BGN + 3 )		//��ǰͨ�����ڴ��� 
#define ERR_BAS_CHIDLE              ( ERR_BAS_BGN + 4 )		//��ǰͨ��Ϊ��     
#define ERR_BAS_OPMAP               ( ERR_BAS_BGN + 5 )		//���õײ㺯��ʧ��
#define ERR_BAS_ERRCONFID           ( ERR_BAS_BGN + 6 )		//����Ļ���
#define ERR_BAS_NOENOUGHCHLS        ( ERR_BAS_BGN + 7 )		//ͨ��������
#define ERR_BAS_GETCFGFAILED		( ERR_BAS_BGN + 8 )		//��ȡ����ʧ��

#define ERR_MIXER_NONE              ( ERR_MIXER_BGN + 0 )	//�޴��� 
#define ERR_MIXER_CALLDEVICE        ( ERR_MIXER_BGN + 1 )	//���õײ㺯��ʧ��
#define ERR_MIXER_MIXING            ( ERR_MIXER_BGN + 2 )	//���ڻ���״̬
#define ERR_MIXER_NOTMIXING         ( ERR_MIXER_BGN + 3 )	//δ����״̬
#define ERR_MIXER_ERRCONFID         ( ERR_MIXER_BGN + 4 )	//��Ч�Ļ���ID
#define ERR_MIXER_CHNINVALID        ( ERR_MIXER_BGN + 5 )	//���ͨ��ʧ��
#define ERR_MIXER_STATUIDLE         ( ERR_MIXER_BGN + 6 )	//������״̬����

/*������*/
#define SAFE_DELETE(ptr) 	\
{	\
	if(NULL != ptr)	\
	{	\
		delete ptr;	\
		ptr = NULL;	\
	}	\
} 
#define SAFE_DELETE_ARRAY(ptr) 	\
{	\
	if(NULL != ptr)	\
	{	\
		delete []ptr;	\
		ptr = NULL;	\
	}	\
} 

/*��ȫ���˺�*/
#define PTR_NULL_VOID(param) 	\
{	\
	if ( NULL == param )	\
	{	\
		return;	\
	}	\
}

#define PTR_NULL_RETURN(param, out) 	\
{	\
	if ( NULL == param )	\
	{	\
		return out;	\
	}	\
}

#define PTR_NULL_CONTINUE(param) 	\
{	\
	if ( NULL == param )	\
	{	\
		continue;	\
	}	\
}

/*�ӿڵ���ʧ��*/


#endif