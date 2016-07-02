#ifndef AUTOUPGRADE_EVENT_H
#define AUTOUPGRADE_EVENT_H

#include "eventcomm.h"

#ifndef _MakeMtEventDescription_

enum EmAutoUpgrade
{
#endif
/***********************************************************/
/*                                                         */
/*                 �Զ�������Ϣ                            */
/*                                                         */
/***********************************************************/

    _ev_segment( autoupgrade )
 
/***********************<< ���ӷ��������� >>********************	
 *[��Ϣ��]			 
 *   +��          //�ն���Ϣ
 *   +u8  bySrc  //ָʾ��ǰ��������Ϣ����mtc,����osd, xjx_080111
 *        bySrc = 1, ����osd; bySrc = 2; ����mtc
 *   
 *[��Ϣ����]
 *   ui  => mtservice 
 */_event(  ev_UpgradeConnectSrvCmd   )
  _body( u8 , 1 )
   _ev_end
   
/***********************<< ȡ�����ӷ��������� >>********************	
 *[��Ϣ��]			 
 *   +��         //�ն���Ϣ
 *   
 *[��Ϣ����]
 *    ui => mtservice
 */_event(  ev_UpgradeDisconnectSrvCmd   )
 _ev_end

 
/***********************<< ���ӷ�����״ָ̬ʾ >>********************	
 *[��Ϣ��]			 
 *   +BOOL           //����״̬,TRUE:�ɹ���FALSE:ʧ��
 *   
 *[��Ϣ����]
 *    ui <=  mtservice
 */_event(  ev_UpgradeConnectSrvStatusInd   )
   _body( BOOL, 1 )
   _ev_end

/***********************<< ���������������°汾��Ϣָʾ >>********************	
 *[��Ϣ��]			 
 *  TVerInfo   //���µİ汾��Ϣ�ṹ         
 *   
 *[��Ϣ����]
 *    ui <=  mtservice
 */_event(  ev_UpgradeNewVersionInfoInd   )
   _body( TVerInfo, 1 )
   _ev_end
   

/***********************<< ��ʼ�������� >>********************	
 *[��Ϣ��]			 
 *           
 *   
 *[��Ϣ����]
 *    ui =>  mtservice
 */_event(  ev_UpgradeStartupgradeCmd   )
   _body( u8, 1 )
   _ev_end


/***********************<< ȡ���������� >>********************	
 *[��Ϣ��]			 
 *           
 *   
 *[��Ϣ����]
 *    ui =>  mtservice
 */_event(  ev_UpgradeCancelupgradeCmd   )
   _ev_end


/***********************<< �ļ�����״ָ̬ʾ >>********************	
 *[��Ϣ��]	
 * +BOOL     //TRUE=�ļ����سɹ���FALSE=����ʧ��  
 * +u16      //�ܰ�
 * +u16      //��ǰ�����
 *[��Ϣ����]
 *    ui =>  mtservice
 */_event(  ev_UpgradeFileDownloadInfoInd   )
  _body( BOOL, 1 )
  _body( u16, 1 )
  _body( u16, 1 )
   _ev_end

  
/***********************<< �ļ��������ָʾ >>********************	
 *[��Ϣ��]
 * +��
 *[��Ϣ����]
 *    ui =>  mtservice
 */_event(  ev_UpgradeFileDownloadCompletedInd   ) 
   _ev_end


/***********************<< ����ϵͳ�ļ�״ָ̬ʾ >>********************	
 *[��Ϣ��]
 * +BOOL     //�����ļ��Ƿ�ɹ�
 * +u8       //����ʧ�ܵ�ԭ��
 *[��Ϣ����]
 *    ui =>  mtservice
 */_event(  ev_UpgradeSysFileUpdateStatusInd   ) 
  _body( BOOL, 1 )
  _body( u8, 1 )   
   _ev_end
 
   
/***********************<< ������������ַ���� >>********************	
 *[��Ϣ��]
 * +BOOL    //�Ƿ�ΪĬ�ϵ�ַ�������û����õķ�������ַ
 * +s8      //��ַ

 *[��Ϣ����]
 *    ui =>  mtservice
 */_event(  ev_UpgradeSrvAddrCfgCmd   )
  _body( BOOL, 1 )
  _body( s8, REMAIN )
  _ev_end
   
/***********************<< ������������ַ����ָʾ >>********************	
 *[��Ϣ��]
 * +BOOL    //�Ƿ�ΪĬ�ϵ�ַ�������û����õķ�������ַ
 * +s8      //�ܰ�

 *[��Ϣ����]
 *    ui =>  mtservice
 */_event(  ev_UpgradeSrvAddrCfgInd   )
  _body( BOOL, 1 )
  _body( s8, REMAIN )
  _ev_end
  
/***********************<<  �����������������ն�����ϵ�����  >>******************
   *[��Ϣ��]	
   * + BOOL  //�Ƿ�ϵ�����	 
   *[��Ϣ����]
   *
   */_event( ev_UpgradeResumeConnectCmd )
   _body( BOOL, 1 )
   _ev_end
   
/***********************<<  ������������·,����ָʾ  >>******************
   *[��Ϣ��]			 
   *[��Ϣ����]
   *
   */_event( ev_UpgradeDisconnectInd )
   _ev_end

/***********************<< ���������������°汾��Ϣ, �б�ָʾ >>********************	
 *[��Ϣ��]			 
 *  TVerInfo   //���µİ汾��Ϣ        
 *   
 *[��Ϣ����]
 *    ui <=  mtservice
 */_event(  ev_UpgradeNewVersionInfoListInd   )
   _body( u8 , 1 ) 
   _body( TVerInfo, REMAIN )
   _ev_end
   

/***********************<< �Ͽ��������������ӵ�ָʾ >>********************	
 *[��Ϣ��]			 
 *   +��         //����Ϣ��������ն˵����������Ϣ
 *            
 *[��Ϣ����]
 *    ui/mtc <= mtservice
 */_event(  ev_UpgradeDisconnectSrvInd   )
 _ev_end

/***********************<< osd���ӷ�������ָʾ >>********************	
 *[��Ϣ��]			 
 *    //�ն���Ϣ,������Ϣ������ev_UpgradeConnectSrvCmd�󣬸���Ϣ����
 *  *u8    //1: ��������osd; 2: ��������mtc
 *   
 *[��Ϣ����]
 *   ui  => mtservice 
 */_event(  ev_UpgradeConnectSrvInd   )
   _body( u8 , 1 )
   _ev_end


#ifndef _MakeMtEventDescription_
   _ev_segment_end( autoupgrade )
};
#endif

#endif