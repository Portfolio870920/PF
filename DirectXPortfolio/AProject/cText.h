#pragma once

class cText
{
private:
	static	cText*		TextInst;
			ID3DXFont*	m_pFont;
			int			m_nMax_X;
			int			m_nMax_Y;
	cText(void);

public:	
	~cText(void);
	void	Init(void);
	void    Print( LPCSTR cSTR, int nX/*문자열의 왼쪽좌표*/ = 0, int nY/*문자열의 윗쪽좌표*/ = 0, D3DXCOLOR ARGB = 0xFFFFFFFF );
	static	cText*		GetInst(void);
			void		FreeInst(void);
};

// 사용법
//TCHAR	str_Text[256];	// 문자열 변수
//ZeroMemory(str_Text,sizeof(str_Text)); //문자열 변수 초기화
////sprintf(변수명,이후로는 printf처럼 사용);
//sprintf(str_Text,"출력 : %d",A0); // 이런식으로 사용
//TEXTMGR->Print(str_Text); // 텍스트출력
//sprintf(str_Text,"출력 : %d",A1); // 이런식으로 사용
//TEXTMGR->Print(str_Text); // 텍스트출력
//sprintf(str_Text,"출력 : %d",A2); // 이런식으로 사용
//TEXTMGR->Print(str_Text); // 텍스트출력
//sprintf(str_Text,"출력 : %d",A3); // 이런식으로 사용
//TEXTMGR->Print(str_Text); // 텍스트출력
//한개 변수로 출력후에 재사용 가능
//dx D3DFRAME->GetDevice()->BeginScene();과 D3DFRAME->GetDevice()->EndScene(); 사이에 넣을것