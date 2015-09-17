// SkinRenderer.cpp: implementation of the CSkinRenderer class.
//
//////////////////////////////////////////////////////////////////////

#include "SkinRenderer.h"
#include "D3DWRAPPER.h"
#include "../AProject/MainHeader.h"

LPD3DXEFFECT					CSkinRenderer::s_pEffect = NULL;
CSkinLoader						CSkinRenderer::s_cSkinLoader;
LPDIRECT3DVERTEXDECLARATION9	CSkinRenderer::s_pVDecl		= NULL;

E_UPDATE_STATUS	SMOTION::CalcCurrentFrame ( void )
{
	E_UPDATE_STATUS	eUpdateStatus = UPDATE_PLAYING;
	int		nRange		= m_nFrameD - m_nFrameS;												// 프레임 길이.
	float	fTime		= (float)(fTimeGetTime()-m_dwStart) / (m_fSpeed * 1000.0f / nRange);	// 정해둔 시간에 맞춰 현재 프레임 계산.

	// 현재 프레임 저장.
	m_fCurFrame= fTime;

	// 보간을 위한 이전 프레임.
	m_nThisFrame	= ((int)m_fCurFrame);

	// 이전 프레임에서 현재 프레임 까지의 소수점.(0~1.0f)
	m_fSlerp		= m_fCurFrame - m_nThisFrame;

	// fTimeGetTime()으로 계산하므로 범위내에서 순환하게 재계산.
	m_nThisFrame	= ((int)m_fCurFrame)%(nRange) + m_nFrameS;

	// 루프애니메이션 다음 프레임 계산.
	if( m_bLooped )
	{
		if ( (m_nThisFrame+1) >= m_nFrameD )
		{
			m_nNextFrame	= m_nFrameS;
			eUpdateStatus = UPDATE_RETURN;
		}
		else	m_nNextFrame	= m_nThisFrame + 1;
	}
	else
	{
		eUpdateStatus = UPDATE_PLAYING;
		// 끝까지 재생했으면 끝프레임에서 고정.
		int		nRange		= m_nFrameD - m_nFrameS;									// 프레임 길이.
		float	fTime		= (float)(fTimeGetTime()-m_dwStart) / (m_fSpeed * 1000.0f / nRange);	// 정해둔 시간에 맞춰 현재 프레임 계산.

		int		S = m_nFrameS;
		int		D = m_nFrameD;
		float	fTimeRelative = fTime + S;
		if ( fTimeRelative >= m_nFrameD )
		{
			m_nThisFrame	= ((int)m_nFrameD);
			m_nNextFrame	= ((int)m_nFrameD);
			m_fCurFrame		= m_nThisFrame-1;
			fTime = m_nFrameD-1;
			m_fSlerp = 0.0f;
			eUpdateStatus		= UPDATE_PAUSE;
			return eUpdateStatus;
		}
		else m_fCurFrame = fTime;
		m_nThisFrame	= ((int)m_fCurFrame);
		m_fSlerp		= m_fCurFrame - m_nThisFrame;
		m_nThisFrame	= ((int)m_fCurFrame)%(nRange) + m_nFrameS;
		m_nNextFrame	= m_nThisFrame + 1;
	}

	return eUpdateStatus;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
bool	CSkinRenderer::LoadEffect ( char* pFileName )
{
	LPD3DXBUFFER	pBuffer = NULL;
	D3DXCreateEffectFromFileA ( D3DFRAME->GetDevice(), pFileName, NULL, NULL, D3DXSHADER_SKIPVALIDATION, NULL, &s_pEffect, &pBuffer );
	if ( pBuffer )
	{
		OutputDebugStringA ( (LPSTR)pBuffer->GetBufferPointer() );
	}
	D3DVERTEXELEMENT9	decl[] ={	{ 0, 0,		D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION	,0 },
									{ 0, 12,	D3DDECLTYPE_FLOAT4,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT	,0 },
									{ 0, 28,	D3DDECLTYPE_UBYTE4,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES,0 },
									{ 0, 32,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL		,0 },
									{ 0, 44,	D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD	,0 },
									{ 1, 0,		D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT		,0 },
									D3DDECL_END() };
	D3DFRAME->GetDevice()->CreateVertexDeclaration( decl, &s_pVDecl );

	return true;
}

CSkinRenderer::CSkinRenderer()
{
	m_fLastBlendingSpeed	= 1.0f;
	m_eUpdateStatus			= UPDATE_PLAYING;
	m_pListBones			= NULL;
	ZeroMemory	( &m_sReplaceMotion, sizeof(SMOTION) );
}

CSkinRenderer::~CSkinRenderer()
{
}

bool			CSkinRenderer::Init		( char* pFileName )
{
	int	i = 0, nLen;
	m_vColor	=	D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	s_cSkinLoader.Initial	( pFileName );			// 로더에서 스키닝객체를 로딩.

	nMatCount	= s_cSkinLoader.GetMaterialCount();	// 로딩된 데이터에서 재질의 갯수를 얻어온다.
	nTexCount	= s_cSkinLoader.GetTextureCount();	// 로딩된 데이터에서 텍스처의 갯수를 얻어온다.
	nObjCount	= s_cSkinLoader.GetObjectCount();	// 로딩된 데이터에서 오브젝트의 갯수를 얻어온다.
	nBoneCount	= s_cSkinLoader.GetBoneCount();		// 로딩된 데이터에서 뼈대의 갯수를 얻어온다.
	nKeyCount	= s_cSkinLoader.GetFrameCount();	// 로딩된 데이터에서 애니메이션 프레임수를 얻어온다.

	m_vRot		= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );	// 회전값 초기화.
	m_vPos		= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );	// 위치값 초기화.
	m_vScl		= D3DXVECTOR3( 1.0f, 1.0f, 1.0f );	// 축척값 초기화.
	D3DXMatrixIdentity ( &m_mWorld );				// 월드행렬 초기화.

	m_ppTB		= new LPDIRECT3DTEXTURE9		[nTexCount];	// 얻어온 텍스처 갯수만큼 텍스처 객체를 생성.
	m_ppVB		= new LPDIRECT3DVERTEXBUFFER9	[nObjCount];	// 얻어온 오브젝트 갯수만큼 버텍스버퍼를 생성.
	m_ppVT		= new LPDIRECT3DVERTEXBUFFER9	[nObjCount];	// 얻어온 오브젝트 갯수만큼 탄젠트버퍼를 생성. -> 노말맵을 이용하기 위함.
	m_ppIB		= new LPDIRECT3DINDEXBUFFER9	[nObjCount];	// 얻어온 오브젝트 갯수만큼 인덱스버퍼를 생성.
	m_pnMatID	= new int						[nObjCount];	// 얻어온 오브젝트 갯수만큼 재질 ID를 생성.
	m_pnVNum	= new int						[nObjCount];	// 얻어온 오브젝트 갯수만큼 버텍스 카운트를 생성.
	m_pnINum	= new int						[nObjCount];	// 얻어온 오브젝트 갯수만큼 인덱스 카운트를 생성.
	m_pbShow	= new bool						[nObjCount];
	m_pBone		= new SBONES					[nBoneCount];	// 얻어온 뼈대 갯수만큼 뼈대정보 객체를 생성.
	m_pMat		= new SKINMATERIAL				[nMatCount];	// 얻어온 재질 갯수만큼 재질정보 객체를 생성.
//	m_pmWorlds	= new D3DXMATRIX				[nBoneCount];	// 얻어온 뼈대 갯수만큼 최종월드행렬을 생성.

//	m_pvSingleMotionPivot	= new D3DXVECTOR3	[nBoneCount];	// 모션1개에 대한 로컬모션 피벗.
//	m_pqSingleMotionRotate	= new D3DXQUATERNION[nBoneCount];	// 모션1개에 대한 로컬모션 회전값.

	m_pVecLocalBoneIndex	= new std::vector<byte>[nObjCount];	// 오브젝트1개당 실제적용되고있는 뼈대의 인덱스.
	// 재질을 생성한다.
	for ( i = 0; i < nMatCount; i ++ )
	{
/*
		SkinLoader에 정의 되어있음.
		struct SKINMATERIAL 
		{
			float	a, r, g, b;		-> Alpha, Red, Green, Blue
			float	Shininess;		-> 하일라이트 광도.
			float	Shine_Str;		-> 하일라이트의 집광도. -> 수가 커질수록 좁은 영역에 집중 된다.

			int		nTextureIdx[2];	-> 쓰일 텍스처의 ID. [0]은 디퓨즈텍스처, [1]은 노말맵. 아무것도 적용되지 않으면 -1이 들어있다.
		};
*/

		PSKINMATERIAL	pData = s_cSkinLoader.GetMaterialSource( i );
		memcpy ( &m_pMat[i], pData, sizeof(SKINMATERIAL));	// 객체 크기가 같으므로 카피만으로 끝이 난다.
	}

	// 텍스처를 생성한다.
	// 로더는 텍스처의 파일 형식 그대로 버퍼에 저장하고 있으므로 D3DXCreateTextureFromFileInMemory 를 이용한다.
	for ( i = 0; i < nTexCount; i ++ )
	{
		unsigned char*	pData = s_cSkinLoader.GetTextureSource( i, &nLen );
		D3DXIMAGE_INFO	Info;
		m_ppTB[i] = NULL;
		D3DXCreateTextureFromFileInMemoryEx( D3DFRAME->GetDevice(), pData, nLen, 0, 0, 0, D3DUSAGE_RENDERTARGET, D3DFMT_DXT5, D3DPOOL_DEFAULT, D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR, 0, &Info, NULL, &m_ppTB[i] );
	}

	// 오브젝트들을 생성한다.
	// 실제로 로더는 비디오메모리버퍼를 가지고 있는것이 아니라
	// 버텍스형식, 인덱스 형식의 시스템 버퍼를 가지고 있으므로
	// 비디오 메모리에 얹어두는 작업을 해준다.
	// 로더는 탄젠트 버퍼를 가지고 있지 않으므로, 탄젠트 버퍼는 따로 만들어준다.
	for ( i = 0; i < nObjCount; i ++ )
	{
		m_pbShow[i] = true;

		PSKINOBJECT	pObj = s_cSkinLoader.GetObjectSource(i);
		DevideBonesByObject( i );

		m_pnVNum[i] = pObj->nVCount;
		m_pnINum[i] = pObj->nICount;
		void*	pVData = NULL;
		void*	pIData = NULL;
		void*	pTData = NULL;
		D3DFRAME->GetDevice()->CreateVertexBuffer	( m_pnVNum[i] * sizeof(SCUSTOMSKINVERTEX),	0, 0, D3DPOOL_DEFAULT, &m_ppVB[i], NULL );				// 버텍스 버퍼
		D3DFRAME->GetDevice()->CreateVertexBuffer	( m_pnVNum[i] * sizeof(D3DXVECTOR3),		0, 0, D3DPOOL_DEFAULT, &m_ppVT[i], NULL );				// 버텍스의 탄젠트 버퍼.
		D3DFRAME->GetDevice()->CreateIndexBuffer	( m_pnINum[i] * sizeof(unsigned short),		0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_ppIB[i], NULL );	// 인덱스 버퍼

		m_ppVB[i]->Lock( 0, 0, &pVData, D3DLOCK_DISCARD );
		m_ppVT[i]->Lock( 0, 0, &pTData, D3DLOCK_DISCARD );
		m_ppIB[i]->Lock( 0, 0, &pIData, D3DLOCK_DISCARD );
		memcpy ( pVData, pObj->pVertices, sizeof(SCUSTOMSKINVERTEX)*m_pnVNum[i]);
		memcpy ( pIData, pObj->pIndices, sizeof(unsigned short)*m_pnINum[i]);
		// 로더는 탄젠트 버퍼를 가지고 있지 않으므로, 탄젠트 버퍼는 따로 만들어준다.
		// 탄젠트 버퍼 생성.
		CreateTangentSpace( m_pnVNum[i], (SCUSTOMSKINVERTEX*)pVData, m_pnINum[i], (unsigned short*)pIData, (LPD3DXVECTOR3)pTData );
		m_ppIB[i]->Unlock();
		m_ppVT[i]->Unlock();
		m_ppVB[i]->Unlock();

		m_pnMatID[i] = pObj->uMatIndex;	// 오브젝트에 사용되는 재질의 ID.
	}

	// 현재 파일은 애니메이션과 합해져 있는 형태이므로 애니메이션도 읽어들인다.
	// 뼈대 갯수만큼 루프를 돌며 각각 뼈대마다 애니메이션 데이터를 읽는다.
	// 애니메이션 데이터는 사원수로 회전을 벡터로 위치를 저장하고 있다.
/*	뼈대 구조체는 SkinRenderer.h에 정의 되어있다.
	struct SBONES
	{
		std::string			strName;	// 뼈대의 이름.

		D3DXVECTOR3			vPivot0;	// 최초 프레임에 저장되어있는 뼈대의 회전기준점.
		D3DXMATRIX			mBasis;		// 해당 뼈대가 최초 프레임에 놓여진 형태.

		int					nParent;	// 부모 뼈대의 번호. -1이 있으면 부모가 없는 뼈대.

		LPD3DXQUATERNION	pqRot;		// 프레임수만큼 저장된 회전값들.
		LPD3DXVECTOR3		pvMov;		// 프레임수만큼 저장된 이동값들.

	// 실제 파일에서 불러오지는 않지만 애니메이션시 필요한 인자들.
	// 해당 프레임에서 보간이 되어진 계산값들.
		bool				bCalculated;// 재귀호출시 계산이 끝난 상태라면 그냥 넘어가게 한다.
		D3DXMATRIX			mInvBasis;	// mBasis의 역행렬. 각 뼈대를 회전할 때 축정렬을 해주기 위함.
		D3DXMATRIX			dmSumRot;	// 부모부터 누적된 자기 자신의 최종 회전값을 행렬로 저장.
		D3DXQUATERNION		qSumRot;	// 부모부터 누적된 자기 자신의 최종 회전값을 사원수로 저장.
		D3DXVECTOR3			vMov;		// 부모부터 누적된 자기 자신의 최종 이동값.
		D3DXMATRIX			mWorld;		// 이동값과 회전값이 모두 적용된 최종 행렬.
		D3DXVECTOR3			vPivot;		// 애니메이션의 된 후 이동되어 있는 회전 기준점.
		D3DXQUATERNION		qCurRot;	// 해당 프레임에서 사진의 로컬 회전값.
	}
*/
	for ( i = 0; i < nBoneCount; i ++ )
	{
		PBONE	pBone = s_cSkinLoader.GetBoneSource(i);
		D3DXMatrixIdentity (&m_pBone[i].mInvBasis);
//		D3DXMatrixIdentity (&m_pmWorlds[i]);
//		D3DXQuaternionIdentity (&m_pBone[i].qCurRot);
//		D3DXMatrixIdentity (&m_pBone[i].dmSumRot);
//		D3DXQuaternionIdentity (&m_pBone[i].qSumRot );
//		m_pBone[i].vMov		= D3DXVECTOR3(0.0f, 0.0f, 0.0f);
//		m_pBone[i].vPivot	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		m_pBone[i].strName	= pBone->strName;
		m_pBone[i].nParent = pBone->nParent;

		m_pBone[i].pqRot = new D3DXQUATERNION	[nKeyCount];
		m_pBone[i].pvMov = new D3DXVECTOR3		[nKeyCount];
		memcpy ( &m_pBone[i].vPivot0,	&pBone->vPivot0,sizeof(float3) );
		memcpy ( &m_pBone[i].mBasis,	&pBone->mBasis,	sizeof(float)*16);
		memcpy ( m_pBone[i].pqRot,		pBone->pqRot,	sizeof(float4) * nKeyCount);
		memcpy ( m_pBone[i].pvMov,		pBone->pvMov,	sizeof(float3) * nKeyCount);
		D3DXMatrixInverse( &m_pBone[i].mInvBasis, NULL, &m_pBone[i].mBasis );
		for ( int j = 0; j < nKeyCount; j ++ ) D3DXQuaternionNormalize ( &(m_pBone[i].pqRot[j]), &(m_pBone[i].pqRot[j]));
		D3DXQuaternionRotationMatrix( &m_pBone[i].qInvBasis, &m_pBone[i].mInvBasis );
		D3DXQuaternionRotationMatrix( &m_pBone[i].qBasis, &m_pBone[i].mBasis );
//		D3DXQuaternionIdentity ( &m_pBone[i].qRotAfterTM );
//		D3DXQuaternionIdentity ( &m_pBone[i].qRotBeforeTM );
//		m_pBone[i].mRotAfterTM = IdentityMatrix;
//		m_pBone[i].mRotBeforeTM = IdentityMatrix;
		std::transform( m_pBone[i].strName.begin(), m_pBone[i].strName.end(), m_pBone[i].strName.begin(), toupper);
	}

	// 일이 끝난 로더는 해제하면 된다.
	s_cSkinLoader.Release	();
	return true;
}

// 실제 오브젝트에 할당된 본들만 추려낸다.
void			CSkinRenderer::DevideBonesByObject		( int idx )
{
	int					i = 0, j = 0;
	char				cBoneIdx[4] = {-1, -1, -1, -1};
	bool*				pbInserted	= new bool [nBoneCount];
	byte*				pbyInserted = new byte [nBoneCount];
	PSKINOBJECT			pObj = s_cSkinLoader.GetObjectSource(idx);
	PCUSTOMSKINVERTEX	pVert= pObj->pVertices;

	memset ( pbInserted, 0, nBoneCount );

	for ( i = 0; i < pObj->nVCount; i ++ )
	{
		cBoneIdx[0] = byte((pVert[i].dwWeightIndex<<24)>>24);
		cBoneIdx[1] = byte((pVert[i].dwWeightIndex<<16)>>24);
		cBoneIdx[2] = byte((pVert[i].dwWeightIndex<<8 )>>24);
		cBoneIdx[3] = byte((pVert[i].dwWeightIndex    )>>24);
		for ( j = 0; j < 4; j ++ )
		{
			byte	byInsertedNumber = m_pVecLocalBoneIndex[idx].size();
			if ( pbInserted[cBoneIdx[j]] )
			{
				cBoneIdx[j] = pbyInserted[cBoneIdx[j]];
				continue;
			}

			pbInserted[cBoneIdx[j]] = true;
			pbyInserted[cBoneIdx[j]]= byInsertedNumber;
			m_pVecLocalBoneIndex[idx].push_back(cBoneIdx[j]);
			cBoneIdx[j] = byInsertedNumber;
		}
		unsigned int dwBoneIdx[4] = {cBoneIdx[0], cBoneIdx[1], cBoneIdx[2], cBoneIdx[3]};
		pVert[i].dwWeightIndex = dwBoneIdx[0] | (dwBoneIdx[1]<<8) | (dwBoneIdx[2]<<16) | (dwBoneIdx[3]<<24);
	}
}

void			CSkinRenderer::Free		( void )
{
	int	i = 0;
	for ( i = 0; i < nTexCount; i ++ )			RELEASE_NODEL(m_ppTB[i]);
	for ( i = 0; i < nObjCount; i ++ )			RELEASE_NODEL(m_ppVB[i]);
	for ( i = 0; i < nObjCount; i ++ )			RELEASE_NODEL(m_ppVT[i]);
	for ( i = 0; i < nObjCount; i ++ )			RELEASE_NODEL(m_ppIB[i]);
	for ( i = 0; i < nObjCount; i ++ )			m_pVecLocalBoneIndex[i].clear();
	for ( i = 0; i < nBoneCount; i ++ )
	{
		DELETE_ARRAY ( m_pBone[i].pqRot );
		DELETE_ARRAY ( m_pBone[i].pvMov );
	}
	DELETE_ARRAY(m_pVecLocalBoneIndex);
//	DELETE_ARRAY(m_pvSingleMotionPivot);
//	DELETE_ARRAY(m_pqSingleMotionRotate);
	DELETE_ARRAY(m_pbShow);
//	DELETE_ARRAY(m_pmWorlds);
	DELETE_ARRAY(m_ppTB);
	DELETE_ARRAY(m_ppVB);
	DELETE_ARRAY(m_ppVT);
	DELETE_ARRAY(m_ppIB);
	DELETE_ARRAY(m_pnMatID);
	DELETE_ARRAY(m_pnVNum);
	DELETE_ARRAY(m_pnINum);
	DELETE_ARRAY(m_pBone);
	DELETE_ARRAY(m_pMat);
}

// 노말맵을 위해 탄젠트 벡터를 구하는 함수. 로딩시에 한번만 실행된다.
void			CSkinRenderer::CreateTangentSpace	( int nVCount, PCUSTOMSKINVERTEX pVIn, int nICount, LPWORD pIIn, LPD3DXVECTOR3 pTan )
{
	if ( nVCount < 3 || nICount < 3 )		return;
	if ( !pVIn || !pIIn )					return;

	ZeroMemory ( pTan, sizeof(D3DXVECTOR3)*nVCount );

	int			i, j;
	for ( i = 0; i < nICount; i += 3 )
	{
		for ( j = 0; j < 3; j ++ )
		{
			int		a1 = (j+0)%3+i;
			int		a2 = (j+1)%3+i;
			int		a3 = (j+2)%3+i;
			int		V1 = pIIn[a1];
			int		V2 = pIIn[a2];
			int		V3 = pIIn[a3];

			float		du1 = pVIn[V2].u - pVIn[V1].u;
			float		du2 = pVIn[V3].u - pVIn[V1].u;
			float		dv1 = pVIn[V2].v - pVIn[V1].v;
			float		dv2 = pVIn[V3].v - pVIn[V1].v;
			D3DXVECTOR3	dP1	= D3DXVECTOR3(pVIn[V2].x, pVIn[V2].y, pVIn[V2].z) - D3DXVECTOR3( pVIn[V1].x, pVIn[V1].y, pVIn[V1].z);
			D3DXVECTOR3	dP2	= D3DXVECTOR3(pVIn[V3].x, pVIn[V3].y, pVIn[V3].z) - D3DXVECTOR3( pVIn[V1].x, pVIn[V1].y, pVIn[V1].z);
			D3DXVECTOR3	T	= -(dP2 * du1) + (dP1 * du2);
			D3DXVECTOR3	B	= (dP2 * dv1) - (dP1 * dv2);
			D3DXVec3Cross		(&T, &B, &D3DXVECTOR3(pVIn[V1].nx, pVIn[V1].ny, pVIn[V1].nz));
			D3DXVec3Cross		(&B, &D3DXVECTOR3(pVIn[V1].nx, pVIn[V1].ny, pVIn[V1].nz), &T);
			D3DXVec3Normalize	(&T, &T);
			D3DXVec3Normalize	(&B, &B);
			(pTan)[V1].x = (pTan)[V1].x+T.x;
			(pTan)[V1].y = (pTan)[V1].y+T.y;
			(pTan)[V1].z = (pTan)[V1].z+T.z;
		}
	}
	for ( i = 0; i < nVCount; i ++ )
	{
//		D3DXVECTOR3	T	= D3DXVECTOR3( (pTan)[i].x, (pTan)[i].y, (pTan)[i].z);
		D3DXVec3Normalize(&pTan[i], &pTan[i]);
	}
}

// 그린다.
// UpdateFrame을 내부에서 호출을 한다.
//	저장된 애니메이션 말고 사용자가 위치 회전을 적용하는 부분도 있음.
void			CSkinRenderer::Render	( bool BeHitCheck, float BuffCheck )
{
	int				i = 0;
	unsigned int	uPass = 0;
	HRESULT			hResult = 0;
	D3DXMATRIX		ViewProj = D3DFRAME->m_mView * D3DFRAME->m_mProj;

	hResult = D3DFRAME->GetDevice()->SetStreamSource	( 0, NULL, 0, 0 );
	hResult = D3DFRAME->GetDevice()->SetStreamSource	( 1, NULL, 0, 0 );
//	UpdateFrame();	// 현재 프레임에서 적용된 애니메이션들을 m_pmWorlds에 계산해 넣는 작업을 한다.
	D3DXVECTOR3	vLightDir	= D3DFRAME->GetLightDir();
	D3DXVECTOR3	vCameraPos	= CAMMGR->GetPosition();

	if( RSCMGR->m_bCHAR_MGR == true && BeHitCheck == true )
	{
		m_vColor	=	D3DXVECTOR3( 0.5f, 0.0f, 0.0f );
	}
	else
		m_vColor	=	D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	hResult = D3DFRAME->GetDevice()->SetVertexDeclaration( s_pVDecl );

	//	무엇을 쓸지
	if ( D3DFRAME->m_bShadow )
	{
		hResult = s_pEffect->SetTechnique	( "createshadow" );
	}
	else
	{
		hResult = s_pEffect->SetTechnique	( "render" );
	}
	//	초기화
	hResult = s_pEffect->SetMatrix		( "view", &D3DFRAME->m_mView );	
	hResult = s_pEffect->SetMatrix		( "proj", &D3DFRAME->m_mProj );
	hResult = s_pEffect->SetMatrix		( "viewproj", &ViewProj );
	hResult = s_pEffect->SetFloatArray	( "lightDir", vLightDir, 3 );
	hResult = s_pEffect->SetFloatArray	( "vCameraPos", vCameraPos, 3 );
	hResult	= s_pEffect->SetFloatArray	( "AtteckCheck", m_vColor, 3 );
	hResult = s_pEffect->SetFloat		( "buffcheck", 0.0f );

	static float fbcheck = 0.0f;
	float fsum = 0.006;
	static bool bcheck = false;
	if( BuffCheck != 0 )
	{
		fbcheck <= 0.0f ? bcheck = true : bcheck;
		fbcheck >= 0.6f ? bcheck = false : bcheck;
		if ( !bcheck )
			fsum *= -1;
		if ( bcheck )
			abs(fsum);
		hResult = s_pEffect->SetFloat		( "buffcheck", fbcheck );
		fbcheck += fsum;
		int Buff = BuffCheck;
		switch( Buff )
		{
		case 1:
			hResult	= s_pEffect->SetFloatArray	( "SubColor", 
												D3DXVECTOR3(0.5f,0.0f, 0.0f), 3 );
			break;
		case 2:
			hResult	= s_pEffect->SetFloatArray	( "SubColor", 
												D3DXVECTOR3(0.5f,0.5f, 0.0f), 3 );
			break;
		case 3:
			hResult	= s_pEffect->SetFloatArray	( "SubColor", 
												D3DXVECTOR3(0.0f,0.5f, 0.0f), 3);
			break;
		case 4:
			hResult	= s_pEffect->SetFloatArray	( "SubColor", 
												D3DXVECTOR3(0.6f,0.3f, 0.0f), 3);
			break;
		}
	}

	//그림자 부분
	s_pEffect->SetTexture	( "shadowTx", D3DFRAME->m_pShadowDep );
	D3DXMATRIX	matrix = D3DFRAME->m_mLView * D3DFRAME->m_mLProj * D3DFRAME->m_mLBias; //직교투영시 -1~1까지 값이 되므로 0부터 1까지하려면 1을더한후 /2를 해주자 
	s_pEffect->SetMatrix	( "matShadow", &matrix );
	//그림자 부분
	D3DXMATRIX		mRootPos;
	D3DXMATRIX		mRootRot;
	D3DXMATRIX		mRootScl;
	//D3DXMATRIX		mRootWorld;
//	저장된 애니메이션 말고 사용자가 위치 회전을 적용하는 부분.
	D3DXMatrixTranslation						( &mRootPos, m_vPos.x, m_vPos.y, m_vPos.z );
	D3DXMatrixScaling							( &mRootScl, m_vScl.x, m_vScl.y, m_vScl.z );
	mRootRot	= D3DFRAME->MatrixFromVector	( m_vRot );

	mRootWorld = mRootScl * mRootRot * mRootPos;

	for ( i = nObjCount-1; i >= 0; i -- )
	{
		if ( !m_pbShow[i] )	continue;
		std::vector<D3DXMATRIX>	matrix;
		int						nReplacedBoneCount = m_pVecLocalBoneIndex[i].size();

		// 여긴.. 뭐. 나중에 최적화 하자.
		for ( int m = 0; m < nReplacedBoneCount; m ++ )		matrix.push_back(m_pmWorlds[m_pVecLocalBoneIndex[i][m]]);
		hResult = s_pEffect->SetMatrixArray					( "Transforms", &matrix[0], nReplacedBoneCount );

		hResult = s_pEffect->SetFloat						( "fStrength", m_pMat[m_pnMatID[i]].Shininess*100 );
		hResult = s_pEffect->SetFloat						( "fSpecular", m_pMat[m_pnMatID[i]].Shine_Str );
		hResult = s_pEffect->SetMatrix						( "world",&mRootWorld );
		hResult = D3DFRAME->GetDevice()->SetStreamSource	( 0, m_ppVB[i], 0, sizeof(SCUSTOMSKINVERTEX) );
		hResult = D3DFRAME->GetDevice()->SetStreamSource	( 1, m_ppVT[i], 0, sizeof(D3DXVECTOR3) );
		hResult = D3DFRAME->GetDevice()->SetIndices			( m_ppIB[i] );
		hResult = s_pEffect->SetTexture						( "texD", (m_pMat[m_pnMatID[i]].nTextureIdx[0] > -1)?m_ppTB[m_pMat[m_pnMatID[i]].nTextureIdx[0]]:NULL );
		if ( !(GetAsyncKeyState(VK_NUMPAD9)&0x8000) )
		{
			hResult = s_pEffect->SetTexture					( "texN", (m_pMat[m_pnMatID[i]].nTextureIdx[1] > -1)?m_ppTB[m_pMat[m_pnMatID[i]].nTextureIdx[1]]:NULL );
			hResult = s_pEffect->SetBool					( "bNor", (m_pMat[m_pnMatID[i]].nTextureIdx[1] > -1) );
		}else
		{
			hResult = s_pEffect->SetTexture					( "texN", NULL );
			hResult = s_pEffect->SetBool					( "bNor", FALSE );
		}

		//	셰이더 적용
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

void	CSkinRenderer::SetPos		( D3DXVECTOR3 vPos )
{
	m_vPos = vPos;
}

void	CSkinRenderer::SetRot		( D3DXVECTOR3 vRot )
{
	m_vRot = vRot;
}

void	CSkinRenderer::SetScl		( D3DXVECTOR3 vScl )
{
	m_vScl = vScl;
}

void			CSkinRenderer::HideObject		( int nObjNo, bool dHide )
{
	int i = 0;
	if ( nObjNo == -1 )	for ( i = 0; i < nObjCount; i ++ )	m_pbShow[i]		= !dHide;
	else													m_pbShow[nObjNo]= !dHide;
}

void			CSkinRenderer::ShowObject		( int nObjNo, bool dShow )
{
	int i = 0;
	if ( nObjNo == -1 )	for ( i = 0; i < nObjCount; i ++ )	m_pbShow[i]		= dShow;
	else													m_pbShow[nObjNo]= dShow;
}

int				CSkinRenderer::GetObjNoFromName( char* szName )
{
	int	i = 0;
	std::string	strName;
	for ( i = 0; i < nObjCount; i ++ )
	{
	}
	return -1;
}

int				CSkinRenderer::GetBoneNoFromName( char* szName )
{
	int	i = 0;
	std::string	strName;
	for ( i = 0; i < nBoneCount; i ++ )
	{
		if ( m_pBone[i].strName.compare(szName) == 0 ) return i;
	}
	return -1;
}

