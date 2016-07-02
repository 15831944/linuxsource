/*****************************************************************************
ģ����      : h320�ն�����ģ��
�ļ���      : MtAdpcom.h
����ļ�    : MtAdpcom.cpp
�ļ�ʵ�ֹ���: ����ģ��
����        : ������
�汾        : V3.6  Copyright(C) 1997-2003 KDC, All rights reserved.
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
2005/06/07  3.6         ������      ����
******************************************************************************/

#ifndef __MTADPCOM_H
#define __MTADPCOM_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mcustruct.h"
#include "mcuconst.h"
#include "kdvh320def.h"
#include "kdvh320.h"


//////////////////////////////////////////////////////////////////////////
//                              �궨��
//////////////////////////////////////////////////////////////////////////

//�����շ���ʼ�˿ں�(��ʱ��Ӧ�ô�dri���ȡ)
#define SWITCH_STARTPORT            30000

//���г�ʱʱ��(s)
#define CALLING_TIMEOUT             2

//��ʼԶң��ʱʱ��
#define STARTACTION_TIMEOUT         0

//E1ͨ��������
#define MAXNUM_CHAN                 4
//��Ƶ�����ŵ�����
#define AUD_RCV_INDEX               0
//��Ƶ�����ŵ�����
#define AUD_SND_INDEX               1
//��Ƶ�����ŵ�����
#define VID_RCV_INDEX               2
//��Ƶ�����ŵ�����
#define VID_SND_INDEX               3

//ÿ���ն�ռ�ö˿ڸ���
#define PORTNUM_PERMT               8
//��Ƶý����ն˿�(����ʼ�˿ڼ��������ͬ)
#define AUD_RCV_PORT                0
//��Ƶý����ƽ��ն˿�
#define AUD_RCV_CTRLPORT            1
//��Ƶý�巢�Ͷ˿�
#define AUD_SND_PORT                2
//��Ƶý����Ʒ��Ͷ˿�
#define AUD_SND_CTRLPORT            3
//��Ƶý����ն˿�
#define VID_RCV_PORT                4
//��Ƶý����ƽ��ն˿�
#define VID_RCV_CTRLPORT            5
//��Ƶý�巢�Ͷ˿�
#define VID_SND_PORT                6
//��Ƶý����Ʒ��Ͷ˿�
#define VID_SND_CTRLPORT            7

//��ӡ����
#define LOG_CRITICAL                0
#define LOG_WARNING                 1
#define LOG_INFO                    2
#define LOG_DEBUG                   3

#define  DEF_VID_NETBAND            8000*1024   //ȱʡ����Ƶλ��
#define  DEF_AUD_NETBAND            200*1024    //ȱʡ����Ƶλ��
#define  DEF_VID_FRAMESIZE          1024*128 
#define  DEF_AUD_FRAMESIZE          1024*5     
#define  DEF_FRAME_RATE	            25

//Ĭ��Զң�����ͷų�ʱʱ��
#define  DEF_LSDTOKEN_RELEASE_TIME  10

//def mcu id
#define  DEF_SMCUID                 2

//rtp�а��������������
#define  BUFLEN_RTPPACK             128*1024   

//�ŵ�״̬
enum CHAN_STATE
{
    CHAN_IDLE = 0,
    CHAN_OPENING,
    CHAN_CONNECTED
};

#define SAFE_DELETE(p)              \
    if(NULL != p)                   \
    {                               \
        delete p;                   \
        p = NULL;                   \
    }


//////////////////////////////////////////////////////////////////////////
//                          ���ݽṹ����
//////////////////////////////////////////////////////////////////////////

//�߼��ŵ��ṹ
typedef struct tagChannel
{
    u8                  m_byState;          //�ŵ�״̬
    TLogicalChannel     m_tLogicChan;       //�ŵ��ṹ
    TTransportAddr	    m_tSndMediaChannel;	//���Ͷ�ý���ŵ�(����net send��ʼ��)

public:
    tagChannel(void)
    {         
        Reset();
    }

    ~tagChannel(void)
    {
    }

    void Reset(void)
    {
        m_byState = CHAN_IDLE;
        memset(&m_tLogicChan, 0, sizeof(m_tLogicChan));
        memset(&m_tSndMediaChannel, 0, sizeof(m_tSndMediaChannel));
        m_tLogicChan.m_tMediaEncrypt.Reset(); 
        m_tLogicChan.m_byChannelType = MEDIA_TYPE_NULL; 
    }

}TChannel;

//semaphore
class CEnter
{
public:
    CEnter(SEMHANDLE semSnd)
    {        
        OspSemTake(semSnd);
        m_semSnd = semSnd;
    }
    
    ~CEnter()
    {
        OspSemGive(m_semSnd);
    }
    
private:
    SEMHANDLE m_semSnd;
};

#define ENTER(sem)  CEnter cEnter(sem);

//////////////////////////////////////////////////////////////////////////
//                          ȫ�ֽӿڶ���
//////////////////////////////////////////////////////////////////////////

/*=============================================================================
  �� �� ���� CapSetIn2Out
  ��    �ܣ� ���ϲ����������ת��ΪЭ��ջ���������� 
  ��    ���� TCapSupport &tCapSupport [in] ����������
             u32 dwTransCap           [in] ����������
             TCapSetEx &tCapSet       [out]Э��ջ������
  �� �� ֵ�� void 
=============================================================================*/
void CapSetIn2Out(TCapSupport &tCapSupport, u32 dwTransCap, TKdvH320CapSet &tCapSet);

/*=============================================================================
  �� �� ���� MediaTypeIn2Out
  ��    �ܣ� ���ϲ����ý������ת��ΪЭ��ջý������ 
  ��    ���� u8 byMediaType [in] ����ý������
  �� �� ֵ�� u8            [out]Э��ջý������
=============================================================================*/
u8  MediaTypeIn2Out(u8 byMediaType);

/*=============================================================================
  �� �� ���� MediaTypeIn2Out
  ��    �ܣ� ��Э��ջý������ת��Ϊ�ϲ����ý������ 
  ��    ���� u8 byMediaType [in] Э��ջý������
  �� �� ֵ�� u8             [out] ����ý������
=============================================================================*/
u8  MediaTypeOut2In(u8 dwMediaType);

/*=============================================================================
  �� �� ���� FeccActionIn2Out
  ��    �ܣ� ���ϲ�Զң����ת��ΪЭ��ջԶң���� 
  ��    ���� u8 byParam     [in] mcuԶң����
  �� �� ֵ�� u8             [out]Э��ջԶң����
=============================================================================*/
u8  FeccActionIn2Out(u8 byParam);

/*=============================================================================
  �� �� ���� FeccActionOut2In
  ��    �ܣ� ��Э��ջԶң����ת��Ϊ�ϲ�Զң����
  ��    ���� u8 byAction     [in] Э��ջԶң����
  �� �� ֵ�� u8             [out] mcuԶң����
=============================================================================*/
u8  FeccActionOut2In(u8 byAction);

/*=============================================================================
  �� �� ���� StackType2Cap
  ��    �ܣ� ��Э��ջ����ת��Ϊ������ 
  ��    ���� u8 byMediaType      [in] ����
             u8 byResolution     [in] �ֱ���
  �� �� ֵ�� u32            [out]������
=============================================================================*/
u32 StackType2Cap(u8 byMediaType, u8 byResolution = 0);

/*=============================================================================
  �� �� ���� MuxMode2Cap
  ��    �ܣ� �ɸ���ģʽת��Ϊ�ϲ���������ʽ  
  ��    ���� TKDVH320MuxMode &tMuxMode      [in] ����ģʽ
             TCapSupport &tCap              [out]������
  �� �� ֵ�� void 
=============================================================================*/
void MuxMode2Cap(TKDVH320MuxMode &tMuxMode, TKdvH320CapSet &tLocalCapSet, TCapSupport &tCap);

/*=============================================================================
  �� �� ���� IsEqualMuxMode
  ��    �ܣ� �ж�������ģʽ�Ƿ���� 
  ��    ���� TKDVH320MuxMode &tMuxMode1     [in]
             TKDVH320MuxMode &tMuxMode2     [in]
  �� �� ֵ�� BOOL32     (true - ��ȣ�false - ����)
=============================================================================*/
BOOL32 IsEqualMuxMode(const TKDVH320MuxMode &tMuxMode1, const TKDVH320MuxMode &tMuxMode2);

/*=============================================================================
  �� �� ���� GetAudioBitrate
  ��    �ܣ� �õ���Ƶ�ı�������� 
  ��    ���� u8 byAudioType     [in] ��Ƶ����
  �� �� ֵ�� u16                [out]��Ƶ�ı��������
=============================================================================*/
u16 GetAudioBitrate(u8 byAudioType);

/*=============================================================================
  �� �� ���� GetTsMaskAndTransFromBitrate
  ��    �ܣ� �ӻ������ʻ��ʱ϶����ʹ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wBitrate       [in] ��������
             u32 &dwTsMask      [out]ʱ϶����
             u32 &dwTransCap    [out]������
  �� �� ֵ�� BOOL32         (TRUE-�ɹ���FALSE-ʧ��)
=============================================================================*/
BOOL32 GetTsMaskAndTransFromBitrate(u16 &wBitrate, u32 &dwTsMask, u32 &dwTransCap);

/*=============================================================================
  �� �� ���� StrIPv4
  ��    �ܣ� ��ip��ַת��Ϊ�ַ��� 
  ��    ���� u32 dwIP (net order)
  �� �� ֵ�� s8 * 
=============================================================================*/
s8 * StrIPv4(u32 dwIP);

#endif //__MTADPCOM_H
