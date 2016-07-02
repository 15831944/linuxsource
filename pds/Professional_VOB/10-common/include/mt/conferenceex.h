#ifndef CONFERENCEEX_EVENT_H
#define CONFERENCEEX_EVENT_H

#include "eventcomm.h"


#ifndef _MakeMtEventDescription_
enum EmConference
{
#endif 
    _ev_segment(conferenceex )//������չ��Ϣ

/**********************<< ����廰 >>**************	
 *[��Ϣ��]			 
 *  
 *[��Ϣ����]
 *
 *   UI => MtService  
 */_event(  ev_ConfApplyChiemeReq  )
   _ev_end


 /**********************<< ָ���廰 >>**************	
 *[��Ϣ��]			 
 *  +TMtId
 *[��Ϣ����]
 *
 *   UI => MtService 
 */_event(  ev_ConfChiemeInReq  )
   _body ( TMtId   , 1          )
   _ev_end


/**********************<< ͬ��廰(��ϯ) >>**************	
 *[��Ϣ��]			 
 *  +TMtId
 *[��Ϣ����]
 *
 *   UI => MtService 
 */_event(  ev_ConfChiemeInCmd  )
   _body ( TMtId   , 1          )
   _ev_end


/**********************<< �ն˲廰���ָʾ >>**************	
 *[��Ϣ��]			 
 *  +BOOL    TRUE ���ڲ廰 FALSEʧ��
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfChimeInStateInd  )  
   _body (  BOOL   , 1          )
   _ev_end


/**********************<< �����ն�����廰ָʾ >>**************	
 *[��Ϣ��]			 
 *  +TMtId
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfApplyChimeInInd   ) 
   _body ( TMtId   , 1          )
   _ev_end


 /**********************<< ��ѯ���� >>**************	
 *[��Ϣ��]			 
 * + TMtPollInfo
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfPollCmd  ) 
   _body ( TMtPollInfo   , 1          )
   _ev_end


 /**********************<< ��ѯ״ָ̬ʾ >>**************	
 *[��Ϣ��]			 
 * + TMtPollInfo
 * 
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfPollStatInd  ) 
   _body ( TMtPollInfo   , 1          )
   _ev_end


/**********************<< ��ѯ��ʱ >>**************	
 *[��Ϣ��]			 
 * 
 * 
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_PollTimeroutInd  )
   _ev_end


/**********************<< ��ϯ�����ն�chair->MC) >>**************	
 *[��Ϣ��]			 
 *  + TTERADDR
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfChairInviteTerReq   ) 
   _body (  TMtAddr   , 1              )
   _ev_end



/**********************<< ��ϯ�����ն�ʧ��(MC->chair) >>**************	
 *[��Ϣ��]			 
 *  +BOOL  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfChairInviteFailInd   ) 
   _body ( BOOL   , 1          )
   _ev_end


 /**********************<< Զ�˾���ָʾ >>**************	
 *[��Ϣ��]			 
 *  +TMtId
 *  + BOOL     TRUE=���� /FALSE=
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 */_event(  ev_ConfMakeTerQuietInd   ) 
   _body ( TMtId   , 1          ) 
   _body ( BOOL   , 1          )
   _ev_end


 /**********************<< Զ������ָʾ >>**************	
 *[��Ϣ��]			 
 *  +TMtId
 *  + BOOL     TRUE=���� /FALSE=
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 */_event(  ev_ConfMakeTerMuteInd   ) 
   _body ( TMtId   , 1          ) 
   _body ( BOOL   , 1          )
   _ev_end


/**********************<< Զ�˾������� >>**************	
 *[��Ϣ��]			 
 *  +TMtId
 *  + BOOL     TRUE=���� /FALSE=
 *[��Ϣ����]
 *
 *   MtService => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfMakeTerQuietCmd   ) 
   _body ( TMtId   , 1          ) 
   _body ( BOOL   , 1          )
   _ev_end


/**********************<< Զ���������� >>**************	
 *[��Ϣ��]			 
 *  +TMtId
 *  + BOOL     TRUE=���� /FALSE=
 *[��Ϣ����]
 *
 *   MtService => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfMakeTerMuteCmd   ) 
   _body ( TMtId   , 1          ) 
   _body ( BOOL   , 1          )
   _ev_end


/**********************<< �ն����������ն�״̬ >>**************	
 *[��Ϣ��]			 
 *  +TMtId
 *  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfTerGetTerStatusReq   ) 
   _body ( TMtId   , 1          )
   _ev_end


/**********************<< �ն����������ն�Ӧ�� >>**************	
 *[��Ϣ��]			 
 *  +TMtId
 *  +TTerStatus
 *  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfTerGetTerStatusRsp   ) 
   _body ( TMtId   , 1          ) 
   _body ( TTerStatus   , 1          )
   _ev_end


/**********************<< MC�����ն�״̬ >>**************	
 *[��Ϣ��]			 
 *  +TMtId
 *  +TTerStatus
 *  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfMcGetTerStatusReq   ) 
   _body ( TMtId   , 1          ) 
   _body ( TTerStatus   , 1          )
   _ev_end


/**********************<< MC�����ն�״̬Ӧ�� >>**************	
 *[��Ϣ��]			 
 *  +TTerStatus
 *  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfMcGetTerStatusRsp   ) 
   _body ( TTerStatus   , 1          )
   _ev_end




/**********************<< ������Ϣ���� >>**************	
 *[��Ϣ��]			 
 *  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfConfInfoReq   ) 
   _ev_end



/**********************<< ������ϢӦ�� >>**************	
 *[��Ϣ��]			 
 *  TMtConfInfo
 *  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfConfInfoInd   ) 
   _body ( TMtConfInfo, 1 )
   _ev_end


/**********************<< ���鼴������ >>**************	
 *[��Ϣ��]			 
 *  +u16 minutes  ��ʾ���ж�ý���
 *  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfWillEndInd   )
   _body ( u16   ,1            )
   _ev_end


/**********************<< �����ӳ����� >>**************	
 *[��Ϣ��]			 
 *  +u16 minutes  
 ��ʾ�ӳ����
 *  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfProlongReq   ) 
   _body ( u16   , 1          )
   _ev_end



/**********************<< �����ӳ�����Ӧ�� >>**************	
 *[��Ϣ��]			 
 *  +BOOL    TRUE=ͬ��
 *  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfProlongRsp   ) 
   _body ( BOOL   , 1          )
   _ev_end



/**********************<< ��ʼ������������ >>**************	
 *[��Ϣ��]			 
 *
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStartVACReq   ) 

   _ev_end


/**********************<< ��ʼ��������Ӧ�� >>**************	
 *[��Ϣ��]	
 *  +BOOL    TRUE=ͬ��
 *  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStartVACRsp   ) 
   _body ( BOOL      , 1          )
   _ev_end


/**********************<< ֹͣ������������ >>**************	
 *[��Ϣ��]	
 *
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStopVACReq   )
   _ev_end


/**********************<< ֹͣ��������Ӧ�� >>**************	
 *[��Ϣ��]	
 *  +BOOL    TRUE=ͬ��
 *
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStopVACRsp   )
   _body ( BOOL      , 1        )
   _ev_end


/**********************<< ��ʼ����ģʽ���� >>**************	
 *[��Ϣ��]			 
 *  +u8  �������
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStartDiscussReq   ) 
   _body( u8       ,   1 )
   _ev_end


/**********************<< ��ʼ����ģʽӦ�� >>**************	
 *[��Ϣ��]	
 *  +BOOL    TRUE=ͬ��
 *  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStartDiscussRsp   ) 
   _body ( BOOL   , 1          )
   _ev_end


/**********************<< ֹͣ����ģʽ���� >>**************	
 *[��Ϣ��]			 

 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStopDiscussReq   ) 
   _ev_end


/**********************<< ֹͣ����ģʽӦ�� >>**************	
 *[��Ϣ��]	
 *  +BOOL    TRUE=ͬ��
 *  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStopDiscussRsp   ) 
   _body ( BOOL   , 1          )
   _ev_end


/**********************<< ��ʼ�Զ��໭��ϳ����� >>**************	
 *[��Ϣ��]			 
 * +TMtVMPParam

 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStartVMPReq   )
   _body( TMtVMPParam, 1 )
   _ev_end


/**********************<< ��ʼ�Զ��໭��ϳ�Ӧ�� >>**************	
 *[��Ϣ��]	
 *  +BOOL    TRUE=ͬ��
 *  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStartVMPRsp   )  
   _body ( BOOL   , 1          )
   _ev_end


/**********************<< ֹͣ�໭��ϳ����� >>**************	
 *[��Ϣ��]			 
 * +TMtVMPParam
 *
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStopVMPReq   )
   _ev_end


/**********************<< ֹͣ�Զ��໭��ϳ�Ӧ�� >>**************	
 *[��Ϣ��]	
 *  +BOOL    TRUE=ͬ��
 *  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStopVMPRsp   ) 
   _body ( BOOL      , 1          )
   _ev_end


/**********************<< �����Զ��໭��ϳɲ������� >>**************	
 *[��Ϣ��]	
 *  +TMtVMPParam
 *  
 *  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfSetVMPParamReq   ) 
   _body ( TMtVMPParam   , 1          )
   _ev_end


/**********************<< �����Զ��໭��ϳɲ���Ӧ�� >>**************	
 *[��Ϣ��]	
 *  +BOOL    TRUE=ͬ��
 *  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfSetVMPParamRsp   ) 
   _body ( BOOL   , 1          )
   _ev_end

/**********************<< ��ȡ�Զ��໭��ϳ�Ӧ��������� >>**************	
 *[��Ϣ��]	
 * ��
 *  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfGetVMPParamReq   )
   _ev_end


/**********************<< ��ȡ�Զ��໭��ϳ�Ӧ�����Ӧ�� >>**************	
 *[��Ϣ��]	
 *  +TMtVMPParam
 *  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfGetVMPParamRsp   ) 
   _body ( TMtVMPParam   , 1          )
   _ev_end


/**********************<< �򵥻�����Ϣָʾ >>**************	
 *[��Ϣ��]	
 *  +TMtSimpConfInfo
 *  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_SimpleConfInfoInd   ) 
   _body ( TMtSimpConfInfo   , 1          )
   _ev_end

/*********************<< ��ƵԴ����ָʾ  >>************************
 *[��Ϣ��]	
 * + char[]
 *
 *[��Ϣ����]
 *    Service  = > UI
 *
*/_event(  ev_VideoSourceNameInd                    )
  _body(   s8    ,REMAIN                            )
  _ev_end

     _ev_segment_end(conference)
};

#endif

