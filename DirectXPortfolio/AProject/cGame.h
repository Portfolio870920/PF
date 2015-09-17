#pragma once

class CGame : public CScene
{
private:
	D3DXVECTOR3			vNormal;	// 현재 위치의 법선을 저장할 벡터
	float				fNomalAngle;// 현재 위치의 법선과 Y축 내적값을 저장할 변수
	CUiBase*			cUiBase;	// UI만드는클래스
	POINT				CMouse;		// 마우스 
	bool				m_bChoiceWp;// 무기선택 유무 체크
	bool				m_bSelectEnemy;	//적선택 유무 체크
	bool				m_bSelectDifficulty;	//난이도선택 유무 체크
	float				PlayerHp;	// 플레이어 체력 변화수치 
	float				MonsterHp;	// 몬스터   체력 변화수치
	float				PlayerSt;	// 플레이어 스태미너 변화수치
	float				Bar;		// 체력바및 스태미너 바 포지션 변수
	BOOL				m_bIsStop;	// 업데이트상태를 멈추는지 안멈추는지 체크
	BOOL				m_bIsShowMS;// 마우스가 보이고 있는지 안보이고 있는지 체크


	//체력바
	VERTEX2D	*PlayerHPBack;			//플레이어 HP백
	LPDIRECT3DTEXTURE9	pTexPlayerHPB;

	//스태미나
	VERTEX2D	*PlayerSTBack;	//플레이어 ST백
	VERTEX2D	*PlayerSTFront;
	LPDIRECT3DTEXTURE9	pTexPlayerSTF;	//플레이어 ST프론트
	LPDIRECT3DTEXTURE9	pTexPlayerSTF2;

	VERTEX2D	*PlayerHPFront;
	LPDIRECT3DTEXTURE9	pTexPlayerHPF;	//플레이어 HP프론트

	VERTEX2D	*BossHPBack;			//보스 HP백
	LPDIRECT3DTEXTURE9	pTexBossHPB;

	VERTEX2D	*BossHPFront;
	LPDIRECT3DTEXTURE9	pTexBossHPF;	//보스 HP프론트

	//버프
	VERTEX2D	*Skill1;
	VERTEX2D	*Buff1;
	VERTEX2D	*Buff2;
	VERTEX2D	*Buff3;
	VERTEX2D	*Buff4;

	//비선택
	LPDIRECT3DTEXTURE9	pTexVamp;
	LPDIRECT3DTEXTURE9	pTexStamina;
	LPDIRECT3DTEXTURE9	pTexSpeed;
	LPDIRECT3DTEXTURE9	pTexDamage;
	//선택
	LPDIRECT3DTEXTURE9	pTexSVamp;
	LPDIRECT3DTEXTURE9	pTexSStamina;
	LPDIRECT3DTEXTURE9	pTexSSpeed;
	LPDIRECT3DTEXTURE9	pTexSDamage;

	VERTEX2D	*Arrow;	//화살표
	//화살표방향
	LPDIRECT3DTEXTURE9	pTexArrow0;
	LPDIRECT3DTEXTURE9	pTexArrow1;
	LPDIRECT3DTEXTURE9	pTexArrow2;
	LPDIRECT3DTEXTURE9	pTexArrow3;
	LPDIRECT3DTEXTURE9	pTexArrow4;

	//암전
	LPDIRECT3DTEXTURE9	pTexDVamp;
	LPDIRECT3DTEXTURE9	pTexDStamina;
	LPDIRECT3DTEXTURE9	pTexDSpeed;
	LPDIRECT3DTEXTURE9	pTexDDamage;

	//무기 선택
	VERTEX2D	*ChoiceWeapon;   
	LPDIRECT3DTEXTURE9	pChoiceWeapon;	// 무기선택

	LPDIRECT3DTEXTURE9	pDualSword;		//쌍수
	LPDIRECT3DTEXTURE9	pGreatSword;	//양손

	int Select;	//선택할때 1(DS)이나 2(GS) 로 기억함
	int Click;	//버튼을 눌렀을시, 1(DS)이나 2(GS)로 기억함

	//적 선택
	VERTEX2D	*SelectEnemy;
	LPDIRECT3DTEXTURE9	pTexSEnemyBG;

	LPDIRECT3DTEXTURE9	pTexDarkKnight;
	LPDIRECT3DTEXTURE9	pTexBlackDragon;

	int EnemySelect;	//고를때 1(DK)나 2(BD)로 기억함
	int EnemyClick;		//버튼을 눌렀을시 1(DK)나 2(BD)로 기억함

	//난이도 선택
	int DSelect;
	int DClick;

	//이지
	VERTEX2D	*Easy;
	LPDIRECT3DTEXTURE9	pTexEasy;
	LPDIRECT3DTEXTURE9	pTexREasy;

	//노말
	VERTEX2D	*Normal;
	LPDIRECT3DTEXTURE9	pTexNormal;
	LPDIRECT3DTEXTURE9	pTexRNormal;

	//하드
	VERTEX2D	*Hard;
	LPDIRECT3DTEXTURE9	pTexHard;
	LPDIRECT3DTEXTURE9	pTexRHard;

	//콤보
	VERTEX2D	*Combo;
	LPDIRECT3DTEXTURE9	pTexCombo;

	VERTEX2D	*ComboNum;
	LPDIRECT3DTEXTURE9	pTexNum1;
	LPDIRECT3DTEXTURE9	pTexNum2;
	LPDIRECT3DTEXTURE9	pTexNum3;
	LPDIRECT3DTEXTURE9	pTexNum4;
	LPDIRECT3DTEXTURE9	pTexNum5;
	LPDIRECT3DTEXTURE9	pTexNum6;
	LPDIRECT3DTEXTURE9	pTexNum7;
	LPDIRECT3DTEXTURE9	pTexNum8;
	LPDIRECT3DTEXTURE9	pTexNum9;
	LPDIRECT3DTEXTURE9	pTexNum10;
	LPDIRECT3DTEXTURE9	pTexNum11;
	LPDIRECT3DTEXTURE9	pTexNum12;
	LPDIRECT3DTEXTURE9	pTexNum13;
	LPDIRECT3DTEXTURE9	pTexNum14;
	LPDIRECT3DTEXTURE9	pTexNum15;
	LPDIRECT3DTEXTURE9	pTexNum16;
	LPDIRECT3DTEXTURE9	pTexNum17;
	LPDIRECT3DTEXTURE9	pTexNum18;
	LPDIRECT3DTEXTURE9	pTexNum19;
	LPDIRECT3DTEXTURE9	pTexNum20;

	//스킬이펙트
	VERTEX2D	*Crit;
	LPDIRECT3DTEXTURE9	pTexCrit;

	//게임끝
	VERTEX2D	*End;				
	LPDIRECT3DTEXTURE9	pTexDead;	//플레이어사망
	LPDIRECT3DTEXTURE9	pTexDefeat; //몹사망

public:

	CGame(void);
	virtual ~CGame(void);
	void			UIBAR( bool BeHit ); // hp 게이지
	unsigned int	Init(void);
	unsigned int	Update(void);
	void			Render(void);
	void			Free(void);
	void			SkyBox(void);
	void			SkyBoxRender(void);
	//void			fSelectWeapon(void);
	//void			fSelectEnemy(void);
	void			ComboRender(void);
	void			ComboInit(void);
	void			BuffInit(void);
	void			BuffRender(void);
};
