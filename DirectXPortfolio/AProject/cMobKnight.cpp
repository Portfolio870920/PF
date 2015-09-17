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

	SetStatusInfo();			//	스테이터스 초기화
	m_SkinRenderer.Init("./SHADER/Knight.BSM");					//	메쉬파일	//	기사
	CSkinRenderer::LoadEffect("./SHADER/skin_render.fx");		//	셰이더 파일

	m_cMobKnightController.Init( &m_SkinRenderer );					//	메쉬 초기화

	m_cMobKnightController.SetScl( m_vScl );	//	기사

}

void	cMobKnight::Update( void )
{
	CheckActionPos();
	AI_MainSystem();
	CollisionCEHCKATTACK();
	//	몬스터 끼리 판정 체크
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
	// 미끄러짐 벡터 부분
	if ( D3DXVec3Dot( &m_vPosNormal, &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) ) <= 0.75f ) // y축과 내위치 삼각형 법선을 내적하여 각을 구함 
	{
		// 미끄러짐 벡터 계산
		m_vSlideVec	=	( -(D3DXVECTOR3(0.0f,1.0f,0.0f) - ( ( D3DXVec3Dot(&D3DXVECTOR3(0.0f,1.0f,0.0f),&m_vPosNormal) ) / ( D3DXVec3Dot(&m_vPosNormal ,&m_vPosNormal) ) ) * m_vPosNormal) );
	}
	else
	{
		m_vSlideVec	=	D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); // 내적값이 0.75 이상이면 미끄러짐 없음
	}
	m_vPos	+=	m_vSlideVec; // 미끄러짐 벡터 적용

	m_cMobKnightController.SetPos( m_vPos );
	m_cMobKnightController.SetRot(D3DXVECTOR3( 0.0f, m_fangle, 0.0f ) );
	
	HITCHECK();

	m_cMobKnightController.UpdateFrame();
}
void	cMobKnight::Render( void )
{
	//TCHAR	str_Text[256];	// 문자열 변수
	//ZeroMemory(str_Text,sizeof(str_Text)); //문자열 변수 초기화
	//sprintf(str_Text,"몬스터 프레임 : %f %d \nn", m_cMobKnightController.GetCurFrame(), m_ActionCheck ); // 이런식으로 사용
	//TEXTMGR->Print(str_Text, 0, 120); // 텍스트출력

	m_cMobKnightController.Render( m_BeHitCheck, NULL );
}

D3DXVECTOR3 cMobKnight::GetMonsterPos( void )
{
	D3DXVECTOR3 vPos;

	vPos	=	m_vPos;

	return vPos;
}

//	상향 벡터로 회전
void cMobKnight::yaw	( float angle )
{
	D3DXMATRIX	T;

	//	지상의 물체에 대해서는 월드 y (0, 1, 0)을 기준으로 회전한다.
	D3DXMatrixRotationY( &T, angle );

	//	
	D3DXVec3TransformCoord(&m_vRight, &m_vRight, &T);
	D3DXVec3TransformCoord(&m_vLook, &m_vLook, &T);
}

//	스테이터스 정보 초기화
void	cMobKnight::SetStatusInfo( void )
{
	m_Status.m_Status		=	STATUS_STANCE;		//	현재 스테이터스
	m_Status.StartFrame		=	1;					//	첫번째 프레임
	m_Status.EndFrame		=	8;					//	마지막 프레임
	m_Status.PlayTime		=	2.0f;				//	프레임 뿌리는 시간 2초
	m_Status.Smotion		=	0.1f;				//	보간모션	0.05초
	m_Status.ActionCansle	=	true;				//	스테이터스 캔슬 여부
	m_Status.m_Dead			=	false;				//	죽었는지 체크
	m_Status.m_HP			=	5000;				//	HP
	m_Status.m_MaxHp		=	5000;				//  풀 HP
	m_Status.m_Damage		=	100.0f + ( D3DXVec3Length( &m_vScl ) );		//	데미지
	m_Status.m_Stamina		=	1000;				//	Stamina

	m_CheckHP	=	m_Status.m_HP;					//	체크 HP 초기화
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
		//	스테이터스가 DIE 상태일 경우
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
			if( m_Status.ActionCansle == true )	//	모션 상태가 진행이 된 후 모션 변경이 가능하면
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
	//	스테이터스가 DIE 상태일 경우
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
		&& m_fDot > 0.85f						//	몬스터의 거리가 25안이고 때릴수 있는 상태이고
		)										//	몬스터와 플레이어의 각도의 길이가 0에 가까우면
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
	float fLen	=	D3DXVec3Length( &(Player_Pos - m_vPos ) );	//	플레이어와 몬스터의 거리
	return fLen;
}

//	시야 체크
void cMobKnight::SetLook( D3DXVECTOR3 Player_Pos )
{
	//	스테이터스가 DIE 상태일 경우
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

	m_vMonster_dir = ( Player_Pos - m_vPos );						//	적이 플레이어를 바라보는 방향

	D3DXVec3Normalize( &m_vMonster_NewDir, &m_vMonster_dir );		//	그 방향 정규화
	D3DXVec3Normalize( &m_vLook, &m_vLook);							//	현재 방향 정규화

	D3DXVec3Cross( &vCross, &m_vMonster_NewDir, &m_vLook );			//	적이 플레이어 바라보는 방향과 원래 방향으로 외적
	m_fDot	=	D3DXVec3Dot( &m_vMonster_NewDir, &m_vLook );
	m_fLen	=	D3DXVec3Length( &vCross );

	//	플레이어와 몬스터의 거리가 210.0f 안이고 몬스터가 현재 움직일 수 있는 상태일 경우
	if( SetLength( Player_Pos ) < 210.0f && m_Status.ActionCansle == true )
	{
		if( m_fDot < 0.99f )
		{
			m_fangle -= vCross.y*0.2f;			//	외적한 벡터의 y 마큼 회전 시킴
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

//	판정 체크
bool	cMobKnight::CheckActionCrash( D3DXVECTOR3 Player_Pos )
{
	D3DXVECTOR3  MonstertoPlayer, Now_vLook;
	MonstertoPlayer	=	( Player_Pos - m_vPos );

	D3DXVec3Normalize( &Now_vLook, &m_vLook);		//	전방 방향 벡터 정규화
	Now_vLook *= 25.0f;								//	전방 방향 벡터 곱하기 25.0f 칼 범위
	Now_vLook += m_vPos;							//	전방 방향 벡터에 위치 벡터 더 함

	if( m_Status.m_Status == STATUS_VERTICAL )
	{
		float fLen = D3DXVec3Length( &( Player_Pos - Now_vLook ) );	//	vLook은 칼 범위 위치 벡터에 몬스터 위치뺀 값 
		m_ActionPos = Now_vLook;
		if( fLen < 18.0f)			//	공격 범위 17
		{
			return true;
		}
	}
	else
	{
		
		float fLen = D3DXVec3Length( &( Player_Pos - m_vPos ) );	//	본인의 위치에서
		m_ActionPos = m_vPos;
		if( fLen < 28.0f )			//	공격 범위 28
		{
			return true;
		}
	}
	return false;
}

//	프레임 체크
bool	cMobKnight::CheckActionFrameCrash( D3DXVECTOR3 Player_Pos )
{
	D3DXVECTOR3  MonstertoPlayer, Now_vLook, MonstertoPlayer_dir, vCross;
	float	CrashFrame, NowFrame;

	MonstertoPlayer	=	( m_vPos - Player_Pos );					//	몬스터와의 거리벡터
	D3DXVec3Normalize( &Now_vLook, &m_vLook );						//	현재 플레이어 전방 벡터

	if( !m_ActionCheck )	//	공격 한번당 체크가 안되었으면 실행
	{
		if( m_Status.m_Status == STATUS_VERTICAL )
		{
			CrashFrame	=	36.0f;
			NowFrame	=	m_cMobKnightController.GetCurFrame();					//	현재 프레임

			if( ( CrashFrame - 2.0f ) < NowFrame && ( CrashFrame + 4.0f ) > NowFrame )
			{
 				SOUNDMGR->PlaySound( MOB_KNIGHT_ATTACK );
				m_ActionCheck		=	true;
				return true;
			}
		}//	스테이터스 상태 체크
		else if( m_Status.m_Status == STATUS_HORIZON_RIGHT )
		{
			CrashFrame	=	112.0f;												//	체크 되야할 프레임
			NowFrame	=	m_cMobKnightController.GetCurFrame();					//	현재 프레임

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
	if( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetPlay_Status().m_Status == STATUS_DIE )	//	몬스터가 죽으면 몬스터의 충돌을 체크하지 않음
	{
		return m_vPos;
	}

	D3DXVECTOR3 new_vPos = m_vPos;						//	플레이어 현재의 위치벡터값을 저장
	D3DXVECTOR3 dir;									//	정규화 할 변수 선언

	D3DXVec3Normalize( &dir, &(new_vPos - Monster_Pos) );		//	플레이어 위치 - 몬스터 위치를 정규화 한것을 dir저장
	float fLen = D3DXVec3Length( &( new_vPos - Monster_Pos ) );	//	플레이어 위치 - 몬스터 위치 나온 값 fLen 저장

	if(m_Status.ActionCansle == true && fLen < 15.0f )									//	flen이 15.0f보다 적다면
	{
		float dist = 15.0f - fLen;						//	15.0f - flen 를 dist에 저장
		new_vPos += dir * dist;							//	dir * dist하여 new_vPos 원래 값에 더하여 리턴
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


//	판정 체크
void	cMobKnight::CollisionCEHCKATTACK( void )
{
	if( m_Status.ActionCansle == false )
	{
		if( SCENEMGR->GetNowScene() != MENU && CheckActionFrameCrash( CHARMGR->GetPos() ) )
		{
			if( CheckActionCrash( CHARMGR->GetPos() ) )
			{
				CHARMGR->SetDamageCheck( m_Status.m_Damage );	//	때리면 데미지 들어감
				CHARMGR->CheckBEHIT();							//	때리면면 아야함
			}
		}
	}
}