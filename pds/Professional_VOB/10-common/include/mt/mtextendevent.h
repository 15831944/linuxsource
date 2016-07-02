#ifndef MT_EXTEND_EVENT_H
#define MT_EXTEND_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmMtExtend
{
#endif 
   _ev_segment( extend ) //��չ���ⲿ��Ϣ

/***********************<<  ����WiFi����������Ŀ���� >>********************	
 *[��Ϣ��]
 * + TWiFiBSSIDCfg , 1 //WiFi����������Ŀ���ýṹ
 *  
 *[��Ϣ����]
 *   UI => mtservice 
 */_event( ev_WiFiCfgAddItemCmd )
   _body( TWiFiBSSIDCfg, 1 )
   _ev_end


/***********************<<  ɾ��WiFi����������Ŀ���� >>********************	
 *[��Ϣ��]
 * + TWiFiBSSIDCfg , 1 //WiFi����������Ŀ���ýṹ
 *  
 *[��Ϣ����]
 *   UI => mtservice 
 */_event( ev_WiFiCfgDelItemCmd )
   _body( TWiFiBSSIDCfg, 1 )
   _ev_end
   
/***********************<<  �޸�WiFi����������Ŀ���� >>********************	
 *[��Ϣ��]
 * + u32 , 1 //��Ŀ���� 
 * + TWiFiBSSIDCfg , 1 //WiFi����������Ŀ���ýṹ
 *  
 *[��Ϣ����]
 *   UI => mtservice 
 */_event( ev_WiFiCfgModifyItemCmd )
   _body( u32, 1 )
   _body( TWiFiBSSIDCfg, 1 )
   _ev_end

/***********************<< ����WiFi�������� >>********************	
 *[��Ϣ��]
 * + TWiFiBSSIDInfo , 1 //WiFi��������ɨ��ṹ
 *  
 *[��Ϣ����]
 *   UI => mtservice 
 */_event( ev_WiFiConnectCmd )
   _body( TWiFiBSSIDInfo, 1 )
   _ev_end
   
   
/***********************<< �Ͽ�WiFi�������� >>********************	
 *[��Ϣ��]
 *  + ��
 *  
 *[��Ϣ����]
 *   UI  => mtservice 
 */_event( ev_WiFiDisConnectCmd )
   _ev_end
   

/***********************<< WiFi��������״ָ̬ʾ >>********************	
 *[��Ϣ��]
 *  + TWifiLinkCfgItem , 1 //WiFi��������״̬�ṹ
 *  
 *[��Ϣ����]
 *   mtservice  => UI 
 */_event( ev_WiFiLinkStatInd  )
   _body ( TWifiLinkCfgItem, 1 )
   _ev_end


/***********************<< ��ȡWiFi��������״̬���� >>********************	
 *[��Ϣ��]
 *  + ��
 *  
 *[��Ϣ����]
 *   UI  => mtservice 
 */_event( ev_WiFiGetLinkStatCmd )
   _ev_end


/***********************<< WiFi������������ָʾ >>********************	
 *[��Ϣ��]
 *  + TWiFiBSSIDCfg , MT_WIFI_MAX_BSSIDCFG_NUM //WiFi�����������ýṹ
 *  
 *[��Ϣ����]
 *   mtservice  => UI
 */_event( ev_WiFiCfgInd )
   _body ( TWiFiBSSIDCfg, MT_WIFI_MAX_BSSIDCFG_NUM )
   _ev_end


/***********************<< WiFi����ɨ������ >>********************	
 *[��Ϣ��]
 *  + ��
 *  
 *[��Ϣ����]
 *   UI  => mtservice 
 */_event( ev_WiFiScanBSSCmd )
   _ev_end


/***********************<< WiFi����ɨ����ָʾ >>********************	
 *[��Ϣ��]
 *  + TWiFiBSSIDInfo , MT_WIFI_MAX_BSSIDCFG_NUM //WiFi��������ɨ��ṹ
 *  
 *[��Ϣ����]
 *   mtservice  => UI 
 */_event( ev_WiFiScanBSSInd )
   _body ( TWiFiBSSIDInfo, MT_WIFI_MAX_BSSIDCFG_NUM )
   _ev_end

   
/***********************<< ����ǽ������������ָʾ >>**********************	
 *[��Ϣ��]	
 *    + u8        0-�ɹ� 1-��ʱ 2-δ�ҵ� 3-ʧ��
 *[��Ϣ����]
 *    UI <= MtService
 */_event(  ev_FwProxyDomainReqInd  )
   _body( u8        ,1 )
   _ev_end

/***********************<< MTC PC �ֱ���ָʾ >>********************	
 *[��Ϣ��]
 *  
 *[��Ϣ����]
 *   MT  <=> MTC
 */_event(   ev_MTCVideoResInd )
   _body( EmVideoResolution , 1 )
   _ev_end

/***********************<<  �����������Ŷ˿�    >>********************	
 *[��Ϣ��]			 
 *     +EmVideoType first video or second video
 *      u32 : PORT_MASK
 *[��Ϣ����]
 *    MtService = >MtMP
 */_event( ev_SetCodecVideoOutCmd  )
   _body ( EmVideoType   ,1)
   _body ( u32   ,1)
   _ev_end

/***********************<<  ������������˿�    >>********************	
 *[��Ϣ��]			 
 *     +EmVideoType first video or second video
 *      u32 : PORT_MASK
 *[��Ϣ����]
 *    MtService = >MtMP
 */_event( ev_SetCodecVideoInCmd  )
   _body ( EmVideoType   ,1)
   _body ( u32   ,1)
   _ev_end


/***********************<<  ָʾ��ǰ�ľ�������    >>********************	
 *[��Ϣ��]			 
 *     +EmVideoType first video or second video
 *      u32 : PORT_MASK
 *[��Ϣ����]
 *    MtService = >MtMP
 */_event( ev_CurMatrixInd  )
   _body ( THDAVInnerMatrixScheme   ,1)
   _ev_end


/***********************<<  ������Ƶ�˿�ѡ������    >>********************	
 *[��Ϣ��]			 
 *     +THdAudioPort  ��Ƶ�˿�
 *[��Ϣ����]
 *    UI/MTC => MtService => MTMP_HD
 */_event( ev_CfgHDAudioPortCmd  )
   _body ( THdAudioPort   ,1)
   _ev_end

/***********************<<  ������Ƶ�˿�ѡ��ָʾ    >>********************	
 *[��Ϣ��]			 
 *     +THdAudioPort  ��Ƶ�˿�
 *[��Ϣ����]
 *    UI/MTC <= MtService
 */_event( ev_CfgHDAudioPortInd  )
   _body ( THdAudioPort   ,1)
   _ev_end

/***********************<<  �а���������    >>********************	
 *[��Ϣ��]			 
 *     +BOOL  �Ƿ��а�����
 *[��Ϣ����]
 *    UI/MTC => MtService => MTMP_HD
 */_event( ev_CfgUseSliceCmd  )
   _body ( BOOL   ,1)
   _ev_end

/***********************<<  �а�����ָʾ    >>********************	
 *[��Ϣ��]			 
 *     +BOOL  �Ƿ��а�����
 *[��Ϣ����]
 *    UI/MTC <= MtService
 */_event( ev_CfgUseSliceInd  )
   _body ( BOOL   ,1)
   _ev_end

/***********************<<  AAC��Ƶ���ò���    >>********************	
 *[��Ϣ��]			 
 *     +EmCodecComponent	��Ƶ����
 *     +u8		            ͨ��������0--7, ����6��ʾ5.1����, 7��ʾ7.1������
 *     +u32					����������0��ʾ��Ч��
 *	   +u16					���ʣ�Ŀǰδ���壩
 *[��Ϣ����]
 *    MtService => MTC/MTMP
 */_event( ev_CodecAudAacParamCmd  )
   _body ( EmCodecComponent		,1 )
   _body ( u8                   ,1 )
   _body ( u32                  ,1 )
   _body ( u16                  ,1 )
   _ev_end

/***********************<<  ����ͼ�������������  >>********************	
 *[��Ϣ��]			 
 *     +TImageAdjustParam	���ڸ���Ŀǰ����������
 *[��Ϣ����]
 *    MtService <= MTC/MTMP
 */_event( ev_CfgHDImageParamCmd  )
   _body( TImageAdjustParam     ,REMAIN )
   _ev_end

/***********************<<  ����ͼ���������ָʾ  >>********************	
 *[��Ϣ��]			 
 *     +TImageAdjustParam	���ڸ���Ŀǰ����������
 *[��Ϣ����]
 *    MtService => MTC/MTMP
 */_event( ev_CfgHDImageParamInd  )
   _body( TImageAdjustParam     ,REMAIN )
   _ev_end

/***********************<<  ����ģʽ������������  >>********************	
 *[��Ϣ��]			 
 *     +TImageAdjustParam	���ڸ���Ŀǰ����������
 *[��Ϣ����]
 *    MtService => MTC/MTMP
 */_event( ev_CfgScreenModeCmd  )
   _body( u8     , 1 )
   _ev_end

/***********************<<  ����ͼ���������ָʾ  >>********************	
 *[��Ϣ��]			 
 *     +TImageAdjustParam	���ڸ���Ŀǰ����������
 *[��Ϣ����]
 *    MtService => MTC/MTMP
 */_event( ev_CfgScreenModeInd  )
   _body( u8     , 1 )
   _ev_end

/***********************<<  ����DVI������������  >>********************	
 *[��Ϣ��]			 
 *     +BOOL  �Ƿ�����DVI������ӿ�
 *[��Ϣ����]
 *    MtService <= MTC/MTMP
 */_event( ev_CfgHDDVIParamCmd  )
   _body( BOOL     , 1 )
   _ev_end

/***********************<<  ����DVI��������ָʾ  >>********************	
 *[��Ϣ��]			 
 *     +BOOL  �Ƿ�����DVI������ӿ�
 *[��Ϣ����]
 *    MtService => MTC/MTMP
 */_event( ev_CfgHDDVIParamInd  )
   _body( BOOL     , 1 )
   _ev_end
   
/***********************<< ��ȡϵͳ����ʱ��ָʾ >>**********************	
 *[��Ϣ��]	
 *    + u32 ����ʱ��,��λΪ��
 *[��Ϣ����]
 *    UI <= MtService
 */_event(  ev_GetSystemRunTimeInd )
   _body( u32        ,1 )
   _ev_end
   
/***********************<< ��ȡϵͳ����ʱ������ >>**********************	
 *[��Ϣ��]	
 *    + ��
 *[��Ϣ����]
 *    UI <= MtService
 */_event(  ev_GetSystemRunTimeCmd )
   _ev_end
   
/***********************<< ����ģʽ�������� >>************************	
 *[��Ϣ��]			 
 *   + u32 ����ģʽ���� 
 *[��Ϣ����]
 *    UI => MtService
 */_event(  ev_CfgEthModeCmd  )
   _body( u32           ,1 )
   _ev_end
   
/***********************<< ����ģʽ����ָʾ >>************************	
 *[��Ϣ��]			 
 *   + u32 ����ģʽ���� 
 *[��Ϣ����]
 *    UI => MtService
 */_event(  ev_CfgEthModeInd  )
   _body( u32           ,1 )
   _ev_end
     
  
/***********************<< MC��ϯѡ������  >>*************************
 *[��Ϣ��]
 *  BOOL  +1
 *  TMtId +1
 *[��Ϣ����]
 *  UI => MtService => MC
 */_event( ev_mcViewCmd )
   _body( BOOL,      1 )
   _body( TMtId,     1 )
   _ev_end

/***********************<< MC��ϯѡ��ָʾ  >>*************************
 *[��Ϣ��]
 *  BOOL  +1
 *  TMtId +1
 *[��Ϣ����]
 *  UI <= MtService <= MC
 */_event( ev_mcViewInd )
   _body( BOOL,      1 )
   _body( TMtId,     1 )
   _ev_end
   
  
   
/***********************<< pcmt������Ƶ�ɼ���������  >>*************************
 *[��Ϣ��]
 *  u16     1  ��Ƶ����ͼ����
 *  u16     1  ��Ƶ����ͼ��߶�
 *  u8      1  ֡���Ƿ�С��1
 *  u8      1  ��Ƶ����֡��
 *  u8      1  ��Ƶ����֡��ʽ
 *  u32     2  ����λ
 *[��Ϣ����]
 *  MtService <= PCMT
 */_event( ev_PcmtSetCapParamCmd )
   _body( u16,     1 )
   _body( u16,     1 )
   _body( u8,      1 )
   _body( u8,      1 )
   _body( u8,      1 )
   _body( u32,     2 )
   _ev_end
    
  
/*********<< pcmt���ݻ������� >>**************	
 *[��Ϣ��]			 
 *
 *[��Ϣ����]
 *
 *   pcmt => mtservice
 *
 */_event(  ev_PcmtDataConfReq   )    
   _ev_end

/*********<< pcmt���ݻ���Ӧ�� >>**************	
 *[��Ϣ��]			 
 * BOOL //TRUE,����;FALSE,������
 *[��Ϣ����]
 *
 *   mtservice => pcmt
 *
 */_event( ev_PcmtDataConfRsp   )
   _body ( BOOL,    1           )
   _ev_end
   
/***********************<< �ӱ�������ȡ�ĵ�һ��·֡���ϱ� >>************************	
 *[��Ϣ��]		 
 *  +u16  //��һ·��Ƶ����֡��
 *  +u16  //��һ·��Ƶ����֡��
 *  +u16  //�ڶ�·��Ƶ����֡��
 *  +u16  //�ڶ�·��Ƶ����֡��
 *[��Ϣ����]
 *    MtService => UI
 */_event ( ev_VidFramerateInd )
  _body ( u16, 1 )
  _body ( u16, 1 )
  _body ( u16, 1 )
  _body ( u16, 1 )
  _ev_end                           

/***********************<<  ������ֱ������ָʾ  >>********************	
 *[��Ϣ��]			 
 *     +BOOL  �Ƿ����ð�����ֱ������
 *[��Ϣ����]
 *    MtService => MTC/MTMP
 */_event( ev_CfgHDResOutputInd  )
   _body( u32     , 1 )
   _body( u32     , 1 )//Reversed 1
   _body( u32     , 1 )//Reversed 2  
   _ev_end  

/***********************<<  ������ֱ����������  >>********************	
 *[��Ϣ��]			 
 *     +BOOL  �Ƿ����ð�����ֱ������
 *[��Ϣ����]
 *    MtService => MTC/MTMP
 */_event( ev_CfgHDResOutputCmd  )
   _body( u32     , 1 )
   _body( u32     , 1 )//Reversed 1
   _body( u32     , 1 )//Reversed 2  
   _ev_end  


/***********************<<  ֱ�ӵ���ͼ�����������  >>********************	
 *[��Ϣ��]			 
 *     +EmCodecComponent  �����ƵĶ���Ŀǰֻ������Ƶ��������
 *     +u32               �������� MT_SCALE_BRIGHTNESS��MT_SCALE_CONTRAST��MT_SCALE_HUE��MT_SCALE_SATURATION           
 *     +u32               ����ֵ
 *[��Ϣ����]
 *    UI => MtService => MTMP
 */_event( ev_AdjustImageParamDirectlyCmd  )
   _body( EmCodecComponent     , 1 )
   _body( u32                  , 1 )
   _body( u32                  , 1 )
   _body( u32                  , 2 ) // Reversed
   _ev_end  
   
/***********************<<  ����PicPlus������������  >>********************	
 *[��Ϣ��]			 
 *     +BOOL  �Ƿ�����PicPlus������ӿ�
 *[��Ϣ����]
 *    MtService <= MTC/MTMP
 */_event( ev_CfgHDPicPlusParamCmd  )
   _body( BOOL     , 1 )
   _ev_end
   
/***********************<<  ����PicPlus��������ָʾ  >>********************	
 *[��Ϣ��]			 
 *     +BOOL  �Ƿ�����PicPlus������ӿ�
 *[��Ϣ����]
 *    MtService => MTC/MTMP
 */_event( ev_CfgHDPicPlusParamInd  )
   _body( BOOL     , 1 )
   _ev_end

/***********************<<  PCMT��Ƶ����ģʽ��������  >>********************	
 *[��Ϣ��]
 *     +u8  PCMT��Ƶ����뷽ʽ  PCMT_VCODEC_SOFTWARE--��������  PCMT_VCODEC_HARDWARE--Ӳ���Կ������
 *     +u8  ����/���� (0:����  1:����)   
 *[��Ϣ����]
 *    MtService <= PCMT
 */_event( ev_CfgVideoWorkModeCmd  )
   _body( u8     , 1 )
   _body( u8     , 1 )
   _body( u32    , 1 )
   _body( u32    , 1 )
   _ev_end

/***********************<<  PCMT��Ƶ����ģʽ����ָʾ  >>********************	
 *[��Ϣ��]
 *     +u8  PCMT��Ƶ����뷽ʽ  PCMT_VCODEC_SOFTWARE--��������  PCMT_VCODEC_HARDWARE--Ӳ���Կ������
 *     +u8  ����/���� (0:����  1:����) 
 *[��Ϣ����]
 *    MtService => PCMT
 */_event( ev_CfgVideoWorkModeInd  )
   _body( u8     , 1 )
   _body( u8     , 1 )
   _body( u32    , 1 )
   _body( u32    , 1 )
   _ev_end

/***********************<<  ���ģʽ֡����������  >>********************	
 *[��Ϣ��]			 
 *     +TVideoOutputFrm  ���֡��

 *[��Ϣ����]
 *    MtService <= MTC/MTMP
 */_event( ev_CfgHDVideoFrmCmd  )
   _body( TVideoDisplayFrm     , 1 )
   _ev_end

/***********************<<  ���ģʽ֡��������ʾ  >>********************	
 *[��Ϣ��]			 
 *     TVideoOutputFrm  ���֡��
 *[��Ϣ����]
 *    MtService <= MTC/MTMP
 */_event( ev_CfgHDVideoFrmInd  )
   _body( TVideoDisplayFrm      , 1 )
   _ev_end
   

/***********************<< �������ʵ�ʵ����ʷֱ���ͳ����Ϣָʾ >>************************	
 *[��Ϣ��]		 
 *  +EmVideoResolution�� �ֱ�Ϊpvenc,svenc, pvdec,svdec
 *[��Ϣ����]
 *    MtMP => MtService 
 */_event ( ev_CodecVideoEncDecResInd )
  _body ( EmVideoResolution, 1 )
  _body ( EmVideoResolution, 1 )
  _body ( EmVideoResolution, 1 )
  _body ( EmVideoResolution, 1 )
  _ev_end
  
/***********************<< �նˣ�������ϯMT��ѡ������ϳ����� >>**********************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *
 *   MtService => 323/sip/320
 */_event( ev_ConfmtSelectVmpReq  )
   _body ( BOOL           ,1 )
   _ev_end

/***********************<< �նˣ�������ϯMT��ѡ������ϳɷ��� >>**********************	
 *[��Ϣ��]			 
 *  BOOL
 *[��Ϣ����]
 *
 *   MtService <= 323/sip/320
 */_event( ev_ConfmtSelectVmpRsp  )
   _body ( BOOL           ,1 )
   _ev_end

#ifndef _MakeMtEventDescription_
   _ev_segment_end( extend )
};
#endif

#endif
