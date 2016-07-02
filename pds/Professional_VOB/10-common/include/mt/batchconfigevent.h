
#ifndef BATCHCONFIG_EVENT_H
#define BATCHCONFIG_EVENT_H

#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmBatchConfig
{
#endif
/***********************************************************/
/*                                                         */
/*               �����ļ�������                            */
/*                                                         */
/***********************************************************/

    _ev_segment( batchconfig )


//mod by xujinxing,2007-01-06
//osp��Ϣ�����ü���Ϣ��
/***********************<< MTB�����½MT >>********************	
 *[��Ϣ��]			 
 *   NULL 
 *[��Ϣ����]
 *    MTB => UE
 */_event(  ev_MTBLoginReq  )
   _ev_end 


/***********************<< MTȷ��MTB��½������Ϣ >>********************	
 *[��Ϣ��]			 
 *   NULL
 *[��Ϣ����]
 *    UE => MTB
 */_event(  ev_MTBLoginAck  )
   _ev_end 
      
   
/***********************<< MT�ܾ�MTB��½������Ϣ >>********************	
 *[��Ϣ��]			 
 *   NULL 
 *[��Ϣ����]
 *     UE => MTB
 */_event(  ev_MTBLoginNack )
   _ev_end 
  

/***********************<< MTB����MT�����ļ����� >>********************	
 *[��Ϣ��]			 
 *   + TfileTransfer  
 *   + �����ļ�����
 *[��Ϣ����]
 *     MTB =>MTService
 */_event(  ev_MTBUpdateCfgFileReq           )
   _body (  TFileTransfer         ,  1       ) 
   _body (  s8                    ,  REMAIN  )
   _ev_end    


/***********************<< MTȷ��MTB����MT�����ļ����� >>********************	
 *[��Ϣ��]			 
 *   + TfileTransfer  
 *[��Ϣ����]
 *     MTService => MTB
 */_event(  ev_MTBUpdateCfgFileAck      )
   _body (  TFileTransfer         ,  1  ) 
   _ev_end    
  
 
/***********************<< MT�ܾ�MTB����MT�����ļ����� >>********************	
 *[��Ϣ��]			 
 *   + TfileTransfer  
 *[��Ϣ����]
 *     MTService => MTB
 */_event(  ev_MTBUpdateCfgFileNack     )
   _body (  TFileTransfer         ,  1  ) 
   _ev_end     


/***********************<< MTB����MT�����ļ����� >>********************	
 *[��Ϣ��]			 
 * +TfileTransfer //�ļ�����ͷ��Ϣ
 *[��Ϣ����]
 *     MTB => MTService
 */_event(  ev_MTBDownloadCfgFileReq      )
   _body (  TFileTransfer           ,  1  )
   _ev_end


/***********************<< MTȷ��MTB���������ļ����� >>********************	
 *[��Ϣ��]			 
 * +TfileTransfer //�ļ�����ͷ��Ϣ
 *[��Ϣ����]
 *     MTService => MTB
 */_event(  ev_MTBDownloadCfgFileAck      )
   _body (  TFileTransfer           ,  1  )
   _ev_end


/***********************<< MT�ܾ�MTB���������ļ����� >>********************	
 *[��Ϣ��]			 
 * +TfileTransfer //�ļ�����ͷ��Ϣ
 *[��Ϣ����]
 *     MTService => MTB
 */_event(  ev_MTBDownloadCfgFileNack     )
   _body (  TFileTransfer           ,  1  )
   _ev_end
   

/***********************<< MT��MTB���������ļ� >>********************	
 *[��Ϣ��]			 
 * + TfileTransfer //�ļ�����ͷ��Ϣ
 * + �����ļ�����
 *[��Ϣ����]
 *     MTService => MTB
 *
 */_event(  ev_MTBDownloadCfgFileNotify        )
   _body (  TFileTransfer           ,  1       )
   _body (  s8                      ,  REMAIN  )
   _ev_end

/***********************************************************/
/*                                                         */
/*               �ն�Ӳ���Զ���������Ϣ                    */
/*                                                         */
/***********************************************************/

/***********************<< Mt Auto Test UI�����½MT >>********************	
 *[��Ϣ��]			 
 *   NULL 
 *[��Ϣ����]
 *    Mt Auto Test UI => UE
 */_event(  ev_MTAutoTestLoginReq  )
   _ev_end 

/***********************<< ȷ��Mt Auto Test UI��½������Ϣ >>********************	
 *[��Ϣ��]			 
 *   NULL 
 *[��Ϣ����]
 *    UE => Mt Auto Test UI
 */_event(  ev_MTAutoTestLoginAck  )
   _ev_end

/***********************<< �ܾ�Mt Auto Test UI��½������Ϣ >>********************	
 *[��Ϣ��]			 
 *   NULL 
 *[��Ϣ����]
 *    UE => Mt Auto Test UI
 */_event(  ev_MTAutoTestLoginNack  )
   _ev_end

/***********************<< Mt Auto Test UI�������� >>********************	
 *[��Ϣ��]			 
 *   TMtAutoTestMsg  //�Զ�������ͷ��Ϣ
 *[��Ϣ����]
 *    Mt Auto Test UI => UE
 */_event(  ev_MTAutoTestCmd     )
   _ev_end

/***********************<< Mt Auto Test UI ���Խ��֪ͨ >>********************	
 *[��Ϣ��]			 
 *   TMtAutoTestMsg //�Զ�������ͷ��Ϣ
 *[��Ϣ����]
 *    UE => Mt Auto Test UI
 */_event(  ev_MTAutoTestNotify  )
   _ev_end

/***********************<< Mt Auto Test UI ����Ĭ��ֵ���� >>********************	
 *[��Ϣ��]			 
 *   ��
 *[��Ϣ����]
 *    Mt Auto Test UI => UE
 */_event(  ev_MTAutoTestSetDefaultCmd  )
   _ev_end

/***********************<< �ָ�Ĭ��ֵ���֪ͨ >>********************	
 *[��Ϣ��]			 
 *   ��
 *[��Ϣ����]
 *    UE => Mt Auto Test UI
 */_event(  ev_MTAutoTestSetDefaultNotify  )
   _ev_end

/***********************<< ���ز�������OSD�˵���Ϣ >>********************	
 *[��Ϣ��]			 
 *   ��
 *[��Ϣ����]
 *    UE => UE
 */_event(  ev_MTAutoTestCloseAllWindowCmd  )
   _ev_end

/***********************<< �ļ�������Ϣ >>********************	
 *[��Ϣ��]			 
 *   ��
 *[��Ϣ����]
 *    UE => UE
 */_event(  ev_MTAutoTestLoadFileCmd  )
   _ev_end   

/***********************<< �ļ����سɹ���Ϣ >>********************	
 *[��Ϣ��]			 
 *   ��
 *[��Ϣ����]
 *    UE => UE
 */_event(  ev_MTAutoTestLoadFileInd  )
   _ev_end   

#ifndef _MakeMtEventDescription_
   _ev_segment_end( batchconfig )
};
#endif

#endif
