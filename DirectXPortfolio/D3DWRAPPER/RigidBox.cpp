// RigidBox.cpp: implementation of the CRigidBox class.
//
//////////////////////////////////////////////////////////////////////

#include "RigidBox.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRigidBox::CRigidBox()
{
	m_mRotation			= IdentityMatrix;
	m_listAngular.clear();
//	m_fAngVel			= 0.0f;
//	m_vAngVelAxis		= D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vPosition			= D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vVelocity			= D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

CRigidBox::~CRigidBox()
{
}
// http://optics.hanyang.ac.kr/~shsong/10-Rotation.pdf

void	CRigidBox::SetBox	( float fSizeX, float fSizeY, float fSizeZ, float fMass )
{
	float	fMoment = 0.0f;
	m_vSize		= D3DXVECTOR3 ( fSizeX, fSizeY, fSizeZ);
	m_fMass		= fMass / 9.8f;
	m_vMoment.x	= fMass * ( fSizeY * fSizeY + fSizeZ * fSizeZ ) / 12.0f;
	m_vMoment.y	= fMass * ( fSizeX * fSizeX + fSizeZ * fSizeZ ) / 12.0f;
	m_vMoment.z	= fMass * ( fSizeX * fSizeX + fSizeY * fSizeY ) / 12.0f;
}

void	CRigidBox::Update	( float	fDelta )
{
//	float			fE		= D3DXVec3Length( &m_vEnergy );
//	float			fV		= sqrt((2.0f*fE/m_fMass));
//	float			fA		= fV;
//	float			fF		= fA / m_fMass;
//	D3DXVECTOR3		vForceN;											/*힘의 방향*/;
//	D3DXVECTOR3		vForce;												/*힘의 방향과 크기*/;
//	D3DXVECTOR3		vRotAxis;											/*가해진힘에 의해 회전할 회전축*/;
//	D3DXVECTOR3		vCenterOfMassToForce = m_vForcePoint - m_vPosition;	/*무게중심과 힘점사이의 벡터*/
//	D3DXVECTOR3		vCenterOfMassToForceN;
//	D3DXVECTOR3		vMoment;											/*3D 관성모멘트*/
//	float			R	= D3DXVec3Length(&vCenterOfMassToForce);		/*무게중심과 힘점사이의 거리.*/
//	D3DXVec3Normalize( &vForceN, &m_vEnergy );
//	vForce				= vForceN * fF * 1.0f;							/*힘의 방향과 크기를 구한다. 힘의 방향 * 힘의 크기*/
//
//	D3DXVec3Normalize( &vCenterOfMassToForceN, &vCenterOfMassToForce );	/*무게중심에서 힘점까지의 방향*/
//
//	float		fDotForce	= -D3DXVec3Dot ( &vCenterOfMassToForceN, &vForceN );
//
//	D3DXVec3Cross		( &vRotAxis, &vCenterOfMassToForce, &vForce );	/*충돌에 대한 회전축을 구한다.*/
//	if ( D3DXVec3LengthSq(&vRotAxis) > 0.00001f )	D3DXVec3Normalize	( &m_vAngVelAxis, &vRotAxis );					/*회전축 정규화*/
//
//	D3DXVECTOR3	vTorque = vForce - (fDotForce * vCenterOfMassToForceN * D3DXVec3Length(&vForce));
//
////////////////////////////////////////////////////////////////////////////
////	회전축에 따라 관성 모멘트양도 바뀜.
////////////////////////////////////////////////////////////////////////////
//	vMoment.x	= m_vMoment.x * m_vAngVelAxis.x;
//	vMoment.y	= m_vMoment.y * m_vAngVelAxis.y;
//	vMoment.z	= m_vMoment.z * m_vAngVelAxis.z;
//
//	float		I			= __max(0.001f, D3DXVec3Length( &vMoment ));
//	float		t			= D3DXVec3Length( &vTorque );
//	float		vAngForce	= t/I;										/*모멘트양과 거리를 곱해 각변화 힘을 계산.*/
//	D3DXMATRIX	mVelAngle;												/*각가속도가 추가된 각속도.*/
//
//	m_fAngVel += vAngForce;
//	D3DXMatrixRotationAxis	( &mVelAngle, &m_vAngVelAxis, m_fAngVel);	/*각속도를 구한다.*/
//
//	float	aK			= t*t * I * 1.0f;								/*토크를 이용해서 회전 운동 에너지를 구한다.*/
//	float	lK			= __max(0.0f, fE - aK);							/*전체 운동에너지에서 회전 운동 에너지를 빼면 변위 운동에너지.*/
//	float	lv			= sqrt(lK*2.0f/m_fMass);						/*변위 운동에너지에서 속도를 추출*/
//
//	m_vVelocity			+= vForceN * lv;								/*현재 속도에 가중한다.*/
//	m_vVelocity.y		-= 9.8f*0.001f;									/*중력가속도를 가중한다.*/
//
//	m_fAngVel		*=	0.997f;											/*각속도는 점점 줄어든다.*/
//	m_vVelocity.x	*=	0.9f;											/*선속도도 점점 줄어든다.*/
//	m_vVelocity.z	*=	0.9f;											/*선속도도 점점 줄어든다.*/
//	m_vPosition		+=	m_vVelocity;									/*선속도를 변위에 추가.*/
//	m_mRotation		= m_mRotation	* mVelAngle;						/*각속도를 각도에 추가.*/
//	D3DXVECTOR3	x(m_mRotation.m[0]);
//	D3DXVECTOR3	y(m_mRotation.m[1]);
//	D3DXVECTOR3	z(m_mRotation.m[2]);
//	D3DXVec3Normalize( &x, &x );
//	D3DXVec3Normalize( &y, &y );
//	D3DXVec3Normalize( &z, &z );
//	memcpy ( m_mRotation.m[0], &x, 12 );
//	memcpy ( m_mRotation.m[1], &y, 12 );
//	memcpy ( m_mRotation.m[2], &z, 12 );
//	if ( m_vPosition.y < 0.0f )
//	{
//		m_vPosition.y = 0.0f;
//		m_vVelocity.y *= -0.2f;
//	}
//	m_vEnergy = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

void	CRigidBox::RenderDebug ( void )
{
	if ( !m_pDebugMesh ) return;
	D3DXQUATERNION	qIdent;
	D3DXQuaternionIdentity( &qIdent );
//	D3DXMatrixTransformation ( &m_mTransform,	&D3DXVECTOR3(0.0f, 0.0f, 0.0f), &qIdent,
//												&D3DXVECTOR3(m_vSize.x, m_vSize.y, m_vSize.z), &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
//												&m_qRotation, &D3DXVECTOR3(m_vPosition.x, m_vPosition.y, m_vPosition.z) );
	D3DXMATRIX	mTralsation;
	D3DXMatrixTranslation( &mTralsation, m_vPosition.x, m_vPosition.y, m_vPosition.z );
	m_mTransform = m_mRotation * mTralsation;

	D3DFRAME->GetDevice()->SetTransform( D3DTS_WORLD,		&m_mTransform );
	D3DFRAME->GetDevice()->SetTransform( D3DTS_VIEW,		&D3DFRAME->m_mView );
	D3DFRAME->GetDevice()->SetTransform( D3DTS_PROJECTION,	&D3DFRAME->m_mProj );
	D3DFRAME->GetDevice()->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	D3DFRAME->GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	D3DFRAME->GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	m_pDebugMesh->DrawSubset(0);
	D3DFRAME->GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	D3DFRAME->GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	D3DFRAME->GetDevice()->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
}

