#include "MainHeader.h"

CMenu::CMenu()
{
}

CMenu::~CMenu(void)
{
}

unsigned int CMenu::Init(void)
{
	//스카이박스
	m_Mesh.LoadMesh("./xFile","skybox.x");
	m_Mesh.SetScale(500.0f,500.0f,500.0f);
	if( RSCMGR->m_bMOB_MGR )	// 몹매니져가 한번이라도 실행됐었다면 할당해제시킨다.
		MOBMGR->FreeInst();
	if( !RSCMGR->m_bMOB_MGR )	// 몹매니져가 한번이라도 실행되지 않았다면 몬스터를 추가
	{
		MOBMGR->AddMob(KNIGHT);	// 몬스터
	}
	
	if ( !RSCMGR->m_bMAP_MGR )	// 맵매니져가 한번도 실행되지 않았다면
	{
		//지형
		CTerrain::LoadEffect ( "./SHADER/Map.fx" );
		MAPMGR->Init ( "./IMG/Height.bmp\0" );

		MAPMGR->CreateChannelTexture( "./IMG/Perturbate.tga\0" );
		MAPMGR->CreateTextureUsingA( "./IMG/alpha.tga\0" );
		MAPMGR->CreateTextureUsingR( "./IMG/red.tga\0" );
		MAPMGR->CreateTextureUsingG( "./IMG/green.tga\0" );
		MAPMGR->CreateTextureUsingB( "./IMG/blue.tga\0" );
		//
	}

	//옵션들어갈때 기존 메뉴를 멈추는 용도
	m_bIsStop	=	FALSE;

	//start에서 시작하게 1로 초기화
	Select = 1;
	//클릭 초기화
	Click = 0;

	//텍스쳐포인터 초기화 하고 그 안에 이미지 넣고 버튼들의 위치 잡기
	pTexTitle = NULL;
	D3DXCreateTextureFromFileA( D3DFRAME->GetDevice(), "MenuTex/Title.png\0", &pTexTitle);
	Title = cUiBase->CreateVertex2D(340, 940, 50, 250, 0.0f);

	pTexStart = NULL;
	D3DXCreateTextureFromFileA ( D3DFRAME->GetDevice(), "MenuTex/Start.png\0", &pTexStart );
	pTexRStart = NULL;
	D3DXCreateTextureFromFileA ( D3DFRAME->GetDevice(), "MenuTex/RStart.png\0", &pTexRStart );
	Start = cUiBase->CreateVertex2D( 120, 400,  800, 920, 0.0f);

	pTexOption= NULL;
	D3DXCreateTextureFromFileA ( D3DFRAME->GetDevice(), "MenuTex/Option.png\0", &pTexOption );
	pTexROption= NULL;
	D3DXCreateTextureFromFileA ( D3DFRAME->GetDevice(), "MenuTex/ROption.png\0", &pTexROption );
	Option = cUiBase->CreateVertex2D(500, 780,  800, 920, 0.0f);

	pTexExit  = NULL;
	D3DXCreateTextureFromFileA ( D3DFRAME->GetDevice(), "MenuTex/Exit.png\0", &pTexExit );
	pTexRExit  = NULL;
	D3DXCreateTextureFromFileA ( D3DFRAME->GetDevice(), "MenuTex/RExit.png\0", &pTexRExit );
	Exit = cUiBase->CreateVertex2D(880, 1160, 800, 920, 0.0f);

	pTexLoading = NULL;
	D3DXCreateTextureFromFileA( D3DFRAME->GetDevice(), "MenuTex/Loading.jpg\0", &pTexLoading);
	Loading = cUiBase->CreateVertex2D(0, 1280, 0, 960, 0.0f);
	//
	
	//사운드재생
	SOUNDMGR->PlaySound(MAIN_SOUND);
	return 0UL;
}
unsigned int CMenu::Update(void)
{
	//옵션키지않았을때
	if( !m_bIsStop )
	{
		MOBMGR->SetIterBengin();
		while ( MOBMGR->GetIter() != MOBMGR->IterEnd() )
		{
			(MOBMGR->MobVec(MOBMGR->GetIter()))->Update();
			MOBMGR->IterInc();
		}
		CAMMGR->Update();

		
		GetCursorPos(&ptMouse);	//커서위치받기
		ScreenToClient(HWnd, &ptMouse);	//좌표설정 창기준으로

		//키보드로 고르기
		if(KEYMGR->OnceKeyDown(VK_RIGHT)) Select += 1;
		if(KEYMGR->OnceKeyDown(VK_LEFT)) Select -= 1;

		if(KEYMGR->OnceKeyDown(VK_RETURN))
		{
			if		(Select == 1) Click = 1;
			else if (Select == 2) Click = 2;
			else if (Select == 3) Click = 3;
			//else if (Select == 4) Click = 4;
		}

		if(Select>3) Select = 1;
		if(Select<1) Select = 3;
		//if(Select<1) Select = 4;
		//

		//마우스로 고르기
		//	스타트
		if( ( ( Start[0].x < ptMouse.x && ptMouse.x < Start[1].x) && (Start[0].y < ptMouse.y && ptMouse.y < Start[2].y)
			&& ( KEYMGR->StayKeyDown(VK_LBUTTON) ) ) || ( Select == 1 && KEYMGR->StayKeyDown(VK_RETURN) ) )
		{
			Select = 1, Click = 1;
			SCENEMGR->ReservationChange(GAME);
		}
		//	옵션
		if((Option[0].x < ptMouse.x && ptMouse.x < Option[1].x) && (Option[0].y < ptMouse.y && ptMouse.y < Option[2].y)
			&& KEYMGR->StayKeyDown(VK_LBUTTON))
		{
			Select = 2, Click = 2;
		}
		//	게임 종료
		if( ( ( Exit[0].x < ptMouse.x && ptMouse.x < Exit[1].x) && (Exit[0].y < ptMouse.y && ptMouse.y < Exit[2].y)
			&& KEYMGR->StayKeyDown(VK_LBUTTON) ) || ( Select == 4 && KEYMGR->StayKeyDown(VK_RETURN) ) )
		{
			Select = 3, Click = 3;
			SCENEMGR->SetGameEnd(true);
		}
		//

		//옵션골랐을경우(Click == 2), 기존 업데이트를 멈춤
		if(Click == 2)
			m_bIsStop = TRUE;
		//
	}
	
	//옵션골랐을때 기존업데이트멈추고 옵션 업데이트
	if( m_bIsStop )
	{
		COPTION->Update();
	}
	
	D3DFRAME->GetDevice()->Clear ( 0, NULL,	D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET, 0x0000FF00, 1.0f, 0UL );
	return 0UL;
}
void CMenu::Render(void)
{
	//스카이 박스
	m_Mesh.Render();
	//지형
	MAPMGR->Render();
	//몬스터
	MOBMGR->SetIterBengin();
	while ( MOBMGR->GetIter() != MOBMGR->IterEnd() )
	{
		(MOBMGR->MobVec(MOBMGR->GetIter()))->Render();
		MOBMGR->IterInc();
	}

	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_ALPHABLENDENABLE, TRUE	);			//알파값 예제
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_SRCBLEND, D3DBLEND_SRCALPHA	);
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA	);
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );

	//제목
	cUiBase->Render(pTexTitle, Title);

	//골랐을경우 하얀 텍스쳐를 쓰고
	//고르지 않았을때는 검은 텍스쳐를 쓴다

	//스타트버튼
	if(Select == 1)
		cUiBase->Render(pTexRStart, Start);
	else
		cUiBase->Render(pTexStart, Start);

	//옵션버튼
	if(Select == 2)
		cUiBase->Render(pTexROption, Option);
	else
		cUiBase->Render(pTexOption, Option);

	//종료버튼
	if(Select == 3)
		cUiBase->Render(pTexRExit, Exit);
	else
		cUiBase->Render(pTexExit, Exit);

	//스타트를 눌렀을 경우(Click == 1), 로딩이미지를 띄우기
	if(Click == 1)
		cUiBase->Render(pTexLoading, Loading);
	//

	//옵션을 눌렀을경우(Click == 2), 옵션 그리기
	if(Click == 2)	
		COPTION->Render();
	//
}
void CMenu::Free(void)
{
	SOUNDMGR->StopSound(MAIN_SOUND);
	MOBMGR->FreeInst();
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_ALPHABLENDENABLE, FALSE	);
}