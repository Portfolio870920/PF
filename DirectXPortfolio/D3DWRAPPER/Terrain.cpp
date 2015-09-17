#include "Terrain.h"
#include "../AProject/MainHeader.h"

LPDIRECT3DVERTEXDECLARATION9	CTerrain::s_pVDecl		= NULL;	// 버텍스 선언 선언 자체를 비디오 메모리로 하겠다
LPD3DXEFFECT					CTerrain::s_pEffect		= NULL;

CTerrain*	CTerrain::Map_Inst	=	NULL;

CTerrain::CTerrain(void)
{
	m_pvBUFFER		= NULL;
	m_piBUFFER		= NULL;
	m_piBUFFERLen	= NULL;
	m_piCenterPos	= NULL;
	m_CellSize		= 64;
	m_CellCnt		= NULL;
	m_IndexCnt		= NULL;
	m_pPerturbation	= NULL;
	m_pChannel		= NULL;
	m_pChannelA		= NULL;
	m_pChannelR		= NULL;
	m_pChannelG		= NULL;
	m_pChannelB		= NULL;
	m_pNormalMap	= NULL;
}

CTerrain::~CTerrain(void)
{
}

void			CTerrain::LoadEffect	( const char* szFileName )
{
	D3DVERTEXELEMENT9		pElem[]		=	{ 
												{
													0, 0,					// 포지션의 시작 위치(몇 번째 바이트?)
													D3DDECLTYPE_FLOAT3,		// float 3개짜리 인자
													D3DDECLMETHOD_DEFAULT,	// 버텍스 사용 방식
													D3DDECLUSAGE_POSITION,	// FPF는 이것밖에 못함
													0
												},
												{
													0,12,					// 포지션의 시작 위치(몇 번째 바이트?)
													D3DDECLTYPE_FLOAT3,		// float 3개짜리 인자
													D3DDECLMETHOD_DEFAULT,	// 버텍스 사용 방식
													D3DDECLUSAGE_NORMAL,	// FPF는 이것밖에 못함
													0
												},
												{
													0, 24,
													D3DDECLTYPE_FLOAT16_4,
													D3DDECLMETHOD_DEFAULT,	
													D3DDECLUSAGE_TEXCOORD,		
													0
												},
												{
													0, 32,
													D3DDECLTYPE_FLOAT16_4,
													D3DDECLMETHOD_DEFAULT,
													D3DDECLUSAGE_TEXCOORD,
													1
												},
												D3DDECL_END()
											};


	D3DFRAME->GetDevice()->CreateVertexDeclaration(pElem, &s_pVDecl);
	LPD3DXBUFFER	pErr	=	NULL;
	D3DXCreateEffectFromFileA ( D3DFRAME->GetDevice(),		//	이펙트를 생성하는 장치의 포인터
								szFileName,					//	파일명의 포인터, 유니코드, ANSI 캐릭터라인 둘 다 지원
								NULL,						//	프리프로세서 매크로 정의의 NULL로 끝나는 옵션의 배열
								NULL,						//	옵션의 인터페이스 포인터
								D3DXSHADER_SKIPVALIDATION,	// 정상 쉐이더 코드인지 검사 안함  );
								NULL, &s_pEffect, &pErr );
	if( pErr )
	{
		OutputDebugStringA( (LPSTR)pErr->GetBufferPointer() );
	}
}

CTerrain* CTerrain::GetInst()
{
	if ( Map_Inst == NULL )
	{
		Map_Inst = new CTerrain;
		RSCMGR->m_bMAP_MGR	=	true;
	}
	return Map_Inst;
}


unsigned	int	CTerrain::Init	( const char* szFileName )
{
	STERRAINVERTEX					*pData		=	NULL;
	unsigned	short				*piData		=	NULL;

	m_IndexCnt			= 0;
	int		SizeRef		= m_CellSize;
	while	( SizeRef >= 1 )
	{
		// LOD가 더이상 되지 않을 때까지 반복하면서 카운팅을 한다.
		SizeRef /= 2;
		m_IndexCnt ++;
	}

//////////////////////////////////////////////////////////////////////////
//	비트맵을 읽어 비트맵의 픽셀갯수 만큼 격자를 가진 필드를 생성.
//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//	비트맵을 읽어들인 후 메모리에 적재하는 부분.
	//////////////////////////////////////////////////////////////////////////
	D3DDEVICE_CREATION_PARAMETERS	param;
	D3DFRAME->GetDevice()->GetCreationParameters ( &param );
	HDC				hMemDC, hdc;
	BITMAPINFO		sBMPINFO;
	byte*			lpBuffer = NULL;
	hdc				= GetDC ( param.hFocusWindow );
	hMemDC			= CreateCompatibleDC ( hdc );
	HBITMAP	hBMP	= (HBITMAP)LoadImage ( NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );
	GetObject ( hBMP, sizeof(sBMPINFO), &sBMPINFO );
	long	scanline= sBMPINFO.bmiHeader.biWidth*3 + (((sBMPINFO.bmiHeader.biWidth*3)%2 == 0)?0:1);
	int		Size	= scanline * sBMPINFO.bmiHeader.biHeight;
	m_CellCnt			= __max(1, sBMPINFO.bmiHeader.biWidth / m_CellSize);
	m_CellSize		++;
	if ( Size < 0 )
	{
		lpBuffer = new byte [-sBMPINFO.bmiHeader.biWidth * sBMPINFO.bmiHeader.biHeight * 3];
		GetBitmapBits ( hBMP, -Size, (LPVOID)lpBuffer );
	}
	else
	{
		lpBuffer = new byte [scanline * sBMPINFO.bmiHeader.biHeight];
		GetBitmapBits ( hBMP, Size, (LPVOID)lpBuffer );
	}
	DeleteObject ( hBMP );
	// lpBuffer에 비트맵의 색상정보가 적재 된다.
	//////////////////////////////////////////////////////////////////////////
	//							비트맵을 읽어들인 후 메모리에 적재하는 부분.
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//	비트맵 헤더에 저장된
	//	가로세로 크기만큼
	//	정점과 인덱스를 생성한다.
	//////////////////////////////////////////////////////////////////////////
	m_nWidth = sBMPINFO.bmiHeader.biWidth;	// 그림의 가로
	m_nDepth = sBMPINFO.bmiHeader.biHeight;	// 그림의 세로

	int	k			= 0;
	int	m			= 0;
	STERRAINVERTEX* pVertex	= NULL;
	unsigned short*		pIndex		= NULL;
	pVertex	= new STERRAINVERTEX	[m_nWidth*m_nDepth];

	// 자르지 않은 데이터를 처리한다. 버텍스 위치와 Normal을 계산한다.
	for ( k = 0; k < m_nWidth*m_nDepth; k ++ )
	{
		int	x = k %	m_nWidth;
		int	z = k /	m_nWidth;
		pVertex[k].x = (float)(x - m_nWidth/2)*1;
		pVertex[k].y = (float)(lpBuffer[(x*3+z*(scanline))+1])*0.6f;
		pVertex[k].z = (float)(z - m_nDepth/2)*-1;

		pVertex[k].nx	= 0;
		pVertex[k].ny	= 0;
		pVertex[k].nz	= 0;
		D3DXFloat32To16Array( &pVertex[k].yt[0], &pVertex[k].y, 1 );
		D3DXFloat32To16Array( &pVertex[k].yt[1], &pVertex[k].y, 1 );
		D3DXFloat32To16Array( &pVertex[k].yt[2], &pVertex[k].y, 1 );
		D3DXFloat32To16Array( &pVertex[k].yt[3], &pVertex[k].y, 1 );
		D3DXFloat32To16Array( &pVertex[k].yt[4], &pVertex[k].y, 1 );
		D3DXFloat32To16Array( &pVertex[k].yt[5], &pVertex[k].y, 1 );
		D3DXFloat32To16Array( &pVertex[k].yt[6], &pVertex[k].y, 1 );
		D3DXFloat32To16Array( &pVertex[k].yt[7], &pVertex[k].y, 1 );
	}

	for ( k = 0; k < m_nWidth*m_nDepth; k ++ )
	{
		int	x = k %	m_nWidth;
		int	z = k /	m_nWidth;
		int			uz	= __max( 0, z - 1 );
		int			dz	= __min( m_nDepth-1, z + 1 );
		int			lx	= __max( 0, x - 1 );
		int			rx	= __min( m_nWidth-1, x + 1 );
		int			iu	= x + uz * m_nWidth;
		int			id	= x + dz * m_nWidth;
		int			il	= lx + z * m_nWidth;
		int			ir	= rx + z * m_nWidth;
		float		y0	= pVertex[k].y;
		float		y1	= pVertex[iu].y;
		float		y2	= pVertex[id].y;
		float		y3	= pVertex[ir].y;
		float		y4	= pVertex[il].y;
		pVertex[k].y = (y0 + y1 + y2 + y3 + y4)*0.2f;
	}
	// Normal 계산.
	for ( k = 0; k < m_nWidth*m_nDepth; k ++ )
	{
		int	x = k %	m_nWidth;
		int	z = k /	m_nWidth;
		D3DXVECTOR3	n	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3	n1	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3	n2	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3	n3	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3	n4	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		int			uz	= __max( 0, z - 1 );
		int			dz	= __min( m_nDepth-1, z + 1 );
		int			lx	= __max( 0, x - 1 );
		int			rx	= __min( m_nWidth-1, x + 1 );
		int			iu	= x + uz * m_nWidth;
		int			id	= x + dz * m_nWidth;
		int			il	= lx + z * m_nWidth;
		int			ir	= rx + z * m_nWidth;

		D3DXVECTOR3	p0	= D3DXVECTOR3(pVertex[k].x, pVertex[k].y, pVertex[k].z);
		D3DXVECTOR3	pu	= D3DXVECTOR3(pVertex[iu].x, pVertex[iu].y, pVertex[iu].z);
		D3DXVECTOR3	pd	= D3DXVECTOR3(pVertex[id].x, pVertex[id].y, pVertex[id].z);
		D3DXVECTOR3	pl	= D3DXVECTOR3(pVertex[il].x, pVertex[il].y, pVertex[il].z);
		D3DXVECTOR3	pr	= D3DXVECTOR3(pVertex[ir].x, pVertex[ir].y, pVertex[ir].z);

		D3DXVec3Cross	( &n1, &(p0-pu), &(p0-pl) );
		D3DXVec3Cross	( &n2, &(pu-p0), &(p0-pr) );
		D3DXVec3Cross	( &n3, &(p0-pd), &(pl-p0) );
		D3DXVec3Cross	( &n4, &(pd-p0), &(pr-p0) );
		D3DXVec3Normalize ( &n, &(n1+n2+n3+n4) );
		D3DXVec3Normalize ( &n, &n );
		pVertex[k].nx	= -n.x;
		pVertex[k].ny	= -n.y;
		pVertex[k].nz	= -n.z;
	}

	// Field devide. 필드를 나눈다. CellSize에 정의해둔 크기별로 자른다.
	// 어느만큼 크기로 잘라낼지는 그림폭/CellSize로 얻을 수 있다. -> CellCnt에 저장.
	STERRAINVERTEX** ppCellVert	= NULL;
	ppCellVert					= new STERRAINVERTEX*	[m_CellCnt*m_CellCnt];
	m_piCenterPos				= new D3DXVECTOR3		[m_CellCnt*m_CellCnt];
	int		StartIdx			= 0;
	for ( k = 0; k < m_CellCnt * m_CellCnt; k ++ )
	{
		int	x = k % m_CellCnt;
		int	z = k / m_CellCnt;
		ppCellVert[k] = new STERRAINVERTEX	[m_CellSize*m_CellSize];

		StartIdx	= (m_CellSize-1)*x + (m_CellSize-1)*z*m_nWidth;
		// 조각나기 시작하는 버텍스의 최초 인덱스(좌측 상단이라고 보면 편함)
		// 나누어 지는 곳은 정점을 공유하게 되므로 공유하는 만큼 정점이 늘어난다.
		//			┌───────┬─────┬─────┐
		//			│				↓			↓			↓
		//	요기──┼→┌┬┬┬┐	┌┬┬┬┐	┌┬┬┬┐	┌┬┬┬┐┐──────────────────┐
		//	요런데	│	├┼┼┼┤	├┼┼┼┤	├┼┼┼┤	├┼┼┼┤│칸은 4칸이지만 정점의 갯수는 5개.	│
		//			│	├┼┼┼┤	├┼┼┼┤	├┼┼┼┤	├┼┼┼┤├CellSize(5)	┐						│
		//			│	├┼┼┼┤	├┼┼┼┤	├┼┼┼┤	├┼┼┼┤│			│						│
		//			│	└┴┴┴┘	└┴┴┴┘	└┴┴┴┘	└┴┴┴┘┘			│						│
		//			│																│						│
		//			└→┌┬┬┬┐	┌┬┬┬┐	┌┬┬┬┐	┌┬┬┬┐				│						│
		//				├┼┼┼┤	├┼┼┼┤	├┼┼┼┤	├┼┼┼┤				│						│
		//				├┼┼┼┤	├┼┼┼┤	├┼┼┼┤	├┼┼┼┤		────┤						├Width/Depth
		//				├┼┼┼┤	├┼┼┼┤	├┼┼┼┤	├┼┼┼┤				│						│
		//				└┴┴┴┘	└┴┴┴┘	└┴┴┴┘	└┴┴┴┘				├CellCnt(4)			│
		//																			│						│
		//				┌┬┬┬┐	┌┬┬┬┐	┌┬┬┬┐	┌┬┬┬┐				│						│
		//				├┼┼┼┤	├┼┼┼┤	├┼┼┼┤	├┼┼┼┤				│						│
		//				├┼┼┼┤	├┼┼┼┤	├┼┼┼┤	├┼┼┼┤		────┤						│
		//				├┼┼┼┤	├┼┼┼┤	├┼┼┼┤	├┼┼┼┤				│						│
		//				└┴┴┴┘	└┴┴┴┘	└┴┴┴┘	└┴┴┴┘				│						│
		//																			│						│
		//				┌┬┬┬┐	┌┬┬┬┐	┌┬┬┬┐	┌┬┬┬┐				│						│
		//				├┼┼┼┤	├┼┼┼┤	├┼┼┼┤	├┼┼┼┤				│						│
		//				├┼┼┼┤	├┼┼┼┤	├┼┼┼┤	├┼┼┼┤		────┘						│
		//				├┼┼┼┤	├┼┼┼┤	├┼┼┼┤	├┼┼┼┤										│
		//				└┴┴┴┘	└┴┴┴┘	└┴┴┴┘	└┴┴┴┘───────────────────┘
		//

		for ( int m = 0; m < m_CellSize; m ++ )
		{
			memcpy ( &ppCellVert[k][m*m_CellSize], &pVertex[m*m_nWidth + StartIdx], sizeof(STERRAINVERTEX)*m_CellSize );
		}
		m_piCenterPos[k].x = (ppCellVert[k][0].x + ppCellVert[k][m_CellSize*m_CellSize-1].x)*0.5f;
		m_piCenterPos[k].y = (ppCellVert[k][0].y + ppCellVert[k][m_CellSize*m_CellSize-1].y)*0.5f;
		m_piCenterPos[k].z = (ppCellVert[k][0].z + ppCellVert[k][m_CellSize*m_CellSize-1].z)*0.5f;

// Geomorph를 위해 사이값들을 만들어낸다.
		int	stepidx = 0;
		for ( int step = 2; step < m_CellSize; step *= 2 )
		{
			for ( int i1 = 0; i1 < m_CellSize-step; i1 += step )
			{
				for ( int j1 = 0; j1 < m_CellSize-step; j1 += step )
				{
					float	y00 = ppCellVert[k][j1+i1*m_CellSize].y;
					float	y10 = ppCellVert[k][j1+step+i1*m_CellSize].y;
					float	y01 = ppCellVert[k][j1+(i1+step)*m_CellSize].y;
					float	y11 = ppCellVert[k][j1+step+(i1+step)*m_CellSize].y;
					float	y01t= y10 + ((y00-y10) + (y11-y10));
					float	y10t= y01 + ((y11-y01) + (y00-y01));
					float	y11t= y00 + ((y10-y00) + (y01-y00));
					float	y00t= y11 + ((y01-y11) + (y10-y11));

					for ( int subi = 0; subi <= step; subi ++ )
					{
						int	subj = 0;
						float	fistep	= (float)(subi)/(float)(step);

						for ( subj = 0; subj <  subi; subj ++ )
						{
							float	fjstep	= (float)(subj)/(float)(step);
							float	yx1		= y00 +( y10t- y00 )*fjstep;
							float	yx2		= y01 +( y11 - y01 )*fjstep;
							float	fResult	= yx1+(yx2-yx1)*fistep;
							for ( int idx = stepidx; idx < 8; idx ++ )	D3DXFloat32To16Array( &ppCellVert[k][j1+subj+(i1+subi)*m_CellSize].yt[idx], &fResult, 1);
						}
						for ( subj = step; subj >= subi; subj -- )
						{
							float	fjstep	= (float)(subj)/(float)(step);
							float	yx1		= y00 +(y10 - y00 )*fjstep;
							float	yx2		= y01t+(y11 - y01t)*fjstep;
							float	fResult	= yx1+(yx2-yx1)*fistep;
							for ( int idx = stepidx; idx < 8; idx ++ )	D3DXFloat32To16Array( &ppCellVert[k][j1+subj+(i1+subi)*m_CellSize].yt[idx], &fResult, 1);
						}
					}
				}
			}
			stepidx ++;
		}
	}
	// 그림 클경우 메모리를 너무 많이 차지한다. 원본은 지우자.
	delete [] pVertex;
	pVertex = NULL;

	// LOD처리를 위해 인덱스 버퍼를 LOD갯수(IndexCnt) 만큼 만든다.
	m_piBUFFER		= new LPDIRECT3DINDEXBUFFER9	[m_IndexCnt];
	m_piBUFFERLen	= new int						[m_IndexCnt];
	int			LodStep	= 1;	// 버텍스 몇개를 건너 뛸지 정한다. -> 1, 2, 4, 8, 16, 32....
	for ( int k1 = 0; k1 < m_IndexCnt; k1 ++ )
	{
		int		LodSqSize	=((m_CellSize-1)/LodStep);				// 건너 뛴 후 한줄에 배치될 버텍스의 갯수.
		m_piBUFFERLen[k1]	= LodSqSize * LodSqSize*2*3;				// 실제 LOD Index Buffer 의 크기.

		pIndex		= new unsigned short		[m_piBUFFERLen[k1]];	// 실제 LOD인덱스.
		int	x		= -1;
		int	z		= -1;
		int	index	= -1;
		for ( k = 0; k < m_piBUFFERLen[k1]/6; k ++ )
		{
			x		= ((k) % ((m_CellSize-1) / LodStep)) * LodStep;				// LodStep만큼 건너뛰며 인덱스 생성.
			z		= ((k) / ((m_CellSize-1) / LodStep)) * LodStep;
			index	= (x + z *	(m_CellSize));
			pIndex[k*6+0] = index;
			pIndex[k*6+1] = index + LodStep;
			pIndex[k*6+2] = index + m_CellSize*LodStep + LodStep;
			pIndex[k*6+3] = index;
			pIndex[k*6+4] = index + LodStep + m_CellSize*LodStep;
			pIndex[k*6+5] = index + m_CellSize*LodStep;
		}

		D3DFRAME->GetDevice()->CreateIndexBuffer (	2*m_piBUFFERLen[k1],					// k1번째 인덱스 버퍼를 생성.
													0, D3DFMT_INDEX16, D3DPOOL_DEFAULT,
													&m_piBUFFER[k1], NULL );
		m_piBUFFER[k1]->Lock				( 0, 0, (void**)&piData, D3DLOCK_DISCARD );
		memcpy ( piData, pIndex, 2*m_piBUFFERLen[k1] );
		m_piBUFFER[k1]->Unlock			();
		LodStep	*= 2;
		delete [] pIndex;
		pIndex		= NULL;
	}

	m_pvBUFFER	= new LPDIRECT3DVERTEXBUFFER9	[m_CellCnt * m_CellCnt];
	for ( k = 0; k < m_CellCnt * m_CellCnt; k ++ )
	{
		D3DFRAME->GetDevice()->CreateVertexBuffer(	sizeof(STERRAINVERTEX)*m_CellSize*m_CellSize,	//	바이트 단위
													0,												//	용도는
													0,												//	
													D3DPOOL_DEFAULT,								//	버텍스 버퍼를 어디에다가 만들 것인가? (비디오 메모리만)
													&m_pvBUFFER[k],									//
													NULL);
		m_pvBUFFER[k]->Lock(	0, 0, (void**)&pData, D3DLOCK_DISCARD);								//	Lock을 거는 방식(데이터 손실을 신경쓰지 않고 가져오겠다)
		memcpy( pData, ppCellVert[k], sizeof(STERRAINVERTEX)*m_CellSize*m_CellSize );
		m_pvBUFFER[k]->Unlock ();
		delete [] ppCellVert[k];
	}
	delete ppCellVert;
//////////////////////////////////////////////////////////////////////////
//			비트맵을 읽어 비트맵의 픽셀갯수 만큼 격자를 가진 필드를 생성.(끗)
//////////////////////////////////////////////////////////////////////////

	return 0UL;
}

void			CTerrain::FreeInst	( void )
{
	int		i = 0;
	for ( i = 0; i < m_CellCnt*m_CellCnt; i ++ )
	{
		m_pvBUFFER[i]->Release();
	}
	delete [] m_pvBUFFER;
	m_pvBUFFER	= NULL;
	for ( i = 0; i < m_IndexCnt; i ++ )
	{
		m_piBUFFER[i]->Release();
	}
	delete [] m_piBUFFER;
	m_piBUFFER	= NULL;

	if ( m_piBUFFERLen )
	{
		delete [] m_piBUFFERLen;
		m_piBUFFERLen	= NULL;
	}
	if ( m_piCenterPos )
	{
		delete [] m_piCenterPos;
		m_piCenterPos	= NULL;
	}

	if ( Map_Inst != NULL )
	{
		delete Map_Inst;
		Map_Inst	=	NULL;
		RSCMGR->m_bMAP_MGR	=	false;
	}
}

void			CTerrain::Render	( void )
{
	if ( !m_pNormalMap ) CreateNormalMap ();
	HRESULT	hr;
	unsigned int uPass = 0, ui = 0, k = 0, nLod = 0;
	int		m_MonsterSize = MOBMGR->IterEnd();
	
	hr	=	s_pEffect->SetTechnique	( "render" );
	hr	=	s_pEffect->SetMatrix	( "fView",	&D3DFRAME->m_mView );
	hr	=	s_pEffect->SetMatrix	( "fProj",	&D3DFRAME->m_mProj );
	hr	=	s_pEffect->SetFloat		( "fTimer", (float)(timeGetTime())*0.001f );
	hr	=	s_pEffect->SetFloatArray( "CamPos", CAMMGR->GetPosition() , 3 );

	//	몬스터 공격 범위 표시
	hr  =	s_pEffect->SetFloat		( "ActionLen0", 0 );
	hr  =	s_pEffect->SetFloat		( "ActionLen1", 0 );
	hr  =	s_pEffect->SetFloat		( "ActionLen2", 0 );
	for( int i = 0; i < m_MonsterSize; i++ )
	{
		switch( i )
		{
		case 0:
			{
				hr	=	s_pEffect->SetFloatArray( "ActionPos0", (MOBMGR->MobVec( i ))->GetActionPos(), 3 );
				hr  =	s_pEffect->SetFloat		( "ActionLen0", (MOBMGR->MobVec( i ))->GetActionLen() );
				break;
			}
		case 1:
			{
				hr	=	s_pEffect->SetFloatArray( "ActionPos1", (MOBMGR->MobVec( i ))->GetActionPos(), 3 );
				hr  =	s_pEffect->SetFloat		( "ActionLen1", (MOBMGR->MobVec( i ))->GetActionLen() );
				break;
			}
		case 2:
			{
				hr	=	s_pEffect->SetFloatArray( "ActionPos2", (MOBMGR->MobVec( i ))->GetActionPos(), 3 );
				hr  =	s_pEffect->SetFloat		( "ActionLen2", (MOBMGR->MobVec( i ))->GetActionLen() );
				break;
			}
		}
	}

	hr	=	s_pEffect->SetTexture	( "texPert",m_pPerturbation );	s_pEffect->SetBool	( "bTexPert",(m_pPerturbation!=NULL) );
	hr	=	s_pEffect->SetTexture	( "texNorm",m_pNormalMap );
	hr	=	s_pEffect->SetTexture	( "texChannel",m_pChannel );
	hr	=	s_pEffect->SetTexture	( "texChannelA",m_pChannelA );
	hr	=	s_pEffect->SetTexture	( "texChannelR",m_pChannelR );
	hr	=	s_pEffect->SetTexture	( "texChannelG",m_pChannelG );
	hr	=	s_pEffect->SetTexture	( "texChannelB",m_pChannelB );


	//그림자 부분
	hr	=	s_pEffect->SetTexture	( "shadowTx", D3DFRAME->m_pShadowDep );
	D3DXMATRIX	matrix = D3DFRAME->m_mLView * D3DFRAME->m_mLProj * D3DFRAME->m_mLBias; //직교투영시 -1~1까지 값이 되므로 0부터 1까지하려면 1을더한후 /2를 해주자 
	hr	=	s_pEffect->SetMatrix	( "matShadow", &matrix );
	//그림자 부분
	D3DFRAME->GetDevice()->SetVertexDeclaration(s_pVDecl);

	s_pEffect->Begin( &uPass, 0 );
	for ( ui = 0; ui < uPass; ui++)
	{
		s_pEffect->BeginPass( ui );
		{
			for ( k = 0; k < m_CellCnt*m_CellCnt; k ++ )	// 나누어 놓은 Cell을 모두 그린다.
			{
				float	fLod = (D3DXVec3Length( &(CAMMGR->GetPosition() - m_piCenterPos[k]) )) * 0.005f;
				nLod = (int)(fLod);
				nLod = __max(0, __min(m_IndexCnt-1, nLod) );

				D3DFRAME->GetDevice()->SetStreamSource		(0, m_pvBUFFER[k], 0, sizeof(STERRAINVERTEX));
				D3DFRAME->GetDevice()->SetIndices			( m_piBUFFER[nLod] );
				D3DFRAME->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0 ,
															0, m_CellSize*m_CellSize, 0,
															m_piBUFFERLen[nLod]/3);
			}
		}
		s_pEffect->EndPass();
	}
	s_pEffect->End();
}

float		CTerrain::GetHeight ( float x, float z, LPD3DXVECTOR3 vCell, LPD3DXVECTOR3 vNormal)
{
//////////////////////////////////////////////////////////////////////////
// Get height of xz plane of height field.
//////////////////////////////////////////////////////////////////////////
	D3DXVECTOR3	Normal	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	int	BlockIdxX = (int)((x + m_nWidth/2)/(float)(m_CellSize-1));
	int	BlockIdxZ = (int)((-z+ m_nDepth/2)/(float)(m_CellSize-1));
	int	BlockIdx = BlockIdxX + BlockIdxZ * m_CellCnt;
	// 블록안에 체크점이 존재한다.
	if ( BlockIdx < 0 || BlockIdx >= m_CellCnt*m_CellCnt )	return 0.0f;

	STERRAINVERTEX	*pVert = NULL;
	// 해당 블록내에서 사각형 하나를 추려낸다.
	m_pvBUFFER[BlockIdx]->Lock( 0, 0, (void**)&pVert, D3DLOCK_READONLY );
	float	xposOffset	= __min((float)(m_CellSize-1)-0.000001f, (x - pVert[0].x));
	float	zposOffset	= __min((float)(m_CellSize-1)-0.000001f,-(z - pVert[0].z));
	int		RCellSize	= m_CellSize + 1;
	int		GridIdx0	= (int)(xposOffset)		+ (int)(zposOffset)*m_CellSize;
	int		GridIdx1	= (int)(xposOffset+1)	+ (int)(zposOffset)*m_CellSize;
	int		GridIdx2	= (int)(xposOffset)		+ (int)(zposOffset+1)*m_CellSize;
	int		GridIdx3	= (int)(xposOffset+1)	+ (int)(zposOffset+1)*m_CellSize;
	
	if ( vCell )
	{
		vCell[0]	= D3DXVECTOR3( pVert[GridIdx0].x, pVert[GridIdx0].y, pVert[GridIdx0].z );
		vCell[1]	= D3DXVECTOR3( pVert[GridIdx1].x, pVert[GridIdx1].y, pVert[GridIdx1].z );
		vCell[2]	= D3DXVECTOR3( pVert[GridIdx3].x, pVert[GridIdx3].y, pVert[GridIdx3].z );
		vCell[3]	= vCell[0];
		vCell[4]	= D3DXVECTOR3( pVert[GridIdx0].x, pVert[GridIdx0].y, pVert[GridIdx0].z );
		vCell[5]	= D3DXVECTOR3( pVert[GridIdx3].x, pVert[GridIdx3].y, pVert[GridIdx3].z );
		vCell[6]	= D3DXVECTOR3( pVert[GridIdx2].x, pVert[GridIdx2].y, pVert[GridIdx2].z );
		vCell[7]	= vCell[4];
	}
	
	float	alphax		= (xposOffset/1.0f);
	float	alphaz		= (zposOffset/1.0f);
	float	y00			= 0;
	float	y10			= 0;
	float	y11			= 0;
	float	y01			= 0;
	double	t			= 0;
	alphax = modf ( alphax, &t );
	alphaz = modf ( alphaz, &t );
	if ( alphax > alphaz )
	{
		y00			= pVert[GridIdx0].y;
		y10			= pVert[GridIdx1].y;
		y11			= pVert[GridIdx3].y;
		y01			= y10 + (y00-y10) + (y11-y10);
		D3DXVec3Cross(&Normal, &(D3DXVECTOR3( pVert[GridIdx1].x, pVert[GridIdx1].y, pVert[GridIdx1].z ) 
			- D3DXVECTOR3( pVert[GridIdx0].x, pVert[GridIdx0].y, pVert[GridIdx0].z )), 
			&(D3DXVECTOR3( pVert[GridIdx3].x, pVert[GridIdx3].y, pVert[GridIdx3].z ) 
			- D3DXVECTOR3( pVert[GridIdx1].x, pVert[GridIdx1].y, pVert[GridIdx1].z )));
	}
	else
	{
		y00			= pVert[GridIdx0].y;
		y11			= pVert[GridIdx3].y;
		y01			= pVert[GridIdx2].y;
		y10			= y01 + (y00-y01) + (y11-y01);
		D3DXVec3Cross(&Normal, &(D3DXVECTOR3( pVert[GridIdx3].x, pVert[GridIdx3].y, pVert[GridIdx3].z )\
			- D3DXVECTOR3( pVert[GridIdx0].x, pVert[GridIdx0].y, pVert[GridIdx0].z )),
			&(D3DXVECTOR3( pVert[GridIdx2].x, pVert[GridIdx2].y, pVert[GridIdx2].z )
			- D3DXVECTOR3( pVert[GridIdx3].x, pVert[GridIdx3].y, pVert[GridIdx3].z )));
	}
	float	x1 = y00	+ (y10-y00)*alphax;
	float	x2 = y01	+ (y11-y01)*alphax;
	float	yy = x1		+ (x2-x1)*alphaz;

	m_pvBUFFER[BlockIdx]->Unlock ();
	D3DXVec3Normalize(&Normal, &Normal);
	
	if(vNormal != NULL)
		(*vNormal)	=	Normal;

	return yy;
}

void		CTerrain::CreatePerturbationTexture	( const char* szFileName )
{
	D3DXCreateTextureFromFileA( D3DFRAME->GetDevice(), szFileName, &m_pPerturbation );
}

void		CTerrain::CreateChannelTexture		( const char* szFileName )
{
	D3DXCreateTextureFromFileA( D3DFRAME->GetDevice(), szFileName, &m_pChannel );
}

void		CTerrain::CreateTextureUsingA		( const char* szFileName )
{
	D3DXCreateTextureFromFileA( D3DFRAME->GetDevice(), szFileName, &m_pChannelA );
}

void		CTerrain::CreateTextureUsingR		( const char* szFileName )
{
	D3DXCreateTextureFromFileA( D3DFRAME->GetDevice(), szFileName, &m_pChannelR );
}

void		CTerrain::CreateTextureUsingG		( const char* szFileName )
{
	D3DXCreateTextureFromFileA( D3DFRAME->GetDevice(), szFileName, &m_pChannelG );
}

void		CTerrain::CreateTextureUsingB		( const char* szFileName )
{
	D3DXCreateTextureFromFileA( D3DFRAME->GetDevice(), szFileName, &m_pChannelB );
}


void		CTerrain::CreateNormalMap	( void )
{
	LPDIRECT3DSURFACE9	pSurface		= NULL;
	LPDIRECT3DSURFACE9	pZBuffer		= NULL;
	LPDIRECT3DSURFACE9	pBackupColor1	= NULL;
	LPDIRECT3DSURFACE9	pBackupColor2	= NULL;
	LPDIRECT3DSURFACE9	pBackupColor3	= NULL;
	LPDIRECT3DSURFACE9	pBackupColor4	= NULL;
	LPDIRECT3DSURFACE9	pBackupDepth	= NULL;

	D3DXCreateTexture	( D3DFRAME->GetDevice(), 2048, 2048, 0, D3DUSAGE_RENDERTARGET|D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pNormalMap );
	m_pNormalMap->GetSurfaceLevel	( 0, &pSurface );
	D3DFRAME->GetDevice()->CreateDepthStencilSurface ( 2048, 2048, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, FALSE, &pZBuffer, NULL );

	D3DFRAME->GetDevice()->GetRenderTarget ( 0, &pBackupColor1 );
	D3DFRAME->GetDevice()->GetRenderTarget ( 1, &pBackupColor2 );
	D3DFRAME->GetDevice()->GetRenderTarget ( 2, &pBackupColor3 );
	D3DFRAME->GetDevice()->GetRenderTarget ( 3, &pBackupColor4 );
	D3DFRAME->GetDevice()->GetDepthStencilSurface ( &pBackupDepth );

	D3DFRAME->GetDevice()->SetRenderTarget ( 0, pSurface );
	D3DFRAME->GetDevice()->SetDepthStencilSurface ( pZBuffer );

	D3DFRAME->GetDevice()->Clear(0,NULL,D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,0x00000000,1.0f,0UL);

	HRESULT	hResult = 0UL;
	hResult = D3DFRAME->GetDevice()->EndScene ();
	hResult = D3DFRAME->GetDevice()->BeginScene ();

	D3DFRAME->SetCamera ( D3DXVECTOR3(0.0f, 5000.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f) );
	D3DXMatrixOrthoLH	( &D3DFRAME->m_mProj, m_nWidth, m_nDepth, 1.0f, 15000.0f );

	unsigned int uPass = 0, ui = 0, k = 0, nLod = 0;
	
	s_pEffect->SetTechnique	( "createNormal" );
	s_pEffect->SetMatrix	( "fView",	&D3DFRAME->m_mView );
	s_pEffect->SetMatrix	( "fProj",	&D3DFRAME->m_mProj );
	D3DFRAME->GetDevice()->SetVertexDeclaration(s_pVDecl);

	s_pEffect->Begin( &uPass, 0 );
	for ( ui = 0; ui < uPass; ui++)
	{
		s_pEffect->BeginPass( ui );
		{
			for ( k = 0; k < m_CellCnt*m_CellCnt; k ++ )	// 나누어 놓은 Cell을 모두 그린다.
			{
				D3DFRAME->GetDevice()->SetStreamSource		(0, m_pvBUFFER[k], 0, sizeof(STERRAINVERTEX));
				D3DFRAME->GetDevice()->SetIndices			( m_piBUFFER[0] );
				D3DFRAME->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0 ,
															0, m_CellSize*m_CellSize, 0,
															m_piBUFFERLen[0]/3);
			}
		}
		s_pEffect->EndPass();
	}
	s_pEffect->End();


	D3DFRAME->GetDevice()->SetRenderTarget ( 0, pBackupColor1 );
	D3DFRAME->GetDevice()->SetRenderTarget ( 1, pBackupColor2 );
	D3DFRAME->GetDevice()->SetRenderTarget ( 2, pBackupColor3 );
	D3DFRAME->GetDevice()->SetRenderTarget ( 3, pBackupColor4 );
	D3DFRAME->GetDevice()->SetDepthStencilSurface ( pBackupDepth );
	hResult = D3DFRAME->GetDevice()->EndScene ();
	hResult = D3DFRAME->GetDevice()->BeginScene ();
}
