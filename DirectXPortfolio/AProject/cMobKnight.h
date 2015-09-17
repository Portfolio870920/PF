#pragma once
//��� Ŭ����
//#include "MainHeader.h"

class cMobKnight : public CMonster
{


public:
	CSkinController		m_cMobKnightController;
	CSkinRenderer		m_SkinRenderer;
	cCollision			m_Collision;
	Play_Status			m_Status;
	CParticleSprite		m_ParticleSprite;

	D3DXVECTOR3		m_vPos;			//	��ġ ����
	D3DXVECTOR3		m_vRight;
	D3DXVECTOR3		m_vUp;
	D3DXVECTOR3		m_vLook;
	D3DXVECTOR3		m_vRot;
	D3DXVECTOR3		m_vScl;//������

	// �̲����� ���� �κ� //
	D3DXVECTOR3		m_vSlideVec;		//	�̲����� ����
	D3DXVECTOR3		m_vPosNormal;		//	�� ��ġ ��������

	D3DXVECTOR3		m_vMonster_dir;		//	���Ͱ� ������ ���� ����
	D3DXVECTOR3		m_vRelease_Pos;		

	float			m_fangle;			//	����
	float			m_Velocity;			//	�ӵ�
	float			m_fLen;				//	�÷��̾ ����Ű�� ���Ϳ� ������ ���Ͱ� ������ ��� 0�� ������ ����
	float			m_fDot;				//	�÷��̾ ����Ű�� ���Ϳ� ������ ���Ͱ� ��ġ�� ��� 1�� ������ ����
	bool			m_ActionCheck;		//	�������� üũ
	float			m_CheckHP;			//	HP üũ
	bool			m_FirstCheck;		//	ó�� ������ ���������
	bool			m_BeHitCheck;		//	�ǰݽ� true
	float			m_HitTime;			//	ó�� ���� �ð�
	float			m_HitEndTime;		//	�ǰݽ� Ÿ��

	D3DXVECTOR3		m_ActionPos;		//	���� ���� ��ġ
	float			m_ActionLen;		//	���� ���� ����
	int				m_ActionSwith;		//	������ �׼� ���� ����
	float			m_ActionStartTime;	//	������ �׼� ���� Ÿ��
	bool			m_ActionEndTime;	//	������ �׼� ���¸� Ȯ���� Ÿ��

	cMobKnight(void);
	virtual ~cMobKnight(void);
	void	Init	( D3DXVECTOR3 MobPos = D3DXVECTOR3(0.0f,0.0f,0.0f), D3DXVECTOR3 MobScl = D3DXVECTOR3(0.0f,0.0f,0.0f), float MobAngle = 0.0f );								//	�ʱ�ȭ
	void	Update	( void );								//	���� ������Ʈ
	void	Render	( void );								//	�׸���
	void	SetStatusInfo	( void );						//	�������ͽ� ���� �ʱ�ȭ

	void	yaw		( float angle );						//	y������ ȸ��

	void	DIE				( void );
	void	STANCE			( void );
	void	MOVE			( void );
	void	VERTICAL		( void );							//	�⺻����1
	void	VERTICAL_RIGHT	( void );							//	�⺻����2

	void	AI_MainSystem	( void );						//	AI
	void	AI_StatusMove	( D3DXVECTOR3 Pos );			//	AI MOVE
	void	AI_StatusAction	( void );						//	AI Action

	void	HITCHECK();										//	�ǰݽ� ī��Ʈ�� ����

	D3DXVECTOR3	SetMonsterCrash	( D3DXVECTOR3 Monster_Pos );	//	�浹 üũ
	void	CollisionCEHCKATTACK( void );						//	���� üũ ���ռ��� ��Ʈ
	bool	CheckActionCrash( D3DXVECTOR3 Player_Pos );			//	���� ���� üũ
	bool	CheckActionFrameCrash( D3DXVECTOR3 Player_Pos );	//	���� ������ üũ
	void	CheckActionPos( void );								//	���� ��ġ ���� üũ

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

	D3DXVECTOR3 GetMonsterPos( void );						//	���� ��ġ�� ������
	Play_Status	GetPlay_Status( void );						//	���� �������ͽ� ������
	inline	bool	GetBeHit( void )						//	�̳༮�� ������ ������ true�� ����
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