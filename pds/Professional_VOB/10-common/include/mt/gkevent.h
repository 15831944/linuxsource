#ifndef GK_EVENT_H
#define GK_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmGK
{
#endif 

  _ev_segment( gk  )

/***********************<< ע��GK���  >>************************
 *[��Ϣ��]			 
 *  + BOOL  TRUE = ע��ɹ� FALSEʧ��
 *
 *[��Ϣ����]
 *
 *    MtService => UI
 *
 */_event(  ev_GKRegistResultInd   )
   _body ( BOOL            , 1     )//TRUE = ע��ɹ� FALSEʧ��
   _ev_end


/***********************<< ע��GK  >>************************
 *[��Ϣ��]			 
 *  + u32      GK IP��ַ
 *  + u32      ע�᱾��IP
 *  + TMtAddr  E164
 *  + TMtAddr  H323id
 *[��Ϣ����]
 *    UI        => MtService 
 *    MtService => H323
 *
 */_event(  ev_GKRegistReq   )
   _body(  u32      ,  1     )//GK IP��ַ
   _body(  u32      ,  1     )//ע�᱾��IP
   _body(  TMtAddr  ,  1     )//E164
   _body(  TMtAddr  ,  1     )//H323id
   _ev_end


/***********************<< ע��GK  >>************************
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *    UI        => MtService 
 *    MtService => H323
 *
 */_event(  ev_GKUnregistReq   )
   _ev_end


#ifndef _MakeMtEventDescription_
   _ev_segment_end(gk)
};
#endif
#endif

