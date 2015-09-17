#pragma once

const int	KEYMAX	=	256;

class cKey
{
private:
	static	cKey*		m_pInstance;			// 클래스 포인터
			bool		m_bKeyDown[KEYMAX];		// 키가 눌렸는지 체크할 배열
			bool		m_bKeyUp[KEYMAX];		// 키가 떼졌는지 체크할 배열
			//float		m_MousePos_x;			// 마우스 x좌표
			//float		m_MousePos_y;			// 마우스 y좌표

private:
	cKey(void);

public:
	~cKey(void);
	static	cKey*		GetInst(void);
			void		FreeInst(void);
			bool		StayKeyDown( INT nKey );
			bool		OnceKeyDown( INT nKey );
			bool		OnceKeyUp( INT nKey );
			bool		IsToggleKey( INT nKey );
};