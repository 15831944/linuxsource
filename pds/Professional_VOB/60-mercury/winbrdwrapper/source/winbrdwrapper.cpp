/******************************************************************************
ģ����  �� simbrdwrapper
�ļ���  �� BoardWrapper.h
����ļ���
�ļ�ʵ�ֹ��ܣ�ģ��BoardWrapperģ������ṩ����Ҫ����
����    ��john
�汾    ��1.0.0.0.0
---------------------------------------------------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
7/0/2006    0.1         john         ����
******************************************************************************/
#include <process.h>
#include "winbrd.h"
#include "mcuconst.h"

u32 g_dwPID = 0;
/*=============================================================================
  �� �� ���� RegGetAdpName
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� s8* lpszAdpName
             u8 byLen
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 RegGetAdpName(s8* lpszAdpName, u16 wLen)
{
    if(NULL == lpszAdpName)
    {
        return FALSE;
    }

#ifdef WIN32
    HKEY hKey, hSubKey, hNdiIntKey;

    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
            "System\\CurrentControlSet\\Control\\Class\\{4d36e972-e325-11ce-bfc1-08002be10318}",
            0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return FALSE;

    DWORD dwIndex = 0;
    DWORD dwBufSize = 256;
    DWORD dwDataType;
    char  szSubKey[256];
    unsigned char szData[256];
    BOOL32 bFlag = FALSE;

    while(ERROR_SUCCESS == RegEnumKeyEx(hKey, dwIndex++, szSubKey, &dwBufSize, NULL, NULL, NULL, NULL))
    {
        if(ERROR_SUCCESS == RegOpenKeyEx(hKey, szSubKey, 0, KEY_READ, &hSubKey))
        { 
            if(ERROR_SUCCESS == RegOpenKeyEx(hSubKey, "Ndi\\Interfaces", 0, KEY_READ, &hNdiIntKey))
            {
                memset(szData, '\0', sizeof(szData));
                dwBufSize = 256;
                if(ERROR_SUCCESS == RegQueryValueEx(hNdiIntKey, "LowerRange", 0, &dwDataType, szData, &dwBufSize))
                {
                    if(strcmp((char*)szData, "ethernet") == 0) // �ж��ǲ�����̫����
                    {
                        memset(szData, '\0', sizeof(szData));
                        dwBufSize = 256;
                        if(ERROR_SUCCESS == RegQueryValueEx(hSubKey, "DriverDesc", 0, &dwDataType, szData, &dwBufSize))
                        {
                            // szData �б�����������ϸ����
                            memset(szData, '\0', sizeof(szData));    
                            dwBufSize = 256;
                            if(ERROR_SUCCESS == RegQueryValueEx(hSubKey, "NetCfgInstanceID", 0, &dwDataType, szData, &dwBufSize))
                            {
                                // szData �б�������������
                                wLen = (wLen < strlen((const char*)szData)) ? wLen : strlen((const char*)szData); // ���ȱ���
                                strncpy(lpszAdpName, (const char*)szData, wLen);
                                bFlag = TRUE;
                            }
                        }
                    }
                }
                RegCloseKey(hNdiIntKey);
            }
            RegCloseKey(hSubKey);
        }

        dwBufSize = 256;
        if(bFlag)
        {
            break; // �ҵ����˳�whileѭ��
        }
    } /* end of while */

    RegCloseKey(hKey);

#endif
    return TRUE;
}

/*=============================================================================
�� �� ���� RegGetIpAdr
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� s8* lpszAdapterName
           s8* pIpAddr
           u16 wLen
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/11/30   4.0		�ܹ��                  ����
=============================================================================*/
BOOL32 RegGetIpAdr(s8* lpszAdapterName, s8* pIpAddr, u16 wLen)
{
    if(NULL == lpszAdapterName || NULL == pIpAddr)
    {
        return FALSE;
    }
    if(0 == wLen) // ip��ַ����Ϊ0
    {
        return FALSE;
    }
    
#ifdef WIN32

    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
    strcat(aszStrKeyName, lpszAdapterName);
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return FALSE;

    s8 aszIp[255];
    memset(aszIp, '\0', sizeof(aszIp));
    int nIpLen = sizeof(aszIp);

    u32 dwType = REG_MULTI_SZ;
    RegQueryValueEx(hKey, "IPAddress", 0, &dwType, (unsigned char*)aszIp, (unsigned long*)&nIpLen);
    if(wLen < nIpLen)
    {
        nIpLen = wLen;// ���ȱ���
    }
    memcpy(pIpAddr, aszIp, nIpLen);
    
    RegCloseKey(hKey);

#endif

    return TRUE;
}

/*=============================================================================
  �� �� ���� RegGetNetMask
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� s8* lpszAdapterName
             s8* pNetMask
             u8 byLen
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 RegGetNetMask(s8* lpszAdapterName, s8* pNetMask, u16 wLen)
{
    if(NULL == lpszAdapterName || NULL == pNetMask)
    {
        return FALSE;
    }
    if(0 == wLen) // �����ַ����Ϊ0
    {
        return FALSE;
    }
    
#ifdef WIN32

    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
    strcat(aszStrKeyName, lpszAdapterName);
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return FALSE;

    s8 aszNetMask[255];
    memset(aszNetMask, '\0', sizeof(aszNetMask));
    int nMaskLen = sizeof(aszNetMask);

    u32 dwType = REG_MULTI_SZ;
    RegQueryValueEx(hKey, "SubnetMask", 0, &dwType, (unsigned char*)aszNetMask, (unsigned long*)&nMaskLen);
    if(wLen < nMaskLen)
    {
        nMaskLen = wLen;// ���ȱ���
    }
    memcpy(pNetMask, aszNetMask, nMaskLen);
    
    RegCloseKey(hKey);

#endif

    return TRUE;
}

/*=============================================================================
  �� �� ���� RegGetNetGate
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� s8* lpszAdapterName
             s8* pNetGate
             u8 byLen
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 RegGetNetGate(s8* lpszAdapterName, s8* pNetGate, u16 wLen)
{
    if(NULL == lpszAdapterName || NULL == pNetGate)
    {
        return FALSE;
    }
    if(0 == wLen) // ���ص�ַ����Ϊ0
    {
        return FALSE;
    }

#ifdef WIN32
    
    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
    strcat(aszStrKeyName, lpszAdapterName);
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return FALSE;
    
    s8 aszNetGate[100];
    memset(aszNetGate, '\0', sizeof(aszNetGate));
    u16 wGateLen = sizeof(aszNetGate);

    u32 dwType = REG_MULTI_SZ;
    RegQueryValueEx(hKey, "DefaultGateway", 0, &dwType, (unsigned char*)aszNetGate, (unsigned long*)&wGateLen);
    if(wLen < wGateLen)
    {
        wGateLen = wLen; // ���ȱ���
    }
    memcpy(pNetGate, aszNetGate, wGateLen);

    RegCloseKey(hKey);

#endif
    
    return TRUE;
}

/*=============================================================================
  �� �� ���� RegSetIpAdr
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� s8* pIpAddr
             u8 byLen
  �� �� ֵ�� BOOL 
=============================================================================*/
BOOL32 RegSetIpAdr(s8* lpszAdapterName, s8* pIpAddr, u16 wLen)
{
    if(NULL == lpszAdapterName || NULL == pIpAddr)
    {
        return FALSE;
    }
    if(0 == wLen) // ������ַ����Ϊ0
    {
        return FALSE;
    }

#ifdef WIN32

    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
    strcat(aszStrKeyName, lpszAdapterName);
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_WRITE, &hKey) != ERROR_SUCCESS)
        return FALSE;

    s8 szIpAddress[100];
    strncpy(szIpAddress, pIpAddr, wLen);

    u8 byIpLen = strlen(szIpAddress);

    *(szIpAddress + byIpLen + 1) = 0x00; // REG_MULTI_SZ������Ҫ�ں����ټӸ�0
    byIpLen += 2;

    RegSetValueEx(hKey, "IPAddress", 0, REG_MULTI_SZ, (unsigned char*)szIpAddress, byIpLen);

    RegCloseKey(hKey);

#endif
    return TRUE;
}

/*=============================================================================
  �� �� ���� RegSetNetMask
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� s8* pNetMask
             u8 byLen
  �� �� ֵ�� BOOL 
=============================================================================*/
BOOL32 RegSetNetMask(s8* lpszAdapterName, s8* pNetMask, u16 wLen)
{
    if(NULL == lpszAdapterName || NULL == pNetMask)
    {
        return FALSE;
    }
    if(0 == wLen) // �����ַ����Ϊ0
    {
        return FALSE;
    }

#ifdef WIN32
    
    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
    strcat(aszStrKeyName, lpszAdapterName);
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_WRITE, &hKey) != ERROR_SUCCESS)
        return FALSE;

    s8 aszNetMask[100];
    memset(aszNetMask, '\0', sizeof(aszNetMask));
    strncpy(aszNetMask, pNetMask, wLen);

    int nMaskLen = strlen(aszNetMask);

    *(aszNetMask + nMaskLen + 1) = 0x00;
    nMaskLen += 2;

    RegSetValueEx(hKey, "SubnetMask", 0, REG_MULTI_SZ, (unsigned char*)aszNetMask, nMaskLen);

    RegCloseKey(hKey);
    
#endif
    return TRUE;
}

/*=============================================================================
  �� �� ���� RegSetNetGate
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� s8* pNetGate
             u8 byLen
  �� �� ֵ�� BOOL 
=============================================================================*/
BOOL32 RegSetNetGate(s8* lpszAdapterName, s8* pNetGate, u16 wLen)
{
    if(NULL == lpszAdapterName || NULL == pNetGate)
    {
        return FALSE;
    }
    if(0 == wLen) // ���ص�ַ����Ϊ0
    {
        return FALSE;
    }

#ifdef WIN32
    
    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
    strcat(aszStrKeyName, lpszAdapterName);
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_WRITE, &hKey) != ERROR_SUCCESS)
        return FALSE;
    
    char aszNetGate[100];
    strncpy(aszNetGate, pNetGate, 98);
    u8 byGateLen = strlen(aszNetGate);
    *(aszNetGate + byGateLen + 1) = 0x00;
    byGateLen += 2;

    u32 dwType = REG_MULTI_SZ;
    RegSetValueEx(hKey, "DefaultGateway", 0, dwType, (unsigned char*)aszNetGate, byGateLen);

    RegCloseKey(hKey);
    
#endif
    return TRUE;
}

/*=============================================================================
�� �� ���� GetBrdIpArray
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  u32 *pIpArray
           u8 &byIpNum
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/11/30   4.0		�ܹ��                  ����
=============================================================================*/
BOOL32 GetBrdIpArray( u32 *pIpArray, u8 &byIpNum )
{
	if ( NULL == pIpArray || 0 == byIpNum )
	{
		OspPrintf(TRUE,FALSE, "[GetBrdIpArray] Get IP fail.Array point.0x%x, Num.%d\n", pIpArray, byIpNum );
		return FALSE;
	}

	s8 aszAdpName[255];
    s8 aszIp[255];
    memset(aszIp, 0, sizeof(aszIp));
    memset(aszAdpName, 0, sizeof(aszAdpName));

	BOOL32 bReturn = TRUE;

    u8 byLen = (u8)sizeof(aszAdpName);
    BOOL32 bRet = RegGetAdpName(aszAdpName, byLen);
    if(bRet)
    {
        bRet = RegGetIpAdr(aszAdpName, aszIp, sizeof(aszIp));
        if(bRet)
        {
			u8 byIpCount = 0;
			s8 achSeps[] = " \t";        // �ָ���
			s8 *pchToken = NULL;
			s8 *pchNTpos = NULL;		// ��¼NULL terminal��λ��
			u8 byNTnum = 0;
			// ����
			pchToken = aszIp;
			while( byNTnum < 2 )
			{
				if ( *pchToken == '\0' )
				{
					byNTnum++;
					if ( *(pchToken+1) == '\0' )
					{
						byNTnum++;
						break;
					}
					else
					{
						byNTnum--;
						*pchToken = ' ';
					}
				}
				pchToken++;
			}
			
			pchToken = NULL;
			pchToken = strtok( aszIp, achSeps );
			while (NULL != pchToken)
			{
				if ( byIpCount >= byIpNum )
				{
					break;
				}
				*(pIpArray+byIpCount) = ntohl( INET_ADDR( pchToken ) );
				byIpCount++;
				pchToken = strtok( NULL, achSeps );
			}
			byIpNum = byIpCount;
			bReturn = TRUE;
        }
        else
        {
            byIpNum = 0;
			bReturn = FALSE;
        }
    }
    else
    {
		byIpNum = 0;
		bReturn = FALSE;
    }

	if ( !bReturn )
	{
		OspPrintf(TRUE,FALSE, "[GetBrdIpArray] Get IP fail.\n");
	}
	return bReturn;
}


/*=============================================================================
�� �� ���� SetBrdIpArray
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  u32 *pIpArray
           u8 &byIpNum
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/11/30   4.0		�ܹ��                  ����
=============================================================================*/
BOOL32 SetBrdIpArray( u32 *pIpArray, u8 &byIpNum )
{
	if ( NULL == pIpArray || 0 == byIpNum )
	{
		OspPrintf(TRUE,FALSE, "[SetBrdIpArray] Set IP fail. Array point.0x%x, Num.%d\n", pIpArray, byIpNum );
		return FALSE;
	}

	s8 aszAdpName[255];
    s8 aszIp[255];
	s8 aszTemp[18];
    memset(aszIp, 0, sizeof(aszIp));
    memset(aszAdpName, 0, sizeof(aszAdpName));
	memset(aszTemp, 0, sizeof(aszTemp) );

	BOOL32 bReturn = TRUE;

    u8 byLen = (u8)sizeof(aszAdpName);
    BOOL32 bRet = RegGetAdpName(aszAdpName, byLen);
    if(bRet)
    {
		for ( u8 byIdx = 0; byIdx < byIpNum; byIdx++ )
		{
			memset(aszTemp, 0, sizeof(aszTemp) );
			sprintf( aszTemp, "%d.%d.%d.%d ", (pIpArray[byIdx]>>24)&0xff, (pIpArray[byIdx]>>16)&0xff, (pIpArray[byIdx]>>8)&0xff, pIpArray[byIdx]&0xff );
			strcat( aszIp, aszTemp );
			if ( strlen(aszIp) + sizeof("255.255.255.255") > 255 )
			{
				byIpNum = byIdx + 1;
				break;
			}
		}
		if ( RegSetIpAdr( aszAdpName, aszIp, strlen(aszIp) ) )
		{
			bReturn = TRUE;
		}
		else
		{
			byIpNum = 0;
			bReturn = FALSE;
		}
    }
    else
    {
		byIpNum = 0;
		bReturn = FALSE;
    }

	if ( !bReturn )
	{
		OspPrintf(TRUE,FALSE, "[SetBrdIpArray] Set IP fail.\n");
	}
	return bReturn;
}

/*=============================================================================
�� �� ���� GetNetMaskIpArray
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  u32 *pMaskArray
           u8 &byMaskNum
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/11/30   4.0		�ܹ��                  ����
=============================================================================*/
BOOL32 GetNetMaskIpArray( u32 *pMaskArray, u8 &byMaskNum )
{
	if ( NULL == pMaskArray || 0 == byMaskNum )
	{
		OspPrintf(TRUE,FALSE, "[GetNetMaskIpArray] Get MASK fail.\n");
		return FALSE;
	}

	s8 aszAdpName[255];
    s8 aszMask[255];
    memset(aszMask, 0, sizeof(aszMask));
    memset(aszAdpName, 0, sizeof(aszAdpName));

	BOOL32 bReturn = TRUE;

    u8 byLen = (u8)sizeof(aszAdpName);
    BOOL32 bRet = RegGetAdpName(aszAdpName, byLen);
    if(bRet)
    {
        bRet = RegGetNetMask(aszAdpName, aszMask, sizeof(aszMask));
        if(bRet)
        {
			u8 byMaskCount = 0;
			s8 achSeps[] = " \t";        // �ָ���
			s8 *pchToken = NULL;
			s8 *pchNTpos = NULL;		// ��¼NULL terminal��λ��
			u8 byNTnum = 0;
			pchToken = aszMask;

			//����
			while( byNTnum < 2 )
			{
				if ( *pchToken == '\0' )
				{
					byNTnum++;
					if ( *(pchToken+1) == '\0' )
					{
						byNTnum++;
						break;
					}
					else
					{
						byNTnum--;
						*pchToken = ' ';
					}
				}
				pchToken++;
			}
			
			pchToken = NULL;

			pchToken = strtok( aszMask, achSeps );
			while (NULL != pchToken)
			{
				if ( byMaskCount >= byMaskNum )
				{
					break;
				}
				*(pMaskArray+byMaskCount) = ntohl( INET_ADDR( pchToken ) );
				byMaskCount++;
				pchToken = strtok( NULL, achSeps );
			}
			byMaskNum = byMaskCount;
			bReturn = TRUE;
        }
        else
        {
            byMaskNum = 0;
			bReturn = FALSE;
        }
    }
    else
    {
		byMaskNum = 0;
		bReturn = FALSE;
    }

	if ( !bReturn )
	{
		OspPrintf(TRUE,FALSE, "[GetNetMaskIpArray] Get MASK fail.\n");
	}
	return bReturn;
}

/*=============================================================================
�� �� ���� SetNetMaskIpArray
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  u32 *pMaskArray
           u8 &byMaskNum
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/11/30   4.0		�ܹ��                  ����
=============================================================================*/
BOOL32 SetNetMaskIpArray( u32 *pMaskArray, u8 &byMaskNum )
{
	if ( NULL == pMaskArray || 0 == byMaskNum )
	{
		OspPrintf(TRUE,FALSE, "[SetNetMaskIpArray] Set MASK fail. Array point.0x%x, Num.%d\n", pMaskArray, byMaskNum );
		return FALSE;
	}

	s8 aszAdpName[255];
    s8 aszMask[255];
	s8 aszTemp[18];
    memset(aszMask, 0, sizeof(aszMask));
    memset(aszAdpName, 0, sizeof(aszAdpName));
	memset(aszTemp, 0, sizeof(aszTemp) );

	BOOL32 bReturn = TRUE;

    u8 byLen = (u8)sizeof(aszAdpName);
    BOOL32 bRet = RegGetAdpName(aszAdpName, byLen);
    if(bRet)
    {
		for ( u8 byIdx = 0; byIdx < byMaskNum; byIdx++ )
		{
			memset(aszTemp, 0, sizeof(aszTemp) );
			sprintf( aszTemp, "%d.%d.%d.%d ", (pMaskArray[byIdx]>>24)&0xff, (pMaskArray[byIdx]>>16)&0xff, (pMaskArray[byIdx]>>8)&0xff, pMaskArray[byIdx]&0xff );
			strcat( aszMask, aszTemp );
			if ( strlen(aszMask) + sizeof("255.255.255.255") > 255 )
			{
				byMaskNum = byIdx + 1;
				break;
			}
		}
		if ( RegSetNetMask( aszAdpName, aszMask, strlen(aszMask) ) )
		{
			bReturn = TRUE;
		}
		else
		{
			byMaskNum = 0;
			bReturn = FALSE;
		}
    }
    else
    {
		byMaskNum = 0;
		bReturn = FALSE;
    }

	if ( !bReturn )
	{
		OspPrintf(TRUE,FALSE, "[SetNetMaskIpArray] Set MASK fail.\n");
	}
	return bReturn;
}
 /*=============================================================================
�� �� ���� GetNetGateArray
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  u32 *pNetGateArray
           u8 &byNetGateNum
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/11/30   4.0		�ܹ��                  ����
=============================================================================*/
BOOL32 GetNetGateArray( u32 *pNetGateArray, u8 &byNetGateNum )
{
	if ( NULL == pNetGateArray || 0 == byNetGateNum )
	{
		OspPrintf(TRUE,FALSE, "[GetNetGateArray] Get NetGate fail.Array point.0x%x, Num.%d\n", pNetGateArray, byNetGateNum );
		return FALSE;
	}

	s8 aszAdpName[255];
    s8 aszNetGate[255];
    memset(aszNetGate, 0, sizeof(aszNetGate));
    memset(aszAdpName, 0, sizeof(aszAdpName));

	BOOL32 bReturn = TRUE;

    u8 byLen = (u8)sizeof(aszAdpName);
    BOOL32 bRet = RegGetAdpName(aszAdpName, byLen);
    if(bRet)
    {
        bRet = RegGetNetGate(aszAdpName, aszNetGate, sizeof(aszNetGate));
        if(bRet)
        {
			u8 byIpCount = 0;
			s8 achSeps[] = " \t";        // �ָ���
			s8 *pchToken = NULL;
			s8 *pchNTpos = NULL;		// ��¼NULL terminal��λ��
			u8 byNTnum = 0;
			// ����
			pchToken = aszNetGate;
			while( byNTnum < 2 )
			{
				if ( *pchToken == '\0' )
				{
					byNTnum++;
					if ( *(pchToken+1) == '\0' )
					{
						byNTnum++;
						break;
					}
					else
					{
						byNTnum--;
						*pchToken = ' ';
					}
				}
				pchToken++;
			}
			
			pchToken = NULL;
			pchToken = strtok( aszNetGate, achSeps );
			while (NULL != pchToken)
			{
				if ( byIpCount >= byNetGateNum )
				{
					break;
				}
				*(pNetGateArray+byIpCount) = ntohl( INET_ADDR( pchToken ) );
				byIpCount++;
				pchToken = strtok( NULL, achSeps );
			}
			byNetGateNum = byIpCount;
			bReturn = TRUE;
        }
        else
        {
            byNetGateNum = 0;
			bReturn = FALSE;
        }
    }
    else
    {
		byNetGateNum = 0;
		bReturn = FALSE;
    }

	if ( !bReturn )
	{
		OspPrintf(TRUE,FALSE, "[GetNetGateArray] Get NetGate fail.\n");
	}
	return bReturn;
}

/*=============================================================================
�� �� ���� SetNetGateArray
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  u32 *pNetGateArray
           u8 &byNetGateNum
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/11/30   4.0		�ܹ��                  ����
=============================================================================*/
BOOL32 SetNetGateArray( u32 *pNetGateArray, u8 &byNetGateNum )
{
	if ( NULL == pNetGateArray || 0 == byNetGateNum )
	{
		OspPrintf(TRUE,FALSE, "[SetNetGateArray] Set NetGate fail. Array point.0x%x, Num.%d\n", pNetGateArray, byNetGateNum );
		return FALSE;
	}

	s8 aszAdpName[255];
    s8 aszNetGate[255];
	s8 aszTemp[18];
    memset(aszNetGate, 0, sizeof(aszNetGate));
    memset(aszAdpName, 0, sizeof(aszAdpName));
	memset(aszTemp, 0, sizeof(aszTemp) );

	BOOL32 bReturn = TRUE;

    u8 byLen = (u8)sizeof(aszAdpName);
    BOOL32 bRet = RegGetAdpName(aszAdpName, byLen);
    if(bRet)
    {
		for ( u8 byIdx = 0; byIdx < byNetGateNum; byIdx++ )
		{
			memset(aszTemp, 0, sizeof(aszTemp) );
			sprintf( aszTemp, "%d.%d.%d.%d ", (pNetGateArray[byIdx]>>24)&0xff, (pNetGateArray[byIdx]>>16)&0xff, (pNetGateArray[byIdx]>>8)&0xff, pNetGateArray[byIdx]&0xff );
			strcat( aszNetGate, aszTemp );
			if ( strlen(aszNetGate) + sizeof("255.255.255.255") > 255 )
			{
				byNetGateNum = byIdx + 1;
				break;
			}
		}
		if ( RegSetNetGate( aszAdpName, aszNetGate, strlen(aszNetGate) ) )
		{
			bReturn = TRUE;
		}
		else
		{
			byNetGateNum = 0;
			bReturn = FALSE;
		}
    }
    else
    {
		byNetGateNum = 0;
		bReturn = FALSE;
    }

	if ( !bReturn )
	{
		OspPrintf(TRUE,FALSE, "[SetNetGateArray] Set NetGate fail.\n");
	}
	return bReturn;
}

/*=============================================================================
�� �� ���� GetLocalIp
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� u32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/11/2   4.0		�ܹ��                  ����
=============================================================================*/
u32 GetLocalIp()
{
	s8 achHostName[64];
	memset(achHostName, 0, sizeof(achHostName));
	// ȡ�ñ����� 
	if(SOCKET_ERROR == gethostname(achHostName, 64))
	{
		return 0;
	}
	// ͨ���������õ���ַ��
	struct hostent *pHost = gethostbyname(achHostName);
	if ( NULL == pHost )
	{
		return 0;
	}
	// Ĭ��ʹ�õ�һ��IP��ַ 
	if ( NULL != pHost->h_addr_list[0] )
	{
		return( *(u32*)pHost->h_addr_list[0] );
	}
	else
	{
		return 0;
	}
}

/*=============================================================================
�� �� ���� CreateDirectory
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const s8* lpszDirPath
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/11/6   4.0		�ܹ��                  ����
=============================================================================*/
BOOL32 CreateDirectory( const s8* lpszDirPath )
{
	if ( NULL == lpszDirPath )
	{
		return FALSE;
	}
	
    BOOL bOk = ::CreateDirectory( lpszDirPath, NULL );
    return bOk;
}

/*=============================================================================
�� �� ���� CreatCfgDebugFile
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� 
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/11/2   4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CreatCfgDebugFile()
{
    s8 achProfileName[64] = {0};
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILENAME_CONFIG);

	FILE *stream = fopen( achProfileName, "r" );
    if( stream != NULL )
    {
		fclose(stream);
        return(	TRUE );
    }

	u8 byLop = 0;

	s8 achProfileDir[64];
	memset( achProfileDir, 0, sizeof(achProfileDir) );
	sprintf( achProfileDir, "%s", DIR_CONFIG );

    //guzh [2008/05/27]
	CreateDirectory( achProfileDir );
	stream = fopen( achProfileName, "w" );
	if ( stream == NULL )
	{
		OspPrintf(TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Creat brdcfgdebug.ini failed!\n");
		return FALSE;
	}
	fclose(stream);

	u32 adwIpArray[16] = {0};
	u8 byIpNum = 16;

	if ( !GetBrdIpArray( adwIpArray, byIpNum ) || byIpNum == 0 )
	{
		OspPrintf(TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Get local ip failed!\n");
		return FALSE;
	}
	
	u32 adwMaskIpArray[16] = {0};
	u8 byIpMaskNum = 16;

	if ( !GetNetMaskIpArray( adwMaskIpArray, byIpMaskNum ) || byIpMaskNum == 0 )
	{
		OspPrintf(TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Get local ip mask failed!\n");
		return FALSE;
	}

	u32 adwNetGateArray[16] = {0};
	u8 byNetGateNum = 16;

	if ( !GetNetGateArray( adwNetGateArray, byNetGateNum ) || byNetGateNum == 0 )
	{
		OspPrintf(TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Get local Netgate failed!\n");
		return FALSE;
	}
//	u32 dwIpAddr = ntohl( GetLocalIp() );
//	if ( 0 == dwIpAddr )
//	{
//		OspPrintf(TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Get local ip failed!\n");
//		return FALSE;
//	}
	
	// ��ý�������
	s8 achFullPathBuf[255];   
	memset( achFullPathBuf, 0, sizeof(achFullPathBuf) );   
	GetModuleFileName( NULL, achFullPathBuf, 255 );
	s8 ch = '\\';
	s8 * pchDest = NULL;
	pchDest = strrchr( achFullPathBuf, ch );
	s8 achNameBuf[255];
	memset( achNameBuf, 0, 255 );
	if ( pchDest == NULL || pchDest == achFullPathBuf )
	{
		strncpy( achNameBuf, achFullPathBuf, strlen(achFullPathBuf) );
	}
	else
	{
		strncpy( achNameBuf, pchDest+1, strlen(pchDest) );
	}
	achNameBuf[sizeof(achNameBuf)-1] = '\0';
	strlwr( achNameBuf );

	//������������, ȷ����ۺź͵�������
    u8 byMcuType = MCUTYPE_UNKOWN;
	u8 byLayer = 0;
	u8 bySlot = 0;
	u8 byType = 0;
	pchDest = NULL;
	if ( NULL != ( pchDest = strstr( achNameBuf, "mcu.exe" ) ) ||
         NULL != ( pchDest = strstr( achNameBuf, "mcu_8000b.exe" ) ) ||
         NULL != ( pchDest = strstr( achNameBuf, "mcu_8000c.exe" ) ) )
	{
        if ( NULL != ( pchDest = strstr( achNameBuf, "mcu.exe" ) ) )
        {
            byMcuType = MCUTYPE_MCU;
        }
        else
        {
            byMcuType = MCUTYPE_MINIMCU;
        }

		byLayer = 0;
		bySlot = 0;
		byType = DSL8000_BRD_MPC;
	}
	else if ( NULL != ( pchDest = strstr( achNameBuf, "cri" ) ) )
	{
		byLayer = 1;
		bySlot = 3;
		byType = DSL8000_BRD_CRI;
	}
	else if ( NULL != ( pchDest = strstr( achNameBuf, "dri" ) ) )
	{
		byLayer = 1;
		bySlot = 6;
		byType = DSL8000_BRD_DRI;
	}
	else if ( NULL != ( pchDest = strstr( achNameBuf, "dsc" ) ) )
	{
		byLayer =0;
		bySlot = 1;
		byType = DSL8000_BRD_MDSC;
	}
	else if ( NULL != ( pchDest = strstr( achNameBuf, "eqp" ) ) )
	{
		byLayer = 2;
		bySlot = 5;
		byType = DSL8000_BRD_MMP;
	}
	else
	{
		byLayer = 2;
		bySlot = 5;
		byType = DSL8000_BRD_MMP;
	}

	// layer
	s32 nValue = byLayer;
	BOOL32 bResult = SetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Layer, nValue );
	if( FALSE == bResult )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Wrong profile while setting %s!\n", KEY_Layer );
	}

	// slot
	nValue = bySlot;
	bResult = SetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Slot, nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Wrong profile while setting %s!\n", KEY_Slot );
	}

	// type
	nValue = byType;
	bResult = SetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Type, nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Wrong profile while setting %s!\n", KEY_Type );
	}
	
	// Ip
	s8 achIpAddr[18] = {0};
	sprintf( achIpAddr, "%d.%d.%d.%d", (adwIpArray[0]>>24)&0xff, (adwIpArray[0]>>16)&0xff, (adwIpArray[0]>>8)&0xff, adwIpArray[0]&0xff );
	bResult = SetRegKeyString( achProfileName, SECTION_BoardConfig, KEY_BoardIpAddr, achIpAddr );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Wrong profile while setting %s!\n", KEY_BoardIpAddr );
	}

	s8 achEthEntry[8];
	memset( achEthEntry, 0, sizeof(achEthEntry) );
	s8 achEthIpNumEntry[16];
	memset( achEthIpNumEntry, 0, sizeof(achEthIpNumEntry) );
	s8 achEthIpAddrEntry[32];
	memset( achEthIpAddrEntry, 0, sizeof(achEthIpAddrEntry) );
	s8 achEthMacEntry[32];
	memset( achEthMacEntry, 0, sizeof(achEthMacEntry) );

    // �����MCU���ͽ������֣���ΪMCS����IP���ڵ����ڣ�8000A��MINIMCU��ϰ���Ƿ���
    // zgc, 2008-06-02
    u8 byEhtId = 0;
    if ( byMcuType == MCUTYPE_MCU )
    {
        byEhtId = 0;
    }
    else
    {
        byEhtId = 1;
    }

	// ������Ĭ��ֵ
	sprintf( achEthEntry, "%s%d", ENTRYPART_ETHERNET, byEhtId );
	sprintf( achEthIpNumEntry, "%s%s", achEthEntry, ENTRYPART_IPNUM );
	sprintf( achEthMacEntry, "%s%s", achEthEntry, ENTRYPART_MAC );
	
	// IP num
	bResult = SetRegKeyInt( achProfileName, SECTION_IpConfig, achEthIpNumEntry, 0 );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Wrong profile while setting %s!\n", achEthIpNumEntry );
	}

	// Mac
	bResult = SetRegKeyString( achProfileName, SECTION_IpConfig, achEthMacEntry, "11:11:11:11:11:11" );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Wrong profile while setting %s!\n", achEthMacEntry );
	}

    if ( byMcuType == MCUTYPE_MCU )
    {
        byEhtId = 1;
    }
    else
    {
        byEhtId = 0;
    }

    // �����MCU���ͽ������֣���ΪMCS����IP���ڵ����ڣ�8000A��MINIMCU��ϰ���Ƿ���
    // zgc, 2008-06-02
	//�������PC��ȡ����ֵ
	memset( achEthEntry, 0, sizeof(achEthEntry) );
	memset( achEthIpNumEntry, 0, sizeof(achEthIpNumEntry) );
	memset( achEthIpAddrEntry, 0, sizeof(achEthIpAddrEntry) );
	memset( achEthMacEntry, 0, sizeof(achEthMacEntry) );

	sprintf( achEthEntry, "%s%d", ENTRYPART_ETHERNET, byEhtId);
	sprintf( achEthIpNumEntry, "%s%s", achEthEntry, ENTRYPART_IPNUM );
	sprintf( achEthMacEntry, "%s%s", achEthEntry, ENTRYPART_MAC );

	// IP num
	bResult = SetRegKeyInt( achProfileName, SECTION_IpConfig, achEthIpNumEntry, byIpNum );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Wrong profile while setting %s!\n", achEthIpNumEntry );
	}

	// Ip
	s8 achIpAndMask[40] = {0};
	for ( byLop = 0; byLop < byIpNum; byLop++ )
	{
		memset( achIpAndMask, 0, sizeof(achIpAndMask) );
		sprintf( achIpAndMask, "%d.%d.%d.%d %d.%d.%d.%d", 
			(adwIpArray[byLop]>>24)&0xff, (adwIpArray[byLop]>>16)&0xff, (adwIpArray[byLop]>>8)&0xff, adwIpArray[byLop]&0xff,
			(adwMaskIpArray[byLop]>>24)&0xff, (adwMaskIpArray[byLop]>>16)&0xff, (adwMaskIpArray[byLop]>>8)&0xff, adwMaskIpArray[byLop]&0xff );
		sprintf( achEthIpAddrEntry, "%s%s%d", achEthEntry, ENTRYPART_IPADDR, byLop );
		bResult = SetRegKeyString( achProfileName, SECTION_IpConfig, achEthIpAddrEntry, achIpAndMask );
		if( bResult == FALSE )  
		{
			OspPrintf( TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Wrong profile while setting %s!ip.0x%x\n", achEthIpAddrEntry, adwIpArray[byLop]  );
		}
	}

	// Mac
	bResult = SetRegKeyString( achProfileName, SECTION_IpConfig, achEthMacEntry, "11:11:11:11:11:11" );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Wrong profile while setting %s!\n", achEthMacEntry );
	}

	// Ip route num
	bResult = SetRegKeyInt( achProfileName, SECTION_IpRoute, KEY_IpRouteNum, byNetGateNum );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Wrong profile while setting %s!\n", KEY_IpRouteNum );
	}

	s8 achIpRoute[64] = {0};
	s8 achIpRouteEntry[32] = {0};
	for ( byLop = 0; byLop < byNetGateNum; byLop++ )
	{
		memset( achIpRoute, 0, sizeof(achIpRoute) );
		sprintf( achIpRoute, "0.0.0.0 0.0.0.0 %d.%d.%d.%d", (adwNetGateArray[byLop]>>24)&0xff, (adwNetGateArray[byLop]>>16)&0xff, (adwNetGateArray[byLop]>>8)&0xff, adwNetGateArray[byLop]&0xff );
		memset( achIpRouteEntry, 0, sizeof(achIpRouteEntry) );
		sprintf( achIpRouteEntry, "%s%d", ENTRYPART_IpRoute, byLop );
		bResult = SetRegKeyString( achProfileName, SECTION_IpRoute, achIpRouteEntry, achIpRoute );
		if( bResult == FALSE )  
		{
			OspPrintf( TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Wrong profile while setting %s!\n", achIpRouteEntry );
		}
	}

	return TRUE;
}

/*=============================================================================
�� �� ���� MoveToSectionStart
��    �ܣ� ���α��Ƶ�ָ���Ķ�����һ�п�ʼ��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� FILE *stream, Profile����   
           const s8* lpszSectionName, Profile�еĶ���
           BOOL32 bCreate, ���û�иö����Ƿ񴴽�
�� �� ֵ�� ����ƫ����Offset�����ʧ�ܷ���-1
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/31   4.0			�ܹ��                  ����
=============================================================================*/
s32	MoveToSectionStart( FILE *stream, const s8* lpszSectionName, BOOL32 bCreate )
{
	char    achRow[MAX_LINE_LEN + 1];
    char    achSectionCopy[MAX_SECTIONNAME_LEN + 1];
    u32  dwLen;
    s8*   lpszReturn;

    ASSERT( strlen( lpszSectionName ) <= MAX_SECTIONNAME_LEN );

    /* move from the beginning */
    if ( fseek( stream, 0, SEEK_SET ) != 0 )
    {
        return( -1 );
    }

    /* copy the section name and change it */
    strncpy( achSectionCopy, lpszSectionName, MAX_SECTIONNAME_LEN );
    achSectionCopy[MAX_SECTIONNAME_LEN] = '\0';
    Trim( achSectionCopy );
    StrUpper( achSectionCopy );

    do
    {
        /* if error or arrive at the end of file */
		memset( achRow, 0, sizeof(achRow) );
        if( fgets( achRow, MAX_LINE_LEN, stream ) == NULL )
        {
            if( bCreate && feof( stream ) )    /* create the section */
            {
                fputs( (const s8*)STR_RETURN, stream );
                fputs( (const s8*)"[", stream );
                fputs( (const s8*)lpszSectionName, stream );
                fputs( (const s8*)"]", stream );
                fputs( (const s8*)STR_RETURN, stream );
                fseek( stream, 0, SEEK_END );
                return( ftell( stream ) );
            }
            else
            {
                return( -1 );
            }
        }

        /* eliminate the return key */
        if( ( lpszReturn = strstr( achRow, "\r\n" ) ) != NULL )
        {
            lpszReturn[0] = '\0';
        }
        else if( ( lpszReturn = strstr( achRow, "\n" ) ) != NULL )
        {
            lpszReturn[0] = '\0';
        }
        Trim( achRow );
        dwLen = strlen( achRow );
        
        /* must be a section */
        if( dwLen <= 2 || achRow[0] != '[' || achRow[dwLen - 1] != ']' )
        {
            continue;
        }
        
        /* verify the section name */
        strncpy( achRow, achRow + 1, dwLen - 2 );
        achRow[dwLen - 2] = '\0';
        Trim( achRow );
        StrUpper( achRow );
        if( strcmp( achRow, achSectionCopy ) == 0 )    /* identical */
        {
            return( ftell( stream ) );
        }
    } while( TRUE );
}

/*=============================================================================
�� �� ���� MoveToSectionEnd
��    �ܣ� ���α��Ƶ�ָ���Ķ�������һ�ο�ʼ�������ļ���β
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� FILE *stream, Profile����   
           const s8* lpszSectionName, Profile�еĶ���
           BOOL32 bCreate, ���û�иö����Ƿ񴴽�
�� �� ֵ�� ����ƫ����Offset�����ʧ�ܷ���-1
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/7/31   4.0			�ܹ��                  ����
=============================================================================*/
s32 MoveToSectionEnd( FILE *stream, const s8* lpszSectionName, BOOL32 bCreate )
{
	char    achRow[MAX_LINE_LEN + 1];
    u32		dwLen;
    s8*		lpszReturn;

	/* get section start offset */
	s32 nStartOffset = MoveToSectionStart( stream, lpszSectionName, bCreate );
	if ( -1 == nStartOffset )
	{
		OspPrintf( TRUE, FALSE, "[MoveToSectionEnd] Move to section %s start failed!\n", lpszSectionName );
		return nStartOffset;
	}

	/* move from the beginning */
    if ( fseek( stream, nStartOffset, SEEK_SET ) != 0 )
    {
        return( -1 );
    }

	s32 nScanOffset = nStartOffset;
	do
    {
        /* if error or arrive at the end of file */
		memset( achRow, 0, sizeof(achRow) );
        if( fgets( achRow, MAX_LINE_LEN, stream ) == NULL )
        {
            if ( feof(stream) != 0 )
			{
				/* eliminate the return key */
				if( ( lpszReturn = strstr( achRow, "\r\n" ) ) != NULL )
				{
					lpszReturn[0] = '\0';
				}
				else if( ( lpszReturn = strstr( achRow, "\n" ) ) != NULL )
				{
					lpszReturn[0] = '\0';
				}
				Trim( achRow );
				dwLen = strlen( achRow );
        
				/* must be a section */
				if( dwLen <= 2 || achRow[0] != '[' || achRow[dwLen - 1] != ']' )
				{
					return ftell(stream);
				}
				else
				{
					return nScanOffset;
				}
			}
			else
			{
				return (-1);
			}
        }

        /* eliminate the return key */
		if( ( lpszReturn = strstr( achRow, "\r\n" ) ) != NULL )
		{
			lpszReturn[0] = '\0';
		}
		else if( ( lpszReturn = strstr( achRow, "\n" ) ) != NULL )
		{
			lpszReturn[0] = '\0';
		}
		Trim( achRow );
		dwLen = strlen( achRow );

		/* must be a section */
		if( dwLen <= 2 || achRow[0] != '[' || achRow[dwLen - 1] != ']' )
		{
			nScanOffset = ftell(stream);
			continue;
		}
		else
		{
			return nScanOffset;
		}

    } while( TRUE );
}


/*=============================================================================
�� �� ���� IsMacAddrInvalid
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� s8* pchMacAddr
           s8 chSep
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/12/4   4.0		�ܹ��                  ����
=============================================================================*/
BOOL32 IsMacAddrInvalid(s8* pchMacAddr, s8 chSep)
{
	if ( NULL == pchMacAddr )
	{
		return TRUE;
	}

	u32 dwlenth = strlen(pchMacAddr);
	if( dwlenth != 17 )
	{
		return TRUE;
	}

	for( u32 dwLoop=0; dwLoop<dwlenth; dwLoop++ )
	{
		if( (dwLoop+1)%3 == 0  )
		{
			if( pchMacAddr[dwLoop] != chSep )
				return TRUE;
		}
		else if( 
				!(
					( pchMacAddr[dwLoop] >= '0' && pchMacAddr[dwLoop] <= '9' ) 
					|| ( pchMacAddr[dwLoop] >= 'a' && pchMacAddr[dwLoop] <= 'f' )
					|| ( pchMacAddr[dwLoop] >= 'A' && pchMacAddr[dwLoop] <= 'F' )
				 )
			   )
		{
			return TRUE;
		}
	}

	return FALSE;
}


/*=============================================================================
�� �� ���� DelSection
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  FILE *stream
           const s8* lpszSectionName
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/12/4   4.0		�ܹ��                  ����
=============================================================================*/
s32	DelSection( FILE *stream, const s8* lpszSectionName )
{
	if ( stream == NULL || lpszSectionName == NULL )
	{
		return -1;
	}
	fseek( stream, 0, SEEK_SET );
	s32 nSecStartOffset = MoveToSectionStart( stream, lpszSectionName, FALSE );
	if ( -1 == nSecStartOffset )
	{
		OspPrintf( TRUE, FALSE,"[DelSection] Move to section %s start failed!\n", lpszSectionName );
		return -1;
	}
	OspPrintf( TRUE, FALSE, "[DelSection] Sec %s start offset: %d\n", lpszSectionName, nSecStartOffset );
	s32 nSetEndOffset = MoveToSectionEnd( stream, lpszSectionName, FALSE );
	if ( -1 == nSetEndOffset )
	{
		OspPrintf( TRUE, FALSE, "[DelSection] Move to section %s end failed!\n", lpszSectionName );
		return -1;
	}
	OspPrintf( TRUE, FALSE, "[DelSection] Sec %s end offset: %d\n", lpszSectionName, nSetEndOffset );
	u32 dwLen = nSetEndOffset - nSecStartOffset;
	if ( 0 == dwLen )
	{
		return ftell(stream);
	}

	fseek( stream, 0, SEEK_END );
	u32 dwCpyLen = ftell(stream) - nSetEndOffset;
	
	s8 *lpszCpyBuf = (s8*)malloc( dwCpyLen + dwLen );
	if ( lpszCpyBuf == NULL )
	{
		return -1;
	}
	memset( lpszCpyBuf, 0, dwCpyLen + dwLen );
	fseek( stream, nSetEndOffset, SEEK_SET );
	dwCpyLen = fread( lpszCpyBuf, sizeof( char ), dwCpyLen, stream );
	fseek( stream, nSecStartOffset, SEEK_SET );
	dwCpyLen = fwrite( lpszCpyBuf, sizeof( char ), dwCpyLen+dwLen, stream );

	free(lpszCpyBuf);
	return ftell(stream);
}


/*=============================================================================
�� �� ���� TrimCfgFile
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� const s8* lpszCfgFilePath
�� �� ֵ�� BOOL32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/12/4   4.0		�ܹ��                  ����
=============================================================================*/
BOOL32 TrimCfgFile( const s8* lpszCfgFilePath )
{
	FILE *stream = fopen( lpszCfgFilePath, "rb+" );
    if( stream == NULL )
    {
        return( FALSE );
    }
	fseek( stream, 0, SEEK_END );
	u32 dwFileLen = ftell( stream );
	s8 *lpszFileBuf = (s8*)malloc( dwFileLen+3 );
	memset( lpszFileBuf, 0 , dwFileLen );

	s8 *lpszTemp = lpszFileBuf;
	s8 *lpszReturn = NULL;
	u32 dwLen = 0;
	u32 dwTrimLen = 0;
	char achRow[MAX_LINE_LEN + 1];
	fseek( stream, 0, SEEK_SET );
	do 
	{
		memset( achRow, 0, sizeof(achRow) );
		if( fgets( achRow, MAX_LINE_LEN, stream ) == NULL )
		{
			if ( 0 == feof(stream) )
			{
				return FALSE;
			}
		}

		/* eliminate the return key */
		if( ( lpszReturn = strstr( achRow, "\r\n" ) ) != NULL )
		{
			lpszReturn[0] = '\0';
		}
		else if( ( lpszReturn = strstr( achRow, "\n" ) ) != NULL )
		{
			lpszReturn[0] = '\0';
		}
		Trim( achRow );
		dwLen = strlen( achRow );
		
		if ( dwLen > 0 )
		{
			sprintf( lpszTemp, "%s", achRow );
			lpszTemp += dwLen;
			sprintf( lpszTemp, "%s", STR_RETURN );
			lpszTemp += strlen(STR_RETURN);
		}
		
	} while( 0 == feof(stream) );

	dwTrimLen = lpszTemp - lpszFileBuf;

	fclose( stream );
	stream = fopen( lpszCfgFilePath, "w" );
	fwrite( lpszFileBuf, sizeof(s8), dwTrimLen, stream );

	free( lpszFileBuf );
	fclose( stream );
	
	return TRUE;
}

/*================================
��������BrdInit 
���ܣ���ģ��ĳ�ʼ������
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  ��
����ֵ˵���� ���󷵻�ERROR���ɹ�����OK
==================================*/
STATUS BrdInit(void)
{
    g_dwPID = getpid();
	return OK;
}

/*================================
��������BrdQueryPosition 
���ܣ������(ID����š���λ)��ѯ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  ptBrdPosition�� �ɸú������ذ������Ϣ�Ľṹָ��
����ֵ˵���� ���󷵻�ERROR���ɹ�����OK�Ͱ������Ϣ��������TBrdPosition�ṹ����
==================================*/
STATUS BrdQueryPosition(TBrdPosition *ptBrdPosition)
{
	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}
	
	s8 achProfileName[32];
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILENAME_CONFIG);

	s32 nValue = 0;
	BOOL32 bResult = GetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Layer, 0, &nValue );
	if( FALSE == bResult )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd] Wrong profile while reading %s!\n", KEY_Layer );
		return( ERROR );
	}
	ptBrdPosition->byBrdLayer = (u8)nValue;

	bResult = GetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Slot, 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd] Wrong profile while reading %s!\n", KEY_Slot );
		return( ERROR );
	}
	ptBrdPosition->byBrdSlot = (u8)nValue;

	bResult = GetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Type, 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd] Wrong profile while reading %s!\n", KEY_Type );
		return( ERROR );
	}
	ptBrdPosition->byBrdID = (u8)nValue;

	return OK;
}

/*================================
��������BrdGetBoardID
���ܣ��豸���ID��ѯ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  ��
����ֵ˵���� �磺��������ID�ź궨��
==================================*/
u8  BrdGetBoardID(void)
{
	if ( !CreatCfgDebugFile() )
	{
		return 0;
	}

	s8 achProfileName[32];
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILENAME_CONFIG);

	s32 nValue = 0;
	BOOL32 bResult = GetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Type, 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[Brd] Wrong profile while reading %s!\n", KEY_Type );
		return 0;
	}
	return (u8)nValue;	
}

/*================================
��������BrdGetFuncID
���ܣ��豸����ID��ѯ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  ��
����ֵ˵���� ���幦��ID
==================================*/
//u8  BrdGetFuncID(void);

/*================================
��������BrdQueryHWVersion 
���ܣ�Ӳ���汾�Ų�ѯ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  ��
����ֵ˵���� Ӳ���汾�ţ�4��BITλ��ȡֵ��Χ0~16
==================================*/
u8  BrdQueryHWVersion (void)
{
	return (u8)BOARDHWVER;
}

/*================================
��������BrdQueryFPGAVersion 
���ܣ�FPGA��EPLD�汾�Ų�ѯ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵������
����ֵ˵���� FPGA��EPLD�汾�ţ�4��BITλ��ȡֵ��Χ0~16
==================================*/
u8  BrdQueryFPGAVersion (void)
{
	return (u8)BOARDFPGAVER;
}

/*================================
��������BrdEthPrintEnable
���ܣ������Ƿ���̫��״̬��ӡ��Ч
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  
����ֵ˵���� �ޡ�
==================================*/
void BrdEthPrintEnable(BOOL32 bEnable)
{
	return;
}

/*================================
��������BrdQueryOsVer
���ܣ��ṩ��ҵ�����õ�os�汾�Ų�ѯ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  pchVer�� ��ȡ�汾���ַ���ָ��
                dwBufLen�����ַ����ĳ���
                pdwVerLen����ȡ�汾��ʵ���ַ������ȣ��ó��ȷ���֮ǰ�����dwBufLen�Ƚ��Ƿ����������ʱ�Ŷ�pVer��ֵ��

����ֵ˵���� �ޡ����ʵ���ַ������ȴ���dwBufLen��pVerLen��ֵΪ0
==================================*/
void  BrdQueryOsVer(s8 *pchVer, u32 dwBufLen,u32 *pdwVerLen)
{
	u32 length = strlen(OSVERSION) < dwBufLen ? strlen(OSVERSION) : dwBufLen ;

	memcpy( pchVer, OSVERSION, length );

	*pdwVerLen = length;
	return;
}

/*================================
��������BrdGetBSP15Speed
���ܣ���ȡBSP-15����Ƶ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵���� byDevID:BSP-15��ID��(��0��ʼ��
����ֵ˵���� �ɹ�����BSP-15����Ƶ,ʧ�ܷ���ERROR
==================================*/
u8 BrdGetBSP15Speed(u8 byDevID)
{
	u8 byretspeed = 0;
	if( byDevID < (u8)5 )
	{
		byretspeed = BSP15SPEED;
	}

	return byretspeed;
}

/*================================
��������BrdGetBSP15SdramSize
���ܣ���ȡBSP-15��SDRAM�Ĵ�С
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵���� byDevID:BSP-15��ID��(��0��ʼ��
����ֵ˵���� �ɹ�����BSP-15SDRAM�Ĵ�С,ʧ�ܷ���ERROR
==================================*/
u8 BrdGetBSP15SdramSize(u8 byDevID)
{
	u8 bydramsize = 0;
	if( byDevID < 5 )
	{
		bydramsize = BSP15SPEED;
	}

	return bydramsize;
}

/*���ڿ��ƽӿ�*/

/*================================
��������BrdOpenSerial
���ܣ��ṩ��ҵ����Rs232 Rs422 Rs485���⴮��
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����byPort���ڶ˿ں�
              #define SERIAL_RS232                 0
              #define SERIAL_RS422                 1
              #define SERIAL_RS485                 2
              #define BRD_SERIAL_INFRARED          3
����ֵ˵���� �򿪴��ڵľ��
==================================*/
s32 BrdOpenSerial(u8 byPort)
{
	s32 nret = 0;
	switch( byPort )
	{
	case BRD_SERIAL_RS232:
		nret = 0;
		break;
	case BRD_SERIAL_RS422:
		nret = 1;
		break;
	case BRD_SERIAL_RS485:
		nret = 2;
		break;
	case BRD_SERIAL_INFRARED:
		nret = 3;
		break;
	default:
		nret = 5;
		break;
	}
	return nret;
}

/*================================
��������BrdCloseSerial
���ܣ��ṩ��ҵ���ر�Rs232 Rs422 Rs485���⴮��
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  nFd�������ļ�����������BrdOpenSerial��������
����ֵ˵���� �ɹ�����0��ʧ�ܷ���ERROR
==================================*/
s32 BrdCloseSerial(s32 nFd)
{
	return OK;
}

/*================================
��������BrdReadSerial
���ܣ��ṩ��ҵ����ȡRs232 Rs422 Rs485����
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  nFd�������ļ�����������BrdOpenSerial��������
                pbyBuf��������ݵĻ�����ָ��
                nMaxbytes��Ҫ��ȡ�����ݵ�����ֽ���
����ֵ˵���� �ɹ����ض�ȡ���ݵ��ֽ�������Χ1~ nMaxbytes��0��ʾû�����ݣ�ʧ�ܷ���
            ERROR
==================================*/
s32  BrdReadSerial(s32 nFd, s8  *pbyBuf, s32 nMaxbytes)
{
	return OK;
}

/*================================
��������BrdWriteSerial 
���ܣ��ṩ��ҵ���дRs232 Rs422 Rs485���⴮��
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  nFd�������ļ�����������BrdOpenSerial��������
                pbyBuf��Ҫд������ݵĻ�����ָ��
                nBytes��Ҫд�����ݵ��ֽ���
����ֵ˵���� �ɹ�����д�����ݵ��ֽ������ҵ���nBytes����������Ч����ERROR��д��ֵ
             ������nBytes��ʾд�뷢������

==================================*/
s32 BrdWriteSerial (s32 nFd, s8  *pbyBuf, s32 nBytes)
{
	return OK;
}

/*================================
��������BrdIoctlSerial 
���ܣ��ṩ��ҵ������Rs232 Rs422 Rs485���ú��⴮�ڣ�ͬioctl����
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  nFd�������ļ�����������BrdOpenSerial��������
                nFunction�����������룬����صĴ��ڲ����궨��
                nArg������
����ֵ˵���� �ɹ�����OK����������Ч��ʧ�ܷ���ERROR
==================================*/
s32 BrdIoctlSerial (s32 nFd, s32 nFunction, s32 nArg)
{
	return OK;
}

/*================================
��������BrdRs485QueryData
���ܣ�Rs485��ѯ���ݣ��ȷ���ѯ֡������նԶ�����
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  nFd�������ļ�����������BrdOpenSerial��������
		        ptRS485InParam�������������Ľṹָ�룻
		        ptRS485RtnData: ��ŷ�����Ϣ�Ľṹָ�롣
����ֵ˵����  485״̬��غ궨�� 
==================================*/
s32 BrdRs485QueryData (s32 nFd, TRS485InParam *ptRS485InParam,TRS485RtnData *ptRS485RtnData)
{
	if( NULL == ptRS485InParam || NULL == ptRS485RtnData )
	{
		return ERROR;
	}
	return OK;
}

/*================================
��������BrdRs485TransSnd 
���ܣ�Rs485͸������
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  nFd�������ļ�����������BrdOpenSerial��������
                pbyMsg�������͵����ݣ�
                dwMsgLen: ���ݳ���
����ֵ˵���� 485״̬��غ궨��
==================================*/
s32 BrdRs485TransSnd (s32 nFd, u8 *pbyMsg, u32 dwMsgLen)
{
	return OK;
}

/*================================
��������BrdLedStatusSet
���ܣ����õƵ�״̬
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  byLedID:�Ƶ�ID�ţ���BoardWrapper.h����صĶ��壩
                byState:�Ƶ�״̬����BoardWrapper.h����صĶ��壩
����ֵ˵���� �ɹ�����OK,ʧ�ܷ���ERROR
==================================*/
s32  BrdLedStatusSet(u8 byLedID, u8 byState)
{
    printf("[BrdLedStatusSet] Set Led.%d to State.%d\n", byLedID, byState);
	return OK;
}
  
/*================================
��������BrdQueryLedState
���ܣ�����ģʽ��ѯ 
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  ptBrdLedState����ŵƵ�״̬�Ľṹ��ָ��
����ֵ˵���� �ɹ�����OK,ʧ�ܷ���ERROR
==================================*/
s32  BrdQueryLedState(TBrdLedState *ptBrdLedState)
{
	if( NULL == ptBrdLedState )
	{
		return ERROR;
	}
	return OK;
}

/*================================
��������BrdQueryNipState
���ܣ�Nip״̬��ѯ 
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵���� ��
����ֵ˵���� �ɹ�����OK,ʧ�ܷ���ERROR
==================================*/
s32  BrdQueryNipState(void)
{
	return OK;
}

/*================================
��������SysRebootEnable 
���ܣ�����ϵͳ����exceptionʱ�Զ���λ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  ��
����ֵ˵���� ��
==================================*/
void  SysRebootEnable (void)
{
	return;
}

/*================================
��������SysRebootDisable
���ܣ���ֹϵͳ����exceptionʱ�Զ���λ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  ��
����ֵ˵���� ��
==================================*/
void  SysRebootDisable(void)
{
	return;
}

/*================================
��������BrdWakeup
���ܣ������豸
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  ��
����ֵ˵���� �ɹ�����OK ��ʧ�ܷ���ERROR
==================================*/
STATUS  BrdWakeup(void)
{
	return OK;
}

/*================================
��������BrdSleep 
���ܣ������豸
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  ��
����ֵ˵���� �ɹ�����OK ��ʧ�ܷ���ERROR
==================================*/
STATUS  BrdSleep (void)
{
	return OK;
}

/*================================
��������BrdHwReset
���ܣ�ϵͳӲ����λ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  ��
����ֵ˵���� ��
==================================*/
void  BrdHwReset(void)
{
    ExitProcess(0);
	return;
}

/*================================
��������SysRebootHookAdd
���ܣ���λ�ص�ע�ắ��
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  ָ��VOIDFUNCPTR���͵ĺ���ָ��
����ֵ˵���� �ɹ�����OK ��ʧ�ܷ���ERROR
==================================*/
//STATUS SysRebootHookAdd(VOIDFUNCPTR ptRebootHookFunc)
//{
//	return OK;
//}

/*================================
��������SysOpenWdGuard
���ܣ��ṩ��ҵ���3.0�汾�Ĵ�ϵͳ��������
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  dwNoticeTimeout��֪ͨ��Ϣʱ�䣬���Ӧ�ó��򳬹����ʱ�仹û��֪ͨ����ϵͳ��Ϣ��ϵͳ����λ��ʱ�䵥λΪ�롣
					�������Ϊ0������ΪӦ�ó�����Ҫ��ʱ����֪ͨ��Ϣ������Ӧ�ó���Ľ���״̬�쳣ʱϵͳ��λ��
����ֵ˵���� �ɹ�����OK,ʧ�ܷ���ERROR
==================================*/
STATUS SysOpenWdGuard(u32 dwNoticeTimeout)
{
	return OK;
}

/*================================
��������SysCloseWdGuard
���ܣ��ṩ��ҵ���3.0�汾�Ĺر�ϵͳ��������
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  ��
����ֵ˵���� �ɹ�����OK,ʧ�ܷ���ERROR
==================================*/
STATUS SysCloseWdGuard(void)
{
	return OK;
}

/*====================================================================
������      : SysNoticeWdGuard
����        ���ṩ��ҵ���3.0�汾��Ӧ�ó���֪ͨ����ϵͳ��������״̬��ͬʱ��˸���еơ�����ϵͳ������
			  Ӧ�ó���ͱ�����ע�����趨��ʱ���ڵ�������ӿڣ��������ϵͳ����λϵͳ��
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵������
����ֵ˵��  ��������ERROR���ɹ�����OK��
====================================================================*/
STATUS SysNoticeWdGuard(void)
{
	return OK;
}

/*====================================================================
������      : SysWdGuardIsOpened
����        ���ṩ��ҵ���3.0�汾�Ļ�ȡϵͳ����״̬��
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵������
����ֵ˵��  ���򿪷���TRUE���رշ���FALSE��
====================================================================*/
BOOL32 SysWdGuardIsOpened(void)
{
	return TRUE;
}

/*====================================================================
������      : BrdSetWatchdogMode
����        �����ÿ��Ź�����ģʽ
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����byMode:���¿��Ź��궨�� 
                #define WATCHDOG_USE_CLK            0x00   ʱ��Ӳ��ι�� 
                #define WATCHDOG_USE_SOFT           0x01    ���ι�� 
                #define WATCHDOG_STOP                0x02   ֹͣι�����豸����������
����ֵ˵��  ����
˵����������VxWorks����ҵ�����ι��Ƶ���������⣬��ˣ���Linux�����˵�����ι����һ����
ͳһ���������ṩ��ϵͳ������һ�׺����ӿڣ��ýӿ���Linux�ϲ����ṩ��
====================================================================*/
void   BrdSetWatchdogMode(u8 byMode)
{
	return;
}

/*====================================================================
������      : BrdFeedDog
����        ��ι������,1.6�������ٵ�һ�Σ�����ϵͳ�����������øú�������������ȼ�����ܸߣ�����Ϊ������ȼ�
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵������
����ֵ˵��  ����
˵����������VxWorks����ҵ�����ι��Ƶ���������⣬��ˣ���Linux�����˵�����ι����һ����
ͳһ���������ṩ��ϵͳ������һ�׺����ӿڣ��ýӿ���Linux�ϲ����ṩ��
====================================================================*/
void   BrdFeedDog(void)
{
	return;
}

/*====================================================================
������      : BrdAlarmStateScan
����        ���ײ�澯��Ϣɨ��
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����ptBrdAlarmState:���صĸ澯��Ϣ
����ֵ˵��  ���򿪷���TRUE���رշ���FALSE��
====================================================================*/
STATUS BrdAlarmStateScan(TBrdAlarmState *ptBrdAlarmState)
{
	return OK;
}

/*====================================================================
������      : BrdSetFanState
����        ������ϵͳ��������״̬
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����byRunState��ģʽ
                #define BRD_FAN_RUN                  0       ����ת��
                #define BRD_FAN_STOP                 1       ����ͣת
����ֵ˵��  ���ɹ�����OK ��ʧ�ܷ���ERROR
====================================================================*/
STATUS BrdSetFanState(u8 byRunState)
{
	if ( BRD_FAN_RUN != byRunState && BRD_FAN_STOP != byRunState )
	{
		return ERROR;
	}
	return OK;
}

/*====================================================================
������      : BrdFanIsStopped
����        ����ѯָ�������Ƿ�ͣת
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵������
����ֵ˵��  ��TRUE=����ͣת / FALSE=��������
====================================================================*/
BOOL32   BrdFanIsStopped(u8 byFanId)
{
	return FALSE;
}

/*====================================================================
������      : BrdExtModuleIdentify
����        ���ն����ģ��ʶ����
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵������
����ֵ˵��  �����ģ��ID�ţ�
            0ΪE1ģ�飬1Ϊ4E1ģ�飬2ΪV35DTEģ�飬3ΪV35DCEģ�飬4Ϊ����ģ�飬5Ϊ2E1ģ��,
            6ΪKDV8000Bģ��, 0xcΪDSCģ��,ĿǰMDSCҲ�����ID�ţ���һ���MDSC���ı�Ϊ0xd,0xeΪHDSCģ�飬
            0xf û�����ģ��
====================================================================*/
u8   BrdExtModuleIdentify(void)
{
	return 0xd; // mdsc;
}

/*====================================================================
������      : BrdVideoMatrixSet
����        ����Ƶ������������
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����ע��ѡ��Ķ˿ڷ�Χ�Ӿ�������Ͷ�����16x16������������˿�ȡֵ��Χ��1~16
            ����Ӳ������޷�ȫ��ʹ�ã�ֻʹ����15������8x8ȡֵ��Χ��1~8��
            videoInSelect = ��Ƶ����Դѡ�� 1~16/8����˿ڣ� 0xfb = ����Ӧ�������
            0xfc = ����Ӧ�������Ĭ�����е�����ڶ��Ǵ򿪵ģ����ҵ�����ر���ĳ
            ������ڣ�������ȴ��������ڣ�Ȼ�������ý���������������
            videoOutSelect = ��Ƶ����˿�ѡ��1~16/8��Ƶ����ӿ�
            ˵������KDV8010A�ϣ��豸����ϵ�6������/�����ڶ�Ӧ������/����˿ں��ǣ�2~7
����ֵ˵��  ����
====================================================================*/
void BrdVideoMatrixSet(u8 byInSelect, u8 byOutSelect)
{
	return;
}

/*====================================================================
������      : BrdTimeGet
����        ��ȡϵͳʱ�䣨�����RTC�л�ȡʱ�䣩
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����ptGettm����ŷ���ʱ��Ľṹָ�룻tmΪϵͳ�����ݽṹ
����ֵ˵��  �����󷵻�ERROR���ɹ�����OK
˵����ҵ�����Ա��ӿڷ��ص�tm�ṹ�е�����±�������˴���֮����ܴ����û���
���崦��Ҫ���ǣ�tm�е���Ҫ����1900��tm�е���Ҫ������1
====================================================================*/
STATUS BrdTimeGet( struct tm* ptGettm )
{
	time_t time_c;
	time( &time_c );
	*ptGettm = *localtime( &time_c );
	return OK;
}

/*====================================================================
������      : BrdTimeSet
����        ������ϵͳʱ�䣬�����RTC��ͬ������
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����ptGettm�����Ҫ���õ�ʱ��Ľṹָ�룻tmΪϵͳ�����ݽṹ
����ֵ˵��  �����󷵻�ERROR���ɹ�����OK
˵����ҵ������tm�ṹ�е�����±�������˴���֮����ܴ����ýӿڣ�
���崦��Ҫ���ǣ�tm�е���Ҫ���û����õ���ֵ��ȥ1900��tm�е���Ҫ���û����õ���ֵ��ȥ1
====================================================================*/
STATUS BrdTimeSet( struct tm* ptGettm )
{
	return OK;
}

/*====================================================================
������      : BrdGetAlarmInput
����        ����ȡ�ֳ��澯(KDM2400/2500ר��)
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����byPort: �澯����˿ں�
              pbyState: ��Ÿ澯����ֵ״̬��ָ�루ֵ���壺0:�澯��1:������
����ֵ˵��  �����󷵻�ERROR���ɹ�����OK
====================================================================*/
STATUS BrdGetAlarmInput(u8 byPort, u8* pbyState)
{
	if( NULL == pbyState )
	{
		return ERROR;
	}
	return OK;
}

/*====================================================================
������      : BrdSetAlarmOutput
����        ������ֳ��澯
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����byPort: �澯����˿ںţ�0,1,2��
              pbyState: 0:��������;1:�����ָ�
����ֵ˵��  �����󷵻�ERROR���ɹ�����OK
====================================================================*/
STATUS BrdSetAlarmOutput(u8 byPort, u8  byState)
{
	return OK;
}

/*====================================================================
������      : BrdGetSdramMaxSize
����        ��ȡ�����ڴ����ֵ,��MByteΪ��λ
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵������
����ֵ˵��  ���ڴ����ֵ,��MByteΪ��λ
====================================================================*/
//u32 BrdGetSdramMaxSize(void)
//{
//	struct sysinfo info;
//	s32 nret = sysinfo( &info );
//	return info.totalram;
//}

/*====================================================================
������      : BrdGetAllDiskInfo
����        ����ȡflash������Ϣ
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����ptBrdAllDiskInfo ��ָ��TbrdAllDiskInfo�ṹ��ָ��
����ֵ˵��  �����󷵻�ERROR���ɹ�����OK
====================================================================*/
STATUS BrdGetAllDiskInfo(TBrdAllDiskInfo *ptBrdAllDiskInfo)
{
	if( NULL == ptBrdAllDiskInfo )
	{ return ERROR; }
	return OK;
}

/*====================================================================
������      : BrdGetFullFileName
����        ��ת���ļ���Ϊ��ȫ·�����ļ���
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����  byPutDiskId:�̵�Id�� 
                pchInFileName:������ļ��� �������̷���
                pchRtnFileName:���صļ���Id���̷�ȫ·�����ļ���
����ֵ˵��  �����صļ���Id���̷����ļ����ĳ��ȣ����ֽ�Ϊ��λ��0Ϊ����
====================================================================*/
u32 BrdGetFullFileName(u8 byPutDiskId, s8 *pchInFileName, s8 *pchRtnFileName)
{
	if( pchInFileName == pchRtnFileName )
	{
		return 0;
	}
	return OK;
}

/*====================================================================
������      : BrdCopyFile
����        �������ļ�
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����  pchSrcFile��Դ�ļ���������ȫ·��
                pchDesFile��Ŀ���ļ���������ȫ·��
����ֵ˵��  ���ɹ�����OK ��ʧ�ܷ���ERROR
====================================================================*/
STATUS BrdCopyFile (s8 *pchSrcFile, s8 *pchDesFile)
{
	return OK;
}

/*====================================================================
������      : BrdBackupFile
����        �������ļ�
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����  pchSrcFile��Դ�ļ���������ȫ·��
                pchDesFile��Ŀ���ļ���������ȫ·��
����ֵ˵��  ���ɹ�����OK ��ʧ�ܷ���ERROR
====================================================================*/
STATUS BrdBackupFile (s8 *pchSrcFile, s8 *pchDesFile)
{
	return OK;
}

/*====================================================================
������      : BrdRestoreFile 
����        ����ԭ�ļ������Ŀ���ļ�������ֻ��������ǿ�и���
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����  pchSrcFile��Դ�ļ���������ȫ·��
                pchDesFile��Ŀ���ļ���������ȫ·��
����ֵ˵��  ���ɹ�����OK ��ʧ�ܷ���ERROR
====================================================================*/
STATUS BrdRestoreFile(s8 *pchSrcFile, s8 *pchDesFile)
{
	return OK;
}

/*====================================================================
������      : SysSetUsrAppParam
����        �������û����������������ã�Ϊ�˼���vx�ϵĽӿ�
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����  pbyFileName:Ӧ���ļ���
                byFileType :�����׺궨�壬������ͬ��Ϊ�˼�����ǰvx��
                #define LOCAL_UNCOMPRESS_FILE   0 ����δѹ�����ļ�
                #define LOCAL_COMPRESS_FILE  1 ����ѹ�����ļ�
                #define REMOTE_UNCOMPRESS_FILE 2 Զ��δѹ�����ļ�
                #define REMOTE_COMPRESS_FILE  3 Զ��ѹ�����ļ�
                #define LOCAL_ZIP_FILE  4 ����zipѹ�����е��ļ�
                pAutoRunFunc:Ϊ�˼�����ǰvx�Ͻӿڶ��裬��������Ч
����ֵ˵��  ���ɹ�����OK ��ʧ�ܷ���ERROR
====================================================================*/
STATUS SysSetUsrAppParam(s8 *pbyFileName, u8 byFileType, s8 *pAutoRunFunc)
{
	return OK;
}

/*====================================================================
������      : SysSetMultiAppParam
����        ������û����������������ã�Linux�������ӿ�
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����  ptAppLoadInf:ָ��Ӧ��������Ϣ�����ݽṹ��ָ��
����ֵ˵��  ���ɹ�����OK ��ʧ�ܷ���ERROR
====================================================================*/
STATUS SysSetMultiAppParam(TAppLoadInf *ptAppLoadInf)
{
	return OK;
}
/*====================================================================
������      : BrdGetMemInfo
����        ����ѯ��ǰ�ڴ�ʹ����Ϣ
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����  pdwByteFree: ��ŵ�ǰ���е��ڴ��С���ݵ�ָ�룬�ֽ�Ϊ��λ
                pdwByteAlloc: ��ŵ�ǰ�ѷ�����ڴ��С���ݵ�ָ�룬�ֽ�Ϊ��λ
����ֵ˵��  ���ɹ�����OK ��ʧ�ܷ���ERROR
====================================================================*/
//STATUS BrdGetMemInfo(u32 *pdwByteFree, u32 *pdwByteAlloc)
//{
//	struct sysinfo info;
//	s32 nret = sysinfo( &info );
//	*pdwByteFree = info.freeram/(1024*1024);
//	*pdwByteAlloc = (info.totalram - info.freeram)/(1024*1024);
//	return (0 == nret) ? OK : ERROR;
//}
/*====================================================================
������      : SysGetIdlePercent
����        ����ѯ��ǰcpu�Ŀ��аٷֱ�
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����  ��
����ֵ˵��  ����ǰcpu�Ŀ��аٷֱ�
====================================================================*/
u8 SysGetIdlePercent(void)
{
	return 50;
}

/*====================================================================
������      : BrdGetDeviceInfo
����        ����ѯ��ǰ�豸��CPU���͡���Ƶ��u-boot���ں˰汾��Ϣ�Ľӿ�
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����  ptBrdDeviceInfo:ָ��TBrdDeviceInfo�ṹ���ָ��
����ֵ˵��  ��OK/ERROR
====================================================================*/
STATUS BrdGetDeviceInfo (TBrdDeviceInfo* ptBrdDeviceInfo)
{
	if( NULL == ptBrdDeviceInfo )
		return ERROR;
	return OK;
}

/*====================================================================
������      : BrdAddUser
����        ������ϵͳ�û�
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����  pchUserName��Ҫ���ӵĵ�½�û���
		        pchPassword��Ҫ���ӵ��û��ĵ�¼����
                tUserType��Ҫ���ӵ��û����ͣ�����صĽṹ�嶨��
����ֵ˵��  ��OK/ERROR
====================================================================*/
STATUS BrdAddUser (s8* pchUserName,s8* pchPassword, TUserType tUserType)
{
	if( NULL == pchUserName )
		return ERROR;
	return OK;
}

/*====================================================================
������      : BrdDelUser
����        ��ɾ��ϵͳ�û�
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����  pchUserName��Ҫɾ���ĵ�½�û���
		        pchPassword��Ҫɾ�����û��ĵ�¼����
                tUserType��Ҫɾ�����û����ͣ�����صĽṹ�嶨��
����ֵ˵��  ��OK/ERROR
====================================================================*/
STATUS BrdDelUser (s8* pchUserName,s8* pchPassword, TUserType tUserType)
{
	if( NULL == pchUserName )
		return ERROR;
	return OK;
}

/*====================================================================
������      : BrdUpdateAppFile
����        �����û�ָ���ĵ����ļ����µ�APP��(/usr/binĿ¼��)�Ľӿ�
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����  pchSrcFile��Ҫ���µ�Դ�ļ�������������·������֧�֡�./�������·������
		        pchDstFile��Ҫ���µ�Ŀ���ļ�������������·������֧�֡�./�������·������
����ֵ˵��  ��OK/ERROR
ʾ����BrdUpdateAppFile����/ramdisk/mp8000b��,��/usr/bin/mp8000b����
	��/ramdisk/Ŀ¼�µ�mp8000b������µ�ֻ����APP��(/usr/bin)������Ϊmp8000b
====================================================================*/
STATUS BrdUpdateAppFile (s8* pchSrcFile,s8* pchDstFile)
{
	return OK;
}

/*====================================================================
������      : BrdUpdateAppImage
����        �����û�ָ����ʹ��mkfs.jffs2�����İ��������ϲ�ҵ������Image�ļ����µ�APP��(/usr/binĿ¼��)�Ľӿ�
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����  pImageFile��Ҫ���µ�Image�ļ�������������·������֧�֡�./�������·������
����ֵ˵��  ��OK/ERROR
ʾ����BrdUpdateAppImage����/ramdisk/mdsc.jffs2����
	��/ramdisk/Ŀ¼��ʹ��mkfs.jff2�����ĵİ���mdsc�����е�ҵ����������ļ���mdsc.jffs2�����ļ����µ�ֻ����APP��(/usr/bin)
ע�⣺������ú������豸���������¸���APP����������Ч	
====================================================================*/
STATUS BrdUpdateAppImage (s8* pImageFile)
{
	return OK;
}

/*================================
��������BrdGetBSP15CapturePort
���ܣ�BSP15ͼ��ɼ��˿ڲ�ѯ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵���� byBSP15ID:BSP-15��ID��(��0��ʼ��
����ֵ˵���� BSP15ͼ��ɼ��˿ں궨��
==================================*/
u8  BrdGetBSP15CapturePort(u8 byBSP15ID)
{
	return 0;
}

/*================================
��������BrdGetSAA7114OutPort
���ܣ�BSP15ͼ������ӿڲ�ѯ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵���� byBSP15ID:BSP-15��ID��(��0��ʼ��
����ֵ˵���� SAA7114ͼ����������˿ں궨��
==================================*/
u8  BrdGetSAA7114OutPort(u8 byBSP15ID)
{
	return 0;
}

/*================================
��������BrdGetCaptureChipType
���ܣ�BSP15ͼ��ɼ�оƬ��ѯ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵���� byBSP15ID:BSP-15��ID��(��0��ʼ��
����ֵ˵���� ͼ��ɼ�оƬ�ͺ�
==================================*/
u8  BrdGetCaptureChipType(u8 byBSP15ID)
{
	return 0;
}

/*================================
��������BrdGetAudCapChipType
���ܣ��ṩ��codec�ϲ��ѯBSP15��Ƶ�ɼ�оƬ������
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵���� byBSP15ID:BSP-15��ID��(��0��ʼ��
����ֵ˵���� ��Ƶ�ɼ�оƬ�ͺ�
==================================*/
u8  BrdGetAudCapChipType(u8 byBSP15ID)
{
	return 0;
}

/*================================
��������BrdBsp15GpdpIsUsed
���ܣ���ѯ��ЩBSP15��gpdp�˿ڵĻ�������
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵������
����ֵ˵���� bsp15ʹ��gpdp����״�������룬ÿ��bitλ����һ��bsp15��0=��ʹ��/1=ʹ�ã�
��λΪ0��map�� ���֧��32��map����
==================================*/
u32 BrdBsp15GpdpIsUsed(void)
{
	return 0;
}

/*================================
��������BrdGetBSP15VGAConf
���ܣ���ȡָ��bsp15��vga����
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵������
����ֵ˵���� byDevId��bsp15���豸��
��λΪ0��map�� ��VGA���ú궨��
==================================*/
u8  BrdGetBSP15VGAConf(u8 byDevId)
{
	return 0;
}

/*================================
��������BrdStopVGACap
���ܣ�ֹͣ�ɼ�vga(8083)����(����fpga)
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵������
����ֵ˵���� ��
==================================*/
void BrdStopVGACap(void)
{
	return;
}

/*================================
��������BrdStartVGACap
���ܣ���ʼ�ɼ�vga(8083)����(����fpga)
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����byMode:�������VGA�ɼ�ģʽ�궨��
����ֵ˵���� ��
==================================*/
void BrdStartVGACap(u8 byMode)
{
	return;
}

/*================================
��������BrdSetVGACapMode
���ܣ���ʼ�ɼ�vga(8083)����(����fpga)
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����byMode:�������VGA�ɼ�ģʽ�궨��
����ֵ˵���� ��
==================================*/
void BrdSetVGACapMode (u8 byMode)
{
	return;
}

/*================================
��������BrdCloseVGA
���ܣ��رղ���ֹvga(8083)�ɼ�оƬ����
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵������
����ֵ˵���� ��
==================================*/
void BrdCloseVGA(void)
{
	return;
}

/*================================
��������BrdOpenVGA
���ܣ��򿪲���ʼvga(8083)�ɼ�оƬ����
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵������
����ֵ˵���� ��
==================================*/
void BrdOpenVGA(void)
{
	return;
}

/*================================
��������BrdSetBsp15GPIOMode
���ܣ�GPIO�����л������ź�,���Ե�11��kdv8010a��Ч
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����byMode:ģʽ������صĺ궨��
����ֵ˵���� ��
==================================*/
void BrdSetBsp15GPIOMode(u8 byMode)
{
	return;
}

/*================================
��������BrdMapDevOpenPreInit
���ܣ���map����ʱ�����Ӳ���ĳ�ʼ��
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����byBSP15ID��bsp15���豸��
����ֵ˵������
==================================*/
void BrdMapDevOpenPreInit(u8 byBSP15ID)
{
	return;
}

/*================================
��������BrdMapDevClosePreInit
���ܣ���map�ر�ʱ�����Ӳ���ĸ�λ�Ȳ���
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����byBSP15ID��bsp15���豸��
����ֵ˵������
==================================*/
void BrdMapDevClosePreInit(u8 byBSP15ID)
{
	return;
}

/*====================================================================
������      : BrdAudioMatrixSet
����        ����Ƶ������������
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����ע��byAudioInSelect = ����Դѡ��0~6 = 7������ӿڣ�0������Ӳ���ڲ�
          �ѽ���������ͼ�񣬶������Ρ�0xb = ����Ӧ�������0xc = ����Ӧ�������
          byAudioOutSelect = ��Ƶ����˿�ѡ��0~6 = 7����Ƶ����ӿڣ�
          0�����Ӳ���ڲ����ѽ�����������Դ���������Ρ����ౣ����
����ֵ˵��  ����
====================================================================*/
void  BrdAudioMatrixSet(u8 byAudioInSelect, u8 byAdioOutSelect)
{
	return;
}

/*====================================================================
������      : BrdGetMatrixType
����        ����������Ͳ�ѯ
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵������
����ֵ˵��  ����������ͣ������������ͺź궨��
====================================================================*/
u8 BrdGetMatrixType(void)
{
	return 0;
}

/*====================================================================
������      : BrdGetPowerVoltage
����        ����ȡ��ǰ��ѹֵ���Ժ���Ϊ��λ
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵����dwSampleTimes ����������0��ʾʹ��Ĭ��ֵ8
����ֵ˵��  ���ɹ����ص�ѹֵ���Ժ���Ϊ��λ����Ч����ERROR
====================================================================*/
s32 BrdGetPowerVoltage(u32 dwSampleTimes)
{
	return 0;
}

/*====================================================================
������      : BrdShutoffPower
����        ���رյ�Դ��Ӧ
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵������
����ֵ˵��  ���ɹ�����OK��ʧ�ܷ���ERROR
====================================================================*/
STATUS BrdShutoffPower(void)
{
	return OK;
}

/*====================================================================
������      : BrdGetSwitchSel
����        ����ȡ����ѡ���״̬
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�������
�������˵������
����ֵ˵��  ��0 or 1
====================================================================*/
u8 BrdGetSwitchSel(void)
{
	return 0;
}
/*================================
��������BrdGetEthParam
���ܣ���ȡ��̫����������ip��mask��mac
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  u8 byEthId --- ��̫��ID�ţ�0~x��;
                TBrdEthParam *ptBrdEthParam --- �����̫����Ϣ�Ľṹָ�롣
����ֵ˵���� ������ERROR���ɹ�����OK��
==================================*/
STATUS BrdGetEthParam(u8 byEthId, TBrdEthParam *ptBrdEthParam)
{
	if( NULL == ptBrdEthParam )
		return ERROR;

	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}

	s8 achProfileName[64];
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILENAME_CONFIG);

	u32 dwIpNum = 0;
	s8 achEthEntry[8];
	memset( achEthEntry, 0, sizeof(achEthEntry) );
	s8 achEthIpNumEntry[16];
	memset( achEthIpNumEntry, 0, sizeof(achEthIpNumEntry) );
	s8 achEthIpAddrEntry[32];
	memset( achEthIpAddrEntry, 0, sizeof(achEthIpAddrEntry) );
	s8 achEthMacEntry[32];
	memset( achEthMacEntry, 0, sizeof(achEthMacEntry) );

	sprintf( achEthEntry, "%s%d", ENTRYPART_ETHERNET, byEthId );
	sprintf( achEthIpNumEntry, "%s%s", achEthEntry, ENTRYPART_IPNUM );

	BOOL32 bRet = ::GetRegKeyInt( achProfileName, SECTION_IpConfig, achEthIpNumEntry, 0, (s32*)&dwIpNum );
	if ( !bRet || 0 == dwIpNum )
	{
		return ERROR;
	}

	sprintf( achEthMacEntry, "%s%s", achEthEntry, ENTRYPART_MAC );
	sprintf( achEthIpAddrEntry, "%s%s0", achEthEntry, ENTRYPART_IPADDR );
	
	s8 achIpAddr[64];
	memset( achIpAddr, 0, sizeof(achIpAddr) );
	s8 achMac[18];
	memset( achMac, 0, sizeof(achMac) );

	bRet = ::GetRegKeyString( achProfileName, SECTION_IpConfig, achEthMacEntry, "00:00:00:00:00:00", achMac, sizeof(achMac) );
	if ( !bRet )
	{
		return ERROR;
	}
	// Mac addr
	if ( IsMacAddrInvalid(achMac, ':') )
	{
		if ( 0 != strlen(achMac) )
		{
			return ERROR;
		}
	}
    else
	{
		u8 byCount = 0;
		s8* pMacAddr = achMac;
		s8* pchToken = strtok( pMacAddr, ":" );
		while( NULL != pchToken)
		{
			BOOL32 bErr = FALSE;
			u8 byTmp = 0;
			s8 ch = 0;
			for (u8 byIndex = 0; byIndex < strlen(pchToken); byIndex ++ )
			{
				ch = pchToken[byIndex];
				if (ch >= '0' && ch <= '9' )
				{
					byTmp = (byTmp << 4) + (ch - '0');
				}
				else if (ch >= 'A' && ch <= 'F' )
				{
					byTmp =  (byTmp << 4) + (ch - 'A') + 10;
				}
				else if (ch >= 'a' && ch <= 'f' )
				{
					byTmp =  (byTmp << 4) + (ch - 'a') + 10;
				}
				else
				{
					bErr = TRUE;
					break;
				}
			}

			if (bErr)
			{
				break;
			}

			ptBrdEthParam->byMacAdrs[byCount] = byTmp;
			byCount ++;
			if (byCount == 6)
			{
				break;
			}

			pchToken = strtok( NULL, ":" );
		}

		if ( byCount != 6 )
		{
			return ERROR;
		}
	}		

	
	bRet = ::GetRegKeyString( achProfileName, SECTION_IpConfig, achEthIpAddrEntry, "127.0.0.1 255.255.255.0", achIpAddr, sizeof(achIpAddr) );
	if ( !bRet )
	{
		return ERROR;
	}

	s8 achSeps[] = " \t";        // �ָ���
    s8 *pchToken = NULL;

	// IpAddr
	pchToken = strtok( achIpAddr, achSeps );
    if (NULL == pchToken)
    {
        return ERROR;
    }
    else
    {
		ptBrdEthParam->dwIpAdrs = INET_ADDR(pchToken);
    }

	// IpMask
	pchToken = strtok( NULL, achSeps );
    if (NULL == pchToken)
    {
        ptBrdEthParam->dwIpMask = htonl(0xffffff00);
    }
    else
    {
		ptBrdEthParam->dwIpMask = INET_ADDR(pchToken);
    }
	
	return OK;
}

/*================================
��������BrdGetEthParamAll
���ܣ���ȡһ�����������е���̫�����������IP��ַ�����
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  u8 byEthId --- ��̫��ID�ţ�0~x��;
                BrdEthParamAll *ptBrdEthParamAll --- �����̫����Ϣ�Ľṹָ�롣
����ֵ˵���� ������ERROR���ɹ�����OK��
==================================*/
STATUS BrdGetEthParamAll(u8 byEthId, TBrdEthParamAll *ptBrdEthParamAll)
{
	if( NULL == ptBrdEthParamAll )
        return ERROR;

	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}

	s8 achProfileName[64];
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILENAME_CONFIG);
    
    u32 dwIpNum = 0;
	s8 achEthEntry[8];
	memset( achEthEntry, 0, sizeof(achEthEntry) );
	s8 achEthIpNumEntry[16];
	memset( achEthIpNumEntry, 0, sizeof(achEthIpNumEntry) );
	s8 achEthIpAddrEntry[32];
	memset( achEthIpAddrEntry, 0, sizeof(achEthIpAddrEntry) );
	s8 achEthMacEntry[32];
	memset( achEthMacEntry, 0, sizeof(achEthMacEntry) );
	s8 achIpAddr[64];
	memset( achIpAddr, 0, sizeof(achIpAddr) );
	s8 achMac[18];
	memset( achMac, 0, sizeof(achMac) );
	

	sprintf( achEthEntry, "%s%d", ENTRYPART_ETHERNET, byEthId );
	sprintf( achEthIpNumEntry, "%s%s", achEthEntry, ENTRYPART_IPNUM );
	sprintf( achEthMacEntry, "%s%s", achEthEntry, ENTRYPART_MAC );

	BOOL32 bRet = ::GetRegKeyInt( achProfileName, SECTION_IpConfig, achEthIpNumEntry, 0, (s32*)&dwIpNum );
	if ( !bRet || 0 == dwIpNum )
	{
		return ERROR;
	}
	else
	{
		ptBrdEthParamAll->dwIpNum = dwIpNum;
	}

	// Mac addr
	u8 abyMacAddr[6];
	memset( abyMacAddr, 0, sizeof(abyMacAddr) );
	memset( achMac, 0, sizeof(achMac) );
	bRet = ::GetRegKeyString( achProfileName, SECTION_IpConfig, achEthMacEntry, "00:00:00:00:00:00", achMac, sizeof(achMac) );
	if ( !bRet )
	{
		return ERROR;
	}
	if ( IsMacAddrInvalid(achMac, ':') )
	{
		if ( 0 != strlen(achMac) )
		{
			return ERROR;
		}
	}
	else
	{
		s8* pchToken;
		u8 byCount = 0;
		s8* pMacAddr = achMac;
		pchToken = strtok( pMacAddr, ":" );
		while( NULL != pchToken)
		{
			BOOL32 bErr = FALSE;
			u8 byTmp = 0;
			s8 ch = 0;
			for (u8 byIndex = 0; byIndex < strlen(pchToken); byIndex ++ )
			{
				ch = pchToken[byIndex];
				if (ch >= '0' && ch <= '9' )
				{
					byTmp = (byTmp << 4) + (ch - '0');
				}
				else if (ch >= 'A' && ch <= 'F' )
				{
					byTmp =  (byTmp << 4) + (ch - 'A') + 10;
				}
				else if (ch >= 'a' && ch <= 'f' )
				{
					byTmp =  (byTmp << 4) + (ch - 'a') + 10;
				}
				else
				{
					bErr = TRUE;
					break;
				}
			}

			if (bErr)
			{
				break;
			}

			abyMacAddr[byCount] = byTmp;
			byCount ++;
			if (byCount == 6)
			{
				break;
			}

			pchToken = strtok( NULL, ":" );
		}

		if ( byCount != 6 )
		{
			return ERROR;
		}
	}		

	for ( u32 dwIndex = 0; dwIndex < dwIpNum; dwIndex++ )
	{	
		memset( achEthIpAddrEntry, 0, sizeof(achEthIpAddrEntry) );
		sprintf( achEthIpAddrEntry, "%s%s%d", achEthEntry, ENTRYPART_IPADDR, dwIndex );

		memset( achIpAddr, 0, sizeof(achIpAddr) );
		bRet = ::GetRegKeyString( achProfileName, SECTION_IpConfig, achEthIpAddrEntry, "127.0.0.1 255.255.255.0", achIpAddr, sizeof(achIpAddr) );
		if ( !bRet )
		{
			return ERROR;
		}

		s8 achSeps[] = " \t";        // �ָ���
		s8 *pchToken = NULL;

		// IpAddr
		pchToken = strtok( achIpAddr, achSeps );
		if (NULL == pchToken)
		{
			return ERROR;
		}
		else
		{
			ptBrdEthParamAll->atBrdEthParam[dwIndex].dwIpAdrs = INET_ADDR(pchToken);
		}

		// IpMask
		pchToken = strtok( NULL, achSeps );
		if (NULL == pchToken)
		{
			return ERROR;
		}
		else
		{
			ptBrdEthParamAll->atBrdEthParam[dwIndex].dwIpMask = INET_ADDR(pchToken);
		}

		memcpy( ptBrdEthParamAll->atBrdEthParam[dwIndex].byMacAdrs, abyMacAddr, sizeof(abyMacAddr) );
	}
    
	return OK;
}

/*=============================
��������BrdEthParam2Str
���ܣ���TBrdEthParamת���������ļ��е��ִ�
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  s8 *achStr : ���ת������ִ�������
                TBrdEthParam tBrdEthParam : ��Ҫת���Ľṹ
����ֵ˵���� ������ERROR���ɹ�����OK��
=============================*/
BOOL32 BrdEthParam2Str( s8 *achIpAddrStr, u8 byIpStrLen, s8*achMacStr, u8 byMacStrLen, TBrdEthParam *ptBrdEthParam )
{
	if ( NULL == achIpAddrStr || NULL == achMacStr
		|| 0 == byIpStrLen || 0 == byMacStrLen )
	{
		return FALSE;
	}

	s8 achIpAddr[64];
	memset( achIpAddr, 0, sizeof(achIpAddr) );
	s8 achIp[17];
	memset( achIp, 0, sizeof(achIp) );
	s8 achMask[17];
	memset( achMask, 0, sizeof(achMask) );
	s8 achMacAddr[18];
	memset( achMacAddr, 0, sizeof(achMacAddr) );

	u32 dwIp = htonl(ptBrdEthParam->dwIpAdrs);
	sprintf( achIp, "%d.%d.%d.%d", (dwIp>>24)&0xFF, (dwIp>>16)&0xFF, (dwIp>>8)&0xFF, dwIp&0xFF );
	dwIp = htonl(ptBrdEthParam->dwIpMask);
	sprintf( achMask, "%d.%d.%d.%d", (dwIp>>24)&0xFF, (dwIp>>16)&0xFF, (dwIp>>8)&0xFF, dwIp&0xFF );
	u32 dwMacAddrSum = 0;
	for (u8 byIndex = 0; byIndex < 6; byIndex ++ )
	{
		dwMacAddrSum += ptBrdEthParam->byMacAdrs[byIndex];
	}
	if ( dwMacAddrSum != 0 )
	{
		s8 *pTemp = achMacAddr;
		for ( u8 byLop = 0; byLop < 6; byLop++ )
		{
			sprintf( pTemp, "%d%d", (ptBrdEthParam->byMacAdrs[byLop]>>4)&0x0F, ptBrdEthParam->byMacAdrs[byLop]&0x0F );
			pTemp += 2;
			if ( byLop < 5 )
			{
				sprintf( pTemp, ":" );
				pTemp++;
			}
		}
	}
	else
	{
		memset( achMacStr, 0, byMacStrLen );
	}

	sprintf( achIpAddr, "%s\t%s", achIp, achMask );
	strncpy( achIpAddrStr, achIpAddr, byIpStrLen );
	strncpy( achMacStr, achMacAddr, byMacStrLen );

	return TRUE;
}

/*================================
��������BrdSetEthParam
���ܣ���������̫������(��IP������)
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  byEthId����̫���ı�ţ�0 ~ X��X�Ӳ�ͬ���Ӷ�����
                byIpOrMac��Ϊ��������ѡ��(��BoardWrapper.h����صĺ궨��)

                ptBrdEthParam��ָ�����趨ֵ��TbrdEthParam���ݽṹ��ָ��
����ֵ˵���� ������ERROR���ɹ�����OK��
==================================*/
STATUS BrdSetEthParam(u8 byEthId, u8 byIpOrMac, TBrdEthParam *ptBrdEthParam)
{
	if( NULL == ptBrdEthParam )
		return ERROR;

	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}

	s8 achProfileName[64];
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILENAME_CONFIG);

	s8 achEthEntry[8];
	memset( achEthEntry, 0, sizeof(achEthEntry) );
	s8 achEthIpNumEntry[16];
	memset( achEthIpNumEntry, 0, sizeof(achEthIpNumEntry) );
	s8 achEthIpAddrEntry[32];
	memset( achEthIpAddrEntry, 0, sizeof(achEthIpAddrEntry) );
	s8 achEthMacEntry[32];
	memset( achEthMacEntry, 0, sizeof(achEthMacEntry) );

	sprintf( achEthEntry, "%s%d", ENTRYPART_ETHERNET, byEthId );
	sprintf( achEthIpNumEntry, "%s%s", achEthEntry, ENTRYPART_IPNUM );
	sprintf( achEthIpAddrEntry, "%s%s0", achEthEntry, ENTRYPART_IPADDR );
	sprintf( achEthMacEntry, "%s%s", achEthEntry, ENTRYPART_MAC );

	u32 dwIpNum = 0;
	BOOL32 bRet = ::GetRegKeyInt( achProfileName, SECTION_IpConfig, achEthIpNumEntry, 0, (s32*)&dwIpNum );
	if ( !bRet )
	{
		return ERROR;
	}

	// ȡ��ǰ��Param�����޸�
	TBrdEthParam tCurBrdEthParam;
	memset( &tCurBrdEthParam, 0, sizeof(tCurBrdEthParam) );
	if ( dwIpNum > 0 && ERROR == BrdGetEthParam( byEthId, &tCurBrdEthParam ) )
	{
		return ERROR;
	}
	
	switch( byIpOrMac )
	{
	case Brd_SET_IP_AND_MASK:
		tCurBrdEthParam.dwIpAdrs = ptBrdEthParam->dwIpAdrs;
		tCurBrdEthParam.dwIpMask = ptBrdEthParam->dwIpMask;
		break;
	case Brd_SET_MAC_ADDR:
		memcpy( tCurBrdEthParam.byMacAdrs, ptBrdEthParam->byMacAdrs, sizeof(tCurBrdEthParam.byMacAdrs) );
		break;
	case Brd_SET_ETH_ALL_PARAM:
		tCurBrdEthParam.dwIpAdrs = ptBrdEthParam->dwIpAdrs;
		tCurBrdEthParam.dwIpMask = ptBrdEthParam->dwIpMask;
		memcpy( tCurBrdEthParam.byMacAdrs, ptBrdEthParam->byMacAdrs, sizeof(tCurBrdEthParam.byMacAdrs) );
		break;
	default:
		return ERROR;
		break;
	}

	s8 achIpAddr[64];
	memset( achIpAddr, 0, sizeof(achIpAddr) );
	s8 achMac[18];
	memset( achMac, 0, sizeof(achMac) );

	BrdEthParam2Str( achIpAddr, sizeof(achIpAddr), achMac, sizeof(achMac), &tCurBrdEthParam );

	bRet = ::SetRegKeyString( achProfileName, SECTION_IpConfig, achEthIpAddrEntry, achIpAddr );
	if ( !bRet )
	{
		return ERROR;
	}
	bRet = ::SetRegKeyString( achProfileName, SECTION_IpConfig, achEthMacEntry, achMac );
	if ( !bRet )
	{
		return ERROR;
	}

	if ( 0 == dwIpNum )
	{
		dwIpNum++;
		bRet = ::SetRegKeyInt( achProfileName, SECTION_IpConfig, achEthIpNumEntry, dwIpNum );
		if ( !bRet )
		{
			return ERROR;
		}
	}
	
	return OK;
}

/*================================
��������BrdSetSecondEthParam
���ܣ����ô���̫������(��IP������)
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  byEthId����̫���ı�ţ�0 ~ X��X�Ӳ�ͬ���Ӷ�����
                byIpOrMac��Ϊ��������ѡ��(��BoardWrapper.h����صĺ궨��)
                ptBrdEthParam��ָ�����趨ֵ��TbrdEthParam���ݽṹ��ָ��
����ֵ˵���� ������ERROR���ɹ�����OK��
ע�⣺��Linux�£�һ�����ڿ������ö��IP��ַ��һ����������������5��IP��ַ��
==================================*/
STATUS BrdSetSecondEthParam(u8 byEthId, u8 byIpOrMac, TBrdEthParam *ptBrdEthParam)
{
	if( NULL == ptBrdEthParam )
		return ERROR;

	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}

	s8 achProfileName[64];
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILENAME_CONFIG);

	s8 achEthEntry[8];
	memset( achEthEntry, 0, sizeof(achEthEntry) );
	s8 achEthIpNumEntry[16];
	memset( achEthIpNumEntry, 0, sizeof(achEthIpNumEntry) );
	s8 achEthIpAddrEntry[32];
	memset( achEthIpAddrEntry, 0, sizeof(achEthIpAddrEntry) );
	s8 achIpAddr[64];
	memset( achIpAddr, 0, sizeof(achIpAddr) );
	s8 achMac[18];
	memset( achMac, 0, sizeof(achMac) );

	sprintf( achEthEntry, "%s%d", ENTRYPART_ETHERNET, byEthId );
	sprintf( achEthIpNumEntry, "%s%s", achEthEntry, ENTRYPART_IPNUM );
	
	// ȡ��ǰIP����
	TBrdEthParamAll tBrdEthParamAll;
	memset( &tBrdEthParamAll, 0, sizeof(tBrdEthParamAll) );
	if ( ERROR == BrdGetEthParamAll( byEthId, &tBrdEthParamAll ) )
	{
		return ERROR;
	}
	if ( 0 == tBrdEthParamAll.dwIpNum )
	{
		return ERROR;
	}

	BOOL32 bRet;
	if ( Brd_DEL_SEC_IP_AND_MASK == byIpOrMac )
	{
		// ɾ��IpConfig��
		FILE *stream = fopen( achProfileName, "rb+" );
		if( stream == NULL )
		{
			return ERROR;
		}
		s32 nOffset = DelSection( stream, SECTION_IpConfig );
		fclose( stream );
		if ( nOffset == -1 )
		{
			return ERROR;
		}
		else
		{
			if ( !TrimCfgFile( achProfileName ) )
			{
				return ERROR;
			}
		}

		// ɾ��
		u32 dwLop = 0;
		for ( dwLop = 1; dwLop < tBrdEthParamAll.dwIpNum; dwLop++ )
		{
			if ( ptBrdEthParam->dwIpAdrs == tBrdEthParamAll.atBrdEthParam[dwLop].dwIpAdrs 
				&& ptBrdEthParam->dwIpMask == tBrdEthParamAll.atBrdEthParam[dwLop].dwIpMask )
			{
				memset( &tBrdEthParamAll.atBrdEthParam[dwLop], 0, sizeof(tBrdEthParamAll.atBrdEthParam[dwLop]) );
				break;
			}
		}
		if ( dwLop == tBrdEthParamAll.dwIpNum )
		{
			return ERROR;
		}

		if ( ERROR == BrdSetEthParam(byEthId, Brd_SET_IP_AND_MASK, &tBrdEthParamAll.atBrdEthParam[0]) )
		{
			return ERROR;
		}
		u32 dwSecIdx = 1;
		for ( dwLop = 1; dwLop < tBrdEthParamAll.dwIpNum; dwLop++ )
		{			
			if ( 0 == tBrdEthParamAll.atBrdEthParam[dwLop].dwIpAdrs )
			{
				continue;
			}
			memset( achIpAddr, 0, sizeof(achIpAddr) );
			memset( achMac, 0, sizeof(achMac) );
			BrdEthParam2Str( achIpAddr, sizeof(achIpAddr), achMac, sizeof(achMac), &tBrdEthParamAll.atBrdEthParam[dwLop] );

			memset( achEthIpAddrEntry, 0, sizeof(achEthIpAddrEntry) );
			sprintf( achEthIpAddrEntry, "%s%s%d", achEthEntry, ENTRYPART_IPADDR, dwSecIdx );

			bRet = ::SetRegKeyString( achProfileName, SECTION_IpConfig, achEthIpAddrEntry, achIpAddr );
			if ( !bRet )
			{
				return ERROR;
			}
			dwSecIdx++;
		}
		tBrdEthParamAll.dwIpNum--;
		bRet = ::SetRegKeyInt( achProfileName, SECTION_IpConfig, achEthIpNumEntry, tBrdEthParamAll.dwIpNum );
		if ( !bRet )
		{
			return ERROR;
		}
	} // if ( Brd_DEL_SEC_IP_AND_MASK == byIpOrMac )

	if ( Brd_SET_SEC_IP_AND_MASK == byIpOrMac )
	{
		u32 dwLop = 0;
		for ( dwLop = 1; dwLop < tBrdEthParamAll.dwIpNum; dwLop++ )
		{
			if ( ptBrdEthParam->dwIpAdrs == tBrdEthParamAll.atBrdEthParam[dwLop].dwIpAdrs 
				|| (ptBrdEthParam->dwIpAdrs&ptBrdEthParam->dwIpMask) == (tBrdEthParamAll.atBrdEthParam[dwLop].dwIpAdrs&ptBrdEthParam->dwIpMask)
				|| (ptBrdEthParam->dwIpAdrs&tBrdEthParamAll.atBrdEthParam[dwLop].dwIpMask) == (tBrdEthParamAll.atBrdEthParam[dwLop].dwIpAdrs&tBrdEthParamAll.atBrdEthParam[dwLop].dwIpMask)  
			   )
			{
				return ERROR;
			}
		}
		memset( achIpAddr, 0, sizeof(achIpAddr) );
		memset( achMac, 0, sizeof(achMac) );
		BrdEthParam2Str( achIpAddr, sizeof(achIpAddr), achMac, sizeof(achMac), ptBrdEthParam );

		memset( achEthIpAddrEntry, 0, sizeof(achEthIpAddrEntry) );
		sprintf( achEthIpAddrEntry, "%s%s%d", achEthEntry, ENTRYPART_IPADDR, tBrdEthParamAll.dwIpNum );

		BOOL32 bRet = ::SetRegKeyString( achProfileName, SECTION_IpConfig, achEthIpAddrEntry, achIpAddr );
		if ( !bRet )
		{
			return ERROR;
		}
		tBrdEthParamAll.dwIpNum++;
		bRet = ::SetRegKeyInt( achProfileName, SECTION_IpConfig, achEthIpNumEntry, tBrdEthParamAll.dwIpNum );
		if ( !bRet )
		{
			return ERROR;
		}
	} // if ( Brd_SET_SEC_IP_AND_MASK == byIpOrMac )

	if ( Brd_SET_SEC_ETH_ALL_PARAM == byIpOrMac )
	{
		u32 dwLop = 0;
		for ( dwLop = 1; dwLop < tBrdEthParamAll.dwIpNum; dwLop++ )
		{
			if ( ptBrdEthParam->dwIpAdrs == tBrdEthParamAll.atBrdEthParam[dwLop].dwIpAdrs 
				|| (ptBrdEthParam->dwIpAdrs&ptBrdEthParam->dwIpMask) == (tBrdEthParamAll.atBrdEthParam[dwLop].dwIpAdrs&ptBrdEthParam->dwIpMask)
				|| (ptBrdEthParam->dwIpAdrs&tBrdEthParamAll.atBrdEthParam[dwLop].dwIpMask) 
					== (tBrdEthParamAll.atBrdEthParam[dwLop].dwIpAdrs&tBrdEthParamAll.atBrdEthParam[dwLop].dwIpMask)  
			   )
			{
				return ERROR;
			}
		}
		memset( achIpAddr, 0, sizeof(achIpAddr) );
		memset( achMac, 0, sizeof(achMac) );
		BrdEthParam2Str( achIpAddr, sizeof(achIpAddr), achMac, sizeof(achMac), ptBrdEthParam );

		memset( achEthIpAddrEntry, 0, sizeof(achEthIpAddrEntry) );
		sprintf( achEthIpAddrEntry, "%s%s%d", achEthEntry, ENTRYPART_IPADDR, tBrdEthParamAll.dwIpNum );

		BOOL32 bRet = ::SetRegKeyString( achProfileName, SECTION_IpConfig, achEthIpAddrEntry, achIpAddr );
		if ( !bRet )
		{
			return ERROR;
		}
		tBrdEthParamAll.dwIpNum++;
		bRet = ::SetRegKeyInt( achProfileName, SECTION_IpConfig, achEthIpNumEntry, tBrdEthParamAll.dwIpNum );
		if ( !bRet )
		{
			return ERROR;
		}
	}
	
	return OK;
}

/*================================
��������BrdDelEthParam
���ܣ�ɾ��ָ������̫���ӿ�
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  byEthId����̫���ı�ţ�0 ~ X��X�Ӳ�ͬ���Ӷ�����
����ֵ˵���� ������ERROR���ɹ�����OK��
ע�⣺��Linux�£�һ�����ڿ������ö��IP��ַ���ýӿڻ�ɾ���ýӿ��ϵ�����IP��ַ��
==================================*/
STATUS BrdDelEthParam(u8 byEthId)
{
	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}
	
	if ( byEthId > 1 )
	{
		return ERROR;
	}

	s8 achProfileName[64];
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILENAME_CONFIG);

	s8 achEthEntry[8];
	memset( achEthEntry, 0, sizeof(achEthEntry) );
	s8 achEthIpNumEntry[16];
	memset( achEthIpNumEntry, 0, sizeof(achEthIpNumEntry) );
	s8 achEthIpAddrEntry[32];
	memset( achEthIpAddrEntry, 0, sizeof(achEthIpAddrEntry) );
	s8 achIpAddr[64];
	memset( achIpAddr, 0, sizeof(achIpAddr) );
	s8 achMac[18];
	memset( achMac, 0, sizeof(achMac) );

	sprintf( achEthEntry, "%s%d", ENTRYPART_ETHERNET, byEthId );
	sprintf( achEthIpNumEntry, "%s%s", achEthEntry, ENTRYPART_IPNUM );

	s8 achEthMacEntry[32];
	memset( achEthMacEntry, 0, sizeof(achEthMacEntry) );
	sprintf( achEthMacEntry, "%s%s", achEthEntry, ENTRYPART_MAC );

	// ȡ��ǰIP����
	TBrdEthParamAll tBrdEthParamAll;
	memset( &tBrdEthParamAll, 0, sizeof(tBrdEthParamAll) );
	if ( ERROR == BrdGetEthParamAll( byEthId, &tBrdEthParamAll ) )
	{
		return ERROR;
	}
	if ( 0 == tBrdEthParamAll.dwIpNum )
	{
		return OK;
	}

	// ɾ��IpConfig��
	FILE *stream = fopen( achProfileName, "rb+" );
	if( stream == NULL )
	{
		return ERROR;
	}
	s32 nOffset = DelSection( stream, SECTION_IpConfig );
	fclose( stream );
	if ( nOffset == -1 )
	{
		return ERROR;
	}
	else
	{
		if ( !TrimCfgFile( achProfileName ) )
		{
			return ERROR;
		}
	}
	
	u16 wSum = 0;
	BOOL32 bRet;
	for ( u8 byIdx = 0; byIdx < 6; byIdx++ )
	{
		wSum += tBrdEthParamAll.atBrdEthParam[0].byMacAdrs[byIdx];
	}
	if ( wSum != 0 )
	{
		memset( achIpAddr, 0, sizeof(achIpAddr) );
		memset( achMac, 0, sizeof(achMac) );
		if ( BrdEthParam2Str( achIpAddr, sizeof(achIpAddr), achMac, sizeof(achMac), &tBrdEthParamAll.atBrdEthParam[0] ) )
		{
			bRet = ::SetRegKeyString( achProfileName, SECTION_IpConfig, achEthMacEntry, achMac );
			if ( !bRet )
			{
				return ERROR;
			}
		}
		else
		{
			return ERROR;
		}
	}

	bRet = ::SetRegKeyInt( achProfileName, SECTION_IpConfig, achEthIpNumEntry, 0 );
	if ( !bRet )
	{
		return ERROR;
	}

	return OK;
}

/*================================
��������BrdGetAllIpRoute
���ܣ���ȡ����·����Ϣ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����ptBrdAllIpRouteInfo��ָ���ŷ���·����Ϣ��TBrdAllIpRouteInfo�ṹָ��
����ֵ˵���� ���󷵻�ERROR���ɹ�����OK
==================================*/
STATUS BrdGetAllIpRoute(TBrdAllIpRouteInfo *ptBrdAllIpRouteInfo)
{
	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}

	s8 achProfileName[64];
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILENAME_CONFIG);

	u32 dwIpRouteNum = 0;
	BOOL32 bRet = ::GetRegKeyInt( achProfileName, SECTION_IpRoute, KEY_IpRouteNum, 0, (s32*)&dwIpRouteNum );
	if ( !bRet )
	{
		ptBrdAllIpRouteInfo->dwIpRouteNum = 0;
		return ERROR;
	}

	if ( ptBrdAllIpRouteInfo->dwIpRouteNum == 0 )
	{
		return OK;
	}

	ptBrdAllIpRouteInfo->dwIpRouteNum = dwIpRouteNum;
	
	s8 achIpRouteEntry[32];
	s8 achIpRoute[64];

	u8 byIdx = 0;
	for ( byIdx = 0; byIdx < dwIpRouteNum; byIdx++ )
	{
		memset( achIpRouteEntry, 0, sizeof(achIpRouteEntry) );
		sprintf( achIpRouteEntry, "%s%d", ENTRYPART_IpRoute, byIdx );

		memset( achIpRoute, 0, sizeof(achIpRoute) );
		bRet = ::GetRegKeyString( achProfileName, SECTION_IpRoute, achIpRouteEntry, "ReadError", achIpRoute, sizeof(achIpRoute) );
		if ( !bRet || strcmp( "ReadError", achIpRoute) == 0 )
		{
			ptBrdAllIpRouteInfo->dwIpRouteNum = 0;
			return ERROR;
		}

		s8 achSeps[] = " \t";        // �ָ���
		s8 *pchToken = NULL;

		// IpNet
		pchToken = strtok( achIpRoute, achSeps );
		if (NULL == pchToken)
		{
			ptBrdAllIpRouteInfo->dwIpRouteNum = 0;
			return ERROR;
		}
		else
		{
			ptBrdAllIpRouteInfo->tBrdIpRouteParam[byIdx].dwDesIpNet = INET_ADDR(pchToken);
		}

		// IpMask
		pchToken = strtok( NULL, achSeps );
		if (NULL == pchToken)
		{
			ptBrdAllIpRouteInfo->dwIpRouteNum = 0;
			return ERROR;
		}
		else
		{
			ptBrdAllIpRouteInfo->tBrdIpRouteParam[byIdx].dwDesIpMask = INET_ADDR(pchToken);
		}

		// GWIp
		pchToken = strtok( NULL, achSeps );
		if ( NULL == pchToken )
		{
			ptBrdAllIpRouteInfo->dwIpRouteNum = 0;
			return ERROR;
		}
		else
		{
			ptBrdAllIpRouteInfo->tBrdIpRouteParam[byIdx].dwGwIpAdrs = INET_ADDR(pchToken);
		}

		// RoutePri
		pchToken = strtok( NULL, achSeps );
		if ( NULL == pchToken )
		{
			ptBrdAllIpRouteInfo->tBrdIpRouteParam[byIdx].dwRoutePri = 0;
		}
		else
		{
			ptBrdAllIpRouteInfo->tBrdIpRouteParam[byIdx].dwRoutePri = atol(pchToken);
		}
	}

	return OK;
}

/*================================
��������BrdAddOneIpRoute
���ܣ�����һ��·
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����ptBrdIpRouteParam��ָ����·����Ϣ��TBrdIpRouteParam�ṹָ��
����ֵ˵���� ���󷵻�ERROR���ɹ�����OK
==================================*/
STATUS BrdAddOneIpRoute(TBrdIpRouteParam *ptBrdIpRouteParam)
{
	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}

	s8 achProfileName[64];
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILENAME_CONFIG);

	TBrdAllIpRouteInfo tBrdAllIpRouteInfo;
	if ( ERROR == BrdGetAllIpRoute(&tBrdAllIpRouteInfo) )
	{
		return ERROR;
	}

	u32 dwIdx, dwIpRouteNum;
	dwIpRouteNum = tBrdAllIpRouteInfo.dwIpRouteNum;
	for ( dwIdx = 0; dwIdx < dwIpRouteNum; dwIdx++ )
	{
		if ( ptBrdIpRouteParam->dwDesIpNet == tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwDesIpNet 
			&& ptBrdIpRouteParam->dwDesIpMask == tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwDesIpMask
			&& ptBrdIpRouteParam->dwGwIpAdrs == tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwGwIpAdrs
		   )
		{
			return OK;	
		}
	}

	// ����·��
	s8 achIpRouteEntry[32];
	s8 achIpRoute[64];

	memset( achIpRouteEntry, 0, sizeof(achIpRouteEntry) );
	sprintf( achIpRouteEntry, "%s%d", ENTRYPART_IpRoute, dwIpRouteNum+1 );

	memset( achIpRoute, 0, sizeof(achIpRoute) );
	u32 dwIpNet = ntohl(ptBrdIpRouteParam->dwDesIpNet);
	u32 dwIpMask = ntohl(ptBrdIpRouteParam->dwDesIpMask);
	u32 dwGWAddr = ntohl(ptBrdIpRouteParam->dwGwIpAdrs);
	u32 dwGWPeri = ptBrdIpRouteParam->dwRoutePri;
	sprintf( achIpRoute, "%s.%s.%s.%s\t%s.%s.%s.%s\t%s.%s.%s.%s\t%d",
				(dwIpNet>>24)&0xff, (dwIpNet>>16)&0xff, (dwIpNet>>8)&0xff, dwIpNet&0xff,
				(dwIpMask>>24)&0xff, (dwIpMask>>16)&0xff, (dwIpMask>>8)&0xff, dwIpMask&0xff,
				(dwGWAddr>>24)&0xff, (dwGWAddr>>16)&0xff, (dwGWAddr>>8)&0xff, dwGWAddr&0xff,
				dwGWPeri );
	BOOL32 bRet = ::SetRegKeyString( achProfileName, SECTION_IpRoute, achIpRouteEntry, achIpRoute );
	if ( !bRet )
	{
		return ERROR;
	}

	dwIpRouteNum++;

	bRet = ::SetRegKeyInt( achProfileName, SECTION_IpRoute, KEY_IpRouteNum, dwIpRouteNum );
	if ( !bRet )
	{
		return ERROR;
	}

	return OK;
}

/*================================
��������BrdDelOneIpRoute
���ܣ�ɾ��һ��·
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����ptBrdIpRouteParam��ָ����·����Ϣ��TBrdIpRouteParam�ṹָ��
����ֵ˵���� ���󷵻�ERROR���ɹ�����OK
==================================*/
STATUS BrdDelOneIpRoute(TBrdIpRouteParam *ptBrdIpRouteParam)
{
	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}

	s8 achProfileName[64];
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILENAME_CONFIG);

	TBrdAllIpRouteInfo tBrdAllIpRouteInfo;
	if ( ERROR == BrdGetAllIpRoute(&tBrdAllIpRouteInfo) )
	{
		return ERROR;
	}

	u32 dwIdx, dwIpRouteNum;
	dwIpRouteNum = tBrdAllIpRouteInfo.dwIpRouteNum;
	for ( dwIdx = 0; dwIdx < dwIpRouteNum; dwIdx++ )
	{
		if ( ptBrdIpRouteParam->dwDesIpNet == tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwDesIpNet 
			&& ptBrdIpRouteParam->dwDesIpMask == tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwDesIpMask
			&& ptBrdIpRouteParam->dwGwIpAdrs == tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwGwIpAdrs
		   )
		{
			memset( &tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx], 0, sizeof(TBrdIpRouteParam) );
			break;	
		}
	}

	if ( dwIdx == dwIpRouteNum )
	{
		return ERROR;
	}

	// ɾ��IpRoute��
	FILE *stream = fopen( achProfileName, "rb+" );
	if( stream == NULL )
	{
		return ERROR;
	}
	s32 nOffset = DelSection( stream, SECTION_IpRoute );
	fclose( stream );
	if ( nOffset == -1 )
	{
		return ERROR;
	}
	else
	{
		if ( !TrimCfgFile( achProfileName ) )
		{
			return ERROR;
		}
	}

	for ( dwIdx = 0; dwIdx < dwIpRouteNum; dwIdx++ )
	{
		if ( tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwDesIpNet != 0 
			|| tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwDesIpMask != 0
			|| tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwGwIpAdrs != 0 )
		{
			if ( ERROR == BrdAddOneIpRoute( &tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx] ) )
			{
				return ERROR;
			}		
		}
	}
	return OK;
}

/*================================
��������BrdGetDefGateway
���ܣ���ȡĬ������ip
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵���� ��
����ֵ˵���� ���󷵻�0���ɹ����س����͵�ip��ַ��������
==================================*/
u32 BrdGetDefGateway(void)
{
	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}
	
	TBrdAllIpRouteInfo tBrdAllIpRouteInfo;
	if ( ERROR == BrdGetAllIpRoute(&tBrdAllIpRouteInfo) )
	{
		return 0;
	}

	u32 dwIpRouteNum = tBrdAllIpRouteInfo.dwIpRouteNum;

	u8 dwIdx = 0;
	u32 dwGWIpAdrs = 0;
	s32 nRoutePeri = -1;
	for ( dwIdx = 0; dwIdx < dwIpRouteNum; dwIdx++ )
	{
		if ( 0 == tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwDesIpNet
			&& 0 == tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwDesIpMask
			&& 0 != tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwGwIpAdrs )
		{
			if ( (s32)tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwRoutePri > nRoutePeri )
			{
				dwGWIpAdrs = tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwGwIpAdrs;
				nRoutePeri = tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwRoutePri;
			}
		}
	}

	return dwGWIpAdrs;
}

/*================================
��������BrdDelDefGateway
���ܣ�ɾ��Ĭ������ip
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵���� ��
����ֵ˵���� ���󷵻�ERROR���ɹ�����OK
==================================*/
STATUS BrdDelDefGateway(void)
{
	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}

	TBrdAllIpRouteInfo tBrdAllIpRouteInfo;
	if ( ERROR == BrdGetAllIpRoute(&tBrdAllIpRouteInfo) )
	{
		return OK;
	}

	u32 dwIpRouteNum = tBrdAllIpRouteInfo.dwIpRouteNum;

	u8 dwIdx = 0;
	u32 dwGWIpAdrs = 0;
	s32 nRoutePeri = -1;
	for ( dwIdx = 0; dwIdx < dwIpRouteNum; dwIdx++ )
	{
		if ( 0 == tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwDesIpNet
			&& 0 == tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwDesIpMask
			&& 0 != tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwGwIpAdrs )
		{
			if ( ERROR == BrdDelOneIpRoute(&tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx]) )
			{
				return ERROR;
			}
		}
	}

	return OK;
}

/*================================
��������BrdSetDefGateway
���ܣ�����Ĭ������ip
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  dwIpAdrs��Ĭ������ip��ַ�������ͣ�������
����ֵ˵���� ������ERROR���ɹ�����OK��
==================================*/
STATUS BrdSetDefGateway(u32 dwIpAdrs)
{
	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}

	TBrdIpRouteParam tBrdIpRouteParam;
	tBrdIpRouteParam.dwDesIpMask = 0;
	tBrdIpRouteParam.dwDesIpNet = 0;
	tBrdIpRouteParam.dwGwIpAdrs = dwIpAdrs;
	if ( ERROR == BrdAddOneIpRoute( &tBrdIpRouteParam ) )
	{
		return ERROR;
	}

	return OK;
}
/*================================
��������BrdGetNextHopIpAddr
���ܣ���ȡͨ��ָ��ip�ĵ�һ��·��ip��ַ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����dwDstIpAddr��Ŀ��ip��ַ�������ͣ�������
		    dwDstMask��Ŀ�����룬�����ͣ�������
����ֵ˵���� ���󷵻�0���ɹ����ص�һ��·��ip��ַ��������
==================================*/
u32 BrdGetNextHopIpAddr(u32 dwDstIpAddr,u32 dwDstMask)
{
	return OK;
}

/*================================
��������BrdGetRouteWayBandwidth
���ܣ���ȡ��E1������·�Ĵ���ֵ����KbpsΪ��λ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵������
����ֵ˵���� ��·����ֵ����KbpsΪ��λ
==================================*/
u32 BrdGetRouteWayBandwidth(void)
{
	return OK;
}





/*================================
��������BrdChkOneIpStatus
���ܣ����ָ��ip�������е�״̬
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����dwIpAdrs��������ip��ַ��������
����ֵ˵���� ����ָ��ip�������е�״̬��
#define IP_SET_AND_UP	        1  ---address set and up 
#define IP_SET_AND_DOWN	        2  ---address set and down
#define IP_NOT_SET	            3  ---address not set
==================================*/
u32 BrdChkOneIpStatus(u32 dwIpAdrs)
{
	return IP_SET_AND_UP;
}

/*================================
��������BrdSaveNipConfig
���ܣ�����nip�����������ļ�
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵������
����ֵ˵���� ���󷵻�ERROR���ɹ�����OK
==================================*/
STATUS BrdSaveNipConfig(void)
{
	return OK;
}

/*================================
��������BrdIpConflictCallBackReg
���ܣ�ע��ip��ַ��ͻʱ�ص�����,���豸��IP��ַ���ⲿ�豸��ͻʱ������øú���֪ͨҵ�����
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����ptFunc��ִ��ҵ�����ע��Ļص�����ָ��
����ֵ˵���� ���󷵻�ERROR���ɹ�����OK
==================================*/
STATUS BrdIpConflictCallBackReg (TIpConflictCallBack  ptFunc)
{
	return OK;
}

/*================================
��������BrdIpOnceConflicted
���ܣ���ѯϵͳ�Ƿ�����ip��ַ��ͻ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵������
����ֵ˵���� TRUE or FALSE
==================================*/
BOOL32 BrdIpOnceConflicted (void)
{
	return OK;
}

/*================================
��������BrdPing
���ܣ���̫��ping�ӿ�
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����pchDestIP:Ping��Ŀ��IP��ַ
              ptPingOpt��Ping�Ĳ����ṹ��ָ��
              nUserID:�û�ID��־���û����ֲ�ͬ���û�����
              ptCallBackFunc:Ping����Ļص�����
����ֵ˵���� TRUE--����Ĳ����Ϸ����ú������óɹ������Ƿ�pingͨ��Ҫ�û�ע��Ļص��������ж�
             FALSE--����Ĳ����Ƿ����ú�������ʧ��
==================================*/
BOOL32 BrdPing(s8* pchDestIP,TPingOpt* ptPingOpt,s32 nUserID,TPingCallBack ptCallBackFunc)
{
	return TRUE;
}

/*================================
��������BrdGetEthActLnkStat
���ܣ���ʵ����̫������״̬���
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����byEthID��Ҫ��ѯ����̫��������
����ֵ˵���� 0=down/1=up
==================================*/
u8 BrdGetEthActLnkStat(u8 byEthID)
{
	return 1;
}

/*================================
��������BrdSetV35Param  
���ܣ�����v35�ӿڲ���
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  dwDevId��v35�ӿڵı�ţ�0 ~ X��X�Ӳ�ͬ���Ӷ�����
                dwIpAdrs��Ϊip��ַ
                dwIpMask��Ϊ�����ַ
����ֵ˵���� ������ERROR���ɹ�����OK��
==================================*/
STATUS BrdSetV35Param(u32 dwDevId, u32 dwIpAdrs, u32 dwIpMask)
{
	return OK;
}

/*================================
��������BrdGetV35Param
���ܣ���ȡv35�ӿڲ���
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  dwDevId��v35�ӿڵı�ţ�0 ~ X��X�Ӳ�ͬ���Ӷ�����
pdwIpAdrs��ָ���ŷ���ip��ַ��ָ��
pdwIpMask: ָ���ŷ��������ַ��ָ��
����ֵ˵���� ������ERROR���ɹ�����OK��
==================================*/
STATUS BrdGetV35Param(u32 dwDevId, u32 *pdwIpAdrs, u32 *pdwIpMask)
{
	return OK;
}

/*================================
��������BrdDelV35Param
���ܣ�ɾ��ָ����v35�ӿڵ�ַ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  dwDevId��v35�ӿڵı�ţ�0 ~ X��X�Ӳ�ͬ���Ӷ���
����ֵ˵���� ������ERROR���ɹ�����OK��
==================================*/
STATUS BrdDelV35Param(u32 dwDevId)
{
	return OK;
}

/*================================
��������BrdLineIsLoop
���ܣ���鵱ǰE1��·�Ի�״̬(����ж��E1��·�Ի������ҵ���һ���Ի���E1��·������IP��ַ)
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����  pdwIpAddr����Žӿ�ip��ַ��ָ�룬���ڷ���ֵΪTRUEʱ��Ч
����ֵ˵���� �Ի��򷵻�TRUE ��û���Ի��򷵻�FALSE
==================================*/
BOOL32   BrdLineIsLoop(u32 *pdwIpAddr)
{
	return OK;
}

/*================================
��������BrdGetE1MaxNum
���ܣ���ѯ�豸�������õ�e1����
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵������
����ֵ˵���� ������õ�e1����
==================================*/
u8 BrdGetE1MaxNum(void)
{
	return OK;
}

/*================================
��������BrdSetE1SyncClkOutputState
���ܣ�Ŀǰ����DRI��DSI��Ч����Ҫ���ô�E1��·����ȡ������ͬ���ο�ʱ���Ƿ������MPC�����ͬ����
      ͬһʱ�̾��Խ�ֹ��ͬ�İ���ͬʱ���ʱ�ӣ�����MPC�޷�ͬ��
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����byMode����ͬ���ο�ʱ�����ģʽ���������£�
                #define E1_ CLK_OUTPUT_DISABLE   ((u8)0)   �����ֹ����ʱ���ź������MPC����
                #define E1_ CLK_OUTPUT_ENABLE    ((u8)1)   �������ʱ���ź������MPC����
����ֵ˵���� ���󷵻�ERROR���ɹ�����OK
==================================*/
STATUS BrdSetE1SyncClkOutputState (u8 byMode)
{
	return OK;
}

/*================================
��������BrdSetE1RelayLoopMode
���ܣ�Ŀǰ����DRI��DSI��KDV8010��Ч����Ҫ����E1��·�̵������أ��������ڻ����⻷��
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����byE1ID��E1��·��ID�ţ����ֵ������豸��أ���������������E1�����궨��;
            byMode���̵�������ģʽ���ã��������£�
            #define BRD_RELAY_MODE_NORMAL     0    �̵�����ͨ
            #define BRD_RELAY_MODE_LOCLOOP    1    �̵����Ի�
            #define BRD_RELAY_MODE_OUTLOOP    2    �̵����⻷
            ˵����
            1����DRI��˵��ֻ���������ڻ����ã�
            2����DSI��˵��ֻ���������⻷���ã�
            3����MT��˵��ֻ���������ڻ����ã��Һ������ģ����ء�
����ֵ˵���� ���󷵻�ERROR���ɹ�����OK
==================================*/
STATUS BrdSetE1RelayLoopMode(u8 byE1ID,u8 byMode)
{
	return OK;
}

/*================================
��������BrdSelectE1NetSyncClk
���ܣ�Ŀǰ����DRI��DSI��Ч����Ҫ������ͬ���ο�ʱ�����ѡ��
ע�⣺�ù��ܱ�����BrdSetE1SyncClkOutputState������ͬ���ο�ʱ���������������²���Ч
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����byE1ID��E1��·��ID�ţ����ֵ������豸��أ���������������E1�����궨��
����ֵ˵���� ���󷵻�ERROR���ɹ�����OK
==================================*/
STATUS BrdSelectE1NetSyncClk(u8 byE1ID)
{
	return OK;
}

/*================================
��������BrdQueryE1Imp
���ܣ�Ŀǰ����DRI��DSI��KDV8010��Ч����Ҫ��ȡָ��e1�迹ֵ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����byE1ID��E1��·��ID�ţ����ֵ������豸��أ���������������E1�����궨��
����ֵ˵���� ���󷵻�0xff���ɹ������迹ֵ�궨��
==================================*/
u8  BrdQueryE1Imp(u8 byE1ID)
{
	return OK;
}

/*================================
��������BrdGetE1AlmState
���ܣ���ȡָ��E1��·�ĸ澯״̬
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����byE1ID��E1��·��ID�ţ����ֵ������豸��أ���������������E1�����궨��
����ֵ˵���� ����0Ϊ��������0�и澯
==================================*/
u8  BrdGetE1AlmState(u8 byE1Id)
{
	return OK;
}


/*================================
��������E1TransGetMaxE1Num
���ܣ���ȡ�豸ʵ��֧�ֵ�E1����
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵������
����ֵ˵���� ���󷵻�0����ӡ������Ϣ���ɹ�, �����豸ʵ��֧�ֵ�E1����
==================================*/
/*
u8 E1TransGetMaxE1Num(void)
{
	return 1;
}
*/

/*================================
��������E1TransChanOpen
���ܣ���һ��E1͸������ͨ������ʼ����Ӧ��mcc_channel
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����byE1ChanID���򿪵�ͨ����,����С�� E1_TRANS_CHAN_MAX_NUM
		      ptChanParam��ͨ���Ĳ�������
              ptE1MsgCallBack��ע��Ļص�����
����ֵ˵���� E1_TRANS_FAILURE/E1_TRANS_SUCCESS
==================================*/
/*
s32 E1TransChanOpen( u8 byE1ChanID,TE1TransChanParam *ptChanParam,TE1MsgCallBack  ptE1MsgCallBack )
{
	return E1_TRANS_SUCCESS;
}
*/

/*================================
��������E1TransChanClose
���ܣ��ر�mcc������һ��E1͸������ͨ��
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����byE1ChanID���򿪵�ͨ����,����С�� E1_TRANS_CHAN_MAX_NUM
����ֵ˵���� E1_TRANS_FAILURE/E1_TRANS_SUCCESS
==================================*/
/*
s32 E1TransChanClose( u8 byE1ChanID)
{
	return E1_TRANS_SUCCESS;
}
*/

/*================================
��������E1TransChanMsgSnd
���ܣ�����ģʽ�������ݰ�
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����byE1ChanID:�豸��
		      tMsgParam:��Ϣ������Ϣָ��
����ֵ˵���� ��E1͸�����䲿�ֵķ���ֵ�궨��
==================================*/
/*
s32 E1TransChanMsgSnd(u8 byE1ChanID, TE1TransMsgParam *ptMsgParam)
{
	return E1_TRANS_SUCCESS;
}
*/

/*================================
��������E1TransTxPacketNumGet
���ܣ���ȡָ��ͨ�����ͻ����������д����͵����ݰ��ĸ���
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����byE1ChanID:�豸��
����ֵ˵���� ������E1_TRANS_FAILURE���ɹ����ض����д����͵����ݰ��ĸ���
==================================*/
/*
s32 E1TransTxPacketNumGet(u8 byE1ChanID)
{
	return 1;
}
*/

/*================================
��������E1TransBufQHeadPtrInc
���ܣ��ƶ���ָ��
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����byE1ChanID:�豸��
              dwOffset����ַƫ��
����ֵ˵���� ��E1͸�����䲿�ֵķ���ֵ�궨��
==================================*/
/*
s32 E1TransBufQHeadPtrInc(u8 byE1ChanID, u32 dwOffset)
{
	return E1_TRANS_SUCCESS;
}
*/

/*================================
��������E1TransBufMsgCopy
���ܣ����ж�ȡָ�����ȵ����ݵ�ָ�������������ƶ���ָ��
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����byE1ChanID:�豸��
              pbyDstBuf:�������ݱ�����
              dwSize:������������
����ֵ˵���� ���󷵻�0���ɹ����ض������ֽ���
==================================*/
/*
s32 E1TransBufMsgCopy(u8 byE1ChanID, u8 *pbyDstBuf, u32 dwSize)
{
	return dwSize;
}
*/

/*================================
��������E1TransBufMsgLenGet
���ܣ���ѯ��ǰ�ɶ����ݵĳ���
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����byE1ChanID:�豸��
����ֵ˵���� ���󷵻�0���ɹ����ؿɶ����ݳ���
==================================*/
/*
s32 E1TransBufMsgLenGet(u8 byE1ChanID)
{
	return 1;
}
*/

/*================================
��������E1TransBufMsgByteRead
���ܣ���ȡ��ǰ��ָ�뿪ʼָ��ƫ�Ƶ�һ���ַ������ƶ���ָ��
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����byE1ChanID:�豸��
              dwOffset:��ַƫ��
              pbyRtnByte:���ֽ����ݵ�ָ��
����ֵ˵���� ��E1͸�����䲿�ֵķ���ֵ�궨��
==================================*/
/*
s32 E1TransBufMsgByteRead(u8 byE1ChanID, u32 dwOffset, u8 *pbyRtnByte)
{
	if ( NULL == pbyRtnByte )
		return E1_TRANS_NO_BUF;

	return E1_TRANS_SUCCESS;
}
*/

/*================================
��������E1TransChanLocalLoopSet
���ܣ���ָ��ͨ��������·�Ի�
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����byE1ChanID:�豸��
              bIsLoop���Ƿ񽫸���·����Ϊ�Ի�ģʽ
����ֵ˵���� ��E1͸�����䲿�ֵķ���ֵ�궨��
==================================*/
/*
s32 E1TransChanLocalLoopSet(u8 byE1ChanID, BOOL32 bIsLoop)
{
	return E1_TRANS_SUCCESS;
}
*/

/*================================
��������E1TransChanInfoGet
���ܣ���ȡָ��ͨ�������շ���ͳ����Ϣ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����byE1ChanID:�豸��
              ptInfo:��ͳ����Ϣ��ָ��
����ֵ˵���� ��E1͸�����䲿�ֵķ���ֵ�궨��
==================================*/
/*
s32 E1TransChanInfoGet(u8 byE1ChanID, TE1TransChanStat *ptInfo)
{
	if ( NULL == ptInfo )
		return E1_TRANS_NO_BUF;

	return E1_TRANS_SUCCESS;
}
*/

/*================================
��������BrdOpenE1SingleLinkChan
���ܣ�����һ��E1����·����ͨ��
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�����
�������˵����dwChanID: ����·����ͨ���ţ���Χ0~ E1_SINGLE_LINK_CHAN_MAX_NUM -1����ID����Ψһ�ԣ�������ʶÿ������·����ͨ�������ɳ�ͻ��һ��ͨ�������ظ��򿪣������ȹر�
              ptChanParam:����·����ͨ�������ṹָ��
����ֵ˵���� E1_RETURN_OK/ERRCODE
==================================*/
STATUS BrdOpenE1SingleLinkChan(u32 dwChanID, TBrdE1SingleLinkChanInfo *ptChanParam)
{
	if ( NULL == ptChanParam )
		return E1_ERR_PARAM_EXCEPTION;

	return E1_RETURN_OK;
}

/*====================================================================
������      : BrdCloseE1SingleLinkChan
����        ���ر�ָ����E1����·����ͨ��
�㷨ʵ��    ����
����ȫ�ֱ�������
�������˵����dwChanID: ����·����ͨ���ţ���Χ0~ E1_SINGLE_LINK_CHAN_MAX_NUM -1��
            ��ID����Ψһ�ԣ�������ʶÿ������·����ͨ�������ɳ�ͻ��
            ��ͨ���������Ѿ��򿪳ɹ��ģ����δ���򷵻�E1_ERR_CHAN_NOT_CONF;
����ֵ˵��  ��E1_RETURN_OK/ERRCODE��
====================================================================*/
STATUS BrdCloseE1SingleLinkChan(u32 dwChanID)
{
	return E1_RETURN_OK;
}

/*====================================================================
������      : BrdGetE1SingleLinkChanInfo
����        ����ȡָ����E1����·����ͨ����Ϣ
�㷨ʵ��    ������·�����ṩ��nipģ��Ľӿڡ�
����ȫ�ֱ�������
�������˵����dwChanID: ����·����ͨ���ţ���Χ0~ E1_SINGLE_LINK_CHAN_MAX_NUM -1��
            ��ID����Ψһ�ԣ�������ʶÿ������·����ͨ�������ɳ�ͻ��
            ��ͨ���������Ѿ��򿪳ɹ��ģ����δ���򷵻�E1_ERR_CHAN_NOT_CONF;
                ptChanInfo:����·����ͨ�������ṹָ��.
����ֵ˵��  ��E1_RETURN_OK/ERRCODE��
====================================================================*/
STATUS BrdGetE1SingleLinkChanInfo(u32 dwChanID, TBrdE1SingleLinkChanInfo *ptChanInfo)
{
	if ( NULL == ptChanInfo )
		return E1_ERR_PARAM_EXCEPTION;

	return E1_RETURN_OK;
}

/*====================================================================
������      : BrdOpenE1MultiLinkChan
����        ������һ��E1����·��������ͨ��
�㷨ʵ��    ������·�����ṩ��nipģ��Ľӿڡ�
����ȫ�ֱ�������
�������˵����dwChanID: ����·��������ͨ���ţ���Χ0~ dwMultiLinkChanNum -1��
            ��ID����Ψһ�ԣ�������ʶÿ������·����ͨ�������ɳ�ͻ��һ��ͨ�������ظ��򿪣�
            �����ȹر�;
             ptChanParam:����·��������ͨ�������ṹָ��.
����ֵ˵��  ��E1_RETURN_OK/ERRCODE��
====================================================================*/
STATUS BrdOpenE1MultiLinkChan(u32 dwChanID, TBrdE1MultiLinkChanInfo *ptChanParam)
{
	if ( NULL == ptChanParam )
		return E1_ERR_PARAM_EXCEPTION;

	return E1_RETURN_OK;
}

/*====================================================================
������      : BrdCloseE1MultiLinkChan
����        ���ر�ָ����E1����·��������ͨ��
�㷨ʵ��    ����
����ȫ�ֱ�������
�������˵����dwChanID: ����·��������ͨ���ţ���Χ0~ dwMultiLinkChanNum -1��
            ��ID����Ψһ�ԣ�������ʶÿ������·��������ͨ�������ɳ�ͻ��
            ��ͨ���������Ѿ��򿪳ɹ��ģ����δ���򷵻�E1_ERR_CHAN_NOT_CONF;
����ֵ˵��  ��E1_RETURN_OK/ERRCODE��
====================================================================*/
STATUS BrdCloseE1MultiLinkChan(u32 dwChanID)
{
	return E1_RETURN_OK;
}
/*====================================================================
������      : BrdGetE1MultiLinkChanInfo
����        ����ȡָ����E1����·��������ͨ����Ϣ
�㷨ʵ��    ������·�����ṩ��nipģ��Ľӿڡ�
����ȫ�ֱ�������
�������˵����dwChanID: ����·��������ͨ���ţ���Χ0~ dwMultiLinkChanNum -1��
            ��ID����Ψһ�ԣ�������ʶÿ������·����ͨ�������ɳ�ͻ��
            ��ͨ���������Ѿ��򿪳ɹ��ģ����δ���򷵻�E1_ERR_CHAN_NOT_CONF;
            ptChanInfo:����·��������ͨ�������ṹָ��.
����ֵ˵��  ��E1_RETURN_OK/ERRCODE��
====================================================================*/
STATUS BrdGetE1MultiLinkChanInfo(u32 dwChanID, TBrdE1MultiLinkChanInfo *ptChanInfo)
{
	if ( NULL == ptChanInfo )
		return E1_ERR_PARAM_EXCEPTION;

	return E1_RETURN_OK;
}

/*RawFlash��صĺ�������*/

/*================================
�������� BrdRawFlashIsUsed
���ܣ���ѯ�Ƿ����rawflash����
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�������
�������˵����  ��
����ֵ˵���� TRUE:ʹ��/FALSE:��ʹ��
==================================*/
BOOL32   BrdRawFlashIsUsed(void)
{
	return TRUE;
}

/*================================
��������BrdGetFullRamDiskFileName
���ܣ�ת�����·�����ļ���Ϊramdisk�д�����·�����ļ���
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�������
�������˵���� pInFileName:���·�����ļ���,��"webfiles/doc/www.html"; 
               pRtnFileName:��ŷ��ؾ���·���ļ����ĵ�ַָ�롣
               ��������ӻ����:"/ramdisk/webfiles/doc/www.html"
����ֵ˵���� ������·���ļ����ĳ��ȡ�
==================================*/
u32 BrdGetFullRamDiskFileName(s8 *pInFileName, s8 *pRtnFileName)
{
	if ( NULL == pInFileName || NULL == pRtnFileName )
		return 0;

	memcpy( pRtnFileName, "/ramdisk/", sizeof("/ramdisk/") );
	strcat( pRtnFileName, pInFileName );
	return strlen(pRtnFileName); 
}

/*================================
��������BrdFpUnzipFile
���ܣ���ѹ���Ϸ�����ָ�����ļ����������ָ�����ļ���
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�������
�������˵���� pUnzipFileName:����ѹ���ļ������ɺ����·������"/webfiles/doc/aaa.html"��
               pOuputFileName:��Ž�ѹ�����ݵ��ļ�����Ŀǰ�����ѹ��/ramdisk/�У���"/ramdisk/bbb.html"
����ֵ˵��  ��OK/ERROR��
==================================*/
STATUS BrdFpUnzipFile(s8 *pUnzipFileName, s8 *pOuputFileName)
{
	return OK;
}

/*================================
��������BrdFpUnzipFileIsExist
���ܣ���ѯ���Ϸ���ѹ������ָ����ѹ���ļ��Ƿ���ڡ�
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�������
�������˵���� pUnzipFileName:����ѹ���ļ���
����ֵ˵���� TRUE/FALSE
==================================*/
BOOL32   BrdFpUnzipFileIsExist(s8 *pUnzipFileName)
{
	if ( NULL == pUnzipFileName )
		return FALSE;

	return TRUE;
}

/*================================
��������BrdFpPartition
���ܣ�������
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�������
�������˵���� ptParam��ָ��TFlPartnParam�ṹ��������ָ�롣
����ֵ˵���� OK/ERROR��
==================================*/
STATUS BrdFpPartition(TFlPartnParam *ptParam)
{
	if ( NULL == ptParam )
		return ERROR;

	return OK;
}

/*================================
��������BrdFpGetFPtnInfo
���ܣ���������Ϣ
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�������
�������˵���� ptParam��ָ��TFlPartnParam�ṹ��������ָ�롣
����ֵ˵���� OK/ERROR��
==================================*/
STATUS BrdFpGetFPtnInfo(TFlPartnParam *ptParam)
{
	if ( NULL == ptParam )
		return ERROR;

	return OK;
}

/*================================
��������BrdFpGetExecDataInfo
���ܣ���ȡ�����ִ�г�������͡���ַ�ʹ�С 
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�������
�������˵���� pdwExecAdrs:��������ִ��������Ե�ַ�Ļ�������ָ�룻
                pdwExeCodeType:��������ִ���������͵Ļ�������ָ�롣
����ֵ˵���� -1����/����ֵΪ�����ִ�����ݵĴ�С��
==================================*/
s32  BrdFpGetExecDataInfo(u32 *pdwExecAdrs, u32 *pdwExeCodeType)
{
	if ( NULL == pdwExecAdrs || NULL == pdwExeCodeType )
		return -1;

	return 1024;
}

/*================================
��������BrdFpReadExecData
���ܣ���ȡ�����ִ�г�������
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�������
�������˵���� pbyDesBuf:��������ִ�����ݵĻ�������ָ��;
              dwLen: Ҫ��ȡ�����ݵĳ��ȡ�
����ֵ˵���� -1����/����ֵΪ��ȡ�����ִ�г������ݵĳ��ȡ�
==================================*/
s32  BrdFpReadExecData(u8 *pbyDesBuf, u32 dwLen)
{
	if ( NULL == pbyDesBuf )
		return -1;

	return dwLen;
}

/*================================
��������BrdFpUpdateExecData
���ܣ����������ִ�г�������
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�������
�������˵���� pFile:FPGA�ļ�����
����ֵ˵���� OK/ERROR��
==================================*/
STATUS BrdFpUpdateExecData(s8* pFile)
{
	if (NULL == pFile)
		return ERROR;

	return OK;
}

/*================================
��������BrdFpWriteExecData
���ܣ�����ExeCode�����ִ�г�������
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�������
�������˵���� ��
����ֵ˵���� OK/ERROR��
==================================*/
STATUS BrdFpEraseExeCode(void)
{
	return OK;
}

/*================================
��������BrdFpUpdateAuxData
���ܣ�����IOS�����е�����(IOS�����е�������ɣ�kernel+ramdisk�����У�kernel��ramdisk��·���鷢����--update.linux)
�㷨ʵ�֣�����ѡ�
����ȫ�ֱ�������
�������˵���� pFile:update.linux�ļ�����
����ֵ˵���� OK/ERROR��
==================================*/
STATUS BrdFpUpdateAuxData(s8* pFile)
{
	if ( NULL == pFile )
		return ERROR;
	
	return OK;
}

/*================================
������      : BrdFpEraseAuxData
����        ����ȡ�û����ݷ����ĸ�����
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�����
�������˵�����ޡ�
����ֵ˵��  ���û������ĸ���
==================================*/
u8  BrdFpGetUsrFpnNum (void)
{
	return 1;
}

/*================================
������      : BrdFpWriteDataToUsrFpn
����        ��дָ�����������ݵ�ָ�����û����ݷ���
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�����
�������˵����byIndex���ڼ����û���������0��ʼ��
              pbyData��Ҫд�������ָ��
              dwLen��Ҫд�����ݵĳ���
����ֵ˵��  ��OK/ERROR��
==================================*/
STATUS BrdFpWriteDataToUsrFpn(u8 byIndex, u8 *pbyData, u32 dwLen)
{
	if ( NULL == pbyData )
		return ERROR;

	return OK;
}

/*================================
������      : BrdFpWriteDataToUsrFpn
����        ����ȡָ��������ָ����ָ�����ȵ���Ч���ݵ�������
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�����
�������˵����byIndex���ڼ����û���������0��ʼ��
              pbyData��Ҫ����������ָ��
              dwLen��Ҫ�������ݵĳ���
����ֵ˵��  ��ʵ�ʶ�ȡ�����ݳ���
==================================*/
s32  BrdFpReadDataFromUsrFpn(u8 bySection, u8 *pbyDesBuf, u32 dwLen)
{
	if ( NULL == pbyDesBuf )
		return ERROR;

	return OK;
}

/*================================
������      : BrdMPCQueryAnotherMPCState
����        ���Զ�����������Ƿ���λ��ѯ
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�����
�������˵������
����ֵ˵��  ���궨�壺
		  #define BRD_MPC_OUTOF_POSITION      ((u8)0)   //�Զ���������岻��λ
          #define BRD_MPC_IN_POSITION               ((u8)1)   //�Զ������������λ
==================================*/
u8 BrdMPCQueryAnotherMPCState(void)
{
	return BRD_MPC_IN_POSITION;
}

/*================================
������      : BrdMPCQueryLocalMSState
����        ����ǰ���������������״̬��ѯ
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�����
�������˵������
����ֵ˵��  ���궨�壺
		  #define BRD_MPC_RUN_MASTER      ((u8)0)   // ��ǰ�������������
          #define BRD_MPC_RUN_SLAVE               ((u8)1)   // ��ǰ��������屸�� 
==================================*/
u8 BrdMPCQueryLocalMSState (void)
{
	return BRD_MPC_RUN_MASTER;
}

/*================================
������      : BrdMPCSetLocalMSState
����        �����õ�ǰ���������������״̬
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�����
�������˵����byState:
              #define BRD_MPC_RUN_MASTER          0      // ��ǰ�������������
              #define BRD_MPC_RUN_SLAVE           1      // ��ǰ��������屸�� 
����ֵ˵��  ��OK/ERROR
==================================*/
STATUS BrdMPCSetLocalMSState (u8 byState)
{
	return OK;
}

/*================================
������      : BrdMPCQuerySDHType
����        ����ģ�������ѯ
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�����
�������˵������
����ֵ˵��  ���궨�壺
		#define BRD_MPC_SDHMODULE_NONE      ((u8)0x07)   // ��ǰ���������û�в�ģ�� 
        #define BRD_MPC_SDHMODULE_SOI1      ((u8)0x00)   // ��ǰ����������ģ��SOI1 
        #define BRD_MPC_SDHMODULE_SOI4      ((u8)0x01)   // ��ǰ����������ģ��SOI4  
==================================*/
u8 BrdMPCQuerySDHType (void)
{
	return BRD_MPC_SDHMODULE_SOI1;
}

/*================================
������      : BrdMPCQueryNetSyncMode
����        �����໷����ģʽ��ѯ
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�����
�������˵������
����ֵ˵��  ���궨�壺
		#define SYNC_MODE_FREERUN           ((u8)0)   // �����񵴣�����MCUӦ����Ϊ��ģʽ�������¼���·ʱ�����MCUͬ��
        #define SYNC_MODE_TRACK_SDH8K       ((u8)1)   // ����ģʽ����Ϊ�¼�MCUӦ����Ϊ��ģʽ������SDH����8Kʱ��
        #define SYNC_MODE_TRACK_SDH2M       ((u8)2)   // ����ģʽ����Ϊ�¼�MCUӦ����Ϊ��ģʽ������SDH����2Mʱ�� 
        #define SYNC_MODE_TRACK_DT2M        ((u8)4)   // ����ģʽ����Ϊ�¼�MCUӦ����Ϊ��ģʽ������DT����2Mʱ�� 
        #define SYNC_MODE_UNKNOWN           ((u8)0xff)// δ֪������ģʽ 
==================================*/
u8 BrdMPCQueryNetSyncMode (void)
{
	return SYNC_MODE_FREERUN;
}

/*================================
������      : BrdMPCSetNetSyncMode
����        �����໷��ͬ��ģʽѡ�����ã���֧��SDH����
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�����
�������˵����byMode�����໷��ͬ��ģʽ�������¶��壺
        #define SYNC_MODE_FREERUN           ((u8)0)   // �����񵴣�������MCUӦ����Ϊ��ģʽ�������¼���·ʱ�����MCUͬ�� 
        #define SYNC_MODE_TRACK_SDH8K       ((u8)1)   //����ģʽ����Ϊ�¼�MCUӦ����Ϊ��ģʽ������SDH����8Kʱ�� 
        #define SYNC_MODE_TRACK_SDH2M       ((u8)2)   // ����ģʽ����Ϊ�¼�MCUӦ����Ϊ��ģʽ������SDH����2Mʱ�� 
        #define SYNC_MODE_TRACK_DT2M        ((u8)4)   // ����ģʽ����Ϊ�¼�MCUӦ����Ϊ��ģʽ������DT����2Mʱ��
����ֵ˵��  �����󷵻�ERROR���ɹ�����OK
==================================*/
STATUS BrdMPCSetNetSyncMode(u8 byMode)
{
	return OK;
}

/*================================
������      : BrdMPCSetAllNetSyncMode
����        �����໷��ͬ��ģʽѡ������,����sdh����
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�����
�������˵����byMode�����໷��ͬ��ģʽ�������¶��壺
        #define SYNC_MODE_FREERUN           ((u8)0)   // �����񵴣�������MCUӦ����Ϊ��ģʽ�������¼���·ʱ�����MCUͬ�� 
        #define SYNC_MODE_TRACK_SDH8K       ((u8)1)   // ����ģʽ����Ϊ�¼�MCUӦ����Ϊ��ģʽ������SDH����8Kʱ�� 
        #define SYNC_MODE_TRACK_SDH2M       ((u8)2)   // ����ģʽ����Ϊ�¼�MCUӦ����Ϊ��ģʽ������SDH����2Mʱ�� 
        #define SYNC_MODE_TRACK_DT2M        ((u8)4)   // ����ģʽ����Ϊ�¼�MCUӦ����Ϊ��ģʽ������DT����2Mʱ��
        dwSdhE1Id������ͬ��ģʽΪSYNC_MODE_TRACK_SDH8K��SYNC_MODE_TRACK_SDH2Mʱ����ָ��sdh��Ӧ��e1�ţ������SOI-1ģ
        �鷶ΧΪ241-301�������SIO-4ģ��ʱ��ΧΪ241-484
����ֵ˵��  �����󷵻�ERROR���ɹ�����OK
==================================*/
STATUS BrdMPCSetAllNetSyncMode(u8 byMode, u32 dwSdhE1Id)
{
	return OK;
}

/*================================
������      : BrdMPCResetSDH
����        ����ģ�鸴λ
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�����
�������˵������
����ֵ˵��  �����󷵻�ERROR���ɹ�����OK
==================================*/
STATUS BrdMPCResetSDH (void)
{
	return OK;
}

/*================================
������      : BrdMPCReSetAnotherMPC
����        ����λ�Զ��������
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�����
�������˵������
����ֵ˵��  �����󷵻�ERROR���ɹ�����OK
==================================*/
STATUS BrdMPCReSetAnotherMPC (void)
{
	return OK;
}

/*================================
������      : BrdMPCOppReSetDisable
����        ����ֹ�Զ����������λ���������
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�����
�������˵������
����ֵ˵��  �����󷵻�ERROR���ɹ�����OK
==================================*/
STATUS BrdMPCOppReSetDisable (void)
{
	return OK;
}

/*================================
������      : BrdMPCOppReSetEnable 
����        ������Զ����������λ���������
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�����
�������˵������
����ֵ˵��  �����󷵻�ERROR���ɹ�����OK
==================================*/
STATUS BrdMPCOppReSetEnable (void)
{
	return OK;
}

/*================================
������      : BrdMPCLedBoardSpeakerSet
����        �����õư���������������״̬
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�����
�������˵����byState��������״̬�����º궨��
        #define LED_BOARD_SPK_ON                ((u8)0x01)   //����������
        #define LED_BOARD_SPK_OFF               ((u8)0x00)   // �ر�������
����ֵ˵��  �����󷵻�ERROR���ɹ�����OK
==================================*/
STATUS BrdMPCLedBoardSpeakerSet(u8 byState)
{
	return OK;
}

/*================================
������      : BrdFastLoadEqtFileIsEn
����        ���Ƿ�����bsp15��������
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�����
�������˵������
����ֵ˵��  ��0=��ֹ��������bsp15����flash�������ٶȽ���;
              1=�����������bsp15�����ڴ澵���������ٶȽϿ죬��ռ�ý϶��ڴ�;
==================================*/
u8 BrdFastLoadEqtFileIsEn(void)
{
	return 1;
}

/*================================
������      : AtaApiMountPartion
����        �����ش��̷���---Linux�������Ĵ��̲����ӿ�
�㷨ʵ��    ������ѡ�
����ȫ�ֱ�����
�������˵����pchPartionName:Ҫ���صĴ��̷������ƣ���"/dev/hda0"
              pMntPath:Ҫ���ص�Ŀ��·������"/ramdisk/hda0"
����ֵ˵��  �����󷵻���ata����ģ����������������룻�ɹ�, ����ATA_SUCCESS.
==================================*/
//s32 AtaApiMountPartion(s8* pchPartionName,s8* pMntPath)
//{
//	return ATA_SUCCESS;
//}

/*================================
������      : BrdNipErase
����        ������NIP������Ϣ
�㷨ʵ��    ���������ã�����consoleģʽ�µ��øú�����
              telnet�ϵ��øú����ᵼ��telnet�޷���������
              ������nip��IP��ַ��������telnet�޷�������������
����ȫ�ֱ�����
�������˵������
����ֵ˵��  �����󷵻�ERROR���ɹ�����OK
==================================*/
STATUS BrdNipErase(void)
{
	return OK;
}

/*================================
������      : BrdMicAdjustIsSupport
����        ����ѯ��ǰmic�����Ƿ�֧��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵������
����ֵ˵��  �����󷵻�FALSE���ɹ�����TRUE
==================================*/
BOOL32   BrdMicAdjustIsSupport(void)
{
	return TRUE;
}

/*================================
������      : BrdMicVolumeSet
����        ������mic��������
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����byTapPosition:���ף���Χ��0-127,������Ϊ���߼�
����ֵ˵��  �����󷵻�ERROR���ɹ�����OK
==================================*/
STATUS BrdMicVolumeSet(u8  byTapPosition)
{
	if ( 0 > byTapPosition || 127 < byTapPosition )
		return ERROR;

	return OK;
}

/*================================
������      : BrdMicVolumeGet
����        ����ѯ��ǰmic�������� 
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵������
����ֵ˵��  ����ǰmic��������(0-127)/0xff=����
==================================*/
u8 BrdMicVolumeGet(void)
{
	return OK;
}


