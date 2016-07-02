
#ifndef USERMANAGER_EVENT_H
#define USERMANAGER_EVENT_H

#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmUserManager
{
#endif
/***********************************************************/
/*                                                         */
/*                 �û�������Ϣ                            */
/*    osp ��Ϣ                                             */
/***********************************************************/
    _ev_segment( user_manager )
#if 0
/***********************<< �ͻ��������¼��Ϣ >>********************	
 *[��Ϣ��]			 
 *   + u8[32]         //ͨ�����ݽṹ�汾����
 *   + CLoginRequest
 *[��Ϣ����]
 *    MTC => UE
 */_event(  ev_UMLoginReq   )
   _ev_end



/***********************<< �ͻ��������¼ȷ��Ӧ�� >>********************	
 *[��Ϣ��]			 
 *   ����Ϣ��
 *[��Ϣ����]
 *   UE <= MTC
 */_event(  ev_UMLoginAck   )
   _ev_end



 /***********************<< �ͻ��������¼�ܾ�Ӧ�� >>********************	
 *[��Ϣ��]
 *   + CUserFullInfo  //�ѵ�½�û���Ϣ
 *   + u8[32]         //ͨ�����ݽṹ�汾����
 *   + u8             // ��½ʧ��ԭ��   0=�û�����ģ�� 1=�����û���½ 2������汾��ƥ��
 *   + u16            // �û�����ģ������
 *   + u32            //�ѵ�½�û�IP
 *[��Ϣ����]
 *   UE => MTC
 */_event(  ev_UMLoginNack   )
   _ev_end
#endif
/******************<< Slave IP��ͻ��ʱ��4  >>**********************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *   MtService => MtService
 */_event( ev_T2SlaveIPConflictTimer4 )
   _ev_end

/******************<< Slave IP��ͻ��ʱ��5  >>**********************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *   MtService => MtService
 */_event( ev_T2SlaveIPConflictTimer5 )
   _ev_end

/******************<< Slave IP��ͻ��ʱ��6  >>**********************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *   MtService => MtService
 */_event( ev_T2SlaveIPConflictTimer6 )
   _ev_end

/************************<< ��ȡ�����û��б�����  >>********************
 *[��Ϣ��]
 * +CUserFullInfo[] �û���Ϣ�б�
 *[��Ϣ����]
 *  UE <= MTC
 */_event(  ev_UMGetAllUserListReq  )
   _ev_end


 
 /**********************<< ��ȡ�����û���Ϣ�б�ȷ��Ӧ��  >>*************
  *[��Ϣ��]
  * +CUserFullInfo[] �û���Ϣ�б�
  *[��Ϣ����]
  * UE => MTC
  */_event(  ev_UMGetAllUserListACK  )
   _ev_end


 
 /***********************<< ��ȡ�����л���Ϣ�б�ܾ�Ӧ�� >>************
  *[��Ϣ��]
 *   + u8[32]         //ͨ�����ݽṹ�汾����
 *   + u8             // ��½ʧ��ԭ��   0=�û�����ģ�� 1=�����û���½ 2������汾��ƥ��
 *   + u16            // �û�����ģ������
 *   + CUserFullInfo  //�ѵ�½�û���Ϣ
 *   + u32            //�ѵ�½�û�IP
 */_event(  ev_UMGetAllUserListNACK  )
   _ev_end


 
  
/***********************<< �ͻ�������û����� >>********************	
 *[��Ϣ��]			 
 *   +CUserFullInfo
 *[��Ϣ����]
 *   UE => MTC
 */_event(  ev_UMAddUserReq  )
   _ev_end



/***********************<< �ͻ�������û�ȷ���� >>********************	
 *[��Ϣ��]			 
 *  ����Ϣ��
 *[��Ϣ����]
 *   UE => MTC
 */_event(  ev_UMAddUserAck  )
   _ev_end



/***********************<< �ͻ�������û��ܾ� >>********************	
 *[��Ϣ��]			 
 *  + u16 �����
 *[��Ϣ����]
 *   UE => MTC
 */_event(  ev_UMAddUserNack  )
   _ev_end



/***********************<< �ͻ���ɾ���û����� >>********************	
 *[��Ϣ��]			 
 *   +CUserFullInfo
 *[��Ϣ����]
 *   UE => MTC
 */_event(  ev_UMRemoveUserReq  )
   _ev_end



/***********************<< �ͻ���ɾ���û�ȷ�� >>********************	
 *[��Ϣ��]			 
 *  ����Ϣ��
 *[��Ϣ����]
 *   UE => MTC
 */_event(  ev_UMRemoveUserAck  )
   _ev_end


/**********************<< �ͻ���ɾ���û��ܾ� >>*********************
 *[��Ϣ��]			 
 *  + u16 �����
 *[��Ϣ����]
 *   UE => MTC
 */_event(  ev_UMRemoveUserNack  )
   _ev_end


 
/**********************<< �ͻ��˸����û���������>>*******************
 *[��Ϣ��]			 
 *   +CUserFullInfo
 *[��Ϣ����]
 *   UE => MTC
 */_event(  ev_UMChangePWDReq  )
   _ev_end



/***********************<< �ͻ��˸����û�����ȷ�� >>********************	
 *[��Ϣ��]			 
 *  ����Ϣ��
 *[��Ϣ����]
 *   UE => MTC
 */_event(  ev_UMChangePWDACK  )
   _ev_end


/**********************<< �ͻ��˸����û�����ܾ� >>*********************
 *[��Ϣ��]			 
 *  + u16 �����
 *[��Ϣ����]
 *   UE => MTC
 */_event(  ev_UMChangePWDNACK  )
   _ev_end


/**********************<< �ͻ��˸����û�����ܾ� >>*********************
 *[��Ϣ��]			 
 *  +CUserFullInfo[16] �û���Ϣ�б�
 *[��Ϣ����]
 *   UE => MTC
 */_event(  ev_UMNotifyMTC  )
   _ev_end

/**********************<< ϵͳ����δ���mtc�Ҷ���Ϣ >>*********************
 *[��Ϣ��]			 
 *  +CUserFullInfo[16] �û���Ϣ�б�
 *[��Ϣ����]
 *   UE => MTC
 */_event(  ev_UMMtSysHasNotInited  )
   _ev_end

#ifndef _MakeMtEventDescription_
   _ev_segment_end(user_manager)
};
#endif

#endif



