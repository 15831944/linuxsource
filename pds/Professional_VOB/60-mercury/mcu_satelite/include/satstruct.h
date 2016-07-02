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


#ifdef WIN32
#pragma comment( lib, "ws2_32.lib" ) 
#pragma pack( push )
#pragma pack( 1 )
#endif

#define  SIGNAL_CHN_BOTH				(u8)0
#define  SIGNAL_CHN_SD					(u8)1
#define  SIGNAL_CHN_HD					(u8)2

#define  MAXLEN_CONF_NAME               (u8)32
#define  MAXLEN_CONF_PASSWD             (u8)32
#define	 MAXNUM_SATMULTICAST            (u8)5       //������·����鲥������
#define  MCU_SENDTOMULTI_PORT           (u16)5000
#define  MT_RCVMCUMEDIA_PORT            (u16)4000	//�ն˽���MCU���ݵ���ʼ�˿�

#define  MAXNUM_SATMT                   (u16)1024   //����ն���


#define MOD_CLKSRC_RCV					3			//���ͣ���ʱ��Դ
#define MOD_CLKSRC_INTER				0			//���ͣ���ʱ��Դ
#define DEM_CLKSRC_RCV					0			//���գ���ʱ��Դ
#define DEM_CLKSRC_INTER				1			//���գ���ʱ��Դ


#define MAXLEN_SATMTALIAS				16

//����ռ���ն˺�
// const u16 BRDCST_VMP			        = 1026;
// const u16 BRDCST_MIXN			    = 1027;
// const u16 BRDCST_MIX			        = 1025;

const u16 BRDCST_VMP			        = 254;
const u16 BRDCST_MIXN			        = 253;
const u16 BRDCST_MIX			        = 252;


const u8 MTSEE_NONE						= 0;
const u8 MTSEE_BRDCAST			        = 1;
const u8 MTSEE_UNICAST			        = 2;


//���ǻ��� ��������
struct TMcuSatInfo
{
public:
    TMcuSatInfo():m_dwApplyIpAddr(0),
                  m_wApplyPort(0),
                  m_wApplyReceivPort(0),
                  m_wRcvMtSignalPort(0),
                  m_dwTimeRefreshIpAddr(0),
                  m_wTimeRefreshPort(0),
                  m_wMcuRcvMtMediaStartPort(0),
                  m_wMtRcvMcuMediaPort(0),
				  m_dwMcuMulticastSignalIpAddr(0),
				  m_wMcuMulticastSignalPort(0),
				  m_dwMcuMulticastDataIpAddr(0),
				  m_wMcuMulticastDataPort(0),
				  m_wMcuMulticastDataPortNum(0),
				  m_byMcuMulticastTTL(0),
				  m_byMcuMaxUploadCount(0),
				  m_dwMcuSecMulticastIpAddr(0)
    {
    }

    u32  GetApplyIp(void) const { return ntohl(m_dwApplyIpAddr); }
    void SetApplyIp(u32 dwIpAddr) { m_dwApplyIpAddr = htonl(dwIpAddr);  }

    u16  GetApplyPort(void) const { return ntohs(m_wApplyPort);    }
    void SetApplyPort(u16 wPort) { m_wApplyPort = htons(wPort); }

    u16  GetApplyRcvPort(void) const { return ntohs(m_wApplyReceivPort);    }
    void SetApplyRcvPort(u16 wPort) { m_wApplyReceivPort = htons(wPort);    }

    u16  GetRcvMtSignalPort(void) const { return ntohs(m_wRcvMtSignalPort);   }
    void SetRcvMtSignalPort(u16 wPort) { m_wRcvMtSignalPort = htons(wPort); }

    u32  GetTimeRefreshIpAddr(void) const { return ntohl(m_dwTimeRefreshIpAddr);    }
    void SetTimeRefreshIpAddr(u32 dwIp) { m_dwTimeRefreshIpAddr = htonl(dwIp);  } 

    u16  GetTimeRefreshPort(void) const { return ntohs(m_wTimeRefreshPort); }
    void SetTImeRefreshPort(u16 wPort) { m_wTimeRefreshPort = htons(wPort); }

    u16  GetMcuRcvMtMediaStartPort(void) const { return ntohs(m_wMcuRcvMtMediaStartPort);   }
    void SetMcuRcvMtMediaStartPort(u16 wPort) { m_wMcuRcvMtMediaStartPort = htons(wPort);   }

    u16  GetMtRcvMcuMediaPort(void) const { return ntohs(m_wMtRcvMcuMediaPort); }
    void SetMtRcvMcuMediaPort(u16 wPort) { m_wMtRcvMcuMediaPort = htons(wPort); }

	u32	 GetMcuMulticastSignalIpAddr(void) const { return ntohl(m_dwMcuMulticastSignalIpAddr);	}
	void SetMcuMulitcastSignalIpAddr(u32 dwIp) {	m_dwMcuMulticastSignalIpAddr = htonl(dwIp);	}

	u16  GetMcuMulticastSignalPort(void) const {	return ntohs(m_wMcuMulticastSignalPort);	}
	void SetMcuMulticastSignalPort(u16 wPort){	m_wMcuMulticastSignalPort = htons(wPort);	}

	u32	 GetMcuMulticastDataIpAddr(void) const { return ntohl(m_dwMcuMulticastDataIpAddr);	}
	void SetMcuMulticastDataIpAddr(u32 dwIp) { m_dwMcuMulticastDataIpAddr = htonl(dwIp);	}

	u16  GetMcuMulticastDataPort(void) const {	return ntohs(m_wMcuMulticastDataPort);	}
	void SetMcuMulticastDataPort(u16 wPort) { m_wMcuMulticastDataPort = htons(wPort);	}

	u16  GetMcuMulticastDataPortNum(void) const{ return ntohs(m_wMcuMulticastDataPortNum);	}
	void SetMcuMulticastDataPortNum(u16 wPort) { m_wMcuMulticastDataPortNum = htons(wPort);	}

	u8   GetMcuMulticastTTL(void) const { return m_byMcuMulticastTTL;	}
	void SetMcuMulticastTTL(u8 byTTL) { m_byMcuMulticastTTL = byTTL;	}

	u8   GetMcuMaxUploadNum(void) const { return m_byMcuMaxUploadCount;	}
	void SetMcuMaxUploadNum(u8 byNum) { m_byMcuMaxUploadCount = byNum;	}

	u32  GetMcuSecMulticastIpAddr(void) const { return ntohl(m_dwMcuSecMulticastIpAddr);	}
	void SetMcuSecMulticastIpAddr(u32 dwIp) { m_dwMcuSecMulticastIpAddr = htonl(dwIp);	}

    void Print() const
    {
        OspPrintf(TRUE, FALSE, "McuSatConfValue as follows:\n");
        OspPrintf(TRUE, FALSE, "\tm_dwApplyIpAddr: \t\t0x%x\n",			GetApplyIp());
        OspPrintf(TRUE, FALSE, "\tm_wApplyPort: \t\t\t%d\n",			GetApplyPort());
        OspPrintf(TRUE, FALSE, "\tm_wApplyReceivPort: \t\t%d\n",		GetApplyRcvPort());
        OspPrintf(TRUE, FALSE, "\tm_wRcvMtSignalPort: \t\t%d\n",		GetRcvMtSignalPort());
        OspPrintf(TRUE, FALSE, "\tm_dwTimeRefreshIpAddr: \t\t0x%x\n",	GetTimeRefreshIpAddr());
        OspPrintf(TRUE, FALSE, "\tm_wTimeRefreshPort: \t\t%d\n",		GetTimeRefreshPort());
        OspPrintf(TRUE, FALSE, "\tm_wMcuRcvMediaStartPort: \t%d\n",		GetMcuRcvMtMediaStartPort());
        OspPrintf(TRUE, FALSE, "\tm_dwMcuMulticastSignalIpAddr: \t0x%x\n", GetMcuMulticastSignalIpAddr());
        OspPrintf(TRUE,	FALSE, "\tm_wMcuMulticastSignalPort: \t%d\n",	GetMcuMulticastSignalPort());
		OspPrintf(TRUE, FALSE, "\tm_dwMcuMulticastDataIpAddr: \t0x%x\n", GetMcuMulticastDataIpAddr());
		OspPrintf(TRUE, FALSE, "\tm_dwMcuSecMulticastIpAddr:\t\0x%x\n",	GetMcuSecMulticastIpAddr());
        OspPrintf(TRUE,	FALSE, "\tm_wMcuMulticastDataPort: \t%d\n",		GetMcuMulticastDataPort());
        OspPrintf(TRUE,	FALSE, "\tm_wMcuMulticastDataPortNum: \t%d\n",	GetMcuMulticastDataPortNum());
		OspPrintf(TRUE, FALSE, "\tm_byMcuMaxUploadCount:\t\t%d\n",		GetMcuMaxUploadNum());
		OspPrintf(TRUE, FALSE, "\tm_byMcuMulticastTTL:\t\t%d\n",		GetMcuMulticastTTL());
    }

private:
    u32             m_dwApplyIpAddr;					/*����Ƶ��-������*/
    u16             m_wApplyPort;						/*����Ƶ��*/
    u16             m_wApplyReceivPort;					/*����Ƶ��Ӧ��˿�*/
    u16             m_wRcvMtSignalPort;                 /*�����ն���������Ķ˿�*/
    u32             m_dwTimeRefreshIpAddr;              /*ʱ����������ַ*/
    u16             m_wTimeRefreshPort;                 /*ʱ���������˿�*/
    u16             m_wMcuRcvMtMediaStartPort;          /*Mcu����Mt�ϴ���ý�������ʼ�˿�*/
    u16             m_wMtRcvMcuMediaPort;               /*MT����Mcu�㲥��ý����ն˿�*/
	u32				m_dwMcuMulticastSignalIpAddr;		/*MCU�����鲥�����IP*/
	u16				m_wMcuMulticastSignalPort;			/*MCU�����鲥����Ķ˿�*/
	u32				m_dwMcuMulticastDataIpAddr;			/*MCU�����鲥���ݵ�IP*/
	u16				m_wMcuMulticastDataPort;			/*MCU�����鲥���ݵĶ˿�*/
	u16				m_wMcuMulticastDataPortNum;			/*MCU�����鲥���ݶ˿ڵĸ���*/
	u8				m_byMcuMulticastTTL;				/*MCU�鲥TTLֵ*/
	u8				m_byMcuMaxUploadCount;				/*���ǻ�������ϴ�·��*/
	u32				m_dwMcuSecMulticastIpAddr;			/*���ǻ���ڶ��鲥Ip, ����˫�ٻ�˫��ʽ����*/
	                                                    /*�鲥Ip�Ժ������������й���������վ��ĸ�ʽ���ʳ���������*/
};


typedef struct tagTInterfaceConfList
{
	u8 m_byConfIdx;
	char m_achName[16];
	tagTInterfaceConfList()
	{
		memset(this,0,sizeof(tagTInterfaceConfList));
	}
public:
	void SetConfIdx( u8 byIndex )
	{
		m_byConfIdx = byIndex;
	}
	u8 GetConfIdx(void)
	{
		return m_byConfIdx;
	}
	
	void SetConfName( LPCSTR lpszAlias )
	{
		if( lpszAlias != NULL )
		{
			strncpy( m_achName, lpszAlias, sizeof( m_achName ) );
			m_achName[sizeof( m_achName ) - 1] = '\0';
		}
		else
		{
			memset( m_achName, 0, sizeof( m_achName ) );
		}
	}
	char* GetConfName()
	{
		return m_achName;
	}
	
}PACKED TInterfaceConfList ,ITSatConfList;

//MODEM����
struct TModemData
{
    u16 wConfIdxSnd;    //ʹ�ñ�MDOEM���͵Ļ����
	u32 dwSendFrequece; //��MDOEM�ķ���Ƶ��
	u32 dwSendBitRate;  //��MDOEM�ķ�������
	TMt tMtSnd;			//ʹ�ñ�MDOEM�ķ��͵��ն�/����

    u16 wConfIdxRcv;    //ʹ�ñ�MDOEM���յĻ����
    u32 dwRevFrequece;  //��MDOEM�Ľ���Ƶ��
    u32 dwRevBitRate;   //��MDOEM�Ľ�������
    TMt tMtRcv;			//ʹ�ñ�MDOEM�ķ��͵��ն�/����

	BOOL bConnected;    //�Ƿ�����
    u8   bySPortNum;	//��Modem����MCU��S�˿ں�
};

//���������Ǵ����¼ �ṹ
struct TConfFreqBitRate
{
    u8 byConfIdx;                       //�����
    u8 byNum;                           //����Ƶ�ʸ���
    u32 dwSendFreq;                     //����Ƶ��
    u32 dwRevFreq[MAXNUM_MCU_MODEM];    //����Ƶ��
    u32 dwSendBitRate;                  //��������
    u32 dwRevBitRate;                   //��������

	void Print()
	{
		OspPrintf(TRUE, FALSE, "[ConfIdx.%d]:\n", byConfIdx);
		OspPrintf(TRUE, FALSE, "\tSndFrq:\t\t%d\n", dwSendFreq);
		OspPrintf(TRUE, FALSE, "\tRcvFrqNum:\t%d\n", byNum);
		for (u8 byIdx = 0; byIdx < byNum; byIdx ++)
		{
			OspPrintf(TRUE, FALSE, "\t      [%d]:\t%d\n", byIdx, dwRevFreq[byIdx]);
		}
		OspPrintf(TRUE, FALSE, "\tRcvBitRate:\t%d\n", dwRevBitRate);
		OspPrintf(TRUE, FALSE, "\tSndBitRate:\t%d\n", dwSendBitRate);
	}
};


//����������Ƶ��(MCU->NMS)
struct TMcuApplyFreq
{
protected:
	u16 wMessageType;   //������
	u32 dwSerialId;     //��ˮ��
	u32 dwBrdBitRate;   //�㲥����
	u16 wRcvNum;        //��������
	u32 dwRcvBitRate;   //��������
	u8	abyConfId[16];
public:
	void SetConfId( CConfId &cConfId )
	{
		u8 byConf[16];
		memset( byConf, 0 , 16 );
		cConfId.GetConfId( byConf, 16 );
		for ( u8 byLoop = 0; byLoop < 16 ; byLoop ++ )
			abyConfId[byLoop] = byConf[byLoop];
	}
	void SetMessageType( u16 wCmd )
	{
		wMessageType = htons( wCmd );
	}
	
	void SetSN( u32 dwSN )
	{
		dwSerialId = htonl( dwSN );
	}
	void SetBrdBitRate( u32 dwBit )
	{
		dwBrdBitRate = htonl( dwBit );
	}
	
	void SetRcvNum( u16 wNum )
	{
		wRcvNum = htons( wNum );
	}

	void SetRcvBitRate( u32 dwBit )
	{
		dwRcvBitRate = htonl( dwBit );
	}

	u32 GetSN()
	{
		return ntohl( dwSerialId );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//�������ͷ�Ƶ��(MCU->NMS)
struct TMcuReleaseFreq
{
protected:
	u16 wMessageType;   //������
	u32 dwSerialId;     //��ˮ��
	u32 dwBrdBitRate;   //�㲥����
	u32 dwBitRate;      //��������
	u16 wFreqCount;     //���ո���
	u32 dwBrdFreq;      //�㲥
	u32 dwFreq[5];
public:
	void SetCmd( u16 wCmd )
	{
		wMessageType = htons( wCmd );
	}
	
	void SetSN( u32 dwSN )
	{
		dwSerialId = htonl( dwSN );
	}
	void SetBrdBitRate( u32 dwBit )
	{
		dwBrdBitRate = htonl( dwBit );
	}
	void SetRcvBitRate( u32 dwBit )
	{
		dwBitRate = htonl( dwBit );
	}
	
	void SetNum( u16 wNum )
	{
		wFreqCount = htons( wNum );
	}
	
	u32 GetSN()
	{
		return ntohl( dwSerialId );
	}
	void SetBrdFreq( u32 dwFrq )
	{
		dwBrdFreq = htonl( dwFrq );
	}
	void SetFreeFreq( u8 byPos, u32 Freq )
	{
		if ( byPos < 1 || byPos > 5 )
			return;
		dwFreq[byPos-1] = htonl( Freq );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//��������Ļ�Ӧ(NMS->MCU)
struct TFreqGetMsg
{
protected:
	u16 wMessageType;   //������
	u32 dwSerialId;     //��ˮ��
	u16 wFreqCount; 
	u32 dwBrdFrq;
	u32 dwContent[5];
public:

	u16 GetMessageType()
	{
		return ntohs( wMessageType );
	}
	u32 GetSN()
	{
		return ntohl( dwSerialId );
	}
	u16 GetNum()
	{
		return ntohs( wFreqCount );
	}
	u32 GetContent( u8 byId )
	{
		if ( byId < 1 || byId > 5 )
			return 0;
		return ntohl( dwContent[byId-1] );
	}
	u32 GetBrdFreq()
	{
		return ntohl( dwBrdFrq );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//��������Ļ�Ӧ(NMS->MCU)
struct TFreqReleaseMsg
{
protected:
	u16 wMessageType;   //������
	u32 dwSerialId;     //��ˮ��
	u16 wFlags; 
public:
	u16 GetMessageType()
	{
		return ntohs( wMessageType );
	}
	u32 GetSN()
	{
		return ntohl( dwSerialId );
	}
	u16 GetFlags()
	{
		return ntohs( wFlags );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
//���Ļ�����Ϣ
struct TConfInfoNotif
{
protected:
	u16 wMessageType;   //������
	u32 dwSerialId;     //��ˮ��
	u8	abyConfId[16];  //��������
	u32 dwStartTime;    //���鿪ʼʱ��
	u32 dwEndTime;      //�������ʱ��
public:
	void SetSerialId( u32 dwSN )
	{
		dwSerialId = htonl( dwSN );
	}
	void SetMessageType( u16 wMsg )
	{
		wMessageType = htons( wMsg );
	}
	void SetSN( u32 dwSn )
	{
		dwSerialId = htonl( dwSn );
	}
	void SetStartTime( u32 dwTime )
	{
		dwStartTime = htonl( dwTime );
	}
	void SetEndTime( u32 dwTime )
	{
		dwEndTime = htonl( dwTime );
	}
	void SetConfId( const CConfId &cConfId )
	{
		u8 byConf[16];
		memset( byConf, 0 , 16 );
		cConfId.GetConfId( byConf, 16 );
		for ( u8 byLoop = 0; byLoop < 16 ; byLoop ++ )
			abyConfId[byLoop] = byConf[byLoop];
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//���ն˽���������MCU������Ϣ�ṹ
struct CSatConfInfo
{
protected:
	u8	m_byConfId;	        //��MCU�У��û���Ļ��������ţ���Ӧÿһ�����������ͬ������
					        //��Ϊ���ֻ���Ļ����
	u8  m_bySpeakerId;      //��ǰ�����˵��ն�ID�ţ�Ϊ���ʾ��ǰû�з����ˡ�
	u8  m_byChairmanId;     //��ǰ��ϯ���ն�ID�ţ�Ϊ���ʾ��ǰû����ϯ��

	TTransportAddr  m_tBrdTransportAddr;    //�������������鲥��ַ
	TConfMtInfo     m_tConfMtInfo;          //����ն�״̬
	TTransportAddr  m_tMpTransportAddr;     //�ն˷��������ĵ�ַ

	u8	m_byState;          //bit0: 0:δ���渴�� 1:���渴��
							//bit1-2: 00:�ǻ��� 01:���� 10:������ͣ
							//bit3-4: 00:��¼�� 01:¼�� 10:¼����ͣ
							//bit5-7: 000:�Ƿ��� 001:���� 010:������ͣ 011:��� 100:����		
	
    char m_achConfName[MAXLEN_CONF_NAME];   //������
	char m_achPasswd[MAXLEN_CONF_PASSWD];   //��������	
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

//���ն˽���������MCU�ն���Ϣ�ṹ
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
	u8	m_byMtId;               //�ն�ID��
	u8  m_byConfId;             //���ն����ڻ����ID�ţ�Ϊ0��ʾ�ն˲����κ�һ��������
	u8  m_byMediaSrc;           //���ն˵�ǰ���ܵ�ý��Դ������ΪSpeaker������ѡ�����ն�
	TBitRate m_tSendFlowRate;   //�ն˷�������������(���Ϊ0��ʾ�ն˲���Ҫ����)����λkbps.
	u8  m_byState;	            // 0λ: �Ƿ���Ҫ������ѭ״̬( 0:����Ҫ��ѭ��1����Ҫ��ѭ����
					            // 1λ���Ƿ��ڻ���״̬(0: �ն˲��ڻ�����1: �ն��ڻ�������
	//Added for Version 2
	u8  m_byCameraCmd;          //�����ն�����ͷ����
	u8  m_byCameraVal;          //����ͷ��������Ĳ���
	u8	m_byVoiceState;	        //�����ն�����
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
    
	BOOL	IsValidSatMsg( void );	    //�жϽ��ܵ���Ϣ�Ƿ�Ϸ�����Ҫ��У�����
	
	void	SetEventId( u16 wEvent );   //�����¼�ID��
	u16		GetEventId( void );

	void	SetConfId( u8 byConfId );   //���û����
	u8		GetConfId( void );
    
	u16		GetSatMsgHeadLen( void );

	u16		GetSatMsgLen( void );	    //��ȡ������Ϣ����
    u8	*	GetSatMsg( void );

	void	SetSatMsgBody( const u8* pContent, u16 wLen );
	void	CatSatMsgBody( const u8* pContent, u16 wLen );
    
    u16		GetSatMsgBodyLen( void );	//��ȡ��Ϣ����������
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
    u32 m_dwMtModemIp;			
    u16 m_wMtModemPort;
    u8  m_byMtModemType;
	u8  m_byMtConnected;     //�Ƿ�ֱ���ն�
protected:
    u16	m_wMcuId;			//MT����MCU��
    u32 m_dwIpAddr;			//��MT��IP��ַ
    u32 m_dwGwIpAddr;		//��MT���ڵ�����IP��ַ
    s8  m_achAlias[MAXLEN_ALIAS];	//��MT����
	u8  m_byIsHD;			//��MT�Ƿ�Ϊ����MT

    
public:
    u16  GetMcuId( void ) const;
    void SetMcuId( u16 wMcuId );
    
    u32  GetIpAddr( void ) const;
    void SetIpAddr( u32 dwIpAddr );

    u32  GetGwIpAddr( void ) const;
    void SetGwIpAddr( u32 dwMtGwIpAddr );
    
    LPCSTR GetAlias( void ) const;
    void   SetAlias( LPCSTR achAlias );

    u8   GetMtId( void ) const;
    void SetMtId( u8 wMtId );

    u8   GetMtType( void ) const;
    void SetMtType( u8 byType );
    
    void SetConnected( u8 byConnect )
    {
        m_byMtConnected = byConnect >= 1 ? byConnect: 0;
    }
	BOOL GetConnected( void )
	{
		return m_byMtConnected == 1;
	}
	
	u32 GetModemIp( void ) const { return htonl(m_dwMtModemIp);	}
	u16 GetModemPort( void ) const { return htons(m_wMtModemPort);	}
	u8  GetModemType( void ) const { return m_byMtModemType;	}

	BOOL IsMtHd( void ) const { return m_byIsHD == 1 ? TRUE: FALSE;	};
	void SetMtHd( BOOL bHd ) { m_byIsHD = bHd ? 1: 0;	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;



/*====================================================================
������      : TMcuModemTopo
����        ��
�㷨ʵ��    ��
����ȫ�ֱ�����
�������˵����
����ֵ˵��  ��u8 byMtId
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��      �汾        �޸���        �޸�����
03/11/26    1.0         zhangsh           ����
====================================================================*/
struct TMcuModemTopo
{
public:
    u8 m_byModemId; //���
    u32 m_dwMcuModemIp;	//IP��ַ		
    u16 m_wMcuModemPort;//�˿�		
    u8 m_byMcuModemType;//����
    u8 m_bySPortNum; //��Ӧ��S�ں�
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
	if( byMtId > MAXNUM_CONF_MT - 1  )
	{
		OspLog( LOGLVL_EXCEPTION, "Mt Id out of range %d", byMtId );
		return ;
	}
	
	m_byUpLoadCurrDmt[byMtId / 8] |= 1 << ( byMtId % 8 );
}

inline void CSatConfInfo::DelCurrUpLoadMt( u8 byMtId)
{
	if( byMtId > MAXNUM_CONF_MT - 1  )
	{
		OspLog( LOGLVL_EXCEPTION, "Mt Id out of range %d", byMtId );
		return ;
	}
	
	m_byUpLoadCurrDmt[byMtId / 8] &= ~( 1 << ( byMtId % 8 ) );
}

inline u8 CSatConfInfo::IsMtInCurrUpLoad( u8 byMtId)
{
	if( byMtId > MAXNUM_CONF_MT - 1  )
	{
		OspLog( LOGLVL_EXCEPTION, "Mt Id out of range %d", byMtId );
		return FALSE;
	}
	return ( ( m_byUpLoadCurrDmt[byMtId / 8] & ( 1 << ( byMtId % 8 ) ) ) != 0 );
}



//��Ϣ��װ
typedef struct tagITSatMessageHeader
{
    enum{
        emMCUID = 0x0000,
            emNMSID = 0xFFF0,
            emBROADCAST = 0xFFFF
    };
    protected:
        u16 m_wEventId; //��ϢID
        u16 m_wMsgSN;   //��Ϣ��ˮ��(1-65535)
        u8  m_byConfIdx; //��������(0xFF ��ʾ�����еĻ���㲥��Ϣ)
        u16 m_wSrcId;   //��Ϣ������ID MCUӦ����0 ,��������ϵͳӦ���� 0xFFF0 ,�ն�����Ӧ��ID
        u16 m_wDstId;   //��Ϣ����Ŀ��ID MCUӦ����0 ,
        //��������ϵͳӦ���� 0xFFF0 ,�ն�����Ӧ��ID
        //0xFFFF ��ʾ�㲥��Ϣ 
        u8  m_byReserve;//�����̶ֹ�Ϊ��
    public:
        tagITSatMessageHeader()
        {
            m_wEventId = ntohs(0);
            m_wMsgSN   = ntohs(1);
            m_byConfIdx = 0xFF;
            m_wSrcId   = ntohs(emMCUID);
            m_wSrcId   = ntohs(emBROADCAST);
            m_byReserve = 0;
        }
        
        //��ȡ��ϢID
        u16 GetEventId()
        {
            return ntohs(m_wEventId);
        }
        //������ϢID
        void SetEventId( u16 wEventId )
        {
            m_wEventId = htons(wEventId);
        }
        //��ȡ��Ϣ��ˮ��
        u16 GetMsgSN()
        {
            return ntohs(m_wMsgSN );
        }
        //������Ϣ��ˮ��
        void SetMsgSN( u16 wMsgSN )
        {
            m_wMsgSN = ntohs(wMsgSN);
        }
        
        //��ȡ����������
        u8 GetConfIdx()
        {
            return m_byConfIdx;
        }
        //���û���������
        void SetConfIdx( u8 byConfIdx)
        {
            m_byConfIdx = byConfIdx;
        }
        
        //��ȡ��Ϣ����ԴID��
        u16 GetSrcId()
        {
            return ntohs(m_wSrcId);
        }
        //������Ϣ����ԴID��
        void SetSrcId( u16 wSrcId)
        {
            m_wSrcId = htons(wSrcId);
        }
        //��ȡ��Ϣ����Ŀ��ID��
        u16 GetDstId()
        {
            return ntohs(m_wDstId);
        }
        //������Ϣ����Ŀ��ID��
        void SetDstId( u16 wDstId)
        {
            m_wDstId = htons(wDstId);
        }
}PACKED ITSatMsgHdr;

//�ն���Ϣ
typedef struct tagTInterfaceMtTopo
{
	enum EMMtType
	{
		emFULL    = 1,//ȫ˫�� ֱ���ն�
			emUPALLOC = 2,//�ش�ͨ�����䣨�����նˣ�
	};
	enum EMModemType
	{
		emMODEM_70M_LBAND     = 1,            
			emMODEM_LBAND_70M     = 2,            
			emMODEM_LBAND_LBAND   = 3 
	};
	tagTInterfaceMtTopo(){memset(this,0,sizeof(tagTInterfaceMtTopo));}
protected:
	u16 m_wMtId;
	u8  m_byConnectType;
	char m_achAlias[MAXLEN_SATMTALIAS];
	u32 m_dwMtIp;
	u32  m_dwModemIp;
	u16  m_wModemPort;
	u8   m_byModemType;
	
	
	
public:
	void SetMtId( u16 wMtId )
	{
		m_wMtId = htons(wMtId);
	}
	u16 GetMtId(void)
	{
		return ntohs(m_wMtId);
	}
	
	void SetMtConnectType( u8 byMode )
	{
		m_byConnectType = byMode;
	}
	u8 GetMtConnectType(void)
	{
		return m_byConnectType;
	}
	
	void SetMtModemIp( u32 dwIp )
	{
		m_dwModemIp = dwIp;
	}
	u32 GetMtModemIp( void )
	{
		return m_dwModemIp;
	}
	
	void SetMtModemPort( u16 wPort )
	{
		m_wModemPort = htons(wPort);
	}
	u16 GetMtModemPort(void)
	{
		return ntohs(m_wModemPort);
	}
	
	void SetMtModemType( u8 byType )
	{
		m_byModemType = byType;
	}
	u8 GetMtModemType()
	{
		return m_byModemType;
	}
	
	void SetMtAlias( LPCSTR lpszAlias )
	{
		if( lpszAlias != NULL )
		{
			strncpy( m_achAlias, lpszAlias, sizeof( m_achAlias ) );
			m_achAlias[sizeof( m_achAlias ) - 1] = '\0';
		}
		else
		{
			memset( m_achAlias, 0, sizeof( m_achAlias ) );
		}
	}
	char* GetMtAlias()
	{
		return m_achAlias;
	}
	
	void SetMtIp( u32 dwIp )
	{
		m_dwMtIp = dwIp;
	}
	
	u32 GetMtIp()
	{
		return m_dwMtIp;
	}
}PACKED TInterfaceMtTopo ,ITSatMtInfo;


typedef struct tagTSatBitrate
{
    
    tagTSatBitrate(){memset(this,0,sizeof(tagTSatBitrate));	}
    protected:
        u8 m_byBitrate;
    public:
        //��ȡ���� (��λ:Kbps)
        u16 Get()
        {
            return m_byBitrate*32;
        }
        
        //�������� (��λ:Kbps )
        void Set( u16 wBitrate )
        {
            if( wBitrate >= 255*32 )
            {
                m_byBitrate = 255;
            }
            else
            {
                m_byBitrate = (u8)(wBitrate/32);
            }
        }
}PACKED ITSatBitrate;


typedef struct tagTInterfaceList
{
    tagTInterfaceList()
    {
        memset(this,0,sizeof(tagTInterfaceList));
    }
public:
    u8 m_abyMemberList[MAXNUM_SATMT/8];
public:
    void SetMember( u16 wWhoId )
    {
        if( wWhoId > MAXNUM_SATMT || wWhoId==0 )
        {
            OspPrintf( TRUE,FALSE, "Mt Id out of range %d", wWhoId );
            return ;
        }
        wWhoId -=1;
        m_abyMemberList[wWhoId / 8] |= 1 << ( wWhoId % 8 );
        return;
    }
    void RemoveMember( u16 wWhoId )
    {
        if( wWhoId > MAXNUM_SATMT || wWhoId==0 )
        {
            OspPrintf(TRUE,FALSE,"Mt Id out of range %d", wWhoId );
            return ;
        }
        wWhoId -=1;
        m_abyMemberList[wWhoId / 8] &= ~( 1 << ( wWhoId % 8 ) );
        return;
    }
    
    BOOL IsInList( u16 wWhoId ) const
    {
        if( wWhoId > MAXNUM_SATMT || wWhoId==0 )
        {
            OspPrintf(TRUE,FALSE,"Mt Id out of range %d", wWhoId );
            return FALSE;
        }
        wWhoId -=1;
        return ( ( m_abyMemberList[wWhoId / 8] & ( 1 << ( wWhoId % 8 ) ) ) != 0 );
    }
}PACKED TInterfaceList ,ITSatMtList;


typedef TInterfaceList TMtList;

//���廭�渴��
typedef struct tagTVMPParam
{
protected:
    u8      m_byVmpStyle;			//�ϳɷ��
    u8		m_byBrdcst;			//�Ƿ�㲥
    u16    	m_wMt[16];        		//��ԱID
public:
    inline u8 GetVmpStyle( void ) const
    {
        return( m_byVmpStyle );
    }
    
    inline void SetVmpStyle( u8 byStyle )
    {
        m_byVmpStyle = byStyle;
    }
    
    inline void SetVmpBrd( BOOL bBroad )
    {
        if ( bBroad == TRUE )
            m_byBrdcst = 1;
        else
            m_byBrdcst = 0;
    }
    
    inline BOOL IsVmpBrd( ) const
    {
        return ( m_byBrdcst == 1 );
    }
    
    inline void SetMtId( u8 byPos , u16 wMtId )
    {
        if( byPos >16 )
            return;
        m_wMt[ byPos-1] = htons( wMtId );
    }
    
    inline u16 GetMtId( u8 byPos ) const
    {
        if ( byPos == 0 || byPos > 16 )
            return 0;
        return ntohs( m_wMt[byPos-1] );
    }
    
}PACKED ITSatVmpParam;


typedef struct tagTInterfaceConfInfo
{
	tagTInterfaceConfInfo()
	{
		memset(this,0,sizeof(tagTInterfaceConfInfo));
	}

protected:
	u16 m_wSpeakerId;
	u16 m_wChairId;
	u8  m_byLineNum;
	u8  m_bySwitchTime;
	char m_achName[32];
	u8  m_byAudioType;
	u8  m_byVideoType;   //��ƵƵ����
	u8  m_byVidResolution;//��Ƶ�ֱ���
    ITSatBitrate m_tConfBitrate;
	/*--------------------------------*/
	u8  m_byState;//������
	/* 0x 0 0 0 0 0 x x x
		      |    | |_____����״̬
		      |    |_______���渴��״̬
		      |____________������Ϣ
	*/
	u8  m_achKey[9];
public:
	TMtList m_tNonActiveMt;//����
	TMtList m_tActiveMt;//ǩ��
	TMtList m_tCanSendMt;//�ɷ��ͣ��ϴ�(ֱ����ռ�ϴ�ͨ·)
	
	TMtList m_tNearMuteMt; //����
	TMtList m_tFarMuteMt; //����
	ITSatVmpParam m_tVmpMt; //���渴��

public:
	//��ȡ������Ƶ��ʽ
	u8 GetVideoType()
	{
		return m_byVideoType;
	}
	//���û�����Ƶ��ʽ
	void SetVideoType( u8 byType)
	{
		m_byVideoType = byType;
	}
	//��ȡ������Ƶ�ֱ���
	u8 GetResolution()
	{
		return m_byVidResolution;
	}
	//���û�����Ƶ�ֱ���
	void SetResolution( u8 byResolution)
	{
		m_byVidResolution = byResolution;
	}

	//���û�����Ƶ�ֱ���
	void SetAudioType( u8 byType )
	{
		m_byAudioType = byType;
	}

	//���û�����Ƶ�ֱ���
	u8 GetAudioType()
	{
		return m_byAudioType;
	}
	
	//���û������� (��λ64K)
	void SetBitrate( u16 wBitrate )
	{
		m_tConfBitrate.Set( wBitrate );
	}
	
	//��ȡ����������� (��λ64K)
	u16 GetBitrate()
	{
		return m_tConfBitrate.Get();
	}


	//������
	void SetSpeakerId( u16 wSpeakerId )
	{
		m_wSpeakerId = htons(wSpeakerId);
	}
	u16 GetSpeakerId(void)
	{
		return ntohs(m_wSpeakerId);
	}
	//��ϯ
	void SetChairId( u16 wChairId )
	{
		m_wChairId = htons(wChairId);
	}
	u16 GetChairId(void)
	{
		return ntohs(m_wChairId);
	}
	//����״̬
	void SetMixing(void)
	{
		m_byState |= 1;
	}
	void SetNoMixing(void)
	{
		m_byState &=~1;
	}
	BOOL IsMixing(void)
	{
		return ( (m_byState & 1 ) != 0 );
	}
	//���渴��״̬
	void SetVmping(void)
	{
		m_byState |= 0x2;
	}
	void SetNoVmping(void)
	{
		m_byState &= ~0x2;
	}
	BOOL IsVmping(void)
	{
		return ( (m_byState & 0x2 ) != 0 );
	}
	//���渴�Ϲ㲥״̬
/*	void SetVmpBrdst(void)
	{
		m_byState |= 0x4;
	}
	void SetVmpNoBrdst(void)
	{
		m_byState &= ~0x4;
	}
	BOOL IsVmpBrdst(void)
	{
		return ( (m_byState & 0x4 ) != 0 );
	}*/
	/*�����Ƿ����,�ն˴�������Ҫ��������
	*/
	void SetConfEncrypted( BOOL bEncrypt )
	{
		if ( bEncrypt )
			m_byState |= 0x08;
		else
			m_byState &= ~0x08;
	}
	
	BOOL IsConfEncrypted()
	{
		return ( (m_byState & 0x08 ) != 0 );
	}
	/*�������룬�ն˴�������ʱ��д*/
	void SetConfKey( u8* buf, u8 len)
	{
		memcpy( m_achKey,buf,len);
		m_achKey[8] = '\0';
	}
	u8* GetConfKey( )
	{
		return m_achKey;
	}
	//������·
	void SetConfLineNum( u8 byNum )
	{
		m_byLineNum = byNum;
	}
	u8 GetConfLineNum(void) const
	{
		return m_byLineNum;
	}
	//�л����
	void SetConfSwitchTime( u8 byTime )
	{
		m_bySwitchTime = byTime;	
	}
	u8 GetConfSwitchTime(void) const
	{
		return m_bySwitchTime;
	}
	LPCSTR GetConfName( void ) const
	{
		return( ( LPCSTR )m_achName );
	}

	void SetConfName( LPCSTR lpszBuf )
	{
		strncpy( m_achName, lpszBuf, sizeof( m_achName ) );
		m_achName[ sizeof( m_achName ) - 1] = '\0';
	}
	
}PACKED TInterfaceConfInfo ,ITSatConfInfo;


/*====================================================================
    ������      : TSchConfInfoHeader
    ����        ��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/11/26    1.0         zhangsh           ����
====================================================================*/
struct TSchConfInfoHeader
{
protected:
	u16 wMessageType;   //������
	u32 dwSerialId;     //��ˮ��
	u8  chConfId[16];   //�����
	u8  chPass[12];     //�û���
	u32 dwStartTime;    //��ʼʱ��
	u32 dwEndTime;      //����ʱ��
	u16 wMtNum;         //�λ��ն˸���
public:
	void SetMessageType( u16 wCmd )
	{
		wMessageType = htons( wCmd );
	}

	u16 GetMessageType( )
	{
		return ntohs( wMessageType );
	}
	
	void SetSerialId( u32 dwSN )
	{
		dwSerialId = htonl( dwSN );
	}
	u32 GetSerialId( )
	{
		return ntohl( dwSerialId );
	}
	void SetStartTime( u32 dwTime )
	{
		dwStartTime = htonl( dwTime );
	}
	
	void SetEndTime( u32 dwTime )
	{
		dwEndTime = htonl( dwTime );
	}
	
	void SetMtNum( u16 wMt )
	{
		wMtNum = htons( wMt );
	}
	void SetConfId( CConfId cConfId )
	{
		u8 byConf[16];
		memset( byConf, 0 , 16 );
		cConfId.GetConfId( byConf, 16 );
		
        for ( u8 byLoop = 0; byLoop < 16 ; byLoop ++ )
        {
			chConfId[byLoop] = byConf[byLoop];
        }
	}
	void SetPassWord( u8* pPassWord )
	{
		if ( pPassWord == NULL )
        {
			return;
        }
		memcpy( chPass, pPassWord, 12 );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//ԤԼ������Ϣ(NMS->MCU)
struct TReSchConfNotif
{
	u16 wMessageType;   //������
	u32 dwSerialId;     //��ˮ��
	u8  chConfId[16];
	u16 wMtNum;
public:
	u16 GetMessageType( )
	{
		return ntohs( wMessageType );
	}
	
	u32 GetSerialId( )
	{
		return ntohl( dwSerialId );
	}
	
	u16 GetMtNum( )
	{
		return ntohs( wMtNum );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
struct TSchConfNotif
{
protected:
	u16 wMessageType;   //������
	u32 dwSerialId;     //��ˮ��
	u8  chConfId[16];
	u8  byMcsCreated;   //�Ƿ�������
	u16 wMtNum;
public:
	u16 GetMessageType( )
	{
		return ntohs( wMessageType );
	}
	
	u32 GetSerialId( )
	{
		return ntohl( dwSerialId );
	}
	
	u16 GetMtNum( )
	{
		return ntohs( wMtNum );
	}
	u8 GetMcs()
	{
		return byMcsCreated;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
//�ն���Ӻ�ɾ��(MCU->NMS)
struct TMtOperateToNms
{
protected:
	u16 wMessageType;   //������
	u32 dwSerialId;     //��ˮ��
	u8  chConfId[16];
	u16 wMtNum;
public:
	void SetMessageType( u16 wCmd )
	{
		wMessageType = htons( wCmd );
	}
	void SetSerialId( u32 dwSN )
	{
		dwSerialId = htonl( dwSN );
	}
	void SetMtNum( u16 wMt )
	{
		wMtNum = htons( wMt );
	}
	void SetConfId( CConfId cConfId )
	{
		u8 byConf[16];
		memset( byConf, 0 , 16 );
		cConfId.GetConfId( byConf, 16 );
		for ( u8 byLoop = 0; byLoop < 16 ; byLoop ++ )
			chConfId[byLoop] = byConf[byLoop];
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//�ն���Ӻ�ɾ����֪ͨ(NMS->MCU)
struct TMtOperateNotif
{
protected:
	u16 wMessageType;   //������
	u32 dwSerialId;     //��ˮ��
	u8  chConfId[16];
	u16 wMtNum;
public:
	u16 GetMessageType( )
	{
		return ntohs( wMessageType );
	}
	u32 GetSerialId( )
	{
		return ntohl( dwSerialId );
	}
	u32 GetMtNum()
	{
		return ntohs(wMtNum);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

typedef struct tagTSatMtId
{
protected:
    u16 m_wMtId;
public:
    
    void Set( u16 wMtId )
    {
        m_wMtId = htons( wMtId );
    }
    u16 Get(void)
    {
        return ntohs( m_wMtId );
    }
}PACKED ITSatMtId;



typedef struct tagTInterfaceMtAddr
{
    tagTInterfaceMtAddr(){memset(this,0,sizeof(tagTInterfaceMtAddr));}
protected:
    u32 m_dwIpAddr;
    u16 m_wPort;
public:
    void SetIpAddr( u32 dwIp )
    {
        m_dwIpAddr =  htonl(dwIp);
    }
    u32 GetIpAddr(void)
    {
        return ntohl(m_dwIpAddr);
    }
    
    void SetPort( u16 wPort )
    {
        m_wPort = htons( wPort );
    }
    u16 GetPort(void)
    {
        return ntohs( m_wPort );
    }
}PACKED TInterfaceMtAddr,ITSatTransAddr;


//���ǻ����ַ��Ϣ
typedef struct tagTSatConfAddrInfo
{
    tagTSatConfAddrInfo(){ memset( this,0,sizeof(tagTSatConfAddrInfo));}
public:
    ITSatMtId      tAudMulticastSrcId;// ��Ƶ�鲥������ƵԴID
    ITSatMtId      tVidMulticastSrcId;// ��Ƶ�鲥������ƵԴID
    ITSatMtId      tAudUnitcastSrcId; // ��Ƶ����������ƵԴID
    ITSatMtId      tVidUnitcastSrcId; // ��Ƶ����������ƵԴID
    
    ITSatTransAddr tMcuRcvMediaAddr;     // MCU��������(������)��ַ ,��Ƶ��ʼ�˿ں�
    ITSatTransAddr tAudMulticastAddr;    // ��Ƶ�鲥��ַ��Ϣ 
    ITSatTransAddr tVidMulticastAddr;    // ��Ƶ�鲥��ַ��Ϣ
	
	//ITSatTransAddr tSecVidMulticastAddr;    // ��Ƶ�ڶ��鲥��ַ��Ϣ

    ITSatTransAddr tAudUnitcastAddr;     // ��Ƶ������ַ��Ϣ-�õ�ַ�����˿ں���Ч
    ITSatTransAddr tVidUnitcastAddr;     // ��Ƶ������ַ��Ϣ-�õ�ַ�����˿ں���Ч
    
    ITSatMtList    tAudMulticastMmbList; // ��Ƶ�鲥���ճ�Ա�б� 
    ITSatMtList    tVidMulticastMmbList; // ��Ƶ�鲥���ճ�Ա�б� 
	
	//ITSatMtList    tSecVidMulticastMmbList; // ��Ƶ�ڶ��鲥���ճ�Ա�б� 

    ITSatMtList    tAudUnitcastMmbList;  // ��Ƶ�������ճ�Ա�б� 
    ITSatMtList    tVidUnitcastMmbList;  // ��Ƶ�������ճ�Ա�б�
}PACKED ITSatConfAddrInfo;


//���ǻ����ַ��Ϣ: �����ն�ʹ��
typedef struct tagTSatConfAddrInfoHd
{
    tagTSatConfAddrInfoHd(){ memset( this,0,sizeof(tagTSatConfAddrInfoHd));}

public:
    ITSatMtId      tAudMulticastSrcId;	// ��Ƶ�鲥������ƵԴID
    ITSatMtId      tVidMulticastSrcId;	// ��Ƶ�鲥������ƵԴID

    ITSatMtId      tAudUnitcastSrcId;	// ��Ƶ����������ƵԴID
    ITSatMtId      tVidUnitcastSrcId;	// ��Ƶ����������ƵԴID
    
    ITSatTransAddr tMcuRcvMediaAddr;    // MCU��������(������)��ַ ,��Ƶ��ʼ�˿ں�

    ITSatTransAddr tAudMulticastAddr;   // ��Ƶ�鲥��ַ��Ϣ 
    ITSatTransAddr tVidMulticastAddr;   // ��Ƶ�鲥��ַ��Ϣ
	
    ITSatTransAddr tAudUnitcastAddr;    // ��Ƶ������ַ��Ϣ-�õ�ַ�����˿ں���Ч
    ITSatTransAddr tVidUnitcastAddr;    // ��Ƶ������ַ��Ϣ-�õ�ַ�����˿ں���Ч
    
    ITSatMtList    tAudMulticastMmbList;// ��Ƶ�鲥���ճ�Ա�б� 
    ITSatMtList    tVidMulticastMmbList;// ��Ƶ�鲥���ճ�Ա�б� 
	
    ITSatMtList    tAudUnitcastMmbList; // ��Ƶ�������ճ�Ա�б� 
    ITSatMtList    tVidUnitcastMmbList; // ��Ƶ�������ճ�Ա�б�


	//˫��֧�֣��ݲ�����˫����������˫��ѡ����
	ITSatMtId      tDSMulticastSrcId;	// ˫���鲥������ƵԴID
	ITSatTransAddr tDSMulticastAddr;	// ˫���鲥��ַ��Ϣ
	ITSatMtList    tDSMulticastMmbList; // ˫���鲥���ճ�Ա�б� 
}PACKED ITSatConfAddrInfoHd;


typedef struct tagTInterfaceMtKey
{
	tagTInterfaceMtKey()
	{
		memset(this,0,sizeof(tagTInterfaceMtKey));
	}
protected:
	u16 m_wMtId;
	char szDesKey[16];
public:
	void SetMtKey( u16 MtId,  char* key )
	{
		m_wMtId = htons( MtId );
		for ( u8 byLoop = 0 ; byLoop < 16 ; byLoop ++)
			szDesKey[byLoop] = key[byLoop];
	}
	//���ܲ�������������ֱ�ӿ���16���ֽ�
	char* GetMtKey()
	{
		return szDesKey;
	}
	//��ȡ��Կ��MT ID
	u16 GetMtId()
	{
		return ntohs(m_wMtId);
	}
	//������Կ��MT ID
	void SetMtId( u16 wMtId )
	{
		m_wMtId = ntohs(wMtId);
	}
}PACKED TInterfaceMtKey,ITSatMtKey;


//�������Ϣ�ṹ
typedef struct tagTMultiPackHeader
{
protected:
    u8          m_byTotalNum;		//�����ܸ���
    u8        m_byCurrNum;	//��ǰ�������
    u8        m_byContentNum;   //��ǰ�����ݸ���
public:
	inline u8 GetTotalNum( void ) const
	{
		return m_byTotalNum;
	}
	
	inline void SetTotalNum( u8 byNum )
	{
		m_byTotalNum = byNum;
	}
	
	inline u8 GetCurrNum( void ) const
	{
		return m_byCurrNum;
	}
	
	inline void SetCurrNum( u8 byNum )
	{
		m_byCurrNum = byNum;
	}
	
	inline u8 GetContentNum( void ) const
	{
		return m_byContentNum;
	}
	
	inline void SetContentNum( u8 byNum )
	{
		m_byContentNum = byNum;
	}
}PACKED ITSatMultiPackHeader;

#ifdef WIN32
#pragma pack( pop )
#endif


#endif

//END OF FILE

