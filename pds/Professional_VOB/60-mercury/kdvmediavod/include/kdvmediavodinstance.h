#ifndef _KDVMEDIAVODINSTANCE_H_
#define _KDVMEDIAVODINSTANCE_H_

#include "kdvmediavod.h"
#include "asflib.h"
#define  KDVCALLBACK

#define CONFIG_FILE_NAME        "./config.ini"



API s32 vodstart();
API void vodend();

void VodLog(s32 nLevel, s8 * pchFormat,...);

#define LOG_EXP			        (s32)0		//�쳣
#define LOG_IMT			        (s32)1		//��Ҫ��־
#define LOG_DEBUG		        (s32)2		//һ��������Ϣ
#define LOG_ALL                 (s32)3      //���е�����Ϣ 

#define INVALID_PALYID          (u32)-1


#define VOD_STATE_LOGOUT            0
#define VOD_STATE_LOGIN             1
#define VOD_STATE_PLAYSTART         2
#define VOD_STATE_PLAYPAUSE         3
#define VOD_STATE_DISCONECT         4

//����
OSPEVENT(MEDIAVOD_QUIT, MEDIAVOD_MESSAGE_START + 34);

typedef struct 
{
    u16             m_wVodListenPort;

    u32             m_dwLocalIP;
    u16             m_wSendPort;

	u16				m_wMTUSize; // MTU��С, zgc, 2007-04-02

    s8              m_achRootDir[MEDIAVOD_MAX_PATHFILENAME];
} TVodConfig;


typedef struct 
{
    u32 m_dwNodeId;
    u32 m_dwClientId;
    time_t m_LoginTime;
    
    s8  m_achUserName[MEDIAVOD_MAX_USERNAME];      //�û���
    s8  m_achPassword[MEDIAVOD_MAX_PASSWORD];      //�û�����
} TVodClientInfo;


typedef struct tagVodPlayInfo
{

    s8  m_achFileName[MEDIAVOD_MAX_PATHFILENAME];    //����·�����ļ���
    u8  m_byStart;             //�Ƿ�������ʼ����
    u16 m_wAudioPort;          //0��ʾ��ʹ��
    u16 m_wVideo1Port;         //0��ʾ��ʹ��
    u16 m_wVideo2Port;         //0��ʾ��ʹ��
    u32 m_dwIP;                //���ŵĵ�ַ

    u8  m_byAudioType;          //��Ƶ�� 255��ʾû�и�·����
    u8  m_byVideo1Type;         //��Ƶ1��255��ʾû�и�·����
    u8  m_byVideo2Type;         //��Ƶ2��255��ʾû�и�·����
} TPlayInfo;


extern TVodConfig g_tVodConfig;

class Ckdvmediavodinstance : public CInstance  
{
public:
	u16 GetFileInfo(s8* pszFileName, TVodFileInfoAck* ptVodFileInfoAck, u8 &byChnlNum, u32 &dwSamplesPerSecond);
	u16 AppiontPlay(TVodPlayGotoReq* ptVodPlayGotoReq);
	u16 StartAgain();
	u16 PausePlay();
	u16 StopPlay();
	u16 StartPlay(TVodPlayReq*);
	u16 PlayInfo(TVodPlayInfoAck* tVodPlayInfoAck);
	Ckdvmediavodinstance();
	virtual ~Ckdvmediavodinstance(){};
	void DaemonInstanceEntry(CMessage* const pcMsg, CApp* pcApp);
	//�¼�����
	void InstanceEntry(CMessage * const pcMsg);
    void InstanceDump(u32 dwParam);

    void InstClear();

    void OnPowerOn(CMessage * const pcMsg);
    void OnPowerOff(CMessage * const pcMsg);
    void DaemonQuit();

    void OnClientReg(CMessage * const pcMsg, CApp* pcApp);

    //���¼�����
    void OnClientUp(CMessage * const pcMsg);

	void OnDisconnect(CMessage * const pcMsg);

    void OnFileListReq(CMessage * const pcMsg);

    void OnFileInfoReq(CMessage * const pcMsg);

    void OnPlayReq(CMessage* const pcMsg);

    void OnStopReq(CMessage * const pcMsg);

	void OnPauseReq(CMessage * const pcMsg);

    void OnResumeReq(CMessage * const pcMsg);

    void OnPalyGotoReq(CMessage * const pcMsg);

    void OnPlayInfoReq(CMessage * const pcMsg);

    void OnUnRegReq(CMessage * const pcMsg);

    void OnNotifyPlayInfo(CMessage * const pcMsg);
    
    u16 IsAutherUser(s8* pszUserName, s8* pszPassword);
private:
    TVodClientInfo m_tClientInfo;

    //u32 m_dwClientId;   //�ͻ��˽ڵ�Id
    //u32 m_dwClientNode;//�ͻ��˽ڵ��
	//u8  m_achClientUserName[MEDIAVOD_MAX_USERNAME];//�ͻ����û���
	//u8  m_byState;  //����״̬�����粥�ţ���ͣ��
    //u8  m_instanceState;//BUSY OR IDLE
    u16 m_wPlayCheckValue;  //play�ļ��ֵ�����ڻص�ʱ�˶ԣ�

	/* [2013/5/10 zhangli]�鿴���ļ���Ϣ���˳�ʱ����������ԭ�����˳�ʱ������OspQuit��CKdvASFFileReader����������
	OspFreeMem�Ĳ�������ϵͳ�Զ�����CKdvASFFileReaderʱ��Ϊosp�Ѿ�quite���±����������ΪCKdvASFFileReader *���˳�ʱ
	ͨ��delete m_pcKdvASFFileReader�Լ���������*/
	CKdvASFFileReader *m_pcKdvASFFileReader;
public:
    u32 m_dwPlayID;
    TPlayInfo m_tPlayInfo;
};

typedef zTemplate< Ckdvmediavodinstance, MEDIAVOD_MAX_CLIENT> CMediavodAPP;
extern CMediavodAPP g_cMediaVodApp;	
extern SEMHANDLE g_hSemQuit;


typedef struct
{
    u16 wCheckValue;
    u8  byRate;
} TPlayRateMsg;

s32 MediaVodInit(TVodConfig* ptVodConfig);


BOOL32 IsValidPlayFile(s8* pszFileName);
BOOL32 IsVideoType(u8 byMediaType);
BOOL32 isAsfFile(const s8 *pFileNme);

#endif // _KDVMEDIAVODINSTANCE_H_