/*****************************************************************************
   ģ����      : Kdv paint
   �ļ���      : kdvpaint.cpp
   ����ļ�    : kdvpaint.h, kdvpaintconst.h
   �ļ�ʵ�ֹ���: �����Ļ�ͼ����
   ����        : liuhuiyun
   �汾        : V0.9  Copyright(C) 2001-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2006/03/28  1.0         john          ����
******************************************************************************/
#include "drawbmp.h"

/*=============================================================================
�� �� ����CKdvGDI
��    �ܣ�CKdvGDI��Ĺ��캯��
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
���������
���������
�� �� ֵ��
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
CKdvGDI::CKdvGDI()
{
	m_bInitGDI = FALSE;
	m_dwBkColor = COLOR_WHITE;
	m_dwTextColor = COLOR_BLACK;
    m_pbyBmpDataBuff = NULL;
	m_byFontLibNum = 0;
	
	// ��ʼ���ֿ�
	memset( m_tKdvFontLib, 0, sizeof( m_tKdvFontLib ) );
  
	// ��ʼ����ɫ�弰��������
    memset( m_tPaletteEntry, 0, sizeof( m_tPaletteEntry ) );
	memset( m_abyPaletteIndex, 0, sizeof( m_abyPaletteIndex ) );

	// ��ʼ����ͼ��Դ
	memset( &m_tKdvFont, 0, sizeof( m_tKdvFont ) );
	memset( &m_tKdvPoint, 0, sizeof( m_tKdvPoint ) );

    // ��ʼ������
	m_bCreatePaint = FALSE;
	memset( &m_tKdvPaint, 0, sizeof( m_tKdvPaint ) );
}

/*=============================================================================
�� �� ����CKdvGDI
��    �ܣ�~CKdvGDI�����������
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
���������
���������
�� �� ֵ��
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
/*lint -save -e1551 -e1579*/
CKdvGDI::~CKdvGDI()
{
	QuitGDI();
}
/*lint -restore*/

/*=============================================================================
�� �� ����InitGDI
��    �ܣ���ʼ����ͼϵͳ
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
���������          
���������
�� �� ֵ��
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
BOOL32 CKdvGDI::InitGDI( void )
{
	if ( m_bInitGDI )  // ��ֹ��γ�ʼ��
	{
		return FALSE;
	}

	// ����ϵͳ��ɫ��
    u16 wLp = 0;
	for ( wLp = 0; wLp < PAINTCOLOR; wLp++ )
	{
		m_tPaletteEntry[ wLp ].byRed = PALETTE[ RGB_COLOR_MAKEUP * wLp ];
		m_tPaletteEntry[ wLp ].byGreen = PALETTE[ RGB_COLOR_MAKEUP * wLp + 1 ];
		m_tPaletteEntry[ wLp ].byBlue = PALETTE[ RGB_COLOR_MAKEUP * wLp + 2 ];
		m_tPaletteEntry[ wLp ].byFlags = 0;
	}

	// ��ʼ����ɫ��˳���
	for ( wLp = 0; wLp < PAINTCOLOR; wLp++ )
	{
		m_abyPaletteIndex[ wLp ] = (u8)wLp;
	}
	
	// ����,���ں�������
	PalSortByShell();
    
    // ����ͼ����Դ
    CreateBitMapRes();

	// �����ֿ�
	s8 achFontPath[ KDV_MAX_PATH ];
	memset(achFontPath, 0, sizeof(achFontPath));
	if ( !GetFontPath( achFontPath, KDV_MAX_PATH ) )
	{
        paintlog( LOG_LVL_ERROR, "[InitGDI]: Get Font Path Failed !\n");		
		return FALSE;
	}
	
	s8 achFontFullName[ KDV_MAX_PATH ];
	memset(achFontFullName, 0, sizeof(achFontFullName));
	sprintf(achFontFullName, "%s/%s", achFontPath, DEFAULT_FONTLIB_NAME);
	// �����ֿ�
	if ( !LoadFontLib( achFontFullName ) )
	{
        paintlog( LOG_LVL_ERROR, "[InitGDI]: Load Font Failed !\n");		
		return FALSE;
	}
    paintlog( LOG_LVL_KEYSTATUS, "[InitGDI]: Load Font Success !\n");

	// ��ʼ������,��ˢ,����
	memset( &m_tKdvPen, 0, sizeof( m_tKdvPen ) );
	m_tKdvPen.byWidth = 1;
	m_tKdvPen.dwColor = COLOR_BLACK;

	memset( &m_tKdvBrush, 0, sizeof( m_tKdvBrush ) );
	m_tKdvBrush.dwColor = COLOR_WHITE;

	memset( &m_tKdvFont, 0, sizeof( m_tKdvFont ) );
	m_tKdvFont.byType = FONT_KAI;
	m_tKdvFont.bySize = FONT_SIZE_32;

	m_bInitGDI = TRUE;

	return TRUE;
}

/*=============================================================================
�� �� ����QuitGDI
��    �ܣ��ͷ�GDI��Դ
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��������� 

���������
�� �� ֵ���ɹ���TRUE ʧ�ܷ���FALSE
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
BOOL32 CKdvGDI::QuitGDI( void )
{
	if ( !m_bInitGDI )
		return FALSE;

	// �ͷ��ֿ���Դ
	ReleaseFontLib();
	// �ͷ�ͼ����Դ
	ReleaseBitMapRes();

	memset( m_tPaletteEntry, 0, sizeof( m_tPaletteEntry ) );
	memset( m_abyPaletteIndex, 0, sizeof( m_abyPaletteIndex ) );

	m_dwTextColor = 0;

	memset( &m_tKdvPen, 0, sizeof( m_tKdvPen ) );
    memset( &m_tKdvBrush, 0, sizeof( m_tKdvBrush ) );
	memset( &m_tKdvFont, 0, sizeof( m_tKdvFont ) );
	memset( &m_tKdvPoint, 0, sizeof( m_tKdvPoint ) );

	m_bInitGDI = FALSE;

	m_bCreatePaint = FALSE;    
    memset( &m_tKdvPaint, 0, sizeof( m_tKdvPaint ) );

	return TRUE;
}

/*=============================================================================
  �� �� ���� GetFontPath
  ��    �ܣ� ȡ�ֿ�·��
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ����  OUT s8 *pachFontPath
             IN u16 wLen
  �� �� ֵ�� BOOL32 
=============================================================================*/
BOOL32 CKdvGDI::GetFontPath( OUT s8 *pachFontPath, IN u16 wBufLen  )
{
	if( NULL == pachFontPath )
	{
		return FALSE;
	}
	u16 wPathLen = strlen(FONT_PATH);
	if( wPathLen > wBufLen )
	{
        paintlog( LOG_LVL_KEYSTATUS, "[GetFontPath]: The Buffer for font path is too small!\n");
		return FALSE;
	}

	sprintf(pachFontPath, "%s", FONT_PATH);
	
	return TRUE;
}

/*=============================================================================
�� �� ����GetFontLibIndex
��    �ܣ���ȡ��ǰ��������ֿ��е�����
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
���������
��������� byLibIndex   ��ǰ��������ֿ��е�����
�� �� ֵ�� �ɹ�����TRUE ʧ�ܷ���FALSE
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
BOOL32 CKdvGDI::GetFontLibIndex( u8 &byLibIndex )
{
	BOOL32 bFound = FALSE;
	for ( u8 byLp = 0; byLp < m_byFontLibNum; byLp++ )
	{
		if ( m_tKdvFont.bySize == m_tKdvFontLib[ byLp ].byFontSize
			&& m_tKdvFont.byType == m_tKdvFontLib[ byLp ].byFontType )
		{
			bFound = TRUE;
			byLibIndex = byLp;
			break;
		}
	}
	return bFound;
}

/*=============================================================================
�� �� ����PalSortByShell
��    �ܣ��Ե�ɫ������
ע    �⣺
�㷨ʵ�֣���ĭ����
ȫ�ֱ�����
��������� 
���������
�� �� ֵ��
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
void CKdvGDI::PalSortByShell( void )
{
	u16 wOffSet = PAINTCOLOR;
	BOOL32 bOrder;
	while ( wOffSet > 1 )
	{
		wOffSet = wOffSet / 2;
		do 
		{
			bOrder = TRUE;
			for ( u16 wLp = 0, wLp1 = wOffSet; wLp < ( PAINTCOLOR - wOffSet ); wLp++,wLp1++ )
			{
				u8 byIndex = m_abyPaletteIndex[ wLp ];
				u32 dwPalCorlor1 = RGB( m_tPaletteEntry[ byIndex ].byRed, 
					m_tPaletteEntry[ byIndex ].byGreen,
					m_tPaletteEntry[ byIndex ].byBlue );

				byIndex = m_abyPaletteIndex[ wLp1 ];
				u32 dwPalCorlor2 = RGB( m_tPaletteEntry[ byIndex ].byRed, 
					m_tPaletteEntry[ byIndex ].byGreen,
					m_tPaletteEntry[ byIndex ].byBlue );
				if ( dwPalCorlor1 > dwPalCorlor2 )
				{
					u8 byTemp = m_abyPaletteIndex[ wLp1 ];
					m_abyPaletteIndex[ wLp1 ] = m_abyPaletteIndex[ wLp ];
					m_abyPaletteIndex[ wLp ] = byTemp;
					bOrder = FALSE;
				}

			}
		} while( !bOrder );
	}
	return;
}

/*=============================================================================
�� �� ����GetColorIndex
��    �ܣ�����RGB��ȡ��ɫ������
ע    �⣺
�㷨ʵ�֣����ֲ���
ȫ�ֱ�����
��������� dwColorValue RGBֵ
���������
�� �� ֵ��u8 ����ɫ�ڵ�ɫ����е�����
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
u8 CKdvGDI::GetColorIndex( u32 dwColorValue )
{
	u16 wLow = 0;
	u16 wHigh = PAINTCOLOR - 1;
	u16 wMid = 0;
	BOOL32 bFound = FALSE;
	u8 byIndex = 0;
	u32 dwPalColor;

	while ( wLow <= wHigh && !bFound )
	{
		wMid = ( wLow + wHigh ) / 2;
		byIndex = m_abyPaletteIndex[ wMid ];
		dwPalColor = RGB(  m_tPaletteEntry[ byIndex ].byRed, 
			m_tPaletteEntry[ byIndex ].byGreen,
			m_tPaletteEntry[ byIndex ].byBlue ); 

		if ( dwColorValue < dwPalColor )
		{	
			wHigh = wMid - 1;
		}
		else if ( dwColorValue > dwPalColor )
		{	
			wLow = wMid + 1;             
		}
		else
		{	
			bFound = TRUE; 
		}
	}

	//���û��, ȡ����ֵ
	if ( !bFound )
	{
		u16 wMin = 0; // wLow - 1;
		byIndex = m_abyPaletteIndex[ wMin ];
		u32 dwMinColor = RGB(  m_tPaletteEntry[ byIndex ].byRed, 
			m_tPaletteEntry[ byIndex ].byGreen,
			m_tPaletteEntry[ byIndex ].byBlue ); 

		for ( u16 wLp = wMin + 1; wLp <= wHigh + 1; wLp++ )
		{
			byIndex = m_abyPaletteIndex[ wLp ];
			dwPalColor = RGB(  m_tPaletteEntry[ byIndex ].byRed, 
				m_tPaletteEntry[ byIndex ].byGreen,
				m_tPaletteEntry[ byIndex ].byBlue ); 
			if ( abs( dwColorValue - dwPalColor ) < abs( dwColorValue - dwMinColor ) )
			{
				dwMinColor = dwPalColor;  // ����ӽ���ֵ
				wMin = wLp;
			}
		}
		wMid = wMin;
	}

	return m_abyPaletteIndex[ wMid ];
}

/*=============================================================================
�� �� ����SetCurrentFont
��    �ܣ����õ�ǰ��������
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��������� tKdvFont �µ�����
��������� 
�� �� ֵ���ɵ�����
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
TKDVFONT CKdvGDI::SetCurrentFont( TKDVFONT &tKdvFont )
{
	TKDVFONT tCurKdvFont = *GetCurrentFont();
	memset( &m_tKdvFont, 0, sizeof( m_tKdvFont ) );
	memcpy( &m_tKdvFont, &tKdvFont, sizeof( m_tKdvFont ) );
	return tCurKdvFont;
}

/*=============================================================================
�� �� ����SetTextColor
��    �ܣ����õ�ǰ�ı���ɫ
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��������� dwColorValue �µ��ı���ɫ
��������� 
�� �� ֵ�� �ɵ��ı���ɫ
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
u32 CKdvGDI::SetTextColor( u32 dwColorValue )
{
	u32 dwCurTextColor = m_dwTextColor;
	m_dwTextColor = dwColorValue;
	return dwCurTextColor;
}

/*=============================================================================
�� �� ����SetCurrentBrush
��    �ܣ����õ�ǰ��ˢ����
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��������� tKdvBrush �µĻ�ˢ
��������� 
�� �� ֵ���ɵĻ�ˢ
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
TKDVBRUSH CKdvGDI::SetCurrentBrush( TKDVBRUSH &tKdvBrush )
{

	TKDVBRUSH tCurKdvBrush = *GetCurrentBrush();
	memset( &m_tKdvBrush, 0, sizeof( m_tKdvBrush ) );
	memcpy( &m_tKdvBrush, &tKdvBrush, sizeof( m_tKdvBrush ) );

	return tCurKdvBrush;
}

/*=============================================================================
�� �� ����SetCurrentPen
��    �ܣ����õ�ǰ��������
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��������� tKdvPen �µĻ���
��������� 
�� �� ֵ���ɵĻ���
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
TKDVPEN CKdvGDI::SetCurrentPen( TKDVPEN &tKdvPen )
{
	TKDVPEN tCurKdvPen = *GetCurrentPen();
	memset( &m_tKdvPen, 0, sizeof( m_tKdvPen ) );
	memcpy( &m_tKdvPen, &tKdvPen, sizeof( m_tKdvPen ) );
	return tCurKdvPen;
}

/*=============================================================================
  �� �� ���� SetCurrentPaint
  ��    �ܣ� ���õ�ǰ����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� TKDVPAINT tPaint
  �� �� ֵ�� TKDVPAINT 
=============================================================================*/
TKDVPAINT CKdvGDI::SetCurrentPaint(TKDVPAINT tPaint)
{
	TKDVPAINT tRetValue = m_tKdvPaint;
	memset(&m_tKdvPaint, 0, sizeof(m_tKdvPaint));
	m_tKdvPaint = tPaint;
	return tRetValue;
} 
/*=============================================================================
�� �� ����SetBkColor
��    �ܣ����õ�ǰ��������ɫ
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��������� dwBkColor �µı���ɫ
��������� 
�� �� ֵ�� �ɵı���ɫ
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
u32 CKdvGDI::SetBkColor( u32 dwBkColor )
{
    if ( !m_bInitGDI )
	{
        paintlog( LOG_LVL_ERROR, "[SetBkColor]: Invalid input param!\n");
		return INVALID_COLOR;
	}

	u32 dwOldBkColor = m_dwBkColor; // ����ԭ�б���ɫ

	u8 byClrIndex = GetColorIndex( dwBkColor );
	u32 dwSize =  m_tKdvPaint.dwWidth * m_tKdvPaint.dwHeight;

	memset( m_pbyBmpDataBuff, byClrIndex, dwSize );  // ���ñ���ɫ

	m_dwBkColor = RGB( m_tPaletteEntry[ byClrIndex ].byRed, 
					   m_tPaletteEntry[ byClrIndex ].byGreen,
					   m_tPaletteEntry[ byClrIndex ].byBlue );

	return dwOldBkColor;

}

/*=============================================================================
�� �� ����SetPixel
��    �ܣ�����
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��������� nPosX nPosY   ���ڻ����е�����
           dwColorValue  RGBֵ
��������� 
�� �� ֵ�� ���RGBֵ
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
u32 CKdvGDI::SetPixel( s32 nPosX, s32 nPosY, u32 dwColorValue )
{
	u32 dwRet = INVALID_COLOR;
	// �������
	if ( nPosX >= (s32)m_tKdvPaint.dwWidth || nPosX < 0 )
	{	
		return dwRet;
	}
	if ( nPosY >= (s32)m_tKdvPaint.dwHeight || nPosY < 0 )
	{
		return dwRet;
	}
	// ȡ��ɫ��Ӧ��ɫ���е����
	u8 byClrIndex = GetColorIndex( dwColorValue );

	dwRet = dwColorValue;

	// Ĭ��Ϊ8*8λ
	u32 dwPos = ( m_tKdvPaint.dwHeight - 1 - nPosY ) * m_tKdvPaint.dwWidth + nPosX ;
	if ( NULL == (m_pbyBmpDataBuff + dwPos) )
	{
		dwRet = INVALID_COLOR;
	}
	else
	{
		m_pbyBmpDataBuff[ dwPos ] = byClrIndex;  // ��Ӧ���¼��ɫ���
	}

	return dwRet;
}

/*=============================================================================
�� �� ����SetPixel
��    �ܣ�����
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
��������� tPoint      �õ��ڻ����е�����
           dwColor     RGBֵ
��������� 
�� �� ֵ�� ���RGBֵ
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
u32 CKdvGDI::SetPixel( TPOINT tPoint, u32 dwColor )
{
	return SetPixel( tPoint.nPosX, tPoint.nPosY, dwColor );
}

/*=============================================================================
�� �� ����LineTo
��    �ܣ�����,�ӵ�ǰλ�õ�ָ��λ��
ע    �⣺
�㷨ʵ�֣�����ֱ�߻����㷨
ȫ�ֱ�����
��������� nPosX  nPosY   �ߵ��յ�����        
��������� 
�� �� ֵ���ɹ�����TRUE ʧ�ܷ���FALSE
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
BOOL32 CKdvGDI::LineTo( s32 nPosX, s32 nPosY )
{	

	s32 nP, nN, nTmpX, nTmpY, nTn;

	// �Ƿ�Խ��
	if ( nPosX >= (s32)m_tKdvPaint.dwWidth || nPosX < 0 )
	{
		return FALSE;
	}

	if ( nPosY >= (s32)m_tKdvPaint.dwHeight || nPosY < 0 )
	{
		return FALSE;
	}

	// ���浱ǰ���λ��
	TPOINT tCurPoint;
	memcpy( &tCurPoint, &m_tKdvPoint, sizeof( tCurPoint ) );

	if( m_tKdvPoint.nPosY == nPosY )
	{
		if( m_tKdvPoint.nPosX > nPosX ) // �������һ���
		{
			nTmpX = nPosX;
			nPosX = m_tKdvPoint.nPosX;
			m_tKdvPoint.nPosX = nTmpX;
		}
		return LevelLine( m_tKdvPoint.nPosX,
			              m_tKdvPoint.nPosY,
			              nPosX - m_tKdvPoint.nPosX + 1 ); // ˮƽ��
	}
	else if( m_tKdvPoint.nPosX == nPosX )
	{
		if( m_tKdvPoint.nPosY > nPosY ) // �������ϻ���
		{
			nTmpY = nPosY;
			nPosY = m_tKdvPoint.nPosY;
			m_tKdvPoint.nPosY = nTmpY;
		}
		return VertLine( m_tKdvPoint.nPosX,
			             m_tKdvPoint.nPosY,
			             nPosY - m_tKdvPoint.nPosY + 1 ); // ��ֱ��
	}
	else if( abs( nPosY - m_tKdvPoint.nPosY ) <= abs( nPosX - m_tKdvPoint.nPosX ) )  // ��x��ļн�С�ڵ���45��
	{
		// ��֤m_tKdvPoint��ָ��������
		if( ( nPosY < m_tKdvPoint.nPosY && nPosX < m_tKdvPoint.nPosX ) 
			||( m_tKdvPoint.nPosY <= nPosY && m_tKdvPoint.nPosX > nPosX ) )
		{
			nTmpX = nPosX;
			nTmpY = nPosY;
			nPosX = m_tKdvPoint.nPosX;
			nPosY = m_tKdvPoint.nPosY;
			m_tKdvPoint.nPosX = nTmpX;
			m_tKdvPoint.nPosY = nTmpY;
		}

		if( nPosY >= m_tKdvPoint.nPosY && nPosX >= m_tKdvPoint.nPosX )  // ָ������ԭ�������
		{
			nTmpX = nPosX-m_tKdvPoint.nPosX;
			nTmpY = nPosY-m_tKdvPoint.nPosY;
			nP = 2 * nTmpY;
			nN = 2 * nTmpX - 2 * nTmpY;
			nTn = nTmpX;
			while( m_tKdvPoint.nPosX <= nPosX )
			{
				if( nTn >= 0 )
				{	nTn -= nP; }
				else 
				{
					nTn += nN;
					m_tKdvPoint.nPosY++;
				}

				s32 nX = m_tKdvPoint.nPosX++;
				for ( u8 byLp = 0; byLp < m_tKdvPen.byWidth; byLp++ )
				{
					if ( byLp % 2 == 0 )
					{
						SetPixel( nX + byLp, 
						          m_tKdvPoint.nPosY,
						          m_tKdvPen.dwColor );
					}
					else
					{
						SetPixel( nX - byLp, 
						          m_tKdvPoint.nPosY,
						          m_tKdvPen.dwColor );
					}
				}
			}
		}
		else   // ����
		{
			nTmpX = nPosX - m_tKdvPoint.nPosX;
			nTmpY = nPosY - m_tKdvPoint.nPosY;
			nP = -2 * nTmpY;
			nN = 2 * nTmpX + 2 * nTmpY;
			nTn = nTmpX;
			while( m_tKdvPoint.nPosX <= nPosX )
			{
				if( nTn >= 0 )
				{
					nTn -= nP;
				}
				else 
				{
					nTn += nN;
					m_tKdvPoint.nPosY--;
				}
				
				s32 nX = m_tKdvPoint.nPosX++;
				for ( u8 byLp = 0; byLp < m_tKdvPen.byWidth; byLp++ )
				{
					if ( byLp % 2 == 0 )
					{	SetPixel( nX + byLp,
								  m_tKdvPoint.nPosY,
								  m_tKdvPen.dwColor );
					}
					else
					{
						SetPixel( nX - byLp,
								  m_tKdvPoint.nPosY,
								  m_tKdvPen.dwColor );
					}
				}
			}
		}
	}
	else
	{
		nTmpX = m_tKdvPoint.nPosX;
		m_tKdvPoint.nPosX = nPosY;
		nPosY = nTmpX;
		nTmpY = m_tKdvPoint.nPosY;
		m_tKdvPoint.nPosY = nPosX;
		nPosX = nTmpY;
		if( ( nPosY < m_tKdvPoint.nPosY && nPosX < m_tKdvPoint.nPosX )
			||( m_tKdvPoint.nPosY <= nPosY && m_tKdvPoint.nPosX > nPosX ) )
		{
			nTmpX = nPosX;
			nTmpY = nPosY;
			nPosX = m_tKdvPoint.nPosX;
			nPosY = m_tKdvPoint.nPosY;
			m_tKdvPoint.nPosX = nTmpX;
			m_tKdvPoint.nPosY = nTmpY;
		}

		if( nPosY >= m_tKdvPoint.nPosY && nPosX >= m_tKdvPoint.nPosX )
		{
			nTmpX = nPosX-m_tKdvPoint.nPosX;
			nTmpY = nPosY-m_tKdvPoint.nPosY;
			nP = 2 * nTmpY;
			nN = 2 * nTmpX - 2 * nTmpY;
			nTn = nTmpX;
			while( m_tKdvPoint.nPosX <= nPosX )
			{
				if( nTn >= 0)
				{	
					nTn -= nP;
				}
				else 
				{ 
					nTn += nN;
					m_tKdvPoint.nPosY++;
				}

				s32 nY = m_tKdvPoint.nPosX++;
				for ( u8 byLp = 0; byLp < m_tKdvPen.byWidth; byLp++ )
				{
					if ( byLp % 2 == 0 )
					{
						SetPixel( m_tKdvPoint.nPosY,
						          nY + byLp,
								  m_tKdvPen.dwColor );
					}
					else
					{
						SetPixel( m_tKdvPoint.nPosY,
								  nY - byLp,
								  m_tKdvPen.dwColor );
					}
				}
			}
		}
		else
		{
			nTmpX = nPosX - m_tKdvPoint.nPosX;
			nTmpY = nPosY - m_tKdvPoint.nPosY;
			nP = -2 * nTmpY;
			nN = 2 * nTmpX + 2 * nTmpY;
			nTn = nTmpX;
			while( m_tKdvPoint.nPosX <= nPosX )
			{
				if( nTn >= 0 )
				{
					nTn -= nP;
				}
				else 
				{
					nTn += nN;
					m_tKdvPoint.nPosY--;
				}

				s32 nY = m_tKdvPoint.nPosX++;
				for ( u8 byLp = 0; byLp < m_tKdvPen.byWidth; byLp++ )
				{
					if ( byLp % 2 == 0 )
						SetPixel( m_tKdvPoint.nPosY,
								  nY + byLp,
								  m_tKdvPen.dwColor );
					else
						SetPixel( m_tKdvPoint.nPosY,
						          nY - byLp,
								  m_tKdvPen.dwColor );

				}
			}
		}
	}

	// �ָ���ǰ���λ��
	memcpy( &m_tKdvPoint, &tCurPoint, sizeof( m_tKdvPoint ) );

	return TRUE;
}

/*=============================================================================
�� �� ����LineTo
��    �ܣ�����,�ӵ�ǰλ�õ�ָ��λ��
ע    �⣺
�㷨ʵ�֣�����ֱ�߻����㷨
ȫ�ֱ�����
���������tPoint  �ߵ��յ�����        
��������� 
�� �� ֵ���ɹ�����TRUE ʧ�ܷ���FALSE
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
BOOL32 CKdvGDI::LineTo( TPOINT tPoint )
{
	return LineTo( tPoint.nPosX, tPoint.nPosY );
}

/*=============================================================================
�� �� ����LevelLine
��    �ܣ���ˮƽ��
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
���������nPosX nPosY  �յ������
          dwWidth      ˮƽ�ߵĿ��
���������
�� �� ֵ�� �ɹ�����TRUE ʧ�ܷ���FALSE
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
BOOL32 CKdvGDI::LevelLine( int nPosX, int nPosY, u32 dwWidth )
{
	// ����У��
	if ( nPosY > (int)m_tKdvPaint.dwHeight || nPosY < 0 )
	{
		return FALSE;
	}

	if ( nPosX + dwWidth > m_tKdvPaint.dwWidth + 1 || nPosX < 0 )
	{
		return FALSE;
	}

	u8 byClrIndex = GetColorIndex( m_tKdvPen.dwColor );

	for ( int byLp = 0; byLp < m_tKdvPen.byWidth; byLp++ )
	{
		
		u32 dwPos = ( m_tKdvPaint.dwHeight - 1 - nPosY ) * m_tKdvPaint.dwWidth + nPosX ;
		if ( NULL == m_pbyBmpDataBuff + dwPos )
		{
			continue;
		}

		memset( m_pbyBmpDataBuff + dwPos, byClrIndex, dwWidth );
		nPosY++;
	}
	return TRUE;
}


/*=============================================================================
�� �� ����VertLine
��    �ܣ�����ֱ��
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
���������nPosX nPosY  �յ������
          dwWidth      ˮƽ�ߵĿ��
���������
�� �� ֵ�� �ɹ�����TRUE ʧ�ܷ���FALSE
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
BOOL32 CKdvGDI::VertLine( int nPosX, int nPosY, u32 dwHeight )
{
	// ����У��
	if ( nPosX > (int)m_tKdvPaint.dwWidth || nPosX < 0 )
	{
		return FALSE;
	}

	if ( nPosY + dwHeight > m_tKdvPaint.dwHeight + 1 || nPosY < 0 )
	{
		return FALSE;
	}

	for ( u8 byOutLp = 0; byOutLp < m_tKdvPen.byWidth; byOutLp++ )
	{
		for ( u8 byInLp = 0; byInLp < dwHeight; byInLp++ )
		{
			u32 dwColorValue = SetPixel( nPosX + byOutLp, nPosY + byInLp, m_tKdvPen.dwColor );
			if ( INVALID_COLOR == dwColorValue  )
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

/*=============================================================================
�� �� ����MoveTo
��    �ܣ��ı䵱ǰλ��
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
���������nPosX nPosY �µ�λ�õ�����       
��������� 
�� �� ֵ��TPOINT  �ɵ�λ�õ�����
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
TPOINT CKdvGDI::MoveTo( s32 nPosX, s32 nPosY )
{
	TPOINT tCurPoint;

	memset( &tCurPoint, 0, sizeof( tCurPoint ) );
	memcpy( &tCurPoint, &m_tKdvPoint, sizeof( tCurPoint ) );

	m_tKdvPoint.nPosX = nPosX;
	m_tKdvPoint.nPosY = nPosY;

	return tCurPoint;
}

/*=============================================================================
�� �� ����MoveTo
��    �ܣ��ı䵱ǰλ��
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
���������tPoint �µ�λ�õ�����         
��������� 
�� �� ֵ��TPOINT  �ɵ�λ�õ�����
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
TPOINT CKdvGDI::MoveTo( TPOINT tPoint )
{
	return MoveTo( tPoint.nPosX, tPoint.nPosY );
}

/*=============================================================================
�� �� ����DrawRect
��    �ܣ�������
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
���������nPosX nPosY �µ�λ�õ�����
          dwWidth     ���εĿ��
		  dwHeight    ���εĳ���
��������� 
�� �� ֵ���ɹ�����TRUE ʧ�ܷ���FALSE
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
BOOL32 CKdvGDI::DrawRect( s32 nPosX, s32 nPosY, u32 dwWidth, u32 dwHeight )
{
	// Ԥ��Խ��
	if ( nPosX < 0  || nPosX + dwWidth > m_tKdvPaint.dwWidth )
	{	
		return FALSE;
	}

	if ( nPosY < 0 || nPosY + dwHeight > m_tKdvPaint.dwHeight )
	{
		return FALSE;
	}

	dwWidth -= 1;  //  Ԥ���߽�
	dwHeight -= 1; // 

	// ���浱ǰ���λ��
	TPOINT tLastPoint = MoveTo( nPosX, nPosY );


	// ���ƾ���

	// �ϱ�
	BOOL32 bRet = LineTo( nPosX + dwWidth, nPosY );
	if ( !bRet )
	{
        paintlog( LOG_LVL_KEYSTATUS, "[DrawRect]: Fail to call LineTo()! Pos: top !\n");
		return FALSE;
	}

	// ���
	bRet = LineTo( nPosX, nPosY + dwHeight );
	if ( !bRet )
	{
        paintlog( LOG_LVL_KEYSTATUS, "[DrawRect]: Fail to call LineTo()! Pos: left !\n");
		return FALSE;
	}

	// �ұ�
	MoveTo( nPosX + dwWidth - m_tKdvPen.byWidth + 1, nPosY );
	bRet = LineTo( nPosX + dwWidth - m_tKdvPen.byWidth + 1, 
		nPosY + dwHeight - m_tKdvPen.byWidth + 1 );
	if ( !bRet )
	{
        paintlog( LOG_LVL_KEYSTATUS, "[DrawRect]: Fail to call LineTo()! Pos: right !\n");
		return FALSE;
	}

	// �±�
	MoveTo( nPosX, nPosY + dwHeight - m_tKdvPen.byWidth + 1 );
	bRet = LineTo( nPosX + dwWidth - m_tKdvPen.byWidth + 1, 
		nPosY + dwHeight - m_tKdvPen.byWidth + 1 );
	if ( !bRet )
	{
        paintlog( LOG_LVL_KEYSTATUS, "[DrawRect]: Fail to call LineTo()! Pos: bottom !\n");
		return FALSE;
	}

	// ���Ƴ��Ϳ�ӿڴ�
	MoveTo( nPosX + dwWidth - m_tKdvPen.byWidth + 1, 
		nPosY + dwHeight - m_tKdvPen.byWidth + 1 );
	bRet = LineTo( nPosX + dwWidth - m_tKdvPen.byWidth + 1, nPosY + dwHeight );
	if ( !bRet )
	{
        paintlog( LOG_LVL_KEYSTATUS,   "[DrawRect]: Fail to call LineTo()! Pos: interface !\n");
		return FALSE;
	}

	// �ָ���ǰ���λ��
	MoveTo( tLastPoint );

	return TRUE;
}

/*=============================================================================
�� �� ����DrawRect
��    �ܣ�������
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
���������tRect ��������
��������� 
�� �� ֵ���ɹ�����TRUE ʧ�ܷ���FALSE
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
BOOL32 CKdvGDI::DrawRect( TRECT tRect )
{
	INITIALIZED(m_bInitGDI);
	
	// Ԥ��Խ��(����ֻ���ڻ�����Χ֮��)
	if ( tRect.nPosLeft < 0 || tRect.nPosRight < tRect.nPosLeft )
	{
		return FALSE;
	}

	if ( tRect.nPosTop < 0 || tRect.nPosBottom < tRect.nPosTop )
	{
		return FALSE;
	}

	return DrawRect( tRect.nPosLeft, tRect.nPosTop,
		tRect.nPosRight - tRect.nPosLeft,
		tRect.nPosBottom - tRect.nPosTop );
}

/*=============================================================================
�� �� ����FillRect
��    �ܣ�������
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
���������nPosX nPosY �������϶��������
          dwWidth     ���ο��
          dwHeight    ���γ���
��������� 
�� �� ֵ���ɹ�����TRUE ʧ�ܷ���FALSE
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
BOOL32 CKdvGDI::FillRect( int nPosX, int nPosY, u32 dwWidth, u32 dwHeight )
{
	INITIALIZED(m_bInitGDI);

	// Ԥ��Խ��(����ֻ���ڻ�����Χ֮��)
	if ( nPosX < 0 || nPosX + dwWidth > m_tKdvPaint.dwWidth )
	{
		return FALSE;
	}

	if ( nPosY < 0  || nPosY + dwHeight > m_tKdvPaint.dwHeight )
	{
		return FALSE;
	}

	// ���浱ǰ���λ��
	TPOINT tLastPoint = MoveTo( nPosX, nPosY );
	TKDVPEN tLastKdvPen  = *GetCurrentPen();

	// ʹ�û��������ɫ
	TKDVPEN tKdvPen;
	memset( &tKdvPen, 0, sizeof( tKdvPen ) );
	tKdvPen.byWidth = MAX_BYTE_FILL;
	tKdvPen.dwColor = m_tKdvBrush.dwColor;
	SetCurrentPen( tKdvPen );

	u32 dwCount = dwHeight / MAX_BYTE_FILL;
	for ( u16 wLp = 0; wLp < dwCount; wLp++ )
	{
		// ������
		MoveTo( nPosX, nPosY + MAX_BYTE_FILL * wLp );
		LineTo( nPosX + dwWidth - 1, nPosY + MAX_BYTE_FILL * wLp );
	}

	// ������һ����
	MoveTo( nPosX, nPosY + MAX_BYTE_FILL * dwCount );
	tKdvPen.byWidth = ( u8 )(dwHeight - dwCount * MAX_BYTE_FILL);
	SetCurrentPen( tKdvPen );
	
	LineTo( nPosX + dwWidth - 1, nPosY + MAX_BYTE_FILL * dwCount );// ������

	MoveTo( tLastPoint );// �ָ���ǰ���λ��
	
	SetCurrentPen( tLastKdvPen );// �ָ���ǰ����

	return TRUE;
}

/*=============================================================================
�� �� ����FillRect
��    �ܣ�������
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
���������tRect ��������
��������� 
�� �� ֵ���ɹ�����TRUE ʧ�ܷ���FALSE
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
BOOL32 CKdvGDI::FillRect( TRECT tRect )
{
	return FillRect( tRect.nPosLeft, tRect.nPosTop,
		tRect.nPosRight - tRect.nPosLeft,
		tRect.nPosBottom - tRect.nPosTop );
}

/*=============================================================================
�� �� ����DrawTextInRect
��    �ܣ�����ַ��������ο���
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
���������pchText       Ҫ������ַ���
          dwTextLen     �ַ�������
          tRect         ��������
��������� 
�� �� ֵ������ʵ�ʵ��ı��������
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
u32 CKdvGDI::TextOutInRect( const char* pchText, u32 dwTextLen, TRECT tRect )
{
	u32 dwActTextLen = 0;
	// ��ȡ��������
	u8 byLibIndex = 0;
	BOOL32 bRet = GetFontLibIndex( byLibIndex );
	if ( !bRet )
	{
		return dwActTextLen;
	}
	// ȡ������Ϣ
	TKDVFONTLIB *ptFontLib = &m_tKdvFontLib[ byLibIndex ];
	
	if ( tRect.nPosBottom - tRect.nPosTop < ptFontLib->byHeight * 8 )
	{
		return dwActTextLen;
	}
	
	// ����У��
	if ( tRect.nPosLeft < 0 
		|| tRect.nPosTop < 0
		|| ( tRect.nPosRight - tRect.nPosLeft ) > (s32)m_tKdvPaint.dwWidth 
		|| ( tRect.nPosBottom - tRect.nPosTop ) > (s32)m_tKdvPaint.dwHeight )

	{
		return dwActTextLen;
	}

	// ʵ�ʿ����ɵ��ַ���Ŀ
	dwActTextLen = 2 * ( tRect.nPosRight - tRect.nPosLeft ) / ( ptFontLib->byWidth * 8 );

	if ( dwActTextLen > dwTextLen )
	{
		dwActTextLen = dwTextLen;
	}

	// ���ִ�
	return TextOut( tRect.nPosLeft, tRect.nPosTop, pchText, dwActTextLen );
}

/*=============================================================================
�� �� ����TextOut
��    �ܣ�����ı�
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
���������nPosX nPosY   ����ı����������
          pchText       Ҫ������ַ���
          dwTextLen     �ַ�������
��������� 
�� �� ֵ������ʵ�ʵ��ı��������
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
u32 CKdvGDI::TextOut( int nPosX, int nPosY, const char* pchText, u32 dwTextLen )
{
	u32 dwActTextLen = 0; // ʵ����ʾ�ı�����
	// ����У��
	if ( NULL == pchText )
	{
		return dwActTextLen;
	}

	if ( nPosX >= (int)m_tKdvPaint.dwWidth || nPosX < 0 )
	{
		return dwActTextLen;
	}
	if ( nPosY >= (int)m_tKdvPaint.dwHeight || nPosY < 0 )
	{
		return dwActTextLen;
	}
	
	u8 byCurChar;		// ��ǰ��ȡ���ַ�
	u8 byCompress;		// �Ƿ�ѹ��

	u8 byArea;		// ����
	u8 byPosi;		// λ��
	u32 dwFontIndex;	// �ַ����ֿ��е�����

	u8 byLibIndex = 0;
	BOOL bRet = GetFontLibIndex( byLibIndex );
	if ( !bRet )
	{
		return dwActTextLen;
	}

	TKDVFONTLIB *ptFontLib = &m_tKdvFontLib[byLibIndex];
	
	while( dwTextLen > 0 )
	{
		if ( NULL == pchText )
		{
			dwTextLen--;
			continue;
		}

		dwActTextLen++;
		byCurChar = *( ( u8* ) pchText++ );
		if ( byCurChar == ' ' )
		{
			dwTextLen--;
			nPosX += ( 8 * ptFontLib->byWidth  ) >> 1;
			continue;
		}

		if ( byCurChar < 32 )  // ������ʾ�ַ�
		{
			dwTextLen--;
			continue;
		}

	    u8* pbyFontData = NULL;
		if ( byCurChar < 161 )
		{
			if ( NULL == ptFontLib->pbyENData )
			{
				nPosX += ( 8 * ptFontLib->byWidth  ) >> 1;
				dwTextLen--;
				continue;
			}

			dwFontIndex = ( byCurChar - 32 ) * ptFontLib->wSize;
			byCompress = 1;

            pbyFontData =  ptFontLib->pbyENData+dwFontIndex;
		}		 
		else  //�����ַ�
		{
			byArea = byCurChar - 0xA1;   // �����λ��
			dwTextLen--;
			if ( 0 == dwTextLen )
			{
				break;
			}

			byPosi = *( ( u8* )pchText++ ) - 0xA1;
			byCompress = 0;
			dwActTextLen++;
			dwFontIndex = byArea * 94 + byPosi; //2016E,2007D
			dwFontIndex *= ptFontLib->wSize;

			pbyFontData =  ptFontLib->pbyCNData+dwFontIndex;

		}

		// �����ֿ����ʾ
		s32 nbyte = 0;
		s32 nScaleX = 1;
		s32 nScaleY = 1;
		
		for ( s32 nLp = 0, cx = nPosX, cy = nPosY; 
			nLp < ptFontLib->byHeight * 8; nLp++, cy += nScaleY, cx = nPosX )
		{
			for ( s32 nLp1 = 0; nLp1 < ptFontLib->byWidth; nLp1++, nbyte++ )
			{
				for ( s32 nLp2 = 7; nLp2 >= 0; nLp2 -= ( byCompress + 1 ), cx += nScaleX )
				{
					if ( 1  == ( ( pbyFontData[ nbyte ] >> nLp2 ) & 1 ))
					{
						SetPixel( cx , cy, m_dwTextColor );
					}
				}
			}
		}
		nPosX += (( ptFontLib->byWidth * 8) >> byCompress );
		dwTextLen--;			
	}

	return dwActTextLen;
}

/*=============================================================================
�� �� ����LoadFontLib
��    �ܣ������ֿ⣬����Ӧ���ֿ����ݶ����ڴ�
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
���������pchFontPath �ļ���
��������� 
�� �� ֵ���ɹ�����TRUE ʧ�ܷ���FALSE
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
BOOL32 CKdvGDI::LoadFontLib( const char* pchFontLibName )
{	
	if ( NULL == pchFontLibName )
	{
        paintlog( LOG_LVL_ERROR, "[LoadFontLib]: Invalide input param!\n" );
		return FALSE;
	}
	/*lint -save -esym(438, hRead)*/
    // ���ֿ�   (û�ж��ֿ��ļ�ѹ���� ���ֿ�ϴ�ʱ���ɿ���ѹ��)
   	FILE *hRead = fopen( pchFontLibName, "rb" );
	if ( NULL == hRead )
	{	
        paintlog( LOG_LVL_ERROR, "[LoadFontLib]: Fail to open the font lib file: %s!\n", pchFontLibName );
		return FALSE;
	}
    
	// �ļ���С
	fseek( hRead, 0, SEEK_END ); 
	s32 nFileLen = ftell(hRead);
	if ( INIT_VALUE == nFileLen )
	{
		fclose( hRead );
		return FALSE;
	}
	/*lint -save -esym(438, pbyDataBuf)*/
	u8 *pbyDataBuf = new u8[ nFileLen + 1 ]; // ѹ����
	if ( NULL == pbyDataBuf )
	{
        paintlog( LOG_LVL_ERROR, "[LoadFontLib]: Open FontLib File Failure!\n");
        fclose( hRead );
		return FALSE;
	}

	memset( pbyDataBuf, 0xFF, nFileLen + 1 );
	fseek( hRead, 0, SEEK_SET );
	fread ( pbyDataBuf, nFileLen, 1, hRead);

	u8* pbyCurPos = pbyDataBuf; // ��¼��ǰλ��

	while(1)
	{
		u8 byFontLibType = 0;// �ֿ�����(1�ֽ�)
		memcpy( ( void* )&byFontLibType, ( void * )pbyCurPos, sizeof( byFontLibType ) );
		if( 0xff == byFontLibType )  // �����ֿ������־
        {
            paintlog( LOG_LVL_DETAIL, "[LoadFontLib]: Read the end of font lib file!\n");
			break;
		}
		
		u8 byFontSize = 0; // �����С
		memcpy( ( void* )&byFontSize, ( void * )( pbyCurPos + 1 ), sizeof( byFontSize ) );// �����С(1�ֽ�)
		
		u8 byFontType = 0;
		memcpy( ( void* )&byFontType, ( void * )( pbyCurPos + 2 ), sizeof( byFontType ) );// ��������(1�ֽ�)
		
		u32 dwChineseSize = 0;
		memcpy( ( void* )&dwChineseSize, ( void * )(pbyCurPos + 3), sizeof( dwChineseSize ) );// ���ֿ��С(4�ֽ�)
		
		u32 dwEngSize = 0;
		memcpy( ( void* )&dwEngSize, ( void * )(pbyCurPos + 7), sizeof( dwEngSize ) );// �����ֿ��С(4�ֽ�)

		TKDVFONTLIB *ptKdvFontLib = &m_tKdvFontLib[m_byFontLibNum];
		m_byFontLibNum++;
		
		switch( byFontLibType )
		{
		case FT_HZK16 :
			ptKdvFontLib->byWidth = 2;
			ptKdvFontLib->byHeight = 2;
			ptKdvFontLib->wSize = 32;  // 2*2 = 4 bytes , 4*8 = 32 bits
			break;
			
		case FT_HZK24 :
			ptKdvFontLib->byWidth = 3;
			ptKdvFontLib->byHeight = 3;
			ptKdvFontLib->wSize = 72;  // 3*3 = 9 bytes, 9*8 = 72 bits
			break;
			
		case FT_HZK32 :
			ptKdvFontLib->byWidth = 4;
			ptKdvFontLib->byHeight = 4;
			ptKdvFontLib->wSize = 128; // 4*4 = 16 bytes, 16*8 = 128 bits
			break;			
			
		default :
			return FALSE;
			
		}

		ptKdvFontLib->byType = byFontLibType;
		ptKdvFontLib->byFontType = byFontType;
		ptKdvFontLib->byFontSize = byFontSize;
		
		// �����ֿ�
		if ( NULL != ptKdvFontLib->pbyCNData )
		{
			delete []ptKdvFontLib->pbyCNData;
			ptKdvFontLib->pbyCNData = NULL;
		}
		ptKdvFontLib->pbyCNData = new u8[ dwChineseSize ];

		memcpy( ( void* )ptKdvFontLib->pbyCNData, ( void * )(pbyCurPos + 11), dwChineseSize );
		
		// �������ֿ�
		if ( NULL != ptKdvFontLib->pbyENData )
		{
			delete []ptKdvFontLib->pbyENData;
			ptKdvFontLib->pbyENData = NULL;
		}
		ptKdvFontLib->pbyENData = new u8[ dwEngSize ];

		memcpy( ( void* )ptKdvFontLib->pbyENData, 
				( void * )(pbyCurPos + 11 + dwChineseSize), dwEngSize );

		pbyCurPos = pbyCurPos + 11 + dwChineseSize + dwEngSize;  // �ƶ�ָ��
	}
	
	if ( NULL != pbyDataBuf )
	{
		delete []pbyDataBuf;
		pbyDataBuf = NULL;
	}
	
   	fclose( hRead ); 
	hRead = NULL;
    /*lint -restore*/

	return TRUE;
}

/*=============================================================================
�� �� ����ReleaseFontLib
��    �ܣ��ͷ��ֿ���Դ
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
���������
��������� 
�� �� ֵ��
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
void CKdvGDI::ReleaseFontLib( void )
{
	TKDVFONTLIB *ptKdvFontLib = NULL;

	for ( u8 byLp = 0; byLp < m_byFontLibNum; byLp++ )
	{
		ptKdvFontLib = &m_tKdvFontLib[ byLp ];
		if ( ptKdvFontLib->pbyENData != NULL )
		{
			delete []ptKdvFontLib->pbyENData ;
			ptKdvFontLib->pbyENData = NULL;
		}

		if ( ptKdvFontLib->pbyCNData != NULL )
		{
			delete []ptKdvFontLib->pbyCNData;
			ptKdvFontLib->pbyCNData = NULL;
		}

		memset( ptKdvFontLib, 0, sizeof( TKDVFONTLIB ) );
	}

	m_byFontLibNum = 0;

	return;
}

/*=============================================================================
  �� �� ���� CreateBitMapRes
  ��    �ܣ� ����ͼ�����ݻ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CKdvGDI::CreateBitMapRes( void )
{
    // ���ͼƬ������
    m_pbyBmpDataBuff = new u8[MAX_BMPDATA_BUFFER];
    memset( m_pbyBmpDataBuff, 0, MAX_BMPDATA_BUFFER );
    
    // �������ͼƬ�ļ�
    m_pbyBmpFileBuff = new u8[MAX_BMPFILE_SIZE];
	memset(m_pbyBmpFileBuff, 0, sizeof(MAX_BMPFILE_SIZE));
}

/*=============================================================================
  �� �� ���� ReleaseBitMap
  ��    �ܣ� �ͷ�ͼ�����ݻ�����
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� void
  �� �� ֵ�� void 
=============================================================================*/
void CKdvGDI::ReleaseBitMapRes( void )
{
	if( NULL != m_pbyBmpDataBuff )
	{
		delete [] m_pbyBmpDataBuff;
		m_pbyBmpDataBuff = NULL;
	}

	if( NULL != m_pbyBmpFileBuff )
	{
		delete []m_pbyBmpFileBuff;
		m_pbyBmpFileBuff = NULL;
	}
	return ;
}
/*=============================================================================
�� �� ����CreatePaint
��    �ܣ���������(������ͼ����)
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
���������wOrignX wOrignY ����ԭ������
          dwWidth   �������
		  dwHeight  ��������

���������
�� �� ֵ�� �ɹ�����TRUE ʧ�ܷ���FALSE
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
BOOL32 CKdvGDI::CreatePaint( u16 wOrignX, u16 wOrignY, 
							   u32 dwWidth, u32 dwHeight,
							   u8 byBmpFileStyle )
{
	TKDVPAINT tKdvPaint = { 0 };
	tKdvPaint.wOrginX = wOrignX;
	tKdvPaint.wOrginY = wOrignY;
	tKdvPaint.dwWidth = dwWidth;
	tKdvPaint.dwHeight = dwHeight;

	return CreatePaint( tKdvPaint, byBmpFileStyle );
}

/*=============================================================================
�� �� ����CreatePaint
��    �ܣ���������
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
���������tKdvPaint ��������
���������
�� �� ֵ�� �ɹ�����TRUE ʧ�ܷ���FALSE
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
BOOL32 CKdvGDI::CreatePaint( TKDVPAINT& tKdvPaint, u8 byBmpFileStyle )
{
	memset( &m_tKdvPaint, 0, sizeof( m_tKdvPaint ) ); 
	memcpy( &m_tKdvPaint, &tKdvPaint, sizeof( m_tKdvPaint ) );

	return TRUE;
}

/*=============================================================================
�� �� ����DeletePaint
��    �ܣ�ɾ������
ע    �⣺
�㷨ʵ�֣�
ȫ�ֱ�����
���������tKdvPaint ��������
���������
�� �� ֵ�� �ɹ�����TRUE ʧ�ܷ���FALSE
-------------------------------------------------------------------------------
�޸ļ�¼��
��      ��  �汾  �޸���  �޸�����
=============================================================================*/
BOOL32 CKdvGDI::DeletePaint( void )
{
	if ( !m_bCreatePaint )
	{
		return FALSE;
	}
	memset( &m_tKdvPaint, 0, sizeof( m_tKdvPaint ) );
	m_bCreatePaint = FALSE;
	return TRUE;
}

// ------------------------------ �½ӿ� ------------------------------------ //

/*=============================================================================
    �� �� ��: IsSupportedPicType
    ��    ��: �ж��Ƿ�֧�ֵ�ǰ��ͼ����
    ע    ��: 
    �㷨ʵ��: 
    ȫ�ֱ���: 
    �������: u8 byPicType ͼƬ����
    �������:
    �� �� ֵ: �ɹ�����TRUE ʧ�ܷ���FALSE
-------------------------------------------------------------------------------
    �޸ļ�¼: 
    ��  ��      �汾        �޸���      �޸�����
    2012/06/13  4.7         liaokang      ����
=============================================================================*/
BOOL32 CKdvGDI::IsSupportedPicType( IN u8 byPicType )
{
    switch ( byPicType ) 
    {
    case BMP_8:
        paintlog( LOG_LVL_DETAIL, "[DrawBmp]: Cur pic type %d!\n", byPicType ); 
        break;
    case BMP_24:
        paintlog( LOG_LVL_DETAIL, "[DrawBmp]: Cur pic type %d!\n", byPicType ); 
        break;
    default:
        paintlog( LOG_LVL_ERROR, "[DrawBmp]:Do not support cur pic type %d!\n", byPicType );        
        return FALSE;
    }
    return TRUE;
}

/*=============================================================================
    �� �� ��: GetMinTextBoxAttrByText
    ��    ��: �����ı����ݻ�ȡ��С�ı�������
    ע    ��: 
    �㷨ʵ��: 
    ȫ�ֱ���: 
    �������: LPCSTR lpszText       �ı�����
    �������: u32& dwHeight         �߶�
              u32& dwWidth          ���
              u8& byLineNum         �ı�����
    �� �� ֵ: �ɹ�����TRUE ʧ�ܷ���FALSE
-------------------------------------------------------------------------------
    �޸ļ�¼: 
    ��  ��      �汾        �޸���      �޸�����
    2012/06/13  4.7         liaokang      ����
=============================================================================*/
void CKdvGDI::GetMinTextBoxAttrByText( OUT u32& dwHeight, OUT u32& dwWidth, OUT u8& byLineNum, IN LPCSTR lpszText )
{
    u8  byTextLen = strlen( lpszText );
    u8  byLibIndex = 0;
    u8  byCurChar = 0;		// ��ǰ��ȡ���ַ�
    u32 dwMaxWidth = 0;

    // ��ȡ��ǰ��������ֿ��е�����
    GetFontLibIndex( byLibIndex );
    // ��ȡ��ǰ��������ֿ��е�����
    TKDVFONTLIB *ptFontLib = &m_tKdvFontLib[byLibIndex];

    /* 
        ptFontLib->byWidth * 8 / 2  һ���ַ��Ŀ�ȣ�Ӣ�ģ�
        ptFontLib->byWidth * 8      �����ַ��Ŀ�ȣ����ģ�
        ptFontLib->byHeight * 8     һ���ַ��ĸ߶�
    */
    if ( byTextLen > 0 )
    {
        // dwHeightӦ��ʼΪ�ַ��߶�
        dwHeight = ptFontLib->byHeight * 8;
        byLineNum = 1;
    }
    else
    {
        paintlog( LOG_LVL_ERROR,   "[GetPaintAutoSizeByText]:Text size is zero!\n" ); 
        return;
    }
    
	while( byTextLen > 0 )
	{
        if ( NULL == lpszText )
        {
            byTextLen--;
            continue;
        }

        byCurChar = *( ( u8* ) lpszText++ );

        // �ո�
        if ( byCurChar == ' ' )
        {
            dwWidth += ptFontLib->byWidth * 4;  
            byTextLen--;
            continue;
        }
        // ���з�
        if ( byCurChar == '\n' )
        {
            dwHeight += ptFontLib->byHeight * 8;
            byLineNum ++;
            if ( dwMaxWidth < dwWidth )
            {
                dwMaxWidth = dwWidth;
            }            
            dwWidth = 0;
            byTextLen--;
            continue;
        }
        // ���಻����ʾ�ַ�
        if ( byCurChar < 32 )
        {
            byTextLen--;
            continue;
        }
        // Ӣ���ַ�
        if ( byCurChar < 161 )
        {
            dwWidth += ptFontLib->byWidth * 4;
            byTextLen--;
            continue;
        }
        //�����ַ�
        else
        {
            dwWidth += ptFontLib->byWidth * 8;
            lpszText++;
            byTextLen -= 2;
            continue;
        }
    }
    if ( dwMaxWidth > dwWidth )
    {
        dwWidth = dwMaxWidth;
    }
    
    return;
}

/*=============================================================================
    �� �� ��: CreatePaint
    ��    ��: ��������
    ע    ��: 
    �㷨ʵ��: 
    ȫ�ֱ���: 
    �������: TDrawParam& tDrawParam  ��ͼ����
    �������:
    �� �� ֵ: �ɹ�����TRUE ʧ�ܷ���FALSE
-------------------------------------------------------------------------------
    �޸ļ�¼: 
    ��  ��      �汾        �޸���      �޸�����
    2012/06/13  4.7         liaokang      ����
=============================================================================*/
BOOL32 CKdvGDI::CreatePaint( IN TDrawParam& tDrawParam )
{
    // �жϻ����Ƿ��Ѵ���
    if ( m_bCreatePaint )
    {
        // �Ѵ�������ɾ������
        DeletePaint();
    }

    u8 byDrawMode = tDrawParam.GetDrawMode();
    if ( AUTO_SIZE_MODE == byDrawMode )
    {
        // �����ı����ݻ�ȡ����Ӧ������С
        u32 dwPaintHeight = 0;
        u32 dwPaintWidth = 0;
        u8  byLineNum = 0;
        GetMinTextBoxAttrByText( dwPaintHeight, dwPaintWidth, byLineNum, tDrawParam.GetText() );
        m_tKdvPaint.wOrginX = 0;
        m_tKdvPaint.wOrginY = 0;
        m_tKdvPaint.dwHeight = dwPaintHeight;
        m_tKdvPaint.dwWidth = dwPaintWidth;
    }
    else
    {
        // ���ݴ������û�����С
        m_tKdvPaint.wOrginX = 0;
        m_tKdvPaint.wOrginY = 0;
        m_tKdvPaint.dwHeight = tDrawParam.GetPicHeight();
        m_tKdvPaint.dwWidth = tDrawParam.GetPicWidth();
    }

    // �жϻ����Ƿ����Ҫ��
    if ( m_tKdvPaint.dwHeight <= 0 || m_tKdvPaint.dwWidth <= 0 )
    {
        paintlog( LOG_LVL_ERROR, "[CreatePaint]: Paint's height or width is zero!\n");
        return FALSE;
    }

    // ��֤�������Ϊ4��������
    m_tKdvPaint.dwWidth = ( m_tKdvPaint.dwWidth % 4 == 0 )
                          ? m_tKdvPaint.dwWidth
                          : ( 4 * ( m_tKdvPaint.dwWidth / 4 + 1 ) );
    m_bCreatePaint = TRUE;
    return TRUE;
}

/*=============================================================================
    �� �� ��: IsVaildPicSize
    ��    ��: �ж�ͼƬ��С�Ƿ񳬹�����Χ
    ע    ��: 
    �㷨ʵ��: 
    ȫ�ֱ���: 
    �������: u8 byPicType          ��ǰͼƬ����
    �������:
    �� �� ֵ: �ɹ�����TRUE ʧ�ܷ���FALSE
-------------------------------------------------------------------------------
    �޸ļ�¼: 
    ��  ��      �汾        �޸���      �޸�����
    2012/06/13  4.7         liaokang      ����
=============================================================================*/
BOOL32 CKdvGDI::IsVaildPicSize()
{
    u32 dwPaintSize = m_tKdvPaint.dwWidth * m_tKdvPaint.dwHeight;
    u32 dwPicDataSize = 0;
    u32 dwPicSize = 0;
    switch ( m_byPicType ) 
    {
    case BMP_8:
        dwPicDataSize = dwPaintSize;
        dwPicSize = ( u32 )( sizeof( TBITMAPFILEHEADER )
            + sizeof( TBITMAPINFOHEADER )
            + PAINTCOLOR * sizeof( TPALETTEENTRY )
            + dwPicDataSize );
        break;
    case BMP_24:
        dwPicDataSize = dwPaintSize * 3;
        dwPicSize = ( u32 )( sizeof( TBITMAPFILEHEADER )
            + sizeof( TBITMAPINFOHEADER )
            + dwPicDataSize );
        break;
    }
    // �ж��������Ƿ�ƫ��
    if ( dwPicDataSize > MAX_BMPDATA_BUFFER )
    {
        paintlog( LOG_LVL_ERROR, "[DrawBmp]: Pic data size is too big!\n");
        return FALSE;
	} 
    // �ж�ͼƬ�Ƿ�ƫ��
    if ( dwPicSize > MAX_BMPFILE_SIZE )
    {
        paintlog( LOG_LVL_ERROR, "[DrawBmp]: Pic file size is too big!\n");
        return FALSE;
	}
    return TRUE;
}

/*=============================================================================
    �� �� ��: PaintBkColor
    ��    ��: ����������ɫ������ɫ��
    ע    ��: 
    �㷨ʵ��: 
    ȫ�ֱ���: 
    �������: u32 dwBkColor   ����ɫ
    �������: 
    �� �� ֵ: �ɹ�����TRUE ʧ�ܷ���FALSE
-------------------------------------------------------------------------------
    �޸ļ�¼: 
    ��  ��      �汾        �޸���      �޸�����
    2012/06/13  4.7         liaokang      ����
=============================================================================*/
BOOL32 CKdvGDI::PaintBkColor(IN u32 dwBkColor )
{
    //////////////////////////////////////////////////////////////////////////
    // ���ݲ�ͬ����ͼƬ��ɫ �����޸�

    u32 dwPaintSize = m_tKdvPaint.dwWidth * m_tKdvPaint.dwHeight;
    u8 byClrIndex = GetColorIndex( dwBkColor );
	memset( m_pbyBmpDataBuff, byClrIndex, dwPaintSize );  // ���ñ���ɫ
    return TRUE;
}

/*=============================================================================
    �� �� ��: TextOutInPaint
    ��    ��: ����ı�������
    ע    ��: 
    �㷨ʵ��: 
    ȫ�ֱ���: 
    �������: LPCSTR lpszText    �ı�
              u8 byTextLen       �ı�����
              u8 byTextAlignment �ı����з�ʽ
    �������: 
    �� �� ֵ: �ɹ�����TRUE ʧ�ܷ���FALSE
-------------------------------------------------------------------------------
    �޸ļ�¼: 
    ��  ��      �汾        �޸���      �޸�����
    2012/06/13  4.7         liaokang      ����
=============================================================================*/
BOOL32 CKdvGDI::TextOutInPaint( IN LPCSTR lpszText,IN u8 byTextLen, IN u8 byTextAlignment )
{
    s8  achText[DEFAULT_TEXT_LEN+1];        // �ı�����
    memset(achText, 0, sizeof(achText));
    s8  achLineText[DEFAULT_TEXT_LEN+1];    // ÿ���ı�����
    memset(achLineText, 0, sizeof(achLineText));
    s8 *pchText = achText;                  // �ı�����ָ��
    u8  LastCharTotalNum = 0;               // ��һ���Ѽ�¼���ַ�����
    u8  CurCharTotalNum = 0;                // ��ǰ�Ѽ�¼���ַ�����
    u8  byPrintTextLen = 0;                 // ��ǰ�ɴ�ӡ���ַ�����
    u8  byLibIndex = 0;                     // �ֿ�����
    u8  byCurChar = 0;		                // ��ǰ��ȡ���ַ�
    s32 nStartPosX = 0;                     // �ı��������������ʼλ��X
    s32 nStartPosY = 0;                     // �ı��������������ʼλ��Y

    // ��¼�ı�����
    memcpy( achText, lpszText, byTextLen ); 

    // ���ı������ж�
    if (  0 == byTextLen )
    {
        paintlog( LOG_LVL_ERROR,   "[TextOutInPaint]: Text size is zero!\n" ); 
        return FALSE;
    } 

    // �����ı����ݻ�ȡ��С���ı����С
    u32 dwMinTextBoxHeight = 0;
    u32 dwMinTextBoxWidth = 0;
    u8  byLineNum = 1;
    GetMinTextBoxAttrByText( dwMinTextBoxHeight, dwMinTextBoxWidth, byLineNum, achText );
    if( dwMinTextBoxHeight > m_tKdvPaint.dwHeight || dwMinTextBoxWidth > m_tKdvPaint.dwWidth )
    {
        paintlog( LOG_LVL_ERROR, "[TextOutInPaint]: The paint can't accommodate %d characters !\n", byTextLen ); 
        return FALSE;
    }
    
    // ��ȡ��ǰ��������ֿ��е�����
    GetFontLibIndex( byLibIndex );
    // ��ȡ��ǰ��������ֿ��е�����
    TKDVFONTLIB *ptFontLib = &m_tKdvFontLib[byLibIndex];

    // ���ݴ�ֱ�����ı����з�ʽ�ó���ֱ������ʼλ��
    switch ( byTextAlignment & 0x0F )
    {
        case VERTICAL_TOP:
            //��ֱ������
            nStartPosY = 0;
            break;
        case VERTICAL_CENTER:
            // ��ֱ�������
            nStartPosY = ( m_tKdvPaint.dwHeight - ptFontLib->byFontSize * byLineNum ) / 2;
            break;
        case VERTICAL_BOTTOM:
            // ��ֱ������
            nStartPosY = m_tKdvPaint.dwHeight - ptFontLib->byFontSize * byLineNum;
            break;
        default:
            paintlog( LOG_LVL_ERROR, "[TextOutInPaint]: Text vertical alignment param is wrong!\n"); 
            return FALSE; 
    } 

    while( byTextLen > 0 )
    {
        if ( NULL == pchText )
        {
            paintlog( LOG_LVL_ERROR, "[TextOutInPaint]: Text vertical alignment param is wrong!\n"); 
            return FALSE; 
        }
        
        byCurChar = *( ( u8* ) pchText++ );
        
        // �ո�
        if ( byCurChar == ' ' )
        {
            byTextLen--;
            CurCharTotalNum++;
            byPrintTextLen++;
            continue;
        }
        // ���з�
        if ( byCurChar == '\n' )
        { 
            // ����ˮƽ�����ı����з�ʽ�ó�ˮƽ������ʼλ��
            switch ( byTextAlignment & 0xF0 )
            {
                case LEVEL_LEFT:
                    // ˮƽ���������
                    nStartPosX = 0;
                    break;
                case LEVEL_CENTER:
                    // ˮƽ�������
                    nStartPosX = ( m_tKdvPaint.dwWidth - byPrintTextLen * ptFontLib->byFontSize / 2 ) / 2 ; // ��һ��2��ʾӢ���ַ������Լ���
                    break;
                case LEVEL_RIGHT:
                    // ˮƽ�����Ҷ���
                    nStartPosX = m_tKdvPaint.dwWidth - ( byPrintTextLen * ptFontLib->byFontSize / 2);
                    break;
                default:
                    paintlog( LOG_LVL_ERROR, "[TextOutInPaint]: Text level alignment param is wrong!\n"); 
                    return FALSE; 
            }
            byTextLen--;
            CurCharTotalNum++;
            memcpy(achLineText, achText + LastCharTotalNum, CurCharTotalNum - LastCharTotalNum );
            TextOut( nStartPosX, nStartPosY, achLineText, CurCharTotalNum - LastCharTotalNum );
            memset(achLineText, 0, sizeof(achLineText));
            nStartPosY += ptFontLib->byFontSize;
            LastCharTotalNum = CurCharTotalNum;
            byPrintTextLen = 0;
            continue;
        }
        // ���಻����ʾ�ַ�
        if ( byCurChar < 32 )
        {
            byTextLen--;
            CurCharTotalNum++;
            continue;
        }
        // Ӣ���ַ�
        if ( byCurChar < 161 )
        {
            byTextLen--;
            CurCharTotalNum++;
            byPrintTextLen++;
            continue;
        }
        //�����ַ�
        else
        {
            pchText++;
            byTextLen -= 2;
            CurCharTotalNum += 2;
            byPrintTextLen += 2;
            continue;
        }
    }

    if ( 0 != byPrintTextLen )
    {
        // ����ˮƽ�����ı����з�ʽ�ó�ˮƽ������ʼλ��
        switch ( byTextAlignment & 0xF0 )
        {
            case LEVEL_LEFT:
                // ˮƽ���������
                nStartPosX = 0;
                break;
            case LEVEL_CENTER:
                // ˮƽ�������
                nStartPosX = ( m_tKdvPaint.dwWidth - byPrintTextLen * ptFontLib->byFontSize / 2 ) / 2 ; // ��һ��2��ʾӢ���ַ������Լ���
                break;
            case LEVEL_RIGHT:
                // ˮƽ�����Ҷ���
                nStartPosX = m_tKdvPaint.dwWidth - ( byPrintTextLen * ptFontLib->byFontSize / 2);
                break;
            default:
                paintlog( LOG_LVL_ERROR, "[TextOutInPaint]: Text level alignment param is wrong!\n"); 
                return FALSE; 
        }
        memcpy(achLineText, achText + LastCharTotalNum, CurCharTotalNum - LastCharTotalNum );
        TextOut( nStartPosX, nStartPosY, achLineText, CurCharTotalNum - LastCharTotalNum );
    }

    return TRUE;
}


/*=============================================================================
    �� �� ��: GetPic
    ��    ��: ��ȡ���ɵ�PIC����
    ע    ��: 
    �㷨ʵ��: 
    ȫ�ֱ���: 
    �������: u8 byPicType  Pic����
    �������: TPic& tPic    Pic
    �� �� ֵ: �ɹ�����TRUE ʧ�ܷ���FALSE
-------------------------------------------------------------------------------
    �޸ļ�¼: 
    ��  ��      �汾        �޸���      �޸�����
    2012/06/13  4.7         liaokang      ����
=============================================================================*/
BOOL32 CKdvGDI::GetPic( OUT TPic& tPic )
{
	if ( NULL == &tPic )
	{
        paintlog( LOG_LVL_ERROR, "[GetPic]: Invalid input param!\n");
        return FALSE;
	}

    if ( NULL == m_pbyBmpDataBuff )
    {
        paintlog( LOG_LVL_ERROR, "[GetPic]: Bmpdata point is null!\n");
        return FALSE;
    }
    
    if ( NULL == m_pbyBmpFileBuff )
    {
        // �������ͼƬ�ļ�
        m_pbyBmpFileBuff = new u8[MAX_BMPFILE_SIZE];
        memset(m_pbyBmpFileBuff, 0, sizeof(MAX_BMPFILE_SIZE));
    }

    u32 dwPaintSize = m_tKdvPaint.dwWidth * m_tKdvPaint.dwHeight;
    u32 dwPicSize = 0;
    u32 dwOffBits = 0;
    switch ( m_byPicType ) 
    {
    case BMP_8:
        {
            dwPicSize = ( u32 )( sizeof( TBITMAPFILEHEADER )
                + sizeof( TBITMAPINFOHEADER )
                + PAINTCOLOR * sizeof( TPALETTEENTRY )
                + dwPaintSize );
            dwOffBits = ( u32 ) ( sizeof( TBITMAPFILEHEADER )
                + sizeof( TBITMAPINFOHEADER )
                + PAINTCOLOR * sizeof( TPALETTEENTRY ) );
            
            // ��ɫ��( BMPͼƬ��,RGB��Ŵ���:BGR )
            TPALETTEENTRY tPalEntry[ PAINTCOLOR ] = { 0 };
            for ( u16 wLp = 0; wLp < PAINTCOLOR; wLp++ )
            {
                tPalEntry[ wLp ].byRed = m_tPaletteEntry[ wLp ].byBlue;
                tPalEntry[ wLp ].byGreen = m_tPaletteEntry[ wLp ].byGreen;
                tPalEntry[ wLp ].byBlue = m_tPaletteEntry[ wLp ].byRed;
                tPalEntry[ wLp ].byFlags = m_tPaletteEntry[ wLp ].byFlags;
            }
            memcpy( m_pbyBmpFileBuff + sizeof( TBITMAPFILEHEADER ) + sizeof( TBITMAPINFOHEADER ),
                tPalEntry, sizeof( tPalEntry ) );
        }
        break;
    case BMP_24:
        {
            dwPicSize = ( u32 )( sizeof( TBITMAPFILEHEADER )
                + sizeof( TBITMAPINFOHEADER )
                + dwPaintSize * 3);
            dwOffBits = ( u32 ) ( sizeof( TBITMAPFILEHEADER )
            + sizeof( TBITMAPINFOHEADER ) );
        }
        break;
    }

	// �ļ�ͷ
	TBITMAPFILEHEADER tBitMapFileHeader;
    memset( &tBitMapFileHeader, 0, sizeof( tBitMapFileHeader ) );
	tBitMapFileHeader.wType = (u16)0x4D42;  // �ļ�ͷ������ֵ�̶�Ϊ"BM", ע��ߵ��ֽ�
	tBitMapFileHeader.dwSize = dwPicSize;
    tBitMapFileHeader.wReserved1 = 0;
    tBitMapFileHeader.wReserved2 = 0;
	tBitMapFileHeader.dwOffBits = dwOffBits;
	memcpy( m_pbyBmpFileBuff, &tBitMapFileHeader, sizeof( tBitMapFileHeader ) );

	// ��Ϣͷ
	TBITMAPINFOHEADER tBitMapInfoHeader;
    memset( &tBitMapInfoHeader, 0, sizeof( tBitMapInfoHeader ) );
    tBitMapInfoHeader.dwSize = sizeof( TBITMAPINFOHEADER );
    tBitMapInfoHeader.dwWidth = m_tKdvPaint.dwWidth;
    tBitMapInfoHeader.dwHeight = m_tKdvPaint.dwHeight;
    tBitMapInfoHeader.wPlanes = 1;
    tBitMapInfoHeader.wBitCount = (m_byPicType == BMP_8)?8:24;
    tBitMapInfoHeader.dwCompression = 0L;
    tBitMapInfoHeader.dwSizeImage = (m_byPicType == BMP_8)?dwPaintSize:3*dwPaintSize;
    tBitMapInfoHeader.dwXPelsPerMeter = 0;
    tBitMapInfoHeader.dwYPelsPerMeter = 0;
    tBitMapInfoHeader.dwClrUsed = 0;
    tBitMapInfoHeader.dwClrImportant = 0;
	memcpy( m_pbyBmpFileBuff + sizeof( TBITMAPFILEHEADER ), &tBitMapInfoHeader, sizeof( tBitMapInfoHeader ) );
	
	// ������
	memcpy( m_pbyBmpFileBuff + tBitMapFileHeader.dwOffBits, m_pbyBmpDataBuff, tBitMapInfoHeader.dwSizeImage );

	// ��������
    tPic.SetPicNull();
    tPic.SetPicType( m_byPicType );
    tPic.SetPicHeight( tBitMapInfoHeader.dwHeight );
    tPic.SetPicWidth( tBitMapInfoHeader.dwWidth );
    tPic.SetPicSize( tBitMapFileHeader.dwSize );
    tPic.SetPic( (s8 *)m_pbyBmpFileBuff );

	return TRUE;
}

/*=============================================================================
    �� �� ��: DrawBmp
    ��    ��: ��Bmpͼ
    �㷨ʵ��:  
    ȫ�ֱ���:  
    ��    ��: OUT TPic& tPic ͼƬ
              IN TDrawParam& tDrawParam ��ͼ����
    �� �� ֵ: �ɹ�����SUCCESS_DRAW ʧ�ܷ���FAIL_DRAW
-------------------------------------------------------------------------------
    �޸ļ�¼: 
    ��  ��      �汾        �޸���      �޸�����
    2012/06/13  4.7         liaokang      ����
=============================================================================*/
BOOL32 CKdvGDI::DrawBmp( OUT TPic& tPic, IN TDrawParam& tDrawParam )
{
    // �ÿ�
    // ��ֹǰ����ýӿڳɹ���������ýӿڲ��ɹ���û���ж�ʱ��������ͬһͼƬ
    tPic.SetPicNull();

    // ��ֹû�г�ʼ��
    if ( FALSE == m_bInitGDI )
    {
        paintlog( LOG_LVL_ERROR, "[DrawBmp]:Initialization is not completed, please initialize!\n");
        return FAIL_DRAW;
	}
    
    BOOL32 bRet = FALSE;
    // ��¼��ǰͼƬ����
    m_byPicType = tDrawParam.GetPicType();
    // �ж��Ƿ�֧�ֵ�ǰ��ͼ����
    bRet = IsSupportedPicType( m_byPicType );
    if ( !bRet )
    {
        paintlog( LOG_LVL_ERROR,   "[DrawBmp]:Do not support cur pic type %d!\n", m_byPicType );        
        return FAIL_DRAW;
    } 

    // ��¼��ǰ����
    TKDVFONT tFont;
    tFont.byType = tDrawParam.GetFontType();
    tFont.bySize = tDrawParam.GetFontSize();
	SetCurrentFont( tFont );

    // ��������
    bRet = CreatePaint( tDrawParam );
    if ( !bRet )
    {
        paintlog( LOG_LVL_ERROR, "[DrawBmp]: Fail to creat paint!\n" );        
        return FAIL_DRAW;
    }
    bRet = IsVaildPicSize();
    if ( !bRet )
    {
        paintlog( LOG_LVL_ERROR, "[DrawBmp]: Pic size is invalid!\n" );        
        return FAIL_DRAW;
    }

    // ����������ɫ������ɫ��
    PaintBkColor( tDrawParam.GetDrawBkColor() );
    // �ı���ɫ
	SetTextColor( tDrawParam.GetTextColor() );

    // ����ַ���
    s8 achText[DEFAULT_TEXT_LEN+1];
    memset(achText, 0, sizeof(achText));
 	memcpy(achText, tDrawParam.GetText(), sizeof(achText));
    bRet = TextOutInPaint( achText, strlen(achText), tDrawParam.GetTextAlignment() );
    if ( !bRet )
    {
        paintlog( LOG_LVL_ERROR, "[DrawBmp]:Fail to text out in paint!\n" );        
        return FAIL_DRAW;
    }

	// �õ�������
 	bRet = GetPic( tPic );
    if ( !bRet )
    {
        paintlog( LOG_LVL_ERROR, "[DrawBmp]:Fail to get pic!\n" );        
        return FAIL_DRAW;
    }
	
	// ɾ������
	bRet = DeletePaint();
    if ( !bRet )
    {
        paintlog( LOG_LVL_ERROR, "[DrawBmp]:Fail to delete paint!\n" );        
        return FAIL_DRAW;
    }

	return SUCCESS_DRAW;
}

/*=============================================================================
�� �� ���� paintlog
��    �ܣ� ��ӡ����
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� u8 byLogLvl
s8 * fmt
...
�� �� ֵ�� void 
=============================================================================*/
/*lint -save -e530 */
/*lint -save -esym(526, __builtin_va_start)*/
/*lint -save -esym(628, __builtin_va_start)*/
void paintlog( u8 byLevel, s8* pszFmt, ... )
{
    s8 achPrintBuf[1024]= {0};
    va_list argptr;
    u16 wBufLen = sprintf(achPrintBuf, "[drawbmp]");
    va_start( argptr, pszFmt );
    vsprintf(achPrintBuf + wBufLen, pszFmt, argptr );
    va_end(argptr);
    LogPrint( byLevel, MID_VMPEQP_DRAWBMP, achPrintBuf );
    return;
}
/*lint -restore*/