#pragma once
// ���� ���� Ŭ������ �θ� ������ ��
//#include "MainHeader.h"

class CMonster
{

protected:

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

	//	Check & Time
	bool			m_ActionCheck;		//	�������� üũ
	int				m_CheckHP;			//	HP üũ
	bool			m_FirstCheck;		//	ó�� ������ ���������
	bool			m_BeHitCheck;		//	�ǰݽ� true
	float			m_HitTime;			//	ó�� ���� �ð�
	float			m_HitEndTime;		//	�ǰݽ� Ÿ��
	D3DXVECTOR3		m_ActionPos;		//	���� ���� ��ġ
	float			m_ActionLen;		//	���� ���� ����

	int				m_ActionSwith;		//	������ �׼� ���� ����
	float			m_ActionStartTime;	//	������ �׼� ���� Ÿ��
	//float			m_ActionEndTime;	//	������ �׼� ���¸� Ȯ���� Ÿ��
	bool			m_ActionChoiceCheck;//	�׼� ���� ���̽��� �Ǹ� true

	
public:

	CMonster(void);
	virtual ~CMonster(void);

	virtual	void	Init	( D3DXVECTOR3 MobPos = D3DXVECTOR3(0.0f,0.0f,0.0f), D3DXVECTOR3 MobScl = D3DXVECTOR3(0.0f,0.0f,0.0f), float MobAngle = 0.0f )	=	0;								//	�ʱ�ȭ
	virtual	void	Update	( void )	=	0;								//	���� ������Ʈ
	virtual	void	Render	( void )	=	0;								//	�׸���
	virtual	void	SetStatusInfo	( void )	=	0;						//	�������ͽ� ���� �ʱ�ȭ

	virtual	void	yaw		( float angle )	=	0;						//	y������ ȸ��
	

	virtual	void	AI_MainSystem	( void )	=	0;						//	AI
	virtual	void	AI_StatusMove	( D3DXVECTOR3 Pos )	=	0;						//	AI MOVE
	virtual	void	AI_StatusAction	( void )	=	0;						//	AI Action

	virtual	bool	CheckActionCrash( D3DXVECTOR3 Player_Pos )	=	0;			//	���� ���� üũ
	virtual	bool	CheckActionFrameCrash( D3DXVECTOR3 Player_Pos )	=	0;	//	���� ������ üũ

	virtual	float	SetLength(D3DXVECTOR3 Player_Pos)	=	0;				//	Player to Monster Length
	virtual	void	SetLook( D3DXVECTOR3 Player_Pos )	=	0;				//	Monster to Cross
	virtual inline	void	SetBeHitTime( float time )	=	0;

	virtual	D3DXVECTOR3 GetMonsterPos( void )	=	0;						//	���� ��ġ�� ������
	virtual	Play_Status	GetPlay_Status( void )	=	0;						//	���� �������ͽ� ������
	virtual	inline	void	SetDie_Staus( bool	bDie )	=	0;					//	�׾����� ��Ҵ��� ������ �־���
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