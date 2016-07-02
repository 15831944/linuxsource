/*****************************************************************************
   ģ����      : KDVSNMPMANAGER
   �ļ���      : kdvnpif.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: �������Ϣ����Ϣ�嶨��,���������
   ����		   : ½����
   �汾        : V5.0  Copyright(C) 2009-2012 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾     �޸���      �޸�����
   2009/12/31  5.0      ½����      ����
******************************************************************************/
#ifndef KDV_NPIF_H
#define KDV_NPIF_H

//��Ϣ�嶨��ͷ�ļ�
#include "smevent.h"

//SM��NP��������Ϣ������
#include "smmsg.h"

//SM��NP�����ĳ��ú�����װ
#include "smTool.h"

//���������Ϣ�����ͷ�ļ�
#include "smcfginfo.h"

//���������Ϣ�����ͷ�ļ�
#include "smperformanceinfo.h"

//����ͼ�����Ϣ�����ͷ�ļ�
#include "smmockmachineinfo.h"


//����Ϊ����Ϣ����Ϣ�����

//����ϢΪNP��SM���͵����������Ϣ
//NP��������ΪSUBEV_TYPE_REQ
//SM�ظ�����ΪSUBEV_TYPE_ACK/SUBEV_TYPE_NACK
//NP���͵�MsgTypeΪTNM_MSG_GET(��ȡ)/TNM_MSG_SET(����)
//SM�ظ���Ϣ��ΪCBoardCfgInfo/CMtCfgInfo��Ӧ��GetBuffer
//NP�����ʱ��ֱ�ӵ���CCfgInfoBase/CMtCfgInfo(const s8* pszBuffer, u32 dwSize)����
//-----OSPEVENT( EV_NP_SM_DEVICE_CFG,					EV_NP_BGN + 1 );


//����ϢΪNP��SM���͵Ļ�ȡ��ǰ���������Ϣ
//NP��������ΪSUBEV_TYPE_REQ
//SM�ظ�����ΪSUBEV_TYPE_ACK/SUBEV_TYPE_NACK
//NP���͵�MsgTypeΪTNM_MSG_GET(��ȡ)
//SM�ظ���Ϣ��ΪCDevPfmInfoBase��Ӧ��GetBuffer
//NP�����ʱ��ֱ�ӵ���CDevPfmInfoBase(const s8* pszBuffer, u32 dwSize)����
//-----OSPEVENT( EV_NP_SM_DEVICE_PFM,					EV_NP_BGN + 2 );


//����ϢΪNP��SM���͵�����ͳ�������Ϣ
//NP��������ΪSUBEV_TYPE_REQ
//SM�ظ�����ΪSUBEV_TYPE_ACK(�ظ�)/SUBEV_TYPE_NACK(�ܾ�)/SUBEV_TYPE_NOTIFY(֪ͨ)/SUBEV_TYPE_FINISH()
//NP���͵�MsgTypeΪTNM_MSG_GET(��ȡ)/TNM_MSG_NTF(֪ͨ)
//���ڿ��ǵ����Ƚϴ�SM���������
//SM�ظ���Ϣ������
//SUBEV_TYPE_ACK��Ϣ�壺
//SUBEV_TYPE_NOTIFY��Ϣ�壺CDevPfmInfoNotify��Ӧ��GetBuffer(���ܷ���Σ�ÿ�����1000)
//SUBEV_TYPE_FINISH��Ϣ�壺u32 num(���ܵ�����)
//-----OSPEVENT( EV_NP_SM_DEVICE_PFMSTATISTIC,			EV_NP_BGN + 3 );


//����ϢΪNP��SM���͵Ļ���ͼ�����Ϣ
//NP��������ΪSUBEV_TYPE_REQ
//SM�ظ�����ΪSUBEV_TYPE_ACK/SUBEV_TYPE_NACK
//NP���͵�MsgTypeΪTNM_MSG_GET(��ȡ)
//SM�ظ���Ϣ��ΪCMockMachineInfo��Ӧ��GetBuffer
//NP�����ʱ��ֱ�ӵ���CMockMachineInfo(const s8* pszBuffer, u32 dwSize)����
//-----OSPEVENT( EV_NP_SM_DEVICE_MOCKMACHIN,			EV_NP_BGN + 4 );

#endif
