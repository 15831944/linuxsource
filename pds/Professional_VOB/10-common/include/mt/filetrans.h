#ifndef FILE_TRANS_H
#define FILE_TRANS_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmFileTrans
{
#endif
/***********************************************************/
/*                                                         */
/*                 �ļ�������Ϣ                            */
/*    CMtMsg ��Ϣ                                             */
/***********************************************************/

   _ev_segment( file_trans )

/***********************<< MTC�������ļ� >>********************	
 *[��Ϣ��]			 
 *   + TFileTransfer
 *[��Ϣ����]
 *    MTC => UE
 */_event( ev_FTSendReq                     )
   _body ( TFileTransfer    ,1              )
   _ev_end


/***********************<< MTC�������ļ�ȷ�� >>********************	
 *[��Ϣ��]			 
 *   + TFileTransfer
 *[��Ϣ����]
 *    MTC => UE
 */_event(  ev_FTSendACK  )
   _body ( TFileTransfer    ,1    )
   _ev_end


 /***********************<< MTC�������ļ��ܾ� >>********************	
 *[��Ϣ��]			 
 *   + TFileTransfer
 *[��Ϣ����]
 *    MTC => UE
 */_event(  ev_FTSendNACK  )
   _body ( TFileTransfer    ,1    )
   _ev_end


 /***********************<< MTC��ʼ�����ļ� >>********************	
 *[��Ϣ��]			 
 *   + TFileTransfer
 *   + �ļ�����
 *[��Ϣ����]
 *    MTC => UE
 */_event(  ev_FTSendFileCmd  )
   _body ( TFileTransfer    ,1         )
   _body ( s8               ,REMAIN    )
   _ev_end



 /***********************<< MTC��ʼ�����ļ�Ӧ�� >>********************	
 *[��Ϣ��]			 
 *   + TFileTransfer
 *[��Ϣ����]
 *    MTC => UE
 */_event(  ev_FTSendFileRsp  )
   _body ( TFileTransfer    ,1         )
   _ev_end


	
 /***********************<< MTC�����ļ����֪ͨ >>********************	
 *[��Ϣ��]			 
 *   + TFileTransfer
 *[��Ϣ����]
 *    MTC => UE
 */_event(  ev_FTSendCompleteInd  )
   _body ( TFileTransfer    ,1         )
   _ev_end

	
 /***********************<< MTC�����ļ���ϱ��� >>********************	
 *[��Ϣ��]			 
 *   + TFileTransfer
 *[��Ϣ����]
 *    MTC => UE
 */_event(  ev_FTSendCompleteRsp  )
   _body ( TFileTransfer    ,1         )
   _ev_end

	
 /***********************<< MTC�������ļ� >>********************	
 *[��Ϣ��]			 
 *   + TFileTransfer
 *[��Ϣ����]
 *    MTC => UE
 */_event(  ev_FTTimeout  )
   _body ( TFileTransfer    ,1         )
   _ev_end

/**********************<< ���ú������  >>********************	
 *[��Ϣ��]	
 *TBannerInfo info //�������
 *[��Ϣ����]
 *    MTC => UE
 */_event( ev_BannerInfoCmd)
   _body ( TBannerInfo ,1)
   _ev_end    
/**********************<< �������֪ͨ  >>********************	
 *[��Ϣ��]	
 *TBannerInfo info //�������
 *[��Ϣ����]
 *    MTC => UE
 */_event( ev_BannerInfoInd)
   _body ( TBannerInfo ,1)
   _ev_end       
/**********************<< ̨�����֪ͨ  >>********************	
 *[��Ϣ��]	
 * NULL
 *[��Ϣ����]
 *    UI <= UE
 */_event( ev_UpdateLabel)
   _ev_end       
/**********************<< �������֪ͨ  >>********************	
 *[��Ϣ��]	
 *NULL
 *[��Ϣ����]
 *    UI <= UE
 */_event( ev_UpdateBanner)
   _ev_end     
#ifndef _MakeMtEventDescription_
   _ev_segment_end(file_trans)
};
#endif	

#endif

