/*****************************************************************************
   ģ����      : MpwLib�໭�渴�ϵ���ǽ
   �ļ���      : mpwcfg.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ���������ļ�������
   ����        : john
   �汾        : V4.0  Copyright(C) 2001-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2005/09/25  1.0         john         ����
******************************************************************************/
#ifndef _VIDEO_MPWCFG_H_
#define _VIDEO_MPWCFG_H_

#include "kdvtype.h"
#include "osp.h"
#include "kdvsys.h"
#include "mcuconst.h"
#include "mcustruct.h"
#include "codeccommon.h"
#include "kdvmulpic.h"


#define INVALID_VALUE      (u8)0  //��Ч�Ĳ���ֵ
/************************************************************************/
/*  MEDIA_TYPE_H262,          0,            300,       31,      2,       2048,       0,       0,     25,    5,       576,     720, */
/************************************************************************/
const u16 wEnVidFmt[6][12]=
{
//����               ѹ�����ʿ��Ʋ��� ���ؼ�֡ ������� ��С���� ���������K ͼ����ʽ �ɼ��˿� ֡��       ͼ������         ��Ƶ�߶� ��Ƶ���
// mpeg4
 MEDIA_TYPE_MP4,           0,            25,        31,      3,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
// 261
 MEDIA_TYPE_H261,          0,            25,        31,      2,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
// 262(mpeg-2)
 MEDIA_TYPE_H262,          0,            25,        31,      2,       2048,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
// 263
 MEDIA_TYPE_H263,          1,            25,        13,      5,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
 // h263+
 MEDIA_TYPE_H263PLUS,      0,            300,       31,      3,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
// h264
 MEDIA_TYPE_H264,          0,            300,       30,      30,      2048,       0,       0,     25,    QC_MODE_QUALITY,       576,     720
};

class CMpwCfg
{
public:
	TMulPicParam m_tMulPicParam;   // ͼ���������
	CKDVVMPParam m_tStatus;        // ���浱ǰ����״̬
    TVideoEncParam tEncparamMp4;   // �������
    TVideoEncParam tEncparamH261;
    TVideoEncParam tEncparamH262;
    TVideoEncParam tEncparamH263;
    TVideoEncParam tEncparamH263plus;
    TVideoEncParam tEncparamH264;

public:
	// �豸����(��ѡ����)
	u8   byEqpType;                //��������
	u8   byEqpId;                  //���������
	s8   szAlias[MAXLEN_ALIAS+1];  //�������
	u32  dwLocalIp;                //����IP��ַ
	u16  wMcuId;		           //mcu ID

	// ��������(�������),�������û���Agent�·�
	u32  dwMcuIP;                 //Ҫ���ӵ�Mcu��IP��ַ
	u16  wMcuPort;                //Ҫ���ӵ�Mcu�Ķ˿ں�
	u16  wRecvStartPort;          //������ʼ�˿ں�
	u32  dwMaxSendBand;           //������緢�ʹ���

public:
    u16  m_wMcuIdB;               //Mcu.B ID
    u32  m_dwMcuIpB;              //Mcu.B ��IP��ַ
    u16  m_wMcuPortB;             //Mcu.B �Ķ˿�
	
	TVideoEncParam m_tVideoEncParam[2];  //ͼ��������(��ѡ����)

public:
	CMpwCfg();
	virtual ~CMpwCfg();
	BOOL32 ParseParam(void);
    void ReadConfigureFile(void);
	void GetDefaultParam(u8 byEnctype, TVideoEncParam& TEncparam);
};

#endif //_VIDEO_MPWCFG_H_
