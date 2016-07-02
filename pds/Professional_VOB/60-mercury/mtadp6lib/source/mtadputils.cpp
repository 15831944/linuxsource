/*****************************************************************************
   ģ����      : mtadp
   �ļ���      : mtadputils.cpp
   ����ļ�    : mtadputils.h, evmcumt.h
   �ļ�ʵ�ֹ���: ����һЩ���ݽṹ��ת��
   ����        : Tan Guang
   �汾        : V1.0  Copyright(C) 2003-2004 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��		�汾        �޸���      �߶���		�޸�����
   2003/11/11	1.0         Tan Guang				����
   2005/12/30	4.0			�ű���					����3.6�ն�
******************************************************************************/

#include "osp.h"
#include "cm.h"
#include "stkutils.h"
#include "mcustruct.h"
#include "mcuver.h"
#include "mtadp.h"
#include "evmcumt.h"
#include "evmcu.h"
//#include "h323adapter.h"
#include "mtadputils.h"

/*=============================================================================
  �� �� ���� PackMtInfoListIntoBuf
  ��    �ܣ� ���ն���Ϣ�б��Խ��շ�ʽ���뻺�����������H323ID��E164
			 ��������������TMt + type(u8) + length(u16) + StrBuf(u8*)
			 Ϊ��Ԫ���б�����Ǵ����ַ��ʽ�ı�������������TMt+
			 type(u8)+TTransportAddrΪ��Ԫ���б�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const TMt	atMt[],	ԴTMt����
			 const TMtAlias atMtAlias[], ԴTMtAlias����
			 u8 byArraySize, �������������Ԫ�ظ���
			 u8 *pbyBuf,	 Ŀ�Ļ������׵�ַ
			 u32 dwBufLen)	 Ԥ�����Ŀ�Ļ���������
			 u8 *pbyBuf,	 ���Ļ�����
  �� �� ֵ�� (u32)ʵ�����Ļ������ֽ���
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang                  ����
=============================================================================*/
u32 CMtAdpUtils::PackMtInfoListIntoBuf( const TMt      atMt[], 
									    const TMtAlias atMtAlias[], 
									    u8  byArraySize, 
									    u8* pbyBuf, 
									    u32 dwBufLen )	
{
	u8 *pbyTmp = pbyBuf;
	u8 *pbyMemLimit = pbyBuf + dwBufLen;

	for( s32 nArrySize = 0; nArrySize < byArraySize; nArrySize ++ )
	{
		if(!TRY_MEM_COPY(pbyTmp, (void*)&atMt[nArrySize], sizeof(TMt), pbyMemLimit))
        {
			return (pbyTmp - pbyBuf);
        }
		pbyTmp += sizeof(TMt);

		u8 byType = atMtAlias[nArrySize].m_AliasType;		
		if( !TRY_MEM_COPY(pbyTmp, &byType, sizeof(byType), pbyMemLimit) )
        {
			return (pbyTmp - pbyBuf);
        }
		pbyTmp += sizeof(byType);			

		if( (u8)mtAliasTypeE164 == byType || (u8)mtAliasTypeH323ID == byType || (u8)mtAliasTypeH320ID == byType || (u8)mtAliasTypeH320Alias == byType)
		{
			u16 wAliasLen = htons(strlen(atMtAlias[nArrySize].m_achAlias));
			if( !TRY_MEM_COPY( pbyTmp, &wAliasLen, sizeof(wAliasLen), pbyMemLimit) )
            {
				return ( pbyTmp - pbyBuf );
            }
			pbyTmp += sizeof(wAliasLen);

			if( !TRY_MEM_COPY(pbyTmp, (void*)atMtAlias[nArrySize].m_achAlias, wAliasLen, pbyMemLimit) )
            {
				return (pbyTmp - pbyBuf);
            }
			pbyTmp += wAliasLen;
		}
		else if( (u8)mtAliasTypeTransportAddress == byType )
		{
			if( !TRY_MEM_COPY( pbyTmp, (void*)&( atMtAlias[nArrySize].m_tTransportAddr ), sizeof(TTransportAddr), pbyMemLimit) )
            {
                return (pbyTmp - pbyBuf);
            }
			pbyTmp += sizeof(TTransportAddr);
		}
		else
		{
			StaticLog( "error mt alias detected in PackMtInfoListIntoBuf(): %d!\n", byType );
		}
	}
	
	return ( pbyTmp - pbyBuf );
}


/*=============================================================================
  �� �� ���� UnpackMtInfoListFromBuf
  ��    �ܣ� �ӻ������ж���TMt��TMtAlias����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pbyBuf - ��PackMtInfoListIntoBuf�������Ļ�����
		  �� dwBufLen - pbyBuf����������
		  �� atMt[] - Ŀ��TMt����
          �� atMtAlias[] - Ŀ��TMtAlias����
	      �� u8 byArraySize - �������������Ԫ�ظ���
  �� �� ֵ�� (u8)ʵ������TMt��TMtAlias�ṹ�ĸ���
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang                  ����
=============================================================================*/
u8 CMtAdpUtils::UnpackMtInfoListFromBuf(const u8	*pbyBuf,	
										u32			dwBufLen,	
										TMt			atMt[],		
										TMtAlias	atMtAlias[],	
										u8			byArraySize)
{
	const u8 *pbyTmp = pbyBuf;	
	const u8 *pbyLimit = pbyBuf + dwBufLen;
	u8 byIdx = 0;
	while( pbyTmp <= pbyLimit )
	{
		if( pbyTmp + sizeof(TMt) > pbyLimit ) 
			break;

		atMt[byIdx] = *(TMt*)pbyTmp;

		if( atMt[byIdx].GetMtId() == 0 || atMt[byIdx].GetMtId() >= 193 )
		{
			StaticLog( "error mt id detected!\n");
		}		
		pbyTmp += sizeof(TMt);

		if( pbyTmp + sizeof(u8) > pbyLimit ) 
        {
            break;		
        }

		u8 byType = *(u8*)pbyTmp;

		atMtAlias[byIdx].m_AliasType = /*(mtAliasType)*/byType;
		pbyTmp += sizeof(u8);

		if( (u8)mtAliasTypeE164 == byType || (u8)mtAliasTypeH323ID == byType || (u8)mtAliasTypeH320ID == byType || (u8)mtAliasTypeH320Alias == byType )
		{

			if( pbyTmp + sizeof(u16) > pbyLimit ) 
            {
                break;
            }
			u16 aliasLen = ntohs(*(u16*)pbyTmp);
			pbyTmp += sizeof(u16);

			if(pbyTmp + aliasLen > pbyLimit) 
            {
				break;
            }
			memcpy( atMtAlias[byIdx].m_achAlias, pbyTmp, aliasLen );
			if( aliasLen < sizeof(atMtAlias[byIdx].m_achAlias) )
            {
				atMtAlias[byIdx].m_achAlias[aliasLen] = '\0';
            }
			pbyTmp += aliasLen;
		}
		else if( (u8)mtAliasTypeTransportAddress == byType )
		{
			if( pbyTmp + sizeof(TTransportAddr) > pbyLimit ) 
            {
				break;
            }
			atMtAlias[byIdx].m_tTransportAddr = *(TTransportAddr*)pbyTmp;
			pbyTmp += sizeof(TTransportAddr);
		}	
		else
		{
			StaticLog( "error mt alias detected in UnpackMtInfoListFromBuf(): %d!\n", byType);
		}

		byIdx++;
		if( byIdx >= byArraySize ) 
        {
			break;
        }
	}

	return byIdx;
}


/*=============================================================================
  �� �� ���� TConfList2FacilityInfo
  ��    �ܣ� TConfList����ת����FacilityInfo�ṹ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const u8		*pbyBuf, 
			 s32 bufLen, 
			 TFACILITYINFO	&tFacilityInfo
  �� �� ֵ�� s8   
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
 2003/11/11    1.0			TanGuang                  ����
=============================================================================*/
void CMtAdpUtils::TConfList2FacilityInfo(const u8		*pbyBuf, 
										 /*s32*/u16			 wBufLen, 
										 TFACILITYINFO	&tFacilityInfo )
{
	if( NULL == pbyBuf )
		return;

	u16 wConfs = wBufLen / sizeof(TConfNameInfo);
	tFacilityInfo.m_tConferences.m_byNum = (u8)wConfs;
	
    tFacilityInfo.m_byReason = (u8)confListChoice;

	for( u16 nConfNum = 0; nConfNum < wConfs; nConfNum++ )
	{	
		TConfNameInfo *pConf = (TConfNameInfo*)(pbyBuf + nConfNum * sizeof(TConfNameInfo));
		pConf->m_cConfId.GetConfId((u8*)tFacilityInfo.m_tConferences.m_atConfInfo[nConfNum].m_abyConfId, MAXLEN_CONFID);
		tFacilityInfo.m_tConferences.m_atConfInfo[nConfNum].m_tConfAlias.m_emType = cat_alias;
		tFacilityInfo.m_tConferences.m_atConfInfo[nConfNum].m_tConfAlias.m_tAlias.m_emType = type_h323ID;
		strncpy( tFacilityInfo.m_tConferences.m_atConfInfo[nConfNum].m_tConfAlias.m_tAlias.m_achAlias, 
			     pConf->achConfName, MAXLEN_CONFNAME - 1 );
		tFacilityInfo.m_tConferences.m_atConfInfo[nConfNum].m_tConfAlias.m_tAlias.m_achAlias[MAXLEN_CONFNAME - 1] = '\0';
	}

	return;
}


/*=============================================================================
  �� �� ���� TFacilityInfo2TConfList
  ��    �ܣ� FacilityInfo�ṹת����TConfList����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const u8		*pbyBuf, 
			 s32 bufLen, 
			 TFACILITYINFO	&tFacilityInfo
  �� �� ֵ�� (s32)ʵ�������ֽ���
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
 2003/11/11    1.0			TanGuang                  ����
=============================================================================*/
// s32 CMtAdpUtils::TFacilityInfo2TConfList(const TFACILITYINFO tFacilityInfo, 
// 										 u8					*pbybuf, 
// 										 s32				 nBufLen)
// {
// //	if( NULL == pbybuf )
// //		return 0;
// 
// 	s32 nRealLen = 0;
// 	s32 nConfs = min( nBufLen / sizeof(TConfNameInfo), tFacilityInfo.m_tConferences.m_byNum );
// 	TConfNameInfo tConfList;
// 	memset( &tConfList, 0, sizeof(tConfList) );
// 	memcpy( &tConfList, pbybuf, sizeof(tConfList) );
// 	
// 	u8 *pbyTmp = pbybuf;
// 
// 	for( s32 nConfNum = 0; nConfNum < nConfs; nConfNum++ )
// 	{
// 		tConfList.m_cConfId.SetConfId( (u8*)tFacilityInfo.m_tConferences.m_atConfInfo[nConfNum].m_abyConfId, MAXLEN_CONFID );
// 
// 		strncpy( tConfList.achConfName, 
// 			     tFacilityInfo.m_tConferences.m_atConfInfo[nConfNum].m_tConfAlias.m_tAlias.m_achAlias,
// 			     sizeof(tConfList.achConfName) - 1 );	
// 
// 		tConfList.achConfName[sizeof(tConfList.achConfName) - 1] = '\0';
// 		
// 		pbyTmp += sizeof(TConfNameInfo);
// 		memset( &tConfList, 0, sizeof(tConfList) );
// 		memcpy( &tConfList, pbyTmp, sizeof(tConfList) );
// 		nRealLen += sizeof(TConfNameInfo);
// 	}
// 
// 	return nRealLen;
// }


/*=============================================================================
  �� �� ���� PayloadTypeIn2Out
  ��    �ܣ� �ڲ�ý������ת��Ϊ�ⲿý������ 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 payloadType
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang              ����
  2005/11/11    4.0			����                ������Ӧý������
=============================================================================*/
u8 CMtAdpUtils::PayloadTypeIn2Out( u8 byPayloadType )
{
	switch( byPayloadType ) 
	{
	//��Ƶ
	case MEDIA_TYPE_PCMA:			return (u8)a_g711a;		
	case MEDIA_TYPE_PCMU:			return (u8)a_g711u;		
	case MEDIA_TYPE_G722:			return (u8)a_g722;
	case MEDIA_TYPE_G7231:			return (u8)a_g7231;
	case MEDIA_TYPE_G728:			return (u8)a_g728;
	case MEDIA_TYPE_G729:			return (u8)a_g729;
	case MEDIA_TYPE_MP3:			return (u8)a_mp3;
	case MEDIA_TYPE_G7221C:			return (u8)a_g7221;
    case MEDIA_TYPE_AACLC:          return (u8)a_mpegaaclc;
    case MEDIA_TYPE_AACLD:          return (u8)a_mpegaacld;
	case MEDIA_TYPE_G719:			return (u8)a_g719;
	//��Ƶ
	case MEDIA_TYPE_H261:			return (u8)v_h261;
	case MEDIA_TYPE_H262:			return (u8)v_h262;
	case MEDIA_TYPE_H263:			return (u8)v_h263;
	case MEDIA_TYPE_H263PLUS:		return (u8)v_h263plus;
	case MEDIA_TYPE_H264:			return (u8)v_h264;
	case MEDIA_TYPE_MP4:			return (u8)v_mpeg4;
	//����
	case MEDIA_TYPE_H224:			return (u8)d_h224;
	case MEDIA_TYPE_T120:			return (u8)d_t120;
	case MEDIA_TYPE_MMCU:			return (u8)t_mmcu;
	//���Ϳ�
	case MEDIA_TYPE_NULL:			return (u8)t_none;		
	default:						return (u8)t_none;
	}
}


/*=============================================================================
  �� �� ���� PayloadTypeOut2In
  ��    �ܣ� �ⲿý������ת��Ϊ�ڲ�ý������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 payLoadType
             u8 &payload
             u8 &mediaType
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang              ����
  2005/11/11    4.0			����                ������Ӧý������
=============================================================================*/
void CMtAdpUtils::PayloadTypeOut2In( u8 byPayLoadType, u8 &byPayload, u8 &byMediaType )
{
	switch( byPayLoadType ) 
	{
	//��Ƶ��ʽ
	case a_g711a:	 byPayload = MEDIA_TYPE_PCMA;    byMediaType = MODE_AUDIO; break;
	case a_g711u:	 byPayload = MEDIA_TYPE_PCMU;    byMediaType = MODE_AUDIO; break;
	case a_g722:	 byPayload = MEDIA_TYPE_G722;    byMediaType = MODE_AUDIO; break;
	case a_g7231:	 byPayload = MEDIA_TYPE_G7231;   byMediaType = MODE_AUDIO; break;
	case a_g728:	 byPayload = MEDIA_TYPE_G728;	 byMediaType = MODE_AUDIO; break;
	case a_g729:	 byPayload = MEDIA_TYPE_G729;    byMediaType = MODE_AUDIO; break;
	case a_mp3:	     byPayload = MEDIA_TYPE_MP3;	 byMediaType = MODE_AUDIO; break;
	case a_g7221:	 byPayload = MEDIA_TYPE_G7221C;	 byMediaType = MODE_AUDIO; break;
    case a_mpegaaclc: byPayload = MEDIA_TYPE_AACLC;	 byMediaType = MODE_AUDIO; break;
    case a_mpegaacld: byPayload = MEDIA_TYPE_AACLD;	 byMediaType = MODE_AUDIO; break;
	case a_g719:	byPayload = MEDIA_TYPE_G719;	 byMediaType = MODE_AUDIO; break;
	//��Ƶ��ʽ
	case v_h261:	 byPayload = MEDIA_TYPE_H261;    byMediaType = MODE_VIDEO; break;
	case v_h262:	 byPayload = MEDIA_TYPE_H262;    byMediaType = MODE_VIDEO; break;
	case v_h263:	 byPayload = MEDIA_TYPE_H263;	 byMediaType = MODE_VIDEO; break;
	case v_h263plus: byPayload = MEDIA_TYPE_H263PLUS;byMediaType = MODE_VIDEO; break;
	case v_h264:	 byPayload = MEDIA_TYPE_H264;	 byMediaType = MODE_VIDEO; break;
	case v_mpeg4:	 byPayload = MEDIA_TYPE_MP4;     byMediaType = MODE_VIDEO; break;
	//���ݸ�ʽ
	case d_t120:	 byPayload = MEDIA_TYPE_T120;	 byMediaType = MODE_DATA;  break;
	case d_h224:	 byPayload = MEDIA_TYPE_H224;	 byMediaType = MODE_DATA;  break;
	case t_mmcu:     byPayload = MEDIA_TYPE_MMCU;    byMediaType = MODE_DATA;  break;

	default:		 byPayload = MEDIA_TYPE_NULL;    byMediaType = MODE_NONE;  break;
	}
	return;
}


/*=============================================================================
  �� �� ���� PayloadTypeOut2In
  ��    �ܣ� �ⲿý������ת��Ϊ�ڲ�ý������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 payLoadType
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang                  ����
=============================================================================*/
u8 CMtAdpUtils::PayloadTypeOut2In( u8 byPayLoadType )
{
	u8 byOutPayloadType = byPayLoadType;
	u8 byInPayloadType  = MEDIA_TYPE_NULL;
	u8 byInMediaType    = MODE_NONE;

	PayloadTypeOut2In( byOutPayloadType, byInPayloadType, byInMediaType);
	
	return byInPayloadType;
}

/*=============================================================================
  �� �� ���� CameraCommandOut2In
  ��    �ܣ� �ⲿ���������ת��Ϊ�ڲ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TFeccStruct &tFeccStruct
             u16 &wEvent
             u8 &byParam
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang                  ����
=============================================================================*/
void CMtAdpUtils::CameraCommandOut2In( TFeccStruct &tFeccStruct, u16 &wEvent, u8 &byParam )
{
	static u16 wMessage = 0;
	static u8  byAction = 0;

	switch(tFeccStruct.m_byAction)
	{
	case actionStart:
	case actionStop:
		{
			if(tFeccStruct.m_byAction == (u8)actionStart)
			{
				wMessage = MT_MCU_MTCAMERA_CTRL_CMD; 
			}
			else
			{
				wMessage = MT_MCU_MTCAMERA_CTRL_STOP; 
			}
			switch(tFeccStruct.arguments.m_byRequest)
			{
			case requestTiltUp:		byAction = CAMERA_CTRL_UP;			break;
			case requestTiltDown:	byAction = CAMERA_CTRL_DOWN;		break;
			case requestPanLeft:	byAction = CAMERA_CTRL_LEFT;		break;
			case requestPanRight:	byAction = CAMERA_CTRL_RIGHT;		break;
			case requestFocusIn:	byAction = CAMERA_CTRL_FOCUSIN;		break;
			case requestFocusOut:	byAction = CAMERA_CTRL_FOCUSOUT;	break;
			case requestZoomIn:		byAction = CAMERA_CTRL_ZOOMIN;		break;
			case requestZoomOut:	byAction = CAMERA_CTRL_ZOOMOUT;		break;
			case requestBrightnessUp:	byAction = CAMERA_CTRL_BRIGHTUP;	break;
			case requestBrightnessDown: byAction = CAMERA_CTRL_BRIGHTDOWN;	break;
			default:	
				break;
			}
		}				
		break;

	case actionContinue:
		break;
		
	case actionVideoSourceSwitched:
        wMessage = MT_MCU_VIDEOSOURCESWITCHED_CMD;        
		byAction = tFeccStruct.arguments.m_bySourceNo;
        break;
		
	case actionSelectVideoSource:
		wMessage = MT_MCU_SELECTVIDEOSOURCE_CMD;
		byAction = tFeccStruct.arguments.m_bySourceNo;
		break;

	case actionStoreAsPreset:
		wMessage = MT_MCU_MTCAMERA_SAVETOPOS_CMD;
		byAction = tFeccStruct.arguments.m_byPresetNo;
		break;

	case actionActivatePreset:
		wMessage = MT_MCU_MTCAMERA_MOVETOPOS_CMD;
		byAction = tFeccStruct.arguments.m_byPresetNo;
		break;	

	case actionAutoFocus:
		wMessage = MT_MCU_MTCAMERA_CTRL_CMD; 
		byAction = CAMERA_CTRL_AUTOFOCUS;
		break;

	default:
		break;
	}
	wEvent  = wMessage;
	byParam = byAction;

	return;
}

 
/*=============================================================================
  �� �� ���� CameraCommandIn2Out
  ��    �ܣ� �ڲ����������ת��Ϊ�ⲿ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const CServMsg &cMsg
             TFeccStruct    &tFeccStruct
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang                  ����
=============================================================================*/
void CMtAdpUtils::CameraCommandIn2Out( const CServMsg &cMsg, TFeccStruct &tFeccStruct)
{
 	if( cMsg.GetMsgBodyLen() < sizeof(TMt) + 1 ) 
		return;
	
//	TMt tMt = *(TMt*)cMsg.GetMsgBody();
	u8  byFeccParam = *(u8*)( cMsg.GetMsgBody() + sizeof(TMt) );	

	switch( cMsg.GetEventId() ) 
	{
	case MCU_MT_MTCAMERA_CTRL_CMD:
	case MCU_MT_MTCAMERA_CTRL_STOP:
		{
			if( cMsg.GetEventId() == MCU_MT_MTCAMERA_CTRL_CMD )
			{
				tFeccStruct.m_byAction = (u8)actionStart;
				tFeccStruct.m_byTimeout = 0x08;
			}
			else
			{
				tFeccStruct.m_byAction = (u8)actionStop;
			}
			switch(byFeccParam) 
			{
			case CAMERA_CTRL_UP: 
				tFeccStruct.arguments.m_byRequest = (u8)requestTiltUp; 				
				break;
				
			case CAMERA_CTRL_DOWN: 
				tFeccStruct.arguments.m_byRequest = (u8)requestTiltDown; 
				break;

			case CAMERA_CTRL_LEFT:
				tFeccStruct.arguments.m_byRequest = (u8)requestPanLeft; 
				break;

			case CAMERA_CTRL_RIGHT:
				tFeccStruct.arguments.m_byRequest = (u8)requestPanRight; 
				break;

			case  CAMERA_CTRL_UPLEFT:
				tFeccStruct.arguments.m_byRequest = (u8)requestPanLeft | (u8)requestTiltUp; 
				break;

			case  CAMERA_CTRL_UPRIGHT:
				tFeccStruct.arguments.m_byRequest = (u8)requestPanRight | (u8)requestTiltUp; 
				break;

			case CAMERA_CTRL_DOWNLEFT:
				tFeccStruct.arguments.m_byRequest = (u8)requestPanLeft | (u8)requestTiltDown; 
				break;

			case CAMERA_CTRL_DOWNRIGHT:
				tFeccStruct.arguments.m_byRequest = (u8)requestPanRight | (u8)requestTiltDown; 
				break;

			case CAMERA_CTRL_ZOOMIN:
				tFeccStruct.arguments.m_byRequest = (u8)requestZoomIn; 
				break;

			case CAMERA_CTRL_ZOOMOUT:
				tFeccStruct.arguments.m_byRequest = (u8)requestZoomOut; 
				break;

			case CAMERA_CTRL_FOCUSIN:
				tFeccStruct.arguments.m_byRequest = (u8)requestFocusIn; 
				break;

			case CAMERA_CTRL_FOCUSOUT:
				tFeccStruct.arguments.m_byRequest = (u8)requestFocusOut; 
				break;

			case CAMERA_CTRL_BRIGHTUP:				
				tFeccStruct.arguments.m_byRequest = (u8)requestBrightnessUp;
				break;

			case CAMERA_CTRL_BRIGHTDOWN:				
				tFeccStruct.arguments.m_byRequest = (u8)requestBrightnessDown;
				break;

			case CAMERA_CTRL_AUTOFOCUS:
				tFeccStruct.m_byAction = (u8)actionAutoFocus;
				break;
				
			default:
				break;
			}
		}
		break;
	case MCU_MT_MTCAMERA_RCENABLE_CMD:		
		break;

	case MCU_MT_MTCAMERA_SAVETOPOS_CMD:
		tFeccStruct.m_byAction = (u8)actionStoreAsPreset;
		tFeccStruct.arguments.m_byPresetNo = byFeccParam;
		break;

	case MCU_MT_MTCAMERA_MOVETOPOS_CMD:
		tFeccStruct.m_byAction = (u8)actionActivatePreset;
		tFeccStruct.arguments.m_byPresetNo = byFeccParam;
		break;
		
	case MCU_MT_VIDEOSOURCESWITCHED_CMD:
		tFeccStruct.m_byAction = (u8)actionVideoSourceSwitched;
		tFeccStruct.arguments.m_bySourceNo = byFeccParam;
		break;

	case MCU_MT_SETMTVIDSRC_CMD:
		tFeccStruct.m_byAction = (u8)actionSelectVideoSource;
		tFeccStruct.arguments.m_bySourceNo = byFeccParam;
		break;

	default:
		tFeccStruct.m_byAction = (u8)actionInvalid;
		break;
	}	
	return;
}


/*=============================================================================
  �� �� ���� TMtStatusOut2In
  ��    �ܣ� �ⲿTTERSTATUS�ṹת��Ϊ�ڲ�TMtStatus�ṹ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const _TTERSTATUS &tTerStatus
  �� �� ֵ�� TMtStatus 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang                  ����
=============================================================================*/
TMtStatus CMtAdpUtils::TMtStatusOut2In(_TTERSTATUS &tTerStatus)
{
	TMtStatus tMtStatus;
	memset( &tMtStatus, 0, sizeof(tMtStatus) );
	tMtStatus.SetDecoderMute( tTerStatus.IsQuiet() );
    tMtStatus.SetCaptureMute( tTerStatus.IsMute() );
	tMtStatus.SetHasMatrix( tTerStatus.GetMatrixStatus() );
	//tMtStatus.SetSelPolling( tTerStatus.GetPollStat() != poll_status_none );
    tMtStatus.SetVideoLose( tTerStatus.IsVideoLose() );
	tMtStatus.SetDecoderVolume( tTerStatus.GetOutputVolume() );
	tMtStatus.SetCaptureVolume( tTerStatus.GetInputVolume() );
	tMtStatus.SetCurVideo( tTerStatus.GetVideoSrc() );
	tMtStatus.SetMtBoardType( tTerStatus.GetBoardType());
    tMtStatus.SetExVideoSrcNum(tTerStatus.GetExVidSrcNum());

	return tMtStatus;
}


/*=============================================================================
  �� �� ���� TMtStatusIn2Out
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const TMtStatus &tMtStatus
  �� �� ֵ�� _TTERSTATUS 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang                  ����
=============================================================================*/
_TTERSTATUS CMtAdpUtils::TMtStatusIn2Out( TMtStatus &tMtStatus )
{
	_TTERSTATUS tTerStatus;
	memset( &tTerStatus, 0, sizeof(tTerStatus) );
	tTerStatus.SetQuiet(tMtStatus.IsDecoderMute());
	tTerStatus.SetMute(tMtStatus.IsCaptureMute());
	tTerStatus.SetRemoteCtrlEnalbe(tMtStatus.IsEnableFECC());
	
	if(tMtStatus.IsHasMatrix())	
	{
		tTerStatus.SetMatrixStatus(1);	
	}
	tTerStatus.SetInputVolume(tMtStatus.GetCaptureVolume());
	tTerStatus.SetOutputVolume(tMtStatus.GetDecoderVolume());

    tTerStatus.SetVideoLose(tMtStatus.IsVideoLose());
    /*
	if(tMtStatus.IsSelPolling())
	{
        tTerStatus.SetPollStat(poll_status_normal);
	}
    else
	{
        tTerStatus.SetPollStat(poll_status_none);
	}
    */
	tTerStatus.SetBoardType(tMtStatus.GetMtBoardType());
    tTerStatus.SetExVidSrcNum(tMtStatus.GetExVideoSrcNum());
		
	return tTerStatus;
}

/*=============================================================================
  �� �� ����  TMtBitrateOut2In
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� _TTERBITRATE &tTerBitrate
  �� �� ֵ�� TMtBitrate  
=============================================================================*/
TMtBitrate   CMtAdpUtils::TMtBitrateOut2In( _TTERBITRATE &tTerBitrate )
{
    u16 wFstBitrate, wSecBitrate;
    TMtBitrate tMtBitrate;

    tTerBitrate.GetSendAverageBitRate(wFstBitrate, wSecBitrate);
    tMtBitrate.SetSendBitRate(wFstBitrate);
    tMtBitrate.SetH239SendBitRate(wSecBitrate);

    tTerBitrate.GetRecvAverageBitRate(wFstBitrate, wSecBitrate);
    tMtBitrate.SetRecvBitRate(wFstBitrate);
    tMtBitrate.SetH239RecvBitRate(wSecBitrate);

    return tMtBitrate;
}

/*=============================================================================
  �� �� ���� CapabilitiesSetIn2Out
  ��    �ܣ� �ڲ��������ṹת��Ϊ�ⲿ�ṹ�������ṹ 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TCapSupport &tCapSupport,
			 TCapSet     *ptCapSet,
			 u8           byVendorId
             TCapSupportEx &tCapEx: ��������ڶ�˫������
			 u8 byAudioTrackNum	 ��Ƶ������
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang              ����
  2004/12/30                xsl                   ����ģʽ���ü����㷨
  2005/11/11    4.0         wancl                 �����Ż�
  2005/12/21	4.0			�ű���				  ����֡������
  2007/12/01    4.0R5       ����                H.264����
  2008/04/15    4.0R5       ����                H.264����
  2008/11/15    4.0R5       �ű���                ��������ڶ�˫������
  2012/03/16	4.7			�´�ΰ                ��������������
=============================================================================*/
void CMtAdpUtils::CapabilitiesSetIn2Out(const TCapSupport &tCapSupport,
                                        const TCapSupportEx &tCapEx,
										TCapSet     *ptCapSet,
										u8           byVendorId,
										TAudioTypeDesc *ptAudioTypeDesc,
										u8 byAudioCapNum,
										const TVideoStreamCap &tDsStreamCapex
										)
{
	TVideoCap   tVideoCap;				//Э��ջ��Ƶ����
    TH264VideoCap tH264Cap;             //Э��ջH.264��Ƶ����
//	TDStreamCap tDStreamCap;			//Э��ջ˫������
    TDStreamCap tDStreamCap2;           //�ڶ�˫������

	TSimCapSet  tSimuCap1, tSimuCap2;
	tSimuCap1   = tCapSupport.GetMainSimCapSet();
	tSimuCap2   = tCapSupport.GetSecondSimCapSet();
//	tDStreamCap = tCapSupport.GetDStreamCapSet();

	s32 nPos = 0;
	s32 nChoicePos = 0;
	u8  byPayload  = 0;
    //u8  byPayload2 = 0;
	u8  byCapIndex = 0;
    u8  byFstH264SubId = 0, bySecH264SubId = 0;
	
	//���õ�һ·��Ƶ������Ŀ�Լ�������
	nPos = 0;
	nChoicePos = 0;
	//��������    �ӽӿ���ȡ���������Ƶ��������д��ptCapSet�У��Ա�֮��֪ͨԶ�˱�������
	if (byAudioCapNum > 0)
	{
		for (u8 byAudioCapIdx = 0;byAudioCapIdx < byAudioCapNum;byAudioCapIdx++,ptAudioTypeDesc++)
		{
			if (ptAudioTypeDesc == NULL)
			{
				StaticLog( "[CapabilitiesSetIn2Out]ptAudioTypeDesc == NULL,So Break!\n");
				break;
			}
			byPayload  = PayloadTypeIn2Out(ptAudioTypeDesc->GetAudioMediaType());
			if( (u8)t_none != byPayload )
			{			
				if( (u8)a_g7231 == byPayload )
				{
					TG7231Cap tG7231Cap;
					tG7231Cap.SetCapDirection( cmCapReceiveAndTransmit );
					ptCapSet->SetG7231Cap( tG7231Cap );
				}
				else
				{	
					if( (u8)a_g7221 == byPayload && MT_MANU_NETMEETING == byVendorId )
					{  
						StaticLog( "netmeeting limit test_main!\n");				
					}
					else if ( (u8)a_mpegaaclc == byPayload || (u8)a_mpegaacld == byPayload )
					{
						TAACCap tAACCap;
						tAACCap.SetCapDirection(cmCapReceiveAndTransmit);
						tAACCap.SetChnl(GetAACChnnNum(ptAudioTypeDesc->GetAudioTrackNum()));
						tAACCap.SetSampleFreq(tagAACCap::emFs32000);
						ptCapSet->SetAACCap(tAACCap, byPayload);
					}
					else
					{
						TAudioCap tAudioCap;
						tAudioCap.SetCapDirection( cmCapReceiveAndTransmit );
						tAudioCap.SetPackTime(30);
						ptCapSet->SetAudioCap( tAudioCap, byPayload );				
					}
				}
				ptCapSet->SetCapDesc(byCapIndex, nPos, nChoicePos++, byPayload);
			}
		}
		nPos++;
	}
    //��һ·��Ƶ(������Ƶͬ)
	nChoicePos = 0;
	byPayload  = PayloadTypeIn2Out(tSimuCap1.GetVideoMediaType());
    u8 byPayload2 = PayloadTypeIn2Out(tSimuCap2.GetVideoMediaType());
	if( (u8)t_none != byPayload )
	{		
        // guzh [04/15/2008]
        if ((u8)v_h264 == byPayload)
        {
            tH264Cap.Clear();
            tH264Cap.SetCapDirection( cmCapReceiveAndTransmit );
            tH264Cap.SetBitRate( tSimuCap1.GetVideoCap().GetMaxBitRate() );
            tH264Cap.SetResolution( ResIn2Out(tSimuCap1.GetVideoCap().GetResolution()),
                                    tSimuCap1.GetUserDefFrameRate() );

			//TBD ����Ӧ����Զ������ã�������ӦͬʱΪH264 [3/30/2012 chendaiwei]
            if (  (u8)v_h264 == byPayload2 &&
                ( tSimuCap1.GetVideoResolution() != tSimuCap2.GetVideoResolution() ||
				tSimuCap1.GetUserDefFrameRate() != tSimuCap2.GetUserDefFrameRate() )  )
            {
                tH264Cap.SetResolution( ResIn2Out(tSimuCap2.GetVideoCap().GetResolution()),
					tSimuCap2.GetUserDefFrameRate() );
				
				//zjj0111219 ����ʽ��������HP����
				tH264Cap.SetSupportProfile( (u8)TH264VideoCap::emProfileBaseline );				
            }

			if( tSimuCap1.GetVideoCap().IsSupportHP() )
			{
				tH264Cap.SetSupportProfile( (u8)TH264VideoCap::emProfileHigh);
				
				TH264VideoCap tH264BpCap;
				memcpy(&tH264BpCap,&tH264Cap,sizeof(TH264VideoCap));

				byFstH264SubId = ptCapSet->SetH264VideoCap(tH264Cap);
				ptCapSet->SetCapDesc(byCapIndex, nPos++, nChoicePos++, byPayload, byFstH264SubId);

				//BP HP�ֿ��������������⳧���ն� [3/30/2012 chendaiwei]
				u8 byH264BpSubId = 0;
				tH264BpCap.SetSupportProfile((u8)TH264VideoCap::emProfileBaseline);
				byH264BpSubId = ptCapSet->SetH264VideoCap(tH264BpCap);
				ptCapSet->SetCapDesc(byCapIndex, nPos-1, nChoicePos++, byPayload, byH264BpSubId);
			}
			else
			{
				tH264Cap.SetSupportProfile( (u8)TH264VideoCap::emProfileBaseline);
				byFstH264SubId = ptCapSet->SetH264VideoCap(tH264Cap);
				ptCapSet->SetCapDesc(byCapIndex, nPos++, nChoicePos++, byPayload, byFstH264SubId);
			}
        }
        else
        {
            tVideoCap.Clear();
            tVideoCap.SetCapDirection( cmCapReceiveAndTransmit );
            tVideoCap.SetBitRate( tSimuCap1.GetVideoCap().GetMaxBitRate() );
            tVideoCap.SetResolution( (emResolution)ResIn2Out(tSimuCap1.GetVideoCap().GetResolution()), 
                                     FrameRateIn2Out(tSimuCap1.GetVideoFrameRate()) );

            if (  byPayload == byPayload2 &&
                  tSimuCap1.GetVideoResolution() != tSimuCap2.GetVideoResolution() ) 
            {
                tVideoCap.SetResolution( (emResolution)ResIn2Out(tSimuCap2.GetVideoCap().GetResolution()), 
                                         FrameRateIn2Out(tSimuCap2.GetVideoFrameRate()) );
            }

            ptCapSet->SetVideoCap( tVideoCap, byPayload );
            ptCapSet->SetCapDesc(byCapIndex, nPos++, nChoicePos++, byPayload);
        }        
	}

    // ��һ·��Ƶ(����ʽ)
	if( (u8)t_none != byPayload2 && 
		byPayload != byPayload2 )
	{
        // guzh [04/15/2008]
        if ((u8)v_h264 == byPayload2)// xliang [12/31/2008] 
        {
            tH264Cap.Clear();
            tH264Cap.SetCapDirection( cmCapReceiveAndTransmit );
            tH264Cap.SetBitRate( tSimuCap2.GetVideoCap().GetMaxBitRate() );
            tH264Cap.SetResolution( ResIn2Out(tSimuCap2.GetVideoCap().GetResolution()),
                                    tSimuCap2.GetUserDefFrameRate() );
			
			//zjj20111227 ����ʽ��������HP����
			tH264Cap.SetSupportProfile( (u8)TH264VideoCap::emProfileBaseline );			

            byFstH264SubId = ptCapSet->SetH264VideoCap( tH264Cap );
            ptCapSet->SetCapDesc(byCapIndex, nPos-1, nChoicePos++, byPayload2, byFstH264SubId);
        }
        else
        {
            tVideoCap.Clear();
            tVideoCap.SetCapDirection( cmCapReceiveAndTransmit );
            tVideoCap.SetBitRate( tSimuCap2.GetVideoCap().GetMaxBitRate() );
            tVideoCap.SetResolution( (emResolution)ResIn2Out(tSimuCap2.GetVideoCap().GetResolution()), 
                                     FrameRateIn2Out(tSimuCap2.GetVideoFrameRate()) );

            ptCapSet->SetVideoCap( tVideoCap, byPayload2 );
            ptCapSet->SetCapDesc(byCapIndex, nPos-1, nChoicePos++, byPayload2);
        }

	}

	//����RADVISION-MCU�ȳ��̣�ͨ����Ҫ��(��һ·֧��H263���ҵڶ�·֧��H263+˫��ʱ), ͬʱҲ���ڵ�һ·ͬʱ֧��H263+�Ŀ�ѡ��
	//��dont send h263+ cap to netmeeting��
	if( MT_MANU_NETMEETING != byVendorId && 
		tCapSupport.IsSupportMediaType(MEDIA_TYPE_H263) && 
		MEDIA_TYPE_H263PLUS == tCapSupport.GetDStreamMediaType() &&
		!tCapSupport.IsDStreamSupportH239() )
	{	
		ptCapSet->SetCapDesc(byCapIndex, nPos-1, nChoicePos++, (u16)v_h263plus);
	}
    
	//���õڶ�·��Ƶ������Ŀ�Լ�������
    //˫��֧��, Ŀǰ֧���������ͣ�
    //������Ƶ��ʽһ�¡�H263����H239(H263/H263+/H264)��ȱʡֵ֧��������Ƶ��ʽһ��
    BOOL32 IsDSEvMain = FALSE;
    if ( tCapSupport.GetDStreamType() == VIDEO_DSTREAM_MAIN || tCapSupport.GetDStreamType() == VIDEO_DSTREAM_MAIN_H239 )
    {
        IsDSEvMain = TRUE;
    }

	if( MT_MANU_NETMEETING != byVendorId )
	{
		u8 byPayloadDV = PayloadTypeIn2Out(tCapSupport.GetDStreamMediaType());
		
		if( (u8)t_none != byPayloadDV )
		{
            // guzh [2008/04/15] H.264 ���������õ�Э��ջʱ��ÿ�ζ���һ���µ����á���������Ƶ����ͬһ��    
            if ( (u8)v_h264 == byPayloadDV )
            {
                tH264Cap.Clear();
                tH264Cap.SetCapDirection( cmCapReceiveAndTransmit );
                tH264Cap.SetBitRate( tCapSupport.GetDStreamMaxBitRate() );
                tH264Cap.SetResolution( ResIn2Out(tCapSupport.GetDStreamResolution()), 
                                        tCapSupport.GetDStreamCapSet().GetUserDefFrameRate() ); 

				s32 nTempPos = 0;
				TH264VideoCap tH264BpCap;
				tH264BpCap.Clear();
				memcpy(&tH264BpCap,&tH264Cap,sizeof(TH264VideoCap));

				if( tCapSupport.GetDStreamCapSet().IsSupportHP())
				{
					tH264Cap.SetSupportProfile( (u8)TH264VideoCap::emProfileHigh);
					bySecH264SubId = ptCapSet->SetH264VideoCap( tH264Cap );

					//[5/13/2011 zhushengze]������ǰͬ����Ƶ����239���������239�Ͳ���239��˫��������������              
					if( tCapSupport.IsDStreamSupportH239() )
					{
						ptCapSet->SetCapDesc(byCapIndex, nPos, nTempPos++, GetH239Type(byPayloadDV), bySecH264SubId);
						
						//ͬ����Ƶ���Ӳ���239˫��
						//ȡ����ͬ����Ƶ����H239�ļ���[4/13/2012 chendaiwei]
// 						if (IsDSEvMain)
// 						{
// 							ptCapSet->SetCapDesc(byCapIndex, nPos, nTempPos++, byPayloadDV, bySecH264SubId);
// 						}
					}
					else
					{
						ptCapSet->SetCapDesc(byCapIndex, nPos, nTempPos++, byPayloadDV, bySecH264SubId);
					}
				}

				//��ͬ����Ƶ,��֧��HP������Ӧ����BP·������ĿǰH264ͬ����Ƶֻ��8000H֧�֣� [3/30/2012 chendaiwei]
				if(  !tCapSupport.GetDStreamCapSet().IsSupportHP()
				    || (tCapSupport.GetDStreamCapSet().IsSupportHP() && !IsDSEvMain) )
				{
					tH264BpCap.SetSupportProfile( (u8)TH264VideoCap::emProfileBaseline);
					bySecH264SubId = ptCapSet->SetH264VideoCap( tH264BpCap );
					
					//[5/13/2011 zhushengze]������ǰͬ����Ƶ����239���������239�Ͳ���239��˫��������������              
					if( tCapSupport.IsDStreamSupportH239() )
					{
						ptCapSet->SetCapDesc(byCapIndex, nPos, nTempPos++, GetH239Type(byPayloadDV), bySecH264SubId);
						
						//ͬ����Ƶ���Ӳ���239˫��
						//ȡ����ͬ����Ƶ����H239�ļ���[4/13/2012 chendaiwei]
// 						if (IsDSEvMain)
// 						{
// 							ptCapSet->SetCapDesc(byCapIndex, nPos, nTempPos++, byPayloadDV, bySecH264SubId);
// 						}
					}
					else
					{
						ptCapSet->SetCapDesc(byCapIndex, nPos, nTempPos++, byPayloadDV, bySecH264SubId);
					}
				}

				//˫��uxGA60��ѡ1080/30��������֯1080����֪ͨ�Զ�[5/28/2013 chendaiwei]
				if( tDsStreamCapex.GetMediaType() == MEDIA_TYPE_H264 )
				{
					u8 bySecH264SubIdEx = 0;
					TH264VideoCap tH264DsCapEx;             //Э��ջH.264��Ƶ����
					tH264DsCapEx.Clear();
					tH264DsCapEx.SetCapDirection( cmCapReceiveAndTransmit );
					tH264DsCapEx.SetBitRate( tCapSupport.GetDStreamMaxBitRate() );
					tH264DsCapEx.SetResolution( ResIn2Out(tDsStreamCapex.GetResolution()), 
                                        tDsStreamCapex.GetUserDefFrameRate() ); 
					
					//��ͬ����Ƶ,��֧��HP����,����HP������ĿǰH264ͬ����Ƶֻ��8000H֧�֣� [3/30/2012 chendaiwei]
					if(tCapSupport.GetDStreamCapSet().IsSupportHP() && !IsDSEvMain)
					{
						tH264DsCapEx.SetSupportProfile( (u8)TH264VideoCap::emProfileHigh);
						bySecH264SubIdEx = ptCapSet->SetH264VideoCap( tH264DsCapEx );          
						if( tCapSupport.IsDStreamSupportH239() )
						{
							ptCapSet->SetCapDesc(byCapIndex, nPos, nTempPos++, GetH239Type(byPayloadDV), bySecH264SubIdEx);
						}
						else
						{
							ptCapSet->SetCapDesc(byCapIndex, nPos, nTempPos++, byPayloadDV, bySecH264SubIdEx);
						}
					}

					//ʼ������BP����[5/28/2013 chendaiwei]
					tH264DsCapEx.SetSupportProfile( (u8)TH264VideoCap::emProfileBaseline);
					bySecH264SubIdEx = ptCapSet->SetH264VideoCap( tH264DsCapEx );          
					if( tCapSupport.IsDStreamSupportH239() )
					{
						ptCapSet->SetCapDesc(byCapIndex, nPos, nTempPos++, GetH239Type(byPayloadDV), bySecH264SubIdEx);
					}
					else
					{
						ptCapSet->SetCapDesc(byCapIndex, nPos, nTempPos++, byPayloadDV, bySecH264SubIdEx);
					}
				}
				
				nPos++;
            }
            else 
            {
                if ( byPayload != byPayloadDV &&
                     byPayload2 != byPayloadDV )
                {
                    tVideoCap.Clear();
                    tVideoCap.SetCapDirection( cmCapReceiveAndTransmit );
                    tVideoCap.SetBitRate( tCapSupport.GetDStreamMaxBitRate() );
                    
                    tVideoCap.SetResolution( (emResolution)ResIn2Out(tCapSupport.GetDStreamResolution()), 
                                             FrameRateIn2Out(tCapSupport.GetDStreamFrameRate()) );
                    ptCapSet->SetVideoCap( tVideoCap, byPayloadDV );
                }

                //[5/13/2011 zhushengze]������ǰͬ����Ƶ����239���������239�Ͳ���239��˫��������������
                if( tCapSupport.IsDStreamSupportH239() )
                {
                    ptCapSet->SetCapDesc(byCapIndex, nPos, 0, GetH239Type(byPayloadDV));

					//ͬ����Ƶ���Ӳ���239˫��
					//ȡ����ͬ����Ƶ����H239�ļ���[4/13/2012 chendaiwei]
//                     if (IsDSEvMain)
//                     {
//                         ptCapSet->SetCapDesc(byCapIndex, nPos, 1, byPayloadDV, bySecH264SubId);
//                     }
                    nPos++;
                    
                }
                else
                {
                    ptCapSet->SetCapDesc(byCapIndex, nPos++, 0, byPayloadDV);
			    }

                //sony ��֧�� H263��H239 ͬʱ����Ӧ֧�� H263+��H239 ��ѡ
                if( MT_MANU_SONY == byVendorId && 
                    (u8)v_h263 == byPayloadDV && 
                    tCapSupport.IsDStreamSupportH239() )
                {
                    ptCapSet->SetCapDesc(byCapIndex, nPos-1, 1, GetH239Type( (u16)v_h263plus ));
                    
                    if( (u8)v_h263plus != byPayload && 
                        (u8)v_h263plus != byPayload2 )
                    {
                        tVideoCap.Clear();
                        tVideoCap.SetCapDirection( cmCapReceiveAndTransmit );
                        tVideoCap.SetBitRate( tCapSupport.GetDStreamMaxBitRate() );
                        
                        tVideoCap.SetResolution( (emResolution)ResIn2Out(tCapSupport.GetDStreamResolution()), 
                                                 FrameRateIn2Out(tCapSupport.GetDStreamFrameRate()) );
                        ptCapSet->SetVideoCap( tVideoCap, (u16)v_h263plus );
                    }
                    
                    //���� role==live(2)������Ӧsony��̬˫��
                    ptCapSet->SetSupportH239( 2 );
			    }
            }
		}
        
        //zbq[11/15/2008] �ڶ�˫����������
        tDStreamCap2.SetSupportH239(tCapSupport.IsDStreamSupportH239());
        tDStreamCap2.SetMediaType(tCapEx.GetSecDSType());
        tDStreamCap2.SetMaxBitRate(tCapEx.GetSecDSBitRate());
        tDStreamCap2.SetResolution(tCapEx.GetSecDSRes());
        
        if (MEDIA_TYPE_H264 == tCapEx.GetSecDSType())
        {
            tDStreamCap2.SetUserDefFrameRate(tCapEx.GetSecDSFrmRate());
        }
        else
        {
            tDStreamCap2.SetFrameRate(tCapEx.GetSecDSFrmRate());
        }

        byPayloadDV = PayloadTypeIn2Out(tDStreamCap2.GetMediaType());
        
        if( (u8)t_none != byPayloadDV )
        {
            // guzh [2008/04/15] H.264 ���������õ�Э��ջʱ��ÿ�ζ���һ���µ����á���������Ƶ����ͬһ��    
            if ( (u8)v_h264 == byPayloadDV )
            {
                tH264Cap.Clear();
                tH264Cap.SetCapDirection( cmCapReceiveAndTransmit );
                tH264Cap.SetBitRate( tDStreamCap2.GetMaxBitRate() );
                tH264Cap.SetResolution( ResIn2Out(tDStreamCap2.GetResolution()), 
                    tDStreamCap2.GetUserDefFrameRate() );

				////zjj20111227 �ڶ�˫����������HP����
				tH264Cap.SetSupportProfile( (u8)TH264VideoCap::emProfileBaseline );				

                bySecH264SubId = ptCapSet->SetH264VideoCap( tH264Cap );
                
                if( tDStreamCap2.IsSupportH239() )
                {
                    ptCapSet->SetCapDesc(byCapIndex, nPos++, 0, GetH239Type(byPayloadDV), bySecH264SubId);
                }
                else
                {
                    ptCapSet->SetCapDesc(byCapIndex, nPos++, 0, byPayloadDV, bySecH264SubId);
                }
            }
            else 
            {
                if ( byPayload != byPayloadDV &&
                    byPayload2 != byPayloadDV )
                {
                    tVideoCap.Clear();
                    tVideoCap.SetCapDirection( cmCapReceiveAndTransmit );
                    tVideoCap.SetBitRate( tDStreamCap2.GetMaxBitRate() );
                    
                    tVideoCap.SetResolution( (emResolution)ResIn2Out(tDStreamCap2.GetResolution()), 
                        FrameRateIn2Out(tDStreamCap2.GetFrameRate()) );
                    ptCapSet->SetVideoCap( tVideoCap, byPayloadDV );
                }
                
                if( tDStreamCap2.IsSupportH239()  )
                {
                    ptCapSet->SetCapDesc(byCapIndex, nPos++, 0, GetH239Type(byPayloadDV));
                }
                else
                {
                    ptCapSet->SetCapDesc(byCapIndex, nPos++, 0, byPayloadDV);
                }
            }
        }
	}
	
    if(tCapSupport.IsSupportH224())
    {
        ptCapSet->SetCapDesc(byCapIndex, nPos++, 0, (u16)d_h224);		
    }
    if(tCapSupport.IsSupportT120())
    {
        ptCapSet->SetCapDesc(byCapIndex, nPos++, 0, (u16)d_t120);		
    }
    if(tCapSupport.IsSupportMMcu())
    {
        ptCapSet->SetCapDesc(byCapIndex, nPos++, 0, (u16)t_mmcu);		
	}

	//��������Ƶ��������ģʽ�µ���Ӧ�����㷨
	u8 byEncryptType = (u8)emEncryptTypeNone;
	u8 byEncrypMode  = tCapSupport.GetEncryptMode();
	byEncryptType |= EncryptTypeIn2Out( byEncrypMode );
	ptCapSet->SetEncryptBits( byEncryptType );

	return;
}


/*=============================================================================
  �� �� ���� VideoCapCommon
  ��    �ܣ� ��ȡ��ͬ��Ƶ������Ĭ��common����local
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ��  
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2008/04/15    4.0R5       ����                H.264��ͬ�����ж�
=============================================================================*/
// void CMtAdpUtils::VideoCapCommon( const TSimCapSet &tLocalCap,  TCapSet *ptRemoteCapSet, 
//                                   TSimCapSet &tCommonCap, 
//                                   u8 byVendor, u8 byEncryptMode)
// {
//     u8 byPayload = PayloadTypeIn2Out(tLocalCap.GetVideoMediaType());
//     u16 wFStreamComType = GetComType(byPayload, byEncryptMode);
// 
//     u16 wBitrate = 0;       
//     if ( byPayload == v_h264 )
//     {
//         // Convert to tStackCap
//         TH264VideoCap tStackCap;
//         VideoCap264In2Out(tLocalCap.GetVideoCap(), tStackCap);
//         
//         if ( !ptRemoteCapSet->IsExistsimultaneous( &wFStreamComType, 1/*, &tStackCap*/) )
//         {
//             //FIXME: print ctrl
//             OspPrintf(TRUE, FALSE, "[Capset::Common] Remote H.264 IsExistsimultaneous failed, NOT Support H.264.\n");
//             tCommonCap.SetVideoMediaType(MEDIA_TYPE_NULL);
//             return;            
//         }        
// 
//         if ( !ptRemoteCapSet->IsExistsimultaneous( &wFStreamComType, 1, &tStackCap) )
//         {
// // [pengjie 2010/6/19] ����С��ȷ��ȡ����
// //          TH264VideoCap* ptRemotePriCap = ptRemoteCapSet->GetH264VideoCap(0);
// 
//             TVideoStreamCap tRemoteCap;
// 			MaxVideoCap264Out2In(ptRemoteCapSet, tLocalCap.GetVideoCap(), 
// 				wFStreamComType, tRemoteCap);
// //          VideoCap264Out2In(*ptRemotePriCap, tLocalCap.GetVideoCap(), tRemoteCap);
//             tCommonCap.SetVideoCap(tRemoteCap);
//         }
// 
// 
//         //guzh [02/21/2008] ����ĸ�������ն˶�ͨ����(����KedaMT����, KedaMCU���������̾�������)
//         //guzh 2008/08/07 BAS HD �ſ�����
//         /*
//         if ( byVendor == MT_MANU_KDC )
//         {
//             // �ο� mtadp.cpp GetCapVideoParam()
//             // 3. CIF �Ļ��飬��Ϊ������ն�û��H.264 CIF����
//             u8 byVcRes = tLocalCap.GetVideoResolution();
//             if ( byVcRes == VIDEO_FORMAT_AUTO )
//             {
//                 byVcRes = VIDEO_FORMAT_CIF;
//             }
//             if (byVcRes == VIDEO_FORMAT_CIF &&
//                 tStackCap.IsSupport(emHD720, 25))
//             {
//                 //FIXME: print ctrl
//                 OspPrintf(TRUE, FALSE, "[Capset::Common] CIF Conf but MT is HD-cap, NOT Support.\n");
//                 tCommonCap.SetVideoMediaType(MEDIA_TYPE_NULL);
//                 return;
//             }
//         }
//         */
//         
//         // guzh [2/27/2007] �����ʽ��б���
//         if (tStackCap.GetBitRate() != 0)
//         {
//             if (tLocalCap.GetVideoMaxBitRate() != 0)
//             {
//                 wBitrate = min(tStackCap.GetBitRate(), tLocalCap.GetVideoMaxBitRate());
//             }
//             else
//             {
//                 wBitrate = tStackCap.GetBitRate();
//             }
//         }
//     }
//     else
//     {            
//         if( !ptRemoteCapSet->IsExistsimultaneous( &wFStreamComType, 1) )
//         {
//             tCommonCap.SetVideoMediaType(MEDIA_TYPE_NULL);
//             return;
//         }        
// 
//         TVideoCap* ptVideoCap = NULL;
//         ptVideoCap = ptRemoteCapSet->GetVideoCap(byPayload);           
//         if( NULL == ptVideoCap )
//         {
//             tCommonCap.SetVideoMediaType(MEDIA_TYPE_NULL);
//             return;
//         }
//         u8 byVcRes = tLocalCap.GetVideoCap().GetResolution();
//         u8 byMPI = ptVideoCap->GetResolution( (emResolution)ResIn2Out(byVcRes) );
//         if( 0 == byMPI )
//         {
//             tCommonCap.SetVideoMediaType(MEDIA_TYPE_NULL);
//             return;
//         }
//         
//         // guzh [2/27/2007] �����ʽ��б���
//         if (ptVideoCap->GetBitRate() != 0)
//         {
//             if (tLocalCap.GetVideoMaxBitRate() != 0)
//             {
//                 wBitrate = min(ptVideoCap->GetBitRate(), tLocalCap.GetVideoMaxBitRate());
//             }
//             else
//             {
//                 wBitrate = ptVideoCap->GetBitRate();
//             }
//         }
//     }										
//     
//     if ( wBitrate != 0 )
//     {
//         tCommonCap.SetVideoMaxBitRate(wBitrate);
//     }
// 
//     return;
// }


/*=============================================================================
  �� �� ���� VideoDCapCommon
  ��    �ܣ� �ж��Ƿ���ڹ�ͬ˫������(first video+dual video)
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2008/04/15    4.0R5       ����                H.264��ͬ�����ж�
=============================================================================*/
// BOOL CMtAdpUtils::VideoDCapCommon(const TDStreamCap tDStreamCap,
//                                   BOOL bH339, u8 byEncryptMode,
//                                   const TVideoStreamCap tFstVideoCap,
//                                   TCapSet *ptRemoteCapSet)
// {
//     u16    awDStreamComType[2] = {0}; 
//     TH264VideoCap atH264Cap[2];        
//     u8 byArraySubId = 0;
//     BOOL bSupport  = FALSE;
//     
//     u8 byPayloadD = PayloadTypeIn2Out(tDStreamCap.GetMediaType());
//     awDStreamComType[1] = GetComType(byPayloadD, byEncryptMode);
//     if(bH339)
//     {
//         awDStreamComType[1] = GetH239Type(awDStreamComType[1]);
//     }
//     
//     u8 byPayload = PayloadTypeIn2Out(tFstVideoCap.GetMediaType());
//     if( t_none != byPayload )
//     {
//         awDStreamComType[0] = GetComType(byPayload, byEncryptMode);
//         
//         //zbq[12/27/2008] ����ͬʱ�������� ��ȡ˫������
// 
//         //[1]. h264��˫����h264��ͬʱ�������ݣ���h264�Ĺ�ͬ����ģʽ
//         if (v_h264 == byPayloadD)
//         {
//             // ������ TH264VideoCap
//             if (v_h264 == byPayload)
//             {
//                 VideoCap264In2Out(tFstVideoCap, atH264Cap[byArraySubId]);
//                 byArraySubId++;
//             }
//             
//             if (v_h264 == byPayloadD)
//             {
//                 VideoCap264In2Out(tDStreamCap, atH264Cap[byArraySubId]);
//                 byArraySubId++;
//             }
//             
//             bSupport = ptRemoteCapSet->IsExistsimultaneous(awDStreamComType, 2, atH264Cap);
//         }
//         //[2].����˫������ͨ�Ĺ�ͬ��ȡģʽ
//         else
//         {
//             bSupport  = ptRemoteCapSet->IsExistsimultaneous(awDStreamComType, 2);
//         }
//     }
//     else  // �����һ·�޹�ͬ������ֱ���ж�˫����ͬ������, zgc, 2008-06-27
//     {
//         if (v_h264 == byPayloadD)
//         {
//             VideoCap264In2Out(tDStreamCap, atH264Cap[byArraySubId]);
//             byArraySubId++;
//         }
// 
//         u16 wDStreamComType = awDStreamComType[1];
//         bSupport = ptRemoteCapSet->IsExistsimultaneous(&wDStreamComType, 1, atH264Cap);
//     }
//     
//     return bSupport;
// }

/*=============================================================================
  �� �� ���� CapSupportCommon
  ��    �ܣ� ��ȡ��ͬ�������ṹ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TCapSupport &tLocalCap       [IN] ����������
             TCapSet     *ptRemoteCapSet  [IN] Զ��������
             TCapSupport &tCommonCap      [OUT]��ͬ������ 
			 const TVideoStreamCap *ptMSCapEx   [IN]������չ��������ѡ����
			 u8 byMSCapExNum			  [IN]������չ��������ѡ��Ŀ
			 cosnt TVideoStreamCap *ptDSCapEx   [IN]˫����չ��������ѡ����
			 u8 byDSCapEXNum			  [IN]˫����չ��������ѡ��Ŀ

  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2004/12/30                xsl                   ����(���Ǽ���ģʽ��ͬʱ�������ж�)
  2005/11/11    4.0         wancl                 �����Ż�
  2005/12/27	4.0			�ű���				  ˫�����ӷֱ��ʵ��ж�
  2008/04/15    4.0R5       ����                H.264��ͬ�����ж�
  2008/11/15    4.0R5       �ű���                ���ڶ�˫������ȡ��ͬ�������� ֧��
  2011/12/08    4.0R7       �´�ΰ                ������չ��������ѡ֧��
=============================================================================*/
void CMtAdpUtils::CapSupportCommon(const TCapSupport &tLocalCap, 
                                   const TCapSupportEx &tCapEx,
								   TCapSet     *ptRemoteCapSet, 
								   TMultiCapSupport &tCommonCap,
								   const TVideoStreamCap *ptMSCapEx,
								   u8 byMSCapExNum,
								   const TVideoStreamCap *ptDSCapEx,
								   u8 byDSCapEXNum,
								   TAudioTypeDesc *ptLocalAudioTypeDesc,
								   u8 byAudioCapNum
								   )
{
//	TSimCapSet tSimuCap1, tSimuCap2, tSimuCapOut;
	u16    wFStreamComType;       //��һ·
	BOOL32 bSupport= FALSE;
	u8     byEncryptMode = EncryptTypeIn2Out(tLocalCap.GetEncryptMode());

	wFStreamComType = (u16)t_mmcu;
	bSupport = (tLocalCap.IsSupportMMcu() && ptRemoteCapSet->IsExistsimultaneous(&wFStreamComType,1));
	tCommonCap.SetSupportMMcu(bSupport);

	wFStreamComType = GetComType((u8)d_t120, byEncryptMode);
	bSupport = (tLocalCap.IsSupportT120() && ptRemoteCapSet->IsExistsimultaneous(&wFStreamComType,1));
	tCommonCap.SetSupportT120(bSupport);
	
	wFStreamComType = GetComType((u8)d_h224, byEncryptMode);
	bSupport = (tLocalCap.IsSupportH224() && ptRemoteCapSet->IsExistsimultaneous(&wFStreamComType,1));
	tCommonCap.SetSupportH224(bSupport);
	
//	tSimuCap1 = tLocalCap.GetMainSimCapSet();
//	tSimuCap2 = tLocalCap.GetSecondSimCapSet();

    BOOL32 bMatchSuccess = FALSE;
    bMatchSuccess = MatchCommonCap(tLocalCap,
                                        tCapEx,
                                        ptRemoteCapSet,
                                        tCommonCap,
                                        /*byVendor,*/
                                        byEncryptMode,
										ptMSCapEx,byMSCapExNum,ptDSCapEx,byDSCapEXNum,
										ptLocalAudioTypeDesc,byAudioCapNum);
    if (!bMatchSuccess)
    {
        StaticLog( "[CapSupportCommon]match common cap failed!\n");
    }
	
// 	//��ȡ��һ·����Ƶ����������
//     tSimuCapOut = tSimuCap1;
//     VideoCapCommon(tSimuCap1, ptRemoteCapSet, tSimuCapOut, byVendor, byEncryptMode);
// 	
// 	// ����Ƶ
// 	wFStreamComType = GetComType(PayloadTypeIn2Out(tSimuCap1.GetAudioMediaType()), byEncryptMode);
// 	if( !ptRemoteCapSet->IsExistsimultaneous( &wFStreamComType, 1) )
// 	{
// 		tSimuCapOut.SetAudioMediaType(MEDIA_TYPE_NULL);
// 	}
// 
// 	tCommonCap.SetMainSimCapSet(tSimuCapOut);
// 	    
// 	if( !tSimuCap2.IsNull() )
// 	{
//         tSimuCapOut = tSimuCap2;
//         VideoCapCommon(tSimuCap2, ptRemoteCapSet, tSimuCapOut, byVendor, byEncryptMode);
// 
//         // ����Ƶ
// 		wFStreamComType = GetComType(PayloadTypeIn2Out(tSimuCap2.GetAudioMediaType()), byEncryptMode);
// 		if( !ptRemoteCapSet->IsExistsimultaneous( &wFStreamComType, 1) )
// 		{
// 			tSimuCapOut.SetAudioMediaType(MEDIA_TYPE_NULL);
// 		}
// 
// 		tCommonCap.SetSecondSimCapSet(tSimuCapOut);
// 	}
// 
// 	//��ȡ�ڶ�·��Ƶ����������
// 
//     BOOL32 bSecDSCap = FALSE;
// 
// 	if( MEDIA_TYPE_NULL != tLocalCap.GetDStreamMediaType() )
// 	{
//         BOOL bH239 = tLocalCap.IsDStreamSupportH239();
// 
//         //[1].��ȡ��һ·����Ƶ�����Ľ�����Ϊ ˫�� ͬʱ����������
//         bSupport = VideoDCapCommon(tLocalCap.GetDStreamCapSet(),
//                                    bH239, byEncryptMode,
//                                    tCommonCap.GetMainSimCapSet().GetVideoCap(),
//                                    ptRemoteCapSet);
// 
// 		
// 		//[2].��֧�֣�����ȡ��һ·����Ƶ�����Ľ�����Ϊ ˫�� ͬʱ����������
// 		if( !bSupport && MEDIA_TYPE_NULL != tCommonCap.GetSecondSimCapSet().GetVideoMediaType() )
// 		{
//             bSupport = VideoDCapCommon(tLocalCap.GetDStreamCapSet(),
//                                        bH239, byEncryptMode,
//                                        tCommonCap.GetSecondSimCapSet().GetVideoCap(),
//                                        ptRemoteCapSet);
// 		}
// 
//         //zbq[11/15/2008] �ٲ�֧�֣����԰� �ڶ�˫������ ȡ��ͬ����
//         if (!bSupport &&
//             MEDIA_TYPE_NULL != tCapEx.GetSecDSType())
//         {
//             TDStreamCap tDStreamCap2;
//             tDStreamCap2.SetSupportH239(bH239);
//             tDStreamCap2.SetMediaType(tCapEx.GetSecDSType());
//             tDStreamCap2.SetMaxBitRate(tCapEx.GetSecDSBitRate());
//             tDStreamCap2.SetResolution(tCapEx.GetSecDSRes());
//             
//             if (MEDIA_TYPE_H264 == tCapEx.GetSecDSType())
//             {
//                 tDStreamCap2.SetUserDefFrameRate(tCapEx.GetSecDSFrmRate());
//             }
//             else
//             {
//                 tDStreamCap2.SetFrameRate(tCapEx.GetSecDSFrmRate());
//             }
// 
//             //[3].��Ȼ��ȡ��һ·����Ƶ�����Ľ�����Ϊ �ڶ�˫�� ͬʱ����������
//             bSupport = VideoDCapCommon(tDStreamCap2,
//                                        bH239, byEncryptMode,
//                                        tCommonCap.GetMainSimCapSet().GetVideoCap(),
//                                        ptRemoteCapSet);
//             
//             
//             //[4].����֧�֣�����ȡ��һ·����Ƶ�����Ľ�����Ϊ �ڶ�˫�� ͬʱ����������
//             if( !bSupport && MEDIA_TYPE_NULL != tCommonCap.GetSecondSimCapSet().GetVideoMediaType() )
//             {
//                 bSupport = VideoDCapCommon(tDStreamCap2,
//                                            bH239, byEncryptMode,
//                                            tCommonCap.GetSecondSimCapSet().GetVideoCap(),
//                                            ptRemoteCapSet);
// 		    }
// 
//             if (bSupport)
//             {
//                 bSecDSCap = TRUE;
//             }
//         }
// 
// 		if( bSupport )
// 		{
//             if (!bSecDSCap)
//             {
//                 tCommonCap.SetDStreamMediaType(tLocalCap.GetDStreamMediaType());
//                 tCommonCap.SetDStreamSupportH239(bH239);
//                 tCommonCap.SetDStreamMaxBitRate(tLocalCap.GetDStreamMaxBitRate());
//                 tCommonCap.SetDStreamResolution(tLocalCap.GetDStreamResolution());
//                 if (tLocalCap.GetDStreamMediaType() == MEDIA_TYPE_H264)
//                 {
//                     tCommonCap.SetDStreamUsrDefFPS(tLocalCap.GetDStreamUsrDefFPS());
//                 }
//                 else
//                 {
//                     tCommonCap.SetDStreamFrameRate(tLocalCap.GetDStreamFrameRate());
// 			    }	
//             }
//             else
//             {
//                 tCommonCap.SetDStreamMediaType(tCapEx.GetSecDSType());
//                 tCommonCap.SetDStreamSupportH239(bH239);
//                 tCommonCap.SetDStreamMaxBitRate(tCapEx.GetSecDSBitRate());
//                 tCommonCap.SetDStreamResolution(tCapEx.GetSecDSRes());
//                 tCommonCap.SetDStreamFrameRate(tCapEx.GetSecDSFrmRate());
//             }
// 		}
//         //[5].˫�����书�ܼ��룬��H264�����������Ʒſ�, zgc, 2008-08-25
//         else if ( ptRemoteCapSet->GetH264Num() > 0 &&
//                   tLocalCap.GetDStreamMediaType() == MEDIA_TYPE_H264 )
//         {
//             tCommonCap.SetDStreamMediaType(MEDIA_TYPE_H264);
//             tCommonCap.SetDStreamSupportH239(bH239);
//             tCommonCap.SetDStreamMaxBitRate(tLocalCap.GetDStreamMaxBitRate());
// 
// 			//zbq[09/02/2008] ֡�������ԣ��ֱ���ȡ�Զ�
// 			u8 byDSFPS = 0;
// 			u8 byDSRes = 0;
// /*			u8 byH264Num = ptRemoteCapSet->GetH264Num();
// 			for(u8 bySubId = 0; bySubId < byH264Num; bySubId ++)
// 			{
//                 byDSRes = 0;
// 
// 				TVideoStreamCap tCommonDSCap;
// 				
// 				TDStreamCap tLocalDSCap = tLocalCap.GetDStreamCapSet();
// 				TVideoStreamCap tLocalDSVidCap = (TVideoStreamCap)tLocalDSCap;
// 				TH264VideoCap *ptDSVidCap = ptRemoteCapSet->GetH264VideoCap(bySubId);
// 
//                 //�Ƿ�ͬ����Ƶ
//                 BOOL32 bDSEqMV = FALSE;
// 
//                 if (tLocalCap.GetMainVideoType() == tLocalCap.GetDStreamMediaType() &&
//                     tLocalCap.GetMainVideoResolution() == tLocalCap.GetDStreamResolution())
//                 {
//                     bDSEqMV = TRUE;
//                 }
// 
//                 //Presentation/Live��ȡ��ǰLocal������
//                 if (MEDIA_TYPE_H264 == tLocalDSCap.GetMediaType())
//                 {
//                     byDSFPS = tLocalDSCap.GetUserDefFrameRate();
//                 }
//                 else
//                 {
//                     byDSFPS = tLocalDSCap.GetFrameRate();
//                 }
// 
// 				VideoCap264Out2In(*ptDSVidCap,
// 							       tLocalDSVidCap,
// 								   tCommonDSCap, TRUE, byDSFPS, bDSEqMV);
//                 
//                 byDSRes = tCommonDSCap.GetResolution();
//                 
//                 if (byDSRes > tLocalCap.GetDStreamResolution())
//                 {
//                     //�Զ˺�����common��ֱ��ȡ����������
//                 }
//                 else
//                 {
//                     //�Զ˺����Ƚ�С�������´�����5fpsȡ�ֱ���
//                     byDSFPS = 5;
// 
//                     VideoCap264Out2In(*ptDSVidCap,
//                                        tLocalDSVidCap,
//                                        tCommonDSCap, TRUE, byDSFPS, bDSEqMV);
//                     byDSRes = tCommonDSCap.GetResolution();
//                 }
//                 //byDSRes = byDSRes > tCommonDSCap.GetResolution() ? byDSRes : tCommonDSCap.GetResolution();
// 
// 				//���⴦���ܿ�SVGA������3֡��XGA
// 				if (VIDEO_FORMAT_SVGA == byDSRes)
// 				{
// 					if (ptDSVidCap->IsSupport(em1024768, 3))
// 					{
// 						byDSRes = ResOut2In(em1024768);
// 						byDSFPS = 3;
// 					}
// 				}
// 			}
// */
// 			//[2010.12.10 zhushz] ȡ�����˫������				
// 			BOOL32 bDSEqMV = FALSE;
// 
// 			//�ж��Ƿ�ͬ����Ƶ
//             if (tLocalCap.GetMainVideoType() == tLocalCap.GetDStreamMediaType() &&
//                 tLocalCap.GetMainVideoResolution() == tLocalCap.GetDStreamResolution())
//             {
//                 bDSEqMV = TRUE;
//             }
// 				
// 
// 			TDStreamCap tLocalDSCap = tLocalCap.GetDStreamCapSet();			
// 			TVideoStreamCap tMainStreamCap = tCommonCap.GetMainSimCapSet().GetVideoCap();
// 			TVideoStreamCap  tCommonDSCap;
// 			//ȡ�����˫������
// 			MaxDSCap264Out2In(ptRemoteCapSet, tLocalDSCap, tMainStreamCap, byEncryptMode, tCommonDSCap, bDSEqMV);
// 			byDSFPS=tCommonDSCap.GetUserDefFrameRate();
// 			byDSRes=tCommonDSCap.GetResolution();
// 			//end
// 			tCommonCap.SetDStreamResolution(byDSRes);
// 			tCommonCap.SetDStreamUsrDefFPS(byDSFPS);
//         }
// 		else
// 		{
// 			//zjj 20090827 �Ƿ�֧��̩ɽ���ն���˫������ʱ���׼���˫������
// 			if( MT_MANU_TAIDE == byVendor && g_cMtAdpApp.m_bSupportTaideDsCap )
// 			{
// 				if (!bSecDSCap)
// 				{
// 					tCommonCap.SetDStreamMediaType(tLocalCap.GetDStreamMediaType());
// 					tCommonCap.SetDStreamSupportH239(bH239);
// 					tCommonCap.SetDStreamMaxBitRate(tLocalCap.GetDStreamMaxBitRate());
// 					tCommonCap.SetDStreamResolution(tLocalCap.GetDStreamResolution());
// 					if (tLocalCap.GetDStreamMediaType() == MEDIA_TYPE_H264)
// 					{
// 						tCommonCap.SetDStreamUsrDefFPS(tLocalCap.GetDStreamUsrDefFPS());
// 					}
// 					else
// 					{
// 						tCommonCap.SetDStreamFrameRate(tLocalCap.GetDStreamFrameRate());
// 					}	
// 				}
// 				else
// 				{
// 					tCommonCap.SetDStreamMediaType(tCapEx.GetSecDSType());
// 					tCommonCap.SetDStreamSupportH239(bH239);
// 					tCommonCap.SetDStreamMaxBitRate(tCapEx.GetSecDSBitRate());
// 					tCommonCap.SetDStreamResolution(tCapEx.GetSecDSRes());
// 					tCommonCap.SetDStreamFrameRate(tCapEx.GetSecDSFrmRate());
// 				}
// 			}
// 		}
// 	}

	//��ȡ��������ģʽ
	tCommonCap.SetEncryptMode(tLocalCap.GetEncryptMode());
	
	return;
}

/*=============================================================================
�� �� ���� CapabilityExSetIn2Out
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  TCapSupportEx &tCapSupportEx
           TCapSet *ptCapSet
�� �� ֵ�� 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/9/27   4.0			�ܹ��                  ����
=============================================================================*/
void CMtAdpUtils::CapabilityExSetIn2Out( TCapSupportEx &tCapSupportEx, TCapSet *ptCapSet )
{
	// FECType
	emFECType emType;
	CMtAdpUtils::FECTypeIn2Out( tCapSupportEx.GetVideoFECType(), emType );
	ptCapSet->SetFECType( (u8)emType );

	return;
}


/*=============================================================================
�� �� ����  FECTypeIn2Out
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  u8 byFECType
			emFECType &FECType
�� �� ֵ�� static void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/9/30   4.0			�ܹ��                  ����
=============================================================================*/
void  CMtAdpUtils::FECTypeIn2Out( u8 byFECType, emFECType &FECType )
{
	switch( byFECType )
	{
	case FECTYPE_NONE:
		FECType = emFECNone ;
		break;
	case FECTYPE_RAID5:
		FECType = emFECRaid5 ;
		break;
	case FECTYPE_RAID6:
		FECType = emFECRaid6 ;
		break;
	default:
		StaticLog( "[CapabilityExSetIn2Out] FECType.%d error!\n", byFECType );
		FECType = emFECNone ;
		break;
	}
}

/*=============================================================================
�� �� ���� FECTypeOut2In
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  emFECType FECType
           u8 &byFECType
�� �� ֵ�� void  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/9/30   4.0			�ܹ��                  ����
=============================================================================*/
void  CMtAdpUtils::FECTypeOut2In( emFECType FECType, u8 &byFECType )
{
	switch( FECType )
	{
	case emFECNone:
		byFECType = FECTYPE_NONE;
		break;
	case emFECRaid5:
		byFECType = FECTYPE_RAID5;
		break;
	case emFECRaid6:
		byFECType = FECTYPE_RAID6;
		break;
	default:
		StaticLog( "[FECTypeOut2In] FECType.%d error!\n", FECType );
		byFECType = FECTYPE_NONE;
		break;
	}
}

/*=============================================================================
�� �� ���� FECTypeCommon
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����  emFECType LocalFECType
           u8 byRemoteFECType
           emFECType &CommonFECType
�� �� ֵ�� void  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/10/8   4.0			�ܹ��                  ����
=============================================================================*/
void  CMtAdpUtils::FECTypeCommon( emFECType LocalFECType, u8 byRemoteFECType, emFECType &CommonFECType )
{
	CommonFECType = (emFECType)( byRemoteFECType & (u8)LocalFECType );
	return;
}

/*=============================================================================
�� �� ���� VideoCap264In2Out
��    �ܣ� H.264 Э��ջ�����ṹ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����
�� �� ֵ�� void  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/04/15  4.0R5	    ����                  ����
=============================================================================*/
void CMtAdpUtils::VideoCap264In2Out(const TVideoStreamCap tInCap, TH264VideoCap &tOutCap)
{
    if ( tInCap.GetMediaType() != MEDIA_TYPE_H264 )
    {
        return;
    }

    u8 byVcRes = tInCap.GetResolution();
    if ( byVcRes == VIDEO_FORMAT_AUTO )
    {
        byVcRes = VIDEO_FORMAT_CIF;
    }
    s32 nStackRes = CMtAdpUtils::ResIn2Out(byVcRes);
    u8 byFps = tInCap.GetUserDefFrameRate();
    tOutCap.SetResolution(nStackRes, byFps);

    if (tInCap.GetMaxBitRate() > 0)
    {
        tOutCap.SetBitRate(tInCap.GetMaxBitRate());
    }

	// ����֧��HP [12/8/2011 chendaiwei]
	if(tInCap.IsSupportHP())
	{
		tOutCap.SetSupportProfile((u8)TH264VideoCap::emProfileHigh);
	}
	else
	{
		tOutCap.SetSupportProfile( (u8)TH264VideoCap::emProfileBaseline);	
	}
}

/*=============================================================================
�� �� ���� MaxVideoCap264Out2In
��    �ܣ� H.264 Э��ջ�����ṹ����(ȡ���������ʵķ�239��264����)
�㷨ʵ�֣� �����Զ�����264��������ȡ��������ʵķ�239��264����
ȫ�ֱ����� 
��    ���� TCapSet *ptRemoteCapSet �Զ�����
           TSimCapSet &tLocalCap ��������
		   u16 wFStreamComType ý���غ�
		   TVideoStreamCap  &tMaxRemoteCap ��ͬ����
�� �� ֵ�� void  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
20100519    4.6         ���                  create
20100902    4.6         ���                  �޸ģ�ȡ����Ϊ�������ʵķ�239��264������
=============================================================================*/
void CMtAdpUtils::MaxVideoCap264Out2In(TCapSet *ptRemoteCapSet, 
									   const TVideoStreamCap &tLocalVideoCap,
									   u16 wFStreamComType,
									   TVideoStreamCap  &tMaxRemoteCap)
{
	u8 abyRes[] = { VIDEO_FORMAT_HD1080 , VIDEO_FORMAT_HD720 , VIDEO_FORMAT_4CIF , VIDEO_FORMAT_CIF };
	u8 byMaxResNum = sizeof(abyRes)/sizeof(u8);
	
	u8 byResIdx;
	TVideoStreamCap tLocalVideoStreamCap = tLocalVideoCap;
	for ( byResIdx = 0; byResIdx < byMaxResNum; byResIdx++ )
	{
		// �ҵ����������ֱ���������λ��
		if( tLocalVideoStreamCap.GetResolution() == abyRes[byResIdx] )
		{
			break;
		}
	}
	
	// �ӵ�ǰ�ֱ��������±ȣ�ȡ�����֧�ֵķֱ���
	for ( u8 byCurResIdx = byResIdx; byCurResIdx < byMaxResNum; byCurResIdx++ )
	{
		if( abyRes[byCurResIdx] <= VIDEO_FORMAT_4CIF && tLocalVideoStreamCap.GetUserDefFrameRate() > 30 )
		{
			tLocalVideoStreamCap.SetUserDefFrameRate(30);
		}
		tLocalVideoStreamCap.SetResolution( abyRes[byCurResIdx] );
		TH264VideoCap tStackCap;
		tStackCap.Clear();
		VideoCap264In2Out(tLocalVideoStreamCap, tStackCap);
        
		if ( ptRemoteCapSet->IsExistsimultaneous( &wFStreamComType, 1, &tStackCap) )
		{
			// ȡ�����ͬ����
			tMaxRemoteCap = tLocalVideoStreamCap;
			break;
		}
		else // û��ȡ��
		{
			// [pengjie 2010/9/8] ����Ŀǰ��û��֡�����䣬�������潵֡�ʱȽϲ����Ȳ�����
			// 			tStackCap.SetMaxMBPS( tStackCap.GetMaxFS() );
			// 			if ( ptRemoteCapSet->IsExistsimultaneous( &wFStreamComType, 1, &tStackCap ) )
			// 			{
			// 				u8 byFps = tStackCap.GetResolution( CMtAdpUtils::ResIn2Out(tLocalVideoStreamCap.GetResolution()) );
			// 				tLocalVideoStreamCap.SetUserDefFrameRate(byFps);
			// 				tMaxRemoteCap = tLocalVideoStreamCap;
			// 				break;
            // 			}
			
			// ����cif��֡��ģ�����������ǰ������ƥ�䣬��֡�����ڴ���25С�ڵ���30֮�䣬�򽫱���֡�ʽ�Ϊ25��ƥ��һ��
			if( VIDEO_FORMAT_CIF == abyRes[byCurResIdx] )
			{
				u8 byLocalFps = tLocalVideoStreamCap.GetUserDefFrameRate();
				if( byLocalFps > 25 && byLocalFps <= 30 )
				{
					tLocalVideoStreamCap.SetUserDefFrameRate( 25 );
					tStackCap.Clear();
					VideoCap264In2Out(tLocalVideoStreamCap, tStackCap);
					
					if ( ptRemoteCapSet->IsExistsimultaneous( &wFStreamComType, 1, &tStackCap ) )
					{
						tMaxRemoteCap = tLocalVideoStreamCap;
						break;
					}
				}
			}
		}
	}
	
	return;
}
/*=============================================================================
�� �� ���� MatchCommonCap
��    �ܣ� ƥ�乲ͬ������
�㷨ʵ�֣�1.��һ·����ʽ
          2.��һ·����ʽ
          3.˫��
            3.1����ʽ+˫��(����ʽ����)
            3.2����ʽ+�ڶ�˫��(����ʽ����)
            3.3����ʽ+˫��
            3.4����ʽ+�ڶ�˫��
ȫ�ֱ����� 
��    ���� const TCapSupport &tLocalCap [IN]����������
           const TCapSupportEx &tCapEx  [IN]���صڶ�˫��
           TCapSet *ptRemoteCapSet      [IN]�Զ�������
           TCapSupport &tCommonCap      [OUT]��ͬ������    
           u8 byEncryptMode             [IN]����ģʽ
           u8 byAdpFps                  [IN]����֡��
		   const TVideoStreamCap *ptMSCapEx   [IN]������չ��������ѡ����
		   u8 byMSCapExNum					  [IN]������չ��������ѡ��Ŀ
		   cosnt TVideoStreamCap *ptDSCapEx   [IN]˫����չ��������ѡ����
		   u8 byDSCapEXNum					  [IN]˫����չ��������ѡ��Ŀ

�� �� ֵ�� BOOL32:��һ·�Ƿ�ƥ��ɹ�  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
20110408    4.6         ��ʤ��                 create
=============================================================================*/
BOOL32 CMtAdpUtils::MatchCommonCap( const TCapSupport &tLocalCap,
                                         const TCapSupportEx &tCapEx,
								         TCapSet *ptRemoteCapSet,
									     TMultiCapSupport &tCommonCap,
									     u8 byEncryptMode,
										 const TVideoStreamCap *ptMSCapEx,
										 u8 byMSCapExNum,			 
										 const TVideoStreamCap *ptDSCapEx,
										 u8 byDSCapEXNum,
										 const TAudioTypeDesc *ptLocalAudioTypeDesc,
										 u8 byAudioCapNum
										 )
{
		
    TSimCapSet tSimuCap1 = tLocalCap.GetMainSimCapSet();
	TSimCapSet tSimuCap2 = tLocalCap.GetSecondSimCapSet();
    TDStreamCap tSimuDSCap = tLocalCap.GetDStreamCapSet();

	//[1].ȡ��һ·����ʽ��ͬ����
    BOOL32 bMainMatchSuccess = FALSE;
	TSimCapSet tMainSimCapSet = tSimuCap1; 
	TSimCapSet tMainExtraSimCapSet;


	//��Ƶ������ƥ�����
	BOOL32 bAudioMatchSuccess = FALSE;
	bAudioMatchSuccess = MatchAudioCap(byEncryptMode,byAudioCapNum,
								ptLocalAudioTypeDesc,ptRemoteCapSet,tCommonCap);
	if (bAudioMatchSuccess)
    {
		TAudioStreamCap tAudioStreamCap;
		tAudioStreamCap.SetMediaType( tCommonCap.GetMainAudioType() );
		tCommonCap.SetExtraAudioCap( tAudioStreamCap,0 ); 
        StaticLog( "[MatchCommonCap]MatchAudioCap (%d,%d) successfully!\n",
			tCommonCap.GetMainAudioType(),tCommonCap.GetMainAudioTrackNum());
    }
	else
	{
		StaticLog( "[MatchCommonCap]MatchAudioCap failed!\n");
	}
	tMainSimCapSet.SetAudioMediaType(tCommonCap.GetMainAudioType());

	//��һ·����ʽ��Ƶ��ͬ������
    u16 wMainVideoComType = GetComType(	PayloadTypeIn2Out(tMainSimCapSet.GetVideoMediaType()),
										byEncryptMode);
	if (ptRemoteCapSet->IsExistsimultaneous(&wMainVideoComType, 1))
	{//Զ���б���������Ƶ�������͵���Ƶ������ȡ��ͬ����
	
        //��һ·����ʽ������ƥ��
		bMainMatchSuccess = MatchVideoCap( tSimuCap1,
										   tSimuDSCap,
									       ptRemoteCapSet,
									       tMainSimCapSet,
									       byEncryptMode,
                                           tMainExtraSimCapSet,
										   ptMSCapEx,byMSCapExNum);		

	}
	else
	{
		StaticLog( "[MatchVideoCommonCap]Remote no main type as local main!\n");
	}

	if(tMainExtraSimCapSet.GetVideoMediaType()!=MEDIA_TYPE_NULL)
	{
		tCommonCap.SetExtraVideoCap(tMainExtraSimCapSet.GetVideoCap(),0);
	}
    
    if (bMainMatchSuccess)
    {
        //save��һ·����ʽ��ͬ������
        tCommonCap.SetMainSimCapSet(tMainSimCapSet);
		
		
    }
    else
    {
        //����ʽ��Ƶ�޹�ͬ����
        TVideoStreamCap tNullVideoCap;
        tNullVideoCap.Clear();
        tMainSimCapSet.SetVideoCap(tNullVideoCap);

        tCommonCap.SetMainSimCapSet(tMainSimCapSet);
        StaticLog( "[MatchVideoCommonCap] No Common Main Video Cap!\n");
    }
	

	//[2].ȡ��һ·����ʽ��ͬ������
    BOOL32 bSecondMatchSuccess = FALSE;
    TSimCapSet tSecondSimCapSet = tSimuCap2;
    tMainExtraSimCapSet.Clear();

    if(!tSecondSimCapSet.IsNull())
    {
        //��һ·����ʽ��Ƶ
        /*u16 wSecondAudioComType = GetComType(PayloadTypeIn2Out(tSecondSimCapSet.GetAudioMediaType()),
                                             byEncryptMode);
        if (!ptRemoteCapSet->IsExistsimultaneous(&wSecondAudioComType, 1))
        {
            tSecondSimCapSet.SetAudioMediaType(MEDIA_TYPE_NULL);
        }*/

        //��һ·����ʽ��Ƶ
        u16 wSecondVideoComType = GetComType(PayloadTypeIn2Out(tSecondSimCapSet.GetVideoMediaType()),
                                                               byEncryptMode);
        if (ptRemoteCapSet->IsExistsimultaneous(&wSecondVideoComType, 1))
        {
            //��һ·����ʽ������ƥ��
            bSecondMatchSuccess = MatchVideoCap(tSimuCap2,
												tSimuDSCap,
                                                ptRemoteCapSet,
                                                tSecondSimCapSet,
                                                byEncryptMode,
                                                tMainExtraSimCapSet,
												ptMSCapEx,byMSCapExNum);
        }
        else
        {
            StaticLog( "[MatchVideoCommonCap]Remote no Media type as Local Second!\n");
        }  
        
        if (bSecondMatchSuccess)
        {
            //save��һ·����ʽ��ͬ������
            tCommonCap.SetSecondSimCapSet(tSecondSimCapSet);

			if(tMainExtraSimCapSet.GetVideoMediaType() != MEDIA_TYPE_NULL)
			{
				tCommonCap.SetExtraVideoCap(tMainExtraSimCapSet.GetVideoCap(),0);
			}
			tSecondSimCapSet.SetAudioMediaType(tCommonCap.GetMainAudioType());
			if (tCommonCap.GetMainVideoType() == MEDIA_TYPE_NULL)
			{
				tCommonCap.SetMainSimCapSet(tSecondSimCapSet);
			}
        }
        else
        {
            StaticLog( "[MatchVideoCommonCap] No Common Second Cap!\n");
        }
    }

	//[3].ȡ˫����Ƶ��ͬ����
    BOOL32 bDSMatchSuccess = FALSE;
    BOOL32 bH239 = tLocalCap.IsDStreamSupportH239();  

    //��һ��ͬ˫��
    TDStreamCap tCommonDSCap;
    tCommonDSCap.Reset();

    //�ڶ���ͬ˫��
    TDStreamCap tSecComDStreamCap;
    tSecComDStreamCap.Reset();
    
    TVideoStreamCap tMainVideoCap = tCommonCap.GetMainSimCapSet().GetVideoCap();

	if ( MEDIA_TYPE_NULL != tLocalCap.GetDStreamMediaType())
	{//��һ·����ʽƥ��ɹ���ƥ��˫��
                
        //��һ·����ʽ+˫��
		bDSMatchSuccess = MatchDSVideoCap(tMainVideoCap,
                                          tSimuDSCap,
                                          ptRemoteCapSet,
                                          tCommonDSCap,
                                          tSecComDStreamCap,
                                          byEncryptMode,
										  ptDSCapEx,
										  byDSCapEXNum);

        if (bDSMatchSuccess)
        {
            //����˫����ͬ������
            tCommonCap.SetDStreamCapSet(tCommonDSCap);
			//����ڶ���ͬ˫��
            if (MEDIA_TYPE_NULL != tSecComDStreamCap.GetMediaType())
            {
                tCommonCap.SetSecComDStreamCapSet(tSecComDStreamCap);
            }
        }
    }
    else
    {
        StaticLog( "[MatchVideoCommonCap]local cap no Double Stream!");
    }

    if ( MEDIA_TYPE_NULL != tCapEx.GetSecDSType())
    {//��һ·����ʽ+��һ˫�����ɹ����õ�һ·����ʽ+�ڶ�˫��

        TDStreamCap tDStreamCap2;
        tDStreamCap2.SetSupportH239(bH239);
        tDStreamCap2.SetMediaType(tCapEx.GetSecDSType());
        tDStreamCap2.SetMaxBitRate(tCapEx.GetSecDSBitRate());
        tDStreamCap2.SetResolution(tCapEx.GetSecDSRes());
        
        if (MEDIA_TYPE_H264 == tCapEx.GetSecDSType())
        {
            tDStreamCap2.SetUserDefFrameRate(tCapEx.GetSecDSFrmRate());
        }
        else
        {
            tDStreamCap2.SetFrameRate(tCapEx.GetSecDSFrmRate());
        }

		tCommonDSCap.Reset();
		tSecComDStreamCap.Reset();

        //��һ·����ʽ+�ڶ�˫��
        bDSMatchSuccess = MatchDSVideoCap( tMainVideoCap,
                                           tDStreamCap2,
                                           ptRemoteCapSet,
                                           tCommonDSCap,
                                           tSecComDStreamCap,
                                           byEncryptMode,
										   ptDSCapEx,
										   byDSCapEXNum);

        if (bDSMatchSuccess)
        {            
            
            //����˫����ͬ������
            //���˫˫�������ȵ�ǰ��һ��ڶ�˫���ֱ��ʴ�||�ֱ�����ȵ������֡�ʴ��ڵ�һ��ڶ�˫��
            //��ô��˫˫���滻��һ��ڶ�˫�����滻��ǰ���Ƿֱ�����ͬ���ͣ�ͬΪ��̬��ͬΪ��̬��
            if ( MEDIA_TYPE_NULL == tCommonCap.GetDStreamMediaType() ||
                 ( IsResIsSameType(tCommonCap.GetDStreamResolution(), tCommonDSCap.GetResolution()) &&
                   ( tCommonCap.GetDStreamResolution() < tCommonDSCap.GetResolution())))
            {
                tCommonCap.SetDStreamCapSet(tCommonDSCap);
            }
            else
            {
                TDStreamCap tSecComDStreamSet = tCommonCap.GetSecComDStreamCapSet();
                if ( MEDIA_TYPE_NULL == tSecComDStreamSet.GetMediaType() ||
                     ( IsResIsSameType( tSecComDStreamSet.GetResolution(), tCommonDSCap.GetResolution()) &&
                       ( tSecComDStreamSet.GetResolution() < tCommonDSCap.GetResolution())))
                {
                    tCommonCap.SetSecComDStreamCapSet(tCommonDSCap);
                }
            }
        }
    }


    TDStreamCap tComDStream = tCommonCap.GetDStreamCapSet();
    TDStreamCap tSecComDStream = tCommonCap.GetSecComDStreamCapSet();
	//ͬΪH264,��ѡ�ֱ��ʣ�֡�ʳ˻��ϴ�����[9/5/2012 chendaiwei]
	if( tComDStream.GetMediaType() == MEDIA_TYPE_H264 &&
		tSecComDStream.GetMediaType() == MEDIA_TYPE_H264 &&
		tComDStream.IsH264CapLower(tSecComDStream.GetResolution(),tSecComDStream.GetUserDefFrameRate()))
	{
        tCommonCap.SetDStreamCapSet(tSecComDStream);
        tCommonCap.SetSecComDStreamCapSet(tComDStream);
	}
    //���ȷֱ��ʸߵ�˫��
	else if( tSecComDStream.GetMediaType() != MEDIA_TYPE_NULL
		&& tComDStream.IsResLowerInProduct(tComDStream.GetResolution(), tSecComDStream.GetResolution()))
	{
        tCommonCap.SetDStreamCapSet(tSecComDStream);
        tCommonCap.SetSecComDStreamCapSet(tComDStream);
    }

	return bMainMatchSuccess | bSecondMatchSuccess;
}

/*=============================================================================
�� �� ���� MatchVideoCap
��    �ܣ� ƥ���һ·��Ƶ������
�㷨ʵ�֣�
ȫ�ֱ����� 
��    ���� const TSimCapSet &tLocalCap  [IN]���ص�һ·������
		   const TDStreamCap &tLocalDStreamCap [IN]����˫��������
           TCapSet *ptRemoteCapSet      [IN]�Զ�������
           TSimCapSet &tCommonCap       [OUT]��һ·��ͬ������    
           u8 byEncryptMode             [IN]����ģʽ
           TSimCapSet &tExtraCommonCap        [OUT]�ڶ����һ·��ͬ���������������ڹ�ѡ��   
		   const TVideoStreamCap *ptMSCapEx   [IN]������չ��������ѡ����
		   u8 byMSCapExNum					  [IN]������չ��������ѡ��Ŀ
		   cosnt TVideoStreamCap *ptDSCapEx   [IN]˫����չ��������ѡ����
		   u8 byDSCapEXNum					  [IN]˫����չ��������ѡ��Ŀ

�� �� ֵ�� BOOL32:�Ƿ�ƥ��ɹ�  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
20110408    4.6         ��ʤ��                 create
20111216    4.7         �´�ΰ					modify
=============================================================================*/
BOOL32 CMtAdpUtils::MatchVideoCap(  const TSimCapSet &tLocalCap, 
									const TDStreamCap &tLocalDStreamCap,
								    TCapSet *ptRemoteCapSet, 
								    TSimCapSet &tCommonCap,
								    u8 byEncryptMode,
                                    TSimCapSet &tExtraCommonCap,
									const TVideoStreamCap *ptMSCapEx,
									u8 byMSCapExNum)
{
	u16 awStreamComType[2] = {0};
	u8 byPayload = PayloadTypeIn2Out(tLocalCap.GetVideoMediaType());
    u16 wFStreamComType = GetComType(byPayload, byEncryptMode);
	awStreamComType[0] = wFStreamComType;
	
    TH264VideoCap atH264Cap[2];	
    u8 byDStreamPayload = PayloadTypeIn2Out(tLocalDStreamCap.GetMediaType());
    awStreamComType[1] = GetComType(byDStreamPayload, byEncryptMode);
	
	// ��ʶ�����Ƿ��й�ͬ������ [12/8/2011 chendaiwei]
    BOOL32 bMatchSuccess = FALSE;

    u16 wBitrate = 0;
    u16 wLocalVideoMaxBitRate = tLocalCap.GetVideoMaxBitRate();

    if ( (u8)v_h264 == byPayload )
    {
		//1.�Ի������ֱ��ʣ���ȡ�𼶽��ֱ��ʵķ�ʽ�ȹ�ͬ�������������ڵڶ��鹲ͬ�������У���Ҫ��VCS��������ʹ��[12/16/2011 chendaiwei]
		// Convert to tStackCap
		TH264VideoCap tStackCap;
		TSimCapSet tLocalSimCapSet = tLocalCap;

		u8 abyRes[] = { VIDEO_FORMAT_HD1080, VIDEO_FORMAT_HD720, VIDEO_FORMAT_4CIF, VIDEO_FORMAT_CIF };
		u8 byResNum = sizeof(abyRes)/sizeof(u8);

		//������Ƶ�����ֱ���
		u8 byLocalResPos;
		for (byLocalResPos = 0; byLocalResPos < byResNum; byLocalResPos++)
		{
			if (tLocalSimCapSet.GetVideoCap().GetResolution() == abyRes[byLocalResPos])
			{
				break;
			}
			else
			{
				if ( byLocalResPos == byResNum - 1 )
				{
					LogPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[MatchVideoCap]not find Res of h264 video cap of local main cap !");
					return FALSE;
				}
				else
				{
					continue;
				}				
			}
		}

		//�ֱ�������
		for (u8 byResIdx = byLocalResPos; byResIdx < byResNum; byResIdx++)
		{
			//4cif���·ֱ������֡��Ϊ25֡
			if( VIDEO_FORMAT_4CIF >= abyRes[byResIdx] &&
				25 < tLocalSimCapSet.GetUserDefFrameRate() )			
			{	
				tLocalSimCapSet.SetUserDefFrameRate(25);								
			}
			
			tLocalSimCapSet.SetVideoResolution(abyRes[byResIdx]);
			
			TVideoStreamCap tLocalVideoCap = tLocalSimCapSet.GetVideoCap();			
			tStackCap.Clear();
			VideoCap264In2Out(tLocalVideoCap, tStackCap);
			
			if (ptRemoteCapSet->IsExistsimultaneous(&wFStreamComType, 1, &tStackCap))
			{//ƥ��ɹ������ù�ͬ������
				
				tExtraCommonCap.SetVideoCap(tLocalVideoCap);
				break;
			} 
			
			//ƥ��ʧ��,�л�HP/BP����ƥ��				
			if( tLocalSimCapSet.GetVideoProfileType() == emHpAttrb )
			{
				tLocalVideoCap.SetH264ProfileAttrb(emBpAttrb);
				tStackCap.Clear();
				VideoCap264In2Out(tLocalVideoCap, tStackCap);

				if (ptRemoteCapSet->IsExistsimultaneous(&wFStreamComType, 1, &tStackCap))
				{
					tExtraCommonCap.SetVideoCap(tLocalVideoCap);
					break;
				}
			}

			if( VIDEO_FORMAT_4CIF != tLocalSimCapSet.GetVideoResolution() &&
					VIDEO_FORMAT_CIF != tLocalSimCapSet.GetVideoResolution() &&
					tLocalVideoCap.GetUserDefFrameRate() > 30 )
			{
				if( tLocalSimCapSet.GetVideoProfileType() == emHpAttrb )
				{
					tLocalVideoCap.SetH264ProfileAttrb(emHpAttrb);
				}
				else
				{
					tLocalVideoCap.SetH264ProfileAttrb(emBpAttrb);
				}
				tLocalVideoCap.SetUserDefFrameRate( tLocalVideoCap.GetUserDefFrameRate() / 2 ) ;
				tStackCap.Clear();
				VideoCap264In2Out(tLocalVideoCap, tStackCap);

				if (ptRemoteCapSet->IsExistsimultaneous(&wFStreamComType, 1, &tStackCap))
				{
					tExtraCommonCap.SetVideoCap(tLocalVideoCap);
					break;
				}

				if( tLocalVideoCap.GetH264ProfileAttrb() == emHpAttrb )
				{
					tLocalVideoCap.SetH264ProfileAttrb(emBpAttrb);					
					tStackCap.Clear();
					VideoCap264In2Out(tLocalVideoCap, tStackCap);

					if (ptRemoteCapSet->IsExistsimultaneous(&wFStreamComType, 1, &tStackCap))
					{
						tExtraCommonCap.SetVideoCap(tLocalVideoCap);
						break;
					}
				}
			}			

			if(byResIdx == byResNum - 1)
			{
				tExtraCommonCap.SetVideoMediaType(MEDIA_TYPE_NULL);

				break;
			} 
			
		}

		// guzh [2/27/2007] �����ʽ��б���
        //[20110729 zhushz]pclnt err:666
        u16 wStackCapBitRate =  tStackCap.GetBitRate();
        
        if (wStackCapBitRate != 0)
        {
            if (wLocalVideoMaxBitRate != 0)
            {
                wBitrate = min(wStackCapBitRate, wLocalVideoMaxBitRate);
            }
            else
            {
                wBitrate = tStackCap.GetBitRate();
            }
        }
		tExtraCommonCap.SetVideoMaxBitRate(wBitrate);
		
		//2.���ý��湴ѡ��չ������ֱ�ӱ�������[12/16/2011 chendaiwei]
        // Convert to tStackCap
		TVideoStreamCap tLocalMainVideoCap = tLocalCap.GetVideoCap();
		tStackCap.Clear();
        VideoCap264In2Out(tLocalMainVideoCap, tStackCap);

		//��ʶ�Ƿ���˫����С��ͬ����H263+/XGA/5FPS��H264/XGA/5FPS  [12/8/2011 chendaiwei]
		BOOL32 bExistDSMinCap = FALSE;

		//  [12/8/2011 chendaiwei]��������ƥ��ɹ������ù�ͬ��������Э���Ƿ���˫����С��ͬ������
		if (ptRemoteCapSet->IsExistsimultaneous(&wFStreamComType, 1, &tStackCap))
		{	
			tCommonCap.SetVideoCap(tLocalMainVideoCap);
			bMatchSuccess = TRUE;

			// Э��˫����С��ͬ������ [12/8/2011 chendaiwei]
			//1)��H263+/XGA/5FPS����
			u8 byH263PlusPayload = PayloadTypeIn2Out(MEDIA_TYPE_H263PLUS);
			u16 wH263ComType = GetComType(byH263PlusPayload, byEncryptMode);
			if (tLocalDStreamCap.IsSupportH239() && 
				ptRemoteCapSet->IsSupportH239())
			{
				wH263ComType = GetH239Type(wH263ComType);
			}

			if (ptRemoteCapSet->IsExistsimultaneous(&wH263ComType, 1))
			{
				bExistDSMinCap = TRUE;
			}

			//2)��H264/XGA/5FPS����
			TVideoStreamCap tMinH264DSCap;
			tMinH264DSCap.Clear();
			tMinH264DSCap.SetMediaType(MEDIA_TYPE_H264);
			tMinH264DSCap.SetResolution(VIDEO_FORMAT_XGA);
			tMinH264DSCap.SetUserDefFrameRate(5);
			tMinH264DSCap.SetMaxBitRate(tLocalDStreamCap.GetMaxBitRate());

			atH264Cap[0].Clear();
			VideoCap264In2Out(tLocalMainVideoCap, atH264Cap[0]);
			atH264Cap[1].Clear();
			VideoCap264In2Out(tMinH264DSCap, atH264Cap[1]);

			if (ptRemoteCapSet->IsExistsimultaneous(awStreamComType, 2, atH264Cap))
			{
				bExistDSMinCap = TRUE;
			}
		}

		// 1.�����޹�ͬ������2.�����й�ͬ������������£�������˫����ͬ���������Ҫ�� [12/8/2011 chendaiwei]
		if(!bMatchSuccess || !bExistDSMinCap ) 
		{
			// ���α���������չ����������[12/8/2011 chendaiwei]
			u8 byIdx = 0;
			for(; byIdx < byMSCapExNum; byIdx++)
			{
				TVideoStreamCap tMSCapEx = ptMSCapEx[byIdx];

				if(tMSCapEx.GetMediaType() == MEDIA_TYPE_NULL)
				{
					break;
				}

				tStackCap.Clear();
				VideoCap264In2Out(tMSCapEx, tStackCap);
				
				//  [12/8/2011 chendaiwei]��չ������ƥ��ɹ������ù�ͬ������
				if (ptRemoteCapSet->IsExistsimultaneous(&wFStreamComType, 1, &tStackCap))
				{
					if(!bMatchSuccess)
					{
						tCommonCap.SetVideoCap(tMSCapEx);
						bMatchSuccess = TRUE;
					}

					// Э��˫����С��ͬ������ [12/8/2011 chendaiwei]
					//1)��H263+/XGA/5FPS����
					u8 byH263PlusPayload = PayloadTypeIn2Out(MEDIA_TYPE_H263PLUS);
					u16 wH263ComType = GetComType(byH263PlusPayload, byEncryptMode);
					if (tLocalDStreamCap.IsSupportH239() && 
						ptRemoteCapSet->IsSupportH239())
					{
						wH263ComType = GetH239Type(wH263ComType);
					}
					
					if (ptRemoteCapSet->IsExistsimultaneous(&wH263ComType, 1))
					{
						bExistDSMinCap = TRUE;
					}
					
					//��H264/XGA/5FPS����
					TVideoStreamCap tMinH264DSCap;
					tMinH264DSCap.Clear();
					tMinH264DSCap.SetMediaType(MEDIA_TYPE_H264);
					tMinH264DSCap.SetResolution(VIDEO_FORMAT_XGA);
					tMinH264DSCap.SetUserDefFrameRate(5);
					tMinH264DSCap.SetMaxBitRate(tLocalDStreamCap.GetMaxBitRate());
					
					atH264Cap[0].Clear();
					VideoCap264In2Out(tMSCapEx, atH264Cap[0]);
					atH264Cap[1].Clear();
					VideoCap264In2Out(tMinH264DSCap, atH264Cap[1]);
					
					if (ptRemoteCapSet->IsExistsimultaneous(awStreamComType, 2, atH264Cap))
					{
						bExistDSMinCap = TRUE;
					}
					
					// �����й�ͬ��������˫������С��ͬ���������ɹ����� [12/8/2011 chendaiwei]
					if(bMatchSuccess && bExistDSMinCap)
					{
						tCommonCap.SetVideoCap(tMSCapEx);
						break;
					}
				}
			}

			if(!bMatchSuccess)
			{
				tCommonCap.SetVideoMediaType(MEDIA_TYPE_NULL);
			}
		}

        // guzh [2/27/2007] �����ʽ��б���
        //[20110729 zhushz]pclnt err:666
        wStackCapBitRate =  tStackCap.GetBitRate();
        
        if (wStackCapBitRate != 0)
        {
            if (wLocalVideoMaxBitRate != 0)
            {
                wBitrate = min(wStackCapBitRate, wLocalVideoMaxBitRate);
            }
            else
            {
                wBitrate = tStackCap.GetBitRate();
            }
        }
    }
    else
    { 
        if( ptRemoteCapSet->IsExistsimultaneous( &wFStreamComType, 1) )
        {
            bMatchSuccess = TRUE;
        } 
        else
        {
            tCommonCap.SetVideoMediaType(MEDIA_TYPE_NULL);
            bMatchSuccess = FALSE;
        }

        TVideoCap* ptVideoCap = NULL;
        ptVideoCap = ptRemoteCapSet->GetVideoCap(byPayload);  
        
        u16 wVideoBitRate = 0;
        u8 byMPI = 0;
        u8 byVcRes = tLocalCap.GetVideoCap().GetResolution();
        if( NULL == ptVideoCap )
        {
            tCommonCap.SetVideoMediaType(MEDIA_TYPE_NULL);
            bMatchSuccess =  FALSE;
        }
        else
        {
            byMPI = ptVideoCap->GetResolution( (emResolution)ResIn2Out(byVcRes) );
            wVideoBitRate = ptVideoCap->GetBitRate();
        }
        
        if( 0 == byMPI )
        {
            tCommonCap.SetVideoMediaType(MEDIA_TYPE_NULL);
            bMatchSuccess = FALSE;
        }
        
        // guzh [2/27/2007] �����ʽ��б���
        if (wVideoBitRate != 0)
        {
            if (wLocalVideoMaxBitRate != 0)
            {
                wBitrate = min(wVideoBitRate, wLocalVideoMaxBitRate);
            }
            else
            {
                wBitrate = wVideoBitRate;
            }
        }

		tExtraCommonCap.SetVideoMediaType(MEDIA_TYPE_NULL);
    }										
    
    if ( wBitrate != 0 )
    {
        tCommonCap.SetVideoMaxBitRate(wBitrate);
    }

    return bMatchSuccess;
}

/*=============================================================================
�� �� ���� MatchDSVideoCap
��    �ܣ� ƥ��˫����ͬ������
�㷨ʵ�֣�
ȫ�ֱ����� 
��    ���� const TVideoStreamCap &tLocalMainVideoCap  [IN]���ص�һ·������
           const TDStreamCap &tLocalDStreamCap        [IN]����˫��������
           TCapSet *ptRemoteCapSet                    [IN]�Զ�������
           TVideoStreamCap  &tCommonDSCap             [OUT]��һ·��ͬ������    
           u8 byEncryptMode                           [IN]����ģʽ
           BOOL32 bH239                               [IN]�Ƿ��239��ǩ
           u8 byAdpFps                                [IN]����֡��

�� �� ֵ�� BOOL32:�Ƿ�ƥ��ɹ�  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
20110408    4.6         ��ʤ��                 create
=============================================================================*/
BOOL32 CMtAdpUtils:: MatchDSVideoCap(const TVideoStreamCap &tLocalMainVideoCap,
                                     const TDStreamCap &tLocalDStreamCap,
                                     TCapSet * ptRemoteCapSet,
                                     /*TVideoStreamCap*/TDStreamCap  &tCommonDSCap,
                                     TDStreamCap &tSecComDStreamCap,
                                     u8 byEncryptMode,
									 const TVideoStreamCap *ptDSCapEx,
									 u8 byDSCapEXNum)
{
    BOOL32 bMatchSuccess = FALSE;

    u16    awStreamComType[2] = {0}; 
    TH264VideoCap atH264Cap[2];

    u8 byFStreamPayload = PayloadTypeIn2Out(tLocalMainVideoCap.GetMediaType());
    awStreamComType[0] = GetComType(byFStreamPayload, byEncryptMode);

    u8 byDStreamPayload = PayloadTypeIn2Out(tLocalDStreamCap.GetMediaType());
    awStreamComType[1] = GetComType(byDStreamPayload, byEncryptMode);

    //�Զ���239��ǩ������Ҳ����239
    if (tLocalDStreamCap.IsSupportH239() && 
        ptRemoteCapSet->IsSupportH239())
    {
        awStreamComType[1] = GetH239Type(awStreamComType[1]);
    }

	

	//1)��˫��H264
	if ((u8)v_h264 == byFStreamPayload && (u8)v_h264 == byDStreamPayload)
	{
		TVideoStreamCap tDSVideoCap = (TVideoStreamCap)tLocalDStreamCap;   
		atH264Cap[1].Clear();
		VideoCap264In2Out(tDSVideoCap, atH264Cap[1]);
		atH264Cap[0].Clear();
		VideoCap264In2Out(tLocalMainVideoCap, atH264Cap[0]);

		//˫�����������ȹ�ͬ������
		if (ptRemoteCapSet->IsExistsimultaneous(awStreamComType, 2, atH264Cap))
		{
			//TODO Ϊʲô��ֱ���ñȳ�����˫��������������Ҫ��������֡��[12/8/2011 chendaiwei]
			//ȡ��ƥ�����֡�����õ�˫����ͬ��������
			/*u8 byRemoteFps = atH264Cap[1].GetResolution(CMtAdpUtils::ResIn2Out(tDSVideoCap.GetResolution()));
			u8 byLocalFps = tDSVideoCap.GetUserDefFrameRate();
			u8 byFps = (byLocalFps < byRemoteFps) ? byLocalFps : byRemoteFps;			
			tDSVideoCap.SetUserDefFrameRate(byFps);
			
			//��������
			tDSVideoCap.SetMaxBitRate(tLocalDStreamCap.GetMaxBitRate());*/
			
			//���ù�ͬ˫��
			memcpy(&tCommonDSCap, &tDSVideoCap, sizeof(TVideoStreamCap));
			tCommonDSCap.SetSupportH239(ptRemoteCapSet->IsSupportH239());
			
			bMatchSuccess = TRUE;
         }
		else
		{
			// ���α���˫����չ����������[12/8/2011 chendaiwei]
			u8 byIdx = 0;
			for(; byIdx < byDSCapEXNum; byIdx++)
			{
				TVideoStreamCap tDSCapEx = ptDSCapEx[byIdx];
				
				if(tDSCapEx.GetMediaType() == MEDIA_TYPE_NULL)
				{
					break;
				}
				
				atH264Cap[1].Clear();
				VideoCap264In2Out(tDSCapEx, atH264Cap[1]);
				atH264Cap[0].Clear();
				VideoCap264In2Out(tLocalMainVideoCap, atH264Cap[0]);

				if (ptRemoteCapSet->IsExistsimultaneous(awStreamComType, 2, atH264Cap))
				{
					//TODO Ϊʲô��ֱ���ñȳ�����˫��������������Ҫ��������֡��[12/8/2011 chendaiwei]
					//ȡ��ƥ�����֡�����õ�˫����ͬ��������
					/*u8 byRemoteFps = atH264Cap[1].GetResolution(CMtAdpUtils::ResIn2Out(tDSCapEx.GetResolution()));
					u8 byLocalFps = tDSCapEx.GetUserDefFrameRate();
					u8 byFps = (byLocalFps < byRemoteFps) ? byLocalFps : byRemoteFps;			
					tDSCapEx.SetUserDefFrameRate(byFps);*/					
					
					//���ù�ͬ˫��
					memcpy(&tCommonDSCap, &tDSCapEx, sizeof(TVideoStreamCap));
					tCommonDSCap.SetSupportH239(ptRemoteCapSet->IsSupportH239());
					
					bMatchSuccess = TRUE;
					break;
				}
			}
			
			if(byIdx == byDSCapEXNum || ptDSCapEx[byIdx].GetMediaType() == MEDIA_TYPE_NULL)
			{
				tCommonDSCap.SetMediaType(MEDIA_TYPE_NULL);
				bMatchSuccess = FALSE;
			}
		}
	}
	//2)˫��H264
	else if((u8)v_h264 != byFStreamPayload && (u8)v_h264 == byDStreamPayload)
	{
		TVideoStreamCap tDSVideoCap = (TVideoStreamCap)tLocalDStreamCap;   
		atH264Cap[1].Clear();
		VideoCap264In2Out(tDSVideoCap, atH264Cap[1]);
		
		//˫����������
		if (ptRemoteCapSet->IsExistsimultaneous(&awStreamComType[1], 1, &atH264Cap[1]))
		{
			//TODO Ϊʲô��ֱ���ñȳ�����˫��������������Ҫ��������֡��[12/8/2011 chendaiwei]
			//ȡ��ƥ�����֡�����õ�˫����ͬ��������
			/*u8 byRemoteFps = atH264Cap[1].GetResolution(CMtAdpUtils::ResIn2Out(tDSVideoCap.GetResolution()));
			u8 byLocalFps = tDSVideoCap.GetUserDefFrameRate();
			u8 byFps = (byLocalFps < byRemoteFps) ? byLocalFps : byRemoteFps;			
			tDSVideoCap.SetUserDefFrameRate(byFps);
			
			//��������
			tDSVideoCap.SetMaxBitRate(tLocalDStreamCap.GetMaxBitRate());*/
			
			//���ù�ͬ˫��
			memcpy(&tCommonDSCap, &tDSVideoCap, sizeof(TVideoStreamCap));
			tCommonDSCap.SetSupportH239(ptRemoteCapSet->IsSupportH239());
			
			bMatchSuccess = TRUE;
        }
		//˫����չ������
		else
		{
			// ���α���˫����չ����������[12/8/2011 chendaiwei]
			u8 byIdx = 0;
			for(; byIdx < byDSCapEXNum; byIdx++)
			{
				TVideoStreamCap tDSCapEx = ptDSCapEx[byIdx];
				
				if(tDSCapEx.GetMediaType() == MEDIA_TYPE_NULL)
				{
					break;
				}
				
				atH264Cap[1].Clear();
				VideoCap264In2Out(tDSCapEx, atH264Cap[1]);

				if (ptRemoteCapSet->IsExistsimultaneous(&awStreamComType[1], 1, &atH264Cap[1]))
				{
					//TODO Ϊʲô��ֱ���ñȳ�����˫��������������Ҫ��������֡��[12/8/2011 chendaiwei]
					//ȡ��ƥ�����֡�����õ�˫����ͬ��������
					/*u8 byRemoteFps = atH264Cap[1].GetResolution(CMtAdpUtils::ResIn2Out(tDSCapEx.GetResolution()));
					u8 byLocalFps = tDSCapEx.GetUserDefFrameRate();
					u8 byFps = (byLocalFps < byRemoteFps) ? byLocalFps : byRemoteFps;			
					tDSCapEx.SetUserDefFrameRate(byFps);	*/		
					
					
					//���ù�ͬ˫��
					memcpy(&tCommonDSCap, &tDSCapEx, sizeof(TVideoStreamCap));
					tCommonDSCap.SetSupportH239(ptRemoteCapSet->IsSupportH239());
					
					bMatchSuccess = TRUE;
					break;
				}
			}
			
			if(byIdx == byDSCapEXNum || ptDSCapEx[byIdx].GetMediaType() == MEDIA_TYPE_NULL)
			{
				tCommonDSCap.SetMediaType(MEDIA_TYPE_NULL);
				bMatchSuccess = FALSE;
			}
		}
	}
	//3)˫����H264
	else if((u8)v_h264 != byDStreamPayload)
	{
        if (ptRemoteCapSet->IsExistsimultaneous(&awStreamComType[1], 1))
        {
            tCommonDSCap.SetMediaType(tLocalDStreamCap.GetMediaType());
            tCommonDSCap.SetResolution(tLocalDStreamCap.GetResolution());
            tCommonDSCap.SetFrameRate(tLocalDStreamCap.GetFrameRate());
            tCommonDSCap.SetMaxBitRate(tLocalDStreamCap.GetMaxBitRate());
            tCommonDSCap.SetSupportH239(ptRemoteCapSet->IsSupportH239());
            bMatchSuccess = TRUE;             
        }
	}

	//���㲻ͬ���͵�˫���ڶ���ͬ������
    //��һ��ͬ˫����ڶ���ͬ˫����һ��Ϊ��̬����һ��Ϊ��̬
    if (bMatchSuccess)
    {
        if ((u8)v_h264 == byDStreamPayload)
        {
			TVideoStreamCap tDSFirstCap =  (TVideoStreamCap)tCommonDSCap;

			if ((u8)v_h264 == byFStreamPayload)
			{
				//TVideoStreamCap tLocalDSVideoCap = (TVideoStreamCap)tLocalDStreamCap; 
				// ���α���˫����չ����������[12/8/2011 chendaiwei]
				u8 byIdx = 0;
				for(; byIdx < byDSCapEXNum; byIdx++)
				{
					TVideoStreamCap tDSCapEx = ptDSCapEx[byIdx];

					if(IsResIsSameType(tDSCapEx.GetResolution(),tDSFirstCap.GetResolution()))
					{
						continue;
					}
					
					if(tDSCapEx.GetMediaType() == MEDIA_TYPE_NULL)
					{
						break;
					}
					
					atH264Cap[1].Clear();
					VideoCap264In2Out(tDSCapEx, atH264Cap[1]);
					atH264Cap[0].Clear();
					VideoCap264In2Out(tLocalMainVideoCap, atH264Cap[0]);
					
					if (ptRemoteCapSet->IsExistsimultaneous(awStreamComType, 2, atH264Cap))
					{
						//TODO Ϊʲô��ֱ���ñȳ�����˫��������������Ҫ��������֡��[12/8/2011 chendaiwei]
						//ȡ��ƥ�����֡�����õ�˫����ͬ��������
						/*u8 byRemoteFps = atH264Cap[1].GetResolution(CMtAdpUtils::ResIn2Out(tDSCapEx.GetResolution()));
						u8 byLocalFps = tDSCapEx.GetUserDefFrameRate();
						u8 byFps = (byLocalFps < byRemoteFps) ? byLocalFps : byRemoteFps;			
						tDSCapEx.SetUserDefFrameRate(byFps);*/						
						
						//�ڶ���ͬ˫������������
						memcpy(&tSecComDStreamCap, &tDSCapEx, sizeof(TVideoStreamCap));
                        tSecComDStreamCap.SetSupportH239(ptRemoteCapSet->IsSupportH239());
						
						break;
					}
				}
			}
			else
			{	
				//TVideoStreamCap tLocalDSVideoCap = (TVideoStreamCap)tLocalDStreamCap; 
				// ���α���˫����չ����������[12/8/2011 chendaiwei]
				u8 byIdx = 0;
				for(; byIdx < byDSCapEXNum; byIdx++)
				{
					TVideoStreamCap tDSCapEx = ptDSCapEx[byIdx];

					if(IsResIsSameType(tDSCapEx.GetResolution(),tDSFirstCap.GetResolution()))
					{
						continue;
					}
					
					if(tDSCapEx.GetMediaType() == MEDIA_TYPE_NULL)
					{
						break;
					}
					
					atH264Cap[1].Clear();
					VideoCap264In2Out(tDSCapEx, atH264Cap[1]);
					
					if (ptRemoteCapSet->IsExistsimultaneous(&awStreamComType[1], 1, &atH264Cap[1]))
					{
						//TODO Ϊʲô��ֱ���ñȳ�����˫��������������Ҫ��������֡��[12/8/2011 chendaiwei]
						//ȡ��ƥ�����֡�����õ�˫����ͬ��������
						/*u8 byRemoteFps = atH264Cap[1].GetResolution(CMtAdpUtils::ResIn2Out(tDSCapEx.GetResolution()));
						u8 byLocalFps = tDSCapEx.GetUserDefFrameRate();
						u8 byFps = (byLocalFps < byRemoteFps) ? byLocalFps : byRemoteFps;			
						tDSCapEx.SetUserDefFrameRate(byFps);*/						
						
						//�ڶ���ͬ˫������������
						memcpy(&tSecComDStreamCap, &tDSCapEx, sizeof(TVideoStreamCap));
                        tSecComDStreamCap.SetSupportH239(ptRemoteCapSet->IsSupportH239());
						
						break;
					}
				}
			}
		}
	}

    return bMatchSuccess;
}

/*=============================================================================
�� �� ���� ReduceOneLevelMainCap
��    �ܣ� ����һ·������һ���ȼ�
�㷨ʵ�֣�
ȫ�ֱ����� 
��    ���� TVideoStreamCap &tViedoCap   [IN/OUT]��һ·����
           u8 byAdpFps                  [IN]����֡��
           u8 byOldFps                  [IN]ԭʼ֡��

�� �� ֵ�� BOOL32:�Ƿ�ƥ��ɹ�  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
20110408    4.6         ��ʤ��                 create
=============================================================================*/
BOOL32 CMtAdpUtils::ReduceOneLevelMainCap(TVideoStreamCap &tViedoCap, u8 byAdpFps, u8 byOldFps)
{
   
    if (MEDIA_TYPE_NULL == tViedoCap.GetMediaType())
    {
        return FALSE;
    }
    
    u8 abyRes[] = { VIDEO_FORMAT_HD1080, VIDEO_FORMAT_HD720, VIDEO_FORMAT_4CIF, VIDEO_FORMAT_CIF };
    u8 byResNum = sizeof(abyRes)/sizeof(u8);

    u8 byFps;
    if(MEDIA_TYPE_H264 == tViedoCap.GetMediaType())
    {
        byFps = tViedoCap.GetUserDefFrameRate();
    }
    else
    {
        byFps = tViedoCap.GetFrameRate();
    }


    if (byFps > byAdpFps && 0 != byAdpFps)
    {
        if(tViedoCap.GetMediaType() == MEDIA_TYPE_H264)
        {
            tViedoCap.SetUserDefFrameRate(byAdpFps);
        }
        else
        {
            tViedoCap.SetFrameRate(byAdpFps);
        }

        return TRUE;
    }
    else
    {//�Ѿ�����֡�ʣ���֡�ʸ�ԭ

        if(tViedoCap.GetMediaType() == MEDIA_TYPE_H264)
        {
            tViedoCap.SetUserDefFrameRate(byOldFps);
        }
        else
        {
            tViedoCap.SetFrameRate(byOldFps);
        }
        
    }

    u8 byResPos;
    for(byResPos = 0; byResPos < byResNum; byResPos++)
    {
        if (tViedoCap.GetResolution() == abyRes[byResPos])
        {
            break;
        } 
        else
        {
            if (byResNum - 1 == byResPos)
            {
                return FALSE;             
            }
            else
            {
                continue;
            }
        }
    }

    if ( byResPos >= byResNum - 1/*VIDEO_FORMAT_CIF == abyRes[byResPos]*/ )
    {
        return FALSE;
    }
    else
    {
        tViedoCap.SetResolution(abyRes[byResPos + 1]);
        if (abyRes[byResPos + 1] <= VIDEO_FORMAT_4CIF &&
            byFps >25)
        {
            if(tViedoCap.GetMediaType() == MEDIA_TYPE_H264)
            {
                tViedoCap.SetUserDefFrameRate(25);
            }
            else
            {
                tViedoCap.SetFrameRate(25);
            }
        }
        return TRUE;
    }       
}
/*=============================================================================
�� �� ���� MaxDSCap264Out2In
��    �ܣ� H.264 Э��ջ�����ṹ����,ȡ�����˫������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ����TCapSet *ptRemoteCapSet:Զ������[IN]
		   const TDStreamCap &tLocalDStreamCap:����˫������[IN] 
		   const TVideoStreamCap &tMainStreamCap:������ͬ����[IN]
		   u8 byEncryptMode:��������[IN] 
		   TVideoStreamCap  &tCommonDSCap:��ͬ˫������[OUT]
		   BOOL32 bDSEqMV:�Ƿ�ͬ����Ƶ[IN] 
�� �� ֵ�� BOOL32:�Ƿ�ȡ�����ͬ˫������  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2010-12-10	1.0			��ʤ��		���		����	
=============================================================================*/
// BOOL32 CMtAdpUtils::MaxDSCap264Out2In(TCapSet *ptRemoteCapSet, 
// 									  const TDStreamCap &tLocalDStreamCap, 
// 									  const TVideoStreamCap &tMainStreamCap,
// 									  u8 byEncryptMode, 
// 									  TVideoStreamCap  &tCommonDSCap,
// 									  BOOL32 bDSEqMV)
// {
// 
// 	BOOL32 bSuppor = FALSE;	
// 	
// 	//���ֱַ��ʲ��ܶ�Ӧ���ⲿ����
// 	u8 abyRes[] = {VIDEO_FORMAT_HD1080, VIDEO_FORMAT_UXGA, /*VIDEO_FORMAT_16CIF,*/ VIDEO_FORMAT_SXGA, 
// 		           VIDEO_FORMAT_HD720, VIDEO_FORMAT_XGA, VIDEO_FORMAT_W4CIF, VIDEO_FORMAT_SVGA, 
// 				   VIDEO_FORMAT_4CIF, VIDEO_FORMAT_4SIF, VIDEO_FORMAT_VGA, VIDEO_FORMAT_2CIF, 
// 				   /*VIDEO_FORMAT_2SIF,*/ VIDEO_FORMAT_CIF, VIDEO_FORMAT_SIF, /*VIDEO_FORMAT_QCIF,*/ 
// 				   /*VIDEO_FORMAT_SQCIF,*/ VIDEO_FORMAT_AUTO};
// 	
// 	u8 abyEqMVRes[] = {VIDEO_FORMAT_HD1080, VIDEO_FORMAT_HD720, VIDEO_FORMAT_4CIF, VIDEO_FORMAT_CIF};
// 
// 	//�ֱ��ʸ���
// 	u8 byMaxResNum;
// 	if (bDSEqMV)
// 	{
// 		byMaxResNum = sizeof(abyEqMVRes)/sizeof(u8);
// 	}
// 	else
// 	{
// 		byMaxResNum = sizeof(abyRes)/sizeof(u8);
// 	}
// 
// 	u16 awStreamComType[2] = {0,0};
// 	
// 	//��ȡ��ͬ�������е�һ������
// 	u8 byPayLoadM = PayloadTypeIn2Out(tMainStreamCap.GetMediaType());
// 	awStreamComType[0] = GetComType(byPayLoadM, byEncryptMode);
// 	
// 	//��ȡ����˫������
// 	TVideoStreamCap tDSVideoCap = (TVideoStreamCap)tLocalDStreamCap;
// 	u8 byPayloadD = PayloadTypeIn2Out(tDSVideoCap.GetMediaType());		
// 	awStreamComType[1] = GetComType(byPayloadD, byEncryptMode);
// 
// 	TH264VideoCap tLocal264Cap[2];
// 	tLocal264Cap[0].Clear();
// 	
// 	if ((u8)v_h264 == byPayLoadM)
// 	{			
// 		VideoCap264In2Out(tMainStreamCap, tLocal264Cap[0]);			
// 	}	
// 
// 	//��ͬ����ʽ�¸�˫����239��ǩ
// 	if(!bDSEqMV)  	
//   	{
// 		awStreamComType[1] = GetH239Type(awStreamComType[1]);
//  	}
// 	
// 	for (u8 byLoop = 0; byLoop < byMaxResNum; byLoop++)
// 	{
// 		//���÷ֱ���
// 		if (bDSEqMV)
// 		{
// 			tDSVideoCap.SetResolution( abyEqMVRes[byLoop] );
// 		}
// 		else
// 		{
// 			tDSVideoCap.SetResolution( abyRes[byLoop] );
// 		}
// 
// 		tLocal264Cap[1].Clear();
// 		VideoCap264In2Out(tDSVideoCap, tLocal264Cap[1]);
// 				
// 		if ((u8)v_h264 == byPayLoadM)
// 		{//��һ·ҲΪ264����ô����·			
// 			if(ptRemoteCapSet->IsExistsimultaneous(awStreamComType, 2, tLocal264Cap))
// 			{
// 				u8 byRemoteFps = tLocal264Cap[1].GetResolution(CMtAdpUtils::ResIn2Out(tDSVideoCap.GetResolution()));
// 				u8 byLocalFps = tLocalDStreamCap.GetUserDefFrameRate();
// 				u8 byFps = (byLocalFps < byRemoteFps) ? byLocalFps : byRemoteFps;			
// 	 			tDSVideoCap.SetUserDefFrameRate(byFps);
// 				tCommonDSCap = tDSVideoCap;	
// 				bSuppor = TRUE;
// 				break;
// 			}
// 			
// 			//��ͬ����Ƶ�½�һ��֡���ٽ��бȽ�
// 			if(!bDSEqMV)
// 			{
// 				tLocal264Cap[1].SetMaxMBPS( tLocal264Cap[1].GetMaxFS() );
// 				if ( ptRemoteCapSet->IsExistsimultaneous( awStreamComType, 2, tLocal264Cap ) )
// 				{
// 					u8 byFps = tLocal264Cap[1].GetResolution( CMtAdpUtils::ResIn2Out(tDSVideoCap.GetResolution()) );
// 					tDSVideoCap.SetUserDefFrameRate(byFps);
// 					tCommonDSCap = tDSVideoCap;
// 					bSuppor = TRUE;
// 					break;
// 				}
// 			}
// 
// 			//���⴦���ܿ�SVGA������3֡��XGA
// 			if( tDSVideoCap.GetResolution() == VIDEO_FORMAT_SVGA )
// 			{
// 				TVideoStreamCap tTempVideoCap = tDSVideoCap;
// 				tTempVideoCap.SetResolution( VIDEO_FORMAT_XGA );
// 				tTempVideoCap.SetUserDefFrameRate( 3 );
// 				tLocal264Cap[1].Clear();
// 				VideoCap264In2Out(tTempVideoCap, tLocal264Cap[1]);
// 				if(ptRemoteCapSet->IsExistsimultaneous(awStreamComType, 2, tLocal264Cap))
// 				{
// 					tCommonDSCap = tTempVideoCap;
// 					bSuppor = TRUE;
// 					break;
// 				}
// 			}
// 		}
// 		else
// 		{//��һ·��Ϊ264����ô��˫��
// 			if(ptRemoteCapSet->IsExistsimultaneous(&awStreamComType[1], 1, &tLocal264Cap[1]))
// 			{				
// 				u8 byRemoteFps = tLocal264Cap[1].GetResolution(CMtAdpUtils::ResIn2Out(tDSVideoCap.GetResolution()));
// 				u8 byLocalFps = tLocalDStreamCap.GetUserDefFrameRate();
// 				u8 byFps = (byLocalFps < byRemoteFps) ? byLocalFps : byRemoteFps;
// 	 			tDSVideoCap.SetUserDefFrameRate(byFps);
// 				tCommonDSCap = tDSVideoCap;			
// 				bSuppor = TRUE;
// 				break;
// 			}
// 
// 			//��ͬ����Ƶ�½�һ���ֱ����ٽ��бȽ�
// 			if(!bDSEqMV)
// 			{
// 				tLocal264Cap[1].SetMaxMBPS( tLocal264Cap[1].GetMaxFS() );
// 				if ( ptRemoteCapSet->IsExistsimultaneous( &awStreamComType[1], 1, &tLocal264Cap[1]))
// 				{
// 					u8 byFps = tLocal264Cap[1].GetResolution( CMtAdpUtils::ResIn2Out(tDSVideoCap.GetResolution()) );
// 					tDSVideoCap.SetUserDefFrameRate(byFps);
// 					tCommonDSCap = tDSVideoCap;
// 					bSuppor = TRUE;
// 					break;
// 				}
// 			}
// 
// 			//���⴦���ܿ�SVGA������3֡��XGA
// 			if( tDSVideoCap.GetResolution() == VIDEO_FORMAT_SVGA )
// 			{
// 				TVideoStreamCap tTempVideoCap = tDSVideoCap;
// 				tTempVideoCap.SetResolution( VIDEO_FORMAT_XGA );
// 				tTempVideoCap.SetUserDefFrameRate( 3 );
// 				tLocal264Cap[1].Clear();
// 				VideoCap264In2Out(tTempVideoCap, tLocal264Cap[1]);
// 				if(ptRemoteCapSet->IsExistsimultaneous(&awStreamComType[1], 1, &tLocal264Cap[1]))
// 				{
// 					tCommonDSCap = tTempVideoCap;
// 					bSuppor = TRUE;
// 					break;
// 				}
// 			}
// 		}
// 		
// 	}			
// 	return bSuppor;
// }

/*=============================================================================
�� �� ���� VideoCap264Out2In
��    �ܣ� H.264 Э��ջ�����ṹ����
�㷨ʵ�֣� �ֱ�������
ȫ�ֱ����� 
��    ���� BOOL32 bDual���Ƿ�ȡ˫����ֱ���
		   u8 byDSFPS��������֡��ȡ�ֱ���
           BOOL32 bDSEqMV: �Ƿ�Ϊͬ����Ƶ��˫��
�� �� ֵ�� void  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/08/12  4.0R5	    ����                  ����
2008/09/03	4.5			�ű���					���Ӷ�˫�����͵Ļ�ȡ����
2008/04/29	4.5			�ű���					����˫�����ʹ���
=============================================================================*/
void CMtAdpUtils::VideoCap264Out2In(const TH264VideoCap     tRemoteCap, 
                                    const TVideoStreamCap  &tLocalCap,
                                    TVideoStreamCap        &tInCap,
                                    BOOL32                  bDual,
                                    u8                      byDSFPS,
                                    BOOL32                  bDSEqMV)
{
    tInCap.SetMediaType(MEDIA_TYPE_H264);

	if (!bDual)
	{
		u8 byLocalFrameRate = tLocalCap.GetUserDefFrameRate();
        u8 byLocalCIF4CIFFrmRate = byLocalFrameRate > 25 ? 25 : byLocalFrameRate;
		if ( tRemoteCap.IsSupport( (u8)emHD1080, byLocalFrameRate ) )
		{
			tInCap.SetResolution( VIDEO_FORMAT_HD1080 );
			tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)emHD1080) );
		}
		else if ( tRemoteCap.IsSupport( (u8)emHD720, byLocalFrameRate ) )
		{
			tInCap.SetResolution( VIDEO_FORMAT_HD720 );
			tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)emHD720) );
		}
		else if ( tRemoteCap.IsSupport( (u8)emCIF4, byLocalCIF4CIFFrmRate ) )
		{
			tInCap.SetResolution( VIDEO_FORMAT_4CIF);
			tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)emCIF4) );
		} 
		else if ( tRemoteCap.IsSupport( (u8)emCIF, byLocalCIF4CIFFrmRate ) )
		{
			tInCap.SetResolution( VIDEO_FORMAT_CIF );
			tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)emCIF) );
		} 
		else
		{
			tInCap.SetMediaType(MEDIA_TYPE_NULL);
		}
	}
	else
	{
		//zbq[09/03/2008] FIXME:˫�������Ļ�ȡ��ʱ��5֡�������жϣ����ܻ��г���...(�Ժ�������ļ���)
		//zbq[09/17/2008] ˫��������ȡ֡�ʰ���������ߣ�����Ϊ0����Ĭ��5֡

		if (byDSFPS == 0)
		{
			byDSFPS = 5;
		}

        if (!bDSEqMV)
        {
            if ( tRemoteCap.IsSupport( (u8)emUXGA, byDSFPS ) )
            {
                tInCap.SetResolution( VIDEO_FORMAT_UXGA );
                tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)emUXGA) );
            }
            else if ( tRemoteCap.IsSupport( (u8)emSXGA, byDSFPS ) )
            {
                tInCap.SetResolution( VIDEO_FORMAT_SXGA );
                tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)emSXGA) );
            }
            else if ( tRemoteCap.IsSupport( (u8)em1024768, byDSFPS ) )
            {
                tInCap.SetResolution( VIDEO_FORMAT_XGA );
                tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)em1024768) );
            }
            else if ( tRemoteCap.IsSupport( (u8)em800600, byDSFPS ) )
            {
                tInCap.SetResolution( VIDEO_FORMAT_SVGA );
                tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)em800600) );
            }
            else if ( tRemoteCap.IsSupport( (u8)em640480, byDSFPS ) )
            {
                tInCap.SetResolution( VIDEO_FORMAT_VGA );
                tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)em640480) );
            }
            else
            {
                tInCap.SetMediaType(MEDIA_TYPE_NULL);
            }
        }
        else
        {
			if ( tRemoteCap.IsSupport( (u8)emHD1080, byDSFPS ) )
            {
                tInCap.SetResolution( VIDEO_FORMAT_HD1080);
                tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)emHD1080) );
            } 
            else if ( tRemoteCap.IsSupport( (u8)emHD720, byDSFPS ) )
            {
                tInCap.SetResolution( VIDEO_FORMAT_HD720);
                tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)emHD720) );
            } 
            else if ( tRemoteCap.IsSupport( (u8)emCIF4, byDSFPS ) )
            {
                tInCap.SetResolution( VIDEO_FORMAT_4CIF );
                tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)emCIF4) );
            }
            else if ( tRemoteCap.IsSupport( (u8)emCIF, byDSFPS ) )
            {
                tInCap.SetResolution( VIDEO_FORMAT_CIF );
                tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)emCIF) );
            }
            else
            {
                tInCap.SetMediaType(MEDIA_TYPE_NULL);
            }
        }
	}

    return;
}


/*=============================================================================
  �� �� ���� GetVideoBitrate 
  ��    �ܣ� ����������ȡ��Ƶ���� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TCapSet *ptCapSet
  u8 byMediaType
  �� �� ֵ�� u16 
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
2005/8/25    3.6			������              ����
2008/04/15  4.0R5	    ����                 ��֧��H.264
=============================================================================*/
/*lint -save -esym(429, ptCapSet)*/
u16 CMtAdpUtils::GetVideoBitrate( TCapSet *ptCapSet, u8 byMediaType )
{
    u16 wBitrate = 0;

    if( NULL != ptCapSet )
    {
        switch(byMediaType)
        {
        case MEDIA_TYPE_H261:
            wBitrate = ptCapSet->GetVideoCap((u16)v_h261)->GetBitRate();
            break;
			
        case MEDIA_TYPE_H262:
            wBitrate = ptCapSet->GetVideoCap((u16)v_h262)->GetBitRate();
            break;
			
        case MEDIA_TYPE_H263:
            wBitrate = ptCapSet->GetVideoCap((u16)v_h263)->GetBitRate();
            break;
			
        case MEDIA_TYPE_H263PLUS:
            wBitrate = ptCapSet->GetVideoCap((u16)v_h263plus)->GetBitRate();
            break;

        case MEDIA_TYPE_H264:
            StaticLog( "[GetVideoBitrate] Get H.264 Video Bitrate failed!\n");
            //guzh FIXME: ��ʱ��ô����
            wBitrate = ptCapSet->GetH264VideoCap(0)->GetBitRate();
            break;
			
        case MEDIA_TYPE_MP4:
            wBitrate = ptCapSet->GetVideoCap((u16)v_mpeg4)->GetBitRate();
            break;

        default:
            break;
        }
    }
	
    return wBitrate;
}
/* lint -restore*/
/*=============================================================================
  �� �� ���� GetComType
  ��    �ܣ� �õ�ý������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byMediaType
             u8 byEncrypt
  �� �� ֵ�� u16 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/1/14                 xsl                   ����
=============================================================================*/
u16 CMtAdpUtils::GetComType( u8 byMediaType, u8 byEncrypt )
{
	if( emEncryptTypeNone != (emEncryptType)byEncrypt )
	{
		return GetEType(byMediaType, byEncrypt);
	}
	else
	{
		return (u16)byMediaType;
	}
}

/*=============================================================================
  �� �� ���� EncryptTypeIn2Out
  ��    �ܣ� ����ģʽ: mcu��������ת��ΪЭ��ջ��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byEncryptMode
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2004/12/30				xsl                     ����
=============================================================================*/
u8 CMtAdpUtils::EncryptTypeIn2Out( u8 byEncryptMode )
{
	switch(byEncryptMode)
	{
	case CONF_ENCRYPTMODE_DES:		return (u8)emEncryptTypeDES;
	case CONF_ENCRYPTMODE_AES:		return (u8)emEncryptTypeAES;
	default:						return (u8)emEncryptTypeNone; //param invalid from mcu	
	}
}

/*=============================================================================
  �� �� ���� EncryptTypeOut2In
  ��    �ܣ� ����ģʽ��Э��ջ����ת��Ϊmcu����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� emEncryptType emType
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2004/12/30				xsl                     ����
=============================================================================*/
u8 CMtAdpUtils::EncryptTypeOut2In(emEncryptType emType)
{
	switch(emType)
	{
	case emEncryptTypeDES:			return CONF_ENCRYPTMODE_DES;
	case emEncryptTypeAES:			return CONF_ENCRYPTMODE_AES;
	case emEncryptTypeNone:		
	default:						return CONF_ENCRYPTMODE_NONE;
	}
}

/*=============================================================================
  �� �� ���� AliasTypeIn2Out
  ��    �ܣ� �ڲ���������ת��Ϊ�ⲿ�ı�������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 mtAliasType
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang                  ����
=============================================================================*/
/*
u8 CMtAdpUtils::AliasTypeIn2Out( u8 byMtAliasType )
{
	switch( byMtAliasType ) 
	{
	case mtAliasTypeE164:			return type_e164;
	case mtAliasTypeH323ID:			return type_h323ID; 	
	default:						return type_others;
	}
}*/

/*=============================================================================
  �� �� ���� AliasTypeOut2In
  ��    �ܣ� �ⲿ��������ת��Ϊ�ڲ��ı�������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� AliasType type
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang                  ����
=============================================================================*/
/*
u8 CMtAdpUtils::AliasTypeOut2In( AliasType type )
{
	switch( type) 
	{
	case type_e164:	  return mtAliasTypeE164;
	case type_h323ID: return mtAliasTypeH323ID;	
	default:	      return mtAliasTypeOthers;
	}
}*/

/*=============================================================================
  �� �� ���� MtAliasIn2Out
  ��    �ܣ� �ڲ�����ת��Ϊ�ⲿ�����ṹ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  TMtAlias &tAlias
             cmAlias &tCmAlias
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang                  ����
=============================================================================*/
void CMtAdpUtils::MtAliasIn2Out( TMtAlias &tAlias, cmAlias &tCmAlias )
{
	/*
     *	AliasAddress ::= CHOICE
		{
			e164 IA5String (SIZE (1..128)) (FROM (��0123456789No. *,")),
			h323-ID BMPString (SIZE (1..256)), -- Basic ISO/IEC 10646-1 (Unicode)
			...,
			url-ID IA5String (SIZE(1..512)), -- URL style address
			transportID TransportAddress,
			email-ID IA5String (SIZE(1..512)), -- rfc822-compliant email address
			partyNumber PartyNumber
		}
	 */
	switch( tAlias.m_AliasType ) 
	{
	case mtAliasTypeE164:             tCmAlias.type = cmAliasTypeE164;             break;
	case mtAliasTypeH323ID:           tCmAlias.type = cmAliasTypeH323ID;           break;
	case mtAliasTypeTransportAddress: tCmAlias.type = cmAliasTypeTransportAddress; break;
	default:                          tCmAlias.type = cmAliasTypeH323ID;           break;	
	}

	if( (u8)mtAliasTypeTransportAddress == tAlias.m_AliasType )
	{
		tCmAlias.transport.type = cmTransportTypeIP;
		tCmAlias.transport.distribution = cmDistributionUnicast;
#ifdef WIN32
		tCmAlias.transport.ip = htonl(tAlias.m_tTransportAddr.GetIpAddr());
#else
		tCmAlias.transport.addr.v4.ip = htonl(tAlias.m_tTransportAddr.GetIpAddr());
#endif
		tCmAlias.transport.port = tAlias.m_tTransportAddr.GetPort();

		tCmAlias.string = NULL;
		tCmAlias.length = 0;
	}
	else if( (u8)mtAliasTypeE164 == tAlias.m_AliasType ) //IA5String
	{
		tAlias.m_achAlias[sizeof(tAlias.m_achAlias) - 1] = 0;

		if( tCmAlias.string )
		{
			strncpy( tCmAlias.string, tAlias.m_achAlias, 128 );

			//truncate illegal s8  
			for( /*s32*/u32 nAlias = 0; nAlias < sizeof(tAlias.m_achAlias) && tAlias.m_achAlias[nAlias]; nAlias ++ )
			{
				if( strchr("#*,0123456789", tAlias.m_achAlias[nAlias]))
				{
					tCmAlias.length ++;
				}
			}
		}				
	}
	else
	{	
		tAlias.m_achAlias[sizeof(tAlias.m_achAlias) - 1] = 0;
		if( tCmAlias.string )
		{			
#ifdef WIN32
            //[4/9/2013 liaokang] ���뷽ʽ֧��
            u16 wLen = 0;
            if( emenCoding_Utf8 == g_cMtAdpApp.GetMcuEncoding() )
            {
                wLen = (u16)utlUTF82Uni( tAlias.m_achAlias, (BYTE*)tCmAlias.string );
            } 
            else if( emenCoding_GBK == g_cMtAdpApp.GetMcuEncoding() )
            {
                wLen = (u16)utlChr2Bmp( tAlias.m_achAlias, (BYTE*)tCmAlias.string );
            }
#else
            //[4/9/2013 liaokang] ���뷽ʽ֧��
            u16 wLen = 0;
            if( emenCoding_Utf8 == g_cMtAdpApp.GetMcuEncoding() )
            {
                wLen = (u16)utlUTF82Uni( tAlias.m_achAlias, (RvUint8*)tCmAlias.string );
            } 
            else if( emenCoding_GBK == g_cMtAdpApp.GetMcuEncoding() )
            {
                wLen = (u16)utlChr2Bmp( tAlias.m_achAlias, (RvUint8*)tCmAlias.string );
            }
#endif
			tCmAlias.length = wLen;
		}		
	}

	return;
}


/*=============================================================================
  �� �� ���� MtAliasOut2In
  ��    �ܣ� �ⲿ����ת��Ϊ�ڲ������ṹ 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� const cmAlias &tCmAlias
             TMtAlias &tAlias
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang                  ����
=============================================================================*/
void CMtAdpUtils::MtAliasOut2In( const cmAlias &tCmAlias, TMtAlias &tAlias )
{
	switch( tCmAlias.type ) 
	{
	case cmAliasTypeE164:      
		tAlias.m_AliasType = (u8)mtAliasTypeE164; 		
		break;
		
	case cmAliasTypeH323ID:     
		{
			s8 achBuf[128] = { 0 };
			memcpy( achBuf, tCmAlias.string, min(sizeof(achBuf), tCmAlias.length));
#ifdef WIN32
#ifdef _UTF8
            //[4/9/2013 liaokang] ���뷽ʽ֧��
            if( emenCoding_Utf8 == g_cMtAdpApp.GetMcuEncoding() )
            {
                utlUni2UTF8((u8*)achBuf, sizeof(achBuf), tCmAlias.string);
            } 
            else if( emenCoding_GBK == g_cMtAdpApp.GetMcuEncoding() )
            {
                utlBmp2Chr(tCmAlias.string, (u8*)achBuf, sizeof(achBuf));
            }
#else
			utlBmp2Chr(tCmAlias.string, (u8*)achBuf, sizeof(achBuf));
#endif
#else
#ifdef _UTF8
            //[4/9/2013 liaokang] ���뷽ʽ֧��
            if( emenCoding_Utf8 == g_cMtAdpApp.GetMcuEncoding() )
            {
                utlUni2UTF8((RvUint16*)achBuf, sizeof(achBuf), tCmAlias.string);
            } 
            else if( emenCoding_GBK == g_cMtAdpApp.GetMcuEncoding() )
            {
                utlBmp2Chr(tCmAlias.string, (u8*)achBuf, sizeof(achBuf));
            }
#else
            utlBmp2Chr(tCmAlias.string, (u8*)achBuf, sizeof(achBuf));
#endif
#endif
			tAlias.m_AliasType = (u8)mtAliasTypeH323ID;	
		}			
		break;

	case cmAliasTypeTransportAddress: 
		tAlias.m_AliasType = (u8)mtAliasTypeTransportAddress; 
		break;

	default: 
		break;	
	}
	
	if( tCmAlias.type == cmAliasTypeTransportAddress )
	{		
#ifdef WIN32
		tAlias.m_tTransportAddr.SetIpAddr( ntohl(tCmAlias.transport.ip) );
#else
		tAlias.m_tTransportAddr.SetIpAddr( ntohl(tCmAlias.transport.addr.v4.ip) );
#endif
		tAlias.m_tTransportAddr.SetPort( tCmAlias.transport.port );
	}

	if( tCmAlias.string )
	{
		strncpy(tAlias.m_achAlias, tCmAlias.string, min(sizeof(tAlias.m_achAlias), tCmAlias.length));
	}
	return;		
}

/*=============================================================================
  �� �� ���� PszGetChannelName
  ��    �ܣ� ��ȡ�ŵ��� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byChannelType
             s8  * pszBuf
             u8 byBufLen 
  �� �� ֵ�� s8  * 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang                  ����
=============================================================================*/
s8* CMtAdpUtils::PszGetChannelName( u8 byChannelType, s8* pszBuf, u8 byBufLen )
{
	if( NULL == pszBuf )
	{
		return NULL;
	}
	switch(byChannelType) 
	{
	//��Ƶ
	case MEDIA_TYPE_PCMA:	  strncpy( pszBuf, CHANNAME_G711A,    byBufLen );	break;	
	case MEDIA_TYPE_PCMU:	  strncpy( pszBuf, CHANNAME_G711U,    byBufLen );	break;
	case MEDIA_TYPE_G722:	  strncpy( pszBuf, CHANNAME_G722,     byBufLen );	break;
	case MEDIA_TYPE_G7231:	  strncpy( pszBuf, CHANNAME_G7231,    byBufLen );	break;
	case MEDIA_TYPE_G728:	  strncpy( pszBuf, CHANNAME_G728,     byBufLen );	break;
	case MEDIA_TYPE_G729:	  strncpy( pszBuf, CHANNAME_G729,     byBufLen );	break;
	case MEDIA_TYPE_MP3:	  strncpy( pszBuf, CHANNAME_MP3,      byBufLen );	break;
	case MEDIA_TYPE_G7221C:	  strncpy( pszBuf, CHANNAME_G7221C,   byBufLen );	break;
    case MEDIA_TYPE_AACLC:	  strncpy( pszBuf, CHANNAME_AACLC,    byBufLen );	break;
    case MEDIA_TYPE_AACLD:	  strncpy( pszBuf, CHANNAME_AACLD,    byBufLen );	break;
    case MEDIA_TYPE_G719:	  strncpy( pszBuf, CHANNAME_G719,	  byBufLen );	break;
	//��Ƶ
	case MEDIA_TYPE_H261:	  strncpy( pszBuf, CHANNAME_H261,     byBufLen );	break;
	case MEDIA_TYPE_H262:	  strncpy( pszBuf, CHANNAME_H262,     byBufLen );	break;
	case MEDIA_TYPE_H263:	  strncpy( pszBuf, CHANNAME_H263,     byBufLen );	break;
	case MEDIA_TYPE_H263PLUS: strncpy( pszBuf, CHANNAME_H263PLUS, byBufLen );	break;
	case MEDIA_TYPE_H264:	  strncpy( pszBuf, CHANNAME_H264,     byBufLen );	break;
	case MEDIA_TYPE_MP4:	  strncpy( pszBuf, CHANNAME_MPEG4,    byBufLen );	break;
	case MEDIA_TYPE_H239:	  strncpy( pszBuf, CHANNAME_H239,     byBufLen );	break;
	//����
	case MEDIA_TYPE_H224:	  strncpy( pszBuf, CHANNAME_H224,     byBufLen );	break;
	case MEDIA_TYPE_T120:	  strncpy( pszBuf, CHANNAME_T120,     byBufLen );	break;
	case MEDIA_TYPE_MMCU:	  strncpy( pszBuf, CHANNAME_MMCU,     byBufLen );   break;
	
	default:                  strncpy( pszBuf, "UnknownType",     byBufLen );   break;
	}
	pszBuf[byBufLen - 1] = 0;
	
	return  pszBuf;
}


/*=============================================================================
  �� �� ���� GetVendorId
  ��    �ܣ� �����ڲ�ת��Ϊ�ⲿ�ṹ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byVendorId
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang                  ����
=============================================================================*/
u8 CMtAdpUtils::GetVendorId(u8 byVendorId)
{	
	switch(byVendorId) 
	{
	case vendor_MICROSOFT_NETMEETING:   return	MT_MANU_NETMEETING;
	case vendor_KEDA:			return	MT_MANU_KDC;
	case vendor_HUAWEI:         return	MT_MANU_HUAWEI;
	case vendor_POLYCOM:        return	MT_MANU_POLYCOM;
	case vendor_POLYCOM_MCU:    return	MT_MANU_POLYCOM;  
	case vendor_TANDBERG:       return	MT_MANU_TAIDE;
	case vendor_RADVISION:      return	MT_MANU_RADVISION;
	case vendor_VCON:           return	MT_MANU_VCON;
	case vendor_SONY:           return	MT_MANU_SONY;
	case vendor_VTEL:           return	MT_MANU_VTEL;        
    case vendor_ZTE:            return  MT_MANU_ZTE;
    case vendor_ZTE_MCU:        return  MT_MANU_ZTEMCU;    
    case vendor_CODIAN:         return  MT_MANU_CODIAN;
	case vendor_AETHRA:			return  MT_MANU_AETHRA;
	case vendor_CHAORAN:        return  MT_MANU_CHAORAN;
	default:                    return	MT_MANU_OTHER;
	}
}

/*=============================================================================
  �� �� ���� ResIn2Out
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byInRes
  �� �� ֵ�� emResolution 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang              ����
  2005/11/11    4.0			����                ������Ӧ�ֱ���
  2007/11/30    4.0R5		����                ������Ӧ�ֱ���
=============================================================================*/
s32 CMtAdpUtils::ResIn2Out(u8 byInRes)
{
	switch(byInRes)
	{
	case VIDEO_FORMAT_SQCIF:	return (s32)emSQCIF;
	case VIDEO_FORMAT_QCIF:		return (s32)emQCIF;
	case VIDEO_FORMAT_CIF:		return (s32)emCIF;
	case VIDEO_FORMAT_2CIF:		return (s32)emCIF2;
	case VIDEO_FORMAT_4CIF:		return (s32)emCIF4;
	case VIDEO_FORMAT_16CIF:	return (s32)emCIF16;
	case VIDEO_FORMAT_AUTO:		return (s32)emAUTO;
		
	case VIDEO_FORMAT_SIF:		return (s32)em352240;
	case VIDEO_FORMAT_4SIF:		return (s32)em704480;
	case VIDEO_FORMAT_VGA:		return (s32)em640480;
	case VIDEO_FORMAT_SVGA:		return (s32)em800600;
	case VIDEO_FORMAT_XGA:		return (s32)em1024768;

    case VIDEO_FORMAT_W4CIF:    return (s32)emW4CIF;
    case VIDEO_FORMAT_HD720:    return (s32)emHD720;
    case VIDEO_FORMAT_SXGA:     return (s32)emSXGA;
    case VIDEO_FORMAT_UXGA:     return (s32)emUXGA;
    case VIDEO_FORMAT_HD1080:  return (s32)emHD1080;

	default:					return (s32)emCIF;
	}
}

/*=============================================================================
  �� �� ���� FrameRateIn2Out
  ��    �ܣ� ֡��ת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byFrameRate
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/27    4.0			�ű���                  ����
  2007/12/01    4.0R5       ����                  ע�ⲻ֧��֡���Զ���
=============================================================================*/
u8 CMtAdpUtils::FrameRateIn2Out( u8 byFrameRate )
{
	switch( byFrameRate )
	{
	case VIDEO_FPS_2997_1:		return	1;						
	case VIDEO_FPS_25:			return	1;
	case VIDEO_FPS_2997_2:		return	2;
	case VIDEO_FPS_2997_3:		return	3;
	case VIDEO_FPS_2997_4:		return	4;
	case VIDEO_FPS_2997_5:		return	5;
	case VIDEO_FPS_2997_6:		return	6;
	case VIDEO_FPS_2997_7P5:    return  7;
    case VIDEO_FPS_2997_10:     return  10;
    case VIDEO_FPS_2997_15:     return  15;
	case VIDEO_FPS_2997_30:		return	30;
	default:					return  1;
	}
//    return 1;
}

u8 CMtAdpUtils::FrameRateOut2In( u8 byFrameRate )
{
	switch( byFrameRate )
	{
		case 1:		return	VIDEO_FPS_2997_1;		
		case 2:		return	VIDEO_FPS_2997_2;
		case 3:		return	VIDEO_FPS_2997_3;
		case 4:		return	VIDEO_FPS_2997_4;
		case 5:		return	VIDEO_FPS_2997_5;
		case 6:		return	VIDEO_FPS_2997_6;
		case 7:		return  VIDEO_FPS_2997_7P5;
		case 10:    return  VIDEO_FPS_2997_10;
		case 15:    return  VIDEO_FPS_2997_15;
		case 30:	return	VIDEO_FPS_2997_30;
		default:	return  byFrameRate;
	}
//    return 1;
}

/*=============================================================================
  �� �� ���� ResOut2In
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� emResolution emRes
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang              ����
  2005/11/11    4.0			����                ������Ӧ�ֱ���
  2007/11/30    4.0R5		����                ������Ӧ�ֱ���
=============================================================================*/
u8 CMtAdpUtils::ResOut2In(s32 emRes)
{
	switch(emRes)
	{
	case emSQCIF:   return VIDEO_FORMAT_SQCIF;
	case emQCIF:    return VIDEO_FORMAT_QCIF;
	case emCIF:     return VIDEO_FORMAT_CIF;
	case emCIF2:    return VIDEO_FORMAT_2CIF;
	case emCIF4:    return VIDEO_FORMAT_4CIF;
	case emCIF16:   return VIDEO_FORMAT_16CIF;
	case emAUTO:    return VIDEO_FORMAT_AUTO;

	case em352240:  return VIDEO_FORMAT_SIF;
	case em704480:  return VIDEO_FORMAT_4SIF;
	case em640480:  return VIDEO_FORMAT_VGA;
	case em800600:  return VIDEO_FORMAT_SVGA;
	case em1024768: return VIDEO_FORMAT_XGA;

    case emW4CIF:   return VIDEO_FORMAT_W4CIF;
    case emHD720:   return VIDEO_FORMAT_HD720;
    case emSXGA:    return VIDEO_FORMAT_SXGA;
    case emUXGA:    return VIDEO_FORMAT_UXGA;
    case emHD1080:  return VIDEO_FORMAT_HD1080;

	default:        return VIDEO_FORMAT_CIF;
	}
}


/*=============================================================================
  �� �� ���� TTERSTATUSVer36ToVer40
  ��    �ܣ� �ն˽ṹת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  _TTERSTATUS &tTerStatus
             _TTERSTATUS_VER36 &tTerStatus
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/04    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::TERSTATUSVer36ToVer40( _TTERSTATUS &tTerStatus, _TTERSTATUS_VER36 &tTerStatus36 )
{
	//tTerStatus.SetPollStat( tTerStatus36.byPollStat );
    // guzh [3/7/2007] 3.6 ��֧��VideoLose״̬�ϱ�������
    tTerStatus.SetVideoLose( FALSE );
	tTerStatus.SetBoardType( tTerStatus36.byMtBoardType );
	tTerStatus.SetMatrixStatus( tTerStatus36.bMatrixStatus );
	tTerStatus.SetVideoSrc( tTerStatus36.byVideoSrc );
	tTerStatus.SetRemoteCtrlEnalbe( tTerStatus36.bRemoteCtrl );
	tTerStatus.SetInputVolume( tTerStatus36.byEncVol );
	tTerStatus.SetOutputVolume( tTerStatus36.byDecVol );
	tTerStatus.SetMute( tTerStatus36.bMute );
	tTerStatus.SetQuiet( tTerStatus36.bQuiet );
	tTerStatus.SetExVidSrcNum( 0 );		// 0 ��ʾû���ⲿ��ƵԴ
	
	return;
}
*/

/*=============================================================================
  �� �� ���� TTERSTATUSVer40ToVer36
  ��    �ܣ� �ն˽ṹת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  _TTERSTATUS_VER36 &tTerStatus
             _TTERSTATUS &tTerStatus
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/04    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::TERSTATUSVer40ToVer36( _TTERSTATUS_VER36 &tTerStatus36, _TTERSTATUS &tTerStatus )
{
    // guzh [3/7/2007] 4.0���ټ�¼ pollstat
	// tTerStatus36.byPollStat = tTerStatus.GetPollStat();
    tTerStatus36.byPollStat = poll_none;
	tTerStatus36.byMtBoardType = tTerStatus.GetBoardType();
	tTerStatus36.bMatrixStatus = tTerStatus.GetMatrixStatus();
	tTerStatus36.byVideoSrc = tTerStatus.GetVideoSrc();
	tTerStatus36.bMute = tTerStatus.IsMute();
	tTerStatus36.bQuiet = tTerStatus.IsQuiet();
	tTerStatus36.byEncVol = tTerStatus.GetInputVolume();
	tTerStatus36.byDecVol = tTerStatus.GetOutputVolume();

	return;
}
*/
/*=============================================================================
  �� �� ���� ConfInfoVer36ToVer40V4R6
  ��    �ܣ� ������Ϣ�ṹ��3.6ת��4.6
  �㷨ʵ�֣� 3.6->4.0->4.5->4.6
  ȫ�ֱ����� 
  ��    ����TConfInfo36 &tConfInfo36: 3.6 ������Ϣ�ṹ
  �� �� ֵ��TConfInfo   &tConfInfo:   4.0 ������Ϣ�ṹ
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/12/07    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::ConfInfoV36ToV4R6( TConfInfo &tConfInfo, TConfInfoVer36 &tConfInfoVer36 )
{
    TConfInfoV4R4B2 tConfInfoV4R4B2;
    ConfInfoVer36ToVer40V4R4B2(tConfInfoV4R4B2, tConfInfoVer36);	//3.6->4.0

	ConfInfoV4R4ToV4R6(tConfInfo, tConfInfoV4R4B2);					//4.0->4.6

// 	TConfInfoV4R5	tConfInfoV4R5;
//     ConfInfoVer40V4R4B2ToVer40V4R5(tConfInfoV4R5, tConfInfoV4R4B2);
// 	
// 	ConfInfoV4R5ToV4R6(tConfInfo, tConfInfoV4R5); 
    return;
}
*/
/*=============================================================================
  �� �� ���� ConfInfoVer40DailyToVer36
  ��    �ܣ� ������Ϣ�ṹ��4.0ת��3.6
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����TConfInfo36 &tConfInfo36: 3.6 ������Ϣ�ṹ
  �� �� ֵ��TConfInfo   &tConfInfo:   4.0 ������Ϣ�ṹ
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/12/07    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::ConfInfoV4R6ToV36( TConfInfoVer36 &tConfInfoVer36, TConfInfo &tConfInfo )
{
    TConfInfoV4R4B2 tConfInfoV4R4B2;
	ConfInfoV4R6ToV4R4(tConfInfoV4R4B2, tConfInfo); //4.6->4.0
    ConfInfoVer40V4R4B2ToVer36(tConfInfoVer36, tConfInfoV4R4B2);//4.0->3.6
    return;
}
*/
/*=============================================================================
  �� �� ���� ConfInfoV4R6ToV4R4
  ��    �ܣ� ������Ϣ�ṹ��4.6ת��4.0
  �㷨ʵ�֣� 4.6->4.5->4.0
  ȫ�ֱ����� 
  ��    ����TConfInfo36 &tConfInfo36: 4.6 ������Ϣ�ṹ
  �� �� ֵ��TConfInfo   &tConfInfo:   4.0 ������Ϣ�ṹ
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/12/07    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::ConfInfoV4R6ToV4R4( TConfInfoV4R4B2 &tConfInfoV4R4B2, TConfInfo &tConfInfo )
{
   
	TConfInfoV4R5	tConfInfoV4R5;
	ConfInfoV4R6ToV4R5(tConfInfoV4R5, tConfInfo);	//4.6->4.5
    ConfInfoVer40V4R5ToVer40V4R4B2(tConfInfoV4R4B2, tConfInfoV4R5);//4.5->4.0
    return;
}
*/
/*=============================================================================
  �� �� ���� ConfInfoV4R6ToV4R4
  ��    �ܣ� ������Ϣ�ṹ��4.0ת��4.6
  �㷨ʵ�֣� 4.0->4.5->4.6
  ȫ�ֱ����� 
  ��    ����TConfInfo36 &tConfInfo36: 4.6 ������Ϣ�ṹ
  �� �� ֵ��TConfInfo   &tConfInfo:   4.0 ������Ϣ�ṹ
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/12/07    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::ConfInfoV4R4ToV4R6( TConfInfo &tConfInfo, TConfInfoV4R4B2 &tConfInfoV4R4B2)
{
   
	TConfInfoV4R5	tConfInfoV4R5;
	ConfInfoVer40V4R4B2ToVer40V4R5(tConfInfoV4R5, tConfInfoV4R4B2);	//4.0->4.5
    ConfInfoV4R5ToV4R6(tConfInfo, tConfInfoV4R5);				//4.5->4.6
    return;
}
*/
/*=============================================================================
  �� �� ���� ConfInfoVer36ToVer40V4R4B2
  ��    �ܣ� ������Ϣ�ṹ��3.6ת��4.0V4R4B2
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����TConfInfo36   &tConfInfo36: 3.6 ������Ϣ�ṹ
  �� �� ֵ��TConfInfoV4R4B2 &tConfInfo: 4.0 ������Ϣ�ṹ
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/31    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::ConfInfoVer36ToVer40V4R4B2( TConfInfoV4R4B2 &tConfInfo, TConfInfoVer36 &tConfInfo36 )
{
	tConfInfo.SetDcsBitRate( 0 );
	tConfInfo.SetBitRate( tConfInfo36.GetBitRate() );
	tConfInfo.SetConfPwd( tConfInfo36.GetConfPwd() );
	tConfInfo.SetDuration( tConfInfo36.GetDuration() );
	tConfInfo.SetConfName( tConfInfo36.GetConfName() );
	tConfInfo.SetConfE164( tConfInfo36.GetConfE164() );
	tConfInfo.SetSecBitRate( tConfInfo36.GetSecBitRate() );
	tConfInfo.SetTalkHoldTime( tConfInfo36.GetTalkHoldTime() );
	
	//�����
	CConfIdVer36 cConfId36;
	memset( &cConfId36, 0, sizeof(cConfId36) );
	cConfId36 = tConfInfo36.GetConfId();

	CConfId cConfId;
	memset( &cConfId, 0, sizeof(cConfId) );
	ConfIdVer36ToVer40( cConfId, cConfId36 );
	tConfInfo.SetConfId( cConfId );

	//ʱ��ṹ
	TKdvTimeVer36 tKdvTime36;
	memset( &tKdvTime36, 0, sizeof(tKdvTime36) );
	tKdvTime36 = tConfInfo36.GetKdvStartTime();

	TKdvTime tKdvTime;
	memset( &tKdvTime, 0, sizeof(tKdvTime) );
	KdvTimeVer36ToVer40( tKdvTime, tKdvTime36 );
	tConfInfo.SetKdvStartTime( tKdvTime );

	//������
	TCapSupportVer36 tCapSupport36;
	tCapSupport36 = tConfInfo36.GetCapSupport();

	TCapSupport tCapSupport;
	CapSupportVer36ToVer40( tCapSupport, tCapSupport36 );
	tConfInfo.SetCapSupport( tCapSupport );

	//��ϯ
	TMtAliasVer36 tChairAlias36;
	memset( &tChairAlias36, 0, sizeof(tChairAlias36) );
	tChairAlias36 = tConfInfo36.GetChairAlias();

	TMtAlias tChairAlias;
	memset( &tChairAlias, 0, sizeof(tChairAlias) );
	MtAliasVer36ToVer40( tChairAlias, tChairAlias36 );
	tConfInfo.SetChairAlias( tChairAlias );
	
	//������
	TMtAliasVer36 tSpeakerAlias36;
	memset( &tSpeakerAlias36, 0, sizeof(tSpeakerAlias36) );
	tSpeakerAlias36 = tConfInfo36.GetSpeakerAlias();
	
	TMtAlias tSpeakerAlias;
	memset( &tSpeakerAlias, 0, sizeof(tSpeakerAlias) );
	MtAliasVer36ToVer40( tSpeakerAlias, tSpeakerAlias36 );
	tConfInfo.SetSpeakerAlias( tSpeakerAlias );
	
	//��������
	TConfAttrbVer36 tConfAttrb36;
	memset( &tConfAttrb36, 0, sizeof(tConfAttrb36) );
	tConfAttrb36 = tConfInfo36.GetConfAttrb();

	TConfAttrb tConfAttrb;
	memset( &tConfAttrb, 0, sizeof(tConfAttrb) );
	ConfAttribVer36ToVer40( tConfAttrb, tConfAttrb36 ); 
	tConfInfo.SetConfAttrb( tConfAttrb );

	//ý������
	TMediaEncryptVer36 tMediaKey36;
	memset( &tMediaKey36, 0, sizeof(tMediaKey36) );
	tMediaKey36 = tConfInfo36.GetMediaKey();
	
	TMediaEncrypt tMediaKey;
	memset( &tMediaKey, 0, sizeof(tMediaKey) );
	MediaEncryptVer36ToVer40( tMediaKey, tMediaKey36 );
	tConfInfo.SetMediaKey( tMediaKey );

	//����״̬
	TConfStatusVer36 tConfStatus36;
	memset( &tConfStatus36, 0, sizeof(tConfStatus36) );
	tConfStatus36 = tConfInfo36.GetStatus();

	TConfStatusV4R4B2 tConfStatus;
	memset( &tConfStatus, 0, sizeof(tConfStatus) );
	ConfStatusVer36ToVer40V4R4B2( tConfStatus, tConfStatus36 );
	tConfInfo.SetStatus( tConfStatus );
	
	return;
}
*/
/*=============================================================================
  �� �� ���� ConfInfoVer40ToVer36
  ��    �ܣ� ������Ϣ�ṹ��4.0V4R4B2ת��3.6
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TConfInfoV4R4B2 &tConfInfo : 4.0 ������Ϣ�ṹ
  �� �� ֵ�� TConfInfoVer36 &tConfInfo36: 3.6 ������Ϣ�ṹ 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/31    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::ConfInfoVer40V4R4B2ToVer36( TConfInfoVer36 &tConfInfo36, TConfInfoV4R4B2 &tConfInfo )
{
	
	tConfInfo36.SetBitRate( tConfInfo.GetBitRate() );
	tConfInfo36.SetConfPwd( tConfInfo.GetConfPwd() );
	tConfInfo36.SetDuration( tConfInfo.GetDuration() );
	tConfInfo36.SetConfName( tConfInfo.GetConfName() );
	tConfInfo36.SetConfE164( tConfInfo.GetConfE164() );
	tConfInfo36.SetSecBitRate( tConfInfo.GetSecBitRate() );
	tConfInfo36.SetTalkHoldTime( tConfInfo.GetTalkHoldTime() );
	
	//�����
	CConfId cConfId;
	memset( &cConfId, 0, sizeof(cConfId) );
	cConfId = tConfInfo.GetConfId();

	CConfIdVer36 cConfId36;
	memset( &cConfId36, 0, sizeof(cConfId36) );
	ConfIdVer40ToVer36( cConfId36, cConfId );
	tConfInfo36.SetConfId( cConfId36 );
	
	//ʱ��ṹ
	TKdvTime tKdvTime;
	memset( &tKdvTime, 0, sizeof(tKdvTime) );
	tKdvTime = tConfInfo.GetKdvStartTime();
	
	TKdvTimeVer36 tKdvTime36;
	memset( &tKdvTime36, 0, sizeof(tKdvTime36) );
	KdvTimeVer40ToVer36( tKdvTime36, tKdvTime );
	tConfInfo36.SetKdvStartTime( tKdvTime36 );
	
	//������
	TCapSupport tCapSupport;
	tCapSupport = tConfInfo.GetCapSupport();

	TCapSupportVer36 tCapSupport36;
	CapSupportVer40ToVer36( tCapSupport36, tCapSupport );
	tConfInfo36.SetCapSupport( tCapSupport36 );
	
	//��ϯ
	TMtAlias tChairAlias;
	memset( &tChairAlias, 0, sizeof(tChairAlias) );
	tChairAlias = tConfInfo.GetChairAlias();	

	TMtAliasVer36 tChairAlias36;
	memset( &tChairAlias36, 0, sizeof(tChairAlias36) );
	MtAliasVer40ToVer36( tChairAlias36, tChairAlias );
	tConfInfo36.SetChairAlias( tChairAlias36 );
	
	//������
	TMtAlias tSpeakerAlias;
	memset( &tSpeakerAlias, 0, sizeof(tSpeakerAlias) );
	tSpeakerAlias = tConfInfo.GetSpeakerAlias();
	
	TMtAliasVer36 tSpeakerAlias36;
	memset( &tSpeakerAlias36, 0, sizeof(tSpeakerAlias36) );
	MtAliasVer40ToVer36( tSpeakerAlias36, tSpeakerAlias );
	tConfInfo36.SetSpeakerAlias( tSpeakerAlias36 );
	
	//��������	
	TConfAttrb tConfAttrb;
	memset( &tConfAttrb, 0, sizeof(tConfAttrb) );
	tConfAttrb = tConfInfo.GetConfAttrb();

	TConfAttrbVer36 tConfAttrb36;
	memset( &tConfAttrb36, 0, sizeof(tConfAttrb36) );
	ConfAttribVer40ToVer36( tConfAttrb36, tConfAttrb );
	tConfInfo36.SetConfAttrb( tConfAttrb36 );
	
	//ý������
	TMediaEncrypt tMediaKey;
	memset( &tMediaKey, 0, sizeof(tMediaKey) );
	tMediaKey = tConfInfo.GetMediaKey();
	
	TMediaEncryptVer36 tMediaKey36;
	memset( &tMediaKey36, 0, sizeof(tMediaKey36) );
	MediaEncryptVer40ToVer36( tMediaKey36, tMediaKey );
	tConfInfo36.SetMediaKey( tMediaKey36 );
	
	//����״̬
	TConfStatusV4R4B2 tConfStatus;
	memset( &tConfStatus, 0, sizeof(tConfStatus) );
	tConfStatus = tConfInfo.GetStatus();

	TConfStatusVer36 tConfStatus36;
	memset( &tConfStatus36, 0, sizeof(tConfStatus36) );
	ConfStatusVer40V4R4B2ToVer36( tConfStatus36, tConfStatus );
	tConfInfo36.SetStatus( tConfStatus36 );

	return;
}
*/
/*=============================================================================
  �� �� ���� ConfInfoVer40V4R5ToVer40V4R4B2
  ��    �ܣ� 4.5->4.0
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TConfInfo &tConfInfo :4.0 ������Ϣ�ṹ
  �� �� ֵ�� void
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/11/06    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::ConfInfoVer40V4R5ToVer40V4R4B2( TConfInfoV4R4B2 &tConfInfoV4R4B2, TConfInfoV4R5 &tConfInfo )
{
    memcpy( &tConfInfoV4R4B2, &tConfInfo, sizeof(TConfInfoV4R4B2));

    //����״̬
    TConfStatusV4R5 tConfStatus;
    memset( &tConfStatus, 0, sizeof(tConfStatus) );
    tConfStatus = tConfInfo.GetStatus();
    
    TConfStatusV4R4B2 tConfStatusV4R4B2;
    memset( &tConfStatusV4R4B2, 0, sizeof(tConfStatusV4R4B2) );
    ConfStatusV4R5ToV4R4B2( tConfStatusV4R4B2, tConfStatus );
    tConfInfoV4R4B2.SetStatus( tConfStatusV4R4B2 );
    return;
}
*/
/*=============================================================================
  �� �� ���� ConfInfoVer40V4R4B2ToVer40V4R5
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TConfInfo &tConfInfo :4.0 ������Ϣ�ṹ
  �� �� ֵ�� void
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/11/06    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::ConfInfoVer40V4R4B2ToVer40V4R5( TConfInfoV4R5 &tConfInfo, TConfInfoV4R4B2 &tConfInfoV4R4B2 )
{
    memcpy( &tConfInfo, &tConfInfoV4R4B2, sizeof(TConfInfoV4R5));
    
    //����״̬
    TConfStatusV4R4B2 tConfStatusV4R4B2;
    memset( &tConfStatusV4R4B2, 0, sizeof(tConfStatusV4R4B2) );
    tConfStatusV4R4B2 = tConfInfoV4R4B2.GetStatus();

    TConfStatusV4R5 tConfStatus;
    memset( &tConfStatus, 0, sizeof(tConfStatus) );  
    ConfStatusVer40V4R4B2ToVer40V4R5( tConfStatus, tConfStatusV4R4B2 );

    tConfInfo.SetStatus( tConfStatus );
    return;
}
*/
/*==============================================================================
������    :  VMPParamV4R6ToV4R5
����      :  
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-2-24					
==============================================================================*/
/*void CMtAdpUtils::VMPParamV4R6ToV4R5(TVMPParamV4R5 &tVMPParamV4R5,TVMPParam &tVMPParam)
{
	tVMPParamV4R5.SetVMPAuto( tVMPParam.IsVMPAuto() );
	tVMPParamV4R5.SetVMPBrdst( tVMPParam.IsVMPBrdst() );
	tVMPParamV4R5.SetVMPStyle( tVMPParam.GetVMPStyle() );
	tVMPParamV4R5.SetVMPSchemeId( tVMPParam.GetMaxMemberNum() );
	tVMPParamV4R5.SetVMPMode( tVMPParam.GetVMPMode() );
	tVMPParamV4R5.SetIsRimEnabled( tVMPParam.GetIsRimEnabled() );
	
	for( s32 nIndex = 0; nIndex < MAXNUM_VMP_MEMBER; nIndex ++ )
	{
		TVMPMember *ptVMPMember = tVMPParam.GetVmpMember(nIndex);
		if ( NULL != ptVMPMember ) 
		{
			TVMPMember tVmpMemberV4R5;
			memset( &tVmpMemberV4R5, 0, sizeof(tVmpMemberV4R5) );
			tVmpMemberV4R5 = *ptVMPMember;
			tVMPParamV4R5.SetVmpMember( nIndex, tVmpMemberV4R5 );
		}
	}
}
*/
/*==============================================================================
������    :  VMPParamV4R5ToV4R6
����      :  
�㷨ʵ��  :  
����˵��  :  
����ֵ˵��:  
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2009-2-24					
==============================================================================*/
/*void CMtAdpUtils::VMPParamV4R5ToV4R6(TVMPParam &tVMPParam, TVMPParamV4R5 &tVMPParamV4R5)
{
	tVMPParam.SetVMPAuto( tVMPParamV4R5.IsVMPAuto() );
	tVMPParam.SetVMPBrdst( tVMPParamV4R5.IsVMPBrdst() );
	tVMPParam.SetVMPStyle( tVMPParamV4R5.GetVMPStyle() );
	tVMPParam.SetVMPSchemeId( tVMPParamV4R5.GetMaxMemberNum() );
	tVMPParam.SetVMPMode( tVMPParamV4R5.GetVMPMode() );
	tVMPParam.SetIsRimEnabled( tVMPParamV4R5.GetIsRimEnabled() );
	tVMPParam.SetVMPBatchPoll( 0 );
	for( s32 nIndex = 0; nIndex < MAXNUM_VMP_MEMBER; nIndex ++ )
	{
		TVMPMember *ptVMPMemberV4R5 = tVMPParamV4R5.GetVmpMember(nIndex);
		if ( NULL != ptVMPMemberV4R5 ) 
		{
			TVMPMember tVmpMember;
			memset( &tVmpMember, 0, sizeof(tVmpMember) );
			tVmpMember = *ptVMPMemberV4R5;
			tVMPParam.SetVmpMember( nIndex, tVmpMember );
		}
	}
	for( u8 byIdx = MAXNUM_VMP_MEMBER; byIdx < MAXNUM_MPUSVMP_MEMBER; byIdx ++)
	{
		TVMPMember tVmpMember;
		memset( &tVmpMember, 0, sizeof(tVmpMember) );
		tVMPParam.SetVmpMember( byIdx, tVmpMember );
	}
	return;
}
*/
/*=============================================================================
�� �� ���� ConfStatusV4R5ToV4R6
��    �ܣ� ����״̬V4.5 ת�� ֮��
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TConfStatus &tConfStatus: 4.5����״̬�ṹ
�� �� ֵ�� void
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����

=============================================================================*/
/*void CMtAdpUtils::ConfStatusV4R5ToV4R6( TConfStatus &tConfStatus, TConfStatusV4R5 &tConfStatusV4R5 )
{
    memcpy(&tConfStatus, &tConfStatusV4R5, sizeof(tConfStatus));

	memcpy(&tConfStatus.m_tMixParam, &tConfStatusV4R5.m_tMixParam, sizeof(TMixParam));
	
	//����TVMPParam
	TVMPParamV4R5 tVMPParamV4R5, tVMPTwParamV4R5;
	tVMPParamV4R5	= tConfStatusV4R5.GetVmpParam();
	tVMPTwParamV4R5 = tConfStatusV4R5.GetVmpTwParam();
	
	TVMPParam	tVMPParam, tVMPTwParam;
	VMPParamV4R5ToV4R6(tVMPParam, tVMPParamV4R5);
	VMPParamV4R5ToV4R6(tVMPTwParam, tVMPTwParamV4R5);
	
	tConfStatus.SetVmpParam(tVMPParam);
	tConfStatus.SetVmpTwParam(tVMPTwParam);

	//����prs
	tConfStatus.SetPrsing(tConfStatusV4R5.IsPrsing());

    return;
}
*/
/*=============================================================================
�� �� ���� ConfStatusV4R6ToV4R5
��    �ܣ� ����״̬V4.6->V4.5, �޸�ʱע��ṹ���ӽṹ������
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TConfStatus &tConfStatus: 4.5����״̬�ṹ
�� �� ֵ�� void
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����

=============================================================================*/
/*void CMtAdpUtils::ConfStatusV4R6ToV4R5( TConfStatusV4R5 &tConfStatusV4R5, TConfStatus &tConfStatus )
{
    memcpy( &tConfStatusV4R5,&tConfStatus, sizeof(tConfStatusV4R5));
	memcpy(&tConfStatusV4R5.m_tMixParam, &tConfStatus.m_tMixParam, sizeof(TMixParam));
	
	//����TVMPParam (16������Ա)
	TVMPParam	tVMPParam, tVMPTwParam;
	tVMPParam	= tConfStatus.GetVmpParam();
	tVMPTwParam = tConfStatus.GetVmpTwParam();
	
	TVMPParamV4R5 tVMPParamV4R5, tVMPTwParamV4R5;
	VMPParamV4R6ToV4R5( tVMPParamV4R5, tVMPParam);
	VMPParamV4R6ToV4R5( tVMPTwParamV4R5, tVMPTwParam );
	
	tConfStatusV4R5.SetVmpParam(tVMPParamV4R5);
	tConfStatusV4R5.SetVmpTwParam(tVMPTwParamV4R5);

	//����prs
	tConfStatusV4R5.SetPrsing(tConfStatus.IsPrsing());

    return;
}
*/
/*=============================================================================
�� �� ���� ConfInfoV4R5ToV4R6
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TConfInfo &tConfInfo :4.6 ������Ϣ�ṹ
�� �� ֵ�� void
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����

=============================================================================*/
/*void CMtAdpUtils::ConfInfoV4R5ToV4R6( TConfInfo &tConfInfo, TConfInfoV4R5 &tConfInfoV4R5 )
{
    memcpy( &tConfInfo, &tConfInfoV4R5, sizeof(TConfInfo));
    
    //����״̬
    TConfStatusV4R5 tConfStatusV4R5;
    memset( &tConfStatusV4R5, 0, sizeof(tConfStatusV4R5) );
    tConfStatusV4R5 = tConfInfoV4R5.GetStatus();
	
    TConfStatus tConfStatus;
    memset( &tConfStatus, 0, sizeof(tConfStatus) );  
    ConfStatusV4R5ToV4R6( tConfStatus, tConfStatusV4R5 );
	
    tConfInfo.SetStatus( tConfStatus );
	
	TConfAttrbEx tConfAttrbEx = tConfInfoV4R5.GetConfAttrbEx();
	tConfInfo.SetConfAttrbEx(tConfAttrbEx);

    return;
}
*/
/*=============================================================================
�� �� ���� ConfInfoV4R6ToV4R5
��    �ܣ� ConfInfo �ṹת�� 4.6 -> 4.5 (��������4.6����ϳɳ�Ա������Ϊ20)
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� TConfInfo &tConfInfo :4.6 ������Ϣ�ṹ
�� �� ֵ�� void
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����

=============================================================================*/
/*void CMtAdpUtils::ConfInfoV4R6ToV4R5( TConfInfoV4R5 &tConfInfoV4R5, TConfInfo &tConfInfo )
{
    memcpy( &tConfInfoV4R5, &tConfInfo, sizeof(tConfInfoV4R5));
    
    //����״̬
	TConfStatus tConfStatus;
    memset( &tConfStatus, 0, sizeof(tConfStatus) ); 
	tConfStatus = tConfInfo.GetStatus();

    TConfStatusV4R5 tConfStatusV4R5;
    memset( &tConfStatusV4R5, 0, sizeof(tConfStatusV4R5) );
    ConfStatusV4R6ToV4R5( tConfStatusV4R5, tConfStatus );
	
    tConfInfoV4R5.SetStatus( tConfStatusV4R5 );
    return;
}
*/
/*=============================================================================
  �� �� ���� CapSupportVer36ToVer40
  ��    �ܣ� �������ṹ��3.6ת��4.0
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TCapSupport36 &tCapSupport36: 3.6 �������ṹ
  �� �� ֵ�� TCapSupport: 4.0�������ṹ 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/31    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::CapSupportVer36ToVer40( TCapSupport &tCapSupport, TCapSupportVer36 &tCapSupportVer36 )
{
	//��ͬʱ������
	TSimCapSet tMainSimCapSet;
	if ( !tCapSupportVer36.GetMainSimCapSet().IsNull() )
	{
		tMainSimCapSet.SetAudioMediaType( MediaTypeVer36ToVer40( tCapSupportVer36.GetMainSimCapSet().GetAudioMediaType() ) );
		tMainSimCapSet.SetVideoMediaType( MediaTypeVer36ToVer40( tCapSupportVer36.GetMainSimCapSet().GetVideoMediaType() ) );
		tCapSupport.SetMainSimCapSet( tMainSimCapSet );
	}
	else
	{
		tCapSupport.GetMainSimCapSet().Clear();
	}
	//��ͬʱ������
	TSimCapSet tSecSimCapSet;
	if ( !tCapSupportVer36.GetSecondSimCapSet().IsNull() )
	{
		tSecSimCapSet.SetAudioMediaType( MediaTypeVer36ToVer40( tCapSupportVer36.GetSecondSimCapSet().GetAudioMediaType() ) );
		tSecSimCapSet.SetVideoMediaType( MediaTypeVer36ToVer40( tCapSupportVer36.GetSecondSimCapSet().GetVideoMediaType() ) );
		tCapSupport.SetSecondSimCapSet( tSecSimCapSet );
	}
	else
	{
		tCapSupport.GetSecondSimCapSet().Clear();
	}
	//�ϲ�����
	tCapSupport.SetSupportMMcu( tCapSupportVer36.IsSupportMMcu() );
	
	//����
	tCapSupport.SetSupportH224( tCapSupportVer36.GetSupportH224() );
	tCapSupport.SetSupportT120( tCapSupportVer36.GetSupportT120() );
	
	//239˫��: 3.6�汾��GetSupportH239()��������, ����Ϊ:
	//(1) FALSE��ʾ��֧��; (2) !FALSE������ʾʹ��, ��ֵ����ʾ�����H239˫������
	if ( FALSE == tCapSupportVer36.GetSupportH239() )
	{
		tCapSupport.SetDStreamSupportH239( FALSE );
	}
	else
	{
		tCapSupport.SetDStreamSupportH239( TRUE );
		tCapSupport.SetDStreamMediaType( tCapSupportVer36.GetSupportH239() );
	}
	//263+˫��
	if ( tCapSupportVer36.GetSupportH263Plus() )
	{
		tCapSupport.SetDStreamMediaType( MEDIA_TYPE_H263PLUS );
	}
	//����: ȡ3.6�汾�еļ���ģʽ�е���ģʽ, ��ģʽĬ��Ϊ��, ���ﲻ���ж�
	tCapSupport.SetEncryptMode( tCapSupportVer36.GetMainEncryptMode() );

	return;
}

*/
/*=============================================================================
  �� �� ���� CapSupportVer40ToVer36
  ��    �ܣ� �������ṹ��4.0ת��3.6
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TCapSupport &tCapSupport: 4.0 �������ṹ
  �� �� ֵ�� TCapSupport36: 3.6�������ṹ 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/31    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::CapSupportVer40ToVer36( TCapSupportVer36 &tCapSupportVer36, TCapSupport &tCapSupport )
{
	//��ͬʱ������
	TSimCapSetVer36 tMainSimCapSet36;
	if ( !tCapSupport.GetMainSimCapSet().IsNull() ) 
	{
		tMainSimCapSet36.SetAudioMediaType( MediaTypeVer40ToVer36( tCapSupport.GetMainSimCapSet().GetAudioMediaType() ) );
		tMainSimCapSet36.SetVideoMediaType( MediaTypeVer40ToVer36( tCapSupport.GetMainSimCapSet().GetVideoMediaType() ) );
		tMainSimCapSet36.SetDataMediaType1( tCapSupport.IsSupportH224() ? MEDIA_TYPE_H224 : MEDIA_TYPE_NULL );
		tMainSimCapSet36.SetDataMediaType2( tCapSupport.IsSupportT120() ? MEDIA_TYPE_T120 : MEDIA_TYPE_NULL );
		tCapSupportVer36.SetMainSimCapSet( tMainSimCapSet36 );
	}
	else
	{
		tCapSupportVer36.GetMainSimCapSet().SetNull();
	}
	//����ͬʱ������
	TSimCapSetVer36 tSecSimCapSet36;
	if ( !tCapSupport.GetSecondSimCapSet().IsNull() )
	{
		tSecSimCapSet36.SetAudioMediaType( MediaTypeVer40ToVer36( tCapSupport.GetSecondSimCapSet().GetAudioMediaType() ) );
		tSecSimCapSet36.SetVideoMediaType( MediaTypeVer40ToVer36( tCapSupport.GetSecondSimCapSet().GetVideoMediaType() ) );
		tCapSupportVer36.SetSecondSimCapSet( tSecSimCapSet36 );
	}
	else
	{
		tCapSupportVer36.GetSecondSimCapSet().SetNull();
	}
	//�ϲ�����
	tCapSupportVer36.SetSupportMMcu( tCapSupport.IsSupportMMcu() );
	
	//����
	tCapSupportVer36.SetSupportH224( tCapSupport.IsSupportH224() );
	tCapSupportVer36.SetSupportT120( tCapSupport.IsSupportT120() );

	//239˫��: 3.6�汾��GetSupportH239()��������, ����Ϊ:
	//(1) FALSE��ʾ��֧��; (2) !FALSE������ʾʹ��, ��ֵ����ʾ�����H239˫������
	if ( FALSE == tCapSupport.IsDStreamSupportH239() ) 
	{
		tCapSupportVer36.SetSupportH239( FALSE );
	}
	else
	{
		tCapSupportVer36.SetSupportH239( tCapSupport.GetDStreamMediaType() );
	}

	//263+˫��
	if ( MEDIA_TYPE_H263PLUS == tCapSupport.GetDStreamMediaType() )
	{
		tCapSupportVer36.SetSupportH263Plus( TRUE );
	}
	//����: ȡ3.6�汾�еļ���ģʽ�е���ģʽ, ��ģʽĬ��Ϊ��, ���ﲻ���ж�
	tCapSupportVer36.SetMainEncryptMode( tCapSupport.GetEncryptMode() );
	tCapSupportVer36.SetSecondEncryptMode( NULL );
	
	return;
}
*/
/*=============================================================================
  �� �� ���� ConfAttribVer36ToVer40
  ��    �ܣ� ����������3.6��4.0ת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TConfAttrb36 &tConfAttrb36: 3.6�Ļ�������
  �� �� ֵ�� TConfAttrb: 4.0�Ļ������� 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/31    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::ConfAttribVer36ToVer40( TConfAttrb &tConfAttrb, TConfAttrbVer36 &tConfAttrbVer36 )
{
	tConfAttrb.SetOpenMode( tConfAttrbVer36.GetOpenMode() );
	tConfAttrb.SetEncryptMode( tConfAttrbVer36.GetEncryptMode() );
	tConfAttrb.SetMulticastMode( tConfAttrbVer36.IsMulticastMode() ? TRUE : FALSE );
	tConfAttrb.SetReleaseMode( tConfAttrbVer36.IsReleaseNoMt() ? TRUE : FALSE );
	tConfAttrb.SetQualityPri( tConfAttrbVer36.IsQualityPri() ? TRUE : FALSE );	
	tConfAttrb.SetDataMode( tConfAttrbVer36.GetDataMode() );
	tConfAttrb.SetSupportCascade( tConfAttrbVer36.IsSupportCascade() );
	tConfAttrb.SetPrsMode( tConfAttrbVer36.IsResendLosePack() );
	tConfAttrb.SetHasTvWallModule( tConfAttrbVer36.IsHasTvWallModule() );
	tConfAttrb.SetHasVmpModule( tConfAttrbVer36.IsHasVmpModule() );
	tConfAttrb.SetDiscussConf( tConfAttrbVer36.IsDiscussConf() );
	tConfAttrb.SetUseAdapter( tConfAttrbVer36.IsUseAdapter() );
	tConfAttrb.SetSpeakerSrc( tConfAttrbVer36.GetSpeakerSrc() );
	tConfAttrb.SetSpeakerSrcMode( tConfAttrbVer36.GetSpeakerSrcMode() );
	tConfAttrb.SetMulcastLowStream( tConfAttrbVer36.IsMulcastLowStream() );
	tConfAttrb.SetAllInitDumb( tConfAttrbVer36.IsAllInitDumb() );
	tConfAttrb.SetUniformMode( tConfAttrbVer36.IsAdjustUniformPack() );

	return;
}
*/
/*=============================================================================
  �� �� ���� ConfAttribVer40ToVer36
  ��    �ܣ� ����������4.0��3.6��ת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TConfAttrb &tConfAttrb: 4.0��������
  �� �� ֵ�� TConfAttrb36: 3.6�������� 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/31    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::ConfAttribVer40ToVer36( TConfAttrbVer36 &tConfAttrbVer36, TConfAttrb &tConfAttrb )
{
	tConfAttrbVer36.SetOpenMode( tConfAttrb.GetOpenMode() );
	tConfAttrbVer36.SetEncryptMode( tConfAttrb.GetEncryptMode() );
	tConfAttrbVer36.SetMulticastMode( tConfAttrb.IsMulticastMode() ? TRUE : FALSE );
	tConfAttrbVer36.SetReleaseMode( tConfAttrb.IsReleaseNoMt() ? TRUE : FALSE );
	tConfAttrbVer36.SetQualityPri( tConfAttrb.IsQualityPri() ? TRUE : FALSE );	
	tConfAttrbVer36.SetDataMode( tConfAttrb.GetDataMode() );
	tConfAttrbVer36.SetSupportCascade( tConfAttrb.IsSupportCascade() );
	tConfAttrbVer36.SetPrsMode( tConfAttrb.IsResendLosePack() );
	tConfAttrbVer36.SetHasTvWallModule( tConfAttrb.IsHasTvWallModule() );
	tConfAttrbVer36.SetHasVmpModule( tConfAttrb.IsHasVmpModule() );
	tConfAttrbVer36.SetDiscussConf( tConfAttrb.IsDiscussConf() );
	tConfAttrbVer36.SetUseAdapter( tConfAttrb.IsUseAdapter() );
	tConfAttrbVer36.SetSpeakerSrc( tConfAttrb.GetSpeakerSrc() );
	tConfAttrbVer36.SetSpeakerSrcMode( tConfAttrb.GetSpeakerSrcMode() );
	tConfAttrbVer36.SetMulcastLowStream( tConfAttrb.IsMulcastLowStream() );
	tConfAttrbVer36.SetAllInitDumb( tConfAttrb.IsAllInitDumb() );
	tConfAttrbVer36.SetUniformMode( tConfAttrb.IsAdjustUniformPack() );

	return;
}
*/
/*=============================================================================
  �� �� ���� MtAliasVer36ToVer40
  ��    �ܣ� �ն˱���3.6��4.0��ת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TMtAlias36 &tMtAlias36: 3.6�ն˱���
  �� �� ֵ�� TMtAlias: 4.0�ն˱��� 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::MtAliasVer36ToVer40( TMtAlias &tMtAlias, TMtAliasVer36 &tMtAliasVer36 )
{
	strncpy( tMtAlias.m_achAlias, tMtAliasVer36.m_achAlias, sizeof(tMtAliasVer36.m_achAlias) );
	tMtAlias.m_AliasType				 = tMtAliasVer36.m_AliasType;
	tMtAlias.m_tTransportAddr.m_dwIpAddr = tMtAliasVer36.m_tTransportAddr.m_dwIpAddr;
	tMtAlias.m_tTransportAddr.m_wPort    = tMtAliasVer36.m_tTransportAddr.m_wPort;

	return;
}
*/
/*=============================================================================
  �� �� ���� MtAliasVer40ToVer36
  ��    �ܣ� �ն˱���4.0��3.6��ת��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TMtAlias &tMtAlias����:4.0�ն˱���
  �� �� ֵ�� TMtAlias36: 3.6�ն˱��� 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::MtAliasVer40ToVer36( TMtAliasVer36 &tMtAliasVer36, TMtAlias &tMtAlias )
{
	strncpy( tMtAliasVer36.m_achAlias, tMtAlias.m_achAlias, sizeof(tMtAlias.m_achAlias) );
	tMtAliasVer36.m_AliasType				  = tMtAlias.m_AliasType;
	tMtAliasVer36.m_tTransportAddr.m_dwIpAddr = tMtAlias.m_tTransportAddr.m_dwIpAddr;
	tMtAliasVer36.m_tTransportAddr.m_wPort    = tMtAlias.m_tTransportAddr.m_wPort;

	return;
}
*/
/*=============================================================================
  �� �� ���� MediaEncryptVer36ToVer40
  ��    �ܣ� ý��������Կ3.6ת��4.0
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����TMediaEncrypt36 &tEncryptKey36: 3.6ý��������Կ
  �� �� ֵ��TMediaEncrypt: 4.0ý��������Կ 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/31    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::MediaEncryptVer36ToVer40( TMediaEncrypt &tMediaKey, TMediaEncryptVer36 &tEncryptKeyVer36 )
{
	s32 nKeyLen = 0;
	u8  abyKey[MAXLEN_KEY] = {0};
	tEncryptKeyVer36.GetEncryptKey( abyKey, &nKeyLen );
	tMediaKey.SetEncryptKey( abyKey, nKeyLen );
	tMediaKey.SetEncryptMode( tEncryptKeyVer36.GetEncryptMode() );

	return;
}
*/
/*=============================================================================
  �� �� ���� MediaEncryptVer40ToVer36
  ��    �ܣ� ý��������Կ4.0ת��3.6
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TMediaEncrypt &tEncryptKey: 4.0ý��������Կ
  �� �� ֵ�� TMediaEncrypt36: 3.6ý��������Կ  
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/31    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::MediaEncryptVer40ToVer36( TMediaEncryptVer36 &tEncryptKeyVer36, TMediaEncrypt &tEncryptKey )
{
	s32 nKeyLen = 0;
	u8  abyKey[MAXLEN_KEY] = {0};
	tEncryptKey.GetEncryptKey( abyKey, &nKeyLen );	
	tEncryptKeyVer36.SetEncryptKey( abyKey, nKeyLen );
	tEncryptKeyVer36.SetEncryptMode( tEncryptKey.GetEncryptMode() );
	return;
}
*/
/*=============================================================================
  �� �� ���� ConfStatusVer36ToVer40V4R4B2
  ��    �ܣ� ����״̬3.6ת��4.0
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TConfStatus36 &tConfStatus36: 3.6����״̬�ṹ
  �� �� ֵ�� TConfStatus: 4.0����״̬�ṹ
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::ConfStatusVer36ToVer40V4R4B2( TConfStatusV4R4B2 &tConfStatus, TConfStatusVer36 &tConfStatusVer36 )
{
	//��ϯ
	TMtVer36 tChairManVer36;
	memset( &tChairManVer36, 0, sizeof(tChairManVer36) );
	tChairManVer36 = tConfStatusVer36.GetChairman();

	TMt tChairMan;
	memset( &tChairMan, 0, sizeof(tChairMan) );
	MtVer36ToVer40( tChairMan, tChairManVer36 );
	tConfStatus.SetChairman( tChairMan );
	
	//������	
	TMtVer36 tSpeakerVer36;
	memset( &tSpeakerVer36, 0, sizeof(tSpeakerVer36) );
	tSpeakerVer36 = tConfStatusVer36.GetSpeaker();

	TMt tSpeaker;
	memset( &tSpeaker, 0, sizeof(tSpeaker) );
	MtVer36ToVer40( tSpeaker, tSpeakerVer36 );
	tConfStatus.SetSpeaker( tSpeaker );
	
	//¼�����
	TRecProgVer36 tRecProgVer36;
	memset( &tRecProgVer36, 0, sizeof(tRecProgVer36) );
	tRecProgVer36 = tConfStatusVer36.GetRecProg();

	TRecProg tRecProg;
	memset( &tRecProg, 0, sizeof(tRecProg) );
	RecProgVer36ToVer40( tRecProg, tRecProgVer36 );
	tConfStatus.SetRecProg( tRecProg );
	
	//�������	
	TRecProgVer36 tPlayProgVer36;
	memset( &tPlayProgVer36, 0, sizeof(tPlayProgVer36) );
	tPlayProgVer36 = tConfStatusVer36.GetPlayProg();

	TRecProg tPlayProg;
	memset( &tPlayProg, 0, sizeof(tPlayProg) );
	RecProgVer36ToVer40( tPlayProg, tPlayProgVer36 );
	tConfStatus.SetPlayProg( tPlayProg );
	
	//��ѯ��Ϣ
	TPollInfoVer36 tPollInfoVer36;
	memset( &tPollInfoVer36, 0, sizeof(tPollInfoVer36) );
	tPollInfoVer36 = *tConfStatusVer36.GetPollInfo();

	TPollInfo tPollInfo;
	memset( &tPollInfo, 0, sizeof(tPollInfo) );
	PollInfoVer36ToVer40( tPollInfo, tPollInfoVer36 );
	tConfStatus.SetPollInfo( tPollInfo );
	
	//������Ϣ
	TDiscussParamVer36 tDiscussParamVer36;
	memset( &tDiscussParamVer36, 0, sizeof(tDiscussParamVer36) );
	tDiscussParamVer36 = tConfStatusVer36.GetDiscussParam();

	TDiscussParamV4R4B2 tDiscussParam;
	memset( &tDiscussParam, 0, sizeof(tDiscussParam) );
	DiscussParamVer36ToVer40V4R4B2( tDiscussParam, tDiscussParamVer36 );
	tConfStatus.SetDiscussParam( tDiscussParam );

	//����ģʽ
	TConfModeVer36 tConfModeVer36;
	memset( &tConfModeVer36, 0, sizeof(tConfModeVer36) );
	tConfModeVer36 = tConfStatusVer36.GetConfMode();
	
	TConfModeV4R4B2 tConfMode;
	memset( &tConfMode, 0, sizeof(tConfMode) );
	ConfModeVer36ToVer40V4R4B2( tConfMode, tConfModeVer36 );
	tConfStatus.SetConfMode( tConfMode );
	
	//����ϳɲ���: ��3.6 ConfMode��VmpModeд��4.0��VmpParam��
	TVMPParamVer36 tVMPParamVer36;
	memset( &tVMPParamVer36, 0, sizeof(tVMPParamVer36) );
	tVMPParamVer36 = tConfStatusVer36.GetVmpParam();
	
	TVMPParamV4R4B2 tVMPParam;	// xliang [2/24/2009] 
	memset( &tVMPParam, 0, sizeof(tVMPParam) );
	VMPParamVer36ToVer40( tVMPParam, tVMPParamVer36 );
	tVMPParam.SetVMPMode( tConfModeVer36.GetVMPMode() );
	tConfStatus.SetVmpParam( tVMPParam );
	
	//�Ƿ����ܶ����ָ�
	tConfStatus.SetPrsing( tConfStatusVer36.IsPrsing() );

	//3.6û�е���ǽ��ѯ���, ����ת4.0��ʱ�����ֵ
	TVMPParamV4R4B2 tTwVmpParam;
	memset( &tTwVmpParam, 0, sizeof(tTwVmpParam) );
	tConfStatus.SetVmpTwParam( tTwVmpParam );

	TTvWallPollInfoV4R4B2 tTvWallPollInfo;
	memset( &tTvWallPollInfo, 0, sizeof(tTvWallPollInfo) );
	tConfStatus.SetTvWallPollInfo( tTvWallPollInfo );

	return;
}
*/
/*=============================================================================
  �� �� ���� ConfStatusVer40V4R4B2ToVer36
  ��    �ܣ� ����״̬4.0ת��3.6
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TConfStatus &tConfStatus: 4.0����״̬�ṹ
  �� �� ֵ�� TConfStatus36: 3.6����״̬�ṹ
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::ConfStatusVer40V4R4B2ToVer36( TConfStatusVer36 &tConfStatusVer36, TConfStatusV4R4B2 &tConfStatus )
{
	//��ϯ
	TMt tChairMan;
	memset( &tChairMan, 0, sizeof(tChairMan) );
	tChairMan = tConfStatus.GetChairman();

	TMtVer36 tChairManVer36;
	memset( &tChairManVer36, 0, sizeof(tChairManVer36) );
	MtVer40ToVer36( tChairManVer36, tChairMan );
	tConfStatusVer36.SetChairman( tChairManVer36 );

	//������
	TMt tSpeaker;
	memset( &tSpeaker, 0, sizeof(tSpeaker) );
	tSpeaker = tConfStatus.GetSpeaker();

	TMtVer36 tSpeakerVer36;
	memset( &tSpeakerVer36, 0, sizeof(tSpeakerVer36) );
	MtVer40ToVer36( tSpeakerVer36, tSpeaker );
	tConfStatusVer36.SetSpeaker( tSpeakerVer36 );
	
	//¼�����
	TRecProg tRecProg;
	memset( &tRecProg, 0, sizeof(tRecProg) );
	tRecProg = tConfStatus.GetRecProg();
	
	TRecProgVer36 tRecProgVer36;
	memset( &tRecProgVer36, 0, sizeof(tRecProgVer36) );
	RecProgVer40ToVer36( tRecProgVer36, tRecProg );
	tConfStatusVer36.SetRecProg( tRecProgVer36 );

	//�������
	TRecProg tPlayProg;
	memset( &tPlayProg, 0, sizeof(tPlayProg) );
	tPlayProg = tConfStatus.GetPlayProg();

	TRecProgVer36 tPlayProgVer36;
	memset( &tPlayProgVer36, 0, sizeof(tPlayProgVer36) );
	RecProgVer40ToVer36( tPlayProgVer36, tPlayProg );
	tConfStatusVer36.SetPlayProg( tPlayProgVer36 );

	//��ѯ��Ϣ
	TPollInfo tPollInfo;
	memset( &tPollInfo, 0, sizeof(tPollInfo) );
	tPollInfo = *tConfStatus.GetPollInfo();

	TPollInfoVer36 tPollInfoVer36;
	memset( &tPollInfoVer36, 0, sizeof(tPollInfoVer36) );
	PollInfoVer40ToVer36( tPollInfoVer36, tPollInfo );
	tConfStatusVer36.SetPollInfo( tPollInfoVer36 );

	//������Ϣ
	TDiscussParamV4R4B2 tDiscussParam;
	memset( &tDiscussParam, 0, sizeof(tDiscussParam) );
	tDiscussParam = tConfStatus.GetDiscussParam();
    
	TDiscussParamVer36 tDiscussParamVer36;
	memset( &tDiscussParamVer36, 0, sizeof(tDiscussParamVer36) );
	DiscussParamVer40V4R4B2ToVer36( tDiscussParamVer36, tDiscussParam );
	tConfStatusVer36.SetDiscussParam( tDiscussParamVer36 );

	//����ϳɲ���
	TVMPParamV4R4B2 tVMPParam;
	memset( &tVMPParam, 0, sizeof(tVMPParam) );
	tVMPParam = tConfStatus.GetVmpParam();
	
	TVMPParamVer36 tVMPParamVer36;
	memset( &tVMPParamVer36, 0, sizeof(tVMPParamVer36) );
	VMPParamVer40ToVer36( tVMPParamVer36, tVMPParam );
	tConfStatusVer36.SetVmpParam( tVMPParamVer36 );
	
	//�Ƿ����ܶ����ָ�
	tConfStatusVer36.SetPrsing( tConfStatus.IsPrsing() );

	//����ģʽ: ��4.0 VmpParam��VmpModeд��3.6��ConfMode��
	TConfModeV4R4B2 tConfMode;
	memset( &tConfMode, 0, sizeof(tConfMode) );
	tConfMode = tConfStatus.GetConfMode();

	TConfModeVer36 tConfModeVer36;
	memset( &tConfModeVer36, 0, sizeof(tConfModeVer36) );
	ConfModeVer40V4R4B2ToVer36( tConfModeVer36, tConfMode );
	tConfModeVer36.SetVMPMode( tVMPParam.GetVMPMode() );
	tConfStatusVer36.SetConfMode( tConfModeVer36 );

	return;
}
*/
/*=============================================================================
  �� �� ���� ConfStatusVer40V4R4B2ToVer40Daily
  ��    �ܣ� ����״̬4.0V4R4B2 ת�� ֮��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TConfStatus &tConfStatus: 4.0����״̬�ṹ
  �� �� ֵ�� void
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/11/06    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::ConfStatusVer40V4R4B2ToVer40V4R5( TConfStatusV4R5 &tConfStatus, TConfStatusV4R4B2 &tConfStatusV4R4B2 )
{
    memcpy( &tConfStatus, &tConfStatusV4R4B2, sizeof(tConfStatus));

    //��������TDiscussParam��TConfMode
    TDiscussParamV4R4B2 tDisParam;
    tDisParam = tConfStatusV4R4B2.GetDiscussParam();

    TMixParamV4R5 tMixParam;
    DiscussParamVer40V4R4B2ToVer40Daily(tMixParam, tDisParam);
    tConfStatus.SetMixerParam(tMixParam);

    TConfModeV4R4B2 tConfModeV4R4B2;
    tConfModeV4R4B2 = tConfStatusV4R4B2.GetConfMode();

    TConfMode tConfMode;
    ConfModeVer40V4R4B2ToVer40Daily(tConfMode, tConfModeV4R4B2);
    tConfStatus.SetConfMode(tConfMode);

    return;
}
*/
/*=============================================================================
  �� �� ���� ConfStatusVer40V4R4B2ToVer36
  ��    �ܣ� ����״̬ v4.5->4.0
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TConfStatus &tConfStatus: 4.0����״̬�ṹ
  �� �� ֵ�� void
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/11/06    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::ConfStatusV4R5ToV4R4B2( TConfStatusV4R4B2 &tConfStatusV4R4B2, TConfStatusV4R5 &tConfStatus )
{
    memcpy( &tConfStatusV4R4B2, &tConfStatus, min(sizeof(tConfStatus),sizeof(tConfStatusV4R4B2)) );

    //��������TDiscussParam��TConfMode
    TMixParamV4R5 tMixParam;
    tMixParam = tConfStatus.GetMixerParam();
    
    TDiscussParamV4R4B2 tDisParam;
    DiscussParamVer40DailyToVer40V4R4B2(tDisParam, tMixParam);
    tConfStatusV4R4B2.SetDiscussParam(tDisParam);
    
    TConfMode tConfMode;
    tConfMode = tConfStatus.GetConfMode();
    
    TConfModeV4R4B2 tConfModeV4R4B2;
    
    ConfModeVer40DailyToVer40V4R4B2(tConfModeV4R4B2, tConfMode);
    tConfModeV4R4B2.SetAudioMixMode(!tConfStatus.IsNoMixing() || tConfStatus.IsVACing());
    tConfModeV4R4B2.SetDiscussMode(!tConfStatus.IsNoMixing());
    tConfModeV4R4B2.SetMixSpecMt(tConfStatus.IsSpecMixing());
    tConfModeV4R4B2.SetVACMode(tConfStatus.IsVACing());

    tConfStatusV4R4B2.SetConfMode(tConfModeV4R4B2);
    
    return;
}
*/
/*=============================================================================
  �� �� ���� ConfIdVer36ToVer40
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  CConfId &cConfId
             CConfIdVer36 &cConfIdVer36
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/1/6    4.0			�ű���                  ����
=============================================================================*/
/* void CMtAdpUtils::ConfIdVer36ToVer40( CConfId &cConfId, CConfIdVer36 &cConfIdVer36 )
{
	u8 abyConfId[MAXLEN_CONFID];
	u8 byLen = 0;
	cConfIdVer36.GetConfId( abyConfId, sizeof(abyConfId) );
	cConfId.SetConfId( abyConfId, sizeof(abyConfId) );
} */

/*=============================================================================
  �� �� ���� ConfIdVer40ToVer36
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  CConfIdVer36 &cConfIdVer36
             CConfId &cConfId
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/1/6    4.0			�ű���                  ����
=============================================================================*/
/* void CMtAdpUtils::ConfIdVer40ToVer36( CConfIdVer36 &cConfIdVer36, CConfId &cConfId )
{
	u8 abyConfId[MAXLEN_CONFID];
	u8 byLen = 0;
	cConfId.GetConfId( abyConfId, sizeof(abyConfId) );
	cConfIdVer36.SetConfId( abyConfId, sizeof(abyConfId) );
}
*/
/*=============================================================================
  �� �� ���� KdvTimeVer36ToVer40
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  TKdvTime &tKdvTime
             TKdvTimeVer36 &tKdvTimeVer36
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/1/6    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::KdvTimeVer36ToVer40( TKdvTime &tKdvTime, TKdvTimeVer36 &tKdvTimeVer36 )
{
	tKdvTime.SetDay( tKdvTimeVer36.GetDay() );
	tKdvTime.SetHour( tKdvTimeVer36.GetHour() );
	tKdvTime.SetMinute( tKdvTimeVer36.GetMinute() );
	tKdvTime.SetMonth( tKdvTimeVer36.GetMonth() );
	tKdvTime.SetSecond( tKdvTimeVer36.GetSecond() );
	tKdvTime.SetYear( tKdvTimeVer36.GetYear() );
}
*/
/*=============================================================================
  �� �� ���� KdvTimeVer40ToVer36
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  TKdvTimeVer36 &tKdvTimeVer36
             TKdvTime &tKdvTime
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/1/6    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::KdvTimeVer40ToVer36( TKdvTimeVer36 &tKdvTimeVer36, TKdvTime &tKdvTime )
{
	tKdvTimeVer36.SetDay( tKdvTime.GetDay() );
	tKdvTimeVer36.SetHour( tKdvTime.GetHour() );
	tKdvTimeVer36.SetMinute( tKdvTime.GetMinute() );
	tKdvTimeVer36.SetMonth( tKdvTime.GetMonth() );
	tKdvTimeVer36.SetSecond( tKdvTime.GetSecond() );
	tKdvTimeVer36.SetYear( tKdvTime.GetYear() );
}
*/
/*=============================================================================
  �� �� ���� MtVer36ToVer40
  ��    �ܣ� �ն˽ṹ3.6��4.0
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TMt36 &tMt36 : 3.6�ն˽ṹ
  �� �� ֵ�� TMt: 4.0�ն˽ṹ
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::MtVer36ToVer40( TMt &tMt, TMtVer36 &tMtVer36 )
{
	tMtVer36.SetType( tMtVer36.GetType() );
	
	if ( TYPE_MCU == tMtVer36.GetType() )
	{
		tMt.SetMcu( tMtVer36.GetMcuId() );
		tMt.SetMcuId( tMtVer36.GetMcuId() );
		tMt.SetMcuType( tMtVer36.GetMcuType() );
	}
	if ( TYPE_MCUPERI == tMtVer36.GetType() )
	{
		tMt.SetMcuEqp( tMtVer36.GetMcuId(), tMtVer36.GetEqpId(), tMtVer36.GetEqpType() );
		tMt.SetEqpId( tMtVer36.GetEqpId() );
		tMt.SetEqpType( tMtVer36.GetEqpType() );
	}
	if ( TYPE_MT == tMtVer36.GetType() )
	{
		tMt.SetMt( tMtVer36.GetMcuId(), tMtVer36.GetMtId(), tMtVer36.GetDriId(), tMtVer36.GetConfIdx() );
		tMt.SetMtId( tMtVer36.GetMtId() );
		tMt.SetMtType( tMtVer36.GetMtType() );
	}
	tMt.SetDriId( tMtVer36.GetDriId() );
	tMt.SetConfIdx( tMtVer36.GetConfIdx() );

	return;
}
*/
/*=============================================================================
  �� �� ���� MtVer40ToVer36
  ��    �ܣ� �ն˽ṹ4.0��3.6
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TMt &tMt :4.0�ն˽ṹ
  �� �� ֵ�� TMt36: 3.6�ն˽ṹ
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::MtVer40ToVer36( TMtVer36 &tMtVer36, TMt &tMt )
{
	tMtVer36.SetType( tMt.GetType() );

	if ( TYPE_MCU == tMt.GetType() )
	{
		tMtVer36.SetMcu( tMt.GetMcuId() );
		tMtVer36.SetMcuId( tMt.GetMcuId() );
		tMtVer36.SetMcuType( tMt.GetMcuType() );
	}
	if ( TYPE_MCUPERI == tMt.GetType() )
	{
		tMtVer36.SetMcuEqp( tMt.GetMcuId(), tMt.GetEqpId(), tMt.GetEqpType() );
		tMtVer36.SetEqpId( tMt.GetEqpId() );
		tMtVer36.SetEqpType( tMt.GetEqpType() );
	}
	if ( TYPE_MT == tMt.GetType() )
	{
		tMtVer36.SetMt( tMt.GetMcuId(), tMt.GetMtId(), tMt.GetDriId(), tMt.GetConfIdx() );
		tMtVer36.SetMtId( tMt.GetMtId() );
		tMtVer36.SetMtType( tMt.GetMtType() );
	}
	tMtVer36.SetDriId( tMt.GetDriId() );
	tMtVer36.SetConfIdx( tMt.GetConfIdx() );
	
	return;
}
*/

/*=============================================================================
  �� �� ���� ConfModeVer36ToVer40V4R4B2
  ��    �ܣ� ����ģʽ3.6��4.0V4R4B2
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TConfMode36 &tConfMode36: 3.6����ģʽ
  �� �� ֵ�� TConfMode: 4.0����ģʽ
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::ConfModeVer36ToVer40V4R4B2( TConfModeV4R4B2 &tConfMode, TConfModeVer36 &tConfModeVer36 )
{
	tConfMode.SetTakeMode( tConfModeVer36.GetTakeMode() );
	tConfMode.SetLockMode( tConfModeVer36.GetLockMode() );
	tConfMode.SetCallMode( ConfCallModeVer36ToVer40( tConfModeVer36.GetCallMode() ) );
	tConfMode.SetVACMode( tConfModeVer36.IsVACMode() );
	tConfMode.SetAudioMixMode( tConfModeVer36.IsAudioMixMode() );
	tConfMode.SetRecordMode( tConfModeVer36.GetRecordMode() );
	tConfMode.SetPlayMode( tConfModeVer36.GetPlayMode() );
	tConfMode.SetBasMode( ConfBasModeVer36ToVer40( tConfModeVer36.GetBasMode(1) ), TRUE );
	tConfMode.SetPollMode( tConfModeVer36.GetPollMode() );
	tConfMode.SetDiscussMode( tConfModeVer36.IsDiscussMode() );
	tConfMode.SetForceRcvSpeaker( tConfModeVer36.IsForceRcvSpeaker() );
	tConfMode.SetNoChairMode( tConfModeVer36.IsNoChairMode() );
	tConfMode.SetRegToGK( tConfModeVer36.IsRegToGK() );
	tConfMode.SetMixSpecMt( tConfModeVer36.IsMixSpecMt() );
	tConfMode.SetCallInterval( tConfModeVer36.GetCallInterval() );
	tConfMode.SetCallTimes( tConfModeVer36.GetCallTimes() );
	
	return;
}
*/
/*=============================================================================
  �� �� ���� ConfModeVer40ToVer36
  ��    �ܣ� ����ģʽ4.0V4R4B2��3.6
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TConfMode &tConfMode:4.0����ģʽ
  �� �� ֵ�� TConfMode36 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::ConfModeVer40V4R4B2ToVer36( TConfModeVer36 &tConfModeVer36, TConfModeV4R4B2 &tConfMode )
{
	tConfModeVer36.SetTakeMode( tConfMode.GetTakeMode() );
	tConfModeVer36.SetLockMode( tConfMode.GetLockMode() );
	tConfModeVer36.SetCallMode( ConfCallModeVer40ToVer36( tConfMode.GetCallMode() ) );
	tConfModeVer36.SetVACMode( tConfMode.IsVACMode() );
	tConfModeVer36.SetAudioMixMode( tConfMode.IsAudioMixMode() );
	tConfModeVer36.SetRecordMode( tConfMode.GetRecordMode() );
	tConfModeVer36.SetPlayMode( tConfMode.GetPlayMode() );
	tConfModeVer36.SetBasMode( ConfBasModeVer40ToVer36( tConfMode.GetBasMode(1) ), TRUE );
	tConfModeVer36.SetPollMode( tConfMode.GetPollMode() );
	tConfModeVer36.SetDiscussMode( tConfMode.IsDiscussMode() );
	tConfModeVer36.SetForceRcvSpeaker( tConfMode.IsForceRcvSpeaker() );
	tConfModeVer36.SetNoChairMode( tConfMode.IsNoChairMode() );
	tConfModeVer36.SetRegToGK( tConfMode.IsRegToGK() );
	tConfModeVer36.SetMixSpecMt( tConfMode.IsMixSpecMt() );
	tConfModeVer36.SetCallInterval( tConfMode.GetCallInterval() );
	tConfModeVer36.SetCallTimes( tConfMode.GetCallTimes() );
	
	return;
}
*/
/*=============================================================================
  �� �� ���� ConfModeVer40R4V4B2ToVer40Daily
  ��    �ܣ� ����ģʽ4.0V4R4B2��4.0ÿ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TConfMode &tConfMode:4.0����ģʽ
  �� �� ֵ�� TConfMode36 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/11/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::ConfModeVer40V4R4B2ToVer40Daily( TConfMode &tConfMode, TConfModeV4R4B2 &tConfModeR4V4B2 )
{
    tConfMode.SetBasMode(tConfModeR4V4B2.GetBasModeValue(), TRUE);
    tConfMode.SetCallInterval(tConfModeR4V4B2.GetCallInterval());
    tConfMode.SetCallMode(tConfModeR4V4B2.GetCallMode());
    tConfMode.SetCallTimes(tConfModeR4V4B2.GetCallTimes());
    tConfMode.SetForceRcvSpeaker(tConfModeR4V4B2.IsForceRcvSpeaker());
    tConfMode.SetGkCharge(tConfModeR4V4B2.IsGkCharge());
    tConfMode.SetLockMode(tConfModeR4V4B2.GetLockMode());
    tConfMode.SetNoChairMode(tConfModeR4V4B2.IsNoChairMode());
    tConfMode.SetPlayMode(tConfModeR4V4B2.GetPlayMode());
    tConfMode.SetPollMode(tConfModeR4V4B2.GetPollMode());
    tConfMode.SetRecordMode(tConfModeR4V4B2.GetRecordMode());
    tConfMode.SetRecSkipFrame(tConfModeR4V4B2.IsRecSkipFrame());
    tConfMode.SetRegToGK(tConfModeR4V4B2.IsRegToGK());
    tConfMode.SetRollCallMode(ROLLCALL_MODE_NONE);
    tConfMode.SetTakeFromFile(tConfModeR4V4B2.IsTakeFromFile());
    tConfMode.SetTakeMode(tConfModeR4V4B2.GetTakeMode());
    
    return;
}
*/
/*=============================================================================
  �� �� ���� ConfModeVer40DailyToVer40R4V4B2
  ��    �ܣ� ����ģʽ4.0ÿ�յ�4.0V4R4B2
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TConfMode &tConfMode:4.0����ģʽ
  �� �� ֵ�� TConfMode36 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/11/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::ConfModeVer40DailyToVer40V4R4B2( TConfModeV4R4B2 &tConfModeR4V4B2, TConfMode &tConfMode )
{
    //tConfModeR4V4B2.SetAudioMixMode()
    tConfModeR4V4B2.SetBasMode(tConfMode.GetBasModeValue(), TRUE);
    tConfModeR4V4B2.SetCallInterval(tConfMode.GetCallInterval());
    tConfModeR4V4B2.SetCallMode(tConfMode.GetCallMode());
    tConfModeR4V4B2.SetCallTimes(tConfMode.GetCallTimes());
    //tConfModeR4V4B2.SetDiscussMode()
    tConfModeR4V4B2.SetForceRcvSpeaker(tConfMode.IsForceRcvSpeaker());
    tConfModeR4V4B2.SetGkCharge(tConfMode.IsGkCharge());
    tConfModeR4V4B2.SetLockMode(tConfMode.GetLockMode());
    //tConfModeR4V4B2.SetMixSpecMt()
    tConfModeR4V4B2.SetNoChairMode(tConfMode.IsNoChairMode());
    tConfModeR4V4B2.SetPlayMode(tConfMode.GetPlayMode());
    tConfModeR4V4B2.SetPollMode(tConfMode.GetPollMode());
    tConfModeR4V4B2.SetRecordMode(tConfMode.GetRecordMode());
    tConfModeR4V4B2.SetRecSkipFrame(tConfMode.IsRecSkipFrame());
    tConfModeR4V4B2.SetRegToGK(tConfMode.IsRegToGK());
    tConfModeR4V4B2.SetTakeFromFile(tConfMode.IsTakeFromFile());
    tConfModeR4V4B2.SetTakeMode(tConfMode.GetTakeMode());
    //tConfModeR4V4B2.SetVACMode()

    return;
}
*/
/*=============================================================================
  �� �� ���� RecProgVer36ToVer40
  ��    �ܣ� ¼�������3.6��4.0
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TRecProg36 &tRecProg36: 3.6��¼�������
  �� �� ֵ�� TRecProg: 4.0��¼�������
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::RecProgVer36ToVer40( TRecProg &tRecProg, TRecProgVer36 &tRecProgVer36 )
{
	tRecProg.SetCurProg( tRecProgVer36.GetCurProg() );
	tRecProg.SetTotalTime( tRecProgVer36.GetTotalTime() );
	return;
}
*/
/*=============================================================================
  �� �� ���� RecProgVer40ToVer36
  ��    �ܣ� ¼�������4.0��3.6
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TRecProg &tRegProc: 4.0��¼�������
  �� �� ֵ�� TRecProg36: 3.6��¼�������
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::RecProgVer40ToVer36( TRecProgVer36 &tRecProgVer36, TRecProg &tRecProg )
{
	tRecProgVer36.SetCurProg( tRecProg.GetCurProg() );
	tRecProgVer36.SetTotalTime( tRecProg.GetTotalTime() );
	return;
}
*/
/*=============================================================================
  �� �� ���� PollInfoVer36ToVer40
  ��    �ܣ� ��ѯ��Ϣ3.6��4.0
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TPollInfo36 &tPollInfo36: 3.6��ѯ��Ϣ
  �� �� ֵ�� TPollInfo: 4.0 ��ѯ��Ϣ
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::PollInfoVer36ToVer40( TPollInfo &tPollInfo, TPollInfoVer36 &tPollInfoVer36 )
{
	//3.6��ָ����ѯ����, 4.0Ĭ����0, ��ʾ������ѯ
	tPollInfo.SetPollNum( 0 );
	tPollInfo.SetMediaMode( tPollInfoVer36.GetMediaMode() );
	tPollInfo.SetPollState( tPollInfoVer36.GetPollState() );

	TMtPollParamVer36 tMtPollParam36;
	memset( &tMtPollParam36, 0, sizeof(tMtPollParam36) );
	tMtPollParam36 = tPollInfoVer36.GetMtPollParam();

	TMtPollParam tMtPollParam;
	memset( &tMtPollParam, 0, sizeof(tMtPollParam) );
	MtPollParamVer36ToVer40( tMtPollParam, tMtPollParam36 );
	tPollInfo.SetMtPollParam( tMtPollParam );

	return;
}
*/
/*=============================================================================
  �� �� ���� PollInfoVer40ToVer36
  ��    �ܣ� ��ѯ��Ϣ4.0��3.6
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TPollInfo &tPollInfo: 4.0��ѯ��Ϣ
  �� �� ֵ�� TPollInfo36: 3.6��ѯ��Ϣ
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::PollInfoVer40ToVer36( TPollInfoVer36 &tPollInfoVer36, TPollInfo &tPollInfo )
{
	tPollInfoVer36.SetMediaMode( tPollInfo.GetMediaMode() );
	tPollInfoVer36.SetPollState( tPollInfo.GetPollState() );

	TMtPollParam tMtPollParam;
	memset( &tMtPollParam, 0, sizeof(tMtPollParam) );
	tMtPollParam = tPollInfo.GetMtPollParam();

	TMtPollParamVer36 tMtPollParam36;
	memset( &tMtPollParam36, 0, sizeof(tMtPollParam36) );
	MtPollParamVer40ToVer36( tMtPollParam36, tMtPollParam );
	tPollInfoVer36.SetMtPollParam( tMtPollParam36 );
	
	return;
}
*/
/*=============================================================================
  �� �� ���� MtPollParamVer36ToVer40
  ��    �ܣ� �ն���ѯ�����ṹ3.6��4.0
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TMtPollParam36 &tMtPollParam36: 3.6�ն���ѯ�����ṹ
  �� �� ֵ�� TMtPollParam: 4.0 �ն���ѯ�����ṹ
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::MtPollParamVer36ToVer40( TMtPollParam &tMtPollParam, TMtPollParamVer36 &tMtPollParamVer36 )
{
	//3.6��ָ����ѯ����, 4.0Ĭ����0, ��ʾ������ѯ
	tMtPollParam.SetPollNum( 0 );
	tMtPollParam.SetKeepTime( tMtPollParamVer36.GetKeepTime() );

	return;
}
*/
/*=============================================================================
  �� �� ���� MtPollParamVer40ToVer36
  ��    �ܣ� �ն���ѯ�����ṹ4.0��3.6
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TMtPollParam &tMtPollParam: 4.0�ն���ѯ�����ṹ
  �� �� ֵ�� TMtPollParam36: 3.6�ն���ѯ�����ṹ
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::MtPollParamVer40ToVer36( TMtPollParamVer36 &tMtPollParamVer36, TMtPollParam &tMtPollParam )
{
	tMtPollParamVer36.SetKeepTime( tMtPollParam.GetKeepTime() );
	return;
}
*/
/*=============================================================================
  �� �� ���� DiscussParamVer36ToVer40V4R4B2
  ��    �ܣ� ���۲����ṹ3.6��4.0V4R4B2
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TDiscussParam36 &tDiscussParam36: 3.6���۲����ṹ
  �� �� ֵ�� TDiscussParam: 4.0���۲����ṹ
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::DiscussParamVer36ToVer40V4R4B2( TDiscussParamV4R4B2 &tDiscussParam, TDiscussParamVer36 &tDiscussParamVer36 )
{
	tDiscussParam.m_byMixMode   = tDiscussParamVer36.m_byMixMode;
	tDiscussParam.m_byMemberNum = tDiscussParamVer36.m_byMemberNum;
	
	for( s32 nIndex = 0; nIndex < MAXNUM_MIXER_DEPTH; nIndex ++ )
	{
		if ( !tDiscussParamVer36.m_atMtMember[nIndex].IsNull() )
		{
			TMt tMtMember;
			memset( &tMtMember, 0, sizeof(tMtMember) );
			MtVer36ToVer40( tMtMember, tDiscussParamVer36.m_atMtMember[nIndex] );
			tDiscussParam.m_atMtMember[nIndex] = tMtMember;
		}
	}
	return;
}
*/
/*=============================================================================
  �� �� ���� DiscussParamVer40DailyToVer36R4V4B2
  ��    �ܣ� ���۲����ṹ4.0ÿ�յ�4.0V4R4B2
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/11/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::DiscussParamVer40DailyToVer40V4R4B2( TDiscussParamV4R4B2 &tDiscussParam, TMixParamV4R5 &tMixParam )
{
    // FIXME�������V4R4B2��ǰ��MCU�ļ����Բ��ã�����������»ᵼ�»���״̬ˢ������
    tDiscussParam.m_byMixMode = tMixParam.GetMode();

	// [11/20/2010 xliang] add member conversion
	u8 byNum = min(MAXNUM_MIXER_DEPTH, tMixParam.GetMemberNum());
	tDiscussParam.m_byMemberNum = byNum;
	for( u8 byIdx = 0; byIdx < byNum; byIdx ++)
	{
		tDiscussParam.m_atMtMember[byIdx].SetMt(LOCAL_MCUID, tMixParam.GetMemberByIdx(byIdx));
	}
    return;
}
*/
/*=============================================================================
  �� �� ���� DiscussParamVer40V4R4B2ToVer40Daily
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� void
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/11/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::DiscussParamVer40V4R4B2ToVer40Daily( TMixParamV4R5 &tMixParam, TDiscussParamV4R4B2 &tDiscussParam )
{
    tMixParam.SetMode(tDiscussParam.m_byMixMode);
    return;
}
*/
/*=============================================================================
  �� �� ���� DiscussParamVer40ToVer36
  ��    �ܣ� ���۲����ṹ4.0��3.6
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TDiscussParam &tDiscussParam: 4.0���۲����ṹ
  �� �� ֵ�� TDiscussParam36: 3.6���۲����ṹ 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::DiscussParamVer40V4R4B2ToVer36(TDiscussParamVer36 &tDiscussParamVer36, TDiscussParamV4R4B2 &tDiscussParam )
{
	tDiscussParamVer36.m_byMixMode   = tDiscussParam.m_byMixMode;
	tDiscussParamVer36.m_byMemberNum = tDiscussParam.m_byMemberNum;
	
	for( s32 nIndex = 0; nIndex < MAXNUM_MIXER_DEPTH_VER36; nIndex ++ )
	{
		if ( !tDiscussParam.m_atMtMember[nIndex].IsNull() )
		{
			TMtVer36 tMtMem36;
			memset( &tMtMem36, 0, sizeof(tMtMem36) );
			MtVer40ToVer36(tMtMem36, tDiscussParam.m_atMtMember[nIndex] );
			tDiscussParamVer36.m_atMtMember[nIndex] = tMtMem36;
		}
	}
	return;
}
*/
/*=============================================================================
  �� �� ���� VMPParamVer36ToVer40
  ��    �ܣ� ����ϳɲ����ṹ3.6��4.0
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TVMPParam36 &tVMPParam36: 3.6����ϳɲ����ṹ
			 TVMPMember36 &tVMPParam36:3.6����ϳɳ�Ա�ṹ
  �� �� ֵ�� TVMPParam: 4.0����ϳɲ����ṹ
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::VMPParamVer36ToVer40( TVMPParamV4R4B2 &tVMPParam, TVMPParamVer36 &tVMPParamVer36 )
{
	tVMPParam.SetVMPAuto( tVMPParamVer36.IsVMPAuto() );
	tVMPParam.SetVMPBrdst( tVMPParamVer36.IsVMPBrdst() );
	tVMPParam.SetVMPStyle( tVMPParamVer36.GetVMPStyle() );
	tVMPParam.SetVMPSchemeId( tVMPParamVer36.GetMaxMemberNum() );

	for( s32 nIndex = 0; nIndex < MAXNUM_VMP_MEMBER; nIndex ++ )
	{
		if ( NULL != tVMPParamVer36.GetVmpMember(nIndex) ) 
		{
			TVMPMember tVmpMember;
			memset( &tVmpMember, 0, sizeof(tVmpMember) );
			VMPMemberVer36ToVer40( tVmpMember, *tVMPParamVer36.GetVmpMember(nIndex) );
			tVMPParam.SetVmpMember( nIndex, tVmpMember );
		}
	}	
	return;
}
*/
/*=============================================================================
  �� �� ���� VMPParamVer40ToVer36
  ��    �ܣ� ����ϳɲ����ṹ4.0��3.6
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TVMPParam &tVMPParam: 4.0����ϳɲ����ṹ
			 TVMPMember &tVMPParam:4.06����ϳɳ�Ա�ṹ
  �� �� ֵ�� TVMPParam36: 3.6����ϳɲ����ṹ 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::VMPParamVer40ToVer36( TVMPParamVer36 &tVMPParamVer36, TVMPParamV4R4B2 &tVMPParam )
{
	tVMPParamVer36.SetVMPAuto( tVMPParam.IsVMPAuto() );
	tVMPParamVer36.SetVMPBrdst( tVMPParam.IsVMPBrdst() );
	tVMPParamVer36.SetVMPStyle( tVMPParam.GetVMPStyle() );

	for( s32 nIndex = 0; nIndex < MAXNUM_VMP_MEMBER_VER36; nIndex ++ )
	{
		if ( NULL != tVMPParam.GetVmpMember(nIndex) ) 
		{
			TVMPMemberVer36 tVMPMember36;
			memset( &tVMPMember36, 0, sizeof(tVMPMember36) );
			VMPMemberVer40ToVer36( tVMPMember36, *tVMPParam.GetVmpMember(nIndex) );
			tVMPParamVer36.SetVmpMember( nIndex, tVMPMember36 );
		}
	}
	return;
}
*/
/*=============================================================================
  �� �� ���� VMPMemberVer36ToVer40
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TVMPMember36 &tVMPMember36
  �� �� ֵ�� TVMPMember 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::VMPMemberVer36ToVer40( TVMPMember &tVMPMember, TVMPMemberVer36 &tVMPMemberVer36 )
{
	//3.6�в�ָ������ϳɳ�Ա״̬, 4.0��Ĭ��Ϊ����
	tVMPMember.SetMemStatus( MT_STATUS_AUDIENCE ); 	
	tVMPMember.SetMemberType( tVMPMemberVer36.GetMemberType() );
	return;
}
*/
/*=============================================================================
  �� �� ���� VMPMemberVer40ToVer36
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TVMPMember &tVMPMember
  �� �� ֵ�� TVMPMember36 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/02    4.0			�ű���                  ����
=============================================================================*/
/*void CMtAdpUtils::VMPMemberVer40ToVer36( TVMPMemberVer36 &tVMPMemberVer36, TVMPMember &tVMPMember )
{
	tVMPMemberVer36.SetMemberType( tVMPMember.GetMemberType() );
	return;
}
*/

/*=============================================================================
  �� �� ���� MediaTypeVer36ToVer40
  ��    �ܣ� ý����������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byMediaType36
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::MediaTypeVer36ToVer40( u8 byMediaType36 )
{
	switch( byMediaType36 )
	{
	//��Ƶ
	case MEDIA_TYPE_MP3_VER36:		return MEDIA_TYPE_MP3;
	case MEDIA_TYPE_MP2_VER36:		return MEDIA_TYPE_H262;
	case MEDIA_TYPE_PCMA_VER36:		return MEDIA_TYPE_PCMA;
	case MEDIA_TYPE_PCMU_VER36:		return MEDIA_TYPE_PCMU;
	case MEDIA_TYPE_G721_VER36:		return MEDIA_TYPE_G721;
	case MEDIA_TYPE_G722_VER36:		return MEDIA_TYPE_G722;
	case MEDIA_TYPE_G7231_VER36:	return MEDIA_TYPE_G7231;
	case MEDIA_TYPE_G728_VER36:		return MEDIA_TYPE_G728;
	case MEDIA_TYPE_G729_VER36:		return MEDIA_TYPE_G729;
	case MEDIA_TYPE_G7221_VER36:	return MEDIA_TYPE_G7221C;
	//��Ƶ
	case MEDIA_TYPE_MP4_VER36:		return MEDIA_TYPE_MP4;
	case MEDIA_TYPE_H261_VER36:		return MEDIA_TYPE_H261;
	case MEDIA_TYPE_H262_VER36:		return MEDIA_TYPE_H262;
	case MEDIA_TYPE_H263_VER36:		return MEDIA_TYPE_H263;
	case MEDIA_TYPE_H263PLUS_VER36:	return MEDIA_TYPE_H263PLUS;
	case MEDIA_TYPE_H264_VER36:		return MEDIA_TYPE_H264;
	//����
	case MEDIA_TYPE_H224_VER36:		return MEDIA_TYPE_H224;
	case MEDIA_TYPE_T120_VER36:		return MEDIA_TYPE_T120;
	case MEDIA_TYPE_H239_VER36:		return MEDIA_TYPE_H239;
	case MEDIA_TYPE_MMCU_VER36:		return MEDIA_TYPE_MMCU;
	case MEDIA_TYPE_NULL_VER36:		return MEDIA_TYPE_NULL;
	default:						return MEDIA_TYPE_NULL;
	}
}
*/
/*=============================================================================
  �� �� ���� MediaTypeVer40ToVer36
  ��    �ܣ� ý����������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byMediaType40
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::MediaTypeVer40ToVer36( u8 byMediaType40 )
{
	switch( byMediaType40 )
	{
	//��Ƶ
	case MEDIA_TYPE_MP3:			return MEDIA_TYPE_MP3_VER36;
	case MEDIA_TYPE_PCMA:			return MEDIA_TYPE_PCMA_VER36;
	case MEDIA_TYPE_PCMU:			return MEDIA_TYPE_PCMU_VER36;
	case MEDIA_TYPE_G721:			return MEDIA_TYPE_G721_VER36;
	case MEDIA_TYPE_G722:			return MEDIA_TYPE_G722_VER36;
	case MEDIA_TYPE_G7231:			return MEDIA_TYPE_G7231_VER36;
	case MEDIA_TYPE_G728:			return MEDIA_TYPE_G728_VER36;
	case MEDIA_TYPE_G729:			return MEDIA_TYPE_G729_VER36;
	case MEDIA_TYPE_G7221C:			return MEDIA_TYPE_G7221_VER36;
	//��Ƶ
	case MEDIA_TYPE_MP4:			return MEDIA_TYPE_MP4_VER36;
	case MEDIA_TYPE_H261:			return MEDIA_TYPE_H261_VER36;
	case MEDIA_TYPE_H262:			return MEDIA_TYPE_H262_VER36;
	case MEDIA_TYPE_H263:			return MEDIA_TYPE_H263_VER36;
	case MEDIA_TYPE_H263PLUS:		return MEDIA_TYPE_H263PLUS_VER36;
	case MEDIA_TYPE_H264:			return MEDIA_TYPE_H264_VER36;
	//����
	case MEDIA_TYPE_H224:			return MEDIA_TYPE_H224_VER36;
	case MEDIA_TYPE_T120:			return MEDIA_TYPE_T120_VER36;
	case MEDIA_TYPE_H239:			return MEDIA_TYPE_H239_VER36;
	case MEDIA_TYPE_MMCU:			return MEDIA_TYPE_MMCU_VER36;
	case MEDIA_TYPE_NULL:			return MEDIA_TYPE_NULL_VER36;
	default:						return MEDIA_TYPE_NULL_VER36;
	}
}
*/
/*=============================================================================
  �� �� ���� VideoFormatVer36ToVer40
  ��    �ܣ� ��Ƶ�ֱ�������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byVideoFormat36
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::VideoFormatVer36ToVer40( u8 byVideoFormat36 )
{
	switch( byVideoFormat36 )
	{
	case VIDEO_FORMAT_SQCIF_VER36:		return VIDEO_FORMAT_SQCIF;
	case VIDEO_FORMAT_QCIF_VER36:		return VIDEO_FORMAT_QCIF;
	case VIDEO_FORMAT_CIF_VER36:		return VIDEO_FORMAT_CIF;
	case VIDEO_FORMAT_4CIF_VER36:		return VIDEO_FORMAT_4CIF;
	case VIDEO_FORMAT_16CIF_VER36:		return VIDEO_FORMAT_16CIF;
	case VIDEO_FORMAT_AUTO_VER36:		return VIDEO_FORMAT_AUTO;
	case VIDEO_FORMAT_2CIF_VER36:		return VIDEO_FORMAT_2CIF;
	default:							return VIDEO_FORMAT_CIF;
	}
}
*/
/*=============================================================================
  �� �� ���� VideoFormatVer40ToVer36
  ��    �ܣ� ��Ƶ�ֱ�������
  �㷨ʵ�֣� 4.0�����ֱ���Ĭ��תΪ3.6��CIF�ֱ���
  ȫ�ֱ����� 
  ��    ���� u8 byDStreamType40
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::VideoFormatVer40ToVer36( u8 byVideoFormat40 )
{
	switch( byVideoFormat40 )
	{
	case VIDEO_FORMAT_SQCIF:		return VIDEO_FORMAT_SQCIF_VER36;
	case VIDEO_FORMAT_QCIF:			return VIDEO_FORMAT_QCIF_VER36;
	case VIDEO_FORMAT_CIF:			return VIDEO_FORMAT_CIF_VER36;
	case VIDEO_FORMAT_4CIF:			return VIDEO_FORMAT_4CIF_VER36;
	case VIDEO_FORMAT_16CIF:		return VIDEO_FORMAT_16CIF_VER36;
	case VIDEO_FORMAT_AUTO:			return VIDEO_FORMAT_AUTO_VER36;
	case VIDEO_FORMAT_2CIF:			return VIDEO_FORMAT_2CIF_VER36;
	//4.0�����ֱ���
	case VIDEO_FORMAT_SIF:			return VIDEO_FORMAT_CIF_VER36;
	case VIDEO_FORMAT_2SIF:			return VIDEO_FORMAT_CIF_VER36;
	case VIDEO_FORMAT_4SIF:			return VIDEO_FORMAT_CIF_VER36;
	case VIDEO_FORMAT_VGA:			return VIDEO_FORMAT_CIF_VER36;
	case VIDEO_FORMAT_SVGA:			return VIDEO_FORMAT_CIF_VER36;
	case VIDEO_FORMAT_XGA:			return VIDEO_FORMAT_CIF_VER36;
	default:						return VIDEO_FORMAT_CIF_VER36;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfCallModeVer36ToVer40
  ��    �ܣ� �����ն˷�ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfCallMode36
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfCallModeVer36ToVer40( u8 byConfCallMode36 )
{
	switch( byConfCallMode36 )
	{
	case CONF_CALLMODE_NONE_VER36:		return CONF_CALLMODE_NONE;
	case CONF_CALLMODE_ONCE_VER36:		return CONF_CALLMODE_TIMER;
	case CONF_CALLMODE_TIMER_VER36:		return CONF_CALLMODE_TIMER;
	default:							return CONF_CALLMODE_TIMER;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfCallModeVer40ToVer36
  ��    �ܣ� �����ն˷�ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfCallMode40
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfCallModeVer40ToVer36( u8 byConfCallMode40 )
{
	switch( byConfCallMode40 )
	{
	case CONF_CALLMODE_NONE:		return CONF_CALLMODE_NONE_VER36;
	case CONF_CALLMODE_TIMER:		return CONF_CALLMODE_TIMER_VER36;
	default:						return CONF_CALLMODE_TIMER_VER36;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfBasModeVer36ToVer40
  ��    �ܣ� �������䷽ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfBasMode36
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfBasModeVer36ToVer40( u8 byConfBasMode36 )
{
	switch( byConfBasMode36 )
	{
	//BasMode���ն˲����ĵ�һ������, ����ת����ʵ�ֶ�Ĭ����дNONE
	case CONF_BASMODE_BR_VER36:		return CONF_BASMODE_NONE;
	case CONF_BASMODE_RTP_VER36:	return CONF_BASMODE_NONE;
	case CONF_BASMODE_MDTP_VER36:	return CONF_BASMODE_NONE;
	case CONF_BASMODE_NONE_VER36:	return CONF_BASMODE_NONE;
	default:						return CONF_BASMODE_NONE;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfBasModeVer40ToVer36
  ��    �ܣ� �������䷽ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfBasMode40
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfBasModeVer40ToVer36( u8 byConfBasMode40 )
{
	switch( byConfBasMode40 )
	{
	//BasMode���ն˲����ĵ�һ������, ����ת����ʵ�ֶ�Ĭ����дNONE
	case CONF_BASMODE_BR:			return CONF_BASMODE_NONE_VER36;
	case CONF_BASMODE_AUD:			return CONF_BASMODE_NONE_VER36;
	case CONF_BASMODE_VID:			return CONF_BASMODE_NONE_VER36;
	case CONF_BASMODE_CASDAUD:		return CONF_BASMODE_NONE_VER36;
	case CONF_BASMODE_CASDVID:		return CONF_BASMODE_NONE_VER36;
	case CONF_BASMODE_NONE:			return CONF_BASMODE_NONE_VER36;
	default:						return CONF_BASMODE_NONE_VER36;
	}
}
*/
/*=============================================================================
  �� �� ���� GetGKErrCode
  ��    �ܣ� ��GK�Ĵ�����ת��MCU�Ĵ����붨��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u16 wErrCode
  �� �� ֵ�� u16 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
u16 CMtAdpUtils::GetGKErrCode( u16 wErrCode )
{
    u16 wErrRet = 0;
    switch( wErrCode ) 
    {
    case RADIUS_INTER_ACCT_NOT_EXIST:   wErrRet = (u16)ERR_MCU_GK_USRNAME;   break;
    case RADIUS_INTER_INVALID_PWD:      wErrRet = (u16)ERR_MCU_GK_USRPWD;    break;
    case RADIUS_INTER_CONF_NOT_EXIST:   wErrRet = (u16)ERR_MCU_GK_CONF;      break;
    case RADIUS_INTER_DB_FAIL:          wErrRet = (u16)ERR_MCU_GK_DB;        break;
    case RADIUS_INTER_TIME_OUT:         wErrRet = (u16)ERR_MCU_GK_STARTCHARGE_TIMEOUT;   break;
    case RADIUS_INTER_GK_NOT_SUPPORT_ACCT:  wErrRet = (u16)ERR_MCU_GK_WITHOUT_RADIUS;    break;
    case RADIUS_INTER_GK_OP_FAILED:     wErrRet = (u16)ERR_MCU_GK_OP_RADIUS_FAILED;      break;
    case RADIUS_INTER_ACCT_IN_USE:      wErrRet = (u16)ERR_MCU_GK_ACCT_IN_USE;   break;
    case RADIUS_INTER__OTHER_ERRPR:     wErrRet = (u16)ERR_MCU_GK_UNKNOW;        break;
    case RADIUS_INTER_SESSION_ID_NULL:  wErrRet = (u16)ERR_MCU_GK_SSNID_NULL;    break;
    case RADIUS_INTER_GK_INSTANCE_FULL: wErrRet = (u16)ERR_MCU_GK_INST_FULL;     break;
    default:                            wErrRet = (u16)ERR_MCU_GK_UNKNOW;        break;
    }
    return wErrRet;
}

/*=============================================================================
  �� �� ���� DisconnectReasonStack2MtAdp
  ��    �ܣ� �������ն�ʧ�ܵĴ�����ת�� Mtadp ��Ӧ�����붨��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CallDisconnectReason emReason
  �� �� ֵ�� emDisconnectReason 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/05/15    4.0			�ű���                  ����
=============================================================================*/
emDisconnectReason CMtAdpUtils::DisconnectReasonStack2MtAdp( CallDisconnectReason emReason )
{
    emDisconnectReason emReasonMtAdp;
    switch( emReason )
    {
    case reason_busy:       emReasonMtAdp = emDisconnectReasonBusy;     break;
    case reason_normal:     emReasonMtAdp = emDisconnectReasonNormal;   break;
    case reason_rejected:   emReasonMtAdp = emDisconnectReasonRejected; break;
    case reason_unreachable:emReasonMtAdp = emDisconnectReasonUnreachable;  break;
    case reason_local:      emReasonMtAdp = emDisconnectReasonLocal;    break;
    case reason_unknown:    emReasonMtAdp = emDisconnectReasonUnknown;  break;
    default:                emReasonMtAdp = emDisconnectReasonUnknown;  break;
    }
    return emReasonMtAdp;
}

/*=============================================================================
  �� �� ���� DisconnectReasonMtAdp2Stack
  ��    �ܣ� �������ն�ʧ�ܵĴ�����ת�� Э��ջ ��Ӧ�����붨��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� emDisconnectReason emReason
  �� �� ֵ�� CallDisconnectReason 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/05/16    4.0			�ű���                  ����
=============================================================================*/
CallDisconnectReason CMtAdpUtils::DisconnectReasonMtAdp2Stack( emDisconnectReason emReason )
{
    CallDisconnectReason emReasonStack;
    switch( emReason )
    {
    case emDisconnectReasonBusy:            emReasonStack = reason_busy;        break;
    case emDisconnectReasonNormal:          emReasonStack = reason_normal;      break;
    case emDisconnectReasonRejected:        emReasonStack = reason_rejected;    break;
    case emDisconnectReasonUnreachable:     emReasonStack = reason_unreachable; break;
    case emDisconnectReasonLocal:           emReasonStack = reason_local;       break;
	case emDisconnectReasonRemoteReconnect: emReasonStack = reason_reconnect;   break;
	case emDisconnectReasonRemoteConfHolding:emReasonStack = reason_confholding;break;
	case emDisconnectReasonRemoteHasCascaded:emReasonStack = reason_hascascaded;break;

    case emDisconnectReasonDRQ:
    case emDisconnectReasonRtd:
    case emDisconnectReasonUnmatched:
    case emDisconnectReasonUnknown:
    default:                                emReasonStack = reason_unknown;     break;
    }
    return emReasonStack;
}

/*=============================================================================
  �� �� ���� DisconnectReasonMtAdp2Vc
  ��    �ܣ� �������ն�ʧ�ܵĴ�����ת��MCU��Ӧ�����붨��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byReason
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/05/14    4.0			�ű���                  ����
=============================================================================*/
u8 CMtAdpUtils::DisconnectReasonMtAdp2Vc( emDisconnectReason emReason )
{
    u8 byReasonVc = 0;
    switch( emReason )
    {
    case emDisconnectReasonDRQ:         byReasonVc = MTLEFT_REASON_DRQ; break;
    case emDisconnectReasonRtd:         byReasonVc = MTLEFT_REASON_RTD; break;
    case emDisconnectReasonBusy:        byReasonVc = MTLEFT_REASON_BUSY; break;
    case emDisconnectReasonNormal:      byReasonVc = MTLEFT_REASON_NORMAL; break;
    case emDisconnectReasonRejected:    byReasonVc = MTLEFT_REASON_REJECTED; break;
    case emDisconnectReasonUnreachable: byReasonVc = MTLEFT_REASON_UNREACHABLE;  break;
    case emDisconnectReasonLocal:       byReasonVc = MTLEFT_REASON_LOCAL;  break;
    case emDisconnectReasonUnmatched:   byReasonVc = MTLEFT_REASON_UNMATCHTYPE; break;
    case emDisconnectReasonUnknown:     byReasonVc = MTLEFT_REASON_UNKNOW; break;
	case emDisconnectReasonBysyExt:     byReasonVc = MTLEFT_REASON_BUSYEXT; break;
	case emDisconnectReasonRemoteReconnect:   byReasonVc = MTLEFT_REASON_REMOTERECONNECT; break;
// [pengjie 2010/6/2] �������ԭ�� Э��ջ->mcu
	case emDisconnectReasonRemoteConfHolding: byReasonVc = MTLEFT_REASON_REMOTECONFHOLDING; break;
	case emDisconnectReasonRemoteHasCascaded: byReasonVc = MTLEFT_REASON_REMOTEHASCASCADED; break;
// End
    default:                            byReasonVc = MTLEFT_REASON_UNKNOW; break;
    }
    return byReasonVc;
}

/*=============================================================================
  �� �� ���� Ipdw2Str
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwIp
             s8* pszStr
  �� �� ֵ�� void 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang                  ����
=============================================================================*/
void CMtAdpUtils::Ipdw2Str( u32 dwIp, s8* pszStr )
{
	u8 *pbyPointer = (u8 *)&dwIp;
	sprintf( pszStr,"%d.%d.%d.%d",pbyPointer[0],pbyPointer[1],pbyPointer[2],pbyPointer[3]);
}


/*=============================================================================
  �� �� ���� GetAliasType
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� s8  * pszStr
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang                  ����
=============================================================================*/
u8 CMtAdpUtils::GetAliasType( s8* pszStr )
{
	if(IsIpStr(pszStr))
	{	
		//ip
		return (u8)mtAliasTypeTransportAddress;
	}
	else if(IsIncludeAllChar(pszStr,"0123456789*,#"))
	{
		//e164
		return (u8)mtAliasTypeE164;
	}
	else
	{
		//h323
		return (u8)mtAliasTypeH323ID;
	}

//	return mtAliasTypeOthers;
}

/*=============================================================================
  �� �� ���� IsIncludeAllChar
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� s8  * pszSrc
             s8  * pszDst
  �� �� ֵ�� BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang                  ����
=============================================================================*/
BOOL32 CMtAdpUtils::IsIncludeAllChar(s8* pszSrc, s8* pszDst)
{
	s8* pszSrcTmp = pszSrc;
	s8* pszDstTmp = pszDst;
	while(*pszSrcTmp)
	{
		pszDstTmp = pszDst;
		while(*pszDstTmp)
		{
			if(*pszSrcTmp == *pszDstTmp) 
				break;
//			*pszDstTmp++;
            pszDstTmp++;
		}
		if(*pszDstTmp == 0)
		{
			return FALSE;
		}
		pszSrcTmp ++;
	}
	return TRUE;
}

/*=============================================================================
  �� �� ���� IsIpStr
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� s8* pszStr
  �� �� ֵ�� BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang                  ����
=============================================================================*/
BOOL32 CMtAdpUtils::IsIpStr(s8* pszStr)
{
    s32 anValue[4]={ 0 };
	s32 anLen[4] = { 0 };

	s32 nDot = 0;
	s8* pszTmp = pszStr;
	s32 nPos;
	for( nPos = 0; *pszTmp && nPos < 16; nPos ++, pszTmp ++ )
	{
		if( *pszTmp == '.' )
		{
			nDot ++;
			//excude 1256.1.1.1.1
			if( nDot > 3 )
			{
				return FALSE;
			}
			continue;			
		}
		//excude a.1.1.1
		if( *pszTmp<'0'|| *pszTmp>'9' )
		{
			return FALSE;
		}

		anValue[nDot] = anValue[nDot]*10 + (*pszTmp-'0');
		anLen[nDot] ++;
	}

	//excude 1256.1.1.1234444
	if( nPos >= 16 )
	{
		return FALSE;
	}

	//excude 0.1.1.1
	if( anValue[0] == 0 )
	{
		return FALSE;
	}

	for( s32 j = 0; j < 4; j ++ )
	{
		//excude 1256.1.1.1
		if( anLen[j] == 0 || anLen[j] > 3 )
		{
			return FALSE;
		}
		//excude 256.1.1.1
		if( anValue[j] > 255 )
		{
			return FALSE;
		}
	}
	return TRUE;
}

/*=============================================================================
  �� �� ���� MediaModeOut2In
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� emMediaType mediatype
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang                  ����
=============================================================================*/
u8 CMtAdpUtils::MediaModeOut2In(emMediaType mediatype)
{
	switch(mediatype)
	{
	case emMediaTypeVideo: return MODE_VIDEO; //break;
	case emMediaTypeAudio: return MODE_AUDIO; //break;
	case emMediaTypeData:  return MODE_DATA;  //break;
	default:               return MODE_NONE;  //break;
	}
}
/*=============================================================================
�� �� ���� GetResFromStr
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� s8* pszRes
�� �� ֵ�� u8 
-----------------------------------------------------------------------------
�޸ļ�¼��
��  ��		�汾		�޸���		�߶���    �޸�����
2008/10/30    4.5			�ű���                  ����
=============================================================================*/
u8 CMtAdpUtils::GetResFromStr(s8* pszRes)
{
    if (0 == strcmp(pszRes, "VGA"))
    {
        return 11;
    }
    else if (0 == strcmp(pszRes, "SVGA"))
    {
        return 12;
    }
    else if (0 == strcmp(pszRes, "XGA"))
    {
        return 13;
    }
    else if (0 == strcmp(pszRes, "SXGA"))
    {
        return 33;
    }
    else if (0 == strcmp(pszRes, "UXGA"))
    {
        return 34;
    }
    else if (0 == strcmp(pszRes, "SQCIF"))
    {
        return 1;
    }
    else if (0 == strcmp(pszRes, "QCIF"))
    {
        return 2;
    }
    else if (0 == strcmp(pszRes, "CIF"))
    {
        return 3;
    }
    else if (0 == strcmp(pszRes, "2CIF"))
    {
        return 4;
    }
    else if (0 == strcmp(pszRes, "4CIF"))
    {
        return 5;
    }
    else if (0 == strcmp(pszRes, "16CIF"))
    {
        return 6;
    }
    else if (0 == strcmp(pszRes, "SIF"))
    {
        return 8;
    }
    else if (0 == strcmp(pszRes, "2SIF"))
    {
        return 9;
    }
    else if (0 == strcmp(pszRes, "4SIF"))
    {
        return 10;
    }
    else if (0 == strcmp(pszRes, "W4CIF"))
    {
        return 31;
    }
    else if (0 == strcmp(pszRes, "720P"))
    {
        return 32;
    }
	
    else if (0 == strcmp(pszRes, "1080P"))
    {
        return 35;
    }
	
    return 0;
}

/*=============================================================================
  �� �� ���� MediaModeIn2Out
  ��    �ܣ� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byMediaMode
  �� �� ֵ�� emMediaType 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang                  ����
=============================================================================*/
emMediaType CMtAdpUtils::MediaModeIn2Out( u8 byMediaMode)
{
	switch(byMediaMode)
	{
	case MODE_VIDEO: return emMediaTypeVideo; //break;
	case MODE_AUDIO: return emMediaTypeAudio; //break;
	case MODE_DATA:  return emMediaTypeData;  //break;
	default:         return emMediaTypeNone;  //break;
	}
}

/*=============================================================================
  �� �� ���� PszGetRASReasonName
  ��    �ܣ� ���RAS����������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� s32    nReason
             s8   * pszBuf
             u8     byBufLen 
  �� �� ֵ�� 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2003/11/11    1.0			TanGuang                  ����
=============================================================================*/
s8* CMtAdpUtils::PszGetRASReasonName( s32 nReason, s8*  pszBuf, u8 byBufLen )
{
	if( ! pszBuf || byBufLen  == 0 )
	{
		return NULL;
	}
	s8 achBuf[128];

#define REASON_CASE(x) \
	case x: \
		strcpy( achBuf, #x); \
		strncpy( pszBuf, achBuf + strlen("cmRASReason"), byBufLen  ); \
	break;

	switch( nReason )
	{
    REASON_CASE( cmRASReasonResourceUnavailable )             /* GRJ, RRJ, ARJ, LRJ - gatekeeper resources exhausted */
    REASON_CASE( cmRASReasonInsufficientResources )           /* BRJ */
    REASON_CASE( cmRASReasonInvalidRevision )                 /* GRJ, RRJ, BRJ */
    REASON_CASE( cmRASReasonInvalidCallSignalAddress )        /* RRJ */
    REASON_CASE( cmRASReasonInvalidRASAddress )               /* RRJ - supplied address is invalid */
    REASON_CASE( cmRASReasonInvalidTerminalType )             /* RRJ */
    REASON_CASE( cmRASReasonInvalidPermission )               /* ARJ - permission has expired */
															  /* BRJ - true permission violation */
															  /* LRJ - exclusion by administrator or feature */
    REASON_CASE( cmRASReasonInvalidConferenceID )             /* BRJ - possible revision */
    REASON_CASE( cmRASReasonInvalidEndpointID )               /* ARJ */
    REASON_CASE( cmRASReasonCallerNotRegistered )             /* ARJ */
    REASON_CASE( cmRASReasonCalledPartyNotRegistered )        /* ARJ - can't translate address */
    REASON_CASE( cmRASReasonDiscoveryRequired )               /* RRJ - registration permission has aged */
    REASON_CASE( cmRASReasonDuplicateAlias )                  /* RRJ - alias registered to another endpoint */
    REASON_CASE( cmRASReasonTransportNotSupported )           /* RRJ - one or more of the transports */
    REASON_CASE( cmRASReasonCallInProgress )                  /* URJ */
    REASON_CASE( cmRASReasonRouteCallToGatekeeper )           /* ARJ */
    REASON_CASE( cmRASReasonRequestToDropOther )              /* DRJ - can't request drop for others */
    REASON_CASE( cmRASReasonNotRegistered )                   /* DRJ, LRJ, INAK - not registered with gatekeeper */
    REASON_CASE( cmRASReasonUndefined )                       /* GRJ, RRJ, URJ, ARJ, BRJ, LRJ, INAK */
    REASON_CASE( cmRASReasonTerminalExcluded )                /* GRJ - permission failure, not a resource failure */
    REASON_CASE( cmRASReasonNotBound )                        /* BRJ - discovery permission has aged */
    REASON_CASE( cmRASReasonNotCurrentlyRegistered )          /* URJ */
    REASON_CASE( cmRASReasonRequestDenied )                   /* ARJ - no bandwidrg available */
                                                              /* LRJ - cannot find location */
    REASON_CASE( cmRASReasonLocationNotFound )                /* LRJ - cannot find location */
    REASON_CASE( cmRASReasonSecurityDenial )                  /* GRJ, RRJ, URJ, ARJ, BRJ, LRJ, DRJ, INAK */
    REASON_CASE( cmRASReasonTransportQOSNotSupported )        /* RRJ */
    REASON_CASE( cmRASResourceUnavailable )                   /* Same as cmRASReasonResourceUnavailable */
    REASON_CASE( cmRASReasonInvalidAlias )                    /* RRJ - alias not consistent with gatekeeper rules */
    REASON_CASE( cmRASReasonPermissionDenied )                /* URJ - requesting user not allowed to unregister specified user */
    REASON_CASE( cmRASReasonQOSControlNotSupported )          /* ARJ */
    REASON_CASE( cmRASReasonIncompleteAddress )               /* ARJ, LRJ */
    REASON_CASE( cmRASReasonFullRegistrationRequired )        /* RRJ - registration permission has expired */
    REASON_CASE( cmRASReasonRouteCallToSCN )                  /* ARJ, LRJ */
    REASON_CASE( cmRASReasonAliasesInconsistent )             /* ARJ, LRJ - multiple aliases in request identify distinct people */
    REASON_CASE( cmRASReasonAdditiveRegistrationNotSupported )/* RRJ */
    REASON_CASE( cmRASReasonInvalidTerminalAliases )          /* RRJ */
    REASON_CASE( cmRASReasonExceedsCallCapacity )             /* ARJ - destination does not have the capacity for this call */
    REASON_CASE( cmRASReasonCollectDestination )              /* ARJ */
    REASON_CASE( cmRASReasonCollectPIN )                      /* ARJ */
    REASON_CASE( cmRASReasonGenericData )                     /* GRJ, RRJ, ARJ, LRJ */
    REASON_CASE( cmRASReasonNeededFeatureNotSupported )       /* GRJ, RRJ, ARJ, LRJ */
    REASON_CASE( cmRASReasonUnknownMessageResponse )          /* XRS message was received for the request */
    REASON_CASE( cmRASReasonHopCountExceeded )                /* LRJ */
	case -1: strncpy(  pszBuf, "Timeout", byBufLen  ); break;
	default : * pszBuf = 0;
	}
#undef REASON_CASE

	return  pszBuf;
}


/*=============================================================================
  �� �� ���� IsSrcFpsAcptable
  ��    �ܣ� Դ�˷��͵�֡���Ƿ����ΪĿ�Ķ˽���
  �㷨ʵ�֣� Դ�˷��͵�֡��С�ڵ��ڻ�ģ��С�ڵ���Ŀ�Ķ˽������� ����Ϊ���Խ���
  ȫ�ֱ����� 
  ��    ���� u8 bySrcFPS
             u8 byDstFPS
  �� �� ֵ�� BOOL32
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/12/21    4.5			�ű���                  ����
=============================================================================*/
BOOL32 CMtAdpUtils::IsSrcFpsAcptable(u8 bySrcFPS, u8 byDstFPS)
{
    //ֱ��֧��
    if ( bySrcFPS <= byDstFPS )
    {
        return TRUE;
    }
    //���е�ģ���ж�
    if ( byDstFPS >= 58 && byDstFPS <= 60 )
    {
        if ( bySrcFPS >= 58 && bySrcFPS <= 60 )
        {
            return TRUE;
        }
    }
    else if ( byDstFPS >= 25 && byDstFPS <= 30 )
    {
        if ( bySrcFPS >= 24 && bySrcFPS <= 30 )
        {
            return TRUE;
        }
    }
    else if ( byDstFPS >= 14 && byDstFPS <= 16 )
    {
        if ( bySrcFPS >= 14 && bySrcFPS <= 16 )
        {
            return TRUE;
        }
    }
    else if ( byDstFPS >= 9 && byDstFPS <= 11 )
    {
        if ( bySrcFPS >= 9 && bySrcFPS <= 11 )
        {
            return TRUE;
        }
    }
    else if ( byDstFPS >= 4 && byDstFPS <= 6 )
    {
        if ( bySrcFPS >= 4 && bySrcFPS <= 6 )
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*=============================================================================
  �� �� ���� TRY_MEM_COPY
  ��    �ܣ� �ڴ濽������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  
  �� �� ֵ�� BOOL32 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2005/12/31    4.0			�ű���                  ����
=============================================================================*/
BOOL32 TRY_MEM_COPY( void *pDst, void *pSrc, u32 dwLen, void *pLimit )
{
	if( (u32)pDst + dwLen > (u32)pLimit ) 
	{
		OspLog( LOGLVL_DEBUG1, "insufficient memory to store the data.\n"); 
		return FALSE; 
	} 
	memcpy( pDst, pSrc, dwLen );	
	return TRUE;
}




/******************************************************************************
	˵��: 
	1. �����ǻ�δ���õ��������亯��
	2. ���º�����3.6�汾��(��)4.0�汾�����漰���������͵ĵ���ʱʹ��
	3. �����3.6�汾��������, ��mastruct.h��3.6��mcustruct.hͬ��, Ȼ����ݵ���
	   �����޸�������Ӧ�����亯��, ������, ����صĵط���������
	4. �����4.0�汾��������, ֱ�Ӹ��ݵ��������޸�������Ӧ�����亯��, ������
	5. ��������汾�������˵���, ��3, 4�޸�, ������
******************************************************************************/
 
/*=============================================================================
  �� �� ���� ActiveMediaTypeVer36ToVer40
  ��    �ܣ� kdvԼ���ı��ط���ʱʹ�õĻý����������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byActiveMediaType36
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ActiveMediaTypeVer36ToVer40( u8 byActiveMediaType36 )
{
	switch( byActiveMediaType36 )
	{
	case ACTIVE_TYPE_PCMA_VER36:		return ACTIVE_TYPE_PCMA;
	case ACTIVE_TYPE_PCMU_VER36:		return ACTIVE_TYPE_PCMU;
	case ACTIVE_TYPE_G721_VER36:		return ACTIVE_TYPE_G721;
	case ACTIVE_TYPE_G722_VER36:		return ACTIVE_TYPE_G722;
	case ACTIVE_TYPE_G7231_VER36:		return ACTIVE_TYPE_G7231;
	case ACTIVE_TYPE_G728_VER36:		return ACTIVE_TYPE_G728;
	case ACTIVE_TYPE_G729_VER36:		return ACTIVE_TYPE_G729;
	case ACTIVE_TYPE_G7221_VER36:		return ACTIVE_TYPE_G7221;			
	case ACTIVE_TYPE_H261_VER36:		return ACTIVE_TYPE_H261;		
	case ACTIVE_TYPE_H262_VER36:		return ACTIVE_TYPE_H262;
	case ACTIVE_TYPE_H263_VER36:		return ACTIVE_TYPE_H263;
	default:							return MEDIA_TYPE_NULL;
	}
}
*/
/*=============================================================================
  �� �� ���� ActiveMediaTypeVer40ToVer36
  ��    �ܣ� kdvԼ���ı��ط���ʱʹ�õĻý����������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byActiveMediaType40
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ActiveMediaTypeVer40ToVer36( u8 byActiveMediaType40 )
{
	switch( byActiveMediaType40 )
	{
	case ACTIVE_TYPE_PCMA:			return ACTIVE_TYPE_PCMA_VER36;
	case ACTIVE_TYPE_PCMU:			return ACTIVE_TYPE_PCMU_VER36;
	case ACTIVE_TYPE_G721:			return ACTIVE_TYPE_G721_VER36;
	case ACTIVE_TYPE_G722:			return ACTIVE_TYPE_G722_VER36;
	case ACTIVE_TYPE_G7231:			return ACTIVE_TYPE_G7231_VER36;
	case ACTIVE_TYPE_G728:			return ACTIVE_TYPE_G728_VER36;
	case ACTIVE_TYPE_G729:			return ACTIVE_TYPE_G729_VER36;
	case ACTIVE_TYPE_G7221:			return ACTIVE_TYPE_G7221_VER36;			
	case ACTIVE_TYPE_H261:			return ACTIVE_TYPE_H261_VER36;		
	case ACTIVE_TYPE_H262:			return ACTIVE_TYPE_H262_VER36;
	case ACTIVE_TYPE_H263:			return ACTIVE_TYPE_H263_VER36;
	default:						return MEDIA_TYPE_NULL_VER36;
	}
}
*/
/*=============================================================================
  �� �� ���� EqpMainTypeVer36ToVer40
  ��    �ܣ� �豸��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byEqpMainType36
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::EqpMainTypeVer36ToVer40( u8 byEqpMainType36 )
{
	switch( byEqpMainType36 )
	{
	case TYPE_MCU_VER36:			return TYPE_MCU;
	case TYPE_MCUPERI_VER36:		return TYPE_MCUPERI; 
	case TYPE_MT_VER36:				return TYPE_MT;
	default:						return NULL;
	}
}
*/
/*=============================================================================
  �� �� ���� EqpMainTypeVer40ToVer36
  ��    �ܣ� �豸��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byEqpMainType40
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::EqpMainTypeVer40ToVer36( u8 byEqpMainType40 )
{
	switch( byEqpMainType40 )
	{
	case TYPE_MCU:					return TYPE_MCU_VER36;
	case TYPE_MCUPERI:				return TYPE_MCUPERI_VER36; 
	case TYPE_MT:					return TYPE_MT_VER36;
	default:						return NULL;
	}
}
*/
/*=============================================================================
  �� �� ���� MediaModeVer36ToVer40
  ��    �ܣ� ����ͼ����������ģʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byMediaType36
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::MediaModeVer36ToVer40( u8 byMediaMode36 )
{
	switch( byMediaMode36)
	{
	case MODE_VIDEO_VER36:				return MODE_VIDEO;				
	case MODE_AUDIO_VER36:				return MODE_AUDIO;
	case MODE_BOTH_VER36:				return MODE_BOTH;
	case MODE_DATA_VER36:				return MODE_DATA;
	case MODE_SECVIDEO_VER36:			return MODE_SECVIDEO;
	case MODE_NONE_VER36:				return MODE_NONE;
	default:							return MODE_NONE;
	}
}
*/
/*=============================================================================
  �� �� ���� MediaModeVer40ToVer36
  ��    �ܣ� ����ͼ����������ģʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byMediaType40
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::MediaModeVer40ToVer36( u8 byMediaMode40 )
{
	switch( byMediaMode40)
	{
	case MODE_VIDEO:				return MODE_VIDEO_VER36;				
	case MODE_AUDIO:				return MODE_AUDIO_VER36;
	case MODE_BOTH:					return MODE_BOTH_VER36;
	case MODE_DATA:					return MODE_DATA_VER36;
	case MODE_SECVIDEO:				return MODE_SECVIDEO_VER36;
	case MODE_NONE:					return MODE_NONE_VER36;
	default:						return MODE_NONE_VER36;
	}
}
*/
/*=============================================================================
  �� �� ���� VideoDStreamTypeVer36ToVer40
  ��    �ܣ� ��Ƶ˫����������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byDStreamType36
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::VideoDStreamTypeVer36ToVer40( u8 byDStreamType36 )
{
	switch( byDStreamType36 )
	{
	case VIDEO_DSTREAM_H263PLUS_VER36:		return VIDEO_DSTREAM_H263PLUS;
	case VIDEO_DSTREAM_MAIN_VER36:			return VIDEO_DSTREAM_MAIN;
	case VIDEO_DSTREAM_H263PLUS_H239_VER36:	return VIDEO_DSTREAM_H263PLUS_H239;
	case VIDEO_DSTREAM_H263_H239_VER36:		return VIDEO_DSTREAM_H263_H239;
	case VIDEO_DSTREAM_H264_H239_VER36:		return VIDEO_DSTREAM_H264_H239;
	default:								return VIDEO_DSTREAM_MAIN;
	}
}
*/
/*=============================================================================
  �� �� ���� VideoDStreamTypeVer40ToVer36
  ��    �ܣ� ��Ƶ˫����������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byDStreamType40
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::VideoDStreamTypeVer40ToVer36( u8 byDStreamType40 )
{
	switch( byDStreamType40 )
	{
	case VIDEO_DSTREAM_H263PLUS:		return VIDEO_DSTREAM_H263PLUS_VER36;
	case VIDEO_DSTREAM_MAIN:			return VIDEO_DSTREAM_MAIN_VER36;
	case VIDEO_DSTREAM_H263PLUS_H239:	return VIDEO_DSTREAM_H263PLUS_H239_VER36;
	case VIDEO_DSTREAM_H263_H239:		return VIDEO_DSTREAM_H263_H239_VER36;
	case VIDEO_DSTREAM_H264_H239:		return VIDEO_DSTREAM_H264_H239_VER36;
	default:							return VIDEO_DSTREAM_MAIN_VER36;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfOpenModeVer36ToVer40
  ��    �ܣ� ���鿪�ŷ�ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfOpenMode36
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfOpenModeVer36ToVer40( u8 byConfOpenMode36 )
{
	switch( byConfOpenMode36 )
	{
	case CONF_OPENMODE_CLOSED_VER36:	return CONF_OPENMODE_CLOSED;
	case CONF_OPENMODE_NEEDPWD_VER36:	return CONF_OPENMODE_NEEDPWD;
	case CONF_OPENMODE_OPEN_VER36:		return CONF_OPENMODE_OPEN;
	default:							return CONF_OPENMODE_CLOSED;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfOpenModeVer40ToVer36
  ��    �ܣ� ���鿪�ŷ�ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfOpenMode40
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfOpenModeVer40ToVer36( u8 byConfOpenMode40 )
{
	switch( byConfOpenMode40 )
	{
	case CONF_OPENMODE_CLOSED:		return CONF_OPENMODE_CLOSED_VER36;
	case CONF_OPENMODE_NEEDPWD:		return CONF_OPENMODE_NEEDPWD_VER36;
	case CONF_OPENMODE_OPEN:		return CONF_OPENMODE_OPEN_VER36;
	default:						return CONF_OPENMODE_CLOSED_VER36;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfEncryptModeVer36ToVer40
  ��    �ܣ� ������ܷ�ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfEncryptMode36
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfEncryptModeVer36ToVer40( u8 byConfEncryptMode36 )
{	
	switch( byConfEncryptMode36 )
	{
	case CONF_ENCRYPTMODE_AES_VER36:	return CONF_ENCRYPTMODE_AES;
	case CONF_ENCRYPTMODE_DES_VER36:	return CONF_ENCRYPTMODE_DES;
	case CONF_ENCRYPTMODE_NONE_VER36:	return CONF_ENCRYPTMODE_NONE;
	default:							return CONF_ENCRYPTMODE_NONE;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfEncryptModeVer40ToVer36
  ��    �ܣ� ������ܷ�ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfEncryptMode40
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfEncryptModeVer40ToVer36( u8 byConfEncryptMode40 )
{
	switch( byConfEncryptMode40 )
	{
	case CONF_ENCRYPTMODE_AES:		return CONF_ENCRYPTMODE_AES_VER36;
	case CONF_ENCRYPTMODE_DES:		return CONF_ENCRYPTMODE_DES_VER36;
	case CONF_ENCRYPTMODE_NONE:		return CONF_ENCRYPTMODE_NONE_VER36;
	default:						return CONF_ENCRYPTMODE_NONE_VER36;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfUniformModeVer36ToVer40
  ��    �ܣ� �Ƿ�֧�ֹ�һ������ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfUniformMode36
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfUniformModeVer36ToVer40( u8 byConfUniformMode36 )
{
	switch( byConfUniformMode36 )
	{
	case CONF_UNIFORMMODE_NONE_VER36:	return CONF_UNIFORMMODE_NONE;
	case CONF_UNIFORMMODE_VALID_VER36:	return CONF_UNIFORMMODE_VALID;
	default:							return CONF_UNIFORMMODE_NONE;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfUniformModeVer40ToVer36
  ��    �ܣ� �Ƿ�֧�ֹ�һ������ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfUniformMode40
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfUniformModeVer40ToVer36( u8 byConfUniformMode40 )
{
	switch( byConfUniformMode40 )
	{
	case CONF_UNIFORMMODE_NONE:		return CONF_UNIFORMMODE_NONE_VER36;
	case CONF_UNIFORMMODE_VALID:	return CONF_UNIFORMMODE_VALID_VER36;
	default:						return CONF_UNIFORMMODE_NONE_VER36;
	}
}
*/
/*=============================================================================
  �� �� ���� SwitchChnnlUniformModeVer36ToVer40
  ��    �ܣ� ����ͨ���Ƿ�֧�ֹ�һ������ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 bySwitchChnnlUniformMode36
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::SwitchChnnlUniformModeVer36ToVer40( u8 bySwitchChnnlUniformMode36 )
{
	switch( bySwitchChnnlUniformMode36 )
	{
	case SWITCHCHANNEL_UNIFORMMODE_NONE_VER36:	return SWITCHCHANNEL_UNIFORMMODE_NONE;
	case SWITCHCHANNEL_UNIFORMMODE_VALID_VER36:	return SWITCHCHANNEL_UNIFORMMODE_VALID;
	default:									return SWITCHCHANNEL_UNIFORMMODE_NONE;
	}
}
*/
/*=============================================================================
  �� �� ���� SwitchChnnlUniformModeVer40ToVer36
  ��    �ܣ� ����ͨ���Ƿ�֧�ֹ�һ������ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 bySwitchChnnlUniformMode40
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::SwitchChnnlUniformModeVer40ToVer36( u8 bySwitchChnnlUniformMode40 )
{
	switch( bySwitchChnnlUniformMode40 )
	{
	case SWITCHCHANNEL_UNIFORMMODE_NONE:	return SWITCHCHANNEL_UNIFORMMODE_NONE_VER36;
	case SWITCHCHANNEL_UNIFORMMODE_VALID:	return SWITCHCHANNEL_UNIFORMMODE_VALID_VER36;
	default:								return SWITCHCHANNEL_UNIFORMMODE_NONE_VER36;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfDataModeVer36ToVer40
  ��    �ܣ� ���ݻ��鷽ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfDataMode36
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfDataModeVer36ToVer40( u8 byConfDataMode36 )
{
	switch( byConfDataMode36 )
	{
	case CONF_DATAMODE_VAONLY_VER36:	return CONF_DATAMODE_VAONLY;
	case CONF_DATAMODE_VAANDDATA_VER36:	return CONF_DATAMODE_VAANDDATA;
	case CONF_DATAMODE_DATAONLY_VER36:	return CONF_DATAMODE_DATAONLY;
	default:							return CONF_DATAMODE_VAONLY;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfDataModeVer40ToVer36
  ��    �ܣ� ���ݻ��鷽ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfDataMode40
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfDataModeVer40ToVer36( u8 byConfDataMode40 )
{
	switch( byConfDataMode40 )
	{
	case CONF_DATAMODE_VAONLY:		return CONF_DATAMODE_VAONLY_VER36;
	case CONF_DATAMODE_VAANDDATA:	return CONF_DATAMODE_VAANDDATA_VER36;
	case CONF_DATAMODE_DATAONLY:	return CONF_DATAMODE_DATAONLY_VER36;
	default:						return CONF_DATAMODE_VAONLY_VER36;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfReleaseModeVer36ToVer40
  ��    �ܣ� ���������ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfReleaseMode36
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfReleaseModeVer36ToVer40( u8 byConfReleaseMode36 )
{
	switch( byConfReleaseMode36 )
	{
	case CONF_RELEASEMODE_NOMT_VER36:	return CONF_RELEASEMODE_NOMT;
	case CONF_RELEASEMODE_NONE_VER36:	return CONF_RELEASEMODE_NONE;
	default:							return CONF_RELEASEMODE_NONE;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfReleaseModeVer40ToVer36
  ��    �ܣ� ���������ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfReleaseMode40
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfReleaseModeVer40ToVer36( u8 byConfReleaseMode40 )
{
	switch( byConfReleaseMode40 )
	{
	case CONF_RELEASEMODE_NOMT:		return CONF_RELEASEMODE_NOMT_VER36;
	case CONF_RELEASEMODE_NONE:		return CONF_RELEASEMODE_NONE_VER36;
	default:						return CONF_RELEASEMODE_NONE_VER36;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfTakeModeVer36ToVer40
  ��    �ܣ� ������з�ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfTakeMode36
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfTakeModeVer36ToVer40( u8 byConfTakeMode36 )
{	
	switch( byConfTakeMode36 )
	{
	case CONF_TAKEMODE_SCHEDULED_VER36:	return CONF_TAKEMODE_SCHEDULED;
	case CONF_TAKEMODE_ONGOING_VER36:	return CONF_TAKEMODE_ONGOING;
	case CONF_TAKEMODE_TEMPLATE_VER36:	return CONF_TAKEMODE_TEMPLATE;
	default:							return CONF_TAKEMODE_SCHEDULED;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfTakeModeVer40ToVer36
  ��    �ܣ� ������з�ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfTakeMode40
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfTakeModeVer40ToVer36( u8 byConfTakeMode40 )
{
	switch( byConfTakeMode40 )
	{
	case CONF_TAKEMODE_SCHEDULED:	return CONF_TAKEMODE_SCHEDULED_VER36;
	case CONF_TAKEMODE_ONGOING:		return CONF_TAKEMODE_ONGOING_VER36;
	case CONF_TAKEMODE_TEMPLATE:	return CONF_TAKEMODE_TEMPLATE_VER36;
	default:						return CONF_TAKEMODE_SCHEDULED_VER36;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfSpeakerSrcVer36ToVer40
  ��    �ܣ� �����˵�Դ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfSpeakerSrc36
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfSpeakerSrcVer36ToVer40( u8 byConfSpeakerSrc36 )
{
	switch( byConfSpeakerSrc36 )
	{
	case CONF_SPEAKERSRC_SELF_VER36:	return CONF_SPEAKERSRC_SELF;
	case CONF_SPEAKERSRC_CHAIR_VER36:	return CONF_SPEAKERSRC_CHAIR;
	case CONF_SPEAKERSRC_LAST_VER36:	return CONF_SPEAKERSRC_LAST;
	default:							return CONF_SPEAKERSRC_SELF;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfSpeakerSrcVer40ToVer36
  ��    �ܣ� �����˵�Դ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfSpeakerSrc40
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfSpeakerSrcVer40ToVer36( u8 byConfSpeakerSrc40 )
{
	switch( byConfSpeakerSrc40 )
	{
	case CONF_SPEAKERSRC_SELF:		return CONF_SPEAKERSRC_SELF_VER36;
	case CONF_SPEAKERSRC_CHAIR:		return CONF_SPEAKERSRC_CHAIR_VER36;
	case CONF_SPEAKERSRC_LAST:		return CONF_SPEAKERSRC_LAST_VER36;
	default:						return CONF_SPEAKERSRC_SELF_VER36;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfVmpModeVer36ToVer40
  ��    �ܣ� ����ϳɷ�ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfVmpMode36
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfVmpModeVer36ToVer40( u8 byConfVmpMode36 )
{
	switch( byConfVmpMode36 )
	{
	case CONF_VMPMODE_NONE_VER36:		return CONF_VMPMODE_NONE;
	case CONF_VMPMODE_CTRL_VER36:		return CONF_VMPMODE_CTRL;
	case CONF_VMPMODE_AUTO_VER36:		return CONF_VMPMODE_AUTO;
	default:							return CONF_VMPMODE_NONE;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfVmpModeVer40ToVer36
  ��    �ܣ� ����ϳɷ�ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfVmpMode40
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfVmpModeVer40ToVer36( u8 byConfVmpMode40 )
{
	switch( byConfVmpMode40 )
	{
	case CONF_VMPMODE_NONE:			return CONF_VMPMODE_NONE_VER36;
	case CONF_VMPMODE_CTRL:			return CONF_VMPMODE_CTRL_VER36;
	case CONF_VMPMODE_AUTO:			return CONF_VMPMODE_AUTO_VER36;
	default:						return CONF_VMPMODE_NONE_VER36;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfRecModeVer36ToVer40
  ��    �ܣ� ����¼��ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfRecMode36
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfRecModeVer36ToVer40( u8 byConfRecMode36 )
{
	switch( byConfRecMode36 )
	{
	case CONF_RECMODE_REC_VER36:		return CONF_RECMODE_REC;
	case CONF_RECMODE_PAUSE_VER36:		return CONF_RECMODE_PAUSE;
	case CONF_RECMODE_NONE_VER36:		return CONF_RECMODE_NONE;
	default:							return CONF_RECMODE_NONE;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfRecModeVer40ToVer36
  ��    �ܣ� ����¼��ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfRecMode40
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfRecModeVer40ToVer36( u8 byConfRecMode40 )
{
	switch( byConfRecMode40 )
	{
	case CONF_RECMODE_REC:			return CONF_RECMODE_REC_VER36;
	case CONF_RECMODE_PAUSE:		return CONF_RECMODE_PAUSE_VER36;
	case CONF_RECMODE_NONE:			return CONF_RECMODE_NONE_VER36;
	default:						return CONF_RECMODE_NONE_VER36;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfPlayModeVer36ToVer40
  ��    �ܣ� �������ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfPlayMode36
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfPlayModeVer36ToVer40( u8 byConfPlayMode36 )
{
	switch( byConfPlayMode36 )
	{
	case CONF_PLAYMODE_PLAY_VER36:		return CONF_PLAYMODE_PLAY;
	case CONF_PLAYMODE_PAUSE_VER36:		return CONF_PLAYMODE_PAUSE;
	case CONF_PLAYMODE_FF_VER36:		return CONF_PLAYMODE_FF;
	case CONF_PLAYMODE_FB_VER36:		return CONF_PLAYMODE_FB;
	case CONF_PLAYMODE_NONE_VER36:		return CONF_PLAYMODE_NONE;
	default:							return CONF_PLAYMODE_NONE;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfPlayModeVer40ToVer36
  ��    �ܣ� �������ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfPlayMode40
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfPlayModeVer40ToVer36( u8 byConfPlayMode40 )
{
	switch( byConfPlayMode40 )
	{
	case CONF_PLAYMODE_PLAY:		return CONF_PLAYMODE_PLAY_VER36;
	case CONF_PLAYMODE_PAUSE:		return CONF_PLAYMODE_PAUSE_VER36;
	case CONF_PLAYMODE_FF:			return CONF_PLAYMODE_FF_VER36;
	case CONF_PLAYMODE_FB:			return CONF_PLAYMODE_FB_VER36;
	case CONF_PLAYMODE_NONE:		return CONF_PLAYMODE_NONE_VER36;
	default:						return CONF_PLAYMODE_NONE_VER36;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfPollModeVer36ToVer40
  ��    �ܣ� ������ѯ��ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfPollMode36
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfPollModeVer36ToVer40( u8 byConfPollMode36 )
{	
	switch( byConfPollMode36 )
	{
	case CONF_POLLMODE_VIDEO_VER36:		return CONF_POLLMODE_VIDEO;
	case CONF_POLLMODE_SPEAKER_VER36:	return CONF_POLLMODE_SPEAKER;
	case CONF_POLLMODE_NONE_VER36:		return CONF_POLLMODE_NONE;
	default:							return CONF_POLLMODE_NONE;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfPlayModeVer40ToVer36
  ��    �ܣ� ������ѯ��ʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfPlayMode40
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfPollModeVer40ToVer36( u8 byConfPollMode40 )
{
	switch( byConfPollMode40 )
	{
	case CONF_POLLMODE_VIDEO:		return CONF_POLLMODE_VIDEO_VER36;
	case CONF_POLLMODE_SPEAKER:		return CONF_POLLMODE_SPEAKER_VER36;
	case CONF_POLLMODE_NONE:		return CONF_POLLMODE_NONE_VER36;
	default:						return CONF_POLLMODE_NONE_VER36;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfLockModeVer36ToVer40
  ��    �ܣ� ��������ģʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfLockMode36
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfLockModeVer36ToVer40( u8 byConfLockMode36 )
{
	switch( byConfLockMode36 )
	{
	case CONF_LOCKMODE_NEEDPWD_VER36:		return CONF_LOCKMODE_NEEDPWD;
	case CONF_LOCKMODE_LOCK_VER36:			return CONF_LOCKMODE_LOCK;
	case CONF_LOCKMODE_NONE_VER36:			return CONF_LOCKMODE_NONE;
	default:								return CONF_LOCKMODE_NONE;
	}
}
*/
/*=============================================================================
  �� �� ���� ConfLockModeVer40ToVer36
  ��    �ܣ� ��������ģʽ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byConfLockMode40
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::ConfLockModeVer40ToVer36( u8 byConfLockMode40 )
{
	switch( byConfLockMode40 )
	{
	case CONF_LOCKMODE_NEEDPWD:			return CONF_LOCKMODE_NEEDPWD_VER36;
	case CONF_LOCKMODE_LOCK:			return CONF_LOCKMODE_LOCK_VER36;
	case CONF_LOCKMODE_NONE:			return CONF_LOCKMODE_NONE_VER36;
	default:							return CONF_LOCKMODE_NONE_VER36;
	}
}
*/
/*=============================================================================
  �� �� ���� VmpMemberTypeVer36ToVer40
  ��    �ܣ� ����ϳɳ�Ա��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byVmpMemberType36
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::VmpMemberTypeVer36ToVer40( u8 byVmpMemberType36 )
{
	switch( byVmpMemberType36 )
	{
	case VMP_MEMBERTYPE_MCSSPEC_VER36:		return VMP_MEMBERTYPE_MCSSPEC;
	case VMP_MEMBERTYPE_SPEAKER_VER36:		return VMP_MEMBERTYPE_SPEAKER;
	case VMP_MEMBERTYPE_CHAIRMAN_VER36:		return VMP_MEMBERTYPE_CHAIRMAN;
	case VMP_MEMBERTYPE_POLL_VER36:			return VMP_MEMBERTYPE_POLL;
	case VMP_MEMBERTYPE_VAC_VER36:			return VMP_MEMBERTYPE_VAC;
	default:								return VMP_MEMBERTYPE_MCSSPEC;
	}
}
*/
/*=============================================================================
  �� �� ���� VmpMemberTypeVer40ToVer36
  ��    �ܣ� ����ϳɳ�Ա��������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byVmpMemberType40
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::VmpMemberTypeVer40ToVer36( u8 byVmpMemberType40 )
{
	switch( byVmpMemberType40 )
	{
	case VMP_MEMBERTYPE_MCSSPEC:		return VMP_MEMBERTYPE_MCSSPEC_VER36;
	case VMP_MEMBERTYPE_SPEAKER:		return VMP_MEMBERTYPE_SPEAKER_VER36;
	case VMP_MEMBERTYPE_CHAIRMAN:		return VMP_MEMBERTYPE_CHAIRMAN_VER36;
	case VMP_MEMBERTYPE_POLL:			return VMP_MEMBERTYPE_POLL_VER36;
	case VMP_MEMBERTYPE_VAC:			return VMP_MEMBERTYPE_VAC_VER36;
	default:							return VMP_MEMBERTYPE_MCSSPEC_VER36;
	}
}
*/
/*=============================================================================
  �� �� ���� VmpStyleTypeVer36ToVer40
  ��    �ܣ� ����ϳɷ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byVmpStyleType36
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::VmpStyleTypeVer36ToVer40( u8 byVmpStyleType36 )
{
	switch( byVmpStyleType36 )
	{
	case VMP_STYLE_DYNAMIC_VER36:		return VMP_STYLE_DYNAMIC;
	case VMP_STYLE_ONE_VER36:			return VMP_STYLE_ONE;
	case VMP_STYLE_VTWO_VER36:			return VMP_STYLE_VTWO;
	case VMP_STYLE_HTWO_VER36:			return VMP_STYLE_HTWO;
	case VMP_STYLE_THREE_VER36:			return VMP_STYLE_THREE;
	case VMP_STYLE_FOUR_VER36:			return VMP_STYLE_FOUR;
	case VMP_STYLE_SIX_VER36:			return VMP_STYLE_SIX;
	case VMP_STYLE_EIGHT_VER36:			return VMP_STYLE_EIGHT;
	case VMP_STYLE_NINE_VER36:			return VMP_STYLE_NINE;
	case VMP_STYLE_TEN_VER36:			return VMP_STYLE_TEN;
	case VMP_STYLE_THIRTEEN_VER36:		return VMP_STYLE_THIRTEEN;
	case VMP_STYLE_SIXTEEN_VER36:		return VMP_STYLE_SIXTEEN;
	case VMP_STYLE_SPECFOUR_VER36:		return VMP_STYLE_SPECFOUR;
	case VMP_STYLE_SEVEN_VER36:			return VMP_STYLE_SEVEN;
	default:							return VMP_STYLE_DYNAMIC;
	}
}
*/
/*=============================================================================
  �� �� ���� VmpStyleTypeVer40ToVer36
  ��    �ܣ� ����ϳɷ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byVmpStyleType40
  �� �� ֵ�� u8 
  -----------------------------------------------------------------------------
  �޸ļ�¼��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/01/03    4.0			�ű���                  ����
=============================================================================*/
/*u8 CMtAdpUtils::VmpStyleTypeVer40ToVer36( u8 byVmpStyleType40 )
{
	switch( byVmpStyleType40 )
	{
	case VMP_STYLE_DYNAMIC:			return VMP_STYLE_DYNAMIC_VER36;
	case VMP_STYLE_ONE:				return VMP_STYLE_ONE_VER36;
	case VMP_STYLE_VTWO:			return VMP_STYLE_VTWO_VER36;
	case VMP_STYLE_HTWO:			return VMP_STYLE_HTWO_VER36;
	case VMP_STYLE_THREE:			return VMP_STYLE_THREE_VER36;
	case VMP_STYLE_FOUR:			return VMP_STYLE_FOUR_VER36;
	case VMP_STYLE_SIX:				return VMP_STYLE_SIX_VER36;
	case VMP_STYLE_EIGHT:			return VMP_STYLE_EIGHT_VER36;
	case VMP_STYLE_NINE:			return VMP_STYLE_NINE_VER36;
	case VMP_STYLE_TEN:				return VMP_STYLE_TEN_VER36;
	case VMP_STYLE_THIRTEEN:		return VMP_STYLE_THIRTEEN_VER36;
	case VMP_STYLE_SIXTEEN:			return VMP_STYLE_SIXTEEN_VER36;
	case VMP_STYLE_SPECFOUR:		return VMP_STYLE_SPECFOUR_VER36;
	case VMP_STYLE_SEVEN:			return VMP_STYLE_SEVEN_VER36;
	default:						return VMP_STYLE_DYNAMIC_VER36;
	}
}*/

/*==============================================================================
������    :  GetResProduct
����      :  ��ȡ��Ӧ�ֱ��ʸ߿�֮�˻�
�㷨ʵ��  :  
����˵��  :  u8 byRes [in]�ֱ��ʸ�ʽ������4cif��720p��
����ֵ˵��:  u32	  [out]�ֱ��ʸ߿�֮�˻�
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2008-10-22   Ѧ��											����
==============================================================================*/
u32 CMtAdpUtils::GetResProduct(u8 byRes)
{
    u16 wWidth = 0;
    u16 wHeight = 0;
    
    switch(byRes)
    {
    case VIDEO_FORMAT_SQCIF_112x96:
        {
			wWidth = 112;
			wHeight = 96;
			break;
        }
    case VIDEO_FORMAT_SQCIF_96x80:
		{
			wWidth = 96;
			wHeight = 80;
			break;
        }
    case VIDEO_FORMAT_SQCIF:
        {
			wWidth = 128;
			wHeight = 96;
			break;
        }
    case VIDEO_FORMAT_QCIF:
        {
			wWidth = 176;
			wHeight = 144;
			break;
        }
    case VIDEO_FORMAT_CIF:
        {
			wWidth = 352;
			wHeight = 288;
			break;
        }
    case VIDEO_FORMAT_2CIF:
        {
			wWidth = 352;
			wHeight = 576;
			break;
        }
    case VIDEO_FORMAT_4CIF: //Ŀǰ��p�ƣ� 704*576
		{
			wWidth = 704;
			wHeight = 576;
			break;
        }
    case VIDEO_FORMAT_16CIF:
        {
			wWidth = 1048;
			wHeight = 1152;
			break;
        }
    case VIDEO_FORMAT_AUTO:
        //������
        break;
        
    case VIDEO_FORMAT_SIF:
        {
			wWidth = 352;
			wHeight = 240;
			break;
        }
    case VIDEO_FORMAT_2SIF:
        {
			wWidth = 352;
			wHeight = 480;
			break;
        }
    case VIDEO_FORMAT_4SIF:
        {
			wWidth = 704;
			wHeight = 480;
			break;
        }
    case VIDEO_FORMAT_VGA:
        {
			wWidth = 640;
			wHeight = 480;
			break;
        }
    case VIDEO_FORMAT_SVGA:
        {
			wWidth = 800;
			wHeight = 600;
			break;
        }
    case VIDEO_FORMAT_XGA:
        {
			wWidth = 1024;
			wHeight = 768;
			break;
		}
    case VIDEO_FORMAT_W4CIF:
        {
			wWidth = 1024;
			wHeight = 576;
			break;
		}
    case VIDEO_FORMAT_HD720:
        {
			wWidth = 1280;
			wHeight = 720;
			break;
		}
    case VIDEO_FORMAT_SXGA:
        {
			wWidth = 1280;
			wHeight = 1024;
			break;
		}
    case VIDEO_FORMAT_UXGA:
        {
			wWidth = 1600;
			wHeight = 1200;
			break;
		}
    case VIDEO_FORMAT_HD1080:
        {
			wWidth = 1920;
			wHeight = 1088;
			break;
		}
    default:
        break;
    }

    if ( 0 == wHeight || 0 == wWidth )
    {
        StaticLog( "[GetResProduct] res.%d, ignore it\n", byRes );
    }

	return (wWidth * wHeight);
   
}


/*==============================================================================
������    :  ResCmp
����      :  
�㷨ʵ��  :  
����˵��  :  u8 byRes1	[in] 
u8 byRes2	[in]	 
����ֵ˵��:  s8, byRes1 < byRes2,�򷵻�-1�� byRes1 = byRes2�򷵻�0��byRes1>byRes2 �򷵻�1;
����Ƚ϶�����VIDEO_FORMAT_AUTO�ģ�����2����û�бȽ�����
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2008-10-22
==============================================================================*/

s8 CMtAdpUtils::ResCmp(u8 byRes1, u8 byRes2)
{
	if( VIDEO_FORMAT_AUTO == byRes1 || VIDEO_FORMAT_AUTO == byRes2)
	{
		return 2;
	}
	
	u32 dwResProduct1 = GetResProduct(byRes1);
	u32 dwResProduct2 = GetResProduct(byRes2);
	
	if( dwResProduct1 <  dwResProduct2 )
	{
		return -1;
	}
	else if( dwResProduct1 >  dwResProduct2 )
	{
		return 1;
	}
	else 
	{
		return 0;
	}
	
}

/*==============================================================================
������    :  McuToMcuMtStatusVerR4V5ToVer40
����      :  mcu���������ն�״̬�ṹת��
�㷨ʵ��  :  
����˵��  : TMcuToMcuMtStatus &tMcuToMcuMtStatus ���ڵ��ն�״̬ 
			TMcuToMcuMtStatusBeforeV4R5 tMcuToMcuMtStatusBeforeV4R5 V4R5��ǰ�汾�ļ����ն�״̬
����ֵ˵��:  u32	  [out]�ֱ��ʸ߿�֮�˻�
-------------------------------------------------------------------------------
�޸ļ�¼  :  
��  ��       �汾          �޸���          �߶���          �޸ļ�¼
2010-04-26   �ܾ���											����
==============================================================================*/
void CMtAdpUtils::McuToMcuMtStatusVerR4V5ToVer40( TMcuToMcuMtStatus &tMcuToMcuMtStatus,
											 TMcuToMcuMtStatusBeforeV4R5 &tMcuToMcuMtStatusBeforeV4R5 
											 )
{
	tMcuToMcuMtStatusBeforeV4R5.SetIsEnableFECC( tMcuToMcuMtStatus.IsEnableFECC() );
	tMcuToMcuMtStatusBeforeV4R5.SetIsMixing( tMcuToMcuMtStatus.IsMixing() );
	tMcuToMcuMtStatusBeforeV4R5.SetIsVideoLose( tMcuToMcuMtStatus.IsVideoLose() );
	tMcuToMcuMtStatusBeforeV4R5.SetCurVideo( tMcuToMcuMtStatus.GetCurVideo() );
	tMcuToMcuMtStatusBeforeV4R5.SetCurAudio( tMcuToMcuMtStatus.GetCurAudio() );
	tMcuToMcuMtStatusBeforeV4R5.SetMtBoardType( tMcuToMcuMtStatus.GetMtBoardType() );
	tMcuToMcuMtStatusBeforeV4R5.SetPartId( tMcuToMcuMtStatus.GetPartId() );
	tMcuToMcuMtStatusBeforeV4R5.SetIsAutoCallMode( tMcuToMcuMtStatus.IsAutoCallMode() );
	tMcuToMcuMtStatusBeforeV4R5.SetSendVideo( tMcuToMcuMtStatus.IsSendVideo() );
	tMcuToMcuMtStatusBeforeV4R5.SetSendAudio( tMcuToMcuMtStatus.IsSendAudio() );
	tMcuToMcuMtStatusBeforeV4R5.SetRecvVideo( tMcuToMcuMtStatus.IsRecvVideo() );
	tMcuToMcuMtStatusBeforeV4R5.SetRecvAudio( tMcuToMcuMtStatus.IsRecvAudio() );

}

/*=============================================================================
    �� �� ���� IsIPTypeFromString
    ��    �ܣ� �Ӵ��б���ִ��б��Ƿ�Ϊip�ִ�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� s8* pszAliasString ���б���ִ�
    �� �� ֵ�� BOOL32
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/25   3.6			����                  ����
=============================================================================*/
BOOL32 CMtAdpUtils::IsIPTypeFromString(s8* pszAliasString)
{
	s32 anValue[4] = {0};
	s32 anLen[4] = {0};
	s32 nDot   = 0;
	s8 *pszTmp = pszAliasString;
	s32 nPos   = 0;
	for(nPos=0; *pszTmp&&nPos<16; nPos++,pszTmp++)
	{
		if( '.' == *pszTmp )
		{
			nDot++;
			//excude 1256.1.1.1.1
			if(nDot > 3)
			{
				return FALSE;
			}
			continue;			
		}
		//excude a.1.1.1
		if( *pszTmp<'0'|| *pszTmp>'9' )
		{
			return FALSE;
		}
		
		anValue[nDot] = anValue[nDot]*10 + (*pszTmp-'0');
		anLen[nDot]++;
	}
	
	//excude 1256.1.1.1234444
	if( nPos >=16 )
	{
		return FALSE;
	}
	
	//excude 0.1.1.1
	if( 0 == anValue[0] )
	{
		return FALSE;
	}
	
	for(nPos=0; nPos<4; nPos++)
	{
		//excude 1256.1.1.1
		if( (0 == anLen[nPos]) || (anLen[nPos] > 3) )
		{
			return FALSE;
		}
		//excude 256.1.1.1
		if(anValue[nPos] > 255)
		{
			return FALSE;
		}
	}

	return TRUE;
}

/*=============================================================================
    �� �� ���� IsIPTypeFromString
    ��    �ܣ� �Ӵ��б���ִ��б��Ƿ�ΪE164�ִ�
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� s8* pszAliasString ���б���ִ�
    �� �� ֵ�� BOOL32
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/25   3.6			����                  ����
=============================================================================*/
BOOL32 CMtAdpUtils::IsE164TypeFromString(s8 *pszSrc, s8* pszDst)
{
	s8 *pszSrcTmp = pszSrc;
	s8 *pszDstTmp = pszDst;
	while(*pszSrcTmp)
	{
		pszDstTmp = pszDst;
		while(*pszDstTmp)
		{
			if(*pszSrcTmp == *pszDstTmp) 
			{
				break;
			}
//			*pszDstTmp++;
            pszDstTmp++;
		}
		if( 0 == *pszDstTmp )
		{
			return FALSE;
		}
		pszSrcTmp++;
	}

	return TRUE;
}

/*=============================================================================
    �� �� ���� GetMtAliasTypeFromString
    ��    �ܣ� �Ӵ��б���ִ��б��ն�����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� s8* pszAliasString ���б���ִ�
    �� �� ֵ�� u8 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    2005/7/25   3.6			����                  ����
=============================================================================*/
u8 CMtAdpUtils::GetMtAliasTypeFromString(s8* pszAliasString)
{
	if(IsIPTypeFromString(pszAliasString))
	{	
		//ip
		return (u8)mtAliasTypeTransportAddress;
	}
	else if(IsE164TypeFromString(pszAliasString,"0123456789*,#"))
	{
		//e164
		return (u8)mtAliasTypeE164;
	}
	else
	{
		//h323
		return (u8)mtAliasTypeH323ID;
	}
	
//	return mtAliasTypeOthers;
}

/*=============================================================================
    �� �� ���� ConverTMt2TTERLABEL
    ��    �ܣ� ��TMT�ṹת����TTERLABEL�ṹ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� IN : const TMt &tMt ��Ҫת����TMT��Ϣ
	           OUT: TTERLABEL &tMtLabel ת�����TTERLABEL��Ϣ
			   IN : BOOL32 bForcLocal �Ƿ�ǿ�ƽ�McuNo����Ϊlocal
			        g_cMtAdpApp.m_wMcuNetId��Ϊlocal(192)������mcu���ն˽������ᱨ
					�Ǳ������ն���Ϣ�����������bForcLocalӦ��ΪTRUE
    �� �� ֵ�� BOOL32 TURE ת���ɹ��� FALSE ʧ�� 
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    20110527    4.6			���                  create
=============================================================================*/
BOOL32 CMtAdpUtils::ConverTMt2TTERLABEL( const TMt &tMt, TTERLABEL &tMtLabel, BOOL32 bForcLocal /*= TRUE*/ )
{
	if( bForcLocal == TRUE && tMt.GetMcuId() != 0 )
	{
		return tMtLabel.SetTerminalLabel( (u8)g_cMtAdpApp.m_wMcuNetId, tMt.GetMtId() );
	}
	else
	{
		return tMtLabel.SetTerminalLabel( (u8)tMt.GetMcuId(), tMt.GetMtId() );
	}
}

/*=============================================================================
    �� �� ���� ConverTTERLABEL2TMt
    ��    �ܣ� ��TTERLABEL�ṹת����TMT�ṹ
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� IN : TTERLABEL &tMtLabel ��Ҫת����TTERLABEL��Ϣ
	           OUT: TMt &tMt   ת�����TMt��Ϣ
			   IN : u8 byConfIdx ����Idx
			   IN : BOOL32 bForcLocal �Ƿ�ǿ�ƽ�McuId����ΪlocalIdx(19200)
			        �����ն˺�mcu�������ᱨ�Ǳ������ն���Ϣ������
					�����bForcLocalӦ��ΪTRUE
    �� �� ֵ�� void
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    20110527    4.6			���                  create
=============================================================================*/
void CMtAdpUtils::ConverTTERLABEL2TMt( /*const*/ TTERLABEL &tMtLabel, TMt &tMt, u8 byConfIdx,  BOOL32 bForcLocal /*= TRUE*/ )
{
	if( bForcLocal == TRUE )
	{
		tMt.SetMcuId( LOCAL_MCUIDX );
	}
	else
	{
		tMt.SetMcuId( tMtLabel.GetMcuNo() );
	}

	tMt.SetMtId( tMtLabel.GetTerNo() );
	tMt.SetConfIdx( byConfIdx );
}
/*=============================================================================
    �� �� ���� IsDynamicRes
    ��    �ܣ� �Ƿ�̬�ֱ���
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const u8 byRes �ֱ���
    �� �� ֵ�� BOOL32
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    20110618    4.6			��ʤ��                  create
=============================================================================*/
BOOL32 CMtAdpUtils::IsDynamicRes( const u8 byRes )
{
    BOOL32 bDynamicRes = FALSE;
    
    switch(byRes)
    {
    case VIDEO_FORMAT_CIF:
    case VIDEO_FORMAT_2CIF:
    case VIDEO_FORMAT_4CIF:
    case VIDEO_FORMAT_16CIF:
        
    case VIDEO_FORMAT_SIF:
    case VIDEO_FORMAT_2SIF:
    case VIDEO_FORMAT_4SIF:
        
    case VIDEO_FORMAT_W4CIF:
    case VIDEO_FORMAT_HD720:
    case VIDEO_FORMAT_HD1080:
        //Auto������
    case VIDEO_FORMAT_AUTO:
        bDynamicRes = TRUE;
        break;
        //DS������SQCIF
    case VIDEO_FORMAT_SQCIF_112x96:
    case VIDEO_FORMAT_SQCIF_96x80:
        break;
    default:
        break;
    }

	return bDynamicRes;    
}
/*=============================================================================
    �� �� ���� IsStaticRes
    ��    �ܣ� �Ƿ�̬�ֱ���
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const u8 byRes �ֱ���
    �� �� ֵ�� BOOL32
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    20110618    4.6			��ʤ��                  create
=============================================================================*/
BOOL32 CMtAdpUtils::IsStaticRes( const u8 byRes )
{
    BOOL32 bStaticRes = FALSE;
    
    switch(byRes)
    {
    case VIDEO_FORMAT_VGA:
    case VIDEO_FORMAT_SVGA:
    case VIDEO_FORMAT_XGA:
    case VIDEO_FORMAT_SXGA:
    case VIDEO_FORMAT_UXGA:
        bStaticRes = TRUE;
        break;
    default:
        break;
    }

	return bStaticRes;    
}
/*=============================================================================
    �� �� ���� IsStaticRes
    ��    �ܣ� �Ƿ�ͬΪ��̬��̬�ֱ��ʣ�˫����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� const u8 bySrcRes �ֱ���
               const u8 byDstRes
    �� �� ֵ�� BOOL32
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    20110804    4.6			��ʤ��                  create
=============================================================================*/
BOOL32 CMtAdpUtils::IsResIsSameType( const u8 bySrcRes, const u8 byDstRes)
{
    BOOL32 bMatched = FALSE;
    if (IsStaticRes(bySrcRes) && IsStaticRes(byDstRes))
    {
        bMatched = TRUE;
    }
    else if (IsDynamicRes(bySrcRes) && IsDynamicRes(byDstRes))
    {
        bMatched = TRUE;
    }
    return bMatched;
}
/*=============================================================================
    �� �� ���� AdjustDStreamFps
    ��    �ܣ� ����˫��֡��
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byFps ֡��
    �� �� ֵ�� u8 ������֡��
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    20110628    4.6			��ʤ��                  create
=============================================================================*/
u8 CMtAdpUtils::AdjustDStreamFps( u8 byFps)
{

    //h246˫��֡�ʣ�1-5/10/15/20/30
    u8 byDstFps = 0;
    if( byFps <= 5)
    {
        byDstFps = byFps;
    }
    else if( byFps <= 20)
    {
        byDstFps = (byFps/5)*5;
    }
    else if( byFps < 30)
    {
        byDstFps = 20;
    }
    else/* if( byFps >= 30 )*/
    {
        byDstFps = 30;
    }

    return byDstFps;
} 

/*=============================================================================
    �� �� ���� GetAACChnnNum
    ��    �ܣ� ��Mcu������ͨ����ת����Э��ͨ�ŵ�ͨ����
    �㷨ʵ�֣� 
    ȫ�ֱ����� 
    ��    ���� u8 byFps ֡��
    �� �� ֵ�� u8 ������֡��
-----------------------------------------------------------------------------
    �޸ļ�¼��
    ��  ��		�汾		�޸���		�߶���    �޸�����
    20110628    4.6			��ʤ��                  create
=============================================================================*/
TAACCap::emAACChnlCfg CMtAdpUtils::GetAACChnnNum(u8 byAudioTrackNum)
{
	TAACCap::emAACChnlCfg emAChnnl = TAACCap::emChnlCust;

	switch ( byAudioTrackNum )
	{
	case 1:
		emAChnnl = TAACCap::emChnl1;
		break;
	case 2:
		emAChnnl = TAACCap::emChnl2;
		break;
	case 3:
		emAChnnl = TAACCap::emChnl3;
		break;
	case 4:
		emAChnnl = TAACCap::emChnl4;
		break;
	case 5:
		emAChnnl = TAACCap::emChnl5;
		break;
	case 6:
		emAChnnl = TAACCap::emChnl5dot1;
		break;
	case 7:
		emAChnnl = TAACCap::emChnl7dot1;
		break;
	default:
		StaticLog("[GetAACChnnNum] unexpected audio track num %d!\n",byAudioTrackNum);
		break;
	}

	return emAChnnl;
}
/*=============================================================================
�� �� ���� MatchAudioCap
��    �ܣ� ƥ����Ƶ��ͬ������
�㷨ʵ�֣�
ȫ�ֱ����� 
��    ����  u8 byEncryMode,                       --�������ģʽ
			u8 byAudioCapNum,                     --��Ƶ��������
			TAudioTypeDesc *ptLocalAudioTypeDesc, --��Ƶ�����б�
			TCapSet *ptRemoteCapSet,              --Զ��������
			TMultiCapSupport &tCommonCap          --��ͬ������
				
				  
�� �� ֵ�� BOOL32:��Ƶ�Ƿ�ƥ��ɹ�  
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
20121221    4.7         ��־��                 create
=============================================================================*/
BOOL32 CMtAdpUtils::MatchAudioCap(   u8 byEncryMode,
									 u8 byAudioCapNum,
								     const TAudioTypeDesc *ptLocalAudioTypeDesc,
									 TCapSet *ptRemoteCapSet,
									 TMultiCapSupport &tCommonCap)
{
	//��Ƶ������Ϣ��ԭ��һ�������Ǵ�����������ṹ���е�����Ƶ�����е���Ƶ�����ֶ��У�
	//�����������ԭ���ĸ���Ƶ��������Ƶ�����ֶ��У����ֶ�ԭ����δʹ�ù�
	tCommonCap.SetMainAudioType(MEDIA_TYPE_NULL);
	u8  byAudioCapType = MEDIA_TYPE_NULL;
	u16 wAudioComType = 0;
	u8  byAudioTrackNum = 0;
	TAudioTypeDesc tAudioTypeDesc;
	TAACCap::emAACChnlCfg eLocalChnnl = TAACCap::emChnlCust;
	TAACCap::emAACChnlCfg eRemoteChnnl = TAACCap::emChnlCust;
	for (u8 byAudioCapIdx = 0;byAudioCapIdx < byAudioCapNum;byAudioCapIdx++,ptLocalAudioTypeDesc++)
	{
		if (ptLocalAudioTypeDesc == NULL)
		{
			StaticLog( "[MatchAudioCap]ptLocalAudioTypeDesc == NULL!So Return False!\n");
			return FALSE;
		}
		tAudioTypeDesc = *ptLocalAudioTypeDesc;
		byAudioCapType = tAudioTypeDesc.GetAudioMediaType();
		byAudioTrackNum = tAudioTypeDesc.GetAudioTrackNum();
		wAudioComType = CMtAdpUtils::GetComType( CMtAdpUtils::PayloadTypeIn2Out(byAudioCapType),byEncryMode);
		if (ptRemoteCapSet->IsExistsimultaneous(&wAudioComType, 1))
		{
			if (byAudioCapType == MEDIA_TYPE_AACLC ||
				byAudioCapType == MEDIA_TYPE_AACLD)
			{
				u16 wUnEncryPayload = CMtAdpUtils::PayloadTypeIn2Out(byAudioCapType);
				eLocalChnnl  = CMtAdpUtils::GetAACChnnNum(byAudioTrackNum);
				
				if(ptRemoteCapSet->GetAACCap(wUnEncryPayload) !=NULL)
				{
					eRemoteChnnl = ptRemoteCapSet->GetAACCap(wUnEncryPayload)->GetChnl();
				}
				else
				{
					StaticLog( "[MatchAudioCap]wUnEncryPayload.%d,GetAACCap return NULL,error!\n",wUnEncryPayload);

					continue;
				}

				if ( eLocalChnnl == TAACCap::emChnl2 && (0x02 & eRemoteChnnl))//֧��AACL˫����
				{
					tCommonCap.SetMainAudioType(byAudioCapType);
					tCommonCap.SetMainAudioTrackNum(byAudioTrackNum);
				}
				else if ( eLocalChnnl == TAACCap::emChnl1 && (0x01 & eRemoteChnnl) )//֧��AACL������
				{
					tCommonCap.SetMainAudioType(byAudioCapType);
					tCommonCap.SetMainAudioTrackNum(byAudioTrackNum);
				}
				else
				{
					StaticLog( "[MatchAudioCap]byAudioCapIdx(%d) Audiotype(%d) AudioComtype(%d) LocalChnnlNum:%d RemoteChnnlNum:%d!,So Break;\n",
						byAudioCapIdx,byAudioCapType,wAudioComType,eLocalChnnl,eRemoteChnnl);
					continue;
				}
			} 
			else
			{
				tCommonCap.SetMainAudioType(byAudioCapType);
				tCommonCap.SetMainAudioTrackNum(1);//������ʽĬ��Ϊ������
			}
			if (tCommonCap.GetMainAudioType() != MEDIA_TYPE_NULL)
			{
				return TRUE;//�ҵ�����ֱ�ӷ���
			}
		}
	}
	return FALSE;
}
// End File