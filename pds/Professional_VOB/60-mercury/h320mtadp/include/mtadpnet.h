/*****************************************************************************
ģ����      : h320�ն�����ģ��
�ļ���      : MtAdpNet.h
����ļ�    : MtAdpNet.cpp
�ļ�ʵ�ֹ���: ʵ��ģ��
����        : ������
�汾        : V3.6  Copyright(C) 1997-2003 KDC, All rights reserved.
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
2005/06/07  3.6         ������      ����
******************************************************************************/

#ifndef __MTADPNET_H
#define __MTADPNET_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mtadpcom.h"
#include "kdvmedianet.h"

class CMtAdpInst;
class CMtAdpNet  
{
public:
	CMtAdpNet(CMtAdpInst * const pcIns);
	virtual ~CMtAdpNet();

    //��Ƶ���ն����ʼ��/�˳�
    BOOL32 VidNetRcvInit();    
    BOOL32 VidNetRcvQuit();

    //��Ƶ���ն����ʼ��/�˳�
    BOOL32 AudNetRcvInit();
    BOOL32 AudNetRcvQuit();

    //��Ƶ���Ͷ����ʼ��/�˳�
    BOOL32 VidNetSndInit();
    BOOL32 VidNetSndQuit();

    //��Ƶ���Ͷ����ʼ��/�˳�
    BOOL32 AudNetSndInit();
    BOOL32 AudNetSndQuit();

    //��Ƶ��������
    BOOL32 VidNetSnd(u8 *pbyData, u32 dwLen);

    //��Ƶ��������
    BOOL32 AudNetSnd(u8 *pbyData, u32 dwLen);

    //������Ϣ��ӡ���
    void MtAdpLog(u8 byLevel, s8 * pInfo, ...);
  
protected:
    CKdvMediaRcv    * m_pcVidNetRcv;
    CKdvMediaRcv    * m_pcAudNetRcv;
    CKdvMediaSnd    * m_pcVidNetSnd;
    CKdvMediaSnd    * m_pcAudNetSnd;

    CMtAdpInst      * m_pcMtAdpIns;    

    u8               m_byAudSndMediaType;   //��Ƶ����ý������
    u8               m_byVidSndMediaType;   //��Ƶ����ý������

    u8              * m_byRtpPackBuf;       //rtp�����������Ƶ����

    SEMHANDLE       m_semVidSnd;            //��Ƶ�����ź���
    SEMHANDLE       m_semAudSnd;            //��Ƶ�����ź���   
   
};

#endif // __MTADPNET_H
