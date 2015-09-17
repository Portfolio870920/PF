#include "MainHeader.h"

CIntro::CIntro(void)
{
}

CIntro::~CIntro(void)
{
}

unsigned int CIntro::Init(void)
{
	//��Ʈ�α���
	pTex4thTeam	= NULL;
	D3DXCreateTextureFromFileA( D3DFRAME->GetDevice(), "IntroTex/4thTeam.png\0", &pTex4thTeam);
	T4thTeam = cUiBase->CreateVertex2D(0, 1280, 0, 960, 0.0f);

	return 0;
}
unsigned int CIntro::Update(void)
{
	//��������
	D3DFRAME->GetDevice()->Clear ( 0, NULL,	D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET, 0x00000000, 1.0f, 0UL );
	return 0UL;
}
void CIntro::Render(void)
{
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_ALPHABLENDENABLE, TRUE	);			//���İ� ����
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_SRCBLEND, D3DBLEND_SRCALPHA	);
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA	);
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );

	//��Ʈ�α���
	cUiBase->Render(pTex4thTeam,T4thTeam);

	SCENEMGR->ReservationChange(MENU);
}
void CIntro::Free(void)
{
	//������ ���ĺ��� ����
	D3DFRAME->GetDevice()->SetRenderState(	D3DRS_ALPHABLENDENABLE, FALSE	);
}