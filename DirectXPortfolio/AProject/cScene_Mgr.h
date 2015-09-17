#pragma once

enum Scene_State
{
	INTRO,
	MENU,
	GAME
};

class cScene_Mgr
{
private:
	HWND HWnd;
	cScene_Mgr(void);
	static	cScene_Mgr*	m_cScene_Mgr_Inst;
			CScene*		m_pcScene;
			Scene_State	m_RsvScene;		// ����� �� ���� Rsv = Reservation
			Scene_State	m_NowScene;		// ���� ��
			bool		m_bSceneChage; // ���� ����ɰ��ΰ� ����
			bool		m_bGameEnd;

public:
	~cScene_Mgr(void);
	static	cScene_Mgr*	GetInst(void);
			void		Init(HWND hwnd);
			void		FreeInst(void);
			void		SceneChange(Scene_State State);
			void		Update(void);
			void		Render(void);
	inline	void		ReservationChange(Scene_State State) // �� ���� �����Լ�
	{
		m_RsvScene		=	State;
		m_bSceneChage	=	true;
	}
	inline	Scene_State	GetRsvScene	(void)	// ����� ���� ����
	{
		return m_RsvScene;
	}
	inline	bool		GetbSceneChange(void)	// ���������� ����
	{
		return m_bSceneChage;
	}
	inline	void		SetGameEnd(bool bEnd)
	{
		m_bGameEnd	=	bEnd;
	}
	inline	bool		GetGameEnd(void)
	{
		return m_bGameEnd;
	}
	inline	Scene_State	GetNowScene(void)
	{
		return m_NowScene;
	}
};
