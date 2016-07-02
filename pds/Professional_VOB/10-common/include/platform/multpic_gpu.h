/*****************************************************************************
  ģ����      : ý�����ģ��
  �ļ���      : mediactrl.h
  ����ļ�    : 
  �ļ�ʵ�ֹ���: �໭��ϳ����ϲ�API ��װ
  ����        : ��С��
  �汾        : V1.0  Copyright(C) 1997-2009 KDC, All rights reserved.
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2009/02/20  V1.0        ��С��       ����
  2009/08/13  V1.1        ��С��       ����
******************************************************************************/
#ifndef _MULPIC_GPU_H_
#define _MULPIC_GPU_H_

#include "codecwrapper_common.h"

class CSemaphoreCom;

class CMulPic;
typedef struct
{
    CMulPic* pcMulPic;            //����ϳ���ָ��
    l32 l32EncNo;                 //���������
}TProcParam;
#define MERGE_NUM (3)
//����ϳ����Ͷ���
typedef enum
{
        MERGE_M0 = 0,              //�޻���ϳ�
        MERGE_M1 = 100,            //һ����ϳ�(ԭPIC_MERGE_ONE)
        MERGE_M2 = 200,            //������ϳ�(ԭPIC_MERGE_VTWO)
        MERGE_M2_1_BR1,            //������ϳ�(һ��һС��С�����ڴ����ڵ����½ǣ�ԭPIC_MERGE_ITWO)
        MERGE_M3_T1 = 300,         //������ϳ�(��С��ȣ�һ�����£�ԭPIC_MERGE_THREE)
        MERGE_M3_B1,               //������ϳ�(��С��ȣ�����һ��)
        MERGE_M3_1_B2,             //������ϳ�(һ����С��С��������)
        MERGE_M3_1_T2,             //������ϳ�(һ����С��С��������)
        MERGE_M3_1_R2,             //������ϳ�(һ����С������棬��С����)
        MERGE_M4 = 400,            //�Ļ���ϳ�(ԭPIC_MERGE_FOUR)
        MERGE_M4_1_R3,             //�Ļ���ϳ�(һ����С��С�������ң�ԭPIC_MERGE_SFOUR)
        MERGE_M6 = 600,            //������ϳ�(��С��ȣ���������)
        MERGE_M6_1_5,              //������ϳ�(һ����С��ԭPIC_MERGE_SIX)
        MERGE_M6_2_B4,             //������(������С��С��������)
        MERGE_M7_3_TL4 = 700,      //�߻���ϳ�(������С���ĸ�С���������Ͻ�)
        MERGE_M7_3_TR4,            //�߻���ϳ�(������С���ĸ�С���������Ͻ�)
        MERGE_M7_3_BL4,            //�߻���ϳ�(������С���ĸ�С���������½�)
        MERGE_M7_3_BR4,            //�߻���ϳ�(������С���ĸ�С���������½ǣ�ԭPIC_MERGE_SEVEN)
        MERGE_M7_3_BLR4,           //�߻���ϳ�(������С���ĸ�С������������������)
        MERGE_M7_3_TLR4,           //�߻���ϳ�(������С���ĸ�С������������������)
        MERGE_M8_1_7 = 800,        //�˻���ϳ�(һ����С��ԭPIC_MERGE_EIGHT)
        MERGE_M9 = 900,            //�Ż���ϳ�(ԭPIC_MERGE_NINE)
        MERGE_M10_2_R8 = 1000,     //ʮ����ϳ�(�����С���˸�С�������Ҳ࣬ԭPIC_MERGE_TEN)
        MERGE_M10_2_B8,            //ʮ����ϳ�(�����С���˸�С���������棬ԭPIC_MERGE_TEN_2_8)
        MERGE_M10_2_T8,            //ʮ����ϳ�(�����С���˸�С����������)
        MERGE_M10_2_L8,            //ʮ����ϳ�(�����С���˸�С���������)
        MERGE_M10_2_TB8,           //ʮ����ϳ�(����������У����¸��ĸ�С���棩
        MERGE_M10_1_9,             //ʮ����ϳ�(һ���С��ԭPIC_MERGE_TEN_1_9)
        MERGE_M12_1_11 = 1200,     //ʮ������ϳ�(һ��ʮһС��ԭPIC_MERGE_TWELVE)
        MERGE_M13_TL1_12 = 1300,   //ʮ������ϳ�(���������Ͻǣ�PIC_MERGE_THIRTEEN)
        MERGE_M13_TR1_12,          //ʮ������ϳ�(���������Ͻ�)
        MERGE_M13_BL1_12,          //ʮ������ϳ�(���������½�)
        MERGE_M13_BR1_12,          //ʮ������ϳ�(���������½�)
        MERGE_M13_1_ROUND12,       //ʮ������ϳ�(������У�С���滷������)
        MERGE_M14_1_13 = 1400,     //ʮ�Ļ���ϳ�(һ��ʮ��С��ԭPIC_MERGE_FOURTEEN)
        MERGE_M14_TL2_12,          //ʮ�Ļ���ϳ�(�������������Ͻ�)
        MERGE_M15_T3_12 = 1500,    //ʮ�廭��ϳ�(������ʮ��)
        MERGE_M16 = 1600,          //ʮ������ϳ�(ԭPIC_MERGE_SIXTEEN)
        MERGE_M16_1_15,            //ʮ������ϳ�(һ��ʮ��С��ԭPIC_MERGE_SIXTEEN_1_15)
        MERGE_M20 = 2000,          //��ʮ����ϳ�
        MERGE_M25 = 2500,          //��ʮ�廭��ϳ�(ԭPIC_MERGE_TWENTYFIVE)
        MERGE_M36 = 3600,          //��ʮ������ϳ�(ԭPIC_MERGE_THIRTYSIX)
        MERGE_M49 = 4900,          //��ʮ�Ż���ϳ�(ԭPIC_MERGE_FORTYNINE)
        MERGE_M64 = 6400,           //��ʮ�Ļ���ϳ�(ԭPIC_MERGE_SIXTYFOUR)
        MERGE_MAX                  //����ϳɷ�ʽ���ֵ
} MergeStyle;

#define PIC_MERGE_NUM_MAX 64 //����ϳ�·�������ֵ
#define PIC_ENC_NUM_MAX 10   //����·�������ֵ

enum enNoStreamBack
{
	NoStrm_PlyBlk = 0,		//��ɫ����(Ĭ��)
	NoStrm_PlyLst = 1,		//������һ֡
	NoStrm_PlyBmp_dft = 2,	//Ĭ��ͼƬ
	NoStrm_PlyBmp_usr = 3,	//�û�����ͼƬ
	NoStrm_Total
};
//����ϳɳ�ʼ������
typedef struct
{
    u32   dwType;           //����ϳ�����
    u32   dwTotalMapNum;    //���뻭��ϳɵ�ͨ����
}TMulPicParam;

//״̬
typedef struct
{
    BOOL32  bMergeStart;     //�Ƿ�ʼ����ϳɣ�
    u32 dwMergeType;          //����ϳ����� 
    u8 byChnNum;          //��ǰ���ս����ͨ����
    TNetAddress atNetRcvAddr[PIC_MERGE_NUM_MAX]; //��ͨ����������յ�ַ

    u8 byEncNum;               //���뷢�͵�ͨ����
    TVidEncParam atVidEncParam[PIC_ENC_NUM_MAX]; //ͼ��������
    TNetAddress atNetSndAddr[PIC_ENC_NUM_MAX]; //���緢�͵�ַ    
}TMulPicStatus;

typedef struct
{
    TVidRecvStatis atRecvStatis[PIC_MERGE_NUM_MAX];
    TVidSendStatis atSenderStatis[PIC_ENC_NUM_MAX];
}TMulPicStatis;    

typedef struct 
{
    u8 RColor;    //R����
    u8 GColor;    //G����
    u8 BColor;    //B����	
}TMulPicColor;

typedef struct 
{
    TMulPicColor tMulPicColor; 
    BOOL32 bShow;     //�Ƿ���ʾ�߿�	
}TDrawFrameColor;
#define MAX_MUL_NUM 10

class CODECWRAPPER_API CMulPic
{
public:
	CMulPic();
	~CMulPic();
	
    /*************************************************************
    ������    : Initialize
    ����      ���ϳɳ�ʼ��
    ����˵��  ��tMulPicParam [I]�ϳɲ���            

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
	BOOL32 Initialize(TMulPicParam tMulPicParam);

    /*************************************************************
    ������    : Quit
    ����      ���ϳ��˳�
    ����˵��  ����            

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
	BOOL32 Quit(); 
	
    /*************************************************************
    ������    : SetChannelParam
    ����      ��ָ���ϳ�����ͨ��
    ����˵��  ��byChnNo         [I] Ҫ�趨��ͨ����
                tNetRcvAddr     [I] ָ���������ݵ�IP��ַ�Ͷ˿�            

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
	BOOL32 SetChannelParam(u8 byChnNo, TNetAddress tNetRcvAddr);

    /*************************************************************
    ������    : SetChannelParam
    ����      ��ָ���ϳ�����ͨ��
    ����˵��  ��byChnNo         [I] Ҫ�趨��ͨ����
                tNetRcvAddr     [I] ָ���������ݵ�IP��ַ�Ͷ˿�  
                tRtcpNetRcvAddr [I] Rtcp��IP��ַ�Ͷ˿� ��ָ�

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
    BOOL32 SetChannelParam(u8 byChnNo, TNetAddress tNetRcvAddr, TNetAddress tRtcpNetRcvAddr,  TNetAddress tRtcpNetLocalAddr);

    /*************************************************************
    ������    : SetChannelActive
    ����      ��ָ������ͨ���Ƿ���
    ����˵��  ��byChnNo         [I] Ҫ�趨��ͨ����
                bActive         [I] TRUEΪ������FALSEΪ������

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
    BOOL32 SetChannelActive(u8 byChnNo, BOOL32 bActive);

    /*************************************************************
    ������    : ClearChannelCache
    ����      �����ָ������ͨ���Ļ�������
    ����˵��  ��byChnNo         [I] Ҫ�趨��ͨ����               

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
    BOOL32 ClearChannelCache(u8 byChnNo);

    /*************************************************************
    ������    : SetEncActive
    ����      ��ָ���������ͨ���Ƿ���
    ����˵��  ��byChnNo         [I] Ҫ�趨��ͨ����
                bActive         [I] TRUEΪ������FALSEΪ������

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
    BOOL32 SetEncActive(u8 byChnNo, BOOL32 bActive);

    /*************************************************************
    ������    : SetAllEncActive
    ����      ���������б�������Ƿ���
    ����˵��  ��bActive         [I] TRUEΪ������FALSEΪ������               

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
    BOOL32 SetAllEncActive(BOOL32 bActive);

    /*************************************************************
    ������    : StartMerge
    ����      ����ʼ�ϳ�
    ����˵��  ��ptVideoEncParam     [I] �����������ָ��
                ptNetSndAddr        [I] ����Ŀ�ص�ַ�Ͷ˿�����ָ��
                ptNetSndLocalAddr   [I] ���ص�ַ�Ͷ˿�����ָ��
                nNum                [I] ���������

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
    BOOL32 StartMerge(TVidEncParam* ptVideoEncParam, TNetAddress* ptNetSndAddr, TNetAddress* ptNetSndLocalAddr,  l32 nNum);

    /*************************************************************
    ������    : SetEncParam
    ����      ������ָ��ͨ���������
    ����˵��  ��byChnNo              [I]ͨ����
                tMulPicStatus        [I] �������

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE   
    **************************************************************/
    BOOL32 SetEncParam(u8 byChnNo, TVidEncParam* ptVideoEncParam);

    /*************************************************************
    ������    : StopMerge
    ����      ��ֹͣ�ϳ�
    ����˵��  ����

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
	BOOL32 StopMerge(void);
   
    /*************************************************************
    ������    : GetActiveStatus
    ����      ��ȡ����/ֹͣ ״̬
    ����˵��  ����
    
    ����ֵ˵����TRUE:Ϊ����  FALSE:Ϊֹͣ
    **************************************************************/
    BOOL32 GetActiveStatus();
    
    /*************************************************************
    ������    : GetStatus
    ����      ��ȡ״̬����
    ����˵��  ��tMulPicStatus        [IO] ״̬����
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
	BOOL32 GetStatus(TMulPicStatus &tMulPicStatus);

    /*************************************************************
    ������    : GetStatis
    ����      ��ȡͳ��
    ����˵��  ��tMulPicStatus        [IO] ״̬����
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
	BOOL32 GetStatis(TMulPicStatis &tMulPicStatis);

    /*************************************************************
    ������    : SetMulPicType
    ����      �����û���ϳɷ�ʽ
    ����˵��  ��byNewType          [I]  ����ϳɷ�ʽ�����MergeStyle��
    ֧�ֶ�̬�л�
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
	BOOL32 SetMulPicType(u32 dwNewType);

    /*************************************************************
    ������    : ChangeBitRate
    ����      �����ñ��뷢�͵�����
    ����˵��  ��byChnNo           [I] ���뷢��ͨ����
                dwBitRate         [I] ����ֵ����λ (Kbit/s)
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
	BOOL32 ChangeEncBitRate(u8 byChnNo, u32 dwBitRate);
    
    /*************************************************************
    ������    : SetFastUpdata
    ����      �����ñ�������ؼ�֡
    ����˵��  ��l32Num             [I]���������
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
    BOOL32 SetFastUpdata(u8 byChnNo);
	
    /*************************************************************
    ������    : GetVidRecvStatis
    ����      ����ȡͨ������ͳ����Ϣ
    ����˵��  ��byChnNo           [I] ͨ����
                tKdvDecStatis     [IO] ͳ����Ϣ
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE     
    **************************************************************/
	BOOL32 GetVidRecvStatis(u8 byChnNo, TVidRecvStatis &tVidRecvStatis);
    
    /*************************************************************
    ������    : GetVidSendStatis
    ����      ����ȡͨ������ͳ����Ϣ
    ����˵��  ��byChnNo           [I] ͨ����
                tKdvDecStatis     [IO] ͳ����Ϣ
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE     
    **************************************************************/
	BOOL32 GetVidSendStatis(u8 byChnNo, TVidSendStatis &tVidSendStatis);

    /*************************************************************
    ������    : SetNetRecvFeedbackVideoParam
    ����      ����������ͼ�����������ش�����
    ����˵��  ��TNetRSParam         [I] �ش�����
                bRepeatSnd          [I] �ش�����
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
	BOOL32  SetNetRecvFeedbackVideoParam(TNetRSParam tNetRSParam, BOOL32 bRepeatSnd = FALSE);

    /*************************************************************
    ������    : SetNetSendFeedbackVideoParam
    ����      ����������ͼ������緢���ش�����
    ����˵��  ��wBufTimeSpan         [I] �ش�����
                bRepeatSnd           [I] �ش�����
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE      
    **************************************************************/
	BOOL32  SetNetSendFeedbackVideoParam(u16 wBufTimeSpan, BOOL32 bRepeatSnd = FALSE);	

    /*************************************************************
    ������    : SetBGDAndSidelineColor
    ����      �����û���ϳɱ�����ɫ,�Լ��߿���ɫ
    ����˵��  ��tBGDColor          [I]  �ϳɱ�����ɫ
                tBGDFrameLineColor [I]  �ϳɱ����߿���ɫ
                TDrawFrameColor    [I]  ǰ���߿���ɫ(����)

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
    BOOL32 SetBGDAndSidelineColor(TMulPicColor tBGDColor, 
                                  TMulPicColor tBGDFrameLineColor,
                                  TDrawFrameColor atFramelineColor[PIC_MERGE_NUM_MAX]); 

    /*************************************************************
    ������    : SetVideoActivePT
    ����      ������ ��̬��Ƶ�غɵ� Playloadֵ
    ����˵��  ��byChnNo        [I] ���ս���ͨ����
                byRmtActivePT  [I] 
                byRealPT       [I] 

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
    BOOL32 SetVideoActivePT(u8 byChnNo, u8 byRmtActivePT, u8 byRealPT);

    /*************************************************************
    ������    : SetDecryptKey
    ����      ������ ��̬��Ƶ�غɵ� Playloadֵ
    ����˵��  ��byChnNo        [I] ���ս���ͨ����
                pszKeyBuf      [I] 
                wKeySize       [I] 
                byDecryptMode  [I]

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
    BOOL32 SetDecryptKey(u8 byChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode=DES_ENCRYPT_MODE);        
    
    //���ü���key�ִ������������Ķ�̬�غ�PTֵ, pszKeyBuf����ΪNULL-��ʾ������

    /*************************************************************
    ������    : SetEncryptKey
    ����      �����ü���key�ִ������������Ķ�̬�غ�PTֵ, pszKeyBuf����ΪNULL-��ʾ������
    ����˵��  ��byChnNo        [I] ���뷢��ͨ����
                pszKeyBuf      [I] 
                wKeySize       [I] 
                byEncryptMode  [I]

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
    BOOL32 SetEncryptKey(u8 byChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode=DES_ENCRYPT_MODE);
    
    /*************************************************************
    ������    : SetLogo
    ����      ������
    ����˵��  ��byChnNo        [I] ���뷢��ͨ����
                pszBuf         [I] �ַ�����ָ��
                wSize          [I] �ַ�������

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
    BOOL32 SetLogo(u8 byChnNo, s8 *pszBuf, u16 wSize);

    /*************************************************************
    ������    : SetLogo
    ����      ������
    ����˵��  ��byChnNo        [I] ���뷢��ͨ����
                pszBuf         [I] BMPͼƬ��ַ
                dwSize         [I] BMPͼƬ����
				dwLen          [I] BMPͼƬ�б�����ַ�����(�����ص���)

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
	BOOL32 SetLogo(u8 byChnNo, u8* pbyBmpbuf, u32 dwSize, u32 dwLen);
    /*************************************************************
    ������    : SetLogoActive
    ����      ������
    ����˵��  ��byChnNo        [I] ͨ����
                bActive        [I] �Ƿ���
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
    BOOL32 SetLogoActive(u8 byChnNo, BOOL32 bActive);
    
    /*************************************************************
    ������    : SaveDecIn
    ����      ���������ǰ����
    ����˵��  ��l32ChnNo         ������ͨ����
                l32FrameNum      Ҫ�����֡��
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
    BOOL32 SaveDecIn(l32 l32ChnNo, l32 l32FrameNum);

    /*************************************************************
    ������    : SaveDecOut
    ����      ����������YUVͼ��
    ����˵��  ��l32ChnNo         ������ͨ����
                l32FrameNum      Ҫ�����֡��
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
    BOOL32 SaveDecOut(l32 l32ChnNo, l32 l32FrameNum);

    /*************************************************************
    ������    : SaveEncIn
    ����      ���������ǰYUV
    ����˵��  ��l32ChnNo         ������ͨ����
                l32FrameNum      Ҫ�����֡��
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
    BOOL32 SaveEncIn(l32 l32ChnNo, l32 l32FrameNum);

    /*************************************************************
    ������    : SaveEncOut
    ����      ��������������
    ����˵��  ��l32ChnNo         ������ͨ����
                l32FrameNum      Ҫ�����֡��
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
    BOOL32 SaveEncOut(l32 l32ChnNo, l32 l32FrameNum);
    
	/*************************************************************
    ������    : SetNoStreamBak
    ����      �����ÿ��б���
    ����˵��	BakType      ��������
                pbyBmp       ͼƬ·��
				dwChnNo      ����ͨ��
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
	s32 SetNoStreamBak(u32 BakType, s8 *pszBmp = NULL, s32 nChnNo = -1);

	/*************************************************************
    ������    : SetResizeMode
    ����      ������ǰ�����ģʽ (������ģʽ��:16:9 ���������ǿ��ʹ�üӺڱ�ģʽ)
    ����˵��	0      �Ӻڱ�
                1      �ñ�
				2      �ǵȱ����� 
				Ĭ�� 1
    ����ֵ˵����    
    **************************************************************/
	u16 SetResizeMode(u32 dwMode);


	 /*************************************************************
    ������    : GetEncStatus
    ����      ����ȡ����ͨ��״̬
    ����˵��  ��byChnNo           [I] ���뷢��ͨ����
                bEncActive        [IO] 
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
	BOOL32 GetEncStatus(u8 byChnNo, BOOL32& bEncActive);
    //////////////////////////////////////////////////////////////////////////
    //�����Ƿ��û��ӿ� 
	BOOL32 SetBmp24Back4v3(u8* pData,  u32 dwWidth, u32  dwHeight, u16 wBitCount);
	BOOL32 SetBmp24Back16v9( u8* pData, u32 dwWidth, u32 dwHeight, u16 wBitCount);

	BOOL32 SetRcvInMarker(u8 byChNo, BOOL32 bMarker);       //����Э���հ���֡ģʽ
    l32 m_al32FrontWidth[PIC_MERGE_NUM_MAX];    //ǰ�����
    l32 m_al32FrontHeight[PIC_MERGE_NUM_MAX];   //ǰ���߶�   

    void* m_pListImg;
    void* m_pListAddLogo;
    void* m_pListEnc;                 //���ձ����б�
    void* m_pListDec;                 //���뷢���б�   

    void* m_ptProcessLock;           //�߳���        
    
    
    void* m_ptLock;                  //�߳���,�ϳ�
    void* m_ptCond;             
    
    void* m_aptLockEnc[PIC_ENC_NUM_MAX];     //�߳���������
    //void* m_aptCondEnc[PIC_ENC_NUM_MAX]; 
    CSemaphoreCom* m_aptCondEnc[PIC_ENC_NUM_MAX];
    
    void* m_pListDataBuf4v3;       
    void* m_pListDataBuf16v9;
    void* m_aptLockClsBuf[PIC_ENC_NUM_MAX];  //�߳�������������
    void* m_aptCondClsBuf[PIC_ENC_NUM_MAX]; 
    void* m_ptEnc0Lock;
    void* m_ptEnc1Lock;
    
    void* m_pListVidEncParam;    
    BOOL32 m_abBG16v9[PIC_ENC_NUM_MAX];            //�ϳɱ�����16��9    

    void SetAllImgActive(BOOL32 bActive);
    BOOL32 SetMergeParam();
    u32 RecalculateBitRate(u32 u32BitRate);
	u32 GetLoopTime();
	u32 GetMaxFrameRate();
	void SetYUV420Back();
    
    l32 m_l32ShowPicNum;

	void* m_ptBackBmpLock; 
	u32 m_bUseBackGroundbmp;
	u8* m_pDataYUV420;
	u32 m_dwbmpWidth;
	u32 m_dwbmpHeight;
	u8* m_pDataYUV420WS; //�����汾
	u32 m_dwbmpWidthWS;
	u32 m_dwbmpHeightWS;
	u8* m_pDataYUV420WS1080; //�����汾1080p
	BOOL32 m_bUsrImgShow[PIC_MERGE_NUM_MAX];
	u8* m_pUsrImgYuv[PIC_MERGE_NUM_MAX];
	u8  m_byDecNoDataCount[PIC_MERGE_NUM_MAX];
	u32 m_dwUsrImgWidth[PIC_MERGE_NUM_MAX];
	u32 m_dwUsrImgHeight[PIC_MERGE_NUM_MAX];
	BOOL32 m_bClearChannelImmediat;
	BOOL32 m_bIsEncChanged;  //��������Ƿ����仯
	BOOL32 m_bIsBgChanged;   //���б����Ƿ����仯
	u32 m_enNoStreamBack;  //���б������ģʽ
//add by taoz
	void* m_pGTime;
	BOOL32 m_bExitMergeLoop ;
	BOOL32 m_bExitMergeProcess ;
	BOOL32 m_bExitEncProcess[PIC_ENC_NUM_MAX];
	BOOL32 m_bExitClearBufProcess ;

    BOOL32 m_bExitMergeLoop1080P;
	BOOL32 m_bIsBaseGround;  //��ͼ�Ƿ����仯
	u32 m_TimerID ;//���嶨ʱ����� 
	u32 m_dwReszMode;
	u32 m_dwReszModeOld;
	u32 m_dwMaxEncWidth;
	u32 m_dwMaxEncHeight;
	u32 m_dwSelectMergebackID; // 0 : ���� �� 1: 720p ; 2:1080p
	u32 m_dwEncMode;			// 0x01 : SD 0x10 HD
	void *m_pvImageZoomHandle;
#ifdef _LINUX_
	pthread_rwlock_t m_rwlock0;
	pthread_rwlock_t    m_rwlock1;

    pthread_rwlockattr_t  m_attr0; 
	pthread_rwlockattr_t  m_attr1;
#endif
private:
    BOOL32 AppendEnc(TVidEncParam* ptVideoEncParam, 
                TNetAddress* ptNetSndAddr,
                TNetAddress* ptNetSndLocalAddr,
                l32 nChannelNum);

    BOOL32 AdjustEncParam();    
	BOOL32 AdjustDecPostSize(l32 dwMergeType, u32 dwBackWidth, u32 dwBackHeight);
    BOOL32 SetAllChannelActive(BOOL32 bActive);
    void AdjustMergeBackSize(TVidEncParam* ptVideoEncParam = NULL, l32 nChannelNum = 0);
    l32 GetFormat(l32 l32Width, l32 l32Height);
    BOOL32 Get16v9Mode(l32 l32Width, l32 l32Height);
    void ConvertEncParam(TVidEncParam* ptVideoEncParam, void* ptVoid, l32 nChannelNum);

    BOOL32 m_bActive;
    l32 m_l32MergeType;    
    l32 m_al32BackWidth[MERGE_NUM];      //�ϳɱ���ͼ���
    l32 m_al32BackHeight[MERGE_NUM];     //�ϳɱ���ͼ�߶�    
    
    TMulPicColor m_tBGDColor;                   //�ϳɱ���ɫ
    TMulPicColor m_tBGDFrameLineColor;          //�ϳɱ����߿�ɫ
    TDrawFrameColor m_atFramelineColor[PIC_MERGE_NUM_MAX]; //�ϳ�ǰ���߿�ɫ  
	TProcParam m_tProcParam[PIC_ENC_NUM_MAX];

	u32 m_dwEncMaxFrameRate;

public:
	u32 m_dwLastTimerTick;
	u32 m_dwCalcTimer;
	
	u32 m_dwLastMergeLoopTick;
	u32 m_dwCalcMergeLoop;

	u32* m_pdwLastEncTick;
	u32* m_pdwCalcEncFrame;
};

#endif //end of _MULPIC_GPU_H_
