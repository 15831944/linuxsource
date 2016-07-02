#ifndef MC_EVENT_H
#define MC_EVENT_H

#include "eventcomm.h"


#ifndef _MakeMtEventDescription_
enum EmMCEvent
{
#endif  
	  _ev_segment( MC )
	
/***********************<< ��չ�������� >>************************
 *[��Ϣ��]
 *  + TDialParam
 *  + s8[MT_MAX_NAME_LEN] �ն˱���
 *[��Ϣ����]
 *
 *    MCService <= MtService <= MTUI
 */_event( ev_mcAdHocCmd )
   _body ( TDialParam ,1 )
   _ev_end

/***********************<< ��չ����Ӧ�� >>************************
 *[��Ϣ��]
 *  + TMtId
 *  + BOOL                       TRUE=ͬ�� FALSE =�ܾ�
 *  + EmCallDisconnectReason     �ܾ�ʱ���ԭ��
 *[��Ϣ����]
 *
 *    MCService <= MtService <= MTUI
 */_event( ev_mcAcceptInd )
   _body( TMtId                   ,1 )
   _body( BOOL                    ,1 )
   _body( EmCallDisconnectReason  ,1 )
   _ev_end

/***********************<< ��չ������н������� >>************************
 *[��Ϣ��]
 *  + TDialParam
 *[��Ϣ����]
 *
 *    MCService <=> MtService <=> MTUI
 */_event( ev_mcCallIncommingInd )
   _body(  TMtId      ,1 )
   _body ( TDialParam ,1 )
   _ev_end   

/***********************<< ��Ե�ʱ������Ϣ >>************************
 *[��Ϣ��]
 *  +s8[MT_MAX_NAME_LEN+1] �Զ˱���
 *  +EmChanType ͨ����������
 *[��Ϣ����]
 *
 *    MCService <=> MtService <=> MTUI
 */_event( ev_mcP2PCallInfo )
   _body(  s8      , MT_MAX_NAME_LEN)
   _ev_end   

/**********************<< mcʱ�����滻���渴���ն�����  >>********************	
 *[��Ϣ��]
 * +u8    //�����λ��
 * +TMtId  //Ҫ�滻���Ǹ��ն�
 *[��Ϣ����]
 *   ui  ->  mtservice
 */_event( ev_mcUIReplaceMtCmd )
   _body( u8,  1 )
   _body( TMtId,  1 )  
   _ev_end


/**********************<< mc����Ƶ�������ָʾ  >>********************	
 *[��Ϣ��]
 * +TVideoEncodeParameter    //vmp�������
 * +EmAudioFormat            //��Ƶ�������
 * +u8    //��Ƶ��̬�غ�
 * +u8    //��Ƶ��̬�غ�
 *[��Ϣ����]
 *   mtservice  ->  mcservice
 */_event( ev_mcAVParamInfo )
   _body( TVideoEncodeParameter,  1 )
   _body( EmAudioFormat,  1 )  
   _body( u8,  1 )
   _body( u8, 1 )
   _ev_end 

   
/**********************<< mc����״ָ̬ʾ  >>********************	
 *[��Ϣ��]
 * +BOOL          //mc�Ƿ�������չ����
 * +BOOL          //mc�Ƿ񼤻�
 *[��Ϣ����]
 *   mtservice  ->  UI
 */_event( ev_mcCapStateInd )
   _body( BOOL,  1 )
   _body( BOOL,  1 )  
   _ev_end

/**********************<< mc�����ն�״ָ̬ʾ  >>********************	
 *[��Ϣ��]
 * +TMtId[]          //���뻭��ϳɵ��ն�id��, 
                     //���terId��mcuId��Ϊ0��Ϊ��Ч, ��ʾ��λ��û���ն�.
					 //����λ�ú���
 *[��Ϣ����]
 *   mtservice  ->  UI
 */_event( ev_mcDiscussMmbInd )
   _body( TMtId,  6 ) 
   _body( EmInnerVMPStyle, 1 )
   _ev_end


/**********************<< mcԶң�ն�����  >>********************	
 *[��Ϣ��]
 * +TMtId         //ҪԶң���ն�id

 *[��Ϣ����]
 *   UI  ->  mtservice
 */_event( ev_mcFeccTermCmd )
   _body( TMtId,  1 ) 
   _ev_end

/**********************<< mcԶң�ն�ָʾ  >>******************** 
 *[��Ϣ��]
 * +TMtId         //ҪԶң���ն�id
 *[��Ϣ����]
 *   UI  ->  mtservice
 */_event( ev_mcFeccTermInd )
   _body( TMtId,  1 ) 
   _ev_end

/**********************<<  ����mc����ģʽ����  >>********************	
 *[��Ϣ��]
 * BOOL        //mc�Ƿ����� 
 * EmMCMode    //mc����ģʽ
 *[��Ϣ����]
 *   ui  ->  mtservice
 */_event( ev_mcModeCfgCmd )
   _body( BOOL,  1   ) 
   _body( EmMCMode ,  1 ) 
   _ev_end
  
/**********************<<  ����mc����ģʽָʾ  >>********************	
 *[��Ϣ��]
 * BOOL        //mc�Ƿ����� 
 * EmMCMode    //mc����ģʽ
 *[��Ϣ����]
 *   mtservice  ->  ui
 */_event( ev_mcModeCfgInd )
   _body( BOOL,  1   ) 
   _body( EmMCMode ,  1 ) 
   _ev_end
   

/**********************<<  ���û���ϳ��Զ�����/���߹̶� >>********************	
 *[��Ϣ��]
 * +BOOL    //TRUE�������Զ����ӣ�FALSE��������̶�
 *[��Ϣ����]
 *   mtservice  ->  mcservice
 */_event( ev_mcVMPCfgCmd )
   _body( BOOL,  1 ) 
   _ev_end 

/**********************<<  ���û���ϳ��Զ�����/���߹̶�ָʾ >>********************	
 *[��Ϣ��]
 * +BOOL    //TRUE�������Զ����ӣ�FALSE��������̶�
 *[��Ϣ����]
 *   mtservice  ->  mcservice
 */_event( ev_mcVMPCfgInd )
   _body( BOOL,  1 ) 
   _ev_end 

/**********************<<  ����vmp�ķ�� >>********************	
 *[��Ϣ��]
 * +EmInnerVMPStyle          //����ķ��
 * +TMtId atMTLoc[6]    //����ϳ�λ���ϵ��ն�id��
 *[��Ϣ����]
 *   mtservice  ->  mcservice
 */_event( ev_mcSetVMPStyleCmd )
   _body( EmInnerVMPStyle,  1 )
   _body( TMtId, 6 ) 
   _ev_end 

/**********************<<  ����vmp�ķ��ָʾ >>********************	
 *[��Ϣ��]
 * +EmInnerVMPStyle      emStyle  //����ķ����/��
 * +TMtId atMTLoc[6]    //����ϳ�λ���ϵ��ն�id��
 *[��Ϣ����]
 *   mtservice  <-  mcservice
 */_event( ev_mcSetVMPStyleInd )
   _body( EmInnerVMPStyle,  1 )
   _body( TMtId, 6 ) 
   _ev_end 

/**********************<< mc�ն���ѯ�㲥 >>********************	
 *[��Ϣ��]
 * +TMtPollInfo
 *[��Ϣ����]
 *   mtservice  ->  mcservice
 */_event( ev_mcPollCmd)
   _body( TMtPollInfo,  1 )
   _ev_end 

/**********************<< mc�ն���ѯ�㲥ָʾ >>********************	
 *[��Ϣ��]
 * +TMtPollInfo
 *[��Ϣ����]
 *   mcservice  ->  mtservice
 */_event( ev_mcPollInd )
   _body( TMtPollInfo,  1 )
   _ev_end 
   
/**********************<< ��ӳ�Ա >>********************	
 *[��Ϣ��]
 * +TMtId          //
 *[��Ϣ����]
 *   mcservice  ->  mtservice
 */_event( ev_mcAddEndPointCmd )
   _body( TMtId ,  1 )
   _ev_end
 

/**********************<< ɾ����Ա >>********************	
 *[��Ϣ��]
 * +u32          //CEndPoint��ָ��ֵ
 *[��Ϣ����]
 *   mcservice  ->  mtservice
 */_event( ev_mcDelEndPointCmd )
   _body( TMtId ,  1 )
   _ev_end 
   
   
/***********************<< ������·״ָ̬ʾ >>**********************	
 *[��Ϣ��]			 
 *  TLinkState
 *[��Ϣ����]
 *
 *   MtService <=> 323/sip/320
 *   McService => UI
 */_event(  ev_mcCallLinkStateInd   )
   _body ( TLinkState   , 1       )
   _body ( TMtId        , 1       )
   _ev_end

/***********************<< MC����״ָ̬ʾ >>**********************	
 *[��Ϣ��]			 
 *  +TMtId 6
 *  +BOOL  6
 *[��Ϣ����]
 *
 *   McService => MtService
 *   MtService => UI/MTC
 */_event(  ev_mcMakeTerMuteInd   )
   _body ( TMtId   , 6       )
   _body ( BOOL    , 6       )
   _ev_end  

/***********************<< MC�������ָʾ >>**********************	
 *[��Ϣ��]			 
 *
 *[��Ϣ����]
 *
 *   McService => MtService
 *   MtService => UI/MTC
 */_event(  ev_mcConfEndInd   )
   _ev_end  
   
/***********************<< MC��ǰ������ָʾ >>**********************	
 *[��Ϣ��]			 
 * +TMtId 1
 *[��Ϣ����]
 *
 *   McService => MtService
 *   MtService => UI/MTC
 */_event(  ev_mcCurSpeekerInd   )
   _body ( TMtId   ,  1 )
   _ev_end

/***********************<< MC�Ҷϵ�ǰ���ں��е��ն� >>**********************	
 *[��Ϣ��]			 
 * 
 *[��Ϣ����]
 *
 *   MtService => McService
 *   UI/MTC => MtService
 */_event(  ev_mcHangUpAddingMtCmd   )
   _ev_end

   
/***********************<< �ն��˳�ʱ�ϱ�����ID >>**********************	
 *[��Ϣ��]			 
 * +TMtId 1
 *[��Ϣ����]
 *
 *   MtService => McService
  */_event(  ev_mcDisconnectedMtInd   )
	_body ( TMtId   ,  1 )
    _ev_end

/***********************<< MC�Ƿ�����ָʾ >>**********************	
 *[��Ϣ��]			 
 * +u8 1          0: Call self  
                  1: the called ter already in MC 
				  255: Unkown err
 *[��Ϣ����]
 *
 *   MtService => UI/MTC
  */_event(  ev_mcIllegalCallInd   )
	_body ( u8   ,  1 )
    _ev_end

	
/***********************<< ��Ե����ʱ����������֪ͨMC >>**********************	
 *[��Ϣ��]			 
 * +u32   : ��Ե��������

 *[��Ϣ����]
 *
 *   MtService => mc
  */_event(  ev_mcConfRateInd   )
	_body ( u32   ,  1 )
    _ev_end
   
#ifndef _MakeMtEventDescription_
   _ev_segment_end(MC)

};
#endif//_MakeMtEventDescription_
#endif//MC_EVENT_H

