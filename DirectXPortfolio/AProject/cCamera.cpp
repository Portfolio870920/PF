#include "MainHeader.h"

CCamera*	CCamera::pCamera		=	NULL;

CCamera::CCamera(void)
{
	m_bKey	=	false;
}

CCamera::~CCamera(void)
{
}

void	CCamera::Init( int cameraType, bool keyEnable, float velocity )
{
	m_fangle		=	0.0f;
	m_nCameraType	=	cameraType;
	m_vPos			=	D3DXVECTOR3( 0.0f, 114.0f, 0.0f );
	m_vRight		=	D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
	m_vUp			=	D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	m_vLook			=	D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
	m_vCurSrc		=	D3DXVECTOR3( 0.0f, 158.0f, 0.0f );
	m_vCurTgt		=	D3DXVECTOR3( 0.0f, 131.0f, 0.0f );
	if ( RSCMGR->m_bCHAR_MGR )
		m_vTarget_Y		=	D3DXVECTOR3( CHARMGR->GetPos().x, CHARMGR->GetPos().y+10.0f, CHARMGR->GetPos().z );


	m_fVelocity		=	velocity;
	D3DXMatrixLookAtLH( &D3DFRAME->m_mView, &m_vPos, &m_vTarget_Y, &m_vUp );
	D3DFRAME->SetViewMatrix( &GetViewMatrix() );
	Pitch( 0.5f );
}

void	CCamera::Update( )
{
	 //ī�޶� �̵�
	/*if( m_bKey )
	{
		if( KEYMGR->StayKeyDown('W') )			CAMMGR->Walk( m_fVelocity );
		if( KEYMGR->StayKeyDown('S') )			CAMMGR->Walk( -m_fVelocity );
		if( KEYMGR->StayKeyDown('A') )			CAMMGR->Strafe( -m_fVelocity  );
		if( KEYMGR->StayKeyDown('D') )			CAMMGR->Strafe( m_fVelocity  );
		if( KEYMGR->StayKeyDown('R') )			CAMMGR->Fly( m_fVelocity  );
		if( KEYMGR->StayKeyDown('F') )			CAMMGR->Fly( -m_fVelocity  );
		if( KEYMGR->StayKeyDown(VK_UP) )		CAMMGR->Pitch(-0.05f);
		if( KEYMGR->StayKeyDown(VK_DOWN) )		CAMMGR->Pitch(0.05f);
		if( KEYMGR->StayKeyDown(VK_LEFT) )		CAMMGR->Yaw(-0.05f);
		if( KEYMGR->StayKeyDown(VK_RIGHT) )		CAMMGR->Yaw(0.05f);
	}*/

	if( SCENEMGR->GetNowScene() == MENU )
	{
		SetbGetVec();
		m_vPos		= ( (MOBMGR->MobVec(MOBMGR->IterBegin()))->GetPos() - m_vLook * 50.0) + m_vUp * 15.0f;
		if ( m_vPos.y	<= MAPMGR->GetHeight( m_vPos.x, m_vPos.z ) )
		{
			m_vPos.y	=	MAPMGR->GetHeight( m_vPos.x, m_vPos.z ) + 10.0f;
		}
		m_vRight	= (MOBMGR->MobVec(MOBMGR->IterBegin()))->GetRight();
		m_vUp		= (MOBMGR->MobVec(MOBMGR->IterBegin()))->GetUp();
		m_vLook		= (MOBMGR->MobVec(MOBMGR->IterBegin()))->GetLook();
		
		m_vTarget_Y		=	D3DXVECTOR3( (MOBMGR->MobVec(MOBMGR->IterBegin()))->GetPos().x, (MOBMGR->MobVec(MOBMGR->IterBegin()))->GetPos().y+30.0f, (MOBMGR->MobVec(MOBMGR->IterBegin()))->GetPos().z );
		m_vCurTgt += (m_vTarget_Y - m_vCurTgt)*0.1f;		//	���� ī�޶� 
		m_vCurSrc += (m_vPos + D3DXVECTOR3(0.0f,0.0f,0.0f) - m_vCurSrc)*0.1f;	//	���� ī�޶�
		m_vPos.z+=25.0f;	//��� �־����� ��������� �ϱ�
		Yaw(m_fangle);		//ī�޶� ȸ����Ų��
		m_fangle+=0.005f;	//ȸ������ ������Ų��
	}
	else
	{
		m_vPos		= (m_vPlayerPos - m_vLook * 60.0f) + m_vUp * 15.0f;
		if ( m_bGetVec )
		{
			m_vRight	= m_vPlayerRight;
			m_vUp		= m_vPlayerUp;
			m_vLook		= m_vPlayerLook;
			m_bGetVec	=	false;
		}
		if ( m_vPos.y	<= MAPMGR->GetHeight( m_vPos.x, m_vPos.z ) )
		{
			m_vPos.y	=	MAPMGR->GetHeight( m_vPos.x, m_vPos.z ) + 15.0f;
		}
		m_vTarget_Y		=	D3DXVECTOR3( CHARMGR->GetPos().x, CHARMGR->GetPos().y+17.0f, CHARMGR->GetPos().z );
		m_vCurTgt += (m_vTarget_Y - m_vCurTgt)*0.1f;		//	���� ī�޶� 
		m_vCurSrc += (m_vPos - m_vCurSrc)*0.1f;				//	���� ī�޶�
		//m_vCurSrc	=	m_vPos;
	}
	//D3DXMatrixLookAtLH( &D3DFRAME->m_mView, &m_vPos, &m_vTarget_Y, &m_vUp );
	D3DXMatrixLookAtLH( &D3DFRAME->m_mView, &m_vCurSrc, &m_vCurTgt, &m_vUp );
	//D3DFRAME->SetViewMatrix( &GetViewMatrix() );
	D3DFRAME->SetPerspective ( 1.0f, 1.333f, 1.0f, 8000.0f );
}

// Straight, Back
void	CCamera::Walk( float move_distance )
{
	if( move_distance == 0.0f ) return;

	// ���� ī�޶�� y������ �̵��� �����Ѵ�.
	if( m_nCameraType == LAND_CAMERA )
	{
		D3DXVECTOR3		dir;
		D3DXVec3Cross( &dir, &m_vRight, &m_vUp );
		m_vPos	+=	D3DXVECTOR3( dir.x, 0.0f, dir.z ) * move_distance;
	}
	else if( m_nCameraType == AIR_CAMERA )
		m_vPos	+=	m_vLook * move_distance;
}

// Left, Right
void	CCamera::Strafe( float move_distance )
{
	if( move_distance == 0.0f ) return;

	// ���� ī�޶�� y������ �̵��� �����Ѵ�.
	if( m_nCameraType == LAND_CAMERA )
		m_vPos	+=	D3DXVECTOR3( m_vRight.x, 0.0f, m_vRight.z ) * move_distance;
	else if( m_nCameraType == AIR_CAMERA )
		m_vPos	+=	m_vRight * move_distance;
}

// Up, Down
void	CCamera::Fly( float move_distance )
{
	if( move_distance == 0.0f ) return;

	if( m_nCameraType == AIR_CAMERA )
		m_vPos	+=	m_vUp * move_distance;
}

// ���� ���� ȸ��
void	CCamera::Pitch( float angle )
{
	// right�� �������� ȸ����� ���
	D3DXMATRIX T;
	D3DXMatrixRotationAxis( &T, &m_vRight, angle );

	// right�� �������� up, look ȸ��
	//D3DXVec3TransformCoord( &m_vUp, &m_vUp, &T );	// �ɾ�ٴϴ� �ɸ��̱⿡ �����ʹ� ������ �ʴ´�
	D3DXVec3TransformCoord( &m_vLook, &m_vLook, &T );
}

// ���� ���� ȸ��
void	CCamera::Yaw( float angle )
{
	D3DXMATRIX T;

	// ���� ī�޶�� y���� �������� ȸ����� ���
	if( m_nCameraType == LAND_CAMERA )
	{
		D3DXMatrixRotationY( &T, angle );
		D3DXVec3TransformCoord( &m_vUp, &m_vUp, &T );
	}
	// ���� ī�޶�� up�� �������� ȸ����� ���
	else if( m_nCameraType == AIR_CAMERA )
		D3DXMatrixRotationAxis( &T, &m_vUp, angle );

	// up�� �������� right, look ȸ��
	D3DXVec3TransformCoord( &m_vRight, &m_vRight, &T );
	D3DXVec3TransformCoord( &m_vLook, &m_vLook, &T );
}

// ���� ���� ȸ��
void	CCamera::Roll( float angle )
{
	// ���� ī�޶� ȸ��
	if( m_nCameraType == AIR_CAMERA )
	{
		// look�� �������� ȸ����� ���
		D3DXMATRIX T;
		D3DXMatrixRotationAxis( &T, &m_vLook, angle );

		// look�� �������� up, right ȸ��
		D3DXVec3TransformCoord( &m_vUp, &m_vUp, &T );
		D3DXVec3TransformCoord( &m_vRight, &m_vRight, &T );
	}
}

void	CCamera::SetPlayerinfo( D3DXVECTOR3 Pos, D3DXVECTOR3 Right, D3DXVECTOR3 Up, D3DXVECTOR3 Look )
{
	m_vPlayerPos	=	Pos;
	m_vPlayerRight	=	Right;
	m_vPlayerUp		=	Up;
	m_vPlayerLook	=	Look;
}


D3DXMATRIX	CCamera::GetViewMatrix()
{

	// ���� ���� ����ȭ
	D3DXVec3Normalize( &m_vLook, &m_vLook );
	D3DXVec3Normalize( &m_vRight, &m_vRight );
	D3DXVec3Cross( &m_vUp, &m_vLook, &m_vRight );
	D3DXVec3Normalize( &m_vUp, &m_vUp );

	// ����� ����
	float x = -D3DXVec3Dot( &m_vRight, &m_vPos );
	float y = -D3DXVec3Dot( &m_vUp, &m_vPos );
	float z = -D3DXVec3Dot( &m_vLook, &m_vPos );

	return D3DXMATRIX(	m_vRight.x, m_vUp.x, m_vLook.x, 0.0f,
		m_vRight.y, m_vUp.y, m_vLook.y, 0.0f,
		m_vRight.z, m_vUp.z, m_vLook.z, 0.0f,
		x,			y,		 z,			1.0f );

}

CCamera*	CCamera::GetInst()
{
	if( !pCamera )
	{
		pCamera = new CCamera;
		RSCMGR->m_bCAM_MGR	=	true;
	}
	return pCamera;
}

void	CCamera::FreeInst()
{
	if( pCamera )
	{
		delete pCamera;
		pCamera = NULL;
		RSCMGR->m_bCAM_MGR	=	false;
	}
}