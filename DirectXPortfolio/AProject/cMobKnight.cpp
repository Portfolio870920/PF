#include "MainHeader.h"

cMobKnight::cMobKnight(void)
{

}

cMobKnight::~cMobKnight(void)
{
}

void	cMobKnight::Init( D3DXVECTOR3 MobPos, D3DXVECTOR3 MobScl, float MobAngle )
{
	m_vRight	=	D3DXVECTOR3( -1.0f, 0.0f, 0.0f );
	m_vUp		=	D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	m_vLook		=	D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
	m_Velocity	=	0.5f;
	m_FirstCheck	=	false;
	m_BeHitCheck	=	false;

	m_ActionStartTime	=	0.0f;
	m_ActionChoiceCheck		=	false;

	m_ActionPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_ActionLen = 0.0f;

	m_vPos		=	MobPos;
	m_fangle	=	MobAngle;
	m_vScl		=	MobScl;

	if ( MobPos == D3DXVECTOR3(0.0f,0.0f,0.0f) && MobScl == D3DXVECTOR3(0.0f,0.0f,0.0f) && MobAngle == 0.0f )
	{
		m_vPos		=	D3DXVECTOR3( 100.0f, 0.0f, 100.0f );		
		m_fangle	=	2.7f;
		m_vScl		=	D3DXVECTOR3 ( 0.15f, 0.2f, 0.2f );
	}
	m_vRelease_Pos	=	m_vPos;

	yaw( m_fangle );

	SetStatusInfo();			//	�������ͽ� �ʱ�ȭ
	m_SkinRenderer.Init("./SHADER/Knight.BSM");					//	�޽�����	//	���
	CSkinRenderer::LoadEffect("./SHADER/skin_render.fx");		//	���̴� ����

	m_cMobKnightController.Init( &m_SkinRenderer );					//	�޽� �ʱ�ȭ

	m_cMobKnightController.SetScl( m_vScl );	//	���

}

void	cMobKnight::Update( void )
{
	CheckActionPos();
	AI_MainSystem();
	CollisionCEHCKATTACK();
	//	���� ���� ���� üũ
	MOBMGR->SaveIter();
	MOBMGR->SetIterBengin();
	while(MOBMGR->GetIter() != MOBMGR->IterEnd() )
	{
		m_vPos		=	SetMonsterCrash( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetMonsterPos() );
		MOBMGR->IterInc();
	}
	MOBMGR->LoadIter();
	//	end

	m_vPos.y	=	MAPMGR->GetHeight(m_vPos.x,m_vPos.z,NULL,&m_vPosNormal);
	// �̲����� ���� �κ�
	if ( D3DXVec3Dot( &m_vPosNormal, &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) ) <= 0.75f ) // y��� ����ġ �ﰢ�� ������ �����Ͽ� ���� ���� 
	{
		// �̲����� ���� ���
		m_vSlideVec	=	( -(D3DXVECTOR3(0.0f,1.0f,0.0f) - ( ( D3DXVec3Dot(&D3DXVECTOR3(0.0f,1.0f,0.0f),&m_vPosNormal) ) / ( D3DXVec3Dot(&m_vPosNormal ,&m_vPosNormal) ) ) * m_vPosNormal) );
	}
	else
	{
		m_vSlideVec	=	D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); // �������� 0.75 �̻��̸� �̲����� ����
	}
	m_vPos	+=	m_vSlideVec; // �̲����� ���� ����

	m_cMobKnightController.SetPos( m_vPos );
	m_cMobKnightController.SetRot(D3DXVECTOR3( 0.0f, m_fangle, 0.0f ) );
	
	HITCHECK();

	m_cMobKnightController.UpdateFrame();
}
void	cMobKnight::Render( void )
{
	//TCHAR	str_Text[256];	// ���ڿ� ����
	//ZeroMemory(str_Text,sizeof(str_Text)); //���ڿ� ���� �ʱ�ȭ
	//sprintf(str_Text,"���� ������ : %f %d \nn", m_cMobKnightController.GetCurFrame(), m_ActionCheck ); // �̷������� ���
	//TEXTMGR->Print(str_Text, 0, 120); // �ؽ�Ʈ���

	m_cMobKnightController.Render( m_BeHitCheck, NULL );
}

D3DXVECTOR3 cMobKnight::GetMonsterPos( void )
{
	D3DXVECTOR3 vPos;

	vPos	=	m_vPos;

	return vPos;
}

//	���� ���ͷ� ȸ��
void cMobKnight::yaw	( float angle )
{
	D3DXMATRIX	T;

	//	������ ��ü�� ���ؼ��� ���� y (0, 1, 0)�� �������� ȸ���Ѵ�.
	D3DXMatrixRotationY( &T, angle );

	//	
	D3DXVec3TransformCoord(&m_vRight, &m_vRight, &T);
	D3DXVec3TransformCoord(&m_vLook, &m_vLook, &T);
}

//	�������ͽ� ���� �ʱ�ȭ
void	cMobKnight::SetStatusInfo( void )
{
	m_Status.m_Status		=	STATUS_STANCE;		//	���� �������ͽ�
	m_Status.StartFrame		=	1;					//	ù��° ������
	m_Status.EndFrame		=	8;					//	������ ������
	m_Status.PlayTime		=	2.0f;				//	������ �Ѹ��� �ð� 2��
	m_Status.Smotion		=	0.1f;				//	�������	0.05��
	m_Status.ActionCansle	=	true;				//	�������ͽ� ĵ�� ����
	m_Status.m_Dead			=	false;				//	�׾����� üũ
	m_Status.m_HP			=	5000;				//	HP
	m_Status.m_MaxHp		=	5000;				//  Ǯ HP
	m_Status.m_Damage		=	100.0f + ( D3DXVec3Length( &m_vScl ) );		//	������
	m_Status.m_Stamina		=	1000;				//	Stamina

	m_CheckHP	=	m_Status.m_HP;					//	üũ HP �ʱ�ȭ
}


void	cMobKnight::DIE	( void )
{
	m_Status.m_Status		=	STATUS_DIE;
	m_Status.StartFrame		=	50;
	m_Status.EndFrame		=	99;
	m_Status.PlayTime		=	7.0f;
	m_Status.ActionCansle	=	false;
}

void	cMobKnight::STANCE		( void )
{
	m_Status.m_Status		=	STATUS_STANCE;
	m_Status.StartFrame		=	1;
	m_Status.EndFrame		=	8;
	m_Status.PlayTime		=	1.0f;	//	2
	m_Status.ActionCansle	=	true;
}

void	cMobKnight::MOVE( void )
{
	m_Status.m_Status		=	STATUS_MOVE;
	m_Status.StartFrame		=	10;
	m_Status.EndFrame		=	22;
	m_Status.PlayTime		=	2.0f;
	m_Status.ActionCansle	=	true;
}

void	cMobKnight::VERTICAL( void )
{
	m_Status.m_Status		=	STATUS_VERTICAL;
	m_Status.StartFrame		=	25;
	m_Status.EndFrame		=	44;
	m_Status.PlayTime		=	2.0f;
	m_Status.ActionCansle	=	false;
}

void	cMobKnight::VERTICAL_RIGHT( void )
{
	m_Status.m_Status		=	STATUS_HORIZON_RIGHT;
	m_Status.StartFrame		=	100;
	m_Status.EndFrame		=	120;
	m_Status.PlayTime		=	2.0f;
	m_Status.ActionCansle	=	false;
}

Play_Status	cMobKnight::GetPlay_Status(void)
{
	return m_Status;
}

//	AI Main
void	cMobKnight::AI_MainSystem	( void )
{
	if( SCENEMGR->GetNowScene() != MENU && m_Status.m_Status == STATUS_DIE )
	{
		//	�������ͽ��� DIE ������ ���
		if( m_Status.m_Status == STATUS_DIE )
		{
			m_cMobKnightController.SetRangeNoLoop(m_Status.StartFrame,
				m_Status.EndFrame,
				m_Status.StartFrame,
				m_Status.EndFrame,
				m_Status.PlayTime );
			return ;
		}
	}
	else if( m_Status.m_HP < 1)
	{
		DIE();
	}

	if( SCENEMGR->GetNowScene() != MENU && m_Status.m_Status != STATUS_DIE )
	{
		if( m_cMobKnightController.SetRangeNoLoop( m_Status.StartFrame, m_Status.EndFrame, 
			m_Status.StartFrame, m_Status.EndFrame, m_Status.PlayTime, m_Status. Smotion ) == UPDATE_PAUSE )
		{
			if( m_Status.ActionCansle == true )	//	��� ���°� ������ �� �� ��� ������ �����ϸ�
			{
				m_ActionSwith	=	rand()%2+1;
				m_ActionChoiceCheck = false;
			}
			STANCE();
		}

		switch( m_ActionSwith )
		{
		case 1:
			{
				if ( m_ActionChoiceCheck != true )
				{
					m_ActionStartTime	=	TIMEMGR->GetTotalTIme();
				}
				m_ActionChoiceCheck	=	true;
				//	AI_Look
				SetLook( CHARMGR->GetPos() );

				//	AI_MOVE
				AI_StatusMove( CHARMGR->GetPos() );
				break;
			}
		case 2:
			{
				if ( m_ActionChoiceCheck != true )
				{
					m_ActionStartTime	=	TIMEMGR->GetTotalTIme();
				}
				m_ActionChoiceCheck	=	true;

				SetLook( CHARMGR->GetPos() );
				//	AI Action
				AI_StatusAction();
				break;
			}
		}
		return ;
	}
	else if( m_Status.m_Status != STATUS_DIE )
	{
		if( m_cMobKnightController.SetRange( m_Status.StartFrame, m_Status.EndFrame, m_Status.PlayTime, m_Status.Smotion, false )
			==	UPDATE_RETURN )
		{
			STANCE();
		}
	}

}

//	AI MOVE
void	cMobKnight::AI_StatusMove	( D3DXVECTOR3 Pos )
{
	D3DXVec3Normalize( &m_vMonster_dir, &m_vMonster_dir );
	m_vMonster_dir *= m_Velocity;
	if( SetLength( Pos ) < 200.0f && SetLength( Pos ) > 23.0f && m_Status.ActionCansle == true )
	{
		MOVE();
		m_vPos.x += ( m_vMonster_dir.x );
		m_vPos.z += ( m_vMonster_dir.z );
	}
}

//	AI Action
void	cMobKnight::AI_StatusAction	( void )
 {
	//	�������ͽ��� DIE ������ ���
	if( m_Status.m_Status == STATUS_DIE )
	{
		m_cMobKnightController.SetRangeNoLoop(m_Status.StartFrame,
			m_Status.EndFrame,
			m_Status.StartFrame,
			m_Status.EndFrame,
			m_Status.PlayTime );
		return;
	}

	if( SCENEMGR->GetNowScene() != MENU &&  m_Status.ActionCansle == true 
		&& SetLength( CHARMGR->GetPos() ) < 45.0f 
		&& m_fDot > 0.85f						//	������ �Ÿ��� 25���̰� ������ �ִ� �����̰�
		)										//	���Ϳ� �÷��̾��� ������ ���̰� 0�� ������
	{
		int Monsterchoise = rand()%2;

		switch( Monsterchoise )
		{
		case 0:
			{
				m_ActionCheck	=	0;
				VERTICAL();
				break;
			}
		case 1:
			{
				if( m_CheckHP != m_Status.m_HP )
				{
					m_CheckHP	=	m_Status.m_HP;
					m_ActionCheck	=	0;
					VERTICAL_RIGHT();
				}
				break;
			}
		}//	switch end
	}//	if end
}

//	Player to ( 0.0f, m_vPos.y, 0.0f ) Length
float	cMobKnight::SetLength( D3DXVECTOR3 Player_Pos )
{
	float fLen	=	D3DXVec3Length( &(Player_Pos - m_vPos ) );	//	�÷��̾�� ������ �Ÿ�
	return fLen;
}

//	�þ� üũ
void cMobKnight::SetLook( D3DXVECTOR3 Player_Pos )
{
	//	�������ͽ��� DIE ������ ���
	if( m_Status.m_Status == STATUS_DIE )
	{
		m_cMobKnightController.SetRangeNoLoop(m_Status.StartFrame,
			m_Status.EndFrame,
			m_Status.StartFrame,
			m_Status.EndFrame,
			m_Status.PlayTime );
		return;
	}

	D3DXVECTOR3 vCross, m_vMonster_NewDir;

	m_vMonster_dir = ( Player_Pos - m_vPos );						//	���� �÷��̾ �ٶ󺸴� ����

	D3DXVec3Normalize( &m_vMonster_NewDir, &m_vMonster_dir );		//	�� ���� ����ȭ
	D3DXVec3Normalize( &m_vLook, &m_vLook);							//	���� ���� ����ȭ

	D3DXVec3Cross( &vCross, &m_vMonster_NewDir, &m_vLook );			//	���� �÷��̾� �ٶ󺸴� ����� ���� �������� ����
	m_fDot	=	D3DXVec3Dot( &m_vMonster_NewDir, &m_vLook );
	m_fLen	=	D3DXVec3Length( &vCross );

	//	�÷��̾�� ������ �Ÿ��� 210.0f ���̰� ���Ͱ� ���� ������ �� �ִ� ������ ���
	if( SetLength( Player_Pos ) < 210.0f && m_Status.ActionCansle == true )
	{
		if( m_fDot < 0.99f )
		{
			m_fangle -= vCross.y*0.2f;			//	������ ������ y ��ŭ ȸ�� ��Ŵ
			yaw( -vCross.y*0.2f );
		}
	}

}
void cMobKnight::CheckActionPos( void )
{
	D3DXVECTOR3 Now_vLook;

	D3DXVec3Normalize( &Now_vLook, &m_vLook );
	Now_vLook *= 25.0f;
	Now_vLook += m_vPos;

	if( !m_Status.ActionCansle )
	{
		if( m_Status.m_Status == STATUS_VERTICAL )
		{
			m_ActionLen	= 18.0f;
			m_ActionPos = Now_vLook;
		}
		else if( m_Status.m_Status == STATUS_HORIZON_RIGHT )
		{
			m_ActionLen = 28.0f;
			m_ActionPos = m_vPos;
		}
		else
		{
			m_ActionLen = 0.0f;
		}
	}
	else
	{
		m_ActionLen = 0.0f;
	}
}

//	���� üũ
bool	cMobKnight::CheckActionCrash( D3DXVECTOR3 Player_Pos )
{
	D3DXVECTOR3  MonstertoPlayer, Now_vLook;
	MonstertoPlayer	=	( Player_Pos - m_vPos );

	D3DXVec3Normalize( &Now_vLook, &m_vLook);		//	���� ���� ���� ����ȭ
	Now_vLook *= 25.0f;								//	���� ���� ���� ���ϱ� 25.0f Į ����
	Now_vLook += m_vPos;							//	���� ���� ���Ϳ� ��ġ ���� �� ��

	if( m_Status.m_Status == STATUS_VERTICAL )
	{
		float fLen = D3DXVec3Length( &( Player_Pos - Now_vLook ) );	//	vLook�� Į ���� ��ġ ���Ϳ� ���� ��ġ�� �� 
		m_ActionPos = Now_vLook;
		if( fLen < 18.0f)			//	���� ���� 17
		{
			return true;
		}
	}
	else
	{
		
		float fLen = D3DXVec3Length( &( Player_Pos - m_vPos ) );	//	������ ��ġ����
		m_ActionPos = m_vPos;
		if( fLen < 28.0f )			//	���� ���� 28
		{
			return true;
		}
	}
	return false;
}

//	������ üũ
bool	cMobKnight::CheckActionFrameCrash( D3DXVECTOR3 Player_Pos )
{
	D3DXVECTOR3  MonstertoPlayer, Now_vLook, MonstertoPlayer_dir, vCross;
	float	CrashFrame, NowFrame;

	MonstertoPlayer	=	( m_vPos - Player_Pos );					//	���Ϳ��� �Ÿ�����
	D3DXVec3Normalize( &Now_vLook, &m_vLook );						//	���� �÷��̾� ���� ����

	if( !m_ActionCheck )	//	���� �ѹ��� üũ�� �ȵǾ����� ����
	{
		if( m_Status.m_Status == STATUS_VERTICAL )
		{
			CrashFrame	=	36.0f;
			NowFrame	=	m_cMobKnightController.GetCurFrame();					//	���� ������

			if( ( CrashFrame - 2.0f ) < NowFrame && ( CrashFrame + 4.0f ) > NowFrame )
			{
 				SOUNDMGR->PlaySound( MOB_KNIGHT_ATTACK );
				m_ActionCheck		=	true;
				return true;
			}
		}//	�������ͽ� ���� üũ
		else if( m_Status.m_Status == STATUS_HORIZON_RIGHT )
		{
			CrashFrame	=	112.0f;												//	üũ �Ǿ��� ������
			NowFrame	=	m_cMobKnightController.GetCurFrame();					//	���� ������

			if( CrashFrame - 2.0f < NowFrame && CrashFrame + 2.5f > NowFrame )
			{
				m_ActionCheck		=	true;
				return true;
			}
		}
	}

	return false;
}

//	Player Crash check
D3DXVECTOR3 cMobKnight::SetMonsterCrash( D3DXVECTOR3 Monster_Pos )
{
	if( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetPlay_Status().m_Status == STATUS_DIE )	//	���Ͱ� ������ ������ �浹�� üũ���� ����
	{
		return m_vPos;
	}

	D3DXVECTOR3 new_vPos = m_vPos;						//	�÷��̾� ������ ��ġ���Ͱ��� ����
	D3DXVECTOR3 dir;									//	����ȭ �� ���� ����

	D3DXVec3Normalize( &dir, &(new_vPos - Monster_Pos) );		//	�÷��̾� ��ġ - ���� ��ġ�� ����ȭ �Ѱ��� dir����
	float fLen = D3DXVec3Length( &( new_vPos - Monster_Pos ) );	//	�÷��̾� ��ġ - ���� ��ġ ���� �� fLen ����

	if(m_Status.ActionCansle == true && fLen < 15.0f )									//	flen�� 15.0f���� ���ٸ�
	{
		float dist = 15.0f - fLen;						//	15.0f - flen �� dist�� ����
		new_vPos += dir * dist;							//	dir * dist�Ͽ� new_vPos ���� ���� ���Ͽ� ����
		return new_vPos;
	}
	return m_vPos;
}

void	cMobKnight::HITCHECK()
{
	if( m_BeHitCheck )
	{
		int i = (int)m_Status.m_HP%4;
		switch ( i )
		{
		case 0:
			SOUNDMGR->PlaySound( MOB_KNIGHT_BEHIT1 );
			break;
		case 1:
			SOUNDMGR->PlaySound( MOB_KNIGHT_BEHIT2 );
			break;
		case 2:
			SOUNDMGR->PlaySound( MOB_KNIGHT_BEHIT3 );
			break;
		case 3:
			SOUNDMGR->PlaySound( MOB_KNIGHT_BEHIT4 );
			break;
		}
	}
	m_HitEndTime	=	TIMEMGR->GetTotalTIme();
	if( m_HitEndTime - m_HitTime > 0.1f )
	{
		m_BeHitCheck = false;
	}
}


//	���� üũ
void	cMobKnight::CollisionCEHCKATTACK( void )
{
	if( m_Status.ActionCansle == false )
	{
		if( SCENEMGR->GetNowScene() != MENU && CheckActionFrameCrash( CHARMGR->GetPos() ) )
		{
			if( CheckActionCrash( CHARMGR->GetPos() ) )
			{
				CHARMGR->SetDamageCheck( m_Status.m_Damage );	//	������ ������ ��
				CHARMGR->CheckBEHIT();							//	������� �ƾ���
			}
		}
	}
}