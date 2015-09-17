#include "MainHeader.h"

CMobAlduin::CMobAlduin(void)
{
}

CMobAlduin::~CMobAlduin(void)
{
}


void	CMobAlduin::Init( D3DXVECTOR3 MobPos, D3DXVECTOR3 MobScl, float MobAngle )
{
	m_vRight	=	D3DXVECTOR3( -1.0f, 0.0f, 0.0f );
	m_vUp		=	D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	m_vLook		=	D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
	m_Velocity	=	0.05f;
	m_FirstCheck	=	false;
	m_BeHitCheck	=	false;

	m_ActionStartTime	=	0.0f;
	m_ActionChoiceCheck	=	false;
	m_Monsterchoise		=	0;
	m_bDashHit			=	false;
	m_CheckBREATH		=	false;

	m_vPos		=	MobPos;
	m_fangle	=	MobAngle;
	m_vScl		=	MobScl;

	if ( MobPos == D3DXVECTOR3(0.0f,0.0f,0.0f) && MobScl == D3DXVECTOR3(0.0f,0.0f,0.0f) && MobAngle == 0.0f )
	{
		m_vPos		=	D3DXVECTOR3( 100.0f, 0.0f, 100.0f );		
		m_fangle	=	2.7f;
		m_vScl		=	D3DXVECTOR3 ( 0.1f, 0.1f, 0.1f );
	}
	m_vRelease_Pos	=	m_vPos;

	yaw( m_fangle );

	SetStatusInfo();			//	스테이터스 초기화
	m_SkinRenderer.Init("./SHADER/alduin.BSM");					//	메쉬파일
	CSkinRenderer::LoadEffect("./SHADER/skin_render.fx");		//	셰이더 파일
	m_cMobAlduinController.Init( &m_SkinRenderer );					//	메쉬 초기화
	m_cMobAlduinController.SetScl( m_vScl );						//	스케일

	pBreath1	= new Gun(&vPivot, &vVel);
	pBreath1->init(D3DFRAME->GetDevice(), /*"flare.bmp"*/"flare2.bmp");

	pBreath2	= new blast(&D3DXVECTOR3(0.0f, 0.0f, 0.0f), 1);
	pBreath2->init(D3DFRAME->GetDevice(), /*"flare.bmp"*/"flare2.bmp");

	pGrass = new Fire(&vPivot2, 1);
	pGrass->init(D3DFRAME->GetDevice(), /*"particle3.bmp"*/"grass.bmp");

	FIRSTSHOW();
}

void	CMobAlduin::Update( void )
{
	AI_MainSystem();
	
	SetBREATHAction();

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

	m_cMobAlduinController.SetPos( m_vPos );
	m_cMobAlduinController.SetRot(D3DXVECTOR3( 0.0f, m_fangle, 0.0f ) );

	int				nHead	= m_cMobAlduinController.GetBoneNoFromName( "BIP01 HEAD\0" );	//	머리
	PBONE_INSTANCE	pIBone	= m_cMobAlduinController.GetIBoneInfo();						//	뼈대 구조체
	D3DXMATRIX		mBreath	= pIBone[nHead].dmSumRot;										//	브레스
	D3DXMATRIX		mWorld	= m_cMobAlduinController.GetWorldMatrix();						//	월드 행렬
	D3DXMATRIX		mRWorld = mWorld;
	mRWorld._14 = 0.0f;
	mRWorld._24 = 0.0f;
	mRWorld._34 = 0.0f;
	mRWorld._41 = 0.0f;
	mRWorld._42 = 0.0f;
	mRWorld._43 = 0.0f;

	//	불의 각도를 꺽어보기위해
	//D3DXMATRIX mRotX;
	//float fAngleX = 200 * 3.14/180; 
	//D3DXVECTOR3	vEnDirX;
	//D3DXMatrixRotationX( &mRotX, fAngleX ); 
	//D3DXVec3TransformCoord( &vEnDirX, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), &mRotX ); 

	//D3DXVECTOR3 vTaget	=	D3DXVECTOR3(m_vPos.x, m_vPos.y, m_vPos.z) - D3DXVECTOR3(mBreath._11, mBreath._12, mBreath._13);
	//D3DXVec3Normalize( &vTaget, &vTaget );	

	D3DXVec3TransformCoord ( &vVel, &D3DXVECTOR3(0.0f, 0.0f, -10.0f), &( mBreath * mRWorld ) );

	//vVel -= vEnDirX;

	D3DXVec3TransformCoord ( &vPivot, &(pIBone[nHead].vPivot), &mWorld );

	//용 왼쪽 다리 좌표값
	int	nLeg	= m_cMobAlduinController.GetBoneNoFromName( "BIP01 L LEG\0" );
	D3DXVec3TransformCoord( &vPivot2, &(pIBone[nLeg].vPivot), &mWorld );
	//vPivot2 += m_vLook * 3.0f;

	pBreath1->update();
	pBreath2->update();

	pGrass->update();


	HITCHECK();

	m_cMobAlduinController.UpdateFrame();
}
void	CMobAlduin::Render( void )
{
	//TCHAR	str_Text[256];	// 문자열 변수
	//ZeroMemory(str_Text,sizeof(str_Text)); //문자열 변수 초기화
	//sprintf(str_Text,"몬스터 상태 : %d %d \n", m_ActionSwith, m_Monsterchoise ); // 이런식으로 사용
	//TEXTMGR->Print(str_Text, 0, 120); // 텍스트출력
	//sprintf(str_Text,"몬스터와의 거리 : %f\n", SetLength( CHARMGR->GetPos() ) ); // 이런식으로 사용
	//TEXTMGR->Print(str_Text, 260, 120); // 텍스트출력
	//sprintf(str_Text,"몬스터 프레임 : %f\n", m_cMobAlduinController.GetCurFrame() ); // 이런식으로 사용
	//TEXTMGR->Print(str_Text, 500, 120); // 텍스트출력


	m_cMobAlduinController.Render( m_BeHitCheck, NULL );

	pBreath1->render();
	pBreath2->render();
	pGrass->render();
}


//	상향 벡터로 회전
void CMobAlduin::yaw	( float angle )
{
	D3DXMATRIX	T;

	//	지상의 물체에 대해서는 월드 y (0, 1, 0)을 기준으로 회전한다.
	D3DXMatrixRotationY( &T, angle );

	//	
	D3DXVec3TransformCoord(&m_vRight, &m_vRight, &T);
	D3DXVec3TransformCoord(&m_vLook, &m_vLook, &T);
}

//	스테이터스 정보 초기화
void	CMobAlduin::SetStatusInfo( void )
{
	m_Status.m_Status		=	STATUS_STANCE;		//	현재 스테이터스
	m_Status.StartFrame		=	41;					//	첫번째 프레임
	m_Status.EndFrame		=	55;					//	마지막 프레임
	m_Status.PlayTime		=	2.0f;				//	프레임 뿌리는 시간 2초
	m_Status.Smotion		=	0.4f;				//	보간모션	0.05초
	m_Status.ActionCansle	=	true;				//	스테이터스 캔슬 여부
	m_Status.m_Dead			=	false;				//	죽었는지 체크
	m_Status.m_HP			=	10000;			//	HP
	m_Status.m_MaxHp		=	10000;			//  풀 HP
	m_Status.m_Damage		=	300.0f + ( D3DXVec3Length( &m_vScl ) );				//	데미지
	m_Status.m_Stamina		=	1000;			//	Stamina

	m_CheckHP	=	m_Status.m_HP;				//	체크 HP 초기화
}

void	CMobAlduin::FIRSTSHOW( void )
{
	m_Status.m_Status		=	STATUS_FIRST_SHOW;
	m_Status.StartFrame		=	1;
	m_Status.EndFrame		=	40;
	m_Status.PlayTime		=	5.0f;
	m_Status.Smotion		=	0.4f;
	m_Status.ActionCansle	=	false;
}
void	CMobAlduin::DIE	( void )
{
	m_Status.m_Status		=	STATUS_DIE;
	m_Status.StartFrame		=	281;
	m_Status.EndFrame		=	310;
	m_Status.PlayTime		=	7.0f;
	m_Status.Smotion		=	0.01f;
	m_Status.ActionCansle	=	false;
}

void	CMobAlduin::STANCE		( void )
{
	m_Status.m_Status		=	STATUS_STANCE;
	m_Status.StartFrame		=	41;
	m_Status.EndFrame		=	55;
	m_Status.PlayTime		=	2.0f;
	m_Status.Smotion		=	0.01f;
	m_Status.ActionCansle	=	true;
}

void	CMobAlduin::MOVE( void )
{
	m_Status.m_Status		=	STATUS_MOVE;
	m_Status.StartFrame		=	322;
	m_Status.EndFrame		=	335;
	m_Status.PlayTime		=	1.0f;
	m_Status.Smotion		=	0.01f;
	m_Status.ActionCansle	=	true;
}

void	CMobAlduin::VERTICAL( void )
{
	m_Status.m_Status		=	STATUS_VERTICAL;
	m_Status.StartFrame		=	250;
	m_Status.EndFrame		=	280;
	m_Status.PlayTime		=	2.0f;
	m_Status.Smotion		=	0.01f;
	m_Status.ActionCansle	=	false;
}

void	CMobAlduin::BREATH( void )
{
	m_Status.m_Status		=	STATUS_BREATH;
	m_Status.StartFrame		=	312;
	m_Status.EndFrame		=	320;
	m_Status.PlayTime		=	2.0f;
	m_Status.Smotion		=	0.01f;
	m_Status.ActionCansle	=	false;
}

void	CMobAlduin::JUMP_DRAGON_BREATH( void )
{
	m_Status.m_Status		=	STATUS_JUMP_DRAGON_BREATH;
	m_Status.StartFrame		=	120;
	m_Status.EndFrame		=	250;
	m_Status.PlayTime		=	7.0f;
	m_Status.Smotion		=	0.01f;
	m_Status.ActionCansle	=	false;
}

void	CMobAlduin::LEFT_BREATH( void )
{
	m_Status.m_Status		=	STATUS_LEFT_BREATH;
	m_Status.StartFrame		=	60;
	m_Status.EndFrame		=	85;
	m_Status.PlayTime		=	3.0f;
	m_Status.Smotion		=	0.01f;
	m_Status.ActionCansle	=	false;
}
void	CMobAlduin::RIGHT_BREATH( void )
{
	m_Status.m_Status		=	STATUS_RIGHT_BREATH;
	m_Status.StartFrame		=	90;
	m_Status.EndFrame		=	115;
	m_Status.PlayTime		=	3.0f;
	m_Status.Smotion		=	0.01f;
	m_Status.ActionCansle	=	false;
}

void	CMobAlduin::DASH( void )
{
	m_Status.m_Status		=	STATUS_DASH;
	m_Status.StartFrame		=	312;
	m_Status.EndFrame		=	320;
	m_Status.PlayTime		=	4.5f;
	m_Status.Smotion		=	0.01f;
	m_Status.ActionCansle	=	false;
}

//	AI Main
void	CMobAlduin::AI_MainSystem	( void )
{
	if( m_Status.m_HP < 1)
	{
		DIE();
	}


	if( SCENEMGR->GetNowScene() != MENU )
	{
		SetAction();	//	m_ActionSwith를 정해줌

		switch( m_ActionSwith )
		{
		case 1:		//	타겟팅
			{
				if ( m_ActionChoiceCheck != true )
				{
					m_ActionStartTime	=	TIMEMGR->GetTotalTIme();
				}
				m_ActionChoiceCheck	=	true;
				////	AI_Look
				SetLook( CHARMGR->GetPos() );

				break;
			}
		case 2:		//	이동
			{
				if ( m_ActionChoiceCheck != true )
				{
					m_ActionStartTime	=	TIMEMGR->GetTotalTIme();
				}
				m_ActionChoiceCheck	=	true;

				//	AI_MOVE
				AI_StatusMove( CHARMGR->GetPos() );
				break;
			}
		case 3:		//	공격
			{
				if ( m_ActionChoiceCheck != true )
				{
					m_ActionStartTime	=	TIMEMGR->GetTotalTIme();
				}
				m_ActionChoiceCheck	=	true;

				////	AI Action
				AI_StatusAction();
				break;
			}
		case 4:		//	공격
			{
				if ( m_ActionChoiceCheck != true )
				{
					m_ActionStartTime	=	TIMEMGR->GetTotalTIme();
				}
				m_ActionChoiceCheck	=	true;

				////	AI Action
				AI_StatusAction();
				break;
			}
		case 5:		//	대쉬 상태
			{
				if ( m_ActionChoiceCheck != true )
				{
					m_ActionStartTime	=	TIMEMGR->GetTotalTIme();
				}
				m_ActionChoiceCheck	=	true;

				if( m_Status.m_Status == STATUS_DASH && TIMEMGR->GetTotalTIme() - m_ActionStartTime > 2.3f )
				{
					m_bDashHit	=	true;
					D3DXVec3Normalize( &m_vLook, &m_vLook);

					m_vPos.x += ( ( m_vLook.x ) + m_Velocity * 0.007f );
					m_vPos.z += ( ( m_vLook.z ) + m_Velocity * 0.007f );
				}
				break;
			}
		}
	}

	//	판정 체크

	if( m_Status.ActionCansle == false )
	{
		if( SCENEMGR->GetNowScene() != MENU && CheckActionFrameCrash( CHARMGR->GetPos() ) )	//	메뉴가 아니거나 프레임 체크가 확정되면
		{
			if( CheckActionCrash( CHARMGR->GetPos() ) )	//	물리적 공격 판정 체크
			{
				m_ActionCheck		=	true;
				CHARMGR->SetDamageCheck( m_Status.m_Damage );	//	맞으면 데미지 들어감
				CHARMGR->CheckBEHIT();							//	맞으면 아야함
			}
		}
	}

	//	애니메이션
	if( m_Status.m_Status == STATUS_FIRST_SHOW )
	{
		if( m_cMobAlduinController.SetRangeNoLoop( m_Status.StartFrame, m_Status.EndFrame, 
			m_Status.StartFrame, m_Status.EndFrame, m_Status.PlayTime ) == UPDATE_PAUSE )
		{
			STANCE();
			return ;
		}
	}
	if( m_cMobAlduinController.SetRange( m_Status.StartFrame, m_Status.EndFrame, m_Status.PlayTime, m_Status.Smotion )
		==	UPDATE_RETURN )
	{
		STANCE();
	}


}

//	AI MOVE
void	CMobAlduin::AI_StatusMove	( D3DXVECTOR3 Pos )
{
	//	스테이터스가 DIE 상태일 경우
	if( m_Status.m_Status == STATUS_DIE )
	{
		m_cMobAlduinController.SetRangeNoLoop(m_Status.StartFrame,
			m_Status.EndFrame,
			m_Status.StartFrame,
			m_Status.EndFrame,
			m_Status.PlayTime );
		return ;
	}

	if( SetLength(CHARMGR->GetPos() ) > 20.0f && m_Status.ActionCansle == true  )
	{
		D3DXVec3Normalize( &m_vLook, &m_vLook);
		MOVE();
		m_vPos.x += ( m_vLook.x /2 );
		m_vPos.z += ( m_vLook.z /2 );
	}
}

//	AI Action
void	CMobAlduin::AI_StatusAction	( void )
{
	//	스테이터스가 DIE 상태일 경우
	if( m_Status.m_Status == STATUS_DIE )
	{
		m_cMobAlduinController.SetRangeNoLoop(m_Status.StartFrame,
			m_Status.EndFrame,
			m_Status.StartFrame,
			m_Status.EndFrame,
			m_Status.PlayTime );
		return;
	}

	if( SCENEMGR->GetNowScene() != MENU &&  m_Status.ActionCansle == true 
		//	몬스터의 거리가 25안이고 때릴수 있는 상태이고
		)										//	몬스터와 플레이어의 각도의 길이가 0에 가까우면
	{
		m_Monsterchoise	=	false;
		if( m_fDot < 0.6f )
		{
			SetLook( CHARMGR->GetPos() );
		}

		if( SetLength( CHARMGR->GetPos() ) > 100.0f )
		{
			m_Monsterchoise = rand()%2+4;
		}
		else if( SetLength( CHARMGR->GetPos() ) > 65.0f )
		{
			m_Monsterchoise = rand()%2+2;
		}
		else if( SetLength( CHARMGR->GetPos() ) < 65.0f )	
		{
			m_Monsterchoise = rand()%2;
		}

		switch( m_Monsterchoise )
		{
			//	근접 공격
		case 0:
			{
				m_ActionCheck	=	false;	//	공격 판정 체크할 때 쓰임
				m_CheckBREATH	=	false;
				VERTICAL();
				break;
			}
		case 1:
			{
				if( m_CheckHP - m_Status.m_HP > 800 )	//	공중 브레스
				{
					m_CheckHP	=	m_Status.m_HP;
					m_ActionCheck	=	false;
					m_CheckBREATH	=	false;
					JUMP_DRAGON_BREATH();
					break;
				}
				else if( m_CheckHP - m_Status.m_HP > 300 )	//	대쉬
				{
					m_ActionCheck	=	false;
					DASH();
					break;
				}
				else if( SetLength(CHARMGR->GetPos() ) > 20 )	//	브레스
				{
					m_ActionCheck	=	false;
					m_CheckBREATH	=	false;
					BREATH();
					break;
				}
				break;
			}
			//	중거리 공격
		case 2:	//	좌측 브레스
			{
				m_ActionCheck	=	false;
				m_CheckBREATH	=	false;
				LEFT_BREATH();
				break;
			}
		case 3:	//	우측 브레스
			{
				m_ActionCheck	=	false;
				m_CheckBREATH	=	false;
				RIGHT_BREATH();
				break;
			}
			//	장거리
		case 4:	//	브레스
			{
				m_ActionCheck	=	false;
				m_CheckBREATH	=	false;
				BREATH();
				break;
			}
		case 5:	//	대쉬
			{
				m_ActionCheck	=	false;
				DASH();
				break;
			}
		}//	switch end
	}//	if end

}

//	Player to ( 0.0f, m_vPos.y, 0.0f ) Length
float	CMobAlduin::SetLength( D3DXVECTOR3 Player_Pos )
{
	float fLen	=	D3DXVec3Length( &(Player_Pos - m_vPos ) );	//	플레이어와 몬스터의 거리
	return fLen;
}

//	시야 체크
void CMobAlduin::SetLook( D3DXVECTOR3 Player_Pos )
{
	//	스테이터스가 DIE 상태일 경우
	if( m_Status.m_Status == STATUS_DIE )
	{
		m_cMobAlduinController.SetRangeNoLoop(m_Status.StartFrame,
			m_Status.EndFrame,
			m_Status.StartFrame,
			m_Status.EndFrame,
			m_Status.PlayTime );
		return;
	}

	D3DXVECTOR3 vCross, m_vMonster_NewDir;



	m_vMonster_dir = ( Player_Pos - m_vPos );						//	적이 플레이어를 바라보는 방향

	D3DXVec3Normalize( &m_vMonster_NewDir, &m_vMonster_dir );		//	그 방향 정규화
	D3DXVec3Normalize( &m_vLook, &m_vLook);							//	원래 방향 정규화

	D3DXVec3Cross( &vCross, &m_vMonster_NewDir, &m_vLook );			//	적이 플레이어 바라보는 방향과 원래 방향으로 외적
	m_fDot	=	D3DXVec3Dot( &m_vMonster_NewDir, &m_vLook );
	m_fLen = D3DXVec3Length( &vCross );


	if( m_Status.ActionCansle == true )
	{
		if( m_fDot < 0.99f )
		{
			m_fangle -= vCross.y*0.2f;		//	외적한 벡터의 y 마큼 회전 시킴
			yaw( -vCross.y*0.2f );
		}
	}
}

//	판정 체크
bool	CMobAlduin::CheckActionCrash( D3DXVECTOR3 Player_Pos )
{
	D3DXVECTOR3  MonstertoPlayer, Now_vLook;
	MonstertoPlayer	=	( Player_Pos - m_vPos );

	D3DXVec3Normalize( &Now_vLook, &m_vLook);		//	전방 방향 벡터 정규화

	if( m_Status.m_Status == STATUS_VERTICAL )
	{
		Now_vLook *= 20.0f;								//	전방 방향 벡터 곱하기 30.0f 칼 범위
		Now_vLook += m_vPos;							//	전방 방향 벡터에 위치 벡터 더 함
		float fLen = D3DXVec3Length( &( Player_Pos - Now_vLook ) );	//	vLook은 칼 범위 위치 벡터에 몬스터 위치뺀 값 
		if( fLen < 15.0f)			//	공격 범위 15
		{
			return true;
		}
	}
	else if( m_Status.m_Status == STATUS_DASH && m_bDashHit == true )
	{
		Now_vLook *= 10.0f;								//	전방 방향 벡터 곱하기 30.0f 칼 범위
		Now_vLook += m_vPos;							//	전방 방향 벡터에 위치 벡터 더 함
		float fLen = D3DXVec3Length( &( Player_Pos - Now_vLook ) );	//	본인의 위치에서
		if( fLen < 20.0f )			//	공격 범위 28
		{
			return true;
		}
	}
	return false;
}

//	프레임 체크
bool	CMobAlduin::CheckActionFrameCrash( D3DXVECTOR3 Player_Pos )
{
	D3DXVECTOR3  MonstertoPlayer, Now_vLook, MonstertoPlayer_dir, vCross;
	float	CrashFrame, NowFrame;

	MonstertoPlayer	=	( m_vPos - Player_Pos );					//	몬스터와의 거리벡터
	D3DXVec3Normalize( &Now_vLook, &m_vLook );						//	현재 플레이어 전방 벡터

	if( !m_ActionCheck )	//	공격 한번당 체크가 안되었으면 실행
	{
		if(m_Status.m_Status == STATUS_VERTICAL )
		{
			CrashFrame	=	m_Status.EndFrame	-	m_Status.StartFrame;		//	마지막 프레임과 스타트 프레임을 넣어 진행되는 프레임 값
			CrashFrame	=	m_Status.StartFrame	+	(CrashFrame / 2.0f);	//	을 반으로 쪼갠후 스타트 프레임 과 덧셈하면 중간 프레임이 나옴
			NowFrame	=	m_cMobAlduinController.GetCurFrame();					//	현재 프레임

			if( CrashFrame - 1.5f < NowFrame && CrashFrame + 1.5f > NowFrame )
			{
				return true;
			}
		}//	스테이터스 상태 체크
		else if( m_Status.m_Status == STATUS_DASH  && m_bDashHit == true )
		{
			return true;
		}
	}

	return false;
}

//	Player Crash check
D3DXVECTOR3 CMobAlduin::SetMonsterCrash( D3DXVECTOR3 Monster_Pos )
{
	if( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetPlay_Status().m_Status == STATUS_DIE )	//	몬스터가 죽으면 몬스터의 충돌을 체크하지 않음
	{
		return m_vPos;
	}

	D3DXVECTOR3 new_vPos = m_vPos;						//	플레이어 현재의 위치벡터값을 저장
	D3DXVECTOR3 dir;									//	정규화 할 변수 선언

	D3DXVec3Normalize( &dir, &(new_vPos - Monster_Pos) );		//	플레이어 위치 - 몬스터 위치를 정규화 한것을 dir저장
	float fLen = D3DXVec3Length( &( new_vPos - Monster_Pos ) );	//	플레이어 위치 - 몬스터 위치 나온 값 fLen 저장

	if( m_Status.ActionCansle == true && fLen < 40.0f )									//	flen이 15.0f보다 적다면
	{
		float dist = 40.0f - fLen;						//	15.0f - flen 를 dist에 저장
		new_vPos += dir * dist;							//	dir * dist하여 new_vPos 원래 값에 더하여 리턴
		return new_vPos;
	}
	return m_vPos;
}

void	CMobAlduin::HITCHECK()
{
	m_HitEndTime	=	TIMEMGR->GetTotalTIme();
	if( m_HitEndTime - m_HitTime > 0.1f )
	{
		m_BeHitCheck = false;
	}
}

//	m_ActionSwith를 정해줌
void	CMobAlduin::SetAction( void )
{
	if( m_Status.m_Status == STATUS_DASH )	//	대쉬 상태
	{
		m_ActionSwith	=	5;
	}
	else if( SetLength( CHARMGR->GetPos() ) > 150.0f //	플레이어와 거리가 멀어진다면
		&& m_ActionSwith != 1					 // 몬스터가 시야를 보지 않는다면
		&& m_Status.m_Status != STATUS_DASH )	 //	대쉬 상태가 아니라면
	{
		m_ActionSwith	=	1;
		m_ActionChoiceCheck = false;
		m_bDashHit = false;
	}
	else if( TIMEMGR->GetTotalTIme() - m_ActionStartTime > 0.65f	//	0.65초 마다 1,2,3,4 랜덤하게 나옴
		&& m_Status.m_Status != STATUS_DASH )
	{
		m_ActionSwith	=	rand()%4+1;
		m_ActionChoiceCheck = false;
		m_bDashHit = false;
	}
}

//	몬스터가 브레스 사용 시
void	CMobAlduin::SetBREATHAction( void )
{

	if( !m_CheckBREATH )	//	새로운 브레스를 사용 시 들어옴
	{
		if( m_Status.m_Status	==	STATUS_BREATH )
		{
			if ( m_cMobAlduinController.GetCurFrame() >= 317.0f && m_cMobAlduinController.GetCurFrame() <= 320.0f )
			{
				m_CheckBREATH	=	true;
				pBreath1->addParticles(80);
				pBreath1->SerialParticles(&pBreath2, &pBreath2);
			}
		}
		else  if( m_Status.m_Status == STATUS_VERTICAL )
		{
			if( m_cMobAlduinController.GetCurFrame() >= 260.0f && m_cMobAlduinController.GetCurFrame() <= 270.0f )
			{
 				//m_CheckBREATH	=	true;
				//용이 밟을때 프레임사이에다가 집어넣어보삼
				pGrass->addParticles(5);
			}
		}
		else if( m_Status.m_Status == STATUS_JUMP_DRAGON_BREATH )
		{
			if ( m_cMobAlduinController.GetCurFrame() >= 160.0f && m_cMobAlduinController.GetCurFrame() <= 180.0f ) 
			{
				//m_CheckBREATH	=	true;
				pBreath1->addParticles(30);
				pBreath1->SerialParticles(&pBreath2, &pBreath2);
			}
		}
		else if( m_Status.m_Status == STATUS_LEFT_BREATH )
		{
			if ( m_cMobAlduinController.GetCurFrame() >= 73.0f &&  m_cMobAlduinController.GetCurFrame() <= 83.0f )
			{
				//m_CheckBREATH	=	true;
				pBreath1->addParticles(15);
				pBreath1->SerialParticles(&pBreath2, &pBreath2);
			}
		}
		else if( m_Status.m_Status == STATUS_RIGHT_BREATH )
		{
			if ( m_cMobAlduinController.GetCurFrame() >= 103.0f &&  m_cMobAlduinController.GetCurFrame() <= 113.0f ) 
			{
				//m_CheckBREATH	=	true;
				pBreath1->addParticles(15);
				pBreath1->SerialParticles(&pBreath2, &pBreath2);
			}
		}
		else if( m_Status.m_Status	==	STATUS_FIRST_SHOW )
		{
			if( m_cMobAlduinController.GetCurFrame() >= 15.0f && m_cMobAlduinController.GetCurFrame() <= 25.0f )
			{
				//m_CheckBREATH	=	true;
				pBreath1->addParticles(20);
				pBreath1->SerialParticles(&pBreath2, &pBreath2);
			}
		}
	}
	
}

