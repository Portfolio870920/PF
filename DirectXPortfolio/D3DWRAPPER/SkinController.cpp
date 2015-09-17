// SkinController.cpp: implementation of the CSkinController class.
//
//////////////////////////////////////////////////////////////////////

#include "D3DWRAPPER.h"
#include "SkinController.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkinController::CSkinController()
{
	m_fLastBlendingSpeed	= 1.0f;
	m_eUpdateStatus			= UPDATE_PLAYING;
	m_pListBones			= NULL;
	ZeroMemory	( &m_sReplaceMotion, sizeof(SMOTION) );
}

CSkinController::~CSkinController()
{
}

unsigned int	CSkinController::Init(CSkinRenderer* pSkinObj )
{
	int	nBoneCount = 0, i = 0;
	m_pSkinSource = pSkinObj;

	nBoneCount = m_pSkinSource->nBoneCount;

	m_pmWorlds				= new D3DXMATRIX				[nBoneCount];	// 얻어온 뼈대 갯수만큼 최종월드행렬을 생성.
	m_pBones				= new SBONE_INSTANCE			[nBoneCount];
	m_pvSingleMotionPivot	= new D3DXVECTOR3	[nBoneCount];	// 모션1개에 대한 로컬모션 피벗.
	m_pqSingleMotionRotate	= new D3DXQUATERNION[nBoneCount];	// 모션1개에 대한 로컬모션 회전값.


	for ( i = 0; i < nBoneCount; i ++ )
	{
		D3DXMatrixIdentity (&m_pmWorlds[i]);
		D3DXQuaternionIdentity (&m_pBones[i].qCurRot);
		D3DXMatrixIdentity (&m_pBones[i].dmSumRot);
		D3DXQuaternionIdentity (&m_pBones[i].qSumRot );
		m_pBones[i].vMov	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_pBones[i].vPivot	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		D3DXQuaternionIdentity ( &m_pBones[i].qRotAfterTM );
		D3DXQuaternionIdentity ( &m_pBones[i].qRotBeforeTM );
		m_pBones[i].mRotAfterTM = IdentityMatrix;
		m_pBones[i].mRotBeforeTM = IdentityMatrix;
	}

	m_vRot		= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );	// 회전값 초기화.
	m_vPos		= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );	// 위치값 초기화.
	m_vScl		= D3DXVECTOR3( 1.0f, 1.0f, 1.0f );	// 축척값 초기화.
	return 0UL;
}

void			CSkinController::Free( void )
{
	DELETE_ARRAY(m_pvSingleMotionPivot);
	DELETE_ARRAY(m_pqSingleMotionRotate);
	DELETE_ARRAY(m_pBones);
	DELETE_ARRAY(m_pmWorlds);
}

void			CSkinController::UpdateWorldMatrix		( void )
{
	int								i = 0;
	D3DXMATRIX						mRot, mPos, mInv, mMov;
	D3DXQUATERNION					qCurRot;

	// 계산이 되지 않은 상태로 설정.
	for ( i = 0; i < m_pSkinSource->nBoneCount; i ++ )	m_pBones[i].bCalculated = false;

	// 특정 본에 대체애니메이션이 존재하면 대체시킨다.
	if ( m_pListBones )					ReplaceBoneTransform	();
	CombineAfterTransform										();
	// 회전값만 부모뼈대와 합산한다.
	for ( i = 0; i < m_pSkinSource->nBoneCount; i ++ ) UnitedRotate ( i );

	// 회전을 합산한 후 원래 놓여져있던 상태의 역행렬을 곱해 축정렬을 한다.
	for ( i = 0; i < m_pSkinSource->nBoneCount; i ++ ) m_pBones[i].dmSumRot = m_pSkinSource->m_pBone[i].mInvBasis * m_pBones[i].dmSumRot;

	// 다시 계산되지 않은 상태로 설정.
	for ( i = 0; i < m_pSkinSource->nBoneCount; i ++ ) m_pBones[i].bCalculated = FALSE;

	// 부모 뼈대도 이동/회전을 했으므로 나의 회전축을 거기에 맞게 재설정.
	for ( i = 0; i < m_pSkinSource->nBoneCount; i ++ ) RepositionPivot ( i );

	// 구해진 뼈대회전축위치, 이동, 회전을 행렬 하나로 만든다.
	for ( i = 0; i < m_pSkinSource->nBoneCount; i ++ ) TransformObject ( i );

	// 쉐이더에 넣기 편하게 연속된 행렬버퍼에 넣는다.
	for ( i = 0; i < m_pSkinSource->nBoneCount; i ++ )	m_pmWorlds[i]	= m_pBones[i].mWorld;
}

void			CSkinController::UpdateSingleMotion		( std::list<SMOTION>::iterator* ppNode )
{
	int								i = 0;
	D3DXMATRIX						mRot, mPos, mInv, mMov;
	D3DXQUATERNION					qCurRot;
	std::list<SMOTION>::iterator	pNode	= *ppNode;
	SMOTION							m		= *pNode;

	for ( i = 0; i < m_pSkinSource->nBoneCount; i ++ )
	{
		m_pvSingleMotionPivot[i]	= m_pSkinSource->m_pBone[i].pvMov[m.m_nThisFrame] + (m_pSkinSource->m_pBone[i].pvMov[m.m_nNextFrame] - m_pSkinSource->m_pBone[i].pvMov[m.m_nThisFrame]) * m.m_fSlerp;
		D3DXQuaternionSlerp	( &m_pqSingleMotionRotate[i], &m_pSkinSource->m_pBone[i].pqRot[m.m_nThisFrame], &m_pSkinSource->m_pBone[i].pqRot[m.m_nNextFrame], m.m_fSlerp );
	}
}

D3DXQUATERNION	CSkinController::UnitedRotate			( int i )
{
	if ( i < 0 )							return D3DXQUATERNION ( 0.0f, 0.0f, 0.0f, 1.0f );
	if ( m_pBones[i].bCalculated == true )	return m_pBones[i].qSumRot;
	m_pBones[i].bCalculated = true;

	int			pi		= (m_pSkinSource->m_pBone[i].nParent == i)?-1:m_pSkinSource->m_pBone[i].nParent;

	D3DXQUATERNION qPRot = UnitedRotate ( pi );
	m_pBones[i].qSumRot = m_pBones[i].qCurRot * qPRot;

	D3DXMatrixRotationQuaternion	( &(m_pBones[i].dmSumRot), &(m_pBones[i].qSumRot) );
	D3DXVec3TransformCoord			( &m_pBones[i].vMov, &m_pBones[i].vMov, &m_pSkinSource->m_pBone[i].mBasis);

	return m_pBones[i].qSumRot;
}

D3DXVECTOR3		CSkinController::RepositionPivot		( int i )
{
	if ( i < 0 )								return D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
	if ( m_pBones[i].bCalculated == TRUE )		return m_pBones[i].vPivot;
	m_pBones[i].bCalculated = true;

	D3DXVECTOR3	vPivot (0.0f, 0.0f, 0.0f);
	int			pi		= (m_pSkinSource->m_pBone[i].nParent == i)?-1:m_pSkinSource->m_pBone[i].nParent;

	D3DXVECTOR3 PivotToBase = RepositionPivot ( pi );

	if ( m_pSkinSource->m_pBone[i].nParent >= 0 )	vPivot = m_pSkinSource->m_pBone[i].vPivot0 - m_pSkinSource->m_pBone[m_pSkinSource->m_pBone[i].nParent].vPivot0;
	else											vPivot = m_pSkinSource->m_pBone[i].vPivot0;

	D3DXMATRIX	dmRot = IdentityMatrix;
	D3DXVECTOR4	dv4Pivot;

	if ( pi >= 0 ) dmRot = m_pBones[pi].dmSumRot;

	D3DXVec3Transform			( &dv4Pivot, &(vPivot), &dmRot );
	D3DXVec3TransformCoord		( &m_pBones[i].vMov, &m_pBones[i].vMov, &m_pSkinSource->m_pBone[i].mInvBasis);

	m_pBones[i].vPivot.x = dv4Pivot.x;
	m_pBones[i].vPivot.y = dv4Pivot.y;
	m_pBones[i].vPivot.z = dv4Pivot.z;

	D3DXVECTOR3 vMov = m_pBones[i].vMov-m_pSkinSource->m_pBone[i].pvMov[0];

	m_pBones[i].vPivot += PivotToBase + vMov;
	return m_pBones[i].vPivot;
}

void			CSkinController::TransformObject		( int i )
{
	D3DXMATRIX	dmPos	= IdentityMatrix;
	D3DXMATRIX	dmRot	= IdentityMatrix;
	D3DXMATRIX	dmMov	= IdentityMatrix;
	D3DXMATRIX	dmBase	= IdentityMatrix;

	dmBase._41 = -(m_pSkinSource->m_pBone[i].vPivot0.x);
	dmBase._42 = -(m_pSkinSource->m_pBone[i].vPivot0.y);
	dmBase._43 = -(m_pSkinSource->m_pBone[i].vPivot0.z);

	dmPos._41 = m_pBones[i].vPivot.x;
	dmPos._42 = m_pBones[i].vPivot.y;
	dmPos._43 = m_pBones[i].vPivot.z;

	m_pBones[i].mWorld	= dmBase * m_pBones[i].dmSumRot * dmPos;
}

E_UPDATE_STATUS	CSkinController::UpdateFrameFoward		( PMOTION pMotion )
{
	E_UPDATE_STATUS	eUpdateStatus = UPDATE_PLAYING;
	int		nRange		= pMotion->m_nFrameD - pMotion->m_nFrameS;												// 프레임 길이.
	float	fTime		= (float)(fTimeGetTime()-pMotion->m_dwStart) / (pMotion->m_fSpeed * 1000.0f / nRange);	// 정해둔 시간에 맞춰 현재 프레임 계산.

	// 현재 프레임 저장.
	pMotion->m_fCurFrame= fTime;

	// 보간을 위한 이전 프레임.
	pMotion->m_nThisFrame	= ((int)pMotion->m_fCurFrame);

	// 이전 프레임에서 현재 프레임 까지의 소수점.(0~1.0f)
	pMotion->m_fSlerp		= pMotion->m_fCurFrame - pMotion->m_nThisFrame;

	// fTimeGetTime()으로 계산하므로 범위내에서 순환하게 재계산.
	pMotion->m_nThisFrame	= ((int)pMotion->m_fCurFrame)%(nRange) + pMotion->m_nFrameS;

	// 루프애니메이션 다음 프레임 계산.
	if ( (pMotion->m_nThisFrame+1) >= pMotion->m_nFrameD )
	{
		pMotion->m_nNextFrame	= pMotion->m_nFrameS;
		eUpdateStatus		= UPDATE_RETURN;
	}
	else
	{
		pMotion->m_nNextFrame	= pMotion->m_nThisFrame + 1;
		eUpdateStatus		= UPDATE_PLAYING;
	}
	return eUpdateStatus;
}

E_UPDATE_STATUS	CSkinController::UpdateFrameBackward	( PMOTION pMotion )
{
	E_UPDATE_STATUS	eUpdateStatus = UPDATE_PLAYING;
	int		nRange		= -(pMotion->m_nFrameD - pMotion->m_nFrameS);												// 프레임 길이.
	float	fTime		= 1.0f - (float)((int)(fTimeGetTime()-pMotion->m_dwStart)%(DWORD)(pMotion->m_fSpeed * 1000.0f))/(pMotion->m_fSpeed * 1000.0f);	// 정해둔 시간에 맞춰 현재 프레임 계산.
	fTime				*= nRange;

	// 현재 프레임 저장.
	pMotion->m_fCurFrame= fTime;

	// 보간을 위한 이전 프레임.
	pMotion->m_nThisFrame	= ((int)pMotion->m_fCurFrame);

	// 이전 프레임에서 현재 프레임 까지의 소수점.(0~1.0f) 시작 프레임이 끝프레임보다 클경우(역방향 재생) 반대로 계산한다.
	pMotion->m_fSlerp		= 1.0f-(pMotion->m_fCurFrame - pMotion->m_nThisFrame);

	// fTimeGetTime()으로 계산하므로 범위내에서 순환하게 재계산.
	pMotion->m_nThisFrame	= ((int)pMotion->m_fCurFrame)%(nRange) + pMotion->m_nFrameD;

	// 루프애니메이션 다음 프레임 계산.
	if ( (pMotion->m_nThisFrame-0) <= pMotion->m_nFrameD )
	{
		pMotion->m_nNextFrame	= pMotion->m_nFrameS-1;
		eUpdateStatus		= UPDATE_RETURN;
	}
	else
	{
		pMotion->m_nNextFrame	= pMotion->m_nThisFrame - 1;
		eUpdateStatus		= UPDATE_PLAYING;
	}
	return eUpdateStatus;
}

E_UPDATE_STATUS	CSkinController::UpdateFrameNoLoop		( PMOTION pMotion )
{
	E_UPDATE_STATUS	eUpdateStatus = UPDATE_PLAYING;
	// 끝까지 재생했으면 끝프레임에서 고정.
	int		nRange		= pMotion->m_nFrameD - pMotion->m_nFrameS;									// 프레임 길이.
	float	fTime		= (float)(fTimeGetTime()-pMotion->m_dwStart) / (pMotion->m_fSpeed * 1000.0f / nRange);	// 정해둔 시간에 맞춰 현재 프레임 계산.

	int		S1 = pMotion->m_nFrameS;
	int		S2 = pMotion->m_nFrameSIn;
	int		D1 = pMotion->m_nFrameD;
	int		D2 = pMotion->m_nFrameDIn;
	float	fTimeRelative = fTime + S1;
	if ( fTimeRelative >= pMotion->m_nFrameD )
	{
		pMotion->m_nThisFrame	= ((int)pMotion->m_nFrameD);
		pMotion->m_nNextFrame	= ((int)pMotion->m_nFrameD);
		fTime = (float)(pMotion->m_nFrameD-1);
		pMotion->m_fSlerp = 0.0f;
		eUpdateStatus		= UPDATE_PAUSE;
	}
	// 재생중...
	else
	{
		// 가속지점.
		if		( pMotion->m_nFrameS < fTimeRelative && pMotion->m_nFrameSIn > fTimeRelative )
		{
			int			nGab = (pMotion->m_nFrameSIn - pMotion->m_nFrameS);
			float		fAlpha	= (float)(fTimeRelative - pMotion->m_nFrameS)/(float)(nGab);
			D3DXVECTOR2	vOut = D3DXVECTOR2(pMotion->m_nFrameS, 0.0f);
			D3DXVECTOR2	vIn1 = D3DXVECTOR2(pMotion->m_nFrameS-nGab,	pMotion->m_nFrameS);
			D3DXVECTOR2	vIn2 = D3DXVECTOR2(pMotion->m_nFrameS,		pMotion->m_nFrameS);
			D3DXVECTOR2	vIn3 = D3DXVECTOR2(pMotion->m_nFrameSIn,		pMotion->m_nFrameSIn);
			D3DXVECTOR2	vIn4 = D3DXVECTOR2(pMotion->m_nFrameSIn+nGab,	pMotion->m_nFrameSIn);
			D3DXVec2CatmullRom ( &vOut, &vIn1, &vIn2, &vIn3, &vIn4, fAlpha);
			pMotion->m_fCurFrame = vOut.y - S1;
		}
		// 감속지점.
		else if	( pMotion->m_nFrameD > fTimeRelative && pMotion->m_nFrameDIn < fTimeRelative )
		{
			int			nGab = (pMotion->m_nFrameD - pMotion->m_nFrameDIn);
			float		fAlpha	= (float)(fTimeRelative - pMotion->m_nFrameDIn)/(float)(nGab);
			D3DXVECTOR2	vOut = D3DXVECTOR2(pMotion->m_nFrameD, 0.0f);
			D3DXVECTOR2	vIn1 = D3DXVECTOR2(pMotion->m_nFrameDIn-nGab,	pMotion->m_nFrameDIn);
			D3DXVECTOR2	vIn2 = D3DXVECTOR2(pMotion->m_nFrameDIn,		pMotion->m_nFrameDIn);
			D3DXVECTOR2	vIn3 = D3DXVECTOR2(pMotion->m_nFrameD,		pMotion->m_nFrameD);
			D3DXVECTOR2	vIn4 = D3DXVECTOR2(pMotion->m_nFrameD+nGab,	pMotion->m_nFrameD);
			D3DXVec2CatmullRom ( &vOut, &vIn1, &vIn2, &vIn3, &vIn4, fAlpha);
			pMotion->m_fCurFrame = vOut.y - S1;
		}
		// 정속지점
		else
		{
			pMotion->m_fCurFrame = fTime;
		}
		pMotion->m_nThisFrame	= ((int)pMotion->m_fCurFrame);
		pMotion->m_fSlerp		= pMotion->m_fCurFrame - pMotion->m_nThisFrame;
		pMotion->m_nThisFrame	= ((int)pMotion->m_fCurFrame)%(nRange) + pMotion->m_nFrameS;
		pMotion->m_nNextFrame	= pMotion->m_nThisFrame + 1;
	}
	return eUpdateStatus;
}

void			CSkinController::UpdateFrame()
{
	std::list<SMOTION>::iterator	pNode = m_listMotion.begin();
	std::list<SMOTION>::iterator	pHead = m_listMotion.begin();
	std::list<SMOTION>::iterator	pTail = m_listMotion.end();

	// 1. 가중치를 계산한다. 죄다 합해버리면 1이 넘거나 안되기 때문에 1에 맞추는 정규화 작업이 필요하다.
	// 이렇게 가중치 재계산을 안하면 캐릭터가 풍선마냥 커지거나 작아지거나 한다. 
	float	fSum = 0.0f;
	for ( pNode = pHead; pNode != pTail; pNode ++ )	fSum += (pNode->m_fBlendingRate * pNode->m_fBlendingRate);
	fSum		= sqrt(fSum);

	// 2. 스택에 쌓인 애니메이션들의 회전과 이동을 전부 구한다.
	pNode = m_listMotion.begin();
	pHead = m_listMotion.begin();
	pTail = m_listMotion.end();

	for ( pNode = pHead; pNode != pTail; )
	{
		E_UPDATE_STATUS	eUpdateStatus = UPDATE_PLAYING;
		if		( pNode->m_bLooped && pNode->m_nFrameD > pNode->m_nFrameS )	eUpdateStatus = UpdateFrameFoward	( &(*pNode) );
		else if	( pNode->m_bLooped && pNode->m_nFrameD < pNode->m_nFrameS )	eUpdateStatus = UpdateFrameBackward	( &(*pNode) );
		else																eUpdateStatus = UpdateFrameNoLoop	( &(*pNode) );
		SMOTION	motion		= *pNode;																// 현재 모션

		if ( motion.m_bIncrease )
		{
			pNode->m_fBlendingRate += m_fLastBlendingSpeed;											// 마지막에 들어온 애니메이션이라면 블렌딩계수가 증가.
			m_eUpdateStatus = eUpdateStatus;
			m_fCurrentFrame = pNode->m_nThisFrame + pNode->m_fSlerp;
		}
		else						pNode->m_fBlendingRate -= m_fLastBlendingSpeed*2;				// 아니라면 감소할것.
		if ( pNode->m_fBlendingRate > 1.0f )	pNode->m_fBlendingRate = 1.0f;						// 블렌딩계수 한계 지정.(0.0f~1.0f)
		if ( pNode->m_fBlendingRate < 0.0f )														// 0보다 작은 블렌딩계수를 가진 모션은 삭제.
		{
			pNode = m_listMotion.erase(pNode);
			pHead = m_listMotion.begin();
			pTail = m_listMotion.end();
			continue;
		}

		// 위의 프레임을 토대로 회전과 이동값을 계산해둔다.
		// 이렇게 나누어서 저장해두는 이유는 행렬단계까지 가면 보간되기가 힘들기때문.
		// 회전은 사원수, 이동은 벡터형태로 계산되어짐.
		UpdateSingleMotion	( &pNode );

		// 위에서 구해진 각각의 사원수와 벡터를 블렌딩계수에 맞춰 1개로 만든다.
		if ( pNode == pHead )	//맨앞에것이라면 그냥 적용.
		{
			int	i = 0;
			for ( i = 0; i < m_pSkinSource->nBoneCount; i ++ )	m_pBones[i].qCurRot	= m_pqSingleMotionRotate[i];
			for ( i = 0; i < m_pSkinSource->nBoneCount; i ++ )	m_pBones[i].vMov		= m_pvSingleMotionPivot[i];
		}
		else
		{
			int	i = 0;
			for ( i = 0; i < m_pSkinSource->nBoneCount; i ++ )	D3DXQuaternionSlerp	( &m_pBones[i].qCurRot,	&m_pBones[i].qCurRot,	&m_pqSingleMotionRotate[i], pNode->m_fBlendingRate/fSum );
			for ( i = 0; i < m_pSkinSource->nBoneCount; i ++ )	D3DXVec3Lerp		( &m_pBones[i].vMov,		&m_pBones[i].vMov,		&m_pvSingleMotionPivot[i],	pNode->m_fBlendingRate/fSum );
		}
		pNode ++;
	}

	// 3. 대체 애니메이션본이 존재 하면 그걸로 대체한다.
	ReplaceBoneTransform();

	// 4. 위에서 계산한거 가지고 피벗이동및 행렬 세팅을 하믄 애니메이션이 된답니다.
	UpdateWorldMatrix();
}

void			CSkinController::ReplaceBoneTransform	( void )
{
	if ( !m_pListBones )	return;
	float					fSlerp	= m_sReplaceMotion.m_fSlerp;
	int						nTimeT	= m_sReplaceMotion.m_nThisFrame;
	int						nTimeN	= m_sReplaceMotion.m_nNextFrame;
	for ( int i = 0; i < m_pSkinSource->nBoneCount; i ++ )
	{
		std::vector<int>::iterator npos = std::find( m_pListBones->begin(), m_pListBones->end(), i );
		if ( npos == m_pListBones->end() )	continue;

		m_pvSingleMotionPivot[i]	= m_pSkinSource->m_pBone[i].pvMov[nTimeT] + (m_pSkinSource->m_pBone[i].pvMov[nTimeN] - m_pSkinSource->m_pBone[i].pvMov[nTimeT]) * fSlerp;
		D3DXQuaternionSlerp	( &m_pqSingleMotionRotate[i], &m_pSkinSource->m_pBone[i].pqRot[nTimeT], &m_pSkinSource->m_pBone[i].pqRot[nTimeN], fSlerp );
		D3DXQuaternionSlerp	( &m_pBones[i].qCurRot, 	&m_pBones[i].qCurRot, &m_pqSingleMotionRotate[i], m_sReplaceMotion.m_fBlendingRate );
		m_pvSingleMotionPivot[i]	= m_pBones[i].vMov + (m_pvSingleMotionPivot[i] - m_pBones[i].vMov)*m_sReplaceMotion.m_fBlendingRate;
	}
}

void			CSkinController::CombineAfterTransform	( void )
{
	int	i = 0;
	for ( i = 0; i < m_pSkinSource->nBoneCount; i ++ )
	{
		// 후처리 애니메이션을 위한 부분.
		D3DXQUATERNION	qCurrectRotB		= m_pBones[i].qRotBeforeTM;
		D3DXQUATERNION	qCurrectRotA		= m_pBones[i].qRotAfterTM;
		D3DXQUATERNION	qCurRotBFromMatrix, qCurRotAFromMatrix;
		D3DXQuaternionRotationMatrix( &qCurRotAFromMatrix, &m_pBones[i].mRotAfterTM );
		D3DXQuaternionRotationMatrix( &qCurRotBFromMatrix, &m_pBones[i].mRotBeforeTM );
		qCurrectRotB		= m_pSkinSource->m_pBone[i].qBasis * m_pBones[i].qRotBeforeTM* qCurRotBFromMatrix	*m_pSkinSource->m_pBone[i].qInvBasis;
		qCurrectRotA		= m_pSkinSource->m_pBone[i].qBasis * m_pBones[i].qRotAfterTM * qCurRotAFromMatrix	*m_pSkinSource->m_pBone[i].qInvBasis;
		m_pBones[i].qCurRot	= qCurrectRotB * m_pBones[i].qCurRot * qCurrectRotA;
	}
}

void			CSkinController::Render			( bool BeHitCheck, float BuffCheck )
{
	m_pSkinSource->SetPos( m_vPos );
	m_pSkinSource->SetRot( m_vRot );
	m_pSkinSource->SetScl( m_vScl );
	UpdateFrame();
	m_pSkinSource->SetWorldMatrix ( m_pmWorlds );
	m_pSkinSource->Render( BeHitCheck, BuffCheck );
}

int				CSkinController::GetObjNoFromName( char* szName )
{
	return -1;
}

int				CSkinController::GetBoneNoFromName( char* szName )
{
	int	i = 0;
	std::string	strName;
	for ( i = 0; i < m_pSkinSource->nBoneCount; i ++ )
	{
		if ( m_pSkinSource->m_pBone[i].strName.compare(szName) == 0 ) return i;
	}
	return -1;
}

void	CSkinController::SetPos		( D3DXVECTOR3 vPos )
{
	m_vPos = vPos;
}

void	CSkinController::SetRot		( D3DXVECTOR3 vRot )
{
	m_vRot = vRot;
}

void	CSkinController::SetScl		( D3DXVECTOR3 vScl )
{
	m_vScl = vScl;
}

// 애니메이션을 재생을 하되 반복하지 않는 상태를 list에 추가한다.
E_UPDATE_STATUS	CSkinController::SetRangeNoLoop	( int fFrameS, int fFrameD, int fFrameSInner, int fFrameDInner, float fRepeatInterval, float fBlendingSpeed )
{
	SMOTION	motion;
	motion.m_bLooped		= false;
	motion.m_fBlendingRate	= 0.0f;
	motion.m_fBlendingSpeed	= fBlendingSpeed;
	motion.m_fSlerp			= 0.0f;
	motion.m_fCurFrame		= fFrameS;
	motion.m_nNextFrame		= fFrameS;
	motion.m_nThisFrame		= fFrameS;
	motion.m_fSpeed			= fRepeatInterval;
	motion.m_nFrameS		= fFrameS;
	motion.m_nFrameD		= fFrameD;
	motion.m_nFrameSIn		= fFrameSInner;
	motion.m_nFrameDIn		= fFrameDInner;
	motion.m_dwStart		= fTimeGetTime();
	motion.m_bIncrease		= true;

	m_fLastBlendingSpeed	= fBlendingSpeed;

	std::list<SMOTION>::reverse_iterator	pTemp = m_listMotion.rbegin();
	if ( m_listMotion.size() > 0 )
	{
		if ( pTemp->m_nFrameS == fFrameS && pTemp->m_nFrameD == fFrameD )	return m_eUpdateStatus;
	}

	std::list<SMOTION>::iterator	pNode = m_listMotion.begin();
	std::list<SMOTION>::iterator	pHead = m_listMotion.begin();
	std::list<SMOTION>::iterator	pTail = m_listMotion.end();
	std::list<SMOTION>::iterator	pLast = m_listMotion.end();

	for ( pNode = pHead; pNode != pTail; pNode ++ )
	{
		pNode->m_bIncrease	= false;
		pLast				= pNode;
	}
	if ( pLast != pTail )
	{
		int		nRange			= (pLast->m_nFrameD - pLast->m_nFrameS);
		float	fElapsed		= (float)((int)(fTimeGetTime() - motion.m_dwStart)%(int)(motion.m_fSpeed*1000.0f));
		float	fLength			= (motion.m_fSpeed*1000.0f);
		float	fSubtract		= fElapsed / fLength;
	}
	if ( pLast != pTail || m_listMotion.size() < 1 )	m_listMotion.push_back ( motion );
	return m_eUpdateStatus;
}

// 특정 본에만 반복 재생하는 애니메이션을 추가한다.
E_UPDATE_STATUS		CSkinController::MixRangeBones	( int fFrameS, int fFrameD, float fRepeatInterval, std::vector<int>* pVecBones, float fOverlayRate )
{
	m_sReplaceMotion.m_bLooped			= true;
	m_sReplaceMotion.m_fSlerp			= 0.0f;
	m_sReplaceMotion.m_fCurFrame		= fFrameS;
	m_sReplaceMotion.m_nNextFrame		= fFrameS;
	m_sReplaceMotion.m_nThisFrame		= fFrameS;
	m_sReplaceMotion.m_fSpeed			= fRepeatInterval;
	m_sReplaceMotion.m_nFrameS			= fFrameS;
	m_sReplaceMotion.m_nFrameD			= fFrameD;
	m_sReplaceMotion.m_nFrameSIn		= 0;
	m_sReplaceMotion.m_nFrameDIn		= 0;
	if ( !m_pListBones )
	{
		m_sReplaceMotion.m_dwStart			= fTimeGetTime();
		m_sReplaceMotion.m_fBlendingRate	= 0.0f;
	}
	m_sReplaceMotion.m_fBlendingRate	= fOverlayRate;
	if ( m_sReplaceMotion.m_fBlendingRate < 0.01f )	return UPDATE_RATE_IS_ZERO;
	m_sReplaceMotion.m_fBlendingRate	= __max(0.0f, __min(1.0f, m_sReplaceMotion.m_fBlendingRate ));
	m_sReplaceMotion.m_bIncrease		= true;

	E_UPDATE_STATUS	eUpdateStatus = UPDATE_PLAYING;
	eUpdateStatus = m_sReplaceMotion.CalcCurrentFrame();
	m_pListBones					= pVecBones;
	return eUpdateStatus;
}

// 특정 본에만 비반복 재생하는 애니메이션을 추가한다.
E_UPDATE_STATUS		CSkinController::MixRangeNoLoopBones	( int fFrameS, int fFrameD, float fRepeatInterval, std::vector<int>* pVecBones, float fOverlayRate )
{
	m_pListBones						= pVecBones;
	m_sReplaceMotion.m_bLooped			= false;
	m_sReplaceMotion.m_fSlerp			= 0.0f;
	m_sReplaceMotion.m_fCurFrame		= fFrameS;
	m_sReplaceMotion.m_nNextFrame		= fFrameS;
	m_sReplaceMotion.m_nThisFrame		= fFrameS;
	m_sReplaceMotion.m_fSpeed			= fRepeatInterval;
	m_sReplaceMotion.m_nFrameS			= fFrameS;
	m_sReplaceMotion.m_nFrameD			= fFrameD;
	m_sReplaceMotion.m_nFrameSIn		= 0;
	m_sReplaceMotion.m_nFrameDIn		= 0;
	if ( !m_pListBones )
	{
		m_sReplaceMotion.m_dwStart			= fTimeGetTime();
		m_sReplaceMotion.m_fBlendingRate	= 0.0f;
	}
	else m_sReplaceMotion.m_fBlendingRate	= fOverlayRate;
	if ( m_sReplaceMotion.m_fBlendingRate < 0.01f )	return UPDATE_RATE_IS_ZERO;
	m_sReplaceMotion.m_fBlendingRate	= __max(0.0f, __min(1.0f, m_sReplaceMotion.m_fBlendingRate ));
	m_sReplaceMotion.m_bIncrease		= true;

	E_UPDATE_STATUS	eUpdateStatus = UPDATE_PLAYING;
	eUpdateStatus = m_sReplaceMotion.CalcCurrentFrame();
	return eUpdateStatus;
}


// 반복 재생하는 애니메이션을 list에 추가한다.
E_UPDATE_STATUS	CSkinController::SetRange	( int fFrameS, int fFrameD, float fRepeatInterval, float fBlendingSpeed, bool bReset )
{
	SMOTION	motion;
	motion.m_bLooped		= true;
	motion.m_fBlendingRate	= 0.0f;
	motion.m_fBlendingSpeed	= fBlendingSpeed;
	motion.m_fSlerp			= 0.0f;
	motion.m_fCurFrame		= fFrameS;
	motion.m_nNextFrame		= fFrameS;
	motion.m_nThisFrame		= fFrameS;
	motion.m_fSpeed			= fRepeatInterval;
	motion.m_nFrameS		= fFrameS;
	motion.m_nFrameD		= fFrameD;
	motion.m_dwStart		= fTimeGetTime();
	motion.m_bIncrease		= true;

	m_fLastBlendingSpeed	= fBlendingSpeed;

	std::list<SMOTION>::reverse_iterator	pTemp = m_listMotion.rbegin();
	if ( m_listMotion.size() > 0 )
	{
		if ( pTemp->m_nFrameS == fFrameS && pTemp->m_nFrameD == fFrameD )	return m_eUpdateStatus;
	}

	std::list<SMOTION>::iterator	pNode = m_listMotion.begin();
	std::list<SMOTION>::iterator	pHead = m_listMotion.begin();
	std::list<SMOTION>::iterator	pTail = m_listMotion.end();
	std::list<SMOTION>::iterator	pLast = m_listMotion.end();

	for ( pNode = pHead; pNode != pTail; pNode ++ )
	{
		pNode->m_bIncrease	= false;
		pLast				= pNode;
	}
	if ( !bReset && pLast != pTail )
	{
		int		nRange			= (pLast->m_nFrameD - pLast->m_nFrameS);	// 프레임 길이.
		float	fElapsed		= (float)((int)(fTimeGetTime() - pLast->m_dwStart)%(int)(pLast->m_fSpeed*1000.0f));
		float	fLength			= (pLast->m_fSpeed*1000.0f);
		float	fSubtract		= fElapsed / fLength;
		motion.m_dwStart		= fTimeGetTime() - ((fSubtract) * (float)(motion.m_fSpeed)*1000.0f);
	}
	if ( pLast != pTail || m_listMotion.size() < 1 )	m_listMotion.push_back ( motion );
	return m_eUpdateStatus;
}

void			CSkinController::HideObject		( int nObjNo, bool dHide )
{
	int i = 0;
	if ( nObjNo == -1 )	for ( i = 0; i < m_pSkinSource->nObjCount; i ++ )	m_pSkinSource->m_pbShow[i]		= !dHide;
	else																	m_pSkinSource->m_pbShow[nObjNo]= !dHide;
}

void			CSkinController::ShowObject		( int nObjNo, bool dShow )
{
	int i = 0;
	if ( nObjNo == -1 )	for ( i = 0; i < m_pSkinSource->nObjCount; i ++ )	m_pSkinSource->m_pbShow[i]		= dShow;
	else																	m_pSkinSource->m_pbShow[nObjNo]= dShow;
}

