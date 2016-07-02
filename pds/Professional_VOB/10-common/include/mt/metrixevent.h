#ifndef MATRIX_EVENT_H
#define MATRIX_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmMatrix
{
#endif 
_ev_segment( matrix)//���þ�����Ϣ


/*********************<< ������ƵԴ��Ϣ����  >>*********************	
 *[��Ϣ��]			 
*TVideoSourceInfo
 *[��Ϣ����]
 *
 *  UI=> MtService => MtDevice
 */_event(  ev_SetVideoSourceInfoCmd  )
   _body (  TVideoSourceInfo ,1            ) 
   _ev_end
/*********************<< ������ƵԴ��Ϣָʾ  >>*********************	
 *[��Ϣ��]			 
*TVideoSourceInfo
 *[��Ϣ����]
 *
 *  UI<= MtService <= MtDevice
 */_event(  ev_VideoSourceInfoInd  )
	_body (  TVideoSourceInfo ,1            ) 
    _ev_end
   
/*********************<< ��ȡ������ƵԴ��Ϣ����  >>*********************	
 *[��Ϣ��]			 
 *��
 *[��Ϣ����]
 *
 *  UI => MtService => MtDevice
 */_event(  ev_GetAllVideoSourceInfoCmd  )
   _ev_end   
/*********************<< ����������ƵԴ��Ϣָʾ  >>*********************	
 *[��Ϣ��]			 
*TVideoSourceInfo
 *[��Ϣ����]
 *
 *  UI<= MtService <= MtDevice
 */_event(  ev_AllVideoSourceInfoInd  )
   _body (  TVideoSourceInfo ,REMAIN     ) 
   _ev_end

/*********************<< ��չ��ƵԴ����ָʾ  >>*********************	
 *[��Ϣ��]			 
*u8  byExtendVideoCapacity 
 *[��Ϣ����]
 *
 *  UI<= MtService <= MtDevice
 */_event(  ev_ExtendVideoSourceCapacityInd  )
   _body ( u8 ,1           ) 
   _ev_end   
/*********************<< ��ȡ�ն�֧���ⲿ������������  >>*********************	
 *[��Ϣ��]			 
 *  ��
 *
 *[��Ϣ����]
 *
 *   MtService =>MtDevice
 */_event(  ev_InnerMatrixGetConfigCmd   )
   _ev_end


/*********************<< �ն��ⲿ��������ָʾ  >>*********************	
 *[��Ϣ��]			 
 * + EmSerialType  emCtrlType; //��������
 * + TIPTransAddr  tIPAddr;    //���ڿ���ʱ��ַ
 *
 *[��Ϣ����]
 *
 *   MtService <= MtDevice
 */_event(  ev_InnerMatrixConfigInd   )
   _body (  EmSerialType  ,1             )
   _body (  TIPTransAddr  ,1             )
   _ev_end


/*********************<< �ն��ⲿ��������  >>*********************	
 *[��Ϣ��]			 
 * + EmSerialType  emCtrlType; //��������
 * + TIPTransAddr  tIPAddr;    //���ڿ���ʱ��ַ
 *
 *[��Ϣ����]
 *
 *   MtService => MtDevice
 */_event(  ev_InnerMatrixConfigCmd   )
   _body (  EmSerialType  ,1             )
   _body (  TIPTransAddr  ,1             )
   _ev_end

 
 /*********************<< ��ȡ�ն�֧���ⲿ������������  >>*********************	
 *[��Ϣ��]			 
 *  ��
 *
 *[��Ϣ����]
 *
 *   MtService =>MtDevice
 */_event(  ev_ExternalMatrixGetConfigCmd   )
   _ev_end


/*********************<< �ն��ⲿ��������ָʾ  >>*********************	
 *[��Ϣ��]			 
 * + EmSerialType  emCtrlType; //��������
 * + TIPTransAddr  tIPAddr;    //���ڿ���ʱ��ַ
 *
 *[��Ϣ����]
 *
 *   MtService <= MtDevice
 */_event(  ev_ExternalMatrixConfigInd   )
   _body (  EmSerialType  ,1             )
   _body (  TIPTransAddr  ,1             )
   _ev_end


/*********************<< �ն��ⲿ��������  >>*********************	
 *[��Ϣ��]			 
 * + EmSerialType  emCtrlType; //��������
 * + TIPTransAddr  tIPAddr;    //���ڿ���ʱ��ַ
 *
 *[��Ϣ����]
 *
 *   MtService => MtDevice
 */_event(  ev_ExternalMatrixConfigCmd   )
   _body (  EmSerialType  ,1             )
   _body (  TIPTransAddr  ,1             )
   _ev_end


/*********************<< ��ȡ���󷽰�������������  >>*********************	
 *[��Ϣ��]		
 *    
 *    
 *[��Ϣ����]
 *
 *   MtService => MtDevice
 */_event(  ev_InnerMatrixGetAllSchemesCmd  )
   _ev_end


 /*********************<< ��ȡ���󷽰���������ָʾ  >>*********************	
 *[��Ϣ��]			 
 *    + u8( ������ ) + TAVInnerMatrixScheme[ ]
 *[��Ϣ����]
 *
 *   MtService <= MtDevice
 */_event(  ev_InnerMatrixGetAllSchemesInd  )
   _body (  u8  ,1       )
   _body (  TAVInnerMatrixScheme  ,REMAIN      )
   _ev_end


 /*********************<< ��ȡ���󷽰�һ����������  >>*********************	
 *[��Ϣ��]		
 *    + u8 ( 0 ΪĬ�Ϸ��� )       ����������( 0 - 6 )
 *[��Ϣ����]
 *
 *   MtService => MtDevice
 */_event(  ev_InnerMatrixGetOneSchemeCmd  )
   _body (  u8  ,1       )
   _ev_end


 /*********************<< ��ȡ���󷽰�һ������ָʾ  >>*********************	
 *[��Ϣ��]			 
 *    TAVInnerMatrixScheme
 *[��Ϣ����]
 *
 *   MtService <= MtDevice
 */_event(  ev_InnerMatrixGetOneSchemeInd  )
   _body ( TAVInnerMatrixScheme  ,1       )
   _ev_end


/*********************<< ���淽��  >>*********************	
 *[��Ϣ��]			 
 *    +  TAVInnerMatrixScheme
 *	   
 *[��Ϣ����]
 *
 *   MtService => MtDevice
 */_event(  ev_InnerMatrixSaveSchemeCmd   )
   _body ( TAVInnerMatrixScheme  ,1       )
   _ev_end


 /*********************<< ���淽��ָʾ  >>*********************	
 *[��Ϣ��]			 
 *    +  TAVInnerMatrixScheme
 *	   
 *[��Ϣ����]
 *
 *   MtService <= MtDevice
 */_event(  ev_InnerMatrixSaveSchemeInd   )
   _body ( TAVInnerMatrixScheme  ,1       )
   _ev_end


/*********************<< ����ָ������Ϊ��ǰ����  >>*********************	
 *[��Ϣ��]			 
 *
 *	  +  u8  ��������
 *[��Ϣ����]
 *
 *   MtService => MtDevice
 */_event(  ev_InnerMatrixSetCurSchemeCmd   )
   _body ( u8         ,1           )
   _ev_end


 /*********************<< ����ָ������Ϊ��ǰ����ָʾ  >>*********************	
 *[��Ϣ��]			 
 *
 *	  +  u8  ��������
 *[��Ϣ����]
 *
 *   MtService <= MtDevice
 */_event(  ev_InnerMatrixSetCurSchemeInd   )
   _body ( u8         ,1           )
   _ev_end


 /*********************<< ��ȡ��ǰ��������  >>*********************	
 *[��Ϣ��]			 
 *    
 *[��Ϣ����]
 *
 *   MtService => MtDevice
 */_event(  ev_InnerMatrixCurSchemeCmd   )
   _ev_end


/*********************<< ��ȡ��ǰ��������ָʾ  >>*********************	
 *[��Ϣ��]			 
 *    +  u8  ��������
 *[��Ϣ����]
 *
 *   MtService <= MtDevice
 */_event(  ev_InnerMatrixCurSchemeInd   )
   _body ( u8         ,1           )
   _ev_end


/*********************<< �����ⲿ����ű��ļ�  >>*********************	
 *[��Ϣ��]			 
 *    +  u8  FileName[ 100 ]
 *[��Ϣ����]
 *
 *   MtService => MtDevice
 */_event(  ev_ExternalMatrixChangeCommandCmd   )
    _body ( u8         ,REMAIN                  )
   _ev_end


/*********************<< �ⲿ�����л�����  >>*********************	
 *[��Ϣ��]			 
 *    +  u8  ( ����˿ں�)
 *[��Ϣ����]
 *
 *   MtService => MtDevice
 */_event(  ev_ExternalMatrixConnectionCmd   )
   _body ( u8         ,1           )
   _ev_end

/*********************<< �ⲿ�����л�����ָʾ  >>*********************	
 *[��Ϣ��]			 
 *    +  u8  ( ����˿ں�)
 *[��Ϣ����]
 *
 *   UI / MTC / MCU <=MtService
 */_event(  ev_ExternalMatrixConnectionInd   )
   _body ( u8         ,1           )
   _ev_end


/*********************<< ��ȡ�ⲿ�����л���������  >>*********************	
 *[��Ϣ��]			 
 *    ��
 *[��Ϣ����]
 *
 *   MtService => MtDevice
 */_event(  ev_ExternalMatrixGetConnectionCmd   )
   _ev_end

/*********************<< ��ȡ�ⲿ�����л�����ָʾ  >>*********************	
 *[��Ϣ��]			 
 *    +  u8  ( ����˿ں�)
 *[��Ϣ����]
 *
 *   MtService <= MtDevice
 */_event(  ev_ExternalMatrixGetConnectionInd   )
   _body ( u8         ,1           )
   _ev_end


/*********************<< ��ȡ�ⲿ������Ϣ����  >>*********************	
 *[��Ϣ��]			 
 *    ��
 *[��Ϣ����]
 *
 *   MtService => MtDevice
 */_event(  ev_ExternalMatrixInfoCmd   )
   _ev_end

/*********************<< ��ȡ�ⲿ������Ϣָʾ  >>*********************	
 *[��Ϣ��]			 
 *    TExternalMatrixInfo
 *[��Ϣ����]
 *
 *   MtService <= MtDevice
 */_event(  ev_ExternalMatrixInfoInd   )
   _body (  TExternalMatrixInfo         ,1            ) 
   _ev_end

/*********************<< �ⲿ����˿�������  >>*********************	
 *[��Ϣ��]			 
 *    +u8 ( 0 - 64 / 0�����������������)
 *    + s8 [ MT_MAX_AVMATRIX_MODEL_NAME + 1 ]
 * 
 *[��Ϣ����]
 *
 *   MtService => MtDevice
 */_event(  ev_ExternalMatrixSavePortCmd   )
   _body (  u8         ,REMAIN            ) 
   _ev_end

/*********************<< �ⲿ����˿�������ָʾ  >>*********************	
 *[��Ϣ��]			 
 *    +u8 ( 0 - 64 / 0�����������������)
 *    + s8 [ MT_MAX_AVMATRIX_MODEL_NAME + 1 ]
 *[��Ϣ����]
 *
 *   UI / MTC / MCU <=MtService
 */_event(  ev_ExternalMatrixSavePortInd   )
   _body (  u8         ,REMAIN            ) 
   _ev_end

/*********************<< ��ȡ�ⲿ����˿���  >>*********************	
 *[��Ϣ��]			 
 *    ��
 *[��Ϣ����]
 *
 *   MtService => MtDevice
 */_event(  ev_ExternalMatrixGetPortCmd   )
   _ev_end

/*********************<< ��ȡ�ⲿ����˿���ָʾ  >>*********************	
 *[��Ϣ��]			 
 *    s8 [ ( MT_MAX_PORTNAME_LEN + 1 ) * ( 64 + 1 ) ]
 * [ 0 ]: Outport Name / [ 1] to [ 64 ] Inport Name 
 *[��Ϣ����]
 *
 *   MtService <= MtDevice
 */_event(  ev_ExternalMatrixGetPortInd   )
   _body (  u8         ,REMAIN            ) 
   _ev_end
   
//xjx_071221, �����ն��µ���Ƶ������Ϣ
 
/*********************<< HD�ն�,��ȡ���󷽰���������ָʾ  >>*********************	
 *[��Ϣ��]			 
 *    + u8( ������ ) + THDAVInnerMatrixScheme[ ]
 *[��Ϣ����]
 *
 *   MtService <= MtDevice
 */_event(  ev_HDInnerMatrixGetAllSchemesInd  )
   _body (  u8  ,1       )
   _body (  THDAVInnerMatrixScheme  ,REMAIN      )
   _ev_end

/*********************<< HD�ն�,��ȡ���󷽰�һ������ָʾ  >>*********************	
 *[��Ϣ��]			 
 *    THDAVInnerMatrixScheme
 *[��Ϣ����]
 *
 *   MtService <= MtDevice
 */_event(  ev_HDInnerMatrixGetOneSchemeInd  )
   _body ( THDAVInnerMatrixScheme  ,1       )
   _ev_end


/*********************<< HD�ն�,���淽��  >>*********************	
 *[��Ϣ��]			 
 *    +  THDAVInnerMatrixScheme
 *	   
 *[��Ϣ����]
 *
 *   MtService => MtDevice
 */_event(  ev_HDInnerMatrixSaveSchemeCmd   )
   _body ( THDAVInnerMatrixScheme  ,1       )
   _ev_end


 /*********************<< HD�ն�,���淽��ָʾ  >>*********************	
 *[��Ϣ��]			 
 *    +  THDAVInnerMatrixScheme
 *	   
 *[��Ϣ����]
 *
 *   MtService <= MtDevice
 */_event(  ev_HDInnerMatrixSaveSchemeInd   )
   _body ( THDAVInnerMatrixScheme  ,1       )
   _ev_end


#ifndef _MakeMtEventDescription_
   _ev_segment_end(matrix)
};
#endif  

#endif

