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

	SetStatusInfo();			//	�������ͽ� �ʱ�ȭ
	m_SkinRenderer.Init("./SHADER/alduin.BSM");					//	�޽�����
	CSkinRenderer::LoadEffect("./SHADER/skin_render.fx");		//	���̴� ����
	m_cMobAlduinController.Init( &m_SkinRenderer );					//	�޽� �ʱ�ȭ
	m_cMobAlduinController.SetScl( m_vScl );						//	������

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

	m_cMobAlduinController.SetPos( m_vPos );
	m_cMobAlduinController.SetRot(D3DXVECTOR3( 0.0f, m_fangle, 0.0f ) );

	int				nHead	= m_cMobAlduinController.GetBoneNoFromName( "BIP01 HEAD\0" );	//	�Ӹ�
	PBONE_INSTANCE	pIBone	= m_cMobAlduinController.GetIBoneInfo();						//	���� ����ü
	D3DXMATRIX		mBreath	= pIBone[nHead].dmSumRot;										//	�극��
	D3DXMATRIX		mWorld	= m_cMobAlduinController.GetWorldMatrix();						//	���� ���
	D3DXMATRIX		mRWorld = mWorld;
	mRWorld._14 = 0.0f;
	mRWorld._24 = 0.0f;
	mRWorld._34 = 0.0f;
	mRWorld._41 = 0.0f;
	mRWorld._42 = 0.0f;
	mRWorld._43 = 0.0f;

	//	���� ������ ���������
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

	//�� ���� �ٸ� ��ǥ��
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
	//TCHAR	str_Text[256];	// ���ڿ� ����
	//ZeroMemory(str_Text,sizeof(str_Text)); //���ڿ� ���� �ʱ�ȭ
	//sprintf(str_Text,"���� ���� : %d %d \n", m_ActionSwith, m_Monsterchoise ); // �̷������� ���
	//TEXTMGR->Print(str_Text, 0, 120); // �ؽ�Ʈ���
	//sprintf(str_Text,"���Ϳ��� �Ÿ� : %f\n", SetLength( CHARMGR->GetPos() ) ); // �̷������� ���
	//TEXTMGR->Print(str_Text, 260, 120); // �ؽ�Ʈ���
	//sprintf(str_Text,"���� ������ : %f\n", m_cMobAlduinController.GetCurFrame() ); // �̷������� ���
	//TEXTMGR->Print(str_Text, 500, 120); // �ؽ�Ʈ���


	m_cMobAlduinController.Render( m_BeHitCheck, NULL );

	pBreath1->render();
	pBreath2->render();
	pGrass->render();
}


//	���� ���ͷ� ȸ��
void CMobAlduin::yaw	( float angle )
{
	D3DXMATRIX	T;

	//	������ ��ü�� ���ؼ��� ���� y (0, 1, 0)�� �������� ȸ���Ѵ�.
	D3DXMatrixRotationY( &T, angle );

	//	
	D3DXVec3TransformCoord(&m_vRight, &m_vRight, &T);
	D3DXVec3TransformCoord(&m_vLook, &m_vLook, &T);
}

//	�������ͽ� ���� �ʱ�ȭ
void	CMobAlduin::SetStatusInfo( void )
{
	m_Status.m_Status		=	STATUS_STANCE;		//	���� �������ͽ�
	m_Status.StartFrame		=	41;					//	ù��° ������
	m_Status.EndFrame		=	55;					//	������ ������
	m_Status.PlayTime		=	2.0f;				//	������ �Ѹ��� �ð� 2��
	m_Status.Smotion		=	0.4f;				//	�������	0.05��
	m_Status.ActionCansle	=	true;				//	�������ͽ� ĵ�� ����
	m_Status.m_Dead			=	false;				//	�׾����� üũ
	m_Status.m_HP			=	10000;			//	HP
	m_Status.m_MaxHp		=	10000;			//  Ǯ HP
	m_Status.m_Damage		=	300.0f + ( D3DXVec3Length( &m_vScl ) );				//	������
	m_Status.m_Stamina		=	1000;			//	Stamina

	m_CheckHP	=	m_Status.m_HP;				//	üũ HP �ʱ�ȭ
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
		SetAction();	//	m_ActionSwith�� ������

		switch( m_ActionSwith )
		{
		case 1:		//	Ÿ����
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
		case 2:		//	�̵�
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
		case 3:		//	����
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
		case 4:		//	����
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
		case 5:		//	�뽬 ����
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

	//	���� üũ

	if( m_Status.ActionCansle == false )
	{
		if( SCENEMGR->GetNowScene() != MENU && CheckActionFrameCrash( CHARMGR->GetPos() ) )	//	�޴��� �ƴϰų� ������ üũ�� Ȯ���Ǹ�
		{
			if( CheckActionCrash( CHARMGR->GetPos() ) )	//	������ ���� ���� üũ
			{
				m_ActionCheck		=	true;
				CHARMGR->SetDamageCheck( m_Status.m_Damage );	//	������ ������ ��
				CHARMGR->CheckBEHIT();							//	������ �ƾ���
			}
		}
	}

	//	�ִϸ��̼�
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
	//	�������ͽ��� DIE ������ ���
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
	//	�������ͽ��� DIE ������ ���
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
		//	������ �Ÿ��� 25���̰� ������ �ִ� �����̰�
		)										//	���Ϳ� �÷��̾��� ������ ���̰� 0�� ������
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
			//	���� ����
		case 0:
			{
				m_ActionCheck	=	false;	//	���� ���� üũ�� �� ����
				m_CheckBREATH	=	false;
				VERTICAL();
				break;
			}
		case 1:
			{
				if( m_CheckHP - m_Status.m_HP > 800 )	//	���� �극��
				{
					m_CheckHP	=	m_Status.m_HP;
					m_ActionCheck	=	false;
					m_CheckBREATH	=	false;
					JUMP_DRAGON_BREATH();
					break;
				}
				else if( m_CheckHP - m_Status.m_HP > 300 )	//	�뽬
				{
					m_ActionCheck	=	false;
					DASH();
					break;
				}
				else if( SetLength(CHARMGR->GetPos() ) > 20 )	//	�극��
				{
					m_ActionCheck	=	false;
					m_CheckBREATH	=	false;
					BREATH();
					break;
				}
				break;
			}
			//	�߰Ÿ� ����
		case 2:	//	���� �극��
			{
				m_ActionCheck	=	false;
				m_CheckBREATH	=	false;
				LEFT_BREATH();
				break;
			}
		case 3:	//	���� �극��
			{
				m_ActionCheck	=	false;
				m_CheckBREATH	=	false;
				RIGHT_BREATH();
				break;
			}
			//	��Ÿ�
		case 4:	//	�극��
			{
				m_ActionCheck	=	false;
				m_CheckBREATH	=	false;
				BREATH();
				break;
			}
		case 5:	//	�뽬
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
	float fLen	=	D3DXVec3Length( &(Player_Pos - m_vPos ) );	//	�÷��̾�� ������ �Ÿ�
	return fLen;
}

//	�þ� üũ
void CMobAlduin::SetLook( D3DXVECTOR3 Player_Pos )
{
	//	�������ͽ��� DIE ������ ���
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



	m_vMonster_dir = ( Player_Pos - m_vPos );						//	���� �÷��̾ �ٶ󺸴� ����

	D3DXVec3Normalize( &m_vMonster_NewDir, &m_vMonster_dir );		//	�� ���� ����ȭ
	D3DXVec3Normalize( &m_vLook, &m_vLook);							//	���� ���� ����ȭ

	D3DXVec3Cross( &vCross, &m_vMonster_NewDir, &m_vLook );			//	���� �÷��̾� �ٶ󺸴� ����� ���� �������� ����
	m_fDot	=	D3DXVec3Dot( &m_vMonster_NewDir, &m_vLook );
	m_fLen = D3DXVec3Length( &vCross );


	if( m_Status.ActionCansle == true )
	{
		if( m_fDot < 0.99f )
		{
			m_fangle -= vCross.y*0.2f;		//	������ ������ y ��ŭ ȸ�� ��Ŵ
			yaw( -vCross.y*0.2f );
		}
	}
}

//	���� üũ
bool	CMobAlduin::CheckActionCrash( D3DXVECTOR3 Player_Pos )
{
	D3DXVECTOR3  MonstertoPlayer, Now_vLook;
	MonstertoPlayer	=	( Player_Pos - m_vPos );

	D3DXVec3Normalize( &Now_vLook, &m_vLook);		//	���� ���� ���� ����ȭ

	if( m_Status.m_Status == STATUS_VERTICAL )
	{
		Now_vLook *= 20.0f;								//	���� ���� ���� ���ϱ� 30.0f Į ����
		Now_vLook += m_vPos;							//	���� ���� ���Ϳ� ��ġ ���� �� ��
		float fLen = D3DXVec3Length( &( Player_Pos - Now_vLook ) );	//	vLook�� Į ���� ��ġ ���Ϳ� ���� ��ġ�� �� 
		if( fLen < 15.0f)			//	���� ���� 15
		{
			return true;
		}
	}
	else if( m_Status.m_Status == STATUS_DASH && m_bDashHit == true )
	{
		Now_vLook *= 10.0f;								//	���� ���� ���� ���ϱ� 30.0f Į ����
		Now_vLook += m_vPos;							//	���� ���� ���Ϳ� ��ġ ���� �� ��
		float fLen = D3DXVec3Length( &( Player_Pos - Now_vLook ) );	//	������ ��ġ����
		if( fLen < 20.0f )			//	���� ���� 28
		{
			return true;
		}
	}
	return false;
}

//	������ üũ
bool	CMobAlduin::CheckActionFrameCrash( D3DXVECTOR3 Player_Pos )
{
	D3DXVECTOR3  MonstertoPlayer, Now_vLook, MonstertoPlayer_dir, vCross;
	float	CrashFrame, NowFrame;

	MonstertoPlayer	=	( m_vPos - Player_Pos );					//	���Ϳ��� �Ÿ�����
	D3DXVec3Normalize( &Now_vLook, &m_vLook );						//	���� �÷��̾� ���� ����

	if( !m_ActionCheck )	//	���� �ѹ��� üũ�� �ȵǾ����� ����
	{
		if(m_Status.m_Status == STATUS_VERTICAL )
		{
			CrashFrame	=	m_Status.EndFrame	-	m_Status.StartFrame;		//	������ �����Ӱ� ��ŸƮ �������� �־� ����Ǵ� ������ ��
			CrashFrame	=	m_Status.StartFrame	+	(CrashFrame / 2.0f);	//	�� ������ �ɰ��� ��ŸƮ ������ �� �����ϸ� �߰� �������� ����
			NowFrame	=	m_cMobAlduinController.GetCurFrame();					//	���� ������

			if( CrashFrame - 1.5f < NowFrame && CrashFrame + 1.5f > NowFrame )
			{
				return true;
			}
		}//	�������ͽ� ���� üũ
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
	if( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetPlay_Status().m_Status == STATUS_DIE )	//	���Ͱ� ������ ������ �浹�� üũ���� ����
	{
		return m_vPos;
	}

	D3DXVECTOR3 new_vPos = m_vPos;						//	�÷��̾� ������ ��ġ���Ͱ��� ����
	D3DXVECTOR3 dir;									//	����ȭ �� ���� ����

	D3DXVec3Normalize( &dir, &(new_vPos - Monster_Pos) );		//	�÷��̾� ��ġ - ���� ��ġ�� ����ȭ �Ѱ��� dir����
	float fLen = D3DXVec3Length( &( new_vPos - Monster_Pos ) );	//	�÷��̾� ��ġ - ���� ��ġ ���� �� fLen ����

	if( m_Status.ActionCansle == true && fLen < 40.0f )									//	flen�� 15.0f���� ���ٸ�
	{
		float dist = 40.0f - fLen;						//	15.0f - flen �� dist�� ����
		new_vPos += dir * dist;							//	dir * dist�Ͽ� new_vPos ���� ���� ���Ͽ� ����
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

//	m_ActionSwith�� ������
void	CMobAlduin::SetAction( void )
{
	if( m_Status.m_Status == STATUS_DASH )	//	�뽬 ����
	{
		m_ActionSwith	=	5;
	}
	else if( SetLength( CHARMGR->GetPos() ) > 150.0f //	�÷��̾�� �Ÿ��� �־����ٸ�
		&& m_ActionSwith != 1					 // ���Ͱ� �þ߸� ���� �ʴ´ٸ�
		&& m_Status.m_Status != STATUS_DASH )	 //	�뽬 ���°� �ƴ϶��
	{
		m_ActionSwith	=	1;
		m_ActionChoiceCheck = false;
		m_bDashHit = false;
	}
	else if( TIMEMGR->GetTotalTIme() - m_ActionStartTime > 0.65f	//	0.65�� ���� 1,2,3,4 �����ϰ� ����
		&& m_Status.m_Status != STATUS_DASH )
	{
		m_ActionSwith	=	rand()%4+1;
		m_ActionChoiceCheck = false;
		m_bDashHit = false;
	}
}

//	���Ͱ� �극�� ��� ��
void	CMobAlduin::SetBREATHAction( void )
{

	if( !m_CheckBREATH )	//	���ο� �극���� ��� �� ����
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
				//���� ������ �����ӻ��̿��ٰ� ����־��
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

