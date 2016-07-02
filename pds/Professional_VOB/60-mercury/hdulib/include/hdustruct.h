/*****************************************************************************
   ģ����      : ������뿨
   �ļ���      : hdustruct.h
   ����ʱ��    : 2008.12.03
   ʵ�ֹ���    : HDU�궨���ͨ���ṹ����ͷ�ļ�
   ����        : ���ֱ�
   �汾        : 
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2008/12/03  4.6         ���ֱ�       ����
******************************************************************************/ 
#ifndef _HDUSTRUCT_H_
#define _HDUSTRUCT_H_

#define CHECK_IFRAME_INTERVAL       (u16)100       // ���ؼ�֡ʱ����(ms)
#define DBG_PRINT                   printf
#define FIRST_REGISTERACK           (u8)1           // ��һ���յ�ע��Ack
#define HDU_CONNETC_TIMEOUT         (u16)3*1000     // ���ӳ�ʱֵ3s
#define HDU_REGISTER_TIMEOUT        (u16)5*1000     // ע�ᳬʱֵ5s

struct TChannelInfo
{
public:
    TChannelInfo()
    {
        m_dwLastFUPTick = 0;
        m_dwLastFUPTick = 0;
		m_tHduChnStatus.SetVolume( HDU_VOLUME_DEFAULT );
		m_tHduChnStatus.SetIsMute( FALSE );
		m_tHduChnStatus.SetSchemeIdx( 0 );
		m_tHduChnStatus.SetStatus( 0 );
		m_tHduChnStatus.SetNull();
		m_tHduChnStatus.SetChnIdx( 0 );
	}
public:
    CConfId         m_cChnConfId;
    u32             m_dwLastFUPTick;    //����յ��ؼ�֡�����Tick��

	THduChnStatus   m_tHduChnStatus;    //ͨ��״̬
};

#endif //_HDUSTRUCT_H_

