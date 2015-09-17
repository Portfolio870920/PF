#pragma		once

namespace Load
{
	LPDIRECT3DTEXTURE9		Texture( const char* filename );
}

class CMesh
{
	const char*				m_pPath;

	LPD3DXMESH				m_pMesh;
	D3DMATERIAL9*			m_pMtrl;
	LPDIRECT3DTEXTURE9*		m_pTex;
	DWORD					m_dwNumMtrl;

	D3DXMATRIX				m_mTrans, m_mRot, m_mScale;

public:
	int			LoadMesh		( const char* pPath, const char* xFileName );
	void		Destroy();
	void		Update();
	void		Render();

	inline void		SetTranslation		( float x, float y, float z	);
	inline void		SetRotation			( float x, float y, float z	);
	inline void		SetScale			( float x, float y, float z	);

public:
	static	CMesh*		pMesh;
	static	CMesh*		LoadModel( const char* pPath, const char* xFileName );
	CMesh()		{}
	~CMesh()	{}
};

inline void		CMesh::SetTranslation		( float x, float y, float z	)
{
	D3DXMatrixTranslation( &m_mTrans, x, y, z );
}

inline void		CMesh::SetRotation		( float x, float y, float z	)
{
	D3DXMATRIX		vRotX, vRotY, vRotZ;
	D3DXMatrixRotationX( &vRotX, x );
	D3DXMatrixRotationY( &vRotX, y );
	D3DXMatrixRotationZ( &vRotX, z );

	m_mRot = vRotX * vRotY * vRotZ;
}

inline void		CMesh::SetScale			( float x, float y, float z	)
{
	D3DXMatrixScaling( &m_mScale, x, y, z );
}