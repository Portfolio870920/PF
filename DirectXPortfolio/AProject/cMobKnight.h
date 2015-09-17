#pragma once
//기사 클래스
//#include "MainHeader.h"

class cMobKnight : public CMonster
{


public:
	CSkinController		m_cMobKnightController;
	CSkinRenderer		m_SkinRenderer;
	cCollision			m_Collision;
	Play_Status			m_Status;
	CParticleSprite		m_ParticleSprite;

	D3DXVECTOR3		m_vPos;			//	위치 벡터
	D3DXVECTOR3		m_vRight;
	D3DXVECTOR3		m_vUp;
	D3DXVECTOR3		m_vLook;
	D3DXVECTOR3		m_vRot;
	D3DXVECTOR3		m_vScl;//스케일

	// 미끄러짐 벡터 부분 //
	D3DXVECTOR3		m_vSlideVec;		//	미끄러짐 벡터
	D3DXVECTOR3		m_vPosNormal;		//	현 위치 법선벡터

	D3DXVECTOR3		m_vMonster_dir;		//	몬스터가 가야할 방향 벡터
	D3DXVECTOR3		m_vRelease_Pos;		

	float			m_fangle;			//	방향
	float			m_Velocity;			//	속도
	float			m_fLen;				//	플레이어를 가르키는 벡터와 몬스터의 벡터가 일지할 경우 0과 가깝게 나옴
	float			m_fDot;				//	플레이어를 가르키는 벡터와 몬스터의 벡터가 일치할 경우 1과 가깝게 나옴
	bool			m_ActionCheck;		//	공격판정 체크
	float			m_CheckHP;			//	HP 체크
	bool			m_FirstCheck;		//	처음 범위에 들어왔을경우
	bool			m_BeHitCheck;		//	피격시 true
	float			m_HitTime;			//	처음 맞은 시간
	float			m_HitEndTime;		//	피격시 타임

	D3DXVECTOR3		m_ActionPos;		//	몬스터 공격 위치
	float			m_ActionLen;		//	몬스터 공격 범위
	int				m_ActionSwith;		//	몬스터의 액션 상태 변수
	float			m_ActionStartTime;	//	시작한 액션 상태 타임
	bool			m_ActionEndTime;	//	끝나는 액션 상태를 확인할 타임

	cMobKnight(void);
	virtual ~cMobKnight(void);
	void	Init	( D3DXVECTOR3 MobPos = D3DXVECTOR3(0.0f,0.0f,0.0f), D3DXVECTOR3 MobScl = D3DXVECTOR3(0.0f,0.0f,0.0f), float MobAngle = 0.0f );								//	초기화
	void	Update	( void );								//	정보 업데이트
	void	Render	( void );								//	그리기
	void	SetStatusInfo	( void );						//	스테이터스 정보 초기화

	void	yaw		( float angle );						//	y축으로 회전

	void	DIE				( void );
	void	STANCE			( void );
	void	MOVE			( void );
	void	VERTICAL		( void );							//	기본공격1
	void	VERTICAL_RIGHT	( void );							//	기본공격2

	void	AI_MainSystem	( void );						//	AI
	void	AI_StatusMove	( D3DXVECTOR3 Pos );			//	AI MOVE
	void	AI_StatusAction	( void );						//	AI Action

	void	HITCHECK();										//	피격시 카운트로 해제

	D3DXVECTOR3	SetMonsterCrash	( D3DXVECTOR3 Monster_Pos );	//	충돌 체크
	void	CollisionCEHCKATTACK( void );						//	판정 체크 종합선물 세트
	bool	CheckActionCrash( D3DXVECTOR3 Player_Pos );			//	공격 판정 체크
	bool	CheckActionFrameCrash( D3DXVECTOR3 Player_Pos );	//	공격 프레임 체크
	void	CheckActionPos( void );								//	공격 위치 범위 체크

	float	SetLength(D3DXVECTOR3 Player_Pos);				//	Player to Monster Length
	void	SetLook( D3DXVECTOR3 Player_Pos );				//	Monster to Cross
	inline void Setangle( float angle )
	{
		m_fangle	-=	angle;
	}
	inline	void SetPos_y( float y )
	{
		m_vPos.y	=	y;
	}

	D3DXVECTOR3 GetMonsterPos( void );						//	몬스터 위치를 던져줌
	Play_Status	GetPlay_Status( void );						//	몬스터 스테이터스 날려줌
	inline	bool	GetBeHit( void )						//	이녀석이 본인이 맞으면 true로 날림
	{
		return m_BeHitCheck;
	}
	inline	void	SetDamageCheck( unsigned int Damage, bool BeHitCheck )
	{
		//m_HitTime	=	TIMEMGR->GetTotalTIme();
		m_Status.m_HP	-= Damage;
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
	inline  Play_Status GetStatus( void )
	{
		return m_Status;
	}
	inline	D3DXVECTOR3 GetActionPos( void )
	{
		return m_ActionPos;
	}
	inline	float GetActionLen( void )
	{
		return m_ActionLen;
	}
};