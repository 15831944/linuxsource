/*****************************************************************************
    ģ����      : EqpNmc
    �ļ���      : eqpagt.h
    ����ļ�    : 
    �ļ�ʵ�ֹ���: EqpAgt�ӿڼ�ʵ�� (for win32)
    ����        : liaokang
    �汾        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    �޸ļ�¼:
    ��  ��      �汾        �޸���          �޸�����
    2012/06/18  4.7         liaokang        ����
******************************************************************************/
#ifndef EQPAGT_H_
#define EQPAGT_H_

#include "eqpagtcommon.h"
#include "eqpagtsysinfo.h"
#include "eqpagtcommbase.h"
#include "kdvdef.h"
#include "eqpagtipinfo.h"

// �汾��
#define VER_EQPNMC              ( LPCSTR )"eqpnmc4.7.1.1.1.120727"

//  ---------------   eqpagtcfg�����ļ���� begin  -------------------
// �豸������
#define SECTION_EqpSubType      ( LPCSTR )"EqpSubType"
#define KEY_EqpSubType          ( LPCSTR )"EqpSubType"

// �̷���
#define SECTION_DiskPartionName ( LPCSTR )"DiskPartionName"
#define KEY_DiskPartionName     ( LPCSTR )"DiskPartionName"
#if defined( WIN32 ) || defined(_WIN32)
#define DEF_DiskPartionName		( LPCSTR )"c:"
#else
#define DEF_DiskPartionName	    ( LPCSTR )"/"
#endif
//  ---------------   eqpagtcfg�����ļ���� end    -------------------

// �߳�ID�����ȼ�
#define AID_EQPAGT_SCAN         AID_AGT_BGN
#define APPPRI_EQPAGT_SCAN      90

#define EXEFILE                 ( LPCSTR )"c:\\WINDOWS\\system32\\netsh.exe"

// ��ʱɨ���豸״̬��ʱ��ʱ������λ: s��
#define SCAN_STATE_TIME_OUT		5

class CEqpAgt
{
public:
    CEqpAgt();
    ~CEqpAgt();
    
    // ��ʼ��
    void   EqpAgtInit(void);
    // �˳�
    void   EqpAgtQuit(void);

    // ��ȡϵͳ״̬
    u32    GetSysState( void );
    // ��ȡ����汾��
    void   GetSoftwareVer( LPSTR lpszSoftwareVer );
    // ��ȡӲ���汾��
    void   GetHardwareVer( LPSTR lpszHardwareVer );
    // ��ȡ����ʱ��
    void   GetCompileTime( LPSTR lpszCompileTime );
    // ��ȡ�豸����ϵͳ
    u32    GetOsType( void );
    // ��ȡ�豸������
    BOOL32 GetSubType( u32* pdwSubType );
    // ��ȡ�̷���
    BOOL32 GetDiskPartionName( LPSTR lpszDiskPartionName );

    // ��ȡIP��Ϣ
    BOOL32 GetIpInfoAll( TNetAdaptInfoAll* ptNetAdapterInfoAll );
    // ����IP��Ϣ
    BOOL32 SetIpInfoAll( TNetAdaptInfoAll& tNetAdapterInfoAll );

private:
        
    // ���ipConfig.ini�ļ��Ƿ���ڣ�������������½�һ��
    BOOL32 IpCfgInit(void);                                                 // ���ó�ʼ��
    BOOL32 ReadIpCfg( LPCSTR lpszCfgFileFullPath );                         // ������
    void   CreateDir( LPCSTR pPathName );			                        // ����Ŀ¼
    BOOL32 PretreatIpCfgFile( LPCSTR lpszCfgPath, LPCSTR lpszCfgName );     // ����Ĭ�������ļ�
   
    // ip��Ϣ��ά��
    BOOL32 ReadIpInfoTable( TNetAdaptInfoAll* ptNetAdapterInfoAll );
    BOOL32 WriteIpInfoTable( TNetAdaptInfoAll tNetAdapterInfoAll );

private:
    s8     m_achIpCfgFile[EQPAGT_MAX_LENGTH];    // ipConfig.ini��·��
    TNetAdaptInfoAll m_tNetAdaptInfoAll;         // IP��Ϣ
};
extern CEqpAgt g_cEqpAgt;

#endif  // EQPAGT_H_