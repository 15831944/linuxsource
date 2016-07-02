#ifndef CONFERENCE_EVENT_H
#define CONFERENCE_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmConference
{
#endif 
   _ev_segment(conference )//������Ϣ

/***********************<< ������·״ָ̬ʾ >>**********************	
 *[��Ϣ��]			 
 *  TLinkState
 *[��Ϣ����]
 *
 *   MtService <=> 323/sip/320
 *   MtService => UI
 */_event(  ev_CallLinkStateInd   )
   _body ( TLinkState   , 1       )
   _ev_end
 
 /***********************<< �����鿪ʼָʾ >>**********************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *   MtService <=> 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfMccStartInd   )
   _ev_end
 
/***********************<< ��������������� >>***********************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *   MtService <=> 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfPasswordReq   )
   _ev_end
 

/***********************<< �������������Ӧ >>***********************	
 *[��Ϣ��]			 
 *  +s8  ����
 *[��Ϣ����]
 *
 *   MtService <=> 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfPasswordRsp   )
   _body (  s8   , REMAIN       )
   _ev_end


/***********************<< ѡ������б����� >>***********************	
 *[��Ϣ��]			 
 *   + TConfListInfo[]     ����
 *[��Ϣ����]
 *����Ϣ��ҵ���͸�������ѡ��Ԥ����Ļ����б�
 *  
 *   MtService => UI
 */_event(  ev_ConfListChoiceReq   )
   _body ( TConfListInfo   , REMAIN      )
   _ev_end


/***********************<< ѡ������б���Ӧ >>***********************	
 *[��Ϣ��]			 
 *   +TConfId
 *[��Ϣ����]
 *  
 *   MtService => UI
 */_event(  ev_ConfListChoiceRsp   )
   _body ( TConfBaseInfo   , 1           )
   _ev_end



/***********************<< �����ն˱�� >>***********************	
 *[��Ϣ��]			 
 *  +TMtId		�ն˱��
 *[��Ϣ����]
 *
 *   MtService <=> 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfTerLabelInd   ) 
   _body ( TMtId       , 1       )
   _ev_end



/***********************<< �ն˼������ >>***********************	
 *[��Ϣ��]			 
 *  +TMtId		�ն˱��
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfTerJoinedInd   ) 
   _body ( TMtId   , 1            )
   _ev_end


/***********************<< �ն��˳����� >>***********************	
 *[��Ϣ��]			 
 *  +TMtId		�ն˱��
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfTerLeftInd   ) 
   _body ( TMtId   , 1          )
   _ev_end


/***********************<< ǿ���ն��˳� >>***********************	
 *[��Ϣ��]			 
 *  +TMtId		�ն˱��
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfDropTerCmd   ) 
   _body ( TMtId   , 1          )
   _ev_end


/***********************<< ǿ���ն��˳�ʧ�� >>***********************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfDropTerRejectInd   )
   _ev_end



/***********************<< ���뷢�� >>***********************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfApplySpeakReq   )
   _ev_end


/***********************<< ����ն����뷢�� >>***********************	
 *[��Ϣ��]			 
 *  TMtId    �ն˱��
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfApplySpeakInInd   )
   _body ( TMtId   , 1          )
   _ev_end


/***********************<< ������λ��ָʾ >>***********************	
 *[��Ϣ��]			 
 *  +TMtId		�ն˱��
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfSpeakerInd   ) 
   _body ( TMtId   , 1          )
   _ev_end


/*********<< �㲥�ն�����(��ϯ)����������Ӧ�� _event(  ev_ConfApplySpeakInInd>>**************	
 *[��Ϣ��]			 
 *  +TMtId		�ն˱��
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfBroadcastCmd   ) 
   _body ( TMtId   , 1          )
   _ev_end


/*********<< �㲥�ն�����(��ϯ)����������ָ��ĳ�ն�Ϊ������>>**************	
 *[��Ϣ��]			 
 *  +TMtId		�ն˱��
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfBroadcastReq   ) 
   _body ( TMtId   , 1          )
   _ev_end


/*********<< �㲥Ӧ�� >>**************	
 *[��Ϣ��]			 
 *  +BOOL    TRUE=ͬ�� FALSE=�ܾ� 
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfBroadcastRsp   ) 
   _body ( BOOL   , 1          )
   _ev_end




/*********<< ȡ���㲥����   >>**************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfCancelBroadcastCmd   ) 
   _ev_end



/**********************<< ѡ���ն����� >>**************	
 *[��Ϣ��]			 
 *  +TMtId		 �ն˱��
 *  +EmMediaType ѡ������
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfSendThisTerCmd   ) 
   _body ( TMtId   , 1          )
   _body ( EmMediaType, 1          )
   _ev_end


 /**********************<< ѡ���ն����� >>**************	
 *[��Ϣ��]			 
 *  +TMtId		�ն˱��
  *  +EmMediaType ѡ������
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfSendThisTerReq   ) 
   _body ( TMtId   , 1          )
   _body ( EmMediaType, 1          )
   _ev_end


/**********************<< ѡ���ն�Ӧ�� >>**************	
 *[��Ϣ��]			 
 *  +BOOL     TRUE=ѡ���ɹ� FALSE=ѡ��ʧ��
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfSendThisTerRsp   ) 
   _body ( BOOL   , 1          )
   _ev_end


 /**********************<< ȡ��ѡ������ >>**************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfCancelSendThisTerCmd   )
   _ev_end


/*********<< ȡ��ѡ���ն�Ӧ�� >>**************	
 *[��Ϣ��]			 
 *  +BOOL    TRUE=ͬ�� FALSE=�ܾ� 
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfCancelSendThisTerRsp   )
   _ev_end


/**********************<< ���ն˱�ѡ��ָʾ >>**************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfSeenByOtherInd   )
   _ev_end


/**********************<< ���˱�ȡ��ѡ�� >>**************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfCancelSeenByOtherInd   )
   _ev_end


/**********************<< ���ڹۿ�����ƵԴ >>**************	
 *[��Ϣ��]			 
 *  +TMtId		�ն˱��
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfYouAreSeeingInd   ) 
   _body ( TMtId   , 1          )
   _ev_end


/**********************<< �����ն��б� >>**************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfTerListReq   ) 
   _ev_end


/**********************<< �ն��б�ָʾ >>**************	
 *[��Ϣ��]			 
 *  +TMtInfo[]   �ն˱������
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfTerListInd   ) 
   _body ( TMtInfo   , REMAIN     )
   _ev_end


/**********************<< ter���������ն��� >>**************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfTerGetAllTerNameReq   )
   _ev_end


/**********************<< MC�ش������ն����� >>**************	
 *[��Ϣ��]			 
 *  +TMtInfo[] terInfo����
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfTerGetAllTerNameRsp   ) 
   _body ( TMtInfo   , REMAIN          )
   _ev_end


/**********************<< ter���������ն��� >>**************	
 *[��Ϣ��]			 
 *  TMtId     �ն˱��
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfTerGetTerNameReq   )
   _body ( TMtId   , 1          )
   _ev_end


/**********************<< MC�ش������ն����� >>**************	
 *[��Ϣ��]			 
 *  +TMtInfo[] terInfo����
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfTerGetTerNameRsp   ) 
   _body ( TMtInfo   , REMAIN          )
   _ev_end


/**********************<< MC�����ն����� >>**************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfMcGetTerNameReq   ) 
   _ev_end


/**********************<< ter�ش��ն����� >>**************	
 *[��Ϣ��]			 
 *  +TMtInfo[] terInfo����
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfMcGetTerNameRsp   ) 
   _body ( TMtInfo   , REMAIN          )
   _ev_end



/**********************<< �������� >>**************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfEndConfCmd   )
   _ev_end


/**********************<< ���㲥ָʾ >>**************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfSeenByAllInd   )
   _ev_end



/**********************<< ȡ���㲥ָʾ >>**************	
 *[��Ϣ��]			 
 *  
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfCancelSeenByAllInd   )
   _ev_end



/**********************<< ������ϯ >>**************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfMakeMeChairReq   )
   _ev_end


/**********************<< ת����ϯ���� >>**************	
 *[��Ϣ��]			 
*  +TMtId
 *[��Ϣ����]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfChairTransferReq  ) 
   _body ( TMtId   , 1          )
   _ev_end


/**********************<< ��ϯ��������>**************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *   MtService <=> 323/sip/320
 *   MtService <=> UI
 */_event(  ev_ConfWithdrawChairCmd  )
   _ev_end



/**********************<< ��ϯ����ָʾ >>**************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *   MtService <=> 323/sip/320
 *   MtService <=> UI
 */_event(  ev_ConfWithdrawChairInd  )
   _ev_end


/**********************<< ѯ�ʵ�ǰ��ϯ >>**************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *   MtService => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfWhoIsChairReq  )
   _ev_end


/**********************<< ѯ�ʵ�ǰ��ϯӦ�� >>**************	
 *[��Ϣ��]			 
 *  +TMtId
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfWhoIsChairRsp  ) 
   _body ( TMtId   , 1          )
   _ev_end


/**********************<< ������ϯ����Ӧ�� >>**************	
 *[��Ϣ��]			 
 *  +BOOL ( TRUE/FALSE )
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfChairTokenRsp  ) 
   _body ( BOOL   , 1          )
   _ev_end


/**********************<< ��ϯ�ն�ָʾ >>**************	
 *[��Ϣ��]			 
 + TMtId
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfChairInd  ) 
   _body ( TMtId   , 1          )
   _ev_end



/**********************<< ���ն�������ϯ >>**************	
 *[��Ϣ��]			 
 *  +TMtId
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfApplyChairInInd  ) 
   _body ( TMtId   , 1          )
   _ev_end



/**********************<< ����Ϣָʾ >>**************	
 *[��Ϣ��]			 
 *  char[]
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfSMSInd  ) 
   _body ( s8   ,REMAIN    )
   _ev_end


/**********************<< ���Ͷ���Ϣ >>**************	
 *[��Ϣ��]			 
 *  char[]
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfSMSCmd  ) 
   _body (  s8   , REMAIN          )
   _ev_end


/**********************<< ����ʼ�鲥 >>**************	
 *[��Ϣ��]			 
 *  +TMtId
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStartMultiCastTerReq  )  
   _body ( TMtId   , 1          )
   _ev_end



  
 
 
 

/**********************<< ����ʼ�鲥��Ӧ >>**************	
 *[��Ϣ��]			 
 *  + TMtId
 *  + u32( Ŀ���ն�IP��ַ,���ʧ��ipΪ0 )
 *  + u16( ��Ƶ�˿� )
 *  + u16( ��Ƶ�˿� )
 *  + u8(��Ƶ��ʵ�غ�ֵ)  
 *  + u8(��Ƶ��ʵ�غ�ֵ)
 *  + EmEncryptArithmetic(��Ƶ��������)
 *  + EmEncryptArithmetic(��Ƶ��������)
 *  + TEncryptKey(��Ƶ������Ϣ)
 *  + u32 ��Ƶ��̬�غ�
 *  + TEncryptKey(��Ƶ������Ϣ)
 *  + u32 ��Ƶ��̬�غ�

 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(ev_ConfStartMultiCastTerRsp) 
   _body ( TMtId   , 1             )  
   _body ( u32     , 1             ) 
   _body ( u16     , 1             ) 
   _body ( u16     , 1             )
   _body ( u8      , 1             )
   _body ( u8      , 1             )
   _body( EmEncryptArithmetic,  1  )
   _body( EmEncryptArithmetic,  1  )
   _body( u32,  1  )
   _body( TEncryptKey,  1  )
   _body( u32,  1  )
   _body( TEncryptKey,  1  )
   _ev_end



/**********************<< ����ֹͣ�鲥 >>**************	
 *[��Ϣ��]			 
 *  +TMtId
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStopMultiCastTerReq  ) 
   _body ( TMtId   , 1          )
   _ev_end
   
/***********************<< MTC MT��Ϣͬ�� >>**********************	
 *[��Ϣ��]			 
 *  NULL
 *[��Ϣ����]
  * UI <- MtService
 */_event( ev_UISynchronizeInd )
   _ev_end

/***********************<< MTC MT��Ϣͬ�� >>**********************	
 *[��Ϣ��]			 
 *  NULL
 *[��Ϣ����]
  * mcservice <- h323service
 */_event( ev_ConfInnerMcAdjustVidResCmd )
   _body( u8, 1 )
   _body( u8, 1 )
   _ev_end
   
/***********************<< �������������Ӧ, DTMF���� >>***********************	
 *[��Ϣ��]			 
 *  +s8  ����
 *[��Ϣ����]
 *
 *   MtService <=> 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfPasswordDtmf   )
   _body (  s8   , REMAIN       )
   _ev_end
   

/***********************<< MC����Է���������֡������ >>***********************	
 *[��Ϣ��]			 
 *  +u8      1     ����֡��
 *[��Ϣ����]
 *
 *   mcservice => 323/sip/320
 */_event(  ev_ConfInnerMcAdjustFrameRateCmd   )
   _body (  u8      , 1       )
   _ev_end

/***********************<< MC����Է���������֡��ָʾ >>***********************	
 *[��Ϣ��]			 
 *  +u8      1     ����֡��
 *[��Ϣ����]
 *
 *   mcservice <= 323/sip/320
 */_event(  ev_ConfInnerMcAdjustFrameRateInd   )
   _body (  u8      , 1       )
   _ev_end

/***********************<< ��ǰ�������FEC����ָʾ >>***********************	
 *[��Ϣ��]			 
 *  +u8      1     FEC�㷨���� 0:NONE  1:RAID5  2:RAID6
 *[��Ϣ����]
 *
 *   mcservice <= 323/sip/320
 */_event(  ev_ConfFecCapacityTypeInd   )
   _body (  u8      , 1       )
   _ev_end

   

#ifndef _MakeMtEventDescription_
   _ev_segment_end(conference)
};
#endif
#endif

