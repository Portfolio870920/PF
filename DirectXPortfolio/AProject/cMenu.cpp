#include "MainHeader.h"

CMenu::CMenu()
{
}

CMenu::~CMenu(void)
{
}

unsigned int CMenu::Init(void)
{
	//��ī�̹ڽ�
	m_Mesh.LoadMesh("./xFile","skybox.x");
	m_Mesh.SetScale(500.0f,500.0f,500.0f);
	if( RSCMGR->m_bMOB_MGR )	// ���Ŵ����� �ѹ��̶� ����ƾ��ٸ� �Ҵ�������Ų��.
		MOBMGR->FreeInst();
	if( !RSCMGR->m_bMOB_MGR )	// ���Ŵ����� �ѹ��̶� ������� �ʾҴٸ� ���͸� �߰�
	{
		MOBMGR->AddMob(KNIGHT);	// ����
	}
	
	if ( !RSCMGR->m_bMAP_MGR )	// �ʸŴ����� �ѹ��� ������� �ʾҴٸ�
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

	//�ɼǵ��� ���� �޴��� ���ߴ� �뵵
	m_bIsStop	=	FALSE;

	//start���� �����ϰ� 1�� �ʱ�ȭ
	Select = 1;
	//Ŭ�� �ʱ�ȭ
	Click = 0;

	//�ؽ��������� �ʱ�ȭ �ϰ� �� �ȿ� �̹��� �ְ� ��ư���� ��ġ ���
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
	
	//�������
	SOUNDMGR->PlaySound(MAIN_SOUND);
	return 0UL;
}
unsigned int CMenu::Update(void)
{
	//�ɼ�Ű���ʾ�����
	if( !m_bIsStop )
	{
		MOBMGR->SetIterBengin();
		while ( MOBMGR->GetIter() != MOBMGR->IterEnd() )
		{
			(MOBMGR->MobVec(MOBMGR->GetIter()))->Update();
			MOBMGR->IterInc();
		}
		CAMMGR->Update();

		
		GetCursorPos(&ptMouse);	//Ŀ����ġ�ޱ�
		ScreenToClient(HWnd, &ptMouse);	//��ǥ���� â��������

		//Ű����� ����
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

		//���콺�� ����
		//	��ŸƮ
		if( ( ( Start[0].x < ptMouse.x && ptMouse.x < Start[1].x) && (Start[0].y < ptMouse.y && ptMouse.y < Start[2].y)
			&& ( KEYMGR->StayKeyDown(VK_LBUTTON) ) ) || ( Select == 1 && KEYMGR->StayKeyDown(VK_RETURN) ) )
		{
			Select = 1, Click = 1;
			SCENEMGR->ReservationChange(GAME);
		}
		//	�ɼ�
		if((Option[0].x < ptMouse.x && ptMouse.x < Option[1].x) && (Option[0].y < ptMouse.y && ptMouse.y < Option[2].y)
			&& KEYMGR->StayKeyDown(VK_LBUTTON))
		{
			Select = 2, Click = 2;
		}
		//	���� ����
		if( ( ( Exit[0].x < ptMouse.x && ptMouse.x < Exit[1].x) && (Exit[0].y < ptMouse.y && ptMouse.y < Exit[2].y)
			&& KEYMGR->StayKeyDown(VK_LBUTTON) ) || ( Select == 4 && KEYMGR->StayKeyDown(VK_RETURN) ) )
		{
			Select = 3, Click = 3;
			SCENEMGR->SetGameEnd(true);
		}
		//

		//�ɼǰ�������(Click == 2), ���� ������Ʈ�� ����
		if(Click == 2)
			m_bIsStop = TRUE;
		//
	}
	
	//�ɼǰ������ ����������Ʈ���߰� �ɼ� ������Ʈ
	if( m_bIsStop )
	{
		COPTION->Update();
	}
	
	D3DFRAME->GetDevice()->Clear ( 0, NULL,	D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET, 0x0000FF00, 1.0f, 0UL );
	return 0UL;
}
void CMenu::Render(void)
{
	//��ī�� �ڽ�
	m_Mesh.Render();
	//����
	MAPMGR->Render();
	//����
	MOBMGR->SetIterBengin();
	while ( MOBMGR->GetIter() != MOBMGR->IterEnd() )
	{
		(MOBMGR->MobVec(MOBMGR->GetIter()))->Render();
		MOBMGR->IterInc();
	}

	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_ALPHABLENDENABLE, TRUE	);			//���İ� ����
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_SRCBLEND, D3DBLEND_SRCALPHA	);
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA	);
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );

	//����
	cUiBase->Render(pTexTitle, Title);

	//�������� �Ͼ� �ؽ��ĸ� ����
	//���� �ʾ������� ���� �ؽ��ĸ� ����

	//��ŸƮ��ư
	if(Select == 1)
		cUiBase->Render(pTexRStart, Start);
	else
		cUiBase->Render(pTexStart, Start);

	//�ɼǹ�ư
	if(Select == 2)
		cUiBase->Render(pTexROption, Option);
	else
		cUiBase->Render(pTexOption, Option);

	//�����ư
	if(Select == 3)
		cUiBase->Render(pTexRExit, Exit);
	else
		cUiBase->Render(pTexExit, Exit);

	//��ŸƮ�� ������ ���(Click == 1), �ε��̹����� ����
	if(Click == 1)
		cUiBase->Render(pTexLoading, Loading);
	//

	//�ɼ��� ���������(Click == 2), �ɼ� �׸���
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