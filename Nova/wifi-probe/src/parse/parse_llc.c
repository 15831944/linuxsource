#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse_llc.h"
#include "parse_public.h"
#include "parse_api_private.h"
#include "ap_log.h"
#include "wifiprobe.h"
/*****************************************************************************
 �� �� ��  : snap_parse
 ��������  : ����snap
 �������  : const u_char *p                       
             u_int length                          
             u_int caplen                          
             u_int bridge_pad                      
             PACKET_STATISTICS_T *pstStaticOutPut  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��4��21��
    ��    ��   : kevin
    �޸�����   : �����ɺ���

*****************************************************************************/
int snap_parse(const u_char *p, 
				u_int length, 
				u_int caplen, 
				u_int bridge_pad, 
				PACKET_STATISTICS_T *pstStaticOutPut)
{
	u_int32_t  uiOrgCode;
	u_int16_t  uiEtherType;
	int iRet;
	
	if(NULL == pstStaticOutPut)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "input null pointer!\n");
		return -1;
	}

	uiOrgCode = EXTRACT_24BITS(p);
	uiEtherType = EXTRACT_16BITS(p + 3);

	
	switch(uiOrgCode)
	{
		case OUI_ENCAP_ETHER:
		case OUI_CISCO_90:
			/*
			 * This is an encapsulated Ethernet packet,
			 * or a packet bridged by some piece of
			 * Cisco hardware; the protocol ID is
			 * an Ethernet protocol type.
			 */
			switch(uiEtherType)
			{
				case ETHERTYPE_IP:
					STATISTIC_PROBE_INC(ip4);
					ip_parse(p + 5, length -5, caplen - 5, pstStaticOutPut);
					return 1;
				case ETHERTYPE_IPV6:
					#ifdef INET6
					STATISTIC_PROBE_INC(ip6);
					ip6_parse(p + 5, length -5, caplen - 5, pstStaticOutPut);
					#endif
					return 1;
				default :
					STATISTIC_PROBE_INC(proto_others);
					break;
			}
			break;
		default :
			//printf("default org:%x\n", orgcode);
			break;
	}
	
	return 0;
}

/*****************************************************************************
 �� �� ��  : llc_parse
 ��������  : logical link control parse
 �������  : const u_char *p   //packet payload                    
             u_int length      //all length (default contain FCS len)    
             u_int caplen      //caplen(not contain FCS len)                  
             PACKET_STATISTICS_T *pstStaticOutPut  //output param
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��4��21��
    ��    ��   : kevin
    �޸�����   : �����ɺ���

*****************************************************************************/
int llc_parse(const u_char *p, 
				u_int length, 
				u_int caplen, 
				PACKET_STATISTICS_T *pstStaticOutPut)
{
	u_int8_t ucDsapField;
	u_int8_t ucSsapField;
	u_int8_t ucDsap;
	u_int8_t ucSsap;

	u_int16_t usControl;
	int	iIsU = 0;
	int iRet = 0;
	
	if(NULL == pstStaticOutPut)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "input null pointer!\n");
		return -1;
	}

	if(caplen < 3)
	{
		return -2;
	}

	ucDsapField = *p;
	ucSsapField = *(p + 1);

	/* �ж�llc����һ��֡��u��i��s */
	usControl = *(p + 2);
	if((usControl & LLC_U_FMT) == LLC_U_FMT)
	{
		iIsU = 1; /* U֡ */
	}
	else
	{
		/* i or s ֡ */
		if(caplen < 4)
		{
			return -2;
		}
		
		usControl = EXTRACT_LE_16BITS(p + 2);
		iIsU = 0;
	}

	ucDsap = ucDsapField & ~LLC_IG;
	ucSsap = ucSsapField & ~LLC_GSAP;
 	if(ucSsap == LLCSAP_SNAP && ucDsap == LLCSAP_SNAP && usControl == LLC_UI)
	{
		/*
		 * XXX - what *is* the right bridge pad value here?
		 * Does anybody ever bridge one form of LAN traffic
		 * over a networking type that uses 802.2 LLC?
		 */
		iRet = snap_parse(p + 3, length - 3, caplen - 3, 2, pstStaticOutPut);
		return iRet;
	}
	return 0;
}

