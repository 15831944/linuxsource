#ifndef _VCSSSN_H_
#define _VCSSSN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mcsssn.h"


class CVcsSsnInst : public CMcsSsnInst  
{
public:
	CVcsSsnInst( void );
	virtual ~CVcsSsnInst( void );

public:
	void InstanceEntry( CMessage * const pcMsg );

	// �û�����ģ�鴦����
	void ProcVCSUserManagerMsg( const CMessage * pcMsg );
	// ����VCS�Ļ���ģ����Ϣ
	void ProcVCSConfTemMsg( CMessage * const pcMsg );
	// ����VCS�Ĵ�������
	void ProcVCSMcuCreateConfMsg( const CMessage * pcMsg );
	// ����VCS�ĵ�������
	void ProcVCSMcuVCMsg( const CMessage * pcMsg );
	// ����VCS��ʼ����ϯ��������
	void ProcVCSConfOprMsg( const CMessage * pcMsg );
	void ProcVCSSoftNameMsg( const CMessage * pcMsg );
	// ����VCS����MCU���账����Ϣ�����ü���ȡ
	void ProcVCSMcuUnProcCfgMsg( const CMessage * pcMsg );
	
	// ����VCS����Ӧ����Ϣͳһת������
	void ProcMcuVcsMsg( const CMessage * pcMsg );

	// ʵ�ֽ�����ģ���ȡ�����û���Ϣ���Ӧ��������Ϣ���
	void PackUserTaskInfo(u8 byUserNumInPack, u8 *pbyExUserBuf, u8 *pbyVCSUserBuf);

	// ����Ա�˳�����ϯ����
	void QuitCurVCConf();


};

class CVcsSsn 
{
public:
	CVcsSsn(void);
	virtual ~CVcsSsn(void);
	
public:
	// ���ָ���Ļ����Ƿ��Դ��ڱ�����״̬
    BOOL ChkVCConfStatus( CConfId cConfId );
	// ����ָ��ʵ�������Ļ���ID��
	void SetCurConfID( u16 byInsId, CConfId& cConfID );
	// ��ȡָ��ʵ�������Ļ���ID��
	CConfId GetCurConfID(u16 byInsId);
	// ������Ϣ������ͬ���VCS,VCS MCS��ſ��ܳ�����ͬ,���Է������������ִ���
    void Broadcast2SpecGrpVcsSsn( u8 byGrdIp, u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );

	// ��ȡ��Ҫͬ����VCSSSn��Ϣ
	BOOL32 GetVCSSSsnData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen);
    BOOL32 SetVCSSSsnData( u8 *pbyBuf, u32 dwBufLen );

protected:
	// ָ��VCSʵ�������Ļ���ID�� �±�=ʵ����-1 
	CConfId m_acCurConfID[MAXNUM_MCU_MC + MAXNUM_MCU_VC];
};

typedef zTemplate< CVcsSsnInst, MAXNUM_MCU_MC + MAXNUM_MCU_VC, CVcsSsn> CVcsSsnApp;

extern CVcsSsnApp g_cVcsSsnApp;
extern CUsrManage g_cVCSUsrManage;

#endif /*_VCSSSN_H_*/



