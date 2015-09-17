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

	D3DXVECTOR3		m_AcitonPos;	//	몬스터 공격 위치
	float			m_ActionLen;
	int		m_Monsterchoise;	//	몬스터가 프레임을 고름
	bool	m_bDashHit;
	bool	m_CheckBREATH;	//	브레스가 한번 나가면 true

	PSystem*		pBreath1;
	PSystem*		pBreath2;
	D3DXVECTOR3		vVel;
	D3DXVECTOR3		vPivot;

	PSystem* 		pGrass;	
	D3DXVECTOR3 	vPivot2;

public:
	CMobAlduin(void);
	virtual ~CMobAlduin(void);

	void	Init	( D3DXVECTOR3 MobPos = D3DXVECTOR3(0.0f,0.0f,0.0f), D3DXVECTOR3 MobScl = D3DXVECTOR3(0.0f,0.0f,0.0f), float MobAngle = 0.0f );								//	초기화
	void	Update	( void );								//	정보 업데이트
	void	Render	( void );								//	그리기
	void	SetStatusInfo	( void );						//	스테이터스 정보 초기화
	void	SetAction		( void );						//	액션을 정하는 함수

	void	yaw		( float angle );						//	y축으로 회전

	//	STATUS SET
	void	FIRSTSHOW		( void );						//	첫 등장
	void	DIE				( void );
	void	STANCE			( void );
	void	MOVE			( void );
	void	VERTICAL		( void );						//	기본공격1
	void	BREATH			( void );						//	브레스
	void	LEFT_BREATH		( void );						//	좌 브레스
	void	RIGHT_BREATH	( void );						//	우 브레스
	void	JUMP_DRAGON_BREATH	( void );					//	점프 후 브레스 공격
	void	DASH			( void );						//	대쉬


	void	AI_MainSystem	( void );						//	AI
	void	AI_StatusMove	( D3DXVECTOR3 Pos );			//	AI MOVE
	void	AI_StatusAction	( void );						//	AI Action

	void	HITCHECK();										//	피격시 카운트로 해제
	void	SetBREATHAction( void );						//	스테이터스가 브레스로 들어왔을 경우 파티클 뿌림

	D3DXVECTOR3	SetMonsterCrash	( D3DXVECTOR3 Monster_Pos );	//	충돌 체크
	bool	CheckActionCrash( D3DXVECTOR3 Player_Pos );			//	공격 판정 체크
	bool	CheckActionFrameCrash( D3DXVECTOR3 Player_Pos );	//	공격 프레임 체크

	float	SetLength(D3DXVECTOR3 Player_Pos);				//	Player to Monster Length
	void	SetLook( D3DXVECTOR3 Player_Pos );				//	Monster to Cross

	inline	D3DXVECTOR3 GetMonsterPos( void )						//	몬스터 위치를 던져줌
	{
		return m_vPos;
	}
	inline	Play_Status	GetPlay_Status( void )						//	몬스터 스테이터스 날려줌
	{
		return m_Status;
	}

	inline	bool	GetBeHit( void )		//	맞으면 true 보냄
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
