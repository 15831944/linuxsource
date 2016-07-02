/*****************************************************************************
    ģ����      : EqpAgt
    �ļ���      : eqpagtsysinfo.h
    ����ļ�    : eqpagtsysinfo.h 
    �ļ�ʵ�ֹ���: EqpAgtϵͳ��Ϣ:
                        ϵͳ״̬
                        ����汾�š�Ӳ���汾�š�����ʱ�䡢�豸����ϵͳ���豸������
                        cpuռ���ʡ��ڴ�ռ���ʡ����̿ռ�ռ����
                        IP��MASK��GATEWAY
    ����        : liaokang
    �汾        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    �޸ļ�¼:
    ��  ��      �汾        �޸���          �޸�����
    2012/06/18  4.7         liaokang        ����
******************************************************************************/
#ifndef _EQPAGT_SYSINFO_H_
#define _EQPAGT_SYSINFO_H_
#include "eqpagtutility.h"
#include "eqpagtipinfo.h"

// �汾��
#define  VER_EQPAGTSYSINFO       ( LPCSTR )"eqpagtsysinfo4.7.1.1.1.120727"

#define  CPU_THRESHOLD    85 // CPUռ���ʷ�ֵ����������Ϊ�쳣
#define  MEM_THRESHOLD    85 // �ڴ�ռ���ʷ�ֵ����������Ϊ�쳣
#define  DISK_THRESHOLD   80 // ���̿ռ�ռ���ʷ�ֵ����������Ϊ�쳣

// CPU��Ϣ
typedef struct
{
    u8 m_byCpuAllocRate; // CPU��ʹ���ʰٷֱȣ�0��100
}TCpuInfo;

// CPU������Ϣ
typedef struct
{
    u8 m_byCpuStatus;
    TCpuInfo tCpuInfo;
}TCpuAllInfo;

// �ڴ���Ϣ
typedef struct
{  
    u32 m_dwMemAllocSize;   // ��ʹ���ڴ������С(KBytes)
    u32 m_dwMemPhysicsSize; // �ڴ��С(KByte)
}TMemInfo;

// �ڴ�������Ϣ
typedef struct
{
    u8 m_byMemStatus;
    TMemInfo tMemInfo;
}TMemAllInfo;

// ���̷�����Ϣ
typedef struct
{
    u32 m_dwDiskAllocSize;   // ��ʹ�ô��̷�����С(MBytes)
    u32 m_dwDiskPhysicsSize; // ���̷�����С(MByte)
}TDiskInfo;

// ���̷���������Ϣ
typedef struct
{
    u8 m_byDiskStatus;
    TDiskInfo tDiskInfo;
    s8 m_achPartionName[EQPAGT_MAX_LENGTH];    // ��ǰ���÷�����
}TDiskAllInfo;

/*-------------------------------------------------------------
                     Nms����ϵͳ״̬����
-------------------------------------------------------------*/
typedef void (*TNmsSetSysState)( u32 dwSysState );

/*-------------------------------------------------------------
                    Nms���ñ���IP��Ϣ����
-------------------------------------------------------------*/
typedef void (*TNmsSetIpInfo)( TNetAdaptInfoAll& tNetAdapterInfoAll );

/*******************************************************************
    EqpAgt ----> Nms   2�ֽں�4�ֽڵ���ֵ������Ҫת��������
    Nms ----> EqpAgt   2�ֽں�4�ֽڵ���ֵ������Ҫת��������
*******************************************************************/
class CEqpAgtSysInfo
{
public:
    CEqpAgtSysInfo();
    ~CEqpAgtSysInfo();
    
    // ��ʼ��
    BOOL32 Initialize( TNmsSetSysState pfNmsSetSysState, TNmsSetIpInfo pfNmsSetIpInfo );  // ��ʼ�����洢����ָ��
    void   SupportNmsGetSysInfo( void );                    // ֧�����ܷ�������ȡϵͳ��Ϣ
    void   SupportNmsSetSysInfo( void );                    // ֧�����ܷ���������ϵͳ��Ϣ 
    void   SupportNmsTrapSysInfo( u32 dwScanTimeSpan );     // ֧��ɨ���쳣��ı��ϵͳ��Ϣ���ϱ������ܷ�����

    /*------------------------------------------------------------------------------------------
                                          ϵͳ״̬  begin
    ------------------------------------------------------------------------------------------*/
    // �û����ýӿ�
    void   SetSysState( u32 dwSysState );                     // �洢ϵͳ״̬
    u32    GetSysState( void );                               // ��ȡϵͳ״̬

    // NMS����Get��
    void   SysStateOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen); // ϵͳ״̬ EqpAgt ----> Nms

    // NMSд��Set��
    BOOL32 SysStateOfNms2EqpAgt(void * pBuf);                 // ϵͳ״̬ Nms ----> EqpAgt
    /*------------------------------------------------------------------------------------------
                                          ϵͳ״̬  end
    ------------------------------------------------------------------------------------------*/



    /*------------------------------------------------------------------------------------------
                  ����汾�š�Ӳ���汾�š�����ʱ�䡢�豸����ϵͳ���豸������  begin
    ------------------------------------------------------------------------------------------*/
    // �û����ýӿ�
    BOOL32 SetSoftwareVer( LPCSTR lpszSoftwareVer );                // �洢����汾��
    BOOL32 SetHardwareVer( LPCSTR lpszHardwareVer );                // �洢Ӳ���汾��
    BOOL32 SetCompileTime( LPCSTR lpszCompileTime );                // �洢����ʱ��
    void   SetEqpOsType( u32 dwEqpOsType );                         // �洢�豸����ϵͳ
    void   SetEqpSubType( u32 dwEqpSubType );                       // �洢�豸������
    BOOL32 GetSoftwareVer( LPSTR lpszSoftwareVer );                 // ��ȡ����汾��
    BOOL32 GetHardwareVer( LPSTR lpszHardwareVer );                 // ��ȡӲ���汾��
    BOOL32 GetCompileTime( LPSTR lpszCompileTime );                 // ��ȡ����ʱ��
    u32    GetEqpOsType( void );                                    // ��ȡ�豸����ϵͳ
    u32    GetEqpSubType( void );                                   // ��ȡ�豸������

    // NMS����Get������汾�š�Ӳ���汾�š�����ʱ�䡢�豸����ϵͳ���豸������
    void   SoftwareVerOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen);    // ����汾��  EqpAgt ----> Nms
    void   HardwareVerOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen);    // Ӳ���汾��  EqpAgt ----> Nms
    void   CompileTimeOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen);    // ����ʱ��    EqpAgt ----> Nms
    void   EqpOsTypeOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen);      // �豸����ϵͳEqpAgt ----> Nms
    void   EqpSubTypOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen);      // �豸������  EqpAgt ----> Nms
    /*------------------------------------------------------------------------------------------
                  ����汾�š�Ӳ���汾�š�����ʱ�䡢�豸����ϵͳ���豸������  begin
    ------------------------------------------------------------------------------------------*/



    /*------------------------------------------------------------------------------------------
                      cpuռ���ʡ��ڴ�ռ���ʡ����̿ռ�ռ����  begin
    ------------------------------------------------------------------------------------------*/
    // �û����ýӿ�
    BOOL32 SetDiskPartionName(LPCSTR lpszDiskPartionName);      // �洢���ô��̷�����    
    BOOL32 GetCpuAllInfo( TCpuAllInfo* ptCpuAllInfo );          // ��ȡ��ǰcpuռ���ʵ�������Ϣ����һ������debug ����
    BOOL32 GetMemAllInfo( TMemAllInfo* ptMemAllInfo );          // ��ȡ��ǰ�ڴ�ռ���ʵ�������Ϣ����һ������debug ����
    BOOL32 GetDiskAllInfo( TDiskAllInfo* ptDiskAllInfo );       // ��ȡ��ǰ���̿ռ�ռ���ʵ�������Ϣ����һ������debug ����

    // NMS����Get��cpuռ���ʡ��ڴ�ռ���ʡ����̿ռ�ռ����
    BOOL32 CpuRateOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen);    // Cpuʹ����     EqpAgt ----> Nms
    BOOL32 MemRateOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen);    // �ڴ�ʹ����    EqpAgt ----> Nms
    BOOL32 DiskRateOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen);   // ���̿ռ�ʹ����EqpAgt ----> Nms

    // ɨ��cpuռ���ʡ��ڴ�ռ���ʡ����̿ռ�ռ����
    u8     ScanCpuInfo( LPSTR lpszCpuInfo2Trap, u16& wCpuInfoLen );    // ɨ��Cpuʹ���ʣ���������ؽڵ�Trap��Ϣ��EqpAgt ----> Nms
    u8     ScanMemInfo( LPSTR lpszMemInfo2Trap, u16& wMemInfoLen );    // ɨ���ڴ�ʹ���ʣ���������ؽڵ�Trap��Ϣ��EqpAgt ----> Nms
    u8     ScanDiskInfo( LPSTR lpszDiskInfo2Trap, u16& wDiskInfoLen ); // ɨ����̿ռ�ʹ���ʣ���������ؽڵ�Trap��Ϣ��EqpAgt ----> Nms
    /*------------------------------------------------------------------------------------------
                      cpuռ���ʡ��ڴ�ռ���ʡ����̿ռ�ռ����  end
    ------------------------------------------------------------------------------------------*/


    /************************************************************************/
    /*                ��ʱ�����������޸�  begin                           */
    /************************************************************************/
    /*------------------------------------------------------------------------------------------
                                    IP��MASK��GATEWAY  begin
    ------------------------------------------------------------------------------------------*/
    // �û����ýӿ�
    BOOL32 SetIpInfo( TNetAdaptInfoAll& tNetAdapterInfoAll );         // �洢IP��Ϣ
    BOOL32 GetIpInfo( TNetAdaptInfoAll* ptNetAdapterInfoAll );        // ��ȡIP��Ϣ

    // NMS����Get��
    void   LocalIpAddrOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen); // ����IP       EqpAgt ----> Nms
    void   LocalIpMaskOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen); // ����IP Mask  EqpAgt ----> Nms
    void   LocalGateWayOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen);// ����GateWay  EqpAgt ----> Nms

    // NMSд��Set��
    BOOL32 IpInfoOfNms2EqpAgt(u32 dwNodeValue, void * pBuf); // ����IP��Ϣ   Nms ----> EqpAgt
    /*------------------------------------------------------------------------------------------
                                    IP��MASK��GATEWAY  end
    ------------------------------------------------------------------------------------------*/
    /************************************************************************/
    /*                ��ʱ�����������޸�  end                             */
    /************************************************************************/

protected:    
    // ���
    void   Free(void);

    // ��ȡcpu���ڴ桢���̿ռ�ʵ��״̬
    BOOL32 GetCpuInfo( TCpuInfo* ptCpuInfo );
    BOOL32 GetMemInfo( TMemInfo* ptMemInfo );
    BOOL32 GetDiskInfo( LPSTR lpszPartionName, TDiskInfo* ptDiskInfo );

private:
    // ϵͳ״̬
    u32 m_dwSysState;                          // ϵͳ״̬
    TNmsSetSysState m_pfNmsSetSysState;        // NMS����ϵͳ״̬�ص�����ָ��

    // ����汾�š�Ӳ���汾�š�����ʱ�䡢�豸����ϵͳ���豸������
    s8  m_achSoftwareVer[EQPAGT_MAX_LENGTH];   // ����汾��
    s8  m_achHardwareVer[EQPAGT_MAX_LENGTH];   // Ӳ���汾��
    s8  m_achCompileTime[EQPAGT_MAX_LENGTH];   // ����ʱ��
    u32 m_dwEqpOsType;                         // �豸����ϵͳ��NMS��u32 ������ ������
    u32 m_dwEqpSubType;                        // �豸�����ͣ�NMS��u32 ������ ������

    // cpuռ���ʡ��ڴ�ռ���ʡ����̿ռ�ռ����
    s8  m_achPartionName[EQPAGT_MAX_LENGTH];   // ��ǰ���÷�����
    u8  m_byCpuStatus;                         // ��ǰcpu״̬
    u8  m_byMemStatus;                         // ��ǰ�ڴ�״̬
    u8  m_byDiskStatus;                        // ��ǰ���̿ռ�״̬

    // IP��MASK��GATEWAY
    TNetAdaptInfoAll m_tNetAdapterInfoAll;     // IP��Ϣ
    TNmsSetIpInfo m_pfNmsSetIpInfo;            // Nms���ñ���IP��Ϣ�ص�����ָ��
};
extern CEqpAgtSysInfo	g_cEqpAgtSysInfo;

// ���ע�ắ��
u16 EqpAgtGetSysInfo(u32 dwNodeName, void * pBuf, u16 * pwBufLen);  // ����ϵͳ��Ϣ���ص�����ʵ�֣�
u16 EqpAgtSetSysInfo(u32 dwNodeName, void * pBuf, u16 * pwBufLen);  // ��ȡϵͳ��Ϣ���ص�����ʵ�֣�
u16 EqpAgtTrapCpuRate( CNodes& cNodes );                            // ��֯cpu trapֵ
u16 EqpAgtTrapMemRate( CNodes& cNodes );                            // ��֯�ڴ�trapֵ
u16 EqpAgtTrapDiskRate( CNodes& cNodes );                           // ��֯���̿ռ�trapֵ

// ������ʾ��ǰ״̬
API void showsysstate(void);
API void showsoftwarever(void);
API void showhardwarever(void);
API void showcompiletime(void);
API void showeqpostype(void);
API void showeqpsubtype(void);
API void showcpuinfo(void);
API void showmeminfo(void);
API void showdiskinfo(void);
// debug ����������ڷ���trap
API void trapcpu( u8 byCpuAllocRate, u8 byCpuStatus );
API void trapmem( u32 dwMemAllocSize, u32 dwMemPhysicsSize, u8 byMemStatus );
API void trapdisk( u32 dwDiskAllocSize, u32 dwDiskPhysicsSize, u8 byDiskStatus );
// debug ����������ڴ�/�ر���Ӧ����trap
API void cputrapoper( u32 dwScanTimeSpan, BOOL32 bEnable );
API void memtrapoper( u32 dwScanTimeSpan, BOOL32 bEnable );
API void disktrapoper( u32 dwScanTimeSpan, BOOL32 bEnable );
// debug �����������ģ��NMS����
API void setsysstate( u32 dwSysState );

#endif  // _EQPAGT_SYSINFO_H_