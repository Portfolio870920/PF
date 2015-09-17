// Flame.cpp: implementation of the CFlame class.
//
//////////////////////////////////////////////////////////////////////

#include "D3DWRAPPER.h"
#include "Flame.h"

LPD3DXEFFECT					CFlame::s_pEffect = NULL;
LPDIRECT3DVERTEXDECLARATION9	CFlame::s_pDecl = NULL;
LPDIRECT3DVERTEXBUFFER9			CFlame::s_pVBuffer = NULL;

struct SFLAMEVERTEX 
{
	D3DXVECTOR3	vPos;
	D3DXVECTOR2	vTex;
};

void	CFlame::FxInit	( LPSTR szFxName )
{
	LPD3DXBUFFER		pBuffer = NULL;
	D3DVERTEXELEMENT9	pElem[MAX_FVF_DECL_SIZE];
	D3DXDeclaratorFromFVF( D3DFVF_XYZ|D3DFVF_TEX1, pElem );
	D3DFRAME->GetDevice()->CreateVertexDeclaration ( pElem, &s_pDecl );
	D3DXCreateEffectFromFileA ( D3DFRAME->GetDevice(), szFxName, NULL, NULL, D3DXSHADER_SKIPVALIDATION, NULL, &s_pEffect, &pBuffer );
	if ( pBuffer )	OutputDebugString ( (LPSTR)(pBuffer->GetBufferPointer()) );

	SFLAMEVERTEX*	pData = NULL;
	D3DFRAME->GetDevice()->CreateVertexBuffer( 4*sizeof(SFLAMEVERTEX), 0, 0, D3DPOOL_DEFAULT, &s_pVBuffer, NULL );
	s_pVBuffer->Lock( 0, 0, (void**)&pData, D3DLOCK_DISCARD );
	pData[0].vPos = D3DXVECTOR3( 1.0f, 4.0f, 0.0f)*10.0f;
	pData[1].vPos = D3DXVECTOR3(-1.0f, 4.0f, 0.0f)*10.0f;
	pData[2].vPos = D3DXVECTOR3( 1.0f,-1.0f, 0.0f)*10.0f;
	pData[3].vPos = D3DXVECTOR3(-1.0f,-1.0f, 0.0f)*10.0f;
	pData[0].vTex = D3DXVECTOR2( 1.5f,-0.5f);
	pData[1].vTex = D3DXVECTOR2(-0.5f,-0.5f);
	pData[2].vTex = D3DXVECTOR2( 1.5f, 1.5f);
	pData[3].vTex = D3DXVECTOR2(-0.5f, 1.5f);
	s_pVBuffer->Unlock();
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFlame::CFlame()
{
	m_pTexDiffuse	= NULL;
	m_pTexNormal	= NULL;

	m_vPos			= D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vScl			= D3DXVECTOR3(1.0f, 1.0f, 1.0f);
}

CFlame::~CFlame()
{
	Free ();
}

bool			CFlame::Init	( LPSTR szDifName, LPSTR szNorName )
{
	D3DXCreateTextureFromFileA ( D3DFRAME->GetDevice(),  szDifName, &m_pTexDiffuse );
	D3DXCreateTextureFromFileA ( D3DFRAME->GetDevice(),  szNorName, &m_pTexNormal );
	return true;
}

void			CFlame::Free	( void )
{
	RELEASE_NODEL(m_pTexDiffuse);
	RELEASE_NODEL(m_pTexNormal);
}

void			CFlame::SetPos	( D3DXVECTOR3 vPos )
{
	m_vPos = vPos;
}

void			CFlame::SetScl	( D3DXVECTOR3 vPos )
{
	m_vScl = vPos;
}

void			CFlame::Draw	( void )
{
	unsigned int	uPass = 0;
	HRESULT			hResult = 0;
	D3DXMATRIX		ViewProj = D3DFRAME->m_mView * D3DFRAME->m_mProj;
	D3DXMATRIX		world, scl;
	float			timer = (float)(fTimeGetTime())*0.001f;
	D3DFRAME->GetDevice()->SetVertexDeclaration	( s_pDecl );
	D3DFRAME->GetDevice()->SetStreamSource		( 0, s_pVBuffer, 0, sizeof(SFLAMEVERTEX) );
	D3DXMatrixTranslation	( &world,	m_vPos.x, m_vPos.y, m_vPos.z );
	D3DXMatrixScaling		( &scl,		m_vScl.x, m_vScl.y, m_vScl.z );
	world	= scl * world;
	hResult = s_pEffect->SetTechnique	( "render" );
	hResult = s_pEffect->SetMatrix		( "world",&world );
	hResult = s_pEffect->SetMatrix		( "view", &D3DFRAME->m_mView );
	hResult = s_pEffect->SetMatrix		( "proj", &D3DFRAME->m_mProj );
	hResult = s_pEffect->SetMatrix		( "viewproj", &ViewProj );
	hResult = s_pEffect->SetFloat		( "fTimer", timer );

	hResult = s_pEffect->SetTexture		( "texD", m_pTexDiffuse );
	hResult = s_pEffect->SetTexture		( "texN", m_pTexNormal );
	hResult = D3DFRAME->GetDevice()->SetVertexDeclaration( s_pDecl );

	hResult = s_pEffect->Begin( &uPass, 0 );
	for ( unsigned int u = 0; u < uPass; u ++ )
	{
		hResult = s_pEffect->BeginPass( u );
		hResult = D3DFRAME->GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 0, 4, 0, 2 );
		hResult = s_pEffect->EndPass();
	}
	hResult = s_pEffect->End();
}

