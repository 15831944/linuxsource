/*****************************************************************************
   ģ����      : MP
   �ļ���      : mputility.cpp
   ����ļ�    : mp.h
   �ļ�ʵ�ֹ���: MP��ҵ���߼���������ʵ��
   ����        : ������
   �汾        : V4.0  Copyright(C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��       �汾        �޸���          �޸�����
   2007/09/24   4.0         �ű���          ����
******************************************************************************/
#include "mp.h"

CNetTrfcStat g_cNetTrfcStat;
BOOL32       g_bPrintMpNetStat = FALSE;


//=======================================================//
//                                                       //
//               ��������ͳ�ƶ���                        //
//                                                       //
//=======================================================//
CNetTrfcStat::CNetTrfcStat()
{
    Reset();
}

CNetTrfcStat::~CNetTrfcStat()
{
}


/*=============================================================================
  �� �� ���� Reset
  ��    �ܣ� ��ʼ��ͳ��ֵ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/12/30    4.0			����                  ����
=============================================================================*/
void CNetTrfcStat::Reset(BOOL32 bRecv, BOOL32 bSend)
{
	m_nMaxCap = 0;
    if ( bRecv )
        memset( m_alnNetRecvBytes, 0, sizeof(m_alnNetRecvBytes) );
    if ( bSend )
        memset( m_alnNetSendBytes, 0, sizeof(m_alnNetSendBytes) );
}

/*=============================================================================
  �� �� ���� AppendLatestStat
  ��    �ܣ� ׷�����µ�ͳ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/12/30    4.0			����                  ����
=============================================================================*/
void CNetTrfcStat::AppendLatestStat(s64 lnRecv, s64 lnSend)
{
    // ����ƶ�һ��
    memmove( (void*)&(m_alnNetRecvBytes[1]), (void*)m_alnNetRecvBytes, sizeof(m_alnNetRecvBytes)-sizeof(s64) );
    memmove( (void*)&(m_alnNetSendBytes[1]), (void*)m_alnNetSendBytes, sizeof(m_alnNetSendBytes)-sizeof(s64) );

    m_alnNetRecvBytes[0] = lnRecv;
    m_alnNetSendBytes[0] = lnSend;

    if ( lnRecv < m_alnNetRecvBytes[1] )
        Reset( TRUE, FALSE );
    if ( lnSend < m_alnNetSendBytes[1] )
        Reset( FALSE, TRUE );

}

/*=============================================================================
  �� �� ���� DumpLatestStat
  ��    �ܣ� ��ӡ���µ�ͳ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/12/30    4.0			����                  ����
=============================================================================*/
void CNetTrfcStat::DumpLatestStat()
{
	/*lint -save -e704*/
    s32 lnRecv = (s32)(((m_alnNetRecvBytes[0] - m_alnNetRecvBytes[1]) / NETFRFCSTAT_TIMESPAN) >> 7);    // Byte -->��ԼKbps
    s32 lnSend = (s32)(((m_alnNetSendBytes[0] - m_alnNetSendBytes[1]) / NETFRFCSTAT_TIMESPAN) >> 7);
	/*lint -restore*/
    OspPrintf( TRUE, FALSE, "[MPDSStat] Data Switch Traffic: Recv: %d Kbps, Send: %d Kbps.\n" ,
               lnRecv, lnSend );
}

/*=============================================================================
  �� �� ���� DumpAllStat
  ��    �ܣ� ��ӡ���е�ͳ��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/12/30    4.0			����                  ����
=============================================================================*/
void CNetTrfcStat::DumpAllStat()
{
    OspPrintf( TRUE, FALSE, "[MPDSStat] Data Switch Traffic Statics\n");
    s32 lnRecv, lnSend;
    for ( u32 i = 0; i < NETFRFCSTAT_BUFFER/NETFRFCSTAT_TIMESPAN; i++ )
    {
		/*lint -save -e704*/
        lnRecv = (s32)(((m_alnNetRecvBytes[i] - m_alnNetRecvBytes[i+1]) / (s64)(NETFRFCSTAT_TIMESPAN)) >> 7);  // Byte -->��ԼKbps
        lnSend = (s32)(((m_alnNetSendBytes[i] - m_alnNetSendBytes[i+1]) / (s64)(NETFRFCSTAT_TIMESPAN)) >> 7); 
		/*lint -restore*/
		OspPrintf( TRUE, FALSE, "[MPDSStat] Time: %.4d, Recv: %d Kbps, Send: %d Kbps.\n" ,
                    i*NETFRFCSTAT_TIMESPAN,
                   lnRecv,
                   lnSend );
    }
    OspPrintf( TRUE, FALSE, "[MPDSStat] Current Max Cap: %d\n", m_nMaxCap);

}

/*=============================================================================
  �� �� ���� GetAvgNetBurden
  ��    �ܣ� ȡָ��ʱ����ڵ����縺��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/12/30    4.0			����                  ����
=============================================================================*/
void  CNetTrfcStat::GetAvgNetBurden(u32 nTimeSpan, s32 &nSend, s32 &nRecv)
{
    s32 lnRecv = 0, lnSend = 0;
    for ( u32 i = 0; i < nTimeSpan/NETFRFCSTAT_TIMESPAN; i++ )
    {
		/*lint -save -e704*/
        lnRecv += (s32)(((m_alnNetRecvBytes[i] - m_alnNetRecvBytes[i+1]) / NETFRFCSTAT_TIMESPAN) >> 7); 
        lnSend += (s32)(((m_alnNetSendBytes[i] - m_alnNetSendBytes[i+1]) / NETFRFCSTAT_TIMESPAN) >> 7); 
		/*lint -restore*/
    }
    nRecv = (s32)(lnRecv / (s32)(nTimeSpan/NETFRFCSTAT_TIMESPAN));
    nSend = (s32)(lnSend / (s32)(nTimeSpan/NETFRFCSTAT_TIMESPAN));
    return ;
}


/*=============================================================================
  �� �� ���� IsNetTrfcOverrun
  ��    �ܣ� ���ݲ����ж��Ƿ��ع���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/12/30    4.0			����                  ����
=============================================================================*/
BOOL32 CNetTrfcStat::IsNetTrfcOverrun()
{
    // ȡһ�����ڵ�ͳ��ֵ
    s32 nSend = 0;
    s32 nRecv = 0;
    GetAvgNetBurden( NETFRFCSTAT_TIMESPAN, nSend, nRecv);
    if ( nSend + nRecv > m_nMaxCap )
        return TRUE;

    return FALSE;
}

/*=============================================================================
  �� �� ���� IsFluxOverrun
  ��    �ܣ� ���ݲ����ж��Ƿ��ع���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/12/30    4.0			�ܹ��                  ����
=============================================================================*/
BOOL32 CNetTrfcStat::IsFluxOverrun(const u32 nTimeSpan)
{
	u32 nTSpan = nTimeSpan;
	if( ( NETFRFCSTAT_BUFFER / NETFRFCSTAT_TIMESPAN ) < ( nTSpan / NETFRFCSTAT_TIMESPAN ) )
	{
		nTSpan = NETFRFCSTAT_BUFFER/NETFRFCSTAT_TIMESPAN;
	}
	if( nTSpan < NETFRFCSTAT_TIMESPAN )
	{
		nTSpan = NETFRFCSTAT_TIMESPAN;
	}

    s32 nSend = 0;
    s32 nRecv = 0;
    GetAvgNetBurden( nTSpan, nSend, nRecv);

	if( nSend + nRecv > m_nMaxCap )
	{
		return TRUE;
	}
    return FALSE;
}



//=======================================================//
//                                                       //
//               �������˼�����                        //
//                                                       //
//=======================================================//


/*=============================================================================
������		:  ConvertN2H
����		�� net order to machine order, batch convert;
�㷨ʵ��	������ѡ�
����ȫ�ֱ����� ��
�������˵��: 
                pBuf    buffer to convert
                nStartIndex  start position
                nSize        size to covert
����ֵ˵���� Valid TRUE,Invalid FALSE;
=============================================================================*/
void CMpBitStreamFilter::ConvertN2H(u8 *pBuf, s32 nStartIndex, s32 nSize)
{
    for(s32 i=nStartIndex; i<(nStartIndex+nSize); i++) 
    {
        ((u32 *)pBuf)[i] = ntohl(((u32 *)pBuf)[i]);
    }
    return;
}


/*=============================================================================
������		��GetBitField
����		����һ��u32 ֵһϵ��λ��ֵ��
�㷨ʵ��	������ѡ�
����ȫ�ֱ�������
�������˵����dwValue      Ҫ�����u32ֵ.                     
  			  nStartBit    ��ʼλ. if nStartBit is set to 1,
                                    and nBits is set to 3,
                                    the bit sequence will occupy bits 1 through 4.                                                    
              nBits        λ����                               
����ֵ˵��  ��������u32 ֵ.
=============================================================================*/
u32 CMpBitStreamFilter::GetBitField(u32 dwValue, s32 nStartBit, s32 nBits)
{
	/*lint -save -e701*/
    s32  nMask = (1 << nBits) - 1;
    /*lint -restore*/
	/*lint -save -e737*/
    return (dwValue >> nStartBit) & nMask; 
	/*lint -restore*/
}	

/*=============================================================================
  �� �� ���� IsKeyFrame
  ��    �ܣ� �жϱ�֡�Ƿ�Ϊ�ؼ�֡
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� u8 byPayLoad, u8 *pBuf, s32 nSize
  �� �� ֵ�� BOOL32
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/11    4.0			�ű���                ����
=============================================================================*/
BOOL32 CMpBitStreamFilter::IsKeyFrame( u8 byPayLoad, u8 *pBuf, s32 nSize  )
{
    if (NULL == pBuf || nSize < RTP_FIXEDHEADER_SIZE )
    {
        return FALSE;
    }        
    
    u8 abyTmpBuf[RTP_FIXEDHEADER_SIZE*3] = {0};
    memcpy(abyTmpBuf, pBuf, min(nSize, RTP_FIXEDHEADER_SIZE*3) );
    
    u32* header =(u32*)abyTmpBuf;
    
    TRtpPack tRtpPack;
    memset(&tRtpPack, 0, sizeof(tRtpPack));
    
    //RTP fixed Header Convert
    ConvertN2H(abyTmpBuf, 0, 3);

    //tRtpPack.m_dwTimeStamp = header[1];
    //tRtpPack.m_wSequence   = (u16)GetBitField(header[0], 0, 16);
    //tRtpPack.m_dwSSRC      = header[2];
    //tRtpPack.m_byMark      = (u8)GetBitField(header[0], 23, 1);
    //tRtpPack.m_byPayload   = (u8)GetBitField(header[0], 16, 7);
    tRtpPack.m_byExtence     = (u8)GetBitField(header[0], 28, 1);
    
    //CSRC
    ConvertN2H(abyTmpBuf, 3, GetBitField(header[0], 24, 4));
    s32 nOffSet          = RTP_FIXEDHEADER_SIZE + GetBitField(header[0], 24, 4)*sizeof(u32);    
    tRtpPack.m_nRealSize = nSize - nOffSet;
    tRtpPack.m_pRealData = abyTmpBuf + nOffSet; 

    //��չͷ �� ������ Fixed Header����, Ŀǰ���ڴ����Զ����mp4��mp3��
    if (tRtpPack.m_byExtence)/*Extension Bit Set*/
    {
        if(tRtpPack.m_nRealSize < EX_HEADER_SIZE+MIN_PACK_EX_LEN) 
        {
            return -1;
        }
        s32 xStart = nOffSet/s32(sizeof(u32));
        ConvertN2H(abyTmpBuf, xStart, 1);
        tRtpPack.m_nExSize    = (u16)GetBitField(header[xStart], 0, 16);
        tRtpPack.m_pExData    = abyTmpBuf + u32((xStart+1)) * sizeof(u32);
        tRtpPack.m_nRealSize -= s32(u32(tRtpPack.m_nExSize + 1)*sizeof(u32));
        tRtpPack.m_pRealData += (u32(tRtpPack.m_nExSize + 1)*sizeof(u32));
    }

    //Padding Bit Set
    if (GetBitField(header[0], 29, 1))
    {
        // ��㴦��
        // ע����ڼ���������padding���ȵ��ռ� - ���һ���ֽ�
        tRtpPack.m_byPadNum  = 1;
    }

    BOOL32 bIFrame = FALSE;

    switch( byPayLoad ) 
    {            
    case MEDIA_TYPE_H263PLUS:
        // 263+��Ƶ��֧�ּ�⣬ֱ��pass
        bIFrame = TRUE;
        break;
    case MEDIA_TYPE_H261:
        bIFrame = IsH261KeyFrame( tRtpPack.m_pRealData);
        break;
    case MEDIA_TYPE_H263:
        bIFrame = IsH263KeyFrame( pBuf, nSize );
        break;
    case MEDIA_TYPE_H264:
        bIFrame = IsH264KeyFrame( pBuf, nSize );
        break;
    case MEDIA_TYPE_H262:
    case MEDIA_TYPE_MP4:
        bIFrame = IsMpeg4KeyFrame( &tRtpPack );
        break;    
        // ��Ƶ�����
    case MEDIA_TYPE_MP3:
    case MEDIA_TYPE_PCMA:
    case MEDIA_TYPE_PCMU:
    case MEDIA_TYPE_G721:
    case MEDIA_TYPE_G722:
    case MEDIA_TYPE_G7231:
    case MEDIA_TYPE_G728:
    case MEDIA_TYPE_G729:
    case MEDIA_TYPE_G7221:
	case MEDIA_TYPE_G719:
        bIFrame = FALSE;
        break;
    default:
        bIFrame = TRUE;
        break;
    }
    
    return bIFrame;
}

/*=============================================================================
  �� �� ���� IsH261KeyFrame
  ��    �ܣ� �ж��Ƿ���H261�Ĺؼ�֡
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pBuf rtp: ͷ��ʼλ�ã�������rtpͷ��
             nSize   : ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/20    4.0			����                  ����
=============================================================================*/
BOOL32 CMpBitStreamFilter::IsH261KeyFrame(u8 *pBuf)
{
    TH261Header tH261Header;
    memset(&tH261Header, 0, sizeof(tH261Header));
    
    u32 dwH261Header = *((u32 *)(pBuf));
    
    //get h261 header
    dwH261Header  = ntohl(dwH261Header);
    //tH261Header.vMvd  = GetBitField(dwH261Header, 0, 5);	
    //tH261Header.hMvd  = GetBitField(dwH261Header, 5, 5);	
    //tH261Header.quant = GetBitField(dwH261Header, 10, 5);	
    //tH261Header.mbaP  = GetBitField(dwH261Header, 15, 5);	
    //tH261Header.gobN  = GetBitField(dwH261Header, 20, 4);	
    //tH261Header.v     = GetBitField(dwH261Header, 24, 1);	
    tH261Header.i	    = GetBitField(dwH261Header, 25, 1);	
    //tH261Header.eBit  = GetBitField(dwH261Header, 26, 3);	
    //tH261Header.sBit  = GetBitField(dwH261Header, 29, 3);
    
    return tH261Header.i;
}

/*=============================================================================
  �� �� ���� IsH263KeyFrame
  ��    �ܣ� �ж��Ƿ���h263�Ĺؼ�֡
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pBuf rtp: ͷ��ʼλ�ã�����rtpͷ��h263ͷ��
             nSize   : ����
  �� �� ֵ�� 0���ǹؼ�֡��1 �ǹؼ�֡��-1 ����(��������h263�����ȴ������ʹ����)
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2006/05       4.0			������                  ����
=============================================================================*/
BOOL32 CMpBitStreamFilter::IsH263KeyFrame(u8 *pBuf, s32 nSize)
{
    if (NULL == pBuf || nSize < RTP_FIXEDHEADER_SIZE + 8)
    {
        return -1;
    }        
    
    //����H263ͷƫ��
    s32 nOffSet = RTP_FIXEDHEADER_SIZE;
    
    //����׼������RTPͷ����12�ֽڵ����
    u8 byCC;
    //CSRC��Ŀ
    byCC = *(pBuf);
    byCC &= 0x0f;
    nOffSet += s32(byCC * sizeof(u32));
    
    if (nSize < nOffSet + 4)
    {
        return -1;
    }
    
    //�Ƿ�����չ
    u8 byExtend;
    byExtend = *(pBuf);
    byExtend &= 0x10;
    if (byExtend)
    {
        u16 wExtLen;
        wExtLen = *(pBuf+nOffSet + 2);
        wExtLen += (wExtLen << 8) + *(pBuf+ nOffSet + 3);
        nOffSet += s32(wExtLen * sizeof(u32));
    }
    
    if (nSize < nOffSet + 4)
    {
        return -1;
    }
    
    BOOL32 bKeyFrame = FALSE;
    //��ģʽ
    u8 byMode;
    byMode = *(pBuf+nOffSet);
    byMode &= 0xc0;
    if ((byMode & 0x80) == 0) // A mode
    {
        u8 byInterFrame;
        byInterFrame = *(pBuf+nOffSet + 1);
        byInterFrame &= 0x10;
        if (!byInterFrame)
        {
            bKeyFrame = TRUE;
        }
    }
    else if ((byMode & 0x80) && ((byMode & 0x40) == 0))
    {
        if (nSize < nOffSet + 8)
        {
            return -1;
        }
        
        u8 byInterFrame;
        byInterFrame = *(pBuf+nOffSet + 4);
        byInterFrame &= 0x80;
        if (!byInterFrame)
        {
            bKeyFrame = TRUE;
        }
    }
    
    if (bKeyFrame)
    {
        return 1;
    }
    
    return 0;
}

/*=============================================================================
  �� �� ���� IsH264KeyFrame
  ��    �ܣ� �ж��Ƿ���H.264�Ĺؼ�֡
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pBuf rtp: ͷ��ʼλ�ã�����rtpͷ��
             nSize   : ����
  �� �� ֵ�� 
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/20    4.0			����                  ����
=============================================================================*/
BOOL32 CMpBitStreamFilter::IsH264KeyFrame(u8 *pBuf, s32 nSize)
{
	u8 byRtpHdr = *pBuf;
	BOOL32 bExt = FALSE; //������չ��RTPͷ��ֻռ12�ֽڣ�����ȡ��չ����

	if ( byRtpHdr & 0x10)
	{
		bExt = TRUE;
	}
	MpLog(LOG_PRTP, "[IsH264KeyFrame]pack[%d] has extension:%d\n", nSize, bExt);

	u16 wRtpHdrLen = RTP_FIXEDHEADER_SIZE;//Ĭ��RTPͷ������Ϊ12B
	u16 extHdrLen = 0; //��չ����
	if ( bExt )
	{
		extHdrLen = *(u16*)(pBuf + 14);
		extHdrLen = ntohs(extHdrLen);
		MpLog(LOG_PRTP, "[IsH264KeyFrame]extHdrLen:%d\n", extHdrLen);
		wRtpHdrLen += 2; //define of profile
		wRtpHdrLen += 2; //extension length
		wRtpHdrLen += 4*(extHdrLen);
	}

	u8 byNalType = *(pBuf + wRtpHdrLen);
	MpLog(LOG_PRTP, "[IsH264KeyFrame]wRtpHdrLen:%d byNalType:0x%x\n", wRtpHdrLen, byNalType);
	
	//�Ƿ��а�
	if ( ( byNalType & 0x1F) == 28)
	{
		//�ְ�����һ�ֽ���NAL  Type;
		byNalType = *(pBuf + wRtpHdrLen + 1);
	}
	
	MpLog(LOG_PRTP, "[IsH264KeyFrame] final byNalType[2]:0x%x\n", byNalType);
	
	u8 byRealNalType = byNalType & 0x1F;
	
	MpLog(LOG_PRTP, "[IsH264KeyFrame]byRealNalType:%d\n", byRealNalType);

	switch ( byRealNalType )
	{
	case 5: //IDR
	case 7: //SPS
	case 8: //PPS
		return TRUE;
	default:
		return FALSE;
	}

/*
    u8* pRtpPackBuf = pBuf + RTP_FIXEDHEADER_SIZE;
    s32 nRtpPackBufSize = nSize - RTP_FIXEDHEADER_SIZE;

    TKdvH264Header tH264Header;
    memset(&tH264Header, 0, sizeof(TKdvH264Header));
    
    if (nRtpPackBufSize <= 1 )
    {
        return FALSE;
    }
    
    BOOL32 bRet = TRUE;
    // the format of the NAL unit type octet is reprinted below:
    //  +---------------+
    //	|0|1|2|3|4|5|6|7|
    //	+-+-+-+-+-+-+-+-+
    //	|F|NRI|  Type   |
    //	+---------------+
    u8  byNaluTypeOctet = (u8) (*pRtpPackBuf);
    
    u32 dwNaluType = byNaluTypeOctet & 0x1F;  // ȡ�õ�5λ
    TBitStream tBitStream;
    memset(&tBitStream, 0, sizeof(tBitStream));

    // TBitStream�ṹ��RTP���ĵ�14�ֽڿ�ʼ��ǰ12�ֽ�ΪRTPͷ����13�ֽ�ΪNalu���ͣ�
    stdh264_bs_init(&tBitStream, (pRtpPackBuf+1), (nRtpPackBufSize-1));
    TSeqParameterSetRBSP tSPS;
    TPicParameterSetRBSP tPPS;
    Tstdh264Dec_SliceHeaderData tSlice_header;
    memset(&tSPS, 0, sizeof(tSPS));
    memset(&tPPS, 0, sizeof(tPPS));
    memset(&tSlice_header, 0, sizeof(tSlice_header));
    
    switch(dwNaluType) 
    {
    case 1:
    case 5:  // NALU_TYPE_IDR
    case 28: // FU_A �а����Լ��֧��
        stdh264_FirstPartOfSliceHeader(&tBitStream, &tSlice_header);
        if(u16(I_SLICE) == tSlice_header.slice_type)
        {
            MpLog(LOG_DETAIL, "[IsH264KeyFrame]Keyframe:I_SLICE\n");
            tH264Header.m_bIsKeyFrame = TRUE;
        }
        break;
    case 7:  // NALU_TYPE_SPS
        bRet = DecodeH264SPS(&tBitStream, &tSPS, &tH264Header);
        if ( bRet )
        {
            MpLog(LOG_DETAIL, "[IsH264KeyFrame]NALU_TYPE_SPS\n");
            //guzh 2008/08/06 ?
            tH264Header.m_bIsKeyFrame = TRUE;
        }
        break;
    case 8:  // NALU_TYPE_PPS
        bRet = DecodeH264PPS(&tBitStream, &tPPS, &tH264Header);
        if ( bRet )
        {
            MpLog(LOG_DETAIL, "[IsH264KeyFrame]NALU_TYPE_PPS\n");
            //guzh 2008/08/06 ?
            tH264Header.m_bIsKeyFrame = TRUE;
        }
        break;
    default:
        break;
    }
    
    return tH264Header.m_bIsKeyFrame;
*/
}

/*=============================================================================
  �� �� ���� IsMpeg4KeyFrame
  ��    �ܣ� �ж��Ƿ���MP4�Ĺؼ�֡
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� pBuf rtp: ͷ��ʼλ�ã�����rtpͷ��
             nSize   : ����
  �� �� ֵ�� 0���ǹؼ�֡��1 �ǹؼ�֡��-1 ���󣨰�������Mpeg4�����ȴ������ʹ���ȣ�
  ----------------------------------------------------------------------
  �޸ļ�¼    ��
  ��  ��		�汾		�޸���		�߶���    �޸�����
  2007/04/20    4.0			����                  ����
=============================================================================*/
BOOL32 CMpBitStreamFilter::IsMpeg4KeyFrame(const TRtpPack *ptRtpPack )
{        
    if ( ptRtpPack->m_nExSize != 0)
    {
        u8* pbyHeader = ptRtpPack->m_pExData;
        return ( 1 == pbyHeader[MPEG4_FRAMEMODE_POS] );
    }

    return FALSE;
}


/*=============================================================================
������		��stdh264_bs_init ....
����		��h264 ������Ϣ����
�㷨ʵ��	������ѡ�
����ȫ�ֱ�������
�������˵����
��           

  ����ֵ˵���� ��
=============================================================================*/
void CMpBitStreamFilter::stdh264_bs_init( TBitStream *s, void *p_data, s32 i_data )
{
    s->pu8Start = (u8*) p_data;
    s->pu8P     = (u8*) p_data;
    s->pu8End   = (u8*) s->pu8P + i_data;
    s->s32Left  = 8;
}

s32 CMpBitStreamFilter::stdh264_bs_pos( TBitStream *s )
{
    return( 8 * ( s->pu8P - s->pu8Start ) + 8 - s->s32Left );
}

s32 CMpBitStreamFilter::stdh264_bs_eof( TBitStream *s )
{
    return( s->pu8P >= s->pu8End ? 1: 0 );
}

u32 CMpBitStreamFilter::stdh264_bs_read( TBitStream *s, s32 i_count )
{
    static const u32 dwstdh264MaskArr[33] = 
	{ 
		0x00,
        0x01,      0x03,      0x07,      0x0f,
        0x1f,      0x3f,      0x7f,      0xff,
        0x1ff,     0x3ff,     0x7ff,     0xfff,
        0x1fff,    0x3fff,    0x7fff,    0xffff,
        0x1ffff,   0x3ffff,   0x7ffff,   0xfffff,
        0x1fffff,  0x3fffff,  0x7fffff,  0xffffff,
        0x1ffffff, 0x3ffffff, 0x7ffffff, 0xfffffff,
        0x1fffffff,0x3fffffff,0x7fffffff,0xffffffff };
		
    s32 i_shr;
    u32 i_result = 0;

    while( i_count > 0 )
    {
        if( s->pu8P >= s->pu8End )
        {
            break;
        }

        if( ( i_shr = s->s32Left - i_count ) >= 0 )
        {
            /* more in the buffer than requested */
            i_result |= ( *s->pu8P >> i_shr )&dwstdh264MaskArr[i_count];
            s->s32Left -= i_count;
            if( s->s32Left == 0 )
            {
                s->pu8P++;
                s->s32Left = 8;
            }
            return( i_result );
        }
        else
        {
            /* less in the buffer than requested */
			//i_result |= ((*s->pu8P & dwstdh264MaskArr[s->s32Left]) << -i_shr);
           i_result |= ((*s->pu8P & dwstdh264MaskArr[s->s32Left]) << abs(i_shr));
           i_count  -= s->s32Left;
           s->pu8P++;
           s->s32Left = 8;
        }
    }

    return( i_result );
}

u32 CMpBitStreamFilter::stdh264_bs_read1( TBitStream *s )
{

    if( s->pu8P < s->pu8End )
    {
        u32 i_result;

        s->s32Left--;
        i_result = ( *s->pu8P >> s->s32Left )&0x01;
        if( s->s32Left == 0 )
        {
            s->pu8P++;
            s->s32Left = 8;
        }
        return i_result;
    }

    return 0;
}

u32 CMpBitStreamFilter::stdh264_bs_show( TBitStream *s, s32 i_count )
{
    if( s->pu8P < s->pu8End && i_count > 0 )
    {
		/*lint -save -e701*/
        u32 i_cache = ((s->pu8P[0] << 24)+(s->pu8P[1] << 16)+(s->pu8P[2] << 8)+s->pu8P[3]) << (8-s->s32Left);
		/*lint -restore*/
        return( i_cache >> ( 32 - i_count) );
    }
    return 0;
}


void CMpBitStreamFilter::stdh264_bs_skip( TBitStream *s, s32 i_count )
{
    s->s32Left -= i_count;

    while( s->s32Left <= 0 )
    {
        s->pu8P++;
        s->s32Left += 8;
    }
}

s32 CMpBitStreamFilter::stdh264_bs_read_ue( TBitStream *s )
{
    s32 i = 0;
    
    while( stdh264_bs_read1( s ) == 0 && s->pu8P < s->pu8End && i < 32 )
    {
        i++;
    }
	/*lint -save -e701*/
    return( ( 1 << i) - 1 + stdh264_bs_read( s, i ) );
	/*lint -restore*/
}

s32 CMpBitStreamFilter::stdh264_bs_read_se( TBitStream *s )
{
    s32 val = stdh264_bs_read_ue( s );
    
    return val&0x01 ? (val+1)/2 : -(val/2);
}

s32 CMpBitStreamFilter::stdh264_bs_read_te( TBitStream *s, s32 x )
{
    if( x == 1 )
    {
        return 1 - stdh264_bs_read1( s );
    }
    else if( x > 1 )
    {
        return stdh264_bs_read_ue( s );
    }
    return 0;
}

s32 CMpBitStreamFilter::stdh264_FirstPartOfSliceHeader(TBitStream *s, Tstdh264Dec_SliceHeaderData *dec_slice_header)
{
    s32 tmp;
    dec_slice_header->first_mb_in_slice = stdh264_bs_read_ue( s );
    tmp = stdh264_bs_read_ue( s );
    if (tmp>4) 
    {
        tmp -=5;
    }
    dec_slice_header->slice_type = (u16) tmp;
    
    return 0;
}

/*=============================================================================
������		��DecodeH264SPS
����		������ h.264 �����е� sps ��Ϣ
�㷨ʵ��	������ѡ�
����ȫ�ֱ�������
�������˵����

  ����ֵ˵���� TRUE - �ɹ�
=============================================================================*/
BOOL32 CMpBitStreamFilter::DecodeH264SPS( TBitStream *s, TSeqParameterSetRBSP *sps, 
                                          TKdvH264Header *pStdH264Header )
{
    u32 i;
    
    sps->profile_idc               = stdh264_bs_read( s, 8 );
    
    sps->constrained_set0_flag     = stdh264_bs_read( s, 1 );
    sps->constrained_set1_flag     = stdh264_bs_read( s, 1 );
    sps->constrained_set2_flag     = stdh264_bs_read( s, 1 );
    stdh264_bs_skip( s, 5 );        //reserved_zero
    
    sps->level_idc                 = stdh264_bs_read( s, 8 );
    
    sps->seq_parameter_set_id      = stdh264_bs_read_ue( s );
    sps->log2_max_frame_num_minus4 = stdh264_bs_read_ue( s );
    sps->pic_order_cnt_type        = stdh264_bs_read_ue( s );
    
    if (sps->pic_order_cnt_type == 0)
    {
        sps->log2_max_pic_order_cnt_lsb_minus4 = stdh264_bs_read_ue( s );
    }
    else if (sps->pic_order_cnt_type == 1)
    {
        sps->delta_pic_order_always_zero_flag      = stdh264_bs_read( s, 1 );
        sps->offset_for_non_ref_pic                = stdh264_bs_read_se( s );
        sps->offset_for_top_to_bottom_field        = stdh264_bs_read_se( s );
        sps->num_ref_frames_in_pic_order_cnt_cycle = stdh264_bs_read_ue( s );
        if (sps->num_ref_frames_in_pic_order_cnt_cycle > MAXnum_ref_frames_in_pic_order_cnt_cycle)
        {
            return FALSE;
        }
        for(i=0; i<sps->num_ref_frames_in_pic_order_cnt_cycle; i++)
        {
            sps->offset_for_ref_frame[i]           = stdh264_bs_read_se( s );
        }
    }
    
    sps->num_ref_frames                        = stdh264_bs_read_ue( s );
    sps->gaps_in_frame_num_value_allowed_flag  = stdh264_bs_read( s, 1 );
    sps->pic_width_in_mbs_minus1               = stdh264_bs_read_ue( s );  //  Width
    sps->pic_height_in_map_units_minus1        = stdh264_bs_read_ue( s );  //  Height
    sps->frame_mbs_only_flag                   = stdh264_bs_read( s, 1 );
    if (!sps->frame_mbs_only_flag)
    {
        sps->mb_adaptive_frame_field_flag      = stdh264_bs_read( s, 1 );
    }
    sps->direct_8x8_inference_flag             = stdh264_bs_read( s, 1 );
    sps->frame_cropping_flag                   = stdh264_bs_read( s, 1 );
    
    if (sps->frame_cropping_flag)
    {
        sps->frame_cropping_rect_left_offset   = stdh264_bs_read_ue( s );
        sps->frame_cropping_rect_right_offset  = stdh264_bs_read_ue( s );
        sps->frame_cropping_rect_top_offset    = stdh264_bs_read_ue( s );
        sps->frame_cropping_rect_bottom_offset = stdh264_bs_read_ue( s );
    }
    
    sps->vui_parameters_present_flag           = stdh264_bs_read( s, 1 );
    if (sps->vui_parameters_present_flag)
    {
        //OspPintf( 1, 0, "VUI sequence parameters present but not supported, ignored, proceeding to next NALU\n");
    }
    
    sps->bIsValid = TRUE;
    
    pStdH264Header->m_bIsValidSPS = TRUE;
    pStdH264Header->m_dwSPSId     = sps->seq_parameter_set_id;
    pStdH264Header->m_wWidth      = (u16)(sps->pic_width_in_mbs_minus1 + 1) * 16;
    pStdH264Header->m_wHeight     = (u16)(sps->pic_height_in_map_units_minus1 + 1) * 16;
    
    return TRUE;
}


/*=============================================================================
	������		��DecodeH264PPS
	����		������ h.264 �����е� pps ��Ϣ
	�㷨ʵ��	������ѡ�
	����ȫ�ֱ�������
	�������˵����
				   
	����ֵ˵���� TRUE - �ɹ�
=============================================================================*/
BOOL32 CMpBitStreamFilter::DecodeH264PPS( TBitStream *s, TPicParameterSetRBSP *pps, 
							              TKdvH264Header *pStdH264Header )
{
	u32 i;
	s32 NumberBitsPerSliceGroupId;
	
	pps->pic_parameter_set_id     = stdh264_bs_read_ue( s );
	pps->seq_parameter_set_id     = stdh264_bs_read_ue( s );
	pps->entropy_coding_mode_flag = stdh264_bs_read( s, 1 );
	pps->pic_order_present_flag   = stdh264_bs_read( s, 1 );
	pps->num_slice_groups_minus1  = stdh264_bs_read_ue( s );
	
	// FMO stuff begins here
    //hual modi 2005-06-24
    if (pps->num_slice_groups_minus1 > MAXnum_slice_groups_minus1)
    {
        return FALSE;
    }

    if (pps->num_slice_groups_minus1 > 0)
	{
		pps->slice_group_map_type = stdh264_bs_read_ue( s );

		switch( pps->slice_group_map_type )
		{
			case 0:
			{
				for (i=0; i<=pps->num_slice_groups_minus1; i++)
				{
					pps->run_length_minus1[i] = stdh264_bs_read_ue( s );
				}
				break;
			}
			case 2:
			{
				for (i=0; i<pps->num_slice_groups_minus1; i++)
				{
					//! JVT-F078: avoid reference of SPS by using ue(v) instead of u(v)
					pps->top_left [i]                  = stdh264_bs_read_ue( s );
					pps->bottom_right [i]              = stdh264_bs_read_ue( s );
				}
				break;
			}
			case 3:
			case 4:
			case 5:
			{
				pps->slice_group_change_direction_flag = stdh264_bs_read( s, 1 );
				pps->slice_group_change_rate_minus1    = stdh264_bs_read_ue( s );
				break;
			}
			case 6:
			{
				if (pps->num_slice_groups_minus1+1 >4)
				{
					NumberBitsPerSliceGroupId = 3;
				}
				else if (pps->num_slice_groups_minus1+1 > 2)
				{
					NumberBitsPerSliceGroupId = 2;
				}
				else
				{
					NumberBitsPerSliceGroupId = 1;
				}
				
				//! JVT-F078, exlicitly signal number of MBs in the map
				pps->num_slice_group_map_units_minus1      = stdh264_bs_read_ue( s );
                
                //hual add 2005-06-24
                if (NULL == pps->slice_group_id)
                {
                    break;
                }

				for (i=0; i<=pps->num_slice_group_map_units_minus1; i++)
				{
					pps->slice_group_id[i] = stdh264_bs_read(s,NumberBitsPerSliceGroupId );//maywrong
				}
				break;
			}
			default:
				break;
		}
	}
	
	// End of FMO stuff
	
	pps->num_ref_idx_l0_active_minus1           = stdh264_bs_read_ue( s );
	pps->num_ref_idx_l1_active_minus1           = stdh264_bs_read_ue( s );
	pps->weighted_pred_flag                     = stdh264_bs_read( s, 1 );
	pps->weighted_bipred_idc                    = stdh264_bs_read( s, 2 );
	pps->pic_init_qp_minus26                    = stdh264_bs_read_se( s );
	pps->pic_init_qs_minus26                    = stdh264_bs_read_se( s );
	pps->chroma_qp_index_offset                 = stdh264_bs_read_se( s );
	pps->deblocking_filter_control_present_flag = stdh264_bs_read( s, 1 );
	pps->constrained_intra_pred_flag            = stdh264_bs_read( s, 1 );
	pps->redundant_pic_cnt_present_flag         = stdh264_bs_read( s, 1 );
	
	pps->bIsValid = TRUE;
	pStdH264Header->m_bIsValidPPS = TRUE;

	return TRUE;
}


//END OF FILE
