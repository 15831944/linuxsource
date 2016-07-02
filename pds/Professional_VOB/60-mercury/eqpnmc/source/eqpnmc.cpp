/*****************************************************************************
    ģ����      : EqpAgt
    �ļ���      : eqpnmc.cpp
    ����ļ�    : 
    �ļ�ʵ�ֹ���: �������ܿͻ���
    ����        : liaokang
    �汾        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    �޸ļ�¼:
    ��  ��      �汾        �޸���          �޸�����
    2012/06/18  4.7         liaokang        ����
******************************************************************************/
#include "eqpnmc.h"
#include "eqpagt.h"

#ifdef WIN32
#include "Shlwapi.h"
#pragma comment (lib,"shlwapi.lib") 
#endif
// ���ؿ��ƴ���
// ��release�� project->setting->link->project option (������ı���)��
// ���/subsystem:"windows" /entry:"mainCRTStartup"���൱���ڴ��������
/* #pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"") */

// ȫ���ź���
static SEMHANDLE g_tSemHandle;

#ifdef WIN32

// �ж��Ƿ�Ϊ�����Զ�����
API BOOL32 IsAutoRun(void)
{
    HKEY   hRegKey;   //ע��key
    s8 achKeyName[256] = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";//ע���
    if( ERROR_SUCCESS != RegOpenKey(HKEY_LOCAL_MACHINE, achKeyName, &hRegKey) )//��ע���     
    {
        StaticLog( "[IsAutoRun] RegOpenKey wrong. Can not do the judgement.\n");
        return FALSE;   
    }
    if( ERROR_SUCCESS != RegQueryValueEx( hRegKey, "eqpnmc", NULL, NULL, NULL, NULL) )
    {
        StaticLog( "[IsAutoRun] The exe is not autorun.\n");
        RegCloseKey(hRegKey);   //�ر�ע���key 
        return FALSE;
    }   
    //�ɹ���ִ���ͷ��ڴ�
    RegCloseKey(hRegKey);
    StaticLog( "[IsAutoRun] The exe is autorun.\n");
    return TRUE;
}


//ʵ�ֿ���������
API BOOL32 AutoRun(void)
{
    BOOL bRe = TRUE; 
    s8   achCurPath[EQPAGT_MAX_LENGTH] = {'\0'}; //����ǰ·��
    
    HKEY   hRegKey;   //ע��key
    s8 achKeyName[256] = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";//ע���
    GetModuleFileName( NULL,achCurPath, EQPAGT_MAX_LENGTH ); //������ó���·��
    StaticLog( "[AutoRun] The program current path is:%s.\n", achCurPath);
    sprintf( achCurPath + strlen(achCurPath), "%s", AUTORUN_FLAG );
    if( ERROR_SUCCESS != RegOpenKey(HKEY_LOCAL_MACHINE, achKeyName, &hRegKey) )//��ע���     
    { 
        StaticLog( "[AutoRun] RegOpenKey wrong, start autorun failed.\n");
        bRe = FALSE;
        return bRe;   
    }   
    if( ERROR_SUCCESS != RegSetValueEx(hRegKey, "eqpnmc", 0,REG_SZ,   
        (const unsigned char *)achCurPath, EQPAGT_MAX_LENGTH))   //�򿪳ɹ�д��Ϣ��ע���
    { 
        StaticLog( "[AutoRun] Start autorun failed.\n");
        //д��ʧ��
        RegCloseKey(hRegKey);   //�ر�ע���key 
        bRe = FALSE;
        return bRe;
    }   
    //�ɹ���ִ���ͷ��ڴ�
    RegCloseKey(hRegKey);  
    StaticLog( "[AutoRun] The program will be autorun.\n");
    return bRe;
}

//ȡ����������
API void CancelAutoRun(void)
{
    s8 achKeyName[256] = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";//ע���
    if ( ERROR_SUCCESS != SHDeleteValue( HKEY_LOCAL_MACHINE, achKeyName, "eqpnmc" ) )
    {
        StaticLog( "[CancleAutoRun] SHDeleteValue wrong, cancel autorun failed.\n");
        return;
    } 
    StaticLog(  "[CancleAutoRun] Cancel autorun succecced.\n");
}

// ������һ��ʵ��
BOOL32 OnlyStartOne()
{  
    HANDLE hSem =  CreateSemaphore(NULL, 1, 1, "eqpnmc.exe");   
    //   �ź����Ѵ��ڣ�   
    //   �ź������ڣ����������һ��ʵ������   
    if (GetLastError()   ==   ERROR_ALREADY_EXISTS)   
    {
        //   �ر��ź������   
        CloseHandle(hSem);   
        //   Ѱ����ǰʵ����������   
        HWND   hWndPrevious = GetWindow( GetDesktopWindow(),GW_CHILD );   
        while( IsWindow(hWndPrevious) )   
        {   
            //   ��鴰���Ƿ���Ԥ��ı��?   
            //   �У���������Ѱ�ҵ�����   
            if( GetProp(hWndPrevious, "eqpnmc.exe") )   
            {                    
                return   FALSE;   
            }   
            //   ����Ѱ����һ������   
            hWndPrevious = GetWindow( hWndPrevious,GW_HWNDNEXT ); 
        }   
        return FALSE;   
    }   
    return TRUE;
}
#endif

// �˳�
API void quit(void) 
{
    // �˳�ǰ��flush
    OspDelay(20);
    logflush();
    OspDelay(20);
    
    // eqpagt�˳�
    g_cEqpAgt.EqpAgtQuit();
    // �˳�OSP
    OspQuit();
    // �ͷ�ȫ���ź���
    OspSemGive( g_tSemHandle );
}

// ������
int main()
{
#ifdef WIN32
    if (!OnlyStartOne())
    {
        return 0;
	}
#endif

    // KdvLogģ���ʼ��
    Err_t err = LogInit( EQPNMC_KDVLOG_FILE );
    if (LOG_ERR_NO_ERR != err)
    {
        printf("[main]LogInit() failed! ERRCODE = [%d]\n",err);
	}    
    
#if _DEBUG
    // ��ʼ��Osp
    OspInit(TRUE, EQP_TELNET_PORT);
#else
    // ��ʼ��Osp release�治��ʾtelnet����
    OspInit(FALSE, EQP_TELNET_PORT);
    //Osp telnet ��ʼ��Ȩ
    OspTelAuthor( EQP_TEL_USRNAME, EQP_TEL_PWD );
#endif

    // ��ʼ����Ԫ����
    g_cEqpAgt.EqpAgtInit();

	// �����ź���
    if ( !OspSemBCreate(&g_tSemHandle) )
    {
        printf( "[main] OspSemBCreate failed\n" );
        return 0;
	}
	// Take���ζ�Ԫ�ź���������    
    if ( !OspSemTake( g_tSemHandle ) )
    {
        printf( "[main] OspSemTake 1 failed\n" );
        return 0;
    }
    if ( !OspSemTake( g_tSemHandle ) )
    {
        printf( "[main] OspSemTake 2 failed\n" );
        return 0;
    }
    return 0;
}