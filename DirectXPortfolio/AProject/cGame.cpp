#include "MainHeader.h"

CGame::CGame(void)
{
}

CGame::~CGame(void)
{

}


void	CGame::UIBAR( bool BeHit )	//HP바나 ST바의 움직임
{
	PlayerHp	= CHARMGR->GetPlay_Status().m_HP/CHARMGR->GetPlay_Status().m_MaxHp; // 캐릭터 hp 바
	PlayerHp	= __max(0.0f, PlayerHp);
	Bar			= ((993.0f - 306.0f) * PlayerHp + 306.0f);
	PlayerHPFront[1].x = Bar , PlayerHPFront[3].x = Bar;

	if( BeHit == true )
	{
 		MonsterHp	= (MOBMGR->MobVec(MOBMGR->GetIter()))->GetPlay_Status().m_HP/(MOBMGR->MobVec(MOBMGR->GetIter()))->GetPlay_Status().m_MaxHp;  // 몬스터 hp 바
		MonsterHp	= __max(0.0f,MonsterHp);
		Bar			= (1213.0f	-	73.0f) * MonsterHp + 73.0f;
		BossHPFront[1].x = Bar	, BossHPFront[3].x	= Bar;
	}

	PlayerSt	= CHARMGR->GetPlay_Status().m_Stamina/CHARMGR->GetPlay_Status().m_MaxSt; // 캐릭터 스태미너바
	PlayerSt	= __max(0.0f,PlayerSt);
	Bar			= ((994.0f - 306.0f ) * PlayerSt + 306.0f);
	PlayerSTFront[1].x	= Bar	,	PlayerSTFront[3].x	= Bar;
}

unsigned int	CGame::Init(void)
{
	srand((unsigned int) time(NULL));
	//스카이박스
	m_Mesh.LoadMesh("./xFile","skybox.x");
	m_Mesh.SetScale(1500.0f,2000.0f,1500.0f);

	//마우스 보임 상태
	m_bIsShowMS	=	TRUE;
	//업데이트 정지 여부 체크
	m_bIsStop	=	FALSE;
	//로드
	//DATAMGR->pPlayer = CHARMGR;
	//DATAMGR->Load();

	vNormal	=	D3DXVECTOR3(0.0f,0.0f,0.0f); // 케릭터 위치 삼각형 법선
	fNomalAngle	=	0.0f; // 케릭터 위치 삼각형 법선 dot y축 값

	m_bSelectEnemy	=	false;	 // 적선택 초기화
	m_bChoiceWp	=	false;		// 무기선택 초기화
	m_bSelectDifficulty = false; //난이도선택 초기화
	
	//몬스터
	//MOBMGR->FreeInst();
	//MOBMGR->AddMob( ALDUIN );
	//MOBMGR->AddMob( ALDUIN, D3DXVECTOR3( -100.0f, 0.0f, -100.0f ), D3DXVECTOR3 ( 0.1f, 0.1f, 0.1f ), 2.7f);
	//MOBMGR->AddMob( KNIGHT );
	//MOBMGR->AddMob( KNIGHT, D3DXVECTOR3( 120.0f, 0.0f, 100.0f ), D3DXVECTOR3 ( 0.1f, 0.15f, 0.15f ), 2.7f );
	//MOBMGR->AddMob( KNIGHT, D3DXVECTOR3( 80.0f, 0.0f, 100.0f ), D3DXVECTOR3 ( 0.1f, 0.15f, 0.15f ), 2.7f );
	//MOBMGR->Init(KNIGHT);

	if ( !RSCMGR->m_bMAP_MGR )//맵매니져가 한번도 실행되지 않았다면
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

	//HP바 텍스쳐생성과 버텍스생성
	pTexPlayerHPB = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/PlayerHealthBarB.png\0", &pTexPlayerHPB);
	PlayerHPBack = cUiBase->CreateVertex2D(300, 1000, 890, 935, 0.0f);

	pTexPlayerHPF = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/PlayerHealthBarF.png\0", &pTexPlayerHPF);
	PlayerHPFront = cUiBase->CreateVertex2D(306, 993, 895, 930, 0.0f);

	pTexBossHPB = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/BossHealthBarB.png\0", &pTexBossHPB);
	BossHPBack = cUiBase->CreateVertex2D(60, 1220, 30, 80, 0.0f);

	pTexBossHPF = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/BossHealthBarF.png\0", &pTexBossHPF);
	BossHPFront = cUiBase->CreateVertex2D(73, 1213, 40, 72, 0.0f);
	//

	//스태미나
	PlayerSTBack = cUiBase->CreateVertex2D(300, 1000, 870, 890, 0.0f);	//스태미나, HP랑 같은 배경씀

	pTexPlayerSTF = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/PlayerStaminaBarF.png\0", &pTexPlayerSTF);
	pTexPlayerSTF2 = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/PlayerStaminaBarF2.png\0", &pTexPlayerSTF2);
	PlayerSTFront = cUiBase->CreateVertex2D(306, 994, 875, 885, 0.0f);


	//버프
	BuffInit();
	//

	//무기 선택 화면
	Select	= 0;
	Click	= 0;
	pChoiceWeapon	=	NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(),"GameTex/SelectBG.png\0",&pChoiceWeapon);
	ChoiceWeapon	=	cUiBase->CreateVertex2D(0,1280,0,960,1.0f);

	pDualSword	=	NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/SelectDS.png\0", &pDualSword);

	pGreatSword	=	NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/SelectGS.png\0", &pGreatSword);

	//적 선택 화면
	EnemySelect	= 0;
	EnemyClick	= 0;
	pTexSEnemyBG	=	NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/EnemySelectBG.png\0", &pTexSEnemyBG);
	SelectEnemy	=	cUiBase->CreateVertex2D(0, 1280, 0, 960, 1.0f);

	pTexDarkKnight	=	NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/SelectDK.png\0", &pTexDarkKnight);

	pTexBlackDragon	=	NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/SelectBD.png\0", &pTexBlackDragon);

	//난이도선택
	DSelect = 0;
	DClick = 0;

	//이지
	pTexEasy = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Difficulty/Easy.png\0", &pTexEasy);
	pTexREasy = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Difficulty/REasy.png\0", &pTexREasy);
	Easy = cUiBase->CreateVertex2D(500, 780, 210, 330, 0.0f);

	//노말
	pTexNormal = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Difficulty/Normal.png\0", &pTexNormal);
	pTexRNormal = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Difficulty/RNormal.png\0", &pTexRNormal);
	Normal = cUiBase->CreateVertex2D(500, 780, 420, 540, 0.0f);

	//하드
	pTexHard = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Difficulty/Hard.png\0", &pTexHard);
	pTexRHard = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Difficulty/RHard.png\0", &pTexRHard);
	Hard = cUiBase->CreateVertex2D(500, 780, 625, 745, 0.0f);


	//콤보
	ComboInit();
	SOUNDMGR->PlaySound(BGM_B2);
	//게임끝
	//플레이어사망
	pTexDead	=	NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Dead.png\0", &pTexDead);
	//몹사망
	pTexDefeat	=	NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Defeat.png\0", &pTexDefeat);
	End	=	cUiBase->CreateVertex2D(0, 1280, 0, 960, 0.0f);

	return 0UL;
}
unsigned int	CGame::Update(void)
{
	if ( KEYMGR->OnceKeyDown('K') )
		SOUNDMGR->PlaySound(MOB_KNIGHT_ATTACK);
	if ( !m_bIsStop )
	{
		//세이브 (일단 실시간)
		DATAMGR->Save();

		GetCursorPos(&CMouse);			// 마우스 위치 받음
		ScreenToClient(HWnd,&CMouse);	// 현재 게임창의 왼쪽 맨위 를 0,0으로 인식하게 함


		if(m_bSelectEnemy == false)	//적 선택하기
		{
			//마우스로 고를때
			if(( 100 < CMouse.x && CMouse.x < 600) && (220 < CMouse.y && CMouse.y < 860))	
			{
				EnemySelect = 1;
				if(KEYMGR->OnceKeyDown(VK_LBUTTON))
				{
					EnemyClick = 1;
					m_bSelectEnemy	=	true;
				}	
			}
			else if((680 < CMouse.x && CMouse.x < 1180) && (220 < CMouse.y && CMouse.y < 860))
			{
				EnemySelect = 2;
				if(KEYMGR->OnceKeyDown(VK_LBUTTON))
				{
					EnemyClick = 2;
					m_bSelectEnemy	=	true;
				}
			}
			//
			//키보드로 고를때
			if(KEYMGR->OnceKeyDown(VK_LEFT))  EnemySelect -= 1;	
			if(KEYMGR->OnceKeyDown(VK_RIGHT)) EnemySelect += 1;

			if(EnemySelect > 2) EnemySelect = 1;
			if(EnemySelect < 1) EnemySelect = 2;

			if(KEYMGR->OnceKeyDown(VK_RETURN))
			{
				if		(EnemySelect == 1) EnemyClick = 1, m_bSelectEnemy	=	true;
				else if (EnemySelect == 2) EnemyClick = 2, m_bSelectEnemy	=	true;
			}
			//
		}
		//난이도 고르기
		if((m_bSelectEnemy == true) && (m_bSelectDifficulty == false))
		{
			//마우스로 고를때
			//이지
			if(( 500 < CMouse.x && CMouse.x < 780) && (210 < CMouse.y && CMouse.y < 330))	
			{
				DSelect = 1;
				if(KEYMGR->OnceKeyDown(VK_LBUTTON))
				{
					DClick = 1;
					m_bSelectDifficulty = true;
				}	
			}
			//노말
			else if((500 < CMouse.x && CMouse.x < 780) && (420 < CMouse.y && CMouse.y < 540))
			{
				DSelect = 2;
				if(KEYMGR->OnceKeyDown(VK_LBUTTON))
				{
					DClick = 2;
					m_bSelectDifficulty = true;
				}
			}
			//하드
			else if((500 < CMouse.x && CMouse.x < 780) && (625 < CMouse.y && CMouse.y < 745))
			{
				DSelect = 3;
				if(KEYMGR->OnceKeyDown(VK_LBUTTON))
				{
					DClick = 3;
					m_bSelectDifficulty = true;
				}
			}
			//
			//키보드로 고를때
			if(KEYMGR->OnceKeyDown(VK_UP))  DSelect -= 1;	
			if(KEYMGR->OnceKeyDown(VK_DOWN)) DSelect += 1;

			if(DSelect > 3) DSelect = 1;
			if(DSelect < 1) DSelect = 3;

			if(KEYMGR->OnceKeyDown(VK_RETURN))
			{
				if		(DSelect == 1) DClick = 1, m_bSelectDifficulty = true;
				else if (DSelect == 2) DClick = 2, m_bSelectDifficulty = true;
				else if (DSelect == 3) DClick = 3, m_bSelectDifficulty = true;
			}

			if(m_bSelectEnemy)
			{
				//난이도에 따라 적 증감
				switch(EnemyClick)
				{
				case 1:
					MOBMGR->FreeInst();
					if(DClick == 1)
					{
						MOBMGR->AddMob( KNIGHT );
					}

					else if(DClick == 2)
					{
						MOBMGR->AddMob( KNIGHT );
						MOBMGR->AddMob( KNIGHT, D3DXVECTOR3( 120.0f, 0.0f, 100.0f ), D3DXVECTOR3 ( 0.1f, 0.15f, 0.15f ), 2.7f );
					}

					else if(DClick == 3)
					{
						MOBMGR->AddMob( KNIGHT );
						MOBMGR->AddMob( KNIGHT, D3DXVECTOR3( 120.0f, 0.0f, 100.0f ), D3DXVECTOR3 ( 0.1f, 0.15f, 0.15f ), 2.7f );
						MOBMGR->AddMob( KNIGHT, D3DXVECTOR3( 80.0f, 0.0f, 100.0f ), D3DXVECTOR3 ( 0.1f, 0.15f, 0.15f ), 2.7f );
					}
					break;

				case 2:
					MOBMGR->FreeInst();
					if(DClick == 1)
					{
						MOBMGR->AddMob( ALDUIN );
					}

					else if(DClick == 2)
					{
						MOBMGR->AddMob( ALDUIN );
						MOBMGR->AddMob( ALDUIN, D3DXVECTOR3( -100.0f, 0.0f, -100.0f ), D3DXVECTOR3 ( 0.1f, 0.1f, 0.1f ), 2.7f);
					}

					else if(DClick == 3)
					{
						MOBMGR->AddMob( ALDUIN );
						MOBMGR->AddMob( ALDUIN, D3DXVECTOR3( -100.0f, 0.0f, -100.0f ), D3DXVECTOR3 ( 0.1f, 0.1f, 0.1f ), 2.7f);
						MOBMGR->AddMob( ALDUIN, D3DXVECTOR3( 130.0f, 0.0f, 0.0f ), D3DXVECTOR3 ( 0.1f, 0.1f, 0.1f ), 2.7f );
					}
					break;
				}
				//
			}
		}
		//적선택이 끝나면 무기선택시작
		if((m_bSelectEnemy == true) && (m_bChoiceWp	== false) &&  (DClick >= 1))
		{
			//마우스로 선택
			if(( 100 < CMouse.x && CMouse.x < 600) && (220 < CMouse.y && CMouse.y < 860))
			{
				Select = 1;
				if(KEYMGR->OnceKeyDown(VK_LBUTTON))
				{
					Click = 1;
					m_bChoiceWp	=	true;
				}	
			}
			else if((680 < CMouse.x && CMouse.x < 1180) && (220 < CMouse.y && CMouse.y < 860))
			{
				Select = 2;
				if(KEYMGR->OnceKeyDown(VK_LBUTTON))
				{
					Click = 2;
					m_bChoiceWp	=	true;
				}
			}
			//
			//키보드로 고를때
			if(KEYMGR->OnceKeyDown(VK_LEFT))  Select -= 1;	
			if(KEYMGR->OnceKeyDown(VK_RIGHT)) Select += 1;

			if(Select > 2) Select = 1;
			if(Select < 1) Select = 2;

			if(KEYMGR->OnceKeyDown(VK_RETURN))
			{
				if		(Select == 1) Click = 1, m_bChoiceWp	=	true;
				else if (Select == 2) Click = 2, m_bChoiceWp	=	true;
			}
			//
			if(m_bChoiceWp)
			{
				//무기선택 완료시 캐릭터 초기화
				switch(Click)
				{
				case 1:
					CHARMGR->Init(1);
					break;
				case 2:
					CHARMGR->Init(2);
					break;
				}
			}
		}

		//적선택과 무기선택이 끝난 후 실행되는 업데이트
		if(( m_bChoiceWp  == true) && ( m_bSelectEnemy == true))
		{
			if( m_bIsShowMS == TRUE )
			{
				m_bIsShowMS	=	FALSE;
				ShowCursor(m_bIsShowMS);
			}
			MOBMGR->Update();
			MOBMGR->SetIterBengin();
			while ( MOBMGR->GetIter() != MOBMGR->IterEnd() )
			{
				//(MOBMGR->MobVec(MOBMGR->GetIter()))->Update();
				UIBAR( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetBeHit() );	//	HP 바
				MOBMGR->IterInc();
			}
			
			CHARMGR->Update();
			CAMMGR->Update();
			//마우스 위치 초기화
			SetCursorPos(CENTERPOS_X,CENTERPOS_Y);
			//마우스커서 숨기기
			//LoadCursor(NULL,NULL);
		}
	}
	// esc를 한번도 안눌렀다면 들어옴 누르면 안들어옴 다시 누르면 들어옴
	if ( KEYMGR->OnceKeyDown(VK_ESCAPE) && ( m_bSelectEnemy == true) && (m_bChoiceWp == true))	
	{
		//멈춤
		switch( m_bIsStop )
		{
		case 0:
			{
				m_bIsStop	=	TRUE;
				ShowCursor(m_bIsStop);
				break;
			}

		case 1:
			{
				m_bIsStop	=	FALSE;
				ShowCursor(m_bIsStop);
				break;
			}
		}
	}
	//옵션켰을때 게임업데이트 멈추고 옵션업데이트
	if ( m_bIsStop )
	{
		COPTION->Update();
	}

	return 0UL;
}
void	CGame::Render(void)
{
	D3DFRAME->GetDevice()->Clear ( 0, NULL,	D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET, 0x00000000, 1.0f, 0UL );

	//적고를때 텍스쳐렌더링
	if( m_bSelectEnemy == false)
	{
		cUiBase->Render(pTexSEnemyBG,SelectEnemy);

		if(EnemySelect == 1) cUiBase->Render(pTexDarkKnight, SelectEnemy);		//골랐을때 텍스쳐변화
		if(EnemySelect == 2) cUiBase->Render(pTexBlackDragon, SelectEnemy);
	}
	//
	if((m_bSelectEnemy == true) && (m_bSelectDifficulty == false))
	{
		//난이도고를때 텍스쳐렌더링
		//이지
		if(DSelect == 1) cUiBase->Render(pTexREasy, Easy);	
		else cUiBase->Render(pTexEasy, Easy);	//골랐을때 텍스쳐변화
		//노말
		if(DSelect == 2) cUiBase->Render(pTexRNormal, Normal);
		else cUiBase->Render(pTexNormal, Normal);
		//하드
		if(DSelect == 3) cUiBase->Render(pTexRHard, Hard);
		else cUiBase->Render(pTexHard, Hard);
	}

	//무기고를때 텍스쳐렌더링
	if((m_bSelectEnemy == true) && (m_bChoiceWp	== false) &&  (DClick >= 1))
	{
		cUiBase->Render(pChoiceWeapon,ChoiceWeapon);

		if(Select == 1) cUiBase->Render(pDualSword, ChoiceWeapon);		//골랐을때 텍스쳐변화
		if(Select == 2) cUiBase->Render(pGreatSword, ChoiceWeapon);
	}
	//

	//적도 고르고 무기도 골랐을때 나머지 렌더링
	if(( m_bSelectEnemy == true) && ( m_bChoiceWp  == true))
	{
		//스카이 박스
		m_Mesh.Render();

		//그림자 시작
		D3DFRAME->BeginShadow( CHARMGR->GetPos(), D3DXVECTOR3(0.5f,-1.0f, 0.0f) );
		CHARMGR->Render();
		MOBMGR->SetIterBengin();
		while ( MOBMGR->GetIter() != MOBMGR->IterEnd() )
		{
			(MOBMGR->MobVec(MOBMGR->GetIter()))->Render();
			MOBMGR->IterInc();
		}
		D3DFRAME->EndShadow();
		// 그림자 종료

		//캐릭터
		CHARMGR->Render();
		MOBMGR->SetIterBengin();
		while ( MOBMGR->GetIter() != MOBMGR->IterEnd() )
		{
			(MOBMGR->MobVec(MOBMGR->GetIter()))->Render();
			MOBMGR->IterInc();
		}

		//지형
		MAPMGR->Render();

		//HP바 그리기
		cUiBase->Render(pTexPlayerHPB, PlayerHPBack);	//플레이어
		cUiBase->Render(pTexPlayerHPF, PlayerHPFront);

		cUiBase->Render(pTexBossHPB, BossHPBack);		//보스
		cUiBase->Render(pTexBossHPF, BossHPFront);
		//

		//ST바 그리기
		cUiBase->Render(pTexPlayerHPB, PlayerSTBack);
		if(CHARMGR->GetBuffStamina() == true)
		{
			cUiBase->Render(pTexPlayerSTF2, PlayerSTFront);
		}
		else
		{
			cUiBase->Render(pTexPlayerSTF, PlayerSTFront);
		}
		

		//버프
		BuffRender();

		//콤보
		if(CHARMGR->GetPlay_Status().m_RealCombo > 0)
		{
			ComboRender();
		}

		//플레이어사망
		if(CHARMGR->GetPlay_Status().m_Status == STATUS_DIE /*|| MOBMGR->GetDieCount() == MOBMGR->GetVectorSize()// 몹 다 잡았으면 알려쥼 */ )
		{
			D3DFRAME->GetDevice()->SetRenderState(	D3DRS_ALPHABLENDENABLE, TRUE	);			//알파값 예제
			D3DFRAME->GetDevice()->SetRenderState(	D3DRS_SRCBLEND, D3DBLEND_SRCALPHA	);
			D3DFRAME->GetDevice()->SetRenderState(	D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA	);
			D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
			D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
			D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );		
			cUiBase->Render(pTexDead, End);
		}
		if(MOBMGR->GetDieCount() == MOBMGR->GetVectorSize())
		{
			D3DFRAME->GetDevice()->SetRenderState(	D3DRS_ALPHABLENDENABLE, TRUE	);			//알파값 예제
			D3DFRAME->GetDevice()->SetRenderState(	D3DRS_SRCBLEND, D3DBLEND_SRCALPHA	);
			D3DFRAME->GetDevice()->SetRenderState(	D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA	);
			D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
			D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
			D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );		
			cUiBase->Render(pTexDefeat, End);
		}
	}

	//옵션켰을때 게임렌딩멈추고 옵션렌더링
	if(m_bIsStop)
	{
		COPTION->Render();
	}
	//

	// fps 출력
	TCHAR	str_Text[256];
	ZeroMemory(str_Text,sizeof(str_Text));
	sprintf(str_Text,"fps : %d",TIMEMGR->GetFrameRate());
	TEXTMGR->Print(str_Text);
	//

	// 마우스 좌표 출력
	/*sprintf(str_Text,"mouse_x : %d, mouse_y : %d",CMouse.x, CMouse.y);
	TEXTMGR->Print(str_Text, 105);*/
}
void	CGame::Free(void)
{
	//초기화할때 알파블렌딩 끔
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_ALPHABLENDENABLE, FALSE	);
	SOUNDMGR->StopSound(BGM_B2);
}


void	CGame::ComboInit(void)
{
	pTexCombo	=	NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Combo/Combo.png\0", &pTexCombo);
	Combo	=	cUiBase->CreateVertex2D(1050, 1210, 365, 420, 0.0f);

	ComboNum	=	cUiBase->CreateVertex2D(930, 1030, 320, 420, 0.0f);

	pTexCrit	=	NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Combo/Crit.png\0", &pTexCrit);
	Crit	=	cUiBase->CreateVertex2D(955, 1280, 280, 500, 0.1f);

	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Combo/1.png\0", &pTexNum1);
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Combo/2.png\0", &pTexNum2);
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Combo/3.png\0", &pTexNum3);
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Combo/4.png\0", &pTexNum4);
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Combo/5.png\0", &pTexNum5);
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Combo/6.png\0", &pTexNum6);
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Combo/7.png\0", &pTexNum7);
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Combo/8.png\0", &pTexNum8);
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Combo/9.png\0", &pTexNum9);
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Combo/10.png\0", &pTexNum10);

	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Combo/11.png\0", &pTexNum11);
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Combo/12.png\0", &pTexNum12);
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Combo/13.png\0", &pTexNum13);
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Combo/14.png\0", &pTexNum14);
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Combo/15.png\0", &pTexNum15);
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Combo/16.png\0", &pTexNum16);
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Combo/17.png\0", &pTexNum17);
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Combo/18.png\0", &pTexNum18);
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Combo/19.png\0", &pTexNum19);
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Combo/20.png\0", &pTexNum20);
}

void	CGame::ComboRender(void)
{
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_ALPHABLENDENABLE, TRUE	);			//알파값 예제
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_SRCBLEND, D3DBLEND_SRCALPHA	);
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA	);
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );


	if(CHARMGR->GetPlay_Status().m_CheckSkill == 1)		//뒤에 깔리는 거 먼저 렌딩
		cUiBase->Render(pTexCrit, Crit);

	cUiBase->Render(pTexCombo, Combo);


	if(CHARMGR->GetPlay_Status().m_RealCombo == 1)
		cUiBase->Render(pTexNum1, ComboNum);
	if(CHARMGR->GetPlay_Status().m_RealCombo == 2)
		cUiBase->Render(pTexNum2, ComboNum);
	if(CHARMGR->GetPlay_Status().m_RealCombo == 3)
		cUiBase->Render(pTexNum3, ComboNum);
	if(CHARMGR->GetPlay_Status().m_RealCombo == 4)
		cUiBase->Render(pTexNum4, ComboNum);
	if(CHARMGR->GetPlay_Status().m_RealCombo == 5)
		cUiBase->Render(pTexNum5, ComboNum);
	if(CHARMGR->GetPlay_Status().m_RealCombo == 6)
		cUiBase->Render(pTexNum6, ComboNum);
	if(CHARMGR->GetPlay_Status().m_RealCombo == 7)
		cUiBase->Render(pTexNum7, ComboNum);
	if(CHARMGR->GetPlay_Status().m_RealCombo == 8)
		cUiBase->Render(pTexNum8, ComboNum);
	if(CHARMGR->GetPlay_Status().m_RealCombo == 9)
		cUiBase->Render(pTexNum9, ComboNum);
	if(CHARMGR->GetPlay_Status().m_RealCombo == 10)
		cUiBase->Render(pTexNum10, ComboNum);
	if(CHARMGR->GetPlay_Status().m_RealCombo == 11)
		cUiBase->Render(pTexNum11, ComboNum);
	if(CHARMGR->GetPlay_Status().m_RealCombo == 12)
		cUiBase->Render(pTexNum12, ComboNum);
	if(CHARMGR->GetPlay_Status().m_RealCombo == 13)
		cUiBase->Render(pTexNum13, ComboNum);
	if(CHARMGR->GetPlay_Status().m_RealCombo == 14)
		cUiBase->Render(pTexNum14, ComboNum);
	if(CHARMGR->GetPlay_Status().m_RealCombo == 15)
		cUiBase->Render(pTexNum15, ComboNum);
	if(CHARMGR->GetPlay_Status().m_RealCombo == 16)
		cUiBase->Render(pTexNum16, ComboNum);
	if(CHARMGR->GetPlay_Status().m_RealCombo == 17)
		cUiBase->Render(pTexNum17, ComboNum);
	if(CHARMGR->GetPlay_Status().m_RealCombo == 18)
		cUiBase->Render(pTexNum18, ComboNum);
	if(CHARMGR->GetPlay_Status().m_RealCombo == 19)
		cUiBase->Render(pTexNum19, ComboNum);
	if(CHARMGR->GetPlay_Status().m_RealCombo == 20)
		cUiBase->Render(pTexNum20, ComboNum);


}

void	CGame::BuffInit(void)
{
	//스킬
	/*Skill1 = cUiBase->CreateVertex2D( 100,  180, 780, 860, 0.0f);*/

	//버프
	//1(흡혈);
	pTexVamp = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/Vamp.png\0", &pTexVamp);
	pTexSVamp = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/SVamp.png\0", &pTexSVamp);
	Buff1 = cUiBase->CreateVertex2D(1100, 1180, 700, 780, 0.0f);
	//2(스태미나무한)
	pTexStamina = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/Stamina.png\0", &pTexStamina);
	pTexSStamina = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/SStamina.png\0", &pTexSStamina);
	Buff2 = cUiBase->CreateVertex2D(1180, 1260, 780, 860, 0.0f);
	//3(공이속1.5배)
	pTexSpeed = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/Speed.png\0", &pTexSpeed);
	pTexSSpeed = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/SSpeed.png\0", &pTexSSpeed);
	Buff3 = cUiBase->CreateVertex2D(1100, 1180, 860, 940, 0.0f);
	//4(공2배)
	pTexDamage = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/Damage.png\0", &pTexDamage);
	pTexSDamage = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/SDamage.png\0", &pTexSDamage);
	Buff4 = cUiBase->CreateVertex2D(1020, 1100, 780, 860, 0.0f);

	//화살표
	Arrow = cUiBase->CreateVertex2D(1100, 1180, 780, 860, 0.0f);

	pTexArrow0 = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/Arrow0.png\0", &pTexArrow0);
	pTexArrow1 = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/Arrow1.png\0", &pTexArrow1);
	pTexArrow2 = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/Arrow2.png\0", &pTexArrow2);
	pTexArrow3 = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/Arrow3.png\0", &pTexArrow3);
	pTexArrow4 = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/Arrow4.png\0", &pTexArrow4);
	//

	//암전
	pTexDVamp = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/DVamp.png\0", &pTexDVamp);
	pTexDStamina = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/DStamina.png\0", &pTexDStamina);
	pTexDSpeed = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/DSpeed.png\0", &pTexDSpeed);
	pTexDDamage = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/DDamage.png\0", &pTexDDamage);
}

void	CGame::BuffRender()
{
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_ALPHABLENDENABLE, TRUE	);			//알파값 예제
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_SRCBLEND, D3DBLEND_SRCALPHA	);
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA	);
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );

	//버프
	//cUiBase->Render(pTexSkillSlot, Skill1);

	//기본
	cUiBase->Render(pTexVamp, Buff1);
	cUiBase->Render(pTexStamina, Buff2);
	cUiBase->Render(pTexSpeed, Buff3);
	cUiBase->Render(pTexDamage, Buff4);
	cUiBase->Render(pTexArrow0, Arrow);
	//

	//1번 버프 선택
	if((CHARMGR->GetBuffTimeCehck() == FALSE) && (CHARMGR->GetBuffSwith() == 1))	//쿨타임
	{
		cUiBase->Render(pTexSVamp, Buff1);
		cUiBase->Render(pTexDStamina, Buff2);
		cUiBase->Render(pTexDSpeed, Buff3);
		cUiBase->Render(pTexDDamage, Buff4);
		cUiBase->Render(pTexArrow1, Arrow);
	}
	else if((CHARMGR->GetBuffTimeCehck() == TRUE) && (CHARMGR->GetBuffSwith() == 1))	//쿨타임끝
	{
		cUiBase->Render(pTexSVamp, Buff1);
		cUiBase->Render(pTexStamina, Buff2);
		cUiBase->Render(pTexSpeed, Buff3);
		cUiBase->Render(pTexDamage, Buff4);
		cUiBase->Render(pTexArrow1, Arrow);
	}

	//2번 버프 선택
	if((CHARMGR->GetBuffTimeCehck() == FALSE) && (CHARMGR->GetBuffSwith() == 2))	//쿨타임
	{
		cUiBase->Render(pTexDVamp, Buff1);
		cUiBase->Render(pTexSStamina, Buff2);
		cUiBase->Render(pTexDSpeed, Buff3);
		cUiBase->Render(pTexDDamage, Buff4);
		cUiBase->Render(pTexArrow2, Arrow);
	}
	else if((CHARMGR->GetBuffTimeCehck() == TRUE) && (CHARMGR->GetBuffSwith() == 2))	//쿨타임끝
	{
		cUiBase->Render(pTexVamp, Buff1);
		cUiBase->Render(pTexSStamina, Buff2);
		cUiBase->Render(pTexSpeed, Buff3);
		cUiBase->Render(pTexDamage, Buff4);
		cUiBase->Render(pTexArrow2, Arrow);
	}
	//3번 버프 선택
	if((CHARMGR->GetBuffTimeCehck() == FALSE) && (CHARMGR->GetBuffSwith() == 3))	//쿨타임
	{
		cUiBase->Render(pTexDVamp, Buff1);
		cUiBase->Render(pTexDStamina, Buff2);
		cUiBase->Render(pTexSSpeed, Buff3);
		cUiBase->Render(pTexDDamage, Buff4);
		cUiBase->Render(pTexArrow3, Arrow);
	}
	else if((CHARMGR->GetBuffTimeCehck() == TRUE) && (CHARMGR->GetBuffSwith() == 3))	//쿨타임끝
	{
		cUiBase->Render(pTexVamp, Buff1);
		cUiBase->Render(pTexStamina, Buff2);
		cUiBase->Render(pTexSSpeed, Buff3);
		cUiBase->Render(pTexDamage, Buff4);
		cUiBase->Render(pTexArrow3, Arrow);
	}
	//4번 버프 선택
	if((CHARMGR->GetBuffTimeCehck() == FALSE) && (CHARMGR->GetBuffSwith() == 4))	//쿨타임
	{
		cUiBase->Render(pTexDVamp, Buff1);
		cUiBase->Render(pTexDStamina, Buff2);
		cUiBase->Render(pTexDSpeed, Buff3);
		cUiBase->Render(pTexSDamage, Buff4);
		cUiBase->Render(pTexArrow4, Arrow);
	}
	else if((CHARMGR->GetBuffTimeCehck() == TRUE) && (CHARMGR->GetBuffSwith() == 4))	//쿨타임끝
	{
		cUiBase->Render(pTexVamp, Buff1);
		cUiBase->Render(pTexStamina, Buff2);
		cUiBase->Render(pTexSpeed, Buff3);
		cUiBase->Render(pTexSDamage, Buff4);
		cUiBase->Render(pTexArrow4, Arrow);
	}
}