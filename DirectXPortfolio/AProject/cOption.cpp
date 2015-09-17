#include "MainHeader.h"

cOption*	cOption::OptionInst	=	NULL;

cOption::cOption(void)
{
}

cOption::~cOption(void)
{
}

cOption*	cOption::GetInst(void)
{
	if ( !OptionInst )
	{
		OptionInst	=	new cOption;
		RSCMGR->m_bOPTION	=	true;
	}
	return OptionInst;
}

void	cOption::FreeInst(void)
{
	if ( OptionInst )
	{
		delete OptionInst;
		OptionInst	=	NULL;
		RSCMGR->m_bOPTION	=	false;
		D3DFRAME->GetDevice()->SetRenderState(	D3DRS_ALPHABLENDENABLE, FALSE	);
	}
}

unsigned int cOption::Init(HWND hwnd)
{
	Select = 0;
	Click = 0;
	Select2 = 0;
	m_hwnd	=	hwnd;
	//옵션킬시 뒷배경 검게
	pTexBlack	=NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "OptionTex/Black.png\0", &pTexBlack);
	Black = cUiBase->CreateVertex2D(0, 1280, 0, 960, 0.0f);

	//기본 옵션
	pTexOptionBG = NULL;
	D3DXCreateTextureFromFileA (D3DFRAME->GetDevice(), "OptionTex/OptionBG.png\0", &pTexOptionBG);
	OptionBG = cUiBase->CreateVertex2D(360, 910, 120, 830, 0.0f);

	//옵션 버튼들
	pTexReTry = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "OptionTex/ReTry.png\0", &pTexReTry);
	pTexShowControl = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "OptionTex/ShowControl.png\0", &pTexShowControl);
	pTexMainMenu = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "OptionTex/MainMenu.png\0", &pTexMainMenu);

	//컨트롤 설명
	Controls = cUiBase->CreateVertex2D(320, 960, 80, 880, 0.0f);
	pTexControls = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "OptionTex/Controls.png\0", &pTexControls);
	pTexComBoSystem = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "OptionTex/ComboSystem.png\0", &pTexComBoSystem);
	pTexBuffSkills	= NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "OptionTex/BuffSkills.png\0", &pTexBuffSkills);

	return 0;
}

unsigned int cOption::Update(void)
{
	GetCursorPos(&ptMouse);
	ScreenToClient(m_hwnd, &ptMouse);

	//커서 이동과 선택 (키보드,마우스)
	if(KEYMGR->OnceKeyDown(VK_UP))		Select -= 1;
	if(KEYMGR->OnceKeyDown(VK_DOWN))	Select += 1;

	if(KEYMGR->OnceKeyDown(VK_RETURN))
	{
		if		(Select == 1) Click = 1;
		else if (Select == 2) Click = 2;
		else if (Select == 3) Click = 3;
	}

	if(Select>3) Select = 1;
	if(Select<1) Select = 3;

	if( (  530 < ptMouse.x && ptMouse.x < 740 ) && (250 < ptMouse.y && ptMouse.y < 320) )
	{
		Select = 1;
		if(KEYMGR->StayKeyDown(VK_LBUTTON) || KEYMGR->StayKeyDown(VK_RETURN) )
			Click = 1;
	}

	if( (  420 < ptMouse.x && ptMouse.x < 855 ) && (425 < ptMouse.y && ptMouse.y < 495) )
	{
		Select = 2;
		if(KEYMGR->StayKeyDown(VK_LBUTTON) || KEYMGR->StayKeyDown(VK_RETURN) )
			Click = 2;
	}

	if( (  420 < ptMouse.x && ptMouse.x < 855 ) && (620 < ptMouse.y && ptMouse.y < 690) )
	{
		Select = 3;
		if(KEYMGR->StayKeyDown(VK_LBUTTON) || KEYMGR->StayKeyDown(VK_RETURN) )
			Click = 3;
	}

	if(Click == 1)	//스테이지선택으로(재시작)
	{
		SCENEMGR->ReservationChange(GAME);
		CAMMGR->SetbGetVec();
		Click = 0;
	}

	if(Click == 3)	//메인메뉴로
	{
		SCENEMGR->ReservationChange(MENU);
		Click = 0;
	}

	if(Click == 2)	//컨트롤 설명 넘기기
	{
		if(KEYMGR->OnceKeyDown(VK_LEFT))	Select2 -= 1;
		if(KEYMGR->OnceKeyDown(VK_RIGHT))	Select2 += 1;

		if(KEYMGR->OnceKeyDown(VK_LBUTTON)) Select2 += 1;		

		if(Select2 > 3) Select2 = 1;
		if(Select2 < 1) Select2 = 3;
	}

	return 0UL;
}

void cOption::Render(void)
{
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_ALPHABLENDENABLE, TRUE	);			//알파블렌딩
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_SRCBLEND, D3DBLEND_SRCALPHA	);
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA	);
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );

	cUiBase->Render(pTexBlack, Black);
	cUiBase->Render(pTexOptionBG, OptionBG);

	if(Select == 1)
		cUiBase->Render(pTexReTry, OptionBG);
	if(Select == 2)
		cUiBase->Render(pTexShowControl, OptionBG);
	if(Select == 3)
		cUiBase->Render(pTexMainMenu, OptionBG);

	//조작법
	if(Click == 2)
	{
		if(KEYMGR->StayKeyDown(VK_ESCAPE)) //esc누르면 설명벗어남
		{
			Click = 0;
		}
		if(Select2 == 1)
			cUiBase->Render(pTexControls, Controls);
		if(Select2 == 2)
			cUiBase->Render(pTexComBoSystem, Controls);
		if(Select2 == 3)
			cUiBase->Render(pTexBuffSkills, Controls);
	}

}