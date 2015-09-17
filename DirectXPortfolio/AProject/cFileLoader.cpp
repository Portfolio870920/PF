#include "MainHeader.h"

LPDIRECT3DTEXTURE9		Load::Texture( const char* filename )
{
	LPDIRECT3DTEXTURE9		pTexture	=	NULL;

	if( FAILED( D3DXCreateTextureFromFile( D3DFRAME->GetDevice(), filename, &pTexture ) ) )
	{
		char	str[256];
		sprintf( str, "텍스처 로딩 실패: %s", filename );
		MessageBox( GetActiveWindow(), str, "Error", MB_OK );
		return NULL;
	}

	return pTexture;
}

CMesh*		CMesh::pMesh	=	NULL;
CMesh*		CMesh::LoadModel( const char* pPath, const char* xFileName )
{
	CMesh*		pMesh = new CMesh;
	pMesh->LoadMesh( pPath, xFileName );
	return pMesh;
}

int			CMesh::LoadMesh		( const char* pPath, const char* xFileName )
{
	HRESULT		hr;

	m_pPath = pPath;

	LPD3DXBUFFER		pMtrlBuffer;
	D3DXMATERIAL*		pMaterial;

	char file[256];

	sprintf( file, "%s/%s", m_pPath, xFileName );

	if(  FAILED( D3DXLoadMeshFromX( file, D3DXMESH_SYSTEMMEM, D3DFRAME->GetDevice(), NULL
						, &pMtrlBuffer, NULL, &m_dwNumMtrl, &m_pMesh ) ) )
	{
		char	str[256];
		sprintf( str, " X파일 로딩 실패 : %s", xFileName );
		MessageBox( GetActiveWindow(), str, "Error", MB_OK );
		exit(-1);
	}

	pMaterial	=	(D3DXMATERIAL*) pMtrlBuffer->GetBufferPointer();
	m_pMtrl		=	new D3DMATERIAL9[ m_dwNumMtrl ];
	m_pTex		=	new LPDIRECT3DTEXTURE9[ m_dwNumMtrl ];

	for( int i=0 ; i<m_dwNumMtrl ; ++i )
	{
		m_pMtrl[i] = pMaterial[i].MatD3D;
		m_pMtrl[i].Ambient = m_pMtrl[i].Diffuse;

		if( pMaterial[i].pTextureFilename != NULL && strlen( pMaterial[i].pTextureFilename ) > 0 )
		{
			char str[256];

			sprintf( str, "%s/%s", m_pPath, pMaterial[i].pTextureFilename );
			m_pTex[i] = Load::Texture( str );
		}
	}

	pMtrlBuffer->Release();

	D3DXMatrixIdentity( &m_mTrans );
	D3DXMatrixIdentity( &m_mRot );
	D3DXMatrixIdentity( &m_mScale );
	return 0;
}

void		CMesh::Destroy()
{
	if( m_pMtrl != NULL ) 
		delete[] m_pMtrl;
	
	if( m_pTex )
	{
		for( DWORD i = 0; i < m_dwNumMtrl; i++ )
		{
			if( m_pTex[i] )
				m_pTex[i]->Release();
		}
		
		delete[] m_pTex;
	}
	
	if( m_pMesh != NULL )
		m_pMesh->Release();
}

void		CMesh::Update()
{
}

void		CMesh::Render()
{
	//설정
	D3DXMATRIX		world = m_mRot * m_mScale * m_mTrans;
	D3DFRAME->GetDevice()->SetTransform( D3DTS_WORLD, &world );
	D3DFRAME->GetDevice()->SetTransform( D3DTS_VIEW,		&D3DFRAME->m_mView );
	//D3DFRAME->GetDevice()->SetTransform( D3DTS_PROJECTION,	&D3DFRAME->m_mProj );
	D3DFRAME->GetDevice()->SetRenderState ( D3DRS_LIGHTING, FALSE );
	
	// 텍스춰 U, V, W의 어드레스 모드를 Wrap으로 설정한다.
	D3DFRAME->GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	D3DFRAME->GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	D3DFRAME->GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);

	// 텍스춰의 필터링을 Linear로 설정한다.
	D3DFRAME->GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	D3DFRAME->GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	D3DFRAME->GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	
	D3DFRAME->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	D3DFRAME->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	D3DFRAME->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	D3DFRAME->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	D3DFRAME->GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	
	D3DFRAME->GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	D3DFRAME->GetDevice()->SetRenderState( D3DRS_ALPHAREF,        156 );
	D3DFRAME->GetDevice()->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER );

	//D3DFRAME->GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	//그리기
	for( DWORD i=0; i<m_dwNumMtrl; i++ )
	{
		// Set the material and texture for this subset
		D3DFRAME->GetDevice()->SetMaterial( &m_pMtrl[i] );
		D3DFRAME->GetDevice()->SetTexture( 0, m_pTex[i] );
		
		// Draw the mesh subset
		m_pMesh->DrawSubset( i );
	}
	//해제
	D3DFRAME->GetDevice()->SetRenderState ( D3DRS_LIGHTING, TRUE );
	D3DFRAME->GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE);
	D3DFRAME->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	D3DFRAME->GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	D3DFRAME->GetDevice()->SetTexture( 0, NULL);

	D3DXMatrixIdentity( &world );
	D3DFRAME->GetDevice()->SetTransform( D3DTS_WORLD, &world );
}