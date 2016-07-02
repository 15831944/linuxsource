
#ifndef PCDUALVIDEO_EVENT_H
#define PCDUALVIDEO_EVENT_H

#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmPCDualVideo
{
#endif
/***********************************************************/
/*                                                         */
/*                 �û�������Ϣ                            */
/*    osp ��Ϣ                                             */
/***********************************************************/

    _ev_segment( pcdualvideo )

 
/***********************<< PC˫����������¼��Ϣ >>********************	
 *[��Ϣ��]			 
 *   + u8[32]         //ͨ�����ݽṹ�汾����
 *   + CLoginRequest
 *[��Ϣ����]
 *    PC˫����� => UE
 */_event(  ev_PCLoginReq   )
   _ev_end



/***********************<< PC˫����������¼ȷ��Ӧ�� >>********************	
 *[��Ϣ��]			 
 *   ����Ϣ��
 *[��Ϣ����]
 *   UE => PC˫����� 
 */_event(  ev_PCLoginAck   )
   _ev_end



 /***********************<< PC˫����������¼�ܾ�Ӧ�� >>********************	
 *[��Ϣ��]			 
 *   + u8[32]         //ͨ�����ݽṹ�汾����
 *   + u8             // ��½ʧ��ԭ��   0=�û�����ģ�� 1=�����û���½ 2������汾��ƥ��
 *   + u16            // �û�����ģ������
 *   + CUserFullInfo  //�ѵ�½�û���Ϣ
 *   + u32            //�ѵ�½�û�IP
 *[��Ϣ����]
 *   UE => PC˫����� 
 */_event(  ev_PCLoginNack   )
   _ev_end
   

 /***********************<< PC˫������ܷ���˫��ָʾ >>********************	
 *[��Ϣ��]			 
 *   + BOOL         //
 *[��Ϣ����]
 *   UE => PC˫����� 
 */_event(  ev_PCCanSendVideoInd   )
   _body( BOOL, 1 )
   _ev_end



/***********************<< �ն�״ָ̬ʾ >>********************	
 *[��Ϣ��]			 
 * + EmMtModel      //�ն��ͺ�
 * + BOOL          //�Ƿ��ڻ�����
 * + EmConfMode    //��������
 *[��Ϣ����]
 *   MT => PC˫����� 
 */_event(  ev_PCMtStatusInd   )
   _body( EmMtModel, 1 )
   _body( BOOL, 1 )
   _body( EmConfMode, 1)
   _ev_end
   
/***********************<< �ն�˫��״ָ̬ʾ >>********************	
 *[��Ϣ��]
 * + BOOL         //�Ƿ��ڷ���˫��
 * + BOOL         //�Ƿ��ڽ���˫��
 *[��Ϣ����]
 *   MT => PC˫����� 
 */_event(  ev_PCDualVideoStatusInd   )
   _body( BOOL, 1 )
   _body( BOOL, 1 )
   _ev_end



/***********************<< PC˫�������Ƶ�ֱ���ָʾ >>********************	
 *[��Ϣ��]			 
 * +EmVideoResolution
 *[��Ϣ����]
 *   MT <= PC˫����� 
 */_event(  ev_PCVideoResInd   )
   _body( EmVideoResolution, 1 )
   _ev_end


/***********************<< �ն�֧�ֵ���Ƶ�ֱ���ָʾ >>********************	
 *[��Ϣ��]
 * +EmVideoResolution
 *[��Ϣ����]
 *   MT => PC˫����� 
 */_event(  ev_PCVideoResMtCapInd )
   _body( EmVideoResolution, 1 )
   _ev_end

 /***********************<< pc˫�������ʼ/ֹͣ˫������  >>************************
 *[��Ϣ��] 
 *  +EmMtVideoPort    //emPC
 *  +BOOL             TRUE=��ʼ FALSE =ֹͣ
 *  +TIPTransAddr   
 *[��Ϣ����]
 *
 *  mtc =>  MtService 
 */_event(  ev_PCSepDualVideoCmd   )
   _body ( EmMtVideoPort   , 1 )//��Ƶ�˿�
   _body ( BOOL            , 1 )//TRUE=��ʼ FALSE =ֹͣ
   _body ( TIPTransAddr    , 1 ) //���������ؽ��յ�rtcp��ַ�Ͷ˿�
   _ev_end


#ifndef _MakeMtEventDescription_
   _ev_segment_end( pcdualvideo )
};
#endif

#endif