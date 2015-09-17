#include "MainHeader.h"
//
cText*		cText::TextInst	=	NULL;

cText::cText(void)
{
	Init();
}
cText::~cText(void)
{
}

void	cText::Init(void)
{
	D3DXFONT_DESC DXFont_DESC;
	ZeroMemory(&DXFont_DESC, sizeof(D3DXFONT_DESC));

	DXFont_DESC.Height = 15;							// 전체 글자 높이
	DXFont_DESC.Width = 15;								// 전체 글자 넓이
	DXFont_DESC.Weight = FW_NORMAL;						// 긁자 굵기 ( FW_BOLD 하면 굵음 )
	DXFont_DESC.MipLevels = D3DX_DEFAULT;
	DXFont_DESC.Italic = false;							// 이텔릭
	DXFont_DESC.CharSet = DEFAULT_CHARSET;
	DXFont_DESC.OutputPrecision = OUT_DEFAULT_PRECIS;
	DXFont_DESC.Quality = DEFAULT_QUALITY;
	DXFont_DESC.PitchAndFamily = DEFAULT_PITCH;
	DXFont_DESC.FaceName, TEXT("돋움체");				// 글씨체
	m_nMax_X	=	1280;								// 윈도우 최대 X
	m_nMax_Y	=	960;								// 윈도우 최대 Y
	D3DXCreateFontIndirect(D3DFRAME->GetDevice(),&DXFont_DESC, &m_pFont);
}

void	cText::Print( LPCSTR cSTR, int nX/*문자열의 왼쪽좌표*/, int nY/*문자열의 윗쪽좌표*/, D3DXCOLOR ARGB )
{
	RECT rt = { nX, nY, m_nMax_X, m_nMax_Y };

#ifdef UNICODE // 유니코드 부분
	m_pFont->DrawTextW(0, cSTR, -1, &rt, DT_TOP|DT_LEFT, ARGB);

#else
	m_pFont->DrawTextA(0, cSTR, -1, &rt, DT_TOP|DT_LEFT, ARGB);

#endif
}

cText*	cText::GetInst(void)
{
	if ( !TextInst )
	{
		TextInst	=	new cText;
		RSCMGR->m_bTEXT_MGR	=	true;
	}
	return TextInst;
}

void	cText::FreeInst(void)
{
	if ( TextInst )
	{
		m_pFont->Release();
		delete	TextInst;
		TextInst	=	NULL;
		RSCMGR->m_bTEXT_MGR	=	false;
	}
}