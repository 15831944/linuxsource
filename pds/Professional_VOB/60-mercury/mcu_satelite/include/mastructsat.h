/*****************************************************************************
   ģ����      : MtAdp
   �ļ���      : mastructsat.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ���ǻ���Э��������ص����ݽṹ�ͺ궨��  
   ����        : �ű���
   �汾        : V4.6  Copyright(C) 2009-2012 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2009/9      4.6         �ű���          ����
******************************************************************************/

#ifndef _SATSTRUCT_H_
#define _SATSTRUCT_H_

#define  MAXLEN_CONF_NAME           (u8)32
#define  MAXLEN_CONF_PASSWD         (u8)32
#define	 MAXNUM_SATMULTICAST        (u8)5       //������·����鲥������
#define  MCU_SENDTOMULTI_PORT       (u16)5000
#define  MT_RCVMCUMEDIA_PORT        (u16)4000	//�ն˽���MCU���ݵ���ʼ�˿�


//���ն˽���������MCU������Ϣ�ṹ
struct CSatConfInfo
{
protected:
	u8	m_byConfId;	//��MCU�У��û���Ļ��������ţ���Ӧÿһ�����������ͬ������
					//��Ϊ���ֻ���Ļ����
	u8  m_bySpeakerId; //��ǰ�����˵��ն�ID�ţ�Ϊ���ʾ��ǰû�з����ˡ�
	u8  m_byChairmanId;//��ǰ��ϯ���ն�ID�ţ�Ϊ���ʾ��ǰû����ϯ��

	TTransportAddr m_tBrdTransportAddr; //�������������鲥��ַ
	TConfMtInfo m_tConfMtInfo;//����ն�״̬
	TTransportAddr m_tMpTransportAddr; //�ն˷��������ĵ�ַ

	u8	m_byState;       //bit0: 0:δ���渴�� 1:���渴��
							//bit1-2: 00:�ǻ��� 01:���� 10:������ͣ
							//bit3-4: 00:��¼�� 01:¼�� 10:¼����ͣ
							//bit5-7: 000:�Ƿ��� 001:���� 010:������ͣ 011:��� 100:����		
	char m_achConfName[MAXLEN_CONF_NAME];//������
	char m_achPasswd[MAXLEN_CONF_PASSWD];//��������	
	u8	 m_byIntervalTime;
	TVMPParam m_tVmpParam;
	u8   m_byLineNum;
	u8	 m_byUpLoadCurrDmt[MAXNUM_MCU_MT / 8]; //��ǰ�ϴ���Ա�б�
public:
	u8   m_byMtInMix[MAXNUM_MCU_MT / 8];
public:
	CSatConfInfo();
	
	//���û���ID
	u8 		GetConfId( void );
	void	SetConfId( u8 byConfId );
	
	//���÷�����ID
	u8		GetSpeakerId( void );
	void	SetSpeakerId( u8 byMtId );
	BOOL	HasSpeaker( void );

	//������ϯID
	u8		GetChairmanId( void );
	void	SetChairmanId( u8 byMtId );
	BOOL	HasChairman( void );

	//�ն˲�������
	void	AddMt( u8 byMtId );
	void	AddJoinedMt( u8 byMtId );
	void	RemoveMt( u8 byMtId );
	void	RemoveJoinedMt( u8 byMtId );
	void	RemoveAllMt( void );
	BOOL	IsMtInConf( u8 byMtId );
	BOOL	IsMtJoinedConf( u8 byMtId );
	void	SetConfMtInfoMcuId( u8 byMcuId );
	u8		GetConfMtInfoMcuId();
	void	SetConfMtInfo( const TConfMtInfo & tConfMtInfo );
	const TConfMtInfo & GetConfMtInfo();
	
	//���鷢���鲥���ݶ˿�(���е������������Ϊ������)
	u32		GetBrdMeidaIpAddr( void );
	u16		GetBrdMediaStartPort( void );
	void	SetBrdMeidaIpAddr ( u32 dwIpAddress );
	void	SetBrdMediaStartPort ( u16 wPort );

	//�������״̬
	//�Ƿ��ڻ��渴��״̬
	BOOL IsVmp( void ) const	{ return( ( m_byState & 0x01 ) == 0x01 ? TRUE : FALSE ); }
	//�����Ƿ��ڻ��渴��״̬
	void SetVmp( BOOL bVmp )    { if( bVmp ) m_byState |= 0x01; else m_byState &= ~0x01;}

	//�������״̬
	BOOL IsNoMixing( void ) const	{ return( ( m_byState & 0x06 ) == 0x00 ? TRUE : FALSE ); }
	void SetNoMixing( void )	{ m_byState &= ~0x06; }
	BOOL IsMixing( void ) const	{ return( ( m_byState & 0x06 ) == 0x02 ? TRUE : FALSE ); }
	void SetMixing( void )	{ SetNoMixing(); m_byState |= 0x02; }
	BOOL IsMixingPause( void ) const	{ return( ( m_byState & 0x06 ) == 0x04 ? TRUE : FALSE ); }
	void SetMixingPause( void )	{ SetNoMixing(); m_byState |= 0x04; }
	
	//���û�����
	LPCSTR	GetConfName( void );
	void	SetConfName( LPCSTR lpszBuf );
	//���û�������
	LPCSTR	GetConfPassword( void );
	void	SetConfPassword( LPCSTR lpszBuf );
	
	void SetSatIntervTime( u8 byTime );
	u8   GetSatIntervTime( );

	void SetMtInMix( u8 byMtId ){m_byMtInMix[byMtId / 8] |= 1 << ( byMtId % 8 );}
	BOOL IsMtInMix( u8 byMtId ){return ( ( m_byMtInMix[byMtId / 8] & ( 1 << ( byMtId % 8 ) ) ) != 0 );}

	void AddCurrUpLoadMt( u8 byMtId);
	void DelCurrUpLoadMt( u8 byMtId);
	u8	 IsMtInCurrUpLoad( u8 byMtId);

	BOOL IsVmpBrdst()
    {
        //FIXME:
        return 0;//m_tVmpParam.IsBrdst();
    }
	
    void SetVmpBrdst( BOOL bBrdst)
    {
        //m_tVmpParam.SetBrdst( bBrdst);
        return;
    }

	void SetVmpParam( TVMPParam &tVmpParam )
	{
		m_tVmpParam = tVmpParam;
	}

	TVMPParam* GetVmpParam()
	{
		return &m_tVmpParam;
	}

	void SetConfLineNum( u8 byNum )
	{
		m_byLineNum = byNum;
	}

	u8 GetConfLineNum()
	{
		return m_byLineNum;
	}

	void SetMpIpAddress(u32 dwIp)
	{
		m_tMpTransportAddr.SetIpAddr(dwIp);
	}
	u32 GetMpIpAddress()
	{
		return m_tMpTransportAddr.GetIpAddr();
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

////���ն˽���������MCU�ն���Ϣ�ṹ
struct CMtMultiCastInfo
{
protected:
	u8 m_abyMtId[MAXNUM_SATMULTICAST];
	TTransportAddr m_atTransportAddr[MAXNUM_SATMULTICAST];
	
public:
	CMtMultiCastInfo();
	
	u8   GetMtIndex( u8 byMtId );  
	BOOL IsMtInMultiCastInfo( u8 byMtId );
	BOOL GetMtMultiCastAddr( u8 byMtId, TTransportAddr * ptTransAddr );
	BOOL GetMtMultiCastIpAddr( u8 byMtId, u32 * pdwIpAddr );
	BOOL GetMtMultiCastPort( u8 byMtId, u16 * pwPort );
	BOOL AddMtToMultiCastInfo( u8 byMtId, const TTransportAddr & tTransAddr );
	void RemoveMtFromMultiCastInfo( u8 byMtId );
	u8   GetIndexMt( u8 byIndex );
	BOOL GetIndexAddr( u8 byIndex, TTransportAddr * ptTransAddr );
	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//�������ʽṹ
struct TBitRate
{
protected:
    u16		m_wBitRate;		//��λkbps
    
public:
    u16 GetBitRate( void ) const;
    void SetBitRate( u16 wBitRate );
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


struct CSatMtStatus
{
protected:
	u8	m_byMtId; //�ն�ID��
	u8  m_byConfId;//���ն����ڻ����ID�ţ�Ϊ0��ʾ�ն˲����κ�һ��������
	u8  m_byMediaSrc;//���ն˵�ǰ���ܵ�ý��Դ������ΪSpeaker������ѡ�����ն�
	TBitRate m_tSendFlowRate;//�ն˷�������������(���Ϊ0��ʾ�ն˲���Ҫ����)����λkbps.
	u8  m_byState;	// 0λ: �Ƿ���Ҫ������ѭ״̬( 0:����Ҫ��ѭ��1����Ҫ��ѭ����
					// 1λ���Ƿ��ڻ���״̬(0: �ն˲��ڻ�����1: �ն��ڻ�������
	//Added for Version 2
	u8  m_byCameraCmd; //�����ն�����ͷ����
	u8  m_byCameraVal; //����ͷ��������Ĳ���
	u8	m_byVoiceState;	   //�����ն�����
							// 0λ���Ƿ���( 1:Y. 0:N )
							// 1λ���Ƿ�����( 1:Y. 0:N )
	u8  m_asDecodeKey[16];
	CMtMultiCastInfo m_tMtMultiCastInfo; //�ն�ѡ����������أ��ն˵ĵ�ַ��Ϣ
public:
	u8 m_byVideoSrcId;
	u8 m_byAudioSrcId;
	TTransportAddr m_tVideoRecAddr;
	TTransportAddr m_tAudioRecAddr;


public:
	CSatMtStatus();

	void	SetMtId( u8 byMtId );
	u8		GetMtId( void );

	void	SetMtConfId( u8 byConfId );
	u8		GetMtConfId( void );

	void	SetMtMediaSrc( u8 byMediaSrc );
	u8		GetMtMediaSrc( void );

	//����/�������Ϊ������
	void	SetMtSendFlowRate( u16 wSendFlowRate );
	u16		GetMtSendFlowRate( void );

	void	SetMtNeedPolling( BOOL bNeedPolling = TRUE );
	BOOL	IsMtNeedPolling();

	void    SetMtMixing( BOOL bMixing = TRUE );
	BOOL	IsMtMixing();

	//Special For Version 2
	void    SetMtCameraCmd( u8 byCameraCmd );
	u8      GetMtCameraCmd();
	void    SetMtCameraVal( u8 byCameraVal );
	u8      GetMtCameraVal();
	
	void    SetMtVoiceState( u8 byVoiceState );
	
	void    SetMtMute( BOOL bMute = TRUE );
	BOOL	IsMtMute();

	void    SetMtDumb( BOOL bDumb = TRUE );
	BOOL	IsMtDumb();

	BOOL	IsMtInMultiCastInfo( u8 byMtId );
	BOOL	GetMtMultiCastIpAddr( u8 byMtId, u32 * pdwIpAddr );
	BOOL	GetMtMultiCastPort( u8 byMtId, u16 * pwPort );
	void	SetMtMultiCastInfo( const CMtMultiCastInfo & cMtMultiCastInfo );
	u8*    GetDecodeKey()
	{
		return m_asDecodeKey;
	}
	void   SetDecodeKey(u8* key)
	{
		for ( u8 byLoop = 0 ; byLoop < 16 ; byLoop ++)
			m_asDecodeKey[byLoop] = key[byLoop];
	}
	const CMtMultiCastInfo & GetMtMultiCastInfo();
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


struct CSatMsg
{
	enum
	{ 
		SATMSG_MAGIC = 0x0a0a0a0a,
		SATMSG_HEAD_LEN = 9,
		SATMSG_BODY_LEN = 1400 
	};
protected:
	u32	m_dwMagic;		//��Ϣͷ�Ļ�����
	u16 m_wEvent;		//��Ϣ���¼���
	u8	m_byConfId;		//��Ϣ�Ļ����
	u16 m_wBodyLen;		//��Ϣ�������ĳ���
	u8	m_abyBuffer[SATMSG_BODY_LEN];//������Ϣ������.
public:
    CSatMsg( void );
    CSatMsg( const u8 * pContent, u16 wLength );
    
	BOOL	IsValidSatMsg( void );	//�жϽ��ܵ���Ϣ�Ƿ�Ϸ�����Ҫ��У�����
	
	void	SetEventId( u16 wEvent ); //�����¼�ID��
	u16		GetEventId( void );

	void	SetConfId( u8 byConfId ); //���û����
	u8		GetConfId( void );
    
	u16		GetSatMsgHeadLen( void );

	u16		GetSatMsgLen( void );		//��ȡ������Ϣ����
    u8	*	GetSatMsg( void );

	void	SetSatMsgBody( const u8* pContent, u16 wLen );
	void	CatSatMsgBody( const u8* pContent, u16 wLen );
    
    u16		GetSatMsgBodyLen( void );		//��ȡ��Ϣ����������
 	void	SetSatMsgBodyLen( u16 wLen );   
	u8	*	GetSatMsgBody( void );     
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//����MT���˽ṹ
struct TSatMtTopo
{
protected:
	char m_achAlias[MAXLEN_ALIAS];	//��MT����      
	u8 m_byMtId;					//MT��(1-127)
	u8 m_byMtConnected;
public:
	u32 m_dwMtModemIp;			
	u16 m_wMtModemPort;		
	u8 m_byMtModemType;
public:
	
	u32 GetModemIpAddr( )
	{
		return m_dwMtModemIp;
	}
	u16 GetModemPort( )
	{
		return m_wMtModemPort;
	}
	u8 GetModemType()
	{
		return m_byMtModemType;
	}
	void SetMtId( u8 byMtId );
	u8	 GetMtId( void );
	void SetConnected( u8 byConnect)
	{
		m_byMtConnected = byConnect;
	}
	u8   GetConnected()
	{
		return m_byMtConnected;
	}
	LPCSTR GetAlias( void );
	void SetAlias( LPCSTR achAlias );
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//����
#define MAXNUM_TOPO_MCU     (u16)10
#define MAXNUM_TOPO_MT      (u16)1000


//�ն�����
const u8	MTTYPE_RCVTRS		= 1;	//���շ���
const u8	MTTYPE_RCVONLY		= 2;	//ֻ����
const u8	MTTYPE_TRSONLY		= 3;	//ֻ����
const u8	MTTYPE_RCVTRSADAPT	= 4;	//���շ��ͣ������䣩

//����MCU���˽ṹ
struct TMcuTopo
{
protected:
    u16	m_wMcuId;			//MCU��
    u16	m_wSuperiorMcuId;	//�ϼ�MCU��
    u32 m_dwIpAddr;			//��MCU��IP��ַ
    u32  m_dwGwIpAddr;		//��MCU������IP��ַ
    char m_achAlias[MAXLEN_ALIAS];	//��MCU����
    
public:
    u16  GetMcuId( void ) const;
    void SetMcuId( u16 wMcuId );

    u16  GetSMcuId( void ) const;
    void SetSMcuId( u16 wSMcuId );

    u32  GetIpAddr( void ) const;
    void SetIpAddr( u32 dwIpAddr );

    u32  GetGwIpAddr( void ) const;
    void SetGwIpAddr( u32 dwGwIpAddr );

    LPCSTR GetAlias( void ) const;
    void   SetAlias( LPCSTR achAlias );
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//����MT���˽ṹ
struct TMtTopo
{
public:
    u8 m_byMtId;			//MT��(1-127)
    u8 m_byPrimaryMtId;		//���������Secondary MT������������MT�ţ��������������MT��
    u8 m_byMtType;			//1: ���շ���, 2: ֻ���� 3: ֻ����
    u8 m_byMtConnected;     //�Ƿ�ֱ���ն�
    u32 m_dwMtModemIp;			
    u16 m_wMtModemPort;		
    u8 m_byMtModemType;
protected:
    u16	m_wMcuId;			//MT����MCU��
    u32 m_dwIpAddr;			//��MT��IP��ַ
    u32 m_dwGwIpAddr;		//��MT���ڵ�����IP��ַ
    char m_achAlias[MAXLEN_ALIAS];	//��MT���� 
    
    
public:
    u16 GetMcuId( void ) const;
    void SetMcuId( u16 wMcuId );
    
    u32 GetIpAddr( void ) const;
    void SetIpAddr( u32 dwIpAddr );

    u32 GetGwIpAddr( void ) const;
    void SetGwIpAddr( u32 dwMtGwIpAddr );
    
    LPCSTR GetAlias( void ) const;
    void SetAlias( LPCSTR achAlias );

    u8 GetMtId( void ) const;
    void SetMtId( u8 wMtId );

    u8 GetMtType( void ) const;
    void SetMtType( u8 byType );
    
    void SetConnected( u8 byConnect )
    {
        m_byMtConnected = byConnect;
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;



/*====================================================================
    ������      ��GetMcuId
    ����        ���õ�MCU��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��u16 �õ���MCU��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/09/18    1.0         LI Yi         ����
====================================================================*/
inline u16 TMcuTopo::GetMcuId( void ) const
{
	return( ntohs( m_wMcuId ) );
}

/*====================================================================
    ������      ��SetMcuId
    ����        ������MCU��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wMcuId, MCU��
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/09/18    1.0         LI Yi         ����
====================================================================*/
inline void TMcuTopo::SetMcuId( u16 wMcuId )
{
	m_wMcuId = htons( wMcuId );
}

/*====================================================================
    ������      ��GetSMcuId
    ����        ���õ��ϼ�MCU��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/09/18    1.0         LI Yi         ����
====================================================================*/
inline u16 TMcuTopo::GetSMcuId( void ) const
{
	return( ntohs( m_wSuperiorMcuId ) );
}

/*====================================================================
    ������      ��SetSMcuId
    ����        �������ϼ�MCU��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wSMcuId, �ϼ�MCU��
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/09/18    1.0         LI Yi         ����
====================================================================*/
inline void TMcuTopo::SetSMcuId( u16 wSMcuId )
{
	m_wSuperiorMcuId = htons( wSMcuId );
}

/*====================================================================
    ������      ��GetIpAddr
    ����        ���õ���MCU��IP��ַ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����void
    ����ֵ˵��  ����MCU IP��ַ(������)
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���				�޸�����
    02/10/24    1.0         LIAOWEIJIANG         ����
====================================================================*/
inline u32 TMcuTopo::GetIpAddr( void ) const
{
	return m_dwIpAddr;//������
}

/*====================================================================
    ������      ��SetIpAddr
    ����        �����ñ�MCU��IP��ַ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u32 dwIpAddr, IP��ַ(������)
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���				�޸�����
    02/10/24    1.0         LIAOWEIJIANG         ����
====================================================================*/
inline void TMcuTopo::SetIpAddr( u32 dwIpAddr )
{
	m_dwIpAddr = dwIpAddr;//������ 
}

/*====================================================================
    ������      ��GetGwIpAddr
    ����        ���õ���MCU������IP��ַ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����void
    ����ֵ˵��  ���õ���MCU������IP��ַ(������)
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���				�޸�����
    02/10/24    1.0         LIAOWEIJIANG         ����
====================================================================*/
inline u32 TMcuTopo::GetGwIpAddr( void ) const
{
	return m_dwGwIpAddr;//������
}

/*====================================================================
    ������      ��SetGwIpAddr
    ����        �����ñ�MCU������IP��ַ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u32 dwGwIpAddr, �µ����ص�ַ(������)
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���				�޸�����
    02/10/24    1.0         LIAOWEIJIANG         ����
====================================================================*/
inline void TMcuTopo::SetGwIpAddr( u32 dwGwIpAddr )
{
	m_dwGwIpAddr = dwGwIpAddr;//������ 
}

/*====================================================================
    ������      ��GetAlias
    ����        ���õ�MCU����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ���õ��ı���
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/24    1.0         LI Yi         ����
====================================================================*/
inline LPCSTR TMcuTopo::GetAlias( void ) const
{
	return( m_achAlias );
}

/*====================================================================
    ������      ��SetAlias
    ����        ������MCU����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����LPCSTR lpszAlias, MCU����
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/24    1.0         LI Yi         ����
====================================================================*/
inline void TMcuTopo::SetAlias( LPCSTR lpszAlias )
{
	strncpy( m_achAlias, lpszAlias, sizeof( m_achAlias ) );
	m_achAlias[sizeof( m_achAlias ) - 1] = '\0';
}

/*====================================================================
    ������      ��GetMcuId
    ����        ���õ�MCU��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ��u16 �õ���MCU��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/24    1.0         LI Yi         ����
====================================================================*/
inline u16 TMtTopo::GetMcuId( void ) const
{
	return( ntohs( m_wMcuId ) );
}

/*====================================================================
    ������      ��SetMcuId
    ����        ������MCU��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u16 wMcuId, MCU��
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/24    1.0         LI Yi         ����
====================================================================*/
inline void TMtTopo::SetMcuId( u16 wMcuId )
{
	m_wMcuId = htons( wMcuId );
}

/*====================================================================
    ������      ��GetIpAddr
    ����        ���õ���Mt��IP��ַ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ���õ���IP��ַ(������)
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/24    1.0         LI Yi         ����
====================================================================*/
inline u32 TMtTopo::GetIpAddr( void ) const
{
	return m_dwIpAddr;//������
}

/*====================================================================
    ������      ��SetMtIpAddr
    ����        ������IP��ַ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u32 dwIpAddr, �µ�IP��ַ(������)
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/24    1.0         LI Yi         ����
====================================================================*/
inline void TMtTopo::SetIpAddr( u32 dwIpAddr )
{
	m_dwIpAddr = dwIpAddr;//������
}

/*====================================================================
    ������      ��GetGwIpAddr
    ����        ���õ���Mt������IP��ַ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ���õ���IP��ַ(������)
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/24    1.0         LI Yi         ����
====================================================================*/
inline u32 TMtTopo::GetGwIpAddr( void ) const
{
	return m_dwGwIpAddr;//������
}

/*====================================================================
    ������      ��SetGwIpAddr
    ����        �����ø�Mt�����ص�ַ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����u32 dwGwIpAddr, �µ�����IP��ַ(������)
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/24    1.0         LI Yi         ����
====================================================================*/
inline void TMtTopo::SetGwIpAddr( u32 dwGwIpAddr )
{
	m_dwGwIpAddr = dwGwIpAddr;//������
}

/*====================================================================
    ������      ��GetAlias
    ����        ���õ�MT����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵������
    ����ֵ˵��  ���õ��ı���
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/24    1.0         LI Yi         ����
====================================================================*/
inline LPCSTR TMtTopo::GetAlias( void ) const
{
	return( m_achAlias );
}

/*====================================================================
    ������      ��SetAlias
    ����        ������MCU��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����LPCSTR lpszAlias, �����ַ���ָ��
    ����ֵ˵��  ����
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/24    1.0         LI Yi         ����
====================================================================*/
inline void TMtTopo::SetAlias( LPCSTR lpszAlias )
{
	strncpy( m_achAlias, lpszAlias, sizeof( m_achAlias ) );
	m_achAlias[sizeof( m_achAlias ) - 1] = '\0';
}

inline void TMtTopo::SetMtId( u8 byMtId )
{
    m_byMtId = byMtId;
}

inline u8 TMtTopo::GetMtId( void ) const
{
    return m_byMtId;
}

inline void TMtTopo::SetMtType( u8 byType )
{
    m_byMtType = byType;
}

inline u8 TMtTopo::GetMtType( void ) const
{
    return m_byMtType;
}

inline CSatMsg::CSatMsg( void )
{
    memset( this, 0, sizeof( CSatMsg ) );
    m_dwMagic = htonl( SATMSG_MAGIC );
}

inline CSatMsg::CSatMsg( const u8* pContent, u16 wMsgLen )
{
    
    if( wMsgLen < SATMSG_HEAD_LEN || pContent == NULL )
        return;
    
    wMsgLen = min( wMsgLen, sizeof( m_abyBuffer ) + SATMSG_HEAD_LEN );
    memcpy( this, pContent, wMsgLen );
    //set length
    SetSatMsgBodyLen( wMsgLen - SATMSG_HEAD_LEN );
}

inline u16 CSatMsg::GetSatMsgHeadLen()
{
    return SATMSG_HEAD_LEN;
}

inline u16 CSatMsg::GetSatMsgLen()
{
    return ( GetSatMsgBodyLen() + SATMSG_HEAD_LEN );
}

inline  u8* CSatMsg::GetSatMsg()
{
    return( (  u8 * )( this ) );
}

inline u16 CSatMsg::GetSatMsgBodyLen()
{
    return ntohs( m_wBodyLen ); 
}

inline void CSatMsg::SetSatMsgBodyLen( u16 wLen )
{
    wLen = min( wLen, SATMSG_BODY_LEN );
    m_wBodyLen = htons( wLen );
}

inline u8 * CSatMsg::GetSatMsgBody()
{
    return ( u8 * )( m_abyBuffer  );
}

inline u8 CSatMsg::GetConfId()
{
    return m_byConfId;
}

inline void CSatMsg::SetConfId( u8 byConfId )
{
    m_byConfId = byConfId;
}

inline u16 CSatMsg::GetEventId()
{
    return ntohs( m_wEvent );
}

inline void CSatMsg::SetEventId( u16 wEvent )
{
    m_wEvent = htons( wEvent );
}



inline BOOL CSatMsg::IsValidSatMsg( )
{
    return( ntohl( m_dwMagic)  == SATMSG_MAGIC );
}


inline void CSatMsg::SetSatMsgBody( const u8* pContent, u16 wLen )
{
    wLen = min( wLen, SATMSG_BODY_LEN );
    memcpy( m_abyBuffer, pContent, wLen );
    SetSatMsgBodyLen( wLen );
}

inline void CSatMsg::CatSatMsgBody( const u8* pContent, u16 wLen )
{
    wLen = min( wLen, SATMSG_BODY_LEN - GetSatMsgBodyLen() );
    
    memcpy( m_abyBuffer + GetSatMsgBodyLen(), pContent, wLen );
    SetSatMsgBodyLen( GetSatMsgBodyLen() + wLen );
}





inline CSatConfInfo::CSatConfInfo()
{
	memset( this, 0, sizeof( CSatConfInfo ) );
}

inline void CSatConfInfo::SetConfId( u8 byConfId )
{
	m_byConfId = byConfId;
}

inline u8  CSatConfInfo::GetConfId()
{
	return m_byConfId;
}

inline void CSatConfInfo::SetChairmanId( u8 byMtId )
{
	m_byChairmanId  = byMtId;
}

inline u8 CSatConfInfo::GetChairmanId()
{
	return m_byChairmanId;
}

inline void CSatConfInfo::SetSpeakerId( u8 byMtId )
{
	m_bySpeakerId = byMtId;
}

inline u8 CSatConfInfo::GetSpeakerId()
{
	return m_bySpeakerId;
}

inline void CSatConfInfo::SetConfName( LPCSTR lpszConfName )
{
	if( lpszConfName != NULL )
	{
		strncpy( m_achConfName, lpszConfName, sizeof( m_achConfName ) );
		m_achConfName[sizeof( m_achConfName ) - 1] = '\0';
	}
	else
	{
		memset( m_achConfName, 0, sizeof( m_achConfName ) );
	}
}

inline LPCSTR CSatConfInfo::GetConfName()
{
	 return( ( LPCSTR )m_achConfName );
}

inline void CSatConfInfo::SetConfPassword( LPCSTR lpszConfPasswd )
{
	if( lpszConfPasswd != NULL )
	{
		strncpy( m_achPasswd, lpszConfPasswd, sizeof( m_achPasswd ) );
		m_achPasswd[sizeof( m_achPasswd ) - 1] = '\0';
	}
	else
	{
		memset( m_achPasswd, 0, sizeof( m_achPasswd ) );
	}
}

inline LPCSTR CSatConfInfo::GetConfPassword()
{
	 return( ( LPCSTR )m_achPasswd );
}

inline void CSatConfInfo::AddMt( u8 byMtId )
{
	m_tConfMtInfo.AddMt( byMtId );
}

inline void CSatConfInfo::RemoveMt( u8 byMtId )
{
	m_tConfMtInfo.RemoveMt( byMtId );
}

inline void CSatConfInfo::AddJoinedMt( u8 byMtId )
{
	m_tConfMtInfo.AddJoinedMt( byMtId );
}

inline void CSatConfInfo::RemoveJoinedMt( u8 byMtId )
{
	m_tConfMtInfo.RemoveJoinedMt( byMtId );
}

inline void CSatConfInfo::RemoveAllMt()
{
	m_tConfMtInfo.RemoveAllMt();
}

inline BOOL CSatConfInfo::IsMtInConf( u8 byMtId )
{
	return m_tConfMtInfo.MtInConf( byMtId );
}

inline BOOL CSatConfInfo::IsMtJoinedConf( u8 byMtId )
{
	return m_tConfMtInfo.MtJoinedConf( byMtId );
}

inline void CSatConfInfo::SetBrdMediaStartPort( u16 wPort )
{
	m_tBrdTransportAddr.SetPort(  wPort );
}

inline u16 CSatConfInfo::GetBrdMediaStartPort( )
{
	return ( m_tBrdTransportAddr.GetPort() );
}

inline void CSatConfInfo::SetBrdMeidaIpAddr( u32 dwIpAddress )
{
	m_tBrdTransportAddr.SetIpAddr( htonl( dwIpAddress ) );
}

inline u32 CSatConfInfo::GetBrdMeidaIpAddr()
{
	return ntohl( m_tBrdTransportAddr.GetIpAddr() );
}

inline void CSatConfInfo::SetConfMtInfo( const TConfMtInfo & tConfMtInfo )
{
	m_tConfMtInfo = tConfMtInfo;
}

inline const TConfMtInfo & CSatConfInfo::GetConfMtInfo()
{
	return m_tConfMtInfo;
}

inline  void CSatConfInfo::SetConfMtInfoMcuId( u8 byMcuId )
{
	m_tConfMtInfo.SetMcuId( byMcuId );
}

inline  u8 CSatConfInfo::GetConfMtInfoMcuId()
{
	return (u8)m_tConfMtInfo.GetMcuId();
}

inline void CSatConfInfo::SetSatIntervTime( u8 byTime )
{
	m_byIntervalTime = byTime;
	return ;
}

inline u8 CSatConfInfo::GetSatIntervTime()
{
	return m_byIntervalTime;
}

inline void CSatConfInfo::AddCurrUpLoadMt( u8 byMtId)
{
	if( byMtId > MAXNUM_MCU_MT - 1  )
	{
		OspLog( LOGLVL_EXCEPTION, "Mt Id out of range %d", byMtId );
		return ;
	}
	
	m_byUpLoadCurrDmt[byMtId / 8] |= 1 << ( byMtId % 8 );
}

inline void CSatConfInfo::DelCurrUpLoadMt( u8 byMtId)
{
	if( byMtId > MAXNUM_MCU_MT - 1  )
	{
		OspLog( LOGLVL_EXCEPTION, "Mt Id out of range %d", byMtId );
		return ;
	}
	
	m_byUpLoadCurrDmt[byMtId / 8] &= ~( 1 << ( byMtId % 8 ) );
}

inline u8 CSatConfInfo::IsMtInCurrUpLoad( u8 byMtId)
{
	if( byMtId > MAXNUM_MCU_MT - 1  )
	{
		OspLog( LOGLVL_EXCEPTION, "Mt Id out of range %d", byMtId );
		return FALSE;
	}
	return ( ( m_byUpLoadCurrDmt[byMtId / 8] & ( 1 << ( byMtId % 8 ) ) ) != 0 );
}

#endif

//END OF FILE