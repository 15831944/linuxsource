/*****************************************************************************
    ģ����      : EqpAgt
    �ļ���      : eqpagtcommbase.cpp
    ����ļ�    : 
    �ļ�ʵ�ֹ���: EqpAgt����
    ����        : liaokang
    �汾        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    �޸ļ�¼:
    ��  ��      �汾        �޸���          �޸�����
    2012/06/18  4.7         liaokang        ����
******************************************************************************/
#include "eqpagtcommbase.h"
#include "eqpagtsnmp.h"
#include "eqpagtcfg.h"
#include "eqpagtscan.h"

CEqpAgtCBList   g_cEqpAgtGetList;
CEqpAgtCBList   g_cEqpAgtSetList;
CEqpAgtTrapList g_cEqpAgtTrapList;

// �ź�������
extern SEMHANDLE   g_hEqpAgtCfg;

// ����
CEqpAgtCBList::CEqpAgtCBList():m_wEqpAgtCBSize(0)
{
    m_pEqpAgtCBHead = NULL;
}

// ����
CEqpAgtCBList::~CEqpAgtCBList()
{
    m_wEqpAgtCBSize = 0;
    m_pEqpAgtCBHead = NULL;
}

/*====================================================================
    ������      : GetEqpAgtCBEntry
    ����        : ��ȡ��Ӧ��ŵĻص������ṹ��Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: u16 wEqpAgtCBIdx                   ���
    �������˵��: TEqpAgtCBEntry* ptEqpAgtCBntry     ��Ӧ�Ļص������ṹ��Ϣ
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtCBList::GetEqpAgtCBEntry( u16 wEqpAgtCBIdx, TEqpAgtCBEntry* ptEqpAgtCBntry )
{
    if ( NULL == ptEqpAgtCBntry )
    {
        EqpAgtLog( LOG_ERROR, "[GetEqpAgtTrapEntry] The input param is null!\n" );
        return FALSE;
    }
    
    EqpAgtCB *pEqpAgtCB = m_pEqpAgtCBHead;
    while( NULL != pEqpAgtCB ) 
    {
        if( wEqpAgtCBIdx == pEqpAgtCB->m_wEqpAgtCBIdx ) 
        {
            memcpy( ptEqpAgtCBntry, &pEqpAgtCB->m_tEqpAgtCBEntry, sizeof(TEqpAgtCBEntry) );
            return TRUE;
        } 
        pEqpAgtCB = pEqpAgtCB->m_pEqpAgtCBNext; 
    }
    EqpAgtLog( LOG_ERROR, "[GetEqpAgtTrapEntry] Do not find right param match to the index %u !\n", wEqpAgtCBIdx );
    return FALSE;
}

/*====================================================================
    ������      : Replace
    ����        : �滻�����ж�Ӧ�Ļص������ṹ��Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: const TEqpAgtTrapEntry& tEqpAgtTrapEntry �ص������ṹ��Ϣ
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtCBList::Replace( const TEqpAgtCBEntry& tEqpAgtCBEntry )
{
    if( NULL == &tEqpAgtCBEntry )
    {
        EqpAgtLog( LOG_ERROR, "[Replace] The input param is null!\n" );
        return;
    }
    
    EqpAgtCB *pTail = m_pEqpAgtCBHead;
    if ( NULL == pTail )
    {
        EqpAgtLog( LOG_ERROR, "[Replace] No item!\n" );
        return;
    }
    else
    {
        while ( NULL != pTail )
        {
            if( tEqpAgtCBEntry.dwNodeValue == pTail->m_tEqpAgtCBEntry.dwNodeValue ) 
            {                   
                // �滻
                memcpy( &pTail->m_tEqpAgtCBEntry, &tEqpAgtCBEntry, sizeof(pTail->m_tEqpAgtCBEntry));
                return;
            }
            pTail = pTail->m_pEqpAgtCBNext;
        }
    }
    return;    
}

/*====================================================================
    ������      : PushBack
    ����        : ������ĩβ�洢�ص������ṹ��Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: const TEqpAgtCBEntry& tEqpAgtCBEntry �ص������ṹ��Ϣ
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtCBList::PushBack( const TEqpAgtCBEntry& tEqpAgtCBEntry )
{
    if( NULL == &tEqpAgtCBEntry )
    {
        EqpAgtLog( LOG_ERROR, "[PushBack] The input param is null!\n" );
        return;
    }

    EqpAgtCB *pNew = new EqpAgtCB[1];
    if( NULL == pNew )
    {
        EqpAgtLog( LOG_ERROR, "[PushBack] Allocate pNew Memory failed!\n" );
        return;
    }
    memcpy( &pNew->m_tEqpAgtCBEntry, &tEqpAgtCBEntry, sizeof(pNew->m_tEqpAgtCBEntry));
    pNew->m_wEqpAgtCBIdx = m_wEqpAgtCBSize;
    m_wEqpAgtCBSize++;
    EqpAgtCB *pTail = m_pEqpAgtCBHead;
    if ( NULL == pTail )
    {
        pTail = pNew;
        pTail->m_pEqpAgtCBNext = NULL;
    }
    else
    {
        while ( NULL != pTail->m_pEqpAgtCBNext )
        {
            pTail = pTail->m_pEqpAgtCBNext;
        }
        pTail->m_pEqpAgtCBNext = pNew;
        pTail = pTail->m_pEqpAgtCBNext;
        pTail->m_pEqpAgtCBNext = NULL;
    }

    if( NULL == m_pEqpAgtCBHead )
    {
        m_pEqpAgtCBHead = pTail;
    }

    return;
}

// ����
CEqpAgtTrapList::CEqpAgtTrapList():m_wEqpAgtTrapSize(0)
{
    m_pEqpAgtTrapHead = NULL;
}

// ����
CEqpAgtTrapList::~CEqpAgtTrapList()
{
    m_wEqpAgtTrapSize = 0;
    m_pEqpAgtTrapHead = NULL;
}

/*====================================================================
    ������      : Replace
    ����        : �滻�����ж�Ӧ��Trap�����ṹ��Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: const TEqpAgtTrapEntry& tEqpAgtTrapEntry Trap�����ṹ��Ϣ
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtTrapList::Replace(const TEqpAgtTrapEntry& tEqpAgtTrapEntry)
{
    if( NULL == &tEqpAgtTrapEntry )
    {
        EqpAgtLog( LOG_ERROR, "[Replace] The input param is null!\n" );
        return;
    }

    EqpAgtTrap *pTail = m_pEqpAgtTrapHead;
    if ( NULL == pTail )
    {
        EqpAgtLog( LOG_ERROR, "[Replace] No item!\n" );
        return;
    }
    else
    {
        while ( NULL != pTail )
        {
            if( tEqpAgtTrapEntry.dwNodeValue == pTail->m_tEqpAgtTrapEntry.dwNodeValue ) 
            {                   
                // �滻
                memcpy( &pTail->m_tEqpAgtTrapEntry, &tEqpAgtTrapEntry, sizeof(pTail->m_tEqpAgtTrapEntry));
                return;
            }
            pTail = pTail->m_pEqpAgtTrapNext;
        }
    }
    return;
}

/*====================================================================
    ������      : PushBack
    ����        : ������ĩβ�洢Trap�����ṹ��Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: const TEqpAgtTrapEntry& tEqpAgtTrapEntry Trap�����ṹ��Ϣ
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void CEqpAgtTrapList::PushBack(const TEqpAgtTrapEntry& tEqpAgtTrapEntry)
{
    if( NULL == &tEqpAgtTrapEntry )
    {
        EqpAgtLog( LOG_ERROR, "[PushBack] The input param is null!\n" );
        return;
    }

    EqpAgtTrap *pNew = new EqpAgtTrap[1];
    if( NULL == pNew )
    {
        EqpAgtLog( LOG_ERROR, "[PushBack] Allocate pNew Memory failed!\n" );
        return;
    }
    memcpy( &pNew->m_tEqpAgtTrapEntry, &tEqpAgtTrapEntry, sizeof(pNew->m_tEqpAgtTrapEntry));
    pNew->m_wEqpAgtTrapIdx = m_wEqpAgtTrapSize;
    m_wEqpAgtTrapSize++;
    EqpAgtTrap *pTail = m_pEqpAgtTrapHead;
    if ( NULL == pTail )
    {
        pTail = pNew;
        pTail->m_pEqpAgtTrapNext = NULL;
    }
    else
    {
        while ( NULL != pTail->m_pEqpAgtTrapNext )
        {
            pTail = pTail->m_pEqpAgtTrapNext;
        }
        pTail->m_pEqpAgtTrapNext = pNew;
        pTail = pTail->m_pEqpAgtTrapNext;
        pTail->m_pEqpAgtTrapNext = NULL;
    }
    
    if( NULL == m_pEqpAgtTrapHead )
    {
        m_pEqpAgtTrapHead = pTail;
    }
    
    return;   
}

/*====================================================================
    ������      : GetEqpAgtTrapEntry
    ����        : ��ȡ��Ӧ��ŵ�trap�����ṹ��Ϣ
    �㷨ʵ��    :
    ����ȫ�ֱ���:
    �������˵��: u16 wEqpAgtTrapIdx                   ���
    �������˵��: TEqpAgtTrapEntry* ptEqpAgtTrapEntry  ��Ӧ��trap�����ṹ��Ϣ
    ����ֵ˵��  : BOOL32 �ɹ�����TURE,ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
BOOL32 CEqpAgtTrapList::GetEqpAgtTrapEntry( u16 wEqpAgtTrapIdx, TEqpAgtTrapEntry* ptEqpAgtTrapEntry )
{
    if ( NULL == ptEqpAgtTrapEntry )
    {
        EqpAgtLog( LOG_ERROR, "[GetEqpAgtTrapEntry] The input param is null!\n" );
        return FALSE;
    }

    EqpAgtTrap *pEqpAgtTrap = m_pEqpAgtTrapHead;
    while( NULL != pEqpAgtTrap ) 
    {
        if( wEqpAgtTrapIdx == pEqpAgtTrap->m_wEqpAgtTrapIdx ) 
        {
            memcpy( ptEqpAgtTrapEntry, &pEqpAgtTrap->m_tEqpAgtTrapEntry, sizeof(TEqpAgtTrapEntry) );
            return TRUE;
        } 
        pEqpAgtTrap = pEqpAgtTrap->m_pEqpAgtTrapNext; 
    }
    EqpAgtLog( LOG_ERROR, "[GetEqpAgtTrapEntry] Do not find right param match to the index %u !\n", wEqpAgtTrapIdx );
    return FALSE;
}

/*====================================================================
    ������      : EqpAgtGetOper
    ����        : snmp�Ļص�get����ע��
    �㷨ʵ��    :
    ����ȫ�ֱ���: g_cEqpAgtGetList
    �������˵��: u32 dwNodeName        ����������ݽ��������ҵ������
                  TEqpAgtCBFunc  pfFunc �ص�����ҵ�������ʵ��
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void EqpAgtGetOper(u32 dwNodeValue, TEqpAgtCBFunc pfFunc )
{
    // 1���ж��������Ƿ��Ѿ����ڶ�Ӧ����,��ֹ����ע��ͬһ����
    u16 wGetFuncNum = g_cEqpAgtGetList.Size();
    TEqpAgtCBEntry tEqpAgtGetEntry;
    memset( &tEqpAgtGetEntry, 0, sizeof(tEqpAgtGetEntry));
    BOOL32 bRet = TRUE;
    u16 wLoop = 0;
    if ( 0!= wGetFuncNum )
    {
        // ��ѯ����Trap������Ϣ��������
        for( wLoop = 0; wLoop < wGetFuncNum; wLoop++ )
        {
            bRet = g_cEqpAgtGetList.GetEqpAgtCBEntry( wLoop, &tEqpAgtGetEntry );
            if ( bRet )
            {
                if( dwNodeValue == tEqpAgtGetEntry.dwNodeValue ) 
                {                   
                    // ֱ���滻
                    TEqpAgtCBEntry tEqpAgtGetEntryNew = { dwNodeValue, pfFunc };
                    g_cEqpAgtGetList.Replace( tEqpAgtGetEntryNew );
                    return;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                return;
            }
        }
    }
    
    
    // 2�������ڣ����
    TEqpAgtCBEntry tEqpAgtGetEntryNew={ dwNodeValue, pfFunc };
    g_cEqpAgtGetList.PushBack( tEqpAgtGetEntryNew );
    return;
}

/*====================================================================
    ������      : EqpAgtSetOper
    ����        : snmp�Ļص�set����ע��
    �㷨ʵ��    :
    ����ȫ�ֱ���: g_cEqpAgtSetList
    �������˵��: u32 dwNodeName        ����������ݽ��������ҵ������
                  TEqpAgtCBFunc  pfFunc �ص�����ҵ�������ʵ��
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void EqpAgtSetOper(u32 dwNodeValue, TEqpAgtCBFunc pfFunc )
{
    // 1���ж��������Ƿ��Ѿ����ڶ�Ӧ����,��ֹ����ע��ͬһ����
    u16 wSetFuncNum = g_cEqpAgtSetList.Size();
    TEqpAgtCBEntry tEqpAgtSetEntry;
    memset( &tEqpAgtSetEntry, 0, sizeof(tEqpAgtSetEntry));
    BOOL32 bRet = TRUE;
    u16 wLoop = 0;
    if ( 0!= wSetFuncNum )
    {
        // ��ѯ����Trap������Ϣ��������
        for( wLoop = 0; wLoop < wSetFuncNum; wLoop++ )
        {
            bRet = g_cEqpAgtSetList.GetEqpAgtCBEntry( wLoop, &tEqpAgtSetEntry );
            if ( bRet )
            {
                if( dwNodeValue == tEqpAgtSetEntry.dwNodeValue ) 
                {                   
                    // ֱ���滻
                    TEqpAgtCBEntry tEqpAgtSetEntryNew = { dwNodeValue, pfFunc };
                    g_cEqpAgtSetList.Replace( tEqpAgtSetEntryNew );
                    return;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                return;
            }
        }
    }


    // 2�������ڣ����
    TEqpAgtCBEntry tEqpAgtSetEntryNew={ dwNodeValue, pfFunc };
    g_cEqpAgtSetList.PushBack( tEqpAgtSetEntryNew );
    return;
}

/*====================================================================
    ������      : EqpAgtTrapOper
    ����        : snmp��trap����ע��
    �㷨ʵ��    :
    ����ȫ�ֱ���: g_cEqpAgtTrapList
    �������˵��: u32 dwNodeName        ����������ݽ��������ҵ������
                  void* pfFunc          Trap����
                  u32 dwScanTimeSpan    ��ʱ���Trap��ʱ������λ���룩
                  BOOL32 bEnable        �Ƿ�����
    ����ֵ˵��  : void
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void EqpAgtTrapOper(u32 dwNodeValue, TEqpAgtTrapFunc pfFunc, u32 dwScanTimeSpan, BOOL32 bEnable )
{
    // 1���ж��������Ƿ��Ѿ����ڶ�Ӧ����,��ֹ����ע��ͬһ����
    u16 wTrapFuncNum = g_cEqpAgtTrapList.Size();
    TEqpAgtTrapEntry tEqpAgtTrapEntry;
    memset( &tEqpAgtTrapEntry, 0, sizeof(tEqpAgtTrapEntry));
    BOOL32 bRet = TRUE;
    u16 wLoop = 0;
    if ( 0!= wTrapFuncNum )
    {
        // ��ѯ����Trap������Ϣ��������
        for( wLoop = 0; wLoop < wTrapFuncNum; wLoop++ )
        {
            bRet = g_cEqpAgtTrapList.GetEqpAgtTrapEntry( wLoop, &tEqpAgtTrapEntry );
            if ( bRet )
            {
                if( dwNodeValue == tEqpAgtTrapEntry.dwNodeValue ) 
                {                   
                    // ֱ���滻
                    TEqpAgtTrapEntry tEqpAgtTrapEntryNew = { dwNodeValue, pfFunc, dwScanTimeSpan, bEnable };
                    g_cEqpAgtTrapList.Replace( tEqpAgtTrapEntryNew );
                    return;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                return;
            }
        }
    }

    // 2�������ڣ����
    TEqpAgtTrapEntry tEqpAgtTrapEntryNew={ dwNodeValue, pfFunc, dwScanTimeSpan, bEnable };
    g_cEqpAgtTrapList.PushBack( tEqpAgtTrapEntryNew );
    return;
}

/*====================================================================
    ������      : AgentCallBack
    ����        : Snmp�ص�����ʵ��
    �㷨ʵ��    :
    ����ȫ�ֱ���: g_cEqpAgtCfg
                  g_cEqpAgtGetList
                  g_cEqpAgtSetList
    �������˵��: u32 dwNodeName:    ����ڵ���
                  u8  byRWFlag:      ��дģʽ
                  void * pBuf:       �����������
                  u16 *pwBufLen:     ���ݳ���
    ����ֵ˵��  : SNMP_SUCCESS       �ɹ�
                  SNMP_GENERAL_ERROR ����
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
u16 AgentCallBack(u32 dwNodeName, u8 byRWFlag, void * pBuf, u16* pwBufLen)
{
    u32    dwNodeValue = GET_NODENAME(dwNodeName);
    BOOL32 bRead = ( READ_FLAG == byRWFlag ) ? TRUE : FALSE;
    u16    wResult = SNMP_SUCCESS;
    BOOL32 bRet = TRUE;

    // 1�������ж��Ƿ�Ϊ����Trap��Ϣ
    switch(dwNodeValue) 
    {
    // Trap Ip��Ϣ
    case NODE_COMMTRAPIPS:
        if( bRead ) // get
        {
            bRet = g_cEqpAgtCfg.TrapIpsOfEqpAgt2Nms( pBuf, pwBufLen );
            if ( !bRet )
            {
                wResult = SNMP_GENERAL_ERROR;
            }
            return wResult;
        }
        else // set
        {
            bRet = g_cEqpAgtCfg.TrapIpsOfNms2EqpAgt( pBuf, pwBufLen );            
            if ( !bRet )
            {
                wResult = SNMP_GENERAL_ERROR;
            }
            return wResult;
        }

    // ������Ϣ
    default:        
        break;
    }	  

    // 2�����Ҷ�Ӧ�������д���
    TEqpAgtCBEntry tEqpAgtCBEntry;
    memset( &tEqpAgtCBEntry, 0, sizeof(tEqpAgtCBEntry));
    u16 wLoop = 0;

    if ( bRead )
    { 
        u16 wGetFuncNum = g_cEqpAgtGetList.Size();
        if ( 0!= wGetFuncNum )
        {
            for( wLoop = 0; wLoop < wGetFuncNum; wLoop++ )
            {
                bRet = g_cEqpAgtGetList.GetEqpAgtCBEntry( wLoop, &tEqpAgtCBEntry );
                if ( bRet )
                {
                    if( dwNodeValue == tEqpAgtCBEntry.dwNodeValue ) 
                    {
                        wResult = (*tEqpAgtCBEntry.pfFunc)( dwNodeName, pBuf, pwBufLen);
                        return wResult;
                    }
                    else
                    {
                        continue;
                    }
                }
                else
                {
                    wResult = SNMP_GENERAL_ERROR;
                    return wResult;
                }
            }
        }
    }
    else
    {
        u16 wSetFuncNum = g_cEqpAgtSetList.Size();
        if ( 0!= wSetFuncNum )
        {
            for( wLoop = 0; wLoop < wSetFuncNum; wLoop++ )
            {
                bRet = g_cEqpAgtSetList.GetEqpAgtCBEntry( wLoop, &tEqpAgtCBEntry );
                if ( bRet )
                {
                    if( dwNodeValue == tEqpAgtCBEntry.dwNodeValue ) 
                    {
                        wResult = (*tEqpAgtCBEntry.pfFunc)( dwNodeName, pBuf, pwBufLen);
                        return wResult;
                    }
                    else
                    {
                        continue;
                    }
                }
                else
                {
                    wResult = SNMP_GENERAL_ERROR;
                    return wResult;
                }
            }
        }
    }

    // 3���Ҳ�����Ӧ�Ĵ�����
    EqpAgtLog( LOG_ERROR, "[AgentCallBack] Received node(0x%x) is not supported by operate: %d<1.read-0.write>!\n", dwNodeName, bRead );
    wResult = SNMP_GENERAL_ERROR;
    return wResult;
}

/*====================================================================
    ������      : EqpAgtInitialize
    ����        : EqpAgt��ʼ��
    �㷨ʵ��    :
    ����ȫ�ֶ���: g_cEqpAgtCfg
                  g_cEqpAgtSnmp
                  g_cEqpAgtScanApp
    �������˵��: u16 wAidEqpAgtScan     �߳�id
                  u8 byAppPri   �߳����ȼ�
                  LPCSTR lpszCfgFileFullPath �����ļ�ȫ·��
    ����ֵ˵��  : �ɹ��򷵻� EQPAGT_SUCCESS
                  ���򷵻ش����룬���eqpagtcommon.h
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
u16 EqpAgtCommBaseInit( u16 wAidEqpAgtScan, u8 byAppPri, LPCSTR lpszCfgPath, LPCSTR lpszCfgName )
{
    u16    wRet = SNMP_SUCCESS;
    s32    nRet = OSP_OK;

    // 1�������ļ���ʼ��
    g_cEqpAgtCfg.EqpAgtCfgInit( lpszCfgPath, lpszCfgName );

    // 2��snmp��ʼ��
    TSnmpAdpParam tSnmpParam;
    memset( &tSnmpParam, 0, sizeof(tSnmpParam) );
	g_cEqpAgtCfg.GetSnmpParam( tSnmpParam );
    wRet = g_cEqpAgtSnmp.Initialize( AgentCallBack, &tSnmpParam );
    if ( SNMP_SUCCESS == wRet )
    {
        EqpAgtLog( LOG_KEYSTATUS, "[EqpAgtInit] snmp init successed!\n" );
    }
    else
    {
        EqpAgtLog( LOG_KEYSTATUS, "[EqpAgtInit] snmp init failed!\n" );
        return EQPAGT_INITERR_SNMPINIT;
    }

    // 3��ɨ���豸trap��Ϣ�̴߳���������
    nRet = g_cEqpAgtScanApp.CreateApp("EqpAgtScan", wAidEqpAgtScan, byAppPri, 50);
    if ( OSP_OK == nRet )
    {
        EqpAgtLog( LOG_KEYSTATUS, "[EqpAgtInit] Creat EqpAgtScan App successed!\n" );
        nRet = OspPost( MAKEIID(wAidEqpAgtScan, 1), EQPAGT_SCAN_POWERON );
        if ( OSP_OK == nRet )
        {
            EqpAgtLog( LOG_KEYSTATUS, "[EqpAgtInit] Post power on msg to EqpAgtScan successed!\n" );
            return EQPAGT_SUCCESS;
        }
        else
        {
            EqpAgtLog( LOG_KEYSTATUS, "[EqpAgtInit] Post power on msg to EqpAgtScan failed!\n" );
            return EQPAGT_INITERR_POSTMSG;
        }
    }
    else
    {      
        EqpAgtLog( LOG_ERROR, "[EqpAgtInit] Creat EqpAgtScan App failed!\n" );
        return EQPAGT_INITERR_CREATAPP;
    }
}

/*====================================================================
    ������      : EqpAgtQuit
    ����        : EqpAgt�˳�
    �㷨ʵ��    :
    ����ȫ�ֶ���: 
    �������˵��: 
    ����ֵ˵��  : 
----------------------------------------------------------------------
    �޸ļ�¼    :
    ��  ��      �汾        �޸���        �޸�����
    12/06/18    4.7         liaokang       ����
====================================================================*/
void EqpAgtCommBaseQuit(void)
{
    g_cEqpAgtSnmp.Quit();
}