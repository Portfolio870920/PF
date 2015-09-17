// MeshRenderer.cpp: implementation of the CMeshRenderer class.
//
//////////////////////////////////////////////////////////////////////

#include "MeshRenderer.h"
#include "D3DWRAPPER.h"

CMeshLoader						CMeshRenderer::s_cMeshLoader;
LPD3DXEFFECT					CMeshRenderer::s_pEffect	= NULL;
LPDIRECT3DVERTEXDECLARATION9	CMeshRenderer::s_pVDecl		= NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
bool	CMeshRenderer::LoadEffect ( char* pFileName )
{
	LPD3DXBUFFER	pBuffer = NULL;
	D3DXCreateEffectFromFileA ( D3DFRAME->GetDevice(), pFileName, NULL, NULL, D3DXSHADER_SKIPVALIDATION, NULL, &s_pEffect, &pBuffer );
	if ( pBuffer )
	{
		OutputDebugStringA ( (LPSTR)pBuffer->GetBufferPointer() );
	}
	D3DVERTEXELEMENT9	decl[] =
	{
		{0, 0, D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	0},
		{0,12, D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,		0},
		{0,24, D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	0},
		D3DDECL_END()
	};
	D3DFRAME->GetDevice()->CreateVertexDeclaration( decl, &s_pVDecl );

	return true;
}


CMeshRenderer::CMeshRenderer()
{
}

CMeshRenderer::~CMeshRenderer()
{
}

bool	CMeshRenderer::Init		( char* pFileName )
{
	int		nLen = 0, i = 0;
	s_cMeshLoader.Initial( pFileName );
	nObjCount = s_cMeshLoader.GetObjectCount();
	nMatCount = s_cMeshLoader.GetTextureCount();

	m_vRot		= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vPos		= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vScl		= D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
	D3DXMatrixIdentity ( &m_mWorld );

	m_ppTB		= new LPDIRECT3DTEXTURE9		[nMatCount];
	m_ppVB		= new LPDIRECT3DVERTEXBUFFER9	[nObjCount];
	m_ppIB		= new LPDIRECT3DINDEXBUFFER9	[nObjCount];
	m_pnMatID	= new int						[nObjCount];
	m_pnVNum	= new int						[nObjCount];
	m_pnINum	= new int						[nObjCount];
	m_pmMatrix	= new D3DXMATRIX				[nObjCount];
	m_pvPivot	= new D3DXVECTOR3				[nObjCount];
	m_pvRot		= new D3DXVECTOR3				[nObjCount];
	m_pvPos		= new D3DXVECTOR3				[nObjCount];
	m_pvScl		= new D3DXVECTOR3				[nObjCount];

	for ( i = 0; i < nMatCount; i ++ )
	{
		unsigned char*	pData = s_cMeshLoader.GetTextureSource( i, &nLen );
		D3DXCreateTextureFromFileInMemory( D3DFRAME->GetDevice(), pData, nLen, &m_ppTB[i] );
	}

	for ( i = 0; i < nObjCount; i ++ )
	{
		POBJECT	pObj = s_cMeshLoader.GetObjectSource(i);
		m_pnVNum[i] = pObj->nVCount;
		m_pnINum[i] = pObj->nICount;
		void*	pData = NULL;
		D3DFRAME->GetDevice()->CreateVertexBuffer	( m_pnVNum[i] * sizeof(SCUSTOMVERTEX),	0, 0, D3DPOOL_DEFAULT, &m_ppVB[i], NULL );
		D3DFRAME->GetDevice()->CreateIndexBuffer	( m_pnINum[i] * sizeof(unsigned short),	D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_ppIB[i], NULL );
		m_ppVB[i]->Lock( 0, 0, &pData, D3DLOCK_DISCARD );
		memcpy ( pData, pObj->pVertices, sizeof(SCUSTOMVERTEX)*m_pnVNum[i]);
		m_ppVB[i]->Unlock();
		m_ppIB[i]->Lock( 0, 0, &pData, D3DLOCK_DISCARD );
		memcpy ( pData, pObj->pIndices, sizeof(unsigned short)*m_pnINum[i]);
		m_ppIB[i]->Unlock();
		m_pnMatID[i] = pObj->uTexIndex;
		D3DXMatrixIdentity ( &m_pmMatrix[i] );
		m_pvPivot[i]	= D3DXVECTOR3( pObj->pivotX, pObj->pivotY, pObj->pivotZ);
		m_pvRot[i]		= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		m_pvPos[i]		= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		m_pvScl[i]		= D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
	}

	s_cMeshLoader.Release();
	return true;
}

void	CMeshRenderer::Free		( void )
{
	int	i = 0;
	for ( i = 0; i < nMatCount; i ++ )
	{
		m_ppTB[i]->Release();
		m_ppTB[i] = NULL;
	}
	for ( i = 0; i < nObjCount; i ++ )
	{
		m_ppVB[i]->Release();
		m_ppIB[i]->Release();
		m_ppVB[i] = NULL;
		m_ppIB[i] = NULL;
	}
	DELETE_ARRAY( m_ppTB );
	DELETE_ARRAY( m_ppVB );
	DELETE_ARRAY( m_ppIB );
	DELETE_ARRAY( m_pnMatID );
	DELETE_ARRAY( m_pnVNum );
	DELETE_ARRAY( m_pnINum );
	DELETE_ARRAY( m_pmMatrix );
	DELETE_ARRAY( m_pvPivot );
	DELETE_ARRAY( m_pvPos );
	DELETE_ARRAY( m_pvRot );
	DELETE_ARRAY( m_pvScl );
}

void	CMeshRenderer::Render		( void )
{
	unsigned int	uPass = 0;
	HRESULT			hResult = 0;
	D3DXMATRIX		ViewProj = D3DFRAME->m_mView * D3DFRAME->m_mProj;
	hResult = s_pEffect->SetTechnique	( "render" );
	hResult = s_pEffect->SetMatrix		( "view", &D3DFRAME->m_mView );
	hResult = s_pEffect->SetMatrix		( "proj", &D3DFRAME->m_mProj );
	hResult = s_pEffect->SetMatrix		( "ViewProj", &ViewProj );
	hResult = s_pEffect->SetFloatArray	( "lightDir", D3DFRAME->GetLightDir(), 3 );
	hResult = D3DFRAME->GetDevice()->SetVertexDeclaration( s_pVDecl );

	D3DXMATRIX		mRootPos;
	D3DXMATRIX		mRootRot;
	D3DXMATRIX		mRootScl;
	D3DXMATRIX		mRootWorld;
	D3DXMatrixTranslation						( &mRootPos, m_vPos.x, m_vPos.y, m_vPos.z );
	D3DXMatrixScaling							( &mRootScl, m_vScl.x, m_vScl.y, m_vScl.z );
	mRootRot	= D3DFRAME->MatrixFromVector	( m_vRot );

	mRootWorld = mRootScl * mRootRot * mRootPos;
	for ( int i = 0; i < nObjCount; i ++ )
	{
		D3DXMATRIX		mNPos;
		D3DXMATRIX		mPPos;
		D3DXMATRIX		mRot;
		D3DXMATRIX		mPos;
		D3DXMATRIX		mScl;
		D3DXMatrixTranslation						( &mPos, m_pvPos[i].x, m_pvPos[i].y, m_pvPos[i].z );
		D3DXMatrixTranslation						( &mPPos, m_pvPivot[i].x, m_pvPivot[i].y, m_pvPivot[i].z );
		D3DXMatrixTranslation						( &mNPos, -m_pvPivot[i].x, -m_pvPivot[i].y, -m_pvPivot[i].z );
		D3DXMatrixScaling							( &mScl, m_pvScl[i].x, m_pvScl[i].y, m_pvScl[i].z );
		mRot		= D3DFRAME->MatrixFromVector	( m_pvRot[i] );

		m_pmMatrix[i] = mNPos * mScl * mRot * mPos * mPPos;

		D3DXMATRIX	mWorld = m_pmMatrix[i] * mRootWorld;
		hResult = s_pEffect->SetMatrix		( "world",&mWorld );
		hResult = D3DFRAME->GetDevice()->SetStreamSource	( 0, m_ppVB[i], 0, sizeof(SCUSTOMVERTEX) );
		hResult = D3DFRAME->GetDevice()->SetIndices		( m_ppIB[i] );
		hResult = s_pEffect->SetTexture					( "texD", m_ppTB[m_pnMatID[i]] );
		hResult = s_pEffect->Begin( &uPass, 0 );
		for ( unsigned int u = 0; u < uPass; u ++ )
		{
			hResult = s_pEffect->BeginPass( u );
			hResult = D3DFRAME->GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_pnVNum[i], 0, m_pnINum[i]/3 );
			hResult = s_pEffect->EndPass();
		}
		hResult = s_pEffect->End();
	}
}

void	CMeshRenderer::SetPos		( D3DXVECTOR3 vPos, int n )
{
	if ( n < 0 )	m_vPos = vPos;
	else			m_pvPos[n%nObjCount] = vPos;
}

void	CMeshRenderer::SetRot		( D3DXVECTOR3 vRot, int n )
{
	if ( n < 0 )	m_vRot = vRot;
	else			m_pvRot[n%nObjCount] = vRot;
}

void	CMeshRenderer::SetScl		( D3DXVECTOR3 vScl, int n )
{
	if ( n < 0 )	m_vScl = vScl;
	else			m_pvScl[n%nObjCount] = vScl;
}
