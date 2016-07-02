/*****************************************************************************
   ģ����      : KDV draw bmp
   �ļ���      : drawbmp.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ���׻�ͼ����ͷ�ļ�
   ����        : 
   �汾        : V0.9  Copyright(C) 2001-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2006/03/29  4.0         john     ����
******************************************************************************/
#ifndef _KDVDRAW_DECLARE_H_
#define _KDVDRAW_DECLARE_H_ 

#include "drawbmpconst.h"
#include "drawinterface.h"
#include "kdvsys.h"
#include "kdvlog.h"
#include "loguserdef.h"

class CKdvGDI
{	
public :
	CKdvGDI();
	virtual ~CKdvGDI();

public:
	
	BOOL32 InitGDI( void );// ��ʼ����ͼϵͳ
	BOOL32 QuitGDI( void );// �˳���ͼϵͳ

// �½ӿ� ��Ҫ����

    // �ж��Ƿ�֧�ֵ�ǰ��ͼ����
    BOOL32 IsSupportedPicType( IN u8 byPicType );
    // �����ı����ݻ�ȡ��С���ı�������
    void   GetMinTextBoxAttrByText(OUT u32& dwHeight, OUT u32& dwWidth, OUT u8& byLineNum, IN LPCSTR lpszText );
    // ��������
    BOOL32 CreatePaint( IN TDrawParam& tDrawParam );
    // �ж�ͼƬ��С�Ƿ񳬹�����Χ
    BOOL32 IsVaildPicSize();
    // ����������ɫ������ɫ��
    BOOL32 PaintBkColor( IN u32 dwBkColor );
    // ����ı�������
    BOOL32 TextOutInPaint( IN LPCSTR lpszText,IN u8 byTextLen, IN u8 byTextAlignment );
    // ��ȡ���ɵ�PIC����
    BOOL32 GetPic( OUT TPic& tPic );
    // ��ͼ
	BOOL32 DrawBmp( OUT TPic& tPic, IN TDrawParam& tDrawParam );

// ���Բ���
	const TKDVFONT* GetCurrentFont( void ) const { return &m_tKdvFont; }// ��ȡ��ǰ��������

	u32 SetBkColor( u32 dwBkColor );// ���õ�ǰ��������ɫ	
	u32 GetBkColor( void ) const { return m_dwBkColor; }// ��ȡ��ǰ��������ɫ
	
	u32 SetTextColor( u32 dwTextColor );// ���õ�ǰ�ı���ɫ
	u32 GetTextColor( void ) const { return m_dwTextColor; }// ��ȡ��ǰ�ı���ɫ

	TKDVFONT SetCurrentFont( TKDVFONT &tKdvFont );// ���õ�ǰ��������
    
	TKDVBRUSH SetCurrentBrush( TKDVBRUSH &tKdvBrush );// ���õ�ǰ��ˢ����		
	const TKDVBRUSH* GetCurrentBrush( void ) const { return &m_tKdvBrush; }// ��ȡ��ǰ��ˢ����

	TKDVPEN SetCurrentPen( TKDVPEN &tKdvPen );// ���õ�ǰ��������
	const TKDVPEN* GetCurrentPen( void )  const { return &m_tKdvPen; }// ��ȡ��ǰ��������
    
	TKDVPAINT SetCurrentPaint(TKDVPAINT tPaint); // ���õ�ǰ����
	const TKDVPAINT* GetPaintInfo( void ) { return &m_tKdvPaint; }// ��ȡ��������

	BOOL32 IsCreate( void ) { return m_bCreatePaint; }	  // �����Ƿ񴴽�

// ����ͼ�λ���
	u32 SetPixel( s32 nPosX, s32 nPosY, u32 dwPixelColor );// ����
	u32 SetPixel( TPOINT tPoint, u32 dwPixelColor );
	
	BOOL32 LineTo( s32 nPosX, s32 nPosY );// ����
	BOOL32 LineTo( TPOINT tPoint );
    BOOL32 LevelLine( IN s32 nPosX, IN s32 nPosY, IN u32 dwWidth );// ��ˮƽֱ��	
	BOOL32 VertLine( IN s32 nPosX, IN s32 nPosY, IN u32 dwHeight );// ����ֱֱ��
		
	TPOINT MoveTo( s32 nPosX, s32 nPosY );// �ı䵱ǰλ��
	TPOINT MoveTo( TPOINT tPoint );

	BOOL32 DrawRect( s32 nPosX, s32 nPosY, u32 dwWidth, u32 dwHeight );// ������
	BOOL32 DrawRect( TRECT tRect );

	BOOL32 FillRect( s32 nPosX, s32 nPosY, u32 dwWidth, u32 dwHeight ); // ���ָ������
	BOOL32 FillRect( TRECT tRect );
	
    // ����
	BOOL32 CreatePaint( u16 wOrignX, u16 wOrignY, u32 dwWidth, u32 dwHeight, u8 byBmpFileStyle = BMP_8 );
	BOOL32 CreatePaint( TKDVPAINT& tKdvPaint, u8 byBmpFileStyle = BMP_8 );
    BOOL32 DeletePaint( void );	

	u32    TextOutInRect( const char* pchText, u32 dwTextLen, TRECT tRect );// �ı����
	u32    TextOut( IN s32 nPosX, IN s32 nPosY, const char* pchText, u32 dwTextLen ); // �ı���� 
		
	void   PalSortByShell( void );// �Ե�ɫ������
	
	u8     GetColorIndex( IN u32 clrColor );// ����RGB��ȡ��ɫ������

	BOOL32 GetFontPath( OUT s8 *pachFontPath, IN u16 wLen  );// ȡ�ֿ�·��
    BOOL32 LoadFontLib( const char* pchFontPath );// �����ֿ���Դ
	BOOL32 GetFontLibIndex( u8 &byLibIndex ); //  �����ֿ��ļ��ɶ���ֿ���ɣ�ȡ��Ӧ���ֿ����
	void   ReleaseFontLib( void );// �ͷ��ֿ���Դ
    
    // ͼ����Դ
    void   CreateBitMapRes( void );
	void   ReleaseBitMapRes( void );

private :

	BOOL32 m_bInitGDI;	// ����ͼ��ϵͳ��ʼ����־

	u8	m_byFontLibNum; // �ֿ���Ŀ
	TKDVFONTLIB m_tKdvFontLib[ MAX_FONTLIB_NUM ];// �ֿ���Դ

	TPALETTEENTRY m_tPaletteEntry[ PAINTCOLOR ];// ��ɫ��	
	u8  m_abyPaletteIndex[ PAINTCOLOR ];// ˳���ɫ��������

	u32 m_dwBkColor;        // ��������ɫ
	u32 m_dwTextColor;      // ��ǰ�ı���ɫ
	
	TKDVFONT   m_tKdvFont;	// ��ǰ��������
	TPOINT     m_tKdvPoint;	// ��ǰ�������
	TKDVPEN    m_tKdvPen;	// ��ǰ��������
	TKDVBRUSH  m_tKdvBrush;	// ��ǰ��ˢ����
	TKDVPAINT  m_tKdvPaint;	// ��ǰ��������

	// ����������־
	BOOL32 m_bCreatePaint;

	// ͼ�ηֽ�Ϊ�ļ�ͷ��������
	// ͼ���ļ�������(ֻ��ͼ������)
	u8 *m_pbyBmpDataBuff;
	// ����ͼ���ļ�������(��ͼ���ļ�ͷ��������)
    u8 *m_pbyBmpFileBuff;

// ��������
    // ͼƬ����
    u8 m_byPicType;
};

void paintlog(u8 byLogLvl, s8 * fmt, ...);

#endif  // _KDVDRAW_DECLARE_H_