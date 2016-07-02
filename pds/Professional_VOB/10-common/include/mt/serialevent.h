#ifndef SERIAL_EVENT_H
#define SERIAL_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmSerial
{
#endif
  _ev_segment( serial)//����ͷ��Ϣ


/*********************<< ���ô��ڲ���  >>****************************	
 *[��Ϣ��]			 
 *    + EmSerialType     232/422/485
 *    + TSerialCfg
 *
 *[��Ϣ����]
 *
 *   UI          => MtService
 *   MtService   => MtDevice
 */_event(  ev_SerialPortConfigCmd  )
   _body ( EmSerialType ,1 )
   _body ( TSerialCfg   ,1 )
   _ev_end
       


/*********************<< ���ڲ���ָʾ  >>****************************	
 *[��Ϣ��]			 
 *    + EmSerialType     232/422/485
 *    + TSerialCfg
 *
 *[��Ϣ����]
 *
 *   MtService   => UI
 */_event(  ev_SerialPortConfigInd  )
   _body ( EmSerialType ,1 )
   _body ( TSerialCfg   ,1 )
   _ev_end


/*********************<< ����͸����������  >>****************************	
 *[��Ϣ��]			 
 *    + TTransparentSerial
 *[��Ϣ����]
 *
 *   MtService   => UI
 */_event(  ev_TransparentSerialDataCmd )
   _body ( TTransparentSerial ,1 )
   _ev_end
   

/*********************<< ����͸����������  >>****************************	
 *[��Ϣ��]			 
 *    + TTransparentSerial
 *[��Ϣ����]
 *
 *   MtService   => UI
 */_event(  ev_TransparentSerialDataInd)
   _body ( TTransparentSerial ,1 )
   _ev_end
   
/*********************<< �ָ�����Ĭ��ֵ  >>****************************	
 *[��Ϣ��]			 
 *    + EmSerialType     232/422/485
 *
 *[��Ϣ����]
 *
 *   UI          => MtService
 *   MtService   => MtDevice
 */_event(  ev_SeriesDefaultConfigCmd   )
   _body ( EmSerialType ,1 )
   _ev_end
     
 

/*********************<< ����Iray  >>****************************	
 *[��Ϣ��]			 
 *    + u8 ��ַ 
 *
 *[��Ϣ����]
 *   MtService   => MtDevice
 */_event(  ev_IrayStartCmd   )
   _ev_end
#ifndef _MakeMtEventDescription_
   _ev_segment_end(serial)
};
#endif


#endif


