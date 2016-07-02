#ifndef CAMERA_EVENT_H
#define CAMERA_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmCamera
{
#endif 
     _ev_segment( camera)//����ͷ��Ϣ
	
/*********************<< �ն�����ͷ����Դ  >>*********************	
 *[��Ϣ��]			 
 *    EmSite
 *
 *[��Ϣ����]
 *
 *   MtService <= UI
 */_event(  ev_CameraControlSourceCmd   )
   _body (  EmSite        ,1         ) 
   _ev_end

/*********************<< �ն�����ͷ����Դָʾ  >>*********************	
 *[��Ϣ��]			 
 *    EmSite
 *
 *[��Ϣ����]
 *
 *   MtService <= UI
 */_event(  ev_CameraControlSourceInd   )
   _body (  EmSite        ,1         ) 
   _ev_end


/*********************<< �ն�֧������ͷ�������  >>*********************	
 *[��Ϣ��]			 
 *    + TCameraTypeInfo[]  ����ͷ�����б�
 *
 *[��Ϣ����]
 *
 *   MtService =>UI
 */_event(  ev_CameraTypeCapsetInd   )
   _body ( TCameraTypeInfo        ,REMAIN ) 
   _ev_end



/***********************<< ����ͷ״ָ̬ʾ  >>************************	
 *[��Ϣ��]			 
 *    + BOOL[MT_MAX_CAMERA_NUM]   TRUE= ���ӳɹ� FALSE =δ���ӳɹ�
 *
 *[��Ϣ����]
 *    MtDevice => MtService
 *
 *   MtService => UI
 */_event(  ev_CameraStatusInd     )
   _body ( BOOL        ,MT_MAX_CAMERA_NUM )
   _ev_end



/***********************<< ѡ���������ͷ����  >>************************	
 *[��Ϣ��]			 
 *    + u8  ����ͷ���� 1-6
 *
 *[��Ϣ����]
 *
 *   MtService => MtDevice
 */_event(  ev_CameraSelectCmd   )
   _body(   u8         ,       1 )
   _ev_end



 /***********************<< ѡ���������ͷ����  >>************************	
 *[��Ϣ��]			 
 *    +TMtId ( Source )
 *    +TMtId ( Dest ) 
 *    + u8  ����ͷ���� 1-6
 *
 *[��Ϣ����]
 *
 *   MtService => MtDevice
 */_event(  ev_FeccCameraSelectCmd   )
   _body ( TMtId        ,1 )
   _body ( TMtId        ,1 )
   _body ( u8           ,1 )
   _ev_end


/***********************<< ����ͷ�ƶ�����  >>************************	
 *[��Ϣ��]			  
 *    + EmDirection
 *    + EmAction      emStart/emStop
 *
 *[��Ϣ����]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_CameraPantiltCmd	  )
   _body ( EmDirection  ,1 )
   _body ( EmAction     ,1 )
   _ev_end



 /***********************<< ����ͷ�ƶ�����  >>************************	
 *[��Ϣ��]			 
 *    +TMtId ( Source )
 *    +TMtId ( Dest ) 
 *    + EmDirection
 *    + EmAction      emStart/emStop
 *
 *[��Ϣ����]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_FeccCameraPantiltCmd	  )
   _body ( TMtId        ,1 )
   _body ( TMtId        ,1 )
   _body ( EmDirection  ,1 )
   _body ( EmAction     ,1 )
   _ev_end


/***********************<< ����ͷ�۽�����  >>************************	
 *[��Ϣ��]			 
 *    + BOOL      TRUE=���� FALSE=Զ��
 *   + EmAction      emStart/emStop
 *
 *[��Ϣ����]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_CameraFocusCmd   )
   _body ( BOOL         ,1 )
   _body ( EmAction     ,1 )
   _ev_end
 

  /***********************<< ����ͷ�۽�����  >>************************	
 *[��Ϣ��]			 
 *    +TMtId ( Source )
 *    +TMtId ( Dest ) 
 *	  + BOOL      TRUE=���� FALSE=Զ��
 *   + EmAction      emStart/emStop
 *
 *[��Ϣ����]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_FeccCameraFocusCmd   )
   _body ( TMtId        ,1 )
   _body ( TMtId        ,1 )
   _body ( BOOL         ,1 )
   _body ( EmAction     ,1 )
   _ev_end


/***********************<< ����ͷ�Զ��۽�����  >>**********************	
 *[��Ϣ��]			 
 *    ����Ϣ��
 *
 *[��Ϣ����]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_CameraAutoFocusCmd	  	  )
   _ev_end


/***********************<< ����ͷ�Զ��۽�����  >>**********************	
 *[��Ϣ��]			 
 *    +TMtId ( Source )
 *    +TMtId ( Dest ) 
 *
 *[��Ϣ����]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_FeccCameraAutoFocusCmd	  	  )
   _body ( TMtId        ,1 )
   _body ( TMtId        ,1 )
   _ev_end

 /***********************<< ����ͷ�Զ����ⲹ��  >>************************	
 *[��Ϣ��]			 
 *    + BOOL      TRUE=�Զ����� FALSE=ȡ���Զ�����
  *
 *[��Ϣ����]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_CameraBackLightCmd   )
   _body ( BOOL         ,1 )
   _ev_end
 

/***********************<< ������Ұ��������  >>************************	
 *[��Ϣ��]			 
 *    + BOOL          TRUE= �Ŵ� /FALSE=��С
 *    + EmAction      emStart/emStop
 *
 *[��Ϣ����]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_CameraZoomCmd                )
   _body ( BOOL         ,1 )
   _body ( EmAction     ,1 )
   _ev_end


/***********************<< ������Ұ��������  >>************************	
 *[��Ϣ��]			 
 *    +TMtId ( Source )
 *    +TMtId ( Dest ) 
 *    + BOOL          TRUE= �Ŵ� /FALSE=��С
 *    + EmAction      emStart/emStop
 *
 *[��Ϣ����]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_FeccCameraZoomCmd                )
   _body ( TMtId        ,1 )
   _body ( TMtId        ,1 )
   _body ( BOOL         ,1 )
   _body ( EmAction     ,1 )
   _ev_end

/***********************<< �������ȵ�������  >>************************	
 *[��Ϣ��]			 
 *    + BOOL     TRUE= ����  FALSE=����
 *    + EmAction      emStart/emStop
 *
 *[��Ϣ����]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_CameraBrightCmd		  )
   _body ( BOOL         ,1 )
   _body ( EmAction     ,1 )
   _ev_end
 

/***********************<< �������ȵ�������  >>************************	
 *[��Ϣ��]			 
 *    +TMtId ( Source )
 *    +TMtId ( Dest ) 
 *    + BOOL     TRUE= ����  FALSE=����
 *    + EmAction      emStart/emStop
 *
 *[��Ϣ����]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_FeccCameraBrightCmd		  )
   _body ( TMtId        ,1 )
   _body ( TMtId        ,1 )
   _body ( BOOL         ,1 )
   _body ( EmAction     ,1 )
   _ev_end

 
 
/***********************<< Ԥ��λ����  >>************************	
 *[��Ϣ��]	
 *    + BOOL    TRUE= �洢 FALSE=����Ԥ��λ		 
 *    + u8      Ԥ��λ���� 1-16
 *
 *[��Ϣ����]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_CameraPresetCmd  		  )
   _body ( BOOL         ,1 )
   _body ( u8           ,1 )
   _ev_end

 

/***********************<< Ԥ��λ����  >>************************	
 *[��Ϣ��]	
 *    +TMtId ( Source )
 *    +TMtId ( Dest ) 
 *    +BOOL    TRUE= �洢 FALSE=����Ԥ��λ		 
 *    + u8      Ԥ��λ���� 1-16
 *
 *[��Ϣ����]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_FeccCameraPresetCmd  		  )
   _body ( TMtId        ,1 )
   _body ( TMtId        ,1 )
   _body ( BOOL         ,1 )
   _body ( u8           ,1 )
   _ev_end

 

/***********************<< ��ȡ����ͷ����  >>************************	
 *[��Ϣ��]	
 *    +TMtId ( Source )
 *    +TMtId ( Dest ) 
 *    +BOOL    TRUE= �洢 FALSE=����Ԥ��λ		 
 *    + u8      Ԥ��λ���� 1-16
 *
 *[��Ϣ����]
 *
 *   UI        => MtService
 *   MtService => MtDevice
 */_event(  ev_CameraGetCapCmd   )
   _body ( TMtId        ,1 )
   _body ( TMtId        ,1 )
   _body ( BOOL         ,1 )
   _body ( u8           ,1 )
   _ev_end


/***********************<< ��Ƶ�������� >>************************	
 *[��Ϣ��]			 
 * ��
 *[��Ϣ����]
 *    MtService <= UI
 */_event(  ev_ApplyAudioPowerCmd  )
   _ev_end

/***********************<< ��Ƶ����ָʾ >>************************	
 *[��Ϣ��]			 
 *  u32  ������Ƶ����
 *  u32  �������Ƶ����
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_AudioPowerInd  )
   _body ( u32    ,1          )
   _body ( u32    ,1          )
   _ev_end
#ifndef _MakeMtEventDescription_
   _ev_segment_end(camera)
};
#endif   

#endif

