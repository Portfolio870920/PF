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

	m_pmWorlds				= new D3DXMATRIX				[nBoneCount];	// ���� ���� ������ŭ ������������� ����.
	m_pBones				= new SBONE_INSTANCE			[nBoneCount];
	m_pvSingleMotionPivot	= new D3DXVECTOR3	[nBoneCount];	// ���1���� ���� ���ø�� �ǹ�.
	m_pqSingleMotionRotate	= new D3DXQUATERNION[nBoneCount];	// ���1���� ���� ���ø�� ȸ����.


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

	m_vRot		= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );	// ȸ���� �ʱ�ȭ.
	m_vPos		= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );	// ��ġ�� �ʱ�ȭ.
	m_vScl		= D3DXVECTOR3( 1.0f, 1.0f, 1.0f );	// ��ô�� �ʱ�ȭ.
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

	// ����� ���� ���� ���·� ����.
	for ( i = 0; i < m_pSkinSource->nBoneCount; i ++ )	m_pBones[i].bCalculated = false;

	// Ư�� ���� ��ü�ִϸ��̼��� �����ϸ� ��ü��Ų��.
	if ( m_pListBones )					ReplaceBoneTransform	();
	CombineAfterTransform										();
	// ȸ������ �θ����� �ջ��Ѵ�.
	for ( i = 0; i < m_pSkinSource->nBoneCount; i ++ ) UnitedRotate ( i );

	// ȸ���� �ջ��� �� ���� �������ִ� ������ ������� ���� �������� �Ѵ�.
	for ( i = 0; i < m_pSkinSource->nBoneCount; i ++ ) m_pBones[i].dmSumRot = m_pSkinSource->m_pBone[i].mInvBasis * m_pBones[i].dmSumRot;

	// �ٽ� ������ ���� ���·� ����.
	for ( i = 0; i < m_pSkinSource->nBoneCount; i ++ ) m_pBones[i].bCalculated = FALSE;

	// �θ� ���뵵 �̵�/ȸ���� �����Ƿ� ���� ȸ������ �ű⿡ �°� �缳��.
	for ( i = 0; i < m_pSkinSource->nBoneCount; i ++ ) RepositionPivot ( i );

	// ������ ����ȸ������ġ, �̵�, ȸ���� ��� �ϳ��� �����.
	for ( i = 0; i < m_pSkinSource->nBoneCount; i ++ ) TransformObject ( i );

	// ���̴��� �ֱ� ���ϰ� ���ӵ� ��Ĺ��ۿ� �ִ´�.
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
	int		nRange		= pMotion->m_nFrameD - pMotion->m_nFrameS;												// ������ ����.
	float	fTime		= (float)(fTimeGetTime()-pMotion->m_dwStart) / (pMotion->m_fSpeed * 1000.0f / nRange);	// ���ص� �ð��� ���� ���� ������ ���.

	// ���� ������ ����.
	pMotion->m_fCurFrame= fTime;

	// ������ ���� ���� ������.
	pMotion->m_nThisFrame	= ((int)pMotion->m_fCurFrame);

	// ���� �����ӿ��� ���� ������ ������ �Ҽ���.(0~1.0f)
	pMotion->m_fSlerp		= pMotion->m_fCurFrame - pMotion->m_nThisFrame;

	// fTimeGetTime()���� ����ϹǷ� ���������� ��ȯ�ϰ� ����.
	pMotion->m_nThisFrame	= ((int)pMotion->m_fCurFrame)%(nRange) + pMotion->m_nFrameS;

	// �����ִϸ��̼� ���� ������ ���.
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
	int		nRange		= -(pMotion->m_nFrameD - pMotion->m_nFrameS);												// ������ ����.
	float	fTime		= 1.0f - (float)((int)(fTimeGetTime()-pMotion->m_dwStart)%(DWORD)(pMotion->m_fSpeed * 1000.0f))/(pMotion->m_fSpeed * 1000.0f);	// ���ص� �ð��� ���� ���� ������ ���.
	fTime				*= nRange;

	// ���� ������ ����.
	pMotion->m_fCurFrame= fTime;

	// ������ ���� ���� ������.
	pMotion->m_nThisFrame	= ((int)pMotion->m_fCurFrame);

	// ���� �����ӿ��� ���� ������ ������ �Ҽ���.(0~1.0f) ���� �������� �������Ӻ��� Ŭ���(������ ���) �ݴ�� ����Ѵ�.
	pMotion->m_fSlerp		= 1.0f-(pMotion->m_fCurFrame - pMotion->m_nThisFrame);

	// fTimeGetTime()���� ����ϹǷ� ���������� ��ȯ�ϰ� ����.
	pMotion->m_nThisFrame	= ((int)pMotion->m_fCurFrame)%(nRange) + pMotion->m_nFrameD;

	// �����ִϸ��̼� ���� ������ ���.
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
	// ������ ��������� �������ӿ��� ����.
	int		nRange		= pMotion->m_nFrameD - pMotion->m_nFrameS;									// ������ ����.
	float	fTime		= (float)(fTimeGetTime()-pMotion->m_dwStart) / (pMotion->m_fSpeed * 1000.0f / nRange);	// ���ص� �ð��� ���� ���� ������ ���.

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
	// �����...
	else
	{
		// ��������.
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
		// ��������.
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
		// ��������
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

	// 1. ����ġ�� ����Ѵ�. �˴� ���ع����� 1�� �Ѱų� �ȵǱ� ������ 1�� ���ߴ� ����ȭ �۾��� �ʿ��ϴ�.
	// �̷��� ����ġ ������ ���ϸ� ĳ���Ͱ� ǳ������ Ŀ���ų� �۾����ų� �Ѵ�. 
	float	fSum = 0.0f;
	for ( pNode = pHead; pNode != pTail; pNode ++ )	fSum += (pNode->m_fBlendingRate * pNode->m_fBlendingRate);
	fSum		= sqrt(fSum);

	// 2. ���ÿ� ���� �ִϸ��̼ǵ��� ȸ���� �̵��� ���� ���Ѵ�.
	pNode = m_listMotion.begin();
	pHead = m_listMotion.begin();
	pTail = m_listMotion.end();

	for ( pNode = pHead; pNode != pTail; )
	{
		E_UPDATE_STATUS	eUpdateStatus = UPDATE_PLAYING;
		if		( pNode->m_bLooped && pNode->m_nFrameD > pNode->m_nFrameS )	eUpdateStatus = UpdateFrameFoward	( &(*pNode) );
		else if	( pNode->m_bLooped && pNode->m_nFrameD < pNode->m_nFrameS )	eUpdateStatus = UpdateFrameBackward	( &(*pNode) );
		else																eUpdateStatus = UpdateFrameNoLoop	( &(*pNode) );
		SMOTION	motion		= *pNode;																// ���� ���

		if ( motion.m_bIncrease )
		{
			pNode->m_fBlendingRate += m_fLastBlendingSpeed;											// �������� ���� �ִϸ��̼��̶�� ��������� ����.
			m_eUpdateStatus = eUpdateStatus;
			m_fCurrentFrame = pNode->m_nThisFrame + pNode->m_fSlerp;
		}
		else						pNode->m_fBlendingRate -= m_fLastBlendingSpeed*2;				// �ƴ϶�� �����Ұ�.
		if ( pNode->m_fBlendingRate > 1.0f )	pNode->m_fBlendingRate = 1.0f;						// ������� �Ѱ� ����.(0.0f~1.0f)
		if ( pNode->m_fBlendingRate < 0.0f )														// 0���� ���� ��������� ���� ����� ����.
		{
			pNode = m_listMotion.erase(pNode);
			pHead = m_listMotion.begin();
			pTail = m_listMotion.end();
			continue;
		}

		// ���� �������� ���� ȸ���� �̵����� ����صд�.
		// �̷��� ����� �����صδ� ������ ��Ĵܰ���� ���� �����ǱⰡ ����⶧��.
		// ȸ���� �����, �̵��� �������·� ���Ǿ���.
		UpdateSingleMotion	( &pNode );

		// ������ ������ ������ ������� ���͸� ��������� ���� 1���� �����.
		if ( pNode == pHead )	//�Ǿտ����̶�� �׳� ����.
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

	// 3. ��ü �ִϸ��̼Ǻ��� ���� �ϸ� �װɷ� ��ü�Ѵ�.
	ReplaceBoneTransform();

	// 4. ������ ����Ѱ� ������ �ǹ��̵��� ��� ������ �Ϲ� �ִϸ��̼��� �ȴ�ϴ�.
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
		// ��ó�� �ִϸ��̼��� ���� �κ�.
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

// �ִϸ��̼��� ����� �ϵ� �ݺ����� �ʴ� ���¸� list�� �߰��Ѵ�.
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

// Ư�� ������ �ݺ� ����ϴ� �ִϸ��̼��� �߰��Ѵ�.
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

// Ư�� ������ ��ݺ� ����ϴ� �ִϸ��̼��� �߰��Ѵ�.
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


// �ݺ� ����ϴ� �ִϸ��̼��� list�� �߰��Ѵ�.
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
		int		nRange			= (pLast->m_nFrameD - pLast->m_nFrameS);	// ������ ����.
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

