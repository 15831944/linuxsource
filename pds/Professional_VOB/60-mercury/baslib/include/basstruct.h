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

typedef struct tagChnInfo{
    enum ChnType
	{
        AudioChn = 1,
        VideoChn
	}tChnType;

    u8              byChnType;
    BOOL32          bIsUsed;            // �Ƿ�ʹ��
    CConfId         m_cChnConfId;
    u32             m_dwLastFUPTick;    //����յ��ؼ�֡�����Tick��
    TBasChnStatus   m_tChnStatus;
}TChannelInfo;

typedef struct tagAdpGrpInfo{

    tagAdpGrpInfo()
    {
        bIsUsed = FALSE;
        m_wLocalBindPort = 0;
        m_wLocalRcvPort = 0;
        byGrpIdx        = 0;
        byCanUseChnNum  = 0;
        byAudioOrVideoGrp = 0;
        memset(abyChnInfo, 240, sizeof(abyChnInfo) );
    }
    
    enum EGRPTYPE
    {
        AUDIO = 1,
        VIDEO
    }tGrpType;
    
    u8             byGrpType;
    BOOL32         bIsUsed;                    // �Ƿ�ʹ��         
    u16            m_wLocalBindPort;           // ���ذ���ʼ�˿ں�    
    u16            m_wLocalRcvPort;            // ���ؽ�����ʼ�˿ں�
    u8             byGrpIdx;                   // ���������
    u8             byCanUseChnNum;             // ���������ܵ�ͨ����
    u8             byNextChnNum;               // ����������һ����ͨ����
    u8             byAudioOrVideoGrp;          // ���������ͣ�1����Ƶ��2����Ƶ
    s32            abyChnInfo[MAX_CHANNEL_NUM_GROUP];   // ���������ͨ����

    u8 AddOneChannel( u8 byChnNo )
    {
        if(byCanUseChnNum < MAX_CHANNEL_NUM_GROUP)
        {
            abyChnInfo[byCanUseChnNum] = byChnNo;
        }
        byCanUseChnNum++;
        return 0;
    }

}TAdpGrpInfo;

#endif // _BASSTRUCT_H_
