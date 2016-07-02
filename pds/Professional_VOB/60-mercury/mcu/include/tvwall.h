/*****************************************************************************
   ģ����      : tvwall
   �ļ���      : tvwall.h
   ����ļ�    : tvwall.cpp
   �ļ�ʵ�ֹ���: �ṩ��tvwall(����hdu��tvwall�豸)�Ļ������ܽӿ�
   ����        : ����
   �汾        : V0.9  Copyright(C) 2001-2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2011/06/01  4.6         ����	       ����
******************************************************************************/

#ifndef MCU_TVWALL_H
#define MCU_TVWALL_H

#include "mcustruct.h"
#include "eqpbase.h"

#define		MAX_NUM_TVW_CHNNL				(u8)16				// Tvw�豸ͨ���������16��
#define		MAX_NUM_TVW_CFG_NUM				MAXNUM_HDUBRD * MAXNUM_HDU_CHANNEL

// TV Wall�豸���Ͷ���
enum ETvwEqpType
{
	E_TVW_TYPE_TVWALL = 0,
	E_TVW_TYPE_HDU,
	E_TVW_TYPE_HDU_L,
	E_TVW_TYPE_HDU_H,
	E_TVW_TYPE_HDU_SCHEME,

	E_TVW_TYPE_SUM
};

typedef TTWMember  TTvwMember;

// �ж��Ƿ��ǺϷ���Tvwͨ����
#define IsValidTvwChnnl( a ) ((a) < MAXNUM_PERIEQP_CHNNL ? TRUE : FALSE)

/** 
*  @��    ��@ :  �ж��Ƿ��ǺϷ���Hdu�豸 
*/
extern BOOL32 IsValidHduEqp(const TEqp& tEqp);

/** 
*  @��    ��@ :  ����Eqp Id��ͨ�����ж��Ƿ��ǺϷ���Hdu
*/
extern BOOL32 IsValidHduChn(const u8 byHduId, const u8 byChnnlIdx, const u8 bySubChnId = 0);

/** 
*  @��    ��@ :  �ж��Ƿ��ǺϷ���Tvwall�豸, ����Hdu
*/
extern BOOL32 IsValidTvwEqp(const TEqp& tEqp);

/** 
*  @��    ��@ :  ����Eqp Id��ͨ�����ж��Ƿ��ǺϷ���Tv wall�豸, ����Hdu
*/
extern BOOL32 IsValidTvw(const u8 byTvwId, const u8 byChnnlIdx );

/** 
*  @��    ��@ :  ���Hduͨ��״̬
*/
extern void ClearHduChnnlStatus( const u8 byEqpId, const u8 byChnnlIdx = 0xFF );

/** 
*  @��    ��@ :  ���Tvwͨ��״̬
*/
extern void ClearTvwChnnlStatus( const u8 byEqpId, const u8 byChnnlIdx = 0xFF );

/** 
*  @��    ��@ :  ��ȡ����ǽ����
*/
extern const char* const GetHduTypeStr( const TEqp& tHdu );

/** 
*  @��    ��@ :  �ж�Hduģʽ�Ƿ�֧��
*/
extern BOOL32 IsValidHduMode(const u8 byHduChnMode);

class CTvwEqp;

// ����ǽͨ������
class CTvwChnnl /*: public CSwitchBase*/
{
public:
	CTvwChnnl() { Clear(); }

	void Clear();
	BOOL32 IsFree() { return m_byConfIdx != 0 ; }
	BOOL32 IsNull() { return m_byEqpId == 0; }

	// ���û��ȡ����״̬
	void SetMute( const BOOL32 bMute ) { m_byMuteFlag = bMute ? 1 : 0; }
	BOOL32 IsMute ( ) const { return m_byMuteFlag; }

	// ���û��ȡͨ��״̬
	void SetStatus( const u8 byStatus ) { m_byStatus = byStatus; }
	u8 GetStatus( ) const { return m_byStatus; }

	// ���û��ȡͨ��������С
	void SetVolume( u8 byVolume ) { m_byVolume = byVolume; }
	u8 GetVolume () const { return m_byVolume; }

	// ���û��ȡ��������
	void SetConfIdx ( const u8 byConfIdx ) { m_byConfIdx = byConfIdx; }
	u8 GetConfIdx( ) const { return m_byConfIdx; }

	// ���û��ȡ������������
	void SetEqpId( const u8 byEqpId ) { m_byEqpId = byEqpId; }
	u8 GetEqpId () const { return m_byEqpId; }

	// ���û��ȡ������������
	void SetChnnlIdx( const u8 byChnnlIdx ) { m_byChnnlIdx = byChnnlIdx; }
	u8 GetChnnlIdx () const { return m_byChnnlIdx; }

	// ���û��ȡͨ����Ա
	void SetMember( const TTvwMember& tMember ) { memcpy(&m_tMember, &tMember, sizeof(m_tMember)); }
	TTvwMember GetMember() const { return m_tMember; }

	// ��ȡͨ�����ڵ�TEqp����
	TEqp GetTvwEqp();

	const CTvwChnnl& operator== ( const CTvwChnnl& );

private:
	u8				m_byStatus;								// ͨ��״̬
	u8				m_byMuteFlag : 1;						// �Ƿ񱻾���. 1��������; 0, û��
	u8				m_byVolume;								// ������С
	u8				m_byConfIdx;							// ��������
	u8				m_byEqpId;								// ����Eqp��Id
	u8				m_byChnnlIdx;							// ����Eqp�µ�ͨ����
	TTvwMember		m_tMember;								// ʵ��ͨ����Ա
};

class CConfTvwChnnl : public CTvwChnnl
{
public:
	// ���
	void Clear() { memset(this, 0, sizeof(CConfTvwChnnl)); CTvwChnnl::Clear(); }

	// ���û��ȡͨ���������õĳ�Ա
	void SetConfMember( const u8 byMember ) { m_byConfMember = byMember; }
	u8 GetConfMember() const { return m_byConfMember; }

	// ���û��ȡ������ͨ����Ա����
	void SetConfMemberType( const u8 byMemberType ) { m_byConfMemberType = byMemberType; }
	u8 GetConfMemberType() const { return m_byConfMemberType; }

	const CConfTvwChnnl& operator== ( const CConfTvwChnnl& );

private:
    u8    m_byConfMember;									// ��ͨ����Ա����
    u8    m_byConfMemberType;								// ��ͨ����Ա��������
};

// ����ǽ�豸����
class CTvwEqp : public TEqp
{
public:											// ��������
	// ���û��ȡͨ������
	//void SetChnnlNum ( const u8 byNum ) { m_byChnnlNum = byNum; }
	u8 GetChnnlNum() const ;/*{ return m_byChnnlNum; }*/

	// ���û��ȡĳ��ͨ��
	void SetChnnl( const u8 byChnnlIdx, const CTvwChnnl& cChnnl );
	CTvwChnnl* GetChnnl( const u8 byChnnlIdx );

	// ���û��ȡ����״̬
	void SetOnline( const BOOL32 bOnline = TRUE ) { m_byOnline = bOnline ? 1 : 0; }
	BOOL32 IsOnline( ) const { return m_byOnline != 0 ? TRUE : FALSE; }

	// ���û��ȡ�Ϸ�״̬
	void SetValid( const BOOL32 bValid = TRUE ) { m_byValid = bValid ? 1 : 0; }
	BOOL32 IsValid( ) const { return m_byValid != 0 ? TRUE : FALSE; }

	// ���û��ȡ����ǽ�豸����
	void SetTvwType( const ETvwEqpType eType ) { m_eTvwEqpType = eType; }
	ETvwEqpType GetTvwType() const { return m_eTvwEqpType; }

	// ���û��ȡý��������
	void SetCapSet( const TSimCapSet& tCapSet){ memcpy(&m_tSimCapSet, &tCapSet, sizeof(TSimCapSet));}
	TSimCapSet GetCapSet() const { return m_tSimCapSet; }

public:										 // ��������
	// ���ĳ��ͨ��
	void ClearChnnl( const u8 byChnnlIdx );
	// �������ͨ��
	void ClearAllChnnl();

	// Ѱ��һ�����е�ͨ��
	CTvwChnnl* FindFreeChnnl();
	// Ѱ��ԴΪtSrc�ĵ���ǽͨ��
	u8 FindChnnl( const TMt& tSrc, CTvwChnnl* pacChnnl, const u8 byChnnlNum );
	// Ѱ��ΪbyConfIdx�����������ͨ��
	u8 FindChnnl( const u8 byConfIdx, CTvwChnnl* pacChnnl, const u8 byChnnlNum );
	// Ѱ��Ϊĳ���¼�mcu�����ͨ��
	u8 FindChnnlByMcu( const TMcu& tSubMcu, CTvwChnnl* pacChnnl, const u8 byChnnlNum );
	
public:
	// ��TEqp��ֵ��CTvwEqp
	const CTvwEqp& operator= ( const TEqp& );

	// �ж��Ƿ���TEqp�������
	BOOL32 operator== ( const TEqp& );

private:
	// ���캯��, ��ֹĬ�Ϲ���
	CTvwEqp();

private:
	/*******************************************************************************/
	/* ĿǰCTvwEqpֻ�Ƕ�tvwall������һЩ��װ, ����Դ��Ȼ����mcudata��mcuvc�е����� */
	/* �Ժ󳹵��ع�ʱ, CTvwEqp�����װtvwall����������							   */
	/*******************************************************************************/
	u8				m_byChnnlNum;							// ͨ��������
	CTvwChnnl		m_acTvwChnnl[MAX_NUM_TVW_CHNNL];		// ͨ������, ���
	u8				m_byOnline : 1;							// �Ƿ�����
	u8				m_byValid : 1;							// �Ƿ�Ϸ�(������֧��)
	ETvwEqpType		m_eTvwEqpType;							// ����ǽ�豸����

	TSimCapSet		m_tSimCapSet;							// ý��������		
};



// VCS�������ǽ������Ϣ����
class CVcsTvWallCfg
{
public:
	// ���ĳ��ͨ��
	void ClearChnnl( const u8 byChnnlIdx );
	// �������ͨ��
	void ClearAllChnnl();
	
	// Ѱ��һ�����е�ͨ��
	CTvwChnnl* FindFreeChnnl();
	// Ѱ��ԴΪtSrc�ĵ���ǽͨ��
	u8 FindChnnl( const TMt& tSrc, CTvwChnnl* pacChnnl, const u8 byChnnlNum = 1 );
	// Ѱ��ΪbyConfIdx�����������ͨ��
	u8 FindChnnl( const u8 byConfIdx, CTvwChnnl* pacChnnl, const u8 byChnnlNum = 1);
	// Ѱ��Ϊĳ���¼�mcu�����ͨ��
	u8 FindChnnlByMcu( const TMcu& tSubMcu, CTvwChnnl* pacChnnl, const u8 byChnnlNum = 1);
	
private:
	u16				m_byChnnlNum;							// ���õĵ���ǽ��Ŀ
	CTvwChnnl*		m_apcTvwChnnl[MAX_NUM_TVW_CFG_NUM];		// ����ǽͨ����Ϣ
	TMt				m_atOrigMemb[MAX_NUM_TVW_CFG_NUM];		// ����ǽԭʼͨ����Ա
};

// ��ǰ��˵��
class CVcsConfTvwMgr;

class CTvwMgrModeBase
{
public:
	// ���캯��
	CTvwMgrModeBase(  );

	// ��ʼ��
	BOOL32 Init( const u8 byMode, CVcsTvWallCfg* pcTvwInfo );
	// �˳�ʱ�����ٺ���
	void Destroy();

	u8 GetMode() const { return m_byTvwMgrMode; }

	// ����
	virtual BOOL32 Enter( CVcsConfTvwMgr *pcMgr );
	// �˳�
	virtual BOOL32 Exit();

	virtual s16 OnMessage( const CServMsg * pcMsg );

protected:
	virtual s16 OnSetMemberMsg( const CServMsg * pcMsg );
	virtual s16 OnClrMemberMsg( const CServMsg * pcMsg );
	virtual s16 OnStartMixMsg( const CServMsg * pcMsg );
	virtual s16 OnStopMixMsg( const CServMsg * pcMsg );

private:
	u8				m_byTvwMgrMode;							// ��ǰ����ǽ����ģʽ
	CVcsConfTvwMgr*	m_pcConfTvwMgr;							// ����ǽ���������
	CVcsTvWallCfg*	m_pcTvwGroup;							// ����ǽ��Ϣ
};

class CTvwReviewMode : public CTvwMgrModeBase
{

};

class CTvwManuMode : public CTvwMgrModeBase
{
	
};

class CTvwAutoMode : public CTvwMgrModeBase
{
	
};

class CMcuVcInst;

class CVcsConfTvwMgr : public CVcsTvWallCfg
{
public:										 // ��������
	BOOL32 Init();
	void Destroy();

	// ȫ���������
	BOOL32 StartMix();
	// ȫ����ֹ����
	BOOL32 StopMix();

	BOOL32 ChangeTvwMgrModel( const u8 byNewMode );

private:
	CMcuVcInst*				m_pcVc;								// ��ض���
	CTvwMgrModeBase*		m_pcVcsTvwModeMgr;					// ����ǽ����ģʽ			
};


/************************************************************************/
/* ȫ�ֺ���                                                             */
/************************************************************************/
// CTvwEqpתTEqp
TEqp	GetTEqpFrmTvw( const CTvwEqp& cTvw );

// TEqpתCTvwEqp
CTvwEqp GetTvwFromTEqp( const TEqp& tTvw );

// ��TEqp��ͨ��������ȡ��Ӧ�ĵ���ǽͨ��
CTvwChnnl GetTvwChnnl( const TEqp& tTvw, const u8 byChnnlIdx );

// ����Eqp Id�ж����豸�Ƿ���TvWall
BOOL32 IsTvWall( u8 byEqpId );

// ����Eqp ID�жϸ��豸�Ƿ���HDU�豸
BOOL32 IsHdu( u8 byEqpId);

#endif