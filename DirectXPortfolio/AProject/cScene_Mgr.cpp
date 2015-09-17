#include "MainHeader.h"

cScene_Mgr*	cScene_Mgr::m_cScene_Mgr_Inst	=	NULL;

cScene_Mgr::cScene_Mgr(void)
{
}

cScene_Mgr::~cScene_Mgr(void)
{
}
void	cScene_Mgr::Init(HWND hwnd)
{
	CAMMGR->Init(AIR_CAMERA, false );
	HWnd			=	hwnd;
	m_bSceneChage	=	false;
	m_bGameEnd		=	false;
	m_pcScene		=	new CIntro;
	m_pcScene->HWnd	=	HWnd;
	m_NowScene		=	INTRO;
	m_pcScene->Init();			// ¾À ÃÊ±âÈ­
}
cScene_Mgr*	cScene_Mgr::GetInst(void)
{
	if (m_cScene_Mgr_Inst == NULL)
	{
		m_cScene_Mgr_Inst = new cScene_Mgr;
		RSCMGR->m_bSCENE_MGR	=	true;
	}
	return m_cScene_Mgr_Inst;
}
void	cScene_Mgr::FreeInst(void)
{
	if (m_cScene_Mgr_Inst != NULL)
	{
		if ( m_pcScene != NULL )
		{
			m_pcScene->m_Mesh.Destroy();//¸Å½¬ÆÄ±«
			m_pcScene->Free();
			delete m_pcScene;
			m_pcScene	=	NULL;
		}
		delete m_cScene_Mgr_Inst;
		m_cScene_Mgr_Inst	=	NULL;
		RSCMGR->m_bSCENE_MGR	=	false;
	}
}

void	cScene_Mgr::SceneChange(Scene_State State)
{
	if ( m_pcScene != NULL )
	{
		m_pcScene->Free();			// ¾À ÇØÁ¦
		delete m_pcScene;
		m_pcScene	=	NULL;
	}
	switch ( State )
	{
	case INTRO:
		{
			m_pcScene	=	new CIntro;
			m_pcScene->HWnd	=	HWnd;
			m_NowScene	=	INTRO;
			break;
		}
	case MENU:
		{
			m_pcScene	=	new CMenu;
			m_pcScene->HWnd	=	HWnd;
			m_NowScene	=	MENU;
			break;
		}
	case GAME:
		{
			m_pcScene	=	new CGame;
			m_pcScene->HWnd	=	HWnd;
			m_NowScene	=	GAME;
			break;
		}
	}
	m_pcScene->Init();			// ¾À ÃÊ±âÈ­
	m_bSceneChage	=	false;
}

void	cScene_Mgr::Update(void)
{
	m_pcScene->Update();
}

void	cScene_Mgr::Render(void)
{
	m_pcScene->Render();
}