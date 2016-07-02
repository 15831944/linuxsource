#ifndef VOD_EVENT_H
#define VOD_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmVOD
{
#endif 

_ev_segment(vod )//VOD��Ϣ

/**********************<< ��½VODServer���� >>**************	
 *[��Ϣ��]			 
 *  + TVODUserInfo
 *[��Ϣ����]
 *
 *   UI  => MtService => VODServer
 *   
 */_event( ev_VODLoginCmd )
 _body( TVODUserInfo, 1 )
 _ev_end



 /**********************<< �˳�VODServer���� >>**************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *   UI  => MtService => VODServer
 *   
 */_event(  ev_VODLogoutCmd  )
   _ev_end




/**********************<< ��ȡ�ļ��б����� >>**************	
 *[��Ϣ��]			 
 *  + s8[ VOD_MAX_FILE_NAME_LENGTH ] // Ŀ¼�� 
 * [ Comment ]
 *	// ServiceDir + DirName = > VODServer
 *[��Ϣ����]
 *
 *   UI  => MtService = > VODServer
 *   
 */_event(  ev_VODGetFileListCmd  )
	_body( s8, VOD_MAX_FILE_NAME_LENGTH )
   _ev_end

/**********************<< ��ȡ�ļ��б�ָʾ >>**************	
 *[��Ϣ��]			 
 *  + TVODFile[]
 *[��Ϣ����]
 *
 *   UI  <= MtService <= VODServer
 *   
 */_event(  ev_VODGetFileListInd  )
   _body( TVODFile ,REMAIN )
   _ev_end



/**********************<< ��ȡ�ļ���Ϣ���� >>**************	
 *[��Ϣ��]			 
 *  + s8[ VOD_MAX_FILE_NAME_LENGTH ] // �ļ���
 * [ Comment ]
 *	
 *[��Ϣ����]
 *
 *   UI  => MtService => VODServer
 *   
 */_event(  ev_VODGetFileInfoCmd  )
   _body( s8, VOD_MAX_FILE_NAME_LENGTH )
   _ev_end


/**********************<< ��ȡ�ļ���Ϣָʾ >>**************	
 *[��Ϣ��]			 
 *  + TVODFile
 *[��Ϣ����]
 *
 *   UI  <= MtService <= VODServer
 *   
 */_event(  ev_VODGetFileInfoInd  )
  _body( TVODFile, 1 )
  _ev_end


/**********************<< �ļ��������� >>**************	
 *[��Ϣ��]			 
 *	u8[VOD_MAX_FILE_NAME_LENGTH] -�����ļ���
 *[��Ϣ����]
 *
 *   UI  => MtService => VODServer
 *   
 */_event(  ev_VODPlayFileCmd  )
	_body(  u8, VOD_MAX_FILE_NAME_LENGTH )
   _ev_end

/**********************<< ���Żָ����� >>**************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *   UI  => MtService => VODServer
 *   
 */_event(  ev_VODResumePlayCmd  )
   _ev_end


/**********************<< ������ת���� >>**************	
 *[��Ϣ��]			 
 *  u32 / ����ʱ�����
 *[��Ϣ����]
 *
 *   UI  => MtService => VODServer
 *   
 */_event(  ev_VODRandomPlayCmd  )
	_body( u32, 1 )
   _ev_end

/**********************<< ������ͣ���� >>**************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *   UI  => MtService => VODServer
 *   
 */_event(  ev_VODPausePlayCmd  )
   _ev_end

/**********************<< �ļ�ֹͣ�������� >>**************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *   UI  => MtService => VODServer
 *   
 */_event(  ev_VODStopPlayFileCmd  )
   _ev_end

/**********************<< VOD�ͻ���״̬֪ͨ >>**************	
 *[��Ϣ��]			 
 *  +EmVODState
 *[��Ϣ����]
 *
 *   UI  => MtService => VODServer
 *   
 */_event(  ev_VODStateInd  )
   _body( EmVODState ,1 )
   _ev_end

/**********************<< ������Ϣָ֪ͨʾ >>**************	
 *[��Ϣ��]			 
 *  + u32 dwDuration         //������ʱ��������
 *  + u32 dwPlayTime         //�Ѳ���ʱ�䣬����
 *[��Ϣ����]
 *
 *   UI  <= MtService <= VODServer
 *   
 */_event(  ev_VODPlayInfoNotificationInd  )
  _body( u32, 1 )
  _body( u32, 1 )
  _ev_end
 

#ifndef _MakeMtEventDescription_
   _ev_segment_end(vod)
};
#endif  
#endif

