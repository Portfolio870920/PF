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
	float	m_Damage;			//	데미지
	float	m_Stamina;			//	Stamina
	float	m_MaxHp;			//  풀 hp
	float	m_MaxSt;			//  풀 스태미너

	float	m_fAttackSpeed;		//	공격 속도

	int		m_RealCombo;		//	콤보 갯수
	bool	m_CheckSkill;		//	스킬이 나갔을 경우 1
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
		LANDOBJECT,				//	땅거지	모드
		ATRCRAFT				//	비행	모드
	};

	POINT				m_pos_MouseCursor;		//	마우스 X좌표

	//	Player value

	PlayerType			m_PlayerType;			//	플레이어가 바닥을 기어야하는가?
	Play_Status			m_Status1;				//	케릭터 스테이터스
	CSkinController		m_cPlayerController;
	CSkinRenderer		m_cSkinRenderer;

	int				m_ComboBuffer[2];	//	콤보 버퍼
	bool			m_ActionCheck;		//	공격이 성공
	bool			m_HitCheck;			//	공격이 성공 버프에 필요함
	float			m_Time;				//	공격한 시간
	float			m_NewTime;			//	현재 시간
	float			m_HitTime;			//	공격이 성공한 시간	
	int				m_CheckHP;			//	HP 체크


	STRAIL_VERTEX	m_vTrail[15*2];
	D3DXVECTOR3		m_vPos;				//	위취 벡터
	D3DXVECTOR3		m_vRight;			//	우향 벡터
	D3DXVECTOR3		m_vUp;				//	상향 벡터
	D3DXVECTOR3		m_vLook;			//	전방 벡터
	D3DXVECTOR3		m_vRot;				//	회전 벡터

	// 미끄러짐 벡터 부분 //
	D3DXVECTOR3		m_vSlideVec;		//	미끄러짐 벡터
	D3DXVECTOR3		m_vPosNormal;		//	현 위치 법선벡터

	float			m_fangle;			//	방향
	float			m_fcamangle;		//	카메라 상하 각
	float			m_Velocity;			//	속도

	//	Weapon	value
	float			m_ActionLen;		//	플레이어가 타격한 위치의 
	float			m_Weapon_Length;	//	무기 범위
	int				m_Weapon_Damage;	//	무기 데미지
	int				m_Weapon;			//  무기 선택

	//	buff	value
	int				m_BuffSwith;		//	버프 선택
	bool			m_BuffTimeCheck;	//	버프가 사용 가능하면 true
	float			m_BuffTime;			//	버프가 바뀐 시간
	int				m_SaveWeapon_Damege;//	데미지 저장
	float			m_SaveSpeed;		//	스피드 저장
	bool			m_Vamp;				//	흡혈된 상태
	bool			m_DamegeUp;			//	데미지 업이라면 true
	bool			m_SpeedUp;			//	스피드가 빨라졌다면 true
	bool			m_StaminaUp;		//	스테미너가 빨리 찬다면 true


	Cplayer(void);
	~Cplayer(void);

	//	funtion

	void	strafe	( float units );					//	왼쪽/오른쪽
	void	fly		( float units );					//	위쪽/아래쪽
	void	Walk	( float units );					//	앞쪽/뒤쪽
	void	pitch	( float angle );					//	우향 벡터로 회전
	void	yaw		( float angle );					//	상향 벡터로 회전
	void	roll	( float angle );					//	전방 벡터로 회전

	void	StatusMainSysstem( void );					//	스테이터스 메인
	void	SetStatusMove( void );						//	스테이터스 이동
	void	SetStatusInfo( void );						//	스테이터스 정보 초기화
	void	SetActionStatus( void );					//	스테이터스 모션
	void	AtteckCheck( void );						//	플레이어 공격 판정 체크

	void	SetCombo	( void );						//	콤보 입력
	int		GetCombo	( void );						//	콤보 출력
	void	SetBuff		( void );						//	버프 입력
	void	CheckBuffTime	( void );					//	버프 타임 체크
	void	CheckBuffSwith	( void );					//	버프 스위치 체크 하여 실행

	////////				STATUS	INFO			///////////

	void	STANCE			( void );							//	서잇음
	void	MOVE			( void );							//	이동
	void	VERTICAL		( void );							//	기본 공격
	void	HORIZON_LEFT	( void );							//	좌베기
	void	HORIZON_RIGHT	( void );							//	우베기
	void	JUMPCUT			( void );							//	점프
	void	DIE				( void );							//	죽음
	void	RUN				( void );							//  뛰기
	void	DOUBLESTAB		( void );							//	두번 찌르기
	void	DOUBLECUT		( void );							//	두번 베기
	void	DASH			( void );							//	회피 대쉬
	void	BEHIT			( void );							//	아픔


	////////				Collision	INFO			///////////
	D3DXVECTOR3 SetPlayerCrash( D3DXVECTOR3 Monster_Pos );					//	충돌 체크
	bool		CheckPlayerActionCrash( D3DXVECTOR3 Monster_Pos );			//	타격 범위 체크
	bool		CheckActionFrameCrash( D3DXVECTOR3 Monster_Pos );			//	프레임 체크
	void		CheckSkill( void );											//	스킬 체크
	void		DashEffectRight( float velocity );
	void		DashEffectLook( float velocity );
	void		DashEffectUpdate( void );
	void		DashEffectRenDer( void );

public:

	static	Cplayer*	Player_Inst;
	static	Cplayer*	GetInst();
	void	FreeInst();

	void	Init	(  int Weapon );					//	초기화
	void	Update	( void );							//	정보 업데이트
	void	Render	( void );							//	그리기

	void		CheckBEHIT( void );						//	피격 몬스터에서 실행시켜야됨


	Play_Status GetPlay_Status( void )					//	플레이어 스테이터스를 반환함
	{
		return m_Status1;
	}
	inline	D3DXVECTOR3	GetPos( void )
	{
		return m_vPos;
	}
	//	버프 스위치 반환
	inline	int		GetBuffSwith( void )
	{
		return m_BuffSwith;
	}
	//	버프 타임체크 반환
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
	inline	void	SetDamageCheck( int Damage )	//	데미지 체크
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
	inline	bool GetBuffStamina( void )		//	스테미너 버프 상태 true 면 켜진것
	{
		return m_StaminaUp;
	}
};