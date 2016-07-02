#ifndef DHCP_EVENT_H
#define DHCP_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmDHCP
{
#endif 

  _ev_segment( dhcp  )

/***********************<< DHCP ����ʧ��  >>************************
 *[��Ϣ��]			 
 *  + BOOL  TRUE = ע��ɹ� FALSEʧ��
 *
 *[��Ϣ����]
 *
 *    MtService => UI
 *
 */_event(  ev_DHCPIpDown   )
  _body(u8,1 )/*ȡ��/ʧȥIP��ַ�����ں�*/
  _body(u32_ip,1 )/*ȡ��/ʧȥ��IP��ַ�������ֽ���*/
  _body(u32_ip,1 )/*ȡ��/ʧȥ��IP��ַ�������룬�����ֽ���*/
  _body(u32_ip,1 )/*ȡ��/ʧȥ��Ĭ�����أ������ֽ����п���Ϊ0����Ϊ�е�DHCP-Server��һ��Ϊ�豸ָ��Ĭ������*/
   _ev_end


/***********************<< DHCP IP����  >>************************
 *[��Ϣ��]			 
 *[��Ϣ����]
 *    HDCP server        => MtService 
 *
 */_event(  ev_DHCPOverdue   )
   _ev_end


/***********************<< ��ȡDHCP IP  >>************************
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *    UI | MTC       => MtService 
 */_event(  ev_GetDHCPIPCmd   )
   _ev_end
   
/***********************<< �ر�DHCP ��������  >>************************
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *    UI | MTC       => MtService 
 */_event(  ev_ShutdownDHCPIPCmd   )
   _ev_end

/***********************<< �ر�DHCP ����ָʾ  >>************************
 *[��Ϣ��]			 
 *  BOOL  �Ƿ�رճɹ�
 *[��Ϣ����]
 *    UI | MTC       => MtService 
 */_event(  ev_ShutdownDHCPIPInd   )
   _body(BOOL,1)
   _ev_end

/***********************<< DHCP��ʱ  >>************************
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *    MtService         => MtService 
 *
 */_event(  ev_DHCPTimesout   )
   _ev_end
   
  /***********************<< ����DHCP  >>************************
 *[��Ϣ��]			 
 * BOOL �Ƿ�����DHCP
 *[��Ϣ����]
 *
 *    UI | MTC       => MtService 
 *
 */_event(  ev_CfgDHCPIPCmd   )
	_body(BOOL , 1)
   _ev_end

  /***********************<< ����DHCPָʾ  >>************************
 *[��Ϣ��]			 
 *  ��	
 *[��Ϣ����]
 *
 *    MtService => UI | MTC
 *
 */_event(  ev_CfgDHCPIPInd   )
   _body(BOOL , 1)
   _ev_end
   
  /***********************<< ֪ͨ�û�DHCP IP��Ϣ  >>************************
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *    MtService => UI | MTC
 *
 */_event(  ev_NotifyDHCPInfoInd   ) 
  _body(u32_ip,1 )/*ȡ��/ʧȥ��IP��ַ�������ֽ���*/
  _body(u32_ip,1 )/*ȡ��/ʧȥ��IP��ַ�������룬�����ֽ���*/
  _body(u32_ip,1 )/*ȡ��/ʧȥ��Ĭ�����أ������ֽ����п���Ϊ0����Ϊ�е�DHCP-Server��һ��Ϊ�豸ָ��Ĭ������*/
  _ev_end   
  /***********************<< ֪ͨServeci DHCP IP����ɹ�  >>************************
 *[��Ϣ��]			 
 * u32_ip  DHCP IP ��ַ
 * u32_ip  DHCP ���ص�ַ
 *[��Ϣ����]
 *
 *    HDCP server        => MtService 
 *
 */_event(  ev_NewDHCPIpInd   )
  _body(u8,1 )/*ȡ��/ʧȥIP��ַ�����ں�*/
  _body(u32_ip,1 )/*ȡ��/ʧȥ��IP��ַ�������ֽ���*/
  _body(u32_ip,1 )/*ȡ��/ʧȥ��IP��ַ�������룬�����ֽ���*/
  _body(u32_ip,1 )/*ȡ��/ʧȥ��Ĭ�����أ������ֽ����п���Ϊ0����Ϊ�е�DHCP-Server��һ��Ϊ�豸ָ��Ĭ������*/
  _ev_end     
   
 /***********************<< ֪ͨService DHCP IP����ʧ��  >>************************
 *[��Ϣ��]			 
 * u32_ip  DHCP IP ��ַ
 * u32_ip  DHCP ���ص�ַ
 *[��Ϣ����]
 *
 *    HDCP server        => MtService 
 *
 */_event(  ev_RenewDHCPIpFailInd   )
  _body(u8,1 )/*ʧȥIP��ַ�����ں�*/
  _body(u32_ip,1 )/*ʧȥ��IP��ַ�������ֽ���*/
  _body(u32_ip,1 )/*ʧȥ��IP��ַ�������룬�����ֽ���*/
  _body(u32_ip,1 )/*ʧȥ��Ĭ�����أ������ֽ����п���Ϊ0����Ϊ�е�DHCP-Server��һ��Ϊ�豸ָ��Ĭ������*/
  _ev_end     

#ifndef _MakeMtEventDescription_
   _ev_segment_end(dhcp)
};
#endif
#endif

