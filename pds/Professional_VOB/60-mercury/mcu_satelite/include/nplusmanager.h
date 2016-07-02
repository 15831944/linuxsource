/*****************************************************************************
   ģ����      : N+1���ݹ���ģ��
   �ļ���      : nplusmanager.h
   ����ļ�    : nplusmanager.cpp
   �ļ�ʵ�ֹ���: N+1���ݹ���
   ����        : ������
   �汾        : V4.0  Copyright(C) 2006-2009 KDCOM, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2006/11/21  4.0         ������      ����
******************************************************************************/
#ifndef _NPLUS_MANAGER_H
#define _NPLUS_MANAGER_H

#include "radiusinterface.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef PACKED
#undef PACKED
#endif

#ifdef WIN32
    #pragma comment( lib, "ws2_32.lib" ) 
    #pragma pack( push )
    #pragma pack( 1 )
    #define window( x )	x
    #define PACKED 
#else
    #include <netinet/in.h>
    #define window( x )
#if defined(__ETI_linux__)
    #define PACKED
#else
    #define PACKED __attribute__((__packed__))	// ȡ�����������Ż�����
#endif
#endif

#define NPLUS_CONNECTMCU_TIMEOUT                3000
#define NPLUS_REGMCU_TIMEOUT                    3000
#define NPLUS_DATAUPDATE_TIMEOUT                3000

#define NPLUS_CONF_START                        1       //���鿪ʼ
#define NPLUS_CONF_RELEASE                      2       //��ʼ

#define NPLUS_MAXNUM_REGNACKBYREMOTECAP         20      //20�Σ�ÿ��ʱ�� NPLUS_REGMCU_TIMEOUT
    
// N+1 ����״̬��
enum ENPlusState
{
    MCU_NPLUS_IDLE,             //û������N+1ģʽ
    MCU_NPLUS_MASTER_IDLE,      //N+1ģʽ��mcu״̬
    MCU_NPLUS_MASTER_CONNECTED, //N+1ģʽ��mcu�������ӱ��ݷ�����״̬
    MCU_NPLUS_SLAVE_IDLE,       //N+1ģʽ����mcu״̬
    MCU_NPLUS_SLAVE_SWITCH      //N+1ģʽ����mcu�л���״̬
};

// �ն˵�ַ��Ϣ
struct TMtInfo
{
public:
    TMtInfo( void ) { memset(this, 0, sizeof(TMtInfo)); }

    void            SetMtAddr( TTransportAddr &tAddr ) { m_dwMtAddr = tAddr; }
    TTransportAddr  GetMtAddr( void ) { return m_dwMtAddr; }
    void            SetCallBitrate( u16 wBitrate ) { m_wCallBitrate = htons(wBitrate); }
    u16             GetCallBitrate( void ) { return ntohs(m_wCallBitrate); }
    BOOL32          IsNull( void ) { return (0 == m_dwMtAddr.GetIpAddr()); }
    BOOL32          operator == (TMtInfo & tMtInfo) const
    {
        TTransportAddr tAddr = tMtInfo.GetMtAddr();
        if (memcmp(&tAddr, &m_dwMtAddr, sizeof(TTransportAddr)) == 0)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

private:
    TTransportAddr      m_dwMtAddr;
    u16                 m_wCallBitrate;

}PACKED;

//vmp info
struct TNPlusVmpInfo
{
public:
    TMtInfo     m_atMtInVmp[MAXNUM_MPUSVMP_MEMBER];    
    u8          m_abyMemberType[MAXNUM_MPUSVMP_MEMBER]; 

public:
    TNPlusVmpInfo( void ) { memset(this, 0, sizeof(TNPlusVmpInfo)); }

    u8      GetVmpMemType( TMtInfo tMtInfo, u8 byChnl ) 
    {
        if (byChnl >= MAXNUM_MPUSVMP_MEMBER)	// xliang [2/23/2009] �з���
        {
            return VMP_MEMBERTYPE_MCSSPEC;
        }

        if (m_atMtInVmp[byChnl] == tMtInfo)
        {
            return m_abyMemberType[byChnl];
        }        
        return VMP_MEMBERTYPE_MCSSPEC; //default
    }

    BOOL32  IsMtInVmpMem( TMtInfo tMtInfo, u8 byChnl )
    {
        if (byChnl >= MAXNUM_MPUSVMP_MEMBER)
        {
            return FALSE;
        }

        if (m_atMtInVmp[byChnl] == tMtInfo)
        {
            return TRUE;
        }
        return FALSE;
    }

}PACKED;

// ���ݵĻ�����Ϣ
struct TNPlusConfData
{
public:
    TNPlusConfData( void ) { Clear(); }
    BOOL32    IsNull( void ) { return m_tConf.GetConfId().IsNull(); }
    void      Clear( void ) { memset(this, 0, sizeof(TNPlusConfData)); }

public:
    TConfInfo       m_tConf;
    TMtInfo         m_atMtInConf[MAXNUM_CONF_MT];   
    u8              m_byMtNum;
    TNPlusVmpInfo   m_tVmpInfo;
    TAcctSessionId  m_tSsnId;
    
}PACKED;

struct TNPlusEqpCap //len:26
{
public:
    u8      m_byMtAdpNum;
    u8      m_byMpNum;
    u8      m_byVmpNum;
    u8      m_byBasAudChnlNum;    
    u8      m_byBasVidChnlNum;    
    u8      m_byPrsChnlNum;
    u8      m_byGKCharge;
    u8      m_byDCSNum;
    u8      m_abyVMPChnNum[MAXNUM_PERIEQP];
    u8      m_byMAUNum;
    u8      m_byMAUH263pNum;
	u8      m_byHduNum;
    u8      m_byMpuBasNum;
    
public:
    TNPlusEqpCap( void ) { SetNull(); }    
    
    void    SetNull( void )
    {
        m_byMtAdpNum = 0;
        m_byMpNum = 0;
        m_byVmpNum = 0;
        m_byBasAudChnlNum = 0;  
        m_byBasVidChnlNum = 0;
        m_byPrsChnlNum = 0;
        m_byGKCharge = 0;
        m_byDCSNum = 0;
        memset( m_abyVMPChnNum, 0, sizeof(m_abyVMPChnNum) );
        m_byMAUNum = 0;
        m_byMAUH263pNum = 0;
        m_byMpuBasNum = 0;
		m_byHduNum = 0;
    }

    // guzh [12/13/2006] С��ֻ�Ƚ���������(��DCS)��GK�Ʒ������������ת��������
    BOOL32 operator < (const TNPlusEqpCap &tObj) const
    {
        if ( m_byVmpNum < tObj.m_byVmpNum ||
             m_byBasAudChnlNum < tObj.m_byBasAudChnlNum ||
             m_byBasVidChnlNum < tObj.m_byBasVidChnlNum ||
             m_byPrsChnlNum < tObj.m_byPrsChnlNum ||
             m_byGKCharge < tObj.m_byGKCharge ||
             m_byDCSNum < tObj.m_byDCSNum ||
             m_byMAUNum < tObj.m_byMAUNum ||
             m_byMAUH263pNum < tObj.m_byMAUH263pNum ||
			 m_byHduNum < tObj.m_byHduNum ||
             m_byMpNum < tObj.m_byMpuBasNum ||
             IsLocalVMPCapInferior( tObj ) )
        {
            OspPrintf( TRUE, FALSE, "operator < (const TNPlusEqpCap &tObj) const = TRUE due to:\n" );
            OspPrintf( TRUE, FALSE, "\tVMP<%d vs %d>\n", m_byVmpNum, tObj.m_byVmpNum );
            OspPrintf( TRUE, FALSE, "\tBAC<%d vs %d>\n", m_byBasAudChnlNum, tObj.m_byBasAudChnlNum );
            OspPrintf( TRUE, FALSE, "\tBVC<%d vs %d>\n", m_byBasVidChnlNum, tObj.m_byBasVidChnlNum );
            OspPrintf( TRUE, FALSE, "\tPRS<%d vs %d>\n", m_byPrsChnlNum, tObj.m_byPrsChnlNum );
            OspPrintf( TRUE, FALSE, "\tGKC<%d vs %d>\n", m_byGKCharge, tObj.m_byGKCharge );
            OspPrintf( TRUE, FALSE, "\tDCS<%d vs %d>\n", m_byDCSNum, tObj.m_byDCSNum );
            OspPrintf( TRUE, FALSE, "\tMAU<%d vs %d>\n", m_byMAUNum, tObj.m_byMAUNum );
            OspPrintf( TRUE, FALSE, "\tMAU(h263p)<%d vs %d>\n", m_byMAUH263pNum, tObj.m_byMAUH263pNum );
            OspPrintf( TRUE, FALSE, "\tMPU<%d vs %d>\n", m_byMpuBasNum, tObj.m_byMpuBasNum );
            OspPrintf( TRUE, FALSE, "\tHDU<%d vs %d>\n", m_byHduNum, tObj.m_byHduNum );

            return TRUE;
        }
        return FALSE;
    }

    // guzh [12/13/2006]  +����ֻ������������(��DCS)��GK�Ʒ������������ת��������
    TNPlusEqpCap operator + (const TNPlusEqpCap &tObj) const
    {
        TNPlusEqpCap tCap;
        tCap.m_byBasAudChnlNum = tObj.m_byBasAudChnlNum + m_byBasAudChnlNum;
        tCap.m_byBasVidChnlNum = tObj.m_byBasVidChnlNum + m_byBasVidChnlNum;
        tCap.m_byPrsChnlNum = tObj.m_byPrsChnlNum + m_byPrsChnlNum;
        tCap.m_byVmpNum = tObj.m_byVmpNum + m_byVmpNum;
        tCap.m_byHduNum = tObj.m_byHduNum + m_byHduNum;


        if ( tCap.m_byGKCharge == 0 )
        {
            tCap.m_byGKCharge = tObj.m_byGKCharge + m_byGKCharge;
        }
        tCap.m_byDCSNum = tObj.m_byDCSNum + m_byDCSNum;

        AdjustChnNumOrder( (u8*)&tCap.m_abyVMPChnNum, tCap.m_byVmpNum );
        AdjustChnNumOrder( (u8*)&tObj.m_abyVMPChnNum, tObj.m_byVmpNum );

        u8 byVMPNum = m_byVmpNum > tObj.m_byVmpNum ? m_byVmpNum : tObj.m_byVmpNum;
        for ( u8 byIdx = 0; byIdx < byVMPNum; byIdx ++ )
        {
            if ( tObj.m_abyVMPChnNum[byVMPNum] > tCap.m_abyVMPChnNum[byVMPNum] )
            {
                tCap.m_abyVMPChnNum[byVMPNum] = tObj.m_abyVMPChnNum[byVMPNum];
            }
        }
        tCap.m_byMAUNum = tObj.m_byMAUNum + m_byMAUNum;
        tCap.m_byMAUH263pNum = tObj.m_byMAUH263pNum + m_byMAUH263pNum;
        tCap.m_byMpuBasNum = tObj.m_byMpuBasNum + m_byMpuBasNum;
        
        return tCap;
    }

    //VMP�������嵽MAP�� [12/28/2006-zbq]
    BOOL32 IsLocalVMPCapInferior( const TNPlusEqpCap &tObj ) const
    {
        AdjustChnNumOrder( (u8*)&m_abyVMPChnNum, m_byVmpNum );
        AdjustChnNumOrder( (u8*)&tObj.m_abyVMPChnNum, tObj.m_byVmpNum );

        u8 byVMPNum = m_byVmpNum < tObj.m_byVmpNum ? m_byVmpNum : tObj.m_byVmpNum;

        if ( byVMPNum > MAXNUM_PERIEQP )
        {
            OspPrintf( TRUE, FALSE, "byVMPNum > MAXNUM_PERIEQP !\n" );
            return TRUE;
        }
        for ( u8 byIdx = 0; byIdx < byVMPNum; byIdx ++ )
        {
            if ( m_abyVMPChnNum[byIdx] < tObj.m_abyVMPChnNum[byIdx] )
            {
                OspPrintf( TRUE, FALSE, "m_abyVMPChnNum[%d].%d < tObj.m_abyVMPChnNum[%d].%d !\n",
                                         byIdx, m_abyVMPChnNum[byIdx], byIdx,  tObj.m_abyVMPChnNum[byIdx] );
                return TRUE;
            }
        }
        return FALSE;
    }

    //��ͨ������ð������ٱȽϣ���ֹ �������ʽ ��� [12/28/2006-zbq]
    void AdjustChnNumOrder( u8 *pbyChn, u8 byNum ) const
    {
        if ( NULL == pbyChn || byNum > MAXNUM_PERIEQP )
        {
            return;
        }
        u8 abyChn[MAXNUM_PERIEQP] = { 0 };
        memcpy( abyChn, pbyChn, byNum );

        for( u8 byIdx = 1; byIdx < byNum; byIdx ++ )
        {
            BOOL32 bAdjusted = FALSE;
            for( u8 byPos = byNum-1; byPos >= byIdx; byPos-- )
            {
                if ( abyChn[byPos] > abyChn[byPos-1] )
                {
                    u8 byTmp = abyChn[byPos];
                    abyChn[byPos] = abyChn[byPos-1];
                    abyChn[byPos-1] = byTmp;

                    bAdjusted = TRUE;
                }
            }
            if ( !bAdjusted )
            {
                break;
            }            
        }
        memcpy( pbyChn, abyChn, byNum );
        return;
    }

    BOOL32 HasMtAdp() const
    {
        return (m_byMtAdpNum > 0);
    }

    BOOL32 HasMp() const
    {
        return (m_byMpNum > 0);
    }

    void SetIsGKCharge( BOOL32 bCharge )
    {
        m_byGKCharge = bCharge ? 1 : 0;
    }
    
}PACKED;

//ע����Ϣ(len:29)
struct TNPlusMcuRegInfo
{
public:
    void            SetMcuIpAddr( u32 dwIpAddr ) { m_dwMcuIpAddr = htonl(dwIpAddr); }
    u32             GetMcuIpAddr( void ) { return ntohl(m_dwMcuIpAddr); }
    void            SetMcuEqpCap( const TNPlusEqpCap &tEqpCap ) { m_tEqpCap = tEqpCap; }
    TNPlusEqpCap    GetMcuEqpCap( void ) { return m_tEqpCap; }
    void            SetMcuType( u8 byType ) { m_byMcuType = byType;    }
    u8              GetMcuType( void ){ return m_byMcuType;    }

private:
    u32             m_dwMcuIpAddr;  //net order
    TNPlusEqpCap    m_tEqpCap;      //�����������
    u8              m_byMcuType;    //8000 or 8000B or WIN32

}PACKED;

class CNPlusInst : public CInstance
{
    //״̬��
    enum
    {
        STATE_IDLE,
        STATE_NORMAL,
        STATE_SWITCHED
    };

public:
    CNPlusInst( void );
    virtual ~CNPlusInst( void );

    //�û���Ϣ
    BOOL32  GetGrpUsrCount( u8 byGrpId, u8 &byMaxNum, u8 &byNum );
    BOOL32  GetGrpUserList( u8 byGrpId, u8 *pbyBuf, u8 &byUsrItr, u8 &byUserNumInPack );
    CExUsrInfo*   GetUserPtr();
    CUsrGrpsInfo* GetUsrGrpInfo() ;

    void ClearInst( void );

protected:
    //��ͨʵ�����
    //����Serverģʽ��ÿ��ʵ����Ӧһ��mcu
	void InstanceEntry( CMessage * const pcMsg );  
    void InstanceDump( u32 dwParam = 0 );

    void ProcRegNPlusMcuReq( const CMessage * pcMsg );
    void PostReplyBack( u16 wEvent, u16 wErrorCode = 0, u8 *const pbyMsg = NULL, u16 wMsgLen = 0 );
    void ProcMcuDataUpdateReq( const CMessage * pcMsg );
    void ProcMcuEqpCapNotif( const CMessage * pcMsg );
    void ProcConfRollback( const CMessage * pcMsg );
    
    void ProcRtdRsp( const CMessage * pcMsg );
    void ProcRtdTimeOut( const CMessage * pcMsg );
    void ProcDisconnect( const CMessage * pcMsg );

    // ǿ��ֹͣ����
    void ProcReset( const CMessage * pcMsg );

    // ��ʼ���湤��
    void RestoreMcuConf( void );
    
    //Daemon ʵ�����
    //����Serverģʽ�¸���ʵ����Ϣ�ķַ�������Clientģʽ��demonʵ����Ӧ����mcu
    void DaemonInstanceEntry( CMessage* const pcMsg, CApp* pcApp );    
       
    void DaemonPowerOn( const CMessage * pcMsg, CApp* pcApp  );
    void DaemonReset( const CMessage * pcMsg, CApp* pcApp  );
    void DaemonDisconnect( const CMessage * pcMsg, CApp* pcApp  );

    // �û�����ع�
    void DaemonMcsRollBack( const CMessage * pcMsg, CApp* pcApp  );
    
    void DaemonConnectNPlusMcuTimeOut( const CMessage * pcMsg);
    void DaemonRegNPlusMcuTimeOut( const CMessage * pcMsg);
    
    void DaemonRegNPlusMcuReq( const CMessage * pcMsg, CApp* pcApp  );
    void DaemonRegNPlusMcuRsp( const CMessage * pcMsg, CApp* pcApp  );
    void DaemonReplyNack( const CMessage * pcMsg, CServMsg &cServMsg );
    void DaemonVcNPlusMsgNotif( const CMessage * pcMsg, CApp* pcApp );
    void DaemonDataUpdateRsp( const CMessage * pcMsg, CApp* pcApp );
    void DaemonRtdReq( const CMessage * pcMsg, CApp* pcApp );
    void DaemonRegNack( const CMessage * pcMsg, u8 byReason );

    //����ع�
    void DaemonConfRollbackReq( const CMessage * pcMsg, CApp* pcApp );
    void DaemonUsrRollbackReq( const CMessage * pcMsg, CApp* pcApp );
    void DaemonGrpRollbackReq( const CMessage * pcMsg, CApp* pcApp );
    
    //������Ϣ��N+1����mcu daemonʵ��
    void PostMsgToNPlusMcuDaemon( u16 wEvent, u8 *const pbyMsg = NULL, u16 wMsgLen = 0 );
    //������Ϣ��N+1����mcuʵ��
    void PostMsgToNPlusMcuInst( CServMsg &cServMsg );

    //�ڲ�������
    BOOL32  SetConfData( TNPlusConfData *ptConfData );
    BOOL32  SetConfInfo( TConfInfo *ptConfInfo, BOOL32 bStart = TRUE );
    BOOL32  SetConfMtInfo( CConfId &cConfId, TMtInfo *ptMtInfo, u8 byMtNum );
    BOOL32  SetConfVmpInfo( CConfId &cConfId, TNPlusVmpInfo *ptVmpInfo, TVMPParam *ptParam );
    BOOL32  SetChairman( CConfId &cConfId, TMtAlias *ptMtAlias );
    BOOL32  SetSpeaker( CConfId &cConfId, TMtAlias *ptMtAlias );
    BOOL32  SetUsrGrpInfo( CUsrGrpsInfo *pcUsrGrpInfo );
    BOOL32  SetUsrInfo( CExUsrInfo *pcUsrInfo, u8 byNum, BOOL32 bAdd = FALSE );

    u8      GetConfIdxByConfId( const CConfId &cConfId );    
    u8      GetEmptyConfIdx( void );
    
    void StopConfChargeOfSwitchedMcu( void );
    void UnRegAllInfoOfSwitchedMcu( void );
    void SetRASInfo( TRASInfo *ptRASInfo ){ memcpy( &m_tRASInfo, ptRASInfo, sizeof(TRASInfo) );    }
    TRASInfo *GetRASInfo( void ){ return &m_tRASInfo;   }
        
private:
    u32             m_dwMcuNode;        
    u32             m_dwMcuIId;
    u32             m_dwMcuIpAddr;                  //mcu��ַ��Ϣ(host order)

    TNPlusConfData  m_atConfData[MAXNUM_ONGO_CONF]; //mcu�ϻ�����Ϣ
    CUsrGrpsInfo    m_cUsrGrpsInfo;                 //mcu���û�����Ϣ
    CExUsrInfo      m_acUsrInfo[MAXNUM_USRNUM];     //�û���Ϣ
    u8              m_byUsrNum;                     //�û�����
    u16             m_wRtdFailTimes;                //rtdʧ�ܴ���
    u16             m_wRegNackByCapTimes;           //���ڶԶ������������޷��ع�ʱ���ܾ���ע��Ĵ���
    TRASInfo        m_tRASInfo;                     //rasinfo, ���ڱ���MCUαװע������MCU�ϵ�ʵ����Ϣ
    TConfChargeInfo m_atChargeInfo[MAXNUM_ONGO_CONF];//mcu�ϻ���ļƷ���Ϣ, ���ڱ���MCUαװ������MCU�ļƷѻ���
};

//ʵ��ȫ������
class CNPlusData
{       
public:
    CNPlusData( void );
    virtual ~CNPlusData( void );
    
    BOOL32          InitNPlusState(void);               // ��ʼ������mcu����״̬���������Ƿ���N+1ģʽ
    u8              GetMcuSwitchedInsId( void );
    u32             GetMcuSwitchedIp( void );
    void            SetMcuSwitchedInsId( u8 byInsId, u32 dwIpAddr );
    ENPlusState     GetLocalNPlusState( void );         // ��ȡ���� mcu N+1״̬��
    void            SetLocalNPlusState( ENPlusState emState );  // ���ñ��� mcu N+1״̬��
    u16             GetRtdTime( void );
    u16             GetRtdNum( void );
    void            SetNPlusSynOk( BOOL32 bSynOk );
    BOOL32          GetNPlusSynOk( void );
    TNPlusEqpCap    GetMcuEqpCap( void );
    TNPlusEqpCap    GetEqpCapFromConf( TConfInfo &tConf );
    void            SetMcuType( u8 byType );
    u8              GetMcuType( void );

    void            PostMsgToNPlusDaemon( u16 wEvent, u8 *const pbyMsg = NULL, u16 wMsgLen = 0 );   

private:
    ENPlusState     m_emLocalNPlusState;
    u8              m_byNPlusSwitchInsId;               //����ģʽ�л���ʵ��id
    u32             m_dwNPlusSwitchMcuIp;               //����ģʽ�л���mcu��ַ
    u16             m_wRtdTime;                         //rtd ʱ����
    u16             m_wRtdNum;                          //rtd ����
    u8              m_byNPlusSynOk;                     //�Ƿ����ݱ��ݳɹ�
    u8              m_byLocalMcuType;                   //����MCU������

};

typedef zTemplate< CNPlusInst, MAXNUM_NPLUS_MCU, CNPlusData > CNPlusApp;

extern  CNPlusApp           g_cNPlusApp;    

#ifdef WIN32
#pragma pack( pop )
#endif

#endif //_NPLUS_MANAGER_H
