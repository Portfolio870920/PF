#pragma once
// 여러 몹들 클래스의 부모 역할을 함
//#include "MainHeader.h"

class CMonster
{

protected:

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

	//	Check & Time
	bool			m_ActionCheck;		//	공격판정 체크
	int				m_CheckHP;			//	HP 체크
	bool			m_FirstCheck;		//	처음 범위에 들어왔을경우
	bool			m_BeHitCheck;		//	피격시 true
	float			m_HitTime;			//	처음 맞은 시간
	float			m_HitEndTime;		//	피격시 타임
	D3DXVECTOR3		m_ActionPos;		//	몬스터 공격 위치
	float			m_ActionLen;		//	몬스터 공격 범위

	int				m_ActionSwith;		//	몬스터의 액션 상태 변수
	float			m_ActionStartTime;	//	시작한 액션 상태 타임
	//float			m_ActionEndTime;	//	끝나는 액션 상태를 확인할 타임
	bool			m_ActionChoiceCheck;//	액션 상태 초이스가 되면 true

	
public:

	CMonster(void);
	virtual ~CMonster(void);

	virtual	void	Init	( D3DXVECTOR3 MobPos = D3DXVECTOR3(0.0f,0.0f,0.0f), D3DXVECTOR3 MobScl = D3DXVECTOR3(0.0f,0.0f,0.0f), float MobAngle = 0.0f )	=	0;								//	초기화
	virtual	void	Update	( void )	=	0;								//	정보 업데이트
	virtual	void	Render	( void )	=	0;								//	그리기
	virtual	void	SetStatusInfo	( void )	=	0;						//	스테이터스 정보 초기화

	virtual	void	yaw		( float angle )	=	0;						//	y축으로 회전
	

	virtual	void	AI_MainSystem	( void )	=	0;						//	AI
	virtual	void	AI_StatusMove	( D3DXVECTOR3 Pos )	=	0;						//	AI MOVE
	virtual	void	AI_StatusAction	( void )	=	0;						//	AI Action

	virtual	bool	CheckActionCrash( D3DXVECTOR3 Player_Pos )	=	0;			//	공격 판정 체크
	virtual	bool	CheckActionFrameCrash( D3DXVECTOR3 Player_Pos )	=	0;	//	공격 프레임 체크

	virtual	float	SetLength(D3DXVECTOR3 Player_Pos)	=	0;				//	Player to Monster Length
	virtual	void	SetLook( D3DXVECTOR3 Player_Pos )	=	0;				//	Monster to Cross
	virtual inline	void	SetBeHitTime( float time )	=	0;

	virtual	D3DXVECTOR3 GetMonsterPos( void )	=	0;						//	몬스터 위치를 던져줌
	virtual	Play_Status	GetPlay_Status( void )	=	0;						//	몬스터 스테이터스 날려줌
	virtual	inline	void	SetDie_Staus( bool	bDie )	=	0;					//	죽었는지 살았는지 강제로 넣어줌
	virtual inline	bool	GetBeHit( void )	=	0;
	virtual	inline	void	SetDamageCheck( unsigned int Damage, bool BeHitCheck )	=	0;
	virtual	inline	D3DXVECTOR3	GetPos( void )	=	0;
	virtual	inline	D3DXVECTOR3 GetRight( void )	=	0;
	virtual	inline	D3DXVECTOR3 GetUp( void )	=	0;
	virtual	inline	D3DXVECTOR3 GetLook( void )	=	0;
	virtual	inline  Play_Status	GetStatus( void ) = 0;
	virtual inline  D3DXVECTOR3 GetActionPos( void ) = 0;
	virtual inline	float GetActionLen( void )	=	0;
};