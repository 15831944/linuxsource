/*****************************************************************************
   ģ����      : Mcu Agent
   �ļ���      : ProcAlarm.cpp
   ����ļ�    : ProcAlarm.h
   �ļ�ʵ�ֹ���: �澯���ά��
   ����        : liuhuiyun
   �汾        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2005/08/17  4.0         liuhuiyun       ����
   2006/04/30  4.0		   liuhuiyun	   �Ż�
******************************************************************************/
//lint -sem(CAlarmProc::Quit,cleanup)
//lint -sem(CAlarmProc::InitAlarmTable,initializer)
/*lint -save -e1565*/
#include "procalarm.h"
#include "configureagent.h"

CAlarmProc g_cAlarmProc;

CAlarmProc::CAlarmProc() : m_tMcuPfm(), m_dwNowAlarmNo(0), m_wErrorNum(0)
{
    m_dwNowAlarmNo = 1;
	m_tMcuPfm.m_wAlarmStamp = 0;
	memset( m_achCfgErr, '\0',sizeof(m_achCfgErr) );
	memset(m_achCfgErr, 0, sizeof(m_achCfgErr));

	InitAlarmTable();
}

CAlarmProc::~CAlarmProc()
{
	Quit();
}

/*=============================================================================
  �� �� ���� InitAlarmTable
  ��    �ܣ� ��ʼ���澯��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/

void CAlarmProc::InitAlarmTable( void )
{
    memset( m_atAlarmTable, 0, sizeof(m_atAlarmTable) );
    	
	if( !OspSemBCreate(&m_hMcuAlarmTable) )  // �澯������ź���
	{
		OspSemDelete( m_hMcuAlarmTable );
		Agtlog(LOG_ERROR, "[Agent] create m_hMcuAlarmTable failed!\n" );
	}
    return;
}

/*=============================================================================
  �� �� ���� Quit
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
/*lint -save -e1551*/
void CAlarmProc::Quit( void )
{
	if( NULL != m_hMcuAlarmTable )
	{
		OspSemDelete( m_hMcuAlarmTable );
		m_hMcuAlarmTable = NULL;
	}
}

/*=============================================================================
  �� �� ���� AddAlarm
  ��    �ܣ� ���Ӹ澯
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u32 dwAlarmCode
             u8 byObjType
             u8 byObject[]
             TMcupfmAlarmEntry * ptData
  �� �� ֵ�� BOOL 
=============================================================================*/
BOOL32 CAlarmProc::AddAlarm( u32 dwAlarmCode, u8 byObjType, u8 abyObject[], TMcupfmAlarmEntry * ptData )
{
    ENTER( m_hMcuAlarmTable )

    u32     dwLoop;
    time_t  tCurTime;
    struct tm   *ptTime;

    // �ҵ�һ���ձ���
    for(dwLoop = 0; dwLoop < MAXNUM_MCU_ALARM; dwLoop++ )
    {
        if( !m_atAlarmTable[dwLoop].m_bExist )
        {
            break;
        }
    }
    
    if( dwLoop >= sizeof( m_atAlarmTable ) / sizeof( m_atAlarmTable[0] ) )
    {
        return FALSE;
    }

	// ��д�ø澯����
    m_atAlarmTable[dwLoop].m_dwAlarmCode = dwAlarmCode;
    m_atAlarmTable[dwLoop].m_byObjType = byObjType;
    memcpy( m_atAlarmTable[dwLoop].m_achObject, abyObject, sizeof(m_atAlarmTable[0].m_achObject) );
    
    tCurTime = time( 0 );
    ptTime = localtime( &tCurTime );
    sprintf( m_atAlarmTable[dwLoop].m_achTime, "%4.4u-%2.2u-%2.2u %2.2u:%2.2u:%2.2u", ptTime->tm_year + 1900, 
             ptTime->tm_mon + 1, ptTime->tm_mday, ptTime->tm_hour, ptTime->tm_min, ptTime->tm_sec );
    
    m_atAlarmTable[dwLoop].m_bExist = TRUE;

    // ����ֵ
    ptData->m_dwSerialNo = dwLoop + 1;  // ��һ��ʼ
    ptData->m_dwAlarmCode = m_atAlarmTable[dwLoop].m_dwAlarmCode;
    ptData->m_byObjType = m_atAlarmTable[dwLoop].m_byObjType;
    memcpy( ptData->m_achObject, m_atAlarmTable[dwLoop].m_achObject, sizeof( ptData->m_achObject ) );
    memcpy( ptData->m_achTime, m_atAlarmTable[dwLoop].m_achTime, sizeof(ptData->m_achTime) );
    
    m_dwNowAlarmNo = m_dwNowAlarmNo < (dwLoop+1) ? (dwLoop+1) : m_dwNowAlarmNo;    // Ŀǰ�ĸ澯��
    m_tMcuPfm.m_wAlarmStamp++;

	Agtlog(LOG_VERBOSE, "[ProcAlarm] m_tMcuPfm.m_wAlarmStamp = %d.\n",m_tMcuPfm.m_wAlarmStamp);
	Agtlog(LOG_VERBOSE, "[ProcAlarm] add alarm wLoop= %d, m_dwNowAlarmNo= %d.\n", dwLoop, m_dwNowAlarmNo);	
	Agtlog(LOG_VERBOSE, "[ProcAlarm] l= %d, s= %d, t= %d.\n", abyObject[0], abyObject[1], abyObject[2]);
    
	return TRUE;
}

/*=============================================================================
  �� �� ���� FindAlarm
  ��    �ܣ� ���Ҹ澯
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  u32 dwAlarmCode
             u8 byObjType
             u8 byObject[]
             TMcupfmAlarmEntry *ptData
  �� �� ֵ�� BOOL 
=============================================================================*/
BOOL32 CAlarmProc::FindAlarm( u32 dwAlarmCode, u8 byObjType, u8 abyObject[], TMcupfmAlarmEntry *ptData )
{
    ENTER( m_hMcuAlarmTable )

    BOOL32    bFind = FALSE;
	u32 dwLoop = 0;
	// �澯���е�ÿһ��
    for( dwLoop = 0; dwLoop < m_dwNowAlarmNo; dwLoop++ )
    {
        if( !m_atAlarmTable[dwLoop].m_bExist )   // �ø澯�Ƿ����
        {
            continue;
        }

		// ��͸澯��������Ҫһ��
        if( dwAlarmCode == m_atAlarmTable[dwLoop].m_dwAlarmCode 
            && byObjType == m_atAlarmTable[dwLoop].m_byObjType )
        {
			// ����Ҫһ��
            switch( byObjType )
            {
            case ALARMOBJ_MCU_BOARD:
                if( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] &&
				    abyObject[1] == m_atAlarmTable[dwLoop].m_achObject[1] &&
				    abyObject[2] == m_atAlarmTable[dwLoop].m_achObject[2] )
                {
                    bFind = TRUE;
                }
                break;
            case ALARMOBJ_MCU_LINK:
                if( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] &&
                    abyObject[1] == m_atAlarmTable[dwLoop].m_achObject[1] &&
					abyObject[2] == m_atAlarmTable[dwLoop].m_achObject[2] &&
					abyObject[3] == m_atAlarmTable[dwLoop].m_achObject[3] )
                {
                    bFind = TRUE;
                }
                break;
            case ALARMOBJ_MCU:
			case ALARMOBJ_MCU_SDH:
                bFind = TRUE;
                break;

			case ALARMOBJ_MCU_SOFTWARE:
				if( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] &&
                    abyObject[1] == m_atAlarmTable[dwLoop].m_achObject[1] &&
					abyObject[2] == m_atAlarmTable[dwLoop].m_achObject[2] )
				{
					bFind = TRUE;
				}
				break;

			case ALARMOBJ_MCU_TASK:
				if( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] &&
                    abyObject[1] == m_atAlarmTable[dwLoop].m_achObject[1] &&
					abyObject[2] == m_atAlarmTable[dwLoop].m_achObject[2] &&
					*(u16*)&abyObject[3] == *(u16*)&m_atAlarmTable[dwLoop].m_achObject[3] )
				{
					bFind = TRUE;
				}
				break;

			case ALARMOBJ_MCU_POWER:
				if( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] &&
					abyObject[1] == m_atAlarmTable[dwLoop].m_achObject[1] )
				{
					bFind = TRUE;
				}
				break;

			case ALARMOBJ_MCU_MODULE:
				if( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] && 
					abyObject[1] == m_atAlarmTable[dwLoop].m_achObject[1] &&
					abyObject[2] == m_atAlarmTable[dwLoop].m_achObject[2] &&
					abyObject[3] == m_atAlarmTable[dwLoop].m_achObject[3] )
				{
					bFind = TRUE;
				}
				break;
				
			case ALARMOBJ_MCU_BRD_FAN:
				if( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] &&
					abyObject[1] == m_atAlarmTable[dwLoop].m_achObject[1] &&
					abyObject[2] == m_atAlarmTable[dwLoop].m_achObject[2] &&
					abyObject[3] == m_atAlarmTable[dwLoop].m_achObject[3] )
				{
					bFind = TRUE;
				}
				break;
			
			// [2010/12/15 miaoqingsong add] NMS5.0�¹������
			case ALARMOBJ_MCU_BOX_FAN:                                               
				if ( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] &&
					 abyObject[1] == m_atAlarmTable[dwLoop].m_achObject[1] )
				{
					bFind = TRUE;
				}
				break;

			case ALARMOBJ_MCU_MPC_CPU:                                               
				if ( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] )
				{
					bFind = TRUE;
				}
				break;

			case ALARMOBJ_MCU_MPC_MEMORY:                                            
				if ( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] )
				{
					bFind = TRUE;
				}
				break;

			case ALARMOBJ_MCU_POWER_TEMP:                                             
				if ( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] )
				{
					bFind = TRUE;
				}
				break;
				
			case ALARMOBJ_MCU_MPC_TEMP:                                               
				if ( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] )
				{
					bFind = TRUE;
				}
				break;
			// [2010/12/15 miaoqingsong add end]
			case ALARMOBJ_MCU_CPU_FAN:
				if ( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] )
				{
					bFind = TRUE;
				}
				break;

            default:
                Agtlog( LOG_VERBOSE, "[FindAlarm]: FindAlarm(): Wrong AlarmObjType!\n");
                break;
            }
        
            if( bFind )
            {
                break;
            }
        }   // end if
    }

    if( bFind )
    {
        // ����ֵ
        ptData->m_dwSerialNo = dwLoop + 1;
        ptData->m_dwAlarmCode = m_atAlarmTable[dwLoop].m_dwAlarmCode;
        ptData->m_byObjType = m_atAlarmTable[dwLoop].m_byObjType;

		//lukunpeng [2/26/2010]
		//����ҵ��ˣ�Ӧ�ø��´�ʱ��Object,��ΪObject�п������豸��State,�����µĻ�������ַ���nms��state����
		memcpy( m_atAlarmTable[dwLoop].m_achObject, abyObject, sizeof(m_atAlarmTable[dwLoop].m_achObject) );
        memcpy( ptData->m_achObject, 
                m_atAlarmTable[dwLoop].m_achObject, sizeof( ptData->m_achObject ) );

        time_t tCurTime = time( 0 );
		struct tm   *ptTime = localtime( &tCurTime );
		sprintf( ptData->m_achTime, "%4.4u-%2.2u-%2.2u %2.2u:%2.2u:%2.2u", ptTime->tm_year + 1900, 
					ptTime->tm_mon + 1, ptTime->tm_mday, ptTime->tm_hour, ptTime->tm_min, ptTime->tm_sec );

    }
    return bFind;
}

/*=============================================================================
  �� �� ���� DeleteAlarm
  ��    �ܣ� ɾ���澯
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwSerialNo
  �� �� ֵ�� BOOL 
=============================================================================*/
BOOL32 CAlarmProc::DeleteAlarm( u32 dwSerialNo )
{
    ENTER( m_hMcuAlarmTable )
	
    if( dwSerialNo > m_dwNowAlarmNo )
    {
        Agtlog(LOG_ERROR, "[DeleteAlarm] The Alarm No is no exist\n");
        return FALSE;
    }
	
    if( 0 != dwSerialNo)    // ����Խ��
    {
        m_atAlarmTable[dwSerialNo - 1].m_bExist = FALSE;
    }
    else
    {
        m_atAlarmTable[dwSerialNo].m_bExist = FALSE;
    }
    
    if( dwSerialNo == m_dwNowAlarmNo && m_dwNowAlarmNo != 0 )     // ���һ��
    {
        m_dwNowAlarmNo--;   // ����һ��
        if(0 == m_dwNowAlarmNo)
        {
            m_dwNowAlarmNo = 1; // ��һ��ʼ
        }
    }

    // �澯�������ӡ�ɾ���澯ʱ��1 
    m_tMcuPfm.m_wAlarmStamp++;

    return TRUE;
}
/*=============================================================================
  �� �� ���� GetAlarmStamp
  ��    �ܣ� ȡ�澯��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16& wAlarmStamp
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CAlarmProc::GetAlarmStamp(u16& wAlarmStamp)
{
    wAlarmStamp = m_tMcuPfm.m_wAlarmStamp;
    return TRUE;
}

/*=============================================================================
  �� �� ���� GetAlarmTableIndex
  ��    �ܣ� ȡ�澯����Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wTableIndex
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CAlarmProc::GetAlarmTableIndex(u16 wTableIndex, TMcupfmAlarmEntry& tAlarmEntry)
{
    BOOL32 bRet = TRUE;

    if ( wTableIndex >= MAXNUM_MCU_ALARM )
    {
        Agtlog( LOG_ERROR, "[GetAlarmTableIndex] param err: wIndex = %d !\n", wTableIndex );
        return FALSE;
    }
    Agtlog(LOG_VERBOSE, "[GetAlarmTableIndex] wTableIndex= %d, m_dwNowAlarmNo= %d, m_bExist= %d\n", 
                        wTableIndex, m_dwNowAlarmNo, m_atAlarmTable[wTableIndex].m_bExist);

	// ɨ��澯��
	if( wTableIndex < m_dwNowAlarmNo && TRUE == m_atAlarmTable[wTableIndex].m_bExist )
	{
        tAlarmEntry.m_dwSerialNo = wTableIndex;
        tAlarmEntry.m_byObjType = m_atAlarmTable[wTableIndex].m_byObjType;
        tAlarmEntry.m_dwAlarmCode = m_atAlarmTable[wTableIndex].m_dwAlarmCode;
        memcpy( tAlarmEntry.m_achTime, m_atAlarmTable[wTableIndex].m_achTime, sizeof(tAlarmEntry.m_achTime));
        memcpy( tAlarmEntry.m_achObject, m_atAlarmTable[wTableIndex].m_achObject, sizeof(tAlarmEntry.m_achObject) );
	}
    else
    {
        bRet = FALSE;
        Agtlog(LOG_VERBOSE, "Get Alarm Info Fail.\n");
    }

    return bRet;
}

/*=============================================================================
  �� �� ���� GetErrorString
  ��    �ܣ� ȡ������Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� s8* pszErrStr
             u16 wLen
  �� �� ֵ�� void 
=============================================================================*/
BOOL32 CAlarmProc::GetErrorString(s8* pszErrStr, u16 wLen)
{
    if( 0 == m_wErrorNum )
	{
        Agtlog(LOG_WARN, "[GetErrorString] m_wErrorNum == 0, no error string !\n");
		return FALSE;
	}
	
	u16 wLength = 0;
    u16 wSrcLen = wLen; // ����ԭ��ֵ

    s8  aszErrStr[MAX_ERROR_NUM * MAX_ERROR_STR_LEN];
    memset(aszErrStr, '\0', sizeof(aszErrStr));

    for(u8 byLop = 0; byLop < m_wErrorNum; byLop++)
    {
        wLength = strlen(m_achCfgErr[byLop]);
        if(wLen < wLength)
        {
            return FALSE;
        }
        wLen = wLen - wLength;
        sprintf(aszErrStr, "%s\n%s", aszErrStr, m_achCfgErr[byLop]);
    }
	
	if( 0 != m_wErrorNum )
    {
		strncpy(pszErrStr, aszErrStr, wSrcLen);
	}
    return TRUE;
}
/*=============================================================================
  �� �� ���� ProcReadError
  ��    �ܣ� �������ļ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byErrorId
  �� �� ֵ�� void 
=============================================================================*/
void CAlarmProc::ProcReadError(u8 byErrorId)
{
	if( MAX_ERROR_NUM < m_wErrorNum ) // Խ�籣��
	{
		m_wErrorNum = 0; 
	}

    switch( byErrorId )
	{
    case ERR_AGENT_READSYSCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_SYS, strlen(ERR_STRING_SYS)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READNETCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_NETWORK, strlen(ERR_STRING_NETWORK)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READLOCALCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_LOACL, strlen(ERR_STRING_LOACL)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READTRAPCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_TRAP, strlen(ERR_STRING_TRAP)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READBOARDCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_BOARD, strlen(ERR_STRING_BOARD)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READMIXCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_MIX, strlen(ERR_STRING_MIX)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READTVCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_TVWALL, strlen(ERR_STRING_TVWALL)+1);
        m_wErrorNum++;
        break;
    case ERR_AGENT_READRECORDERCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_RECORDER, strlen(ERR_STRING_RECORDER)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READBASCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_BAS, strlen(ERR_STRING_BAS)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READBASHDCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_BASHD, strlen(ERR_STRING_BASHD)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READVMPCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_VMP, strlen(ERR_STRING_VMP)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READMPWCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_MPW, strlen(ERR_STRING_MPW)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READVMPATTACHCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_VMPATTACH, strlen(ERR_STRING_VMPATTACH)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READPRSCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_PRS, strlen(ERR_STRING_PRS)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READQOSCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_QOS, strlen(ERR_STRING_QOS)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READNETSYSCCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_NETSYSC, strlen(ERR_STRING_NETSYSC)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READDATACONFCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_DATACONF, strlen(ERR_STRING_DATACONF)+1);
        m_wErrorNum++;
        break;
        
    default:
        break;
    }
    return;
}

/*=============================================================================
  �� �� ���� ShowErrorString
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CAlarmProc::ShowErrorString(void)
{
	OspPrintf(TRUE, FALSE, "\t\t The Error String\n");
    for(u8 byLoop = 0; byLoop < MAX_ERROR_NUM; byLoop++)
    {
        OspPrintf(TRUE, FALSE, "[Agent] %s\n", m_achCfgErr[byLoop]);
    }
	return;
}

/*=============================================================================
  �� �� ���� ShowCurAlarm
  ��    �ܣ� ��ʾ��ǰ�澯
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CAlarmProc::ShowCurAlarm( void )
{
	OspPrintf(TRUE, FALSE, "[ShowCurAlarm] m_tMcuPfm.m_wAlarmStamp = %d.\n", m_tMcuPfm.m_wAlarmStamp);
	OspPrintf(TRUE, FALSE, "[ShowCurAlarm] m_dwNowAlarmNo = %d.\n", m_dwNowAlarmNo );
	
	for( u8 byLoop = 0; byLoop < m_dwNowAlarmNo; byLoop++ )
	{
		if( m_atAlarmTable[byLoop].m_bExist )
		{
			OspPrintf(TRUE, FALSE, "No= %d, Type= %d, Code= %d, Object= %d,%d,%d, time= %s\n", 
                                    byLoop, 
			                        m_atAlarmTable[byLoop].m_byObjType,
			                        m_atAlarmTable[byLoop].m_dwAlarmCode,
			                        m_atAlarmTable[byLoop].m_achObject[0],
			                        m_atAlarmTable[byLoop].m_achObject[1],
			                        m_atAlarmTable[byLoop].m_achObject[2],
			                        m_atAlarmTable[byLoop].m_achTime);
		}
	}
    return;
}

/*=============================================================================
  �� �� ���� ShowSemHandle
  ��    �ܣ� ��ʾ�ź���ֵ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CAlarmProc::ShowSemHandle(void)
{
    OspPrintf( TRUE, FALSE, "Brd Manager Handle: 0x%x \n", m_hMcuAlarmTable );
    return;
}
/*lint -restore*/
// END OF FILE
