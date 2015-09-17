// D3DWRAPPER.cpp: implementation of the D3DWRAPPER class.
//
//////////////////////////////////////////////////////////////////////

#include "D3DWRAPPER.h"
#pragma comment ( lib, "D3Dx9.lib")
#pragma comment ( lib, "D3D9.lib")
#ifdef	_DEBUG
#pragma comment ( lib, "../MeshLoader/MeshLoader.lib")
#pragma comment ( lib, "../SkinLoader/SkinLoader.lib")
#else
#pragma comment ( lib, "../MeshLoader/release/MeshLoader.lib")
#pragma comment ( lib, "../SkinLoader/release/SkinLoader.lib")
#endif
// D3DWRAPPER.cpp: implementation of the D3DWRAPPER class.
//

#define		SHADOW_SIZE	(1024.0f)
struct	SQUADVERTEX
{
	float	x, y, z;
	float	u, v;
};

double	fTimeGetTime	( void )
{
	LARGE_INTEGER	Count, Freq;
	QueryPerformanceCounter		( &Count );
	QueryPerformanceFrequency	( &Freq );
	double	fFreq	= (double)(Freq.QuadPart)/1000.0;
	return (double)(Count.QuadPart)/fFreq;
}

D3DWRAPPER*	D3DWRAPPER::m_pInstance = NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

D3DWRAPPER::D3DWRAPPER()
{
}

D3DWRAPPER::~D3DWRAPPER()
{
}

D3DWRAPPER* D3DWRAPPER::GetInstance()
{
	if ( !m_pInstance )	m_pInstance = new D3DWRAPPER;
	return m_pInstance;
}

D3DPRESENT_PARAMETERS	DeviceSettings		( HWND hwnd, int Width, int Height, D3DFORMAT FMT_BPP, D3DFORMAT FMT_ZB )
{
	D3DPRESENT_PARAMETERS sParams;

	ZeroMemory ( &sParams, sizeof (D3DPRESENT_PARAMETERS) );
	sParams.AutoDepthStencilFormat = FMT_ZB;
	sParams.BackBufferCount = 1;
	sParams.BackBufferFormat = FMT_BPP;
	sParams.BackBufferHeight = Height;
	sParams.BackBufferWidth = Width;
	sParams.EnableAutoDepthStencil = TRUE;
	sParams.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	sParams.Windowed = TRUE;
	sParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	sParams.FullScreen_RefreshRateInHz = 0;
	sParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	sParams.MultiSampleQuality	= D3DMULTISAMPLE_NONE;

	return sParams;
}

bool	D3DWRAPPER::Init	( LPDIRECT3DDEVICE9 pD3Device )
{
	m_pD3DDevice = pD3Device;
	m_bExternal	= true;

	D3DVERTEXELEMENT9	decl[] =
	{
		{0, 0, D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	0},
		{0,12, D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	0},
		D3DDECL_END()
	};
	GetDevice()->CreateVertexDeclaration( decl, &m_pQuadDecl );

	SQUADVERTEX*	pData = NULL;
	m_pD3DDevice->CreateVertexBuffer ( sizeof(SQUADVERTEX)*4, 0, 0, D3DPOOL_DEFAULT, &m_pQuadVertex, NULL );
	m_pQuadVertex->Lock ( 0, 0, (void**)&pData, D3DLOCK_DISCARD );
	pData[0].x = 0.0f;	pData[0].y = 1.0f;	pData[0].z = 0.0f;	pData[0].u = 0.0f;	pData[0].v = 0.0f;
	pData[1].x = 1.0f;	pData[1].y = 1.0f;	pData[1].z = 0.0f;	pData[1].u = 1.0f;	pData[1].v = 0.0f;
	pData[2].x = 1.0f;	pData[2].y = 0.0f;	pData[2].z = 0.0f;	pData[2].u = 1.0f;	pData[2].v = 1.0f;
	pData[3].x = 0.0f;	pData[3].y = 0.0f;	pData[3].z = 0.0f;	pData[3].u = 0.0f;	pData[3].v = 1.0f;
	m_pQuadVertex->Unlock ();

	LPD3DXBUFFER	pErr = NULL;
	D3DXCreateEffectFromFileA ( GetDevice(), "SHADER\\Board2D.fx\0", NULL, NULL, D3DXSHADER_SKIPVALIDATION, NULL, &m_pEffect, &pErr );
	if ( pErr )
	{
		OutputDebugString ( (LPSTR)pErr->GetBufferPointer() );
	}
	m_hPos	=	m_pEffect->GetParameterBySemantic	( NULL, "Position\0" );
	m_hScl	=	m_pEffect->GetParameterBySemantic	( NULL, "Scale\0" );
	m_hTRect=	m_pEffect->GetParameterBySemantic	( NULL, "TexRect\0" );
	m_hTex	=	m_pEffect->GetParameterBySemantic	( NULL, "TEXIMAGE0\0" );
	m_hTech	=	m_pEffect->GetTechniqueByName		( "Render" );

	D3DXCreateTexture( GetDevice(), SHADOW_SIZE, SHADOW_SIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_P8, D3DPOOL_DEFAULT, &m_pShadowTex );
	D3DXCreateTexture( GetDevice(), SHADOW_SIZE, SHADOW_SIZE, 1, D3DUSAGE_DEPTHSTENCIL, D3DFMT_D24X8, D3DPOOL_DEFAULT, &m_pShadowDep );
	m_bShadow	=	false;

	return true;
}

void	D3DWRAPPER::BeginShadow( D3DXVECTOR3 vLightPos, D3DXVECTOR3 vLightDir )
{
	HRESULT	hResult = 0UL;
	m_bShadow	=	true;
	LPDIRECT3DSURFACE9	pSurfaceRT	=	NULL;
	LPDIRECT3DSURFACE9	pSurfaceDP	=	NULL;

	hResult = m_pShadowTex->GetSurfaceLevel( 0, &pSurfaceRT );
	hResult = m_pShadowDep->GetSurfaceLevel( 0, &pSurfaceDP );

	hResult = GetDevice()->GetRenderTarget( 0, &m_pPrevRT );
	hResult = GetDevice()->GetDepthStencilSurface( &m_pPrevDp );

	hResult = GetDevice()->SetRenderTarget( 0, pSurfaceRT );
	hResult = GetDevice()->SetDepthStencilSurface( pSurfaceDP );

	GetDevice()->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0 );

	pSurfaceRT->Release();
	pSurfaceDP->Release();

	D3DXMatrixLookAtLH( &m_mLView, 
						&(vLightPos - vLightDir * 800.0f),
						&vLightPos,
						&D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
	// 태양처럼 광원을 | | 평행하게 줌 멀리 있든 가까이 있든 같은 크기로 그린다.
	D3DXMatrixOrthoLH( &m_mLProj, 256, 256, 1.0f, 15000.0f );

	// 바이스 행렬 을 곱하여 uv좌표 값을 -1 ~ 1 까지의 값을 0 ~ 1 까지의 값으로 변경한다.
	m_mLBias	= D3DXMATRIX(	0.5f,					 0.0f,		 			  0.0f, 0.0f,
								0.0f,					-0.5f,					  0.0f, 0.0f,
								0.0f,					 0.0f,					  1.0f, 0.0f,
								0.5f+(0.5f/SHADOW_SIZE), 0.5f+(0.5f/SHADOW_SIZE), 0.0f, 1.0f);

	mTemp1	=	m_mView; // 기존 view proj을 백업 후 교체
	mTemp2	=	m_mProj;
	m_mView =	m_mLView;
	m_mProj =	m_mLProj;
}

void	D3DWRAPPER::EndShadow( void )
{
	m_bShadow	=	false;
	GetDevice()->SetRenderTarget( 0, m_pPrevRT );
	GetDevice()->SetDepthStencilSurface( m_pPrevDp );
	D3DFRAME->m_mView	=	mTemp1;
	D3DFRAME->m_mProj	=	mTemp2;
}

bool	D3DWRAPPER::Init	( HWND hwnd, int width, int height, D3DFORMAT fmt, bool bWindowed )
{
	m_bExternal	= false;
	D3DPRESENT_PARAMETERS	sParams;
	int						Adapter = 0;

	m_pDirect3D = Direct3DCreate9 (D3D_SDK_VERSION);

	sParams = DeviceSettings ( hwnd, width, height, D3DFMT_X8R8G8B8, D3DFMT_D24X8 );
	m_pDirect3D->CreateDevice ( Adapter, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED, &sParams, &m_pD3DDevice );
	if ( !m_pD3DDevice )
	{
		sParams.Windowed = TRUE;
		sParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
		sParams.FullScreen_RefreshRateInHz = 0;
		sParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		m_pDirect3D->CreateDevice ( Adapter, D3DDEVTYPE_REF, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED, &sParams, &m_pD3DDevice );
	}

	m_pD3DDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState ( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState ( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState ( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState ( 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState ( 3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState ( 3, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState ( 4, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState ( 4, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState ( 5, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState ( 5, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState ( 6, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState ( 6, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState ( 7, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState ( 7, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, FALSE );
	m_pD3DDevice->SetRenderState ( D3DRS_ALPHATESTENABLE, TRUE );
	m_pD3DDevice->SetRenderState ( D3DRS_ALPHAREF, 1 );
	m_pD3DDevice->SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_GREATER );

	D3DVERTEXELEMENT9	decl[] =
	{
		{0, 0, D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	0},
		{0,12, D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	0},
		D3DDECL_END()
	};
	GetDevice()->CreateVertexDeclaration( decl, &m_pQuadDecl );

	SQUADVERTEX*	pData = NULL;
	m_pD3DDevice->CreateVertexBuffer ( sizeof(SQUADVERTEX)*4, 0, 0, D3DPOOL_DEFAULT, &m_pQuadVertex, NULL );
	m_pQuadVertex->Lock ( 0, 0, (void**)&pData, D3DLOCK_DISCARD );
	pData[0].x = 0.0f;	pData[0].y = 1.0f;	pData[0].z = 0.0f;	pData[0].u = 0.0f;	pData[0].v = 0.0f;
	pData[1].x = 1.0f;	pData[1].y = 1.0f;	pData[1].z = 0.0f;	pData[1].u = 1.0f;	pData[1].v = 0.0f;
	pData[2].x = 1.0f;	pData[2].y = 0.0f;	pData[2].z = 0.0f;	pData[2].u = 1.0f;	pData[2].v = 1.0f;
	pData[3].x = 0.0f;	pData[3].y = 0.0f;	pData[3].z = 0.0f;	pData[3].u = 0.0f;	pData[3].v = 1.0f;
	m_pQuadVertex->Unlock ();

	LPD3DXBUFFER	pErr = NULL;
	D3DXCreateEffectFromFileA ( GetDevice(), "./SHADER/Board2D.fx\0", NULL, NULL, D3DXSHADER_SKIPVALIDATION, NULL, &m_pEffect, &pErr );
	if ( pErr )
	{
		OutputDebugString ( (LPSTR)pErr->GetBufferPointer() );
	}
	m_hPos	=	m_pEffect->GetParameterBySemantic	( NULL, "Position\0" );
	m_hScl	=	m_pEffect->GetParameterBySemantic	( NULL, "Scale\0" );
	m_hTRect=	m_pEffect->GetParameterBySemantic	( NULL, "TexRect\0" );
	m_hTex	=	m_pEffect->GetParameterBySemantic	( NULL, "TEXIMAGE0\0" );
	m_hTech	=	m_pEffect->GetTechniqueByName		( "Render" );

	D3DXCreateTexture( GetDevice(), SHADOW_SIZE, SHADOW_SIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_P8, D3DPOOL_DEFAULT, &m_pShadowTex );
	D3DXCreateTexture( GetDevice(), SHADOW_SIZE, SHADOW_SIZE, 1, D3DUSAGE_DEPTHSTENCIL, D3DFMT_D24X8, D3DPOOL_DEFAULT, &m_pShadowDep );
	m_bShadow	=	false;

	return true;
}

void	D3DWRAPPER::DrawQuad		( float	x, float y, float w, float h, LPD3DXVECTOR4 pRect, LPDIRECT3DTEXTURE9 pTex )
{
	D3DXVECTOR4	rect(0.0f, 0.0f, 1.0f, 1.0f);
	D3DXVECTOR4	pos(x, y, w, h);
	RECT		cRect;
	D3DDEVICE_CREATION_PARAMETERS	Param;
	GetDevice()->GetCreationParameters ( &Param );
	GetClientRect ( Param.hFocusWindow, &cRect );

	float		fw		= cRect.right - cRect.left;
	float		fh		= cRect.bottom - cRect.top;
	pos.x			= (pos.x / fw);
	pos.y			= 1.0f-((pos.y / fh));

	pos.z			= (pos.z / fw) * 2.0f;
	pos.w			= (pos.w / fh) *-2.0f;

	if ( pRect )	rect = *pRect;

	if ( !m_pEffect )	return;

	GetDevice()->SetStreamSource		( 0, m_pQuadVertex, 0, sizeof(SQUADVERTEX) );
	GetDevice()->SetVertexDeclaration	( m_pQuadDecl );

	m_pEffect->SetTechnique	( m_hTech );
	m_pEffect->SetTexture	( m_hTex, pTex );
	m_pEffect->SetFloatArray( m_hPos, &pos.x, 2 );
	m_pEffect->SetFloatArray( m_hScl, &pos.z, 2 );
	m_pEffect->SetFloatArray( m_hTRect, rect, 4 );

	unsigned int	uPass, i;
	m_pEffect->Begin ( &uPass, 0 );
	for ( i = 0; i < uPass; i ++ )
	{
		m_pEffect->BeginPass ( i );
		GetDevice()->DrawPrimitive ( D3DPT_TRIANGLEFAN, 0, 2 );
		m_pEffect->EndPass ( );
	}
	m_pEffect->End ();
}

void	D3DWRAPPER::Free				( void )
{
	if ( m_bExternal ) return;
	m_pD3DDevice->Release ();
	m_pDirect3D->Release ();

	m_pD3DDevice = NULL;
	m_pDirect3D = NULL;
}

D3DXMATRIX		D3DWRAPPER::SetCamera		( D3DXVECTOR3& vPos, D3DXVECTOR3& vTgt, D3DXVECTOR3& vUp )
{
	m_vCameraPos = vPos;
	D3DXMatrixLookAtLH( &m_mView, &vPos, &vTgt, &vUp );
	return m_mView;
}

D3DXMATRIX		D3DWRAPPER::SetPerspective	( float fFov, float fAspect, float fNear, float fFar )
{
	D3DXMatrixPerspectiveFovLH( &m_mProj, fFov, fAspect, fNear, fFar );
	m_pD3DDevice->SetTransform( D3DTS_PROJECTION, &m_mProj );
	return m_mProj;
}

D3DXMATRIX		D3DWRAPPER::SetOrthographic ()
{
	D3DXMatrixOrthoOffCenterLH( &m_mProj, -1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f );
	m_pD3DDevice->SetTransform( D3DTS_PROJECTION, &m_mProj );
	return m_mProj;
}

void			D3DWRAPPER::SetLightDir		( D3DXVECTOR3& vDir )
{
	m_vLightDir = vDir;
}

D3DXVECTOR3		D3DWRAPPER::GetLightDir		( void )
{
	return m_vLightDir;
}

D3DXVECTOR3		D3DWRAPPER::GetCameraPos		( void )
{
	return m_vCameraPos;
}

void			D3DWRAPPER::CheckFrame		( void )
{
	LARGE_INTEGER	ThisTime;
	QueryPerformanceCounter ( &ThisTime );

	m_dwInterval	= ThisTime.QuadPart - m_dwTime;
	m_dwTime		= ThisTime.QuadPart;
}

float			D3DWRAPPER::GetTimeDelta	( DWORD dwDefault )
{
	LARGE_INTEGER		Freq;
	QueryPerformanceFrequency ( &Freq );
	double		fInterval	= (double)(m_dwInterval);
	double		fFrequancy	= (double)(Freq.QuadPart);
	double		fDefault	= (1000.0/(double)(dwDefault));

	return (float)((fInterval/fFrequancy)*(fDefault));
}