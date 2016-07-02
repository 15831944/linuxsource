/*****************************************************************************
ģ����      : ������뵥Ԫ
�ļ���      : hdu2inst.h
����ļ�    : 
�ļ�ʵ�ֹ���: hdu2ʵ��ͷ�ļ�
����        : ��־��
�汾        : 4.7  Copyright(C) 2011-2013 KDV, All rights reserved.
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
11/10/31    4.7         ��־��      ����
******************************************************************************/
#ifndef _HDU2_INST_H_
#define _HDU2_INST_H_
#include "osp.h"
#include "mcustruct.h"
#include "eqpcfg.h"
#include "mcuconst.h"
#include "kdvmedianet.h"
#include "./dm816x/codecwrapper_hd.h"
#include "./dm816x/codecwrapperdef_hd.h"
#include <fstream>
#include "hdu2agent.h"
#include "evmcueqp.h"
#include "mcuver.h"

#define MAX_VIDEO_FRAME_SIZE        (512 * 1024)      //�����Ƶ֡��С
#define MAX_AUDIO_FRAME_SIZE        (8 * 1024)        //�����Ƶ֡��С

#define HDU_CONNETC_TIMEOUT         (u16)3*1000       // ���ӳ�ʱֵ3s
#define HDU_REGISTER_TIMEOUT        (u16)5*1000       // ע�ᳬʱֵ5s
#define CHECK_IFRAME_INTERVAL       (u16)100          // ���ؼ�֡ʱ����(ms)

// Hdu2Print�ȼ�����
#define		HDU_LVL_ERROR			(u8)1			  // �������д���(�߼���ҵ�����),�������
#define		HDU_LVL_WARNING			(u8)2			  // �澯��Ϣ, ������ȷ, Ҳ���ܴ���
#define		HDU_LVL_KEYSTATUS		(u8)3			  // �������е�һ���ؼ�״̬ʱ����Ϣ���
#define		HDU_LVL_DETAIL			(u8)4			  // ��ͨ��Ϣ
#define		HDU_LVL_FRAME			(u8)5			  // �ؼ�֡�����Ϣ���
#define		HDU_LVL_GENERAL			(u8)6			  // һ����Ϣ����Ҫ���HDU2���յ�����Ϣ����Ϣ�ķ��ͣ�����߼���

class CHdu2ChnMgrGrp;

// ���ջص��ṹ  
class CRcvData
{
public:
    CHdu2ChnMgrGrp* m_pcHdu2ChnMgrGrp;  //�ص���Hdu2
    u32            m_byHduSubChnId;	    //�ص�����ͨ���� 
	CRcvData()
	{
		m_pcHdu2ChnMgrGrp = NULL;
		m_byHduSubChnId = 0;
	}
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

class CHdu2ChnMgrGrp
{
private:
	CKdvMediaRcv     	m_acVidMediaRcv[HDU_MODEFOUR_MAX_SUBCHNNUM];        //һ����Ƶ����
	CKdvMediaRcv     	m_cAudMediaRcv;        								//һ����Ƶ����(HDUVMP��ʱ��֧����Ƶ��δ��չ)
	CKdvVidDec    	    m_cVidDecoder;										//��Ƶ������
    CKdvAudDec          m_cAudDecoder;										//��Ƶ������
	u8					m_abyMode[HDU_MODEFOUR_MAX_SUBCHNNUM];				//��¼��ǰ������ͨ������ģʽ
	TMediaEncrypt       m_tMediaDec;										//���ܲ���
	TDoublePayload      m_atVidDp[HDU_MODEFOUR_MAX_SUBCHNNUM];              //��Ƶ��̬�غ�
	TDoublePayload      m_tAudDp;											//��Ƶ��̬�غ�
	u8                  m_tIdleChnBackGround;								//����ͨ����ʾ����(HDUVMPʹ��HDUͨ�����ԣ�δ��չ)
	TLocalNetParam      m_atVidLocalNetParam[HDU_MODEFOUR_MAX_SUBCHNNUM];    //��Ƶ������ض˿���Ϣ
	TLocalNetParam      m_tAudLocalNetParam;    //��Ƶ������ض˿���Ϣ
	BOOL32              m_bIsNeedRs;			//�Ƿ��������綪���ش�����(HDUVMPʹ��HDUͨ�����ԣ�δ��չ)
	BOOL32              m_bIsMute;              //�Ƿ���(HDUVMPʹ��HDUͨ�����ԣ�δ��չ)
	u8                  m_byVolume;             //����(HDUVMPʹ��HDUͨ�����ԣ�δ��չ)
	u8                  m_byAudChnNum;          //��������(HDUVMPʹ��HDUͨ�����ԣ�δ��չ)
//	u8					m_byOneChnlDecCount;								// ͳ��HDUͨ����������ʹ�ô���
	u8					m_abyHduSubChnId[HDU_MODEFOUR_MAX_SUBCHNNUM];		// Hdu�໭����ͨ��
	CRcvData			m_acRcvCB[HDU_MODEFOUR_MAX_SUBCHNNUM];				//���ջص�
public:
	CHdu2ChnMgrGrp()
	{
		memset(m_abyMode, 0, sizeof(m_abyMode));
		m_tIdleChnBackGround = 0;
		m_bIsNeedRs = FALSE;
		m_byVolume = HDU_VOLUME_DEFAULT;
	}
	~CHdu2ChnMgrGrp()
	{
	}
	void      SetMode( u8 byMode, u8 bySubChnId = 0 ){ m_abyMode[bySubChnId] = byMode; }
	u8        GetMode(u8 bySubChnId = 0){ return m_abyMode[bySubChnId]; }
	void      SetVidDoublePayload(TDoublePayload tVidDp, u8 byHduSubChnId)
	{
		memcpy( &m_atVidDp[byHduSubChnId], &tVidDp, sizeof(TDoublePayload) );
	}
	TDoublePayload   GetVidDoublePayload( u8 byHduSubChnId )
	{
		return m_atVidDp[byHduSubChnId];
	}
	void      SetAudDoublePayload(TDoublePayload tAudDp)
	{
		memcpy( &m_tAudDp, &tAudDp, sizeof(TDoublePayload) );
	}
	TDoublePayload      GetAudDoublePayload(void)
	{
		return m_tAudDp;
	}
	BOOL32    GetMediaEncrypt(TMediaEncrypt& tMediaEncrypt)
	{
        memcpy( &tMediaEncrypt, &m_tMediaDec, sizeof(TMediaEncrypt) );
		return TRUE;
	}
	void      SetMediaEncrypt(TMediaEncrypt *ptMediaEncrypt)
	{
		memcpy( &m_tMediaDec, ptMediaEncrypt, sizeof(TMediaEncrypt) );
		return;
	}
	void      SetAudChnNum( u8	byAudChnNum ){ m_byAudChnNum = byAudChnNum; }
	u8        GetAudChnNum(void){ return m_byAudChnNum; }
	//����
    BOOL32 Create(u16 wChnId,BOOL32 bIsHdu2_s);

	//��ʼ����
	BOOL32 StartDecode(u8 byMode = MODE_BOTH);

	//ֹͣ����
    BOOL32 StopDecode(u8 byMode = MODE_BOTH );
	
	//��ʼ����
	BOOL32 StartNetRecv( u8 byMode = MODE_BOTH, u8 byHduSubChnId = 0);
	
	//ֹͣ����
	BOOL32 StopNetRecv(u8 byMode = MODE_BOTH, u8 byHduSubChnId = 0);

	//������Ƶ���յ�ַ����(���еײ��׽��ӵĴ������󶨶˿ڵȶ���)
	BOOL32  SetVidLocalNetParam(TLocalNetParam *ptVidLocalNetParam = NULL, u8 byHduSubChnId = 0);

	//������Ƶ���յ�ַ����(���еײ��׽��ӵĴ������󶨶˿ڵȶ���)
	BOOL32  SetAudLocalNetParam(TLocalNetParam *ptAudLocalNetParam = NULL);

	//������������ش�����
	BOOL32 SetNetRecvRsParam(u8 byMode, BOOL32 bRepeatSnd, u8 byHduSubChnId = 0);

	//���ö�̬�غ�ֵ
	BOOL32 SetActivePT(u8 byMode , BOOL32 bIsVidAutoAjust = TRUE, u8 byHduSubChnId = 0);
	
	//���ý��ܲ���
	BOOL32 SetDecryptKey(u8 byMode = MODE_BOTH, u8 byHduSubChnId = 0);

	//������Ƶ���Ŷ˿����� VGA or DVI
	BOOL32 SetVidPlyPortType(u32 dwType);
	
	//������Ƶ����ź���ʽ��NULL��ʾ�Զ�����
    BOOL32 SetVideoPlyInfo(TVidSrcInfo* ptInfo);
	
	//�������ű���
	BOOL32 SetPlayScales( u16 wWidth, u16 wHeigh );

	//������ʾ����
	BOOL32  SetVidDecResizeMode(u16 nMode);

	//������Ƶ���л�
	BOOL32 SetVidPIPParam(const u8 byHduVmpMode);

	//Ϊ��������������
	BOOL32 SetData(u8 byMode ,TFrameHeader tFrameHdr, u8 byHduSubChnId = 0);

	//��ȡ��Ƶ������״̬
	BOOL32 GetDecoderStatus(u8 byMode ,TKdvDecStatus &tKdvDecStatus, u8 byHduSubChnId);

    //��ȡ��Ƶ��������ͳ����Ϣ
	BOOL32 GetDecoderStatis(u8 byMode ,TKdvDecStatis &tKdvDecStatis, u8 byHduSubChnId);	

    //������Ƶ���Ŷ˿����� HDMI or C
    BOOL32 SetAudOutPort(u32 dwAudPort);
	
	//���������������
	BOOL32  SetVolume(u8 byVolume = HDU_VOLUME_DEFAULT); 
	
	//�õ������������
    u8  GetVolume();
	
	//�����Ƿ���
    BOOL32  SetIsMute(BOOL32 bMute);

	//��ȡ�Ƿ���
    BOOL32  GetIsMute();

	//���ÿ���ͨ��������ʾ����
	BOOL32 SetNoStreamBak(u8 byShowMode = VMP_SHOW_BLACK_MODE);//Ĭ����ʾ����

	// ����HDU����ӿں������ʽ�õ���ƵԴ��Ϣ
	BOOL32 GetVidSrcInfoByTypeAndMode(u8 byOutPortType, u8 byOutModeType, TVidSrcInfo *ptVidSrcInfo,BOOL32 bIsHdu2);

	//��ʾͨ����Ϣ,����·���붼��ӡ����
	void ShowInfo(u8 byChnId);

	//������Ƶ����ӿ�����
	BOOL32 SetVidPlayPolicy(u8 byShowMode);
	// [2013/03/11 chenbing] 
	void SetHduSubChnId( u8 byHduSubChnId )
	{
		m_abyHduSubChnId[byHduSubChnId] = byHduSubChnId;
	}
	
	u8 GetHduSubChnId( u8 byHduSubChnId )
	{
		return m_abyHduSubChnId[byHduSubChnId];
	}
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

class CHdu2Instance: public CInstance
{
private:
	CHdu2ChnMgrGrp     m_cHdu2ChnMgrGrp;                                 //���Ͻ�����ָ��
	CConfId            m_cChnConfId;
	enum EHDU2CHNSTATE
	{
		emIDLE,
		emINIT,
		emREADY,
		emRUNNING,
	};
public:
	CHdu2Instance();
	virtual ~CHdu2Instance();
private:

	/* --------- ����ʵ����Ϣ������ -------------*/
	void  DaemonInstanceEntry( CMessage* const pMsg, CApp* pcApp );     //����ʵ����Ϣ�����
	void  DaemonProcPowerOn( CMessage* const pMsg);       //�ϵ��ʼ������
	void  DaemonProcConnectTimeOut( BOOL32 bIsConnectA );               //��MCU������Ϣ����
	void  DaemonProcOspDisconnect( CMessage* const pMsg); //����������
	void  DaemonProcRegisterTimeOut( BOOL32 bIsRegiterA );              //��MCUע����Ϣ����    
	void  DaemonProcMcuRegAck(CMessage* const pMsg);      //����ע��ACK��Ӧ
    void  DaemonProcMcuRegNack(CMessage* const pMsg);                   //����ע��NACK��Ӧ
	void  DaemonProcHduStartPlay( CMessage* const pMsg );               //��ʼ����
    void  DaemonProcHduStopPlay( CMessage* const pMsg );                //ֹͣ����
	// [2013/03/11 chenbing]  
	void  DaemonProcHduChgVmpMode( CMessage* const pMsg );				//HDUͨ��ģʽ�л�
	void  DaemonProcHduSetAudioPayLoad(CMessage* const pMsg);			//������Ƶ�غ�
	void  DaemonProcModePortChangeNotif( CMessage* const pMsg );        //��ʽ�ı�֪ͨ
	void  DeamonProcGetMsStatusRsp( CMessage* const pMsg );             //ȡ��������״̬
    void  DaemonProcHduChangeVolumeReq( CMessage* const pMsg );         //��������
	void  DaemonProcChangeModeCmd( CMessage* const pMsg );              //�ı�ģʽ
	void  DaemonProcChangePlayPolicy( CMessage* const pMsg );           //ͨ������ʵ�ֲ��Ըı�֪ͨ

	/* --------- ��ͨʵ����Ϣ������ -------------*/
	void  InstanceEntry( CMessage* const pMsg );                        //��ͨʵ����Ϣ�����
	void  ProcHduChnGrpCreate(void);                                    //������ͨ����������Ϣ
	void  ProcInitChnCfg(void);										    //��ʼ��hduͨ������
	void  ProcStartPlayReq( CServMsg &cServMsg );  						//��ʼ����
	void  ProcChangeHduVmpMode( CServMsg &cServMsg );					//HDUͨ��ģʽ�л�
	void  ProcHduSetAudioPayLoad(CServMsg &cServMsg );					//������Ƶ�غ�
	void  ProcStopPlayReq( CServMsg &cServMsg );   		                //ֹͣ����
	void  ProcTimerNeedIFrame(void);									//Hdu����Mcu���ؼ�֡
	void  ProcChangeChnCfg( CMessage* const pMsg );                     //�޸�ͨ������
    void  ProcHduChangeVolumeReq( CServMsg &cServMsg );                 //������������
	void  ProcChangeModeCmd( );											//���ģʽ
	void  ProcChangePlayPolicy( CMessage* const pMsg );                 //ͨ������ʵ�ֲ��Ըı�֪ͨ
	void  ProcDisConnect(void);											//HDU��������
	void  ProcShowMode(void);											//��Ѱ����ͨ��ģʽ
	void  ProcClearMode(void);											//���ͨ��ģʽ

	/* ---------------- ���ܺ��� ------------------*/
	void   SendMsgToMcu( u16 wEvent, CServMsg& cServMsg );              //��MCU������Ϣͨ�ú���
	void   SendChnNotify( BOOL32 bFirstNotif = FALSE, u8 byHduSubChnId = 0 );//ͨ��״̬֪ͨ 
	void   StatusShow( void );                                          //״̬��ʾ����ʾ���Ͻ�����״̬
	void   StatusNotify(void);                                          //HDU״̬֪ͨ������ʵ���е���
	BOOL32 Hdu2ChnStartPlay( u8 byMode = MODE_BOTH, u8 byHduSubChnId = 0);			//��ʼͨ������
	BOOL32 Hdu2ChnStopPlay( u8 byMode = MODE_BOTH, u8 byHduSubChnId = 0);	//ֹͣͨ������
	BOOL32 ChangeHduVmpMode(const u8 byHduChnId, const u8 byHduVmpMode);		//�л����

	u8 GetOldVersionEqpTypeBySubType ( u8 byHduEqpSubType ); //���¶����HDU2SUBtype�л�ȡ�ϰ汾��4.7.1������ǰ�汾��EQPTYPE��ʾ
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;


class CHdu2Data
{
public:
	CHdu2Data();
	virtual ~CHdu2Data();
public:
	void SetNull(void)
	{
		memset(this, 0, sizeof(CHdu2Data));
		// [2013/03/12 chenbing] 
		memset(m_abyHduChnMode, HDUCHN_MODE_ONE, sizeof(m_abyHduChnMode));
	}
    void FreeStatusDataA(void)
	{
		m_dwMcuNode = INVALID_NODE;
		m_dwMcuIId  = INVALID_INS;
	}
    void FreeStatusDataB(void)
	{
		m_dwMcuNodeB = INVALID_NODE;
		m_dwMcuIIdB  = INVALID_INS;
	}
	void SetMcuNode(u32 NodeId){	m_dwMcuNode = NodeId;	};
	u32  GetMcuNode(){	return m_dwMcuNode;	};
	void SetMcuNodeB(u32 NodeId){	m_dwMcuNodeB = NodeId;	};
	u32  GetMcuNodeB(){	return m_dwMcuNodeB;	};
	void SetMcuIId(u32 InstId){	m_dwMcuIId = InstId;	};
	u32  GetMcuIId(){	return m_dwMcuIId;	};
	void SetMcuIIdB(u32 InstId){	m_dwMcuIIdB = InstId;	};
	u32  GetMcuIIdB(){	return m_dwMcuIIdB;	};
	void SetMcuSSrc(u32 dwSSrc){	m_dwMcuSSrc = dwSSrc;	};
	u32  GetMcuSSrc(){	return m_dwMcuSSrc;	};
	void SetHduCfg(THduCfg tHduCfg){	m_tHduCfg = tHduCfg;	};
	THduCfg  GetHduCfg(){	return m_tHduCfg;	};
	void SetPrsTimeSpan(TPrsTimeSpan tPrsTimeSpan){	m_tPrsTimeSpan = tPrsTimeSpan;	};
	TPrsTimeSpan  GetPrsTimeSpan(){	return m_tPrsTimeSpan;	};

	// [2013/03/11 chenbing] 
	void SetHduChnMode(u8 byHduChnId, u8 byHduChnMode, BOOL32 bIsOtherChnVmpMode = FALSE)
	{
		if (bIsOtherChnVmpMode)
		{
			for (u8 byIndex=0; byIndex<MAXNUM_HDU_CHANNEL; byIndex++)
			{
				if ( byIndex != byHduChnId )
				{
					m_abyHduChnMode[byIndex] = byHduChnMode;
				}
			}
		}
		else
		{
			m_abyHduChnMode[byHduChnId] = byHduChnMode;
		}
	}
	
	u8 GetHduChnMode(u8 byHduChnId, BOOL32 bIsOtherChnVmpMode = FALSE)
	{
		if (bIsOtherChnVmpMode)
		{
			for (u8 byIndex=0; byIndex<MAXNUM_HDU_CHANNEL; byIndex++)
			{
				if (   byIndex != byHduChnId
					&& HDUCHN_MODE_FOUR == m_abyHduChnMode[byIndex]
					)
				{
					return m_abyHduChnMode[byIndex];
				}
				
			}
			return HDUCHN_MODE_ONE;
		}
		else
		{
			return m_abyHduChnMode[byHduChnId];
		}
	}

	
	// [2013/03/12 chenbing] �жϷ���Ƿ�Ϸ�
	BOOL32 IsValidHduVmpMode(u8 byHduVmpMode)
	{
		if (byHduVmpMode == HDUCHN_MODE_ONE)
		{
			return TRUE;
		}
		else if (byHduVmpMode == HDUCHN_MODE_FOUR)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

private:
	u32              	m_dwMcuNode;          	        // ��MCU.Aͨ�Žڵ��
	u32             	m_dwMcuNodeB;         	        // ��MCU.Bͨ�Žڵ��
	u32              	m_dwMcuIId;           	        // ͨ��MCU.A��ʾ
	u32              	m_dwMcuIIdB;           	        // ͨ��MCU.B��ʾ
	u32              	m_dwMcuSSrc;           	        // ҵ���ỰУ��ֵ
	THduCfg         	m_tHduCfg;                      // HDU2����
	TPrsTimeSpan     	m_tPrsTimeSpan;                 // �ش�ʱ����

	// [2013/03/11 chenbing]  
	u8					m_abyHduChnMode[MAXNUM_HDU_CHANNEL];	//�������ֶΡ���¼HDUͨ��ģʽ
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

void Hdu2Print( const u8 byLevel, const s8* pszFmt, ...);
void hdu2log( const u8 byLevel, const u16 wModule, const s8* pszFmt, ...);
u32 GetVidPlyPortTypeByChnAndType(u16 wChnIdx,u32 dwType);
BOOL32 CheckChnCfgParam(THduModePort &tChnCfg,BOOL32 bIsHdu2_l);
s8 *IpToStr( u32 dwIP );
typedef zTemplate< CHdu2Instance, MAXNUM_HDU_CHANNEL, CHdu2Data > CHdu2App;
extern CHdu2App g_cHdu2App;
#endif //end _HDU2_INST_H_


