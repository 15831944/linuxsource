/*****************************************************************************
   ģ����      : drawbmp
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
#ifndef _DRAW_INTERFACE_H
#define _DRAW_INTERFACE_H

#include "kdvtype.h"
#include "osp.h"

#ifdef WIN32
    #pragma  pack (1)
#endif

#define IN     // ������������
#define OUT    // �����������

#ifndef min
#define min(a,b) ((a)>(b)?(b):(a))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

// Ĭ���ı�����
#define DEF_TEXT_LEN                (u8)128 
// ���BMP�ļ���С
#define MAX_BMP_FILESIZE            (u32)800*600  
// ���BMP���ݴ�С
#define MAX_BMP_DATASIZE            (u32)720*576  

#ifdef WIN32
#define DEF_FONT_PATHNAME	        (s8*)"../res/font/kdvuni.ttf"
#else
#define DEF_FONT_PATHNAME	        (s8*)"/usr/bin/font/kdvuni.ttf"
#endif

//���뷽ʽ
#define ENCODE_UTF8	                (u8)1
#define ENCODE_GBK	                (u8)2

//bmp �滭ģʽ
#define BMP_8                       (u8)1
#define BMP_24                      (u8)2 

// ������뷽ʽ
// ��ֱ����
#define VERTICAL_TOP                (u16)0x0001  // ��ֱ������
#define VERTICAL_CENTER             (u16)0x0002  // ��ֱ�������
#define VERTICAL_BOTTOM             (u16)0x0004  // ��ֱ������
// ˮƽ����
#define LEVEL_RIGHT                 (u16)0x0010  // ˮƽ�����Ҷ���
#define LEVEL_CENTER                (u16)0x0020  // ˮƽ�������
#define LEVEL_LEFT                  (u16)0x0040  // ˮƽ���������
// �������з�ʽ
#define ARRANGE_LEFT_RIGHT          (u16)0x0100  // ������������
#define ARRANGE_RIGHT_LEFT          (u16)0x0200  // ������������

// ����ͼƬ��С������ʽ
#define AUTO_SIZE_MODE              (u8)0x01        // ��������Ӧ��С��ͼƬ���������ı����ɺ��ʴ�С��ͼƬ
#define USER_DEFINED_SIZE_MODE      (u8)0x02        // �����û��Զ����С��ͼƬ

// ����
#define FONT_HEI                    (u8)01 // ����
#define FONT_SONG                   (u8)02 // ���� (Ĭ��)
#define FONT_KAI                    (u8)03 // ����

// �ֺ�
#define FONT_SIZE16                 (u8)16	        // 24
#define FONT_SIZE24                 (u8)24	        // 24
#define FONT_SIZE32                 (u8)32	        // 32
#define FONT_SIZE48                 (u8)48	        // 48
#define FONT_SIZE64                 (u8)64	        // 64

// RGB
#undef RGB
#define RGB(r,g,b) ((u32)(((u8)(r)|((u16)((u8)(g))<<8))|(((u32)(u8)(b))<<16)))
// ��ɫ
const u32 COLOR_BLACK = RGB( 0, 0, 0 );
const u32 COLOR_WHITE = RGB( 255, 255, 255 );
const u32 COLOR_GRAY = RGB( 196, 206, 212 );
const u32 COLOR_MENUHEADER = RGB( 54,40,111 );
const u32 COLOR_MENUBODY = RGB( 124,114,124 )/*RGB( 120,120,120 )*/;
const u32 COLOR_MENUFOOT = RGB(90,90,90 );
const u32 COLOR_ITEMFONT = RGB( 42,31,85 );
const u32 COLOR_ITEMHL = RGB( 85,95,170 );
const u32 COLOR_YELLOW = RGB( 252, 226, 84 );
const u32 COLOR_BLUE = RGB( 44, 94, 180 );
const u32 COLOR_GREEN = RGB( 132, 218, 132 );
const u32 COLOR_CURSOR = RGB( 100, 202, 68 );
const u32 COLOR_RED = RGB( 255, 0, 0 );

// ͼƬ����
struct TPicParam
{
public:
    /*   Ĭ�ϲ�����BMP_24  */  
    TPicParam() : m_byPicType( BMP_24 ),
                  m_dwPicHeight(0),
                  m_dwPicWidth(0),
                  m_dwPicSize(0)
    { 
    
    }
    void SetPicParamNull(void) { memset(this, 0, sizeof(TPicParam)); }

    u8  GetPicType( void ) { return m_byPicType; }
    u32 GetPicHeight( void ) { return m_dwPicHeight; }
    u32 GetPicWidth( void )  { return m_dwPicWidth;  }
    u32 GetPicSize( void )   { return m_dwPicSize;  }

    void SetPicType( u8 byPicType )     { m_byPicType = byPicType; }
    void SetPicHeight( u32 dwPicHeight ) { m_dwPicHeight = dwPicHeight; }
    void SetPicWidth( u32 dwPicWidth )   { m_dwPicWidth = dwPicWidth; }

protected:
    void SetPicSize( u32 dwPicSize )    { m_dwPicSize = dwPicSize; }
    
protected:
    u8  m_byPicType;        // ͼƬ����
    u32 m_dwPicHeight;      // ͼƬ�߶�
    u32 m_dwPicWidth;       // ͼƬ���
    u32 m_dwPicSize;        // ͼƬ��С
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// ��С�ı�ͼƬ����
struct TMinTextPicParam
{
public:
    TMinTextPicParam()
    { 
        SetMinTextPicParamNull();
    }
    void SetMinTextPicParamNull(void) { memset(this, 0, sizeof(TMinTextPicParam)); }

    u32 GetMinTextPicTop( void ) { return m_dwMinTextPicTop; }
    u32 GetMinTextPicLeft( void )  { return m_dwMinTextPicLeft;  }
    u32 GetMinTextPicHeight( void ) { return m_dwMinTextPicHeight; }
    u32 GetMinTextPicWidth( void )  { return m_dwMinTextPicWidth;  }
    
    void SetMinTextPicTop( u32 dwMinTextPicTop ) { m_dwMinTextPicTop = dwMinTextPicTop; }
    void SetMinTextPicLeft( u32 dwMinTextPicLeft )   { m_dwMinTextPicLeft = dwMinTextPicLeft; }
    void SetMinTextPicHeight( u32 dwMinTextPicHeight ) { m_dwMinTextPicHeight = dwMinTextPicHeight; }
    void SetMinTextPicWidth( u32 dwMinTextPicWidth )   { m_dwMinTextPicWidth = dwMinTextPicWidth; }
    
protected:
    u32 m_dwMinTextPicTop;          // ͼƬtopλ��
    u32 m_dwMinTextPicLeft;         // ͼƬleftλ��
    u32 m_dwMinTextPicHeight;      // ͼƬ�߶�
    u32 m_dwMinTextPicWidth;       // ͼƬ���
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// ͼƬ
struct TPic : public TPicParam,
              public TMinTextPicParam
{
public:
    TPic() 
    { 
        SetPicNull();
    }
    void SetPicNull(void) { memset(this, 0, sizeof(TPic)); }

    const u8* GetPic( void ) { return m_abyPic; }
    void SetPic( const u8* lpszPic, u32 dwPicSize )
    {
        memset( m_abyPic, 0, sizeof(m_abyPic) );
        SetPicSize( 0 );
        if( NULL == lpszPic || dwPicSize >= sizeof(m_abyPic) )
        {            
            return;
        }

        memcpy( m_abyPic, lpszPic, dwPicSize );
        SetPicSize( dwPicSize );
        m_abyPic[ sizeof( m_abyPic ) - 1 ] = '\0';
    }
protected:
    u8 m_abyPic[MAX_BMP_FILESIZE+1];    // ͼƬBUF
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// �ı�����
struct TTextParam
{
public: 
    TTextParam() : m_byFontType( FONT_KAI ), // Ŀǰ��֧������
                   m_byFontSize( FONT_SIZE32 ), // ����ʹ�ó����ֺ�
                   m_wTextDistribution( ARRANGE_LEFT_RIGHT | VERTICAL_CENTER | LEVEL_LEFT ),
                   m_dwTextColor( COLOR_WHITE ),
                   m_byTextEncode( ENCODE_UTF8 )   // Ĭ��ΪUTF8����
    { 
        memset( m_achText, 0, sizeof(m_achText) );
    }
    void SetTextParamNull(void) { memset(this, 0, sizeof(TTextParam)); }

    u8  GetFontType( void)       { return m_byFontType; }
    u8  GetFontSize( void )      { return m_byFontSize; }
    u16 GetTextDistribution( void ) { return m_wTextDistribution; }
    u32 GetTextColor( void )     { return m_dwTextColor; }
    const s8 * GetText( void )       { return m_achText; }
    
    void SetFontType( u8 byFontType )            { m_byFontType = byFontType; }
    void SetFontSize( u8 byFontSize )            { m_byFontSize = byFontSize; }
    void SetTextDistribution( u16 wTextDistribution ) { m_wTextDistribution = wTextDistribution; }
    void SetTextColor( u32 dwTextColor )         { m_dwTextColor = dwTextColor; }
    void SetText( const s8 * lpszText)
    {
        if( NULL == lpszText )
        {
            return;
        }
        memset( m_achText, 0, sizeof(m_achText) );
        memcpy( m_achText, lpszText, min(sizeof(m_achText), strlen(lpszText)) );
        m_achText[ sizeof( m_achText ) - 1 ] = '\0';
	}

    void SetTextEncode(u8 byTextEncode) { m_byTextEncode = byTextEncode; }
    u8   GetTextEncode() { return m_byTextEncode; }

protected:
    u8  m_byFontType;       // ��������
    u8  m_byFontSize;       // �����С
    u16 m_wTextDistribution; // �ı��ֲ���ʽ�����÷�ʽ: ARRANGE_LEFT_RIGHT | VerticalAlignment | LevelAlignment��������/���ҵ������� λ�� ��ֱ���뷽ʽ λ�� ˮƽ���뷽ʽ��
    u32 m_dwTextColor;      // �ı���ɫ
	s8  m_achText[DEF_TEXT_LEN+1]; // �ı�����
    u8 m_byTextEncode;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// ��ͼ����
/*
ͨ������m_byDrawMode�ж���ͼģʽ��

����AUTO_SIZE_MODEģʽ�������ñ���������
��TextParam ��m_byFontType,m_byFontSize,m_wTextDistribution,m_dwTextColor,m_achText,m_byTextEncode
��TPicParam ��m_byPicType
��TDrawParam��m_dwDrawBkColor

����USER_DEFINED_SIZE_MODEģʽ�������ñ���������
��TextParam ��m_byFontType,m_byFontSize,m_wTextDistribution,m_dwTextColor,m_achText,m_byTextEncode
��TPicParam ��m_byPicType,m_wPicHeight�����𲿷֣�,m_wPicWidth�����𲿷֣�
��TDrawParam��m_dwDrawBkColor
*/
struct TDrawParam : public TPicParam, public TTextParam
{
public:
    /*   Ĭ�ϲ�����AUTO_SIZE_MODEģʽ����ɫ����  */ 
    TDrawParam() : m_byDrawMode( AUTO_SIZE_MODE ),
                   m_dwDrawBkColor( COLOR_BLACK )
    {

    }
    void SetDrawParamNull(void) { memset(this, 0, sizeof(TDrawParam)); }

    u8  GetDrawMode( void ) { return m_byDrawMode; }
    u32 GetDrawBkColor( void ) { return m_dwDrawBkColor; }    
    
    void SetDrawMode( u8 byDrawMode )     { m_byDrawMode = byDrawMode; }
    void SetDrawBkColor( u32 dwDrawBkColor ) { m_dwDrawBkColor = dwDrawBkColor; }

protected:
    u8  m_byDrawMode;           // ��ͼģʽ: AUTO_SIZE_MODE ���� USER_DEFINED_SIZE_MODE
    u32 m_dwDrawBkColor;        // ��ͼ����ɫ
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

class CKdvGDI;

class CDrawBmp
{	
public :
	CDrawBmp();
	virtual ~CDrawBmp();

    //Ŀǰ������ĳ���ֿ�  ֻ֧��һ������һ���ֿ�
    BOOL32 LoadFont(const s8 * pFontPathName = DEF_FONT_PATHNAME);
    // ��ͼ  success: 1, fail: 0
	BOOL32 DrawBmp( OUT TPic& tPic, IN TDrawParam& tDrawParam );

private:
	CKdvGDI *m_pDrawGdi;
};

API void drawbmphelp();
API void drawbmpver();
API void setdrawbmplog(u8 byLogLvl);

#endif // _DRAW_INTERFACE_H

