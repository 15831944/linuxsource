/*****************************************************************************
   ģ����      : Configure Agent
   �ļ���      : ConfAgent.cpp
   ����ļ�    : ConfAgent.h reccfg.ini
   �ļ�ʵ�ֹ���: ��ȡ������Ϣ����
   ����        : 
   �汾        : V3.5  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
******************************************************************************/
#include "ConfAgent.h"

#define LOCAL_IP	"127.0.0.1"

BOOL    bPrintLog = TRUE;
u32     g_adwDevice[MAXNUM_RECORDER_CHNNL+1] = {(u32)0};

TCfgAgent   g_tCfg;
CDBOperate  g_cTDBOperate;       // DB ����

u32	g_adwRecDuration[MAXNUM_RECORDER_CHNNL+1] = {(u32)0};

u16 GetRecChnIndex( u32 dwRecId )
{
    for(u8 byLop = 0; byLop < MAXNUM_RECORDER_CHNNL+1; byLop++ )
    {
        if( g_adwDevice[byLop] == dwRecId )
        {
            return byLop;
        }
    }
    return (u16)NOT_FIND_RECID; // not find the rec id
}


/*====================================================================
    ������      ��GetExecuteProgramPath
    ����        �����ִ�г����·��
    ����ȫ�ֱ�����
    �������˵����path - ָ����·����ָ��
    ����ֵ˵��  ��TRUE/FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    04/12/06    3.6         libo           ����
    06/08/21    4.0         ����        
====================================================================*/
void GetExecuteProgramPath(s8 * path)
{
	TCHAR achExeName[KDV_MAX_PATH] = _T("");

	GetModuleFileName(AfxGetInstanceHandle(), achExeName, sizeof (achExeName));

	CString cstrExePath = achExeName;
	cstrExePath = cstrExePath.Left( cstrExePath.ReverseFind('\\') );    // ����·��
    strcpy(path, cstrExePath);
}

/*====================================================================
    ������	     ��ReadConfigFromFile
	����		 ���������ļ��ж�ȡ���ò���
	����ȫ�ֱ��� ����
    �������˵�� ����
	����ֵ˵��   ���ɹ����� TRUE�����򷵻�FALSE
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    04/12/28    3.6         libo          �޸������ļ���·��
    06/08/28    4.0         ����        ����
====================================================================*/
BOOL TCfgAgent::ReadConfigFromFile()
{
	s8   achBuf[256];
	s8   achHead[256];
	s32 nValue;
	
	BOOL bFlag = TRUE;

    m_bInited = FALSE;
	
	// ����Ӣ����ʾ, zgc, 2007-03-23
	m_tLangId = GetSystemDefaultLangID();
	// Os���Ǽ������İ�
    if( LANGUAGE_CHINESE != m_tLangId )
    {
        m_bOSEn = TRUE;
    }

	if( m_bOSEn )
	{
		sprintf(achHead, "The current configuration file: %s\nSection: %s\n",
		    m_szCfgFilename, SECTION_RECSYS);
	}
	else
	{
		sprintf(achHead, "��ǰ�����ļ� : %s\nSection: %s\n",
		    m_szCfgFilename, SECTION_RECSYS);
	}

	//ID
	if ( !::GetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_PERIEQPID, 0, &nValue ) )
	{
		if( m_bOSEn )
		{
			sprintf( achBuf, "%sKey    :%s\nRead the peripheral ID number failed.\n", achHead, KEY_RECSYS_PERIEQPID);
		}
		else
		{
			sprintf( achBuf, "%sKey    :%s\n��ȡ����ID��ʧ��\n", achHead, KEY_RECSYS_PERIEQPID);
		}
		AfxMessageBox( achBuf );
		return FALSE;
	}
	
	m_byEqpId   = (u8)nValue;

	//Type
	/*if ( !::GetRegKeyInt( RECCFG_FILE, SECTION_RECSYS, KEY_RECSYS_PERIEQPTYPE, 
						  0, &nValue ) )
	{
		sprintf( achBuf ,"%sKey    :%s\n��ȡ�������ͺ�ʧ��",achHead,KEY_RECSYS_PERIEQPTYPE);
		AfxMessageBox( achBuf );
		return FALSE;
	}*/

	m_byEqpType   = EQP_TYPE_RECORDER;//(u8)nValue;

	//Alias
	if ( !::GetRegKeyString( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_PERIEQPALIAS, 
							 NULL, m_szAlias, sizeof( m_szAlias ) ) )
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nRead the alias failed.",achHead,KEY_RECSYS_PERIEQPALIAS);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n��ȡ����ʧ��",achHead,KEY_RECSYS_PERIEQPALIAS);
		}
			
		AfxMessageBox( achBuf );
		return FALSE;
	}
	
    // ������
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD( 2, 2 );
    int err = WSAStartup( wVersionRequested, &wsaData );

    if( !::GetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MACHINENAME, "\0", m_szMachineName, sizeof(m_szMachineName)))
    {
        /*
		if( m_bOSEn )
        {
			sprintf(m_szMachineName, "Please set the recorder's name.");
		}
		else
		{
			sprintf(m_szMachineName, "�����û�����");
		}
        */
        gethostname(m_szMachineName, 32);
        
        // AfxMessageBox(szMachineName);
        // return FALSE;
    }
    else
    {
        if ( strlen(m_szMachineName) == 0 || strcmp(m_szMachineName, "KEDACOM") == 0 )
        {
            gethostname(m_szMachineName, 32);
        }
    }

	//¼��IP��ַ
	if ( !::GetRegKeyString( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_IPADDR, 
						  0, m_achLocalIP ,sizeof(m_achLocalIP) ) )
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nRead the local host's IP failed.",achHead,KEY_RECSYS_IPADDR);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n��ȡ����IP��ַʧ��",achHead,KEY_RECSYS_IPADDR);
		}
		AfxMessageBox( achBuf );
		return FALSE;
	}


	bFlag = IsLegality( m_achLocalIP );
	if(bFlag)
	{
		m_dwRecIpAddr = inet_addr(m_achLocalIP);
	}
	else
	{
		m_dwRecIpAddr = inet_addr(LOCAL_IP);
	}

    if ( m_dwRecIpAddr == inet_addr(LOCAL_IP) )
    {
        hostent *tHost = gethostbyname(m_szMachineName);
        if (tHost != NULL)
        {
            if ( tHost->h_addr_list[0] )
            {                 
                memcpy (&m_dwRecIpAddr, tHost->h_addr_list[0],tHost->h_length);
            }
        }            
    }

	//¼���������ʼ�˿ں�
	if (!::GetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_RCVPORT, REC_EQP_STARTPORT, &nValue))
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nRead the receive port failed.",achHead,KEY_RECSYS_RCVPORT);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n��ȡ¼�������ʼ�˿ں�ʧ��",achHead,KEY_RECSYS_RCVPORT);
		}
		
		AfxMessageBox( achBuf );
		return FALSE;
	}
	
	bFlag = IsLegality(nValue);
	if(bFlag)
	{
		m_wRecStartPort = (u16)nValue;
	}
	else
	{
		m_wRecStartPort = REC_EQP_STARTPORT;
		//ErrorMsg( 1 );
	}
	
	//����ͨ����ʼ�˿ں�
	if (!::GetRegKeyInt(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_PLAYPORT, REC_EQP_STARTPORT, &nValue))
	{
		if( m_bOSEn )
		{
			sprintf(achBuf, "%sKey    :%s\nRead the play port failed.", achHead, KEY_RECSYS_PLAYPORT);
		}
		else
		{
			sprintf(achBuf, "%sKey    :%s\n��ȡ������ʼ�˿ں�ʧ��", achHead, KEY_RECSYS_PLAYPORT);
		}
		
		AfxMessageBox(achBuf);
		return FALSE;
	}

	bFlag = IsLegality(nValue);
	if(bFlag == TRUE)
	{
		m_wPlayStartPort = (u16)nValue;

	}
	else
	{
		m_wPlayStartPort = REC_EQP_STARTPORT;
	}

	// rec path
	if ( !::GetRegKeyString( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_RECFILEPATH, 
						 "\0", m_szRecordPath, sizeof( m_szRecordPath ) ) )
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nRead the path of saving record file failed.",achHead, KEY_RECSYS_RECFILEPATH);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n��ȡ¼��洢·��ʧ��",achHead, KEY_RECSYS_RECFILEPATH);
		}
		
		AfxMessageBox( achBuf );
		return FALSE;
	}

	bFlag = IsLegality( m_szRecordPath );
    if (bFlag)
    {
        // ���Դ�����Ŀ¼
        ::CreateDirectory( m_szRecordPath, NULL );
    }
	
	// MCU ID
    /*
	if ( !::GetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUID, 
						  0, &nValue ) )
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nRead the MCU ID failed.",achHead,KEY_RECSYS_MCUID);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n��ȡMCU IDʧ��",achHead,KEY_RECSYS_MCUID);
		}
		
		AfxMessageBox( achBuf );
		return FALSE;
	}
    */
	m_wMcuId = LOCAL_MCUID;

    /*
	bFlag = IsLegality( nValue );
	if(bFlag == FALSE)
	{
		m_wMcuId = LOCAL_MCUID;
	}
    */

	// MCU IP ��ַ
	if ( !::GetRegKeyString( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUIPADDR, 
						  "\0", achBuf,sizeof(achBuf) ) )
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nRead the MCU IP failed.",achHead,KEY_RECSYS_MCUIPADDR);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n��ȡMCU IP��ַʧ��",achHead,KEY_RECSYS_MCUIPADDR);
		}
		AfxMessageBox( achBuf );
		return FALSE;
	}
	m_dwMcuIpAddr = inet_addr(achBuf);

	bFlag = IsLegality( achBuf );	// ���Mcu IpΪ�գ�����Ϊ�Ǳ��ص�ַ
	if(!bFlag)
	{
		m_dwMcuIpAddr = inet_addr( LOCAL_IP );
	}

	// MCU IP Connect Port
	if ( !::GetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUCONNPORT, 
						  0, &nValue ) )
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nRead the MCU connecting port failed.",achHead,KEY_RECSYS_MCUCONNPORT);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n��ȡMCU ���Ӷ˿ں�ʧ��",achHead,KEY_RECSYS_MCUCONNPORT);
		}
		
		nValue = MCU_LISTEN_PORT;
		return FALSE;
	}
	m_wMcuConnPort = (u16)nValue;
	
	bFlag = IsLegality(nValue);
	if(!bFlag)
	{
		m_wMcuConnPort = MCU_LISTEN_PORT;
	}
	
/*
    // MCU ID B
	if ( !::GetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUIDB, 
						  0, &nValue ) )
	{
		sprintf( achBuf ,"%sKey    :%s\n��ȡMcu IDʧ��",achHead,KEY_RECSYS_MCUIDB);
	}
	wMcuIdB = (u8)nValue;

	bFlag = IsLegality( nValue );
	if(bFlag == FALSE)
	{
		wMcuIdB = LOCAL_MCUID;
	}

	
	// MCUB IP ��ַ
	if ( !::GetRegKeyString( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUIPADDRB, 
						  "\0", achBuf,sizeof(achBuf) ) )
	{
		sprintf( achBuf ,"%sKey    :%s\n��ȡMcu IP��ַʧ��",achHead,KEY_RECSYS_MCUIPADDRB);
        dwMcuIpAddrB = 0;
	}
    else
    {
        dwMcuIpAddrB = inet_addr(achBuf);
    }

	// MCUB Connect Port
	if ( !::GetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUCONNPORTB, 
						  0, &nValue ) )
	{
		sprintf( achBuf ,"%sKey    :%s\n��ȡMcu ���Ӷ˿ں�ʧ��",achHead,KEY_RECSYS_MCUCONNPORTB);
        nValue = MCU_LISTEN_PORT;
	}
	wMcuConnPortB = (u16)nValue;
	
    if(dwMcuIpAddr == dwMcuIpAddrB) // ����
    {
        dwMcuIpAddrB = 0;
        wMcuConnPortB = 0;
    }
	 
    if(0 == dwMcuIpAddr && 0 == dwMcuIpAddrB)
    {
        // AfxMessageBox("The McuA and McuB's Ip are all 0, refuse\n");
        // return FALSE;
    }*/ 

	// Play channels
	if ( !::GetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_PLAYCHNUM, 
						  0, &nValue ) )
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nRead the play channel number failed.",achHead,KEY_RECSYS_PLAYCHNUM);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n��ȡ����ͨ������ʧ��",achHead,KEY_RECSYS_PLAYCHNUM);
		}
		AfxMessageBox( achBuf );
		return FALSE;
	}
	m_byPlayChnNum = (u8)nValue;
	bFlag = IsLegality(nValue);
	if(bFlag == FALSE)
	{
		m_byPlayChnNum = 0;
	}

	// Recorder channels
	if (!::GetRegKeyInt(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_RECCHNUM, 0, &nValue))
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nRead the record channel number failed.",achHead,KEY_RECSYS_RECCHNUM);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n��ȡ¼��ͨ������ʧ��",achHead,KEY_RECSYS_RECCHNUM);
		}
		
		AfxMessageBox( achBuf );
		return FALSE;
	}
	m_byRecChnNum = (u8)nValue;
	bFlag = IsLegality(nValue);
	if(bFlag == FALSE)
	{
		m_byRecChnNum = 0;
	}

    // ���˿�֮���Ƿ����㹻�ļ��
    BOOL32 bEnough = TRUE;
    if(m_wPlayStartPort < m_wRecStartPort)
    {
        bEnough = ( (m_wRecStartPort - m_wPlayStartPort) >= (m_byPlayChnNum*PORTSPAN) ) ;
        if (!bEnough)
        {
            // ���˵��㹻�ռ�
            m_wPlayStartPort = m_wRecStartPort - m_byPlayChnNum*PORTSPAN;
        }
    }
    else
    {
        bEnough = ( (m_wPlayStartPort - m_wRecStartPort) >= (m_byRecChnNum*PORTSPAN) );
        if (!bEnough)
        {
            // ���˵��㹻�ռ�
            m_wRecStartPort = m_wPlayStartPort - m_byRecChnNum*PORTSPAN;
        }
    }


	m_byTotalChnNum = m_byRecChnNum + m_byPlayChnNum;
	if ((m_byTotalChnNum ==0) || ( m_byTotalChnNum >MAXNUM_RECORDER_CHNNL))
	{
		if( m_bOSEn )
		{
			sprintf(achBuf,"The largest number of the play and record channels of the recorder is %d��\nThe configuration requre the %d play channels, the %d record channels��\nWill start recorder as the minist configuration, please modify the configuration.",
			MAXNUM_RECORDER_CHNNL, m_byPlayChnNum, m_byRecChnNum );
		}
		else
		{
			sprintf(achBuf,"��¼��������¼��ͨ���������Ϊ%d��\n����Ҫ�����ͨ��%d ,¼��ͨ��%d��\n��������С�������������޸����á�",
			MAXNUM_RECORDER_CHNNL, m_byPlayChnNum, m_byRecChnNum );
		}
		AfxMessageBox( achBuf );

        // Ĭ����������
		m_byRecChnNum = 3;
        m_byPlayChnNum = 3;
        m_byTotalChnNum = m_byRecChnNum + m_byPlayChnNum;
	}

	memset(m_achMSInfo, 0, sizeof(m_achMSInfo));

	// �Ƿ�֧�ַ���
	if (!::GetRegKeyInt(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_ISPUBLISH, 0, &nValue))
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nRead wether support the publishing failed.",achHead,KEY_RECSYS_RECCHNUM);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n��ȡ�Ƿ�֧�ַ���ʧ��",achHead,KEY_RECSYS_RECCHNUM);
		}
		AfxMessageBox( achBuf );
		return FALSE;
	}
	m_bMediaServer = (BOOL)nValue;
	
	// ���ݿ�IP��ַ
	if ( !::GetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_DBHOSTIP, 
		"\0", m_achDBHostIP, sizeof(m_achDBHostIP)))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo, "Get the database's IP failed. Key=\"DBHostIP\"\n");
		}
		else
		{
			strcat(m_achMSInfo, "��ȡ���ݿ�IP��ַʧ�� Key=\"DBHostIP\"\n");
		}
		//bMediaServer = FALSE;
		AfxMessageBox(m_achMSInfo);
		return FALSE;
	}
	if(m_achDBHostIP[0] >= '0' && m_achDBHostIP[0] <= '9')
	{
		m_dwDBHostIP = inet_addr(m_achDBHostIP);
	}
	else
	{
		m_dwDBHostIP= 0;
	}
	
	bFlag = IsLegality(m_achDBHostIP);
	if((bFlag == FALSE) || (m_dwDBHostIP == 0))
	{
		if( m_bMediaServer )
		{
			if( m_bOSEn )
				AfxMessageBox("The DB Ip can not be Null");
			else
				AfxMessageBox("���ݿ�IP��ַ����Ϊ�գ�");
			
		}
	}
	
	//���ݿ��û���
	if (!::GetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_USERNAME,
		 "\0", m_achDBUsername, sizeof(m_achDBUsername)))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo ,"Get username failed. Key=\"DBUserName\"\n");
		}
		else
		{
			strcat(m_achMSInfo ,"��ȡ�û���ʧ�� Key=\"DBUserName\"\n");
		}
		AfxMessageBox( achBuf );
		return FALSE;
	}
	
	
	//���ݿ��û�����
	if (!::GetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_PASSWORD, 
		 "\0", m_achDBPassword, sizeof(m_achDBPassword)))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo ,"Get the database user password failed. Key=\"DBPassWord\"\n");
		}
		else
		{
			strcat(m_achMSInfo ,"��ȡ���ݿ��û�����ʧ�� Key=\"DBPassWord\"\n");
		}
		AfxMessageBox( achBuf );
		return FALSE;
	}		
	
	//FTP ·��
	if (!::GetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_VODMMSPATH, 
		 "\0", m_achVodFtpPatch, sizeof(m_achVodFtpPatch)))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo, "Get the FTP path failed. Key=\"VodMMSPath\"\n");
		}
		else
		{
			strcat(m_achMSInfo, "FTP ·��ʧ�� Key=\"VodMMSPath\"\n");
		}
		AfxMessageBox(achBuf);
		return FALSE;
	}
	
	//FTP�û���
	if (!::GetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MMSUSERNAME, 
		 "\0", m_achFTPUsername, sizeof(m_achFTPUsername)))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo, "Get the FTP username failed. Key=\"MMSUserName\"\n");
		}
		else
		{
			strcat(m_achMSInfo, "��ȡFTP�û���ʧ�� Key=\"MMSUserName\"\n");
		}	
	}
	
	//FTP����
	if (!::GetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MMSPASSWORD, 
		 "\0", m_achFTPPassword, sizeof(m_achFTPUsername)))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo, "Get the FTP password failed. Key=\"MMSPassWord\"\n");
		}
		else
		{
			strcat(m_achMSInfo, "��ȡFTP����ʧ�� Key=\"MMSPassWord\"\n");
		}
	}


    // �Ƿ��������ж����Ĭ������[06/07/27]
    if (!::GetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MULTIINSTANCE,
		                 1, &nValue ))
    {
        nValue = 1;
    }
    m_bMultiInst = nValue;

// [pengjie 2010/9/26] �д��ļ����û�֧��
    // ����¼��������¼���ļ���С������������ļ�����λ��M
    if (!::GetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, "MaxRecFileLen",
		1200, &nValue ))
    {
		::SetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, "MaxRecFileLen", 1200);
        nValue = 1200;
    }
    m_dwMaxRecFileLen = nValue * 1024 * 1024;
// End

	s8 achProfileName [KDV_MAX_PATH] = {0};

    GetExecuteProgramPath(achProfileName);
    strcat(achProfileName, RECDBGCFG_FILE);

#ifdef REC_CPPUNIT_TEST
	//Telnet Port
	if ( !::GetRegKeyInt( achProfileName, SECTION_RECSYS,
		                  KEY_RECSYS_TELNETPORT, 0, &nValue ) )
	{
		m_wTelnetPort = (u16)nValue;	//�����ļ���û��Telnet port������Ϊ0
	}

	m_bCppUnitTest = TRUE;
	//��ȡ CPP UNIT ����ʱ���Է���IP��ַ
	if ( !::GetRegKeyString( achProfileName,
		                     "RecorderDebugTestSystem", 
		                     "CPPUnitTestServerIpAddr",
		                     "", achBuf, sizeof(achBuf) ) )
	{
		m_bCppUnitTest = FALSE;
	}
	else
	{
		m_dwCppUnitIpAddr = inet_addr(achBuf);
	}

	//��ȡ CPP UNIT ����ʱ���Է���˿�
	if ( !::GetRegKeyInt( achProfileName, 
		                  "RecorderDebugTestSystem", 
						  "CPPUnitTestServerPort" , 
						  0, &nValue ) )
	{
		m_bCppUnitTest = FALSE;
	}
	else
	{
		m_wCppUnitPort = (u16)nValue;
	}
#endif

	//[6/1/2012 zhangli]  ��֡ʱ�Ƿ�д�ļ�
	if (!::GetRegKeyInt(achProfileName, SECTION_RECORDERDEBUGINFO, "IsWriteAsfWhenLoseFrame", 0, &nValue))
    {
		nValue = 0;
    }
    m_bIsWriteAsfWhenLoseFrame = nValue;
    
	//[6/1/2012 zhangli]  ��֡ʱ�Ƿ�����ؼ�֡
	if (!::GetRegKeyInt(achProfileName, SECTION_RECORDERDEBUGINFO, "IsNeedKeyFrameWhenLoseFrame", 0, &nValue))
    {
		nValue = 0;
    }
    m_bIsNeedKeyFrameWhenLoseFrame = nValue;
	
	//[2012/3/19 zhangli] ֡��������Ϣ
	::GetRegKeyInt(achProfileName, SECTION_RECORDERDEBUGINFO, "FrameSizeInfoNum", 0, &nValue);
	
	s8 achSection[32] = {0};
	s8    achReturn[MAX_VALUE_LEN + 1];
	s8    *pchToken = NULL;
	s8 achSeps[] = " \t";
	TFrameSizeInfo tFrameSizeInfo;
	
	for (u8 byLoop = 0; byLoop < nValue; ++byLoop)
	{
		memset(achSection, 0, sizeof(achSection));
		
		sprintf(achSection, "FrameSizeInfo%d", byLoop);
		
		::GetRegKeyString(achProfileName, SECTION_RECORDERDEBUGINFO, achSection, "", achReturn, MAX_VALUE_LEN+1);
		
		//media type
		pchToken = strtok(achReturn, achSeps);
        if (NULL == pchToken)
        {
            LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, "Wrong profile while reading %s!\n",achSection);
            continue;
        }
        else
        {
            tFrameSizeInfo.m_byMediaType = atoi(pchToken);
        }
		
		//format
		pchToken = strtok(NULL, achSeps);
        if (NULL == pchToken)
        {
            LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, "Wrong profile while reading %s!\n",achSection);
            continue;
        }
        else
        {
            tFrameSizeInfo.m_dwFrameSize = atoi(pchToken);
        }
		
		SetFrameSizeInfo(tFrameSizeInfo);
	}

	//[6/1/2012 zhangli]  ��Ƶbuff��С��Ĭ��Ϊ16
	if (!::GetRegKeyInt(achProfileName, SECTION_RECORDERDEBUGINFO, "audioBuffSize", MAX_BUF_NUM, &nValue))
    {
		nValue = MAX_BUF_NUM;
    }
    m_byAudioBuffSize = nValue;

	//[6/1/2012 zhangli]  ��Ƶbuff��С��Ĭ��Ϊ16
	if (!::GetRegKeyInt(achProfileName, SECTION_RECORDERDEBUGINFO, "videoBuffSize", MAX_BUF_NUM, &nValue))
    {
		nValue = MAX_BUF_NUM;
    }
    m_byVideoBuffSize = nValue;

	//[6/1/2012 zhangli]  ˫��buff��С��Ĭ��Ϊ16
	if (!::GetRegKeyInt(achProfileName, SECTION_RECORDERDEBUGINFO, "dsBuffSize", MAX_BUF_NUM, &nValue))
    {
		nValue = MAX_BUF_NUM;
    }
    m_byDsBuffSize = nValue;

	//[6/1/2012 zhangli]  ���ʣ��buff��С��Ĭ��Ϊ10
	if (!::GetRegKeyInt(achProfileName, SECTION_RECORDERDEBUGINFO, "leftBuffSize", 10, &nValue))
    {
		nValue = 10;
    }
    m_byLeftBuffSize = nValue;

	if (!::GetRegKeyInt(achProfileName, SECTION_RECORDERDEBUGINFO, "minSizeForBroadcast", 512, &nValue))
    {
		nValue = 512;
    }
	
	m_wMinSizeForBroadcast = nValue;

    m_bInited = TRUE;
	return m_bInited;

}

/*====================================================================
������      :SetFrameSizeInfo
����        :����֡��������Ϣ
�㷨ʵ��    :
����ȫ�ֱ���:
�������˵��: u8 byMediaType ý������
u32 dwFrameSize֡��
����ֵ˵��  :TRUE or FLASE
----------------------------------------------------------------------
�޸ļ�¼    :
��  ��      �汾        �޸���        �޸�����
2012/06/17	4.7			zhangli		  create
====================================================================*/
BOOL32 TCfgAgent::SetFrameSizeInfo(TFrameSizeInfo &tFrameSizeInfo)
{
	if (tFrameSizeInfo.m_byMediaType == 0 || tFrameSizeInfo.m_dwFrameSize == 0)
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_RPCTRL, "SetFrameSizeInfo parma error, byMediaType:%d, dwFrameSize:%d\n",
			tFrameSizeInfo.m_byMediaType, tFrameSizeInfo.m_dwFrameSize);
		
		return FALSE;
	}
	
	for (u8 byLoop = 0; byLoop < MAX_AUDIO_TYPE_NUM; ++byLoop)
	{
		if (m_atFrameSize[byLoop].m_byMediaType == 0 || m_atFrameSize[byLoop].m_dwFrameSize == 0)
		{
			memcpy(&m_atFrameSize[byLoop], &tFrameSizeInfo, sizeof(TFrameSizeInfo));
			return TRUE;
		}
	}
	return FALSE;
}

/*====================================================================
    ������	     ��Write
	����		 �������ò���д�������ļ�
	����ȫ�ֱ��� ����
    �������˵�� ����
	����ֵ˵��   ���ɹ����� TRUE�����򷵻�FALSE
----------------------------------------------------------------------
	�޸ļ�¼    ��
	��  ��      �汾        �޸���        �޸�����
    04/12/31    3.6         libo          create
	06/01/18	4.0			�ű���		  adapt to read only config file
====================================================================*/
BOOL TCfgAgent::WriteConfigIntoFile()
{
	s8   achBuf[256];
	s8   achHead[256];
	s32 nValue;
    struct in_addr in;

	if( m_bOSEn )
	{
		sprintf(achHead, "The current configuration file : %s\nSection: %s\n",
		    m_szCfgFilename, SECTION_RECSYS);
	}
	else
	{
		sprintf(achHead, "��ǰ�����ļ� : %s\nSection: %s\n",
		    m_szCfgFilename, SECTION_RECSYS);
	}

   
	
	//�ı������ļ�������ΪNormal, �Ա����޸ĺ�Ĳ���. 
	CFile* pFile = NULL;
	CFileStatus cFileOrignalStatus;
	memset( &cFileOrignalStatus, 0, sizeof(CFileStatus) );
	CFileStatus cFileChangedStatus;
	memset( &cFileChangedStatus, 0, sizeof(CFileStatus) );
	pFile->GetStatus( m_szCfgFilename, cFileOrignalStatus );
	pFile->GetStatus( m_szCfgFilename, cFileChangedStatus );

	if ( 0x00 != cFileOrignalStatus.m_attribute ) 
	{
		cFileChangedStatus.m_attribute = 0x00;
		pFile->SetStatus(m_szCfgFilename, cFileChangedStatus);
	}
	
	//¼��IP��ַ
    in.s_addr = m_dwRecIpAddr;
    sprintf(m_achLocalIP, "%s", inet_ntoa(in));
	if (!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_IPADDR, m_achLocalIP))
	{
		if( m_bOSEn )
		{
			sprintf(achBuf ,"%sKey    :%s\nWrite the record IP failed.", achHead, KEY_RECSYS_IPADDR);
		}
		else
		{
			sprintf(achBuf ,"%sKey    :%s\nд¼��IP��ַʧ��", achHead, KEY_RECSYS_IPADDR);
		}
		AfxMessageBox(achBuf);
		return FALSE;
	}

    //�������
	if (!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_PERIEQPALIAS, m_szAlias))
	{
		if( m_bOSEn )
		{
			sprintf(achBuf, "%sKey    :%s\nWrite the alias failed.",achHead,KEY_RECSYS_PERIEQPALIAS);
		}
		else
		{
			sprintf(achBuf, "%sKey    :%s\nд����ʧ��",achHead,KEY_RECSYS_PERIEQPALIAS);
		}
		AfxMessageBox(achBuf);
		return FALSE;
	}
	
	//����ID
    nValue = (s32)m_byEqpId;
	if (!::SetRegKeyInt(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_PERIEQPID, nValue))
	{
		if( m_bOSEn )
		{
			sprintf(achBuf, "%sKey    :%s\nWrite the peripheral ID failed.\n", achHead, KEY_RECSYS_PERIEQPID);
		}
		else
		{
			sprintf(achBuf, "%sKey    :%s\nд����ID��ʧ��\n", achHead, KEY_RECSYS_PERIEQPID);
		}
		AfxMessageBox(achBuf);
		return FALSE;
	}

	//��������---�̶�ֵ
	// m_byEqpType = EQP_TYPE_RECORDER;	


	//¼���������ʼ�˿ں�
    nValue = (s32)m_wRecStartPort;
	if (!::SetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_RCVPORT, nValue))
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nWrite the receive port failed.",achHead,KEY_RECSYS_RCVPORT);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\nд¼�������ʼ�˿ں�ʧ��",achHead,KEY_RECSYS_RCVPORT);
		}
		AfxMessageBox(achBuf);
		return FALSE;
	}	
    // ������ʼ�˿�
    nValue = m_wPlayStartPort;
    if (!::SetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_PLAYPORT, nValue))
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nWrite the play port failed.",achHead, KEY_RECSYS_PLAYPORT);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\nд���������ʼ�˿ں�ʧ��",achHead, KEY_RECSYS_PLAYPORT);
		}		
		AfxMessageBox(achBuf);
		return FALSE;
	}

	//rec path
	if (!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_RECFILEPATH, m_szRecordPath))
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nWrite the path of saving record file failed.",achHead,KEY_RECSYS_IPADDR);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\nд¼��洢·��ʧ��",achHead,KEY_RECSYS_IPADDR);
		}
		AfxMessageBox(achBuf);
		return FALSE;
	}

    /*
	//MCU ID
    nValue = (s32)m_wMcuId;
	if (!::SetRegKeyInt(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUID, nValue))
	{
		if( m_bOSEn )
		{
			sprintf(achBuf ,"%sKey    :%s\nWrite the MCU ID failed.",achHead,KEY_RECSYS_MCUID);
		}
		else
		{
			sprintf(achBuf ,"%sKey    :%s\nдMCU IDʧ��",achHead,KEY_RECSYS_MCUID);
		}
		AfxMessageBox(achBuf);
		return FALSE;
	}
    */
    
	/*
    if(dwMcuIpAddr == dwMcuIpAddrB) // ����
    {
        dwMcuIpAddrB = 0;
        wMcuConnPortB = 0;
    }
    
    if(0 == dwMcuIpAddr && 0 == dwMcuIpAddrB)
    {
        AfxMessageBox("The McuA and McuB's Ip are all 0, refuse\n");
        return FALSE;
    }*/

	//MCU IP ��ַ
    in.s_addr = m_dwMcuIpAddr;
    sprintf(achBuf, "%s", inet_ntoa(in));
	if (!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUIPADDR, achBuf))
	{
		if( m_bOSEn )
		{
			sprintf(achBuf, "%sKey    :%s\nWrite the MCU IP failed.", achHead, KEY_RECSYS_MCUIPADDR);
		}
		else
		{
			sprintf(achBuf, "%sKey    :%s\nдMCU IP��ַʧ��", achHead, KEY_RECSYS_MCUIPADDR);
		}
		AfxMessageBox(achBuf);
		return FALSE;
	}

	//MCU IP Connect Port
    nValue = (s32)m_wMcuConnPort;
	if (!::SetRegKeyInt(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUCONNPORT, nValue))
	{
		if( m_bOSEn )
		{
			sprintf(achBuf, "%sKey    :%s\nWrite the MCU connecting port failed.", achHead, KEY_RECSYS_MCUCONNPORT);
		}
		else
		{
			sprintf(achBuf, "%sKey    :%s\nдMCU ���Ӷ˿ں�ʧ��", achHead, KEY_RECSYS_MCUCONNPORT);
		}
		AfxMessageBox(achBuf);
		return FALSE;
	}

    /*
    //MCU ID
    nValue = (s32)wMcuIdB;
	if (!::SetRegKeyInt(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUIDB, nValue))
	{
		sprintf(achBuf ,"%sKey    :%s\nдMCU IDʧ��",achHead, KEY_RECSYS_MCUIDB);
		AfxMessageBox(achBuf);
		return FALSE;
	}
	
	//MCU IP ��ַ
    if( 0 != dwMcuIpAddrB)
    {
        in.s_addr = dwMcuIpAddrB;
        sprintf(achBuf, "%s", inet_ntoa(in));
    }
    else
    {
        memcpy(achBuf, "0", sizeof("0"));
    }

	if (!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUIPADDRB, achBuf))
	{
		sprintf(achBuf, "%sKey    :%s\nдMCU IP��ַʧ��", achHead, KEY_RECSYS_MCUIPADDRB);
		AfxMessageBox(achBuf);
		// return FALSE;
	}

	//MCU IP Connect Port
    nValue = (s32)wMcuConnPortB;
	if (!::SetRegKeyInt(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUCONNPORTB, nValue))
	{
		sprintf(achBuf, "%sKey    :%s\nдMCU ���Ӷ˿ں�ʧ��", achHead, KEY_RECSYS_MCUCONNPORTB);
		AfxMessageBox(achBuf);
		// return FALSE;
	}
	*/
	

	m_byTotalChnNum = m_byRecChnNum + m_byPlayChnNum;
	if ((m_byTotalChnNum == 0) || (m_byTotalChnNum > MAXNUM_RECORDER_CHNNL))
	{
		if( m_bOSEn )
		{
			sprintf(achBuf,"The largest number of the play and record channels of the recorder is %d��\nThe configuration requre the %d play channels, the %d record channels��\nWill start recorder as the minist configuration, please modify the configuration.",
			MAXNUM_RECORDER_CHNNL, m_byPlayChnNum, m_byRecChnNum );
		}
		else
		{
			sprintf(achBuf,"��¼��������¼��ͨ���������Ϊ%d��\n����Ҫ�����ͨ��%d ,¼��ͨ��%d��\n��������С�������������޸����á�",
			MAXNUM_RECORDER_CHNNL, m_byPlayChnNum, m_byRecChnNum );
		}
		AfxMessageBox( achBuf );

        // Ĭ����������
		m_byRecChnNum = 3;
        m_byPlayChnNum = 3;
        m_byTotalChnNum = m_byRecChnNum + m_byPlayChnNum;
	}
    
	//Play channels
    nValue = (s32)m_byPlayChnNum;
	if (!::SetRegKeyInt(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_PLAYCHNUM, nValue))
	{
		if( m_bOSEn )
		{
			sprintf(achBuf, "%sKey    :%s\nWrite the play channel number failed.", achHead, KEY_RECSYS_PLAYCHNUM);
		}
		else
		{
			sprintf(achBuf, "%sKey    :%s\nд����ͨ������ʧ��", achHead, KEY_RECSYS_PLAYCHNUM);
		}
		AfxMessageBox(achBuf);
		return FALSE;
	}

	//Recorder channels
    nValue = (s32)m_byRecChnNum;
	if (!::SetRegKeyInt(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_RECCHNUM, nValue))
	{
		if( m_bOSEn )
		{
			sprintf(achBuf, "%sKey    :%s\nWrite the record channel number failed.", achHead, KEY_RECSYS_RECCHNUM);
		}
		else
		{
			sprintf(achBuf, "%sKey    :%s\nд¼��ͨ������ʧ��", achHead, KEY_RECSYS_RECCHNUM);
		}
		AfxMessageBox(achBuf);
		return FALSE;
	}

    // ������
    if(!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MACHINENAME, m_szMachineName))
    {
		if( m_bOSEn )
		{
			sprintf(m_szMachineName, "Write the local recorder name failed.");
		}
		else
		{
			sprintf(m_szMachineName, "д���ػ�����ʧ��");
		}
        AfxMessageBox(m_szMachineName);
        // return FALSE;
    }
    
    //�Ƿ�֧�ַ�������
    nValue = (s32)m_bMediaServer;
    if (!::SetRegKeyInt(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_ISPUBLISH, nValue))
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nWrite wether support the publishing failed.", achHead, KEY_RECSYS_ISPUBLISH);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\nд�Ƿ�֧�ַ���ʧ��", achHead, KEY_RECSYS_ISPUBLISH);
		}
		AfxMessageBox( achBuf );
		return FALSE;
	}

	//���ݿ�IP��ַ
    in.s_addr = m_dwDBHostIP;
    sprintf(m_achDBHostIP, "%s", inet_ntoa(in));
	if (!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_DBHOSTIP, m_achDBHostIP))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo, "Write the database IP failed. Key=\"DBHostIP\"\n");
		}
		else
		{
			strcat(m_achMSInfo, "д���ݿ�IP��ַʧ�� Key=\"DBHostIP\"\n");
		}
		AfxMessageBox(m_achMSInfo);
		return FALSE;
	}	    

	//���ݿ��û���
	if (!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_USERNAME, m_achDBUsername))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo ,"Write the username failed. Key=\"DBUserName\"\n");
		}
		else
		{
			strcat(m_achMSInfo ,"д�û���ʧ�� Key=\"DBUserName\"\n");
		}
		AfxMessageBox(m_achMSInfo);
		return FALSE;
	}

	//���ݿ��û�����
	if (!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_PASSWORD, m_achDBPassword))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo ,"Write the database user password failed. Key=\"DBPassWord\"\n");
		}
		else
		{
			strcat(m_achMSInfo ,"д���ݿ��û�����ʧ�� Key=\"DBPassWord\"\n");
		}
		AfxMessageBox(m_achMSInfo);
		return FALSE;
	}		
	
	//FTP ·��
	if (!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_VODMMSPATH, m_achVodFtpPatch))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo, "Write the FTP path failed. Key=\"VodFtpPath\"\n");
		}
		else
		{
			strcat(m_achMSInfo, "дFTP ·��ʧ�� Key=\"VodFtpPath\"\n");
		}
		AfxMessageBox(m_achMSInfo);
		return FALSE;
	}
    
	//FTP�û���
	if (!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MMSUSERNAME, m_achFTPUsername))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo, "Write the FTP username failed. Key=\"FTPUserName\"\n");
		}
		else
		{
			strcat(m_achMSInfo, "дFTP�û���ʧ�� Key=\"FTPUserName\"\n");
		}
		AfxMessageBox(m_achMSInfo);
		return FALSE;
	}
	
	//FTP����
	if (!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MMSPASSWORD, m_achFTPPassword))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo, "Write the FTP password failed. Key=\"FTPPassWord\"\n");
		}
		else
		{
			strcat(m_achMSInfo, "дFTP����ʧ�� Key=\"FTPPassWord\"\n");
		}
		AfxMessageBox(m_achMSInfo);
		return FALSE;
	}

    /*
	//�ָ�ԭ�����ļ�����
	if ( 0x00 != cFileOrignalStatus.m_attribute )
	{
		pFile->SetStatus( m_szCfgFilename, cFileOrignalStatus );
	}
    */
    
	return TRUE;
}

// �жϲ����Ƿ�Ϸ�
BOOL TCfgAgent::IsLegality(s32 nNum )
{
	if( nNum < 0 )
	{
		ErrorMsg( 1 );
		return FALSE;

	}
	else
	{
		ErrorMsg( 0 );
		return TRUE;
	}

}

BOOL TCfgAgent::IsLegality(s8* aChar)
{
	s32 nLeng = -1;
	nLeng = strlen(aChar);
	if(nLeng == 0)
	{
		ErrorMsg( 4 );
		return FALSE;
	}
	else
	{
		//ErrorMsg(0);
		return TRUE;
	}
}

void TCfgAgent::ErrorMsg(int nNum)
{
	switch(nNum)
	{
	case -1:
		break;

	case 0:
		// RecLog("Success!");
		break;

	case 1:
		RecLog("The Parameter can not be negative");
		break;

	case 2:
		RecLog("The Parameter is more small");
		break;

	case 3:
		RecLog("The Parameter is more Big");
		break;

	case 4:
		RecLog( "The String can not be Null");
		break;

	case 5:
		RecLog("Read the config file Fail");
		break;

	case 6:
		RecLog("Write the config file Fail");
		break;

	case 7:
		RecLog("Call the interface Fail");
		break;

	case 8:
		break;
	case 9:
		break;
	case 10:
		break;
	default:
		break;
	}
}

/*=============================================================================
  �� �� ���� IsLegalityPort
  ��    �ܣ� �ж������˿��Ƿ����㹻�ļ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wPort1
             u16 wPort2
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 TCfgAgent::IsLegalityPort(u16 wPort1, u16 wPort2)
{
    return TRUE;
}

void TCfgAgent::RecLog(char * fmt,...)
{
	char PrintBuf[255];
    int BufLen = 0;
    va_list argptr;
	if( bPrintLog )
	{		  
		BufLen = sprintf(PrintBuf,"[Rec]:"); 
		va_start(argptr, fmt);
		BufLen += vsprintf(PrintBuf+BufLen,fmt,argptr); 
		va_end(argptr); 
		BufLen += sprintf(PrintBuf+BufLen,"\n"); 
		OspPrintf(1,0,PrintBuf); 
	}
}