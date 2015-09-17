#pragma once

class CMobAlduin :
	public CMonster
{
private:
	CSkinController		m_cMobAlduinController;
	CSkinRenderer		m_SkinRenderer;
	cCollision			m_Collision;
	Play_Status			m_Status;
	CParticleSprite		m_ParticleSprite;

	D3DXVECTOR3		m_AcitonPos;	//	���� ���� ��ġ
	float			m_ActionLen;
	int		m_Monsterchoise;	//	���Ͱ� �������� ��
	bool	m_bDashHit;
	bool	m_CheckBREATH;	//	�극���� �ѹ� ������ true

	PSystem*		pBreath1;
	PSystem*		pBreath2;
	D3DXVECTOR3		vVel;
	D3DXVECTOR3		vPivot;

	PSystem* 		pGrass;	
	D3DXVECTOR3 	vPivot2;

public:
	CMobAlduin(void);
	virtual ~CMobAlduin(void);

	void	Init	( D3DXVECTOR3 MobPos = D3DXVECTOR3(0.0f,0.0f,0.0f), D3DXVECTOR3 MobScl = D3DXVECTOR3(0.0f,0.0f,0.0f), float MobAngle = 0.0f );								//	�ʱ�ȭ
	void	Update	( void );								//	���� ������Ʈ
	void	Render	( void );								//	�׸���
	void	SetStatusInfo	( void );						//	�������ͽ� ���� �ʱ�ȭ
	void	SetAction		( void );						//	�׼��� ���ϴ� �Լ�

	void	yaw		( float angle );						//	y������ ȸ��

	//	STATUS SET
	void	FIRSTSHOW		( void );						//	ù ����
	void	DIE				( void );
	void	STANCE			( void );
	void	MOVE			( void );
	void	VERTICAL		( void );						//	�⺻����1
	void	BREATH			( void );						//	�극��
	void	LEFT_BREATH		( void );						//	�� �극��
	void	RIGHT_BREATH	( void );						//	�� �극��
	void	JUMP_DRAGON_BREATH	( void );					//	���� �� �극�� ����
	void	DASH			( void );						//	�뽬


	void	AI_MainSystem	( void );						//	AI
	void	AI_StatusMove	( D3DXVECTOR3 Pos );			//	AI MOVE
	void	AI_StatusAction	( void );						//	AI Action

	void	HITCHECK();										//	�ǰݽ� ī��Ʈ�� ����
	void	SetBREATHAction( void );						//	�������ͽ��� �극���� ������ ��� ��ƼŬ �Ѹ�

	D3DXVECTOR3	SetMonsterCrash	( D3DXVECTOR3 Monster_Pos );	//	�浹 üũ
	bool	CheckActionCrash( D3DXVECTOR3 Player_Pos );			//	���� ���� üũ
	bool	CheckActionFrameCrash( D3DXVECTOR3 Player_Pos );	//	���� ������ üũ

	float	SetLength(D3DXVECTOR3 Player_Pos);				//	Player to Monster Length
	void	SetLook( D3DXVECTOR3 Player_Pos );				//	Monster to Cross

	inline	D3DXVECTOR3 GetMonsterPos( void )						//	���� ��ġ�� ������
	{
		return m_vPos;
	}
	inline	Play_Status	GetPlay_Status( void )						//	���� �������ͽ� ������
	{
		return m_Status;
	}

	inline	bool	GetBeHit( void )		//	������ true ����
	{
		return m_BeHitCheck;
	}
	inline	void	SetDamageCheck( unsigned int Damage, bool BeHitCheck )
	{
		//m_HitTime	=	TIMEMGR->GetTotalTIme();
		m_Status.m_HP -= Damage;
		m_BeHitCheck	=	BeHitCheck;
	}
	inline	void	SetBeHitTime( float time )
	{
		m_HitTime	=	time;
	}
	inline	D3DXVECTOR3	GetPos( void )
	{
		return m_vPos;
	}
	inline	D3DXVECTOR3 GetRight( void )
	{
		return m_vRight;
	}
	inline	D3DXVECTOR3 GetUp( void )
	{
		return m_vUp;
	}
	inline	D3DXVECTOR3 GetLook( void )
	{
		return m_vLook;
	}
	inline	void	SetDie_Staus( bool	bDie )
	{
		m_Status.m_Dead	=	true;
	}
	inline  Play_Status	GetStatus( void )
	{
		return m_Status;
	}
	inline  D3DXVECTOR3 GetActionPos( void )
	{
		return m_AcitonPos;
	}
	inline	float GetActionLen( void )
	{
		return m_ActionLen;
	}
};
