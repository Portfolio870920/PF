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
			Scene_State	m_RsvScene;		// 변경될 씬 예약 Rsv = Reservation
			Scene_State	m_NowScene;		// 현재 씬
			bool		m_bSceneChage; // 씬이 변경될것인가 유무
			bool		m_bGameEnd;

public:
	~cScene_Mgr(void);
	static	cScene_Mgr*	GetInst(void);
			void		Init(HWND hwnd);
			void		FreeInst(void);
			void		SceneChange(Scene_State State);
			void		Update(void);
			void		Render(void);
	inline	void		ReservationChange(Scene_State State) // 씬 변경 예약함수
	{
		m_RsvScene		=	State;
		m_bSceneChage	=	true;
	}
	inline	Scene_State	GetRsvScene	(void)	// 예약된 씬을 얻어옴
	{
		return m_RsvScene;
	}
	inline	bool		GetbSceneChange(void)	// 예약유무를 얻어옴
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
