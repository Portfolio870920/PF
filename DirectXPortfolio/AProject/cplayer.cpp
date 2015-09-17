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
	m_PlayerType	=	LANDOBJECT;					//	������ 
	m_Velocity		=	0.4f;						//	�ӵ�
	m_Time			=	0.0f;						//	ù �޺� ���۽ð�
	m_NewTime		=	0.0f;						//	���� �ð�
	m_ActionLen		=	0.0f;						//	���� �Ÿ�
	m_Weapon		=	Weapon;						//	���� �� ����
	m_ComboBuffer[0]	=	NULL;					//	�޺� ���� �ʱ�ȭ
	m_ComboBuffer[1]	=	NULL;
	m_BuffSwith		=	0;							//	���� ����
	m_BuffTime		=	0.0f;						//	���� ������ �ð�
	m_BuffTimeCheck	=	true;						//	�����ð��� 10�� ������ true
	m_HitCheck		=	false;						//	������ �­��°� üũ
	m_ActionCheck	=	false;						//	������ �ߴ°� üũ

	m_DamegeUp	=	false;							//	������ �� ������ ����Ǹ� true
	m_SpeedUp	=	false;							//	���ǵ� �� ������ ����Ǹ� true
	m_StaminaUp	=	false;							//	���׹̳� ������ ����Ǹ� true


	SetStatusInfo();								//	�������ͽ� �ʱ�ȭ

	m_CheckHP	=	m_Status1.m_HP;
	m_vPos		=	D3DXVECTOR3( 0.0f, 114.0f, 0.0f );
	m_vRight	=	D3DXVECTOR3( -1.0f, 0.0f, 0.0f );
	m_vUp		=	D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	m_vLook		=	D3DXVECTOR3( 0.0f, 0.0f, -1.0f );

	m_fcamangle	=	0.0f;
	m_fangle	=	-2.86f;
	yaw( -2.86f );

	m_cSkinRenderer.Init("./SHADER/SD.hero1.BSM");						//	�޽�����
	CSkinRenderer::LoadEffect("./SHADER/skin_render.fx");			//	���̴� ����
	m_cPlayerController.Init( &m_cSkinRenderer );						//	�޽� ����
	m_cPlayerController.SetPos( m_vPos );
	m_cPlayerController.SetScl( D3DXVECTOR3 ( 0.1f, 0.1f, 0.1f ) );	
}

void	Cplayer::Update( void )
{	
	//( m_Status1.m_HP<100 )? m_Status1.m_HP+=100 : m_Status1.m_HP; // �� ��� �� ����׿�
	GetCursorPos(&m_pos_MouseCursor);
	StatusMainSysstem( );

	MOBMGR->SetIterBengin();
	while( MOBMGR->GetIter() != MOBMGR->IterEnd() )
	{
		m_vPos		=	SetPlayerCrash( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetMonsterPos() );
		MOBMGR->IterInc();
	}
	m_vPos.y	=	MAPMGR->GetHeight(m_vPos.x,m_vPos.z,NULL,&m_vPosNormal);
	// �̲����� ���� �κ�
	if ( D3DXVec3Dot( &m_vPosNormal, &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) ) <= 0.85f ) // y��� ����ġ �ﰢ�� ������ �����Ͽ� ���� ���� 
	{
		// �̲����� ���� ���
		m_vSlideVec	=	( -(D3DXVECTOR3(0.0f,1.0f,0.0f) - ( ( D3DXVec3Dot(&D3DXVECTOR3(0.0f,1.0f,0.0f),&m_vPosNormal) ) / ( D3DXVec3Dot(&m_vPosNormal ,&m_vPosNormal) ) ) * m_vPosNormal) );
	}
	else
	{
		m_vSlideVec	=	D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); // �������� 0.85 �̻��̸� �̲����� ����
	}
	m_vPos	+=	m_vSlideVec; // �̲����� ���� ����

	m_cPlayerController.SetPos( m_vPos );
	m_cPlayerController.SetRot( D3DXVECTOR3(0.0f, m_fangle, 0.0f ) );

	
	CAMMGR->SetPlayerinfo(m_vPos, m_vRight, m_vUp, m_vLook);

	DashEffectUpdate();

	m_cPlayerController.UpdateFrame();
}

void	Cplayer::Render( void )
{
	//TCHAR	str_Text [256];	// ���ڿ� ����
	//ZeroMemory(str_Text,sizeof(str_Text)); //���ڿ� ���� �ʱ�ȭ
	////sprintf(������,���ķδ� printfó�� ���);
	//sprintf(str_Text,"�÷��̾� ���� : %f\n", m_vPos.y ); // �̷������� ���
	//TEXTMGR->Print(str_Text, NULL, 140); // �ؽ�Ʈ���
	//sprintf(str_Text,"�÷��̾� ������ : %f\n", CAMMGR->m_vPos.y ); // �̷������� ���
	//TEXTMGR->Print(str_Text, NULL, 160); // �ؽ�Ʈ���
	//sprintf(str_Text,"ī�޶� �� : %f\n", CAMMGR->m_vTarget_Y.y ); // �̷������� ���
	//TEXTMGR->Print(str_Text, NULL, 200); // �ؽ�Ʈ���

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
	if( m_vPos.y < 50.0f)	//	�ɸ��Ͱ� �� ������ ������ ���.
	{
		m_Status1.m_HP = 0;
	}

	if( m_Status1.m_HP < 1)
	{
		DIE();
	}

	//	�������ͽ��� DIE ������ ���
	if( m_Status1.m_Status == STATUS_DIE )
	{
		m_cPlayerController.SetRangeNoLoop(m_Status1.StartFrame,
			m_Status1.EndFrame,
			m_Status1.StartFrame,
			m_Status1.EndFrame,
			m_Status1.PlayTime );
		return;
	}

	//	�޺��� ������
	SetCombo();
	//	�̵�
	SetStatusMove();
	//	���
	SetActionStatus();
	//	����
	SetBuff();
}


//	�������� �̵�
void	Cplayer::SetStatusMove( void )
{

	//	�������ͽ��� DIE ������ ���
	if( m_Status1.m_Status == STATUS_DIE )
	{
		m_cPlayerController.SetRangeNoLoop(m_Status1.StartFrame,
			m_Status1.EndFrame,
			m_Status1.StartFrame,
			m_Status1.EndFrame,
			m_Status1.PlayTime );
		return ;
	}

	//	�������ͽ��� ���̰� �ƴϰų� ĵ���� �޽��� �ƴҰ��
	if( m_Status1.ActionCansle != false )
	{
		// �ڷ�
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


		// ������ ������
		if( KEYMGR->StayKeyDown('W') && KEYMGR->StayKeyDown(VK_SHIFT) && m_Status1.m_Stamina > 0.0f)
		{			
			RUN();
			Walk(m_Velocity * 2.0f);
		}
		// ������
		else if( KEYMGR->StayKeyDown('W') )
		{
			MOVE();
			Walk( m_Velocity );
		}
		// ī�޶� ���콺 ȸ��


		// ī�޶� ��������
		if( !KEYMGR->StayKeyDown('Q') && m_pos_MouseCursor.x < CENTERPOS_X - MOUSE_SENSE )
		{
			m_fangle -= 0.06f;
			yaw( -0.06f );
			CAMMGR->Yaw(-0.06f);
		}
		// ī�޶� ��������
		else if ( !KEYMGR->StayKeyDown('Q') && m_pos_MouseCursor.x > CENTERPOS_X + MOUSE_SENSE )
		{
			m_fangle += 0.06f;
			yaw( 0.06f );
			CAMMGR->Yaw(0.06f);
		}

		// ī�޶� ����
		if ( !KEYMGR->StayKeyDown('Q') && m_pos_MouseCursor.y < CENTERPOS_Y - MOUSE_SENSE )
		{
			if ( m_fcamangle > -0.16f )
			{
				m_fcamangle -= 0.02f;
				CAMMGR->Pitch( -0.02f );
			}
		}
		//ī�޶� �Ʒ���
		else if( !KEYMGR->StayKeyDown('Q') && m_pos_MouseCursor.y > CENTERPOS_Y + MOUSE_SENSE )
		{
			if ( m_fcamangle < 0.5f )
			{
				m_fcamangle += 0.02f;
				CAMMGR->Pitch( 0.02f );
			}
		}


		// ������ ��������
		if( KEYMGR->StayKeyDown('A') && KEYMGR->StayKeyDown(VK_SHIFT) && m_Status1.m_Stamina > 0.0f)
		{	
			RUN();
			strafe(( -m_Velocity/1.5f ) * 2.0f );
		}
		//	��������
		else if( KEYMGR->StayKeyDown('A') )
		{
			MOVE();
			strafe( -m_Velocity /1.5f );

		}


		// ������ ��������
		if( KEYMGR->StayKeyDown('D') && KEYMGR->StayKeyDown(VK_SHIFT) && m_Status1.m_Stamina > 0.0f)
		{			
			RUN();
			strafe(( m_Velocity/1.5f ) * 2.0f );
		}
		//	��������
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


//	Status ���� �ʱ�ȭ
void Cplayer::SetStatusInfo( void )
{
	m_Status1.m_Status		=	STATUS_STANCE;		//	���� �������ͽ�
	m_Status1.StartFrame	=	1;					//	ù��° ������
	m_Status1.EndFrame		=	11;					//	������ ������
	m_Status1.PlayTime		=	1.0f;				//	������ �Ѹ��� �ð� 1��
	m_Status1.Smotion		=	0.1f;				//	�������	0.05��
	//	0.05�� �ʹ� ���� 0.1�� ������
	m_Status1.ActionCansle	=	true;				//	�������ͽ� ĵ�� ����
	m_Status1.m_Dead		=	false;				//	�׾����� üũ
	if( m_Weapon == 1 )	//	�ּ�
	{
		m_Status1.m_fAttackSpeed	=	0.7f;			//	���� �ӵ�
		m_Weapon_Damage	= 50;							//	���� ������
		m_Weapon_Length	= 5.0f;							//	���� ��ġ
		m_SaveWeapon_Damege	=	m_Weapon_Damage;
		m_SaveSpeed	=	m_Status1.m_fAttackSpeed;
	}
	else				//	���
	{
		m_Status1.m_fAttackSpeed	=	0.85f;			//	���� �ӵ�
		m_Weapon_Damage = 75;							//	���� ������
		m_Weapon_Length = 7.0f;							//	���� ��ġ
		m_SaveWeapon_Damege	=	m_Weapon_Damage;
		m_SaveSpeed	=	m_Status1.m_fAttackSpeed;
	}

	m_Status1.m_HP			=	1000;			//	���� HP
	m_Status1.m_MaxHp		=	1000;			//  Ǯ HP
	m_Status1.m_Damage		=	30;				//	������
	m_Status1.m_Stamina		=	100;			//	Stamina
	m_Status1.m_MaxSt		=	100;			//  Ǯ ���¹̳�

	m_Status1.m_RealCombo	=	0;				//	���� �޺�

}


//	Set �׼� ����
void Cplayer::SetActionStatus( void )
{

	//	�������ͽ��� DIE ������ ���
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
			if( m_Status1.m_CheckSkill == true )	//	��ų�� ��� ���� ���� Ÿ�� �Ұ�� 
			{
				m_Status1.m_RealCombo	=	0;
			}
			m_HitCheck	=	false;

			m_NewTime	=	TIMEMGR->GetTotalTIme();
			m_Time	=	m_NewTime;				//	�޺��� �����ϸ� �ð� ����
			DOUBLESTAB();
			break;
		}
	case 22:
		{
			if( m_Status1.m_CheckSkill == true )	//	��ų�� ��� ���� ���� Ÿ�� �Ұ�� 
			{
				m_Status1.m_RealCombo	=	0;
			}
			if( m_Status1.m_RealCombo != 0 )		//	�޺��� 0�̶�� �ȳ���
			{
				m_HitCheck	=	false;
				m_NewTime	=	TIMEMGR->GetTotalTIme();
				m_Time	=	m_NewTime;				//	�޺��� �����ϸ� �ð� ����
				DOUBLECUT();
			}
			break;
		}
		////////////	DEFULT CUT	/////////////////
	case VK_LBUTTON:
		{
			if( m_Status1.m_CheckSkill == true )	//	��ų�� ��� ���� ���� Ÿ�� �Ұ�� 
			{
				m_Status1.m_RealCombo	=	0;
			}
			m_HitCheck	=	false;

			m_NewTime	=	TIMEMGR->GetTotalTIme();
			m_Time	=	m_NewTime;				//	�޺��� �����ϸ� �ð� ����
			VERTICAL();
			break;
		}
	case 'S':
		{
			if( m_Status1.m_CheckSkill == true )	//	��ų�� ��� ���� ���� Ÿ�� �Ұ�� 
			{
				m_Status1.m_RealCombo	=	0;
			}
			m_HitCheck	=	false;
			m_NewTime	=	TIMEMGR->GetTotalTIme();
			m_Time	=	m_NewTime;				//	�޺��� �����ϸ� �ð� ����
			JUMPCUT();
			break;
		}
	case 'A':
		{
			if( m_Status1.m_CheckSkill == true )	//	��ų�� ��� ���� ���� Ÿ�� �Ұ�� 
			{
				m_Status1.m_RealCombo	=	0;
			}
			m_HitCheck	=	false;
			m_NewTime	=	TIMEMGR->GetTotalTIme();
			m_Time	=	m_NewTime;				//	�޺��� �����ϸ� �ð� ����
			HORIZON_LEFT();
			break;
		}
	case 'D':
		{
			if( m_Status1.m_CheckSkill == true )	//	��ų�� ��� ���� ���� Ÿ�� �Ұ�� 
			{
				m_Status1.m_RealCombo	=	0;
			}
			m_HitCheck	=	false;
			m_NewTime	=	TIMEMGR->GetTotalTIme();
			m_Time	=	m_NewTime;				//	�޺��� �����ϸ� �ð� ����
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
		//	�뽬 �����϶��� ���� üũ�� �ϸ� �ȵ�
		if( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetPlay_Status().m_Status != STATUS_DIE 
			&& (MOBMGR->MobVec(MOBMGR->GetIter()))->GetPlay_Status().m_Status != STATUS_JUMP_DRAGON_BREATH )	//	���� ����
		{
			if( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetBeHit() == false && m_Status1.m_Status != STATUS_DASH && m_Status1.m_Status != STATUS_BEHIT )	//	�÷��̾� ����
			{
				if( CheckActionFrameCrash( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetMonsterPos() ) )		//	������ üũ
				{
					if( CheckPlayerActionCrash( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetMonsterPos() ) )		//	���� üũ
					{
						m_HitCheck		=	true;
						m_ActionCheck	=	true;						//	�׼� ���¸� üũ�Ͽ� ������ Ȯ���Ǹ� True
						m_HitTime	= TIMEMGR->GetTotalTIme();			//	���� ���� �ð�
						m_Status1.m_RealCombo++;						//	�޺� ���� 
						CheckSkill();									//	��ų�� �����Ѱ���
						(MOBMGR->MobVec(MOBMGR->GetIter()))->SetDamageCheck( m_Status1.m_Damage, true );	//	���Ϳ��� ������ ����
						(MOBMGR->MobVec(MOBMGR->GetIter()))->SetBeHitTime( TIMEMGR->GetTotalTIme() );
					}
				}
			}
		}
		MOBMGR->IterInc();
	}
}


//	����/������
void Cplayer::strafe	( float Velocity )
{
	//	������ ��ü�� xz ������� �������� �����Ѵ�.
	if( m_PlayerType == LANDOBJECT )
	{
		m_vPos	+=	D3DXVECTOR3( m_vRight.x, 0.0f, m_vRight.z ) * Velocity;
	}

	if( m_PlayerType == ATRCRAFT )
	{
		m_vPos += m_vRight * Velocity;
	}
}


//	����/�Ʒ���
void Cplayer::fly	( float Velocity )
{

}


//	����/����
void Cplayer::Walk	( float Velocity )
{
	if( Velocity == 0)	{	return;	}

	//	������ ��ü�� xz ������� �������� �����Ѵ�.
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


//	���� ���ͷ� ȸ��
void Cplayer::pitch	( float angle )
{
	D3DXMATRIX	T;
	D3DXMatrixRotationAxis(&T, &m_vRight, angle );

	//	m_vRight�� �������� m_vUp�� m_vLook�� ȸ���Ѵ�.
	D3DXVec3TransformCoord(&m_vUp, &m_vUp, &T );
	D3DXVec3TransformCoord(&m_vLook, &m_vLook, &T );
}


//	���� ���ͷ� ȸ��
void Cplayer::yaw	( float angle )
{
	D3DXMATRIX	T;

	//	������ ��ü�� ���ؼ��� ���� y (0, 1, 0)�� �������� ȸ���Ѵ�.
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


//	���� ���ͷ� ȸ��
void Cplayer::roll	( float angle )
{
	//	���� Ÿ���� ��쿡�� ȸ���Ѵ�.
	if( m_PlayerType	==	ATRCRAFT )
	{
		D3DXMATRIX	T;
		D3DXMatrixRotationAxis( &T, &m_vLook, angle );

		//	Look ���� �������� ���� ���������� ȸ���Ѵ�.
		D3DXVec3TransformCoord( &m_vRight, &m_vRight, &T );
		D3DXVec3TransformCoord( &m_vUp, &m_vUp, &T );
	}
}


//	Player Crash check
D3DXVECTOR3 Cplayer::SetPlayerCrash( D3DXVECTOR3 Monster_Pos )
{
	if( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetPlay_Status().m_Status == STATUS_DIE )	//	���Ͱ� ������ ������ �浹�� üũ���� ����
	{
		return m_vPos;
	}

	D3DXVECTOR3 new_vPos = m_vPos;						//	�÷��̾� ������ ��ġ���Ͱ��� ����
	D3DXVECTOR3 dir;									//	����ȭ �� ���� ����

	D3DXVec3Normalize( &dir, &(new_vPos - Monster_Pos) );		//	�÷��̾� ��ġ - ���� ��ġ�� ����ȭ �Ѱ��� dir����
	float fLen = D3DXVec3Length( &( new_vPos - Monster_Pos ) );	//	�÷��̾� ��ġ - ���� ��ġ ���� �� fLen ����

	if( fLen < 15.0f )									//	flen�� 15.0f���� ���ٸ�
	{
		float dist = 15.0f - fLen;						//	15.0f - flen �� dist�� ����
		new_vPos += dir * dist;							//	dir * dist�Ͽ� new_vPos ���� ���� ���Ͽ� ����
		return new_vPos;
	}
	return m_vPos;
}


//	�������ͽ� ���ٽ� ����
void	Cplayer::STANCE		(void)
{
	m_Status1.m_Status		=	STATUS_STANCE;
	m_Status1.StartFrame	=	1;
	m_Status1.EndFrame		=	10;
	m_Status1.PlayTime		=	0.5f;
	m_Status1.ActionCansle	=	true;
}


//	�������ͽ� ���� ����
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

//	�������ͽ� �⺻ ����
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


//	�������ͽ� �� ���� ����
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


//	�������ͽ� �� ���� ����
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


//	�������ͽ� ���� ����
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


//	�������ͽ� ����
void	Cplayer::DIE			(void)
{
	m_Status1.m_Status		=	STATUS_DIE;
	m_Status1.StartFrame	=	171;
	m_Status1.EndFrame		=	219;
	m_Status1.PlayTime		=	2.0f;
	m_Status1.ActionCansle	=	false;
}

//	�ι� ���
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

//	�ι� ����
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



//	�޺� �Է�
void	Cplayer::SetCombo( void )
{
	m_NewTime	=	TIMEMGR->GetTotalTIme();

	//	ù ���� ������ �ϰ��� 0.8�ʰ� ������ Ű�� ���� 0.5�� �������� �ٿ��� ��
	if( m_ComboBuffer[0] == -1 
		&& m_NewTime-m_Time > ( m_Status1.m_fAttackSpeed - 0.25f ) )		//	�޺� ����
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
			//printf("�ð� �Ϸ�\n");
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


//	�޺� ���
int	Cplayer::GetCombo	( void )
{
	m_NewTime	=	TIMEMGR->GetTotalTIme();
	if( m_ComboBuffer[1] )	//	�ι�° ����
	{
		int Action = 0;
		Action = m_ComboBuffer[ 1 ];
		m_ComboBuffer[ 1 ]	=	NULL;			//	�޺� Ű�� ���� �� ������ Ű�� �����ų ��� �̰� �ּ�ó��

		if( m_NewTime-m_Time > m_Status1.m_fAttackSpeed - 0.3f )
		{
			return Action;
		}
	}

	//	ù ��° ������ 1�� �ƴϰų�, ù ��° ������ 0�� �ƴϰų� �޺� �����ϴ� ���� �ƴ϶��
	if( m_ComboBuffer[ 0 ] != -1 && m_ComboBuffer[ 0 ] != NULL
		&& !m_ComboBuffer[1] )	//	ù��° ������ ������ ����
	{
		//printf("ù ��° ����  : %d\n", m_ComboBuffer[0] );
		m_Status1.m_RealCombo	=	0;
		int Action;
		Action = m_ComboBuffer[ 0 ];
		m_ComboBuffer[ 0 ] = -1;			//	ù��° ������ ���� �Ǹ� 1�� �ٲ�
		return Action;
	}
	return -1;
}


//	Ÿ�� ���� üũ
bool		Cplayer::CheckPlayerActionCrash( D3DXVECTOR3 Monster_Pos )
{
	D3DXVECTOR3  MonstertoPlayer, Now_vLook;
	MonstertoPlayer	=	( m_vPos - Monster_Pos );

	D3DXVec3Normalize( &Now_vLook, &m_vLook);		//	���� ���� ���� ����ȭ
	Now_vLook *= m_Weapon_Length;					//	���� ���� ���� ���ϱ� 5.0f Į ����
	Now_vLook += m_vPos;							//	���� ���� ���Ϳ� ��ġ ���� ����

	if( m_Status1.m_Status != STATUS_DOUBLE_CUT_BACKDASH )
	{
		m_ActionLen = D3DXVec3Length( &( Monster_Pos - Now_vLook ) );	//	vLook�� Į ���� ��ġ ���Ϳ� ���� ��ġ�� �� 
		if( m_ActionLen < 18.0f )
		{
			//printf("���� ����\n");
			return true;
		}

	}
	else if( m_Status1.m_Status == STATUS_DOUBLE_CUT_BACKDASH )
	{
		m_ActionLen = D3DXVec3Length( &( Monster_Pos - m_vPos ) );	//	vLook�� Į ���� ��ġ ���Ϳ� ���� ��ġ�� �� 
		if( m_ActionLen < 18.0f + m_Weapon_Length )
		{
			return true;
		}
	}
	return false;
}

//	üũ ������ 
bool		Cplayer::CheckActionFrameCrash( D3DXVECTOR3 Monster_Pos )
{
	D3DXVECTOR3  MonstertoPlayer, Now_vLook, MonstertoPlayer_dir, vCross;
	float	CrashFrame, NowFrame;
	MonstertoPlayer	=	( m_vPos - Monster_Pos );						//	���Ϳ��� �Ÿ�����

	D3DXVec3Normalize( &Now_vLook, &m_vLook );							//	���� �÷��̾� ���� ����
	D3DXVec3Normalize( &MonstertoPlayer_dir, &MonstertoPlayer );		//	���͸� ����Ű�� ����

	D3DXVec3Cross( &vCross, &MonstertoPlayer_dir, &Now_vLook );			//	����

	if( !(MOBMGR->MobVec(MOBMGR->GetIter()))->GetBeHit() && m_Status1.ActionCansle == false )				//	���� �ѹ��� üũ�� �ȵǾ����� ����
	{
		CrashFrame	=	m_Status1.EndFrame	-	m_Status1.StartFrame;		//	������ �����Ӱ� ��ŸƮ �������� �־� ����Ǵ� ������ ��
		CrashFrame	=	m_Status1.StartFrame	+	(CrashFrame / 2.0f);	//	�� ������ �ɰ��� ��ŸƮ ������ �� �����ϸ� �߰� �������� ����
		//CrashFrame -=   vCross.y * 8 ;										//	������ ������ŭ �������� ������Ű�ų� ����
		NowFrame	=	m_cPlayerController.GetCurFrame();					//	���� ������

		if( CrashFrame-1.0f < NowFrame &&  CrashFrame+1.0f  > NowFrame )
		{
			//	������ ���� �κп��� �������� ������ �ϰų� �� �����̿�����
			return true;								//	�� �ȵ�
		}
	}

	//	�� ����� �ȴ�...
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

//	�ǰ� ��
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
		//	�ǰ� ��� �ٷ� ��� 
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
	//	�ʱ�ȭ
	D3DFRAME->GetDevice()->SetTransform( D3DTS_WORLD,		&IdentityMatrix );
	D3DFRAME->GetDevice()->SetTransform( D3DTS_VIEW,		&D3DFRAME->m_mView );
	D3DFRAME->GetDevice()->SetTransform( D3DTS_PROJECTION,	&D3DFRAME->m_mProj );

	//	���� �� ����ȭ
	D3DFRAME->GetDevice()->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );

	//D3DFRAME->GetDevice()->SetRenderState ( D3DRS_ALPHATESTENABLE, FALSE );
	//D3DFRAME->GetDevice()->SetRenderState ( D3DRS_ALPHAREF, 0 );
	D3DFRAME->GetDevice()->SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	D3DFRAME->GetDevice()->SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	D3DFRAME->GetDevice()->SetRenderState ( D3DRS_LIGHTING, FALSE );
	D3DFRAME->GetDevice()->SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	//	��Ŷ�
	D3DFRAME->GetDevice()->SetFVF ( D3DFVF_XYZ|D3DFVF_DIFFUSE );
	D3DFRAME->GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 28, m_vTrail, sizeof(STRAIL_VERTEX) );
	//	��� ����
	D3DFRAME->GetDevice()->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CCW );
}

//	���� �Է�
void	Cplayer::SetBuff( void )
{
	//	m_NuffSwith
	//	1:	����
	//	2:	���׹̳� ����
	//	3:	�����̼� ����
	//	4:	������ ����

	CheckBuffTime();	//	���� Ÿ�� üũ


	if( m_BuffTimeCheck == true )
	{
		if( KEYMGR->OnceKeyDown( '1' ) )
		{
			SOUNDMGR->PlaySound(CHAR_BUFF1 );	//	����
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

	CheckBuffSwith();	//	���� ���� üũ
}

//	������ �ٲ� �� 10�� �� ����
void	Cplayer::CheckBuffTime( void )
{
	if( TIMEMGR->GetTotalTIme() - m_BuffTime > 10.0f )
	{
		m_BuffTimeCheck	=	true;
	}
}

//	���� ���¸� üũ�ϰ� ����
void	Cplayer::CheckBuffSwith( void )
{

	switch( m_BuffSwith )
	{
	case 1:	//	������
		{
			if( m_HitCheck == true && m_Vamp	== false )	//	������ �����ϰ� ���� ���� �ƴϸ�
			{
				m_Vamp	=	true;
				if( m_Status1.m_HP < m_Status1.m_MaxHp )
				{
					int vampHP	=	m_Status1.m_MaxHp - m_Status1.m_HP;	//	���� �ǿ� �ƽ� �� ��
					if( vampHP < (int)( m_Status1.m_Damage * 0.2f ) )	//	�ִ� ������ ������ �ǰ� ������15%���� ������
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
	case 2:	//	���׹̳� 100 
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
	case 3:	//	���� ����
		{
			if( !m_SpeedUp )
			{
				m_SpeedUp	=	true;
				m_SaveSpeed	=	m_Status1.m_fAttackSpeed;	//	���� ���� ����
				m_Status1.m_fAttackSpeed	=	m_Status1.m_fAttackSpeed - ( m_Status1.m_fAttackSpeed * 0.25f );
			}

			m_StaminaUp	=	false;
			m_DamegeUp	=	false;
			break;
		}
	case 4:	//	������ ����
		{
			if( !m_DamegeUp )
			{
				m_DamegeUp	=	true;
				m_SaveWeapon_Damege	=	m_Weapon_Damage;	//	���� ������ ����
				m_Weapon_Damage	=	m_Weapon_Damage + (m_Weapon_Damage);
			}

			m_StaminaUp	=	false;
			m_SpeedUp	=	false;
			break;
		}
	}

	if( !m_DamegeUp )	//	������ ���� false �϶� ���� �������� ����
	{
		m_Weapon_Damage	=	m_SaveWeapon_Damege;
	}

	if( !m_SpeedUp )	//	���ǵ� ���� false �϶� ���� ���ǵ�� ����
	{
		m_Status1.m_fAttackSpeed = m_SaveSpeed;
	}

	// ���¹̳ʰ� �۶� ���� ����
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