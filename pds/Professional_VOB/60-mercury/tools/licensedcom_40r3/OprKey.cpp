// OprKey.cpp : Implementation of COprKey
#include "stdafx.h"
#include "Mculicense.h"
#include "OprKey.h"
#include "osp.h"
#include "mcuconst.h"
#include "kdvencrypt.h"


/////////////////////////////////////////////////////////////////////////////
// COprKey

#define SUCCESS_KDVLICENSE              (u32)0 // �ɹ�
#define ERR_KDVLICENSE_AUTH_FAILED      (u32)1 // ��Ȩ����֤ʧ��
#define ERR_KDVLICENSE_GETPARA_FAILED   (u32)2 // ��ϸ��Ȩ��Ϣ��ȡʧ��
#define ERR_KDVLICENSE_INVALID_DEVICEID (u32)3 // �豸ID�Ƿ�
#define ERR_KDVLICENSE_INVALID_NUM      (u32)4 // ��������Ƿ�
#define ERR_KDVLICENSE_INVALID_DATE     (u32)5 // �������ڷǷ�
#define ERR_KDVLICENSE_INVALID_PRODUCT  (u32)6 // ��Ʒ���ͷǷ�
#define ERR_KDVLICENSE_WRITEKEY_FAILED  (u32)7 // дlicensekey�ļ�ʧ��
#define ERR_KDVLICENSE_UNKNOWN          (u32)100 // ����ԭ��Ĵ���

//ȱʡ����Ȩ�� ��klms.kedacom.com����md5�������32λ�����
#define DEFAULT_KDVLICENSE_AUTHCODE      (LPCSTR)"b2ae4e5afc3619dae447fd23dc0605b2"

//�ļ�����·��
#define DEFAULT_KEY_PATHNAME             (LPCSTR)"c:\\KedaLicenseKey\\"

//��ǰ�汾��
#define CURRENT_VERSION                 (u16)44

struct TLicenseParam 
{
    TLicenseParam(void) { memset(this, 0, sizeof(TLicenseParam)); }

	u16 wAuthNum;           //��Ȩ����
	s8  szAuthName[256];    //���֤����,��Ϊ�û���
    s8  szDeviceID[32];     //�豸��, ��mac addr
    TKdvTime tExpireDate;   //ʧЧ����
    u8  byProductID;        //��Ʒ����, T120 / MCU
    u16 wVersion;           //�汾��
};

/*=============================================================================
�� �� ���� GenEncodeData
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 *pOutBuf
           u16 wInBufLen
           u8 *pInBuf
           BOOL32 bDirect
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/10/19  4.0			������                  ����
=============================================================================*/
void GenEncodeData(u8 *pOutBuf, u16 wInBufLen, u8 *pInBuf, BOOL32 bDirect)
{
    s8 achIV[MAX_IV_SIZE] = {0};
	
    s8 achKey[32] = {0};
	strncpy( achKey, KEDA_AES_KEY, sizeof(KEDA_AES_KEY) );// ����keyΪ16�ı���
    u16 wKeyLen = strlen(achKey);

    s32 nRet;
	if( bDirect ) // eccrypt 
	{
		nRet = KdvAES( achKey, wKeyLen, MODE_CBC, DIR_ENCRYPT, achIV,
					   pInBuf, wInBufLen, pOutBuf );
	}
	else  // decrypt
	{
		nRet = KdvAES( achKey, wKeyLen, MODE_CBC, DIR_DECRYPT, achIV,
					   pInBuf, wInBufLen, pOutBuf );
	}

	return;
}

/*=============================================================================
�� �� ���� GenLicense
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CString &csLicenseKey
           CString &csLicenseContent
�� �� ֵ�� u32 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/10/19  4.0			������                  ����
=============================================================================*/
u32 GenLicense(CString &csLicenseKey, CString &csLicenseContent)
{   
    TLicenseParam tLicenseParam;

    //����licensecontent
    if (csLicenseKey.IsEmpty() || csLicenseContent.IsEmpty())
    {
        return ERR_KDVLICENSE_AUTH_FAILED;
    }

	s8  achSeps[] = "$"; //$ as seperators
	s8 *pchToken  = NULL;
    s8  achbuf[2048] = {0};
	strcpy(achbuf, csLicenseContent);    
		
	// 1. �豸id
	pchToken = strtok(achbuf, achSeps);
    if (NULL == pchToken)
    {
        return ERR_KDVLICENSE_GETPARA_FAILED;
    }
    else
    {
        strcpy(tLicenseParam.szDeviceID, pchToken);
    }
	
    // 2. �������
    pchToken = strtok(NULL, achSeps);
    if (NULL == pchToken)
    {
        return ERR_KDVLICENSE_GETPARA_FAILED;
    }
    else
    {
        s32 nNum = atoi(pchToken);
        if (nNum <= 0 || nNum > MAXNUM_MCU_MT)
        {
            return ERR_KDVLICENSE_INVALID_NUM;
        }        
        tLicenseParam.wAuthNum = (u16)nNum;
    }

    // 3. ��ֹ����
    pchToken = strtok(NULL, achSeps);
    if (NULL == pchToken)
    {
        return ERR_KDVLICENSE_GETPARA_FAILED;
    }
    else
    {
        s8 achTmp[32];
        strcpy(achTmp, pchToken);
        s8 achSeps[] = "-";
        pchToken = strtok(achTmp, achSeps);
        if (NULL == pchToken)
        {
            return ERR_KDVLICENSE_INVALID_DATE;
        }
        u16 wYear = atoi(pchToken);
        if (wYear >= 2038)
        {
            return ERR_KDVLICENSE_INVALID_DATE;
        }
        pchToken = strtok(NULL, achSeps);
        if (NULL == pchToken)
        {
            return ERR_KDVLICENSE_INVALID_DATE;
        }
        u8 byMonth = atoi(pchToken);
        if (byMonth > 12)
        {
            return ERR_KDVLICENSE_INVALID_DATE;
        }
        pchToken = strtok(NULL, achSeps);
        if (NULL == pchToken)
        {
            return ERR_KDVLICENSE_INVALID_DATE;
        }
        u8 byDay = atoi(pchToken);
        if (byDay > 31)
        {
            return ERR_KDVLICENSE_INVALID_DATE;
        }
        
        tLicenseParam.tExpireDate.SetYear(wYear);
        tLicenseParam.tExpireDate.SetMonth(byMonth);
        tLicenseParam.tExpireDate.SetDay(byDay);      
    }

    //���֤����   
    strcpy(tLicenseParam.szAuthName, csLicenseKey.SpanExcluding("|"));

    //generator key file
    u8 achEncText[LEN_KEYFILE_CONT] = {0};

	// ����ԭʼ��������'|'�ָ�   
    s8 achSequenceStr[1024] = {0};
	sprintf(achSequenceStr, "%s|%s|%d|%d|%d|%d", tLicenseParam.szAuthName, 
        tLicenseParam.szDeviceID, tLicenseParam.wAuthNum, 
        tLicenseParam.tExpireDate.GetYear(), tLicenseParam.tExpireDate.GetMonth(), 
        tLicenseParam.tExpireDate.GetDay());    
	u16 wSequenceStrLen = strlen(achSequenceStr);	
	while( 0 != wSequenceStrLen % 16 )  // ����16�ı���
	{
		strcat(achSequenceStr, "|");
		wSequenceStrLen += 1;
	}

    CreateDirectory(DEFAULT_KEY_PATHNAME, NULL);
    
    s8 achKeyFileName[KDV_MAX_PATH] = {0};
    sprintf(achKeyFileName, "%s%s", DEFAULT_KEY_PATHNAME, KEY_FILENAME);
	FILE *pHandler = fopen( achKeyFileName, "wb" );
	if( NULL == pHandler )
	{
		return ERR_KDVLICENSE_WRITEKEY_FAILED;
	}
	
	GenEncodeData( achEncText, wSequenceStrLen, (u8*)achSequenceStr, TRUE);

	s8 szCopyright[128];
	memset( szCopyright, 0 ,sizeof(szCopyright) );
	memcpy( szCopyright, KEDA_COPYRIGHT, strlen(KEDA_COPYRIGHT) );
	
	fwrite( szCopyright, strlen(KEDA_COPYRIGHT), 1, pHandler );
	
	u16 wEncTextLen = htons(LEN_KEYFILE_CONT);
	fwrite( &wEncTextLen, sizeof(wEncTextLen), 1, pHandler );	

	fwrite( achEncText, sizeof(achEncText), 1, pHandler );
	
	fwrite( szCopyright, sizeof(KEDA_COPYRIGHT), 1, pHandler );
	
	memset( szCopyright, 0, sizeof(szCopyright) );
	strcpy( szCopyright, ". Copyright 2003 - 2008." );

	fwrite( szCopyright, strlen( szCopyright ), 1, pHandler );

	fclose( pHandler );
	pHandler = NULL;		

    return SUCCESS_KDVLICENSE;
}

/*=============================================================================
�� �� ���� WriteKey
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� BSTR bsAuthCode              [in]  ��Ȩ�� 32λ
           BSTR bsLicenseKey            [in]  ���֤���� 36λ
           BSTR bsLicenseContent        [in]  license��Ȩ��Ϣ����������Ϊ���豸ID + "$" + ������ + "$" + ��ֹ����
           BSTR *pbsRetId               [out] ������           
�� �� ֵ�� STDMETHODIMP 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2006/10/19  4.0			������                  ����
=============================================================================*/
STDMETHODIMP COprKey::WriteKey(BSTR bsAuthCode, BSTR bsLicenseKey, BSTR bsLicenseContent, BSTR *pbsRetId)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

    CString csRetId;
	CString csAuthCode = bsAuthCode;
	CString csLicenseKey = bsLicenseKey;
	CString csLicenseContent = bsLicenseContent;
	u32 dwRet = SUCCESS_KDVLICENSE;

    //��Ȩ����֤
    if (0 != csAuthCode.Compare(DEFAULT_KDVLICENSE_AUTHCODE))
	{
		dwRet = ERR_KDVLICENSE_AUTH_FAILED;
	}
    else
    {        
        dwRet = GenLicense(csLicenseKey, csLicenseContent);        
    }    
	
	//����ֵ
	csRetId.Format("%d", dwRet);
	*pbsRetId = csRetId.AllocSysString();
	csRetId.ReleaseBuffer();

	if (SUCCESS_KDVLICENSE != dwRet)
	{
		return S_FALSE;
	}
	return S_OK;
}
