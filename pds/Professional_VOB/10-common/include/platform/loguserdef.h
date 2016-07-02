/*****************************************************************************
   ģ����      : kdvlog
   �ļ���      : logusrdef.h
   ����ļ�    : kdvlog.h
   �ļ�ʵ�ֹ���: ģ��ID�Ķ���
   ע������	   : ��Ҫ�����ģ��ID, ���mcu����ϵ
   ����        : ����
   �汾        : V4.6  Copyright(C) 2001-2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2011/04/5   4.6          ����        ����
******************************************************************************/

#ifndef KDV_LOG_USER_DEF_H
#define KDV_LOG_USER_DEF_H

#include "kdvtype.h"
#include "kdvlog.h"

// ģ������ʼֵ����, ��0��ʼ
#define MID_START						(u16)0

/************************************************************************/
/* ��ģ�鷶Χֵ����                                                     */
/************************************************************************/
// ������������ʼֵ, ��0��ʼ, ��Χ[0�� 99]
#define MID_PUBLIC_START				(u16)(MID_START + 1)

// mcuģ�����ʼֵ, ��100��ʼ,��Χ[100,599]
#define MID_MCU_START					(u16)(MID_PUBLIC_START + 100)
// ���ģ�����ʼֵ, ��600��ʼ,��Χ[600,799]
#define MID_MCS_START					(u16)(MID_MCU_START + 500)
// �ն���ʼֵ, ��800��ʼ,��Χ[800,999]
#define MID_MT_START					(u16)(MID_MCS_START + 200)
// �������ʼֵ, ��5000��ʼ,��Χ[5000,6000]
#define MID_PERI_START					(u16)(MID_START + 5000)

/************************************************************************/
/*  ������������                                                        */
/************************************************************************/
MODULE_DEF( MID_PUB_ALWAYS,			MID_PUBLIC_START + 0 );		// �κ��������ӡ

/************************************************************************/
/* MCUģ����������                                                      */
/************************************************************************/
MODULE_DEF( MID_MCU_MCS,			MID_MCU_START + 0 );		// pmcsmsg
MODULE_DEF( MID_MCU_VCS,			MID_MCU_START + 1 );		// pvcsmsg
MODULE_DEF( MID_MCU_EQP,			MID_MCU_START + 2 );		// peqpmsg
MODULE_DEF( MID_MCU_REC,			MID_MCU_START + 3 );		// peqpmsg
MODULE_DEF( MID_MCU_HDU,			MID_MCU_START + 4 );		// peqpmsg

MODULE_DEF( MID_MCU_VMP,			MID_MCU_START + 5 );		// peqpmsg
MODULE_DEF( MID_MCU_BAS,			MID_MCU_START + 6 );		// peqpmsg
MODULE_DEF( MID_MCU_PRS,			MID_MCU_START + 7 );		// peqpmsg
MODULE_DEF( MID_MCU_MIXER,			MID_MCU_START + 8 );		// peqpmsg

MODULE_DEF( MID_MCU_SPY,			MID_MCU_START + 16 );		// pspymsg
MODULE_DEF( MID_MCU_MMCU,			MID_MCU_START + 17 );		// pmmcumsg
MODULE_DEF( MID_MCU_DCS,			MID_MCU_START + 18 );		// pdcsmsg
MODULE_DEF( MID_MCU_MT	,			MID_MCU_START + 19 );		// pmtmsg
MODULE_DEF( MID_MCU_MT2,			MID_MCU_START + 20 );		// pmt2msg

MODULE_DEF( MID_MCU_CALL,			MID_MCU_START + 21 );		// pcallmsg
MODULE_DEF( MID_MCU_MPMGR,			MID_MCU_START + 22 );		// pmpmgrmsg
MODULE_DEF( MID_MCU_CFG,			MID_MCU_START + 23 );		// pcfgmsg
MODULE_DEF( MID_MCU_GUARD,			MID_MCU_START + 24 );		// pgdmsg
MODULE_DEF( MID_MCU_NPLUS,			MID_MCU_START + 25 );		// pnplusmsg

MODULE_DEF( MID_MCU_PFM,			MID_MCU_START + 26 );		// ppfmmsg
MODULE_DEF( MID_MCU_MSMGR,			MID_MCU_START + 27 );		// CMSManagerSsnInst

MODULE_DEF( MID_MCU_CONF,			MID_MCU_START + 40 );		// ConfLog

// mcu����lib���ӡ
MODULE_DEF( MID_MCULIB_MTADP,		MID_MCU_START + 100 );		// MtAdp log
MODULE_DEF( MID_MCULIB_MCUAGT,		MID_MCU_START + 110 );		// MCU Agent log
MODULE_DEF( MID_MCULIB_MMCULIB,		MID_MCU_START + 111 );		// MMcuLib log ������

/************************************************************************/
/* ����ID����                                                         */
/************************************************************************/

// ¼���������ʼֵ, ��1000��ʼ,��Χ[1000,1019]
#define MID_RECEQP_START			(u16)(MID_START + 1000)
// BAS�������ʼֵ, ��1020��ʼ,��Χ[1020,1039]
#define MID_BASEQP_START			(u16)(MID_RECEQP_START + 20)
// HDU�������ʼֵ, ��1040��ʼ,��Χ[1040,1059]
#define MID_HDUEQP_START			(u16)(MID_BASEQP_START + 20)
// VMP�������ʼֵ, ��1060��ʼ,��Χ[1060,1079]
#define MID_VMPEQP_START			(u16)(MID_HDUEQP_START + 20)
// BAS�������ʼֵ, ��1080��ʼ,��Χ[1080,1099]
#define MID_MIXEQP_START			(u16)(MID_VMPEQP_START + 20)
// TVW�������ʼֵ, ��1100��ʼ,��Χ[1100,1119]
#define MID_TVWEQP_START			(u16)(MID_MIXEQP_START + 20)
// TVW�������ʼֵ, ��1120��ʼ,��Χ[1120,1139]
#define MID_PRSEQP_START			(u16)(MID_TVWEQP_START + 20)
// SNMP��Ԫ��ʼֵ, ��1140��ʼ, ��Χ[1140, 1159]
#define MID_SNMP_START				(u16)(MID_PRSEQP_START + 20)

// ¼�����ID����
MODULE_DEF( MID_RECEQP_COMMON,		MID_RECEQP_START + 0 );			// ͨ��
MODULE_DEF( MID_RECEQP_ASFLIB,		MID_RECEQP_START + 3 );			// asflib��
MODULE_DEF( MID_RECEQP_RPCTRL,		MID_RECEQP_START + 6 );			// rpctrl��
MODULE_DEF( MID_RECEQP_REC,			MID_RECEQP_START + 9 );			// ¼����
MODULE_DEF( MID_RECEQP_PLAY,		MID_RECEQP_START + 12 );		// ������
MODULE_DEF( MID_RECEQP_RECSERVER,	MID_RECEQP_START + 15 );		// ¼�������

// Bas����
MODULE_DEF( MID_BASEQP_COMMON,		MID_BASEQP_START + 0 );			// ͨ��

// HDU����
MODULE_DEF( MID_HDUEQP_COMMON,		MID_HDUEQP_START + 0 );			// ͨ��

// VMP����
MODULE_DEF( MID_VMPEQP_COMMON,		MID_VMPEQP_START + 0 );			// ͨ��
MODULE_DEF( MID_VMPEQP_DRAWBMP,		MID_VMPEQP_START + 1 );			//drawBmpģ��

// MIX����
MODULE_DEF( MID_MIXEQP_COMMON,		MID_MIXEQP_START + 0 );			// ͨ��

// MIX����
MODULE_DEF( MID_TVWEQP_COMMON,		MID_TVWEQP_START + 0 );			// ͨ��

// PRS����
MODULE_DEF( MID_PRSEQP_COMMON,		MID_PRSEQP_START + 0 );			// ͨ��

// SNMP��Ԫ 
MODULE_DEF( MID_SNMP_EQPAGT,		MID_SNMP_START + 0 );			// ͨ��

//CNS
MODULE_DEF (MID_CNS_1,				MID_MT_START + 1 );			//802			
MODULE_DEF (MID_CNS_2,				MID_MT_START + 2 );			//803	
MODULE_DEF (MID_CNS_3,				MID_MT_START + 3 );			//804
MODULE_DEF (MID_CNS_4,				MID_MT_START + 4 );			//805
MODULE_DEF (MID_CNS_5,				MID_MT_START + 5 );			//806	
MODULE_DEF (MID_CNS_6,				MID_MT_START + 6 );			//807
MODULE_DEF (MID_CNS_7,				MID_MT_START + 7 );			//808
MODULE_DEF (MID_CNS_8,				MID_MT_START + 8 );			//809	
MODULE_DEF (MID_CNS_9,				MID_MT_START + 9 );			//810
MODULE_DEF (MID_CNS_10,				MID_MT_START + 10 );		//811
MODULE_DEF (MID_CNS_11,				MID_MT_START + 11 );		//812			
MODULE_DEF (MID_CNS_12,				MID_MT_START + 12 );		//813	
MODULE_DEF (MID_CNS_13,				MID_MT_START + 13 );		//814
MODULE_DEF (MID_CNS_14,				MID_MT_START + 14 );		//815
MODULE_DEF (MID_CNS_15,				MID_MT_START + 15 );		//816	
MODULE_DEF (MID_CNS_16,				MID_MT_START + 16 );		//817
MODULE_DEF (MID_CNS_17,				MID_MT_START + 17 );		//818
MODULE_DEF (MID_CNS_18,				MID_MT_START + 18 );		//819	
MODULE_DEF (MID_CNS_19,				MID_MT_START + 19 );		//820
MODULE_DEF (MID_CNS_20,				MID_MT_START + 20 );		//821

//UMS
MODULE_DEF (MID_UMS_1,				MID_MT_START + 51 );			//852			
MODULE_DEF (MID_UMS_2,				MID_MT_START + 52 );			//853	
MODULE_DEF (MID_UMS_3,				MID_MT_START + 53 );			//854
MODULE_DEF (MID_UMS_4,				MID_MT_START + 54 );			//855
MODULE_DEF (MID_UMS_5,				MID_MT_START + 55 );			//856	
MODULE_DEF (MID_UMS_6,				MID_MT_START + 56 );			//857
MODULE_DEF (MID_UMS_7,				MID_MT_START + 57 );			//858
MODULE_DEF (MID_UMS_8,				MID_MT_START + 58 );			//859	
MODULE_DEF (MID_UMS_9,				MID_MT_START + 59 );			//860
MODULE_DEF (MID_UMS_10,				MID_MT_START + 60 );		//861
MODULE_DEF (MID_UMS_11,				MID_MT_START + 61 );		//862			
MODULE_DEF (MID_UMS_12,				MID_MT_START + 62 );		//863	
MODULE_DEF (MID_UMS_13,				MID_MT_START + 63 );		//864
MODULE_DEF (MID_UMS_14,				MID_MT_START + 64 );		//865
MODULE_DEF (MID_UMS_15,				MID_MT_START + 65 );		//866	
MODULE_DEF (MID_UMS_16,				MID_MT_START + 66 );		//867
MODULE_DEF (MID_UMS_17,				MID_MT_START + 67 );		//868
MODULE_DEF (MID_UMS_18,				MID_MT_START + 68 );		//869	
MODULE_DEF (MID_UMS_19,				MID_MT_START + 69 );		//870
MODULE_DEF (MID_UMS_20,				MID_MT_START + 70 );		//871

#endif
//end of file