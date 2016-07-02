
#ifndef CALL_EVENT_H
#define CALL_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmMtCall
{
#endif  
  
  
    _ev_segment( mtcall  )


/***********************<< ��������֪ͨ >>************************
 *[��Ϣ��]			 
 *  + TCallConfig
 *
 *[��Ϣ����]
 *    MtService => 323/320/sip
 */_event(  ev_CallConfigInd   )
   _body ( s8    ,REMAIN        )
   _ev_end

	
/***********************<< ��������  >>************************
 *[��Ϣ��]			 
 *  + TDialParam
 *
 *[��Ϣ����]
 *    UI        => MtService
 *    MtService => 323/320/sip
 */_event(  ev_MakeCallCmd     )
   _body (  TDialParam    ,1   )
   _ev_end


/***********************<< �������ڴ���ָʾ  >>************************
 *[��Ϣ��]			 
 *  ��
 *
 *[��Ϣ����]
 *
 *    MtService <= 323/320/sip
 */_event(  ev_CallProccedingInd  )
   _ev_end

/***********************<< ��������  >>************************
 *[��Ϣ��]			 
 *  + TDialParam
 *
 *[��Ϣ����]
 *
 *    MtService <= 323/320/sip
 */_event(  ev_CallIncomingInd   )
   _body ( TDialParam    ,1      )
   _ev_end



/***********************<< ���н����ɹ�  >>************************
 *[��Ϣ��]			 
 *  ����Ϣ��
 *
 *[��Ϣ����]
 *
 *    MtService <= 323/320/sip
 */_event(  ev_CallConnectedInd   )
   _ev_end


/***********************<< ���ܺ���ָʾ  >>************************
 *[��Ϣ��]			 
 *  + BOOL  TRUE=���ܺ���  FALSE=�ܾ�����  
 *
 *[��Ϣ����]
 *
 *    MtService => 323/320/sip
 */_event(  ev_CallAcceptInd   )
   _body ( BOOL    ,1      )
   _ev_end



/***********************<< ���йҶ�ָʾ  >>************************
 *[��Ϣ��]			 
 *  +u8  byReason	�Ҷ�ԭ��
 *
 *[��Ϣ����]
 *
 *    MtService <=> 323/320/sip
 */_event(  ev_CallDisconnectedInd     )
   _body(  EmCallDisconnectReason ,1   )
   _ev_end

 
/***********************<< �ҶϺ���  >>************************
 *[��Ϣ��]			 
 *  +u8  byReason	�Ҷ�ԭ��
 *
 *[��Ϣ����]
 *
 *    MtService <=> 323/320/sip
 */_event(  ev_CallHangupCmd   )
   _body(  u8   ,  1   )//�Ҷ�ԭ��
   _ev_end


 
/***********************<< ��ʱָʾ  >>************************
 *[��Ϣ��]			 
 *  +u32
 *
 *[��Ϣ����]
 *
 *    MtService <=> 323/320/sip
 */_event(  ev_CallKeepLiveTimeOutInd   )
   _body ( u32   ,1                     )
   _ev_end






/***********************<< ģʽ������ʱ  >>************************
 *[��Ϣ��]			 
 *  +TLinkState  
 *
 *[��Ϣ����]
 *
 *    MtService <=> 323/320/sip
 */_event(  ev_CallEntryModeInd   )
   _body ( TLinkState   ,1        )
   _ev_end




 /***********************<< ��ʼ˫������  >>************************
 *[��Ϣ��]			 
 *  +EmMtVideoPort  
 *  +BOOL             TRUE=��ʼ FALSE =ֹͣ
 *[��Ϣ����]
 *
 *    MtService <=> 323/320/sip
 */_event(  ev_DualVideoCmd   )
   _body ( EmMtVideoPort   , 1 )//��Ƶ�˿�
   _body ( BOOL            , 1 )//TRUE=��ʼ FALSE =ֹͣ
   _ev_end





/***********************<< ��ʼ˫������  >>************************
 *[��Ϣ��]			 
 *  +EmMtVideoPort  
 *  +BOOL             TRUE=��ʼ FALSE =ֹͣ
 *[��Ϣ����]
 *
 *    MtService <=> 323/320/sip
 */_event(  ev_DualVideoInd   )
   _body ( EmMtVideoPort   , 1 )//��Ƶ�˿�
   _body ( BOOL            , 1 )//TRUE=��ʼ FALSE =ֹͣ
   _ev_end

/***********************<< �������ͳ����Ϣ >>************************	
 *[��Ϣ��]		 
 *   �� 
 *[��Ϣ����]
 *    UI => MtService
 */_event ( ev_ApplyCallInfoStatisticsCmd )
  _ev_end

/***********************<< ����ͳ����Ϣָʾ >>************************	
 *[��Ϣ��]		 
 *  +TCallInfoStatistics  //ͳ����Ϣ
 *[��Ϣ����]
 *    MtService => UI
 */_event ( ev_CallInfoStatisticsInd )
  _body ( TCallInfoStatistics, 1 )
  _ev_end 

/***********************<< ͨ��״̬�ı� >>************************	
 *[��Ϣ��]		 
 * EmChanType 
 * BOOL       
 *[��Ϣ����]
 *    UI => MtService
 */_event ( ev_ChannelStatusChanged )
   _body ( EmChanType, 1 )
   _body ( BOOL, 1 )
  _ev_end

/***********************<< �����Ƿ����ָʾ >>************************	
 *[��Ϣ��]		 
 *  +BOOL  //�Ƿ����
 *[��Ϣ����]
 *    MtService => UI
 */_event ( ev_CallEncryptInd )
  _body ( BOOL, 1 )
  _ev_end 

/***********************<< ����˫��ָʾ  >>************************
 *[��Ϣ��]
 *  +BOOL             TRUE=��ʼ FALSE =ֹͣ
 *[��Ϣ����]
 *
 *    MtService <=> 323/320/sip
 */_event(  ev_RecvDualVideoInd   )
   _body ( BOOL            , 1 )//TRUE=��ʼ FALSE =ֹͣ
   _ev_end
   
/**********************<< mt ����mtc����ͳ����Ϣ >>********************	
 *[��Ϣ��]
 * ��
 *[��Ϣ����]
 *    Mt  = > MTC
 */_event( ev_MTCEncStatisReq  )
   _ev_end

/**********************<< mtc����ͳ����Ϣ����  >>********************	
 *[��Ϣ��]
 * TCodecPackStat  //ͳ����Ϣ
 *[��Ϣ����]
 *    Mt  = > MTC
 */_event( ev_MTCEncStatisAck  )
   _body( TCodecPackStat , 1 )
   _ev_end

 
/***********************<< �鲥ģʽ��ַָʾ  >>************************
 *[��Ϣ��]
 * +TIPTransAddr     ��Ƶ���յ�ַ
 * +TIPTransAddr     ��һ·��Ƶ���յ�ַ
 * +TIPTransAddr     �ڶ�·��Ƶ���յ�ַ
 *[��Ϣ����]
 *
 *    MtService <= 323/320/sip
 */_event(  ev_MulticastModeAddrInd   )
   _body ( TIPTransAddr       , 1 )//��Ƶ���յ�ַ
   _body ( TIPTransAddr       , 1 )//��һ·��Ƶ���յ�ַ
   _body ( TIPTransAddr       , 1 )//�ڶ�·��Ƶ���յ�ַ
   _ev_end

     
   /***********************<< ��DNS��ѯ����IP��ַ  >>************************	
 *[��Ϣ��]			 
 *  + u32 DNS Server IP
 *  + u32 dwUserId 
 *  + u16 �������� 
 *  + s8  ���� 
 
 *[��Ϣ����]
 *
 *    MtService => MtH323Service
 */_event( ev_DomainNameReq  )
   _body(  u32_ip    ,1             )
   _body(  u32       ,1             )
   _body(  u16       ,1             )
   _body(  s8        ,REMAIN             )
   _ev_end

    /***********************<< DNS��ѯ����IP��ַ��Ӧ  >>************************	
 *[��Ϣ��]			 
 *  + BOOL   ��ѯ���
    + u32_ip ������IP��ַ
 *
 *[��Ϣ����]
 *
 *    MtH323Service => MtService
 */_event( ev_DomainNameRsp  )
   _body(  BOOL          , 1             )
   _body(  u32_ip        , 1             )
   _ev_end

    /***********************<< ��ѯ������ʱ��  >>************************	
 *[��Ϣ��]			 
 *  + BOOL   ��ѯ���
    + u32_ip ������IP��ַ
 *
 *[��Ϣ����]
 *
 *    MtH323Service => MtService
 */_event( ev_DomainNameQueryTimeOut  )
   _ev_end
   
 /***********************<< �ն�æ���޼�������ʱ�ܾ�����ָʾ  >>************************	
 *[��Ϣ��]			 
 * + u32_ip     1                        IP��ַ
   + s8         MT_MAX_H323ALIAS_LEN+1   ����
   + s8         MT_MAX_E164NUM_LEN+1     ����
   + u16        1                        ��������
 *
 *[��Ϣ����]
 *
 *    MtH323Service => MtService => MtUI
 */_event( ev_CallRejectWhenBusyInd  )
   _body( u32_ip      ,1      )
   _body( s8          ,MT_MAX_H323ALIAS_LEN+1)
   _body( s8          ,MT_MAX_E164NUM_LEN+1  )
   _body( u16         ,1      )
   _ev_end
   

/***********************<< �绰״ָ̬ʾ >********************** 
 *[��Ϣ��]    
 *  EmFxoState
 *[��Ϣ����]
 *
 *   MtService => UI
 *   
 */_event(  ev_FxoCallLinkStateInd )
   _body ( EmFxoState, 1       )
   _ev_end

/***********************<< ��绰 >********************** 
 *[��Ϣ��]    
 *  s8*  �Է�����
 *[��Ϣ����]
 *
 *   MtService <= UI
 *   
 */_event(  ev_FxoMakeCall )
   _body ( s8, REMAIN       )
   _ev_end


/***********************<< �Ҷϵ绰 >********************** 
 *[��Ϣ��]    
 *  s8*  �Է�����
 *[��Ϣ����]
 *
 *   MtService <= UI
 *   
 */_event(  ev_FxoHangup )
   _ev_end


/***********************<< �ܽӵ绰 >********************** 
 *[��Ϣ��]    
 *[��Ϣ����]
 *
 *   MtService <= UI
 *   
 */_event(  ev_FxoReject )
   _ev_end
         
   
/***********************<< �绰����ָʾ >********************** 
 *[��Ϣ��]    
 *  s8*  �Է�����
 *[��Ϣ����]
 *
 *   MtService => UI
 *   
 */_event(  ev_FxoCallIncomingInd )
   _body ( s8, REMAIN       )
   _ev_end
 
  
/***********************<< ���յ绰 >********************** 
 *[��Ϣ��]    
 *  s8*  �Է�����
 *[��Ϣ����]
 *
 *   MtService <= UI
 *   
 */_event( ev_FxoAccept )
   _ev_end

/***********************<< �绰��ʱ >********************** 
 *[��Ϣ��]    
 *  
 *[��Ϣ����]
 *
 *   MtService 
 *   
 */_event( ev_FxoSetupTimerInd )
   _ev_end

   
/***********************<< �Զ˲��ٵȴ����ӻ�����·��ʱ >********************** 
 *[��Ϣ��]    
 *  
 *[��Ϣ����]
 *
 *   MtService => MTUI
 *   
 */_event( ev_FxoPeerNoLongerWaitingInd )
   _ev_end

   
/***********************<< �绰������������ >********************** 
 *[��Ϣ��]    
 *  
 *[��Ϣ����]
 *
 *   MtService => MTUI
 *   
 */_event( ev_FxoDialOutErrorInd )
   _ev_end


#ifndef _MakeMtEventDescription_
   _ev_segment_end(mtcall)
};
#endif
#endif

