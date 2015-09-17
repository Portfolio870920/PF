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
	int		nRange		= m_nFrameD - m_nFrameS;												// ������ ����.
	float	fTime		= (float)(fTimeGetTime()-m_dwStart) / (m_fSpeed * 1000.0f / nRange);	// ���ص� �ð��� ���� ���� ������ ���.

	// ���� ������ ����.
	m_fCurFrame= fTime;

	// ������ ���� ���� ������.
	m_nThisFrame	= ((int)m_fCurFrame);

	// ���� �����ӿ��� ���� ������ ������ �Ҽ���.(0~1.0f)
	m_fSlerp		= m_fCurFrame - m_nThisFrame;

	// fTimeGetTime()���� ����ϹǷ� ���������� ��ȯ�ϰ� ����.
	m_nThisFrame	= ((int)m_fCurFrame)%(nRange) + m_nFrameS;

	// �����ִϸ��̼� ���� ������ ���.
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
		// ������ ��������� �������ӿ��� ����.
		int		nRange		= m_nFrameD - m_nFrameS;									// ������ ����.
		float	fTime		= (float)(fTimeGetTime()-m_dwStart) / (m_fSpeed * 1000.0f / nRange);	// ���ص� �ð��� ���� ���� ������ ���.

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
	s_cSkinLoader.Initial	( pFileName );			// �δ����� ��Ű�װ�ü�� �ε�.

	nMatCount	= s_cSkinLoader.GetMaterialCount();	// �ε��� �����Ϳ��� ������ ������ ���´�.
	nTexCount	= s_cSkinLoader.GetTextureCount();	// �ε��� �����Ϳ��� �ؽ�ó�� ������ ���´�.
	nObjCount	= s_cSkinLoader.GetObjectCount();	// �ε��� �����Ϳ��� ������Ʈ�� ������ ���´�.
	nBoneCount	= s_cSkinLoader.GetBoneCount();		// �ε��� �����Ϳ��� ������ ������ ���´�.
	nKeyCount	= s_cSkinLoader.GetFrameCount();	// �ε��� �����Ϳ��� �ִϸ��̼� �����Ӽ��� ���´�.

	m_vRot		= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );	// ȸ���� �ʱ�ȭ.
	m_vPos		= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );	// ��ġ�� �ʱ�ȭ.
	m_vScl		= D3DXVECTOR3( 1.0f, 1.0f, 1.0f );	// ��ô�� �ʱ�ȭ.
	D3DXMatrixIdentity ( &m_mWorld );				// ������� �ʱ�ȭ.

	m_ppTB		= new LPDIRECT3DTEXTURE9		[nTexCount];	// ���� �ؽ�ó ������ŭ �ؽ�ó ��ü�� ����.
	m_ppVB		= new LPDIRECT3DVERTEXBUFFER9	[nObjCount];	// ���� ������Ʈ ������ŭ ���ؽ����۸� ����.
	m_ppVT		= new LPDIRECT3DVERTEXBUFFER9	[nObjCount];	// ���� ������Ʈ ������ŭ ź��Ʈ���۸� ����. -> �븻���� �̿��ϱ� ����.
	m_ppIB		= new LPDIRECT3DINDEXBUFFER9	[nObjCount];	// ���� ������Ʈ ������ŭ �ε������۸� ����.
	m_pnMatID	= new int						[nObjCount];	// ���� ������Ʈ ������ŭ ���� ID�� ����.
	m_pnVNum	= new int						[nObjCount];	// ���� ������Ʈ ������ŭ ���ؽ� ī��Ʈ�� ����.
	m_pnINum	= new int						[nObjCount];	// ���� ������Ʈ ������ŭ �ε��� ī��Ʈ�� ����.
	m_pbShow	= new bool						[nObjCount];
	m_pBone		= new SBONES					[nBoneCount];	// ���� ���� ������ŭ �������� ��ü�� ����.
	m_pMat		= new SKINMATERIAL				[nMatCount];	// ���� ���� ������ŭ �������� ��ü�� ����.
//	m_pmWorlds	= new D3DXMATRIX				[nBoneCount];	// ���� ���� ������ŭ ������������� ����.

//	m_pvSingleMotionPivot	= new D3DXVECTOR3	[nBoneCount];	// ���1���� ���� ���ø�� �ǹ�.
//	m_pqSingleMotionRotate	= new D3DXQUATERNION[nBoneCount];	// ���1���� ���� ���ø�� ȸ����.

	m_pVecLocalBoneIndex	= new std::vector<byte>[nObjCount];	// ������Ʈ1���� ��������ǰ��ִ� ������ �ε���.
	// ������ �����Ѵ�.
	for ( i = 0; i < nMatCount; i ++ )
	{
/*
		SkinLoader�� ���� �Ǿ�����.
		struct SKINMATERIAL 
		{
			float	a, r, g, b;		-> Alpha, Red, Green, Blue
			float	Shininess;		-> ���϶���Ʈ ����.
			float	Shine_Str;		-> ���϶���Ʈ�� ������. -> ���� Ŀ������ ���� ������ ���� �ȴ�.

			int		nTextureIdx[2];	-> ���� �ؽ�ó�� ID. [0]�� ��ǻ���ؽ�ó, [1]�� �븻��. �ƹ��͵� ������� ������ -1�� ����ִ�.
		};
*/

		PSKINMATERIAL	pData = s_cSkinLoader.GetMaterialSource( i );
		memcpy ( &m_pMat[i], pData, sizeof(SKINMATERIAL));	// ��ü ũ�Ⱑ �����Ƿ� ī�Ǹ����� ���� ����.
	}

	// �ؽ�ó�� �����Ѵ�.
	// �δ��� �ؽ�ó�� ���� ���� �״�� ���ۿ� �����ϰ� �����Ƿ� D3DXCreateTextureFromFileInMemory �� �̿��Ѵ�.
	for ( i = 0; i < nTexCount; i ++ )
	{
		unsigned char*	pData = s_cSkinLoader.GetTextureSource( i, &nLen );
		D3DXIMAGE_INFO	Info;
		m_ppTB[i] = NULL;
		D3DXCreateTextureFromFileInMemoryEx( D3DFRAME->GetDevice(), pData, nLen, 0, 0, 0, D3DUSAGE_RENDERTARGET, D3DFMT_DXT5, D3DPOOL_DEFAULT, D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR, 0, &Info, NULL, &m_ppTB[i] );
	}

	// ������Ʈ���� �����Ѵ�.
	// ������ �δ��� �����޸𸮹��۸� ������ �ִ°��� �ƴ϶�
	// ���ؽ�����, �ε��� ������ �ý��� ���۸� ������ �����Ƿ�
	// ���� �޸𸮿� ���δ� �۾��� ���ش�.
	// �δ��� ź��Ʈ ���۸� ������ ���� �����Ƿ�, ź��Ʈ ���۴� ���� ������ش�.
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
		D3DFRAME->GetDevice()->CreateVertexBuffer	( m_pnVNum[i] * sizeof(SCUSTOMSKINVERTEX),	0, 0, D3DPOOL_DEFAULT, &m_ppVB[i], NULL );				// ���ؽ� ����
		D3DFRAME->GetDevice()->CreateVertexBuffer	( m_pnVNum[i] * sizeof(D3DXVECTOR3),		0, 0, D3DPOOL_DEFAULT, &m_ppVT[i], NULL );				// ���ؽ��� ź��Ʈ ����.
		D3DFRAME->GetDevice()->CreateIndexBuffer	( m_pnINum[i] * sizeof(unsigned short),		0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_ppIB[i], NULL );	// �ε��� ����

		m_ppVB[i]->Lock( 0, 0, &pVData, D3DLOCK_DISCARD );
		m_ppVT[i]->Lock( 0, 0, &pTData, D3DLOCK_DISCARD );
		m_ppIB[i]->Lock( 0, 0, &pIData, D3DLOCK_DISCARD );
		memcpy ( pVData, pObj->pVertices, sizeof(SCUSTOMSKINVERTEX)*m_pnVNum[i]);
		memcpy ( pIData, pObj->pIndices, sizeof(unsigned short)*m_pnINum[i]);
		// �δ��� ź��Ʈ ���۸� ������ ���� �����Ƿ�, ź��Ʈ ���۴� ���� ������ش�.
		// ź��Ʈ ���� ����.
		CreateTangentSpace( m_pnVNum[i], (SCUSTOMSKINVERTEX*)pVData, m_pnINum[i], (unsigned short*)pIData, (LPD3DXVECTOR3)pTData );
		m_ppIB[i]->Unlock();
		m_ppVT[i]->Unlock();
		m_ppVB[i]->Unlock();

		m_pnMatID[i] = pObj->uMatIndex;	// ������Ʈ�� ���Ǵ� ������ ID.
	}

	// ���� ������ �ִϸ��̼ǰ� ������ �ִ� �����̹Ƿ� �ִϸ��̼ǵ� �о���δ�.
	// ���� ������ŭ ������ ���� ���� ���븶�� �ִϸ��̼� �����͸� �д´�.
	// �ִϸ��̼� �����ʹ� ������� ȸ���� ���ͷ� ��ġ�� �����ϰ� �ִ�.
/*	���� ����ü�� SkinRenderer.h�� ���� �Ǿ��ִ�.
	struct SBONES
	{
		std::string			strName;	// ������ �̸�.

		D3DXVECTOR3			vPivot0;	// ���� �����ӿ� ����Ǿ��ִ� ������ ȸ��������.
		D3DXMATRIX			mBasis;		// �ش� ���밡 ���� �����ӿ� ������ ����.

		int					nParent;	// �θ� ������ ��ȣ. -1�� ������ �θ� ���� ����.

		LPD3DXQUATERNION	pqRot;		// �����Ӽ���ŭ ����� ȸ������.
		LPD3DXVECTOR3		pvMov;		// �����Ӽ���ŭ ����� �̵�����.

	// ���� ���Ͽ��� �ҷ������� ������ �ִϸ��̼ǽ� �ʿ��� ���ڵ�.
	// �ش� �����ӿ��� ������ �Ǿ��� ��갪��.
		bool				bCalculated;// ���ȣ��� ����� ���� ���¶�� �׳� �Ѿ�� �Ѵ�.
		D3DXMATRIX			mInvBasis;	// mBasis�� �����. �� ���븦 ȸ���� �� �������� ���ֱ� ����.
		D3DXMATRIX			dmSumRot;	// �θ���� ������ �ڱ� �ڽ��� ���� ȸ������ ��ķ� ����.
		D3DXQUATERNION		qSumRot;	// �θ���� ������ �ڱ� �ڽ��� ���� ȸ������ ������� ����.
		D3DXVECTOR3			vMov;		// �θ���� ������ �ڱ� �ڽ��� ���� �̵���.
		D3DXMATRIX			mWorld;		// �̵����� ȸ������ ��� ����� ���� ���.
		D3DXVECTOR3			vPivot;		// �ִϸ��̼��� �� �� �̵��Ǿ� �ִ� ȸ�� ������.
		D3DXQUATERNION		qCurRot;	// �ش� �����ӿ��� ������ ���� ȸ����.
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

	// ���� ���� �δ��� �����ϸ� �ȴ�.
	s_cSkinLoader.Release	();
	return true;
}

// ���� ������Ʈ�� �Ҵ�� ���鸸 �߷�����.
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

// �븻���� ���� ź��Ʈ ���͸� ���ϴ� �Լ�. �ε��ÿ� �ѹ��� ����ȴ�.
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

// �׸���.
// UpdateFrame�� ���ο��� ȣ���� �Ѵ�.
//	����� �ִϸ��̼� ���� ����ڰ� ��ġ ȸ���� �����ϴ� �κе� ����.
void			CSkinRenderer::Render	( bool BeHitCheck, float BuffCheck )
{
	int				i = 0;
	unsigned int	uPass = 0;
	HRESULT			hResult = 0;
	D3DXMATRIX		ViewProj = D3DFRAME->m_mView * D3DFRAME->m_mProj;

	hResult = D3DFRAME->GetDevice()->SetStreamSource	( 0, NULL, 0, 0 );
	hResult = D3DFRAME->GetDevice()->SetStreamSource	( 1, NULL, 0, 0 );
//	UpdateFrame();	// ���� �����ӿ��� ����� �ִϸ��̼ǵ��� m_pmWorlds�� ����� �ִ� �۾��� �Ѵ�.
	D3DXVECTOR3	vLightDir	= D3DFRAME->GetLightDir();
	D3DXVECTOR3	vCameraPos	= CAMMGR->GetPosition();

	if( RSCMGR->m_bCHAR_MGR == true && BeHitCheck == true )
	{
		m_vColor	=	D3DXVECTOR3( 0.5f, 0.0f, 0.0f );
	}
	else
		m_vColor	=	D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	hResult = D3DFRAME->GetDevice()->SetVertexDeclaration( s_pVDecl );

	//	������ ����
	if ( D3DFRAME->m_bShadow )
	{
		hResult = s_pEffect->SetTechnique	( "createshadow" );
	}
	else
	{
		hResult = s_pEffect->SetTechnique	( "render" );
	}
	//	�ʱ�ȭ
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

	//�׸��� �κ�
	s_pEffect->SetTexture	( "shadowTx", D3DFRAME->m_pShadowDep );
	D3DXMATRIX	matrix = D3DFRAME->m_mLView * D3DFRAME->m_mLProj * D3DFRAME->m_mLBias; //���������� -1~1���� ���� �ǹǷ� 0���� 1�����Ϸ��� 1�������� /2�� ������ 
	s_pEffect->SetMatrix	( "matShadow", &matrix );
	//�׸��� �κ�
	D3DXMATRIX		mRootPos;
	D3DXMATRIX		mRootRot;
	D3DXMATRIX		mRootScl;
	//D3DXMATRIX		mRootWorld;
//	����� �ִϸ��̼� ���� ����ڰ� ��ġ ȸ���� �����ϴ� �κ�.
	D3DXMatrixTranslation						( &mRootPos, m_vPos.x, m_vPos.y, m_vPos.z );
	D3DXMatrixScaling							( &mRootScl, m_vScl.x, m_vScl.y, m_vScl.z );
	mRootRot	= D3DFRAME->MatrixFromVector	( m_vRot );

	mRootWorld = mRootScl * mRootRot * mRootPos;

	for ( i = nObjCount-1; i >= 0; i -- )
	{
		if ( !m_pbShow[i] )	continue;
		std::vector<D3DXMATRIX>	matrix;
		int						nReplacedBoneCount = m_pVecLocalBoneIndex[i].size();

		// ����.. ��. ���߿� ����ȭ ����.
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

		//	���̴� ����
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

