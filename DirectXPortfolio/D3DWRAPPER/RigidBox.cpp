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
//	D3DXVECTOR3		vForceN;											/*���� ����*/;
//	D3DXVECTOR3		vForce;												/*���� ����� ũ��*/;
//	D3DXVECTOR3		vRotAxis;											/*���������� ���� ȸ���� ȸ����*/;
//	D3DXVECTOR3		vCenterOfMassToForce = m_vForcePoint - m_vPosition;	/*�����߽ɰ� ���������� ����*/
//	D3DXVECTOR3		vCenterOfMassToForceN;
//	D3DXVECTOR3		vMoment;											/*3D �������Ʈ*/
//	float			R	= D3DXVec3Length(&vCenterOfMassToForce);		/*�����߽ɰ� ���������� �Ÿ�.*/
//	D3DXVec3Normalize( &vForceN, &m_vEnergy );
//	vForce				= vForceN * fF * 1.0f;							/*���� ����� ũ�⸦ ���Ѵ�. ���� ���� * ���� ũ��*/
//
//	D3DXVec3Normalize( &vCenterOfMassToForceN, &vCenterOfMassToForce );	/*�����߽ɿ��� ���������� ����*/
//
//	float		fDotForce	= -D3DXVec3Dot ( &vCenterOfMassToForceN, &vForceN );
//
//	D3DXVec3Cross		( &vRotAxis, &vCenterOfMassToForce, &vForce );	/*�浹�� ���� ȸ������ ���Ѵ�.*/
//	if ( D3DXVec3LengthSq(&vRotAxis) > 0.00001f )	D3DXVec3Normalize	( &m_vAngVelAxis, &vRotAxis );					/*ȸ���� ����ȭ*/
//
//	D3DXVECTOR3	vTorque = vForce - (fDotForce * vCenterOfMassToForceN * D3DXVec3Length(&vForce));
//
////////////////////////////////////////////////////////////////////////////
////	ȸ���࿡ ���� ���� ���Ʈ�絵 �ٲ�.
////////////////////////////////////////////////////////////////////////////
//	vMoment.x	= m_vMoment.x * m_vAngVelAxis.x;
//	vMoment.y	= m_vMoment.y * m_vAngVelAxis.y;
//	vMoment.z	= m_vMoment.z * m_vAngVelAxis.z;
//
//	float		I			= __max(0.001f, D3DXVec3Length( &vMoment ));
//	float		t			= D3DXVec3Length( &vTorque );
//	float		vAngForce	= t/I;										/*���Ʈ��� �Ÿ��� ���� ����ȭ ���� ���.*/
//	D3DXMATRIX	mVelAngle;												/*�����ӵ��� �߰��� ���ӵ�.*/
//
//	m_fAngVel += vAngForce;
//	D3DXMatrixRotationAxis	( &mVelAngle, &m_vAngVelAxis, m_fAngVel);	/*���ӵ��� ���Ѵ�.*/
//
//	float	aK			= t*t * I * 1.0f;								/*��ũ�� �̿��ؼ� ȸ�� � �������� ���Ѵ�.*/
//	float	lK			= __max(0.0f, fE - aK);							/*��ü ����������� ȸ�� � �������� ���� ���� �������.*/
//	float	lv			= sqrt(lK*2.0f/m_fMass);						/*���� ����������� �ӵ��� ����*/
//
//	m_vVelocity			+= vForceN * lv;								/*���� �ӵ��� �����Ѵ�.*/
//	m_vVelocity.y		-= 9.8f*0.001f;									/*�߷°��ӵ��� �����Ѵ�.*/
//
//	m_fAngVel		*=	0.997f;											/*���ӵ��� ���� �پ���.*/
//	m_vVelocity.x	*=	0.9f;											/*���ӵ��� ���� �پ���.*/
//	m_vVelocity.z	*=	0.9f;											/*���ӵ��� ���� �پ���.*/
//	m_vPosition		+=	m_vVelocity;									/*���ӵ��� ������ �߰�.*/
//	m_mRotation		= m_mRotation	* mVelAngle;						/*���ӵ��� ������ �߰�.*/
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

