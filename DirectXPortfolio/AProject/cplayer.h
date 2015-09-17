#pragma once

struct Play_Status
{
	int		m_Status;
	bool	m_Dead;
	int		StartFrame;
	int		EndFrame;
	float	PlayTime;
	float	Smotion;
	bool	ActionCansle;

	float	m_HP;				//	HP
	float	m_Damage;			//	������
	float	m_Stamina;			//	Stamina
	float	m_MaxHp;			//  Ǯ hp
	float	m_MaxSt;			//  Ǯ ���¹̳�

	float	m_fAttackSpeed;		//	���� �ӵ�

	int		m_RealCombo;		//	�޺� ����
	bool	m_CheckSkill;		//	��ų�� ������ ��� 1
};

struct STRAIL_VERTEX 
{
	D3DXVECTOR3	p;
	DWORD		c;
};
class Cplayer
{
private:
	enum	PlayerType
	{
		LANDOBJECT,				//	������	���
		ATRCRAFT				//	����	���
	};

	POINT				m_pos_MouseCursor;		//	���콺 X��ǥ

	//	Player value

	PlayerType			m_PlayerType;			//	�÷��̾ �ٴ��� �����ϴ°�?
	Play_Status			m_Status1;				//	�ɸ��� �������ͽ�
	CSkinController		m_cPlayerController;
	CSkinRenderer		m_cSkinRenderer;

	int				m_ComboBuffer[2];	//	�޺� ����
	bool			m_ActionCheck;		//	������ ����
	bool			m_HitCheck;			//	������ ���� ������ �ʿ���
	float			m_Time;				//	������ �ð�
	float			m_NewTime;			//	���� �ð�
	float			m_HitTime;			//	������ ������ �ð�	
	int				m_CheckHP;			//	HP üũ


	STRAIL_VERTEX	m_vTrail[15*2];
	D3DXVECTOR3		m_vPos;				//	���� ����
	D3DXVECTOR3		m_vRight;			//	���� ����
	D3DXVECTOR3		m_vUp;				//	���� ����
	D3DXVECTOR3		m_vLook;			//	���� ����
	D3DXVECTOR3		m_vRot;				//	ȸ�� ����

	// �̲����� ���� �κ� //
	D3DXVECTOR3		m_vSlideVec;		//	�̲����� ����
	D3DXVECTOR3		m_vPosNormal;		//	�� ��ġ ��������

	float			m_fangle;			//	����
	float			m_fcamangle;		//	ī�޶� ���� ��
	float			m_Velocity;			//	�ӵ�

	//	Weapon	value
	float			m_ActionLen;		//	�÷��̾ Ÿ���� ��ġ�� 
	float			m_Weapon_Length;	//	���� ����
	int				m_Weapon_Damage;	//	���� ������
	int				m_Weapon;			//  ���� ����

	//	buff	value
	int				m_BuffSwith;		//	���� ����
	bool			m_BuffTimeCheck;	//	������ ��� �����ϸ� true
	float			m_BuffTime;			//	������ �ٲ� �ð�
	int				m_SaveWeapon_Damege;//	������ ����
	float			m_SaveSpeed;		//	���ǵ� ����
	bool			m_Vamp;				//	������ ����
	bool			m_DamegeUp;			//	������ ���̶�� true
	bool			m_SpeedUp;			//	���ǵ尡 �������ٸ� true
	bool			m_StaminaUp;		//	���׹̳ʰ� ���� ���ٸ� true


	Cplayer(void);
	~Cplayer(void);

	//	funtion

	void	strafe	( float units );					//	����/������
	void	fly		( float units );					//	����/�Ʒ���
	void	Walk	( float units );					//	����/����
	void	pitch	( float angle );					//	���� ���ͷ� ȸ��
	void	yaw		( float angle );					//	���� ���ͷ� ȸ��
	void	roll	( float angle );					//	���� ���ͷ� ȸ��

	void	StatusMainSysstem( void );					//	�������ͽ� ����
	void	SetStatusMove( void );						//	�������ͽ� �̵�
	void	SetStatusInfo( void );						//	�������ͽ� ���� �ʱ�ȭ
	void	SetActionStatus( void );					//	�������ͽ� ���
	void	AtteckCheck( void );						//	�÷��̾� ���� ���� üũ

	void	SetCombo	( void );						//	�޺� �Է�
	int		GetCombo	( void );						//	�޺� ���
	void	SetBuff		( void );						//	���� �Է�
	void	CheckBuffTime	( void );					//	���� Ÿ�� üũ
	void	CheckBuffSwith	( void );					//	���� ����ġ üũ �Ͽ� ����

	////////				STATUS	INFO			///////////

	void	STANCE			( void );							//	������
	void	MOVE			( void );							//	�̵�
	void	VERTICAL		( void );							//	�⺻ ����
	void	HORIZON_LEFT	( void );							//	�º���
	void	HORIZON_RIGHT	( void );							//	�캣��
	void	JUMPCUT			( void );							//	����
	void	DIE				( void );							//	����
	void	RUN				( void );							//  �ٱ�
	void	DOUBLESTAB		( void );							//	�ι� ���
	void	DOUBLECUT		( void );							//	�ι� ����
	void	DASH			( void );							//	ȸ�� �뽬
	void	BEHIT			( void );							//	����


	////////				Collision	INFO			///////////
	D3DXVECTOR3 SetPlayerCrash( D3DXVECTOR3 Monster_Pos );					//	�浹 üũ
	bool		CheckPlayerActionCrash( D3DXVECTOR3 Monster_Pos );			//	Ÿ�� ���� üũ
	bool		CheckActionFrameCrash( D3DXVECTOR3 Monster_Pos );			//	������ üũ
	void		CheckSkill( void );											//	��ų üũ
	void		DashEffectRight( float velocity );
	void		DashEffectLook( float velocity );
	void		DashEffectUpdate( void );
	void		DashEffectRenDer( void );

public:

	static	Cplayer*	Player_Inst;
	static	Cplayer*	GetInst();
	void	FreeInst();

	void	Init	(  int Weapon );					//	�ʱ�ȭ
	void	Update	( void );							//	���� ������Ʈ
	void	Render	( void );							//	�׸���

	void		CheckBEHIT( void );						//	�ǰ� ���Ϳ��� ������Ѿߵ�


	Play_Status GetPlay_Status( void )					//	�÷��̾� �������ͽ��� ��ȯ��
	{
		return m_Status1;
	}
	inline	D3DXVECTOR3	GetPos( void )
	{
		return m_vPos;
	}
	//	���� ����ġ ��ȯ
	inline	int		GetBuffSwith( void )
	{
		return m_BuffSwith;
	}
	//	���� Ÿ��üũ ��ȯ
	inline	bool	GetBuffTimeCehck( void )
	{
		return m_BuffTimeCheck;
	}
	inline	bool	SetWeapon(int Weapon)
	{
		m_Weapon	=	Weapon;
		return true;
	}

	inline  void	SetStatus_HP( float HP )
	{
		m_Status1.m_HP	=	HP;
	}
	inline	void	SetDamageCheck( int Damage )	//	������ üũ
	{
		m_Status1.m_HP -= Damage;
	}
	inline	void SetPos_y( float y )
	{
		m_vPos.y	=	y;
	}
	inline	void SetPos(D3DXVECTOR3 Pos )
	{
		m_vPos	=	Pos;
	}
	inline	bool GetBuffStamina( void )		//	���׹̳� ���� ���� true �� ������
	{
		return m_StaminaUp;
	}
};