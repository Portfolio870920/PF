#include "MainHeader.h"

CGame::CGame(void)
{
}

CGame::~CGame(void)
{

}


void	CGame::UIBAR( bool BeHit )	//HP�ٳ� ST���� ������
{
	PlayerHp	= CHARMGR->GetPlay_Status().m_HP/CHARMGR->GetPlay_Status().m_MaxHp; // ĳ���� hp ��
	PlayerHp	= __max(0.0f, PlayerHp);
	Bar			= ((993.0f - 306.0f) * PlayerHp + 306.0f);
	PlayerHPFront[1].x = Bar , PlayerHPFront[3].x = Bar;

	if( BeHit == true )
	{
 		MonsterHp	= (MOBMGR->MobVec(MOBMGR->GetIter()))->GetPlay_Status().m_HP/(MOBMGR->MobVec(MOBMGR->GetIter()))->GetPlay_Status().m_MaxHp;  // ���� hp ��
		MonsterHp	= __max(0.0f,MonsterHp);
		Bar			= (1213.0f	-	73.0f) * MonsterHp + 73.0f;
		BossHPFront[1].x = Bar	, BossHPFront[3].x	= Bar;
	}

	PlayerSt	= CHARMGR->GetPlay_Status().m_Stamina/CHARMGR->GetPlay_Status().m_MaxSt; // ĳ���� ���¹̳ʹ�
	PlayerSt	= __max(0.0f,PlayerSt);
	Bar			= ((994.0f - 306.0f ) * PlayerSt + 306.0f);
	PlayerSTFront[1].x	= Bar	,	PlayerSTFront[3].x	= Bar;
}

unsigned int	CGame::Init(void)
{
	srand((unsigned int) time(NULL));
	//��ī�̹ڽ�
	m_Mesh.LoadMesh("./xFile","skybox.x");
	m_Mesh.SetScale(1500.0f,2000.0f,1500.0f);

	//���콺 ���� ����
	m_bIsShowMS	=	TRUE;
	//������Ʈ ���� ���� üũ
	m_bIsStop	=	FALSE;
	//�ε�
	//DATAMGR->pPlayer = CHARMGR;
	//DATAMGR->Load();

	vNormal	=	D3DXVECTOR3(0.0f,0.0f,0.0f); // �ɸ��� ��ġ �ﰢ�� ����
	fNomalAngle	=	0.0f; // �ɸ��� ��ġ �ﰢ�� ���� dot y�� ��

	m_bSelectEnemy	=	false;	 // ������ �ʱ�ȭ
	m_bChoiceWp	=	false;		// ���⼱�� �ʱ�ȭ
	m_bSelectDifficulty = false; //���̵����� �ʱ�ȭ
	
	//����
	//MOBMGR->FreeInst();
	//MOBMGR->AddMob( ALDUIN );
	//MOBMGR->AddMob( ALDUIN, D3DXVECTOR3( -100.0f, 0.0f, -100.0f ), D3DXVECTOR3 ( 0.1f, 0.1f, 0.1f ), 2.7f);
	//MOBMGR->AddMob( KNIGHT );
	//MOBMGR->AddMob( KNIGHT, D3DXVECTOR3( 120.0f, 0.0f, 100.0f ), D3DXVECTOR3 ( 0.1f, 0.15f, 0.15f ), 2.7f );
	//MOBMGR->AddMob( KNIGHT, D3DXVECTOR3( 80.0f, 0.0f, 100.0f ), D3DXVECTOR3 ( 0.1f, 0.15f, 0.15f ), 2.7f );
	//MOBMGR->Init(KNIGHT);

	if ( !RSCMGR->m_bMAP_MGR )//�ʸŴ����� �ѹ��� ������� �ʾҴٸ�
	{
		//����
		CTerrain::LoadEffect ( "./SHADER/Map.fx" );
		MAPMGR->Init ( "./IMG/Height.bmp\0" );

		MAPMGR->CreateChannelTexture( "./IMG/Perturbate.tga\0" );
		MAPMGR->CreateTextureUsingA( "./IMG/alpha.tga\0" );
		MAPMGR->CreateTextureUsingR( "./IMG/red.tga\0" );
		MAPMGR->CreateTextureUsingG( "./IMG/green.tga\0" );
		MAPMGR->CreateTextureUsingB( "./IMG/blue.tga\0" );
		//
	}

	//HP�� �ؽ��Ļ����� ���ؽ�����
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

	//���¹̳�
	PlayerSTBack = cUiBase->CreateVertex2D(300, 1000, 870, 890, 0.0f);	//���¹̳�, HP�� ���� ��澸

	pTexPlayerSTF = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/PlayerStaminaBarF.png\0", &pTexPlayerSTF);
	pTexPlayerSTF2 = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/PlayerStaminaBarF2.png\0", &pTexPlayerSTF2);
	PlayerSTFront = cUiBase->CreateVertex2D(306, 994, 875, 885, 0.0f);


	//����
	BuffInit();
	//

	//���� ���� ȭ��
	Select	= 0;
	Click	= 0;
	pChoiceWeapon	=	NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(),"GameTex/SelectBG.png\0",&pChoiceWeapon);
	ChoiceWeapon	=	cUiBase->CreateVertex2D(0,1280,0,960,1.0f);

	pDualSword	=	NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/SelectDS.png\0", &pDualSword);

	pGreatSword	=	NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/SelectGS.png\0", &pGreatSword);

	//�� ���� ȭ��
	EnemySelect	= 0;
	EnemyClick	= 0;
	pTexSEnemyBG	=	NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/EnemySelectBG.png\0", &pTexSEnemyBG);
	SelectEnemy	=	cUiBase->CreateVertex2D(0, 1280, 0, 960, 1.0f);

	pTexDarkKnight	=	NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/SelectDK.png\0", &pTexDarkKnight);

	pTexBlackDragon	=	NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/SelectBD.png\0", &pTexBlackDragon);

	//���̵�����
	DSelect = 0;
	DClick = 0;

	//����
	pTexEasy = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Difficulty/Easy.png\0", &pTexEasy);
	pTexREasy = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Difficulty/REasy.png\0", &pTexREasy);
	Easy = cUiBase->CreateVertex2D(500, 780, 210, 330, 0.0f);

	//�븻
	pTexNormal = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Difficulty/Normal.png\0", &pTexNormal);
	pTexRNormal = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Difficulty/RNormal.png\0", &pTexRNormal);
	Normal = cUiBase->CreateVertex2D(500, 780, 420, 540, 0.0f);

	//�ϵ�
	pTexHard = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Difficulty/Hard.png\0", &pTexHard);
	pTexRHard = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Difficulty/RHard.png\0", &pTexRHard);
	Hard = cUiBase->CreateVertex2D(500, 780, 625, 745, 0.0f);


	//�޺�
	ComboInit();
	SOUNDMGR->PlaySound(BGM_B2);
	//���ӳ�
	//�÷��̾���
	pTexDead	=	NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Dead.png\0", &pTexDead);
	//�����
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
		//���̺� (�ϴ� �ǽð�)
		DATAMGR->Save();

		GetCursorPos(&CMouse);			// ���콺 ��ġ ����
		ScreenToClient(HWnd,&CMouse);	// ���� ����â�� ���� ���� �� 0,0���� �ν��ϰ� ��


		if(m_bSelectEnemy == false)	//�� �����ϱ�
		{
			//���콺�� ����
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
			//Ű����� ����
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
		//���̵� ����
		if((m_bSelectEnemy == true) && (m_bSelectDifficulty == false))
		{
			//���콺�� ����
			//����
			if(( 500 < CMouse.x && CMouse.x < 780) && (210 < CMouse.y && CMouse.y < 330))	
			{
				DSelect = 1;
				if(KEYMGR->OnceKeyDown(VK_LBUTTON))
				{
					DClick = 1;
					m_bSelectDifficulty = true;
				}	
			}
			//�븻
			else if((500 < CMouse.x && CMouse.x < 780) && (420 < CMouse.y && CMouse.y < 540))
			{
				DSelect = 2;
				if(KEYMGR->OnceKeyDown(VK_LBUTTON))
				{
					DClick = 2;
					m_bSelectDifficulty = true;
				}
			}
			//�ϵ�
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
			//Ű����� ����
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
				//���̵��� ���� �� ����
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
		//�������� ������ ���⼱�ý���
		if((m_bSelectEnemy == true) && (m_bChoiceWp	== false) &&  (DClick >= 1))
		{
			//���콺�� ����
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
			//Ű����� ����
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
				//���⼱�� �Ϸ�� ĳ���� �ʱ�ȭ
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

		//�����ð� ���⼱���� ���� �� ����Ǵ� ������Ʈ
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
				UIBAR( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetBeHit() );	//	HP ��
				MOBMGR->IterInc();
			}
			
			CHARMGR->Update();
			CAMMGR->Update();
			//���콺 ��ġ �ʱ�ȭ
			SetCursorPos(CENTERPOS_X,CENTERPOS_Y);
			//���콺Ŀ�� �����
			//LoadCursor(NULL,NULL);
		}
	}
	// esc�� �ѹ��� �ȴ����ٸ� ���� ������ �ȵ��� �ٽ� ������ ����
	if ( KEYMGR->OnceKeyDown(VK_ESCAPE) && ( m_bSelectEnemy == true) && (m_bChoiceWp == true))	
	{
		//����
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
	//�ɼ������� ���Ӿ�����Ʈ ���߰� �ɼǾ�����Ʈ
	if ( m_bIsStop )
	{
		COPTION->Update();
	}

	return 0UL;
}
void	CGame::Render(void)
{
	D3DFRAME->GetDevice()->Clear ( 0, NULL,	D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET, 0x00000000, 1.0f, 0UL );

	//������ �ؽ��ķ�����
	if( m_bSelectEnemy == false)
	{
		cUiBase->Render(pTexSEnemyBG,SelectEnemy);

		if(EnemySelect == 1) cUiBase->Render(pTexDarkKnight, SelectEnemy);		//������� �ؽ��ĺ�ȭ
		if(EnemySelect == 2) cUiBase->Render(pTexBlackDragon, SelectEnemy);
	}
	//
	if((m_bSelectEnemy == true) && (m_bSelectDifficulty == false))
	{
		//���̵����� �ؽ��ķ�����
		//����
		if(DSelect == 1) cUiBase->Render(pTexREasy, Easy);	
		else cUiBase->Render(pTexEasy, Easy);	//������� �ؽ��ĺ�ȭ
		//�븻
		if(DSelect == 2) cUiBase->Render(pTexRNormal, Normal);
		else cUiBase->Render(pTexNormal, Normal);
		//�ϵ�
		if(DSelect == 3) cUiBase->Render(pTexRHard, Hard);
		else cUiBase->Render(pTexHard, Hard);
	}

	//������� �ؽ��ķ�����
	if((m_bSelectEnemy == true) && (m_bChoiceWp	== false) &&  (DClick >= 1))
	{
		cUiBase->Render(pChoiceWeapon,ChoiceWeapon);

		if(Select == 1) cUiBase->Render(pDualSword, ChoiceWeapon);		//������� �ؽ��ĺ�ȭ
		if(Select == 2) cUiBase->Render(pGreatSword, ChoiceWeapon);
	}
	//

	//���� ���� ���⵵ ������� ������ ������
	if(( m_bSelectEnemy == true) && ( m_bChoiceWp  == true))
	{
		//��ī�� �ڽ�
		m_Mesh.Render();

		//�׸��� ����
		D3DFRAME->BeginShadow( CHARMGR->GetPos(), D3DXVECTOR3(0.5f,-1.0f, 0.0f) );
		CHARMGR->Render();
		MOBMGR->SetIterBengin();
		while ( MOBMGR->GetIter() != MOBMGR->IterEnd() )
		{
			(MOBMGR->MobVec(MOBMGR->GetIter()))->Render();
			MOBMGR->IterInc();
		}
		D3DFRAME->EndShadow();
		// �׸��� ����

		//ĳ����
		CHARMGR->Render();
		MOBMGR->SetIterBengin();
		while ( MOBMGR->GetIter() != MOBMGR->IterEnd() )
		{
			(MOBMGR->MobVec(MOBMGR->GetIter()))->Render();
			MOBMGR->IterInc();
		}

		//����
		MAPMGR->Render();

		//HP�� �׸���
		cUiBase->Render(pTexPlayerHPB, PlayerHPBack);	//�÷��̾�
		cUiBase->Render(pTexPlayerHPF, PlayerHPFront);

		cUiBase->Render(pTexBossHPB, BossHPBack);		//����
		cUiBase->Render(pTexBossHPF, BossHPFront);
		//

		//ST�� �׸���
		cUiBase->Render(pTexPlayerHPB, PlayerSTBack);
		if(CHARMGR->GetBuffStamina() == true)
		{
			cUiBase->Render(pTexPlayerSTF2, PlayerSTFront);
		}
		else
		{
			cUiBase->Render(pTexPlayerSTF, PlayerSTFront);
		}
		

		//����
		BuffRender();

		//�޺�
		if(CHARMGR->GetPlay_Status().m_RealCombo > 0)
		{
			ComboRender();
		}

		//�÷��̾���
		if(CHARMGR->GetPlay_Status().m_Status == STATUS_DIE /*|| MOBMGR->GetDieCount() == MOBMGR->GetVectorSize()// �� �� ������� �˷��� */ )
		{
			D3DFRAME->GetDevice()->SetRenderState(	D3DRS_ALPHABLENDENABLE, TRUE	);			//���İ� ����
			D3DFRAME->GetDevice()->SetRenderState(	D3DRS_SRCBLEND, D3DBLEND_SRCALPHA	);
			D3DFRAME->GetDevice()->SetRenderState(	D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA	);
			D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
			D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
			D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );		
			cUiBase->Render(pTexDead, End);
		}
		if(MOBMGR->GetDieCount() == MOBMGR->GetVectorSize())
		{
			D3DFRAME->GetDevice()->SetRenderState(	D3DRS_ALPHABLENDENABLE, TRUE	);			//���İ� ����
			D3DFRAME->GetDevice()->SetRenderState(	D3DRS_SRCBLEND, D3DBLEND_SRCALPHA	);
			D3DFRAME->GetDevice()->SetRenderState(	D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA	);
			D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
			D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
			D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );		
			cUiBase->Render(pTexDefeat, End);
		}
	}

	//�ɼ������� ���ӷ������߰� �ɼǷ�����
	if(m_bIsStop)
	{
		COPTION->Render();
	}
	//

	// fps ���
	TCHAR	str_Text[256];
	ZeroMemory(str_Text,sizeof(str_Text));
	sprintf(str_Text,"fps : %d",TIMEMGR->GetFrameRate());
	TEXTMGR->Print(str_Text);
	//

	// ���콺 ��ǥ ���
	/*sprintf(str_Text,"mouse_x : %d, mouse_y : %d",CMouse.x, CMouse.y);
	TEXTMGR->Print(str_Text, 105);*/
}
void	CGame::Free(void)
{
	//�ʱ�ȭ�Ҷ� ���ĺ��� ��
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
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_ALPHABLENDENABLE, TRUE	);			//���İ� ����
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_SRCBLEND, D3DBLEND_SRCALPHA	);
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA	);
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );


	if(CHARMGR->GetPlay_Status().m_CheckSkill == 1)		//�ڿ� �򸮴� �� ���� ����
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
	//��ų
	/*Skill1 = cUiBase->CreateVertex2D( 100,  180, 780, 860, 0.0f);*/

	//����
	//1(����);
	pTexVamp = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/Vamp.png\0", &pTexVamp);
	pTexSVamp = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/SVamp.png\0", &pTexSVamp);
	Buff1 = cUiBase->CreateVertex2D(1100, 1180, 700, 780, 0.0f);
	//2(���¹̳�����)
	pTexStamina = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/Stamina.png\0", &pTexStamina);
	pTexSStamina = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/SStamina.png\0", &pTexSStamina);
	Buff2 = cUiBase->CreateVertex2D(1180, 1260, 780, 860, 0.0f);
	//3(���̼�1.5��)
	pTexSpeed = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/Speed.png\0", &pTexSpeed);
	pTexSSpeed = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/SSpeed.png\0", &pTexSSpeed);
	Buff3 = cUiBase->CreateVertex2D(1100, 1180, 860, 940, 0.0f);
	//4(��2��)
	pTexDamage = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/Damage.png\0", &pTexDamage);
	pTexSDamage = NULL;
	D3DXCreateTextureFromFileA(D3DFRAME->GetDevice(), "GameTex/Buff/SDamage.png\0", &pTexSDamage);
	Buff4 = cUiBase->CreateVertex2D(1020, 1100, 780, 860, 0.0f);

	//ȭ��ǥ
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

	//����
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
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_ALPHABLENDENABLE, TRUE	);			//���İ� ����
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_SRCBLEND, D3DBLEND_SRCALPHA	);
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA	);
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );

	//����
	//cUiBase->Render(pTexSkillSlot, Skill1);

	//�⺻
	cUiBase->Render(pTexVamp, Buff1);
	cUiBase->Render(pTexStamina, Buff2);
	cUiBase->Render(pTexSpeed, Buff3);
	cUiBase->Render(pTexDamage, Buff4);
	cUiBase->Render(pTexArrow0, Arrow);
	//

	//1�� ���� ����
	if((CHARMGR->GetBuffTimeCehck() == FALSE) && (CHARMGR->GetBuffSwith() == 1))	//��Ÿ��
	{
		cUiBase->Render(pTexSVamp, Buff1);
		cUiBase->Render(pTexDStamina, Buff2);
		cUiBase->Render(pTexDSpeed, Buff3);
		cUiBase->Render(pTexDDamage, Buff4);
		cUiBase->Render(pTexArrow1, Arrow);
	}
	else if((CHARMGR->GetBuffTimeCehck() == TRUE) && (CHARMGR->GetBuffSwith() == 1))	//��Ÿ�ӳ�
	{
		cUiBase->Render(pTexSVamp, Buff1);
		cUiBase->Render(pTexStamina, Buff2);
		cUiBase->Render(pTexSpeed, Buff3);
		cUiBase->Render(pTexDamage, Buff4);
		cUiBase->Render(pTexArrow1, Arrow);
	}

	//2�� ���� ����
	if((CHARMGR->GetBuffTimeCehck() == FALSE) && (CHARMGR->GetBuffSwith() == 2))	//��Ÿ��
	{
		cUiBase->Render(pTexDVamp, Buff1);
		cUiBase->Render(pTexSStamina, Buff2);
		cUiBase->Render(pTexDSpeed, Buff3);
		cUiBase->Render(pTexDDamage, Buff4);
		cUiBase->Render(pTexArrow2, Arrow);
	}
	else if((CHARMGR->GetBuffTimeCehck() == TRUE) && (CHARMGR->GetBuffSwith() == 2))	//��Ÿ�ӳ�
	{
		cUiBase->Render(pTexVamp, Buff1);
		cUiBase->Render(pTexSStamina, Buff2);
		cUiBase->Render(pTexSpeed, Buff3);
		cUiBase->Render(pTexDamage, Buff4);
		cUiBase->Render(pTexArrow2, Arrow);
	}
	//3�� ���� ����
	if((CHARMGR->GetBuffTimeCehck() == FALSE) && (CHARMGR->GetBuffSwith() == 3))	//��Ÿ��
	{
		cUiBase->Render(pTexDVamp, Buff1);
		cUiBase->Render(pTexDStamina, Buff2);
		cUiBase->Render(pTexSSpeed, Buff3);
		cUiBase->Render(pTexDDamage, Buff4);
		cUiBase->Render(pTexArrow3, Arrow);
	}
	else if((CHARMGR->GetBuffTimeCehck() == TRUE) && (CHARMGR->GetBuffSwith() == 3))	//��Ÿ�ӳ�
	{
		cUiBase->Render(pTexVamp, Buff1);
		cUiBase->Render(pTexStamina, Buff2);
		cUiBase->Render(pTexSSpeed, Buff3);
		cUiBase->Render(pTexDamage, Buff4);
		cUiBase->Render(pTexArrow3, Arrow);
	}
	//4�� ���� ����
	if((CHARMGR->GetBuffTimeCehck() == FALSE) && (CHARMGR->GetBuffSwith() == 4))	//��Ÿ��
	{
		cUiBase->Render(pTexDVamp, Buff1);
		cUiBase->Render(pTexDStamina, Buff2);
		cUiBase->Render(pTexDSpeed, Buff3);
		cUiBase->Render(pTexSDamage, Buff4);
		cUiBase->Render(pTexArrow4, Arrow);
	}
	else if((CHARMGR->GetBuffTimeCehck() == TRUE) && (CHARMGR->GetBuffSwith() == 4))	//��Ÿ�ӳ�
	{
		cUiBase->Render(pTexVamp, Buff1);
		cUiBase->Render(pTexStamina, Buff2);
		cUiBase->Render(pTexSpeed, Buff3);
		cUiBase->Render(pTexSDamage, Buff4);
		cUiBase->Render(pTexArrow4, Arrow);
	}
}