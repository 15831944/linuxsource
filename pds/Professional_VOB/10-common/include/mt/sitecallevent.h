#ifndef SITECALL_EVENT_H
#define SITECALL_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EMSitecall
{
#endif 
	   _ev_segment( sitecall )
/***********************<< ��ʼ�����к��������************************	
 *[��Ϣ��]			 
 *    + THANDLE  ���
 *
 *[��Ϣ����]
 *
 *   Service => StackOut
 */_event( ev_SiteCallSetGKAddrCmd    )
 	_body(u32_ip ,1)
   _ev_end
/***********************<< ��ʼ�����к�����Ӧ����************************	
 *[��Ϣ��]			 
 *    + BOOL
 *[��Ϣ����]
 *
 *   Service <= StackOut
 */_event( ev_SiteCallSetGKAddrInd   )
   _body(BOOL , 1)
   _ev_end
/***********************<< �������к�������>>************************	
 *[��Ϣ��]			 
 *    + NULL
 *
 *[��Ϣ����]
 *
 *   Service => StackOut
 */_event( ev_SiteCallCreateCmd    )
   _body(TSitecallInformation , 1)
   _ev_end
/***********************<< �������к�����Ӧ>>************************	
 *[��Ϣ��]			 
 *    + BOOL
 *
 *[��Ϣ����]
 *
 *   Service <= StackOut
 */_event( ev_SiteCallCreateInd    )
   _body(u32 , 1)
   _ev_end



_event(  ev_SiteCallResultInd  )
   _body( u32        ,1 )
_ev_end
   
/***********************<< �յ�һ�����к���>>************************	
 *[��Ϣ��]			 
 *    + NULL
 *
 *[��Ϣ����]
 *
 *   Service <= StackIn
 */_event( ev_SiteCallIncoming    )
   _body(BOOL , 1)
   _ev_end

/***********************<< �ر����к�������>>************************	
 *[��Ϣ��]			 
 *    +NULL
 *
 *[��Ϣ����]
 *
 *   Service => StackOut
 */_event( ev_SiteCallDropCmd    )
   _ev_end
/***********************<< �ر����к�����Ӧ>>************************	
 *[��Ϣ��]			 
 *    + NULL
 *
 *[��Ϣ����]
 *
 *   Service <= StackOut
 */_event( ev_SiteCallDropInd    )
   _ev_end
/***********************<< ж�����к�������>>************************	
 *[��Ϣ��]			 
 *    + NULL
 *
 *[��Ϣ����]
 *
 *   Service => StackOut
 */_event( ev_SiteCallUninstallCmd    )
   _ev_end
/***********************<< ж�����к�����Ӧ>>************************	
 *[��Ϣ��]			 
 *    + NULL
 *
 *[��Ϣ����]
 *
 *   Service <= StackOut
 */_event( ev_SiteCallUninstallDropInd    )
   _ev_end
   
/***********************<< ��DNS��ѯ����IP��ַ  >>************************	
 *[��Ϣ��]			 
 *  + u32 DNS Server IP
 *  + u32 dwUserId 
 *  + u16 �������� 
 *  + s8  ���� 
 *[��Ϣ����]
 *
 *    MtH323Service => StatckIn
 */_event( ev_h323DomainNameReq  )
   _body(  u32_ip    ,1             )
   _body(  u32       ,1             )
   _body(  u16       ,1             )
   _body(  s8        ,REMAIN             )
   _ev_end

    /***********************<< DNS��ѯ����IP��ַ��Ӧ  >>************************	
 *[��Ϣ��]			 
 *  + u8
 *
 *[��Ϣ����]
 *
 *    MtH323Service => StatckIn
 */_event( ev_h323DomainNameRsp  )
   _body(  BOOL          , 1             )
   _body(  u32_ip        , 1             )
   _ev_end
#ifndef _MakeMtEventDescription_
   _ev_segment_end(sitecall)
};
  

inline BOOL IsSitecallEvent(u16 wEvent)
{
	return (wEvent > get_first_event_val(sitecall)) && (wEvent < get_last_event_val(sitecall));
}  
#endif //_MakeMtEventDescription_
#endif //SITECALL_EVENT_H


