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
	m_PlayerType	=	LANDOBJECT;					//	¶¥¿¡¼­ 
	m_Velocity		=	0.4f;						//	¼Óµµ
	m_Time			=	0.0f;						//	Ã¹ ÄÞº¸ ½ÃÀÛ½Ã°£
	m_NewTime		=	0.0f;						//	ÇöÀç ½Ã°£
	m_ActionLen		=	0.0f;						//	°ø°Ý °Å¸®
	m_Weapon		=	Weapon;						//	¹«±â °ª ¾òÀ½
	m_ComboBuffer[0]	=	NULL;					//	ÄÞº¸ ¹öÆÛ ÃÊ±âÈ­
	m_ComboBuffer[1]	=	NULL;
	m_BuffSwith		=	0;							//	¹öÇÁ »óÅÂ
	m_BuffTime		=	0.0f;						//	¹öÇÁ ½ÃÀÛÇÑ ½Ã°£
	m_BuffTimeCheck	=	true;						//	¹öÇÁ½Ã°£ÀÌ 10ÃÊ Áö³ª¸é true
	m_HitCheck		=	false;						//	°ø°ÝÀ» ¸Â­Ÿ´Â°¡ Ã¼Å©
	m_ActionCheck	=	false;						//	°ø°ÝÀ» Çß´Â°¡ Ã¼Å©

	m_DamegeUp	=	false;							//	µ¥¹ÌÁö ¾÷ ¹öÇÁ°¡ ½ÇÇàµÇ¸é true
	m_SpeedUp	=	false;							//	½ºÇÇµå ¾÷ ¹öÇÁ°¡ ½ÇÇàµÇ¸é true
	m_StaminaUp	=	false;							//	½ºÅ×¹Ì³Ê ¹öÇÁ°¡ ½ÇÇàµÇ¸é true


	SetStatusInfo();								//	½ºÅ×ÀÌÅÍ½º ÃÊ±âÈ­

	m_CheckHP	=	m_Status1.m_HP;
	m_vPos		=	D3DXVECTOR3( 0.0f, 114.0f, 0.0f );
	m_vRight	=	D3DXVECTOR3( -1.0f, 0.0f, 0.0f );
	m_vUp		=	D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	m_vLook		=	D3DXVECTOR3( 0.0f, 0.0f, -1.0f );

	m_fcamangle	=	0.0f;
	m_fangle	=	-2.86f;
	yaw( -2.86f );

	m_cSkinRenderer.Init("./SHADER/SD.hero1.BSM");						//	¸Þ½¬ÆÄÀÏ
	CSkinRenderer::LoadEffect("./SHADER/skin_render.fx");			//	¼ÎÀÌ´õ ÆÄÀÏ
	m_cPlayerController.Init( &m_cSkinRenderer );						//	¸Þ½¬ ÀÎÈý
	m_cPlayerController.SetPos( m_vPos );
	m_cPlayerController.SetScl( D3DXVECTOR3 ( 0.1f, 0.1f, 0.1f ) );	
}

void	Cplayer::Update( void )
{	
	//( m_Status1.m_HP<100 )? m_Status1.m_HP+=100 : m_Status1.m_HP; // ÇÇ °è¼Ó Âü µð¹ö±×¿ë
	GetCursorPos(&m_pos_MouseCursor);
	StatusMainSysstem( );

	MOBMGR->SetIterBengin();
	while( MOBMGR->GetIter() != MOBMGR->IterEnd() )
	{
		m_vPos		=	SetPlayerCrash( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetMonsterPos() );
		MOBMGR->IterInc();
	}
	m_vPos.y	=	MAPMGR->GetHeight(m_vPos.x,m_vPos.z,NULL,&m_vPosNormal);
	// ¹Ì²ô·¯Áü º¤ÅÍ ºÎºÐ
	if ( D3DXVec3Dot( &m_vPosNormal, &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) ) <= 0.85f ) // yÃà°ú ³»À§Ä¡ »ï°¢Çü ¹ý¼±À» ³»ÀûÇÏ¿© °¢À» ±¸ÇÔ 
	{
		// ¹Ì²ô·¯Áü º¤ÅÍ °è»ê
		m_vSlideVec	=	( -(D3DXVECTOR3(0.0f,1.0f,0.0f) - ( ( D3DXVec3Dot(&D3DXVECTOR3(0.0f,1.0f,0.0f),&m_vPosNormal) ) / ( D3DXVec3Dot(&m_vPosNormal ,&m_vPosNormal) ) ) * m_vPosNormal) );
	}
	else
	{
		m_vSlideVec	=	D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); // ³»Àû°ªÀÌ 0.85 ÀÌ»óÀÌ¸é ¹Ì²ô·¯Áü ¾øÀ½
	}
	m_vPos	+=	m_vSlideVec; // ¹Ì²ô·¯Áü º¤ÅÍ Àû¿ë

	m_cPlayerController.SetPos( m_vPos );
	m_cPlayerController.SetRot( D3DXVECTOR3(0.0f, m_fangle, 0.0f ) );

	
	CAMMGR->SetPlayerinfo(m_vPos, m_vRight, m_vUp, m_vLook);

	DashEffectUpdate();

	m_cPlayerController.UpdateFrame();
}

void	Cplayer::Render( void )
{
	//TCHAR	str_Text [256];	// ¹®ÀÚ¿­ º¯¼ö
	//ZeroMemory(str_Text,sizeof(str_Text)); //¹®ÀÚ¿­ º¯¼ö ÃÊ±âÈ­
	////sprintf(º¯¼ö¸í,ÀÌÈÄ·Î´Â printfÃ³·³ »ç¿ë);
	//sprintf(str_Text,"ÇÃ·¹ÀÌ¾î ³ôÀÌ : %f\n", m_vPos.y ); // ÀÌ·±½ÄÀ¸·Î »ç¿ë
	//TEXTMGR->Print(str_Text, NULL, 140); // ÅØ½ºÆ®Ãâ·Â
	//sprintf(str_Text,"ÇÃ·¹ÀÌ¾î ÇÁ·¹ÀÓ : %f\n", CAMMGR->m_vPos.y ); // ÀÌ·±½ÄÀ¸·Î »ç¿ë
	//TEXTMGR->Print(str_Text, NULL, 160); // ÅØ½ºÆ®Ãâ·Â
	//sprintf(str_Text,"Ä«¸Þ¶ó °¢ : %f\n", CAMMGR->m_vTarget_Y.y ); // ÀÌ·±½ÄÀ¸·Î »ç¿ë
	//TEXTMGR->Print(str_Text, NULL, 200); // ÅØ½ºÆ®Ãâ·Â

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
	if( m_vPos.y < 50.0f)	//	ÄÉ¸¯ÅÍ°¡ ¸Ê ¹ÛÀ¸·Î ³ª°¬À» °æ¿ì.
	{
		m_Status1.m_HP = 0;
	}

	if( m_Status1.m_HP < 1)
	{
		DIE();
	}

	//	½ºÅ×ÀÌÅÍ½º°¡ DIE »óÅÂÀÏ °æ¿ì
	if( m_Status1.m_Status == STATUS_DIE )
	{
		m_cPlayerController.SetRangeNoLoop(m_Status1.StartFrame,
			m_Status1.EndFrame,
			m_Status1.StartFrame,
			m_Status1.EndFrame,
			m_Status1.PlayTime );
		return;
	}

	//	ÄÞº¸¸¦ °¡Á®¿È
	SetCombo();
	//	ÀÌµ¿
	SetStatusMove();
	//	¸ð¼Ç
	SetActionStatus();
	//	¹öÇÁ
	SetBuff();
}


//	½ºÅ×ÀÌÅÍ ÀÌµ¿
void	Cplayer::SetStatusMove( void )
{

	//	½ºÅ×ÀÌÅÍ½º°¡ DIE »óÅÂÀÏ °æ¿ì
	if( m_Status1.m_Status == STATUS_DIE )
	{
		m_cPlayerController.SetRangeNoLoop(m_Status1.StartFrame,
			m_Status1.EndFrame,
			m_Status1.StartFrame,
			m_Status1.EndFrame,
			m_Status1.PlayTime );
		return ;
	}

	//	½ºÅ×ÀÌÅÍ½º°¡ ´ÙÀÌ°¡ ¾Æ´Ï°Å³ª Äµ½½ÀÌ ÆÞ½º°¡ ¾Æ´Ò°æ¿ì
	if( m_Status1.ActionCansle != false )
	{
		// µÚ·Î
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


		// ºü¸£°Ô ¾ÕÀ¸·Î
		if( KEYMGR->StayKeyDown('W') && KEYMGR->StayKeyDown(VK_SHIFT) && m_Status1.m_Stamina > 0.0f)
		{			
			RUN();
			Walk(m_Velocity * 2.0f);
		}
		// ¾ÕÀ¸·Î
		else if( KEYMGR->StayKeyDown('W') )
		{
			MOVE();
			Walk( m_Velocity );
		}
		// Ä«¸Þ¶ó ¸¶¿ì½º È¸Àü


		// Ä«¸Þ¶ó ÁÂÃøÀ¸·Î
		if( !KEYMGR->StayKeyDown('Q') && m_pos_MouseCursor.x < CENTERPOS_X - MOUSE_SENSE )
		{
			m_fangle -= 0.06f;
			yaw( -0.06f );
			CAMMGR->Yaw(-0.06f);
		}
		// Ä«¸Þ¶ó ¿ìÃøÀ¸·Î
		else if ( !KEYMGR->StayKeyDown('Q') && m_pos_MouseCursor.x > CENTERPOS_X + MOUSE_SENSE )
		{
			m_fangle += 0.06f;
			yaw( 0.06f );
			CAMMGR->Yaw(0.06f);
		}

		// Ä«¸Þ¶ó À§·Î
		if ( !KEYMGR->StayKeyDown('Q') && m_pos_MouseCursor.y < CENTERPOS_Y - MOUSE_SENSE )
		{
			if ( m_fcamangle > -0.16f )
			{
				m_fcamangle -= 0.02f;
				CAMMGR->Pitch( -0.02f );
			}
		}
		//Ä«¸Þ¶ó ¾Æ·¡·Î
		else if( !KEYMGR->StayKeyDown('Q') && m_pos_MouseCursor.y > CENTERPOS_Y + MOUSE_SENSE )
		{
			if ( m_fcamangle < 0.5f )
			{
				m_fcamangle += 0.02f;
				CAMMGR->Pitch( 0.02f );
			}
		}


		// ºü¸£°Ô ÁÂÃøÀ¸·Î
		if( KEYMGR->StayKeyDown('A') && KEYMGR->StayKeyDown(VK_SHIFT) && m_Status1.m_Stamina > 0.0f)
		{	
			RUN();
			strafe(( -m_Velocity/1.5f ) * 2.0f );
		}
		//	ÁÂÃøÀ¸·Î
		else if( KEYMGR->StayKeyDown('A') )
		{
			MOVE();
			strafe( -m_Velocity /1.5f );

		}


		// ºü¸£°Ô ¿ìÃøÀ¸·Î
		if( KEYMGR->StayKeyDown('D') && KEYMGR->StayKeyDown(VK_SHIFT) && m_Status1.m_Stamina > 0.0f)
		{			
			RUN();
			strafe(( m_Velocity/1.5f ) * 2.0f );
		}
		//	¿ìÃøÀ¸·Î
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


//	Status ÀÎÆ÷ ÃÊ±âÈ­
void Cplayer::SetStatusInfo( void )
{
	m_Status1.m_Status		=	STATUS_STANCE;		//	ÇöÀç ½ºÅ×ÀÌÅÍ½º
	m_Status1.StartFrame	=	1;					//	Ã¹¹øÂ° ÇÁ·¹ÀÓ
	m_Status1.EndFrame		=	11;					//	¸¶Áö¸· ÇÁ·¹ÀÓ
	m_Status1.PlayTime		=	1.0f;				//	ÇÁ·¹ÀÓ »Ñ¸®´Â ½Ã°£ 1ÃÊ
	m_Status1.Smotion		=	0.1f;				//	º¸°£¸ð¼Ç	0.05ÃÊ
	//	0.05°¡ ³Ê¹« »¡¶ó¼­ 0.1·Î ¼öÁ¤ÇÔ
	m_Status1.ActionCansle	=	true;				//	½ºÅ×ÀÌÅÍ½º Äµ½½ ¿©ºÎ
	m_Status1.m_Dead		=	false;				//	Á×¾ú´ÂÁö Ã¼Å©
	if( m_Weapon == 1 )	//	½Ö¼ö
	{
		m_Status1.m_fAttackSpeed	=	0.7f;			//	°ø°Ý ¼Óµµ
		m_Weapon_Damage	= 50;							//	¹«±â µ¥¹ÌÁö
		m_Weapon_Length	= 5.0f;							//	¹«±â ¸®Ä¡
		m_SaveWeapon_Damege	=	m_Weapon_Damage;
		m_SaveSpeed	=	m_Status1.m_fAttackSpeed;
	}
	else				//	¾ç¼Õ
	{
		m_Status1.m_fAttackSpeed	=	0.85f;			//	°ø°Ý ¼Óµµ
		m_Weapon_Damage = 75;							//	¹«±â µ¥¹ÌÁö
		m_Weapon_Length = 7.0f;							//	¹«±â ¸®Ä¡
		m_SaveWeapon_Damege	=	m_Weapon_Damage;
		m_SaveSpeed	=	m_Status1.m_fAttackSpeed;
	}

	m_Status1.m_HP			=	1000;			//	ÇöÀç HP
	m_Status1.m_MaxHp		=	1000;			//  Ç® HP
	m_Status1.m_Damage		=	30;				//	µ¥¹ÌÁö
	m_Status1.m_Stamina		=	100;			//	Stamina
	m_Status1.m_MaxSt		=	100;			//  Ç® ½ºÅÂ¹Ì³Ê

	m_Status1.m_RealCombo	=	0;				//	ÇöÀç ÄÞº¸

}


//	Set ¾×¼Ç º¯°æ
void Cplayer::SetActionStatus( void )
{

	//	½ºÅ×ÀÌÅÍ½º°¡ DIE »óÅÂÀÏ °æ¿ì
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
			if( m_Status1.m_CheckSkill == true )	//	½ºÅ³ÀÌ »ç¿ë µÈÈÄ ´ÙÀ½ Å¸°Ý ÇÒ°æ¿ì 
			{
				m_Status1.m_RealCombo	=	0;
			}
			m_HitCheck	=	false;

			m_NewTime	=	TIMEMGR->GetTotalTIme();
			m_Time	=	m_NewTime;				//	ÄÞº¸°¡ ¼º°øÇÏ¸é ½Ã°£ °»½Å
			DOUBLESTAB();
			break;
		}
	case 22:
		{
			if( m_Status1.m_CheckSkill == true )	//	½ºÅ³ÀÌ »ç¿ë µÈÈÄ ´ÙÀ½ Å¸°Ý ÇÒ°æ¿ì 
			{
				m_Status1.m_RealCombo	=	0;
			}
			if( m_Status1.m_RealCombo != 0 )		//	ÄÞº¸°¡ 0ÀÌ¶ó¸é ¾È³ª°¨
			{
				m_HitCheck	=	false;
				m_NewTime	=	TIMEMGR->GetTotalTIme();
				m_Time	=	m_NewTime;				//	ÄÞº¸°¡ ¼º°øÇÏ¸é ½Ã°£ °»½Å
				DOUBLECUT();
			}
			break;
		}
		////////////	DEFULT CUT	/////////////////
	case VK_LBUTTON:
		{
			if( m_Status1.m_CheckSkill == true )	//	½ºÅ³ÀÌ »ç¿ë µÈÈÄ ´ÙÀ½ Å¸°Ý ÇÒ°æ¿ì 
			{
				m_Status1.m_RealCombo	=	0;
			}
			m_HitCheck	=	false;

			m_NewTime	=	TIMEMGR->GetTotalTIme();
			m_Time	=	m_NewTime;				//	ÄÞº¸°¡ ¼º°øÇÏ¸é ½Ã°£ °»½Å
			VERTICAL();
			break;
		}
	case 'S':
		{
			if( m_Status1.m_CheckSkill == true )	//	½ºÅ³ÀÌ »ç¿ë µÈÈÄ ´ÙÀ½ Å¸°Ý ÇÒ°æ¿ì 
			{
				m_Status1.m_RealCombo	=	0;
			}
			m_HitCheck	=	false;
			m_NewTime	=	TIMEMGR->GetTotalTIme();
			m_Time	=	m_NewTime;				//	ÄÞº¸°¡ ¼º°øÇÏ¸é ½Ã°£ °»½Å
			JUMPCUT();
			break;
		}
	case 'A':
		{
			if( m_Status1.m_CheckSkill == true )	//	½ºÅ³ÀÌ »ç¿ë µÈÈÄ ´ÙÀ½ Å¸°Ý ÇÒ°æ¿ì 
			{
				m_Status1.m_RealCombo	=	0;
			}
			m_HitCheck	=	false;
			m_NewTime	=	TIMEMGR->GetTotalTIme();
			m_Time	=	m_NewTime;				//	ÄÞº¸°¡ ¼º°øÇÏ¸é ½Ã°£ °»½Å
			HORIZON_LEFT();
			break;
		}
	case 'D':
		{
			if( m_Status1.m_CheckSkill == true )	//	½ºÅ³ÀÌ »ç¿ë µÈÈÄ ´ÙÀ½ Å¸°Ý ÇÒ°æ¿ì 
			{
				m_Status1.m_RealCombo	=	0;
			}
			m_HitCheck	=	false;
			m_NewTime	=	TIMEMGR->GetTotalTIme();
			m_Time	=	m_NewTime;				//	ÄÞº¸°¡ ¼º°øÇÏ¸é ½Ã°£ °»½Å
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
		//	´ë½¬ »óÅÂÀÏ¶§´Â ÆÇÁ¤ Ã¼Å©¸¦ ÇÏ¸é ¾ÈµÊ
		if( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetPlay_Status().m_Status != STATUS_DIE 
			&& (MOBMGR->MobVec(MOBMGR->GetIter()))->GetPlay_Status().m_Status != STATUS_JUMP_DRAGON_BREATH )	//	¸ó½ºÅÍ »óÅÂ
		{
			if( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetBeHit() == false && m_Status1.m_Status != STATUS_DASH && m_Status1.m_Status != STATUS_BEHIT )	//	ÇÃ·¹ÀÌ¾î »óÅÂ
			{
				if( CheckActionFrameCrash( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetMonsterPos() ) )		//	ÇÁ·¹ÀÓ Ã¼Å©
				{
					if( CheckPlayerActionCrash( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetMonsterPos() ) )		//	ÆÇÁ¤ Ã¼Å©
					{
						m_HitCheck		=	true;
						m_ActionCheck	=	true;						//	¾×¼Ç »óÅÂ¸¦ Ã¼Å©ÇÏ¿© ÆÇÁ¤ÀÌ È®Á¤µÇ¸é True
						m_HitTime	= TIMEMGR->GetTotalTIme();			//	°ø°Ý ¼º°ø ½Ã°£
						m_Status1.m_RealCombo++;						//	ÄÞº¸ °¹¼ö 
						CheckSkill();									//	½ºÅ³·Î °ø°ÝÇÑ°ÇÁö
						(MOBMGR->MobVec(MOBMGR->GetIter()))->SetDamageCheck( m_Status1.m_Damage, true );	//	¸ó½ºÅÍ¿¡°Ô µ¥¹ÌÁö º¸³¿
						(MOBMGR->MobVec(MOBMGR->GetIter()))->SetBeHitTime( TIMEMGR->GetTotalTIme() );
					}
				}
			}
		}
		MOBMGR->IterInc();
	}
}


//	¿ÞÂÊ/¿À¸¥ÂÊ
void Cplayer::strafe	( float Velocity )
{
	//	Áö»óÀÇ ¹°Ã¼´Â xz Æò¸éÀ¸·Î ¿òÁ÷ÀÓÀ» Á¦ÇÑÇÑ´Ù.
	if( m_PlayerType == LANDOBJECT )
	{
		m_vPos	+=	D3DXVECTOR3( m_vRight.x, 0.0f, m_vRight.z ) * Velocity;
	}

	if( m_PlayerType == ATRCRAFT )
	{
		m_vPos += m_vRight * Velocity;
	}
}


//	À§ÂÊ/¾Æ·¡ÂÊ
void Cplayer::fly	( float Velocity )
{

}


//	¾ÕÂÊ/µÚÂÊ
void Cplayer::Walk	( float Velocity )
{
	if( Velocity == 0)	{	return;	}

	//	Áö»óÀÇ ¹°Ã¼´Â xz Æò¸éÀ¸·Î ¿òÁ÷ÀÓÀ» Á¦ÇÑÇÑ´Ù.
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


//	¿ìÇâ º¤ÅÍ·Î È¸Àü
void Cplayer::pitch	( float angle )
{
	D3DXMATRIX	T;
	D3DXMatrixRotationAxis(&T, &m_vRight, angle );

	//	m_vRight¸¦ ±âÁØÀ¸·Î m_vUp°ú m_vLookÀ» È¸ÀüÇÑ´Ù.
	D3DXVec3TransformCoord(&m_vUp, &m_vUp, &T );
	D3DXVec3TransformCoord(&m_vLook, &m_vLook, &T );
}


//	»óÇâ º¤ÅÍ·Î È¸Àü
void Cplayer::yaw	( float angle )
{
	D3DXMATRIX	T;

	//	Áö»óÀÇ ¹°Ã¼¿¡ ´ëÇØ¼­´Â ¿ùµå y (0, 1, 0)À» ±âÁØÀ¸·Î È¸ÀüÇÑ´Ù.
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


//	Àü¹æ º¤ÅÍ·Î È¸Àü
void Cplayer::roll	( float angle )
{
	//	ºñÇà Å¸ÀÔÀÏ °æ¿ì¿¡¸¸ È¸ÀüÇÑ´Ù.
	if( m_PlayerType	==	ATRCRAFT )
	{
		D3DXMATRIX	T;
		D3DXMatrixRotationAxis( &T, &m_vLook, angle );

		//	Look º¤ÅÍ ±âÁØÀ¸·Î À§¿Í ¿À¸¥ÂÊÀ¸·Î È¸ÀüÇÑ´Ù.
		D3DXVec3TransformCoord( &m_vRight, &m_vRight, &T );
		D3DXVec3TransformCoord( &m_vUp, &m_vUp, &T );
	}
}


//	Player Crash check
D3DXVECTOR3 Cplayer::SetPlayerCrash( D3DXVECTOR3 Monster_Pos )
{
	if( (MOBMGR->MobVec(MOBMGR->GetIter()))->GetPlay_Status().m_Status == STATUS_DIE )	//	¸ó½ºÅÍ°¡ Á×À¸¸é ¸ó½ºÅÍÀÇ Ãæµ¹À» Ã¼Å©ÇÏÁö ¾ÊÀ½
	{
		return m_vPos;
	}

	D3DXVECTOR3 new_vPos = m_vPos;						//	ÇÃ·¹ÀÌ¾î ÇöÀçÀÇ À§Ä¡º¤ÅÍ°ªÀ» ÀúÀå
	D3DXVECTOR3 dir;									//	Á¤±ÔÈ­ ÇÒ º¯¼ö ¼±¾ð

	D3DXVec3Normalize( &dir, &(new_vPos - Monster_Pos) );		//	ÇÃ·¹ÀÌ¾î À§Ä¡ - ¸ó½ºÅÍ À§Ä¡¸¦ Á¤±ÔÈ­ ÇÑ°ÍÀ» dirÀúÀå
	float fLen = D3DXVec3Length( &( new_vPos - Monster_Pos ) );	//	ÇÃ·¹ÀÌ¾î À§Ä¡ - ¸ó½ºÅÍ À§Ä¡ ³ª¿Â °ª fLen ÀúÀå

	if( fLen < 15.0f )									//	flenÀÌ 15.0fº¸´Ù Àû´Ù¸é
	{
		float dist = 15.0f - fLen;						//	15.0f - flen ¸¦ dist¿¡ ÀúÀå
		new_vPos += dir * dist;							//	dir * distÇÏ¿© new_vPos ¿ø·¡ °ª¿¡ ´õÇÏ¿© ¸®ÅÏ
		return new_vPos;
	}
	return m_vPos;
}


//	½ºÅ×ÀÌÅÍ½º ½ºÅÙ½º »óÅÂ
void	Cplayer::STANCE		(void)
{
	m_Status1.m_Status		=	STATUS_STANCE;
	m_Status1.StartFrame	=	1;
	m_Status1.EndFrame		=	10;
	m_Status1.PlayTime		=	0.5f;
	m_Status1.ActionCansle	=	true;
}


//	½ºÅ×ÀÌÅÍ½º ¹«ºê »óÅÂ
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

//	½ºÅ×ÀÌÅÍ½º ±âº» °ø°Ý
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


//	½ºÅ×ÀÌÅÍ½º ÁÂ º£±â °ø°Ý
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


//	½ºÅ×ÀÌÅÍ½º ¿ì º£±â °ø°Ý
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


//	½ºÅ×ÀÌÅÍ½º Á¡ÇÁ °ø°Ý
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


//	½ºÅ×ÀÌÅÍ½º Á×À½
void	Cplayer::DIE			(void)
{
	m_Status1.m_Status		=	STATUS_DIE;
	m_Status1.StartFrame	=	171;
	m_Status1.EndFrame		=	219;
	m_Status1.PlayTime		=	2.0f;
	m_Status1.ActionCansle	=	false;
}

//	µÎ¹ø Âî¸£±â
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

//	µÎ¹ø º£±â
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



//	ÄÞº¸ ÀÔ·Â
void	Cplayer::SetCombo( void )
{
	m_NewTime	=	TIMEMGR->GetTotalTIme();

	//	Ã¹ ¹ø±ú °ø°ÝÀ» ÇÏ°í³ª¼­ 0.8ÃÊ°¡ Áö³ª¸é Å°¸¦ ¹ÞÀ½ 0.5ÃÊ Á¤µµ±îÁö ÁÙ¿©µµ µÊ
	if( m_ComboBuffer[0] == -1 
		&& m_NewTime-m_Time > ( m_Status1.m_fAttackSpeed - 0.25f ) )		//	ÄÞº¸ °ø°Ý
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
			//printf("½Ã°£ ¿Ï·á\n");
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


//	ÄÞº¸ Ãâ·Â
int	Cplayer::GetCombo	( void )
{
	m_NewTime	=	TIMEMGR->GetTotalTIme();
	if( m_ComboBuffer[1] )	//	µÎ¹øÂ° °ø°Ý
	{
		int Action = 0;
		Action = m_ComboBuffer[ 1 ];
		m_ComboBuffer[ 1 ]	=	NULL;			//	ÄÞº¸ Å°¸¦ ´©¸¥ ÈÄ ¸¶Áö¸· Å°·Î ÀúÀå½ÃÅ³ °æ¿ì ÀÌ°É ÁÖ¼®Ã³¸®

		if( m_NewTime-m_Time > m_Status1.m_fAttackSpeed - 0.3f )
		{
			return Action;
		}
	}

	//	Ã¹ ¹øÂ° °ø°ÝÀÌ 1ÀÌ ¾Æ´Ï°Å³ª, Ã¹ ¹øÂ° °ø°ÝÀÌ 0ÀÌ ¾Æ´Ï°Å³ª ÄÞº¸ °ø°ÝÇÏ´Â ÁßÀÌ ¾Æ´Ï¶ó¸é
	if( m_ComboBuffer[ 0 ] != -1 && m_ComboBuffer[ 0 ] != NULL
		&& !m_ComboBuffer[1] )	//	Ã¹¹øÂ° °ø°ÝÀÌ ÀÖÀ¸¸é ½ÇÇà
	{
		//printf("Ã¹ ¹øÂ° °ø°Ý  : %d\n", m_ComboBuffer[0] );
		m_Status1.m_RealCombo	=	0;
		int Action;
		Action = m_ComboBuffer[ 0 ];
		m_ComboBuffer[ 0 ] = -1;			//	Ã¹¹øÂ° °ø°ÝÀÌ ½ÇÇà µÇ¸é 1·Î ¹Ù²ñ
		return Action;
	}
	return -1;
}


//	Å¸°Ý ¹üÀ§ Ã¼Å©
bool		Cplayer::CheckPlayerActionCrash( D3DXVECTOR3 Monster_Pos )
{
	D3DXVECTOR3  MonstertoPlayer, Now_vLook;
	MonstertoPlayer	=	( m_vPos - Monster_Pos );

	D3DXVec3Normalize( &Now_vLook, &m_vLook);		//	Àü¹æ ¹æÇâ º¤ÅÍ Á¤±ÔÈ­
	Now_vLook *= m_Weapon_Length;					//	Àü¹æ ¹æÇâ º¤ÅÍ °öÇÏ±â 5.0f Ä® ¹üÀ§
	Now_vLook += m_vPos;							//	Àü¹æ ¹æÇâ º¤ÅÍ¿¡ À§Ä¡ º¤ÅÍ ´õÇÔ

	if( m_Status1.m_Status != STATUS_DOUBLE_CUT_BACKDASH )
	{
		m_ActionLen = D3DXVec3Length( &( Monster_Pos - Now_vLook ) );	//	vLookÀº Ä® ¹üÀ§ À§Ä¡ º¤ÅÍ¿¡ ¸ó½ºÅÍ À§Ä¡»« °ª 
		if( m_ActionLen < 18.0f )
		{
			//printf("°ø°Ý ¼º°ø\n");
			return true;
		}

	}
	else if( m_Status1.m_Status == STATUS_DOUBLE_CUT_BACKDASH )
	{
		m_ActionLen = D3DXVec3Length( &( Monster_Pos - m_vPos ) );	//	vLookÀº Ä® ¹üÀ§ À§Ä¡ º¤ÅÍ¿¡ ¸ó½ºÅÍ À§Ä¡»« °ª 
		if( m_ActionLen < 18.0f + m_Weapon_Length )
		{
			return true;
		}
	}
	return false;
}

//	Ã¼Å© ÇÁ·¹ÀÓ 
bool		Cplayer::CheckActionFrameCrash( D3DXVECTOR3 Monster_Pos )
{
	D3DXVECTOR3  MonstertoPlayer, Now_vLook, MonstertoPlayer_dir, vCross;
	float	CrashFrame, NowFrame;
	MonstertoPlayer	=	( m_vPos - Monster_Pos );						//	¸ó½ºÅÍ¿ÍÀÇ °Å¸®º¤ÅÍ

	D3DXVec3Normalize( &Now_vLook, &m_vLook );							//	ÇöÀç ÇÃ·¹ÀÌ¾î Àü¹æ º¤ÅÍ
	D3DXVec3Normalize( &MonstertoPlayer_dir, &MonstertoPlayer );		//	¸ó½ºÅÍ¸¦ °¡¸®Å°´Â º¤ÅÍ

	D3DXVec3Cross( &vCross, &MonstertoPlayer_dir, &Now_vLook );			//	¿ÜÀû

	if( !(MOBMGR->MobVec(MOBMGR->GetIter()))->GetBeHit() && m_Status1.ActionCansle == false )				//	°ø°Ý ÇÑ¹ø´ç Ã¼Å©°¡ ¾ÈµÇ¾úÀ¸¸é ½ÇÇà
	{
		CrashFrame	=	m_Status1.EndFrame	-	m_Status1.StartFrame;		//	¸¶Áö¸· ÇÁ·¹ÀÓ°ú ½ºÅ¸Æ® ÇÁ·¹ÀÓÀ» ³Ö¾î ÁøÇàµÇ´Â ÇÁ·¹ÀÓ °ª
		CrashFrame	=	m_Status1.StartFrame	+	(CrashFrame / 2.0f);	//	À» ¹ÝÀ¸·Î ÂÉ°µÈÄ ½ºÅ¸Æ® ÇÁ·¹ÀÓ °ú µ¡¼ÀÇÏ¸é Áß°£ ÇÁ·¹ÀÓÀÌ ³ª¿È
		//CrashFrame -=   vCross.y * 8 ;										//	¿ÜÀûÀÇ °¢µµ¸¸Å­ ÇÁ·¹ÀÓÀ» Áõ°¡½ÃÅ°°Å³ª ³·Ãã
		NowFrame	=	m_cPlayerController.GetCurFrame();					//	ÇöÀç ÇÁ·¹ÀÓ

		if( CrashFrame-1.0f < NowFrame &&  CrashFrame+1.0f  > NowFrame )
		{
			//	¿ø·¡´Â ¿©±â ºÎºÐ¿¡¼­ ÇÁ·¹ÀÓÀ» ´À¸®°Ô ÇÏ°Å³ª ÇÒ »ý°¢ÀÌ¿´Áö¸¸
			return true;								//	Àß ¾ÈµÊ
		}
	}

	//	¾Æ µð¹ö±× ½È´Ù...
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

//	ÇÇ°Ý ½Ã
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
		//	ÇÇ°Ý ¸ð¼Ç ¹Ù·Î Ãâ·Â 
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
	//	ÃÊ±âÈ­
	D3DFRAME->GetDevice()->SetTransform( D3DTS_WORLD,		&IdentityMatrix );
	D3DFRAME->GetDevice()->SetTransform( D3DTS_VIEW,		&D3DFRAME->m_mView );
	D3DFRAME->GetDevice()->SetTransform( D3DTS_PROJECTION,	&D3DFRAME->m_mProj );

	//	¾ËÆÄ °ª Åõ¸íÈ­
	D3DFRAME->GetDevice()->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );

	//D3DFRAME->GetDevice()->SetRenderState ( D3DRS_ALPHATESTENABLE, FALSE );
	//D3DFRAME->GetDevice()->SetRenderState ( D3DRS_ALPHAREF, 0 );
	D3DFRAME->GetDevice()->SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	D3DFRAME->GetDevice()->SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	D3DFRAME->GetDevice()->SetRenderState ( D3DRS_LIGHTING, FALSE );
	D3DFRAME->GetDevice()->SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	D3DFRAME->GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	//	¿ä°Å¶û
	D3DFRAME->GetDevice()->SetFVF ( D3DFVF_XYZ|D3DFVF_DIFFUSE );
	D3DFRAME->GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 28, m_vTrail, sizeof(STRAIL_VERTEX) );
	//	¿ä±â ±îÁö
	D3DFRAME->GetDevice()->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CCW );
}

//	¹öÇÁ ÀÔ·Â
void	Cplayer::SetBuff( void )
{
	//	m_NuffSwith
	//	1:	ÈíÇ÷
	//	2:	½ºÅ×¹Ì³Ê ¹«ÇÑ
	//	3:	°ø¼ÓÀÌ¼Ó Áõ°¡
	//	4:	µ¥¹ÌÁö Áõ°¡

	CheckBuffTime();	//	¹öÇÁ Å¸ÀÓ Ã¼Å©


	if( m_BuffTimeCheck == true )
	{
		if( KEYMGR->OnceKeyDown( '1' ) )
		{
			SOUNDMGR->PlaySound(CHAR_BUFF1 );	//	»ç¿îµå
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

	CheckBuffSwith();	//	¹öÇÁ »óÅÂ Ã¼Å©
}

//	¹öÇÁ°¡ ¹Ù²ï ÈÄ 10ÃÊ ÈÄ ½ÇÇà
void	Cplayer::CheckBuffTime( void )
{
	if( TIMEMGR->GetTotalTIme() - m_BuffTime > 10.0f )
	{
		m_BuffTimeCheck	=	true;
	}
}

//	¹öÇÁ »óÅÂ¸¦ Ã¼Å©ÇÏ°í ½ÇÇà
void	Cplayer::CheckBuffSwith( void )
{

	switch( m_BuffSwith )
	{
	case 1:	//	ÈíÇ÷µÊ
		{
			if( m_HitCheck == true && m_Vamp	== false )	//	°ø°ÝÀÌ ¼º°øÇÏ°í ÈíÇ÷ ÁßÀÌ ¾Æ´Ï¸é
			{
				m_Vamp	=	true;
				if( m_Status1.m_HP < m_Status1.m_MaxHp )
				{
					int vampHP	=	m_Status1.m_MaxHp - m_Status1.m_HP;	//	ÇöÀç ÇÇ¿Í ¸Æ½º ÇÇ ºñ±³
					if( vampHP < (int)( m_Status1.m_Damage * 0.2f ) )	//	ÃÖ´ë ÈíÇ÷ÀÌ °¡´ÉÇÑ ÇÇ°¡ µ¥¹ÌÁö15%º¸´Ù ÀûÀ¸¸é
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
	case 2:	//	½ºÅ×¹Ì³Ê 100 
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
	case 3:	//	°ø¼Ó Áõ°¡
		{
			if( !m_SpeedUp )
			{
				m_SpeedUp	=	true;
				m_SaveSpeed	=	m_Status1.m_fAttackSpeed;	//	¿ø·¡ °ø¼Ó ÀúÀå
				m_Status1.m_fAttackSpeed	=	m_Status1.m_fAttackSpeed - ( m_Status1.m_fAttackSpeed * 0.25f );
			}

			m_StaminaUp	=	false;
			m_DamegeUp	=	false;
			break;
		}
	case 4:	//	µ¥¹ÌÁö Áõ°¡
		{
			if( !m_DamegeUp )
			{
				m_DamegeUp	=	true;
				m_SaveWeapon_Damege	=	m_Weapon_Damage;	//	¿ø·¡ µ¥¹ÌÁö ÀúÀå
				m_Weapon_Damage	=	m_Weapon_Damage + (m_Weapon_Damage);
			}

			m_StaminaUp	=	false;
			m_SpeedUp	=	false;
			break;
		}
	}

	if( !m_DamegeUp )	//	µ¥¹ÌÁö ¾÷ÀÌ false ÀÏ¶§ ¿ø·¡ µ¥¹ÌÁö·Î µ¹¸²
	{
		m_Weapon_Damage	=	m_SaveWeapon_Damege;
	}

	if( !m_SpeedUp )	//	½ºÇÇµå ¾÷ÀÌ false ÀÏ¶§ ¿ø·¡ ½ºÇÇµå·Î µ¹¸²
	{
		m_Status1.m_fAttackSpeed = m_SaveSpeed;
	}

	// ½ºÅÂ¹Ì³Ê°¡ ¶Û¶§ »©°í Âù´Ù
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