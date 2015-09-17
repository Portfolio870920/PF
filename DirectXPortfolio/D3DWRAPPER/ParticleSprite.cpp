// ParticleSprite.cpp: implementation of the CParticleSprite class.
//
//////////////////////////////////////////////////////////////////////

#include "ParticleSprite.h"

#define							INSTANCE_LENGTH	40
LPD3DXEFFECT					CParticleSprite::s_pEffect	= NULL;
LPDIRECT3DVERTEXBUFFER9			CParticleSprite::s_pImage	= NULL;
LPDIRECT3DINDEXBUFFER9			CParticleSprite::s_pIndex	= NULL;
LPDIRECT3DVERTEXDECLARATION9	CParticleSprite::s_pVDecl	= NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParticleSprite::CParticleSprite()
{
}

CParticleSprite::~CParticleSprite()
{
}

void	CParticleSprite::CreateEffect	( char* szFileName )
{
	LPD3DXBUFFER	pBuffer = NULL;
	D3DXCreateEffectFromFileA ( D3DFRAME->GetDevice(), szFileName, NULL, NULL, D3DXSHADER_SKIPVALIDATION, NULL, &s_pEffect, &pBuffer );
	if ( pBuffer )	OutputDebugStringA ( (LPSTR)pBuffer->GetBufferPointer() );

	D3DVERTEXELEMENT9	decl[] ={	{ 0, 0,		D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION	,0 },
									{ 0, 12,	D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD	,0 },
									{ 1, 0,		D3DDECLTYPE_FLOAT1,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD	,1 },
									D3DDECL_END() };
	D3DFRAME->GetDevice()->CreateVertexBuffer	( 4 * 20,	0, 0, D3DPOOL_DEFAULT, &s_pImage, NULL );
	D3DFRAME->GetDevice()->CreateIndexBuffer	( 6 * 2,	0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &s_pIndex, NULL );
	D3DFRAME->GetDevice()->CreateVertexDeclaration( decl, &s_pVDecl );
	float*	pData = NULL;
	WORD*	pIndex = NULL;
	s_pImage->Lock( 0, 0, (void**)&pData, D3DLOCK_DISCARD );
	pData[0]	=-1.0f;	pData[1]	= 1.0f;	pData[2]	= 0.0f;	pData[3]	= 0.0f;	pData[4]	= 0.0f;
	pData[5]	= 1.0f;	pData[6]	= 1.0f;	pData[7]	= 0.0f;	pData[8]	= 1.0f;	pData[9]	= 0.0f;
	pData[10]	= 1.0f;	pData[11]	=-1.0f;	pData[12]	= 0.0f;	pData[13]	= 1.0f;	pData[14]	= 1.0f;
	pData[15]	=-1.0f;	pData[16]	=-1.0f;	pData[17]	= 0.0f;	pData[18]	= 0.0f;	pData[19]	= 1.0f;
	s_pImage->Unlock();

	s_pIndex->Lock( 0, 0, (void**)&pIndex, D3DLOCK_DISCARD );
	pIndex[0] = 0;	pIndex[1] = 1;	pIndex[2] = 2;
	pIndex[3] = 0;	pIndex[4] = 2;	pIndex[5] = 3;
	s_pIndex->Unlock();
}

unsigned int	CParticleSprite::Init	( int nSeed, float fGravity )
{
	float*	pData = NULL;
	D3DFRAME->GetDevice()->CreateVertexBuffer	( nSeed*4,	0, 0, D3DPOOL_DEFAULT, &m_pInstance, NULL );
	m_pInstance->Lock( 0, 0, (void**)&pData, D3DLOCK_DISCARD );
	for ( int i = 0; i < nSeed; i++ )
	{
		pData[i] = (float)i;
	}
	m_pInstance->Unlock();

	m_pvSize	= new D3DXVECTOR2 [nSeed];
	m_pvSizeS	= new D3DXVECTOR2 [nSeed];
	m_pvSizeD	= new D3DXVECTOR2 [nSeed];

	ZeroMemory ( m_pvSize, sizeof(D3DXVECTOR2)*nSeed );
	ZeroMemory ( m_pvSizeS, sizeof(D3DXVECTOR2)*nSeed );
	ZeroMemory ( m_pvSizeD, sizeof(D3DXVECTOR2)*nSeed );
	return CParticle::Init( nSeed, fGravity );
}

void	CParticleSprite::Render			( LPDIRECT3DTEXTURE9 pTex )
{
	unsigned int	uPass = 0;
	HRESULT			hResult = 0;
	D3DXMATRIX		ViewProj = D3DFRAME->m_mView * D3DFRAME->m_mProj, iView;
	D3DXMatrixTranspose( &iView, &D3DFRAME->m_mView );
	hResult = s_pEffect->SetTechnique	( "render" );
	hResult = s_pEffect->SetMatrix		( "view", &D3DFRAME->m_mView );
	hResult = s_pEffect->SetMatrix		( "proj", &D3DFRAME->m_mProj );
	hResult = s_pEffect->SetMatrix		( "viewproj", &ViewProj );

	iView._14 = 0.0f;	iView._24 = 0.0f;	iView._34 = 0.0f;
	iView._41 = 0.0f;	iView._42 = 0.0f;	iView._43 = 0.0f;

	hResult = D3DFRAME->GetDevice()->SetVertexDeclaration( s_pVDecl );

	hResult = s_pEffect->SetMatrix						( "world",&iView );
	hResult = s_pEffect->SetTexture						( "texD", pTex );

	int nLoop = (m_nSeedCnt / INSTANCE_LENGTH)+1;
	int	nSeedCount = m_nSeedCnt;
	for ( int i = 0; i < nLoop; i ++ )
	{
		if ( nSeedCount < 1 )	continue;

		s_pEffect->SetFloatArray( "positions",	&m_pvPos[i*INSTANCE_LENGTH].x,		__min(INSTANCE_LENGTH, nSeedCount) * 3 );
		s_pEffect->SetFloatArray( "scales",	&m_pvSize[i*INSTANCE_LENGTH].x,		__min(INSTANCE_LENGTH, nSeedCount) * 2 );
		s_pEffect->SetFloatArray( "colors",	&m_pcCColor[i*INSTANCE_LENGTH].r,	__min(INSTANCE_LENGTH, nSeedCount) * 4 );
		hResult = D3DFRAME->GetDevice()->SetStreamSource	( 0, s_pImage,		0, 20 );
		hResult = D3DFRAME->GetDevice()->SetStreamSource	( 1, m_pInstance,	0, 4 );
		hResult = D3DFRAME->GetDevice()->SetIndices			( s_pIndex );
		D3DFRAME->GetDevice()->SetStreamSourceFreq ( 0, D3DSTREAMSOURCE_INDEXEDDATA|__min(INSTANCE_LENGTH, nSeedCount));
		D3DFRAME->GetDevice()->SetStreamSourceFreq ( 1, D3DSTREAMSOURCE_INSTANCEDATA|1UL);

		D3DFRAME->GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		hResult = s_pEffect->Begin( &uPass, 0 );
		for ( unsigned int u = 0; u < uPass; u ++ )
		{
			hResult = s_pEffect->BeginPass( u );
			hResult = D3DFRAME->GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2 );
			hResult = s_pEffect->EndPass();
		}
		hResult = s_pEffect->End();
		D3DFRAME->GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		D3DFRAME->GetDevice()->SetStreamSourceFreq ( 0, 1);
		D3DFRAME->GetDevice()->SetStreamSourceFreq ( 1, 1);
		nSeedCount -= INSTANCE_LENGTH;
	}
}

unsigned int	CParticleSprite::Update	( void )
{
	for ( int i = 0; i < m_nSeedCnt; i ++ )
	{
		if ( m_pwStart[i] == 0 )							continue;

		float	fAlpha = __min(1.0f, (float)(timeGetTime()-m_pwStart[i])/(float)(m_pwLife[i]));
		m_pvSize[i]	= m_pvSizeS[i] + (m_pvSizeD[i] - m_pvSizeS[i]) * fAlpha;
	}
	CParticle::Update();
	return 0UL;
}

int		CParticleSprite::Insert	( LPD3DXVECTOR3 pPos, LPD3DXVECTOR2 pSclS, LPD3DXVECTOR2 pSclD, LPD3DXVECTOR3 pVel, D3DXCOLOR sColor, D3DXCOLOR dColor, DWORD dwLife )
{
	m_pvSizeS[m_nCurSeed]		= *pSclS;
	m_pvSizeD[m_nCurSeed]		= *pSclD;
	return CParticle::Insert( pPos, pVel, sColor, dColor, dwLife );
}
