/*****************************************************************************
   ģ����      : ҵ���·���
   �ļ���      : vctopo.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: MCU���˹����������壬���ļ����к�������topo��ͷ
   ����        : ����
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2002/07/15  0.9         ����        ����
******************************************************************************/

#ifndef _VCTOPO_H_
#define _VCTOPO_H_

#include "mcuvc.h"

//������������
const u8 DIRECTION_UP		= 1;	//������������
const u8 DIRECTION_DOWN		= 2;	//������������
const u8 DIRECTION_TOPMCU	= 3;	//���½�������ת���ص�
const u8 DIRECTION_NONWAY	= 4;	//���ڽ���·����

//MCU���������ļ�
#define FILENAME_TOPOCFG				(LPCSTR)"topocfg.ini"
/*mcuTopoTable*/
#define SECTION_mcuTopoTable			(LPCSTR)"mcuTopoTable"
#define FILED_McuId						(LPCSTR)"McuId"
#define FILED_SuperiorMcuId				(LPCSTR)"SuperiorMcuId"
#define FILED_McuIpAddr					(LPCSTR)"McuIpAddr"
#define FILED_McuGwIpAddr				(LPCSTR)"McuGwIpAddr"
#define FILED_McuAlias					(LPCSTR)"McuAlias"
/*mtTopoTable*/
#define SECTION_mtTopoTable				(LPCSTR)"mtTopoTable"
#define FILED_MtId						(LPCSTR)"MtId"
#define FILED_McuId						(LPCSTR)"McuId"
#define FILED_MtIpAddr					(LPCSTR)"MtIpAddr"
#define FILED_MtGwIpAddr				(LPCSTR)"MtGwIpAddr"
#define FILED_MtAlias					(LPCSTR)"MtAlias"
#define FILED_PrimaryMt					(LPCSTR)"PrimaryMt"
#define FILED_MtType					(LPCSTR)"MtType"
#define FILED_MtConnected				(LPCSTR)"MTConnect"

/*====================================================================
    ������      ��topoGetAllTopoSubMcu
    ����        ����ȡ�������˽ṹ�и�MCU�������¼�MCU(��ѡһ������в�)
    �㷨ʵ��    ���ݹ�
    ����ȫ�ֱ�������
    �������˵����WORD wMcuId, �û������McuId
				  u16 wMcuTopo[], ���飬���ڴ�Ų��ҵĽ��
				  u8 bySize, ����Ԫ�ظ���
				  TMcuTopo atMcuTopo[], MCU��������
				  u16 wMcuTopoNum, ȫ����MCU��Ŀ
				  u8 byMode, ���ʲ�����0��ʾ���в㣬1��ʾһ�㣬ȱʡΪ0
    ����ֵ˵��  ��ʵ����Ŀ
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/08    1.0         ����ƽ        ����
    02/09/19    1.0         LI Yi         ��CMcuAgent���ƹ���
    02/10/24    1.0         LI Yi         ��Ӳ���byMode
====================================================================*/
 u16 topoGetAllTopoSubMcu( u16    wMcuId,
                                 u16    wSubMcu[],
                                 u8  bySize, 
							     TMcuTopo atMcuTopo[],
                                 u16    wMcuTopoNum,
                                 u8  byMode = 0 );
/*====================================================================
    ������      ��topoGetMcuTopo
    ����        ����ȡָ��MCU����TOP MCU�������·��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId, �û������McuId
				  u16 wMcuTopo[], ���飬���ڴ�Ų��ҵĽ������ָ��MCU
						          ��TOP MCU����"3,2,1"�� 0��β
				  u8 bySize, ����Ԫ�ظ�����Ҫ��С��5
				  TMcuTopo atMcuTopo[], MCU��������
				  u16 wMcuTopoNum, ȫ����MCU��Ŀ
    ����ֵ˵��  ���ɹ�����TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/08/02    1.0         ����ƽ        ����
    02/09/19    1.0         LI Yi         ��CMcuAgent���ƹ���
====================================================================*/
  BOOL topoGetMcuTopo( u16     wMcuId,
                            u16     wMcuTopo[],
                            u8   bySize, 
					        TMcuTopo atMcuTopo[],
                            u16     wMcuTopoNum );

/*====================================================================
    ������      ��topoGetInterMcu
    ����        �������ԴMCU������Ŀ��MCU����һվֱ��MCU��
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wSrcMcu, ԴMCU��
				  u16 wDstMcu, Ŀ��MCU��
				  TMcuTopo atMcuTopo[], MCU��������
				  u16 wMcuTopoNum, ȫ����MCU��Ŀ
    ����ֵ˵��  ������ֱ��MCU�ţ�0xffff��ʾ��һվΪԴMCU���ϼ�MCU��
				  ʧ�ܷ���NULL
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/07/30    1.0         LI Yi         ����
====================================================================*/
  u16 topoGetInterMcu( u16 wSrcMcu, u16 wDstMcu, TMcuTopo atMcuTopo[], u16 wMcuTopoNum );

/*====================================================================
    ������      ��topoFindMcuPath
    ����        �������ԴMCU������Ŀ��MCU��·
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wSrcMcu, ԴMCU��
				  u16 wDstMcu, Ŀ��MCU��
				  u16 awMcuPath[], ���ص�·��BUFFER����Դ��Ŀ��MCU��
						��"1,2,3"��0��β
				  u8 byBufSize, BUFFER��С
				  TMcuTopo atMcuTopo[], MCU��������
				  u16 wMcuTopoNum, ȫ����MCU��Ŀ
    ����ֵ˵��  ���ɹ�����TRUE��ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/09/19    1.0         LI Yi         ����
====================================================================*/
  BOOL topoFindMcuPath( u16 wSrcMcu, u16 wDstMcu, u16 awMcuPath[], u8 byBufSize, 
					         TMcuTopo atMcuTopo[], u16 wMcuTopoNum );

/*====================================================================
    ������      ��topoIsCollide
    ����        ���ж��û����ڿ��������նˣ������裩�Ƿ����MCU����
					�����ĸı�����·��ͻ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wCurSrcMcuId, �û���ѡ���������ն˻���������MCU��
				  u16 wConnMcuId, �û����ӵ�MCU��
				  u16 wSrcMcuId, ��·�ı��Դ�ն˻���������MCU��
				  u16 wDstMcuId, ��·�ı��Ŀ���ն˻���������MCU��
				  TMcuTopo atMcuTopo[], MCU��������
				  u16 wMcuTopoNum, ȫ����MCU��Ŀ
    ����ֵ˵��  ���г�ͻ����TRUE����֮FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/09/19    1.0         LI Yi         ����
====================================================================*/
  BOOL topoIsCollide( u16 wCurSrcMcuId,
                           u16 wConnMcuId,
                           u16 wSrcMcuId, 
					       u16 wDstMcuId,
                           TMcuTopo atMcuTopo[],
                           u16      wMcuTopoNum );
/*====================================================================
    ������      ��topoGetMcuInfo
    ����        ���õ���Ҫ��Mcu��TMcuTopo�ṹ��
				  �û����Լ���֤�����MCU����������ʵ�ʴ���
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId,Ҫ���MCU��
				  TMcuTopo atMcuTopo[]��MCU�������� 
				  u16 wMcuTopoNum��ȫ���е�MCU��Ŀ
    ����ֵ˵��  ���ɹ�����TMcuTopo�����򷵻�һ���ڲ�ȫΪ0��TMcuTopo�ṹ
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/25    1.0         Liaoweijiang  ����
====================================================================*/
  TMcuTopo topoGetMcuInfo( u16 wMcuId,  
					            TMcuTopo atMcuTopo[], u16 wMcuTopoNum );

/*====================================================================
    ������      ��topoGetMtInfo
    ����        ���õ���Ҫ��Mt��TMtTopo�ṹ
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId��MT���ڵ�MCU��
				  u8 byMtId��MT��MCU�еı��
				  TMcuTopo atMtTopo[]��Mt����������
				  u16 wMtTopoNum��ȫ����Mt����Ŀ
    ����ֵ˵��  ���ɹ�����TMtTopo�����򷵻�һ��Ϊȫ0��TMtTopo�ṹ
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/25    1.0         Liaoweijiang  ����
====================================================================*/
  TMtTopo topoGetMtInfo( u16 wMcuId, u8 byMtId, 
					          TMtTopo atMtTopo[], u16 wMtTopoNum );

/*====================================================================
    ������      ��topoIsSecondaryMt
    ����        ���ж�һ��Mt�Ƿ��Ǵμ��ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId��MT���ڵ�MCU��
				  u8 byMtId����������MCU�е�MT��
				  TMtTopo atMtTopo[]��MT����������
				  u16 wMtTopoNum��ȫ��MT����Ŀ
    ����ֵ˵��  ������Ƿ���TRUE, ����FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/25    1.0         Liaoweijiang  ����
====================================================================*/
  BOOL topoIsSecondaryMt( u16 wMcuId, u8 byMtId, 
					           TMtTopo atMtTopo[], u16 wMtTopoNum );
/*====================================================================
    ������      ��topoHasSecondaryMt
    ����        ���ж�һ��Mt�Ƿ��дμ��ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId��MT���ڵ�MCU��
				  u8 byMtId��MT�ڴ�MCU�еĺ�
				  TMtTopo atMtTopo[]��ȫ����MT��������
				  u16 wMtTopoNum��ȫ���е�MT��Ŀ
    ����ֵ˵��  ���ɹ�����TRUE, ����FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/25    1.0         Liaoweijiang  ����
====================================================================*/
  BOOL topoHasSecondaryMt( u16 wMcuId, u8 byMtId, 
					            TMtTopo atMtTopo[], u16 wMtTopoNum );

/*====================================================================
    ������      ��topoGetPrimaryMt
    ����        ���õ�һ���ն˵����ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId��MT���ڵ�MCU��
				  u8 byMtId��MT�ڴ�MCU�еĺ�
				  TMtTopo atMtTopo[]��ȫ����MT��������
				  u16 wMtTopoNum��ȫ���е�MT��Ŀ
    ����ֵ˵��  ���ɹ������������ն˺�,����0˵�����������նˣ���Ƿ��ն�
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/25    1.0         Liaoweijiang  ����
====================================================================*/
  u8 topoGetPrimaryMt( u16 wMcuId, u8 byMtId, 
					           TMtTopo atMtTopo[],  u16  wMtTopoNum );

/*====================================================================
    ������      ��topoGetExcludedMt
    ����        ���õ�һ���ն˵����л����ն�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId��MT���ڵ�MCU��
				  u8 byMtId��MT�ڴ�MCU�еĺ�
				  TMtTopo atMtTopo[]��ȫ����MT��������
				  u16 wMtTopoNum��ȫ���е�MT��Ŀ
				  u8 MtBuffer[]��������׼��װ�����ն˺ŵ�����
				  u16 BufferSize������������Ĵ�С
    ����ֵ˵��  ���ɹ�����ʵ�ʵ��ն���Ŀ, ����NULL
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/25    1.0         Liaoweijiang  ����
====================================================================*/
  u16 topoGetExcludedMt( u16 wMcuId, u8 byMtId, 
					          TMtTopo atMtTopo[], u16 wMtTopoNum,
                              u8 MtBuffer[], u16 wBufferSize);

/*====================================================================
    ������      ��topoGetAllSubMt
    ����        ���õ�һ��Mcu�µ�����Mt
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId : MCU��
				  TMtTopo atMtTopo[] : Mt����������
				  u16 wMtTopoNum��ȫ��MT����Ŀ
				  TMt MtBuffer[], ׼�����õõ�MT��TMt�ṹ������
				  u16 wBufferSize, �����С
				  byMode ����ģʽ
					Ϊ0,��ʾֻ�õ�������MT
					Ϊ1,��ʾ���Եõ���һ��MCU��MT
					Ϊ2,��ʾ�õ����¶���MCU��MT
    ����ֵ˵��  ��ʵ�ʵõ���MT����Ŀ
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/10/25    1.0         Liaoweijiang  ����
====================================================================*/
  u16 topoGetAllSubMt( u16 wMcuId, TMtTopo atMtTopo[], u16 wMtTopoNum, 
							TMt MtBuffer[], u16 wBufferSize, u8 byMode = 0 );

/*====================================================================
    ������      ��topoGetTopMcuBetweenTwoMcu
    ����        ���õ�����MCU��������ߵ��MCU
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId1 : MCU��
				  u16 wMcuId2 : MCU��
				  TMtTopo atMtTopo[] : Mcu����������
				  u16 wMtTopoNum��ȫ��Mcu����Ŀ
    ����ֵ˵��  ������MCU��������ߵ��MCU��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/11/07    1.0         Liaoweijiang  ����
====================================================================*/
  u16 topoGetTopMcuBetweenTwoMcu ( u16 wMcuId1, u16 wMcuId2,
										TMcuTopo atMcuTopo[], u16 wMcuTopoNum );

/*====================================================================
    ������      ��topoGetTopMcu
    ����        ���õ�����MCU��������ߵ��Mcu
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId[] : MCU������
				  u8 byMcuNum : �����С
				  TMtTopo atMtTopo[] : Mcu����������
				  u16 wMtTopoNum��ȫ��Mcu����Ŀ
    ����ֵ˵��  ��������ߵ��Mcu
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/11/07    1.0         Liaoweijiang  ����
====================================================================*/
  u16 topoGetTopMcu( u16 wMcuId[], u8 byMcuNum, 
						  TMcuTopo atMcuTopo[], u16 wMcuTopoNum );

/*====================================================================
    ������      ��topoJudgeLegality
    ����        �����˽ṹ�Ϸ����ж�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����TMcuTopo atMcuTopo[], Mcu����������
				  u16 wMcuTopoNum, ȫ��Mcu����Ŀ
				  TMtTopo atMtTopo[] : Mt����������
				  u16 wMtTopoNum��ȫ��Mt����Ŀ
    ����ֵ˵��  ��������ߵ��Mcu
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/11/13    1.0         Liaoweijiang  ����
====================================================================*/
BOOL topoJudgeLegality( TMcuTopo atMcuTopo[], u16 wMcuTopoNum,
							   TMtTopo atMtTopo[], u16 wMtTopoNum );

/*====================================================================
    ������      ��topoIsValidMt
    ����        �������ն˺ţ�IP��ַ�ͼ������кϷ����ж�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId, MCU��
				  u8 byMtId, MT��
				  LPCSTR lpszAlias, �ն˼���
				  TMtTopo atMtTopo[], Mt����������
				  u16 wMtTopoNum, ȫ��Mt����Ŀ
    ����ֵ˵��  ���ɹ�����TRUE��ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/05    1.0         LI Yi         ����
====================================================================*/
BOOL topoIsValidMt( u16 wMcuId, u8 byMtId, LPCSTR lpszAlias, TMtTopo atMtTopo[], u16 wMtTopoNum );

/*====================================================================
    ������      ��topoIsValidMcu
    ����        ������MCU�ţ�IP��ַ�ͼ������кϷ����ж�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId, MCU��
				  u32 dwIpAddr, �ն�IP��ַ��������
				  LPCSTR lpszAlias, �ն˼���
				  TMcuTopo atMcuTopo[], Mcu����������
				  u16 wMcuTopoNum, ȫ��Mcu����Ŀ
    ����ֵ˵��  ���ɹ�����TRUE��ʧ�ܷ���FALSE
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    02/12/05    1.0         LI Yi         ����
====================================================================*/
BOOL topoIsValidMcu( u16 wMcuId, u32 dwIpAddr, LPCSTR lpszAlias, 
						   TMcuTopo atMcuTopo[], u16 wMcuTopoNum );

/*====================================================================
    ������      ��topoFindSubMtByIp
    ����        ������MCU�ţ�IP��ַ�õ��¼��ն˺�
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId, MCU��
				  u32 dwIpAddr, �ն�IP��ַ��������
				  TMcuTopo atMtTopo[]��Mt����������
				  u16 wMtTopoNum��ȫ����Mt����Ŀ
    ����ֵ˵��  ���ɹ������ն˺ţ�ʧ�ܷ���0
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/03/12    1.0         LI Yi         ����
====================================================================*/
u8 topoFindSubMtByIp( u16 wMcuId, u32 dwIpAddr, TMtTopo atMtTopo[], u16 wMtTopoNum );

/*====================================================================
    ������      ��topoMtNeedAdapt
    ����        ���ж��ն��Ƿ���Ҫ��������
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����WORD wMcuId, MCU��
				  u32 dwIpAddr, �ն�IP��ַ��������
				  TMcuTopo atMtTopo[]��Mt����������
				  u16 wMtTopoNum��ȫ����Mt����Ŀ
    ����ֵ˵��  ���ɹ������ն˺ţ�ʧ�ܷ���0
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/03/12    1.0         LI Yi         ����
====================================================================*/
BOOL topoMtNeedAdapt( u16 wMcuId, u8 byMtId, TMtTopo atMtTopo[], u16 wMtTopoNum );


/*====================================================================
    ������      ��ReadMcuTopoTable
    ����        ������MCU������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�������
    �������˵����LPCSTR lpszProfileName, �ļ�����������·����
                  TMcuTopo atMcuTopoBuf[], MCU��ϢBUFFER
				  u16 wBufSize, BUFFER��С
    ����ֵ˵��  ��������MCUʵ����Ŀ
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	02/09/18    1.1         LI Yi         ����
====================================================================*/
u8 ReadMcuTopoTable( LPCSTR lpszProfileName, TMcuTopo atMcuTopo[], u16 wBufSize );

/*====================================================================
    ������      ��ReadMtTopoTable
    ����        ������MT������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�������
    �������˵����LPCSTR lpszProfileName, �ļ�����������·����
                  TMtTopo atMtTopoBuf[], Mt��ϢBUFFER
				  u16 wBufSize, BUFFER��С
    ����ֵ˵��  ��������Mtʵ����Ŀ
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	02/10/24    1.0         liaoweijiang  ����
====================================================================*/
u16 ReadMtTopoTable( LPCSTR lpszProfileName, TMtTopo atMtTopo[], u16 wBufSize );

/*====================================================================
    ������      :ReadMcuModemTable
    ����        ������MCU Modem������Ϣ
    �㷨ʵ��    ��
    ����ȫ�ֱ�������
    �������˵����LPCSTR lpszProfileName, �ļ�����������·����
                  TMtTopo atMtTopoBuf[], Mt��ϢBUFFER
				  u16 wBufSize, BUFFER��С
    ����ֵ˵��  ��������Mtʵ����Ŀ
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
	02/10/24    1.0         liaoweijiang  ����
====================================================================*/
u8 ReadMcuModemTable( LPCSTR lpszProfileName, TMcuModemTopo atMcuModemTopo[], u16 wBufSize );

#endif /* _VCTOPO_H_ */

