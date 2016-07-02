
#include "windataswitch.h"

/*=============================================================================
  �� �� ���� dsCreate
  ��    �ܣ� dataswitch-win32 ��ʼ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u32 dwIfNum				IP����
			 TDSNetAddr *ptDsNetAddr    IP��ַ
  �� �� ֵ�� DSID 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2012/06/11    4.7	        �ܼ���				  ����
=============================================================================*/
DSID dsCreate( u32 dwIfNum,  TDSNetAddr *ptDsNetAddr)
{
	u32 dwRet = DSOK;
	if (0 == dwIfNum)
	{
		printf("[dsCreate] dwIfNum = 0!\n");
		return DSERROR;
	}

	if (NULL == ptDsNetAddr)
	{
		return DSERROR;
	}

	if (DS_TYPE_IPV4 == ptDsNetAddr->GetType())
	{   
		u32 dwV4IP = htonl(ptDsNetAddr->GetV4IPAddress());
		dwRet = dsCreate(1, &dwV4IP);
	}
	else
	{
		printf("[dsCreate] unexpected socktype.%d!\n", ptDsNetAddr->GetType());
		return DSERROR;
	}

	return dwRet;
}

/*=============================================================================
  �� �� ���� dsAdd
  ��    �ܣ� dataswitch-win32 ����һ��һ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� DSID dsId					ds���
		     TDSNetAddr* ptRcvAddr		ת����ַ
		     TDSNetAddr* ptInLocalIP    ���ؽ���
		     TDSNetAddr* ptSendtoAddr   Ŀ�ĵ�ַ
		     TDSNetAddr* ptOutLocalIP   ���ط���
  �� �� ֵ�� u32 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2012/06/11    4.7	        �ܼ���				  ����
=============================================================================*/
u32  dsAdd(DSID dsId,
				TDSNetAddr* ptRcvAddr,
				TDSNetAddr* ptInLocalIP,
				TDSNetAddr* ptSendtoAddr,
				TDSNetAddr* ptOutLocalIP)
{
	if (NULL == ptRcvAddr)
	{
		OspPrintf(1, 0, "[dsAdd] ptRcvAddr is null!\n");
		return DSERROR;
	}

	if (NULL == ptSendtoAddr)
	{
		OspPrintf(1, 0, "[dsAdd] ptSendtoAddr is null!\n");
		return DSERROR;
	}
	
	u32 dwRcvIP  = ptRcvAddr->GetV4IPAddress();
	u16 wRcvPort = ptRcvAddr->GetPort();
	u32 dwDstIP  = ptSendtoAddr->GetV4IPAddress();
	u16 wDstPort = ptSendtoAddr->GetPort();

	u32 dwRet = DSOK;
	dwRet = dsAdd( dsId, htonl(dwRcvIP), wRcvPort, 0, 
						 htonl(dwDstIP), wDstPort, 0);
	return dwRet;
}

/*=============================================================================
  �� �� ���� dsSetSendCallback
  ��    �ܣ� dataswitch-win32 ���ͻص�
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� DSID dsId					ds���
		     TDSNetAddr*  ptRcvAddr		ת����ַ
		     TDSNetAddr*  ptSrcAddr,    Դ��ַ
			 SendCallback pfCallback    �ص�
  �� �� ֵ�� u32 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2012/06/11    4.7	        �ܼ���				  ����
=============================================================================*/
u32 dsSetSendCallback( DSID dsId, 
							TDSNetAddr* ptRcvAddr, 
							TDSNetAddr* ptSrcAddr,
							SendCallback pfCallback)
{
	if (NULL == ptRcvAddr)
	{
		OspPrintf(1, 0, "[dsSetSendCallback] ptRcvAddr is null!\n");
		return DSERROR;
	}

	u32 dwRcvIP  = ptRcvAddr->GetV4IPAddress();
	u16 wRcvPort = ptRcvAddr->GetPort();

	u32 dwRet = dsSetSendCallback( dsId, htonl(dwRcvIP), wRcvPort, 0, 0, pfCallback);
	return DSOK;
}

/*=============================================================================
  �� �� ���� dsSetAppDataForSend
  ��    �ܣ� dataswitch-win32 Ϊ����Ŀ������һ���Զ����ָ�� 
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� DSID dsId					ds���
		     TDSNetAddr*  ptRcvAddr		ת����ַ
		     TDSNetAddr*  ptSrcAddr,	Դ��ַ
			 TDSNetAddr*  ptDstAddr,    Ŀ�ĵ�ַ
			 void *       pAppData      �Զ���ָ��  
  �� �� ֵ�� u32 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2012/06/11    4.7	        �ܼ���				  ����
=============================================================================*/
u32 dsSetAppDataForSend( DSID dsId, 
							TDSNetAddr* ptRcvAddr,
							TDSNetAddr* ptSrcAddr,
							TDSNetAddr* ptDstAddr, 
							void * pAppData)
{
	if (NULL == ptRcvAddr)
	{
		OspPrintf(1, 0, "[dsSetAppDataForSend] ptRcvAddr is null!\n");
		return DSERROR;
	}

	if (NULL == ptDstAddr)
	{
		OspPrintf(1, 0, "[dsSetAppDataForSend] ptDstAddr is null!\n");
		return DSERROR;
	}

	u32 dwRcvIP  = ptRcvAddr->GetV4IPAddress();
	u16 wRcvPort = ptRcvAddr->GetPort();
	u32 dwDstIP  = ptDstAddr->GetV4IPAddress();
	u16 wDstPort = ptDstAddr->GetPort();

	u32 dwRet = DSOK;
	dwRet = ::dsSetAppDataForSend( dsId, htonl(dwRcvIP), wRcvPort, 0, 0,
										 htonl(dwDstIP), wDstPort, pAppData);
	
	return dwRet;
}

/*=============================================================================
  �� �� ���� dsRemove
  ��    �ܣ� dataswitch-win32 ɾ��ת��Ŀ��Ϊָ����ַ��ת������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� DSID       dsId					ds���
			TDSNetAddr* ptSendtoAddr			Ŀ�ĵ�ַ
  �� �� ֵ�� u32 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2012/06/11    4.7	        �ܼ���				  ����
=============================================================================*/
u32 dsRemove(DSID dsId, TDSNetAddr* ptSendtoAddr)
{
	if (NULL == ptSendtoAddr)
	{
		OspPrintf(1, 0, "[dsRemove] ptSendtoAddr is null!\n");
		return DSERROR;
	}
	u32 dwRcvIP  = ptSendtoAddr->GetV4IPAddress();
	u16 wRcvPort = ptSendtoAddr->GetPort();

	u32 dwRet = DSOK;
	dwRet = dsRemove( dsId, htonl(dwRcvIP), wRcvPort );
	return dwRet;
}

/*=============================================================================
  �� �� ���� dsAddManyToOne
  ��    �ܣ� dataswitch-win32 ���Ӷ��һ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� DSID        dsId		    ds���
			 TDSNetAddr* ptRcvAddr		ת����ַ
			 TDSNetAddr* ptInLocalIP    ���ؽ���
			 TDSNetAddr* ptSendtoAddr   Ŀ�ĵ�ַ
		     TDSNetAddr* ptOutLocalIP   ���ط���
  �� �� ֵ�� u32 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2012/06/11    4.7	        �ܼ���				  ����
=============================================================================*/
u32 dsAddManyToOne(DSID dsId ,
						TDSNetAddr* ptRecvAddr,
						TDSNetAddr* ptInLocalIP,
					    TDSNetAddr* ptSendtoAddr,
						TDSNetAddr* ptOutLocalIP)
{
	if (NULL == ptRecvAddr)
	{
		OspPrintf(1, 0, "[dsAddManyToOne] ptRecvAddr is null!\n");
		return DSERROR;
	}

	if (NULL == ptSendtoAddr)
	{
		OspPrintf(1, 0, "[dsAddManyToOne] ptSendtoAddr is null!\n");
		return DSERROR;
	}
	
	u32 dwRcvIP  = ptRecvAddr->GetV4IPAddress();
	u16 wRcvPort = ptRecvAddr->GetPort();
	u32 dwDstIP  = ptSendtoAddr->GetV4IPAddress();
	u16 wDstPort = ptSendtoAddr->GetPort();

	u32 dwRet = DSOK;
	dwRet = ::dsAddManyToOne( dsId, htonl(dwRcvIP), wRcvPort, 0, 
                                    htonl(dwDstIP), wDstPort, 0 );
	return DSOK;
}

/*=============================================================================
  �� �� ���� dsSpecifyFwdSrc
  ��    �ܣ� dataswitch-win32 Ϊָ�����յ�ַ����ת�����ݰ�������Դ��ַ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� DSID        dsId		    ds���
			 TDSNetAddr* ptOrigAddr,	ԭʼ��ַ
			 TDSNetAddr* ptMappedAddr	ӳ���ַ
  �� �� ֵ�� u32 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2012/06/11    4.7	        �ܼ���				  ����
=============================================================================*/
u32 dsSpecifyFwdSrc(DSID dsId, 
					     TDSNetAddr* ptOrigAddr, 
						 TDSNetAddr* ptMappedAddr)
{
	if (NULL == ptOrigAddr)
	{
		OspPrintf(1, 0, "[dsSpecifyFwdSrc] ptOrigAddr is null!\n");
		return DSERROR;
	}

	if (NULL == ptMappedAddr)
	{
		OspPrintf(1, 0, "[dsSpecifyFwdSrc] ptMappedAddr is null!\n");
		return DSERROR;
	}
	
	u32 dwRcvIP  = ptOrigAddr->GetV4IPAddress();
	u16 wRcvPort = ptOrigAddr->GetPort();
	u32 dwMapIP  = ptMappedAddr->GetV4IPAddress();
	u16 wMapPort = ptMappedAddr->GetPort();

	u32 dwRet = DSOK;
	dwRet = dsSpecifyFwdSrc( dsId, htonl(dwRcvIP), wRcvPort, htonl(dwMapIP), wMapPort);
	return dwRet;
}

/*=============================================================================
  �� �� ���� dsRemoveManyToOne
  ��    �ܣ� dataswitch-win32 �Ƴ���㵽һ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� DSID        dsId		    ds���
			 TDSNetAddr* ptRecvAddr 	ת����ַ
			 TDSNetAddr* ptSendtoAddr	Ŀ�ĵ�ַ
  �� �� ֵ�� u32 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2012/06/11    4.7	        �ܼ���				  ����
=============================================================================*/
u32 dsRemoveManyToOne(DSID dsId ,
					       TDSNetAddr* ptRecvAddr,
                           TDSNetAddr* ptSendtoAddr)
{
	if (NULL == ptRecvAddr)
	{
		OspPrintf(1, 0, "[dsRemoveManyToOne] ptRecvAddr is null!\n");
		return DSERROR;
	}

	if (NULL == ptSendtoAddr)
	{
		OspPrintf(1, 0, "[dsRemoveManyToOne] ptSendtoAddr is null!\n");
		return DSERROR;
	}

	u32 dwRcvIP  = ptRecvAddr->GetV4IPAddress();
	u16 wRcvPort = ptRecvAddr->GetPort();
	u32 dwDstIP  = ptSendtoAddr->GetV4IPAddress();
	u16 wDstPort = ptSendtoAddr->GetPort();
	
	u32 dwRet = DSOK;
	dwRet = dsRemoveManyToOne( dsId, htonl(dwRcvIP), wRcvPort, htonl(dwDstIP), wDstPort);
	return dwRet;
}

/*=============================================================================
  �� �� ���� dsRemoveAllManyToOne
  ��    �ܣ� dataswitch-win32 ɾ������ת��Ŀ��Ϊָ����ַ�Ķ��һ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� DSID        dsId		    ds���
			 TDSNetAddr* ptSendtoAddr	Ŀ�ĵ�ַ
  �� �� ֵ�� u32 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2012/06/11    4.7	        �ܼ���				  ����
=============================================================================*/
u32 dsRemoveAllManyToOne(DSID dsId, TDSNetAddr* ptSendtoAddr)
{
	if (NULL == ptSendtoAddr)
	{
		OspPrintf(1, 0, "[dsRemoveAllManyToOne] ptSendtoAddr is null!\n");
		return DSERROR;
	}
	
	u32 dwDstIP  = ptSendtoAddr->GetV4IPAddress();
	u16 wDstPort = ptSendtoAddr->GetPort();
	
	u32 dwRet = DSOK;
	dwRet = dsRemoveAllManyToOne(dsId, htonl(dwDstIP), wDstPort);
	return dwRet;
}

/*=============================================================================
  �� �� ���� dsAddDump
  ��    �ܣ� dataswitch-win32 ����dump����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� DSID        dsId		    ds���
			 TDSNetAddr* ptRecvAddr     ת����ַ
			 TDSNetAddr* ptInLocalIP	���ص�ַ
  �� �� ֵ�� u32 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2012/06/11    4.7	        �ܼ���				  ����
=============================================================================*/
u32 dsAddDump(DSID dsId, TDSNetAddr* ptRecvAddr, TDSNetAddr* ptInLocalIP)
{
	if (NULL == ptRecvAddr)
	{
		OspPrintf(1, 0, "[dsAddDump] ptRecvAddr is null!\n");
		return DSERROR;
	}

	u32 dwRcvIP  = ptRecvAddr->GetV4IPAddress();
	u16 wRcvPort = ptRecvAddr->GetPort();

	u32 dwRet = DSOK;
	dwRet = dsAddDump(dsId, htonl(dwRcvIP), wRcvPort, 0);
	return dwRet;
}

/*=============================================================================
  �� �� ���� dsRemoveDump
  ��    �ܣ� dataswitch-win32 �Ƴ�dump����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� DSID        dsId		    ds���
			 TDSNetAddr* ptRecvAddr     ת����ַ
  �� �� ֵ�� u32 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2012/06/11    4.7	        �ܼ���				  ����
=============================================================================*/
u32 dsRemoveDump(DSID dsId, TDSNetAddr* ptRecvAddr)
{
	if (NULL == ptRecvAddr)
	{
		OspPrintf(1, 0, "[dsRemoveDump] ptRecvAddr is null!\n");
		return DSERROR;
	}
	u32 dwRcvIP  = ptRecvAddr->GetV4IPAddress();
	u16 wRcvPort = ptRecvAddr->GetPort();
	u32 dwRet    = DSOK;
	dwRet = dsRemoveDump(dsId, htonl(dwRcvIP), wRcvPort);
	return dwRet;
}






