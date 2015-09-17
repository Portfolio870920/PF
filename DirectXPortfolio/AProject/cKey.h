#pragma once

const int	KEYMAX	=	256;

class cKey
{
private:
	static	cKey*		m_pInstance;			// Ŭ���� ������
			bool		m_bKeyDown[KEYMAX];		// Ű�� ���ȴ��� üũ�� �迭
			bool		m_bKeyUp[KEYMAX];		// Ű�� �������� üũ�� �迭
			//float		m_MousePos_x;			// ���콺 x��ǥ
			//float		m_MousePos_y;			// ���콺 y��ǥ

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