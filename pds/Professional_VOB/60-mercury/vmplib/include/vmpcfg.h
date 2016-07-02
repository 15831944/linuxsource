/*****************************************************************************
   ģ����      : ���渴����(Video MultiPlexer)
   �ļ���      : VMPCfg.h
   ����ʱ��    : 2003�� 12�� 4��
   ʵ�ֹ���    : ���渴������������
   ����        : zhangsh
   �汾        : 
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
******************************************************************************/
#ifndef _VIDEO_MULTIPLEXER_CONFIG_H_
#define _VIDEO_MULTIPLEXER_CONFIG_H_

#include "kdvtype.h"
#include "osp.h"
#include "kdvsys.h"
#include "mcuconst.h"
#include "mcustruct.h"
#include "codeccommon.h"
#include "codeclib.h"
#include "kdvmulpic.h"

/***********************************************************************************************************************************/
/*  MEDIA_TYPE_H262,          0,            300,       31,      2,       2048,       0,       0,     25,    5,       576,     720, */
/***********************************************************************************************************************************/
const u16 EnVidFmt[6][12]=
{
//����               ѹ�����ʿ��Ʋ��� ���ؼ�֡ ������� ��С���� ���������K ͼ����ʽ �ɼ��˿� ֡��       ͼ������         ��Ƶ�߶� ��Ƶ���
//mpeg4
 MEDIA_TYPE_MP4,           0,            25,        31,      2,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
//261
 MEDIA_TYPE_H261,          0,            25,        31,      2,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
//262(mpeg-2)
 MEDIA_TYPE_H262,          0,            25,        31,      2,       2048,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
//263
 MEDIA_TYPE_H263,          1,            25,        31,      2,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
 //h263+
 MEDIA_TYPE_H263PLUS,      0,            300,       31,      2,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
//h264
 MEDIA_TYPE_H264,          0,            300,       51,      2,       2048,       0,       0,     25,    QC_MODE_QUALITY,       576,     720
};

// �����������ͱ�������
#define MIN_BITRATE_OUTPUT      (u16)128

#define		VMP_SMOOTH_OPEN		(u8)1
struct TVmpBitrateDebugVal
{
protected:
    BOOL32          m_bEnableBitrateCheat;          // �Ƿ����������������
    u16             m_wDefault;                     // ȱʡ�µ��İٷֱ�
public:
    void   SetEnableBitrateCheat(BOOL32 bEnable)
    {
        m_bEnableBitrateCheat = bEnable;
    }
    BOOL32 IsEnableBitrateCheat() const
    {
        return m_bEnableBitrateCheat;
    }
    u16    GetDecRateDebug(u16 wOriginRate) const
    {
        // guzh [10/19/2007] û���򷵻�ȱʡֵ
        wOriginRate = wOriginRate*m_wDefault/100;

        if (wOriginRate > MIN_BITRATE_OUTPUT)
            return wOriginRate;
        else
            return MIN_BITRATE_OUTPUT;
    }
    
    void   SetDefaultRate(u16 wPctRate)
    {
        m_wDefault = wPctRate;
    }    
};

class TVMPCfg
{
public:
	TMulPicParam m_tMulPicParam;   //ͼ���������
    CKDVVMPParam m_tStatus;        //���浱ǰ����״̬
	u32  dwPort;
    
public:
	//�豸����(��ѡ����)
	u8   byEqpType;                //��������
	u8   byEqpId;                  //���������
	s8   szAlias[MAXLEN_ALIAS+1];  //�������
	u32  dwLocalIp;                //����IP��ַ
	u8   byMcuId;		           //mcu iD

	//��������(�������), �������û���Agent�·�
	u32  dwMcuIP;                 //Ҫ���ӵ�Mcu��IP��ַ
	u16  wMcuPort;                //Ҫ���ӵ�Mcu�Ķ˿ں�
	u16  wRecvStartPort;          //������ʼ�˿ں�
	u32  dwMaxSendBand;           //������緢�ʹ���

	//�Ƿ��ƽ������, zgc, 2007-09-28
	//�������ֵ�˫���룬zgc, 2008-03-19
	u8   m_byIsUseSmoothSend;	  // ͨ����mcueqp.ini��������
    
public:
    u32  m_dwMcuIdB;              //Mcu B ID
    u32  m_dwMcuIpB;              //Mcu B ��IP��ַ
    u16  m_wRecvStartPortB;       //Mcu B ������ʼ�˿ں�
    u16  m_wMcuPortB;             //Mcu B �Ķ˿�
    
    //ͼ��������(��ѡ����)
	TVideoEncParam m_tVideoEncParam[2];

    TVmpBitrateDebugVal     m_tDebugVal;                //����ֵ
public:
    TVMPCfg();
	~TVMPCfg();
    void Clear();
	void PrintMulPicParam(void) const ;
	void PrintEncParam( u8 byVideoIdx ) const;
	
	BOOL ParseParam();
	void GetDefaultParam(u8 byEnctype,TVideoEncParam& TEncparam);
	void GetDisplayType( u8& byDisplayType );
    void ReadBitrateCheatValues();
    BOOL32 IsEnableBitrateCheat() const
    {
        return m_tDebugVal.IsEnableBitrateCheat();
    }
    u16 GetDecRateDebug(u16 wOriginRate) const
    {
        return m_tDebugVal.GetDecRateDebug(wOriginRate);
    }

	BOOL32 IsNoSmooth()
	{
		return ( 1 != m_byIsUseSmoothSend );
	}

	//  [1/11/2010 pengjie] Modify SetBackBoardOut
	BOOL32 ReadIsNeedBackBrdOut( void );
	// End
};

#endif //_BITRATE_ADAPTER_SERVER_CONFIG_H_
