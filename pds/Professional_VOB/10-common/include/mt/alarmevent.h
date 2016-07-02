#ifndef ALAEM_EVENT_H
#define ALAEM_EVENT_H

#include "eventcomm.h"


#ifndef _MakeMtEventDescription_
enum EmAlarm
{
#endif  
 _ev_segment(alarm )//�澯��Ϣ

/***********************<< ��ʼ���澯APP  >>************************	
 *[��Ϣ��]			 
 *    TSNMPCfg
 *
 *[��Ϣ����]
 *    MT => AGENT
 */_event(  ev_AgtInitInd  )
   _body( TSNMPCfg, 1 )
   _ev_end


/***********************<< E1��·״̬�ı� >>************************	
 *[��Ϣ��]			 
 *    TAlarmCode
 *
 *[��Ϣ����]
 *    MT => AGENT
 */_event(  ev_AgtE1LineInd  )
   _body( u32, 1 )
   _ev_end


/***********************<< �ڴ�״̬�ı� >>************************	
 *[��Ϣ��]			 
 *    TAlarmCode
 *
 *[��Ϣ����]
 *    MT => AGENT
 */_event(  ev_AgtMemoryInd  )
 _body( u32, 1 )
   _ev_end


/***********************<< �ļ�ϵͳ״̬�ı� >>************************	
 *[��Ϣ��]			 
 *    TAlarmCode
 *
 *[��Ϣ����]
 *    MT => AGENT
 */_event(  ev_AgtFileSystemInd  )
   _ev_end


/***********************<< ����״̬�ı� >>************************	
 *[��Ϣ��]			 
 *    TAlarmCode
 *
 *[��Ϣ����]
 *    MT => AGENT
 */_event(  ev_AgtTaskInd  )
 _body( u32, 1 )
   _ev_end



/***********************<< MAP״̬�ı� >>************************	
 *[��Ϣ��]			 
 *    TAlarmCode
 *
 *[��Ϣ����]
 *    MT => AGENT
 */_event(  ev_AgtMapInd  )
 _body( u32, 1 )
   _ev_end


 
/***********************<< CCI״̬�ı� >>************************	
 *[��Ϣ��]			 
 *    TAlarmCode
 *
 *[��Ϣ����]
 *    MT => AGENT
 */_event(  ev_AgtCCIInd  )
 _body( u32, 1 )
   _ev_end



/***********************<< ��̫��״̬ >>************************	
 *[��Ϣ��]			 
 *    TAlarmCode
 *
 *[��Ϣ����]
 *    MT => AGENT
 */_event(  ev_AgtEthernetInd  )
 _body( u32, 1 )
   _ev_end

/***********************<< ��̫��״̬ >>************************	
 *[��Ϣ��]			 
 *    BOOL
 *
 *[��Ϣ����]
 *    MT => UI
 */_event(  ev_EthernetStateInd  )
 _body( u32, 1 )
   _ev_end


/***********************<< E1״̬ >>************************	
 *[��Ϣ��]			 
 *    BOOL
 *
 *[��Ϣ����]
 *    MT => UI
 */_event(  ev_E1StateInd  )
 _body( u32, 1 )
   _ev_end


/***********************<< ý����״̬ >>************************	
 *[��Ϣ��]			 
 *    TAlarmCode
 *
 *[��Ϣ����]
 *    MT => AGENT
 */_event(  ev_AgtMediaStreamInd  )
 _body( u32, 1 )
   _ev_end


/***********************<< �豸����״̬�ı� >>************************	
 *[��Ϣ��]			 
 *    TAlarmCode
 *
 *[��Ϣ����]
 *    MT => AGENT
 */_event(  ev_AgtEqpConnectInd  )
 _body( u32, 1 )
   _ev_end



/***********************<< ��Ƶ�����״̬ >>************************	
 *[��Ϣ��]			 
 *    TAlarmCode
 *
 *[��Ϣ����]
 *    MT => AGENT
 */_event(  ev_AgtVidoSourceInd  )
 _body( u32, 1 )
   _ev_end



/***********************<< ����״̬�ı� >>************************	
 *[��Ϣ��]			 
 *    TAlarmCode
 *
 *[��Ϣ����]
 *    MT => AGENT
 */_event(  ev_AgtFanInd  )
 _body( u32, 1 )
   _ev_end



/***********************<< V35״̬�ı� >>************************	
 *[��Ϣ��]			 
 *    TAlarmCode
 *
 *[��Ϣ����]
 *    MT => AGENT
 */_event(  ev_AgtV35Ind  )
 _body( u32, 1 )
   _ev_end



/***********************<< ����״̬֪ͨ >>************************	
 *[��Ϣ��]			 
 *    TAlarmCode
 *
 *[��Ϣ����]
 *    MT => AGENT
 */_event(  ev_AgtPaneLEDInd  )
 _body( u32, 1 )
   _ev_end



/***********************<< ��·����֪ͨ >>************************	
 *[��Ϣ��]			 
 *    U32
 *
 *[��Ϣ����]
 *    MT => MTService
 */_event(  ev_AgtGuardChanBWInd  )
   _ev_end



/***********************<< E1״̬�Ի�֪ͨ >>************************	
 *[��Ϣ��]			 
 *    TAlarmCode
 *
 *[��Ϣ����]
 *    MT => AGENT
 */_event(  ev_AgtE1LoopInd  )
 _body(BOOL , 1)
 _body( u32, 1 )
 _ev_end


/***********************<< ϵͳ������֪ͨ >>************************	
 *[��Ϣ��]			 
 *
 *[��Ϣ����]
 *    MT => AGENT
 */_event(  ev_AgtSysRestarInd  )
 _ev_end

#ifndef _MakeMtEventDescription_
   _ev_segment_end(alarm)
};
#endif

#endif

