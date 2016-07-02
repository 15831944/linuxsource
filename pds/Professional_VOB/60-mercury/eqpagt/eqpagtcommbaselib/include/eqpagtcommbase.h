/*****************************************************************************
    ģ����      : EqpAgt
    �ļ���      : eqpagtcommbase.h
    ����ļ�    : 
    �ļ�ʵ�ֹ���: EqpAgtͨѶ����
    ����        : liaokang
    �汾        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    �޸ļ�¼:
    ��  ��      �汾        �޸���          �޸�����
    2012/06/18  4.7         liaokang        ����
******************************************************************************/

#ifndef _EQPAGT_COMMBASE_H_
#define _EQPAGT_COMMBASE_H_

#include "eqpagtutility.h"

struct TEqpAgtCBEntry
{
    // ����������ݽ��������ҵ������
    u32  dwNodeValue;
    // �ص�����ҵ�������ʵ��
    TEqpAgtCBFunc pfFunc;
};

struct TEqpAgtTrapEntry
{
    // ����������ݽ��������ҵ������
    u32  dwNodeValue;
    // Trap����
    TEqpAgtTrapFunc pfFunc;
    // ��ʱ���Trap��ʱ������λ���룩
    u32 dwScanTimeSpan;
    // ֧��Trap�������Ա���Կ�ĳ���ӡ��
    BOOL32 bEnable;
};

typedef struct EqpAgtCB
{
    u16            m_wEqpAgtCBIdx;
    TEqpAgtCBEntry m_tEqpAgtCBEntry;
    EqpAgtCB      *m_pEqpAgtCBNext;
}EqpAgtCB;

typedef struct EqpAgtTrap
{
    u16              m_wEqpAgtTrapIdx;
    TEqpAgtTrapEntry m_tEqpAgtTrapEntry;
    EqpAgtTrap      *m_pEqpAgtTrapNext;
}EqpAgtTrap;

// Get/Set �������
class CEqpAgtCBList
{
public:
    CEqpAgtCBList();
	~CEqpAgtCBList();

    u16    Size( void ) { return m_wEqpAgtCBSize;}
    BOOL32 GetEqpAgtCBEntry( u16 wEqpAgtCBIdx, TEqpAgtCBEntry* ptEqpAgtCBntry );
    void   PushBack( const TEqpAgtCBEntry& tEqpAgtCBEntry );    
    void   Replace( const TEqpAgtCBEntry& tEqpAgtCBEntry );
private:
    EqpAgtCB *m_pEqpAgtCBHead;
    u16       m_wEqpAgtCBSize;
};

// Trap �������
class CEqpAgtTrapList
{
public:
    CEqpAgtTrapList();
	~CEqpAgtTrapList();

    u16    Size( void ) { return m_wEqpAgtTrapSize;}
    BOOL32 GetEqpAgtTrapEntry( u16 wEqpAgtTrapIdx, TEqpAgtTrapEntry* ptEqpAgtTrapEntry );
    void   PushBack( const TEqpAgtTrapEntry& tEqpAgtTrapEntry );    
    void   Replace( const TEqpAgtTrapEntry& tEqpAgtTrapEntry );
private:
    EqpAgtTrap *m_pEqpAgtTrapHead;
    u16         m_wEqpAgtTrapSize;
};

// �������
extern CEqpAgtCBList   g_cEqpAgtGetList;
extern CEqpAgtCBList   g_cEqpAgtSetList;
extern CEqpAgtTrapList g_cEqpAgtTrapList;

// �ص�����ʵ��
u16 AgentCallBack(u32 dwNodeName, u8 byRWFlag, void * pBuf, u16* pwBufLen);
// �ͷ��ź���
BOOL32 FreeSemHandle( SEMHANDLE &hSem );
// EqpAgt��ʼ��
u16 EqpAgtCommBaseInit( u16 wAidEqpAgtScan, u8 byAppPri, LPCSTR lpszCfgPath, LPCSTR lpszCfgName );
// �˳�
void EqpAgtCommBaseQuit(void);

#endif  // _EQPAGT_COMMBASE_H_