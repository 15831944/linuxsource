/*****************************************************************************
    ģ����      : EqpAgt
    �ļ���      : eqpagtcfg.h
    ����ļ�    : 
    �ļ�ʵ�ֹ���: �����豸������Ϣ
    ����        : liaokang
    �汾        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    �޸ļ�¼:
    ��  ��      �汾        �޸���          �޸�����
    2012/06/18  4.7         liaokang        ����
******************************************************************************/
#ifndef _EQPAGT_CFG_H_
#define _EQPAGT_CFG_H_
#include "eqpagtutility.h"
#include "eqpagtcommon.h"

#define MAXLEN_PORT		         (u8)5                      // ���˿��ַ�������

/*------------------------------------------------------------- 
*EqpAgt��Ϣ���� 
*-------------------------------------------------------------*/
#define SECTION_EqpAgtInfoTable   ( LPCSTR )"EqpAgtInfoTable"// EqpAgt��Ϣ��
#define ENTRY_NUM		          ( LPCSTR )"EntryNum"       // ����Trap�����ܷ�������Ŀ
#define ENTRY_KEY				  ( LPCSTR )"Entry"          // Entry �ַ���
#define FIELD_TrapSrvIpAddr       ( LPCSTR )"TrapSrvIpAddr"  // ����Trap�����ܷ�����Ip(������)
#define FIELD_ReadCommunity       ( LPCSTR )"ReadCommunity"  // ����ͬ��
#define FIELD_WriteCommunity      ( LPCSTR )"WriteCommunity" // д��ͬ��
#define FIELD_GetSetPort          ( LPCSTR )"GetSetPort"     // ��ȡ�˿�(������)
#define FIELD_SendTrapPort        ( LPCSTR )"SendTrapPort"   // ��Trap�˿�(������)

class CEqpAgtCfg
{
public:
    CEqpAgtCfg();
	~CEqpAgtCfg();

    // ������ں���
    BOOL32 EqpAgtCfgInit( LPCSTR lpszCfgPath, LPCSTR lpszCfgName );		     // ��ʼ��������

    // EqpAgt��Ϣ��ά��������EqpAgt��Ϣ��'����'��/д
    BOOL32 ReadEqpAgtInfoTable( u8* pbyTrapSrvNum, TEqpAgtInfo* ptEqpAgtInfoTable ); // �����EqpAgt��Ϣ��  
    BOOL32 WriteEqpAgtInfoTable( u8 byTrapSrvNum,  TEqpAgtInfo* ptEqpAgtInfoTable ); // ����дEqpAgt��Ϣ��

    // Trap��Ϣ������Trap��Ϣ��'����'��/д
    BOOL32 TrapIpsOfEqpAgt2Nms( void *pBuf, u16 *pwBufLen );    // Trap Ip��Ϣ EqpAgt ----> Nms
    BOOL32 TrapIpsOfNms2EqpAgt( void *pBuf, u16 *pwBufLen );    // Trap Ip��Ϣ Nms ----> EqpAgt

    // snmp    
    u8     GetTrapServerNum( void );                            // ȡTrap��������    
    void   GetTrapTarget( u8 byIdx, TTarget& tTrapTarget );     // ȡĳһTrap��������Ӧ��Trap��Ϣ�ṹ    
    BOOL32 GetSnmpParam( TSnmpAdpParam& tParam ) const;         // ȡSNMP����    
    BOOL32 HasSnmpNms( void ) const;                            // ȡ�Ƿ�����������

protected:
    // �����ļ�  
    BOOL32 OrganizeEqpAgtInfoTable( u8 byIdx, TEqpAgtInfo * pEqpAgtInfo, s8* ptInfTobeWrited ); // ����EqpAgt����������
    BOOL32 ReadTrapTable( LPCSTR lpszCfgFileFullPath );         // ��Trap��
    void   CreateDir( LPCSTR pPathName );			            // ����Ŀ¼
    BOOL32 PretreatCfgFile(  LPCSTR lpszCfgPath, LPCSTR lpszCfgName ); // û�������ļ�ʱ����Ĭ�������ļ�
private:
    s8          m_achCfgPath[EQPAGT_MAX_PATH_LEN];	            // �����ļ�·��
    s8          m_achCfgName[EQPAGT_MAX_NAME_LEN];	            // �����ļ���
    u8		    m_byTrapSrvNum;				                    // Trap��������Ŀ
    TEqpAgtInfo m_atEqpAgtInfoTable[MAXNUM_EQPAGTINFO_LIST];	// EqpAgt��Ϣ��
};


// EqpAgtCfg ����Ĭ��ֵ�������
typedef struct EqpAgtCfgDefValue
{
    u16                 m_wIdx;
    TEqpAgtCfgDefValue  m_pfFunc;
    EqpAgtCfgDefValue  *m_pNext;
}EqpAgtCfgDefValue;

class CEqpAgtCfgDefValueList
{
public:
    CEqpAgtCfgDefValueList();
    ~CEqpAgtCfgDefValueList();
    
    u16    Size( void ) { return m_wEqpAgtCfgDefValueSize;}
    BOOL32 GetEqpAgtCfgDefValue( u16 wIdx, TEqpAgtCfgDefValue* pfFunc );
    void   PushBack( TEqpAgtCfgDefValue pfFunc );    
private:
    EqpAgtCfgDefValue *m_pEqpAgtCfgDefValueHead;
    u16                m_wEqpAgtCfgDefValueSize;
};


extern CEqpAgtCfg	g_cEqpAgtCfg;
extern CEqpAgtCfgDefValueList   g_cEqpAgtCfgDefValueList;

#endif  // _EQPAGT_CFG_H_
