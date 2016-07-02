#ifndef CONFERENCEEX_EVENT_H
#define CONFERENCEEX_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmConferenceEx
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
   _body( u8         , 1 )
   _body( TMtVmpItem , REMAIN )
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
   _body( u8         , 1 )
   _body( TMtVmpItem , REMAIN )
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


/**********************<< ֹͣ�Զ��໭��ϳ����� >>**************	
 *[��Ϣ��]			 
 * +TMtVMPParam
 *
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStopVMPReq   )
   _body( TMtVMPParam, 1 )
   _body( u8         , 1 )
   _body( TMtVmpItem , REMAIN )
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
   _body( u8         , 1 )
   _body( TMtVmpItem , REMAIN )
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
   _body( u8         , 1 )
   _body( TMtVmpItem , REMAIN )
   _ev_end


/**********************<< ���ö໭��ϳɲ�������(��Ϊ) >>**************	
 *[��Ϣ��]	
 *  +EmVMPStyle
 *  +TMtId  16
 *  
 *  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfHSetVMPParamReq   ) 
   _body ( EmVMPStyle, 1  )
   _body ( TMtId, 16  )
   _ev_end


/**********************<< ���ö໭��ϳɲ���Ӧ��(��Ϊ) >>**************	
 *[��Ϣ��]	
 *  +BOOL    TRUE=ͬ��
 *  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfHSetVMPParamRsp   ) 
   _body ( BOOL   , 1          )
   _ev_end

/**********************<< ��ʼǿ�ƹ㲥 >>**************	
 *[��Ϣ��]	
 *  +��
 *  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStartForceBroadcastCmd  ) 
   _ev_end

   /**********************<< ֹͣǿ�ƹ㲥 >>**************	
 *[��Ϣ��]	
 *  +��
 *  
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStopForceBroadcastCmd   ) 
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
  

/**********************<< ����δ����ն��б� >>**************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfOfflineTerListReq   ) 
   _ev_end


/**********************<< δ����ն��б�ָʾ >>**************	
 *[��Ϣ��]			 
 *  +TMtInfo[]   �ն˱������
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfOfflineTerListInd   ) 
   _body ( TMtInfo   , REMAIN     )
   _ev_end

/**********************<< ���������ն� >>**************	
 *[��Ϣ��]			 
 *  +TMtId
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfCallOfflineTerCmd   ) 
   _body ( TMtId   , 1     )
   _ev_end

 /*********************<< ��������  >>************************
 *[��Ϣ��]	
 * + TMtId
 *
 *[��Ϣ����]
 *    Service  = > UI
 *
*/_event(  ev_ConfRollCallReq                    )
  _body(   TMtId    ,1                            )
  _ev_end

 /*********************<< ������Ӧ  >>************************
 *[��Ϣ��]	
 * + TMtId
 *
 *[��Ϣ����]
 *    Service  = > UI
 *
*/_event(  ev_ConfRollCallRsp                    )
  _body(   BOOL    ,1                            )
  _ev_end

  /*********************<< �л�������  >>************************
 *[��Ϣ��]	
 * + TMtId
 *
 *[��Ϣ����]
 *    Service  = > UI
 *
*/_event(  ev_ConfSwitchBroadCastStyleCmd              )
  _body(   BOOL    ,1                            )
  _ev_end

  
  /*********************<< �Զ˻�������ָʾ  >>************************
 *[��Ϣ��]	
 * + TMtId
 *
 *[��Ϣ����]
 *    Service  = > UI
 *
*/_event(  ev_ConfPeerCapInfo                  )
  _body(   TPeerCapabilityInfo    ,1           )
  _ev_end
 
/*********************<< MCU���ն˵���Ϣ֪ͨ  >>************************
 *[��Ϣ��]	
 * + TMtId
 *  EmMcuNtfMsgType, ֪ͨ����Ϣ����
 *[��Ϣ����]
 *    h323service  = >  mtservice
 *
*/_event(  ev_ConfMcuInfoNotify                  )
  _body(   EmMcuNtfMsgType   ,  1                )
  _ev_end
 
 /*********************<< �Զ��Ƿ�֧��keda˽��Э�������ָʾ  >>************************
 * xjx_070904, ����ϢΪ˽����Ϣ��ֻ��mtservice��mth323service֮��ͨ�ŵ���Ϣ
 * //�ڲ���Ϣ������չ
 *[��Ϣ��]	
 * + BOOL //�Ƿ�ָʾ�����ش�
 * + BOOL //�Ƿ�֧����Ƶ����
 *  
 *[��Ϣ����]
 *    h323service  = >  mtservice
 *
*/_event(  ev_ConfPeerCapPrivateInfo   )
  _ev_end

 /*********************<< �Զ˲�֧��keda˽��Э�������,�ڹҶϺ�Ļָ�  >>************************
 * xjx_070904, ����ϢΪ˽����Ϣ��ֻ��mtservice��mth323service֮��ͨ�ŵ���Ϣ
 * //�ڲ���Ϣ������չ
 *[��Ϣ��]	
 * + BOOL //�Ƿ�ָʾ�����ش�
 * + BOOL //�Ƿ�֧����Ƶ����
 *  
 *[��Ϣ����]
 *    h323service  = >  mtservice
 *
*/_event(  ev_ConfPeerCapPrivateRecover   )
  _ev_end
  
//xjx_071207
/**********************<< �Ҷ�����ն� >>**************	
 *[��Ϣ��]			 
 *  +TMtId   //�� ev_ConfCallOfflineTerCmd������Ӧ
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfDisconnectOnlineTerCmd   ) 
   _body ( TMtId   , 1     )
   _ev_end

/**********************<< ��ӻ᳡ >>**************	
 *[��Ϣ��]			 
 *  +          //Ӧ�ø�ɾ���᳡�Ƕ�Ӧ��
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event( ev_ConfAddRoomReq )
// _body( )
_ev_end

/**********************<< ��ӻ᳡��Ӧ >>**************	
 *[��Ϣ��]			 
 *  +          //
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event( ev_ConfAddRoomRsp )
_ev_end

/**********************<< ��ǰ����֧�����к��� >>**************	
 *[��Ϣ��]			 
 *  +     //˵����ǰ�Ƕ����飬mcu��huawei�ġ� 
 *[��Ϣ����]
 *
 *   MtService  <= 323/sip/320
 *   MtService => UI
 */_event( ev_ConfSitecallSupportInd )
_ev_end

/**********************<< ���Ḩ���������� >>**************	
 *[��Ϣ��]			 
 *  +      
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event( ev_ConfDepriveTokenReq )
_ev_end

/**********************<< ���Ḩ�����Ƶ���Ӧ >>**************	
 *[��Ϣ��]			 
 *  +      
 *[��Ϣ����]
 *
 *   MtService  <= 323/sip/320
 *   MtService => UI
 */_event( ev_ConfDepriveTokenRsp )
_ev_end


/**********************<< �Ƿ�����Զ��㲥ָʾ���� >>**************	
 *[��Ϣ��]			 
 *  + u8     
 *[��Ϣ����]
 *
 *     UI => MtService
 */_event( ev_ConfHWPollTypeCmd )
 _body( u8    , 1 )
_ev_end



  

#ifndef _MakeMtEventDescription_
   _ev_segment_end(conferenceex)
};
#endif


#endif


