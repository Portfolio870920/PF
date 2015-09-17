#include "MainHeader.h"

Cplayer*	Cplayer::Player_Inst	=	NULL;

Cplayer::Cplayer(void)
{

}

Cplayer::~Cplayer(void)
{
	m_cSkinRenderer.Free();
	m_cPlayerController.Free();
}

void	Cplayer::Init( int Weapon )
{
	m_PlayerType	=	LANDOBJECT;					//	땅에서 
	m_Velocity		=	0.4f;						//	속도
	m_Time			=	0.0f;						//	첫 콤보 시작시간
	m_NewTime		=	0.0f;						//	현재 시간
	m_ActionLen		=	0.0f;						//	공격 거리
	m_Weapon		=	Weapon;						//	무기 값 얻음
	m_ComboBuffer[0]	=	NULL;					//	콤보 버퍼 초기화
	m_ComboBuffer[1]	=	NULL;
	m_BuffSwith		=	0;							//	버프 상태
	m_BuffTime		=	0.0f;						//	버프 시작한 시간
	m_BuffTimeCheck	=	true;						//	버프시간이 10초 지나면 true
	m_HitCheck		=	false;						//	공격을 맞췃는가 체크
	m_ActionCheck	=	false;						//	공격을 했는가 체크

	m_DamegeUp	=	false;							//	데미지 업 버프가 실행되면 true
	m_SpeedUp	=	false;							//	스피드 업 버프가 실행되면 true
	m_StaminaUp	=	false;							//	스테미너 버프가 실행되면 true


	SetStatusInfo();								//	스테이터스 초기화

	m_CheckHP	=	m_Status1.m_HP;
	m_vPos		=	D3DXVECTOR3( 0.0f, 114.0f, 0.0f );
	m_vRight	=	D3DXVECTOR3( -1.0f, 0.0f, 0.0f );
	m_vUp		=	D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	m_vLook		=	D3DXVECTOR3( 0.0f, 0.0f, -1.0f );

	m_fcamangle	=	0.0f;
	m_fangle	=	-2.86f;
	yaw( -2.86f );

	m_cSkinRenderer.Init("./SHADER/SD.hero1.BSM");						//	메쉬파일
	CSkinRenderer::LoadEffect("./SHADER/skin_render.fx");			//	셰이더 파일
	m_cPlayerController.Init( &m_cSkinRenderer );						//	메쉬 인힛
	m_cPlayerController.SetPos( m_vPos );
	m_cPlayerController.SetScl( D3DXVECTOR3 ( 0.1f, 0.1f, 0.1f ) );	
}

void	Cplayer::Update( void )
{	
	//( m_Status1.m_HP<100 )? m_Status1.m_HP+=100 : m_Status1.m_HP; // 피 계속 참 디버그용
	GetCursorPos(&m_pos_MouseCursor);
	StatusMainSysstem( );

	MOBMGR->SetIterBengin();
	while( MOBMGR->GetIter() != MOBMGR->IterEnd() )
	{
		m_vPos		=	SetPlayerCrash( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetMonsterPos() );
		MOBMGR->IterInc();
	}
	m_vPos.y	=	MAPMGR->GetHeight(m_vPos.x,m_vPos.z,NULL,&m_vPosNormal);
	// 미끄러짐 벡터 부분
	if ( D3DXVec3Dot( &m_vPosNormal, &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) ) <= 0.85f ) // y축과 내위치 삼각형 법선을 내적하여 각을 구함 
	{
		// 미끄러짐 벡터 계산
		m_vSlideVec	=	( -(D3DXVECTOR3(0.0f,1.0f,0.0f) - ( ( D3DXVec3Dot(&D3DXVECTOR3(0.0f,1.0f,0.0f),&m_vPosNormal) ) / ( D3DXVec3Dot(&m_vPosNormal ,&m_vPosNormal) ) ) * m_vPosNormal) );
	}
	else
	{
		m_vSlideVec	=	D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); // 내적값이 0.85 이상이면 미끄러짐 없음
	}
	m_vPos	+=	m_vSlideVec; // 미끄러짐 벡터 적용

	m_cPlayerController.SetPos( m_vPos );
	m_cPlayerController.SetRot( D3DXVECTOR3(0.0f, m_fangle, 0.0f ) );

	
	CAMMGR->SetPlayerinfo(m_vPos, m_vRight, m_vUp, m_vLook);

	DashEffectUpdate();

	m_cPlayerController.UpdateFrame();
}

void	Cplayer::Render( void )
{
	//TCHAR	str_Text [256];	// 문자열 변수
	//ZeroMemory(str_Text,sizeof(str_Text)); //문자열 변수 초기화
	////sprintf(변수명,이후로는 printf처럼 사용);
	//sprintf(str_Text,"플레이어 높이 : %f\n", m_vPos.y ); // 이런식으로 사용
	//TEXTMGR->Print(str_Text, NULL, 140); // 텍스트출력
	//sprintf(str_Text,"플레이어 프레임 : %f\n", CAMMGR->m_vPos.y ); // 이런식으로 사용
	//TEXTMGR->Print(str_Text, NULL, 160); // 텍스트출력
	//sprintf(str_Text,"카메라 각 : %f\n", CAMMGR->m_vTarget_Y.y ); // 이런식으로 사용
	//TEXTMGR->Print(str_Text, NULL, 200); // 텍스트출력

	DashEffectRenDer();

	if(m_Weapon == 1)
	{
		m_cPlayerController.HideObject( 1, true );
		m_cPlayerController.Render( false, m_BuffSwith );
	}
	else
	{
		m_cPlayerController.HideObject( 2, true );
		m_cPlayerController.Render( false, m_BuffSwith );
	}

}

void	Cplayer::StatusMainSysstem( void )
{
	if( m_vPos.y < 50.0f)	//	케릭터가 맵 밖으로 나갔을 경우.
	{
		m_Status1.m_HP = 0;
	}

	if( m_Status1.m_HP < 1)
	{
		DIE();
	}

	//	스테이터스가 DIE 상태일 경우
	if( m_Status1.m_Status == STATUS_DIE )
	{
		m_cPlayerController.SetRangeNoLoop(m_Status1.StartFrame,
			m_Status1.EndFrame,
			m_Status1.StartFrame,
			m_Status1.EndFrame,
			m_Status1.PlayTime );
		return;
	}

	//	콤보를 가져옴
	SetCombo();
	//	이동
	SetStatusMove();
	//	모션
	SetActionStatus();
	//	버프
	SetBuff();
}


//	스테이터 이동
void	Cplayer::SetStatusMove( void )
{

	//	스테이터스가 DIE 상태일 경우
	if( m_Status1.m_Status == STATUS_DIE )
	{
		m_cPlayerController.SetRangeNoLoop(m_Status1.StartFrame,
			m_Status1.EndFrame,
			m_Status1.StartFrame,
			m_Status1.EndFrame,
			m_Status1.PlayTime );
		return ;
	}

	//	스테이터스가 다이가 아니거나 캔슬이 펄스가 아닐경우
	if( m_Status1.ActionCansle != false )
	{
		// 뒤로
		if(  KEYMGR->StayKeyDown('S') && KEYMGR->StayKeyDown(VK_SHIFT) && m_Status1.m_Stamina > 0.0f )
		{
			RUN();
			Walk( ( -m_Velocity/1.5f ) * 2.0f );
		}
		else if( KEYMGR->StayKeyDown('S') )
		{
			MOVE();
			Walk( ( -m_Velocity/1.5f ) );
		}


		// 빠르게 앞으로
		if( KEYMGR->StayKeyDown('W') && KEYMGR->StayKeyDown(VK_SHIFT) && m_Status1.m_Stamina > 0.0f)
		{			
			RUN();
			Walk(m_Velocity * 2.0f);
		}
		// 앞으로
		else if( KEYMGR->StayKeyDown('W') )
		{
			MOVE();
			Walk( m_Velocity );
		}
		// 카메라 마우스 회전


		// 카메라 좌측으로
		if( !KEYMGR->StayKeyDown('Q') && m_pos_MouseCursor.x < CENTERPOS_X - MOUSE_SENSE )
		{
			m_fangle -= 0.06f;
			yaw( -0.06f );
			CAMMGR->Yaw(-0.06f);
		}
		// 카메라 우측으로
		else if ( !KEYMGR->StayKeyDown('Q') && m_pos_MouseCursor.x > CENTERPOS_X + MOUSE_SENSE )
		{
			m_fangle += 0.06f;
			yaw( 0.06f );
			CAMMGR->Yaw(0.06f);
		}

		// 카메라 위로
		if ( !KEYMGR->StayKeyDown('Q') && m_pos_MouseCursor.y < CENTERPOS_Y - MOUSE_SENSE )
		{
			if ( m_fcamangle > -0.16f )
			{
				m_fcamangle -= 0.02f;
				CAMMGR->Pitch( -0.02f );
			}
		}
		//카메라 아래로
		else if( !KEYMGR->StayKeyDown('Q') && m_pos_MouseCursor.y > CENTERPOS_Y + MOUSE_SENSE )
		{
			if ( m_fcamangle < 0.5f )
			{
				m_fcamangle += 0.02f;
				CAMMGR->Pitch( 0.02f );
			}
		}


		// 빠르게 좌측으로
		if( KEYMGR->StayKeyDown('A') && KEYMGR->StayKeyDown(VK_SHIFT) && m_Status1.m_Stamina > 0.0f)
		{	
			RUN();
			strafe(( -m_Velocity/1.5f ) * 2.0f );
		}
		//	좌측으로
		else if( KEYMGR->StayKeyDown('A') )
		{
			MOVE();
			strafe( -m_Velocity /1.5f );

		}


		// 빠르게 우측으로
		if( KEYMGR->StayKeyDown('D') && KEYMGR->StayKeyDown(VK_SHIFT) && m_Status1.m_Stamina > 0.0f)
		{			
			RUN();
			strafe(( m_Velocity/1.5f ) * 2.0f );
		}
		//	우측으로
		else if( KEYMGR->StayKeyDown('D') )
		{
			MOVE();
			strafe( m_Velocity/1.5f );

		}

	}//	m_Status1.ActionCansle != false end
	if( m_cPlayerController.SetRange(m_Status1.StartFrame, m_Status1.EndFrame, m_Status1.PlayTime, m_Status1.Smotion )
		==	UPDATE_RETURN )
	{
		STANCE();
	}
}


//	Status 인포 초기화
void Cplayer::SetStatusInfo( void )
{
	m_Status1.m_Status		=	STATUS_STANCE;		//	현재 스테이터스
	m_Status1.StartFrame	=	1;					//	첫번째 프레임
	m_Status1.EndFrame		=	11;					//	마지막 프레임
	m_Status1.PlayTime		=	1.0f;				//	프레임 뿌리는 시간 1초
	m_Status1.Smotion		=	0.1f;				//	보간모션	0.05초
	//	0.05가 너무 빨라서 0.1로 수정함
	m_Status1.ActionCansle	=	true;				//	스테이터스 캔슬 여부
	m_Status1.m_Dead		=	false;				//	죽었는지 체크
	if( m_Weapon == 1 )	//	쌍수
	{
		m_Status1.m_fAttackSpeed	=	0.7f;			//	공격 속도
		m_Weapon_Damage	= 50;							//	무기 데미지
		m_Weapon_Length	= 5.0f;							//	무기 리치
		m_SaveWeapon_Damege	=	m_Weapon_Damage;
		m_SaveSpeed	=	m_Status1.m_fAttackSpeed;
	}
	else				//	양손
	{
		m_Status1.m_fAttackSpeed	=	0.85f;			//	공격 속도
		m_Weapon_Damage = 75;							//	무기 데미지
		m_Weapon_Length = 7.0f;							//	무기 리치
		m_SaveWeapon_Damege	=	m_Weapon_Damage;
		m_SaveSpeed	=	m_Status1.m_fAttackSpeed;
	}

	m_Status1.m_HP			=	1000;			//	현재 HP
	m_Status1.m_MaxHp		=	1000;			//  풀 HP
	m_Status1.m_Damage		=	30;				//	데미지
	m_Status1.m_Stamina		=	100;			//	Stamina
	m_Status1.m_MaxSt		=	100;			//  풀 스태미너

	m_Status1.m_RealCombo	=	0;				//	현재 콤보

}


//	Set 액션 변경
void Cplayer::SetActionStatus( void )
{

	//	스테이터스가 DIE 상태일 경우
	if( m_Status1.m_Status == STATUS_DIE )
	{
		m_cPlayerController.SetRangeNoLoop(m_Status1.StartFrame,
			m_Status1.EndFrame,
			m_Status1.StartFrame,
			m_Status1.EndFrame,
			m_Status1.PlayTime );
		return;
	}

	switch( GetCombo() )
	{
		////////////	DASH	/////////////////
	case 11:
		{
			DASH();
			DashEffectRight( -20 );
			strafe( -20 );
			break;
		}
	case 12:
		{
			DASH();
			DashEffectRight( 20 );
			strafe( 20 );
			break;
		}
	case 13:
		{
			DASH();
			DashEffectLook( -20 );
			Walk( -20 );
			break;
		}
	case 14:
		{
			DASH();
			DashEffectLook( 20 );
			Walk( 20 );
			break;
		}
		////////////	SKILL	/////////////////
	case 'W':
		{
			if( m_Status1.m_CheckSkill == true )	//	스킬이 사용 된후 다음 타격 할경우 
			{
				m_Status1.m_RealCombo	=	0;
			}
			m_HitCheck	=	false;

			m_NewTime	=	TIMEMGR->GetTotalTIme();
			m_Time	=	m_NewTime;				//	콤보가 성공하면 시간 갱신
			DOUBLESTAB();
			break;
		}
	case 22:
		{
			if( m_Status1.m_CheckSkill == true )	//	스킬이 사용 된후 다음 타격 할경우 
			{
				m_Status1.m_RealCombo	=	0;
			}
			if( m_Status1.m_RealCombo != 0 )		//	콤보가 0이라면 안나감
			{
				m_HitCheck	=	false;
				m_NewTime	=	TIMEMGR->GetTotalTIme();
				m_Time	=	m_NewTime;				//	콤보가 성공하면 시간 갱신
				DOUBLECUT();
			}
			break;
		}
		////////////	DEFULT CUT	/////////////////
	case VK_LBUTTON:
		{
			if( m_Status1.m_CheckSkill == true )	//	스킬이 사용 된후 다음 타격 할경우 
			{
				m_Status1.m_RealCombo	=	0;
			}
			m_HitCheck	=	false;

			m_NewTime	=	TIMEMGR->GetTotalTIme();
			m_Time	=	m_NewTime;				//	콤보가 성공하면 시간 갱신
			VERTICAL();
			break;
		}
	case 'S':
		{
			if( m_Status1.m_CheckSkill == true )	//	스킬이 사용 된후 다음 타격 할경우 
			{
				m_Status1.m_RealCombo	=	0;
			}
			m_HitCheck	=	false;
			m_NewTime	=	TIMEMGR->GetTotalTIme();
			m_Time	=	m_NewTime;				//	콤보가 성공하면 시간 갱신
			JUMPCUT();
			break;
		}
	case 'A':
		{
			if( m_Status1.m_CheckSkill == true )	//	스킬이 사용 된후 다음 타격 할경우 
			{
				m_Status1.m_RealCombo	=	0;
			}
			m_HitCheck	=	false;
			m_NewTime	=	TIMEMGR->GetTotalTIme();
			m_Time	=	m_NewTime;				//	콤보가 성공하면 시간 갱신
			HORIZON_LEFT();
			break;
		}
	case 'D':
		{
			if( m_Status1.m_CheckSkill == true )	//	스킬이 사용 된후 다음 타격 할경우 
			{
				m_Status1.m_RealCombo	=	0;
			}
			m_HitCheck	=	false;
			m_NewTime	=	TIMEMGR->GetTotalTIme();
			m_Time	=	m_NewTime;				//	콤보가 성공하면 시간 갱신
			HORIZON_RIGHT();
			break;
		}
	}

	AtteckCheck();

	if( m_cPlayerController.SetRange(m_Status1.StartFrame, m_Status1.EndFrame, m_Status1.PlayTime, m_Status1.Smotion )
		==	UPDATE_RETURN )
	{
		STANCE();
	}
}

void	Cplayer::AtteckCheck( void )
{
	MOBMGR->SetIterBengin();
	while( MOBMGR->GetIter() != MOBMGR->IterEnd() )
	{
		//	대쉬 상태일때는 판정 체크를 하면 안됨
		if( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetPlay_Status().m_Status != STATUS_DIE 
			&& (MOBMGR->MobVec(MOBMGR->GetIter()))->GetPlay_Status().m_Status != STATUS_JUMP_DRAGON_BREATH )	//	몬스터 상태
		{
			if( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetBeHit() == false && m_Status1.m_Status != STATUS_DASH && m_Status1.m_Status != STATUS_BEHIT )	//	플레이어 상태
			{
				if( CheckActionFrameCrash( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetMonsterPos() ) )		//	프레임 체크
				{
					if( CheckPlayerActionCrash( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetMonsterPos() ) )		//	판정 체크
					{
						m_HitCheck		=	true;
						m_ActionCheck	=	true;						//	액션 상태를 체크하여 판정이 확정되면 True
						m_HitTime	= TIMEMGR->GetTotalTIme();			//	공격 성공 시간
						m_Status1.m_RealCombo++;						//	콤보 갯수 
						CheckSkill();									//	스킬로 공격한건지
						(MOBMGR->MobVec(MOBMGR->GetIter()))->SetDamageCheck( m_Status1.m_Damage, true );	//	몬스터에게 데미지 보냄
						(MOBMGR->MobVec(MOBMGR->GetIter()))->SetBeHitTime( TIMEMGR->GetTotalTIme() );
					}
				}
			}
		}
		MOBMGR->IterInc();
	}
}


//	왼쪽/오른쪽
void Cplayer::strafe	( float Velocity )
{
	//	지상의 물체는 xz 평면으로 움직임을 제한한다.
	if( m_PlayerType == LANDOBJECT )
	{
		m_vPos	+=	D3DXVECTOR3( m_vRight.x, 0.0f, m_vRight.z ) * Velocity;
	}

	if( m_PlayerType == ATRCRAFT )
	{
		m_vPos += m_vRight * Velocity;
	}
}


//	위쪽/아래쪽
void Cplayer::fly	( float Velocity )
{

}


//	앞쪽/뒤쪽
void Cplayer::Walk	( float Velocity )
{
	if( Velocity == 0)	{	return;	}

	//	지상의 물체는 xz 평면으로 움직임을 제한한다.
	if( m_PlayerType == LANDOBJECT )
	{
		D3DXVECTOR3	dir;
		D3DXVec3Cross(&dir, &m_vRight, &m_vUp);
		m_vPos	+=	D3DXVECTOR3( dir.x, 0.0f, dir.z) * Velocity;
	}
	if( m_PlayerType == ATRCRAFT )
	{
		m_vPos	+=	m_vLook * Velocity;
	}
}


//	우향 벡터로 회전
void Cplayer::pitch	( float angle )
{
	D3DXMATRIX	T;
	D3DXMatrixRotationAxis(&T, &m_vRight, angle );

	//	m_vRight를 기준으로 m_vUp과 m_vLook을 회전한다.
	D3DXVec3TransformCoord(&m_vUp, &m_vUp, &T );
	D3DXVec3TransformCoord(&m_vLook, &m_vLook, &T );
}


//	상향 벡터로 회전
void Cplayer::yaw	( float angle )
{
	D3DXMATRIX	T;

	//	지상의 물체에 대해서는 월드 y (0, 1, 0)을 기준으로 회전한다.
	if( m_PlayerType == LANDOBJECT )
	{
		D3DXMatrixRotationY( &T, angle );
	}

	if( m_PlayerType == ATRCRAFT )
	{
		D3DXMatrixRotationAxis(&T, &m_vUp, angle );
	}

	//	
	D3DXVec3TransformCoord(&m_vRight, &m_vRight, &T);
	D3DXVec3TransformCoord(&m_vLook, &m_vLook, &T);
}


//	전방 벡터로 회전
void Cplayer::roll	( float angle )
{
	//	비행 타입일 경우에만 회전한다.
	if( m_PlayerType	==	ATRCRAFT )
	{
		D3DXMATRIX	T;
		D3DXMatrixRotationAxis( &T, &m_vLook, angle );

		//	Look 벡터 기준으로 위와 오른쪽으로 회전한다.
		D3DXVec3TransformCoord( &m_vRight, &m_vRight, &T );
		D3DXVec3TransformCoord( &m_vUp, &m_vUp, &T );
	}
}


//	Player Crash check
D3DXVECTOR3 Cplayer::SetPlayerCrash( D3DXVECTOR3 Monster_Pos )
{
	if( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetPlay_Status().m_Status == STATUS_DIE )	//	몬스터가 죽으면 몬스터의 충돌을 체크하지 않음
	{
		return m_vPos;
	}

	D3DXVECTOR3 new_vPos = m_vPos;						//	플레이어 현재의 위치벡터값을 저장
	D3DXVECTOR3 dir;									//	정규화 할 변수 선언

	D3DXVec3Normalize( &dir, &(new_vPos - Monster_Pos) );		//	플레이어 위치 - 몬스터 위치를 정규화 한것을 dir저장
	float fLen = D3DXVec3Length( &( new_vPos - Monster_Pos ) );	//	플레이어 위치 - 몬스터 위치 나온 값 fLen 저장

	if( fLen < 15.0f )									//	flen이 15.0f보다 적다면
	{
		float dist = 15.0f - fLen;						//	15.0f - flen 를 dist에 저장
		new_vPos += dir * dist;							//	dir * dist하여 new_vPos 원래 값에 더하여 리턴
		return new_vPos;
	}
	return m_vPos;
}


//	스테이터스 스텐스 상태
void	Cplayer::STANCE		(void)
{
	m_Status1.m_Status		=	STATUS_STANCE;
	m_Status1.StartFrame	=	1;
	m_Status1.EndFrame		=	10;
	m_Status1.PlayTime		=	0.5f;
	m_Status1.ActionCansle	=	true;
}


//	스테이터스 무브 상태
void	Cplayer::MOVE		(void)
{
	m_Status1.m_Status		=	STATUS_MOVE;
	m_Status1.StartFrame	=	12;
	m_Status1.EndFrame		=	24;
	m_Status1.PlayTime		=	0.75f;
	m_Status1.ActionCansle	=	true;
}

void	Cplayer::RUN		(void)
{
	m_Status1.m_Status		=	STATUS_RUN;
	m_Status1.StartFrame	=	12;
	m_Status1.EndFrame		=	24;
	m_Status1.PlayTime		=	0.5f;
	m_Status1.ActionCansle	=	true;
	m_Status1.m_Stamina		-=	0.05f;
}

//	스테이터스 기본 공격
void	Cplayer::VERTICAL	(void)
{
	m_Status1.m_Status		=	STATUS_VERTICAL;
	m_Status1.StartFrame	=	25;
	m_Status1.EndFrame		=	45;
	m_Status1.PlayTime		=	m_Status1.m_fAttackSpeed;
	m_Status1.ActionCansle	=	false;
	m_Status1.m_Stamina		-=	5.0f;
	m_Status1.m_Damage		=	m_Weapon_Damage;
}


//	스테이터스 좌 베기 공격
void	Cplayer::HORIZON_LEFT	(void)
{
	m_Status1.m_Status		=	STATUS_HORIZON_LEFT;
	m_Status1.StartFrame	=	51;
	m_Status1.EndFrame		=	78;
	m_Status1.PlayTime		=	m_Status1.m_fAttackSpeed;
	m_Status1.ActionCansle	=	false;
	m_Status1.m_Stamina		-=	5.0f;
	m_Status1.m_Damage		=	m_Weapon_Damage;
}


//	스테이터스 우 베기 공격
void	Cplayer::HORIZON_RIGHT	(void)
{
	m_Status1.m_Status		=	STATUS_HORIZON_RIGHT;
	m_Status1.StartFrame	=	81;
	m_Status1.EndFrame		=	104;
	m_Status1.PlayTime		=	m_Status1.m_fAttackSpeed;
	m_Status1.ActionCansle	=	false;
	m_Status1.m_Stamina		-=	5.0f;
	m_Status1.m_Damage		=	m_Weapon_Damage;
}


//	스테이터스 점프 공격
void	Cplayer::JUMPCUT		(void)
{
	m_Status1.m_Status		=	STATUS_JUMPCUT;
	m_Status1.StartFrame	=	111;
	m_Status1.EndFrame		=	148;
	m_Status1.PlayTime		=	m_Status1.m_fAttackSpeed;
	m_Status1.ActionCansle	=	false;
	m_Status1.m_Stamina		-=	5.0f;
	m_Status1.m_Damage		=	m_Weapon_Damage;
}


//	스테이터스 죽음
void	Cplayer::DIE			(void)
{
	m_Status1.m_Status		=	STATUS_DIE;
	m_Status1.StartFrame	=	171;
	m_Status1.EndFrame		=	219;
	m_Status1.PlayTime		=	2.0f;
	m_Status1.ActionCansle	=	false;
}

//	두번 찌르기
void	Cplayer::DOUBLESTAB		( void )
{
	m_Status1.m_Status		=	STATUS_DOUBLE_STAB;
	m_Status1.StartFrame	=	221;
	m_Status1.EndFrame		=	239;
	m_Status1.PlayTime		=	m_Status1.m_fAttackSpeed;
	m_Status1.ActionCansle	=	false;
	m_Status1.m_Stamina		-=	10.0f;
	m_Status1.m_Damage		=	m_Weapon_Damage * m_Status1.m_RealCombo;
}

//	두번 베기
void	Cplayer::DOUBLECUT		( void )
{
	if( m_cPlayerController.GetCurFrame() > 255.0f )
	{
		SOUNDMGR->PlaySound( CHAR_SKILL1 );
	}
	m_Status1.m_Status		=	STATUS_DOUBLE_CUT_BACKDASH;
	m_Status1.StartFrame	=	240;
	m_Status1.EndFrame		=	270;
	m_Status1.PlayTime		=	m_Status1.m_fAttackSpeed;
	m_Status1.ActionCansle	=	false;
	m_Status1.m_Stamina		-=	10.0f;
	m_Status1.m_Damage		=	m_Weapon_Damage * m_Status1.m_RealCombo;
}


void	Cplayer::DASH			( VOID )
{
	SOUNDMGR->PlaySound( CHAR_DASH );
	m_Status1.m_Status		=	STATUS_DASH;
	m_Status1.StartFrame	=	271;
	m_Status1.EndFrame		=	276;
	m_Status1.PlayTime		=	0.5f;
	m_Status1.ActionCansle	=	false;
	m_Status1.m_Stamina		-=	10.0f;
}

void	Cplayer::BEHIT			(void)
{
	m_Status1.m_Status		=	STATUS_BEHIT;
	m_Status1.StartFrame	=	171;
	m_Status1.EndFrame		=	174;
	m_Status1.PlayTime		=	0.5f;
	m_Status1.ActionCansle	=	false;
}



//	콤보 입력
void	Cplayer::SetCombo( void )
{
	m_NewTime	=	TIMEMGR->GetTotalTIme();

	//	첫 번깨 공격을 하고나서 0.8초가 지나면 키를 받음 0.5초 정도까지 줄여도 됨
	if( m_ComboBuffer[0] == -1 
		&& m_NewTime-m_Time > ( m_Status1.m_fAttackSpeed - 0.25f ) )		//	콤보 공격
	{
		//	DASH
		if( KEYMGR->StayKeyDown( 'A' )  && KEYMGR->OnceKeyDown( VK_SPACE ) && m_Status1.m_Stamina	> 10.0f 
			&& m_Status1.m_Status != STATUS_DASH )
		{
			m_ComboBuffer[1]	=	11;
			return ;
		}
		else if( KEYMGR->StayKeyDown( 'D' )  && KEYMGR->OnceKeyDown( VK_SPACE ) && m_Status1.m_Stamina	> 10.0f
			&& m_Status1.m_Status != STATUS_DASH )
		{
			m_ComboBuffer[1]	=	12;
			return ;
		}
		else if( KEYMGR->StayKeyDown( 'S' )  && KEYMGR->OnceKeyDown( VK_SPACE ) && m_Status1.m_Stamina	> 10.0f 
			&& m_Status1.m_Status != STATUS_DASH  )
		{
			m_ComboBuffer[1]	=	13;
			return ;
		}
		else if( KEYMGR->OnceKeyDown( VK_SPACE ) && m_Status1.m_Stamina	> 10.0f 
			&& m_Status1.m_Status != STATUS_DASH  )
		{
			m_ComboBuffer[1]	=	14;
			return ;
		}//	DASH END

		if( KEYMGR->OnceKeyDown( VK_RBUTTON )
			&& m_Status1.m_Status != STATUS_DOUBLE_CUT_BACKDASH && m_Status1.m_Stamina	> 15.0f && m_Status1.m_Status != STATUS_DASH )
		{

			m_ComboBuffer[1]	=	22;
			return ;
		}
		else if( KEYMGR->StayKeyDown( 'S' ) && KEYMGR->OnceKeyDown( VK_LBUTTON ) 
			&& m_Status1.m_Status !=  STATUS_JUMPCUT && m_Status1.m_Stamina	> 5.0f && m_Status1.m_Status != STATUS_DASH)
		{
			m_ComboBuffer[1]	=	'S';
			return ;
		}
		//else if( KEYMGR->StayKeyDown( 'W' ) && KEYMGR->OnceKeyDown( VK_LBUTTON ) 
		//	&& m_ComboBuffer[3] != 'W' && m_Status1.m_Stamina	> 10.0f )
		//{
		//	m_ComboBuffer[1]	=	'W';
		//	return ;
		//}
		else if( KEYMGR->StayKeyDown( 'A' ) && KEYMGR->OnceKeyDown( VK_LBUTTON ) 
			&& m_Status1.m_Status != STATUS_HORIZON_LEFT && m_Status1.m_Stamina	> 5.0f && m_Status1.m_Status != STATUS_DASH )
		{
			m_ComboBuffer[1]	=	'A';
			return ;
		}
		else if( KEYMGR->StayKeyDown( 'D' ) && KEYMGR->OnceKeyDown( VK_LBUTTON ) 
			&&  m_Status1.m_Status != STATUS_HORIZON_RIGHT && m_Status1.m_Stamina > 5.0f && m_Status1.m_Status != STATUS_DASH )
		{
			m_ComboBuffer[1]	=	'D';
			return ;
		}
		else if( KEYMGR->OnceKeyDown( VK_LBUTTON ) 
			&& m_Status1.m_Status != STATUS_VERTICAL && m_Status1.m_Stamina	> 5.0f && m_Status1.m_Status != STATUS_DASH )
		{
			m_ComboBuffer[1]	=	VK_LBUTTON;
			return ;
		}

		if( m_ActionCheck == true && m_NewTime - m_HitTime > m_Status1.m_fAttackSpeed + m_Status1.m_fAttackSpeed + 0.3f )
		{
			//printf("시간 완료\n");
			m_ActionCheck	=	false;
			m_Status1.m_CheckSkill	=	false;
			m_Status1.m_RealCombo	=	0;
			m_ComboBuffer[0]		=	NULL;
			m_ComboBuffer[1]		=	NULL;
		}
	}

	if( m_ComboBuffer[0]	==	NULL && m_Status1.ActionCansle == true )
	{

		//	DASH END
		if( KEYMGR->StayKeyDown( 'A' )  && KEYMGR->OnceKeyDown( VK_SPACE ) && m_Status1.m_Stamina	> 10.0f
			&& m_Status1.m_Status != STATUS_DASH  )
		{
			m_Time	=	TIMEMGR->GetTotalTIme();
			m_ComboBuffer[0]	=	11;
		}
		else if( KEYMGR->StayKeyDown( 'D' )  && KEYMGR->OnceKeyDown( VK_SPACE ) && m_Status1.m_Stamina	> 10.0f 
			&& m_Status1.m_Status != STATUS_DASH )
		{
			m_Time	=	TIMEMGR->GetTotalTIme();
			m_ComboBuffer[0]	=	12;
		}
		else if( KEYMGR->StayKeyDown( 'S' )  && KEYMGR->OnceKeyDown( VK_SPACE ) && m_Status1.m_Stamina	> 10.0f
			&& m_Status1.m_Status != STATUS_DASH  )
		{
			m_Time	=	TIMEMGR->GetTotalTIme();
			m_ComboBuffer[0]	=	13;
		}
		else if( KEYMGR->OnceKeyDown( VK_SPACE ) && m_Status1.m_Stamina	> 10.0f 
			&& m_Status1.m_Status != STATUS_DASH )
		{
			m_Time	=	TIMEMGR->GetTotalTIme();
			m_ComboBuffer[0]	=	14;
		}//	DASH END

		//else if( KEYMGR->StayKeyDown( 'W' ) && KEYMGR->OnceKeyDown( VK_LBUTTON) && m_Status1.m_Stamina > 5.0f)
		//{
		//	m_Time	=	TIMEMGR->GetTotalTIme();
		//	m_ComboBuffer[0]	=	'W';
		//}
		else if( KEYMGR->StayKeyDown( 'S' ) && KEYMGR->OnceKeyDown( VK_LBUTTON ) && m_Status1.m_Stamina	> 5.0f 
			&& m_Status1.m_Status != STATUS_DASH )
		{
			m_Time	=	TIMEMGR->GetTotalTIme();
			m_ComboBuffer[0]	=	'S';
		}
		else if( KEYMGR->StayKeyDown( 'A' ) && KEYMGR->OnceKeyDown( VK_LBUTTON ) && m_Status1.m_Stamina	> 5.0f
			&& m_Status1.m_Status != STATUS_DASH )
		{
			m_Time	=	TIMEMGR->GetTotalTIme();
			m_ComboBuffer[0]	=	'A';
		}
		else if( KEYMGR->StayKeyDown( 'D' ) && KEYMGR->OnceKeyDown( VK_LBUTTON ) && m_Status1.m_Stamina	> 5.0f
			&& m_Status1.m_Status != STATUS_DASH )
		{
			m_Time	=	TIMEMGR->GetTotalTIme();
			m_ComboBuffer[0]	=	'D';
		}
		else if( KEYMGR->OnceKeyDown( VK_LBUTTON ) && m_Status1.m_Stamina > 5.0f
			&& m_Status1.m_Status != STATUS_DASH )
		{
			m_Time	=	TIMEMGR->GetTotalTIme();
			m_ComboBuffer[0]	=	VK_LBUTTON;
		}
	}

}


//	콤보 출력
int	Cplayer::GetCombo	( void )
{
	m_NewTime	=	TIMEMGR->GetTotalTIme();
	if( m_ComboBuffer[1] )	//	두번째 공격
	{
		int Action = 0;
		Action = m_ComboBuffer[ 1 ];
		m_ComboBuffer[ 1 ]	=	NULL;			//	콤보 키를 누른 후 마지막 키로 저장시킬 경우 이걸 주석처리

		if( m_NewTime-m_Time > m_Status1.m_fAttackSpeed - 0.3f )
		{
			return Action;
		}
	}

	//	첫 번째 공격이 1이 아니거나, 첫 번째 공격이 0이 아니거나 콤보 공격하는 중이 아니라면
	if( m_ComboBuffer[ 0 ] != -1 && m_ComboBuffer[ 0 ] != NULL
		&& !m_ComboBuffer[1] )	//	첫번째 공격이 있으면 실행
	{
		//printf("첫 번째 공격  : %d\n", m_ComboBuffer[0] );
		m_Status1.m_RealCombo	=	0;
		int Action;
		Action = m_ComboBuffer[ 0 ];
		m_ComboBuffer[ 0 ] = -1;			//	첫번째 공격이 실행 되면 1로 바뀜
		return Action;
	}
	return -1;
}


//	타격 범위 체크
bool		Cplayer::CheckPlayerActionCrash( D3DXVECTOR3 Monster_Pos )
{
	D3DXVECTOR3  MonstertoPlayer, Now_vLook;
	MonstertoPlayer	=	( m_vPos - Monster_Pos );

	D3DXVec3Normalize( &Now_vLook, &m_vLook);		//	전방 방향 벡터 정규화
	Now_vLook *= m_Weapon_Length;					//	전방 방향 벡터 곱하기 5.0f 칼 범위
	Now_vLook += m_vPos;							//	전방 방향 벡터에 위치 벡터 더함

	if( m_Status1.m_Status != STATUS_DOUBLE_CUT_BACKDASH )
	{
		m_ActionLen = D3DXVec3Length( &( Monster_Pos - Now_vLook ) );	//	vLook은 칼 범위 위치 벡터에 몬스터 위치뺀 값 
		if( m_ActionLen < 18.0f )
		{
			//printf("공격 성공\n");
			return true;
		}

	}
	else if( m_Status1.m_Status == STATUS_DOUBLE_CUT_BACKDASH )
	{
		m_ActionLen = D3DXVec3Length( &( Monster_Pos - m_vPos ) );	//	vLook은 칼 범위 위치 벡터에 몬스터 위치뺀 값 
		if( m_ActionLen < 18.0f + m_Weapon_Length )
		{
			return true;
		}
	}
	return false;
}

//	체크 프레임 
bool		Cplayer::CheckActionFrameCrash( D3DXVECTOR3 Monster_Pos )
{
	D3DXVECTOR3  MonstertoPlayer, Now_vLook, MonstertoPlayer_dir, vCross;
	float	CrashFrame, NowFrame;
	MonstertoPlayer	=	( m_vPos - Monster_Pos );						//	몬스터와의 거리벡터

	D3DXVec3Normalize( &Now_vLook, &m_vLook );							//	현재 플레이어 전방 벡터
	D3DXVec3Normalize( &MonstertoPlayer_dir, &MonstertoPlayer );		//	몬스터를 가리키는 벡터

	D3DXVec3Cross( &vCross, &MonstertoPlayer_dir, &Now_vLook );			//	외적

	if( !(MOBMGR->MobVec(MOBMGR->GetIter()))->GetBeHit() && m_Status1.ActionCansle == false )				//	공격 한번당 체크가 안되었으면 실행
	{
		CrashFrame	=	m_Status1.EndFrame	-	m_Status1.StartFrame;		//	마지막 프레임과 스타트 프레임을 넣어 진행되는 프레임 값
		CrashFrame	=	m_Status1.StartFrame	+	(CrashFrame / 2.0f);	//	을 반으로 쪼갠후 스타트 프레임 과 덧셈하면 중간 프레임이 나옴
		//CrashFrame -=   vCross.y * 8 ;										//	외적의 각도만큼 프레임을 증가시키거나 낮춤
		NowFrame	=	m_cPlayerController.GetCurFrame();					//	현재 프레임

		if( CrashFrame-1.0f < NowFrame &&  CrashFrame+1.0f  > NowFrame )
		{
			//	원래는 여기 부분에서 프레임을 느리게 하거나 할 생각이였지만
			return true;								//	잘 안됨
		}
	}

	//	아 디버그 싫다...
	return false;
}

//	skill status check
void	Cplayer::CheckSkill( void )
{
	if(m_Status1.m_Status == STATUS_DOUBLE_CUT_BACKDASH || m_Status1.m_Status == STATUS_DOUBLE_STAB)
	{
		m_Status1.m_CheckSkill	=	true;
	}
	else
	{
		m_Status1.m_CheckSkill	=	false;
	}
}

//	피격 시
void	Cplayer::CheckBEHIT( void )
{
	if( m_CheckHP != m_Status1.m_HP )
	{
		BEHIT();
		m_Status1.m_CheckSkill	=	false;
		m_Status1.m_RealCombo	=	0;
		m_ComboBuffer[0]		=	NULL;
		m_ComboBuffer[1]		=	NULL;
		m_CheckHP	=	m_Status1.m_HP;
		//	피격 모션 바로 출력 
		m_cPlayerController.SetRange(m_Status1.StartFrame, m_Status1.EndFrame, m_Status1.PlayTime, m_Status1.Smotion);
	}
}

//	DASH EFFECT
void		Cplayer::DashEffectRight( float velocity )
{
	for ( float j = 0; j < 15; j ++ )
	{
		int	i = int(j);
		m_vTrail[i*2+0].p = m_vPos + D3DXVECTOR3(m_vRight.x, 0.0f, m_vRight.z) * velocity/14.0f*j;
		m_vTrail[i*2+0].p.y	= MAPMGR->GetHeight(m_vTrail[i*2+0].p.x,m_vTrail[i*2+0].p.z) + 3.0f + ( 0.285714f * j );
		m_vTrail[i*2+1].p = m_vPos + D3DXVECTOR3(m_vRight.x, 0.0f, m_vRight.z) * velocity/14.0f*j;
		m_vTrail[i*2+1].p.y	= MAPMGR->GetHeight(m_vTrail[i*2+1].p.x,m_vTrail[i*2+1].p.z) + 3.0f - ( 0.285714f * j );
		m_vTrail[i*2+0].c = D3DXCOLOR(1.0f, 1.0f, 1.0f, j/40.0f);
		m_vTrail[i*2+1].c = D3DXCOLOR(1.0f, 1.0f, 1.0f, j/40.0f);
	}
}

void		Cplayer::DashEffectLook( float velocity )
{
	for ( float j = 0; j < 15; j ++ )
	{
		int	i = int(j);
		m_vTrail[i*2+0].p = m_vPos + D3DXVECTOR3(m_vLook.x, 0.0f, m_vLook.z) * velocity/14.0f*j + D3DXVECTOR3(0.0f, 8.0f, 0.0f);
		m_vTrail[i*2+1].p = m_vPos + D3DXVECTOR3(m_vLook.x, 0.0f, m_vLook.z) * velocity/14.0f*j;
		m_vTrail[i*2+0].c = D3DXCOLOR(1.0f, 1.0f, 1.0f, j/40.0f);
		m_vTrail[i*2+1].c = D3DXCOLOR(1.0f, 1.0f, 1.0f, j/40.0f);
	}
}

void		Cplayer::DashEffectUpdate( void )
{
	for ( int i = 0; i < 14; i ++ )
	{
		m_vTrail[(i+0)*2+0].p = m_vTrail[(i+1)*2+0].p;
		m_vTrail[(i+0)*2+1].p = m_vTrail[(i+1)*2+1].p;
		m_vTrail[(i+0)*2+0].c = m_vTrail[(i+1)*2+0].c;
		m_vTrail[(i+0)*2+1].c = m_vTrail[(i+1)*2+1].c;
	}
	m_vTrail[14*2+0].c = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	m_vTrail[14*2+1].c = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	m_vTrail[14*2+0].p = m_vPos;
	m_vTrail[14*2+1].p = m_vPos;
}


void		Cplayer::DashEffectRenDer( void )
{
	//	초기화
	D3DFRAME->GetDevice()->SetTransform( D3DTS_WORLD,		&IdentityMatrix );
	D3DFRAME->GetDevice()->SetTransform( D3DTS_VIEW,		&D3DFRAME->m_mView );
	D3DFRAME->GetDevice()->SetTransform( D3DTS_PROJECTION,	&D3DFRAME->m_mProj );

	//	알파 값 투명화
	D3DFRAME->GetDevice()->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );

	//D3DFRAME->GetDevice()->SetRenderState ( D3DRS_ALPHATESTENABLE, FALSE );
	//D3DFRAME->GetDevice()->SetRenderState ( D3DRS_ALPHAREF, 0 );
	D3DFRAME->GetDevice()->SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	D3DFRAME->GetDevice()->SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	D3DFRAME->GetDevice()->SetRenderState ( D3DRS_LIGHTING, FALSE );
	D3DFRAME->GetDevice()->SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	//	요거랑
	D3DFRAME->GetDevice()->SetFVF ( D3DFVF_XYZ|D3DFVF_DIFFUSE );
	D3DFRAME->GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 28, m_vTrail, sizeof(STRAIL_VERTEX) );
	//	요기 까지
	D3DFRAME->GetDevice()->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CCW );
}

//	버프 입력
void	Cplayer::SetBuff( void )
{
	//	m_NuffSwith
	//	1:	흡혈
	//	2:	스테미너 무한
	//	3:	공속이속 증가
	//	4:	데미지 증가

	CheckBuffTime();	//	버프 타임 체크


	if( m_BuffTimeCheck == true )
	{
		if( KEYMGR->OnceKeyDown( '1' ) )
		{
			SOUNDMGR->PlaySound(CHAR_BUFF1 );	//	사운드
			m_BuffTimeCheck	=	false;
			m_BuffTime	=	TIMEMGR->GetTotalTIme();
			m_BuffSwith	=	1;
		}
		else if( KEYMGR->OnceKeyDown( '2' ) )
		{
			SOUNDMGR->PlaySound(CHAR_BUFF2 );
			m_BuffTimeCheck	=	false;
			m_BuffTime	=	TIMEMGR->GetTotalTIme();
			m_BuffSwith	=	2;
		}
		else if( KEYMGR->OnceKeyDown( '3' ) )
		{
			SOUNDMGR->PlaySound(CHAR_BUFF3 );
			m_BuffTimeCheck	=	false;
			m_BuffTime	=	TIMEMGR->GetTotalTIme();
			m_BuffSwith	=	3;
		}
		else if( KEYMGR->OnceKeyDown( '4' ) )
		{
			SOUNDMGR->PlaySound(CHAR_BUFF4 );
			m_BuffTimeCheck	=	false;
			m_BuffTime	=	TIMEMGR->GetTotalTIme();
			m_BuffSwith	=	4;
		}
	}

	CheckBuffSwith();	//	버프 상태 체크
}

//	버프가 바뀐 후 10초 후 실행
void	Cplayer::CheckBuffTime( void )
{
	if( TIMEMGR->GetTotalTIme() - m_BuffTime > 10.0f )
	{
		m_BuffTimeCheck	=	true;
	}
}

//	버프 상태를 체크하고 실행
void	Cplayer::CheckBuffSwith( void )
{

	switch( m_BuffSwith )
	{
	case 1:	//	흡혈됨
		{
			if( m_HitCheck == true && m_Vamp	== false )	//	공격이 성공하고 흡혈 중이 아니면
			{
				m_Vamp	=	true;
				if( m_Status1.m_HP < m_Status1.m_MaxHp )
				{
					int vampHP	=	m_Status1.m_MaxHp - m_Status1.m_HP;	//	현재 피와 맥스 피 비교
					if( vampHP < (int)( m_Status1.m_Damage * 0.2f ) )	//	최대 흡혈이 가능한 피가 데미지15%보다 적으면
					{
						m_Status1.m_HP += vampHP;	
					}
					else
					{
						m_Status1.m_HP += (int)( m_Status1.m_Damage * 0.2f );
					}
				}
			}
			else if( m_HitCheck == false )
			{
				m_Vamp	=	false;
			}

			m_StaminaUp	=	false;
			m_DamegeUp	=	false;
			m_SpeedUp	=	false;

			break;
		}
	case 2:	//	스테미너 100 
		{
			if( m_StaminaUp
				&& m_Status1.m_Stamina < 100
				&& m_Status1.m_Status != STATUS_RUN
				&& m_Status1.ActionCansle != FALSE
				 )
			{
				m_Status1.m_Stamina	+=	0.75f;
			}
			
			m_StaminaUp	=	true;
			m_SpeedUp	=	false;
			m_DamegeUp	=	false;
			break;
		}
	case 3:	//	공속 증가
		{
			if( !m_SpeedUp )
			{
				m_SpeedUp	=	true;
				m_SaveSpeed	=	m_Status1.m_fAttackSpeed;	//	원래 공속 저장
				m_Status1.m_fAttackSpeed	=	m_Status1.m_fAttackSpeed - ( m_Status1.m_fAttackSpeed * 0.25f );
			}

			m_StaminaUp	=	false;
			m_DamegeUp	=	false;
			break;
		}
	case 4:	//	데미지 증가
		{
			if( !m_DamegeUp )
			{
				m_DamegeUp	=	true;
				m_SaveWeapon_Damege	=	m_Weapon_Damage;	//	원래 데미지 저장
				m_Weapon_Damage	=	m_Weapon_Damage + (m_Weapon_Damage);
			}

			m_StaminaUp	=	false;
			m_SpeedUp	=	false;
			break;
		}
	}

	if( !m_DamegeUp )	//	데미지 업이 false 일때 원래 데미지로 돌림
	{
		m_Weapon_Damage	=	m_SaveWeapon_Damege;
	}

	if( !m_SpeedUp )	//	스피드 업이 false 일때 원래 스피드로 돌림
	{
		m_Status1.m_fAttackSpeed = m_SaveSpeed;
	}

	// 스태미너가 뛸때 빼고 찬다
	if( !m_StaminaUp
		&& m_Status1.m_Stamina < 100
		&& m_Status1.m_Status != STATUS_RUN
		&& m_Status1.ActionCansle != FALSE )
	{
		m_Status1.m_Stamina += 0.5f;
	}
}

Cplayer*	Cplayer::GetInst()
{
	if ( Player_Inst	==	NULL )
	{
		Player_Inst	=	new Cplayer;
		RSCMGR->m_bCHAR_MGR	=	true;
	}
	return Player_Inst;
}


void	Cplayer::FreeInst()
{
	if ( Player_Inst	!= NULL )
	{
		delete Player_Inst;
		Player_Inst	= NULL;
		RSCMGR->m_bCHAR_MGR	=	false;
	}
}