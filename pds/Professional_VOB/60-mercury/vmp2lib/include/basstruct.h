/*****************************************************************************
   ģ����      : ����������
   �ļ���      : basinst.h
   ����ʱ��    : 2003�� 6�� 25��
   ʵ�ֹ���    : ����궨���ͨ���ṹ����ͷ�ļ�
   ����        : ������
   �汾        : 
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2006/03/21  4.0         �ű���      �����Ż�
******************************************************************************/ 
#ifndef _BASSTRUCT_H_
#define _BASSTRUCT_H_

#include "mcuconst.h"
#include "mcustruct.h"

// �����顢ͨ������
#define TYPE_AUDIO                  (u8)1           // ��Ƶ����
#define TYPE_VIDEO                  (u8)2           // ��Ƶ����
#define LENGTH_TYPE                 (u8)64          // ����Ƶͨ�����ͳ���

#define MAX_CHANNEL_NUM_GROUP       (u8)3           // һ�����������ͨ����
#define MAX_CHANNELS_BAS            (u8)15          // Bas�����ͨ����

#define CHECK_IFRAME_INTERVAL       (u16)1000       // ���ؼ�֡ʱ����(ms)
#define DBG_PRINT                   printf
#define FIRST_REGACK                (u8)1           // ��һ���յ�ע��Ack
#define BAS_CONNETC_TIMEOUT         (u16)3*1000     // ���ӳ�ʱֵ3s
#define BAS_REGISTER_TIMEOUT        (u16)5*1000     // ע�ᳬʱֵ5s

#define LOCALBINDSTARTPORT          (u16)10300
#define BAS_PRI                     (u16)60

typedef struct tagChnInfo
{
    tagChnInfo()
    {
        m_dwLastFUPTick = 0;
    }

    CConfId         m_cChnConfId;
    u32             m_dwLastFUPTick;    //����յ��ؼ�֡�����Tick��

    THDBasVidChnStatus  m_tChnStatus;
    THDBasDVidChnStatus m_tChnStatusD;
}TChannelInfo;


#endif // _BASSTRUCT_H_
