/*****************************************************************************
  ģ����      : ���������
  �ļ���      : codecadapter.h
  ����ļ�    : 
  �ļ�ʵ�ֹ���: ���������
  ����        : ��С��
  �汾        : V1.0  Copyright(C) 1997-2009 KDC, All rights reserved.
  -----------------------------------------------------------------------------
  �޸ļ�¼:
  ��  ��      �汾        �޸���      �޸�����
  2009/03/16  V1.0        ��С��       ����
******************************************************************************/
#ifndef _CODECADAPTER_H_
#define _CODECADAPTER_H_

#include "codecwrapper_common.h"

//״̬
typedef struct
{
    BOOL32  bIsActive;             //�Ƿ�������
    TNetAddress tNetRcvAddr;       //��ͨ����������յ�ַ    
    TVidEncParam tVidEncParam;     //ͼ��������
    TNetAddress tNetSndAddr;       //���緢�͵�ַ    
}TVidAdapterStatus;

typedef struct
{
    TVidRecvStatis tRecvStatis;
    TVidSendStatis tSenderStatis;
}TVidAdapterStatis;    

class CODECWRAPPER_API CCodecAdapter
{
public:
	CCodecAdapter();
	~CCodecAdapter();	

    /*************************************************************
    ������    : Start
    ����      ����ʼ����
    ����˵��  ��tNetSndAddr        [I] ����Ŀ�ص�ַ�Ͷ˿�
                tNetSndLocalAddr   [I] ���ص�ַ�Ͷ˿�                 
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
    BOOL32 Start(TNetAddress tNetSndAddr, TNetAddress tNetSndLocalAddr);

    /*************************************************************
    ������    : Stop
    ����      ��ֹͣ����
    ����˵��  ����            
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
    BOOL32 Stop();

    /*************************************************************
    ������    : SetChannelParam
    ����      ��ָ���ϳ�����ͨ��
    ����˵��  ��tNetRcvAddr     [I] ָ���������ݵ�IP��ַ�Ͷ˿�  
                tRtcpNetRcvAddr [I] Rtcp��IP��ַ�Ͷ˿� ��ָ�

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
    BOOL32 SetChannelParam(TNetAddress tNetRcvAddr, TNetAddress tRtcpNetRcvAddr);

    /*************************************************************
    ������    : SetChannelActive
    ����      ��ָ������ͨ���Ƿ���
    ����˵��  ��byChnNo         [I] Ҫ�趨��ͨ����
                bActive         [I] TRUEΪ������FALSEΪ������

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
    BOOL32 SetChannelActive(BOOL32 bActive);       

    /*************************************************************
    ������    : SetEncParam
    ����      ������ָ��ͨ���������
    ����˵��  ��byChnNo              [I]ͨ����
                tMulPicStatus        [I] �������

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE   
    **************************************************************/
    BOOL32 SetEncParam(TVidEncParam* ptVideoEncParam);   
   
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
	BOOL32 GetStatus(TVidAdapterStatus &tVidAdapterStatus);

    /*************************************************************
    ������    : GetStatis
    ����      ��ȡͳ��
    ����˵��  ��tMulPicStatus        [IO] ״̬����
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
	BOOL32 GetStatis(TVidAdapterStatis &tVidAdapterStatis);    

    /*************************************************************
    ������    : ChangeBitRate
    ����      �����ñ��뷢�͵�����
    ����˵��  ��dwBitRate         [I] ����ֵ����λ (Kbit/s)
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
	BOOL32 ChangeEncBitRate(u32 dwBitRate);
    
    /*************************************************************
    ������    : SetFastUpdata
    ����      �����ñ�������ؼ�֡
    ����˵��  ����
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    **************************************************************/
    BOOL32 SetFastUpdata();   

    /*************************************************************
    ������    : GetVidSendStatis
    ����      ����ȡͨ������ͳ����Ϣ
    ����˵��  ��tKdvDecStatis     [IO] ͳ����Ϣ
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE  
    **************************************************************/
    BOOL32 GetVidSendStatis(TVidSendStatis &tVidSendStatis);

    /*************************************************************
    ������    : GetVidRecvStatis
    ����      ����ȡͨ������ͳ����Ϣ
    ����˵��  ��tKdvDecStatis     [IO] ͳ����Ϣ
    
    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE
    --------------------------------------------------------------
    �޸ļ�¼  ��
    ��  ��      �汾        �޸���      �߶���        �޸ļ�¼
    2009/03/10  V1.0        ��С��                      ����
    **************************************************************/
    BOOL32 GetVidRecvStatis(TVidRecvStatis &tKdvDecStatis);
 
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
    ������    : SetVideoActivePT
    ����      ������ ��̬��Ƶ�غɵ� Playloadֵ
    ����˵��  ��byRmtActivePT  [I] 
                byRealPT       [I] 

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
    BOOL32 SetVideoActivePT(u8 byRmtActivePT, u8 byRealPT);

    /*************************************************************
    ������    : SetDecryptKey
    ����      ������ ��̬��Ƶ�غɵ� Playloadֵ
    ����˵��  ��byRmtActivePT  [I] 
                byRealPT       [I] 

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
    BOOL32 SetDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode=DES_ENCRYPT_MODE);

    /*************************************************************
    ������    : SetDecryptKey
    ����      �����ü���key�ִ������������Ķ�̬�غ�PTֵ, pszKeyBuf����ΪNULL-��ʾ������
    ����˵��  ��byRmtActivePT  [I] 
                byRealPT       [I] 

    ����ֵ˵�����ɹ�����TRUE��ʧ�ܷ���FALSE    
    **************************************************************/
    BOOL32 SetEncryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode=DES_ENCRYPT_MODE);
    
    //////////////////////////////////////////////////////////////////////////
    //�����Ƿ��û��ӿ�

    void* m_pListImg;
    void* m_pListEnc;                   //���ձ����б�
    void* m_pListDec;                   //���뷢���б�       
    void* m_ptLock;                     //�߳���
    void* m_ptProcessLock;              //�߳���
    void* m_ptCond;           
    void* m_pListVidEncParam;

private:
    BOOL32 SetDecParam();    
    void ConvertEncParam(TVidEncParam* ptVideoEncParam, void* ptVoid);
    BOOL32 Quit();

    BOOL32 m_bActive;    
};

#endif //end of _CODECADAPTER_H_


