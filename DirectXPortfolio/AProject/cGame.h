#pragma once

class CGame : public CScene
{
private:
	D3DXVECTOR3			vNormal;	// ���� ��ġ�� ������ ������ ����
	float				fNomalAngle;// ���� ��ġ�� ������ Y�� �������� ������ ����
	CUiBase*			cUiBase;	// UI�����Ŭ����
	POINT				CMouse;		// ���콺 
	bool				m_bChoiceWp;// ���⼱�� ���� üũ
	bool				m_bSelectEnemy;	//������ ���� üũ
	bool				m_bSelectDifficulty;	//���̵����� ���� üũ
	float				PlayerHp;	// �÷��̾� ü�� ��ȭ��ġ 
	float				MonsterHp;	// ����   ü�� ��ȭ��ġ
	float				PlayerSt;	// �÷��̾� ���¹̳� ��ȭ��ġ
	float				Bar;		// ü�¹ٹ� ���¹̳� �� ������ ����
	BOOL				m_bIsStop;	// ������Ʈ���¸� ���ߴ��� �ȸ��ߴ��� üũ
	BOOL				m_bIsShowMS;// ���콺�� ���̰� �ִ��� �Ⱥ��̰� �ִ��� üũ


	//ü�¹�
	VERTEX2D	*PlayerHPBack;			//�÷��̾� HP��
	LPDIRECT3DTEXTURE9	pTexPlayerHPB;

	//���¹̳�
	VERTEX2D	*PlayerSTBack;	//�÷��̾� ST��
	VERTEX2D	*PlayerSTFront;
	LPDIRECT3DTEXTURE9	pTexPlayerSTF;	//�÷��̾� ST����Ʈ
	LPDIRECT3DTEXTURE9	pTexPlayerSTF2;

	VERTEX2D	*PlayerHPFront;
	LPDIRECT3DTEXTURE9	pTexPlayerHPF;	//�÷��̾� HP����Ʈ

	VERTEX2D	*BossHPBack;			//���� HP��
	LPDIRECT3DTEXTURE9	pTexBossHPB;

	VERTEX2D	*BossHPFront;
	LPDIRECT3DTEXTURE9	pTexBossHPF;	//���� HP����Ʈ

	//����
	VERTEX2D	*Skill1;
	VERTEX2D	*Buff1;
	VERTEX2D	*Buff2;
	VERTEX2D	*Buff3;
	VERTEX2D	*Buff4;

	//����
	LPDIRECT3DTEXTURE9	pTexVamp;
	LPDIRECT3DTEXTURE9	pTexStamina;
	LPDIRECT3DTEXTURE9	pTexSpeed;
	LPDIRECT3DTEXTURE9	pTexDamage;
	//����
	LPDIRECT3DTEXTURE9	pTexSVamp;
	LPDIRECT3DTEXTURE9	pTexSStamina;
	LPDIRECT3DTEXTURE9	pTexSSpeed;
	LPDIRECT3DTEXTURE9	pTexSDamage;

	VERTEX2D	*Arrow;	//ȭ��ǥ
	//ȭ��ǥ����
	LPDIRECT3DTEXTURE9	pTexArrow0;
	LPDIRECT3DTEXTURE9	pTexArrow1;
	LPDIRECT3DTEXTURE9	pTexArrow2;
	LPDIRECT3DTEXTURE9	pTexArrow3;
	LPDIRECT3DTEXTURE9	pTexArrow4;

	//����
	LPDIRECT3DTEXTURE9	pTexDVamp;
	LPDIRECT3DTEXTURE9	pTexDStamina;
	LPDIRECT3DTEXTURE9	pTexDSpeed;
	LPDIRECT3DTEXTURE9	pTexDDamage;

	//���� ����
	VERTEX2D	*ChoiceWeapon;   
	LPDIRECT3DTEXTURE9	pChoiceWeapon;	// ���⼱��

	LPDIRECT3DTEXTURE9	pDualSword;		//�ּ�
	LPDIRECT3DTEXTURE9	pGreatSword;	//���

	int Select;	//�����Ҷ� 1(DS)�̳� 2(GS) �� �����
	int Click;	//��ư�� ��������, 1(DS)�̳� 2(GS)�� �����

	//�� ����
	VERTEX2D	*SelectEnemy;
	LPDIRECT3DTEXTURE9	pTexSEnemyBG;

	LPDIRECT3DTEXTURE9	pTexDarkKnight;
	LPDIRECT3DTEXTURE9	pTexBlackDragon;

	int EnemySelect;	//���� 1(DK)�� 2(BD)�� �����
	int EnemyClick;		//��ư�� �������� 1(DK)�� 2(BD)�� �����

	//���̵� ����
	int DSelect;
	int DClick;

	//����
	VERTEX2D	*Easy;
	LPDIRECT3DTEXTURE9	pTexEasy;
	LPDIRECT3DTEXTURE9	pTexREasy;

	//�븻
	VERTEX2D	*Normal;
	LPDIRECT3DTEXTURE9	pTexNormal;
	LPDIRECT3DTEXTURE9	pTexRNormal;

	//�ϵ�
	VERTEX2D	*Hard;
	LPDIRECT3DTEXTURE9	pTexHard;
	LPDIRECT3DTEXTURE9	pTexRHard;

	//�޺�
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

	//��ų����Ʈ
	VERTEX2D	*Crit;
	LPDIRECT3DTEXTURE9	pTexCrit;

	//���ӳ�
	VERTEX2D	*End;				
	LPDIRECT3DTEXTURE9	pTexDead;	//�÷��̾���
	LPDIRECT3DTEXTURE9	pTexDefeat; //�����

public:

	CGame(void);
	virtual ~CGame(void);
	void			UIBAR( bool BeHit ); // hp ������
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
