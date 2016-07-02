#ifndef CODEC_EVENT_H
#define CODEC_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmCodec
{
#endif  
   _ev_segment( codec )//ý�崦����Ϣ

/**********************<< PCMT ץͼ  >>********************	
 *[��Ϣ��]			 
 *     +TPCCapParam
 *[��Ϣ����]
 *    PCMT UI  = > MtMP
 */_event( ev_GrabPicCmd  )
   _body ( EmCodecComponent   ,1) //ץͼ��� ����������һ·���������ڶ�·������
   _body ( u32                ,1) //CDrawWndָ��
   _ev_end

/***********************<<  ���ö����ش�����    >>********************	
 *[��Ϣ��]			 
 *     +TLostPackResend
 *[��Ϣ����]
 *    MtService = >MtMP
 */_event( ev_CodecLostPackResendCmd  )
   _body ( TLostPackResend   ,1)
   _ev_end
/***********************<< ���ý��������յ�ַ >>**********************	
 *[��Ϣ��]			 
 *  + EmCodecComponent
 *  + TIPTransAddr      RTP���յ�ַ      
 *  + TIPTransAddr      RTCP���յ�ַ
 *  + TIPTransAddr      RTCP����Ŀ�ĵ�ַ
 *  + u32               ���պ����ص�ָ��(H320)
 *[��Ϣ����]
 *   MtService ->Codec
 */_event( ev_CodecRecvAddrCmd )
   _body ( EmCodecComponent, 1    )
   _body ( TIPTransAddr    , 1    )
   _body ( TIPTransAddr    , 1    )
   _body ( TIPTransAddr    , 1    )
   _body ( u32             , 1    )
   _ev_end


/***********************<< ���ý��������͵�ַ >>**********************	
 *[��Ϣ��]			 
 *  + EmCodecComponent
 *  + TIPTransAddr      RTP����Ŀ�ĵ�ַ 
 *  + TIPTransAddr      RTCP����Ŀ�ĵ�ַ
 *  + TIPTransAddr      RTCP���յ�ַ
 *  + u32               ���ͺ����ص�ָ��(H320)
 *[��Ϣ����]
 *   MtService ->Codec
 */_event( ev_CodecSendAddrCmd )
   _body ( EmCodecComponent, 1    )
   _body ( TIPTransAddr    , 1    )
   _body ( TIPTransAddr    , 1    )
   _body ( TIPTransAddr    , 1    )
   _body ( u32             , 1    )
   _ev_end

/***********************<< ѡ����ƵԴ >>**********************	
 *[��Ϣ��]			 
 *  + EmVideoType ����Ƶ���ͣ���/��
 *  + EmMtVideoPort ����ƵԴ����
 *[��Ϣ����]
 *  MtService  ->Codec
 */_event( ev_VideoSourceSelCmd )
   _body ( EmVideoType    , 1    )
   _body ( EmMtVideoPort    , 1    )
   _ev_end

/***********************<< ѡ����ƵԴ >>**********************	
 *[��Ϣ��]			 
 *  + EmVideoType ����Ƶ���ͣ���/��
 *  + EmMtVideoPort ����ƵԴ����
 *[��Ϣ����]
 *  MtService  ->UI
 */_event( ev_VideoSourceSelInd )
   _body ( EmVideoType    , 1    )
   _body ( EmMtVideoPort    , 1    )
   _ev_end

/***********************<< ������Ƶ������� >>**********************	
 *[��Ϣ��]			 
 *  + EmCodecComponent  =emPriomVideoEncoder/emSecondVideoEncoder
 *  + TVideoEncodeParameter 
 *[��Ϣ����]
 *  MtService  ->Codec
 */_event( ev_CodecVideoEncodeParamCmd )
   _body ( EmCodecComponent    , 1    )
   _body ( TVideoEncodeParameter    , 1    )
   _ev_end

/***********************<< ������Ƶ������� >>**********************	
 *[��Ϣ��]			 
 *  +EmCodecComponent  =emAudioEncoder
 *  + EmAudioFormat 
 *[��Ϣ����]
 * MtService  ->Codec
 */_event( ev_CodecAudioEncodeParamCmd )
   _body ( EmCodecComponent    , 1    )
   _body ( EmAudioFormat    , 1    )
   _ev_end

/***********************<< ������Ƶ������� >>**********************	
 *[��Ϣ��]			 
 *  +EmCodecComponent  =emPriomVideoDecoder/emSecondVideoDecoder
 *  + EmLostPackageRestore �� �����ָ���ʽ
 *  + EmVideoFormat    �������ʽ
 *[��Ϣ����]
 * MtService  ->Codec
 */_event( ev_CodecVideoDecodeParamCmd )
   _body ( EmCodecComponent    , 1    )
   _body ( EmLostPackageRestore    , 1    )
   _body ( EmVideoFormat    , 1    )
   _ev_end

/***********************<< ������Ƶ������� >>**********************	
 *[��Ϣ��]			 
 *  +EmCodecComponent  =emAudioDecoder 
 *  +EmAudioFormat 
 *[��Ϣ����]
 * MtService  ->Codec
 */_event( ev_CodecAudioDecodeParamCmd )
   _body ( EmCodecComponent    , 1    )
   _body ( EmAudioFormat       , 1    ) 
   _ev_end

/***********************<< ��ʼ����������� >>**********************	
 *[��Ϣ��]			 
 *  +EmCodecComponent  
 *[��Ϣ����]
 * MtService  ->Codec
 */_event( ev_CodecStartCmd )
   _body ( EmCodecComponent    , 1    )
   _ev_end

/***********************<< ֹͣ����������� >>**********************	
 *[��Ϣ��]			 
 *  +EmCodecComponent  
 *[��Ϣ����]
 * MtService  ->Codec
 */_event( ev_CodecStopCmd )
   _body ( EmCodecComponent    , 1    )
   _ev_end
   
/***********************<< ��ʼ��� >>**********************	
 *[��Ϣ��]			 
 *  +EmSite   ����/Զ��
 *  +TIPTransAddr   ��ص�ַ
 *[��Ϣ����]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_MonitorStartCmd )
   _body ( EmSite    , 1    )
   _body ( TIPTransAddr    , 1    )
   _ev_end

/***********************<< ֹͣ��� >>**********************	
 *[��Ϣ��]			 
 *  +EmSite   ����/Զ��
 *[��Ϣ����]
 * UI         ->MtService
 * MtService  ->Codec
 *->Codec
 */_event( ev_MonitorStopCmd )
   _body ( EmSite    , 1    )
   _ev_end

/***********************<< ������ͼ��ؼ�֡ >>************************	
 *[��Ϣ��]		 
 *  +EmSite	    ����/Զ��
 *[��Ϣ����]
 * UI         ->MtService
 * MtService  ->Codec
 */_event ( ev_MonitorVideoFastUpadateCmd )
  _body ( EmSite, 1 )
  _ev_end
    
/***********************<< ��Ƶ���ͼ�����ָʾ >>************************	
 *[��Ϣ��]		 
 * + u8[2] payload type   ��һ·��Ƶ����Ƶ���غ�����
 * + u8[2] dynamic ploady ��һ·��Ƶ����Ƶ�Ķ�̬�غ����� 
 * + TEncryptKey[2]       ��һ·��Ƶ����Ƶ����Կ
 * + EmSite               ����orԶ��
 *[��Ϣ����]
 * Codec      -> MtService
 * MtService  -> UI
 */_event ( ev_MonitorVideoParamInd )
  _body ( u8, 2 )
  _body ( u8, 2 )
  _body ( TEncryptKey, 2 )
  _body ( EmSite, 1)
  _ev_end

/***********************<< ��ʼ��ý�幦�� >>**********************	
 *[��Ϣ��]			 
 *  +TStreamMedia
 *[��Ϣ����]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_CodecStreamMediaCmd      )
   _body ( TStreamMedia    , 1    )
   _ev_end

/***********************<< ��ʼָ�������ý�幦�� >>**********************	
 *[��Ϣ��]		
 *  + EmCodecComponent -�����
 *  + u32_ip  - �鲥��ַ
 *  + u16     - �˿ں�
 *  + u8      - TTLֵ
 *[��Ϣ����]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_CodecStreamMediaStartCmd      )
   _body (  EmCodecComponent ,1)
   _body ( u32_ip    , 1    )
   _body ( u16       , 1    )
   _body ( u8        , 1    )
   _ev_end

/***********************<< ֹͣ��ָ�����ý�� >>**********************	
 *[��Ϣ��]			 
 *  + EmCodecComponent -�����
 *[��Ϣ����]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_CodecStreamMediaStopCmd )
  _body (  EmCodecComponent ,1)
  _ev_end

/***********************<< ֹͣȫ�������ý�� >>**********************	
 *[��Ϣ��]			 
 *  + 
 *[��Ϣ����]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_CodecStreamMediaStopAllCmd )
  _ev_end

/***********************<< ��ý��Դ�仯ָʾ >>**********************	
 *[��Ϣ��]			 
 * BOOL  TRUE: ��ǰ������ /FALSE :��ǰ��Զ��
 *[��Ϣ����]
 * MtMP -> MtMP 
 */_event( ev_CodecStreamMediaChangedInd )
   _body ( BOOL , 1 )  
   _ev_end


/***********************<< ����˫����ʾģʽ >>**********************	
 *[��Ϣ��]			 
 *  +EmDualVideoShowMode
 *[��Ϣ����]
 * UI         ->MtService
 */_event( ev_CodecDualShowModeCmd )
   _body ( EmDualVideoShowMode    , 1    )
   _ev_end


/***********************<< ������ʾ����ģʽ >>**********************	
 *[��Ϣ��]			 
 *  +EmDisplayRatio
 *[��Ϣ����]
 * UI         ->MtService
 */_event( ev_CodecDisplayRatioCmd )
   _body ( EmDisplayRatio    , 1    )
   _ev_end
   

/***********************<< ���л���С�����л�>>**********************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_SwitchPiPCmd )
   _ev_end

/***********************<< �Ի����� >>**********************	
 *[��Ϣ��]			 
 *  +BOOL      TRUE=��ʼ����  ��FALSE=ֹͣ����
 *[��Ϣ����]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_LoopbackTestCmd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< �Ի�����ָʾ >>**********************	
 *[��Ϣ��]			 
 *  +BOOL      TRUE=��ʼ����  ��FALSE=ֹͣ����
 *[��Ϣ����]
 * MtService  ->UI
 */_event( ev_LoopbackTestInd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< �Ի��������� >>**********************	
 *[��Ϣ��]
 *  +EmMediaType      ý������
 *  +BOOL             TRUE=��ʼ����  ��FALSE=ֹͣ����
 *[��Ϣ����]
 * MtService  ->Codec
 */_event( ev_AutoTestLoopbackCmd )
   _body ( EmMediaType    , 1    )
   _body ( BOOL           , 1    )
   _ev_end

   
/***********************<< ɫ������ >>**********************	
 *[��Ϣ��]			 
 *  +EmSite    ����/Զ��
 *  +BOOL      TRUE=��ʼ����  ��FALSE=ֹͣ����
 *[��Ϣ����]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_RibbonTestCmd )
   _body ( EmSite    , 1    )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< ɫ������ָʾ >>**********************	
 *[��Ϣ��]			 
 *  +EmSite    ����/Զ��
 *  +BOOL      TRUE=��ʼ����  ��FALSE=ֹͣ����
 *[��Ϣ����]
 * MtService  ->UI
 */_event( ev_RibbonTestInd )
   _body ( EmSite    , 1    )
   _body ( BOOL    , 1    )
   _ev_end
/***********************<< ���;�̬ͼƬ >>**********************	
 *[��Ϣ��]			 
 *  +BOOL      TRUE=��ʼ����  ��FALSE=ֹͣ����
 *[��Ϣ����]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_SendPictureCmd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< ���;�̬ͼƬָʾ >>**********************	
 *[��Ϣ��]			 
 *  +BOOL      TRUE=��ʼ����  ��FALSE=ֹͣ����
 *[��Ϣ����]
 * MtService  ->UI
 */_event( ev_SendPictureInd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< ���չ��� >>**********************	
 *[��Ϣ��]			 
 *  +EmVideoType ����Ƶ����
 *[��Ϣ����]
  * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_SnapshotCmd )
   _body ( EmVideoType    , 1    )
   _ev_end
   
/***********************<< ���չ���ָʾ >>**********************	
 *[��Ϣ��]			 
 *  TSnapInfo
 *[��Ϣ����]
  * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_SnapshotInd )
   _body ( TSnapInfo    , 1    )
   _ev_end
   
/***********************<< ������Ϣ���� >>**********************	
 *[��Ϣ��]			 
 *  NULL 
 *[��Ϣ����]
  * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_SnapshotInfoCmd )
   _ev_end
/***********************<< ������Ϣ����ָʾ >>**********************	
 *[��Ϣ��]			 
 *  TSnapInfo
 *[��Ϣ����]
  * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_SnapshotInfoInd )
   _body ( TSnapInfo    , 1    )
   _ev_end

 /***********************<< ��տ������� >>**********************	
 *[��Ϣ��]			 
 *  NULL
 *[��Ϣ����]
  * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_ClearAllSnapshotCmd )
   _ev_end
 
 /***********************<< ��տ���ָʾ >>**********************	
 *[��Ϣ��]			 
 *  BOOL
 *[��Ϣ����]
  * MTC         <- MtService
 * MtService    <- Codec
 */_event( ev_ClearAllSnapshotInd )
    _body ( BOOL    , 1    )
   _ev_end

/***********************<< ���þ������� >>**********************	
 *[��Ϣ��]			 
 *  +BOOL      TRUE=����  ��FALSE=������
 *[��Ϣ����]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_QuietCmd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< ���þ���ָʾ >>**********************	
 *[��Ϣ��]			 
 *  +BOOL      TRUE=����  ��FALSE=������
 *[��Ϣ����]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_QuietInd    )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< ������������ >>**********************	
 *[��Ϣ��]			 
 *  +BOOL      TRUE=����  ��FALSE=������
 *[��Ϣ����]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_MuteCmd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< ��������ָʾ >>**********************	
 *[��Ϣ��]			 
 *  +BOOL      TRUE=����  ��FALSE=������
 *[��Ϣ����]
 * MtService  ->UI
 */_event( ev_MuteInd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< ������������ >>**********************	
 *[��Ϣ��]			 
 *  +u8  ����ֵ
 *[��Ϣ����]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_InputVolumeCmd )
   _body ( u8    , 1    )
   _ev_end

/***********************<< �������ָʾ >>************************	
 *[��Ϣ��]			 
 *    +u8 (�������ֵ)
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_InputVolumeInd  )
   _body ( u8    , 1    )
   _ev_end

/***********************<< ����������� >>**********************	
 *[��Ϣ��]			 
 *  +u8  ����ֵ
 *[��Ϣ����]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_OutputVolumeCmd )
   _body ( u8    , 1    )
   _ev_end

/***********************<< �������ָʾ >>************************	
 *[��Ϣ��]			 
 *    +u8 (�������ֵ)
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_OutputVolumeInd  )
   _body ( u8    , 1    )
   _ev_end
   
/***********************<< ����������������� >>************************	
 *[��Ϣ��]			 
 *    +EmMtLoudspeakerVal (�������ֵ)
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_LoudSpeakerVolumeCmd  )
   _body ( EmMtLoudspeakerVal    , 1    )
   _ev_end

/***********************<< �������������ָʾ >>************************	
 *[��Ϣ��]			 
 *    +EmMtLoudspeakerVal (�������ֵ)
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_LoudSpeakerVolumeInd  )
   _body ( EmMtLoudspeakerVal    , 1    )
   _ev_end

/***********************<< ����/�رջ������� >>**********************	
 *[��Ϣ��]			 
 *  +BOOL  TRUE= ���� ;FALSE=�ر�
 *[��Ϣ����]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_AECCmd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< ����/�رջ���ָʾ >>**********************	
 *[��Ϣ��]			 
 *  +BOOL  TRUE= ���� ;FALSE=�ر�
 *[��Ϣ����]
 * MtService  ->UI
 */_event( ev_AECInd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< ����/�ر��Զ�������� >>**********************	
 *[��Ϣ��]			 
 *  +BOOL  TRUE= ���� ;FALSE=�ر�
 *[��Ϣ����]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_AGCCmd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< AGC(�Զ��������)ָʾ >>************************	
 *[��Ϣ��]			 
 *    +BOOL TRUE=enable
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_AGCInd  )
   _body ( BOOL    , 1    )
   _ev_end
   
/***********************<< ����/�ر��Զ��������� >>**********************	
 *[��Ϣ��]			 
 *  +BOOL  TRUE= ���� ;FALSE=�ر�
 *[��Ϣ����]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_AnsCmd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< Ans(�Զ��������ƿ���)ָʾ >>************************	
 *[��Ϣ��]			 
 *    +BOOL TRUE=enable
 *[��Ϣ����]
 *    MtService => UI
 */_event(  ev_AnsInd  )
   _body ( BOOL    , 1    )
   _ev_end



/***********************<< ���ö�̬�غ� >>**********************	
 *[��Ϣ��]			 
 *  +EmCodecComponent ���������
 *  +u8	����̬�غ�ֵ
 *[��Ϣ����]
 * MtService  ->Codec
 */_event( ev_CodecDynamicPayloadCmd )
   _body ( EmCodecComponent    , 1    )
   _body ( u8    , 1    )
   _ev_end
  
 /********************<< ��Ƶ���ʵ���  >>*****************************
 *[��Ϣ��]	
 *    + EmCodecComponent
 *    + u16       	����ֵ kbps
 *[��Ϣ����]
 *    MtService = > MtMP
*/_event ( ev_CodecSetBitrateCmd )
  _body ( EmCodecComponent , 1 )
  _body ( u16, 1 )
  _ev_end
  
 /********************<< ��Ƶ����֡�ʵ���  >>*****************************
 *[��Ϣ��]	
 *    + EmCodecComponent
 *    + u8       	����֡��ֵ
 *[��Ϣ����]
 *    MtService = > MtMP
*/_event ( ev_CodecSetEncFrameRateCmd )
  _body ( EmCodecComponent , 1 )
  _body ( u8, 1 )
  _ev_end
  
 /********************<< �����͹ؼ�֡  >>*****************************
 *[��Ϣ��]	
 *    + EmCodecComponent  
 *[��Ϣ����]
 *    MtService = > MtMP
*/_event ( ev_CodecVideoFastUpdateCmd  )
  _body ( EmCodecComponent , 1 )
  _ev_end
  
/***********************<< ֻ��ؼ�֡���� >>************************	
 *[��Ϣ��]			 
 *    +EmVideoType
 *[��Ϣ����]
 * MtService  ->Codec
 */_event ( ev_DecodeIFrameOnlyCmd )
  _body ( EmVideoType, 1 )
  _ev_end
  
 /********************<< ����Ƶ�������Կ  >>*****************************
 *[��Ϣ��]	
 *    + EmCodecComponent
 *    + TEncryptKey
 *[��Ϣ����]
 *    MtService = > MtMP
*/_event ( ev_CodecEncryptKeyCmd )
  _body ( EmCodecComponent , 1 )
  _body ( TEncryptKey, 1 )
  _ev_end
 
/***********************<< ���ô��ھ�� >>************************	
 *[��Ϣ��]			 
 *    +u32   Ԥ������
 *    +u32   ������������
 *    +u32   ������������
 *[��Ϣ����]
 *    MtService => MtMP
 */_event ( ev_SetWindowHandle )
  _body ( u32, 1 )
  _body ( u32, 1 )
  _body ( u32, 1 )
  _ev_end
  
   
/***********************<< ע��˫���� >>************************	
 *[��Ϣ��]		 
 *    ��
 *[��Ϣ����]
 *    MtSevice => MtMP 
 */ _event ( ev_CodecDualVidBoxRegCmd )
  _ev_end

/***********************<< ����˫���� >>************************	
 *[��Ϣ��]		 
 *    TDualStreamBoxInfo  tDVBInfo;
 *[��Ϣ����]
 *    MtSevice => MtMP 
 */_event ( ev_CodecDualVidBoxCfgCmd )
  _body ( TDualStreamBoxInfo, 1 )
  _ev_end

/***********************<< ����˫����ָʾ >>************************	
 *[��Ϣ��]		 
 *    TDualStreamBoxInfo  tDVBInfo;
 *[��Ϣ����]
 *    MtMP => MtService 
 */_event ( ev_CodecDualVidBoxCfgInd )
  _body ( TDualStreamBoxInfo, 1 )
  _ev_end

/***********************<< ע��˫���� >>************************	
 *[��Ϣ��]		 
 *   �� 
 *[��Ϣ����]
 *    MtSevice => MtMP 
 */_event ( ev_CodecDualVidBoxUnRegCmd )
  _ev_end
 
/***********************<< ����������ͳ����Ϣ >>************************	
 *[��Ϣ��]		 
 *   �� 
 *[��Ϣ����]
 *    MtSevice => MtMP 
 */_event ( ev_CodecPackStatCmd )
  _ev_end

/***********************<< �������ͳ����Ϣָʾ >>************************	
 *[��Ϣ��]		 
 *  +TCodecPackStat[emCodecComponentEnd]  //ͳ����Ϣ�ṹ��
 *[��Ϣ����]
 *    MtMP => MtService 
 */_event ( ev_CodecPackStatInd )
  _body ( TCodecPackStat, emCodecComponentEnd )
  _ev_end  

/***********************<< ˫��������ָʾ >>************************	
 *[��Ϣ��]		 
 * ��  
 *[��Ϣ����]
 *    MtMP => MtService 
 */_event ( ev_CodecDualVidBoxOnlineInd )
  _ev_end
  
/***********************<< ˫���йҶ�ָʾ >>************************	
 *[��Ϣ��]		 
 * ��  
 *[��Ϣ����]
 *    MtMP => MtService 
 */_event ( ev_CodecDualVidBoxOfflineInd )
  _ev_end
  
/***********************<< ��ý��ָʾ >>************************	
 *[��Ϣ��]		 
 * + u8[3] payload type   ��һ·�ڶ�·��Ƶ����Ƶ���غ�����
 * + u8[3] dynamic ploady ��һ·�ڶ�·��Ƶ����Ƶ�Ķ�̬�غ����� 
 * + TEncryptKey[3]       ��һ·�ڶ�·��Ƶ����Ƶ����Կ
 *[��Ϣ����]
 *    MtMP => MtService 
 */_event ( ev_StreamMediaParamInd )
  _body ( u8, 3 )
  _body ( u8, 3 )
  _body ( TEncryptKey, 3 )
  _ev_end

  
/***********************<< ��ʼ��ֹͣԤ�� >>************************	
 *[��Ϣ��]		 
 * +BOOL  TRUE: ��ʼԤ��; FALSE :ֹͣԤ��
 *[��Ϣ����]
 *    MtMP => MtService 
 */ _event ( ev_CodecPreviewCmd )
  _body ( BOOL, 1  )
  _ev_end

/***********************<< ̨���������� >>************************	
 *[��Ϣ��]		 
 * +BOOL
 * +TSymboPoint
 * +s8[MT_MAX_NAME_LEN] 
 *[��Ϣ����]
 *    MtMP => MtService 
 */ _event ( ev_CodecSetLogoCmd )
   _body  ( BOOL, 1  )
   _body  ( TSymboPoint ,1 )
   _body  ( s8   ,MT_MAX_NAME_LEN )
   _ev_end


/***********************<< DSP �������� >>************************	
 *[��Ϣ��]		 
 * 
 *[��Ϣ����]
 *    MtService = > MtMP
 */ _event ( ev_CodecDspSleepCmd )
    _ev_end

/***********************<< DSP �������� >>************************	
 *[��Ϣ��]		 
 * 
 *[��Ϣ����]
 *    MtService = > MtMP
 */ _event ( ev_CodecDspWakeupCmd )
    _ev_end


/***********************<< MtMP DSP ״ָ̬ʾ >>************************	
 *[��Ϣ��]		 
 *  + BOOL     TRUE =���� FALSE =����
 *[��Ϣ����]
 *     MtMP = > MtService
 */ _event ( ev_CodecDspSleepStateInd )
    _body(   BOOL   , 1          )
    _ev_end


/***********************<< ���ñ����TOSֵ   >>************************	
 *[��Ϣ��]		 
 *  + u8        ��Ƶ
 *  + u8        ��Ƶ
 *  + u8        ����
 *[��Ϣ����]
 *     MtMP = > MtService
 */ _event ( ev_CodecTosCmd      )
    _body(   u8     , 1          )//��Ƶ
	_body(   u8     , 1          )//��Ƶ
	_body(   u8     , 1          )//����
    _ev_end

/***********************<< ���õ���ʱVga���ű���   >>************************	
 *[��Ϣ��]		 
 * + BOOL         TRUE=��ʾ���� FALSE����ʾԶ��
 *[��Ϣ����]
 *     MtMP = > MtService
 */ _event ( ev_CodecVgaShowLocalCmd      )
    _body(   BOOL     , 1          )
    _ev_end

/***********************<< ���û��л�  >>************************	
 *[��Ϣ��]		 
 * + EmPiPMode   
 *[��Ϣ����]
 *      MtService = > MtMP
 */ _event ( ev_CodecSetPIPCmd      )
    _body ( EmPiPMode ,1 )
    _ev_end


/***********************<< ������Ƶ���� >>************************	
 *[��Ϣ��]			 
 *  ��
 *[��Ϣ����]
 *    MtService => MtMP
 */_event(  ev_CodecApplyAudioPowerCmd  )
   _ev_end

/***********************<< ��Ƶ����ָʾ >>************************	
 *[��Ϣ��]			 
 *  u32  ������Ƶ����
 *  u32  �������Ƶ����
 *[��Ϣ����]
 *    MtService <= MtMP
 */_event(  ev_CodecAudioPowerInd  )
   _body ( u32    ,1          )
   _body ( u32    ,1          )
   _ev_end


/***********************<< ��Ƶ��������  >>************************	
 *[��Ϣ��]			 
 *    +EmVideoType
 *    +TImageAdjustParam
 *[��Ϣ����]
 *    MtService => MtMP
 */_event(  ev_CodecImageAdjustParamCmd  )
   _body( EmVideoType       ,1           )
   _body( TImageAdjustParam ,1           )
   _ev_end


/**********************<< Pcmt ѡ���ļ���ƵԴ  >>********************	
 *[��Ϣ��]			 
 * + u32   //�ļ�������
 * + s8    //�ļ�������
 *[��Ϣ����]
 *    PCMT UI  = > MtMP
 */_event( ev_CodecPcmtVidSourceCmd  )
   _body ( u32  ,1) //�ļ�������
   _body ( s8 , MT_MAX_FULLFILENAME_LEN ) 
   _ev_end
  
/**********************<< Pcmt ѡ���ļ���ƵԴָʾ  >>********************	
 *[��Ϣ��]			 
 * + u32   //�ļ�������
 * + s8    //�ļ�������
 *[��Ϣ����]
 *    PCMT UI  = > MtMP
 */_event( ev_CodecPcmtVidSourceInd  )
   _body ( u32  ,1) //�ļ�������
   _body ( s8 , MT_MAX_FULLFILENAME_LEN ) 
   _ev_end
   
/**********************<< ���û�ȡ��Ƶ���ʻص�ָ��  >>********************	
 *[��Ϣ��]	
 * + EmCodecComponent = emAudioEncoder/emAudioDecoder
 * + u32
 * + u32
 *[��Ϣ����]
 *    PCMT UI  = > MtMP
 */_event( ev_CodecSetAudioPowerCB  )
   _body (EmCodecComponent, 1)
   _body ( u32  ,1) //
   _body ( u32  ,1) //
   _ev_end



/***********************<< ������ý�����  >>************************	
 *[��Ϣ��]
 * + BOOL  bForwardLocal //�Ƿ�ת������   
 *[��Ϣ����]
 *      MtMP = > MtMP
 */ _event ( ev_CodecUpdateStreamMediaParamCmd      )
    _body ( BOOL ,1 )
    _ev_end

/***********************<< ���¼�ز���  >>************************	
 *[��Ϣ��]
 * +  EmSite   
 *[��Ϣ����]
 *      MtMP = > MtMP
 */ _event ( ev_CodecUpdateMonitorParamCmd )
    _body ( EmSite ,1 )
    _ev_end

////////////////////////////////////
/***********************<< DVBע������ >>********************** 
 *[��Ϣ��]
* ��
 *[��Ϣ����]
 *  MtMp  ->DVB
 */_event( ev_CodecDVBRegisterReq )
   _ev_end

/***********************<< DVBע��ȷ�� >>********************** 
 *[��Ϣ��]
 * BOOL TRUE/FALSE
 *[��Ϣ����]
 *  MtMp  <-DVB
 */_event( ev_CodecDVBRegisterAck )
   _body( BOOL, 1 )
   _ev_end

/***********************<< DVBע������ >>********************** 
 *[��Ϣ��]
 * ��
 *[��Ϣ����]
 *  MtMp  ->DVB
 */_event( ev_CodecDVBUnRegisterReq )
   _ev_end

/***********************<< DVBע��ȷ�� >>********************** 
 *[��Ϣ��]
 * BOOL  TRUE/FALSE 
 *[��Ϣ����]
 *  MtMp  <-DVB
 */_event( ev_CodecDVBUnRegisterAck )
   _body ( BOOL, 1 )
   _ev_end

/***********************<< ˫���н��������� >>********************** 
 *[��Ϣ��]
 *  + EmDVBComponent, 
*   + BOOL TRUE / ��ʼ FALSE / ֹͣ
*   + TLocalNetParam
 *[��Ϣ����]
 *  MtMp  ->DVB
 */_event( ev_DVBVideoDecoderRcvCmd )
   _body ( EmDVBComponent , 1 )
  _body ( BOOL, 1 )
  _body ( TDVBLocalNetParam, 1 )
   _ev_end

/***********************<< ˫���н��������� >>********************** 
 *[��Ϣ��]
 *  + EmDVBComponent, 
*   + BOOL TRUE / ��ʼ FALSE / ֹͣ
 *[��Ϣ����]
 *  MtMp  ->DVB
 */_event( ev_DVBVideoDecoderDecCmd )
   _body ( EmDVBComponent , 1 )
  _body ( BOOL, 1 )
   _ev_end

/***********************<< ����˫���ж�̬�غ� >>********************** 
 *[��Ϣ��]
 *  + EmDVBComponent
 *  + u8 ����̬�غ�ֵ
 *  + u8 : �غ����� 
 *[��Ϣ����]
 *  MtMp  ->DVB
 */_event( ev_DVBDynamicPayloadCmd )
   _body ( EmDVBComponent, 1    )
   _body ( u8    , 1    )
   _body ( u8    , 1    )
   _ev_end
   
/***********************<< ˫���н��������� >>********************** 
 *[��Ϣ��]
 *  + EmDVBComponent, 
*   + BOOL TRUE / ��ʼ FALSE / ֹͣ
 *[��Ϣ����]
 *  MtMp  ->DVB
 */_event( ev_DVBVideoDecoderPlyCmd )
   _body ( EmDVBComponent , 1 )
  _body ( BOOL, 1 )
   _ev_end

/***********************<< ����˫������Կ >>********************** 
 *[��Ϣ��]
 *  + EmDVBComponent,
 *  + TEncryptKey ����Կ
 *[��Ϣ����]
 *  MtMp  ->DVB
 */_event( ev_DVBEncryptKeyCmd )
   _body ( EmDVBComponent, 1    )
   _body ( TEncryptKey    , 1    )
   _ev_end   

/***********************<< ����˫���б������ >>********************** 
 *[��Ϣ��]
 *  + TDVBVideoEncParam
 *[��Ϣ����]
 *  MtMp  ->DVB
 */_event( ev_DVBVideoEncodeParamCmd )
  _body ( TDVBVideoEncParam    , 1    )
   _ev_end

/***********************<< ˫���б��������� >>********************** 
 *[��Ϣ��]
 *  + BOOL = TRUE����ʼ����/FALSE��ֹͣ����
 *[��Ϣ����]
 *  MtMp  ->DVB
 */_event( ev_DVBVideoEncodeCmd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< ˫���б��������� >>********************** 
 *[��Ϣ��]
 *  + BOOL = TRUE����ʼ����/FALSE��ֹͣ����
 *[��Ϣ����]
 *  MtMp  ->DVB
 */_event( ev_DVBVideoSendCmd )
   _body ( BOOL    , 1    )  
   _ev_end

/***********************<< ˫���б������ɼ� >>********************** 
 *[��Ϣ��]
 *  + BOOL = TRUE����ʼ�ɼ�/FALSE��ֹͣ�ɼ�
 *[��Ϣ����]
 *  MtMp  ->DVB
 */_event( ev_DVBVideoCapCmd )
   _body ( BOOL    , 1    )
   _ev_end
   
/***********************<< ����˫���б��������͵�ַ >>********************** 
 *[��Ϣ��]
 *  + TDVBNetSndParam //���͵�ַ 
 *[��Ϣ����]
 *  MtMp  ->DVB
 */_event( ev_DVBSetVideoSendAddrCmd )
   _body ( TDVBNetSndParam, 1 )
   _ev_end

/***********************<< ���͹ؼ�֡ >>********************** 
 *[��Ϣ��]
* ��
 *[��Ϣ����]
 *  MtMp  ->DVB
 */_event( ev_DVBSndKeyFrame)
   _ev_end
   
 
 /***********************<< ��ʼPC˫������  >>************************
 *[��Ϣ��] 
 *  +EmMtVideoPort 
 *  +BOOL             TRUE=��ʼ FALSE =ֹͣ
 *  +TIPTransAddr   
 *[��Ϣ����]
 *
 *  mtc =>  MtService 
 */_event(  ev_PCDualVideoCmd   )
   _body ( EmMtVideoPort   , 1 )//��Ƶ�˿�
   _body ( BOOL            , 1 )//TRUE=��ʼ FALSE =ֹͣ
   _body ( TIPTransAddr    , 1 ) //���������ؽ��յ�rtcp��ַ�Ͷ˿�
   _ev_end

/***********************<< PC˫��ָʾ  >>************************
 *[��Ϣ��] 
 *  +EmMtVideoPort 
 *  +BOOL             TRUE=��ʼ FALSE =ֹͣ
 *[��Ϣ����]
 *
 *    MtService <=> ui/mtc
 */_event(  ev_PCDualVideoInd   )
   _body ( EmMtVideoPort   , 1 ) //��Ƶ�˿�
   _body ( BOOL            , 1 ) //TRUE=��ʼ FALSE =ֹͣ
   _body ( BOOL            , 1 ) //TRUE=�ɹ� FALSE= ʧ��
   _ev_end

/***********************<< �����Զ����л�  >>************************
 *[��Ϣ��] 
 *  +BOOL             TRUE/ FALSE
 *[��Ϣ����]
 *
 *    MtService => mtmp
 */_event(  ev_CodecAutoPIPCmd   )
   _body ( BOOL            , 1 )//TRUE=�Զ����л�,FALSE =��
   _ev_end


/***********************<< ���ö���ʱ������������  >>************************
 *[��Ϣ��] 
 *  +TBitRateAdjust  //
 *[��Ϣ����]
 *
 *    MtService => mtmp
 */_event(  ev_CodecRateAdjustCmd   )
   _body ( TBitRateAdjust      , 1 )//����ʱ������������
   _ev_end
   
/***********************<< ��ƵԴ����ָʾ >>**********************	
 *[��Ϣ��]			 
 * EmVideoType : emPriomVideo/emSecondVideo 
 * EmMtVidPort[MT_MAXNUM_VIDSOURCE] : ��֧�ֵ���ƵԴ
 *[��Ϣ����]
 * MtMP -> MtService
 * MtService-> MtUI 
 */_event( ev_CodecVidSourceCapInd )
   _body ( EmVideoType , 1 )  
   _body ( EmMtVideoPort,  MT_MAXNUM_VIDSOURCE )
   _ev_end

/***********************<< ����������������  >>************************
 *[��Ϣ��]
 *  +u8   //����ʱ���ʽ��͵İٷֱ�
 *[��Ϣ����]
 *    mtmp->mtservice
 *    
 */_event(  ev_CodecRequireRateAdjustCmd   )
   _body ( u16      , 1 ) //����ʱ���ʽ��͵İٷֱ�
   _ev_end

/***********************<< ���ٵ�����  >>************************
 *[��Ϣ��]
 *  +u8   //���ٵİٷֱ�
 *[��Ϣ����]
 *    mtmp->mtservice
 *    
 */_event(  ev_CodecRequireIncreaseRateAdjustCmd   )
   _body ( u16      , 1 ) //Ҫ�����ٵİٷֱ�
   _ev_end

/***********************<< ���ٵ�����  >>************************
 *[��Ϣ��]
 *  +BOOL  //�Ƿ���������
 *  +u8    //���ٱ�   
 *  +u16   //���ʱ���
 *  +u16   //��֡��
 *
 *[��Ϣ����]
 *    mtmp->mtservice
 *    
 */_event(  ev_CodecIncreaseRateAdjustCmd   )
   _body ( BOOL       , 1 ) //Ҫ�����ٵİٷֱ�
   _body ( u8, 1 )
   _body ( u16, 1 )
   _body ( u16, 1 )
   _ev_end

   
/**********************<< mt ����˫���б���ͳ����Ϣ  >>********************	
 *[��Ϣ��]
 * ��
 *[��Ϣ����]
 *    Mtmp  = > dvb
 */_event( ev_DVBEncStatisReq  )
   _ev_end

/**********************<< ˫���б���ͳ����Ϣ����  >>********************	
 *[��Ϣ��]
 * TCodecPackStat  //ͳ����Ϣ
 *[��Ϣ����]
 *    Mtmp  = > dvb
 */_event( ev_DVBEncStatisAck  )
   _body( TCodecPackStat , 1 )
   _ev_end
 
	
/**********************<< ���õڶ�·��Ƶ������˿�(vga/video)  >>********************	
 *[��Ϣ��]
 * BOOL bVga ; TRUE:�����vga/FALSE: �����video
 *[��Ϣ����]
 *    mtservice  = > mtmp
 */_event( ev_CodecSetSVOutput  )
   _body( BOOL ,  1 )
   _ev_end
   
/**********************<< mtmp������ָʾ  >>********************	
 *[��Ϣ��]
 * +u16 //mtmp������ֵ
 *[��Ϣ����]
 *    mtmp  = > mtservice
 */_event( ev_CodecErrorCodeInd )
   _body( u16, 1 )
   _ev_end
   
/**********************<< Mic�������  >>********************	
 *[��Ϣ��]
 * +u8 //mic����ֵ
 *[��Ϣ����]
 *    ui  = > mtservice
 */_event( ev_MicAdjustCmd )
   _body( u8, 1 )
   _ev_end

/**********************<< Mic�������ָʾ  >>********************	
 *[��Ϣ��]
 * +u8  //mic����ֵ
 *[��Ϣ����]
 *    mtservice  = > ui/mtc
 */_event( ev_MicAdjustInd )
   _body( u8, 1 )
   _ev_end
  
/**********************<< Osd�ػ�ָʾ  >>********************	
 *[��Ϣ��]
 * ��
 *[��Ϣ����]
 *   mtmp  = > mtservice
 */_event( ev_OsdUpdateInd )
   _ev_end

/***********************<<  ����ͨ���ֱ��� >>********************** 
 *[��Ϣ��]
//*    +EmCodecComponent
//*    +EmVideoResolution
 *[��Ϣ����]
 *  channelservice  -> mtmp
 */_event( ev_CodecAdjustVideoResCmd )
   _body( EmCodecComponent,  1 )
   _body( EmVideoResolution, 1 )   
   _ev_end

/**********************<< ��Ƶ�ź�����ָʾ  >>********************	
 *[��Ϣ��]
 * BOOL         //��һ·��Ƶ�Ƿ����ź�
 * BOOL         //�ڶ�·��Ƶ�Ƿ����ź�
 *[��Ϣ����]
 *   mtmp  ->  mtservice
 *   mtservice ->ui /mtc
 */_event( ev_CodecVideoInputInd )
   _body( BOOL , 1 )
   _body( BOOL, 1 )
   _ev_end
   
/***********************<<  ���ý������鲥���յ�ַ >>********************** 
 *[��Ϣ��]
//*    +EmCodecComponent, 
//*    +u32  ip
//*    +u16  port
 *[��Ϣ����]
 *  h323service -> mtservice(channelservie)
 */_event( ev_CodecSetMultiCastAddrCmd)
   _body( EmCodecComponent, 1 )
   _body( u32_ip,  1 )  
   _body( u16,  1 ) 
   _ev_end

/**********************<< Ntsc��Pal��ʽ�ı�ָʾ  >>********************	
 *[��Ϣ��]
 * ��
 *[��Ϣ����]
 *   mtmp  = > mtservice
 */_event( ev_NtscPalChangeInd )
   _ev_end

/**********************<< ����ƵԴʱ�����;�̬ͼƬ  >>********************	
 *[��Ϣ��]
 * BOOL  //�Ƿ�����ƵԴʱ�����;�̬ͼƬ
 *[��Ϣ����]
 *   mtmp  = > mtservice
 */_event( ev_CodecNoVideoShowPicCmd )
  _body( BOOL, 1 )
   _ev_end   


/**********************<< �����ʽ���һ������  >>********************	
 *[��Ϣ��]
 * u8  //���Ϊ95�������ʵ�����ԭ����95%
 * u8  //����ipͷ�����࣬��Ҫ�ٽ��������ʽ���һ��ֵ
       //Ĭ����90%������Ϊԭ����90%�������������Ϊԭ����ֵ,�������ʽ�
 *[��Ϣ����]
 *   mtservice  = > mtmp
 */_event( ev_CodecBitRateDecsRatioCmd )
  _body( u8, 1 )
  _body( u8, 1 ) 
   _ev_end  
 
/**********************<< ��������������  >>********************	
 *[��Ϣ��]
 * + EmMtLoudspeakerVal 1
 *[��Ϣ����]
 *   mtui|mtc  => mtservice => mtmp
 */_event( ev_LoudspeakerTestCmd )
   _body( EmMtLoudspeakerVal, 1 )
   _ev_end

/**********************<< ����������ָʾ  >>********************	
 *[��Ϣ��]
 * + EmMtLoudspeakerVal 1
 *[��Ϣ����]
 *   mtui|mtc  <= mtservice
 */_event( ev_LoudspeakerTestInd )
   _body( EmMtLoudspeakerVal, 1 )
   _ev_end
   
/**********************<< ����������  >>********************	
 *[��Ϣ��]
 * + EmMtLoudspeakerVal 1  ����
 * + BOOL               1  ����/�ر�
 *[��Ϣ����]
 *   mtui|mtc => mtservice => mtmp_t2
 */_event( ev_PlayRingCmd )
   _body( EmMtLoudspeakerVal, 1 )
   _body( BOOL,               1 )
   _ev_end
   
/**********************<< ���ŵ绰��ť������  >>********************	
 *[��Ϣ��]
 * + s8                 1  ����/�ر�
 *[��Ϣ����]
 *   mtui|mtc => mtservice => mtmp_t2
 */_event( ev_PlayDialRingCmd )
   _body( s8,                 1 )
   _ev_end

/**********************<< ����ָʾ  >>********************	
 *[��Ϣ��]
 * + BOOL         1
 *[��Ϣ����]
 *   mtui|mtc  <= mtservice
 */_event( ev_PackLostInd )
   _body( BOOL,          1)  
   _ev_end
   

/**********************<< ������Ƶ����Ļ���  >>********************	
 *[��Ϣ��]
 * + u32,  dwStartRcvBuf
 * + u32,  dwFastRcvBuf
 *[��Ϣ����]
 *   mtui|mtc  <= mtservice
 */_event( ev_CodecAudioDecodeBufCfg )
   _body( u32,     1 )  
   _body( u32,     1 )
   _ev_end
   

/**********************<< ����mic��������  >>********************	
 *[��Ϣ��]
 * +u8     mic����ֵ
 *[��Ϣ����]
 *   mtui|mtc  <= mtservice
 */_event( ev_CodecSetMicCmd )
   _body( u8,     1 )  
   _ev_end

/**********************<< ��ȡmic��������  >>********************	
 *[��Ϣ��]

 *[��Ϣ����]
 *   mtservice => codec
 */_event( ev_CodecGetMicCmd )
   _ev_end

/**********************<< ��ȡmic����ָʾ  >>********************	
 *[��Ϣ��]
 * +u8     mic����ֵ
 *[��Ϣ����]
 *   mtservice <= codec
 */_event( ev_CodecGetMicInd )
   _body( u8,     1 )  
   _ev_end


/**********************<< OSD�Ƿ��ָʾ  >>********************	
 *[��Ϣ��]
 * +BOOL    �Ƿ��
 *[��Ϣ����]
 *   ui => codec
 */_event( ev_CodecOsdOpenInd )
   _body( BOOL,     1 )  
   _ev_end
   
/**********************<< T2CPU�쳣ָʾ  >>********************	
 *[��Ϣ��]
 * +u32    CPU Type
 *[��Ϣ����]
 *   codec => mtservice => ui
 */_event( ev_CodecCpuResetInd )
   _body( u32,     1 )  
   _ev_end

/**********************<< ����pcmt��Ƶ�������ػ�  >>********************	
 *[��Ϣ��]
 * +EmCodecComponet 
 *[��Ϣ����]
 *  mtservice => mtmp
 */_event( ev_CodecRedrawLastFrameCmd )
   _body( EmCodecComponent,     1 )  
   _ev_end
   
/**********************<< T2 Osd��ʾλ��ָʾ  >>********************	
 *[��Ϣ��]
 * +u32    map Id
 *[��Ϣ����]
 *   codec => mtservice => ui
 */_event( ev_CodecSetOsdMapIdInd )
   _body( u32,     1 )  
   _ev_end


/**********************<< FEC�㷨��������  >>********************	
 *[��Ϣ��]
 * +u8    FEC�㷨����
 *[��Ϣ����]
 *    mtservice => codec
 */_event( ev_CodecSetFecCapacityTypeCmd )
   _body( u8,     1 )  
   _ev_end


/**********************<< FEC�㷨����ָʾ  >>********************	
 *[��Ϣ��]
 * +u8    FEC�㷨����
 *[��Ϣ����]
 *    mtservice => UI
 */_event( ev_CodecSetFecCapacityTypeInd )
   _body( u8,     1 )  
   _ev_end
   
/**********************<< G7221C�ֽ���ת  >>********************	
 *[��Ϣ��]
 * BOOL bReverse
 *[��Ϣ����]
 *   mtservice => codec
 */_event( ev_CodecSetReverseG7221cCmd )
   _body( BOOL,     1 )  
   _ev_end

  /*********************<< Զ�˻ػ��������� >>************************
 *[��Ϣ��]	
 * + char[]
 *
 *[��Ϣ����]
 *    Service  = > UI
 *
*/_event(  ev_RemoteLoopBackTestCmd )
  _body( EmVideoType,   1 )
  _body( BOOL, 1 )
  _ev_end


  /*********************<< Զ�˻ػ�����״ָ̬ʾ  >>************************
 *[��Ϣ��]	
 * + char[]
 *
 *[��Ϣ����]
 *    Service  = > UI
 *
*/_event(  ev_RemoteLoopBackTestInd )
  _body( EmVideoType,   1 )
  _body( BOOL, 1 )
  _ev_end

#ifndef _MakeMtEventDescription_
   _ev_segment_end(codec)
};
#endif

#endif
