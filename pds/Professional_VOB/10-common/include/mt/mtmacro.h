/*******************************************************************************
 *  ģ����   : MT                                                              *
 *  �ļ���   : mtmacro.h                                                       *
 *  ����ļ� :                                                                 *
 *  ʵ�ֹ��� : �ն˳��ú궨��                                                  *
 *  ����     : ������                                                          *
 *  �汾     : V3.0  Copyright(c) 1997-2005 KDC, All rights reserved.          *
 *                                                                             *
 *    =======================================================================  *
 *  �޸ļ�¼:                                                                  *
 *    ��  ��      �汾        �޸���      �޸�����                             *
 *  2005/8/6  x.0         ������      ����          *
 *                                                                             *
 *******************************************************************************/

#ifndef _MT_MACRO_H_
#define _MT_MACRO_H_

#include "mtconst.h"
#include <string.h>


#ifdef _MSC_VER
#define inline _inline
#endif

#define MT_OK 0
#define MT_ERR 0xFF

#define IN_PARAM
#define OUT_PARAM
	
#define  _OffSetOf(s,m)   (size_t)&(((s *)0)->m)
#define  _CountOf(array)  (sizeof(array)/sizeof(array[0]))
#define  _SizeOfMember( s ,m ) (size_t)(sizeof((((s *)0)->m)))

#define HEXCAR(  val ) ( ( (u8)(val) >= 0xA ) ? ('A'+ (u8)(val)-0xA) : ('0' + (u8)(val) )  )

// ָ���ж�
#define MT_ASSERT( p ) if ( p == NULL ) { ::OspPrintf( TRUE, FALSE, "[SYSTEM]Invalid Pointer(%s:(%d))!\n", __FILE__, __LINE__ ); return; }
#define MT_ASSERT_RET( p, ret ) if ( p == NULL ) { ::OspPrintf( TRUE, FALSE, "[SYSTEM]Invalid Pointer(%s:(%d))!\n", __FILE__, __LINE__ ); return ret; }

// �ͷ��ڴ�
#define MT_SAFE_DELETE( p ) if ( p != NULL ) { delete p; p = NULL; }
#define MT_SAFE_DELETE_ARRAY( p ) if ( p != NULL ) { delete []p; p = NULL; }


#define IPFORMAT     "%d.%d.%d.%d"
#define u32ToIP( ip )   (u8)(ntohl(ip)>>24),\
                        (u8)(ntohl(ip)>>16),\
                        (u8)(ntohl(ip)>>8), \
                        (u8)ntohl(ip) 
//�ж��Ƿ�Ϊ�鲥��ַ
#define IS_MCADDR( ip ) ( (ntohl(ip) &0xE0000000) == 0xE0000000 )

#define SEAL_ARRAY( array ,val) (array[sizeof(array)-1]=val)

//��ȫ�����ַ�����ּ�ڷ�ֹĿ�����������
//dst: Ŀ���ַ������ַ
//src: Դ�ַ���ָ��
#define SAFE_STRCPY( dst, src)\
{\
	strncpy(dst, src, sizeof(dst));\
	dst[sizeof(dst)-1] = '\0';\
}

//////////////////////////////////////////////////////////////////////////


#ifdef _LINUX_

#ifndef min
#define min(a,b) ((a)>(b)?(b):(a))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

#endif

#ifdef _LINUX_
#define mtRegCommand( fun ) OspRegCommand (#fun, (void*) fun, "")
#else
#define mtRegCommand( fun ) 
#endif

#ifndef ABS
#define ABS(x) ((x <0) ? -x: x )
#endif

//#ifdef _LINUX_
//	#ifndef _LINUX_CONFIG_DEBUG_
//	#define _LINUX_CONFIG_DEBUG_
//	#endif
// #else
	#ifdef _LINUX_CONFIG_DEBUG_
	#undef _LINUX_CONFIG_DEBUG_
	#endif
// #endif


#define CHECK_POINTER(module, p)  \
{                  \
	if (p == NULL) \
	{              \
		MtLogErr(module, "NULL Pointer, in %s,%d", __FILE__, __LINE__); \
		return MT_ERR; \
	}\
}

#define VOID_CHECK_POINTER(module, p)  \
{                  \
	if (p == NULL) \
	{              \
		MtLogErr(module, "NULL Pointer, in %s,%d", __FILE__, __LINE__); \
		return; \
	}\
}

#ifdef _LINUX_
#define _MT_STRICMP( str1, str2 ) strcasecmp( str1, str2 )
#else
#define _MT_STRICMP( str1, str2 ) stricmp( str1, str2 )
#endif

#define BOOL_DESCRIPTION( val )  ( (val) ? "TRUE" : "FALSE" )

// --------------- ����ͼ�����������غ� --------------- 
#define MT_HD_IMAGE_PARAM_COUNT         3      // ��������
#define MT_HD_IMAGE_PARAM_CVBS      (u8)0      // CVBS��ͼ�������������
#define MT_HD_IMAGE_PARAM_YPbPr0    (u8)1      // ����1��ͼ�������������
#define MT_HD_IMAGE_PARAM_YPbPr1    (u8)2      // ����2��ͼ�������������
#define MT_HD_IMAGE_PARAM_INVALID   (u8)0xFF   // ��Ч����
#define GetHdParamImageIndex( emMtVideoPort )\
( emMtVideoPort == emMtC3Vid ? MT_HD_IMAGE_PARAM_YPbPr0 : \
( emMtVideoPort == emMtC4Vid ? MT_HD_IMAGE_PARAM_YPbPr1 : \
( emMtVideoPort == emMtC5Vid ? MT_HD_IMAGE_PARAM_CVBS : MT_HD_IMAGE_PARAM_INVALID ) ) )

#define MT_AACLC_IDLE_CHAN_TYPE     (u8)1


// --------------- ��ϼ���غ궨�� --------------- 
#define MT_COMBINED_HELP_RED      0   // HELP(3s) + Red
#define MT_COMBINED_HELP_YLW      1   // HELP(3s) + YLW
#define MT_COMBINED_HELP_GRN      2   // HELP(3s) + GRN

// --------------- ����ģʽ�궨�� --------------- 
#define ETH_MODE_AUTO           (u32)0
#define ETH_MODE_10M_FULL       (u32)1
#define ETH_MODE_10M_HALF       (u32)2
#define ETH_MODE_100M_FULL      (u32)3
#define ETH_MODE_100M_HALF      (u32)4

// --------------- ������ʽ�궨�� --------------- 
#define MT_HD_IMAGE_AUTOADJUST           (u32)0
#define MT_HD_IMAGE_ORIGINSIZE           (u32)1

// --------------- ͼ����������Ĳ������� ------------------
#define MT_SCALE_BRIGHTNESS     (u32)0 /* �������ȣ�  C | YPbPr */
#define MT_SCALE_CONTRAST       (u32)1 /* ���öԱȶȣ�C | YPbPr */
#define MT_SCALE_HUE            (u32)2 /* ����ɫ�ȣ�  C */
#define MT_SCALE_SATURATION     (u32)3 /* ���ñ��Ͷȣ�C | YPbPr */

// --------------- Զ�˽��˲������� --------------- 
#define MT_DIALOG_PA           (u32)0 /*��������*/
#define MT_DIALOG_MC           (u32)1 /*�ƶ�����ͷ*/
#define MT_DIALOG_CP           (u32)2 /*��������ͷ*/

// --------------- PCMT��Ƶ����뷽ʽ���� --------------- 
#define PCMT_VCODEC_SOFTWARE           (u8)0 /*��������*/
#define PCMT_VCODEC_HARDWARE           (u8)1 /*Ӳ���Կ������*/
#define PCMT_VENCODE                   (u8)0 /*����*/
#define PCMT_VDECODE                   (u8)1 /*����*/

// --------------- ����1080i--֡������ --------------- 
#define MT_HD_1080i_50                   (u32)0
#define MT_HD_1080i_60                   (u32)1
#define MT_HD_1080i_AUTO                 (u32)2

// --------------- ����1080P--֡������ --------------- 
#define MT_HD_1080p_24                   (u32)0
#define MT_HD_1080p_25                   (u32)1
#define MT_HD_1080p_30                   (u32)2
#define MT_HD_1080p_AUTO                 (u32)3
#define MT_HD_1080p_1080i                (u32)4

// --------------- VGA--Ƶ������ --------------- 
#define MT_HD_VGA_60                     (u32)0
#define MT_HD_VGA_75                     (u32)1

#endif//!_MT_MACRO_H_

