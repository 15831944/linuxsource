/*==============================================================================
����    :  CEapuTestClient
����    :  EAPU�Զ����Կͻ�����
��Ҫ�ӿ�:  
��ע    :  
-------------------------------------------------------------------------------
�޸ļ�¼:  
��  ��     �汾          �޸���          �߶���          �޸ļ�¼						
==============================================================================*/
#ifdef _LINUX_
#include "nipwrapper.h"
#include "nipwrapperdef.h"
#include "brdwrapperdef.h"
#endif

#ifdef _LINUX12_
#include "brdwrapper.h"
#endif

#include "eapuautotest.h"
#include "kdvmixer_eapu.h"
#include "ummessagestruct.h"
#include "mcustruct.h"
#include "kdvmedianet.h"
#include "mcuver.h"
#define MAX_AUDIO_FRAME_SIZE            (u32)8 * 1024       //���ܴ�С
#define EV_C_BASE                       60000

#define EV_C_CONNECT_TIMER                 EV_C_BASE + 1    // ����
#define EV_C_REGISTER_TIMER				   EV_C_BASE + 2
#define EV_C_INIT                          EV_C_BASE + 3    // ��ʼ��

#define EV_C_CHANGEAUTOTEST_CMD            EV_C_BASE + 6    // �ı��Զ����Ա�־

#define MIN_BITRATE_OUTPUT              (u16)128
#define EAPU_CONNETC_TIMEOUT            (u16)3*1000     // ���ӳ�ʱֵ3s
#define EAPU_REGISTER_TIMEOUT           (u16)5*1000     // ע�ᳬʱֵ5s
#define PORT_SNDBIND                    20000

//#define ERR_EAPU_ERRDSPID                  (u16)1002
/*EV_EAPUAUTOTEST_BGN + 1       //���������������ע�����󡣰����û������û�������Ϣ���Ա�������˽��������֤��
TEST_S_C_LOGIN_ACK      ������������ע�������ȷ�ϡ�
TEST_S_C_LOGIN_NACK     ������������ע������ľܾ��������ܾ�ԭ��
TEST_S_C_START_REG      ������Ҫ�����迪�����ԣ�������Ҫ����Ե�DSP�ţ����������ţ����Լ���Ƶ��ʽ����Ϣ��
TEST_C_S_START_ACK     ����Է������Ŀ�ʼ���������ȷ�ϡ�
TEST_C_S_START_NACK    ����Է������Ŀ�ʼ��������ľܾ��������ܾ�ԭ��
TEST_S_C_STOP_REG       ������Ҫ������ֹͣ��ǰ���ԡ�
TEST_C_S_STOP_ACK       ����Է�������ֹͣ��ǰ���������ȷ�ϡ�
TEST_C_S_STOP_NACK     ����Է�������ֹͣ��ǰ��������ľܾ��������ܾ�ԭ��*/

//const s8 achServIpAddr[] = "10.1.1.1";
const s8 achServIpAddr[] = "172.16.160.10";

#define MAX_SERIAL_LENGTH	    32
#define MAX_MAC_LENGTH		    32
#define MAX_VERSION_LENGTH	    32
class CDeviceInfo
{
private:
    s8          achSerial[MAX_SERIAL_LENGTH];           //���к�
    s8          achMac[MAX_MAC_LENGTH];                 //mac��ַ
    s8          achSoftVersion[MAX_VERSION_LENGTH];     //����汾
    s8          achHardVersion[MAX_VERSION_LENGTH];     //Ӳ���汾
    u32         dwIp;                                   //�豸ip
    u32         dwSubMask;                              //��������
public:
    u32 getSubMask()
    {
        return dwSubMask;
    }
	
    void setSubMask(u32 dwMask)
    {
        dwSubMask = dwMask;
    }
	
    void setSerial(s8 * buf)
    {
        if(buf == NULL) return;
        memset(achSerial,0,MAX_SERIAL_LENGTH);
        int length = (strlen(buf) >= MAX_SERIAL_LENGTH -1 ? MAX_SERIAL_LENGTH - 1: strlen(buf));
        memcpy(achSerial,buf,length);
        achSerial[length] = '\0';
    }    
    s8* getSerial()
    {
        return achSerial;
    }
    
    void setIp(u32 IP)
    {
        dwIp = IP;
    }
    
    u32 getIp()
    {
        return dwIp;
    }
	
    s8* getMac()
    {
        return achMac;
    } 
    void setMac(s8* pchbuf)
    {
        if(pchbuf == NULL) return;
        memset(achMac,0,MAX_VERSION_LENGTH);
        int length = (strlen(pchbuf) >= MAX_MAC_LENGTH -1 ? MAX_MAC_LENGTH - 1: strlen(pchbuf));
        memcpy(achMac, pchbuf, length);
    }
	
    void setSoftVersion(s8* buf)
    {
        if(buf == NULL) return;
        memset(achSoftVersion, 0, MAX_VERSION_LENGTH);
        int length = (strlen(buf) >= MAX_VERSION_LENGTH -1 ? MAX_VERSION_LENGTH - 1 : strlen(buf));
        memcpy(achSoftVersion, buf, length);
        achSoftVersion[length] = '\0';
    }
    s8* getSoftVersion()
    {
        return achSoftVersion;
    }
    void setHardVersion(s8 * buf)
    {
        if(buf == NULL) return;
        memset(achHardVersion, 0, MAX_VERSION_LENGTH);
        int length = (strlen(buf) >= MAX_VERSION_LENGTH -1 ? MAX_VERSION_LENGTH - 1 : strlen(buf));
        memcpy(achHardVersion, buf, length);
        achHardVersion[length] = '\0';
    }    
    s8* getHardVersion()
    {
        return achHardVersion;
    }
};

class CEapuMixTestInst;
class CRcvTestCallBack
{
public:
    CEapuMixTestInst* m_pcMixerInst;    //�ص��Ļ�����
	u32           m_byChnnlId;      //�ص���ͨ���� 
};
class CEapuMixTestInst : public CInstance
{
public:
    CEapuMixTestInst();
    virtual ~CEapuMixTestInst();

	void DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp );
    void InstanceEntry( CMessage* const pMsg );
	void Init();                             //Ӳ����ʼ��
    BOOL ConnectServ();												//connect server 
	void ProcStartTestReq(CMessage* const pMsg);
	void ProcStopTestReq(CMessage* const pMsg);
	void RegisterSrv();
	void ProcRegAck( CMessage * const pcMsg ); 
	void ProDisconnect( CMessage * const pcMsg );
	void RestoreDefault(void);
	void DaemonProcChangeAutoTestCmd( CMessage* const pMsg );		//�ı���������flag
private:
	
    //�����豸��Ϣ
	BOOL32 SendMsgToMcu( u16 wEvent, CServMsg const &cServMsg );    //������MCU������Ϣ
    void SetEapuDeviceInfo(CDeviceInfo &cDeviceInfo);
public:
	CKdvMixer m_cMixer;  
	u8               m_byAudType;
	CRcvTestCallBack m_acRcvCB[MAXNUM_MIXER_CHNNL];
	CKdvMediaRcv*    m_pcAudRcv[MAXNUM_MIXER_CHNNL];                             //ͨ����Ӧý���������
	//BOOL32           m_bIsChnUsed[MAXNUM_MIXER_CHNNL];                           //��ͨ���Ƿ�ʹ��
	CKdvMediaSnd*    m_pcAudSnd[MAXNUM_MIXER_CHNNL + 2 * MAXNUM_MIXER_AUDTYPE]; 
private:
	u32  m_dwEapuTestDstNode;
	u32  m_dwEapuTestDstInst;
	BOOL m_bConnected;
	u32  m_dwMcuRcvIp;
	u8   m_byCurTestDspId;
};
typedef zTemplate< CEapuMixTestInst, 1 > CEapuMixerTestApp;
extern CEapuMixerTestApp g_cEapuMixerTestApp;
