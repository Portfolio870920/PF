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
	 //카메라 이동
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
		m_vCurTgt += (m_vTarget_Y - m_vCurTgt)*0.1f;		//	가속 카메라 
		m_vCurSrc += (m_vPos + D3DXVECTOR3(0.0f,0.0f,0.0f) - m_vCurSrc)*0.1f;	//	가속 카메라
		m_vPos.z+=25.0f;	//계속 멀어졌다 가까워졌다 하기
		Yaw(m_fangle);		//카메라를 회전시킨다
		m_fangle+=0.005f;	//회전각을 증가시킨다
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
		m_vCurTgt += (m_vTarget_Y - m_vCurTgt)*0.1f;		//	가속 카메라 
		m_vCurSrc += (m_vPos - m_vCurSrc)*0.1f;				//	가속 카메라
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

	// 지상 카메라는 y축으로 이동을 제한한다.
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

	// 지상 카메라는 y축으로 이동을 제한한다.
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

// 우향 벡터 회전
void	CCamera::Pitch( float angle )
{
	// right를 기준으로 회전행렬 계산
	D3DXMATRIX T;
	D3DXMatrixRotationAxis( &T, &m_vRight, angle );

	// right를 기준으로 up, look 회전
	//D3DXVec3TransformCoord( &m_vUp, &m_vUp, &T );	// 걸어다니는 케릭이기에 업벡터는 변하지 않는다
	D3DXVec3TransformCoord( &m_vLook, &m_vLook, &T );
}

// 상향 벡터 회전
void	CCamera::Yaw( float angle )
{
	D3DXMATRIX T;

	// 지상 카메라는 y축을 기준으로 회전행렬 계산
	if( m_nCameraType == LAND_CAMERA )
	{
		D3DXMatrixRotationY( &T, angle );
		D3DXVec3TransformCoord( &m_vUp, &m_vUp, &T );
	}
	// 공중 카메라는 up을 기준으로 회전행렬 계산
	else if( m_nCameraType == AIR_CAMERA )
		D3DXMatrixRotationAxis( &T, &m_vUp, angle );

	// up를 기준으로 right, look 회전
	D3DXVec3TransformCoord( &m_vRight, &m_vRight, &T );
	D3DXVec3TransformCoord( &m_vLook, &m_vLook, &T );
}

// 전방 벡터 회전
void	CCamera::Roll( float angle )
{
	// 공중 카메라만 회전
	if( m_nCameraType == AIR_CAMERA )
	{
		// look을 기준으로 회전행렬 계산
		D3DXMATRIX T;
		D3DXMatrixRotationAxis( &T, &m_vLook, angle );

		// look을 기준으로 up, right 회전
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

	// 방향 벡터 정규화
	D3DXVec3Normalize( &m_vLook, &m_vLook );
	D3DXVec3Normalize( &m_vRight, &m_vRight );
	D3DXVec3Cross( &m_vUp, &m_vLook, &m_vRight );
	D3DXVec3Normalize( &m_vUp, &m_vUp );

	// 뷰행렬 구성
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