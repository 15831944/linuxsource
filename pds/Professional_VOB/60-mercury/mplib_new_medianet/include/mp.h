 /*****************************************************************************
   ģ����      : MP
   �ļ���      : mp.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: MP����Ϣ�ӿڶ���
   ����        : ������
   �汾        : V4.0  Copyright(C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��       �汾        �޸���          �޸�����
   2003/07/10   0.1         ������          ����
   2007/03/21   4.0         �ű���          �㲥�����߼������ӡ������Ż�
******************************************************************************/
#ifndef __DMP_H_
#define __DMP_H_

#include "osp.h"
#include "evmp.h"
#include "mcustruct.h"
#include "dataswitch.h"
#include "kdvmedianet.h"

//#ifdef WIN32
//#include "dataswitch36.h"
//#else
//#include "dataswitch.h"
//#endif

#define ENTER( hSem) \
        CSemOpt cSemOpt( hSem );

#define NEW_DS

// ��ӡ����
#define LOG_CRIT                        (u8)0
#define LOG_ERR                         (u8)1
#define LOG_WARN                        (u8)2
#define LOG_INFO                        (u8)3
#define LOG_DETAIL                      (u8)4
#define LOG_VERBOSE                     (u8)5

#define MAXNUM_HISPACKS                 (u8)100

// �ڲ��л���Դ �˿�״̬ ����
#define PS_IDLE                         (u8)0
#define PS_GETSRC                       (u8)1
#define PS_SWITCHED                     (u8)2
#define PS_UNEXIST                      (u8)0xff

#define POINT_NUM                       (u8)2           // �����ЧԴ�е���

#define INVALID_IPADDR                  0xffffffff
#define INVALID_SWITCH_CHANNEL          (u8)255         // ��Ч�Ľ������
#define NUM_IPADDR                      (u8)1           // ����dsʱIP��

#define MP_CONNECT_TIMEOUT              (u16)(2*1000)   // ���ӳ�ʱ��ʱ��
#define MP_REGISTER_TMEROUT             (u16)(4*1000)   // ע�ᳬʱ��ʱ��
#define MP_CHGSRCTOT_CHECK_TIMEOUT      (u16)100        // Դ�л���鶨ʱ��
#define MP_CHGSRCTOT_TIMEOUT            (u16)3000       // Դ�л����ȴ�ʱ��

#define WAITIFRAME_START_TIMER          (u16)1000       // �ȹؼ�֡��ʱ����ʼid,����osp�ڲ���Ϣid
#define DEFAULT_TIMESTAMP_INTERVAL      (u16)3600
#define SWITCH_SRC_SEQ_INTERVAL         (u8)6   
#define MAX_SEQNUM_INTERVAL             (s32)50
#define MIN_SEQNUM_INTERVAL             (s32)(-50)
#define RTP_FIXEDHEADER_SIZE            (s32)12
#define MAX_TIMESTAMP_INTERVAL          (s32)(MAX_SEQNUM_INTERVAL * DEFAULT_TIMESTAMP_INTERVAL)
#define MIN_TIMESTAMP_INTERVAL          (s32)(MIN_SEQNUM_INTERVAL * DEFAULT_TIMESTAMP_INTERVAL)


//mp�ڲ�net�ն˶˿���չ, ����mcu��ǰ��udp�˿�ʹ�ã���Ӧ�ܺ�400��������
#define RCV_PORT_OFFSET					(u16)10000
#define SND_PORT_OFFSET					(u16)20000


// ��������
#define SWITCH_NONE                     (u8)0
#define SWITCH_ONE2ONE                  (u8)1
#define SWITCH_MUL2ONE                  (u8)2
#define SWITCH_DUMP                     (u8)3

#define NETFRFCSTAT_TIMESPAN            (u32)5          // DataSwitch ����ͳ��ʱ����
#define NETFRFCSTAT_BUFFER              (u32)120        // �ܻ���ʱ��

/* ÿ������֧�ֵĹ㲥��������ʼ��ַ��ÿ������ռ��10�˿ڣ�
   +0�㲥�ڵ㣬+2����Դ�ڵ�һ��+4����Դ�ڵ�������� ����Դ�ڵ�һ�ͽ���Դ�ڵ������
   ���Ϲ����϶�û���Ⱥ�����ҵ�ǰ���еĿ��п��ö˿ڷ�ΧΪ2000����֧��200������ */
#define CONFBRD_STARTPORT               (u16)59000

//�ؼ�֡��ȡ ϵ�к궨��
#define     EX_HEADER_SIZE              4
#define     MIN_PACK_EX_LEN             4


#define MAX_SWITCH_PERPAGE              (u16)100   // ������ÿҳ��ӡ�ı���

// DataSwitch ����ͳ��ʱ����
#define NETFRFCSTAT_TIMESPAN            (u32)5
// �ܻ���ʱ��
#define NETFRFCSTAT_BUFFER              (u32)120

// ���ڹ�һ������
typedef struct
{
    s32  nSeqNumInterval;       //���Ͱ���ŵļ��
    s32  nTimeStampInterval;    //���Ͱ�ʱ�صļ��

    u16  wModifySeqNum;         //�޸ĵķ��Ͱ������
    u32  dwModifyTimeStamp;     //�޸ĵķ��Ͱ���ʱ��
    u32  dwModifySSRC;          //�޸ĵķ��Ͱ���SSRC

    u16  wLastSeqNum;           //���ķ��Ͱ������
    u32  dwLastTimeStamp;       //���ķ��Ͱ���ʱ��
    u32  dwLastSSRC;            //���ķ��Ͱ���SSRC

}TSendFilterParam;

typedef struct
{
    u8     byConfNo;            //�����
    u16    wChannelNo;          //�ŵ���
    u32    dwDstIp;
    u16    wDstPort;
    u32    dwAddTime;           //���ӽ�����ʱ��
    BOOL32 bRemoved;            //��Ӧ�Ľ����Ƿ�ɾ��
    BOOL32 bUniform;
    u8     byPayload;

    u16    wSeqNum;
    u32    dwTimeStamp;
    u32    dwSSRC;

    u16    wRcvPort;            /*���ֶ�������������¶�δ��ֵ��Ŀǰֻ��Խ���
                                  M->S1/S2->T�� ԭ�б�ʶ�޷����֣��ش˱�ʶ��*/
}TSendFilterAppData;

//�ȴ��ؼ�֡��Ϣ
typedef struct 
{
    u32 dwSrcIp;
    u16 wSrcPort;
    u8  byConfNo;               //0-index
    u8  byMtId;   
    BOOL32 bWaitingIFrame;      //�Ƿ���Ҫ�ȹؼ�֡
    u32  dwFstTimeStamp;

}TWaitIFrameData;


// ��������ͳ�ƶ���
class CNetTrfcStat
{
public:
    CNetTrfcStat();
    ~CNetTrfcStat();

public:
    // ׷������ֵ
    void AppendLatestStat( s64 lnRecv, s64 lnSend );

    // ��ӡ����ֵ
    void DumpLatestStat();
    void DumpAllStat();

    // ��������ͳ���ж��Ƿ񳬸���
    BOOL32 IsNetTrfcOverrun();
	
	// zgc [2007/01/12] ����ָ��ʱ����ͳ�������Ƿ񳬸���
	BOOL32 IsFluxOverrun(const u32 nTimeSpan);

    // ȡָ��ʱ����ڵ����縺��
    void  GetAvgNetBurden(u32 nTimeSpan, s32 &nSend, s32 &nRecv);
private:
    void Reset(BOOL32 bRecv = TRUE, BOOL32 bSend = TRUE);

private:
     // �����ͳ��ֵ
     s64 m_alnNetRecvBytes[NETFRFCSTAT_BUFFER/NETFRFCSTAT_TIMESPAN+1];
     s64 m_alnNetSendBytes[NETFRFCSTAT_BUFFER/NETFRFCSTAT_TIMESPAN+1];

public:
    s32  m_nMaxCap; // ��MP�����ת����������λKbps

};


class CMpInst: public CInstance
{
    enum 
    { 
        STATE_IDLE,
        STATE_NORMAL
    };

public:
    CMpInst();
    virtual ~CMpInst();
    
public:
    void InstanceEntry(CMessage * const pcMsg);							    
    void DaemonInstanceEntry(CMessage * const pcMsg, CApp* pcApp);
    
    void ProcPowerOn();
    void ProcConnectTimeOut(BOOL32 bIsConnectA);
    BOOL32 ConnectToMcu(BOOL32 bIsConnectA, u32& dwMcuNode);	//����Mcu
    void ProcRegisterTimeOut(BOOL32 bIsRegisterA);
    void RegisterToMcu(BOOL32 bIsRegisterA, u32 dwMcuNode);		//��Mcuע��
    void ProcDisconnect(CMessage * const pcMsg);				//��������
    void ProcMcuRegisterRsp(CMessage * const pcMsg);			//Mcuע���Ӧ����
	void ProcGetMsStatusRsp(CMessage* const pcMsg);             // ȡ��������״̬
    void ProcAddSwitchReq(CMessage * const pcMsg);				//����Mcu�����ӽ�������
    void ProcRemoveSwitchReq(CMessage * const pcMsg);			//����Mcu���Ƴ���������
    void ProcGetStatusReq(CMessage * const pcMsg);				//����Mcu�ĵõ�״̬����
	void ProcReleaseConfNotify(CMessage * const pcMsg);			//����Mcu�Ľ�������֪ͨ
	
    void ProcAddMulToOneSwitchReq(CMessage * const pcMsg);		//����Mcu�����Ӷ�㵽һ�㽻������
	void ProcRemoveMulToOneSwitchReq(CMessage * const pcMsg);	//����Mcu���Ƴ���㵽һ�㽻������
	void ProcStopMulToOneSwitchReq(CMessage * const pcMsg);		//����Mcu��ֹͣ��㵽һ�㽻������
	
    void ProcAddRecvOnlySwitchReq(CMessage * const pcMsg);		//����Mcu�����ֻ���ս���������
	void ProcRemoveRecvOnlySwitchReq(CMessage * const pcMsg);	//����Mcu���Ƴ�ֻ���ս���������

    void ProcAddBroadSwitchSrcReq(CMessage * const pcMsg);      // ����Mcu����ӹ㲥Դ������
    void ProcAddBroadSwitchDstReq(CMessage * const pcMsg);      // ����Mcu����ӹ㲥Ŀ�������
    void ProcRemoveBroadSwitchSrcReq(CMessage * const pcMsg);   // ����Mcu���Ƴ��㲥Դ������

    void ProcConfUniformModeNotify(CMessage * const pcMsg);
    void DaemonProcWaitingIFrameTimer(CMessage * const pcMsg);

    void ProcAddTable(CMessage* const pcMsg);
    void ProcNetStat(CMessage* const pcMsg);
	void ProcFluxOverRun(CMessage* const pcMsg);
	void ProcSendFluxOverrunNotify(CMessage* const pcMsg);
    void NotifyMcuFluxStatus( void );
    void RestrictFluxOfMp(CMessage* const pcMsg);
    void ProcChangeSrcTimeout( void );
    void ProcCallBackSwitchChgNtf(CMessage* const pcMsg);
    BOOL32 SwitchSrcToT( u8 byConfIdx, u8 byPoint, TSwitchChannel &tSrcSwitch, 
                         u8 bySwitchChnMode, u8 byUniformPayload, BOOL32 bSwitchNow = FALSE );

    void ClearInstStatus(void);
    void ShowSwitch(void);

protected:
    void SendMsgToMcu(u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0);
    void SendMsg2Mcu(u16 wEvent, CServMsg &cServMsg);
};

class CMpData
{
public:
    CMpData();
    virtual ~CMpData();

public:
    BOOL32 Init();
    BOOL32 CreateDS();
    void   DestroyDS();
    void   FreeStatusA(void);
    void   FreeStatusB(void);

    void   MpSendFilterParamInit( void );
    void   MpResetSendFilterParam( u8 byConfNo );
    BOOL32 GetWaitIFrameDataBySrc( u32 dwSrcIp, u16 wSrcPort, TWaitIFrameData *ptWaitData );

    BOOL32 IsPointT( u16 wPort );
    BOOL32 IsPointMt( u16 wPort );
    BOOL32 IsPointSrc( u16 wPort, u8 *pbyConfIdx = NULL, u8 *pbyPointId = NULL );
    BOOL32 IsPortSrcToT( u8 byConfIdx, u16 wRcvPort );
    
    void   SetChgDelay( u8 byConfIdx, u16 wDelayTick );
    u16    GetChgDelay( u8 byConfIdx );
    void   ClearChgDelay( u8 byConfIdx );

    void   SetConfUniMode( u8 byConfIdx, u8 byMode );
    u8     GetConfUniMode( u8 byConfIdx );

    BOOL32 IsPortNeedChkIFrm( u16 wRcvPort );
    BOOL32 IsTInUseStill( u8 byConfIdx );
    
    u8     GetPtIdle( u8 byConfIdx );
    u8     GetPtState( u8 byConfIdx, u8 byPoint, BOOL32 bAnother = FALSE );
    void   SetPtState( u8 byConfIdx, u8 byPoint, u8 byState, BOOL32 bAnother = FALSE );
    u16    GetChanNoByDst( u8 byConfIdx, u32 dwDstIp, u16 wDstPort, u8 byGetType, u16 wRcvPort = 0 );

    BOOL32 GetSwitchChan    ( u8 byConfIdx, u16 wIndex, TSwitchChannel *ptSwitchChannel );
    BOOL32 RemoveSwitchTable( u8 byConfIdx, TSwitchChannel *ptSwitchChannel, u8 byType, BOOL32 bStop = FALSE );
    BOOL32 RemoveSwitchTableSrc( u8 byConfIdx, TSwitchChannel *ptSwitchChannel );
    BOOL32 SearchSwitchTable( u8 byConfIdx, u32 dwDstIp, u16 wDstPort, TSwitchChannel &tSwitchChannel );
    BOOL32 IsSrcSwitchExist( u8 byConfIdx, TSwitchChannel *ptSrcSwitch );
    BOOL32 SearchBridgeSwitch( u8 byConfIdx, u16 wRcvDstPort, TSwitchChannel &tSwitchChannel );

    void   NtfMcuSrcSwitchChg( u8 byConfIdx, TSwitchChannel &tSwitchSrc );
    void   ProceedSrcChange( u8 byConfIdx, u8 byPID, BOOL32 bAnotherPt = TRUE );

    //void   ShowSwitch();
    void   ShowWholeSwitch();
    void   ShowSwitchUniformInfo();
    BOOL32 AddSwitchTable( u8 byConfIdx, TSwitchChannel* ptSwitchChannel, u8 byType );
    void   ClearSwitchTable( BOOL32 bClearAll, u8 byConfIdx = 0 );

    u8     GetMtNumNeedT( u8 byConfIdx );
    void   SetMtNumNeedT( u8 byConfIdx, u8 byMtNum );

    void   SendMsg2Mcu(u16 wEvent, CServMsg &cServMsg);
    
    void   ShowHandle( void );


public:
    s32  m_nInitUdpHandle;

    u32	 m_dwMcuNode;
    u32	 m_dwMcuIId;
    u32  m_dwMcuNodeB;
    u32  m_dwMcuIIdB;
    u32  m_dwMcuIpAddr;
    u16  m_wMcuPort;
    u32  m_dwMcuIpAddrB;
    u16  m_wMcuPortB;
    u16  m_wRecvMtDataPort;
    u16  m_wRecvMtDataPortB;

    BOOL32 m_bEmbedA;
    BOOL32 m_bEmbedB;

    u32  m_dwMpcSSrc;           // guzh [6/12/2007] ҵ���ỰУ��ֵ    

    u8   m_byAttachMode;        //MP�ĸ�����ʽ ( 1-MCU,2-������,3-����������,255-���� )
    u8   m_byMpId;              //��Mp���
    u8   m_byMcuId;             //Ҫ���ӵ�Mcu��ID
    u32  m_dwIpAddr;            //Ip��ַ����
	s8   m_abyMpAlias[16];      //Mp����

    u8   m_byRegAckNum;         // �Ƿ��յ�ע��Ack��Ϣ
    u8   m_byConnectNum;        // ���ӳɹ��Ĵ���    
    BOOL32 m_bIsRestrictFlux;   // �Ƿ�����ת������
    BOOL32 m_bIsSendOverrunNtf; // �Ƿ�������ת����������֪ͨ
    BOOL32 m_bIsWaitingIFrame;  // �Ƿ�ȴ��ؼ�֡

    BOOL32 m_bCancelWaitIFrameNoUni;

    //��һ����������Ϣ�����
    TSendFilterParam   (*m_pptFilterParam)[MAX_SWITCH_CHANNEL];
    TSendFilterAppData (*m_pptFilterData)[MAX_SWITCH_CHANNEL];
    TWaitIFrameData    (*m_pptWaitIFrmData)[MAXNUM_CONF_MT+POINT_NUM]; //193��194������ת�����Ĺ�һ����
    TSwitchChannel     (*m_pptSwitchChannel)[MAX_SWITCH_CHANNEL];

private:
    u8        m_abyConfMode[MAX_CONFIDX];       // �����һ����ģʽ
    u16       m_awChgDelayTick[MAX_CONFIDX];    // �л�ǰ�ȴ���ʱ��
    u8        m_byMtNumNeedT[MAX_CONFIDX];      // ��ҪT�ڵ���ն���
    u8        m_abyPtState[MAX_CONFIDX][POINT_NUM];  // ���������л���ת����Ϣ
    SEMHANDLE m_hPtInfo;        // ��ת�ڵ�״̬д���ź���
    SEMHANDLE m_hPtTick;        // �����л��ȴ���ʱʱ��д���ź���
    SEMHANDLE m_hMtNum;         // ת���ն���д���ź���
};

typedef zTemplate< CMpInst, 1, CMpData > CMpApp;
extern CMpApp g_cMpApp;


class CSemOpt{
public:
    CSemOpt( SEMHANDLE& hSem )
    {
        m_hSem = hSem;
        if( OspSemTakeByTime( m_hSem, WAIT_SEM_TIMEOUT ) != TRUE )
        {
            OspPrintf( TRUE, FALSE, "[Mp]: semTake error accord to handle<0x%x>\n", hSem );
        }
    }
    ~CSemOpt( )
    {
        OspSemGive( m_hSem );
    }
private:
    SEMHANDLE m_hSem;
};


/*****************************************************************************
   ����        : CMpBitStreamFilter
   ʵ�ֹ���    : �������˼��(�����࣬ͨ����̬�����ṩ�ӿ�)
   ����        : ����
   �汾        : V4.0  Copyright(C) 2001-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2007/04/20  4.0         ����        ����
******************************************************************************/

/*
// RTP ���ṹ����
typedef struct
{
    u8	 m_byMark;			//�Ƿ�֡�߽�1��ʾ���һ��
    u8   m_byExtence;		//�Ƿ�����չ��Ϣ
    u8   m_byPadNum;		//���ܵ�padding����
    u8   m_byPayload;		//�غ�
    u32  m_dwSSRC;			//ͬ��Դ
    u16  m_wSequence;		//���к�
    u32  m_dwTimeStamp;		//ʱ���
    u8  *m_pExData;         //��չ����
    s32  m_nExSize;			//��չ��С��sizeof(u32)�ı�����
    u8  *m_pRealData;		//ý������ 
    s32  m_nRealSize;		//���ݴ�С 
    s32  m_nPreBufSize;		//m_pRealDataǰԤ����Ŀռ�;
}TRtpPack;
*/

//H261ͷ�ṹ��Ϊ�˱�����⣬�ֶ����ͱ�׼һ��
typedef struct
{
    s32 sBit;
    s32 eBit;
    s32 i;
    s32 v;
    s32 gobN;
    s32 mbaP;
    s32 quant;
    s32 hMvd;
    s32 vMvd;
}TH261Header;

//H264ͷ�ṹ
typedef struct
{
    u16		 m_wWidth;			// ����֡�Ŀ��
    u16		 m_wHeight;			// ����֡�ĸ߶�
    BOOL32	 m_bIsKeyFrame;		// ����֡�Ƿ�Ϊ�ؼ�֡�� 1���ǹؼ�֡  0�����ǹؼ�֡
    u32		 m_dwSPSId;			// SPS��ID
    BOOL32	 m_bIsValidSPS;		// H264��SPS�Ƿ���Ч
    BOOL32	 m_bIsValidPPS;		// H264��PPS�Ƿ���Ч
} TKdvH264Header;

//h264 ������Ϣ����
typedef struct
{
    u8 *pu8Start;
    u8 *pu8P;
    u8 *pu8End;
    s32 s32Left;        // i_count number of available bits 
    
} TBitStream; 

typedef enum 
{
    P_SLICE = 0,
    B_SLICE,
    I_SLICE,
    SP_SLICE,
    SI_SLICE
} stdh264SliceType;


#define MAXnum_ref_frames_in_pic_order_cnt_cycle  255
#define MAXIMUMVALUEOFcpb_cnt                     32
#define MAXnum_slice_groups_minus1                8
#define MPEG4_FRAMEMODE_POS                       2

typedef struct tagSeqParameterSetRBSP
{
    BOOL32   bIsValid;												// indicates the parameter set is valid
    
    u32	profile_idc;										        // u(8)
    //#ifdef G50_SPS
    BOOL32		constrained_set0_flag;								// u(1)
    BOOL32		constrained_set1_flag;								// u(1)
    BOOL32		constrained_set2_flag;								// u(1)
    //#endif
    //uint_8  reserved_zero_5bits; /*equal to 0*/					// u(5)
    u32			level_idc;											// u(8)
    //#ifndef G50_SPS
    //	BOOL32		more_than_one_slice_group_allowed_flag;			// u(1)
    //	BOOL32		arbitrary_slice_order_allowed_flag;				// u(1)
    //	BOOL32		redundant_slices_allowed_flag;					// u(1)
    //#endif
    u32			seq_parameter_set_id;								// ue(v)
    u32			log2_max_frame_num_minus4;							// ue(v)
    u32			pic_order_cnt_type;									// ue(v)
    // if( pic_order_cnt_type == 0 ) 
    u32			log2_max_pic_order_cnt_lsb_minus4;					// ue(v)
    // else if( pic_order_cnt_type == 1 )
    BOOL32		delta_pic_order_always_zero_flag;					// u(1)
    s32			offset_for_non_ref_pic;								// se(v)
    s32			offset_for_top_to_bottom_field;						// se(v)
    u32			num_ref_frames_in_pic_order_cnt_cycle;				// ue(v)
    // for( i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
    s32			offset_for_ref_frame[MAXnum_ref_frames_in_pic_order_cnt_cycle];   // se(v)
    u32			num_ref_frames;										// ue(v)
    BOOL32		gaps_in_frame_num_value_allowed_flag;				// u(1)
    u32			pic_width_in_mbs_minus1;							// ue(v)
    u32			pic_height_in_map_units_minus1;						// ue(v)
    BOOL32		frame_mbs_only_flag;								// u(1)
    // if( !frame_mbs_only_flag ) 
    BOOL32		mb_adaptive_frame_field_flag;						// u(1)
    BOOL32		direct_8x8_inference_flag;							// u(1)
    //#ifdef G50_SPS
    BOOL32		frame_cropping_flag;								// u(1)
    u32			frame_cropping_rect_left_offset;					// ue(v)
    u32			frame_cropping_rect_right_offset;					// ue(v)
    u32			frame_cropping_rect_top_offset;						// ue(v)
    u32			frame_cropping_rect_bottom_offset;					// ue(v)
    //#endif
    BOOL32		vui_parameters_present_flag;						// u(1)
    //not of syntax
    s32			MaxFrameNum;
} TSeqParameterSetRBSP;


typedef struct  tagPicParameterSetRBSP
{
    BOOL32		bIsValid;											// indicates the parameter set is valid
    u32			pic_parameter_set_id;                               // ue(v)
    u32			seq_parameter_set_id;                               // ue(v)
    BOOL32		entropy_coding_mode_flag;                           // u(1)
    // if( pic_order_cnt_type < 2 )  in the sequence parameter set
    BOOL32      pic_order_present_flag;                             // u(1)
    u32			num_slice_groups_minus1;                            // ue(v)
    u32			slice_group_map_type;								// ue(v)
    // if( slice_group_map_type = = 0 )
    u32			run_length_minus1[MAXnum_slice_groups_minus1];  	// ue(v)
    // else if( slice_group_map_type = = 2 )
    u32			top_left[MAXnum_slice_groups_minus1];				// ue(v)
    u32			bottom_right[MAXnum_slice_groups_minus1];			// ue(v)
    // else if( slice_group_map_type = = 3 || 4 || 5
    BOOL32		slice_group_change_direction_flag;				    // u(1)
    u32			slice_group_change_rate_minus1;					    // ue(v)
    // else if( slice_group_map_type = = 6 )
    u32			num_slice_group_map_units_minus1;					// ue(v)
    u32			*slice_group_id;									// complete MBAmap u(v)
    u32			num_ref_idx_l0_active_minus1;                       // ue(v)
    u32			num_ref_idx_l1_active_minus1;                       // ue(v)
    BOOL32		weighted_pred_flag;                                 // u(1)
    BOOL32		weighted_bipred_idc;                                // u(2)
    s32			pic_init_qp_minus26;                                // se(v)
    s32			pic_init_qs_minus26;                                // se(v)
    s32			chroma_qp_index_offset;                             // se(v)
    BOOL32		deblocking_filter_control_present_flag;             // u(1)
    BOOL32		constrained_intra_pred_flag;                        // u(1)
    BOOL32		redundant_pic_cnt_present_flag;                     // u(1)
    BOOL32		vui_pic_parameters_flag;                            // u(1)
    //#ifndef G50_SPS
    //	BOOL32   frame_cropping_flag;                               // u(1)
    //    u32  frame_cropping_rect_left_offset;					    // ue(v)
    //    u32  frame_cropping_rect_right_offset;					// ue(v)
    //    u32  frame_cropping_rect_top_offset;					    // ue(v)
    //    u32  frame_cropping_rect_bottom_offset;				    // ue(v)
    //#endif
} TPicParameterSetRBSP;

typedef struct
{
    u16		first_mb_in_slice;						//ue(v)  slice�е�һ��MB�ĵ�ַ
    u16		slice_type;								//ue(v)  slice�ı�������
    u16		pic_parameters_set_id;	
} Tstdh264Dec_SliceHeaderData;

class CMpBitStreamFilter
{
public:
    // �ؼ�֡���
    static BOOL32 IsKeyFrame( u8 byPayLoad, u8 *pBuf, s32 nSize );

private:    
    // Toolkits and utilities    
    // u32������������ת��
    static void ConvertN2H(u8 *pBuf, s32 nStartIndex, s32 nSize);
    
    // u32��ȡָ��bits range
    static u32  GetBitField(u32 dwValue, s32 nStartBit, s32 nBits);

    static BOOL32 IsH261KeyFrame( u8 *pBuf, s32 nSize );
    static BOOL32 IsH263KeyFrame( u8 *pBuf, s32 nSize );
    static BOOL32 IsH264KeyFrame( u8 *pBuf, s32 nSize );
    static BOOL32 IsMpeg4KeyFrame( const TRtpPack *ptRtpPack );   // MPEG4 & H262
    
private:
    static void stdh264_bs_init( TBitStream *s, void *p_data, s32 i_data );
    static s32 stdh264_bs_pos( TBitStream *s );
    static s32 stdh264_bs_eof( TBitStream *s );
    static u32 stdh264_bs_read( TBitStream *s, s32 i_count );
    static u32 stdh264_bs_read1( TBitStream *s );
    static u32 stdh264_bs_show( TBitStream *s, s32 i_count );
    static void stdh264_bs_skip( TBitStream *s, s32 i_count );
    static s32 stdh264_bs_read_ue( TBitStream *s );
    static s32 stdh264_bs_read_se( TBitStream *s );
    static s32 stdh264_bs_read_te( TBitStream *s, s32 x );

    static s32 stdh264_FirstPartOfSliceHeader(TBitStream *s, Tstdh264Dec_SliceHeaderData *dec_slice_header);

    static BOOL32 DecodeH264SPS( TBitStream *s, TSeqParameterSetRBSP *sps, 
                                 TKdvH264Header *pStdH264Header );
    
    static BOOL32 DecodeH264PPS( TBitStream *s, TPicParameterSetRBSP *pps, 
							     TKdvH264Header *pStdH264Header );
};


//�Ƿ����� ȫ�ּ�¼��
struct TPackBuf
{
public:
    TPackBuf();
    void AddPack(u32 dwSSRC, u16 wSequence);
    BOOL IsPackExist(u32 dwSSRC, u16 wSequence);
    void Clear();
private:
    u32 m_dwSSRC[MAXNUM_HISPACKS];
    u16 m_wSequence[MAXNUM_HISPACKS];
};


class CPackDefer
{

};

#define	MAXNUM_NET_SW					(u8)10
#define	MAX_FRAME_SIZE					(u32)512 * 1024
#define NET_SW_NETBAND					(u32)8 * 1024 * 1024


//Medianet�շ��м���
class CNetSwitch
{
public:
	CKdvMediaRcv m_cMediaRcv;
	CKdvMediaSnd m_cMediaSnd;
	TSwitchChannel m_tSWChn;
	u8 m_bySndPT;

public:
	void Init(TSwitchChannel tSwitchChn, u8 bySndPT);

	void Quit();

	BOOL32 IsNull();

	void SetNull();
};

//���֧��ʮ���м��շ�
class CNetSWGrp
{
public:
	//CNetSWGrp::CNetSWGrp();

	u8 GetIdleNetSW();

	void InitNetSW(u8 byIdx, TSwitchChannel tSwitchChn, u8 bySndPT);

	void QuitNetSW(u8 byIdx);

	void QuitNetSW(TSwitchChannel tSwitchChn, u8 bySndPT);

public:
	CNetSwitch m_atNetSW[MAXNUM_NET_SW];

};



// ȫ�ֽӿ�
s8*   strofip(u32 dwIp);
void  strofip(u32 dwIp, s8* StrIp);
void  MpAPIEnableInLinux( void );
void  MpLog( u8 byLvl, s8 * pszFmt, ... );

void   MpSendFilterCallBackProc( u32 dwRecvIP, u16 wRecvPort,
                                 u32 dwSrcIP,  u16 wSrcPort,
                                 TNetSndMember * ptSends,      // ת��Ŀ���б�
                                 u16 * pwSendNum,              // ת��Ŀ�����
                                 u8  * pUdpData, u32 dwUdpLen );

#endif  // !__DMP_H_

// END OF FILE
