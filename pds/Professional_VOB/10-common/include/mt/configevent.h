#ifndef CONFIG_EVENT_H
#define CONFIG_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmConfig
{
#endif
     _ev_segment( config)//������Ϣ
		
/***********************<< ����ָʾ>>************************	
 *[��Ϣ��]			 
 *    + TGuideCfg
 *[��Ϣ����]
 *   MtService => UI
 */_event(  ev_CfgGuidNeedInd  )
   _body( TGuideCfg       ,1  ) 
   _ev_end



/***********************<< ����ָʾ>>************************	
 *[��Ϣ��]			 
 *    + TGuideCfg
 *[��Ϣ����]
 *   MtService => UI
 */_event(  ev_CfgGuidInd  )
   _body( TGuideCfg       ,1  ) 
   _ev_end



/***********************<< ��������>>************************	
 *[��Ϣ��]			 
 *    + TGuideCfg
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgGuidCmd  )
   _body( TGuideCfg       ,1  ) 
   _ev_end



/***********************<< ����ָʾ >>************************	
 *[��Ϣ��]			 
 *    + EmLanguage
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_CfgLanguageInd  )
   _body( EmLanguage       ,1  ) 
   _ev_end



/***********************<< �������� >>************************	
 *[��Ϣ��]			 
 *    + EmLanguage
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgLanguageCmd  )
   _body( EmLanguage       ,1  ) 
   _ev_end



 /**********************<< �ն����� >>**************	
 *[��Ϣ��]			 
 *  
 *  
 *  
 *[��Ϣ����]
 *
 *   
 *   MtService <= UI
 */_event(  ev_CfgResetCmd   )
   _ev_end



/***********************<< ��ʾ����ָʾ >>************************	
 *[��Ϣ��]			 
 *    + TDisplay (TOsdCfg)
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_CfgDisplayInd  )
   _body( TOsdCfg    ,1  ) 
   _ev_end



/***********************<< ��ʾ���� >>************************	
 *[��Ϣ��]			 
 *    + TDisplay (TOsdCfg)
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgDisplayCmd  )
   _body( TOsdCfg    ,1  ) 
   _ev_end


/***********************<< VGA��ʾ����ָʾ >>************************	
 *[��Ϣ��]			 
 *    + BOOL
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_CfgVGAShowLocalInd  )
   _body( BOOL    ,1  ) 
   _ev_end



/***********************<< VGA��ʾ���� >>************************	
 *[��Ϣ��]			 
 *    + BOOL
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgVGAShowLocalCmd  )
   _body( BOOL    ,1  ) 
   _ev_end

/***********************<< �ڶ�·��Ƶ�����VGAָʾ >>************************	
 *[��Ϣ��]			 
 *    + BOOL
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_CfgSecondVideoToVGAInd  )
   _body( BOOL    ,1  ) 
   _ev_end



/***********************<< �ڶ�·��Ƶ�����VGA���� >>************************	
 *[��Ϣ��]			 
 *    + BOOL
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgSecondVideoToVGACmd  )
   _body( BOOL    ,1  ) 
   _ev_end
   
/***********************<< �û���������ָʾ >>************************	
 *[��Ϣ��]			 
 *    + TUserCfg
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_CfgUserInd  )
   _body( TUserCfg    ,1  )
   _ev_end


/***********************<< �û��������� >>************************	
 *[��Ϣ��]			 
 *    + TUserCfg
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgUserCmd  )
   _body( TUserCfg    ,1  )
   _ev_end



/***********************<< ��̫��ָʾ >>************************	
 *[��Ϣ��]			 
 *    + u8 ��̫������ (0-eth0   )
 *    + TEthnetInfo
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_CfgEthnetInd  )
   _body( u8          ,1  )
   _body( TEthnetInfo ,1  )
   _ev_end


/***********************<< ��̫������ >>************************	
 *[��Ϣ��]			 
 *    + u8 ��̫������ (0-eth0   )
 *    + TEthnetInfo
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgEthnetCmd  )
   _body( u8          ,1  )
   _body( TEthnetInfo ,1  )
   _ev_end



/***********************<< PPPOEָʾ >>************************	
 *[��Ϣ��]			 
 *    + TPPPOECfg
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_CfgPPPOEInd  )
   _body( TPPPOECfg ,1  )
   _ev_end


/***********************<< PPPOE���� >>************************	
 *[��Ϣ��]			 
 *    + TPPPOECfg
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgPPPOECmd  )
   _body( TPPPOECfg ,1  )
   _ev_end


/***********************<< E1ָʾ >>************************	
 *[��Ϣ��]			 
 *    + TE1Config
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_CfgE1Ind  )
   _body( TE1Config ,1  )
   _ev_end


/***********************<< E1���� >>************************	
 *[��Ϣ��]			 
 *    + TE1Config
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgE1Cmd  )
   _body( TE1Config ,1  )
   _ev_end


/***********************<< ·��ָʾ >>************************	
 *[��Ϣ��]			 
 *    + TRouteCfg[MT_MAX_ROUTE_NUM] 
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_CfgRouteInd  )
   _body( TRouteCfg ,MT_MAX_ROUTE_NUM  )
   _ev_end



/***********************<< ����·�� >>************************	
 *[��Ϣ��]			 
 *    + TRouteCfg
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgAddRouteCmd  )
   _body( TRouteCfg ,1  )  
   _ev_end



/***********************<< ɾ��·�� >>************************	
 *[��Ϣ��]			 
 *    + TRouteCfg
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgRemoveRouteCmd  )
   _body( TRouteCfg ,1  )
   _ev_end


/***********************<< ����ǽָʾ >>************************	
 *[��Ϣ��]			 
 *    + TEmbedFwNatProxy
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_CfgFireWallInd  )
   _body( TEmbedFwNatProxy ,1  )
   _ev_end


/***********************<< ����ǽ���� >>************************	
 *[��Ϣ��]			 
 *    + TEmbedFwNatProxy
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgFireWallCmd  )
   _body( TEmbedFwNatProxy ,1  )
   _ev_end


/***********************<< �����ش�ָʾ >>************************	
 *[��Ϣ��]			 
 *    + TLostPackResend
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_CfgStreamRetransInd  )
   _body( TLostPackResend ,1 )
   _ev_end


/***********************<< �����ش����� >>************************	
 *[��Ϣ��]			 
 *    + TLostPackResend
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgStreamRetransCmd  )
   _body( TLostPackResend ,1 )
   _ev_end



/***********************<< SNMPָʾ >>************************	
 *[��Ϣ��]			 
 *    + TSNMPCfg
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_CfgSNMPInd  )
   _body( TSNMPCfg ,1 )
   _ev_end


/***********************<< SNMP���� >>************************	
 *[��Ϣ��]			 
 *    + TSNMPCfg
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgSNMPCmd  )
   _body( TSNMPCfg ,1 )
   _ev_end


/***********************<< SNTPָʾ >>************************	
 *[��Ϣ��]			 
 *    + TSNTPCfg
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_CfgSNTPInd  )
   _body( u32, 1 )
   _ev_end

/***********************<< SNTP���� >>************************	
 *[��Ϣ��]			 
 *    + TSNTPCfg
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgSNTPCmd  )
   _body( u32, 1 )
   _ev_end

/***********************<< Qosָʾ >>************************	
 *[��Ϣ��]			 
 *    + TIPQos
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_CfgQosInd  )
   _body( TIPQoS ,1 )
   _ev_end


/***********************<< Qos���� >>************************	
 *[��Ϣ��]			 
 *    + TIPQos
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgQosCmd  )
   _body( TIPQoS ,1 )
   _ev_end





/***********************<< H323ָʾ >>************************	
 *[��Ϣ��]			 
 *    + TH323Cfg
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_CfgH323SettingInd  )
   _body( TH323Cfg ,1 )
   _ev_end


/***********************<< H323���� >>************************	
 *[��Ϣ��]			 
 *    + TH323Cfg
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgH323SettingCmd  )
   _body( TH323Cfg ,1 )
   _ev_end



/***********************<< ����Ƶ��ѡЭ��ָʾ >>************************	
 *[��Ϣ��]			 
 *    + TAVPriorStrategy
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_CfgAVPriorStrategyInd  )
   _body( TAVPriorStrategy ,1 )
   _ev_end


/***********************<< ����Ƶ��ѡЭ������ >>************************	
 *[��Ϣ��]			 
 *    + TAVPriorStrategy
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgAVPriorStrategyCmd  )
   _body( TAVPriorStrategy ,1 )
   _ev_end




/***********************<< ��Ƶ����ָʾ >>************************	
 *[��Ϣ��]			 
 *    +EmVideoType
 *    +TVideoEncodeParameter
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_CfgVideoParamInd  )
   _body( EmVideoType ,1 )
   _body( TVideoParam ,1 )
   _ev_end


/***********************<< ��Ƶ�������� >>************************	
 *[��Ϣ��]			 
 *    +EmVideoType
 *    +TVideoParam
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgVideoParamCmd  )
   _body( EmVideoType ,1 )
   _body( TVideoParam ,1 )
   _ev_end
   

/***********************<< �������������ĵ�ǰ��Ƶ��ʽѡ���������� >>************************	
 *[��Ϣ��]			 
 *    u32   +1
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgH264FormatSelCmd  )
   _body( u32          ,1 )
   _ev_end

/***********************<< �������������ĵ�ǰ��Ƶ��ʽѡ������ָʾ >>************************	
 *[��Ϣ��]			 
 *    u32   +1
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgH264FormatSelInd  )
   _body( u32          ,1 )
   _ev_end


/***********************<< VOD����ָʾ >>************************	
 *[��Ϣ��]			 
 *    +TVODUserInfo
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_CfgVODUserParamInd  )
   _body( TVODUserInfo ,1 )
   _ev_end


/***********************<< VOD�������� >>************************	
 *[��Ϣ��]			 
 *    +TVODUserInfo
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgVODUserParamCmd  )
   _body( TVODUserInfo ,1 )
   _ev_end

/***********************<< ��������ѡ���б�ָʾ >>************************	
 *[��Ϣ��]			 
 *    +u32(mask)
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_CfgCallBitrateListInd  )
   _body( u32        ,1               )
   _ev_end


/***********************<< ��������ѡ���б����� >>************************	
 *[��Ϣ��]			 
 *    +u32(mask)
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgCallBitrateListCmd  )
   _body( u32        ,1               )
   _ev_end



/***********************<< ��ý��ָʾ >>************************	
 *[��Ϣ��]			 
 *    +TStreamMedia
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_CfgStreamMediaInd  )
   _body( TStreamMedia ,1         )
   _ev_end


/***********************<< ��ý������ >>************************	
 *[��Ϣ��]			 
 *    +TStreamMedia
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgStreamMediaCmd  )
   _body( TStreamMedia ,1         )
   _ev_end


 
/***********************<< ����ͷ�����޸�  >>************************	
 *[��Ϣ��]			 
 *    + u8  ����ͷ������ (1-6)
 *    + TCameraCfg
 *
 *[��Ϣ����]
 *   UI => MtService
 *
 *   MtService => MtDevice
 */_event(  ev_CameraConfigCmd	  )
   _body( u8           ,1         )
   _body( TCameraCfg   ,1         )
   _ev_end
 

/***********************<< �����ָʾ  >>************************	
 *[��Ϣ��]			 
 *    + u8  ����ͷ������ (1-6)
 *    + TCameraCfg
 *
 *[��Ϣ����]
 *   UI <= MtService
 */_event(  ev_CameraConfigInd		  )
   _body( u8           ,1         )
   _body( TCameraCfg   ,1         )
   _ev_end
 




/***********************<< ͼ�����ָʾ >>************************	
 *[��Ϣ��]			 
 *    +TImageAdjustParam
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_ImageAdjustParamInd  )
   _body( TImageAdjustParam ,1      )
   _ev_end


/***********************<< ͼ��������� >>**********************	
 *[��Ϣ��]	
 *    +TImageAdjustParam
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_ImageAdjustParamCmd  )
   _body( TImageAdjustParam ,1      )
   _ev_end


/***********************<< ��Ƶ��ʽָʾ >>************************	
 *[��Ϣ��]			 
 *    +TVideoStandard
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_CfgVideoStandardInd  )
   _body( TVideoStandard ,1         )
   _ev_end


/***********************<< ��Ƶ��ʽ���� >>**********************	
 *[��Ϣ��]	
 *    +TVideoStandard
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgVideoStandardCmd  )
   _body( TVideoStandard ,1         )
   _ev_end




/***********************<< ��̬NATӳ���ַָʾ >>************************	
 *[��Ϣ��]			 
 *    +TNATMapAddr
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_CfgNATMapAddrInd  )
   _body( TNATMapAddr ,1         )
   _ev_end


/***********************<< ��̬NATӳ���ַ���� >>**********************	
 *[��Ϣ��]	
 *    +TNATMapAddr 
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgNATMapAddrCmd  )
   _body( TNATMapAddr ,1         )
   _ev_end



/***********************<< �ն�ͨ����ʼ�˿ں�����   >>**********************	
 *[��Ϣ��]	
 *    +u16  TCP ��ʼ�˿ں�
 *    +u16  UDP ��ʼ�˿ں�
 *[��Ϣ����]
 *    UI => MtService
 */_event(  ev_CfgCommonBasePortCmd  )
   _body (  u16    ,1             ) 
   _body (  u16    ,1             ) 
   _ev_end


/***********************<< �ն�ͨ����ʼ�˿ں�ָʾ >>**********************	
 *[��Ϣ��]	
 *    +u16  TCP ��ʼ�˿ں�
 *    +u16  UDP ��ʼ�˿ں�
 *[��Ϣ����]
 *    UI< = MtService
 */_event(  ev_CfgCommonBasePortInd  )
   _body (  u16    ,1             ) 
   _body (  u16    ,1             ) 
   _ev_end

 
/**********************<< UI�����ն�״̬ >>**************	
 *[��Ϣ��]			 
 *  +TTerStatus
 *  
 *[��Ϣ����]
 *
 *   
 *   MtService <= UI
 */_event(  ev_MtStatusReq      )
   _body( TTerStatus ,1         )
   _ev_end


/**********************<< �����ն�״ָ̬ʾ >>**************	
 *[��Ϣ��]			 
 *  +TTerStatus
 *  
 *[��Ϣ����]
 *
 *   MtService => UI
 */_event(  ev_MtStatusInd   )
   _body( TTerStatus ,1         )
   _ev_end

/***********************<< ����Ĭ������ >>**********************	
 *[��Ϣ��]			 
 *  + u32_ip    Ĭ������
  *[��Ϣ����]
 *  UI => MtService
 */_event(  ev_CfgDefaultGatewayCmd )
   _body (  u32_ip    ,1            )
   _ev_end

/***********************<< Ĭ����������ָʾ >>**********************	
 *[��Ϣ��]			 
 *  + u32_ip    Ĭ������
 *[��Ϣ����]
 *  MtService => UI
 */_event(  ev_CfgDefaultGatewayInd )
   _body (  u32_ip    ,1            )
   _ev_end
   
   /***********************<< ����DNS Server >>**********************	
 *[��Ϣ��]			 
 *  + u32_ip    DNS Server ip
 *[��Ϣ����]
 *  MtService => UI
 */_event(  ev_CfgDNSServerCmd )
   _body (  u32_ip    ,1            )
   _ev_end

     /***********************<< DNS Serverָʾ >>**********************	
 *[��Ϣ��]			 
 *  + u32_ip    DNS Server ip
 *[��Ϣ����]
 *  MtService => UI
 */_event(  ev_CfgDNSServerInd )
   _body (  u32_ip    ,1            )
   _ev_end

/***********************<< ���л��ƶ�����  >>************************	
 *[��Ϣ��]	
 *
 *[��Ϣ����]
 *
 *   UI        => MtService
 *   
 */_event(  ev_MovePiPCmd   )
   _ev_end  

 /***********************<< ϵͳ�������� >>********************	
  * NULL
  * MTC => CMiscService
  * CMiscService => MTUI
 */_event(ev_UpdateSystemRQS)
   _body(s8 , 256)
   _ev_end


 /***********************<< ϵͳ�������� >>********************	
  * NULL
  * MTC => CMiscService
  * CMiscService => MTUI
 */_event(ev_UpdateSystemFileCmd)
   _ev_end

 /***********************<< ϵͳ����ָʾ>********************	
  * BOOL  bSuccess
  * MTC => CMiscService
  * CMiscService => MTUI
 */_event(ev_UpdateSystemFileInd)
   _body(BOOL , 1)
   _ev_end
   
/***********************<< ϵͳ�����ظ�    >>********************	
  * BOOL ͬ������ TRUE , �ܾ�����FALSE
  * CMiscService <= MTUI  
  * MTC <= CMiscService
 */_event(ev_UpdateSystemACK)
  _body(BOOL , 1)
   _ev_end
  
/***********************<< ϵͳ�����ظ�    >>********************	
  * BOOL �ϴ��ɹ� TRUE , �ϴ�ʧ��FALSE
  * MTC => CMiscService
  * CMiscService => MTUI  
 */_event(ev_SysfileUploadStatus)
  _body(BOOL , 1)
   _ev_end

 /***********************<< ϵͳ�ָ�Ĭ��ֵ���� >>********************	
  * NULL
  * CService <= MTUI
 */_event(ev_ResetSystemCmd)
   _ev_end

/***********************<< ϵͳ�ָ�Ĭ��ֵָʾ    >>********************	
 */_event(ev_ResetSystemInd)
   _ev_end
  
/***********************<< ϵͳ�ָ�Ĭ��ֵ״̬    >>********************	
  * BOOL �ɹ� TRUE , ʧ��FALSE
  * CService => MTUI  
 */_event(ev_ResetSystemStatus)
  _body(BOOL , 1)
   _ev_end
/***********************<< ���ʹ��֪ͨ>>************************ 
 *[��Ϣ��]    
 *    BOOL
 *
 *[��Ϣ����]
 *    MT <= MTService
 */_event(  ev_MonitorSwitchCapInd  )
  _body ( BOOL, 1    )
   _ev_end
 
/***********************<< ��ز���ָʾ>>************************ 
 *[��Ϣ��]    
 *    TMonitorService
 *
 *[��Ϣ����]
 *    MT <= MTService
 */_event(  ev_MonitorSwitchParamInd  )
  _body ( TMonitorService, 1    )
   _ev_end
 
/***********************<< ��ز�������>>************************ 
 *[��Ϣ��]    
 *    TMonitorService
 *
 *[��Ϣ����]
 *    MT <= MTService
 */_event(  ev_MonitorSwitchParamCmd  )
  _body ( TMonitorService, 1    )
   _ev_end

/***********************<< pc˫����Ƶ����ָʾ >>************************	
 *[��Ϣ��]			 
 *    +TVideoParam
 *[��Ϣ����]
 *    MtService => UI/mtc
 */_event(  ev_CfgPCDualVideoParamInd  )
   _body( TVideoParam ,1 )
   _ev_end

/***********************<< pc˫����Ƶ�������� >>************************	
 *[��Ϣ��]			 
 *    +TVideoParam
 *[��Ϣ����]
 *    UI => MtService
 */_event(  ev_CfgPCDualVideoParamCmd  )
   _body( TVideoParam ,1 )
   _ev_end

/***********************<< 8010c��ʾVGA����video����ָʾ>>************************ 
 *[��Ϣ��]    
 *    u16
 *
 *[��Ϣ����]
 *    MT <= MTService
 */_event(  ev_8010cShowModelParamInd  )
  _body ( TVgaOutCfg, 1    )
   _ev_end
 
/***********************<< 8010c��ʾVGA����video��������>>************************ 
 *[��Ϣ��]    
 *    u16
 *
 *[��Ϣ����]
 *    MT <= MTService
 */_event(  ev_8010cShowModelParamCmd  )
  _body ( TVgaOutCfg, 1    )
   _ev_end

 
/***********************<< ˫�����ʱ�����>>************************ 
 *[��Ϣ��]    
 *    u8
 *
 *[��Ϣ����]
 *    MT <= MTService
 */_event(  ev_CfgDualCodeRationCmd  )
  _body ( TDualRation, 1    )
   _ev_end

/***********************<< ˫�����ʱ�ָʾ>>************************ 
 *[��Ϣ��]    
 *    u8
 *
 *[��Ϣ����]
 *    MT <= MTService
 */_event(  ev_CfgDualCodeRationInd  )
  _body ( TDualRation, 1    )
   _ev_end
   
/***********************<< pcmt��ƵԴ���� >>************************	
 *[��Ϣ��]		 
 *  +TCapType  
 *[��Ϣ����]
 *    pcmt => MtServie
 */_event ( ev_CfgPcmtVidSourceCmd )
  _body ( TCapType, 1 )
  _ev_end

/***********************<< pcmt��ƵԴ����ָʾ >>************************	
 *[��Ϣ��]		 
 *  +TCapType     
 *[��Ϣ����]
 *    MtServie => pcmt
 */_event ( ev_CfgPcmtVidSourceInd )
  _body ( TCapType , 1 )
  _ev_end
  
//////////////////////////////////////////////////////////////////////////
//[xujinxing-2006-10-17]
/***********************<< ��Ƕmc���� >>************************	
 *[��Ϣ��]		 
 *  + TInnerMcCfg //mc���õĽṹ    
 *[��Ϣ����
 *   ui => mtservice => mcservice
 */_event ( ev_CfgInnerMcCmd )
  _body ( TInnerMcCfg , 1 )
  _ev_end
  
//////////////////////////////////////////////////////////////////////////
//[xujinxing-2006-10-17]
/***********************<< ��Ƕmc����ָʾ >>************************	
 *[��Ϣ��]		 
 *  + TInnerMcCfg //mc���õĽṹ    
 *[��Ϣ����
 *   ui <= mtservice 
 */_event ( ev_CfgInnerMcInd )
  _body ( TInnerMcCfg , 1 )
  _ev_end
  
//////////////////////////////////////////////////////////////////////////
//[fangtao-2007-01-17]
/***********************<< ��ݼ��������� >>************************	
 *[��Ϣ��]		 
 *  + TRapidKey    
 *[��Ϣ����
 *   ui => mtservice 
 */_event ( ev_CfgRapidKeyCmd )
  _body ( TRapidKey , 1 )
  _ev_end

//////////////////////////////////////////////////////////////////////////
//[fangtao-2007-01-17]
/***********************<< ��ݼ�����ָʾ >>************************	
 *[��Ϣ��]		 
 *  + TRapidKey    
 *[��Ϣ����
 *   ui <= mtservice 
 */_event ( ev_CfgRapidKeyInd )
  _body ( TRapidKey , 1 )
  _ev_end
  

//////////////////////////////////////////////////////////////////////////
//[fangtao-2007-01-22]
/***********************<< T2����Ƶ�������� >>************************	
 *[��Ϣ��]		 
 *  + BOOL  TRUE��ʾ�����Video    
 *[��Ϣ����
 *   ui => mtservice 
 */_event ( ev_CfgT2MainVideoOutCmd )
  _body ( BOOL , 1 )
  _ev_end

//////////////////////////////////////////////////////////////////////////
//[fangtao-2007-01-22]
/***********************<< ��ݼ�����ָʾ >>************************	
 *[��Ϣ��]		 
 *  + BOOL  TRUE��ʾ�����Video   
 *[��Ϣ����
 *   ui <= mtservice 
 */_event ( ev_CfgT2MainVideoOutInd )
  _body ( BOOL , 1 )
  _ev_end

//////////////////////////////////////////////////////////////////////////
//[fangtao-2007-03-12]
/***********************<< ��ַ������ָʾ >>************************	
 *[��Ϣ��]		 
 *  + s8  100  �Ѿ��ϴ��õĵ�ַ���ļ�����·��   
 *[��Ϣ����
 *   ui <= mtservice 
 */_event ( ev_AddrbookUploadInd )
  _body ( s8 ,    100 )
  _ev_end

/***********************<< License����ָʾ >>************************	
 *[��Ϣ��]		 
 *  + s8  100  �Ѿ��ϴ��õ�License�ļ�����·��   
 *[��Ϣ����
 *   ui <= mtservice 
 */_event ( ev_LicenseKeyUploadInd )
  _body ( s8 ,    100 )
  _ev_end


/***********************<< ����GK�������� >>************************	
 *[��Ϣ��]		 
 *  + TInnerGK     
 *[��Ϣ����
 *   ui <= mtservice 
 */_event ( ev_CfgInnerGKCmd )
  _body( TInnerGKCfg, 1 )
  _ev_end

/***********************<< ����GK����ָʾ >>************************	
 *[��Ϣ��]		 
 *  + TInnerGK     
 *[��Ϣ����
 *   ui <= mtservice 
 */_event ( ev_CfgInnerGKInd )
  _body( TInnerGKCfg, 1 ) 
  _ev_end 

/***********************<< ���ô����������� >>************************	
 *[��Ϣ��]		 
 *  + TInnerGK     
 *[��Ϣ����
 *   ui <= mtservice 
 */_event ( ev_CfgInnerProxyCmd )
  _body( TInnerProxyCfg, 1 ) 
  _ev_end

/***********************<< ���ô�������ָʾ >>************************	
 *[��Ϣ��]		 
 *  + TInnerGK     
 *[��Ϣ����
 *   ui <= mtservice 
 */_event ( ev_CfgInnerProxyInd )
  _body( TInnerProxyCfg, 1 ) 
  _ev_end


/***********************<< �����øĶ���Ҫ�������� >>************************	
 *[��Ϣ��]		 
   
 *[��Ϣ����
 *   ui => mtservice 
 */_event ( ev_GuideNeedRebootCmd )
  _ev_end

/***********************<< ��������WAN/LANѡ������ >>************************	
 *[��Ϣ��]		 
 * + u8   ������λ���� MT/GK/PXY, 1��ʾѡ��LAN
 *[��Ϣ����
 *   ui => mtservice 
 */_event ( ev_CfgSetNetSelectMaskCmd )
  _body( u8          ,1 )
  _ev_end

/***********************<< ��������WAN/LANѡ��ָʾ >>************************	
 *[��Ϣ��]		 
 * + u8   ������λ���� MT/GK/PXY, 1��ʾѡ��LAN
 *[��Ϣ����
 *   mtservice => ui
 */_event ( ev_CfgSetNetSelectMaskInd )
  _body( u8          ,1 )
  _ev_end


/***********************<< ����MTU���� >>************************	
 *[��Ϣ��]		 
 * + u32   MTUֵ
 *[��Ϣ����
 *   ui => mtservice 
 */_event ( ev_CfgSetMTUCmd )
  _body( u32          ,1 )
  _ev_end

/***********************<< ����MTUָʾ >>************************	
 *[��Ϣ��]		 
 * + TMtuInfo   MTUֵ
 *[��Ϣ����
 *   mtservice => ui
 */_event ( ev_CfgSetMTUInd )
  _body( u32          ,1 )
  _ev_end 
  

/***********************<< ���������������� >>************************	
 *[��Ϣ��]		 
 * + u32   �������ȵȼ�
 *[��Ϣ����
 *   ui => mtservice
 */_event ( ev_CfgSetAudioPrecedenceCmd )
  _body( u32         ,1 )
  _ev_end 

/***********************<< ������������ָʾ >>************************	
 *[��Ϣ��]		 
 * + u32   �������ȵȼ�
 *[��Ϣ����
 *   mtservice => ui
 */_event ( ev_CfgSetAudioPrecedenceInd )
  _body( u32         ,1 )
  _ev_end 
  

/***********************<< ������������������� >>************************	
 *[��Ϣ��]		 
 * + u16   �����������
 *[��Ϣ����
 *   ui => mtservice
 */_event ( ev_CfgSetLastCallRateCmd )
  _body( u16         ,1 )
  _ev_end 

/***********************<< ���������������ָʾ >>************************	
 *[��Ϣ��]		 
 * + u16   �����������
 *[��Ϣ����
 *   mtservice => ui
 */_event ( ev_CfgSetLastCallRateInd )
  _body( u16         ,1 )
  _ev_end 


/***********************<< ����������е�ַ��¼���� >>************************	
 *[��Ϣ��]		 
 * + s8[MT_MAXNUM_CALLADDR_RECORD][MT_MAX_H323ALIAS_LEN+1]������е�ַ��¼
 *[��Ϣ����
 *   ui => mtservice
 */_event ( ev_CfgSetCallAddrRecordCmd )
  _body( s8         ,MT_MAXNUM_CALLADDR_RECORD*(MT_MAX_H323ALIAS_LEN+1) )
  _ev_end 

/***********************<< ����������е�ַ��¼ָʾ >>************************	
 *[��Ϣ��]		 
 * + s8[MT_MAXNUM_CALLADDR_RECORD][MT_MAX_H323ALIAS_LEN+1]   ������е�ַ��¼
 *[��Ϣ����
 *   mtservice => ui
 */_event ( ev_CfgSetCallAddrRecordInd )
  _body( s8         ,MT_MAXNUM_CALLADDR_RECORD*(MT_MAX_H323ALIAS_LEN+1) )
  _ev_end 


/***********************<< ��������绰�����¼���� >>************************	
 *[��Ϣ��]		 
 * + s8[MT_MAXNUM_CALLADDR_RECORD][MT_MAX_H323ALIAS_LEN+1]����绰�����¼
 *[��Ϣ����
 *   ui => mtservice
 */_event ( ev_CfgSetTeleAddrRecordCmd )
  _body( s8         ,MT_MAXNUM_CALLADDR_RECORD*(MT_MAX_H323ALIAS_LEN+1) )
  _ev_end 

/***********************<< ��������绰�����¼ָʾ >>************************	
 *[��Ϣ��]		 
 * + s8[MT_MAXNUM_CALLADDR_RECORD][MT_MAX_H323ALIAS_LEN+1]   ����绰�����¼
 *[��Ϣ����
 *   mtservice => ui
 */_event ( ev_CfgSetTeleAddrRecordInd )
  _body( s8         ,MT_MAXNUM_CALLADDR_RECORD*(MT_MAX_H323ALIAS_LEN+1) )
  _ev_end 
   

/***********************<< ���ô���ip��ַ���õ�ָʾ >>************************	
 *[��Ϣ��]			 
 *    + TPxyIPCfg[MT_MAX_PXYIP_NUM] 
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_CfgPxyIPInd  )
   _body( TPxyIPCfg ,MT_MAX_PXYIP_NUM  )
   _ev_end

/***********************<< �������ô����ip��ַ���� >>************************	
 *[��Ϣ��]			 
 *    + TPxyIPCfg
 *[��Ϣ����]
 *    UI => MtService
 */_event(  ev_CfgAddPxyIPCmd  )
   _body( TPxyIPCfg ,1  )  
   _ev_end

/***********************<< ɾ�����ô����ip��ַ���� >>************************	
 *[��Ϣ��]			 
 *    + TPxyIPCfg
 *[��Ϣ����]
 *    UI => MtService
 */_event(  ev_CfgDeletePxyIPCmd  )
   _body( TPxyIPCfg ,1  )  
   _ev_end

/***********************<< ʹ���ô����ip��ַ������Ч >>************************	
 *[��Ϣ��]			 
 *  //����Ϣ�������նˣ��������ֻ�������Ż���Ч 
 *[��Ϣ����]
 *    UI => MtService
 */_event(  ev_CfgApplyPxyIPCmd  )
   _ev_end

/***********************<< FEC�������� >>************************	
 *[��Ϣ��]			 
 *   + TFecInfo FEC�㷨���� 0:�� 1:Raid5 2:Raid6
 *[��Ϣ����]
 *    UI => MtService
 */_event(  ev_CfgFecTypeCmd  )
   _body( TFecInfo           ,1 )
   _ev_end

/***********************<< FEC����ָʾ >>************************	
 *[��Ϣ��]			 
 *   + TFecInfo FEC�㷨���� 0:�� 1:Raid5 2:Raid6
 *[��Ϣ����]
 *    UI => MtService
 */_event(  ev_CfgFecTypeInd  )
   _body( TFecInfo           ,1 )
   _ev_end
   
/***********************<<  ����ldap��������ַ��ѯ >>********************	
 *[��Ϣ��]
 * +s8 , MT_MAX_LDAPSERVER_NAME_LEN  //�Զ˵�����
 *  
 *[��Ϣ����]
 *   mtservice  => mtservice 
 */_event(   ev_LdapServerIPCfgCmd )
   _body( s8 , MT_MAX_LDAPSERVER_NAME_LEN )
   _ev_end

/***********************<<  ����ldap��������ַ��ѯָʾ >>********************	
 *[��Ϣ��]
 * +s8 , MT_MAX_LDAPSERVER_NAME_LEN  //�Զ˵�����
 *  
 *[��Ϣ����]
 *   mtservice  => mtservice 
 */_event(   ev_LdapServerIPCfgInd )
   _body( s8 , MT_MAX_LDAPSERVER_NAME_LEN )
   _ev_end
   
/***********************<<  WiFi�����ַ���� >>********************	
 *[��Ϣ��]
 * +TWiFiNetCfg , 1 //WiFi�����ַ���ýṹ
 *  
 *[��Ϣ����]
 *   mtservice  => mtservice 
 */_event(   ev_CfgWiFiNetCmd )
   _body( TWiFiNetCfg, 1 )
   _ev_end

/***********************<<  WiFi�����ַָʾ >>********************	
 *[��Ϣ��]
 * +TWiFiNetCfg , 1 //WiFi�����ַ���ýṹ
 *  
 *[��Ϣ����]
 *   mtservice  => mtservice 
 */_event(   ev_CfgWiFiNetInd ) 
   _body( TWiFiNetCfg, 1 )
   _ev_end
 
   
#ifndef _MakeMtEventDescription_
   _ev_segment_end(config)
};

inline BOOL IsConfigEvent(u16 wEvent)
{
	return (wEvent > get_first_event_val(config)) && (wEvent < get_last_event_val(config));
}

#endif
#endif

