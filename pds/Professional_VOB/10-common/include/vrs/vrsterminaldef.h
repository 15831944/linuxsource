/*****************************************************************************
ģ����      : vrs
�ļ���      : vrsterminaldef.h
����ļ�    : 
�ļ�ʵ�ֹ���: ¼������������ģ�鹫������
����        : pengjie
�汾        : V2R1  Copyright(C) 2013-2015 KDC, All rights reserved.
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
2013/11/27  V2R1        zhangli       create
******************************************************************************/
#ifndef _VRSTERMINALDEF_H_
#define _VRSTERMINALDEF_H_

//¼����״̬
enum ERPWorkState
{
	RP_STATE_STOP		= 0,	/*δ��ʼ*/
	RP_STATE_WORKING	= 1,	/*����¼/����*/
	RP_STATE_PAUSE		= 2,	/*��ͣ¼/����*/
		
	RP_STATE_START_WAIT,
	RP_STATE_PAUSE_WAIT
};


#endif // _VRSTERMINALDEF_H_

// END OF FILE