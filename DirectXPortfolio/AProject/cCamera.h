#pragma once

enum CAMERA_TYPE
{
	LAND_CAMERA,			//	땅거지	형
	AIR_CAMERA,				//	자유	형
};

class CCamera
{
	bool			m_bGetVec;//케릭터 좌표 받았는지 체크
	float			m_fangle;
	int				m_nCameraType;
	D3DXVECTOR3		m_vRight;
	D3DXVECTOR3		m_vUp;
	D3DXVECTOR3		m_vLook;

	D3DXVECTOR3		m_vPlayerPos;
	D3DXVECTOR3		m_vPlayerRight;
	D3DXVECTOR3		m_vPlayerUp;
	D3DXVECTOR3		m_vPlayerLook;
	

	D3DXVECTOR3		m_vCurSrc;
	D3DXVECTOR3		m_vCurTgt;

	bool			m_bKey;

	float			m_fVelocity;
	static	CCamera*	pCamera;

public:
	static	CCamera*	GetInst();
	D3DXVECTOR3		m_vPos;
	D3DXVECTOR3		m_vTarget_Y;

	void	FreeInst();
	void	Init( int cameraType = 0, bool keyEnable = false, float velocity = 10.0f );
	void	Update( );
	void	Walk( float move_distance );		// Straight, Back
	void	Strafe( float move_distance );		// Left, Right
	void	Fly( float move_distance );			// Up, Down

	void	Pitch( float angle );		// 우향 벡터 회전
	void	Yaw( float angle );			// 상향 벡터 회전
	void	Roll( float angle );		// 전방 벡터 회전

	D3DXMATRIX	GetViewMatrix();
	void	SetPlayerinfo( D3DXVECTOR3 Pos, D3DXVECTOR3 Right, D3DXVECTOR3 Up, D3DXVECTOR3 Look );

public:
	CCamera(void);
	~CCamera(void);

	// Set
	inline	void	SetCameraType( CAMERA_TYPE cameraType )	{	m_nCameraType	=	cameraType;	}
	inline	void	SetPosition( D3DXVECTOR3 pos )			{	m_vPos = pos;		}
	inline	void	SetLook( D3DXVECTOR3 look )				{	m_vLook = look;		}
	inline	void	SetRight( D3DXVECTOR3 right )			{	m_vRight = right;	}
	inline	void	SetbGetVec( void )						{	if ( !m_bGetVec )	m_bGetVec = true;	}

	// Get
	inline	D3DXVECTOR3		GetPosition()	{	return m_vPos;		}
	inline	D3DXVECTOR3		GetRight()		{	return m_vRight;	}
	inline	D3DXVECTOR3		GetUp()			{	return m_vUp;		}
	inline	D3DXVECTOR3		GetLook()		{	return m_vLook;		}
};